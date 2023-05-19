/**********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher

    ---------------------------------------------------------------------
    File:        types.h
    Project:     Single Chip Embedded Internet
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2021
    *********************************************************************
    02.11.2007 Add typedef MEMORY_RANGE_POINTER for compatibility
    13.03.2008 Add typedef LENGTH_CHUNK_COUNT
    02.04.2008 Add LM3SXXXX support
    31.05.2008 Modify size_t dependency                                  {1}
    02.02.2009 Add UART_MODE_CONFIG for compatibility                    {2}
    04.06.2009 Add AVR32 support
    04.04.2010 Add LPC17XX support
    29.11.2010 Add RX6XX support
    29.03.2011 Add Kinetis support
    27.11.2021 Add STM32 support
   
*/


#ifndef __TYPES__
#define __TYPES__

// Here we have the capability of optimising the types used by the present system hardware


typedef unsigned long     UTASK_TICK;                                    // tick counts from 0 to 0xffffffff
typedef unsigned char     CONFIG_LIMIT;                                  // up to 255 configurations possible
typedef unsigned char     NETWORK_LIMIT;                                 // up to 255 nodes possible
typedef unsigned char     TASK_LIMIT;                                    // the system supports up to 255 tasks
typedef unsigned short    STACK_REQUIREMENTS;                            // the system supports up to 64k heap
typedef unsigned short    HEAP_REQUIREMENTS;                             // the system supports up to 64k heap
typedef   signed char     UTASK_TASK;                                    // task reference
typedef unsigned char     TIMER_LIMIT;                                   // the system supports up to 255 timers
typedef unsigned char     QUEUE_LIMIT;                                   // the system supports up to 255 queues
#define QUEUE_HANDLE      QUEUE_LIMIT                                    // as many queue handles as there are queues
typedef unsigned short    QUEUE_TRANSFER;                                // the system supports transfers to 64k bytes
typedef unsigned char     PHYSICAL_Q_LIMIT;                              // the system supports up to 255 physical queues
typedef unsigned short    DELAY_LIMIT;                                   // delays up to 64k TICKs
typedef unsigned short    MAX_MALLOC;                                    // upto 64k heap chunks
typedef unsigned long     MAX_FILE_LENGTH;                               // over 64k file lengths
typedef unsigned long     MAX_FILE_SYSTEM_OFFSET;                        // offsets of over 64k in file system - use for file system sizes of larger than 64k
typedef unsigned short    LENGTH_CHUNK_COUNT;                            // http string insertion and chunk counter for dynamic generation {}

// TCP/IP support
typedef signed char       USOCKET;                                       // socket support from 0..127 (negative values are errors)

// UART mode
typedef unsigned short    UART_MODE_CONFIG;                              // {2} UART mode (use unsigned long for extended mode support)


// General variable types for protability
typedef char              CHAR;

#if defined _HW_NE64 && defined SPI_SW_UPLOAD                            // treat pointers as long so that extended virtual address range is possible
    #define MEMORY_RANGE_POINTER   unsigned long
#else
    #define MEMORY_RANGE_POINTER   unsigned char *
#endif

#if !defined _CODE_WARRIOR_CF || defined _M5223X_BOOT                    // {1}
    typedef unsigned int size_t;
#endif

#ifdef _HW_NE64
    #include "../../Hardware/ne64/ne64.h"
#endif
#ifdef _HW_SAM7X
    #include "../../Hardware/sam7x/sam7x.h"
#endif
#ifdef _M5223X
    #include "../../Hardware/M5223X/M5223x.h"
#endif
#ifdef _KINETIS
    #include "../../Hardware/Kinetis/kinetis.h"
#endif
#ifdef _STR91XF
    #include "../../Hardware/STR91XF/STR91XF.h"
#endif
#ifdef _LPC23XX
    #include "../../Hardware/LPC23XX/LPC23XX.h"
#endif
#ifdef _LPC17XX
    #include "../../Hardware/LPC17XX/LPC17XX.h"
#endif
#ifdef _LM3SXXXX
    #include "../../Hardware/LM3SXXXX/LM3SXXXX.h"
#endif
#ifdef _HW_AVR32
    #include "../../Hardware/AVR32/AVR32.h"
#endif
#ifdef _RX6XX
    #include "../../Hardware/RX6XX/RX6XX.h"
#endif
#if defined _STM32
    #include "../../Hardware/STM32/STM32.h"
#endif
#endif

