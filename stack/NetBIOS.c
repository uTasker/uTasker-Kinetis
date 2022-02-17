/**********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher

    ---------------------------------------------------------------------
    File:      NetBIOS.c
    Project:   Single Chip Embedded Internet
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2016
    *********************************************************************
    21.12.2012 Change fnStartNetBIOS_Server() return value to USOCKET    {1}
    07.04.2013 Don't reply when the IP address is presently 0.0.0.0 (eg. before DHCP has completed) {2}

*/

#include "config.h"

#ifdef USE_NETBIOS

// We support NetBIOS over UDP since this is the usual method.
// NetBIOS is described in rfc1001 (concepts and methods) and rfc1002 (detailed specifications).
// The support is useful to allows the device to be identified by a name rather than its IP address.
//
// This implementation is a very simple one which only supports single queries (typically used to find the named device in a local network).
// Any other queries will be ignored but it should never be able to otherwise fail.
// Please record any occurances of queries which fail so that these can be added to the handling and report the case on the
// uTasker forum at http://www.uTasker.com/forum/

// When the IP settings are changed the PCs in the network that remember the original NetBIOS match between the name and the IP address for a certain
// amount of time, meaning that the operation is no longer successful after the change until the NetBIOS cache times out.
// This cache can be reset on Windows PCs with the command "nbtstat /R" so that the operation is immediately possibel without needing to wait this time.

/* =================================================================== */
/*                          local definitions                          */
/* =================================================================== */

#define NetBIOS_PORT                   137

#define NetBIOS_NAME_MAX               16

#define NetBIOS_RECURSIVE_DESIRED_FLAG 0x01
#define NetBIOS_AUTHORITATIVE_FLAG     0x04
#define NetBIOS_OPCODE_MASK            0x78
#define NetBIOS_RESPONSE_FLAG          0x80

#define NetBIOS_NAME_QUERY             0x00


/* =================================================================== */
/*                      local structure definitions                    */
/* =================================================================== */
__PACK_ON                                                                // compilers using pragmas to control packing will start struct packing from here
typedef struct _PACK stNetBIOS_QUERY
{
    unsigned char  ucLabelLength;                                        // this must be 0x20 in case of a NetBIOS name
    unsigned char  NetBIOS_name[(NetBIOS_NAME_MAX * 2) + 1];             // fixed length for NetBIOS name
    unsigned char  type[2];
    unsigned char  Class[2];
} NetBIOS_QUERY;

typedef struct _PACK stNetBIOS
{
    unsigned char  transaction_ID[2];
    unsigned char  flags[2];
    unsigned char  questions[2];
    unsigned char  answer_RRs[2];
    unsigned char  authority_RRs[2];
    unsigned char  additional_RRs[2];
    NetBIOS_QUERY  query;
} NetBIOS;

typedef struct _PACK stNetBIOS_ANSWER
{
    NetBIOS        query;
    unsigned char  ttl[4];                                               // time-to-live in seconds
    unsigned char  length[2];                                            // length of flags and data
    unsigned char  flags[2];
    unsigned char  ip_address[IPV4_LENGTH];
} NetBIOS_ANSWER;
__PACK_OFF

/* =================================================================== */
/*                 local function prototype declarations               */
/* =================================================================== */

static int fnNetBIOS_Listener(USOCKET NetBIOS_SocketNr, unsigned char ucEvent, unsigned char *ucIP, unsigned short usPort, unsigned char *ucData, unsigned short usDataLen);

/* =================================================================== */
/*                             constants                               */
/* =================================================================== */

static const unsigned char ucTTL[] = {0x00, 0x01, 0x51, 0x80};           // time to live fixed at 24 hours (in seconds)


/* =================================================================== */
/*                      local variable definitions                     */
/* =================================================================== */

static USOCKET NetBIOS_SocketNr = -1;
static CHAR *NetBIOS_name = 0;



// Call this to set a UDP lister on the NetBIOS port and enter the device name
//
extern USOCKET fnStartNetBIOS_Server(CHAR *name)                         // {1}
{
    if ((NetBIOS_SocketNr = fnGetUDP_socket(TOS_MINIMISE_DELAY, fnNetBIOS_Listener, (UDP_OPT_SEND_CS | UDP_OPT_CHECK_CS))) >= 0) {
        NetBIOS_name = name;
        fnBindSocket(NetBIOS_SocketNr, NetBIOS_PORT);
    }
    return NetBIOS_SocketNr;                                             // {1}
}



// NetBIOS call back function on UDP reception
//
static int fnNetBIOS_Listener(USOCKET NetBIOS_SocketNr, unsigned char ucEvent, unsigned char *ucIP, unsigned short usPort, unsigned char *ucData, unsigned short usDataLen)
{
    int iNetwork = DEFAULT_NETWORK;
    NetBIOS *ptrNetBIOS = (NetBIOS *)ucData;                             // treat the frame as a NetBIOS frame

    if (ptrNetBIOS->flags[0] & NetBIOS_RESPONSE_FLAG) {                  // ignore responses
        return 0;
    }

    if ((ptrNetBIOS->flags[0] & NetBIOS_OPCODE_MASK) != NetBIOS_NAME_QUERY) { // accept only name query
        return 0;
    }

    if ((ptrNetBIOS->questions[0] != 0) || (ptrNetBIOS->questions[1] != 1)) {
        return 0;                                                        // we only accept single queries
    }

    if (ptrNetBIOS->query.ucLabelLength != 0x20) {                       // if not NetBIOS name with fixed length, ignore
        return 0;
    }
#if IP_NETWORK_COUNT > 1
    iNetwork = extractNetwork(NetBIOS_SocketNr);                         // the network that the request is being handled on
#endif    
    if (uMemcmp(network[iNetwork].ucOurIP, cucNullMACIP, IPV4_LENGTH) == 0) { // {2} if the IP address is presently 0.0.0.0 (usually because DHCP is still in progress) we don't answer
        return 0;
    }
    else {
        // We have received a simple single query of a NetBIOS name so we see whether we are being searched.
        // The name uses a simple first level decoding which has to be extracted first
        //
        unsigned char ucTestOut[NetBIOS_NAME_MAX];                       // a fixed length buffer to extract the name to - the name is usually padded with spaces after it
        int i = 0;
        int ucLabelLength = (NetBIOS_NAME_MAX * 2);
        unsigned char *ptrName = ptrNetBIOS->query.NetBIOS_name;
        NetBIOS_ANSWER *ptrAnswer = (NetBIOS_ANSWER *)ucData;

        while (ucLabelLength >= 2) {
            ucTestOut[i] = ((*ptrName++ - 'A') << 4);
            ucTestOut[i++] |= (*ptrName++ - 'A');
            ucLabelLength -= 2;
        }

        i = uStrEquiv((CHAR *)ucTestOut, NetBIOS_name);                  // check the length of the match - due to padding, it is usually shorter than the search string
        while (ucTestOut[i] != 0) {                                      // check that the rest is really padding
            if (' ' != ucTestOut[i]) {
                return 0;                                                // searched name is longer than ours so ignore
            }
            if (++i >= NetBIOS_NAME_MAX) {                               // the extracted sting is not null-terminated - which is not expected
                return 0;
            }
        }

        // If we get this far it means that a request is being made for our name, so answer with our IP address
        // - directly manipulate the receive buffer and use it for our answer packet
        //
        ptrAnswer->query.flags[0] = (NetBIOS_RESPONSE_FLAG | NetBIOS_AUTHORITATIVE_FLAG | NetBIOS_RECURSIVE_DESIRED_FLAG);
        ptrAnswer->query.flags[1] = 0;
        ptrAnswer->query.questions[1] = 0;
        ptrAnswer->query.answer_RRs[0] = 0; ptrAnswer->query.answer_RRs[1] = 1; // one answer
        uMemcpy(ptrAnswer->ttl, ucTTL, sizeof(ptrAnswer->ttl));
        ptrAnswer->length[0] = 0;
        ptrAnswer->length[1] = sizeof(ptrAnswer->flags) + IPV4_LENGTH;
        ptrAnswer->flags[0] = ptrAnswer->flags[1] = 0;

        uMemcpy(ptrAnswer->ip_address, &network[iNetwork].ucOurIP[0], IPV4_LENGTH); // add our ip address
                                                                         // send an answer so that we can be contacted by using our name
        return ((fnSendUDP(NetBIOS_SocketNr, ucIP, usPort, ((unsigned char *)ptrNetBIOS - sizeof(UDP_HEADER)), sizeof(NetBIOS_ANSWER), 0)) != 0);
    }
}
#endif
