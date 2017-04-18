/************************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher

    ---------------------------------------------------------------------
    File:      TaskConfig.h
    Project:   Single Chip Embedded Internet
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2017
    *********************************************************************
    28.04.2007 Add SNMP task
    13.05.2007 Add PPP task
    10.04.2008 Remove unused ADC task
    14.06.2008 Add USB task
    07.11.2008 Add MODBUS task
    13.02.2009 Delay Ethernet task when using a PHY with very long power up delay {1}
    03.05.2009 Add TFT and GLED options                                  {2}
    16.08.2009 Increase key scan rate to 50ms                            {3}
    09.10.2009 LCD task conditional also on GLCD_COLOR                   {4}
    23.11.2009 Add SD-card and managed files support                     {5}
    05.02.2010 Add IPV6                                                  {6}
    07.07.2011 Add zero_config task                                      {7}
    05.09.2012 Remove LCD task when SUPPORT_TFT is enabled               {8}
    16.12.2012 Add PHY_POLL_LINK support                                 {9}
    16.12.2012 Add IGMP task                                             {10}
    02.06.2014 Allow task TASK_NETWORK_INDICATOR to be used with define INTERRUPT_TASK_PHY {11}
    11.02.2015 Added task TASK_TIME_KEEPER                               {12}
    18.11.2015 Add USB host task                                         {13}
    18.04.2017 Add BLINKEY configuration

*/
 

/******************************** The tasks used in the system ********************************************/
/* Warning - each task must use a unique letter equivalent to the start of its task name                  */
#define TASK_WATCHDOG           'W'                                      // watchdog task
#define TASK_ETHERNET           'E'                                      // ethernet task
#define TASK_ARP                'A'                                      // ARP task
#define TASK_TCP                'T'                                      // TCP task
#define TASK_DHCP               'D'                                      // DHCP task
#define TASK_DNS                'd'                                      // DNS task
#define TASK_POP3               'P'                                      // POP 3 task
#define TASK_PPP                'U'                                      // PPP task
#define TASK_SMTP               'S'                                      // SMTP task
#define TASK_FTP                'F'                                      // FTP task
#define TASK_TFTP               't'                                      // TFTP task
#define TASK_APPLICATION        'a'                                      // application task
#define TASK_CAN_SIM            'c'                                      // CAN simulator task
#define TASK_DEBUG              'm'                                      // debugger (maintenance) task
#define TASK_TIMER              'p'                                      // timer task for global timer use
#define TASK_LCD                'L'                                      // application LCD task
#define TASK_KEY                'K'                                      // keyboard scanning task
#define TASK_NETWORK_INDICATOR  'N'                                      // task displaying network activity
#define TASK_DATA_SOCKET        's'                                      // task handling data socket
#define TASK_ICMP               'i'                                      // this is a pseudotask for sending ping results
#define TASK_IGMP               'I'                                      // {10} IGMP task
#define TASK_LOW_POWER          'l'                                      // task supporting power saving
#define TASK_SNMP               'n'                                      // SnMP protocol task
#define TASK_USB                'u'                                      // USB task
#define TASK_USB_HOST           'H'                                      // {13} USB host task
#define TASK_MODBUS             'O'                                      // MODBUS task
#define TASK_MASS_STORAGE       'M'                                      // {5} mass storage task
#define TASK_ZERO_CONFIG        'z'                                      // {7} zero config task
#define TASK_TIME_KEEPER        'k'                                      // {12} time keeper task (RTC/SNTP)

#define TASK_DEV_1              '1'
#define TASK_DEV_2              '2'
#define TASK_DEV_3              '3'
#define TASK_DEV_4              '4'
#undef  OWN_TASK

extern void fnHTTP_task(TTASKTABLE *ptrTaskTable);

#if defined LAN_REPORT_ACTIVITY
   #define INTERRUPT_TASK_PHY         TASK_NETWORK_INDICATOR             // this task is woken on PHY changes (set 0 for none)
#endif
#define UNETWORK_MASTER               TASK_APPLICATION                   // this task is woken on uNetwork error events
#define INTERRUPT_TASK_LAN_EXCEPTIONS TASK_NETWORK_INDICATOR             // this task is woken on Ethernet exceptions (set 0 for none)
#define INTERRUPT_TASK_SERIAL_CONTROL TASK_APPLICATION                   // this task is woken on general serial control line changes (set 0 for none)
#define CAN_ERROR_TASK                TASK_APPLICATION                   // this task is woken on CAN errors

extern void fnTaskWatchdog(TTASKTABLE *);
extern void fnApplication(TTASKTABLE *);
extern void fnDebug(TTASKTABLE *);
extern void fnTaskEthernet(TTASKTABLE *);
extern void fnTaskArp(TTASKTABLE *);
extern void fnTaskTCP(TTASKTABLE *);
extern void fnDHCP(TTASKTABLE *);
extern void fnDNS(TTASKTABLE *);
extern void fnPOP3(TTASKTABLE *);
extern void fnSmtp(TTASKTABLE *);
extern void fnIgmp(TTASKTABLE *);                                        // {10}
extern void fnTftp(TTASKTABLE *);
extern void fnLCD(TTASKTABLE *);
extern void fnKey(TTASKTABLE *);
extern void fnTimer(TTASKTABLE *);
extern void fnNetworkIndicator(TTASKTABLE *);
extern void fnLowPower(TTASKTABLE *);
extern void fnSNMP(TTASKTABLE *);
extern void fnPPP(TTASKTABLE *);
extern void fnTaskUSB(TTASKTABLE *);
extern void fnMODBUS(TTASKTABLE *);
extern void fnMassStorage(TTASKTABLE *);                                 // {5}
extern void fnZeroConfig(TTASKTABLE *);                                  // {7}
extern void fnTimeKeeper(TTASKTABLE *ptrTaskTable);                      // {12}
#if defined QUICK_DEV_TASKS
    extern void fnQuickTask1(TTASKTABLE *ptrTaskTable);
    extern void fnQuickTask2(TTASKTABLE *ptrTaskTable);
    extern void fnQuickTask3(TTASKTABLE *ptrTaskTable);
    extern void fnQuickTask4(TTASKTABLE *ptrTaskTable);
#endif


/************ uTasker task table is defined here but only used by the hardware module initiates the system ***********/

#define DEFAULT_NODE_NUMBER    1                                         // we use one fixed node in the system

#if defined SUPPORT_DISTRIBUTED_NODES
    extern const unsigned char ucNodeMac[MAX_NETWORK_NODES][MAC_LENGTH];
#endif

#if defined OPSYS_CONFIG                                                 // this is only set in the hardware module

CONFIG_LIMIT OurConfigNr = DEFAULT_NODE_NUMBER;                          // in single node system this can be initialised with a fixed value

#if defined SUPPORT_DISTRIBUTED_NODES
    NETWORK_LIMIT OurNetworkNumber = 0;                                  // this value must be set (to non-zero) on startup individually for each node in the network

    const unsigned char ucNodeMac[MAX_NETWORK_NODES][MAC_LENGTH] = {
        {0,0,0,0,0,1},                                                   // MAC address of nodes in the system
        {0,0,0,0,0,2}
    };
#endif

const HEAP_NEEDS ctOurHeap[] = {
    {DEFAULT_NODE_NUMBER, OUR_HEAP_SIZE},                                // our node requires this amount of heap space
    {0}                                                                  // end
};


const UTASK_TASK ctNodes[] = {                                           // we use a single fixed configuration (single node)
    DEFAULT_NODE_NUMBER,                                                 // configuration our single node

    TASK_WATCHDOG,                                                       // watchdog task
#if defined USE_IP || defined USE_IPV6                                   // {6}
    TASK_ARP,                                                            // ARP task
#endif
#if defined ETH_INTERFACE
    TASK_ETHERNET,                                                       // ethernet task
#endif
#if defined USE_TCP
    TASK_TCP,                                                            // TCP task
#endif
#if defined USB_INTERFACE
    TASK_USB,                                                            // USB task
#endif
#if defined USE_MODBUS
    TASK_MODBUS,                                                         // MODBUS task
#endif
#if !defined BLINKEY
    TASK_APPLICATION,                                                    // application task
    TASK_DEBUG,                                                          // maintenance task
#endif
#if defined SDCARD_SUPPORT || defined SPI_FLASH_FAT || defined FLASH_FAT || defined MANAGED_FILES || defined USB_MSD_HOST
    TASK_MASS_STORAGE,                                                   // {5} mass storage task
#endif
#if defined USE_DHCP_CLIENT
    TASK_DHCP,                                                           // DHCP task
#endif
#if defined USE_DNS
    TASK_DNS,                                                            // DNS client task
#endif
#if defined USE_POP3
    TASK_POP3,                                                           // POP 3 task
#endif
#if defined USE_SMTP
    TASK_SMTP,                                                           // POP 3 task
#endif
#if defined USE_FTP
    TASK_FTP,                                                            // FTP task
#endif
#if defined USE_TFTP
    TASK_TFTP,                                                           // TFTP task
#endif
#if defined SUPPORT_LCD || defined SUPPORT_GLCD || defined SUPPORT_OLED/* || defined SUPPORT_TFT || defined GLCD_COLOR*/ // {2}{4}{8}
    TASK_LCD,                                                            // LCD task
#endif
#if defined USE_SNMP
    TASK_SNMP,                                                           // SNMP task
#endif
#if defined USE_PPP
    TASK_PPP,                                                            // PPP task
#endif
#if defined USE_ZERO_CONFIG
    TASK_ZERO_CONFIG,                                                    // zero config client task
#endif
#if defined SUPPORT_KEY_SCAN
    TASK_KEY,                                                            // key scan task
#endif
#if defined GLOBAL_TIMER_TASK
    TASK_TIMER,                                                          // global Timer Task
#endif
#if defined USE_IGMP
    TASK_IGMP,                                                           // {10} IGMP task
#endif
#if (defined LAN_REPORT_ACTIVITY || defined PHY_POLL_LINK || defined INTERRUPT_TASK_PHY) && !defined BLINKEY // {11}
    TASK_NETWORK_INDICATOR,                                              // network activity indicator task
#endif
#if (defined USE_SNTP || defined USE_TIME_SERVER || defined USE_TIME_SERVER || defined SUPPORT_RTC || defined SUPPORT_SW_RTC) && !defined BLINKEY // {12}
    TASK_TIME_KEEPER,
#endif
#if defined QUICK_DEV_TASKS
    TASK_DEV_1,
    TASK_DEV_2,
    TASK_DEV_3,
    TASK_DEV_4,
#endif
#if defined SUPPORT_LOW_POWER
    TASK_LOW_POWER,                                                      // low power task
#endif
    0,                                                                   // end of single configuration

    // insert more node configurations here if required
    //
    0                                                                    // end of configuration list
};


#if !defined PHY_POWERUP_DELAY
    #define PHY_POWERUP_DELAY 0                                          // {1}
#endif

const UTASKTABLEINIT ctTaskTable[] = {
    // task name,  task routine,   input queue size, start delay, period, initial task state
    //
#if defined _HW_SAM7X
    {"Wdog",      fnTaskWatchdog, NO_QUE,   (DELAY_LIMIT)(0.2 * SEC), (DELAY_LIMIT)(0.2 * SEC),  UTASKER_STOP}, // watchdog task (note SAM7X is not allowed to start watchdog immediately since it also checks for too fast triggering!!)
#else
    {"Wdog",      fnTaskWatchdog, NO_QUE,   0, (DELAY_LIMIT)(0.2 * SEC),  UTASKER_GO}, // watchdog task (runs immediately and then periodically)
#endif
#if defined USE_IP || defined USE_IPV6                                   // {6} warning - start ARP task before Ethernet. If Ethernet messages are received before ARP table is ready there would be an error..
    {"ARP",       fnTaskArp,    MEDIUM_QUE, (DELAY_LIMIT)(0.05 * SEC), 0, UTASKER_STOP}, // ARP task check periodically state of ARP table
#endif
#if defined ETH_INTERFACE
    {"Eth",       fnTaskEthernet, (HEADER_LENGTH * 12), (DELAY_LIMIT)((0.05 * SEC) + (PHY_POWERUP_DELAY)), 0, UTASKER_STOP}, // {1} ethernet task - runs automatically
#endif
#if defined USE_TCP
    {"TCP",       fnTaskTCP,    MEDIUM_QUE,  (DELAY_LIMIT)(NO_DELAY_RESERVE_MONO), 0, UTASKER_STOP}, // {1} TCP task checks periodically state of session timeouts (controlled by task itself)
#endif
#if defined USE_MODBUS
    {"O-MOD",     fnMODBUS,     MEDIUM_QUE,  (DELAY_LIMIT)(NO_DELAY_RESERVE_MONO), 0, UTASKER_STOP}, // MODBUS task  
#endif
#if defined _EXE && defined ETH_INTERFACE && !defined BLINKEY
    {"app",       fnApplication,  MEDIUM_QUE,  (DELAY_LIMIT)((0.5 * SEC) + (PHY_POWERUP_DELAY)), 0, UTASKER_STOP}, // application - start after Ethernet to be sure we have Ethernet handle
#elif !defined BLINKEY
    {"app",       fnApplication,  MEDIUM_QUE,  (DELAY_LIMIT)((0.10 * SEC) + (PHY_POWERUP_DELAY)), 0, UTASKER_STOP}, // application - start after Ethernet to be sure we have Ethernet handle
#endif
#if defined SDCARD_SUPPORT || defined SPI_FLASH_FAT || defined FLASH_FAT || defined MANAGED_FILES || defined USB_MSD_HOST
    {"MassSt",    fnMassStorage,  MEDIUM_QUE,  (DELAY_LIMIT)(NO_DELAY_RESERVE_MONO), 0, UTASKER_STOP}, // mass storage task
#endif
#if defined USE_DHCP_CLIENT
    {"DHCP",      fnDHCP,       SMALL_QUEUE, (DELAY_LIMIT)(NO_DELAY_RESERVE_MONO), 0, UTASKER_STOP}, // delay only for timer queue space
#endif
#if defined USE_DNS
    {"dNS",       fnDNS,        SMALL_QUEUE, (DELAY_LIMIT)(0.05 * SEC), 0, UTASKER_STOP}, // start before application calls a search
#endif
#if defined USE_POP3
    {"POP",       fnPOP3,       SMALL_QUEUE, (DELAY_LIMIT)(0.10 * SEC), 0, UTASKER_STOP},
#endif
#if defined USE_SMTP
    {"SMTP",      fnSmtp,       SMALL_QUEUE, (DELAY_LIMIT)(NO_DELAY_RESERVE_MONO), 0, UTASKER_STOP},
#endif
#if defined USE_IGMP                                                     // {10}
    {"IGMP",      fnIgmp,       SMALL_QUEUE, (DELAY_LIMIT)(NO_DELAY_RESERVE_MONO), 0, UTASKER_STOP},
#endif
#if defined USE_TFTP
    {"tFTP",      fnTftp,       SMALL_QUEUE, (DELAY_LIMIT)(NO_DELAY_RESERVE_MONO), 0, UTASKER_STOP},
#endif
#if defined USE_SNMP
    {"MsnMp",     fnSNMP,       SMALL_QUEUE, (DELAY_LIMIT)(NO_DELAY_RESERVE_MONO), 0, UTASKER_STOP},
#endif
#if defined USE_PPP
    {"Uart_ppp",  fnPPP,        SMALL_QUEUE, (DELAY_LIMIT)(NO_DELAY_RESERVE_MONO), 0, UTASKER_STOP},
#endif
#if defined USE_ZERO_CONFIG
    {"zero",      fnZeroConfig, SMALL_QUEUE, (DELAY_LIMIT)(NO_DELAY_RESERVE_MONO), 0, UTASKER_STOP},
#endif
#if defined SUPPORT_LCD
    {"LCD",       fnLCD,        MEDIUM_QUE,  (DELAY_LIMIT)(NO_DELAY_RESERVE_MONO), 0, UTASKER_STOP},
#elif defined SUPPORT_GLCD || defined SUPPORT_OLED /* || defined SUPPORT_TFT || defined GLCD_COLOR */ // {2}{4}{8}
    #if defined GLCD_COLOR
    {"LCD",       fnLCD,        SMALL_QUEUE, (DELAY_LIMIT)(NO_DELAY_RESERVE_MONO), 0, UTASKER_ACTIVATE}, // runs immediately
    #else
    {"LCD",       fnLCD,        (LARGE_QUE + 128),  (DELAY_LIMIT)(NO_DELAY_RESERVE_MONO), 0, UTASKER_STOP}, // large queue for queuing text
    #endif
#endif
#if defined SUPPORT_KEY_SCAN
    {"Key",       fnKey,        NO_QUE,      (DELAY_LIMIT)(0.4 * SEC), (DELAY_LIMIT)(0.05 * SEC), UTASKER_STOP}, // {3}
#endif
#if defined GLOBAL_TIMER_TASK
    {"period",    fnTimer,      SMALL_QUEUE, (DELAY_LIMIT)(NO_DELAY_RESERVE_MONO), 0, UTASKER_STOP}, // gobal timer task
#endif
#if defined USE_MAINTENANCE
    {"maintenace",fnDebug,      SMALL_QUEUE, (DELAY_LIMIT)(NO_DELAY_RESERVE_MONO), 0, UTASKER_STOP}, // task used for debug messages (started by application)
#endif
#if (defined LAN_REPORT_ACTIVITY || defined PHY_POLL_LINK || defined INTERRUPT_TASK_PHY) && !defined BLINKEY // {9}{11}
    {"NetInd",    fnNetworkIndicator,LARGE_QUE,   (DELAY_LIMIT)(NO_DELAY_RESERVE_MONO), 0, UTASKER_STOP}, // network activity task
#endif
#if defined USB_INTERFACE
    {"usb",       fnTaskUSB,    SMALL_QUEUE, (DELAY_LIMIT)(NO_DELAY_RESERVE_MONO), 0, UTASKER_STOP}, // USB (application) task
#endif
#if (defined USE_SNTP || defined USE_TIME_SERVER || defined USE_TIME_SERVER || defined SUPPORT_RTC || defined SUPPORT_SW_RTC) && !defined BLINKEY // {12}
    {"keeper",    fnTimeKeeper, SMALL_QUEUE, (DELAY_LIMIT)(NO_DELAY_RESERVE_MONO), 0, UTASKER_STOP}, // time keeper task
#endif
#if defined QUICK_DEV_TASKS
    {"1",    fnQuickTask1, MEDIUM_QUE, (DELAY_LIMIT)(NO_DELAY_RESERVE_MONO), 0, UTASKER_STOP}, // quick development  tasks
    {"2",    fnQuickTask2, MEDIUM_QUE, (DELAY_LIMIT)(NO_DELAY_RESERVE_MONO), 0, UTASKER_STOP},
    {"3",    fnQuickTask3, MEDIUM_QUE, (DELAY_LIMIT)(NO_DELAY_RESERVE_MONO), 0, UTASKER_STOP},
    {"4",    fnQuickTask4, MEDIUM_QUE, (DELAY_LIMIT)(NO_DELAY_RESERVE_MONO), 0, UTASKER_STOP},
#endif
#if defined SUPPORT_LOW_POWER
    {"lowPower",  fnLowPower,   NO_QUE,      0, 0, UTASKER_GO},          // low power task
#endif
    {0}
};
#endif                                                                   // end of task configuration

