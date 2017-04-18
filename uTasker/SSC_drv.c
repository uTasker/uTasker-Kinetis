/**********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher

    ---------------------------------------------------------------------
    File:      SSC_drv.c
    Project:   Single Chip Embedded Internet
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2016
    *********************************************************************
    04.06.2013 Added SSC_DRV_MALLOC() and SCC_DRV_MALLOC_ALIGN() defaults {1}

*/


/* =================================================================== */
/*                           include files                             */
/* =================================================================== */

#include "config.h"


#ifdef SSC_INTERFACE


/* =================================================================== */
/*                          local definitions                          */
/* =================================================================== */

#ifndef SSC_DRV_MALLOC                                                   // {1}
    #define SSC_DRV_MALLOC(x)          uMalloc((MAX_MALLOC)(x))
#endif
#ifndef SCC_DRV_MALLOC_ALIGN                                             // {1}
    #define SCC_DRV_MALLOC_ALIGN(x, y) uMallocAlign((MAX_MALLOC)(x), (y))
#endif

/* =================================================================== */
/*                       local structure definitions                   */
/* =================================================================== */


/* =================================================================== */
/*                 local function prototype declarations               */
/* =================================================================== */
static void send_next_word(QUEUE_HANDLE channel, SSCQUE *ptSSCQue);

/* =================================================================== */
/*                             constants                               */
/* =================================================================== */


/* =================================================================== */
/*                      local variable definitions                     */
/* =================================================================== */


static SSCQUE *tx_control[NUMBER_SSC];
static SSCQUE *rx_control[NUMBER_SSC] = {0};


/* =================================================================== */
/*                      local function definitions                     */
/* =================================================================== */


/* =================================================================== */
/*                      global function definitions                    */
/* =================================================================== */

// Standard entry call to driver - dispatches required sub-routine
//
static QUEUE_TRANSFER entry_ssc(QUEUE_HANDLE channel, unsigned char *ptBuffer, QUEUE_TRANSFER Counter, unsigned char ucCallType, QUEUE_HANDLE DriverID)
{
    SSCQUE *ptSSCQue;
    QUEUE_TRANSFER rtn_val = 0;

    uDisable_Interrupt();                                                // disable all interrupts

    switch ( ucCallType ) {
    case CALL_DRIVER:                                                    // request changes and return status
        if ((CAST_POINTER_ARITHMETIC)ptBuffer & MODIFY_TX) {
            ptSSCQue = (struct stSSCQue *)(que_ids[DriverID].output_buffer_control);// tx state
        }
        else {
            ptSSCQue = (struct stSSCQue *)(que_ids[DriverID].input_buffer_control); // rx state
        }

        if (Counter) {                                                   // modify driver state
                if (Counter & RX_ON) {
#ifdef SSC_SUPPORT_DMA
                    if (ptSSCQue->ucDMA_mode & UART_RX_DMA) {
                        fnPrepareSSCRxDMA(channel, ptSSCQue->ssc_queue.put, ptSSCQue->ucWordsPerFrame);
    #ifdef SSC_DMA_DOUBLE_BUF_RX
                        ptSSCQue->ssc_queue.put += (ptSSCQue->ucWordsPerFrame * ptSSCQue->ucBytesPerWord);
    #endif
                    }
                    else {
                        fnSSC_RxOn(channel);
                    }
#else
                    fnSSC_RxOn(channel);
#endif
                }
                else if (Counter & RX_OFF) {
                    fnSSC_RxOff(channel);
                }

                if (Counter & TX_ON) {
                    fnSSC_TxOn(channel);
                }
                else if (Counter & TX_OFF) {
                    fnSSC_TxOff(channel);
                }
                if (Counter & MODIFY_WAKEUP) {
                    ptSSCQue->wake_task = (UTASK_TASK)((CAST_POINTER_ARITHMETIC)ptBuffer & 0x7f);
                }
        }

        rtn_val = (QUEUE_TRANSFER)ptSSCQue->usOpn_mode;
        break;
#if !defined _NO_CHECK_QUEUE_INPUT
    case CALL_INPUT:                                                     // request the number or input characters waiting
        ptSSCQue = (struct stSSCQue *)(que_ids[DriverID].input_buffer_control); // set to input control block
        rtn_val = (ptSSCQue->ssc_queue.chars/ptSSCQue->ucBytesPerWord);
        break;
#endif
    case CALL_WRITE:                                                     // write data into the queue
                                                                         // copy the data to the output buffer and start transmission if not already done
        ptSSCQue = (struct stSSCQue *)(que_ids[DriverID].output_buffer_control); // set to output control block
        if (!ptBuffer) {                                                 // the caller wants to see whether the data will fit and not copy data so inform
#ifdef SSC_SUPPORT_DMA_
            if (ptSSCQue->ucDMA_mode & UART_TX_DMA) {
                QUEUE_TRANSFER reduction = (ptSSCQue->lastDMA_block_length - fnRemainingDMA_tx(channel)); // get the number of characters
                ptSSCQue->ssc_queue.chars -= reduction;
                ptSSCQue->lastDMA_block_length -= reduction;
                ptSSCQue->ssc_queue.put += reduction;
                if (ptSSCQue->ssc_queue.put >= ptSSCQue->ssc_queue.buffer_end) {
                    ptSSCQue->ssc_queue.put -= ptSSCQue->ssc_queue.buf_length;
                }
            }
#endif
            if ((ptSSCQue->ssc_queue.buf_length - ptSSCQue->ssc_queue.chars) >= Counter) {
                rtn_val = ptSSCQue->ssc_queue.buf_length - ptSSCQue->ssc_queue.chars; // the remaining space
            }
        }
        else {
            uEnable_Interrupt();                                         // fnFillBuffer disables and then re-enables interrupts - be sure we are compatible
            rtn_val = fnFillBuf(&ptSSCQue->ssc_queue, ptBuffer, (QUEUE_TRANSFER)(Counter * ptSSCQue->ucBytesPerWord));
            uDisable_Interrupt();
            if (!(ptSSCQue->ucState & (TX_WAIT | TX_ACTIVE))) {
                send_next_word(channel, ptSSCQue);                       // this is not done when the transmitter is already performing a transfer or if suspended
            }
            uEnable_Interrupt();
            return (rtn_val);
        }
        break;

    case CALL_READ:                                                      // read data from the queue
         ptSSCQue = (struct stSSCQue *)(que_ids[DriverID].input_buffer_control); // set to input control block
         rtn_val = fnGetBuf(&ptSSCQue->ssc_queue, ptBuffer, (QUEUE_TRANSFER)(Counter * ptSSCQue->ucBytesPerWord)); // interrupts are re-enabled as soon as no longer critical
         return (rtn_val/ptSSCQue->ucBytesPerWord);
    default:
       break;
  }

  uEnable_Interrupt();                                                   // enable interrupts
  return (rtn_val);
}

static SSCQUE *fnGetControlBlock(QUEUE_TRANSFER queue_size, unsigned char ucWordBytes)
{
    SSCQUE *ptSSCQue;

    if (NO_MEMORY == (ptSSCQue = (SSCQUE*)SSC_DRV_MALLOC(sizeof(struct stSSCQue)))) {
        return (0);                                                      // failed, no memory
    }
    queue_size *= ucWordBytes;
    if (NO_MEMORY == (ptSSCQue->ssc_queue.QUEbuffer = (unsigned char*)SCC_DRV_MALLOC_ALIGN(queue_size, ucWordBytes))) {
        return (0);                                                      // failed, no memory
    }
    ptSSCQue->ssc_queue.get = ptSSCQue->ssc_queue.put = ptSSCQue->ssc_queue.buffer_end = ptSSCQue->ssc_queue.QUEbuffer;
    ptSSCQue->ssc_queue.buffer_end += queue_size;
    ptSSCQue->ssc_queue.buf_length = queue_size;
    return (ptSSCQue);                                                   // malloc returns zeroed memory so other variables are zero initialised
}


extern QUEUE_HANDLE fnOpenSSC(SSCTABLE *pars, unsigned char driver_mode)
{
    QUEUE_HANDLE DriverID;
    IDINFO *ptrQueue;
    QUEUE_TRANSFER (*entry_add)(QUEUE_HANDLE channel, unsigned char *ptBuffer, QUEUE_TRANSFER Counter, unsigned char ucCallType, QUEUE_HANDLE DriverID) = entry_ssc;
    unsigned char ucWordBytes = ((pars->ucWordSize + 7) / 8);            // the number fo bytes required for each message word

    if (NO_ID_ALLOCATED != (DriverID = fnSearchID(entry_add, pars->Channel))) {
        if (!(driver_mode & MODIFY_CONFIG)) {
            return DriverID;                                             // channel already configured
        }
    }
    else if (NO_ID_ALLOCATED == (DriverID = fnSearchID(0, 0))) {         // get next free ID
        return (NO_ID_ALLOCATED);                                        // no free IDs available
    }

    ptrQueue = &que_ids[DriverID-1];
    ptrQueue->CallAddress = entry_add;

    if (driver_mode & FOR_WRITE) {                                       // define transmitter
        ptrQueue->output_buffer_control = (QUEQUE*)(tx_control[pars->Channel] = fnGetControlBlock(pars->Rx_tx_sizes.TxQueueSize, ucWordBytes));
    }

    if (driver_mode & FOR_READ) {                                        // define Receiver
        SSCQUE *ptSSCQue = fnGetControlBlock(pars->Rx_tx_sizes.RxQueueSize, ucWordBytes);
        ptrQueue->input_buffer_control = (QUEQUE*)(rx_control[pars->Channel] = ptSSCQue);
    }

    if (tx_control[pars->Channel] != 0) {
        tx_control[pars->Channel]->usOpn_mode = pars->usConfig;
    #ifdef SSC_SUPPORT_DMA
        tx_control[pars->Channel]->ucDMA_mode = pars->ucDMAConfig;
    #endif
        tx_control[pars->Channel]->ucBytesPerWord = ucWordBytes;
    }

    if (rx_control[pars->Channel] != 0) {
        rx_control[pars->Channel]->wake_task = pars->Task_to_wake;
        rx_control[pars->Channel]->usOpn_mode = pars->usConfig;
    #ifdef SSC_SUPPORT_DMA
        rx_control[pars->Channel]->ucDMA_mode = pars->ucDMAConfig;
    #endif
        rx_control[pars->Channel]->ucWordsPerFrame = pars->ucFrameLength;
        rx_control[pars->Channel]->ucBytesPerWord = ucWordBytes;
    }

    ptrQueue->qHandle = pars->Channel;
    fnConfigSSC(pars->Channel, pars);                                    // configure hardware for this channel

    return (DriverID);                                                   // return the allocated ID
}


static void send_next_word(QUEUE_HANDLE channel, SSCQUE *ptSSCQue)       // interrupts are assumed to be disabled here
{
#ifdef SSC_SUPPORT_DMA
    if (ptSSCQue->ucDMA_mode & UART_TX_DMA) {                            // DMA mode of operation
        ptSSCQue->ssc_queue.chars -= ptSSCQue->lastDMA_block_length;     // the last block has been transmitted so this space is available again for additional characters
        ptSSCQue->lastDMA_block_length = 0;
    }
#endif
    if (!(ptSSCQue->ucState & TX_WAIT)) {                                // send the next byte if possible - either first char or tx interrupt
        if (!ptSSCQue->ssc_queue.chars) {                                // are there more to send?
            ptSSCQue->ucState &= ~TX_ACTIVE;                             // transmission of a block has terminated
            fnClearSSCTxInt(channel);                                    // clear interrupt
        }
        else {
            unsigned long ulNextWord = 0;
            int iBytes = ptSSCQue->ucBytesPerWord;
#ifdef SSC_SUPPORT_DMA
            if (ptSSCQue->ucDMA_mode & UART_TX_DMA) {                    // DMA mode of operation
                QUEUE_TRANSFER tx_length;
                                                                         // calculate whether we can send block in one go or not
                if ((ptSSCQue->ssc_queue.get + ptSSCQue->ssc_queue.chars) >= ptSSCQue->ssc_queue.buffer_end) {
                    tx_length = (QUEUE_TRANSFER)(ptSSCQue->ssc_queue.buffer_end - ptSSCQue->ssc_queue.get); // single transfer up to the end of the buffer
                }
                else {
                    tx_length = ptSSCQue->ssc_queue.chars;               // single transfer block
                }
                if (!tx_length) {
                    return;
                }
                ptSSCQue->ucState |= TX_ACTIVE;                          // mark activity
                tx_length /= iBytes;                                     // the number of native words to be transmitted
                tx_length = fnTxSCCWordDMA(channel, ptSSCQue->ssc_queue.get, tx_length); // initiate buffer transfer using DMA
                tx_length *= iBytes;
                if ((ptSSCQue->ssc_queue.get += tx_length) >= ptSSCQue->ssc_queue.buffer_end) { // assume that transmission will be successful
                    ptSSCQue->ssc_queue.get = ptSSCQue->ssc_queue.QUEbuffer;
                }
                ptSSCQue->lastDMA_block_length = tx_length;              // save the block length for removal after complete tramsmission
                return;
            }
#endif
            switch (iBytes) {
            case 1:
                ulNextWord = *(ptSSCQue->ssc_queue.get);
                break;
            case 2:
                ulNextWord = *(unsigned short *)(ptSSCQue->ssc_queue.get);
                break;
            case 4:
                ulNextWord = *(unsigned long *)(ptSSCQue->ssc_queue.get);
                break;
            }     
            fnTxSSCword(channel, ulNextWord);
            ptSSCQue->ucState |= TX_ACTIVE;                              // mark activity
            ptSSCQue->ssc_queue.get += iBytes;
            if (ptSSCQue->ssc_queue.get >= ptSSCQue->ssc_queue.buffer_end) { // wrap in circular buffer
                ptSSCQue->ssc_queue.get -= ptSSCQue->ssc_queue.buf_length; // wrap to beginning
            }
            ptSSCQue->ssc_queue.chars -= iBytes;
        }
    }
    else {
        fnClearSSCTxInt(channel);                                        // clear interrupt since we are not allowed to send at the moment
    }
}

/* =================================================================== */
/*                         interrupt  interface routines               */
/* =================================================================== */

// This is called by the interrupt routine to put one received word into the input buffer, or when a frame has been received in DMA mode
//
extern void fnSSCRxWord( unsigned long ulWord, QUEUE_HANDLE Channel )
{
    SSCQUE *rx_ctl = rx_control[Channel];
#ifdef SSC_SUPPORT_DMA
    if (rx_ctl->ucDMA_mode & UART_RX_DMA) {                              // new characters in the buffer - increment message count
        QUEUE_TRANSFER transfer_length = (rx_ctl->ucWordsPerFrame * rx_ctl->ucBytesPerWord);
        rx_ctl->ssc_queue.put += transfer_length;
        if (rx_ctl->ssc_queue.put >= rx_ctl->ssc_queue.buffer_end) {
            rx_ctl->ssc_queue.put -= rx_ctl->ssc_queue.buf_length;
        }
        fnPrepareSSCRxDMA(Channel, rx_ctl->ssc_queue.put, (QUEUE_TRANSFER)(transfer_length/rx_ctl->ucBytesPerWord)); // enable next reception
        rx_ctl->ssc_queue.chars += transfer_length;                      // the number of words in the input buffer
        if ( rx_ctl->wake_task ) {                                       // wake up an input task, if defined
            uTaskerStateChange(rx_ctl->wake_task, UTASKER_ACTIVATE);     // wake up service interface task
        }
        return;
    }
#endif
    // interrupt reception of individual words could be added here
}


// The tx interrupt routine calls this
//
extern void fnSSCTxWord(QUEUE_HANDLE Channel)
{
    send_next_word(Channel, tx_control[Channel]);
}

#endif
