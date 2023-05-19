/***********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher

    ---------------------------------------------------------------------
    File:      uTasker.c
    Project:   Single Chip Embedded Internet
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2016
    *********************************************************************
    24.03.2007 Starting a monostable timer with zero event changes periodic repetition timer {1}
    30.03.2007 Change end of task table on zero name rather than zero function. This allows more flexibility when redefining tasks {2}
    30.03.2007 Corrections in multi-start mode                                               {3}
    10.06.2007 RND_HW_SUPPORT used to get random numbers from hardware when available        {4}
    24.08.2007 Timer delays of less than 1 TICK rounded up to 1 so that they will not be lost{5}
    24.08.2007 Timer entries use enabled flag to ensure validity                             {6}
    04.09.2007 Correct timer list counter                                                    {7}
    04.03.2009 Change use of random number seed                                              {8}
    28.04.2009 Correct and improve random number generator                                   {9}
    30.05.2009 Add uTaskerRemainingTime()                                                    {10}
    14.08.2009 Only resynchronise monostable timer when stopping when there is a repetition  {12}
    29.09.2009 Save new random number value                                                  {13}
    01.11.2009 Add optional user TICK interrupt handler                                      {14}
    09.02.2011 Add MONITOR_PERFORMANCE support                                               {15}
    29.05.2011 Add optional timer event to periodic tasks                                    {16}
    04.06.2013 Added OS_MALLOC() default                                                     {17}
    02.02.2014 Add UTASKER_POLLING                                                           {18}
    22.07.2014 Add uGetTaskState()                                                           {19}

*/


/* =================================================================== */
/*                           include files                             */
/* =================================================================== */

#include "config.h"

/* =================================================================== */
/*                          local definitions                          */
/* =================================================================== */

#ifndef OS_MALLOC                                                        // {17}
    #define OS_MALLOC(x) uMalloc((MAX_MALLOC)(x))
#endif

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
/*                     global variable definitions                     */
/* =================================================================== */

volatile UTASK_TICK uTaskerSystemTick= 0;                                // system tick counter

#if defined MULTISTART
    MULTISTART_TABLE *ptMultiStartTable = 0;
    JUMP_TABLE *JumpTable = 0;
#endif

#if defined MONITOR_PERFORMANCE                                          // {15}
    unsigned long ulMaximumIdle = 0;
#endif


/* =================================================================== */
/*                      local variable definitions                     */
/* =================================================================== */

static TTIMETABLE *tTimerList = 0;                                       // pointer to timer list
static TTASKTABLE *tTaskTable = 0;                                       // pointer to process table

#if defined (RANDOM_NUMBER_GENERATOR) && !defined (RND_HW_SUPPORT)
    static unsigned short usRandomNumber = 0;
#endif

#if defined MONITOR_PERFORMANCE                                          // {15}
    static unsigned long ulTotalIdle = 0;
#endif

#if defined MULTISTART_SHARE
static void *fnGetDefaultTaskEntry(CHAR entry_name)
{
    const UTASKTABLEINIT *ptrTaskDescription = ctTaskTable;
    while (ptrTaskDescription->pcTaskName) {
        if (*ptrTaskDescription->pcTaskName == entry_name) {
            return (void*)ptrTaskDescription->ptrTaskEntry;
        }
        ptrTaskDescription++;                                            // {3}
    }
    return 0;
}
#endif

// Use this to initialise and start the uTasker by passing it its task table configuration
//
extern TASK_LIMIT uTaskerStart(const UTASKTABLEINIT *ptATaskTable, const UTASK_TASK *a_node_descriptions, const PHYSICAL_Q_LIMIT nr_physicalQueues)
{
    UTASKTABLEINIT    *ptTaskTable;
    TTASKTABLE        *ptFillTable;
    const UTASK_TASK  *node_descriptions;
    TASK_LIMIT         nr_of_tasks = (TASK_LIMIT)0;
    TIMER_LIMIT        nr_of_timers = (TIMER_LIMIT)0;
    QUEUE_LIMIT        nr_of_queues = (QUEUE_LIMIT)0;
#if defined MULTISTART_SHARE
    int                iRetry = 0;
#endif

    tTimerList = 0;
    tTaskTable = 0;

    node_descriptions = a_node_descriptions;

    while ((int)(*node_descriptions) != 0) {                             // search through node description to find our node
        if ((CONFIG_LIMIT)(*node_descriptions) == OurConfigNr) {         // required node details have been found
            while (*(++node_descriptions) != (UTASK_TASK)0x00) {         // count the number of tasks to be created
                nr_of_tasks++;
            }
            ptFillTable = tTaskTable = (TTASKTABLE*)OS_MALLOC(((nr_of_tasks * sizeof(TTASKTABLE)) + sizeof(ptTaskTable->pcTaskName))); // allocate required memory for the tasks

            node_descriptions -= (nr_of_tasks + 1);                      // set back to beginning
            while ((int)(*(++node_descriptions)) != 0) {
                ptTaskTable = (UTASKTABLEINIT*)ptATaskTable;             // set at start and work down to bottom
#if defined MULTISTART_SHARE
retry:
#endif
                while (ptTaskTable->pcTaskName != 0) {                   // search for each task in the reference table {2}
                    if (*ptTaskTable->pcTaskName == *node_descriptions) {// only compare first letter
                        ptFillTable->ptrTaskEntry   = (void(*)(void *))ptTaskTable->ptrTaskEntry;
#if defined MULTISTART_SHARE
                        if (ptFillTable->ptrTaskEntry == 0) {
                            ptFillTable->ptrTaskEntry = (void(*)(void *))fnGetDefaultTaskEntry(*node_descriptions);
                            iRetry = 1;                                  // avoid taking all settings from default phase
                        }
#endif
                        ptFillTable->pcTaskName     = ptTaskTable->pcTaskName; // copy the contents
                        if ((ptFillTable->QueLength = ptTaskTable->QueLength) != 0) {
                            nr_of_queues++;
                        }
                        ptFillTable->TaskDelay      = ptTaskTable->TaskDelay; // set task start delay
                        ptFillTable->TaskRepetition = ptTaskTable->TaskRepetition; // set task repetition
                        ptFillTable->ucTaskState    = ptTaskTable->ucTaskState; // set the initial task state
#if defined MONITOR_PERFORMANCE                                          // {15}
                        ptFillTable->ulMinimumExecution = 0xffffffff;
#endif
                        if ((ptFillTable->TaskDelay != 0) || (ptFillTable->TaskRepetition != 0)) {
                            nr_of_timers++;                              // count number of tasks requiring timers
                        }

                        ptFillTable++;
                        break;
                    }
                    ptTaskTable++;
                }
#if defined MULTISTART_SHARE
                if ((ptTaskTable->ptrTaskEntry == 0) && (!iRetry)) {     // specified task has not been found - try to get it from the default table
                    ptTaskTable = (UTASKTABLEINIT*)ctTaskTable;          // set default table
                    iRetry = 1;
                    goto retry;
                }
                iRetry = 0;
#endif
            }
            ptFillTable->pcTaskName = (CHAR *)0;                         // add null task at end
            break;
        }
        else {
            while ((int)(*(++node_descriptions)) != 0) {}                // not our node - jump this block
            ++node_descriptions;
        }
    }

    if (tTaskTable != 0) {
        if ((int)nr_of_timers != 0) {
            tTimerList = (TTIMETABLE*)OS_MALLOC((nr_of_timers * sizeof(TTIMETABLE) + sizeof(tTimerList->ptTaskEntry)));  // get exact required timer list space

            ptFillTable = tTaskTable;                                    // establish initial timer list
            while (ptFillTable->pcTaskName) {                            // start at top and work down to bottom
                int iTimerEntryValid = 0;                                // {7}
                if (ptFillTable->TaskDelay != 0) {                       // delayed start required
                    if (NO_DELAY_RESERVE_MONO == ptFillTable->TaskDelay) { // place holder only so remove the delay
                        ptFillTable->TaskDelay = 0;
                    }
                    tTimerList->taskDelay = ptFillTable->TaskDelay;
                  //tTimerList->ptTaskEntry = ptFillTable;               // {6}
                  //tTimerList++;
                    iTimerEntryValid = 1;                                // {7}
                }
                else if (ptFillTable->TaskRepetition != 0) {             // no start delay but repetition
                    tTimerList->taskDelay = ptFillTable->TaskRepetition;
                  //tTimerList->ptTaskEntry = ptFillTable;               // {6}
                  //tTimerList++;
                    iTimerEntryValid = 1;                                // {7}
                }
                if (tTimerList->taskDelay != 0) {
                    tTimerList->ucTimerEnabled = (unsigned char)1;       // mark that the timer is active {6}
                }
                if (iTimerEntryValid != 0) {                             // {7}
                    tTimerList->ptTaskEntry = ptFillTable;               // enter the function
                    tTimerList++;
                }
                ptFillTable++;
            }
          //tTimerList->ptTaskEntry = 0;                                 // end of table (uMalloc returns zeroed memory)
            tTimerList -= nr_of_timers;
        }

        fnNeedQueues((QUEUE_LIMIT)(nr_of_queues + nr_physicalQueues));   // ensure the correct amount of queue space if reserved

        if ((int)nr_of_queues != 0) {
            ptFillTable = tTaskTable;                                    // create task queues
            while (ptFillTable->pcTaskName != 0) {                       // start at top and work down to bottom
                if (ptFillTable->QueLength != 0) {                       // establish the input queues to task if required
                    if (NO_ID_ALLOCATED == (ptFillTable->TaskID = (fnOpen(TYPE_QUEUE, FOR_READ, ptFillTable)))) {
                        ptFillTable->TaskID = ID_ERROR_STATE;            // mark not valid
                    }
                }
                ptFillTable++;
            }
        }
    }
#if defined RANDOM_NUMBER_GENERATOR
    #if defined RND_HW_SUPPORT
    fnInitialiseRND(0);                                                  // initialise HW with additional entropy if required
    #else
    fnInitialiseRND(&usRandomNumber);                                    // {8} get the random number seed
    #endif
#endif
    uTaskerSystemTick = 0;                                               // initialise the tick here since it may be used for random seed
    fnStartTick();

#if defined MULTISTART
    ptMultiStartTable = 0;
#endif
    return (nr_of_tasks);
}

#if defined RANDOM_NUMBER_GENERATOR

#define PRNG_POLY  0xb400                                                // taps: 16 14 13 11; characteristic polynomial: x^16 + x^14 + x^13 + x^11 + 1

extern unsigned short fnRandom(void)                                     // {9}
{
    #if defined RND_HW_SUPPORT
    return fnGetRndHW();                                                 // get a random value from the hardware
    #else
    register unsigned short usShifter = usRandomNumber;                  // for speed, copy to register

    usShifter = (unsigned short)((usShifter >> 1) ^ (-(signed short)(usShifter & 1) & PRNG_POLY));
    usRandomNumber = usShifter;                                          // {13} save the new value
    return (usShifter);
    #endif
}
#endif

#if defined MONITOR_PERFORMANCE                                          // {15}
// Each time a task starts and stops its duration is recorded in its task struct - an idle duration is recorded when no tasks are operating
// 
static void fnTimeCheck(TTASKTABLE *ptTaskTable)
{
    static TTASKTABLE *ptrPresentTask = 0;
    if (ptTaskTable == 0) {                                              // going idle, or remaining idle
        if (ptrPresentTask != 0) {                                       // a task has completed so we are going idle
            unsigned long ulTime = EXECUTION_DURATION();                 // the duration of the last task (read the value and restart the counter to measure the idle duration)
            if (ulTime < ptrPresentTask->ulMinimumExecution) {
                ptrPresentTask->ulMinimumExecution = ulTime;             // note the minimum duration recorded
            }
            if (ulTime > ptrPresentTask->ulMaximumExecution) {
                ptrPresentTask->ulMaximumExecution = ulTime;             // note the maximum duration recorded
                ptrPresentTask->ulTimeStamp = uTaskerSystemTick;         // note the tick value when this occurred
            }
            ptrPresentTask->ulTotalExecution += ulTime;                  // sum the total duration of task execution
            ptrPresentTask->ulExecutions++;                              // the number of times this task has been executed
            ptrPresentTask = 0;                                          // no task being executed
            if (ulMaximumIdle == 0xffffffff) {                           // the last task commands reset of the present performance measurement period
                TTASKTABLE *ptTaskTable = tTaskTable;                    // set at start and work down to bottom
                while (ptTaskTable->pcTaskName) {                        // for each task
                    ptTaskTable->ulMinimumExecution = 0xffffffff;        // reset task counters
                    ptTaskTable->ulExecutions = 0;
                    ptTaskTable->ulMaximumExecution = 0;
                    ptTaskTable->ulTotalExecution = 0;
                    ptTaskTable->ulTimeStamp = 0;
                    ptTaskTable++;
                }
                ulTotalIdle = 0;                                         // reset idle counters
                ulMaximumIdle = 0;
            }
        }
    }
    else {                                                               // task being started
        unsigned long ulTime = EXECUTION_DURATION();                     // the duration of the last idle phase (read the value and restart the counter to measure the idle duration)
        if (ulTime > ulMaximumIdle) {
            ulMaximumIdle = ulTime;
        }
        ulTotalIdle += ulTime;                                           // sum the total idle duration
        ptrPresentTask = ptTaskTable;                                    // remember the task being started
    }
}

// This routine calculates the present use of the CPU time from the task duration counters and returns the results for a specific task to the 
// struct pointed to by the UTASK_PERFORMANCE pointer
//
extern TTASKTABLE *fnGetTaskPerformance(int iTaskNumber, UTASK_PERFORMANCE *ptrDetails)
{
    TTASKTABLE *ptTaskTable = tTaskTable;                                // set at start and work down to bottom
    TTASKTABLE *ptrMatchingTask = 0;
    unsigned long ulTotalTasksDuration = 0;
    
    uMemset(ptrDetails, 0, sizeof(UTASK_PERFORMANCE));                   // ensure result is zeroed in case the task is not found
    while (ptTaskTable->pcTaskName) {                                    // for each task
        ulTotalTasksDuration += ptTaskTable->ulTotalExecution;           // sum total task execution duration of all tasks
        if (iTaskNumber == 0) {                                          // fill out for specific task
            ptrDetails->ulExecutions = ptTaskTable->ulExecutions;
            if (ptrDetails->ulExecutions != 0) {
                ptrDetails->ulMaximumExecution = ptTaskTable->ulMaximumExecution; // copy the present values belonging to the specified task
                ptrDetails->ulMinimumExecution = ptTaskTable->ulMinimumExecution;
                ptrDetails->ulTotalExecution = ptTaskTable->ulTotalExecution;
                ptrDetails->ulAverageExecution = (ptrDetails->ulTotalExecution / ptrDetails->ulExecutions);
            }
            ptrDetails->ulTimeStamp = ptTaskTable->ulTimeStamp;
            ptrMatchingTask = ptTaskTable;
        }
        ptTaskTable++;
        iTaskNumber--;
    }
    ptrDetails->ulTotalTasksDuration = ulTotalTasksDuration;
    ptrDetails->ulTotalIdle = ulTotalIdle;
    return ptrMatchingTask;
}
#endif

// This is normally called from within a forever loop in main()
//
#if defined MULTISTART
    extern MULTISTART_TABLE *uTaskerSchedule(void)
#else
    extern void uTaskerSchedule(void)
#endif
{
    TTASKTABLE *ptTaskTable = tTaskTable;                                // set at start and work down to bottom
    while (ptTaskTable->pcTaskName != 0) {                               // for each task in the task table
        if ((int)(ptTaskTable->ucTaskState & (UTASKER_POLLING | UTASKER_GO | UTASKER_ACTIVATE)) != 0) { // {18} if the task is to be scheduled
#if defined MONITOR_PERFORMANCE                                          // {15}
            fnTimeCheck(ptTaskTable);                                    // start measuring the duration of this task
#endif
            uDisable_Interrupt();                                        // protect from interrupts during this check
            if (((int)(ptTaskTable->TaskRepetition) != 0) || ((int)(ptTaskTable->ucTaskState & (UTASKER_GO)) == 0)) { // {18} if a repetitive task and not in go mode
#if defined PERIODIC_TIMER_EVENT                                         // {16}
                unsigned char timer_message[HEADER_LENGTH];              // timer event message space
                if (ptTaskTable->ucEvent != 0) {                         // if woken from monostable timer delay, set write event to queue if there is one
                    timer_message[MSG_TIMER_EVENT] = ptTaskTable->ucEvent; // event number
                    ptTaskTable->ucEvent = 0;                            // event handled so reset it
                }
                else {                                                   // periodic timer or initial task delay
                    if (ptTaskTable->TaskDelay != 0) {                   // initial delay
                        timer_message[MSG_TIMER_EVENT] = TIMER_EVENT_INITIAL_DELAY;
                        ptTaskTable->TaskDelay = 0;                      // this happens only once so clear the delay
                    }
                    else if (ptTaskTable->TaskRepetition != 0) {
                        timer_message[MSG_TIMER_EVENT] = TIMER_EVENT_PERIODIC;
                    }
                }
                if (ptTaskTable->QueLength != 0) {                       // ignore if task has no queue
                    timer_message[MSG_DESTINATION_NODE] = INTERNAL_ROUTE;// internal event
                    timer_message[MSG_SOURCE_NODE] = INTERNAL_ROUTE;
                    timer_message[MSG_DESTINATION_TASK] = *(ptTaskTable->pcTaskName); // event to this task
                    timer_message[MSG_SOURCE_TASK] = TIMER_EVENT;        // event type
    #if defined DELAYED_TIMER_EVENT
                    fnWrite(INTERNAL_ROUTE, timer_message, HEADER_LENGTH); // queue timer event in task queue
    #else
                    if (TIMER_EVENT_INITIAL_DELAY != timer_message[MSG_TIMER_EVENT]) {
                        fnWrite(INTERNAL_ROUTE, timer_message, HEADER_LENGTH); // queue timer event in task queue
                    }
    #endif
                }
#else
                if ((int)(ptTaskTable->ucEvent) != 0) {                  // if woken from monostable timer delay, set write event to queue if there is one
                    unsigned char timer_message[HEADER_LENGTH];          // timer event message space
                    timer_message[MSG_DESTINATION_NODE] = INTERNAL_ROUTE;// internal event
                    timer_message[MSG_SOURCE_NODE] = INTERNAL_ROUTE;
                    timer_message[MSG_DESTINATION_TASK] = (unsigned char)*(ptTaskTable->pcTaskName); // event to this task
                    timer_message[MSG_SOURCE_TASK] = TIMER_EVENT;        // event type
                    timer_message[MSG_TIMER_EVENT] = ptTaskTable->ucEvent; // event number
                    (void)fnWrite(INTERNAL_ROUTE, timer_message, HEADER_LENGTH); // queue timer event in task queue
                    ptTaskTable->ucEvent = NON_EVENT;                    // event handled so reset it
                }
#endif
                if ((int)(ptTaskTable->ucTaskState & (UTASKER_SUSPENDED | UTASKER_POLLING)) == 0) { // {18} check whether this task is suspended or in polling mode
                    ptTaskTable->ucTaskState = UTASKER_STOP;             // as long as it is not suspended or polling, we stop it so that it is only scheduled this once
                }
            }
            uEnable_Interrupt();                                         // enable interrupts before calling task
            ptTaskTable->ptrTaskEntry(ptTaskTable);                      // call the task
        }
#if defined MONITOR_PERFORMANCE                                          // {15}
        fnTimeCheck(0);                                                  // stop measuring the duration of last task and start measuring the idle phase
#endif
        ptTaskTable++;
    }
#if defined MULTISTART
    return ptMultiStartTable;                                            // return the pointer to next configuration
#endif
}

#if defined SUPPORT_LOW_POWER
// Check for tasks waiting to be scheduled, apart from calling task
//
extern int uNoSchedule(UTASK_TASK pcTaskName)
{
    TTASKTABLE *ptTaskTable = tTaskTable;                                // set at start and work down to bottom

    uDisable_Interrupt();                                                // protect from interrupts during this check
    while (ptTaskTable->pcTaskName != 0) {                               // for each task in the task table
        if ((ptTaskTable->ucTaskState & (UTASKER_POLLING | UTASKER_GO | UTASKER_ACTIVATE)) && (*ptTaskTable->pcTaskName != pcTaskName)) { // {18} if another task is to be scheduled
            uEnable_Interrupt();                                         // enable interrupts before leaving
            return 1;
        }
        ptTaskTable++;
    }
    return 0;                                                            // return with interrupts disabled
}
#endif

// Get the present state that a task is in
//
extern unsigned char uGetTaskState(UTASK_TASK pcTaskName)                // {19}
{
    TTASKTABLE *ptTaskTable = tTaskTable;                                // set at start and work down to bottom

    while (ptTaskTable->pcTaskName != 0) {                               // for each task in the task table
        if (*ptTaskTable->pcTaskName == pcTaskName) {
            return (ptTaskTable->ucTaskState);                           // return the state of the task at this moment in time
        }
        ptTaskTable++;
    }
    return UTASKER_ERROR;                                                // task not found
}

#if !defined _NO_TASK_STATE_CHANGE
// This is called by a task wanting to change its or another task's state
//
extern void uTaskerStateChange( UTASK_TASK pcTaskName, unsigned char ucSetState)
{
    TTASKTABLE *ptTaskTable;
    TTIMETABLE *ptTimerList;

    ptTaskTable = tTaskTable;                                            // set at start and work down to bottom
    while (ptTaskTable->pcTaskName != 0) {
        if (*ptTaskTable->pcTaskName == pcTaskName) {                    // only compare first letter - for speed - we only have small system
            if (UTASKER_STOP == ucSetState) {                            // are we stopping the task
                if ((ptTimerList = tTimerList) != 0) {                   // if there is a timer list
                    while (ptTimerList->ptTaskEntry != 0) {              // search for task in the timer list
                        if (ptTaskTable == ptTimerList->ptTaskEntry) {
                            uDisable_Interrupt();                        // protect from timer interrupts
                            if (ptTaskTable->TaskRepetition != 0) {      // {11}
                                ptTimerList->taskDelay = (uTaskerSystemTick + ptTaskTable->TaskRepetition); // re-synchronise timer
                            }
                            ptTaskTable->ucTaskState = UTASKER_STOP;     // set the new state
                            uEnable_Interrupt();                         // enable interrupts after modifications
                            return;                                      // completed
                        }
                        ptTimerList++;                                   // move to next timer in the timer list
                    }
                }
            }
            if ((int)(ucSetState & UTASKER_ACTIVATE) != 0) {             // if the task is being scheduled
                if ((int)(ptTaskTable->ucTaskState & UTASKER_POLLING) != 0) {
                    return;                                              // don't change the state of a polling task when UTASKER_ACTIVATE is to be set
                }
            }
            ptTaskTable->ucTaskState = ucSetState;                       // set new task state
            return;                                                      // completed
        }
        ptTaskTable++;
    }
}
#endif

// Start a monostable task timer
//
extern void uTaskerMonoTimer(UTASK_TASK pcTaskName, DELAY_LIMIT delay, unsigned char time_out_nr)
{
    TTASKTABLE *ptTaskTable;
    TTIMETABLE *ptTimerList = tTimerList;

    if (ptTimerList != 0) {                                                  // protection - it is not expected that there will never be timers in the syste,
        ptTaskTable = tTaskTable;                                            // set at start of the task table  and work down to bottom
        while (ptTaskTable->pcTaskName != 0) {                               // while task entries present
            if (*ptTaskTable->pcTaskName == pcTaskName) {                    // only compare first letter or task name
                while (ptTimerList->ptTaskEntry != 0) {                      // while tasks queued
                    if (ptTaskTable == ptTimerList->ptTaskEntry) {           // the task we are looking for
                        uDisable_Interrupt();                                // protect from interrupts
                        ptTaskTable->ucTaskState &= ~UTASKER_SUSPENDED;      // ensure the task can start again
                        if ((int)(ptTimerList->ucEvent = time_out_nr) == 0) {// set event message for this type of timeout
                            ptTaskTable->TaskRepetition = delay;             // modify delay value when zero event {1}
                            if (delay == 0) {                                // zero delay stops repetition
                                ptTimerList->ucTimerEnabled = (unsigned char)0; // disable timer {6}
                                uEnable_Interrupt();                         // enable interrupts after modifications
                                return;
                            }
                        }
                        if (delay == 0) {                                    // if the delay is less that a TICK interval round it up rather than never timing out {5}
                            delay = 1;
                        }
                        ptTimerList->taskDelay = (uTaskerSystemTick + delay);// set delay
                        ptTimerList->ucTimerEnabled = (unsigned char)1;      // ensure timer enabled {6}                    
                        uEnable_Interrupt();                                 // enable interrupts after modifications
                        return;
                    }
                    ptTimerList++;
                }
                break;                                                       // entry not found in timer queue
            }
            ptTaskTable++;
        }
    }
}

// Request the remaining time in TICKs before the task's monostable timer will fire. A zero return value means that the timer has already fired, or doesn't exist.
//
extern UTASK_TICK uTaskerRemainingTime(UTASK_TASK pcTaskName)            // {10}
{
    TTASKTABLE *ptTaskTable;
    TTIMETABLE *ptTimerList = tTimerList;

    if (ptTimerList != 0) {
        ptTaskTable = tTaskTable;                                        // set at start and work down to bottom
        while (ptTaskTable->pcTaskName != 0) {
            if (*ptTaskTable->pcTaskName == pcTaskName) {                // only compare first letter
                while (ptTimerList->ptTaskEntry != 0) {                  // while tasks in the timer list
                    if (ptTaskTable == ptTimerList->ptTaskEntry) {
                        UTASK_TICK ticks_remaining;
                        uDisable_Interrupt();                            // protect from interrupts
                        if ((int)(ptTimerList->ucTimerEnabled) != 0) {   // time is enabled
                            ticks_remaining = (ptTimerList->taskDelay - uTaskerSystemTick);
                        }
                        else {
                            ticks_remaining = 0;
                        }
                        uEnable_Interrupt();                             // enable interrupts after obtaining the instantaneous value
                        return (ticks_remaining);
                    }
                    ptTimerList++;                                       // move to next timer in the timer list
                }
                break;
            }
            ptTaskTable++;                                               // move to the next task in the task table
        }
    }
    return 0;
}

// This is used to kill a monostable timer
//
extern void uTaskerStopTimer(UTASK_TASK pcTaskName)
{
    TTASKTABLE *ptTaskTable;
    TTIMETABLE *ptTimerList = tTimerList;

    if (ptTimerList == 0) {
        return;                                                          // no timers in system
    }

    ptTaskTable = tTaskTable;                                            // set at start and work down to bottom
    while (ptTaskTable->pcTaskName != 0) {                               // for each task
        if (*ptTaskTable->pcTaskName == pcTaskName) {                    // only compare first letter for speed
            while (ptTimerList->ptTaskEntry != 0) {                      // while tasks queued
                if (ptTaskTable == ptTimerList->ptTaskEntry) {           // if the searched entry
                    uDisable_Interrupt();                                // protect from interrupts
                        ptTimerList->ucEvent = (unsigned char)0;         // ensure no event can be sent (even if queued)
                        ptTimerList->ucTimerEnabled = (unsigned char)0;  // disable timer {6}
                        ptTaskTable->ucTaskState |= UTASKER_SUSPENDED;   // {18} suspend the timer
                    uEnable_Interrupt();                                 // enable interrupts after modifications
                    return;
                }
                ptTimerList++;                                           // move to next timer in the timer list
            }
            return;
        }
        ptTaskTable++;                                                   // move to the next task in the task table
    }
}



/* =================================================================== */
/*                         interrupt  routines                         */
/* =================================================================== */

// This should be called by a tick interrupt routine - enters and leaves with interrupt disabled
//
extern void fnRtmkSystemTick(void)
{
    TTASKTABLE *ptTaskTable;
    TTIMETABLE *ptTimerList = tTimerList;

    uTaskerSystemTick++;                                                 // increment system tick
    TICK_INTERRUPT();                                                    // {14} allow user code in the TICK interrupt routine
    if (ptTimerList == 0) {
        return;                                                          // no timers in system (yet)
    }

    while (ptTimerList->ptTaskEntry != 0) {                              // while tasks queued
        if (((int)(ptTimerList->ucTimerEnabled) != 0) && (uTaskerSystemTick == ptTimerList->taskDelay)) { // {6} if enabled task timer matches
            ptTaskTable = ptTimerList->ptTaskEntry;
            if ((int)(ptTaskTable->ucTaskState & UTASKER_SUSPENDED) == 0) { // as long as not suspended task
                if ((int)(ptTaskTable->ucTaskState & UTASKER_POLLING) == 0) { // {18} as long as not in polling mode
                    ptTaskTable->ucTaskState = UTASKER_ACTIVATE;         // release task to run once
                }
                ptTaskTable->ucEvent = ptTimerList->ucEvent;             // enable event to be put to the input queue
            }
            if (ptTaskTable->TaskRepetition != 0) {                      // if the task is repetitive
                ptTimerList->taskDelay += ptTaskTable->TaskRepetition;   // set next time for periodic scheduling
            }
            else {
                ptTimerList->ucTimerEnabled = (unsigned char)0;          // disable single shot timer {6}
            }
        }
        ptTimerList++;                                                   // move to the next task in the task table
    }
}

