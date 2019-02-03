/**********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher

    ---------------------------------------------------------------------
    File:      app_hw_stm32.h
    Project:   uTasker Demonstration project
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2019
    *********************************************************************
    Application specific hardware configuration

    27.02.2012 Correct STM3210C_EVAL GLCD setup                          {1}
    15.07.2012 Modify output ports to avoid LCD interface on STM3241G_EVAL {2}
    09.09.2012 Added key and LED configuation for keypad simulator       {3}
    09.09.2012 Add SD card detection input                               {4}
    12.09.2012 Add I2C 3 and I2C multiplexing options                    {5}
    22.03.2013 Add EMBEST_BASE_BOARD support                             {6}
    03.10.2013 Adjust parameter system for 128k STM32F1xx connectivity line sizes {7}
    09.01.2014 Add pull-up input options for UART Rx and CTS inputs      {8}
    09.03.2012 Add support for magic reset Ethernet reception frame      {9}
    01.08.2015 Add WISDOM_STM32F407
    09.08.2015 Add NUCLEO_F401RE

*/

#if defined _STM32 && !defined __APP_HW_STM32__
#define __APP_HW_STM32__

#if defined _WINDOWS
    #define _SIM_PORTS fnSimPorts()
#else
    #define _SIM_PORTS
#endif

#if defined NUCLEO_L432KC                                                // STM32L432 (80MHz)
    #define SYSTICK_DIVIDE_8                                             // use HCLK/8 as SYSTICK clock
    #define CRYSTAL_FREQ        32768                                    // when there is a 32kHz crystal it can be used for the RTC, LSCO or MCO (optionally divided)

    #define MCO_CONNECTED_TO_MSI
    #define MCO_DIVIDE          16                                       // 1, 2, 4, 8 or 16 possible (defaults to /1 if not specified)

    #define USE_MSI_CLOCK                                                // use internal MSI clock source (4MHz default)
    #define MSI_CLOCK           48000000                                 // 100kHz, 200kHz, 400kHz, 800kHz, 1MHz, 2MHz, 4MHz, 8MHz, 16MHz, 32MHz, 48MHz possible
  //#define USE_HSI_CLOCK                                                // use internal HSI clock source (16MHz)
    #define DISABLE_PLL                                                  // run from clock source directly

    #define HCLK_DIVIDE         1                                        // 1,2,4,8,16,32,64,128,256 or 512
    #define PCLK1_DIVIDE        2                                        // 1,2,4,8, or 16
    #define PCLK2_DIVIDE        1                                        // 1,2,4,8, or 16

    #define PIN_COUNT           PIN_COUNT_32_PIN
    #define PACKAGE_TYPE        PACKAGE_QFN
    #define SIZE_OF_RAM         (64 * 1024)                              // 64k SRAM
    #define SIZE_OF_FLASH       (256 * 1024)                             // 256k FLASH

    #define CORE_VOLTAGE        VCORE_RANGE_1                            // normal core voltage operation
#elif defined NUCLEO_L011K4
    #define CRYSTAL_FREQ        32768                                    // when there is a 32kHz crystal it can be used for the RTC, LSCO or MCO (optionally divided)

    #define MCO_CONNECTED_TO_MSI
    #define MCO_DIVIDE          16                                       // 1, 2, 4, 8 or 16 possible (defaults to /1 if not specified)

    #define USE_MSI_CLOCK                                                // use internal MSI clock source (4.194MHz default)
    #define MSI_CLOCK           4194000                                  // 65.536kHz, 131.072kHz, 262.144kHz, 524.288kHz, 1.048MHz, 2.097MHz and 4.194MHz possible
  //#define USE_HSI_CLOCK                                                // use internal HSI clock source (16MHz)
    #define DISABLE_PLL                                                  // run from clock source directly

    #define HCLK_DIVIDE         1                                        // 1,2,4,8,16,32,64,128,256 or 512
    #define PCLK1_DIVIDE        2                                        // 1,2,4,8, or 16
    #define PCLK2_DIVIDE        1                                        // 1,2,4,8, or 16

    #define PIN_COUNT           PIN_COUNT_32_PIN
    #define PACKAGE_TYPE        PACKAGE_LQFP
    #define SIZE_OF_RAM         (2 * 1024)                               // 2k SRAM
    #define SIZE_OF_FLASH       (16 * 1024)                              // 16k FLASH

    #define CORE_VOLTAGE        VCORE_RANGE_1                            // normal core voltage operation (1.65V..1.95V)
#elif defined NUCLEO_F031K6
    #define CRYSTAL_FREQ        32768                                    // when there is a 32kHz crystal it can be used for the RTC, LSCO or MCO (optionally divided)

    #define MCO_CONNECTED_TO_HSI
    #define MCO_DIVIDE          128                                       // 1, 2, 4, 8, 16, 32, 64 or 128 possible (defaults to /1 if not specified)

    #define USE_HSI_CLOCK                                                // use internal HSI clock source (8MHz)
    #define DISABLE_PLL                                                  // run from clock source directly

    #define HCLK_DIVIDE         1                                        // 1,2,4,8,16,32,64,128,256 or 512
    #define PCLK_DIVIDE         2                                        // 1,2,4,8, or 16

    #define PIN_COUNT           PIN_COUNT_32_PIN
    #define PACKAGE_TYPE        PACKAGE_LQFP
    #define SIZE_OF_RAM         (4 * 1024)                               // 4k SRAM
    #define SIZE_OF_FLASH       (32 * 1024)                              // 32k FLASH

  //#define CORE_VOLTAGE        VCORE_RANGE_1                            // normal core voltage operation
#elif defined NUCLEO_L031K6
    #define CRYSTAL_FREQ        32768                                    // when there is a 32kHz crystal it can be used for the RTC, LSCO or MCO (optionally divided)

    #define MCO_CONNECTED_TO_MSI
    #define MCO_DIVIDE          16                                       // 1, 2, 4, 8 or 16 possible (defaults to /1 if not specified)

    #define USE_MSI_CLOCK                                                // use internal MSI clock source (4.194MHz default)
    #define MSI_CLOCK           4194000                                  // 65.536kHz, 131.072kHz, 262.144kHz, 524.288kHz, 1.048MHz, 2.097MHz and 4.194MHz possible
  //#define USE_HSI_CLOCK                                                // use internal HSI clock source (16MHz)
    #define DISABLE_PLL                                                  // run from clock source directly

    #define HCLK_DIVIDE         1                                        // 1,2,4,8,16,32,64,128,256 or 512
    #define PCLK1_DIVIDE        2                                        // 1,2,4,8, or 16
    #define PCLK2_DIVIDE        1                                        // 1,2,4,8, or 16

    #define PIN_COUNT           PIN_COUNT_32_PIN
    #define PACKAGE_TYPE        PACKAGE_LQFP
    #define SIZE_OF_RAM         (8 * 1024)                               // 8k SRAM
    #define SIZE_OF_FLASH       (32 * 1024)                              // 32k FLASH

    #define CORE_VOLTAGE        VCORE_RANGE_1                            // normal core voltage operation (1.65V..1.95V)
#elif defined ARDUINO_BLUE_PILL
    #define CRYSTAL_FREQ        8000000
  //#define DISABLE_PLL                                                  // run from clock source directly
  //#define USE_HSI_CLOCK                                                // use internal HSI clock source
    #define USE_PLL2_CLOCK                                               // use the PLL2 output as PLL input (don't use USE_HSI_CLOCK in this configuration)
    #define PLL2_INPUT_DIV      5                                        // clock input is divided by 5 to give 5MHz to the PLL2 input (range 1..16)
    #define PLL2_VCO_MUL        8                                        // the pll2 frequency is multiplied by 8 to 40MHz (range 8..14 or 16 or 20)
    #define PLL_INPUT_DIV       1                                        // 1..16 - should set the input to pll in the range 3..12MHz - not valid for HSI clock source
    #define PLL_VCO_MUL         9                                        // 4..9 where PLL out must be 18..72MHz. Also 65 is accepted as x6.5 (special case)
    #define PIN_COUNT           PIN_COUNT_48_PIN
    #define PACKAGE_TYPE        PACKAGE_LQFP
    #define SIZE_OF_RAM         (20 * 1024)                              // 20k SRAM
    #define SIZE_OF_FLASH       (64 * 1024)                              // 64k FLASH
    #define PCLK1_DIVIDE        2
    #define PCLK2_DIVIDE        1
#elif defined STM3210C_EVAL                                              // STM32F107VCT (72MHz)
    #define CRYSTAL_FREQ        25000000
  //#define DISABLE_PLL                                                  // run from clock source directly
  //#define USE_HSI_CLOCK                                                // use internal HSI clock source
    #define USE_PLL2_CLOCK                                               // use the PLL2 output as PLL input (don't use USE_HSI_CLOCK in this configuration)
    #define PLL2_INPUT_DIV      5                                        // clock input is divided by 5 to give 5MHz to the PLL2 input (range 1..16)
    #define PLL2_VCO_MUL        8                                        // the pll2 frequency is multiplied by 8 to 40MHz (range 8..14 or 16 or 20)
    #define PLL_INPUT_DIV       5                                        // 1..16 - should set the input to pll in the range 3..12MHz - not valid for HSI clock source
    #define PLL_VCO_MUL         9                                        // 4..9 where PLL out must be 18..72MHz. Also 65 is accepted as x6.5 (special case)
    #define PIN_COUNT           PIN_COUNT_100_PIN
    #define PACKAGE_TYPE        PACKAGE_LQFP
    #define SIZE_OF_RAM         (64 * 1024)                              // 64k SRAM
    #define SIZE_OF_FLASH       (256 * 1024)                             // 256k FLASH
    #define PCLK1_DIVIDE        2
    #define PCLK2_DIVIDE        1
#elif defined NUCLEO_F401RE
    #define CRYSTAL_FREQ        8000000
  //#define DISABLE_PLL                                                  // run from clock source directly
  //#define USE_HSI_CLOCK                                                // use internal HSI clock source
    #define PLL_INPUT_DIV       8                                        // 2..64 - should set the input to pll in the range 1..2MHz (with preference near to 2MHz)
    #define PLL_VCO_MUL         168                                      // 64 ..432 where VCO must be 64..432MHz
    #define PLL_POST_DIVIDE     2                                        // post divide VCO by 2, 4, 6, or 8 to get the system clock speed
    #define PIN_COUNT           PIN_COUNT_64_PIN
    #define PACKAGE_TYPE        PACKAGE_LQFP
    #define SIZE_OF_RAM         (64 * 1024)                              // 64k SRAM
  //#define SIZE_OF_CCM         (32 * 1024)                              // 64k Core Coupled Memory
    #define SIZE_OF_FLASH       (256 * 1024)                             // 256 FLASH
    #define SUPPLY_VOLTAGE      SUPPLY_2_7__3_6                          // power supply is in the range 2.7V..3.6V
    #define PCLK1_DIVIDE        4
    #define PCLK2_DIVIDE        2
    #define HCLK_DIVIDE         1
#elif defined WISDOM_STM32F407
    #define CRYSTAL_FREQ        25000000
  //#define DISABLE_PLL                                                  // run from clock source directly
  //#define USE_HSI_CLOCK                                                // use internal HSI clock source
    #define PLL_INPUT_DIV       25                                       // 2..64 - should set the input to pll in the range 1..2MHz (with preference near to 2MHz)
    #define PLL_VCO_MUL         336                                      // 64 ..432 where VCO must be 64..432MHz
    #define PLL_POST_DIVIDE     2                                        // post divide VCO by 2, 4, 6, or 8 to get the system clock speed
    #define PIN_COUNT           PIN_COUNT_176_PIN
    #define PACKAGE_TYPE        PACKAGE_BGA
    #define SIZE_OF_RAM         (128 * 1024)                             // 128k SRAM
    #define SIZE_OF_CCM         (64 * 1024)                              // 64k Core Coupled Memory
    #define SIZE_OF_FLASH       (1024 * 1024)                            // 1M FLASH
    #define SUPPLY_VOLTAGE      SUPPLY_2_7__3_6                          // power supply is in the range 2.7V..3.6V
    #define PCLK1_DIVIDE        4
    #define PCLK2_DIVIDE        2
    #define HCLK_DIVIDE         1
#elif defined STM3241G_EVAL                                              // STM32F417IGH6 (168MHz)
    #define CRYSTAL_FREQ        25000000
  //#define DISABLE_PLL                                                  // run from clock source directly
  //#define USE_HSI_CLOCK                                                // use internal HSI clock source
    #define PLL_INPUT_DIV       25                                       // 2..64 - should set the input to pll in the range 1..2MHz (with preference near to 2MHz)
    #define PLL_VCO_MUL         336                                      // 64 ..432 where VCO must be 64..432MHz
    #define PLL_POST_DIVIDE     2                                        // post divide VCO by 2, 4, 6, or 8 to get the system clock speed
    #define PIN_COUNT           PIN_COUNT_176_PIN
    #define PACKAGE_TYPE        PACKAGE_BGA
    #define SIZE_OF_RAM         (128 * 1024)                             // 128k SRAM
    #define SIZE_OF_CCM         (64 * 1024)                              // 64k Core Coupled Memory
    #define SIZE_OF_FLASH       (1024 * 1024)                            // 1M FLASH
    #define SUPPLY_VOLTAGE      SUPPLY_2_7__3_6                          // power supply is in the range 2.7V..3.6V
    #define PCLK1_DIVIDE        4
    #define PCLK2_DIVIDE        2
    #define HCLK_DIVIDE         1
#elif defined NUCLEO_F429ZI
    #define CRYSTAL_FREQ        8000000                                  // 8MHz crystal
  //#define DISABLE_PLL                                                  // run from clock source directly
  //#define USE_HSI_CLOCK                                                // use internal HSI clock source
    #define PLL_INPUT_DIV       8                                        // 2..64 - should set the input to pll in the range 1..2MHz (with preference near to 2MHz)
    #define PLL_VCO_MUL         336                                      // 192 ..432 where VCO must be 192..432MHz
    #define PLL_POST_DIVIDE     2                                        // post divide VCO by 2, 4, 6, or 8 to get the system clock speed
    #define PIN_COUNT           PIN_COUNT_144_PIN
    #define PACKAGE_TYPE        PACKAGE_LQFP
    #define SIZE_OF_RAM         (192 * 1024)                             // 192k SRAM (0x20000000)
    #define SIZE_OF_CCM         (64 * 1024)                              // 64k Core Coupled Memory (0x10000000)
    #define SIZE_OF_FLASH       (2 * 1024 * 1024)                        // 2M FLASH
    #define SUPPLY_VOLTAGE      SUPPLY_2_7__3_6                          // power supply is in the range 2.7V..3.6V
    #define HCLK_DIVIDE         1                                        // HCLK is divided by 1 (1, 2, 4, 8, 16, 64, 128 or 512 are possible) - max. 168MHz
    #define PCLK1_DIVIDE        4                                        // PCLK1 is HCLK divided by 4 (1, 2, 4, 8, or 16 are possible) - max. 42MHz
    #define PCLK2_DIVIDE        2                                        // PCLK2 is HCLK divided by 2 (1, 2, 4, 8, or 16 are possible) - max. 84MHz

  //#define FLASH_OPTION_SETTING    (FLASH_OPTCR_nWRP0 | FLASH_OPTCR_nWRP1 | FLASH_OPTCR_nWRP2 | FLASH_OPTCR_nWRP3 | FLASH_OPTCR_nWRP4 | FLASH_OPTCR_nWRP5 | FLASH_OPTCR_nWRP6 | FLASH_OPTCR_nWRP7 | FLASH_OPTCR_nWRP8 | FLASH_OPTCR_nWRP9 | FLASH_OPTCR_nWRP10 | FLASH_OPTCR_nWRP11 | FLASH_OPTCR_BOR_LEV_1 | FLASH_OPTCR_RDP_LEVEL_0 | FLASH_OPTCR_USER_nRST_STDBY | FLASH_OPTCR_USER_nRST_STOP | FLASH_OPTCR_USER_WDG_SW)
  //#define FLASH_OPTION_SETTING_1  (FLASH_OPTCR1_nWRP0 | FLASH_OPTCR1_nWRP1 | FLASH_OPTCR1_nWRP2 | FLASH_OPTCR1_nWRP3 | FLASH_OPTCR1_nWRP4 | FLASH_OPTCR1_nWRP5 | FLASH_OPTCR1_nWRP6 | FLASH_OPTCR1_nWRP7 | FLASH_OPTCR1_nWRP8 | FLASH_OPTCR1_nWRP9 | FLASH_OPTCR1_nWRP10 | FLASH_OPTCR1_nWRP11)
#elif defined NUCLEO_L476RG
    #define SYSTICK_DIVIDE_8                                             // use HCLK/8 as SYSTICK clock
    #define CRYSTAL_FREQ        32768                                    // when there is a 32kHz crystal it can be used for the RTC, LSCO or MCO (optionally divided)

    #define MCO_CONNECTED_TO_MSI
    #define MCO_DIVIDE          16                                       // 1, 2, 4, 8 or 16 possible (defaults to /1 if not specified)

    #define USE_MSI_CLOCK                                                // use internal MSI clock source (4MHz default)
    #define MSI_CLOCK           48000000                                 // 100kHz, 200kHz, 400kHz, 800kHz, 1MHz, 2MHz, 4MHz, 8MHz, 16MHz, 32MHz, 48MHz possible
  //#define USE_HSI_CLOCK                                                // use internal HSI clock source (16MHz)
    #define DISABLE_PLL                                                  // run from clock source directly

    #define HCLK_DIVIDE         1                                        // 1,2,4,8,16,32,64,128,256 or 512
    #define PCLK1_DIVIDE        2                                        // 1,2,4,8, or 16
    #define PCLK2_DIVIDE        1                                        // 1,2,4,8, or 16

    #define PIN_COUNT           PIN_COUNT_32_PIN
    #define PACKAGE_TYPE        PACKAGE_QFN
    #define SIZE_OF_RAM         (64 * 1024)                              // 64k SRAM
    #define SIZE_OF_FLASH       (256 * 1024)                             // 256k FLASH

    #define CORE_VOLTAGE        VCORE_RANGE_1                            // normal core voltage operation
#elif defined NUCLEO_L496RG
    #define SYSTICK_DIVIDE_8                                             // use HCLK/8 as SYSTICK clock
    #define CRYSTAL_FREQ        32768                                    // when there is a 32kHz crystal it can be used for the RTC, LSCO or MCO (optionally divided)

    #define MCO_CONNECTED_TO_MSI
    #define MCO_DIVIDE          16                                       // 1, 2, 4, 8 or 16 possible (defaults to /1 if not specified)

    #define USE_MSI_CLOCK                                                // use internal MSI clock source (4MHz default)
    #define MSI_CLOCK           48000000                                 // 100kHz, 200kHz, 400kHz, 800kHz, 1MHz, 2MHz, 4MHz, 8MHz, 16MHz, 32MHz, 48MHz possible
  //#define USE_HSI_CLOCK                                                // use internal HSI clock source (16MHz)
    #define DISABLE_PLL                                                  // run from clock source directly

    #define HCLK_DIVIDE         1                                        // 1,2,4,8,16,32,64,128,256 or 512
    #define PCLK1_DIVIDE        2                                        // 1,2,4,8, or 16
    #define PCLK2_DIVIDE        1                                        // 1,2,4,8, or 16

    #define PIN_COUNT           PIN_COUNT_144_PIN
    #define PACKAGE_TYPE        PACKAGE_LQFP
    #define SIZE_OF_RAM         (256 * 1024)                             // 256k SRAM
    #define SIZE_OF_RAM2        (64 * 1024)                              // 64k SRAM2 (with hardware parity check)
    #define SIZE_OF_FLASH       (1024 * 1024)                            // 1M FLASH

    #define CORE_VOLTAGE        VCORE_RANGE_1                            // normal core voltage operation
#elif defined STM32F407ZG_SK
    #define CRYSTAL_FREQ        25000000
  //#define DISABLE_PLL                                                  // run from clock source directly
  //#define USE_HSI_CLOCK                                                // use internal HSI clock source
    #define PLL_INPUT_DIV       25                                       // 2..64 - should set the input to pll in the range 1..2MHz (with preference near to 2MHz)
    #define PLL_VCO_MUL         336                                      // 192 ..432 where VCO must be 192..432MHz
    #define PLL_POST_DIVIDE     2                                        // post divide VCO by 2, 4, 6, or 8 to get the system clock speed
    #define PIN_COUNT           PIN_COUNT_144_PIN
    #define PACKAGE_TYPE        PACKAGE_LQFP
    #define SIZE_OF_RAM         (128 * 1024)                             // 128k SRAM
    #define SIZE_OF_CCM         (64 * 1024)                              // 64k Core Coupled Memory
    #define SIZE_OF_FLASH       (1024 * 1024)                            // 1M FLASH
    #define SUPPLY_VOLTAGE      SUPPLY_2_7__3_6                          // power supply is in the range 2.7V..3.6V
    #define PCLK1_DIVIDE        4
    #define PCLK2_DIVIDE        2
    #define HCLK_DIVIDE         1
#elif defined ST_MB997A_DISCOVERY                                        // STM32F407VGT6 (168MHz)
    #define CRYSTAL_FREQ        8000000
  //#define DISABLE_PLL                                                  // run from clock source directly
  //#define USE_HSI_CLOCK                                                // use internal HSI clock source
    #define PLL_INPUT_DIV       4                                        // 2..64 - should set the input to pll in the range 1..2MHz (with preference near to 2MHz)
    #define PLL_VCO_MUL         168                                      // 64 ..432 where VCO must be 64..432MHz
    #define PLL_POST_DIVIDE     2                                        // post divide VCO by 2, 4, 6, or 8 to get the system clock speed
    #define PIN_COUNT           PIN_COUNT_100_PIN
    #define PACKAGE_TYPE        PACKAGE_LQFP
    #define SIZE_OF_RAM         (128 * 1024)                             // 128k SRAM
    #define SIZE_OF_CCM         (64 * 1024)                              // 64k Core Coupled Memory
    #define SIZE_OF_FLASH       (1024 * 1024)                            // 1M FLASH
    #define SUPPLY_VOLTAGE      SUPPLY_2_7__3_6                          // power supply is in the range 2.7V..3.6V
    #define PCLK1_DIVIDE        4
    #define PCLK2_DIVIDE        2
    #define HCLK_DIVIDE         1
#elif defined STM32_P207
    #define CRYSTAL_FREQ        25000000
  //#define DISABLE_PLL                                                  // run from clock source directly
  //#define USE_HSI_CLOCK                                                // use internal HSI clock source
    #define PLL_INPUT_DIV       25                                       // 2..64 - should set the input to pll in the range 1..2MHz (with preference near to 2MHz)
    #define PLL_VCO_MUL         240                                      // 192 ..432 where VCO must be 192..432MHz
    #define PLL_POST_DIVIDE     2                                        // post divide VCO by 2, 4, 6, or 8 to get the system clock speed
    #define PIN_COUNT           PIN_COUNT_144_PIN
    #define PACKAGE_TYPE        PACKAGE_LQFP
    #define SIZE_OF_RAM         (128 * 1024)                             // 128k SRAM
    #define SIZE_OF_FLASH       (1024 * 1024)                            // 1M FLASH
    #define SUPPLY_VOLTAGE      SUPPLY_2_7__3_6                          // power supply is in the range 2.7V..3.6V
    #define PCLK1_DIVIDE        4                                        // maximum 30MHz
    #define PCLK2_DIVIDE        2                                        // maximum 60MHz
    #define HCLK_DIVIDE         1
#elif defined ST_MB913C_DISCOVERY                                        // STM32F100RB (24MHz)
    #define CRYSTAL_FREQ        8000000
  //#define DISABLE_PLL                                                  // run from clock source directly
  //#define USE_HSI_CLOCK                                                // use internal HSI clock source
    #define PLL_INPUT_DIV       2                                        // 1..16 - should set the input to pll in the range 1..24MHz (with preference near to 8MHz) - not valid for HSI clock source
    #define PLL_VCO_MUL         6                                        // 2 ..16 where PLL out must be 16..24MHz
    #define SIZE_OF_RAM         (8 * 1024)                               // 8k SRAM
    #define SIZE_OF_FLASH       (128 * 1024)                             // 128k FLASH
    #define PIN_COUNT           PIN_COUNT_64_PIN
    #define PACKAGE_TYPE        PACKAGE_LQFP
    #define STM32F100RB                                                  // exact processor type
    #define PCLK1_DIVIDE        2
    #define PCLK2_DIVIDE        1
#elif defined STM32F746G_DISCO                                           // STM32F746NGH6 (216MHz)
    #define CRYSTAL_FREQ        25000000
  //#define DISABLE_PLL                                                  // run from clock source directly
  //#define USE_HSI_CLOCK                                                // use internal HSI clock source
    #define PLL_INPUT_DIV       25                                       // 2..64 - should set the input to pll in the range 1..2MHz (with preference near to 2MHz)
    #define PLL_VCO_MUL         336                                      // 64 ..432 where VCO must be 64..432MHz
    #define PLL_POST_DIVIDE     2                                        // post divide VCO by 2, 4, 6, or 8 to get the system clock speed
    #define PIN_COUNT           PIN_COUNT_216_PIN
    #define PACKAGE_TYPE        PACKAGE_BGA
    #define SIZE_OF_RAM         (320 * 1024)                             // 320k SRAM (DTCM + SRAM1 + SRAM2)
    #define SIZE_OF_FLASH       (1024 * 1024)                            // 1M FLASH
    #define SUPPLY_VOLTAGE      SUPPLY_2_7__3_6                          // power supply is in the range 2.7V..3.6V
    #define PCLK1_DIVIDE        4
    #define PCLK2_DIVIDE        2
    #define HCLK_DIVIDE         1
#else
                                                                         // other configurations can be added here
#endif



#include "../../Hardware/STM32/STM32.h"


// F4 needs 4..24MHz external clock
// F4 starts with internal 16MHz 1% RC oscillator (F1 and F2 2%)

#define CONFIGURE_WATCHDOG()       DBGMCU_CR = DBG_IWDGSTOP; IWDG_PR = IWDG_PR_PRESCALE_32; while (IWDG_SR != 0) {} IWDG_RLR = 0x9c4; while (IWDG_SR != 0) {} // 40kHz/32 * 2500 = 2s and wait for change to complete (set WATCHDOG_DISABLED to disable)

// Memory set up for this target
//
#define BOOT_CODE_SIZE             0x800

#define APP_START_ADDRESS          (unsigned char *)(FLASH_START_ADDRESS + BOOT_CODE_SIZE) // from start of app code
#define APP_END_ADDRESS            (unsigned char *)(FLASH_START_ADDRESS + 0x18000) // to the file system and parameters

                                                                         //
#if defined SPI_FLASH_ST                                                 // ST SPI FLASH used
  //#define SPI_FLASH_STM25P05                                           // the available ST chips
  //#define SPI_FLASH_STM25P10
  //#define SPI_FLASH_STM25P20
  //#define SPI_FLASH_STM25P40
  //#define SPI_FLASH_STM25P80                                               
  //#define SPI_FLASH_STM25P16                                               
  //#define SPI_FLASH_STM25P32                                               
    #define SPI_FLASH_STM25P64
  //#define SPI_FLASH_STM25P128

    #if defined SPI_FLASH_STM25P05                                       // 64k
        #define SPI_FLASH_PAGES             (1 * 256)                          
    #elif defined SPI_FLASH_STM25P10                                     // 128k
        #define SPI_FLASH_PAGES             (2 * 256)
    #elif defined SPI_FLASH_STM25P20                                     // 256k
        #define SPI_FLASH_PAGES             (4 * 256)
    #elif defined SPI_FLASH_STM25P40                                     // 512k
        #define SPI_FLASH_PAGES             (8 * 256)
    #elif defined SPI_FLASH_STM25P80                                     // 1M
        #define SPI_FLASH_PAGES             (16 * 256)
    #elif defined SPI_FLASH_STM25P16                                     // 2M
        #define SPI_FLASH_PAGES             (32  *256)
    #elif defined SPI_FLASH_STM25P32                                     // 4M
        #define SPI_FLASH_PAGES             (64 * 256)
    #elif defined SPI_FLASH_STM25P64                                     // 8M
        #define SPI_FLASH_PAGES             (128 * 256)
    #elif defined SPI_FLASH_STM25P128                                    // 16M
        #define SPI_FLASH_PAGES             (256 * 256)
    #endif

    #define SPI_FLASH_PAGE_LENGTH 256
    #if defined SPI_DATA_FLASH
        #define SPI_FLASH_SECTOR_LENGTH (16 * SPI_FLASH_PAGE_LENGTH)     // sub-sector size of data FLASH
    #else
        #define SPI_FLASH_SECTOR_LENGTH (256 * SPI_FLASH_PAGE_LENGTH)    // sector size of code FLASH
    #endif
    #define SPI_FLASH_BLOCK_LENGTH  SPI_FLASH_SECTOR_LENGTH
#elif defined SPI_FLASH_SST25
  //#define SPI_FLASH_SST25VF010A                                        // the supported SST chips
  //#define SPI_FLASH_SST25LF020A
  //#define SPI_FLASH_SST25LF040A
  //#define SPI_FLASH_SST25VF040B
  //#define SPI_FLASH_SST25VF080B
  //#define SPI_FLASH_SST25VF016B
    #define SPI_FLASH_SST25VF032B

    #if defined SPI_FLASH_SST25VF010A                                    // 1Mbit (128k)
        #define SST25_A_VERSION
        #define SPI_FLASH_PAGES             (32)
    #elif defined SPI_FLASH_SST25LF020A                                  // 2Mbit (256k)
        #define SST25_A_VERSION
        #define SPI_FLASH_PAGES             (64)
    #elif defined SPI_FLASH_SST25LF040A                                  // 4Mbit (512k)
        #define SST25_A_VERSION
        #define SPI_FLASH_PAGES             (128)
    #elif defined SPI_FLASH_SST25VF040B                                  // 4Mbit (512k)
        #define SPI_FLASH_PAGES             (128)
    #elif defined SPI_FLASH_SST25VF080B                                  // 8Mbit (1M)
        #define SPI_FLASH_PAGES             (256)
    #elif defined SPI_FLASH_SST25VF016B                                  // 16Mbit (2M)
        #define SPI_FLASH_PAGES             (512)
    #elif defined SPI_FLASH_SST25VF032B                                  // 32Mbit (4M)
        #define SPI_FLASH_PAGES             (1024)
    #endif

    #define SPI_FLASH_SUB_SECTOR_LENGTH  (4 * 1024)                      // sub-sector size of SPI FLASH
    #define SPI_FLASH_HALF_SECTOR_LENGTH (32 * 1024)                     // half-sector size of SPI FLASH
    #define SPI_FLASH_SECTOR_LENGTH      (64 * 1024)                     // sector size of SPI FLASH (not available on A-versions)

    #define SPI_FLASH_PAGE_LENGTH        SPI_FLASH_SUB_SECTOR_LENGTH     // for compatibility - smallest erasable block
    #define SPI_FLASH_BLOCK_LENGTH       SPI_FLASH_SUB_SECTOR_LENGTH     // for compatibility - file system granularity
#else
  //#define SPI_FLASH_AT45DB011                                          // define the ATMEL type used here
  //#define SPI_FLASH_AT45DB021
  //#define SPI_FLASH_AT45DB041
  //#define SPI_FLASH_AT45DB081
  //#define SPI_FLASH_AT45DB161
    #define SPI_FLASH_AT45DB321
  //#define SPI_FLASH_AT45DB642
    #define SPI_FLASH_AT45XXXXD_TYPE                                     // specify that a D-type rather than a B type is being used

    #if defined SPI_FLASH_AT45DB642                                      // define whether used in power of 2 mode or not
        #define SPI_FLASH_PAGE_LENGTH 1056                               // standard page size (B-device only allows 1056)
      //#define SPI_FLASH_PAGE_LENGTH 1024                               // size when power of 2 mode selected (only possible on D-device)
        #define SPI_FLASH_PAGES             (8 * 1024)                     // 8Meg part {23}
    #elif defined SPI_FLASH_AT45DB321 || defined SPI_FLASH_AT45DB161
        #define SPI_FLASH_PAGE_LENGTH 528                                // standard page size (B-device only allows 528)
      //#define SPI_FLASH_PAGE_LENGTH 512                                // size when power of 2 mode selected (only possible on D-device)
        #if defined SPI_FLASH_AT45DB161
            #define SPI_FLASH_PAGES         (4 * 1024)                   // 2Meg part {23}
        #else
            #define SPI_FLASH_PAGES         (8 * 1024)                   // 4Meg part {23}
        #endif
    #else
        #define SPI_FLASH_PAGE_LENGTH 264                                // standard page size (B-device only allows 264)
      //#define SPI_FLASH_PAGE_LENGTH 256                                // size when power of 2 mode selected (only possible on D-device)
        #if defined SPI_FLASH_AT45DB011
            #define SPI_FLASH_PAGES         (512)                        // 128k part
        #elif defined SPI_FLASH_AT45DB021
            #define SPI_FLASH_PAGES         (1024)                       // 256k part
        #elif defined SPI_FLASH_AT45DB041
            #define SPI_FLASH_PAGES         (2 * 1024)                   // 512k part
        #elif defined SPI_FLASH_AT45DB081
            #define SPI_FLASH_PAGES         (4 * 1024)                   // 1Meg part
        #endif
    #endif

    #define SPI_FLASH_BLOCK_LENGTH (8 * SPI_FLASH_PAGE_LENGTH)           // block size - a block can be deleted
    #define SPI_FLASH_SECTOR_LENGTH (64 * 4 * SPI_FLASH_PAGE_LENGTH)     // exception sector 0a is 2k and sector 0b is 62k
#endif

#if defined STM3241G_EVAL || defined WISDOM_STM32F407 || defined NUCLEO_F401RE || defined STM32F746G_DISCO || defined NUCLEO_F429ZI
    // SPI FLASH system setup
    //
    //#define SPI_FLASH_MULTIPLE_CHIPS                                   // activate when multiple physical chips are used
    #define CS0_LINE                            PORTA_BIT4               // CS0 line used when SPI FLASH is enabled
    #define CS1_LINE                                                     // CS1 line used when extended SPI FLASH is enabled
    #define CS2_LINE                                                     // CS2 line used when extended SPI FLASH is enabled
    #define CS3_LINE                                                     // CS3 line used when extended SPI FLASH is enabled

    #define SPI_CS0_PORT                        GPIOA_ODR                // for simulator
    #define SPI_TX_BYTE                         SPI3_DR                  // for simulator
    #define SPI_RX_BYTE                         SPI3_DR                  // for simulator

    #define SSPDR_X                             SPI3_DR
    #define SSPSR_X                             SPI3_SR

    #define __ASSERT_CS(cs_line)                _CLEARBITS(A, cs_line)
    #define __NEGATE_CS(cs_line)                _SETBITS(A, cs_line)

    // SPI 3 used for SPI Flash interface - speed set to 22MHz (PCLK1 168MHz/4)
    //
    #define POWER_UP_SPI_FLASH_INTERFACE()      POWER_UP(APB1, (RCC_APB1ENR_SPI3EN))
    #define POWER_DOWN_SPI_FLASH_INTERFACE()    POWER_DOWN(APB1, (RCC_APB1ENR_SPI3EN))
    #define CONFIGURE_SPI_FLASH_INTERFACE()     _CONFIG_DRIVE_PORT_OUTPUT_VALUE(A, CS0_LINE, (OUTPUT_FAST | OUTPUT_PUSH_PULL), CS0_LINE); \
    _CONFIG_PERIPHERAL_OUTPUT(B, (PERIPHERAL_SPI3_I2S3ext), (SPI3_CLK_B_3 | SPI3_MOSI_B_5), (OUTPUT_FAST | OUTPUT_PUSH_PULL | INPUT_PULL_DOWN)); \
    _CONFIG_PERIPHERAL_INPUT(B,  (PERIPHERAL_SPI3_I2S3ext), (SPI3_MISO_B_4), INPUT_PULL_DOWN); \
    _RESET_CYCLE_PERIPHERAL(APB1, RCC_APB1RSTR_SPI3RST); \
    SPI3_CR1 = (SPICR1_BR_PCLK2_DIV2 | SPICR1_MSTR | SPICR1_SSI | SPICR1_CPOL | SPICR1_CPHA | SPICR1_SSM); \
    SPI3_I2SCFGR = 0; \
    SPI3_CR1 = (SPICR1_SPE | SPICR1_BR_PCLK2_DIV2 | SPICR1_MSTR | SPICR1_SSI | SPICR1_CPOL | SPICR1_CPHA | SPICR1_SSM)
#endif

#define SPI_DATA_FLASH_0_SIZE       (SPI_FLASH_PAGES*SPI_FLASH_PAGE_LENGTH) 
#define SPI_DATA_FLASH_1_SIZE       SPI_DATA_FLASH_0_SIZE 
#define SPI_DATA_FLASH_2_SIZE       SPI_DATA_FLASH_0_SIZE
#if defined SPI_FLASH_MULTIPLE_CHIPS
    #define SPI_FLASH_DEVICE_COUNT  3                                    // only define CS_LINE for this amount of devices
    #if SPI_FLASH_DEVICE_COUNT > 2
        #define SPI_DATA_FLASH_SIZE   (SPI_DATA_FLASH_0_SIZE + SPI_DATA_FLASH_1_SIZE + SPI_DATA_FLASH_2_SIZE)
    #else
        #define SPI_DATA_FLASH_SIZE   (SPI_DATA_FLASH_0_SIZE + SPI_DATA_FLASH_1_SIZE)
    #endif
#else
    #define SPI_DATA_FLASH_SIZE       SPI_DATA_FLASH_0_SIZE                                  
#endif


#define SPI_FLASH_START        (FLASH_START_ADDRESS + SIZE_OF_FLASH)     // SPI FLASH starts immediately after FLASH

#define SW_UPLOAD_FILE()       (*ucIp_Data == 'H') && (*(ucIp_Data+1) == 'S') && (*(ucIp_Data+2) == '.')  && (fnSPI_Flash_available() != 0)

#define SET_SPI_FLASH_MODE()
#define REMOVE_SPI_FLASH_MODE()

#if defined SDCARD_SUPPORT
    #define SDCARD_SIM_SIZE   SDCARD_SIZE_2G                             // the size of SD card when simulating
  //#define _NO_SD_CARD_INSERTED                                         // simulate no SD card inserted
    #define T_CHECK_CARD_REMOVAL    ((DELAY_LIMIT)(SEC * 10))            // if the card has no detection switch it can be polled to detect removal
  //#define SDCARD_DETECT_INPUT_POLL                                     // {4} use card detect switch for detection polling (use together with T_CHECK_CARD_REMOVAL)
  //#define SDCARD_DETECT_INPUT_INTERRUPT                                // {4} use card detect switch for detection by interrupt (T_CHECK_CARD_REMOVAL and SDCARD_DETECT_INPUT_POLL should be disabled)

    #if defined STM3241G_EVAL || defined ST_MB997A_DISCOVERY || defined WISDOM_STM32F407 || defined NUCLEO_F401RE || defined STM32F746G_DISCO
        #define SD_CONTROLLER_AVAILABLE                                  // use SDIO rather than SPI (necessary on STM3240G-EVAL board)

        #if defined SD_CONTROLLER_AVAILABLE
            #define RING_POST_DIVIDE 8                                   // control the speed of the SDIO clock (ring clock) [VCC clock / 2..15] when USB is not used - if USB is enabled it will be 48MHz
            #define SET_SD_CS_HIGH()                                     // dummy in SDIO mode
            #define SET_SD_CS_LOW()                                      // dummy in SDIO mode
            #define POWER_UP_SD_CARD()                   SDIO_POWER = SDIO_POWER_POWER_ON; SDIO_CLKCR |= (SDIO_POWER_CLKEN);
            #if ((PLL_VCO_FREQUENCY/PLL_Q_VALUE)/20000000) >= 2
                #define SET_SPI_SD_INTERFACE_FULL_SPEED()    SDIO_CLKCR = (SDIO_POWER_HWFC_EN | SDIO_POWER_POSEDGE | SDIO_POWER_BUS_4BIT | SDIO_POWER_PWRSAV | SDIO_POWER_CLKEN | (((PLL_VCO_FREQUENCY/PLL_Q_VALUE)/20000000) - 2)); // set about 20MHz
            #else
                #define SET_SPI_SD_INTERFACE_FULL_SPEED()    SDIO_CLKCR = (SDIO_POWER_HWFC_EN | SDIO_POWER_POSEDGE | SDIO_POWER_BUS_4BIT | SDIO_POWER_PWRSAV | SDIO_POWER_CLKEN | 0); // set highest possible speed
            #endif
        #else
            // Configure to suit SD card SPI mode at between 100k and 400k
            //
            #define SDCARD_CS                  PORTC_BIT11
            #define SDCARD_MISO                PORTB_BIT4
            #define SDCARD_MOSI                PORTB_BIT5
            #define SDCARD_CLK                 PORTB_BIT3
            #define INITIALISE_SPI_SD_INTERFACE() _CONFIG_PORT_OUTPUT(C, SDCARD_CS, (OUTPUT_FAST | OUTPUT_PUSH_PULL)); _SETBITS(C, SDCARD_CS); \
                    POWER_UP(APB1, RCC_APB1ENR_SPI3EN); \
                    _CONFIG_PERIPHERAL_INPUT(B, (PERIPHERAL_SPI3_I2S3ext), (SDCARD_MISO), (OUTPUT_PUSH_PULL | OUTPUT_MEDIUM)); \
                    _CONFIG_PERIPHERAL_OUTPUT(B, (PERIPHERAL_SPI3_I2S3ext), (SDCARD_CLK | SDCARD_MOSI), (OUTPUT_PUSH_PULL | OUTPUT_MEDIUM)); \
                    RCC_APB1RSTR |= RCC_APB1ENR_SPI3RST; RCC_APB1RSTR &= ~RCC_APB1ENR_SPI3RST; \
                    SPI3_CR1 = (SPICR1_BR_PCLK2_DIV128 | SPICR1_MSTR | SPICR1_SSI | SPICR1_CPOL | SPICR1_CPHA | SPICR1_SSM); \
                    SPI3_I2SCFGR = 0; \
                    SPI3_CR1 = (SPICR1_SPE | SPICR1_BR_PCLK2_DIV128 | SPICR1_MSTR | SPICR1_SSI | SPICR1_CPOL | SPICR1_CPHA | SPICR1_SSM);

            #define SHARE_SPI                                            // ensure that LCD operations are terminated to avoid conflicts
            #define POWER_UP_SD_CARD()                                   // apply power to the SD card if appropriate
            #define ENABLE_SPI_SD_OPERATION()
            // Set maximum speed
            //
            #define SET_SPI_SD_INTERFACE_FULL_SPEED() SPI3_CR1 = (SPICR1_SPE | SPICR1_BR_PCLK2_DIV2 | SPICR1_MSTR | SPICR1_SSI | SPICR1_CPOL | SPICR1_CPHA | SPICR1_SSM)
            #if defined _WINDOWS
                #define WRITE_SPI_CMD(byte)      SPI3_DR = (unsigned short)byte; SPI3_DR = _fnSimSD_write((unsigned char)byte)
                #define WAIT_TRANSMISSON_END()   while (!(SPI3_SR & SPISR_TXE)) { SPI3_SR |= SPISR_TXE;} \
                                                 while (SPI3_SR & SPISR_BSY) {SPI3_SR &= ~SPISR_BSY;}
            #else
                #define WRITE_SPI_CMD(byte)      SPI3_DR = (unsigned short)byte
                #define WAIT_TRANSMISSON_END()   while (!(SPI3_SR & SPISR_TXE)) {} \
                                                 while (SPI3_SR & SPISR_BSY)
            #endif
            #define READ_SPI_DATA()              (unsigned char)SPI3_DR

            #define SET_SD_DI_CS_HIGH()           _CONFIG_PORT_OUTPUT(C, SDCARD_MISO, (OUTPUT_FAST | OUTPUT_PUSH_PULL)); _SETBITS(C, SDCARD_MISO); _SETBITS(C, SDCARD_CS)     // force DI and CS lines high ready for the initialisation sequence
            #define SET_SD_CARD_MODE()            _CONFIG_PORT_INPUT(C, (SDCARD_MISO), (INPUT_PULL_UP | PULLUP_BIT11));
            #define SET_SD_CS_LOW()               _CLEARBITS(C, SDCARD_CS) // assert the CS line of the SD card to be read
            #define SET_SD_CS_HIGH()              _SETBITS(C, SDCARD_CS) // negate the CS line of the SD card to be read
        #endif        
        #define POWER_DOWN_SD_CARD()                                     // remove power from SD card interface

        #define GET_SDCARD_WP_STATE() 0                                  // write protection disabled (change to read switch if required)

        #if defined EMBEST_BASE_BOARD
            #define CONFIGURE_SDCARD_DETECT_INPUT() _CONFIG_PORT_INPUT(B, PORTB_BIT15, (INPUT_PULL_UP | PULLUP_BIT13))
            #define SDCARD_DETECTION()              (_READ_PORT_MASK(B, PORTB_BIT15) == 0)

            #define SDCARD_SINGLE_EDGE_INTERRUPT                         // port interrupts are not possible on both edges at the same time
            #define SDCARD_DETECT_PORT              PORTB
            #define SDCARD_DETECT_PIN               15
        #else
            #define CONFIGURE_SDCARD_DETECT_INPUT() _CONFIG_PORT_INPUT(H, PORTH_BIT13, (INPUT_PULL_UP | PULLUP_BIT13))
            #define SDCARD_DETECTION()              (_READ_PORT_MASK(H, PORTH_BIT13) == 0)

            #define SDCARD_SINGLE_EDGE_INTERRUPT                         // port interrupts are not possible on both edges at the same time
            #define SDCARD_DETECT_PORT              PORTH
            #define SDCARD_DETECT_PIN               13
        #endif
        #define PRIORITY_SDCARD_DETECT_PORT_INT 7
    #elif defined STM3210C_EVAL || defined ST_MB913C_DISCOVERY || defined ARDUINO_BLUE_PILL
        // Configure to suit SD card SPI mode at between 100k and 400k
        //
        #define SPI_CS1_0             PORTA_BIT4
        #define INITIALISE_SPI_SD_INTERFACE() _CONFIG_PORT_OUTPUT(B, PORTB_BIT2, (OUTPUT_FAST | OUTPUT_PUSH_PULL)); _SETBITS(B, PORTB_BIT2); \
                _CONFIG_PORT_OUTPUT(A, SPI_CS1_0, (OUTPUT_FAST | OUTPUT_PUSH_PULL)); _SETBITS(A, SPI_CS1_0); \
                RCC_APB2ENR |= (RCC_APB2ENR_AFIOEN | RCC_APB2ENR_IOPCEN); AFIO_MAPR |= SPI3_REMAP; RCC_APB1ENR |= (RCC_APB1ENR_SPI3EN); \
                _CONFIG_PORT_OUTPUT(C, (PORTC_BIT10 | PORTC_BIT12), (ALTERNATIVE_FUNCTION | OUTPUT_FAST | OUTPUT_PUSH_PULL)); \
                _CONFIG_PORT_INPUT(C, (PORTC_BIT11), (INPUT_PULL_UP | PULLUP_BIT11)); \
                RCC_APB1RSTR |= RCC_APB1RSTR_SPI3RST; RCC_APB1RSTR &= ~RCC_APB1RSTR_SPI3RST; \
                SPI3_CR1 = (SPICR1_BR_PCLK2_DIV128 | SPICR1_MSTR | SPICR1_SSI | SPICR1_CPOL | SPICR1_CPHA | SPICR1_SSM); \
                SPI3_I2SCFGR = 0; \
                SPI3_CR1 = (SPICR1_SPE | SPICR1_BR_PCLK2_DIV128 | SPICR1_MSTR | SPICR1_SSI | SPICR1_CPOL | SPICR1_CPHA | SPICR1_SSM);

        #define SHARE_SPI                                                // ensure that LCD operations are terminated to avoid conflicts

        #define ENABLE_SPI_SD_OPERATION()
        // Set maximum speed
        //
        #define SET_SPI_SD_INTERFACE_FULL_SPEED() SPI3_CR1 = (SPICR1_SPE | SPICR1_BR_PCLK2_DIV2 | SPICR1_MSTR | SPICR1_SSI | SPICR1_CPOL | SPICR1_CPHA | SPICR1_SSM)
        #if defined _WINDOWS
            #define WRITE_SPI_CMD(byte)      SPI3_DR = (unsigned short)byte; SPI3_DR = _fnSimSD_write((unsigned char)byte)
            #define WAIT_TRANSMISSON_END()   while (!(SPI3_SR & SPISR_TXE)) { SPI3_SR |= SPISR_TXE;} \
                                             while (SPI3_SR & SPISR_BSY) {SPI3_SR &= ~SPISR_BSY;}
        #else
            #define WRITE_SPI_CMD(byte)      SPI3_DR = (unsigned short)byte
            #define WAIT_TRANSMISSON_END()   while (!(SPI3_SR & SPISR_TXE)) {} \
                                             while (SPI3_SR & SPISR_BSY)
        #endif
        #define READ_SPI_DATA()              (unsigned char)SPI3_DR

        #define POWER_UP_SD_CARD()                                       // apply power to the SD card if appropriate
        #define POWER_DOWN_SD_CARD()                                     // remove power from SD card interface
        #define SET_SD_DI_CS_HIGH()           _CONFIG_PORT_OUTPUT(C, PORTC_BIT11, (OUTPUT_FAST | OUTPUT_PUSH_PULL)); _SETBITS(C, PORTC_BIT11); _SETBITS(A, SPI_CS1_0)     // force DI and CS lines high ready for the initialisation sequence
        #define SET_SD_CARD_MODE()            _CONFIG_PORT_INPUT(C, (PORTC_BIT11), (INPUT_PULL_UP | PULLUP_BIT11));
        #define SET_SD_CS_LOW()               _CLEARBITS(A, SPI_CS1_0)   // assert the CS line of the SD card to be read
        #define SET_SD_CS_HIGH()              _SETBITS(A, SPI_CS1_0)     // negate the CS line of the SD card to be read
        #define GET_SDCARD_WP_STATE() 0                                  // write protection disabled (change to read switch if required)

        #define CONFIGURE_SDCARD_DETECT_INPUT() _CONFIG_PORT_INPUT(E, PORTE_BIT0, (INPUT_PULL_UP | PULLUP_BIT0))
        #define SDCARD_DETECTION()           (!_READ_PORT_MASK(E, PORTE_BIT0))
    #endif
#endif

// FLASH based File System setup
//
#if defined FLASH_FILE_SYSTEM
    #if defined SPI_FILE_SYSTEM
        #define uFILE_START (SPI_FLASH_START)                            // FLASH location end of internal FLASH
        #if defined _STM32F2XX || defined _STM32F4XX || defined _STM32F7XX
            #define PARAMETER_BLOCK_START (FLASH_START_ADDRESS + FLASH_GRANULARITY_BOOT) // FLASH location at 16k start in boot block
        #else                                                            // this is a test setup for external SPI FLASH, with the parameters at the end of internal FLASH
            #define PARAMETER_BLOCK_START (SPI_FLASH_START - PAR_BLOCK_SIZE) // FLASH location at 2 parameter blocks short of end of internal FLASH
        #endif
        #if FILE_GRANULARITY < 20000
            #if SPI_DATA_FLASH_SIZE < (64 * FILE_GRANULARITY)            // allow testing smallest SPI FLASH
                #define FILE_SYSTEM_SIZE  SPI_DATA_FLASH_SIZE
            #else
                #define FILE_SYSTEM_SIZE (124 * FILE_GRANULARITY)        // 512k reserved for file system (assuming 4k file size)
            #endif
        #else
            #define FILE_SYSTEM_SIZE (124 * FILE_GRANULARITY)            // 512k reserved for file system (assuming 4k file size)
        #endif
    #else
        #if (SIZE_OF_FLASH <= (64 * 1024))
            #define FILE_GRANULARITY (1 * FLASH_GRANULARITY)             // each file a multiple of page size
            #define FILE_SYSTEM_SIZE (1 * FILE_GRANULARITY)              // one page reserved for file system
            #define PARAMETER_BLOCK_START (FLASH_START_ADDRESS + SIZE_OF_FLASH - (2 * FLASH_GRANULARITY)) // FLASH location of parameter system
            #define uFILE_START (PARAMETER_BLOCK_START - FILE_SYSTEM_SIZE) // FLASH location of uFileSystem
        #elif (SIZE_OF_FLASH == (128 * 1024))
            #if defined _CONNECTIVITY_LINE                               // {7}
                #define PARAMETER_BLOCK_START (FLASH_START_ADDRESS + 0x10000)// FLASH location at 64k start
                #define uFILE_START (FLASH_START_ADDRESS + 0x11000)      // FLASH location at 68k start

                #define FILE_GRANULARITY (1 * FLASH_GRANULARITY)         // each file a multiple of 2k
                #define FILE_SYSTEM_SIZE (30 * FILE_GRANULARITY)         // 60k reserved for file system
            #else
                #define PARAMETER_BLOCK_START (FLASH_START_ADDRESS + 0x10000)// FLASH location at 64k start
                #define uFILE_START (FLASH_START_ADDRESS + 0x10800)      // FLASH location at 66k start

                #define FILE_GRANULARITY (2 * FLASH_GRANULARITY)         // each file a multiple of 2k
                #define FILE_SYSTEM_SIZE (31 * FILE_GRANULARITY)         // 62k reserved for file system
            #endif
        #else
            #if (defined _STM32F7XX) && (defined FLASH_FILE_SYSTEM && !defined SPI_FILE_SYSTEM) // devices with 256k Flash granularity
                #define PARAMETER_BLOCK_START (FLASH_START_ADDRESS + FLASH_GRANULARITY_BOOT) // FLASH location at 32k start in boot block
                #define uFILE_START (FLASH_START_ADDRESS + 0x40000)      // FLASH location at 256k start

                #define FILE_GRANULARITY (1 * FLASH_GRANULARITY)         // each file a multiple of 256k
                #define FILE_SYSTEM_SIZE (1 * FILE_GRANULARITY)          // 256k reserved for file system
                #define SUB_FILE_SIZE    (FILE_GRANULARITY/64)           // 4k sub file sizes
            #elif (defined _STM32F4XX || defined _STM32F2XX) && (defined FLASH_FILE_SYSTEM && !defined SPI_FILE_SYSTEM) // devices with 128k Flash granularity
                #define PARAMETER_BLOCK_START (FLASH_START_ADDRESS + FLASH_GRANULARITY_BOOT) // FLASH location at 16k start in boot block
                #define uFILE_START (FLASH_START_ADDRESS + 0x40000)      // FLASH location at 256k start

                #define FILE_GRANULARITY (1 * FLASH_GRANULARITY)         // each file a multiple of 128k
                #define FILE_SYSTEM_SIZE (2 * FILE_GRANULARITY)          // 256k reserved for file system
                #define SUB_FILE_SIZE    (FILE_GRANULARITY/32)           // 4k sub file sizes
            #else
                #define PARAMETER_BLOCK_START (FLASH_START_ADDRESS + 0x22000) // FLASH location at 136k start
                #define uFILE_START (FLASH_START_ADDRESS + 0x23000)      // FLASH location at 140k start

                #define FILE_GRANULARITY (2 * FLASH_GRANULARITY)         // each file a multiple of 4k
                #define FILE_SYSTEM_SIZE (29 * FILE_GRANULARITY)         // 116k reserved for file system
            #endif
        #endif
    #endif
#endif

#if defined USE_PARAMETER_BLOCK
    #if defined _STM32F2XX || defined _STM32F4XX || defined _STM32F7XX
        #define PARAMETER_BLOCK_SIZE    (FLASH_GRANULARITY_BOOT)         // use boot block size
    #else
        #define PARAMETER_BLOCK_SIZE    (FLASH_GRANULARITY)              // use page size
    #endif
    #if defined USE_PAR_SWAP_BLOCK
        #define PAR_BLOCK_SIZE      (2 * PARAMETER_BLOCK_SIZE)
    #else
        #define PAR_BLOCK_SIZE      (1 * PARAMETER_BLOCK_SIZE)
    #endif
#else
    #define PAR_BLOCK_SIZE          (0)
#endif


#define USB_DOWNLOAD_FILE           "0"                                  // USB firmware uploads are saved here

#define USB_BUS_SWITCH                                                   // control USB bus power using USB0EPE and USB0PFLT


//#define SUPPORT_RTC
#define RTC_OSCILLATOR_STABILISATION_DELAY   (2 * SEC)                   // typical RTC oscillator stabilisation time when first enabled


// Serial interfaces
//
#if defined SERIAL_INTERFACE
    #define WELCOME_MESSAGE_UART   "\r\n\nHello, world... STM32\r\n"
    #define NUMBER_EXTERNAL_SERIAL 0
    #define NUMBER_SERIAL  CHIP_HAS_UARTS                                // the number of physical queue needed for serial interface(s)
    #define SIM_COM_EXTENDED                                             // COM ports defined from 1..255
    #define SERIAL_PORT_0  4                                             // if we open UART channel 0 we simulate using this com port on the PC (0 for no mapping)
    #define SERIAL_PORT_1  6                                             // if we open UART channel 1 we simulate using this com port on the PC
    #define SERIAL_PORT_2  8                                             // if we open UART channel 2 we simulate using this com port on the PC
    #define SERIAL_PORT_3  10                                            // if we open UART channel 3 we simulate using this com port on the PC
    #define SERIAL_PORT_4  12                                            // if we open UART channel 4 we simulate using this com port on the PC
    #define SERIAL_PORT_5  4                                             // if we open UART channel 5 we simulate using this com port on the PC
    #define SERIAL_PORT_6  16                                            // if we open UART channel 6 we simulate using this com port on the PC
    #define SERIAL_PORT_7  18                                            // if we open UART channel 7 we simulate using this com port on the PC

  //#define SERIAL_SUPPORT_DMA                                           // enable UART DMA support
  //#define SUPPORT_HW_FLOW                                              // enable hardware flow control support

    #if defined ST_MB913C_DISCOVERY || defined NUCLEO_F429ZI || defined ARDUINO_BLUE_PILL
        #define DEMO_UART    2                                           // use UART channel 2 (USART 3 since ST USARTs count from 1)
    #elif defined NUCLEO_L496RG
        #define DEMO_UART    5                                           // use LPUART1 (channel 5) [0 = USART1, 1 = USART2, 2= USART3, 3 = UART4, 4 = UART5, 5 = LPUART1]
        #define LPUART_REMAP_G                                           // STLink VCOM
    #elif defined WISDOM_STM32F407 || defined STM32F746G_DISCO || defined NUCLEO_F031K6
        #define DEMO_UART    0                                           // use UART channel 0 (USART 1 since ST USARTs count from 1)
    #elif defined STM3241G_EVAL || defined STM32_P207 || defined STM32F407ZG_SK 
        #define DEMO_UART    2                                           // use UART channel 2 (USART 3 since ST USARTs count from 1) - the STM3241G can't use USART 4 and SD card at the same time so needs a modification for this
      //#define DEMO_UART    3                                           // use UART channel 3 (USART 4 since ST USARTs count from 1)
    #elif (defined ST_MB997A_DISCOVERY && defined EMBEST_BASE_BOARD)     // {6}
        #define DEMO_UART    5                                           // use UART channel 5 (USART 6 since ST USARTs count from 1)
    #else
        #define DEMO_UART    1                                           // use UART channel 1 (USART 2 since ST USARTs count from 1)
    #endif
  //#define PPP_UART         1                                           // use UART 1 for PPP
    #define MODBUS_UART_0    0
    #define MODBUS_UART_1    1
    #define MODBUS_UART_2    3
    #define MODBUS_UART_3    4
    #define MODBUS_UART_4    5
    #define MODBUS_UART_5    2
    #if defined NUCLEO_L011K4                                            // 2k SRAM device
        #define TX_BUFFER_SIZE   (200)                                   // the size of RS232 output buffer
        #define RX_BUFFER_SIZE   (16)
    #elif defined SDCARD_SUPPORT
        #define TX_BUFFER_SIZE   (2048)                                  // the size of RS232 output buffer
        #define RX_BUFFER_SIZE   (64)
    #else
        #define TX_BUFFER_SIZE   (512)                                   // the size of RS232 output buffer
        #define RX_BUFFER_SIZE   (64)
    #endif

    #if defined STM32F746G_DISCO                                         // virtual com port on st-link
        #define USART1_REMAP                                             // use USART1 on remapped pins (note that this is channel 0)
        #define USART1_NOREMAP_TX                                        // rx is remapped but tx not
    #endif
    #if defined NUCLEO_F031K6
        #define USART1_PARTIAL_REMAP
    #endif
    #if defined NUCLEO_L432KC || defined NUCLEO_L031K6 || defined NUCLEO_L011K4 || defined NUCLEO_F031K6
        #define USART2_PARTIAL_REMAP
    #else
        #define USART2_REMAP                                             // use USART2 on remapped pins (note that this is channel 1)
    #endif
    #if defined STM32_P207 || defined STM32F407ZG_SK || defined NUCLEO_F429ZI
        #define USART3_FULL_REMAP                                        // use USART3 on second set of remapped pins (note that this is channel 2)
    #elif !defined STM3241G_EVAL && !defined ARDUINO_BLUE_PILL
        #define USART3_PARTIAL_REMAP                                     // use USART3 on first set of remapped pins (note that this is channel 2)
    #endif
  //#define USART3_FULL_REMAP                                            // use USART3 on second set of remapped pins (note that this is channel 2)
  //#define USART6_REMAP                                                 // use USART6 on remapped pins (note that this is channel 5)

  //#define UART_RX_INPUT_TYPE  INPUT_PULL_UP                            // {8} enable pull-ups on UART Rx inputs
  //#define UART_CTS_INPUT_TYPE INPUT_PULL_UP                            // {8} enable pull-ups on UART CTS inputs
#else
    #define TX_BUFFER_SIZE   (256)
    #define RX_BUFFER_SIZE   (256)
#endif

//#define SUPPORT_ADC
#define ADC_REFERENCE_VOLTAGE                      3300                  // ADC uses 3.3V reference
#define ADC_SIM_STEP_SIZE                          200                   // 200mV steps when simulating

#if defined SUPPORT_ADC
    #define ADC123_0_START_VOLTAGE                 33                    // initial voltages when simulating (mV)
    #define ADC123_1_START_VOLTAGE                 500
    #define ADC123_2_START_VOLTAGE                 1000
    #define ADC123_3_START_VOLTAGE                 1500
    #define ADC12_4_START_VOLTAGE                  2000
    #define ADC12_5_START_VOLTAGE                  2500
    #define ADC12_6_START_VOLTAGE                  3000
    #define ADC12_7_START_VOLTAGE                  3300
    #define ADC12_8_START_VOLTAGE                  600
    #define ADC12_9_START_VOLTAGE                  1000
    #define ADC123_10_START_VOLTAGE                1500
    #define ADC123_11_START_VOLTAGE                2000
    #define ADC123_12_START_VOLTAGE                2500
    #define ADC123_13_START_VOLTAGE                3000
    #define ADC12_14_START_VOLTAGE                 3300
    #define ADC12_15_START_VOLTAGE                 1500
#endif

#define SUPPORT_TIMER                                                    // support hardware timer interrupt configuration
#if defined MODBUS_RTU && !defined SUPPORT_TIMER
    #define SUPPORT_TIMER                                                // MODBUS required HW timer in RTU mode
#endif

#define _TIMER_INTERRUPT_SETUP  TIMER_INTERRUPT_SETUP

#define MODBUS0_TIMER_CHANNEL    3                                       // general purpose HW timers to use for MODBUS RTU and RTS timing (2,3,4,5,10,11,13 and 14 supported - 10,11 are APB2 timers and the others are APB1 timers)
#define MODBUS1_TIMER_CHANNEL    4
#define MODBUS2_TIMER_CHANNEL    5
#define MODBUS3_TIMER_CHANNEL    10
#define MODBUS4_TIMER_CHANNEL    11
#define MODBUS5_TIMER_CHANNEL    13

#define SUPPORT_PORT_INTERRUPTS                                          // support code for port interrupts

#define TICK_INTERRUPT()                                                 // user callback from system TICK

// I2C Interface
//
#if defined I2C_INTERFACE
    #if CHIP_HAS_I2C > 2
        #define OUR_I2C_CHANNEL     2                                    // use I2C2 for demo (I2C3)
    #else
        #define OUR_I2_I2C_INIT_CODEC_CHANNEL     0                      // use I2C0 for demo (I2C1)
    #endif
    #if defined _STM32F2XX || defined _STM32F4XX || defined _STM32F7XX   // {5} add alternative I2C multiplexing options
      //#define I2C1_ALT_PINS                                            // I2C1 on PB7 and PB8 rather than on PB6 and PB9
      //#define I2C2_ALT_PINS_1                                          // I2C2 on PF0 and PF1 rather than on PB10 and PB11
      //#define I2C2_ALT_PINS_2                                          // I2C2 on PH4 and PH5 rather than on PB10 and PB11
      //#define I2C3_ALT_PINS                                            // I2C3 on PH7 and PH8 rather than on PA8 and PC9
    #endif
#endif


// LAN interface
//
#if defined ETH_INTERFACE
    #define IP_RX_CHECKSUM_OFFLOAD                                       // allow the HW to perform IPv4/v6 UDP, TCP and ICMP checksum verification so that no such calculation is required in code
    #define IP_TX_CHECKSUM_OFFLOAD                                       // allow the HW to insert IPv4/v6 header checksum and so the software doesn't need to calculate and insert this
    #define IP_TX_PAYLOAD_CHECKSUM_OFFLOAD                               // allow the HW to insert IPv4/v6 payload checksum and so the software doesn't need to calculate and insert this (overrides IP_TX_CHECKSUM_OFFLOAD)

  //#define _MAGIC_RESET_FRAME        0x1234                             // {9} allow a magic Ethernet reception frame to reset the board to boot loader mode (Ethernet protocol used) followed by three repetitions of the same value (total frame length 61 bytes)

  //#define LAN_REPORT_ACTIVITY                                          // transmissions/receptions and errors are reported to chosen task (for link LED control, etc)
    #if defined LAN_REPORT_ACTIVITY
        #define LNKLED                 LED1
        #define ACTLED                 LED2
        #define SPDLED                 0
        #define DUPLED                 0
        #define COLLED                 0
        #define ACTIVITY_LED_OFF()     _CLEARBITS(D, ACTLED)
        #define ACTIVITY_LED_ON()      _SETBITS(D, ACTLED)
        #define TURN_LINK_LED_ON()     _SETBITS(D, LNKLED)
        #define SPEED_LED_ON()         _SETBITS(D, (SPDLED | LNKLED))
        #define SPEED_LED_OFF()        _CLEARBITS(D, (SPDLED | LNKLED))
        #define LINK_DOWN_LEDS()       _CLEARBITS(D, (LNKLED | ACTLED | COLLED | SPDLED | DUPLED))

        #define CONFIGURE_LAN_LEDS()   _CONFIG_DRIVE_PORT_OUTPUT_VALUE(D, (LNKLED | ACTLED | COLLED | SPDLED | DUPLED), (OUTPUT_SLOW | OUTPUT_PUSH_PULL),  0)
    #endif

    #define NUMBER_OF_TX_BUFFERS_IN_ETHERNET_DEVICE 3
    #define NUMBER_OF_RX_BUFFERS_IN_ETHERNET_DEVICE 4
#endif

#if defined USE_IP
    #define LAN_BUFFER_SIZE           1514                               // STM32 has ample space for full tx buffer
#else
    #define LAN_BUFFER_SIZE           256                                // if using Ethernet without IP the buffer size can be set here
#endif

#if defined USE_BUFFERED_TCP                                             // if using a buffer for TCP to allow interractive data applications (like TELNET)
    #define TCP_BUFFER                2800                               // size of TCP buffer (with USE_BUFFERED_TCP) - generous with LM3Sxxxx
    #define TCP_BUFFER_FRAME          1400                               // allow this max. TCP frame size
#endif

#if defined USE_HTTP
    #define HTTP_BUFFER_LENGTH        1400                               // we send frames with this maximum amount of payload data - generous with LM3Sxxxx
#endif

#if defined USE_FTP                                                      // specify FTP support details
    #define FTP_WELCOME_RESPONSE         "220 Welcome STM32 FTP.\r\n"
    //                                        ^^^^^^^^^^^^^                 customise the FTP welcome here
#endif

// SMTP settings
//
#if defined STM3210C_EVAL
    #define SENDERS_EMAIL_ADDRESS             "STM3210C_EVAL@uTasker.com" // fictional Email address of the board being used
    #define EMAIL_SUBJECT                     "STM3210C Test"            // email subject
    #define EMAIL_CONTENT                     "Hello!!\r\nThis is an email message from the STM3210C-EVAL.\r\nI hope that you have received this test and have fun using the uTasker operating system with integrated TCP/IP stack.\r\r\nRegards your STM3210C!!";
#elif defined WISDOM_STM32F407
    #define SENDERS_EMAIL_ADDRESS             "STM32F407_EVAL@uTasker.com" // fictional Email address of the board being used
    #define EMAIL_SUBJECT                     "STM32F407 Test"           // email subject
    #define EMAIL_CONTENT                     "Hello!!\r\nThis is an email message from the STM32F407.\r\nI hope that you have received this test and have fun using the uTasker operating system with integrated TCP/IP stack.\r\r\nRegards your STM32F407!!";
#elif defined STM3241G_EVAL || defined STM32F746G_DISCO
    #define SENDERS_EMAIL_ADDRESS             "STM3241G_EVAL@uTasker.com"// fictional Email address of the board being used
    #define EMAIL_SUBJECT                     "STM3241G Test"            // email subject
    #define EMAIL_CONTENT                     "Hello!!\r\nThis is an email message from the STM3241G-EVAL.\r\nI hope that you have received this test and have fun using the uTasker operating system with integrated TCP/IP stack.\r\r\nRegards your STM3241G!!";
#elif defined NUCLEO_F429ZI
    #define SENDERS_EMAIL_ADDRESS             "Nucleo-F429ZI@uTasker.com"// fictional Email address of the board being used
    #define EMAIL_SUBJECT                     "Nucleo-F429ZI"            // email subject
    #define EMAIL_CONTENT                     "Hello!!\r\nThis is an email message from the Nucleo-F429ZI.\r\nI hope that you have received this test and have fun using the uTasker operating system with integrated TCP/IP stack.\r\r\nRegards your Nucleo-F429ZI!!";
#elif defined STM32F407ZG_SK
    #define SENDERS_EMAIL_ADDRESS             "STM32F407ZG-SK@uTasker.com" // fictional Email address of the board being used
    #define EMAIL_SUBJECT                     "STM32F407ZG-SK"           // email subject
    #define EMAIL_CONTENT                     "Hello!!\r\nThis is an email message from the STM32F407ZG-SK.\r\nI hope that you have received this test and have fun using the uTasker operating system with integrated TCP/IP stack.\r\r\nRegards your STM32F407ZG-SK!!";
#elif defined ST_MB997A_DISCOVERY && defined EMBEST_BASE_BOARD
    #define SENDERS_EMAIL_ADDRESS             "ST_MB997A@uTasker.com"    // fictional Email address of the board being used
    #define EMAIL_SUBJECT                     "ST_MB997A"                // email subject
    #define EMAIL_CONTENT                     "Hello!!\r\nThis is an email message from the ST_MB997A.\r\nI hope that you have received this test and have fun using the uTasker operating system with integrated TCP/IP stack.\r\r\nRegards your ST_MB997A!!";
#elif defined STM32_P207
    #define SENDERS_EMAIL_ADDRESS             "STM32-P207@uTasker.com"   // fictional Email address of the board being used
    #define EMAIL_SUBJECT                     "STM32-P207 Test"          // email subject
    #define EMAIL_CONTENT                     "Hello!!\r\nThis is an email message from the STM32-P207.\r\nI hope that you have received this test and have fun using the uTasker operating system with integrated TCP/IP stack.\r\r\nRegards your STM32-P207!!";
#endif
// SMTP account details if not in parameters
//
#define SMTP_ACCOUNT_NAME                     "User Name";
#define SMTP_PASSWORD                         "Password";

// internal HTML message pages
//
#define SUPPORT_INTERNAL_HTML_FILES                                      // enable the use of the following files
#define UPLOAD_FAILED         "<html><head><title>SW Upload failed</title></head><body bgcolor=#ff9000 text=#000000 topmargin=3 marginheight=3><center><td valign=top class=h><font color=#ff0000 style=font-size:30px><b style='mso-bidi-font-weight:normal'>&micro;Tasker</font></i></b></td><br></td><td align=left><br><br>Sorry but upload has failed.</font><br><br><a href=""javascript:history.back()"">Return</a></body></html>"
#define SW_UPLOAD_COMPLETED   "<html><head><meta http-equiv=""refresh"" content=""10;URL=0Menu.htm""><title>STM32 SW Uploaded</title></head><body bgcolor=#ffffff text=#000000 topmargin=3 marginheight=3><center><td valign=top class=h><font color=#ff0000 style=font-size:30px><b style='mso-bidi-font-weight:normal'>&micro;Tasker</font> - SW Update</i></b></td><br></td><td align=left><br><br>SW Upload successful. The STM32 target will now reset and start the new program. Please wait 10s...</body></html>"
#if defined USE_TFTP
    #define TFTP_ERROR_MESSAGE               "uTasker TFTP Error"        // fixed TFTP error test
#endif

// Special support for this processor type
//
#if !defined _STM32F7XX
    #define DMA_MEMCPY_SET                                               // memcpy and memset functions performed by DMA (if supported by processor - uses one DMA channel)
#endif
#if (defined _STM32F2XX || defined _STM32F4XX || defined _STM32F7XX)     // DMA2 must be used since DMA1 doesn't doesn't support memory to memory transfers
    #define MEMCPY_CHANNEL             8                                 // use channel 1 of DMA controller 2 (avoid using this channel for other DMA purpose)
#else
    #define MEMCPY_CHANNEL             1                                 // use channel 1 of DMA controller 1 (avoid using this channel for other DMA purpose)
                                                                         // MEMCPY_CHANNEL > 7 means from DMA controller 2 (eg. MEMCPY_CHANNEL 8 is channel 1 of DMA controller 2)
    #define DMA_CNDTR_MEMCPY  DMA1_CNDTR1                                // define the DMA controller registers to be used
    #define DMA_CMAR_MEMCPY   DMA1_CMAR1
    #define DMA_CPAR_MEMCPY   DMA1_CPAR1
    #define DMA_CCR_MEMCPY    DMA1_CCR1
    #define DMA_CNDTR1_MEMCPY DMA1_CNDTR1
#endif

// Define interrupt priorities in the system (STM32 cortex-m3/m4/m7 supports 0..15 - 0 is highest priority and 15 is lowest priority) (STM32L cortex-m0+ supports 0..3)
//
#if defined ARM_MATH_CM0PLUS || defined ARM_MATH_CM0
    #define SYSTICK_PRIORITY           3

    #define PRIORITY_EXI10_15          3
    #define PRIORITY_EXI5_9            3
    #define PRIORITY_EXI0              3
    #define PRIORITY_EXI1              3
    #define PRIORITY_EXI2              3
    #define PRIORITY_EXI3              3
    #define PRIORITY_EXI4              3
    #define PHY_INT_PRIORITY           PRIORITY_EXI10_15
    #define PRIORITY_UART5             2
    #define PRIORITY_UART4             2
    #define PRIORITY_USART6            2
    #define PRIORITY_UART7             2
    #define PRIORITY_UART8             2
    #define PRIORITY_USART1            2
    #define PRIORITY_USART2            2
    #define PRIORITY_USART3            2
    #define PRIORITY_RTC               2
    #define PRIORITY_HW_TIMER          1
    #define PRIORITY_TIMERS            1
    #define PRIORITY_USB_OTG           1
    #define PRIORITY_I2C1              1
    #define PRIORITY_I2C2              1
    #define PRIORITY_I2C3              1
    #define PRIORITY_TWI               1
    #define PRIORITY_TICK_TIMER        1
    #define PRIORITY_ADC               0
    #define PRIORITY_EMAC              0
    #define PRIORITY_OTG_FS            0
#else
    #define SYSTICK_PRIORITY           15

    #define PRIORITY_EXI10_15          8
    #define PRIORITY_EXI5_9            8
    #define PRIORITY_EXI0              8
    #define PRIORITY_EXI1              8
    #define PRIORITY_EXI2              8
    #define PRIORITY_EXI3              8
    #define PRIORITY_EXI4              8
    #define PHY_INT_PRIORITY           PRIORITY_EXI10_15
    #define PRIORITY_UART5             7
    #define PRIORITY_UART4             7
    #define PRIORITY_USART6            6
    #define PRIORITY_UART7             6
    #define PRIORITY_UART8             6
    #define PRIORITY_USART1            6
    #define PRIORITY_USART2            6
    #define PRIORITY_USART3            6
    #define PRIORITY_LPUART1           6
    #define PRIORITY_RTC               5
    #define PRIORITY_HW_TIMER          5
    #define PRIORITY_TIMERS            5
    #define PRIORITY_DEVICE_LP_FS      4
    #define PRIORITY_USB_OTG           4
    #define PRIORITY_I2C1              4
    #define PRIORITY_I2C2              4
    #define PRIORITY_I2C3              4
    #define PRIORITY_TWI               4
    #define PRIORITY_TICK_TIMER        3
    #define PRIORITY_ADC               2
    #define PRIORITY_EMAC              1
    #define PRIORITY_DEVICE_HP_FS      1
    #define PRIORITY_OTG_FS            1
#endif

#if defined NUCLEO_L496RG
    #define LED1                       PORTB_BIT14                        // green RED
    #define LED2                       PORTB_BIT7                         // green LED
    #define LED3                       PORTB_BIT5

    #define USR_BUTTON                 PORTC_BIT13

    #define DEMO_LED_1                 (LED1 >> 14)
    #define DEMO_LED_2                 (LED2 >> 6)
    #define DEMO_LED_3                 (LED3 >> 3)
    #define DEMO_USER_PORTS            (DEMO_LED_1 | DEMO_LED_2 | DEMO_LED_3)

    #define BLINK_LED                  LED1

    #define DEMO_INPUT_PORT            GPIOB_IDR

    #if !defined USE_MAINTENANCE || defined REMOVE_PORT_INITIALISATIONS
        #define INIT_WATCHDOG_LED() _CONFIG_DRIVE_PORT_OUTPUT_VALUE(B, (BLINK_LED), (BLINK_LED), (OUTPUT_SLOW | OUTPUT_PUSH_PULL))
        #define CONFIG_TEST_OUTPUT() _CONFIG_DRIVE_PORT_OUTPUT_VALUE(B, (LED2), (LED2), (OUTPUT_SLOW | OUTPUT_PUSH_PULL))
    #else
        #define INIT_WATCHDOG_LED()                                      // configured according to user parameters
        #define CONFIG_TEST_OUTPUT()
    #endif
    #define TOGGLE_WATCHDOG_LED()      _TOGGLE_PORT(B, BLINK_LED)        // blink the LED, if set as output
    #define TOGGLE_TEST_OUTPUT()       _TOGGLE_PORT(B, LED2)

    #define INIT_WATCHDOG_DISABLE()    _CONFIG_PORT_INPUT(C, (USR_BUTTON), (INPUT_PULL_DOWN)) // PC13 configured as input with pull-down (USR_BUTTON)
    #define WATCHDOG_DISABLE()         ((_READ_PORT_MASK(C, (USR_BUTTON))) != 0)

    #define KEYPAD "KeyPads/NUCLEO-L4XX.bmp"

    #define BUTTON_KEY_DEFINITIONS     {_PORTC, USR_BUTTON, {26, 497, 46, 517}}


                                        // '0'            '1'   input state center (x,   y)   0 = circle, radius, controlling port, controlling pin 
    #define KEYPAD_LED_DEFINITIONS     {RGB(50,50,50),RGB(255,0,0),  0, {119, 116, 128, 124}, _PORTB, LED1}, \
                                       {RGB(50,50,50),RGB(0,0,255),  0, {135, 116, 141, 124}, _PORTB, LED2}

#elif defined NUCLEO_L432KC || defined NUCLEO_L031K6 || defined NUCLEO_L011K4 || defined NUCLEO_F031K6
    #define LED1                       PORTB_BIT3                        // green LED
    #define LED2                       PORTB_BIT4
    #define LED3                       PORTB_BIT5

    #define DEMO_LED_1                 (LED1 >> 3)
    #define DEMO_LED_2                 (LED2 >> 3)
    #define DEMO_LED_3                 (LED3 >> 3)
    #define DEMO_USER_PORTS            (DEMO_LED_1 | DEMO_LED_2 | DEMO_LED_3)

    #define BLINK_LED                  LED1

    #define DEMO_INPUT_PORT            GPIOB_IDR

    #if !defined USE_MAINTENANCE || defined REMOVE_PORT_INITIALISATIONS
        #define INIT_WATCHDOG_LED() _CONFIG_DRIVE_PORT_OUTPUT_VALUE(B, (BLINK_LED), (BLINK_LED), (OUTPUT_SLOW | OUTPUT_PUSH_PULL))
        #define CONFIG_TEST_OUTPUT() _CONFIG_DRIVE_PORT_OUTPUT_VALUE(B, (LED2), (LED2), (OUTPUT_SLOW | OUTPUT_PUSH_PULL))
    #else
        #define INIT_WATCHDOG_LED()                                      // configured according to user parameters
        #define CONFIG_TEST_OUTPUT()
    #endif
    #define TOGGLE_WATCHDOG_LED()      _TOGGLE_PORT(B, BLINK_LED)        // blink the LED, if set as output
    #define TOGGLE_TEST_OUTPUT()       _TOGGLE_PORT(B, LED2)

    #define INIT_WATCHDOG_DISABLE()    _CONFIG_PORT_INPUT(A, (PORTA_BIT12), (INPUT_PULL_UP)) // PA12 configured as input with pull-up (CN3-5 on extension connector)
    #define WATCHDOG_DISABLE()         ((_READ_PORT_MASK(A, (PORTA_BIT12))) == 0)

    #define KEYPAD "KeyPads/NUCLEO32.bmp"

                                        // '0'            '1'   input state center (x,   y)   0 = circle, radius, controlling port, controlling pin 
    #define KEYPAD_LED_DEFINITIONS     {RGB(50,50,50),RGB(0,255,0),  0, {118, 408, 127, 424}, _PORTB, LED1}
#elif defined STM3241G_EVAL
    #define JOYSTICK_SEL               0x80                              // I/O expander input
    #define JOYSTICK_DOWN              0x08                              // I/O expander input
    #define JOYSTICK_LEFT              0x20                              // I/O expander input
    #define JOYSTICK_RIGHT             0x10                              // I/O expander input
    #define JOYSTICK_UP                0x40                              // I/O expander input

    #define USER_KEY_BUTTON            PORTG_BIT15
    #define WAKEUP_BUTTON              PORTA_BIT0
    #define TAMPER_BUTTON              PORTC_BIT13
    #define LED1                       PORTG_BIT6
    #define LED2                       PORTG_BIT8
    #define LED3                       PORTI_BIT9
    #define LED4                       PORTC_BIT7

    #define DEMO_LED_1                 (LED1 >> 6)                       // bit 0
    #define DEMO_LED_2                 (LED2 >> 7)                       // bit 2
    #define DEMO_LED_3                 (LED3 >> 7)                       // bit 3
    #define DEMO_LED_4                 (LED4 >> 4)                       // bit 4
    #define DEMO_USER_PORTS            (DEMO_LED_1 | DEMO_LED_2 | DEMO_LED_3 | DEMO_LED_4)

    #define BLINK_LED                  LED1
    #define ENABLE_LED_PORT()
    #define MEASURE_LOW_POWER_ON()     
    #define MEASURE_LOW_POWER_OFF()

    #define CONFIG_TEST_OUTPUT()                                         // we use DEMO_LED_2 which is configured by the user code (and can be disabled in parameters if required)
    #define TOGGLE_TEST_OUTPUT()       _TOGGLE_PORT(C, LED2)

    #if defined USE_MAINTENANCE && !defined REMOVE_PORT_INITIALISATIONS
        #define INIT_WATCHDOG_LED()                                      // we let the application configure all LEDs but we ensure that the port is enabled to avoid any access problems
    #else
        #define INIT_WATCHDOG_LED()    _CONFIG_PORT_OUTPUT(G, BLINK_LED, (OUTPUT_SLOW | OUTPUT_PUSH_PULL))
    #endif
    #define TOGGLE_WATCHDOG_LED()      _TOGGLE_PORT(G, BLINK_LED)        // blink the LED, if set as output

    #define INIT_WATCHDOG_DISABLE()    _CONFIG_PORT_INPUT(G, (USER_KEY_BUTTON), (INPUT_PULL_UP)) // PG15 configured as input with pull-up
    #define WATCHDOG_DISABLE()         ((_READ_PORT_MASK(G, (USER_KEY_BUTTON))) == 0) // disable watchdog by holding the user button down at reset

    #define PORT_EXT_TOUCH_IRQ_PORT     PORT_I                           // touch screen I2C port expander has interrupt in this port
    #define PORT_EXT_TOUCH_IRQ_PORT_BIT 2                                // touch screen I2C port expander has interrupt in this port bit
    #define PORT_EXT_TOUCH_IRQ_PRIORITY PRIORITY_EXI2                    // corresponding interrupt priority

  //#define JOYSTICK_CONTROL                                             // enable the second I2C expender to monitor joystick inputs (use together with GLCD touch screen)

    #if defined JOYSTICK_CONTROL                                         // USB mouse requires joystick
        #define _ucIO_expander ucIO_expander
    #else
        #define _ucIO_expander 0xff                                      // never active
    #endif

    #define CONFIGURE_MOUSE_INPUTS()                                     // I/O expander needs to be in operation
    #define MOUSE_LEFT_CLICK()         (!(_ucIO_expander & JOYSTICK_SEL)) // press this button to left-click on mouse
    #define MOUSE_UP()                 (!(_ucIO_expander & JOYSTICK_UP))  // press this button to move mouse up
    #define MOUSE_DOWN()               (!(_ucIO_expander & JOYSTICK_DOWN))// press this button to move mouse down
    #define MOUSE_LEFT()               (!(_ucIO_expander & JOYSTICK_LEFT))// press this button to move mouse left
    #define MOUSE_RIGHT()              (!(_ucIO_expander & JOYSTICK_RIGHT)) // press this button to move mouse right

    // LEDs                                                              {3}
    //
    #define KEYPAD_LEDS  4

                                       // '0'            '1'   input state center (x,   y)   0 = circle, radius, controlling port, controlling pin 
    #define KEYPAD_LED_DEFINITIONS     {RGB(50,50,50),RGB(0,255,0),  1, {136, 363, 140, 368}, _PORTG, LED1}, \
                                       {RGB(50,50,50),RGB(255,200,0),1, {130, 363, 134, 368}, _PORTG, LED2}, \
                                       {RGB(50,50,50),RGB(255,0,0),  1, {124, 363, 128, 368}, _PORTI, LED3}, \
                                       {RGB(50,50,50),RGB(0,0,255),  1, {118, 363, 122, 368}, _PORTC, LED4}


    #define BUTTON_KEY_DEFINITIONS     {_PORTG, USER_KEY_BUTTON, {220, 372, 233, 386}}, \
                                       {_PORTA, WAKEUP_BUTTON,   {119, 368, 135, 386}}, \
                                       {_PORTC, TAMPER_BUTTON,   {156, 372, 171, 385}}

    #define KEYPAD "KeyPads/STM3241G-EVAL.bmp"
#elif defined NUCLEO_F401RE
    #define USER_BUTTON_B1             PORTC_BIT13

    #define LED1                       PORTA_BIT5                        // LD2
    #define LED2                       PORTA_BIT6
    #define LED3                       PORTA_BIT7
    #define LED4                       PORTA_BIT8

    #define DEMO_LED_1                 (LED1 >> 5)                       // bit 5
    #define DEMO_LED_2                 (LED2 >> 5)                       // bit 6
    #define DEMO_LED_3                 (LED3 >> 5)                       // bit 7
    #define DEMO_LED_4                 (LED4 >> 5)                       // bit 8
    #define DEMO_USER_PORTS            (DEMO_LED_1 | DEMO_LED_2 | DEMO_LED_3 | DEMO_LED_4)

    #define BLINK_LED                  LED1
    #define ENABLE_LED_PORT()
    #define MEASURE_LOW_POWER_ON()     
    #define MEASURE_LOW_POWER_OFF()

    #define CONFIG_TEST_OUTPUT()                                         // we use DEMO_LED_2 which is configured by the user code (and can be disabled in parameters if required)
    #define TOGGLE_TEST_OUTPUT()       _TOGGLE_PORT(A, LED2)

    #if defined USE_MAINTENANCE && !defined REMOVE_PORT_INITIALISATIONS
        #define INIT_WATCHDOG_LED()                                      // we let the application configure all LEDs but we ensure that the port is enabled to avoid any access problems
    #else
        #define INIT_WATCHDOG_LED()    _CONFIG_PORT_OUTPUT(A, BLINK_LED, (OUTPUT_SLOW | OUTPUT_PUSH_PULL))
    #endif
    #define TOGGLE_WATCHDOG_LED()      _TOGGLE_PORT(A, BLINK_LED)        // blink the LED, if set as output

    #define INIT_WATCHDOG_DISABLE()    _CONFIG_PORT_INPUT(C, (USER_BUTTON_B1), (INPUT_PULL_UP)) // PC13 configured as input with pull-up
    #define WATCHDOG_DISABLE()         ((_READ_PORT_MASK(C, (USER_BUTTON_B1))) == 0) // disable watchdog by holding the user button down at reset

    // LEDs
    //
    #define KEYPAD_LEDS  4

                                       // '0'            '1'   input state center (x,   y)   0 = circle, radius, controlling port, controlling pin 
    #define KEYPAD_LED_DEFINITIONS     {RGB(50, 50, 50),  RGB(0, 255, 0), 0, {185, 192, 192, 198}, _PORTA, LED1}


    #define BUTTON_KEY_DEFINITIONS     {_PORTC, USER_BUTTON_B1, {105, 125, 124, 145}}

    #define KEYPAD "KeyPads/NUCLEO.bmp"
#elif defined WISDOM_STM32F407
    #define KEY_BUTTON_1               PORTE_BIT15
    #define KEY_BUTTON_2               PORTE_BIT14
    #define KEY_BUTTON_3               PORTE_BIT13
    #define KEY_BUTTON_4               PORTE_BIT12

    #define LED1                       PORTE_BIT8
    #define LED2                       PORTE_BIT9
    #define LED3                       PORTE_BIT10
    #define LED4                       PORTE_BIT11

    #define DEMO_LED_1                 (LED1 >> 8)                       // bit 8
    #define DEMO_LED_2                 (LED2 >> 8)                       // bit 9
    #define DEMO_LED_3                 (LED3 >> 8)                       // bit 10
    #define DEMO_LED_4                 (LED4 >> 8)                       // bit 11
    #define DEMO_USER_PORTS            (DEMO_LED_1 | DEMO_LED_2 | DEMO_LED_3 | DEMO_LED_4)

    #define BLINK_LED                  LED1
    #define ENABLE_LED_PORT()
    #define MEASURE_LOW_POWER_ON()     
    #define MEASURE_LOW_POWER_OFF()

    #define CONFIG_TEST_OUTPUT()                                         // we use DEMO_LED_2 which is configured by the user code (and can be disabled in parameters if required)
    #define TOGGLE_TEST_OUTPUT()       _TOGGLE_PORT(E, LED2)

    #if defined USE_MAINTENANCE && !defined REMOVE_PORT_INITIALISATIONS
        #define INIT_WATCHDOG_LED()                                      // we let the application configure all LEDs but we ensure that the port is enabled to avoid any access problems
    #else
        #define INIT_WATCHDOG_LED()    _CONFIG_PORT_OUTPUT(E, BLINK_LED, (OUTPUT_SLOW | OUTPUT_PUSH_PULL))
    #endif
    #define TOGGLE_WATCHDOG_LED()      _TOGGLE_PORT(E, BLINK_LED)        // blink the LED, if set as output

    #define INIT_WATCHDOG_DISABLE()    _CONFIG_PORT_INPUT(E, (KEY_BUTTON_1), (INPUT_PULL_UP)) // PE15 configured as input with pull-up
    #define WATCHDOG_DISABLE()         ((_READ_PORT_MASK(E, (KEY_BUTTON_1))) == 0) // disable watchdog by holding the user button down at reset

    // LEDs
    //
    #define KEYPAD_LEDS  4

                                       // '0'            '1'   input state center (x,   y)   0 = circle, radius, controlling port, controlling pin 
    #define KEYPAD_LED_DEFINITIONS     {RGB(255,0,0),  RGB(50,50,50),1, {208, 220, 216, 228}, _PORTE, LED1}, \
                                       {RGB(255,0,0),  RGB(50,50,50),1, {229, 218, 237, 225}, _PORTE, LED2}, \
                                       {RGB(255,0,0),  RGB(50,50,50),1, {250, 213, 258, 222}, _PORTE, LED3}, \
                                       {RGB(255,0,0),  RGB(50,50,50),1, {269, 211, 278, 219}, _PORTE, LED4}


    #define BUTTON_KEY_DEFINITIONS     {_PORTE, KEY_BUTTON_1, {227, 234, 242, 246}}, \
                                       {_PORTE, KEY_BUTTON_2, {252, 230, 264, 241}}, \
                                       {_PORTE, KEY_BUTTON_3, {274, 226, 287, 237}}, \
                                       {_PORTE, KEY_BUTTON_4, {297, 221, 308, 233}}

    #define KEYPAD "KeyPads/Wisdom_STM32F407.bmp"
#elif defined STM3210C_EVAL
    #define JOYSTICK_SEL               0x80                              // I/O expander input
    #define JOYSTICK_DOWN              0x40                              // I/O expander input
    #define JOYSTICK_LEFT              0x20                              // I/O expander input
    #define JOYSTICK_RIGHT             0x10                              // I/O expander input
    #define JOYSTICK_UP                0x08                              // I/O expander input

    #define USER_KEY_BUTTON            PORTB_BIT9
    #define WAKEUP_BUTTON              PORTA_BIT0
    #define TAMPER_BUTTON              PORTC_BIT13
    #define LED1                       PORTD_BIT7                        // green
    #define LED2                       PORTD_BIT13                       // orange
    #define LED3                       PORTD_BIT3                        // red
    #define LED4                       PORTD_BIT4                        // blue

    #define DEMO_LED_1                 (LED4 >> 4)                       // bit 0
    #define DEMO_LED_2                 (LED3 >> 2)                       // bit 1
    #define DEMO_LED_3                 (LED1 >> 5)                       // bit 2
    #define DEMO_LED_4                 (LED2 >> 10)                      // bit 3
    #define DEMO_USER_PORTS            (DEMO_LED_1 | DEMO_LED_2 | DEMO_LED_3 | DEMO_LED_4)

    #define BLINK_LED                  LED4
    #define DEMO_LED_PORT              GPIOD_ODR
    #define DEMO_INPUT_PORT            GPIOD_IDR
    #define ENABLE_LED_PORT()
    #define MEASURE_LOW_POWER_ON()     
    #define MEASURE_LOW_POWER_OFF()

    #define CONFIG_TEST_OUTPUT()                                         // we use DEMO_LED_2 which is configured by the user code (and can be disabled in parameters if required)
    #define TOGGLE_TEST_OUTPUT()       _TOGGLE_PORT(D, LED2)

    #if defined USE_MAINTENANCE && !defined REMOVE_PORT_INITIALISATIONS
        #define INIT_WATCHDOG_LED()                                      // we let the application configure all LEDs but we ensure that the port is enabled to avoid any access problems
    #else
        #define INIT_WATCHDOG_LED()    _CONFIG_PORT_OUTPUT(D, BLINK_LED, (OUTPUT_SLOW | OUTPUT_PUSH_PULL))
    #endif
    #define TOGGLE_WATCHDOG_LED()      _TOGGLE_PORT(D, BLINK_LED)        // blink the LED, if set as output

    #define INIT_WATCHDOG_DISABLE()    _CONFIG_PORT_INPUT(B, (USER_KEY_BUTTON), (INPUT_PULL_UP)) // PB9 configured as input with pull-up
    #define WATCHDOG_DISABLE()         ((_READ_PORT_MASK(B, (USER_KEY_BUTTON))) == 0) // disable watchdog by holding the user button down at reset

    #define PORT_EXT_TOUCH_IRQ_PORT    PORTB                             // touch screen I2C port expander has interrupt in this port
    #define PORT_EXT_TOUCH_IRQ_PORT_BIT 14                               // touch screen I2C port expander has interrupt in this port bit
    #define PORT_EXT_TOUCH_IRQ_PRIORITY PRIORITY_EXI10_15                // corresponding interrupt priority

  //#define JOYSTICK_CONTROL                                             // enable the second I2C expender to monitor joystick inputs (use together with GLCD touch screen)
    #if defined JOYSTICK_CONTROL                                         // USB mouse requires joystick
        #define _ucIO_expander ucIO_expander
    #else
        #define _ucIO_expander 0xff                                      // never active
    #endif

    #define CONFIGURE_MOUSE_INPUTS()                                     // I/O expander needs to be in operation
    #define MOUSE_LEFT_CLICK()         (!(_ucIO_expander & JOYSTICK_SEL)) // press this button to left-click on mouse
    #define MOUSE_UP()                 (!(_ucIO_expander & JOYSTICK_UP))  // press this button to move mouse up
    #define MOUSE_DOWN()               (!(_ucIO_expander & JOYSTICK_DOWN))// press this button to move mouse down
    #define MOUSE_LEFT()               (!(_ucIO_expander & JOYSTICK_LEFT))// press this button to move mouse left
    #define MOUSE_RIGHT()              (!(_ucIO_expander & JOYSTICK_RIGHT)) // press this button to move mouse right


    // LEDs                                                              {3}
    //
    #define KEYPAD_LEDS  4

                                       // '0'            '1'   input state center (x,   y)   0 = circle, radius, controlling port, controlling pin 
    #define KEYPAD_LED_DEFINITIONS     {RGB(190,190,190),RGB(0,255,0),  1, {123, 365, 127, 370}, _PORTD, LED1}, \
                                       {RGB(190,190,190),RGB(255,200,0),1, {117, 365, 121, 370}, _PORTD, LED2}, \
                                       {RGB(190,190,190),RGB(255,0,0),  1, {111, 365, 115, 370}, _PORTD, LED3}, \
                                       {RGB(190,190,190),RGB(0,0,255),  1, {105, 365, 109, 370}, _PORTD, LED4}


    #define BUTTON_KEY_DEFINITIONS     {_PORTB, USER_KEY_BUTTON, {226, 374, 242, 388}}, \
                                       {_PORTA, WAKEUP_BUTTON,   {111, 374, 126, 388}}, \
                                       {_PORTC, TAMPER_BUTTON,   {155, 374, 171, 388}}


    #define KEYPAD "KeyPads/STM32F1-EVAL.bmp"
#elif defined ST_MB997A_DISCOVERY                                        // F4
    #define USER_KEY_BUTTON            PORTA_BIT0
    #define LED1                       PORTD_BIT12                       // green LED
    #define LED2                       PORTD_BIT13                       // orange LED
    #define LED3                       PORTD_BIT14                       // red LED
    #define LED4                       PORTD_BIT15                       // blue LED

    #define PORT_SHIFT                 12

    #define DEMO_LED_1                 (LED1 >> PORT_SHIFT)
    #define DEMO_LED_2                 (LED2 >> PORT_SHIFT)
    #define DEMO_LED_3                 (LED3 >> PORT_SHIFT)
    #define DEMO_LED_4                 (LED4 >> PORT_SHIFT)
    #define DEMO_USER_PORTS            (DEMO_LED_1 | DEMO_LED_2)

    #define DEMO_INPUT_PORT            GPIOD_IDR
    #define DEMO_LED_PORT              GPIOD_ODR
    #define BLINK_LED                  LED1

    #define ENABLE_LED_PORT()        //_CONFIG_PORT_OUTPUT(D, BLINK_LED, (OUTPUT_SLOW | OUTPUT_PUSH_PULL))
    #define MEASURE_LOW_POWER_ON()     
    #define MEASURE_LOW_POWER_OFF()

    #define CONFIG_TEST_OUTPUT()
    #define TOGGLE_TEST_OUTPUT()

    #if defined USE_MAINTENANCE && !defined REMOVE_PORT_INITIALISATIONS
        #define INIT_WATCHDOG_LED()                                      // we let the application configure all LEDs but we ensure that the port is enabled to avoid any access problems
    #else
        #define INIT_WATCHDOG_LED()    _CONFIG_PORT_OUTPUT(D, BLINK_LED, (OUTPUT_SLOW | OUTPUT_PUSH_PULL))
    #endif    
    #define TOGGLE_WATCHDOG_LED()      _TOGGLE_PORT(D, BLINK_LED)        // blink the LED, if set as output

    #define INIT_WATCHDOG_DISABLE()    _CONFIG_PORT_INPUT(A, (USER_KEY_BUTTON), (FLOATING_INPUT)) // PA0 configured as input (pull-down on board)
    #define WATCHDOG_DISABLE()         (_READ_PORT_MASK(A, (USER_KEY_BUTTON))) // disable watchdog by holding the user button down at reset

    #define CONFIGURE_MOUSE_INPUTS()
    #define MOUSE_LEFT_CLICK()         (_READ_PORT_MASK(A, (USER_KEY_BUTTON)))
    #define MOUSE_UP()                 0
    #define MOUSE_DOWN()               0
    #define MOUSE_LEFT()               0
    #define MOUSE_RIGHT()              0

    // LEDs                                                              {3}
    //
    #define KEYPAD_LEDS  4

                                       // '0'            '1'   input state center (x,   y)   0 = circle, radius, controlling port, controlling pin 
    #define KEYPAD_LED_DEFINITIONS     {RGB(50,50,50),RGB(0,255,0),  1, {111, 257, 117, 262}, _PORTD, LED1}, \
                                       {RGB(50,50,50),RGB(255,200,0),1, {125, 241, 134, 247}, _PORTD, LED2}, \
                                       {RGB(50,50,50),RGB(255,0,0),  1, {142, 257, 148, 262}, _PORTD, LED3}, \
                                       {RGB(50,50,50),RGB(0,0,255),  1, {125, 273, 134, 279}, _PORTD, LED4}


    #define BUTTON_KEY_DEFINITIONS     {_PORTA, USER_KEY_BUTTON, {73, 248, 93, 268}},

    #define KEYPAD "KeyPads/STM32F4-DISC.bmp"
#elif defined NUCLEO_F429ZI
    #define USERS_BUTTON               PORTC_BIT13

    #define LED1                       PORTB_BIT0                        // green LED
    #define LED2                       PORTB_BIT7                        // blue LED
    #define LED3                       PORTB_BIT14                       // red LED

    #define DEMO_LED_1                 (LED1 >> 0)
    #define DEMO_LED_2                 (LED2 >> 6)
    #define DEMO_LED_3                 (LED3 >> 12)
    #define DEMO_USER_PORTS            (DEMO_LED_1 | DEMO_LED_2 | DEMO_LED_3)

    #define DEMO_INPUT_PORT            GPIOB_IDR
    #define DEMO_LED_PORT              GPIOB_ODR
    #define BLINK_LED                  LED1

    #define ENABLE_LED_PORT()        //_CONFIG_PORT_OUTPUT(F, BLINK_LED, (OUTPUT_SLOW | OUTPUT_PUSH_PULL))
    #define MEASURE_LOW_POWER_ON()     
    #define MEASURE_LOW_POWER_OFF()

    #define CONFIG_TEST_OUTPUT()
    #define TOGGLE_TEST_OUTPUT()      _TOGGLE_PORT(B, LED2)

    #if defined USE_MAINTENANCE && !defined REMOVE_PORT_INITIALISATIONS
        #define INIT_WATCHDOG_LED()                                      // we let the application configure all LEDs but we ensure that the port is enabled to avoid any access problems
    #else
        #define INIT_WATCHDOG_LED()    _CONFIG_PORT_OUTPUT(B, BLINK_LED, (OUTPUT_SLOW | OUTPUT_PUSH_PULL))
    #endif    
    #define TOGGLE_WATCHDOG_LED()      _TOGGLE_PORT(B, BLINK_LED)        // blink the LED, if set as output

    #define INIT_WATCHDOG_DISABLE()    _CONFIG_PORT_INPUT(C, (USERS_BUTTON), (FLOATING_INPUT)) // configured as input (pull-down on board)
    #define WATCHDOG_DISABLE()         (_READ_PORT_MASK(C, (USERS_BUTTON)) != 0) // disable watchdog by holding the user button down at reset

    #define CONFIGURE_MOUSE_INPUTS()
    #define MOUSE_LEFT_CLICK()         (_READ_PORT_MASK(C, (USERS_BUTTON)) != 0)
    #define MOUSE_UP()                 0
    #define MOUSE_DOWN()               0
    #define MOUSE_LEFT()               0
    #define MOUSE_RIGHT()              0

    // LEDs
    //
    #define KEYPAD_LEDS  3

                                        // '0'          '1'   input state center (x,   y)   0 = circle, radius, controlling port, controlling pin 
    #define KEYPAD_LED_DEFINITIONS      {RGB(50,50,50), RGB(0,255,0),  0, {170, 128, 176, 136}, _PORTB, LED1}, \
                                        {RGB(50,50,50), RGB(0,0,255),  0, {155, 128, 162, 136}, _PORTB, LED2}, \
                                        {RGB(50,50,50), RGB(255,0,0),  0, {140, 128, 146, 136}, _PORTB, LED3}


    #define BUTTON_KEY_DEFINITIONS      {_PORTC, USERS_BUTTON,  {27, 586,  51, 607}}

    #define KEYPAD "KeyPads/NUCLEO-144.bmp"

    #define TIMER_3_PARTIAL_REMAP
#elif defined STM32_P207 || defined STM32F407ZG_SK                       // F2/F4
    #if defined STM32F407ZG_SK
        #define USERS_BUTTON           PORTG_BIT6
    #endif
    #define TAMPER_BUTTON              PORTC_BIT13
    #define WKUP_BUTTON                PORTA_BIT0

    #define LED1                       PORTF_BIT6                        // green LED
    #define LED2                       PORTF_BIT7                        // yellow LED
    #define LED3                       PORTF_BIT8                        // red LED
    #define LED4                       PORTF_BIT9                        // green LED

    #define PORT_SHIFT                 6

    #define DEMO_LED_1                 (LED1 >> PORT_SHIFT)
    #define DEMO_LED_2                 (LED2 >> PORT_SHIFT)
    #define DEMO_LED_3                 (LED3 >> PORT_SHIFT)
    #define DEMO_LED_4                 (LED4 >> PORT_SHIFT)
    #define DEMO_USER_PORTS            (DEMO_LED_1 | DEMO_LED_2 | DEMO_LED_3 | DEMO_LED_4)

    #define DEMO_INPUT_PORT            GPIOF_IDR
    #define DEMO_LED_PORT              GPIOF_ODR
    #define BLINK_LED                  LED1

    #define ENABLE_LED_PORT()        //_CONFIG_PORT_OUTPUT(F, BLINK_LED, (OUTPUT_SLOW | OUTPUT_PUSH_PULL))
    #define MEASURE_LOW_POWER_ON()     
    #define MEASURE_LOW_POWER_OFF()

    #define CONFIG_TEST_OUTPUT()
    #define TOGGLE_TEST_OUTPUT()      _TOGGLE_PORT(F, LED2)

    #if defined USE_MAINTENANCE && !defined REMOVE_PORT_INITIALISATIONS
        #define INIT_WATCHDOG_LED()                                      // we let the application configure all LEDs but we ensure that the port is enabled to avoid any access problems
    #else
        #define INIT_WATCHDOG_LED()    _CONFIG_PORT_OUTPUT(F, BLINK_LED, (OUTPUT_SLOW | OUTPUT_PUSH_PULL))
    #endif    
    #define TOGGLE_WATCHDOG_LED()      _TOGGLE_PORT(F, BLINK_LED)        // blink the LED, if set as output

    #define INIT_WATCHDOG_DISABLE()    _CONFIG_PORT_INPUT(A, (WKUP_BUTTON), (FLOATING_INPUT)) // PA0 configured as input (pull-up on board)
    #define WATCHDOG_DISABLE()         (_READ_PORT_MASK(A, (WKUP_BUTTON)) == 0) // disable watchdog by holding the user button down at reset

    #define CONFIGURE_MOUSE_INPUTS()
    #define MOUSE_LEFT_CLICK()         (_READ_PORT_MASK(A, (WKUP_BUTTON)) == 0)
    #define MOUSE_UP()                 0
    #define MOUSE_DOWN()               0
    #define MOUSE_LEFT()               0
    #define MOUSE_RIGHT()              0

    #if defined STM32F407ZG_SK
        // LEDs
        //
        #define KEYPAD_LEDS  4

                                           // '0'            '1'   input state center (x,   y)   0 = circle, radius, controlling port, controlling pin 
        #define KEYPAD_LED_DEFINITIONS     {RGB(50,50,50),RGB(0,255,0),  0, {106, 210, 112, 215}, _PORTF, LED1}, \
                                           {RGB(50,50,50),RGB(255,200,0),0, {106, 218, 112, 223}, _PORTF, LED2}, \
                                           {RGB(50,50,50),RGB(255,0,0),  0, {106, 224, 112, 229}, _PORTF, LED3}, \
                                           {RGB(50,50,50),RGB(0,255,0),  0, {106, 231, 112, 236}, _PORTF, LED4}


        #define BUTTON_KEY_DEFINITIONS     {_PORTA, WKUP_BUTTON,   {21, 185, 43, 207}}, \
                                           {_PORTC, TAMPER_BUTTON, {51, 55,  58, 64 }}, \
                                           {_PORTG, USERS_BUTTON,  {21, 88,  40, 107}}, \

        #define KEYPAD "KeyPads/STM32F407ZG_SK.bmp"
    #else
        // LEDs
        //
        #define KEYPAD_LEDS  4

                                           // '0'            '1'   input state center (x,   y)   0 = circle, radius, controlling port, controlling pin 
        #define KEYPAD_LED_DEFINITIONS     {RGB(50,50,50),RGB(0,255,0),  0, {294, 206, 0, 3}, _PORTF, LED1}, \
                                           {RGB(50,50,50),RGB(255,200,0),0, {299, 203, 0, 3}, _PORTF, LED2}, \
                                           {RGB(50,50,50),RGB(255,0,0),  0, {304, 200, 0, 3}, _PORTF, LED3}, \
                                           {RGB(50,50,50),RGB(0,255,0),  0, {309, 197, 0, 3}, _PORTF, LED4}


        #define BUTTON_KEY_DEFINITIONS     {_PORTA, WKUP_BUTTON,   {318, 258, 340, 279}}, \
                                           {_PORTC, TAMPER_BUTTON, {215, 317, 240, 340}}, \

        #define KEYPAD "KeyPads/STM32-P207.bmp"
    #endif
#elif defined ST_MB913C_DISCOVERY || defined ARDUINO_BLUE_PILL           // F1
    #define USER_KEY_BUTTON            PORTA_BIT0
    #if defined ARDUINO_BLUE_PILL
        #define LED3                   PORTC_BIT13                       // green LED
        #define LED4                   PORTC_BIT14
        #define PORT_SHIFT             13
    #else
        #define LED3                   PORTC_BIT8                        // blue LED
        #define LED4                   PORTC_BIT9                        // green LED
        #define PORT_SHIFT                 8
    #endif

    #define DEMO_LED_1                 (LED3 >> PORT_SHIFT)
    #define DEMO_LED_2                 (LED4 >> PORT_SHIFT)
    #define DEMO_LED_3                 0
    #define DEMO_USER_PORTS            (DEMO_LED_1 | DEMO_LED_2)

    #define DEMO_INPUT_PORT            GPIOC_IDR
    #define DEMO_LED_PORT              GPIOC_ODR
    #define BLINK_LED                  LED3

    #define ENABLE_LED_PORT()          //_CONFIG_PORT_OUTPUT(C, BLINK_LED, (OUTPUT_SLOW | OUTPUT_PUSH_PULL))
    #define MEASURE_LOW_POWER_ON()     
    #define MEASURE_LOW_POWER_OFF()

    #define CONFIG_TEST_OUTPUT()
    #define TOGGLE_TEST_OUTPUT()
    #if defined USE_MAINTENANCE && !defined REMOVE_PORT_INITIALISATIONS
        #define INIT_WATCHDOG_LED()                                      // we let the application configure all LEDs but we ensure that the port is enabled to avoid any access problems
    #else
        #define INIT_WATCHDOG_LED()    _CONFIG_PORT_OUTPUT(C, BLINK_LED, (OUTPUT_SLOW | OUTPUT_PUSH_PULL))
    #endif
    #define TOGGLE_WATCHDOG_LED()      _TOGGLE_PORT(C, BLINK_LED)        // blink the LED, if set as output

    #define INIT_WATCHDOG_DISABLE()    _CONFIG_PORT_INPUT(A, (USER_KEY_BUTTON), (INPUT_PULL_UP)) // PA0 configured as input with pull-up
    #define WATCHDOG_DISABLE()         (1) // (0 == (_READ_PORT_MASK(A, (USER_KEY_BUTTON)))) // disable watchdog by holding the user button down at reset

    #define CONFIGURE_MOUSE_INPUTS()
    #define MOUSE_LEFT_CLICK()         0
    #define MOUSE_UP()                 0
    #define MOUSE_DOWN()               0
    #define MOUSE_LEFT()               0
    #define MOUSE_RIGHT()              0

    // LEDs                                                              {3}
    //
    #define KEYPAD_LEDS  2
    #if defined ARDUINO_BLUE_PILL
                                           // '0'            '1'    input state center (x,   y)   0 = circle, radius, controlling port, controlling pin 
        #define KEYPAD_LED_DEFINITIONS     {RGB(0,255,0),RGB(20,20,20), 1, {650, 215, 666, 244}, _PORTC, LED3},

        #define BUTTON_KEY_DEFINITIONS     {_PORTA, USER_KEY_BUTTON, {580, 55, 608, 79}},

        #define KEYPAD "KeyPads/BluePill.bmp"
    #else
                                           // '0'            '1'    input state center (x,   y)   0 = circle, radius, controlling port, controlling pin 
        #define KEYPAD_LED_DEFINITIONS     {RGB(190,190,190),RGB(0,0,255), 1, {41,  482, 49,  502}, _PORTC, LED3}, \
                                           {RGB(190,190,190),RGB(0,255,0), 1, {223, 482, 231, 502}, _PORTC, LED4}

        #define BUTTON_KEY_DEFINITIONS     {_PORTA, USER_KEY_BUTTON, {75, 425, 111, 457}},

        #define KEYPAD "KeyPads/STM32F1-DISC.bmp"
    #endif
#elif defined STM32F746G_DISCO
    #define USER_KEY_BUTTON            PORTI_BIT11

    #define LED1                       PORTI_BIT1                        // green LED
    #define LED2                       PORTI_BIT2                        // no further LEDs available on the board
    #define LED3                       PORTI_BIT3
    #define LED4                       PORTI_BIT4

    #define DEMO_LED_1                 (LED1 >> 1)
    #define DEMO_LED_2                 (LED2 >> 1)
    #define DEMO_LED_3                 (LED3 >> 1)
    #define DEMO_LED_4                 (LED4 >> 1)
    #define DEMO_USER_PORTS            (DEMO_LED_1 | DEMO_LED_2 | DEMO_LED_3 | DEMO_LED_4)

    #define BLINK_LED                  LED1
    #define ENABLE_LED_PORT()
    #define MEASURE_LOW_POWER_ON()     
    #define MEASURE_LOW_POWER_OFF()

    #define CONFIG_TEST_OUTPUT()                                         // we use DEMO_LED_2 which is configured by the user code (and can be disabled in parameters if required)
    #define TOGGLE_TEST_OUTPUT()       _TOGGLE_PORT(I, LED2)

    #if defined USE_MAINTENANCE && !defined REMOVE_PORT_INITIALISATIONS
        #define INIT_WATCHDOG_LED()                                      // we let the application configure all LEDs but we ensure that the port is enabled to avoid any access problems
    #else
        #define INIT_WATCHDOG_LED()    _CONFIG_PORT_OUTPUT(I, BLINK_LED, (OUTPUT_SLOW | OUTPUT_PUSH_PULL))
    #endif
    #define TOGGLE_WATCHDOG_LED()      _TOGGLE_PORT(I, BLINK_LED)        // blink the LED, if set as output

    #define INIT_WATCHDOG_DISABLE()    _CONFIG_PORT_INPUT(I, (USER_KEY_BUTTON), (INPUT_PULL_DOWN)) // PI11 configured as input with pull-down
    #define WATCHDOG_DISABLE()         ((_READ_PORT_MASK(I, (USER_KEY_BUTTON))) != 0) // disable watchdog by holding the user button down at reset

    // LEDs                                                              {3}
    //
    #define KEYPAD_LEDS  4

                                       // '0'            '1'        input state  center (x,   y)  0 = circle, radius, controlling port, controlling pin 
    #define KEYPAD_LED_DEFINITIONS     {RGB(50, 50, 50), RGB(0, 255, 0), 0, {14, 163, 27, 168}, _PORTI, LED1}

    #define BUTTON_KEY_DEFINITIONS     {_PORTI, USER_KEY_BUTTON, {16, 235, 31, 250}}

    #define KEYPAD "KeyPads/STM32F746-DISC.bmp"
#endif

#define SET_USB_SYMBOL()                                                 // dummy - can be use to indicate when USB is active
#define DEL_USB_SYMBOL()                                                 // dummy - can be use to indicate when USB has been deactivated

#define CONFIG_TIMER_TEST_LEDS()       _CONFIG_PORT_OUTPUT(D, (DEMO_LED_2 | DEMO_LED_3), (OUTPUT_SLOW | OUTPUT_PUSH_PULL))
#define TIMER_TEST_LED_ON()            _SETBITS(D, DEMO_LED_2)
#define TIMER_TEST_LED2_ON()           _SETBITS(D, DEMO_LED_3)
#define TIMER_TEST_LED_OFF()           _CLEARBITS(D, DEMO_LED_2)
#define TIMER_TEST_LED2_OFF()          _CLEARBITS(D, DEMO_LED_3)


// Ethernet PHY (MII mode)
//
#if defined ETH_INTERFACE
    #if defined WISDOM_STM32F407
        #define _DP83848
        #define ETHERNET_RMII                                            // use RMII Ethernet interface instead of MII
        #define ETH_TXD_TX_EN_G                                          // TXD0, TXD1 and TX_EN on port G
        #define PHY_ADDRESS_           0x01                              // address of PHY on board
        #define VNDR_MDL               0x09                              // vendor model number
        #define MDL_REV                0x00                              // model revision number
        #define PHY_IDENTIFIER         (0x20005c00 | (VNDR_MDL << 4) | MDL_REV) // NATIONAL DP83848 identifier
        #define PHY_POLL_LINK                                            // activate polling of the link state since this board has no interrupt line
        #define INTERRUPT_TASK_PHY     TASK_NETWORK_INDICATOR            // link status reported to this task (do not use together with LAN_REPORT_ACTIVITY)
    #elif defined STM3241G_EVAL
        #define _DP83848
        #define PHY_ADDRESS_           0x01                              // address of PHY on STM3241G_EVAL board (MII mode)
        #define VNDR_MDL               0x09                              // vendor model number
        #define MDL_REV                0x00                              // model revision number
        #define PHY_IDENTIFIER         (0x20005c00 | (VNDR_MDL << 4) | MDL_REV) // NATIONAL DP83848 identifier
        #define PHY_INTERRUPT          PORTB_BIT14                       // use PHY interrupt
        #define PHY_INT_PORT           PORTB
        #define PHY_INT_PIN_STATE()    _READ_PORT_MASK(B, PHY_INTERRUPT)
    #elif defined ST_MB997A_DISCOVERY && defined EMBEST_BASE_BOARD       // {6}
        #define _LAN8720
        #define PHY_ADDRESS_           0x00                              // address of PHY on DiscoverMo board (RMII mode)
        #define VNDR_MDL               0x0f                              // vendor model number
        #define MDL_REV                0x00                              // model revision number
        #define PHY_IDENTIFIER         (0x0007c000 | (VNDR_MDL << 4) | MDL_REV) // SMSC identifier
        #define ETHERNET_RMII                                            // use RMII Ethernet interface instead of MII
        #define RESET_PHY                                                // use a port output to control the reset input of the phy
        #define ASSERT_PHY_RST()       _CONFIG_DRIVE_PORT_OUTPUT_VALUE(E, PORTE_BIT2, (OUTPUT_SLOW | OUTPUT_PUSH_PULL), 0) // assert reset line to phy
        #define CONFIG_PHY_STRAPS()                                      // dummy
        #define NEGATE_PHY_RST()       _SETBITS(E, PORTE_BIT2)           // negate reset line to phy
      //#define PHY_INTERRUPT                                            // no PHY interrupt is connected
        #define PHY_POLL_LINK                                            // activate polling of the link state
        #define INTERRUPT_TASK_PHY     TASK_NETWORK_INDICATOR            // link status reported to this task (do not use together with LAN_REPORT_ACTIVITY)
    #elif defined STM32F407ZG_SK
        #define ETHERNET_RMII                                            // use RMII Ethernet interface instead of MII
        #define _ST802RT1B                                               // ST802RT1B PHY used on the board
        #define PHY_ADDRESS_           (0x01)                            // address of PHY on board
        #define PHY_IDENTIFIER         0x02038461                        // ST802RT1B identifier
        #define ETH_TXD_G                                                // locate TXD0 and TXD1 on port G rather than port B
        #define PHY_INTERRUPT          PORTA_BIT3                        // use PHY interrupt
        #define SUPPORT_PORT_INTERRUPTS                                  // support code for port interrupts due to the PHY interrupt
        #define PHY_INT_PORT           PORTA                             // interrupt on PA3
        #define PHY_INT_PIN_STATE()    _READ_PORT_MASK(A, PHY_INTERRUPT)
    #elif defined STM32_P207
        #define ETHERNET_RMII                                            // use RMII Ethernet interface instead of MII
        #define _KS8721                                                  // Micrel KS8721BLMM PHY
        #define PHY_ADDRESS_           (0x01)                            // address of PHY on board
        #define PHY_IDENTIFIER         0x00221619                        // MICREL identifier
        #define ETH_TXD_G                                                // locate TXD0 and TXD1 on port G rather than port B
        #define PHY_INTERRUPT          PORTA_BIT3                        // use PHY interrupt
        #define SUPPORT_PORT_INTERRUPTS                                  // support code for port interrupts due to the PHY interrupt
        #define PHY_INT_PORT           PORTA                             // interrupt on PA3
        #define PHY_INT_PIN_STATE()    _READ_PORT_MASK(A, PHY_INTERRUPT)
    #elif defined STM32F746G_DISCO || defined NUCLEO_F429ZI
        #define _LAN8742
        #define PHY_ADDRESS_           0x00                              // address of PHY on DiscoverMo board (RMII mode)
        #define VNDR_MDL               0x13                              // vendor model number
        #define MDL_REV                0x00                              // model revision number
        #define PHY_IDENTIFIER         (0x0007c000 | (VNDR_MDL << 4) | MDL_REV) // SMSC identifier
        #define ETHERNET_RMII                                            // use RMII Ethernet interface instead of MII
        #if defined NUCLEO_F429ZI
            #define ETH_TXD1_B                                           // TXD1 on port B with TXD0 and TX_EN on port G
        #else
            #define ETH_TXD_TX_EN_G                                      // TXD0, TXD1 and TX_EN on port G
        #endif
      //#define RESET_PHY                                                // use a port output to control the reset input of the phy
      //#define ASSERT_PHY_RST()       _CONFIG_DRIVE_PORT_OUTPUT_VALUE(E, PORTE_BIT2, (OUTPUT_SLOW | OUTPUT_PUSH_PULL), 0) // assert reset line to phy
      //#define CONFIG_PHY_STRAPS()                                      // dummy
      //#define NEGATE_PHY_RST()       _SETBITS(E, PORTE_BIT2)           // negate reset line to phy
      //#define PHY_INTERRUPT                                            // no PHY interrupt is connected
        #define PHY_POLL_LINK                                            // activate polling of the link state
        #define INTERRUPT_TASK_PHY     TASK_NETWORK_INDICATOR            // link status reported to this task (do not use together with LAN_REPORT_ACTIVITY)
    #else
        #define _DP83848
        #define PHY_ADDRESS_           0x01                              // address of PHY on STM3210C_EVAL board
        #define ETHERNET_RMII                                            // use RMII Ethernet interface instead of MII
        #define VNDR_MDL               0x09                              // vendor model number
        #define MDL_REV                0x00                              // model revision number
        #define PHY_IDENTIFIER         (0x20005c00 | (VNDR_MDL << 4) | MDL_REV) // NATIONAL DP83848 identifier
        #define PHY_INTERRUPT          PORTC_BIT13                       // use PHY interrupt on this pin
        #define PHY_INT_PORT           PORTC
        #define PHY_INT_PIN_STATE()    _READ_PORT_MASK(C, PHY_INTERRUPT)      
    #endif

    #define ETHERNET_DRIVE_PHY_25MHZ                                     // set to drive 25MHz from MCO output for use as external PHY clock (saves 25MHz crystal requirement) [used also to specify driving 50MHz clock in RMII mode]
#endif

// GLCD
//
#if (defined STM3241G_EVAL || defined STM32F746G_DISCO) && defined SUPPORT_GLCD // {1} FSMC 16 bit data bus used a data lines, configured as inputs, port D0 used as C/D, port G 5,6,7 used as RD and WR
    #define GLCD_C_D               PORTD_BIT0
    #define GLCD_WR                PORTG_BIT7
    #define GLCD_RD                PORTG_BIT6

    #define SET_PULL_DOWNS()       GPIOPDR_F = 0xff;
    #define REMOVE_PULL_DOWNS()    GPIOPDR_F = 0x00;

    #define LCD_ADDRESS_SETUP_TIME_HCLK         5                        // recommended GLCD access timings at maximum clock speed
    #define LCD_ADDRESS_HOLD_TIME_HCLK          0 
    #define LCD_DATA_SETUP_TIME_HCLK            9
    #define LCD_BUS_TURNAROUND_DURATION_HCLK    0
    #define LCD_CLK_PRESCALE_HCLK               2                        // 1..16 - don't care in NOR Flash, SRAM and ROM modes
    #define LCD_DATA_LATENCY_CLK                2                        // 2..17 - don't care in NOR Flash, SRAM and ROM modes
                                                                         // configure the D15..D0, A0, NOE, NWE, CS - enable FSMC bus, configure LCD as SRAM in 16 bit data mode in bank 3, with corresponding access timing
    #define CONFIGURE_GLCD()       _CONFIG_DRIVE_PORT_OUTPUT_VALUE(C, PORTC_BIT6, (OUTPUT_SLOW | OUTPUT_PUSH_PULL), 0); \
                                   _CONFIG_PERIPHERAL_OUTPUT(D, (PERIPHERAL_FSMC_SDIO_OTG), (PORTD_BIT10 | PORTD_BIT9 | PORTD_BIT8 | PORTD_BIT1 | PORTD_BIT0 | PORTD_BIT15 | PORTD_BIT14 | PORTD_BIT4 | PORTD_BIT5), (OUTPUT_FAST | OUTPUT_PUSH_PULL)); \
                                   _CONFIG_PERIPHERAL_OUTPUT(E, (PERIPHERAL_FSMC_SDIO_OTG), (PORTE_BIT15 | PORTE_BIT14 | PORTE_BIT13 | PORTE_BIT12 | PORTE_BIT11 | PORTE_BIT10 | PORTE_BIT9 | PORTE_BIT8 | PORTE_BIT7), (OUTPUT_FAST | OUTPUT_PUSH_PULL)); \
                                   _CONFIG_PERIPHERAL_OUTPUT(F, (PERIPHERAL_FSMC_SDIO_OTG), (PORTF_BIT0), (OUTPUT_FAST | OUTPUT_PUSH_PULL)); \
                                   _CONFIG_PERIPHERAL_OUTPUT(G, (PERIPHERAL_FSMC_SDIO_OTG), (PORTG_BIT10), (OUTPUT_FAST | OUTPUT_PUSH_PULL)); \
                                   POWER_UP(AHB3, RCC_AHB3ENR_FSMCEN); \
                                   FSMC_BCR3 = (FSMC_BCR_MTYP_SRAM_ROM | FSMC_BCR_MWD_16 | FSMC_BCR_WAITPOL_LOW | FSMC_BCR_WREN); \
                                   SET_MEMORY_TIMING(3, LCD_ADDRESS_SETUP_TIME_HCLK, LCD_ADDRESS_HOLD_TIME_HCLK, LCD_DATA_SETUP_TIME_HCLK, LCD_BUS_TURNAROUND_DURATION_HCLK, LCD_CLK_PRESCALE_HCLK, LCD_DATA_LATENCY_CLK, A); \
                                   FSMC_BCR3 = (FSMC_BCR_MTYP_SRAM_ROM | FSMC_BCR_MWD_16 | FSMC_BCR_WAITPOL_LOW | FSMC_BCR_WREN | FSMC_BCR_MBKEN);
                                   // After configuring the FSMC module a delay of 50ms is expected before its use - this is controlled by delaying the start of the application
                                   //
    #define ENABLE_BACKLIGHT()

    #define GLCD_DATAASOUTPUT()    GPIODIR_F = 0xff 
    #define GLCD_DATAASINPUT()     GPIODIR_F = 0x00 

    #define GLCD_DATAOUT(data)     GPIODATA_F = data  
    #define GLCD_DATAIN()          GPIODATA_F

    #define GLCD_WR_H()            GPIODATA_G |= GLCD_WR
    #define GLCD_WR_L()            GPIODATA_G &= ~(GLCD_WR)

    #define GLCD_CD_H()          //GPIODATA_D |= GLCD_C_D
    #define GLCD_CD_L()            GPIODATA_D &= ~(GLCD_C_D)

    #define GLCD_RD_H()            GPIODATA_G |= GLCD_RD
    #define GLCD_RD_L()            GPIODATA_G &= ~(GLCD_RD)

    #define GLCD_DELAY_WRITE()     //GLCD_WR_L()                         // no write delay since the data is stable for long enough at full speed
    #define GLCD_DELAY_READ()      GLCD_RD_L()                           // one extra operation to ensure set up time of read

    #define GLCD_RST_H()           GPIODATA_G |= GLCD_RST 
    #define GLCD_RST_L()           GPIODATA_G &= ~(GLCD_RST)
    #if defined _WINDOWS
        #define MAX_GLCD_WRITE_BURST   10000                             // the maximum number of writes to the GLCD before the task yields
    #else
        #define MAX_GLCD_WRITE_BURST   20                                // the maximum number of writes to the GLCD before the task yields
    #endif

    #define SUPPORT_TOUCH_SCREEN                                         // touch screen operation via I2C touch controller and EXTI interrupt on PB14
    #define RESET_IRQ_LINE()       fnSimulateInputChange(PORTB, (15 - 14), SET_INPUT);
    #define GLCD_BACKLIGHT_CONTROL                                       // control TFT backlight using PWM - needs new connection

    #define BACK_LIGHT_MAX_INTENSITY()                                          
    #define BACK_LIGHT_MIN_INTENSITY()
    #define _GLCD_BACKLIGHT_TIMER            3                           // generate backlight control signal with timer 3
    #define _GLCD_TIMER_MODE_OF_OPERATION    (TIMER_PWM_CH1)
    #define _GLCD_BACKLIGHT_PWM_FREQUENCY    TIMER_US_DELAY(TIMER_FREQUENCY_VALUE(1300)) // 1300 Hz backlight frequency (possible at faster CPU speed)
    #define TIMER_3_FULL_REMAP                                           // timer 3 ch1 = PC6, ch2 = PC7, ch3 = PC8, ch4= PC9

#elif defined STM3210C_EVAL && defined SUPPORT_GLCD                      // port F used a data lines, configured as inputs, port D0 used as C/D, port G 5,6,7 used as RST, RD and WR
    #define GLCD_RST               PORTG_BIT5
    #define GLCD_C_D               PORTD_BIT0
    #define GLCD_WR                PORTG_BIT7
    #define GLCD_RD                PORTG_BIT6

    #define SET_PULL_DOWNS()       GPIOPDR_F = 0xff;
    #define REMOVE_PULL_DOWNS()    GPIOPDR_F = 0x00;
                                                                         // configure the CS line, enable GPIOC clock, remap pins, enable SPI3 clock, reset and configure SPI, enable SPI
    #define CONFIGURE_GLCD()       _CONFIG_PORT_OUTPUT(B, PORTB_BIT2, (OUTPUT_FAST | OUTPUT_PUSH_PULL)); _SETBITS(B, PORTB_BIT2); \
    RCC_APB2ENR |= (RCC_APB2ENR_AFIOEN | RCC_APB2ENR_IOPCEN); AFIO_MAPR |= SPI3_REMAP; RCC_APB1ENR |= (RCC_APB1ENR_SPI3EN); \
    _CONFIG_PORT_OUTPUT(C, (PORTC_BIT10 | PORTC_BIT11 | PORTC_BIT12), (ALTERNATIVE_FUNCTION | OUTPUT_FAST | OUTPUT_PUSH_PULL)); \
    RCC_APB1RSTR |= RCC_APB1RSTR_SPI3RST; RCC_APB1RSTR &= ~RCC_APB1RSTR_SPI3RST; \
    SPI3_CR1 = (SPICR1_BR_PCLK2_DIV2 | SPICR1_MSTR | SPICR1_SSI | SPICR1_CPOL | SPICR1_CPHA | SPICR1_SSM); \
    SPI3_I2SCFGR = 0; \
    SPI3_CR1 = (SPICR1_SPE | SPICR1_BR_PCLK2_DIV2 | SPICR1_MSTR | SPICR1_SSI | SPICR1_CPOL | SPICR1_CPHA | SPICR1_SSM);
    #define ENABLE_BACKLIGHT()

    #define GLCD_DATAASOUTPUT()    GPIODIR_F = 0xff 
    #define GLCD_DATAASINPUT()     GPIODIR_F = 0x00 

    #define GLCD_DATAOUT(data)     GPIODATA_F = data  
    #define GLCD_DATAIN()          GPIODATA_F

    #define GLCD_WR_H()            GPIODATA_G |= GLCD_WR
    #define GLCD_WR_L()            GPIODATA_G &= ~(GLCD_WR)

    #define GLCD_CD_H()          //GPIODATA_D |= GLCD_C_D
    #define GLCD_CD_L()            GPIODATA_D &= ~(GLCD_C_D)

    #define GLCD_RD_H()            GPIODATA_G |= GLCD_RD
    #define GLCD_RD_L()            GPIODATA_G &= ~(GLCD_RD)

    #define GLCD_DELAY_WRITE()     //GLCD_WR_L()                         // no write delay since the data is stable for long enough at full speed
    #define GLCD_DELAY_READ()      GLCD_RD_L()                           // one extra operation to ensure set up time of read

    #define GLCD_RST_H()           GPIODATA_G |= GLCD_RST 
    #define GLCD_RST_L()           GPIODATA_G &= ~(GLCD_RST)
    #if defined _WINDOWS
        #define MAX_GLCD_WRITE_BURST   10000                             // the maximum number of writes to the GLCD before the task yields
    #else
        #define MAX_GLCD_WRITE_BURST   20                                // the maximum number of writes to the GLCD before the task yields
    #endif

  //#define SUPPORT_TOUCH_SCREEN                                         // touch screen operation via I2C touch controller and EXTI interrupt on PB14
    #define RESET_IRQ_LINE()          fnSimulateInputChange(PORTB, (15 - 14), SET_INPUT);
    #define GLCD_BACKLIGHT_CONTROL                                       // control TFT backlight using PWM - needs new connection

    #define BACK_LIGHT_MAX_INTENSITY()                                          
    #define BACK_LIGHT_MIN_INTENSITY()
    #define _GLCD_BACKLIGHT_TIMER            3                           // generate backlight control signal with timer 3
    #define _GLCD_TIMER_MODE_OF_OPERATION    (TIMER_PWM_CH1)
    #define _GLCD_BACKLIGHT_PWM_FREQUENCY    TIMER_US_DELAY(TIMER_FREQUENCY_VALUE(1100)) // 1100 Hz backlight frequency
    #define TIMER_3_FULL_REMAP                                           // timer 3 ch1 = PC6, ch2 = PC7, ch3 = PC8, ch4= PC9
#endif


#define GPIO_DEFAULT_INPUT_A       0xffff                                // initial port input states for simulator
#define GPIO_DEFAULT_INPUT_B       0xffff
#define GPIO_DEFAULT_INPUT_C       0xffff
#define GPIO_DEFAULT_INPUT_D       0xffff
#define GPIO_DEFAULT_INPUT_E       0xfffe                                // set to 0xfffe to detect SD card on STM3210C_EVAL
#define GPIO_DEFAULT_INPUT_F       0xffff
#define GPIO_DEFAULT_INPUT_G       0xffff
#define GPIO_DEFAULT_INPUT_H       0xdfff                                // set to 0xdfff to detect SD card on STM3241G_EVAL
#define GPIO_DEFAULT_INPUT_I       0xffff
#define GPIO_DEFAULT_INPUT_J       0xffff
#define GPIO_DEFAULT_INPUT_K       0xffff

#if defined STM3241G_EVAL || defined WISDOM_STM32F407 || defined NUCLEO_F401RE || defined STM32F746G_DISCO // {2}
    // User port mapping
    //
    #define USER_PORT_1_BIT        PORTI_BIT0                            // use free pins on Eval board
    #define USER_PORT_2_BIT        PORTI_BIT1
    #define USER_PORT_3_BIT        PORTI_BIT2
    #define USER_PORT_4_BIT        PORTI_BIT3
    #define USER_PORT_5_BIT        PORTI_BIT4
    #define USER_PORT_6_BIT        PORTI_BIT5
    #define USER_PORT_7_BIT        PORTI_BIT6
    #define USER_PORT_8_BIT        PORTI_BIT7
    #define USER_PORT_9_BIT        PORTI_BIT8
    #define USER_PORT_10_BIT       PORTI_BIT9

    #define USER_PORT_11_BIT       PORTG_BIT0
    #define USER_PORT_12_BIT       PORTG_BIT1
    #define USER_PORT_13_BIT       PORTG_BIT2
    #define USER_PORT_14_BIT       PORTG_BIT3
    #define USER_PORT_15_BIT       PORTG_BIT4
    #define USER_PORT_16_BIT       PORTG_BIT5

    // Port use definitions
    //
    #define USER_PORT_1            GPIOI_ODR
    #define USER_PORT_2            GPIOI_ODR
    #define USER_PORT_3            GPIOI_ODR
    #define USER_PORT_4            GPIOI_ODR
    #define USER_PORT_5            GPIOI_ODR
    #define USER_PORT_6            GPIOI_ODR
    #define USER_PORT_7            GPIOI_ODR
    #define USER_PORT_8            GPIOI_ODR
    #define USER_PORT_9            GPIOI_ODR
    #define USER_PORT_10           GPIOI_ODR

    #define USER_PORT_11           GPIOG_ODR
    #define USER_PORT_12           GPIOG_ODR
    #define USER_PORT_13           GPIOG_ODR
    #define USER_PORT_14           GPIOG_ODR
    #define USER_PORT_15           GPIOG_ODR
    #define USER_PORT_16           GPIOG_ODR

    #define CONFIG_USER_PORT_1()   _CONFIG_PORT_OUTPUT(I, USER_PORT_1_BIT, (OUTPUT_FAST | OUTPUT_PUSH_PULL))
    #define CONFIG_USER_PORT_2()   _CONFIG_PORT_OUTPUT(I, USER_PORT_2_BIT, (OUTPUT_FAST | OUTPUT_PUSH_PULL))
    #define CONFIG_USER_PORT_3()   _CONFIG_PORT_OUTPUT(I, USER_PORT_3_BIT, (OUTPUT_FAST | OUTPUT_PUSH_PULL))
    #define CONFIG_USER_PORT_4()   _CONFIG_PORT_OUTPUT(I, USER_PORT_4_BIT, (OUTPUT_FAST | OUTPUT_PUSH_PULL))
    #define CONFIG_USER_PORT_5()   _CONFIG_PORT_OUTPUT(I, USER_PORT_5_BIT, (OUTPUT_FAST | OUTPUT_PUSH_PULL))
    #define CONFIG_USER_PORT_6()   _CONFIG_PORT_OUTPUT(I, USER_PORT_6_BIT, (OUTPUT_FAST | OUTPUT_PUSH_PULL))
    #define CONFIG_USER_PORT_7()   _CONFIG_PORT_OUTPUT(I, USER_PORT_7_BIT, (OUTPUT_FAST | OUTPUT_PUSH_PULL))
    #define CONFIG_USER_PORT_8()   _CONFIG_PORT_OUTPUT(I, USER_PORT_8_BIT, (OUTPUT_FAST | OUTPUT_PUSH_PULL))
    #define CONFIG_USER_PORT_9()   _CONFIG_PORT_OUTPUT(I, USER_PORT_9_BIT, (OUTPUT_FAST | OUTPUT_PUSH_PULL))
    #define CONFIG_USER_PORT_10()  _CONFIG_PORT_OUTPUT(I, USER_PORT_10_BIT, (OUTPUT_FAST | OUTPUT_PUSH_PULL))

    #define CONFIG_USER_PORT_11()  _CONFIG_PORT_OUTPUT(G, USER_PORT_11_BIT, (OUTPUT_FAST | OUTPUT_PUSH_PULL))
    #define CONFIG_USER_PORT_12()  _CONFIG_PORT_OUTPUT(G, USER_PORT_12_BIT, (OUTPUT_FAST | OUTPUT_PUSH_PULL))
    #define CONFIG_USER_PORT_13()  _CONFIG_PORT_OUTPUT(G, USER_PORT_13_BIT, (OUTPUT_FAST | OUTPUT_PUSH_PULL))
    #define CONFIG_USER_PORT_14()  _CONFIG_PORT_OUTPUT(G, USER_PORT_14_BIT, (OUTPUT_FAST | OUTPUT_PUSH_PULL))
    #define CONFIG_USER_PORT_15()  _CONFIG_PORT_OUTPUT(G, USER_PORT_15_BIT, (OUTPUT_FAST | OUTPUT_PUSH_PULL))
    #define CONFIG_USER_PORT_16()  _CONFIG_PORT_OUTPUT(G, USER_PORT_16_BIT, (OUTPUT_FAST | OUTPUT_PUSH_PULL))

    #define POWER_UP_USER_PORTS()  POWER_UP(AHB1, RCC_AHB1ENR_GPIOIEN); POWER_UP(AHB1, RCC_AHB1ENR_GPIOGEN);
#elif defined STM3210C_EVAL
    // User port mapping
    //
    #define USER_PORT_1_BIT        PORTE_BIT2                            // use free pins on Eval board
    #define USER_PORT_2_BIT        PORTE_BIT3
    #define USER_PORT_3_BIT        PORTE_BIT4
    #define USER_PORT_4_BIT        PORTE_BIT5
    #define USER_PORT_5_BIT        PORTE_BIT6

    #define USER_PORT_6_BIT        PORTE_BIT8
    #define USER_PORT_7_BIT        PORTE_BIT9
    #define USER_PORT_8_BIT        PORTE_BIT10
    #define USER_PORT_9_BIT        PORTE_BIT11
    #define USER_PORT_10_BIT       PORTE_BIT12
    #define USER_PORT_11_BIT       PORTE_BIT13

    #define USER_PORT_12_BIT       PORTA_BIT13
    #define USER_PORT_13_BIT       PORTD_BIT14
    #define USER_PORT_14_BIT       PORTD_BIT15

    #define USER_PORT_15_BIT       PORTB_BIT6
    #define USER_PORT_16_BIT       PORTB_BIT15

    // Port use definitions
    //
    #define USER_PORT_1            GPIOE_ODR
    #define USER_PORT_2            GPIOE_ODR
    #define USER_PORT_3            GPIOE_ODR
    #define USER_PORT_4            GPIOE_ODR
    #define USER_PORT_5            GPIOE_ODR
    #define USER_PORT_6            GPIOE_ODR

    #define USER_PORT_7            GPIOE_ODR
    #define USER_PORT_8            GPIOE_ODR
    #define USER_PORT_9            GPIOE_ODR
    #define USER_PORT_10           GPIOE_ODR
    #define USER_PORT_11           GPIOE_ODR

    #define USER_PORT_12           GPIOA_ODR
    #define USER_PORT_13           GPIOD_ODR
    #define USER_PORT_14           GPIOD_ODR

    #define USER_PORT_15           GPIOB_ODR
    #define USER_PORT_16           GPIOB_ODR

    #define CONFIG_USER_PORT_1()   _CONFIG_PORT_OUTPUT(E, USER_PORT_1_BIT, (OUTPUT_FAST | OUTPUT_PUSH_PULL))
    #define CONFIG_USER_PORT_2()   _CONFIG_PORT_OUTPUT(E, USER_PORT_2_BIT, (OUTPUT_FAST | OUTPUT_PUSH_PULL))
    #define CONFIG_USER_PORT_3()   _CONFIG_PORT_OUTPUT(E, USER_PORT_3_BIT, (OUTPUT_FAST | OUTPUT_PUSH_PULL))
    #define CONFIG_USER_PORT_4()   _CONFIG_PORT_OUTPUT(E, USER_PORT_4_BIT, (OUTPUT_FAST | OUTPUT_PUSH_PULL))
    #define CONFIG_USER_PORT_5()   _CONFIG_PORT_OUTPUT(E, USER_PORT_5_BIT, (OUTPUT_FAST | OUTPUT_PUSH_PULL))
    #define CONFIG_USER_PORT_6()   _CONFIG_PORT_OUTPUT(E, USER_PORT_6_BIT, (OUTPUT_FAST | OUTPUT_PUSH_PULL))
    #define CONFIG_USER_PORT_7()   _CONFIG_PORT_OUTPUT(E, USER_PORT_7_BIT, (OUTPUT_FAST | OUTPUT_PUSH_PULL))
    #define CONFIG_USER_PORT_8()   _CONFIG_PORT_OUTPUT(E, USER_PORT_8_BIT, (OUTPUT_FAST | OUTPUT_PUSH_PULL))
    #define CONFIG_USER_PORT_9()   _CONFIG_PORT_OUTPUT(E, USER_PORT_9_BIT, (OUTPUT_FAST | OUTPUT_PUSH_PULL))
    #define CONFIG_USER_PORT_10()  _CONFIG_PORT_OUTPUT(E, USER_PORT_10_BIT, (OUTPUT_FAST | OUTPUT_PUSH_PULL))
    #define CONFIG_USER_PORT_11()  _CONFIG_PORT_OUTPUT(E, USER_PORT_11_BIT, (OUTPUT_FAST | OUTPUT_PUSH_PULL))

    #define CONFIG_USER_PORT_12()  _CONFIG_PORT_OUTPUT(A, USER_PORT_12_BIT, (OUTPUT_FAST | OUTPUT_PUSH_PULL))
    #define CONFIG_USER_PORT_13()  _CONFIG_PORT_OUTPUT(D, USER_PORT_13_BIT, (OUTPUT_FAST | OUTPUT_PUSH_PULL))
    #define CONFIG_USER_PORT_14()  _CONFIG_PORT_OUTPUT(D, USER_PORT_14_BIT, (OUTPUT_FAST | OUTPUT_PUSH_PULL))

    #define CONFIG_USER_PORT_15()  _CONFIG_PORT_OUTPUT(B, USER_PORT_15_BIT, (OUTPUT_FAST | OUTPUT_PUSH_PULL))
    #define CONFIG_USER_PORT_16()  _CONFIG_PORT_OUTPUT(B, USER_PORT_16_BIT, (OUTPUT_FAST | OUTPUT_PUSH_PULL))

    #define POWER_UP_USER_PORTS()  POWER_UP(APB2, (RCC_APB2ENR_IOPBEN)); POWER_UP(APB2, (RCC_APB2ENR_IOPDEN)); POWER_UP(APB2, (RCC_APB2ENR_IOPEEN))
#elif defined ST_MB997A_DISCOVERY
    // User port mapping
    //
    #define USER_PORT_1_BIT        PORTB_BIT0                            // use free PB pins on Eval board
    #define USER_PORT_2_BIT        PORTB_BIT1
    #define USER_PORT_3_BIT        PORTB_BIT2
    #define USER_PORT_4_BIT        PORTB_BIT3
    #define USER_PORT_5_BIT        PORTB_BIT4
    #define USER_PORT_6_BIT        PORTB_BIT5
    #define USER_PORT_7_BIT        PORTB_BIT6
    #define USER_PORT_8_BIT        PORTB_BIT7
    #define USER_PORT_9_BIT        PORTB_BIT8
    #define USER_PORT_10_BIT       PORTB_BIT9
    #define USER_PORT_11_BIT       PORTB_BIT10
    #if defined EMBEST_BASE_BOARD                                        // {6} avoid some pins on port B so that they can be used for Ethernet RMII interface
        #define USER_PORT_12_BIT   PORTE_BIT11
        #define USER_PORT_13_BIT   PORTE_BIT12
        #define USER_PORT_14_BIT   PORTE_BIT13
    #else
        #define USER_PORT_12_BIT   PORTB_BIT11
        #define USER_PORT_13_BIT   PORTB_BIT12
        #define USER_PORT_14_BIT   PORTB_BIT13
    #endif
    #define USER_PORT_15_BIT       PORTB_BIT14
    #if defined EMBEST_BASE_BOARD                                        // {6} avoid pin on port B so that it can be used for SD card detect input
        #define USER_PORT_16_BIT   PORTE_BIT15
    #else
        #define USER_PORT_16_BIT   PORTB_BIT15
    #endif

    // Port use definitions
    //
    #define USER_PORT_1            GPIOB_ODR
    #define USER_PORT_2            GPIOB_ODR
    #define USER_PORT_3            GPIOB_ODR
    #define USER_PORT_4            GPIOB_ODR
    #define USER_PORT_5            GPIOB_ODR
    #define USER_PORT_6            GPIOB_ODR
    #define USER_PORT_7            GPIOB_ODR
    #define USER_PORT_8            GPIOB_ODR
    #define USER_PORT_9            GPIOB_ODR
    #define USER_PORT_10           GPIOB_ODR
    #define USER_PORT_11           GPIOB_ODR
    #if defined EMBEST_BASE_BOARD                                        // {6} avoid some pins on port B so that they can be used for Ethernet RMII interface
        #define USER_PORT_12       GPIOE_ODR
        #define USER_PORT_13       GPIOE_ODR
        #define USER_PORT_14       GPIOE_ODR
    #else
        #define USER_PORT_12       GPIOB_ODR
        #define USER_PORT_13       GPIOB_ODR
        #define USER_PORT_14       GPIOB_ODR
    #endif
    #define USER_PORT_15           GPIOB_ODR
    #if defined EMBEST_BASE_BOARD                                        // {6} avoid pin on port B so that it can be used for SD card detect input
        #define USER_PORT_16       GPIOE_ODR
    #else
        #define USER_PORT_16       GPIOB_ODR
    #endif

    #define CONFIG_USER_PORT_1()   _CONFIG_PORT_OUTPUT(B, USER_PORT_1_BIT, (OUTPUT_FAST | OUTPUT_PUSH_PULL))
    #define CONFIG_USER_PORT_2()   _CONFIG_PORT_OUTPUT(B, USER_PORT_2_BIT, (OUTPUT_FAST | OUTPUT_PUSH_PULL))
    #define CONFIG_USER_PORT_3()   _CONFIG_PORT_OUTPUT(B, USER_PORT_3_BIT, (OUTPUT_FAST | OUTPUT_PUSH_PULL))
    #define CONFIG_USER_PORT_4()   _CONFIG_PORT_OUTPUT(B, USER_PORT_4_BIT, (OUTPUT_FAST | OUTPUT_PUSH_PULL))
    #define CONFIG_USER_PORT_5()   _CONFIG_PORT_OUTPUT(B, USER_PORT_5_BIT, (OUTPUT_FAST | OUTPUT_PUSH_PULL))
    #define CONFIG_USER_PORT_6()   _CONFIG_PORT_OUTPUT(B, USER_PORT_6_BIT, (OUTPUT_FAST | OUTPUT_PUSH_PULL))
    #define CONFIG_USER_PORT_7()   _CONFIG_PORT_OUTPUT(B, USER_PORT_7_BIT, (OUTPUT_FAST | OUTPUT_PUSH_PULL))
    #define CONFIG_USER_PORT_8()   _CONFIG_PORT_OUTPUT(B, USER_PORT_8_BIT, (OUTPUT_FAST | OUTPUT_PUSH_PULL))
    #define CONFIG_USER_PORT_9()   _CONFIG_PORT_OUTPUT(B, USER_PORT_9_BIT, (OUTPUT_FAST | OUTPUT_PUSH_PULL))
    #define CONFIG_USER_PORT_10()  _CONFIG_PORT_OUTPUT(B, USER_PORT_10_BIT, (OUTPUT_FAST | OUTPUT_PUSH_PULL))
    #define CONFIG_USER_PORT_11()  _CONFIG_PORT_OUTPUT(B, USER_PORT_11_BIT, (OUTPUT_FAST | OUTPUT_PUSH_PULL))
    #if defined EMBEST_BASE_BOARD                                        // {6} avoid some pins on port B so that they can be used for Ethernet RMII interface
        #define CONFIG_USER_PORT_12()  _CONFIG_PORT_OUTPUT(E, USER_PORT_12_BIT, (OUTPUT_FAST | OUTPUT_PUSH_PULL))
        #define CONFIG_USER_PORT_13()  _CONFIG_PORT_OUTPUT(E, USER_PORT_13_BIT, (OUTPUT_FAST | OUTPUT_PUSH_PULL))
        #define CONFIG_USER_PORT_14()  _CONFIG_PORT_OUTPUT(E, USER_PORT_14_BIT, (OUTPUT_FAST | OUTPUT_PUSH_PULL))
    #else
        #define CONFIG_USER_PORT_12()  _CONFIG_PORT_OUTPUT(B, USER_PORT_12_BIT, (OUTPUT_FAST | OUTPUT_PUSH_PULL))
        #define CONFIG_USER_PORT_13()  _CONFIG_PORT_OUTPUT(B, USER_PORT_13_BIT, (OUTPUT_FAST | OUTPUT_PUSH_PULL))
        #define CONFIG_USER_PORT_14()  _CONFIG_PORT_OUTPUT(B, USER_PORT_14_BIT, (OUTPUT_FAST | OUTPUT_PUSH_PULL))
    #endif
    #define CONFIG_USER_PORT_15()      _CONFIG_PORT_OUTPUT(B, USER_PORT_15_BIT, (OUTPUT_FAST | OUTPUT_PUSH_PULL))
    #if defined EMBEST_BASE_BOARD                                        // {6} avoid pin on port B so that it can be used for SD card detect input
        #define CONFIG_USER_PORT_16()  _CONFIG_PORT_OUTPUT(E, USER_PORT_16_BIT, (OUTPUT_FAST | OUTPUT_PUSH_PULL))
    #else
        #define CONFIG_USER_PORT_16()  _CONFIG_PORT_OUTPUT(B, USER_PORT_16_BIT, (OUTPUT_FAST | OUTPUT_PUSH_PULL))
    #endif

    #define POWER_UP_USER_PORTS()  POWER_UP(AHB1, RCC_AHB1ENR_GPIOBEN)
#elif defined ST_MB913C_DISCOVERY || defined ARDUINO_BLUE_PILL
    // User port mapping
    //
    #define USER_PORT_1_BIT        PORTB_BIT0                            // use free PB pins on Eval board
    #define USER_PORT_2_BIT        PORTB_BIT1
    #define USER_PORT_3_BIT        PORTB_BIT2
    #define USER_PORT_4_BIT        PORTB_BIT3
    #define USER_PORT_5_BIT        PORTB_BIT4
    #define USER_PORT_6_BIT        PORTB_BIT5
    #define USER_PORT_7_BIT        PORTB_BIT6
    #define USER_PORT_8_BIT        PORTB_BIT7
    #define USER_PORT_9_BIT        PORTB_BIT8
    #define USER_PORT_10_BIT       PORTB_BIT9
    #define USER_PORT_11_BIT       PORTB_BIT10
    #define USER_PORT_12_BIT       PORTB_BIT11
    #define USER_PORT_13_BIT       PORTB_BIT12
    #define USER_PORT_14_BIT       PORTB_BIT13
    #define USER_PORT_15_BIT       PORTB_BIT14
    #define USER_PORT_16_BIT       PORTB_BIT15

    // Port use definitions
    //
    #define USER_PORT_1            GPIOB_ODR
    #define USER_PORT_2            GPIOB_ODR
    #define USER_PORT_3            GPIOB_ODR
    #define USER_PORT_4            GPIOB_ODR
    #define USER_PORT_5            GPIOB_ODR
    #define USER_PORT_6            GPIOB_ODR
    #define USER_PORT_7            GPIOB_ODR
    #define USER_PORT_8            GPIOB_ODR
    #define USER_PORT_9            GPIOB_ODR
    #define USER_PORT_10           GPIOB_ODR
    #define USER_PORT_11           GPIOB_ODR
    #define USER_PORT_12           GPIOB_ODR
    #define USER_PORT_13           GPIOB_ODR
    #define USER_PORT_14           GPIOB_ODR
    #define USER_PORT_15           GPIOB_ODR
    #define USER_PORT_16           GPIOB_ODR

    #define CONFIG_USER_PORT_1()   _CONFIG_PORT_OUTPUT(B, USER_PORT_1_BIT, (OUTPUT_FAST | OUTPUT_PUSH_PULL))
    #define CONFIG_USER_PORT_2()   _CONFIG_PORT_OUTPUT(B, USER_PORT_2_BIT, (OUTPUT_FAST | OUTPUT_PUSH_PULL))
    #define CONFIG_USER_PORT_3()   _CONFIG_PORT_OUTPUT(B, USER_PORT_3_BIT, (OUTPUT_FAST | OUTPUT_PUSH_PULL))
    #define CONFIG_USER_PORT_4()   _CONFIG_PORT_OUTPUT(B, USER_PORT_4_BIT, (OUTPUT_FAST | OUTPUT_PUSH_PULL))
    #define CONFIG_USER_PORT_5()   _CONFIG_PORT_OUTPUT(B, USER_PORT_5_BIT, (OUTPUT_FAST | OUTPUT_PUSH_PULL))
    #define CONFIG_USER_PORT_6()   _CONFIG_PORT_OUTPUT(B, USER_PORT_6_BIT, (OUTPUT_FAST | OUTPUT_PUSH_PULL))
    #define CONFIG_USER_PORT_7()   _CONFIG_PORT_OUTPUT(B, USER_PORT_7_BIT, (OUTPUT_FAST | OUTPUT_PUSH_PULL))
    #define CONFIG_USER_PORT_8()   _CONFIG_PORT_OUTPUT(B, USER_PORT_8_BIT, (OUTPUT_FAST | OUTPUT_PUSH_PULL))
    #define CONFIG_USER_PORT_9()   _CONFIG_PORT_OUTPUT(B, USER_PORT_9_BIT, (OUTPUT_FAST | OUTPUT_PUSH_PULL))
    #define CONFIG_USER_PORT_10()  _CONFIG_PORT_OUTPUT(B, USER_PORT_10_BIT, (OUTPUT_FAST | OUTPUT_PUSH_PULL))
    #define CONFIG_USER_PORT_11()  _CONFIG_PORT_OUTPUT(B, USER_PORT_11_BIT, (OUTPUT_FAST | OUTPUT_PUSH_PULL))
    #define CONFIG_USER_PORT_12()  _CONFIG_PORT_OUTPUT(B, USER_PORT_12_BIT, (OUTPUT_FAST | OUTPUT_PUSH_PULL))
    #define CONFIG_USER_PORT_13()  _CONFIG_PORT_OUTPUT(B, USER_PORT_13_BIT, (OUTPUT_FAST | OUTPUT_PUSH_PULL))
    #define CONFIG_USER_PORT_14()  _CONFIG_PORT_OUTPUT(B, USER_PORT_14_BIT, (OUTPUT_FAST | OUTPUT_PUSH_PULL))
    #define CONFIG_USER_PORT_15()  _CONFIG_PORT_OUTPUT(B, USER_PORT_15_BIT, (OUTPUT_FAST | OUTPUT_PUSH_PULL))
    #define CONFIG_USER_PORT_16()  _CONFIG_PORT_OUTPUT(B, USER_PORT_16_BIT, (OUTPUT_FAST | OUTPUT_PUSH_PULL))

    #define POWER_UP_USER_PORTS()  POWER_UP(APB2, (RCC_APB2ENR_IOPBEN))
#endif



#if defined KEY_COLUMNS && (KEY_COLUMNS > 0)
    // Keypad (scan out D4..D7, scan in E0..E3)
    //
    #define KEY_ROW_IN_1           PORTE_BIT0
    #define KEY_ROW_IN_2           PORTE_BIT1
    #define KEY_ROW_IN_3           PORTE_BIT2
    #define KEY_ROW_IN_4           PORTE_BIT3

    #define KEY_ROW_IN_PORT_1_REF  E
    #define KEY_ROW_IN_PORT_2_REF  E
    #define KEY_ROW_IN_PORT_3_REF  E
    #define KEY_ROW_IN_PORT_4_REF  E

    #define KEY_COL_OUT_1          PORTD_BIT4
    #define KEY_COL_OUT_2          PORTD_BIT5
    #define KEY_COL_OUT_3          PORTD_BIT6
    #define KEY_COL_OUT_4          PORTD_BIT7

    #define KEY_COL_OUT_PORT_1     GPIODATA_D
    #define KEY_COL_OUT_DDR_1      GPIODIR_D
    #define KEY_COL_OUT_PORT_2     GPIODATA_D
    #define KEY_COL_OUT_DDR_2      GPIODIR_D
    #define KEY_COL_OUT_PORT_3     GPIODATA_D
    #define KEY_COL_OUT_DDR_3      GPIODIR_D
    #define KEY_COL_OUT_PORT_4     GPIODATA_D
    #define KEY_COL_OUT_DDR_4      GPIODIR_D

                                                                         // drive each column low
    #define DRIVE_COLUMN_1()       _DRIVE_PORT_OUTPUT_VALUE(D, KEY_COL_OUT_1, 0) // drive output low (column 1)
    #define DRIVE_COLUMN_2()       _DRIVE_PORT_OUTPUT_VALUE(D, KEY_COL_OUT_2, 0) // drive output low (column 2)
    #define DRIVE_COLUMN_3()       _DRIVE_PORT_OUTPUT_VALUE(D, KEY_COL_OUT_3, 0) // drive output low (column 3)
    #define DRIVE_COLUMN_4()       _DRIVE_PORT_OUTPUT_VALUE(D, KEY_COL_OUT_4, 0) // drive output low (column 4)
                                                                         // drive high (to avoid slow rise time) then set back as input
    #define RELEASE_COLUMN_1()     _SETBITS(D, KEY_COL_OUT_1); _FLOAT_PORT(D, KEY_COL_OUT_1) 
    #define RELEASE_COLUMN_2()     _SETBITS(D, KEY_COL_OUT_2); _FLOAT_PORT(D, KEY_COL_OUT_2)
    #define RELEASE_COLUMN_3()     _SETBITS(D, KEY_COL_OUT_3); _FLOAT_PORT(D, KEY_COL_OUT_3)
    #define RELEASE_COLUMN_4()     _SETBITS(D, KEY_COL_OUT_4); _FLOAT_PORT(D, KEY_COL_OUT_4)

    #define KEY_ROW_IN_PORT_1      GPIODATA_E
    #define KEY_ROW_IN_PORT_2      GPIODATA_E
    #define KEY_ROW_IN_PORT_3      GPIODATA_E
    #define KEY_ROW_IN_PORT_4      GPIODATA_E
                                                                         // reset any changes ready for next scan sequence
    #define RESET_SCAN()         

    #define INIT_KEY_SCAN()        _CONFIG_PORT_INPUT(E, (KEY_ROW_IN_1 | KEY_ROW_IN_2 | KEY_ROW_IN_3 | KEY_ROW_IN_4)); \
                                   GPIOPUR_E |= (KEY_ROW_IN_1 | KEY_ROW_IN_2 | KEY_ROW_IN_3 | KEY_ROW_IN_4); \
                                   _CONFIG_PORT_INPUT(D, (KEY_COL_OUT_1 | KEY_COL_OUT_2 | KEY_COL_OUT_3 | KEY_COL_OUT_4));
#else                                                                // not matrix scanning input
    #define KEY_IN_1               PORTG_BIT0
    #define KEY_IN_2               PORTG_BIT1
    #define KEY_IN_3               PORTG_BIT2
    #define KEY_IN_4               PORTG_BIT3
    #define KEY_IN_5               PORTG_BIT4
    #define KEY_IN_6               PORTG_BIT5
    #define KEY_IN_7               PORTG_BIT6
    #define KEY_IN_8               PORTG_BIT7

    #define INIT_KEY_STATE         (KEY_IN_1 | KEY_IN_2 | KEY_IN_3 | KEY_IN_4 | KEY_IN_5 | KEY_IN_6 | KEY_IN_7 | KEY_IN_8) // all keys are expected to be pulled up at start

    #define INIT_KEY_SCAN()        _CONFIG_PORT_INPUT(G, (KEY_IN_1 | KEY_IN_2 | KEY_IN_3 | KEY_IN_4 | KEY_IN_5 | KEY_IN_6 | KEY_IN_7 | KEY_IN_8)); GPIOPUR_G |= (KEY_IN_1 | KEY_IN_2 | KEY_IN_3 | KEY_IN_4 | KEY_IN_5 | KEY_IN_6 | KEY_IN_7 | KEY_IN_8)
    #define READ_KEY_INPUTS()      _READ_PORT(G)


    #define KEY_1_PORT_REF         G                                     // to allow simulator to map key pad to inputs
    #define KEY_1_PORT_PIN         KEY_IN_1
    #define KEY_5_PORT_REF         G
    #define KEY_5_PORT_PIN         KEY_IN_2
    #define KEY_9_PORT_REF         G
    #define KEY_9_PORT_PIN         KEY_IN_3
    #define KEY_13_PORT_REF        G
    #define KEY_13_PORT_PIN        KEY_IN_4

    #define KEY_2_PORT_REF         G
    #define KEY_2_PORT_PIN         KEY_IN_5
    #define KEY_6_PORT_REF         G
    #define KEY_6_PORT_PIN         KEY_IN_6
    #define KEY_10_PORT_REF        G
    #define KEY_10_PORT_PIN        KEY_IN_7
    #define KEY_14_PORT_REF        G
    #define KEY_14_PORT_PIN        KEY_IN_8
#endif

// LCD interface: Backlight control PORTE_BIT8 : Data bus (4 Bit) PORTE_BIT7..PORTE_BIT4 / (8 bit) PORTE_BIT7..PORTE_BIT0 : RS PORTE_BIT9, RW PORTE_BIT10, E PORTE_BIT11
//
typedef unsigned short LCD_BUS_PORT_SIZE;                                // we use 16 bit ports
typedef unsigned short LCD_CONTROL_PORT_SIZE;
//#define LCD_BUS_8BIT                                                   // data bus in 8 bit mode
#define LCD_BUS_4BIT                                                     // data bus in 4 bit mode

#if defined LCD_BUS_8BIT
    #define LCD_BUS_MASK           0xff
    #define DATA_SHIFT_RIGHT       0
    #define DATA_SHIFT_LEFT        0                                     // byte shift down required to bring data into position
#else
    #define LCD_BUS_MASK           0xf0
    #define DATA_SHIFT_RIGHT       0
    #define DATA_SHIFT_LEFT        0                                     // nibble shift down required to bring data into position
#endif

#define O_CONTROL_RS               PORTE_BIT9
#define O_WRITE_READ               PORTE_BIT10
#define O_CONTROL_EN               PORTE_BIT11
#define O_LCD_BACKLIGHT            PORTE_BIT8

#define O_CONTROL_LINES            (O_CONTROL_RS | O_WRITE_READ | O_CONTROL_EN)
#define IO_BUS_PORT_DAT            GPIOE_ODR
#define IO_BUS_PORT_DAT_IN         GPIOE_IDR
#define O_CONTROL_PORT_DAT         GPIOE_ODR

#define SET_DATA_LINES_INPUT()     _FLOAT_PORT(E, LCD_BUS_MASK)
#define SET_DATA_LINES_OUTPUT()    _DRIVE_PORT_OUTPUT(E, LCD_BUS_MASK)
#define SET_BUS_DATA(x)            _WRITE_PORT(E, (x))
#define O_SET_CONTROL_LOW(x)       _CLEARBITS(E, (x))
#define O_SET_CONTROL_HIGH(x)      _SETBITS(E, (x))

// Drive the control lines R/W + LCD Backlight '1', RS + E '0' and the data lines with all high impedance at start up
// 
#define INITIALISE_LCD_CONTROL_LINES() _CONFIG_DRIVE_PORT_OUTPUT_VALUE(E, (O_CONTROL_LINES | O_LCD_BACKLIGHT), (OUTPUT_MEDIUM | OUTPUT_PUSH_PULL), (O_LCD_BACKLIGHT)); \
                                       _CONFIG_PORT_INPUT(E, LCD_BUS_MASK, FLOATING_INPUT);

#define LCD_DRIVE_DATA()           SET_DATA_LINES_OUTPUT();  SET_DATA_LINES_OUTPUT();
                                   // ensure data bus outputs (delay) by repetitions according to processor speed

#define CLOCK_EN_HIGH()            O_SET_CONTROL_HIGH(O_CONTROL_EN); O_SET_CONTROL_HIGH(O_CONTROL_EN); _SIM_PORTS;
                                   // clock EN to high state - repeat to slow down (delay)

#define DELAY_ENABLE_CLOCK_HIGH()  _CLEARBITS(E, (O_CONTROL_EN))

#define SET_CONTROL_LINES(x)       _WRITE_PORT_MASK(E, x, O_CONTROL_LINES)



#define LCD_CONTRAST_CONTROL                                             // use electronic contrast control
#define SUPPORT_PWM_MODULE                                               // enable PWM module support for contrast and backlight control
    #define _LCD_CONTRAST_TIMER    3                                     // generate backlight control signal with timer 3
    #define _LCD_CONTRAST_TIMER_MODE_OF_OPERATION (TIMER_PWM_CH1)
    #define _LCD_CONTRAST_PWM_FREQUENCY  TIMER_US_DELAY(TIMER_FREQUENCY_VALUE(10000)) // 10000 Hz contrast frequency

#define LCD_BACKLIGHT_CONTROL                                            // use electronic backlight control
    #define _LCD_BACKLIGHT_TIMER   3                                     // generate backlight control signal with timer 3
    #define _LCD_BACKLIGHT_TIMER_MODE_OF_OPERATION (TIMER_PWM_CH2)
    #define _LCD_BACKLIGHT_PWM_FREQUENCY  TIMER_US_DELAY(TIMER_FREQUENCY_VALUE(10000)) // 10000 Hz backlight frequency
#endif                                                                   // endif - defined _STM32 && !defined __APP_HW_STM32__
