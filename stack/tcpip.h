/***********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher

    ---------------------------------------------------------------------
    File:      tcpip.h
    Project:   Single Chip Embedded Internet
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2018
    *********************************************************************
    16.02.2007 Add SMTP LOGIN defines
    17.02.2007 Add TFTP support defines
    15.03.2007 Add RARP and VLAN defines
    24.03.2007 Define structures to ensure packing with GNU compiler     {1}
    08.04.2007 Add fnSubnetBroadcast(), added UNETWORK_STATS
    28.04.2007 Add SNMP defines
    01.06.2007 Modify fnVerifyUser() control define                      {2}
    03.06.2007 Add fnGetFreeTCP_Port() and extend fnStartFtp() with timeout and mode
    12.07.2007 ucNegotiate no longer conditional on Telnet               {3}
    21.08.2007 Add HTTP windowing tx support (HTTP_WINDOWING_BUFFERS)    {4}
    29.08.2007 Add content size history (HTTP_WINDOWING_BUFFERS)         {5}
    03.09.2007 Add optional external retrigger of socket idle timer      {6}
    06.11.2007 Add NetBIOS support                                       {7}
    02.01.2008 Remove some packed struct defines which could cause problems with GNU compiler {8}
    06.01.2008 Add tx buffer end to fnInsertHTMLString()                 {9}
    11.01.2008 Extend HTTP struct to include user data pointer and dynamic count array {10}
    12.01.2008 Optionally pass HTTP session information to fnInsertValue(){11}
    13.01.2008 fnStartHTTP's fnInsertRoutine() parameter now uses typedef LENGTH_CHUNK_COUNT as does DynamicCnt {12}
    23.04.2008 Add INFINITE_TIMEOUT define                               {13}
    25.04.2008 Add TCP_STATE_BAD_SOCKET and fnGetTCP_state()             {14}
    18.05.2008 Modify fnEncode64() parameters for general use            {15}
    25.05.2008 Add HTTP states HTTP_STATE_POSTING_PLAIN
    02.07.2008 Add fnResetuNetwork_stats()                               {16}
    09.07.2008 Correct IPV6 length to 16                                 {17}
    07.09.2008 Add POSTING_PARAMETER_DATA_TO_APP                         {18}
    22.09.2008 Add iFetchingInternalMemory to HTTP session struct        {19}
    23.09.2008 Add HTTP state HTTP_STATE_DOING_PARAMETER_POST            {20}
    04.10.2008 Add APP_REQUEST_AUTHENTICATION                            {21}
    04.11.2008 Add conditional on FTP_DATA_WINDOWS                       {22}
    13.11.2008 Correct typo in variable name                             {23}
    13.11.2008 Add flags LAST_DYNAMIC_CONTENT_DATA and GENERATING_DYNAMIC_BINARY {24}
    01.02.2009 Add POSTING_PARTIAL_PARAMETER_DATA_TO_APP                 {25}
    13.06.2009 Add flag DELAY_DYNAMIC_SERVING                            {26}
    07.08.2009 Add SNTP_PORT UDP port number and SNTP defines            {27}
    18.11.2009 Add permanent ARP entry                                   {28}
    23.11.2009 Struct packing control removed to driver.h
    10.01.2010 Add IPV6 network settings and IPV6 structs                {29}
    12.01.2010 Add error codes SOCKET_NOT_FOUND, SOCKET_STATE_INVALID and NO_FREE_PORTS_AVAILABLE {30}
    25.01.2010 Rename fnReportWindow() to fnReportTCPWindow()            {31}
    25.01.2010 Add WINDOW_UPDATE flag                                    {32}
    25.01.2010 Add return value to fnModifyTCPWindow()                   {33}
    02.02.2010 Add IPV6 functions                                        {34}
    15.02.2010 Modify HTTP struct to allow HTTP_DYNAMIC_CONTENT to be disabled {35}
    08.11.2010 Add MAXIMUM_DYNAMIC_INSERTS                               {36}
    17.11.2010 Add optional fall-back DNS server                         {37}
    08.12.2010 Add optional HTTP session pointer passed to the HTTP generator callback {38}
    08.12.2010 Add WEB_BLACKLIST_IP flag                                 {39}
    27.03.2011 Support INDIVIDUAL_BUFFERED_TCP_BUFFER_SIZE (allowing each buffered TCP socket to have its own buffer size) {40}
    16.04.2011 Add variable HTTP port number                             {41}
    06.07.2011 Add IP_IGMPV2 and fnHandleIGMP()                          {42}
    07.07.2011 Add fnStartZeroConfig() and fnCheckZeroConfigCollision()  {43}
    11.07.2011 Add fnActiveTCP_connections() and make fnSendARP_request() extern if USE_ZERO_CONFIG is active {44}
    03.08.2011 Add zero-configuration states                             {45}
    24.10.2011 Change fnServeDelayed() to optionally pass file string rather than just character {46}
    30.10.2011 Add extended buffered TCP options                         {47}
    11.11.2011 Add ftp client functions and callback events              {48}
    11.12.2011 Add fnTCP_IdleTimeout()                                   {49}
    11.03.2012 Add ICMPV6, redefine fnHandleICMPV6() and remove fnHandleIPV6() {50}
    11.03.2012 Add fnHandleIPV6in4()                                     {51}
    11.03.2012 Add members ucIPV4_Protocol, ucIPV6_Protocol, usIPLength and usDataLength to ETHERNET_FRAME {52}
    11.03.2012 Redefine fnHandleIPv4() and fnHandleTCP() and add TCP_OVER_IPV6 {53}
    26.03.2012 Add struct IPV6INV4_RELAY_DESTINATION                     {54}
    26.03.2012 Add fnSetIPv6in4Destinations()                            {55}
    27.03.2012 Add IPv6in4 tunnel IPv4 address                           {56}
    02.04.2012 Modify fnTCP_Connect() parameter when IPv6 is enabled     {57}
    02.04.2012 Add WEB_UTFAT_OFF and FTP_UTFAT_OFF                       {58}
    16.04.2012 Add fnBridgeEthernetFrame()                               {59}
    21.04.2012 Extend Ethernet counters                                  {60}
    12.05.2012 Add multiple networks                                     {61}
    14.05.2012 Change interface to fnGetARPentry()                       {62}
    27.05.2012 Moved ARP states from arp.c                               {63}
    03.06.2012 Add flags to fnFTP_client_connect() and optional IPv6 address pointer to TCP_CLIENT_MESSAGE_BOX {64}
    07.08.2012 Correct interface flag use in socket value                {65}
    05.09.2012 Modified fnSubnetBroadcast() parameters                   {66}
    16.12.2012 Add dynamic VLAN support                                  {67}
    17.12.2012 Correct NetworkInterface() when multiple networks on only one interface {68}
    17.12.2012 Add alternative VLAN ID support                           {69}
    17.12.2012 New fnAddARP() interface                                  {70}
    17.12.2012 Add fnGetTCP_port()                                       {71}
    17.12.2012 Add ARP_BROADCAST_ENTRY                                   {72}
    17.12.2012 Redefine fnHandleUDP()                                    {73}
    14.01.2013 Enable socket masks when multiple networks                {74}
    30.01.2013 Add addVLAN() macro                                       {75}
    30.01.2013 Add phy tail tagging support                              {76}
    27.03.2013 Add fnGetDHCP_host_name()                                 {77}
    02.08.2013 Change fnStartNetBIOS_Server() return parameter           {78}
    06.08.2013 Add IMMEDIATE_MEMORY_ALLOCATION option                    {79}
    12.07.2013 Add usPeerMSS to HTTP                                     {80}
    23.12.2013 Add fnRestrictGatewayInterface()                          {81}
    06.03.2014 Update SNMP for V1 and V2c operation                      {82}
    21.03.2014 Add HTTP_FILE_REFERENCE                                   {83}
    08.04.2014 Add IGMP support                                          {84}
    13.04.2014 Modify defineVLAN() dependency for compatibility with multiple networks without VLAN {85}
    13.04.2014 Add fnAssignNetwork()                                     {86}
    16.04.2014 Add IGMP counters and extend counters to multiple networks{87}
    04.06.2014 Modify HTTP_FILE_REFERENCE to have a local string of definable length {88}
    04.09.2014 Add ucBroadcastResponse to ETHERNET_FRAME                 {89}
    05.09.2014 Add GET_STARTING and CHECK_AUTHENTICATION_ON_PAGE         {90}
    31.10.2014 Add TELNET client support                                 {91}
    07.11.2014 Moved the definition of TCP_DEF_MTU from tcp.c to here and made it dependent on the IPv4/IPv6 usage and limit user's ethernet buffer size {92}
    19.11.2014 Modify fnSendIPV6Discovery() prototype                    {93}
    23.11.2014 Replace fnGetTCP_state() by fnGetSocketControl() and remove TCP_STATE_BAD_SOCKET {94}
    03.12.2014 Add UDP_TX_PROPERTIES and modify fnSendUDP_multicast() prototype {95}
    31.01.2015 Extended SNMP to have read and write communities          {96}
    31.01.2015 Add fnInsertOID_value() and fnExtractOID_value()          {97}
    05.12.2015 Add fnHandleEthernetFrame(), fnEthernetStateChange() and fnGetLinkState() {98}
    12.12.2015 Add network parameter to fnStartDHCP() and fnStopDHCP()   {99}
    10.02.2016 Add return value to fnDecode64()                          {100}
    11.02 2016 Parameters for fnStartHTTP() modified                     {101}
    05.11.2016 Add pseudo flag TCP_FLAG_FIN_RECEIVED                     {102}
    16.02.2017 Add RFC 2217 (Telnet com port control option) mode        {103}
    10.05.2017 Add optional Ethernet frame ucErrorFlags field            {104}
    09.01.2018 Add fnInsertTCPHeader(), fnSecureLayerTransmission() and SECURE_SOCKET_MODE {105}
    20.02.2018 Add buffered TCP extended option TCP_BUF_PREPARE in order to allow preparing data in the output buffer of an open connection but not start its transmission yet {106}

*/

#ifndef _TCP_IP_
#define _TCP_IP_


#define MAC_LENGTH              6                                        // Ethernet MAC hardware address length
#define IPV4_LENGTH             4                                        // IPv4
#define IPV6_LENGTH             16                                       // IPv6 {17}
#define VLAN_TAG_LENGTH         4

#define _IP6_ADD_DIGIT(x)       (unsigned char)(x >> 8), (unsigned char)(x)
#define MAX_IPV6_STRING         40                                       // maximum size of IPV6 address as terminated string

#if !defined IP_NETWORK_COUNT                                            // {61}
    #define IP_NETWORK_COUNT    1                                        // default is for one network
#endif
#if !defined IP_INTERFACE_COUNT
    #define IP_INTERFACE_COUNT  1
#endif
#if !defined DEFAULT_NETWORK                                             // {61}
    #define DEFAULT_NETWORK      (unsigned char)0
#endif
#if !defined DEFAULT_IP_INTERFACE                                        // {61}
    #define DEFAULT_IP_INTERFACE (unsigned char)0
#endif
#if defined SUPPORT_VLAN && defined SUPPORT_DYNAMIC_VLAN_TX && (defined ALTERNATIVE_VLAN_COUNT && ALTERNATIVE_VLAN_COUNT > 0) // {69}
    #define ARP_VLAN_SUPPORT                                             // enable support of VLAN IDs in the ARP table
    #define _ALTERNATIVE_VLAN(uSocket)  (unsigned char)(((uSocket) >> VLAN_SHIFT) & VLAN_MASK) // {69}
#else
    #define _ALTERNATIVE_VLAN(uSocket) 0                                 // {69}
#endif
#if defined USE_SECURE_SOCKET_LAYER                                      // {105}
    #define SECURE_SOCKET_MODE   (1 << ((sizeof(USOCKET) * 8) - 2))      // 0x40, 0x4000 or 0x40000000
    #if IP_INTERFACE_COUNT > 1 || IP_NETWORK_COUNT > 1                   // {74}
        #define _TCP_SOCKET_MASK_ASSIGN(uSocket)  (uSocket &= (SOCKET_NUMBER_MASK))
        #define _TCP_SOCKET_MASK(uSocket)         (USOCKET)((uSocket) & (SOCKET_NUMBER_MASK))
        #define _UDP_SOCKET_MASK_ASSIGN(uSocket)  (uSocket &= (SOCKET_NUMBER_MASK))
        #define _UDP_SOCKET_MASK(uSocket)         (USOCKET)((uSocket) & (SOCKET_NUMBER_MASK))
    #else
        #define _TCP_SOCKET_MASK_ASSIGN(uSocket)  (uSocket &= ~(SECURE_SOCKET_MODE))
        #define _TCP_SOCKET_MASK(uSocket)         (uSocket & ~(SECURE_SOCKET_MODE))
        #define _UDP_SOCKET_MASK_ASSIGN(uSocket)  (uSocket &= ~(SECURE_SOCKET_MODE))
        #define _UDP_SOCKET_MASK(uSocket)         (uSocket & ~(SECURE_SOCKET_MODE))
    #endif
#else
    #if IP_INTERFACE_COUNT > 1 || IP_NETWORK_COUNT > 1                   // {74}
        #define _TCP_SOCKET_MASK_ASSIGN(uSocket)  (uSocket &= (SOCKET_NUMBER_MASK))
        #define _TCP_SOCKET_MASK(uSocket)         (USOCKET)((uSocket) & (SOCKET_NUMBER_MASK))
        #define _UDP_SOCKET_MASK_ASSIGN(uSocket)  (uSocket &= (SOCKET_NUMBER_MASK))
        #define _UDP_SOCKET_MASK(uSocket)         (USOCKET)((uSocket) & (SOCKET_NUMBER_MASK))
    #else
        #define _TCP_SOCKET_MASK_ASSIGN(uSocket)
        #define _TCP_SOCKET_MASK(uSocket)         (uSocket)
        #define _UDP_SOCKET_MASK_ASSIGN(uSocket) 
        #define _UDP_SOCKET_MASK(uSocket)         (uSocket)
    #endif
#endif

/************************** Ethernet Defines *****************************************************************/
#define    ETH_HEADER_LEN          14                                    // fixed ethernet header length
#define    ETH_MTU                 1500                                  // max ethernet frame contents length

#define    PROTOCOL_IPv4           0x0800                                // IPv4 over Ethernet    
#define    PROTOCOL_IPv6           0x86dd                                // IPv6 over Ethernet
#define    PROTOCOL_ARP            0x0806                                // ARP over Ethernet
#define    PROTOCOL_RARP           0x8035                                // RARP over Ethernet
#define    TAG_PROTOCOL_IDENTIFIER 0x8100                                // VLAN Protocol Identifier IEEE 802.1Q 

__PACK_ON                                                                // compilers using pragmas to control packing will start struct packing from here
typedef struct _PACK stETHERNET_II                                       // {1}
{
    unsigned char  destination_mac_address[MAC_LENGTH];    
    unsigned char  source_mac_address[MAC_LENGTH];
    unsigned char  ethernet_type[2];
} ETHERNET_II;

typedef struct _PACK stETHERNET_CONTENT                                  // {1}
{
    unsigned char  ethernet_destination_MAC[MAC_LENGTH];                 // MAC address of destination    
    unsigned char  ethernet_source_MAC[MAC_LENGTH];                      // MAC address of source
    unsigned char  ethernet_frame_type[2];                               // Ethernet frame tye
    unsigned char  ucData[ETH_MTU];                                      // maximum ethernet data packet
} ETHERNET_FRAME_CONTENT;


#define IPV4_PROTOCOL_OFFSET 9                                           // fixed offset in Ethernet frame data where the IPv4 protcol is stored


typedef struct _PACK stETHERNET_FRAME                                    // {1}
{
    ETHERNET_FRAME_CONTENT *ptEth;                                       // pointer to the frame
#if IP_INTERFACE_COUNT > 1
    QUEUE_HANDLE            Tx_handle;                                   // the handle of the interface that responses are to be returned to
#endif
    unsigned short          usIPLength;
    unsigned short          usDataLength;
    unsigned short          frame_size;                                  // physical size of ethernet frame
#if defined IPV4_SUPPORT_RX_DEFRAGMENTATION && defined IP_RX_CHECKSUM_OFFLOAD
    unsigned short          usFragment_CS;                               // checksum of payload fragment in this frame
    unsigned char           ucSpecialHandling;
#endif
#if defined SUPPORT_VLAN && defined SUPPORT_DYNAMIC_VLAN                 // {67}
    unsigned short          usVLAN_ID;                                   // 0xffff when not a VLAN tagged frame or else value
    unsigned char           ucVLAN_content;                              // extra details about the VLAN content
#endif
#if IP_INTERFACE_COUNT > 1
    unsigned char           ucInterface;                                 // the number of the interface that responses are to be returned to
    unsigned char           ucInterfaceHandling;                         // special handling details (eg. when an interface doesn't use Ethernet II)
#endif
#if defined USE_IPV6
    unsigned char           ucIPV4_Protocol;                             // {52}
    unsigned char           ucIPV6_Protocol;
#endif
#if IP_NETWORK_COUNT > 1
    unsigned char           ucNetworkID;                                 // the network that the frame belongs to
    unsigned char           ucBroadcastResponse;                         // {89}
#endif
#if defined PHY_TAIL_TAGGING                                             // {76}
    unsigned char           ucRxPort;                                    // the source port of the receive frame
#endif
#if defined ETH_ERROR_FLAGS
    unsigned char           ucErrorFlags;                                // {104}
#endif
} ETHERNET_FRAME;

#define ETH_ERROR_INVALID_IPv4                0x01                       // not IPv4, or corrupted IPv4 content
#define ETH_ERROR_INVALID_IPv4_CHECKSUM       0x02                       // IPv4 type but with bad IP checksum
#define ETH_ERROR_INVALID_ARP_RARP            0x04                       // not ARP/RARP

#define INTERFACE_NO_RX_CS_OFFLOADING         0x01                       // force receive IP checksum calculation
#define INTERFACE_NO_TX_CS_OFFLOADING         0x02                       // force transmit IP checksum calculation in all cases
#define INTERFACE_NO_TX_PAYLOAD_CS_OFFLOADING 0x04                       // force transmit IP checksum calculation when payload is involved
#define INTERFACE_NO_MAC_FILTERING            0x08                       // the interface operate in promiscuous mode so MAC filtering may be required in software
#define INTERFACE_NO_MAC_ETHERNET_II          0x10                       // the interface uses IP without MAC (eg. PPP)

#define INTERFACE_RX_PAYLOAD_CS_FRAGS         0x40                       // used only locally - do not set as interface characteristic
#define INTERFACE_CALC_TCP_IPv6               0x80                       // used only locally - do not set as interface characteristic

#define VLAN_UNTAG_TX_FRAME                   0x10                       // force transmission to be untagged
#define VLAN_UNTAGGED_FRAME                   0x20                       // {67} this flag indicates that the frame arrived untagged but hasn't been dropped even though VLAN operation is enabled (eg. untagged frames are allowed on certain ports in certain circumstances)
#define VLAN_CONTENT_PRESENT                  0x40                       // the VLAN content has not been removed
#define VLAN_TAGGED_FRAME                     0x80                       // the original frame had VLAN content
#define VLAN_MEMBER_MASK                      0x0f                       // maximum alternative VLAN groups is 15

typedef struct _PACK stETHERNET_STATS                                    // don't change order due to dynamic html references...
{
  unsigned long ulRxTot;                                                 // total frames received by the Ethernet controller
  unsigned long ulLostRx;                                                // lost frames (overrun)

  unsigned long ulRxARP;                                                 // ARP frames received for our IP address
#if defined USE_IGMP
  unsigned long ulRxIGMP;                                                // {87} IGMP frames received for our IP address
#endif
  unsigned long ulRxICMP;                                                // ICMP frames received for our IP address
  unsigned long ulRxUDP;                                                 // UDP frames received for our IP address
  unsigned long ulRxTCP;                                                 // TCP frames received for our IP address

  unsigned long ulCheckSumError;                                         // TCP/IP frames discarded due to check sum error
  unsigned long ulOtherProtocol;                                         // Other received TCP/IP frames using non-supported protocol

  unsigned long ulForeignRxARP;                                          // foreign ARP frames seen on the network
  unsigned long ulForeignRxICMP;                                         // foreign ICMP frames seen on the network
  unsigned long ulForeignRxUDP;                                          // foreign UDP frames seen on the network
  unsigned long ulForeignRxTCP;                                          // foreign TCP frames seen on the network

  unsigned long ulTxTot;                                                 // total frames sent from the Ethernet controller
  unsigned long ulTxARP;                                                 // ARP frames sent from the Ethernet controller
#if defined USE_IGMP
  unsigned long ulTxIGMP;                                                // {87} IGMP frames sent from the Ethernet controller
#endif
  unsigned long ulTxICMP;                                                // ICMP frames sent from the Ethernet controller
  unsigned long ulTxUDP;                                                 // UDP frames sent from the Ethernet controller
  unsigned long ulTxTCP;                                                 // TCP frames sent from the Ethernet controller

  unsigned long ulOthers;                                                // non-specified Ethernet events

#if defined USE_IPV6
  unsigned long ulRxICMPV6;                                              // total amount of ICMPV6 receptions
  unsigned long ulTxICMPV6;                                              // total amount of ICMPV6 transmissions
  unsigned long ulRxUDPV6;                                               // total amount of UDP over IPv6 receptions {60}
  unsigned long ulTxUDPV6;                                               // total amount of UDP over IPv6 transmissions
  unsigned long ulRxTCPV6;                                               // total amount of TCP over IPv6 receptions
  unsigned long ulTxTCPV6;                                               // total amount of TCP over IPv6 transmissions
#endif
} ETHERNET_STATS;



// Warning: the following must match the array structure of Ethernet stats
//
#if defined USE_IGMP                                                     // {87}
    #define TOTAL_RX_FRAMES                     0                        // HTTP a
    #define TOTAL_LOST_RX_FRAMES                1                        // HTTP b
    #define RECEIVED_ARP_FRAMES                 2                        // HTTP c
    #define RECEIVED_IGMP_FRAMES                3                        // HTTP d
    #define RECEIVED_ICMP_FRAMES                4                        // warning: keep order of ICMP/UDP and TCP here
    #define RECEIVED_UDP_FRAMES                 5                        // HTTP f
    #define RECEIVED_TCP_FRAMES                 6                        // HTTP g
    #define DISCARDED_CHECKSUM_FRAMES           7                        // HTTP h
    #define UNSUPPORTED_PROTOCOL_FRAMES         8                        // HTTP i

    #define SEEN_FOREIGN_ARP_FRAMES             9                        // HTTP j
    #define SEEN_FOREIGN_ICMP_FRAMES            10                        // ..and here
    #define SEEN_FOREIGN_UDP_FRAMES             11                       // HTTP l
    #define SEEN_FOREIGN_TCP_FRAMES             12                       // HTTP m
    #define LAST_RX_COUNTER                     SEEN_FOREIGN_TCP_FRAMES

    #define TOTAL_TX_FRAMES                     13                       // HTTP n
    #define SENT_ARP_FRAMES                     14                       // HTTP o
    #define SENT_IGMP_FRAMES                    15                       // HTTP p
    #define SENT_ICMP_FRAMES                    16                       // HTTP q
    #define SENT_UDP_FRAMES                     17                       // HTTP r
    #define SENT_TCP_FRAMES                     18                       // HTTP s
    #define LAST_TX_COUNTER                     SENT_TCP_FRAMES

    #define TOTAL_OTHER_EVENTS                  19                       // HTTP t

    // IPV6 counters
    //
    #define RECEIVED_ICMPV6_FRAMES              18                       // HTTP u
    #define TRANSMITTED_ICMPV6_FRAMES           19                       // HTTP v
    #define RECEIVED_UDPV6_FRAMES               20                       // HTTP w {60}
    #define TRANSMITTED_UDPV6_FRAMES            21                       // HTTP x
    #define RECEIVED_TCPV6_FRAMES               22                       // HTTP y
    #define TRANSMITTED_TCPV6_FRAMES            23                       // HTTP y
#else
    #define TOTAL_RX_FRAMES                     0                        // HTTP a
    #define TOTAL_LOST_RX_FRAMES                1                        // HTTP b
    #define RECEIVED_ARP_FRAMES                 2                        // HTTP c
    #define RECEIVED_ICMP_FRAMES                3                        // warning: keep order of ICMP/UDP and TCP here
    #define RECEIVED_UDP_FRAMES                 4                        // HTTP e
    #define RECEIVED_TCP_FRAMES                 5                        // HTTP f
    #define DISCARDED_CHECKSUM_FRAMES           6                        // HTTP g
    #define UNSUPPORTED_PROTOCOL_FRAMES         7                        // HTTP h

    #define SEEN_FOREIGN_ARP_FRAMES             8                        // HTTP i
    #define SEEN_FOREIGN_ICMP_FRAMES            9                        // ..and here
    #define SEEN_FOREIGN_UDP_FRAMES             10                       // HTTP k
    #define SEEN_FOREIGN_TCP_FRAMES             11                       // HTTP l
    #define LAST_RX_COUNTER                     SEEN_FOREIGN_TCP_FRAMES

    #define TOTAL_TX_FRAMES                     12                       // HTTP m
    #define SENT_ARP_FRAMES                     13                       // HTTP n
    #define SENT_ICMP_FRAMES                    14                       // HTTP o
    #define SENT_UDP_FRAMES                     15                       // HTTP p
    #define SENT_TCP_FRAMES                     16                       // HTTP q
    #define LAST_TX_COUNTER                     SENT_TCP_FRAMES

    #define TOTAL_OTHER_EVENTS                  17                       // HTTP r

    // IPV6 counters
    //
    #define RECEIVED_ICMPV6_FRAMES              18                       // HTTP s
    #define TRANSMITTED_ICMPV6_FRAMES           19                       // HTTP t
    #define RECEIVED_UDPV6_FRAMES               20                       // HTTP u {60}
    #define TRANSMITTED_UDPV6_FRAMES            21                       // HTTP v
    #define RECEIVED_TCPV6_FRAMES               22                       // HTTP w
    #define TRANSMITTED_TCPV6_FRAMES            23                       // HTTP x
#endif


typedef struct _PACK stUNETWORK_STATS                                    // don't change order due to dynamic html references...
{
  unsigned long ulBroadcast_tx;
  unsigned long ulTx_frames;
  unsigned long ulRx_frames;      
#if defined UPROTOCOL_WITH_RETRANS
  unsigned long ulRx_ack;
  unsigned long ulRxSynch_error;
  unsigned long ulTx_rep;
  unsigned long ulTx_lost;
  unsigned long ulTx_ack;
  unsigned long ulRx_bad_ack;
#endif
} UNETWORK_STATS;

#define UNETWORK_BROADCAST_TX               0                            // HTTP A
#define UNETWORK_TX_FRAMES                  1                            // HTTP B
#define UNETWORK_RX_FRAMES                  2                            // HTTP C
#define UNETWORK_RX_ACKS                    3                            // HTTP D
#define UNETWORK_RX_SYNCH_ERRORS            4                            // HTTP E
#define UNETWORK_TX_REPS                    5                            // HTTP F
#define UNETWORK_TX_LOST                    6                            // HTTP G
#define UNETWORK_TX_ACKS                    7                            // HTTP H
#define UNETWORK_RX_BADACK                  8                            // HTTP I

// Warning: the following must match the array structure of Ethernet stats

/************************** ARP Defines *****************************************************************/
#define ETHERNET_HARDWARE       0x0001                                   // hardware is ethernet

typedef struct _PACK stARP_FRAME                                         // {1}
{
    unsigned char ucHardwareType[2];
    unsigned char ucProtocolType[2];
    unsigned char ucHardware_size;
    unsigned char ucProtocolSize;
    unsigned char ucOpCode[2];
    unsigned char ucSenderMAC_address[MAC_LENGTH];
    unsigned char ucSender_IP_address[IPV4_LENGTH];
    unsigned char ucTargetMAC_address[MAC_LENGTH];
    unsigned char ucTarget_IP_address[IPV4_LENGTH];
} ARP_FRAME;

typedef struct _PACK stETHERNET_ARP_FRAME                                // {1}
{
    ETHERNET_II   ethernet_header;
    ARP_FRAME     arp_content;
} ETHERNET_ARP_FRAME;

__PACK_OFF

typedef struct stARP_TAB                                                 // {8} don't force packed
{
    unsigned char ucState;                                               // present state of this ARP entry
    UTASK_TASK    OwnerTask;                                             // a task can be woken on low level errors
    USOCKET       OwnerSocket;                                           // a task can be informed in more detail about a specific socket
#if IP_INTERFACE_COUNT > 1
    QUEUE_HANDLE  Tx_handle;                                             // handle of the interface that has resolved an entry
#endif
#if defined ARP_VLAN_SUPPORT                                             // {70}
    unsigned short usEntryVLAN_ID;                                       // VLAN ID that the entry is valid for (0xffff means no VLAN tag)
#endif
#if IP_NETWORK_COUNT > 1
    unsigned char ucNetworkID;                                           // the network that this entry belongs to
#endif
#if IP_INTERFACE_COUNT > 1
    unsigned char ucInterface;                                           // the interface(s) that this entry requested on
#endif
    unsigned char ucRetries;                                             // number of retries to resolve IP address
    unsigned char ucTimeToLive;                                          // remaining time to live for this entry
    unsigned char ucMac[MAC_LENGTH];                                     // MAC address of entry
    unsigned char ucIP[IPV4_LENGTH];                                     // IP address of entry
} ARP_TAB;

typedef struct stARP_DETAILS                                             // {69}
{
#if IP_INTERFACE_COUNT > 1
    QUEUE_HANDLE  Tx_handle;                                             // the interface handle associated with the ARP entry
#endif
#if defined ARP_VLAN_SUPPORT
    unsigned short usVLAN_ID;                                            // VLAN ID for checking with ARP entries (0xffff means no VLAN tag)
#endif
    unsigned char ucType;                                                // the type of ARP entry (ARP_FIXED_IP, ARP_TEMP_IP, ARP_PERMANENT_IP)
#if IP_NETWORK_COUNT > 1
    unsigned char ucNetworkID;                                           // the network that ARP activity belongs to
#endif
} ARP_DETAILS;

#ifndef MAX_HW_ADDRESS_LENGTH
    #define MAX_HW_ADDRESS_LENGTH     MAC_LENGTH
#endif

typedef struct stNEIGHBOR_TAB                                            // {34}
{
    unsigned char ucState;                                               // present state of this ARP entry
    UTASK_TASK    OwnerTask;                                             // a task can be woken on low level errors
    USOCKET       OwnerSocket;                                           // a task can be informed in more detail about a specific socket
    unsigned char ucRetries;                                             // number of retries to resolve IP address
    unsigned char ucTimeToLive;                                          // remaining time to live for this entry
    unsigned char ucHWAddress[MAX_HW_ADDRESS_LENGTH];                    // MAC address of entry
    unsigned char ucHWAddressLength;                                     // hardware address entry length
    unsigned char ucIPV6[IPV6_LENGTH];                                   // IPV6 address of entry
#if IP_INTERFACE_COUNT > 1
    QUEUE_HANDLE  Tx_handle;                                             // handle of the interface that has resolved an entry
#endif
#if defined ARP_VLAN_SUPPORT
    unsigned short usEntryVLAN_ID;                                       // VLAN ID that the entry is valid for (0xffff means no VLAN tag)
#endif
#if IP_NETWORK_COUNT > 1
    unsigned char ucNetworkID;                                           // the network that this entry belongs to
#endif
#if IP_INTERFACE_COUNT > 1
    unsigned char ucInterface;                                           // the interface(s) that this entry requested on
#endif
} NEIGHBOR_TAB;

// Arp Entry Types
//
#define ARP_FIXED_IP        0x01                                         // for Fixed addresses like GW - entry is automatically refreshed after timeout
#define ARP_TEMP_IP         0x02                                         // for Temporary addresses - entry is removed after timeout
#define ARP_PERMANENT_IP    0x04                                         // {28} for permanent addresses - these never timeout and so never need refreshing
#define ARP_BROADCAST_ENTRY 0x08                                         // {72}

// Arp Entry States (sharing flags with entry type)                      {63}
//
#define ARP_FREE            0x00                                         // entry is Unused (initial value)        
#define ARP_RESOLVING       0x40                                         // entry is being resolved
#define ARP_RESOLVED        0x80                                         // entry has been resolved    
#define ARP_STATE_MASK      (ARP_RESOLVING | ARP_RESOLVED)               // mask bits in entry state so that other flags can be used


#define NO_ARP_ENTRY          -1                                         // ARP error codes



/************************** IP Defines *****************************************************************/
#define IP_VERSION_4            (4 << 4)
#define IP_VERSION_MASK         0xf0                                     // field masks
#define IP_HEADER_LENGTH_MASK   0x0f
#define IP_MIN_HLEN             20                                       // IP Header Length in bytes (without options)
#define IP_DEFAULT_VERSION_HEADER_LENGTH  (unsigned char)(IP_VERSION_4 | (IP_MIN_HLEN/4))
#define MAX_IP_OPTLEN           40                                       // max. IP Header option field length
#define ROUTER_ALERT_OPTION_LENGTH 4
//#define MAX_IPV6_OPTLEN       0
#define IP_MAX_HLEN             (IP_MIN_HLEN + MAX_IP_OPTLEN)
#if defined IP_USE_TX_OPTIONS
    #define IP_MAX_TX_HLEN      (IP_MAX_HLEN)
#else
    #define IP_MAX_TX_HLEN      (IP_MIN_HLEN)
#endif

#if defined LAN_BUFFER_SIZE > 1514                                       // {92} check the LAN buffer size that the user wants to configure
    #define _LAN_BUFFER_SIZE     1514                                    // maximum encapsulated Ethernet length (without CRC-32 trailer)
#else
    #define _LAN_BUFFER_SIZE     LAN_BUFFER_SIZE                         // accept the user's length
#endif

#define IP_ICMP                 1                                        // 0x01 - ICMP over IP
#define IP_UDP                  17                                       // 0x11 - UDP over IP
#define IP_TCP                  6                                        // 0x06 - TCP over IP
#define IP_IGMPV2               2                                        // 0x02 - IGMP over IP {42}
#define IP_6IN4                 41                                       // 0x29 - IPV6 in IPV4 tunneling
#define IP_ICMPV6               58                                       // 0x3a - ICMPV6 {50}
#define IPV6_FRAME              0x40                                     // flag used to signal that the protocol carried over IPv6
#define FOREIGN_FRAME           0x80                                     // flag used to signal that the protocol is not addressing us


// IP frament flags and fragment offset field
//
#define IP_FRAGMENT_RESERVED        0x8000                               // reserved - must be zero
#define IP_DONT_FRAGMENT            0x4000                               // don't fragment flag            
#define IP_MORE_FRAGMENTS           0x2000                               // more fragments bit            
#define IP_FRAGMENT_OFFSET_BITS     0x1FFF                               // fragment offset mask

#define TOS_NORMAL_SERVICE          0x00                                 // normal type-of-service
#define TOS_MINIMISE_DELAY          0x10
#define TOS_MAXIMISE_THROUGHPUT     0x08
#define TOS_MAXIMISE_RELIABILITY    0x04
#define TOS_MINIMISE_MONETARY_COST  0x02

#define TTL_1                       1                                    // TTL of 1 is usually used for multicast frames
#define MAX_TTL                     0x80

__PACK_ON
typedef struct _PACK stIP_PACKET                                         // {1}
{
    // IPv4 structure
    //
    unsigned char  version_header_length;                                // version and header length
    unsigned char  differentiatedServicesField;                          // differentiated services field
    unsigned char  total_length[2];                                      // total length
    unsigned char  identification[2];                                    // identification
    unsigned char  fragment_offset[2];                                   // fragments and offset
    unsigned char  time_to_live;                                         // time to live
    unsigned char  ip_protocol;                                          // protocol eg. ICMP, UDP or TCP
    unsigned char  ip_checksum[2];                                       // header checksum
    unsigned char  source_IP_address[IPV4_LENGTH];                       // source IP address
    unsigned char  destination_IP_address[IPV4_LENGTH];                  // destination IP address
    unsigned char  ip_options[MAX_IP_OPTLEN];                            // options - if available
} IP_PACKET;

typedef struct _PACK stIP_PACKET_V6                                      // {29}
{
    // IPv6 structure
    //
    unsigned char  version_traffic_class;                                // version and traffic class (high nibble)
    unsigned char  traffic_class_flow;                                   // traffic class (low nibble) and flow label (high nibble)
    unsigned char  flow_lable[2];                                        // flow label (lower 4 nibbles)
    unsigned char  payload_length[2];                                    // pay load length (0..64k)
    unsigned char  next_header;                                          // next header
    unsigned char  hop_limit;                                            // hop limit
    unsigned char  source_IP_address[IPV6_LENGTH];                       // source IPV6 address
    unsigned char  destination_IP_address[IPV6_LENGTH];                  // destination IPV6 address
    #if defined MAX_IPV6_OPTLEN
    unsigned char  ip_v6_options[MAX_IPV6_OPTLEN];                       // options - if available
    #endif
} IP_PACKET_V6;
            
#define IP_VERSION_6             (6 << 4)
#define IPV6_SUBNET_LENGTH       8


#define SOURCE_LINK_LAYER_ADDRESS 1
#define TARGET_LINK_LAYER_ADDRESS 2
#define ICMPV6_OPTION_LENGTH_8    1


/************************** ICMP defines *****************************************************************/

#define ECHO_PING_REPLY                    0x00
#define DESTINATION_UNREACHABLE            0x03
#define ECHO_PING                          0x08

#define ICMP_HEADER_LENGTH                 8

#define PORT_UNREACHABLE                   0x03

#define REPORTING_LENGTH_UDP_UNREACHABLE   8                             // it is typical to report first 8 bytes at least
#define ICMP_ERROR_HEADER_LENGTH           8                             // the fields from ICMP type to and including ucICMP_variable[4]

#define PING_DATA_LENGTH 26
typedef struct _PACK stPING_FRAME                                        // {1}
{
    unsigned char  ucICMPType;                                           // ICMP type
    unsigned char  ucICMPCode;                                           // ICMP code
    unsigned char  ucICMPCheckSum[2];                                    // checksum of ICMP message
    unsigned char  ucICMPIdent[2];                                       // Ping identifier
    unsigned char  ucICMPSequence[2];                                    // Ping sequence number
    unsigned char  ucICMPData[PING_DATA_LENGTH];
} PING_FRAME;

typedef struct _PACK stICMP_ERROR                                        // {1}
{
    unsigned char  ucICMPType;                                           // ICMP type
    unsigned char  ucICMPCode;                                           // ICMP code
    unsigned char  ucICMPCheckSum[2];                                    // checksum of ICMP message
    unsigned char  ucICMP_variable[4];                                   // these fields vary depending on ICMP type
    IP_PACKET      tCopy_IP_header;                                      // space for a copy of the original IP header including possible options
    unsigned char  ucIP_dataGram_contents[REPORTING_LENGTH_UDP_UNREACHABLE];
} ICMP_ERROR;

#define PING_RESULT                       1                              // ICMP internal messages
#define PING_IPV6_RESULT                  2

typedef struct _PACK stICMPV6                                            // {29}
{
    unsigned char  ucICMPV6Type;                                         // ICMPV6 type
    unsigned char  ucICMPV6Code;                                         // ICMPV6 code
    unsigned char  ucICMPV6CheckSum[2];                                  // checksum of ICMPV6 message
    unsigned char  ucICMPV6Flags[4];                                     // flags
    unsigned short target_IP_address[IPV6_LENGTH/sizeof(unsigned short)];// target IPV6 address
    unsigned char  ucICMPV6_option_type;
    unsigned char  ucICMPV6_option_length;
    unsigned char  ucICMPV6_option_data[6];                              // length defined in the option
} ICMPV6;

#define IPV6_PING_DATA_LENGTH 26                                         // data length same as 'a' .. 'z'

typedef struct _PACK stICMPV6_ECHO_REQUEST                               // {29}
{
    unsigned char  ucICMPV6Type;                                         // ICMPV6 type
    unsigned char  ucICMPV6Code;                                         // ICMPV6 code
    unsigned char  ucICMPV6CheckSum[2];                                  // checksum of ICMPV6 message
    unsigned char  ucICMPV6ID[2];                                        // ID
    unsigned char  ucICMPV6sequence[2];                                  // sequence
    unsigned char  ucICMPV6_data[IPV6_PING_DATA_LENGTH];                 // data field
} ICMPV6_ECHO_REQUEST;

// ucICMPV6Flags[0]
//
#define OVERRIDE             0x20
#define SOLICITED_RESPONSE   0x40
#define AM_ROUTER            0x80

// ICMPV6 errors according to RFC 2463
//
#define ICMPV6_ERROR_DESTINATION_UNREACHABLE                         1   // Destination Unreachable
#define ICMPV6_ERROR_PACKET_TOO_BIG                                  2   // Packet Too Big
#define ICMPV6_ERROR_TIME_EXCEEDED                                   3   // Time Exceeded
#define ICMPV6_ERROR_PARAMETER_PROBLEM                               4   // Parameter Problem


// ICMPV6 information messages
//
#define ICMPV6_TYPE_ECHO_REQUEST                                     128 // Echo Request - RFC 2463
#define ICMPV6_TYPE_ECHO_REPLY                                       129 // Echo Reply - RFC 2463
#define ICMPV6_TYPE_MULTICAST_LISTENER_QUERY                         130 // Multicast Listener Query - RFC 2710 and RFC 3810
#define ICMPV6_TYPE_V1_MULTICAST_LISTENER_REPORT                     131 // Version 1 Multicast Listener Report - RFC 2710
#define ICMPV6_TYPE_MULTICAST_LISTENER_DONE                          132 // Multicast Listener Done - RFC 2710
#define ICMPV6_TYPE_ROUTER_SOLICITATION                              133 // Router Solicitation - RFC 2461
#define ICMPV6_TYPE_ROUTER_ADVERTISEMENT                             134 // Router Advertisement - RFC 2461
#define ICMPV6_TYPE_NEIGHBOR_SOLICITATION                            135 // Neighbor Solicitation - RFC 2461
#define ICMPV6_TYPE_NEIGHBOR_ADVERTISEMENT                           136 // Neighbor Advertisement - RFC 2461
#define ICMPV6_TYPE_REDIRECT                                         137 // Redirect - RFC 2461
#define ICMPV6_TYPE_ROUTER_RENUMBERING                               138 // Router Renumbering
#define ICMPV6_TYPE_ICMP_NODE_INFO_QUERY                             139 // ICMP Node Information Query
#define ICMPV6_TYPE_ICMP_NODE_INFO_RESPONSE                          140 // ICMP Node Information Response
#define ICMPV6_TYPE_INVERSE_NEIGHBOR_DISCOVERY_SOLICITATION_MESSAGE  141 // Inverse Neighbor Discovery Solicitation Message - RFC 3122
#define ICMPV6_TYPE_INVERSE_NEIGHBOR_DISCOVERY_ADVERTISEMENT_MESSAGE 142 // Inverse Neighbor Discovery Advertisement Message - RFC 3122
#define ICMPV6_TYPE_V2_MULTICAST_LISTENER_REPORT                     143 // Version 2 Multicast Listener Report - RFC 3810
#define ICMPV6_TYPE_HOME_AGENT_ADDRESS_DISCOVERY_REQUEST_MESSAGE     144 // Home Agent Address Discovery Request Message - RFC 3775
#define ICMPV6_TYPE_HOME_AGENT_ADDRESS_DISCOVERY_REPLY_MESSAGE       145 // Home Agent Address Discovery Reply Message - RFC 3775
#define ICMPV6_TYPE_MOBILE_PREFIX_SOLICITATION                       146 // Mobile Prefix Solicitation - RFC 3775
#define ICMPV6_TYPE_MOBILE_PREFIX_ADVERTISEMENT                      147 // Mobile Prefix Advertisement - RFC 3775
#define ICMPV6_TYPE_CERTIFICATION_PATH_SOLICITATION_MESSAGE          148 // Certification Path Solicitation Message - RFC 3971
#define ICMPV6_TYPE_CERTIFICATION_PATH_ADVERTISEMENT_MESSAGE         149 // Certification Path Advertisement Message - RFC 3971
#define ICMPV6_TYPE_MULTICAST_ROUTER_ADVERTISEMENT                   151 // Multicast Router Advertisement 	
#define ICMPV6_TYPE_MULTICAST_ROUTER_SOLICITATION                    152 // Multicast Router Solicitation 	
#define ICMPV6_TYPE_MULTICAST_ROUTER_TERMINATION                     153 // Multicast Router Termination 


typedef struct _PACK stIPV6_DISCOVERY_FRAME                              // {29}
{
    ETHERNET_II    ethernetII;
    IP_PACKET_V6   ipv6;
    ICMPV6         icmpv6;
} IPV6_DISCOVERY_FRAME;


typedef struct _PACK stIPV6_DISCOVERY_FRAME_RX
{
    IP_PACKET_V6   ipv6;
    ICMPV6         icmpv6;
} IPV6_DISCOVERY_FRAME_RX;


typedef struct _PACK stIPV6INV4_RELAY_DESTINATION                        // {54}
{
    unsigned char ucIPV6_destination[IPV6_LENGTH];                       // global IPv6 address of destination to relay IPv6in6 frames to
    unsigned char ucIPV4_destination[IPV4_LENGTH];                       // IPv4 address of destination to relay IPv6in6 frames to
    unsigned char ucMAC_destination[MAC_LENGTH];                         // MAC address of destination to relay IPv6in6 frames to
} IPV6INV4_RELAY_DESTINATION;
__PACK_OFF

/************************** UDP Defines *****************************************************************/

typedef struct stUDP_TAB                                                 // {8} don't force packed
{                                                                        // call back function for this socket
    int     (*fnListener)(USOCKET, unsigned char, unsigned char *, unsigned short, unsigned char *, unsigned short );
    unsigned char  ucState;                                              // present state of this UDP entry
    unsigned char  ucServiceType;                                        // type of UDP service
    unsigned short usLocalPort;                                          // local UDP port of socket
    unsigned char  ucOptions;                                            // options (checkum)
} UDP_TAB;

__PACK_ON
typedef struct _PACK stUDP_HEADER                                        // {1}
{
    unsigned short  usSourcePort;
    unsigned short  usDestinationPort;
    unsigned short  usLength;
    unsigned short  ucCheckSum;
} UDP_HEADER;
__PACK_OFF

#define UDP_HLEN            8                                            // UDP Header length
#define UDP_SEND_MTU        (ETH_MTU - UDP_HLEN - IP_MAX_TX_HLEN)

#define UDP_HEADER_SPACE    0,0,0,0,0,0,0,0                              // can be used to pre-define UDP header to zero

#define UDP_OPT_SEND_CS     0x01                                         // open a socket with this option to send data with extra check sum security
#define UDP_OPT_CHECK_CS    0x02                                         // check receive check sum
#define UDP_OPT_NO_LOOPBACK 0x04                                         // pseudo-option to not loop back multicast transmission to group hosts on the interface


#define UDP_EVENT_RXDATA                 1                               // ARP/UDP events
#define UDP_EVENT_PORT_UNREACHABLE       2
#define ARP_RESOLUTION_FAILED            3
#define ARP_RESOLUTION_SUCCESS           4
#define UDP_NO_ACK                       5
#define NN_RESOLUTION_FAILED             6
#define NN_RESOLUTION_SUCCESS            7

#define NO_LISTENER_DEFINED              -11                             // UDP error codes
#define NO_FREE_UDP_SOCKETS              -12
#define INVALID_SOCKET_HANDLE            -13
#define INVALID_LOCAL_PORT               -14
#define INVALID_DEST_IP                  -15
#define INVALID_REMOTE_PORT              -16
#define SOCKET_CLOSED                    -17
#define ZERO_PORT                        -18
#define NO_UDP_LISTENER_FOUND            -19
#define UDP_NOT_INITIALISED              -20

typedef struct stUDP_TX_PROPERTIES                                       // {95}
{
    USOCKET        SocketHandle;
    unsigned char  *ucIPAddress;
    unsigned short usSourcePort;
    unsigned short usDestinationPort;
    unsigned char  ucOptions;
    unsigned char  ucTTL;
    unsigned char  ucTOS;
} UDP_TX_PROPERTIES;


// Error codes
//
#define DHCP_REBOOT_REJECTED             -1
#define NO_UDP_SOCKET_FOR_DHCP           -2
#define NO_SUPPORT_FOR_DHCP_OVERLOAD     -3
#define NO_VALID_DHCP_MSG                -4
#define DHCP_ADDRESS_ALREADY_EXISTS      -5
#define INVALID_DHCP_PACKET              -6
#define NO_OPTIONS_ALLOWED_IN_DHCP_STATE -7
#define NOT_DHCP_SOCKET                  -8
#define FOREIGN_DHCP_PACKET              -9
#define BAD_MAGIC_COOKIE                 -10
#define NO_UDP_SOCKET_FOR_DHCP_SERVER    -11

// DHCP states
//
#define DHCP_INIT                        0x00                            // state not yet active
#define DHCP_STATE_INIT_REBOOT           0x01                            // initialising, trying to re-obtain parameters
#define DHCP_STATE_INIT                  0x02                            // initialising with not parameters
#define DHCP_STATE_REBOOTING             0x04                            // rebooting (with preferred parameters)
#define DHCP_STATE_SELECTING             0x08                            // selecting DHCP server
#define DHCP_STATE_REQUESTING            0x10                            // offer received from DHCP server and we are requesting
#define DHCP_STATE_BOUND                 0x20                            // we have received all parameters and are using them (possibly until lease times out)
#define DHCP_STATE_RENEWING              0x40                            // renewing after lease timeout
#define DHCP_STATE_REBINDING             0x80                            // last attempt to reobtain lease

// Zero configuration states                                             // {45}
//
#define ZERO_CONFIG_OFF                  0
#define ZERO_CONFIG_PROBING              1
#define ZERO_CONFIG_ACTIVE_DEFENDED      2                               // within a ten second period since defending the address after a previous collision
#define ZERO_CONFIG_ACTIVE               3


// SNTP defines                                                          // {27}
//
#define NTP_FLAG_LI_MASK                   0xc0                          // leap indicator mask field
#define NTP_FLAG_LI_NO_WARNING             0x00
#define NTP_FLAG_LI_61_SEC                 0x40
#define NTP_FLAG_LI_59_SEC                 0x80
#define NTP_FLAG_LI_CLOCK_NOT_SYNCHRONISED 0xc0
#define NTP_FLAG_VN_MASK                   0x38
#define NTP_FLAG_VN_3                      0x18
#define NTP_FLAG_VN_4                      0x20
#define NTP_FLAG_MODE_MASK                 0x07
#define NTP_FLAG_MODE_SYMMETRIC_ACTIVE     0x01
#define NTP_FLAG_MODE_SYMMETRIC_PASSIVE    0x02
#define NTP_FLAG_MODE_CLIENT               0x03
#define NTP_FLAG_MODE_SERVER               0x04
#define NTP_FLAG_MODE_BROADCAST            0x05
#define NTP_FLAG_MODE_RES_NTP_CONTROL      0x06
#define NTP_FLAG_MODE_RES_PRIVATE_USE      0x07

#define STRATUM_KISS_O_DEATH               0x00
#define STRATUM_PRIMARY_REFERENCE          0x01
#define STRATUM_SECONDARY_REFERENCE_MAX    0x0f

typedef struct stNTP_FRAME
{
    unsigned char  ucFlags;
    unsigned char  ucPeerClockStratum;
    unsigned char  ucPeerPollingInterval;
    unsigned char  ucPeerClockPrecision;
    unsigned char  ucRootDelay[4];
    unsigned char  ucRootDispersion[4];
    unsigned char  ucReferenceClockID[4];
    unsigned char  ucReferenceClockUpdateTime[8];
    unsigned char  ucOriginateTimeStamp[8];
    unsigned char  ucReceiveTimeStamp[8];
    unsigned char  ucTransmitTimeStamp[8];
  //unsigned char  ucKeyIndicator[4];                                    // optional for NTP authentication
  //unsigned char  ucMessageDigest[16];                                  // optional for NTP authentication
} NTP_FRAME;

typedef struct stSNTP_TIME
{
    unsigned long  ulSeconds;
    unsigned long  ulFraction;
} SNTP_TIME;


#define SNTP_SYNCHRONISED             0
#define USE_OTHER_SNTP_SERVER         -1
#define SNTP_SERVER_NOT_SYNCHRONISED  -2
#define SNTP_SERVER_FORMAT_ERROR      -3
#define SNTP_SERVER_MODE_INVALID      -4

/************************** IGMP defines *****************************************************************/
#if defined USE_IGMP
    typedef struct stIGMP_HOST                                           // {84}
    {
        unsigned char  ucMulticast_group_address[IPV4_LENGTH];           // multicast address of multicast group the host belongs to
        DELAY_LIMIT    report_delay;                                     // queued repetition
        USOCKET        igmp_host_details;                                // details about which networks/interfaces the group is on
    #if IP_INTERFACE_COUNT > 1
        USOCKET        delayed_host_details;
    #endif
        unsigned char  ucProcessCount;                                   // number of processes that belong to the host group
    #if defined USE_IGMP_V2 || defined USE_IGMP_V3
        unsigned char  ucReporter;
    #endif
        void (*process_call_back[IGMP_MAX_PROCESSES])(int iHostID, unsigned short usSourcePort, unsigned short usRemotePort, unsigned char *ptrBuf, unsigned short usDataLen);
    } IGMP_HOST;
#endif

extern int  fnJoinMulticastGroup(unsigned char ucMulticastGroupAddress[IPV4_LENGTH], USOCKET host_details, void (*call_back)(int iHostID, unsigned short usSourcePort, unsigned short usRemotePort, unsigned char *ptrBuf, unsigned short usDataLen));
extern int  fnLeaveMulticastGroup(int iHostID);
extern void fnReportIGMP(void);
extern void fnHandleMulticastRx(USOCKET SocketHandle, int iHostID, unsigned char *dest_IP, unsigned short usSourcePort, unsigned short usRemotePort, unsigned char *ptrBuf, unsigned short usDataLen);
//extern signed short fnSendUDP_multicast(USOCKET SocketHandle, int iHostID, unsigned char *dest_IP, unsigned short usSourcePort, unsigned short usRemotePort, unsigned char *ptrBuf, unsigned short usDataLen, unsigned char ucOptions, unsigned char ucTTL);
extern signed short fnSendUDP_multicast(int iHostID, UDP_TX_PROPERTIES *ptrUDP, unsigned char  *ptrBuf, unsigned short usDataLen); // {95}
    #define BAD_MULTICAST_ADDRESS          -2

__PACK_ON
typedef struct _PACK stIGMP_FRAME
{
    unsigned char  ucIGMPType;                                           // IGMP type
    unsigned char  ucIGMPResponseTime;                                   // maximum response time
    unsigned char  ucIGMPCheckSum[2];                                    // checksum of IGMP message
    unsigned char  ucIGMPMulticastAddress[IPV4_LENGTH];                  // multicast address
} IGMP_V1_V2_FRAME;
__PACK_OFF

#define IGMP_VERSION_1                      0                            // version used on an interface
#define IGMP_VERSION_2                      1
#define IGMP_VERSION_3                      2

#define IGMP_V1_QUERY                       0x11
#define IGMP_V1_REPORT                      0x12
#define IGMP_V2_MEMBERSHIP_QUERY            IGMP_V1_QUERY
#define IGMP_V2_MEMBERSHIP_REPORT           0x16
#define IGMP_V2_LEAVE_GROUP                 0x17
#define IGMP_V3_MEMBERSHIP_QUERY            0x22

#define IGMP_PROCESS_REMOVED                1
#define IGMP_GROUP_FREED                    2

#define ERROR_IGMP_HOSTS_EXHAUSED          -1
#define ERROR_IGMP_HOST_PROCESSES_EXHAUSED -2
#define ERROR_IGMP_ILLEGAL_HOST_ID         -3
#define ERROR_IGMP_NON_EXISTANT_HOST_ID    -4
#define ERROR_IGMP_NETWORK_NOT_AVAILABLE   -5
#define ERROR_IGMP_INVALID_MULT_ADDRESS    -6


/************************** TCP Defines *****************************************************************/

                                                                         // TCP states. The values have been carefully chosen to allow optimum program code for checking
                                                                         //             single and multiple states. DON'T CHANGE
#define TCP_STATE_FREE                 0x00                              // potential TCP socket space is free
#define TCP_STATE_RESERVED             0x01                              // TCP socket has been obtained
#define TCP_STATE_CLOSED               0x02                              // TCP socket has been bound and is in closed state
#define TCP_STATE_LISTEN               0x03                              // server has been set to the listening state and is waiting for a SYN reception (incoming connections)
#define TCP_STATE_SYN_RCVD             0x08                              // SYN has been received (either to the SYN we sent or from an incomming connection)
#define TCP_STATE_SYN_SENT             0x10                              // SYN packet sent as an attempt to establish a connection
#define TCP_STATE_FIN_WAIT_1           0x20                              // application wants to close, we have sent FIN
#define TCP_STATE_FIN_WAIT_2           0x40                              // ACK received to our FIN, wait for FIN from partner
#define TCP_STATE_CLOSING              0x80                              // doing simultaneous close; FIN received and ACK sent, waiting for final ACK to our FIN
#define TCP_STATE_CLOSE_WAIT           0x90                              // passive close. We have received a FIN and sent an ACK to it. We are waiting for our application to close.
#define TCP_STATE_LAST_ACK             0xa0                              // passive close. Our application has terminated and we have sent FIN. Waiting for final ACK to our FIN.
#define TCP_STATE_TIME_WAIT            0x04                              // active close terminated. Wait 2MSL time before allowing new connection.
#define TCP_STATE_ESTABLISHED          0xb0                              // connection established - in data transfer state
//#define TCP_STATE_BAD_SOCKET         0xff                              // this state doesn't exist but this is returned by fnGetTCP_state if the socket is invalid {14}{94}

#define TCP_FLAG_FIN                   0x01                              // protocol flag bits
#define TCP_FLAG_SYN                   0x02
#define TCP_FLAG_RESET                 0x04
#define TCP_FLAG_PUSH                  0x08
#define TCP_FLAG_ACK                   0x10
#define TCP_FLAG_FIN_RECEIVED          0x80                              // {102}

#define MIN_TCP_HLEN                   20                                // TCP header length without options
#define MAX_TCP_OPTLEN                 40                                // maximum TCP options length

#define MAX_SECURE_SOCKET_HEADER       21                                // record header plus maximum initial vector length
#define MAX_SECURE_SOCKET_TAIL         36                                // MAC plus maximum padding length

#define TCP_CLOSEPENDING               0x01                              // internal flag
#define SILLY_WINDOW_AVOIDANCE         0x02                              // we are stalling the transmitter to avoid silly (small) window
#define WINDOW_UPDATE                  0x04                              // {32} windows update in progress, avaiting ARP resolution
#define TCP_OVER_IPV6                  0x08                              // {53} TCP using IPv6


#if defined USE_BUFFERED_TCP                                             // optional TCP connection buffer
typedef struct stTCP_WINDOWS_LIST                                        // {8} don't force packed
{
    unsigned short usFrameSize;                                          // the size of the frame in the buffer
    #if defined USE_TELNET
    unsigned short usEscapeLength;                                       // the number of escaped characters in the output buffer
    #endif
    unsigned short usWindowTimeout;                                      // monitor ack timeouts for each window
    unsigned char  ucNegotiate;                                          // details about negotiation content {3}
} TCP_WINDOWS_LIST;

typedef struct stTCP_TX_BUFFER                                           // {8} don't force packed
{   
    unsigned char  *ucDataStart;
    unsigned char  *ucDataEnd;     
    unsigned short usPacketSize;
    unsigned short usWaitingSize;
#if defined SUPPORT_PEER_WINDOW
    unsigned short usOpenBytes;                                          // total bytes not yet acked
    unsigned char  ucCwnd;                                               // congestion window count
    unsigned char  ucOpenAcks;                                           // total frames not yet acked
    unsigned char  ucPutFrame;                                           // tx_window for next use
    unsigned char  ucGetFrame;                                           // first outstanding tx_window
    TCP_WINDOWS_LIST tx_window[WINDOWING_BUFFERS];                       // window management
#endif
#if defined WAKE_BLOCKED_TCP_BUF
    UTASK_TASK     WakeTask;
#endif
#if defined INDIVIDUAL_BUFFERED_TCP_BUFFER_SIZE                          // {40}
    unsigned char  *ucTCP_tx;                                            // pointer to the location of the buffered TCP buffer
#else
    unsigned char  ucTCP_tx[TCP_BUFFER];                                 // buffered TCP buffer as part of the TCP_TX_BUFFER struct
#endif
} TCP_TX_BUFFER;
#endif

__PACK_ON
typedef struct _PACK stTCP_HEADER                                        // header define for transmitter use {1}
{
    unsigned char  ucLocalPort[2];                                       // elements are defines in bytes to ensure correct length
    unsigned char  ucRemotePort[2];
    unsigned char  ucUnacked[4];
    unsigned char  ucRxNext[4];
    unsigned char  ucLen;
    unsigned char  ucFlags;
    unsigned char  ucDefMTU[2];
    unsigned char  ucCheckSum[2];
    unsigned char  ucUrgent[2];
} TCP_HEADER;
__PACK_OFF

typedef struct stTCP_CONTROL                                             // {8} don't force packing
{
    unsigned long  ulNextTransmissionNumber;                             // sequence number to be sent next
    unsigned long  ulNextReceptionNumber;                                // sequence number which we expect to receive
    unsigned long  ulSendUnackedNumber;                                  // ack number to be sent
    int (*event_listener)(USOCKET, unsigned char, unsigned char *, unsigned short);
#if defined USE_BUFFERED_TCP
    TCP_TX_BUFFER  *ptrTCP_Tx_Buffer;                                    // optional transmit buffer pointer
#endif
    unsigned short usRemport;                                            // remote TCP port
    unsigned short usLocport;                                            // local TCP port
    unsigned short usIdleTimeout;                                        // timeout in seconds until the TCP connection timeout without activity
    unsigned short usLinkTimer;                                          // counter for the idle timer
    unsigned short usTransmitTimer;                                      // counter for the retransmit
#if defined CONTROL_WINDOW_SIZE
    unsigned short usRxWindowSize;                                       // space in our receiver
    #if defined HTTP_WINDOWING_BUFFERS || defined  FTP_DATA_WINDOWS      // {4}{22}
        unsigned char  ucPersistentTimer;                                // persistent timer count down
        unsigned char  ucProbeCount;                                     // persistent probe exponential back-off index
    #endif
#endif
#if defined SUPPORT_PEER_WINDOW
    unsigned short usTxWindowSize;                                       // space in peer's receiver
#endif
#if defined SUPPORT_PEER_MSS
    unsigned short usPeerMSS;                                            // the maximum segment size that our peer can accept
#endif
#if defined HTTP_WINDOWING_BUFFERS || defined  FTP_DATA_WINDOWS          // {22}
    unsigned short usOpenCnt;                                            // {4}
#endif
#if defined USE_BUFFERED_TCP && defined INDIVIDUAL_BUFFERED_TCP_BUFFER_SIZE // {40}
    unsigned short usTCP_buffer_length;                                  // length of the buffered TCP buffer associated with this socket
#endif
    USOCKET        MySocketNumber;                                       // save socket number also in structure for convenience
    unsigned char  ucTCP_state;                                          // TCP socket state
    unsigned char  ucSendFlags;                                          // flags to be sent in the control message
#if defined USE_IPV6
    unsigned char  ucRemoteIP[IPV6_LENGTH];                              // our peers IPv4/IPv6 address
#else
    unsigned char  ucRemoteIP[IPV4_LENGTH];                              // our peers IPv4 address
#endif
    unsigned char  ucTos;                                                // type of service
    unsigned char  ucRetransmissions;                                    // number of retransmissions left before aborting
    unsigned char  ucTCPInternalFlags;                                   // internal flags for software control
#if defined T_TCP_PERIOD                                                 // user is defining a higher TCP polling resolution
    unsigned char  ucLinkDivider;                                        // pre-scaler used to convert link seconds to user's resolution
#endif
} TCP_CONTROL;

__PACK_ON
typedef struct _PACK stTCP_PACKET                                        // {1}
{
  unsigned short usSourcePort;                                           // the source port which sent the TCP packet
  unsigned short usDestPort;                                             // the port it is sending the TCP packet to
  unsigned long  ulSeqNr;                                                // sequence number of data
  unsigned long  ulAckNo;                                                // acknowledge number of data
  unsigned short usHeaderLengthAndFlags;                                 // length of data and optional flags
  unsigned short usWindowSize;                                           // advertised window size
  unsigned short usChecksum;                                             // TCP frame check sum
  unsigned short usUrgentPointer;                                        // urgent data pointer (when URG flag is set)
//unsigned char  ucOptData[MAX_TCP_OPTLEN];                              // possible TCP options (since we do not use them we do not save them)
} TCP_PACKET;
__PACK_OFF

#define TCP_PORTS_START         49152                                    // start in dynamic port range
#define TCP_PORTS_END           65535                                    // end in dynamic port range

#if defined USE_IPV6                                                     // {92}
    #if defined USE_IPV6INV4
        #define TCP_DEF_MTU      (_LAN_BUFFER_SIZE - ETH_HEADER_LEN - IP_MIN_HLEN - MIN_TCP_HLEN) // default TCP window size for tunnelled IPv6
    #else
        #define TCP_DEF_MTU      (_LAN_BUFFER_SIZE - ETH_HEADER_LEN - IP_MIN_HLEN - MIN_TCP_HLEN) // default TCP window size for IPv6
    #endif
#else
    #define TCP_DEF_MTU          (_LAN_BUFFER_SIZE - ETH_HEADER_LEN - IP_MIN_HLEN - MIN_TCP_HLEN) // default TCP window size for IPv4
#endif

// Timeouts (in seconds)
//
#if defined T_TCP_PERIOD                                                 // if there is no user defined TCP polling resolution seconds are used with the following timeouts
    #define HIGH_RESOLUTION_TCP_POLLING
    #define TCP_DEFAULT_TIMEOUT     120                                  // connection timeout with no activity (2 minutes)

    #define _TCP_POLLING_SECONDS   ((DELAY_LIMIT)(1 * SEC)/T_TCP_PERIOD) // conversion factor between user defined resolution and seconds

    #define TCP_SYN_RETRY_TOUT      ((USER_TCP_SYN_RETRY_MS * _TCP_POLLING_SECONDS)/1000) // SYN retry timeout
    #define TCP_STANDARD_RETRY_TOUT ((USER_TCP_STANDARD_RETRY_MS * _TCP_POLLING_SECONDS)/1000) // normal TCP packet retry timeout
    #define TCP_SILLY_WINDOW_DELAY  ((USER_TCP_SILLY_WINDOWS_DELAY_MS * _TCP_POLLING_SECONDS)/1000) // wait for this time when avoiding silly windows
    #define TCP_FIN_WAIT_2_TIMEOUT  (USER_TCP_FIN_WAIT_2_SECONDS * _TCP_POLLING_SECONDS) // we don't allow blocking in this state so set  a reasonable timeout
    #define TCP_MSL_TIMEOUT         (USER_TCP_MSL_SECONDS * _TCP_POLLING_SECONDS) // 2MSL is twice this timeout - normally this should be about 1min
                                                                         // but it is a problem if we have a limited amount of sockets and will cause
                                                                         // the web server (for example) to not be able to respond until the timeout passes.
                                                                         // since the chance of problems with shorter times is very small (in LANs especially)
                                                                         // we prefer fast reactivation and a low count of sockets
#else
    #define TCP_DEF_RETRIES         7                                    // packets will be retransmitted this amount of times
    #define TCP_CON_ATTEMPTS        7                                    // connection attempts will stop after this amount of tries

    #define TCP_SYN_RETRY_TOUT      2                                    // SYN retry timeout
    #define TCP_STANDARD_RETRY_TOUT 3                                    // normal TCP packet retry timeout (3 seconds)
    #define TCP_SILLY_WINDOW_DELAY  5                                    // wait for this time when avoiding silly windows
    #define TCP_FIN_WAIT_2_TIMEOUT  60                                   // we don't allow blocking in this state so set a reasonable timeout
    #define TCP_DEFAULT_TIMEOUT     120                                  // default connection timeout with no activity (2 minutes)
    #define TCP_MSL_TIMEOUT         2                                    // 2MSL is twice this timeout - normally this should be about 1min
                                                                         // but it is a problem if we have a limited amount of sockets and will cause
                                                                         // the web server (for example) to not be able to respond until the timeout passes.
                                                                         // since the chance of problems with shorter times is very small (in LANs especially)
                                                                         // we prefer fast reactivation and a low count of sockets
#endif

#define TCP_EVENT_ABORT                     1                            // TCP EVENTS
#define TCP_EVENT_CONNECTED                 2
#define TCP_EVENT_ACK                       3
#define TCP_EVENT_DATA                      4
#define TCP_EVENT_CLOSE                     5
#define TCP_EVENT_CLOSED                    6
#define TCP_EVENT_REGENERATE                7
#define TCP_EVENT_CONREQ                    8
#define TCP_EVENT_ARP_RESOLUTION_FAILED     9
#define TCP_EVENT_PARTIAL_ACK               10
#define TCP_WINDOW_UPDATE                   11                           // {4}
#define TCP_WINDOW_PROBE                    12                           // {4}

#define APP_ACCEPT                          0x0000                       // TCP listener return values
#define APP_SENT_DATA                       0x0001
#define APP_REJECT                          0x0002
#define APP_WAIT                            0x0004
#define APP_REQUEST_CLOSE                   0x0008
#define APP_REJECT_DATA                     0x0010
#define HANDLING_PARTICAL_ACK               0x0020
#define APP_REQUEST_AUTHENTICATION          0x0040                       // {21}
#define APP_SECURITY_HANDLED                0x0080
#define APP_FREE_DATA                       0x0100
#define APP_SECURITY_CONNECTED              0x0200


#define NO_TCP_LISTENER_INSTALLED          -1                            // TCP error codes
#define NO_TCP_SOCKET_FREE                 -2
#define SOCKET_NOT_FOUND                   -3                            // {30}
#define SOCKET_STATE_INVALID               -4                            // {30}
#define NO_FREE_PORTS_AVAILABLE            -5                            // {30}
#define INVALID_INTERFACE                  -6                            // {91}

/************************** TELNET Defines ***************************************************************/

#define TELNET_IAC            255
#define TELNET_DONT           254
#define TELNET_DO             253
#define TELNET_WONT           252
#define TELNET_WILL           251
#define TELNET_SB             250                                        // start sub negotiation
#define TELNET_GO_AHEAD       249
#define TELNET_ERASE_LINE     248
#define TELNET_ERASE_CHAR     247
#define TELNET_AYT            246                                        // Are You There?
#define TELNET_ABORT_OUTPUT   245
#define TELNET_INTERRUPT_PROC 244
#define TELNET_BREAK          243
#define TELNET_DATA_MARK      242                                        // data stream portion of sync - always TCP urgent notification
#define TELNET_NOP            241
#define TELNET_SE             240                                        // end sub negotiation
#define TELNET_EOR            239                                        // end of record
#define TELNET_ABORT          238
#define TELNET_SUSP           237                                        // suspend current process
#define TELNET_EOF            236                                        // end of file


#define TELNET_BINARY                   0
#define TELNET_ECHO                     1
#define TELNET_RECONNECTION             2
#define TELNET_SUPPRESS_GO_AHEAD        3
#define TELNET_APPROX_MSG_SIZE          4
#define TELNET_STATUS                   5
#define TELNET_TIMING_MARK              6
#define TELNET_REM_TRANS_ECHO           7
#define TELNET_OUTPUT_LINE_WIDTH        8
#define TELNET_OUTPUT_PAGE_SIZE         9
#define TELNET_OUTPUT_CR_DISP          10
#define TELNET_OUTPUT_TAB_STOPS        11
#define TELNET_OUTPUT_HOR_TAB_DISP     12
#define TELNET_OUTPUT_FORMFEED_DISP    13
#define TELNET_OUTPUT_VER_TAB_STOPS    14
#define TELNET_OUTPUT_VER_TAB_DISP     15
#define TELNET_OUTPUT_LF_DISP          16
#define TELNET_EXTENDED_ASCII          17
#define TELNET_LOGOUT                  18
#define TELNET_BYTE_MACRO              19
#define TELNET_DATA_ENTRY_TERMINAL     20
#define TELNET_SUPDUP                  21
#define TELNET_SUPDUP_OUTPUT           22
#define TELNET_SEND_LOCATION           23
#define TELNET_TERMINAL_TYPE           24
#define TELNET_END_OF_RECORD           25
#define TELNET_TACACS_USER_ID          26
#define TELNET_OUTPUT_MARKING          27
#define TELNET_TERMINAL_LOC_NUMBER     28
#define TELNET_3270_REGIME             29
#define TELNET_X_3_PAD                 30
#define TELNET_NEG_WINDOW_SIZE         31
#define TELNET_TERMINAL_SPEED          32
#define TELNET_REMOTE_FLOW_CONTROL     33
#define TELNET_LINEMODE                34
#define TELNET_X_DISPLAY_LOCATION      35
#define TELNET_ENVIRONMENT_OPTION      36
#define TELNET_AUTHENTICATION_OPTION   37
#define TELNET_ENCRYPTION_OPTION       38
#define TELNET_NEW_ENVIRONMENT_OPTION  39
#define TELNET_TN3270E                 40

#define TELNET_RFC2217_COM_PORT_OPTION 44                                // {103}




// Telnet commands
//
#define GOTO_ECHO_MODE                 0
#define LEAVE_ECHO_MODE                1
#define PASSWORD_ENTRY                 2
#define CLEAR_TEXT_ENTRY               3
#define TELNET_ASCII_MODE              4
#define TELNET_RAW_MODE                5
#define TELNET_RAW_RX_IAC_ON           6
#define TELNET_RAW_RX_IAC_OFF          7
#define TELNET_RAW_TX_IAC_ON           8
#define TELNET_RAW_TX_IAC_OFF          9
#define TELNET_RFC2217_ON              10                                // {103}
#define TELNET_RFC2217_OFF             11
#define TELNET_RESET_MODE              12

// RFC 2217 commands
//
#define SET_SIGNATURE_SERVER           0
#define SET_SIGNATURE_CLIENT           100

#define SET_BAUDRATE_SERVER            1
#define SET_BAUDRATE_CLIENT            101

#define SET_DATASIZE_SERVER            2
#define SET_DATASIZE_CLIENT            102

#define SET_PARITY_SERVER              3
#define SET_PARITY_CLIENT              103

#define SET_STOPSIZE_SERVER            4
#define SET_STOPSIZE_CLIENT            104

#define SET_CONTROL_SERVER             5
#define SET_CONTROL_CLIENT             105

#define NOTIFY_LINESTATE_SERVER        6
#define NOTIFY_LINESTATE_CLIENT        106

#define NOTIFY_MODEMSTATE_SERVER       7
#define NOTIFY_MODEMSTATE_CLIENT       107

#define FLOWCONTROL_SUSPEND_SERVER     8
#define FLOWCONTROL_SUSPEND_CLIENT     108

#define FLOWCONTROL_RESUME_SERVER      9
#define FLOWCONTROL_RESUME_CLIENT      109

#define SET_LINESTATE_MASK_SERVER      10
#define SET_LINESTATE_MASK_CLIENT      110

#define SET_MODEMSTATE_MASK_SERVER     11
#define SET_MODEMSTATE_MASK_CLIENT     111

#define PURGE_DATA_SERVER              12
#define PURGE_DATA_CLIENT              112

// Telnet modes
//
#define TELNET_DO_ECHO                 0x0001
#define TELNET_CLIENT_MODE             0x0002                            // {91}
#define TELNET_RAW_SOCKET              0x0004
#define TELNET_COMMAND_MODE            0x0008
#define TELNET_WILL_ECHO               0x0010
#define TELNET_CLIENT_MODE_ECHO        (TELNET_WILL_ECHO)
#define TELNET_PASSWORD_ENTRY_MODE     0x0020
#define TELNET_WONT_ECHO               0x0040
#define TELNET_BINARY_MODE             0x0080
#define TELNET_BIN_RX_IAC              0x0100
#define TELNET_BIN_TX_IAC              0x0200
#define TELNET_STUFF_TX_IAC            0x0400
#define TELNET_SEARCH_RX_IAC           0x0800
#define TELNET_CLIENT_NO_NEGOTIATION   0x1000
#define TELNET_RFC2217_ACTIVE          0x2000                            // {103}

typedef struct stTELNET_CLIENT_DETAILS                                   // {91}
{
    int (*listener)(USOCKET, unsigned char, unsigned char *, unsigned short); // application call-back handler (may not be 0)
    unsigned short usPortNumber;
    unsigned short usMaxWindow;
    unsigned short usTelnetMode;
    unsigned short usIdleTimeout;
    unsigned char  ucDestinationIPv4[IPV4_LENGTH];                       // destination address for the connection
    unsigned char  ucInterfaceReference;
    USOCKET Socket;
    UTASK_TASK wakeOnAck;
} TELNET_CLIENT_DETAILS;


#define RFC2217_PARITY_NONE                      1
#define RFC2217_PARITY_ODD                       2
#define RFC2217_PARITY_EVEN                      3
#define RFC2217_PARITY_MARK                      4
#define RFC2217_PARITY_SPACE                     5

#define RFC2217_STOPS_ONE                        1
#define RFC2217_STOPS_TWO                        2
#define RFC2217_STOPS_1_5                        3

#define RFC2217_REQUEST_FLOW_CONTROL             0
#define RFC2217_NO_FLOW_CONTROL                  1
#define RFC2217_XON_XOFF_FLOW_CONTROL            2
#define RFC2217_HARDWARE_FLOW_CONTROL            3
#define RFC2217_REQUEST_BREAK_STATE              4
#define RFC2217_SET_BREAK_STATE_ON               5
#define RFC2217_SET_BREAK_STATE_OFF              6
#define RFC2217_REQUEST_DTR_STATE                7
#define RFC2217_SET_DTR_STATE_ON                 8
#define RFC2217_SET_DTR_STATE_OFF                9
#define RFC2217_REQUEST_RTS_STATE                10
#define RFC2217_SET_RTS_STATE_ON                 11
#define RFC2217_SET_RTS_STATE_OFF                12
#define RFC2217_REQUEST_FLOW_CONTROL_INBOUND     13
#define RFC2217_NO_FLOW_CONTROL_INBOUND          14
#define RFC2217_XON_XOFF_FLOW_CONTROL_INBOUND    15
#define RFC2217_HARDWARE_FLOW_CONTROL_INBOUND    16
#define RFC2217_USE_DCD_FLOW_CONTROL             17
#define RFC2217_USE_DTR_FLOW_CONTROL             18
#define RFC2217_USE_DSR_FLOW_CONTROL             19

#define RFC2217_LINE_STATE_MASK_DATA_READY                         0x01
#define RFC2217_LINE_STATE_MASK_OVERRUN_ERROR                      0x02
#define RFC2217_LINE_STATE_MASK_PARITY_ERROR                       0x04
#define RFC2217_LINE_STATE_MASK_FRAMING_ERROR                      0x08
#define RFC2217_LINE_STATE_MASK_BREAK_DETECT_ERROR                 0x10
#define RFC2217_LINE_STATE_MASK_TRANSFER_HOLDING_REGISTER_EMPTY    0x20
#define RFC2217_LINE_STATE_MASK_TRANSFER_SHIFT_REGISTER_EMPTY      0x40
#define RFC2217_LINE_STATE_MASK_TIMEOUT_ERROR                      0x80

#define RFC2217_LINE_STATE_MASK_DELTA_CLEAR_TO_SEND                0x01
#define RFC2217_LINE_STATE_MASK_DELTA_DATA_SET_READY               0x02
#define RFC2217_LINE_STATE_MASK_TRAINING_EDGE_RING_DETECTOR        0x04
#define RFC2217_LINE_STATE_MASK_DELTA_RECEIVE_LINE_SIGNAL_DETECT   0x08
#define RFC2217_LINE_STATE_MASK_CLEAR_TO_SEND_SIGNAL_STATE         0x10
#define RFC2217_LINE_STATE_MASK_DATA_SET_READY_SIGAL_STATE         0x20
#define RFC2217_LINE_STATE_MASK_RING_INDICATOR                     0x40
#define RFC2217_LINE_STATE_MASK_CARRIER_DETECT                     0x80

#define RFC2217_PURGE_ACCESS_SERVER_RECEIVE_DATA_BUFFER    1
#define RFC2217_PURGE_ACCESS_SERVER_TRANSMIT_DATA_BUFFER   2
#define RFC2217_PURGE_ACCESS_SERVER_TX_RX_DATA_BUFFERS     3

typedef struct stRFC2217_UART_SETTINGS                                   // {103}
{
    unsigned long ulBaudRate;
    QUEUE_HANDLE  uartID;
    unsigned char ucDataSize;                                            // 5, 6, 7 or 8
    unsigned char ucParity;
    unsigned char ucStopBits;
    unsigned char ucFlowControl;
} RFC2217_UART_SETTINGS;

#define RFC2217_CONNECTION_OPENED    1
#define RFC2217_SETTINGS_CHANGED     2
#define RFC2217_CONNECTION_CLOSED    3

typedef struct stRFC2217_INSTANCE                                        // {103}
{
    int     iInstanceState;
    USOCKET Telnet_RFC2217_socket;
    int    (*RFC2217_callback)(int, RFC2217_UART_SETTINGS *);
    unsigned char ucLineStateMask;
    unsigned char ucModemStateMast;
    RFC2217_UART_SETTINGS uart_settings;
    RFC2217_UART_SETTINGS uart_settings_original;
} RFC2217_INSTANCE;

typedef struct stRFC2217_SESSION_CONFIG                                   // {103}
{
    int(*RFC2217_userCallback)(int, RFC2217_UART_SETTINGS *);
    unsigned short usPortNumber;
    unsigned short usIdleTimeout;
    RFC2217_UART_SETTINGS uart_settings;
} RFC2217_SESSION_CONFIG;

typedef struct stTELNET                                                  // {8} don't force packed
{
    int (*fnApp)(USOCKET, unsigned char, unsigned char *, unsigned short);
#if defined USE_TELNET_CLIENT                                            // {91}
    TELNET_CLIENT_DETAILS *ptrTelnetClientDetails;                       // pointer to client characteristics
#endif
#if defined TELNET_RFC2217_SUPPORT                                       // {103}
    RFC2217_INSTANCE *ptrRFC2217_Instance;                               // pointer to an RFC2217 session instance
#endif
    unsigned short usTelnetMode;
    unsigned short usTelnetPortNumber;
    unsigned short usMaxWindow;
    USOCKET        Telnet_socket;
    unsigned char  ucState;
    UTASK_TASK     wakeOnAck;
} TELNET;


/************************** HTTP Defines *****************************************************************/

typedef struct stHTTP                                                    // {8} don't force packed
{
#if defined SUPPORT_HTTP_POST
    MEMORY_RANGE_POINTER ptrFile;
#endif
    MEMORY_RANGE_POINTER ptrFileStart;
#if defined HTTP_FILE_REFERENCE                                          // {83}
    CHAR FileNameReference[HTTP_FILE_REFERENCE_LENGTH];                  // {88}
#endif
#if defined HTTP_DYNAMIC_CONTENT && defined HTTP_USER_DETAILS            // {10}
    void *ptrUserData;                                                   // pointer to memory containing connection details needed by application
#endif
#if defined HTTP_DYNAMIC_CONTENT || defined HTTP_WINDOWING_BUFFERS       // {35}
    #if defined HTTP_WINDOWING_BUFFERS
    LENGTH_CHUNK_COUNT DynamicCnt[1 + HTTP_WINDOWING_BUFFERS];           // reference during dynamic content generation {10} array {12}
    #else
    LENGTH_CHUNK_COUNT DynamicCnt[2];                                    // {12}
    #endif
#endif
    MAX_FILE_LENGTH FileLength;
    MAX_FILE_LENGTH FilePoint;
    unsigned short usUnacked;
#if defined SUPPORT_PEER_MSS
    unsigned short usPeerMSS;                                            // {80} the maximum segment size that our peer can accept
#endif
    USOCKET        OwnerTCPSocket;
    unsigned char  ucState;
#if defined SUPPORT_MIME_IDENTIFIER
    unsigned char  ucMimeType;                                           // file type
#endif
#if defined HTTP_AUTHENTICATION
    #define USER_PASS_LENGTH                 24                          // base 64 coded user and pass of each 8 bytes need this amount of coded space. ((name length + 1 + password length) * 8)/6) rounded up to nearest 2
    CHAR           cUserPass[USER_PASS_LENGTH];                          // we must save parts of authentication frames
    unsigned char  ucAuth;                                               // authorisation counter
#endif
#if defined HTTP_WINDOWING_BUFFERS                                       // {4} raw data size
    unsigned short usUnackedPrevious;                                    // previous unacked window (we only support 2 at present)
    unsigned short usUnackedPreviousContent;                             // {5} Content data size (can be different when dynmically filled)
    unsigned char  ucOpenWindows;                                        // the number of open acks when sending using windowing
#endif
#if defined HTTP_DYNAMIC_CONTENT
    #if defined HTTP_WINDOWING_BUFFERS
    unsigned char  ucPresentFrame;                                       // counter of frame in a windowing sequence
    #endif
    unsigned char  ucDynamicFlags;                                       // special flags helpful when generating dynamic data {23 - corrected from ucDymamic}
#endif
#if defined SUPPORT_HTTP_POST
    unsigned char  ucBoundaryLength;                                     // length of boundary identifier used in post
    CHAR           cDisplayFile;                                         // file to be returned after posting
    #if defined SUB_FILE_SIZE
    unsigned char  ucSubFileWrite;                                       // write to sub-file is in progress
    #endif
#endif
#if defined HTTP_UTFAT
    UTFILE         utFile;
#endif
#if defined _WINDOWS                                                     // {19}
    int iFetchingInternalMemory;
#endif
} HTTP;

#if defined HTTP_DYNAMIC_CONTENT
  #define QUIT_FRAME_DURING_GENERATION  0x01                             // flag used to quit frame filling during dynamic content generation
  #define NO_DYNAMIC_CONTENT_TO_ADD     0x02                             // flag used to ignore generation - can be used by application
  #define LAST_DYNAMIC_CONTENT_DATA     0x04                             // {24} flag that this is the last chunk of data to send - it avoids padding being added, which may corrupt binary content
  #define GENERATING_DYNAMIC_BINARY     0x08                             // {24} flag that the user is generating binary data rather than HTML content. This is used to avoid probing with HTML character
  #define DELAY_DYNAMIC_SERVING         0x10                             // {26} flag that the user requires the HTTP connection to pause before sending more content
  #define MAXIMUM_DYNAMIC_INSERTS       0x20                             // {36} flag that no further dynamic inserts should be started in the present TCP frame
#endif

#if !defined NO_OF_HTTPS_SESSIONS                                        // if no define is available for the number of secure HTTP sockets
    #define NO_OF_HTTPS_SESSIONS 0                                       // set none in order to disable HTTPS
#endif

#define HTTP_STATE_FREE                  0                               // HTTP states
#define HTTP_STATE_RESERVED              1
#define HTTP_STATE_ACTIVE                2
#define HTTP_STATE_REQUEST_CREDS         3
#define HTTP_STATE_PROCESSING            4
#define HTTP_STATE_START_POST            5
#define HTTP_STATE_POSTING               6
#define HTTP_STATE_BEGIN_POSTING         7
#define HTTP_STATE_READY_POSTING         8
#define HTTP_STATE_POSTING_DATA          9
#define HTTP_STATE_DUMPING_DATA          10
#define HTTP_STATE_POSTING_TO_APP        11
#define HTTP_STATE_DELAYED_SERVING       12
#define HTTP_STATE_POSTING_PLAIN         13
#define HTTP_STATE_DOING_PARAMETER_POST  14                              // {20}
#define HTTP_STATE_WEB_SOCKET_CONNECTION 15

#define CREDENTIALS_REQUIRED           -1                                // user return codes
#define DISPLAY_INTERNAL               -2
#define PROCESSING_INPUT               -3
#define DELAY_SERVING                  -4
#define WEB_SOCKET_HANDSHAKE           -5

#define GET_STARTING                  (unsigned char)0                   // {90}
#define CAN_POST_BEGIN                (unsigned char)-1
#define INFORM_POST_SUCCESS           (unsigned char)-2
#define INFORM_POST_FAILED            (unsigned char)-3
#define POSTING_DATA_TO_APP           (unsigned char)-4
#define POSTING_PARAMETER_DATA_TO_APP (unsigned char)-5                  // {18}
#define POSTING_PARTIAL_PARAMETER_DATA_TO_APP (unsigned char)-6          // {25}
#define CHECK_AUTHENTICATION_ON_PAGE  (unsigned char)-7                  // {90}

typedef struct stWEB_SOCKET_FRAME_MASKED
{
    unsigned char ucFinOpcode;
    unsigned char ucPayloadLength;
    unsigned char ucMaskingKey[4];
    unsigned char ucPayload[1];                                          // variable length payload
} WEB_SOCKET_FRAME_MASKED;

typedef struct stWEB_SOCKET_FRAME_UNMASKED
{
    unsigned char ucFinOpcode;
    unsigned char ucPayloadLength;
    unsigned char ucPayload[1];                                          // variable length payload
} WEB_SOCKET_FRAME_UNMASKED;

typedef struct stWEB_SOCKET_FRAME_EXT_16_MASKED                          // if the payload length is 126
{
    unsigned char ucFinOpcode;
    unsigned char ucPayloadLength[3];
    unsigned char ucMaskingKey[4];
    unsigned char ucPayload[1];                                          // variable length payload
} WEB_SOCKET_FRAME_16_MASKED;

typedef struct stWEB_SOCKET_FRAME_EXT_16_UNMASKED                        // if the payload length is 126
{
    unsigned char ucFinOpcode;
    unsigned char ucPayloadLength[3];
    unsigned char ucPayload[1];                                          // variable length payload
} WEB_SOCKET_FRAME_16_UNMASKED;

typedef struct stWEB_SOCKET_FRAME_EXT_64_MASKED                          // if the payload length is 127
{
    unsigned char ucFinOpcode;
    unsigned char ucPayloadLength[9];
    unsigned char ucMaskingKey[4];
    unsigned char ucPayload[1];                                          // variable length payload
} WEB_SOCKET_FRAME_64_MASKED;

typedef struct stWEB_SOCKET_FRAME_EXT_64_UNMASKED                        // if the payload length is 127
{
    unsigned char ucFinOpcode;
    unsigned char ucPayloadLength[9];
    unsigned char ucMaskingKey[4];
    unsigned char ucPayload[1];                                          // variable length payload
} WEB_SOCKET_FRAME_64_UNMASKED;

#define WEB_SOCKET_FIN                     0x80                          // indicates that the payload contains the final fragment of a message
#define WEB_SOCKET_OPCODE_MASK             0x0f
#define WEBSOCKET_CONTROL_FRAME            0x08
#define WEB_SOCKET_OPCODE_CONTINUATION     0x00
#define WEB_SOCKET_OPCODE_TEXT             0x01
#define WEB_SOCKET_OPCODE_BINARY           0x02
#define WEB_SOCKET_OPCODE_CONNECTION_CLOSE 0x08
#define WEB_SOCKET_OPCODE_PING             0x09
#define WEB_SOCKET_OPCODE_PONG             0x0a

#define WEB_SOCKET_MASK                    0x80                          // payload is masked
#define WEB_SOCKET_PAYLOAD_LEN_MASK        0x7f

/************************** SMTP ************************************************************************************/

#define ERROR_SMTP_NOT_READY      -1
#define ERROR_SMTP_IN_USE         -2

// Call back events
//
#define SMTP_GET_DOMAIN             1
#define SMTP_GET_SENDER             2
#define SMTP_GET_DESTINATION        3
#define SMTP_GET_SUBJECT            4
#define SMTP_SEND_MESSAGE           5
#define SMTP_USER_NAME              6
#define SMTP_USER_PASS              7

#define SMTP_MAIL_SUCCESSFULLY_SENT 8

#define ERROR_SMTP_ARP_FAIL         10
#define ERROR_SMTP_TIMEOUT          11
#define ERROR_SMTP_HOST_CLOSED      12
#define ERROR_SMTP_LOGIN_FAILED     13
#define ERROR_SMTP_LOGIN_NOT_SUPPORTED 14
#define ERROR_SMTP_POLICY_REJECT    15

/************************** POP3 ************************************************************************************/

#define ERROR_POP3_NOT_READY       -1
#define ERROR_POP3_IN_USE          -2

// Call back events
//
#define POP3_USER_NAME              1
#define POP3_USER_PASS              2
#define POP3_WAITING_MESSAGES       3
#define POP3_RX_MESSAGE             4
#define POP3_RX_MESSAGE_END         5
#define POP3_SEARCH_SUBJECT         6
#define POP3_RX_SUBJECT             7
#define POP3_GOOD_SUBJECT           8
#define POP3_RX_TINY_MESSAGE        9
#define POP3_RX_MESSAGE_START       10
#define POP3_DELETE_MESSAGE         11
#define POP3_CONNECTION_CLOSED      12

#define ERROR_POP3_ARP_FAIL         30
#define ERROR_POP3_TIMEOUT          31
#define ERROR_POP3_DIDNT_UNDERSTAND 32
#define ERROR_POP3_HOST_CLOSED      33

/************************** DNS *************************************************************************************/

// Call back events
//
#define DNS_EVENT_SUCCESS           1

#define DNS_ERROR_NO_ARP_RES        10
#define DNS_ERROR_TIMEOUT           11
#define DNS_ERROR_GENERAL           12
#define DNS_OPCODE_ERROR            13

/************************** TFTP ************************************************************************************/

// Codes
//
#define TFTP_OPCODE_RRQ       1
#define TFTP_OPCODE_WRQ       2
#define TFTP_OPCODE_DATA      3
#define TFTP_OPCODE_ACK       4
#define TFTP_OPCODE_ERROR     5

// TFTP error codes
//
#define TFTP_NON_DEFINED_SEE_ERROR_MSG     0 
#define TFTP_FILE_NOT_FOUND                1
#define TFTP_ACCESS_VIOLATION              2
#define TFTP_DISK_FULL                     3
#define TFTP_ILLEGAL_OPERATION             4
#define TFTP_UNKNOWN_TRANSFER_ID           5
#define TFTP_FILE_ALREADY_EXISTS           6
#define TFTP_NO_SUCH_USER                  7
#define TFTP_FILE_NOT_EQUAL                8

// Callback codes
//
#define TFTP_CLIENT_ERROR             0x8000                             // flag to inform client side error
#define TFTP_TRANSFER_DID_NOT_START   (TFTP_CLIENT_ERROR | 0x100)        // call back messages
#define TFTP_TRANSFER_READ_COMPLETE   0x101                              // non-error
#define TFTP_TRANSFER_WRITE_COMPLETE  0x102                              // non-error
#define TFTP_FILE_EQUALITY            0x103                              // non-error
#define TFTP_ARP_RESOLUTION_FAILED    0x104                              // TFTP server could not be resolved at ARP level. Probably doesn't exist.
#define TFTP_ARP_RESOLVED             0x105                              // when acting as a client this can occur on transfer initialisation. The user should restart the request, which will then complete
#define TFTP_DESTINATION_UNREACHABLE  0x106                              // no port listening


/************************** FTP client *****************************************************************************/

// Callback codes                                                        {48}
//
#define FTP_CLIENT_EVENT_REQUEST_FTP_USER_NAME     0x0001                // application should return user name as string (null pointer or zero length for anonymous login)
#define FTP_CLIENT_EVENT_REQUEST_FTP_USER_PASSWORD 0x0002                // application should return user password as string
#define FTP_CLIENT_EVENT_LOGGED_IN                 0x0004                // successfully logged into FTP server
#define FTP_CLIENT_EVENT_LOGGED_FAILED             0x0080                // FTP server login failed
#define FTP_CLIENT_EVENT_CONNECTION_CLOSED         0x0005                // FTP server control connection closed
#define FTP_CLIENT_EVENT_ACTIVE_PASSIVE_LIST       0x0040                // application should return 0 for listing in passive mode or non-zero for listing in active mode
#define FTP_CLIENT_USER_NAME_ERROR                 0x0081                // login error - user name invalid
#define FTP_CLIENT_USER_PASS_ERROR                 0x0082                // login error - password invalid
#define FTP_CLIENT_EVENT_LISTING_DATA              0x0041                // received data from a listing passed to application
#define FTP_CLIENT_EVENT_GET_DATA                  0x0042                // received data from a get passed to application
#define FTP_CLIENT_EVENT_PUT_CAN_START             0x8001                // the data connection is ready and the application can now start sending data
#define FTP_CLIENT_EVENT_APPEND_CAN_START          0x8002                // the data connection is ready and the application can now start sending data
#define FTP_CLIENT_EVENT_DATA_CONNECTION_FAILED    0x0100                // data connection not possible for listing
#define FTP_CLIENT_EVENT_LOCATION_SET              0x0020                // new directory location successfully set
#define FTP_CLIENT_EVENT_LISTING_DATA_COMPLETE     0x0021                // listing completely received
#define FTP_CLIENT_EVENT_PUT_DATA_COMPLETE         0x0022                // put completed sucessfully
#define FTP_CLIENT_EVENT_APPEND_DATA_COMPLETE      0x0023                // append completed sucessfully
#define FTP_CLIENT_EVENT_GET_DATA_COMPLETE         0x0024                // get completely received
#define FTP_CLIENT_EVENT_DIR_CREATED               0x0025                // new directory was successfully created
#define FTP_CLIENT_EVENT_DIR_DELETED               0x0026                // empty directory successfully deleted
#define FTP_CLIENT_EVENT_FILE_DELETED              0x0027                // file successfully deleted
#define FTP_CLIENT_EVENT_RENAMED                   0x0028                // file or directory successfully renamed
#define FTP_CLIENT_EVENT_DATA_CONNECTED            0x0043                // data port connection has been successfully established
#define FTP_CLIENT_EVENT_DATA_SENT                 0x0044                // previous tcp packet was acknowledged
#define FTP_CLIENT_EVENT_DATA_DISCONNECTED         0x0045                // data connection closed
#define FTP_CLIENT_EVENT_DATA_LOST                 0x0083                // previous tcp packet was lost and needs to be repeated
#define FTP_CLIENT_EVENT_ACTIVE_LISTEN_DATA        0x0400                // the application should set a listening TCP port on the port passed number for active FTP server data connection
#define FTP_CLIENT_EVENT_PASSIVE_CONNECT_DATA      0x0800                // details of data connection passed to application to establish its own TCP connection
#define FTP_CLIENT_EVENT_FLAG_ASCII_MODE           0x1000                // the mode is ASCII rather than binary
#define FTP_CLIENT_EVENT_FLAG_PUT_DIRECTION        0x2000                // the data transfer direction is to the FTP server rather than from it
#define FTP_CLIENT_EVENT_FLAG_LISTING              0x4000                // the data is listing and not file content
#define FTP_CLIENT_ERROR_RENAMING_FILE             0x0200                // ftp command errors
#define FTP_CLIENT_ERROR_CREATING_DIRECTORY        0x0201
#define FTP_CLIENT_ERROR_DELETING_DIR              0x0202
#define FTP_CLIENT_ERROR_DELETING_FILE             0x0203
#define FTP_CLIENT_ERROR_SETTING_LOCATION          0x0204
#define FTP_CLIENT_ERROR_SETTING_MODE              0x0205
#define FTP_CLIENT_ERROR_SETTING_PORT              0x0206
#define FTP_CLIENT_ERROR_PASSIVE_MODE              0x0207
#define FTP_CLIENT_ERROR_LISTING                   0x0208
#define FTP_CLIENT_ERROR_APPENDING                 0x0209
#define FTP_CLIENT_ERROR_PUTTING                   0x020a
#define FTP_CLIENT_ERROR_GETTING                   0x020b
#define FTP_CLIENT_ERROR_FLAG                      0x0200                // flag in the code signalling an ftp error

typedef struct stTCP_CLIENT_MESSAGE_BOX
{
#if defined FTP_CLIENT_EXTERN_DATA_SOCKET                                // {64}
    unsigned char *ptrIPv6Address;                                       // pointer to IPv6 connection address (zero when IPv4 connection)
#endif
    int            iCallbackEvent;                                       // event
    unsigned char *ptrData;                                              // pointer to data
    unsigned short usDataLength;                                         // lenght of data
    unsigned short usDataPort;                                           // data port number
    unsigned char  ucIP_data_address[IPV4_LENGTH];                       // IP address for use by the data connection (only valid in IPv4 mode)
    USOCKET        uDataSocket;                                          // FTP client data socket
    USOCKET        uControlSocket;                                       // FTP client control socket
} TCP_CLIENT_MESSAGE_BOX;

/************************** SNMP ************************************************************************************/

#define SNMPV1                       0
#define SNMPV2                       1
#define SNMPV3                       2

#define SNMP_ERROR_STATUS_NO_ERROR             0
#define SNMP_ERROR_STATUS_TOO_BIG              1
#define SNMP_ERROR_STATUS_NO_SUCH_NAME         2
#define SNMP_ERROR_STATUS_BAD_VALUE            3
#define SNMP_ERROR_STATUS_READ_ONLY            4
#define SNMP_ERROR_STATUS_GENERAL_ERROR        5
#define SNMP_ERROR_STATUS_NO_ACCESS            6
#define SNMP_ERROR_STATUS_WRONG_TYPE           7
#define SNMP_ERROR_STATUS_WRONG_LENGTH         8
#define SNMP_ERROR_STATUS_WRONG_ENCODING       10
#define SNMP_ERROR_STATUS_NO_CREATION          11
#define SNMP_ERROR_STATUS_INCONSISTENT_VALUE   12
#define SNMP_ERROR_STATUS_RESOURCE_UNAVAILABLE 13
#define SNMP_ERROR_STATUS_COMMIT_FAILED        14
#define SNMP_ERROR_STATUS_UNDO_FAILED          15
#define SNMP_ERROR_STATUS_AUTHORIZATION_ERROR  16
#define SNMP_ERROR_STATUS_NOT_WRITABLE         17
#define SNMP_ERROR_STATUS_INCONSISTENT_NAME    18

#define SNMP_GET_REQUEST             0
#define SNMP_GET_NEXT_REQUEST        1
#define SNMP_GET_RESPONSE            2
#define SNMP_SET_REQUEST             3
#define SNMP_TRAP                    4

#define SNMP_TRAP_TX_RESOLVING                 -1
#define SNMP_TRAP_TX_NOT_SUPPORTED_VERSION     -2
#define SNMP_TRAP_TX_FORMATTING_ERROR          -3

// PDUs
//
#define ASN1_SNMP_GET_REQUEST        0xa0
#define ASN1_SNMP_GET_NEXT_REQUEST   0xa1
#define ASN1_SNMP_GET_RESPONSE       0xa2
#define ASN1_SNMP_SET_REQUEST        0xa3
#define ASN1_SNMP_TRAP               0xa4                                // only SNMPv1
#define ASN1_SNMP_GET_BULK_REQUEST   0xa5                                // SNMPV2c {100a}
#define ASN1_SNMP_INFORM_REQUEST     0xa6                                // SNMPV2c
#define ASN1_SNMP_TRAP2              0xa7                                // SNMPV2c
#define ASN1_SNMP_REPORT             0xa8                                // SNMPV2c

#define ASN1_SEQUENCE                0x30


#define ASN1_UNKNOWN_CODE            0x00                                // pseudo value (used internally but not inserted into messages)
#define ASN1_PHY_ADDRESS_CODE        0x01                                // pseudo value (used internally but not inserted into messages)
#define ASN1_INTEGER_CODE            0x02
#define ASN1_BIT_STRING_CODE         0x03
#define ASN1_OCTET_STRING_CODE       0x04
#define ASN1_NULL_CODE               0x05
#define ASN1_OBJECT_IDENT_CODE       0x06
#define ASN1_IP_ADDRESS_CODE         0x40
#define ASN1_GAUGE_CODE              0x42
#define ASN1_TIME_STAMP_CODE         0x43

#define SNMP_OBJECT_ERROR_NO_SUCH_OBJECT       0x80
#define SNMP_OBJECT_ERROR_NO_SUCH_INSTANCE     0x81
#define SNMP_OBJECT_ERROR_END_OF_MIB_VIEW      0x82

// Traps
//
#define SNMP_COLDSTART               0
#define SNMP_WARMSTART               1
#define SNMP_LINK_DOWN               2
#define SNMP_LINK_UP                 3
#define SNMP_AUTH_FAILURE            4
#define SNMP_EGP_NEIGHBORLOSS        5
#define SNMP_ENTERPRISE_SPECIFIC     6

// Callback codes
//
#define SNMP_COMMUNITY_CHECK_READ    0                                   // {96}
#define SNMP_COMMUNITY_CHECK_WRITE   1
#define SNMP_GET_ENTERPRISE          2
#define SNMP_GET_COMMUNITY           3
#define SNMP_GET_ERROR_STATUS        4
#define SNMP_GET_ERROR_INDEX         5
#define SNMP_GET_VARIABLE_BINDING    6

#define SNMP_ERROR_NO_ARP_RES        10
#define SNMP_ERROR_V2_NOT_SUPPORTED  11
#define SNMP_ERROR_V3_NOT_SUPPORTED  12
#define SNMP_ERROR_INVALID_WRITE_ATTEMPT     13
#define SNMP_ERROR_UNPERMITTED_WRITE_ATTEMPT 14


#define SNMP_COMMUNITY_MISMATCH      -1
#define SNMP_COMMUNITY_READ_ONLY     0
#define SNMP_COMMUNITY_READ_WRITE    1


typedef struct stMIB_CONTROL {
    unsigned char *ptrContent;
    int iObjectsCount;
    int iCommunityAccessRights;
    unsigned long ulRequestID;
    unsigned long ulErrorStatus;
    unsigned long ulErrorIndex;
#if defined SUPPORT_SNMPV2C
    unsigned long ulMaxRepetitions;
#endif
    unsigned long ulInteger;
    const CHAR *ptrString;
    unsigned char ucVariableBinding[64];                                 // longest foreseen variable binding
    unsigned char ucVariableBindingLength;
    unsigned char ucSNMP_version;
    unsigned char ucObjectType;
    unsigned char ucObjectLength;
    unsigned char ucLevel;
#if defined SUPPORT_SNMPV2C
    unsigned char ucBulkLevel;
    unsigned char ucTrapReference;
#endif
    unsigned char ucRequestType;
}
MIB_CONTROL;

typedef struct stMIB_TABLE
{
    signed char   cLevel;                                                // level in path
    unsigned char ucNode;                                                // node value to match in path
    unsigned char ucAccessType;                                          // read/write or read-only
    int (*snmp_get_set_handler)(MIB_CONTROL *ptrMIB_control);            // get/set handler
} MIB_TABLE;

#define MIB_R_W                  0x00                                    // read-write access as long as the community has write access rights too
#define MIB_R_ONLY               0x01                                    // unconditionally read-only
#define MIB_TAB                  0x02                                    // the handling routine supports tables

#define NOT_ACCESSIBLE           0                                       // location access is non-accessible
#define NOT_SUPPORTED            0                                       // not supported by this software implementation

typedef struct stSNMP_MANAGER                                            // {100a}
{
    unsigned char  snmp_manager_ip_address[IPV4_LENGTH];    
    USOCKET        snmp_manager_details;
    unsigned char  ucSNMP_versions;
} SNMP_MANAGER;

#define SNMP_MANAGER_V1 0x01
#define SNMP_MANAGER_V2 0x02

extern int  fnStartSNMP(int (*fnAppCallback)(unsigned char ucEvent, unsigned char *data, unsigned short usLength, int iManagerReference), const SNMP_MANAGER *ptrManagerDetails, const MIB_TABLE *ptr_MIB_table);
extern int  fnConnectSMTP(unsigned char *ucIP, unsigned char ucMode, const CHAR *(*fnCallback)(unsigned char, unsigned short *));
extern CHAR *fnCopyASN1_string(CHAR *ptrBuffer, unsigned char *ptrData, unsigned short usBufferLength);
extern void fnSendSNMPTrap(unsigned char ucTrap, unsigned char ucSpecificCode, unsigned long ulManagers);
    #define ALL_SNMP_MANAGERS    0xffffffff
extern unsigned char *fnInsertOID_value(unsigned char *ucData, unsigned long ulValue, int *iBufferSpace); // {97}
extern unsigned long fnExtractOID_value(unsigned char *ucPtrVariableBinding); // {97}

/************************** Ethernet/IP *****************************************************************************/

#define ETH_IP_UINT_SIZE           2
#define ETH_IP_UDINT_SIZE          4
__PACK_ON
typedef struct _PACK stETH_IP_ENCAPSULATION_HEADER                       // fixed length header (24 bytes) integer fields are transmitted in little-endian order
{
    unsigned char  Command[ETH_IP_UINT_SIZE];
    unsigned char  Length[ETH_IP_UINT_SIZE];
    unsigned char  Session_handle[ETH_IP_UDINT_SIZE];
    unsigned char  Status[ETH_IP_UDINT_SIZE];
    unsigned char  SenderContext[8];
    unsigned char  OPtions[ETH_IP_UDINT_SIZE];
} ETH_IP_ENCAPSULATION_HEADER;



/************************** Well known Port numbers *****************************************************************/

// UDP ports
//
#define DNS_UDP_PORT               53
#define DHCP_SERVER_PORT           67
#define DHCP_CLIENT_PORT           68
#define TFTP_SERVER_PORT           69
#define SNTP_PORT                  123                                   // {27}
#define NetBIOS_PORT               137                                   // {7}
#define SNMP_AGENT_PORT            161
#define SNMP_MANAGER_PORT          162
#define mDNS_PORT                  5353
#define ETHERNET_IP_PORT           44818                                 // 0xaf12 - at least 2 sockets expected

// TCP ports
//
#define FTP_DATA_PORT              20
#define FTP_CONTROL_PORT           21
#define TELNET_SERVERPORT          23
#define SMTP_PORT                  25
#define TIME_PORT                  37
#define HTTP_SERVERPORT            80
#define POP_PORT                   110
#define HTTPS_SERVERPORT           443
//#define ETHERNET_IP_PORT         44818                                 // used by both UDP and TCP
#define MQTT_PORT                  1883
#define MQTTS_PORT                 8883


// Cipher suites
//
#define TLS_ECDHE_ECDSA_WITH_AES_128_GCM_SHA256      0xc02b
#define TLS_ECDHE_RSA_WITH_AES_128_GCM_SHA256        0xc02f
#define TLS_ECDHE_ECDSA_WITH_AES_128_CBC_SHA256      0xc023
#define TLS_ECDHE_RSA_WITH_AES_128_CBC_SHA256        0xc027
#define TLS_ECDHE_ECDSA_WITH_AES_256_CBC_SHA         0xc00a
#define TLS_ECDHE_ECDSA_WITH_AES_128_CBC_SHA         0xc009
#define TLS_ECDHE_RSA_WITH_AES_128_CBC_SHA           0xc013
#define TLS_ECDHE_RSA_WITH_AES_256_CBC_SHA           0xc014
#define TLS_ECDHE_ECDSA_WITH_RC4_128_SHA             0xc007
#define TLS_ECDHE_RSA_WITH_RC4_128_SHA               0xc011
#define TLS_RSA_WITH_AES_128_GCM_SHA256              0x009c
#define TLS_DHE_RSA_WITH_AES_128_CBC_SHA             0x0033
#define TLS_DHE_DSS_WITH_AES_128_CBC_SHA             0x0032
#define TLS_DHE_RSA_WITH_AES_256_CBC_SHA             0x0039
#define TLS_RSA_WITH_AES_128_CBC_SHA                 0x002f
#define TLS_RSA_WITH_AES_256_CBC_SHA                 0x0035
#define TLS_RSA_WITH_3DES_EDE_CBC_SHA                0x000a
#define TLS_RSA_WITH_RC4_128_SHA                     0x0005
#define TLS_RSA_WITH_RC4_128_MD5                     0x0004
#define TLS_EMPTY_RENEGOTIATION_INFO_SCSV            0x00ff

// Secure sockets layer - messages
//
#define SSL_TLS_CONTENT_CHANGE_CIPHER_SPEC           0x14                // 20
#define SSL_TLS_CONTENT_ALERT                        0x15                // 21
#define SSL_TLS_CONTENT_HANDSHAKE                    0x16                // 22
#define SSL_TLS_CONTENT_APPLICATION_DATA             0x17                // 23

#define TLS_VERSION_1_0                              0x0301              // version number - follows messages (plus length in two bytes)
#define TLS_VERSION_1_2                              0x0303

// Secure sockets layer - handshakes (followed by three byte length)
//
#define SSL_TLS_HANDSHAKE_TYPE_HELLO_REQUEST         0x00
#define SSL_TLS_HANDSHAKE_TYPE_CLIENT_HELLO          0x01
#define SSL_TLS_HANDSHAKE_TYPE_SERVER_HELLO          0x02
#define SSL_TLS_HANDSHAKE_TYPE_HELLO_VERIFY_REQUEST  0x03
#define SSL_TLS_HANDSHAKE_TYPE_NEW_SESSION_TICKET    0x04
#define SSL_TLS_HANDSHAKE_TYPE_CERTIFICATE           0x0b                // 11
#define SSL_TLS_HANDSHAKE_TYPE_SERVER_KEY_EXCHANGE   0x0c                // 12
#define SSL_TLS_HANDSHAKE_TYPE_CERTIFICATE_REQUEST   0x0d                // 13
#define SSL_TLS_HANDSHAKE_TYPE_SERVER_HELLO_DONE     0x0e                // 14
#define SSL_TLS_HANDSHAKE_TYPE_CERTIFICATE_VERIFY    0x0f                // 15
#define SSL_TLS_HANDSHAKE_TYPE_CLIENT_KEY_EXCHANGE   0x10                // 16
#define SSL_TLS_HANDSHAKE_TYPE_FINISHED              0x14                // 20

#define SSL_TLS_CHANGE_CIPHER_SPEC_MESSAGE           0x01

// Alert levels and codes
//
#define SSL_TLS_ALERT_LEVEL_WARNING                  0x01
#define SSL_TLS_ALERT_LEVEL_FATAL                    0x02

#define SSL_TLS_ALERT_MSG_CLOSE_NOTIFY               0x00                // 0
#define SSL_TLS_ALERT_MSG_UNEXPECTED_MESSAGE         0x0a                // 10
#define SSL_TLS_ALERT_MSG_BAD_RECORD_MAC             0x14                // 20 
#define SSL_TLS_ALERT_MSG_DECRYPTION_FAILED          0x15                // 21 
#define SSL_TLS_ALERT_MSG_RECORD_OVERFLOW            0x16                // 22 
#define SSL_TLS_ALERT_MSG_DECOMPRESSION_FAILURE      0x1E                // 30 
#define SSL_TLS_ALERT_MSG_HANDSHAKE_FAILURE          0x28                // 40 
#define SSL_TLS_ALERT_MSG_NO_CERT                    0x29                // 41 
#define SSL_TLS_ALERT_MSG_BAD_CERT                   0x2A                // 42 
#define SSL_TLS_ALERT_MSG_UNSUPPORTED_CERT           0x2B                // 43 
#define SSL_TLS_ALERT_MSG_CERT_REVOKED               0x2C                // 44 
#define SSL_TLS_ALERT_MSG_CERT_EXPIRED               0x2D                // 45 
#define SSL_TLS_ALERT_MSG_CERT_UNKNOWN               0x2E                // 46 
#define SSL_TLS_ALERT_MSG_ILLEGAL_PARAMETER          0x2F                // 47 
#define SSL_TLS_ALERT_MSG_UNKNOWN_CA                 0x30                // 48 
#define SSL_TLS_ALERT_MSG_ACCESS_DENIED              0x31                // 49 
#define SSL_TLS_ALERT_MSG_DECODE_ERROR               0x32                // 50 
#define SSL_TLS_ALERT_MSG_DECRYPT_ERROR              0x33                // 51 
#define SSL_TLS_ALERT_MSG_EXPORT_RESTRICTION         0x3C                // 60 
#define SSL_TLS_ALERT_MSG_PROTOCOL_VERSION           0x46                // 70 
#define SSL_TLS_ALERT_MSG_INSUFFICIENT_SECURITY      0x47                // 71 
#define SSL_TLS_ALERT_MSG_INTERNAL_ERROR             0x50                // 80 
#define SSL_TLS_ALERT_MSG_INAPROPRIATE_FALLBACK      0x56                // 86 
#define SSL_TLS_ALERT_MSG_USER_CANCELED              0x5A                // 90 
#define SSL_TLS_ALERT_MSG_NO_RENEGOTIATION           0x64                // 100
#define SSL_TLS_ALERT_MSG_UNSUPPORTED_EXT            0x6E                // 110
#define SSL_TLS_ALERT_MSG_UNRECOGNIZED_NAME          0x70                // 112
#define SSL_TLS_ALERT_MSG_UNKNOWN_PSK_IDENTITY       0x73                // 115
#define SSL_TLS_ALERT_MSG_NO_APPLICATION_PROTOCOL    0x78                // 120

typedef struct _PACK stSSL_TLS_EXTENSION
{
    unsigned char  extension_type[2];
    unsigned char  length[2];
    unsigned char  renegotiation_info_extension;
} SSL_TLS_EXTENSION;

typedef struct _PACK stSSL_TLS_HANDSHAKE_PROTOCOL_HELLO_DETAILS
{
    unsigned char  cipher[2];
    unsigned char  compression_method;
    unsigned char  extensionsLength[2];
    SSL_TLS_EXTENSION extension;
} SSL_TLS_HANDSHAKE_PROTOCOL_HELLO_DETAILS;

typedef struct _PACK stSSL_TLS_HANDSHAKE_PROTOCOL_HELLO_NO_ID
{
    unsigned char  version[2];
    unsigned char  random[32];
    unsigned char  session_id_length;
    SSL_TLS_HANDSHAKE_PROTOCOL_HELLO_DETAILS session_details;
} SSL_TLS_HANDSHAKE_PROTOCOL_HELLO_NO_ID;

typedef struct _PACK stSSL_TLS_HANDSHAKE_PROTOCOL_HELLO_16_ID
{
    unsigned char  version[2];
    unsigned char  random[32];
    unsigned char  session_id_length;
    unsigned char  session_id[16];
    SSL_TLS_HANDSHAKE_PROTOCOL_HELLO_DETAILS session_details;
} SSL_TLS_HANDSHAKE_PROTOCOL_HELLO_16_ID;

typedef struct _PACK stSSL_TLS_HANDSHAKE_PROTOCOL_HELLO_32_ID
{
    unsigned char  version[2];
    unsigned char  random[32];
    unsigned char  session_id_length;
    unsigned char  session_id[32];
    SSL_TLS_HANDSHAKE_PROTOCOL_HELLO_DETAILS session_details;
} SSL_TLS_HANDSHAKE_PROTOCOL_HELLO_32_ID;

typedef struct _PACK stSSL_TLS_HANDSHAKE_PROTOCOL
{
    unsigned char  handshake_type;
    unsigned char  length[3];
    SSL_TLS_HANDSHAKE_PROTOCOL_HELLO_32_ID handshake_protocol;
} SSL_TLS_HANDSHAKE_PROTOCOL;

typedef struct _PACK stSSL_TLS_RECORD
{
    unsigned char  content_type;
    unsigned char  version[2];
    unsigned char  length[2];
    SSL_TLS_HANDSHAKE_PROTOCOL handshake;
} SSL_TLS_RECORD;

/* =================================================================== */
/*                  multi-network/interface socket control             */
/* =================================================================== */

#define extractVLAN(Socket)          (unsigned char)(((Socket) >> VLAN_SHIFT) & VLAN_MASK) // {69}
#define extractNetwork(Socket)       (unsigned char)(((Socket) >> NETWORK_SHIFT) & NETWORK_MASK)
#define extractInterface(Socket)     (unsigned char)(((Socket) >> INTERFACE_SHIFT) & INTERFACE_MASK)
#define defineNetwork(Network)       (USOCKET)(((Network) & NETWORK_MASK) << NETWORK_SHIFT)
#define defineInterface(Interfaces)  (USOCKET)(((0x01 << (Interfaces)) & INTERFACE_MASK) << INTERFACE_SHIFT)
#define addInterface(Interface)      (USOCKET)(((Interface) & INTERFACE_MASK) << INTERFACE_SHIFT) // {65}

#if defined SUPPORT_VLAN && defined SUPPORT_DYNAMIC_VLAN                 // {75}
    #define defineVLAN(VLAN)         (USOCKET)(((VLAN) & VLAN_MASK) << VLAN_SHIFT) // {69}{85}
    #define addVLAN(Socket, VLAN)    (Socket) |= defineVLAN(VLAN)
#else
    #define defineVLAN(VLAN)         0                                   // {85}
    #define addVLAN(Socket, VLAN)
#endif

#if IP_NETWORK_COUNT > 1 && IP_INTERFACE_COUNT > 1
    #define NetworkInterface(Network, Interfaces)  (USOCKET)(defineNetwork(Network) | addInterface(Interfaces)) // {65}
#elif IP_INTERFACE_COUNT > 1
    #define NetworkInterface(Network, Interfaces)  (USOCKET)(0 | addInterface(Interfaces)) // {65}
#elif IP_NETWORK_COUNT > 1
    #define NetworkInterface(Network, Interfaces)  (USOCKET)(defineNetwork(Network) | 0) // {68}
#else
    #define NetworkInterface(Network, Interfaces)  (USOCKET)(0 | 0)
#endif
#define addNetworkInterface(Socket, Network, Interfaces)  Socket |= NetworkInterface(Network, Interfaces)

#if IP_INTERFACE_COUNT > 1
    extern int fnGetInterfaceCount(USOCKET cSocket);
    extern int fnGetInterfaceReference(USOCKET cSocket);
#endif



/* =================================================================== */
/*                      global function definitions                    */
/* =================================================================== */


extern unsigned char fnAssignNetwork(ETHERNET_FRAME *frame, unsigned char *ptrDestinationIP); // {86} user supplied routine which decides which network a received frame is handled on
    #define NETWORK_REJECT 0xff
extern int fnHandleEthernetFrame(ETHERNET_FRAME *ptr_rx_frame, QUEUE_HANDLE interface_handle); // {98}
extern void fnEthernetStateChange(int iInterface, unsigned char ucEvent);// {98}
extern unsigned char fnGetLinkState(int iInterface);                     // {98}
extern void fnTxStats(unsigned char ucProtType, int iNetworkReference);  // {87}
extern void fnRxStats(unsigned char ucProtType, int iNetworkReference);
extern void fnIncrementEthernetStats(unsigned char ucStat, int iNetworkReference);
extern unsigned long fnGetEthernetStats(unsigned char ucStat, int iNetworkReference);
extern unsigned long fnGetuNetworkStats(unsigned char ucStat, int iNetworkReference);
extern void fnDeleteEthernetStats(int iNetworkReference);
extern void fnResetuNetwork_stats(void);                                 // {16}
extern int fnProcessARP(ETHERNET_FRAME *frame);                          // process a received ethernet frame which seems to be ARP type
extern ARP_TAB *fnGetIP_ARP(unsigned char *Search_IP, UTASK_TASK OwnerTask, USOCKET cSocket); // get destination details from ARP cache
extern ARP_TAB *fnGetARPentry(unsigned char ucEntry);                    // {62} get a pointer to an ARP cache entry
extern unsigned char *fnGetNeighborEntry(unsigned char ucEntry, int iIP_MAC); // {34}
    #define GET_IP    1
    #define GET_MAC   0
#if IP_INTERFACE_COUNT > 1 && defined RESTRICTED_GATEWAY_INTERFACE
    extern void fnRestrictGatewayInterface(ARP_TAB *ptrARPTab);          // {81}
#endif
extern void fnDeleteArp(void);                                           // delete ARP cache
#if defined USE_ZERO_CONFIG
    extern void fnSendARP_request(ARP_TAB *ptrARPTab);
#endif
extern void fnDeleteNeighbors(void);                                     // {34} delete IPV6 neighbor cache
extern int fnSubnetBroadcast(unsigned char *ip_address, unsigned char *out_ip_address, unsigned char *subnet_mask, unsigned char ucLength); // {66}
extern int fnHandleIPv4(ETHERNET_FRAME *frame);                          // {53}
extern int fnHandleIP_ICMP(ETHERNET_FRAME *frame, IP_PACKET *received_ip_packet);
#if defined RUN_LOOPS_IN_RAM
    extern void fnInitIP(void);
    extern unsigned short (*fnCalcIP_CS)(unsigned short cs, unsigned char *dat, unsigned short usLen);
#else
    extern unsigned short fnCalcIP_CS(unsigned short cs, unsigned char *dat, unsigned short usLen);
#endif
    #define IP_GOOD_CS 0xffff
extern void fnAddARP(unsigned char *new_ip, unsigned char *rem_hwadr, ARP_DETAILS *ptrARP_details); // {70}
extern void fnSendIPV6Discovery(NEIGHBOR_TAB *ptrNeighbor);              // {93}
extern NEIGHBOR_TAB *fnEnterIPV6Neighbor(unsigned char *ucHWAddress, unsigned char *ucIPV6Address, unsigned char ucHWAddress_length); // {34}
    #define REFRESH_ADDRESS    0x00
    #define SEARCH_ADDRESS     0x40
    #define RESOLVED_ADDRESS   0x80
extern unsigned char *fnGetIPV6_NN(unsigned char *ucIPV6Address, UTASK_TASK OwnerTask, USOCKET Socket); // {34}
extern void fnSetIPv6in4Destinations(IPV6INV4_RELAY_DESTINATION *ptrTable); // {55}
extern void fnHandleICMP(ETHERNET_FRAME *ptrRx_frame);
extern void fnHandleICMPV6(ETHERNET_FRAME *frame);                       // {34}{50}
extern int  fnHandleIPV6in4(ETHERNET_FRAME *frame, unsigned short usIPLength); // {51}
extern int  fnSendPing(unsigned char *ping_address, unsigned char ucTTL, UTASK_TASK OwnerTask, USOCKET Socket);
extern int  fnSendV6Ping(unsigned char *ping_address, unsigned char ucTTL, UTASK_TASK OwnerTask, USOCKET Socket); // {34}
extern void fnHandleTCP(ETHERNET_FRAME *ptrRx_frame);                    // {53}
extern void fnHandleIGMP(ETHERNET_FRAME *ptrRx_frame);                   // {42}
extern void fnHandleUDP(ETHERNET_FRAME *ptrRx_frame);                    // {73}
extern USOCKET fnGetTCP_Socket(unsigned char ucTos, unsigned short usIdleTimeout, int (*listener)(USOCKET, unsigned char, unsigned char *, unsigned short));
    #define INFINITE_TIMEOUT 0xffff                                      // {13}
extern USOCKET fnGetUDP_socket(unsigned char ucTOS, int (*fnListener)(USOCKET, unsigned char, unsigned char *, unsigned short, unsigned char *, unsigned short), unsigned char ucOpts);
extern USOCKET fnReleaseUDP_socket(USOCKET SocketHandle);
extern USOCKET fnReleaseTCP_Socket(USOCKET TCPSocket);
extern USOCKET fnBindSocket(USOCKET SocketHandle, unsigned short usLocalPort);
extern USOCKET fnTCP_Listen(USOCKET TCP_socket, unsigned short usPort, unsigned short usMaxWindow);
extern unsigned short fnGetFreeTCP_Port(void);
extern signed short fnSendUDP(USOCKET cSocketHandle, unsigned char *dest_IP, unsigned short ucRemotePort, unsigned char *ptrBuf, unsigned short usDataLen, UTASK_TASK OwnerTask);
extern void fnReportUDP(unsigned short usSourcePort, unsigned short usDestPort, unsigned char ucEvent, unsigned char *ucIP);
extern signed short fnSendIPv4(unsigned char *prIP_to, unsigned char ucProtType, unsigned char ucTypeOfService, unsigned char ucTTL, unsigned char *dat, unsigned short usLen, UTASK_TASK OwnerTask, USOCKET cSocket);
extern signed short fnSendIPV6(unsigned char *prIP_to, unsigned char ucProtType, unsigned char ucTTL, unsigned char *dat, unsigned short usLen, UTASK_TASK Owner, USOCKET cSocket); // {34}
extern void fnSendICMPError(ICMP_ERROR *tICMP_error, unsigned short usLength);
#if defined USE_IPV6
    extern USOCKET fnTCP_Connect(USOCKET TCP_socket, unsigned char *RemoteIP, unsigned short usRemotePort, unsigned short usOurPort, unsigned long ulMaxWindow_flags); // {57}
       #define TCP_CONNECT_IPV6 0x00010000
#else
    extern USOCKET fnTCP_Connect(USOCKET TCP_socket, unsigned char *RemoteIP, unsigned short usRemotePort, unsigned short usOurPort, unsigned short usMaxWindow);
#endif
extern void *fnInsertSecureLayer(USOCKET TCP_socket, int(*listener)(USOCKET, unsigned char, unsigned char *, unsigned short), int iInsert);
extern USOCKET fnTCP_close(USOCKET TCP_Socket);
//extern unsigned char fnGetTCP_state(USOCKET TCP_socket);               // {14}{94}
extern TCP_CONTROL *fnGetSocketControl(USOCKET TCP_socket);              // {94}
extern unsigned short fnGetTCP_port(USOCKET TCP_socket, int iDestination); // {71}
    #define GET_LOCAL_PORT             0
    #define GET_REMOTE_PORT            1
extern signed short fnSendTCP(USOCKET TCP_socket, unsigned char *ptrBuf, unsigned short usDataLen, unsigned char ucTempFlags);
#if defined _EXTENDED_BUFFERED_TCP                                       // {47}
    typedef unsigned short COMMAND_TYPE;
#else
    typedef unsigned char COMMAND_TYPE;
#endif
    extern QUEUE_TRANSFER fnSendBufTCP(USOCKET TCP_socket, unsigned char *ptrBuf, unsigned short usDataLen, COMMAND_TYPE Command);
    #define TCP_BUF_SEND               0x01
    #define TCP_BUF_NEXT               0x02
    #define TCP_BUF_REP                0x04
    #define TCP_BUF_CHECK              0x08
    #define TCP_BUF_SEND_REPORT_COPY   0x10
    #define TCP_CONTENT_NEGOTIATION    0x20
    #define TCP_REPEAT_WINDOW          0x40
    #define TCP_BUF_KICK_NEXT          0x80
    #define TCP_BUF_QUEUE              0x0100                            // {47} queue the data if the connection is not yet established
    #define TCP_BUF_PREPARE            0x0200                            // {106} allow preparing data in the output buffer of an open connection but not start its transmission yet
#if defined INDIVIDUAL_BUFFERED_TCP_BUFFER_SIZE                          // {40} 
    extern unsigned short fnDefineTCPBufferSize(USOCKET TCP_socket, unsigned short usBufferSize); // enter the buffer size associated with a buffered TCP socket
#endif
extern USOCKET fnModifyTCPWindow(USOCKET TCPSocket, unsigned short usBufferSpace); // {33}
extern signed short fnReportTCPWindow(USOCKET TCPSocket, unsigned short usBufferSpace); // {31}
extern void fnTCP_Activity(USOCKET TCP_socket);                          // {6}
extern int  fnActiveTCP_connections(int iReset);                         // {44}
    #define SEARCH_CONNECTION 0
    #define RESET_CONNECTIONS 1
extern USOCKET fnTCP_IdleTimeout(USOCKET TCPSocket, unsigned short usIdleTimeout); // {49}
extern unsigned char *fnInsertTCPHeader(USOCKET TCPSocket, unsigned char *ptrBuffer); // {105}
extern int fnSecureLayerTransmission(USOCKET Socket, unsigned char *ucPrtData, unsigned short usLength, unsigned char ucFlag);// {105}

extern signed short fnResolveHostName(const CHAR *cHostNamePtr, void (*fnListener)(unsigned char , unsigned char *));
extern int  fnConnectPOP3(unsigned char *ucIP);
extern void fnStartPopPolling(DELAY_LIMIT PollTime, CHAR *(*fnCallback)(unsigned char, unsigned char *));
extern void fnStartSNTP(DELAY_LIMIT syncDelay);
extern void fnStartTimeServer(DELAY_LIMIT syncDelay);
extern int  fnStartDHCP(UTASK_TASK OwnerTask, USOCKET uDetails);         // {99}
    #define FORCE_INIT            0x80                                   // UTASK_TASK option
    #define DHCP_CLIENT_OPERATION 0x01                                   // USOCKET options
    #define DHCP_SERVER_OPERATION 0x02
#if defined DHCP_HOST_NAME
    extern CHAR *fnGetDHCP_host_name(unsigned char *ptr_ucHostNameLength, int iNetwork); // {77}
#endif
extern void fnStartZeroConfig(UTASK_TASK Task);                          // {43}
extern void fnStopZeroConfig(void);
extern void fnCheckZeroConfigCollision(unsigned char *ptrData);          // {43}
extern void fnStopDHCP(int iNetwork);                                    // {99}
extern USOCKET fnStartNetBIOS_Server(CHAR *);                            // {7}{78}
extern int  fnStartTFTP_server(void (*Callback)(unsigned short, CHAR *));
extern void fnStopTFTP_server(void);
extern int  fnStartTFTP_client(void (*Callback)(unsigned short, CHAR *), unsigned char *ucIP, unsigned char ucReadWrite, CHAR *cFile, CHAR uFile);
    #define TFTP_PUT          0x00
    #define TFTP_GET          0x01
    #define TFTP_GET_COMPARE  0x02
#if defined FNGENERATOR_PASS_HTTP                                        // {38}
    #define FGEN_PROTO unsigned char (*fnGenerator)(unsigned char *, HTTP *)
#else
    #define FGEN_PROTO unsigned char (*fnGenerator)(unsigned char *)
#endif
extern int fnStart_mDNS(USOCKET network_interface);

typedef struct stHTTP_FUNCTION_SET                                       // {101}
{
    int (*fnWebHandler)(unsigned char, CHAR *, HTTP *);
#if defined FNGENERATOR_PASS_HTTP
    unsigned char (*fnGenerator)(unsigned char *, HTTP *);
#else
    unsigned char (*fnGenerator)(unsigned char *);
#endif
#if defined HTTP_DYNAMIC_CONTENT
    CHAR *(*fnInsertRoutine)(unsigned char *, LENGTH_CHUNK_COUNT, unsigned short *, HTTP *);
#else
    CHAR *(*fnInsertRoutine)(unsigned char *, LENGTH_CHUNK_COUNT, unsigned short *);
#endif
#if defined SUPPORT_WEBSOCKET
    int (*fnWebSocketHandler)(unsigned char *, unsigned long, unsigned char, HTTP *);
#endif
#if defined _VARIABLE_HTTP_PORT
    unsigned short usPort;
#endif
    unsigned char  ucParameters;
} HTTP_FUNCTION_SET;

extern void fnStartHTTP(HTTP_FUNCTION_SET *ptrFunctionSet);              // {101}
    #define NOT_SELECTED   0
    #define IS_SELECTED    1
    #define IS_CHECKED     2
    #define IS_DISABLED    3
extern void fnStopHTTP(void);
extern int fnWebSocketSend(unsigned char *ptrPayload, unsigned long ulPayloadLength, unsigned char ucOpCode, HTTP *http_session);

#if defined PASS_SERVER_DELAYED_STRING
    extern unsigned char fnServeDelayed(CHAR *cFile, unsigned char ucOption);
#else
    extern unsigned char fnServeDelayed(CHAR cFile, unsigned char ucOption);
#endif
    #define WEB_SUPPORT_PARAM_GEN 0x01
    #define WEB_SUPPORT_HANDLER   0x02
    #define WEB_AUTHENTICATE      0x04
    #define WEB_TRUSTED_IP        0x08
    #define WEB_BLACKLIST_IP      0x10                                   // {39}
    #define WEB_UTFAT_OFF         0x20                                   // {58} utFAT is disabled initially (default is on when utFAT is in operation)

#if defined IMMEDIATE_MEMORY_ALLOCATION                                  // {79}
    extern USOCKET fnStartTelnet(unsigned short usTelnetPortNumber, unsigned short usIdleTimeout, unsigned short usMaxWindow, unsigned short usTxBufSize, UTASK_TASK wakeOnAck, int (*listener)(USOCKET, unsigned char, unsigned char *, unsigned short));
#else
    extern USOCKET fnStartTelnet(unsigned short usTelnetPortNumber, unsigned short usIdleTimeout, unsigned short usMaxWindow, UTASK_TASK wakeOnAck, int (*listener)(USOCKET, unsigned char, unsigned char *, unsigned short));
#endif
extern void fnStopTelnet(USOCKET TelnetSocket);
extern int  fnTelnet(USOCKET Telnet_socket, int iCommand);
extern int  fnCheckTelnetBinaryTx(USOCKET Socket);
extern USOCKET fnConnectTELNET(TELNET_CLIENT_DETAILS *ptrTelnetClientDetails); // {91}
#if defined TELNET_RFC2217_SUPPORT
    extern USOCKET fnTelnetRF2217(RFC2217_SESSION_CONFIG *ptrConfig);
#endif

extern void fnStartFtp(unsigned short usFTPTimeout, unsigned char ucFTP_operating_mode);
    #define FTP_AUTHENTICATE       0x01                                  // require authentication rather than accepting anonymous login
    #define FTP_UTFAT_OFF          0x02                                  // {58} utFAT is disabled initially (default is on when utFAT is in operation)

extern void fnStopFtp(void);
extern USOCKET fnFTP_client_connect(unsigned char *ptr_ucIP_address, unsigned short ucPortNumber, unsigned short usFTPTimeout, int (*user_callback)(TCP_CLIENT_MESSAGE_BOX *), int iFlags); // {48}{64}
    #define FTP_CLIENT_IPv4        0
    #define FTP_CLIENT_IPv6        1
extern int  fnFTP_client_disconnect(void);                               // {48}
extern int  fnFTP_client_dir(CHAR *ptrPath, int iAction);                // {48}
    #define FTP_DIR_LIST           0x00
    #define FTP_DIR_SET_PATH       0x01
    #define FTP_DIR_MAKE_DIR       0x02
    #define FTP_DIR_RENAME         0x04
    #define FTP_DIR_DELETE         0x08
    #define FTP_DIR_REMOVE_DIR     0x10
extern int  fnFTP_client_transfer(CHAR *ptrFilePath, int iMode);         // {48}
    #define FTP_TRANSFER_BINARY    0x00
    #define FTP_TRANSFER_ASCII     0x01
    #define FTP_DO_GET             0x00
    #define FTP_DO_PUT             0x02
    #define FTP_DO_APPEND          0x04
extern int  fnInsertHTMLString(CHAR *cToAdd, unsigned short usAddLength, unsigned char **ptrBuffer, unsigned short *usMaxLen, unsigned short *usLen, unsigned char *ptrBufferEnd); // {9}
extern int  fnDecode64(CHAR *ptrInput, CHAR *ptrOutput);                 // {100}
extern MAX_FILE_LENGTH fnEncode64(unsigned char *ptrInput, CHAR *ptrOutput, MAX_FILE_LENGTH input_length); // {15}
extern int  fnVerifyUser(CHAR *cDecodedUser, unsigned char iCheckUser);
    #define DO_CHECK_USER_NAME            0x01
    #define DO_CHECK_PASSWORD             0x02
    #define HTML_PASS_CHECK               0x04
    #define FTP_PASS_CHECK                0x08

extern int fnConnectMQTT(unsigned char *ucIP, unsigned short(*fnCallback)(signed char, unsigned char *, unsigned long, unsigned char), unsigned long ulModeFlags);
    #define UNSECURE_MQTT_CONNECTION                 0
    #define SECURE_MQTT_CONNECTION               0x100
    #define MQTT_CONNECT_FLAG_CLEAN_SESSION       0x02                   // flag to request that a new session clears and previous session states
    #define MQTT_CONNECT_FLAG_WILL_FLAG           0x04                   // flag to signal that the "will QoS" and "will retain" fields will be used by the server
    #define MQTT_CONNECT_FLAG_WILL_QOS_MASK       0x18                   // the QoS level to be used when publishing the "will message" (must be set to 0 if the "will flag" is not set)
    #define MQTT_CONNECT_FLAG_WILL_RETAIN         0x20                   // flag to signal that the server must publish the "will message" as a retained message (not allowed if the "will flag" is not set)
    #define MQTT_CONNECT_FLAG_PASSWORD_FLAG       0x40                   // flag to indicate that a password must be present in the payload (not allowed if the user name flag is not set)
    #define MQTT_CONNECT_FLAG_USER_NAME_FLAG      0x80                   // flag to indicate that a user name must be present in the payload

extern int fnDisconnectMQTT(void);
extern int fnPublishMQTT(unsigned char ucTopicReference, CHAR *ptrTopic, signed char cQoS);
    #define MQTT_SUBSCRIPTION_QoS_0       0x00
    #define MQTT_SUBSCRIPTION_QoS_1       0x01
    #define MQTT_SUBSCRIPTION_QoS_2       0x02
extern int fnShowMQTT_subscription(int iRef);
extern int fnSubscribeMQTT(CHAR *ptrInput, unsigned char ucQoS);
extern int fnUnsubscribeMQTT(unsigned char ucSubscriptionRef);

#define ERROR_MQTT_NOT_READY             -1
#define ERROR_MQTT_IN_USE                -2
#define ERROR_MQTT_ARP_FAIL              -3
#define ERROR_MQTT_NO_SUBSCRIPTION_ENTRY -4
#define ERROR_MQTT_MISSING_TOPIC         -5
#define ERROR_MQTT_INVALID_SUBSCRIPTION  -6
#define MQTT_RESULT_OK                    0
#define MQTT_CLIENT_IDENTIFIER            1
#define MQTT_WILL_TOPIC                   2
#define MQTT_WILL_MESSAGE                 3
#define MQTT_USER_NAME                    4
#define MQTT_USER_PASSWORD                5
#define MQTT_CONNACK_RECEIVED             6
#define MQTT_SUBACK_RECEIVED              7
#define MQTT_UNSUBACK_RECEIVED            8
#define MQTT_PUBLISH_ACKNOWLEDGED         9
#define MQTT_PUBLISH_TOPIC                10
#define MQTT_PUBLISH_DATA                 11
#define MQTT_TOPIC_MESSAGE                12
#define MQTT_CONNECTION_CLOSED            13
#define MQTT_HOST_CLOSED                  14

extern int  fnCheckPass(CHAR *ucReference, CHAR *ucNewInput);
extern CHAR *fnWebStrcpy(CHAR *cStrOut, CHAR *cStrIn);

// Dynamic VLAN                                                          // {67}
//
extern void fnRemoveVLAN_tag(ETHERNET_FRAME *rx_frame);                  // takes an ethernet frame as input and removes its VLAN tag (if existing) and updates the VLAN details of the frame
extern void fnAddVLAN_tag(ETHERNET_FRAME *rx_frame);
extern int  fnVLAN_Accept(ETHERNET_FRAME *ptr_rx_frame);                 // user supplied routine which decides whether a particular VLAN tagged frame is accepted or not
extern int  fnVLAN_tag_tx(QUEUE_HANDLE channel);                         // user supplied routine which defines whether the transmission (on the channel in question) is VLAN tagged or not
#if defined SUPPORT_VLAN && defined SUPPORT_DYNAMIC_VLAN_TX
    extern QUEUE_TRANSFER fnFirstWrite(QUEUE_HANDLE driver_id, unsigned char *output_buffer, QUEUE_TRANSFER nr_of_bytes, unsigned char ucVLAN_content);
#else
    #define fnFirstWrite(handle, data, length, vlan_info) fnWrite(handle, data, length)
#endif

// IP utilities functions
//
extern CHAR *fnStrIP(CHAR *ptr_input, unsigned char *ucIP_address);
extern CHAR *fnIPStr(unsigned char *ptrIP, CHAR *cStr);
extern CHAR *fnSetMAC(CHAR *ptr_input, unsigned char *ptrMac);
extern CHAR *fnMACStr(unsigned char *ptrMAC, CHAR *cStr);

// IPV6 utility functions                                                // {34}
//
extern CHAR *fnIPV6Str(unsigned char *ptrIP, CHAR *cStr);
extern CHAR *fnStrIPV6(CHAR *ptr_input, unsigned char *ucIP_address);

extern unsigned char fnGetMimeType(CHAR *ptrFileName);
extern const CHAR   *cMimeTable[];

/* =================================================================== */
/*              Global Ethernet and TCP/IP variables                   */
/* =================================================================== */


extern const unsigned char cucNullMACIP[MAC_LENGTH];                     // system constant - 0x00 0x00 0x00 0x00 0x00 0x00
extern const unsigned char cucBroadcast[MAC_LENGTH];                     // system constant - 0xff 0xff 0xff 0xff 0xff 0xff


extern const CHAR    cSMTP_provider[];                                   // supplied and initialised by application
extern unsigned char ucSMTP_server[IPV4_LENGTH];                         // supplied and initialised by application

#if defined USE_IPV6
    extern unsigned char ucLinkLocalIPv6Address[IP_NETWORK_COUNT][IPV6_LENGTH];
#endif

// Web server support of IP parameter modification
//
#define ADD_IP_ADDRESS     0
#define ADD_MAC_ADDRESS    1
#define ADD_STATS          2
#define ADD_MEM            3


typedef struct _PACK stNETWORK_PARAMETERS
{
    unsigned short usNetworkOptions;                                     // options passed to the network device on configuration
    unsigned char  ucOurMAC[MAC_LENGTH];                                 // when no other value can be read from parameters this will be used
    unsigned char  ucOurIP[IPV4_LENGTH];                                 // our default IP address
    unsigned char  ucNetMask[IPV4_LENGTH];                               // our default network mask
    unsigned char  ucDefGW[IPV4_LENGTH];                                 // our default gateway
    unsigned char  ucDNS_server[IPV4_LENGTH];                            // our default DNS server
#if defined SECOND_DNS_SERVER                                            // {37}
    unsigned char  ucDNS_server_reserve[IPV4_LENGTH];                    // our fall-back DNS server
#endif
#if defined USE_IPV6                                                     // {29}
    unsigned char  ucOurIPV6[IPV6_LENGTH];                               // global IPV6 address
    #if defined USE_IPV6INV4
    unsigned char  ucTunnelIPV4[IPV4_LENGTH];                            // {56}
    #endif
#endif
} NETWORK_PARAMETERS;
__PACK_OFF

#define OFFSET_OUR_IP (sizeof(unsigned short) + MAC_LENGTH)

extern NETWORK_PARAMETERS network[IP_NETWORK_COUNT];                     // {61}

#if defined SUPPORT_VLAN
    extern int vlan_active;                                              // these variables are supplied by the user
    #if defined SUPPORT_DYNAMIC_VLAN && (defined ALTERNATIVE_VLAN_COUNT && ALTERNATIVE_VLAN_COUNT > 0)     // {69}
        extern unsigned short vlan_vid[1 + ALTERNATIVE_VLAN_COUNT];
    #else
        extern unsigned short vlan_vid;
    #endif
#endif

extern void fnGetEthernetPars(void);                                     // the application will tend to have to assure this is available since it knows where the parameters are located
extern void fnBridgeEthernetFrame(ETHERNET_FRAME *ptr_rx_frame);         // when bridging Ethernet reception to other interfaces the user must supply this function

#if IP_INTERFACE_COUNT > 1
    extern void fnEnterInterfaceHandle(unsigned char ucInterface, QUEUE_HANDLE interfaceHandle, unsigned char ucCharacteristics);
    extern QUEUE_HANDLE fnGetInterfaceHandle(int iReference);
    extern int fnGetFirstInterface(USOCKET cSocket);
    extern unsigned char fnGetInterfaceCharacteristics(int iReference);
    extern int fnGetAnyInterfaceCharacteristics(USOCKET uSocket, unsigned char ucCharacteristic);
    extern unsigned char fnGetInterfaceFlag(QUEUE_HANDLE interfaceHandle, int iCharacteristics);
#endif

#endif
