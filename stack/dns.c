/**********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher

    ---------------------------------------------------------------------
    File:      dns.c
    Project:   Single Chip Embedded Internet
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2016
    *********************************************************************
    26.12.2009 Add random ID to ensure that no routers block repeated ID values {1}
    09.12.2013 Correction of DNS query length and allow a host name to end with '.' (thanks to Jakob M.N) {2}

*/        

#include "config.h"


#ifdef USE_DNS


#define OWN_TASK               TASK_DNS

#define DNS_RESEND_PERIOD     (DELAY_LIMIT)(2 * SEC)

#define E_DNS_RESEND           1

#define DNS_STATE_INIT         0
#define DNS_STATE_READY        1
#define DNS_STATE_BUSY         2
#define DNS_STATE_RESEND       3


#define DNS_NUM_RETRIES        5


#define DNS_ERROR_BUSY        -1
#define DNS_ERROR_LABEL       -2
#define DNS_ERROR_NAME        -3
#define DNS_ERROR_OVERFLOW    -4


static const unsigned char ucZeroQTypeClass[] = {0x00, 0x00, 0x01, 0x00, 0x01};
static const unsigned char ucTypeClass[]      = {0x00, 0x01, 0x00, 0x01};

static unsigned char ucDNS_server_temp_ip[IPV4_LENGTH];

static int  fnDNSListner(USOCKET dns_socket, unsigned char ucEvent, unsigned char *ucIP, unsigned short usPort, unsigned char *ucData, unsigned short usDataLen);
static int  fnRetry(void);
static void fnFindAnswer(unsigned char *ucData, unsigned short usDNS_nscount, unsigned short usDNS_ancount);
static void fnDNS_error(unsigned char ucError);

#define DNS_BUFFER             ((263 + 13 + 5) - sizeof(UDP_HEADER))

typedef struct stUDP_DNS_MESSAGE
{     
    UDP_HEADER     tUDP_Header;                                          // reserve header space
    unsigned char  ucUDP_Message[DNS_BUFFER];
} UDP_DNS_MESSAGE;

static unsigned char ucDNS_state = DNS_STATE_INIT;
static USOCKET DNSSocketNr = -1;                                         // UDP socket number
static unsigned char ucDNSRetries;
static void (*fnClientListener)(unsigned char ucEvent, unsigned char *ucIP);
static const CHAR *ptrDNS_host_name;
#ifdef RANDOM_NUMBER_GENERATOR                                           // {1}
    static unsigned short usRequestID;
#endif

// DNS task
//
void fnDNS(TTASKTABLE *ptrTaskTable)
{
    QUEUE_HANDLE PortIDInternal = ptrTaskTable->TaskID;                  // queue ID for task input
    unsigned char ucInputMessage[SMALL_QUEUE];                           // reserve space for receiving messages

    if (!ucDNS_state) {
        ucDNS_state = DNS_STATE_READY;
        if (((DNSSocketNr = fnGetUDP_socket(TOS_MINIMISE_DELAY, fnDNSListner, (UDP_OPT_SEND_CS | UDP_OPT_CHECK_CS))) >= 0)) {
            fnBindSocket(DNSSocketNr, DNS_UDP_PORT);
        }    
    } 
    else {
        if (fnRead(PortIDInternal, ucInputMessage, HEADER_LENGTH)) {    // check input queue
            if (ucInputMessage[MSG_SOURCE_TASK] == TASK_ARP) {
                fnRead(PortIDInternal, ucInputMessage, ucInputMessage[MSG_CONTENT_LENGTH]);  // read the contents
                if (ARP_RESOLUTION_FAILED == ucInputMessage[0]) {
                    if (fnRetry()) {                                     // DNS server could not be resolved - try a few times before informing listener  
                        fnDNS_error(DNS_ERROR_NO_ARP_RES);
                    }
                }
                else if (ARP_RESOLUTION_SUCCESS == ucInputMessage[0]) {
                    ucDNSRetries = (DNS_NUM_RETRIES+1);                  // the first request was lost due to ARP resolution
                    fnRetry();                                           // the ARP resolution was successful - now start fresh
                }
            }
            else if (ucInputMessage[MSG_SOURCE_TASK] == TIMER_EVENT) {
                if (fnRetry()) {                                         // resolution timeout (E_DNS_RESEND)
                    fnDNS_error(DNS_ERROR_TIMEOUT);
                }
            }
        }
    }
}

// Resend a DNS query
//
static int fnRetry(void)
{
    if (ucDNSRetries != 0) {
        ucDNS_state = DNS_STATE_RESEND;
        ucDNSRetries--;
        fnResolveHostName(ptrDNS_host_name, fnClientListener);           // retry
    }
    else {                                                               // last timeout - resolution failed
        ucDNS_state = DNS_STATE_READY;
        return 1;                                                        // we give up
    }
    return 0;                                                            // OK - repetition attempted
}

// Send a DNS query to resolve the address of specified host name
//
extern signed short fnResolveHostName(const CHAR *cHostNamePtr, void (*fnListener)(unsigned char , unsigned char *))
{
    unsigned int    i;
    unsigned short  usTotal;
    UDP_DNS_MESSAGE UDP_Message;
    unsigned char  *ucBuf;

    switch (ucDNS_state) {
        case DNS_STATE_READY:
            ucDNS_state = DNS_STATE_BUSY;
            ptrDNS_host_name = cHostNamePtr;                             // remember the name we want to resolve (caller must keep this valid)
            fnClientListener = fnListener;                               // save the client listener function   
            uMemcpy(ucDNS_server_temp_ip, &network[DEFAULT_NETWORK].ucDNS_server[0], IPV4_LENGTH);
            ucDNSRetries = DNS_NUM_RETRIES;
            break;

        case DNS_STATE_BUSY:                                             // we support only one resolve at a time so return with error
            return DNS_ERROR_BUSY;

        case DNS_STATE_RESEND:
            if (ptrDNS_host_name != cHostNamePtr) {
                return DNS_ERROR_BUSY;
            }
            break;

        default:
            return DNS_ERROR_BUSY;
    }
    ucBuf = UDP_Message.ucUDP_Message;
    uMemset(ucBuf, 0, DNS_BUFFER);                                       // ensure content zeroed

#ifdef RANDOM_NUMBER_GENERATOR                                           // {1}
    usRequestID = fnRandom();
    *ucBuf++ = (unsigned char)(usRequestID);                             // give out request a transaction ID for verifying the corresponding response
    *ucBuf++ = (unsigned char)(usRequestID >> 8);
#else
    *ucBuf++ = 0xaa;                                                     // now create the message
    *ucBuf++ = 0xaa;                                                     // a fixed transaction ID
#endif
    *ucBuf++ = 0x01;
    *ucBuf++ = 0x00;
    *ucBuf++ = 0x00;                                                     // single question count
    *ucBuf++ = 0x01;

    ucBuf += 6;                                                          // other entries left at zero

    usTotal = 12;                                                        // create the question section {2} initial length

    while ((*cHostNamePtr) != '\0') {                                    // wile the end of the host server name string has not been encountered
        ucBuf++;                                                         // reserve space for count
        i = 0;

        while (((*cHostNamePtr) != '.') && ((*cHostNamePtr) != '\0')) {  // whiel neither dot nor end of string
            if (++i >= (DNS_BUFFER - usTotal/* - 13*/)) {                // ensure that there is adequate space in the DNS buffer
                return (DNS_ERROR_OVERFLOW);                             // buffer overflow (not enough buffer space available for name)
            }
            *ucBuf++ = *(unsigned char *)cHostNamePtr++;                 // copy name
        }

        if ((!i) || (i > 63)){                                           // label doesn't exist or longer than 63 bytes?
            return(DNS_ERROR_LABEL);
        }

        usTotal += (i + 1);                                              // total name length

        if (usTotal >= (264 + 12)) {                                     // {2} check for maximum length
            return(DNS_ERROR_NAME);
        }

        *(ucBuf - i - 1) = (unsigned char)i;                             // store label length

        if ((*cHostNamePtr) == '.') {                                    // still not at the end, skip dot
            cHostNamePtr++;
        }
        //else {                                                         // {2}
        if ((*cHostNamePtr) == '\0') {                                   // add ZERO, QTYPE and QCLASS
            uMemcpy(ucBuf, ucZeroQTypeClass, sizeof(ucZeroQTypeClass));
            usTotal += sizeof(ucZeroQTypeClass);
            uTaskerMonoTimer(OWN_TASK, DNS_RESEND_PERIOD, E_DNS_RESEND); // monitor resolution
                                                                         // send the request
            fnSendUDP(DNSSocketNr, ucDNS_server_temp_ip, DNS_UDP_PORT, (unsigned char *)&UDP_Message.tUDP_Header, (unsigned short)(usTotal/* + 13 + sizeof(ucZeroQTypeClass)*/), OWN_TASK); // {2} (13 was originally one byte too many which could cause rejection by certain DNS servers)
        }
        //}
    }
    return 0;
}

// The local DNS listener function - this will call the client listener if needed
//
static int fnDNSListner(USOCKET dns_socket, unsigned char ucEvent, unsigned char *ucIP, unsigned short usPort, unsigned char *ucData, unsigned short usDataLen)
{
    unsigned short  usDNS_nscount, usDNS_ancount;
    const CHAR     *cTmpPtr;
    unsigned char   ucLabLen;

    if ((DNSSocketNr != dns_socket) || (ucDNS_state == DNS_STATE_READY) || (uMemcmp(ucIP, ucDNS_server_temp_ip, IPV4_LENGTH))) {
        return (-1);                                                     // not our socket, or not in waiting state or bad IP
    }

    if (ucEvent == UDP_EVENT_RXDATA) {
        if (usDataLen < (16 - UDP_HLEN)) {
            return -1;                                                   // too short for DNS reply
        }
#ifdef RANDOM_NUMBER_GENERATOR                                           // {1}
        if ((*ucData++ != (unsigned char)(usRequestID)) || (*ucData++ != (unsigned char)(usRequestID >> 8))) {
            return -1;                                                   // wrong ID
        }
#else
        if ((*ucData++ != 0xaa) || (*ucData++ != 0xaa)) {
            return -1;                                                   // wrong ID
        }
#endif
        if (!(*ucData & 0x80)) {
            return -1;                                                   // not an answer - we are not DNS server...
        }
        if ((*ucData++ & 0x7a)) {
            fnDNS_error(DNS_ERROR_GENERAL);
            return -1;                                                   // op code not zero or message truncated
        }
        if ((*ucData & 0x7f) && (*ucData & 0x80)) {
            fnDNS_error(DNS_OPCODE_ERROR);
            return -1;                                                   // op code not zero with recursion available
        }
        ucData++;
        if ((*ucData++) || (*ucData++ != 1)) {
            fnDNS_error(DNS_ERROR_GENERAL);
            return -1;                                                   // more that one answer to a single question...
        }

        usDNS_ancount = *ucData++;
        usDNS_ancount <<= 8;
        usDNS_ancount |= *ucData++;

        usDNS_nscount = *ucData++;
        usDNS_nscount <<= 8;
        usDNS_nscount |= *ucData++;

        ucData += 2;                                                     // skip additional RRs

        cTmpPtr = ptrDNS_host_name;
        while ((ucLabLen = *ucData++) != 0) {                            // label length
            if (uMemcmp(ucData, (const unsigned char *)cTmpPtr, ucLabLen)) {
                return -1;                                               // compare with the searched host name
            }
            ucData += ucLabLen;
            cTmpPtr += ucLabLen;

            if (*cTmpPtr == '\0') {              
                continue;
            }

            if (*cTmpPtr++ != '.') {
                return (-1);                                             // DOT should be here...
            }
        }

        if (uMemcmp(ucZeroQTypeClass, (ucData - 1), sizeof(ucZeroQTypeClass))) {
            fnDNS_error(DNS_ERROR_GENERAL);
            return -1;                                                   // bad type / class
        }

        ucData += (sizeof(ucZeroQTypeClass) - 1);

        fnFindAnswer(ucData, usDNS_nscount, usDNS_ancount);              // attempt to find the answer we are looking for
    }
    return 0;
}

static void fnFindAnswer(unsigned char *ucData, unsigned short usDNS_nscount, unsigned short usDNS_ancount)
{
    unsigned short usLen;

    while (usDNS_nscount || usDNS_ancount) {
        while ((*ucData) && ((*ucData & 0xc0) != 0xc0)) {
            ucData++;
        }
        if (*ucData) {
            ucData++;                                                    // jump second offset byte
        }
        ucData++;

        if (!(uMemcmp(ucTypeClass, ucData, sizeof(ucTypeClass)))) {      // check type and class
                                                                         // we have found it..CLASS==INET and TYPE=A
            ucData += sizeof(ucTypeClass) + 4;                           // skip TTL

            usLen = *ucData++;                                           // read RDLENGTH
            usLen <<= 8;
            usLen |= *ucData++;

            if (usLen == IPV4_LENGTH) {
                uMemcpy(ucDNS_server_temp_ip, ucData, IPV4_LENGTH);      // copy the IP address
                if (usDNS_ancount) {
                    uTaskerStopTimer(OWN_TASK);                          // stop monitoring since successful
                    fnClientListener(DNS_EVENT_SUCCESS, ucDNS_server_temp_ip);
                    ucDNS_state = DNS_STATE_READY;
                }
                else {
                    ucDNSRetries = (DNS_NUM_RETRIES+1);                  // invoke another query to the authority IP address just received
                    fnRetry();
                }
                return;
            }
            else {
              ucData += usLen;                                           // skip data
            }
        }
        else {
            ucData += (sizeof(ucTypeClass) + 4);                         // skip TTL

            usLen = *ucData++;                                           // read RDLENGTH
            usLen <<= 8;
            usLen |= *ucData++;

            ucData += usLen;                                             // skip data
        }

        if (usDNS_ancount) {                                             // decrement counters
            usDNS_ancount--;
        }
        else {
            if (usDNS_nscount) {
                usDNS_nscount--;
            }
        }
    }
}

static void fnDNS_error(unsigned char ucError)
{
    fnClientListener(ucError, 0);
}
#endif
