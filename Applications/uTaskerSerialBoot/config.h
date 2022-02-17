/*********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher

    ---------------------------------------------------------------------
    File:      config.h
    Project:   Serial Loader (SREC/iHex serial, USB-MSD, memory stick, SD card, ethernet, Modbus, I2C slave)
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2019
    *********************************************************************
    02.02.2017 Adapt for us tick resolution (_TICK_RESOLUTION)
    05.10.2017 Add modbus configuration
    17.01.2018 Add I2C slave configuration
    09.06.2018 Added STM32 targets

    See this video for details of building the serial loader with KDS: https://youtu.be/bilc_4Cr7eo
    See this video for details of building and using the serial loader's Ethernet loading method: https://youtu.be/g71PGlQy6eI
    See this video for I2C slave loading: https://youtu.be/awREsqeCEzQ
*/

#if !defined __CONFIG__
    #define __CONFIG__
                  

#if !defined _ASSEMBLER_CONFIG                                           // remove all following when used for assembler configuration

#if defined _WINDOWS
    #define MEM_FACTOR 1.0                                               // Windows tends to use more memory so expand heap slightly
#else
    #define MEM_FACTOR 1.0
#endif

#define _BOOT_LOADER                                                     // set for compatibility with applications that require adequate uninitialised variables at the top of RAM

#define _NO_CHECK_QUEUE_INPUT                                            // code size optimisations
#define _MINIMUM_IRQ_INITIALISATION

#define _TICK_RESOLUTION     TICK_UNIT_MS(50)                            // 50 ms system time period - max possible at 50MHz SYSTICK would be about 335ms !

// Major hardware dependent settings for this project (choice of board - select only one at a time)
//
// Kinetis
//
#if defined _KINETIS                                                     // NXP Kinetis targets
    //#define TRK_KEA64                                                  // EA ultra-reliable automotive processors Cortex-M0+ - starterTRAK board http://www.utasker.com/kinetis/TRK-KEA64.html
    //#define TRK_KEA128                                                 // starterTRAK board http://www.utasker.com/kinetis/TRK-KEA128.html
    //#define FRDM_KEAZN32Q64                                            // freedom board http://www.utasker.com/kinetis/FRDM-KEAZN32Q64.html
    //#define FRDM_KEAZ64Q64                                             // freedom board http://www.utasker.com/kinetis/FRDM-KEAZ64Q64.html
    //#define FRDM_KEAZ128Q80                                            // freedom board http://www.utasker.com/kinetis/FRDM-KEAZ128Q80.html

    //#define FRDM_KE02Z                                                 // E processors Cortex-M0+ / M4 (5V robust) - freedom board http://www.utasker.com/kinetis/FRDM-KE02Z.html
    //#define FRDM_KE02Z40M                                              // freedom board http://www.utasker.com/kinetis/FRDM-KE02Z40M.html
    //#define FRDM_KE04Z                                                 // freedom board http://www.utasker.com/kinetis/FRDM-KE04Z.html
    //#define FRDM_KE06Z                                                 // freedom board http://www.utasker.com/kinetis/FRDM-KE06Z.html
    //#define FRDM_KE15Z                                                 // freedom board http://www.utasker.com/kinetis/FRDM-KE15Z.html

    //#define FRDM_KL02Z                                                 // L processors Cortex-M0+ (ultra-low power) basic - freedom board http://www.utasker.com/kinetis/FRDM-KL02Z.html
    //#define FRDM_KL03Z                                                 // freedom board http://www.utasker.com/kinetis/FRDM-KL03Z.html
    //#define FRDM_KL05Z                                                 // freedom board http://www.utasker.com/kinetis/FRDM-KL05Z.html

    //#define FRDM_KL25Z                                                 // L processors Cortex-M0+ (ultra-low power) with USB - freedom board http://www.utasker.com/kinetis/FRDM-KL25Z.html
    //#define TWR_KL25Z48M                                               // tower board http://www.utasker.com/kinetis/TWR-KL25Z48M.html
    //#define FRDM_KL26Z                                                 // freedom board http://www.utasker.com/kinetis/FRDM-KL26Z.html
    //#define TEENSY_LC                                                  // USB development board with KL26Z64 - http://www.utasker.com/kinetis/TEENSY_LC.html
    //#define FRDM_KL27Z                                                 // freedom board http://www.utasker.com/kinetis/FRDM-KL27Z.html
    //#define FRDM_KL28Z                                                 // freedom board http://www.utasker.com/kinetis/FRDM-KL28Z.html
    //#define FRDM_KL43Z                                                 // L processors Cortex-M0+ (ultra-low power) with USB and segment LCD - freedom board http://www.utasker.com/kinetis/FRDM-KL43Z.html
    //#define TWR_KL43Z48M                                               // tower board http://www.utasker.com/kinetis/TWR-KL43Z48M.html
    //#define FRDM_KL46Z                                                 // freedom board http://www.utasker.com/kinetis/FRDM-KL46Z.html
    //#define TWR_KL46Z48M                                               // tower board http://www.utasker.com/kinetis/TWR-KL46Z48M.html

    //#define FRDM_KL82Z                                                 // freedom board http://www.utasker.com/kinetis/FRDM-KL82Z.html

    //#define TWR_KM34Z50M                                               // M processors Cortex M0+ (metrology) - tower board http://www.utasker.com/kinetis/TWR-KM34Z50M.html
    //#define TWR_KM34Z75M                                               // tower board http://www.utasker.com/kinetis/TWR-KM34Z75M.html

    //#define TWR_KV10Z32                                                // V processors Cortex M0+/M4 (M0+ - motor control and power conversion - low dynamic control) - tower board http://www.utasker.com/kinetis/TWR-KV10Z32.html
    //#define FRDM_KV31F                                                 // (M4 - high dynamic control) - freedom board http://www.utasker.com/kinetis/FRDM-KV31F.html
    //#define TWR_KV31F120M                                              // (M4 - high dynamic control) - tower board http://www.utasker.com/kinetis/TWR-KV31F120M.html
    //#define TWR_KV58F220M                                              // (M7 - motor control and power coversion, Ethernet)

    //#define TWR_KW21D256                                               // W processors Cortex M0+/M4 (wireless connectivity) - tower board http://www.utasker.com/kinetis/TWR-KW21D256.html
    //#define TWR_KW24D512                                               // tower board http://www.utasker.com/kinetis/TWR-KW24D512.html
    //#define HEXIWEAR_KW40Z                                             // hexiwear - wearable development kit for IoT (KW40Z160 support wireless processor) http://www.hexiwear.com/

    //#define K02F100M                                                   // development board with 100MHz K02F
    //#define K12D50M                                                    // development board with 50MHz K12
    //#define FRDM_K20D50M                                               // K processors Cortex M4 (performance and integration) with USB - freedom board http://www.utasker.com/kinetis/FRDM-K20D50M.html
    //#define tinyK20                                                    // USB memory stick format board with SD card and 50MHz K20DX128 http://www.utasker.com/kinetis/tinyK20.html
    //#define TWR_K20D50M                                                // tower board http://www.utasker.com/kinetis/TWR-K20D50M.html
    //#define TWR_K20D72M                                                // tower board http://www.utasker.com/kinetis/TWR-K20D72M.html
    //#define TEENSY_3_1                                                 // USB development board with K20DX256 http://www.utasker.com/kinetis/TEENSY_3.1.html
    //#define K20FX512_120                                               // development board with 120MHz K20F
    //#define TWR_K21D50M                                                // tower board http://www.utasker.com/kinetis/TWR-K21D50M.html
    //#define TWR_K21F120M                                               // tower board http://www.utasker.com/kinetis/TWR-K21F120M.html
    //#define tinyK22                                                    // USB memory stick format board with SD card and 120MHz K22FN512 http://www.utasker.com/kinetis/tinyK22.html
    //#define FRDM_K22F                                                  // freedom board http://www.utasker.com/kinetis/FRDM-K22F.html
    //#define TWR_K22F120M                                               // tower board http://www.utasker.com/kinetis/TWR-K22F120M.html
    //#define BLAZE_K22                                                  // K22FN1M0 with 1.6" color display and touch http://www.utasker.com/kinetis/BLAZE_K22.html
    //#define TWR_K24F120M                                               // tower board http://www.utasker.com/kinetis/TWR-K24F120M.html
    //#define K24FN1M0_120
    //#define K26FN2_180                                                 // development board with 180MHz K26F
    //#define FRDM_K28F                                                  // freedom board http://www.utasker.com/kinetis/FRDM-K28F.html

    //#define KWIKSTIK                                                   // K processors Cortex M4 with USB and segment LCD http://www.utasker.com/kinetis/KWIKSTIK.html
    //#define TWR_K40X256                                                // tower board http://www.utasker.com/kinetis/TWR-K40X256.html
    //#define TWR_K40D100M                                               // tower board http://www.utasker.com/kinetis/TWR-K40D100M.html

    //#define TWR_K53N512                                                // K processors Cortex M4 with Ethernet, USB, integrated measurement engine and segment LCD - tower board http://www.utasker.com/kinetis/TWR-K53N512.html

    //#define TWR_K60N512                                                // K processors Cortex M4 with Ethernet, USB, encryption, tamper - tower board http://www.utasker.com/kinetis/TWR-K60N512.html
    //#define TWR_K60D100M                                               // tower board http://www.utasker.com/kinetis/TWR-K60D100M.html
    //#define TWR_K60F120M                                               // tower board http://www.utasker.com/kinetis/TWR-K60F120M.html
    //#define K60F150M_50M                                               // board with 150MHz K60 and 50MHz clock

    //#define EMCRAFT_K61F150M                                           // K processors Cortex M4 with Ethernet, USB, encryption, tamper, key storage protection area - http://www.utasker.com/kinetis/EMCRAFT_K61F150M.html

      #define FRDM_K64F                                                  // next generation K processors Cortex M4 with Ethernet, USB, encryption, tamper, key storage protection area - freedom board http://www.utasker.com/kinetis/FRDM-K64F.html
    //#define TWR_K64F120M                                               // tower board http://www.utasker.com/kinetis/TWR-K64F120M.html
    //#define HEXIWEAR_K64F                                              // hexiwear - wearable development kit for IoT (K64FN1M0VDC12 main processor) http://www.hexiwear.com/
    //#define TEENSY_3_5                                                 // USB development board with K64FX512 - http://www.utasker.com/kinetis/TEENSY_3.5.html
    //#define TWR_K65F180M                                               // tower board http://www.utasker.com/kinetis/TWR-K65F180M.html
    //#define K66FX1M0                                                   // development board with K66FX1M0
    //#define FRDM_K66F                                                  // freedom board http://www.utasker.com/kinetis/FRDM-K66F.html
    //#define TEENSY_3_6                                                 // USB development board with K66FX1M0 - http://www.utasker.com/kinetis/TEENSY_3.6.html

    //#define TWR_K70F120M                                               // K processors Cortex M4 with graphical LCD, Ethernet, USB, encryption, tamper - tower board http://www.utasker.com/kinetis/TWR-K70F120M.html
    //#define K70F150M_12M                                               // development board with 150MHz K70 and 12MHz crystal

    //#define FRDM_K82F                                                  // K processors Cortex M4 with USB, encryption, tamper (scalable and secure) - freedom board http://www.utasker.com/kinetis/FRDM-K82F.html
    //#define TWR_POS_K81
    //#define TWR_K80F150M                                               // tower board http://www.utasker.com/kinetis/TWR-K80F150M.html
#elif defined _STM32
    // STM32
    //
    // Nucleo 32 range
    //
    //#define NUCLEO_F031K6                                              // evaluation board with STM32F031 (cortex-m0)
    //#define NUCLEO_L011K4                                              // evaluation board with STM32L011 (cortex-m0+)
    //#define NUCLEO_L031K6                                              // evaluation board with STM32L031 (cortex-m0+)
    //#define NUCLEO_L432KC                                              // evaluation board with STM32L432 (cortex-m4 with FPU)

    // Nucleo 144 range
    //
    //#define NUCLEO_F401RE                                              // evaluation board with STM32F401RET6
    //#define NUCLEO_F429ZI                                              // evaluation board with STM32F429ZIT6
    //#define NUCLEO_L476RG                                              // evaluation board with STM32L476RGT6U
    //#define NUCLEO_L496RG                                              // evaluation board with STM32L496ZGT6U

    //#define ST_MB913C_DISCOVERY                                        // discovery board with STM32F100RB
      #define ARDUINO_BLUE_PILL                                          // board with STM32F103C8T6 (48 pin LQFP, 64k Flash/20k SRAM performance line processor)
    //#define STM3210C_EVAL                                              // evaluation board with STM32F107VCT
    //#define STM32_P207                                                 // olimex prototyping board with STM32F207ZET6
    //#define STM32F746G_DISCO                                           // evaluation board with STM32F746NGH6
    //#define WISDOM_STM32F407                                           // evaluation board with STM32F407ZET6
    //#define STM3240G_EVAL                                              // evaluation board with STM32F407IGH6
    //#define ST_MB997A_DISCOVERY                                        // discovery board with STM32F407VGT6
    //#define STM32F407ZG_SK                                             // IAR prototyping board with STM32F407ZGT6
#endif

#if defined FRDM_KE02Z
    #define KINETIS_KE
    #define KINETIS_KE02
    #define KINETIS_MAX_SPEED    20000000
    #define TARGET_HW            "FRDM-KE02Z Kinetis"
    #define DEVICE_WITHOUT_USB                                           // KE02 doesn't have USB
    #define OUR_HEAP_SIZE        (HEAP_REQUIREMENTS)((2 * 1024) * MEM_FACTOR)
    #define DEVICE_WITHOUT_ETHERNET                                      // KE doesn't have Ethernet controller
#elif defined FRDM_KE02Z40M
    #define KINETIS_KE
    #define KINETIS_KE02
    #define KINETIS_MAX_SPEED    40000000
    #define TARGET_HW            "FRDM-KE02ZM40 Kinetis"
    #define DEVICE_WITHOUT_USB                                           // KE02 doesn't have USB
    #define OUR_HEAP_SIZE        (HEAP_REQUIREMENTS)((2 * 1024) * MEM_FACTOR)
    #define DEVICE_WITHOUT_ETHERNET                                      // KE doesn't have Ethernet controller
#elif defined FRDM_KE04Z
    #define KINETIS_KE
    #define KINETIS_KE04
    #define KINETIS_MAX_SPEED    48000000
    #define TARGET_HW            "FRDM-KE04Z Kinetis"
    #define DEVICE_WITHOUT_USB                                           // KE04 doesn't have USB
    #define OUR_HEAP_SIZE        (HEAP_REQUIREMENTS)((512) * MEM_FACTOR) // the device has only 1k of SRAM
    #define DEVICE_WITHOUT_ETHERNET                                      // KE doesn't have Ethernet controller
#elif defined FRDM_KE06Z
    #define KINETIS_KE
    #define KINETIS_KE06
    #define KINETIS_MAX_SPEED    48000000
    #define TARGET_HW            "FRDM-KE06Z Kinetis"
    #define DEVICE_WITHOUT_USB                                           // KE04 doesn't have USB
    #define OUR_HEAP_SIZE        (HEAP_REQUIREMENTS)((2 * 1024) * MEM_FACTOR)
    #define DEVICE_WITHOUT_ETHERNET                                      // KE doesn't have Ethernet controller
#elif defined FRDM_KE15Z
    #define TARGET_HW            "FRDM-KE15Z"
    #define KINETIS_MAX_SPEED    72000000
    #define OUR_HEAP_SIZE        (HEAP_REQUIREMENTS)((10 * 1024) * MEM_FACTOR)
    #define KINETIS_KE
    #define KINETIS_KE15
    #define DEVICE_WITHOUT_ETHERNET                                      // KE doesn't have Ethernet controller
    #define DEVICE_WITHOUT_USB                                           // KE doesn't have USB
  //#define KBOOT_RS485
    #if defined KBOOT_RS485
        #define SUPPORT_HW_FLOW
    #endif
#elif defined TRK_KEA64
    #define TARGET_HW            "TRK-KEA64"
    #define KINETIS_MAX_SPEED    40000000                                // 40MHz version
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
    #define OUR_HEAP_SIZE        (HEAP_REQUIREMENTS)((12 * 1024) * MEM_FACTOR)
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
#elif defined FRDM_KEAZ128Q80 || defined FRDM_KEAZ64Q64
    #define KINETIS_KE
    #define KINETIS_KEA
    #if defined FRDM_KEAZ64Q64
        #define KINETIS_KEA64
        #define TARGET_HW        "FRDM-KEAZ64Q64"
        #define OUR_HEAP_SIZE    (HEAP_REQUIREMENTS)((2 * 1024) * MEM_FACTOR)
    #else
        #define KINETIS_KEA128
        #define TARGET_HW        "FRDM-KEAZ128Q80"
        #define OUR_HEAP_SIZE    (HEAP_REQUIREMENTS)((8 * 1024) * MEM_FACTOR)
    #endif
    #define KINETIS_MAX_SPEED    48000000
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
  //#define _DEV2                                                        // special development version
    #define TARGET_HW            "FRDM-KL27Z Kinetis"
    #define OUR_HEAP_SIZE        (HEAP_REQUIREMENTS)((10 * 1024) * MEM_FACTOR)
    #define KINETIS_KL
    #define KINETIS_KL27
    #define DEVICE_WITHOUT_CAN                                           // KL doesn't have CAN controller
    #define DEVICE_WITHOUT_ETHERNET                                      // KL doesn't have Ethernet controller
#elif defined FRDM_KL28Z
    #define TARGET_HW            "FRDM-KL28Z Kinetis"
    #define OUR_HEAP_SIZE        (HEAP_REQUIREMENTS)((10 * 1024) * MEM_FACTOR)
    #define KINETIS_KL
    #define KINETIS_KL28
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
#elif defined FRDM_KL82Z
    #define KINETIS_KL
    #define KINETIS_KL82
    #define TARGET_HW       "FRDM-KL82Z"
    #if defined _WINDOWS
        #define OUR_HEAP_SIZE   (HEAP_REQUIREMENTS)((63 * 1024) * MEM_FACTOR) // use large heap so that large UART input buffer can be used when simulating in order to avoid reception overflows
    #else
        #define OUR_HEAP_SIZE   (HEAP_REQUIREMENTS)((12 * 1024) * MEM_FACTOR)
    #endif
    #define DEVICE_WITHOUT_ETHERNET                                      // KL doesn't have Ethernet controller
#elif defined TWR_KV10Z32
    #define TARGET_HW            "TWR-KV10Z32"
    #define OUR_HEAP_SIZE        (HEAP_REQUIREMENTS)((5 * 1024) * MEM_FACTOR)
    #define KINETIS_KV
    #define KINETIS_KV10                                                 // specify the sub-family type
    #define DEVICE_WITHOUT_USB
    #define DEVICE_WITHOUT_CAN
    #define DEVICE_WITHOUT_ETHERNET
#elif defined TWR_KV31F120M || defined TWR_KV46F150M || defined FRDM_KV31F
    #if defined FRDM_KV31F
        #define TARGET_HW        "FRDM-KV31F"
    #elif defined TWR_KV46F150M
        #define TARGET_HW        "TWR-KV46F150M"
    #else
        #define TARGET_HW        "TWR-KV31F120M"
    #endif
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
#elif defined TWR_KV58F220M
    #define TARGET_HW            "TWR-KV58F220M"
    #define OUR_HEAP_SIZE        (HEAP_REQUIREMENTS)((32 * 1024) * MEM_FACTOR)
    #define KINETIS_MAX_SPEED    220000000
    #define KINETIS_K_FPU                                                // part with floating point unit
    #define KINETIS_KV
    #define KINETIS_KV50                                                 // specify the sub-family type
    #define KINETIS_KV58
    #define DEVICE_WITHOUT_USB
    #define DEVICE_WITHOUT_CAN
    #define KINETIS_REVISION_2
#elif defined TWR_KM34Z50M
    #define TARGET_HW            "TWR-KM34Z50M"
    #define OUR_HEAP_SIZE        (HEAP_REQUIREMENTS)((8 * 1024) * MEM_FACTOR)
    #define KINETIS_KM
    #define KINETIS_KM34                                                 // specify the sub-family type
    #define KINETIS_MAX_SPEED    50000000
    #define DEVICE_WITHOUT_USB
    #define DEVICE_WITHOUT_CAN
    #define DEVICE_WITHOUT_ETHERNET
#elif defined TWR_KM34Z75M
    #define TARGET_HW            "TWR-KM34Z75M"
    #define OUR_HEAP_SIZE        (HEAP_REQUIREMENTS)((8 * 1024) * MEM_FACTOR)
    #define KINETIS_KM
    #define KINETIS_KM34                                                 // specify the sub-family type
    #define KINETIS_MAX_SPEED    75000000
    #define DEVICE_WITHOUT_USB
    #define DEVICE_WITHOUT_CAN
    #define DEVICE_WITHOUT_ETHERNET
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
#elif defined K02F100M || defined K12D50M
  //#define DEV5                                                         // special option which forces SD card loading
    #if defined K12D50M
        #define TARGET_HW            "K12D50M"
        #define KINETIS_K10                                              // specify the sub-family
        #define KINETIS_K12                                              // extra sub-family type precision
        #define KINETIS_MAX_SPEED    50000000
    #else
        #define TARGET_HW            "K02F100M"
        #define KINETIS_K_FPU                                           // part with floating point unit
        #define KINETIS_K00                                             // specify the sub-family
        #define KINETIS_K02                                             // extra sub-family type precision
        #define KINETIS_MAX_SPEED    100000000
    #endif
    #define DEVICE_WITHOUT_ETHERNET                                      // K02F doesn't have Ethernet controller
    #define DEVICE_WITHOUT_USB                                           // K02F doesn't have USB
    #define DEVICE_WITHOUT_CAN                                           // K02F doesn't have CAN controller
    #define OUR_HEAP_SIZE        (HEAP_REQUIREMENTS)((10 * 1024) * MEM_FACTOR)
    #if defined DEV5
        #define SDCARD_SUPPORT                                           // use SD card loading method
        #define UREVERSEMEMCPY                                           // required when SD card used in SPI mode
    #endif
#elif defined TWR_K20D50M
    #define KINETIS_K20
    #define KINETIS_MAX_SPEED    50000000
    #define TARGET_HW       "TWR-K20N50M Kinetis"
    #define DEVICE_WITHOUT_ETHERNET                                      // K20 doesn't have Ethernet controller
    #define OUR_HEAP_SIZE   (HEAP_REQUIREMENTS)((12 * 1024) * MEM_FACTOR)
#elif defined TWR_K20D72M
  //#define DEV1                                                         // temporary development configuration
    #define KINETIS_K20
    #define KINETIS_MAX_SPEED    72000000
    #define KINETIS_REVISION_2
    #define TARGET_HW       "TWR-K20D72M Kinetis"
    #define DEVICE_WITHOUT_ETHERNET                                      // K20 doesn't have Ethernet controller
    #if defined DEV1
        #define OUR_HEAP_SIZE   (HEAP_REQUIREMENTS)((8 * 1024) * MEM_FACTOR)
    #else
        #define OUR_HEAP_SIZE   (HEAP_REQUIREMENTS)((12 * 1024) * MEM_FACTOR)
    #endif
#elif defined TEENSY_3_1
  //#define SPECIAL_VERSION                                              // temporary special version with some specific setups
      //#define SPECIAL_VERSION_SDCARD                                   // temporary special version with some specific setups
      //#define SPECIAL_VERSION_2                                        // temporary special version with some specific setups
        #if defined SPECIAL_VERSION && defined SPECIAL_VERSION_SDCARD
            #define USE_USB_MSD                                          // host to SD card
            #define SDCARD_SUPPORT
            #define WILDCARD_FILES
          //#define DELETE_SDCARD_FILE_AFTER_UPDATE
        #endif
    #define KINETIS_K20
    #define KINETIS_REVISION_2
    #define TARGET_HW   "TEENSY 3.1 (K20DX256)"
    #if defined SPECIAL_VERSION_2
        #define KINETIS_MAX_SPEED    50000000
        #define OUR_HEAP_SIZE   (HEAP_REQUIREMENTS)((10 * 1024) * MEM_FACTOR)
    #else
        #define KINETIS_MAX_SPEED    72000000
        #define OUR_HEAP_SIZE   (HEAP_REQUIREMENTS)((12 * 1024) * MEM_FACTOR)
    #endif
    #define DEVICE_WITHOUT_ETHERNET                                      // K20 doesn't have Ethernet controller
#elif defined FRDM_K20D50M
    #define KINETIS_K20
    #define KINETIS_MAX_SPEED    50000000
    #define TARGET_HW       "FRDM-K20D50M Kinetis"
    #define DEVICE_WITHOUT_ETHERNET                                      // K20 doesn't have Ethernet controller
    #define OUR_HEAP_SIZE   (HEAP_REQUIREMENTS)((8 * 1024) * MEM_FACTOR)
#elif defined tinyK20
  //#define TINYK20_16MHz                                                // special version with 16MHz crystal
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
#elif defined FRDM_K22F ||defined tinyK22
    #if defined tinyK22
        #define TARGET_HW        "tinyK22"
    #else
      //#define DEV1                                                     // temporary development configuration
        #define TARGET_HW        "FRDM-K22F"
    #endif
    #define KINETIS_K_FPU                                                // part with floating point unit
    #define KINETIS_K20
    #define KINETIS_K22
    #define KINETIS_REVISION_2
    #define DEVICE_WITHOUT_ETHERNET                                      // K22 doesn't have Ethernet controller
    #define OUR_HEAP_SIZE   (HEAP_REQUIREMENTS)((62 * 1024) * MEM_FACTOR)
#elif defined BLAZE_K22
    #define TARGET_HW            "BLAZE"
    #define KINETIS_K_FPU                                                // part with floating point unit
    #define KINETIS_MAX_SPEED    120000000
    #define KINETIS_K20                                                  // specify the sub-family
    #define KINETIS_K22                                                  // extra sub-family type precision
    #define KINETIS_REVISION_2
    #define DEVICE_WITHOUT_ETHERNET                                      // K20 doesn't have Ethernet controller
    #define OUR_HEAP_SIZE        (HEAP_REQUIREMENTS)((24 * 1024) * MEM_FACTOR)
  //#define SUPPORT_GLCD                                                 // enable the task for interfacing to a graphical LCD
        #define TFT2N0369_GLCD_MODE                                      // use colour TFT in GLCD compatible mode (as base)
        #define ST7789S_GLCD_MODE                                        // adjustments for specific controller
  //#define SUPPORT_TOUCH_SCREEN                                         // with touch screen operation
        #define TOUCH_FT6206                                             // FT6206 capacitative touch panel controller
            #define DONT_HANDLE_TOUCH_SCREEN_MOVEMENT                    // don't handle movement
    #define GLCD_BACKLIGHT_CONTROL                                       // PWM based backlight control
        #define FIXED_BACKLIGHT_INTENSITY                                // don't use PWM but instead fixed on
    #define I2C_INTERFACE                                                // enable I2C driver for touch screen interface
        #define GLCD_X             240                                   // horizontal resolution of the GLCD in pixels
        #define GLCD_Y             240                                   // vertical resolution of the GLCD in pixels
        #undef BIG_PIXEL
        #define CGLCD_PIXEL_SIZE   1                                     // for each CGLCD pixel use 2 physical pixels in x and y directions - also reduces GLCD memory requirements
        #define LCD_PARTNER_TASK   TASK_APPLICATION                      // GLCD task sends initialise complete event and acks to this task
            #define LCD_PIXEL_COLOUR   (COLORREF)RGB(247,150,17)         // RGB colour of LCD pixel when on
            #define LCD_ON_COLOUR      (COLORREF)RGB(40,40,40)           // RGB colour of LCD when backlight is on
            #define LCD_OFF_COLOUR     (COLORREF)RGB(0,0,0)              // RGB colour of LCD when backlight is off (used only by the simulator)
            #define VARIABLE_PIXEL_COLOUR

    #define USER_SPECIAL_SEVEN_DOT      0                                // extend 7 dot font by 0 user characters
    #define USER_SPECIAL_NINE_DOT       0                                // extend 9 dot font by 3 user characters
    #define USER_SPECIAL_TEN_DOT        0                                // extend 10 dot font by 0 user characters
    #define USER_SPECIAL_FIFTEEN_DOT    0                                // extend 15 dot font by 0 user characters
    #define USER_SPECIAL_EIGHTEEN_DOT   0                                // extend 18 dot font by 0 user characters

  //#define EN_FIVE_DOT                                                  // define the fonts that should be available for use
  //#define EN_FIVE_DOT_FIX
  //#define EN_SIX_DOT
  //#define EN_SIX_DOT_FIX
  //#define EN_SEVEN_DOT
    #define EN_NINE_DOT
  //#define EN_TEN_DOT
  //#define EN_FIFTEEN_DOT
  //#define EN_EIGHTEEN_DOT
  //#define EN_CHAR_LCD_DOT
  //#define MAX_BLINKING_OBJECTS   3                                     // the number of blinking objects to be supported - comment out when not required
    #define MAX_TEXT_LENGTH        64                                    // maximum text length when writing fonts

    #if defined _NO_CHECK_QUEUE_INPUT
        #undef _NO_CHECK_QUEUE_INPUT
    #endif
#elif defined TWR_K22F120M
    #define TARGET_HW       "TWR-K22F120M"
    #define KINETIS_K_FPU                                                // part with floating point unit
    #define KINETIS_K20
    #define KINETIS_K22
    #define KINETIS_REVISION_2
    #define DEVICE_WITHOUT_ETHERNET                                      // K22 doesn't have Ethernet controller
    #define OUR_HEAP_SIZE   (HEAP_REQUIREMENTS)((24 * 1024) * MEM_FACTOR)
#elif defined K24FN1M0_120
    #define TARGET_HW            "K24FN1M0-120M"
    #define KINETIS_K_FPU                                                // part with floating point unit
    #define KINETIS_MAX_SPEED    120000000
    #define KINETIS_K20                                                  // specify the sub-family
    #define KINETIS_K24                                                  // extra sub-family type precision
    #define KINETIS_REVISION_2
    #define DEVICE_WITHOUT_ETHERNET                                      // K20 doesn't have Ethernet controller
    #define OUR_HEAP_SIZE        (HEAP_REQUIREMENTS)((24 * 1024) * MEM_FACTOR)
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
#elif defined FRDM_K28F
    #define TARGET_HW            "FRDM-K28F"
    #define OUR_HEAP_SIZE        (HEAP_REQUIREMENTS)((48 * 1024) * MEM_FACTOR) // large SRAM parts
    #define KINETIS_MAX_SPEED    150000000
    #define KINETIS_K_FPU                                                // part with floating point unit
    #define KINETIS_K60                                                  // specify the sub-family
    #define KINETIS_REVISION_2
    #define KINETIS_K66                                                  // extra sub-family type precision
    #define USB_HS_INTERFACE                                             // use HS interface rather than FS interface
    #define DEVICE_WITHOUT_ETHERNET                                      // K20 doesn't have Ethernet controller
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
  //#define DEV6                                                         // special option which forces SD card loading
    #define TARGET_HW       "TWR-K60D100M"
    #define KINETIS_K60
    #define KINETIS_REVISION_2
    #define OUR_HEAP_SIZE   (HEAP_REQUIREMENTS)((24 * 1024) * MEM_FACTOR)
    #if defined DEV6
        #define SDCARD_SUPPORT                                           // use SD card loading method
        #define UREVERSEMEMCPY                                           // required when SD card used in SPI mode
    #endif
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
#elif defined HEXIWEAR_K64F
    #define TARGET_HW            "HEXIWEAR-K64F"
    #define OUR_HEAP_SIZE        (HEAP_REQUIREMENTS)((48 * 1024) * MEM_FACTOR) // large SRAM parts
    #define KINETIS_MAX_SPEED    120000000
    #define KINETIS_K_FPU                                                // part with floating point unit
    #define KINETIS_K60                                                  // specify the sub-family
    #define KINETIS_REVISION_2
    #define KINETIS_K64                                                  // extra sub-family type precision
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
    #define TWR_SER                                                      // use TWR-SER serial board instead of OpenSDA virtual COM port
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
#elif defined FRDM_K66F || defined K66FX1M0
    #define TARGET_HW            "FRDM-K66F"
    #define OUR_HEAP_SIZE        (HEAP_REQUIREMENTS)((48 * 1024) * MEM_FACTOR) // large SRAM parts
    #define KINETIS_MAX_SPEED    180000000
    #define KINETIS_K_FPU                                                // part with floating point unit
    #define KINETIS_K60                                                  // specify the sub-family
    #define KINETIS_REVISION_2
    #define KINETIS_K66                                                  // extra sub-family type precision
    #if !defined K66FX1M0
        #define USB_HS_INTERFACE                                         // use HS interface (USB1) rather than FS interface (USB0)
      //#define MEMORY_SWAP                                              // use memory swap method
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
  //#define USB_HS_INTERFACE                                             // use HS interface rather than FS interface (needs external ULPI transceiver)
  //#define TWR_SER2                                                     // use SER2 serial board instead of standard serial board (used also when HS USB is enabled)
    #define OUR_HEAP_SIZE   (HEAP_REQUIREMENTS)((24 * 1024) * MEM_FACTOR)// kinetis uses dedicated Ethernet buffers so doesn't need heap for these
#elif defined K70F150M_12M
    #define DWGB_SDCARD                                                  // SD card configuration option
    #define TARGET_HW       "K70F150M-12MHz crystal"
    #define KINETIS_MAX_SPEED    150000000
    #define KINETIS_K_FPU                                                // part with floating point unit
    #define KINETIS_K70
    #define OUR_HEAP_SIZE   (HEAP_REQUIREMENTS)((24 * 1024) * MEM_FACTOR)
    #if defined DWGB_SDCARD
        #define SDCARD_SUPPORT                                           // SD card loading
        #define DELETE_SDCARD_FILE_AFTER_UPDATE
        #define UTFAT_WRITE                                              // ensure SD card write is supported in order to delete firmware file after successful update
      //#define SDCARD_DETECT_INPUT_POLL                                 // use card detection switch to identify SD card inserted (not needed since the serial lloader is only entered when detected)
    #endif
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
#elif defined STM3210C_EVAL
    #define TARGET_HW       "STM3210C-EVAL (STM32F107VCT)"
    #define _STM32F107X                                                  // part group
    #define OUR_HEAP_SIZE (HEAP_REQUIREMENTS)((24 * 1024) * MEM_FACTOR)  // we have the LAN buffers in HEAP and big RX/TX
#elif defined STM3240G_EVAL
    #define _ERRATE_REV_A_Z                                              // activate (SDIO) workarounds for revisions A and Z
    #define TARGET_HW       "STM3240C-EVAL (STM32F407IGH6)"
    #define _STM32F4XX
    #define _STM32F407
    #define OUR_HEAP_SIZE (HEAP_REQUIREMENTS)((24 * 1024) * MEM_FACTOR)  // we have the LAN buffers in HEAP and big RX/TX
#elif defined STM32F407ZG_SK
    #define _ERRATE_REV_A_Z                                              // activate (SDIO) workarounds for revisions A and Z
    #define TARGET_HW       "STM32F407ZG-SK"
    #define _STM32F4XX
    #define _STM32F407
    #define OUR_HEAP_SIZE (HEAP_REQUIREMENTS)((32 * 1024) * MEM_FACTOR)
#elif defined ST_MB997A_DISCOVERY
    #define EMBEST_BASE_BOARD                                            // activate when available to use Ethernet and SD card
    #define _ERRATE_REV_A_Z                                              // activate (SDIO) workarounds for revisions A and Z
    #if defined EMBEST_BASE_BOARD
        #define TARGET_HW       "MB997A DISCOVERY/MO (STM32F407VGT6)"
    #else
        #define TARGET_HW       "MB997A DISCOVERY (STM32F407VGT6)"
        #define DEVICE_WITHOUT_ETHERNET                                  // board doesn't have Ethernet without base-board
    #endif
    #define _STM32F4XX
    #define _STM32F407
    #define OUR_HEAP_SIZE (HEAP_REQUIREMENTS)((32 * 1024) * MEM_FACTOR)
#elif defined STM32F405_BOARD
    #define TARGET_HW       "STM32F405 BOARD"
    #define FINAL_HW
    #define UREVERSEMEMCPY
    #define DEVICE_WITHOUT_ETHERNET                                      // board doesn't have Ethernet
    #define _STM32F4XX
    #define _STM32F407
    #define OUR_HEAP_SIZE (HEAP_REQUIREMENTS)((32 * 1024) * MEM_FACTOR)
#elif defined ST_MB913C_DISCOVERY
    #define TARGET_HW       "MB913C DISCOVERY (STM32F100RBT6B)"
    #define _STM32F100X                                                  // part group
    #define DEVICE_WITHOUT_ETHERNET                                      // STM32F100 doesn't have Ethernet
    #define OUR_HEAP_SIZE (HEAP_REQUIREMENTS)((2 * 1024) * MEM_FACTOR)
#elif defined ARDUINO_BLUE_PILL
    #define TARGET_HW            "BLUE PILL (STM32F103C8T6)"
    #define _STM32F103X                                                  // part group
    #define DEVICE_WITHOUT_ETHERNET                                      // STM32F103 doesn't have Ethernet
    #define OUR_HEAP_SIZE        (HEAP_REQUIREMENTS)((6 * 1024) * MEM_FACTOR)
#elif defined STM32_P207
    #define _ERRATE_REV_A_Z                                              // activate (SDIO) workarounds for revisions A and Z
    #define TARGET_HW       "STM32-P207"
    #define _STM32F2XX
    #define _STM32F207
    #define OUR_HEAP_SIZE (HEAP_REQUIREMENTS)((24 * 1024) * MEM_FACTOR)  // we have the LAN buffers in HEAP and big RX/TX
#elif defined STM32F746G_DISCO
    #define TARGET_HW            "STM32F746-Discovery"
    #define _STM32F7XX
    #define _STM32F746
    #define OUR_HEAP_SIZE        (HEAP_REQUIREMENTS)((24 * 1024) * MEM_FACTOR) // we have the LAN buffers in HEAP and big RX/TX
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

// Configure MODBUS extension package
//
//#define USE_MODBUS                                                     // activate MODBUS support in the project
#if defined USE_MODBUS
    #undef _NO_CHECK_QUEUE_INPUT
    #define SUPPORT_FLUSH
    #define USE_MODBUS_SLAVE                                             // slave capability supported
      #define NO_SLAVE_MODBUS_READ_COILS                                 // disable specific slave public function support
      #define NO_SLAVE_MODBUS_READ_DISCRETE_INPUTS
    //#define NO_SLAVE_MODBUS_READ_HOLDING_REGISTERS
    //#define NO_SLAVE_MODBUS_READ_INPUT_REGISTERS
      #define NO_SLAVE_MODBUS_WRITE_SINGLE_COIL
    //#define NO_SLAVE_MODBUS_WRITE_SINGLE_REGISTER
      #define NO_SLAVE_MODBUS_READ_EXCEPTION_STATUS
      #define NO_SLAVE_MODBUS_DIAGNOSTICS
      #define NO_SLAVE_MODBUS_DIAG_SUB_RETURN_QUERY_DATA
      #define NO_SLAVE_MODBUS_DIAG_SUB_RESTART_COMS
      #define NO_SLAVE_MODBUS_DIAG_SUB_RETURN_DIAG_REG
      #define NO_SLAVE_MODBUS_DIAG_SUB_CHANGE_ASCII_DELIM
      #define NO_SLAVE_MODBUS_DIAG_SUB_FORCE_LISTEN_ONLY
      #define NO_SLAVE_MODBUS_DIAG_SUB_CLEAR_DISGNOSTICS
      #define NO_SLAVE_MODBUS_DIAG_SUB_RTN_BUS_MSG_CNT
      #define NO_SLAVE_MODBUS_DIAG_SUB_RTN_BUS_COM_ERR_CNT
      #define NO_SLAVE_MODBUS_DIAG_SUB_RTN_BUS_EXC_ERR_CNT
      #define NO_SLAVE_MODBUS_DIAG_SUB_RTN_SLAVE_MSG_CNT
      #define NO_SLAVE_MODBUS_DIAG_SUB_RTN_SLAVE_NO_RSP_CNT
      #define NO_SLAVE_MODBUS_DIAG_SUB_RTN_SLAVE_NAK_CNT
      #define NO_SLAVE_MODBUS_DIAG_SUB_RTN_SLAVE_BSY_CNT
      #define NO_SLAVE_MODBUS_DIAG_SUB_RTN_BUS_CHR_ORUN_CNT
      #define NO_SLAVE_MODBUS_DIAG_SUB_CLEAR_ORUN_AND_FLG
      #define NO_SLAVE_MODBUS_GET_COMM_EVENT_COUNTER
      #define NO_SLAVE_MODBUS_GET_COMM_EVENT_LOG
      #define NO_SLAVE_MODBUS_WRITE_MULTIPLE_COILS
    //#define NO_SLAVE_MODBUS_WRITE_MULTIPLE_REGISTERS
      #define NO_SLAVE_MODBUS_REPORT_SLAVE_ID
      #define NO_SLAVE_MODBUS_MASK_WRITE_REGISTER
      #define NO_SLAVE_MODBUS_READ_WRITE_MULTIPLE_REGISTER
      #define NO_SLAVE_MODBUS_READ_FIFO_QUEUE
      #define NOTIFY_ONLY_COIL_CHANGES                                   // notify user of individual coil changes only
    //#define USE_MODBUS_MASTER                                          // master capability supported (either slave or master required)
      #define NO_MASTER_MODBUS_READ_COILS                                // disable specific master public function support
      #define NO_MASTER_MODBUS_READ_DISCRETE_INPUTS
      #define NO_MASTER_MODBUS_READ_HOLDING_REGISTERS
      #define NO_MASTER_MODBUS_READ_INPUT_REGISTERS
      #define NO_MASTER_MODBUS_WRITE_SINGLE_COIL
      #define NO_MASTER_MODBUS_WRITE_SINGLE_REGISTER
      #define NO_MASTER_MODBUS_READ_EXCEPTION_STATUS
      #define NO_MASTER_MODBUS_DIAGNOSTICS                               // shared by all sub-functions
      #define NO_MASTER_MODBUS_GET_COMM_EVENT_COUNTER
      #define NO_MASTER_MODBUS_GET_COMM_EVENT_LOG
      #define NO_MASTER_MODBUS_WRITE_MULTIPLE_COILS
      #define NO_MASTER_MODBUS_WRITE_MULTIPLE_REGISTERS
      #define NO_MASTER_MODBUS_REPORT_SLAVE_ID
      #define NO_MASTER_MODBUS_MASK_WRITE_REGISTER
      #define NO_MASTER_MODBUS_READ_WRITE_MULTIPLE_REGISTER
      #define NO_MASTER_MODBUS_READ_FIFO_QUEUE
    #if defined USE_MODBUS_MASTER
      //#define MODBUS_GATE_WAY_ROUTING                                  // configurable routing from slave gateways (requires master functionality)
      //#define MODBUS_GATE_WAY_QUEUE                                    // support queuing of MODBUS transmissions - advisable for gateways
            #define SUPPORT_FIFO_QUEUES
            #define MODBUS_DELAYED_RESPONSE                              // allow slave parameter interface to delay request responses - for example to prepare latest data from external location
            #define MAX_QUEUED_REQUEST_LENGTH    8                       // longest request data length that needs to be saved when requests are delayed
    #endif
#endif


#if !(defined K70F150M_12M && !defined DWGB_SDCARD) && !defined KWIKSTIK && !(defined TEENSY_3_1 && defined SPECIAL_VERSION) && !defined BLAZE_K22 && !(defined FRDM_KL27Z && defined _DEV2)
  //#define SERIAL_INTERFACE                                             // enable serial interface driver
#endif
#if defined SERIAL_INTERFACE
    #if defined USE_MODBUS
        #define MODBUS_RTU                                               // support binary RTU mode
      //#define MODBUS_ASCII                                             // support ASCII mode
        #define STRICT_MODBUS_SERIAL_MODE                                // automatically adjust the character length according to mode
        #define MODBUS_SERIAL_INTERFACES      1
      //#define MODBUS_SHARED_SERIAL_INTERFACES   3                      // number of slave interfaces sharing UARTs
        #define MODBUS_RS485_SUPPORT                                     // support RTS control for RS485 transmission
      //#define FAST_MODBUS_RTU                                          // speeds of greater than 19200 use calculated RTU times rather than recommended fixed values
        #if defined MODBUS_RS485_SUPPORT
            #if !defined SUPPORT_HW_FLOW
                #define SUPPORT_HW_FLOW                                  // ensure that HW flow control is enabled when RS485 support required
                #define LOW_WATER_MARK    20                             // 20% low water mark
                #define HIGH_WATER_MARK   80                             // 80% high water mark
            #endif
            #define UART_FRAME_COMPLETE                                  // the UART driver informs of frame completion - activated when RS485 mode is required
        #endif
      //#define MODBUS_SUPPORT_SERIAL_LINE_FUNCTIONS                     // support the serial line function at the slave
      //#define MODBUS_SUPPORT_SERIAL_LINE_DIAGNOSTICS                   // support serial line diagnostics
      //#define MODBUS_CRC_FROM_LOOKUP_TABLE                             // MODBUS RTU cyclic redundancy check performed with help of loop up table (requires 512 bytes FLASH table, but faster than calculation loop)
        #define REMOVE_SREC_LOADING
    #else
      //#define KBOOT_LOADER                                             // use KBOOT UART interface rather than SREC/iHex interface
          //#define KBOOT_LOADER_MASS_ERASE_TO_UNLOCK
          //#define KBOOT_LOADER_BACKDOOR_KEY_TO_UNLOCK
          //#define KBOOT_SECURE_LOADER                                  // decrypt and accept only encrypted/authenticated firmware
      //#define DEVELOPERS_LOADER                                        // Freescale Developer's Bootloader (AN2295) compatible mode (rather than SREC/iHex)
          //#define DEVELOPERS_LOADER_PROTOCOL_VERSION_9                 // user protocol version 9 rather than obsolete Kinetis 8 (not completed at the moment)
            #define DEVELOPERS_LOADER_READ                               // support reading back program
            #define DEVELOPERS_LOADER_CRC                                // support CRC in communication
        #define REMOVE_SREC_LOADING                                      // disable SREC (and Intel Hex) loading but keep debug output and the command line menu
        #if !defined REMOVE_SREC_LOADING
          //#define SUPPORT_INTEL_HEX_MODE                               // support Intel Hex mode together with SREC (auto-recognition)
          //#define EXCLUSIVE_INTEL_HEX_MODE                             // loading mode is exclusively Intel Hex (use with or without SUPPORT_INTEL_HEX_MODE)
        #endif
      //#define SERIAL_STATS                                             // keep statistics about serial interface use
      //#define SUPPORT_MSG_MODE                                         // enable terminator recognition (MSG_MODE)
      //#define SUPPORT_MSG_CNT                                          // enable the message counter mode (MSG_MODE_RX_CNT) - requires also SUPPORT_MSG_MODE
      //#define WAKE_BLOCKED_TX                                          // allow a blocked transmitter to continue after an interrupt event
      //#define SUPPORT_FLUSH                                            // support rx flush
        #define SERIAL_SUPPORT_XON_XOFF                                  // enable XON/XOFF support in driver
        #define HIGH_WATER_MARK   20                                     // stop flow control when the input buffer has less than this space (if variable settings are required, use SUPPORT_FLOW_HIGH_LOW define)
        #define LOW_WATER_MARK    20                                     // restart when the input buffer content falls below this value
      //#define SUPPORT_FLOW_HIGH_LOW                                    // allow flow control levels to be configured (in % of buffer size)
      //#define SERIAL_SUPPORT_ECHO                                      // enable echo mode in rx driver
      //#define SERIAL_SUPPORT_ESCAPE                                    // enable escape sequencing in driver
      //#define SERIAL_SUPPORT_SCAN                                      // serial receiver supports scanning of input buffer for a sequence
      //#define SUPPORT_HW_FLOW                                          // support RTS/CTS flow control and other possible modem signals
    #endif

    #define LOG_UART0                                                    // activate this option to log all data sent to UART 0 to a file called "UART0.txt"
    #define LOG_UART1                                                    // activate this option to log all data sent to UART 1 to a file called "UART1.txt"
    #define LOG_UART2                                                    // activate this option to log all data sent to UART 2 to a file called "UART2.txt"
    #define LOG_UART3                                                    // activate this option to log all data sent to UART 3 to a file called "UART3.txt"
    #define LOG_UART4                                                    // activate this option to log all data sent to UART 2 to a file called "UART2.txt"
    #define LOG_UART5                                                    // activate this option to log all data sent to UART 3 to a file called "UART3.txt"
#else
    #define NUMBER_SERIAL              0                                 // no physical queue needed
#endif
#define NUMBER_EXTERNAL_SERIAL         0                                 // no external UARTs

//#define I2C_INTERFACE                                                  // enable I2C slave mode - see I2C slave loader video at https://youtu.be/awREsqeCEzQ
#if defined I2C_INTERFACE
    #define NUMBER_I2C       (I2C_AVAILABLE + LPI2C_AVAILABLE)           // I2C interfaces available
    #define I2C_SLAVE_MODE                                               // support slave mode (interrupt call-back method)
#else
    #define NUMBER_I2C     0                                             // no physical queue needed
#endif

#if defined DEVICE_WITHOUT_USB || defined DWGB_SDCARD
    #define NUMBER_USB     0                                             // no physical queue needed
#else
    #define USB_INTERFACE                                                // enable USB driver interface
    #if defined USB_INTERFACE
      //#define USE_USB_CDC                                              // allow SREC/iHex loading via virtual COM
        #define USB_MSD_DEVICE_LOADER                                    // USB-MSD device mode (the board appears as a hard-drive to the host)
      //#define USB_MSD_HOST_LOADER                                      // USB-MSD host mode (the board operates as host and can read new code from a memory stick)
        #if defined USE_USB_CDC
            #undef SERIAL_INTERFACE                                      // remove the UART interface
            #define NUMBER_SERIAL          0
        #endif
        #if defined USB_MSD_DEVICE_LOADER
          //#define USB_MSD_DEVICE_SECURE_LOADER
            #define USB_DEVICE_SUPPORT                                   // requires USB device driver support
          //#define USB_MSD_TIMEOUT                                      // if there is no enumeration within a short time the application will be started
          //#define FAT_EMULATION                                        // use fat emulation
            #if defined FAT_EMULATION
                #define EMULATED_FAT_LUNS  2
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
          //#define USB_MSD_REJECTS_BINARY_FILES                         // default is to accept binary files
          //#define USB_MSD_ACCEPTS_SREC_FILES                           // optionally accept SREC content
          //#define USB_MSD_ACCEPTS_HEX_FILES                            // optionally accept Intel HEX content
        #endif
        #if defined USB_MSD_HOST_LOADER                                  // support loading from memory stick
            #if defined USB_HS_INTERFACE
                #define NXP_MSD_HOST                                     // use NXP USB host interface for realisation
            #endif
            #define USB_MSD_HOST                                         // requires USB-MSD support in the mass-storage module
            #if !defined NXP_MSD_HOST || defined _WINDOWS
                #define USB_HOST_SUPPORT                                 // requires USB host driver support
            #endif
            #define SUPPORT_USB_SIMPLEX_HOST_ENDPOINTS                   // allow operation with memory sticks using bulk IN/OUT on the same endpoint
            #define RANDOM_NUMBER_GENERATOR                              // random numbers required for USB-MSD host tags
          //#define DELETE_SDCARD_FILE_AFTER_UPDATE
            #if defined DELETE_SDCARD_FILE_AFTER_UPDATE
                #define UTFAT_WRITE
            #endif
        #endif
        #if defined USB_MSD_DEVICE_LOADER || defined USB_MSD_HOST_LOADER
            #define SUPPORT_FLUSH                                        // allow flush command to be used (important for mass storage class)
            #if defined _DEV2
                #define USB_MSD_PARAMETER_FILE                           // allow programming a parameter file in addition to an application image
            #endif
        #endif
      //#define USE_USB_MSD                                              // full USB-MSD to SD card interface on USB (no emulated loader function) - requires SDCARD_SUPPORT (USB_MSD_DEVICE_LOADER can be disabled)
          //#define DISK_COUNT         1                                 // single upload disk (set to 2 for two upload disks)
          //#define DEBUG_MAC                                            // activate debug output used to monitor the operation of MAC OS X
      //#define HID_LOADER                                               // Freescale HIDloader.exe or KBOOT compatible
          //#define KBOOT_HID_LOADER                                     // select KBOOT mode of operation (rather than HIDloader.exe)
              //#define KBOOT_SECURE_LOADER                              // decrypt and accept only encrypted/authenticated firmware
                #define KBOOT_HID_ENUMERATION_LIMIT  (DELAY_LIMIT)(5 * SEC)  // if there is no USB enumeration we start the application after this delay
                #define KBOOT_COMMAND_LIMIT          (DELAY_LIMIT)(10 * SEC) // if there is no valid KBOOT command received after this delay the application will be started
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
          //#define USB_SIMPLEX_ENDPOINTS                                // share IN and OUT on single endpoint
        #endif
        #define USB_STRING_OPTION                                        // support optional string descriptors (host or device)
            #define USB_MAX_STRINGS                3                     // the maximum number of strings supported by host
            #define USB_MAX_STRING_LENGTH          16                    // the maximum length of each string supported by host (unicode characters)
        #if defined _DEV2
            #define USB_RUN_TIME_DEFINABLE_STRINGS                       // enable USB string content to be defined at run time (variable)
            #if !defined USB_STRING_OPTION
                #define USB_STRING_OPTION                                // support optional string descriptors
            #endif
        #else
          //#define USB_RUN_TIME_DEFINABLE_STRINGS                       // enable USB string content to be defined at run time (variable)
        #endif
      //#define USE_USB_OTG_CHARGE_PUMP                                  // enable charge pump control in the driver
        #if defined USE_USB_OTG_CHARGE_PUMP
            #define IIC_INTERFACE                                        // activate IIC interface since it will be needed
        #endif
        #define USB_POWER_CONSUMPTION_MA 100                             // USB powered with maximum 100mA
    #else
        #define NUMBER_USB     0                                         // no physical queue needed
    #endif
#endif

#if !defined TWR_K20D50M && !defined FRDM_K20D50M && !defined FRDM_KL46Z && !defined FRDM_KL43Z && !defined TWR_KL46Z48M && !defined FRDM_KL26Z && !defined FRDM_KL27Z && !defined TWR_KL25Z48M && !defined FRDM_KL02Z && !defined FRDM_KL03Z && !defined FRDM_KL05Z && !defined FRDM_KE02Z && !defined FRDM_KE02Z40M && !defined FRDM_KE04Z && !defined TWR_K20D72M && !defined TWR_K21D50M && !defined TWR_K22F120M && !defined TWR_K24F120M && !defined K24FN1M0_120 && !defined FRDM_K22F && !defined TWR_KV10Z32 && !defined TWR_KV31F120M && !defined K66FX1M0 // boards have no SD card socket
  //#define SDCARD_SUPPORT                                               // SD-card interface (only choose one of these options at a time)
  //#define SPI_FLASH_FAT                                                // SPI flash
        #define SIMPLE_FLASH                                             // don't perform block management and wear-levelling
        #define FLASH_FAT_MANAGEMENT_ADDRESS     (SIZE_OF_FLASH)
  //#define DELETE_SDCARD_FILE_AFTER_UPDATE                              // once new firmware has been copied from the SD card it will be automatically deleted from the card
    #if defined SERIAL_INTERFACE && !defined REMOVE_SREC_LOADING
        #define UTFAT_DISABLE_DEBUG_OUT                                  // disable general mass-storage output so that the SREC loader is not disturbed
    #endif
    #if defined DELETE_SDCARD_FILE_AFTER_UPDATE || defined USE_USB_MSD
        #define UTFAT_WRITE
    #endif
    #if defined USE_USB_MSD
        #define SUPPORT_FLUSH
    #endif
    #if defined SDCARD_SUPPORT
      //#define SDCARD_SECURE_LOADER
        #if !defined DWGB_SDCARD && defined USE_USB_MSD
            #define UREVERSEMEMCPY                                       // required when SD card used in SPI mode
            #if !defined SPECIAL_VERSION_SDCARD && !defined DEV5 && !defined DEV6
                #define SDCARD_ACCESS_WITHOUT_UTFAT                      // no utFAT interface (just low level access for USB-MSD operation)
                #define SDCARD_FIXED                                     // no SD card monitoring since it is fixed in hardware
                #if defined SDCARD_FIXED
                    #define UTFAT_MULTIPLE_BLOCK_READ
                  //#define UTFAT_MULTIPLE_BLOCK_WRITE
                #endif
            #endif
        #endif
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
    #if !defined TEENSY_3_5 && !defined TEENSY_3_6  && !defined K66FX1M0 && !defined HEXIWEAR_K64F && !defined HEXIWEAR_KW40Z && !defined DWGB_SDCARD
      //#define ETH_INTERFACE                                            // enable Ethernet interface driver
    #endif
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
  //#define USE_IPV6                                                     // enable IPv6
      //#define USE_IPV6INV4                                             // support tunneling IPv6 ind IPv4
        #define USE_IPV6INV4_RELAY_DESTINATIONS 2                        // enable relaying to other nodes in the network - the number of destination in the IPv6 in IPv4 relay table
        #define MAX_HW_ADDRESS_LENGTH  MAC_LENGTH                        // set a variable maximum hardware address length - default is Ethernet MAC-48, 6 bytes
        #define NEIGHBOR_TABLE_ENTRIES 4                                 // the maximum entries in IPV6 neighbor table
    #define USE_IP                                                       // enable IP and ARP and all other possible tcp/ip services
    #if defined USE_IP
      //#define USE_IP_STATS                                             // enable IP statistics (counters)
        #define ARP_TABLE_ENTRIES      4                                 // the maximum entries in ARP table
        #define ARP_IGNORE_FOREIGN_ENTRIES                               // only add used addresses to ARP table
      //#define USE_ICMP                                                 // enable ICMP
        #if defined _WINDOWS
          //#define PSEUDO_LOOPBACK                                      // pseudo loop back when simulating - only for use with the simulator!! (this allows an application to send test frames to its own IP address)
        #endif
      //#define USE_ZERO_CONFIG                                          // support IPv4 link-local and zero configuration (autoIP)
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
         // #define USE_TFTP                                             // enable TFTP  - needs UDP

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
            #define WEB_PARSER_START          '\xA3'                     // this symbol is used in Web pages to instruct parsing to begin
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

            #define FILE_404_CONTENT        "<html><head><title>\xA3vN0 Loader</title></head><body bgcolor=#d0d000 text=#000000 topmargin=3 marginheight=3><center><td valign=top class=h> \
<font color=#ff0000 style=font-size:30px><b style='mso-bidi-font-weight:normal'>\xA3vN0</font> - Loader (\xA3vV0)</b></td><br><td align=left><br><br> \
<form action=webpage.html name=e1><input type=submit value=""Erase-Application"" name=e>Enter Password <input maxLength=17 size=17 name=c1 value=""\xA3ve0""> \xA3vV1</form> \
<form action=0S.bin enctype=""multipart/form-data"" method=""post""><p><input type=""file"" name=""datafile"" size=""30""><input type=""submit"" value=""Upload"" \xA3ds0></p></form> \
<br><form action=webpage.html name=e0><input type=submit value=""Mass-Erase"" name=e>Enter Password <input maxLength=17 size=17 name=c0 value=""\xA3ve0""></form> \
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

// Cryptography
//
#if defined KBOOT_SECURE_LOADER || defined SDCARD_SECURE_LOADER || defined USB_MSD_DEVICE_SECURE_LOADER
    #define CRYPTOGRAPHY                                                 // enable cryptography support - details at http://www.utasker.com/docs/uTasker/uTasker_Cryptography.pdf
#endif
  //#define CRYPTO_OPEN_SSL                                              // use OpenSSL library code (for simulation or HW when native support is not available and enabled)
  //#define CRYPTO_WOLF_SSL                                              // use wolfSSL library code (for simulation or HW when native support is not available and enabled)
    #define CRYPTO_MBEDTLS                                               // use mbedTLS library code (for simulation or HW when native support is not available and enabled)
    #define CRYPTO_AES                                                   // use AES (advanced encryption standard) cypher
      //#define MBEDTLS_AES_ROM_TABLES                                   // mbedTLS uses ROM tables for AES rather than calculating sbox and tables (costs 8k Flash, saves 8.5k RAM, loses about 70% performance)
      //#define OPENSSL_AES_FULL_LOOP_UNROLL                             // unroll loops for improved performance (costs 4k Flash, gains about 20% performance)
        #define NATIVE_AES_CAU                                           // use uTasker mmCAU (LTC) - only possible when the device has mmCAU (LTC) - simulation requires a SW library to be enabled for alternate use
          //#define AES_DISABLE_CAU                                      // force software implementation by disabling any available crypto accelerator (used mainly for testing CAU efficiency increase)
          //#define AES_DISABLE_LTC                                      // LTC has priority over CAU unless it is disabled (when device supports LTC - Low Power Trusted Cryptography)
  //#define CRYPTO_SHA                                                   // use SHA (secure hash algorithm)
        #define NATIVE_SHA256_CAU                                        // use uTasker mmCAU (LTC) - only possible when the device has mmCAU (LTC) - simulation requires a SW library to be enabled for alternate use
          //#define SHA_DISABLE_CAU                                      // force software implementation by disabling any available crypto accelerator (used mainly for testing CAU efficiency increase)
          //#define SHA_DISABLE_LTC                                      // LTC has priority over CAU unless it is disabled (when device supports LTC - Low Power Trusted Cryptography)


#define PHYSICAL_QUEUES   (NUMBER_LAN + NUMBER_SERIAL + NUMBER_I2C + NUMBER_USB) // the number of physical queues in the system


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
    #if defined USE_MODBUS
        #include "../../uTasker/MODBUS/modbus.h"
    #endif
#include "TaskConfig.h"                                                  // the specific task configuration
#include "Loader.h"                                                      // general project specific include
#if defined SUPPORT_GLCD
    #include "../../uTasker/uGLCDLIB/glcd.h"                             // LCD
#endif
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
