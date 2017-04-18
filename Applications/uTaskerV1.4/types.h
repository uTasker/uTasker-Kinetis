/**********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher

    ---------------------------------------------------------------------
    File:      types.h
    Project:   Single Chip Embedded Internet
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2016
    *********************************************************************
    01.03.2007 Increased SAM7 file system dimensions {1} to allow boot loader to operate
    15.09.2007 Add support for Luminary LM3SXXXX
    12.10.2007 Add typedef MEMORY_RANGE_POINTER
    13.03.2008 Add typedef LENGTH_CHUNK_COUNT
    19.02.2009 Add MODBUS_BITS_ELEMENT
    26.02.2009 Add UART_MODE_CONFIG
    16.04.2009 Add optional STRING_OPTIMISATION - note that this is set as standard from V1.4
    26.04.2009 Add AVR32 hardware header
    07.09.2009 Promote app_hw_sam7x.h over sam7x.h                       {2}
    22.09.2009 Remove "../../Hardware/m5223x/M5223X.h" include to app_hw_m5223x.h {3}
    01.10.2009 {3} generally for all hardware types                      {4}
    15.01.2010 Add LPC17XX                                               {5}
    30.01.2010 Add Flexis32                                              {6}
    18.06.2010 Add STM32                                                 {7}
    29.11.2010 Add RX6XX                                                 {8}
    22.12.2010 Add Kinetis                                               {9}
    09.08.2012 Add multiple networks/interface configuration             {10}
    06.03.2014 Add USER_INFO_MASK and USER_INFO_SHIFT                    {11}
    13.04.2014 Add USOCKET configuration for two networks                {12}

*/


#if !defined __TYPES__
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
#if !defined _HW_NE64                                                    // {1}
    typedef unsigned long   MAX_FILE_LENGTH;                             // over 64k file lengths
    typedef unsigned long   MAX_FILE_SYSTEM_OFFSET;                      // offsets of over 64k in file system - use for file system sizes of larger than 64k
    #if defined GLOBAL_HARDWARE_TIMER
//      typedef unsigned long   CLOCK_LIMIT;                             // 32 bit hardware timer support
    #else
//      typedef unsigned short  CLOCK_LIMIT;                             // 16 bit normal hardware timer support
    #endif
#else
    #if defined _HW_NE64 && (defined SPI_FILE_SYSTEM && defined FLASH_FILE_SYSTEM)
        typedef unsigned long   MAX_FILE_LENGTH;                         // up to 64k file lengths
        typedef unsigned long   MAX_FILE_SYSTEM_OFFSET;                  // offsets of up to 64k in file system
    #else
        typedef unsigned short  MAX_FILE_LENGTH;                         // up to 64k file lengths
        typedef unsigned short  MAX_FILE_SYSTEM_OFFSET;                  // offsets of up to 64k in file system
    #endif
//    typedef unsigned short  CLOCK_LIMIT;                               // 16 bit normal and hardware timer support
#endif

// TCP/IP support
//

// Multi-network/interface socket control                                // {10}
//
// Example of configuration for 2 networks and 3 interfaces - note that USOCKET is usually set to signed short to accommodate this

//#define NETWORK_SHIFT        13                                        // valid for up to 4 networks and USOCKET as signed short
//#define NETWORK_MASK         0x03
//#define INTERFACE_SHIFT      8                                         // valid for up to 5 interfaces and USOCKET as signed short
//#define INTERFACE_MASK       0x07
//#define VLAN_SHIFT           6                                         // valid for up to 3 alternative VLANs and USOCKET as signed short
//#define VLAN_MASK            0x03                                      // [maximum 15 alternative are possible - limit 0x0f]
//#define SOCKET_NUMBER_MASK   0x03f
//#define USER_INFO_SHIFT      11
//#define USER_INFO_MASK       0x03                                      // valid for up to 4 user functions
// 15  14   13   12   11   10    9    8   7    6    5   4   3   2   1   0 //
//------------------------------------------------------------------------//
// V | N1 | N0 | U1 | U0 | I2 | I1 | I0 | V1 | V0 | S | S | S | S | S | S //
//------------------------------------------------------------------------//
// V = valid socket
// Nx = Network that the socket can use - there are four networks available in this example
// Ix = flags for each interface that the socket can use I0, I1, I2, I3 and I4 means that there are 5 physical sockets available in this example
// Vx = virtual lan membership reference (0 is standard virtual LAN and 1, 2, 3 are alternative VLANs)
// S = socket number from 0..0x3f (maximum 64 TCP and 64 UDP sockets possible)
// note that USOCKET would be chosen as signed short to give adequate width for this example

#if IP_NETWORK_COUNT > 1                                                 // {12}
    #if defined USE_SNMP
        typedef signed short         USOCKET;
        #define NETWORK_SHIFT        10
        #define NETWORK_MASK         0x01
        #define USER_INFO_SHIFT      8
        #define USER_INFO_MASK       0x0003
        #define SOCKET_NUMBER_MASK   0x00ff                              // dual-network, 256 UDP and 256 TCP sockets possible - up to 4 user functions
    #else
        #define NETWORK_MASK         0x01                                // 2 networks possible
        #define SOCKET_NUMBER_MASK   0x3f                                // dual-network, 128 UDP and 128 TCP sockets possible
        #define NETWORK_SHIFT        6
        #if IP_INTERFACE_COUNT > 1                                       // multiple interfaces
            typedef signed short     USOCKET;                            // socket support from 0..32k (negative values are errors)            #define INTERFACE_SHIFT  7
            #define INTERFACE_SHIFT  7                                   // single interface
            #define INTERFACE_MASK   0x07                                // three interfaces supported
        #else
            typedef signed char      USOCKET;                            // socket support from 0..127 (negative values are errors)
            #define INTERFACE_SHIFT  0                                   // single interface
            #define INTERFACE_MASK   0
        #endif
    #endif
#else                                                                    // single network
    #if defined USE_SNMP                                                 // {11}
        #if IP_INTERFACE_COUNT > 1
            typedef signed short     USOCKET;                            // socket support from 0..32k (negative values are errors)
            #define INTERFACE_SHIFT  7
            #define INTERFACE_MASK   0x07                                // three interfaces supported
        #else
            typedef signed char      USOCKET;                            // socket support from 0..127 (negative values are errors)
            #define INTERFACE_SHIFT  0                                   // single interface
            #define INTERFACE_MASK   0
        #endif
        #define USER_INFO_SHIFT      5
        #define USER_INFO_MASK       0x03                                // 4 users supported
        #define SOCKET_NUMBER_MASK   0x1f                                // single network and interface with up to 4 user functions (USOCKET can be single byte width)
    #else
        #if IP_INTERFACE_COUNT > 1
            typedef signed char          USOCKET;                        // socket support from 0..63 (negative values are errors)
            #define SOCKET_NUMBER_MASK   0x1f                            // socket mask for 0..31
            #define INTERFACE_SHIFT      5                               // interface bit location
            #define INTERFACE_MASK       0x03                            // two interfaces possible
        #else
            typedef signed char          USOCKET;                        // socket support from 0..127 (negative values are errors)
            #define SOCKET_NUMBER_MASK   0x7f                            // default when using a single network and interface (USOCKET can be single byte width)
            #define INTERFACE_SHIFT      0                               // single interface
            #define INTERFACE_MASK       0
        #endif
    #endif
    #define NETWORK_SHIFT            0                                   // single network
    #define NETWORK_MASK             0
#endif

// UART mode
//
#define UART_EXTENDED_MODE
#if defined UART_EXTENDED_MODE
    typedef unsigned long    UART_MODE_CONFIG;                          // UART mode (use unsigned long for extended mode support)
#else
    typedef unsigned short   UART_MODE_CONFIG;                          // UART mode (use unsigned short for normal mode support)
#endif

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


typedef unsigned int size_t;

#if defined _HW_NE64
    #if defined (_CODE_WARRIOR) && !defined (_WINDOWS)
        #include "stdtypes.h"                                            // some non-standard typedefs required with this compiler
    #endif
    #include "app_hw_ne64.h"
#endif
#if defined _HW_SAM7X
    #include "app_hw_sam7x.h"                                            // {2}
#endif
#if defined _M5223X
    #include "app_hw_m5223x.h"
#endif
#if defined _FLEXIS32
    #include "app_hw_flexis32.h"
#endif
#if defined _STR91XF
    #include "app_hw_str91xf.h"
#endif
#if defined _LPC17XX
    #include "app_hw_lpc17xx.h"                                          // {5}
#endif
#if defined _KINETIS
    #include "app_hw_kinetis.h"                                          // {9}
#endif
#if defined _LPC23XX
    #include "app_hw_lpc23xx.h"
#endif
#if defined _LM3SXXXX
    #include "app_hw_LM3SXXXX.h"
#endif
#if defined _HW_AVR32
    #include "app_hw_AVR32.h"
#endif
#if defined _STM32                                                       // {7}
    #include "app_hw_STM32.h"
#endif
#if defined _RX6XX                                                       // {8}
    #include "app_hw_RX6XX.h"
#endif

#endif                                                                   // end not defined __TYPES__


