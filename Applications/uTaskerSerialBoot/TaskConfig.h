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
    Copyright (C) M.J.Butcher Consulting 2004..2016
    *********************************************************************
    07.05.2011 Application task dependent on SERIAL_INTERFACE            {1}
    22.01.2012 Add SD card support                                       {2}
    15.04.2014 Add web server upload support                             {3}
    04.06.2014 Add network indicator task option for PHY polling         {4}

*/
 

/******************************** The tasks used in the system ********************************************/
/* Warning - each task must use a unique letter equivalent to the start of its task name                  */
#define TASK_WATCHDOG           'W'                                      // watchdog task
#define TASK_ETHERNET           'E'                                      // ethernet task
#define TASK_ARP                'A'                                      // ARP task
#define TASK_DHCP               'D'                                      // DHCP task
#define TASK_TFTP               't'                                      // TFTP task
#define TASK_TCP                'T'                                      // TCP task
#define TASK_APPLICATION        'a'                                      // application task
#define TASK_LOW_POWER          'l'                                      // Task supporting power saving
#define TASK_USB                'u'                                      // USB device task
#define TASK_USB_HOST           'H'                                      // USB host task
#define TASK_MASS_STORAGE       'M'                                      // mass storage task
#define TASK_SD_LOADER          'S'                                      // SD loader
#define TASK_NETWORK_INDICATOR  'N'                                      // task displaying network activity

#undef  OWN_TASK


extern void fnTaskWatchdog(TTASKTABLE *);
extern void fnTaskEthernet(TTASKTABLE *);
extern void fnTaskArp(TTASKTABLE *);
extern void fnDHCP(TTASKTABLE *);
extern void fnTftp(TTASKTABLE *);
extern void fnTaskTCP(TTASKTABLE *);
extern void fnApplication(TTASKTABLE *);
extern void fnTaskUSB_host(TTASKTABLE *);
extern void fnTaskUSB(TTASKTABLE *);
extern void fnLowPower(TTASKTABLE *);
extern void fnMassStorage(TTASKTABLE *);
extern void fnSD_loader(TTASKTABLE *);                                   // {2}
extern void fnNetworkIndicator(TTASKTABLE *);




/************ uTasker task table is defined here but only used by the hardware module initiates the system ***********/

#define DEFAULT_NODE_NUMBER    1                                         // we use one fixed node in the system


#if defined OPSYS_CONFIG                                                 // this is only set in the hardware module

CONFIG_LIMIT OurConfigNr = DEFAULT_NODE_NUMBER;                          // in single node system this can be initialised with a fixed value


const HEAP_NEEDS ctOurHeap[] = {
    {DEFAULT_NODE_NUMBER, OUR_HEAP_SIZE},                                // our node requires this amount of heap space
    {0}                                                                  // end
};


const UTASK_TASK ctNodes[] = {                                           // we use a single fixed configuration (single node)
    DEFAULT_NODE_NUMBER,                                                 // configuration our single node

    TASK_WATCHDOG,                                                       // watchdog task
#if defined USE_IP || defined USE_IPV6                                   // {3}
    TASK_ARP,                                                            // ARP task
#endif
#if defined ETH_INTERFACE                                                // {3}
    TASK_ETHERNET,                                                       // ethernet task
    #if defined PHY_POLL_LINK                                            // {4}
    TASK_NETWORK_INDICATOR,                                              // network activity indicator task
    #endif
#endif
#if defined USE_DHCP_CLIENT
    TASK_DHCP,                                                           // DHCP task
#endif
#if defined USE_TFTP
    TASK_TFTP,                                                           // TFTP task
#endif
#if defined USE_TCP                                                      // {3}
    TASK_TCP,                                                            // TCP task
#endif
#if defined USB_INTERFACE
    TASK_USB_HOST,                                                       // USB host task
    TASK_USB,                                                            // USB device task
#endif
#if defined SDCARD_SUPPORT || defined SPI_FLASH_FAT || defined USB_MSD_HOST_LOADER // {2}
    TASK_MASS_STORAGE,                                                   // mass storage task
    TASK_SD_LOADER,
#endif
#if defined SERIAL_INTERFACE || defined ETH_INTERFACE || defined USE_USB_CDC
    TASK_APPLICATION,                                                    // application task
#endif
#if defined SUPPORT_LOW_POWER
    TASK_LOW_POWER,                                                      // low power task
#endif
    0,                                                                   // end of single configuration

    // insert more node configurations here if required
    0                                                                    // end of configuration list
};


#if !defined PHY_POWERUP_DELAY
    #define PHY_POWERUP_DELAY 0
#endif

const UTASKTABLEINIT ctTaskTable[] = {
  // task name,  task routine,   input queue size, start delay, period, initial task state
#if defined _HW_SAM7X
  { "Wdog",      fnTaskWatchdog, NO_QUE,   (DELAY_LIMIT)(0.2 * SEC), (DELAY_LIMIT)(0.2 * SEC),  UTASKER_STOP}, // watchdog task (Note SAM7X is not allowed to start watchdog immediately since it also checks for too fast triggering!!)
#else
  { "Wdog",      fnTaskWatchdog, NO_QUE,   0, (DELAY_LIMIT)(0.1 * SEC),  UTASKER_GO}, // watchdog task (runs immediately and then periodically)
#endif
#if defined USE_IP || defined USE_IPV6                                   // {3} warning - start ARP task before Ethernet. If Ethernet messages are received before ARP table is ready there would be an error..
  { "ARP",       fnTaskArp,      MEDIUM_QUE, (DELAY_LIMIT)(0.05 * SEC), 0, UTASKER_STOP}, // ARP task check periodically state of ARP table
#endif
#if defined ETH_INTERFACE                                                // {3}
  { "Eth",       fnTaskEthernet, (HEADER_LENGTH * 12), (DELAY_LIMIT)((0.05 * SEC) + (PHY_POWERUP_DELAY)), 0, UTASKER_STOP}, // ethernet task - runs automatically
    #if defined PHY_POLL_LINK                                            // {4}
  {"NetInd",    fnNetworkIndicator, NO_QUE,   (DELAY_LIMIT)(2 * SEC), (DELAY_LIMIT)(2 * SEC), UTASKER_STOP}, // network activity task polling PHY state once every 2s (after initial 2s delay)
    #endif
#endif
#if defined USE_DHCP_CLIENT
  {"DHCP",      fnDHCP,          SMALL_QUEUE, (DELAY_LIMIT)(NO_DELAY_RESERVE_MONO), 0, UTASKER_STOP}, // delay only for timer queue space
#endif
#if defined USE_TFTP
  {"tFTP",      fnTftp,          SMALL_QUEUE, (DELAY_LIMIT)(NO_DELAY_RESERVE_MONO), 0, UTASKER_STOP},
#endif
#if defined USE_TCP                                                      // {3}
  { "TCP",       fnTaskTCP,      MEDIUM_QUE,  (DELAY_LIMIT)(0.10 * SEC), 0, UTASKER_STOP}, // TCP task checks periodically state of session timeouts (controlled by task itself)
#endif
#if defined SERIAL_INTERFACE || defined ETH_INTERFACE || defined USE_USB_CDC
  { "app",       fnApplication,  MEDIUM_QUE,  (DELAY_LIMIT)((0.10 * SEC) + (PHY_POWERUP_DELAY)), 0, UTASKER_STOP}, // Application - start after Ethernet to be sure we have Ethernet handle
#endif
#if defined USB_INTERFACE
    #if defined USB_MSD_HOST_LOADER
  { "Host",      fnTaskUSB_host, SMALL_QUEUE, (DELAY_LIMIT)(NO_DELAY_RESERVE_MONO), 0, UTASKER_STOP}, // USB host (application) task (started by the SD loader task when required)
    #endif
    #if !defined USB_MSD_HOST_LOADER || defined USB_MSD_DEVICE_LOADER
  { "usb",       fnTaskUSB,      SMALL_QUEUE, (DELAY_LIMIT)(NO_DELAY_RESERVE_MONO), 0, UTASKER_ACTIVATE}, // USB (application) task
    #endif
#endif
#if defined SDCARD_SUPPORT || defined SPI_FLASH_FAT || defined USB_MSD_HOST_LOADER // {2}
  { "MassSt",    fnMassStorage,  MEDIUM_QUE,  (DELAY_LIMIT)(NO_DELAY_RESERVE_MONO), 0, UTASKER_ACTIVATE}, // mass storage task
    #if defined USB_MSD_HOST_LOADER && defined USB_MSD_DEVICE_LOADER
  { "SD",        fnSD_loader,    SMALL_QUEUE, (DELAY_LIMIT)(NO_DELAY_RESERVE_MONO), 0, UTASKER_STOP}, // SD card loader task (not started automatically)
    #else
  { "SD",        fnSD_loader,    SMALL_QUEUE, (DELAY_LIMIT)(NO_DELAY_RESERVE_MONO), 0, UTASKER_ACTIVATE}, // SD card loader task
    #endif
#endif
#if defined SUPPORT_LOW_POWER
  { "lowPower", fnLowPower,      NO_QUE,      0, 0, UTASKER_GO},         // low power task
#endif
  { 0 }
};
#endif                                                                   // end of task configuration

