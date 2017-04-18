/***********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher

    ---------------------------------------------------------------------
    File:      ip.c
    Project:   Single Chip Embedded Internet
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2016
    *********************************************************************
    08.04.2007 Add SUPPORT_SUBNET_BROADCAST - new function fnSubnetBroadcast() {1}
    25.08.2007 Use static RAM for code from RAM since uMalloc may not always be ready {2}
    11.01.2010 Allow use of fnCalcIP_CS() by IPV6 alone                  {3}
    05.02.2010 Add IPV6                                                  {4}
    06.09.2010 Optimise code when the device performs rx or tx offloading{5}
    06.07.2011 Removed unnecessary copy of received IP options           {6}
    03.03.2012 Remove fnCalcIP_CS() when offloading is enabled           {7}
    11.03.2012 Remove fnHandleIPV6()                                     {8}
    11.03.2012 Redefine fnHandleIPv4()                                   {9}
    25.03.2012 Use IPv6 link-local address for network local protocols   {10}
    15.08.2012 Add SUPPORT_SUBNET_TX_BROADCAST                           {11}
    05.09.2012 Modified fnSubnetBroadcast() parameters to correct all cases {12}
    16.09.2012 Specifically accept broadcasts together with SUPPORT_SUBNET_BROADCAST {13}
    16.12.2012 Add SUPPORT_DYNAMIC_VLAN_TX                               {14}
    17.12.2012 New fnAddARP() interface                                  {15}
    17.12.2012 When sending IP broadcasts send to interface defined by user {16}
    17.12.2012 Correct usCalcIPCheckSum()'s use of source IP when multi-networking is enabled {17}
    06.08.2013 Add SUPPORT_MULTICAST_TX to send UDP multicast transmissions {18}
    06.08.2013 Add SUPPORT_MULTICAST_RX to accept UDP multicast receptions {19}
    12.04.2014 Add IGMP to replace SUPPORT_MULTICAST_RX                  {20}
    13.04.2014 Add fnAssignNetwork() call to associate received frames to a network {21}
    15.04.2014 Add fnGetInterfaceCount() and fnGetInterfaceReference()   {22}
    04.09.2014 Allow user to define which networks respond to broadcasts {23}
    02.01.2016 Optionally support IPv4 transmission fragmentation        {24}
    02.01.2016 Optionally support IPv4 reception de-fragmentation        {25}

*/        


/* =================================================================== */
/*                           include files                             */
/* =================================================================== */
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

#define ETH_IP_HEADER_SIZE     (sizeof(ETHERNET_II) + sizeof(IP_PACKET)) // size of Ethernet and IP header with full options
#define DEFRAG_ENTRY_TTL       10

/* =================================================================== */
/*                      local structure definitions                    */
/* =================================================================== */

#if defined IPV4_SUPPORT_RX_DEFRAGMENTATION                              // {25}
    typedef struct stHOLE_DESCRIPTOR
    {
        unsigned short usFirst;                                          // the number of the first octet in the hole
        unsigned short usLast;                                           // the number of the last octet in the hole
        unsigned char  ucValidHole;
    }   HOLE_DESCRIPTOR;

    typedef struct stDEFRAG_ENTRY
    {
        unsigned char  ucIPv4ForeignLocalAddress[IPV4_LENGTH * 2];
        unsigned short usFragmentRef;                                    // IP identification of the fragmented datagram
        unsigned short usDatagramFullSize;
        unsigned short usBufferEnd;
    #if defined IPV4_SUPPORT_RX_DEFRAGMENTATION && defined IP_RX_CHECKSUM_OFFLOAD
        unsigned short usFragment_CS;                                    // checksum of payload fragment in this frame
    #endif
        unsigned char  ucActive;                                         // active life-time of the defragmentation entry
        unsigned char  ucProtocolID;
        HOLE_DESCRIPTOR hole[MAX_DEFRAG_HOLES];
    #if MAX_DEFRAG_DATAGRAMS > 1
        unsigned short usOffset;                                         // offset in the datagram data pool when shared with other entries
    #endif
    } DEFRAG_ENTRY;
#endif

/* =================================================================== */
/*                 local function prototype declarations               */
/* =================================================================== */

/* =================================================================== */
/*                             constants                               */
/* =================================================================== */

/* =================================================================== */
/*                     global variable definitions                     */
/* =================================================================== */

/* =================================================================== */
/*                      local variable definitions                     */
/* =================================================================== */

#if defined USE_IPV6
    unsigned char ucLinkLocalIPv6Address[IP_NETWORK_COUNT][IPV6_LENGTH] = {{0}};
#endif
#if IP_INTERFACE_COUNT > 1
    QUEUE_HANDLE InterfaceHandles[IP_INTERFACE_COUNT] = {0};
    unsigned char InterfaceCharacteristics[IP_INTERFACE_COUNT] = {0};
#endif
#if defined IPV4_SUPPORT_RX_DEFRAGMENTATION                              // {25}
    static unsigned char *ptrDefragPool = 0;                             // pointer to de-fragmentation data polll
    static DEFRAG_ENTRY defragEntries[MAX_DEFRAG_DATAGRAMS] = {{{0}}};   // de-fragmentation management
#endif

#if defined _WINDOWS || defined IP_INTERFACE_WITHOUT_CS_OFFLOADING || defined FORCE_PAYLOAD_ICMPV6_TX || defined FORCE_PAYLOAD_ICMPV6_RX || (defined USE_IGMP && (!defined IGMP_RX_CHECKSUM_OFFLOAD || !defined IGMP_TX_CHECKSUM_OFFLOAD)) || ((defined USE_IP || defined USE_IPV6) && (!defined IP_RX_CHECKSUM_OFFLOAD || !defined IP_TX_PAYLOAD_CHECKSUM_OFFLOAD || !defined IP_TX_CHECKSUM_OFFLOAD)) // {3}{7}
// Check sum calculation used by IP, ICMP, UDP and TCP
//
#if defined RUN_LOOPS_IN_RAM
unsigned short (*fnCalcIP_CS)(unsigned short cs, unsigned char *dat, unsigned short usLen);
static unsigned short _fnCalcIP_CS(unsigned short cs, unsigned char *dat, unsigned short usLen)
#else
extern unsigned short fnCalcIP_CS(unsigned short cs, unsigned char *dat, unsigned short usLen)
#endif
{
    unsigned char cs_l = (unsigned char)(cs);
    unsigned char cs_h = (unsigned char)(cs >> 8);                       // prime check sum initial value

    while (usLen--) {
        if ((cs_h += *dat) < *dat) {                                     // process MSB
            if ( ++cs_l == 0 ) {
                cs_h++;
            }
        }

        if (0 == usLen--) {
            break;
        }
        dat++;

        if ((cs_l += *dat) < *dat) {                                     // process LSB
            if ( ++cs_h == 0 ) {
                cs_l++;
            }
        }
        dat++;
    }
    return (((unsigned short)cs_h << 8) + cs_l);
}
#endif

#if defined USE_IP
    #ifdef RUN_LOOPS_IN_RAM
extern void fnInitIP(void)                                               // {2}
{
    #if defined _WINDOWS
        #define PROG_SIZE  400
    #else
        #define PROG_SIZE  ((CAST_POINTER_ARITHMETIC)fnInitIP - (CAST_POINTER_ARITHMETIC)_fnCalcIP_CS)
    #endif
    static unsigned long ulProgSpace[PROG_SIZE/sizeof(unsigned long)];   // SRAM code space on long boundary

    uMemcpy(ulProgSpace, (unsigned char*)_fnCalcIP_CS, PROG_SIZE);
    fnCalcIP_CS = (unsigned short(*)(unsigned short, unsigned char *, unsigned short))ulProgSpace;
}
    #endif

    #if defined SUPPORT_SUBNET_BROADCAST || defined SUPPORT_SUBNET_TX_BROADCAST // {11}
// Check to see whether an IP address is a broadcast to our subnet
//
extern int fnSubnetBroadcast(unsigned char *ip_address, unsigned char *our_ip_address, unsigned char *subnet_mask, unsigned char ucLength) // {12}
{
    register unsigned char ucAddress;
    register unsigned char ucMask;
    while (ucLength--) {
        ucAddress = *ip_address++;
        ucMask = *subnet_mask++;
        if ((ucAddress | ucMask) != 0xff) {
            return 0;                                                    // not a broadcast to our subnet
        }
        if ((ucAddress & ucMask) != (*our_ip_address++ & ucMask)) {      // {12} verify that it is really in our sub-net
            return 0;                                                    // doesn't match our subnet
        }
    }
    return 1;                                                            // yes, it is a broadcast to our subnet
}
    #endif

    #if IP_INTERFACE_COUNT > 1
extern void fnEnterInterfaceHandle(unsigned char ucInterface, QUEUE_HANDLE interfaceHandle, unsigned char ucCharacteristics)
{
    if (ucInterface >= IP_INTERFACE_COUNT) {
        return;                                                          // ignore invalid entries
    }
    InterfaceHandles[ucInterface] = interfaceHandle;
    InterfaceCharacteristics[ucInterface] = ucCharacteristics;
}

// This routine takes an interface reference and returns the handle corresponding to it
//
extern QUEUE_HANDLE fnGetInterfaceHandle(int iReference)
{
    if (iReference >= IP_INTERFACE_COUNT) {
        _EXCEPTION("Invalid IP interface handle requested!!!");          // this must never happen
        return 0;
    }
    return (InterfaceHandles[iReference]);                               // return the handle for this interface
}

// This routine takes an interface reference and returns the handle
// corresponding to it
//
extern unsigned char fnGetInterfaceCharacteristics(int iReference)
{
    if (iReference >= IP_INTERFACE_COUNT) {
        _EXCEPTION("Invalid IP interface handle requested!!!");          // this must never happen
        return 0;
    }
    return (InterfaceCharacteristics[iReference]);                       // return the characteristics of this interface
}

extern int fnGetAnyInterfaceCharacteristics(USOCKET uSocket, unsigned char ucCharacteristic)
{
    unsigned char ucInterface = extractInterface(uSocket);               // extract the interface mask bits from the socket
    if (ucInterface != 0) {                                              // if interface specified
        int iInterfaceReference = 0;
        while (ucInterface != 0) {                                       // until the first interface has been found
            if ((ucInterface & 0x01) != 0) {
                if (InterfaceCharacteristics[iInterfaceReference] & ucCharacteristic) {
                    return 1;
                }
            }
            ucInterface >>= 1;
            iInterfaceReference++;
        }
        return 0;
    }
    return (InterfaceCharacteristics[0] & ucCharacteristic);             // if no interface is specified we return the characeteristic of the first (default) interface
}


// This routine takes the handle to an interface which has been previously entered and returns its interface flag
//
extern unsigned char fnGetInterfaceFlag(QUEUE_HANDLE interfaceHandle, int iCharacteristics)
{
    unsigned char ucInterfaceFlag = 0x01;                                // default interface
    int iInterfaceEntry = 0;
    while (iInterfaceEntry < IP_INTERFACE_COUNT) {                       // search the interface handle table
        if (InterfaceHandles[iInterfaceEntry] == interfaceHandle) {
            if (iCharacteristics != 0) {
                return InterfaceCharacteristics[iInterfaceEntry];        // return the interface characterists instead of interface bit
            }
            return ucInterfaceFlag;                                      // return the individual interface bit
        }
        iInterfaceEntry++;
        ucInterfaceFlag <<= 1;
    }
    _EXCEPTION("Invalid IP interface handle given!!!");                  // this must never happen
    return 0;
}

extern int fnGetFirstInterface(USOCKET cSocket)
{
    unsigned char ucInterface = extractInterface(cSocket);               // extract the interface mask bits from the socket
    if (ucInterface != 0) {                                              // if interface specified
        int iInterfaceReference = 0;
        while (ucInterface != 0) {                                       // until the first interface has been found
            if (ucInterface & 0x01) {
                return iInterfaceReference;
            }
            ucInterface >>= 1;
            iInterfaceReference++;
        }
    }
    return 0;                                                            // if no interface is specified we return the first (default)
}
    #endif

#if IP_INTERFACE_COUNT > 1 && defined IP_INTERFACE_WITHOUT_CS_OFFLOADING
static unsigned short fnRxCalcIP_CS(unsigned short cs, ETHERNET_FRAME *frame, unsigned short usLen)
{
    if (frame->ucInterfaceHandling & INTERFACE_NO_RX_CS_OFFLOADING) {    // if the frame was received on an interface that doesn't perform automatic checksum verificatio we need to do it in software
        return fnCalcIP_CS(cs, frame->ptEth->ucData, usLen);
    }
    return IP_GOOD_CS;                                                   // an interface with automatic checksum verification doesn't need to be checked again
}
#else
    #define fnRxCalcIP_CS(cs, frame, usLength) fnCalcIP_CS(cs, frame->ptEth->ucData, usLength)
#endif

#if defined IPV4_SUPPORT_RX_DEFRAGMENTATION                              // {25}
static unsigned short fnMaxDatagramSize(int iDatagramEntry)
{
    #if MAX_DEFRAG_DATAGRAMS > 1
    int iEntries;
    defragEntries[iDatagramEntry].usOffset = 0;                          // default is to allocate the start of the pool for this datagram
    for (iEntries = 0; iEntries < MAX_DEFRAG_DATAGRAMS; iEntries++) {
        if (iDatagramEntry != iEntries) {                                // skip caller's datagram entry
            if (defragEntries[iEntries].ucActive != 0) {                 // de-fragmentation entry is in use
                int iHole;
                int iMaxHole = 0;
                unsigned short usUsed = 0;
                unsigned short usMax = 0;
                for (iHole = 0; iHole < MAX_DEFRAG_HOLES; iHole++) {     // check all holes belonging to this entry
                    if (defragEntries[iEntries].hole[iHole].ucValidHole != 0) { // if the hole is valid
                        if (defragEntries[iEntries].hole[iHole].usFirst > usUsed) {
                            usUsed = defragEntries[iEntries].hole[iHole].usFirst; // presently occupied length
                        }
                        if (defragEntries[iEntries].hole[iHole].usLast > usMax) {
                            usMax = defragEntries[iEntries].hole[iHole].usLast;
                            iMaxHole = iHole;                            // the hole at the end of the buffer
                        }
                    }
                }
                usUsed += ETH_IP_HEADER_SIZE;                            // physical size of buffer in use
                if (usUsed > (DEFRAG_MEMORY_POOL_SIZE/2)) {              // if more that half of the buffer is already in use
                    return 0;                                            // don't disturb present pool
                }
                defragEntries[iEntries].hole[iMaxHole].usLast = (((DEFRAG_MEMORY_POOL_SIZE/2) - ETH_IP_HEADER_SIZE) - 1); // limit the size of the reconstruction in progress
                if (defragEntries[iEntries].usOffset == 0) {             // entry using start of the pool
                    defragEntries[iDatagramEntry].usOffset = (DEFRAG_MEMORY_POOL_SIZE/2); // allocate the second half of the pool for this daragram
                }
                return (((DEFRAG_MEMORY_POOL_SIZE/2) - ETH_IP_HEADER_SIZE) - 1); // half of the pool is available
            }
        }
    }
    return (DEFRAG_MEMORY_POOL_SIZE - ETH_IP_HEADER_SIZE - 1);           // complete pool presently available
    #else
    return (DEFRAG_MEMORY_POOL_SIZE - ETH_IP_HEADER_SIZE - 1);           // complete pool is available
    #endif
}

static int fnGetHole(int iDatagramEntry, unsigned short usStart, unsigned short usEnd)
{
    int iHole;
    for (iHole = 0; iHole < MAX_DEFRAG_HOLES; iHole++) {
        if (defragEntries[iDatagramEntry].hole[iHole].ucValidHole == 0) { // free entry found
            defragEntries[iDatagramEntry].hole[iHole].usFirst = usStart;
            defragEntries[iDatagramEntry].hole[iHole].usLast = usEnd;
            defragEntries[iDatagramEntry].hole[iHole].ucValidHole = 1;   // hole entry is now valid
            return iHole;                                                // the entry being used
        }
    }
    return -1;                                                           // no free holes available
}

static void fnDeleteDefragEntry(int iDatagramEntry)
{
    uMemset(&defragEntries[iDatagramEntry], 0, sizeof(defragEntries[iDatagramEntry]));
}

static int fnGetFragEntry(IP_PACKET *received_ip_packet, unsigned short usFragmentSize, ETHERNET_FRAME *frame, unsigned char ucNetworkID)
{
    int iDatagramID;
    int iMatchDatagram = 0;
    unsigned short usIpID = ((received_ip_packet->identification[0] << 8) | received_ip_packet->identification[1]); // IP datagram identification
    unsigned short usFragOffset;
    unsigned short usFragStart;
    usFragOffset = ((received_ip_packet->fragment_offset[0] << 8) | received_ip_packet->fragment_offset[1]); // the offset of this fragment in the complete datagram
    usFragOffset &= (IP_FRAGMENT_OFFSET_BITS);
    usFragStart = (usFragOffset * 8);
    for (iDatagramID = 0; iDatagramID < MAX_DEFRAG_DATAGRAMS; iDatagramID++) {
        if (defragEntries[iDatagramID].ucActive != 0) {                  // this entry is in use
            if (defragEntries[iDatagramID].usFragmentRef == usIpID) {    // reassembly entry is matched by the identification field, protocol ID and foreign and local IP addresses
                if (defragEntries[iDatagramID].ucProtocolID == received_ip_packet->ip_protocol) {
                    if (uMemcmp(defragEntries[iDatagramID].ucIPv4ForeignLocalAddress, received_ip_packet->source_IP_address, sizeof(defragEntries[iDatagramID].ucIPv4ForeignLocalAddress)) == 0) {
                        break;                                           // full match so it is accepted
                    }
                }
            }
            iMatchDatagram++;
        }
    }
    if (iMatchDatagram < MAX_DEFRAG_DATAGRAMS) {                         // this entry is to be used for this datagram
    #if defined IPV4_SUPPORT_RX_DEFRAGMENTATION && defined IP_RX_CHECKSUM_OFFLOAD
        unsigned long ulNewCS;
    #endif
        int iHole = 0;
        int iListCount = 0;
        int iNewCount = 0;
        unsigned short usOriginalLast;
        if (defragEntries[iMatchDatagram].ucActive == 0) {               // this is a new datagram to be managed
            unsigned short usBufferEnd = fnMaxDatagramSize(iMatchDatagram);
            if (usBufferEnd == 0) {
                return -1;                                               // no free buffer space
            }
            if (fnGetHole(iMatchDatagram, 0, usBufferEnd) < 0) {         // create a default hole for maximum datagram total size (this will alway be the first entry)
                return -1;                                               // no hole is available (probably due to lack of memory to receive a large datagram)
            }
            defragEntries[iMatchDatagram].usFragmentRef = usIpID;        // the IP datagram reference
            defragEntries[iMatchDatagram].ucProtocolID = received_ip_packet->ip_protocol;
            uMemcpy(defragEntries[iMatchDatagram].ucIPv4ForeignLocalAddress, received_ip_packet->source_IP_address, sizeof(defragEntries[iMatchDatagram].ucIPv4ForeignLocalAddress));
            defragEntries[iMatchDatagram].usBufferEnd = usBufferEnd;
        }
    #if defined IPV4_SUPPORT_RX_DEFRAGMENTATION && defined IP_RX_CHECKSUM_OFFLOAD
        ulNewCS = (defragEntries[iMatchDatagram].usFragment_CS + frame->usFragment_CS);
        defragEntries[iMatchDatagram].usFragment_CS = (unsigned short)ulNewCS;
        if (ulNewCS > 0xffff) {
            defragEntries[iMatchDatagram].usFragment_CS++;
        }
    #endif
        defragEntries[iMatchDatagram].ucActive = DEFRAG_ENTRY_TTL;       // retrigger the life-time of the datagram management entry
        for (iHole = 0; iHole < MAX_DEFRAG_HOLES; iHole++) {             // check all holes belonging to this entry
            if (defragEntries[iMatchDatagram].hole[iHole].ucValidHole == 0) { // if the hole is not in use
                continue;
            }
            iListCount++;                                                // count the valid hole entries
            if (usFragStart > defragEntries[iMatchDatagram].hole[iHole].usLast) { // if the start of the new fragment is beyond the end of this hole
                continue;
            }
            if ((usFragStart + usFragmentSize) < defragEntries[iMatchDatagram].hole[iHole].usFirst) { // if the end of the new fragment is before the start of this hole
                continue;
            }
            iListCount--;                                                // this fragment can be located in this hole
            if ((usFragStart + usFragmentSize) > (defragEntries[iMatchDatagram].hole[iHole].usLast + 1)) { // check that there is enough re-generation pool space for the data
                fnDeleteDefragEntry(iMatchDatagram);                     // the size of the payload is greater than the buffer space available so has to be discarded
                return -1;
            }
            uMemcpy((ptrDefragPool + usFragStart + defragEntries[iMatchDatagram].usOffset + ETH_IP_HEADER_SIZE), received_ip_packet->ip_options, usFragmentSize); // save the datagram fragment to the reassembly buffer
            if (usFragStart == 0) {                                      // the fragment containing the IP header
                uMemcpy((ptrDefragPool + defragEntries[iMatchDatagram].usOffset + MAX_IP_OPTLEN - (frame->usIPLength - IP_MIN_HLEN)), frame->ptEth, (ETH_IP_HEADER_SIZE - MAX_IP_OPTLEN)); // add the frame header in first fragment
            }
            if (defragEntries[iMatchDatagram].usDatagramFullSize < (usFragStart + usFragmentSize)) {
                defragEntries[iMatchDatagram].usDatagramFullSize = (unsigned short)(usFragStart + usFragmentSize); // the total length known up to now
            }
            defragEntries[iMatchDatagram].hole[iHole].ucValidHole = 0;   // delete the present entry from the hole descriptor list
            usOriginalLast = defragEntries[iMatchDatagram].hole[iHole].usLast; // save the original end of invalidated hole
            if (usFragStart > defragEntries[iMatchDatagram].hole[iHole].usFirst) { // if the new fragment doesn't fit at the start of the original hole
                unsigned short usBufferEnd = (usFragStart - 1);
                if (usBufferEnd > defragEntries[iMatchDatagram].usBufferEnd) {
                    usBufferEnd = defragEntries[iMatchDatagram].usBufferEnd;
                }
                if (fnGetHole(iMatchDatagram, defragEntries[iMatchDatagram].hole[iHole].usFirst, usBufferEnd) < 0) { // create a hole for the area before this fragment
                    fnDeleteDefragEntry(iMatchDatagram);                 // a hole could not be created
                    return -1;
                }
                iNewCount++;                                             // count new hole entry
            }
            if (((usFragStart + usFragmentSize) < usOriginalLast) && ((received_ip_packet->fragment_offset[0] & (IP_MORE_FRAGMENTS >> 8)) != 0)) { // the new fragment doesn't fill to the end of the original hole
                if (fnGetHole(iMatchDatagram, (unsigned short)((usFragStart + usFragmentSize) + 1), usOriginalLast) < 0) { // create a new hole from the end of the fragment to the end of the original hole
                    fnDeleteDefragEntry(iMatchDatagram);                 // a hole could not be created
                    return -1;
                }
                iNewCount++;                                             // count new hole entry
            }
            if (iNewCount != 0) {                                        // if holes were created
                iHole = -1;                                              // restart the process
                iNewCount = 0;
                iListCount = 0;
            }
        }
        if (iListCount == 0) {                                           // the hole descriptor list is now empty so the datagram is complete
            frame->ptEth = (ETHERNET_FRAME_CONTENT *)(ptrDefragPool + defragEntries[iMatchDatagram].usOffset + MAX_IP_OPTLEN - (frame->usIPLength - IP_MIN_HLEN)); // set frame pointer to the reconstructed content
            frame->usDataLength = defragEntries[iMatchDatagram].usDatagramFullSize;
    #if defined IPV4_SUPPORT_RX_DEFRAGMENTATION && defined IP_RX_CHECKSUM_OFFLOAD
            frame->usFragment_CS = defragEntries[iMatchDatagram].usFragment_CS;
        #if defined _WINDOWS
            frame->usFragment_CS = 0xffff;
        #endif
            frame->ucSpecialHandling |= INTERFACE_RX_PAYLOAD_CS_FRAGS;   // mark that the payload checksum is in the usFragment_CS field
    #endif
            fnDeleteDefragEntry(iMatchDatagram);                         // ensure that the datagram management is reset
            return 1;                                                    // de-fragmentation complete
        }
        return 0;                                                        // fragment successfully handled, but not complete
    }
    return -1;                                                           // no free entries
}

static void fnCheckDefragBuffers(void)
{
    int iEntries;
    for (iEntries = 0; iEntries < MAX_DEFRAG_DATAGRAMS; iEntries++) {
        if (defragEntries[iEntries].ucActive != 0) {                     // de-fragmentation entry is in use
            if (--defragEntries[iEntries].ucActive == 0) {               // count down reaches end of life time
                fnDeleteDefragEntry(iEntries);                           // reset the datagram management entry so that it can be reused
            }
        }
    }
}
#endif

// Handle a received IP frame
//
extern int fnHandleIPv4(ETHERNET_FRAME *frame)                           // {9}
{
    #if IP_NETWORK_COUNT > 1
    register unsigned char ucNetworkID;
    #endif
    #if IP_INTERFACE_COUNT > 1
    register unsigned char Tx_handle = frame->Tx_handle;
    #endif
    unsigned short tLen;
    unsigned char ucOptionsLength;
    IP_PACKET *received_ip_packet;
    ARP_DETAILS arp_details;
    arp_details.ucType = ARP_TEMP_IP;
    #if IP_INTERFACE_COUNT > 1
    arp_details.Tx_handle = Tx_handle;
    #endif
    #if defined ARP_VLAN_SUPPORT
    arp_details.usVLAN_ID = frame->usVLAN_ID;
    #endif

    if ((frame->ptEth->ethernet_frame_type[1] != (unsigned char)PROTOCOL_IPv4) // not IP frame
       || (frame->frame_size < ETH_HEADER_LEN) || ((frame->frame_size - ETH_HEADER_LEN) < IP_MIN_HLEN) // invalid length
       ) {
        return 0;                                 
    }
    received_ip_packet = (IP_PACKET *)frame->ptEth->ucData;              // use a structure for interpretation of frame

    if ((received_ip_packet->version_header_length & IP_VERSION_MASK) != (IPV4_LENGTH << 4)) { // check IP version
        return 0;                                                        // not IPv4
    }
        
    tLen = received_ip_packet->total_length[0];
    tLen <<= 8; 
    tLen += received_ip_packet->total_length[1];
    #if !defined IP_RX_CHECKSUM_OFFLOAD || defined IP_INTERFACE_WITHOUT_CS_OFFLOADING || defined _WINDOWS // {5}    
    if (tLen > (frame->frame_size - ETH_HEADER_LEN)) {
        return 0;                                                        // length error
    }
    #endif
    #if IP_NETWORK_COUNT > 1
    frame->ucBroadcastResponse = 0;                                      // {23} default is no response to broadcasts - the user must decide the behaviour
    ucNetworkID = fnAssignNetwork(frame, received_ip_packet->destination_IP_address); // {21}
    if (ucNetworkID == NETWORK_REJECT) {
        return 0;                                                        // ignore the reception if the user decides to reject the frame
    }
    arp_details.ucNetworkID = frame->ucNetworkID = ucNetworkID;
    #endif
    if ((uMemcmp(received_ip_packet->destination_IP_address, &network[_NETWORK_ID].ucOurIP[0], IPV4_LENGTH)) // check whether we are addressed (unicast address)
    #if IP_NETWORK_COUNT > 1                                             // {23}
        && (frame->ucBroadcastResponse == 0)                             // user has defined that broadcast, or subnet broadcast should be responded to on one or more networks
    #else
        #if defined SUPPORT_SUBNET_BROADCAST                             // {1}
        && ((fnSubnetBroadcast(received_ip_packet->destination_IP_address, &network[_NETWORK_ID].ucOurIP[0], &network[_NETWORK_ID].ucNetMask[0], IPV4_LENGTH) == 0) && (uMemcmp(received_ip_packet->destination_IP_address, cucBroadcast, IPV4_LENGTH))) // {12}{13}
        #else
        && (uMemcmp(received_ip_packet->destination_IP_address, cucBroadcast, IPV4_LENGTH))
        #endif
    #endif
    #if defined USE_IGMP                                                 // {19}{20}
        && ((received_ip_packet->destination_IP_address[0] < 224) || (received_ip_packet->destination_IP_address[0] > 239)) // not multicast address range
    #endif
        ) {
    #if defined USE_IP_STATS
        fnRxStats((unsigned char)(received_ip_packet->ip_protocol | FOREIGN_FRAME), _NETWORK_ID); // count protocol frames above IPv4 on foreign IP address
    #endif
    #if defined USE_ICMP
                                                                         // not our address, but check whether ICMP
        if (received_ip_packet->ip_protocol != IP_ICMP) {                // if not ICMP protocol we don't accept foreign IP addresses
            return 0;        
        }
        if (uMemcmp(frame->ptEth->ethernet_destination_MAC, &network[_NETWORK_ID].ucOurMAC[0], MAC_LENGTH)) { // if we are not (MAC) addressed quit
            return 0;
        }
    #else
        return 0;
    #endif
    }
    #if defined USE_IP_STATS
    else {
        fnRxStats(received_ip_packet->ip_protocol, _NETWORK_ID);         // count protocol frames above IPv4 for our IP address
    }
    #endif
    ucOptionsLength = (((received_ip_packet->version_header_length & IP_HEADER_LENGTH_MASK) << 2) - IP_MIN_HLEN); // check options    
    if (ucOptionsLength > MAX_IP_OPTLEN) {
        return 0;                                                        // option too long
    }
    if (ucOptionsLength > (frame->frame_size - ETH_HEADER_LEN - IP_MIN_HLEN)) {
        return 0;                                                        // option field too long
    }

  //uMemcpy(received_ip_packet->ip_options, (frame->ptEth->ucData + IP_MIN_HLEN), ucOptionsLength); // copy option field {6}
    #if !defined IP_RX_CHECKSUM_OFFLOAD || defined IP_INTERFACE_WITHOUT_CS_OFFLOADING || defined _WINDOWS // {5}    
    if (IP_GOOD_CS != fnRxCalcIP_CS(0, frame, (unsigned short)(IP_MIN_HLEN + ucOptionsLength))) {
        #if !defined _WINDOWS                                            // Win7 offloading is difficult to disable so ignore
        return 0;                                                        // check sum error - quit    
        #endif
    }
    #endif
    if (uMemcmp(received_ip_packet->source_IP_address, cucBroadcast, IPV4_LENGTH) != 0) { // add the address to ARP cache as long as not broadcast
        fnAddARP(received_ip_packet->source_IP_address, frame->ptEth->ethernet_source_MAC, &arp_details); // {15}
    }
    frame->usIPLength = (IP_MIN_HLEN + ucOptionsLength);
    frame->usDataLength = (tLen - (IP_MIN_HLEN + ucOptionsLength));      // {9}
    #if defined IPV4_SUPPORT_RX_DEFRAGMENTATION                          // {25}
    fnCheckDefragBuffers();                                              // use IP activity to count down de-fragmentation buffers that haven't completed
    #endif
    if ((received_ip_packet->fragment_offset[1] != 0) || (received_ip_packet->fragment_offset[0] & ((IP_MORE_FRAGMENTS | IP_FRAGMENT_OFFSET_BITS) >> 8))) { // if further IP fragments will follow or the there is a frament offset indicated
    #if defined IPV4_SUPPORT_RX_DEFRAGMENTATION                          // {25}
        if (ptrDefragPool == 0) {                                        // if no de-fragmentation has been required before
            ptrDefragPool = (unsigned char *)uMalloc(DEFRAG_MEMORY_POOL_SIZE); // obtain memory
            if (ptrDefragPool == 0) {                                    // if no memory available
                return 0;                                                // silently discard fragmented frame
            }
        }
        if (fnGetFragEntry(received_ip_packet, frame->usDataLength, frame, _NETWORK_ID) <= 0) { // see whether there is already an entry for this ID and create oe if not
            return 0;                                                    // de-fragmentation not yet ready complete or frame had to be dropped due to no resources
        }
    #else
        return 0;                                                        // fragmented packets not supported (they are dropped)
    #endif
    }
    return (IP_MIN_HLEN + ucOptionsLength);                              // return length of IP header
}

// This quickly extracts IP info from an embedded ICMP frame - since we sent the embedded frame and its contents have been checked, we do minimum work
//
extern int fnHandleIP_ICMP(ETHERNET_FRAME *frame, IP_PACKET *received_ip_packet)
{
    unsigned char *ptrData;
    unsigned char olen;

    ptrData = frame->ptEth->ethernet_destination_MAC;                    // we have no mac addresses so use first location as data

    received_ip_packet->version_header_length = *ptrData;
    ptrData += 2;
    received_ip_packet->total_length[0]  = *ptrData++;
    received_ip_packet->total_length[1]  = *ptrData++;
    ptrData += 5;
    received_ip_packet->ip_protocol = *ptrData++;
    ptrData += 2;
    uMemcpy(received_ip_packet->source_IP_address, ptrData, IPV4_LENGTH);
    ptrData += IPV4_LENGTH;
    uMemcpy(received_ip_packet->destination_IP_address, ptrData, IPV4_LENGTH);
    ptrData += IPV4_LENGTH;
    olen = ((received_ip_packet->version_header_length & IP_HEADER_LENGTH_MASK) << 2) - IP_MIN_HLEN;
    return (IP_MIN_HLEN + olen);                                         // return IP header actual size
}

    #if IP_INTERFACE_COUNT > 1
// This function counts the number of individual interfaces referenced by the USOCKET value
//
extern int fnGetInterfaceCount(USOCKET cSocket)                          // {22}
{
    int iInterfaceCount = 0;
    unsigned char ucInterfaces = extractInterface(cSocket);              // there will be one or more interface flags defined
    while (ucInterfaces != 0) {                                          // while interface flags exist
        if ((ucInterfaces & 0x1) != 0) {                                 // check single interface flag
            iInterfaceCount++;                                           // count it
        }
        ucInterfaces >>= 1;                                              // shift to next interface flag
    }
    return iInterfaceCount;                                              // the number of interfaces involved
}

// This function returns an index to the first interface in the USOCKET value
// - it is generally used when it is known that there is only one interface contained in USOCKET and converts this to its index
//
extern int fnGetInterfaceReference(USOCKET cSocket)                      // {22}
{
    int iInterfaceRef = 0;
    unsigned char ucInterfaceMask = extractInterface(cSocket);           // there will be one or more interface flags defined
    if (ucInterfaceMask != 0) {
        while ((ucInterfaceMask & 0x1) == 0) {                           // check single interface flag that is not active
            ucInterfaceMask >>= 1;                                       // shift to next interface flag
            iInterfaceRef++;                                             // increment the interface reference
        }
    }
    return iInterfaceRef;                                                // the interface reference
}
    #endif

static unsigned short fnInsertIP_checksum(unsigned char *ptrData, unsigned char *prIP_to, unsigned char ucNetworkID, int iCalculateCS)
{
    #if (!defined IP_TX_CHECKSUM_OFFLOAD && !defined IP_TX_PAYLOAD_CHECKSUM_OFFLOAD) || defined IP_INTERFACE_WITHOUT_CS_OFFLOADING || defined _WINDOWS // {5}
    if (iCalculateCS != 0) {
        unsigned short usCheckSum = fnCalcIP_CS(0, (ptrData - 10), 10); 
        usCheckSum = fnCalcIP_CS(usCheckSum, &network[ucNetworkID].ucOurIP[0], IPV4_LENGTH);
        usCheckSum = fnCalcIP_CS(usCheckSum, prIP_to, IPV4_LENGTH);
        usCheckSum = ~usCheckSum;                                            // we do not support options after the IP addresses !!
        *ptrData++  = (unsigned char)((usCheckSum) >> 8);                    // insert the checksum value
        *ptrData    = (unsigned char)(usCheckSum);
        return usCheckSum;
    }
    else {
        *ptrData++  = 0;
        *ptrData    = 0;
    }
    #else
    *ptrData++  = 0;
    *ptrData    = 0;
    #endif
    return 0;
}


// Build and transmit an IP frame with payload
//
extern signed short fnSendIPv4(unsigned char *prIP_to, unsigned char ucProtType, unsigned char ucTypeOfService, unsigned char ucTTL, unsigned char *dat, unsigned short usLen, UTASK_TASK Owner, USOCKET cSocket)
{
    static const unsigned char ucIP_ProtV4[] = {(unsigned char)(PROTOCOL_IPv4 >> 8), (unsigned char)(PROTOCOL_IPv4)};
    static unsigned short usIP_identification_field = 0;                 // IP identification field content initially zero and is incremented in every IP frame that is sent
    #if defined USE_IGMP && (defined USE_IGMP_V2 || defined USE_IGMP_V3) // {20}
        #define MAX_IP (ETH_HEADER_LEN + IP_MIN_HLEN + ROUTER_ALERT_OPTION_LENGTH) // router alert option space
        unsigned char ucOptionLength = 0;
        #define OPTION_LENGTH ucOptionLength
        #if ((!defined IP_TX_CHECKSUM_OFFLOAD && !defined IP_TX_PAYLOAD_CHECKSUM_OFFLOAD) || defined IP_INTERFACE_WITHOUT_CS_OFFLOADING || defined _WINDOWS)
    unsigned short usCheckSum;
        #endif
    #else
        #define MAX_IP (ETH_HEADER_LEN + IP_MIN_HLEN)                    // no options supported
        #define OPTION_LENGTH 0
    #endif
    ARP_TAB *ptrARP;
    #if IP_NETWORK_COUNT > 1
    register unsigned char ucNetworkID;
    #endif
    #if IP_INTERFACE_COUNT > 1
    register QUEUE_HANDLE Tx_handle = 0;
    #endif
    unsigned short i;
    #if defined SUPPORT_MULTICAST_TX || defined USE_IGMP                 // {18}
    ARP_TAB UDP_multicast_ARP;
    #endif
    #if defined IP_TX_CHECKSUM_OFFLOAD && !defined IP_INTERFACE_WITHOUT_CS_OFFLOADING && !defined _WINDOWS
    unsigned char ucTxInterfaceHandling = 0;
    #else
    unsigned char ucTxInterfaceHandling = INTERFACE_NO_TX_CS_OFFLOADING;
    #endif
    #if defined IPV4_SUPPORT_TX_FRAGMENTATION                            // {24}
    unsigned short usRemainder;
    unsigned short usFragmentOffset = 0;
    int iReturn;
    #endif
    unsigned char ucData[MAX_IP];                                        // space for temporary Ethernet II and IP header
    
    #if defined SUPPORT_MULTICAST_TX || defined USE_IGMP                 // {18}
    if (((ucProtType != IP_UDP) && (ucProtType != IP_IGMPV2)) || (*prIP_to < 224) || (*prIP_to > 239)) { // {20} if not a UDP multicast destination use standard ARP resolution
        if ((ptrARP = fnGetIP_ARP(prIP_to, Owner, cSocket)) == 0) {      // see whether IP is in ARP table
            return NO_ARP_ENTRY;                                         // ARP will normally try to resolve the address here and we receive notification when it has completed
        }
    }
    else {                                                               // transmission to an IGMP or UDP multicast address
        #if IP_INTERFACE_COUNT > 1
        int iInterfaceCount = fnGetInterfaceCount(cSocket);              // see how many interfaces the transmission is for
        if (iInterfaceCount > 1) {                                       // if more that one interface is defined for transmission
            int uReference = 0;
            USOCKET this_interface = (USOCKET)(cSocket & ~(INTERFACE_MASK << INTERFACE_SHIFT)); // details without interface information
            USOCKET interfaces = (0x1 << INTERFACE_SHIFT);              // first possible interface
            unsigned short usUDPchecksum = ((*(dat + 6) << 8) | *(dat + 7)); // backup the original UDP checksum so that it can be restored if needed
            while ((interfaces & (INTERFACE_MASK << INTERFACE_SHIFT)) != 0) {
                if (cSocket & interfaces) {                              // multicast frame is to be sent on this interface
                    if ((ucProtType == IP_UDP) && (usUDPchecksum != 0)) {// multicast UDP with a UDP checksum
                        if ((fnGetInterfaceCharacteristics(uReference) & INTERFACE_NO_TX_CS_OFFLOADING) == 0) { // if this interface performes checksum offloading we must ensure that the UDP checksum is set to zero
                            *(dat + 6) = 0;                           // set the UDP checksum in the frame zu zero
                            *(dat + 7) = 0;
                        }
                    }
                    fnSendIPv4(prIP_to, ucProtType, ucTypeOfService, ucTTL, dat, usLen, Owner, (USOCKET)(this_interface | interfaces)); // send the multicast frame to each of the interfaces
                    if (usUDPchecksum != 0) {                            // restore the original UDP checksum in case it is needed by a following interface
                        *(dat + 6) = (unsigned char)(usUDPchecksum >> 8);
                        *(dat + 7) = (unsigned char)(usUDPchecksum);
                    }
                }
                interfaces <<= 1;                                        // repeat for all possible interfaces
                uReference++;
            }
            return usLen;
        }
        #endif
        ptrARP = &UDP_multicast_ARP;                                     // use a dummy ARP entry to define the MAC address to be sent to
        #if IP_INTERFACE_COUNT > 1
        ptrARP->ucState = ARP_BROADCAST_ENTRY;                           // handle as a broadcast, whereby the user defines the interface that is used
        #endif
        ptrARP->ucMac[0] = 0x01;                                         // IANA owned multicast ethernet address block
        ptrARP->ucMac[1] = 0x00;
        ptrARP->ucMac[2] = 0x5e;                                         // enter the multicast MAC address belonging to IANA for this purpose
        ptrARP->ucMac[3] = (*(prIP_to + 1) & 0x7f);                      // add 23 bits of the destination IP address
        ptrARP->ucMac[4] = *(prIP_to + 2);
        ptrARP->ucMac[5] = *(prIP_to + 3);
        #if defined USE_IGMP && (defined USE_IGMP_V2 || defined USE_IGMP_V3)
        if (ucProtType == IP_IGMPV2) {
            ucOptionLength = ROUTER_ALERT_OPTION_LENGTH;                 // add router alert option to IGMP transmissions
        }
        #endif
    }
    #else
    if ((ptrARP = fnGetIP_ARP(prIP_to, Owner, cSocket)) == 0) {          // see whether IP is in ARP table
        return NO_ARP_ENTRY;                                             // ARP will normally try to resolve the address here and we receive notification when it has completed
    }
    #endif

    #if IP_NETWORK_COUNT > 1                                             // use the network and interface as detailed by the sender (not ARP entry)
    ucNetworkID = extractNetwork(cSocket);
    #endif
    #if IP_INTERFACE_COUNT > 1
    if (ptrARP->ucState & ARP_BROADCAST_ENTRY) {                         // {16} when sending broadcasts the caller's interface is used rather than the ARP entry
        int iInterfaceReference = fnGetFirstInterface(cSocket);
        Tx_handle = fnGetInterfaceHandle(iInterfaceReference);           // send broadcasts only to first interface found
    }
    else {
        Tx_handle = ptrARP->Tx_handle;                                   // use the interface according to ARP entry
    }
        #if defined IP_INTERFACE_WITHOUT_CS_OFFLOADING
    ucTxInterfaceHandling = (fnGetInterfaceFlag(Tx_handle, 1) & INTERFACE_NO_TX_CS_OFFLOADING); // determine whether this interface performs checksum offloading or not
        #endif
    #endif
    uMemcpy(&ucData[0], ptrARP->ucMac, MAC_LENGTH);                      // add datalink (Ethernet addresses) information
    uMemcpy(&ucData[MAC_LENGTH], &network[_NETWORK_ID].ucOurMAC[0], MAC_LENGTH);
    uMemcpy(&ucData[2 * MAC_LENGTH], ucIP_ProtV4, sizeof(ucIP_ProtV4));
    #if defined IPV4_SUPPORT_TX_FRAGMENTATION                            // {24}
    do {                                                                 // send as many fragments as required
        i = (2 * MAC_LENGTH + sizeof(ucIP_ProtV4));
        if (usLen > ETH_MTU) {                                           // if the datagram payload is larger than can be carried in the MTU
            unsigned short usPayloadMax = (ETH_MTU - IP_MIN_HLEN);
        #if defined USE_IGMP && (defined USE_IGMP_V2 || defined USE_IGMP_V3)
            usPayloadMax -= ucOptionLength;
        #endif
        #if defined EMAC_ENHANCED && defined IP_TX_CHECKSUM_OFFLOAD && defined IP_TX_PAYLOAD_CHECKSUM_OFFLOAD
            fnSetFragmentMode(1);
        #endif
            usRemainder = (usLen - usPayloadMax);                        // the remaining payload content after this frame transmission
            usLen = usPayloadMax;                                        // reduce the datagram payload
        }
        else {
            usRemainder = 0;                                             // no fragmentation required for this frame
        }
    #else
        i = (2 * MAC_LENGTH + sizeof(ucIP_ProtV4));
    #endif
        ucData[i++]  = (IP_DEFAULT_VERSION_HEADER_LENGTH + OPTION_LENGTH/4); // construct the IP header with no option
        ucData[i++]  = ucTypeOfService;
        ucData[i++]  = (unsigned char)((IP_MIN_HLEN + OPTION_LENGTH + usLen) >> 8);
        ucData[i++]  = (unsigned char)(IP_MIN_HLEN + OPTION_LENGTH + usLen);
        ucData[i++]  = (unsigned char)((usIP_identification_field) >> 8);
        ucData[i++]  = (unsigned char)(usIP_identification_field++);
    #if defined IPV4_SUPPORT_TX_FRAGMENTATION                            // {24}
        ucData[i] = (unsigned char)(usFragmentOffset >> 8);
        if (usRemainder != 0) {
            ucData[i] |= (unsigned char)(IP_MORE_FRAGMENTS >> 8);
        }
        i++;
        ucData[i++] = (unsigned char)usFragmentOffset;
    #else
        ucData[i++]  = 0;                                                // neither flags nor fragment offset
        ucData[i++]  = 0;
    #endif
        ucData[i++]  = ucTTL;
        ucData[i++]  = ucProtType;
    #if (defined USE_IGMP && (defined USE_IGMP_V2 || defined USE_IGMP_V3)) && ((!defined IP_TX_CHECKSUM_OFFLOAD && !defined IP_TX_PAYLOAD_CHECKSUM_OFFLOAD) || defined IP_INTERFACE_WITHOUT_CS_OFFLOADING || defined _WINDOWS)
        usCheckSum = fnInsertIP_checksum(&ucData[i], prIP_to, _NETWORK_ID, ucTxInterfaceHandling); // insert IP checksum (unless the interface performs it automatically, in which case it is set with 0x0000)
    #else
        fnInsertIP_checksum(&ucData[i], prIP_to, _NETWORK_ID, ucTxInterfaceHandling); // insert IP checksum (unless the interface performs it automatically, in which case it is set with 0x0000)
    #endif
        i += 2;
        uMemcpy(&ucData[i], &network[_NETWORK_ID].ucOurIP[0], IPV4_LENGTH); // add the source IP addresses
        i += IPV4_LENGTH;    
        uMemcpy(&ucData[i], prIP_to, IPV4_LENGTH);                       // add the destination ID address
        i += IPV4_LENGTH;
    #if (defined USE_IGMP && (defined USE_IGMP_V2 || defined USE_IGMP_V3))
        if (ucOptionLength != 0) {
            ucData[i++] = 0x94;                                          // add fixed router alert option
            ucData[i++] = 0x04;
            ucData[i++] = 0x00;                                          // every router to examine the packet
            ucData[i++] = 0x00;
        #if ((!defined IP_TX_CHECKSUM_OFFLOAD && !defined IP_TX_PAYLOAD_CHECKSUM_OFFLOAD) || defined IP_INTERFACE_WITHOUT_CS_OFFLOADING || defined _WINDOWS)
            if (ucTxInterfaceHandling != 0) {                            // if the interface doesn't perform checksum offloading
                usCheckSum = ~fnCalcIP_CS((unsigned short)~usCheckSum, &ucData[i - 4], 4); // update the checksum
                ucData[i - (2 * IPV4_LENGTH) - 6]  = (unsigned char)((usCheckSum) >> 8); // insert the checksum value
                ucData[i - (2 * IPV4_LENGTH) - 5]  = (unsigned char)(usCheckSum);
            }
        #endif
        }
    #endif
    #if defined USE_IP_STATS
        fnTxStats(ucProtType, _NETWORK_ID);                              // perform Tx statistics
    #endif
        i = (int)fnFirstWrite(_ETHERNET_HANDLE, ucData, (QUEUE_TRANSFER)i, _ALTERNATIVE_VLAN(cSocket)); // {14} prepare the IP frame for transmission
        if (i == 0) { 
    #if defined EMAC_ENHANCED && defined IP_TX_CHECKSUM_OFFLOAD && defined IP_TX_PAYLOAD_CHECKSUM_OFFLOAD && defined IPV4_SUPPORT_TX_FRAGMENTATION
            fnSetFragmentMode(0);
    #endif
            return 0;                                                    // failed
        }
    #if defined USE_IPV6
        if (dat == 0) {                                                  // this is used when tunneling v6tov4 to leave IPV6 to add its header and payload
            return (unsigned short)i;
        }
    #endif
    #if defined IPV4_SUPPORT_TX_FRAGMENTATION                            // {24}
        fnWrite(_ETHERNET_HANDLE, (dat + (usFragmentOffset * 8)), usLen);// add the pay load
        iReturn = (fnWrite(_ETHERNET_HANDLE, 0, 0));                     // transmit the ETHERNET frame
        if ((iReturn == 0) || (usRemainder == 0)) {                      // no more datagram fragments to be sent
        #if defined EMAC_ENHANCED && defined IP_TX_CHECKSUM_OFFLOAD && defined IP_TX_PAYLOAD_CHECKSUM_OFFLOAD
            fnSetFragmentMode(0);
        #endif
            break;
        }
        usFragmentOffset += (usLen/8);                                   // next fragment offset
        usLen = usRemainder;
        usIP_identification_field--;
    } while (1);                                                         // repeat sending datagram fragments
    return iReturn;
    #else
    fnWrite(_ETHERNET_HANDLE, dat, usLen);                               // add the pay load
    return (fnWrite(_ETHERNET_HANDLE, 0, 0));                            // transmit the ETHERNET frame
    #endif
}

    #if defined USE_IPV6                                                 // {4}

static const unsigned char ucIP_ProtV6[] = {(unsigned char)(PROTOCOL_IPv6 >> 8), (unsigned char)(PROTOCOL_IPv6)};

// Build and transmit an IPV6 frame with payload
//
extern signed short fnSendIPV6(unsigned char *prIP_to, unsigned char ucProtType, unsigned char ucTTL, unsigned char *dat, unsigned short usLen, UTASK_TASK Owner, USOCKET cSocket)
{
    unsigned char *ptrHW_address;
    int iExternal = 0;
    unsigned short i;
    unsigned char  ucData[(ETH_HEADER_LEN + sizeof(IP_PACKET_V6))];      // space for temporary Ethernet II and IPV6 header (no options supported)
        #ifdef USE_IP            
    if (uMemcmp(prIP_to, ucLinkLocalIPv6Address[DEFAULT_NETWORK], IPV6_SUBNET_LENGTH) != 0) { // if not in the IPv6 local subnet use the IPV4 gateway instead
        ARP_TAB *ptrARP;   
        if ((ptrARP = fnGetIP_ARP(&network[DEFAULT_NETWORK].ucDefGW[0], Owner, cSocket)) == 0) { // see whether IPV4 gateway is in ARP table
            return 0;                                                    // ARP will normally try to resolve the address here
        }
        iExternal = 1;
        ptrHW_address =  ptrARP->ucMac;                                  // set pointer to the MAC address of the gateway
    }
    else if ((ptrHW_address = fnGetIPV6_NN(prIP_to, Owner, cSocket)) == 0) {  // see whether IPV6 address is in IPV6 neighbor table
        return NO_ARP_ENTRY;                                             // ARP / network neighbor will normally try to resolve the address here
    }
        #else
    if ((ptrHW_address = fnGetIPV6_NN(prIP_to, Owner, cSocket)) == 0) {  // see whether IPV6 address is in IPV6 neighbor table
        return NO_ARP_ENTRY;                                             // ARP / network neighbor will normally try to resolve the address here
    }
        #endif    

    uMemcpy(&ucData[0], ptrHW_address, MAC_LENGTH);                      // add datalink (Ethernet addresses) information
    uMemcpy(&ucData[MAC_LENGTH], &network[DEFAULT_NETWORK].ucOurMAC[0], MAC_LENGTH);
    uMemcpy(&ucData[2 * MAC_LENGTH], ucIP_ProtV6, sizeof(ucIP_ProtV6));
    i = ((2 * MAC_LENGTH) + sizeof(ucIP_ProtV6));

    // Add IPv6 header
    //
    ucData[i++]       = 0x60;                                            // version_traffic_class
    ucData[i++]       = 0;                                               // traffic_class_flow
    ucData[i++]       = 0;                                               // flow_lable[2]
    ucData[i++]       = 0;
    ucData[i++]       = (unsigned char)(usLen >> 8);                     // payload_length[2]
    ucData[i++]       = (unsigned char)usLen;
    ucData[i++]       = ucProtType;                                      // next_header
    ucData[i++]       = ucTTL;                                           // hop_limit
    if (iExternal != 0) {                                                // {10} if not in the local network
        uMemcpy(&ucData[i], &network[DEFAULT_NETWORK].ucOurIPV6[0], IPV6_LENGTH); // add global source IPv6 addres
    }
    else {
        uMemcpy(&ucData[i], ucLinkLocalIPv6Address[DEFAULT_NETWORK], IPV6_LENGTH); // {10} add link-local source IPv6 address
    }
    i += IPV6_LENGTH;    
    uMemcpy(&ucData[i], prIP_to, IPV6_LENGTH);                           // add destination IPv6 address
    i += IPV6_LENGTH;

    if (ucProtType == IP_ICMPV6) {                                       // special case for ICMPV6 since the check sum always needs to be added
        #if !defined IP_TX_CHECKSUM_OFFLOAD || defined FORCE_PAYLOAD_ICMPV6_TX || defined IP_INTERFACE_WITHOUT_CS_OFFLOADING || defined _WINDOWS // {7}
        unsigned short usCheckSum;                                       // add the checksum by calculating over pseudo-header plus payload, where the pseudo-header consists of the next header type, the payload length, the source IPV6 and destination IPV6 addresses
        usCheckSum = fnCalcIP_CS(ucProtType, (unsigned char*)&ucData[((2 * MAC_LENGTH) + sizeof(ucIP_ProtV6)) + 4], 2);
        usCheckSum = fnCalcIP_CS(usCheckSum, (unsigned char*)&ucData[((2 * MAC_LENGTH) + sizeof(ucIP_ProtV6)) + 8], (2 * IPV6_LENGTH));
        usCheckSum = ~fnCalcIP_CS(usCheckSum, dat, usLen);
        dat[2] = (unsigned char)(usCheckSum >> 8);                       // insert new checksum
        dat[3] = (unsigned char)(usCheckSum);
        #else
        dat[2] = 0;
        dat[3] = 0;
        #endif
        #ifdef USE_IP_STATS
        fnTxStats(IP_ICMPV6, DEFAULT_NETWORK);                           // count ICMPV6 transmissions
        #endif
    }

        #if defined USE_IPV6INV4
    if ((iExternal != 0) && (uMemcmp(&network[DEFAULT_NETWORK].ucTunnelIPV4[0], cucNullMACIP, IPV4_LENGTH) != 0)) { // if the IPv6 address is external and we have an IPv6 in IPv4 tunnelling address
        i -= ((2 * MAC_LENGTH) + sizeof(ucIP_ProtV6));
        if (!fnSendIPv4(&network[DEFAULT_NETWORK].ucTunnelIPV4[0], IP_6IN4, TOS_NORMAL_SERVICE, ucTTL, 0, (QUEUE_TRANSFER)(i + usLen), Owner, cSocket)) {
            return 0;
        }
        fnWrite(Ethernet_handle[0], &ucData[((2 * MAC_LENGTH) + sizeof(ucIP_ProtV6))], (QUEUE_TRANSFER)i); // add IPV6
    }
    else {
        if (fnWrite(Ethernet_handle[0], ucData, (QUEUE_TRANSFER)i) == 0) { // prepare the Ethernet II + IPV6 frame for transmission
            return 0;                                                    // failed
        }
    }
        #else
    if (fnWrite(Ethernet_handle[0], ucData, (QUEUE_TRANSFER)i) == 0) {   // prepare the Ethernet II + IPV6 frame for transmission
        return 0;                                                        // failed
    }
        #endif
    fnWrite(Ethernet_handle[0], dat, usLen);                             // add the pay load
    return (fnWrite(Ethernet_handle[0], 0, 0));                          // transmit the ETHERNET frame
}

        #if defined USE_IPV6INV4
            #if defined USE_IPV6INV4_RELAY_DESTINATIONS && (USE_IPV6INV4_RELAY_DESTINATIONS != 0)

IPV6INV4_RELAY_DESTINATION *ptrRelayTable = 0;

extern void fnSetIPv6in4Destinations(IPV6INV4_RELAY_DESTINATION *ptrTable)
{
    ptrRelayTable = ptrTable;
}

static IPV6INV4_RELAY_DESTINATION *fnGetIPV6in4_destination(unsigned char destination_IPv6_address[IPV6_LENGTH])
{
    IPV6INV4_RELAY_DESTINATION *ptrDestinations = ptrRelayTable;
    int i;
    if (ptrRelayTable == 0) {
        return 0;
    }
    for (i = 0; i < USE_IPV6INV4_RELAY_DESTINATIONS; i++) {
        if (uMemcmp(destination_IPv6_address, ptrDestinations->ucIPV6_destination, IPV6_LENGTH) == 0) {
            return ptrDestinations;                                      // return pointer to the destination details
        }
        ptrDestinations++;
    }
    return 0;                                                            // no relay destination found
}
            #endif

// Handle a tunnelled IPv6 frame (note that this routine uses a fixed PC address for initial test purposes)
//
extern int fnHandleIPV6in4(ETHERNET_FRAME *frame, unsigned short usIPLength)
{
            #if defined USE_IPV6INV4_RELAY_DESTINATIONS && (USE_IPV6INV4_RELAY_DESTINATIONS != 0)
    IPV6INV4_RELAY_DESTINATION *ptrRelayDestination;
            #endif
    IPV6_DISCOVERY_FRAME_RX *ptrDiscoveryFrame = (IPV6_DISCOVERY_FRAME_RX *)(frame->ptEth->ucData + usIPLength);
    unsigned short usPayloadLength;
            #if !defined IP_RX_CHECKSUM_OFFLOAD || !defined IP_TX_CHECKSUM_OFFLOAD || defined IP_INTERFACE_WITHOUT_CS_OFFLOADING || defined _WINDOWS
    unsigned short usCheckSum;
            #endif
    usPayloadLength = (ptrDiscoveryFrame->ipv6.payload_length[0] << 8);
    usPayloadLength += ptrDiscoveryFrame->ipv6.payload_length[1];
    if (usPayloadLength > (frame->frame_size - usIPLength - ETH_HEADER_LEN - sizeof(IP_PACKET_V6))) { // check basic payload size validity
        return -3;                                                       // ignore frames with unrealistic payload size
    }
            #if !defined IP_RX_CHECKSUM_OFFLOAD || defined IP_INTERFACE_WITHOUT_CS_OFFLOADING || defined _WINDOWS
    usCheckSum = fnCalcIP_CS(ptrDiscoveryFrame->ipv6.next_header, (unsigned char*)&ptrDiscoveryFrame->ipv6.payload_length, 2); // check pseudo-header plus payload, where the pseudo-header consists of the next header type, the payload length, the source IPV6 and destination IPV6 addresses
    if (IP_GOOD_CS != fnCalcIP_CS(usCheckSum, (unsigned char*)&ptrDiscoveryFrame->ipv6.source_IP_address, (unsigned short)((2 * IPV6_LENGTH) + usPayloadLength))) {
        return -2;                                                        // quietly discard frame when there is a checksum error
    }
            #endif
    if (!uMemcmp(ptrDiscoveryFrame->ipv6.destination_IP_address, &network[DEFAULT_NETWORK].ucOurIPV6[0], IPV6_LENGTH) != 0) { // is it a tunnelled IPv6 frame destined to our global address?
        frame->ptEth = (ETHERNET_FRAME_CONTENT *)((unsigned char *)frame->ptEth + usIPLength); // move to the IPv6 content
        return 1;                                                        // ipv6 content to be handled
    }
            #if defined USE_IPV6INV4_RELAY_DESTINATIONS && (USE_IPV6INV4_RELAY_DESTINATIONS != 0)
    else if ((ptrRelayDestination = fnGetIPV6in4_destination(ptrDiscoveryFrame->ipv6.destination_IP_address)) != 0) { // is there a known destination to which we can relay to?
        uMemcpy(frame->ptEth->ethernet_destination_MAC, ptrRelayDestination->ucMAC_destination, MAC_LENGTH); // add MAC address on local network
        uMemcpy(frame->ptEth->ethernet_source_MAC, &network[DEFAULT_NETWORK].ucOurMAC[0], MAC_LENGTH); // our MAC address
        if (usIPLength != 0) {
            IP_PACKET *ptrIPV4 = (IP_PACKET *)(frame->ptEth->ucData);
            uMemcpy(ptrIPV4->destination_IP_address, ptrRelayDestination->ucIPV4_destination, IPV4_LENGTH); // the local IP address to deliver to
            ptrIPV4->ip_checksum[0] = ptrIPV4->ip_checksum[1] = 0;
                #if !defined IP_TX_CHECKSUM_OFFLOAD || defined IP_INTERFACE_WITHOUT_CS_OFFLOADING || defined _WINDOWS
            usCheckSum = ~fnCalcIP_CS(0, frame->ptEth->ucData, usIPLength); // modify the checksum of the content
            ptrIPV4->ip_checksum[0] = (unsigned char)(usCheckSum >> 8);
            ptrIPV4->ip_checksum[1] = (unsigned char)(usCheckSum);
                #endif
            if (fnWrite(Ethernet_handle[0], (unsigned char *)frame->ptEth, frame->frame_size) == 0) { // prepare the frame for PC's MAC address
                return -1;                                               // failed
            }
        }
        else {
            uMemcpy(frame->ptEth->ethernet_frame_type, ucIP_ProtV6, sizeof(ucIP_ProtV6));
            if (fnWrite(Ethernet_handle[0], (unsigned char *)frame->ptEth, ETH_HEADER_LEN) == 0) { // prepare the Ethernet II + IPV6 frame for transmission
                return -1;                                               // failed
            }
            fnWrite(Ethernet_handle[0], (unsigned char *)ptrDiscoveryFrame, (unsigned short)(usPayloadLength + sizeof(ptrDiscoveryFrame->ipv6))); // add the pay load
        }
        fnWrite(Ethernet_handle[0], 0, 0);                               // transmit the ETHERNET frame
        return 0;
    }
            #endif
    fnEnterIPV6Neighbor(frame->ptEth->ethernet_source_MAC, (unsigned char *)ptrDiscoveryFrame->ipv6.source_IP_address, (REFRESH_ADDRESS | MAC_LENGTH));
    return 0;
}
        #endif                                                           // endif USE_IPV6INV4
    #endif                                                               // endif USE_IPV6
#endif                                                                   // endif USE_IP

