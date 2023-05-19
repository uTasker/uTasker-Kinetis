/**********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher

    ---------------------------------------------------------------------
    File:      can_drv.c 
    Project:   Single Chip Embedded Internet
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2016
    *********************************************************************
    04.12.2011 Pass channel to fnCAN_tx() and fnCAN_get_rx()             {1}

*/

#include "config.h"

#if defined CAN_INTERFACE


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
/*                      global function definitions                    */
/* =================================================================== */


// Standard entry call to driver - dispatches required sub-routine
//
static QUEUE_TRANSFER entry_can(unsigned char channel, unsigned char *ptBuffer, QUEUE_TRANSFER Counter, unsigned char ucCallType, QUEUE_HANDLE DriverID)
{
    TTYQUE *ptTTYQue;
    QUEUE_TRANSFER rtn_val = 0;

    uDisable_Interrupt();                                                // disable all interrupts

    switch (ucCallType) {
    case CALL_DRIVER:                                                    // request changes and return status
        if ((CAST_POINTER_ARITHMETIC)ptBuffer & MODIFY_TX) {
            ptTTYQue = (struct stTTYQue *)(que_ids[DriverID].output_buffer_control); // tx state
        }
        else {
            ptTTYQue = (struct stTTYQue *)(que_ids[DriverID].input_buffer_control); // rx state
        }

        if (Counter) {                                                   // modify driver  state
            if (Counter & MODIFY_WAKEUP) {
                ptTTYQue->wake_task = (unsigned char)((CAST_POINTER_ARITHMETIC)ptBuffer & 0x7f);
            }
        }
        rtn_val = (QUEUE_TRANSFER)ptTTYQue->opn_mode;
        break;


    case CALL_INPUT:                                                     // request the number or input characters waiting
        ptTTYQue = (struct stTTYQue *)(que_ids[DriverID].input_buffer_control); // set to input control block
        if (ptTTYQue->opn_mode & MSG_MODE) {
            rtn_val = ptTTYQue->msgs;
        }
        else {
            rtn_val = ptTTYQue->tty_queue.chars;
        }
        break;

    case CALL_WRITE:                                                     // write data to CAN bus
        rtn_val = fnCAN_tx(que_ids[DriverID].qHandle, (QUEUE_HANDLE)(DriverID + 1), ptBuffer, Counter); // {1}
        break;

    case CALL_READ:                                                      // read a waiting CAN message
        rtn_val = fnCAN_get_rx(que_ids[DriverID].qHandle, (QUEUE_HANDLE)(DriverID + 1), ptBuffer, Counter); // {1}
        break;

    default:
        break;
    }
    uEnable_Interrupt();                                                 // enable interrupts

    return (rtn_val);
}



extern QUEUE_HANDLE fnOpenCAN(CANTABLE *pars, unsigned char driver_mode)
{
    QUEUE_HANDLE DriverID;
    IDINFO *ptrQueue;
    QUEUE_TRANSFER (*entry_add)(unsigned char channel, unsigned char *ptBuffer, QUEUE_TRANSFER Counter, unsigned char ucCallType, QUEUE_HANDLE DriverID) = entry_can;

    if (NO_ID_ALLOCATED == (DriverID = fnSearchID (entry_add, pars->Channel))) {
        fnInitCAN(pars);                                                 // this is the first call for this CAN interface so initialise general hardware
    }

    if (NO_ID_ALLOCATED == (DriverID = fnSearchID (0, 0))) {             // get next free ID
        return (NO_ID_ALLOCATED);                                        // no free IDs available
    }

    ptrQueue = &que_ids[DriverID - 1];
    ptrQueue->CallAddress = entry_add;
    ptrQueue->qHandle = pars->Channel;

    fnConfigCAN(DriverID, pars);                                         // configure buffers for this logical channel

    return (DriverID);                                                   // return the allocated ID
}

#endif
