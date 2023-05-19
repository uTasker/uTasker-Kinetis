/**********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher

    ---------------------------------------------------------------------
    File:      udp.c
    Project:   Single Chip Embedded Internet
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2016
    *********************************************************************
    08.04.2007 Don't send destination unreachable to subnet broadcast (with new SUPPORT_SUBNET_BROADCAST) {1}
    06.09.2010 Optimise reception code when the device performs rx offloading {2}
    03.03.2012 Remove checksum calculation when IP_TX_CHECKSUM_OFFLOAD is enabled {3}
    29.04.2012 Enable check sum when simulating                          {4}
    05.09.2012 Add own IP address to call to fnSubnetBroadcast()         {5}
    16.09.2012 Specifically accept broadcasts together with SUPPORT_SUBNET_BROADCAST {6}
    17.12.2012 Changed fnHandleUDP() interface to allow IPv6 and multi-homed operation {7}
    04.06.2013 Added UDP_SOCKET_MALLOC() default                         {8}
    12.04.2014 Added IGMP/multicast UDP reception handling               {9}
    03.12.2014 Modify fnSendUDP_multicast() and fnTransmitUDP() parameters {10}
    01.09.2015 Never send destination unreachable to a multi-cast address {11}
    03.02.2016 Pass interface details to multicast reception             {12}

*/        

#include "config.h"    


#ifdef USE_UDP                                                           // if we want to use UDP

/* =================================================================== */
/*                          local definitions                          */
/* =================================================================== */

#ifndef UDP_SOCKET_MALLOC                                                // {8}
    #define UDP_SOCKET_MALLOC(x)      uMalloc((MAX_MALLOC)(x))
#endif

#define UDP_STATE_FREE          0                                        // this must be zero
#define UDP_STATE_CLOSED        1
#define UDP_STATE_OPENED        2


#if IP_NETWORK_COUNT > 1
    #define _NETWORK_ID        ucNetworkID
#else
    #define _NETWORK_ID        DEFAULT_NETWORK 
#endif

/* =================================================================== */
/*                      local variable definitions                     */
/* =================================================================== */

static UDP_TAB *tUDP = 0;

// An application can obtain an UDP socket by calling this function
//
extern USOCKET fnGetUDP_socket(unsigned char ucTOS, int (*fnListener)(USOCKET, unsigned char, unsigned char *, unsigned short, unsigned char *, unsigned short), unsigned char ucOpts)
{
    UDP_TAB *ptrUDP;
    USOCKET udpSocket = 0;

    if (tUDP == 0) {                                                     // automatically get heap space on first call 
        tUDP = (UDP_TAB*)UDP_SOCKET_MALLOC(sizeof(UDP_TAB) * UDP_SOCKETS); // get UDP table space (malloc zeroes it)
    }

    ptrUDP = tUDP;

    if (fnListener == 0) {
        return NO_LISTENER_DEFINED;                                      // the application must define a listener function
    }

    while (udpSocket < UDP_SOCKETS) {
        if (ptrUDP->ucState == UDP_STATE_FREE) {                         // search for the next free socket
            ptrUDP->ucState       = UDP_STATE_CLOSED;                    // initialise socket structure
            ptrUDP->ucServiceType = ucTOS;
            ptrUDP->usLocalPort   = 0;
            ptrUDP->ucOptions     = ucOpts;
            ptrUDP->fnListener    = fnListener;
            return udpSocket;                                            // socket 0.. max-1 returned
        }
        ptrUDP++;
        udpSocket++;                                   
    }
    return NO_FREE_UDP_SOCKETS;                                          // no free UDP sockets
}

// This routine is called to find the corresponding open UDP socket
//
static USOCKET fnFindOpenSocket(unsigned short usDestinationPort)
{
    UDP_TAB *ptrUDP = tUDP;
    USOCKET udpSocket = 0;

    if (!ptrUDP) {
        return UDP_NOT_INITIALISED;
    }

    while (udpSocket < UDP_SOCKETS) {
        if ((ptrUDP->ucState == UDP_STATE_OPENED) && (ptrUDP->usLocalPort == usDestinationPort)) {
            return udpSocket;
        }
        udpSocket++;
        ptrUDP++;
    }
    return NO_UDP_LISTENER_FOUND;                                        // no open socket for this port found
}

// The client/server can call this to release a socket which is no longer required
//
extern USOCKET fnReleaseUDP_socket(USOCKET cSocketHandle)
{
    UDP_TAB *ptrUDP = tUDP;
    
    if ((!ptrUDP) || ( cSocketHandle > UDP_SOCKETS )) {
        return INVALID_SOCKET_HANDLE;
    }

    ptrUDP += cSocketHandle;
    uMemset(ptrUDP, 0x00, sizeof(UDP_TAB));                              // clear no longer used socket structure

    return (cSocketHandle);                                              // this socket has just been freed
}

// After the client/server has obtained a socket it can bind it using this function
//
extern USOCKET fnBindSocket(USOCKET SocketHandle, unsigned short usLocalPort)
{
    UDP_TAB *ptrUDP = tUDP;
    
    _UDP_SOCKET_MASK_ASSIGN(SocketHandle);                               // remove network, interface and user flags from the socket number
    if ((ptrUDP == 0) || (SocketHandle > UDP_SOCKETS)) {
        return INVALID_SOCKET_HANDLE;
    }

    if (usLocalPort == 0) {
        return (INVALID_LOCAL_PORT);                                     // a local port with number zero is not allowed
    }
    
    ptrUDP += SocketHandle;                                              // bind the port by setting its local port number
    ptrUDP->ucState = UDP_STATE_OPENED;
    ptrUDP->usLocalPort = usLocalPort;
    return (SocketHandle);
}


const unsigned char ucIP_UDP_TYPE[] = {0x00, IP_UDP};

#if !defined IP_RX_CHECKSUM_OFFLOAD || defined IP_INTERFACE_WITHOUT_CS_OFFLOADING || defined _WINDOWS
static int fnCheckUDP_checksum(IP_PACKET *received_ip_packet, unsigned char *upd_data, unsigned short usLength) // {9}
{
    unsigned char ucTemp[2];                                             // we check its checksum
    unsigned short usCheckSum = 0;

    usCheckSum = fnCalcIP_CS(0, received_ip_packet->destination_IP_address, IPV4_LENGTH); // do it first to IP pseudo header
    usCheckSum = fnCalcIP_CS(usCheckSum, received_ip_packet->source_IP_address, IPV4_LENGTH);
    usCheckSum = fnCalcIP_CS(usCheckSum, (unsigned char *)ucIP_UDP_TYPE, sizeof(ucIP_UDP_TYPE));
    ucTemp[0] = (unsigned char)(usLength >> 8);
    ucTemp[1] = (unsigned char)usLength;
    usCheckSum = fnCalcIP_CS(usCheckSum, ucTemp, sizeof(usLength));    

    if (fnCalcIP_CS(usCheckSum, (upd_data - 8), usLength) != IP_GOOD_CS) {
    #if defined _WINDOWS                                                 // avoid potential UDP checksum offload problem when simulating
        return 0;
    #else
        return -1;                                                       // ignore bad check sums
    #endif
    }
    else {
        return 0;                                                        // good checksum
    }
}
#endif

// This is called when a UDP frame has been received
//
extern void fnHandleUDP(ETHERNET_FRAME *ptrRx_frame)                     // {7}
{
    IP_PACKET *received_ip_packet = (IP_PACKET *)&ptrRx_frame->ptEth->ucData;
    UDP_HEADER received_udp;
    USOCKET Socket;
    unsigned short usIP_HeaderLength = ptrRx_frame->usIPLength;
    unsigned char *upd_data = (unsigned char *)received_ip_packet;
    
    upd_data += usIP_HeaderLength;

    received_udp.usSourcePort       = ((*upd_data++) << 8);
    received_udp.usSourcePort      |= *upd_data++;

    received_udp.usDestinationPort  = ((*upd_data++) << 8);
    received_udp.usDestinationPort |= *upd_data++;

    received_udp.usLength           = ((*upd_data++) << 8);
    received_udp.usLength          |= *upd_data++;

    if (received_udp.usLength < UDP_HLEN) {
        return;                                                          // frame too short to be a valid UDP frame so ignore it
    }

    received_udp.ucCheckSum         = ((*upd_data++) << 8);
    received_udp.ucCheckSum        |= *upd_data++;

#if defined USE_IGMP                                                     // {9}
    if ((received_ip_packet->destination_IP_address[0] >= 224) && (received_ip_packet->destination_IP_address[0] <= 239)) { // if multicast destination
    #if !defined IP_RX_CHECKSUM_OFFLOAD || defined IP_INTERFACE_WITHOUT_CS_OFFLOADING || defined _WINDOWS
        if (received_udp.ucCheckSum != 0) {
            if (fnCheckUDP_checksum(received_ip_packet, upd_data, received_udp.usLength) != 0) {
                return;
            }
        }
    #endif
    #if IP_INTERFACE_COUNT > 1                                           // {12}
        fnHandleMulticastRx((ptrRx_frame->ucInterface << INTERFACE_SHIFT), 0, received_ip_packet->destination_IP_address, received_udp.usSourcePort, received_udp.usDestinationPort, upd_data, received_udp.usLength); // allow the received multicast frame to be handled by hosts belonging to the corresponding multi-cast group
    #else
        fnHandleMulticastRx(0, 0, received_ip_packet->destination_IP_address, received_udp.usSourcePort, received_udp.usDestinationPort, upd_data, received_udp.usLength); // allow the received multicast frame to be handled by hosts belonging to the corresponding multi-cast group
    #endif
        return;
    }
#endif
    if ((Socket = fnFindOpenSocket(received_udp.usDestinationPort)) < 0) { // try to find an open socket with the destination port to handle the received message
#if defined USE_ICMP
    #if defined ICMP_DEST_UNREACHABLE
        #if defined SUPPORT_SUBNET_BROADCAST
            #if IP_NETWORK_COUNT > 1                                     // {7}
        register unsigned char ucNetworkID = ptrRx_frame->ucNetworkID;
            #endif
        if ((fnSubnetBroadcast(received_ip_packet->destination_IP_address, &network[_NETWORK_ID].ucOurIP[0], &network[_NETWORK_ID].ucNetMask[0], IPV4_LENGTH) == 0) && (uMemcmp(received_ip_packet->destination_IP_address, cucBroadcast, IPV4_LENGTH) != 0)) // {1}{5}{6} not subnet broadcast nor broadcast
        #else
        if (uMemcmp(received_ip_packet->destination_IP_address, cucBroadcast, IPV4_LENGTH)) // don't send destination unreachable to broadcast messages
        #endif
        {
            if ((received_ip_packet->destination_IP_address[0] < 224) || (received_ip_packet->destination_IP_address[0] > 239)) { // {11} not multi-cast
                ICMP_ERROR tICMP_error;                                      // send ICMP here to inform that no open destination port exists in our system

                tICMP_error.ucICMPType = DESTINATION_UNREACHABLE;            // add ICMP header type 
                tICMP_error.ucICMPCode = PORT_UNREACHABLE;                   // add ICMP header code
                uMemset(&tICMP_error.ucICMPCheckSum, 0, sizeof(tICMP_error.ucICMP_variable) + sizeof(tICMP_error.ucICMPCheckSum)); // zero rest of header

                // We are expected to report the complete IP header plus the start of the IP datagram which was received
                //
                uMemcpy(&tICMP_error.tCopy_IP_header, &received_ip_packet->version_header_length, usIP_HeaderLength + REPORTING_LENGTH_UDP_UNREACHABLE);
                fnSendICMPError(&tICMP_error, (unsigned short)((REPORTING_LENGTH_UDP_UNREACHABLE + ICMP_ERROR_HEADER_LENGTH) + usIP_HeaderLength)); 
            }
        }
    #endif
#endif
        return;
    }
    else {
        UDP_TAB *ptrUDP = tUDP;                                          // we have a listening socket for this UDP frame
        ptrUDP += Socket;
#if !defined IP_RX_CHECKSUM_OFFLOAD || defined IP_INTERFACE_WITHOUT_CS_OFFLOADING || defined _WINDOWS // {2}
        if ((ptrUDP->ucOptions & UDP_OPT_CHECK_CS) && (received_udp.ucCheckSum != 0)) {
            if (fnCheckUDP_checksum(received_ip_packet, upd_data, received_udp.usLength) != 0) { // {9}
                return;
            }   
        }
#endif
#if IP_INTERFACE_COUNT > 1
        Socket |= defineInterface(ptrRx_frame->ucInterface);             // add the interface to the socket details so that direct responses will be sent back to it
#endif
#if IP_NETWORK_COUNT > 1
        Socket |= defineNetwork(ptrRx_frame->ucNetworkID);               // add the network to the socket details
#endif
        // Pass the received frame contents to the listener
        //
        ptrUDP->fnListener(Socket, UDP_EVENT_RXDATA, received_ip_packet->source_IP_address, received_udp.usSourcePort, upd_data, (unsigned short)(received_udp.usLength - UDP_HLEN));
    }
}

static signed short fnTransmitUDP(UDP_TX_PROPERTIES *ptrUDP, unsigned char *ptrBuf, unsigned short usDataLen, UTASK_TASK OwnerTask) // {10}
{
    unsigned char *ptrData;

    if (ptrUDP->usSourcePort == 0) {
        return ZERO_PORT;
    }

    usDataLen += UDP_HLEN;
    if (usDataLen > UDP_SEND_MTU) {
        usDataLen = UDP_SEND_MTU;                                        // if the user tries to send messages larger that possible we simply shorten them
    }

    ptrData = ptrBuf;                        
    
    *ptrBuf++ = (unsigned char)(ptrUDP->usSourcePort >> 8);              // put header into user buffer
    *ptrBuf++ = (unsigned char)ptrUDP->usSourcePort;
    *ptrBuf++ = (unsigned char)(ptrUDP->usDestinationPort >> 8);
    *ptrBuf++ = (unsigned char)ptrUDP->usDestinationPort;
    *ptrBuf++ = (unsigned char)(usDataLen >> 8);
    *ptrBuf++ = (unsigned char)(usDataLen);
    *ptrBuf++ = 0;                                                       // checksum space
    *ptrBuf   = 0;
#if !defined IP_TX_CHECKSUM_OFFLOAD || defined IP_INTERFACE_WITHOUT_CS_OFFLOADING || defined _WINDOWS // {3}{4}
    if (ptrUDP->ucOptions & UDP_OPT_SEND_CS) {                           // calculate checksum if needed
    #if IP_NETWORK_COUNT > 1                                             // {7}
        register unsigned char ucNetworkID = extractNetwork(ptrUDP->SocketHandle);
    #endif
        unsigned char  ucTemp[2];
        unsigned short usCheckSum = 0;
    #if IP_INTERFACE_COUNT > 1
        if (fnGetAnyInterfaceCharacteristics(ptrUDP->SocketHandle, INTERFACE_NO_TX_CS_OFFLOADING) != 0) { // only calculate the checksum when an interface will need it
    #endif
            usCheckSum = fnCalcIP_CS(0, &network[_NETWORK_ID].ucOurIP[0], IPV4_LENGTH); // do it first to IP pseudo header
            usCheckSum = fnCalcIP_CS(usCheckSum, ptrUDP->ucIPAddress, IPV4_LENGTH);
            usCheckSum = fnCalcIP_CS(usCheckSum, (unsigned char *)ucIP_UDP_TYPE, sizeof(ucIP_UDP_TYPE));
            ucTemp[0] = (unsigned char)((usDataLen) >> 8);
            ucTemp[1] = (unsigned char)(usDataLen);
            usCheckSum = fnCalcIP_CS(usCheckSum, ucTemp, sizeof(ucTemp));    
    
            if ((usCheckSum = ~fnCalcIP_CS(usCheckSum, ptrData, usDataLen)) == 0) {
                usCheckSum = 0xffff;                                     // avoid zero as checksum since this signifies no checksum; it is equivalent to 0x0000
            }
            *(ptrBuf - 1) = (unsigned char)(usCheckSum >> 8);            // save checksum in correct place
            *ptrBuf = (unsigned char)(usCheckSum);
    #if IP_INTERFACE_COUNT > 1
        }
    #endif
    }
#endif
    return (fnSendIPv4(ptrUDP->ucIPAddress, IP_UDP, ptrUDP->ucTOS, ptrUDP->ucTTL, ptrData, usDataLen, OwnerTask, ptrUDP->SocketHandle)); // {9}
}


// The client/server call this routine to send a UDP message. The caller ensures that the transmit buffer contains UDP header space
//
extern signed short fnSendUDP(USOCKET SocketHandle, unsigned char *dest_IP, unsigned short usRemotePort, unsigned char *ptrBuf, unsigned short usDataLen, UTASK_TASK OwnerTask)
{
    UDP_TAB *ptrUDP = tUDP;

    if (_UDP_SOCKET_MASK(SocketHandle) > UDP_SOCKETS) {                  // {7}
        return INVALID_SOCKET_HANDLE;
    }
    if ((uMemcmp(dest_IP, cucNullMACIP, IPV4_LENGTH)) == 0) {
        return(INVALID_DEST_IP);
    }
    if (usRemotePort == 0) {
        return INVALID_REMOTE_PORT;
    }
    
    ptrUDP += _UDP_SOCKET_MASK(SocketHandle);                            // {7}

    if (ptrUDP->ucState != UDP_STATE_OPENED) {
        return SOCKET_CLOSED;
    }
    else {
        UDP_TX_PROPERTIES udp_properties;                                // {10}
        udp_properties.SocketHandle = SocketHandle;
        udp_properties.ucIPAddress = dest_IP;
        udp_properties.ucOptions = ptrUDP->ucOptions;
        udp_properties.ucTOS = ptrUDP->ucServiceType;                    // {10}
        udp_properties.ucTTL = MAX_TTL;
        udp_properties.usDestinationPort = usRemotePort;

        udp_properties.usSourcePort = ptrUDP->usLocalPort;
        return (fnTransmitUDP(&udp_properties, ptrBuf, usDataLen, OwnerTask));
    }
}

#if defined USE_IGMP || defined SUPPORT_MULTICAST_TX                     // {9}

extern signed short fnSendUDP_multicast(int iHostID, UDP_TX_PROPERTIES *ptrUDP, unsigned char  *ptrBuf, unsigned short usDataLen) // {10}
{
    signed short sResult;
    if ((*ptrUDP->ucIPAddress < 224) || (*ptrUDP->ucIPAddress > 239)) {
        return BAD_MULTICAST_ADDRESS;
    }
    sResult = fnTransmitUDP(ptrUDP, ptrBuf, usDataLen, 0);               // {10}
    #if defined USE_IGMP
    if ((ptrUDP->ucOptions & UDP_OPT_NO_LOOPBACK) == 0) {                // if the loopback hasn't been disabled for this frame
        fnHandleMulticastRx(ptrUDP->SocketHandle, iHostID, ptrUDP->ucIPAddress, ptrUDP->usSourcePort, ptrUDP->usDestinationPort, (ptrBuf + sizeof(UDP_HEADER)), usDataLen); // let host groups handle the transmitted frame as reception
    }
    #endif
    return sResult;
}
#endif


#if defined ICMP_DEST_UNREACHABLE
 
// ICMP reports if a destination reports that a UDP port was not reachable
//
extern void fnReportUDP(unsigned short usSourcePort, unsigned short usDestPort, unsigned char ucEvent, unsigned char *ucIP)
{
    USOCKET udpSocket;

    if ((udpSocket = fnFindOpenSocket(usSourcePort)) >= 0) {             // find the (open) socket which sent the UDP message
        UDP_TAB *ptrUDP = tUDP + udpSocket;                              // get socket details
        (void)ptrUDP->fnListener(udpSocket, ucEvent, ucIP, usDestPort, 0, 0); // inform the event to the server/client 
    }
}
#endif                                                                   // end UDP ICMP reporting

#endif                                                                   // end UDP support
