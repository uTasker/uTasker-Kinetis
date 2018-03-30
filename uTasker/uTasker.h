/**********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher

    ---------------------------------------------------------------------
    File:      uTasker.h
    Project:   Single Chip Embedded Internet
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2018
    *********************************************************************
    11.05.2007 UTASK_TASK used consistently for node ids
    23.08.2007 enabled flag added to Timer table                         {1}
    10.04.2008 move fnSetHardwareTimer() to hardware.h
    30.05.2009 Add uTaskerRemainingTime()
    30.11.2010 Add CHECK_QUEUE                                           {2}
    09.02.2011 Add MONITOR_PERFORMANCE support                           {3}
    02.02.2014 Add UTASKER_POLLING                                       {4}
    22.07.2014 Add UTASKER_ERROR and uGetTaskState()                     {5}
    06.11.2015 Modify fnStackFree() to allow worst-case used stack to be returned {6}
    31.01.2017 Add TICK_UNIT_MS() and TICK_UNIT_US()                     {7}
    10.01.2018 pucBottomOfHeap made extern                               {8}

*/

#if !defined __UTASKER__
#define __UTASKER__

#define TICK_UNIT_MS(T) (T * 1000)                                       // {7} tick is defined in ms
#define TICK_UNIT_US(T) (T)                                              // {7} tick is defined in us
#if defined _WINDOWS
    #if _TICK_RESOLUTION < 1000                                          // limit tick to 1ms when simulating
        #define TICK_RESOLUTION (1000)
    #else
        #define TICK_RESOLUTION (_TICK_RESOLUTION)
    #endif
#else
    #define TICK_RESOLUTION (_TICK_RESOLUTION)
#endif

#define SEC     (1000000 / TICK_RESOLUTION)                              // {7} used for sec conversions


/*********************************** uTaster states *************************************/
#define UTASKER_STOP      (unsigned char)0x00                            // task stopped - waiting for event to cause it to run again.
#define UTASKER_GO        (unsigned char)0x01                            // task free to run - it will however only run once if it has a repetition timer set; otherwise this
                                                                         // is equivalent to polling mode except that an event sent to the task will cause it to stop
#define UTASKER_SUSPENDED (unsigned char)0x02                            // the task is suspended when its monostable timer has been stopped
#define UTASKER_ACTIVATE  (unsigned char)0x04                            // the task has been activated by a timer or other system event
#define UTASKER_POLLING   (unsigned char)0x08                            // {4} the task is in polling mode and will not stop when it receives an event
#define UTASKER_ERROR     (unsigned char)0x80                            // {5} this is not a true state but allows identifying when a task was not found

/********************************** uTasker message queues *****************************/

#define GLOBAL_MESSAGE            0x00

#define INTERNAL_ROUTE            (QUEUE_LIMIT)0

#define MSG_DESTINATION_NODE      0                                      // message header constitution
#define MSG_SOURCE_NODE           1
#define MSG_DESTINATION_TASK      2
#define MSG_SOURCE_TASK           3
#define MSG_CONTENT_LENGTH        4
#define MSG_INTERRUPT_EVENT       4
#define MSG_TIMER_EVENT           4
#define MSG_TIMER_INTERRUPT_EVENT 4
#define MSG_CONTENT_COMMAND       5
#define MSG_CONTENT_DATA_START    6

#define HEADER_LENGTH             5                                      // a message header has always this length

#define NO_QUE                 0
#define NO_QUEUE               NO_QUE

#define SMALL_MESSAGE          32                                        // these are a few message length defines - users can befine their own in config.h if special sizes are required
#define MEDIUM_MESSAGE         64
#define LARGE_MESSAGE          128
#define LARGER_MESSAGE         256
#define BIG_MESSAGE            512
#define BIGGER_MESSAGE         1024

#define SMALLEST_QUE           (HEADER_LENGTH + 0)                       // absolutely smallest size - use when only a timer or interrupt event can arrive
#define SMALL_QUEUE            (HEADER_LENGTH + SMALL_MESSAGE)
#define MEDIUM_QUE             (HEADER_LENGTH + MEDIUM_MESSAGE)
#define LARGE_QUE              (HEADER_LENGTH + LARGE_MESSAGE)
#define SMALLEST_QUEUE         SMALLEST_QUE
#define MEDIUM_QUEUE           MEDIUM_QUE
#define LARGE_QUEUE            LARGE_QUE

#define NO_DELAY_RESERVE_MONO (DELAY_LIMIT)(0 - 1)                       // zero delay but reserve a mono stable timer for the task

/********************************** uTasker timer and interrupt events *****************************/

#define TIMER_EVENT            (unsigned char)0x00                       // message from task 0x00 can only be timer
#define INTERRUPT_EVENT        (unsigned char)0x01                       // message from task 0x01 can only be interrupt
#define FUNCTION_EVENT         (unsigned char)0x02                       // message from task 0x02 is a function event, specifying handling by a function rather than a task
#define CHECK_QUEUE            (unsigned char)0x03                       // task reference used to check input queues {2}

#define NON_EVENT              (unsigned char)0x00
/* =================================================================== */
/*                     global structure definitions                    */
/* =================================================================== */

// Define task table structure
//
typedef struct stTaskTable
{
    CHAR           *pcTaskName;                                          // name of task
    void          (*ptrTaskEntry)(void *);                               // entry address of routine
#if defined MONITOR_PERFORMANCE                                          // {3}
    unsigned long  ulExecutions;                                         // total number of times that the task was executed
    unsigned long  ulTotalExecution;                                     // total duration of task execution since last monitoring reset
    unsigned long  ulMaximumExecution;                                   // longest execution duration recorded
    unsigned long  ulMinimumExecution;                                   // shortest execution duration recorded
    unsigned long  ulTimeStamp;                                          // tick time stamp of maximum execution duration detected
#endif
    QUEUE_TRANSFER QueLength;                                            // length of input queue to task
    DELAY_LIMIT    TaskDelay;                                            // delay before starting or pause length ticks)
    DELAY_LIMIT    TaskRepetition;                                       // repetition time for repetitive task (ticks)
    QUEUE_HANDLE   TaskID;                                               // identity given to read from input queue
    unsigned char  ucTaskState;                                          // initial / present state of task
    unsigned char  ucEvent;                                              // event to send on wake up from timer
} TTASKTABLE;



// Define task performance structure                                     {3}
//
typedef struct stUTASK_PERFORMANCE
{
    unsigned long ulTotalExecution;                                      // total duration of task execution since last monitoring reset
    unsigned long ulExecutions;                                          // number of times that the task was executed
    unsigned long ulMaximumExecution;                                    // longest execution duration recorded
    unsigned long ulAverageExecution;                                    // average execution duration
    unsigned long ulMinimumExecution;                                    // shortest execution duration recorded
    unsigned long ulTimeStamp;                                           // tick time stamp of maximum execution duration detected
    unsigned long ulTotalTasksDuration;                                  // total system task duraction
    unsigned long ulTotalIdle;                                           // total idle duraction
} UTASK_PERFORMANCE;

// Define task table initialisation structure
//
typedef struct stTaskTableInit
{
    CHAR           *pcTaskName;                                          // name of task
    void          (*ptrTaskEntry)(TTASKTABLE *);                         // entry address of routine
    QUEUE_TRANSFER QueLength;                                            // length of input que to task
    DELAY_LIMIT    TaskDelay;                                            // delay before starting or pause length (ticks)
    DELAY_LIMIT    TaskRepetition;                                       // repetition time for repetitive task (ticks)
    unsigned char  ucTaskState;                                          // Initial / present state of task
} UTASKTABLEINIT;


// Define timer table structure
//
typedef struct stTimeTable
{
    TTASKTABLE    *ptTaskEntry;                                          // address of task table for entry
    UTASK_TICK    taskDelay;                                             // count value to be matched
    unsigned char ucEvent;                                               // event to send on wake up from timer
    unsigned char ucTimerEnabled;                                        // indicate whether timer is presently enabled {1}
} TTIMETABLE;

typedef struct stHEAP_NEEDS
{
    CONFIG_LIMIT      ConfigNr;
    HEAP_REQUIREMENTS need_this_amount;
} HEAP_NEEDS;


typedef struct stJUMP_TABLE
{
    unsigned char ucVersion;                                             // version for table compatibility
    void          *fncPtr;                                               // list of jump functions (or others)
} JUMP_TABLE;

typedef struct stMULTISTART_TABLE
{
#if defined DYNAMIC_MULTISTART
    unsigned char     *(*new_hw_init)(JUMP_TABLE *, void **, unsigned char);
#else
    unsigned char     *(*new_hw_init)(JUMP_TABLE *);
#endif
    HEAP_NEEDS        *ptHeapNeed;
    UTASKTABLEINIT    *ptTaskTable;
    const UTASK_TASK  *ptNodesTable;
} MULTISTART_TABLE;


extern const UTASKTABLEINIT ctTaskTable[];
extern const UTASK_TASK     ctNodes[];

/* =================================================================== */
/*                 global variables/consts definitions                 */
/* =================================================================== */

extern volatile UTASK_TICK   uTaskerSystemTick;                          // global tick timer
extern const HEAP_NEEDS      ctOurHeap[];                                // constant table with list of heap requirements per configuration
extern CONFIG_LIMIT          OurConfigNr;                                // present configuration
extern NETWORK_LIMIT         OurNetworkNumber;                           // present network node number
#if defined MULTISTART
    extern MULTISTART_TABLE *ptMultiStartTable;                          // pointer to a multi-start table
    extern JUMP_TABLE       *JumpTable;                                  // pointer to table holding jump table
#endif
#if defined MONITOR_PERFORMANCE
    extern unsigned long ulMaximumIdle;                                  // this value contains the maximum idle duration that has occurred - setting it to 0xffffffff causes the performance monitoring to be reset after the next schedule sequence
#endif
extern unsigned char *pucBottomOfHeap;                                   // {8} the location of heap memory, which is the top of static variables

/* =================================================================== */
/*                 global function prototype declarations              */
/* =================================================================== */

extern TASK_LIMIT uTaskerStart(const UTASKTABLEINIT *ptATaskTable, const UTASK_TASK *a_node_descriptions, const PHYSICAL_Q_LIMIT nr_physical_queues);
#if defined MULTISTART
    extern MULTISTART_TABLE *uTaskerSchedule(void);
#else
    extern void uTaskerSchedule(void);
#endif
extern int  uNoSchedule(UTASK_TASK pcTaskName);
extern unsigned char uGetTaskState(UTASK_TASK pcTaskName);               // {5}
extern void uTaskerStateChange(UTASK_TASK pcTaskName, unsigned char ucSetState); // change the state of a task

extern void fnRtmkSystemTick(void);
extern void uTaskerMonoTimer(UTASK_TASK pcTaskName, DELAY_LIMIT delay, unsigned char time_out_nr); // schedule task after delay, with timeout event
extern UTASK_TICK uTaskerRemainingTime(UTASK_TASK pcTaskName);
extern void uTaskerStopTimer(UTASK_TASK pcTaskName);
extern void fnInitialiseHeap(const HEAP_NEEDS *ctOurHeap, void *HeapStart);
extern HEAP_REQUIREMENTS  fnHeapAvailable(void);
extern HEAP_REQUIREMENTS  fnHeapFree(void);
extern STACK_REQUIREMENTS fnStackFree(STACK_REQUIREMENTS *stackUsed);    // {6}

extern TTASKTABLE *fnGetTaskPerformance(int iTaskNumber, UTASK_PERFORMANCE *ptrDetails); // {3}

extern unsigned short fnRandom(void);

#endif
