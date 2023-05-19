/**********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher

    ---------------------------------------------------------------------
    File:      Watchdog.c
    Project:   Single Chip Embedded Internet
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2016
    *********************************************************************
    
*/        

#include "config.h"


extern void fnTaskWatchdog(TTASKTABLE *ptrTaskTable)                     // watchdog called regularly
{
    fnRetriggerWatchdog();                                               // hardware dependent
} 
