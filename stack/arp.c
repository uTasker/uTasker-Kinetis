/***********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher
    
    ---------------------------------------------------------------------
    File:      arp.c
    Project:   Single Chip Embedded Internet
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2016
    *********************************************************************
    14.03.2007 Added RARP response                                       {1}
    21.08.2007 Optionally only add used addresses to ARP table (rather than all 'seen' ones) {2}
    18.11.2009 Add permanent ARP entry option                            {3}
    05.02.2010 Add IPV6                                                  {4}
    05.02.2010 Modify return value fnGetARPentry() to zero when no entry found, instead of a pointer to cucNullMACIP {5}
    10.07.2011 Made fnSendARP_request() extenal for use by zero-configuration {6}
    11.07.2011 Allow zero-config to analyse ARP receptions for collision {7}
    25.03.2012 Use IPv6 link-local address for network neighbor protocol {8}
    14.05.2012 Change interface to fnGetARPentry()                       {9}
    27.05.2012 Moved ARP states to tcpip.h
    15.08.2012 Add SUPPORT_SUBNET_TX_BROADCAST                           {10}
    05.09.2012 Add own IP address to call to fnSubnetBroadcast()         {11}
    16.12.2012 Add SUPPORT_DYNAMIC_VLAN_TX                               {12}
    17.12.2012 Add ALTERNATIVE_VLAN_COUNT support                        {13}
    17.12.2012 New fnAddARP() interface                                  {14}
    17.12.2012 Add ARP_BROADCAST_ENTRY type to broadcast ARP entry       {15}
    04.06.2013 Added ARP_NN_MALLOC() default                             {16}
    20.11.2013 Avoid flushing arp entries that are presently resolving   {17}
    22.11.2013 Use subroutine when resolving ARP to share code between subnet and non-subnet paths {18}
    23.12.2013 Automatic gateway entry re-resolve on multiple interfaces {19}
    23.12.2013 Add gateway specific timeout value                        {20}
    30.12.2013 Set single interface flag on ARP resolution               {21}
    30.03.2014 Ensure ARP resolution is reported to owner when receiving an ARP request from the destination whilst resolving {22}
    13.04.2014 Add fnAssignNetwork() call to associate received frames to a network {23}
    05.06.2015 Modify the default behavior of re-reolves of the gatway address in multiple-network environments {24}

*/  
      
    
#include "config.h"


#if defined USE_IP


/* =================================================================== */
/*                          local definitions                          */
/* =================================================================== */

#define OWN_TASK                   TASK_ARP

#ifndef ARP_NN_MALLOC                                                    // {16}
    #define ARP_NN_MALLOC(x)       uMalloc((MAX_MALLOC)(x))
#endif

#define E_ARP_CHECK                1
#define E_ARP_PENDING_CHECK        2

#define T_ARP_PERIOD              (DELAY_LIMIT)(60 * SEC)                // manage ARP table once a minute
#define T_ARP_PENDING_PERIOD      (DELAY_LIMIT)(0.5 * SEC)               // check ARP resolution after short delay


#define ARP_TIMEOUT                20                                    // ARP entry lives for this many minutes
#define ARP_GATEWAY_TIMEOUT        5                                     // {20} ARP gateway entry lives for this many minutes
#define ARP_RESEND                 2                                     // on retry, lives for this many retry periods

#define ARP_MAXRETRY               4                                     // give up after this many attempts

#define ARP_REQUEST                0x0001                                // ARP Request to resolve address
#define ARP_REPLY                  0x0002                                // reply to resolve request
#define RARP_REQUEST               0x0003                                // reverse ARP request
#define RARP_REPLY                 0x0004                                // replay to RARP request


typedef struct stARP_INPUT
{
    ETHERNET_FRAME *frame;                                               // pointer to the received frame
    unsigned char  *ptrData;                                             // pointer to the frame data
#if defined USE_RARP
    unsigned char ucResponseType;                                        // the type of response that is required (SEND_ARP / SEND_RARP)
#endif
} ARP_INPUT;

#if defined USE_RARP
    #define SEND_ARP   0
    #define SEND_RARP  1
#endif

typedef struct stARP_OUTPUT
{
    unsigned char *ptrDestMAC;
    unsigned char *ptrReqMAC;
    unsigned char *ptrIP;
#if IP_INTERFACE_COUNT > 1
    QUEUE_HANDLE  Tx_handle;                                             // the handle of the interface that frames are to be sent over
#endif
#if IP_NETWORK_COUNT > 1
    unsigned char ucNetworkID;                                           // the network that the frame belongs to
#endif
#if defined SUPPORT_VLAN && defined SUPPORT_DYNAMIC_VLAN_TX
    unsigned char ucVLAN_content;                                        // {12} details about the VLAN characteristics of reception
#endif
} ARP_OUTPUT;


#if IP_NETWORK_COUNT > 1
    #define _NETWORK_ID        ucNetworkID
#else
    #define _NETWORK_ID        DEFAULT_NETWORK 
#endif
#if IP_INTERFACE_COUNT > 1
    #define _ETHERNET_HANDLE   Tx_handle
#else
    #define _ETHERNET_HANDLE   Ethernet_handle[0]                        // when there is only one network and one interface all output is sent to the ethernet handle
#endif

/* =================================================================== */
/*                             constants                               */
/* =================================================================== */

#ifdef USE_IP
static const unsigned char ucArpData[] = {
    (unsigned char)(ETHERNET_HARDWARE >> 8), (unsigned char)(ETHERNET_HARDWARE),
    (unsigned char)(PROTOCOL_IPv4 >> 8), (unsigned char)(PROTOCOL_IPv4), // protocol type                                                  
    MAC_LENGTH, 
    IPV4_LENGTH
};
#endif

/* =================================================================== */
/*                 local function prototype declarations               */
/* =================================================================== */

    static void     fnRefreshARP(void);
    static int      fnARP_check_pending(void);
    #ifdef USE_ZERO_CONFIG
    extern void     fnSendARP_request(ARP_TAB *ptrARPTab);
    #else
    static void     fnSendARP_request(ARP_TAB *ptrARPTab);
    #endif
    static void     fnSendARP_response(ARP_INPUT *ptrArpInput);
    static void     fnHandleARP_response(ARP_INPUT *ptrArpInput);
    static void     fnArpMessage(ARP_TAB *ptrArpEntry, unsigned char ucIntEvent);
#endif
#ifdef USE_IPV6
    static void fnNN_Message(NEIGHBOR_TAB *ptrNeighborTab, unsigned char ucIntEvent);
#endif

/* =================================================================== */
/*                          local variables                            */
/* =================================================================== */
#ifdef USE_IP
    static ARP_TAB *tARP = 0;                                            // ARP cache - size defined by ARP_TABLE_ENTRIES
#endif
#ifdef USE_IPV6
    static NEIGHBOR_TAB *tIPV6_Neighbors = 0;                            // {4} IPV6 neighbor cache - size defined by NEIGHBOR_TABLE_ENTRIES
#endif


#if defined USE_IP || defined USE_IPV6                                   // {4} ARP and IPV6 neighbor discovery share the task
// ARP task
//
extern void fnTaskArp(TTASKTABLE *ptrTaskTable)                          // ARP task
{
    QUEUE_HANDLE PortIDInternal = ptrTaskTable->TaskID;                  // queue ID for task input 
    unsigned char ucInputMessage[MEDIUM_MESSAGE];                        // reserve space for receiving messages 
    #if defined USE_IP
    if (tARP == 0) {                                                     // ARP table memory not yet allocated
        tARP = (ARP_TAB *)ARP_NN_MALLOC(sizeof(ARP_TAB) * ARP_TABLE_ENTRIES); // get zeroed ARP table space 
        tARP->ucState = (ARP_FIXED_IP | ARP_RESOLVED | ARP_BROADCAST_ENTRY); // {15} set fixed broadcast entry
        uMemcpy(tARP->ucIP,  cucBroadcast, IPV4_LENGTH);
        uMemcpy(tARP->ucMac, cucBroadcast, MAC_LENGTH);
        uTaskerMonoTimer(OWN_TASK, T_ARP_PERIOD, E_ARP_CHECK);           // timer for periodic update of ARP cache 
    }
    #endif
    #if defined USE_IPV6
    if (tIPV6_Neighbors == 0) {
        tIPV6_Neighbors = (NEIGHBOR_TAB *)ARP_NN_MALLOC(sizeof(NEIGHBOR_TAB) * NEIGHBOR_TABLE_ENTRIES); // get zeroed neighbor table space 
        #ifndef USE_IP
        uTaskerMonoTimer(OWN_TASK, T_ARP_PERIOD, E_ARP_CHECK);           // timer for periodic update of ARP cache 
        #endif
    }
    #endif

    while (fnRead(PortIDInternal, ucInputMessage, HEADER_LENGTH) != 0) { // check input queue
        switch (ucInputMessage[MSG_SOURCE_TASK]) {
        case TIMER_EVENT:
            if (ucInputMessage[MSG_TIMER_EVENT] == E_ARP_CHECK) {        // refresh the ARP/NN cache
                fnRefreshARP();                                          // refresh the ARP/NN table and restart next period    
            }
            else if (ucInputMessage[MSG_TIMER_EVENT] == E_ARP_PENDING_CHECK) { // an address is in the process of being resolved 
                if (fnARP_check_pending() != 0) {                        // check pending ARP entries                
                    uTaskerMonoTimer(OWN_TASK, T_ARP_PENDING_PERIOD, E_ARP_PENDING_CHECK); // start ARP resolution monitor timer
                }
                else {
                    uTaskerMonoTimer(OWN_TASK, T_ARP_PERIOD, E_ARP_CHECK); // no more pending entries, switch back to normal cache refresh timer
                }
            }
            break;
        }
    }
}


// This is called at a fixed rate to refresh the ARP table - entries expire if not used during a certain time
// Gateway timeouts are automatically refreshed, if found
//
static void fnRefreshARP(void)
{
    int i;
    int iFastPoll = 0;
    #if defined USE_IPV6
    NEIGHBOR_TAB *ptrNN = tIPV6_Neighbors;
    #endif
    #if defined USE_IP
    ARP_TAB *ptrARPTab = tARP;
    i = 0;
    while (++i < ARP_TABLE_ENTRIES) {                                    // note that we jump the broadcast entry which is always the first in the table - it never times out...
        ptrARPTab++;
        if ((ptrARPTab->ucState & ARP_RESOLVED) != 0) {                  // resolved entry
            if (ptrARPTab->ucTimeToLive != 0) {                          // not timeout out
                if (--(ptrARPTab->ucTimeToLive) == 0) {                  // if an entry has timed out
                    if ((ptrARPTab->ucState & ARP_FIXED_IP) == 0) {      // ordinary address timeout    
                        ptrARPTab->ucState = ARP_FREE;                   // release entry 
                    } 
                    else {                                               // gateway timeout - repeat request automatically
                        if (ptrARPTab->ucRetries != 0) {
                            (ptrARPTab->ucRetries)--;
                        }

                        if (ptrARPTab->ucRetries != 0) {
        #if IP_INTERFACE_COUNT > 1                                       // {19}
            #if defined RESTRICTED_GATEWAY_INTERFACE
                            fnRestrictGatewayInterface(ptrARPTab);       // user can decide how the re-resolve is controlled
            #else                                                        // default behavior
                #if IP_NETWORK_COUNT < 2                                 // {24} if there are more that one network we re-resolve on the original interface only to avoid sending ARPs to interfaces that are not on this network
                            ptrARPTab->ucInterface = INTERFACE_MASK;     // re-resolve on all available interfaces
                #endif
            #endif
        #endif
                            fnSendARP_request(ptrARPTab);              
                            ptrARPTab->ucState = (ARP_FIXED_IP | ARP_RESOLVING); // waiting for resolve answer
                            iFastPoll = 1;                               // this happens once every timeout period for the gateway only
                        }
                        else {
                            ptrARPTab->ucState = ARP_FREE;               // release entry 
                        }
                        ptrARPTab->ucTimeToLive = ARP_RESEND;            // retransmission timeout
                    }
                }
            }
        }
    }
    #endif
    #ifdef USE_IPV6
    i = 0;
    while (i++ < NEIGHBOR_TABLE_ENTRIES) {
        if ((ptrNN->ucState & ARP_RESOLVED) != 0) {
            if (ptrNN->ucTimeToLive != 0) {
                if (--(ptrNN->ucTimeToLive) == 0) {                      // if an entry has timed out  
                    ptrNN->ucState = ARP_FREE;                           // release entry 
                }
            }
        }
        ptrNN++;
    }
    #endif
    if (iFastPoll != 0) {
        uTaskerMonoTimer(OWN_TASK, T_ARP_PENDING_PERIOD, E_ARP_PENDING_CHECK); // short timeout for answer
    }
    else {
        uTaskerMonoTimer(OWN_TASK, T_ARP_PERIOD, E_ARP_CHECK);           // repeat after next interval 
    }
}



// This is called at a faster rate only when there are entries being resolved
// It returns zero if no entries are in the process of being resolved or a pointer to the first entry in cache which has not received an answer
//
static int fnARP_check_pending(void)
{
    int i;
    int iRetry = 0;
    #if defined USE_IPV6
    NEIGHBOR_TAB *ptrNN = tIPV6_Neighbors;
    #endif
    #if defined USE_IP
    ARP_TAB *ptrARPTab = tARP;
    i = 0;
    while (++i < ARP_TABLE_ENTRIES) {                                    // note that we jump the broadcast entry which is always the first in the table - it never times out...
        ptrARPTab++;
        if ((ptrARPTab->ucState & ARP_RESOLVING) != 0) {                 // are we trying to resolve this entry?
            if (ptrARPTab->ucRetries != 0) {
                ptrARPTab->ucRetries--;
                iRetry = 1;                                              // retain the fast polling rate
                fnSendARP_request(ptrARPTab);                            // we must retry a request for the specified ARP entry
            }
            else {
                ptrARPTab->ucState = ARP_FREE;                           // release entry after maximum retries
                if (ptrARPTab->OwnerTask != 0) {
                    fnArpMessage(ptrARPTab, ARP_RESOLUTION_FAILED);      // inform owner of resolution timeout
                }
            }
        }
    }
    #endif
    #if defined USE_IPV6
    i = 0;
    while (i++ < NEIGHBOR_TABLE_ENTRIES) {
        if ((ptrNN->ucState & ARP_RESOLVING) != 0) {                     // are we trying to resolve this entry?
            if (ptrNN->ucRetries != 0) {
                ptrNN->ucRetries--;
                iRetry = 1;                                              // retain the fast polling rate
                fnSendIPV6Discovery(ptrNN);                              // we must retry a request for the specified NN entry
            }
            else {
                ptrNN->ucState = ARP_FREE;                               // release entry after maximum retries
                if (ptrNN->OwnerTask != 0) {
                    fnNN_Message(ptrNN, NN_RESOLUTION_FAILED);           // inform owner of resolution timeout
                }
            }
        }
        ptrNN++;
    }
    #endif
    return iRetry;
}
#endif

#ifdef USE_IP
#if defined SUPPORT_VLAN && defined SUPPORT_DYNAMIC_VLAN_TX              // {12}
// Cover function for first write into ethernet output buffer including VLAN control on a frame basis
//
extern QUEUE_TRANSFER fnFirstWrite(QUEUE_HANDLE driver_id, unsigned char *output_buffer, QUEUE_TRANSFER nr_of_bytes, unsigned char ucVLAN_content)
{
    int iRtn;
    int iVlanState = vlan_active;                                        // backup present VLAN mode
    if ((ucVLAN_content & VLAN_UNTAG_TX_FRAME) != 0) {                   // if the transmission should be untagged
        vlan_active = 0;                                                 // don't insert VLAN tag into this transmission
    }
    #if defined ARP_VLAN_SUPPORT                                         // {13}
    else if (vlan_active != 0) {                                         // else if VLAN is enabled
        vlan_active += (ucVLAN_content & (VLAN_MEMBER_MASK));            // define VLAN ID to use for this transmission
    }
    #endif
    iRtn = fnWrite(driver_id, output_buffer, nr_of_bytes);               // prepare message
    vlan_active = iVlanState;                                            // restore general VLAN mode
    return iRtn;
}
#endif

// This function checks whether the IP address belongs to our sub-net
// - it returns zero if not true
//
#if IP_NETWORK_COUNT > 1
    static int is_subnet(unsigned char *new_ip, unsigned char ucNetworkID)
#else
    static int is_subnet(unsigned char *new_ip)
#endif
{
    int i = IPV4_LENGTH;
    unsigned char ucMaskBits;
    unsigned char *ptrOutIP = &network[_NETWORK_ID].ucOurIP[0];
    unsigned char *ptrNetMask = &network[_NETWORK_ID].ucNetMask[0];

    while (i--) {
        ucMaskBits = *ptrNetMask & *new_ip;
        if (((ucMaskBits ^ *ptrOutIP) & *ptrNetMask) != 0) {
            return 0;                                                    // not in our subnet !!
        }

        new_ip++;
        ptrOutIP++;
        ptrNetMask++;
    }
    return (1);                                                          // yes, in our IPv4 sub-net
}


// This function adds a new entry to the ARP cache and returns a pointer to it in cache. Should there be
// no more free entries it replaces the oldest
//
static ARP_TAB *fnAddNewARP(unsigned char ucType)
{
    int i = 0;
    ARP_TAB *ptrARPTab = tARP;
    ARP_TAB *ptrOldestARP = 0;

    while (++i < ARP_TABLE_ENTRIES) {                                    // note that we jump the broadcast entry which is always the first in the table 
        ++ptrARPTab;
        if (ptrARPTab->ucState == ARP_FREE) {                            // entry is free
            ptrARPTab->ucState = ucType;                                 // set state provisionally to type (IP fixed or not)
            return (ptrARPTab);                                          // return a pointer to the entry
        }
    }

    i = 0;                                                               // no entry space left - we must flush oldest entry
    ptrARPTab = tARP;
    while (++i < ARP_TABLE_ENTRIES) {
        ptrARPTab++;
        if ((ptrARPTab->ucState & (ARP_FIXED_IP | ARP_PERMANENT_IP | ARP_RESOLVING)) == 0) { // {3} never flush fixed types, {17} nor those that are presently resolving
            if ((ptrOldestARP == 0) || (ptrARPTab->ucTimeToLive < ptrOldestARP->ucTimeToLive)) {
                ptrOldestARP = ptrARPTab;                                // oldest entry found up to now
            }          
        }
    }
    if (ptrOldestARP != 0) {                                             // {17} don't enter if there was no entry that could be freed
        ptrOldestARP->ucState = ucType;                                  // set state provisionally to type (IP fixed or not)
    }
    return (ptrOldestARP);                                               // the return value will be 0 if all entries are fixed, permanent or resolving - this should be avoided (by having enough free non-fixed entries) since it may cause further ARP resolution to temporarily fail
}

static unsigned char fnTimeoutValue(unsigned char ucType)                // {20}
{
    if ((ucType & ARP_PERMANENT_IP) != 0) {
        return 0;                                                        // {3} never expires and so never needs refreshing
    }
    else if ((ucType & ARP_FIXED_IP) != 0) {
        return ARP_GATEWAY_TIMEOUT;
    }
    else {
        return ARP_TIMEOUT;                                              // standard timeout
    }
}

// This function either adds a new entry or refreshes an existing one since there has been activity
//
extern void fnAddARP(unsigned char *new_ip, unsigned char *rem_hwadr, ARP_DETAILS *ptrARP_details) // {14}
{
    int i = 0;
    ARP_TAB *ptrARPTab = tARP;
    #if IP_NETWORK_COUNT > 1
    unsigned char ucNetworkID = ptrARP_details->ucNetworkID;
    #endif
    unsigned char ucType = ptrARP_details->ucType;                       // {14}

    #if defined _WINDOWS
    if (ptrARPTab == 0) {                                                // avoid handling reception before the ARP table is ready
        return;
    }
    #endif
    while (++i < ARP_TABLE_ENTRIES) {                                    // note that we jump the broadcast entry which is always the first in the table - it never times out...
        ++ptrARPTab;
        if (ptrARPTab->ucState == ARP_FREE) {
            continue;
        }

        if (((uMemcmp(ptrARPTab->ucIP, new_ip, IPV4_LENGTH)) == 0)
    #if defined ARP_VLAN_SUPPORT                                         // {13}
                && (ptrARP_details->usVLAN_ID == ptrARPTab->usEntryVLAN_ID) // the IP address and the VLAN ID must match
    #endif
            ) {                                                          // if the IP address is already known
            if ((ucType & ARP_PERMANENT_IP) != 0) {
                return;                                                  // {3} never disturb a permanent entry because it will never timeout anyway
            }
            if (((ptrARPTab->ucState & ARP_RESOLVING) != 0) && (ptrARPTab->OwnerTask != 0)) { // {22} if adding while in the process or resolving
                fnArpMessage(ptrARPTab, ARP_RESOLUTION_SUCCESS);         // inform owner that we have resolved
                ptrARPTab->OwnerTask = 0;                                // avoid repeating message on refreshes of gateway
            }
            ptrARPTab->ucState &= ~ARP_STATE_MASK;
            goto _refresh;                                               // refresh already existing entries
        }
    }
                                                                         // there is no present entry - check that it is our subnet
    #if IP_NETWORK_COUNT > 1
    if ((is_subnet(new_ip, ucNetworkID)) == 0) 
    #else
    if ((is_subnet(new_ip)) == 0) 
    #endif
    {
        return;                                                          // do nothing if not in our subnet
    }
    
    if ((uMemcmp(new_ip, &network[_NETWORK_ID].ucDefGW[0], IPV4_LENGTH)) == 0) { // is it our default gateway?           
        if ((uMemcmp(&network[_NETWORK_ID].ucDefGW[0], cucNullMACIP, IPV4_LENGTH)) == 0) { // check that not null address
            return;                                                      // since we have no valid gatweway we can't enter one
        }
        else {
            ucType = ARP_FIXED_IP;                                       // this is a fixed entry which will be automatically resolved on timeout
        }
    }

    #if defined _WINDOWS && !defined PSEUDO_LOOPBACK
    if (uMemcmp(new_ip, &network[_NETWORK_ID].ucOurIP[0], IPV4_LENGTH) == 0) { // when simulating we can see our own broadcast messages, so do not enter them
        return;
    }
    #endif
    ptrARPTab = fnAddNewARP(ucType);                                     // we either add the new entry or replace the oldest entry (if no space)
    if (ptrARPTab == 0) {                                                // {17} protect from case when all entries are fixed or being used to resolve
        return;
    }
    
    uMemcpy(ptrARPTab->ucIP, new_ip, IPV4_LENGTH);                       // enter the new IPv4 address
    #if defined ARP_VLAN_SUPPORT                                         // {13}
    ptrARPTab->usEntryVLAN_ID = ptrARP_details->usVLAN_ID;
    #endif
_refresh:
    uMemcpy(ptrARPTab->ucMac, rem_hwadr, MAC_LENGTH);                    // add the details to the cache or refresh already existing entries

    #if IP_NETWORK_COUNT > 1
    ptrARPTab->ucNetworkID = ucNetworkID;                                // note which network this entry belongs to
    #endif
    #if IP_INTERFACE_COUNT > 1
    ptrARPTab->Tx_handle = ptrARP_details->Tx_handle;                    // the interface handle of where the IP can be contacted
    ptrARPTab->ucInterface = fnGetInterfaceFlag(ptrARP_details->Tx_handle, 0); // set the interface flag (single one corresponding to the interface handle)
    #endif

    ptrARPTab->ucTimeToLive = fnTimeoutValue(ucType);                    // {20} define the entry timeout value
    ptrARPTab->ucRetries    = ARP_MAXRETRY;
    ptrARPTab->ucState     |= ARP_RESOLVED;
}

static void fnBuildSendARP(ETHERNET_ARP_FRAME *arp, ARP_OUTPUT *ptrArpOutput)
{
    #if IP_NETWORK_COUNT > 1
    register unsigned char ucNetworkID = ptrArpOutput->ucNetworkID;
    #endif
    #if IP_INTERFACE_COUNT > 1
    register QUEUE_HANDLE  Tx_handle = ptrArpOutput->Tx_handle;
    #endif
    uMemcpy(arp->ethernet_header.destination_mac_address, ptrArpOutput->ptrDestMAC, MAC_LENGTH); // requestor's MAC or broadcast address
    uMemcpy(arp->ethernet_header.source_mac_address, &network[_NETWORK_ID].ucOurMAC[0], MAC_LENGTH); // our MAC address
    #if defined USE_RARP
    if (arp->arp_content.ucOpCode[1] > ARP_REPLY) {
        arp->ethernet_header.ethernet_type[0] = (unsigned char)(PROTOCOL_RARP >> 8); // ethernet type is RARP
        arp->ethernet_header.ethernet_type[1] = (unsigned char)(PROTOCOL_RARP); 
    }
    else {
        arp->ethernet_header.ethernet_type[0] = (unsigned char)(PROTOCOL_ARP >> 8); // ethernet type is ARP
        arp->ethernet_header.ethernet_type[1] = (unsigned char)(PROTOCOL_ARP);  
    }
    #else
    arp->ethernet_header.ethernet_type[0] = (unsigned char)(PROTOCOL_ARP >> 8); // ethernet type is ARP
    arp->ethernet_header.ethernet_type[1] = (unsigned char)(PROTOCOL_ARP);  
    #endif

    uMemcpy(arp->arp_content.ucHardwareType,      ucArpData, sizeof(ucArpData)); // hardware type / protocol type / hw size / protocol size
    uMemcpy(arp->arp_content.ucSenderMAC_address, &network[_NETWORK_ID].ucOurMAC[0], MAC_LENGTH); // sender MAC address
    uMemcpy(arp->arp_content.ucSender_IP_address, &network[_NETWORK_ID].ucOurIP[0], IPV4_LENGTH); // sender IP address
    uMemcpy(arp->arp_content.ucTargetMAC_address, ptrArpOutput->ptrReqMAC, MAC_LENGTH); // requesting MAC address
    uMemcpy(arp->arp_content.ucTarget_IP_address, ptrArpOutput->ptrIP, IPV4_LENGTH); // requesting IP address
    if (fnFirstWrite(_ETHERNET_HANDLE, (unsigned char*)arp, sizeof(ETHERNET_ARP_FRAME), ptrArpOutput->ucVLAN_content) != 0) { // {12} prepare message
        fnWrite(_ETHERNET_HANDLE, 0, 0);                                 // release frame
    }
    #if defined USE_IP_STATS
    fnIncrementEthernetStats(SENT_ARP_FRAMES, _NETWORK_ID);              // increment sent ARP count for statistics
    #endif
}


// This function sends an ARP request based on the resolved cache entry
//
#if defined USE_ZERO_CONFIG                                              // {6}
extern void fnSendARP_request(ARP_TAB *ptrARPTab)
#else
static void fnSendARP_request(ARP_TAB *ptrARPTab)
#endif
{
    #if IP_INTERFACE_COUNT > 1
    unsigned char ucInterfaces = ptrARPTab->ucInterface;                 // one or more interfaces to be requested over
    unsigned char ucInterfaceFlag = 0x01;
    int iReference = 0;
    #endif
    ARP_OUTPUT ArpOutput;
    ETHERNET_ARP_FRAME ucMsgArpRequest;                                  // fixed length arp request

    ucMsgArpRequest.arp_content.ucOpCode[0] = (unsigned char)(ARP_REQUEST >> 8); // add ARP Request OpCode
    ucMsgArpRequest.arp_content.ucOpCode[1] = (unsigned char)(ARP_REQUEST);
    ArpOutput.ptrReqMAC = (unsigned char *)cucNullMACIP;
    ArpOutput.ptrDestMAC = (unsigned char *)cucBroadcast;
    ArpOutput.ptrIP = ptrARPTab->ucIP;
    #if IP_NETWORK_COUNT > 1
    ArpOutput.ucNetworkID = ptrARPTab->ucNetworkID;
    #endif
    #if defined SUPPORT_VLAN && defined SUPPORT_DYNAMIC_VLAN_TX          // {12}
        #if defined ALTERNATIVE_VLAN_COUNT && ALTERNATIVE_VLAN_COUNT > 0 // {13}
    ArpOutput.ucVLAN_content = extractVLAN(ptrARPTab->OwnerSocket);      // enter possible alternative VLAN
        #else
    ArpOutput.ucVLAN_content = 0;                                        // no VLAN content present
        #endif
    #endif
    #if IP_INTERFACE_COUNT > 1
    do {                                                                 // send request to all interfaces belonging to the network
        if ((ucInterfaces & ucInterfaceFlag) != 0) {
            ArpOutput.Tx_handle = fnGetInterfaceHandle(iReference);
            if (ArpOutput.Tx_handle != 0) {
                fnBuildSendARP(&ucMsgArpRequest, &ArpOutput);            // send an ARP request on the interface
            }
        }
        iReference++;
        ucInterfaces &= ~ucInterfaceFlag;
        ucInterfaceFlag <<= 1;
    } while (ucInterfaces != 0);
    #else
    fnBuildSendARP(&ucMsgArpRequest, &ArpOutput);
    #endif
}

// This function sends an (R)ARP response if a request was destined to us. It updates the ARP cache in every case
//
static void fnSendARP_response(ARP_INPUT *ptrArpInput)
{
    #if IP_NETWORK_COUNT > 1
    register unsigned char ucNetworkID = ptrArpInput->frame->ucNetworkID;
    #endif
    #if IP_INTERFACE_COUNT > 1
    register QUEUE_HANDLE Tx_handle = ptrArpInput->frame->Tx_handle;
    #endif
    #ifdef USE_RARP
    int iMisMatch;
    unsigned short usArpNarp_reply;
    #endif
    register unsigned char *ptrData = ptrArpInput->ptrData;
    unsigned char *ucRequestingMAC = ptrData;                            // position of requesting MAC address in received message
    unsigned char *ucRequestingIP;
    ARP_DETAILS arp_details;
    arp_details.ucType = ARP_TEMP_IP;
    #if IP_NETWORK_COUNT > 1
    arp_details.ucNetworkID = ucNetworkID;
    #endif
    #if IP_INTERFACE_COUNT > 1
    arp_details.Tx_handle = Tx_handle;
    #endif
    #if defined ARP_VLAN_SUPPORT                                         // {13}
    arp_details.usVLAN_ID = ptrArpInput->frame->usVLAN_ID;
    #endif

    ptrData += MAC_LENGTH;                                               // jump MAC
    ucRequestingIP = ptrData;                                            // position of requesting IP address in received message
    ptrData += IPV4_LENGTH + MAC_LENGTH;                                 // jump our HW address
    
    #if defined _WINDOWS
    if ((uMemcmp(&network[_NETWORK_ID].ucOurIP[0], ucRequestingIP, IPV4_LENGTH)) == 0) {
        return;                                                          // ignore our own (R)ARP requests
    }
    #endif
    #if defined USE_RARP                                                 // is it for us ?
    if (ptrArpInput->ucResponseType == SEND_ARP) {
        iMisMatch = uMemcmp(&network[_NETWORK_ID].ucOurIP[0], ptrData, IPV4_LENGTH); // check match with our IP address
        usArpNarp_reply = ARP_REPLY;
    }
    else {
        iMisMatch = uMemcmp(&network[_NETWORK_ID].ucOurMAC[0], ptrData, MAC_LENGTH); // check match with our MAC address
        usArpNarp_reply = RARP_REPLY;
    }

    if (iMisMatch == 0)
    #else    
    if (uMemcmp(&network[_NETWORK_ID].ucOurIP[0], ptrData, IPV4_LENGTH) == 0) // check whether it is destined for our IP address
    #endif
    {
        ARP_OUTPUT ArpOutput;
        ETHERNET_ARP_FRAME ucMsgArpReply;                                // fixed length arp response

    #ifdef USE_RARP
        ucMsgArpReply.arp_content.ucOpCode[0] = 0;                       // add Reply OpCode (MSB always zero)
        ucMsgArpReply.arp_content.ucOpCode[1] = (unsigned char)(usArpNarp_reply);
    #else
        ucMsgArpReply.arp_content.ucOpCode[0] = (unsigned char)(ARP_REPLY >> 8); // add ARP reply OpCode
        ucMsgArpReply.arp_content.ucOpCode[1] = (unsigned char)(ARP_REPLY);
    #endif
        ArpOutput.ptrReqMAC = ArpOutput.ptrDestMAC = ucRequestingMAC;
        ArpOutput.ptrIP = ucRequestingIP;
    #if IP_NETWORK_COUNT > 1
        ArpOutput.ucNetworkID = ucNetworkID;
    #endif
    #if IP_INTERFACE_COUNT > 1
        ArpOutput.Tx_handle = Tx_handle;
    #endif
    #if defined SUPPORT_DYNAMIC_VLAN_TX
        ArpOutput.ucVLAN_content = ptrArpInput->frame->ucVLAN_content;   // {12}
    #endif
        fnBuildSendARP(&ucMsgArpReply, &ArpOutput);
    #if defined ARP_IGNORE_FOREIGN_ENTRIES                               // {2}
        fnAddARP(ucRequestingIP, ucRequestingMAC, &arp_details);         // {14}
    #endif
    #if defined USE_IP_STATS
        fnIncrementEthernetStats(RECEIVED_ARP_FRAMES, _NETWORK_ID);                                         
    #endif
    }
    else {
    #if defined USE_IP_STATS
        fnIncrementEthernetStats(SEEN_FOREIGN_ARP_FRAMES, _NETWORK_ID);  // add ARP to foreign statistics
    #endif
    }   
    #if !defined ARP_IGNORE_FOREIGN_ENTRIES                              // {2}
    fnAddARP(ucRequestingIP, ucRequestingMAC, &arp_details);             // {14}
    #endif
}

// We have received an ARP reply
//
static void fnHandleARP_response(ARP_INPUT *ptrArpInput)
{
    #if IP_NETWORK_COUNT > 1
    register unsigned char ucNetworkID = ptrArpInput->frame->ucNetworkID;// identity of the network that the response is for
    #endif
    #if IP_INTERFACE_COUNT > 1
    register unsigned char Tx_handle = ptrArpInput->frame->Tx_handle;    // the handle of the interface to be responded on
    #endif
    register unsigned char *ptrData = ptrArpInput->ptrData;
    unsigned char *ucRequestingMAC = ptrData;                            // position of requesting MAC address in received message
    unsigned char *ucRequestingIP;
    ARP_DETAILS arp_details;
    arp_details.ucType = ARP_TEMP_IP;
    #if IP_NETWORK_COUNT > 1
    arp_details.ucNetworkID = ucNetworkID;
    #endif
    #if IP_INTERFACE_COUNT > 1
    arp_details.Tx_handle = Tx_handle;
    #endif
    #if defined ARP_VLAN_SUPPORT                                         // {13}
    arp_details.usVLAN_ID = ptrArpInput->frame->usVLAN_ID;
    #endif
    ptrData += MAC_LENGTH;                                               // jump MAC
    ucRequestingIP = ptrData;                                            // position of requesting IP address in received message
    ptrData += (IPV4_LENGTH + MAC_LENGTH);                               // jump IP skip our HW address                          
    if ((uMemcmp(&network[_NETWORK_ID].ucOurIP[0], ptrData, IPV4_LENGTH)) == 0) { // if this ARP response is addressed to our IPv4 address
        int i = 0;
        ARP_TAB *ptrARPTab = tARP;

        while (++i < ARP_TABLE_ENTRIES) {                                // skip broadcast at first position
            ptrARPTab++;
            if (ptrARPTab->ucState == ARP_FREE) {                        // check whether we are waiting for the response
                continue;
            }

            if ((uMemcmp(ucRequestingIP, ptrARPTab->ucIP, IPV4_LENGTH) == 0) // from an IP address in the cache
    #if defined ARP_VLAN_SUPPORT                                         // {13}
                && (ptrArpInput->frame->usVLAN_ID == ptrARPTab->usEntryVLAN_ID) // the IP address and the VLAN ID must match
    #endif
                ) {
                uMemcpy(ptrARPTab->ucMac, ucRequestingMAC, MAC_LENGTH);  // refresh the entry belonging to the IP address
                ptrARPTab->ucTimeToLive = fnTimeoutValue(ptrARPTab->ucState); // {20} set the entry timeout value
                ptrARPTab->ucRetries = ARP_MAXRETRY;
                if (((ptrARPTab->ucState & ARP_RESOLVING) != 0) && (ptrARPTab->OwnerTask != 0))  {
                    fnArpMessage(ptrARPTab, ARP_RESOLUTION_SUCCESS);     // inform owner that we have resolved
                    ptrARPTab->OwnerTask = 0;                            // avoid repeating message on refreshes of gateway
                }
    #if IP_INTERFACE_COUNT > 1
                ptrARPTab->Tx_handle = Tx_handle;                        // enter the interface that resolved the entry
                ptrARPTab->ucInterface = fnGetInterfaceFlag(Tx_handle, 0); // {21} set the interface flag (single one corresponding to the interface handle)
    #endif
                ptrARPTab->ucState &= ~ARP_STATE_MASK;
                ptrARPTab->ucState |= ARP_RESOLVED;                      // entry resolved and can be used
                break;
            }
        }
    #if defined USE_IP_STATS
        fnIncrementEthernetStats(RECEIVED_ARP_FRAMES, _NETWORK_ID);      // updata addresses statistics
    #endif
    }
    else {                                                               // it was not an ARP to our IP address but we can still add it to our table or refresh the entry
        if (uMemcmp(ucRequestingIP, cucBroadcast, IPV4_LENGTH) != 0) {   // ignore broadcasts
            fnAddARP(ucRequestingIP, ucRequestingMAC, &arp_details);     // {14}
        }
    #if defined USE_IP_STATS
        fnIncrementEthernetStats(SEEN_FOREIGN_ARP_FRAMES, _NETWORK_ID);  // update statistics for foreign addresses
    #endif
    } 
}

// This function sends a message to the task which caused an ARP resolve to be stated indicating whether it was successful or not
//
static void fnArpMessage(ARP_TAB *ptrArpEntry, unsigned char ucIntEvent)
{
    unsigned char int_message[HEADER_LENGTH + 1 + sizeof(ptrArpEntry->OwnerSocket)]; // = { 0, 0 , ptrArpEntry->OwnerTask, OWN_TASK, 2, ucIntEvent, ptrArpEntry->OwnerSocket };  define standard event message
    int_message[MSG_DESTINATION_NODE]   = int_message[MSG_SOURCE_NODE] = INTERNAL_ROUTE;
    int_message[MSG_DESTINATION_TASK]   = (unsigned char)ptrArpEntry->OwnerTask;
    int_message[MSG_SOURCE_TASK]        = OWN_TASK;
    int_message[MSG_CONTENT_COMMAND]    = ucIntEvent;
    uMemcpy(&int_message[MSG_CONTENT_DATA_START], &ptrArpEntry->OwnerSocket, sizeof(ptrArpEntry->OwnerSocket));
    if (sizeof(ptrArpEntry->OwnerSocket) == 2) {                         // USOCKET is signed short rather than signed char
        int_message[MSG_CONTENT_LENGTH] = 3;
    }
    else {
        int_message[MSG_CONTENT_LENGTH] = 2;
    }
    fnWrite(INTERNAL_ROUTE, int_message, sizeof(int_message));           // we send details to the owner of the message starting ARP resolution
}


/****************************************** Global ARP routines ********************************************/


// This is called when an ARP packet (0806) seems to have been received
//
extern int fnProcessARP(ETHERNET_FRAME *frame) 
{
    register unsigned char *ptrData;
    ARP_INPUT arp_input;
    unsigned short usOpCode;
    #if defined USE_RARP                                                  // {1} do full check of frame type since it could be ARP or RARP
    int iArpType = 1;

    if ((frame->ptEth->ethernet_frame_type[0] != (unsigned char)(PROTOCOL_ARP >> 8)) && (frame->ptEth->ethernet_frame_type[1] != (unsigned char)(PROTOCOL_ARP))) { // Note caller checks whether second byte in type is correct (efficiency reasons)
        iArpType = 0;                                                    // not ARP
        if ((frame->ptEth->ethernet_frame_type[0] != (unsigned char)(PROTOCOL_RARP >> 8)) && (frame->ptEth->ethernet_frame_type[1] != (unsigned char)(PROTOCOL_RARP))) { // Note caller checks whether second byte in type is correct (efficiency reasons)
            return 0;                                                    // and not RARP
        }
    }
    #else
    if (frame->ptEth->ethernet_frame_type[0] != (unsigned char)(PROTOCOL_ARP >> 8)) { // note caller checks whether second byte in type is correct (efficiency reasons)
        return 0;                                                        // not ARP
    }
    #endif
   
    if (frame->frame_size < ((2 * MAC_LENGTH) + (2 * IPV4_LENGTH) + 2 + 6)) { // check that the frame is not too small to be valid 
        return (0);
    }
    
    ptrData = frame->ptEth->ucData;
    ptrData += 6;                                                        // jump hardware type (0001 == Ethernet) and Protocol (0800 == IP) and hardware and protocol sizes (6 and 4)

    usOpCode = *ptrData++;                                               // extract the op code from the input
    usOpCode <<= 8;
    usOpCode |= *ptrData++;

    arp_input.ptrData = ptrData;
    arp_input.frame = frame;
    #if IP_NETWORK_COUNT > 1
    frame->ucNetworkID = fnAssignNetwork(frame, (ptrData + (2 * MAC_LENGTH) + IPV4_LENGTH)); // {23}
    if (NETWORK_REJECT == frame->ucNetworkID) {
        return 0;                                                        // ignore the reception if the user decides to reject the frame
    }
    #endif
    #if defined USE_RARP                                                 // {1}
    if (iArpType == 0) {
        if (usOpCode == RARP_REQUEST) {
            arp_input.ucResponseType = SEND_RARP;
            fnSendARP_response(&arp_input);
            return 1;                                                    // RARP frame handled, no further processing necessary
        }
        else {
            return 0;                                                    // unrecognised type
        }
    }
    #endif
    #if defined USE_ZERO_CONFIG
        #if IP_NETWORK_COUNT > 1
    if (frame->ucNetworkID == DEFAULT_NETWORK) {                         // zero config only supported on the default network
        fnCheckZeroConfigCollision(arp_input.ptrData);                   // allow zero-configuration to analyse the ARP frame for collisions
    }
        #else
    fnCheckZeroConfigCollision(arp_input.ptrData);                       // {7} allow zero-configuration to analyse the ARP frame for collisions
        #endif
    #endif

    switch (usOpCode) {
    case ARP_REQUEST:
    #if defined USE_RARP
        arp_input.ucResponseType = SEND_ARP;                             // {1} send arp rather than RARP
    #endif
        fnSendARP_response(&arp_input);
        break;

    case ARP_REPLY:
        fnHandleARP_response(&arp_input);
        break;

    default:
        return 0;                                                        // invalid/unsupported op code
    }
    return 1;                                                            // ARP frame handled, no further processing necessary
}

// Get a free ARP table entry, fill it with details for resolving an IP address and issue an ARP resolution request
//
static void fnResolveAddress(unsigned char ucType, unsigned char *Search_IP, UTASK_TASK OwnerTask, USOCKET Socket) // {18}
{
    ARP_TAB *ptrARPTab = fnAddNewARP(ucType);                            // we either add the new entry or replace the oldest entry (if no space)
    if (ptrARPTab != 0) {                                                // protect from case when all entries are fixed or being used to resolve
#if IP_NETWORK_COUNT > 1
        ptrARPTab->ucNetworkID = extractNetwork(Socket);                 // note which network this entry belongs to
#endif
#if defined ARP_VLAN_SUPPORT
        if (vlan_active != 0) {                                          // if VLAN operation is enabled
            ptrARPTab->usEntryVLAN_ID = vlan_vid[extractVLAN(Socket)];   // VLAN that the request is to be made on
        }
        else {
            ptrARPTab->usEntryVLAN_ID = 0xffff;                          // mark no VLAN
        }
#endif
#if IP_INTERFACE_COUNT > 1
        ptrARPTab->ucInterface = extractInterface(Socket);               // note which interface(s) the request is to be made on
        if (ptrARPTab->ucInterface == 0) {                               // no interface specified
            ptrARPTab->ucInterface = (0x01 << DEFAULT_IP_INTERFACE);     // so assume default interface
        }
#endif
#if defined ALTERNATIVE_VLAN_COUNT && ALTERNATIVE_VLAN_COUNT > 0
        ptrARPTab->OwnerSocket = Socket;
#endif
        uMemcpy(ptrARPTab->ucIP, Search_IP, IPV4_LENGTH);                // enter the IP address that is being resolved
        uMemcpy(ptrARPTab->ucMac, cucBroadcast, MAC_LENGTH);             // enter broadcast MAC address during resolving

        ptrARPTab->ucTimeToLive = ARP_RESEND;                            // rest resolving timers and attempts
        ptrARPTab->ucRetries = ARP_MAXRETRY;
        fnSendARP_request(ptrARPTab);                                    // send first request

        ptrARPTab->OwnerTask = OwnerTask;                                // enter details of the owner and socket
        ptrARPTab->OwnerSocket = Socket;
        ptrARPTab->ucState |= ARP_RESOLVING;                             // entry state is pending
        uTaskerMonoTimer(OWN_TASK, T_ARP_PENDING_PERIOD, E_ARP_PENDING_CHECK); // adapt the ARP timer to monitor response timeout
    }
}

// This function returns details of a resolved entry in its cache. If there is no entry and the destination is in our subnet it will start trying to resolve (and returns invalid).
// If the destination is not in our subnet the gateway will be returned if known or else it will start to resolve it (and returns invalid).
//
extern ARP_TAB *fnGetIP_ARP(unsigned char *Search_IP, UTASK_TASK OwnerTask, USOCKET Socket)
{
    int i = 0;
    ARP_TAB *ptrARPTab = tARP;
    unsigned char ucType = ARP_TEMP_IP;
    #if IP_NETWORK_COUNT > 1
    unsigned char ucNetworkID = extractNetwork(Socket);                  // determine the network that the socket is operating on
    #endif
    #if defined ARP_VLAN_SUPPORT                                         // {13}
    unsigned short usVLAN_ID;
    #endif

    #if defined SUPPORT_SUBNET_TX_BROADCAST                              // {10}
    if (fnSubnetBroadcast(Search_IP, &network[_NETWORK_ID].ucOurIP[0], &network[_NETWORK_ID].ucNetMask[0], IPV4_LENGTH) != 0) { // {11} check whether the destination is our subnet broadcast
        return ptrARPTab;                                                // return the broadcast entry which is always at the start of the table
    }
    #endif
    #if defined ARP_VLAN_SUPPORT                                         // {13}
    if (vlan_active != 0) {
        usVLAN_ID = vlan_vid[extractVLAN(Socket)];
    }
    else {
        usVLAN_ID = 0xffff;
    }
    #endif

    // Note that the broadcast entry is always the first in the table and is not skipped since it is often required
    //
    while (i++ < ARP_TABLE_ENTRIES) {
        if (ptrARPTab->ucState != ARP_FREE) {
            if ((uMemcmp(ptrARPTab->ucIP, Search_IP, IPV4_LENGTH) == 0)
    #if defined ARP_VLAN_SUPPORT                                         // {13}
                && ((i == 1) || (usVLAN_ID == ptrARPTab->usEntryVLAN_ID))// the IP address and the VLAN ID must match (ignored by broadcast)
    #endif
                ) {
                if ((ptrARPTab->ucState & ARP_RESOLVED) == 0) {
                    return (0);                                          // IP address is in cache but is still unresolved...
                }
                return (ptrARPTab);                                      // details returned
            }
        }
        ptrARPTab++;
    }

    // There is no entry in the ARP table for the destination IPv4 address
    //
    #if IP_NETWORK_COUNT > 1          
    if (is_subnet(Search_IP, ucNetworkID) != 0)
    #else
    if (is_subnet(Search_IP) != 0) 
    #endif
    {                                                                    // we haven't found it in cache. If it is in our subnet, we will request
        if (uMemcmp(Search_IP, &network[_NETWORK_ID].ucDefGW[0], IPV4_LENGTH) == 0) { // if our default gateway
            if (uMemcmp(&network[_NETWORK_ID].ucDefGW[0], cucNullMACIP, IPV4_LENGTH) != 0) {
                ucType = ARP_FIXED_IP;                                   // fix the default gateway entry
            }
        }
        fnResolveAddress(ucType, Search_IP, OwnerTask, Socket);          // {18}
    }
    else {                                                               // not in our subnet so use gateway
        if ((uMemcmp(&network[_NETWORK_ID].ucDefGW[0], cucNullMACIP, IPV4_LENGTH)) == 0) { 
            return 0;                                                    // no valid gateway
        }

        ptrARPTab = tARP;                                                // start at the beginning of the ARP table
        i = 0;

        while (++i < ARP_TABLE_ENTRIES) {
            ptrARPTab++;                                                 // note that we jump the broadcast entry which is always the first in the table 
            if (ptrARPTab->ucState == ARP_FREE) {
                continue;
            }
    #if defined ARP_VLAN_SUPPORT                                         // {18}
            if (usVLAN_ID != ptrARPTab->usEntryVLAN_ID) {                // the IP address and the VLAN ID must match
                continue;
            }
    #endif

            if ((uMemcmp(ptrARPTab->ucIP, &network[_NETWORK_ID].ucDefGW[0], IPV4_LENGTH)) == 0) {
                if ((ptrARPTab->ucState & ARP_RESOLVED) == 0) {
                    return 0;                                            // IP address is in cache but is unresolved...
                }
                return (ptrARPTab);                                      // details of our default gateway returned
            }
        }
        fnResolveAddress(ARP_FIXED_IP, &network[_NETWORK_ID].ucDefGW[0], OwnerTask, Socket); // {18}
    }
    return 0;
}


/***************************************** routines for displaying and manipulating ARP cache ***********************************/

extern ARP_TAB *fnGetARPentry(unsigned char ucEntry/*, int iIP_MAC*/)    // {9}
{
    int i = 0;
    unsigned char ucEntryFound = 0;
    ARP_TAB *ptrARPTab = tARP;

    while (++i < ARP_TABLE_ENTRIES) {
        ptrARPTab++;                                                     // we don't include broadcast entry
        if ((ptrARPTab->ucState & ARP_RESOLVED) != 0) {
            if (++ucEntryFound == ucEntry) {
                return ptrARPTab;                                        // {9} return general pointer
//              if (iIP_MAC == GET_IP) {
//                  return ptrARPTab->ucIP;
//              }
//              else {
//                  return ptrARPTab->ucMac;
//              }
            }
        }                            
    }
    return 0;                                                            // {5} no entry found 
}

extern void fnDeleteArp(void)                                            // delete the ARP cache
{
    int i = 0;
    ARP_TAB *ptrARPTab = tARP;

    while (++i < ARP_TABLE_ENTRIES) {
        ptrARPTab++;                                                     // jump fixed broadcast entry
        ptrARPTab->ucState = ARP_FREE;
        ptrARPTab->OwnerTask = 0;
    }
}
#endif                                                                   // endif USE_IP

#ifdef USE_IPV6
extern unsigned char *fnGetNeighborEntry(unsigned char ucEntry, int iIP_MAC)
{
    int i = 0;
    unsigned char ucEntryFound = 0;
    NEIGHBOR_TAB *ptrNeighborTab = tIPV6_Neighbors;

    while (i++ < NEIGHBOR_TABLE_ENTRIES) {
        if ((ptrNeighborTab->ucState & ARP_RESOLVED) != 0) {
            if (++ucEntryFound == ucEntry) {
                if (iIP_MAC == GET_IP) {
                    return ptrNeighborTab->ucIPV6;
                }
                else {
                    return ptrNeighborTab->ucHWAddress;
                }
            }
        }
        ptrNeighborTab++;
    }
    return 0;                                                            // no entry found                           
}

extern void fnDeleteNeighbors(void)                                      // delete the IPV6 neighbor cache
{
    int i = 0;
    NEIGHBOR_TAB *ptrNeighborTab = tIPV6_Neighbors;

    while (i++ < NEIGHBOR_TABLE_ENTRIES) {
        ptrNeighborTab->ucState = ARP_FREE;
        ptrNeighborTab->OwnerTask = 0;
        ptrNeighborTab++;  
    }
}


// This function sends a message to the task which caused a neighbor discovery to be stated indicating whether it was successful or not
//
static void fnNN_Message(NEIGHBOR_TAB *ptrNeighborTab, unsigned char ucIntEvent)
{
    unsigned char int_message[HEADER_LENGTH + 2]; // = { 0, 0 , ptrArpEntry->OwnerTask, OWN_TASK, 2, ucIntEvent, ptrArpEntry->OwnerSocket };  define standard event message
    int_message[MSG_DESTINATION_NODE]   = int_message[MSG_SOURCE_NODE] = INTERNAL_ROUTE;
    int_message[MSG_DESTINATION_TASK]   = (unsigned char)ptrNeighborTab->OwnerTask;
    int_message[MSG_SOURCE_TASK]        = OWN_TASK;
    int_message[MSG_CONTENT_LENGTH]     = 2;
    int_message[MSG_CONTENT_COMMAND]    = ucIntEvent;
    int_message[MSG_CONTENT_DATA_START] = (unsigned char)ptrNeighborTab->OwnerSocket;
    fnWrite(INTERNAL_ROUTE, int_message, HEADER_LENGTH + 2);             // we send details to the owner of the message starting ARP resolution
}


// Enter or refresh an IPV6 neighbor entry
//
extern NEIGHBOR_TAB *fnEnterIPV6Neighbor(unsigned char *ucHWAddress, unsigned char *ucIPV6Address, unsigned char ucHWAddress_control_length)
{
    int i = 0;
    NEIGHBOR_TAB *ptrNeighborTab = tIPV6_Neighbors;
    NEIGHBOR_TAB *ptrOldestEntry = tIPV6_Neighbors;
    NEIGHBOR_TAB *ptrFreeEntry = 0;
    unsigned char ucOldestEntryTimeToLive = 0xff;
    unsigned char ucHWAddress_length = (ucHWAddress_control_length & ~(RESOLVED_ADDRESS | SEARCH_ADDRESS)); // mask control bits
    if ((ucHWAddress_control_length & RESOLVED_ADDRESS) != 0) {
        ucHWAddress_length = 0;
    }
    else {
        ucHWAddress_length = (ucHWAddress_control_length & ~(RESOLVED_ADDRESS | SEARCH_ADDRESS)); // mask control bits but reserve length
    }

    while (i++ < NEIGHBOR_TABLE_ENTRIES) {       
        if (ptrNeighborTab->ucState == ARP_FREE) {                       // entry is free
            if (ptrFreeEntry == 0) {
                ptrFreeEntry = ptrNeighborTab;                           // mark first free entry found
            }
        }
        else if (((ucHWAddress_length == 0) || ((ucHWAddress_length == ptrNeighborTab->ucHWAddressLength) && (uMemcmp(ucHWAddress, ptrNeighborTab->ucHWAddress, ucHWAddress_length) == 0))) && ((uMemcmp(ptrNeighborTab->ucIPV6, ucIPV6Address, IPV6_LENGTH)) == 0)) { // if the entry can be found refresh it
            if (ucHWAddress != 0) {                                      // if not a look-up
                if ((ucHWAddress_control_length & RESOLVED_ADDRESS) != 0) {
                    ucHWAddress_length = (ucHWAddress_control_length & ~(RESOLVED_ADDRESS | SEARCH_ADDRESS));
                    fnNN_Message(ptrNeighborTab, NN_RESOLUTION_SUCCESS); // inform owner that we have resolved
                    ptrFreeEntry = ptrNeighborTab;                       // update this resolving entry
                    break;
                }
                ptrNeighborTab->ucTimeToLive = ARP_TIMEOUT;              // refresh the entry
            }
            return ptrNeighborTab;                                       // return the NN entry
        }
        else if (ptrNeighborTab->ucTimeToLive < ucOldestEntryTimeToLive) {
            ucOldestEntryTimeToLive = ptrNeighborTab->ucTimeToLive;
            ptrOldestEntry = ptrNeighborTab;                             // remember oldest entry found
        }
        ptrNeighborTab++;
    }
    if (ptrFreeEntry == 0) {                                             // if the complete table is full we replace the oldest entry
        ptrFreeEntry = ptrOldestEntry;
    }
    uMemcpy(ptrFreeEntry->ucIPV6, ucIPV6Address, IPV6_LENGTH);           // enter the IPV6 address
    if (ucHWAddress != 0) {
        uMemcpy(ptrFreeEntry->ucHWAddress, ucHWAddress, ucHWAddress_length); // enter the hardware address
        ptrFreeEntry->ucHWAddressLength = ucHWAddress_length;
        ptrFreeEntry->ucTimeToLive = ARP_TIMEOUT;
        ptrFreeEntry->ucState = ARP_RESOLVED;
    }
    return ptrFreeEntry;
}

extern unsigned char *fnGetIPV6_NN(unsigned char *ucIPV6Address, UTASK_TASK OwnerTask, USOCKET Socket)
{
    NEIGHBOR_TAB *ptrNeighbor;
    ptrNeighbor = fnEnterIPV6Neighbor(0, ucIPV6Address, SEARCH_ADDRESS); // search for existing entry
    if (ptrNeighbor->ucState == ARP_RESOLVED) {                          // entry exists in the IPV6 neighbor table
        return ptrNeighbor->ucHWAddress;                                 // return a pointer to the MAC address of the device in the local network
    }
    else {                                                               // no entry available so either a free one was taken or the oldest re-allocated for us
        ptrNeighbor->ucTimeToLive = ARP_RESEND;                          // repetition delay
        ptrNeighbor->ucRetries    = ARP_MAXRETRY;                        // maximum repetitions
        ptrNeighbor->ucState      = ARP_RESOLVING;                       // mark that the entry is in the process of being resolved
        ptrNeighbor->OwnerTask    = OwnerTask;                           // the task owning the resolution
        ptrNeighbor->OwnerSocket  = Socket;                              // the socket number
        fnSendIPV6Discovery(ptrNeighbor);                                // transmit a discovery message
        uTaskerMonoTimer(OWN_TASK, T_ARP_PENDING_PERIOD, E_ARP_PENDING_CHECK); // monitor request
        return 0;
    }
}
#endif
