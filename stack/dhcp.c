/**********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher

    ---------------------------------------------------------------------
    File:      dhcp.c
    Project:   Single Chip Embedded Internet
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2017
    *********************************************************************
    10.06.2007 Quieten GNU compiler by initialising a variable           {1}
    22.06.2008 Allow operation without random number generator support   {2}
    14.10.2009 Allow overload options to be tolerated, but not handled   {3}
    22.03.2010 Verify the MAC address in offers to ensure accepting only own values {4}
    03.06.2010 Correct MAX_TIMER_DELAY value                             {5}
    04.06.2010 Check for invalid renewal and rebind timers from server and force realistic values {6}
    24.10.2010 Add relay agent support                                   {7}
    15.12.2012 DHCP is presently restricted to the default network and interface {8}
    27.03.2013 Add optional host name option                             {9}
    07.04.2013 Set IP address to 0.0.0.0 when forcing with FORCE_INIT    {10}
    20.05.2015 Extract first IP address for a DHCP option field (rather than last) {11}
    15.10.2015 Set default T1 and T2 values in case the DHCP server doesn't supply them {12}
    12.12.2015 Add network parameter to fnStartDHCP() and fnStopDHCP()   {13}
    12.12.2015 Extend to support multiple network                        {14}
    07.02.2015 Add DHCP server mode                                      {15}
    09.07.2015 Upgrade interrupt events to fnARP_report() contain network information {16}
    10.08.2015 Correct lease time seconds to byte conversion             {17}
    02.02.2017 Adapt for us tick resolution                              {18}

    See http://www.utasker.com/docs/uTasker/uTaskerDHCP.pdf for DHCP documentation
*/        

/*
   The 'xid' field is used by the client to match incoming DHCP messages
   with pending requests.  A DHCP client MUST choose 'xid's in such a
   way as to minimize the chance of using an 'xid' identical to one used
   by another client. For example, a client may choose a different,
   random initial 'xid' each time the client is rebooted, and
   subsequently use sequential 'xid's until the next reboot.  Selecting
   a new 'xid' for each retransmission is an implementation decision.  A
   client may choose to reuse the same 'xid' or select a new 'xid' for
   each retransmitted message.

   Read more: http://www.faqs.org/rfcs/rfc2131.html#ixzz0j8gXkSXK
*/

    
#include "config.h"


#if defined USE_DHCP_CLIENT || defined USE_DHCP_SERVER

#define ALLOW_OVERLOAD_OPTIONS                                           // {3}
#define VERIFY_TIMERS                                                    // {6}
#define SUPPORT_RELAY_AGENT                                              // {7}

/* =================================================================== */
/*                 local function prototype declarations               */
/* =================================================================== */

#if defined USE_DHCP_CLIENT
    static void          fnStateEventDHCP(unsigned char ucEvent, int iNetwork);
    static int           fnDHCPListener(USOCKET socket, unsigned char uc, unsigned char *ucIP, unsigned short us, unsigned char *data, unsigned short us2);
    static void          fnRandomise(DELAY_LIMIT DHCPTimeout, unsigned char ucTimerEvent, int iNetwork);
    static unsigned char fnStartDHCPTimer(unsigned char ucTimerEvent, int iNetwork);
    static void          fnSendDHCP(unsigned char ucDHCP_message, int iNetwork);
#endif
#if defined USE_DHCP_SERVER                                              // {15}
    static int           fnDHCPServerListener(USOCKET socket, unsigned char uc, unsigned char *ucIP, unsigned short us, unsigned char *data, unsigned short us2);
#endif


/* =================================================================== */
/*                          local definitions                          */
/* =================================================================== */


#define OWN_TASK                 TASK_DHCP


/*
 --------                               -------
|        | +-------------------------->|       |<-------------------+
| INIT-  | |     +-------------------->| INIT  |                    |
| REBOOT |DHCPNAK/         +---------->|       |<---+               |
|        |Restart|         |            -------     |               |
 --------  |  DHCPNAK/     |               |                        |
    |      Discard offer   |      -/Send DHCPDISCOVER               |
-/Send DHCPREQUEST         |               |                        |
    |      |     |      DHCPACK            v        |               |
 -----------     |   (not accept.)/   -----------   |               |
|           |    |  Send DHCPDECLINE |           |                  |
| REBOOTING |    |         |         | SELECTING |<----+            |
|           |    |        /          |           |     |DHCPOFFER/  |
 -----------     |       /            -----------   |  |Collect     |
    |            |      /                  |   |       |  replies   |
DHCPACK/         |     /  +----------------+   +-------+            |
Record lease, set|    |   v   Select offer/                         |
timers T1, T2   ------------  send DHCPREQUEST      |               |
    |   +----->|            |             DHCPNAK, Lease expired/   |
    |   |      | REQUESTING |                  Halt network         |
    DHCPOFFER/ |            |                       |               |
    Discard     ------------                        |               |
    |   |        |        |                   -----------           |
    |   +--------+     DHCPACK/              |           |          |
    |              Record lease, set    -----| REBINDING |          |
    |                timers T1, T2     /     |           |          |
    |                     |        DHCPACK/   -----------           |
    |                     v     Record lease, set   ^               |
    +----------------> -------      /timers T1,T2   |               |
               +----->|       |<---+                |               |
               |      | BOUND |<---+                |               |
  DHCPOFFER, DHCPACK, |       |    |            T2 expires/   DHCPNAK/
   DHCPNAK/Discard     -------     |             Broadcast  Halt network
               |       | |         |            DHCPREQUEST         |
               +-------+ |        DHCPACK/          |               |
                    T1 expires/   Record lease, set |               |
                 Send DHCPREQUEST timers T1, T2     |               |
                 to leasing server |                |               |
                         |   ----------             |               |
                         |  |          |------------+               |
                         +->| RENEWING |                            |
                            |          |----------------------------+
                             ----------

          Figure 5:  State-transition diagram for DHCP clients [RFC 2131]
*/

// Events
//
#define E_START_DHCP            1
#define E_DISCOVER_TIMEOUT      2
#define E_REQUEST_TIMEOUT       3
#define E_DHCP_COLLISION        4
#define E_DHCP_BIND_NOW         5
#define E_ACK_RECEIVED          6
#define E_REJECT_RECEIVED       7
#define E_RENEW_NOW             8
#define E_START_RENEWAL         9
#define E_START_REBIND          10
#define E_REBIND_NOW            11

#define E_NETWORK_TIMER_OFFSET  16                                       // the timer event will be offset by (iNetwork * E_NETWORK_TIMER_OFFSET) so that events can be easily associated with its network


// DHCP messages
//
#define DHCP_DISCOVER           1
#define DHCP_OFFER              2
#define DHCP_REQUEST            3
#define DHCP_DECLINE            4
#define DHCP_ACK                5
#define DHCP_NAK                6
#define DHCP_RELEASE            7
#define DHCP_INFORM             8

// DHCP options
//
#define DHCP_OPT_PAD            0
#define DHCP_OPT_SUBNET_MASK    1
#define DHCP_OPT_TIME_OFFSET    2
#define DHCP_OPT_ROUTER         3
#define DHCP_OPT_TIME_SERVER    4
#define DHCP_OPT_NAME_SERVER    5
#define DHCP_OPT_DNS_SERVER     6
#define DHCP_OPT_HOST_NAME      12
#define DHCP_OPT_REQUESTED_IP   50
#define DHCP_OPT_LEASE_TIME     51
#define DHCP_OPT_OVERLOAD       52
#define DHCP_OPT_MSG_TYPE       53
#define DHCP_OPT_SERV_IDENT     54
#define DHCP_OPT_PARAM_REQUEST  55
#define DHCP_OPT_MAX_MSG_SIZE   57
#define DHCP_OPT_T1_VALUE       58
#define DHCP_OPT_T2_VALUE       59
#define DHCP_OPT_POP3_SERVER    70
#define DHCP_OPT_END            255


#define BOOT_REQUEST            1
#define BOOT_REPLY              2


#define HW_ETHERNET_10M         1

#define BROADCAST_FLAG          0x8000

#define DHCL_OPTIONS            (BROADCAST_FLAG)

#define CHADD_LENGTH            16
#define SNAME_LENGTH            64
#define FILE_LENGTH             128

#define XID_LENGTH              4
#define XID_OFFSET              4

#define MAGIC_COOKIE_LENGTH     4

#define DHCP_BUFFER             (300)                                    // adequate for the largest DHCP message we send

#define T1_RESPONSE_RECEIVED    0x01
#define T2_RESPONSE_RECEIVED    0x02

#define LEASE_TIME_SECONDS      (60 * 60)                                // 1 hour lease time

/* =================================================================== */
/*                             constants                               */
/* =================================================================== */

#if defined USE_DHCP_CLIENT
static const unsigned char cDHCP_op_param_req[] = {
    DHCP_OPT_PARAM_REQUEST,
#if defined USE_DNS
    7,                                                                   // number of parameters being requested (inc. DNS)
#else
    6,                                                                   // number of parameters being requested (without DNS)
#endif
    DHCP_OPT_SUBNET_MASK, DHCP_OPT_ROUTER,
#if defined USE_DNS
    DHCP_OPT_DNS_SERVER,                                                 // only request DNS when we need it
#endif
    DHCP_OPT_HOST_NAME,
    DHCP_OPT_LEASE_TIME, DHCP_OPT_T1_VALUE, DHCP_OPT_T2_VALUE
};

static const unsigned char cRequestHeader[] = {
    BOOT_REQUEST,                                                        // operation is boot request
    HW_ETHERNET_10M,                                                     // htype = ethernet
    MAC_LENGTH,                                                          // hlen
    0x00,                                                                // hops is 0 for clients
    0x12, 0x34, 0x56, 0x78,                                              // XID (random number)
    0x00, 0x00,                                                          // seconds since our boot    
    (unsigned char)(DHCL_OPTIONS >> 8),                                  // options
    (unsigned char)(DHCL_OPTIONS)
};
#endif

static const unsigned char ucMagicCookie_and_message[] = {
    99, 130, 83, 99,                                                     // magic cookie
    DHCP_OPT_MSG_TYPE, 1                                                 // DHCP message and length
};

/* =================================================================== */
/*                 local function prototype declarations               */
/* =================================================================== */

#if defined USE_DHCP_CLIENT
    static void fnARP_report(int iNetwork, int iEvent);                  // {16}
#endif

/* =================================================================== */
/*                       local structure definitions                   */
/* =================================================================== */

typedef struct stUDP_DHCP_MESSAGE
{     
    UDP_HEADER       tUDP_Header;                                         // reserve header space
    unsigned char    ucUDP_Message[DHCP_BUFFER];
} UDP_DHCP_MESSAGE;


/* =================================================================== */
/*                     global variable definitions                     */
/* =================================================================== */

#if defined USE_DHCP_CLIENT
    static USOCKET       DHCPClientSocketNr = -1;                        // UDP client socket
    static USOCKET       DHCPClientSocketNetwork[IP_NETWORK_COUNT] = {
        -1,
    #if IP_NETWORK_COUNT > 1
        -1,
    #endif
    #if IP_NETWORK_COUNT > 2
        -1,
    #endif
    #if IP_NETWORK_COUNT > 3
        -1,
    #endif
    #if IP_NETWORK_COUNT > 4
        -1,
    #endif
    #if IP_NETWORK_COUNT > 5
        -1,
    #endif
    #if IP_NETWORK_COUNT > 6
        -1,
    #endif
    };
    static unsigned char ucDHCP_state[IP_NETWORK_COUNT] = {DHCP_INIT};   // DHCP not yet active
    static unsigned char ucDHCP_IP[IP_NETWORK_COUNT][IPV4_LENGTH] = {{0}}; // requested IP
    static unsigned char ucDHCP_SERVER_IP[IP_NETWORK_COUNT][IPV4_LENGTH] = {{0}}; // server's IP
    static DELAY_LIMIT   DHCPTimeout[IP_NETWORK_COUNT] = {0};

    static unsigned long ulLeaseTime[IP_NETWORK_COUNT] = {0};            // lease time variables
    static unsigned long ulRenewalTime[IP_NETWORK_COUNT] = {0};
    static unsigned long ulRebindingTime[IP_NETWORK_COUNT] = {0};
    static unsigned char ucServerResponses[IP_NETWORK_COUNT] = {0};      // {12}

    static unsigned char ucResendAfterArp[IP_NETWORK_COUNT] = {0};

    static UTASK_TASK    MasterTask[IP_NETWORK_COUNT] = {0};             // task which started the DHCP process
    #if IP_NETWORK_COUNT > 1
        static unsigned char ucLastTimer[IP_NETWORK_COUNT] = {0};
    #endif
    #if defined RANDOM_NUMBER_GENERATOR 
        static unsigned char ucXid[IP_NETWORK_COUNT][XID_LENGTH] = {{0}};// DHCP transation ID for checking negotiation match
    #endif
#endif

#if defined USE_DHCP_SERVER                                              // {15}
    static USOCKET DHCPServerSocketNr = -1;
    static USOCKET DHCPServerSocketNetwork[IP_NETWORK_COUNT] = {
        -1,
    #if IP_NETWORK_COUNT > 1
        -1,
    #endif
    #if IP_NETWORK_COUNT > 2
        -1,
    #endif
    #if IP_NETWORK_COUNT > 3
        -1,
    #endif
    #if IP_NETWORK_COUNT > 4
        -1,
    #endif
    #if IP_NETWORK_COUNT > 5
        -1,
    #endif
    #if IP_NETWORK_COUNT > 6
        -1,
    #endif
    };
#endif

#if defined USE_DHCP_CLIENT
// DHCP task
//
extern void fnDHCP(TTASKTABLE *ptrTaskTable)  
{
    QUEUE_HANDLE PortIDInternal = ptrTaskTable->TaskID;                  // queue ID for task input 
    unsigned char ucInputMessage[SMALL_QUEUE];                           // reserve space for receiving messages
    int iNetwork = 0;

    while (fnRead(PortIDInternal, ucInputMessage, HEADER_LENGTH) != 0) { // check input queue
        if (ucInputMessage[MSG_SOURCE_TASK] == TASK_ARP) {
            // Note that we receive ARP messages only on our attempt to send a test message to a node with our allocated IP address.
            // Since DHCP uses broadcast messages until this point there can be no ARP errors
            //
            fnRead(PortIDInternal, ucInputMessage, ucInputMessage[MSG_CONTENT_LENGTH]); // read the contents
            if (ARP_RESOLUTION_SUCCESS == ucInputMessage[0]) {
#if IP_NETWORK_COUNT > 1
                USOCKET socket;
                uMemcpy(&socket, &ucInputMessage[1], sizeof(socket));    // {50} extract the socket number from the received message
                iNetwork = extractNetwork(socket);
#endif
                if (ucResendAfterArp[iNetwork] != 0) {
                    fnSendDHCP(ucResendAfterArp[iNetwork], iNetwork);
                }
                else {
                    fnStateEventDHCP(E_DHCP_COLLISION, iNetwork);        // this is bad news; we have received IP data from DHCP server but found that someone is already using the address. We have to try again
                }
            }
            else if (ARP_RESOLUTION_FAILED == ucInputMessage[0]) {
                // We have probed to ensure than no one else has the IP address which we have received
                // the probing failed which means we can use it - inform application...
                //
#if IP_NETWORK_COUNT > 1
                USOCKET socket;
                uMemcpy(&socket, &ucInputMessage[1], sizeof(socket));    // {50} extract the socket number from the received message
                iNetwork = extractNetwork(socket);
#endif
                fnStateEventDHCP(E_DHCP_BIND_NOW, iNetwork);
            }
            ucResendAfterArp[iNetwork] = 0;
        }
        else {                                                           // assume a timer event
#if IP_NETWORK_COUNT > 1
            while (ucInputMessage[MSG_TIMER_EVENT] >= E_NETWORK_TIMER_OFFSET) { // extract the network number from the timer event
                iNetwork++;
                ucInputMessage[MSG_TIMER_EVENT] -= E_NETWORK_TIMER_OFFSET;
            }
            ucLastTimer[iNetwork] = 0;                                   // mark that the timer is no longer active
#endif
            fnStateEventDHCP(ucInputMessage[MSG_TIMER_EVENT], iNetwork); // timer event
        }
    }
}

// Call to stop DHCP client and return our preferred IP address
//
extern void fnStopDHCP(int iNetwork)                                     // {13}
{
#if IP_NETWORK_COUNT > 1
    if (ucLastTimer[iNetwork] != 0) {
        uTaskerGlobalStopTimer(OWN_TASK, ucLastTimer[iNetwork]);
    }
#else
    uTaskerStopTimer(OWN_TASK);                                          // stop all DHCP activity
#endif
    ucDHCP_state[iNetwork] = DHCP_INIT;
    uMemcpy(&network[iNetwork].ucOurIP[0], ucDHCP_IP, IPV4_LENGTH);      // return the preferred IP address to the global value so that we can continue if we want
#if defined _WINDOWS
    fnUpdateIPConfig();                                                  // update display in simulator
#endif
}

// Function allowing an option with IP address to be added to a DHCP message
//
static unsigned char *fnAddIPoption(unsigned char ucHDCP_option, unsigned char *ucIP, unsigned char *ucBuf)
{    
    *ucBuf++ = ucHDCP_option;                                            // DHCP message type
    *ucBuf++ = IPV4_LENGTH;                                              // length
    uMemcpy(ucBuf, ucIP, IPV4_LENGTH);                                   // add the IP address
    ucBuf += IPV4_LENGTH;
    return (ucBuf);
}

// Generate and send a DHCP message
//
static void fnSendDHCP(unsigned char ucDHCP_message, int iNetwork)
{
#if defined DHCP_HOST_NAME                                               // {9}
    CHAR *ptrOurHostName;
    unsigned char ucHostNameLength;
#endif
    UDP_DHCP_MESSAGE tUDP_Message;                                       // message space
    unsigned char *ucBuf = tUDP_Message.ucUDP_Message;                   // insert to message content
    uMemset(ucBuf, 0, DHCP_BUFFER);                                      // ensure message content is initially cleared
    uMemcpy(ucBuf, cRequestHeader, sizeof(cRequestHeader));              // build the message (with default XID)
#if defined RANDOM_NUMBER_GENERATOR 
    ucBuf += XID_OFFSET;
    ucXid[iNetwork][0] = (unsigned char)fnRandom();                      // randomise the XID
    ucXid[iNetwork][1] = (unsigned char)fnRandom();
    ucXid[iNetwork][2] = (unsigned char)fnRandom();
    ucXid[iNetwork][3] = (unsigned char)fnRandom();
    uMemcpy(ucBuf, ucXid[iNetwork], XID_LENGTH);                         // insert our XID
    ucBuf += (sizeof(cRequestHeader) - XID_LENGTH);
#else
    ucBuf += sizeof(cRequestHeader);
#endif
    
    if ((ucDHCP_state[iNetwork] & (DHCP_STATE_BOUND | DHCP_STATE_RENEWING | DHCP_STATE_REBINDING)) != 0) { // ciaddr is only sent when in BOUND, RENEW or REBINDING state
        uMemcpy(ucBuf, &network[iNetwork].ucOurIP[0], IPV4_LENGTH);      // insert our IP address
    }
    ucBuf += (4 * IPV4_LENGTH);                                          // leave yiaddr, siaddr and giaddr at zero
    
    uMemcpy(ucBuf, &network[iNetwork].ucOurMAC[0], MAC_LENGTH);          // chaddr
    ucBuf += CHADD_LENGTH + SNAME_LENGTH + FILE_LENGTH;                  // leave sname and file blank
    
    uMemcpy(ucBuf, ucMagicCookie_and_message, sizeof(ucMagicCookie_and_message));
    ucBuf += sizeof(ucMagicCookie_and_message);                          // options field. First magic cookie and DHCP message type
    *ucBuf++ = ucDHCP_message;                                           // the message type we are building
        
    switch (ucDHCP_message) {                                            // next options depend on what state we're in and message type
        case DHCP_REQUEST:
        case DHCP_DECLINE:                                               // sent only from REQUESTING state
            if ((ucDHCP_state[iNetwork] & (DHCP_STATE_RENEWING | DHCP_STATE_REBINDING)) == 0) { // not allowed in renewing or rebinding
                ucBuf = fnAddIPoption(DHCP_OPT_REQUESTED_IP, ucDHCP_IP[iNetwork], ucBuf);
            }    

            if ((ucDHCP_state[iNetwork] & (DHCP_STATE_INIT_REBOOT | DHCP_STATE_REBOOTING | DHCP_STATE_RENEWING | DHCP_STATE_REBINDING)) == 0) {
                ucBuf = fnAddIPoption(DHCP_OPT_SERV_IDENT, ucDHCP_SERVER_IP[iNetwork], ucBuf); // server identification
            }    
            if (ucDHCP_message == DHCP_DECLINE) {                        // decline doesn't send a parameter list
                 break;
            }
            // Fall through intentional
            //
        case DHCP_DISCOVER:        
            uMemcpy(ucBuf, cDHCP_op_param_req, sizeof(cDHCP_op_param_req)); // request parameter list (also for DHCP request)
            ucBuf += sizeof(cDHCP_op_param_req);
            break;            

        default:
            break;
    }
#if defined DHCP_HOST_NAME                                               // {9}
    ptrOurHostName = fnGetDHCP_host_name(&ucHostNameLength, iNetwork);   // allow the application to return a pointer to our host name string (and content length)
    if ((ptrOurHostName != 0) && (ucHostNameLength >= 1) && ((ucBuf + ucHostNameLength + 3) < &tUDP_Message.ucUDP_Message[DHCP_BUFFER])) { // if the string is of valid length and will fit in the buffer space
        *ucBuf++ = DHCP_OPT_HOST_NAME;                                   // host name option
        *ucBuf++ = ucHostNameLength;                                     // length of host name
        uMemcpy(ucBuf, ptrOurHostName, ucHostNameLength);                // the name string
        ucBuf += ucHostNameLength;
    }
#endif
    *ucBuf = DHCP_OPT_END;                                               // end option list
        
    if (ucDHCP_state[iNetwork] & (DHCP_STATE_BOUND | DHCP_STATE_RENEWING)) { // send unicast - message is fixed length with padding
        if (NO_ARP_ENTRY == fnSendUDP(DHCPClientSocketNetwork[iNetwork], ucDHCP_SERVER_IP[iNetwork], DHCP_SERVER_PORT, (unsigned char *)&tUDP_Message.tUDP_Header, DHCP_BUFFER, OWN_TASK)) {
            ucResendAfterArp[iNetwork] = ucDHCP_message;                 // we no longer have the address of the DHCP server in our ARP cache so we must repeat after the address has been resolved
        }
    }
    else {                                                               // or broadcast
        fnSendUDP(DHCPClientSocketNetwork[iNetwork], (unsigned char *)cucBroadcast, DHCP_SERVER_PORT, (unsigned char *)&tUDP_Message.tUDP_Header, DHCP_BUFFER, OWN_TASK);
    }
}

// Start an ARP enquiry to an IP address and wait for either ARP resolution success or failure
//
static void fnCheckIPAddress(int iNetwork)
{
#if IP_NETWORK_COUNT > 1
    if (ucLastTimer[iNetwork] != 0) {
        uTaskerGlobalStopTimer(OWN_TASK, ucLastTimer[iNetwork]);
    }
#else
    uTaskerStopTimer(OWN_TASK);                                          // stop timer until we receive the result from the ARP test
#endif
    uMemcpy(network[iNetwork].ucOurIP, ucDHCP_IP[iNetwork], IPV4_LENGTH);// temporarily accept IP address as own
    fnDeleteArp();                                                       // ensure our ARP table is empty
    fnGetIP_ARP(&network[iNetwork].ucOurIP[0], OWN_TASK, NetworkInterface(iNetwork, DEFAULT_IP_INTERFACE)); // {8} cause a ping of our own address and wait for the result
}

#if defined VERIFY_TIMERS
static void fnCheckTimerValidity(int iNetwork)                           // {12}
{
    if (((ucServerResponses[iNetwork] & T2_RESPONSE_RECEIVED) == 0) || (ulRebindingTime[iNetwork] == 0) || (ulLeaseTime[iNetwork] <= (ulRebindingTime[iNetwork] + 10))) { // invalid or unrealistic rebinding time
        ulRebindingTime[iNetwork] = ((ulLeaseTime[iNetwork]/4) * 3);     // set a rebinding time of 3/4 the lease time
    }                
    if (((ucServerResponses[iNetwork] & T1_RESPONSE_RECEIVED) == 0) || (ulRenewalTime[iNetwork] == 0) || (ulRenewalTime[iNetwork] >= (ulRebindingTime[iNetwork] - 10))) { // invalid or unrealistic renewal time
        ulRenewalTime[iNetwork] = ((ulRebindingTime[iNetwork]/3) * 2);   // set a renewal time of 2/3 the rebinding time (typically 1/2 the lease time)
    }
    ucServerResponses[iNetwork] = 0;                                     // reset for next use
}
#endif

// This function handles the DHCP state event machine
//
static void fnStateEventDHCP(unsigned char ucEvent, int iNetwork)
{
    switch (ucDHCP_state[iNetwork]) {
        case DHCP_STATE_INIT_REBOOT:                                     // try to obtain a preferred address which we already know
            ucDHCP_state[iNetwork] = DHCP_STATE_REBOOTING;               // move to rebooting state
            DHCPTimeout[iNetwork] = (DELAY_LIMIT)(4 * SEC);              // first timeout is 4s
            // Fall through intentional
            //
        case DHCP_STATE_REBOOTING:                                       // timeout while waiting for answer
            if (E_ACK_RECEIVED == ucEvent) {                             // ACK received, we first test the new IP address
                fnCheckIPAddress(iNetwork);                              // check whether the IP address is really free and bind if OK
                break;
            }
            // Fall through intentional
            //
        case DHCP_STATE_REQUESTING:
            if ((E_REJECT_RECEIVED == ucEvent) || (E_DHCP_COLLISION == ucEvent)) {
                if (E_DHCP_COLLISION == ucEvent) {
                    fnSendDHCP(DHCP_DECLINE, iNetwork);                  // we must send a decline to the server in this case
                    fnARP_report(iNetwork, DHCP_COLLISION);              // {16} inform the application of the event
                }
                ucDHCP_state[iNetwork] = DHCP_STATE_INIT;                // NACK received - reset
                uMemset(&network[iNetwork].ucOurIP[0], 0, IPV4_LENGTH);  // clear our known IP address and restart from INIT
                fnRandomise((11 * SEC), E_START_DHCP, iNetwork);         // start between 10..12s
                break;
            }
            else if (E_DHCP_BIND_NOW == ucEvent) {                       // we have checked that a newly received IP is really free so we use it
#if defined VERIFY_TIMERS                                                // {6}
                fnCheckTimerValidity(iNetwork);                          // {12}
#endif
                ucDHCP_state[iNetwork] = DHCP_STATE_BOUND;
                ulLeaseTime[iNetwork] -= ulRebindingTime[iNetwork];      // difference between lease expiracy and T2
                ulRebindingTime[iNetwork] -= ulRenewalTime[iNetwork];    // difference between T2 and T1
                fnStartDHCPTimer(E_START_RENEWAL, iNetwork);             // start the renewal timer (T1)
                fnARP_report(iNetwork, DHCP_SUCCESSFUL);                 // {16} we inform the master task that we have details to use the TCP/IP
#if defined _WINDOWS
                fnUpdateIPConfig();                                      // update display in simulator
#endif
                break;
            }
            fnSendDHCP(DHCP_REQUEST, iNetwork);                          // send the request message
            fnRandomise(DHCPTimeout[iNetwork], E_REQUEST_TIMEOUT, iNetwork);
            DHCPTimeout[iNetwork] *= 2;                                  // exponential backoff for timeout
            if (DHCPTimeout[iNetwork] > (DELAY_LIMIT)(64 * SEC)) {       // once the timeout gets too long we shorten and start again
                DHCPTimeout[iNetwork] = (DELAY_LIMIT)(4 * SEC);
                fnARP_report(iNetwork, DHCP_MISSING_SERVER);             // {16} inform the application that we are probably missing a server
            }
            break;

        case DHCP_STATE_INIT:                                            // try to obtain fresh parameters
            ucDHCP_state[iNetwork] = DHCP_STATE_SELECTING;               // move to selecting state
            DHCPTimeout[iNetwork] = (DELAY_LIMIT)(4 * SEC);              // first timeout is 4s
            // Fall through intentional
            //
        case DHCP_STATE_SELECTING:                                       // timeout while waiting for offer
            fnSendDHCP(DHCP_DISCOVER, iNetwork);                         // send the discover message
            fnRandomise(DHCPTimeout[iNetwork], E_DISCOVER_TIMEOUT, iNetwork);
            DHCPTimeout[iNetwork] *= 2;                                  // exponential backoff for timeout
            if (DHCPTimeout[iNetwork] > (DELAY_LIMIT)(64 * SEC)) {       // once the timeout gets too long we shorten and start again
                DHCPTimeout[iNetwork] = (DELAY_LIMIT)(4 * SEC);          // go back to minimum retransmission
                fnARP_report(iNetwork, DHCP_MISSING_SERVER);             // {16} inform the application that we are probably missing a server
            }
            break;

        case DHCP_STATE_BOUND:
            if (fnStartDHCPTimer(ucEvent, iNetwork) == 0) {
                break;                                                   // intermediate timer - don't handle yet
            }

            if (E_RENEW_NOW == ucEvent) {
                ucDHCP_state[iNetwork] = DHCP_STATE_RENEWING;            // T1 expired, start renewing process
                fnSendDHCP(DHCP_REQUEST, iNetwork);
                ulRebindingTime[iNetwork] /= 2;
                ulRenewalTime[iNetwork] = ulRebindingTime[iNetwork];     // the retransmission timer is set to half of the remaining
                // We don't check 60s minimum time here, only on subsequent repetitions
                //
                fnStartDHCPTimer(E_START_RENEWAL, iNetwork);             // use as retransmission timer
#if defined _WINDOWS
                fnUpdateIPConfig();                                      // update display in simulator
#endif
            }
            break;

        case DHCP_STATE_RENEWING:
            if (E_RENEW_NOW == ucEvent) {                                // timeout
                if (fnStartDHCPTimer(ucEvent, iNetwork) == 0) {
                    break;                                               // intermediate timer - don't handle yet
                }

                ulRebindingTime[iNetwork] /= 2;
                ulRenewalTime[iNetwork] = ulRebindingTime[iNetwork];     // the retransmission timer is set to half of the remaining
                if (ulRenewalTime[iNetwork] < 60) {                      // we are not allowed to use retransmission times less that 60s so give up
                    ucDHCP_state[iNetwork] = DHCP_STATE_REBINDING;       // we don't check 60s minimum time here, only on subsequent repetitions
                    ulLeaseTime[iNetwork] /= 2;                          // set timeout to half the remaining time to lease end
                    ulRenewalTime[iNetwork] = ulLeaseTime[iNetwork];
                    fnStartDHCPTimer(E_START_REBIND, iNetwork);          // use as retransmission timer
                }
                else {
                    fnStartDHCPTimer(E_START_RENEWAL, iNetwork);         // use as retransmission timer
                }
                fnSendDHCP(DHCP_REQUEST, iNetwork);
#if defined _WINDOWS
                fnUpdateIPConfig();                                      // update display in simulator
#endif
            }
            else if (E_ACK_RECEIVED == ucEvent) {                        // lease renewal successful
                ucDHCP_state[iNetwork] = DHCP_STATE_BOUND;               // go back to bound state
#if defined VERIFY_TIMERS
                fnCheckTimerValidity(iNetwork);                          // {12}
#endif
                ulLeaseTime[iNetwork] -= ulRebindingTime[iNetwork];      // difference between least expiracy and T2
                ulRebindingTime[iNetwork] -= ulRenewalTime[iNetwork];    // difference between T2 and T1
                fnStartDHCPTimer(E_START_RENEWAL, iNetwork);             // start the renewal timer (T1)
            }
            break;

        case DHCP_STATE_REBINDING:
            if (E_REBIND_NOW == ucEvent) {                               // timeout in rebinding
                if (fnStartDHCPTimer(ucEvent, iNetwork) == 0) {          // intermediate timer - don't handle yet
                    break;
                }

                ulLeaseTime[iNetwork] /= 2;                              // set timeout to half the remaining time to lease end
                ulRenewalTime[iNetwork] = ulLeaseTime[iNetwork];
                if (ulRenewalTime[iNetwork] < 60) {                      // we are not allowed to use retransmission times less that 60s so give up
                    ucDHCP_state[iNetwork] = DHCP_STATE_INIT;            // NACK received. reset
                    uMemset(&network[iNetwork].ucOurIP[0], 0, IPV4_LENGTH); // clear our known IP address and restart from INIT
                    fnRandomise((11 * SEC), E_START_DHCP, iNetwork);     // retry after 10..12s
                    fnARP_report(iNetwork, DHCP_LEASE_TERMINATED);       // {16}
                    break;
                }
                fnStartDHCPTimer(E_START_REBIND, iNetwork);              // use as retransmission timer
                fnSendDHCP(DHCP_REQUEST, iNetwork);
            }
            else if (E_ACK_RECEIVED == ucEvent) {
                ucDHCP_state[iNetwork] = DHCP_STATE_REQUESTING;          // go to this state and wait for result of ARP test before going to bound state or else restarting
                fnCheckIPAddress(iNetwork);                              // check whether the IP address is really free and bind if OK
            }
            break;
    }
}



// Read a DHCP option field and return value of last bytes (max 4, can be less)
//
static unsigned long fnReadDHCP_long(unsigned char **data)
{
    unsigned long ulRet = 0;
    unsigned char ucMsgLength = **data;
    unsigned char *ptr = (*data + 1);

    *data += ucMsgLength + 1;                                            // modify calling pointer

    while (ucMsgLength--) {                                              // collect data, resulting in the last 4 bytes as unsigned long
        ulRet <<= 8;
        ulRet |= *ptr++;
    }                                                                       
    return ulRet;
}

// Read a DHCP option field and return an IP address
//
static void fnReadDHCP_IP(unsigned char **data, unsigned char *ucIP)
{
    unsigned char ucMsgLength = **data;                                  // get length of the option message (length does not include option value or option length bytes)
    unsigned char *ptr = (*data + 1);                                    // {11} RFC 2132 indicates the I addresses should be returned in order of preference and the first one is therefore used rather than the last
  //unsigned char *ptr = (*data + ucMsgLength - IPV4_LENGTH + 1);        // previously returned the last address, which could be 0.0.0.0 from some commercial routers

    *data += (ucMsgLength + 1);                                          // modify calling pointer: point to next option value

    uMemcpy(ucIP, ptr, IPV4_LENGTH);                                     // copy the first/preferred IP address 
}


// Process a DHCP reception frame at the client
//
static int fnDoClientRxDHCP(unsigned char *data, unsigned char *usOfferedIP, unsigned short usLength, int iNetwork)
{
    int iOfferValid = 0;
    unsigned char ucMsgTyp = 0;                                          // {1}

    switch (ucDHCP_state[iNetwork]) {
        case DHCP_STATE_SELECTING:                                       // we are waiting for an offer from a DHCP server
            while (*data != DHCP_OPT_END) {                              // handle all received options
                switch (*data++) {                                       // this option
                    case DHCP_OPT_PAD:                                   // skip pads
                        break;

                    case DHCP_OPT_MSG_TYPE:
                        data++;
                        ucMsgTyp = *data++;                              // save message type
                        break;

                    case DHCP_OPT_SERV_IDENT:
                        data++;
                        uMemcpy(ucDHCP_SERVER_IP[iNetwork], data, IPV4_LENGTH); // read server identifier
                        data += IPV4_LENGTH;
                        iOfferValid = 1;                                 // mark that we have saved the offering server IP
                        break;

                    case DHCP_OPT_LEASE_TIME:                            // lease time received
                        ulLeaseTime[iNetwork] = fnReadDHCP_long(&data);  // get the lease time in seconds
                        break;

#if !defined ALLOW_OVERLOAD_OPTIONS                                      // {3}
                    case DHCP_OPT_OVERLOAD:
                        return NO_SUPPORT_FOR_DHCP_OVERLOAD;             // we don't process overload messages
#endif

                    case DHCP_OPT_SUBNET_MASK:
                        fnReadDHCP_IP(&data, &network[iNetwork].ucNetMask[0]); // update the network mask based on the offered option
                        break;

                    case DHCP_OPT_ROUTER:
                        fnReadDHCP_IP(&data, &network[iNetwork].ucDefGW[0]);
                        break;
#if defined USE_DNS
                    case DHCP_OPT_DNS_SERVER:
                        fnReadDHCP_IP(&data, &network[iNetwork].ucDNS_server[0]);
                        break;
#endif

                    default:                                             // non-supported options are skipped
                        fnReadDHCP_long(&data);
                        break;
                }
            }

            if ((iOfferValid == 0) || (ucMsgTyp != DHCP_OFFER)) {
                return NO_VALID_DHCP_MSG;                                // info not found - quit
            }

            ucDHCP_state[iNetwork] = DHCP_STATE_REQUESTING;              // move to requesting state
            uMemcpy(ucDHCP_IP[iNetwork], usOfferedIP, IPV4_LENGTH);      // save the offered IP
            
            fnSendDHCP(DHCP_REQUEST, iNetwork);                          // send the request
            DHCPTimeout[iNetwork] = (DELAY_LIMIT)(4 * SEC);              // first timeout is 4s
            fnRandomise(DHCPTimeout[iNetwork], E_REQUEST_TIMEOUT, iNetwork);
            break;

        case DHCP_STATE_REQUESTING:            
            while (*data != DHCP_OPT_END) {
                switch (*data++) {
                    case DHCP_OPT_SUBNET_MASK:
                        fnReadDHCP_IP(&data, &network[iNetwork].ucNetMask[0]);
                        break;

                    case DHCP_OPT_ROUTER:
                        fnReadDHCP_IP(&data, &network[iNetwork].ucDefGW[0]);
                        break;

                    case DHCP_OPT_SERV_IDENT:
                        data++;
                        uMemcpy(ucDHCP_SERVER_IP[iNetwork], data, IPV4_LENGTH); // read server identifier
                        data += IPV4_LENGTH;
                        iOfferValid = 1;
                        break;

                    case DHCP_OPT_LEASE_TIME:                            // lease time received
                        ulLeaseTime[iNetwork] = fnReadDHCP_long(&data);
                        break;

#if defined USE_DNS
                    case DHCP_OPT_DNS_SERVER:
                        fnReadDHCP_IP(&data, &network[iNetwork].ucDNS_server[0]);
                        break;
#endif

                    case DHCP_OPT_MSG_TYPE:
                        data++;
                        if (*data++ == DHCP_ACK){
                            // Probe to be sure that not already assigned to another local hardware - if the
                            // ARP check times out, we are really bound...
                            //
                            fnCheckIPAddress(iNetwork);
                        }
                        else {
                            return (INVALID_DHCP_PACKET);                // not DHCP packet...
                        }    
                        break;

                    case DHCP_OPT_T1_VALUE:                              // renewal time value in s
                        ulRenewalTime[iNetwork] = fnReadDHCP_long(&data);// value in s
                        ucServerResponses[iNetwork] |= T1_RESPONSE_RECEIVED; // {12}
                        break;

                    case DHCP_OPT_T2_VALUE:                              // rebinding time value
                        ulRebindingTime[iNetwork] = fnReadDHCP_long(&data); // value in s
                        ucServerResponses[iNetwork] |= T2_RESPONSE_RECEIVED; // {12}
                        break;

#if !defined ALLOW_OVERLOAD_OPTIONS                                      // {3}
                    case DHCP_OPT_OVERLOAD:
                        return NO_SUPPORT_FOR_DHCP_OVERLOAD;             // we don't process overload messages
#endif

                    case DHCP_OPT_PAD:                                   // skip pads
                        break;

                    case DHCP_OPT_HOST_NAME:                             // we don't save this
                    default:
                        fnReadDHCP_long(&data);
                        break;
                }
            }
            break;

        case DHCP_STATE_RENEWING:
        case DHCP_STATE_REBINDING:
        case DHCP_STATE_REBOOTING:
            while (*data != DHCP_OPT_END) {                              // process all options until end is encountered
                switch (*data++) {
                    case DHCP_OPT_PAD:                                   // pad so ignore
                        break;

                    case DHCP_OPT_LEASE_TIME:                            // lease time received 
                        ulLeaseTime[iNetwork] = fnReadDHCP_long(&data);  // get the IP address lease time in seconds
                        break;                                

                    case DHCP_OPT_MSG_TYPE:
                        data++;                                          // jump length which is always 1
                        if (*data == DHCP_NAK) {
                            fnStateEventDHCP(E_REJECT_RECEIVED, iNetwork); 
                            return (DHCP_REBOOT_REJECTED);
                        }

                        if (*data == DHCP_ACK) {
                            iOfferValid = 1;
                        }
                        data++;
                        break;

                    case DHCP_OPT_T1_VALUE:                              // renewal time value in s
                        ulRenewalTime[iNetwork] = fnReadDHCP_long(&data);// value in s
                        ucServerResponses[iNetwork] |= T1_RESPONSE_RECEIVED; // {12}
                        break;

                    case DHCP_OPT_T2_VALUE:                              // rebinding time value
                        ulRebindingTime[iNetwork] = fnReadDHCP_long(&data); // value in s
                        ucServerResponses[iNetwork] |= T2_RESPONSE_RECEIVED; // {12}
                        break;

                    default:                                             // unsupported, read the options but ignore them
                        fnReadDHCP_long(&data);
                        break;
                }
            }
            if (iOfferValid != 0) {
                fnStateEventDHCP(E_ACK_RECEIVED, iNetwork);
            }
            break;

        default:
            return NO_OPTIONS_ALLOWED_IN_DHCP_STATE;                     // don't process any options in other states
    }
    return 0;
}


// The UDP client function on DHCP client port
//
static int fnDHCPListener(USOCKET SocketNr, unsigned char ucEvent, unsigned char *ucIP, unsigned short usPortNr, unsigned char *data, unsigned short usLength)
{
    int iNetwork = extractNetwork(SocketNr);                             // the network that the reception was received on
    unsigned char usOfferedIP[IPV4_LENGTH];
#if defined SUPPORT_RELAY_AGENT
  //unsigned char usNextServerIP[IPV4_LENGTH];
    unsigned char usRelayAgentIP[IPV4_LENGTH];
#endif

    if (DHCPClientSocketNr != _UDP_SOCKET_MASK(SocketNr)) {
        return NOT_DHCP_SOCKET;                                          // not our socket so ignore
    }
    #if IP_NETWORK_COUNT > 0
    if (DHCPClientSocketNetwork[iNetwork] < 0) {
        return FOREIGN_DHCP_PACKET;                                      // no server active on this network
    }
    #endif
    
    switch (ucEvent) {
    case UDP_EVENT_PORT_UNREACHABLE:                                     // we have sent a UDP frame to a device with no socket open on the relevant port
        break;

    case UDP_EVENT_RXDATA:                                               // reception frame on our port
        if ((usLength < (236 - UDP_HLEN)) || (*data++ != BOOT_REPLY)) {
            return INVALID_DHCP_PACKET;                                  // bad length or not boot reply
        }
        data += 3;                                                       // skip htype, hlen, hops
      //iNetwork = 0;
#if defined RANDOM_NUMBER_GENERATOR
      /*while (iNetwork < IP_NETWORK_COUNT) {
            if (uMemcmp(data, ucXid[iNetwork], XID_LENGTH) == 0) {       // if a reception matched the XID
                uMemset(ucXid[iNetwork], 0, sizeof(ucXid[iNetwork]));    // reset to avoid a future match possibility
                break;
            }
            iNetwork++;
        }
        if (iNetwork >= IP_NETWORK_COUNT) {
            return FOREIGN_DHCP_PACKET;                                  // XID doesn't match any transmission
        }*/
        if (uMemcmp(data, ucXid[iNetwork], XID_LENGTH) != 0) {
            return FOREIGN_DHCP_PACKET;                                  // XID not as expected
        }
        uMemset(ucXid[iNetwork], 0, sizeof(ucXid[iNetwork]));            // reset to avoid a future match possibility
#else
        if (uMemcmp(data, &cRequestHeader[XID_OFFSET], XID_LENGTH) != 0) {
            return FOREIGN_DHCP_PACKET;                                  // XID not as expected
        }
#endif          
        data += (8 + XID_LENGTH);                                        // skip secs, flags, ciaddr
        uMemcpy(usOfferedIP, data, IPV4_LENGTH);                         // get offered address
#if defined SUPPORT_RELAY_AGENT
      //uMemcpy(usNextServerIP, (data + IPV4_LENGTH), IPV4_LENGTH);      // get next server IP address
        uMemcpy(usRelayAgentIP, (data + (2 * IPV4_LENGTH)), IPV4_LENGTH);// get relay agent IP address
#endif
        data += (IPV4_LENGTH * 3);                                       // move past yiaddr, skip siaddr, giaddr
        if (uMemcmp(data, &network[iNetwork].ucOurMAC[0], MAC_LENGTH) != 0) { // {4}
            return FOREIGN_DHCP_PACKET;                                  // chaddr not as expected (offer is to another client)
        }
        data += (16 + 64 + 128);                                         // skip chaddr, sname, file and get all info from the options field                
                                                                         // check magic cookie
        if (uMemcmp(data, ucMagicCookie_and_message, MAGIC_COOKIE_LENGTH) != 0) {
            return BAD_MAGIC_COOKIE;                                     // magic cookie is bad - quit
        }
        data += MAGIC_COOKIE_LENGTH;

        if (ucDHCP_state[iNetwork] == DHCP_STATE_REQUESTING) {           // we do this check here simply to avoid having to pass parameters
#if defined SUPPORT_RELAY_AGENT
            if ((uMemcmp(usOfferedIP, ucDHCP_IP[iNetwork], IPV4_LENGTH) != 0) || ((uMemcmp(ucDHCP_SERVER_IP[iNetwork], ucIP, IPV4_LENGTH) != 0) && (uMemcmp(usRelayAgentIP, ucIP, IPV4_LENGTH) != 0))) {
                return FOREIGN_DHCP_PACKET;                              // server or requested IP not same
            }
#else
            if ((uMemcmp(ucDHCP_SERVER_IP[iNetwork], ucIP, IPV4_LENGTH)) || (uMemcmp(usOfferedIP, ucDHCP_IP[iNetwork], IPV4_LENGTH))) {
                return FOREIGN_DHCP_PACKET;                          // server or requested IP not same
            }
#endif
        }
        // The initial checks were successful - now we do state specific stuff
        //
        return (fnDoClientRxDHCP(data, usOfferedIP, (unsigned short)(usLength + UDP_HLEN - (3 + 8 + XID_LENGTH + IPV4_LENGTH + 216)), iNetwork));
    }
    return 0;
}

// This routine handles the renewal timers. It is special because it allows the handling of possibly very long
// delays with limited maximum timer length
//
static unsigned char fnStartDHCPTimer(unsigned char ucTimerEvent, int iNetwork)
{
    DELAY_LIMIT ThisDelay;
    unsigned char ucNewEvent;
    #define MAX_TIMER_DELAY (((DELAY_LIMIT)0xffffffff)/(SEC)/*((TICK_RESOLUTION/1000)/1000)*/) // {5}

    switch (ucTimerEvent) {
    case E_START_RENEWAL:
        if (0xffffffff == ulRenewalTime[iNetwork]) {
            return 0;                                                    // if we have received an infinite lease time we do not start the timer
        }
        ucNewEvent = E_RENEW_NOW;
        break;

    case E_START_REBIND:
        ucNewEvent = E_REBIND_NOW;
        break;

    default:
        ucNewEvent = ucTimerEvent;
        break;
    }
    if (ulRenewalTime[iNetwork] == 0) {
        return ucTimerEvent;                                             // the timer has completely timed out
    }
    else if (ulRenewalTime[iNetwork] >= (MAX_TIMER_DELAY)) {             // note that the T1,T2 times are in seconds and we have to convert
        ThisDelay = (DELAY_LIMIT)0xffffffff;
        ulRenewalTime[iNetwork] -= (MAX_TIMER_DELAY);
    }
    else {
        ThisDelay = (DELAY_LIMIT)(((DELAY_LIMIT)ulRenewalTime[iNetwork]) * SEC);
        ulRenewalTime[iNetwork] = 0;
    }
#if IP_NETWORK_COUNT > 1
    if (ucLastTimer[iNetwork] != 0) {
        uTaskerGlobalStopTimer(OWN_TASK, ucLastTimer[iNetwork]);
    }
    ucTimerEvent += (iNetwork * E_NETWORK_TIMER_OFFSET);
    ucLastTimer[iNetwork] = ucTimerEvent;                                // remember the last timer event started so that we can stop it if necesary
    uTaskerGlobalMonoTimer(OWN_TASK, ThisDelay, ucNewEvent);
#else
    uTaskerMonoTimer(OWN_TASK, ThisDelay, ucNewEvent);
#endif
    return 0;
}


// This routine randomises the value to +/- 1 second of passed time value
//
static void fnRandomise(DELAY_LIMIT DHCPTimeout, unsigned char ucTimerEvent, int iNetwork)
{
#if defined RANDOM_NUMBER_GENERATOR                                      // {2}
    #if TICK_RESOLUTION >= 1000                                          // {18}
    DELAY_LIMIT random_sec = (fnRandom()/(0xffff/(4*(TICK_RESOLUTION/1000))));
    #else
    DELAY_LIMIT random_sec = (fnRandom()/(0xffff/(4/(1000/TICK_RESOLUTION))));
    #endif
    if (random_sec > (DELAY_LIMIT)(1 * SEC)) {
        DHCPTimeout -= ((DELAY_LIMIT)(2 * SEC) - random_sec);            // decrease of 0..1s
    }
    else {
        DHCPTimeout += random_sec;                                       // increase of 0..1s   
    }
#else                                                                    // simply change time +/- 0.5s each time
    static unsigned char random_sec = 0;
    if (random_sec != 0) {
        DHCPTimeout += (DELAY_LIMIT)(0.5 * SEC);
    }
    else {
        DHCPTimeout -= (DELAY_LIMIT)(0.5 * SEC);
    }
    random_sec ^= 1;
#endif
#if IP_NETWORK_COUNT > 1
    if (ucLastTimer[iNetwork] != 0) {
        uTaskerGlobalStopTimer(OWN_TASK, ucLastTimer[iNetwork]);
    }
    ucTimerEvent += (iNetwork * E_NETWORK_TIMER_OFFSET);
    ucLastTimer[iNetwork] = ucTimerEvent;                                // remember the last timer event started so that we can stop it if necesary
    uTaskerGlobalMonoTimer(OWN_TASK, DHCPTimeout, ucTimerEvent);
#else
    uTaskerMonoTimer(OWN_TASK, DHCPTimeout, ucTimerEvent);                               
#endif
}

// This function sends a message to the task which is managing the DHCP client
//
static void fnARP_report(int iNetwork, int iEvent)                       // {16}
{
    unsigned char int_message[HEADER_LENGTH + 2];
    int_message[MSG_DESTINATION_NODE]   = int_message[MSG_SOURCE_NODE] = INTERNAL_ROUTE;
    int_message[MSG_DESTINATION_TASK]   = MasterTask[iNetwork];
    int_message[MSG_SOURCE_TASK]        = TASK_ETHERNET;                 // general report from Ethernet
    int_message[MSG_CONTENT_LENGTH]     = 2;
    int_message[MSG_CONTENT_COMMAND]    = (unsigned char)iEvent;         // the event
    int_message[MSG_CONTENT_DATA_START] = (unsigned char)iNetwork;       // the network that the event belogs to
    fnWrite(INTERNAL_ROUTE, int_message, HEADER_LENGTH + 2);             // we send details to the DHCP manager
}

#if defined _WINDOWS
extern unsigned char fnGetDHCP_State(int iNetwork)
{
    return ucDHCP_state[iNetwork];
}
#endif
#endif


#if defined USE_DHCP_SERVER                                              // {15}

typedef struct stBOOTP_PACKET
{     
    unsigned char    op;                                                 // boot request or boot reply
    unsigned char    htype;                                              // hardware address type
    unsigned char    hlen;                                               // hardware address length
    unsigned char    hops;                                               // set to 0 by client
    unsigned char    xid[4];                                             // random transaction ID
    unsigned char    secs[2];                                            // seconds elapsed since client started trying to boot
    unsigned char    flags[2];
    unsigned char    ciaddr[4];                                          // client IPv4 address
    unsigned char    yiaddr[4];                                          // "your" IPv4 address
    unsigned char    siaddr[4];                                          // server IPv4 address
    unsigned char    giaddr[4];                                          // gateway IPv4 address
    unsigned char    chaddr[16];                                         // client hardware address
    unsigned char    sname[64];                                          // optional server host name
    unsigned char    file[128];                                          // optional boot file name
    unsigned char    vend[4];                                            // optional vendor-specific area (64 bytes in size) - starting with DHCP magic cookie
} BOOTP_PACKET;

typedef struct stDHCP_CLIENT_ENTRY
{     
    unsigned char    ucClientState;                                      // present state of the client
    unsigned char    clientMAC[MAC_LENGTH];
    unsigned char    clientIPv4[IPV4_LENGTH];
    unsigned long    ulClientTimeout;
} DHCP_CLIENT_ENTRY;

#define MAX_DHCP_CLIENTS 4
#define START_IP_RANGE   100                                             // the range of IP addresses that are to be offered
#define END_IP_RANGE     110

static DHCP_CLIENT_ENTRY dncp_clients[MAX_DHCP_CLIENTS] = {{0}};
static unsigned long ulAllocatedIP[IP_NETWORK_COUNT] = {0};

static int fnOfferAddress(unsigned char *ptrIPv4, int iNetwork)
{
    unsigned char ucOfferAddress[IPV4_LENGTH];
    int i = 0;
    unsigned long ulBit = 0x00000001;
    unsigned char ucAdd = START_IP_RANGE;
    while (i < IPV4_LENGTH) {
        ucOfferAddress[i] = (network[iNetwork].ucOurIP[i] & network[iNetwork].ucNetMask[i]);
        i++;
    }
    while ((ulAllocatedIP[iNetwork] & ulBit) != 0) {
        ucAdd++;
        ulBit <<= 1;
        if (ucAdd > END_IP_RANGE) {
            return -1;
        }
    }
    ucOfferAddress[IPV4_LENGTH - 1] |= ucAdd;                            // just change the lowest byte
    ulAllocatedIP[iNetwork] |= ulBit;                                    // mark that the address has been allocated
    uMemcpy(ptrIPv4, ucOfferAddress, IPV4_LENGTH);                       // the address to offer to the client
    return 0;
}

// Add an option to a transmitted frame
//
static unsigned char *fnAddOption(unsigned char *ptrData, unsigned char ucParameter, int iNetwork)
{
    *ptrData++ = ucParameter;
    switch (ucParameter) {
    case DHCP_OPT_ROUTER:
    case DHCP_OPT_DNS_SERVER:
    case DHCP_OPT_SERV_IDENT:
        *ptrData++ = IPV4_LENGTH;
        uMemcpy(ptrData, network[iNetwork].ucOurIP, IPV4_LENGTH);        // add our server IP address
        ptrData += IPV4_LENGTH;
        break;
    case DHCP_OPT_SUBNET_MASK:
        *ptrData++ = IPV4_LENGTH;
        uMemcpy(ptrData, network[iNetwork].ucNetMask, IPV4_LENGTH);      // add the sub-net mask
        ptrData += IPV4_LENGTH;
        break;
    case DHCP_OPT_LEASE_TIME:
        *ptrData++ = 4;                                                  // length of seconds field
        *ptrData++ = (unsigned char)(LEASE_TIME_SECONDS >> 24);          // {17} was << x
        *ptrData++ = (unsigned char)(LEASE_TIME_SECONDS >> 16);
        *ptrData++ = (unsigned char)(LEASE_TIME_SECONDS >> 8);
        *ptrData++ = (unsigned char)(LEASE_TIME_SECONDS);                // 60 seconds lease time
        break;
    default:                                                             // unsupported options are ignored
        return (ptrData - 1);
    }
    return ptrData;                                                      // return the new end of the buffer
}

static const unsigned char ptrMinPars[] = {4, DHCP_OPT_SUBNET_MASK, DHCP_OPT_SERV_IDENT, DHCP_OPT_LEASE_TIME, DHCP_OPT_SERV_IDENT}; // minium options offered

static unsigned char *fnAddBaseOption(unsigned char *ptrData, const unsigned char *ptr_ucParameter, int iNetwork)
{
    const unsigned char *ptrBaseOption = &ptrMinPars[0];
    unsigned char ucBaseOptionCount = ptrMinPars[0];
    const unsigned char *ptrPars;
    unsigned char ucParQuantity;
    while (ucBaseOptionCount-- != 0) {                                   // for each base option parameter
        ptrBaseOption++;
        ptrPars = ptr_ucParameter;                                       // start of the parameter list
        ucParQuantity = *ptrPars++;                                      // quantity in the parameter list
        while (ucParQuantity-- != 0) {                                   // for each parameter entry
            if (*ptrBaseOption == *ptrPars++) {                          // if the base parameter option has been requested we can skip inserting it
                goto _next_base_par;                                     // go to next
            }
        }
        ptrData = fnAddOption(ptrData, *ptrBaseOption, iNetwork);        // insert the missing option
_next_base_par:
        ;
    }
    return ptrData;
}


static void fnHandleClient(BOOTP_PACKET *ptrBootp, const unsigned char *ptrParameterList, const unsigned char *ptrRequestedIP, unsigned char ucMessageType, USOCKET SocketNr)
{
    int iNetwork = extractNetwork(SocketNr);                                 // the network that the reception was received on
    DHCP_CLIENT_ENTRY *ptrFreeEntry = 0;
    DHCP_CLIENT_ENTRY *ptrClientEntry = dncp_clients;
    unsigned char *ptrData = (unsigned char *)(ptrBootp + 1);
    int iClientReference = 0;

    while (iClientReference < MAX_DHCP_CLIENTS) {
        if (ptrClientEntry->ucClientState > 0) {
            if (uMemcmp(ptrBootp->chaddr, ptrClientEntry->clientMAC, MAC_LENGTH) == 0) { // if the MAC address matches
                break;                                                   // client matched
            }
        }
        else if (ptrFreeEntry == 0) {
            ptrFreeEntry = ptrClientEntry;                               // remember first free entry
        }
        ptrClientEntry++;
        iClientReference++;
    }
    if (iClientReference >= MAX_DHCP_CLIENTS) {                          // no entry exists
        if (ptrFreeEntry == 0) {
            return;                                                      // no free client space left
        }
        if (fnOfferAddress(ptrFreeEntry->clientIPv4, iNetwork) < 0) {    // allocate an IP address to offer to the client
            return;                                                      // if no IP addresses remain for allocation we ignore
        }
        ptrFreeEntry->ucClientState = 1;                                 // enter the new client
        uMemcpy(ptrFreeEntry->clientMAC, ptrBootp->chaddr, MAC_LENGTH);  // enter the client's MAC address
        ptrClientEntry = ptrFreeEntry;
    }
fnDebugMsg("DHCP [");
fnDebugDec(iNetwork, 0);
fnDebugMsg("] ");
    switch (ucMessageType) {
            /*
    #define DHCP_OFFER              2
    #define DHCP_DECLINE            4
    #define DHCP_ACK                5
    #define DHCP_NAK                6
    #define DHCP_RELEASE            7*/
    case DHCP_INFORM:
fnDebugMsg("Discover\r\n");
        break;
    case DHCP_REQUEST:                                                   // this client is requesting
    case DHCP_DISCOVER:                                                  // this client is discovering
        {
            unsigned char *ptrDestinationIP;
            unsigned char ucPars;
            unsigned char ucThisPar = 0;
            ptrClientEntry->ucClientState = 2;
            ptrBootp->op = BOOT_REPLY;
            ptrBootp->secs[0] = ptrBootp->secs[1] = 0;
// Temporarily always return a broadcast response
//ptrBootp->flags[0] = (unsigned char)(0x8000 >> 8);
//ptrBootp->flags[1] = (unsigned char)(0x8000);

            if ((ptrBootp->flags[0] & 0x80) != 0) {                      // the client requests a broadcast response
                ptrDestinationIP = (unsigned char *)cucBroadcast;        // respond to broadcast address as requested
            }
            else {
                ARP_DETAILS arp_details;
    #if IP_INTERFACE_COUNT > 1
                unsigned char ucInterface = extractInterface(SocketNr);
                int iInterfaceReference = 0;
                if (ucInterface != 0) {
                    while ((ucInterface & 0x01) == 0) {
                        ucInterface >>= 1;
                        iInterfaceReference++;
                    }
                }
                arp_details.Tx_handle = fnGetInterfaceHandle(iInterfaceReference);
    #endif
    #if IP_NETWORK_COUNT > 1
                arp_details.ucNetworkID = (unsigned char)iNetwork;
    #endif
                arp_details.ucType = ARP_TEMP_IP;
                ptrDestinationIP = ptrClientEntry->clientIPv4;           // respond to unicast address as requested
                fnAddARP(ptrDestinationIP, ptrClientEntry->clientMAC, &arp_details); // add the destination address to the ARP table so that the response can be sent
            }
            uMemcpy(ptrBootp->yiaddr, ptrClientEntry->clientIPv4, IPV4_LENGTH); // the address that is being offered
            uMemcpy(ptrBootp->siaddr, network[iNetwork].ucOurIP, IPV4_LENGTH); // our address (as server)
            if (ptrParameterList == 0) {                                 // if the client hasn't requested specific parameters
                ptrParameterList = ptrMinPars;                           // use a minium list
            }
            ucPars = ptrParameterList[0];                                // the number of parameters to add
            *ptrData++ = DHCP_OPT_MSG_TYPE;                              // start with offer message type
            *ptrData++ = 1;
            if (ucMessageType == DHCP_REQUEST) {
fnDebugMsg("Request ");
                if ((ptrRequestedIP != 0) && (uMemcmp(ptrRequestedIP, ptrClientEntry->clientIPv4, IPV4_LENGTH) == 0)) { // only accept if the IP address has been offered
                    *ptrData = DHCP_ACK;
                }
                else {
fnDebugMsg("\r\nptrRequestedIP = ");
fnDebugHex((unsigned long)ptrRequestedIP, (WITH_CR_LF | WITH_LEADIN | sizeof(unsigned long)));
#if defined USE_MAINTENANCE
if (ptrRequestedIP != 0) {
    fnDisplayIP((unsigned char *)ptrRequestedIP);
    fnDebugMsg(", ");
    fnDisplayIP(ptrClientEntry->clientIPv4);
}
#endif
    fnDebugMsg("  ");

                    *ptrData = DHCP_NAK;                                 // nack this request
fnDebugMsg("n");
                }
fnDebugMsg("acked\r\n");
            }
            else {
fnDebugMsg("Discover\r\n");
                *ptrData = DHCP_OFFER;
            }
            if (*ptrData++ != DHCP_NAK) {                                // if not nacking
                ptrData = fnAddBaseOption(ptrData, ptrParameterList, iNetwork); // in case necessary insert parameters that are not specifically requested
                while (ucThisPar++ < ucPars) {
                    ptrData = fnAddOption(ptrData, ptrParameterList[ucThisPar], iNetwork); // add parameters
                }
            }
            *ptrData++ = DHCP_OPT_END;                                   // terminate the option list
            fnSendUDP(DHCPServerSocketNetwork[iNetwork], ptrDestinationIP, DHCP_CLIENT_PORT, (((unsigned char *)ptrBootp) - sizeof(UDP_HEADER)), (ptrData - (unsigned char *)ptrBootp), 0); // return the response
        }
    }
}

static int fnDoServerRxDHCP(unsigned char *data, unsigned short usLength, USOCKET SocketNr)
{
    BOOTP_PACKET *ptrBootp = (BOOTP_PACKET *)data;
    if (usLength < sizeof(BOOTP_PACKET)) {                               // check for illegal paket size
        return 0;                   
    }
    if (ptrBootp->op != BOOT_REQUEST) {                                  // a client sends only boot requests
        return 0;
    }
    if ((ptrBootp->htype != HW_ETHERNET_10M) || (ptrBootp->hlen != MAC_LENGTH)) { // we only expect Ethernet types
        return 0;
    }
    if (uMemcmp(ptrBootp->vend, ucMagicCookie_and_message, 4) == 0) {    // check the DHCP magic cookie
        unsigned char *ptrRequestedIP = 0;
        unsigned char *ptrParameterList = 0;
        unsigned char ucOptionlLength;
        unsigned char ucThisOption;
        unsigned char ucMessageType = 0;
        data += sizeof(BOOTP_PACKET);                                    // move to the first DHCP option field
        usLength -= sizeof(BOOTP_PACKET);
        while (usLength != 0) {                                          // handle options
            ucThisOption = *data++;
            ucOptionlLength = *data++;
            switch (ucThisOption) {
            case DHCP_OPT_END:                                           // end of the option list
                usLength = 0;
                break;
            case DHCP_OPT_MSG_TYPE:
                ucMessageType = *data;
                break;
            case DHCP_OPT_REQUESTED_IP:                                  // IP address that the client is requesting
                ptrRequestedIP = data;
                break;
            case DHCP_OPT_PARAM_REQUEST:                                 // list of parameters that the client would like to receive
                ptrParameterList = (data - 1);                           // set pointer to the list of requested parameter option
                break;
            default:                                                     // ignore other options
                break;
            }
            data += ucOptionlLength;
            ucOptionlLength += 2;
            if (ucOptionlLength >= usLength) {
                break;
            }
            usLength -= ucOptionlLength;
        }
        fnHandleClient(ptrBootp, (const unsigned char *)ptrParameterList, (const unsigned char *)ptrRequestedIP, ucMessageType, SocketNr);
    }
    return 0;
}


// The UDP server function on DHCP server port
//
static int fnDHCPServerListener(USOCKET SocketNr, unsigned char ucEvent, unsigned char *ucIP, unsigned short usPortNr, unsigned char *data, unsigned short usLength)
{
    int iNetwork = extractNetwork(SocketNr);                             // the network that the reception was received on

    if (DHCPServerSocketNr != _UDP_SOCKET_MASK(SocketNr)) {
        return NOT_DHCP_SOCKET;                                          // not DHCP socket so ignore
    }
    #if IP_NETWORK_COUNT > 0
    if (DHCPServerSocketNetwork[iNetwork] < 0) {
        return FOREIGN_DHCP_PACKET;                                      // no server active on this network
    }
    #endif
    
    switch (ucEvent) {
    case UDP_EVENT_RXDATA:                                               // reception frame on our port
        return (fnDoServerRxDHCP(data, usLength, SocketNr));
    }
    return 0;
}
#endif

// Call this function to start DHCP client and/or server
//
extern int fnStartDHCP(UTASK_TASK Task, USOCKET uDetails)                // {13}
{
    int iNetwork = extractNetwork(uDetails);                             // network on which the DHCP client or server opertaes
    #if defined USE_DHCP_SERVER                                          // {15}
    if ((DHCP_SERVER_OPERATION & uDetails) != 0) {                       // start DHCP server operation
        if (DHCPServerSocketNr < 0) {                                    // bind the socket on first call
            DHCPServerSocketNr = fnGetUDP_socket(TOS_MINIMISE_DELAY, fnDHCPServerListener, (UDP_OPT_SEND_CS | UDP_OPT_CHECK_CS));
            if (DHCPServerSocketNr < 0) {
                return NO_UDP_SOCKET_FOR_DHCP_SERVER;
            }
            fnBindSocket(DHCPServerSocketNr, DHCP_SERVER_PORT);          // bind the listener to UDP port 67
        }
        DHCPServerSocketNetwork[iNetwork] = (DHCPServerSocketNr | (uDetails & ~(SOCKET_NUMBER_MASK))); // enter the network details to the socket used by the particuar network
        if ((DHCP_CLIENT_OPERATION & uDetails) == 0) {
            return 0;                                                    // only server mode so complete
        }
    }
    #endif
    #if defined USE_DHCP_CLIENT                                          // DHCP client
    if (DHCPClientSocketNr < 0) {                                        // bind the socket on first call
        DHCPClientSocketNr = fnGetUDP_socket(TOS_MINIMISE_DELAY, fnDHCPListener, (UDP_OPT_SEND_CS | UDP_OPT_CHECK_CS));
        if (DHCPClientSocketNr < 0) {
            return NO_UDP_SOCKET_FOR_DHCP;
        }
        fnBindSocket(DHCPClientSocketNr, DHCP_CLIENT_PORT);
    }
    DHCPClientSocketNetwork[iNetwork] = (DHCPClientSocketNr | (uDetails & ~(SOCKET_NUMBER_MASK))); // enter the network details to the socket used by the particuar network
    MasterTask[iNetwork] = (Task & ~FORCE_INIT);
    if (((Task & FORCE_INIT) == 0) && (uMemcmp(&network[iNetwork].ucOurIP[0], cucNullMACIP, IPV4_LENGTH) != 0)) { // if we have a non-zero IP address we will try to re-obtain it
        uMemcpy(ucDHCP_IP[iNetwork], &network[iNetwork].ucOurIP[0], IPV4_LENGTH); // copy our IP address to the DHCP preferred address
        //uMemset(&network[iNetwork].ucOurIP[0], 0, IPV4_LENGTH);        // remove the local IP since it may only be used after being validated {10}
        ucDHCP_state[iNetwork] = DHCP_STATE_INIT_REBOOT;                 // we already have a previous value - we will try to obtain it again
    }
    else {
        ucDHCP_state[iNetwork] = DHCP_STATE_INIT;                        // we have none so we must start fresh
    }
    uMemset(&network[iNetwork].ucOurIP[0], 0, IPV4_LENGTH);              // {10} remove the local IP since it may only be used after being validated (performed also when IP address is not 0.0.0.0 but initialisation is forced)
    fnRandomise((4 * SEC), E_START_DHCP, iNetwork);                      // perform DHCP state/event after short delay
    #endif
    return 0;                                                            // OK
}
#endif
