/***********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher

    ---------------------------------------------------------------------
    File:      tty_drv.c
    Project:   Single Chip Embedded Internet
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2016
    *********************************************************************
    22.02.2007 WAKE_BLOCKED_TX allow TX_FREE on char <= level (was < level) {1}. Remove check on tx done {2}
    17.03.2007 Add check fnWakeBlockedTx when transmission complete for DMA mode {3}
    13.05.2007 Add buffer scan in forward direction                      {4}
    29.10.2007 Correct variable name in SUPPORT_HW_FLOW code             {5}
    02.11.2007 Synchronise CTS state to buffer control in RTS/CTS mode   {6}
    02.11.2007 Correct transmission of XOFF when not in correct mode     {7}
    02.11.2007 Set active flag when sending XON/XOFF in order to protect tx buffer during its transmission {8}
    26.01.2008 Allow WAKE_BLOCKED_TX operation with fixed buffer level   {9}
    23.05.2008 Add message counter mode with optional short word length count {10}
    23.05.2008 Extend break support to include Rx DMA and message counter{11}
    23.05.2008 Improve message extraction in half-buffer DMA mode        {12}
    30.08.2008 Don't put XON to input buffer, even when not in XOFF mode {13}
    31.10.2008 Add inter-character timer support for use by MODBUS       {14}
    27.12.2008 Correct DMA transmission counter                          {15}
    26.02.2009 Add call on frame transmission termination                {16}
    24.03.2009 Adapt to limit transmission to DMA capabilities fnTxByteDMA() {17}
    10.04.2009 Optimise DMA operating in XON/XOFF and RTS/CTS flow control modes {18}
    08.05.2009 Add _NO_CHECK_QUEUE_INPUT to aid code size optimisation   {19}
    10.06.2009 Rename usConfig to Config in UART configuration           {20}
    10.06.2009 Pass operating mode to fnControlLine() rather than DCE_MODE {21}
    11.09.2009 Add MULTIDROP_MODE support                                {22}
    15.10.2009 Allow WAKE_BLOCKED_TX operation without SUPPORT_FLOW_HIGH_LOW and use tx control structure {23}
    16.10.2009 Add NUMBER_EXTERNAL_SERIAL                                {24}
    11.06.2009 Add MULTIDROP_MODE reception support                      {25}
    22.06.2011 Support reading waiting messages in message break mode    {26}
    21.10.2011 DMA support conditional part of break support             {27}
    04.06.2013 Added TTY_DRV_MALLOC() default                            {28}
    05.08.2013 Add SUPPORT_MSG_MODE_EXTRACT to allow message content to be extracted as individual bytes {29}
    06.08.2013 Add DMA reception length to reception character count     {30}
    06.08.2013 Only release transmissions on CTS assertion when originally blocked {31}
    06.08.2013 Add SERIAL_SUPPORT_DMA_RX_FREERUN support                 {32}
    06.08.2013 Allow fnMsgs() to work with free-running DMA              {33}

*/


/* =================================================================== */
/*                           include files                             */
/* =================================================================== */

#include "config.h"


#if defined SERIAL_INTERFACE


#if defined SERIAL_SUPPORT_XON_XOFF
    #define SUPPORT_FLOW_CONTROL
#endif
#if defined SUPPORT_HW_FLOW
    #define SUPPORT_FLOW_CONTROL
#endif


/* =================================================================== */
/*                          local definitions                          */
/* =================================================================== */

#if !defined TTY_DRV_MALLOC                                                       // {28}
    #define TTY_DRV_MALLOC(x)    uMalloc((MAX_MALLOC)(x))
#endif

/* =================================================================== */
/*                       local structure definitions                   */
/* =================================================================== */


/* =================================================================== */
/*                 local function prototype declarations               */
/* =================================================================== */
static void send_next_byte(QUEUE_HANDLE channel, TTYQUE *ptTTYQue);

/* =================================================================== */
/*                             constants                               */
/* =================================================================== */


/* =================================================================== */
/*                      local variable definitions                     */
/* =================================================================== */


static TTYQUE *tx_control[NUMBER_SERIAL + NUMBER_EXTERNAL_SERIAL] = {0}; // {24}
static TTYQUE *rx_control[NUMBER_SERIAL + NUMBER_EXTERNAL_SERIAL] = {0};


/* =================================================================== */
/*                      local function definitions                     */
/* =================================================================== */


/* =================================================================== */
/*                      global function definitions                    */
/* =================================================================== */

// Standard entry call to driver - dispatches required sub-routine
//
static QUEUE_TRANSFER entry_tty(QUEUE_HANDLE channel, unsigned char *ptBuffer, QUEUE_TRANSFER Counter, unsigned char ucCallType, QUEUE_HANDLE DriverID)
{
    TTYQUE *ptTTYQue;
    QUEUE_TRANSFER rtn_val = 0;

    uDisable_Interrupt();                                                // disable all interrupts

    switch (ucCallType) {
    case CALL_DRIVER:                                                    // request changes and return status
        if ((CAST_POINTER_ARITHMETIC)ptBuffer & MODIFY_TX) {
            ptTTYQue = (struct stTTYQue *)(que_ids[DriverID].output_buffer_control);// tx state
        }
        else {
            ptTTYQue = (struct stTTYQue *)(que_ids[DriverID].input_buffer_control); // rx state
        }

        if (Counter != 0) {                                              // modify driver state
#if defined SUPPORT_HW_FLOW                                              // {6}
            if (Counter & MODIFY_CONTROL) {                              // control signals
                fnControlLine(channel, (unsigned short)Counter, ptTTYQue->opn_mode);// pass on to hardware
            }
            else if (MODIFY_INTERRUPT & Counter) {
                rtn_val = fnControlLineInterrupt(channel, (unsigned short)Counter, ptTTYQue->opn_mode); // pass on to hardware
                fnRTS_change(channel, ((rtn_val & SET_CTS) != 0));       // synchronise control with buffer control
                break;
            }
            else {
#endif
                if ((Counter & RX_ON) != 0) {
#if defined SERIAL_SUPPORT_DMA && defined SERIAL_SUPPORT_DMA_RX
                    if ((ptTTYQue->ucDMA_mode & UART_RX_DMA) != 0) {
                        QUEUE_TRANSFER transfer_length = ptTTYQue->tty_queue.buf_length;
                        if (ptTTYQue->ucDMA_mode & UART_RX_DMA_HALF_BUFFER) {
                            transfer_length /= 2;
                        }
                        if (ptTTYQue->opn_mode & MSG_MODE_RX_CNT) {
    #if defined MSG_CNT_WORD                                             // {10}
                            transfer_length -= 2;
    #else
                            transfer_length -= 1;
    #endif
                        }
                        ptTTYQue->msgchars = transfer_length;
                        fnPrepareRxDMA(channel, ptTTYQue->tty_queue.put, transfer_length);
                    }
                    else {
                        fnRxOn(channel);
                    }
#else
                    fnRxOn(channel);
#endif
                }
                else if ((Counter & RX_OFF) != 0) {
                    fnRxOff(channel);
                }

                if ((Counter & TX_ON) != 0) {
                    fnTxOn(channel);
                }
                else if ((Counter & TX_OFF) != 0) {
                    fnTxOff(channel);
                }
#if defined SERIAL_SUPPORT_ECHO
                if ((Counter & ECHO_ON) != 0) {
                    ptTTYQue->opn_mode |= ECHO_RX_CHARS;
                }
                else if ((Counter & ECHO_OFF) != 0) {
                    ptTTYQue->opn_mode &= ~ECHO_RX_CHARS;
                }
#endif
#if defined (SUPPORT_MSG_CNT) && defined (SUPPORT_MSG_MODE)
                if ((Counter & SET_MSG_CNT_MODE) != 0) {
                    ptTTYQue->ucMessageTerminator = (unsigned char)((CAST_POINTER_ARITHMETIC)ptBuffer & 0xff);
                    ptTTYQue->opn_mode |= (MSG_MODE | MSG_MODE_RX_CNT);
                }
                else if (Counter & SET_CHAR_MODE) {
                    ptTTYQue->opn_mode &= ~(MSG_MODE | MSG_MODE_RX_CNT);
                }
#endif

                if ((Counter & MODIFY_WAKEUP) != 0) {
                    ptTTYQue->wake_task = (UTASK_TASK)((CAST_POINTER_ARITHMETIC)ptBuffer & 0x7f);
                }
#if defined SUPPORT_HW_FLOW
            }
#endif
        }
        rtn_val = (QUEUE_TRANSFER)ptTTYQue->opn_mode;
        break;
#if !defined _NO_CHECK_QUEUE_INPUT                                       // {19}
    case CALL_INPUT:                                                     // request the number or input characters waiting
        ptTTYQue = (struct stTTYQue *)(que_ids[DriverID].input_buffer_control); // set to input control block
        if (ptTTYQue->opn_mode & (MSG_MODE | MSG_BREAK_MODE)) {          // {26}
    #if defined (SUPPORT_MSG_CNT)
            if (ptTTYQue->opn_mode & MSG_MODE_RX_CNT) {
                rtn_val = (ptTTYQue->msgs + 1)/2;                        // in count mode we count the count and the actual message
            }
            else {
                rtn_val = ptTTYQue->msgs;
            }
    #else
            rtn_val = ptTTYQue->msgs;
    #endif
        }
        else {
    #if defined SERIAL_SUPPORT_DMA && defined SERIAL_SUPPORT_DMA_RX && defined SERIAL_SUPPORT_DMA_RX_FREERUN  // {33}
        if ((ptTTYQue->ucDMA_mode & (UART_RX_DMA | UART_RX_DMA_FULL_BUFFER | UART_RX_DMA_HALF_BUFFER | UART_RX_DMA_BREAK)) == (UART_RX_DMA)) { // if receiver is free-running in DMA mode
            fnPrepareRxDMA(channel, (unsigned char *)&(ptTTYQue->tty_queue), 0); // update the input with present DMA reception information
        }
    #endif
            rtn_val = ptTTYQue->tty_queue.chars;
        }
        break;
#endif
    case CALL_WRITE:                                                     // write data into the queue
                                                                         // copy the data to the output buffer and start transmission if not already done
        ptTTYQue = (struct stTTYQue *)(que_ids[DriverID].output_buffer_control); // set to output control block
        if (ptBuffer == 0) {                                             // the caller wants to see whether the data will fit and not copy data so inform
#if defined SERIAL_SUPPORT_DMA_                                          // {18}
            if (ptTTYQue->ucDMA_mode & UART_TX_DMA) {
                QUEUE_TRANSFER reduction = (ptTTYQue->lastDMA_block_length - fnRemainingDMA_tx(channel)); // get the number of characters
                ptTTYQue->tty_queue.chars -= reduction;
                ptTTYQue->lastDMA_block_length -= reduction;
                ptTTYQue->tty_queue.put += reduction;
                if (ptTTYQue->tty_queue.put >= ptTTYQue->tty_queue.buffer_end) {
                    ptTTYQue->tty_queue.put -= ptTTYQue->tty_queue.buf_length;
                }
            }
#endif
            if ((ptTTYQue->tty_queue.buf_length - ptTTYQue->tty_queue.chars) >= Counter) {
                rtn_val = ptTTYQue->tty_queue.buf_length - ptTTYQue->tty_queue.chars; // the remaining space
            }
        }
        else {
            uEnable_Interrupt();                                         // fnFillBuffer disables and then re-enables interrupts - be sure we are compatible
            rtn_val = fnFillBuf(&ptTTYQue->tty_queue, ptBuffer, Counter);
            uDisable_Interrupt();
            if ((ptTTYQue->ucState & (TX_WAIT | TX_ACTIVE)) == 0) {
                send_next_byte(channel, ptTTYQue);                       // this is not done when the transmitter is already performing a transfer or if suspended
            }
            uEnable_Interrupt();
            return (rtn_val);
        }
        break;

    case CALL_READ:                                                      // read data from the queue
        ptTTYQue = (struct stTTYQue *)(que_ids[DriverID].input_buffer_control); // set to input control block
#if defined (SUPPORT_MSG_MODE) || defined (SUPPORT_MSG_CNT) || defined (UART_BREAK_SUPPORT)
        if (ptTTYQue->opn_mode & (MSG_MODE | MSG_MODE_RX_CNT | MSG_BREAK_MODE)) {
    #if defined SUPPORT_MSG_MODE_EXTRACT                                 // {29}
            if (Counter == 0) {
                Counter = 1;
            }
            else {
    #endif
                if (!ptTTYQue->msgs) {
                    uEnable_Interrupt();                                 // enable interrupts
                    return rtn_val;
                }
                --ptTTYQue->msgs;                                        // delete this message (or its length)
    #if defined SUPPORT_MSG_MODE_EXTRACT
            }
    #endif
        }
#endif
#if defined SERIAL_SUPPORT_DMA && defined SERIAL_SUPPORT_DMA_RX_FREERUN  // {32}
        if ((ptTTYQue->ucDMA_mode & (UART_RX_DMA | UART_RX_DMA_FULL_BUFFER | UART_RX_DMA_HALF_BUFFER | UART_RX_DMA_BREAK)) == (UART_RX_DMA)) { // if receiver is free-running in DMA mode
            fnPrepareRxDMA(channel, (unsigned char *)&(ptTTYQue->tty_queue), 0); // update the input with present DMA reception information

    #if defined SUPPORT_FLOW_CONTROL && defined SUPPORT_HW_FLOW          // handle CTS control when the buffer is critical
            if ((ptTTYQue->opn_mode & RTS_CTS) && (!(ptTTYQue->ucState & RX_HIGHWATER)) // RTS/CTS for receiver
        #if defined SUPPORT_FLOW_HIGH_LOW
            && ((ptTTYQue->tty_queue.chars >= ptTTYQue->high_water_level)))
        #else
            && ((ptTTYQue->tty_queue.chars > (ptTTYQue->tty_queue.buf_length - HIGH_WATER_MARK))))
        #endif
            {
                fnControlLine(channel, CLEAR_CTS, ((struct stTTYQue *)(que_ids[DriverID].output_buffer_control))->opn_mode); // remove CTS as quickly as possible
                ptTTYQue->ucState |= RX_HIGHWATER;                       // mark that we have requested that the transmitter stop sending to us
        #if defined SERIAL_STATS
                ptTTYQue->ulSerialCounter[SERIAL_STATS_FLOW]++;          // count the number of times we stall the flow
        #endif
            }
    #endif
        }
#endif
        rtn_val = fnGetBuf(&ptTTYQue->tty_queue, ptBuffer, Counter);     // interrupts are re-enabled as soon as no longer critical

#if defined SERIAL_SUPPORT_DMA                                           // {12}
        if ((ptTTYQue->ucDMA_mode & UART_RX_DMA_HALF_BUFFER) && (!(ptTTYQue->msgs & 0x1))) { // complete message extracted, set to next half buffer
            if (ptTTYQue->tty_queue.get < ptTTYQue->tty_queue.QUEbuffer + (ptTTYQue->tty_queue.buf_length/2)) {
                ptTTYQue->tty_queue.get = ptTTYQue->tty_queue.QUEbuffer + (ptTTYQue->tty_queue.buf_length/2);
            }
            else {
                ptTTYQue->tty_queue.get = ptTTYQue->tty_queue.QUEbuffer;
            }
        }
#endif

#if defined SUPPORT_FLOW_CONTROL
        if (((ptTTYQue->opn_mode & (USE_XON_OFF | RTS_CTS)) && (ptTTYQue->ucState & RX_HIGHWATER)) // flow control support for receiver
    #if defined SUPPORT_FLOW_HIGH_LOW
        && ((ptTTYQue->tty_queue.chars <= ptTTYQue->low_water_level)))
    #else
        && ((ptTTYQue->tty_queue.chars < LOW_WATER_MARK)))
    #endif
        {
            ptTTYQue->ucState &= ~RX_HIGHWATER;
            if (ptTTYQue->opn_mode & USE_XON_OFF) {
                ptTTYQue = (struct stTTYQue *)(que_ids[DriverID].output_buffer_control); // set pointer to output control
                ptTTYQue->ucState |= SEND_XON;
                send_next_byte(channel, ptTTYQue);                       // send XON
            }
    #if defined SUPPORT_HW_FLOW
            else {
                fnControlLine(channel, SET_CTS, ptTTYQue->opn_mode);    // {21} activate CTS again
            }
    #endif
        }
#endif
        return rtn_val;

#if defined SERIAL_SUPPORT_SCAN
    case CALL_SCAN:                                                      // scan characters in input buffer for a defined sequence
    {
    #if defined SERIAL_SCAN_FORWARD
        unsigned char *ptrCheck = ptBuffer;
    #endif
        unsigned char *ptrInBuffer;
        unsigned char ucMatch = 0;
        ptTTYQue = (struct stTTYQue *)(que_ids[DriverID].input_buffer_control); // set to input control block
        rtn_val = ptTTYQue->tty_queue.chars;                             // the number of characters presently in the input buffer

        uEnable_Interrupt();                                             // don't block interrupts any longer
    #if defined SERIAL_SCAN_FORWARD                                      // {4}
        ptrInBuffer = ptTTYQue->tty_queue.get;
        while (rtn_val != 0) {
            if (*ptrInBuffer == *ptrCheck) {
                if (++ucMatch >= Counter) {                              // if match length correct, quit
                    break;
                }
                ptrCheck++;
            }
            else {
                ptrCheck = ptBuffer;                                     // start again
            }
            if (++ptrInBuffer >= ptTTYQue->tty_queue.buffer_end) {       // wrap in circular buffer
                ptrInBuffer = ptTTYQue->tty_queue.QUEbuffer;             // wrap to beginning
            }
            rtn_val--;
        }
    #else
        ptrInBuffer = ptTTYQue->tty_queue.put;
        do {
            if (--ptrInBuffer < ptTTYQue->tty_queue.QUEbuffer) {         // wrap in circular buffer
                ptrInBuffer = (ptTTYQue->tty_queue.buffer_end-1);        // wrap to end
            }
            if (*ptrInBuffer == *ptBuffer) {
                if (++ucMatch >= Counter) {
                    break;
                }
                ++ptBuffer;
            }
            else {
                break;
            }
        } while (rtn_val--);
    #endif

        return (ucMatch);
    }
#endif

#if defined SERIAL_STATS
    case CALL_STATS:                                                     // return a driver statistic value
    {
        unsigned char *ptr;
        int i;
        if (Counter >= SERIAL_COUNTERS) {
            ptTTYQue = (struct stTTYQue *)(que_ids[DriverID].output_buffer_control); // set to output control block
            Counter -= SERIAL_COUNTERS;
        }
        else {
            ptTTYQue = (struct stTTYQue *)(que_ids[DriverID].input_buffer_control); // set to input control block
        }
        if (ptBuffer == 0) {                                             // reset a counter rather than obtain it
            if (Counter >= SERIAL_COUNTERS) {
                for (i = 0; i < SERIAL_COUNTERS; i++) {
                    ptTTYQue->ulSerialCounter[i] = 0;                    // delete all output counters
                }
                ptTTYQue = (struct stTTYQue *)(que_ids[DriverID].input_buffer_control);  // set to input control block
                for (i = 0; i < SERIAL_COUNTERS; i++) {
                    ptTTYQue->ulSerialCounter[i] = 0;                    // delete all input counters
                }
            }
            else {
                ptTTYQue->ulSerialCounter[Counter] = 0;                  // reset specific counter
            }
        }
        else {
            ptr = (unsigned char *)&ptTTYQue->ulSerialCounter[Counter];
            i = sizeof(ptTTYQue->ulSerialCounter[0]);
            while (i--) {
                *ptBuffer++ = *ptr++;
            }
        }
    }
    break;
#endif

#if defined SUPPORT_FLUSH
    case CALL_FLUSH:                                                     // flush input or output queue completely
        if (Counter != FLUSH_RX) {                                       // tx
            ptTTYQue = (struct stTTYQue *)(que_ids[DriverID].output_buffer_control); // set to output control block
        }
        else {                                                           // rx
            ptTTYQue = (struct stTTYQue *)(que_ids[DriverID].input_buffer_control);  // set to input control block
    #if defined SUPPORT_FLOW_CONTROL
            if ((ptTTYQue->ucState & RX_HIGHWATER) != 0) {               // XOFF has been sent so send XON
                ptTTYQue->ucState &= ~RX_HIGHWATER;
                ptTTYQue = (struct stTTYQue *)(que_ids[DriverID].output_buffer_control); // set pointer to output control
                if ((ptTTYQue->opn_mode & USE_XON_OFF) != 0) {
                    ptTTYQue->ucState |= SEND_XON;                       // send XON to unblock after flushing
                    send_next_byte(channel, ptTTYQue);
                }
        #if defined SUPPORT_HW_FLOW
                else {
                    fnControlLine(channel, SET_CTS, ptTTYQue->opn_mode); // {21} activate CTS again
                }
        #endif
                ptTTYQue = (struct stTTYQue *)(que_ids[DriverID].input_buffer_control); // set (again) to input control block
            }
    #endif
        }
        ptTTYQue->tty_queue.get = ptTTYQue->tty_queue.put = ptTTYQue->tty_queue.QUEbuffer;
        ptTTYQue->msgs = ptTTYQue->tty_queue.chars = 0;

    #if defined SUPPORT_MSG_CNT
        if ((Counter == FLUSH_RX) && ((ptTTYQue->opn_mode & MSG_MODE_RX_CNT) != 0)) {
          //*(ptTTYQue->tty_queue.put) = 0;                              // reserve space for length in next message
            (ptTTYQue->tty_queue.put)++;
        #if defined MSG_CNT_WORD                                         // {10}
            (ptTTYQue->tty_queue.put)++;
        #endif
        #if defined SUPPORT_MSG_MODE
            ptTTYQue->msgchars = 0;
        #endif
            ptTTYQue->tty_queue.chars = 1;
        }
    #endif
        break;
#endif

    default:
       break;
    }
    uEnable_Interrupt();                                                  // enable interrupts
    return (rtn_val);
}

static TTYQUE *fnGetControlBlock(QUEUE_TRANSFER queue_size)
{
    TTYQUE *ptTTYQue;

    if (NO_MEMORY == (ptTTYQue = (TTYQUE*)TTY_DRV_MALLOC(sizeof(struct stTTYQue)))) {
        return (0);                                                      // failed, no memory
    }

    if (NO_MEMORY == (ptTTYQue->tty_queue.QUEbuffer = (unsigned char*)TTY_DRV_MALLOC(queue_size))) {
        return (0);                                                      // failed, no memory
    }
    ptTTYQue->tty_queue.get = ptTTYQue->tty_queue.put = ptTTYQue->tty_queue.buffer_end = ptTTYQue->tty_queue.QUEbuffer;
    ptTTYQue->tty_queue.buffer_end += queue_size;
    ptTTYQue->tty_queue.buf_length = queue_size;
    return (ptTTYQue);                                                   // malloc returns zeroed memory so other variables are zero initialised
}


extern QUEUE_HANDLE fnOpenTTY(TTYTABLE *pars, unsigned char driver_mode)
{
    QUEUE_HANDLE DriverID;
    IDINFO *ptrQueue;
    QUEUE_TRANSFER (*entry_add)(QUEUE_HANDLE channel, unsigned char *ptBuffer, QUEUE_TRANSFER Counter, unsigned char ucCallType, QUEUE_HANDLE DriverID) = entry_tty;

    if (NO_ID_ALLOCATED != (DriverID = fnSearchID (entry_add, pars->Channel))) {
        if ((driver_mode & MODIFY_CONFIG) == 0) {
            return DriverID;                                             // channel already configured
        }
    }
    else if (NO_ID_ALLOCATED == (DriverID = fnSearchID (0, 0))) {        // get next free ID
        return (NO_ID_ALLOCATED);                                        // no free IDs available
    }

    ptrQueue = &que_ids[DriverID-1];
    ptrQueue->CallAddress = entry_add;

    if (driver_mode & FOR_WRITE) {                                       // define transmitter
        ptrQueue->output_buffer_control = (QUEQUE *)(tx_control[pars->Channel] = fnGetControlBlock(pars->Rx_tx_sizes.TxQueueSize));
    }

    if (driver_mode & FOR_READ) {                                        // define receiver
        TTYQUE *ptTTYQue = fnGetControlBlock(pars->Rx_tx_sizes.RxQueueSize);
        ptrQueue->input_buffer_control = (QUEQUE *)(rx_control[pars->Channel] = ptTTYQue);
#if defined SUPPORT_MSG_CNT
        if (pars->Config & MSG_MODE_RX_CNT) {                            // {20}
    #if defined MSG_CNT_WORD
            ptTTYQue->tty_queue.put += 2;
            ptTTYQue->tty_queue.chars = 2;                               // reserve for length of first message
    #else
            ++ptTTYQue->tty_queue.put;
            ptTTYQue->tty_queue.chars = 1;                               // reserve for length of first message
    #endif
        }
#endif
    }

    if (tx_control[pars->Channel] != 0) {
        tx_control[pars->Channel]->opn_mode = pars->Config;              // {20}
#if defined SERIAL_SUPPORT_DMA
        tx_control[pars->Channel]->ucDMA_mode = pars->ucDMAConfig;
#endif
#if defined WAKE_BLOCKED_TX
    #if defined WAKE_BLOCKED_TX_BUFFER_LEVEL                             // {23}
        tx_control[pars->Channel]->low_water_level  = pars->tx_wake_level; // set specified wake up level
    #elif defined SUPPORT_FLOW_HIGH_LOW
        tx_control[pars->Channel]->low_water_level  = ((pars->ucFlowLowWater * pars->Rx_tx_sizes.TxQueueSize)/100);   // save the number of byte converted from % of buffer size
    #endif
#endif
    }

    if (rx_control[pars->Channel] != 0) {
        rx_control[pars->Channel]->wake_task = pars->Task_to_wake;
#if defined SUPPORT_FLOW_HIGH_LOW
        rx_control[pars->Channel]->high_water_level = ((pars->ucFlowHighWater * pars->Rx_tx_sizes.RxQueueSize)/100);  // save the number of byte converted from % of buffer size
        rx_control[pars->Channel]->low_water_level  = ((pars->ucFlowLowWater * pars->Rx_tx_sizes.RxQueueSize)/100);   // save the number of byte converted from % of buffer size
#endif
        rx_control[pars->Channel]->opn_mode = pars->Config;              // {20}
#if defined SERIAL_SUPPORT_DMA
        rx_control[pars->Channel]->ucDMA_mode = pars->ucDMAConfig;
#endif
#if defined SUPPORT_MSG_MODE
        rx_control[pars->Channel]->ucMessageTerminator = pars->ucMessageTerminator;
#endif
#if defined SERIAL_SUPPORT_ESCAPE
        rx_control[pars->Channel]->ucMessageFilter = pars->ucMessageFilter;
#endif
    }

    ptrQueue->qHandle = pars->Channel;
    fnConfigSCI(pars->Channel, pars);                                    // configure hardware for this channel
    return (DriverID);                                                   // return the allocated ID
}

#if defined WAKE_BLOCKED_TX
static void fnWakeBlockedTx(TTYQUE *ptTTYQue, QUEUE_TRANSFER low_water_level)
{
    if ((ptTTYQue->wake_task != 0) && (ptTTYQue->tty_queue.chars <= low_water_level)) { // {1} we have just reduced the buffer content adequately so inform a blocked task that it can continue writing
        unsigned char tx_continue_message[ HEADER_LENGTH ]; // = { INTERNAL_ROUTE, INTERNAL_ROUTE , ptTTYQue->wake_task, INTERRUPT_EVENT, TX_FREE };  // define standard interrupt event
        tx_continue_message[MSG_DESTINATION_NODE] = INTERNAL_ROUTE;
        tx_continue_message[MSG_SOURCE_NODE]      = INTERNAL_ROUTE;
        tx_continue_message[MSG_DESTINATION_TASK] = ptTTYQue->wake_task;
        tx_continue_message[MSG_SOURCE_TASK]      = INTERRUPT_EVENT;
        tx_continue_message[MSG_INTERRUPT_EVENT]  = TX_FREE;

        fnWrite(INTERNAL_ROUTE, (unsigned char*)tx_continue_message, HEADER_LENGTH); // inform the blocked task
        ptTTYQue->wake_task = 0;                                         // remove task since this is only performed once
    }
}
#endif

#if defined SERIAL_SUPPORT_DMA && (defined SERIAL_SUPPORT_XON_XOFF || defined SUPPORT_HW_FLOW) // {18}
static void fnAbortDMA_transmission(QUEUE_HANDLE Channel, TTYQUE *tx_ctl)
{
    QUEUE_TRANSFER aborted_length = fnAbortTxDMA(Channel, &tx_ctl->tty_queue); // stop DMA transmission
    tx_ctl->tty_queue.chars -= (tx_ctl->lastDMA_block_length - aborted_length);
    tx_ctl->tty_queue.get -= aborted_length;
    if (tx_ctl->tty_queue.get < tx_ctl->tty_queue.QUEbuffer) {
        tx_ctl->tty_queue.get += tx_ctl->tty_queue.buf_length;
    }
    tx_ctl->lastDMA_block_length = 0;
}
#endif

static void send_next_byte(QUEUE_HANDLE channel, TTYQUE *ptTTYQue)       // interrupts are assumed to be disabled here
{
#if defined SERIAL_SUPPORT_DMA                                           // {15}
    if ((ptTTYQue->ucDMA_mode & UART_TX_DMA) != 0) {                     // DMA mode of operation
        ptTTYQue->tty_queue.chars -= ptTTYQue->lastDMA_block_length;     // the last block has been transmitted so this space is available again for additional characters
        ptTTYQue->lastDMA_block_length = 0;
    }
#endif
#if defined SERIAL_SUPPORT_XON_XOFF                                      // support for XON XOFF at receiver
    if ((ptTTYQue->ucState & (SEND_XON | SEND_XOFF)) != 0) {             // if receiver requires this to be sent by the transmitter
        unsigned char ucBusy;
        if ((ptTTYQue->ucState & SEND_XON) != 0) {
            ucBusy = fnTxByte(channel, XON_CODE);
        }
        else {
            ucBusy = fnTxByte(channel, XOFF_CODE);
        }
        if (ucBusy == 0) {
            ptTTYQue->ucState |= TX_ACTIVE;                              // set active flag during XON/XOFF transmission phase {8}
            ptTTYQue->ucState &= ~(SEND_XON | SEND_XOFF);
        }
        return;
    }
#endif

    if ((ptTTYQue->ucState & TX_WAIT) == 0) {                            // send the next byte if possible - either first char or tx interrupt
#if defined SERIAL_SUPPORT_ESCAPE
        int iBlock = 0;
#endif

        if (ptTTYQue->tty_queue.chars == 0) {                            // are there more to send?
            ptTTYQue->ucState &= ~TX_ACTIVE;                             // transmission of a block has terminated
            fnClearTxInt(channel);                                       // clear interrupt
#if defined (WAKE_BLOCKED_TX) && defined (SERIAL_SUPPORT_DMA)            // {2},{3}
            fnWakeBlockedTx(ptTTYQue, 0);
#endif
#if defined UART_BREAK_SUPPORT
            if ((ptTTYQue->opn_mode & BREAK_AFTER_TX) != 0) {
                fnStartBreak(channel);
            }
#endif
#if defined UART_FRAME_COMPLETE                                          // {16}
            if ((ptTTYQue->opn_mode & INFORM_ON_FRAME_TRANSMISSION) != 0) {
                fnUARTFrameTermination(channel);
            }
#endif
        }
        else {
            unsigned char ucNextByte = *(ptTTYQue->tty_queue.get);

#if defined SERIAL_SUPPORT_ESCAPE
            if ((ptTTYQue->opn_mode & TX_ESC_MODE) != 0) {
                if (ptTTYQue->ucState & ESCAPE_SEQUENCE) {
                    ptTTYQue->ucState &= ~ESCAPE_SEQUENCE;
                }
                else if ((ucNextByte == ptTTYQue->ucMessageFilter) || ((ucNextByte == ptTTYQue->ucMessageTerminator) && (ptTTYQue->tty_queue.chars != 1))) {
                    ptTTYQue->ucState |= ESCAPE_SEQUENCE;
                    ucNextByte = ptTTYQue->ucMessageFilter;
                    iBlock = 1;
                }
            }
#endif
#if defined SERIAL_SUPPORT_DMA
            if ((ptTTYQue->ucDMA_mode & UART_TX_DMA) != 0) {             // DMA mode of operation
                QUEUE_TRANSFER tx_length;
                                                                         // calculate whether we can send block in one go or not
                if ((ptTTYQue->tty_queue.get + ptTTYQue->tty_queue.chars) >= ptTTYQue->tty_queue.buffer_end) {
                    tx_length = (QUEUE_TRANSFER)(ptTTYQue->tty_queue.buffer_end - ptTTYQue->tty_queue.get); // single transfer up to the end of the buffer
                }
                else {
                    tx_length = ptTTYQue->tty_queue.chars;               // single transfer block
                }
                if (tx_length == 0) {
                    return;
                }
    #if defined WAKE_BLOCKED_TX
        #if defined SUPPORT_FLOW_HIGH_LOW                                // {23}
                if ((ptTTYQue->tty_queue.chars > ptTTYQue->tty_queue.buf_length/2) && ((ptTTYQue->tty_queue.chars - tx_length) >= tx_control[channel]->low_water_level)) 
        #else
                if ((ptTTYQue->tty_queue.chars > ptTTYQue->tty_queue.buf_length/2) && ((ptTTYQue->tty_queue.chars - tx_length) >= LOW_WATER_MARK)) 
        #endif
                {
                    if (tx_length > ptTTYQue->tty_queue.chars/2) {
                        tx_length /= 2;                                  // perform the DMA in at least two blocks so that a waiting task
                    }
                }                                                        // has a chance of being woken before the transmit buffer completely empties
    #endif
                ptTTYQue->ucState |= TX_ACTIVE;                          // mark activity
                tx_length = fnTxByteDMA(channel, ptTTYQue->tty_queue.get, tx_length); // {17} initiate buffer transfer using DMA
                if ((ptTTYQue->tty_queue.get += tx_length) >= ptTTYQue->tty_queue.buffer_end) { // assume that transmission will be successful
                    ptTTYQue->tty_queue.get = ptTTYQue->tty_queue.QUEbuffer;
                }
    #if defined WAKE_BLOCKED_TX
        #if defined SUPPORT_FLOW_HIGH_LOW                                // {23}
                fnWakeBlockedTx(ptTTYQue, tx_control[channel]->low_water_level);
        #else
                fnWakeBlockedTx(ptTTYQue, LOW_WATER_MARK);                
        #endif
    #endif
        //      ptTTYQue->tty_queue.chars -= tx_length;                  // {15} don't remove length until after complete transmission
                ptTTYQue->lastDMA_block_length = tx_length;              // {15} save the block length for removal after complete tramsmission
                return;
            }
#endif
#if defined UART_EXTENDED_MODE && defined SERIAL_MULTIDROP_TX            // {22}
            if ((ptTTYQue->opn_mode & MULTIDROP_MODE_TX) != 0) {         // send 9 bits
                if (ptTTYQue->tty_queue.chars > 1) {
                    if (++ptTTYQue->tty_queue.get >= ptTTYQue->tty_queue.buffer_end) { // wrap in circular buffer
                        ptTTYQue->tty_queue.get = ptTTYQue->tty_queue.QUEbuffer; // wrap to beginning
                    }
                    fnSetMultiDropByte(channel, *(ptTTYQue->tty_queue.get)); // prepare the 9th bit ready for transmission
                    --ptTTYQue->tty_queue.chars;
                }
            }
#endif
            fnTxByte(channel, ucNextByte);
#if defined SERIAL_STATS
            ptTTYQue->ulSerialCounter[SERIAL_STATS_CHARS]++;
#endif
            ptTTYQue->ucState |= TX_ACTIVE;                              // mark activity

#if defined SERIAL_SUPPORT_ESCAPE
            if (iBlock != 0) {
                return;                                                  // we have sent an escape character and so don't delete from input buffer
            }
#endif
            if (++ptTTYQue->tty_queue.get >= ptTTYQue->tty_queue.buffer_end) { // wrap in circular buffer
                ptTTYQue->tty_queue.get = ptTTYQue->tty_queue.QUEbuffer; // wrap to beginning
            }
            --ptTTYQue->tty_queue.chars;
#if defined WAKE_BLOCKED_TX
    #if defined SUPPORT_FLOW_HIGH_LOW
            fnWakeBlockedTx(ptTTYQue, tx_control[channel]->low_water_level); // {23}
    #else
            fnWakeBlockedTx(ptTTYQue, LOW_WATER_MARK);                   // {9}
    #endif
#endif
        }
    }
    else {
        fnClearTxInt(channel);                                           // clear interrupt since we are not allowed to send at the moment
    }
}

/* =================================================================== */
/*                         interrupt  interface routines               */
/* =================================================================== */

// This is called by the interrupt routine to put one received byte into the input buffer
//
extern void fnSciRxByte(unsigned char ch, QUEUE_HANDLE Channel)
{
#if defined (SUPPORT_MSG_CNT) && defined (SUPPORT_MSG_MODE)
    unsigned char *ptrBuffer;
#endif
    int iBlockBuffer = 0;
    TTYQUE *rx_ctl = rx_control[Channel];
#if defined SERIAL_SUPPORT_XON_XOFF || defined SERIAL_SUPPORT_ECHO || defined SUPPORT_FLOW_CONTROL // {9}
    TTYQUE *tx_ctl = tx_control[Channel];
#endif

#if defined SERIAL_SUPPORT_DMA && defined SERIAL_SUPPORT_DMA_RX
    if (rx_ctl->ucDMA_mode & UART_RX_DMA) {                              // new characters in the buffer - increment message count
        QUEUE_TRANSFER transfer_length = rx_ctl->tty_queue.buf_length;
        if (rx_ctl->ucDMA_mode & UART_RX_DMA_HALF_BUFFER) {
            transfer_length /= 2;
        }
        rx_ctl->tty_queue.put += transfer_length;
        rx_ctl->tty_queue.chars += transfer_length;                      // {30} amount of characters that are ready to be read
        if (rx_ctl->opn_mode & MSG_MODE_RX_CNT) {
    #if defined MSG_CNT_WORD                                             // {10}
            transfer_length -= 2;
    #else
            transfer_length -= 1;
    #endif
        }
        if (rx_ctl->tty_queue.put >= rx_ctl->tty_queue.buffer_end) {
            rx_ctl->tty_queue.put -= rx_ctl->tty_queue.buf_length;
        }
        fnPrepareRxDMA(Channel, rx_ctl->tty_queue.put, transfer_length); // enable next reception
        rx_ctl->msgs++;                                                  // we have 1 complete message
        if (rx_ctl->wake_task != 0) {                                    // wake up an input task, if defined
            uTaskerStateChange(rx_ctl->wake_task, UTASKER_ACTIVATE);     // wake up service interface task
        }
        return;
    }
#endif
#if defined MODBUS_RTU                                                   // {14}
    if (RTU_RX_MODE & rx_ctl->opn_mode) {                                // inter-space timer needs to be started
        fnRetrigger_T1_5_monitor(Channel);                               // retrigger the chosen timer to monitor the 1.5 character interval
    }
#endif
#if defined SERIAL_SUPPORT_ESCAPE                                        // escape sequence
    if (RX_ESC_MODE & rx_ctl->opn_mode) {
        if (rx_ctl->ucState & ESCAPE_SEQUENCE) {
            rx_ctl->ucState &= ~ESCAPE_SEQUENCE;
        }
        else if (rx_ctl->ucMessageFilter == ch) {
            rx_ctl->ucState |= ESCAPE_SEQUENCE;
            return;                                                      // we don't save this character yet
        }
        else {
            if (MSG_MODE & rx_ctl->opn_mode) {
                if (rx_ctl->ucMessageTerminator == ch) {
                    iBlockBuffer = 1;
                }
            }
        }
    }
    else {
        if (MSG_MODE & rx_ctl->opn_mode) {
            if (rx_ctl->ucMessageTerminator == ch) {
                iBlockBuffer = 1;
            }
        }
    }
#else
    #if defined SUPPORT_MSG_MODE
    if (MSG_MODE & rx_ctl->opn_mode) {
        if (rx_ctl->ucMessageTerminator == ch) {
            iBlockBuffer = 1;
        }
    }
    #endif
#endif

#if defined SERIAL_SUPPORT_XON_XOFF                                      // XON XOFF support
    if (rx_ctl->opn_mode & USE_XON_OFF) {
        if (XOFF_CODE == ch) {
    #if defined SERIAL_SUPPORT_DMA                                       // {18}
            if (tx_ctl->ucDMA_mode & UART_TX_DMA) {
                fnAbortDMA_transmission(Channel, tx_ctl);
            }
    #endif
            tx_ctl->ucState |= TX_WAIT;                                  // block transmission
    #if defined SERIAL_STATS
            tx_ctl->ulSerialCounter[SERIAL_STATS_FLOW]++;                // count the times we have to stall the transmitter
    #endif
            return;
        }
        else if (XON_CODE == ch) {                                       // {13}
            if (tx_ctl->ucState & TX_WAIT) {                             // if in XOFF state
                tx_ctl->ucState &= ~TX_WAIT;                             // unblock transmitter
                send_next_byte(Channel, tx_ctl);                         // restart transmission
            }
            return;
        }
    }
#endif
#if defined SERIAL_STATS
    rx_ctl->ulSerialCounter[SERIAL_STATS_CHARS]++;                       // count the number of characters we receive
#endif
#if defined SERIAL_SUPPORT_ECHO
    if ((rx_ctl->opn_mode & ECHO_RX_CHARS) != 0) {                       // echo received chars automatically
        if (tx_ctl->tty_queue.chars <= tx_ctl->tty_queue.buf_length) {   // copy the data to the output buffer and start transmission if not already done
            *(tx_ctl->tty_queue.put) = ch;                               // copy rx byte to output buffer
            if (++tx_ctl->tty_queue.put >= tx_ctl->tty_queue.buffer_end) { // wrap in circular buffer
                tx_ctl->tty_queue.put = tx_ctl->tty_queue.QUEbuffer;     // wrap to beginning
            }
            ++tx_ctl->tty_queue.chars;
        }

        if ((tx_ctl->ucState & (TX_WAIT | TX_ACTIVE)) == 0) {
            send_next_byte(Channel, tx_ctl);                             // this is not done when the transmitter is already performing a transfer or if suspended
        }
    }
#endif

    if (iBlockBuffer == 0) {
        if (rx_ctl->tty_queue.chars < rx_ctl->tty_queue.buf_length) {    // never overwrite contents of buffer
            ++rx_ctl->tty_queue.chars;
#if defined (SUPPORT_MSG_CNT) && defined (SUPPORT_MSG_MODE)
            rx_ctl->msgchars++;                                          // update the present message length
#endif
            *(rx_ctl->tty_queue.put) = ch;                               // put received character in input buffer
            if (++rx_ctl->tty_queue.put >= rx_ctl->tty_queue.buffer_end) { // wrap in circular buffer
                rx_ctl->tty_queue.put = rx_ctl->tty_queue.QUEbuffer;     // wrap to beginning
            }
#if defined UART_EXTENDED_MODE && defined SERIAL_MULTIDROP_TX            // {25}
            if ((rx_ctl->opn_mode & MULTIDROP_MODE_RX) != 0) {           // extra byte is required
                ++rx_ctl->tty_queue.chars;
                *(rx_ctl->tty_queue.put) = fnGetMultiDropByte(Channel);  // get the extra byte and store in the input buffer
                if (++rx_ctl->tty_queue.put >= rx_ctl->tty_queue.buffer_end) { // wrap in circular buffer
                    rx_ctl->tty_queue.put = rx_ctl->tty_queue.QUEbuffer; // wrap to beginning
                }
            }
#endif
        }

#if defined SUPPORT_FLOW_CONTROL
        if (((rx_ctl->opn_mode & (USE_XON_OFF | RTS_CTS)) != 0) && ((rx_ctl->ucState & RX_HIGHWATER) == 0) // support XON XOFF and RTS/CTS for receiver

    #if defined SUPPORT_FLOW_HIGH_LOW
        && ((rx_ctl->tty_queue.chars >= rx_ctl->high_water_level)))
    #else
        && ((rx_ctl->tty_queue.chars > (rx_ctl->tty_queue.buf_length - HIGH_WATER_MARK))))
    #endif
        {
            if ((rx_ctl->opn_mode & USE_XON_OFF) != 0) {
                tx_ctl->ucState |= SEND_XOFF;                            // {7}
                send_next_byte(Channel, tx_ctl);                         // send XOFF as quickly as possible
            }
    #if defined SUPPORT_HW_FLOW
            else {
                fnControlLine(Channel, CLEAR_CTS, tx_ctl->opn_mode);     // {21} remove CTS as quickly as possible
            }
    #endif
            rx_ctl->ucState |= RX_HIGHWATER;
    #if defined SERIAL_STATS
            rx_ctl->ulSerialCounter[SERIAL_STATS_FLOW]++;                // count the number of times we stall the flow
    #endif
        }
#endif
        if ((rx_ctl->opn_mode & (MSG_MODE | MSG_BREAK_MODE)) != 0) {
            return;                                                      // we don't wake until a complete message has been received
        }
    }
    else  {                                                              // end of message received
#if defined (SUPPORT_MSG_CNT) && defined (SUPPORT_MSG_MODE)
        if ((rx_ctl->opn_mode & MSG_MODE_RX_CNT) != 0) {
            if (rx_ctl->msgchars != 0) {                                 // if we have not actually collected any characters don't save a length
                rx_ctl->msgs += 2;                                       // a new message length and data message is ready
    #if defined MSG_CNT_WORD                                             // {10}
                ptrBuffer = rx_ctl->tty_queue.put - rx_ctl->msgchars - 2;// move back to start of this message
    #else
                ptrBuffer = rx_ctl->tty_queue.put - rx_ctl->msgchars - 1;// move back to start of this message
    #endif
                if (++rx_ctl->tty_queue.put >= rx_ctl->tty_queue.buffer_end) { // make space for next length and handle wrap in circular buffer
                    rx_ctl->tty_queue.put = rx_ctl->tty_queue.QUEbuffer; // wrap to beginning
                }
    #if defined MSG_CNT_WORD                                             // {10}
                if (++rx_ctl->tty_queue.put >= rx_ctl->tty_queue.buffer_end) { // make space for next length and handle wrap in circular buffer
                    rx_ctl->tty_queue.put = rx_ctl->tty_queue.QUEbuffer; // wrap to beginning
                }
    #endif
                if (ptrBuffer < rx_ctl->tty_queue.QUEbuffer) {           // set pointer to reserved message length position
                    ptrBuffer += rx_ctl->tty_queue.buf_length;
                }
    #if defined MSG_CNT_WORD                                             // {10}
                *ptrBuffer = (unsigned char)(rx_ctl->msgchars>>8);
                if (++ptrBuffer >= rx_ctl->tty_queue.buffer_end) {       // handle wrap in circular buffer
                    ptrBuffer = rx_ctl->tty_queue.QUEbuffer;             // wrap to beginning
                }
                rx_ctl->tty_queue.chars++;                               // include the length of next message
    #endif
                *ptrBuffer = (unsigned char)rx_ctl->msgchars;            // write the length of the message at first position (doesn't include the terminator in this mode)
                rx_ctl->tty_queue.chars++;                               // include the length of next message
                rx_ctl->msgchars = 0;                                    // reset for next message
            }
        }
        else {
#endif
            rx_ctl->msgs++;                                              // we have 1 complete message
            if (rx_ctl->tty_queue.chars < rx_ctl->tty_queue.buf_length) {// never overwrite contents of buffer
                ++rx_ctl->tty_queue.chars;
                *(rx_ctl->tty_queue.put) = ch;                           // put received character in input buffer
                if (++rx_ctl->tty_queue.put >= rx_ctl->tty_queue.buffer_end) { // wrap in circular buffer
                    rx_ctl->tty_queue.put = rx_ctl->tty_queue.QUEbuffer; // wrap to beginning
                }
            }
#if defined (SUPPORT_MSG_CNT) && defined (SUPPORT_MSG_MODE)
        }
#endif
    }

    if (rx_ctl->wake_task != 0) {                                        // wake up an input task, if defined
        uTaskerStateChange(rx_ctl->wake_task, UTASKER_ACTIVATE);         // wake up owner task
    }
}


#if defined UART_BREAK_SUPPORT
extern void fnSciRxMsg(QUEUE_HANDLE channel)                             // {11}
{
    TTYQUE *rx_ctl = rx_control[channel];
    #if defined SERIAL_SUPPORT_DMA
    unsigned char *ptrNextHalfBuffer;
    QUEUE_TRANSFER halfBufferLength;
    if ((rx_ctl->ucDMA_mode & UART_RX_DMA) != 0) {
        halfBufferLength = rx_ctl->msgchars;
        rx_ctl->msgchars = fnGetDMACount(channel, halfBufferLength);     // get the actual received character count

        if (rx_ctl->msgchars != 0) {
            rx_ctl->msgchars--;                                          // remove break character
        }
        else {
            rx_ctl->msgchars = halfBufferLength;
            return;                                                      // this happens when an end of break is detected (beginning of break has already caused buffer to be read)
        }
        ptrNextHalfBuffer = (rx_ctl->tty_queue.put + halfBufferLength);
        if ((rx_ctl->opn_mode & MSG_MODE_RX_CNT) != 0) {
        #if defined MSG_CNT_WORD
            ptrNextHalfBuffer += 2;
        #else
            ptrNextHalfBuffer++;
        #endif
        }
        if (ptrNextHalfBuffer >= rx_ctl->tty_queue.buffer_end) {
            ptrNextHalfBuffer -= rx_ctl->tty_queue.buf_length;
        }
        fnPrepareRxDMA(channel, ptrNextHalfBuffer, halfBufferLength);    // enable next DMA transfer
        rx_ctl->tty_queue.put += rx_ctl->msgchars;
        rx_ctl->tty_queue.chars += rx_ctl->msgchars;
    }
    #endif
    if ((rx_ctl->opn_mode & MSG_BREAK_MODE) != 0) {
        if ((rx_ctl->opn_mode & MSG_MODE_RX_CNT) != 0) {
    #if defined MSG_CNT_WORD
            unsigned char *ptrLen = (unsigned char *)&rx_ctl->msgchars;
    #endif
            unsigned char *ptrBuffer;
            rx_ctl->msgs += 2;                                           // a new message length and data message is ready
    #if defined MSG_CNT_WORD
            ptrBuffer = rx_ctl->tty_queue.put - rx_ctl->msgchars - 2;    // move back to start of this message
            rx_ctl->tty_queue.put += 2;
            if (rx_ctl->tty_queue.put >= rx_ctl->tty_queue.buffer_end) { // make space for next length and handle wrap in circular buffer
                rx_ctl->tty_queue.put -= rx_ctl->tty_queue.buf_length;   // wrap to beginning
            }
    #else
            ptrBuffer = rx_ctl->tty_queue.put - rx_ctl->msgchars - 1;    // move back to start of this message
            if (++rx_ctl->tty_queue.put >= rx_ctl->tty_queue.buffer_end) { // make space for next length and handle wrap in circular buffer
                rx_ctl->tty_queue.put = rx_ctl->tty_queue.QUEbuffer;     // wrap to beginning
            }
    #endif
            if (ptrBuffer < rx_ctl->tty_queue.QUEbuffer) {               // set pointer to reserved message length position
                ptrBuffer += rx_ctl->tty_queue.buf_length;
            }

    #if defined MSG_CNT_WORD
            *ptrBuffer++ = *ptrLen++;
            if (ptrBuffer >= rx_ctl->tty_queue.buffer_end) {             // handle wrap in circular buffer
                ptrBuffer = rx_ctl->tty_queue.QUEbuffer;                 // wrap to beginning
            }
            *ptrBuffer = *ptrLen;
            rx_ctl->tty_queue.chars += 2;                                // include the length of next message
    #else
            *ptrBuffer = (unsigned char)rx_ctl->msgchars;                // write the length of the message at first position (doesn't include the terminator in this mode)
            rx_ctl->tty_queue.chars++;                                   // include the length of next message
    #endif
    #if defined SERIAL_SUPPORT_DMA                                       // {27}
            if (rx_ctl->ucDMA_mode & UART_RX_DMA) {
                rx_ctl->tty_queue.put = ptrNextHalfBuffer;               // set to next half of buffer
                rx_ctl->msgchars = halfBufferLength;                     // reset for next message
            }
            else {
                rx_ctl->msgchars = 0;                                    // reset for next message
            }
    #else
            rx_ctl->msgchars = 0;                                        // reset for next message
    #endif
        }
        else {
            rx_ctl->msgs++;
        }
        if (rx_ctl->wake_task != 0) {                                    // wake up an input task, if defined
            uTaskerStateChange(rx_ctl->wake_task, UTASKER_ACTIVATE);     // wake up service interface task
        }
    }
}
#endif

#if defined SUPPORT_HW_FLOW
// The RTS line has just changed - we assume it only arrives when in RTS_CTS mode
//
extern void fnRTS_change(QUEUE_HANDLE Channel, int iState)
{
    TTYQUE *tx_ctl = tx_control[Channel];

    if (iState != 0) {                                                   // line has just gone active - we are allowed to transmit
        if (tx_ctl->ucState & TX_WAIT) {                                 // {31} if the state was blocking transmission
            tx_ctl->ucState &= ~TX_WAIT;                                 // unblock transmitter
            send_next_byte(Channel, tx_ctl);                             // restart transmission, if paused {5}
        }
    }
    else {                                                               // line has just gone inactive - we are not allowed to transmit
        tx_ctl->ucState |= TX_WAIT;                                      // block transmission
    #if defined SERIAL_SUPPORT_DMA
        if (tx_ctl->ucDMA_mode & UART_TX_DMA) {
            fnAbortDMA_transmission(Channel, tx_ctl);                    // abort DMA transmission
        }
    #endif
    }
}
#endif

// The tx interrupt routine calls this
//
extern void fnSciTxByte(QUEUE_HANDLE Channel)
{
    send_next_byte(Channel, tx_control[Channel]);
}

#endif
