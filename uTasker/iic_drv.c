/**********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher

    ---------------------------------------------------------------------
    File:      iic_drv.c
    Project:   uTasker project
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2016
    *********************************************************************
    21.05.2007 Read modified to return zero when no receive messages are ready, even when there are characters available {1}
    13.08.2007 Removed GNU warning                                       {2}
    15.12.2007 Add tx buffer space request                               {3}
    05.06.2010 Add check of read/write address when simulating           {4}
    01.10.2015 Add I2C save mode support                                 {5}
    19.01.2015 Add support for modifying wakeup task                     {6}

*/


/* =================================================================== */
/*                           include files                             */
/* =================================================================== */

#include "config.h"


#if defined IIC_INTERFACE                                                // used only when specified

/* =================================================================== */
/*                          local definitions                          */
/* =================================================================== */


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
/*                      local function definitions                     */
/* =================================================================== */


/* =================================================================== */
/*                      global variable definitions                    */
/* =================================================================== */

IICQue *IIC_tx_control[NUMBER_IIC] = {0};
IICQue *IIC_rx_control[NUMBER_IIC] = {0};


// Standard entry call to driver - dispatches required sub-routine
//
static QUEUE_TRANSFER entry_IIC(QUEUE_HANDLE channel, unsigned char *ptBuffer, QUEUE_TRANSFER Counter, unsigned char ucCallType, QUEUE_HANDLE DriverID)
{
    IICQue *ptIICQue;
    QUEUE_TRANSFER rtn_val = 0;

    uDisable_Interrupt();                                                // disable all interrupts

    switch (ucCallType) {
    case CALL_INPUT:                                                     // request the number or input characters waiting
        ptIICQue = (struct stIICQue *)(que_ids[DriverID].input_buffer_control); // set to input control block
        if (ptIICQue->msgs != 0) {                                       // only give byte count if complete message has been received
            rtn_val = ptIICQue->IIC_queue.chars;                         // the number of received bytes
        }
        break;

    case CALL_READ:                                                      // read data from the queue
    #if defined IIC_SLAVE_MODE                                           // {5}
        ptIICQue = (struct stIICQue *)(que_ids[DriverID].input_buffer_control); // set to input control block
    #endif
        if (Counter == 0) {                                              // prepare a read sequence
    #if defined IIC_SLAVE_MODE                                           // {5}
            if (ptIICQue->ucState == IIC_RX_MESSAGE_MODE) {              // a call of zero length at the slave reduces the message counter (since complete message has been retrieved)
                if (ptIICQue->msgs) {
                    ptIICQue->msgs--;
                }
                break;
            }
    #endif
    #if defined _WINDOWS                                                 // {4}
            if ((*(ptBuffer + 1) & 0x01) == 0) {
                _EXCEPTION("Incorrect I2C address being used!!");        // cause an exception to warn of bad use
            }                                                            // the caller is trying to read from a write I2C address!!
    #endif
            ptIICQue = (struct stIICQue *)(que_ids[DriverID].output_buffer_control); // set to input control block

            uEnable_Interrupt();                                         // fnFillBuffer disables and then re-enables interrupts - be sure we are compatible
                rtn_val = fnFillBuf(&ptIICQue->IIC_queue, ptBuffer, 3);  // add three bytes (number of bytes to be read, the slave IIC address and the receiving task ID)
            uDisable_Interrupt();

            if ((ptIICQue->ucState & TX_ACTIVE) == 0) {                  // if active, we must queue the read
                fnTxIIC(ptIICQue, channel);                              // start the read function (begins with Tx)
            }
        }
        else {
    #if defined IIC_SLAVE_MODE                                           // {5}
            if (ptIICQue->ucState == IIC_RX_MESSAGE_MODE) {              // since the slave works in message mode it is allowed to read characters when the message counter is zero
                return fnGetBuf(&ptIICQue->IIC_queue, ptBuffer, Counter);// collect contents
            }
    #else
            ptIICQue = (struct stIICQue *)(que_ids[DriverID].input_buffer_control); // set to input control block
    #endif
            if (ptIICQue->msgs == 0) {                                   // we only retrieve characters when a complete message is ready {1}
                break;
            }
            ptIICQue->msgs--;                                            // remove message the caller is expected to retrieve complete message in one go
            return fnGetBuf(&ptIICQue->IIC_queue, ptBuffer, Counter);    // collect contents
        }
        break;

    case CALL_WRITE:                                                     // write data into the queue
        if (Counter != 0) {                                              // copy the data to the output buffer and start transmission if not already done
            ptIICQue = (struct stIICQue *)(que_ids[DriverID].output_buffer_control); // set to output control block
            if (ptBuffer == 0) {                                         // {3} call to check output buffer space
                QUEUE_TRANSFER free_space;
                if (ptIICQue->IIC_queue.put > ptIICQue->IIC_queue.get) {
                    free_space = (ptIICQue->IIC_queue.buf_length - (ptIICQue->IIC_queue.put - ptIICQue->IIC_queue.get));
                }
                else if (ptIICQue->IIC_queue.put < ptIICQue->IIC_queue.get) {
                    free_space = (ptIICQue->IIC_queue.get - ptIICQue->IIC_queue.put);
                }
                else {                                                   // identical pointer locations
                    if (ptIICQue->IIC_queue.chars != 0) {                // full buffer
                        break;                                           // no space
                    }
                    free_space = ptIICQue->IIC_queue.buf_length;         // empty buffer
                }
                if (free_space > Counter) {                              // check that the available buffer space is adequate to accept the message, including its present length field
                    rtn_val = (free_space - Counter);                    // return space afer the write (plus 1 so that exact space doesn't return 0)
                }
                break;
            }
    #if defined _WINDOWS                                                 // {4}
            if (*ptBuffer & 0x01) {
                _EXCEPTION("Incorrect I2C address being used!!");        // cause an exception to warn of bad use
            }                                                            // the caller is trying to write to a read I2C address!!
    #endif
            *ptIICQue->IIC_queue.put++ = (unsigned char)(Counter - 1);   // put length at first position in message
            if (ptIICQue->IIC_queue.put >= ptIICQue->IIC_queue.buffer_end) {
                ptIICQue->IIC_queue.put = ptIICQue->IIC_queue.QUEbuffer; // handle circular buffer
            }

            uEnable_Interrupt();                                         // fnFillBuffer disables and then re-enables interrupts - be sure we are compatible
                rtn_val = fnFillBuf(&ptIICQue->IIC_queue, ptBuffer, Counter);
            uDisable_Interrupt();
        
            if ((ptIICQue->ucState & (TX_WAIT | TX_ACTIVE | RX_ACTIVE)) == 0) { // if idle we can initiate transmission
                fnTxIIC(ptIICQue, channel);                              // initiate transmission
            }
        }
        break;

#if defined SUPPORT_FLUSH_IIC
    case CALL_FLUSH:                                                     // flush input or output queue completely
        if (Counter != FLUSH_RX) {                                       // tx
            ptIICQue = (struct stIICQue *)(que_ids[DriverID].output_buffer_control); // set to output control block
        }
        else {                                                           // rx
            ptIICQue = (struct stIICQue *)(que_ids[DriverID].input_buffer_control);  // set to input control block
        }
        ptIICQue->IIC_queue.get = ptIICQue->IIC_queue.put = ptIICQue->IIC_queue.QUEbuffer;
        ptIICQue->msgs = ptIICQue->IIC_queue.chars = 0;
        ptIICQue->ucState = 0;
        break;
#endif

    case CALL_DRIVER:                                                    // {6}
        if ((Counter & MODIFY_WAKEUP) != 0) {
            if ((CAST_POINTER_ARITHMETIC)ptBuffer & MODIFY_TX) {         // tx
                ptIICQue = (struct stIICQue *)(que_ids[DriverID].output_buffer_control); // set to output control block
            }
            else {                                                       // rx
                ptIICQue = (struct stIICQue *)(que_ids[DriverID].input_buffer_control);  // set to input control block
            }
            ptIICQue->wake_task = (UTASK_TASK)((CAST_POINTER_ARITHMETIC)ptBuffer & 0x7f);
        }
        break;

    default:
       break;
    }
    uEnable_Interrupt();                                                   // enable interrupts before leaving
    return (rtn_val);
}

// Open the interface and return its handle
//
extern QUEUE_HANDLE fnOpenIIC(IICTABLE *pars)
{
    QUEUE_HANDLE DriverID;

    if ((pars->Channel > NUMBER_IIC) || (NO_ID_ALLOCATED != fnSearchID (entry_IIC, pars->Channel))) {
        return (NO_ID_ALLOCATED);                                        // no hardware for this channel or else already open
    }

    if (NO_ID_ALLOCATED == (DriverID = fnSearchID (0, 0))) {             // get next free ID
        return (NO_ID_ALLOCATED);                                        // no free IDs available
    }
    --DriverID;                                                          // convert to array offset

                                                                         // configure the new driver and its queue(s)
    if (NO_MEMORY == fnAllocateQueue(&pars->Rx_tx_sizes, pars->Channel, entry_IIC, sizeof(IICQue))) {
        return (NO_ID_ALLOCATED);                                        // not enough memory for the queues
    }

    if ((IIC_rx_control[pars->Channel] = (IICQue *)(que_ids[DriverID].input_buffer_control)) != 0) { // {2}
        IIC_rx_control[pars->Channel]->wake_task = pars->Task_to_wake;
    }

    if ((IIC_tx_control[pars->Channel] = (IICQue *)(que_ids[DriverID].output_buffer_control)) != 0) { // {2}
        IIC_tx_control[pars->Channel]->wake_task = pars->Task_to_wake;
    }

    fnConfigIIC(pars);                                                   // configure the hardware

    return (DriverID + 1);
}
#endif
