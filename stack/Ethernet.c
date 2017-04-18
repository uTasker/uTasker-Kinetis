/**********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher

    ---------------------------------------------------------------------
    File:      Ethernet.c
    Project:   Single Chip Embedded Internet
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2016
    *********************************************************************
    01.03.2007 EthernetStats array made conditional on USE_IP_STATS      {1}
    14.03.2007 Added RARP reception support                              {2}
    15.03.2007 Added VLAN support                                        {3}
    09.06.2008 Allow variable uNetwork protocol                          {4}
    10.01.2010 Add IPV6                                                  {5}
    27.08.2010 Limit to optionally handling only one reception at a time {6}
    06.09.2010 Add rx offload support                                    {7}
    25.08.2011 Modify optional handling of one reception frame at a time to operate without queued events and control
               maximum number of frames handled each pass                {8}
    10.11.2011 Quit initialisation if the Ethernet configuration fails   {9}
    11.03.2012 Modified to handle protocols carried by IPv6              {10}
    25.03.2012 Generate IPv6 link-local address from MAC-48              {11}
    16.04.2012 Add optional bridging call                                {12}
    07.10.2012 Return 0 from fnGetEthernetStats() if the parameter is invalid {13}
    16.12.2012 Add fnRemoveVLAN_tag() and fnAddVLAN_tag()                {14}
    16.12.2012 Add dynamic VLAN support                                  {15}
    17.12.2012 Changed fnHandleUDP() interface to allow IPv6 and multi-homed operation {16}
    06.08.2013 Add SUPPORT_MULTICAST_RX support (in promiscuous bridging mode) {17}
    16.04.2014 Extend IGMP counters and IP statistics to multiple networks {18}
    05.12.2015 Add ETHERNET_FILTERED_BRIDGING support                    {19}

*/


/* =================================================================== */
/*                           include files                             */
/* =================================================================== */

#include "config.h"

#if defined ETH_INTERFACE || defined USB_CDC_RNDIS

#if defined _WINDOWS
    extern void fnOpenDefaultHostAdapter(void);
#endif

/* =================================================================== */
/*                          local definitions                          */
/* =================================================================== */

#define OWN_TASK               TASK_ETHERNET

#if IP_NETWORK_COUNT > 1
    #define _NETWORK_ID        ucNetworkID
#else
    #define _NETWORK_ID        DEFAULT_NETWORK 
#endif
#if !defined ETHERNET_IP_INTERFACE
    #define ETHERNET_IP_INTERFACE (DEFAULT_IP_INTERFACE)
#endif


/* =================================================================== */
/*                      local structure definitions                    */
/* =================================================================== */

/* =================================================================== */
/*                 local function prototype declarations               */
/* =================================================================== */

#if defined USE_IP
    static void fnHandleIP_protocol(unsigned char ucNextProtocol, ETHERNET_FRAME *ptrRx_frame);
#endif
#if defined USE_IPV6
    static void fnHandleIPv6_protocol(ETHERNET_FRAME *ptrFrame);
#endif
#if defined ENC424J600_INTERFACE && (!defined ETHERNET_AVAILABLE || defined NO_INTERNAL_ETHERNET || ETHERNET_INTERFACES > 1) && !defined REMOTE_SIMULATION_INTERFACE
    static CHAR fnEthernetMuxEvent(ETHERNET_FRAME *ptrFrame, int *ptr_iEthernetInterface);
    static int fnConfigENC424J600(ETHTABLE *pars);
    static int fnGetQuantityENC424J600RxBuf(void);
    static unsigned char *fnGetENC424J600TxBufferAdd(int iBufNr);
    static int fnWaitENC424J600TxFree(void);
    static void fnPutInBuffer_ENC424J600(unsigned char *ptrOut, unsigned char *ptrIn, QUEUE_TRANSFER nr_of_bytes);
    static QUEUE_TRANSFER fnStartEthTxENC424J600(QUEUE_TRANSFER DataLen, unsigned char *ptr_put);
    static void fnFreeEthernetBufferENC424J600(int iBufNr);
        #if defined USE_IGMP
    static void fnModifyMulticastFilterENC424J600(QUEUE_TRANSFER action, unsigned char *ptrIP);
        #endif
#endif


/* =================================================================== */
/*                             constants                               */
/* =================================================================== */

#if defined ETHERNET_RELEASE_AFTER_EVERY_FRAME                           // {8}
    static const unsigned char ucEMAC_RX_INTERRUPT = EMAC_RX_INTERRUPT;
#endif
#if !defined ETHERNET_AVAILABLE || defined NO_INTERNAL_ETHERNET || (IP_INTERFACE_COUNT > 1)
    #if defined ETH_INTERFACE && defined ETHERNET_AVAILABLE && !defined NO_INTERNAL_ETHERNET
    static const ETHERNET_FUNCTIONS InternalEthernetFunctions = {
        fnConfigEthernet,                                                // configuration function for this interface
        fnGetQuantityRxBuf,
        fnGetTxBufferAdd,
        fnWaitTxFree,
        fnPutInBuffer,
        fnStartEthTx,
        fnFreeEthernetBuffer,
        #if defined USE_IGMP
        fnModifyMulticastFilter,
        #endif
    };
    #endif
    #if defined ENC424J600_INTERFACE && !defined REMOTE_SIMULATION_INTERFACE
    static const ETHERNET_FUNCTIONS ENC424J600EthernetFunctions = {
        fnConfigENC424J600,                                              // configuration function for this interface
        fnGetQuantityENC424J600RxBuf,
        fnGetENC424J600TxBufferAdd,
        fnWaitENC424J600TxFree,
        fnPutInBuffer_ENC424J600,
        fnStartEthTxENC424J600,
        fnFreeEthernetBufferENC424J600,
        #if defined USE_IGMP
        fnModifyMulticastFilterENC424J600,
        #endif
    };
    #endif
#endif
/* =================================================================== */
/*                     global variable definitions                     */
/* =================================================================== */

QUEUE_HANDLE Ethernet_handle[ETHERNET_INTERFACES] = {NO_ID_ALLOCATED};

/* =================================================================== */
/*                      local variable definitions                     */
/* =================================================================== */

#if defined USE_IP_STATS                                                 // {1}
    static unsigned long EthernetStats[IP_NETWORK_COUNT][sizeof(ETHERNET_STATS)/sizeof(unsigned long)] = {{0}}; // {18}
#endif
#if defined ENC424J600_INTERFACE
    static int iENC424J600_frames = 0;
#endif

#if defined ETH_INTERFACE
// Ethernet task
//
extern void fnTaskEthernet(TTASKTABLE *ptrTaskTable)
{
#if !defined ETHERNET_RELEASE_AFTER_EVERY_FRAME                          // {8} no queue used
    QUEUE_HANDLE PortIDInternal = ptrTaskTable->TaskID;                  // queue ID for task input
    unsigned char ucInputMessage[MEDIUM_MESSAGE];                        // reserve space for receiving messages
#elif defined ETHERNET_RELEASE_LIMIT
    int iMaxRxFrames = ETHERNET_RELEASE_LIMIT;                           // allow the task to handle up to this many reception frames before yielding
#endif
    int iEthernetInterface = 0;
    ETHERNET_FRAME rx_frame;
    signed char cEthernetBuffer;

    if (Ethernet_handle[ETHERNET_IP_INTERFACE] == NO_ID_ALLOCATED) {
        ETHTABLE ethernet;                                               // configuration structure to be passed to the Ethernet configuration
#if (defined ETHERNET_AVAILABLE && !defined NO_INTERNAL_ETHERNET && (ETHERNET_INTERFACES > 1)) || defined USB_CDC_RNDIS
        ethernet.ptrEthernetFunctions = (void *)&InternalEthernetFunctions; // enter the Ethernet function list for the defult internal controller
#endif
#if defined REMOTE_SIMULATION_INTERFACE                                  // when being uses as siulation extensin node
        if (RemoteSimUDPSocket < 0) {                                    // if the USD socket has not yet been configured
            RemoteSimUDPSocket = fnGetUDP_socket(TOS_MINIMISE_DELAY, fnRemoteSimUDP_Listner, (UDP_OPT_SEND_CS | UDP_OPT_CHECK_CS));
            fnBindSocket(RemoteSimUDPSocket, REMOTE_SIM_UDP_PORT);       // bind socket and listen
            CONFIGURE_ENC424J600_SPI_MODE();                             // prepare the SPI for use
            fnConfigureENC424J600Interrupt();                            // enable interrupts
        }
#endif
        fnGetEthernetPars();                                             // we get the network parameters for the storage system before configuring
        ethernet.Task_to_wake  = OWN_TASK;                               // when there are Ethernet receptions this task will handle them
        ethernet.Channel       = 0;                                      // default channel number
        ethernet.usMode        = network[DEFAULT_NETWORK].usNetworkOptions; // options to be used by the interface
#if defined USE_IPV6                                                     // {5}{11} generate an IPv6 link-local address from the MAC address
        ucLinkLocalIPv6Address[DEFAULT_NETWORK][0]  = 0xfe;              // link-local unicast
        ucLinkLocalIPv6Address[DEFAULT_NETWORK][1]  = 0x80;              // link-local unicast
                                                                         // intermediate values left at 0x00
        ucLinkLocalIPv6Address[DEFAULT_NETWORK][8]  = (network[DEFAULT_NETWORK].ucOurMAC[0] | 0x2); // invert the universal/local bit (since it is always '0' it means setting to '1')
        ucLinkLocalIPv6Address[DEFAULT_NETWORK][9]  = network[DEFAULT_NETWORK].ucOurMAC[1];
        ucLinkLocalIPv6Address[DEFAULT_NETWORK][10] = network[DEFAULT_NETWORK].ucOurMAC[2];
        ucLinkLocalIPv6Address[DEFAULT_NETWORK][11] = 0xff;              // insert standard 16 bit value to extend MAC-48 to EUI-64
        ucLinkLocalIPv6Address[DEFAULT_NETWORK][12] = 0xfe;
        ucLinkLocalIPv6Address[DEFAULT_NETWORK][13] = network[DEFAULT_NETWORK].ucOurMAC[3];
        ucLinkLocalIPv6Address[DEFAULT_NETWORK][14] = network[DEFAULT_NETWORK].ucOurMAC[4];
        ucLinkLocalIPv6Address[DEFAULT_NETWORK][15] = network[DEFAULT_NETWORK].ucOurMAC[5];
        ethernet.usMode |= CON_MULTICAST;                                // enable multicast when using IPV6
#else
        uMemcpy(ethernet.ucMAC, &network[DEFAULT_NETWORK].ucOurMAC[0], MAC_LENGTH); // the MAC address to be used by the interface (if not overridden to use a factory programmed one)
#endif
#if defined ETHERNET_BRIDGING                                            // {12}
        ethernet.usMode        |= (PROMISCUOUS);                         // Ethernet bridging requires promiscuous operation
#endif
        ethernet.usSizeRx      = LAN_BUFFER_SIZE;                        // receive buffer size requested by user
        ethernet.usSizeTx      = LAN_BUFFER_SIZE;                        // transmit buffer size requested by user
        ethernet.ucEthTypes    = (ARP | IPV4);                           // enable reception of these protocols (used only by NE64 controller)
        ethernet.usExtEthTypes = 0;                                      // specify extended frame types (only used by NE64 controller)
#if defined ETHERNET_AVAILABLE && !defined NO_INTERNAL_ETHERNET          // default internal controller
        Ethernet_handle[ETHERNET_IP_INTERFACE] = fnOpen(TYPE_ETHERNET, FOR_I_O, &ethernet); // open the channel with defined configuration
        if (Ethernet_handle[ETHERNET_IP_INTERFACE] == NO_ID_ALLOCATED) { // {9} if the hardware configuration failed
            return;                                                      // stop any further activity - the developer can catch this here and identify the cause in the hardware-specific initialisation (usually due to PHY not being detected)
        }
#endif
#if !defined ETHERNET_AVAILABLE || defined NO_INTERNAL_ETHERNET || (IP_INTERFACE_COUNT > 1)
    #if defined ETHERNET_AVAILABLE && !defined NO_INTERNAL_ETHERNET
        #define ENC424J600_INTERFACE_REFERENCE 1
        #if IP_INTERFACE_COUNT > 1
        fnEnterInterfaceHandle(ETHERNET_IP_INTERFACE, Ethernet_handle[ETHERNET_IP_INTERFACE], DEFAULT_INTERFACE_CHARACTERISTICS); // enter the local controller as interface handler
        #endif
    #else
        #define ENC424J600_INTERFACE_REFERENCE 0                         // external Ethernet controller is the default one
    #endif
    #if defined ENC424J600_INTERFACE && (!defined ETHERNET_AVAILABLE || defined NO_INTERNAL_ETHERNET || (IP_INTERFACE_COUNT > 1)) && !defined REMOTE_SIMULATION_INTERFACE
        ethernet.ptrEthernetFunctions = (void *)&ENC424J600EthernetFunctions;
        ethernet.Channel = ENC424J600_INTERFACE_REFERENCE;
        #if defined NO_INTERNAL_ETHERNET || defined DEVICE_WITHOUT_ETHERNET
        ethernet.usMode |= USE_FACTORY_MAC;                              // use the factory programmed MAC address from the Ethernet controller
        #endif
        Ethernet_handle[ENC424J600_INTERFACE_REFERENCE] = fnOpen(TYPE_ETHERNET, FOR_I_O, &ethernet);
        #if IP_INTERFACE_COUNT > 1
        fnEnterInterfaceHandle(ENC424J00_IP_INTERFACE, Ethernet_handle[ENC424J600_INTERFACE_REFERENCE], (INTERFACE_NO_RX_CS_OFFLOADING | INTERFACE_NO_TX_CS_OFFLOADING | INTERFACE_NO_TX_PAYLOAD_CS_OFFLOADING));
        #else
            #if !defined USE_IPV6
        uMemcpy(&network[DEFAULT_NETWORK].ucOurMAC[0], ethernet.ucMAC, MAC_LENGTH);
            #endif
        network[DEFAULT_NETWORK].usNetworkOptions |= NETWORK_VALUES_FIXED; // fix the values so that the MAC address doesn't get overwritten by reloading parameters
        #endif
    #endif
#endif
#if defined _WINDOWS
    fnOpenDefaultHostAdapter();
#endif
    }
#if !defined ETHERNET_RELEASE_AFTER_EVERY_FRAME                          // {8}
    while (fnRead(PortIDInternal, ucInputMessage, HEADER_LENGTH)) {      // check input queue
        switch (ucInputMessage[MSG_SOURCE_TASK]) {
        case INTERRUPT_EVENT:
            while ((cEthernetBuffer = fnEthernetEvent(&ucInputMessage[MSG_INTERRUPT_EVENT], &rx_frame)) >= 0)
#elif defined ENC424J600_INTERFACE && (!defined ETHERNET_AVAILABLE || defined NO_INTERNAL_ETHERNET || ETHERNET_INTERFACES > 1) && !defined REMOTE_SIMULATION_INTERFACE
            while ((cEthernetBuffer = fnEthernetMuxEvent(&rx_frame, &iEthernetInterface)) >= 0) // check for reception on multiple interface types
#else
            while ((cEthernetBuffer = fnEthernetEvent((unsigned char *)&ucEMAC_RX_INTERRUPT, &rx_frame)) >= 0) // check the Ethernet interface for frame reception
#endif
            {
                fnHandleEthernetFrame(&rx_frame, Ethernet_handle[iEthernetInterface]);
                fnFreeBuffer(Ethernet_handle[iEthernetInterface], cEthernetBuffer); // free up the reception buffer since we have completed working with it
#if defined ETHERNET_RELEASE_AFTER_EVERY_FRAME                           // {6}
    #if defined ETHERNET_RELEASE_LIMIT                                   // {8}
                if (--iMaxRxFrames >= 0) {                               // if the task has not yet handled the maximum number of frames allow it to continue running
                    continue;                                            // processes next possibly waiting reception
                }
    #endif
                uTaskerStateChange(OWN_TASK, UTASKER_ACTIVATE);          // schedule the task to check the input once more after allowing other tasks to run
                return;                                                  // quit for the moment
#endif
            }                                                            // end while frames waiting
#if !defined ETHERNET_RELEASE_AFTER_EVERY_FRAME                          // {8}
            break;

        default:
            break;
        }                                                                // end switch
    }                                                                    // end while input messages queued
#endif
}
#endif

extern int fnHandleEthernetFrame(ETHERNET_FRAME *ptr_rx_frame, QUEUE_HANDLE interface_handle)
{
#if defined ETHERNET_BRIDGING                                            // {12}
    int iBridgeFrame;                                                    // flag for bridging option
#endif
#if defined IP_RX_CHECKSUM_OFFLOAD                                       // {7} the reception has flagged a corrupted frame
    if (ptr_rx_frame->frame_size == 0) {                                 // reception is invalid IPv4/v6 frame so discard
    #if defined ETHERNET_BRIDGING
        iBridgeFrame = 1;                                                // allow bridging corrupted frames in case they are a special protocol
    #endif
        goto _bridge_ethernet_buffer;
    }
#endif
#if IP_INTERFACE_COUNT > 1
    ptr_rx_frame->Tx_handle = interface_handle;                          // mark the interface handle to be used when responding directly
#endif
#if defined USE_IP || defined USE_IPV6
    #if defined SUPPORT_VLAN
        #if defined SUPPORT_DYNAMIC_VLAN                                 // {15} dynamic VLAN on reception
    if ((ptr_rx_frame->ptEth->ethernet_frame_type[0] == (unsigned char)(TAG_PROTOCOL_IDENTIFIER >> 8)) && // check for VLAN tag
        (ptr_rx_frame->ptEth->ethernet_frame_type[1] == (unsigned char)(TAG_PROTOCOL_IDENTIFIER))) {
        ptr_rx_frame->ucVLAN_content = (VLAN_TAGGED_FRAME | VLAN_CONTENT_PRESENT); // flag that the frame is associated to a VLAN but has had the content removed
        fnRemoveVLAN_tag(ptr_rx_frame);                                  // remove the tag from the frame content
    }
    else {                                                               // no VLAN tag
        ptr_rx_frame->ucVLAN_content = 0;                                // frame is not VLAN tagged
        ptr_rx_frame->usVLAN_ID = 0xffff;                                // mark no VLAN
    }
        #else                                                            // standard VLAN operation on a single fixed VLAN
    if (vlan_active) {                                                   // if VLAN mode active, accept only frames to our identifier [vlan_active and vlan_vid are supplied by the user]
        if ((ptr_rx_frame->ptEth->ethernet_frame_type[0] != (unsigned char)(TAG_PROTOCOL_IDENTIFIER  >> 8)) 
            || (ptr_rx_frame->ptEth->ethernet_frame_type[1] != (unsigned char)(TAG_PROTOCOL_IDENTIFIER))
            || ((ptr_rx_frame->ptEth->ucData[0] & 0x0f) != (unsigned char)((vlan_vid >> 8) & 0x0f)) 
            || (ptr_rx_frame->ptEth->ucData[1] != (unsigned char)vlan_vid)) {
            #if defined ETHERNET_BRIDGING
            iBridgeFrame = 1;                                            // allow bridging of VLAN tagged frames that don't match our vlan
            #endif
            goto _bridge_ethernet_buffer;
        }
        else {
            unsigned char *ptrTo = (unsigned char *)ptr_rx_frame->ptEth + (2 * MAC_LENGTH + (VLAN_TAG_LENGTH - 1));
            unsigned char *ptrFrom = (unsigned char *)ptr_rx_frame->ptEth + (2 * MAC_LENGTH - 1);
            while (ptrFrom >= (unsigned char *)ptr_rx_frame->ptEth) {
                *ptrTo-- = *ptrFrom--;                                   // shift the header to overwrite the VLAN tag, resulting in filtered frame
            }
            ptr_rx_frame->ptEth = (ETHERNET_FRAME_CONTENT *)((unsigned char *)ptr_rx_frame->ptEth + VLAN_TAG_LENGTH);
            ptr_rx_frame->frame_size -= VLAN_TAG_LENGTH;                 // {15}
        }
    }
        #endif
    #endif
#endif
#if defined ETHERNET_BRIDGING                                            // {12}
    if (uMemcmp(ptr_rx_frame->ptEth->ethernet_destination_MAC, cucBroadcast, MAC_LENGTH) == 0) { // if the frame is a broadcast
        iBridgeFrame = 1;                                               // broadcast frames must be passed on after processing by us
    }
    #if defined SUPPORT_MULTICAST_RX                                     // {17}
    else if ((ptr_rx_frame->ptEth->ethernet_destination_MAC[0] & 0x01) && (fnMulticastMemberMAC(ptr_rx_frame->ptEth->ethernet_destination_MAC) != 0)) {
        iBridgeFrame = 1;                                                // multicast frames must be passed on after processing by us
    }
    #endif
    else if (uMemcmp(ptr_rx_frame->ptEth->ethernet_destination_MAC, network[DEFAULT_NETWORK].ucOurMAC, MAC_LENGTH) != 0) {
        iBridgeFrame = 1;                                                // other frames not addressed to us are passed on
        goto _bridge_ethernet_buffer;                                    // don't handle locally but allow bridging
    }
    else {
        iBridgeFrame = 0;                                                // this is a frame to use so don't pass it on
    }
#endif
#if defined USE_IP
    #if defined SUPPORT_VLAN && defined SUPPORT_DYNAMIC_VLAN             // {15}
    if (fnVLAN_Accept(ptr_rx_frame) == 0) {                              // check whether the received frame may be handled locally [this routine must be supplied by the user]
        goto _bridge_ethernet_buffer;                                    // don't handle locally but allow bridging
    }
    #endif
    #if defined USE_IPV6                                                 // {5}
    if ((ptr_rx_frame->ptEth->ethernet_frame_type[1] == (unsigned char)(PROTOCOL_IPv6)) && (ptr_rx_frame->ptEth->ethernet_frame_type[0] == (unsigned char)(PROTOCOL_IPv6 >> 8))) { // check for IPV6
        fnHandleIPv6_protocol(ptr_rx_frame);                             // handle pure IPv6 frame content
        goto _bridge_ethernet_buffer;                                    // don't handle locally but allow bridging
    }
    #endif
    // Note that we check whether it is probably an ARP message by looking only at the second byte in the protocol field
    // This is for efficiency - only call ARP routine when it is probable that it is an ARP (the ARP routine then only checks first byte in type field
    //
    if (ptr_rx_frame->ptEth->ethernet_frame_type[0] == (unsigned char)(PROTOCOL_IPv4 >> 8)) { // if IPv4 type
        if ((ptr_rx_frame->ptEth->ethernet_frame_type[1] != (unsigned char)PROTOCOL_ARP)
    #if defined USE_RARP                                                 // {2}
            && (ptr_rx_frame->ptEth->ethernet_frame_type[1] != (unsigned char)PROTOCOL_RARP) 
    #endif
            || (fnProcessARP(ptr_rx_frame) == 0)) {                      // if (R)ARP, handle it
            if (fnHandleIPv4(ptr_rx_frame) != 0) {                       // if IPv4 handle it
    #if defined USE_IPV6
                ptr_rx_frame->ucIPV6_Protocol = 0;                       // {10} not IPv6
                ptr_rx_frame->ucIPV4_Protocol = ptr_rx_frame->ptEth->ucData[IPV4_PROTOCOL_OFFSET]; // IPv4 protocol
    #endif
                fnHandleIP_protocol(ptr_rx_frame->ptEth->ucData[IPV4_PROTOCOL_OFFSET], ptr_rx_frame); // {10} handle the next protocol layer (above IPv4)
            }
        }                                                                // end if not ARP
    }                                                                    // end if PROTOCOL IP V4
    #if defined SUPPORT_DISTRIBUTED_NODES
        #if defined VARIABLE_PROTOCOL_UNETWORK                           // {4}
    else if ((ptr_rx_frame->ptEth->ethernet_frame_type[0] == uc_uNetworkProtocol[0]) && (ptr_rx_frame->ptEth->ethernet_frame_type[1] == uc_uNetworkProtocol[1])) {
        fnHandle_unet(ptr_rx_frame->ptEth->ucData);
            #if defined ETHERNET_BRIDGING
        return 0;                                                        // never bridge uNetwork frames
            #endif
    }
        #else
    else if ((ptr_rx_frame->ptEth->ethernet_frame_type[0] == (unsigned char)(PROTOCOL_UNETWORK>>8)) && (ptr_rx_frame->ptEth->ethernet_frame_type[1] == (unsigned char)(PROTOCOL_UNETWORK))) {
        fnHandle_unet(ptr_rx_frame->ptEth->ucData);
            #if defined ETHERNET_BRIDGING
        return 0;                                                        // never bridge uNetwork frames
            #endif
    }
        #endif
    #endif
#elif defined SUPPORT_DISTRIBUTED_NODES
    #if defined VARIABLE_PROTOCOL_UNETWORK                               // {4}
    if ((ptr_rx_frame->ptEth->ethernet_frame_type[0] == uc_uNetworkProtocol[0]) && (ptr_rx_frame->ptEth->ethernet_frame_type[1] == uc_uNetworkProtocol[1])) {
        fnHandle_unet(ptr_rx_frame->ptEth->ucData);
            #if defined ETHERNET_BRIDGING
        return 0;                                                        // never bridge uNetwork frames
            #endif
    }
    #else
    if ((ptr_rx_frame->ptEth->ethernet_frame_type[0] == (unsigned char)(PROTOCOL_UNETWORK>>8)) && (ptr_rx_frame->ptEth->ethernet_frame_type[1] == (unsigned char)(PROTOCOL_UNETWORK))) {
         fnHandle_unet(ptr_rx_frame->ptEth->ucData);
            #if defined ETHERNET_BRIDGING
        return 0;                                                        // never bridge uNetwork frames
            #endif
    }
    #endif
#endif
#if defined ETHERNET_BRIDGING || defined IP_RX_CHECKSUM_OFFLOAD || defined SUPPORT_VLAN || defined USE_IPV6
_bridge_ethernet_buffer:
#endif
#if defined ETHERNET_BRIDGING                                            // {12}
    if (iBridgeFrame != 0) {                                             // if Ethernet frame is to be bridged
        fnBridgeEthernetFrame(ptr_rx_frame);                             // the user layer must supply this and can copy the data content to other interfaces
        return 1;                                                        // the frame has been bridged
    }
    return 0;                                                            // frame was not bridged
#elif defined ETHERNET_FILTERED_BRIDGING                                 // {19}
    fnBridgeEthernetFrame(ptr_rx_frame);                                 // the user layer must supply this and can copy the data content to other interfaces
    return 1;                                                            // frame was bridged
#else
    return 0;                                                            // frame was not bridged
#endif
}

#if defined USE_IP
static void fnHandleIP_protocol(unsigned char ucNextProtocol, ETHERNET_FRAME *ptrRx_frame)
{
    switch (ucNextProtocol) {
    #if defined USE_ICMP
    case IP_ICMP:
        fnHandleICMP(ptrRx_frame);                                       // handle ICMP reception
        break;
    #endif
    #if defined USE_IPV6                                                 // {5}
    case IP_ICMPV6:
        fnHandleICMPV6(ptrRx_frame);
        break;
        #if defined USE_IPV6INV4
    case IP_6IN4:                                                        // IPv4 is being used to tunnel IPv6 content
        if (fnHandleIPV6in4(ptrRx_frame, ptrRx_frame->usIPLength) != 0) {
            fnHandleIPv6_protocol(ptrRx_frame);                          // tunneled IPv6 content is for the global IPv6 address
        }
        break;
    #    endif
    #endif
    #if defined USE_UDP
    case IP_UDP:
        fnHandleUDP(ptrRx_frame);                                        // {16}
        break;
    #endif
    #if defined USE_TCP
    case IP_TCP:
        fnHandleTCP(ptrRx_frame);                                        // {10}
        break;
    #endif
    #if defined USE_IGMP
    case IP_IGMPV2:
        fnHandleIGMP(ptrRx_frame);
        break;
    #endif
    default:
        break;                                                           // unsupported protcol is silently ignored
    }
}
#endif

#if defined USE_IPV6
static void fnHandleIPv6_protocol(ETHERNET_FRAME *ptrFrame)
{
    #if IP_NETWORK_COUNT > 1
    register unsigned char ucNetworkID;
    #endif
    IP_PACKET_V6 *ipv6 = (IP_PACKET_V6 *)ptrFrame->ptEth->ucData;
    ptrFrame->ucIPV4_Protocol = 0;                                       // not IPv4
    ptrFrame->ucIPV6_Protocol = ipv6->next_header;
    #if IP_NETWORK_COUNT > 1
    ptrFrame->ucBroadcastResponse = 0;                                   // default is no response to broadcasts - the user must decide the behaviour
    ucNetworkID = fnAssignNetwork(ptrFrame, ipv6->destination_IP_address);
    #endif
    #if defined USE_IP_STATS
    fnRxStats((unsigned char)(ptrFrame->ucIPV6_Protocol | IPV6_FRAME), _NETWORK_ID); // count IPv6 receptions
    #endif
    ptrFrame->usDataLength = ((ipv6->payload_length[0] << 8) | ipv6->payload_length[1]);
    ptrFrame->usIPLength = 40;                                           // payload located at this offset
    fnHandleIP_protocol(ptrFrame->ucIPV6_Protocol, ptrFrame);            // {10} handle the next protocol layer above IPv6
}
#endif

static unsigned char ucEthernetState[IP_NETWORK_COUNT] = {0};
extern void fnEthernetStateChange(int iInterface, unsigned char ucEvent)
{
    fnDebugMsg("Eth");
    fnDebugDec(iInterface, 0);
    fnDebugMsg(" link-");
    ucEthernetState[iInterface] = ucEvent;
    if (ucEvent == LAN_LINK_DOWN) {
        fnDebugMsg("down\r\n");
        return;
    }
    fnDebugMsg("up 10");
    if ((ucEvent == LAN_LINK_UP_100_FD) || (ucEvent == LAN_LINK_UP_100)) {
        fnDebugMsg("0");
    }
    if ((ucEvent == LAN_LINK_UP_100_FD) || (ucEvent == LAN_LINK_UP_10_FD)) {
        fnDebugMsg(" full");
    }
    else {
        fnDebugMsg(" half");
    }
    fnDebugMsg("-duplex\r\n");
}

extern unsigned char fnGetLinkState(int iInterface)
{
    return ucEthernetState[iInterface];
}

#if defined USE_IP_STATS
extern void fnIncrementEthernetStats(unsigned char ucStat, int iNetworkReference) // {18}
{
    EthernetStats[iNetworkReference][ucStat]++;
    if (ucStat <= LAST_RX_COUNTER) {
        EthernetStats[iNetworkReference][TOTAL_RX_FRAMES]++;
    }
    else if (ucStat <= LAST_TX_COUNTER) {
        EthernetStats[iNetworkReference][TOTAL_TX_FRAMES]++;
    }
}

extern unsigned long fnGetEthernetStats(unsigned char ucStat, int iNetworkReference) // {18}
{
    if (ucStat >= sizeof(ETHERNET_STATS)/sizeof(unsigned long)) {        // {13}
        return 0;
    }
    return EthernetStats[iNetworkReference][ucStat];                     // return the specified value
}

extern void fnTxStats(unsigned char ucProtType, int iNetworkReference)   // {18}
{
    unsigned char ucCounter;
    switch (ucProtType) {                                                // set counter according to protocol being sent
    case IP_UDP:
        ucCounter = SENT_UDP_FRAMES;
        break;

    case IP_TCP:
        ucCounter = SENT_TCP_FRAMES;
        break;
    #if defined USE_IGMP                                                 // {18}
    case IP_IGMPV2:
        ucCounter = SENT_IGMP_FRAMES;
        break;
    #endif
    case IP_ICMP:
        ucCounter = SENT_ICMP_FRAMES;
        break;
    #if defined USE_IPV6
    case IP_ICMPV6:
        ucCounter = TRANSMITTED_ICMPV6_FRAMES;
        break;
    case TRANSMITTED_TCPV6_FRAMES:
        ucCounter = ucProtType;
        break;
    #endif
    default:
        EthernetStats[iNetworkReference][TOTAL_TX_FRAMES]++;             // we have sent a frame with a protocol which is not specifically counted
        return;
    }
    fnIncrementEthernetStats(ucCounter, iNetworkReference);
}

extern void fnRxStats(unsigned char ucProtType, int iNetworkReference)   // {18}
{
    unsigned char ucCounter;
    switch (ucProtType & ~(FOREIGN_FRAME | IPV6_FRAME)) {                // set counter according to protocol being received
    case IP_UDP:
    #if defined USE_IPV6
        if (ucProtType & IPV6_FRAME) {
            ucCounter = RECEIVED_UDPV6_FRAMES;
        }
        else {
            ucCounter = RECEIVED_UDP_FRAMES;
        }
    #else
        ucCounter = RECEIVED_UDP_FRAMES;
    #endif
        break;
    case IP_TCP:
    #if defined USE_IPV6
        if (ucProtType & IPV6_FRAME) {
            ucCounter = RECEIVED_TCPV6_FRAMES;
        }
        else {
            ucCounter = RECEIVED_TCP_FRAMES;
        }
    #else
        ucCounter = RECEIVED_TCP_FRAMES;
    #endif
        break;
    #if defined USE_IGMP                                                 // {18}
    case IP_IGMPV2:
        ucCounter = RECEIVED_IGMP_FRAMES;
        break;
    #endif
    case IP_ICMP:
        ucCounter = RECEIVED_ICMP_FRAMES;
        break;
    #if defined USE_IPV6
    case IP_ICMPV6:
        ucCounter = RECEIVED_ICMPV6_FRAMES;
        break;
    #endif
    default:
        EthernetStats[iNetworkReference][UNSUPPORTED_PROTOCOL_FRAMES]++; // we have received a frame with a protocol which is not specifically supported
        EthernetStats[iNetworkReference][TOTAL_RX_FRAMES]++;
        return;
    }
    if (ucProtType & FOREIGN_FRAME) {
        ucCounter += (SEEN_FOREIGN_ICMP_FRAMES - RECEIVED_ICMP_FRAMES);  // set index to foreign counters
    }
    fnIncrementEthernetStats(ucCounter, iNetworkReference);
}

// Reset all IP counters
//
extern void fnDeleteEthernetStats(int iNetworkReference)
{
    uMemset(&EthernetStats[iNetworkReference], 0, sizeof(EthernetStats[iNetworkReference]));
}
#endif

#if defined SUPPORT_VLAN && defined SUPPORT_DYNAMIC_VLAN                 // {14}
// Remove the VLAN content from a VLAN frame
//
extern void fnRemoveVLAN_tag(ETHERNET_FRAME *rx_frame)
{
    if ((rx_frame->ucVLAN_content & (VLAN_TAGGED_FRAME | VLAN_CONTENT_PRESENT)) == (VLAN_TAGGED_FRAME | VLAN_CONTENT_PRESENT)) { // only treat VLAN frames with content
        unsigned char *ptrTo = (unsigned char *)rx_frame->ptEth + (2 * MAC_LENGTH + (VLAN_TAG_LENGTH - 1));
        unsigned char *ptrFrom = (unsigned char *)rx_frame->ptEth + (2 * MAC_LENGTH - 1);
        rx_frame->usVLAN_ID = ((rx_frame->ptEth->ucData[0] & 0x0f) << 8);
        rx_frame->usVLAN_ID |= (rx_frame->ptEth->ucData[1]);             // note the VLAN ID of the reception frame
        while (ptrFrom >= (unsigned char *)rx_frame->ptEth) {
            *ptrTo-- = *ptrFrom--;                                       // shift the header to overwrite the VLAN tag, resulting in filtered frame
        }
        rx_frame->ptEth = (ETHERNET_FRAME_CONTENT *)((unsigned char *)rx_frame->ptEth + VLAN_TAG_LENGTH);
        rx_frame->frame_size -= VLAN_TAG_LENGTH;
        rx_frame->ucVLAN_content &= ~(VLAN_CONTENT_PRESENT);             // flag that the VLAN content has been removed
    }
}

// Insert the VLAN content in to a VLAN frame with removed content (beware that the buffer used by rx_frame->ptEth must allow data to be inserted VLAN_TAG_LENGTH before its initial value!!)
//
extern void fnAddVLAN_tag(ETHERNET_FRAME *rx_frame)                      // {14}
{
    if ((rx_frame->ucVLAN_content & (VLAN_TAGGED_FRAME | VLAN_CONTENT_PRESENT)) == (VLAN_TAGGED_FRAME)) { // only treat VLAN frames without content
        rx_frame->ptEth = (ETHERNET_FRAME_CONTENT *)((unsigned char *)rx_frame->ptEth - VLAN_TAG_LENGTH); // set the frame pointer back to the start of the original buffer
        uMemcpy(rx_frame->ptEth, &(rx_frame->ptEth->ethernet_destination_MAC[VLAN_TAG_LENGTH]), (2 * MAC_LENGTH)); // shift the MAC addresses back into their original position 
        rx_frame->ptEth->ethernet_frame_type[0] = (unsigned char)(TAG_PROTOCOL_IDENTIFIER >> 8);
        rx_frame->ptEth->ethernet_frame_type[1] = (unsigned char)(TAG_PROTOCOL_IDENTIFIER); // insert the VLAN type
        rx_frame->ptEth->ucData[0] = (unsigned char)(rx_frame->usVLAN_ID >> 8);
        rx_frame->ptEth->ucData[1] = (unsigned char)(rx_frame->usVLAN_ID); // insert the original VLAN ID
        rx_frame->frame_size += VLAN_TAG_LENGTH;
        rx_frame->ucVLAN_content |= (VLAN_CONTENT_PRESENT);              // flag that the VLAN content is contained in the frame
    }
}
#endif








#if defined REMOTE_SIMULATION_INTERFACE

// UDP data server - reception call back function
//
static int fnRemoteSimUDP_Listner(USOCKET SocketNr, unsigned char ucEvent, unsigned char *ucIP, unsigned short usPortNr, unsigned char *data, unsigned short usLength)
{
    unsigned short usThisLength;
    unsigned char  ucThisCommand = 0;
    switch (ucEvent) {
    case UDP_EVENT_RXDATA:
        while (usLength != 0) {
            usThisLength = usLength;
            switch (*data) {
    #if defined nRF24L01_INTERFACE
            case REMOTE_RF_DISABLE_RX_TX:
                DISABLE_RX_TX();                                         // set the enable line to '0'
                usThisLength = 2;
                break;
            case REMOTE_RF_ENABLE_RX_TX:
                ENABLE_RX_TX();                                         // set the enable line to '1'
                usThisLength = 2;
                break;
            case REMOTE_RF_W_COMMAND:
                ucThisCommand = *(data + 2);
                fnCommand_nRF24L01_0(ucThisCommand);                     // command the single byte command
                usThisLength = 3;
                break;
            case REMOTE_RF_W_REGISTER:
                ucThisCommand = *(data + 2);
                usThisLength = 3;
                break;
            case REMOTE_RF_W_REGISTER_VALUE:
                usThisLength = *(data + 1);
                fnWrite_nRF24L01(ucThisCommand, (data + 2), (unsigned char)usThisLength);
                usThisLength += 2;
                break;
    #endif
    #if defined ENC424J600_INTERFACE
            case REMOTE_ETH_CMD_WITHOUT_DATA:
                ucThisCommand = *(data + 3);
                usThisLength = 4;
                enc424j600ExecuteOp0(ucThisCommand);
                break;
            case REMOTE_ETH_CMD:
                ucThisCommand = *(data + 3);
                usThisLength = 4;
                break;
            case REMOTE_ETH_DATA:
                usThisLength = ((*(data + 1) << 8) + *(data + 2));
                if (usThisLength == 2) {
                    unsigned short usThisData = ((*(data + 3) << 8) + *(data + 4));
                    unsigned short usResult = enc424j600ExecuteOp16(ucThisCommand, usThisData);
                    if ((ucThisCommand & 0xc0) == 0) {                   // read command
                        RemoteSimUDP_Frame.ucUDP_Message[0] = REMOTE_ETH_INTERFACE;
                        RemoteSimUDP_Frame.ucUDP_Message[1] = REMOTE_ETH_RX_DATA;
                        RemoteSimUDP_Frame.ucUDP_Message[2] = 0;
                        RemoteSimUDP_Frame.ucUDP_Message[3] = 2;         // length
                        RemoteSimUDP_Frame.ucUDP_Message[4] = (unsigned char)(usResult >> 8);
                        RemoteSimUDP_Frame.ucUDP_Message[5] = (unsigned char)(usResult);
                        fnSendUDP(RemoteSimUDPSocket, ucRemoteSimIP_address, REMOTE_SIM_UDP_PORT, (unsigned char *)&RemoteSimUDP_Frame.tUDP_Header, (unsigned short)(6), 0); // send to the simulator
                    }
                    usThisLength += 2;
                }
                else if (usThisLength == 3) {
                    unsigned long ulThisData = ((*(data + 3) << 16) + (*(data + 4) << 8) + *(data + 5));
                    unsigned long ulResult = enc424j600ExecuteOp24(ucThisCommand, ulThisData);
                    if ((ucThisCommand & 0xc0) == 0) {                   // read command
                        RemoteSimUDP_Frame.ucUDP_Message[0] = REMOTE_ETH_INTERFACE;
                        RemoteSimUDP_Frame.ucUDP_Message[1] = REMOTE_ETH_RX_DATA;
                        RemoteSimUDP_Frame.ucUDP_Message[2] = 0;
                        RemoteSimUDP_Frame.ucUDP_Message[3] = 3;         // length
                        RemoteSimUDP_Frame.ucUDP_Message[4] = (unsigned char)(ulResult >> 16);
                        RemoteSimUDP_Frame.ucUDP_Message[5] = (unsigned char)(ulResult >> 8);
                        RemoteSimUDP_Frame.ucUDP_Message[6] = (unsigned char)(ulResult);
                        fnSendUDP(RemoteSimUDPSocket, ucRemoteSimIP_address, REMOTE_SIM_UDP_PORT, (unsigned char *)&RemoteSimUDP_Frame.tUDP_Header, (unsigned short)(7), 0); // send to the simulator
                    }
                    usThisLength += 2;
                }
                break;
            case REMOTE_ETH_DATA_BUFFER:
                usThisLength = ((*(data + 1) << 8) + *(data + 2));
                enc424j600WriteN(ucThisCommand, (data + 3), usThisLength); // write data - no read performed
                usThisLength += 2;
                break;
            case REMOTE_ETH_DATA_RX:
                usThisLength = ((*(data + 1) << 8) + *(data + 2));
                RemoteSimUDP_Frame.ucUDP_Message[0] = REMOTE_ETH_INTERFACE;
                RemoteSimUDP_Frame.ucUDP_Message[1] = REMOTE_ETH_RX_DATA;
                RemoteSimUDP_Frame.ucUDP_Message[2] = (unsigned char)(usThisLength << 8);
                RemoteSimUDP_Frame.ucUDP_Message[3] = (unsigned char)usThisLength;
                enc424j600ReadN(ucThisCommand, &RemoteSimUDP_Frame.ucUDP_Message[4], usThisLength);
                fnSendUDP(RemoteSimUDPSocket, ucRemoteSimIP_address, REMOTE_SIM_UDP_PORT, (unsigned char *)&RemoteSimUDP_Frame.tUDP_Header, (unsigned short)(usThisLength + 4), 0); // send to the simulator
                usThisLength += 2;
                break;
    #endif
            default:
                break;
            }
            if (usThisLength >= usLength) {
                break;
            }
            data += usThisLength;
            usLength -= usThisLength;
        }
        break;

    case UDP_EVENT_PORT_UNREACHABLE:                                     // we have received information that this port is not available at the destination so quit
        break;
    }
    return 0;
}
#endif












#if defined ENC424J600_INTERFACE
    // Development with FRDM_K64F
    //
    #if defined FRDM_K64F
        #define CONFIGURE_ENC424J600_SPI_MODE() POWER_UP(6, SIM_SCGC6_SPI0); \
                                     _CONFIG_PERIPHERAL(D, 0, (PD_0_SPI0_PCS0 | PORT_DSE_HIGH | PORT_PS_UP_ENABLE | PORT_SRE_FAST)); \
                                     _CONFIG_PERIPHERAL(D, 2, (PD_2_SPI0_SOUT | PORT_DSE_HIGH | PORT_PS_UP_ENABLE | PORT_SRE_FAST)); \
                                     _CONFIG_PERIPHERAL(D, 3, (PD_3_SPI0_SIN | PORT_PS_UP_ENABLE)); \
                                     _CONFIG_PERIPHERAL(D, 1, (PD_1_SPI0_SCK  | PORT_DSE_HIGH | PORT_PS_UP_ENABLE | PORT_SRE_FAST)); \
                                     SPI0_MCR = (SPI_MCR_MSTR | SPI_MCR_DCONF_SPI | SPI_MCR_CLR_RXF | SPI_MCR_CLR_TXF | SPI_MCR_PCSIS_CS0 | SPI_MCR_PCSIS_CS1 | SPI_MCR_PCSIS_CS2 | SPI_MCR_PCSIS_CS3 | SPI_MCR_PCSIS_CS4 | SPI_MCR_PCSIS_CS5); \
                                     SPI0_CTAR1 = (SPI_CTAR_ASC_1 | SPI_CTAR_PBR_3 | SPI_CTAR_DBR | SPI_CTAR_FMSZ_8 | SPI_CTAR_PDT_2 | SPI_CTAR_BR_2); // for 120MHz system, 10MHz speed
    #elif defined TEENSY_LC
        #define ENC424J600_CS        PORTC_BIT4
        #define ASSERT_ENC424J600_CS_LINE()  _CLEARBITS(C, ENC424J600_CS)
        #define NEGATE_ENC424J600_CS_LINE()  _SETBITS(C, ENC424J600_CS)

        #define CONFIGURE_ENC424J600_SPI_MODE() POWER_UP(4, SIM_SCGC4_SPI0); \
                                     _CONFIG_PERIPHERAL(C, 5, PC_5_SPI0_SCK); \
                                     _CONFIG_PERIPHERAL(C, 6, (PC_6_SPI0_MOSI | PORT_SRE_FAST | PORT_DSE_HIGH)); \
                                     _CONFIG_PERIPHERAL(C, 7, (PC_7_SPI0_MISO | PORT_PS_UP_ENABLE)); \
                                     _CONFIG_DRIVE_PORT_OUTPUT_VALUE(C, ENC424J600_CS, ENC424J600_CS, (PORT_SRE_FAST | PORT_DSE_HIGH)); \
                                     SPI0_C1 = (SPI_C1_MSTR | SPI_C1_SPE); \
                                     SPI0_BR = (SPI_BR_SPPR_PRE_1 | SPI_BR_SPR_DIV_2); \
                                     (void)SPI0_S; (void)SPI0_D

        #if defined _WINDOWS
            #define WRITE_ENC424J600_SPI(byte)          SPI0_D = (byte)
            #define WRITE_ENC424J600_SPI_LAST(byte)     SPI0_D = (byte)
            #define WAIT_ENC424J600_SPI_RECEPTION_END() while ((SPI0_S & (SPI_S_SPRF)) == 0) { SPI0_S |= (SPI_S_SPRF); }
            #define READ_ENC424J600_SPI_FLASH_DATA()    (unsigned char)SPI0_D
        #else
            #define WRITE_ENC424J600_SPI(byte)          SPI0_D = (byte)
            #define WRITE_ENC424J600_SPI_LAST(byte)     SPI0_D = (byte)
            #define WAIT_ENC424J600_SPI_RECEPTION_END() while ((SPI0_S & (SPI_S_SPRF)) == 0) {}
            #define READ_ENC424J600_SPI_FLASH_DATA()    (unsigned char)SPI0_D
        #endif
        #define CLEAR_ENC424J600_SPI_RECEPTION_FLAG()                    // dummy for KL

        #define BACKUP_SPI()         unsigned char Original_SPI0_C1 = SPI0_C1; \
                                     unsigned char Original_SPI0_BR = SPI0_BR; \
                                     SPI0_C1 = (SPI_C1_MSTR | SPI_C1_SPE); \
                                     SPI0_BR = (SPI_BR_SPPR_PRE_1 | SPI_BR_SPR_DIV_2)
        
        #define RETURN_SPI()         SPI0_C1 = Original_SPI0_C1; \
                                     SPI0_BR = Original_SPI0_BR;
    #else
        #define CONFIGURE_ENC424J600_SPI_MODE() POWER_UP(6, SIM_SCGC6_SPI0); \
                                     _CONFIG_PERIPHERAL(C, 4, (PC_4_SPI0_PCS0 | PORT_DSE_HIGH | PORT_PS_UP_ENABLE | PORT_SRE_FAST)); \
                                     _CONFIG_PERIPHERAL(C, 5, PC_5_SPI0_SCK | PORT_SRE_FAST | PORT_DSE_HIGH); \
                                     _CONFIG_PERIPHERAL(C, 6, (PC_6_SPI0_SOUT | PORT_SRE_FAST | PORT_DSE_HIGH)); \
                                     _CONFIG_PERIPHERAL(C, 7, (PC_7_SPI0_SIN | PORT_PS_UP_ENABLE)); \
                                     SPI0_MCR = (SPI_MCR_MSTR | SPI_MCR_DCONF_SPI | SPI_MCR_CLR_RXF | SPI_MCR_CLR_TXF | SPI_MCR_PCSIS_CS0 | SPI_MCR_PCSIS_CS1 | SPI_MCR_PCSIS_CS2 | SPI_MCR_PCSIS_CS3 | SPI_MCR_PCSIS_CS4 | SPI_MCR_PCSIS_CS5); \
                                     SPI0_CTAR1 = (SPI_CTAR_ASC_1 | SPI_CTAR_PBR_3 | SPI_CTAR_DBR | SPI_CTAR_FMSZ_8 | SPI_CTAR_PDT_2 | SPI_CTAR_BR_2); // for 120MHz system, 10MHz speed
    #endif
    #if !defined KINETIS_KL
        #define FLUSH_ENC424J600_SPI_FIFO_AND_FLAGS()  SPI0_MCR |= SPI_MCR_CLR_RXF; SPI0_SR = (SPI_SR_EOQF | SPI_SR_TFUF | SPI_SR_TFFF | SPI_SR_RFOF | SPI_SR_RFDF)

        #define WRITE_ENC424J600_SPI(byte)            SPI0_PUSHR = (byte | SPI_PUSHR_CONT | SPI_PUSHR_PCS0| SPI_PUSHR_CTAS_CTAR1) // write a single byte to the output FIFO - assert CS line
        #define WRITE_ENC424J600_SPI_LAST(byte)       SPI0_PUSHR = (byte | SPI_PUSHR_EOQ  | SPI_PUSHR_PCS0| SPI_PUSHR_CTAS_CTAR1) // write final byte to output FIFO - this will negate the CS line when complete
        #define READ_ENC424J600_SPI_FLASH_DATA()      (unsigned char)SPI0_POPR
        #define WAIT_ENC424J600_SPI_RECEPTION_END()   while ((SPI0_SR & SPI_SR_RFDF) == 0) {}
        #define CLEAR_ENC424J600_SPI_RECEPTION_FLAG() SPI0_SR |= SPI_SR_RFDF

        #define BACKUP_SPI()
        #define RETURN_SPI()
    #endif

    #if defined REMOTE_SIMULATION_INTERFACE
        static void enc424j600ExecuteOp0(unsigned char ucCommand);
        static unsigned short enc424j600ExecuteOp16(unsigned char ucCommand, unsigned short usData);
        static unsigned long enc424j600ExecuteOp24(unsigned char ucCommand, unsigned long data);
        static void enc424j600WriteN(unsigned char ucCommand, unsigned char *data, unsigned short dataLen);
        static void enc424j600ReadN(unsigned char ucCommand, unsigned char *data, unsigned short usLength);
        static int fnRemoteSimUDP_Listner(USOCKET SocketNr, unsigned char ucEvent, unsigned char *ucIP, unsigned short usPortNr, unsigned char *data, unsigned short usLength);
        static void fnConfigureENC424J600Interrupt(void);

        typedef struct stREMOTE_SIM_UDP_MESSAGE
        {
            UDP_HEADER     tUDP_Header;                                      // reserve header space
            unsigned char  ucUDP_Message[1500];                              // reserve message space
        } REMOTE_SIM_UDP_MESSAGE;

        static USOCKET RemoteSimUDPSocket = -1;
        static unsigned char ucRemoteSimIP_address[] = {192, 168, 0, 99};    // PC running the simulator
        #define REMOTE_SIM_UDP_PORT   42989                                  // UDP port used for simulation traffic
        static REMOTE_SIM_UDP_MESSAGE RemoteSimUDP_Frame;
    #endif



// Define macro for 8-bit PSP SFR address translation to SPI addresses
#define ENC100_TRANSLATE_TO_PIN_ADDR(a)        ((a) & 0x00FFu)



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// SPI instruction set
//
// Single byte commands
//
#define ENC424J600_B0SEL         0xc0                                    // bank 0 select
#define ENC424J600_B1SEL         0xc2                                    // bank 1 select
#define ENC424J600_B2SEL         0xc4                                    // bank 2 select
#define ENC424J600_B3SEL         0xc6                                    // bank 3 selec
#define ENC424J600_SETETHRST     0xca                                    // system reset
#define ENC424J600_FCDIS         0xe0                                    // flow control disable
#define ENC424J600_FCSINGLE      0xe2                                    // flow control single
#define ENC424J600_FCMULTIPLE    0xe4                                    // flow control multiple
#define ENC424J600_FCCLEAR       0xe6                                    // flow control clear
#define ENC424J600_SETPKTDEC     0xcc                                    // decrement packet counter
#define ENC424J600_DMASTOP       0xd0                                    // DMA stop
#define ENC424J600_DMACKSUM      0xd8                                    // DMA start checksum
#define ENC424J600_DMACKSUMS     0xda                                    // DMA start checksum with seed
#define ENC424J600_DMACOPY       0xdc                                    // DMA start copy
#define ENC424J600_DMACOPYS      0xde                                    // DMA Start copy and checksum with seed
#define ENC424J600_SETTXRTS      0xd4                                    // request packet transmission
#define ENC424J600_ENABLERX      0xe8                                    // enable RX
#define ENC424J600_DISABLERX     0xea                                    // disable RX
#define ENC424J600_SETEIE        0xec                                    // enable interrupts
#define ENC424J600_CLREIE        0xee                                    // disable interrupts

// Single byte read
//
#define ENC424J600_RBSEL         0xc8                                    // read bank select

// Double byte read/writes
//
#define ENC424J600_WGPRDPT       0x60                                    // write EGPRDPT (General Purpose Read Pointer)
#define ENC424J600_RGPRDPT       0x62                                    // read EGPRDPT (General Purpose Read Pointer)
#define ENC424J600_WRXRDPT       0x64                                    // write ERXRDPT (RX Read Pointer)
#define ENC424J600_RRXRDPT       0x66                                    // read ERXRDPT (RX Read Pointer)
#define ENC424J600_WUDARDPT      0x68                                    // write EUDARDPT (User Defined Area Read Pointer)
#define ENC424J600_RUDARDPT      0x6a                                    // read EUDARDPT (User Defined Area Read Pointer)
#define ENC424J600_WGPWRPT       0x6c                                    // write EGPWRPT (General Purpose Write Pointer)
#define ENC424J600_RGPWRPT       0x6e                                    // read EGPWRPT (General Purpose Write Pointer)
#define ENC424J600_WRXWRPT       0x70                                    // write ERXWRPT (RX Write Pointer)
#define ENC424J600_RRXWRPT       0x72                                    // read ERXWRPT (RX Write Pointer)
#define ENC424J600_WUDAWRPT      0x74                                    // write EUDAWRPT (User Defined Area Write Pointer)
#define ENC424J600_RUDAWRPT      0x76                                    // read EUDAWRPT  (User Defined Area Write Pointer)


// Tripple byte read/writes
//
#define ENC424J600_RCR           0x00                                    // read control register
#define ENC424J600_WCR           0x40                                    // write control register
#define ENC424J600_RCRU          0x20                                    // read control register unbanked
#define ENC424J600_WCRU          0x22                                    // write control register unbanked
#define ENC424J600_BFS           0x80                                    // bit field set
#define ENC424J600_BFC           0xa0                                    // bit field clear
#define ENC424J600_BFSU          0x24                                    // bit field set unbanked
#define ENC424J600_BFCU          0x26                                    // bit field clear unbank
#define ENC424J600_RGPDATA       0x28                                    // read general purpose buffer memory
#define ENC424J600_WGPDATA       0x2a                                    // write general purpose buffer memory
#define ENC424J600_RRXDATA       0x2c                                    // read rx buffer memory
#define ENC424J600_WRXDATA       0x2e                                    // write rx buffer memory
#define ENC424J600_RUDADATA      0x30                                    // read user defined buffer memory
#define ENC424J600_WUDADATA      0x32                                    // write user defined buffer memory


// Bank 1 registers
//
#define ENC424J600_EHT1L         0x7e20                                  // hast table entres
#define ENC424J600_EHT1H         0x7e21
#define ENC424J600_EHT2L         0x7e22
#define ENC424J600_EHT2H         0x7e23
#define ENC424J600_EHT3L         0x7e24
#define ENC424J600_EHT3H         0x7e25
#define ENC424J600_EHT4L         0x7e26
#define ENC424J600_EHT4H         0x7e27


#define ENC424J600_ERXFCON       0x7e34
    #define ENC424J600_ERXFCON_HTEN       0x8000                         // hash table collection filter enable
    #define ENC424J600_ERXFCON_MPEN       0x4000                         // magic packet collection filter enable
    #define ENC424J600_ERXFCON_NOTPM      0x1000                         // pattern match inversion (match checksum errors)
    #define ENC424J600_ERXFCON_PMEN_MAGIC 0x0900                         // accept magic packet for local unicast address
    #define ENC424J600_ERXFCON_PMEN_HASH  0x0800                         // accept hash match
    #define ENC424J600_ERXFCON_PMEN_NBROAD 0x0700                        // accept not broadcast address
    #define ENC424J600_ERXFCON_PMEN_BROAD 0x0600                         // accept broadcast address
    #define ENC424J600_ERXFCON_PMEN_NMUL  0x0500                         // accept not multicast address
    #define ENC424J600_ERXFCON_PMEN_MUL   0x0400                         // accept multicast address
    #define ENC424J600_ERXFCON_PMEN_NUNI  0x0300                         // accept not local unicast
    #define ENC424J600_ERXFCON_PMEN_UNI   0x0200                         // accept local unicast
    #define ENC424J600_ERXFCON_PMEN_CS    0x0100                         // accept all packets with checksum match
    #define ENC424J600_ERXFCON_PMEN_OFF   0x0000                         // pattern match disabled
    #define ENC424J600_ERXFCON_CRCEEN     0x0080                         // CRC error collection filter enable
    #define ENC424J600_ERXFCON_CRCEN      0x0040                         // CRC error rejection filter bit enabled
    #define ENC424J600_ERXFCON_RUNTEEN    0x0020                         // runt error collection filter enable
    #define ENC424J600_ERXFCON_RUNTEN     0x0010                         // runt error rejection filter enable
    #define ENC424J600_ERXFCON_UCEN       0x0008                         // unicast destination collection filter enable
    #define ENC424J600_ERXFCON_NOTMEEN    0x0004                         // not-me unicast destination filter enable
    #define ENC424J600_ERXFCON_MCEN       0x0002                         // multicast destination collection filter enable
    #define ENC424J600_ERXFCON_BCEN       0x0001                         // broadcast destination collection filter enable

// Bank 3 registers
//
#define ENC424J600_MAADR3        0x7e60                                  // MAC address (7:0 15:8) - factory programmed but can be modified
#define ENC424J600_MAADR2        0x7e62                                  // MAC address (23:16 31:24) - factory programmed but can be modified
#define ENC424J600_MAADR1        0x7e64                                  // MAC address (39:32 47:40) - factory programmed but can be modified

#define ENC424J600_ECON2         0x7e6e                                  // Ethernet control register 2 (power on reset state = 0xcb00)
    #define ENC424J600_ECON2_ETHEN        0x8000                         // Ethernet enable bit
    #define ENC424J600_ECON2_STRCH        0x4000                         // LED stretching enable bit
    #define ENC424J600_ECON2_TXMAC        0x2000                         // automatically transmit MAC address enable bit
    #define ENC424J600_ECON2_SHA1MD5      0x1000                         // hashing engine computes a SHA-1 hash rather than an MD5 hash
    #define ENC424J600_ECON2_COCON_50kHz  0x0f00                         // CLKOUT frequency control (only reset on power up)
    #define ENC424J600_ECON2_COCON_100kHz 0x0e00
    #define ENC424J600_ECON2_COCON_3_125M 0x0c00
    #define ENC424J600_ECON2_COCON_4M     0x0b00                         // power on reset default
    #define ENC424J600_ECON2_COCON_5M     0x0a00
    #define ENC424J600_ECON2_COCON_6_25M  0x0900
    #define ENC424J600_ECON2_COCON_8M     0x0800                         // note - not 50% duty cycle
    #define ENC424J600_ECON2_COCON_8_333M 0x0700
    #define ENC424J600_ECON2_COCON_10M    0x0600
    #define ENC424J600_ECON2_COCON_12_5M  0x0500
    #define ENC424J600_ECON2_COCON_16_67M 0x0400
    #define ENC424J600_ECON2_COCON_20M    0x0300
    #define ENC424J600_ECON2_COCON_25M    0x0200
    #define ENC424J600_ECON2_COCON_33_33M 0x0100
    #define ENC424J600_ECON2_COCON_OFF    0x0000
    #define ENC424J600_ECON2_AUTOFC       0x0080                         // automatic flow control enable
    #define ENC424J600_ECON2_TXRST        0x0040                         // transmit logic reset
    #define ENC424J600_ECON2_RXRST        0x0020                         // receive logic reset
    #define ENC424J600_ECON2_ETHRST       0x0010                         // master Ethernet reset
    #define ENC424J600_ECON2_MODLEN_1024  0x0008                         // modular exponential length control
    #define ENC424J600_ECON2_MODLEN_768   0x0004
    #define ENC424J600_ECON2_MODLEN_512   0x0000
    #define ENC424J600_ECON2_AESLEN_256   0x0002                         // AES key length control
    #define ENC424J600_ECON2_AESLEN_192   0x0001
    #define ENC424J600_ECON2_AESLEN_128   0x0000

#define ENC424J600_EIE           0x7e72                                  // Ethernet interrupt enable register
    #define ENC424J600_EIE_INTIE          0x8000                         // global interrupt enable
    #define ENC424J600_EIE_MODEXIE        0x4000                         // modular exponentiation interrupt enable
    #define ENC424J600_EIE_HASHIE         0x2000                         // MD5/SHA-1 hash interrupt enable
    #define ENC424J600_EIE_AESIE          0x1000                         // AES encrypt/decrypt interrupt enable
    #define ENC424J600_EIE_LINKIE         0x0800                         // PHY link status change interrupt enable
    #define ENC424J600_EIE_PKTIE          0x0040                         // receive packet pending interrupt enable
    #define ENC424J600_EIE_DMAIE          0x0020                         // DMA interrupt enable
    #define ENC424J600_EIE_TXIE           0x0008                         // transmit done interrupt enable
    #define ENC424J600_EIE_TXABTIE        0x0004                         // transmit abort interrupt enable
    #define ENC424J600_EIE_RXABTIE        0x0002                         // receive abort interrupt enable
    #define ENC424J600_EIE_PCFULIE        0x0001                         // packet counter full interrupt enable



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// SPI all bank registers
//
#define ENC424J600_EUDAST        0x7e16                                  // user defined area start pointer
#define ENC424J600_ESTAT         0x7e1a                                  // Ethernet status register
    #define ENC424J600_ESTAT_INT          0x8000                         // interrupt pending status (one of the EIR bits is set and enabled by the EIE register)
    #define ENC424J600_ESTAT_FCIDLE       0x4000                         // flow control idle status (internal flow control state machine is idle)
    #define ENC424J600_ESTAT_RXBUSY       0x2000                         // receive logic active status (currently receiving packet)
    #define ENC424J600_ESTAT_CLKRDY       0x1000                         // clock ready status
    #define ENC424J600_ESTAT_PHYDPX       0x0400                         // PHY full duplex status
    #define ENC424J600_ESTAT_PHYLNK       0x0100                         // PHY link status
    #define ENC424J600_ESTAT_PKTCNT_MASK  0x00ff                         // receive packet count - number of complete packets that are saved in the receive buffer andready for software processing
#define ENC424J600_EIR           0x7e1c                                  // Ethernet interrupt flag register
    #define ENC424J600_EIR_CRYPTEN        0x8000                         // modular exponentiation and AES cryptographic modules enable
    #define ENC424J600_EIR_MODEXIF        0x4000                         // modular exponentiation calculation is complete - interrupt flag
    #define ENC424J600_EIR_HASHIF         0x2000                         // MD5/SHA-1 hash operation is complete - interrupt flag
    #define ENC424J600_EIR_AESIF          0x1000                         // AES encrypt/decrypt operation is complete - interrupt flag
    #define ENC424J600_EIR_LINKIF         0x0800                         // PHY link status change interrupt flag
    #define ENC424J600_EIR_PKTIF          0x0040                         // receive packet pending interrupt flag
    #define ENC424J600_EIR_DMAIF          0x0020                         // DMA interrupt flag
    #define ENC424J600_EIR_TXIF           0x0008                         // transmit done interrupt flag
    #define ENC424J600_EIR_TXABTIF        0x0004                         // transmit abort interrupt flag
    #define ENC424J600_EIR_RXABTIF        0x0002                         // receive abort interrupt flag
    #define ENC424J600_EIR_PCFULIF        0x0001                         // packet counter full interrupt flag






#define ENC424J600_EUDAND        ENC100_TRANSLATE_TO_PIN_ADDR(0x7E18u)
#define ENC424J600_EUDANDL        ENC100_TRANSLATE_TO_PIN_ADDR(0x7E18u)
#define ENC424J600_EUDANDH        ENC100_TRANSLATE_TO_PIN_ADDR(0x7E19u)

#define ENC424J600_ESTATL        ENC100_TRANSLATE_TO_PIN_ADDR(0x7E1Au)
#define ENC424J600_ESTATH        ENC100_TRANSLATE_TO_PIN_ADDR(0x7E1Bu)

#define ENC424J600_EIRL        ENC100_TRANSLATE_TO_PIN_ADDR(0x7E1Cu)
#define ENC424J600_EIRH        ENC100_TRANSLATE_TO_PIN_ADDR(0x7E1Du)
#define ENC424J600_ECON1        ENC100_TRANSLATE_TO_PIN_ADDR(0x7E1Eu)
#define ENC424J600_ECON1L        ENC100_TRANSLATE_TO_PIN_ADDR(0x7E1Eu)
#define ENC424J600_ECON1H        ENC100_TRANSLATE_TO_PIN_ADDR(0x7E1Fu)

// SPI Bank 0 registers --------
#define ENC424J600_ETXST        ENC100_TRANSLATE_TO_PIN_ADDR(0x7E00u)
#define ENC424J600_ETXSTL        ENC100_TRANSLATE_TO_PIN_ADDR(0x7E00u)
#define ENC424J600_ETXSTH        ENC100_TRANSLATE_TO_PIN_ADDR(0x7E01u)
#define ENC424J600_ETXLEN        ENC100_TRANSLATE_TO_PIN_ADDR(0x7E02u)
#define ENC424J600_ETXLENL        ENC100_TRANSLATE_TO_PIN_ADDR(0x7E02u)
#define ENC424J600_ETXLENH        ENC100_TRANSLATE_TO_PIN_ADDR(0x7E03u)
#define ENC424J600_ERXST        ENC100_TRANSLATE_TO_PIN_ADDR(0x7E04u)
#define ENC424J600_ERXSTL        ENC100_TRANSLATE_TO_PIN_ADDR(0x7E04u)
#define ENC424J600_ERXSTH        ENC100_TRANSLATE_TO_PIN_ADDR(0x7E05u)
#define ENC424J600_ERXTAIL        ENC100_TRANSLATE_TO_PIN_ADDR(0x7E06u)
#define ENC424J600_ERXTAILL    ENC100_TRANSLATE_TO_PIN_ADDR(0x7E06u)
#define ENC424J600_ERXTAILH    ENC100_TRANSLATE_TO_PIN_ADDR(0x7E07u)
#define ENC424J600_ERXHEAD        ENC100_TRANSLATE_TO_PIN_ADDR(0x7E08u)
#define ENC424J600_ERXHEADL    ENC100_TRANSLATE_TO_PIN_ADDR(0x7E08u)
#define ENC424J600_ERXHEADH    ENC100_TRANSLATE_TO_PIN_ADDR(0x7E09u)
#define ENC424J600_EDMAST        ENC100_TRANSLATE_TO_PIN_ADDR(0x7E0Au)
#define ENC424J600_EDMASTL        ENC100_TRANSLATE_TO_PIN_ADDR(0x7E0Au)
#define ENC424J600_EDMASTH        ENC100_TRANSLATE_TO_PIN_ADDR(0x7E0Bu)
#define ENC424J600_EDMALEN        ENC100_TRANSLATE_TO_PIN_ADDR(0x7E0Cu)
#define ENC424J600_EDMALENL    ENC100_TRANSLATE_TO_PIN_ADDR(0x7E0Cu)
#define ENC424J600_EDMALENH    ENC100_TRANSLATE_TO_PIN_ADDR(0x7E0Du)
#define ENC424J600_EDMADST        ENC100_TRANSLATE_TO_PIN_ADDR(0x7E0Eu)
#define ENC424J600_EDMADSTL    ENC100_TRANSLATE_TO_PIN_ADDR(0x7E0Eu)
#define ENC424J600_EDMADSTH    ENC100_TRANSLATE_TO_PIN_ADDR(0x7E0Fu)
#define ENC424J600_EDMACS        ENC100_TRANSLATE_TO_PIN_ADDR(0x7E10u)
#define ENC424J600_EDMACSL        ENC100_TRANSLATE_TO_PIN_ADDR(0x7E10u)
#define ENC424J600_EDMACSH        ENC100_TRANSLATE_TO_PIN_ADDR(0x7E11u)
#define ENC424J600_ETXSTAT        ENC100_TRANSLATE_TO_PIN_ADDR(0x7E12u)
#define ENC424J600_ETXSTATL    ENC100_TRANSLATE_TO_PIN_ADDR(0x7E12u)
#define ENC424J600_ETXSTATH    ENC100_TRANSLATE_TO_PIN_ADDR(0x7E13u)
#define ENC424J600_ETXWIRE        ENC100_TRANSLATE_TO_PIN_ADDR(0x7E14u)
#define ENC424J600_ETXWIREL    ENC100_TRANSLATE_TO_PIN_ADDR(0x7E14u)
#define ENC424J600_ETXWIREH    ENC100_TRANSLATE_TO_PIN_ADDR(0x7E15u)

// SPI Bank 1 registers -----
/*#define ENC424J600_EHT1        ENC100_TRANSLATE_TO_PIN_ADDR(0x7E20u)
#define ENC424J600_EHT1L        ENC100_TRANSLATE_TO_PIN_ADDR(0x7E20u)
#define ENC424J600_EHT1H        ENC100_TRANSLATE_TO_PIN_ADDR(0x7E21u)
#define ENC424J600_EHT2        ENC100_TRANSLATE_TO_PIN_ADDR(0x7E22u)
#define ENC424J600_EHT2L        ENC100_TRANSLATE_TO_PIN_ADDR(0x7E22u)
#define ENC424J600_EHT2H        ENC100_TRANSLATE_TO_PIN_ADDR(0x7E23u)
#define ENC424J600_EHT3        ENC100_TRANSLATE_TO_PIN_ADDR(0x7E24u)
#define ENC424J600_EHT3L        ENC100_TRANSLATE_TO_PIN_ADDR(0x7E24u)
#define ENC424J600_EHT3H        ENC100_TRANSLATE_TO_PIN_ADDR(0x7E25u)
#define ENC424J600_EHT4        ENC100_TRANSLATE_TO_PIN_ADDR(0x7E26u)
#define ENC424J600_EHT4L        ENC100_TRANSLATE_TO_PIN_ADDR(0x7E26u)
#define ENC424J600_EHT4H        ENC100_TRANSLATE_TO_PIN_ADDR(0x7E27u)*/
#define ENC424J600_EPMM1        ENC100_TRANSLATE_TO_PIN_ADDR(0x7E28u)
#define ENC424J600_EPMM1L        ENC100_TRANSLATE_TO_PIN_ADDR(0x7E28u)
#define ENC424J600_EPMM1H        ENC100_TRANSLATE_TO_PIN_ADDR(0x7E29u)
#define ENC424J600_EPMM2        ENC100_TRANSLATE_TO_PIN_ADDR(0x7E2Au)
#define ENC424J600_EPMM2L        ENC100_TRANSLATE_TO_PIN_ADDR(0x7E2Au)
#define ENC424J600_EPMM2H        ENC100_TRANSLATE_TO_PIN_ADDR(0x7E2Bu)
#define ENC424J600_EPMM3        ENC100_TRANSLATE_TO_PIN_ADDR(0x7E2Cu)
#define ENC424J600_EPMM3L        ENC100_TRANSLATE_TO_PIN_ADDR(0x7E2Cu)
#define ENC424J600_EPMM3H        ENC100_TRANSLATE_TO_PIN_ADDR(0x7E2Du)
#define ENC424J600_EPMM4        ENC100_TRANSLATE_TO_PIN_ADDR(0x7E2Eu)
#define ENC424J600_EPMM4L        ENC100_TRANSLATE_TO_PIN_ADDR(0x7E2Eu)
#define ENC424J600_EPMM4H        ENC100_TRANSLATE_TO_PIN_ADDR(0x7E2Fu)
#define ENC424J600_EPMCS        ENC100_TRANSLATE_TO_PIN_ADDR(0x7E30u)
#define ENC424J600_EPMCSL        ENC100_TRANSLATE_TO_PIN_ADDR(0x7E30u)
#define ENC424J600_EPMCSH        ENC100_TRANSLATE_TO_PIN_ADDR(0x7E31u)
#define ENC424J600_EPMO        ENC100_TRANSLATE_TO_PIN_ADDR(0x7E32u)
#define ENC424J600_EPMOL        ENC100_TRANSLATE_TO_PIN_ADDR(0x7E32u)
#define ENC424J600_EPMOH        ENC100_TRANSLATE_TO_PIN_ADDR(0x7E33u)



// SPI Bank 2 registers -----
#define ENC424J600_MACON1        ENC100_TRANSLATE_TO_PIN_ADDR(0x7E40u)
#define ENC424J600_MACON1L        ENC100_TRANSLATE_TO_PIN_ADDR(0x7E40u)
#define ENC424J600_MACON1H        ENC100_TRANSLATE_TO_PIN_ADDR(0x7E41u)
#define ENC424J600_MACON2        ENC100_TRANSLATE_TO_PIN_ADDR(0x7E42u)
#define ENC424J600_MACON2L        ENC100_TRANSLATE_TO_PIN_ADDR(0x7E42u)
#define ENC424J600_MACON2H        ENC100_TRANSLATE_TO_PIN_ADDR(0x7E43u)
#define ENC424J600_MABBIPG        ENC100_TRANSLATE_TO_PIN_ADDR(0x7E44u)
#define ENC424J600_MABBIPGL    ENC100_TRANSLATE_TO_PIN_ADDR(0x7E44u)
#define ENC424J600_MABBIPGH    ENC100_TRANSLATE_TO_PIN_ADDR(0x7E45u)
#define ENC424J600_MAIPG        ENC100_TRANSLATE_TO_PIN_ADDR(0x7E46u)
#define ENC424J600_MAIPGL        ENC100_TRANSLATE_TO_PIN_ADDR(0x7E46u)
#define ENC424J600_MAIPGH        ENC100_TRANSLATE_TO_PIN_ADDR(0x7E47u)
#define ENC424J600_MACLCON        ENC100_TRANSLATE_TO_PIN_ADDR(0x7E48u)
#define ENC424J600_MACLCONL    ENC100_TRANSLATE_TO_PIN_ADDR(0x7E48u)
#define ENC424J600_MACLCONH    ENC100_TRANSLATE_TO_PIN_ADDR(0x7E49u)
#define ENC424J600_MAMXFL        ENC100_TRANSLATE_TO_PIN_ADDR(0x7E4Au)
#define ENC424J600_MAMXFLL        ENC100_TRANSLATE_TO_PIN_ADDR(0x7E4Au)
#define ENC424J600_MAMXFLH        ENC100_TRANSLATE_TO_PIN_ADDR(0x7E4Bu)
#define ENC424J600_MICMD        ENC100_TRANSLATE_TO_PIN_ADDR(0x7E52u)
#define ENC424J600_MICMDL        ENC100_TRANSLATE_TO_PIN_ADDR(0x7E52u)
#define ENC424J600_MICMDH        ENC100_TRANSLATE_TO_PIN_ADDR(0x7E53u)
#define ENC424J600_MIREGADR    ENC100_TRANSLATE_TO_PIN_ADDR(0x7E54u)
#define ENC424J600_MIREGADRL    ENC100_TRANSLATE_TO_PIN_ADDR(0x7E54u)
#define ENC424J600_MIREGADRH    ENC100_TRANSLATE_TO_PIN_ADDR(0x7E55u)

// SPI Bank 3 registers -----
//#define ENC424J600_MAADR3        ENC100_TRANSLATE_TO_PIN_ADDR(0x7E60u)
//#define ENC424J600_MAADR3L        ENC100_TRANSLATE_TO_PIN_ADDR(0x7E60u)
//#define ENC424J600_MAADR3H        ENC100_TRANSLATE_TO_PIN_ADDR(0x7E61u)
//#define ENC424J600_MAADR2        ENC100_TRANSLATE_TO_PIN_ADDR(0x7E62u)
//#define ENC424J600_MAADR2L        ENC100_TRANSLATE_TO_PIN_ADDR(0x7E62u)
//#define ENC424J600_MAADR2H        ENC100_TRANSLATE_TO_PIN_ADDR(0x7E63u)
//#define ENC424J600_MAADR1        ENC100_TRANSLATE_TO_PIN_ADDR(0x7E64u)
//#define ENC424J600_MAADR1L        ENC100_TRANSLATE_TO_PIN_ADDR(0x7E64u)
//#define ENC424J600_MAADR1H        ENC100_TRANSLATE_TO_PIN_ADDR(0x7E65u)
#define ENC424J600_MIWR        ENC100_TRANSLATE_TO_PIN_ADDR(0x7E66u)
#define ENC424J600_MIWRL        ENC100_TRANSLATE_TO_PIN_ADDR(0x7E66u)
#define ENC424J600_MIWRH        ENC100_TRANSLATE_TO_PIN_ADDR(0x7E67u)
#define ENC424J600_MIRD        ENC100_TRANSLATE_TO_PIN_ADDR(0x7E68u)
#define ENC424J600_MIRDL        ENC100_TRANSLATE_TO_PIN_ADDR(0x7E68u)
#define ENC424J600_MIRDH        ENC100_TRANSLATE_TO_PIN_ADDR(0x7E69u)
#define ENC424J600_MISTAT        ENC100_TRANSLATE_TO_PIN_ADDR(0x7E6Au)
#define ENC424J600_MISTATL        ENC100_TRANSLATE_TO_PIN_ADDR(0x7E6Au)
#define ENC424J600_MISTATH        ENC100_TRANSLATE_TO_PIN_ADDR(0x7E6Bu)
#define ENC424J600_EPAUS        ENC100_TRANSLATE_TO_PIN_ADDR(0x7E6Cu)
#define ENC424J600_EPAUSL        ENC100_TRANSLATE_TO_PIN_ADDR(0x7E6Cu)
#define ENC424J600_EPAUSH        ENC100_TRANSLATE_TO_PIN_ADDR(0x7E6Du)
//#define ENC424J600_ECON2        ENC100_TRANSLATE_TO_PIN_ADDR(0x7E6Eu)
#define ENC424J600_ECON2L        ENC100_TRANSLATE_TO_PIN_ADDR(0x7E6Eu)
#define ENC424J600_ECON2H        ENC100_TRANSLATE_TO_PIN_ADDR(0x7E6Fu)
#define ENC424J600_ERXWM        ENC100_TRANSLATE_TO_PIN_ADDR(0x7E70u)
#define ENC424J600_ERXWML        ENC100_TRANSLATE_TO_PIN_ADDR(0x7E70u)
#define ENC424J600_ERXWMH        ENC100_TRANSLATE_TO_PIN_ADDR(0x7E71u)
//#define ENC424J600_EIE        ENC100_TRANSLATE_TO_PIN_ADDR(0x7E72u)
#define ENC424J600_EIEL        ENC100_TRANSLATE_TO_PIN_ADDR(0x7E72u)
#define ENC424J600_EIEH        ENC100_TRANSLATE_TO_PIN_ADDR(0x7E73u)
#define ENC424J600_EIDLED        ENC100_TRANSLATE_TO_PIN_ADDR(0x7E74u)
#define ENC424J600_EIDLEDL        ENC100_TRANSLATE_TO_PIN_ADDR(0x7E74u)
#define ENC424J600_EIDLEDH        ENC100_TRANSLATE_TO_PIN_ADDR(0x7E75u)



// SPI Non-banked Special Function Registers
#define ENC424J600_EGPDATA        ENC100_TRANSLATE_TO_PIN_ADDR(0x7E80u)
#define ENC424J600_EGPDATAL    ENC100_TRANSLATE_TO_PIN_ADDR(0x7E80u)
//#define ENC424J600_r            ENC100_TRANSLATE_TO_PIN_ADDR(0x7E81u)
#define ENC424J600_ERXDATA        ENC100_TRANSLATE_TO_PIN_ADDR(0x7E82u)
#define ENC424J600_ERXDATAL    ENC100_TRANSLATE_TO_PIN_ADDR(0x7E82u)
//#define ENC424J600_r            ENC100_TRANSLATE_TO_PIN_ADDR(0x7E83u)
#define ENC424J600_EUDADATA    ENC100_TRANSLATE_TO_PIN_ADDR(0x7E84u)
#define ENC424J600_EUDADATAL    ENC100_TRANSLATE_TO_PIN_ADDR(0x7E84u)
//#define ENC424J600_r            ENC100_TRANSLATE_TO_PIN_ADDR(0x7E85u)
#define ENC424J600_EGPRDPT        ENC100_TRANSLATE_TO_PIN_ADDR(0x7E86u)
#define ENC424J600_EGPRDPTL    ENC100_TRANSLATE_TO_PIN_ADDR(0x7E86u)
#define ENC424J600_EGPRDPTH    ENC100_TRANSLATE_TO_PIN_ADDR(0x7E87u)
#define ENC424J600_EGPWRPT        ENC100_TRANSLATE_TO_PIN_ADDR(0x7E88u)
#define ENC424J600_EGPWRPTL    ENC100_TRANSLATE_TO_PIN_ADDR(0x7E88u)
#define ENC424J600_EGPWRPTH    ENC100_TRANSLATE_TO_PIN_ADDR(0x7E89u)
#define ENC424J600_ERXRDPT        ENC100_TRANSLATE_TO_PIN_ADDR(0x7E8Au)
#define ENC424J600_ERXRDPTL    ENC100_TRANSLATE_TO_PIN_ADDR(0x7E8Au)
#define ENC424J600_ERXRDPTH    ENC100_TRANSLATE_TO_PIN_ADDR(0x7E8Bu)
#define ENC424J600_ERXWRPT        ENC100_TRANSLATE_TO_PIN_ADDR(0x7E8Cu)
#define ENC424J600_ERXWRPTL    ENC100_TRANSLATE_TO_PIN_ADDR(0x7E8Cu)
#define ENC424J600_ERXWRPTH    ENC100_TRANSLATE_TO_PIN_ADDR(0x7E8Du)
#define ENC424J600_EUDARDPT    ENC100_TRANSLATE_TO_PIN_ADDR(0x7E8Eu)
#define ENC424J600_EUDARDPTL    ENC100_TRANSLATE_TO_PIN_ADDR(0x7E8Eu)
#define ENC424J600_EUDARDPTH    ENC100_TRANSLATE_TO_PIN_ADDR(0x7E8Fu)
#define ENC424J600_EUDAWRPT    ENC100_TRANSLATE_TO_PIN_ADDR(0x7E90u)
#define ENC424J600_EUDAWRPTL    ENC100_TRANSLATE_TO_PIN_ADDR(0x7E90u)
#define ENC424J600_EUDAWRPTH    ENC100_TRANSLATE_TO_PIN_ADDR(0x7E91u)



// Crypto memory addresses.  These are accessible by the DMA only and therefore
// have the same addresses no matter what MCU interface is being used (SPI,
// 8-bit PSP, or 16-bit PSP)
#define ENC424J600_ENC100_MODEX_Y            (0x7880u)
#define ENC424J600_ENC100_MODEX_E            (0x7800u)
#define ENC424J600_ENC100_MODEX_X            (0x7880u)
#define ENC424J600_ENC100_MODEX_M            (0x7900u)
#define ENC424J600_ENC100_HASH_DATA_IN        (0x7A00u)
#define ENC424J600_ENC100_HASH_IV_IN        (0x7A40u)
#define ENC424J600_ENC100_HASH_LEN_IN        (0x7A54u)
#define ENC424J600_ENC100_HASH_DIGEST_OUT          (0x7A70u)
#define ENC424J600_ENC100_HASH_LEN_OUT        (0x7A84u)
#define ENC424J600_ENC100_HASH_BASE_ADDR           (0x7A00u)
#define ENC424J600_ENC100_AES_KEY            (0x7C00u)
#define ENC424J600_ENC100_AES_TEXTA        (0x7C20u)
#define ENC424J600_ENC100_AES_TEXTB        (0x7C30u)
#define ENC424J600_ENC100_AES_XOROUT        (0x7C40u)


////////////////////////////////////////////////////
// ENC424J600/624J600 PHY Register Addresses      //
////////////////////////////////////////////////////
#define ENC424J600_PHCON1    0x00u
#define ENC424J600_PHSTAT1    0x01u
#define ENC424J600_PHANA    0x04u
#define ENC424J600_PHANLPA    0x05u
#define ENC424J600_PHANE    0x06u
#define ENC424J600_PHCON2    0x11u
#define ENC424J600_PHSTAT2    0x1Bu
#define ENC424J600_PHSTAT3    0x1Fu



////////////////////////////////////////////////////
// ENC424J600/624J600 register bits                  //
////////////////////////////////////////////////////
// ESTAT bits ----------


// EIR bits ------------


// ECON1 bits ----------
#define ENC424J600_ECON1_MODEXST           ((unsigned short)1<<15)
#define ENC424J600_ECON1_HASHEN            ((unsigned short)1<<14)
#define ENC424J600_ECON1_HASHOP            ((unsigned short)1<<13)
#define ENC424J600_ECON1_HASHLST           ((unsigned short)1<<12)
#define ENC424J600_ECON1_AESST        ((unsigned short)1<<11)
#define ENC424J600_ECON1_AESOP1            ((unsigned short)1<<10)
#define ENC424J600_ECON1_AESOP0            ((unsigned short)1<<9)
#define ENC424J600_ECON1_PKTDEC            ((unsigned short)1<<8)
#define ENC424J600_ECON1_FCOP1        (1<<7)
#define ENC424J600_ECON1_FCOP0        (1<<6)
#define ENC424J600_ECON1_DMAST        (1<<5)
#define ENC424J600_ECON1_DMACPY            (1<<4)
#define ENC424J600_ECON1_DMACSSD           (1<<3)
#define ENC424J600_ECON1_DMANOCS           (1<<2)
#define ENC424J600_ECON1_TXRTS        (1<<1)
#define ENC424J600_ECON1_RXEN        (1)

// ETXSTAT bits --------
#define ENC424J600_ETXSTAT_r12        ((unsigned short)1<<12)
#define ENC424J600_ETXSTAT_r11        ((unsigned short)1<<11)
#define ENC424J600_ETXSTAT_LATECOL         ((unsigned short)1<<10)
#define ENC424J600_ETXSTAT_MAXCOL          ((unsigned short)1<<9)
#define ENC424J600_ETXSTAT_EXDEFER         ((unsigned short)1<<8)
#define ENC424J600_ETXSTAT_DEFER           (1<<7)
#define ENC424J600_ETXSTAT_r6        (1<<6)
#define ENC424J600_ETXSTAT_r5        (1<<5)
#define ENC424J600_ETXSTAT_CRCBAD          (1<<4)
#define ENC424J600_ETXSTAT_COLCNT3         (1<<3)
#define ENC424J600_ETXSTAT_COLCNT2         (1<<2)
#define ENC424J600_ETXSTAT_COLCNT1         (1<<1)
#define ENC424J600_ETXSTAT_COLCNT0         (1)



// MACON1 bits ---------
#define ENC424J600_MACON1_r15        ((unsigned short)1<<15)
#define ENC424J600_MACON1_r14        ((unsigned short)1<<14)
#define ENC424J600_MACON1_r11        ((unsigned short)1<<11)
#define ENC424J600_MACON1_r10        ((unsigned short)1<<10)
#define ENC424J600_MACON1_r9        ((unsigned short)1<<9)
#define ENC424J600_MACON1_r8        ((unsigned short)1<<8)
#define ENC424J600_MACON1_LOOPBK           (1<<4)
#define ENC424J600_MACON1_r3        (1<<3)
#define ENC424J600_MACON1_RXPAUS           (1<<2)
#define ENC424J600_MACON1_PASSALL          (1<<1)
#define ENC424J600_MACON1_r0        (1)

// MACON2 bits ---------
#define ENC424J600_MACON2_DEFER            ((unsigned short)1<<14)
#define ENC424J600_MACON2_BPEN        ((unsigned short)1<<13)
#define ENC424J600_MACON2_NOBKOFF          ((unsigned short)1<<12)
#define ENC424J600_MACON2_r9        ((unsigned short)1<<9)
#define ENC424J600_MACON2_r8        ((unsigned short)1<<8)
#define ENC424J600_MACON2_PADCFG2          (1<<7)
#define ENC424J600_MACON2_PADCFG1          (1<<6)
#define ENC424J600_MACON2_PADCFG0          (1<<5)
#define ENC424J600_MACON2_TXCRCEN          (1<<4)
#define ENC424J600_MACON2_PHDREN           (1<<3)
#define ENC424J600_MACON2_HFRMEN           (1<<2)
#define ENC424J600_MACON2_r1        (1<<1)
#define ENC424J600_MACON2_FULDPX           (1)

// MABBIPG bits --------
#define ENC424J600_MABBIPG_BBIPG6          (1<<6)
#define ENC424J600_MABBIPG_BBIPG5          (1<<5)
#define ENC424J600_MABBIPG_BBIPG4          (1<<4)
#define ENC424J600_MABBIPG_BBIPG3          (1<<3)
#define ENC424J600_MABBIPG_BBIPG2          (1<<2)
#define ENC424J600_MABBIPG_BBIPG1          (1<<1)
#define ENC424J600_MABBIPG_BBIPG0          (1)

// MAIPG bits ----------
#define ENC424J600_MAIPG_r14        ((unsigned short)1<<14)
#define ENC424J600_MAIPG_r13        ((unsigned short)1<<13)
#define ENC424J600_MAIPG_r12        ((unsigned short)1<<12)
#define ENC424J600_MAIPG_r11        ((unsigned short)1<<11)
#define ENC424J600_MAIPG_r10        ((unsigned short)1<<10)
#define ENC424J600_MAIPG_r9        ((unsigned short)1<<9)
#define ENC424J600_MAIPG_r8        ((unsigned short)1<<8)
#define ENC424J600_MAIPG_IPG6        (1<<6)
#define ENC424J600_MAIPG_IPG5        (1<<5)
#define ENC424J600_MAIPG_IPG4        (1<<4)
#define ENC424J600_MAIPG_IPG3        (1<<3)
#define ENC424J600_MAIPG_IPG2        (1<<2)
#define ENC424J600_MAIPG_IPG1        (1<<1)
#define ENC424J600_MAIPG_IPG0        (1)

// MACLCON bits --------
#define ENC424J600_MACLCON_r13        ((unsigned short)1<<13)
#define ENC424J600_MACLCON_r12        ((unsigned short)1<<12)
#define ENC424J600_MACLCON_r11        ((unsigned short)1<<11)
#define ENC424J600_MACLCON_r10        ((unsigned short)1<<10)
#define ENC424J600_MACLCON_r9        ((unsigned short)1<<9)
#define ENC424J600_MACLCON_r8        ((unsigned short)1<<8)
#define ENC424J600_MACLCON_MAXRET3         (1<<3)
#define ENC424J600_MACLCON_MAXRET2         (1<<2)
#define ENC424J600_MACLCON_MAXRET1         (1<<1)
#define ENC424J600_MACLCON_MAXRET0         (1)

// MICMD bits ----------
#define ENC424J600_MICMD_MIISCAN           (1<<1)
#define ENC424J600_MICMD_MIIRD        (1)

// MIREGADR bits -------
#define ENC424J600_MIREGADR_r12            ((unsigned short)1<<12)
#define ENC424J600_MIREGADR_r11            ((unsigned short)1<<11)
#define ENC424J600_MIREGADR_r10            ((unsigned short)1<<10)
#define ENC424J600_MIREGADR_r9        ((unsigned short)1<<9)
#define ENC424J600_MIREGADR_r8        ((unsigned short)1<<8)
#define ENC424J600_MIREGADR_PHREG4         (1<<4)
#define ENC424J600_MIREGADR_PHREG3         (1<<3)
#define ENC424J600_MIREGADR_PHREG2         (1<<2)
#define ENC424J600_MIREGADR_PHREG1         (1<<1)
#define ENC424J600_MIREGADR_PHREG0         (1)

// MISTAT bits ---------
#define ENC424J600_MISTAT_r3        (1<<3)
#define ENC424J600_MISTAT_NVALID           (1<<2)
#define ENC424J600_MISTAT_SCAN        (1<<1)
#define ENC424J600_MISTAT_BUSY        (1)



// ERXWM bits ----------
#define ENC424J600_ERXWM_RXFWM7            ((unsigned short)1<<15)
#define ENC424J600_ERXWM_RXFWM6            ((unsigned short)1<<14)
#define ENC424J600_ERXWM_RXFWM5            ((unsigned short)1<<13)
#define ENC424J600_ERXWM_RXFWM4            ((unsigned short)1<<12)
#define ENC424J600_ERXWM_RXFWM3            ((unsigned short)1<<11)
#define ENC424J600_ERXWM_RXFWM2            ((unsigned short)1<<10)
#define ENC424J600_ERXWM_RXFWM1            ((unsigned short)1<<9)
#define ENC424J600_ERXWM_RXFWM0            ((unsigned short)1<<8)
#define ENC424J600_ERXWM_RXEWM7            (1<<7)
#define ENC424J600_ERXWM_RXEWM6            (1<<6)
#define ENC424J600_ERXWM_RXEWM5            (1<<5)
#define ENC424J600_ERXWM_RXEWM4            (1<<4)
#define ENC424J600_ERXWM_RXEWM3            (1<<3)
#define ENC424J600_ERXWM_RXEWM2            (1<<2)
#define ENC424J600_ERXWM_RXEWM1            (1<<1)
#define ENC424J600_ERXWM_RXEWM0            (1)



// EIDLED bits ---------
#define ENC424J600_EIDLED_LACFG3           ((unsigned short)1<<15)
#define ENC424J600_EIDLED_LACFG2           ((unsigned short)1<<14)
#define ENC424J600_EIDLED_LACFG1           ((unsigned short)1<<13)
#define ENC424J600_EIDLED_LACFG0           ((unsigned short)1<<12)
#define ENC424J600_EIDLED_LBCFG3           ((unsigned short)1<<11)
#define ENC424J600_EIDLED_LBCFG2           ((unsigned short)1<<10)
#define ENC424J600_EIDLED_LBCFG1           ((unsigned short)1<<9)
#define ENC424J600_EIDLED_LBCFG0           ((unsigned short)1<<8)
#define ENC424J600_EIDLED_DEVID2           (1<<7)
#define ENC424J600_EIDLED_DEVID1           (1<<6)
#define ENC424J600_EIDLED_DEVID0           (1<<5)
#define ENC424J600_EIDLED_REVID4           (1<<4)
#define ENC424J600_EIDLED_REVID3           (1<<3)
#define ENC424J600_EIDLED_REVID2           (1<<2)
#define ENC424J600_EIDLED_REVID1           (1<<1)
#define ENC424J600_EIDLED_REVID0           (1)

// PHCON1 bits ---------
#define ENC424J600_PHCON1_PRST        ((unsigned short)1<<15)
#define ENC424J600_PHCON1_PLOOPBK          ((unsigned short)1<<14)
#define ENC424J600_PHCON1_SPD100           ((unsigned short)1<<13)
#define ENC424J600_PHCON1_ANEN        ((unsigned short)1<<12)
#define ENC424J600_PHCON1_PSLEEP           ((unsigned short)1<<11)
#define ENC424J600_PHCON1_r10        ((unsigned short)1<<10)
#define ENC424J600_PHCON1_RENEG            ((unsigned short)1<<9)
#define ENC424J600_PHCON1_PFULDPX          ((unsigned short)1<<8)
#define ENC424J600_PHCON1_r7        (1<<7)
#define ENC424J600_PHCON1_r6        (1<<6)
#define ENC424J600_PHCON1_r5        (1<<5)
#define ENC424J600_PHCON1_r4        (1<<4)
#define ENC424J600_PHCON1_r3        (1<<3)
#define ENC424J600_PHCON1_r2        (1<<2)
#define ENC424J600_PHCON1_r1        (1<<1)
#define ENC424J600_PHCON1_r0        (1)

// PHSTAT1 bits --------
#define ENC424J600_PHSTAT1_r15        ((unsigned short)1<<15)
#define ENC424J600_PHSTAT1_FULL100         ((unsigned short)1<<14)
#define ENC424J600_PHSTAT1_HALF100         ((unsigned short)1<<13)
#define ENC424J600_PHSTAT1_FULL10          ((unsigned short)1<<12)
#define ENC424J600_PHSTAT1_HALF10          ((unsigned short)1<<11)
#define ENC424J600_PHSTAT1_r10        ((unsigned short)1<<10)
#define ENC424J600_PHSTAT1_r9        ((unsigned short)1<<9)
#define ENC424J600_PHSTAT1_r8        ((unsigned short)1<<8)
#define ENC424J600_PHSTAT1_r7        (1<<7)
#define ENC424J600_PHSTAT1_r6        (1<<6)
#define ENC424J600_PHSTAT1_ANDONE          (1<<5)
#define ENC424J600_PHSTAT1_LRFAULT         (1<<4)
#define ENC424J600_PHSTAT1_ANABLE          (1<<3)
#define ENC424J600_PHSTAT1_LLSTAT          (1<<2)
#define ENC424J600_PHSTAT1_r1        (1<<1)
#define ENC424J600_PHSTAT1_EXTREGS         (1)

// PHANA bits ----------
#define ENC424J600_PHANA_ADNP        ((unsigned short)1<<15)
#define ENC424J600_PHANA_r14        ((unsigned short)1<<14)
#define ENC424J600_PHANA_ADFAULT           ((unsigned short)1<<13)
#define ENC424J600_PHANA_r12        ((unsigned short)1<<12)
#define ENC424J600_PHANA_ADPAUS1           ((unsigned short)1<<11)
#define ENC424J600_PHANA_ADPAUS0           ((unsigned short)1<<10)
#define ENC424J600_PHANA_r9        ((unsigned short)1<<9)
#define ENC424J600_PHANA_AD100FD           ((unsigned short)1<<8)
#define ENC424J600_PHANA_AD100        (1<<7)
#define ENC424J600_PHANA_AD10FD            (1<<6)
#define ENC424J600_PHANA_AD10        (1<<5)
#define ENC424J600_PHANA_ADIEEE4           (1<<4)
#define ENC424J600_PHANA_ADIEEE3           (1<<3)
#define ENC424J600_PHANA_ADIEEE2           (1<<2)
#define ENC424J600_PHANA_ADIEEE1           (1<<1)
#define ENC424J600_PHANA_ADIEEE0           (1)

// PHANLPA bits --------
#define ENC424J600_PHANLPA_LPNP            ((unsigned short)1<<15)
#define ENC424J600_PHANLPA_LPACK           ((unsigned short)1<<14)
#define ENC424J600_PHANLPA_LPFAULT     ((unsigned short)1<<13)
#define ENC424J600_PHANLPA_r12        ((unsigned short)1<<12)
#define ENC424J600_PHANLPA_LPPAUS1         ((unsigned short)1<<11)
#define ENC424J600_PHANLPA_LPPAUS0         ((unsigned short)1<<10)
#define ENC424J600_PHANLPA_LP100T4         ((unsigned short)1<<9)
#define ENC424J600_PHANLPA_LP100FD         ((unsigned short)1<<8)
#define ENC424J600_PHANLPA_LP100           (1<<7)
#define ENC424J600_PHANLPA_LP10FD          (1<<6)
#define ENC424J600_PHANLPA_LP10            (1<<5)
#define ENC424J600_PHANLPA_LPIEEE4         (1<<4)
#define ENC424J600_PHANLPA_LPIEEE3         (1<<3)
#define ENC424J600_PHANLPA_LPIEEE2         (1<<2)
#define ENC424J600_PHANLPA_LPIEEE1         (1<<1)
#define ENC424J600_PHANLPA_LPIEEE0         (1)

// PHANE bits ----------
#define ENC424J600_PHANE_r15        ((unsigned short)1<<15)
#define ENC424J600_PHANE_r14        ((unsigned short)1<<14)
#define ENC424J600_PHANE_r13        ((unsigned short)1<<13)
#define ENC424J600_PHANE_r12        ((unsigned short)1<<12)
#define ENC424J600_PHANE_r11        ((unsigned short)1<<11)
#define ENC424J600_PHANE_r10        ((unsigned short)1<<10)
#define ENC424J600_PHANE_r9        ((unsigned short)1<<9)
#define ENC424J600_PHANE_r8        ((unsigned short)1<<8)
#define ENC424J600_PHANE_r7        (1<<7)
#define ENC424J600_PHANE_r6        (1<<6)
#define ENC424J600_PHANE_r5        (1<<5)
#define ENC424J600_PHANE_PDFLT        (1<<4)
#define ENC424J600_PHANE_r3        (1<<3)
#define ENC424J600_PHANE_r2        (1<<2)
#define ENC424J600_PHANE_LPARCD            (1<<1)
#define ENC424J600_PHANA_LPANABL           (1)

// PHCON2 bits ---------
#define ENC424J600_PHCON2_r15        ((unsigned short)1<<15)
#define ENC424J600_PHCON2_r14        ((unsigned short)1<<14)
#define ENC424J600_PHCON2_EDPWRDN          ((unsigned short)1<<13)
#define ENC424J600_PHCON2_r12        ((unsigned short)1<<12)
#define ENC424J600_PHCON2_EDTHRES          ((unsigned short)1<<11)
#define ENC424J600_PHCON2_r10        ((unsigned short)1<<10)
#define ENC424J600_PHCON2_r9        ((unsigned short)1<<9)
#define ENC424J600_PHCON2_r8        ((unsigned short)1<<8)
#define ENC424J600_PHCON2_r7        (1<<7)
#define ENC424J600_PHCON2_r6        (1<<6)
#define ENC424J600_PHCON2_r5        (1<<5)
#define ENC424J600_PHCON2_r4        (1<<4)
#define ENC424J600_PHCON2_r3        (1<<3)
#define ENC424J600_PHCON2_FRCLNK           (1<<2)
#define ENC424J600_PHCON2_EDSTAT           (1<<1)
#define ENC424J600_PHCON2_r0        (1)

// PHSTAT2 bits ---------
#define ENC424J600_PHSTAT2_r15        ((unsigned short)1<<15)
#define ENC424J600_PHSTAT2_r14        ((unsigned short)1<<14)
#define ENC424J600_PHSTAT2_r13        ((unsigned short)1<<13)
#define ENC424J600_PHSTAT2_r12        ((unsigned short)1<<12)
#define ENC424J600_PHSTAT2_r11        ((unsigned short)1<<11)
#define ENC424J600_PHSTAT2_r10        ((unsigned short)1<<10)
#define ENC424J600_PHSTAT2_r9        ((unsigned short)1<<9)
#define ENC424J600_PHSTAT2_r8        ((unsigned short)1<<8)
#define ENC424J600_PHSTAT2_r7        (1<<7)
#define ENC424J600_PHSTAT2_r6        (1<<6)
#define ENC424J600_PHSTAT2_r5        (1<<5)
#define ENC424J600_PHSTAT2_PLRITY          (1<<4)
#define ENC424J600_PHSTAT2_r3        (1<<3)
#define ENC424J600_PHSTAT2_r2        (1<<2)
#define ENC424J600_PHSTAT2_r1        (1<<1)
#define ENC424J600_PHSTAT2_r0        (1)

// PHSTAT3 bits --------
#define ENC424J600_PHSTAT3_r15        ((unsigned short)1<<15)
#define ENC424J600_PHSTAT3_r14        ((unsigned short)1<<14)
#define ENC424J600_PHSTAT3_r13        ((unsigned short)1<<13)
#define ENC424J600_PHSTAT3_r12        ((unsigned short)1<<12)
#define ENC424J600_PHSTAT3_r11        ((unsigned short)1<<11)
#define ENC424J600_PHSTAT3_r10        ((unsigned short)1<<10)
#define ENC424J600_PHSTAT3_r9        ((unsigned short)1<<9)
#define ENC424J600_PHSTAT3_r8        ((unsigned short)1<<8)
#define ENC424J600_PHSTAT3_r7        (1<<7)
#define ENC424J600_PHSTAT3_r6        (1<<6)
#define ENC424J600_PHSTAT3_r5        (1<<5)
#define ENC424J600_PHSTAT3_SPDDPX2     (1<<4)
#define ENC424J600_PHSTAT3_SPDDPX1         (1<<3)
#define ENC424J600_PHSTAT3_SPDDPX0         (1<<2)
#define ENC424J600_PHSTAT3_r1        (1<<1)
#define PHSTAT3_r0        (1)



// ENC424J600 config
// - The SRAM buffer in the ENC424J600 is a bulk 12k word x 16 bit (24k) memory used for TX/RX packet buffering and general purpose storage by teh host microcontroller.
//
#define ENC424J600_RAMSIZE                        (24 * 1024)            // fixed size of SRAM
#define ENC424J600_CIRCULAR_RX_FIFO_BUFFER_SIZE   (18 * 1025)            // define 18k of the SRAM for receiver use (size must be even)
#define ENC424J600_TRANSMIT_BUFFER_START          (0)                    // all of the RAM can be used for transmit data
#define ENC424J600_USER_RAM_SIZE                  (24 * 1024)            // this area can be used for use data - set to zero is not required (size must be even)


static unsigned char ucPresentBank = 0;                                  // banks 0, 1, 2 or 3

static unsigned short usRxDataOffset;                                    // index in RAM marking where the next reception data will be located



#define ENC424J600_UNBANKED_ACCESS 0
#define ENC424J600_BANKED_ACCESS   1


// Issue a single byte command
//
static void enc424j600ExecuteOp0(unsigned char ucCommand)
{
#if defined KINETIS_KL
    ASSERT_ENC424J600_CS_LINE();                                         // assert CS
#else
    FLUSH_ENC424J600_SPI_FIFO_AND_FLAGS();                               // ensure that the SPI FIFOs are empty and the status flags are reset before starting
#endif
    WRITE_ENC424J600_SPI_LAST(ucCommand);                                // send command
    WAIT_ENC424J600_SPI_RECEPTION_END();                                 // wait until the command has been sent and the received byte is available
    (void)READ_ENC424J600_SPI_FLASH_DATA();                              // dummy read
    CLEAR_ENC424J600_SPI_RECEPTION_FLAG();                               // clear the receive flag
#if defined KINETIS_KL
    NEGATE_ENC424J600_CS_LINE();                                         // negate CS
#endif
#if defined _WINDOWS && !defined REMOTE_SIMULATION_INTERFACE
    fnRemoteSimulationInterface(REMOTE_ETH_INTERFACE, REMOTE_ETH_CMD_WITHOUT_DATA, &ucCommand, 1, 0);
#endif
}


static int fnChangeBank(unsigned short usAddress)
{
    unsigned char ucBank = (unsigned char)((usAddress >> 4) & 0x0e);     // extract the bank reference
    if (ucBank <= 0x06) {                                                // if a banked address
        if (ucBank != ucPresentBank) {                                   // if a change in bank is required
            if ((usAddress & 0x1f) >= 0x16) {                            // registers 0x16..0x1f, 0x36..0x3f, 0x56..0x5f and 0x76..0x7f can be addressed in banks 0,1,2 or 3
                return ENC424J600_BANKED_ACCESS;                         // banked address (no change of bank required for particular registers)
            }
            enc424j600ExecuteOp0((unsigned char)(ENC424J600_B0SEL + ucBank)); // set the new bank
            ucPresentBank = ucBank;                                      // set the new bank reference
        }
        return ENC424J600_BANKED_ACCESS;                                 // banked address
    }
    else {
        return ENC424J600_UNBANKED_ACCESS;                               // unbanked address
    }
}


// 2 byte register write returning 2 byte read value
//
static unsigned short enc424j600ExecuteOp16(unsigned char ucCommand, unsigned short usData)
{
    unsigned short usStatus;
#if defined KINETIS_KL
    ASSERT_ENC424J600_CS_LINE();                                         // assert CS
#else
    FLUSH_ENC424J600_SPI_FIFO_AND_FLAGS();                               // ensure that the SPI FIFOs are empty and the status flags are reset before starting
#endif
    WRITE_ENC424J600_SPI(ucCommand);                                     // send command
    WAIT_ENC424J600_SPI_RECEPTION_END();                                 // wait until the command has been sent and the received byte is available
    (void)READ_ENC424J600_SPI_FLASH_DATA();                              // dummy read
    CLEAR_ENC424J600_SPI_RECEPTION_FLAG();                               // clear the receive flag
    WRITE_ENC424J600_SPI((unsigned char)(usData));                       // send LSB
    WAIT_ENC424J600_SPI_RECEPTION_END();                                 // wait until the command has been sent and the received byte is available
    usStatus = READ_ENC424J600_SPI_FLASH_DATA();                         // read LSB or status
    CLEAR_ENC424J600_SPI_RECEPTION_FLAG();                               // clear the receive flag
    WRITE_ENC424J600_SPI_LAST((unsigned char)(usData >> 8));             // send MSB
    WAIT_ENC424J600_SPI_RECEPTION_END();                                 // wait until the command has been sent and the received byte is available
    usStatus |= (READ_ENC424J600_SPI_FLASH_DATA() << 8);                 // read MSB of status
    CLEAR_ENC424J600_SPI_RECEPTION_FLAG();                               // clear the receive flag
#if defined KINETIS_KL
    NEGATE_ENC424J600_CS_LINE();                                         // negate CS
#endif
#if defined _WINDOWS && !defined REMOTE_SIMULATION_INTERFACE             // if simulation remote interface in operation request the action and return the read value
    {
        unsigned char ucData[2];
        fnRemoteSimulationInterface(REMOTE_ETH_INTERFACE, REMOTE_ETH_CMD, &ucCommand, 1, 1); // send command byte
        ucData[0] = (unsigned char)(usData >> 8);
        ucData[1] = (unsigned char)(usData);
        usStatus = (unsigned short)fnRemoteSimulationInterface(REMOTE_ETH_INTERFACE, REMOTE_ETH_DATA, ucData, 2, 0); // send data and return the status
    }
#endif
    return usStatus;
}

// 3 byte register write returning 3 byte read value
//
static unsigned long enc424j600ExecuteOp24(unsigned char ucCommand, unsigned long data)
{
    unsigned long ulStatus;
#if defined _WINDOWS && !defined REMOTE_SIMULATION_INTERFACE
    unsigned char ucData[3];
#endif

#if defined KINETIS_KL
    ASSERT_ENC424J600_CS_LINE();                                         // assert CS
#else
    FLUSH_ENC424J600_SPI_FIFO_AND_FLAGS();                               // ensure that the SPI FIFOs are empty and the status flags are reset before starting
#endif

    WRITE_ENC424J600_SPI(ucCommand);                                     // send command
    WAIT_ENC424J600_SPI_RECEPTION_END();                                 // wait until the command has been sent and the received byte is available
    (void)READ_ENC424J600_SPI_FLASH_DATA();                              // dummy read
    CLEAR_ENC424J600_SPI_RECEPTION_FLAG();                               // clear the receive flag

    WRITE_ENC424J600_SPI((unsigned char)(data >> 16));                   // send first data byte
    WAIT_ENC424J600_SPI_RECEPTION_END();                                 // wait until the command has been sent and the received byte is available
    ulStatus = READ_ENC424J600_SPI_FLASH_DATA();
    CLEAR_ENC424J600_SPI_RECEPTION_FLAG();                               // clear the receive flag

    WRITE_ENC424J600_SPI((unsigned char)(data >> 8));                    // send second data byte
    WAIT_ENC424J600_SPI_RECEPTION_END();                                 // wait until the command has been sent and the received byte is available
    ulStatus |= (READ_ENC424J600_SPI_FLASH_DATA() << 8);
    CLEAR_ENC424J600_SPI_RECEPTION_FLAG();                               // clear the receive flag

    WRITE_ENC424J600_SPI_LAST((unsigned char)data);                      // send third and final data byte
    WAIT_ENC424J600_SPI_RECEPTION_END();                                 // wait until the command has been sent and the received byte is available
    ulStatus |= (READ_ENC424J600_SPI_FLASH_DATA() << 16);
    CLEAR_ENC424J600_SPI_RECEPTION_FLAG();                               // clear the receive flag

#if defined KINETIS_KL
    NEGATE_ENC424J600_CS_LINE();                                         // negate CS
#endif
#if defined _WINDOWS && !defined REMOTE_SIMULATION_INTERFACE
    fnRemoteSimulationInterface(REMOTE_ETH_INTERFACE, REMOTE_ETH_CMD, &ucCommand, 1, 1);
    ucData[0] = (unsigned char)(data >> 16);
    ucData[1] = (unsigned char)(data >> 8);
    ucData[2] = (unsigned char)(data);
    ulStatus = fnRemoteSimulationInterface(REMOTE_ETH_INTERFACE, REMOTE_ETH_DATA, ucData, 3, 0);
#endif
    return ulStatus;
}

// Register write (either unbanked or banked)
//
static void enc424j600WriteReg(unsigned short usAddress, unsigned short usData)
{
    if (fnChangeBank(usAddress) == ENC424J600_UNBANKED_ACCESS) {         // check whether the address is banked or unbanked - if banked, change bank if needed
        enc424j600ExecuteOp24(ENC424J600_WCRU, (unsigned long)(((unsigned long)usAddress << 16) | (unsigned short)((usData << 8) | (usData >> 8)))); // 3 byte unbanked address
    }
    else {
        enc424j600ExecuteOp16((unsigned char)(ENC424J600_WCR | (usAddress & 0x1f)), usData); // 2 byte banked address
    }
}

// Register read (either unbanked or banked)
//
static unsigned short enc424j600ReadReg(unsigned short usAddress)
{
    if (fnChangeBank(usAddress) == ENC424J600_UNBANKED_ACCESS) {         // check whether the address is banked or unbanked - if banked, change bank if needed
        return (unsigned short)enc424j600ExecuteOp24(ENC424J600_RCRU, (unsigned long)((unsigned long)usAddress << 16)); // 3 byte unbanked address
    }
    else {
        return enc424j600ExecuteOp16((unsigned char)(ENC424J600_RCR | (usAddress & 0x1f)), 0xffff); // 2 byte banked address
    }
}

static void enc424j600BFSReg(unsigned short usAddress, unsigned short usBitMask)
{
    fnChangeBank(usAddress);                                             // set the bank if a banked register and this bank is not presently selected
    enc424j600ExecuteOp16((unsigned char)(ENC424J600_BFS | (usAddress & 0x1f)), usBitMask);
}

static void enc424j600WriteN(unsigned char ucCommand, unsigned char *data, unsigned short dataLen)
{
#if defined _WINDOWS && !defined REMOTE_SIMULATION_INTERFACE
    unsigned short usTheDataLength = dataLen;
#endif

#if defined KINETIS_KL
    ASSERT_ENC424J600_CS_LINE();                                         // assert CS
#else
    FLUSH_ENC424J600_SPI_FIFO_AND_FLAGS();                               // ensure that the SPI FIFOs are empty and the status flags are reset before starting
#endif

    WRITE_ENC424J600_SPI(ucCommand);                                     // send command
    WAIT_ENC424J600_SPI_RECEPTION_END();                                 // wait until the command has been sent and the received byte is available
    (void)READ_ENC424J600_SPI_FLASH_DATA();                              // dummy read
    CLEAR_ENC424J600_SPI_RECEPTION_FLAG();                               // clear the receive flag

    while (dataLen--) {
        if (dataLen == 0) {
            WRITE_ENC424J600_SPI_LAST(*data);                            // send last data byte
        }
        else {
            WRITE_ENC424J600_SPI(*data);                                 // send data byte
        }
        data++;
        WAIT_ENC424J600_SPI_RECEPTION_END();                             // wait until the command has been sent and the received byte is available
        (void)READ_ENC424J600_SPI_FLASH_DATA();
        CLEAR_ENC424J600_SPI_RECEPTION_FLAG();                           // clear the receive flag
    }

#if defined KINETIS_KL
    NEGATE_ENC424J600_CS_LINE();                                         // negate CS
#endif
#if defined _WINDOWS && !defined REMOTE_SIMULATION_INTERFACE
    fnRemoteSimulationInterface(REMOTE_ETH_INTERFACE, REMOTE_ETH_CMD, &ucCommand, 1, 1);
    data -= usTheDataLength;
    fnRemoteSimulationInterface(REMOTE_ETH_INTERFACE, REMOTE_ETH_DATA_BUFFER, data, usTheDataLength, 0);
#endif
}

static void enc424j600BFCReg(unsigned short address, unsigned short bitMask)
{
    fnChangeBank(address);
    enc424j600ExecuteOp16((unsigned char)(ENC424J600_BFC | (address & 0x1F)), bitMask);
}

static void enc424j600ReadN(unsigned char ucCommand, unsigned char *data, unsigned short usLength)
{
#if defined _WINDOWS && !defined REMOTE_SIMULATION_INTERFACE
    unsigned short usThisLength = usLength;
#endif

#if defined KINETIS_KL
    ASSERT_ENC424J600_CS_LINE();                                         // assert CS
#else
    FLUSH_ENC424J600_SPI_FIFO_AND_FLAGS();                               // ensure that the SPI FIFOs are empty and the status flags are reset before starting
#endif

    WRITE_ENC424J600_SPI(ucCommand);                                     // send command
    WAIT_ENC424J600_SPI_RECEPTION_END();                                 // wait until the command has been sent and the received byte is available
    (void)READ_ENC424J600_SPI_FLASH_DATA();                              // dummy read
    CLEAR_ENC424J600_SPI_RECEPTION_FLAG();                               // clear the receive flag

    while (usLength--) {
        if (usLength == 0) {                                             // final byte
            WRITE_ENC424J600_SPI_LAST(0xff);                             // send final dummy byte
        }
        else {
            WRITE_ENC424J600_SPI(0xff);                                  // send dummy byte
        }
        WAIT_ENC424J600_SPI_RECEPTION_END();                             // wait until the command has been sent and the received byte is available
        *data++ = READ_ENC424J600_SPI_FLASH_DATA();                      // read returned data byte
        CLEAR_ENC424J600_SPI_RECEPTION_FLAG();                           // clear the receive flag
    }

#if defined KINETIS_KL
    NEGATE_ENC424J600_CS_LINE();                                         // negate CS
#endif
#if defined _WINDOWS && !defined REMOTE_SIMULATION_INTERFACE
    fnRemoteSimulationInterface(REMOTE_ETH_INTERFACE, REMOTE_ETH_CMD, &ucCommand, 1, 1);
    data -= usThisLength;
    fnRemoteSimulationInterface(REMOTE_ETH_INTERFACE, REMOTE_ETH_DATA_RX, data, usThisLength, 0);
#endif
}


// Read the MAC address from the device
//
static void enc424j600ReadMacAddr(unsigned char *macAddr)
{
    unsigned short regValue;
    regValue = enc424j600ReadReg(ENC424J600_MAADR1);
    *macAddr++ = (unsigned char)(regValue);
    *macAddr++ = (unsigned char)(regValue >> 8);
    regValue = enc424j600ReadReg(ENC424J600_MAADR2);
    *macAddr++ = (unsigned char)(regValue);
    *macAddr++ = (unsigned char)(regValue >> 8);
    regValue = enc424j600ReadReg(ENC424J600_MAADR3);
    *macAddr++ = (unsigned char)(regValue);
    *macAddr++ = (unsigned char)(regValue >> 8);
}

// Write the MAC address to the device
//
static void enc424j600SetMacAddr(unsigned char *macAddr)
{
    unsigned short regValue;
    regValue = ((*(macAddr + 1) << 8) | *macAddr);
    macAddr += 2;
    enc424j600WriteReg(ENC424J600_MAADR1, regValue);
    regValue = ((*(macAddr + 1) << 8) | *macAddr);
    macAddr += 2;
    enc424j600WriteReg(ENC424J600_MAADR2, regValue);
    regValue = ((*(macAddr + 1) << 8) | *macAddr);
    enc424j600WriteReg(ENC424J600_MAADR3, regValue);
}

static void enc424j600WritePHYReg(unsigned char address, unsigned short Data)
{
    enc424j600WriteReg(ENC424J600_MIREGADR, (unsigned short)(0x0100 | address)); // write the register address
    enc424j600WriteReg(ENC424J600_MIWR, Data);                           // write the data
    while (enc424j600ReadReg(ENC424J600_MISTAT) & ENC424J600_MISTAT_BUSY) {} // Wait until the PHY register has been written
}


static void enc424j600MACFlush(void)
{
    // Check to see if the duplex status has changed.  This can
    // change if the user unplugs the cable and plugs it into a
    // different node.  Auto-negotiation will automatically set
    // the duplex in the PHY, but we must also update the MAC
    // inter-packet gap timing and duplex state to match.
    if (enc424j600ReadReg(ENC424J600_EIR) & ENC424J600_EIR_LINKIF) {
        unsigned short w;
        enc424j600BFCReg(ENC424J600_EIR, ENC424J600_EIR_LINKIF);

        // Update MAC duplex settings to match PHY duplex setting
        w = enc424j600ReadReg(ENC424J600_MACON2);
        if (enc424j600ReadReg(ENC424J600_ESTAT) & ENC424J600_ESTAT_PHYDPX) {
            // Switching to full duplex
            enc424j600WriteReg(ENC424J600_MABBIPG, 0x15);
            w |= ENC424J600_MACON2_FULDPX;
        } else {
            // Switching to half duplex
            enc424j600WriteReg(ENC424J600_MABBIPG, 0x12);
            w &= ~ENC424J600_MACON2_FULDPX;
        }
        enc424j600WriteReg(ENC424J600_MACON2, w);
    }

    // Start the transmission, but only if we are linked.
    if (enc424j600ReadReg(ENC424J600_ESTAT) & ENC424J600_ESTAT_PHYLNK)
        enc424j600BFSReg(ENC424J600_ECON1, ENC424J600_ECON1_TXRTS);
}




// Perform a system reset in accordance with the ENC424J600 recommendations
//
static int fn_ENC424J600_reset(void)
{
    int iMonitor0;
    int iMonitor1 = 0;
    do {
        if (++iMonitor1 > 10) {
            return -1;                                                   // hardware error - can't continue
        }
        iMonitor0 = 0;
        // Write 1234h to EUDAST and read it back to ensure that this value can be successfully set
        // Repeat a few times if not since the interface may not yet be ready
        //
        do {
            enc424j600WriteReg(ENC424J600_EUDAST, 0x1234);               // write a test value
            if (++iMonitor0 > 100) {                                     // check maxium attempts
#if defined _WINDOWS
                break;
#else
                return -2;                                               // hardware error - can't continue
#endif
            }
        } while (enc424j600ReadReg(ENC424J600_EUDAST) != 0x1234);        // check that the value written to the register could be read back
        // Poll CLKRDY and wait for it to become set, then issue the system reset command
        //
        iMonitor0 = 0;
        while ((enc424j600ReadReg(ENC424J600_ESTAT) & ENC424J600_ESTAT_CLKRDY) == 0) { // read the Ethernet status register and ensure that the clock is ready
            if (++iMonitor0 > 100) {
#if defined _WINDOWS
                break;
#else
                return -3;                                               // hardware error - can't continue
#endif
            }
        }
        enc424j600BFSReg(ENC424J600_ECON2, ENC424J600_ECON2_ETHRST);     // command a reset of the Ethernet controller by setting ENC424J600_ECON2_ETHRST in ENC424J600_ECON2
        ucPresentBank = 0;                                               // reset the bank reference since it was reset in the device
        // Wait at least 25us for the reset to take place and the interface to being operating again
        //
        fnDelayLoop(35);                                                 // the reset operation requires 25us to complete, during which time no access is possible
        // Read EUDAST to confirm that the system reset took place, whereby EUDAST should have reverted back to 0000h
        //
        //
    } while (enc424j600ReadReg(ENC424J600_EUDAST) != 0x0000);            // verify that the reset was been successful by checking that the written pattern has been reset
    // Wait at least 256us for the PHY registers and PHY status bits to become available
    //
    fnDelayLoop(300);
    enc424j600WriteReg(ENC424J600_ECON2, (ENC424J600_ECON2_ETHEN | ENC424J600_ECON2_STRCH | ENC424J600_ECON2_COCON_OFF)); // enable Ethernet with LED stretching but disabled the CLKOUT
    return 0;                                                            // successful
}

// Interrupt due to receive packet pending
//
static void fn_ENC424J600_interrupt(void)
{
    #if defined REMOTE_SIMULATION_INTERFACE
    unsigned short usStatus;
    usStatus = enc424j600ReadReg(ENC424J600_EIR);
    RemoteSimUDP_Frame.ucUDP_Message[0] = REMOTE_ETH_INTERFACE;
    RemoteSimUDP_Frame.ucUDP_Message[1] = REMOTE_ETH_INTERRUPT;
    RemoteSimUDP_Frame.ucUDP_Message[2] = 0;
    RemoteSimUDP_Frame.ucUDP_Message[3] = 2;
    RemoteSimUDP_Frame.ucUDP_Message[4] = (unsigned char)(usStatus >> 8);
    RemoteSimUDP_Frame.ucUDP_Message[5] = (unsigned char)usStatus;
    fnSendUDP(RemoteSimUDPSocket, ucRemoteSimIP_address, REMOTE_SIM_UDP_PORT, (unsigned char *)&RemoteSimUDP_Frame.tUDP_Header, 6, 0); // send to the simulator
    #else
        #if defined ETHERNET_RELEASE_AFTER_EVERY_FRAME
    uTaskerStateChange(TASK_ETHERNET, UTASKER_ACTIVATE);                 // schedule the Ethernet task
    iENC424J600_frames++;                                                // the number of frames received
        #else
            #error "ETHERNET_RELEASE_AFTER_EVERY_FRAME should be used together with the ENC424J600"
        #endif
    #endif
}

// Configure an interrupt line for the external Ethernet device
//
static void fnConfigureENC424J600Interrupt(void)
{
    INTERRUPT_SETUP interrupt_setup;                                     // interrupt configuration parameters
    interrupt_setup.int_type       = PORT_INTERRUPT;                     // identifier to configure port interrupt
    interrupt_setup.int_handler    = fn_ENC424J600_interrupt;            // handling function
    interrupt_setup.int_priority   = ENC424J600_IRQ_PRIORITY;            // interrupt priority level
    interrupt_setup.int_port       = ENC424J600_IRQ_PORT;                // the port that the interrupt input is on
    interrupt_setup.int_port_bits  = ENC424J600_IRQ;                     // the IRQ input connected
    interrupt_setup.int_port_sense = (IRQ_FALLING_EDGE | PULLUP_ON);     // interrupt is to be falling edge sensitive
    fnConfigureInterrupt((void *)&interrupt_setup);                      // configure interrupt input
}
#endif





#if defined ENC424J600_INTERFACE                                         // temp to allow external Ethernet controller instead of internal one

#if defined _MAGIC_RESET_FRAME
extern void _fnEnableMagicFrames(int iOnOff)
{
}
#endif



#if defined PHY_MULTI_PORT                                               // {18}
extern unsigned short _fnMIIread_(unsigned char ucPort, unsigned char _mradr)
{
    return 0;
}

extern void _fnMIIwrite_(unsigned char ucPort, unsigned char _mradr, unsigned short _mwdata)
{
}
#else
extern unsigned short _fnMIIread_(unsigned char _mradr)
{
    return 0;
}

extern void _fnMIIwrite_(unsigned char _mradr, unsigned short _mwdata)
{

}
#endif




#if defined PHY_POLL_LINK || defined PHY_INTERRUPT                       // {37}
extern void _fnCheckEthLinkState(void)
{
}
#endif



// This can be called by the ethernet driver to get the address of the corresponding Ethernet receive buffer
//
extern unsigned char *_fnGetRxBufferAdd(int iBufNr)
{
    return 0;
}

#if defined USE_IGMP || defined USE_IPV6
static unsigned char fnCalculateHash(unsigned char *ptrMac)
{
    #define CRC_POLY 0xedb88320                                          // CRC-32 polynomial in reverse direction
    unsigned long ulCRC32 = 0xffffffff;
    int i, iBit;
    unsigned char ucByte;
	for (i = 0; i < MAC_LENGTH; i++) {
        ucByte = *ptrMac++;
        for (iBit = 0; iBit < 8; iBit++) {
            if ((ulCRC32 ^ ucByte) & 1) {
                ulCRC32 = (ulCRC32 >> 1) ^ CRC_POLY;
            }
            else {
                ulCRC32 >>= 1;
            }
            ucByte >>= 1;
        }
    }
    ulCRC32 >>= 26;                                                      // the upper 6 bits of the CRC32 determine the hash entry location
    return (unsigned char)ulCRC32;
}

// Enable or disable a single hash entry
//
static void fnEnableDisableMulticastHash(unsigned char ucHashRef, int iEnable)
{
    unsigned short usHashReg = ENC424J600_EHT1L;
    while (ucHashRef > 15) {
        ucHashRef -= 16;
        usHashReg += 2;                                                  // move to the appropriate hash register
    }
    if (iEnable != 0) {
        enc424j600BFSReg(usHashReg, (1 << ucHashRef));                   // enable the hash entry
        enc424j600BFSReg(ENC424J600_ERXFCON, ENC424J600_ERXFCON_HTEN);   // ensure the hash table collection filter is enabled
    }
    else {
        enc424j600BFCReg(usHashReg, (1 << ucHashRef));                   // disable the hash entry
    }
}
#endif

#if defined USE_IGMP
    #if defined USE_IPV6
    #define MAX_MULTICAST_FILTERS (IGMP_MAX_HOSTS + 2)
    #else
    #define MAX_MULTICAST_FILTERS (IGMP_MAX_HOSTS + 1)
    #endif
static unsigned char ucGroupHashes[MAX_MULTICAST_FILTERS] = {0};

static void fnModifyMulticastFilterENC424J600(QUEUE_TRANSFER action, unsigned char *ptrIP)
{
    int iFree = -1;
    int iMatch = -1;
    int i;
    int iHashCount = 0;
    unsigned char ucMac[MAC_LENGTH];
    unsigned char ucHashRef;
    switch (action) {
    case REMOVE_IPV4_FILTER:
    case ADD_IPV4_FILTER:                                                // a filter has to be enabled for this IPv4 address
        ucMac[0] = 0x01;                                                 // IANA owned multicast ethernet address block
        ucMac[1] = 0x00;
        ucMac[2] = 0x5e;                                                 // enter the multicast MAC address belonging to IANA for this purpose
        ucMac[3] = (*(ptrIP + 1) & 0x7f);                                // add 23 bits of the destination IP address
        ucMac[4] = *(ptrIP + 2);
        ucMac[5] = *(ptrIP + 3);
        ucHashRef = fnCalculateHash(ucMac);
        break;
    default:                                                             // ignore any other calls
        return;
    }
    for (i = 0; i < MAX_MULTICAST_FILTERS; i++) {                        // check whether the entry exists
        if (ucGroupHashes[i] == ucHashRef) {                             // existing hash entry found
            iMatch = i;
            iHashCount++;                                                // count the number of groups requiring this hash
        }
        else if (ucGroupHashes[i] == 0) {                                // free location
            iFree = i;                                                   // remember the free entry
        }
    }
    if (action == ADD_IPV4_FILTER) {                                     // the entry doesn't exist so we add it
        if (iFree < 0) {
            _EXCEPTION("No more hash entry space");
        }
        else {
            ucGroupHashes[iFree] = ucHashRef;                            // note that this hash is required
            fnEnableDisableMulticastHash(ucHashRef, 1);                  // enable the hash entry
        }
    }
    else if (action == REMOVE_IPV4_FILTER) {
        if (iMatch >= 0) {                                               // remove an entry for this hash
            ucGroupHashes[iMatch] = 0;                                   // entry free
            if (iHashCount == 1) {                                       // single entry so it can be disabled
                fnEnableDisableMulticastHash(ucHashRef, 0);
            }
        }
    }
}
#endif

#if defined USE_IPV6
// Configure a multicast address for IPV6 use
//
static void fnConfigureMulticastIPV6(void)
{
    unsigned char ucHashRef;
    unsigned char ucMac[MAC_LENGTH];                                     // set hash according to the multicast address 0x33 0x33 0xff XX XX XX (where XX XX XX are the last three bytes of the IPV6 IP address) as used by ND (neighbor discovery)
    ucMac[0] = 0x33;
    ucMac[1] = 0x33;
    ucMac[2] = 0xff;
    ucMac[3] = ucLinkLocalIPv6Address[ENC424J600_NETWORK][3 + (IPV6_LENGTH - MAC_LENGTH)];
    ucMac[4] = ucLinkLocalIPv6Address[ENC424J600_NETWORK][4 + (IPV6_LENGTH - MAC_LENGTH)];
    ucMac[5] = ucLinkLocalIPv6Address[ENC424J600_NETWORK][5 + (IPV6_LENGTH - MAC_LENGTH)];
    ucHashRef = fnCalculateHash(ucMac);
    fnEnableDisableMulticastHash(ucHashRef, 1);
    #if defined USE_IGMP
    ucGroupHashes[0] = ucHashRef;                                        // mark that this hash is required by IPv6
    #endif
}
#endif


#if !defined REMOTE_SIMULATION_INTERFACE
// Ethernet configuration routine
//
static int fnConfigENC424J600(ETHTABLE *pars)
{
    CONFIGURE_ENC424J600_SPI_MODE();                                     // prepare the SPI for use

    if (fn_ENC424J600_reset() != 0) {                                    // command a reset of the device
        return -1;                                                       // hardware initialisation failed
    }

    // Initialize RX tracking variables and other control state flags
    //
    usRxDataOffset = (unsigned short)(ENC424J600_RAMSIZE - ENC424J600_CIRCULAR_RX_FIFO_BUFFER_SIZE);

    // Set up TX/RX/UDA buffer addresses
    // Set up the RAM buffer areas
    // - the receive logic is presently disable, making these writable
    //
    enc424j600WriteReg(ENC424J600_ETXST,  ENC424J600_TRANSMIT_BUFFER_START); // start of transmit buffer area
    enc424j600WriteReg(ENC424J600_ERXST,  usRxDataOffset);               // start of the circular receive buffer area
    enc424j600WriteReg(ENC424J600_ERXTAIL,(ENC424J600_RAMSIZE - 2));     // end of the circular receive buffer area
    enc424j600WriteReg(ENC424J600_EUDAST, (unsigned short)(ENC424J600_RAMSIZE)); // these (user area) can be modified at any time and could be changed during operation (even address) - initially allowing access to the entire SRAM
    enc424j600WriteReg(ENC424J600_EUDAND, (unsigned short)(ENC424J600_RAMSIZE + 1)); // odd address

    if (pars->usMode & PROMISCUOUS) {
        enc424j600WriteReg(ENC424J600_ERXFCON, (ENC424J600_ERXFCON_CRCEN | ENC424J600_ERXFCON_RUNTEN | ENC424J600_ERXFCON_UCEN | ENC424J600_ERXFCON_NOTMEEN | ENC424J600_ERXFCON_MCEN)); // no destination address checking
    }
    #if defined USE_IPV6
    else if (pars->usMode & CON_MULTICAST) {                             // if multicast reception is to be configured
        fnConfigureMulticastIPV6();                                      // set IPV6 multicast address
    }
    if (pars->usMode & USE_FACTORY_MAC) {                                // the ENC424J600 has a unique MAC address (eg. 00 04 a3 11 d2 43, where the 00 04 a3 part is registered to Microchip)
        enc424j600ReadMacAddr(network[ENC424J600_NETWORK].ucOurMAC);     // read the MAC from the chip so that the caller can adjust its reference if required - the value will be used for hardware operation
    }
    else {                                                               // else overwrite the MAC address
        enc424j600SetMacAddr(network[ENC424J600_NETWORK].ucOurMAC);      // overwrite the pre-defined address
    }
    #else
    if (pars->usMode & USE_FACTORY_MAC) {                                // the ENC424J600 has a unique MAC address (eg. 00 04 a3 11 d2 43, where the 00 04 a3 part is registered to Microchip)
        enc424j600ReadMacAddr(pars->ucMAC);                              // read the MAC from the chip so that the caller can adjust its reference if required - the value will be used for hardware operation
    }
    else {                                                               // else overwrite the MAC address
        enc424j600SetMacAddr(pars->ucMAC);                               // overwrite the pre-defined address
    }
    #endif

    // Set PHY Auto-negotiation to support 10BaseT Half duplex,
    // 10BaseT Full duplex, 100BaseTX Half Duplex, 100BaseTX Full Duplex,
    // and symmetric PAUSE capability
    //
    enc424j600WritePHYReg(ENC424J600_PHANA, ENC424J600_PHANA_ADPAUS0 | ENC424J600_PHANA_AD10FD | ENC424J600_PHANA_AD10 | ENC424J600_PHANA_AD100FD | ENC424J600_PHANA_AD100 | ENC424J600_PHANA_ADIEEE0);
    enc424j600BFSReg(ENC424J600_ECON1, ENC424J600_ECON1_RXEN);           // enable packet reception
    fnConfigureENC424J600Interrupt();                                    // configure the interrupt input from the controller
    enc424j600WriteReg(ENC424J600_EIE, (ENC424J600_EIE_INTIE | ENC424J600_EIE_PKTIE)); // enable interrupt sources (reception interrupt pending)
    return 0;                                                            // initialisation was sucessful
}

// This can be called by the ethernet driver to find out how many receive buffers the Ethernet Controller has
//
static int fnGetQuantityENC424J600RxBuf(void)
{
    return 1;
}


// This can be called by the ethernet driver to get the address of the corresponding Ethernet transmit buffer
//
static unsigned char *fnGetENC424J600TxBufferAdd(int iBufNr)
{
    return 0;
}

// This can be called by the Ethernet driver to wait a short time for the transmit buffer to become free
//
static int fnWaitENC424J600TxFree(void)
{
    return 0;
}

static void fnPutInBuffer_ENC424J600(unsigned char *ptrOut, unsigned char *ptrIn, QUEUE_TRANSFER nr_of_bytes)
{
    BACKUP_SPI();                                                        // if the SPI interface is being shared we back up the settings and set our own
    if (ptrOut == 0) {                                                   // first write to this frame
        enc424j600WriteReg(ENC424J600_EGPWRPT, ENC424J600_TRANSMIT_BUFFER_START); // start at the beginning of the transmit buffer space
    }
    enc424j600WriteN(ENC424J600_WGPDATA, ptrIn, nr_of_bytes);            // copy the frame to the output buffer
    RETURN_SPI();                                                        // return original settings
}

// Data has been prepared, now send it over the ETHERNET interface
//
static QUEUE_TRANSFER fnStartEthTxENC424J600(QUEUE_TRANSFER DataLen, unsigned char *ptr_put)
{
    BACKUP_SPI();                                                        // if the SPI interface is being shared we back up the settings and set our own
    enc424j600WriteReg(ENC424J600_ETXLEN, DataLen);                      // the frame length to be transmitted
    enc424j600MACFlush();
    RETURN_SPI();                                                        // return original settings
    return (DataLen);
}

// This can be called by the ethernet driver to free an Ethernet receive buffer after it has been used
//
static void fnFreeEthernetBufferENC424J600(int iBufNr)
{
}

typedef struct _PACK stENC424J600_RECEIVE_STATUS_VECTOR
{
    unsigned short usNextDataOffset;
    unsigned short usReceiveByteCount;
    unsigned long  ulFrameDetails;
} ENC424J600_RECEIVE_STATUS_VECTOR;

#if defined _LITTLE_ENDIAN || defined _WINDOWS
    #define ENC424J600_RECEIVE_DETAILS_PPI        0x00000001             // packet previously ignored
    #define ENC424J600_RECEIVE_DETAILS_CVPS       0x00000004             // carrier event previously seen
    #define ENC424J600_RECEIVE_DETAILS_CRC_ERROR  0x00000010             // this frame has a CRC error
    #define ENC424J600_RECEIVE_DETAILS_LCE        0x00000020             // this frame has a length check error
    #define ENC424J600_RECEIVE_DETAILS_LOR        0x00000040             // this frame has larger than 1500 bytes type field
    #define ENC424J600_RECEIVE_DETAILS_RX_OK      0x00000080             // this frame has valid contents
    #define ENC424J600_RECEIVE_DETAILS_RMP        0x00000100             // this frame has been received as a valid multicast frame
    #define ENC424J600_RECEIVE_DETAILS_RBP        0x00000200             // this frame has been received as a valid broadcast frame
    #define ENC424J600_RECEIVE_DETAILS_DN         0x00000400             // dribble nibble discarded after frame
    #define ENC424J600_RECEIVE_DETAILS_RCF        0x00000800             // this frame was recognised as a control frame
    #define ENC424J600_RECEIVE_DETAILS_RPCF       0x00001000             // this frame was recognised as a pause control frame
    #define ENC424J600_RECEIVE_DETAILS_RUOP       0x00002000             // this frame was recognised as an unknown control frame
    #define ENC424J600_RECEIVE_DETAILS_RVLAN      0x00004000             // this frame was recognised as a VLAN tagged frame
    #define ENC424J600_RECEIVE_DETAILS_RFM        0x00008000             // this frame met the criteria for the runt packet receive filter
    #define ENC424J600_RECEIVE_DETAILS_NMFM       0x00010000             // this frame met the criteria for the not-me receive filter
    #define ENC424J600_RECEIVE_DETAILS_HFM        0x00020000             // this frame met the criteria for the hash receive filter
    #define ENC424J600_RECEIVE_DETAILS_MPFM       0x00040000             // this frame met the criteria for the magic packet receive filter
    #define ENC424J600_RECEIVE_DETAILS_PMFM       0x00080000             // this frame met the criteria for the pattern match receive filter
    #define ENC424J600_RECEIVE_DETAILS_UFM        0x00100000             // this frame met the criteria for the unicast receive filter
#else
    #define ENC424J600_RECEIVE_DETAILS_PPI        0x01000000             // packet previously ignored
    #define ENC424J600_RECEIVE_DETAILS_CVPS       0x04000000             // carrier event previously seen
    #define ENC424J600_RECEIVE_DETAILS_CRC_ERROR  0x10000000             // this frame has a CRC error
    #define ENC424J600_RECEIVE_DETAILS_LCE        0x20000000             // this frame has a length check error
    #define ENC424J600_RECEIVE_DETAILS_LOR        0x40000000             // this frame has larger than 1500 bytes type field
    #define ENC424J600_RECEIVE_DETAILS_RX_OK      0x80000000             // this frame has valid contents
    #define ENC424J600_RECEIVE_DETAILS_RMP        0x00010000             // this frame has been received as a valid multicast frame
    #define ENC424J600_RECEIVE_DETAILS_RBP        0x00020000             // this frame has been received as a valid broadcast frame
    #define ENC424J600_RECEIVE_DETAILS_DN         0x00040000             // dribble nibble discarded after frame
    #define ENC424J600_RECEIVE_DETAILS_RCF        0x00080000             // this frame was recognised as a control frame
    #define ENC424J600_RECEIVE_DETAILS_RPCF       0x00100000             // this frame was recognised as a pause control frame
    #define ENC424J600_RECEIVE_DETAILS_RUOP       0x00200000             // this frame was recognised as an unknown control frame
    #define ENC424J600_RECEIVE_DETAILS_RVLAN      0x00400000             // this frame was recognised as a VLAN tagged frame
    #define ENC424J600_RECEIVE_DETAILS_RFM        0x00800000             // this frame met the criteria for the runt packet receive filter
    #define ENC424J600_RECEIVE_DETAILS_NMFM       0x00000100             // this frame met the criteria for the not-me receive filter
    #define ENC424J600_RECEIVE_DETAILS_HFM        0x00000200             // this frame met the criteria for the hash receive filter
    #define ENC424J600_RECEIVE_DETAILS_MPFM       0x00000400             // this frame met the criteria for the magic packet receive filter
    #define ENC424J600_RECEIVE_DETAILS_PMFM       0x00000800             // this frame met the criteria for the patter match receive filter
    #define ENC424J600_RECEIVE_DETAILS_UFM        0x00001000             // this frame met the criteria for the unicast receive filter
#endif

#define ENC424J600_DEBUG_ENTRIES   10
ENC424J600_RECEIVE_STATUS_VECTOR DebugInfo[ENC424J600_DEBUG_ENTRIES];
int iDebugCnt = 0;

static void fnWriteStatus(ENC424J600_RECEIVE_STATUS_VECTOR *ptrNewStatus)
{
    if (iDebugCnt < ENC424J600_DEBUG_ENTRIES) {
        uMemcpy(&DebugInfo[iDebugCnt++], ptrNewStatus, sizeof(ENC424J600_RECEIVE_STATUS_VECTOR));
    }
}

// This is called when an Ethernet event has been received. Generally it is due to a reception and returns channel and message details.
//
static signed char fnEthernetEventENC424J600(ETHERNET_FRAME *rx_frame)
{
    // Read the interrupt register
    //
    int iReturn = -1;                                                    // no packet waiting
    unsigned short usStatus;
    BACKUP_SPI();                                                        // if the SPI interface is being shared we back up the settings and set our own
    usStatus = enc424j600ReadReg(ENC424J600_EIR);                        // check the interrupt register
    if (usStatus & ENC424J600_EIR_PKTIF) {                               // a reception packet is available
        static unsigned char ucRxInputBuffer[1600];                      // local static reception buffer to copy received frames to
        ENC424J600_RECEIVE_STATUS_VECTOR ReceiveStatusVector;
        unsigned short newRXTail;
        enc424j600BFCReg(ENC424J600_EIE, ENC424J600_EIE_INTIE);          // temporarily disable further interrupts, which causes the interrupt pin to return to the non-asserted high state
        enc424j600BFCReg(ENC424J600_EIR, ENC424J600_EIR_PKTIF);          // clear the interrupt
        enc424j600WriteReg(ENC424J600_ERXRDPT, usRxDataOffset);          // set pointer to the location of next reception data to be retrieved
        enc424j600ReadN(ENC424J600_RRXDATA, (unsigned char *)&ReceiveStatusVector, sizeof(ReceiveStatusVector)); // load the receive status vector (at head of packet)
        usRxDataOffset = LITTLE_SHORT_WORD(ReceiveStatusVector.usNextDataOffset); // read the reception header to determine where the next reception frame will be located
fnWriteStatus(&ReceiveStatusVector); // learning the details...
        rx_frame->frame_size = LITTLE_SHORT_WORD(ReceiveStatusVector.usReceiveByteCount); // extract the length of the present frame (including CRC-32)
        enc424j600ReadN(ENC424J600_RRXDATA, ucRxInputBuffer, rx_frame->frame_size); // read the frame reception content
        rx_frame->ptEth = (ETHERNET_FRAME_CONTENT *)ucRxInputBuffer;     // set the ethernet reception pointer to the static buffer
        if (usRxDataOffset == (unsigned short)(ENC424J600_RAMSIZE - ENC424J600_CIRCULAR_RX_FIFO_BUFFER_SIZE)) { // special case due to buffer wrap-around
            newRXTail = (ENC424J600_RAMSIZE - 2);                        // set the tail to the top of the receive circular buffer (even aligned)
        }
        else {
            newRXTail = (usRxDataOffset - 2);
        }
        enc424j600BFSReg(ENC424J600_ECON1, ENC424J600_ECON1_PKTDEC);     // decrement the packet count since we have extracted the last fra,e
        enc424j600WriteReg(ENC424J600_ERXTAIL, newRXTail);               // write the new rx tail location
        enc424j600BFSReg(ENC424J600_EIE, ENC424J600_EIE_INTIE);          // enable further interrupts by setting the global interrupt enable
        if (ReceiveStatusVector.ulFrameDetails & ENC424J600_RECEIVE_DETAILS_RX_OK) { // check whether the receptio was a valid frame
            rx_frame->frame_size -= 4;                                   // remove the trailing CRC-32 from the length
    #if IP_INTERFACE_COUNT > 1
            rx_frame->ucInterface = (ENC424J00_INTERFACE >> INTERFACE_SHIFT);
            rx_frame->ucInterfaceHandling = INTERFACE_NO_RX_CS_OFFLOADING; // this interface doesn't support IP checksum offloading
    #endif
            iReturn = 0;                                                 // channel received from 0 (fixed value for compatibility with other devices)
        }
    }
    RETURN_SPI();
    return iReturn;                                                      // return the result (non-negative means that a valid frame is available)
}

#if !defined ETHERNET_AVAILABLE || defined NO_INTERNAL_ETHERNET || ETHERNET_INTERFACES > 1
static CHAR fnEthernetMuxEvent(ETHERNET_FRAME *ptrFrame, int *ptr_iEthernetInterface)
{
    #if !defined ETHERNET_AVAILABLE || defined NO_INTERNAL_ETHERNET
    return fnEthernetEventENC424J600(ptrFrame);
    #else
    static int iToggle = 0;
    CHAR cEthernetBuffer;
    if (iToggle == 0) {
        cEthernetBuffer = fnEthernetEvent((unsigned char *)&ucEMAC_RX_INTERRUPT, ptrFrame); // check the internal Ethernet controller
        if (cEthernetBuffer >= 0) {
            *ptr_iEthernetInterface = 0;
            iToggle = 1;                                                 // next time check the other Ethernet interface first
        }
        else if (iENC424J600_frames != 0) {
            cEthernetBuffer = fnEthernetEventENC424J600(ptrFrame);
            iENC424J600_frames--;
            *ptr_iEthernetInterface = 1;
        }
    }
    else {
        if (iENC424J600_frames != 0) {
            cEthernetBuffer = fnEthernetEventENC424J600(ptrFrame);
            iENC424J600_frames--;
            iToggle = 0;                                                 // next time check the other Ethernet interface first
            *ptr_iEthernetInterface = 1;
        }
        else {
            cEthernetBuffer = fnEthernetEvent((unsigned char *)&ucEMAC_RX_INTERRUPT, ptrFrame); // check the internal Ethernet controller
            if (cEthernetBuffer >= 0) {
                *ptr_iEthernetInterface = 0;
            }
        }
    }
    return cEthernetBuffer;
    #endif
}
#endif
#endif
#endif
#endif                                                                   // endif ETH_INTERFACE
