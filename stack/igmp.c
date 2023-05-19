/***********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher

    ---------------------------------------------------------------------
    File:        igmp.c
    Project:     Single Chip Embedded Internet
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2016
    *********************************************************************
    04.02.2016 Correct interface mask when there are multiple interfaces available {1}

*/        

/* =================================================================== */
/*                           include files                             */
/* =================================================================== */

#include "config.h"


#if defined USE_IGMP

/* =================================================================== */
/*                          local definitions                          */
/* =================================================================== */

#define OWN_TASK  TASK_IGMP

#if ((IGMP_MAX_HOSTS > 1) || (defined USE_IGMP_V2 || defined USE_IGMP_V3)) && !defined GLOBAL_TIMER_TASK
    #error IGMP module requires GLOBAL_TIMER_TASK to be defined (in config.h) so that it can use multiple SW timers!!!
#endif

#define IGMP_REPORT    0
#define IGMP_LEAVE     1


/* =================================================================== */
/*                 local function prototype declarations               */
/* =================================================================== */

static void fnSendIGMP_report(IGMP_HOST *ptrIGMP_host, int iType, USOCKET host_details);


/* =================================================================== */
/*                             constants                               */
/* =================================================================== */

#if defined USE_IGMP_V2 || defined USE_IGMP_V3
    static const unsigned char ucAll_routers[IPV4_LENGTH] = {224, 0, 0, 2};
#endif


/* =================================================================== */
/*                      local variable definitions                     */
/* =================================================================== */

static IGMP_HOST igmp_hosts[IGMP_MAX_HOSTS + 1] = {{{0}}};               // list of hosts

#if (defined USE_IGMP_V2 || defined USE_IGMP_V3)
    static int iRouterVersionV1[IP_INTERFACE_COUNT] = {0};               // there is initially no IGMP v1 router on this interface
#endif


// IGMP task
//
extern void fnIgmp(TTASKTABLE *ptrTaskTable)
{
    QUEUE_HANDLE PortIDInternal = ptrTaskTable->TaskID;                  // queue ID for task input
    IGMP_HOST *ptrIGMP_host;
    unsigned char ucInputMessage[SMALL_MESSAGE];                         // reserve space for receiving messages

    while (fnRead(PortIDInternal, ucInputMessage, HEADER_LENGTH)) {      // check input queue
        switch (ucInputMessage[MSG_SOURCE_TASK]) {
            case TIMER_EVENT:
    #if (defined USE_IGMP_V2 || defined USE_IGMP_V3)
                if (ucInputMessage[MSG_TIMER_EVENT] >= (255 - (IP_INTERFACE_COUNT - 1))) {
                    iRouterVersionV1[255 - ucInputMessage[MSG_TIMER_EVENT]] = 0; // there is no more IGMP v1 router considered on this interface
                    continue;
                }
    #endif
                ptrIGMP_host = igmp_hosts;
                ptrIGMP_host += ucInputMessage[MSG_TIMER_EVENT];
                ptrIGMP_host->report_delay = 0;
    #if IP_INTERFACE_COUNT > 1
                fnSendIGMP_report(ptrIGMP_host, IGMP_REPORT, ptrIGMP_host->delayed_host_details); // repeat a report for the host group
                ptrIGMP_host->delayed_host_details = 0;
    #else
                fnSendIGMP_report(ptrIGMP_host, IGMP_REPORT, ptrIGMP_host->igmp_host_details); // repeat a report for the host group
    #endif
                ptrIGMP_host->ucReporter = 1;                            // mark that this host group was the last in the physical network to report this group membership
                break;
        }
    }
}


// Send a report to all interfaces that the group belongs to (TTL is always set to 1)
//
static void fnSendIGMP_report(IGMP_HOST *ptrIGMP_host, int iType, USOCKET host_details)
{
    IGMP_V1_V2_FRAME igmp_frame;
    unsigned char *ptrDestinationAddress;
    #if defined USE_IGMP_V2 || defined USE_IGMP_V3
    int iInterfaceRef = 0;
    #endif
    #if !defined IGMP_TX_CHECKSUM_OFFLOAD || defined _WINDOWS
    unsigned short usCheckSum;
    #endif
    #if IP_INTERFACE_COUNT > 1
    int iInterfaces = fnGetInterfaceCount(host_details);                 // get the number of interfaces that the group can exist on
    if (iInterfaces > 1) {                                               // if more that one interface is defined for transmission                                    
        USOCKET this_interface = (USOCKET)(host_details & ~(INTERFACE_MASK << INTERFACE_SHIFT));
        USOCKET interfaces = (0x1 << INTERFACE_SHIFT);                   // first possible interface
        while ((interfaces & (INTERFACE_MASK << INTERFACE_SHIFT)) != 0) {
            if (host_details & interfaces) {                             // report is to be sent on this interface
                fnSendIGMP_report(ptrIGMP_host, iType, (USOCKET)(this_interface | interfaces)); // send the report to each of the interfaces
            }
            interfaces <<= 1;                                            // repeat for all possible interfaces
        }
        return;
    }
        #if defined USE_IGMP_V2 || defined USE_IGMP_V3
    else {
        iInterfaceRef = fnGetInterfaceReference(host_details);           // get an index to the single interface
    }
        #endif
    #endif                                                               // we handle a single interface from this point
    #if defined USE_IGMP_V2 || defined USE_IGMP_V3
    if (IGMP_LEAVE == iType) {
        if (iRouterVersionV1[iInterfaceRef] != 0) {                    
            return;                                                      // don't sent leaves when there is an IGMP v1 router on the interface
        }
        igmp_frame.ucIGMPType = IGMP_V2_LEAVE_GROUP;
        ptrDestinationAddress = (unsigned char *)ucAll_routers;
    }
    else {
    #endif
    #if defined USE_IGMP_V2 || defined USE_IGMP_V3
        if (iRouterVersionV1[iInterfaceRef] != 0) {
            igmp_frame.ucIGMPType = IGMP_V1_REPORT;                      // IGMP v1 report used when there is an IGMP v1 router on the interface
        }
        else {
            igmp_frame.ucIGMPType = IGMP_V2_MEMBERSHIP_REPORT;           // use IGMP v2 membership report
        }
    #else
        igmp_frame.ucIGMPType = IGMP_V1_REPORT;                          // IGMP v1 report only used in IGMP v1 only mode
    #endif
        ptrDestinationAddress = ptrIGMP_host->ucMulticast_group_address;
    #if defined USE_IGMP_V2 || defined USE_IGMP_V3
    }
    #endif
    igmp_frame.ucIGMPResponseTime = 0;
    igmp_frame.ucIGMPCheckSum[0] = 0;                                    // set the checksum to zero
    igmp_frame.ucIGMPCheckSum[1] = 0;
    uMemcpy(igmp_frame.ucIGMPMulticastAddress, ptrIGMP_host->ucMulticast_group_address, IPV4_LENGTH);
    #if !defined IGMP_TX_CHECKSUM_OFFLOAD || defined _WINDOWS            // presently there are no supported interfaces that support IGMP offloading so always add a checksum here
    usCheckSum = ~fnCalcIP_CS(0, (unsigned char *)&igmp_frame, sizeof(igmp_frame)); // calculate the new checksum
    igmp_frame.ucIGMPCheckSum[0] = (unsigned char)(usCheckSum >> 8);
    igmp_frame.ucIGMPCheckSum[1] = (unsigned char)(usCheckSum);
    #endif
    fnSendIPv4(ptrDestinationAddress, IP_IGMPV2, TOS_NORMAL_SERVICE, 1, (unsigned char *)&igmp_frame, sizeof(igmp_frame), 0, host_details); // send out on all specified interfaces
}

// Send a report to all interfaces that the group belongs to after a random delay from 0 to max delay (in 1/10 seconds)
//
static void fnSendIGMP_report_delayed(int iHostGroupID, unsigned char ucMaxDelay, USOCKET delayed_host_details)
{
    IGMP_HOST *ptrIGMP_host = igmp_hosts;
    #if !defined USE_IGMP_V2 && !defined USE_IGMP_V3
    ucMaxDelay = 100;                                                    // IGMP v1 uses a fixed maximum delay of 10s
    #else
    if (ucMaxDelay == 0) {                                               // use the maximum delay specified in the query
        ucMaxDelay = 100;                                                // 10s when no maximum delay specified
    }
    #endif
    ptrIGMP_host += (unsigned char)iHostGroupID;
    #if IP_INTERFACE_COUNT > 1
    ptrIGMP_host->delayed_host_details |= delayed_host_details;          // accumulate interfaces to send delayed report on
    #endif
    if (ptrIGMP_host->report_delay != 0) {                               // if a report is already queued we generally leave it to timeout and be sent
    #if defined USE_IGMP_V2 && !defined USE_IGMP_V3
        if (ptrIGMP_host->report_delay <= (ucMaxDelay * SEC)) {          // if the existing delay is greater than the maximum specified delay
            return;
        }
    #else
        return;
    #endif
    }
    ptrIGMP_host->report_delay = ((fnRandom() * ucMaxDelay * SEC)/(0x10000 * 10)); // random time in seconds between 0 and maximum delay
    if (ptrIGMP_host->report_delay == 0) {                               // don't allow zero delay
        ptrIGMP_host->report_delay = 1;                                  // single tick as minimum
    }
    #if ((IGMP_MAX_HOSTS > 1) || (defined USE_IGMP_V2 || defined USE_IGMP_V3))
    uTaskerGlobalMonoTimer(OWN_TASK, ptrIGMP_host->report_delay, (unsigned char)iHostGroupID); // start the task's software timer
    #else
    uTaskerMonoTimer(OWN_TASK, ptrIGMP_host->report_delay, (unsigned char)iHostGroupID); // start the task's software timer
    #endif
}

static void fnMulticastRxFilter(QUEUE_TRANSFER Action, IGMP_HOST *ptrIGMP_host)
{
    #if IP_INTERFACE_COUNT > 1
    USOCKET host_details = ptrIGMP_host->igmp_host_details;
    int iInterfaces = fnGetInterfaceCount(host_details);                 // the number of interfaces that the group is using
    int iInterfaceRef;
    while (iInterfaces--) {
        iInterfaceRef = fnGetInterfaceReference(host_details);           // get an index to the first interface
        fnWrite(fnGetInterfaceHandle(iInterfaceRef), ptrIGMP_host->ucMulticast_group_address, Action); // set or remove multicast reception filter
        host_details &= ~(defineInterface(iInterfaceRef));               // this one has been handled
    }
    #else
    fnWrite(Ethernet_handle[0], ptrIGMP_host->ucMulticast_group_address, Action); // single interface defaults to ethernet
    #endif
}


// A host process wants to join a multcast group
//
extern int fnJoinMulticastGroup(unsigned char ucMulticastGroupAddress[IPV4_LENGTH], USOCKET host_details, void (*process_call_back)(int iHostID, unsigned short usSourcePort, unsigned short usRemotePort, unsigned char *ptrBuf, unsigned short usDataLen))
{
    IGMP_HOST *ptrIGMP_host = igmp_hosts;
    int iHostID;
    int iFreeHostID = -1;
    if ((ucMulticastGroupAddress[0] < 224) || (ucMulticastGroupAddress[0] > 239)) {
        return ERROR_IGMP_INVALID_MULT_ADDRESS;                          // invalid multicast address
    }
    for (iHostID = 0; iHostID <= IGMP_MAX_HOSTS; iHostID++) {
        if (ptrIGMP_host->ucProcessCount == 0) {                         // free
            if (iHostID == 0) {                                          // all-hosts entry has not yet been configured
                ptrIGMP_host->ucMulticast_group_address[0] = 224;
              //ptrIGMP_host->ucMulticast_group_address[1] = 0;
              //ptrIGMP_host->ucMulticast_group_address[2] = 0;
                ptrIGMP_host->ucMulticast_group_address[3] = 1;
                ptrIGMP_host->ucProcessCount = 1;
                #if IP_INTERFACE_COUNT > 1
                ptrIGMP_host->igmp_host_details = IGMP_ALL_HOSTS_INTERFACES;
                #else
              //ptrIGMP_host->igmp_host_details = 0;
                #endif
                fnMulticastRxFilter(ADD_IPV4_FILTER, ptrIGMP_host);      // enable a multicast reception filter for this all-hosts group
            }
            else {
                iFreeHostID = iHostID;                                   // mark the last free host group entry found until now
            }
        }
        else if (ptrIGMP_host->igmp_host_details == host_details) {      // existing host group found using the same versions and the same network/interfaces
            if (uMemcmp(ucMulticastGroupAddress, ptrIGMP_host->ucMulticast_group_address, IPV4_LENGTH) == 0) { // see whether the multicast address matches
                if (ptrIGMP_host->ucProcessCount >= IGMP_MAX_PROCESSES) {
                    return ERROR_IGMP_HOST_PROCESSES_EXHAUSED;           // no more process space available
                }
                ptrIGMP_host->process_call_back[ptrIGMP_host->ucProcessCount] = process_call_back; // enter process call-back
                return (iHostID | (ptrIGMP_host->ucProcessCount++ << 8)); // found an existing host group that the process can belong to
            }
        }
        ptrIGMP_host++;                                                  // move to next
    }
    if (iHostID > IGMP_MAX_HOSTS) {                                      // no matching host group exists
        if (iFreeHostID < 0) {                                           // if no free host entry found
            return ERROR_IGMP_HOSTS_EXHAUSED;                            // no more host space available
        }
        else {
            ptrIGMP_host = igmp_hosts;                                   // use the last free host space that was found
            iHostID = iFreeHostID;
            ptrIGMP_host += iHostID;                                     // the new entry to be used
            if (uMemcmp(&network[extractNetwork(host_details)].ucOurIP[0], cucNullMACIP, IPV4_LENGTH) == 0) { // if the network is not available
                return ERROR_IGMP_NETWORK_NOT_AVAILABLE;
            }
            uMemcpy(ptrIGMP_host->ucMulticast_group_address, ucMulticastGroupAddress, IPV4_LENGTH); // enter its multicast address
            ptrIGMP_host->igmp_host_details = host_details;              // enter the network and interface details
            ptrIGMP_host->process_call_back[0] = process_call_back;      // enter process call-back at first process location
            ptrIGMP_host->ucProcessCount = 1;                            // the first process
            fnMulticastRxFilter(ADD_IPV4_FILTER, ptrIGMP_host);          // enable a multicast reception filter for this group
            fnSendIGMP_report(ptrIGMP_host, IGMP_REPORT, host_details);  // report that the host is joining the group on all of its interfaces (only when the first process joins the group)
            fnSendIGMP_report_delayed(iHostID, 0, host_details);         // the IGMT report is sent again after a random delay between 0 and 10s to increase the probability that it will be received by a multicast router
        }
    }
    return iHostID;                                                      // return the ID of group that the process now belongs to, as well as its process entry refernce
}

static void fnRemoveAllHosts(void)
{
    int iHostID;
    IGMP_HOST *ptrIGMP_host = igmp_hosts;
    for (iHostID = 1; iHostID <= IGMP_MAX_HOSTS; iHostID++) {            // check for hosts that are members of the groups on this interface
        ptrIGMP_host++;
        if (ptrIGMP_host->ucProcessCount != 0) {                         // for each host group with at least one process
            return;                                                      // host group found so quit
        }
    }
    igmp_hosts[0].ucProcessCount = 0;                                    // free the all-hosts group
    fnMulticastRxFilter(REMOVE_IPV4_FILTER, &igmp_hosts[0]);             // disable a multicast reception filter for this all-hosts group
}


//A host process wants to leave a multicast Igroup
//
extern int fnLeaveMulticastGroup(int iHostID)
{
    IGMP_HOST *ptrIGMP_host = igmp_hosts;
    int iHostID_check = (iHostID & 0xff);                                // extract the host reference
    int iProcessReference = (iHostID >> 8);                              // extract the process reference
    ptrIGMP_host += iHostID_check ;
    if ((iHostID_check == 0) || (iHostID_check > IGMP_MAX_HOSTS)) {
        return ERROR_IGMP_ILLEGAL_HOST_ID;
    }
    if (ptrIGMP_host->ucProcessCount == 0) {
        return ERROR_IGMP_NON_EXISTANT_HOST_ID;
    }
    ptrIGMP_host->process_call_back[iProcessReference] = 0;              // remove the process call-back
    if (--(ptrIGMP_host->ucProcessCount) == 0) {                         // last process in group leaves
        ptrIGMP_host->report_delay = 0;
        fnMulticastRxFilter(REMOVE_IPV4_FILTER, ptrIGMP_host);           // disable the multicast reception filter for this group
    #if ((IGMP_MAX_HOSTS > 1) || (defined USE_IGMP_V2 || defined USE_IGMP_V3))
        uTaskerGlobalStopTimer(OWN_TASK, (unsigned char)iProcessReference); // kill the host timer, in case it is active
    #else
        uTaskerStopTimer(OWN_TASK);
    #endif
    #if defined USE_IGMP_V2 || defined USE_IGMP_V3
        fnSendIGMP_report(ptrIGMP_host, IGMP_LEAVE, ptrIGMP_host->igmp_host_details); // report leaving on each interface
    #endif
        fnRemoveAllHosts();                                              // check whether there are further groups - if no remove the all-hosts entry too
        return IGMP_GROUP_FREED;                                         // last process in a host group freed so that host group no longer has membership
    }
    else {
        return IGMP_PROCESS_REMOVED;                                     // process successfully removed (the host is still a member since there are still active processes)
    }
}


// Handle an IGMP datagram reception (received on the all-hosts address 224.0.0.1 or a multicast group address)
//
extern void fnHandleIGMP(ETHERNET_FRAME *ptrRx_frame)
{
    int iHostID;
    int iGeneralQuery = 0;
    IGMP_HOST *ptrIGMP_host = igmp_hosts;
    IGMP_V1_V2_FRAME *ptrIGMP;
    IP_PACKET *received_ip_packet = (IP_PACKET *)&ptrRx_frame->ptEth->ucData;
    unsigned short usLength = ((received_ip_packet->total_length[0] << 8) | received_ip_packet->total_length[1]);
    ptrIGMP = (IGMP_V1_V2_FRAME *)(received_ip_packet->ip_options + (ptrRx_frame->usIPLength - IP_MIN_HLEN));
    usLength -= ptrRx_frame->usIPLength;
    switch (ptrIGMP->ucIGMPType) {
    case IGMP_V1_QUERY:
  //case IGMP_V2_MEMBERSHIP_QUERY:                                       // same type value
    #if !defined IGMP_RX_CHECKSUM_OFFLOAD || defined _WINDOWS
        if (IP_GOOD_CS != fnCalcIP_CS(0, (unsigned char *)ptrIGMP, usLength)) {
            return;                                                      // discard if checksum error
        }
    #endif
        if (uMemcmp(received_ip_packet->destination_IP_address, ptrIGMP_host->ucMulticast_group_address, sizeof(ptrIGMP->ucIGMPMulticastAddress)) == 0) { // all-hosts group address
            if (uMemcmp(ptrIGMP->ucIGMPMulticastAddress, cucNullMACIP, sizeof(ptrIGMP->ucIGMPMulticastAddress)) == 0) {
                iGeneralQuery = 1;                                       // general query being received
        #if (defined USE_IGMP_V2 || defined USE_IGMP_V3)
                if (ptrIGMP->ucIGMPResponseTime == 0) {                  // IGMP v1 router detected
            #if IP_INTERFACE_COUNT > 1                    
                    iRouterVersionV1[ptrRx_frame->ucInterface] = 1;      // this interface presently has an IGMP v1 router detected on it
                    uTaskerGlobalMonoTimer(OWN_TASK, (400 * SEC), (unsigned char)(255 - ptrRx_frame->ucInterface));// start the task's software timer
            #else
                    iRouterVersionV1[0] = 1;                             // this interface presently has an IGMP v1 router detected on it
                    uTaskerGlobalMonoTimer(OWN_TASK, (400 * SEC), (unsigned char)(255 - 0));// start the task's software timer
            #endif
                }
        #endif
            }
        }
    #if !defined USE_IGMP_V2 && !defined USE_IGMP_V3
        if (iGeneralQuery == 0) {
            return;                                                      // IGMP v1 only accepts general queries
        }
    #endif
        for (iHostID = 1; iHostID <= IGMP_MAX_HOSTS; iHostID++) {        // check for hosts that are members of groups on this interface
            ptrIGMP_host++;
    #if IP_INTERFACE_COUNT > 1
            if ((ptrIGMP_host->igmp_host_details & defineInterface(ptrRx_frame->ucInterface)) == 0) { // if the interface doesn't belong to the group
                continue;
            }
    #endif
    #if defined USE_IGMP_V2 || defined USE_IGMP_V3
            if (iGeneralQuery == 0) {                                    // group query
                if (uMemcmp(ptrIGMP->ucIGMPMulticastAddress, ptrIGMP_host->ucMulticast_group_address, sizeof(ptrIGMP->ucIGMPMulticastAddress)) != 0) {
                    continue;                                            // don't respond for this host group
                }
            }
    #endif
            if (ptrIGMP_host->ucProcessCount != 0) {                     // if there are active processes in the group it sends a report after a random delay
    #if IP_INTERFACE_COUNT > 1
                fnSendIGMP_report_delayed(iHostID, ptrIGMP->ucIGMPResponseTime, (USOCKET)((ptrIGMP_host->igmp_host_details & ~(INTERFACE_MASK << INTERFACE_SHIFT)) | defineInterface(ptrRx_frame->ucInterface))); // report after random delay on this interface
    #else
                fnSendIGMP_report_delayed(iHostID, ptrIGMP->ucIGMPResponseTime, ptrIGMP_host->igmp_host_details); // report after random delay
    #endif
            }
        }
        break;

    case IGMP_V1_REPORT:
    case IGMP_V2_MEMBERSHIP_REPORT:
    #if !defined IGMP_RX_CHECKSUM_OFFLOAD || defined _WINDOWS
        if (IP_GOOD_CS != fnCalcIP_CS(0, (unsigned char *)ptrIGMP, usLength)) {
            return;                                                      // discard if checksum error
        }
    #endif
        for (iHostID = 1; iHostID <= IGMP_MAX_HOSTS; iHostID++) {        // check for hosts that are members of the groups on this interface
            ptrIGMP_host++;
    #if IP_INTERFACE_COUNT > 1
            if ((ptrIGMP_host->igmp_host_details & defineInterface(ptrRx_frame->ucInterface)) == 0) { // if the interface doesn't belong to the group
                continue;
            }
    #endif
            if (uMemcmp(ptrIGMP->ucIGMPMulticastAddress, ptrIGMP_host->ucMulticast_group_address, sizeof(ptrIGMP->ucIGMPMulticastAddress)) == 0) { // if this is a report on this interface matching out group we can cancel any queued reports from this host group
                if (ptrIGMP_host->report_delay != 0) {                   // if a delayed report is pending
    #if (defined USE_IGMP_V2 || defined USE_IGMP_V3)
        #if IP_INTERFACE_COUNT > 1
                    ptrIGMP_host->ucReporter &= ~(defineInterface(ptrRx_frame->ucInterface)); // since another host group has reported membership we are no longer reporter on this interface
                    if (ptrIGMP_host->ucReporter != 0) {
                        break;                                           // don't stop the timer since it is still required for other interfaces
                    }
        #else
                    ptrIGMP_host->ucReporter = 0;                        // no longer reported
        #endif
    #endif
    #if ((IGMP_MAX_HOSTS > 1) || (defined USE_IGMP_V2 || defined USE_IGMP_V3))
                    uTaskerGlobalStopTimer(OWN_TASK, (unsigned char)iHostID); // cancel the timer
    #else
                    uTaskerStopTimer(OWN_TASK);
    #endif
                    ptrIGMP_host->report_delay = 0;                      // cancel queued report
                }
            }
        }
        break;
    case IGMP_V2_LEAVE_GROUP:
        break;
    case IGMP_V3_MEMBERSHIP_QUERY:
        break;
    default:
        return;                                                          // silently ignore unknown types
    }
}

// A multicast UDP packet has been received on a particular interface - if there is a member host group it is passed to each of the processes belonging to it
//
extern void fnHandleMulticastRx(USOCKET SocketHandle, int iHostID_Sending, unsigned char *dest_IP, unsigned short usSourcePort, unsigned short usRemotePort, unsigned char *ptrBuf, unsigned short usDataLen)
{
    int iHostID;
    IGMP_HOST *ptrIGMP_host = igmp_hosts;
    int iProcess;

    for (iHostID = 0; iHostID <= IGMP_MAX_HOSTS; iHostID++) {            // check for hosts that are members of the groups on this interface
        if (ptrIGMP_host->ucProcessCount != 0) {
#if IP_INTERFACE_COUNT > 1
            if ((ptrIGMP_host->igmp_host_details & (SocketHandle & (INTERFACE_MASK << INTERFACE_SHIFT))) == 0) { // {1} if the interface doesn't match
                ptrIGMP_host++;
                continue;
            }
#endif
            if (uMemcmp(ptrIGMP_host->ucMulticast_group_address, dest_IP, sizeof(ptrIGMP_host->ucMulticast_group_address)) == 0) { // if this is a report on this interface matching out group we can cancel any queued repprts from this host group
                for (iProcess = 0; iProcess < IGMP_MAX_PROCESSES; iProcess++) { // for all possible processes
                    if (ptrIGMP_host->process_call_back[iProcess] != 0) {
                        int iHostReference = (iHostID | (iProcess << 8));
                        if (iHostReference != iHostID_Sending) {         // don't loop back to the sending process
                            ptrIGMP_host->process_call_back[iProcess](iHostReference, usSourcePort, usRemotePort, ptrBuf, usDataLen); // pass the UDP data to the process
                        }
                    }
                }
            }
        }
        ptrIGMP_host++;
    }
}

#if defined USE_MAINTENANCE
extern void fnReportIGMP(void)
{
    int iHostID;
    int iFound = 0;
    int iProcesses;
    IGMP_HOST *ptrIGMP_host = igmp_hosts;
    for (iHostID = 1; iHostID <= IGMP_MAX_HOSTS; iHostID++) {            // check for hosts that are members of the groups on this interface
        ptrIGMP_host++;
        if (ptrIGMP_host->ucProcessCount != 0) {                         // for each host group with at least one process
            iFound = 1;
            fnDebugMsg("Host group with ");
            fnDebugDec(ptrIGMP_host->ucProcessCount, 0);
            fnDebugMsg(" Process(es):\r\n");
            iProcesses = ptrIGMP_host->ucProcessCount;
            for (iProcesses = 0; iProcesses < IGMP_MAX_PROCESSES; iProcesses++) {
                if (ptrIGMP_host->process_call_back[iProcesses] != 0) {
                    fnDebugHex((unsigned short)((iProcesses << 8) | iHostID), (WITH_SPACE | WITH_LEADIN | WITH_CR_LF | sizeof(unsigned short)));
                }
            }
            fnDebugMsg("on multicast address ");
            fnDisplayIP(&ptrIGMP_host->ucMulticast_group_address[0]);
            fnDebugMsg("\r\n");
        }
    }
    if (iFound == 0) {
        fnDebugMsg("No host groups exist\r\n");
    }
}
#endif
#endif
