/**********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher

    ---------------------------------------------------------------------
    File:      icmp.c
    Project:   Single Chip Embedded Internet
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2016
    *********************************************************************
    15.03.2010 Extended to include ICMPV6 (USE_IPV6) to handle neighbor solicitation/advertisement and pings
    06.09.2010 Optimise code when the device performs rx or tx offloading {1}
    25.03.2012 Use IPv6 link-local address for network neighbor protocol {2}
    08.08.2012 Support sockets wider than bytes                          {3}
    15.08.2012 Add ICMP_PING_IP_RESULT                                   {4}
    09.09.2012 Protect ping result queue if sub-net ping is supported    {5}
    30.01.2013 Add VLAN to ping-response socket                          {6}
    04.09.2014 Allow ping response from multiple networks when broadcast or subnet broadcast received {7}
    19.11.2014 Modify fnSendIPV6Discovery() prototype and add multiple interface support {8}
    01.09.2015 Never respond to ping when addressed on a multi.cast address {9}

*/
          
#include "config.h"


/* =================================================================== */
/*                          local definitions                          */
/* =================================================================== */

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
/*                 local function prototype declarations               */
/* =================================================================== */

#if (defined USE_IPV6 || defined USE_IP) && defined ICMP_SEND_PING
    #if defined ICMP_PING_IP_RESULT                                      // {4}
    static void fnReportPing(UTASK_TASK Task, USOCKET *Socket, unsigned char ucResult, unsigned char *ptrIP);
    #else
    static void fnReportPing(UTASK_TASK Task, USOCKET *Socket, unsigned char ucResult); // {3}
    #endif
#endif


#if defined USE_ICMP && defined USE_IP                                   // IPV4 ICMP

#if (IP_INTERFACE_COUNT > 1 && defined IP_INTERFACE_WITHOUT_CS_OFFLOADING) || (defined IPV4_SUPPORT_RX_DEFRAGMENTATION && defined IP_RX_CHECKSUM_OFFLOAD && !defined IP_INTERFACE_WITHOUT_CS_OFFLOADING)
static unsigned short fnRxCalcIP_CS(ETHERNET_FRAME *ptrRx_frame, unsigned char *ptrData, unsigned short usLen)
{
    #if (IP_INTERFACE_COUNT > 1 && defined IP_INTERFACE_WITHOUT_CS_OFFLOADING)
    if ((ptrRx_frame->ucInterfaceHandling & INTERFACE_NO_RX_CS_OFFLOADING) != 0) { // this interface doesn't support checksum offloading so the calculation is performed in software
        return fnCalcIP_CS(0, ptrData, usLen);
    }
    #endif
    #if (defined IPV4_SUPPORT_RX_DEFRAGMENTATION && defined IP_RX_CHECKSUM_OFFLOAD)
    if ((ptrRx_frame->ucSpecialHandling & INTERFACE_RX_PAYLOAD_CS_FRAGS) != 0) { // if this reception was reconstructed from IP fragments the payload checkum has to be checked based on collected fragments
        return (ptrRx_frame->usFragment_CS);                             // return the prepared payload checkum
    }
    #endif
    return IP_GOOD_CS;                                                   // an interface with automatic checksum verification doesn't need to be checked again
}
#else
    #define fnRxCalcIP_CS(dummy, data, usLength) fnCalcIP_CS(0, data, usLength)
#endif

static void fnInsertPingChecksum(ETHERNET_FRAME *ptrRx_frame, ICMP_ERROR *ptrICP_frame)
{
    #if !defined IP_TX_PAYLOAD_CHECKSUM_OFFLOAD || defined IP_INTERFACE_WITHOUT_CS_OFFLOADING || (defined IPV4_SUPPORT_TX_FRAGMENTATION && defined IP_TX_PAYLOAD_CHECKSUM_OFFLOAD) || defined _WINDOWS
    int iHWCheckSum;
    unsigned short usCheckSum;
    ptrICP_frame->ucICMPCheckSum[0] = 0;                                 // checksum set to 0 (for possible calcualtion)
    ptrICP_frame->ucICMPCheckSum[1] = 0;
        #if defined IPV4_SUPPORT_TX_FRAGMENTATION && defined IP_TX_PAYLOAD_CHECKSUM_OFFLOAD
    if (((ptrRx_frame->ucSpecialHandling & INTERFACE_NO_TX_PAYLOAD_CS_OFFLOADING) == 0) && (ptrRx_frame->usDataLength <= ETH_MTU)) {
        iHWCheckSum = 1;                                                 // the hardware may insert the checksum since no fragmentation will be involved
            #if defined _WINDOWS
        iHWCheckSum = -1;                                                // force checkum insertion when simulating
            #endif
    }
    else {
        iHWCheckSum = -1;                                                // the payload checksum must be calculated in the software since it can't be performed over fragments by hardware
    }
        #else
    iHWCheckSum = 0;
        #endif
        #if defined IP_INTERFACE_WITHOUT_CS_OFFLOADING && (IP_INTERFACE_COUNT > 1)
    if ((ptrRx_frame->ucInterfaceHandling & (INTERFACE_NO_TX_PAYLOAD_CS_OFFLOADING | INTERFACE_NO_TX_CS_OFFLOADING)) == 0) { // if the interface won't perform payload checksum offloading
        if (iHWCheckSum == 0) {                                          // if not blocked due to other decisions
            iHWCheckSum = 1;                                             // the hardware may insert the checksum
        }
    }
    else {
        iHWCheckSum = 0;                                                 // this interface doesn't support checksum offloading so it must be calculated
    }
        #endif
    if (iHWCheckSum <= 0) {                                              // if the hardware will not insert the checksum we do it in the software
        usCheckSum = ~fnCalcIP_CS(0, &ptrRx_frame->ptEth->ucData[ptrRx_frame->usIPLength], ptrRx_frame->usDataLength); // calculate the new checksum of reply
        ptrICP_frame->ucICMPCheckSum[0] = (unsigned char)(usCheckSum >> 8); // insert new payload checksum
        ptrICP_frame->ucICMPCheckSum[1] = (unsigned char)(usCheckSum);
    }
    #else
    ptrICP_frame->ucICMPCheckSum[0] = 0;                                 // checksum set to 0 (it will be filled out automatically by the controller when transmitted)
    ptrICP_frame->ucICMPCheckSum[1] = 0;
    #endif
}

// This routine is called when an IPv4 frame has been received, which has been recognised as an ICMP type
//
extern void fnHandleICMP(ETHERNET_FRAME *ptrRx_frame)
{
    unsigned char *icp_data = &ptrRx_frame->ptEth->ucData[ptrRx_frame->usIPLength];
    ICMP_ERROR *ptrICP_frame = (ICMP_ERROR *)icp_data;
    unsigned short usLen = ptrRx_frame->usDataLength;
    IP_PACKET *received_ip_packet = (IP_PACKET *)&ptrRx_frame->ptEth->ucData;
    #if !defined IP_RX_CHECKSUM_OFFLOAD || defined IP_INTERFACE_WITHOUT_CS_OFFLOADING || (defined IPV4_SUPPORT_RX_DEFRAGMENTATION && defined IP_RX_CHECKSUM_OFFLOAD) || defined _WINDOWS // {1}
    if (IP_GOOD_CS != fnRxCalcIP_CS(ptrRx_frame, icp_data, usLen)) {     // check the ICMP checksum
        return;                                                          // if the checksum of the ICMP frame is corrupt we ignore it
    }
    #endif
    switch (ptrICP_frame->ucICMPType) {                                  // ICMP type
    #if defined ICMP_PING
    case (ECHO_PING):                                                    // we simply echo the ICMP message back with ECHO_PING_REPLY
        #if defined SUPPORT_MULTICAST_RX || defined USE_IGMP             // {9}
        if ((received_ip_packet->destination_IP_address[0] >= 224) && (received_ip_packet->destination_IP_address[0] <= 239)) { // never respond to pings on a broadcast address
            break;                                                       // we may never respond if addressed on a multi-cast address
        }
        #endif
        ptrICP_frame->ucICMPType = ECHO_PING_REPLY;                      // overwrite the ICMP type for reply
        fnInsertPingChecksum(ptrRx_frame, ptrICP_frame);                 // insert the checksum
        // Note that we do not add an owner to this IP attempt since we have just received an IP and so the IP is unconditionally resolved
        //
        #if IP_NETWORK_COUNT > 1                                         // {7}
        if (ptrRx_frame->ucBroadcastResponse != 0) {                     // if a broadcast on one or more networks is to be responded to
            int iBroadcastNetworks = ptrRx_frame->ucBroadcastResponse;
            unsigned char ucNetwork = 0;
            while (iBroadcastNetworks != 0) {                            // while networks with responses are listed
                if (iBroadcastNetworks & (1 << ucNetwork)) {             // response shoudl be sent on this network
                    fnSendIPv4(received_ip_packet->source_IP_address, IP_ICMP, TOS_NORMAL_SERVICE, MAX_TTL, icp_data, usLen, 0, (USOCKET)(defineVLAN(ptrRx_frame->ucVLAN_content) | NetworkInterface(ucNetwork, fnGetInterfaceFlag(ptrRx_frame->Tx_handle, 0)))); // {6}
                }
                iBroadcastNetworks &= ~(1 << ucNetwork);                 // remove this network from the response list
                ucNetwork++;                                             // check next network
            }
        }
        else {
            fnSendIPv4(received_ip_packet->source_IP_address, IP_ICMP, TOS_NORMAL_SERVICE, MAX_TTL, icp_data, usLen, 0, (USOCKET)(defineVLAN(ptrRx_frame->ucVLAN_content) | NetworkInterface(ptrRx_frame->ucNetworkID, fnGetInterfaceFlag(ptrRx_frame->Tx_handle, 0)))); // {6}
        }
        #elif IP_INTERFACE_COUNT > 1
        fnSendIPv4(received_ip_packet->source_IP_address, IP_ICMP, TOS_NORMAL_SERVICE, MAX_TTL, icp_data, usLen, 0, (USOCKET)(defineVLAN(ptrRx_frame->ucVLAN_content) | NetworkInterface(ptrRx_frame->ucNetworkID, fnGetInterfaceFlag(ptrRx_frame->Tx_handle, 0)))); // {6}
        #else
        fnSendIPv4(received_ip_packet->source_IP_address, IP_ICMP, TOS_NORMAL_SERVICE, MAX_TTL, icp_data, usLen, 0, 0);
        #endif
        break;
    #endif

    #if defined ICMP_SEND_PING
    case ECHO_PING_REPLY:
        #if defined ICMP_PING_IP_RESULT                                  // {4}
        fnReportPing((UTASK_TASK)ptrICP_frame->ucICMP_variable[0], (USOCKET *)&ptrICP_frame->ucICMP_variable[2], PING_RESULT, received_ip_packet->source_IP_address);
        #else
        fnReportPing((UTASK_TASK)ptrICP_frame->ucICMP_variable[0], (USOCKET *)&ptrICP_frame->ucICMP_variable[2], PING_RESULT);
        #endif
        break;
    #endif

    #if defined USE_UDP
        #if defined ICMP_DEST_UNREACHABLE
    case DESTINATION_UNREACHABLE:                                        // if we support UDP we report an error here
        if (ptrICP_frame->ucICMPCode == PORT_UNREACHABLE) {
            if (ptrICP_frame->tCopy_IP_header.ip_protocol == IP_UDP) {
                unsigned char *ptrPort;                                  // UDP port was unreachable - inform listner from Port source
                unsigned short usEmbeddedLength = ((ptrICP_frame->tCopy_IP_header.version_header_length & IP_HEADER_LENGTH_MASK) << 2); // length if IP including any options
                unsigned short usSourcePort, usDestPort;

                ptrPort = (icp_data + usEmbeddedLength + ICMP_HEADER_LENGTH); // set a pointer to the UDP port info in the error message

                usSourcePort = *ptrPort++;                               // extract the UDP ports involved
                usSourcePort <<= 8;
                usSourcePort |= *ptrPort++;
                usDestPort   = *ptrPort++;
                usDestPort   <<= 8;
                usDestPort   |= *ptrPort++;
                fnReportUDP(usSourcePort, usDestPort, UDP_EVENT_PORT_UNREACHABLE, received_ip_packet->destination_IP_address);
            }
        }
        break;
        #endif
    #endif
    }
}

    #if defined ICMP_SEND_PING
// Send an IPV4 ping request
//
extern int fnSendPing(unsigned char *ping_address, unsigned char ttl, UTASK_TASK OwnerTask, USOCKET Socket)
{
    int i = 0;
    unsigned char ucData = 'a';
        #if !defined IP_TX_PAYLOAD_CHECKSUM_OFFLOAD || defined IP_INTERFACE_WITHOUT_CS_OFFLOADING || defined _WINDOWS // {1}
    unsigned short usCheckSum;
        #endif
    PING_FRAME ping_frame;

    uMemset((unsigned char*)&ping_frame, 0, sizeof(PING_FRAME));         // start with empty frame data

    ping_frame.ucICMPType = ECHO_PING;
  //ping_frame.ucICMPCode = 0;
  //ping_frame.ucICMPCheckSum[0] = 0;
  //ping_frame.ucICMPCheckSum[1] = 0;
    ping_frame.ucICMPIdent[0] = (unsigned char)OwnerTask;                // we pack our owner's details for identification purposes
  //ping_frame.ucICMPIdent[1] = 0;                                       // spare
    uMemcpy(ping_frame.ucICMPSequence, &Socket, sizeof(USOCKET));        // {3} supports USOCKETS of unsigned char and unsigned short
  //ping_frame.ucICMPSequence[0] = (unsigned char)Socket;
  //ping_frame.ucICMPSequence[1] = 0;
    while (ucData <= 'z') {
        ping_frame.ucICMPData[i++] = ucData++;
    }
#if !defined IP_TX_PAYLOAD_CHECKSUM_OFFLOAD || defined IP_INTERFACE_WITHOUT_CS_OFFLOADING || defined _WINDOWS // {1}
    usCheckSum = ~fnCalcIP_CS(0, (unsigned char*)&ping_frame, sizeof(PING_FRAME)); // calculate the ICMP frame checksum
    ping_frame.ucICMPCheckSum[0] = (unsigned char)(usCheckSum >> 8);
    ping_frame.ucICMPCheckSum[1] = (unsigned char)(usCheckSum);
#endif
    return (fnSendIPv4(ping_address, IP_ICMP, TOS_NORMAL_SERVICE, ttl, (unsigned char*)&ping_frame, sizeof(PING_FRAME), OwnerTask, Socket));
}
    #endif

// Report an IPV4 error
//
extern void fnSendICMPError(ICMP_ERROR *tICMP_error, unsigned short usLength)
{
#if !defined IP_TX_PAYLOAD_CHECKSUM_OFFLOAD || defined IP_INTERFACE_WITHOUT_CS_OFFLOADING || defined _WINDOWS // {1}
    unsigned short usChecksum;

    usChecksum = ~fnCalcIP_CS(0, (unsigned char *)tICMP_error, usLength);
    tICMP_error->ucICMPCheckSum[0] = (unsigned char)(usChecksum >> 8);   // insert the check sum
    tICMP_error->ucICMPCheckSum[1] = (unsigned char)(usChecksum);
#endif
    fnSendIPv4(tICMP_error->tCopy_IP_header.source_IP_address, IP_ICMP, TOS_NORMAL_SERVICE, MAX_TTL, (unsigned char *)tICMP_error, usLength, 0, 0);
}
#endif


#if defined USE_IPV6                                                     // IPV6 always uses ICMPV6

// Send a neighbor advertisement
//
static int fnSendIPV6Advertisement(ETHERNET_FRAME *ptrRxFrame)
{
    IPV6_DISCOVERY_FRAME_RX *ptrDiscoveryFrame = (IPV6_DISCOVERY_FRAME_RX *)(ptrRxFrame->ptEth->ucData);
    IPV6_DISCOVERY_FRAME discovery_frame;
    ETHERNET_FRAME_CONTENT *frame_cont = ptrRxFrame->ptEth;
    #if IP_INTERFACE_COUNT > 1
    QUEUE_HANDLE Tx_handle = ptrRxFrame->Tx_handle;                      // interface handle that the frame was received on and where the advertisement will be sent back on
    #endif
    #if IP_NETWORK_COUNT > 1
    register unsigned char ucNetworkID = ptrRxFrame->ucNetworkID;        // set the netwoirk to the one that the rx frame is on
    #endif
    #if !defined IP_TX_PAYLOAD_CHECKSUM_OFFLOAD || defined FORCE_PAYLOAD_ICMPV6_TX || defined IP_INTERFACE_WITHOUT_CS_OFFLOADING || defined _WINDOWS // {1}
    unsigned short usCheckSum;
    #endif
    // Construct a discovery frame
    //
    uMemcpy(discovery_frame.ethernetII.destination_mac_address, frame_cont->ethernet_source_MAC, MAC_LENGTH); // destination MAC address is equal to the received frame's source MAC address
    uMemcpy(&discovery_frame.ethernetII.source_mac_address, network[_NETWORK_ID].ucOurMAC, MAC_LENGTH); // insert our source MAC address
    discovery_frame.ethernetII.ethernet_type[0] = (unsigned char)(PROTOCOL_IPv6 >> 8); // IPv6 protocol
    discovery_frame.ethernetII.ethernet_type[1] = (unsigned char)PROTOCOL_IPv6;
    discovery_frame.ipv6.version_traffic_class = IP_VERSION_6;
    discovery_frame.ipv6.traffic_class_flow = 0;
    discovery_frame.ipv6.flow_lable[0] = discovery_frame.ipv6.flow_lable[1] = 0;
    discovery_frame.ipv6.payload_length[0] = 0;
    discovery_frame.ipv6.payload_length[1] = 32;                         // fixed payload length
    discovery_frame.ipv6.next_header = IP_ICMPV6;                        // carrying ICMPV6
    discovery_frame.ipv6.hop_limit = 255;                                // maximum hop limit
    uMemcpy(discovery_frame.ipv6.source_IP_address, ucLinkLocalIPv6Address[DEFAULT_NETWORK], IPV6_LENGTH); // {2} insert our link local address as source address for advertisements
    uMemcpy(discovery_frame.ipv6.destination_IP_address, (unsigned char *)ptrDiscoveryFrame->ipv6.source_IP_address, sizeof(discovery_frame.ipv6.destination_IP_address)); // set the destination address to the rx frame's source address
    uMemset(&discovery_frame.icmpv6, 0, sizeof(discovery_frame.icmpv6));
    discovery_frame.icmpv6.ucICMPV6Type = ICMPV6_TYPE_NEIGHBOR_ADVERTISEMENT;
    discovery_frame.icmpv6.ucICMPV6Flags[0] = (SOLICITED_RESPONSE | OVERRIDE);
    uMemcpy(discovery_frame.icmpv6.target_IP_address, ucLinkLocalIPv6Address[DEFAULT_NETWORK], IPV6_LENGTH); // {2}
    discovery_frame.icmpv6.ucICMPV6_option_type = TARGET_LINK_LAYER_ADDRESS;
    discovery_frame.icmpv6.ucICMPV6_option_length = ICMPV6_OPTION_LENGTH_8;
    uMemcpy(discovery_frame.icmpv6.ucICMPV6_option_data, network[_NETWORK_ID].ucOurMAC, MAC_LENGTH);
    #if !defined IP_TX_PAYLOAD_CHECKSUM_OFFLOAD || defined FORCE_PAYLOAD_ICMPV6_TX || defined IP_INTERFACE_WITHOUT_CS_OFFLOADING || defined _WINDOWS // {1}
    usCheckSum = (IP_ICMPV6 + 32);
    usCheckSum = fnCalcIP_CS(usCheckSum, (unsigned char *)&discovery_frame.ipv6.source_IP_address, (2 * sizeof(discovery_frame.ipv6.source_IP_address))); // calculate the ICMPV6 checksum - first over pseudo header
    usCheckSum = ~fnCalcIP_CS(usCheckSum, (unsigned char *)&discovery_frame.icmpv6, sizeof(discovery_frame.icmpv6));
    if (usCheckSum == 0) {
        usCheckSum = 0xffff;
    }
    discovery_frame.icmpv6.ucICMPV6CheckSum[0] = (unsigned char)(usCheckSum >> 8);
    discovery_frame.icmpv6.ucICMPV6CheckSum[1] = (unsigned char)usCheckSum;
    #else
    discovery_frame.icmpv6.ucICMPV6CheckSum[0] = 0;
    discovery_frame.icmpv6.ucICMPV6CheckSum[1] = 0;
    #endif
    #if defined USE_IP_STATS
    fnTxStats(IP_ICMPV6, _NETWORK_ID);                                   // count ICMPV6 transmissions
    #endif

    if (fnWrite(_ETHERNET_HANDLE, (unsigned char *)&discovery_frame, sizeof(discovery_frame)) == 0) { // add the pay load
        return 0;                                                        // failed
    }
    return (fnWrite(_ETHERNET_HANDLE, 0, 0));                            // transmit the ETHERNET frame
}

// Reply to an IPV6 ping by returning the message with rx and tx swapped
//
static int fnSendIPV6EchoResponse(ETHERNET_FRAME_CONTENT *frame_cont, IPV6_DISCOVERY_FRAME_RX *ptrDiscoveryFrame)
{
    unsigned short usLength = ((ptrDiscoveryFrame->ipv6.payload_length[0] << 8) + ptrDiscoveryFrame->ipv6.payload_length[1]);
    ptrDiscoveryFrame->icmpv6.ucICMPV6Type = ICMPV6_TYPE_ECHO_REPLY;
    ptrDiscoveryFrame->icmpv6.ucICMPV6CheckSum[0] = ptrDiscoveryFrame->icmpv6.ucICMPV6CheckSum[1] = 0;
    #if defined USE_IP_STATS
    fnTxStats(IP_ICMPV6, _NETWORK_ID);                                   // count ICMPV6 transmissions
    #endif
    return (fnSendIPV6(ptrDiscoveryFrame->ipv6.source_IP_address, IP_ICMPV6, MAX_TTL, (unsigned char *)&ptrDiscoveryFrame->icmpv6, (unsigned short)(usLength), 0, 0));
}

// Send a discovery message to resolve an IPV6 address
//
extern void fnSendIPV6Discovery(NEIGHBOR_TAB *ptrNeighbor)               // {8}
{
    IPV6_DISCOVERY_FRAME discovery_frame;
    #if !defined IP_TX_PAYLOAD_CHECKSUM_OFFLOAD || defined FORCE_PAYLOAD_ICMPV6_TX || defined IP_INTERFACE_WITHOUT_CS_OFFLOADING || defined _WINDOWS // {1}
    unsigned short usCheckSum;
    #endif
    #if IP_INTERFACE_COUNT > 1                                           // {8}
    unsigned char ucInterfaceFlag = 0x01;
    int iReference = 0;
    unsigned char ucInterfaces = extractInterface(ptrNeighbor->OwnerSocket); // note which interface(s) the request is to be made on
    if (ucInterfaces == 0) {                                             // no interface specified
        ucInterfaces = (0x01 << DEFAULT_IP_INTERFACE);                   // so assume default
    }
    #endif
    discovery_frame.ethernetII.destination_mac_address[0] = 0x33;        // construct multicast destination address
    discovery_frame.ethernetII.destination_mac_address[1] = 0x33;
    discovery_frame.ethernetII.destination_mac_address[2] = 0xff;
    discovery_frame.ethernetII.destination_mac_address[3] = ptrNeighbor->ucIPV6[13];
    discovery_frame.ethernetII.destination_mac_address[4] = ptrNeighbor->ucIPV6[14];
    discovery_frame.ethernetII.destination_mac_address[5] = ptrNeighbor->ucIPV6[15];
    uMemcpy(&discovery_frame.ethernetII.source_mac_address, network[_NETWORK_ID].ucOurMAC, MAC_LENGTH);
    discovery_frame.ethernetII.ethernet_type[0] = (unsigned char)(PROTOCOL_IPv6 >> 8);
    discovery_frame.ethernetII.ethernet_type[1] = (unsigned char)PROTOCOL_IPv6;
    discovery_frame.ipv6.version_traffic_class = IP_VERSION_6;
    discovery_frame.ipv6.traffic_class_flow = 0;
    discovery_frame.ipv6.flow_lable[0] = discovery_frame.ipv6.flow_lable[1] = 0;
    discovery_frame.ipv6.payload_length[0] = 0;
    discovery_frame.ipv6.payload_length[1] = 32;
    discovery_frame.ipv6.next_header = IP_ICMPV6;
    discovery_frame.ipv6.hop_limit = 255;
    uMemcpy(discovery_frame.ipv6.source_IP_address, ucLinkLocalIPv6Address[DEFAULT_NETWORK], IPV6_LENGTH); // {2}
    uMemcpy(discovery_frame.ipv6.destination_IP_address, ptrNeighbor->ucIPV6, IPV6_LENGTH);
    uMemset(&discovery_frame.icmpv6, 0, sizeof(discovery_frame.icmpv6));
    discovery_frame.icmpv6.ucICMPV6Type = ICMPV6_TYPE_NEIGHBOR_SOLICITATION;
    uMemcpy(discovery_frame.icmpv6.target_IP_address, ptrNeighbor->ucIPV6, IPV6_LENGTH);
    discovery_frame.icmpv6.ucICMPV6_option_type = SOURCE_LINK_LAYER_ADDRESS;
    discovery_frame.icmpv6.ucICMPV6_option_length = ICMPV6_OPTION_LENGTH_8;
    uMemcpy(discovery_frame.icmpv6.ucICMPV6_option_data, network[_NETWORK_ID].ucOurMAC, MAC_LENGTH);
    #if !defined IP_TX_PAYLOAD_CHECKSUM_OFFLOAD || defined FORCE_PAYLOAD_ICMPV6_TX || defined IP_INTERFACE_WITHOUT_CS_OFFLOADING || defined _WINDOWS // {1}
    usCheckSum = (IP_ICMPV6 + 32);
    usCheckSum = fnCalcIP_CS(usCheckSum, (unsigned char *)&discovery_frame.ipv6.source_IP_address, (2 * sizeof(discovery_frame.ipv6.source_IP_address))); // calculate the ICMPV6 checksum - first over pseudo header
    usCheckSum = ~fnCalcIP_CS(usCheckSum, (unsigned char *)&discovery_frame.icmpv6, sizeof(discovery_frame.icmpv6));
    if (usCheckSum == 0) {
        usCheckSum = 0xffff;
    }
    discovery_frame.icmpv6.ucICMPV6CheckSum[0] = (unsigned char)(usCheckSum >> 8);
    discovery_frame.icmpv6.ucICMPV6CheckSum[1] = (unsigned char)usCheckSum;
    #else
    discovery_frame.icmpv6.ucICMPV6CheckSum[0] = 0;
    discovery_frame.icmpv6.ucICMPV6CheckSum[1] = 0;
    #endif
    #if defined USE_IP_STATS
    fnTxStats(IP_ICMPV6, _NETWORK_ID);                                   // count ICMPV6 transmissions
    #endif
    #if IP_INTERFACE_COUNT > 1                                           // {8}
    do {                                                                 // send request to all interfaces belonging to the network
        if (ucInterfaces & ucInterfaceFlag) {
            ptrNeighbor->Tx_handle = fnGetInterfaceHandle(iReference);
            if (ptrNeighbor->Tx_handle != 0) {
                if (fnWrite(ptrNeighbor->Tx_handle, (unsigned char *)&discovery_frame, sizeof(discovery_frame)) != 0) { // add the pay load
                    fnWrite(ptrNeighbor->Tx_handle, 0, 0);               // transmit the ETHERNET frame on the interface
                }
            }
        }
        iReference++;
        ucInterfaces &= ~ucInterfaceFlag;
        ucInterfaceFlag <<= 1;
    } while (ucInterfaces != 0);
    #else
    if (fnWrite(_ETHERNET_HANDLE, (unsigned char *)&discovery_frame, sizeof(discovery_frame)) != 0) { // add the pay load
        fnWrite(_ETHERNET_HANDLE, 0, 0);                                 // transmit the ETHERNET frame
    }
    #endif
}

// Handle ICMPv6 reception (note: presently only supports default network)
//
extern void fnHandleICMPV6(ETHERNET_FRAME *ptrRxFrame)
{
    IPV6_DISCOVERY_FRAME_RX *ptrDiscoveryFrame = (IPV6_DISCOVERY_FRAME_RX *)(ptrRxFrame->ptEth->ucData);
    unsigned short usPayloadLength;
    #if !defined IP_RX_CHECKSUM_OFFLOAD || defined FORCE_PAYLOAD_ICMPV6_RX || defined IP_INTERFACE_WITHOUT_CS_OFFLOADING || defined _WINDOWS
    unsigned short usCheckSum;
    #endif
    ETHERNET_FRAME_CONTENT *frame_cont = ptrRxFrame->ptEth;
    usPayloadLength = (ptrDiscoveryFrame->ipv6.payload_length[0] << 8);
    usPayloadLength += ptrDiscoveryFrame->ipv6.payload_length[1];
    if (usPayloadLength > (ptrRxFrame->frame_size - ETH_HEADER_LEN - sizeof(IP_PACKET_V6))) { // check basic payload size validity
        return;                                                          // ignore frames with unrealistic payload size
    }
    #if !defined IP_RX_CHECKSUM_OFFLOAD || defined FORCE_PAYLOAD_ICMPV6_RX || defined IP_INTERFACE_WITHOUT_CS_OFFLOADING || defined _WINDOWS
    usCheckSum = fnCalcIP_CS(ptrDiscoveryFrame->ipv6.next_header, (unsigned char*)&ptrDiscoveryFrame->ipv6.payload_length, 2); // check pseudo-header plus payload, where the pseudo-header consists of the next header type, the payload length, the source IPV6 and destination IPV6 addresses
    if (IP_GOOD_CS != fnCalcIP_CS(usCheckSum, (unsigned char*)&ptrDiscoveryFrame->ipv6.source_IP_address, (unsigned short)((2 * IPV6_LENGTH) + usPayloadLength))) {
        return;                                                          // quietly discard frame when there is a checksum error
    }
    #endif

    switch (ptrDiscoveryFrame->icmpv6.ucICMPV6Type) {
    case ICMPV6_TYPE_NEIGHBOR_SOLICITATION:
        fnSendIPV6Advertisement(ptrRxFrame);                             // respond with an advertisement
        break;
    case ICMPV6_TYPE_NEIGHBOR_ADVERTISEMENT:
        fnEnterIPV6Neighbor(frame_cont->ethernet_source_MAC, (unsigned char *)ptrDiscoveryFrame->ipv6.source_IP_address, (MAC_LENGTH | RESOLVED_ADDRESS));
        return;                                                          // no need to refresh neighbor table
    #ifdef ICMP_SEND_PING
    case ICMPV6_TYPE_ECHO_REPLY:
        {
            ICMPV6_ECHO_REQUEST *ptrAnswer = (ICMPV6_ECHO_REQUEST *)&ptrDiscoveryFrame->icmpv6;
        #if defined ICMP_PING_IP_RESULT                                  // {4}
            fnReportPing((UTASK_TASK)ptrAnswer->ucICMPV6ID[0], (USOCKET *)&ptrAnswer->ucICMPV6sequence[0], PING_IPV6_RESULT, ptrDiscoveryFrame->ipv6.source_IP_address);
        #else
            fnReportPing((UTASK_TASK)ptrAnswer->ucICMPV6ID[0], (USOCKET *)&ptrAnswer->ucICMPV6sequence[0], PING_IPV6_RESULT);
        #endif
        }
        break;
    #endif
    case ICMPV6_TYPE_ECHO_REQUEST:
        fnEnterIPV6Neighbor(frame_cont->ethernet_source_MAC, (unsigned char *)ptrDiscoveryFrame->ipv6.source_IP_address, (REFRESH_ADDRESS | MAC_LENGTH));
        fnSendIPV6EchoResponse(frame_cont, ptrDiscoveryFrame);
        return;                                                          // no need to refresh neighbor table
    }
    fnEnterIPV6Neighbor(ptrRxFrame->ptEth->ethernet_source_MAC, (unsigned char *)ptrDiscoveryFrame->ipv6.source_IP_address, (REFRESH_ADDRESS | MAC_LENGTH));
}

    #ifdef ICMP_SEND_PING
// Send an IPV6 ping request
//
extern int fnSendV6Ping(unsigned char *ping_address, unsigned char ucttl, UTASK_TASK OwnerTask, USOCKET Socket)
{
    int i = 0;
    unsigned char ucData = 'a'; 
    ICMPV6_ECHO_REQUEST echo_request_frame;

    uMemset((unsigned char*)&echo_request_frame, 0, sizeof(ICMPV6_ECHO_REQUEST)); // start with empty frame data

    echo_request_frame.ucICMPV6Type = ICMPV6_TYPE_ECHO_REQUEST;
  //echo_request_frame.ucICMPV6Code = 0;
  //echo_request_frame.ucICMPV6CheckSum[0] = 0;
  //echo_request_frame.ucICMPV6CheckSum[1] = 0;
    echo_request_frame.ucICMPV6ID[0] = OwnerTask;                        // we pack our owner's details for identification purposes
  //echo_request_frame.ucICMPV6ID[1] = 0;
    uMemcpy(echo_request_frame.ucICMPV6sequence, &Socket, sizeof(USOCKET)); // {3} supports USOCKETS of unsigned char and unsigned short
  //echo_request_frame.ucICMPV6sequence[0] = (unsigned char)Socket;
  //echo_request_frame.ucICMPV6sequence[1] = 0;
    while (ucData <= 'z') {
        echo_request_frame.ucICMPV6_data[i++] = ucData++;                // insert data content
    }
    return (fnSendIPV6(ping_address, IP_ICMPV6, ucttl, (unsigned char*)&echo_request_frame, sizeof(ICMPV6_ECHO_REQUEST), OwnerTask, Socket));
}
    #endif
#endif

#if (defined USE_IPV6 || defined USE_IP) && defined ICMP_SEND_PING
// We have received an ECHO ping reply - either promiscuously or as a response to a request we made
// If it is for our IP address, inform the owner that a PING has been received
//
    #if defined ICMP_PING_IP_RESULT                                      // {4}
static void fnReportPing(UTASK_TASK Task, USOCKET *Socket, unsigned char ucResult, unsigned char *ptrIP)
    #else
static void fnReportPing(UTASK_TASK Task, USOCKET *Socket, unsigned char ucResult)
    #endif
{
    #if defined ICMP_PING_IP_RESULT                                      // {4}
        #if defined USE_IPV6
            #define RESULT_INFO_LENGTH (1 + sizeof(USOCKET) + IPV6_LENGTH)
        #else
            #define RESULT_INFO_LENGTH (1 + sizeof(USOCKET) + IPV4_LENGTH)
        #endif
    #else
        #define RESULT_INFO_LENGTH (1 + sizeof(USOCKET))
    #endif
    unsigned char message[HEADER_LENGTH + RESULT_INFO_LENGTH];           // space for event message for delivery to the ping sending task
    message[MSG_DESTINATION_NODE] = message[MSG_SOURCE_NODE] = INTERNAL_ROUTE;
    message[MSG_DESTINATION_TASK] = (unsigned char)Task;
    #if defined SUPPORT_SUBNET_TX_BROADCAST                              // {5}
    message[MSG_SOURCE_TASK]      = CHECK_QUEUE;
    #else
    message[MSG_SOURCE_TASK]      = TASK_ICMP;
    #endif
    message[MSG_CONTENT_COMMAND]  = ucResult;
    uMemcpy(&message[MSG_CONTENT_COMMAND + 1], Socket, sizeof(USOCKET)); // {3}
    #if defined ICMP_PING_IP_RESULT
        #if defined USE_IPV6
    if (PING_IPV6_RESULT == ucResult) {
        message[MSG_CONTENT_LENGTH] = RESULT_INFO_LENGTH;
        uMemcpy(&message[MSG_CONTENT_COMMAND + 1 + sizeof(USOCKET)], ptrIP, IPV6_LENGTH);
    }
    else {
        message[MSG_CONTENT_LENGTH] = (1 + sizeof(USOCKET) + IPV4_LENGTH);
        uMemcpy(&message[MSG_CONTENT_COMMAND + 1 + sizeof(USOCKET)], ptrIP, IPV4_LENGTH);
    }
        #else
    message[MSG_CONTENT_LENGTH]   = RESULT_INFO_LENGTH;
    uMemcpy(&message[MSG_CONTENT_COMMAND + 1 + sizeof(USOCKET)], ptrIP, IPV4_LENGTH);
        #endif
    #else
    message[MSG_CONTENT_LENGTH]   = RESULT_INFO_LENGTH;
    #endif
    #if defined SUPPORT_SUBNET_TX_BROADCAST                              // {5} since there may be many ping responses make sure that the destination task's queue has space
    if (fnWrite(INTERNAL_ROUTE, message, HEADER_LENGTH) > (QUEUE_TRANSFER)(HEADER_LENGTH + message[MSG_CONTENT_LENGTH])) { // check that the reception task has adequate queue space
        message[MSG_SOURCE_TASK]  = TASK_ICMP;
        fnWrite(INTERNAL_ROUTE, message, (QUEUE_TRANSFER)(HEADER_LENGTH + message[MSG_CONTENT_LENGTH])); // we send details to the owner of the ping request
    }
    #else
    fnWrite(INTERNAL_ROUTE, message, (QUEUE_TRANSFER)(HEADER_LENGTH + message[MSG_CONTENT_LENGTH])); // we send details to the owner of the ping request
    #endif
}
#endif
