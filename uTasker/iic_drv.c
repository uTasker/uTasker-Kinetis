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
    Copyright (C) M.J.Butcher Consulting 2004..2017
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


#if defined I2C_INTERFACE                                                // used only when specified

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

I2CQue *I2C_tx_control[NUMBER_I2C] = {0};
I2CQue *I2C_rx_control[NUMBER_I2C] = {0};


// Standard entry call to driver - dispatches required sub-routine
//
static QUEUE_TRANSFER entry_I2C(QUEUE_HANDLE channel, unsigned char *ptBuffer, QUEUE_TRANSFER Counter, unsigned char ucCallType, QUEUE_HANDLE DriverID)
{
    I2CQue *ptI2CQue;
    QUEUE_TRANSFER rtn_val = 0;

    uDisable_Interrupt();                                                // disable all interrupts

    switch (ucCallType) {
    case CALL_INPUT:                                                     // request the number or input characters waiting
        ptI2CQue = (struct stI2CQue *)(que_ids[DriverID].input_buffer_control); // set to input control block
        if (ptI2CQue->msgs != 0) {                                       // only give byte count if complete message has been received
            rtn_val = ptI2CQue->I2C_queue.chars;                         // the number of received bytes
        }
        break;

    case CALL_READ:                                                      // read data from the queue
    #if defined I2C_SLAVE_MODE                                           // {5}
        ptI2CQue = (struct stI2CQue *)(que_ids[DriverID].input_buffer_control); // set to input control block
    #endif
        if (Counter == 0) {                                              // prepare a read sequence
    #if defined I2C_SLAVE_MODE                                           // {5}
            if ((ptI2CQue->ucState & I2C_SLAVE_RX_MESSAGE_MODE) != 0) {  // a call of zero length at the slave reduces the message counter (since complete message has been retrieved)
                if (ptI2CQue->msgs != 0) {
                    ptI2CQue->msgs--;
                }
                break;
            }
    #endif
    #if defined _WINDOWS                                                 // {4}
            if ((*(ptBuffer + 1) & 0x01) == 0) {
                _EXCEPTION("Incorrect I2C address being used!!");        // cause an exception to warn of bad use
            }                                                            // the caller is trying to read from a write I2C address!!
    #endif
            ptI2CQue = (struct stI2CQue *)(que_ids[DriverID].output_buffer_control); // set to input control block

            uEnable_Interrupt();                                         // fnFillBuffer disables and then re-enables interrupts - be sure we are compatible
                rtn_val = fnFillBuf(&ptI2CQue->I2C_queue, ptBuffer, 3);  // add three bytes (number of bytes to be read, the slave I2C address and the receiving task ID)
            uDisable_Interrupt();

            if ((ptI2CQue->ucState & TX_ACTIVE) == 0) {                  // if active, we must queue the read
                fnTxI2C(ptI2CQue, channel);                              // start the read function (begins with Tx)
            }
        }
        else {
    #if defined I2C_SLAVE_MODE                                           // {5}
            if ((ptI2CQue->ucState & I2C_SLAVE_RX_MESSAGE_MODE) != 0) {  // since the slave works in message mode it is allowed to read characters when the message counter is zero
                return fnGetBuf(&ptI2CQue->I2C_queue, ptBuffer, Counter);// collect contents
            }
    #else
            ptI2CQue = (struct stI2CQue *)(que_ids[DriverID].input_buffer_control); // set to input control block
    #endif
            if (ptI2CQue->msgs == 0) {                                   // we only retrieve characters when a complete message is ready {1}
                break;
            }
            ptI2CQue->msgs--;                                            // remove message the caller is expected to retrieve complete message in one go
            return fnGetBuf(&ptI2CQue->I2C_queue, ptBuffer, Counter);    // collect contents
        }
        break;

    case CALL_WRITE:                                                     // write data into the queue
        if (Counter != 0) {                                              // copy the data to the output buffer and start transmission if not already done
            ptI2CQue = (struct stI2CQue *)(que_ids[DriverID].output_buffer_control); // set to output control block
            if (ptBuffer == 0) {                                         // {3} call to check output buffer space
                QUEUE_TRANSFER free_space;
                if (ptI2CQue->I2C_queue.put > ptI2CQue->I2C_queue.get) {
                    free_space = (ptI2CQue->I2C_queue.buf_length - (ptI2CQue->I2C_queue.put - ptI2CQue->I2C_queue.get));
                }
                else if (ptI2CQue->I2C_queue.put < ptI2CQue->I2C_queue.get) {
                    free_space = (ptI2CQue->I2C_queue.get - ptI2CQue->I2C_queue.put);
                }
                else {                                                   // identical pointer locations
                    if (ptI2CQue->I2C_queue.chars != 0) {                // full buffer
                        break;                                           // no space
                    }
                    free_space = ptI2CQue->I2C_queue.buf_length;         // empty buffer
                }
                if (free_space > Counter) {                              // check that the available buffer space is adequate to accept the message, including its present length field
                    rtn_val = (free_space - Counter);                    // return space afer the write (plus 1 so that exact space doesn't return 0)
                }
                break;
            }
    #if defined I2C_SLAVE_MODE
            if ((ptI2CQue->ucState & I2C_SLAVE_TX_BUFFER_MODE) == 0) {   // if the channel is in slave buffer mode we don't enter the length in the output buffer
    #endif
        #if defined _WINDOWS                                             // {4}
                if (*ptBuffer & 0x01) {
                    _EXCEPTION("Incorrect I2C address being used!!");    // cause an exception to warn of bad use
                }                                                        // the caller is trying to write to a read I2C address!!
        #endif
                *ptI2CQue->I2C_queue.put++ = (unsigned char)(Counter - 1); // put length at first position in message
    #if defined I2C_SLAVE_MODE
                }
    #endif
            if (ptI2CQue->I2C_queue.put >= ptI2CQue->I2C_queue.buffer_end) {
                ptI2CQue->I2C_queue.put = ptI2CQue->I2C_queue.QUEbuffer; // handle circular buffer
            }

            uEnable_Interrupt();                                         // fnFillBuffer disables and then re-enables interrupts - be sure we are compatible
                rtn_val = fnFillBuf(&ptI2CQue->I2C_queue, ptBuffer, Counter);
            uDisable_Interrupt();
        
            if ((ptI2CQue->ucState & (TX_WAIT | TX_ACTIVE | RX_ACTIVE)) == 0) { // if idle we can initiate transmission
                fnTxI2C(ptI2CQue, channel);                              // initiate transmission
            }
        }
        break;

#if defined SUPPORT_FLUSH_I2C
    case CALL_FLUSH:                                                     // flush input or output queue completely
        if (Counter != FLUSH_RX) {                                       // tx
            ptI2CQue = (struct stI2CQue *)(que_ids[DriverID].output_buffer_control); // set to output control block
        }
        else {                                                           // rx
            ptI2CQue = (struct stI2CQue *)(que_ids[DriverID].input_buffer_control);  // set to input control block
        }
        ptI2CQue->I2C_queue.get = ptI2CQue->I2C_queue.put = ptI2CQue->I2C_queue.QUEbuffer;
        ptI2CQue->msgs = ptI2CQue->I2C_queue.chars = 0;
        ptI2CQue->ucState = 0;
        break;
#endif

    case CALL_DRIVER:                                                    // {6}
        if ((Counter & MODIFY_WAKEUP) != 0) {
            if ((CAST_POINTER_ARITHMETIC)ptBuffer & MODIFY_TX) {         // tx
                ptI2CQue = (struct stI2CQue *)(que_ids[DriverID].output_buffer_control); // set to output control block
            }
            else {                                                       // rx
                ptI2CQue = (struct stI2CQue *)(que_ids[DriverID].input_buffer_control);  // set to input control block
            }
            ptI2CQue->wake_task = (UTASK_TASK)((CAST_POINTER_ARITHMETIC)ptBuffer & 0x7f);
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
extern QUEUE_HANDLE fnOpenI2C(I2CTABLE *pars)
{
    QUEUE_HANDLE DriverID;

    if ((pars->Channel > NUMBER_I2C) || (NO_ID_ALLOCATED != fnSearchID (entry_I2C, pars->Channel))) {
        return (NO_ID_ALLOCATED);                                        // no hardware for this channel or else already open
    }

    if (NO_ID_ALLOCATED == (DriverID = fnSearchID (0, 0))) {             // get next free ID
        return (NO_ID_ALLOCATED);                                        // no free IDs available
    }
    --DriverID;                                                          // convert to array offset

                                                                         // configure the new driver and its queue(s)
    if (NO_MEMORY == fnAllocateQueue(&pars->Rx_tx_sizes, pars->Channel, entry_I2C, sizeof(I2CQue))) {
        return (NO_ID_ALLOCATED);                                        // not enough memory for the queues
    }

    if ((I2C_rx_control[pars->Channel] = (I2CQue *)(que_ids[DriverID].input_buffer_control)) != 0) { // {2}
        I2C_rx_control[pars->Channel]->wake_task = pars->Task_to_wake;
    }

    if ((I2C_tx_control[pars->Channel] = (I2CQue *)(que_ids[DriverID].output_buffer_control)) != 0) { // {2}
        I2C_tx_control[pars->Channel]->wake_task = pars->Task_to_wake;
    }

    fnConfigI2C(pars);                                                   // configure the hardware

    return (DriverID + 1);
}
#endif
