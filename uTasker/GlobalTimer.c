/**********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher

    ---------------------------------------------------------------------
    File:      GlobalTimer.c
    Project:   Single Chip Embedded Internet
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2016
    *********************************************************************
    This task is used as global system timer task to allow multiple timers
    18.01.2007 Correct hardware timer check                              {1}
    04.04.2007 Correct start of software timer when hardware time for the same task is active {2}
    03.01.2008 Correct deleted software timer firing early               {3}
    05.03.2008 Add ability to kill all timers belonging to a certain task {4}
    10.04.2008 Correct global HW monostable retrigger                    {5}
    22.10.2010 Correct starting a new timer just before an existing fires {6}
    17.01.2011 Ensure stTimer is zeroed at reset                         {7}
    24.05.2011 Make use of the uTaskerRemainingTime() routine            {8}
    24.06.2011 Code optimisations                                        {9}
    24.06.2011 Replace CLOCK_LIMIT with DELAY_LIMIT                      {10}

 */

#include "config.h"


#if defined GLOBAL_TIMER_TASK                                            // activate this define to support global timer task

/* =================================================================== */
/*                       local structure definitions                   */
/* =================================================================== */

typedef struct stTimerBlock
{
    UTASK_TASK    OwnerTask;
    DELAY_LIMIT   TimerDelay;                                            // {10}
    unsigned char ucEvent;
} TIMER_BLOCK;


/* =================================================================== */
/*                      local variable definitions                     */
/* =================================================================== */

static TIMER_BLOCK stTimer[TIMER_QUANTITY] = {{0}};                      // {7}
//static UTASK_TICK  TimeStamp;                                          // {9}
static DELAY_LIMIT   NextFire;                                           // {10}
#if defined USE_TIMER_FUNCTION_EVENT
    static void     (*function_task)(unsigned char ucEvent) = 0;
#endif


/* =================================================================== */
/*                          local definitions                          */
/* =================================================================== */

#define OWN_TASK               TASK_TIMER

#define E_TIMER_FIRED 1                                                  // the event signaling that next timer has fired

/* =================================================================== */
/*                 local function prototype declarations               */
/* =================================================================== */

static void fnSWTimerFired(void);
#if defined GLOBAL_HARDWARE_TIMER
    static void fnHWTimerFired(void);
#endif


extern void fnTimer(TTASKTABLE *ptrTaskTable)                            // global Timer task
{
    QUEUE_HANDLE PortIDInternal = ptrTaskTable->TaskID;                  // queue ID for task input
    unsigned char ucInputMessage[HEADER_LENGTH];                         // reserve space for receiving messages

    while (fnRead( PortIDInternal, ucInputMessage, HEADER_LENGTH)) {     // check input queue
        switch (ucInputMessage[MSG_SOURCE_TASK]) {
        case TIMER_EVENT:
            fnSWTimerFired();
            break;

#if defined GLOBAL_HARDWARE_TIMER
        case INTERRUPT_EVENT:
            fnHWTimerFired();
            break;
#endif
        }
    }
}

static TIMER_BLOCK *fnGetFreeTimer(void)
{
    int iTimers = 0;
    TIMER_BLOCK *ptrTim = stTimer;

    while (iTimers++ < TIMER_QUANTITY) {
        if (!ptrTim->OwnerTask) {
            return ptrTim;                                               // timer block is free
        }
        ptrTim++;
    }
    return 0;                                                            // no free timers...
}

static TIMER_BLOCK *fnGetNotTimer(TIMER_BLOCK *ptrNewTimer)
{
    int iTimers = 0;
    TIMER_BLOCK *ptrTim = stTimer;

    while (iTimers++ < TIMER_QUANTITY) {
#if defined GLOBAL_HARDWARE_TIMER                                        // {2} - ignore hardware timers when checking for active software timers
        if ((ptrTim->OwnerTask) && (!(ptrTim->OwnerTask & HARDWARE_TIMER)) && (ptrTim != ptrNewTimer)) return ptrTim;
#else
        if ((ptrTim->OwnerTask) && (ptrTim != ptrNewTimer)) {
            return ptrTim;
        }
#endif
        ptrTim++;
    }
    return 0;                                                            // no other active timers
}

static TIMER_BLOCK *fnGetOwnerTimer(UTASK_TASK Owner, unsigned char ucEvent)
{
    int iTimers = 0;
    TIMER_BLOCK *ptrTim = stTimer;

#if defined GLOBAL_HARDWARE_TIMER
    Owner &= ~HARDWARE_TIMER;
#endif

    while (iTimers++ < TIMER_QUANTITY) {
        if ((ptrTim->ucEvent == ucEvent) &&
#if defined GLOBAL_HARDWARE_TIMER
            ((UTASK_TASK)(ptrTim->OwnerTask & ~HARDWARE_TIMER) == Owner)
#else
            (ptrTim->OwnerTask == Owner)
#endif
        ) {
            return ptrTim;
        }
        ptrTim++;
    }
    return 0;                                                            // no owner found
}

static TIMER_BLOCK *fnGetSWFired(DELAY_LIMIT Time)                       // {10}
{
    int iTimers = 0;
    TIMER_BLOCK *ptrTim = stTimer;
    TIMER_BLOCK *ptrFired = 0;

    while (iTimers < TIMER_QUANTITY) {
        if (((ptrTim->OwnerTask) && (ptrTim->TimerDelay <= Time)) && ((!ptrFired) || (ptrTim->TimerDelay < ptrFired->TimerDelay))) {
            ptrFired = ptrTim;                                           // lowest value
        }
        ptrTim++;
        iTimers++;
    }
    return ptrFired;
}


static void fnReduceSWTimers(TIMER_BLOCK *ptrNewTimer, DELAY_LIMIT TimerDelay) // {10}
{
    int iTimers = 0;
    TIMER_BLOCK *ptrTim = stTimer;
    if (TimerDelay == 0) {                                               // {9}
        return;
    }

    while (iTimers++ < TIMER_QUANTITY) {
        if ((ptrTim->OwnerTask) && (ptrTim != ptrNewTimer)) {
            ptrTim->TimerDelay -= TimerDelay;
        }
        ptrTim++;
    }
}

#if defined GLOBAL_HARDWARE_TIMER
static TIMER_BLOCK *fnReduceHWTimers(DELAY_LIMIT TimerDelay)             // {10}
{
    TIMER_BLOCK *ptrTim = stTimer;
    TIMER_BLOCK *ptrNextTim = 0;
    int iTimers = 0;
    DELAY_LIMIT NextDelay = (DELAY_LIMIT)(0 - 1);                        // {10}

    while (iTimers++ < TIMER_QUANTITY) {
        if (ptrTim->OwnerTask & HARDWARE_TIMER) {
            ptrTim->TimerDelay -= TimerDelay;
            if (ptrTim->TimerDelay <= NextDelay) {
                NextDelay = ptrTim->TimerDelay;                          // lowest next delay found up to now
                ptrNextTim = ptrTim;                                     // timer to be set up next
            }
        }
        ptrTim++;
    }
    return ptrNextTim;
}

// Check whether the reference will be the first to fire on its own or not
//
static int fnCheckHWFirst(TIMER_BLOCK *ptrCheckTimer)                    // {5}
{
    int iTimers = 0;
    TIMER_BLOCK *ptrTim = stTimer;

    while (iTimers++ < TIMER_QUANTITY) {
        if (ptrTim->OwnerTask & HARDWARE_TIMER) {                        // active hardware timer
            if (ptrTim->TimerDelay < ptrCheckTimer->TimerDelay) {
                return 0;                                                // another timer will fire earlier than reference one
            }
            if (ptrTim->TimerDelay == ptrCheckTimer->TimerDelay) {       // lowest value found
                if (ptrTim != ptrCheckTimer) {                           // ignore the timer being compared
                    return 0;                                            // another will fire at same time as reference
                }
            }
        }
        ptrTim++;
    }
    return 1;                                                            // the reference will be next to fire and it will fire alone
}

// Start hardware timer or queue the new timer
//
static void fnSetNewHWTimer(TIMER_BLOCK *ptrNewTimer)                    // {5} stop and adjust if retriggering HW monostable timer which is next to fire on its own
{
    DELAY_LIMIT adjusted;                                                // {10}
    uDisable_Interrupt();
    if (fnCheckHWFirst(ptrNewTimer) != 0) {                              // if we are adjusting the active HW timer that would fire as next, and alone
        DELAY_LIMIT TimerDelay = fnStopHW_Timer();                       // {10} stop the timer and get remaining time
        uEnable_Interrupt();
        if (TimerDelay != 0) {
            TIMER_BLOCK *ptrTim = stTimer;
            int iTimers = 0;
            while (iTimers++ < TIMER_QUANTITY) {                         // decrease hardware timer values
                if (ptrTim->OwnerTask & HARDWARE_TIMER) {
                    ptrTim->TimerDelay -= TimerDelay;
                }
                ptrTim++;
            }
            ptrNewTimer->TimerDelay += TimerDelay;                       // correct for our timer
        }
    }
    else {
        uEnable_Interrupt();
    }
    adjusted = fnSetHardwareTimer(&ptrNewTimer->TimerDelay);

    if (adjusted) {
        fnReduceHWTimers(adjusted);
        ptrNewTimer->TimerDelay += adjusted;                             // compensate adjustment for new timer itself
    }
}
#endif


static void fnStartNewTimer(TIMER_BLOCK *ptrNewTimer)
{
#if defined GLOBAL_HARDWARE_TIMER
    if (ptrNewTimer->OwnerTask & HARDWARE_TIMER) {                       // start a new hardware timer
        fnSetNewHWTimer(ptrNewTimer);                                    // start hardware timer or queue the new timer
        return;
    }
#endif
    if (!fnGetNotTimer(ptrNewTimer)) {                                   // start new mono-stable timer
        uTaskerMonoTimer(OWN_TASK, (DELAY_LIMIT)ptrNewTimer->TimerDelay, E_TIMER_FIRED); // if no other timer active, simply start ours
        NextFire = ptrNewTimer->TimerDelay;                              // the present delay
    }
    else {                                                               // if our delay is longer than the present delay we calculate delay difference
      //UTASK_TICK RemainingTime = (TimeStamp + NextFire);               // the time that the timer will fire
      //if (RemainingTime >= uTaskerSystemTick) {
      //    RemainingTime -= uTaskerSystemTick;
      //}
      //else {
      //    RemainingTime = (uTaskerSystemTick - RemainingTime);
      //}
        UTASK_TICK RemainingTime;                                        // {9}
        uDisable_Interrupt();                                            // ensure that the tick can not increment while doing the following
            RemainingTime = uTaskerRemainingTime(OWN_TASK);              // {8} get the time remaining before the timer fires - if this is 0 it means that it has fired and is waiting to be handled
            if (RemainingTime > ptrNewTimer->TimerDelay) {               // the new timer value is shorter that the next firing time
              //fnReduceSWTimers(ptrNewTimer, (unsigned char)(NextFire - (unsigned char)RemainingTime)); {6}
                fnReduceSWTimers(ptrNewTimer, (DELAY_LIMIT)(NextFire - RemainingTime)); // {6}
                uTaskerMonoTimer(OWN_TASK, (DELAY_LIMIT)ptrNewTimer->TimerDelay, E_TIMER_FIRED);
                NextFire = ptrNewTimer->TimerDelay;
            }
            else {                                                       // {6} the next timer needs to fire either at the same time as the next firing time or later
                ptrNewTimer->TimerDelay += (DELAY_LIMIT)(NextFire - RemainingTime); // {10} modify the delta
            }
        uEnable_Interrupt();                                             // leave critical code region
    }
}

// Start/or retrigger a mono-stable timer for the defined event number belonging to the calling task
//
extern void uTaskerGlobalMonoTimer(UTASK_TASK OwnerTask, DELAY_LIMIT delay, unsigned char time_out_event) // {10}
{
    TIMER_BLOCK *ptrTimer;

    ptrTimer = fnGetOwnerTimer(OwnerTask, time_out_event);               // get the timer block
    if (ptrTimer) {
        ptrTimer->TimerDelay = delay;                                    // set new timeout (retrigger)
#if defined GLOBAL_HARDWARE_TIMER
        if (ptrTimer->OwnerTask != OwnerTask) {                          // swapping from hardware to sw timer, or inverse is true
            if ((OwnerTask & HARDWARE_TIMER) && (fnGetNotTimer(ptrTimer) == 0)) { // moving to hardware timer from last software timer
                uTaskerStopTimer(OWN_TASK);                              // since there are no other timers being used, simply stop the monostable timer
            }
            ptrTimer->OwnerTask = OwnerTask;                             // set correct type
        }
#endif
        fnStartNewTimer(ptrTimer);
    }
    else {
        ptrTimer = fnGetFreeTimer();                                     // define new timer
        if (ptrTimer) {
            ptrTimer->OwnerTask = OwnerTask;                             // enter timer details
            ptrTimer->TimerDelay = delay;
            ptrTimer->ucEvent = time_out_event;
            fnStartNewTimer(ptrTimer);                                   // start new timer delay
        }
    }
}

// Stop a mono-stable timer belonging to calling task, with defined event number
//
extern void uTaskerGlobalStopTimer(UTASK_TASK OwnerTask, unsigned char time_out_event)
{
    TIMER_BLOCK *ptrTimer;

    if (time_out_event == 0) {                                           // {4} kill all timers belonging to the task
        int iTimers = 0;
        TIMER_BLOCK *ptrTim = stTimer;

        while (iTimers++ < TIMER_QUANTITY) {                             // search all timers belonging to the owner task
#if defined GLOBAL_HARDWARE_TIMER
            if ((UTASK_TASK)(ptrTim->OwnerTask & ~HARDWARE_TIMER) == OwnerTask) // ignore hardware flag
#else
            if (ptrTim->OwnerTask == OwnerTask)
#endif
            {
                if (ptrTim->ucEvent != 0) {                              // safety check before recursive call
                    uTaskerGlobalStopTimer(ptrTim->OwnerTask, ptrTim->ucEvent);
                }
            }
            ptrTim++;
        }
        return;
    }

    ptrTimer = fnGetOwnerTimer(OwnerTask, time_out_event);               // get the timer block
    if (ptrTimer) {
#if defined GLOBAL_HARDWARE_TIMER
        if (ptrTimer->OwnerTask & HARDWARE_TIMER) {                      // we need to stop a hardware timer
            ptrTimer->OwnerTask = (UTASK_TASK)HARDWARE_TIMER;            // we let the hardware timer continue since it is costing us nothing and will just be ignored when it fires
            return;
        }
#endif
        ptrTimer->OwnerTask = 0;
        if (!fnGetNotTimer(ptrTimer)) {
            uTaskerStopTimer(OWN_TASK);                                  // since there are no other timers being used, simply stop the monostable timer
        }
    }
}

// The global timer task's timer has fired
//
static void fnSWTimerFired(void)
{
    TIMER_BLOCK dead_timer;                                              // {3}
    unsigned char ucMessage[ HEADER_LENGTH];
    TIMER_BLOCK *ptrTimer;
    TIMER_BLOCK *ptrRef = 0;
    DELAY_LIMIT FiredTime = (DELAY_LIMIT)(0 - 1);                        // {10}
#if defined USE_TIMER_FUNCTION_EVENT
    unsigned char ucEvents[TIMER_QUANTITY];
    int iEvents = 0;
#endif

    ucMessage[MSG_DESTINATION_NODE] = INTERNAL_ROUTE;                    // internal message
    ucMessage[MSG_SOURCE_NODE] = INTERNAL_ROUTE;
    ucMessage[MSG_SOURCE_TASK] = TIMER_EVENT;                            // a timer event

    while ((ptrTimer = fnGetSWFired(FiredTime)) != 0) {                  // handle multiple timers which may have fired at the same time
        if (NextFire < ptrTimer->TimerDelay) {                           // {3} if the timer fired before expected it is due to a deleted timer whose monostable timer could not be stopped. In this case we must not generate an event.
            dead_timer.TimerDelay = NextFire;                            // set the killed timer time
            ptrRef = &dead_timer;
            break;
        }
#if defined USE_TIMER_FUNCTION_EVENT
        if (FUNCTION_EVENT == (unsigned char)ptrTimer->OwnerTask) {      // if defined as function event
            ucEvents[iEvents++] = ptrTimer->ucEvent;                     // collect function events
            FiredTime = ptrTimer->TimerDelay;
            ptrTimer->OwnerTask = 0;                                     // clear timer block since event has been sent
            ptrRef = ptrTimer;
            continue;
        }
#endif
        ucMessage[MSG_DESTINATION_TASK] = ptrTimer->OwnerTask;           // destination task
        ucMessage[MSG_TIMER_EVENT] = ptrTimer->ucEvent;                  // action
        FiredTime = ptrTimer->TimerDelay;
        ptrTimer->OwnerTask = 0;                                         // clear timer block since event has been sent
        fnWrite(INTERNAL_ROUTE, ucMessage, HEADER_LENGTH);               // send timer event to defined task
        ptrRef = ptrTimer;
    }
    if (ptrRef) {
        fnReduceSWTimers(ptrRef, ptrRef->TimerDelay);
    }
    if ((ptrTimer = fnGetSWFired((DELAY_LIMIT)(0 - 1))) != 0) {          // {10} if there are still timers, set next timeout
        uTaskerMonoTimer(OWN_TASK, (DELAY_LIMIT)ptrTimer->TimerDelay, E_TIMER_FIRED);
        NextFire = ptrTimer->TimerDelay;
    }
#if defined USE_TIMER_FUNCTION_EVENT
    if (iEvents && (function_task != 0)) {                               // if we have collected function events
        int iEventRep = 0;
        do {
            (*function_task)(ucEvents[iEventRep++]);                     // call function task with each event
        } while (iEventRep < iEvents);
    }
#endif
}

#if defined GLOBAL_HARDWARE_TIMER

static TIMER_BLOCK *fnGetHWFired(void)
{
    int iTimers = 0;
    TIMER_BLOCK *ptrTim = stTimer;
    TIMER_BLOCK *ptrFired = 0;
    DELAY_LIMIT FiredTime = (DELAY_LIMIT)(0 - 1);                        // {10}

    while (iTimers++ < TIMER_QUANTITY) {
        if (ptrTim->OwnerTask & HARDWARE_TIMER) {                        // active hardware timer
            if (ptrTim->TimerDelay <= FiredTime) {                       // lowest value found
                FiredTime = ptrTim->TimerDelay;                          // new lowest value
                ptrFired = ptrTim;                                       // lowest timer
            }
        }
        ptrTim++;
    }
    return ptrFired;
}

static void fnHWTimerFired(void)
{
    unsigned char ucMessage[ HEADER_LENGTH];
    TIMER_BLOCK *ptrTimer;
#if defined USE_TIMER_FUNCTION_EVENT
    unsigned char ucEvents[TIMER_QUANTITY];
    int iEvents = 0;
#endif

    ucMessage[MSG_DESTINATION_NODE] = 0;                                 // internal message
    ucMessage[MSG_SOURCE_NODE] = 0;
    ucMessage[MSG_SOURCE_TASK] = TIMER_EVENT;                            // a timer event

    while ((ptrTimer = fnGetHWFired()) != 0) {                           // handle multiple timers which may have fired at the same time
#if defined USE_TIMER_FUNCTION_EVENT
        if ((FUNCTION_EVENT | HARDWARE_TIMER) == (unsigned char)ptrTimer->OwnerTask) { // if defined as function event
            ucEvents[iEvents++] = ptrTimer->ucEvent;                     // collect function events
        }
        else {
#endif
            ucMessage[MSG_DESTINATION_TASK] = (ptrTimer->OwnerTask & ~HARDWARE_TIMER);// destination task
            ucMessage[MSG_TIMER_EVENT] = ptrTimer->ucEvent;              // action
            if (ucMessage[ MSG_DESTINATION_TASK ] != 0) {
                fnWrite(INTERNAL_ROUTE, ucMessage, HEADER_LENGTH);      // send timer event to defined task
            }
#if defined USE_TIMER_FUNCTION_EVENT
        }
#endif
        ptrTimer->OwnerTask = 0;                                         // clear timer block since event has been sent

        ptrTimer = fnReduceHWTimers(ptrTimer->TimerDelay);               // get next timer, if available
        if (ptrTimer) {                                                  // if there are still timers, set next timeout
            if (ptrTimer->TimerDelay == 0) {                             // further timer(s) with identical timeout
                continue;
            }
            fnSetNewHWTimer(ptrTimer);                                   // start hardware timer or queue the new timer
            break;
        }
    }
#if defined USE_TIMER_FUNCTION_EVENT
    if (iEvents && (function_task != 0)) {                               // if we have collected function events
        do {
            (*function_task)(ucEvents[--iEvents]);                       // call function task with each event
        } while (iEvents);
    }
#endif
}
#endif

#if defined USE_TIMER_FUNCTION_EVENT
void fnSetFuncTask(void (*function)(unsigned char ucEvent)) {
    function_task = function;
}
#endif

#endif
