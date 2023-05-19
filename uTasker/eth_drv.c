/**********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher

    ---------------------------------------------------------------------
    File:      eth_drv.c
    Project:   Single Chip Embedded Internet
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2016
    *********************************************************************
    15.03.2007 VLAN transmission support added                           {1}
    07.05.2007 Added STR91XF support                                     {2}
    30.07.2007 Added LPC23XX support
    18.11.2007 Added LM3SXXXX support                                    {3}
    10.11.2011 Interrupt configuartion if the ethernet hardware configuration is not successful {4}
    16.12.2012 Add SUPPORT_DYNAMIC_VLAN_TX                               {5}
    17.12.2012 Add ALTERNATIVE_VLAN_COUNT                                {6}
    04.06.2013 Added ETH_DRV_MALLOC() default                            {7}
    16.04.2014 Add multicast filter control for IGMP reception           {8}
    25.06.2014 Add request to see whether the transmitter is presently busy {9}
    22.08.2014 Add interrupt mask settings to block pre-emption when frame is being prepared {10}
    22.06.2015 Correct output buffer length check to allow maximum packet size exactly equal to the output buffer size {11}

*/


/* =================================================================== */
/*                           include files                             */
/* =================================================================== */


#include "config.h"


#if defined ETH_INTERFACE || (defined USB_CDC_RNDIS && defined USB_TO_TCP_IP)

/* =================================================================== */
/*                          local definitions                          */
/* =================================================================== */

#if !defined ETH_DRV_MALLOC                                              // {7}
    #define ETH_DRV_MALLOC(x)      uMalloc((MAX_MALLOC)(x))
#endif
#define ETH_BUFF_COPY(a,b,c) uMemcpy(a,b,c)

/* =================================================================== */
/*                       local structure definitions                   */
/* =================================================================== */


/* =================================================================== */
/*                 local function prototype declarations               */
/* =================================================================== */


/* =================================================================== */
/*                             constants                               */
/* =================================================================== */


/* =================================================================== */
/*                      local variable definitions                     */
/* =================================================================== */

/* =================================================================== */
/*                     global variable definitions                     */
/* =================================================================== */

#if defined _HW_SAM7X
    ETHERNETQue *eth_tx_control;
    ETHERNETQue *eth_rx_control;
#endif

/* =================================================================== */
/*                      local function definitions                     */
/* =================================================================== */


/* =================================================================== */
/*                      global function definitions                    */
/* =================================================================== */



// Standard entry call to driver - dispatches required sub-routine
//
static QUEUE_TRANSFER entry_eth(QUEUE_HANDLE channel, unsigned char *ptBuffer, QUEUE_TRANSFER Counter, unsigned char ucCallType, QUEUE_HANDLE DriverID)
{
    ETHERNETQue *ptTTYQue;
    unsigned short rtn_val = 0;

    // Special case for ethernet write - note that we do not protect interrupts since it is not necessary here
    //
    if (ucCallType == CALL_WRITE) {
        ptTTYQue = (struct stETHERNETQue *)(que_ids[DriverID].output_buffer_control); // set to output control block
        if (ptBuffer != 0) {                                             // data being passed
#if !defined (_HW_NE64)                                                  // {2}
            if ((Counter != 0) && (Counter <= (ptTTYQue->ETH_queue.buf_length - ptTTYQue->ETH_queue.chars))) // {11}
#else
            if ((Counter != 0) && (ptTTYQue->ETH_queue.chars < ptTTYQue->ETH_queue.buf_length))
#endif
            {
                if (ptTTYQue->ETH_queue.chars == 0) {                    // first part of a frame
#if !defined (_HW_NE64)                                                  // {2}{3}
                    uDisable_Interrupt();                                // {9} disable all interrupts to avoid conflicts in case an interrupt routine could attempt to acquire the buffer
    #if defined EMAC_PREEMPT_LEVEL
                    uMask_Interrupt(EMAC_PREEMPT_LEVEL);                 // {10} set interrupt mask level to block low priority pre-emption until the frame has been completed and released
    #endif
    #if !defined ETH_INTERFACE || !defined ETHERNET_AVAILABLE || defined NO_INTERNAL_ETHERNET || (ETHERNET_INTERFACES > 1) || defined USB_CDC_RNDIS
                    ptTTYQue->ETH_queue.put = ((ETHERNET_FUNCTIONS *)(que_ids[DriverID].ptrDriverFunctions))->fnGetTxBufferAdd(0);
    #else
                    ptTTYQue->ETH_queue.put = fnGetTxBufferAdd(0);       // get next buffer space
    #endif
                    uEnable_Interrupt();                                 // {9} enable all interrupts - a non-zero value in ptTTYQue->ETH_queue.put now signals that the buffer is in use
#endif
#if !defined _LM3SXXXX                                                   // {3}
    #if !defined ETH_INTERFACE || !defined ETH_INTERFACE || !defined ETHERNET_AVAILABLE || defined NO_INTERNAL_ETHERNET || (ETHERNET_INTERFACES > 1) || defined USB_CDC_RNDIS
                    if (((ETHERNET_FUNCTIONS *)(que_ids[DriverID].ptrDriverFunctions))->fnWaitTxFree() != 0)
    #else
                    if (fnWaitTxFree() != 0)                             // wait for a short time if the buffer is not free
    #endif
                    {
    #if defined EMAC_PREEMPT_LEVEL
                        uMask_Interrupt(LOWEST_PRIORITY_PREEMPT_LEVEL);  // {10} allow pre-emption again
    #endif
                        return 0;                                        // the buffer is busy and remains so for a long time so abort
                    }
#endif
#if defined SUPPORT_VLAN                                                 // {1}
    #if defined SUPPORT_DYNAMIC_VLAN_TX
                    if (fnVLAN_tag_tx(channel) != 0)                     // {5} allow the user to define whether the outgoing frame is VLAN tagged on this channel
    #else
                    if (vlan_active) 
    #endif
                    {                                                    // VLAN tag on first write - it is assumed that only normal Ethernet frames are sent
                        unsigned char ucVLAN_tag[VLAN_TAG_LENGTH];
                        ETH_BUFF_COPY(ptTTYQue->ETH_queue.put, ptBuffer, (2 * MAC_LENGTH)); // copy MAC addresses
                        ptTTYQue->ETH_queue.put += (2 * MAC_LENGTH);
                        ptBuffer += (2 * MAC_LENGTH);
                        ucVLAN_tag[0] = (unsigned char)(TAG_PROTOCOL_IDENTIFIER>>8);
                        ucVLAN_tag[1] = (unsigned char)(TAG_PROTOCOL_IDENTIFIER);
    #if defined SUPPORT_DYNAMIC_VLAN && (defined ALTERNATIVE_VLAN_COUNT && ALTERNATIVE_VLAN_COUNT > 0) // {6}
                        ucVLAN_tag[2] = (unsigned char)(vlan_vid[vlan_active - 1] >> 8); // insert the appropriate VLAN tag
                        ucVLAN_tag[3] = (unsigned char)vlan_vid[vlan_active - 1];
    #else
                        ucVLAN_tag[2] = (unsigned char)(vlan_vid >> 8);
                        ucVLAN_tag[3] = (unsigned char)vlan_vid;
    #endif
                        ETH_BUFF_COPY(ptTTYQue->ETH_queue.put, ucVLAN_tag, VLAN_TAG_LENGTH); // copy VLAN tag
                        ptTTYQue->ETH_queue.put += VLAN_TAG_LENGTH;
                        ptTTYQue->ETH_queue.chars = (2 * MAC_LENGTH + VLAN_TAG_LENGTH);
                        Counter -= (2 * MAC_LENGTH);
                    }
#endif
                }
#if !defined ETHERNET_AVAILABLE || defined NO_INTERNAL_ETHERNET || ETHERNET_INTERFACES > 1 || defined USB_CDC_RNDIS
                ((ETHERNET_FUNCTIONS *)(que_ids[DriverID].ptrDriverFunctions))->fnPutInBuffer(ptTTYQue->ETH_queue.put, ptBuffer, Counter);
#else
                ETH_BUFF_COPY(ptTTYQue->ETH_queue.put, ptBuffer, Counter); // copy since buffers are linear
#endif
                ptTTYQue->ETH_queue.put += Counter;                      // increment pointer in output buffer
                ptTTYQue->ETH_queue.chars += Counter;                    // the number of bytes in the output buffer that will be sent
                return (Counter);
            }
#if defined USE_IGMP                                                     // {8}
            else {                                                       // use this case to communicate multicast filter requirements
    #if !defined ETH_INTERFACE || !defined ETHERNET_AVAILABLE || defined NO_INTERNAL_ETHERNET || ETHERNET_INTERFACES > 1 || defined USB_CDC_RNDIS
                ((ETHERNET_FUNCTIONS *)(que_ids[DriverID].ptrDriverFunctions))->fnModifyMulticastFilter(Counter, ptBuffer);
    #else
                fnModifyMulticastFilter(Counter, ptBuffer);
    #endif
            }
#endif
            return 0;
        }
        else {
#if !defined (_HW_NE64)                                                  // {2}{3}
            QUEUE_TRANSFER TxLength = ptTTYQue->ETH_queue.chars;
            ptTTYQue->ETH_queue.chars = 0;
    #if !defined ETH_INTERFACE || !defined ETHERNET_AVAILABLE || defined NO_INTERNAL_ETHERNET || (ETHERNET_INTERFACES > 1) || defined USB_CDC_RNDIS
            rtn_val = ((ETHERNET_FUNCTIONS *)(que_ids[DriverID].ptrDriverFunctions))->fnStartEthTx(TxLength, ptTTYQue->ETH_queue.put);
    #else
            rtn_val = fnStartEthTx(TxLength, ptTTYQue->ETH_queue.put);   // this causes frame to be sent
    #endif
            ptTTYQue->ETH_queue.put = 0;                                 // {9} output buffer no longer in use
    #if defined EMAC_PREEMPT_LEVEL
            uMask_Interrupt(LOWEST_PRIORITY_PREEMPT_LEVEL);              // {10} allow pre-emption again
    #endif
            return rtn_val;
#else
            return (fnStartEthTx(ptTTYQue->ETH_queue.chars, ptTTYQue->ETH_queue.put)); // this causes frame to be sent
#endif
        }
    }

    uDisable_Interrupt();                                                // disable all interrupts

    switch (ucCallType) {
    case CALL_DRIVER:                                                    // {9} request whether the transmit buffer is presently in use
        ptTTYQue = (struct stETHERNETQue *)(que_ids[DriverID].output_buffer_control); // set to output control block
        rtn_val = (ptTTYQue->ETH_queue.put != 0);
        break;

    case CALL_CLOSE:                                                     // close a driver channel and de-allocate its memory
        break;

    case CALL_READ_LINEAR:
        ptTTYQue = (struct stETHERNETQue *)(que_ids[DriverID].input_buffer_control); // set to input control block
        while (Counter--) {
            ptTTYQue = ptTTYQue->NextTTYbuffer;
        }
        *(unsigned char **)ptBuffer = ptTTYQue->ETH_queue.QUEbuffer;     // set pointer to data
        rtn_val = ptTTYQue->ETH_queue.chars;                             // return the number of characters in the input buffer
        break;

    case CALL_FREE:
        ptTTYQue = (struct stETHERNETQue *)(que_ids[DriverID].input_buffer_control); // set to input control block
#if !defined ETH_INTERFACE || !defined ETHERNET_AVAILABLE || defined NO_INTERNAL_ETHERNET || (ETHERNET_INTERFACES > 1) || defined USB_CDC_RNDIS
        ((ETHERNET_FUNCTIONS *)(que_ids[DriverID].ptrDriverFunctions))->fnFreeEthernetBuffer(Counter);
#else
        fnFreeEthernetBuffer(Counter);
#endif
#if !defined _LPC23XX && !defined _LPC17XX && !defined _LM3SXXXX && !defined _STM32 && !defined _KINETIS
        while (Counter-- != 0) {
            ptTTYQue = ptTTYQue->NextTTYbuffer;
        }
#endif
        ptTTYQue->ETH_queue.chars = 0;
        break;

    default:
        break;
    }
    uEnable_Interrupt();                                                 // enable interrupts
    return (rtn_val);
}


// Open the Ethernet interface
//
extern QUEUE_HANDLE fnOpenETHERNET(ETHTABLE *pars, unsigned short driver_mode)
{
    QUEUE_HANDLE DriverID;
    int iRxBuffers = 0;
    void *new_memory;
    ETHERNETQue *ptEthQue = 0;
    QUEUE_TRANSFER (*entry_add)(QUEUE_HANDLE channel, unsigned char *ptBuffer, QUEUE_TRANSFER Counter, unsigned char ucCallType, QUEUE_HANDLE DriverID) = entry_eth;

    if (NO_ID_ALLOCATED != (DriverID = fnSearchID(entry_add, pars->Channel))) { // check to see whether this interface has already been opened
        return DriverID;
    }
    else {
        if (NO_ID_ALLOCATED == (DriverID = fnSearchID (0, 0))) {         // get next free ID
            return (NO_ID_ALLOCATED);                                    // no free IDs available
        }
    }

    --DriverID;                                                          // convert to array offset

  //que_ids[ucDriverID].ulEntryAddress = 0;                              already clear since from heap
  //que_ids[ucDriverID].input_buffer_control = 0;
  //que_ids[ucDriverID].output_buffer_control = 0;

#if defined(SUPPORT_DISTRIBUTED_NODES) && defined (UPROTOCOL_WITH_RETRANS)
    fnInitUNetwork();
#endif
#if !defined ETH_INTERFACE || !defined ETHERNET_AVAILABLE || defined NO_INTERNAL_ETHERNET || (ETHERNET_INTERFACES > 1) || defined USB_CDC_RNDIS
    que_ids[DriverID].ptrDriverFunctions = pars->ptrEthernetFunctions;
    if (((ETHERNET_FUNCTIONS *)(pars->ptrEthernetFunctions))->fnConfigEthernet(pars) != 0) {
        return NO_ID_ALLOCATED;
    }
    while (iRxBuffers < ((ETHERNET_FUNCTIONS *)(pars->ptrEthernetFunctions))->fnGetQuantityRxBuf())
#else
    if (fnConfigEthernet(pars) != 0) {                                   // configure the physical device
        return NO_ID_ALLOCATED;                                          // {4} the hardware configuration was not successful
    }                                                                    // we need to configure it before getting all buffer resources
                                                                         // since some internal buffer addresses depend on parameters
    while (iRxBuffers < fnGetQuantityRxBuf())                            // configure the new driver and its queue(s)
                                                                         // define receiver
                                                                         // allocate memory for the driver queue and set up structures
#endif
    {
        if (NO_MEMORY == (new_memory = ETH_DRV_MALLOC(sizeof(struct stETHERNETQue)))) {
            return (NO_ID_ALLOCATED);                                    // failed, no memory
        }
        if (ptEthQue != 0) {
            ptEthQue->NextTTYbuffer = new_memory;
            ptEthQue = ptEthQue->NextTTYbuffer;
        }
        else {
            que_ids[DriverID].input_buffer_control = new_memory;
            ptEthQue = (struct stETHERNETQue *)(que_ids[DriverID].input_buffer_control);
    #if defined _HW_SAM7X
            eth_rx_control = ptEthQue;
    #endif
        }
      //ptEthQue->NextTTYbuffer = 0;
      //ptTTYQue->opn_mode = 0;
    #if defined ETH_INTERFACE && (defined ETHERNET_AVAILABLE && !defined NO_INTERNAL_ETHERNET)
        ptEthQue->ETH_queue.get = ptEthQue->ETH_queue.put = ptEthQue->ETH_queue.QUEbuffer = fnGetRxBufferAdd(iRxBuffers);
    #endif
        ptEthQue->ETH_queue.buf_length = pars->usSizeRx;
        if (pars->Task_to_wake != 0) {                                   // if a task is entered
            ptEthQue->wake_task = pars->Task_to_wake;                    // this task is to be woken on events
        }
      //que_ids[DriverID].CallAddress = entry_add;
        iRxBuffers++;
    }
                                                                         // define transmitter
    if (NO_MEMORY == (new_memory = ETH_DRV_MALLOC(sizeof(struct stETHERNETQue)))) { // allocate memory for the driver queue and set up structures
        return (NO_ID_ALLOCATED);                                        // failed, no memory
    }
    que_ids[DriverID].output_buffer_control = new_memory;

    ptEthQue = (struct stETHERNETQue *)(que_ids[DriverID].output_buffer_control);
    #if defined _HW_SAM7X
    eth_tx_control = ptEthQue;
    #endif
    ptEthQue->ETH_queue.put = ptEthQue->ETH_queue.get = ptEthQue->ETH_queue.QUEbuffer = 0; // {9} fnGetTxBufferAdd(0) replaced since 0 means that the buffer is presently not in use
    ptEthQue->ETH_queue.buf_length = pars->usSizeTx;
  //ptEthQue->opn_mode = 0;

    que_ids[DriverID].CallAddress = entry_add;    

    if (0 == que_ids[DriverID].CallAddress) {
        return (NO_ID_ALLOCATED);                                        // call was not valid
    }
    else {                                                               // fill general structure entries
        que_ids[DriverID].qHandle = pars->Channel;
        return (DriverID + 1);                                           // return the allocated ID - begins with 1 ... MAX
    }
}
#endif
