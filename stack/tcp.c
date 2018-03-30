/**********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher

    ---------------------------------------------------------------------
    File:      tcp.c
    Project:   Single Chip Embedded Internet
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2018
    **********************************************************************
    03.06.2007 fnGetFreeTCP_Port() made external                         {1}
    03.06.2007 Extend REUSE_TIME_WAIT_SOCKETS to rx connection use       {2}
    21.08.2007 Add HTTP windowing tx support (HTTP_WINDOWING_BUFFERS)    {3}
    22.08.2007 Discard buffered TCP data when no connection              {4}
    23.08.2007 Optionally silently discard peer's data when closing (DISCARD_DATA_WHILE_CLOSING) {5}
    29.08.2007 Reset usOpenCnt on new connection (correction for HTTP_WINDOWING_BUFFERS) {7}
    03.09.2007 Add optional external retrigger of socket idle timer      {8}
    25.04.2008 Add fnGetTCP_state()                                      {9}
    01.11.2008 Add FTP_DATA_WINDOWS option support for windowing         {10}
    18.04.2009 Correct repetition timer delay and limit repeats          {11}
    04.06.2009 Add option TCP_CHECK_SYS_REPS                             {12}
    01.07.2009 Correct transmission timer decrement on idle connection   {13}
    02.10.2009 Add a TCP_EVENT_CLOSED call back event when connection closed during the connection phase {14}
    11.10.2009 Workaround for compiler error in Codewarrior 7.1.2        {15}
    29.12.2009 Update present_tcp in regeneration events to ensure that it is correct {16}
    12.01.2010 Return SOCKET_NOT_FOUND if no socket exists               {17}
    14.01.2010 Allow socket reuse in state TCP_STATE_TIME_WAIT           {18}
    25.01.2010 Remove fnModifyTCPWindow() by integrating it into fnReportTCPWindow() {19}
    25.01.2010 Add a return value from fnSendTCPControl() and fnReportTCPWindow() {20}
    25.01.2010 Add automatic window update ACK repeat on ARP resolution  {21}
    24.04.2010 Commit TCP improvements:
              - send ack back to repeated data even if already handled   {22}
              - don't allow receptions to retrigger transmission timer   {23}
              - ensure no unnecessary ACKs returned                      {24}
    24.04.2010 Optionally protect buffered TCP transmission so that use from interrupt routines can't disturb task use {25}
    01.05.2010 Allow all TCP transmissions (also outside of lister) to retrigger counters {26}
    09.05.2010 Allow use of TCP_BUF_CHECK without defining a handling task {27}
    03.06.2010 Ignore windows tcp data when a previous frame has been not been received {28}
    15.06.2010 Don't call partial ACK event if the length is zero        {29}
    06.09.2010 Optimise code when the device performs rx and tx offloading {30}
    20.03.2011 Repeat SYN + ACK transmission when gateway MAC has had to be resolved while answering to remote SYN {31}
    20.03.2011 When updating peers rx window that is being informed of respect that there may still be non-acknowledged transmitted data {32}
    24.03.2011 Backup ack counter when handling acks                     {33}
    27.03.2011 Support INDIVIDUAL_BUFFERED_TCP_BUFFER_SIZE (allowing each buffered TCP socket to have its own buffer size) {34}
    28.03.2011 Enable TCP_WINDOW_UPDATE event when SUPPORT_PEER_WINDOW is enabled {35}
    11.07.2011 Add fnActiveTCP_connections()                             {36}
    04.08.2011 Use _TCP_BUFFER to ensure STANDARD_BUFFER_SIZE is understood correctly {37}
    30.10.2011 Add extended buffered TCP options                         {38}
    11.12.2011 Add fnTCP_IdleTimeout()                                   {39}
    27.02.2012 Conditionally remove ucIP_TCP_TYPE[]                      {40}
    11.03.2012 Redefine fnHandleTCP() and handle transmission over IPv6  {41}
    25.03.2012 Return acks to TCP keep-alives with zero date length      {42}
    25.03.2012 Use IPv6 link-local address for network local connections {43}
    02.04.2012 Add flags to fnTCP_Connect()                              {44}
    20.04.2012 Correct IP sources address when refusing TCP connection (active IPv6) {45}
    03.06.2012 Handle network neighbour resolution                       {46}
    19.07.2012 Reset TCP connection if SYN (without ACK) received from peer {47}
    09.08.2012 Handle ARP/NN resolution sockets wider than a byte (used with multiple networks and interfaces) {48}
    17.12.2012 Add fnGetTCP_port()                                       {49}
    17.12.2012 Add multiple network transmission support                 {50}
    17.12.2012 Pass received frame to fnMapTCPSocket()                   {51}
    13.01.2013 Modify fnTCP_send_reset() parameters to allow TCP resets to be returned to multiple networks {52}
    30.01.2013 Add VLAN to socket                                        {53}
    04.06.2013 Added TCP_SOCKET_MALLOC() and TCP_BUFFER_MALLOC() defaults {54}
    05.08.2013 Add IMMEDIATE_MEMORY_ALLOCATION option                    {55}
    06.08.2013 Handle TCP connection sockets wider than a byte (used with multiple networks and interfaces) {56}
    27.08.2013 Reset usOpenCnt on active client connections to ensure consistency after previously aborted connection {57}
    07.09.2013 Only poll TCP when there is a socket in a state requiring it {58}
    07.09.2013 Allow high resolution user defined TCP polling rate       {59}
    30.10.2013 Allow application to reject close request, rather than half-close {60}
    26.11.2013 Don't send TCP reset if no TCP socket pool available      {61}
    28.02.2014 Allow original port number to be retained when setting TCP socket to listening mode {62}
    29.03.2014 Set default interface port when PHY_TAIL_TAGGING is not used {63}
    07.11.2014 Move TCP_DEF_MTU from here to tcpip.h
    23.11.2014 Removed fnGetTCP_state() and made fnGetSocketControl() extern {64}
    25.05.2015 Add optional out-of-order reception buffer                {65}
    01.10.2015 Start testing MJBC_TEST5
    31.05.2016 Modify MJB_TEST5 case to accept FIN together with final data content {66}
    13.10.2016 Simultaneous close correction                             {67}
    02.02.2017 Adapt for us tick resolution                              {68}
    09.01.2018 Add secure socket layer support                           {69}
    09.01.2018 Add fnInsertTCPHeader()                                   {70}
    06.02.2018 Only callback with TCP_EVENT_CLOSED event when socket is reused from the TCP_STATE_FIN_WAIT_2 state {71}
    20.02.2018 Allow preparing data in the output buffer of an open connection but not start its transmission yet {72}

*/

#include "config.h"

// Test defines being validated
//
#define MJBC_TEST3                                                       // test recursive windowing call to optimise windowed TCP throughput
#define MJBC_TEST4                                                       // correct silly-window follow-ons
#define MJBC_TEST5                                                       // 1.10.2015 test ignoring FINs whos sequence counter indicates we have not seen some data yet

#if defined USE_TCP

/* =================================================================== */
/*                          local definitions                          */
/* =================================================================== */

#define OWN_TASK                 TASK_TCP

#if defined SOCKET_NUMBER_MASK && ((SOCKET_NUMBER_MASK + 1) < NO_OF_TCPSOCKETS)
    #error socket mask is not adequate for the TCP socket pool size!!
#endif

#if !defined TCP_SOCKET_MALLOC                                           // {54}
    #define TCP_SOCKET_MALLOC(x) uMalloc((MAX_MALLOC)(x))
#endif
#if !defined TCP_BUFFER_MALLOC                                           // {54}
    #define TCP_BUFFER_MALLOC(x) uMalloc((MAX_MALLOC)(x))
#endif

#if defined INDIVIDUAL_BUFFERED_TCP_BUFFER_SIZE
    #define STANDARD_BUFFER_SIZE TCP_BUFFER
    #define _TCP_BUFFER          (ptr_TCP->usTCP_buffer_length)          // {37}
#else
    #define _TCP_BUFFER          (TCP_BUFFER)                            // {37}
#endif
#if defined SUPPORT_PEER_MSS
    #define SUPPORT_RX_TCP_OPTIONS
#endif

#if !defined T_TCP_PERIOD
    #define T_TCP_PERIOD         (DELAY_LIMIT)(1 * SEC)                  // default polling period of 1 seconds
#endif
#define E_POLL_TCP               1                                       // polling event

#define TCP_MAX_WINDOW_SIZE      TCP_DEF_MTU

#if IP_NETWORK_COUNT > 1                                                 // {50}
    #define _NETWORK_ID          ucNetworkID
#else
    #define _NETWORK_ID          DEFAULT_NETWORK 
#endif
/* =================================================================== */
/*                      local structure definitions                    */
/* =================================================================== */

typedef struct stOUT_OF_ORDER_FRAME                                      // {65}
{
    TCP_CONTROL   *ptr_TCP;                                              // the present owner of the out-of-order buffer
    unsigned long  ulSequenceNumber;                                     // the sequence number of the out of order buffer
    unsigned short usLength;                                             // the length of the data in the storage buffer
    unsigned char  ucTTL;                                                // time to live of the out of order buffer (invalid when zero)
    unsigned char  ucData[TCP_DEF_MTU];                                  // single flame storage memory
} OUT_OF_ORDER_FRAME;

/* =================================================================== */
/*                 local function prototype declarations               */
/* =================================================================== */

static void fnPollTCP(void);
static signed short fnSendTCPControl(TCP_CONTROL *ptr_TCP);              // {20}
static void fnAbortTCPSession(TCP_CONTROL *ptr_TCP);
#if (!defined IP_RX_CHECKSUM_OFFLOAD || !defined IP_TX_PAYLOAD_CHECKSUM_OFFLOAD) || (defined FORCE_PAYLOAD_ICMPV6_TX && defined USE_IPV6 && defined USE_IPV6INV4) || defined IP_INTERFACE_WITHOUT_CS_OFFLOADING || defined _WINDOWS // {30}
    static unsigned short fnCalculateTCP_checksum(unsigned char *ucIPSource, unsigned char *ucIPDest, unsigned char *ucTCP_data, unsigned short usTCP_len, int iOptions);
#endif
#if defined ANNOUNCE_MAX_SEGMENT_SIZE
    static void fnSendSyn(TCP_CONTROL *ptr_TCP, unsigned char ucFlags);
    #define MSS_OPTION_LENGTH 4
    #define MSS_KIND          2
    #define MSS_LENGTH        4
#endif

/* =================================================================== */
/*                             constants                               */
/* =================================================================== */

#if (!defined IP_RX_CHECKSUM_OFFLOAD || !defined IP_TX_PAYLOAD_CHECKSUM_OFFLOAD) || (defined FORCE_PAYLOAD_ICMPV6_TX && defined USE_IPV6 && defined USE_IPV6INV4) || defined IP_INTERFACE_WITHOUT_CS_OFFLOADING || defined _WINDOWS // {40}
    static const unsigned char ucIP_TCP_TYPE[] = {0x00, IP_TCP};
#endif

#if defined SUPPORT_PEER_WINDOW && (defined HTTP_WINDOWING_BUFFERS || defined FTP_DATA_WINDOWS) // {10}
    const static unsigned char ucProbeTime[] = {5, 6, 12, 24, 28, 60};   // {3}
#endif

/* =================================================================== */
/*                     global variable definitions                     */
/* =================================================================== */

#if defined HTTP_WINDOWING_BUFFERS || defined FTP_DATA_WINDOWS           // {10}
    TCP_CONTROL *present_tcp = 0;                                        // {3} global pointer to present tcp socket's control structure
#endif

/* =================================================================== */
/*                      local variable definitions                     */
/* =================================================================== */

static TCP_CONTROL *tTCP = 0;
static int iTCP_timer_active = 0;                                        // {58}
#if defined USE_OUT_OF_ORDER_TCP_RX_BUFFER
    OUT_OF_ORDER_FRAME OutOfOrder = {0};                                 // {65} single out of order frame storage
#endif

// TCP task
//
extern void fnTaskTCP(TTASKTABLE *ptrTaskTable)
{
    QUEUE_HANDLE PortIDInternal = ptrTaskTable->TaskID;                  // queue ID for task input
    unsigned char ucInputMessage[MEDIUM_MESSAGE];                        // reserve space for receiving messages

    while (fnRead(PortIDInternal, ucInputMessage, HEADER_LENGTH)) {      // check input queue
        switch (ucInputMessage[MSG_SOURCE_TASK]) {
            case TIMER_EVENT:
                fnPollTCP();                                             // do TCP management on a periodic basis
              //uTaskerMonoTimer(OWN_TASK, T_TCP_PERIOD, E_POLL_TCP);    // restart the timer when still required - {58} retrigger now in fnPollTCP()
                break;

            case TASK_ARP:
            {
                TCP_CONTROL *ptr_TCP = tTCP;
                USOCKET socket;                                          // {48}
                fnRead(PortIDInternal, ucInputMessage, ucInputMessage[MSG_CONTENT_LENGTH]);// read the contents
                uMemcpy(&socket, &ucInputMessage[1], sizeof(socket));    // {48} extract the socket number from the received message
                _TCP_SOCKET_MASK_ASSIGN(socket);                         // {56}
                ptr_TCP += (socket & SOCKET_NUMBER_MASK);                // {48} set to referenced socket
                switch (ucInputMessage[0]) {                             // ARP sends us either ARP resolution success or failed
#if defined USE_IPV6                                                     // {46}
                case NN_RESOLUTION_SUCCESS:                              // network neighbour resolution is handled equivalently to ARP resolution
#endif
                case ARP_RESOLUTION_SUCCESS:                             // a TCP socket is owner of resolved address
                    if (ptr_TCP->ucTCP_state == TCP_STATE_SYN_SENT) {    // TCP socket waiting for connection
#if defined ANNOUNCE_MAX_SEGMENT_SIZE
                        fnSendSyn(ptr_TCP, TCP_FLAG_SYN);                // destination resolved to which a SYN is being sent - repeat SYN + ACK with MSS announcement
#else
                        ptr_TCP->ucSendFlags = TCP_FLAG_SYN;             // destination resolved to which a SYN is being sent - repeat SYN
                        fnSendTCPControl(ptr_TCP);                       // resent because IP address has just been resolved
#endif
                    }
                    else if (ptr_TCP->ucTCP_state == TCP_STATE_SYN_RCVD) { // {31}
#if defined ANNOUNCE_MAX_SEGMENT_SIZE 
                        fnSendSyn(ptr_TCP, (TCP_FLAG_SYN | TCP_FLAG_ACK)); // gateway resolved after trying to respond to a remote SYN - repeat SYN + ACK
#else
                        ptr_TCP->ucSendFlags = (TCP_FLAG_SYN | TCP_FLAG_ACK); // gateway resolved after trying to respond to a remote SYN - repeat SYN + ACK
                        fnSendTCPControl(ptr_TCP);                       // resent because IP address has just been resolved
#endif
                    }
                    else {
                        if (ptr_TCP->event_listener) {                   // this was due to an ARP resolve during established connection so the application must repeat
                            unsigned short usRepeatLength = (unsigned short)(ptr_TCP->ulNextTransmissionNumber - ptr_TCP->ulSendUnackedNumber);
                            ptr_TCP->ulNextTransmissionNumber = ptr_TCP->ulSendUnackedNumber;
#if defined CONTROL_WINDOW_SIZE                                          // {21}
                            if (ptr_TCP->ucTCPInternalFlags & WINDOW_UPDATE) {
                                ptr_TCP->ucTCPInternalFlags &= ~WINDOW_UPDATE;
                                if (usRepeatLength == 0) {               // if no payload data to be resent, otherwise the ACK will be sent with the payload
                                    fnReportTCPWindow(ptr_TCP->MySocketNumber, ptr_TCP->usRxWindowSize);
                                    break;
                                }
                            }
#endif
#if defined HTTP_WINDOWING_BUFFERS || defined FTP_DATA_WINDOWS           // {16}
                            present_tcp = ptr_TCP;                       // set global pointer to the present receive socket TCP control structure
#endif

                            ptr_TCP->event_listener(socket, TCP_EVENT_REGENERATE, 0, usRepeatLength); // {56}
                        }
                        else {
                            // This occurs when a reset message is sent to an external IP address which must first be resolved
                            // We retransmit the prepared data (this assumes that two different resets are not in progress)
                            //
                            fnSendTCPControl(&tTCP[NO_OF_TCPSOCKETS]);   // use last socket internally
                        }
                    }
                    break;

#if defined USE_IPV6                                                     // {46}
                case NN_RESOLUTION_FAILED:                               // network neighbour resolution failure is handled equivalently to ARP resolution failure
#endif
                case ARP_RESOLUTION_FAILED:
                    if (ptr_TCP->event_listener) {                       // inform listener of problem
                        ptr_TCP->event_listener(socket, TCP_EVENT_ARP_RESOLUTION_FAILED, ptr_TCP->ucRemoteIP, ptr_TCP->usRemport); // {56}
                    }
                    break;
                }
            }
            break;
        }
    }
}


// Routine used to get a TCP socket. It will generate a pool of sockets on first call.
//
extern USOCKET fnGetTCP_Socket(unsigned char ucTos, unsigned short usIdleTimeout, int (*listener)(USOCKET, unsigned char, unsigned char *, unsigned short))
{
    USOCKET sockets;
    TCP_CONTROL *ptr_TCP;

    if (listener == 0) {
        return (NO_TCP_LISTENER_INSTALLED);                              // no listener function specified
    }

    if (tTCP == 0) {                                                     // automatically get heap space on first call
        tTCP = (TCP_CONTROL *)TCP_SOCKET_MALLOC(sizeof(TCP_CONTROL) * (NO_OF_TCPSOCKETS + 1)); // get TCP control space including an internal socket used for resets
        tTCP[NO_OF_TCPSOCKETS].MySocketNumber = NO_OF_TCPSOCKETS;        // special case for last socket which is used to send resets (not a real socket...)
      //uTaskerMonoTimer(OWN_TASK, T_TCP_PERIOD, E_POLL_TCP);            // start polling - {58} don't start until required
    }

    ptr_TCP = tTCP;                                                      // set a local pointer to socket pool

    for (sockets = 0; sockets < NO_OF_TCPSOCKETS; sockets++)    {
        if (ptr_TCP->ucTCP_state == TCP_STATE_FREE) {                    // search for a free socket
            uMemset(ptr_TCP, 0, sizeof(TCP_CONTROL));                    // socket free

            ptr_TCP->ucTCP_state = TCP_STATE_RESERVED;                   // make sure no one else gets socket before it starts to be used
            ptr_TCP->ucTos = ucTos;                                      // set the type of service required from socket
            ptr_TCP->event_listener = listener;                          // add the listener call back function
            ptr_TCP->usIdleTimeout = usIdleTimeout;                      // seconds
            return (sockets);
        }
        ptr_TCP++;
    }
    return (NO_TCP_SOCKET_FREE);                                         // no free socket found
}

// Routine used to release a socket back to the TCP socket pool
//
extern USOCKET fnReleaseTCP_Socket(USOCKET TCPSocket)
{
    TCP_CONTROL *ptr_TCP;

    if ((ptr_TCP = fnGetSocketControl(TCPSocket)) == 0) {                // get a pointer to the TCP socket control structure
        return SOCKET_NOT_FOUND;                                         // {17}
    }

    uMemset(ptr_TCP, 0, sizeof(TCP_CONTROL));                            // clear all contents
    return (TCPSocket);
}

// Passive open for server use
//
extern USOCKET fnTCP_Listen(USOCKET TCP_socket, unsigned short usPort, unsigned short usMaxWindow)
{
    TCP_CONTROL *ptr_TCP;

    if ((ptr_TCP = fnGetSocketControl(TCP_socket)) == 0) {               // get a pointer to the TCP socket control structure
        return SOCKET_NOT_FOUND;                                         // {17}
    }

    if ((ptr_TCP->ucTCP_state & (TCP_STATE_RESERVED | TCP_STATE_LISTEN | TCP_STATE_CLOSED | TCP_STATE_TIME_WAIT)) == 0) {
        return SOCKET_STATE_INVALID;                                     // not possible to listen
    }

    ptr_TCP->ucTCP_state = TCP_STATE_LISTEN;                             // initialise socket
    ptr_TCP->ucTCPInternalFlags = 0;
    uMemset(ptr_TCP->ucRemoteIP, 0, sizeof(ptr_TCP->ucRemoteIP));
    ptr_TCP->usRemport = 0;
    if (usPort != 0) {                                                   // {62} retain original port number if none is specified
        ptr_TCP->usLocport = usPort;
    }
    ptr_TCP->ulSendUnackedNumber = 0;
    ptr_TCP->ucSendFlags = 0;
    ptr_TCP->ulNextTransmissionNumber = 0xFFFFFFFF;
    ptr_TCP->ulNextReceptionNumber = 0;
    ptr_TCP->ucRetransmissions = 0;
#if defined CONTROL_WINDOW_SIZE
    if (usMaxWindow == 0) {
        ptr_TCP->usRxWindowSize = TCP_MAX_WINDOW_SIZE;                   // set standard window size if application doesn't care
    }
    else {
        ptr_TCP->usRxWindowSize = usMaxWindow;                           // if application specifies a special windows size, set it here
    }
#endif
    return (TCP_socket);
}

// Get a free TCP port from the dynamic port range
//
extern unsigned short fnGetFreeTCP_Port(void)                            // {1}
{
    TCP_CONTROL *ptr_TCP;
#if !defined RANDOM_NUMBER_GENERATOR
    static unsigned short usLastport = TCP_PORTS_START;
#else
    static unsigned short usLastport = 0;
#endif
    unsigned short usStart;
    unsigned short us_i;

#if defined RANDOM_NUMBER_GENERATOR
    if (usLastport == 0) {
        usLastport = TCP_PORTS_START;
        usLastport += (fnRandom() / (0xffff / (TCP_PORTS_END - TCP_PORTS_START)));
    }
#endif

    for (usStart = usLastport++; usStart != usLastport; usLastport++) {
        if (usLastport < TCP_PORTS_START) {
            usLastport = TCP_PORTS_START;
        }

        ptr_TCP = tTCP;

        for (us_i = 0; us_i < NO_OF_TCPSOCKETS; us_i++) {
            if ((ptr_TCP->ucTCP_state > TCP_STATE_CLOSED) && (ptr_TCP->usLocport == usLastport)) {
                break;                                                   // this socket has reserved the port, try next one...
            }
            ptr_TCP++;
        }

        if (us_i == NO_OF_TCPSOCKETS) {
            break;
        }
    }

    if (usLastport == usStart) {
        return (0);                                                      // no TCP ports available...
    }

    return (usLastport);
}

extern TCP_CONTROL *fnGetSocketControl(USOCKET TCP_socket)               // {64}
{
    TCP_CONTROL *ptr_TCP = tTCP;
    if (TCP_socket < 0) {                                                // protect against invalid sockets
        return 0;                                                        // invalid
    }
    _TCP_SOCKET_MASK_ASSIGN(TCP_socket);                                 // {56}
    if (TCP_socket > NO_OF_TCPSOCKETS) {                                 // protect against invalid sockets
        return 0;                                                        // invalid
    }
    return (ptr_TCP + TCP_socket);                                       // return a pointer to the socket control structure
}

// Request present state of a TCP socket
//
/*
extern unsigned char fnGetTCP_state(USOCKET TCP_socket)                  // {9}
{
    TCP_CONTROL *TCP_contol = fnGetSocketControl(TCP_socket);            // get the control structure belonging to this socket

    if (TCP_contol == 0) {                                               // invalid socket
        return TCP_STATE_BAD_SOCKET;
    }
    else {
        return TCP_contol->ucTCP_state;                                  // return present state
    }
}*/

extern unsigned short fnGetTCP_port(USOCKET TCP_socket, int iDestination)// {49}
{
    TCP_CONTROL *TCP_contol = fnGetSocketControl(TCP_socket);            // get the control structure belonging to this socket
    if (TCP_contol == 0) {
        return 0;
    }
    if (GET_LOCAL_PORT == iDestination) {
        return TCP_contol->usLocport;                                    // return the local port that this socket is using
    }
    else {                                                               // GET_REMOTE_PORT
        return TCP_contol->usRemport;                                    // return the remote port that this socket is using
    }
}


#if defined DATA_PORT_TRIGGERS_CONTROL
extern void fnTCP_Activity(USOCKET TCP_socket)                           // {8}
{
    TCP_CONTROL *ptr_TCP = fnGetSocketControl(TCP_socket);
    if (ptr_TCP != 0) {
        ptr_TCP->usLinkTimer = ptr_TCP->usIdleTimeout;
    #if defined HIGH_RESOLUTION_TCP_POLLING                              // {59}
        ptr_TCP->ucLinkDivider = 0;                                      // reset the link timer prescaler
    #endif
    }
}
#endif

#if defined SUPPORT_PEER_WINDOW
static unsigned long fnGetTxSequence(TCP_CONTROL *ptr_TCP)
{
    unsigned long ulSequenceNumber = ptr_TCP->ulSendUnackedNumber;

#if defined USE_BUFFERED_TCP
    if (ptr_TCP->ptrTCP_Tx_Buffer != 0) {
        ulSequenceNumber += ptr_TCP->ptrTCP_Tx_Buffer->usOpenBytes;
    }
    #if defined HTTP_WINDOWING_BUFFERS || defined FTP_DATA_WINDOWS       // {3}{10}
    else {
        ulSequenceNumber += ptr_TCP->usOpenCnt;
    }
    #endif
#elif defined HTTP_WINDOWING_BUFFERS || defined FTP_DATA_WINDOWS         // {3}{10}
    ulSequenceNumber += ptr_TCP->usOpenCnt;
#endif

    return ulSequenceNumber;
}
#endif

// Send a TCP frame
//
extern signed short fnSendTCP(USOCKET TCP_socket, unsigned char *ptrBuf, unsigned short usDataLen, unsigned char ucTempFlags)
{
    TCP_CONTROL    *ptr_TCP;
    unsigned char  *ptrData = ptrBuf;
#if IP_NETWORK_COUNT > 1                                                 // {50}
    #if !defined IP_TX_PAYLOAD_CHECKSUM_OFFLOAD || defined IP_INTERFACE_WITHOUT_CS_OFFLOADING || defined _WINDOWS || (defined FORCE_PAYLOAD_ICMPV6_TX && defined USE_IPV6 && defined USE_IPV6INV4)
    register unsigned char ucNetworkID = extractNetwork(TCP_socket);
    #endif
#endif
#if defined SUPPORT_PEER_WINDOW
    unsigned long   ulSequenceNumber;
#endif
#if defined USE_IPV6
    int iIPv6 = 0;
#endif
#if !defined IP_TX_PAYLOAD_CHECKSUM_OFFLOAD || defined IP_INTERFACE_WITHOUT_CS_OFFLOADING || (defined FORCE_PAYLOAD_ICMPV6_TX && defined USE_IPV6 && defined USE_IPV6INV4) || defined _WINDOWS // {30}
    unsigned short  usCheckSum;
    #if IP_INTERFACE_COUNT > 1
    unsigned char ucTxInterfaceHandling = 0;
    #else
        #define ucTxInterfaceHandling  INTERFACE_NO_TX_PAYLOAD_CS_OFFLOADING
    #endif
#endif
    if ((ptr_TCP = fnGetSocketControl(TCP_socket)) == 0) {
        return SOCKET_NOT_FOUND;                                         // {17} get a pointer to the TCP socket control structure
    }

#if defined USE_SECURE_SOCKET_LAYER                                      // {69}
    if ((TCP_socket & SECURE_SOCKET_MODE) != 0) {                        // if we are operating in secure mode
        return (fnSecureLayerTransmission(_TCP_SOCKET_MASK(TCP_socket), ptrBuf, usDataLen, ucTempFlags));
    }
#endif
#if defined USE_IPV6
    if (ptr_TCP->ucTCPInternalFlags & TCP_OVER_IPV6) {                   // {41} send over IPv6 and not IPv4
        iIPv6 = 1;
    }
#endif
    // Create a TCP frame in a hardware independent manner
    //
    *ptrData++ = (unsigned char)(ptr_TCP->usLocport >> 8);               // source port
    *ptrData++ = (unsigned char)(ptr_TCP->usLocport);
    *ptrData++ = (unsigned char)(ptr_TCP->usRemport >> 8);               // destination port
    *ptrData++ = (unsigned char)(ptr_TCP->usRemport);
#if defined SUPPORT_PEER_WINDOW
    ulSequenceNumber = fnGetTxSequence(ptr_TCP);                         // get sequence number, respecting all non-acked frames
    *ptrData++ = (unsigned char)(ulSequenceNumber >>24);                 // sequence number
    *ptrData++ = (unsigned char)(ulSequenceNumber >>16);
    *ptrData++ = (unsigned char)(ulSequenceNumber >>8);
    *ptrData++ = (unsigned char)(ulSequenceNumber);
#else
    *ptrData++ = (unsigned char)(ptr_TCP->ulSendUnackedNumber >>24);     // sequence number
    *ptrData++ = (unsigned char)(ptr_TCP->ulSendUnackedNumber >>16);
    *ptrData++ = (unsigned char)(ptr_TCP->ulSendUnackedNumber >>8);
    *ptrData++ = (unsigned char)(ptr_TCP->ulSendUnackedNumber);
#endif
    *ptrData++ = (unsigned char)(ptr_TCP->ulNextReceptionNumber >>24);   // acknowledgement number
    *ptrData++ = (unsigned char)(ptr_TCP->ulNextReceptionNumber >>16);
    *ptrData++ = (unsigned char)(ptr_TCP->ulNextReceptionNumber >>8);
    *ptrData++ = (unsigned char)(ptr_TCP->ulNextReceptionNumber);
#if defined ANNOUNCE_MAX_SEGMENT_SIZE
    if ((ptr_TCP->ucSendFlags & TCP_FLAG_SYN) != 0) {                    // we add MSS length option to our syn frame
        *ptrData++ = ((MIN_TCP_HLEN + MSS_OPTION_LENGTH) << 2);          // complete header length including MSS option
    }
    else {
        *ptrData++ = (MIN_TCP_HLEN << 2);                                // fixed header length without options
        ptr_TCP->ulNextTransmissionNumber += usDataLen;                  // set the next transmission sequence number based on the data to be sent
    }
#else
    *ptrData++ = (MIN_TCP_HLEN << 2);                                    // fixed header length without options
    ptr_TCP->ulNextTransmissionNumber += usDataLen;                      // set the next transmission sequence number based on the data to be sent
#endif
    *ptrData++ = (ptr_TCP->ucSendFlags | ucTempFlags);                   // flags plus any additional defined by the application
#if defined CONTROL_WINDOW_SIZE
    *ptrData++ = (unsigned char)(ptr_TCP->usRxWindowSize >> 8);          // our windows size (the number of bytes we are capable of receiving)
    *ptrData++ = (unsigned char)(ptr_TCP->usRxWindowSize);
#else
    *ptrData++ = (unsigned char)(TCP_DEF_MTU >> 8);                      // our windows size (the number of bytes we are capable of receiving)
    *ptrData++ = (unsigned char)(TCP_DEF_MTU);
#endif
    *ptrData++ = 0;                                                      // checksum
    *ptrData++ = 0;
    *ptrData++ = 0;                                                      // URGENT pointer
    *ptrData   = 0;

    usDataLen += MIN_TCP_HLEN;                                           // add the length of the header without options
                                                                         // calculate checksum of IP pseudo header and payload
#if !defined IP_TX_PAYLOAD_CHECKSUM_OFFLOAD || defined IP_INTERFACE_WITHOUT_CS_OFFLOADING || defined _WINDOWS // {30}
    #if IP_INTERFACE_COUNT > 1
    ucTxInterfaceHandling = (fnGetInterfaceCharacteristics(fnGetFirstInterface(TCP_socket)) & INTERFACE_NO_TX_PAYLOAD_CS_OFFLOADING); // extract the interface's transmission characteristics
    #endif
    #if defined USE_IPV6
    if (iIPv6 != 0) {
        unsigned char *ptrIpv6Address;
        if (uMemcmp(ptr_TCP->ucRemoteIP, ucLinkLocalIPv6Address[DEFAULT_NETWORK], IPV6_SUBNET_LENGTH) != 0) { // if not in the IPv6 local subnet use the IPVv6 global IPv6 address
            ptrIpv6Address = &network[_NETWORK_ID].ucOurIPV6[0];
        }
        else {
            ptrIpv6Address = ucLinkLocalIPv6Address[DEFAULT_NETWORK];
        }
        usCheckSum = fnCalculateTCP_checksum(ptrIpv6Address, ptr_TCP->ucRemoteIP, (ptrData - (MIN_TCP_HLEN - 1)), usDataLen, (ucTxInterfaceHandling |INTERFACE_CALC_TCP_IPv6)); // {43}
    }
    else {
    #endif
        usCheckSum = fnCalculateTCP_checksum(&network[_NETWORK_ID].ucOurIP[0], ptr_TCP->ucRemoteIP, (ptrData - (MIN_TCP_HLEN - 1)), usDataLen, ucTxInterfaceHandling);
    #if defined USE_IPV6
    }
    #endif
    ptrData -= 3;
    *ptrData++ = (unsigned char)(usCheckSum >> 8);                       // save TCP checksum in correct place
    *ptrData   = (unsigned char)usCheckSum;
#elif (defined FORCE_PAYLOAD_ICMPV6_TX && defined USE_IPV6 && defined USE_IPV6INV4) // checksum only needed when generating TCP frames that are tunneled
    if ((iIPv6 != 0) && (uMemcmp(ptr_TCP->ucRemoteIP, ucLinkLocalIPv6Address[DEFAULT_NETWORK], IPV6_SUBNET_LENGTH) != 0) && (uMemcmp(&network[_NETWORK_ID].ucTunnelIPV4[0], cucNullMACIP, IPV4_LENGTH) != 0)) { // if not in the IPv6 local subnet and tunnelling is enabled calculate the checksum
        usCheckSum = fnCalculateTCP_checksum(&network[_NETWORK_ID].ucOurIPV6[0], ptr_TCP->ucRemoteIP, (ptrData - (MIN_TCP_HLEN - 1)), usDataLen, INTERFACE_CALC_TCP_IPv6);
        ptrData -= 3;
        *ptrData++ = (unsigned char)(usCheckSum >> 8);                   // save TCP checksum in correct place
        *ptrData   = (unsigned char)usCheckSum;
    }
#endif

    if ((usDataLen > MIN_TCP_HLEN) && (ptr_TCP->ucTCP_state == TCP_STATE_ESTABLISHED)) { // {26} when data sent in established state (not including control frames, which have no data)
        ptr_TCP->ucRetransmissions = TCP_DEF_RETRIES;                    // standard retries if the message times out
        ptr_TCP->usLinkTimer = ptr_TCP->usIdleTimeout;                   // retrigger the keep alive timer since there is activity
        ptr_TCP->usTransmitTimer = TCP_STANDARD_RETRY_TOUT;              // retransmit timer standard repetition rate
#if defined HIGH_RESOLUTION_TCP_POLLING                                  // {59}
        ptr_TCP->ucLinkDivider = 0;                                      // reset the link timer prescaler
#endif
    }
#if defined USE_IPV6
    if (iIPv6 != 0) {                                                    // {41} send over IPv6 and not IPv4
    #if defined USE_IP_STATS
        fnTxStats(TRANSMITTED_TCPV6_FRAMES, DEFAULT_NETWORK);            // count TCP IPv6 transmissions
    #endif
        return (fnSendIPV6(ptr_TCP->ucRemoteIP, IP_TCP, ptr_TCP->ucTos, ptrBuf, usDataLen, OWN_TASK, TCP_socket)); // send it to IPv6 - note TCP task is always responsible for the delivery (OWN_TASK)
    }
#endif
    return (fnSendIPv4(ptr_TCP->ucRemoteIP, IP_TCP, ptr_TCP->ucTos, MAX_TTL, ptrBuf, usDataLen, OWN_TASK, TCP_socket)); // send it to IPv4 - note TCP task is always responsible for the delivery (OWN_TASK)
}

#if defined SUPPORT_PEER_WINDOW && defined USE_BUFFERED_TCP
// This routine coordinates windowing of a TCP buffer
//
static void fnHandleTxWindowing(TCP_TX_BUFFER *tcp_tx, QUEUE_TRANSFER usTxFrameSize, unsigned short usEscapeLength, unsigned char ucNeg)
{
    tcp_tx->tx_window[tcp_tx->ucPutFrame].usFrameSize = usTxFrameSize;   // the length which we have just sent
    tcp_tx->tx_window[tcp_tx->ucPutFrame].usWindowTimeout = TCP_STANDARD_RETRY_TOUT; // each frame has its own retry timeout
    #if defined USE_TELNET
    tcp_tx->tx_window[tcp_tx->ucPutFrame].usEscapeLength = usEscapeLength; // the number of additional escaped characters sent
    tcp_tx->tx_window[tcp_tx->ucPutFrame].ucNegotiate = ucNeg;           // save the negotiation field
    #endif
    tcp_tx->usOpenBytes += usTxFrameSize;                                // the total amount of bytes that haven't yet been acknowledged
    tcp_tx->ucOpenAcks++;                                                // the number of transmitted (windowed) frames that haven't been acknowledged yet
    if (++tcp_tx->ucPutFrame >= WINDOWING_BUFFERS) {                     // handle circular put frame index
        tcp_tx->ucPutFrame = 0;
    }
}

// Some data has been acked, delete it from the backup buffer
//
static int fnAckedTxWindowing(TCP_TX_BUFFER *tcp_tx, unsigned short usAckedBytes, TCP_CONTROL *ptr_TCP)
{
    if (!tcp_tx->ucOpenAcks) {                                           // if we are not expecting an ack, ignore
        return 1;
    }

    if ((tcp_tx->ucOpenAcks > tcp_tx->ucCwnd) && (tcp_tx->ucCwnd < (WINDOWING_BUFFERS - 1))) { // limit the congestion window to the amount of buffers we can handle
        ++tcp_tx->ucCwnd;
    }

    if (usAckedBytes != 0) {                                             // partial ack possible
        unsigned short usFrameLength;

        usAckedBytes = (tcp_tx->usOpenBytes - usAckedBytes);
        while ((usAckedBytes) && (usAckedBytes >= (usFrameLength = tcp_tx->tx_window[tcp_tx->ucGetFrame].usFrameSize))) {
            usAckedBytes -= usFrameLength;
    #if defined USE_TELNET
            usAckedBytes -= tcp_tx->tx_window[tcp_tx->ucGetFrame].usEscapeLength; // ignore escape sequences since these were not backed up
    #endif
            tcp_tx->usPacketSize -= usFrameLength;
            tcp_tx->ucDataStart += usFrameLength;
            tcp_tx->usOpenBytes -= usFrameLength;
            ptr_TCP->ulSendUnackedNumber += usFrameLength;
            tcp_tx->ucOpenAcks--;
            if (++tcp_tx->ucGetFrame >= WINDOWING_BUFFERS) {
                tcp_tx->ucGetFrame = 0;
            }
            if (tcp_tx->ucGetFrame == tcp_tx->ucPutFrame) {
                break;
            }
        }
    }
    else {
    #if defined USE_TELNET
        while (tcp_tx->ucGetFrame != tcp_tx->ucPutFrame) {
            tcp_tx->usOpenBytes -= tcp_tx->tx_window[tcp_tx->ucGetFrame].usEscapeLength;
            if (++tcp_tx->ucGetFrame >= WINDOWING_BUFFERS) {
                tcp_tx->ucGetFrame = 0;
            }
        }
    #else
        tcp_tx->ucGetFrame = tcp_tx->ucPutFrame;                         // all outstanding data has been acked so synchronise buffers
    #endif
        tcp_tx->ucDataStart += tcp_tx->usOpenBytes;
        tcp_tx->usOpenBytes = 0;
        tcp_tx->ucOpenAcks = 0;
        tcp_tx->usPacketSize = 0;
    }
    return 0;
}

// When multiple windows have been sent, each requires timeout monitoring. The timeout is managed here.
//
static int fnWindowTimeOut(TCP_CONTROL *ptr_TCP)
{
    TCP_TX_BUFFER *tcp_tx = ptr_TCP->ptrTCP_Tx_Buffer;
    int x = WINDOWING_BUFFERS;
    int i;

    if (tcp_tx == 0) {
        return 0;
    }

    i = tcp_tx->ucGetFrame;

    while (x-- != 0) {
        if ((i == tcp_tx->ucPutFrame) && (tcp_tx->ucOpenAcks != WINDOWING_BUFFERS)) { // no more open windows
            return 0;                                                    // no timeout
        }
        if (tcp_tx->tx_window[i].usWindowTimeout != 0) {
            if (--tcp_tx->tx_window[i].usWindowTimeout == 0) {           // count down
                return 1;                                                // timeout detected
            }
        }
        if (++i >= WINDOWING_BUFFERS) {
            i = 0;
        }
    }
    return 0;                                                            // nothing has timed out
}

static void fnResetWindowing(TCP_TX_BUFFER *tcp_tx)
{
    if (tcp_tx == 0) {
        return;
    }
    tcp_tx->ucGetFrame    = 0;
    tcp_tx->ucPutFrame    = 0;
    tcp_tx->ucDataEnd     = tcp_tx->ucDataStart = tcp_tx->ucTCP_tx;
    tcp_tx->usOpenBytes   = 0;
    tcp_tx->ucOpenAcks    = 0;
    tcp_tx->usPacketSize  = 0;
    tcp_tx->usWaitingSize = 0;
    tcp_tx->ucCwnd        = 0;
}
#endif                                                                   // end if defined SUPPORT_PEER_WINDOW && defined USE_BUFFERED_TCP

// Some TCP sockets may require a transmission buffer to improve efficiency - a good example is TELNET where
// characters are often put into the buffer faster than the buffer is freed. They are collected and maximum frames sent whenever possible.
//
#if defined USE_BUFFERED_TCP
    #if defined PROTECT_SEND_BUFFERED_TCP                                // {25}
    #define BUFFERED_TCP_ENTER_CRITICAL() uDisable_Interrupt()
    #define BUFFERED_TCP_LEAVE_CRITICAL() uEnable_Interrupt()
    #else
    #define BUFFERED_TCP_ENTER_CRITICAL()
    #define BUFFERED_TCP_LEAVE_CRITICAL()
    #endif
extern QUEUE_TRANSFER fnSendBufTCP(USOCKET TCP_socket, unsigned char *ptrBuf, unsigned short usDataLen, COMMAND_TYPE Command)
{
    unsigned char *ptrEnd;
    TCP_TX_BUFFER *tcp_tx;
    QUEUE_TRANSFER DataCopied = 0;
    QUEUE_TRANSFER TxFrameSize;
    TCP_CONTROL *ptr_TCP;

    if ((ptr_TCP = fnGetSocketControl(TCP_socket)) == 0) {               // get a pointer to the TCP socket control structure
        return 0;                                                        // error - trying to sent to invalid socket
    }

    #if defined _EXTENDED_BUFFERED_TCP                                   // {38} - allow data to be queued before connection is established
    if ((ptr_TCP->ucTCP_state != TCP_STATE_ESTABLISHED) && ((Command & TCP_BUF_QUEUE) == 0)) {
        return 0;
    }
    #else
    if (ptr_TCP->ucTCP_state != TCP_STATE_ESTABLISHED) {                 // {4}
        return 0;                                                        // if not established throw away data
    }
    #endif

    tcp_tx = ptr_TCP->ptrTCP_Tx_Buffer;

    BUFFERED_TCP_ENTER_CRITICAL();                                       // in systems where TCP frames can be sent from within interrupt routines this region is protected from interrupts

    #if !defined IMMEDIATE_MEMORY_ALLOCATION                             // {55} if the immediate memeory option is enabled the allocation is performed when the socket is created
    if (tcp_tx == 0) {                                                   // we get working space from heap only when we actually use it (we assume such sockets are never released)
        if ((tcp_tx = (TCP_TX_BUFFER *)TCP_BUFFER_MALLOC(sizeof(TCP_TX_BUFFER))) == 0) {
            BUFFERED_TCP_LEAVE_CRITICAL();                               // leave potentially critical region
            return 0;                                                    // couldn't get required memory from heap!
        }
        #if defined INDIVIDUAL_BUFFERED_TCP_BUFFER_SIZE                  // {34}
        if (ptr_TCP->usTCP_buffer_length == 0) {                         // no value has been entered for this socket
            ptr_TCP->usTCP_buffer_length = STANDARD_BUFFER_SIZE;         // ensure compatibility when user doesn't specify a value
        }
        if ((tcp_tx->ucTCP_tx = TCP_BUFFER_MALLOC(ptr_TCP->usTCP_buffer_length)) == 0) { // get the buffer memory associated with the buffered TCP socket
            BUFFERED_TCP_LEAVE_CRITICAL();                               // leave potentially critical region
            return 0;                                                    // couldn't get required memory from heap!
        }
        #endif
        ptr_TCP->ptrTCP_Tx_Buffer = tcp_tx;                              // initialise/flush buffer
        tcp_tx->ucDataEnd = tcp_tx->ucDataStart = tcp_tx->ucTCP_tx;
      //tcp_tx->usPacketSize = tcp_tx->usWaitingSize = 0;                // cleared automatically by uMalloc
    }
    #endif
    #if defined WAKE_BLOCKED_TCP_BUF
    if ((Command & TCP_BUF_CHECK) != 0) {                                // the caller doesn't want to send the data yet but wishes to check that there is enough space for it
        if (tcp_tx->usWaitingSize + tcp_tx->usPacketSize + usDataLen > _TCP_BUFFER) { // space not adequate to add the data
            if (ptrBuf != 0) {                                           // {27} if a handling task is being defined enter this to be informed when the buffer becomes free
                tcp_tx->WakeTask = *ptrBuf;                              // mark task to wake when buffer free
            }
            BUFFERED_TCP_LEAVE_CRITICAL();                               // leave potentially critical region
            return 0;                                                    // not enough space to send intended buffer
        }
        else {
            QUEUE_TRANSFER available_space = (_TCP_BUFFER - (tcp_tx->usWaitingSize + tcp_tx->usPacketSize)); // return the space available (before putting the data in)
            BUFFERED_TCP_LEAVE_CRITICAL();                               // leave potentially critical region
            return available_space;
        }
    }
    #endif
    ptrEnd = (tcp_tx->ucTCP_tx + _TCP_BUFFER);                           // add data to buffer as long as there is space for it

    if (ptrBuf != 0) {                                                   // data pointer available
        QUEUE_TRANSFER Remaining_space = (_TCP_BUFFER - (tcp_tx->usWaitingSize + tcp_tx->usPacketSize));
        if (usDataLen > Remaining_space) {                               // if there is not space for the whole message we crop it
            usDataLen = Remaining_space;
        }
        DataCopied += usDataLen;
        if ((tcp_tx->ucDataEnd + usDataLen) >= ptrEnd) {                 // we need a double copy
            QUEUE_TRANSFER FirstCopy = (QUEUE_TRANSFER)(ptrEnd - tcp_tx->ucDataEnd);
            uMemcpy(tcp_tx->ucDataEnd, ptrBuf, FirstCopy);
            uMemcpy(tcp_tx->ucTCP_tx, (ptrBuf + FirstCopy), (usDataLen -= FirstCopy));
            tcp_tx->ucDataEnd = tcp_tx->ucTCP_tx;
        }
        else {
            uMemcpy(tcp_tx->ucDataEnd, ptrBuf, usDataLen);               // we can do it with a single copy
        }
        tcp_tx->ucDataEnd += usDataLen;                                  // set the end of the waiting data

    #if defined _EXTENDED_BUFFERED_TCP
        if ((ptr_TCP->ucTCP_state != TCP_STATE_ESTABLISHED) || ((Command & TCP_BUF_PREPARE) != 0)) { // if the connection has not yet been established {72} or the caller wishes to prepare the buffer before sending
            tcp_tx->usWaitingSize += usDataLen;                          // extra waiting data size
            return 0;                                                    // data added to buffer but no transmission started since the connection is not (yet) open
        }
    #endif
    }

    if ((Command & TCP_BUF_NEXT) != 0) {                                 // we can delete the old data since it has been acked and any remaining can be sent
    #if defined SUPPORT_PEER_WINDOW
        if ((((TCP_BUF_KICK_NEXT | TCP_BUF_SEND) & Command) == 0) &&     // don't acknowledge anything in these cases
                  (fnAckedTxWindowing(tcp_tx, usDataLen, ptr_TCP) != 0)) {
            BUFFERED_TCP_LEAVE_CRITICAL();                               // leave potentially critical region
            return 0;                                                    // ack not recognised so ignore it
        }
    #else
        tcp_tx->ucDataStart += tcp_tx->usPacketSize;
        tcp_tx->usPacketSize = 0;
    #endif
    #if defined MJBC_TEST4
        if (usDataLen == 0) {                                            // no further outstanding data to be acknowledged
            if (ptr_TCP->ucTCPInternalFlags & SILLY_WINDOW_AVOIDANCE) {  // if acknowledgement to all outstanding data before entering the silly window avoidance state
                ptr_TCP->usTransmitTimer = TCP_SILLY_WINDOW_DELAY;       // probe after this delay if the peer doesn't inform of its reception window opening
                if ((TCP_BUF_SEND & Command) != 0) {                     // unconditional probe when we are in the silly window avoidance state
                    return 0;                                            // normally a window probe would be sent here this is not implemented
                }
            }
        }
    #endif
        if (tcp_tx->ucDataStart >= ptrEnd) {                             // handle wrap-around of pointer to start of not yet acknowledged data
            tcp_tx->ucDataStart -= _TCP_BUFFER;
        }
        DataCopied = tcp_tx->usWaitingSize;                              // the amount of data in the buffer waiting to be sent
        tcp_tx->usWaitingSize = 0;                                       // reset the waiting size, assuming that all will be sent (will be corrected if not possible to send all)
    #if defined WAKE_BLOCKED_TCP_BUF
        if (DataCopied == 0) {                                           // if the buffer is already empty
            if (tcp_tx->WakeTask != 0) {                                 // and the application is waiting for confirmation of the buffer being free
                //unsigned char tx_continue_message[HEADER_LENGTH] = {INTERNAL_ROUTE, INTERNAL_ROUTE, tcp_tx->WakeTask, INTERRUPT_EVENT, TX_FREE};
                unsigned char tx_continue_message[HEADER_LENGTH];
                tx_continue_message[MSG_DESTINATION_NODE] = INTERNAL_ROUTE;
                tx_continue_message[MSG_SOURCE_NODE] = INTERNAL_ROUTE;
                tx_continue_message[MSG_DESTINATION_TASK] = tcp_tx->WakeTask;
                tx_continue_message[MSG_SOURCE_TASK] = INTERRUPT_EVENT;
                tx_continue_message[MSG_INTERRUPT_EVENT] = TX_FREE;
                fnWrite(INTERNAL_ROUTE, tx_continue_message, HEADER_LENGTH); // inform the blocked task
            }
        }
    #endif
    }
    else if ((Command & TCP_BUF_REP) != 0) {
        if ((TCP_REPEAT_WINDOW & Command) != 0) {
            DataCopied = tcp_tx->tx_window[tcp_tx->ucPutFrame].usFrameSize;
            Command = tcp_tx->tx_window[tcp_tx->ucPutFrame].ucNegotiate;
        }
        else {
            // We have to repeat all outstanding frames. We can however combine them if we want, but beware of negotiation frames...
            // The simplest solution (when no negs) is to clear all window buffers and send the largest frame as possible.
            // If we find neg blocks we must force these to be repeated at the next possibility
            //
            int iRepeatSimple = 0;
            unsigned short ucTheseWindowSizes = 0;
            while (tcp_tx->ucOpenAcks-- != 0) {
                if (tcp_tx->ucPutFrame-- == 0) {
                    tcp_tx->ucPutFrame = (WINDOWING_BUFFERS-1);
                }
                if ((iRepeatSimple != 0) || ((tcp_tx->tx_window[tcp_tx->ucPutFrame].ucNegotiate & TCP_CONTENT_NEGOTIATION) != 0)) {
                    tcp_tx->tx_window[tcp_tx->ucPutFrame].usWindowTimeout = 1; // repeat as soon as possible if necessary
                    iRepeatSimple++;
                }
                ucTheseWindowSizes += tcp_tx->tx_window[tcp_tx->ucPutFrame].usFrameSize;
            }
            tcp_tx->ucOpenAcks = 0;
            tcp_tx->usOpenBytes = tcp_tx->usPacketSize = 0;              // no more outstanding acks
            if (iRepeatSimple == 0) {
                DataCopied = ucTheseWindowSizes;
                Command = TCP_BUF_SEND;                                  // kick off the largest buffer transfer possible
            }
            else {
                QUEUE_TRANSFER rtnSent = 0;
                tcp_tx->usWaitingSize += ucTheseWindowSizes;
                while ((iRepeatSimple-- != 0) && ((DataCopied = fnSendBufTCP(TCP_socket, 0, 0, (TCP_BUF_REP | TCP_REPEAT_WINDOW))) != 0)) {
                    rtnSent |= DataCopied;
                }
                BUFFERED_TCP_LEAVE_CRITICAL();                           // leave potentially critical region
                return rtnSent;
            }
        }
    }

    #if defined SUPPORT_PEER_WINDOW                                      // send the largest packet possible
    if ((DataCopied != 0) && (tcp_tx->ucCwnd >= tcp_tx->ucOpenAcks) && (tcp_tx->ucOpenAcks < WINDOWING_BUFFERS))  // we can try to send if we have something waiting and congestion window is adequately open (slow start)
    #else
    if ((tcp_tx->usPacketSize == 0) && (DataCopied))                     // we are not waiting for an ack so can send the block
    #endif
    {
        unsigned char ucTCP_data_buf[TCP_BUFFER_FRAME + MIN_TCP_HLEN];   // temporary buffer space
        unsigned char *ptrTxBuffer = &ucTCP_data_buf[MIN_TCP_HLEN];      // location of the start of payload data in the frame to be sent
        unsigned char *ptrIn = tcp_tx->ucDataStart;                      // the start of data to be sent
        unsigned short usEscapeLength = 0;                               // the length of escaped data (only when binary data sent over an ASCII TELNET connection)
        signed short ssReturn;
    #if defined USE_TELNET
        QUEUE_TRANSFER TxFrameLimit = TCP_BUFFER_FRAME;
    #endif

        tcp_tx->usWaitingSize += DataCopied;                             // the new amount of data waiting to be transmitted

        if (DataCopied > TCP_BUFFER_FRAME) {                             // ensure we don't overrun the single frame buffer
            DataCopied = TCP_BUFFER_FRAME;                               // limit the payload to the physical buffer size
        }

    #if defined SUPPORT_PEER_MSS
        if (DataCopied > ptr_TCP->usPeerMSS) {                           // check that the peer's maximum segment can accept the payload length
            DataCopied = ptr_TCP->usPeerMSS;                             // don't send individual frames longer than the Maximum Seqment Size of the peer
        #if defined USE_TELNET
            TxFrameLimit = DataCopied;
        #endif
        }
    #endif
    #if defined SUPPORT_PEER_WINDOW
        if (DataCopied > ptr_TCP->usTxWindowSize) {                      // check that the payload can fit into the peer's open receive window
            DataCopied = ptr_TCP->usTxWindowSize;                        // don't send more data than the receiver can accept to its input window
        #if defined USE_TELNET
            TxFrameLimit = DataCopied;
        #endif
        }
        #if defined SUPPORT_PEER_MSS
            #if defined MJBC_TEST4
        if (ptr_TCP->usTxWindowSize < ptr_TCP->usPeerMSS/2) {            // peers reception window is critical
            if (DataCopied < tcp_tx->usWaitingSize) {                    // the window is not adequate to send all waiting date                
                ptr_TCP->ucTCPInternalFlags |= SILLY_WINDOW_AVOIDANCE;   // mark that the transmission has been paused until the peer's reception window opens adequately
                if (tcp_tx->ucOpenAcks == 0) {                           // if no transmitted data expects acks
                    ptr_TCP->usTransmitTimer = TCP_SILLY_WINDOW_DELAY;   // probe if the peer doesn't inform of its reception window opening
                }
                return 0;                                                // don't send to avoid silly-window syndrome
            }
        }
        ptr_TCP->ucTCPInternalFlags &= ~SILLY_WINDOW_AVOIDANCE;          // clear silly window state flag
            #else
        if (((ptr_TCP->ucTCPInternalFlags & SILLY_WINDOW_AVOIDANCE) == 0) && (ptr_TCP->usTxWindowSize < ptr_TCP->usPeerMSS/2)) {
            DataCopied = 0;                                              // wait with sending data since the peer's receiver is critically low
            ptr_TCP->usTransmitTimer = TCP_SILLY_WINDOW_DELAY;
            ptr_TCP->ucTCPInternalFlags |= SILLY_WINDOW_AVOIDANCE;
        }
        else {
            ptr_TCP->ucTCPInternalFlags &= ~SILLY_WINDOW_AVOIDANCE;
        }
            #endif
        #endif
        ptr_TCP->usTxWindowSize -= DataCopied;                           // update our copy of the peer's window size since we are sending this amount of raw data
        ptrIn += tcp_tx->usPacketSize;
        if (ptrIn >= ptrEnd) {                                           // handle circular buffer
            ptrIn -= _TCP_BUFFER;
        }
        tcp_tx->usPacketSize += DataCopied;                              // the length of presently non-acked data
    #else
        tcp_tx->usPacketSize = DataCopied;
    #endif
        TxFrameSize = DataCopied;                                        // this frame size
        tcp_tx->usWaitingSize -= DataCopied;                             // remaining waiting data in buffer after transmitting this frame
    #if defined USE_TELNET
        if (((Command & TCP_CONTENT_NEGOTIATION) == 0) && (fnCheckTelnetBinaryTx(TCP_socket) != 0)) {
            QUEUE_TRANSFER TxStuffedFrameSize = 0;                       // we must perform TELNET_IAC stuffing on outgoing data for the TELNET connection in binary mode
            while (DataCopied) {                                         // for each byte in the buffer
                if (*ptrIn == TELNET_IAC) {                              // is it an IAC character?
                    if (TxStuffedFrameSize >= (TxFrameLimit - 1)) {      // stop if there is not enough space for stuffing this time around
                        break;
                    }
                    *ptrTxBuffer++ = TELNET_IAC;                         // stuff IAC
                    TxStuffedFrameSize++;                                // to monitor that the frame doesn't grow too large when stuffing
                    usEscapeLength++;                                    // count the number of escaped characters in this frame
                }
                *ptrTxBuffer++ = *ptrIn++;
                DataCopied--;
                if (ptrIn >= ptrEnd) {                                   // handle wrap in input buffer
                    ptrIn = tcp_tx->ucTCP_tx;
                }
                if (++TxStuffedFrameSize >= TxFrameLimit) {              // if buffer full due to stuffed bytes quit
                    break;
                }
            }
            TxFrameSize = TxStuffedFrameSize;                            // the actual size of frame to send (can have grown due to stuffing)
            tcp_tx->usWaitingSize += DataCopied;                         // compensate for any byte not sent this time around so they will be sent in next frame
        }
        else {                                                           // copy from circular buffer to linear output buffer
            while (DataCopied-- != 0) {                                  // without doing any IAC stuffing
                *ptrTxBuffer++ = *ptrIn++;                               // copy from the TCP buffer to the frame transmission buffer
                if (ptrIn >= ptrEnd) {                                   // handle wrap around in the TCP buffer
                    ptrIn = tcp_tx->ucTCP_tx;
                }
            }
        }
    #else
        while (DataCopied-- != 0) {                                      // copy from circular buffer to linear output buffer
            *ptrTxBuffer++ = *ptrIn++;                                   // without doing any IAC stuffing
            if (ptrIn >= ptrEnd) {
                ptrIn = tcp_tx->ucTCP_tx;
            }
        }
    #endif
        if ((ssReturn = fnSendTCP(TCP_socket, ucTCP_data_buf, TxFrameSize, TCP_FLAG_PUSH)) > TxFrameSize) { // send a TCP frame
            ssReturn = TxFrameSize;                                      // as long as we sent more characters than we passed (IP will be added) we return the TCP frame length
        }
    #if defined SUPPORT_PEER_WINDOW
        if ((Command & TCP_BUF_REP) == 0) {                              // as long as this wasn't a repetition
            fnHandleTxWindowing(tcp_tx, TxFrameSize, usEscapeLength, (unsigned char)Command); // log in details for frame management
        }
    #endif
        BUFFERED_TCP_LEAVE_CRITICAL();                                   // leave potentially critical region
    #if defined MJBC_TEST3                                               // test sending as much open data that can be accepted
        if ((ssReturn == TxFrameSize) && (tcp_tx->usWaitingSize != 0)) { // a frame was successfully sent - now try to send more windows if there is still data waiting
            fnSendBufTCP(TCP_socket, 0, 0, (TCP_BUF_NEXT | TCP_BUF_KICK_NEXT)); // this recursive call will allow a number of frames to be sent up to the allowed limit
        }
    #endif
        return ssReturn;                                                 // return the length of the first frame sent
    }

    tcp_tx->usWaitingSize += DataCopied;                                 // no data transmitted but waiting queue size increased accordingly
    BUFFERED_TCP_LEAVE_CRITICAL();                                       // leave potentially critical region
    if ((TCP_BUF_SEND_REPORT_COPY & Command) != 0) {                     // if the user wants to know how many bytes have been queued we return the value
        return DataCopied;
    }
    return 0;                                                            // we haven't sent a TCP frame
}

    #if defined INDIVIDUAL_BUFFERED_TCP_BUFFER_SIZE || defined IMMEDIATE_MEMORY_ALLOCATION // {34}{55}
// Enter the buffer size associated with a buffered TCP socket
//
extern unsigned short fnDefineTCPBufferSize(USOCKET TCP_socket, unsigned short usBufferSize)
{
    TCP_CONTROL *ptr_TCP;

    if ((ptr_TCP = fnGetSocketControl(TCP_socket)) == 0) {               // get a pointer to the TCP socket control structure
        return 0;                                                        // error - trying to sent to invalid socket
    }
    #if defined IMMEDIATE_MEMORY_ALLOCATION                              // {55}
            #if defined INDIVIDUAL_BUFFERED_TCP_BUFFER_SIZE
    if (ptr_TCP->usTCP_buffer_length == 0) {                             // if the buffer has not yet been used
        if ((ptr_TCP->ptrTCP_Tx_Buffer = (TCP_TX_BUFFER *)uMalloc(sizeof(TCP_TX_BUFFER))) != 0) {
            if ((ptr_TCP->ptrTCP_Tx_Buffer->ucTCP_tx = uMalloc(usBufferSize)) != 0) { // get the buffer memory associated with the buffered TCP socket
                ptr_TCP->ptrTCP_Tx_Buffer->ucDataEnd = ptr_TCP->ptrTCP_Tx_Buffer->ucDataStart = ptr_TCP->ptrTCP_Tx_Buffer->ucTCP_tx;
              //ptr_TCP->ptrTCP_Tx_Buffer->usPacketSize = ptr_TCP->ptrTCP_Tx_Buffer->usWaitingSize = 0; // cleared automatically by uMalloc
                ptr_TCP->usTCP_buffer_length = usBufferSize;
            }
        }
    }
            #else                                                        // {55}
    ptr_TCP->ptrTCP_Tx_Buffer = (TCP_TX_BUFFER *)uMalloc(sizeof(TCP_TX_BUFFER));
            #endif
    #else
    if (ptr_TCP->usTCP_buffer_length == 0) {                             // if the buffer has not yet been used
        ptr_TCP->usTCP_buffer_length = usBufferSize;                     // allow its size to be modified
    }
    #endif
    return ptr_TCP->usTCP_buffer_length;                                 // return the buffers (new) size
}
    #endif
#endif                                                                   // end USE_BUFFERED_TCP

// Send a control frame
//
static signed short fnSendTCPControl(TCP_CONTROL *ptr_TCP)
{
    unsigned char ucTCP_control_buf[MIN_TCP_HLEN];                       // temporary control buffer space
#if defined USE_SECURE_SOCKET_LAYER                                      // {69}
    return (fnSendTCP((ptr_TCP->MySocketNumber & ~(SECURE_SOCKET_MODE)), ucTCP_control_buf, 0, 0)); // send a control frame without data
#else
    return (fnSendTCP(ptr_TCP->MySocketNumber, ucTCP_control_buf, 0, 0)); // send a control frame without data
#endif
}

static void fnRestartTCP_timer(void)                                     // {58}
{
    if (iTCP_timer_active == 0) {                                        // if the TCP polling is not active it it started
        uTaskerMonoTimer(OWN_TASK, T_TCP_PERIOD, E_POLL_TCP);
        iTCP_timer_active = 1;                                           // the TCP timer is now active
    }
}

// Set a new TCP state - suitably configuring timers
//
static void fnNewTCPState(TCP_CONTROL *ptr_TCP, unsigned char ucNextState)
{
    ptr_TCP->ucRetransmissions = TCP_DEF_RETRIES;                        // standard retries if the message times out

    switch (ptr_TCP->ucTCP_state = ucNextState) {
        case TCP_STATE_TIME_WAIT:                                        // we do not want to block applications in this state so we give them the chance to reuse the socket immediately if they want to
            ptr_TCP->event_listener(_TCP_SOCKET_MASK(ptr_TCP->MySocketNumber), TCP_EVENT_CLOSED, ptr_TCP->ucRemoteIP, ptr_TCP->usRemport); // {56}
            ptr_TCP->usTransmitTimer = (2 * TCP_MSL_TIMEOUT);
            // Fall through intentional
            //
#if defined SUPPORT_PEER_WINDOW
    #if defined USE_BUFFERED_TCP
        case TCP_STATE_CLOSED:
            fnResetWindowing(ptr_TCP->ptrTCP_Tx_Buffer);                 // ensure windowing reset ready for next use
    #endif
#endif
#if defined USE_OUT_OF_ORDER_TCP_RX_BUFFER
            if (ptr_TCP == OutOfOrder.ptr_TCP) {                         // {65} if this socket owns the out-of-order buffer
                OutOfOrder.ucTTL = 0;                                    // invalidate the buffer to ensure that it can be reused by other sockets
            }
#endif
            return;

        case TCP_STATE_FIN_WAIT_2:
            ptr_TCP->usTransmitTimer = TCP_FIN_WAIT_2_TIMEOUT;           // it is possible for a socket to block in this state. We don't allow this to happen and reset after a reasonable delay
            ptr_TCP->ucRetransmissions = 0;                              // note that some browsers remain in this state while a password is entered so we don't set it too short...
            return;

        case TCP_STATE_CLOSE_WAIT:
        case TCP_STATE_LAST_ACK:
        case TCP_STATE_FIN_WAIT_1:
        case TCP_STATE_CLOSING:
            ptr_TCP->ucRetransmissions = 1;
            break;

        case TCP_STATE_ESTABLISHED:
            ptr_TCP->usLinkTimer = ptr_TCP->usIdleTimeout;               // retrigger the keep alive timer since there is activity on the connection
#if defined HIGH_RESOLUTION_TCP_POLLING                                  // {59}
            ptr_TCP->ucLinkDivider = 0;                                  // reset the link timer prescaler
#endif
            break;

        case TCP_STATE_SYN_RCVD:                                         // {58}
        case TCP_STATE_SYN_SENT:                                         // {58}
            fnRestartTCP_timer();                                        // {58} start polling timer when connectiong or when being connected to 
            break;

        default:                                                         // all other states use standard settings
            break;
    }
    ptr_TCP->usTransmitTimer = TCP_STANDARD_RETRY_TOUT;                  // retransmit timer standard repetition rate
}


// Each time a TCP connection is established a sequence number must be initialised. RFC 793 states that this
// this can be viewed as a 32 bit counter incremented every 4us.
// we use our system tick as base and convert it to an equivalent value based on the fact that the tick is incremented once every RES ms
//
static unsigned long fnGetTCP_init_seq(void)
{
    static unsigned char ucSequenceUses = 0;                             // this variable ensures different sequence numbers for multiple connections started in one tick interval
                                                                         // in this period the counter would count RESms/4us == RES*1000/4 or RES*250
#if TICK_RESOLUTION >= 1000                                              // {68}
    return (((uTaskerSystemTick + ucSequenceUses++) * ((TICK_RESOLUTION/1000) * 250)));
#else
    return (((uTaskerSystemTick + ucSequenceUses++) * (250/(1000/TICK_RESOLUTION))));
#endif
}

// Client connect
//
#if defined USE_IPV6
    extern USOCKET fnTCP_Connect(USOCKET TCP_socket, unsigned char *RemoteIP, unsigned short usRemotePort, unsigned short usOurPort, unsigned long ulMaxWindow_flags) // {44}
#else
    extern USOCKET fnTCP_Connect(USOCKET TCP_socket, unsigned char *RemoteIP, unsigned short usRemotePort, unsigned short usOurPort, unsigned short usMaxWindow)
#endif
{
    TCP_CONTROL *ptr_TCP;
#if defined USE_IPV6
    unsigned short usMaxWindow = (unsigned short)ulMaxWindow_flags;      // extract the length
#endif

    if ((ptr_TCP = fnGetSocketControl(TCP_socket)) == 0) {               // get a pointer to the TCP socket control structure
        return (SOCKET_NOT_FOUND);                                       // {17}
    }

    if (usOurPort == 0) {
        if ((usOurPort = fnGetFreeTCP_Port()) == 0) {
            return (NO_FREE_PORTS_AVAILABLE);
        }
    }

#if defined USE_SECURE_SOCKET_LAYER                                      // {69}
    if ((TCP_socket & SECURE_SOCKET_MODE) != 0) {                        // if the connection is over secure socket layer
        ptr_TCP->event_listener = (int(*)(USOCKET, unsigned char, unsigned char *, unsigned short))fnInsertSecureLayer(TCP_socket, ptr_TCP->event_listener, 1); // insert the secure socket layer between TCP and the application                   
    }
    else {                                                               // else bypass secure layer
        ptr_TCP->event_listener = (int(*)(USOCKET, unsigned char, unsigned char *, unsigned short))fnInsertSecureLayer(TCP_socket, ptr_TCP->event_listener, 0); // remove secure layer in case it was previously installed
    }
#endif

    if ((ptr_TCP->ucTCP_state & (TCP_STATE_RESERVED | TCP_STATE_LISTEN | TCP_STATE_CLOSED)) == 0) { // are we in an invalid state?
#if defined REUSE_TIME_WAIT_SOCKETS
        // It has been observed that if a final FIN is missed while in the TCP_STATE_FIN_WAIT_2, the socket
        // remains in the TCP_STATE_FIN_WAIT_2 forever. This can quite easily happen when the FIN is corrupted
        // or the partner is powered down at this moment. There is no ack and no rep to the final fin!!
        // So that the socket is not blocked forever we also allow it to be reused here...
        //
        if (ptr_TCP->ucTCP_state & (TCP_STATE_FIN_WAIT_2 | TCP_STATE_TIME_WAIT)) {
            int iTimeWait2 = ((ptr_TCP->ucTCP_state & TCP_STATE_FIN_WAIT_2) != 0);
            fnNewTCPState(ptr_TCP, TCP_STATE_CLOSED);
            if (iTimeWait2 != 0) {                                       // {71} only inform of close if the socket was is the TCP_STATE_FIN_WAIT_2 state
                ptr_TCP->event_listener(_TCP_SOCKET_MASK(ptr_TCP->MySocketNumber), TCP_EVENT_CLOSED, ptr_TCP->ucRemoteIP, ptr_TCP->usRemport); // {56}
            }
        }
        else {
            return (SOCKET_STATE_INVALID);
        }
#else
        return (SOCKET_STATE_INVALID);                                   // socket in invalid state to initialise CONNECT
#endif
    }

#if defined HTTP_WINDOWING_BUFFERS || defined FTP_DATA_WINDOWS
    ptr_TCP->usOpenCnt = 0;                                              // {57}
#endif
#if defined USE_IPV6
    if (ulMaxWindow_flags & TCP_CONNECT_IPV6) {                          // IPv6 connection requested
        uMemcpy(ptr_TCP->ucRemoteIP, RemoteIP, IPV6_LENGTH);             // set remote IPv6 address
        ptr_TCP->ucTCPInternalFlags = TCP_OVER_IPV6;                     // mark that this connection will be over IPv6 and reset other flags
    }
    else {
        uMemcpy(ptr_TCP->ucRemoteIP, RemoteIP, IPV4_LENGTH);             // set remote IPv4 address
        ptr_TCP->ucTCPInternalFlags = 0;                                 // reset all internal flags
    }
#else
    uMemcpy(ptr_TCP->ucRemoteIP, RemoteIP, IPV4_LENGTH);                 // set parameters and send SYN
    ptr_TCP->ucTCPInternalFlags = 0;                                     // reset all internal flags
#endif
    ptr_TCP->usRemport = usRemotePort;
    ptr_TCP->usLocport = usOurPort;
#if defined CONTROL_WINDOW_SIZE
    if (0 == usMaxWindow) {
        ptr_TCP->usRxWindowSize = TCP_MAX_WINDOW_SIZE;                   // set standard window size if application doesn't care
    }
    else {
        ptr_TCP->usRxWindowSize = usMaxWindow;                           // if application sepcifies a special windows size, set it here
    }
#endif
                                                                         // get initial sequence number
    ptr_TCP->ulSendUnackedNumber = fnGetTCP_init_seq();                  // we are establishing a connection so get a sequence number
    ptr_TCP->ulNextTransmissionNumber = (ptr_TCP->ulSendUnackedNumber + 1);
    ptr_TCP->MySocketNumber = TCP_socket;
#if defined ANNOUNCE_MAX_SEGMENT_SIZE
    fnSendSyn(ptr_TCP, TCP_FLAG_SYN);                                    // send SYN with MSS announcement
#else
    ptr_TCP->ucSendFlags = TCP_FLAG_SYN;
    fnSendTCPControl(ptr_TCP);
#endif
    fnNewTCPState(ptr_TCP, TCP_STATE_SYN_SENT);
    return (TCP_socket);
}


// Close a TCP connection
//
extern USOCKET fnTCP_close(USOCKET TCP_Socket)
{
    TCP_CONTROL *ptr_TCP;

    if ((ptr_TCP = fnGetSocketControl(TCP_Socket)) == 0) {               // get a pointer to the TCP socket control structure
        return SOCKET_NOT_FOUND;                                         // {17}
    }

    switch (ptr_TCP->ucTCP_state) {
        case TCP_STATE_SYN_RCVD:
            ptr_TCP->ulSendUnackedNumber++;
            ptr_TCP->ulNextTransmissionNumber++;
            ptr_TCP->ucSendFlags = (TCP_FLAG_ACK | TCP_FLAG_FIN);
            fnSendTCPControl(ptr_TCP);
            fnNewTCPState(ptr_TCP, TCP_STATE_FIN_WAIT_1);
            break;

        case TCP_STATE_SYN_SENT:
            ptr_TCP->event_listener(_TCP_SOCKET_MASK(ptr_TCP->MySocketNumber), TCP_EVENT_CLOSED, ptr_TCP->ucRemoteIP, ptr_TCP->usRemport); // {14}{56}
            // Fall through intentional
            //
        case TCP_STATE_LISTEN:
            fnNewTCPState(ptr_TCP, TCP_STATE_CLOSED);
            break;

        case TCP_STATE_ESTABLISHED:
            if (ptr_TCP->ulSendUnackedNumber == ptr_TCP->ulNextTransmissionNumber) { // is there unacked data?
                ptr_TCP->ulNextTransmissionNumber++;                     // there is no unacked data so we close immediately
                if ((ptr_TCP->ucSendFlags & TCP_FLAG_FIN_RECEIVED) != 0) { // {67}
                    ptr_TCP->ulNextReceptionNumber++;                    // acknowledge the peer's FIN during a simultaneous close
                }
                ptr_TCP->ucSendFlags = (TCP_FLAG_ACK | TCP_FLAG_FIN);    // we set the ACK flag to ensure that any unacked data which led to us closing is acknowledged
                fnSendTCPControl(ptr_TCP);
                fnNewTCPState(ptr_TCP, TCP_STATE_FIN_WAIT_1);
            }
            else {                                                       // a close has been requested although we still have unacknowledged data
                ptr_TCP->ucTCPInternalFlags |= TCP_CLOSEPENDING;         // we signal that we want to close as soon as all data has been acknowledged
            }
            break;

        case TCP_STATE_FIN_WAIT_1:
        case TCP_STATE_FIN_WAIT_2:
        case TCP_STATE_CLOSING:
        case TCP_STATE_TIME_WAIT:
        case TCP_STATE_LAST_ACK:
            break;                                                       // already closing

        default:
            return (SOCKET_STATE_INVALID);
    }
    return (TCP_Socket);
}

// This routine searches for a socket to handle the port address
//
#if defined USE_IPV6
static TCP_CONTROL *fnMapTCPSocket(ETHERNET_FRAME *ptrRx_frame, TCP_PACKET *tcp_frame, unsigned char *source_IP_address, int iIPv6) // {51}
#else
static TCP_CONTROL *fnMapTCPSocket(ETHERNET_FRAME *ptrRx_frame, TCP_PACKET *tcp_frame, unsigned char *source_IP_address) // {51}
#endif
{
    USOCKET Socket = 0;
    TCP_CONTROL *ptr_TCP = tTCP;
    int iSynFrame = 0;
    int ipLength = IPV4_LENGTH;
#if defined REUSE_TIME_WAIT_SOCKETS
    USOCKET ReuseSocket = -1;
#endif

    if (tTCP == 0) {
        return 0;                                                        // protect if no TCP sockets opened
    }

#if defined USE_IPV6
    if (iIPv6 != 0) {
        ipLength = IPV6_LENGTH;
    }
#endif
                                                                         // we check to see whether we are receiving a connection initialisation synchronise frame
    if ((tcp_frame->usHeaderLengthAndFlags & (TCP_FLAG_ACK | TCP_FLAG_RESET | TCP_FLAG_FIN | TCP_FLAG_SYN)) == TCP_FLAG_SYN) {
#if defined TCP_CHECK_SYS_REPS                                           // {12} check whether a SYN + ACK may already have been sent (meaning it was lost). Then re-use this socket to avoid it repeating
        for ( ; Socket < NO_OF_TCPSOCKETS; Socket++) {
            if ((ptr_TCP->ucTCP_state == TCP_STATE_SYN_RCVD) && (ptr_TCP->usLocport == tcp_frame->usDestPort) && (ptr_TCP->usRemport == tcp_frame->usSourcePort)) {
                ptr_TCP->ucTCP_state = TCP_STATE_LISTEN;                 // set back to listening so that it accepts the repeated SYN
                goto _syn_frame_rpt;
            }
            ptr_TCP++;
        }
        Socket = 0;
        ptr_TCP = tTCP;
_syn_frame_rpt:
#endif
        iSynFrame = 1;                                                   // flag that we are handling a SYN frame
    }

    for (; Socket < NO_OF_TCPSOCKETS; Socket++) {
        if (ptr_TCP->usLocport == tcp_frame->usDestPort) {               // we are looking for a TCP socket to accept on this port
            if (ptr_TCP->ucTCP_state == TCP_STATE_LISTEN) {              // a listening port is only interested in a SYN
                if (iSynFrame != 0) {                                    // check whether we are handling a SYN
                    ptr_TCP->usRemport = tcp_frame->usSourcePort;        // we bind the socket on SYN reception
                    uMemcpy(ptr_TCP->ucRemoteIP, source_IP_address, ipLength);
                    ptr_TCP->MySocketNumber = Socket;
                    addNetworkInterface(ptr_TCP->MySocketNumber, ptrRx_frame->ucNetworkID, ptrRx_frame->ucInterface); // {51} add the interface that established the connection
                    addVLAN(ptr_TCP->MySocketNumber, ptrRx_frame->ucVLAN_content); // {53}
                    return (ptr_TCP);                                    // return pointer to the newly bound socket
                }
            }
            else {
#if defined REUSE_TIME_WAIT_SOCKETS
                if (iSynFrame != 0) {
                    if (ptr_TCP->ucTCP_state == (TCP_STATE_FIN_WAIT_2 | TCP_STATE_TIME_WAIT)) { // {18}
                        ReuseSocket = Socket;                            // mark that we could use this socket in an emergency
                    }
                }
#endif
                if (ptr_TCP->usRemport == tcp_frame->usSourcePort) {     // check that the source port corresponds
                    if ((uMemcmp(ptr_TCP->ucRemoteIP, source_IP_address, ipLength)) == 0) { // and the source IP
                        return (ptr_TCP);                                // return pointer to the active socket
                    }
                }
            }
        }
        ptr_TCP++;
    }
#if defined REUSE_TIME_WAIT_SOCKETS                                      // {2}
    if (ReuseSocket >= 0) {                                              // there was no free socket to accept a new connection but there is one in TCP_STATE_FIN_WAIT_2
        ptr_TCP = fnGetSocketControl(ReuseSocket);
        fnNewTCPState(ptr_TCP, TCP_STATE_CLOSED);                        // close the socket (prematurely)
        ptr_TCP->event_listener(_TCP_SOCKET_MASK(ptr_TCP->MySocketNumber), TCP_EVENT_CLOSED, ptr_TCP->ucRemoteIP, 0); // {56}
        ptr_TCP->usRemport = tcp_frame->usSourcePort;                    // we bind the socket on SYN reception
        uMemcpy(ptr_TCP->ucRemoteIP, source_IP_address, ipLength);
        ptr_TCP->MySocketNumber = ReuseSocket;
        addNetworkInterface(ptr_TCP->MySocketNumber, ptrRx_frame->ucNetworkID, ptrRx_frame->ucInterface); // {51} add the interface that established the connection
        addVLAN(ptr_TCP->MySocketNumber, ptrRx_frame->ucVLAN_content);   // {53}
        return (ptr_TCP);                                                // return pointer to the newly bound socket
    }
#endif
    return 0;                                                            // no free socket found - return null pointer
}

#if (!defined IP_RX_CHECKSUM_OFFLOAD || !defined IP_TX_PAYLOAD_CHECKSUM_OFFLOAD) || defined IP_INTERFACE_WITHOUT_CS_OFFLOADING || (defined FORCE_PAYLOAD_ICMPV6_TX && defined USE_IPV6 && defined USE_IPV6INV4) || defined _WINDOWS // {30}
// Check a TCP check sum - if all is well the function returns the value zero
//
static unsigned short fnCalculateTCP_checksum(unsigned char *ucIPSource, unsigned char *ucIPDest, unsigned char *ucTCP_data, unsigned short usTCP_len, int iOptions)
{
    if (iOptions & (INTERFACE_NO_RX_CS_OFFLOADING | INTERFACE_NO_TX_CS_OFFLOADING | INTERFACE_NO_TX_PAYLOAD_CS_OFFLOADING)) {
        unsigned short usIPlength = IPV4_LENGTH;
        unsigned char ucTemp[2];
        unsigned short usCheckSum;

    #if defined USE_IPV6
        if (iOptions & INTERFACE_CALC_TCP_IPv6) {
            usIPlength = IPV6_LENGTH;
        }
    #endif

        usCheckSum = fnCalcIP_CS(0, ucIPSource, usIPlength);                 // do it first to IP pseudo header
        usCheckSum = fnCalcIP_CS(usCheckSum, ucIPDest, usIPlength);
        usCheckSum = fnCalcIP_CS(usCheckSum, (unsigned char *)ucIP_TCP_TYPE, sizeof(ucIP_TCP_TYPE));
        ucTemp[0] = (unsigned char)(usTCP_len >> 8);
        ucTemp[1] = (unsigned char)(usTCP_len);
        usCheckSum = fnCalcIP_CS(usCheckSum, ucTemp, sizeof(usTCP_len));
        return (~fnCalcIP_CS(usCheckSum, ucTCP_data, usTCP_len));            // complete calculation over the TCP payload
    }
    return 0;
}
#endif

// Send a TCP RST frame
//
#if defined USE_IPV6                                                     // {41}
static void fnTCP_send_reset(TCP_PACKET *rx_tcp_packet, ETHERNET_FRAME *ptrRx_frame, int iIPv6) // {52}
#else
static void fnTCP_send_reset(TCP_PACKET *rx_tcp_packet, ETHERNET_FRAME *ptrRx_frame) //  {52}
#endif
{
#if defined PHY_TAIL_TAGGING                                             // {63}
    #define INTERFACE_REFERENCE   ptrRx_frame->ucRxPort
#else
    #define INTERFACE_REFERENCE   0
#endif
    IP_PACKET *received_ip_packet = (IP_PACKET *)&ptrRx_frame->ptEth->ucData; // {52} IPv4 or IPv6 header
    TCP_CONTROL *ptr_TCP = &tTCP[NO_OF_TCPSOCKETS];

    if ((rx_tcp_packet->usHeaderLengthAndFlags & TCP_FLAG_RESET) || (!tTCP)) { // {61}
        return;
    }

#if defined USE_IPV6                                                     // {41}
    if (iIPv6 != 0) {                                                    // if connecting over IPv6
        uMemcpy(ptr_TCP->ucRemoteIP, ((IP_PACKET_V6 *)received_ip_packet)->source_IP_address, IPV6_LENGTH); // enter the peer's IPv6 address on connection attempt {45}
        ptr_TCP->ucTCPInternalFlags |= TCP_OVER_IPV6;                    // flag that the socket is operating over IPv6
    }
    else {
        uMemcpy(ptr_TCP->ucRemoteIP, received_ip_packet->source_IP_address, IPV4_LENGTH); // {45}
        ptr_TCP->ucTCPInternalFlags &= ~TCP_OVER_IPV6;
    }
#else
    uMemcpy(ptr_TCP->ucRemoteIP, received_ip_packet->source_IP_address, IPV4_LENGTH);
#endif
    ptr_TCP->usRemport = rx_tcp_packet->usSourcePort;
    ptr_TCP->usLocport = rx_tcp_packet->usDestPort;
    ptr_TCP->ucTos     = 0;

    if ((rx_tcp_packet->usHeaderLengthAndFlags & TCP_FLAG_ACK) != 0) {   // does the packet have ACK flag set?
        ptr_TCP->ulSendUnackedNumber = rx_tcp_packet->ulAckNo;           // jump - use it as our sequence
        ptr_TCP->ucSendFlags = TCP_FLAG_RESET;
        ptr_TCP->ulNextReceptionNumber = rx_tcp_packet->ulSeqNr;
    }
    else {
        ptr_TCP->ulSendUnackedNumber = 0;
        ptr_TCP->ucSendFlags = TCP_FLAG_RESET | TCP_FLAG_ACK;
        ptr_TCP->ulNextReceptionNumber = (rx_tcp_packet->ulSeqNr + 1);
    }
    _TCP_SOCKET_MASK_ASSIGN(ptr_TCP->MySocketNumber);                    // {52}
    addNetworkInterface(ptr_TCP->MySocketNumber, ptrRx_frame->ucNetworkID, INTERFACE_REFERENCE); // {52}{63}
    addVLAN(ptr_TCP->MySocketNumber, ptrRx_frame->ucVLAN_content);       // {53}
    fnSendTCPControl(ptr_TCP);                                           // use last socket internally
}

// Code saving routines
//
static int fnCheckFIN(TCP_PACKET *rx_tcp_packet, TCP_CONTROL *ptr_TCP, unsigned short usLength, unsigned char ucFlags)
{
    if ((rx_tcp_packet->usHeaderLengthAndFlags & TCP_FLAG_FIN) != 0) {   // repeated FIN
        ptr_TCP->ulNextReceptionNumber = (rx_tcp_packet->ulSeqNr + 1);   // peer's sequence number
        ptr_TCP->ulNextReceptionNumber += usLength;
        ptr_TCP->ucSendFlags = ucFlags;
        fnSendTCPControl(ptr_TCP);                                       // send Flags as defined
        return 1;
    }
    return 0;
}

#if defined SUPPORT_PEER_MSS

#define KIND_END_OF_OPTION_LIST   0
#define KIND_NO_OPERATION         1
#define KIND_MAXIMUM_SEGMENT_SIZE 2
#define KIND_WINDOW_SCALE_FACTOR  3
#define KIND_SACK_PERMITTED       4
#define KIND_TIME_STAMP           8

static unsigned short fnGetTCP_Option(unsigned char **ptr_tcp_options, unsigned char *ucOptsLength)
{
    unsigned short usMSS = 0;
    unsigned char ucLength;
    unsigned char *tcp_options = *ptr_tcp_options;

    ucLength = *tcp_options++ - 2;
    *ucOptsLength -= (ucLength + 1);
    while (ucLength--) {
        usMSS <<= 8;
        usMSS |= *tcp_options++;
    }

    *ptr_tcp_options = tcp_options;
    return usMSS;
}

static unsigned short fnExtractMSS(unsigned char *tcp_options, unsigned char ucOptsLength)
{
    unsigned short usMSS = 512;                                          // if we do not find an MSS option we set MSS to 512, which is a conservative - probably worst case

    while (ucOptsLength-- != 0) {
        switch (*tcp_options++) {
        case KIND_MAXIMUM_SEGMENT_SIZE:
            usMSS = fnGetTCP_Option(&tcp_options, &ucOptsLength);
            break;

        case KIND_TIME_STAMP:
        case KIND_SACK_PERMITTED:
        case KIND_WINDOW_SCALE_FACTOR:
            fnGetTCP_Option(&tcp_options, &ucOptsLength);
            break;

        case KIND_NO_OPERATION:
            break;

        case KIND_END_OF_OPTION_LIST:
        default:
            return usMSS;
        }
    }
    return usMSS;
}
#endif                                                                   // end SUPPORT_PEER_MSS


// This function is called when we have received a TCP frame
//
extern void fnHandleTCP(ETHERNET_FRAME *ptrRx_frame)                     // {41}
{
#if defined _WORKAROUND_CW_7_1_2                                         // {15}
    register unsigned short _usHeaderLengthAndFlags;
#endif
    TCP_CONTROL   *ptr_TCP;                                              // pointer to the receive socket TCP control structure
    TCP_PACKET    rx_tcp_packet;                                         // structure for storing temporary frame header
    unsigned char ucHeadLen;
    int           iAppTCP = 0;
    IP_PACKET    *received_ip_packet = (IP_PACKET *)&ptrRx_frame->ptEth->ucData; // IPv4 or IPv6 header
    unsigned char *tcp_data = &ptrRx_frame->ptEth->ucData[ptrRx_frame->usIPLength]; // set data to pointer to TCP content
    unsigned short usLen = ptrRx_frame->usDataLength;
    USOCKET       tcp_socket;                                            // {56}
#if defined SUPPORT_RX_TCP_OPTIONS
    unsigned char *tcp_options;
#endif
#if defined MJBC_TEST5
    unsigned long ulOriginalNextReceptionNumber;                         // {66}
#endif
#if !defined IP_RX_CHECKSUM_OFFLOAD || defined IP_INTERFACE_WITHOUT_CS_OFFLOADING || defined _WINDOWS
    #if IP_INTERFACE_COUNT > 1
    unsigned char ucRxInterfaceHandling = (ptrRx_frame->ucInterfaceHandling & INTERFACE_NO_RX_CS_OFFLOADING);
    #else
        #define ucRxInterfaceHandling  INTERFACE_NO_RX_CS_OFFLOADING
    #endif
#endif
#if defined USE_IPV6
    int iIPv6 = 0;
    if (ptrRx_frame->ucIPV6_Protocol == IP_TCP) {                        // if connecting over IPv6
        iIPv6 = 1;                                                       // flag that TCP is being transported over IPv6 and not IPv4
    }
#endif
#if !defined IP_RX_CHECKSUM_OFFLOAD || defined IP_INTERFACE_WITHOUT_CS_OFFLOADING || defined _WINDOWS // {30}
    // Check the TCP checksum (is not performed when the reception device performs TCP offloading)
    //
    #if defined USE_IPV6
    if (iIPv6 != 0) {
        if (fnCalculateTCP_checksum(((IP_PACKET_V6 *)received_ip_packet)->source_IP_address, ((IP_PACKET_V6 *)received_ip_packet)->destination_IP_address, tcp_data, usLen, (ucRxInterfaceHandling | INTERFACE_CALC_TCP_IPv6)) != 0) {
        #if !defined ETHEREAL                                            // this is set in a windows project when using Ethereal or simulator via NIC due to (presumably) WinPcap errors in saving the check sum
        return;                                                          // ignore bad check sum
        #endif
        }
    }
    else {
    #endif
        if (fnCalculateTCP_checksum(received_ip_packet->source_IP_address, received_ip_packet->destination_IP_address, tcp_data, usLen, ucRxInterfaceHandling) != 0) {
        #if !defined ETHEREAL                                            // this is set in a windows project when using Ethereal or simulator via NIC due to (presumably) WinPcap errors in saving the check sum
            return;                                                      // ignore bad check sum
        #endif
        }
    #if defined USE_IPV6
    }
    #endif
#endif
    // Copy the TCP header from the received frame in a manner which is always hardware independent
    //
    rx_tcp_packet.usSourcePort             = *tcp_data++;
    rx_tcp_packet.usSourcePort           <<= 8;
    rx_tcp_packet.usSourcePort            |= *tcp_data++;

    rx_tcp_packet.usDestPort               = *tcp_data++;
    rx_tcp_packet.usDestPort             <<= 8;
    rx_tcp_packet.usDestPort              |= *tcp_data++;

    rx_tcp_packet.ulSeqNr                  = *tcp_data++;
    rx_tcp_packet.ulSeqNr                <<= 8;
    rx_tcp_packet.ulSeqNr                 |= *tcp_data++;
    rx_tcp_packet.ulSeqNr                <<= 8;
    rx_tcp_packet.ulSeqNr                 |= *tcp_data++;
    rx_tcp_packet.ulSeqNr                <<= 8;
    rx_tcp_packet.ulSeqNr                 |= *tcp_data++;

#if defined _WINDOWS                                                     // use as breakpoint location to stop on a known sequence number
    if (rx_tcp_packet.ulSeqNr == 0x18aa1716) {
        rx_tcp_packet.ulSeqNr = rx_tcp_packet.ulSeqNr;
    }
#endif

	rx_tcp_packet.ulAckNo                  = *tcp_data++;
    rx_tcp_packet.ulAckNo                <<= 8;
    rx_tcp_packet.ulAckNo                 |= *tcp_data++;
    rx_tcp_packet.ulAckNo                <<= 8;
    rx_tcp_packet.ulAckNo                 |= *tcp_data++;
    rx_tcp_packet.ulAckNo                <<= 8;
    rx_tcp_packet.ulAckNo                 |= *tcp_data++;

    rx_tcp_packet.usHeaderLengthAndFlags   = *tcp_data++;                // not that CW7.1.2 puts this variable at SP + 25, which is normal
    rx_tcp_packet.usHeaderLengthAndFlags <<= 8;
    rx_tcp_packet.usHeaderLengthAndFlags  |= *tcp_data++;

#if defined _WORKAROUND_CW_7_1_2                                         // {15}
    _usHeaderLengthAndFlags = rx_tcp_packet.usHeaderLengthAndFlags;      // we make a backup of this variable in a register
#endif

    rx_tcp_packet.usWindowSize             = *tcp_data++;
    rx_tcp_packet.usWindowSize           <<= 8;
    rx_tcp_packet.usWindowSize            |= *tcp_data++;

    rx_tcp_packet.usChecksum               = *tcp_data++;
    rx_tcp_packet.usChecksum             <<= 8;
    rx_tcp_packet.usChecksum              |= *tcp_data++;

    rx_tcp_packet.usUrgentPointer          = *tcp_data++;
    rx_tcp_packet.usUrgentPointer        <<= 8;
    rx_tcp_packet.usUrgentPointer         |= *tcp_data++;

    ucHeadLen = (unsigned char)(rx_tcp_packet.usHeaderLengthAndFlags >> 10); // extract the header length
    ucHeadLen &= 0x3c;

    if ((ucHeadLen < MIN_TCP_HLEN) || (ucHeadLen > usLen)) {             // check the validity of the header length
        return;                                                          // too short or too long to be valid so ignore the received frame
    }

    if (ucHeadLen > MAX_TCP_OPTLEN + MIN_TCP_HLEN) {                     // check the options length
        return;                                                          // options too long to be valid so ignore the received frame
    }

#if defined USE_IPV6                                                     // {41}
    if (iIPv6 != 0) {
        if ((ptr_TCP = fnMapTCPSocket(ptrRx_frame, &rx_tcp_packet, ((IP_PACKET_V6 *)received_ip_packet)->source_IP_address, 1)) == 0) { // {51} find a socket to handle this port number
            fnTCP_send_reset(&rx_tcp_packet, ptrRx_frame, 1);            // {52} send RST if the port doesn't exist
            return;                                                      // no port
        }
    }
    else {
        if ((ptr_TCP = fnMapTCPSocket(ptrRx_frame, &rx_tcp_packet, received_ip_packet->source_IP_address, 0)) == 0) { // {51} find a socket to handle this port number
            fnTCP_send_reset(&rx_tcp_packet, ptrRx_frame, 0);            // {52} send RST if the port doesn't exist
            return;                                                      // no port
        }
    }
#else
    if ((ptr_TCP = fnMapTCPSocket(ptrRx_frame, &rx_tcp_packet, received_ip_packet->source_IP_address)) == 0) { // {51} find a socket to handle this port number
        fnTCP_send_reset(&rx_tcp_packet, ptrRx_frame);                   // {52} send RST if the port doesn't exist
        return;                                                          // no port
    }
#endif

#if defined HTTP_WINDOWING_BUFFERS || defined FTP_DATA_WINDOWS           // {10}
    present_tcp = ptr_TCP;                                               // {3} set global pointer to the present receive socket TCP control structure
#endif
#if defined SUPPORT_RX_TCP_OPTIONS
    tcp_options = tcp_data;
#endif
    tcp_data += (ucHeadLen - MIN_TCP_HLEN);                              // set the data pointer to the start of the TCP data content
#if defined SUPPORT_PEER_WINDOW
    ptr_TCP->usTxWindowSize = rx_tcp_packet.usWindowSize;                // save the present windows size advertised by the peer
#endif
    tcp_socket = ptr_TCP->MySocketNumber;                                // {56}

    // We have what seems to be a valid frame for a local TCP port and we now process it in the TCP state-event machine
    //
    if (ptr_TCP->ucTCP_state <= TCP_STATE_CLOSED) {                      // check invalid states
    #if defined USE_IPV6                                                 // {41}
        fnTCP_send_reset(&rx_tcp_packet, ptrRx_frame, iIPv6);            // {52} send RST
    #else
        fnTCP_send_reset(&rx_tcp_packet, ptrRx_frame);                   // {52} send RST
    #endif
        return;                                                          // do no more since our socket is not ready
    }

    if ((rx_tcp_packet.usHeaderLengthAndFlags & TCP_FLAG_RESET) != 0) {  // check whether we are receiving a RST
        fnNewTCPState(ptr_TCP, TCP_STATE_CLOSED);                        // move to the closed state and inform the application
        ptr_TCP->event_listener(tcp_socket, TCP_EVENT_ABORT, ptr_TCP->ucRemoteIP, ptr_TCP->usRemport); // {56}
        return;
    }

    switch (ptr_TCP->ucTCP_state) {                                      // switch depending on TCP socket state
        case TCP_STATE_LISTEN:                                           // we are normally a server and are waiting for a client to send us a SYN
        {
                                                                         // note that we will only have reached here if we had found a valid listening socket and have received a SYN
                                                                         // SYN received - inform application. The application usually accepts it
              int iAccept;
#if defined USE_IPV6                                                     // {41}
              if (iIPv6 != 0) {                                          // if connecting over IPv6
                  ptr_TCP->ucTCPInternalFlags = TCP_OVER_IPV6;           // flag that the socket is operating over IPv6
              }
              else {
                  ptr_TCP->ucTCPInternalFlags = 0;                       // start with flags cleared
              }
#else
              ptr_TCP->ucTCPInternalFlags = 0;                           // start with flags cleared
#endif
              iAccept = ptr_TCP->event_listener(ptr_TCP->MySocketNumber, TCP_EVENT_CONREQ, ptr_TCP->ucRemoteIP, ptr_TCP->usRemport);
              if (iAccept == APP_REJECT) {                               // has the application rejected the connection request?
#if defined USE_IPV6                                                     // {41}
                  fnTCP_send_reset(&rx_tcp_packet, ptrRx_frame, iIPv6);  // {52} reset connection because application is not interested
#else
                  fnTCP_send_reset(&rx_tcp_packet, ptrRx_frame);         // {52} reset connection because application is not interested
#endif
                  return;
              }
              else if (iAccept == APP_WAIT) {                            // the application doesn't reject but wants to wait
                  return;                                                // we ignore it since the application may want to accept a repetition later
              }
              else {
                  // The application accepts the connection
                  //
#if defined HTTP_WINDOWING_BUFFERS || defined FTP_DATA_WINDOWS           // {10}
                  ptr_TCP->usOpenCnt = 0;                                // {7}
#endif
#if defined SUPPORT_PEER_MSS
                  ptr_TCP->usPeerMSS = fnExtractMSS(tcp_options, (unsigned char)(ucHeadLen - MIN_TCP_HLEN));
#endif
                  fnNewTCPState(ptr_TCP, TCP_STATE_SYN_RCVD);            // next state is SYN_RCVD where an ACK will establish the connection
                  ptr_TCP->ulNextReceptionNumber = (rx_tcp_packet.ulSeqNr + 1); // synchronise sequence number (SYN counts as 1)
                  ptr_TCP->ulSendUnackedNumber = fnGetTCP_init_seq();    // get a sequence number for our acks
#if defined ANNOUNCE_MAX_SEGMENT_SIZE
                  fnSendSyn(ptr_TCP, (TCP_FLAG_SYN | TCP_FLAG_ACK));     // send SYN, ACK with MSS announcement
#else
                  ptr_TCP->ucSendFlags = (TCP_FLAG_SYN | TCP_FLAG_ACK);  // prepare flags to be transmitted
                  fnSendTCPControl(ptr_TCP);                             // send SYN, ACK
#endif
                  ptr_TCP->ulNextTransmissionNumber = (ptr_TCP->ulSendUnackedNumber + 1); // SYN counts as 1 so increment
              }
        }
        break;

        case TCP_STATE_SYN_RCVD:                                         // server TCP connection established
            if (rx_tcp_packet.usHeaderLengthAndFlags & TCP_FLAG_ACK) {   // ACK from our SYN
#if defined ETHEREAL
                ptr_TCP->ulNextTransmissionNumber = rx_tcp_packet.ulAckNo; // synchronise when simulating
#endif
                if (rx_tcp_packet.ulAckNo == ptr_TCP->ulNextTransmissionNumber) {  // check that it is correct ack
                    if (rx_tcp_packet.usHeaderLengthAndFlags & TCP_FLAG_SYN) {     // check for simultaneous open
                        ptr_TCP->ulNextReceptionNumber = (rx_tcp_packet.ulSeqNr + 1);// get peer's sequence number
                        ptr_TCP->ucSendFlags = TCP_FLAG_ACK;
                        fnSendTCPControl(ptr_TCP);                       // send ACK
                    }
                    else if (rx_tcp_packet.ulSeqNr != ptr_TCP->ulNextReceptionNumber) { // check that the sequence number is correct
                        return;
                    }
                    ptr_TCP->ucSendFlags = TCP_FLAG_ACK;                 // ACK without SYN flag
                    ptr_TCP->ulSendUnackedNumber = ptr_TCP->ulNextTransmissionNumber; // synchronise
                    fnNewTCPState(ptr_TCP, TCP_STATE_ESTABLISHED);       // we are connected and in data transfer state - inform application
#if defined USE_SECURE_SOCKET_LAYER
                    if (ptr_TCP->event_listener(tcp_socket, TCP_EVENT_CONNECTED, ptr_TCP->ucRemoteIP, ptr_TCP->usRemport) == APP_SECURITY_CONNECTED) { // if server uses secure layer
                        ptr_TCP->MySocketNumber |= SECURE_SOCKET_MODE;   // flag that the socket is secure
                        ptr_TCP->event_listener = (int(*)(USOCKET, unsigned char, unsigned char *, unsigned short))fnInsertSecureLayer(ptr_TCP->MySocketNumber, ptr_TCP->event_listener, 1); // insert the secure socket layer between TCP and the application                   
                    }
#else
                    ptr_TCP->event_listener(tcp_socket, TCP_EVENT_CONNECTED, ptr_TCP->ucRemoteIP, ptr_TCP->usRemport); // {56}
#endif
                }
            }
            break;

        case TCP_STATE_ESTABLISHED:                                      // we have an active connection, in data transfer state
            if ((rx_tcp_packet.usHeaderLengthAndFlags & TCP_FLAG_SYN) != 0) { // reception of SYN during established connection
                if ((rx_tcp_packet.usHeaderLengthAndFlags & TCP_FLAG_ACK) != 0) { // SYN + ACK
                    if ((rx_tcp_packet.ulSeqNr + 1) == ptr_TCP->ulNextReceptionNumber) { // we are receiving a SYN ACK which means the peer probably didn't receive our original ACK to its SYN
                        if (rx_tcp_packet.ulAckNo == ptr_TCP->ulNextTransmissionNumber) {
                            ptr_TCP->ucSendFlags = TCP_FLAG_ACK;
                            fnSendTCPControl(ptr_TCP);                   // we repeat the lost ACK
                        }
                    }
                }
                else {                                                   // {47} SYN on an active connection means that the client closed the original connection and is trying to establish a new connection on the same port pair
                    fnNewTCPState(ptr_TCP, TCP_STATE_CLOSED);            // move to the closed state and inform the application (we handle this as a RST)
                    ptr_TCP->event_listener(tcp_socket, TCP_EVENT_ABORT, ptr_TCP->ucRemoteIP, ptr_TCP->usRemport); // {56}
                }
                return;
            }
#if defined MJBC_TEST5
            ulOriginalNextReceptionNumber = ptr_TCP->ulNextReceptionNumber; // {66}
#endif
            if (ptr_TCP->ulSendUnackedNumber != ptr_TCP->ulNextTransmissionNumber) { // do we have unacked data and expect an ACK ?
                if ((rx_tcp_packet.usHeaderLengthAndFlags & TCP_FLAG_ACK) == 0) { // ignore everything apart from an ACK when we are waiting for one
                    return;
                }
#if defined SUPPORT_PEER_WINDOW && (defined HTTP_WINDOWING_BUFFERS || defined FTP_DATA_WINDOWS) // {10}
                ptr_TCP->ucPersistentTimer = ptr_TCP->ucProbeCount = 0;  // {3}
#endif
                if (rx_tcp_packet.ulAckNo == ptr_TCP->ulNextTransmissionNumber) { // is all outstanding data being acked?
//#if defined DISCARD_DATA_WHILE_CLOSING                                 // {5}{33} if the reception frame is carrying data as well as acking we need to synchronise the ack counter here in case the application initiates a close
                    int iReceptionAdjusted = 0;
                    if ((usLen > ucHeadLen) && (ptr_TCP->ulNextReceptionNumber == rx_tcp_packet.ulSeqNr)) {
                        ptr_TCP->ulNextReceptionNumber += (usLen - ucHeadLen);
                        iReceptionAdjusted = 1;
                    }
//#endif
                    ptr_TCP->ulSendUnackedNumber = ptr_TCP->ulNextTransmissionNumber; // no more unacked data
                                                                         // inform application that data has been successfully acked
                    if ((rx_tcp_packet.usHeaderLengthAndFlags & TCP_FLAG_FIN) != 0) { // {67}
                        ptr_TCP->ucSendFlags = TCP_FLAG_FIN_RECEIVED;    // if the application closes (simultaneous close) we must increment the ack counter to also accept the received FIN
                    }
                    if ((APP_REQUEST_CLOSE & (iAppTCP |= ptr_TCP->event_listener(tcp_socket, TCP_EVENT_ACK, ptr_TCP->ucRemoteIP, ptr_TCP->usRemport))) != 0) { // {56}
                        if ((rx_tcp_packet.usHeaderLengthAndFlags & TCP_FLAG_FIN) != 0) { // the application has commanded the close of the connection
                                                                         // we have just received FIN + ACK, meaning that the other side has also initiated a close
                            fnNewTCPState(ptr_TCP, TCP_STATE_CLOSING);   // this a simultaneous close, so we go to state closing and wait for the last ack
                        }
#if defined DISCARD_DATA_WHILE_CLOSING                                   // {5} don't return in case there is also data to be handled
                        iAppTCP |= APP_REJECT_DATA;                      // {23} mark that we have already sent an ack
#else
                        return;                                          // if the application closes we no longer accept any more data
#endif
                    }
//#if defined DISCARD_DATA_WHILE_CLOSING                                 {33}
                    if (iReceptionAdjusted != 0) {                       // {5} if we have just adjusted the ack counter, set it back and let the data reception handle it
                        ptr_TCP->ulNextReceptionNumber -= (usLen - ucHeadLen);
                    }
//#endif
#if defined SUPPORT_PEER_WINDOW && (defined HTTP_WINDOWING_BUFFERS || defined FTP_DATA_WINDOWS) // {3}{10}
                    if (ptr_TCP->usTxWindowSize == 0) {                  // peer's window has completely closed so start persistent timer
                        ptr_TCP->ucPersistentTimer = 5;                  // persistent timer initial timeout value
                    }
#endif
                }
#if defined SUPPORT_PEER_WINDOW
                else {                                                   // inform application that partial data has possibly been successfully acked
                    unsigned short usPartialDataLength = (unsigned short)(ptr_TCP->ulNextTransmissionNumber - rx_tcp_packet.ulAckNo); // the length of the data being acknowledged
                    if (usPartialDataLength != 0) {                      // {29}
                        ptr_TCP->usTxWindowSize -= usPartialDataLength;  // {32} since there is still data underway to the peer respect this in the window size
                        if ((APP_REQUEST_CLOSE & (iAppTCP |= ptr_TCP->event_listener(tcp_socket, TCP_EVENT_PARTIAL_ACK, ptr_TCP->ucRemoteIP, usPartialDataLength))) != 0) { // {56} usPartialDataLength is the number of bytes that are still unaccounted for
                            if ((rx_tcp_packet.usHeaderLengthAndFlags & TCP_FLAG_FIN) != 0) { // the application has commanded the close of the connection
                                // We have just received FIN + ACK, meaning that the other side has also initiated a close
                                //
                                fnNewTCPState(ptr_TCP, TCP_STATE_CLOSING); // this a simultaneous close, so we go to state closing and wait for the last ack
                            }
                            return;
                        }
                    }
                    iAppTCP |= HANDLING_PARTICAL_ACK;                    // mark that we are dealing with a partial ACK
                }
#endif
            }
#if defined SUPPORT_PEER_WINDOW || defined HTTP_WINDOWING_BUFFERS || defined FTP_DATA_WINDOWS // {3}{10}{35}
            else {
                if ((rx_tcp_packet.usHeaderLengthAndFlags & TCP_FLAG_ACK) != 0) { // TCP window update
    #if defined HTTP_WINDOWING_BUFFERS || defined FTP_DATA_WINDOWS       // {35}
                      ptr_TCP->ucPersistentTimer = ptr_TCP->ucProbeCount = 0; // disable persistent timer since we have a window update
    #endif
                      ptr_TCP->event_listener(tcp_socket, TCP_WINDOW_UPDATE, ptr_TCP->ucRemoteIP, ptr_TCP->usRemport); // {56}
                }
            }
#endif
            if (usLen > ucHeadLen) {                                     // {24} received TCP frame carrying data
                if (ptr_TCP->ulNextReceptionNumber == rx_tcp_packet.ulSeqNr) { // data content and not a possible repetition
                                                                         // data received which must be acked
                    ptr_TCP->ucSendFlags = TCP_FLAG_ACK;                 // prepare the ACK
                    ptr_TCP->ulNextReceptionNumber += (usLen - ucHeadLen); // the data that we will have acked
                                                                         // pass the data to the application (if it sends data it also ACKs so we don't have to ack here)
                    iAppTCP |= ptr_TCP->event_listener(tcp_socket, TCP_EVENT_DATA, tcp_data, (unsigned short)(usLen - ucHeadLen)); // {56} pass the received data to the listerer
                    if ((iAppTCP & (APP_REQUEST_CLOSE | APP_REJECT_DATA)) != 0) { // has the application just closed the connection? If so we don't need to send ACK (also if application doesn't want to acknowledge)
                        ptr_TCP->ulNextReceptionNumber -= (usLen - ucHeadLen); // correct in case we forced the ack to be ignored (otherwise we will ignore the repetition)
                        return;
                    }
#if defined USE_OUT_OF_ORDER_TCP_RX_BUFFER
                    if (OutOfOrder.ucTTL != 0) {                         // {65} there is an out-of-order frame waiting
                        if (ptr_TCP == OutOfOrder.ptr_TCP) {             // if we are the owner of the buffer
                            if (OutOfOrder.ulSequenceNumber == ptr_TCP->ulNextReceptionNumber) { // if its sequence number matches our new reception number we can process it
                                ptr_TCP->ulNextReceptionNumber += OutOfOrder.usLength; // the data that we have now acked
                                iAppTCP |= ptr_TCP->event_listener(tcp_socket, TCP_EVENT_DATA, OutOfOrder.ucData, OutOfOrder.usLength); // pass the stored data to the listerer
                                if (iAppTCP & (APP_REQUEST_CLOSE | APP_REJECT_DATA)) { // has the application just closed the connection? If so we don't need to send ACK (also if application doesn't want to acknowledge)
                                    ptr_TCP->ulNextReceptionNumber -= OutOfOrder.usLength; // correct in case we forced the ack to be ignored (otherwise we will ignore the repetition)
                                }
                                else {
                                    OutOfOrder.ucTTL = 0;                // out of order buffer no longer valid
                                }
                            }
                            else {
                                OutOfOrder.ucTTL--;                      // reduce the life time of the buffer so that it can never block further use indefinitely
                            }
                        }
                    }
#endif
                }
                else {                                                   // {22} this is a repeated reception. If it has already been treated send an ACK back to it to quieten the peer. If it is repeating with more data handle just the extra data and ack it
                    unsigned short usPayload = (usLen - ucHeadLen);
                    unsigned short usExtraData = (unsigned short)(usPayload - (ptr_TCP->ulNextReceptionNumber - rx_tcp_packet.ulSeqNr));
                    if (usExtraData != 0) {                              // the peer is repeating with extra data content added
                        if (usPayload < usExtraData) {                   // {28} a windowed frame after a reception packet loss
#if defined USE_OUT_OF_ORDER_TCP_RX_BUFFER
                            if (OutOfOrder.ucTTL == 0) {                 // {65} if out of order buffer is free
                                OutOfOrder.ucTTL = 3;                    // its life-time (the content is now valid)
                                OutOfOrder.ptr_TCP = ptr_TCP;            // owned by this socket
                                OutOfOrder.usLength = usPayload;         // the amount of data in the buffer
                                uMemcpy(OutOfOrder.ucData, tcp_data, usPayload); // temporarily save out of order data
                                OutOfOrder.ulSequenceNumber = rx_tcp_packet.ulSeqNr; // the sequence number of the data in the buffer
                            }
                            else if (ptr_TCP == OutOfOrder.ptr_TCP) {    // if this socket owns the buffer
                                OutOfOrder.ucTTL--;                      // reduce the life time of the buffer so that it can never block further use indefinitely
                            }
#endif
                            return;                                      // this frame needs to be ignored since an ack would cause previous lost content to be acked and lost forever
                        }
                        ptr_TCP->ulNextReceptionNumber += usExtraData;
                        iAppTCP |= ptr_TCP->event_listener(tcp_socket, TCP_EVENT_DATA, (tcp_data + (usPayload - usExtraData)), usExtraData); // {56} pass the extra data
                        if ((iAppTCP & (APP_REQUEST_CLOSE | APP_REJECT_DATA)) != 0) { // has the application just closed the connection? If so we don't need to send ACK (also if application doesn't want to acknowledge)
                            ptr_TCP->ulNextReceptionNumber -= usExtraData; // correct in case we forced the ack to be ignored (otherwise we will ignore the repetition)
                            return;
                        }
                    }
                }
            }
            else {                                                       // {24}
                ucHeadLen = 0;                                           // use this to ensure no unnecessary ACKs are returned
            }
#if defined _WORKAROUND_CW_7_1_2                                         // {15}
            if ((_usHeaderLengthAndFlags & TCP_FLAG_FIN) != 0)           // use the backup value from the register
#else
            if ((rx_tcp_packet.usHeaderLengthAndFlags & TCP_FLAG_FIN) != 0) // CW7.1.2 otherwise takes the variable from SP + 30, which is 5 bytes from its real location and no TCP FINs are ever recognised!!
#endif
            {                                                            // check whether FIN flag is set
                                                                         // inform application then we have received a close request
#if defined MJBC_TEST5
                if (ulOriginalNextReceptionNumber != rx_tcp_packet.ulSeqNr) { // {66} data content and not a possible repetition
                    return;                                              // if the FIN's sequence counter doesn't match what we expect it means that we have probably not seen some data that was sent before it - therefore we ignore it until the data data has been received
                }
#endif
                iAppTCP = ptr_TCP->event_listener(tcp_socket, TCP_EVENT_CLOSE, ptr_TCP->ucRemoteIP, ptr_TCP->usRemport); // {56}{60}
                if (APP_ACCEPT == iAppTCP) {                             // application is accepting the close request
                    ptr_TCP->ucSendFlags = (TCP_FLAG_ACK + TCP_FLAG_FIN);// ack the received FIN and send our own FIN
                    fnNewTCPState(ptr_TCP, TCP_STATE_LAST_ACK);          // go to state LAST ACK doing passive close. We wait for a final ack
                }
                else if (APP_REJECT_DATA == iAppTCP) {                   // {60}
                    return;                                              // reject the close so that it will be repeated
                }
                else {
                    ptr_TCP->ucSendFlags = TCP_FLAG_ACK;                 // the application wants to perform a half close so we only ack the FIN for the moment
                    fnNewTCPState(ptr_TCP, TCP_STATE_CLOSE_WAIT);        // go to state CLOSE WAIT - the application must actively close the connection at a later point in time
                }
                ptr_TCP->ulNextReceptionNumber++;
                ptr_TCP->ulNextTransmissionNumber++;
                fnSendTCPControl(ptr_TCP);
                return;
            }
            if ((iAppTCP & (APP_REJECT_DATA | APP_SENT_DATA)) == 0) {    // {26}
                if ((ucHeadLen != 0) || (ptr_TCP->ulNextReceptionNumber != rx_tcp_packet.ulSeqNr)) { // {24}{42}
                    ptr_TCP->ucSendFlags = TCP_FLAG_ACK;
                    fnSendTCPControl(ptr_TCP);                           // send ack
                }
                ptr_TCP->usLinkTimer = ptr_TCP->usIdleTimeout;           // retrigger the keep alive timer since there is activity
#if defined HIGH_RESOLUTION_TCP_POLLING                                  // {59}
                ptr_TCP->ucLinkDivider = 0;                              // reset the link timer prescaler
#endif
            }
            break;

        case TCP_STATE_SYN_SENT:                                         // we have sent SYN and are hoping for a SYN ACK
            if ((rx_tcp_packet.usHeaderLengthAndFlags & (TCP_FLAG_SYN | TCP_FLAG_ACK)) == (TCP_FLAG_SYN | TCP_FLAG_ACK)) { // client TCP connection established
#if defined ETHEREAL
                ptr_TCP->ulNextTransmissionNumber = rx_tcp_packet.ulAckNo; // synchronise counters when simulating
#endif
                if (rx_tcp_packet.ulAckNo != ptr_TCP->ulNextTransmissionNumber) {
                    return;                                              // incorrect ACK so ignore
                }
                ptr_TCP->ulNextReceptionNumber = (rx_tcp_packet.ulSeqNr + 1); // peer's sequence number
                ptr_TCP->ulSendUnackedNumber = ptr_TCP->ulNextTransmissionNumber; // synchronise
#if defined SUPPORT_PEER_MSS
                ptr_TCP->usPeerMSS = fnExtractMSS(tcp_options, (unsigned char)(ucHeadLen - MIN_TCP_HLEN));
#endif
                fnNewTCPState(ptr_TCP, TCP_STATE_ESTABLISHED);
                ptr_TCP->ucSendFlags = TCP_FLAG_ACK;

                fnSendTCPControl(ptr_TCP);                               // send an ack and the TCP connection is established
                                                                         // inform application that a link has been established
                ptr_TCP->event_listener(tcp_socket, TCP_EVENT_CONNECTED, ptr_TCP->ucRemoteIP, ptr_TCP->usRemport); // {56}
            }
            else if ((rx_tcp_packet.usHeaderLengthAndFlags & TCP_FLAG_ACK) != 0) {
                fnAbortTCPSession(ptr_TCP);                              // this happens when we have a previous non-correctly disconnected session - we will try to close it
            }
            break;

        case TCP_STATE_FIN_WAIT_1:
            if (((rx_tcp_packet.usHeaderLengthAndFlags & (TCP_FLAG_FIN | TCP_FLAG_ACK)) == (TCP_FLAG_FIN | TCP_FLAG_ACK)) // sometimes we receive a FIN + ACK but the ACK is not valid - we handle this as FIN (below) and don't just ignore
                  && (rx_tcp_packet.ulAckNo == ptr_TCP->ulNextTransmissionNumber)) {
                ptr_TCP->ulNextReceptionNumber =  (rx_tcp_packet.ulSeqNr + 1); // peer's sequence number
                ptr_TCP->ulNextReceptionNumber += (unsigned short)(usLen - ucHeadLen);
                ptr_TCP->ulSendUnackedNumber = ptr_TCP->ulNextTransmissionNumber; // synchronise
                ptr_TCP->ucSendFlags = TCP_FLAG_ACK;
                fnSendTCPControl(ptr_TCP);                               // send ACK
                fnNewTCPState(ptr_TCP, TCP_STATE_TIME_WAIT);             // go to STATE TIME WAIT after sending ack since this may cause the application to set listening state again
                return;
            }
                                                                         // check FIN and return ACK if correct
            if (fnCheckFIN(&rx_tcp_packet, ptr_TCP, (unsigned short)(usLen - ucHeadLen), TCP_FLAG_ACK)) {
                fnNewTCPState(ptr_TCP, TCP_STATE_CLOSING);
                return;
            }

            if ((rx_tcp_packet.usHeaderLengthAndFlags & TCP_FLAG_ACK) != 0) {
                if( rx_tcp_packet.ulAckNo != ptr_TCP->ulNextTransmissionNumber) {
                    return;                                              // invalid ACK so ignore the frame
                }
                // we have been acked but the peer doesn't want to disconnect just yet
#if defined DISCARD_DATA_WHILE_CLOSING                                   // {5} if the reception frame is carrying data we acknowledge it but don't pass it to the application.
                if (usLen > ucHeadLen) {
                    ptr_TCP->ulNextReceptionNumber = (rx_tcp_packet.ulSeqNr + (usLen - ucHeadLen));
                    ptr_TCP->ucSendFlags = TCP_FLAG_ACK;
                    fnSendTCPControl(ptr_TCP);                           // send ack to data just received, while waiting for FIN
                }
#endif
                ptr_TCP->ulSendUnackedNumber = ptr_TCP->ulNextTransmissionNumber; // synchronise
                fnNewTCPState(ptr_TCP, TCP_STATE_FIN_WAIT_2);            // move to FIN WAIT 2
            }
            break;

        case TCP_STATE_FIN_WAIT_2:                                       // we are waiting for the other side to send its FIN
            if (fnCheckFIN(&rx_tcp_packet, ptr_TCP, (unsigned short)(usLen - ucHeadLen), TCP_FLAG_ACK)) { // check FIN and return ACK is correct
                fnNewTCPState(ptr_TCP, TCP_STATE_TIME_WAIT);             // go to new state, which informs the application
            }
#if defined DISCARD_DATA_WHILE_CLOSING                                   // {5} if the reception frame is carrying data we acknowledge it but don't pass it to the application.
            else if (usLen > ucHeadLen) {
                ptr_TCP->ulNextReceptionNumber = (rx_tcp_packet.ulSeqNr + (usLen - ucHeadLen));
                ptr_TCP->ucSendFlags = TCP_FLAG_ACK;
                fnSendTCPControl(ptr_TCP);                               // send ack to data just received, while waiting for FIN
            }
#endif
            break;

        case TCP_STATE_CLOSING:
            if ((rx_tcp_packet.usHeaderLengthAndFlags & TCP_FLAG_ACK) != 0) {
                if( rx_tcp_packet.ulAckNo != ptr_TCP->ulNextTransmissionNumber) {
                    return;                                              // invalid ACK so ignore the frame
                }
                // We have been acked because the peer wants to disconnect too
                //
                ptr_TCP->ulSendUnackedNumber = ptr_TCP->ulNextTransmissionNumber; // synchronise
                fnNewTCPState(ptr_TCP, TCP_STATE_TIME_WAIT);
                return;
            }
            fnCheckFIN(&rx_tcp_packet, ptr_TCP, (unsigned short)(usLen - ucHeadLen), TCP_FLAG_ACK); // check FIN and return ACK is correct
            break;

        case TCP_STATE_LAST_ACK:                                         // doing passive close
            if (rx_tcp_packet.usHeaderLengthAndFlags & TCP_FLAG_ACK) {
                if (rx_tcp_packet.ulAckNo != ptr_TCP->ulNextTransmissionNumber) {
                    return;                                              // invalid ACK so ignore the frame
                }

                ptr_TCP->ulSendUnackedNumber = ptr_TCP->ulNextTransmissionNumber; // synchronise
                // Inform application that the connection has been close. It is up to the application to command listen again if it so desires
                //
                fnNewTCPState(ptr_TCP, TCP_STATE_CLOSED);
                ptr_TCP->event_listener(tcp_socket, TCP_EVENT_CLOSED, ptr_TCP->ucRemoteIP, ptr_TCP->usRemport); // {56}
                return;
            }
            fnCheckFIN(&rx_tcp_packet, ptr_TCP, (unsigned short)(usLen - ucHeadLen), (TCP_FLAG_FIN | TCP_FLAG_ACK)); // check FIN and return FIN ACK if correct
            break;

        case TCP_STATE_TIME_WAIT:
            fnCheckFIN(&rx_tcp_packet, ptr_TCP, (unsigned short)(usLen - ucHeadLen), TCP_FLAG_ACK); // check FIN and return ACK is correct
            break;
    }
}


// Code saving routine
//
static int fnDoCountDown(TCP_CONTROL *ptr_TCP)
{
    if (ptr_TCP->usTransmitTimer != 0) {
        ptr_TCP->usTransmitTimer--;
    }
    else {
        if (ptr_TCP->ucRetransmissions != 0) {                           // timeout
            ptr_TCP->ucRetransmissions--;
            ptr_TCP->usTransmitTimer = TCP_STANDARD_RETRY_TOUT;
            return 1;
        }
        else {
            fnAbortTCPSession(ptr_TCP);                                  // close TCP connection on maximum retries/timeout
        }
    }
    return 0;
}

// This is called at a second rate to update timers and to check timeouts or cause retransmissions
//
static void fnPollTCP(void)
{
    int iInUse = 0;                                                      // {58}
    TCP_CONTROL *ptr_TCP = tTCP;
    USOCKET Socket;

    for (Socket = 0; Socket < NO_OF_TCPSOCKETS; Socket++) {              // for each TCP socket
        switch (ptr_TCP->ucTCP_state) {
            case TCP_STATE_ESTABLISHED:
                if ((ptr_TCP->ucTCPInternalFlags & TCP_CLOSEPENDING) && (ptr_TCP->ulNextTransmissionNumber == ptr_TCP->ulSendUnackedNumber)) {
                    ptr_TCP->ulNextTransmissionNumber++;                 // we have a queued close and all data has now been acknowledged so perform the close
                    ptr_TCP->ucSendFlags = (TCP_FLAG_ACK | TCP_FLAG_FIN);
                    fnSendTCPControl(ptr_TCP);
                    fnNewTCPState(ptr_TCP, TCP_STATE_FIN_WAIT_1);
                    ptr_TCP->ucTCPInternalFlags &= ~TCP_CLOSEPENDING;
                }
#if defined SUPPORT_PEER_WINDOW && (defined HTTP_WINDOWING_BUFFERS || defined FTP_DATA_WINDOWS) // {3}{10}
                else if (ptr_TCP->ucPersistentTimer != 0) {
                    if (--ptr_TCP->ucPersistentTimer == 0) {
                        ptr_TCP->ucPersistentTimer = ucProbeTime[ptr_TCP->ucProbeCount];
                        if (ptr_TCP->ucProbeCount < (sizeof(ucProbeTime) - 1)) { // limit maximum value
                            ptr_TCP->ucProbeCount++;
                        }
                        ptr_TCP->event_listener(Socket, TCP_WINDOW_PROBE, ptr_TCP->ucRemoteIP, ptr_TCP->usRemport);
                    }
                }
#endif
                else if (ptr_TCP->usLinkTimer == 0) {                    // idle time out
                    ptr_TCP->ulNextTransmissionNumber++;
                    ptr_TCP->ucSendFlags = (TCP_FLAG_ACK | TCP_FLAG_FIN);
                    fnSendTCPControl(ptr_TCP);
                    fnNewTCPState(ptr_TCP, TCP_STATE_FIN_WAIT_1);
                    ptr_TCP->event_listener(Socket, TCP_EVENT_CLOSE, ptr_TCP->ucRemoteIP, ptr_TCP->usRemport); // inform application of timeout and link closing
                }
                else {
                    if (ptr_TCP->usLinkTimer != INFINITE_TIMEOUT) {      // if idle timer not disabled
#if defined HIGH_RESOLUTION_TCP_POLLING                                  // {59}
                        if (++ptr_TCP->ucLinkDivider >= (unsigned char)_TCP_POLLING_SECONDS) { // link timer prescaler to convert to seconds
                            ptr_TCP->usLinkTimer--;                      // count down idle period (seconds)
                            ptr_TCP->ucLinkDivider = 0;
                        }
#else
                        ptr_TCP->usLinkTimer--;                          // count down idle period (seconds)
#endif
                    }
#if defined SUPPORT_PEER_WINDOW && !defined MJBC_TEST4                   // {13} this block modified
                    if (!(ptr_TCP->ucTCPInternalFlags & SILLY_WINDOW_AVOIDANCE)) { // force retransmission on silly window
#endif
                        if (ptr_TCP->ulNextTransmissionNumber != ptr_TCP->ulSendUnackedNumber) { // {13} an ack is expected, otherwise don't adjust timers                                              
                            if (ptr_TCP->usTransmitTimer != 0) {         // no transmission timeout timeout yet
                                ptr_TCP->usTransmitTimer--;              // count down
#if defined USE_BUFFERED_TCP
                                if (fnWindowTimeOut(ptr_TCP) == 0) {     // check timeouts on multiple windows which can override
                                    break;                               // no timeout
                                }
#else
                                break;
#endif
                            }
                        }
                        else {
#if defined MJBC_TEST4                                                   // no open transmission to repeat
                            if (ptr_TCP->ucTCPInternalFlags & SILLY_WINDOW_AVOIDANCE) { // always count down when the silly window probe is active and no repetitions
                                if (ptr_TCP->usTransmitTimer != 0) {     // no transmission timeout timeout yet
                                    ptr_TCP->usTransmitTimer--;          // count down
                                }
                                else {                                   // persist timer fired while waiting of a window update from peer with closed receive window
                                    ptr_TCP->event_listener(Socket, TCP_EVENT_REGENERATE, 0, 0); // regenerate zero data size - to be understood as probe request
                                }
                            }
#endif
                            break;
                        }
#if defined SUPPORT_PEER_WINDOW && !defined MJBC_TEST4
                    }
#endif                                                                   // retransmission is necessary
                    if (ptr_TCP->ucRetransmissions != 0) {               // unacked data timeout but not maximum attempts
                        unsigned char ucRetransmissions = ptr_TCP->ucRetransmissions; // {26}
                        unsigned short usRepeatLength = (unsigned short)(ptr_TCP->ulNextTransmissionNumber - ptr_TCP->ulSendUnackedNumber);
                        ptr_TCP->ulNextTransmissionNumber = ptr_TCP->ulSendUnackedNumber;
#if defined HTTP_WINDOWING_BUFFERS || defined FTP_DATA_WINDOWS           // {16}
                        present_tcp = ptr_TCP;                           // set global pointer to the present receive socket TCP control structure
#endif
                        if (ptr_TCP->event_listener(Socket, TCP_EVENT_REGENERATE, 0, usRepeatLength)) { // application must retransmit the data
                            ptr_TCP->usTransmitTimer = (TCP_STANDARD_RETRY_TOUT - 1); // the application has resent the non-acked data, set repeat delay {11}
                            ptr_TCP->ucRetransmissions = (ucRetransmissions - 1); // {26} decrement the remaining retry count before giving up
                            break;
                        }
                    }                                                    // application did not repeat - error - close link
                    fnAbortTCPSession(ptr_TCP);                          // close link due to timeout or error
                }
                break;

            case TCP_STATE_SYN_SENT:
            case TCP_STATE_SYN_RCVD:
                if (fnDoCountDown(ptr_TCP) != 0) {
                    if (ptr_TCP->ucTCP_state == TCP_STATE_SYN_SENT) {
                        ptr_TCP->usTransmitTimer = TCP_SYN_RETRY_TOUT;
#if defined ANNOUNCE_MAX_SEGMENT_SIZE
                        fnSendSyn(ptr_TCP, TCP_FLAG_SYN);                // send SYN with MSS announcement
                        break;
#endif
                    }
#if defined ANNOUNCE_MAX_SEGMENT_SIZE
                    else {
                        fnSendSyn(ptr_TCP, (TCP_FLAG_SYN | TCP_FLAG_ACK)); // send SYN+ACK with MSS announcement
                        break;
                    }
#else
                    fnSendTCPControl(ptr_TCP);                           // retry
#endif
                }
                break;

            case TCP_STATE_TIME_WAIT:
                if (ptr_TCP->usTransmitTimer != 0) {
                    ptr_TCP->usTransmitTimer--;
                }
                else {
                    fnNewTCPState(ptr_TCP, TCP_STATE_CLOSED);            // 2MSL timeout
                }
                break;

            case TCP_STATE_LAST_ACK:
            case TCP_STATE_FIN_WAIT_1:
            case TCP_STATE_CLOSING:
                if (fnDoCountDown(ptr_TCP) != 0) {
                    ptr_TCP->ucSendFlags = (TCP_FLAG_FIN | TCP_FLAG_ACK);
                    fnSendTCPControl(ptr_TCP);
                }
                break;

            case TCP_STATE_FIN_WAIT_2:
                fnDoCountDown(ptr_TCP);
                break;

            case TCP_STATE_FREE:                                         // nothing to do in these states
            case TCP_STATE_RESERVED:
            case TCP_STATE_CLOSED:
            case TCP_STATE_LISTEN:
            default:
                ptr_TCP++;                                               // {58} continue without setting iInUse
                continue;
        }
        iInUse = 1;                                                      // {58} mark that there are sockets using the TCP timer
        ptr_TCP++;
    }
    if (iInUse != 0) {
        uTaskerMonoTimer(OWN_TASK, T_TCP_PERIOD, E_POLL_TCP);            // {58} restart the timer when still required
        iTCP_timer_active = 1;                                           // the TCP timer is active
    }
    else {
        iTCP_timer_active = 0;                                           // the TCP timer is no longer active
    }
}

static void fnAbortTCPSession(TCP_CONTROL *ptr_TCP)
{
    ptr_TCP->ucSendFlags = TCP_FLAG_RESET;                               // send a TCP RST
    fnSendTCPControl(ptr_TCP);

    fnNewTCPState(ptr_TCP, TCP_STATE_CLOSED);

    ptr_TCP->event_listener(_TCP_SOCKET_MASK(ptr_TCP->MySocketNumber), TCP_EVENT_ABORT, ptr_TCP->ucRemoteIP, ptr_TCP->usRemport); // {56} inform application
}

// Allow changing a socket's idle timeout
//
extern USOCKET fnTCP_IdleTimeout(USOCKET TCPSocket, unsigned short usIdleTimeout) // {39}
{
    TCP_CONTROL *ptr_TCP;
    if ((ptr_TCP = fnGetSocketControl(TCPSocket)) == 0) {
        return SOCKET_NOT_FOUND;
    }
    ptr_TCP->usIdleTimeout = usIdleTimeout;
    return TCPSocket;
}

// Adjust the application's buffer payload location by inerting space for a TCP header (optionally with secure layer header)
//
extern unsigned char *fnInsertTCPHeader(USOCKET TCPSocket, unsigned char *ptrBuffer) // {70}
{
    ptrBuffer += MIN_TCP_HLEN;                                           // advance the buffer over the fixed TCP header length
#if defined USE_SECURE_SOCKET_LAYER                                      // {69}
    if ((TCPSocket & SECURE_SOCKET_MODE) != 0) {                         // when using secure socket operation
        ptrBuffer += MAX_SECURE_SOCKET_HEADER;                           // leave additional space for the secure header
    }
#endif
    return ptrBuffer;                                                    // return the payload location that the appication should use
}


#if defined ANNOUNCE_MAX_SEGMENT_SIZE
static void fnSendSyn(TCP_CONTROL *ptr_TCP, unsigned char ucFlags)
{
    unsigned char ucTCP_control_buf[MIN_TCP_HLEN + MSS_OPTION_LENGTH];   // temporary control buffer space including space for the MSS option

    ptr_TCP->ucSendFlags = ucFlags;

    ucTCP_control_buf[MIN_TCP_HLEN]     = MSS_KIND;
    ucTCP_control_buf[MIN_TCP_HLEN + 1] = MSS_LENGTH;
    ucTCP_control_buf[MIN_TCP_HLEN + 2] = (unsigned char)(TCP_DEF_MTU >> 8);
    ucTCP_control_buf[MIN_TCP_HLEN + 3] = (unsigned char)(TCP_DEF_MTU);
    #if defined USE_SECURE_SOCKET_LAYER
    fnSendTCP((ptr_TCP->MySocketNumber & ~(SECURE_SOCKET_MODE)), ucTCP_control_buf, MSS_OPTION_LENGTH, 0); // send a control frame without data but with options
    #else
    fnSendTCP(ptr_TCP->MySocketNumber, ucTCP_control_buf, MSS_OPTION_LENGTH, 0); // send a control frame without data but with options
    #endif
}
#endif

#if defined CONTROL_WINDOW_SIZE
extern USOCKET fnModifyTCPWindow(USOCKET TCPSocket, unsigned short usBufferSpace)
{
    TCP_CONTROL *ptr_TCP;
    if ((ptr_TCP = fnGetSocketControl(TCPSocket)) == 0) {
        return SOCKET_NOT_FOUND;
    }

    #if !defined ANNOUNCE_MAX_SEGMENT_SIZE
    if (usBufferSpace > TCP_MAX_WINDOW_SIZE) {                           // if we don't accounce the MSS value we keep the windows smaller than it to avoid a possible overrun
        usBufferSpace = TCP_MAX_WINDOW_SIZE;
    }
    #endif
    ptr_TCP->usRxWindowSize = usBufferSpace;                             // set the size to the socket struct
    return TCPSocket;
}


extern signed short fnReportTCPWindow(USOCKET TCPSocket, unsigned short usBufferSpace) // {19}
{
    TCP_CONTROL *ptr_TCP;
    signed short ssReturnValue;
    if ((ptr_TCP = fnGetSocketControl(TCPSocket)) == 0) {
        return SOCKET_NOT_FOUND;
    }

    #if !defined ANNOUNCE_MAX_SEGMENT_SIZE
    if (usBufferSpace > TCP_MAX_WINDOW_SIZE) {                           // if we don't accounce the MSS value we keep the windows smaller than it to avoid a possible overrun
        usBufferSpace = TCP_MAX_WINDOW_SIZE;
    }
    #endif
    ptr_TCP->usRxWindowSize = usBufferSpace;                             // set the size to the socket struct

    if (ptr_TCP->ucTCP_state != TCP_STATE_ESTABLISHED) {
        return SOCKET_STATE_INVALID;                                     // only send an ACK when a connection is established
    }
    ptr_TCP->ucSendFlags = TCP_FLAG_ACK;
    ssReturnValue = fnSendTCPControl(ptr_TCP);                           // send ack to inform of new window size
    if (ssReturnValue == NO_ARP_ENTRY) {                                 // ack was not sent becasue distination address must first be resolved
        ptr_TCP->ucTCPInternalFlags |= WINDOW_UPDATE;                    // mark that a window update is in progress
    }
    return ssReturnValue;
}
#endif                                                                   // end CONTROL_WINDOW_SIZE

#if defined USE_ZERO_CONFIG                                              // {36}
// Check all TCP sockets in an attempt to find any connected one
// - returns 0 if no connected connections exist otherwise returns 1
// - if RESET_CONNECTIONS is passed all active connections are reset
// - if SEARCH_CONNECTION is passed the routine returns as soon as any active connection has been found without resetting it
//
extern int fnActiveTCP_connections(int iReset)
{
    USOCKET sockets;
    TCP_CONTROL *ptr_TCP;

    if (tTCP == 0) {
        return 0;                                                        // ignore if no sockets have been created
    }

    ptr_TCP = tTCP;                                                      // set a local pointer to socket pool

    for (sockets = 0; sockets < NO_OF_TCPSOCKETS; sockets++)    {
        if (ptr_TCP->ucTCP_state > TCP_STATE_LISTEN) {                   // search for a free socket
            if (SEARCH_CONNECTION == iReset) {
                return (1);                                              // connected TCP socket found
            }
            fnAbortTCPSession(ptr_TCP);                                  // send a reset and immediately close the TCP connection
        }
        ptr_TCP++;
    }
    return (0);                                                          // no connected socket found
}
#endif
#endif                                                                   // end USE_TCP
