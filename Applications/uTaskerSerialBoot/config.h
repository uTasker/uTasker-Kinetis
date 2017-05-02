/*********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher

    ---------------------------------------------------------------------
    File:      config.h
    Project:   Serial Loader (SREC/iHex serial, USB-MSD, SD card)
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2016
    *********************************************************************
    02.02.2017 Adapt for us tick resolution (_TICK_RESOLUTION)

*/

#if !defined __CONFIG__
    #define __CONFIG__
                  

#if !defined _ASSEMBLER_CONFIG                                           // remove all following when used for assembler configuration

#define _TICK_RESOLUTION     TICK_UNIT_MS(50)                            // 50ms system tick period - max possible at 50MHz SYSTICK would be about 335ms !

#if defined _WINDOWS
    #define MEM_FACTOR 1.0                                               // Windows tends to use more memory so expand heap slightly
#else
    #define MEM_FACTOR 1.0
#endif

#define _BOOT_LOADER                                                     // set for compatibility with applications that require adequate uninitialised variables at the top of RAM

#define _NO_CHECK_QUEUE_INPUT                                            // code size optimisations
#define _MINIMUM_IRQ_INITIALISATION


// Major hardware dependent settings for this project (choice of board - select only one at a time)
//
//#define FRDM_KE02Z                                                     // E processors Cortex-M0+ (5V robust)
//#define FRDM_KE02Z40M
//#define FRDM_KE04Z                                                     // this board's device has 8k Flash and 1k SRAM and so is generally not practical to use boot loader with the serial loader
//#define FRDM_KE06Z

//#define TRK_KEA64                                                      // EA ultra-reliable automotive processors Cortex-M0+
//#define TRK_KEA128

//#define FRDM_KL02Z                                                     // L processors Cortex-M0+ (ultra-low power) basic
//#define FRDM_KL03Z
//#define FRDM_KL05Z

//#define FRDM_KL25Z                                                     // L processors Cortex-M0+ (ultra-low power) with USB
//#define TWR_KL25Z48M
//#define FRDM_KL26Z
//#define TEENSY_LC                                                      // USB development board with KL26Z64
//#define FRDM_KL27Z

//#define FRDM_KL43Z                                                     // L processors Cortex-M0+ (ultra-low power) with USB and segment LCD
//#define TWR_KL43Z48M
//#define FRDM_KL46Z
//#define TWR_KL46Z48M

//#define TWR_KM34Z50M                                                   // M processors Cortex M0+ (metrology)

//#define TWR_KV10Z32                                                    // V processors Cortex M0+/M4 (motor control and power conversion)
//#define TWR_KV31F120M                                                  // (M4 - high dynamic control)

//#define TWR_KW21D256                                                   // W processors Cortex M0+/M4 (wireless connectivity)
//#define TWR_KW24D512

//#define K02F100M                                                       // development board with 100MHz K02F
#define FRDM_K20D50M                                                     // K processors Cortex M4 (performance and integration) with USB
//#define tinyK20                                                        // USB memory stick format board with SD card and 50MHz K20DX128
//#define TWR_K20D50M
//#define TWR_K20D72M
//#define TEENSY_3_1                                                     // USB development board with K20DX256
//#define K20FX512_120                                                   // development board with 120MHz K20F
//#define TWR_K21D50M
//#define TWR_K21F120M
//#define FRDM_K22F
//#define TWR_K22F120M
//#define TWR_K24F120M
//#define K26FN2_180                                                     // development board with 180MHz K26F

//#define KWIKSTIK                                                       // K processors Cortex M4 with USB and segment LCD
//#define TWR_K40X256
//#define TWR_K40D100M

//#define TWR_K53N512                                                    // K processors Cortex M4 with Ethernet, USB, integrated measurement engine and segment LCD

//#define TWR_K60N512                                                    // K processors Cortex M4 with Ethernet, USB, encryption, tamper
//#define TWR_K60D100M
//#define TWR_K60F120M
//#define K60F150M_50M                                                   // board with 150MHz K60 and 50MHz clock

//#define EMCRAFT_K61F150M                                               // K processors Cortex M4 with Ethernet, USB, encryption, tamper, key storage protection area

//#define FRDM_K64F                                                      // next generation K processors Cortex M4 with Ethernet, USB, encryption, tamper, key storage protection area
//#define TWR_K64F120M
//#define TEENSY_3_5                                                     // USB development board with K64FX512 - http://www.utasker.com/kinetis/TEENSY_3.5.html

//#define TWR_K65F180M
//#define TEENSY_3_6                                                     // USB development board with K66FX1M0 - http://www.utasker.com/kinetis/TEENSY_3.6.html

//#define TWR_K70F120M                                                   // K processors Cortex M4 with graphical LCD, Ethernet, USB, encryption, tamper
//#define K70F150M_12M                                                   // development board with 150MHz F70 and 12MHz oscillator

//#define FRDM_K82F                                                      // K processors Cortex M4 with USB, encryption, tamper (scalable and secure)
//#define TWR_POS_K81
//#define TWR_K80F120M

#if defined FRDM_KE02Z
    #define KINETIS_KE
    #define KINETIS_KE02
    #define KINETIS_MAX_SPEED    20000000
    #define TARGET_HW       "FRDM-KE02Z Kinetis"
    #define DEVICE_WITHOUT_USB                                           // KE02 doesn't have USB
    #define OUR_HEAP_SIZE   (HEAP_REQUIREMENTS)((2 * 1024) * MEM_FACTOR)
    #define DEVICE_WITHOUT_ETHERNET                                      // KE doesn't have Ethernet controller
#elif defined FRDM_KE02Z40M
    #define KINETIS_KE
    #define KINETIS_KE02
    #define KINETIS_MAX_SPEED    40000000
    #define TARGET_HW       "FRDM-KE02ZM40 Kinetis"
    #define DEVICE_WITHOUT_USB                                           // KE02 doesn't have USB
    #define OUR_HEAP_SIZE   (HEAP_REQUIREMENTS)((2 * 1024) * MEM_FACTOR)
    #define DEVICE_WITHOUT_ETHERNET                                      // KE doesn't have Ethernet controller
#elif defined FRDM_KE04Z
    #define KINETIS_KE
    #define KINETIS_KE04
    #define KINETIS_MAX_SPEED    48000000
    #define TARGET_HW       "FRDM-KE04Z Kinetis"
    #define DEVICE_WITHOUT_USB                                           // KE04 doesn't have USB
    #define OUR_HEAP_SIZE   (HEAP_REQUIREMENTS)((512) * MEM_FACTOR)      // the device has only 1k of SRAM
    #define DEVICE_WITHOUT_ETHERNET                                      // KE doesn't have Ethernet controller
#elif defined FRDM_KE06Z
    #define KINETIS_KE
    #define KINETIS_KE06
    #define KINETIS_MAX_SPEED    48000000
    #define TARGET_HW       "FRDM-KE06Z Kinetis"
    #define DEVICE_WITHOUT_USB                                           // KE04 doesn't have USB
    #define OUR_HEAP_SIZE   (HEAP_REQUIREMENTS)((2 * 1024) * MEM_FACTOR)
    #define DEVICE_WITHOUT_ETHERNET                                      // KE doesn't have Ethernet controller
#elif defined TRK_KEA64
    #define TARGET_HW            "TRK-KEA64"
    #define KINETIS_MAX_SPEED    40000000
    #define OUR_HEAP_SIZE        (HEAP_REQUIREMENTS)((2 * 1024) * MEM_FACTOR)
    #define KINETIS_KE
    #define KINETIS_KEA
    #define KINETIS_KEA64
    #define DEVICE_WITHOUT_ETHERNET                                      // KEA doesn't have Ethernet controller
    #define DEVICE_WITHOUT_USB                                           // KEA doesn't have USB
#elif defined TRK_KEA128
    #define TARGET_HW            "TRK-KEA128"
    #define KINETIS_MAX_SPEED    48000000
    #define OUR_HEAP_SIZE        (HEAP_REQUIREMENTS)((12 * 1024) * MEM_FACTOR)
    #define KINETIS_KE
    #define KINETIS_KEA
    #define KINETIS_KEA128
    #define DEVICE_WITHOUT_ETHERNET                                      // KEA doesn't have Ethernet controller
    #define DEVICE_WITHOUT_USB                                           // KEA doesn't have USB
#elif defined FRDM_KL02Z
    #define KINETIS_KL
    #define KINETIS_KL02
    #define TARGET_HW       "FRDM-KL02Z Kinetis"
    #define DEVICE_WITHOUT_USB                                           // KL02 doesn't have USB
    #define OUR_HEAP_SIZE   (HEAP_REQUIREMENTS)((2 * 1024) * MEM_FACTOR)
    #define DEVICE_WITHOUT_ETHERNET                                      // KL doesn't have Ethernet controller
#elif defined FRDM_KL03Z
    #define KINETIS_KL
    #define KINETIS_KL03
    #define TARGET_HW       "FRDM-KL03Z Kinetis"
    #define DEVICE_WITHOUT_USB                                           // KL03 doesn't have USB
    #define OUR_HEAP_SIZE   (HEAP_REQUIREMENTS)((1 * 1024) * MEM_FACTOR)
    #define DEVICE_WITHOUT_ETHERNET                                      // KL doesn't have Ethernet controller
#elif defined FRDM_KL05Z
    #define KINETIS_KL
    #define KINETIS_KL05
    #define TARGET_HW       "FRDM-KL05Z Kinetis"
    #define DEVICE_WITHOUT_USB                                           // KL05 doesn't have USB
    #define OUR_HEAP_SIZE   (HEAP_REQUIREMENTS)((2 * 1024) * MEM_FACTOR)
    #define DEVICE_WITHOUT_ETHERNET                                      // KL doesn't have Ethernet controller
#elif defined FRDM_KL25Z
    #define KINETIS_KL
    #define KINETIS_KL25
    #define TARGET_HW       "FRDM-KL25Z Kinetis"
    #define OUR_HEAP_SIZE   (HEAP_REQUIREMENTS)((8 * 1024) * MEM_FACTOR)
    #define DEVICE_WITHOUT_ETHERNET                                      // KL doesn't have Ethernet controller
  //#define KL25_TEST_BOARD                                              // test variation of KL25 board (UART 2 used)
#elif defined TWR_KL25Z48M
    #define KINETIS_KL
    #define KINETIS_KL25
    #define TARGET_HW       "TWR-KL25Z48M Kinetis"
    #define OUR_HEAP_SIZE   (HEAP_REQUIREMENTS)((8 * 1024) * MEM_FACTOR)
    #define DEVICE_WITHOUT_ETHERNET                                      // KL doesn't have Ethernet controller
#elif defined FRDM_KL26Z
    #define KINETIS_KL
    #define KINETIS_KL26
    #define TARGET_HW       "FRDM-KL26Z Kinetis"
    #define OUR_HEAP_SIZE   (HEAP_REQUIREMENTS)((8 * 1024) * MEM_FACTOR)
    #define DEVICE_WITHOUT_ETHERNET                                      // KL doesn't have Ethernet controller
#elif defined FRDM_KL27Z
    #define TARGET_HW            "FRDM-KL27Z Kinetis"
    #define OUR_HEAP_SIZE        (HEAP_REQUIREMENTS)((10 * 1024) * MEM_FACTOR)
    #define KINETIS_KL
    #define KINETIS_KL27
    #define DEVICE_WITHOUT_CAN                                           // KL doesn't have CAN controller
    #define DEVICE_WITHOUT_ETHERNET                                      // KL doesn't have Ethernet controller
#elif defined TEENSY_LC
  //#define SPECIAL_VERSION                                              // temporary special version with some specific setups
    #define KINETIS_KL
    #define KINETIS_KL26
    #define TARGET_HW       "Teensy LC"
    #define OUR_HEAP_SIZE   (HEAP_REQUIREMENTS)((4.5 * 1024) * MEM_FACTOR)
    #define DEVICE_WITHOUT_ETHERNET                                      // KL doesn't have Ethernet controller
#elif defined FRDM_KL46Z || defined FRDM_KL43Z || defined TWR_KL43Z48M
    #define KINETIS_KL
    #if defined FRDM_KL46Z
        #define TARGET_HW   "FRDM-KL46Z"
        #define KINETIS_KL46
    #elif defined FRDM_KL43Z
        #define TARGET_HW   "FRDM-KL43Z"
        #define KINETIS_KL43
    #else
        #define TARGET_HW   "TWR-KL43Z48M"
        #define KINETIS_KL43
    #endif
    #define OUR_HEAP_SIZE   (HEAP_REQUIREMENTS)((16 * 1024) * MEM_FACTOR)
    #define DEVICE_WITHOUT_ETHERNET                                      // KL doesn't have Ethernet controller
#elif defined TWR_KL46Z48M
    #define KINETIS_KL
    #define KINETIS_KL46
    #define TARGET_HW       "TWR-KL46Z48M Kinetis"
    #define OUR_HEAP_SIZE   (HEAP_REQUIREMENTS)((12 * 1024) * MEM_FACTOR)
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
#elif defined TWR_KM34Z50M
    #define KINETIS_KM
    #define KINETIS_MAX_SPEED    50000000
    #define TARGET_HW       "TWR-KM34Z50M"
    #define DEVICE_WITHOUT_ETHERNET                                      // KM doesn't have Ethernet controller
    #define DEVICE_WITHOUT_USB                                           // KM doesn't have USB
    #define OUR_HEAP_SIZE   (HEAP_REQUIREMENTS)((8 * 1024) * MEM_FACTOR)
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
#elif defined TWR_K20D50M
    #define KINETIS_K20
    #define KINETIS_MAX_SPEED    50000000
    #define TARGET_HW       "TWR-K20N50M Kinetis"
    #define DEVICE_WITHOUT_ETHERNET                                      // K20 doesn't have Ethernet controller
    #define OUR_HEAP_SIZE   (HEAP_REQUIREMENTS)((12 * 1024) * MEM_FACTOR)
#elif defined TWR_K20D72M
    #define KINETIS_K20
    #define KINETIS_MAX_SPEED    72000000
    #define KINETIS_REVISION_2
    #define TARGET_HW       "TWR-K20D72M Kinetis"
    #define DEVICE_WITHOUT_ETHERNET                                      // K20 doesn't have Ethernet controller
    #define OUR_HEAP_SIZE   (HEAP_REQUIREMENTS)((12 * 1024) * MEM_FACTOR)
#elif defined TEENSY_3_1
    #define KINETIS_K20
    #define KINETIS_REVISION_2
    #define TARGET_HW   "TEENSY 3.1 (K20DX256)"
    #define KINETIS_MAX_SPEED    72000000
    #define DEVICE_WITHOUT_ETHERNET                                      // K20 doesn't have Ethernet controller
    #define OUR_HEAP_SIZE   (HEAP_REQUIREMENTS)((12 * 1024) * MEM_FACTOR)
#elif defined FRDM_K20D50M
    #define KINETIS_K20
    #define KINETIS_MAX_SPEED    50000000
    #define TARGET_HW       "FRDM-K20D50M Kinetis"
    #define DEVICE_WITHOUT_ETHERNET                                      // K20 doesn't have Ethernet controller
    #define OUR_HEAP_SIZE   (HEAP_REQUIREMENTS)((8 * 1024) * MEM_FACTOR)
#elif defined tinyK20
    #define TARGET_HW            "tinyK20"
    #define DEVICE_WITHOUT_CAN                                           // 50MHz K20 doesn't have CAN controller
    #define KINETIS_K20                                                  // specify the sub-family
    #define KINETIS_REVISION_2
    #define KINETIS_MAX_SPEED    50000000
    #define DEVICE_WITHOUT_ETHERNET                                      // K20 doesn't have Ethernet controller
    #define OUR_HEAP_SIZE        (HEAP_REQUIREMENTS)((10 * 1024) * MEM_FACTOR)
#elif defined K20FX512_120
    #define KINETIS_K20
    #define KINETIS_K_FPU                                                // part with floating point unit
    #define KINETIS_REVISION_2
    #define KINETIS_MAX_SPEED    120000000
    #define TARGET_HW       "Kinetis K20FX512_120"
    #define DEVICE_WITHOUT_ETHERNET                                      // K20 doesn't have Ethernet controller
    #define OUR_HEAP_SIZE   (HEAP_REQUIREMENTS)((32 * 1024) * MEM_FACTOR)
#elif defined TWR_K21D50M
    #define KINETIS_K20
    #define KINETIS_K21
    #define KINETIS_REVISION_2
    #define KINETIS_MAX_SPEED    50000000
    #define TARGET_HW       "TWR-K21D50M Kinetis"
    #define DEVICE_WITHOUT_ETHERNET                                      // K20 doesn't have Ethernet controller
    #define OUR_HEAP_SIZE   (HEAP_REQUIREMENTS)((12 * 1024) * MEM_FACTOR)
#elif defined TWR_K21F120M
    #define KINETIS_K20
    #define KINETIS_K21
    #define KINETIS_K_FPU                                                // part with floating point unit
    #define KINETIS_REVISION_2
    #define KINETIS_MAX_SPEED    120000000
    #define TARGET_HW       "TWR-K21F120M Kinetis"
    #define DEVICE_WITHOUT_ETHERNET                                      // K20 doesn't have Ethernet controller
    #define OUR_HEAP_SIZE   (HEAP_REQUIREMENTS)((24 * 1024) * MEM_FACTOR)
#elif defined FRDM_K22F
    #define TARGET_HW       "FRDM-K22F"
    #define KINETIS_K_FPU                                                // part with floating point unit
    #define KINETIS_K20
    #define KINETIS_K22
    #define KINETIS_REVISION_2
    #define DEVICE_WITHOUT_ETHERNET                                      // K22 doesn't have Ethernet controller
    #define OUR_HEAP_SIZE   (HEAP_REQUIREMENTS)((62 * 1024) * MEM_FACTOR) 
#elif defined TWR_K22F120M
    #define TARGET_HW       "TWR-K22F120M"
    #define KINETIS_K_FPU                                                // part with floating point unit
    #define KINETIS_K20
    #define KINETIS_K22
    #define KINETIS_REVISION_2
    #define DEVICE_WITHOUT_ETHERNET                                      // K22 doesn't have Ethernet controller
    #define OUR_HEAP_SIZE   (HEAP_REQUIREMENTS)((24 * 1024) * MEM_FACTOR)
#elif defined TWR_K24F120M
    #define TARGET_HW            "TWR-K24F120M"
    #define KINETIS_K_FPU                                                // part with floating point unit
    #define KINETIS_MAX_SPEED    120000000
    #define KINETIS_K20                                                  // specify the sub-family
    #define KINETIS_K24                                                  // extra sub-family type precision
    #define KINETIS_REVISION_2
    #define DEVICE_WITHOUT_ETHERNET                                      // K20 doesn't have Ethernet controller
    #define OUR_HEAP_SIZE        (HEAP_REQUIREMENTS)((24 * 1024) * MEM_FACTOR)
#elif defined K26FN2_180
    #define TARGET_HW            "Kinetis K26F"
    #define OUR_HEAP_SIZE        (HEAP_REQUIREMENTS)((48 * 1024) * MEM_FACTOR) // large SRAM parts
    #define KINETIS_MAX_SPEED    180000000
    #define KINETIS_K_FPU                                                // part with floating point unit
    #define KINETIS_K20                                                  // specify the sub-family
    #define KINETIS_REVISION_2
    #define KINETIS_K26                                                  // extra sub-family type precision
    #define DEVICE_WITHOUT_ETHERNET                                      // K26 doesn't have Ethernet controller
    #define USB_HS_INTERFACE                                             // use HS interface rather than FS interface
#elif defined TWR_K40X256
    #define TARGET_HW       "TWR-K40X256 Kinetis"
    #define DEBUG_ON_VIRT_COM                                            // optionally set UART debug on virtual COM rather than the serial board
    #define KINETIS_K40
    #define DEVICE_WITHOUT_ETHERNET                                      // K40 doesn't have Ethernet controller
    #define OUR_HEAP_SIZE   (HEAP_REQUIREMENTS)((24 * 1024) * MEM_FACTOR)// we have the LAN buffers in HEAP and big RX/TX - a little more for USB
#elif defined KWIKSTIK
    #define KWIKSTIK_V3_V4                                               // when using versions 3 or 4 the SDHC cannot be used
    #define TARGET_HW       "KWIKSTIK K40"
    #define KINETIS_K40
    #define DEVICE_WITHOUT_ETHERNET                                      // K40 doesn't have Ethernet controller
    #define OUR_HEAP_SIZE   (HEAP_REQUIREMENTS)((24 * 1024) * MEM_FACTOR)// we have the LAN buffers in HEAP and big RX/TX - a little more for USB
#elif defined TWR_K40D100M
    #define DEBUG_ON_VIRT_COM                                            // optionally set UART debug on virtual COM rather than the serial board
    #define KINETIS_REVISION_2                                           // revision 2 mask (from 2N22D - see AN4445)
    #define TARGET_HW       "TWR-K40D100M Kinetis"
    #define KINETIS_K40
    #define DEVICE_WITHOUT_ETHERNET                                      // K40 doesn't have Ethernet controller
    #define OUR_HEAP_SIZE   (HEAP_REQUIREMENTS)((24 * 1024) * MEM_FACTOR)// we have the LAN buffers in HEAP and big RX/TX - a little more for USB
#elif defined TWR_K53N512
    #define TARGET_HW       "TWR-K53N512"
    #define KINETIS_K53
    #define OUR_HEAP_SIZE   (HEAP_REQUIREMENTS)((24 * 1024) * MEM_FACTOR)
#elif defined TWR_K60F120M
    #define TARGET_HW       "TWR-K60F120M"
    #define KINETIS_K_FPU                                                // part with floating point unit
    #define KINETIS_K60
    #define OUR_HEAP_SIZE   (HEAP_REQUIREMENTS)((24 * 1024) * MEM_FACTOR)
#elif defined TWR_K60D100M
    #define TARGET_HW       "TWR-K60D100M"
    #define KINETIS_K60
    #define KINETIS_REVISION_2
    #define OUR_HEAP_SIZE   (HEAP_REQUIREMENTS)((24 * 1024) * MEM_FACTOR)
#elif defined TWR_K60N512
    #define TARGET_HW       "TWR-K60N512"
    #define KINETIS_K60
    #define OUR_HEAP_SIZE   (HEAP_REQUIREMENTS)((2 * 1024) * MEM_FACTOR)
#elif defined K60F150M_50M
    #define TARGET_HW       "K60F150M_50M"
    #define KINETIS_K_FPU                                                // part with floating point unit
    #define KINETIS_K60
    #define KINETIS_MAX_SPEED    150000000
    #define KINETIS_REVISION_2
    #define OUR_HEAP_SIZE   (HEAP_REQUIREMENTS)((24 * 1024) * MEM_FACTOR)
#elif defined FRDM_K64F
    #define TARGET_HW       "FRDM-K64F"
  //#define MEMORY_SWAP                                                  // use memory swap method
    #define KINETIS_K60
    #define KINETIS_K64
    #define KINETIS_MAX_SPEED    120000000
    #define KINETIS_K_FPU                                                // part with floating point unit
    #define KINETIS_REVISION_2
    #define OUR_HEAP_SIZE   (HEAP_REQUIREMENTS)((48 * 1024) * MEM_FACTOR)
#elif defined TWR_K64F120M
  //#define TWR_SER                                                      // use TWR-SER serial board instead of OpenSDA virtual COM port
    #define TARGET_HW       "TWR-K64F120M"
    #define KINETIS_K60
    #define KINETIS_K64
    #define KINETIS_MAX_SPEED    120000000
    #define KINETIS_K_FPU                                                // part with floating point unit
    #define KINETIS_REVISION_2
    #define OUR_HEAP_SIZE   (HEAP_REQUIREMENTS)((48 * 1024) * MEM_FACTOR)
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
#elif defined TEENSY_3_6
    #define TARGET_HW            "Teensy 3.6 (K66FX1M0)"
    #define OUR_HEAP_SIZE        (HEAP_REQUIREMENTS)((48 * 1024) * MEM_FACTOR) // large SRAM parts
    #define KINETIS_MAX_SPEED    180000000
    #define KINETIS_K_FPU                                                // part with floating point unit
    #define KINETIS_K60                                                  // specify the sub-family
    #define KINETIS_REVISION_2
    #define KINETIS_K66                                                  // extra sub-family type precision
  //#define USB_HS_INTERFACE                                             // use HS interface rather than FS interface
#elif defined TWR_K70F120M
    #define TARGET_HW       "TWR-K70F120M Kinetis"
    #define KINETIS_K_FPU                                                // part with floating point unit
    #define KINETIS_K70
    #define KINETIS_MAX_SPEED    120000000
    #define USB_HS_INTERFACE                                             // use HS interface rather than FS interface (needs external ULPI transceiver)
    #define OUR_HEAP_SIZE   (HEAP_REQUIREMENTS)((24 * 1024) * MEM_FACTOR) // kinetis uses dedicated Ethernet buffers so doesn't need heap for these
#elif defined K70F150M_12M
    #define TARGET_HW       "K70F150M-12MHz crystal"
    #define KINETIS_MAX_SPEED    150000000
    #define KINETIS_K_FPU                                                // part with floating point unit
    #define KINETIS_K70
    #define OUR_HEAP_SIZE   (HEAP_REQUIREMENTS)((24 * 1024) * MEM_FACTOR) // kinetis uses dedicated Ethernet buffers so doesn't need heap for these
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



//#define NO_FLASH_SUPPORT                                               // neither parameter nor file system
/**************** Specify a file system for use by FTP, HTML and such functions *******************************/
#if !defined NO_FLASH_SUPPORT
  //#define USE_PARAMETER_BLOCK                                          // enable a parameter block for storing and retrieving non-volatile information
  //#define USE_PAR_SWAP_BLOCK                                           // we support a backup block which can be restored if desired (it is recommended to use this together with USE_PARAMETER_BLOCK

  //#define SPI_FILE_SYSTEM                                              // we have an external file system via SPI interface
  //#define FLASH_FILE_SYSTEM                                            // we have an internal file system in FLASH
  //#define NVRAM                                                        // we have an external file system in non-volatile RAM
  //#define INTERNAL_USER_FILES                                          // allow user to specify program file content
    #define FLASH_ROUTINES                                               // supply flash routines

    #if !defined SPI_FILE_SYSTEM && !defined I2C_EEPROM_FILE_SYSTEM && !defined SPI_EEPROM_FILE_SYSTEM && !defined NVRAM && !defined EXT_FLASH_FILE_SYSTEM
        #define ONLY_INTERNAL_FLASH_STORAGE
    #endif
#endif

#if defined (SPI_FILE_SYSTEM) || defined (FLASH_FILE_SYSTEM) || defined (NVRAM) || defined INTERNAL_USER_FILES
    #define ACTIVE_FILE_SYSTEM
#endif

//#define SUPPORT_MIME_IDENTIFIER                                        // if the file type is to be handled (eg. when mixing HTML with JPGs etc.) this should be set - note that the file system header will be adjusted

#if defined FLASH_FILE_SYSTEM
    #if defined SPI_FILE_SYSTEM
      //#define SPI_FLASH_SST25                                          // use SST SPI FLASH rather than ATMEL
      //#define SPI_FLASH_ST                                             // use ST FLASH rather than ATMEL
      //#define SPI_DATA_FLASH                                           // FLASH type is data FLASH supporting sub-sectors (relevant for ST types)
        #if defined SPI_FLASH_ST
            #if defined SPI_DATA_FLASH
                #define FILE_GRANULARITY (2 * SPI_FLASH_BLOCK_LENGTH)    // (2 x 4096 byte blocks) file granularity is equal to a multiple of the FLASH granularity (as defined by the device)
            #else
                #define FILE_GRANULARITY (SPI_FLASH_BLOCK_LENGTH)        // (65535 byte blocks) file granularity is equal to a multiple of the FLASH granularity (as defined by the device)
            #endif
        #elif defined SPI_FLASH_SST25
            #define FILE_GRANULARITY (SPI_FLASH_BLOCK_LENGTH)            // (4096 byte blocks) file granularity is equal to sub-sector FLASH granularity (as defined by the device)
        #else
            #define FILE_GRANULARITY (4 * SPI_FLASH_BLOCK_LENGTH)        // (4224/2112 byte blocks) file granularity is equal to a multiple of the FLASH granularity (as defined by the device)
        #endif
    #endif
#endif


#define TICK_INTERRUPT()                                                 // dummy user TICK call-back because not used

/**************** Configure driver services *******************************************************************/

#if !defined K70F150M_12M && !defined KWIKSTIK
    #define SERIAL_INTERFACE                                             // enable serial interface driver
#endif
#if defined SERIAL_INTERFACE
    #define KBOOT_LOADER                                                 // use KBOOT UART interface rather than SREC/iHex interface
  //#define DEVELOPERS_LOADER                                            // Freescale Developer's Bootloader (AN2295) compatible mode (rather than SREC/iHex)
      //#define DEVELOPERS_LOADER_PROTOCOL_VERSION_9                     // user protocol version 9 rather than obselete Kinetis 8 (not completed at the moment)
        #define DEVELOPERS_LOADER_READ                                   // support reading back program
        #define DEVELOPERS_LOADER_CRC                                    // support CRC in communication
  //#define REMOVE_SREC_LOADING                                          // disable SREC (and Intel Hex) loading but keep debug output and the command line menu
    #if !defined REMOVE_SREC_LOADING
        #define SUPPORT_INTEL_HEX_MODE                                   // support Intel Hex mode together with SREC (auto-recognition)
      //#define EXCLUSIVE_INTEL_HEX_MODE                                 // loading mode is exclusively Intel Hex (use with or without SUPPORT_INTEL_HEX_MODE)
    #endif
  //#define SERIAL_STATS                                                 // keep statistics about serial interface use
  //#define SUPPORT_MSG_MODE                                             // enable terminator recognition (MSG_MODE)
  //#define SUPPORT_MSG_CNT                                              // enable the message counter mode (MSG_MODE_RX_CNT) - requires also SUPPORT_MSG_MODE
  //#define WAKE_BLOCKED_TX                                              // allow a blocked transmitter to continue after an interrupt event
  //#define SUPPORT_FLUSH                                                // support rx flush
    #define SERIAL_SUPPORT_XON_XOFF                                      // enable XON/XOFF support in driver
    #define HIGH_WATER_MARK   20                                         // stop flow control when the input buffer has less than this space (if variable settings are required, use SUPPORT_FLOW_HIGH_LOW define)
    #define LOW_WATER_MARK    20                                         // restart when the input buffer content falls below this value
  //#define SUPPORT_FLOW_HIGH_LOW                                        // allow flow control levels to be configured (in % of buffer size)
  //#define SERIAL_SUPPORT_ECHO                                          // enable echo mode in rx driver
  //#define SERIAL_SUPPORT_ESCAPE                                        // enable escape sequencing in driver
  //#define SERIAL_SUPPORT_SCAN                                          // serial receiver supports scanning of input buffer for a sequence
  //#define SUPPORT_HW_FLOW                                              // support RTS/CTS flow control and other possible modem signals

    #define LOG_UART0                                                    // activate this option to log all data sent to UART 0 to a file called "UART0.txt"
    #define LOG_UART1                                                    // activate this option to log all data sent to UART 1 to a file called "UART1.txt"
    #define LOG_UART2                                                    // activate this option to log all data sent to UART 2 to a file called "UART2.txt"
    #define LOG_UART3                                                    // activate this option to log all data sent to UART 3 to a file called "UART3.txt"
#else
    #define NUMBER_SERIAL              0                                 // no physical queue needed
#endif
#define NUMBER_EXTERNAL_SERIAL         0                                 // no external UARTs

#if defined DEVICE_WITHOUT_USB
    #define NUMBER_USB     0                                             // no physical queue needed
#else
    #define USB_INTERFACE                                                // enable USB driver interface
    #if defined USB_INTERFACE
      //#define USE_USB_CDC                                              // allow SREC/iHex loading via virtual COM
        #define USB_MSD_DEVICE_LOADER                                    // USB-MSD device mode (the board appears as a hardware to the host)
      //#define USB_MSD_HOST_LOADER                                      // USB-MSD host mode (the board operates as host and can read new code from a memory stick)
        #if defined USE_USB_CDC
            #undef SERIAL_INTERFACE                                      // remove the UART interface
            #define NUMBER_SERIAL          0
        #endif
        #if defined USB_MSD_DEVICE_LOADER
            #define USB_DEVICE_SUPPORT                                   // requires USB device driver support
          //#define USB_MSD_TIMEOUT                                      // if there is no enumeration within a short time the application will be started
          //#define FAT_EMULATION                                        // use fat emulation
            #if defined FAT_EMULATION
                #define EMULATED_FAT_LUMS  2
                #define MAXIMUM_DATA_FILES 1
                #define ROOT_DIR_SECTORS   2
                #define EMULATED_FAT_DISK_SIZE (64 * 1024 * 1024)
                #define EMULATED_FAT_FILE_NAME_CONTROL
                #define FAT_EMULATION_LFN
                #define UTFAT_LFN_WRITE_PATCH
                #define RANDOM_NUMBER_GENERATOR
                #define MAX_FIRMWARE_NAME  64                            // longest firmware file name string buffer
                #define EMULATED_FAT_FILE_DATE_CONTROL
            #endif
        #endif
        #if defined USB_MSD_HOST_LOADER                                  // support loading from memory stick
            #define USB_MSD_HOST                                         // requires USB-MSD support in the mass-storage module
            #define USB_HOST_SUPPORT                                     // requires USB host driver support
            #define SUPPORT_USB_SIMPLEX_HOST_ENDPOINTS                   // allow operation with memory sticks using bulk IN/OUT on the same endpoint
            #define RANDOM_NUMBER_GENERATOR                              // random numbers required for USB-MSD host tags
            #define DELETE_SDCARD_FILE_AFTER_UPDATE
            #if defined DELETE_SDCARD_FILE_AFTER_UPDATE
                #define UTFAT_WRITE
            #endif
        #endif
        #if defined USB_MSD_DEVICE_LOADER || defined USB_MSD_HOST_LOADER
            #define SUPPORT_FLUSH                                        // allow flush command to be used (important for mass storage class)
        #endif
      //#define USE_USB_MSD                                              // full USB-MSD to SD card interface on USB (no emulated loader function) - requires SDCARD_SUPPORT (USB_MSD_DEVICE_LOADER can be disabled)
            #define DISK_COUNT         1                                 // single upload disk (set to 2 for two upload disks)
          //#define DEBUG_MAC                                            // activate debug output used to monitor the operation of MAC OS X
        #define HID_LOADER                                               // Freescale HIDloader.exe or KBOOT compatible
            #define KBOOT_HID_LOADER                                     // select KBOOT mode of operation (rather than HIDloader.exe)
        #undef _NO_CHECK_QUEUE_INPUT
        #define WAKE_BLOCKED_USB_TX                                      // allow a blocked USB transmitter to continue after an interrupt event
        #define NUMBER_USB     (5 + 1)                                   // 6 physical queues (control plus 5 endpoints) needed for USB interface
        #define LOG_USB_TX                                               // log USB transmissions in simulator
        #define NUMBER_OF_POSSIBLE_CONFIGURATIONS  1                     // one USB configuration
        #if defined USB_HS_INTERFACE
            #define ENDPOINT_0_SIZE                64
            #define USBHS_RX_QUEUE_SIZE            (512 * 8)             // input queue capable of holding 4k of data to be saved
            #define USB_SIMPLEX_ENDPOINTS                                // share IN and OUT on single endpoint
        #else
            #if defined HID_LOADER
                #define ENDPOINT_0_SIZE            64
            #else
                #define ENDPOINT_0_SIZE            8                     // maximum packet size for endpoint 0. Low speed devices must use 8 whereas full speed devices can chose to use 8, 16, 32 or 64
            #endif
        #endif
        #define USB_STRING_OPTION                                        // support optional string descriptors
            #define USB_MAX_STRINGS                3                     // the maximum number of strings supported by host
            #define USB_MAX_STRING_LENGTH          16                    // the maximum length of each string supported by host (unicode characters)
      //#define USB_RUN_TIME_DEFINABLE_STRINGS                           // enable USB string content to be defined at run time (variable)
      //#define USE_USB_OTG_CHARGE_PUMP                                  // enable charge pump control in the driver
        #if defined USE_USB_OTG_CHARGE_PUMP
            #define IIC_INTERFACE                                        // activate IIC interface since it will be needed
        #endif
        #define USB_POWER_CONSUMPTION_MA 100                             // USB powered with maximum 100mA
    #else
        #define NUMBER_USB     0                                         // no physical queue needed
    #endif
#endif

#if !defined TWR_K20D50M && !defined FRDM_K20D50M && !defined FRDM_KL46Z && !defined FRDM_KL43Z && !defined TWR_KL46Z48M && !defined FRDM_KL26Z && !defined FRDM_KL27Z && !defined TWR_KL25Z48M && !defined FRDM_KL02Z && !defined FRDM_KL03Z && !defined FRDM_KL05Z && !defined FRDM_KE02Z && !defined FRDM_KE02Z40M && !defined FRDM_KE04Z && !defined TWR_K20D72M && !defined TWR_K21D50M && !defined TWR_K22F120M && !defined TWR_K24F120M && !defined FRDM_K22F && !defined TWR_KV10Z32 && !defined TWR_KV31F120M // boards have no SD card socket
  //#define SDCARD_SUPPORT                                               // SD-card interface (only choose one of these options at a time)
  //#define SPI_FLASH_FAT                                                // SPI flash
        #define SIMPLE_FLASH                                             // don't perform block management and wear-leveling
        #define FLASH_FAT_MANAGEMENT_ADDRESS     (SIZE_OF_FLASH)
  //#define DELETE_SDCARD_FILE_AFTER_UPDATE
    #if defined DELETE_SDCARD_FILE_AFTER_UPDATE || defined USE_USB_MSD
        #define UTFAT_WRITE
    #endif
    #if defined USE_USB_MSD
        #define SUPPORT_FLUSH
    #endif
    #if defined SDCARD_SUPPORT
        #define UREVERSEMEMCPY                                           // required when SD card used in SPI mode
    #endif
#endif
#if defined SDCARD_SUPPORT || defined SPI_FLASH_FAT || defined USB_MSD_HOST_LOADER
    #if defined SPI_FLASH_FAT
        #undef ONLY_INTERNAL_FLASH_STORAGE                               // allow multiple flash storage support
    #endif
    #define SD_CARD_RETRY_INTERVAL       0.4                             // attempt SD card initialisation at 0.4s intervals
    #define UT_DIRECTORIES_AVAILABLE     1                               // this many directories objects are available for allocation
  //#define UTMANAGED_FILE_COUNT        10                               // allow this many managed files at one time
    #define UTFAT_LFN_READ                                               // enable long file name read support
    #define STR_EQUIV_ON                                                 // ensure that this routine is available
    #if defined UTFAT_LFN_READ
        #define MAX_UTFAT_FILE_NAME     (100)                            // the maximum file name length supported
    #endif
  //#define UTFAT_WRITE                                                  // enable write functions
    #if defined UTFAT_WRITE
        #define UTFAT_LFN_WRITE
      //#define UTFAT_LFN_WRITE_PATCH
      //#define UTFAT_FORMATTING                                         // enable formatting SD cards (requires also write)
      //#define UTFAT_FULL_FORMATTING                                    // enable formatting SD cards including zeroing of data sectors as well as FAT sectors
      //#define SUPPORT_FILE_TIME_STAMP                                  // when activated fnGetLocalFileTime() must exist, which return the date and time information
    #endif
    #define UTFAT16                                                      // support FAT16 as well as FAT32
  //#define UTFAT12                                                      // support FAT12 as well as FAT32
    #define UTFAT_RETURN_FILE_CREATION_TIME                              // when a file is opened, its creation time and date is returned in the file object
  //#define UTFAT_DISABLE_DEBUG_OUT                                      // disable all debug output messages from the utFAT module
#endif

#if !defined DEVICE_WITHOUT_ETHERNET
  //#define ETH_INTERFACE                                                // enable Ethernet interface driver
    #if defined FRDM_K64F
      //#define ENC424J600_INTERFACE                                     // 10/100 Ethernet connected via SPI (also ENC624J600 in larger package with more parallel modes)
      //#define USE_SIMULATION_INTERFACE                                 // work with a remote simulation interface (remove when not present)
      //#define REMOTE_SIMULATION_INTERFACE                              // FRDM-K64 being used in remote simulation mode as HW interface to the simulator
      //#define NO_INTERNAL_ETHERNET                                     // disable Ethernet on the internal controller
    #endif
#elif defined TEENSY_3_1
  //#define ETH_INTERFACE                                                // enable Ethernet interface driver
  //#define ENC424J600_INTERFACE                                         // 10/100 Ethernet connected via SPI (also ENC624J600 in larger package with more parallel modes)
#endif
#if defined ETH_INTERFACE
    #define MAC_DELIMITER  '-'                                           // used for display and entry of mac addresses
    #define IPV6_DELIMITER ':'                                           // used for display and entry of IPV6 addresses
    #define NUMBER_LAN     1                                             // one physical interface needed for LAN
    #if defined ENC424J600_INTERFACE
        #define IP_INTERFACE_WITHOUT_CS_OFFLOADING                       // enable software based checksum offloading so that it can be used by this interface
    #endif

    #define ETHERNET_RELEASE_AFTER_EVERY_FRAME                           // handle only one Ethernet reception frame at a time and allow other tasks to be scheduled in between
        #define ETHERNET_RELEASE_LIMIT  3                                // allow a maximum of three reception frames to be handled
/**************** Configure TCP/IP services ******************************************************************/
  //#define USE_IPV6                                                     // enable IPV6
        #define USE_IPV6INV4                                             // support tunnelling IPv6 ind IPv4
        #define USE_IPV6INV4_RELAY_DESTINATIONS 2                        // enable relaying to other nodes in the network - the number of destination in the IPv6 in IPv4 relay table
        #define MAX_HW_ADDRESS_LENGTH  MAC_LENGTH                        // set a variable maximum hardware address length - default is Ethernet MAC-48, 6 bytes
        #define NEIGHBOR_TABLE_ENTRIES 4                                 // the maximum entries in IPV6 neighbor table
    #define USE_IP                                                       // enable IP and ARP and all other possible tcp/ip services
    #if defined USE_IP
      //#define USE_IP_STATS                                             // enable IP statistics (counters)
        #define ARP_TABLE_ENTRIES      4                                 // the maximum entries in ARP table
        #define ARP_IGNORE_FOREIGN_ENTRIES                               // only add used addresses to ARP table
        #define USE_ICMP                                                 // enable ICMP
        #if defined _WINDOWS
          //#define PSEUDO_LOOPBACK                                      // pseudo loop back when simulating - only for use with the simulator!! (this allows an application to send test frames to its own IP address)
        #endif
      //#define USE_ZERO_CONFIG                                          // support IPv4 link-local and zero configuratio (autoIP)
        #if defined USE_ICMP                                             // specify ICMP support details
            #define ICMP_PING                                            // allow PING reply
          //#define ICMP_SEND_PING                                       // support PING transmission
          //#define ICMP_DEST_UNREACHABLE                                // allow destination unreachable if no UDP port exists
        #endif
    #endif
    #if defined USE_IP || defined USE_IPV6
      //#define USE_UDP                                                  // enable UDP over IP - needs IP
        #define USE_TCP                                                  // enable TCP over IP - needs IP

        #if defined USE_TCP                                              // specify TCP support details
            #define REUSE_TIME_WAIT_SOCKETS                              // allow sockets in 2MSL wait to be reallocated to different IP/port pairs
            #define TCP_CHECK_SYS_REPS                                   // reuse a socket whose SYN + ACK has been lost (avoids it repeating its SYN + ACK and unnecessarily blocking in a socket until it times out)
            #define RST_ON_NO_TCP_PORT                                   // SYN to non-existing ports receive a RST
            #define ANNOUNCE_MAX_SEGMENT_SIZE                            // we announce the maximum segment size we can handle when establishing a connection
            #define SUPPORT_PEER_MSS                                     // respect peer's MSS when sending TCP frames
            #define SUPPORT_PEER_WINDOW                                  // respect peer's Open Window when sending TCP frames
            #define WINDOWING_BUFFERS      4                             // we can send 4 frames before requiring ACKs
            #define CONTROL_WINDOW_SIZE                                  // support variable windows size to quench reception
            #define USE_OUT_OF_ORDER_TCP_RX_BUFFER                       // enable a single TCP frame buffer to handle out-of-order receptions (useful when receiving large data streams in case the server starts sending out-of-order frames after a frame loss)
          //#define USE_FTP                                              // enable FTP - needs TCP
          //#define USE_FTP_CLIENT                                       // enable FTP client - needs TCP
          //#define USE_SMTP                                             // enable SMTP  - needs TCP
          //#define USE_POP3                                             // enable POP3 Email - needs TCP
            #define USE_HTTP                                             // support embedded Web server - needs TCP
          //#define USE_TELNET                                           // enable TELNET support
          //#define USE_TIME_SERVER                                      // enable time server support - presently demo started in application

          //#define TEST_CLIENT_SERVER                                   // TCP client/server test (see debug.c)
            #if defined TEST_CLIENT_SERVER
                #define USER_TCP_SOCKETS      1
            #else
                #define USER_TCP_SOCKETS      0                          // we use no non-standard TCP sockets
            #endif
        #endif

        #if defined USE_UDP
            #define USER_UDP_SOCKETS       1                             // we reserve  one non-standard UDP socket in case the UDP demo is required

          //#define USE_BOOTP                                            // enable BOOTP - needs UDP - IPCONFIG default zero.
            #define USE_DHCP_CLIENT                                      // enable DHCP  - needs UDP - IPCONFIG default zero. Needs 1k Ethernet RX Buffers!! (set random number also)
          //#define USE_DNS                                              // enable DNS   - needs UDP
          //    #define DNS_SERVER_OWN_ADDRESS                           // command line menu allows DNS server address to be set, otherwise it uses the default gateway
          //#define USE_TFTP                                             // enable TFTP  - needs UDP

          //#define USE_NETBIOS                                          // enable NetBIOS - needs UDP
          //#define USE_SNMP
          //#define USE_SNTP                                             // simple network time protocol

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
          //#define ANONYMOUS_LOGIN                                      // allow anonymous FTP without user/password
          //#define FILE_NAMES_PER_FTP_FRAME    6                        // limit size of buffers required to display files to this many names (remove to use maximum possible)
            #define FTP_SOCKETS 2                                        // reserve 2 TCP sockets for command and data
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
                #define MAX_FILE_NAME_LENGTH    20                       // user files maximum file name length
            #endif
            #if defined SDCARD_SUPPORT || defined SPI_FLASH_FAT
                #define FTP_UTFAT                                        // allow FTP to work with utFAT
                #define FTP_ROOT                "/"                      // the root directory as seen by the FTP server (can be set to a sub-directory of the main disk to restrict FTP accesses to there)
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
            #define MODBUS_TCP_GATEWAY                                   // support MODBUS TCP <-> MODBUS serial gateway
            #if defined USE_MODBUS_MASTER                                // following option srequire master support
                #define TCP_SLAVE_ROUTER                                 // TCP slave router support
                #define MODBUS_TCP_MASTERS    2                          // support MODBUS TCP masters
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
            #define SMTP_PARAMETERS
            #define USE_SMTP_AUTHENTICATION
        #else
            #define SMTP_SOCKET 0                                        // no TCP socket needed
        #endif

        #if defined USE_TELNET
            #define USE_BUFFERED_TCP                                     // always use buffered TCP for telnet
            #define NO_OF_TELNET_SESSIONS   1                            // reserve sockets TELNET sessions
        #else
            #define NO_OF_TELNET_SESSIONS   0                            // no sockets reserved for Telnet
        #endif

        #if defined USE_BUFFERED_TCP                                     // if using a buffer for TCP to allow interractive data applications (like TELNET)
            #define WAKE_BLOCKED_TCP_BUF                                 // support waking blocked TCP buffer (with USE_BUFFERED_TCP)
            #define INDIVIDUAL_BUFFERED_TCP_BUFFER_SIZE                  // allow each buffered TCP user to define their own TCP buffer size
        #endif

        #if defined USE_HTTP                                             // specify HTTP support details
            #define _VARIABLE_HTTP_PORT                                  // allow server to be started with user-defined port number
            #define HTTP_WINDOWING_BUFFERS     2                         // we can send 2 frames before requiring ACKs
            #define HTTP_IGNORE_PARTIAL_ACKS                             // when transmission error rates are low, ignore partial ack handling in favour of retransmission on timeout

            #define WEB_PARAMETER_GENERATION                             // support of parameter generating (eg. manipulating select and adding values)
            #define WEB_PARAMETER_HANDLING                               // support  handling of received web parameters
            #define WEB_PARSER_START          '£'                        // this symbol is used in Web pages to instruct parsing to begin
            #define WEB_INSERT_STRING         'v'
            #define WEB_DISABLE_FIELD         'D'
            #define WEB_NOT_DISABLE_FIELD     'd'
            #define WEB_SELECTED_FIELD        's'
            #define WEB_INSERT_DYNAMIC        'H'
          //#define WEB_ESCAPE_LEN             5                         // defaults to 4 if not defined
            #define FILE404 (uFILE_SYSTEM_START + (FILE_GRANULARITY * (LAST_FILE_BLOCK - 1)))// last block fixed for 404 error page
            #if !defined SPI_FILE_SYSTEM || defined FLASH_FILE_SYSTEM    // SPI file system requires 404 file also to be in file
                #define FILE404_IN_PROG                                  // fixed FILE404 in Code (no NE64 support since it pages the file system in memory)
            #endif

            #define FILE_404_CONTENT        "<html><head><title>£vN0 Loader</title></head><body bgcolor=#d0d000 text=#000000 topmargin=3 marginheight=3><center><td valign=top class=h> \
<font color=#ff0000 style=font-size:30px><b style='mso-bidi-font-weight:normal'>£vN0</font> - Loader (£vV0)</b></td><br><td align=left><br><br> \
<form action=webpage.html name=e1><input type=submit value=""Erase-Application"" name=e>Enter Password <input maxLength=17 size=17 name=c1 value=""£ve0""> £vV1</form> \
<form action=0S.bin enctype=""multipart/form-data"" method=""post""><p><input type=""file"" name=""datafile"" size=""30""><input type=""submit"" value=""Upload"" £ds0></p></form> \
<br><form action=webpage.html name=e0><input type=submit value=""Mass-Erase"" name=e>Enter Password <input maxLength=17 size=17 name=c0 value=""£ve0""></form> \
<br></font></td></body></html>";

            #define _FIXED_WEB_PAGE_404                                  // force 404 error on every file served
            #define SUPPORT_INTERNAL_HTML_FILES

            #define SUPPORT_CHROME                                       // always answer with HTTP header so that Chrome accepts date

            #define SUPPORT_HTTP_POST                                    // support file uploads using POST command
            #define RESTRICT_POST_CONTENT_TYPE                           // filter allowed posting formats
            #if defined RESTRICT_POST_CONTENT_TYPE
                #define SUPPORT_POST_BINARY
              //#define SUPPORT_POST_TEXT
              //#define SUPPORT_POST_GIF
              //#define SUPPORT_POST_BMP
              //#define SUPPORT_POST_PDF
            #endif
          //#define PLAIN_TEXT_POST                                      // allow posting parameters
          //#define SUPPORT_HTTP_CONTINUE                                // respond to Expect: 100-continue to start a posting sequence without any delay (useful when working with cURL)
          //#define X_WWW_FORM_URLENCODED                                // this encoding is also understood as plain text post (used in addition to PLAIN_TEXT_POST)
          //#define AUTO_ASCII_PLAIN_TEXT_DECODE                         // automatically decode any ASCII characters send as "%xx" - this require '%' to be avoided in post content! (Suggested for use with Chrome)
            #define SUPPORT_HTTP_POST_TO_APPLICATION                     // allow application to handle posted data rather than always saving to file
          //#define SUPPORT_DELAY_WEB_SERVING                            // enable delayed web page serving defined by the application

            #define NO_OF_HTTP_SESSIONS        4                         // this many parallel HTTP sockets are reserved for this many parallel sessions
            #define NO_OF_HTTPS_SESSIONS       0
          //#define HTTP_AUTHENTICATION                                  // activate basic authentication
          //#define PROJECT_CREDENTIALS  "HTTP/1.0 401\r\nWWW-Authenticate: Basic realm=""uTasker""\r\n\r\n" // put project name here to spice up credential pop-up
                                                                         //               ^^^^^^^
          //#define PROJECT_CREDENTIALS  "HTTP/1.0 401\r\nWWW-Authenticate: Digest algorithm=MD5 realm=""uTasker"" nonce=""61ff892f""\r\n\r\n" // put project name here to spice up credential pop-up
          //#define HTTP_DYNAMIC_CONTENT                                 // activate support of dynamic content generation
            #define DYNAMIC_DATA_FIXED_LENGTH  0
            #if defined HTTP_DYNAMIC_CONTENT
                #define HTTP_USER_DETAILS                                // enable the application to manage details belonging to a particalar connection
            #endif
          //#define HTTP_HEADER_CONTENT_INFO                             // add plain text HTTP header information - ensures that Internet Explorer correctly displays txt files
            #if defined SDCARD_SUPPORT || defined SPI_FLASH_FAT
              //#define HTTP_UTFAT                                       // allow HTTP to work with utFAT
              //#define HTTP_ROOT              "dir1"                    // the root directory as seen by the HTTP server (can be set to a sub-directory of the main disk to restrict HTTP accesses to there)
              //#define DEFAULT_HTTP_FILE      "index.htm"               // the file served when first contact is made with the web server
            #endif
          //#define HTTP_SET_USERFILE_START_PAGE                         // allow a specific user file to act as HTTP default start side
                #define HTTP_START_SIDE        "index.html"              // start side used with user files
          //#define HTTP_SERVE_ONLY_USER_FILES                           // only serve user file - if a defined user file is not found display 404 error rather than a uFileSystem equivalent
        #else
            #define NO_OF_HTTP_SESSIONS        0                         // no TCP sockets are reserved for HTML
        #endif

        #define USER_NAME_AND_PASS                                       // routines for user name and password support

        #define NO_OF_TCPSOCKETS (NO_OF_HTTP_SESSIONS + FTP_SOCKETS + FTP_CLIENT_SOCKETS + POP3_SOCKET + SMTP_SOCKET + NO_OF_TELNET_SESSIONS + TIME_SERVER_SOCKET + MODBUS_TCP_SOCKETS + USER_TCP_SOCKETS)   // reserve the number of TCP sockets necessary for our configuration
    #endif
#else
    #define NUMBER_LAN     0
    #define _NO_FILE_INTERFACE                                           // code size optimisation
    #if !defined DEBUG_MAC
        #define _REMOVE_FORMATTED_OUTPUT
    #endif
#endif

#define PHYSICAL_QUEUES   (NUMBER_LAN + NUMBER_SERIAL + NUMBER_USB)      // the number of physical queues in the system



// Project includes are set here for all files in the correct order
//
#include "types.h"                                                       // project specific type settings
#if defined USB_INTERFACE && defined _COMPILE_COSMIC
    #include "../../uTasker/usb.h"
#endif
#include "../../uTasker/driver.h"                                        // driver and general formatting routines
#if defined SDCARD_SUPPORT || defined SPI_FLASH_FAT || defined USB_MSD_HOST_LOADER || defined FAT_EMULATION
    #include "../../uTasker/utFAT/mass_storage.h"
#endif
#include "../../stack/tcpip.h"                                           // TCP/IP stack and web utilities
#include "../../Hardware/hardware.h"                                     // general hardware
#include "../../uTasker/uTasker.h"                                       // operating system defines
#include "TaskConfig.h"                                                  // the specific task configuration
#include "Loader.h"                                                      // general project specific include
#if defined _WINDOWS
  #include "../../WinSim/WinSim.h"
#endif



#if defined OPSYS_CONFIG                                                 // this is only set in the hardware module
    #if defined ETH_INTERFACE                                            // if we support Ethernet we define some constants for its (TCP/IP) use
        const unsigned char cucNullMACIP[MAC_LENGTH] = { 0, 0, 0, 0, 0, 0 };
        const unsigned char cucBroadcast[MAC_LENGTH] = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff }; // used also for broadcast IP
    #endif
#endif


#endif
#endif
