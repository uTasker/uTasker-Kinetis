/**********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher

    ---------------------------------------------------------------------
    File:      low_power.c
    Project:   Single Chip Embedded Internet
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2016
    *********************************************************************
    25.11.2007 Add optional low power monitoring                         {1}
    15.07.2014 Add optional preparation for low power mode and recovery after leaving low power mode {2}

*/        

#include "config.h"


#if defined SUPPORT_LOW_POWER

#define OWN_TASK  TASK_LOW_POWER

extern void fnLowPower(TTASKTABLE *ptrTaskTable)                         // low power task called on every scheduling pass
{
    if (uNoSchedule(OWN_TASK) == 0) {                                    // is the scheduler idle?
    #if defined PREPARE_SLEEP_STATE
        PREPARE_SLEEP_STATE();                                           // {2} optional preparation for the sleep mode to reduce peripheral consumption or enable only certain wakeup sources
    #endif
    #if defined MEASURE_LOW_POWER_ON
        MEASURE_LOW_POWER_ON();                                          // {1} interrupts are disabled at this point when the low power mode is to be entered
    #endif
        fnDoLowPower();                                                  // switch to low power mode until woken
    #if defined MEASURE_LOW_POWER_OFF
        MEASURE_LOW_POWER_OFF();                                         // {1} interrupt have been re-enabled when the low power mode is exited
    #endif
    #if defined RECOVER_SLEEP_STATE
        RECOVER_SLEEP_STATE();                                           // {2} optional changes on sleep mode exit to allow full power operation again
    #endif
    }
}
#endif
