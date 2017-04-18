/**********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher

    ---------------------------------------------------------------------
    File:      types.h
    Project:   Single Chip Embedded Internet - serial loader
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2016
    *********************************************************************
    14.02.2010 Remove "../../Hardware/xxxx/xxxx.h" include to app_hw_xxxx.h {1}
    01.12.2010 Add RX6XX                                                 {2}
    04.01.2011 Add Kinetis                                               {3}
    01.02.2011 Add LPC17XX                                               {4}

*/


#ifndef __TYPES__
#define __TYPES__



// Here we have the capability of optimising the types used by the present system hardware
//
typedef unsigned long     UTASK_TICK;                                    // tick counts from 0 to 0xffffffff
typedef unsigned char     CONFIG_LIMIT;                                  // up to 255 configurations possible
typedef unsigned char     NETWORK_LIMIT;                                 // up to 255 nodes possible
typedef unsigned char     TASK_LIMIT;                                    // the system supports up to 255 tasks
typedef unsigned short    STACK_REQUIREMENTS;                            // the system supports up to 64k heap
typedef unsigned short    HEAP_REQUIREMENTS;                             // the system supports up to 64k heap
typedef unsigned char     TIMER_LIMIT;                                   // the system supports up to 255 timers
typedef   signed char     UTASK_TASK;                                    // task reference
typedef unsigned char     QUEUE_LIMIT;                                   // the system supports up to 255 queues
#define QUEUE_HANDLE      QUEUE_LIMIT                                    // as many queue handles as there are queues
typedef unsigned short    QUEUE_TRANSFER;                                // the system supports transfers to 64k bytes
typedef unsigned char     PHYSICAL_Q_LIMIT;                              // the system supports up to 255 physical queues
typedef unsigned short    DELAY_LIMIT;                                   // delays up to 64k TICKs
typedef unsigned short    MAX_MALLOC;                                    // upto 64k heap chunks
typedef unsigned short    LENGTH_CHUNK_COUNT;                            // http string insertion and chunk counter for dynamic generation
//typedef unsigned short  CLOCK_LIMIT;                                   // 16 bit normal hardware timer support
#if defined (_M5223X) || defined (_STR91XF) || defined (_HW_SAM7X) || defined (_LM3SXXXX) || defined (_LPC23XX) || defined (_LPC17XX) || defined _HW_AVR32 || defined _RX6XX || defined _KINETIS // {1}{2}{3}{4}
    typedef unsigned long   MAX_FILE_LENGTH;                             // over 64k file lengths
    typedef unsigned long   MAX_FILE_SYSTEM_OFFSET;                      // offsets of over 64k in file system - use for file system sizes of larger than 64k
#else
    #if defined _HW_NE64 && (defined SPI_FILE_SYSTEM && defined FLASH_FILE_SYSTEM)
        typedef unsigned long   MAX_FILE_LENGTH;                         // up to 64k file lengths
        typedef unsigned long   MAX_FILE_SYSTEM_OFFSET;                  // offsets of up to 64k in file system
    #else
        typedef unsigned short  MAX_FILE_LENGTH;                         // up to 64k file lengths
        typedef unsigned short  MAX_FILE_SYSTEM_OFFSET;                  // offsets of up to 64k in file system
    #endif
//  typedef unsigned short  CLOCK_LIMIT;                                 // 16 bit normal and hardware timer support
#endif

// TCP/IP support
//
typedef signed char       USOCKET;                                       // socket support from 0..127 (negative values are errors)
#define SOCKET_NUMBER_MASK 0x7f


// UART mode
//
typedef unsigned short     UART_MODE_CONFIG;                              // UART mode (use unsigned long for extended mode support)

// MODBUS
//
#define MODBUS_BITS_ELEMENT_WIDTH 8
//#define MODBUS_BITS_ELEMENT_WIDTH 16
//#define MODBUS_BITS_ELEMENT_WIDTH 32
#if MODBUS_BITS_ELEMENT_WIDTH == 32
    typedef unsigned long   MODBUS_BITS_ELEMENT;
#elif MODBUS_BITS_ELEMENT_WIDTH == 16
    typedef unsigned short  MODBUS_BITS_ELEMENT;
#else
    typedef unsigned char   MODBUS_BITS_ELEMENT;
#endif

// General variable types for portability
//
typedef char              CHAR;

#define STRING_OPTIMISATION                                              // activate to optimise return pointers from string output functions. This is not fully compatible with existing project use

#if defined _HW_NE64 && defined SPI_FILE_SYSTEM && defined FLASH_FILE_SYSTEM // treat pointers as long so that extended virtual address range is possible
    #define MEMORY_RANGE_POINTER   unsigned long
#else
    #define MEMORY_RANGE_POINTER   unsigned char *
#endif


#if defined COMPILE_IAR && !defined _HW_SAM7X
    typedef unsigned long size_t;
#else
    typedef unsigned int size_t;
#endif

#if defined _HW_NE64
    #if defined (_CODE_WARRIOR) && !defined (_WINDOWS)
        #include "stdtypes.h"                                            // some non-standard typedefs required with this compiler
    #endif
  //#include "../../Hardware/ne64/ne64.h"                                // {1}
    #include "app_hw_ne64.h"
#endif
#if defined _HW_SAM7X
  //#include "../../Hardware/sam7x/sam7x.h"                              // {1}
    #include "app_hw_sam7x.h"
#endif
#if defined _M5223X
  //#include "../../Hardware/M5223X/M5223X.h"                            // {1}
    #include "app_hw_m5223x.h"
#endif
#if defined _STR91XF
  //#include "../../Hardware/STR91XF/STR91XF.h"                          // {1}
    #include "app_hw_str91xf.h"
#endif
#if defined _LPC23XX
  //#include "../../Hardware/LPC23XX/LPC23XX.h"                          // {1}
    #include "app_hw_lpc23xx.h"
#endif
#if defined _LPC17XX
    #include "app_hw_lpc17xx.h"                                          // {4}
#endif
#if defined _LM3SXXXX
    #include "app_hw_LM3SXXXX.h"
  //#include "../../Hardware/LM3SXXXX/LM3SXXXX.h"                        // {1}
#endif
#if defined _HW_AVR32
    #include "app_hw_AVR32.h"
  //#include "../../Hardware/AVR32/AVR32.h"                              // {1}
#endif
#if defined _RX6XX                                                       // {2}
    #include "app_hw_RX6XX.h"
#endif
#if defined _KINETIS                                                     // {3}
    #include "app_hw_kinetis.h"
#endif
#endif                                                                   // end not defined __TYPES__


