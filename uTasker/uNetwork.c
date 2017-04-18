/**********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher

    ---------------------------------------------------------------------
    File:      uNetwork.c
    Project:   Single Chip Embedded Internet
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2016
    *********************************************************************
    10.03.2007 Sequence number correction                                {1}
    08.03.2007 Improve global message sequence number handling           {2}
    23.08.2007 Add next rx acknowledgement count to ensure correct order of received acks {3}
    10.03.2007 Sequence number correction on sequence count overflow     {4}
    04.09.2007 Message defines changed to aid in monitoring/debugging    {5}
    09.06.2008 uNetwork Broadcast address made definable                 {6}
    02.07.2008 Add fnResetuNetwork_stats()                               {7}
    13.10.2012 Add fnIncNextAck() subroutine and use after tx message loss {8}
    04.06.2013 Added UNETWORK_MALLOC() default                           {9}

*/


#include "config.h"


#if defined SUPPORT_DISTRIBUTED_NODES

#if !defined UNETWORK_MALLOC                                             // {9}
    #define UNETWORK_MALLOC(x)    uMalloc((MAX_MALLOC)(x))
#endif

#define FIRST_UPROT_TIMER         1
#define MESSAGE                   'M'                                    // {5}
#define REP_MESSAGE               'R'                                    // repeated message
#define UNET_ACK                  'A'                                    // acknowledge
#define NO_ACK_REQ                'B'                                    // broadcast


#if !defined UNET_BROADCAST_MAC_ADDRESS                                  // {6}
    #define UNET_BROADCAST_MAC_ADDRESS cucBroadcast                      // standard broadcast address if nothing defined
#endif


#if defined VARIABLE_PROTOCOL_UNETWORK                                   // {4}
    unsigned char uc_uNetworkProtocol[2] = {(unsigned char)(PROTOCOL_UNETWORK >> 8), (unsigned char)(PROTOCOL_UNETWORK)};
#else
    static const unsigned char uc_uNetworkProtocol[] = {(unsigned char)(PROTOCOL_UNETWORK >> 8), (unsigned char)(PROTOCOL_UNETWORK)};
#endif

#if defined GLOBAL_HARDWARE_TIMER
    #define OUR_TASK              (UTASK_TASK)(FUNCTION_EVENT | HARDWARE_TIMER)
#else
    #define OUR_TASK              FUNCTION_EVENT
#endif

#if defined UPROTOCOL_WITH_RETRANS
typedef struct stUPROTCONNECTION {
    unsigned char   ucDestNodeNr;
    unsigned char   ucNextSeqNr;
    unsigned char   ucNextAckNr;
    unsigned char   ucNextRxAck;                                         // {3}
} UPROTCONNECTION;

typedef struct stUPROTSAVEDMSG {
    QUEUE_TRANSFER  MsgLength;
    unsigned char   ucDestNodeNr;
    unsigned char   ucSeqNr;
    UTASK_TASK      SendingTask;
    unsigned char   ucRetransmission;
    unsigned char   *ptrMsgData;
} UPROTSAVEDMSG;

typedef struct stUPROT_MSG_ACK {
    unsigned char   ucMsgOrAck;
    unsigned char   ucSeqNr;
    unsigned char   ucNodeNumber;
} UPROT_MSG_ACK;

static UPROTCONNECTION retransTable[MAX_NETWORK_NODES];
static UPROTSAVEDMSG   msgPool[MAX_STORED_MESSAGES] = {0};

#if defined ENABLE_UNETWORK_STATS
    static UNETWORK_STATS uNetwork_stats = {0};
#endif

static void fnUprotRetrans(unsigned char ucEventNr);

extern void fnInitUNetwork(void)
{
    unsigned char i;

    for (i = 0; i < MAX_NETWORK_NODES; i++) {
        retransTable[i].ucNextSeqNr = 0;                                 // synchronise to anything
        retransTable[i].ucNextAckNr = 0;                                 // ensure our first message always accepted
        retransTable[i].ucNextRxAck = 0;                                 // {3}
    }

    for (i = 0; i < MAX_STORED_MESSAGES; i++) {
        msgPool[i].ucDestNodeNr = 0;
        if (!msgPool[i].ptrMsgData) {
            msgPool[i].ptrMsgData = UNETWORK_MALLOC(MAX_UPROT_MSG_SIZE);
        }
    }

    fnSetFuncTask(fnUprotRetrans);
}
#endif

extern QUEUE_TRANSFER fnDistributedTx(unsigned char *output_buffer, QUEUE_TRANSFER nr_of_bytes) // send the message across the network to destination task
{
#if !defined UPROTOCOL_WITH_RETRANS
    unsigned char  ucData[2 * MAC_LENGTH + sizeof(uc_uNetworkProtocol)]; // space for temporary raw Ethernet II and payload
#else
    UPROT_MSG_ACK  *msgSeq;
    unsigned char  ucData[2 * MAC_LENGTH + sizeof(uc_uNetworkProtocol) + sizeof(UPROT_MSG_ACK)]; // space for temporary raw Ethernet II and payload
    unsigned short i;
    unsigned char  ucType = NO_ACK_REQ;
#endif

    if ((!Ethernet_handle[0]) || (!OurNetworkNumber)) {                  // trying to send before network ready...
        return 0;
    }
    if (output_buffer[MSG_DESTINATION_NODE] == GLOBAL_MESSAGE) {
        uMemcpy(&ucData[0], UNET_BROADCAST_MAC_ADDRESS, MAC_LENGTH);     // {6} broadcast
#if defined ENABLE_UNETWORK_STATS
        uNetwork_stats.ulBroadcast_tx++;
#endif
    }
    else {
        if ((output_buffer[MSG_DESTINATION_NODE] > MAX_NETWORK_NODES) || (!uMemcmp(ucNodeMac[output_buffer[MSG_DESTINATION_NODE] - 1], cucNullMACIP, MAC_LENGTH))) {
            return 0;
        }
        uMemcpy(&ucData[0], ucNodeMac[output_buffer[MSG_DESTINATION_NODE] - 1], MAC_LENGTH); // add datalink (Ethernet addresses) information
#if defined ENABLE_UNETWORK_STATS
        uNetwork_stats.ulTx_frames++;
#endif
    }

    uMemcpy(&ucData[MAC_LENGTH], ucNodeMac[OurNetworkNumber - 1], MAC_LENGTH);
    uMemcpy(&ucData[2 * MAC_LENGTH], uc_uNetworkProtocol, sizeof(uc_uNetworkProtocol));

#if defined UPROTOCOL_WITH_RETRANS
    if ((output_buffer[MSG_DESTINATION_NODE] <= MAX_NETWORK_NODES) && (nr_of_bytes <= MAX_UPROT_MSG_SIZE)) {
        if (output_buffer[MSG_DESTINATION_NODE] != GLOBAL_MESSAGE) {
            for ( i = 0; i <  MAX_STORED_MESSAGES; i++) {                // save the message for possible retransmission
                if (msgPool[i].ucDestNodeNr == 0) {                      // space free
                    msgPool[i].ucDestNodeNr = output_buffer[MSG_DESTINATION_NODE];
                    msgPool[i].ucSeqNr = retransTable[output_buffer[MSG_DESTINATION_NODE]-1].ucNextAckNr;
                    msgPool[i].SendingTask = output_buffer[MSG_SOURCE_TASK];
                    msgPool[i].ucRetransmission = 0;
                    msgPool[i].MsgLength = nr_of_bytes;
                    uMemcpy(msgPool[i].ptrMsgData, output_buffer, nr_of_bytes);// backup the transmission
                    uTaskerGlobalMonoTimer(OUR_TASK, UPROT_RETRANS_TIME, (unsigned char)(i + FIRST_UPROT_TIMER));
                    ucType = MESSAGE;
                    break;
                }
            }
        }
    }

    msgSeq = (UPROT_MSG_ACK*)&ucData[2*MAC_LENGTH+sizeof(uc_uNetworkProtocol)];
    msgSeq->ucMsgOrAck = ucType;

    if (ucType == NO_ACK_REQ) {                                          // {2}
        msgSeq->ucSeqNr = 0;
    }
    else {
        msgSeq->ucSeqNr = retransTable[output_buffer[MSG_DESTINATION_NODE] - 1].ucNextAckNr++; // add sequence number
        if (msgSeq->ucSeqNr == 0xff) {                                   // {4}
            retransTable[output_buffer[MSG_DESTINATION_NODE] - 1].ucNextAckNr = 1; // wrap around sequence number
        }
    }
    msgSeq->ucNodeNumber = OurNetworkNumber;
#endif

    if (fnWrite(Ethernet_handle[0], ucData, (QUEUE_TRANSFER)(sizeof(ucData))) == 0) { // prepare the frame for transmission
        return 0;                                                        // failed
    }
    fnWrite(Ethernet_handle[0], output_buffer, nr_of_bytes);             // add the pay load
    return (fnWrite(Ethernet_handle[0], 0, 0));                          // transmit the ETHERNET frame
}

// Increment the ucNextRxAck counter in the retransmission entry
//
static void fnIncNextAck(unsigned char ucMessageNumber)                  // {8}
{
    unsigned char *ptrNextRxAct = &(retransTable[msgPool[ucMessageNumber].ucDestNodeNr - 1].ucNextRxAck);
    *ptrNextRxAct = (*ptrNextRxAct + 1);                                 // increment the next acknowledge count
    if (*ptrNextRxAct == 0) {                                            // avoid 0
        *ptrNextRxAct = 1;                                               // and skip to 1 to match the sequence counter
    }
}

// Distributed intertask message received from the network. Pass it on to the correct task
//
extern void fnHandle_unet(unsigned char *ucData)
{
    QUEUE_TRANSFER Length = HEADER_LENGTH;

#if defined UPROTOCOL_WITH_RETRANS
    UPROT_MSG_ACK  *msgSeq, *msgAck;
    unsigned char  i, ucReply[2*MAC_LENGTH + sizeof(uc_uNetworkProtocol) + sizeof(UPROT_MSG_ACK)]; // space for temporary raw Ethernet II and payload
    unsigned char *NextSeq;

    msgSeq = (UPROT_MSG_ACK *)ucData;
    ucData += sizeof(UPROT_MSG_ACK);

    if ((msgSeq->ucMsgOrAck == MESSAGE) || (msgSeq->ucMsgOrAck == REP_MESSAGE)) { // this is a new message -> send ack {5}
        uMemcmp(ucNodeMac[ucData[MSG_SOURCE_NODE] - 1], cucNullMACIP, MAC_LENGTH);
        uMemcpy(&ucReply[0], ucNodeMac[ucData[MSG_SOURCE_NODE] - 1], MAC_LENGTH); // add datalink (Ethernet addresses) information
        uMemcpy(&ucReply[MAC_LENGTH], ucNodeMac[OurNetworkNumber-1], MAC_LENGTH);
        uMemcpy(&ucReply[2 * MAC_LENGTH], uc_uNetworkProtocol, sizeof(uc_uNetworkProtocol));
        msgAck = (UPROT_MSG_ACK *)&ucReply[2*MAC_LENGTH+sizeof(uc_uNetworkProtocol)];
        msgAck->ucMsgOrAck = UNET_ACK;
        msgAck->ucSeqNr = msgSeq->ucSeqNr;
        msgAck->ucNodeNumber = OurNetworkNumber;

        if (fnWrite(Ethernet_handle[0], ucReply, (QUEUE_TRANSFER)(sizeof(ucReply))) != 0) { // prepare the frame for transmission
            fnWrite(Ethernet_handle[0], 0, 0);                           // transmit the ETHERNET frame
    #if defined ENABLE_UNETWORK_STATS
            uNetwork_stats.ulTx_ack++;
    #endif
        }
    }
    else if (msgSeq->ucMsgOrAck == UNET_ACK) {                           // this is an ACK - find entry in message pool, stop timer and delete it
        for (i = 0; i < MAX_STORED_MESSAGES; i++) {
                                                                         // we expect acks in the order that the data was sent
            if ((msgPool[i].ucDestNodeNr == msgSeq->ucNodeNumber) && (retransTable[msgPool[i].ucDestNodeNr - 1].ucNextRxAck == msgSeq->ucSeqNr) && (msgPool[i].ucSeqNr == msgSeq->ucSeqNr)) { // {3}
                uTaskerGlobalStopTimer(FUNCTION_EVENT, (unsigned char)(i + FIRST_UPROT_TIMER));  //stop timer
                fnIncNextAck(i);                                         // {8}
                msgPool[i].ucDestNodeNr = 0;
    #if defined ENABLE_UNETWORK_STATS
                uNetwork_stats.ulRx_ack++;
    #endif
                break;
            }
        }
    #if defined ENABLE_UNETWORK_STATS
        if (i >= MAX_STORED_MESSAGES) {                                  // ack could not be matched to a transmitted message (possibly out of sequence)
            uNetwork_stats.ulRx_bad_ack++;
        }
    #endif
        return;                                                          // no need to pass ACK to application
    }
    NextSeq = &retransTable[ucData[MSG_SOURCE_NODE] - 1].ucNextSeqNr;

    if ((*NextSeq == msgSeq->ucSeqNr) || (*NextSeq == 0)  || (msgSeq->ucSeqNr == 0)) { // ignored repeated messages
        if (msgSeq->ucMsgOrAck != NO_ACK_REQ) {                          // {2}
            if (msgSeq->ucSeqNr == 0xff) {                               // {1}
                *NextSeq = 1;                                            // wrap around sequence number (avoiding zero)
            }
            else {
                *NextSeq = (msgSeq->ucSeqNr + 1);
            }
        }

        if (ucData[MSG_SOURCE_TASK] != INTERRUPT_EVENT) {
           Length += ucData[MSG_CONTENT_LENGTH];
        }
        ucData[MSG_DESTINATION_NODE] = ucData[MSG_SOURCE_NODE];          // avoid re-transmission

#if defined ENABLE_UNETWORK_STATS
        uNetwork_stats.ulRx_frames++;
#endif
        fnWrite(INTERNAL_ROUTE, ucData, Length);
    }
    else {
#if defined ENABLE_UNETWORK_STATS
        uNetwork_stats.ulRxSynch_error++;                                // ignored due to sequence count error
#endif
        fnInterruptMessage(UNETWORK_MASTER, UNETWORK_SYNC_LOSS);         // inform of sync loss - probably after network problem
    }
#else
    if (ucData[MSG_SOURCE_TASK] != INTERRUPT_EVENT) {
       Length += ucData[MSG_CONTENT_LENGTH];
    }
    ucData[MSG_DESTINATION_NODE] = ucData[MSG_SOURCE_NODE];              // avoid re-transmission

#if defined ENABLE_UNETWORK_STATS
    uNetwork_stats.ulRx_frames++;
#endif

    fnWrite(INTERNAL_ROUTE, ucData, Length);
#endif
}

#if defined UPROTOCOL_WITH_RETRANS
static void fnUprotRetrans(unsigned char ucEventNr)
{
    unsigned char  ucMsgNr = (ucEventNr - FIRST_UPROT_TIMER);            // the event number defines the message in the pool for which the timer fired
    UPROT_MSG_ACK *msgSeq;
    unsigned char  ucData[2 * MAC_LENGTH + sizeof(uc_uNetworkProtocol) + sizeof(UPROT_MSG_ACK)]; // space for temporary raw Ethernet II and payload

    if (++(msgPool[ucMsgNr].ucRetransmission) > MAX_UPROT_RETRANS_ATTEMPS) { // check for maximum transmissions to this node without a response
        fnInterruptMessage(UNETWORK_MASTER, UNETWORK_FRAME_LOSS);        // inform of frame loss - probably destination not reachable
        fnIncNextAck(ucMsgNr);                                           // {8} ensure the counter remains synchronised after transmit message loss
        msgPool[ucMsgNr].ucDestNodeNr = 0;
#if defined ENABLE_UNETWORK_STATS
        uNetwork_stats.ulTx_lost++;
#endif
    }
    else {
        uMemcmp(ucNodeMac[msgPool[ucMsgNr].ucDestNodeNr - 1], cucNullMACIP, MAC_LENGTH);
        uMemcpy(&ucData[0], ucNodeMac[msgPool[ucMsgNr].ucDestNodeNr - 1], MAC_LENGTH); // add datalink (Ethernet addresses) information

        uMemcpy(&ucData[MAC_LENGTH], ucNodeMac[OurNetworkNumber-1], MAC_LENGTH);
        uMemcpy(&ucData[2 * MAC_LENGTH], uc_uNetworkProtocol, sizeof(uc_uNetworkProtocol));

        msgSeq = (UPROT_MSG_ACK *)&ucData[2 * MAC_LENGTH+sizeof(uc_uNetworkProtocol)];
        msgSeq->ucNodeNumber = OurNetworkNumber;                         // msgPool[ucMsgNr].ucDestNodeNr; {5}
        msgSeq->ucMsgOrAck = REP_MESSAGE;                                // mark that repeated message to aid in monitoring/debugging {5}
        msgSeq->ucSeqNr = msgPool[ucMsgNr].ucSeqNr;                      // add sequence number

        if (fnWrite(Ethernet_handle[0], ucData, (QUEUE_TRANSFER)(sizeof(ucData))) == 0) { // prepare the frame for transmission
            return;
        }
        fnWrite(Ethernet_handle[0], msgPool[ucMsgNr].ptrMsgData, msgPool[ucMsgNr].MsgLength); // add the pay load
        fnWrite(Ethernet_handle[0], 0, 0);                               // retransmit the ETHERNET frame

#if defined ENABLE_UNETWORK_STATS
        uNetwork_stats.ulTx_rep++;
#endif
#if defined EXP_BACKOFF
        uTaskerGlobalMonoTimer(OUR_TASK, (DELAY_LIMIT)(UPROT_RETRANS_TIME * (1 << msgPool[ucMsgNr].ucRetransmission)), (unsigned char)(FIRST_UPROT_TIMER + ucMsgNr));
#else
        uTaskerGlobalMonoTimer(OUR_TASK, UPROT_RETRANS_TIME, (unsigned char)(FIRST_UPROT_TIMER + ucMsgNr));
#endif
    }
}
#endif

#if defined ENABLE_UNETWORK_STATS
// Retrieve a uNetwork counter
//
extern unsigned long fnGetuNetworkStats(unsigned char ucStat)
{
    return ((unsigned long *)(&uNetwork_stats))[ucStat];                 // return the specified value
}

// Reset all uNetwork counters to zero
//
extern void fnResetuNetwork_stats(void)                                  // {7}
{
    uMemset(&uNetwork_stats, 0, sizeof(uNetwork_stats));
}
#endif

#endif
