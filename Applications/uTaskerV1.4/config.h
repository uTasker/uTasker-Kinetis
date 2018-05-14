/**********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher
    
    ---------------------------------------------------------------------
    File:      config.h
    Project:   uTaskerV1.4 project
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2017
    **********************************************************************
    02.02.2017 Adapt for us tick resolution (_TICK_RESOLUTION)
    
*/


#if !defined __CONFIG__
    #define __CONFIG__

#if !defined _ASSEMBLER_CONFIG                                           // remove all following when used for assembler configuration

/////////////////////////////////////////////////////////////////////////
//                                                                       // new users who would like to see just a blinking LED before enabling the project's many powerful features can set this
//#define BLINKY                                                         // it give simplest scheduling of a single task called at 200ms rate that retriggers the watchdog and toggles respective the board's heartbeat LED
//                                                                       // 
/////////////////////////////////////////////////////////////////////////
//#define QUICK_DEV_TASKS                                                // add 4 additional tasks for simple and quick development use (located at the end of appication.c)

#define _TICK_RESOLUTION     TICK_UNIT_MS(50)                            // 50ms system tick period - max possible at 50MHz SYSTICK would be about 335ms !

#if defined _WINDOWS
    #define MEM_FACTOR 1.0                                               // Windows tends to use more memory so expand heap slightly
#else
    #define MEM_FACTOR 1.0
#endif

//#define _APPLICATION_VALIDATION                                        // support application validation
#define USE_MAINTENANCE                                                  // include some maintenance support for the application and command line interface (remove to reduce project size for special tests or possibly running from limited RAM)
    #define PREVIOUS_COMMAND_BUFFERS  4
    #define MEMORY_DEBUGGER                                              // memory debugger interface (read, write and fill)

//#define MONITOR_PERFORMANCE                                            // support measuring duration of tasks and idle phases

//#define DUSK_AND_DAWN                                                  // support dusk and dawn calculation

#define UNUSED_STACK_PATTERN       0x55                                  // this is the stack fill pattern for recognising maximum stack usage
  //#define UNUSED_HEAP_PATTERN    0xaa                                  // this is the initial unused heap content for usage debugging recognistion

//#define IMMEDIATE_MEMORY_ALLOCATION                                    // immediately allocate all dynamic memory that will be used rather than doing it only when first used
//#define SUPPORT_UFREE                                                  // we allow program memory to be freed when it has stopped
#define UREVERSEMEMCPY                                                   // allow uReverseMemcpy() - useful for shifting buffers to right


// Major hardware dependent settings for this project (choice of board - select only one at a time)
//
//#define FRDM_KE02Z                                                     // E processors Cortex-M0+ (5V robust)
//#define FRDM_KE02Z40M
//#define FRDM_KE04Z
//#define FRDM_KE06Z

//#define TRK_KEA8                                                       // EA ultra-reliable automotive processors Cortex-M0+ - starterTRAK board http://www.utasker.com/kinetis/TRK-KEA8.html
//#define TRK_KEA64                                                      // starterTRAK board http://www.utasker.com/kinetis/TRK-KEA64.html
//#define TRK_KEA128                                                     // starterTRAK board http://www.utasker.com/kinetis/TRK-KEA128.html
//#define FRDM_KEAZN32Q64                                                // freedom board http://www.utasker.com/kinetis/FRDM-KEAZN32Q64.html
//#define FRDM_KEAZ64Q64                                                 // freedom board http://www.utasker.com/kinetis/FRDM-KEAZ64Q64.html
//#define FRDM_KEAZ128Q80                                                // freedom board http://www.utasker.com/kinetis/FRDM-KEAZ128Q80.html

//#define FRDM_KL02Z                                                     // L processors Cortex-M0+ (ultra-low power) basic
//#define FRDM_KL03Z
//#define FRDM_KL05Z

//#define FRDM_KL25Z                                                     // L processors Cortex-M0+ (ultra-low power) with USB
//#define TWR_KL25Z48M
//#define FRDM_KL26Z
//#define rcARM_KL26                                                     // development board with KL26
//#define FRDM_KL27Z
//#define CAPUCCINO_KL27
//#define TEENSY_LC                                                      // USB development board with KL26Z64
//#define FRDM_KL43Z                                                     // L processors Cortex-M0+ (ultra-low power) with USB and segment LCD
//#define KL43Z_256_32_CL                                                // crystal-less reference board
//#define TWR_KL43Z48M
//#define FRDM_KL46Z
//#define TWR_KL46Z48M

//#define TWR_KL82Z72M                                                   // tower board http://www.utasker.com/kinetis/FRDM-KL82Z72M
//#define FRDM_KL82Z                                                     // freedom board http://www.utasker.com/kinetis/FRDM-KL82Z.html

//#define TWR_KM34Z50M                                                   // M processors Cortex M0+ (metrology)

//#define TWR_KV10Z32                                                    // V processors Cortex M0+/M4 (M0+ - motor control and power conversion - low dynamic control)
//#define TWR_KV31F120M                                                  // (M4 - high dynamic control)

//#define TWR_KW21D256                                                   // W processors Cortex M0+/M4 (wireless connectivity)
//#define TWR_KW24D512

//#define K02F100M                                                       // development board with 100MHz K02F
//#define FRDM_K20D50M                                                   // K processors Cortex M4 (performance and integration) with USB
//#define tinyK20                                                        // USB memory stick format board with SD card and 50MHz K20DX128
//#define TWR_K20D50M
//#define TWR_K20D72M
//#define TEENSY_3_1                                                     // USB development board with K20DX256
//#define K20FX512_120                                                   // development board with 120MHz K20
//#define TWR_K21D50M
//#define TWR_K21F120M
//#define FRDM_K22F
//#define K22F128_100M
//#define TWR_K22F120M
//#define BLAZE_K22
//#define TWR_K24F120M

//#define KWIKSTIK                                                       // K processors Cortex M4 with USB and segment LCD
//#define TWR_K40X256
//#define TWR_K40D100M

//#define TWR_K53N512                                                    // K processors Cortex M4 with Ethernet, USB, integrated measurement engine and segment LCD

//#define TWR_K60N512                                                    // K processors Cortex M4 with Ethernet, USB, encryption, tamper
//#define TWR_K60D100M
//#define TWR_K60F120M
//#define K60F150M_50M                                                   // board with 150MHz K60 and 50MHz clock
//#define NET_KBED
//#define NET_K60

//#define EMCRAFT_K61F150M                                               // K processors Cortex M4 with Ethernet, USB, encryption, tamper, key storage protection area

//#define FRDM_K64F                                                      // next generation K processors Cortex M4 with Ethernet, USB, encryption, tamper, key storage protection area - freedom board http://www.utasker.com/kinetis/FRDM-K64F.html
//#define TWR_K64F120M                                                   // tower board http://www.utasker.com/kinetis/TWR-K64F120M.html
//#define TEENSY_3_5                                                     // USB development board with K64FX512 - http://www.utasker.com/kinetis/TEENSY_3.5.html
//#define FreeLON                                                        // K64 based with integrated LON
//#define TWR_K65F180M                                                   // tower board http://www.utasker.com/kinetis/TWR-K65F180M.html
#define FRDM_K66F                                                        // freedom board http://www.utasker.com/kinetis/FRDM-K66F.html
//#define TEENSY_3_6                                                     // USB development board with K66FX1M0 - http://www.utasker.com/kinetis/TEENSY_3.6.html

//#define TWR_K70F120M                                                   // K processors Cortex M4 with graphical LCD, Ethernet, USB, encryption, tamper
//#define EMCRAFT_K70F120M
//#define K70F150M_12M                                                   // development board with 150MHz K70 and 12MHz crystal

//#define FRDM_K82F                                                      // K processors Cortex M4 with USB, encryption, tamper (scalable and secure)
//#define TWR_POS_K81
//#define TWR_K80F150M


// Add some basic details for the board/processor being used
//
#if defined FRDM_KE02Z
    #define TARGET_HW            "FRDM_KE02Z Kinetis"
    #define KINETIS_MAX_SPEED    20000000
    #define OUR_HEAP_SIZE        (HEAP_REQUIREMENTS)((1.5 * 1024) * MEM_FACTOR)
    #define KINETIS_KE
    #define KINETIS_KE02
    #define DEVICE_WITHOUT_CAN                                           // KE02 doesn't have CAN controller
    #define DEVICE_WITHOUT_ETHERNET                                      // KE doesn't have Ethernet controller
    #define DEVICE_WITHOUT_USB                                           // KE doesn't have USB
#elif defined FRDM_KE02Z40M
    #define TARGET_HW            "FRDM-KE02Z40M"
    #define KINETIS_MAX_SPEED    40000000
    #define OUR_HEAP_SIZE        (HEAP_REQUIREMENTS)((1.5 * 1024) * MEM_FACTOR)
    #define KINETIS_KE
    #define KINETIS_KE02
    #define DEVICE_WITHOUT_CAN                                           // KE02 doesn't have CAN controller
    #define DEVICE_WITHOUT_ETHERNET                                      // KE doesn't have Ethernet controller
    #define DEVICE_WITHOUT_USB                                           // KE doesn't have USB
#elif defined FRDM_KE04Z
    #define TARGET_HW            "FRDM_KE04Z"
    #define KINETIS_MAX_SPEED    48000000
    #define OUR_HEAP_SIZE        (HEAP_REQUIREMENTS)((820) * MEM_FACTOR)
    #undef USE_MAINTENANCE                                               // this target has very limited RAM - disable maintenance so that accelerometer operation can bee tested
    #define KINETIS_KE
    #define KINETIS_KE04
    #define DEVICE_WITHOUT_CAN                                           // KE04 doesn't have CAN controller
    #define DEVICE_WITHOUT_ETHERNET                                      // KE doesn't have Ethernet controller
    #define DEVICE_WITHOUT_USB                                           // KE doesn't have USB
#elif defined FRDM_KE06Z
    #define TARGET_HW            "FRDM-KE06Z"
    #define KINETIS_MAX_SPEED    48000000
    #define OUR_HEAP_SIZE        (HEAP_REQUIREMENTS)((3 * 1024) * MEM_FACTOR)
    #define KINETIS_KE
    #define KINETIS_KE06
    #define DEVICE_WITHOUT_ETHERNET                                      // KE doesn't have Ethernet controller
    #define DEVICE_WITHOUT_USB                                           // KE doesn't have USB
#elif defined TRK_KEA8
    #define TARGET_HW            "TRK-KEA8"
    #define KINETIS_MAX_SPEED    48000000
    #define OUR_HEAP_SIZE        (HEAP_REQUIREMENTS)((2 * 1024) * MEM_FACTOR)
    #define KINETIS_KE
    #define KINETIS_KEA
    #define KINETIS_KEA8
    #define DEVICE_WITHOUT_ETHERNET                                      // KEA doesn't have Ethernet controller
    #define DEVICE_WITHOUT_USB                                           // KEA doesn't have USB
#elif defined TRK_KEA64
    #define TARGET_HW            "TRK-KEA64"
    #define KINETIS_MAX_SPEED    40000000
    #define OUR_HEAP_SIZE        (HEAP_REQUIREMENTS)((2 * 1024) * MEM_FACTOR)
    #define KINETIS_KE
    #define KINETIS_KEA
    #define KINETIS_KEA64
    #define KINETIS_KEAN64                                               // N variation
    #define DEVICE_WITHOUT_ETHERNET                                      // KEA doesn't have Ethernet controller
    #define DEVICE_WITHOUT_USB                                           // KEA doesn't have USB
#elif defined TRK_KEA128
    #define TARGET_HW            "TRK-KEA128"
    #define KINETIS_MAX_SPEED    48000000
    #define OUR_HEAP_SIZE        (HEAP_REQUIREMENTS)((8 * 1024) * MEM_FACTOR)
    #define KINETIS_KE
    #define KINETIS_KEA
    #define KINETIS_KEA128
    #define DEVICE_WITHOUT_ETHERNET                                      // KEA doesn't have Ethernet controller
    #define DEVICE_WITHOUT_USB                                           // KEA doesn't have USB
#elif defined FRDM_KEAZN32Q64
    #define TARGET_HW            "FRDM-KEAZN32Q64"
    #define KINETIS_MAX_SPEED    40000000                                // 40MHz version
    #define OUR_HEAP_SIZE        (HEAP_REQUIREMENTS)((2 * 1024) * MEM_FACTOR)
    #define KINETIS_KE
    #define KINETIS_KEA
    #define KINETIS_KEA32
    #define DEVICE_WITHOUT_ETHERNET                                      // KEA doesn't have Ethernet controller
    #define DEVICE_WITHOUT_USB                                           // KEA doesn't have USB
#elif defined FRDM_KEAZ64Q64
    #define TARGET_HW            "FRDM-KEAZ64Q64"
    #define KINETIS_MAX_SPEED    48000000
    #define OUR_HEAP_SIZE        (HEAP_REQUIREMENTS)((2 * 1024) * MEM_FACTOR)
    #define KINETIS_KE
    #define KINETIS_KEA
    #define KINETIS_KEA64
    #define DEVICE_WITHOUT_ETHERNET                                      // KEA doesn't have Ethernet controller
    #define DEVICE_WITHOUT_USB                                           // KEA doesn't have USB
#elif defined FRDM_KEAZ128Q80
    #define TARGET_HW            "FRDM-KEAZ128Q80"
    #define KINETIS_MAX_SPEED    48000000
    #define OUR_HEAP_SIZE        (HEAP_REQUIREMENTS)((8 * 1024) * MEM_FACTOR)
    #define KINETIS_KE
    #define KINETIS_KEA
    #define KINETIS_KEA128
    #define DEVICE_WITHOUT_ETHERNET                                      // KEA doesn't have Ethernet controller
    #define DEVICE_WITHOUT_USB                                           // KEA doesn't have USB
#elif defined FRDM_KL02Z
    #define TARGET_HW            "FRDM-KL02Z Kinetis"
    #undef MONITOR_PERFORMANCE                                           // KL02 has no PIT
    #define OUR_HEAP_SIZE        (HEAP_REQUIREMENTS)((2 * 1024) * MEM_FACTOR)
    #define KINETIS_KL
    #define KINETIS_KL02
    #define DEVICE_WITHOUT_CAN                                           // KL doesn't have CAN controller
    #define DEVICE_WITHOUT_ETHERNET                                      // KL doesn't have Ethernet controller
    #define DEVICE_WITHOUT_USB                                           // KL00 doesn't have USB
#elif defined FRDM_KL03Z
    #define TARGET_HW            "FRDM-KL03Z Kinetis"
    #define OUR_HEAP_SIZE        (HEAP_REQUIREMENTS)((1.0 * 1024) * MEM_FACTOR)
    #undef MONITOR_PERFORMANCE                                           // KL03 has no PIT
    #define KINETIS_KL
    #define KINETIS_KL03
    #define DEVICE_WITHOUT_CAN                                           // KL doesn't have CAN controller
    #define DEVICE_WITHOUT_ETHERNET                                      // KL doesn't have Ethernet controller
    #define DEVICE_WITHOUT_USB                                           // KL00 doesn't have USB
#elif defined FRDM_KL05Z
    #define TARGET_HW            "FRDM_KL05Z Kinetis"
    #define OUR_HEAP_SIZE        (HEAP_REQUIREMENTS)((2 * 1024) * MEM_FACTOR)
    #define KINETIS_KL
    #define KINETIS_KL05
    #define DEVICE_WITHOUT_CAN                                           // KL doesn't have CAN controller
    #define DEVICE_WITHOUT_ETHERNET                                      // KL doesn't have Ethernet controller
    #define DEVICE_WITHOUT_USB                                           // KL00 doesn't have USB
#elif defined TWR_KL25Z48M
    #define TARGET_HW            "TWR-KL25Z48M Kinetis"
    #define OUR_HEAP_SIZE        (HEAP_REQUIREMENTS)((10 * 1024) * MEM_FACTOR)
    #define KINETIS_KL
    #define KINETIS_KL25
    #define DEVICE_WITHOUT_CAN                                           // KL doesn't have CAN controller
    #define DEVICE_WITHOUT_ETHERNET                                      // KL doesn't have Ethernet controller
#elif defined FRDM_KL25Z
  //#define FRDM_FXS_MULTI_B                                             // use connections for FRDM-FXS-MULTI-B
    #if defined FRDM_FXS_MULTI_B
        #define TARGET_HW        "FRDM-KL25Z + FRDM-FXS-MULTI-B"
    #else
        #define TARGET_HW        "FRDM-KL25Z"
    #endif
    #define OUR_HEAP_SIZE        (HEAP_REQUIREMENTS)((12 * 1024) * MEM_FACTOR)
    #define KINETIS_KL
    #define KINETIS_KL25
    #define DEVICE_WITHOUT_CAN                                           // KL doesn't have CAN controller
    #define DEVICE_WITHOUT_ETHERNET                                      // KL doesn't have Ethernet controller
#elif defined FRDM_KL26Z
    #define TARGET_HW            "FRDM-KL26Z"
    #define OUR_HEAP_SIZE        (HEAP_REQUIREMENTS)((10 * 1024) * MEM_FACTOR)
    #define KINETIS_KL
    #define KINETIS_KL26
    #define DEVICE_WITHOUT_CAN                                           // KL doesn't have CAN controller
    #define DEVICE_WITHOUT_ETHERNET                                      // KL doesn't have Ethernet controller
#elif defined rcARM_KL26
    #define TARGET_HW            "FRDM-KL26Z"
    #define OUR_HEAP_SIZE        (HEAP_REQUIREMENTS)((10 * 1024) * MEM_FACTOR)
    #define KINETIS_KL
    #define KINETIS_KL26
    #define DEVICE_WITHOUT_CAN                                           // KL doesn't have CAN controller
    #define DEVICE_WITHOUT_ETHERNET                                      // KL doesn't have Ethernet controller
    #define SPI_LCD                                                      // optional SPI_LCD
#elif defined FRDM_KL27Z
    #define TARGET_HW            "FRDM-KL27Z"
    #define OUR_HEAP_SIZE        (HEAP_REQUIREMENTS)((10 * 1024) * MEM_FACTOR)
    #define KINETIS_KL
    #define KINETIS_KL27
    #define DEVICE_WITHOUT_CAN                                           // KL doesn't have CAN controller
    #define DEVICE_WITHOUT_ETHERNET                                      // KL doesn't have Ethernet controller
#elif defined CAPUCCINO_KL27
    #define TARGET_HW            "Capuccino KL27"
    #define OUR_HEAP_SIZE        (HEAP_REQUIREMENTS)((10 * 1024) * MEM_FACTOR)
    #define KINETIS_KL
    #define KINETIS_KL27
    #define DEVICE_WITHOUT_CAN                                           // KL doesn't have CAN controller
    #define DEVICE_WITHOUT_ETHERNET                                      // KL doesn't have Ethernet controller
#elif defined TEENSY_LC
    #define KINETIS_KL
    #define KINETIS_KL26
    #define TARGET_HW            "Teensy LC"
    #define OUR_HEAP_SIZE        (HEAP_REQUIREMENTS)((5.0 * 1024) * MEM_FACTOR)
    #define DEVICE_WITHOUT_ETHERNET                                      // KL doesn't have Ethernet controller
#elif defined TWR_KL46Z48M
    #define TARGET_HW            "TWR-KL46Z48M"
    #define OUR_HEAP_SIZE        (HEAP_REQUIREMENTS)((12 * 1024) * MEM_FACTOR)
    #define KINETIS_KL
    #define KINETIS_KL40                                                 // specify the sub-family type due to SLCD capability
    #define KINETIS_KL46
    #define DEVICE_WITHOUT_CAN                                           // KL doesn't have CAN controller
    #define DEVICE_WITHOUT_ETHERNET
#elif defined FRDM_KL82Z
    #define KINETIS_KL
    #define KINETIS_KL82
    #define TARGET_HW       "FRDM-KL82Z"
    #define OUR_HEAP_SIZE   (HEAP_REQUIREMENTS)((12 * 1024) * MEM_FACTOR)
    #define DEVICE_WITHOUT_CAN                                           // KL doesn't have CAN controller
    #define DEVICE_WITHOUT_ETHERNET                                      // KL doesn't have Ethernet controller
#elif defined TWR_KV10Z32
    #define TARGET_HW            "TWR-KV10Z32"
    #define OUR_HEAP_SIZE        (HEAP_REQUIREMENTS)((5 * 1024) * MEM_FACTOR)
    #define KINETIS_KV
    #define KINETIS_KV10                                                 // specify the sub-family type
    #define DEVICE_WITHOUT_USB
    #define DEVICE_WITHOUT_CAN
    #define DEVICE_WITHOUT_ETHERNET
#elif defined TWR_KV31F120M
    #define TARGET_HW            "TWR-KV31F120M"
    #define OUR_HEAP_SIZE        (HEAP_REQUIREMENTS)((32 * 1024) * MEM_FACTOR)
    #define KINETIS_MAX_SPEED    120000000
    #define KINETIS_K_FPU                                                // part with floating point unit
    #define KINETIS_KV
    #define KINETIS_KV30                                                 // specify the sub-family type
    #define KINETIS_KV31
    #define DEVICE_WITHOUT_USB
    #define DEVICE_WITHOUT_CAN
    #define DEVICE_WITHOUT_ETHERNET
    #define KINETIS_REVISION_2
#elif defined TWR_KL43Z48M
    #define TARGET_HW            "TWR-KL43Z48M"
    #define OUR_HEAP_SIZE        (HEAP_REQUIREMENTS)((16 * 1024) * MEM_FACTOR)
    #define KINETIS_KL
    #define KINETIS_KL40                                                 // specify the sub-family type due to SLCD capability
    #define KINETIS_KL43
    #define DEVICE_WITHOUT_CAN                                           // KL doesn't have CAN controller
    #define DEVICE_WITHOUT_ETHERNET                                      // KL doesn't have Ethernet controller
#elif defined FRDM_KL43Z || defined KL43Z_256_32_CL
    #define TARGET_HW            "FRDM-KL43Z"
    #define OUR_HEAP_SIZE        (HEAP_REQUIREMENTS)((18 * 1024) * MEM_FACTOR)
    #define KINETIS_KL
    #define KINETIS_KL40                                                 // specify the sub-family type due to SLCD capability
    #define KINETIS_KL43
    #define DEVICE_WITHOUT_CAN                                           // KL doesn't have CAN controller
    #define DEVICE_WITHOUT_ETHERNET                                      // KL doesn't have Ethernet controller
    #if defined KL43Z_256_32_CL
        #define QUICK_DEV_TASKS
        #define STR_EQUIV_ON
    #endif
#elif defined FRDM_KL46Z
    #define TARGET_HW            "FRDM-KL46Z"
    #define OUR_HEAP_SIZE        (HEAP_REQUIREMENTS)((12 * 1024) * MEM_FACTOR)
    #define KINETIS_KL
    #define KINETIS_KL40                                                 // specify the sub-family type due to SLCD capability
    #define KINETIS_KL46
    #define DEVICE_WITHOUT_CAN                                           // KL doesn't have CAN controller
    #define DEVICE_WITHOUT_ETHERNET                                      // KL doesn't have Ethernet controller
#elif defined TWR_KW21D256
    #define KINETIS_KW2X
    #define KINETIS_KW21
    #define KINETIS_REVISION_2
    #define KINETIS_MAX_SPEED    50000000
    #define TARGET_HW       "TWR-KW21D256"
    #define DEVICE_WITHOUT_ETHERNET                                      // KW doesn't have Ethernet controller
    #define DEVICE_WITHOUT_USB                                           // KW21 doesn't have USB
    #define OUR_HEAP_SIZE   (HEAP_REQUIREMENTS)((16 * 1024) * MEM_FACTOR)
#elif defined TWR_KW24D512
    #define KINETIS_KW2X
    #define KINETIS_KW24
    #define KINETIS_REVISION_2
    #define KINETIS_MAX_SPEED    50000000
    #define TARGET_HW       "TWR-KW24D512"
    #define DEVICE_WITHOUT_ETHERNET                                      // KW doesn't have Ethernet controller
    #define OUR_HEAP_SIZE   (HEAP_REQUIREMENTS)((16 * 1024) * MEM_FACTOR)
#elif defined K02F100M
    #define TARGET_HW            "K02F100M"
    #define KINETIS_K_FPU                                                // part with floating point unit
    #define KINETIS_K00                                                  // specify the sub-family
    #define KINETIS_K02                                                  // extra sub-family type precision
    #define KINETIS_MAX_SPEED    100000000
    #define DEVICE_WITHOUT_ETHERNET                                      // K02F doesn't have Ethernet controller
    #define DEVICE_WITHOUT_USB                                           // K02F doesn't have USB
    #define DEVICE_WITHOUT_CAN                                           // K02F doesn't have CAN controller
    #define OUR_HEAP_SIZE        (HEAP_REQUIREMENTS)((10 * 1024) * MEM_FACTOR)
#elif defined FRDM_K20D50M
    #define TARGET_HW            "FRDM-K20D50M"
    #define DEVICE_WITHOUT_CAN                                           // 50MHz K20 doesn't have CAN controller
    #define KINETIS_K20                                                  // specify the sub-family
    #define KINETIS_REVISION_2
    #define KINETIS_MAX_SPEED    50000000
    #define DEVICE_WITHOUT_ETHERNET                                      // K20 doesn't have Ethernet controller
    #define OUR_HEAP_SIZE        (HEAP_REQUIREMENTS)((10 * 1024) * MEM_FACTOR)
#elif defined tinyK20
    #define TARGET_HW            "tinyK20"
    #define DEVICE_WITHOUT_CAN                                           // 50MHz K20 doesn't have CAN controller
    #define KINETIS_K20                                                  // specify the sub-family
    #define KINETIS_REVISION_2
    #define KINETIS_MAX_SPEED    50000000
    #define DEVICE_WITHOUT_ETHERNET                                      // K20 doesn't have Ethernet controller
    #define OUR_HEAP_SIZE        (HEAP_REQUIREMENTS)((10 * 1024) * MEM_FACTOR)
#elif defined TWR_K20D50M
    #define TARGET_HW            "TWR-K20N50M"
    #define DEVICE_WITHOUT_CAN                                           // 50MHz K20 doesn't have CAN controller
    #define KINETIS_MAX_SPEED    50000000
    #define KINETIS_K20                                                  // specify the sub-family
    #define KINETIS_REVISION_2
    #define DEVICE_WITHOUT_ETHERNET                                      // K20 doesn't have Ethernet controller
    #define OUR_HEAP_SIZE        (HEAP_REQUIREMENTS)((10 * 1024) * MEM_FACTOR)
#elif defined TWR_K21D50M
    #define TARGET_HW            "TWR-K21D50M Kinetis"
    #define KINETIS_MAX_SPEED    50000000
    #define DEVICE_WITHOUT_CAN                                           // 50MHz K201 doesn't have CAN controller
    #define KINETIS_K20                                                  // specify the sub-family
    #define KINETIS_REVISION_2
    #define KINETIS_K21                                                  // extra sub-family type precision
    #define DEVICE_WITHOUT_ETHERNET                                      // K20 doesn't have Ethernet controller
    #define OUR_HEAP_SIZE        (HEAP_REQUIREMENTS)((16 * 1024) * MEM_FACTOR)
#elif defined TWR_K20D72M
    #define TARGET_HW            "TWR-K20D72M Kinetis"
    #define KINETIS_K20                                                  // specify the sub-family
    #define KINETIS_REVISION_2
    #define KINETIS_MAX_SPEED    72000000
    #define DEVICE_WITHOUT_ETHERNET                                      // K20 doesn't have Ethernet controller
    #define OUR_HEAP_SIZE        (HEAP_REQUIREMENTS)((20 * 1024) * MEM_FACTOR)
#elif defined TEENSY_3_1
    #define TARGET_HW            "TEENSY 3.1 (K20DX256)"
    #define KINETIS_MAX_SPEED    72000000
    #define KINETIS_K20                                                  // specify the sub-family
    #define KINETIS_REVISION_2
    #define DEVICE_WITHOUT_ETHERNET                                      // K20 doesn't have Ethernet controller
    #define OUR_HEAP_SIZE        (HEAP_REQUIREMENTS)((20 * 1024) * MEM_FACTOR)
#elif defined K20FX512_120
    #define KINETIS_K20                                                  // specify the sub-family
    #define KINETIS_K_FPU                                                // part with floating point unit
    #define KINETIS_MAX_SPEED    120000000
    #define KINETIS_REVISION_2
    #define TARGET_HW            "Kinetis K20FX512-120"
    #define OUR_HEAP_SIZE        (HEAP_REQUIREMENTS)((32 * 1024) * MEM_FACTOR)
    #define DEVICE_WITHOUT_ETHERNET                                      // K20 doesn't have Ethernet controller
#elif defined TWR_K21F120M
    #define KINETIS_K_FPU                                                // part with floating point unit
    #define TARGET_HW            "TWR-K21F120M Kinetis"
    #define KINETIS_MAX_SPEED    120000000
    #define KINETIS_K20                                                  // specify the sub-family
    #define KINETIS_K21                                                  // extra sub-family type precision
    #define KINETIS_REVISION_2
    #define DEVICE_WITHOUT_ETHERNET                                      // K20 doesn't have Ethernet controller
    #define OUR_HEAP_SIZE        (HEAP_REQUIREMENTS)((24 * 1024) * MEM_FACTOR)
#elif defined TWR_K22F120M
    #define TARGET_HW            "TWR-K22F120M"
    #define KINETIS_K_FPU                                                // part with floating point unit
    #define KINETIS_K20                                                  // specify the sub-family
    #define KINETIS_K22                                                  // extra sub-family type precision
    #define KINETIS_REVISION_2
    #define KINETIS_MAX_SPEED    120000000
    #define DEVICE_WITHOUT_ETHERNET                                      // K20 doesn't have Ethernet controller
    #define OUR_HEAP_SIZE        (HEAP_REQUIREMENTS)((24 * 1024) * MEM_FACTOR)
#elif defined FRDM_K22F
    #define TARGET_HW            "FRDM-K22F"
    #define KINETIS_K_FPU                                                // part with floating point unit
    #define KINETIS_MAX_SPEED    120000000
    #define KINETIS_K20                                                  // specify the sub-family
    #define KINETIS_K22                                                  // extra sub-family type precision
    #define KINETIS_REVISION_2
    #define DEVICE_WITHOUT_ETHERNET                                      // K20 doesn't have Ethernet controller
    #define OUR_HEAP_SIZE        (HEAP_REQUIREMENTS)((24 * 1024) * MEM_FACTOR)
#elif defined K22F128_100M
    #define TARGET_HW            "K22F128-100M"
    #define KINETIS_K_FPU                                                // part with floating point unit
    #define KINETIS_MAX_SPEED    100000000
    #define KINETIS_K20                                                  // specify the sub-family
    #define KINETIS_K22                                                  // extra sub-family type precision
    #define KINETIS_REVISION_2
    #define DEVICE_WITHOUT_ETHERNET                                      // K20 doesn't have Ethernet controller
    #define OUR_HEAP_SIZE        (HEAP_REQUIREMENTS)((12 * 1024) * MEM_FACTOR)
#elif defined BLAZE_K22
    #define TARGET_HW            "BLAZE"
    #define KINETIS_K_FPU                                                // part with floating point unit
    #define KINETIS_MAX_SPEED    120000000
    #define KINETIS_K20                                                  // specify the sub-family
    #define KINETIS_K22                                                  // extra sub-family type precision
    #define KINETIS_REVISION_2
    #define DEVICE_WITHOUT_ETHERNET                                      // K20 doesn't have Ethernet controller
    #define OUR_HEAP_SIZE        (HEAP_REQUIREMENTS)((24 * 1024) * MEM_FACTOR)
#elif defined TWR_K24F120M
    #define TARGET_HW            "TWR-K24F120M"
    #define KINETIS_K_FPU                                                // part with floating point unit
    #define KINETIS_MAX_SPEED    120000000
    #define KINETIS_K20                                                  // specify the sub-family
    #define KINETIS_REVISION_2
    #define KINETIS_K24                                                  // extra sub-family type precision
    #define DEVICE_WITHOUT_ETHERNET                                      // K20 doesn't have Ethernet controller
    #define OUR_HEAP_SIZE        (HEAP_REQUIREMENTS)((24 * 1024) * MEM_FACTOR)
#elif defined KWIKSTIK
    #define KWIKSTIK_V3_V4                                               // when using versions 3 or 4 the SDHC cannot be used due to a wiring error
    #define TARGET_HW            "KWIKSTIK K40"
    #define KINETIS_K40                                                  // specify the sub-family
    #define OUR_HEAP_SIZE        (HEAP_REQUIREMENTS)((24 * 1024) * MEM_FACTOR)
    #define DEVICE_WITHOUT_ETHERNET                                      // K40 doesn't have Ethernet controller
#elif defined TWR_K40D100M
    #define DEBUG_ON_VIRT_COM                                            // optionally set UART debug on virtual COM rather than the serial board
    #define TARGET_HW            "TWR-K40D100M Kinetis"
    #define KINETIS_K40                                                  // specify the sub-family
    #define KINETIS_REVISION_2                                           // revision 2 mask (from 2N22D - see AN4445)
    #define OUR_HEAP_SIZE        (HEAP_REQUIREMENTS)((24 * 1024) * MEM_FACTOR)
    #define DEVICE_WITHOUT_ETHERNET                                      // K40 doesn't have Ethernet controller
#elif defined TWR_K40X256
    #define TARGET_HW            "TWR-K40X256 Kinetis"
    #define KINETIS_K40                                                  // specify the sub-family
    #define OUR_HEAP_SIZE        (HEAP_REQUIREMENTS)((24 * 1024) * MEM_FACTOR)
    #define DEBUG_ON_VIRT_COM                                            // optionally set UART debug on virtual COM rather than the serial board
    #define DEVICE_WITHOUT_ETHERNET                                      // K40 doesn't have Ethernet controller
#elif defined TWR_K53N512
    #define TARGET_HW            "TWR-K53N512 Kinetis"
    #define OUR_HEAP_SIZE        (HEAP_REQUIREMENTS)((32 * 1024) * MEM_FACTOR)
    #define KINETIS_K50                                                  // specify the sub-family
    #define KINETIS_K53                                                  // extra sub-family type precision
#elif defined TWR_K60F120M
    #define TARGET_HW            "TWR-K60F120M Kinetis"
    #define KINETIS_K_FPU                                                // part with floating point unit
  //#define TWR_SER2                                                     // use SER2 serial board instead of standard serial board
    #define KINETIS_K60                                                  // specify the sub-family
    #define OUR_HEAP_SIZE        (HEAP_REQUIREMENTS)((36 * 1024) * MEM_FACTOR) // we have the LAN buffers in HEAP and big RX/TX - a little more for USB
#elif defined K60F150M_50M
    #define TARGET_HW            "K60F150M_50M"
    #define KINETIS_K_FPU                                                // part with floating point unit
    #define KINETIS_K60
    #define KINETIS_MAX_SPEED    150000000
    #define KINETIS_REVISION_2
    #define OUR_HEAP_SIZE   (HEAP_REQUIREMENTS)((62 * 1024) * MEM_FACTOR)
#elif defined NET_KBED
  //#define KBED_FPGA                                                    // KBED wit FPGA option
    #define KBEDM_BOARD	                                                 // KBEDM board for KBED
  //#define SPI_LCD                                                      // optional SPI_LCD
  //#define TESTBED_BOARD	                                             // testBed Board for KBED
  //#define MXBASE_BOARD                                                 // MX-Base Board for KBED
    #define NET_I2C_8E8A                                                 // I2C I/O module connected
    #if defined KBEDM_BOARD
        //#define KBEDM_4IO                                              // KBEDM4IO sub-board for KBEDM
        #define TARGET_HW        "KBED-KBEDM Kinetis"
    #elif defined TESTBED_BOARD
        #define TARGET_HW        "KBED-TestBed Kinetis"
    #elif defined MXBASE_BOARD
        #define TARGET_HW        "KBED-MX-Base Kinetis"
    #else
        #define TARGET_HW        "KBED-NoBoard Kinetis"
    #endif
    #define KINETIS_K60                                                  // specify the sub-family
    #define OUR_HEAP_SIZE        (HEAP_REQUIREMENTS)((32 * 1024) * MEM_FACTOR) // we have the LAN buffers in HEAP and big RX/TX - a little more for USB
#elif defined NET_K60
    #define DEVICE_WITHOUT_USB                                           // NETK60 doesn't have USB
  //#define SUPPORT_NET3A4IO                                             // enable NET-3A4IO support (ADCs)
    #define TARGET_HW            "NET-K60N512 Kinetis"
    #define KINETIS_K60                                                  // specify the sub-family
    #define OUR_HEAP_SIZE        (HEAP_REQUIREMENTS)((32 * 1024) * MEM_FACTOR) // we have the LAN buffers in HEAP and big RX/TX - a little more for USB
#elif defined TWR_K60D100M
  //#define TWR_SER2                                                     // use SER2 serial board instead of standard serial board
    #define TARGET_HW            "TWR-K60D100M"
    #define KINETIS_K60                                                  // specify the sub-family
    #define KINETIS_REVISION_2
    #define OUR_HEAP_SIZE        (HEAP_REQUIREMENTS)((32 * 1024) * MEM_FACTOR) // we have the LAN buffers in HEAP and big RX/TX - a little more for USB
#elif defined TWR_K60N512
  //#define TWR_SER2                                                     // use SER2 serial board instead of standard serial board
  //#define DEBUG_ON_VIRT_COM                                            // optionally set UART debug on virtual COM rather than the serial board
    #define TARGET_HW            "TWR-K60N512"
    #define KINETIS_K60                                                  // specify the sub-family
    #define OUR_HEAP_SIZE        (HEAP_REQUIREMENTS)((32 * 1024) * MEM_FACTOR) // we have the LAN buffers in HEAP and big RX/TX - a little more for USB
#elif defined EMCRAFT_K61F150M
    #define KINETIS_K_FPU                                                // part with floating point unit
    #define KINETIS_MAX_SPEED    150000000                               // 150MHz part
    #define KINETIS_K60                                                  // specify the sub-family
    #define KINETIS_K61                                                  // extra sub-family type precision
    #define TARGET_HW            "EMCRAFT-K61F150M"
    #define OUR_HEAP_SIZE        (HEAP_REQUIREMENTS)((30 * 1024) * MEM_FACTOR)
#elif defined FreeLON
    #define TARGET_HW            "FreeLON"
    #define OUR_HEAP_SIZE        (HEAP_REQUIREMENTS)((48 * 1024) * MEM_FACTOR) // large SRAM parts
    #define KINETIS_MAX_SPEED    120000000
    #define KINETIS_K_FPU                                                // part with floating point unit
    #define KINETIS_K60                                                  // specify the sub-family
    #define KINETIS_REVISION_2
    #define KINETIS_K64                                                  // extra sub-family type precision
#elif defined FRDM_K66F
    #define TARGET_HW            "FRDM-K66F"
    #define OUR_HEAP_SIZE        (HEAP_REQUIREMENTS)((48 * 1024) * MEM_FACTOR) // large SRAM parts
    #define KINETIS_MAX_SPEED    180000000
    #define KINETIS_K_FPU                                                // part with floating point unit
    #define KINETIS_K60                                                  // specify the sub-family
    #define KINETIS_REVISION_2
    #define KINETIS_K66                                                  // extra sub-family type precision
    #define USB_HS_INTERFACE                                             // use HS interface rather than FS interface
#elif defined TEENSY_3_6
    #define TARGET_HW            "Teensy 3.6 (K66FX1M0)"
    #define OUR_HEAP_SIZE        (HEAP_REQUIREMENTS)((48 * 1024) * MEM_FACTOR) // large SRAM parts
    #define KINETIS_MAX_SPEED    180000000
    #define KINETIS_K_FPU                                                // part with floating point unit
    #define KINETIS_K60                                                  // specify the sub-family
    #define KINETIS_REVISION_2
    #define KINETIS_K66                                                  // extra sub-family type precision
    #define USB_HS_INTERFACE                                             // use HS interface rather than FS interface
#elif defined FRDM_K64F
    #define TARGET_HW            "FRDM-K64F"
    #define OUR_HEAP_SIZE        (HEAP_REQUIREMENTS)((48 * 1024) * MEM_FACTOR) // large SRAM parts
    #define KINETIS_MAX_SPEED    120000000
    #define KINETIS_K_FPU                                                // part with floating point unit
    #define KINETIS_K60                                                  // specify the sub-family
    #define KINETIS_REVISION_2
    #define KINETIS_K64                                                  // extra sub-family type precision
#elif defined TWR_K64F120M
  //#define TWR_SER                                                      // use TWR-SER serial board instead of OpenSDA virtual COM port
    #define TARGET_HW            "TWR-K64F120M"
    #define OUR_HEAP_SIZE        (HEAP_REQUIREMENTS)((48 * 1024) * MEM_FACTOR) // large SRAM parts
    #define KINETIS_MAX_SPEED    120000000
    #define KINETIS_K_FPU                                                // part with floating point unit
    #define KINETIS_K60                                                  // specify the sub-family
    #define KINETIS_REVISION_2
    #define KINETIS_K64                                                  // extra sub-family type precision
#elif defined TEENSY_3_5
    #define TARGET_HW            "Teensy 3.5 (K64FX512)"
    #define OUR_HEAP_SIZE        (HEAP_REQUIREMENTS)((48 * 1024) * MEM_FACTOR) // large SRAM parts
    #define KINETIS_MAX_SPEED    120000000
    #define KINETIS_K_FPU                                                // part with floating point unit
    #define KINETIS_K60                                                  // specify the sub-family
    #define KINETIS_REVISION_2
    #define KINETIS_K64                                                  // extra sub-family type precision
#elif defined TWR_K65F180M
  //#define TWR_SER                                                      // use TWR-SER serial board instead of OpenSDA virtual COM port
    #define TARGET_HW            "TWR-K65F180M"
    #define OUR_HEAP_SIZE        (HEAP_REQUIREMENTS)((48 * 1024) * MEM_FACTOR) // large SRAM parts
    #define KINETIS_MAX_SPEED    180000000
    #define KINETIS_K_FPU                                                // part with floating point unit
    #define KINETIS_K60                                                  // specify the sub-family
    #define KINETIS_REVISION_2
    #define KINETIS_K65                                                  // extra sub-family type precision
    #if !defined TWR_SER
        #define USB_HS_INTERFACE                                         // use HS interface rather than FS interface
    #endif
#elif defined EMCRAFT_K70F120M
    #define TARGET_HW           "EMCRAFT-K70F120M"
    #define KINETIS_MAX_SPEED    120000000
    #define KINETIS_K70                                                  // specify the sub-family
    #define KINETIS_K_FPU                                                // part with floating point unit
    #define OUR_HEAP_SIZE        (HEAP_REQUIREMENTS)((30 * 1024) * MEM_FACTOR)
#elif defined K70F150M_12M
    #define KINETIS_MAX_SPEED    150000000                               // 150MHz part
    #define TARGET_HW            "K70F150M-12MHz crystal"
    #define KINETIS_K70                                                  // specify the sub-family
    #define KINETIS_K_FPU                                                // part with floating point unit
    #define OUR_HEAP_SIZE        (HEAP_REQUIREMENTS)((30 * 1024) * MEM_FACTOR)
#elif defined TWR_K70F120M
    #define TARGET_HW            "TWR-K70F120M"
    #define TWR_SER2                                                     // use SER2 serial board instead of standard serial board (used also when HS USB is enabled)
    #define KINETIS_K70                                                  // specify the sub-family
    #define KINETIS_K_FPU                                                // part with floating point unit
    #define OUR_HEAP_SIZE        (HEAP_REQUIREMENTS)((30 * 1024) * MEM_FACTOR)
#elif defined TWR_K80F150M
    #define KINETIS_MAX_SPEED    150000000
    #define TARGET_HW            "TWR-K80F150M"
  //#define TWR_SER2                                                     // use SER2 serial board instead of standard serial board (used also when HS USB is enabled)
    #define KINETIS_K80                                                  // specify the sub-family
    #define KINETIS_K_FPU                                                // part with floating point unit
    #define KINETIS_REVISION_2
    #define DEVICE_WITHOUT_ETHERNET                                      // K80 doesn't have Ethernet controller
    #define OUR_HEAP_SIZE        (HEAP_REQUIREMENTS)((30 * 1024) * MEM_FACTOR)
#elif defined FRDM_K82F
    #define KINETIS_MAX_SPEED    150000000
    #define TARGET_HW            "FRDM-K82F"
    #define KINETIS_K80                                                  // specify the sub-family
    #define KINETIS_K82                                                  // specify part
    #define KINETIS_K_FPU                                                // part with floating point unit
    #define KINETIS_REVISION_2
    #define DEVICE_WITHOUT_ETHERNET                                      // K82 doesn't have Ethernet controller
    #define OUR_HEAP_SIZE        (HEAP_REQUIREMENTS)((30 * 1024) * MEM_FACTOR)
#endif



// Specify the parameter system and a file for use by FTP, HTML and such functions
//
#if defined FRDM_KE04Z || defined FRDM_KL03Z || defined TWR_KV10Z32 || defined TEENSY_LC || defined TRK_KEA8 // due to the restricted flash size in this device the flash is used only for program code
    #define NO_FLASH_SUPPORT                                             // neither parameter nor file system
#endif
#if !defined NO_FLASH_SUPPORT
    #define USE_PARAMETER_BLOCK                                          // enable a parameter block for storing and retrieving non-volatile information
        #define USE_PAR_SWAP_BLOCK                                       // we support a backup block which can be restored if desired (it is recommended to use this together with USE_PARAMETER_BLOCK
      //#define PARAMETER_NO_ALIGNMENT                                   // the driver doesn't need to respect byte write restrictions since the application does - this can improve memory utilisation when bytes writes are not supported by the hardware
  //#define USE_PARAMETER_AREA                                           // simple parameter area rather than parameter block

    #if defined KL43Z_256_32_CL
        #define SPI_FILE_SYSTEM                                          // we have an external file system via SPI interface
    #else
      //#define SPI_FILE_SYSTEM                                          // we have an external file system via SPI interface
    #endif
  //#define I2C_EEPROM_FILE_SYSTEM                                       // we have an EEPROM based external file system via I2C interface
  //#define SPI_EEPROM_FILE_SYSTEM                                       // we have an EEPROM based external file system via SPI interface
  //#define EXT_FLASH_FILE_SYSTEM                                        // we have a file system in external FLASH memory
    #if !defined NAND_FLASH_FAT
        #define FLASH_FILE_SYSTEM                                        // we have an internal file system in FLASH
    #endif
  //#define NVRAM                                                        // we have an external file system in non-volatile RAM
  //#define INTERNAL_USER_FILES                                          // allow user to specify program file content
  //#define EMBEDDED_USER_FILES                                          // use together with INTERNAL_USER_FILES to allow sending an embedded user file to the uFileSystem
    #if !defined NAND_FLASH_FAT && defined FLASH_FILE_SYSTEM
      //#define MANAGED_FILES                                            // use memory management when interfacing with uFileSystem in internal or external SPI memory
            #define MANAGED_FILE_COUNT       5                           // the maximum number of managed files open at the same time
          //#define TIME_SLICED_FILE_OPERATION                           // allow file operations to be controlled in time slice manner, otherwise one pass per scheduler cycle (needs GLOBAL_TIMER_TASK)
          //#define MANAGED_FILE_WRITE                                   // allow writes to files to be controlled in managed mode so that they take place as a background operation
          //#define MANAGED_FILE_READ                                    // allow reads from files to be controlled in managed mode so that they take place as a background operation
    #endif
  //#define FLASH_ROUTINES                                               // supply flash routines (can be used to force flash write/erase support without any file system)

    #if !defined SPI_FILE_SYSTEM && !defined I2C_EEPROM_FILE_SYSTEM && !defined SPI_EEPROM_FILE_SYSTEM && !defined NVRAM && !defined EXT_FLASH_FILE_SYSTEM
        #define ONLY_INTERNAL_FLASH_STORAGE
    #endif

    #if defined (SPI_FILE_SYSTEM) || defined (FLASH_FILE_SYSTEM) || defined (NVRAM) || defined (I2C_EEPROM_FILE_SYSTEM) || defined SPI_EEPROM_FILE_SYSTEM || defined EXT_FLASH_FILE_SYSTEM
        #define ACTIVE_FILE_SYSTEM
    #endif
#endif

#define SUPPORT_MIME_IDENTIFIER                                          // if the file type is to be handled (eg. when mixing HTML with JPGs etc.) this should be set - note that the file system header will be adjusted
//#define IMMEDIATE_MEMORY_ALLOCATION                                    // immediately allocate all dynamic memory that will be used rather than doing it only when first used
//#define SUPPORT_UFREE                                                  // support allocating and freeing a uMalloc() region

#if defined FLASH_FILE_SYSTEM && defined SPI_FILE_SYSTEM                 // when a file system is located in SPI flash
    // Specify the SPI flash type used
    //
    #if defined KL43Z_256_32_CL
        #define SPI_FLASH_S25FL1_K                                       // use Spansion ST25FL1-K SPI flash rather than ATMEL
    #else
        #define SPI_FLASH_W25Q                                           // use Winbond W25Q SPI flash rather than ATMEL
      //#define SPI_FLASH_SST25                                          // use SST SPI SPI flash rather than ATMEL
      //#define SPI_FLASH_ST                                             // use ST SPI flash rather than ATMEL
      //#define SPI_DATA_FLASH                                           // FLASH type is data flash supporting sub-sectors (relevant for ST types)
    #endif
    #if defined SPI_FLASH_ST
        #if defined SPI_DATA_FLASH
            #define FILE_GRANULARITY (2 * SPI_FLASH_BLOCK_LENGTH)        // (2 x 4096 byte blocks) file granularity is equal to a multiple of the FLASH granularity (as defined by the device)
        #else
            #define FILE_GRANULARITY (SPI_FLASH_BLOCK_LENGTH)            // (65535 byte blocks) file granularity is equal to a multiple of the FLASH granularity (as defined by the device)
        #endif
    #elif defined SPI_FLASH_S25FL1_K
        #define FILE_GRANULARITY (8 * SPI_FLASH_BLOCK_LENGTH)            // 32k file granularity
    #elif defined SPI_FLASH_SST25 || defined SPI_FLASH_W25Q
        #define FILE_GRANULARITY (SPI_FLASH_BLOCK_LENGTH)                // (4096 byte blocks) file granularity is equal to sub-sector FLASH granularity (as defined by the device)
    #else
      //#define FILE_GRANULARITY (4 * SPI_FLASH_BLOCK_LENGTH)            // (4224/2112 byte blocks) file granularity is equal to a multiple of the FLASH granularity (as defined by the device)
        #define FILE_GRANULARITY (FLASH_GRANULARITY)
    #endif
#endif

//#define UFILESYSTEM_APPEND                                             // support appending data to uFileSystem files
//#define EXTENDED_UFILESYSTEM  3                                        // extend file names to "zXXX"
#define EXTENSION_FILE_SIZE   FILE_GRANULARITY                           // file granularity used in the extension memory (this should presently always be equal to FILE_GRANULARITY)
#define EXTENDED_FILE_SYSTEM_SIZE  (100 * (4 * 1024))                    // size of the extended uFileSystem area (in addition to standard area)

#define uFILE_SYSTEM_START    (MEMORY_RANGE_POINTER)(uFILE_START)
#define uFILE_SYSTEM_END      (MEMORY_RANGE_POINTER)(uFILE_START + FILE_SYSTEM_SIZE)
#define LAST_FILE_BLOCK       ((FILE_SYSTEM_SIZE)/FILE_GRANULARITY)      // last block in our file system

#if defined USE_PARAMETER_BLOCK
    #define POINTER_USER_NAME parameters->cUserName                      // define where the user name and password is taken from
    #define POINTER_USER_PASS parameters->cUserPass
#else
    #define POINTER_USER_NAME (CHAR *)(cParameters.cUserName)            // define where the user name and password is taken from
    #define POINTER_USER_PASS (CHAR *)(cParameters.cUserPass)
#endif


// Configure MODBUS extension package
//
//#define USE_MODBUS                                                     // activate MODBUS support in the project
#if defined USE_MODBUS
    #define USE_MODBUS_SLAVE                                             // slave capability supported
    //#define NO_SLAVE_MODBUS_READ_COILS                                 // disable specific slave public function support
    //#define NO_SLAVE_MODBUS_READ_DISCRETE_INPUTS
    //#define NO_SLAVE_MODBUS_READ_HOLDING_REGISTERS
    //#define NO_SLAVE_MODBUS_READ_INPUT_REGISTERS
    //#define NO_SLAVE_MODBUS_WRITE_SINGLE_COIL
    //#define NO_SLAVE_MODBUS_WRITE_SINGLE_REGISTER
    //#define NO_SLAVE_MODBUS_READ_EXCEPTION_STATUS
    //#define NO_SLAVE_MODBUS_DIAGNOSTICS
    //#define NO_SLAVE_MODBUS_DIAG_SUB_RETURN_QUERY_DATA
    //#define NO_SLAVE_MODBUS_DIAG_SUB_RESTART_COMS
    //#define NO_SLAVE_MODBUS_DIAG_SUB_RETURN_DIAG_REG
    //#define NO_SLAVE_MODBUS_DIAG_SUB_CHANGE_ASCII_DELIM
    //#define NO_SLAVE_MODBUS_DIAG_SUB_FORCE_LISTEN_ONLY
    //#define NO_SLAVE_MODBUS_DIAG_SUB_CLEAR_DISGNOSTICS
    //#define NO_SLAVE_MODBUS_DIAG_SUB_RTN_BUS_MSG_CNT
    //#define NO_SLAVE_MODBUS_DIAG_SUB_RTN_BUS_COM_ERR_CNT
    //#define NO_SLAVE_MODBUS_DIAG_SUB_RTN_BUS_EXC_ERR_CNT
    //#define NO_SLAVE_MODBUS_DIAG_SUB_RTN_SLAVE_MSG_CNT
    //#define NO_SLAVE_MODBUS_DIAG_SUB_RTN_SLAVE_NO_RSP_CNT
    //#define NO_SLAVE_MODBUS_DIAG_SUB_RTN_SLAVE_NAK_CNT
    //#define NO_SLAVE_MODBUS_DIAG_SUB_RTN_SLAVE_BSY_CNT
    //#define NO_SLAVE_MODBUS_DIAG_SUB_RTN_BUS_CHR_ORUN_CNT
    //#define NO_SLAVE_MODBUS_DIAG_SUB_CLEAR_ORUN_AND_FLG
    //#define NO_SLAVE_MODBUS_GET_COMM_EVENT_COUNTER
    //#define NO_SLAVE_MODBUS_GET_COMM_EVENT_LOG
    //#define NO_SLAVE_MODBUS_WRITE_MULTIPLE_COILS
    //#define NO_SLAVE_MODBUS_WRITE_MULTIPLE_REGISTERS
    //#define NO_SLAVE_MODBUS_REPORT_SLAVE_ID
    //#define NO_SLAVE_MODBUS_MASK_WRITE_REGISTER
    //#define NO_SLAVE_MODBUS_READ_WRITE_MULTIPLE_REGISTER
    //#define NO_SLAVE_MODBUS_READ_FIFO_QUEUE
    //#define NOTIFY_ONLY_COIL_CHANGES                                   // notify user of individual coil changes only
    //#define USE_MODBUS_MASTER                                          // master capability supported (either slave or master required)
    //#define NO_MASTER_MODBUS_READ_COILS                                // disable specific master public function support
    //#define NO_MASTER_MODBUS_READ_DISCRETE_INPUTS
    //#define NO_MASTER_MODBUS_READ_HOLDING_REGISTERS
    //#define NO_MASTER_MODBUS_READ_INPUT_REGISTERS
    //#define NO_MASTER_MODBUS_WRITE_SINGLE_COIL
    //#define NO_MASTER_MODBUS_WRITE_SINGLE_REGISTER
    //#define NO_MASTER_MODBUS_READ_EXCEPTION_STATUS
    //#define NO_MASTER_MODBUS_DIAGNOSTICS                               // shared by all sub-functions
    //#define NO_MASTER_MODBUS_GET_COMM_EVENT_COUNTER
    //#define NO_MASTER_MODBUS_GET_COMM_EVENT_LOG
    //#define NO_MASTER_MODBUS_WRITE_MULTIPLE_COILS
    //#define NO_MASTER_MODBUS_WRITE_MULTIPLE_REGISTERS
    //#define NO_MASTER_MODBUS_REPORT_SLAVE_ID
    //#define NO_MASTER_MODBUS_MASK_WRITE_REGISTER
    //#define NO_MASTER_MODBUS_READ_WRITE_MULTIPLE_REGISTER
    //#define NO_MASTER_MODBUS_READ_FIFO_QUEUE
    #if defined USE_MODBUS_MASTER
        #define MODBUS_GATE_WAY_ROUTING                                  // configurable routing from slave gateways (requires master functionality)
        #define MODBUS_GATE_WAY_QUEUE                                    // support queuing of MODBUS transmissions - advisable for gateways
            #define SUPPORT_FIFO_QUEUES
            #define MODBUS_DELAYED_RESPONSE                              // allow slave parameter interface to delay request responses - for example to prepare latest data from external location
            #define MAX_QUEUED_REQUEST_LENGTH    8                       // longest request data length that needs to be saved when requests are delayed
    #endif
#endif


// Configure driver services
//
#if !defined DEVICE_WITHOUT_CAN                                          // if the device has CAN
  //#define CAN_INTERFACE                                                // enable CAN bus interface
#endif
#if defined CAN_INTERFACE
    #define NUMBER_CAN          4                                        // the number of logical queues required for CAN support (2 logical queues each for up to 2 CAN controllers)
  //#define UTASKER_SIM                                                  // simulator HW extension
        #define SIM_HW_IP_ADD       192,168,0,4                          // IP address of our HW simulator extension
        #define SIM_HW_PORT_NUMBER  1234                                 // port number used by out HW simulator
    #define SIM_KOMODO                                                   // use Komodo as simulator CAN extension
        #define KOMODO_USB_PORT 1                                        // use this USB port (0 or 1) - any additional monitor program sharing the Komodo can use the other port
#else
    #define NUMBER_CAN   0                                               // no physical queue needed
#endif

#define SERIAL_INTERFACE                                                 // enable serial interface driver
#if !defined BLINKY && defined SERIAL_INTERFACE
  //#define FREEMASTER_UART                                              // UART for run-time debugging use
  //#define UART_EXTENDED_MODE                                           // required for 9-bit mode
  //    #define SERIAL_MULTIDROP_TX                                      // enable 9-bit support in the transmission direction
  //    #define SERIAL_MULTIDROP_RX                                      // enable 9-bit support in the reception direction
    #define SERIAL_STATS                                                 // keep statistics about serial interface use
  //#define SUPPORT_MSG_MODE                                             // enable terminator recognition (MSG_MODE)
      //#define SUPPORT_MSG_CNT                                          // enable the message counter mode (MSG_MODE_RX_CNT) - requires also SUPPORT_MSG_MODE
      //#define MSG_CNT_WORD                                             // 64k message length support (rather than 255)
      //#define SUPPORT_MSG_MODE_EXTRACT                                 // allow extracting messages using a number of reads rather than one single read
    #define WAKE_BLOCKED_TX                                              // allow a blocked transmitter to continue after an interrupt event
      //#define WAKE_BLOCKED_TX_BUFFER_LEVEL                             // define specific level in output buffer for wake to occur
    #define SUPPORT_FLUSH                                                // support rx flush
    #define SERIAL_SUPPORT_XON_XOFF                                      // enable XON/XOFF support in driver
    #define HIGH_WATER_MARK   20                                         // stop flow control when the input buffer has less than this space (if variable settings are required, use SUPPORT_FLOW_HIGH_LOW define)
    #define LOW_WATER_MARK    20                                         // restart when the input buffer content falls below this value
    #define SUPPORT_FLOW_HIGH_LOW                                        // allow flow control levels to be configured (in % of buffer size)
    #define SERIAL_SUPPORT_ECHO                                          // enable echo mode in rx driver
    #define SERIAL_SUPPORT_ESCAPE                                        // enable escape sequencing in driver
  //#define SERIAL_SUPPORT_SCAN                                          // serial receiver supports scanning of input buffer for a sequence
    #if !defined KINETIS_KL
      //#define SUPPORT_HW_FLOW                                          // support RTS/CTS flow control and other possible modem signals
    #endif
  //#define UART_BREAK_SUPPORT                                           // support break control in the UART driver

    #define LOG_UART0                                                    // activate this option to log all data sent to UART 0 to a file called "UART0.txt"
    #define LOG_UART1                                                    // activate this option to log all data sent to UART 1 to a file called "UART1.txt"
    #define LOG_UART2                                                    // activate this option to log all data sent to UART 2 to a file called "UART2.txt"
    #define LOG_UART3                                                    // activate this option to log all data sent to UART 3 to a file called "UART3.txt"
    #define LOG_UART4                                                    // activate this option to log all data sent to UART 4 to a file called "UART4.txt"
    #define LOG_UART5                                                    // activate this option to log all data sent to UART 5 to a file called "UART5.txt"

    #if defined USE_MODBUS
        #define MODBUS_RTU                                               // support binary RTU mode
        #define MODBUS_ASCII                                             // support ASCII mode
        #define STRICT_MODBUS_SERIAL_MODE                                // automatically adjust the character length according to mode
        #if defined FRDM_KL02Z
            #define MODBUS_SERIAL_INTERFACES      1
        #else
            #define MODBUS_SERIAL_INTERFACES      2
        #endif
      //#define MODBUS_SHARED_SERIAL_INTERFACES   3                      // number of slave interfaces sharing UARTs
        #define MODBUS_RS485_SUPPORT                                     // support RTS control for RS485 transmission
        #define FAST_MODBUS_RTU                                          // speeds of greater than 19200 use calculated RTU times rather than recommended fixed values
        #if defined MODBUS_RS485_SUPPORT
            #if !defined SUPPORT_HW_FLOW
                #define SUPPORT_HW_FLOW                                  // ensure that HW flow control is enabled when RS485 support required
            #endif
            #define UART_FRAME_COMPLETE                                  // the UART driver informs of frame completion - activated when RS485 mode is required
        #endif
        #define MODBUS_SUPPORT_SERIAL_LINE_FUNCTIONS                     // support the serial line function at the slave
        #define MODBUS_SUPPORT_SERIAL_LINE_DIAGNOSTICS                   // support serial line diagnostics
        #define MODBUS_CRC_FROM_LOOKUP_TABLE                             // MODBUS RTU cyclic redundancy check performed with help of loop up table (requires 512 bytes FLASH table, but faster than calculation loop)
    #endif
#else
    #define NUMBER_SERIAL              0                                 // no physical queue needed
    #define NUMBER_EXTERNAL_SERIAL     0
#endif

#if defined USE_MODBUS && defined MODBUS_GATE_WAY_QUEUE && !defined SUPPORT_FLUSH
    #define SUPPORT_FLUSH                                                // support queue flush for use by the MODBUS gateway
#endif

#if defined DEVICE_WITHOUT_USB
    #define NUMBER_USB     0                                             // no physical queue needed
#else
    #define USB_INTERFACE                                                // enable USB driver interface
    #if defined USB_INTERFACE
      //#define USB_HOST_SUPPORT                                         // host rather than device
        #define USB_SPEC_VERSION            USB_SPEC_VERSION_2_0         // report USB2.0 rather than USB1.1 - usually USB1.1 is used since it is equivalent to USB2.0 but requires one less descriptor transfer
        #if defined USB_HOST_SUPPORT
            #define NUMBER_USB     (5 + 1)                               // physical queues (control plus 5 endpoints)
        #else                                                            // define one or more device classes (multiple classes creates a composite device)
            #define USE_USB_CDC                                          // USB-CDC (use also for Modbus over USB)
          //#define USE_USB_MSD                                          // needs SD card to compile (or alternatives FLASH_FAT / SPI_FLASH_FAT / FAT_EMULATION)
          //#define USE_USB_HID_MOUSE                                    // human interface device (mouse)
          //#define USE_USB_HID_KEYBOARD                                 // human interface device (keyboard)
          //#define USE_USB_HID_RAW                                      // human interface device (raw)
          //#define USE_USB_AUDIO                                        // audio endpoints
                #define AUDIO_BUFFER_COUNT  (32)                         // this many isochronous packets can fit into the buffer (the buffer size is AUDIO_BUFFER_COUNT * isochronous endpoint size)
            #if defined USE_USB_CDC
              //#define USB_CDC_RNDIS                                    // Remote Network Driver Interface Specification for virtual network adapter
                  //#define USB_TX_MESSAGE_MODE                          // enable transmission message mode when using RNDIS
            #endif
            #if defined USE_USB_HID_MOUSE || defined USE_USB_HID_RAW
                #define NUMBER_USB_HID 1
            #else
                #define NUMBER_USB_HID 0
            #endif
            #if defined USE_USB_MSD
              //#define FAT_EMULATION                                    // support FAT emulation (full mass-storage not required by USB-MSD)
                #if defined FAT_EMULATION
                    #define NUMBER_USB_MSD 2                             // single MSD LUM (set to 2 for SD card and emulated drive)
                    #define EMULATED_FAT_LUMS         1                  // the number of logical units on emulated drive
                    #define EMULATED_FAT_DISK_SIZE   (1024 * 1024)       // 1 MByte disk to be emulated
                    #define MAXIMUM_DATA_FILES        8                  // maximum number of data files that could be used by the disk
                    #define ROOT_DIR_SECTORS          2                  // 2 sectors is adequate for file information (including LFNs) for maximum file count
                    #define EMULATED_FAT_FILE_DATE_CONTROL               // allow the application to define the time stamp of files
                    #define EMULATED_FAT_FILE_NAME_CONTROL               // allow the application to define the name of files
                    #if defined EMULATED_FAT_FILE_NAME_CONTROL
                        #define FAT_EMULATION_LFN                        // allow long file names to be used for emulated files
                            #define UTFAT_LFN_WRITE_PATCH                // use Linux workaround to avoid patent issues
                    #endif
                #else
                    #define NUMBER_USB_MSD 1                             // single MSB LUM
                #endif
            #else
                #define NUMBER_USB_MSD 0
            #endif
            #if defined USE_USB_HID_KEYBOARD
                #define SUPPORT_FIFO_QUEUES
            #endif
            #if defined USE_USB_HID_KEYBOARD || defined USE_USB_HID_RAW
                #define IN_COMPLETE_CALLBACK
            #endif
            #if defined USE_USB_CDC
              //#define USB_SIMPLEX_ENDPOINTS                            // share IN and OUT on a single endpoint
              //#define FREEMASTER_CDC                                   // CDC instance for run-time debugging use (if USB_CDC_COUNT is 1 the single USB-CDC connection is used, otherwise the last instance is used)
                #if defined USB_CDC_RNDIS
                    #define USB_CDC_RNDIS_COUNT       1                  // the number of RNDIS virtual network interfaces
                    #define USB_CDC_VCOM_COUNT        0                  // the number of CDC virtual COM ports in composite device
                    #define USB_CDC_COUNT             (USB_CDC_VCOM_COUNT + USB_CDC_RNDIS_COUNT) // number of USB-CDC interfaces
                    #if !defined DEVICE_WITHOUT_ETHERNET
                        #define USB_TO_ETHERNET                          // allow RNDIS to Ethernet operation (this doesn't need the TCP/IP stack but does activate the Ethernet driver)
                        #if defined USB_TO_ETHERNET
                            #define NO_USB_ETHERNET_BRIDGING             // keep RNDIS and Ethernet networks separated (disable when there is no Ethernet interface)
                          //#define ETHERNET_FILTERED_BRIDGING           // enable non-promiscuous bridging from Ethernet to the RNDIS (without TCP/IP stack)
                            #define ETHERNET_BRIDGING                    // allow bridging in parallel with TCP/IP stack (can be removed if there is no Ethernet interface)
                        #endif
                    #endif
                    #if !defined ETHERNET_FILTERED_BRIDGING              // if not operating as a pure USB-Ethernet adapter
                        #define USB_TO_TCP_IP                            // enable RNDIS interface to communicate with the local TCP/IP stack
                        #if defined NO_USB_ETHERNET_BRIDGING
                            #define IP_INTERFACE_COUNT    2              // two interfaces available (RDNIS and Ethernet)
                            #define RNDIS_IP_INTERFACE    (unsigned char)1
                            #define ETHERNET_IP_INTERFACE (DEFAULT_IP_INTERFACE)
                            #define RNDIS_INTERFACE       defineInterface(RNDIS_IP_INTERFACE) // RNDIS interface is second interface
                        #elif defined DEVICE_WITHOUT_ETHERNET
                            #define IP_INTERFACE_WITHOUT_CS_OFFLOADING
                        #endif
                        #define SUPPORT_FIFO_QUEUES
                    #endif
                #else
                    #define USB_CDC_COUNT             1                  // number of USB-CDC interfaces
                #endif
                #if defined USE_MAINTENANCE && (USB_CDC_COUNT > 1)
                    #define MODBUS_USB_INTERFACE_BASE 1                  // MODBUS USB interface follows maintenance
                #else
                    #define MODBUS_USB_INTERFACE_BASE 0                  // MODBUS USB is first interface
                #endif
                #define MODBUS_USB_CDC_COUNT          0                  // the number of CDC interfaces assigned to MODBUS
                #define USB_SERIAL_CONNECTIONS                           // if serial interfaces are supported allow attaching each USB-CDC connection to a UART
                #if defined USB_CDC_COUNT && (USB_CDC_COUNT > 1) && !defined USB_SPEC_VERSION
                    #define USB_SPEC_VERSION          USB_SPEC_VERSION_2_0 // multiple interface composite devices should use USB2.0
                #endif
                #if defined USB_SIMPLEX_ENDPOINTS
                    #define NUMBER_USB     ((2 * USB_CDC_COUNT) + NUMBER_USB_MSD + NUMBER_USB_HID + 1) // physical queues (control plus 3 endpoints for each USB-CDC interface) needed for USB-CDC interface
                #else
                    #define NUMBER_USB     ((3 * USB_CDC_COUNT) + NUMBER_USB_MSD + NUMBER_USB_HID + 1) // physical queues (control plus 3 endpoints for each USB-CDC interface) needed for USB-CDC interface
                #endif
            #else
                #define NUMBER_USB     (NUMBER_USB_MSD + NUMBER_USB_HID + 1) // physical queues (control plus 5 endpoints)
            #endif
        #endif

        #if defined TWR_K70F120M || defined EMCRAFT_K61F150M
          //#define USB_HS_INTERFACE                                     // use HS interface rather than FS interface (needs external ULPI transceiver) - use with TWR_SER2 and secondary elevator (not dummy elevator)
        #endif
        #if defined USB_HOST_SUPPORT
          //#define USB_MSD_HOST                                         // works together with mass-storage for a USB memory stick as disk E
            #define USB_CDC_HOST
            #if defined USB_MSD_HOST
                #define SUPPORT_USB_SIMPLEX_HOST_ENDPOINTS               // allow operation with memory sticks using bulk IN/OUT on the same endpoint
            #endif
        #endif
        #if defined USB_HS_INTERFACE || defined USB_HOST_SUPPORT || defined USE_USB_AUDIO || defined USB_CDC_RNDIS // since RNDIS makes intensive use of enpoint 0 for status and control it makes snese to use the largest size possible (assuming at least full-speed operation)
            #define ENDPOINT_0_SIZE         64                           // high speed devices should use 64 bytes (and hosts use full size endpoint size)
        #else
            #define ENDPOINT_0_SIZE         8                            // maximum packet size for endpoint 0. Low speed devices must use 8 whereas full speed devices can chose to use 8, 16, 32 or 64
        #endif
        #define SUPPORT_FLUSH                                            // allow flush command to be used (important for mass storage class)
        #define WAKE_BLOCKED_USB_TX                                      // allow a blocked USB transmitter to continue after an interrupt event
        #define LOG_USB_TX                                               // log USB transmissions in simulator
        #define NUMBER_OF_POSSIBLE_CONFIGURATIONS  1                     // one USB configuration
        #define USB_STRING_OPTION                                        // support optional string descriptors
            #define USB_MAX_STRINGS                3                     // the maximum number of strings supported by host
            #define USB_MAX_STRING_LENGTH          16                    // the maximum length of each string supported by host (unicode characters)
        #define USB_RUN_TIME_DEFINABLE_STRINGS                           // enable USB string content to be defined at run time (variable)
      //#define USE_USB_OTG_CHARGE_PUMP                                  // enable charge pump control in the driver
        #if defined USE_USB_OTG_CHARGE_PUMP
            #define I2C_INTERFACE                                        // activate I2C interface since it will be needed
        #endif
        #if defined USE_MODBUS && defined USE_MODBUS_SLAVE && !defined USB_HOST_SUPPORT
            #define MODBUS_USB_SLAVE                                     // MODBUS serial slave realised as USB
        #endif
    #else
        #define NUMBER_USB     0                                         // no physical queue needed
    #endif
#endif

#if defined KL43Z_256_32_CL
    #define I2C_INTERFACE                                                // enable I2C interface driver
    #define SUPPORT_FLUSH_I2C
#else
  //#define I2C_INTERFACE
#endif
#if defined I2C_INTERFACE
    #define NUMBER_I2C       I2C_AVAILABLE                               // I2C interfaces available
  //#define I2C_SLAVE_MODE                                               // support slave mode
#else
    #define NUMBER_I2C     0                                             // no physical queue needed
#endif

#if defined TWR_K60F120M || defined TWR_K70F120M || defined EMCRAFT_K70F120M // NAND flash available so utFAT can be operated in it
  //#define NAND_FLASH_FAT                                               // NAND flash requires SDCARD_SUPPORT to also be active but takes priority over SD card
        #define VERIFY_NAND                                              // development help functions
#endif

//#define SDCARD_SUPPORT                                                 // SD-card interface
//#define FLASH_FAT                                                      // FAT in internal flash interface
//#define SPI_FLASH_FAT                                                  // SPI flash
    #define SIMPLE_FLASH                                                 // don't perform block management and wear-leveling
#if (defined SDCARD_SUPPORT || defined SPI_FLASH_FAT || defined FLASH_FAT || defined USB_MSD_HOST) && !defined BLINKY
    #if defined SPI_FLASH_FAT
        #undef ONLY_INTERNAL_FLASH_STORAGE                               // allow multiple flash storage support
    #endif
    #if defined FLASH_FAT && !defined FLASH_ROUTINES
        #define FLASH_ROUTINES                                           // ensure that internal flash routines are available
    #endif
    #if defined FLASH_FAT
        #if defined FRDM_K64F || defined TWR_K21F120M || defined FreeLON
            #define FLASH_FAT_MANAGEMENT_ADDRESS    (512 * 1024)         // physical address where the used flash starts
        #elif defined FRDM_K22F
            #define FLASH_FAT_MANAGEMENT_ADDRESS    (160 * 1024)         // physical address where the used flash starts
        #elif defined KWIKSTIK
            #define FLEXFLASH_DATA                                       // use FlexNMV in data mode
            #define FLASH_FAT_MANAGEMENT_ADDRESS    (256 * 1024)         // physical address where the used flash starts
        #else
            #define FLASH_FAT_MANAGEMENT_ADDRESS    (128 * 1024)         // physical address where the used flash starts
        #endif
        #define INTERNAL_FLASH_FAT_SIZE   (SIZE_OF_FLASH - FLASH_FAT_MANAGEMENT_ADDRESS) // size of the flash area used as disk (to the end of internal flash)
    #endif

    #define SD_CARD_RETRY_INTERVAL       5                               // attempt SD card initialisation at 5s intervals
    #define UT_DIRECTORIES_AVAILABLE     DISK_COUNT                      // this many directories objects are available for allocation (just one per disk for application interface)
    #define UTMANAGED_FILE_COUNT         10                              // allow this many managed files at one time
    #define UTFAT_LFN_READ                                               // enable long file name read support
    #define STR_EQUIV_ON                                                 // ensure that this routine is available
    #if defined UTFAT_LFN_READ
        #define MAX_UTFAT_FILE_NAME      (100)                           // the maximum file name length supported - maximum 255
    #endif
  //#define UTFAT_MULTIPLE_BLOCK_WRITE                                   // use multiple block writes where possible (write speed efficiency)
      //#define UTFAT_PRE_ERASE                                          // use pre-erase where possible (potential write speed efficiency)
    #define UTFAT_WRITE                                                  // enable write functions
    #if defined UTFAT_WRITE
        #define UTFAT_FORMATTING                                         // enable formatting SD cards (requires also write)
        #define UTFAT_FULL_FORMATTING                                    // enable formatting SD cards including zeroing of data sectors as well as FAT sectors
        #define UTFAT_LFN_DELETE                                         // support deleting files with long file names (cleaning up all LFN directory entries)
        #define UTFAT_LFN_WRITE                                          // support writing long file names
        #define UTFAT_LFN_WRITE_PATCH                                    // patch long file writes to potentially avoid possible patent issues
            #define SFN_ENTRY_CACHE_SIZE 20                              // short file name cache used to speed up SFN alias collision searching when writing LFNs
      //#define UTFAT_SAFE_DELETE                                        // delete operation removes all information (name and content details are removed from disk - no undelete possible)
        #define UTFAT_UNDELETE                                           // undelete a file or directory
      //#define UTFAT_FILE_CACHE_POOL    2                               // two file buffers in the pool - used on an individual file basis to cache data writes (latest write on close)
    #endif
    #define UTFAT_EXPERT_FUNCTIONS                                       // enable additional functions for monitoring operation and performing advanced operations
    #define UTFAT16                                                      // support FAT16 as well as FAT32
  //#define UTFAT12                                                      // support FAT12 as well as FAT32
  //#define SUPPORT_FILE_TIME_STAMP                                      // when activated fnGetLocalFileTime() must exist, which returns the date and time information
  //#define UTFAT_SECT_LITTLE_ENDIAN                                     // display debug views in little-endian converted form
  //#define UTFAT_SECT_BIG_ENDIAN                                        // display debug views in big-endian converted form
#endif

#if defined FLASH_ROUTINES && (defined FREEMASTER_CDC || defined FREEMASTER_UART)
    #define FREEMASTER_STORAGE_ACCESS                                    // use storage interface rather than direct memory mapping so that SPI flash content can be addressed and also flash can be written
#endif

#if defined FRDM_K64F || defined FRDM_K66F || defined FRDM_K22F || defined FRDM_KL25Z || defined FRDM_KL46Z || defined FRDM_KL03Z || defined FRDM_KL43Z // during development only - these boards have been configured and tested
  //#define nRF24L01_INTERFACE                                           // nRF24L01+ interface - low power RF
    #if defined FRDM_K64F
      //#define ENC424J600_INTERFACE                                     // 10/100 Ethernet connected via SPI (also ENC624J600 in larger package with more parallel modes)
      //#define USE_SIMULATION_INTERFACE                                 // work with a remote simulation interface (remove when not present)
      //#define REMOTE_SIMULATION_INTERFACE                              // FRDM-K64 being used in remote simulation mode as HW interface to the simulator
      //#define NO_INTERNAL_ETHERNET                                     // disable Ethernet on the internal controller
    #endif
#endif

#if !defined DEVICE_WITHOUT_ETHERNET && !defined K70F150M_12M && !defined TEENSY_3_5 && !defined TEENSY_3_6
    #define ETH_INTERFACE                                                // enable Ethernet interface driver
#elif defined TEENSY_3_1 || defined TEENSY_LC
  //#define ETH_INTERFACE                                                // enable external Ethernet interface driver
    #if defined ETH_INTERFACE
        #define ENC424J600_INTERFACE                                     // using ENC424J600
    #endif
#endif
#if (defined ETH_INTERFACE || defined USB_CDC_RNDIS) && !defined BLINKY
    #define MAC_DELIMITER  '-'                                           // used for display and entry of mac addresses
    #define IPV6_DELIMITER ':'                                           // used for display and entry of IPV6 addresses
    #define NUMBER_LAN     1                                             // one physical interface needed for LAN

  //#define LOG_ETHERNET                                                 // activate this option to log all transmitted data to a file called "Eth_tx.txt"
  //#define SUPPORT_DISTRIBUTED_NODES
    #if defined SUPPORT_DISTRIBUTED_NODES                                // needs global timer to operate
        #define PROTOCOL_UNETWORK 0x754e                                 // uN - uNetwork protocol
        #define MAX_NETWORK_NODES 2
        #define UPROTOCOL_WITH_RETRANS
        #define MAX_STORED_MESSAGES       10                             // set TIMER_QUANTITY to at least this amount since each message needs a timer
        #define MAX_UPROT_MSG_SIZE        255
        #define UPROT_RETRANS_TIME        (DELAY_LIMIT)(0.05 * SEC)
        #define MAX_UPROT_RETRANS_ATTEMPS 5
        #define USE_TIMER_FUNCTION_EVENT                                 // global timer handles messages to itself as function event
    #endif
  //#define ETHERNET_BRIDGING                                            // bridge received Ethernet frames to alternative interfaces (uses must supply fnBridgeEthernetFrame(ETHERNET_FRAME *ptr_rx_frame))
    #define SUPPORT_SUBNET_BROADCAST                                     // allow reception to subnet broadcast address
    #define SUPPORT_SUBNET_TX_BROADCAST                                  // allow transmission to subnet broadcast address

    #define ETHERNET_RELEASE_AFTER_EVERY_FRAME                           // handle only one Ethernet reception frame at a time and allow other tasks to be scheduled in between
        #define ETHERNET_RELEASE_LIMIT  3                                // allow a maximum of three reception frames to be handled

    #if defined ETH_INTERFACE && defined NO_USB_ETHERNET_BRIDGING
        #define IP_NETWORK_COUNT             2                           // number of networks
            #define SECOND_NETWORK           1                           // reference to second network (after DEFAULT_NETWORK which is 0)
    #endif
    #if defined ENC424J600_INTERFACE
        #define IP_INTERFACE_WITHOUT_CS_OFFLOADING                       // enable software based checksum offloading so that it can be used by this interface
        #if !defined DEVICE_WITHOUT_ETHERNET && !defined NO_INTERNAL_ETHERNET
            #define IP_INTERFACE_COUNT   2                               // two interfaces available
            #define ETHERNET_INTERFACES  2                               // both treated as Ethernet interfaces
        #else
            #define IP_INTERFACE_COUNT   1                               // one interface available
            #define ETHERNET_INTERFACES  1
        #endif
        #define ETHERNET_IP_INTERFACE    DEFAULT_IP_INTERFACE
        #define ENC424J00_IP_INTERFACE   (unsigned char)1
      //#define RF_IP_INTERFACE          (unsigned char)2
        #define ETHERNET_INTERFACE       defineInterface(ETHERNET_IP_INTERFACE) // ethernet interface is default interface
        #define ENC424J00_INTERFACE      defineInterface(ENC424J00_IP_INTERFACE) // ENC424J600 interface is second interface
      //#define RF_INTERFACE             defineInterface(RF_IP_INTERFACE)// nRF24L01+ interface is third interface
        #if defined nRF24L01_INTERFACE
            #define USER_WRITE_HANDLERS  1                               // if application is handling some interface handles the quantity needs to be entered
        #endif
        #define ENC424J600_NETWORK       DEFAULT_NETWORK                 // the ENC424J600 interface is on the default network
    #else
        #if !defined IP_INTERFACE_COUNT
            #define IP_INTERFACE_COUNT       1                           // single interface available
        #endif
        #define ETHERNET_INTERFACE  defineInterface(DEFAULT_IP_INTERFACE)// ethernet interface is default interface
    #endif
  //#define SUPPORT_VLAN                                                 // enable VLAN support
      //#define SUPPORT_DYNAMIC_VLAN                                     // enable support of VLAN reception on a frame by frame basis
      //#define SUPPORT_DYNAMIC_VLAN_TX                                  // allow transmission VLAN IDs to be controlled on a frame by frame basis
        //#define ALTERNATIVE_VLAN_COUNT  2                              // alternative VLANs managed (in addition to standard VLAN) - used only together with SUPPORT_DYNAMIC_VLAN
      // Configure TCP/IP services
      //
        #define USE_IPV6                                                 // enable IPv6
      //#define USE_IPV6INV4                                             // support tunnelling IPv6 ind IPv4
      //#define USE_IPV6INV4_RELAY_DESTINATIONS 2                        // enable relaying to other nodes in the network - the number of destination in the IPv6 in IPv4 relay table
        #define MAX_HW_ADDRESS_LENGTH  MAC_LENGTH                        // set a variable maximum hardware address length - default is Ethernet MAC-48, 6 bytes
        #define NEIGHBOR_TABLE_ENTRIES 4                                 // the maximum entries in IPV6 neighbor table
    #if !defined ETHERNET_FILTERED_BRIDGING
        #define USE_IP                                                   // enable IP and ARP and all other possible tcp/ip services
    #endif
    #if defined USE_IP
      //#define IPV4_SUPPORT_TX_FRAGMENTATION                            // support sending IPv4 fragments (for datagrams exceeding the Ethernet MTU)
      //#define IPV4_SUPPORT_RX_DEFRAGMENTATION                          // support receiving and reassembling IPv4 fragments
            #define DEFRAG_MEMORY_POOL_SIZE (4 * 1024)                   // total de-fragmentation datagram memory to be shared by multiple datagrams if they occur at the same time
            #define MAX_DEFRAG_DATAGRAMS 2                               // the number of datagrams can be reassembled in parallel (sharing the de-fragmentation memory) - 1 or 2 possible
            #define MAX_DEFRAG_HOLES     8                               // the maximum number of holes that can be managed for each de-fragmented datagram

        #define USE_IP_STATS                                             // enable IP statistics (counters)
        #define ARP_TABLE_ENTRIES      8                                 // the maximum entries in ARP table
        #define ARP_IGNORE_FOREIGN_ENTRIES                               // only add used addresses to ARP table
      //#define RESTRICTED_GATEWAY_INTERFACE                             // used only when multiple interfaces are available, in which case the user must supply fnRestrictGatewayInterface() to decide which interfaces are included in gateway ARP re-resolves (when not used all interfaces are used in a single network environment or only the original interface in a multi-network environment)
        #define USE_ICMP                                                 // enable ICMP
      //#define USE_IGMP                                                 // enable IGMP
            #if defined ENC424J600_INTERFACE && (ETHERNET_INTERFACES > 1)
                #define IGMP_ALL_HOSTS_INTERFACES   (ETHERNET_INTERFACE | ENC424J00_INTERFACE) // interfaces that IGMP is to work on (enabling the all-host group on all required interfaces - only needed when IP_INTERFACE_COUNT > 1)
            #elif defined RNDIS_IP_INTERFACE && (ETHERNET_INTERFACES > 1)
                #define IGMP_ALL_HOSTS_INTERFACES   (ETHERNET_INTERFACE | RNDIS_IP_INTERFACE) // interfaces that IGMP is to work on (enabling the all-host group on all required interfaces - only needed when IP_INTERFACE_COUNT > 1)
            #else
                #define IGMP_ALL_HOSTS_INTERFACES   defineInterface(DEFAULT_IP_INTERFACE) // interfaces that IGMP is to work on (enabling the all-host group on all required interfaces - only needed when IP_INTERFACE_COUNT > 1)
            #endif
            #define USE_IGMP_V2                                          // support IGMP v2
          //#define USE_IGMP_V3                                          // support IGMP v3
            #define IGMP_MAX_HOSTS     2                                 // host groups (in addition to all-hosts)
            #define IGMP_MAX_PROCESSES 4                                 // the maximum number of processes in each host group
        #if defined _WINDOWS
          //#define PSEUDO_LOOPBACK                                      // pseudo loop back when simulating - only for use with the simulator!! (this allows an application to send test frames to its own IP address)
        #endif
      //#define USE_ZERO_CONFIG                                          // support IPv4 link-local and zero configuration (autoIP)
        #if defined USE_ICMP                                             // specify ICMP support details
            #define ICMP_PING                                            // allow PING reply
            #define ICMP_SEND_PING                                       // support PING transmission
            #define ICMP_DEST_UNREACHABLE                                // allow destination unreachable if no UDP port exists
            #define ICMP_PING_IP_RESULT                                  // report the source IP in ping result
        #endif
    #endif
    #if defined USE_IP || defined USE_IPV6
        #define USE_UDP                                                  // enable UDP over IP - needs IP
        #define USE_TCP                                                  // enable TCP over IP - needs IP

        #if defined USE_TCP                                              // specify TCP support details
          //#define T_TCP_PERIOD           (DELAY_LIMIT)(0.1 * SEC)      // user defined TCP polling resolution (allows higher resolution polling and the following user defined values rather than defaults)
            #if defined T_TCP_PERIOD
                #define TCP_DEF_RETRIES              7                   // packets will be retransmitted this amount of times
                #define TCP_CON_ATTEMPTS             7                   // connection attempts will stop after this amount of tries
                #define USER_TCP_SYN_RETRY_MS        600                 // TCP connection repetition timer (ms)
                #define USER_TCP_STANDARD_RETRY_MS   400                 // standard TCP transmission repetition timer (ms)
                #define USER_TCP_SILLY_WINDOWS_DELAY_MS 3800             // silly windows delay (ms)
                #define USER_TCP_FIN_WAIT_2_SECONDS  60                  // FIN_WAIT_2 timeout (seconds)
                #define USER_TCP_MSL_SECONDS         2                   // MSL (seconds)
            #endif
            #define USE_OUT_OF_ORDER_TCP_RX_BUFFER                       // enable a single TCP frame buffer to handle out-of-order receptions (useful when receiving large data streams in case the server starts sending out-of-order frames after a frame loss)
            #define REUSE_TIME_WAIT_SOCKETS                              // allow sockets in 2MSL wait to be reallocated to different IP/port pairs
            #define TCP_CHECK_SYS_REPS                                   // reuse a socket whose SYN + ACK has been lost (avoids it repeating its SYN + ACK and unnecessarily blocking in a socket until it times out)
            #define RST_ON_NO_TCP_PORT                                   // SYN to non-existing ports receive a RST
            #define ANNOUNCE_MAX_SEGMENT_SIZE                            // we announce the maximum segment size we can handle when establishing a connection
            #define SUPPORT_PEER_MSS                                     // respect peer's MSS when sending TCP frames
            #define SUPPORT_PEER_WINDOW                                  // respect peer's Open Window when sending TCP frames
            #define WINDOWING_BUFFERS      4                             // we can send 4 frames before requiring ACKs
            #define CONTROL_WINDOW_SIZE                                  // support variable windows size to quench reception

          //#define USE_FTP                                              // enable FTP - needs TCP
          //#define USE_FTP_CLIENT                                       // enable FTP client - needs TCP
          //#define USE_SMTP                                             // enable SMTP - needs TCP
          //#define USE_POP3                                             // enable POP3 Email - needs TCP
            #define USE_HTTP                                             // support embedded Web server - needs TCP
          //#define USE_TELNET                                           // enable TELNET support
          //#define USE_TELNET_CLIENT                                    // enable TELNET client support
          //#define USE_TIME_SERVER                                      // enable time server support - presently demo started in application
                #define NUMBER_OF_TIME_SERVERS 3                         // number of time servers that are used
          //#define MODBUS_TCP                                           // support MODBUS TCP protocol
            #define USE_MQTT_CLIENT                                      // enable MQTT (message queuing telemetry transport) client support
          //#define USE_MQTT_BROKER                                      // enable MQTT (message queuing telemetry transport) broker support
              //#define SECURE_MQTT                                      // MQTTS support
              //#define SUPPORT_CLIENT_SIDE_CERTIFICATE                  // support client certificate and private key

          //#define TEST_CLIENT_SERVER                                   // TCP client/server test (see debug.c)
          //#define TEST_TCP_SERVER                                      // TCP server (see debug.c) - uses also a TELNET session
            #if defined TEST_CLIENT_SERVER
                #define USER_TCP_SOCKETS      1
            #else
                #define USER_TCP_SOCKETS      0                          // we use no non-standard TCP sockets
            #endif
            #if defined SDCARD_SUPPORT || defined SPI_FLASH_FAT || defined FLASH_FAT || defined USB_MSD_HOST
                #if UT_DIRECTORIES_AVAILABLE < 5
                    #undef UT_DIRECTORIES_AVAILABLE
                    #define UT_DIRECTORIES_AVAILABLE 5                   // this many directories objects are available for allocation (more to allow web server and FTP)
                #endif
            #endif
        #endif

        #if defined USE_UDP
          //#define SUPPORT_MULTICAST_TX                                 // allow multicast UDP transmission (to IP addresses in the range 224.0.0.0 to 239.255.255.255)
          //#define SUPPORT_MULTICAST_RX                                 // allow reception of defined UDP multicast receptions (specific IP addresses in the range 224.0.0.0 to 239.255.255.255)
                #define MAX_MULTICAST_MEMBERS   1

          //#define ELZET80_DEVICE_DISCOVERY
            #if defined ELZET80_DEVICE_DISCOVERY
                #define ELZET80_DISCOVERY_UDP_PORT  1338                 // listen on this UDP port
                #if defined REMOTE_SIMULATION_INTERFACE
                    #define USER_UDP_SOCKETS   3
                #else
                    #define USER_UDP_SOCKETS   2                         // we reserve one UDP socket for device discovery and one non-standard UDP socket in case the UDP demo is required
                #endif
            #else
                #if defined REMOTE_SIMULATION_INTERFACE
                    #define USER_UDP_SOCKETS   2
                #else
                    #define USER_UDP_SOCKETS   1                         // we reserve one non-standard UDP socket in case the UDP demo is required
                #endif
            #endif

            #define USE_DHCP_CLIENT                                      // enable DHCP  - needs UDP - IPCONFIG default zero. Needs 1k Ethernet RX Buffers!! (set random number also)
                #define DHCP_HOST_NAME                                   // we send our host name as DHCP option - the application must supply fnGetDHCP_host_name()
          //#define USE_DNS                                              // enable DNS   - needs UDP
                #define DNS_SERVER_OWN_ADDRESS                           // command line menu allows DNS server address to be set, otherwise it uses the default gateway
          //#define USE_TFTP                                             // enable TFTP  - needs UDP
            #define USE_NETBIOS                                          // enable NetBIOS - needs UDP
          //#define USE_SNMP
                #define SUPPORT_SNMPV2C                                  // SNMPV2c as well as SNMPV1
                #define SNMP_MANAGER_COUNT        3                      // the number of managers supported
                #define SNMP_TRAP_QUEUE_LENGTH    8                      // traps that can be queued to each manager
                #define SNMP_MAX_BUFFER           512                    // largest UDP content size (in bytes - cannot be larger than maximum UDP data length)
          //#define USE_SNTP                                             // simple network time protocol
                #define SNTP_SERVERS              4                      // number of SNTP servers that are used

            #if defined USE_DHCP_CLIENT
                #define DHCP_SOCKET 1
            #else
                #define DHCP_SOCKET 0
            #endif
            #if defined USE_DNS
                #define DNS_SOCKET 1
            #else
                #define DNS_SOCKET 0
            #endif
            #if defined USE_TFTP
                #define TFTP_SOCKET 1
            #else
                #define TFTP_SOCKET 0
            #endif
            #if defined USE_SNMP
                #define SNMP_SOCKET 1
            #else
                #define SNMP_SOCKET 0
            #endif
            #if defined USE_SNTP
                #define SNTP_SOCKET 1
            #else
                #define SNTP_SOCKET 0
            #endif
            #if defined USE_NETBIOS
                #define NETBIOS_SOCKET 1
                #define SUPPORT_SUBNET_BROADCAST                         // NetBIOS needs to receive sub-net broadcast frames
            #else
                #define NETBIOS_SOCKET 0
            #endif
            #define UDP_SOCKETS   (DHCP_SOCKET + DNS_SOCKET + SNMP_SOCKET + TFTP_SOCKET + NETBIOS_SOCKET + USER_UDP_SOCKETS + SNTP_SOCKET) // the number of UDP sockets supported
        #endif

        #if defined USE_FTP                                              // specify FTP support details
            #define FTP_CONNECTIONS             2
          //#define ANONYMOUS_LOGIN                                      // allow anonymous FTP without user/password
            #define FTP_SOCKETS     (FTP_CONNECTIONS * 2)                // reserve 2 TCP sockets for each command and data connection
            #define FTP_SUPPORTS_NAME_DISPLAY                            // show details of files
            #define FTP_SUPPORTS_DELETE                                  // enable delete of files via FTP
            #define FTP_SUPPORTS_DOWNLOAD                                // support read of files via FTP
            #define FTP_VERIFY_DATA_PORT                                 // check for valid data port when in active mode
            #define FTP_PASV_SUPPORT                                     // allow passive mode of operation
            #define DATA_PORT_TRIGGERS_CONTROL                           // activity on the FTP data port resets the control port TCP idle timer - stops control connection timing out on large data transfers with short idle time value
            #define FTP_USER_LOGIN
            #define FTP_WILDCARD_DEL                                     // file system content delete using *.* supported
            #define FTP_DATA_WINDOWS            2                        // allow transmission of FTP data with windowing support
            #if defined INTERNAL_USER_FILES
                #define FTP_DISPLAY_USER_FILES                           // allow user files to be displayed by FTP
            #endif
            #if defined SDCARD_SUPPORT || defined SPI_FLASH_FAT || defined FLASH_FAT // disk D: is used
                #define FTP_UTFAT                                        // allow FTP to work with utFAT
                #define FTP_ROOT                "/"                      // the root directory as seen by the FTP server (can be set to a sub-directory of the main disk to restrict FTP accesses to there)
                #define UT_FTP_PATH_LENGTH      128                      // this must be non-zero to enable PWD request to return a path name
            #endif
            #define MANAGED_FTP_DELETES                                  // FTP deletes managed
        #else
            #define FTP_SOCKETS 0                                        // no TCP sockets needed
        #endif

        #if defined USE_FTP_CLIENT
          //#define FTP_CLIENT_EXTERN_DATA_SOCKET                        // let application use its own TCP socket for data transfer
          //#define FTP_CLIENT_BUFFERED_SOCKET_MODE                      // FTP client data socket uses buffered TCP socket rather than simple TCP socket
              #define FTP_CLIENT_TCP_BUFFER_LENGTH 512                   // size of optional FTP client data socket TCP buffer
            #define MAX_FTP_CLIENT_PATH_LENGTH 64                        // maximum path string for relative directory referencing
            #if defined FTP_CLIENT_EXTERN_DATA_SOCKET
                #define FTP_CLIENT_SOCKETS  1                            // the user needs to supply the data socket
            #else
                #define FTP_CLIENT_SOCKETS  2                            // FTP client supplies control and data sockets
            #endif
        #else
            #define FTP_CLIENT_SOCKETS  0                                // no TCP sockets needed
        #endif

        #if defined USE_POP3
            #define POP3_SOCKET 1                                        // reserve a TCP socket for POP3 use
        #else
            #define POP3_SOCKET 0                                        // no TCP socket needed
        #endif

        #if defined USE_TIME_SERVER
            #define TIME_SERVER_SOCKET 1                                 // reserve a TCP socket for time server use use
        #else
            #define TIME_SERVER_SOCKET 0                                 // no TCP socket needed
        #endif

        #if defined MODBUS_TCP
          //#define MODBUS_TCP_GATEWAY                                   // support MODBUS TCP <-> MODBUS serial gateway
            #if defined USE_MODBUS_MASTER                                // following options require master support
                #define TCP_SLAVE_ROUTER                                 // TCP slave router support
                #define MODBUS_TCP_MASTERS    2                          // support MODBUS TCP masters
            #else
                #define MODBUS_TCP_MASTERS    0                          // no TCP masters
            #endif
            #define MODBUS_TCP_SERVERS    2                              // individual
            #define MODBUS_SOCKETS_0      5                              // the number of sockets (sessions) allocated to first MODBUS TCP slave port
            #define MODBUS_SOCKETS_1      3                              // the number of sockets (sessions) allocated to second MODBUS TCP slave port
            #define MODBUS_SHARED_TCP_INTERFACES 0                       // number of slave interfaces sharing TCP sessions
            #define MODBUS_TCP_SOCKETS    (MODBUS_SOCKETS_0 + MODBUS_SOCKETS_1 + MODBUS_TCP_MASTERS) // total number of sockets required by MODBUS TCP
        #else
            #define MODBUS_TCP_SOCKETS    0
            #define MODBUS_TCP_SERVERS    0
        #endif

        #if defined USE_SMTP
            #define SMTP_MESSAGE_LEN   300                               // frame length of SMTP transmissions (temporary on stack)
            #define SMTP_SOCKET 1                                        // reserve a TCP socket for SMTP use

            #define OUR_USER_DOMAIN                       "my_domain.com";
            #define DEFAULT_DESTINATION_EMAIL_ADDRESS     {'t','e','s','t','_','n','a','m','e','@','t','e','s','t','.','c','o','m',0}; //"test_name@test.com"; declare as array to avoid newer versions of CodeWarrior from putting the string in flash
            #define SMTP_PROVIDER_IP_ADDRESS              { 194,158,229,11 }
            #define SMTP_PROVIDER_ADDRESS                 "mail.provider.com"
          //#define SMTP_PARAMETERS
            #define USE_SMTP_AUTHENTICATION
        #else
            #define SMTP_SOCKET 0                                        // no TCP socket needed
        #endif

        #if defined USE_MQTT_CLIENT
            #define MQTT_CLIENT_SOCKET 1
        #else
            #define MQTT_CLIENT_SOCKET 0
        #endif

        #if defined USE_TELNET || defined USE_TELNET_CLIENT
            #define USE_BUFFERED_TCP                                     // always use buffered TCP for telnet
        #endif
        #if defined USE_TELNET_CLIENT
            #define TELNET_CLIENT_COUNT   2                              // allow up to 2 parallel TELNET client interfaces
        #else
            #define TELNET_CLIENT_COUNT   0
        #endif
        #if defined USE_TELNET
            #if defined TEST_TCP_SERVER
                #define NO_OF_TELNET_SESSIONS   (TELNET_CLIENT_COUNT + 2) // reserve sockets TELNET client and server sessions
            #else
                #define NO_OF_TELNET_SESSIONS   (TELNET_CLIENT_COUNT + 1) // reserve sockets TELNET client and server sessions
            #endif
        #else
            #define NO_OF_TELNET_SESSIONS   TELNET_CLIENT_COUNT          // no sockets reserved for Telnet server
        #endif

        #if defined USE_BUFFERED_TCP                                     // if using a buffer for TCP to allow interractive data applications (like TELNET)
            #define WAKE_BLOCKED_TCP_BUF                                 // support waking blocked TCP buffer (with USE_BUFFERED_TCP)
            #define INDIVIDUAL_BUFFERED_TCP_BUFFER_SIZE                  // allow each buffered TCP user to define their own TCP buffer size
        #endif

        #if defined USE_HTTP                                             // specify HTTP support details
          //#define _VARIABLE_HTTP_PORT                                  // allow HTTP server to be started with user-defined port number
            #define HTTP_WINDOWING_BUFFERS     2                         // we can send 2 frames before requiring ACKs
            #define HTTP_IGNORE_PARTIAL_ACKS                             // when transmission error rates are low, ignore partial ack handling in favour of retransmission on timeout
              //#define HTTP_REPEAT_DOUBLE_ACKS                          // if the peer responds with an ACK that is not acknowledging and transmitted data we retransmit all data on the fly to avoid possible retransmission delays (workaround for a wifi based system with this behaviour)

            #define WEB_PARAMETER_GENERATION                             // support of parameter generating (eg. manipulating select and adding values)
            #define WEB_PARAMETER_HANDLING                               // support  handling of received web parameters
            #define WEB_PARSER_START          '£'                        // this symbol is used in Web pages to instruct parsing to begin
            #define WEB_INSERT_STRING         'v'
            #define WEB_DISABLE_FIELD         'D'
            #define WEB_NOT_DISABLE_FIELD     'd'
            #define WEB_SELECTED_FIELD        's'
            #define WEB_INSERT_DYNAMIC        'H'
          //#define WEB_ESCAPE_LEN             5                         // defaults to 4 if not defined
            #define FILE404 (uFILE_SYSTEM_START + (FILE_GRANULARITY * (LAST_FILE_BLOCK - 1))) // last block fixed for 404 error page
            #if !defined SPI_FILE_SYSTEM || defined FLASH_FILE_SYSTEM    // SPI file system requires 404 file also to be in file
                #define FILE404_IN_PROG                                  // fixed FILE404 in Code (no NE64 support since it pages the file system in memory)
            #endif
            #define FILE_404_CONTENT        "<html><head><title>uTasker No file</title></head><body bgcolor=#ff9000 text=#000000 topmargin=3 marginheight=3><center><td valign=top class=h><font color=#ff0000 style=font-size:30px><b style='mso-bidi-font-weight:normal'>uTasker</font> - Error 404</i></b></td><br></td><td align=left><br><br>The requested file has not been found on the server. Please check that html files have been loaded correctly.<br></font></td></tr></body></html>"

            #define SUPPORT_CHROME                                       // always answer with HTTP header so that Chrome accepts data (this is a standard setting)
                #define _EXTENDED_HTTP_MIME_CONTENT_SUPPORT              // add content type to HTTP header (needed often with newer browser versions)

            #define SUPPORT_HTTP_POST                                    // support file uploads using POST command
            #define RESTRICT_POST_CONTENT_TYPE                           // filter allowed posting formats
            #if defined RESTRICT_POST_CONTENT_TYPE
                #define SUPPORT_POST_BINARY
                #define SUPPORT_POST_TEXT
                #define SUPPORT_POST_GIF
                #define SUPPORT_POST_BMP
                #define SUPPORT_POST_PDF
            #endif
          //#define VARIABLE_PAGE_AUTHENTICATION
            #define PLAIN_TEXT_POST                                      // allow posting parameters
          //#define HTTP_POST_DEFINES_PAGE                               // return the user file as specified by the file name in a POST, rather than special uFile name
            #define SUPPORT_HTTP_CONTINUE                                // respond to Expect: 100-continue to start a posting sequence without any delay (useful when working with cURL)
            #define X_WWW_FORM_URLENCODED                                // this encoding is also understood as plain text post (used in addition to PLAIN_TEXT_POST)
            #define AUTO_ASCII_PLAIN_TEXT_DECODE                         // automatically decode any ASCII characters send as "%xx" - this require '%' to be avoided in post content! (Suggested for use with Chrome)
            #define SUPPORT_HTTP_POST_TO_APPLICATION                     // allow application to handle posted data rather than always saving to file
            #define SUPPORT_DELAY_WEB_SERVING                            // enable delayed web page serving defined by the application

            #define NO_OF_HTTPS_SESSIONS       0                         // this many parallel HTTPS sockets are reserved for this many parallel sessions
            #define NO_OF_HTTP_SESSIONS        4                         // this many parallel HTTP sockets are reserved for this many parallel sessions
            #define HTTP_AUTHENTICATION                                  // activate basic authentication
            #define PROJECT_CREDENTIALS  "HTTP/1.0 401\r\nWWW-Authenticate: Basic realm=""uTasker""\r\n\r\n" // put project name here to spice up credential pop-up
                                                                         //               ^^^^^^^
          //#define PROJECT_CREDENTIALS  "HTTP/1.0 401\r\nWWW-Authenticate: Digest algorithm=MD5 realm=""uTasker"" nonce=""61ff892f""\r\n\r\n" // put project name here to spice up credential pop-up
            #define HTTP_DYNAMIC_CONTENT                                 // activate support of dynamic content generation
            #define DYNAMIC_DATA_FIXED_LENGTH  0
            #if defined HTTP_DYNAMIC_CONTENT
                #define HTTP_USER_DETAILS                                // enable the application to manage details belonging to a particalar connection
            #endif
            #define HTTP_HEADER_CONTENT_INFO                             // add plain text HTTP header information - ensures that Internet Explorer correctly displays txt files
            #if defined SDCARD_SUPPORT || defined SPI_FLASH_FAT || defined FLASH_FAT // disk D: i sused
                #define HTTP_UTFAT                                       // allow HTTP to work with utFAT
                #define HTTP_ROOT              "web"                     // the root directory as seen by the HTTP server (can be set to a sub-directory of the main disk to restrict HTTP accesses to there)
                #define DEFAULT_HTTP_FILE      "index.htm"               // the file served when first contact is made with the web server
            #endif
          //#define HTTP_SET_USERFILE_START_PAGE                         // allow a specific user file to act as HTTP default start side
                #define HTTP_START_SIDE        "index.html"              // start side used with user files
          //#define HTTP_SERVE_ONLY_USER_FILES                           // only serve user file - if a defined user file is not found display 404 error rather than a uFileSystem equivalent
        #else
            #define NO_OF_HTTP_SESSIONS        0                         // no TCP sockets are reserved for HTML
            #define NO_OF_HTTPS_SESSIONS       0
        #endif

        #define USER_NAME_AND_PASS                                       // routines for user name and password support

        #define NO_OF_TCPSOCKETS (NO_OF_HTTP_SESSIONS + NO_OF_HTTPS_SESSIONS + FTP_SOCKETS + POP3_SOCKET + SMTP_SOCKET + NO_OF_TELNET_SESSIONS + TIME_SERVER_SOCKET + MODBUS_TCP_SOCKETS + FTP_CLIENT_SOCKETS + MQTT_CLIENT_SOCKET + USER_TCP_SOCKETS) // reserve the number of TCP sockets necessary for our configuration
    #endif
#else                                                                    // else no LAN support
    #define NUMBER_LAN     0                                             // no physical queue needed
#endif

#if defined USE_MODBUS && defined USE_MODBUS_MASTER && defined MODBUS_GATE_WAY_QUEUE
    #if !defined MODBUS_TCP
        #define MODBUS_TCP_SERVERS       0
        #define MODBUS_TCP_MASTERS       0
    #endif
    #if!defined MODBUS_SERIAL_INTERFACES
        #define MODBUS_SERIAL_INTERFACES 0
    #endif
    #define NUMBER_MODBUS_QUEUES  (MODBUS_SERIAL_INTERFACES + MODBUS_TCP_SERVERS + MODBUS_TCP_MASTERS) // reserve one queue for each serial MODBUS port and each MODBUS tcp socket
#else
    #define NUMBER_MODBUS_QUEUES  0
#endif
#if defined SUPPORT_FIFO_QUEUES
    #define NUMBER_FIFO_QUEUES    3                                      // the number of software FIFOs required
#else
    #define NUMBER_FIFO_QUEUES    0
#endif

#define PHYSICAL_QUEUES   (NUMBER_SERIAL + NUMBER_EXTERNAL_SERIAL + NUMBER_LAN + NUMBER_I2C + NUMBER_CAN + NUMBER_USB + NUMBER_MODBUS_QUEUES + NUMBER_FIFO_QUEUES) // the number of physical queues in the system

#define RANDOM_NUMBER_GENERATOR                                          // support a random number generator (useful for DHCP and possibly DNS)

//#define SUPPORT_LCD                                                    // enable a task for interfacing to a character LCD
#if defined SUPPORT_LCD
    #define LCD_LINES              2                                     // use 2 x 16 LCD
    #define LCD_CHARACTERS         16                                    // options are 1:8 / 1:16 / 1:20 / 1:24 / 1:40 / 2:x / 4:x
    #define LCD_ON_COLOUR          (COLORREF)RGB(60,220,60)              // RGB colour of LCD when backlight is on
    #define LCD_OFF_COLOUR         (COLORREF)RGB(70,160,0)               // RGB colour of LCD when backlight is off
    #define LCD_PIXEL_COLOUR       (COLORREF)RGB(0,0,0)                  // RGB colour of LCD pixels
    #define LCD_PARTNER_TASK       TASK_APPLICATION
  //#define LCD_CYRILLIC_FONT                                            // use cyrillic extended ASCII character set instead of standard extended
#endif
#if defined KINETIS_K30 || defined KINETIS_K40 || defined KINETIS_K51 || defined KINETIS_K53 || (defined KINETIS_KL40  && !defined KL43Z_256_32_CL) // if processor supports SLCD
  //#define SUPPORT_SLCD                                                 // segment LCD
#endif
#if defined BLAZE_K22
    #define SUPPORT_GLCD                                                 // enable the task for interfacing to a graphical LCD
    #define TFT2N0369_GLCD_MODE                                          // use colour TFT in GLCD compatible mode (TWR-LCD)
    #define ST7789S_GLCD_MODE
    #define SUPPORT_TOUCH_SCREEN
    #define TOUCH_FT6206                                                 // FT6206 capacitative touch panal controller
#elif defined NET_K60	|| ((defined NET_KBED || defined rcARM_KL26) && defined SPI_LCD)                  
    #define SUPPORT_GLCD                                                 // enable the task for interfacing to a graphical LCD
    #define ST7565S_GLCD_MODE                                            // use ST7565 LCD in compatible mode
#elif defined K70F150M_12M
    #define SUPPORT_GLCD                                                 // enable the task for interfacing to a graphical LCD
    #define SUPPORT_TOUCH_SCREEN                                         // touch screen operation
    #define TOUCH_MOUSE_TASK    TASK_APPLICATION                         // application received touch mouse events
#else
  //#define SUPPORT_GLCD
  //#define SUPPORT_TOUCH_SCREEN                                         // touch screen operation
#endif

#if defined SUPPORT_GLCD
    #if defined ST7565S_GLCD_MODE
        #define BIG_PIXEL                                                // display double size in the simulator
    #else
      //#define BIG_PIXEL                                                // display double size in the simulator
      //#define OLED_GLCD_MODE                                           // use an OLED in GLCD compatible mode
      //#define NOKIA_GLCD_MODE                                          // use Nokia 6100 LCD in compatible mode
      //#define CGLCD_GLCD_MODE                                          // use colour LCD in GLCD compatible mode
      //#define KITRONIX_GLCD_MODE                                       // use colour TFT in GLCD compatible mode (IDM_L35_B)
      //#define MB785_GLCD_MODE                                          // use colour TFT in GLCD compatible mode (STM321C-EVAL)
      //#define TFT2N0369_GLCD_MODE                                      // use colour TFT in GLCD compatible mode (TWR-LCD)
        #if defined TWR_K70F120M || defined K70F150M_12M
            #define TWR_LCD_RGB_GLCD_MODE                                // use colour TFT in GLCD compatible mode (TWR-LCD-RGB)
            #define TFT_GLCD_MODE                                        // use a TFT in GLCD compatible mode (only use together with LCD controller)
        #endif
    #endif
    #define SPECIAL_LCD_DEMO                                             // when images are posted to the display, stop the CGLCD_GLCD_MODE demonstration so that the image remains
    #define LCD_PARTNER_TASK       TASK_APPLICATION                      // GLCD task sends initialise complete event and acks to this task
    #if defined BLAZE_K22
        #define GLCD_X             240                                   // horizontal resolution of the GLCD in pixels
        #define GLCD_Y             240                                   // vertical resolution of the GLCD in pixels
        #undef BIG_PIXEL
        #define CGLCD_PIXEL_SIZE   1                                     // for each CGLCD pixel use 2 physical pixels in x and y directions - also reduces GLCD memory requirements
    #elif defined OLED_GLCD_MODE
        #define GLCD_X             128                                   // horizontal resolution of the GLCD in pixels
        #define GLCD_Y             96                                    // vertical resolution of the GLCD in pixels
        #define CGLCD_PIXEL_SIZE   1                                     // for each CGLCD pixel use 1 physical pixels in x and y directions
    #elif defined NOKIA_GLCD_MODE
        #define GLCD_X             130                                   // horizontal resolution of the GLCD in pixels - 132 x 132 but two not visible
        #define GLCD_Y             130                                   // vertical resolution of the GLCD in pixels
        #undef BIG_PIXEL
        #define CGLCD_PIXEL_SIZE   1                                     // for each CGLCD pixel use 1 physical pixels in x and y directions
    #elif defined TWR_LCD_RGB_GLCD_MODE
        #define TWR_RGB_LCD_REV_E                                        // latest hardware revision type available
        #if defined K70F150M_12M
            #define GLCD_X         800                                   // horizontal resolution of the GLCD in pixels (WQVGA)
            #define GLCD_Y         480                                   // vertical resolution of the GLCD in pixels
            #define CGLCD_PIXEL_SIZE   4                                 // for each CGLCD pixel use 4 physical pixels in x and y directions - also reduces GLCD memory requirements
        #else
            #define GLCD_X         480                                   // horizontal resolution of the GLCD in pixels (WQVGA)
            #define GLCD_Y         272                                   // vertical resolution of the GLCD in pixels
            #define CGLCD_PIXEL_SIZE   2                                 // for each CGLCD pixel use 2 physical pixels in x and y directions - also reduces GLCD memory requirements
        #endif
        #undef BIG_PIXEL
    #elif defined TFT_GLCD_MODE || defined AVR32_EVK1105 || defined KITRONIX_GLCD_MODE || defined TFT2N0369_GLCD_MODE || defined MB785_GLCD_MODE
        #define GLCD_X             320                                   // horizontal resolution of the GLCD in pixels
        #define GLCD_Y             240                                   // vertical resolution of the GLCD in pixels
        #undef BIG_PIXEL
        #define CGLCD_PIXEL_SIZE   2                                     // for each CGLCD pixel use 2 physical pixels in x and y directions - also reduces GLCD memory requirements
    #elif defined ST7565S_GLCD_MODE
        #define GLCD_X             128                                   // horizontal resolution of the GLCD in pixels	          
        #define GLCD_Y             64                                    // vertical resolution of the GLCD in pixels	         
        #define CGLCD_PIXEL_SIZE   1                                     // for each CGLCD pixel use 1 physical pixels in x and y directions
    #else
        #define GLCD_X             160                                   // horizontal resolution of the GLCD in pixels
        #define GLCD_Y             80                                    // vertical resolution of the GLCD in pixels
        #define CGLCD_PIXEL_SIZE   1                                     // for each CGLCD pixel use 1 physical pixels in x and y directions
    #endif
    #if defined OLED_GLCD_MODE
        #define LCD_PIXEL_COLOUR   (COLORREF)RGB(255,255,255)            // RGB colour of LCD pixel when on
        #define LCD_ON_COLOUR      (COLORREF)RGB(0,0,0)                  // RGB colour of LCD when backlight is on
        #define LCD_OFF_COLOUR     (COLORREF)RGB(0,0,0)                  // RGB colour of LCD when backlight is off
    #elif defined TFT_GLCD_MODE || defined NOKIA_GLCD_MODE || defined CGLCD_GLCD_MODE || defined KITRONIX_GLCD_MODE || defined TFT2N0369_GLCD_MODE || defined MB785_GLCD_MODE || defined TWR_LCD_RGB_GLCD_MODE
        #define LCD_PIXEL_COLOUR   (COLORREF)RGB(255,255,0)              // RGB colour of LCD pixel when on
        #define LCD_ON_COLOUR      (COLORREF)RGB(0,0,255)                // RGB colour of LCD when backlight is on
      //#define LCD_ON_COLOUR      (COLORREF)RGB(0,0,85)                 // RGB colour of LCD when backlight is on
        #define LCD_OFF_COLOUR     (COLORREF)RGB(0,0,0)                  // RGB colour of LCD when backlight is off
        #define NOKIA_EPSON_S1D15G00                                     // Nokia (type) CGLCD can have either this chip or
      //#define PHILIPS_PCF8833                                          // this one
    #elif defined ST7565S_GLCD_MODE
        #define LCD_PIXEL_COLOUR   (COLORREF)RGB(10,10,10)               // RGB colour of LCD pixel when on
        #define LCD_ON_COLOUR      (COLORREF)RGB(215,215,45)             // RGB colour of LCD when backlight is on
        #define LCD_OFF_COLOUR     (COLORREF)RGB(80,80,0)                // RGB colour of LCD when backlight is off
    #else
      //#define _GLCD_SAMSUNG                                            // Samsung controller based display rather than Toshiba
        #if defined _GLCD_SAMSUNG
            #undef GLCD_X
            #undef GLCD_Y
            #define GLCD_X         128                                   // horizontal resolution of the GLCD in pixels
            #define GLCD_Y         64                                    // vertical resolution of the GLCD in pixels
        #endif
        #define SAMSUNG_CHIPS      (GLCD_X/64)                           // Samsung controller has fixed 64 x 64 per chip and may need multiple controllers
        #define LCD_PIXEL_COLOUR   (COLORREF)RGB(255,255,255)            // RGB colour of LCD pixel when on
        #define LCD_ON_COLOUR      (COLORREF)RGB(0,0,255)                // RGB colour of LCD when backlight is on
        #define LCD_OFF_COLOUR     (COLORREF)RGB(0,0,255)                // RGB colour of LCD when backlight is off
    #endif
#endif
#if defined SUPPORT_GLCD
    #define USER_SPECIAL_SEVEN_DOT      0                                // extend 7 dot font by 0 user characters
    #define USER_SPECIAL_NINE_DOT       3                                // extend 9 dot font by 3 user characters
    #define USER_SPECIAL_TEN_DOT        0                                // extend 10 dot font by 0 user characters
    #define USER_SPECIAL_FIFTEEN_DOT    0                                // extend 15 dot font by 0 user characters
    #define USER_SPECIAL_EIGHTEEN_DOT   0                                // extend 18 dot font by 0 user characters

    #define EN_FIVE_DOT                                                  // define the fonts that should be available for use
    #define EN_FIVE_DOT_FIX
    #define EN_SIX_DOT
    #define EN_SIX_DOT_FIX
    #define EN_SEVEN_DOT
    #define EN_NINE_DOT
    #define EN_TEN_DOT
    #define EN_FIFTEEN_DOT
    #define EN_EIGHTEEN_DOT
    #define EN_CHAR_LCD_DOT
    #define MAX_BLINKING_OBJECTS   3                                     // the number of blinking objects to be supported - comment out when not required
    #define MAX_TEXT_LENGTH        64                                    // maximum text length when writing fonts
#endif

//#define SUPPORT_OLED                                                   // enable the task for interfacing to a graphical OLED
#if defined SUPPORT_OLED
    #define LCD_PARTNER_TASK       TASK_APPLICATION
    #define GLCD_X                 128                                   // horizontal resolution of the OLED display in pixels
    #define GLCD_Y                 96                                    // vertical resolution of the OLED display in pixels
    #define LCD_ON_COLOUR          (COLORREF)RGB(0,0,0)                  // RGB colour of LCD when backlight is on
    #define LCD_OFF_COLOUR         (COLORREF)RGB(0,0,0)                  // RGB colour of LCD when backlight is off
    #define BIG_PIXEL
    #define LCD_PIXEL_COLOUR       (COLORREF)RGB(255,255,255)
    #define EN_FIVE_DOT                                                  // define the fonts that should be available
    #define EN_FIVE_DOT_FIX
    #define EN_SIX_DOT
    #define EN_SIX_DOT_FIX
    #define EN_SEVEN_DOT
    #define EN_NINE_DOT
    #define EN_TEN_DOT
    #define EN_FIFTEEN_DOT
    #define EN_EIGHTEEN_DOT
    #define EN_CHAR_LCD_DOT
    #define MAX_BLINKING_OBJECTS   3
    #define MAX_TEXT_LENGTH        64                                    // maximum text length when writing fonts
#endif

#if defined NET_K60 || defined rcARM_KL26                                // NET-K60 enables key pad or rcARM touch buttons
    #define SUPPORT_KEY_SCAN                                             // enable a task for key pad scanning
#else
  //#define SUPPORT_KEY_SCAN                                             // enable a task for key pad scanning
#endif
#if defined SUPPORT_KEY_SCAN
    #if defined FRDM_KL26Z || defined FRDM_KL25Z
        #define USE_SLIDER                                               // 2 keys combined as a slider
        #define KEY_ROWS              2
    #elif defined rcARM_KL26
        #define KEY_ROWS              3
    #else
        #define KEY_ROWS              4                                  // 4 rows by
    #endif
    #if defined NET_K60
        #define KEY_COLUMNS           2                                  // NET-K60 2 columns (matrix keypad inputs)
    #else	
        #define KEY_COLUMNS           0                                  // 4 columns (set to zero for non-matrix keypad inputs)
    #endif
    #define KEYPAD_PARTNER_TASK       TASK_APPLICATION                   // key change events sent to this task

    #if defined FRDM_KL26Z || defined FRDM_KL25Z
        #define VIRTUAL_KEY_ROWS      2
    #elif defined rcARM_KL26
        #define VIRTUAL_KEY_ROWS      3
    #else
        #define VIRTUAL_KEY_ROWS      4                                  // virtual rows and columns when KEY_COLUMNS is zero
    #endif
    #define VIRTUAL_KEY_COLUMNS       1
#endif


#if defined SUPPORT_DISTRIBUTED_NODES
    #define GLOBAL_TIMER_TASK
#elif defined USE_MODBUS                                                 // MODBUS generally requires multiple SW timer support
    #define GLOBAL_TIMER_TASK
#elif defined USE_IGMP && ((defined USE_IGMP_V2 || defined USE_IGMP_V3) || (IGMP_MAX_HOSTS > 1))
    #define GLOBAL_TIMER_TASK
#else
    #define GLOBAL_TIMER_TASK                                            // enable a task for global timer tasks
#endif

#if defined GLOBAL_TIMER_TASK
    #if defined USE_IGMP && (IGMP_MAX_HOSTS > 1)
        #define TIMER_QUANTITY (IGMP_MAX_HOSTS + 10)                     // the number of global timers required
    #else
        #define TIMER_QUANTITY 10                                        // the number of global timers required
    #endif
#endif

#if !((defined K70F150M_12M || defined TWR_K70F120M || defined TWR_K60F120M || defined K60F150M_50M) && defined USB_INTERFACE) // don't use low power mode due to errata e7166
    #define SUPPORT_LOW_POWER                                            // a low power task supervises power reduction when possible
#endif

#define SUPPORT_DOUBLE_QUEUE_WRITES                                      // allow double queue writes to improve efficiency of long queue copies
//#define MULTISTART                                                     // enable a board to user multiple task configurations
//#define PERIODIC_TIMER_EVENT                                           // delayed and periodic tasks are schedule with timer events if enabled (otherwise they are simply scheduled)


#if defined BLINKY                                                       // if the BLINKY operation is defined we ensure that the following are disabled to give simplest configuration
    #undef USE_MAINTENANCE
    #undef USB_INTERFACE
    #undef USB_HOST_SUPPORT
    #undef SERIAL_INTERFACE
    #undef I2C_INTERFACE
    #undef CAN_INTERFACE
    #undef SUPPORT_KEY_SCAN
    #undef ETH_INTERFACE
    #undef SUPPORT_GLCD
    #undef SUPPORT_OLED
    #undef SUPPORT_GLCD
    #undef CMSIS_DSP_CFFT
    #undef CRYPTOGRAPHY
    #undef SDCARD_SUPPORT
    #undef USB_MSD_HOST
    #undef UTFAT_LFN_READ
    #undef UTFAT_WRITE
    #undef UTFAT_EXPERT_FUNCTIONS
    #undef FLASH_FAT
    #undef SPI_FLASH_FAT
    #undef FLASH_FILE_SYSTEM
    #undef SPI_FILE_SYSTEM
    #undef USE_PARAMETER_BLOCK
    #undef INTERNAL_USER_FILES
    #undef ACTIVE_FILE_SYSTEM
    #undef MANAGED_FILES
    #undef UTMANAGED_FILE_COUNT
    #undef GLOBAL_TIMER_TASK
    #undef USE_MODBUS
    #undef RANDOM_NUMBER_GENERATOR
    #undef QUICK_DEV_TASKS
    #define NO_FLASH_SUPPORT
    #define REMOVE_PORT_INITIALISATIONS
    #define NO_PERIPHERAL_DEMONSTRATIONS
#endif

// Project includes are set here for all files in the correct order
//
#include "types.h"                                                       // project specific type settings (include the hardware configuration headers)
#if defined SDCARD_SUPPORT || defined SPI_FLASH_FAT || defined FLASH_FAT || defined USB_MSD_HOST || defined MANAGED_FILES || defined FAT_EMULATION
    #include "../../uTasker/utFAT/mass_storage.h"
#endif
#include "../../stack/tcpip.h"                                           // TCP/IP stack and web utilities
#include "../../uTasker/driver.h"                                        // driver and general formatting routines
#include "../../Hardware/hardware.h"                                     // general hardware
#include "../../uTasker/uTasker.h"                                       // operating system defines
#if defined USE_MODBUS
    #include "../../uTasker/MODBUS/modbus.h"
#endif
#include "TaskConfig.h"                                                  // the specific task configuration
#include "application.h"                                                 // general project specific include
#include "../../uTasker/uGLCDLIB/glcd.h"                                 // LCD
#if defined _WINDOWS
    #include "../../WinSim/WinSim.h"
#endif

#if defined OPSYS_CONFIG                                                 // this is only set in the hardware module
    #if defined ETH_INTERFACE || defined USB_CDC_RNDIS                   // if we support Ethernet we define some constants for its (TCP/IP) use
        const unsigned char cucNullMACIP[MAC_LENGTH] = { 0, 0, 0, 0, 0, 0 };
        const unsigned char cucBroadcast[MAC_LENGTH] = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff }; // used also for broadcast IP
    #endif

    #if defined SUPPORT_MIME_IDENTIFIER
    const CHAR *cMimeTable[] = {                                         // keep the ordering in the table to match the MIME type defines below!!
        (const CHAR *)"HTM",                                             // HTML file - will be interpreted by web server
        (const CHAR *)"JPG",                                             // JPG image
        (const CHAR *)"GIF",                                             // GIF image
        (const CHAR *)"PNG",                                             // PNG image
        (const CHAR *)"CSS",                                             // CSS Cascading Style Sheets
        (const CHAR *)"JS",                                              // Java script
        (const CHAR *)"BIN",                                             // binary data file
        (const CHAR *)"TXT",                                             // text data file
        (const CHAR *)"ICO",                                             // icon
        (const CHAR *)"BMP",                                             // BMP image
        (const CHAR *)"???",                                             // all other types will be displayed as unknown
    };
    #endif
#endif

// File type identifiers
//
#define MIME_HTML                  0                                     // this and any lower types will be parsed by the web server
#define MIME_JPG                   1
#define MIME_GIF                   2
#define MIME_PNG                   3
#define MIME_CSS                   4
#define MIME_JAVA_SCRIPT           5
#define MIME_BINARY                6
#define MIME_TXT                   7
#define MIME_ICON                  8
#define MIME_BMP                   9
#define UNKNOWN_MIME               10                                    // this entry is needed to terminate the list

#define PLAIN_TEXTCONTENT(x) (x == MIME_TXT)                             // list of content types considered as plain text

#endif
#endif
