/***********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher

    ---------------------------------------------------------------------
    File:      app_hw_kinetis.h
    Project:   uTasker project
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2016
    *********************************************************************
    03.03.2012 Add TWR_K70F120M and defined TWR_K53N512 support
    12.03.2012 Add ADC setup                                             {1}
    15.04.2012 Add TWR_K20D50M                                           {2}
    17.04.2012 Add touch sensor inputs                                   {3}
    17.04.2012 Add DMA channel and priority configuration                {4}
    21.04.2012 File system in internal Flash set to start at 140k and have 372k size {5}
    29.04.2012 Enable caching with FPU parts and add LAN_TX_FPU_WORKAROUND to workaround an Ethernet problem that was stopping this from operating correctly before {6}
    22.07.2012 Add SUPPORT_PROGRAM_ONCE                                  {7}
    10.08.2012 Add SD card detection input                               {8}
    11.08.2012 Add Emcraft K70F120 and K61F150 boards                    {9}
    13.08.2012 Add MAC_FROM_USER_REG to allow management of MAC in OTP Flash {10}
    13.08.2012 Add support for magic reset Ethernet reception frame      {11}
    26.10.2012 Change READ_SPI_DATA() to use volatile access (avoid loss due to optimisation) {12}
    25.01.2013 Change user outputs to avoid UART3 pins                   {13}
    28.05.2013 Add SDRAM controller                                      {14}
    03.06.2013 Add DAC                                                   {15}
    09.08.2013 Add NET_KBED and NET_K60                                  {16}
    15.08.2013 Add SER-2 serial tower board                              {17}
    21.08.2013 Correct matrix keyboard port (simulator)                  {18}
    27.10.2013 Add PDB interface                                         {19}
    26.12.2013 Add RANDOM_NUMBER_GENERATOR_B for devices without RNGA    {20}
    16.01.2014 Add FRDM_K20D50M                                          {21}
    26.01.2014 Add FRDM_KL46Z                                            {22}
    28.01.2014 Add TWR_KL46Z48M                                          {23}
    30.01.2014 Add FRDM_KL25Z, FRDM_KL26Z  and TWR_KL25Z48M              {24}
    22.02.2014 Add FRDM_KL02Z, FRDM_KL05Z and FRDM_KE02Z                 {25}
    24.02.2014 Add LED state when driving pin is an input to all configurations
    08.04.2014 Add FRDM_K64F and TWR_K64F120M                            {26}
    09.06.2014 Add Crossbar Switch configuration                         {27}
    15.06.2014 Add low power timer                                       {28}
    25.06.2014 Add USB configuration using PLL1                          {29}
    12.07.2014 Add FRDM_KE02Z40M, FRDM_KE04Z and FRDM_KE06Z              {30}
    28.11.2014 Add FRDM_KL03Z and FRDM_KL43Z
    11.01.2015 Add TWR_K24F120M
    17.03.2015 Add FRDM_KL27Z
    18.04.2015 Add CAPUCCINO_KL27
    09.05.2015 Add TRK_KEA128, TRK_KEA64 and TRK_KEA8
    23.05.2015 Add TWR_K65F180M
    03.12.2015 Add TWR_K80F150M
    06.01.2016 Add FRDM_K82F
    21.07.2016 Add FRDM_KEAZN32Q64, FRDM_KEAZ64Q64 and FRDM_KEAZ128Q80

    22.11.2016 Add TEENSY_3_5 and TEENSY_3_6

    Application specific hardware configuration
*/

#if defined _KINETIS && !defined __APP_HW_KINETIS__
#define __APP_HW_KINETIS__
#if defined KINETIS_K_FPU || defined K02F100M || defined TWR_K20D50M || defined TWR_K21D50M || defined FRDM_K20D50M || defined tinyK20 || defined KINETIS_KL || defined KINETIS_KE || defined KINETIS_KV || defined KINETIS_KW2X // newer devices have these errate solved
    #define ERRATA_E2583_SOLVED                                          // in early silicon the CAN controllers only work when the OSC is enabled (enable if the chip revision used doesn't suffer from the problem)
    #define ERRATA_E2644_SOLVED                                          // early devices without flex memory doesn't support speculation logic and this should be disabled
    #define ERRATE_E2647_SOLVED                                          // early 512k and 384k flash-only devices don't support cache aliasing and this needs to be disabled
    #define ERRATE_E2448_SOLVED                                          // early devices can have a flash pre-fetch problem when writing the SIM_DIV1 register during the clock change sequence
    #define ERRATE_E3402_SOLVED                                          // early devices cannot use the XTAL pin as GPIO when the ERCLKEN bit is enabled (simulator blocks its operation in this case)
    #if !defined FRDM_K64F && !defined TWR_K64F120M && !defined FreeLON
        #define LAN_TX_FPU_WORKAROUND                                    // {6} when ERRATE_E2647_SOLVED is enabled it is found that the Ethernet Tx can miss a waiting frame in an output buffer. This workaround enables the transmit frame interrupt, which retriggers buffer polling
    #endif
    #define ERRATE_E2776_SOLVED                                          // write accesses to the CRC should always be 32-bit when transpose is enabled
#else                                                                    // when using older devices they can be set accoring to the revision used
    #define ERRATA_E2583_SOLVED                                          // in early silicon the CAN controllers only work when the OSC is enabled (enable if the chip revision used doesn't suffer from the problem)
  //#define ERRATA_E2644_SOLVED                                          // early devices without flex memory doesn't support speculation logic and this should be disabled
  //#define ERRATE_E2647_SOLVED                                          // early 512k and 384k flash-only devices don't support cache aliasing and this needs to be disabled
  //#define ERRATE_E2448_SOLVED                                          // early devices can have a flash pre-fetch problem when writing the SIM_DIV1 register during the clock change sequence
  //#define ERRATE_E3402_SOLVED                                          // early devices cannot use the XTAL pin as GPIO when the ERCLKEN bit is enabled (simulator blocks its operation in this case)
  //#define ERRATE_E2776_SOLVED                                          // write accesses to the CRC should always be 32-bit when transpose is enabled
#endif


// Define clock settings
//
#if defined NET_KBED || defined NET_K60                                  // {16} KBED or NETK60
    #if !defined _BMLOADER
        #define USER_STARTUP_CODE	PHY_RESET_20MS                       // the processor needs to be clocked form the external PHY, which is initially in reset - the PHY is taken out of reset and allowed to stabilise before switching to this clock
    #endif
    #define EXTERNAL_CLOCK       50000000                                // this must be 50MHz in order to use Ethernet in RMII mode
    #define _EXTERNAL_CLOCK      EXTERNAL_CLOCK
    #if defined USB_INTERFACE                                            // when using USB generate 96MHz clock so that a 48Mhz clock can be generated from it
        #define CLOCK_DIV        25                                      // input must be divided to 2MHz..4MHz range (/1 to /25 possible)
        #define CLOCK_MUL        48                                      // the PLL multiplication factor to achieve operating frequency of 100MHz (x24 to x55 possible)
    #else
        #define CLOCK_DIV        16                                      // input must be divided to 2MHz..4MHz range (/1 to /25 possible)
        #define CLOCK_MUL        32                                      // the PLL multiplication factor to achieve operating frequency of 100MHz (x24 to x55 possible)
    #endif
    #define USB_CLOCK_GENERATED_INTERNALLY                               // use USB clock from internal source rather than external pin
#elif defined EMCRAFT_K61F150M                                           // {9}
    #define EXTERNAL_CLOCK       50000000                                // this must be 50MHz in order to use Ethernet in RMII mode
    #define _EXTERNAL_CLOCK      EXTERNAL_CLOCK
    #define CLOCK_DIV            5                                       // input must be divided to 8MHz..16MHz range (/1 to /8 for FPU parts)
    #define CLOCK_MUL            30                                      // PLL0 multiplication factor to achieve operating frequency of 150MHz (x16 to x47 possible - divided by 2 at VCC output)
    #define FLEX_CLOCK_DIVIDE    3                                       // 150/3 to give 50MHz
    #define FLASH_CLOCK_DIVIDE   6                                       // 150/6 to give 25MHz
    #define USB_CLOCK_SOURCE_MCGPLL1CLK                                  // {29} the clock source for the USB clock is dedicated to the FS USB interface (48MHz)
    #define CLOCK_DIV_1          5                                       // input must be divided to 8MHz..16MHz range (/1 to /8 for FPU parts)
    #define CLOCK_MUL_1          24                                      // PLL1 multiplication factor to achieve operating frequency of 120MHz [suitable for FS USB] (x16 to x47 possible - divided by 2 at VCC output)
    #define USB_CLOCK_GENERATED_INTERNALLY                               // use USB clock from internal source rather than external pin
#elif defined FRDM_K64F || defined TWR_K64F120M || defined FreeLON || defined TEENSY_3_5 // {26}
    #define MASK_1N83J
  //#define RUN_FROM_DEFAULT_CLOCK                                       // default mode is FLL Engaged Internal - the 32kHz IRC is multiplied by FLL factor of 640 to obtain 20.9715MHz nominal frequency (20MHz..25MHz)
  //#define RUN_FROM_HIRC                                                // clock directly from internal 48MHz RC clock
  //#define RUN_FROM_HIRC_PLL                                            // use 48MHz RC clock as input to the PLL
  //#define RUN_FROM_HIRC_FLL                                            // use 48MHz RC clock as input to the FLL
  //#define RUN_FROM_RTC_FLL                                             // use 32.76kHz crystal clock as input to the FLL
    #if defined RUN_FROM_DEFAULT_CLOCK
      //#define FLL_FACTOR           2929                                // use FLL (factors available are 640, 732, 1280, 1464, 1920, 2197, 2560 and 2929)
        #define FLEX_CLOCK_DIVIDE    1                                   // approx. 22.5MHz
        #define FLASH_CLOCK_DIVIDE   1                                   // approx. 22.5MHz 
        #define BUS_CLOCK_DIVIDE     1                                   // approx. 22.5MHz
    #elif defined RUN_FROM_HIRC                                          // use IRC48M internal oscillator directly (no PLL or FLL)
        #define FLEX_CLOCK_DIVIDE    2                                   // approx. 24MHz
        #define FLASH_CLOCK_DIVIDE   2                                   // approx. 24MHz 
        #define BUS_CLOCK_DIVIDE     1                                   // approx. 48MHz
    #elif defined RUN_FROM_HIRC_FLL
        #define EXTERNAL_CLOCK       48000000                            // this is not really external but the IRC48MCLK is otherwise selected as if it were (Ethernet not possible!)
        #define _EXTERNAL_CLOCK      EXTERNAL_CLOCK
        #define FLL_FACTOR           2929                                // use FLL (factors available are 640, 732, 1280, 1464, 1920, 2197, 2560 and 2929)
        #define FLEX_CLOCK_DIVIDE    3                                   // 120/3 to give 40MHz
        #define FLASH_CLOCK_DIVIDE   5                                   // 120/5 to give 24MHz
    #elif defined RUN_FROM_RTC_FLL
        #define EXTERNAL_CLOCK       32768
        #define _EXTERNAL_CLOCK      EXTERNAL_CLOCK
        #define FLL_FACTOR           2929                                // use FLL (factors available are 640, 732, 1280, 1464, 1920, 2197, 2560 and 2929)
        #define FLEX_CLOCK_DIVIDE    3                                   // 96/2 to give 48MHz
        #define FLASH_CLOCK_DIVIDE   4                                   // 96/4 to give 24MHz
    #elif defined RUN_FROM_HIRC_PLL
        #define EXTERNAL_CLOCK       48000000                            // this is not really external but the IRC48MCLK is otherwise selected as if it were (Ethernet not possible!)
        #define _EXTERNAL_CLOCK      EXTERNAL_CLOCK
        #define CLOCK_DIV            20                                  // input must be divided to 2MHz..4MHz range (/1 to /24)
        #define CLOCK_MUL            50                                  // the PLL multiplication factor to achieve operating frequency of 120MHz (x24 to x55 possible)
        #define FLEX_CLOCK_DIVIDE    3                                   // 120/3 to give 40MHz
        #define FLASH_CLOCK_DIVIDE   5                                   // 120/5 to give 24MHz
    #elif defined TEENSY_3_5
        #define CRYSTAL_FREQUENCY    16000000                            // 16 MHz crystal
        #define OSC_LOW_GAIN_MODE
        #define _EXTERNAL_CLOCK      CRYSTAL_FREQUENCY
        #define CLOCK_DIV            4                                   // input must be divided to 2MHz..4MHz range (/1 to /24)
        #define CLOCK_MUL            30                                  // the PLL multiplication factor to achieve operating frequency of 120MHz (x24 to x55 possible)
        #define FLEX_CLOCK_DIVIDE    3                                   // 120/3 to give 40MHz
        #define FLASH_CLOCK_DIVIDE   5                                   // 120/5 to give 24MHz
    #else                                                                // run from external clock (typical configuration when Ethernet is required)
        #define EXTERNAL_CLOCK       50000000                            // this must be 50MHz in order to use Ethernet in RMII mode
        #define _EXTERNAL_CLOCK      EXTERNAL_CLOCK
        #define CLOCK_DIV            20                                  // input must be divided to 2MHz..4MHz range (/1 to /24)
        #define CLOCK_MUL            48                                  // the PLL multiplication factor to achieve operating frequency of 120MHz (x24 to x55 possible)
        #define FLEX_CLOCK_DIVIDE    3                                   // 120/3 to give 40MHz
        #define FLASH_CLOCK_DIVIDE   5                                   // 120/5 to give 24MHz
    #endif
    #define USB_CRYSTAL_LESS                                             // use 48MHz IRC as USB source (according to Freescale AN4905 - only possible in device mode)
  //#define USB_CLOCK_GENERATED_INTERNALLY                               // use USB clock from internal source rather than external pin - 120MHz is suitable from PLL
#elif defined TWR_K65F180M || defined FRDM_K66F || defined TEENSY_3_6 || defined FRDM_KL82Z
  //#define RUN_FROM_DEFAULT_CLOCK                                       // default mode is FLL Engaged Internal - the 32kHz IRC is multiplied by FLL factor of 640 to obtain 20.9715MHz nominal frequency (20MHz..25MHz)
  //#define RUN_FROM_HIRC                                                // clock directly from internal 48MHz RC clock
  //#define RUN_FROM_HIRC_PLL                                            // use 48MHz RC clock as input to the PLL
  //#define RUN_FROM_HIRC_FLL                                            // use 48MHz RC clock as input to the FLL
  //#define RUN_FROM_RTC_FLL                                             // use 32.76kHz crystal clock as input to the FLL
    #if defined RUN_FROM_DEFAULT_CLOCK
      //#define FLL_FACTOR           2929                                // use FLL (factors available are 640, 732, 1280, 1464, 1920, 2197, 2560 and 2929)
        #define FLEX_CLOCK_DIVIDE    1                                   // approx. 22.5MHz
        #define FLASH_CLOCK_DIVIDE   1                                   // approx. 22.5MHz 
        #define BUS_CLOCK_DIVIDE     1                                   // approx. 22.5MHz
    #elif defined RUN_FROM_HIRC                                          // use IRC48M internal oscillator directly (no PLL or FLL)
        #define FLEX_CLOCK_DIVIDE    2                                   // approx. 24MHz
        #define FLASH_CLOCK_DIVIDE   2                                   // approx. 24MHz 
        #define BUS_CLOCK_DIVIDE     1                                   // approx. 48MHz
    #elif defined RUN_FROM_HIRC_FLL
        #define EXTERNAL_CLOCK       48000000                            // this is not really external but the IRC48MCLK is otherwise selected as if it were (Ethernet not possible!)
        #define _EXTERNAL_CLOCK      EXTERNAL_CLOCK
        #define FLL_FACTOR           2929                                // use FLL (factors available are 640, 732, 1280, 1464, 1920, 2197, 2560 and 2929)
        #define FLEX_CLOCK_DIVIDE    3                                   // 120/3 to give 40MHz
        #define FLASH_CLOCK_DIVIDE   5                                   // 120/5 to give 24MHz
    #elif defined RUN_FROM_RTC_FLL
        #define EXTERNAL_CLOCK       32768
        #define _EXTERNAL_CLOCK      EXTERNAL_CLOCK
        #define FLL_FACTOR           2929                                // use FLL (factors available are 640, 732, 1280, 1464, 1920, 2197, 2560 and 2929)
        #define FLEX_CLOCK_DIVIDE    3                                   // 96/2 to give 48MHz
        #define FLASH_CLOCK_DIVIDE   4                                   // 96/4 to give 24MHz
    #elif defined RUN_FROM_HIRC_PLL
        #define EXTERNAL_CLOCK       48000000                            // this is not really external but the IRC48MCLK is otherwise selected as if it were (Ethernet not possible!)
        #define _EXTERNAL_CLOCK      EXTERNAL_CLOCK
        #define CLOCK_DIV            20                                  // input must be divided to 2MHz..4MHz range (/1 to /24)
        #define CLOCK_MUL            50                                  // the PLL multiplication factor to achieve operating frequency of 120MHz (x24 to x55 possible)
        #define FLEX_CLOCK_DIVIDE    3                                   // 120/3 to give 40MHz
        #define FLASH_CLOCK_DIVIDE   5                                   // 120/5 to give 24MHz
    #else
        #if defined FRDM_K66F || defined FRDM_KL82Z
            #define CRYSTAL_FREQUENCY    12000000                        // 12 MHz crystal
            #define CLOCK_DIV            1                               // input must be divided to 8MHz..16MHz range (/1 to /8)
        #else
            #define CRYSTAL_FREQUENCY    16000000                        // 16 MHz crystal
            #define CLOCK_DIV            2                               // input must be divided to 8MHz..16MHz range (/1 to /8)
        #endif
        #define OSC_LOW_GAIN_MODE
        #define _EXTERNAL_CLOCK      CRYSTAL_FREQUENCY
      //#define USE_HIGH_SPEED_RUN_MODE
        #if defined USE_HIGH_SPEED_RUN_MODE                              // high speed run mode allow faster operation but can't program/erase flash
            #if defined FRDM_KL82Z
                #define CLOCK_MUL        16                              // the PLL multiplication factor to achieve operating frequency of 96MHz (x16 to x47 possible) [PLL output range 90..180MHz - VCO is PLL * 2]
            #elif defined FRDM_K66F
                #define CLOCK_MUL        30                              // the PLL multiplication factor to achieve operating frequency of 180MHz (x16 to x47 possible) [PLL output range 90..180MHz - VCO is PLL * 2]
            #else
                #define CLOCK_MUL        45                              // the PLL multiplication factor to achieve operating frequency of 180MHz (x16 to x47 possible) [PLL output range 90..180MHz - VCO is PLL * 2]
            #endif
            #if defined FRDM_KL82Z
                #define BUS_CLOCK_DIVIDE     4                           // 96/4 to give 24MHz (max. 24MHz)
                #define QSPI_CLOCK_DIVIDE    1                           // 96/1 to give 96MHz (max. 96MHz)
                #define FLASH_CLOCK_DIVIDE   4                           // 96/4 to give 24MHz (max. ?MHz)
            #else
                #define BUS_CLOCK_DIVIDE     3                           // 180/3 to give 60MHz (max. 60MHz)
                #define FLEX_CLOCK_DIVIDE    3                           // 180/3 to give 60MHz (max. 60MHz)
                #define FLASH_CLOCK_DIVIDE   7                           // 180/7 to give 25.714MHz (max. 28MHz)
            #endif
        #else                                                            // run mode has no functional restrictions but can't operate as fast as high speed run mode
            #if defined FRDM_KL82Z
                #define CLOCK_MUL        24                              // the PLL multiplication factor to achieve operating frequency of 144MHz (x16 to x47 possible) [PLL output range 90..180MHz - VCO is PLL * 2]
            #elif defined TEENSY_3_6
                #define CLOCK_MUL        30                              // the PLL multiplication factor to achieve operating frequency of 120MHz (x16 to x47 possible) [PLL output range 90..180MHz - VCO is PLL * 2]
            #else
                #define CLOCK_MUL        20                              // the PLL multiplication factor to achieve operating frequency of 120MHz (x16 to x47 possible) [PLL output range 90..180MHz - VCO is PLL * 2]
            #endif
            #if defined FRDM_KL82Z
                #define SYSTEM_CLOCK_DIVIDE  2                           // 144/2 to give 72MHz
                #define BUS_CLOCK_DIVIDE     6                           // 144/6 to give 24MHz (max. 24MHz)
                #define QSPI_CLOCK_DIVIDE    2                           // 144/2 to give 72MHz (max. 72MHz)
                #define FLASH_CLOCK_DIVIDE   6                           // 144/6 to give 24MHz (max. 24MHz)
            #else
                #define BUS_CLOCK_DIVIDE     2                           // 120/2 to give 60MHz (max. 60MHz)
                #define FLEX_CLOCK_DIVIDE    2                           // 120/2 to give 60MHz (max. 60MHz)
                #define FLASH_CLOCK_DIVIDE   5                           // 120/7 to give 24MHz (max. 28MHz)
            #endif 
        #endif
    #endif
    #define USB_CRYSTAL_LESS                                             // use 48MHz IRC as USB source (according to Freescale AN4905 - only possible in device mode)
  //#define USB_CLOCK_GENERATED_INTERNALLY                               // use USB clock from internal source rather than external pin - 180MHz/120MHz is suitable from PLL
#elif defined TWR_KW21D256
  //#define RUN_FROM_DEFAULT_CLOCK                                       // default mode is FLL Engaged Internal - the 32kHz IRC is multiplied by FLL factor of 640 to obtain 20.9715MHz nominal frequency (20MHz..25MHz)
    #define RUN_FROM_MODEM_CLK_OUT                                       // use 32MHz modem clock as source (defaults to 32.768kHz or 4MHz)
    #if defined RUN_FROM_MODEM_CLK_OUT
        #define EXTERNAL_CLOCK       4000000                             // 4 MHz CLK_OUT option
        #define _EXTERNAL_CLOCK      EXTERNAL_CLOCK
        #define CLOCK_DIV            2                                   // input must be divided to 2MHz..4MHz range (/1 to /25 possible)
        #define CLOCK_MUL            24                                  // the PLL multiplication factor to achieve operating frequency of 48MHz (x24 to x55 possible)
    #else
        #define FLASH_CLOCK_DIVIDE   1                                   // no dividers required since the default speed is low enough
        #define BUS_CLOCK_DIVIDE     1                                   // 
    #endif
#elif defined TWR_KW24D512
  //#define RUN_FROM_DEFAULT_CLOCK                                       // default mode is FLL Engaged Internal - the 32kHz IRC is multiplied by FLL factor of 640 to obtain 20.9715MHz nominal frequency (20MHz..25MHz)
    #define RUN_FROM_MODEM_CLK_OUT                                       // use 32MHz modem clock as source (defaults to 32.768kHz or 4MHz)
    #if defined RUN_FROM_MODEM_CLK_OUT
        #define EXTERNAL_CLOCK       4000000                             // 4 MHz CLK_OUT option
        #define _EXTERNAL_CLOCK      EXTERNAL_CLOCK
        #define CLOCK_DIV            2                                   // input must be divided to 2MHz..4MHz range (/1 to /25 possible)
        #define CLOCK_MUL            24                                  // the PLL multiplication factor to achieve operating frequency of 48MHz (x24 to x55 possible)
    #else
        #define FLASH_CLOCK_DIVIDE   1                                   // no dividers required since the default speed is low enough
        #define BUS_CLOCK_DIVIDE     1                                   // 
    #endif
#elif defined K02F100M
  //#define RUN_FROM_DEFAULT_CLOCK                                       // 32kHz IRC used as source
    #define RUN_FROM_HIRC                                                // clock directly from internal 48MHz RC clock
  //#define RUN_FROM_HIRC_FLL                                            // clock from FLL derived from internal 48MHz RC clock
    #if defined RUN_FROM_DEFAULT_CLOCK
        #define FLL_FACTOR           1464                                // use FLL for 46..57MHz from 32kHz IRC (factors available are 640, 732, 1280, 1464, 1920, 2197, 2560 and 2929)
        #define SYSTEM_CLOCK_DIVIDE  1
        #define BUS_CLOCK_DIVIDE     2                                   // 40..50MHz
        #define FLASH_CLOCK_DIVIDE   4                                   // 20..25MHz
    #elif defined RUN_FROM_HIRC
        #define FLEX_CLOCK_DIVIDE    2                                   // approx. 24MHz
        #define FLASH_CLOCK_DIVIDE   2                                   // approx. 24MHz 
        #define BUS_CLOCK_DIVIDE     1                                   // approx. 48MHz
    #else                                                                // RUN_FROM_HIRC_FLL
        #define FLL_FACTOR           2197                                // use FLL for 68.65MHz from 48MHz/1536 IRC (factors available are 640, 732, 1280, 1464, 1920, 2197, 2560 and 2929)
        #define SYSTEM_CLOCK_DIVIDE  1
        #define BUS_CLOCK_DIVIDE     2                                   // 48MHz
        #define FLASH_CLOCK_DIVIDE   4                                   // 24MHz
    #endif
#elif defined K70F150M_12M
    #define MASK_3N96B                                                   // mask used - this ensures that the mask's erratas are respected
    #define CRYSTAL_FREQUENCY    12000000                                // 12 MHz crystal
    #define _EXTERNAL_CLOCK      CRYSTAL_FREQUENCY
    #define CLOCK_DIV            1                                       // input must be divided to 8MHz..16MHz range (/1 to /8 for 150MHz parts)
    #define CLOCK_MUL            25                                      // the PLL multiplication factor to achieve operating frequency of 120MHz (x16 to x47 possible - divided by 2 at VCO output)
    #define USB_CLOCK_GENERATED_INTERNALLY                               // use USB clock from internal source rather than external pin
    #define FLEX_CLOCK_DIVIDE    3                                       // 150/3 to give 50MHz
    #define FLASH_CLOCK_DIVIDE   6                                       // 150/6 to give 25MHz
    #define USB_CLOCK_GENERATED_INTERNALLY                               // use USB clock from internal source rather than external pin - 150MHz is suitable from PLL1
    #define USB_CLOCK_SOURCE_MCGPLL1CLK                                  // the clock source for the USB clock
    #define CLOCK_DIV_1          1                                       // input must be divided to 8MHz..16MHz range (/1 to /8 for FPU parts)
    #define CLOCK_MUL_1          20                                      // PLL1 multiplication factor to achieve operating frequency of 120MHz [suitable for FS USB and DDR2] (x16 to x47 possible - divided by 2 at VCC output)
#elif defined TWR_K60F120M || defined TWR_K70F120M || defined EMCRAFT_K70F120M || defined TWR_VF65GS10 // {9}
    #define EXTERNAL_CLOCK       50000000                                // this must be 50MHz in order to use Ethernet in RMII mode
    #define _EXTERNAL_CLOCK      EXTERNAL_CLOCK
    #define CLOCK_DIV            5                                       // input must be divided to 8MHz..16MHz range (/1 to /8 for FPU parts)
    #define CLOCK_MUL            24                                      // the PLL multiplication factor to achieve operating frequency of 120MHz (x16 to x47 possible - divided by 2 at VCO output)
    #define FLEX_CLOCK_DIVIDE    3                                       // 120/3 to give 40MHz
    #define FLASH_CLOCK_DIVIDE   5                                       // 120/5 to give 24MHz
    #define USB_CLOCK_GENERATED_INTERNALLY                               // use USB clock from internal source rather than external pin - 120MHz is suitable
    #define USB_CLOCK_SOURCE_MCGPLL0CLK                                  // the clock source for the USB clock
#elif defined K60F150M_50M
    #define EXTERNAL_CLOCK       50000000                                // this must be 50MHz in order to use Ethernet in RMII mode
    #define _EXTERNAL_CLOCK      EXTERNAL_CLOCK
    #define CLOCK_DIV            5                                       // input must be divided to 8MHz..16MHz range (/1 to /8 for FPU parts)
    #define CLOCK_MUL            30                                      // the PLL multiplication factor to achieve operating frequency of 150MHz (x16 to x47 possible - divided by 2 at VCC output)
    #define FLEX_CLOCK_DIVIDE    3                                       // 150/3 to give 50MHz
    #define FLASH_CLOCK_DIVIDE   6                                       // 150/6 to give 25MHz
    #define USB_CLOCK_GENERATED_INTERNALLY                               // use USB clock from internal source rather than external pin - 120MHz is suitable
    #define USB_CLOCK_SOURCE_MCGPLL1CLK                                  // the clock source for the USB clock is dedicated to the FS USB interface (48MHz)
    #define CLOCK_DIV_1          5                                       // input must be divided to 8MHz..16MHz range (/1 to /8 for FPU parts) (Neets - BDR)
    #define CLOCK_MUL_1          24                                      // PLL1 multiplication factor to achieve operating frequency of 120MHz [suitable for FS USB] (x16 to x47 possible - divided by 2 at VCC output) (Neets - BDR)
#elif defined TWR_K80F150M || defined FRDM_K82F
    #define OSC_LOW_GAIN_MODE
    #define CRYSTAL_FREQUENCY    12000000                                // 12 MHz crystal
    #define _EXTERNAL_CLOCK      CRYSTAL_FREQUENCY
  //#define USE_HIGH_SPEED_RUN_MODE
    #define CLOCK_DIV            1                                       // input must be divided to 8MHz..16MHz range (/1 to /8 for 150MHz parts)
    #if defined USE_HIGH_SPEED_RUN_MODE
        #define CLOCK_MUL            25                                  // the PLL multiplication factor to achieve operating frequency of 150MHz (x16 to x47 possible - divided by 2 at VCO output)
        #define FLEX_CLOCK_DIVIDE    2                                   // 150/2 to give 75MHz
        #define FLASH_CLOCK_DIVIDE   6                                   // 150/6 to give 25MHz
    #else
        #define CLOCK_MUL            20                                  // the PLL multiplication factor to achieve operating frequency of 120MHz (x16 to x47 possible - divided by 2 at VCO output)
        #define FLEX_CLOCK_DIVIDE    3                                   // 120/2 to give 60MHz
        #define FLASH_CLOCK_DIVIDE   6                                   // 120/5 to give 24MHz
    #endif
    #define USB_CRYSTAL_LESS                                             // use 48MHz IRC as USB source (according to Freescale AN4905 - only possible in device mode)
  //#define USB_CLOCK_GENERATED_INTERNALLY                               // use USB clock from internal source rather than external pin - 120MHz is suitable
#elif defined TWR_K60N512 || defined TWR_K60D100M || defined KINETIS_K52 || defined TWR_K53N512 || defined KINETIS_K61 || defined KINETIS_K70
        #define EXTERNAL_CLOCK       50000000                            // this must be 50MHz in order to use Ethernet in RMII mode
        #define _EXTERNAL_CLOCK      EXTERNAL_CLOCK
  //#define RUN_FROM_DEFAULT_CLOCK
    #if defined RUN_FROM_DEFAULT_CLOCK
        #define SYSTEM_CLOCK_DIVIDE   1
        #define BUS_CLOCK_DIVIDE      1
        #define FLASH_CLOCK_DIVIDE    2
        #define FLEX_CLOCK_DIVIDE     2
    #else
        #if defined USB_INTERFACE                                        // when using USB generate 96MHz clock so that a 48Mhz clock can be generated from it
            #define CLOCK_DIV        25                                  // input must be divided to 2MHz..4MHz range (/1 to /25 possible)
            #define CLOCK_MUL        48                                  // the PLL multiplication factor to achieve operating frequency of 100MHz (x24 to x55 possible)
        #else
            #define CLOCK_DIV        16                                  // input must be divided to 2MHz..4MHz range (/1 to /25 possible)
            #define CLOCK_MUL        32                                  // the PLL multiplication factor to achieve operating frequency of 100MHz (x24 to x55 possible)
        #endif
        #define USB_CLOCK_GENERATED_INTERNALLY                           // use USB clock from internal source rather than external pin
    #endif
#elif defined TWR_KV31F120M || defined FRDM_KV31F
  //#define RUN_FROM_DEFAULT_CLOCK                                       // default mode is FLL Engaged Internal - the 32kHz IRC is multiplied by FLL factor of 640 to obtain 20.9715MHz nominal frequency (20MHz..25MHz)
    #if !defined RUN_FROM_DEFAULT_CLOCK
        #define OSC_LOW_GAIN_MODE
        #define CRYSTAL_FREQUENCY    8000000                             // 8 MHz crystal
        #define _EXTERNAL_CLOCK      CRYSTAL_FREQUENCY
        #define CLOCK_DIV            2                                   // input must be divided to 2MHz..4MHz range (/1 to /24)
        #define CLOCK_MUL            30                                  // the PLL multiplication factor to achieve operating frequency of 120MHz (x24 to x55 possible)
        #define FLEX_CLOCK_DIVIDE    3                                   // 120/3 to give 40MHz
        #define FLASH_CLOCK_DIVIDE   5                                   // 120/5 to give 24MHz
    #else
        #define FLEX_CLOCK_DIVIDE    1                                   // 
        #define FLASH_CLOCK_DIVIDE   1                                   // 
        #define BUS_CLOCK_DIVIDE     1                                   // 
    #endif
#elif defined TWR_K24F120M
  //#define RUN_FROM_DEFAULT_CLOCK                                       // default mode is FLL Engaged Internal - the 32kHz IRC is multiplied by FLL factor of 640 to obtain 20.9715MHz nominal frequency (20MHz..25MHz)
    #if !defined RUN_FROM_DEFAULT_CLOCK
        #define OSC_LOW_GAIN_MODE
        #define CRYSTAL_FREQUENCY    8000000                             // 8 MHz crystal
        #define _EXTERNAL_CLOCK      CRYSTAL_FREQUENCY
        #define CLOCK_DIV            2                                   // input must be divided to 2MHz..4MHz range (/1 to /24)
        #define CLOCK_MUL            30                                  // the PLL multiplication factor to achieve operating frequency of 120MHz (x24 to x55 possible)
        #define FLEX_CLOCK_DIVIDE    3                                   // 120/3 to give 40MHz
        #define FLASH_CLOCK_DIVIDE   5                                   // 120/5 to give 24MHz
    #else
        #define FLEX_CLOCK_DIVIDE    1                                   // 
        #define FLASH_CLOCK_DIVIDE   1                                   // 
        #define BUS_CLOCK_DIVIDE     1                                   // 
    #endif    
    #define USB_CRYSTAL_LESS                                             // use 48MHz IRC as USB source (according to Freescale AN4905 - only possible in device mode)
  //#define USB_CLOCK_GENERATED_INTERNALLY                               // use USB clock from internal source rather than external pin - 120MHz is suitable
#elif defined TWR_K21F120M
    #define OSC_LOW_GAIN_MODE                                            // oscillator without feedback resistor or load capacitors so use low gain mode
    #define CRYSTAL_FREQUENCY    8000000                                 // 8 MHz crystal
  //#define RUN_FROM_EXTERNAL_CLOCK                                      // run directly from external 8MHz clock (without FLL)
    #define _EXTERNAL_CLOCK      CRYSTAL_FREQUENCY
    #define CLOCK_DIV            2                                       // input must be divided to 2MHz..4MHz range (/1 to /24 for 120MHz parts)
    #define CLOCK_MUL            30                                      // the PLL multiplication factor to achieve operating frequency of 120MHz (x24 to x55 possible)
    #define FLEX_CLOCK_DIVIDE    3                                       // 120/3 to give 40MHz
    #define FLASH_CLOCK_DIVIDE   5                                       // 120/5 to give 24MHz
    #define USB_CLOCK_GENERATED_INTERNALLY                               // use USB clock from internal source rather than external pin - 120MHz is suitable
#elif defined BLAZE_K22
    #define OSC_LOW_GAIN_MODE                                            // oscillator without feedback resistor or load capacitors so use low gain mode
    #define CRYSTAL_FREQUENCY    16000000                                // 16 MHz crystal
    #define _EXTERNAL_CLOCK      CRYSTAL_FREQUENCY
    #define CLOCK_DIV            4                                       // input must be divided to 2MHz..4MHz range (/1 to /24 for 120MHz parts)
    #define CLOCK_MUL            30                                      // the PLL multiplication factor to achieve operating frequency of 120MHz (x24 to x55 possible)
    #define FLEX_CLOCK_DIVIDE    3                                       // 120/3 to give 40MHz
    #define FLASH_CLOCK_DIVIDE   5                                       // 120/5 to give 24MHz
    #define USB_CLOCK_GENERATED_INTERNALLY                               // use USB clock from internal source rather than external pin - 120MHz is suitable
#elif defined FRDM_K22F || defined TWR_K22F120M || defined K22F128_100M
  //#define RUN_FROM_DEFAULT_CLOCK                                       // default mode is FLL Engaged Internal - the 32kHz IRC is multiplied by FLL factor of 640 to obtain 20.9715MHz nominal frequency (20MHz..25MHz)
  //#define RUN_FROM_LIRC                                                // clock directly from internal 4MHz RC clock
    #define RUN_FROM_HIRC                                                // clock directly from internal 48MHz RC clock
  //#define RUN_FROM_HIRC_PLL                                            // use 48MHz RC clock as input to the PLL
  //#define RUN_FROM_HIRC_FLL                                            // use 48MHz RC clock as input to the FLL
    #if defined RUN_FROM_LIRC                                            // 4MHz
        #define FLEX_CLOCK_DIVIDE    5                                   // 4/5 to give 800kHz
        #define FLASH_CLOCK_DIVIDE   5                                   // 4/5 to give 800kHz
        #define BUS_CLOCK_DIVIDE     5                                   // 4/5 to give 800kHz
    #elif !defined RUN_FROM_DEFAULT_CLOCK
        #define OSC_LOW_GAIN_MODE
        #define CRYSTAL_FREQUENCY    8000000                             // 8 MHz crystal on board
      //#define USE_HIGH_SPEED_RUN_MODE
        #if defined USE_HIGH_SPEED_RUN_MODE                              // 120 MHz requires use of the high speed run mode (with restriction of not being able to program flash in that mode)
            #define _EXTERNAL_CLOCK      CRYSTAL_FREQUENCY
            #define CLOCK_DIV            2                               // input must be divided to 2MHz..4MHz range (/1 to /24)
            #define CLOCK_MUL            30                              // the PLL multiplication factor to achieve operating frequency of 120MHz (x24 to x55 possible) - > 80MHz requires high speed run mode, which doesn't allow flash programming
            #define FLEX_CLOCK_DIVIDE    3                               // 120/3 to give 40MHz
            #define FLASH_CLOCK_DIVIDE   5                               // 120/5 to give 24MHz
            #define BUS_CLOCK_DIVIDE     2                               // 120/2 to give 60MHz
        #elif defined RUN_FROM_HIRC                                      // use IRC48M internal oscillator directly (no PLL or FLL)
            #define FLEX_CLOCK_DIVIDE    2                               // approx. 24MHz
            #define FLASH_CLOCK_DIVIDE   2                               // approx. 24MHz 
            #define BUS_CLOCK_DIVIDE     1                               // approx. 48MHz
        #elif defined RUN_FROM_HIRC_PLL
            #define EXTERNAL_CLOCK       48000000                        // this is not really external but the IRC48MCLK is otherwise selected as if it were
            #define _EXTERNAL_CLOCK      EXTERNAL_CLOCK
            #define CLOCK_DIV            24                              // input must be divided to 2MHz..4MHz range (/1 to /24)
            #define CLOCK_MUL            40                              // the PLL multiplication factor to achieve operating frequency of 120MHz (x24 to x55 possible)
            #define FLEX_CLOCK_DIVIDE    4                               // 80/3 to give 20MHz
            #define FLASH_CLOCK_DIVIDE   3                               // 80/3 to give 26.7MHz
            #define BUS_CLOCK_DIVIDE     2                               // 80/2 to give 40MHz
        #else                                                            // fastest run mode operation of 80MHz
            #define _EXTERNAL_CLOCK      CRYSTAL_FREQUENCY
            #define CLOCK_DIV            4                               // input must be divided to 2MHz..4MHz range (/1 to /24)
            #define CLOCK_MUL            40                              // the PLL multiplication factor to achieve operating frequency of 80MHz (x24 to x55 possible) - uses normal run mode and can program flash
            #define FLEX_CLOCK_DIVIDE    4                               // 80/3 to give 20MHz
            #define FLASH_CLOCK_DIVIDE   3                               // 80/3 to give 26.7MHz
            #define BUS_CLOCK_DIVIDE     2                               // 80/2 to give 40MHz
        #endif
    #else
        #define FLEX_CLOCK_DIVIDE    1                                   // 
        #define FLASH_CLOCK_DIVIDE   1                                   // 
        #define BUS_CLOCK_DIVIDE     1                                   // 
    #endif
    #define USB_CRYSTAL_LESS                                             // use 48MHz IRC as USB source (according to Freescale AN4905 - only possible in device mode)
  //#define USB_CLOCK_GENERATED_INTERNALLY                               // use USB clock from internal source rather than external pin - 120MHz is suitable
#elif defined TWR_K20D72M
    #define CRYSTAL_FREQUENCY    8000000                                 // 8 MHz crystal
    #define _EXTERNAL_CLOCK      CRYSTAL_FREQUENCY
    #define CLOCK_DIV            4                                       // input must be divided to 2MHz..4MHz range (/1 to /25 possible)
    #define CLOCK_MUL            36                                      // the PLL multiplication factor to achieve operating frequency of 48MHz (x24 to x55 possible)
    #define USB_CLOCK_GENERATED_INTERNALLY                               // use USB clock from internal source rather than external pin
#elif defined TEENSY_3_1
    #define OSC_LOW_GAIN_MODE
    #define CRYSTAL_FREQUENCY    16000000                                // 16 MHz crystal
    #define _EXTERNAL_CLOCK      CRYSTAL_FREQUENCY
    #define CLOCK_DIV            8                                       // input must be divided to 2MHz..4MHz range (/1 to /25 possible)
    #define CLOCK_MUL            36                                      // the PLL multiplication factor to achieve operating frequency of 48MHz (x24 to x55 possible)
    #define USB_CLOCK_GENERATED_INTERNALLY                               // use USB clock from internal source rather than external pin
#elif defined K20FX512_120
    #define CRYSTAL_FREQUENCY    8000000                                 // 8 MHz crystal
    #define _EXTERNAL_CLOCK      CRYSTAL_FREQUENCY

    #define CLOCK_DIV            1                                       // input must be divided to 8MHz..16MHz range (/1 to /8 for 120MHz parts)
    #define CLOCK_MUL            30                                      // the PLL multiplication factor to achieve operating frequency of 120MHz (x16 to x47 possible - divided by 2 at VCO output)
    #define USB_CLOCK_GENERATED_INTERNALLY                               // use USB clock from internal source rather than external pin
    #define FLEX_CLOCK_DIVIDE    3                                       // 120/3 to give 40MHz
    #define FLASH_CLOCK_DIVIDE   5                                       // 120/5 to give 24MHz
    #define USB_CLOCK_GENERATED_INTERNALLY                               // use USB clock from internal source rather than external pin - 120MHz is suitable
    #define USB_CLOCK_SOURCE_MCGPLL0CLK                                  // the clock source for the USB clock
#elif defined TWR_KL43Z48M
    #define OSC_LOW_GAIN_MODE
    #define CRYSTAL_FREQUENCY    8000000                                 // 8MHz crystal
    #define _EXTERNAL_CLOCK      CRYSTAL_FREQUENCY
    #define RUN_FROM_HIRC                                                // clock from internal 48MHz RC clock
  //#define RUN_FROM_LIRC                                                // clock from internal 8MHz RC clock

    #define USB_CRYSTAL_LESS                                             // use 48MHz IRC as USB source (according to Freescale AN4905 - only possible in device mode) - rather than external pin
#elif defined KL43Z_256_32_CL
    #define RUN_FROM_HIRC                                                // clock from internal 48MHz RC clock
    #define BUS_CLOCK_DIVIDE     2                                       // bus and flash clock divider value (1..8)

    #define USB_CRYSTAL_LESS                                             // use 48MHz IRC as USB source (according to Freescale AN4905 - only possible in device mode) - rather than external pin
#elif defined FRDM_KL43Z || defined FRDM_KL27Z || defined CAPUCCINO_KL27
    #define OSC_LOW_GAIN_MODE
    #define CRYSTAL_FREQUENCY    32768                                   // 32kHz crystal
    #define _EXTERNAL_CLOCK      CRYSTAL_FREQUENCY
    #define RUN_FROM_HIRC                                                // clock from internal 48MHz RC clock
  //#define RUN_FROM_LIRC                                                // clock from internal 8MHz RC clock
    #define BUS_CLOCK_DIVIDE     2                                       // bus and flash clock divider value (1..8)

    #define USB_CRYSTAL_LESS                                             // use 48MHz IRC as USB source (according to Freescale AN4905 - only possible in device mode) - rather than external pin
#elif defined TWR_K20D50M || defined TWR_K21D50M || defined FRDM_K20D50M || defined tinyK20 || defined FRDM_KL46Z || defined TWR_KL46Z48M || defined FRDM_KL25Z || defined FRDM_KL26Z || defined TWR_KL25Z48M // {2}{22}{23}{24}
    #if defined FRDM_K20D50M || defined tinyK20 || defined TWR_KL46Z48M || defined FRDM_KL25Z || defined FRDM_KL26Z || defined TWR_KL25Z48M || defined TWR_K21D50M
        #define OSC_LOW_GAIN_MODE                                        // oscillator without feedback resistor or load capacitors so use low gain mode
    #endif
  //#define RUN_FROM_DEFAULT_CLOCK
  //#define RUN_FROM_EXTERNAL_CLOCK                                      // run directly from external 8MHz clock (without PLL)
    #define CRYSTAL_FREQUENCY    8000000                                 // 8 MHz crystal
    #define _EXTERNAL_CLOCK      CRYSTAL_FREQUENCY
    #define CLOCK_DIV            4                                       // input must be divided to 2MHz..4MHz range (/1 to /25 possible)
    #if defined FRDM_KL46Z || defined TWR_KL46Z48M || defined FRDM_KL25Z || defined FRDM_KL26Z || defined TWR_KL25Z48M
        #define CLOCK_MUL        48                                      // the PLL multiplication factor to achieve MCGPLLCLK operating frequency of 98MHz (x24 to x55 possible) (MCGPLLCLK/2 is 48MHz - required by USB)
        #define SYSTEM_CLOCK_DIVIDE 2                                    // divide (1,2,3..16 possible) to get core clock of 48MHz
        #define FLASH_CLOCK_DIVIDE  2                                    // divide from core clock for bus and flash clock (1,2,3..8 possible) 24MHz
    #else
        #define CLOCK_MUL        24                                      // the PLL multiplication factor to achieve system/core operating frequency of 48MHz (x24 to x55 possible)
    #endif
    #if defined FRDM_K20D50M
        #define BUS_CLOCK_DIVIDE 1
        #define FLASH_CLOCK_DIVIDE  2
    #endif
    #define USB_CLOCK_GENERATED_INTERNALLY                               // use USB clock from internal source rather than external pin
#elif defined rcARM_KL26
    #define EXTERNAL_CLOCK       10000000                                // 10 MHz external clock
    #define _EXTERNAL_CLOCK      EXTERNAL_CLOCK
    #define CLOCK_DIV            5                                       // input must be divided to 2MHz..4MHz range (/1 to /25 possible)
    #define CLOCK_MUL            48                                      // the PLL multiplication factor to achieve MCGPLLCLK operating frequency of 98MHz (x24 to x55 possible) (MCGPLLCLK/2 is 48MHz - required by USB)
    #define SYSTEM_CLOCK_DIVIDE  2                                       // divide (1,2,3..16 possible) to get core clock of 48MHz
    #define FLASH_CLOCK_DIVIDE   2                                       // divide from core clock for bus and flash clock (1,2,3..8 possible) 24MHz
    #define USB_CLOCK_GENERATED_INTERNALLY                               // use USB clock from internal source rather than external pin
#elif defined TEENSY_LC
    #define OSC_LOW_GAIN_MODE                                            // oscillator without feedback resistor or load capacitors so use low gain mode
    #define CRYSTAL_FREQUENCY    16000000                                // 16 MHz crystal
    #define _EXTERNAL_CLOCK      CRYSTAL_FREQUENCY
    #define CLOCK_DIV            8                                       // input must be divided to 2MHz..4MHz range (/1 to /25 possible)
    #define CLOCK_MUL            48                                      // the PLL multiplication factor to achieve MCGPLLCLK operating frequency of 98MHz (x24 to x55 possible)
    #define SYSTEM_CLOCK_DIVIDE  2                                       // divide to get core clock of 48MHz (MCGPLLCLK/2 is 48MHz - required by USB)
    #define USB_CLOCK_GENERATED_INTERNALLY                               // use USB clock from internal source rather than external pin
#elif defined TWR_KV10Z32
    #define CRYSTAL_FREQUENCY    10000000                                // 10 MHz crystal
    #define _EXTERNAL_CLOCK      CRYSTAL_FREQUENCY
    #define RUN_FROM_DEFAULT_CLOCK                                       // default mode is FLL Engaged Internal - the 32kHz IRC is multiplied by FLL factor of 640 to obtain 20.9715MHz nominal frequency (20MHz..25MHz)
    #define FLL_FACTOR           2197                                    // 72MHz these devices have no PLL so use FLL (factors available are 640, 732, 1280, 1464, 1920, 2197, 2560 and 2929)
    #define BUS_CLOCK_DIVIDE     3                                       // bus and flash clock 72MHz/3 = 24MHz
    #define ADC_CLOCK_DIVIDE     2
  //#define ADC_CLOCK_ENABLED
#elif defined KWIKSTIK
    #define CRYSTAL_FREQUENCY    4000000                                 // 4 MHz crystal
    #define _EXTERNAL_CLOCK      CRYSTAL_FREQUENCY
    #define CLOCK_DIV            2                                       // input must be divided to 2MHz..4MHz range (/1 to /25 possible)
    #define CLOCK_MUL            48                                      // the PLL multiplication factor to achieve operating frequency of 96MHz (x24 to x55 possible)
    #define USB_CLOCK_GENERATED_INTERNALLY                               // use USB clock from internal source rather than external pin
#elif defined FRDM_KL03Z
    #define CRYSTAL_FREQUENCY    32768                                   // 32768 Hz crystal (if no different clock options are set this is used directly as system clock)
    #define _EXTERNAL_CLOCK      CRYSTAL_FREQUENCY
    #define RUN_FROM_HIRC                                                // clock from internal 48MHz RC clock
  //#define RUN_FROM_LIRC                                                // clock from internal 8MHz/2MHz RC clock
    #if defined RUN_FROM_HIRC
        #define SYSTEM_CLOCK_DIVIDE  1                                   // 1..16
        #define BUS_CLOCK_DIVIDE     2                                   // 1..8 (valid for bus/flash and divisor is after the system clock divider)
    #elif defined RUN_FROM_LIRC
        #define RUN_FROM_LIRC_2M                                         // selet 2MHz rather than 8MHz
        #define SLOW_CLOCK_DIVIDE    1                                   // optionally divide the slow clock output (1, 2, 4, 8, 16, 32, 64 or 128)
        #define SYSTEM_CLOCK_DIVIDE  1                                   // 1..16
        #define BUS_CLOCK_DIVIDE     2                                   // 1..8 (valid for bus/flash and divisor is after the system clock divider)
    #else                                                                // run from the 32kHz osciallator
        #define SYSTEM_CLOCK_DIVIDE  1                                   // 32kHz
        #define BUS_CLOCK_DIVIDE     1                                   // 32kHz
    #endif
#elif defined FRDM_KL02Z || defined FRDM_KL05Z                           // {25}
  //#define RUN_FROM_LIRC                                                // clock from internal 4MHz RC clock
  //#define RUN_FROM_DEFAULT_CLOCK                                       // default mode is FLL Engaged Internal - the 32kHz IRC is multiplied by FLL factor of 640 to obtain 20.971MHz nominal frequency (20MHz..25MHz)
    #if defined RUN_FROM_LIRC
        #define BUS_CLOCK_DIVIDE    1
        #define FLASH_CLOCK_DIVIDE  1
    #elif defined RUN_FROM_DEFAULT_CLOCK
        #define FLL_FACTOR          1464                                 // set highest frequency (47.972MHz)
        #if defined FLL_FACTOR
            #define FLASH_CLOCK_DIVIDE  2                                // 24MHz (valid also as bus clock divide)
        #else
            #define FLASH_CLOCK_DIVIDE  1
        #endif
    #else
        #define OSC_LOW_GAIN_MODE                                        // oscillator without feedback resistor or load capacitors so use low gain mode
        #define CRYSTAL_FREQUENCY   32768                                // 32768 Hz crystal
        #define _EXTERNAL_CLOCK     CRYSTAL_FREQUENCY
        #define FLL_FACTOR          1464                                 // 48MHz these devices have no PLL so use FLL (factors available are 640, 732, 1280, 1464, 1920, 2197, 2560 and 2929)
        #define FLASH_CLOCK_DIVIDE  2                                    // 24MHz
    #endif
#elif defined TRK_KEA64 || defined TRK_KEA8
    #define RUN_FROM_DEFAULT_CLOCK                                       // default mode is FLL Engaged Internal - the 31.25kHz IRC is multiplied by FLL factor of 1024 to obtain 32MHz nominal frequency
    #define SYSTEM_CLOCK_DIVIDE  1                                       // divide the clock output to give the system clock (maximum 40MHz) (/1, 2, 4, 8, 16, 32, 64 or 128 possible)
    #define BUS_CLOCK_DIVIDE     2                                       // divide by 1 or 2 to give bus and flash clock (maximum 20MHz)
#elif defined TRK_KEA128 || defined FRDM_KEAZ64Q64 || defined FRDM_KEAZ128Q80
    #define CRYSTAL_FREQUENCY    8000000                                 // 8 MHz crystal
  //#define RUN_FROM_EXTERNAL_CLOCK                                      // run directly from external 8MHz clock (without FLL)
    #define _EXTERNAL_CLOCK      CRYSTAL_FREQUENCY
    #define SYSTEM_CLOCK_DIVIDE  1                                       // divide the clock output to give the system clock (maximum 48MHz) (/1, 2, 4, 8, 16, 32, 64 or 128 possible)
    #if defined RUN_FROM_EXTERNAL_CLOCK
        #define BUS_CLOCK_DIVIDE 1                                       // divide by 1 or 2 to give bus and flash clock (maximum 24MHz)
    #else
        #define CLOCK_DIV        256                                     // input must be divided to 31.25kHz..39.06525kHz range (/1, 2, 4, 8, 16, 32, 64, 128, 256, 512 or 1024 possible)
                                                                         // this input is multiplied by 1280 to 40MHz..50MHz at the FLL output
        #define BUS_CLOCK_DIVIDE 2                                       // divide by 1 or 2 to give bus and flash clock (maximum 24MHz)
    #endif
#elif defined FRDM_KE06Z                                                 // {30}
    #define CRYSTAL_FREQUENCY    8000000                                 // 8 MHz crystal
  //#define RUN_FROM_EXTERNAL_CLOCK                                      // run directly from external 8MHz clock (without FLL)
    #define _EXTERNAL_CLOCK      CRYSTAL_FREQUENCY
    #define CLOCK_DIV            256                                     // input must be divided to 31.25kHz..39.06525kHz range (/1, 2, 4, 8, 16, 32, 64, 128, 256, 512 or 1024 possible)
                                                                         // this input is multipled by 1280 to 40MHz..50MHz at the FLL output
    #define SYSTEM_CLOCK_DIVIDE  1                                       // divide the clock output to give the system clock (maximum 40MHz) (/1, 2, 4, 8, 16, 32, 64 or 128 possible)
    #if defined RUN_FROM_EXTERNAL_CLOCK
        #define BUS_CLOCK_DIVIDE 1                                       // divide by 1 or 2 to give bus and flash clock (maximum 20MHz)
    #else
        #define BUS_CLOCK_DIVIDE 2                                       // divide by 1 or 2 to give bus and flash clock (maximum 20MHz)
    #endif
#elif defined FRDM_KE02Z                                                 // {25}
    #define CRYSTAL_FREQUENCY    10000000                                // 10 MHz crystal
    #define _EXTERNAL_CLOCK      CRYSTAL_FREQUENCY
    #define CLOCK_DIV            256                                     // input must be divided to 31.25kHz..39.06525kHz range (/1, 2, 4, 8, 16, 32, 64, 128, 256, 512 or 1024 possible)
                                                                         // this input is multipled by 1024 to 32MHz..40MHz at the FLL output
    #define SYSTEM_CLOCK_DIVIDE  2                                       // divide the FLL output to give the system clock (maximum 20MHz) (/1, 2, 4, 8, 16, 32, 64 or 128 possible)
    #define BUS_CLOCK_DIVIDE     1                                       // divide by 1 or 2 to give bus and flash clock (maximum 20MHz)
#elif defined FRDM_KE04Z
    #define CRYSTAL_FREQUENCY    8000000                                 // 8 MHz crystal
    #define _EXTERNAL_CLOCK      CRYSTAL_FREQUENCY
    #define CLOCK_DIV            256                                     // input must be divided to 31.25kHz..39.06525kHz range (/1, 2, 4, 8, 16, 32, 64, 128, 256, 512 or 1024 possible)
    #define SYSTEM_CLOCK_DIVIDE  1                                       // divide the clock output to give the system clock (maximum 40MHz) (/1, 2, 4, 8, 16, 32, 64 or 128 possible)
                                                                         // this input is multipled by 1280 to 32MHz..40MHz at the FLL output
    #if defined RUN_FROM_EXTERNAL_CLOCK
        #define BUS_CLOCK_DIVIDE 1                                       // divide by 1 or 2 to give bus and flash clock (maximum 20MHz)
    #else
        #define BUS_CLOCK_DIVIDE 2                                       // divide by 1 or 2 to give bus and flash clock (maximum 20MHz)
    #endif
#elif defined FRDM_KE02Z40M                                              // {30}
    #define CRYSTAL_FREQUENCY    10000000                                // 10 MHz crystal
    #define _EXTERNAL_CLOCK      CRYSTAL_FREQUENCY
    #define CLOCK_DIV            256                                     // input must be divided to 31.25kHz..39.06525kHz range (/1, 2, 4, 8, 16, 32, 64, 128, 256, 512 or 1024 possible)
                                                                         // this input is multipled by 1024 to 32MHz..40MHz at the FLL output
    #define SYSTEM_CLOCK_DIVIDE  1                                       // divide the FLL output to give the system clock (maximum 40MHz) (/1, 2, 4, 8, 16, 32, 64 or 128 possible)
    #define BUS_CLOCK_DIVIDE     2                                       // divide by 1 or 2 to give bus and flash clock (maximum 20MHz)
#else                                                                    // TWR_K40X256 or TWR_K40D100M
    #define CRYSTAL_FREQUENCY    8000000                                 // 8 MHz crystal
    #define _EXTERNAL_CLOCK      CRYSTAL_FREQUENCY
    #define CLOCK_DIV            4                                       // input must be divided to 2MHz..4MHz range (/1 to /25 possible)
    #define CLOCK_MUL            48                                      // the PLL multiplication factor to achieve operating frequency of 96MHz (x24 to x55 possible)
    #define USB_CLOCK_GENERATED_INTERNALLY                               // use USB clock from internal source rather than external pin
#endif

#if !defined SYSTEM_CLOCK_DIVIDE
    #define SYSTEM_CLOCK_DIVIDE  1                                       // 1 to 16 - usually 1
#endif
#if !defined BUS_CLOCK_DIVIDE
    #define BUS_CLOCK_DIVIDE     2                                       // 1 to 16
#endif
#if !defined FLEX_CLOCK_DIVIDE
    #define FLEX_CLOCK_DIVIDE    2                                       // 1 to 16
#endif
#if !defined FLASH_CLOCK_DIVIDE
    #define FLASH_CLOCK_DIVIDE   4                                       // 1 to 16
#endif


// Default settings can be over-ridden here by adding board-specific settings at the start of the list
//
#if defined KINETIS_K10
  //#define KINETIS_FLEX                                                 // X part with flex memory rather than N part with program Flash only
  //#define DEVICE_80_PIN
  //#define DEVICE_100_PIN
  //#define DEVICE_121_PIN
    #define DEVICE_144_PIN                                               // 144 pin package
    #define SIZE_OF_FLASH       (512 * 1024)                             // 512k FLASH (256 program Flash / 256 Flex)
    #define SIZE_OF_RAM         (64 * 1024)                              // 64k SRAM
#elif defined TWR_K20D72M
    #define KINETIS_FLEX                                                 // X part with flex memory rather than N part with program Flash only
    #define PIN_COUNT           PIN_COUNT_100_PIN                        // 100 pin LQFP
    #define PACKAGE_TYPE        PACKAGE_LQFP
    #define SIZE_OF_FLEXFLASH   (32 * 1024)                              // 32 Flex
    #define SIZE_OF_RAM         (64 * 1024)                              // 64k SRAM
    #define SIZE_OF_EEPROM      (2 * 1024)                               // 2k EEPROM
    #if defined FLEXFLASH_DATA
        #define SIZE_OF_FLASH   ((256 * 1024) + SIZE_OF_FLEXFLASH)       // 256k program FLASH plus data flash
    #else
        #define SIZE_OF_FLASH   (256 * 1024)                             // 256k program FLASH
    #endif
#elif defined TEENSY_3_1
    #define SUPPORT_SW_RTC                                               // support real time clock based purely on software (remove if 32kHz quarz is connected)
    #define KINETIS_FLEX                                                 // X part with flex memory rather than N part with program Flash only
    #define PIN_COUNT           PIN_COUNT_64_PIN                         // 64 pin LQFP
    #define PACKAGE_TYPE        PACKAGE_LQFP
    #define SIZE_OF_FLEXFLASH   (32 * 1024)                              // 32 Flex
    #define SIZE_OF_RAM         (64 * 1024)                              // 64k SRAM
    #define SIZE_OF_EEPROM      (2 * 1024)                               // 2k EEPROM
    #if defined FLEXFLASH_DATA
        #define SIZE_OF_FLASH   ((256 * 1024) + SIZE_OF_FLEXFLASH)       // 256k program FLASH plus data flash
    #else
        #define SIZE_OF_FLASH   (256 * 1024)                             // 256k program FLASH
    #endif
#elif defined K20FX512_120
    #define KINETIS_FLEX                                                 // X part with flex memory rather than N part with program Flash only
  //#define PIN_COUNT           PIN_COUNT_32_PIN
  //#define PIN_COUNT           PIN_COUNT_48_PIN
  //#define PIN_COUNT           PIN_COUNT_64_PIN                         // 64 pin package
  //#define PIN_COUNT           PIN_COUNT_80_PIN                         // LQFP80
  //#define PIN_COUNT           PIN_COUNT_81_PIN                         // MAPBGA81
  //#define PIN_COUNT           PIN_COUNT_100_PIN
  //#define PIN_COUNT           PIN_COUNT_121_PIN
    #define PIN_COUNT           PIN_COUNT_144_PIN
    #define PACKAGE_TYPE        PACKAGE_LQFP
    #define SIZE_OF_FLASH       (512 * 1024)                             // only consider program flash because flex flash is at 0x10000000
  //#define SIZE_OF_FLASH       (1024 * 1024)                            // 1M FLASH (512k program Flash / 512k Flex)
    #define SIZE_OF_FLEXFLASH   (512 * 1024)
    #define SIZE_OF_RAM         (128 * 1024)                             // 128k SRAM
    #define SIZE_OF_EEPROM      (16 * 1024)                              // 16k EEPROM
#elif defined TWR_K24F120M
  //#define PIN_COUNT           PIN_COUNT_100_PIN                        // 100 pin LQFP
    #define PIN_COUNT           PIN_COUNT_121_PIN                        // XBGA121
  //#define PIN_COUNT           PIN_COUNT_144_PIN                        // 144 pin LQFP
  //#define SIZE_OF_FLASH       (1024 * 1024)                            // 1024k program Flash
    #define SIZE_OF_FLASH       (256 * 1024)                             // 256k program Flash
    #define SIZE_OF_RAM         (256 * 1024)                             // 128k SRAM
#elif defined TWR_K21F120M
  //#define KINETIS_FLEX
    #define PIN_COUNT           PIN_COUNT_121_PIN                        // MAPBGA121
    #define SIZE_OF_FLASH       (1024 * 1024)                            // 1024k program Flash
    #define SIZE_OF_RAM         (128 * 1024)                             // 128k SRAM
#elif defined FRDM_K22F
    #define MASK_0N50M                                                   // mask relevant to this this device
    #define PIN_COUNT           PIN_COUNT_64_PIN                         // 64 LQFP pin package
    #define PACKAGE_TYPE        PACKAGE_LQFP
    #define SIZE_OF_FLASH       (512 * 1024)                             // 512k FLASH
    #define SIZE_OF_RAM         (128 * 1024)                             // 128k SRAM
#elif defined K22F128_100M
    #define MASK_0N74K                                                   // mask relevant to this this device
    #define PIN_COUNT           PIN_COUNT_64_PIN                         // 64 LQFP pin package
    #define PACKAGE_TYPE        PACKAGE_LQFP
    #define SIZE_OF_FLASH       (128 * 1024)                             // 128k FLASH
    #define SIZE_OF_RAM         (24 * 1024)                              // 24k SRAM
#elif defined BLAZE_K22
    #define MASK_0N50M                                                   // mask relevant to this this device
    #define PIN_COUNT           PIN_COUNT_64_PIN                         // 64 LQFP pin package
    #define PACKAGE_TYPE        PACKAGE_LQFP
    #define SIZE_OF_FLASH       (1024 * 1024)                            // 1M FLASH
    #define SIZE_OF_RAM         (128 * 1024)                             // 128k SRAM
#elif defined TWR_K22F120M
    #define PIN_COUNT           PIN_COUNT_121_PIN                        // 121 XFBGA package
    #define SIZE_OF_FLASH       (512 * 1024)                             // 512k FLASH
    #define SIZE_OF_RAM         (128 * 1024)                             // 128k SRAM
#elif defined TWR_K21D50M
    #define PIN_COUNT           PIN_COUNT_121_PIN                        // MAPBGA121
    #define SIZE_OF_FLASH       (512 * 1024)                             // 512k program Flash
    #define SIZE_OF_RAM         (64 * 1024)                              // 64k SRAM
#elif defined TWR_KW21D256
  //#define SIZE_OF_FLASH       (512 * 1024)                             // 512k program Flash
    #define SIZE_OF_FLASH       (256 * 1024)                             // 256k program Flash
  //#define SIZE_OF_RAM         (64 * 1024)                              // 64k SRAM
    #define SIZE_OF_RAM         (32 * 1024)                              // 32k SRAM
#elif defined TWR_KW24D512
    #define SIZE_OF_FLASH       (512 * 1024)                             // 512k program Flash
  //#define SIZE_OF_FLASH       (256 * 1024)                             // 256k program Flash
    #define SIZE_OF_RAM         (64 * 1024)                              // 64k SRAM
  //#define SIZE_OF_RAM         (32 * 1024)                              // 32k SRAM
#elif defined K02F100M
    #define MASK_0N36M                                                   // enable errata workarounds for this mask
  //#define PIN_COUNT           PIN_COUNT_32_PIN                         // 32 pin QFN package
  //#define PIN_COUNT           PIN_COUNT_48_PIN                         // 48 pin LQFP package
    #define PIN_COUNT           PIN_COUNT_64_PIN                         // 64 pin LQFP package
    #define SIZE_OF_RAM         (16 * 1024)                              // 16k SRAM
  //#define SIZE_OF_FLASH       (64 * 1024)                              // 64k program FLASH
    #define SIZE_OF_FLASH       (128 * 1024)                             // 128k program FLASH
#elif defined tinyK20
    #define KINETIS_FLEX                                                 // X part with flex memory rather than N part with program Flash only
    #define PIN_COUNT           PIN_COUNT_48_PIN
    #define PACKAGE_TYPE        PACKAGE_LQFP
    #define FLEXFLASH_DATA                                               // use FlexNMV in data mode
    #define SIZE_OF_FLEXFLASH   (32 * 1024)                              // 32 Flex
    #define SIZE_OF_RAM         (16 * 1024)                              // 16k SRAM
    #define SIZE_OF_EEPROM      (2 * 1024)                               // 2k EEPROM
    #if defined FLEXFLASH_DATA
        #define SIZE_OF_FLASH   ((128 * 1024) + SIZE_OF_FLEXFLASH)       // 128k program FLASH plus data flash
    #else
        #define SIZE_OF_FLASH   (128 * 1024)                             // 128k program FLASH
    #endif
#elif defined KINETIS_K20                                                // TWR_K20D50M/FRDM_K20D50M {2}
    #define KINETIS_FLEX                                                 // X part with flex memory rather than N part with program Flash only
  //#define PIN_COUNT           PIN_COUNT_32_PIN
  //#define PIN_COUNT           PIN_COUNT_48_PIN
    #define PIN_COUNT           PIN_COUNT_64_PIN                         // 64 pin package
  //#define PIN_COUNT           PIN_COUNT_80_PIN                         // LQFP80
  //#define PIN_COUNT           PIN_COUNT_81_PIN                         // MAPBGA81
  //#define PIN_COUNT           PIN_COUNT_100_PIN
  //#define PIN_COUNT           PIN_COUNT_121_PIN
  //#define PIN_COUNT           PIN_COUNT_144_PIN
    #define PACKAGE_TYPE        PACKAGE_LQFP
  //#define FLEXFLASH_DATA                                               // use FlexNMV in data mode
    #define SIZE_OF_FLEXFLASH   (32 * 1024)                              // 32 Flex
    #define SIZE_OF_RAM         (16 * 1024)                              // 16k SRAM
    #define SIZE_OF_EEPROM      (2 * 1024)                               // 2k EEPROM
    #if defined FLEXFLASH_DATA
        #define SIZE_OF_FLASH   ((128 * 1024) + SIZE_OF_FLEXFLASH)       // 128k program FLASH plus data flash
    #else
        #define SIZE_OF_FLASH   (128 * 1024)                             // 128k program FLASH
    #endif
#elif defined KINETIS_K30
  //#define KINETIS_FLEX                                                 // X part with flex memory rather than N part with program Flash only
  //#define DEVICE_80_PIN
  //#define DEVICE_100_PIN
  //#define DEVICE_121_PIN
    #define DEVICE_144_PIN                                               // 144 pin package
    #define SIZE_OF_FLEXFLASH   (256 * 1024)
    #define SIZE_OF_RAM         (64 * 1024)                              // 64k SRAM
    #if defined FLEXFLASH_DATA
        #define SIZE_OF_FLASH   ((256 * 1024) + SIZE_OF_FLEXFLASH)       // 256k program FLASH plus data flash
    #else
        #define SIZE_OF_FLASH   (256 * 1024)                             // 256k program FLASH
    #endif
#elif defined KINETIS_K40
    #define MASK_0M33Z                                                   // enable errata workarounds for this mask
    #define KINETIS_FLEX                                                 // X part with flex memory rather than N part with program Flash only
  //#define DEVICE_80_PIN
  //#define DEVICE_100_PIN
  //#define DEVICE_121_PIN
    #define DEVICE_144_PIN                                               // 144 pin package
    #define SIZE_OF_FLEXFLASH   (256 * 1024)
    #define SIZE_OF_RAM         (64 * 1024)                              // 64k SRAM
    #define SIZE_OF_EEPROM      (4 * 1024)                               // 4k EEPROM
  //#define FLEXFLASH_DATA                                               // use FlexNMV in data mode
    #if defined FLEXFLASH_DATA
        #define SIZE_OF_FLASH   ((256 * 1024) + SIZE_OF_FLEXFLASH)       // 256k program FLASH plus data flash
    #else
        #define SIZE_OF_FLASH   (256 * 1024)                             // 256k program FLASH
    #endif
#elif defined KINETIS_K50
    #define KINETIS_FLEX                                                 // X part with flex memory rather than N part with program Flash only
  //#define DEVICE_80_PIN
  //#define DEVICE_100_PIN
  //#define DEVICE_121_PIN
    #define DEVICE_144_PIN                                               // 144 pin package
    #define SIZE_OF_FLEXFLASH   (256 * 1024)
    #define SIZE_OF_RAM         (64 * 1024)                              // 64k SRAM
    #if defined FLEXFLASH_DATA
        #define SIZE_OF_FLASH   ((256 * 1024) + SIZE_OF_FLEXFLASH)       // 256k program FLASH plus data flash
    #else
        #define SIZE_OF_FLASH   (256 * 1024)                             // 256k program FLASH
    #endif
#elif defined KINETIS_K51
    #define KINETIS_FLEX                                                 // X part with flex memory rather than N part with program Flash only
    #define DEVICE_80_PIN
  //#define DEVICE_100_PIN
  //#define DEVICE_121_PIN
  //#define DEVICE_144_PIN                                               // 144 pin package
    #define SIZE_OF_FLEXFLASH   (256 * 1024)
    #define SIZE_OF_RAM         (64 * 1024)                              // 64k SRAM
    #if defined FLEXFLASH_DATA
        #define SIZE_OF_FLASH   ((256 * 1024) + SIZE_OF_FLEXFLASH)       // 256k program FLASH plus data flash
    #else
        #define SIZE_OF_FLASH   (256 * 1024)                             // 256k program FLASH
    #endif
#elif defined KINETIS_K52
    #define KINETIS_FLEX                                                 // X part with flex memory rather than N part with program Flash only
    #define DEVICE_144_PIN                                               // 144 pin package - only package available
    #define SIZE_OF_FLEXFLASH   (256 * 1024)
    #define SIZE_OF_RAM         (64 * 1024)                              // 64k SRAM
    #if defined FLEXFLASH_DATA
        #define SIZE_OF_FLASH   ((256 * 1024) + SIZE_OF_FLEXFLASH)       // 256k program FLASH plus data flash
    #else
        #define SIZE_OF_FLASH   (256 * 1024)                             // 256k program FLASH
    #endif
#elif defined TWR_K53N512
    #define MASK_0M33Z                                                   // enable errata workarounds for this mask
  //#define KINETIS_FLEX                                                 // X part with flex memory rather than N part with program Flash only
  //#define DEVICE_100_PIN
  //#define DEVICE_121_PIN
    #define DEVICE_144_PIN                                               // 144 pin package
    #define SIZE_OF_FLASH       (512 * 1024)                             // 512k FLASH
    #define SIZE_OF_RAM         (128 * 1024)                             // 128k SRAM
#elif defined KINETIS_K61
  //#define KINETIS_FLEX                                                 // X part with flex memory rather than N part with program Flash only        
    #define PIN_COUNT           PIN_COUNT_256_PIN                        // 256 MAPBGA pin package
  //#define PIN_COUNT           PIN_COUNT_196_PIN                        // 196 MAPBGA pin package
  //#define PIN_COUNT           PIN_COUNT_144_PIN                        // 144 MAPBGA pin package
    #define SIZE_OF_FLASH       (1024 * 1024)                            // 1024k FLASH
    #define SIZE_OF_RAM         (128 * 1024)                             // 128k SRAM
#elif defined TWR_K64F120M
  //#define KINETIS_FLEX                                                 // X part with flex memory rather than N part with program Flash only
  //#define PIN_COUNT           PIN_COUNT_100_PIN                        // 100 LQFP pin package
  //#define PIN_COUNT           PIN_COUNT_121_PIN                        // 121 XFBGA
    #define PIN_COUNT           PIN_COUNT_144_PIN                        // 144 LQFP/MAPBGA pin package
  //#define PACKAGE_TYPE        PACKAGE_LQFP
  //#define PACKAGE_TYPE        PACKAGE_MAPBGA
    #define SIZE_OF_FLASH       (1024 * 1024)                            // 1M FLASH
    #define SIZE_OF_RAM         (256 * 1024)                             // 256k SRAM
#elif defined FRDM_K64F || defined FreeLON
  //#define KINETIS_FLEX                                                 // X part with flex memory rather than N part with program Flash only
    #define PIN_COUNT           PIN_COUNT_100_PIN                        // 100 LQFP pin package
  //#define PIN_COUNT           PIN_COUNT_121_PIN                        // 121 XFBGA
  //#define PIN_COUNT           PIN_COUNT_144_PIN                        // 144 LQFP/MAPBGA pin package
    #define PACKAGE_TYPE        PACKAGE_LQFP
  //#define PACKAGE_TYPE        PACKAGE_MAPBGA
    #define SIZE_OF_FLASH       (1024 * 1024)                            // 1M FLASH
    #define SIZE_OF_RAM         (256 * 1024)                             // 256k SRAM
#elif defined TEENSY_3_5
    #define PIN_COUNT           PIN_COUNT_144_PIN                        // 169 pin package
    #define PACKAGE_TYPE        PACKAGE_MAPBGA
    #define KINETIS_FLEX                                                 // X part with flex memory rather than N part with program Flash only
    #define SIZE_OF_FLASH       (512 * 1024)                             // 1M FLASH
    #define SIZE_OF_RAM         (256 * 1024)                             // 256k SRAM
#elif defined TWR_K65F180M || defined FRDM_K66F
    #define MASK_0N65N
    #if defined FRDM_K66F
        #define PIN_COUNT       PIN_COUNT_144_PIN                        // 144 pin package
    #else
        #define PIN_COUNT       PIN_COUNT_169_PIN                        // 169 pin package
    #endif
    #define PACKAGE_TYPE        PACKAGE_MAPBGA
  //#define PACKAGE_TYPE        PACKAGE_WLCSP
  //#define KINETIS_FLEX                                                 // X part with flex memory rather than N part with program Flash only
    #define SIZE_OF_FLASH       (2 * 1024 * 1024)                        // 2M FLASH
    #define SIZE_OF_RAM         (256 * 1024)                             // 256k SRAM
#elif defined TEENSY_3_6
    #define MASK_0N65N
    #define PIN_COUNT           PIN_COUNT_144_PIN                        // 169 pin package
    #define PACKAGE_TYPE        PACKAGE_MAPBGA
  //#define PACKAGE_TYPE        PACKAGE_LQFP
    #define KINETIS_FLEX                                                 // X part with flex memory rather than N part with program Flash only
    #define SIZE_OF_FLASH       (1 * 1024 * 1024)                        // 1M FLASH
    #define SIZE_OF_RAM         (256 * 1024)                             // 256k SRAM
#elif defined FRDM_KL43Z || defined TWR_KL43Z48M || defined KL43Z_256_32_CL
    #define PIN_COUNT           PIN_COUNT_64_PIN                         // 64 pin LQFP or MAPBGA
    #define PACKAGE_TYPE        PACKAGE_LQFP                             // LQFP
  //#define PACKAGE_TYPE        PACKAGE_MAPBGA
    #define SIZE_OF_FLASH       (256 * 1024)                             // 256k program Flash
  //#define SIZE_OF_FLASH       (128 * 1024)
    #define SIZE_OF_RAM         (32 * 1024)                              // 32k SRAM
  //#define SIZE_OF_RAM         (16 * 1024)
#elif defined FRDM_KL46Z || defined TWR_KL46Z48M                         // {22}{23}
  //#define PIN_COUNT           PIN_COUNT_64_PIN
    #define PIN_COUNT           PIN_COUNT_100_PIN                        // 100 pin package
  //#define PIN_COUNT           PIN_COUNT_121_PIN
    #define PACKAGE_TYPE        PACKAGE_LQFP                             // LQFP
  //#define PACKAGE_TYPE        PACKAGE_MAPBGA
  //#define PACKAGE_TYPE        PACKAGE_BGA
    #define SIZE_OF_FLASH       (256 * 1024)                             // 256k program Flash
    #define SIZE_OF_RAM         (32 * 1024)                              // 32k SRAM
#elif defined TWR_KV10Z32
  //#define PIN_COUNT           PIN_COUNT_32_PIN
    #define PIN_COUNT           PIN_COUNT_48_PIN                         // 48 pin package
    #define PACKAGE_TYPE        PACKAGE_LQFP                             // LQFP
  //#define PACKAGE_TYPE        PACKAGE_QFN
    #define SIZE_OF_FLASH       (32 * 1024)                              // 32k program Flash
    #define SIZE_OF_RAM         (8 * 1024)                               // 8k SRAM
#elif defined TWR_KV31F120M || defined FRDM_KV31F
  //#define PIN_COUNT           PIN_COUNT_64_PIN
    #define PIN_COUNT           PIN_COUNT_100_PIN                        // 48 pin package
    #define SIZE_OF_FLASH       (512 * 1024)                             // 512k program Flash
    #define SIZE_OF_RAM         (96 * 1024)                              // 96k SRAM
#elif defined FRDM_KL25Z || defined TWR_KL25Z48M                         // {24}
  //#define PIN_COUNT           PIN_COUNT_32_PIN
  //#define PIN_COUNT           PIN_COUNT_48_PIN
  //#define PIN_COUNT           PIN_COUNT_64_PIN
    #define PIN_COUNT           PIN_COUNT_80_PIN                         // 80 pin package
    #define PACKAGE_TYPE        PACKAGE_LQFP                             // LQFP
  //#define PACKAGE_TYPE        PACKAGE_QFN
  //#define SIZE_OF_FLASH       (32 * 1024)                              // 32k program Flash
  //#define SIZE_OF_FLASH       (64 * 1024)                              // 64k program Flash
    #define SIZE_OF_FLASH       (128 * 1024)                             // 128k program Flash
  //#define SIZE_OF_FLASH       (256 * 1024)                             // 256k program Flash
  //#define SIZE_OF_RAM         (4 * 1024)                               // 4k SRAM
  //#define SIZE_OF_RAM         (8 * 1024)                               // 8k SRAM
    #define SIZE_OF_RAM         (16 * 1024)                              // 16k SRAM
  //#define SIZE_OF_RAM         (32 * 1024)                              // 32k SRAM
#elif defined FRDM_KL26Z                                                 // {24}
  //#define PIN_COUNT           PIN_COUNT_32_PIN
  //#define PIN_COUNT           PIN_COUNT_48_PIN
    #define PIN_COUNT           PIN_COUNT_64_PIN                         // 64 pin package
  //#define PIN_COUNT           PIN_COUNT_100_PIN
  //#define PIN_COUNT           PIN_COUNT_121_PIN
    #define PACKAGE_TYPE        PACKAGE_LQFP                             // LQFP
  //#define PACKAGE_TYPE        PACKAGE_QFN
  //#define PACKAGE_TYPE        PACKAGE_BGA
  //#define SIZE_OF_FLASH       (32 * 1024)                              // 32k program Flash
  //#define SIZE_OF_FLASH       (64 * 1024)                              // 64k program Flash
    #define SIZE_OF_FLASH       (128 * 1024)                             // 128k program Flash
  //#define SIZE_OF_FLASH       (256 * 1024)                             // 256k program Flash
  //#define SIZE_OF_RAM         (4 * 1024)                               // 4k SRAM
  //#define SIZE_OF_RAM         (8 * 1024)                               // 8k SRAM
    #define SIZE_OF_RAM         (16 * 1024)                              // 16k SRAM
  //#define SIZE_OF_RAM         (32 * 1024)                              // 32k SRAM
#elif defined rcARM_KL26
    #define PIN_COUNT           PIN_COUNT_64_PIN                         // 64 pin package
    #define PACKAGE_TYPE        PACKAGE_LQFP                             // LQFP
    #define SIZE_OF_FLASH       (256 * 1024)                             // 256k program Flash
    #define SIZE_OF_RAM         (32 * 1024)                              // 32k SRAM
#elif defined FRDM_KL27Z
  //#define PIN_COUNT           PIN_COUNT_32_PIN
  //#define PIN_COUNT           PIN_COUNT_36_PIN
  //#define PIN_COUNT           PIN_COUNT_48_PIN
    #define PIN_COUNT           PIN_COUNT_64_PIN                         // 64 pin package
    #define PACKAGE_TYPE        PACKAGE_LQFP                             // LQFP
  //#define PACKAGE_TYPE        PACKAGE_QFN
  //#define PACKAGE_TYPE        PACKAGE_MAPBGA
  //#define SIZE_OF_FLASH       (32 * 1024)                              // 32k program Flash
    #define SIZE_OF_FLASH       (64 * 1024)                              // 64k program Flash
  //#define SIZE_OF_FLASH       (128 * 1024)                             // 128k program Flash
  //#define SIZE_OF_FLASH       (256 * 1024)                             // 256k program Flash
  //#define SIZE_OF_RAM         (4 * 1024)                               // 4k SRAM
  //#define SIZE_OF_RAM         (8 * 1024)                               // 8k SRAM
    #define SIZE_OF_RAM         (16 * 1024)                              // 16k SRAM
  //#define SIZE_OF_RAM         (32 * 1024)                              // 32k SRAM
#elif defined CAPUCCINO_KL27
    #define PIN_COUNT           PIN_COUNT_64_PIN                         // 64 pin package
    #define PACKAGE_TYPE        PACKAGE_LQFP                             // LQFP
    #define SIZE_OF_FLASH       (256 * 1024)                             // 256k program Flash
    #define SIZE_OF_RAM         (32 * 1024)                              // 32k SRAM
#elif defined TEENSY_LC
    #define PIN_COUNT           PIN_COUNT_64_PIN                         // 64 pin package
    #define PACKAGE_TYPE        PACKAGE_QFN                              // QFN
    #define SIZE_OF_FLASH       (64 * 1024)                              // 64k program Flash
    #define SIZE_OF_RAM         (8 * 1024)                               // 8k SRAM
#elif defined FRDM_KL82Z
    #define MASK_0N51R                                                   // enable errata workarounds for this mask
  //#define PIN_COUNT           PIN_COUNT_64_PIN
    #define PIN_COUNT           PIN_COUNT_80_PIN                         // 80 pin package
  //#define PIN_COUNT           PIN_COUNT_100_PIN
  //#define PIN_COUNT           PIN_COUNT_121_PIN
    #define PACKAGE_TYPE        PACKAGE_LQFP                             // LQFP
  //#define PACKAGE_TYPE        PACKAGE_BGA
    #define SIZE_OF_FLASH       (128 * 1024)                             // 128k program Flash
    #define SIZE_OF_RAM         (96 * 1024)                              // 96k SRAM
#elif defined TRK_KEA8
  //#define PIN_COUNT           PIN_COUNT_16_PIN                         // 16 pin TSSOP
    #define PIN_COUNT           PIN_COUNT_24_PIN                         // 24 pin QFN
    #define SIZE_OF_FLASH       (64 * 1024)                              // 8k program Flash
    #define SIZE_OF_RAM         (1 * 1024)                               // 1k SRAM
    #define INTERRUPT_VECTORS_IN_FLASH                                   // since RAM is limited interrupt vectors are fixed in flash
#elif defined FRDM_KEAZN32Q64
  //#define PIN_COUNT           PIN_COUNT_32_PIN                         // 32 pin LQFP
    #define PIN_COUNT           PIN_COUNT_64_PIN                         // 64 pin LQFP
    #define SIZE_OF_FLASH       (32 * 1024)                              // 32k program Flash
  //#define SIZE_OF_FLASH       (64 * 1024)
  //#define SIZE_OF_RAM         (2 * 1024)
    #define SIZE_OF_RAM         (4 * 1024)                               // 4k SRAM
#elif defined TRK_KEA64 || defined FRDM_KEAZ64Q64
  //#define PIN_COUNT           PIN_COUNT_32_PIN                         // 32 pin LQFP
    #define PIN_COUNT           PIN_COUNT_64_PIN                         // 64 pin LQFP
  //#define SIZE_OF_FLASH       (32 * 1024)
    #define SIZE_OF_FLASH       (64 * 1024)                              // 64k program Flash
  //#define SIZE_OF_RAM         (2 * 1024)
    #define SIZE_OF_RAM         (4 * 1024)                               // 4k SRAM
#elif defined FRDM_KE06Z || defined TRK_KEA128 || defined FRDM_KEAZ128Q80 // {30}
  //#define PIN_COUNT           PIN_COUNT_44_PIN                         // 44 pin LQFP
  //#define PIN_COUNT           PIN_COUNT_64_PIN                         // 64 pin (L)QFP
    #define PIN_COUNT           PIN_COUNT_80_PIN                         // 80 pin LQFP
  //#define SIZE_OF_FLASH       (64 * 1024)
    #define SIZE_OF_FLASH       (128 * 1024)                             // 128k Flash
  //#define SIZE_OF_RAM         (8 * 1024)
    #define SIZE_OF_RAM         (16 * 1024)                              // 16k SRAM
#elif defined FRDM_KE04Z
  //#define PIN_COUNT           PIN_COUNT_16_PIN                         // 16 pin TSSOP
  //#define PIN_COUNT           PIN_COUNT_20_PIN                         // 20 pin SOIC
    #define PIN_COUNT           PIN_COUNT_24_PIN                         // 24 pin QFN
  //#define PIN_COUNT           PIN_COUNT_64_PIN                         // 64 pin LQFP
  //#define PIN_COUNT           PIN_COUNT_80_PIN                         // 80 pin LQFP
  //#define SIZE_OF_FLASH       (128 * 1024)
  //#define SIZE_OF_FLASH       (64 * 1024)
    #define SIZE_OF_FLASH       (8 * 1024)                               // 8k program Flash
  //#define SIZE_OF_RAM         (16 * 1024)
  //#define SIZE_OF_RAM         (8 * 1024)
    #define SIZE_OF_RAM         (1 * 1024)                               // 1k SRAM
    #define INTERRUPT_VECTORS_IN_FLASH                                   // since RAM is limited interrupt vectors are fixed in flash
#elif defined FRDM_KE02Z || defined FRDM_KE02Z40M                        // {25}{30}
  //#define PIN_COUNT           PIN_COUNT_32_PIN                         // 32 pin LQFP
  //#define PIN_COUNT           PIN_COUNT_44_PIN                         // 44 pin LQFP
    #define PIN_COUNT           PIN_COUNT_64_PIN                         // 64 pin QFP
    #define SIZE_OF_EEPROM      256
  //#define SIZE_OF_FLASH       ((16 * 1024) + SIZE_OF_EEPROM)
  //#define SIZE_OF_FLASH       ((32 * 1024) + SIZE_OF_EEPROM)
    #define SIZE_OF_FLASH       ((64 * 1024) + SIZE_OF_EEPROM)           // 64k program Flash
  //#define SIZE_OF_RAM         (2 * 1024)
    #define SIZE_OF_RAM         (4 * 1024)                               // 4k SRAM
#elif defined FRDM_KL03Z
    #define MASK_1N86K                                                   // enable errata workarounds for this mask
  //#define PIN_COUNT           PIN_COUNT_16_PIN                         // 16 pin QFN
  //#define PIN_COUNT           PIN_COUNT_20_PIN                         // 20 pin WLCSP
    #define PIN_COUNT           PIN_COUNT_24_PIN                         // 24 pin QFN
  //#define SIZE_OF_FLASH       (8 * 1024)
  //#define SIZE_OF_FLASH       (16 * 1024)
    #define SIZE_OF_FLASH       (32 * 1024)                              // 32k program Flash
    #define SIZE_OF_RAM         (2 * 1024)                               // 2k SRAM
    #define INTERRUPT_VECTORS_IN_FLASH                                   // since RAM is limited interrupt vectors are fixed in flash
#elif defined FRDM_KL02Z                                                 // {25}
  //#define PIN_COUNT           PIN_COUNT_16_PIN                         // 16 pin QFN
  //#define PIN_COUNT           PIN_COUNT_20_PIN                         // 20 pin WLCSP
  //#define PIN_COUNT           PIN_COUNT_24_PIN                         // 24 pin QFN
    #define PIN_COUNT           PIN_COUNT_32_PIN                         // 32 pin QFN
  //#define SIZE_OF_FLASH       (8 * 1024)
  //#define SIZE_OF_FLASH       (16 * 1024)
    #define SIZE_OF_FLASH       (32 * 1024)                              // 32k program Flash
  //#define SIZE_OF_RAM         (1 * 1024)
  //#define SIZE_OF_RAM         (2 * 1024)
    #define SIZE_OF_RAM         (4 * 1024)                               // 4k SRAM
#elif defined FRDM_KL05Z                                                 // {25}
  //#define PIN_COUNT           PIN_COUNT_24_PIN
    #define PIN_COUNT           PIN_COUNT_32_PIN                         // 32 pin package
  //#define PIN_COUNT           PIN_COUNT_48_PIN
    #define PACKAGE_TYPE        PACKAGE_QFN                              // QFN
  //#define PACKAGE_TYPE        PACKAGE_LQFP
  //#define SIZE_OF_FLASH       (8 * 1024)
  //#define SIZE_OF_FLASH       (16 * 1024)
    #define SIZE_OF_FLASH       (32 * 1024)                              // 32k program Flash
  //#define SIZE_OF_RAM         (1 * 1024)
  //#define SIZE_OF_RAM         (2 * 1024)
    #define SIZE_OF_RAM         (4 * 1024)                               // 4k SRAM
#elif defined TWR_K60F120M || defined K60F150M_50M
  //#define KINETIS_FLEX                                                 // X part with flex memory rather than N part with program Flash only
  //#define PIN_COUNT_100_PIN
  //#define PIN_COUNT_121_PIN
    #define PIN_COUNT           PIN_COUNT_144_PIN                        // 144 MAPBGA pin package
    #define PACKAGE_TYPE        PACKAGE_LQFP
  //#define PACKAGE_TYPE        PACKAGE_MAPBGA
    #define PIN_COUNT           PIN_COUNT_144_PIN                        // 144 LQFP pin package
    #define PACKAGE_TYPE        PACKAGE_LQFP
  //#define SIZE_OF_FLASH       (256 * 1024)                             // 256k FLASH
  //#define SIZE_OF_FLASH       (512 * 1024)                             // 512k FLASH
    #define SIZE_OF_FLASH       (1024 * 1024)                            // 1M FLASH
  //#define SIZE_OF_RAM         (64 * 1024)                              // 128k SRAM
    #define SIZE_OF_RAM         (128 * 1024)                             // 128k SRAM
#elif defined K70F150M_12M
    #define PIN_COUNT           PIN_COUNT_256_PIN
    #define SIZE_OF_FLASH       (1024 * 1024)                            // 1M FLASH
    #define SIZE_OF_RAM         (128 * 1024)                             // 128k SRAM
#elif defined EMCRAFT_K70F120M || defined TWR_K70F120M
  //#define KINETIS_FLEX                                                 // X part with flex memory rather than N part with program Flash only        
    #define PIN_COUNT           PIN_COUNT_256_PIN                        // 256 pin MAPBGA package
  //#define PIN_COUNT           PIN_COUNT_196_PIN                        // 196 pin MAPBGA package
    #define SIZE_OF_FLASH       (1024 * 1024)                            // 1024k FLASH
    #define SIZE_OF_RAM         (128 * 1024)                             // 128k SRAM
#elif defined TWR_K80F150M
  //#define PIN_COUNT           PIN_COUNT_100_PIN                        // 100 pin LQFP package
    #define PIN_COUNT           PIN_COUNT_121_PIN                        // 121 pin XFBGA/WLCSP package
  //#define PIN_COUNT           PIN_COUNT_144_PIN                        // 141 pin LQFP package
  //#define PACKAGE_TYPE        PACKAGE_WLCSP                            // WLCSP
    #define PACKAGE_TYPE        PACKAGE_BGA                              // XFBGA
    #define SIZE_OF_FLASH       (256 * 1024)                             // 256k FLASH
    #define SIZE_OF_RAM         (256 * 1024)                             // 256k SRAM
#elif defined FRDM_K82F
    #define PIN_COUNT           PIN_COUNT_100_PIN                        // 100 pin LQFP package
  //#define PIN_COUNT           PIN_COUNT_121_PIN                        // 121 pin XFBGA/WLCSP package
  //#define PIN_COUNT           PIN_COUNT_144_PIN                        // 141 pin LQFP package
  //#define PACKAGE_TYPE        PACKAGE_WLCSP                            // WLCSP
  //#define PACKAGE_TYPE        PACKAGE_BGA                              // XFBGA
    #define SIZE_OF_FLASH       (256 * 1024)                             // 256k FLASH
    #define SIZE_OF_RAM         (256 * 1024)                             // 256k SRAM
    #define QSPI_FILE_SYSTEM                                             // user QSPI interface
#elif defined KINETIS_K60
  //#define KINETIS_FLEX                                                 // X part with flex memory rather than N part with program Flash only
  //#define PIN_COUNT_100_PIN
  //#define PIN_COUNT_121_PIN
    #define PIN_COUNT           PIN_COUNT_144_PIN                        // 144 MAPBGA pin package
    #if defined NET_K60                                                  // {16}
        #define PACKAGE_TYPE    PACKAGE_LQFP
    #else
        #define PACKAGE_TYPE    PACKAGE_MAPBGA
    #endif
    #define SIZE_OF_FLASH       (512 * 1024)                             // 512k FLASH
    #define SIZE_OF_RAM         (128 * 1024)                             // 128k SRAM
#endif


//#define USE_SECTION_PROGRAMMING                                        // allow the flash section command to be used to accelerate programming (faster than word programming but blocks interrupts for longer)

//#define STOP_MII_CLOCK                                                 // only apply clock when needed (don't use when DEVELOP_PHY_CONTROL is set in debug.c)
#if !defined DEVICE_WITHOUT_ETHERNET
    #define EMAC_ENHANCED                                                // use enhanced mode for Ethernet controller operation
        #define EMAC_IEEE1588                                            // enable IEEE 1588 time stamping (needs EMAC_ENHANCED)
#endif

#if defined NET_KBED || defined NET_K60                                  // {16}
    #define PHY_KSZ8031                                                  // use this PHY type
  //#define JTAG_DEBUG_IN_USE_ERRATA_2541                                // pull the optional MII0_RXER line down to 0V to avoid disturbing JTAG_TRST - not needed when using SWD for debugging 
    #define ETHERNET_RMII                                                // RMII mode of operation instead of MII
    #define FORCE_PHY_CONFIG                                             // activate forced configuration
    #define POLL_PHY               10000                                 // PHY detection is unreliable on this board so allow this many attempts
    #define FNFORCE_PHY_CONFIG()   PHY_RESET_HIGH()
    #define PHY_ADDRESS            0x00                                  // address of  PHY on board
    #if defined NET_K60                                                  // {16}
        #define PHY_INTERRUPT_PORT     PORTA
        #define PHY_INTERRUPT          PORTA_BIT11                       // PHY interrupt input 
    #else	    
        #define PHY_INTERRUPT_PORT     PORTB
        #define PHY_INTERRUPT          PORTB_BIT22                       // PHY interrupt input 
    #endif
    #define PHY_IDENTIFIER         0x00221555                            // MICREL KSZ8031NL identifier
    #define FNRESETPHY()
    #define MII_MANAGEMENT_CLOCK_SPEED    2500000                        // typ. 2,5MHz Speed
#elif defined EMCRAFT_K70F120M || defined EMCRAFT_K61F150M               // {9}
  //#define JTAG_DEBUG_IN_USE_ERRATA_2541                                // pull the optional MII0_RXER line down to 0V to avoid disturbing JTAG_TRST - not needed when using SWD for debugging 
    #define ETHERNET_RMII                                                // RMII mode of operation instead of MII
    #define FORCE_PHY_CONFIG                                             // activate forced configuration
    #define POLL_PHY               10000                                 // PHY detection is unreliable on this board so allow this many attempts
    #define FNFORCE_PHY_CONFIG()   
    #define PHY_ADDRESS            0x01                                  // address of external PHY on board
    #define PHY_INTERRUPT_PORT     PORTA
    #define PHY_INTERRUPT          PORTA_BIT27                           // IRQ1 is used as PHY interrupt input this is connected to PA.27
    #define PHY_IDENTIFIER         0x00221550                            // MICREL KSZ8051 identifier
    #define FNRESETPHY()
    #define MII_MANAGEMENT_CLOCK_SPEED   2500000                         // 2.5MHz
#elif defined TWR_K60F120M || defined K60F150M_50M || defined TWR_K70F120M || defined TWR_K53N512 || defined TWR_VF65GS10
  //#define JTAG_DEBUG_IN_USE_ERRATA_2541                                // pull the optional MII0_RXER line down to 0V to avoid disturbing JTAG_TRST - not needed when using SWD for debugging 
    #if defined K60F150M_50M
        #define ETHERNET_RMII                                            // RMII mode of operation instead of MII
        #define FORCE_PHY_CONFIG                                         // activate forced configuration
        #define POLL_PHY               10000                             // PHY detection is unreliable on this board so allow this many attempts
        #define PHY_ADDRESS            0x01                              // address of external PHY on board
        #define PHY_INTERRUPT_PORT     PORTB
        #define PHY_INTERRUPT          PORTB_BIT7
        #define PHY_IDENTIFIER         0x00221550                        // MICREL KSZ8051RNL identifier
        #define RESET_PHY
        #define ASSERT_PHY_RST()

      //#define CONFIG_PHY_STRAPS()    _CONFIG_DRIVE_PORT_OUTPUT_VALUE_FAST_LOW(B, PORTB_BIT9, 0, (PORT_SRE_SLOW | PORT_DSE_LOW)); _CONFIG_DRIVE_PORT_OUTPUT_VALUE_FAST_LOW(A, PORTA_BIT14, 0, (PORT_SRE_SLOW | PORT_DSE_LOW)) // assert PHY reset and drive FEC_CRS_DV low (PHY config2 input)
      //#define FNFORCE_PHY_CONFIG()   _SETBITS(B, PORTB_BIT9); fnDelayLoop(1000) // negate PHY reset with CONFIG2 held low

        #define CONFIG_PHY_STRAPS()    _CONFIG_DRIVE_PORT_OUTPUT_VALUE_FAST_LOW(A, (PORTA_BIT4 | PORTA_BIT14), 0, (PORT_SRE_SLOW | PORT_DSE_LOW))
        #define FNFORCE_PHY_CONFIG()   _SETBITS(A, PORTA_BIT4); fnDelayLoop(1000) // negate PHY reset with CONFIG2 held low
        #define _KSZ8051RNL                                              // specify phy type used
        #define FNRESETPHY()
    #elif defined TWR_SER2                                               // {17}
        #define ETHERNET_RMII                                            // RMII mode of operation instead of MII
        #define PHY_POLL_LINK                                            // poll the link status since there is no interrupt connected
        #define INTERRUPT_TASK_PHY     TASK_NETWORK_INDICATOR            // link status reported to this task (do not use together with LAN_REPORT_ACTIVITY)

        #define PHY_ADDRESS            0x00                              // address of external PHY on board - channel A (channel B is 0x01)
        #define FNRESETPHY()
        #define PHY_IDENTIFIER         0x20005ca2                        // National/TI DP83849I identifier
        #define _DP83849I
    #else  
        #define ETHERNET_RMII                                            // RMII mode of operation instead of MII
        #define FORCE_PHY_CONFIG                                         // activate forced configuration
        #define POLL_PHY               10000                             // PHY detection is unreliable on this board so allow this many attempts
        #define FNFORCE_PHY_CONFIG()   
        #define PHY_ADDRESS            0x01                              // address of external PHY on board
        #define PHY_INTERRUPT_PORT     PORTB
        #define PHY_INTERRUPT          PORTB_BIT7                        // IRQ4 is used as PHY interrupt input (set J6 to position 7-8 on TWR-SER board) - this is connected to PB.7
        #define PHY_IDENTIFIER         0x00221512                        // MICREL KSZ8041NL identifier
        #define FNRESETPHY()
        #if defined TWR_K53N512                                          // this tower board has a port output controlling the reset line on the elevator - it is set to an output driving '1' to avoid the PHY being held in reset
            #define RESET_PHY
            #define ASSERT_PHY_RST() _CONFIG_DRIVE_PORT_OUTPUT_VALUE_FAST_LOW(C, (RESETOUT), (RESETOUT), (PORT_SRE_SLOW | PORT_DSE_LOW))
            #define CONFIG_PHY_STRAPS()                                  // dummy
        #endif
    #endif
    // See below for MII_MANAGEMENT_CLOCK_SPEED since it depends on the system clock (after #include "../../Hardware/Kinetis/kinetis.h")
    //
#elif defined TWR_K60N512 || defined TWR_K60D100M || defined KINETIS_K52 || defined KINETIS_K61 || defined TWR_K64F120M || defined TWR_K65F180M || defined FRDM_K66F || defined KINETIS_K70
  //#define JTAG_DEBUG_IN_USE_ERRATA_2541                                // pull the optional MII0_RXER line down to 0V to avoid disturbing JTAG_TRST - not needed when using SWD for debugging
    #if defined TWR_K65F180M || defined FRDM_K66F
        #define ETHERNET_RMII_CLOCK_INPUT                                // the ENET_1588_CLKIN is used as clock since a 50MHz PHY clock is not available on EXTAL
    #endif
    #if defined TWR_SER2                                                 // {17}
        #define ETHERNET_RMII                                            // RMII mode of operation instead of MII
        #define PHY_POLL_LINK                                            // poll the link status since there is no interrupt connected
        #define INTERRUPT_TASK_PHY     TASK_NETWORK_INDICATOR            // link status reported to this task (do not use together with LAN_REPORT_ACTIVITY)

        #define PHY_ADDRESS            0x00                              // address of external PHY on board - channel A (channel B is 0x01)
        #define FNRESETPHY()
        #define PHY_IDENTIFIER         0x20005ca2                        // National/TI DP83849I identifier
        #define _DP83849I
    #else                                                                // MICREL PHY KS8041 on tower SER board
        #define ETHERNET_RMII                                            // RMII mode of operation instead of MII
        #define FORCE_PHY_CONFIG                                         // activate forced configuration
        #define FNFORCE_PHY_CONFIG()
        #if defined TWR_K65F180M || defined FRDM_K66F
            #define INTERRUPT_TASK_PHY     TASK_NETWORK_INDICATOR        // link status reported to this task (do not use together with LAN_REPORT_ACTIVITY)
            #define PHY_ADDRESS            0x00                          // address of external PHY on board
            #define PHY_INTERRUPT_PORT     PORTE
            #define PHY_INTERRUPT          PORTE_BIT28                   // IRQ1 is used as PHY interrupt input (set J6 to position 7-8 on TWR-SER board) - this is connected to PTD15
            #define ETHERNET_MDIO_WITH_PULLUPS                           // there is no pull-up on the tower board so enable one at the MDIO input
        #elif defined TWR_K64F120M                                       // for ethernet operation the clock settings must be J32-ON (and J33-OFF)
            #define INTERRUPT_TASK_PHY     TASK_NETWORK_INDICATOR        // link status reported to this task (do not use together with LAN_REPORT_ACTIVITY)
            #define PHY_ADDRESS            0x00                          // address of external PHY on board
            #define PHY_INTERRUPT_PORT     PORTD
            #define PHY_INTERRUPT          PORTD_BIT15                   // IRQ1 is used as PHY interrupt input (set J6 to position 7-8 on TWR-SER board) - this is connected to PTD15
            #define ETHERNET_MDIO_WITH_PULLUPS                           // there is no pull-up on the tower board so enable one at the MDIO input
        #else
            #define POLL_PHY               10000                         // PHY detection is unreliable on this board so allow this many attempts
            #define PHY_ADDRESS            0x01                          // address of external PHY on board
            #define PHY_INTERRUPT_PORT     PORTA
            #define PHY_INTERRUPT          PORTA_BIT27                   // IRQ1 is used as PHY interrupt input (set J6 to position 7-8 on TWR-SER board) - this is connected to PA.27
        #endif
        #define PHY_IDENTIFIER             0x00221512                    // MICREL KSZ8041NL identifier
        #define FNRESETPHY()
    #endif
    #define MII_MANAGEMENT_CLOCK_SPEED     800000                        // reduced speed due to weak data line pull up resistor and long back-plane distance (warning - too low results in a divider overflow in MSCR)
#elif defined FRDM_K64F || defined FreeLON
    #define ETHERNET_RMII                                                // RMII mode of operation instead of MII
    #define FORCE_PHY_CONFIG                                             // activate forced configuration
    #define FNFORCE_PHY_CONFIG()   
    #define PHY_ADDRESS            0x00                                  // address of external PHY on board
    #define PHY_IDENTIFIER         0x00221560                            // MICREL KSZ8081RNA identifier
    #define FNRESETPHY()
    #define MII_MANAGEMENT_CLOCK_SPEED    2500000                        // typ. 2.5MHz Speed
    #define ETHERNET_MDIO_WITH_PULLUPS                                   // there is no pull-up on the FRDM board so enable one at the MDIO input
    #if defined FreeLON
        #define INTERRUPT_TASK_PHY     TASK_NETWORK_INDICATOR            // link status reported to this task (do not use together with LAN_REPORT_ACTIVITY)
        #define PHY_INTERRUPT_PORT     PORTA
        #define PHY_INTERRUPT          PORTA_BIT4                        // IRQ1 is used as PHY interrupt input (set J6 to position 7-8 on TWR-SER board) - this is connected to PTD15
    #else
        #define PHY_POLL_LINK                                            // no interrupt line connected so poll the link state
        #define INTERRUPT_TASK_PHY     TASK_NETWORK_INDICATOR            // enable link state output messages
    #endif
    #define _KSZ8081RNA
#endif

// If a design doesn't have the phy interrupt line connected, the following can be used to cause the network indicator task to poll the state of the link and synchronise the Ethernet controller accordingly
//
//#define PHY_POLL_LINK
//#define INTERRUPT_TASK_PHY     TASK_NETWORK_INDICATOR                  // link status reported to this task (do not use together with LAN_REPORT_ACTIVITY)

// Designs using KSZ8863 switch can use tail tagging to control the two switch ports independently
//
//#define _PHY_KSZ8863
#if defined _PHY_KSZ8863
    #define PHY_MULTI_PORT                                               // phy has multiple ports
    #define PHY_TAIL_TAGGING                                             // enable tail tagging operation
    #define PHY_MICREL_SMI                                               // enable smi interface to phy
    #undef PHY_ADDRESS
    #define PHY_ADDRESS            2                                     // use 0x02 since 0x01 may not correctly return the PHYID
    #define PHY_ADDRESS_2          1
#endif

#define PHY_MASK                   0xfffffff0                            // don't check the revision number
#if defined PHY_INTERRUPT && !defined SUPPORT_PORT_INTERRUPTS            // port interrupt support must be enabled to use the PHY interrupt
    #define SUPPORT_PORT_INTERRUPTS
#endif

#if !defined ETH_INTERFACE
    #undef INTERRUPT_TASK_PHY
    #undef PHY_POLL_LINK
    #undef LAN_REPORT_ACTIVITY
#endif

#if defined KINETIS_KE
  //#define TICK_USES_RTC                                                // use RTC for TICK so that it continues to operate in stop based low power modes
    #if defined TICK_USES_RTC
        #define RTC_USES_EXT_CLK                                         // use the external clock as RTC clock source
      //#define RTC_USES_INT_REF                                         // use the internal reference clock (ICSIRCLK) as RTC clock source
            #define RTC_CLOCK_PRESCALER_1  32                            // 1, 2, 4, 8, 16, 32 or 64 (valid for internal reference or external clock input)
      //#define RTC_USES_BUS_CLOCK                                       // use the bus clock as RTC clock source
      //#define RTC_USES_LPO_1kHz                                        // use the 1kHz LPO clock as RTC source (not high accuracy)
            #define RTC_CLOCK_PRESCALER_2  100                           // 128, 256, 512, 1024, 2048, 100 or 1000 (valid for bus clock or 1kHz LPO clock)
    #endif
#else
  //#define SUPPORT_LPTMR                                                // {28} support low power timer
    #if defined SUPPORT_LPTMR
        #define TICK_USES_LPTMR                                          // use low power timer for TICK so that it continues to operate in stop based low power modes
        //Select the clock used by the low power timer - if the timer if to continue running in low power modes the clock chosen should continue to run in that mode too
        //
        #define LPTMR_CLOCK_LPO                                          // clock the low power timer from LPO (1kHz)
      //#define LPTMR_CLOCK_INTERNAL_30_40kHz                            // clock the low power timer from the 30..40kHz internal reference
      //#define LPTMR_CLOCK_INTERNAL_4MHz                                // clock the low power timer from the 4MHz internal reference
      //#define LPTMR_CLOCK_EXTERNAL_32kHz                               // clock the low power timer from external 32kHz reference
      //#define LPTMR_CLOCK_OSCERCLK                                     // clock the low power timer from the external reference
        #if defined FRDM_K64F || defined FreeLON
          //#define LPTMR_PRESCALE   64                                  // when using the external oscillator add this pre-scaler value to the low power timer input
            #undef LPTMR_CLOCK_OSCERCLK
            #define LPTMR_CLOCK_LPO                                      // run the low power timer from the 1kHz low power clock
        #elif defined FRDM_KL02Z
            #undef LPTMR_CLOCK_OSCERCLK
            #define LPTMR_CLOCK_EXTERNAL_32kHz                           // run the low power timer from the 32kHz oscillator
        #elif defined FRDM_KL46Z
            #define LPTMR_PRESCALE   32                                  // when using the external oscillator add this pre-scaler value to the low power timer input
        #endif
    #endif
#endif


//#define SUPPORT_TIMER                                                  // support hardware timer interrupt configuration (FlexTimer or TPM)

#if defined KINETIS_KL
  //#define TPM_CLOCKED_FROM_OSCERCLK                                    // TPM clock is connected to OSCERCLK (external crystal or oscillator)
    #define TPM_CLOCKED_FROM_MCGIRCLK                                    // TPM clock is connected to MCGIRCLK (either 32kHz or 4MHz)
    #define USE_FAST_INTERNAL_CLOCK
                                                                         // default is to use MCGPLLCLK/2 or MCGFLLCLK (depending on whether FLL or PLL is used)
#endif

#if defined KINETIS_KV || defined KINETIS_KL02 || defined KINETIS_K02    // device without RTC
    #define SUPPORT_SW_RTC                                               // support real time clock based purely on software
#else
    #define SUPPORT_RTC                                                  // support real time clock
    #define ALARM_TASK   TASK_APPLICATION                                // alarm is handled by the application task (handled by time keeper if not defined)
    #if defined TWR_KL46Z48M || defined TWR_KL43Z48M
        #define RTC_USES_RTC_CLKIN                                       // TWR-KL46Z48M and TWR-KL43Z48M have a 32kHz oscillator supplying an accurate clock and the OpenSDA interface supplies a clock on the FRDM-KL46Z as long as the debug interface is powered (not possible with P&E debugger version)
    #elif defined KINETIS_KL && !defined FRDM_KL03Z && !defined FRDM_KL27Z // FRDM-KL03Z and FRDM-KL27Z have 32kHz crystals which are used as preference
        #define RTC_USES_LPO_1kHz                                        // use the 1kHz LPO clock as RTC source (not high accuracy)
      //#define RTC_USES_RTC_CLKIN                                       // 32.768kHz input on RTC_CLKIN
    #elif defined KINETIS_KE && defined SUPPORT_RTC
      //#define RTC_USES_EXT_CLK                                         // use the external clock as RTC clock source
        #define RTC_USES_INT_REF                                         // use the internal reference clock (ICSIRCLK - 32768Hz) as RTC clock source
            #define RTC_CLOCK_PRESCALER_1  64                            // 1, 2, 4, 8, 16, 32 or 64 (valid for internal reference or external clock input)
      //#define RTC_USES_BUS_CLOCK                                       // use the bus clock as RTC clock source
      //#define RTC_USES_LPO_1kHz                                        // use the 1kHz LPO clock as RTC source (not high accuracy)
            #define RTC_CLOCK_PRESCALER_2  100                           // 128, 256, 512, 1024, 2048, 100 or 1000 (valid for bus clock or 1kHz LPO clock)
    #endif
#endif

// Include the Kinetis hardware header here
// - beware that the header delivers rules for subsequent parts of this header file but also accepts some rules from previous parts,
// therefore its position should only be moved after careful consideration of its consequences
//
#include "../../Hardware/Kinetis/kinetis.h"                              // include the Kinetis processor header at this location

#if defined RNG_AVAILABLE
    #define RND_HW_SUPPORT                                               // enable the use of the hardware resources in this chip
#endif

#if defined TWR_K60F120M || defined K60F150M_50M || defined TWR_K70F120M || defined TWR_K53N512 || defined TWR_VF65GS10
    #if defined K60F150M_50M
        #define MII_MANAGEMENT_CLOCK_SPEED   2500000                     // 2.5MHz
    #else
        #if SYSTEM_CLOCK > 100000000
            #define MII_MANAGEMENT_CLOCK_SPEED   1000000                 // reduced speed due to weak data line pull up resistor and long back-plane distance (warning - too low results in a divider overflow in MSCR)
        #else
            #define MII_MANAGEMENT_CLOCK_SPEED   800000                  // reduced speed due to weak data line pull up resistor and long back-plane distance (warning - too low results in a divider overflow in MSCR)
        #endif
    #endif
#endif

#define TICK_INTERRUPT()                                                 // user callback from system TICK (dummy if left empty)

#if defined MONITOR_PERFORMANCE
    #if PITS_AVAILABLE > 2                                               // PIT3 is used to monitor task durations
        #define INITIALISE_MONITOR_TIMER()        POWER_UP(6, SIM_SCGC6_PIT); PIT_MCR = 0; LOAD_PIT(3, 0xffffffff); PIT_TCTRL3 = PIT_TCTRL_TEN
        #define EXECUTION_DURATION()              (0xffffffff - PIT_CVAL3); LOAD_PIT(3, 0xffffffff); PIT_TCTRL3 = PIT_TCTRL_TEN // read the elapsed count value and reset the counter back to 0xffffffff
    #else                                                                // PIT0 is used to monitor task durations
        #define INITIALISE_MONITOR_TIMER()        POWER_UP(6, SIM_SCGC6_PIT); PIT_MCR = 0; LOAD_PIT(0, 0xffffffff); PIT_TCTRL0 = PIT_TCTRL_TEN
        #define EXECUTION_DURATION()              (0xffffffff - PIT_CVAL0); LOAD_PIT(0, 0xffffffff); PIT_TCTRL0 = PIT_TCTRL_TEN // read the elapsed count value and reset the counter back to 0xffffffff
    #endif
    #define PIT_TIMER_USED_BY_PERFORMANCE_MONITOR                        // since a PIT timer is used for the monitoring function don't allow PITS to be powered down
#endif


// SPI FLASH system setup
//
//#define SPI_FLASH_MULTIPLE_CHIPS                                       // activate when multiple physical chips are used

#if defined NET_KBED                                                     // {16}
    #if defined SPI_FLASH_S25FL1_K
        #define CS0_LINE                    SPI_PUSHR_PCS0               // CS0 line used when SPI FLASH is enabled
    #else
        #define CS0_LINE                    SPI_PUSHR_PCS5               // CS0 line used when SPI FLASH is enabled
    #endif
    #define CS1_LINE                                                     // CS1 line used when extended SPI FLASH is enabled
    #define CS2_LINE                                                     // CS2 line used when extended SPI FLASH is enabled
    #define CS3_LINE                                                     // CS3 line used when extended SPI FLASH is enabled

    #define SPI_CS0_PORT                    ~(SPI0_PUSHR)                // for simulator
    #define SPI_TX_BYTE                     SPI0_PUSHR                   // for simulator
    #define SPI_RX_BYTE                     SPI0_POPR                    // for simulator

    #define POWER_UP_SPI_FLASH_INTERFACE()  POWER_UP(6, SIM_SCGC6_SPI0)
    #if defined SPI_FLASH_S25FL1_K                                       // use CS0 on PTC4 (no reset signal)
        #define CONFIGURE_SPI_FLASH_INTERFACE() _CONFIG_PERIPHERAL(C, 4, (PC_4_SPI0_PCS0 | PORT_SRE_FAST | PORT_DSE_HIGH));\
                                                _CONFIG_PERIPHERAL(C, 5, (PC_5_SPI0_SCK | PORT_SRE_FAST | PORT_DSE_HIGH));\
                                                _CONFIG_PERIPHERAL(C, 6, (PC_6_SPI0_SOUT | PORT_SRE_FAST | PORT_DSE_HIGH));\
                                                _CONFIG_PERIPHERAL(C, 7, PC_7_SPI0_SIN);\
                                                SPI0_MCR = (SPI_MCR_MSTR | SPI_MCR_DCONF_SPI | SPI_MCR_CLR_RXF | SPI_MCR_CLR_TXF | SPI_MCR_PCSIS_CS0 | SPI_MCR_PCSIS_CS1 | SPI_MCR_PCSIS_CS2 | SPI_MCR_PCSIS_CS3 | SPI_MCR_PCSIS_CS4 | SPI_MCR_PCSIS_CS5);\
                                                SPI0_CTAR0 = (SPI_CTAR_DBR | SPI_CTAR_FMSZ_8 | SPI_CTAR_PDT_7 | SPI_CTAR_BR_2 | SPI_CTAR_CPHA | SPI_CTAR_CPOL); // for 50MHz bus, 25MHz speed and 140ns min de-select time
                                              //SPI0_CTAR0 = (/*SPI_CTAR_DBR | */ SPI_CTAR_BR_2 | SPI_CTAR_FMSZ_8 | SPI_CTAR_PDT_7 | SPI_CTAR_BR_2 | SPI_CTAR_CPHA | SPI_CTAR_CPOL); // for 50MHz bus, 6.25MHz speed and 140ns min de-select time (double speed bit removed and divider added)
    #else                                                                // use CS5 on PTB23 (with reset signal on PTA4)
        #define CONFIGURE_SPI_FLASH_INTERFACE() _CONFIG_PERIPHERAL(B, 23, (PB_23_SPI0_PCS5 | PORT_SRE_FAST | PORT_DSE_HIGH));\
                                                _CONFIG_PERIPHERAL(C, 5, (PC_5_SPI0_SCK | PORT_SRE_FAST | PORT_DSE_HIGH));\
                                                _CONFIG_PERIPHERAL(C, 6, (PC_6_SPI0_SOUT | PORT_SRE_FAST | PORT_DSE_HIGH));\
                                                _CONFIG_PERIPHERAL(C, 7, PC_7_SPI0_SIN);\
											    _CONFIG_DRIVE_PORT_OUTPUT_VALUE(A, (PORTA_BIT4), (PORTA_BIT4), (PORT_SRE_SLOW | PORT_DSE_HIGH));\
											    _SETBITS(A, PORTA_BIT4);\
                                                SPI0_MCR = (SPI_MCR_MSTR | SPI_MCR_DCONF_SPI | SPI_MCR_CLR_RXF | SPI_MCR_CLR_TXF | SPI_MCR_PCSIS_CS0 | SPI_MCR_PCSIS_CS1 | SPI_MCR_PCSIS_CS2 | SPI_MCR_PCSIS_CS3 | SPI_MCR_PCSIS_CS4 | SPI_MCR_PCSIS_CS5);\
                                                SPI0_CTAR0 = (SPI_CTAR_DBR | SPI_CTAR_FMSZ_8 | SPI_CTAR_PDT_7 | SPI_CTAR_BR_2 | SPI_CTAR_CPHA | SPI_CTAR_CPOL); // for 50MHz bus, 25MHz speed and 140ns min de-select time
                                              //SPI0_CTAR0 = (/*SPI_CTAR_DBR | */ SPI_CTAR_BR_2 | SPI_CTAR_FMSZ_8 | SPI_CTAR_PDT_7 | SPI_CTAR_BR_2 | SPI_CTAR_CPHA | SPI_CTAR_CPOL); // for 50MHz bus, 6.25MHz speed and 140ns min de-select time (double speed bit removed and divider added)
    #endif

    #define POWER_DOWN_SPI_FLASH_INTERFACE() POWER_DOWN(6, SIM_SCGC6_SPI0) // power down SPI interface if no SPI Flash detected

    #define FLUSH_SPI_FIFO_AND_FLAGS()      SPI0_MCR |= SPI_MCR_CLR_RXF; SPI0_SR = (SPI_SR_EOQF | SPI_SR_TFUF | SPI_SR_TFFF | SPI_SR_RFOF | SPI_SR_RFDF);

    #define WRITE_SPI_CMD0(byte)            SPI0_PUSHR = (byte | SPI_PUSHR_CONT | ulChipSelectLine | SPI_PUSHR_CTAS_CTAR0) // write a single byte to the output FIFO - assert CS line
    #define WRITE_SPI_CMD0_LAST(byte)       SPI0_PUSHR = (byte | SPI_PUSHR_EOQ  | ulChipSelectLine | SPI_PUSHR_CTAS_CTAR0) // write final byte to output FIFO - this will negate the CS line when complete
    #define READ_SPI_FLASH_DATA()           (unsigned char)SPI0_POPR
    #define WAIT_SPI_RECEPTION_END()        while ((SPI0_SR & SPI_SR_RFDF) == 0) {}
    #define CLEAR_RECEPTION_FLAG()          SPI0_SR |= SPI_SR_RFDF
#elif defined NET_K60 || defined FRDM_K64F || defined FRDM_K22F || defined TWR_K22F120M || defined FreeLON || defined K22F128_100M
    #define CS0_LINE                        SPI_PUSHR_PCS0               // CS0 line used when SPI FLASH is enabled
    #define CS1_LINE                                                     // CS1 line used when extended SPI FLASH is enabled
    #define CS2_LINE                                                     // CS2 line used when extended SPI FLASH is enabled
    #define CS3_LINE                                                     // CS3 line used when extended SPI FLASH is enabled

    #define SPI_CS0_PORT                    ~(SPI0_PUSHR)                // for simulator
    #define SPI_TX_BYTE                     SPI0_PUSHR                   // for simulator
    #define SPI_RX_BYTE                     SPI0_POPR                    // for simulator

    #define POWER_UP_SPI_FLASH_INTERFACE()  POWER_UP(6, SIM_SCGC6_SPI0)
    #if defined NET_K60
        #define CONFIGURE_SPI_FLASH_INTERFACE() _CONFIG_PERIPHERAL(C, 4, (PC_4_SPI0_PCS0 | PORT_SRE_FAST | PORT_DSE_HIGH));\
                                                _CONFIG_PERIPHERAL(C, 5, (PC_5_SPI0_SCK | PORT_SRE_FAST | PORT_DSE_HIGH));\
                                                _CONFIG_PERIPHERAL(C, 6, (PC_6_SPI0_SOUT | PORT_SRE_FAST | PORT_DSE_HIGH));\
                                                _CONFIG_PERIPHERAL(C, 7, PC_7_SPI0_SIN);\
											    _CONFIG_DRIVE_PORT_OUTPUT_VALUE(C, (PORTC_BIT13), (PORTC_BIT13), (PORT_SRE_SLOW | PORT_DSE_HIGH));\
											    _SETBITS(C, PORTC_BIT13);\
                                                SPI0_MCR = (SPI_MCR_MSTR | SPI_MCR_DCONF_SPI | SPI_MCR_CLR_RXF | SPI_MCR_CLR_TXF | SPI_MCR_PCSIS_CS0 | SPI_MCR_PCSIS_CS1 | SPI_MCR_PCSIS_CS2 | SPI_MCR_PCSIS_CS3 | SPI_MCR_PCSIS_CS4 | SPI_MCR_PCSIS_CS5);\
                                                SPI0_CTAR0 = (SPI_CTAR_DBR | SPI_CTAR_FMSZ_8 | SPI_CTAR_PDT_7 | SPI_CTAR_BR_2 | SPI_CTAR_CPHA | SPI_CTAR_CPOL); // for 50MHz bus, 25MHz speed and 140ns min de-select time
    #else
        #define CONFIGURE_SPI_FLASH_INTERFACE() _CONFIG_PERIPHERAL(D, 0, (PD_0_SPI0_PCS0 | PORT_SRE_FAST | PORT_DSE_HIGH));\
                                                _CONFIG_PERIPHERAL(D, 1, (PD_1_SPI0_SCK | PORT_SRE_FAST | PORT_DSE_HIGH));\
                                                _CONFIG_PERIPHERAL(D, 2, (PD_2_SPI0_SOUT | PORT_SRE_FAST | PORT_DSE_HIGH));\
                                                _CONFIG_PERIPHERAL(D, 3, (PD_3_SPI0_SIN));\
                                                SPI0_MCR = (SPI_MCR_MSTR | SPI_MCR_DCONF_SPI | SPI_MCR_CLR_RXF | SPI_MCR_CLR_TXF | SPI_MCR_PCSIS_CS0 | SPI_MCR_PCSIS_CS1 | SPI_MCR_PCSIS_CS2 | SPI_MCR_PCSIS_CS3 | SPI_MCR_PCSIS_CS4 | SPI_MCR_PCSIS_CS5);\
                                                SPI0_CTAR0 = (SPI_CTAR_DBR | SPI_CTAR_FMSZ_8 | SPI_CTAR_PDT_7 | SPI_CTAR_BR_2 | SPI_CTAR_CPHA | SPI_CTAR_CPOL); // for 50MHz bus, 25MHz speed and 140ns min de-select time
    #endif

    #define POWER_DOWN_SPI_FLASH_INTERFACE() POWER_DOWN(6, SIM_SCGC6_SPI0) // power down SPI interface if no SPI Flash detected

    #define FLUSH_SPI_FIFO_AND_FLAGS()      SPI0_MCR |= SPI_MCR_CLR_RXF; SPI0_SR = (SPI_SR_EOQF | SPI_SR_TFUF | SPI_SR_TFFF | SPI_SR_RFOF | SPI_SR_RFDF);

    #define WRITE_SPI_CMD0(byte)            SPI0_PUSHR = (byte | SPI_PUSHR_CONT | ulChipSelectLine | SPI_PUSHR_CTAS_CTAR0) // write a single byte to the output FIFO - assert CS line
    #define WRITE_SPI_CMD0_LAST(byte)       SPI0_PUSHR = (byte | SPI_PUSHR_EOQ  | ulChipSelectLine | SPI_PUSHR_CTAS_CTAR0) // write final byte to output FIFO - this will negate the CS line when complete
    #define READ_SPI_FLASH_DATA()           (unsigned char)SPI0_POPR
    #define WAIT_SPI_RECEPTION_END()        while ((SPI0_SR & SPI_SR_RFDF) == 0) {}
    #define CLEAR_RECEPTION_FLAG()          SPI0_SR |= SPI_SR_RFDF
    #define SET_SPI_FLASH_MODE()                                         // this can be used to change SPI settings on-the-fly when the SPI is shared with SPI Flash and other devices
    #define REMOVE_SPI_FLASH_MODE()                                      // this can be used to change SPI settings on-the-fly when the SPI is shared with SPI Flash and other devices
#elif defined TWR_K24F120M
    #define CS0_LINE                        SPI_PUSHR_PCS0               // CS0 line used when SPI FLASH is enabled
    #define CS1_LINE                                                     // CS1 line used when extended SPI FLASH is enabled
    #define CS2_LINE                                                     // CS2 line used when extended SPI FLASH is enabled
    #define CS3_LINE                                                     // CS3 line used when extended SPI FLASH is enabled

    #define SPI_CS0_PORT                    ~(SPI1_PUSHR)                // for simulator
    #define SPI_TX_BYTE                     SPI1_PUSHR                   // for simulator
    #define SPI_RX_BYTE                     SPI1_POPR                    // for simulator

    #define POWER_UP_SPI_FLASH_INTERFACE()  POWER_UP(6, SIM_SCGC6_SPI1)
    #define CONFIGURE_SPI_FLASH_INTERFACE() _CONFIG_PERIPHERAL(B, 10, (PB_10_SPI1_PCS0 | PORT_SRE_FAST | PORT_DSE_HIGH)); \
                                            _CONFIG_PERIPHERAL(B, 11, (PB_11_SPI1_SCK | PORT_SRE_FAST | PORT_DSE_HIGH)); \
                                            _CONFIG_PERIPHERAL(B, 16, (PB_16_SPI1_SOUT | PORT_SRE_FAST | PORT_DSE_HIGH)); \
                                            _CONFIG_PERIPHERAL(B, 17, (PB_17_SPI1_SIN)); \
                                            SPI1_MCR = (SPI_MCR_MSTR | SPI_MCR_DCONF_SPI | SPI_MCR_CLR_RXF | SPI_MCR_CLR_TXF | SPI_MCR_PCSIS_CS0 | SPI_MCR_PCSIS_CS1 | SPI_MCR_PCSIS_CS2 | SPI_MCR_PCSIS_CS3 | SPI_MCR_PCSIS_CS4 | SPI_MCR_PCSIS_CS5); \
                                            SPI1_CTAR0 = (SPI_CTAR_DBR | SPI_CTAR_FMSZ_8 | SPI_CTAR_PDT_7 | SPI_CTAR_BR_2 | SPI_CTAR_CPHA | SPI_CTAR_CPOL); // for 50MHz bus, 25MHz speed and 140ns min de-select time

    #define POWER_DOWN_SPI_FLASH_INTERFACE() POWER_DOWN(6, SIM_SCGC6_SPI1) // power down SPI interface if no SPI Flash detected

    #define FLUSH_SPI_FIFO_AND_FLAGS()      SPI1_MCR |= SPI_MCR_CLR_RXF; SPI1_SR = (SPI_SR_EOQF | SPI_SR_TFUF | SPI_SR_TFFF | SPI_SR_RFOF | SPI_SR_RFDF);

    #define WRITE_SPI_CMD0(byte)            SPI1_PUSHR = (byte | SPI_PUSHR_CONT | ulChipSelectLine | SPI_PUSHR_CTAS_CTAR0) // write a single byte to the output FIFO - assert CS line
    #define WRITE_SPI_CMD0_LAST(byte)       SPI1_PUSHR = (byte | SPI_PUSHR_EOQ  | ulChipSelectLine | SPI_PUSHR_CTAS_CTAR0) // write final byte to output FIFO - this will negate the CS line when complete
    #define READ_SPI_FLASH_DATA()           (unsigned char)(SPI1_POPR)
    #define WAIT_SPI_RECEPTION_END()        while ((SPI1_SR & SPI_SR_RFDF) == 0) {}
    #define CLEAR_RECEPTION_FLAG()          SPI1_SR |= SPI_SR_RFDF
    #define SET_SPI_FLASH_MODE()                                         // this can be used to change SPI settings on-the-fly when the SPI is shared with SPI Flash and other devices
    #define REMOVE_SPI_FLASH_MODE()                                      // this can be used to change SPI settings on-the-fly when the SPI is shared with SPI Flash and other devices
#elif defined FRDM_KL46Z
    // Configure to suit special connection SPI mode (SPI0) - this board has the option to solder in an AT45DB161
    // - SPI0_CS   PTC-2
    // - SPI0_SCK  PTC-5
    // - SPI0_MOSI PTC-0
    // - SPI0_MISO PTC-7
    // - SPI Flash reset PTC-1
    //
    #define CS0_LINE                        PORTC_BIT2                   // CS0 line used when SPI FLASH is enabled
    #define CS1_LINE                                                     // CS1 line used when extended SPI FLASH is enabled
    #define CS2_LINE                                                     // CS2 line used when extended SPI FLASH is enabled
    #define CS3_LINE                                                     // CS3 line used when extended SPI FLASH is enabled

    #define ASSERT_CS_LINE(ulChipSelectLine) _CLEARBITS(C, ulChipSelectLine)
    #define NEGATE_CS_LINE(ulChipSelectLine) _SETBITS(C, ulChipSelectLine)

    #define SPI_CS0_PORT                    GPIOC_PDOR                   // for simulator
    #define SPI_TX_BYTE                     SPI0_D                       // for simulator
    #define SPI_RX_BYTE                     SPI0_D                       // for simulator

    #define POWER_UP_SPI_FLASH_INTERFACE()  POWER_UP(4, SIM_SCGC4_SPI0); _CONFIG_DRIVE_PORT_OUTPUT_VALUE(C, PORTC_BIT1, PORTC_BIT1, (PORT_SRE_FAST | PORT_DSE_HIGH))
    #define CONFIGURE_SPI_FLASH_INTERFACE() _CONFIG_PERIPHERAL(C, 5, PC_5_SPI0_SCK); \
                                            _CONFIG_PERIPHERAL(C, 6, (PC_6_SPI0_MOSI | PORT_SRE_FAST | PORT_DSE_HIGH)); \
                                            _CONFIG_PERIPHERAL(C, 7, (PC_7_SPI0_MISO | PORT_PS_UP_ENABLE)); \
                                            _CONFIG_DRIVE_PORT_OUTPUT_VALUE(C, CS0_LINE, CS0_LINE, (PORT_SRE_FAST | PORT_DSE_HIGH)); \
                                            SPI0_C1 = (SPI_C1_CPHA | SPI_C1_CPOL | SPI_C1_MSTR | SPI_C1_SPE); \
                                            SPI0_BR = (SPI_BR_SPPR_PRE_1 | SPI_BR_SPR_DIV_2); \
                                            (void)SPI0_S; (void)SPI0_D

    #define POWER_DOWN_SPI_FLASH_INTERFACE() POWER_DOWN(4, SIM_SCGC4_SPI0) // power down SPI interface if no SPI Flash detected

    #define FLUSH_SPI_FIFO_AND_FLAGS()      

    #define WRITE_SPI_CMD0(byte)            SPI0_D = (byte)              // write a single byte
    #define WRITE_SPI_CMD0_LAST(byte)       SPI0_D = (byte)              // write final byte
    #define READ_SPI_FLASH_DATA()           (unsigned char)SPI0_D
    #if defined _WINDOWS
        #define WAIT_SPI_RECEPTION_END()    while ((SPI0_S & (SPI_S_SPRF)) == 0) {SPI0_S |= SPI_S_SPRF;}
    #else
        #define WAIT_SPI_RECEPTION_END()    while ((SPI0_S & (SPI_S_SPRF)) == 0) {}
    #endif
    #define CLEAR_RECEPTION_FLAG()          
            
    #define SET_SPI_FLASH_MODE()                                         // this can be used to change SPI settings on-the-fly when the SPI is shared with SPI Flash and other devices
    #define REMOVE_SPI_FLASH_MODE()                                      // this can be used to change SPI settings on-the-fly when the SPI is shared with SPI Flash and other devices
#elif defined KL43Z_256_32_CL
    // Configure to suit special connection SPI mode (SPI0)
    // - SPI0_CS   PTD-0
    // - SPI0_SCK  PTD-1
    // - SPI0_MOSI PTD-2
    // - SPI0_MISO PTD-3
    #define CS0_LINE                        PORTD_BIT0                   // CS0 line used when SPI FLASH is enabled
    #define CS1_LINE                                                     // CS1 line used when extended SPI FLASH is enabled
    #define CS2_LINE                                                     // CS2 line used when extended SPI FLASH is enabled
    #define CS3_LINE                                                     // CS3 line used when extended SPI FLASH is enabled

    #define ASSERT_CS_LINE(ulChipSelectLine) _CLEARBITS(D, ulChipSelectLine)
    #define NEGATE_CS_LINE(ulChipSelectLine) _SETBITS(D, ulChipSelectLine)

    #define SPI_CS0_PORT                    GPIOD_PDOR                   // for simulator
    #define SPI_TX_BYTE                     SPI0_D                       // for simulator
    #define SPI_RX_BYTE                     SPI0_D                       // for simulator

    #define POWER_UP_SPI_FLASH_INTERFACE()  POWER_UP(4, SIM_SCGC4_SPI0)
    #define CONFIGURE_SPI_FLASH_INTERFACE() _CONFIG_PERIPHERAL(D, 1, PD_1_SPI0_SCK); \
                                            _CONFIG_PERIPHERAL(D, 2, (PD_2_SPI0_MOSI | PORT_SRE_FAST | PORT_DSE_HIGH)); \
                                            _CONFIG_PERIPHERAL(D, 3, (PD_3_SPI0_MISO | PORT_PS_UP_ENABLE)); \
                                            _CONFIG_DRIVE_PORT_OUTPUT_VALUE(D, CS0_LINE, CS0_LINE, (PORT_SRE_FAST | PORT_DSE_HIGH)); \
                                            SPI0_C1 = (SPI_C1_CPHA | SPI_C1_CPOL | SPI_C1_MSTR | SPI_C1_SPE); \
                                            SPI0_BR = (SPI_BR_SPPR_PRE_1 | SPI_BR_SPR_DIV_2); \
                                            (void)SPI0_S; (void)SPI0_D

    #define POWER_DOWN_SPI_FLASH_INTERFACE() POWER_DOWN(4, SIM_SCGC4_SPI0) // power down SPI interface if no SPI Flash detected

    #define FLUSH_SPI_FIFO_AND_FLAGS()      

    #define WRITE_SPI_CMD0(byte)            SPI0_D = (byte)              // write a single byte
    #define WRITE_SPI_CMD0_LAST(byte)       SPI0_D = (byte)              // write final byte
    #define READ_SPI_FLASH_DATA()           (unsigned char)SPI0_D
    #if defined _WINDOWS
        #define WAIT_SPI_RECEPTION_END()    while ((SPI0_S & (SPI_S_SPRF)) == 0) {SPI0_S |= SPI_S_SPRF;}
    #else
        #define WAIT_SPI_RECEPTION_END()    while ((SPI0_S & (SPI_S_SPRF)) == 0) {}
    #endif
    #define CLEAR_RECEPTION_FLAG()          
            
    #define SET_SPI_FLASH_MODE()                                         // this can be used to change SPI settings on-the-fly when the SPI is shared with SPI Flash and other devices
    #define REMOVE_SPI_FLASH_MODE()                                      // this can be used to change SPI settings on-the-fly when the SPI is shared with SPI Flash and other devices
#elif defined FRDM_KL26Z || defined FRDM_KL27Z || defined TEENSY_LC || defined TWR_KL46Z48M || defined FRDM_KL46Z || defined FRDM_KL43Z || defined rcARM_KL26 || defined KL43Z_256_32_CL
    // Configure to suit special connection SPI mode at between 100k and 400k (SPI1)
    // - SPI1_CS   PTD-4 (J2-6) [VDD J3-4 / 0V J3-14]
    // - SPI1_SCK  PTD-5 (J2-12)
    // - SPI1_MOSI PTD-6 (J2-8)
    // - SPI1_MISO PTD-7 (J2-10)
    //
    #define CS0_LINE                        PORTE_BIT4                   // CS0 line used when SPI FLASH is enabled
    #define CS1_LINE                                                     // CS1 line used when extended SPI FLASH is enabled
    #define CS2_LINE                                                     // CS2 line used when extended SPI FLASH is enabled
    #define CS3_LINE                                                     // CS3 line used when extended SPI FLASH is enabled

    #define ASSERT_CS_LINE(ulChipSelectLine) _CLEARBITS(E, ulChipSelectLine)
    #define NEGATE_CS_LINE(ulChipSelectLine) _SETBITS(E, ulChipSelectLine)

    #define SPI_CS0_PORT                    GPIOE_PDOR                   // for simulator
    #define SPI_TX_BYTE                     SPI1_D                       // for simulator
    #define SPI_RX_BYTE                     SPI1_D                       // for simulator

    #define POWER_UP_SPI_FLASH_INTERFACE()  POWER_UP(4, SIM_SCGC4_SPI1)
    #define CONFIGURE_SPI_FLASH_INTERFACE() _CONFIG_PERIPHERAL(E, 2, PE_2_SPI1_SCK); \
                                            _CONFIG_PERIPHERAL(E, 1, (PE_1_SPI1_MOSI | PORT_SRE_FAST | PORT_DSE_HIGH)); \
                                            _CONFIG_PERIPHERAL(E, 3, (PE_3_SPI1_MISO | PORT_PS_UP_ENABLE)); \
                                            _CONFIG_DRIVE_PORT_OUTPUT_VALUE(E, CS0_LINE, CS0_LINE, (PORT_SRE_FAST | PORT_DSE_HIGH)); \
                                            SPI1_C1 = (SPI_C1_CPHA | SPI_C1_CPOL | SPI_C1_MSTR | SPI_C1_SPE); \
                                            SPI1_BR = (SPI_BR_SPPR_PRE_1 | SPI_BR_SPR_DIV_2); \
                                            (void)SPI1_S; (void)SPI1_D

    #define POWER_DOWN_SPI_FLASH_INTERFACE() POWER_DOWN(4, SIM_SCGC4_SPI1) // power down SPI interface if no SPI Flash detected

    #define FLUSH_SPI_FIFO_AND_FLAGS()      

    #define WRITE_SPI_CMD0(byte)            SPI1_D = (byte)              // write a single byte
    #define WRITE_SPI_CMD0_LAST(byte)       SPI1_D = (byte)              // write final byte
    #define READ_SPI_FLASH_DATA()           (unsigned char)SPI1_D
    #if defined _WINDOWS
        #define WAIT_SPI_RECEPTION_END()    while ((SPI1_S & (SPI_S_SPRF)) == 0) {SPI1_S |= SPI_S_SPRF;}
    #else
        #define WAIT_SPI_RECEPTION_END()    while ((SPI1_S & (SPI_S_SPRF)) == 0) {}
    #endif
    #define CLEAR_RECEPTION_FLAG()          
            
    #define SET_SPI_FLASH_MODE()                                         // this can be used to change SPI settings on-the-fly when the SPI is shared with SPI Flash and other devices
    #define REMOVE_SPI_FLASH_MODE()                                      // this can be used to change SPI settings on-the-fly when the SPI is shared with SPI Flash and other devices
#elif defined FRDM_KL25Z
    // - SPI1_CS   PTE-4 (J9-13) [VDD J9-4 / 0V J9-14] card detect input on PTB-8 (J9-1)
    // - SPI1_SCK  PTE-2 (J9-9)
    // - SPI1_MOSI PTE-1 (J2-20)
    // - SPI1_MISO PTE-3 (J9-11)
    //
    #define CS0_LINE                        PORTE_BIT4                   // CS0 line used when SPI FLASH is enabled
    #define CS1_LINE                                                     // CS1 line used when extended SPI FLASH is enabled
    #define CS2_LINE                                                     // CS2 line used when extended SPI FLASH is enabled
    #define CS3_LINE                                                     // CS3 line used when extended SPI FLASH is enabled

    #define ASSERT_CS_LINE(ulChipSelectLine) _CLEARBITS(E, ulChipSelectLine)
    #define NEGATE_CS_LINE(ulChipSelectLine) _SETBITS(E, ulChipSelectLine)

    #define SPI_CS0_PORT                    GPIOE_PDOR                   // for simulator
    #define SPI_TX_BYTE                     SPI1_D                       // for simulator
    #define SPI_RX_BYTE                     SPI1_D                       // for simulator

    #define POWER_UP_SPI_FLASH_INTERFACE()  POWER_UP(4, SIM_SCGC4_SPI1)
    #define CONFIGURE_SPI_FLASH_INTERFACE() _CONFIG_PERIPHERAL(E, 2, PE_2_SPI1_SCK); \
                                            _CONFIG_PERIPHERAL(E, 1, (PE_1_SPI1_MOSI | PORT_SRE_FAST | PORT_DSE_HIGH)); \
                                            _CONFIG_PERIPHERAL(E, 3, (PE_3_SPI1_MISO | PORT_PS_UP_ENABLE)); \
                                            _CONFIG_DRIVE_PORT_OUTPUT_VALUE(E, CS0_LINE, CS0_LINE, (PORT_SRE_FAST | PORT_DSE_HIGH)); \
                                            SPI1_C1 = (SPI_C1_CPHA | SPI_C1_CPOL | SPI_C1_MSTR | SPI_C1_SPE); \
                                            SPI1_BR = (SPI_BR_SPPR_PRE_1 | SPI_BR_SPR_DIV_2); \
                                            (unsigned char)SPI1_S; (unsigned char)SPI1_D

    #define POWER_DOWN_SPI_FLASH_INTERFACE() POWER_DOWN(4, SIM_SCGC4_SPI1) // power down SPI interface if no SPI Flash detected

    #define FLUSH_SPI_FIFO_AND_FLAGS()      

    #define WRITE_SPI_CMD0(byte)            SPI1_D = (byte)              // write a single byte
    #define WRITE_SPI_CMD0_LAST(byte)       SPI1_D = (byte)              // write final byte
    #define READ_SPI_FLASH_DATA()           (unsigned char)SPI1_D
    #if defined _WINDOWS
        #define WAIT_SPI_RECEPTION_END()    while ((SPI1_S & (SPI_S_SPRF)) == 0) {SPI1_S |= SPI_S_SPRF;}
    #else
        #define WAIT_SPI_RECEPTION_END()    while ((SPI1_S & (SPI_S_SPRF)) == 0) {}
    #endif
    #define CLEAR_RECEPTION_FLAG()          
            
    #define SET_SPI_FLASH_MODE()                                         // this can be used to change SPI settings on-the-fly when the SPI is shared with SPI Flash and other devices
    #define REMOVE_SPI_FLASH_MODE()                                      // this can be used to change SPI settings on-the-fly when the SPI is shared with SPI Flash and other devices
#elif defined KINETIS_K80 && defined QSPI_FILE_SYSTEM                    // interface using QSPI
    #define POWER_UP_SPI_FLASH_INTERFACE()     POWER_UP(2, SIM_SCGC2_QSPI);
    #define POWER_DOWN_SPI_FLASH_INTERFACE()   POWER_DOWN(2, SIM_SCGC2_QSPI);
    #define CONFIGURE_SPI_FLASH_INTERFACE() \
        _CONFIG_PERIPHERAL(E, 0, (PE_0_QSPI0A_DATA3)); \
        _CONFIG_PERIPHERAL(E, 1, (PE_1_QSPI0A_SCLK)); \
        _CONFIG_PERIPHERAL(E, 2, (PE_2_QSPI0A_DATA0)); \
        _CONFIG_PERIPHERAL(E, 3, (PE_3_QSPI0A_DATA2)); \
        _CONFIG_PERIPHERAL(E, 4, (PE_4_QSPI0A_DATA1)); \
        _CONFIG_PERIPHERAL(E, 5, (PE_5_QSPI0A_SS0_B)); \
        _CONFIG_PERIPHERAL(E, 6, (PE_6_QSPI0B_DATA3)); \
        _CONFIG_PERIPHERAL(E, 7, (PE_7_QSPI0B_SCLK)); \
        _CONFIG_PERIPHERAL(E, 8, (PE_8_QSPI0B_DATA0)); \
        _CONFIG_PERIPHERAL(E, 9, (PE_9_QSPI0B_DATA2)); \
        _CONFIG_PERIPHERAL(E, 10, (PE_10_QSPI0B_DATA1)); \
        _CONFIG_PERIPHERAL(E, 11, (PE_11_QSPI0B_SS0_B))
      //QuadSPI0_SOCCR = ()

#if 0
    /* Clock source from PLL */
    config->clockSrc =  kClockQspiSrcPllClk;
    config->sclk = 60000000U;
    config->AHBbufferMaster[0] = 0xE;
    config->AHBbufferMaster[1] = 0xE;
    config->AHBbufferMaster[2] = 0xE;
    config->AHBbuffer3AllMaster = true;
    config->txWatermark = 8;
    config->rxWatermark = 8;
    config.txSource = kDmaRequestMux0Group1QSPI0Tx;
    config.rxSource = kDmaRequestMux0Group1QSPI0Rx;
    config.AHBbufferSize[3] = FLASH_PAGE_SIZE;
    QSPI_DRV_ConfigQspi(0, &config);


void QSPI_DRV_ExecuteFlashCommand(uint32_t instance, uint32_t index)
{
    while((QSPI_HAL_GetState(QuadSPI0, kQspiBusy)) && (QSPI_HAL_GetState(QuadSPI0, kQspiIPAccess))){}
    QSPI_HAL_ClearSeqId(QuadSPI0, kQspiIPSeq);
    QSPI_HAL_SetIPCommandSeqIndex(g_qspiBase[instance], index/4);
}


static inline uint32_t QSPI_HAL_GetState(QuadSPI_Type * base, uint32_t state)
{
    return (QuadSPI_RD_SR(base) & state);
}

static inline void QSPI_HAL_ClearSeqId(QuadSPI_Type * base, qspi_command_seq_t seq)
{
    QuadSPI_WR_SPTRCLR(base, seq); /* Clear both IP and buffer pointer */
}




#define FLASH_PAGE_SIZE  (256)
#define FLASH_SECTORE_SIZE (4096)
#define FLASH_SIZE (0x00400000U)
#define DUMMY_MASTER (0xE)

#endif


    #define FLUSH_SPI_FIFO_AND_FLAGS()
    #define SET_SPI_FLASH_MODE()
    #define REMOVE_SPI_FLASH_MODE()
    #define WRITE_SPI_CMD0(byte)                                         // write a single byte to the output FIFO - no automatic chip select control
    #define WRITE_SPI_CMD0_LAST(byte)
    #define SPI_TX_BYTE             dummy
    #define SPI_RX_BYTE             dummy
    #define WAIT_SPI_RECEPTION_END()
    #define READ_SPI_FLASH_DATA()   0
    #define CLEAR_RECEPTION_FLAG()
    #define CS0_LINE                0
    #if defined MANUAL_FLASH_CS_CONTROL
        #define SPI_CS0_PORT                GPIOE_PDOR
    #else
        #define SPI_CS0_PORT                ~(SPI2_PUSHR)                // for simulator
    #endif
#else                                                                    // SPI flash configuration not assigned to a specific board
    #define MANUAL_FLASH_CS_CONTROL
    #define CS0_LINE                        PORTE_BIT4                   // CS0 line used when SPI FLASH is enabled
    #define ASSERT_CS_LINE(ulChipSelectLine) _CLEARBITS(E, ulChipSelectLine)
    #define NEGATE_CS_LINE(ulChipSelectLine) _SETBITS(E, ulChipSelectLine)


  //#define CS0_LINE                        SPI_PUSHR_PCS0               // CS0 line used when SPI FLASH is enabled
    #define CS1_LINE                                                     // CS1 line used when extended SPI FLASH is enabled
    #define CS2_LINE                                                     // CS2 line used when extended SPI FLASH is enabled
    #define CS3_LINE                                                     // CS3 line used when extended SPI FLASH is enabled

    #if defined MANUAL_FLASH_CS_CONTROL
        #define SPI_CS0_PORT                GPIOE_PDOR
    #else
        #define SPI_CS0_PORT                ~(SPI2_PUSHR)                // for simulator
    #endif
    #define SPI_TX_BYTE                     SPI2_PUSHR                   // for simulator
    #define SPI_RX_BYTE                     SPI2_POPR                    // for simulator

    #define POWER_UP_SPI_FLASH_INTERFACE()  POWER_UP(3, SIM_SCGC3_SPI2)
    #define CONFIGURE_SPI_FLASH_INTERFACE() _CONFIG_PERIPHERAL(D, 11, (PD_11_SPI2_PCS0 | PORT_SRE_FAST | PORT_DSE_HIGH));\
                                            _CONFIG_PERIPHERAL(D, 12, (PD_12_SPI2_SCK | PORT_SRE_FAST | PORT_DSE_HIGH));\
                                            _CONFIG_PERIPHERAL(D, 13, (PD_13_SPI2_SOUT | PORT_SRE_FAST | PORT_DSE_HIGH));\
                                            _CONFIG_PERIPHERAL(D, 14, PD_14_SPI2_SIN);\
                                            SPI2_MCR = (SPI_MCR_MSTR | SPI_MCR_DCONF_SPI | SPI_MCR_CLR_RXF | SPI_MCR_CLR_TXF | SPI_MCR_PCSIS_CS0 | SPI_MCR_PCSIS_CS1 | SPI_MCR_PCSIS_CS2 | SPI_MCR_PCSIS_CS3 | SPI_MCR_PCSIS_CS4 | SPI_MCR_PCSIS_CS5);\
                                            SPI2_CTAR0 = (SPI_CTAR_DBR | SPI_CTAR_FMSZ_8 | SPI_CTAR_PDT_7 | SPI_CTAR_BR_2 | SPI_CTAR_CPHA | SPI_CTAR_CPOL); // for 50MHz bus, 25MHz speed and 140ns min de-select time
                                          //SPI2_CTAR0 = (/*SPI_CTAR_DBR | */ SPI_CTAR_BR_2 | SPI_CTAR_FMSZ_8 | SPI_CTAR_PDT_7 | SPI_CTAR_BR_2 | SPI_CTAR_CPHA | SPI_CTAR_CPOL); // for 50MHz bus, 6.25MHz speed and 140ns min de-select time (double speed bit removed and divider added)

    #define POWER_DOWN_SPI_FLASH_INTERFACE() POWER_DOWN(3, SIM_SCGC3_SPI2) // power down SPI interface if no SPI Flash detected

    #define FLUSH_SPI_FIFO_AND_FLAGS()      SPI2_MCR |= SPI_MCR_CLR_RXF; SPI2_SR = (SPI_SR_EOQF | SPI_SR_TFUF | SPI_SR_TFFF | SPI_SR_RFOF | SPI_SR_RFDF);

    #if defined MANUAL_FLASH_CS_CONTROL
        #define WRITE_SPI_CMD0(byte)        SPI2_PUSHR = (byte | SPI_PUSHR_CONT | 0 | SPI_PUSHR_CTAS_CTAR0) // write a single byte to the output FIFO - no automatic chip select control
        #define WRITE_SPI_CMD0_LAST(byte)   SPI2_PUSHR = (byte | SPI_PUSHR_EOQ  | 0 | SPI_PUSHR_CTAS_CTAR0) // write final byte to output FIFO - no automatic chip select control
    #else
        #define WRITE_SPI_CMD0(byte)        SPI2_PUSHR = (byte | SPI_PUSHR_CONT | ulChipSelectLine | SPI_PUSHR_CTAS_CTAR0) // write a single byte to the output FIFO - assert CS line
        #define WRITE_SPI_CMD0_LAST(byte)   SPI2_PUSHR = (byte | SPI_PUSHR_EOQ  | ulChipSelectLine | SPI_PUSHR_CTAS_CTAR0) // write final byte to output FIFO - this will negate the CS line when complete
    #endif
    #define READ_SPI_FLASH_DATA()           (unsigned char)SPI2_POPR
    #define WAIT_SPI_RECEPTION_END()        while ((SPI2_SR & SPI_SR_RFDF) == 0) {}
    #define CLEAR_RECEPTION_FLAG()          SPI2_SR |= SPI_SR_RFDF
            
    #define SET_SPI_FLASH_MODE()                                         // this can be used to change SPI settings on-the-fly when the SPI is shared with SPI Flash and other devices
    #define REMOVE_SPI_FLASH_MODE()                                      // this can be used to change SPI settings on-the-fly when the SPI is shared with SPI Flash and other devices
#endif

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
        #define SPI_FLASH_PAGES             (32 * 256)
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
#elif defined SPI_FLASH_S25FL1_K
    #if defined KL43Z_256_32_CL
        #define SPI_FLASH_S25FL132K                                      // specific type used
        #define SPI_FLASH_SIZE           (4 * 1024 * 1024)               // 32 Mbits/4 MBytes
    #else
        #define SPI_FLASH_S25FL164K                                      // specific type used
        #define SPI_FLASH_SIZE           (8 * 1024 * 1024)               // 64 Mbits/8 MBytes
    #endif
    #define SPI_FLASH_PAGE_LENGTH        (256)
    #define SPI_FLASH_PAGES              (SPI_FLASH_SIZE/SPI_FLASH_PAGE_LENGTH)
    #define SPI_FLASH_SECTOR_LENGTH      (4 * 1024)                      // sector size of SPI FLASH
    #define SPI_FLASH_SECTORS            (SPI_FLASH_SIZE/SPI_FLASH_SECTOR_LENGTH)
    #define SPI_FLASH_BLOCK_LENGTH       SPI_FLASH_SECTOR_LENGTH         // for compatibility - file system granularity
  //#define SUPPORT_ERASE_SUSPEND                                        // automatically suspend an erase that is in progress when a write or a read is performed in a different sector (advised when FAT used in SPI Flash with block mnagement/wear-levelling)
#elif defined SPI_FLASH_W25Q
    #define SPI_FLASH_W25Q128
  //#define SPI_FLASH_W25Q16
    #if defined SPI_FLASH_W25Q128
        #define SPI_FLASH_PAGES          (65536)
    #else
        #define SPI_FLASH_PAGES          (8192)
    #endif
    #define SPI_FLASH_PAGE_LENGTH        (256)
    #define SPI_FLASH_SUB_SECTOR_LENGTH  (4 * 1024)                      // sub-sector size of SPI FLASH
    #define SPI_FLASH_HALF_SECTOR_LENGTH (32 * 1024)                     // half-sector size of SPI FLASH
    #define SPI_FLASH_SECTOR_LENGTH      (64 * 1024)                     // sector size of SPI FLASH (not available on A-versions)
    #define SPI_FLASH_BLOCK_LENGTH       SPI_FLASH_HALF_SECTOR_LENGTH    // for compatibility - file system granularity
#elif defined SPI_FLASH_SST25
  //#define SPI_FLASH_SST25VF010A                                        // the supported SST chips
  //#define SPI_FLASH_SST25LF020A
  //#define SPI_FLASH_SST25LF040A
    #define SPI_FLASH_SST25VF040B
  //#define SPI_FLASH_SST25VF080B
  //#define SPI_FLASH_SST25VF016B
  //#define SPI_FLASH_SST25VF032B

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
#else                                                                    // AT45DBxxx 
  //#define SPI_FLASH_AT45DB011                                          // define the ATMEL type used here
  //#define SPI_FLASH_AT45DB021
  //#define SPI_FLASH_AT45DB041
  //#define SPI_FLASH_AT45DB081
    #define SPI_FLASH_AT45DB161
  //#define SPI_FLASH_AT45DB321
  //#define SPI_FLASH_AT45DB642
    #define SPI_FLASH_AT45XXXXD_TYPE                                     // specify that a D-type rather than a B/C type is being used

    #if defined SPI_FLASH_AT45DB642                                      // define whether used in power of 2 mode or not
        #define SPI_FLASH_PAGE_LENGTH 1056                               // standard page size (B-device only allows 1056)
      //#define SPI_FLASH_PAGE_LENGTH 1024                               // size when power of 2 mode selected (only possible on D-device) WARNING: this mode will be automatically programmed to the SPI Flash and cannot be reversed!!
        #define SPI_FLASH_PAGES             (32 * 1024)                  // 8Meg part
    #elif defined SPI_FLASH_AT45DB321 || defined SPI_FLASH_AT45DB161
          //#define SPI_FLASH_PAGE_LENGTH 512                            // size when power of 2 mode selected (only possible on D-device) WARNING: this mode will be automatically programmed to the SPI Flash and cannot be reversed!!
            #define SPI_FLASH_PAGE_LENGTH 528                            // standard page size (B/C-device only allows 528)
        #if defined SPI_FLASH_AT45DB161
            #define SPI_FLASH_PAGES         (4 * 1024)                   // 2Meg part
        #else
            #define SPI_FLASH_PAGES         (8 * 1024)                   // 4Meg part
        #endif
    #else
        #define SPI_FLASH_PAGE_LENGTH 264                                // standard page size (B-device only allows 264)
      //#define SPI_FLASH_PAGE_LENGTH 256                                // size when power of 2 mode selected (only possible on D-device) WARNING: this mode will be automatically programmed to the SPI Flash and cannot be reversed!!
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

    #define SPI_FLASH_BLOCK_LENGTH  (8 * SPI_FLASH_PAGE_LENGTH)          // block size - a block can be deleted
    #define SPI_FLASH_SECTOR_LENGTH (64 * 4 * SPI_FLASH_PAGE_LENGTH)     // exception sector 0a is 2k and sector 0b is 62k
#endif

#define SPI_DATA_FLASH_0_SIZE   (SPI_FLASH_PAGES * SPI_FLASH_PAGE_LENGTH) 
#define SPI_DATA_FLASH_1_SIZE   SPI_DATA_FLASH_0_SIZE 
#define SPI_DATA_FLASH_2_SIZE   SPI_DATA_FLASH_0_SIZE
#define SPI_DATA_FLASH_3_SIZE   SPI_DATA_FLASH_0_SIZE
#if defined SPI_FLASH_MULTIPLE_CHIPS
    #define SPI_FLASH_DEVICE_COUNT  4
    #if SPI_FLASH_DEVICE_COUNT >= 4
        #define SPI_DATA_FLASH_SIZE     (SPI_DATA_FLASH_0_SIZE + SPI_DATA_FLASH_1_SIZE + SPI_DATA_FLASH_2_SIZE + SPI_DATA_FLASH_3_SIZE)
    #elif SPI_FLASH_DEVICE_COUNT >= 3
        #define SPI_DATA_FLASH_SIZE     (SPI_DATA_FLASH_0_SIZE + SPI_DATA_FLASH_1_SIZE + SPI_DATA_FLASH_2_SIZE)
    #else
        #define SPI_DATA_FLASH_SIZE     (SPI_DATA_FLASH_0_SIZE + SPI_DATA_FLASH_1_SIZE)
    #endif
#else
    #define SPI_DATA_FLASH_SIZE         SPI_DATA_FLASH_0_SIZE
#endif


#define SPI_FLASH_START        (FLASH_START_ADDRESS + SIZE_OF_FLASH)     // SPI FLASH starts immediately after internal FLASH in the (virtual) memory map

#define SW_UPLOAD_FILE()       (*ucIp_Data == 'H') && (*(ucIp_Data+1) == 'S') && (*(ucIp_Data+2) == '.')  && (fnSPI_Flash_available() != 0)


// FLASH based File System setup
//
#if defined FLASH_FILE_SYSTEM
    #if defined SPI_FILE_SYSTEM                                          // this is a test setup for external SPI FLASH, with the parameters at the end of internal FLASH
        #define uFILE_START      (SPI_FLASH_START)                       // FLASH location at end of internal FLASH
        #if defined KL43Z_256_32_CL
            #define FILE_SYSTEM_SIZE (64 * FILE_GRANULARITY)             // 2M reserved for file system
        #else
            #define FILE_SYSTEM_SIZE (32 * FILE_GRANULARITY)             // 512k reserved for file system
        #endif
    #else
        #if defined FRDM_KL02Z || defined FRDM_KL03Z || defined FRDM_KL05Z || defined FRDM_KE02Z || defined FRDM_KE02Z40M || defined TWR_KV10Z32 // {25}{30}
            #define uFILE_START      (FLASH_START_ADDRESS + (30 * 1024)) // FLASH location at 30k start
            #define FILE_GRANULARITY (1 * FLASH_GRANULARITY)             // each file a multiple of 1k (4k larger devices)
            #define FILE_SYSTEM_SIZE (2 * 1024)                          // 2k reserved for file system
        #elif defined TEENSY_LC || defined FRDM_KL27Z || defined TRK_KEA64
            #define uFILE_START      (FLASH_START_ADDRESS + (63 * 1024)) // FLASH location at 62k start
            #define FILE_GRANULARITY (1 * FLASH_GRANULARITY)             // each file a multiple of 1k
            #define FILE_SYSTEM_SIZE (2 * 1024)                          // 4k reserved for file system
        #elif defined FRDM_KL25Z || defined FRDM_KL26Z || defined FRDM_KL82Z || defined TWR_KL25Z48M || defined FRDM_KE06Z || defined TRK_KEA128 || defined rcARM_KL26 || defined FRDM_KEAZ128Q80 // {21}{24}{30}
            #define uFILE_START      (FLASH_START_ADDRESS + (100 * 1024))// FLASH location at 100k start
            #define FILE_GRANULARITY (2 * FLASH_GRANULARITY)             // each file a multiple of 2k
            #define FILE_SYSTEM_SIZE (28 * 1024)                         // 28k reserved for file system
        #elif (defined FRDM_K20D50M || defined tinyK20) && defined FLEXFLASH_DATA // put parameters and file system in FlexNVM
            #define uFILE_START      (FLASH_START_ADDRESS + SIZE_OF_FLASH - SIZE_OF_FLEXFLASH + PAR_BLOCK_SIZE) // FLASH location in data flash
            #define FILE_GRANULARITY (1 * FLASH_GRANULARITY)             // each file a multiple of 2k
            #define FILE_SYSTEM_SIZE (28 * 1024)                         // 28k reserved for file system
        #elif defined TWR_K20D50M || defined K22F128_100M ||  defined TWR_K20D72M || defined FRDM_K20D50M || defined K02F100M || defined tinyK20
            #define uFILE_START      (FLASH_START_ADDRESS + (100 * 1024))// FLASH location at 100k start
            #define FILE_GRANULARITY (1 * FLASH_GRANULARITY)             // each file a multiple of 2k
            #define FILE_SYSTEM_SIZE (28 * 1024)                         // 28k reserved for file system
        #else
            #define uFILE_START      (FLASH_START_ADDRESS + (144 * 1024))// FLASH location at 140k start {5}
            #if defined KINETIS_K_FPU
                #define FILE_GRANULARITY (1 * FLASH_GRANULARITY)         // each file a multiple of 4k
            #else
                #define FILE_GRANULARITY (2 * FLASH_GRANULARITY)         // each file a multiple of 4k
            #endif
            #if SIZE_OF_FLASH >= (512 * 1025) 
                #define FILE_SYSTEM_SIZE (372 * 1024)                    // 372k reserved for file system {5}
            #else
                #define FILE_SYSTEM_SIZE (SIZE_OF_FLASH - uFILE_START)
            #endif
        #endif
        #define FILES_ADDRESSABLE                                        // file system is addressable (doesn't have to be retrieved)
    #endif
#endif

#if !defined KINETIS_KE
  //#define SUPPORT_PROGRAM_ONCE                                         // {7} enable read and write of program once area
    #define MAC_FROM_USER_REG                                            // {10} MAC address is located in OTP area
#endif

#if defined USE_PARAMETER_BLOCK
    #if FLASH_GRANULARITY == 512                                         // KE parts with 512 byte sectors
        #define PARAMETER_BLOCK_GRANULARITY (2 * FLASH_GRANULARITY)      // use 1k for each block
    #else
        #define PARAMETER_BLOCK_GRANULARITY FLASH_GRANULARITY
    #endif
    #define PARAMETER_BLOCK_SIZE            PARAMETER_BLOCK_GRANULARITY  // use the smallest size for the device
    #if defined USE_PAR_SWAP_BLOCK
        #define PAR_BLOCK_SIZE              (2 * PARAMETER_BLOCK_SIZE)
    #else
        #define PAR_BLOCK_SIZE              (1 * PARAMETER_BLOCK_SIZE)
    #endif
    #if defined ACTIVE_FILE_SYSTEM
        #define PARAMETER_BLOCK_START       (uFILE_START - PAR_BLOCK_SIZE) // parameters just before the file system
    #else
        #define PARAMETER_BLOCK_START       (SIZE_OF_FLASH - PAR_BLOCK_SIZE) // parameters just before the end of flash
    #endif
#else
    #define PAR_BLOCK_SIZE  (0)
#endif

// FLASH configuration settings
//
#define BACKDOOR_KEY_0     0
#define BACKDOOR_KEY_1     0
#define BACKDOOR_KEY_2     0
#define BACKDOOR_KEY_3     0
#define BACKDOOR_KEY_4     0
#define BACKDOOR_KEY_5     0
#define BACKDOOR_KEY_6     0
#define BACKDOOR_KEY_7     0


//#if defined FRDM_K64F                                                  // (removed since mbed bootloader has been corrected) since this board may be used with the MBED loader the flash configuration settings are left with all bits apart from security disable set at '1' - otherwise the MBED loader tends to crash
//  #define KINETIS_FLASH_CONFIGURATION_BACKDOOR_KEY       {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff}
//  #define KINETIS_FLASH_CONFIGURATION_PROGRAM_PROTECTION (0xffffffff)
//  #define KINETIS_FLASH_CONFIGURATION_SECURITY           (0xfe)
//  #define KINETIS_FLASH_CONFIGURATION_NONVOL_OPTION      (0xff)
//#endif
#if defined KINETIS_KE
    #define KINETIS_FLASH_CONFIGURATION_BACKDOOR_KEY       {BACKDOOR_KEY_0, BACKDOOR_KEY_1, BACKDOOR_KEY_2, BACKDOOR_KEY_3, BACKDOOR_KEY_4, BACKDOOR_KEY_5, BACKDOOR_KEY_6, BACKDOOR_KEY_7}
    #define KINETIS_FLASH_CONFIGURATION_PROGRAM_PROTECTION (0xffffffff)  // PROT[24:31]:PROT[23:16]:PROT[15:8]:PROT[7:0] - no protection when all are '1'
    #define KINETIS_FLASH_CONFIGURATION_SECURITY           (FTMRH_FSEC_SEC_UNSECURE | FTMRH_FSEC_KEYEN_ENABLED)
    #define KINETIS_FLASH_CONFIGURATION_NONVOL_OPTION      (0xff)

    // SIM_SOPT has some "write once" fields which are set immediately after starting - if the IRQ on PTA5 is required the reset function must be removed here so that it is possible
    //
    #define SIM_SOPT_KE_DEFAULT                            (SIM_SOPT_RSTPE | SIM_SOPT_SWDE); // reset and SWD pins enabled (nmi disabled)
#else
    #define KINETIS_FLASH_CONFIGURATION_BACKDOOR_KEY       {BACKDOOR_KEY_0, BACKDOOR_KEY_1, BACKDOOR_KEY_2, BACKDOOR_KEY_3, BACKDOOR_KEY_4, BACKDOOR_KEY_5, BACKDOOR_KEY_6, BACKDOOR_KEY_7}
    #define KINETIS_FLASH_CONFIGURATION_PROGRAM_PROTECTION (0xffffffff)  // PROT[24:31]:PROT[23:16]:PROT[15:8]:PROT[7:0] - no protection when all are '1'
    #define KINETIS_FLASH_CONFIGURATION_SECURITY           (FTFL_FSEC_SEC_UNSECURE | FTFL_FSEC_FSLACC_GRANTED | FTFL_FSEC_MEEN_ENABLED | FTFL_FSEC_KEYEN_ENABLED)
    #if defined KINETIS_KL || defined KINETIS_KV
        #if defined ROM_BOOTLOADER
            #define BOOTLOADER_ERRATA
            #if defined TWR_KL43Z48M || defined FRDM_KL43Z || defined FRDM_KL03Z || defined FRDM_KL27Z || defined FRDM_KL82Z || defined CAPUCCINO_KL27
                #define KINETIS_FLASH_CONFIGURATION_NONVOL_OPTION  (FTFL_FOPT_LPBOOT_CLK_DIV_0 | FTFL_FOPT_RESET_PIN_ENABLED | FTFL_FOPT_BOOTSRC_SEL_FLASH | FTFL_FOPT_BOOTPIN_OPT_DISABLE | FTFL_FOPT_NMI_DISABLED) // never use boot ROM
              //#define KINETIS_FLASH_CONFIGURATION_NONVOL_OPTION  (FTFL_FOPT_LPBOOT_CLK_DIV_0 | FTFL_FOPT_RESET_PIN_ENABLED | FTFL_FOPT_BOOTSRC_SEL_FLASH | FTFL_FOPT_BOOTPIN_OPT_ENABLE | FTFL_FOPT_NMI_DISABLED) // use boot ROM if NMI is held low at reset
              //#define KINETIS_FLASH_CONFIGURATION_NONVOL_OPTION (FTFL_FOPT_BOOTSRC_SEL_ROM | FTFL_FOPT_BOOTPIN_OPT_DISABLE | FTFL_FOPT_FAST_INIT | FTFL_FOPT_LPBOOT_CLK_DIV_0 | FTFL_FOPT_RESET_PIN_ENABLED | FTFL_FOPT_NMI_DISABLED) // always use boot ROM
            #else
                #define KINETIS_FLASH_CONFIGURATION_NONVOL_OPTION  (FTFL_FOPT_LPBOOT_CLK_DIV_0 | FTFL_FOPT_RESET_PIN_ENABLED | FTFL_FOPT_BOOTSRC_SEL_FLASH | FTFL_FOPT_BOOTPIN_OPT_DISABLE | FTFL_FOPT_NMI_ENABLED)
            #endif
        #else
            #define KINETIS_FLASH_CONFIGURATION_NONVOL_OPTION  (FTFL_FOPT_LPBOOT_CLK_DIV_8 | FTFL_FOPT_RESET_PIN_ENABLED)
        #endif
    #else
        #if defined KINETIS_REVISION_2
            #if defined FRDM_K64F
                #define KINETIS_FLASH_CONFIGURATION_NONVOL_OPTION  (FTFL_FOPT_EZPORT_DISABLED | FTFL_FOPT_LPBOOT_NORMAL | FTFL_FOPT_NMI_DISABLED) // there is a large capacitor on the NMI/EzP_CS input so these are disabled to allow it to start without requiring an NMI handler or moving to EzPort mode
            #else
                #define KINETIS_FLASH_CONFIGURATION_NONVOL_OPTION  (FTFL_FOPT_EZPORT_ENABLED | FTFL_FOPT_LPBOOT_NORMAL | FTFL_FOPT_NMI_DISABLED)
            #endif
        #else
            #define KINETIS_FLASH_CONFIGURATION_NONVOL_OPTION  (FTFL_FOPT_EZPORT_ENABLED | FTFL_FOPT_LPBOOT_NORMAL | FTFL_FOPT_NMI_ENABLED)
        #endif
    #endif
#endif
#define KINETIS_FLASH_CONFIGURATION_EEPROM_PROT        0xff
#define KINETIS_FLASH_CONFIGURATION_DATAFLASH_PROT     0xff


// Serial interfaces
//
#if defined SERIAL_INTERFACE
    #define WELCOME_MESSAGE_UART   "\r\n\nHello, world... KINETIS\r\n"
    #define NUMBER_EXTERNAL_SERIAL 0                                     // when set to 2 or 4 then one or two SC16IS7XX can be connected for further UARTs via SPI
    #if NUMBER_EXTERNAL_SERIAL > 0
        #define EXT_UART_SC16IS7XX
    #endif
    #define NUMBER_SERIAL   (UARTS_AVAILABLE + LPUARTS_AVAILABLE)        // the number of physical queues needed for serial interface(s)
    #define SIM_COM_EXTENDED                                             // COM ports defined from 1..255
    #define SERIAL_PORT_0    4                                           // if we open UART channel 0 we simulate using comx on the PC
    #define SERIAL_PORT_1    4                                           // if we open UART channel 1 we simulate using comx on the PC
    #define SERIAL_PORT_2    6                                           // if we open UART channel 2 we simulate using comx on the PC
    #define SERIAL_PORT_3    4                                           // if we open UART channel 3 we simulate using comx on the PC
    #define SERIAL_PORT_4    4                                           // if we open UART channel 4 we simulate using comx on the PC
    #define SERIAL_PORT_5    4                                           // if we open UART channel 5 we simulate using comx on the PC

    #if defined KWIKSTIK || defined TWR_K60F120M || defined K20FX512_120 || defined TWR_K21F120M || (defined TWR_K64F120M && defined TWR_SER) || (defined TWR_K60N512 && defined DEBUG_ON_VIRT_COM)
        #define DEMO_UART    5                                           // use UART 5
    #elif defined FRDM_K82F
        #define DEMO_UART    4                                           // use UART 4
    #elif defined TWR_K70F120M || defined EMCRAFT_K70F120M || defined EMCRAFT_K61F150M || defined TRK_KEA128 || defined TRK_KEA64 || defined FRDM_KEAZN32Q64 || defined FRDM_KEAZ64Q64 || defined FRDM_KEAZ128Q80 || defined TWR_KL46Z48M || defined TWR_KL43Z48M || defined TWR_K21D50M || defined TWR_K65F180M || defined TEENSY_3_5 || defined TEENSY_3_6 // {9}{23}
        #define DEMO_UART    2                                           // use UART 2
    #elif defined TWR_K20D50M || defined TWR_K80F150M || defined tinyK20 || defined TWR_K20D72M || defined NET_K60 || defined FRDM_KE02Z || defined FRDM_KE02Z40M || defined FRDM_KE06Z || defined FRDM_K22F || defined TWR_K22F120M || defined TWR_K24F120M || defined TWR_K64F120M || defined TWR_KW21D256 || defined TWR_KW24D512 || defined rcARM_KL26 || defined BLAZE_K22 || defined K22F128_100M // {2}{16}{25}{30}
        #define DEMO_UART    1                                           // use UART 1
    #elif defined K02F100M || defined FRDM_K20D50M || defined FRDM_KL46Z || defined FRDM_KL43Z || defined FRDM_KL25Z || defined FRDM_KL26Z || defined FRDM_KL27Z || defined FRDM_KL82Z || defined CAPUCCINO_KL27 || defined TEENSY_LC || defined TWR_KL25Z48M || defined FRDM_KL02Z || defined FRDM_KL03Z || defined FRDM_KL05Z || defined TRK_KEA8 || defined TEENSY_3_1 || defined FRDM_KE04Z || defined FRDM_K64F || defined FRDM_K66F || defined TWR_KV10Z32  || defined TWR_KV31F120M || defined FRDM_KV31F || ((defined TWR_K40X256 || defined TWR_K40D100M) && defined DEBUG_ON_VIRT_COM) || defined FreeLON // {21}{22}{24}{25}
        #define DEMO_UART    0                                           // use UART 0
    #elif defined NET_KBED                                               // {16}
        #if defined KBEDM_BOARD
            #if defined SPI_LCD
                #define DEMO_UART    5                                   // use UART 5 KBED
            #else
                #define DEMO_UART    0                                   // use UART 0 KBED
            #endif
        #elif defined TESTBED_BOARD                   
            #define DEMO_UART    5                                       // use UART 5 KBED
        #elif defined MXBASE_BOARD 
            #define DEMO_UART    1                                       // use UART 1 KBED
        #else
            #define DEMO_UART    5                                       // use UART 5        
        #endif
    #else
        #define DEMO_UART        3                                       // use UART 3
    #endif
    #if defined FRDM_KL03Z || defined FRDM_KL43Z || defined FRDM_KL27Z || defined FRDM_KL82Z || defined CAPUCCINO_KL27 || defined TWR_KL43Z48M || defined FRDM_K22F || defined K22F128_100M || defined TWR_KV31F120M || defined TWR_K80F150M || defined FRDM_K82F
        #define LPUART_IRC48M                                            // if the 48MHz clock is available clock the LPUART from it
      //#define LPUART_OSCERCLK                                          // clock the LPUART from the external clock
      //#define LPUART_MCGIRCLK                                          // clock the LPUART from MCGIRCLK (IRC8M/FCRDIV/LIRC_DIV2) - default if others are not defined
    #endif
    #if defined FRDM_KL02Z && defined USE_MODBUS
        #undef DEMO_UART
        #define MODBUS_UART_0    0                                       // single UART available so disable debug output in favour of Modbus
    #else
        #define MODBUS_UART_0    1                                       // note - to use via USB use NUMBER_SERIAL instead of UART number
        #define MODBUS_UART_1    2
        #define MODBUS_UART_2    0
    #endif
    #define PPP_UART             4                                       // use UART 4 for PPP
    #define FREEMASTER_UART_CH   1                                       // user UART 1 for FreeMaster via UART
    #if !defined DEVICE_WITHOUT_DMA
        #if defined KINETIS_K_FPU || defined TWR_K20D50M || defined FRDM_K20D50M || defined tinyK20 || defined KINETIS_KL || defined KINETIS_KV
            #define SERIAL_SUPPORT_DMA                                   // enable UART DMA support
          //#define SERIAL_SUPPORT_DMA_RX                                // enable also DMA on receiver (used less that transmit DMA)
          //#define SERIAL_SUPPORT_DMA_RX_FREERUN                        // support free-running reception mode
        #else
            #define SERIAL_SUPPORT_DMA                                   // enable UART DMA support
            #define SERIAL_SUPPORT_DMA_RX                                // enable also DMA on receiver (used less that transmit DMA)
          //#define SERIAL_SUPPORT_DMA_RX_FREERUN                        // support free-running reception mode
        #endif
    #endif

    #if defined FRDM_KE04Z || defined TRK_KEA8
        #define TX_BUFFER_SIZE   (64)                                    // the size of demo RS232 input and output buffers
        #define RX_BUFFER_SIZE   (8)
    #elif defined FRDM_KL03Z
        #define TX_BUFFER_SIZE   (128)                                   // the size of demo RS232 input and output buffers
        #define RX_BUFFER_SIZE   (8)
    #elif defined FRDM_KL02Z || defined FRDM_KL03Z || defined FRDM_KL05Z || defined FRDM_KE02Z || defined TRK_KEA64 || defined FRDM_KE02Z40M || defined TEENSY_LC // {25}{30} these devices have small RAM size
        #define TX_BUFFER_SIZE   (256)                                   // the size of RS232 input and output buffers
        #define RX_BUFFER_SIZE   (32)
    #else
        #define TX_BUFFER_SIZE   (QUEUE_TRANSFER)(0.5 * 1024)            // the size of RS232 input and output buffers
        #define RX_BUFFER_SIZE   (128)
    #endif
  //#define TRUE_UART_TX_2_STOPS                                         // allow true 2 stop bit transmission timing on devices without this UART controller support

    #if defined NET_KBED	                                             // {16} UART Pin mappings for KBED
        #define UART0_ON_B                                               // alternative UART0 pin mapping
        #define UART3_ON_B                                               // alternative UART3 pin mapping
        #define UART5_ON_D                                               // alternative UART5 pin mapping	
    #elif defined NET_K60                                                // NETK60 uses default UART1,4,5 pin mapping
        #define UART3_ON_B                                               // alternative UART3 pin mapping
    #elif defined rcARM_KL26
        #define UART1_ON_C                                               // UART1-RX on PTC3
        #define UART1_ON_A_TX                                            // UART1-TX on PTA19 (override)
    #elif defined FRDM_K20D50M || defined TEENSY_3_1 || defined FRDM_KE04Z || defined FRDM_K64F || defined FRDM_K66F || defined TWR_KV10Z32 || defined TWR_KV31F120M || defined FRDM_KV31F || defined FreeLON // {21}{26}
        #define UART0_ON_B                                               // alternative UART0 pin mapping
        #if defined FRDM_K20D50M
            #define UART1_ON_C
        #elif defined FRDM_K64F || defined FreeLON
            #define UART1_ON_C                                           // alternative UART1 pin mapping
            #define UART3_ON_B                                           // alternative UART3 pin mapping
            #define UART4_ON_C                                           // alternative UART4 pin mapping (Bluetooth connector)
        #elif defined TEENSY_3_1
            #define UART1_ON_C
        #endif
    #elif defined tinyK20
        #define UART1_ON_C
    #elif defined TWR_K21F120M
        #define UART5_ON_D                                               // alternative UART5 pin mapping
        #define UART1_ON_C
        #define UART3_ON_C
        #define UART4_ON_C
    #elif defined FRDM_KL46Z || defined FRDM_KL25Z || defined FRDM_KL26Z || defined FRDM_KL27Z || defined CAPUCCINO_KL27 // {22}{24}
        #define UART0_A_LOW                                              // alternative UART0 pin mapping
        #if defined FRDM_KL27Z || defined CAPUCCINO_KL27
            #define LPUART1_ON_E
        #endif
    #elif defined TWR_K70F120M || defined TWR_K65F180M
        #define UART2_ON_E                                               // alternative UART2 pin mapping on port E (default is on port D)
        #define UART3_ON_F                                               // alternative UART2 pin mapping on port F
    #elif defined EMCRAFT_K70F120M || defined EMCRAFT_K61F150M || defined TWR_K21D50M || defined TWR_KL46Z48M // {9}{23}
        #if defined TWR_K21D50M
            #define UART1_ON_C                                           // alternative UART1 pin mapping on port C (default is on port E)
        #endif
        #define UART2_ON_E                                               // alternative UART2 pin mapping on port E (default is on port D)
        #define UART3_ON_C                                               // alternative UART3 pin mapping
    #elif defined K20FX512_120
        #define UART4_ON_C                                               // alternative UART4 pin mapping
        #define UART3_ON_B                                               // alternative UART3 pin mapping
        #define UART1_ON_C                                               // alternative UART1 pin mapping (default is on port E)
    #elif defined FRDM_KL43Z || defined KL43Z_256_32_CL
        #define UART0_A_LOW                                              // alternative UART0 pin mapping
        #define UART2_ON_E_HIGH                                          // alternative UART2 pin mapping
    #elif defined TWR_KL43Z48M
        #define UART2_ON_E_HIGH                                          // alternative UART2 pin mapping
    #elif ((defined TWR_K40X256 || defined TWR_K40D100M) && defined DEBUG_ON_VIRT_COM) || defined FRDM_K22F || defined K22F128_100M
        #define UART0_ON_D                                               // alternative UART0 pin mapping
    #elif defined TWR_K64F120M
        #define UART1_ON_C                                               // alternative UART1 pin mapping
    #elif defined TEENSY_LC
        #define UART0_ON_B                                               // alternative UART0 pin mappingC
        #define UART1_ON_C
    #elif defined TWR_K20D50M
        #define UART0_ON_B                                               // alternative UART0 pin mapping
    #elif defined FRDM_KL82Z
        #define LPUART0_ON_B                                             // alternative LPUART0 pin mapping
    #elif defined TWR_K20D72M
        #define UART3_ON_B                                               // alternative UART3 pin mapping
    #else
      //#define UART0_A_LOW                                              // alternative UART0 pin mapping
      //#define UART0_ON_B                                               // alternative UART0 pin mapping
      //#define UART0_ON_D                                               // alternative UART0 pin mapping
      //#define UART1_ON_C                                               // alternative UART1 pin mapping (default is on port E)
      //#define UART2_ON_E                                               // alternative UART2 pin mapping on port E (default is on port D)
      //#define UART2_ON_F                                               // alternative UART2 pin mapping on port F (default is on port D)
      //#define UART3_ON_B                                               // alternative UART3 pin mapping
      //#define UART3_ON_F                                               // alternative UART3 pin mapping on port F
        #define UART3_ON_C                                               // alternative UART3 pin mapping
      //#define UART4_ON_C                                               // alternative UART4 pin mapping
      //#define UART5_ON_D                                               // alternative UART5 pin mapping
        #if defined KINETIS_KL
          //#define UART0_ON_E                                           // alternative UART0 pin mapping
          //#define UART1_ON_A                                           // alternative UART1 pin mapping
          //defined UART1_ON_A_LOW                                       // alternative UART1 pin mapping
          //#define UART2_ON_E_HIGH                                      // alternative UART2 pin mapping
          //#define UART2_ON_D_HIGH                                      // alternative UART2 pin mapping
        #endif
    #endif
    #if DEMO_UART == 0
        #define CDC_UART_1     1                                         // UARTs to be used with multiple USB-CDC/serial connections
        #define CDC_UART_2     2
        #define CDC_UART_3     3
        #define CDC_UART_4     4
        #define CDC_UART_5     5
    #elif DEMO_UART == 1
        #define CDC_UART_1     0                                         // UARTs to be used with multiple USB-CDC/serial connections
        #define CDC_UART_2     2
        #define CDC_UART_3     3
        #define CDC_UART_4     4
        #define CDC_UART_5     5
    #elif DEMO_UART == 2
        #define CDC_UART_1     0                                         // UARTs to be used with multiple USB-CDC/serial connections
        #define CDC_UART_2     1
        #define CDC_UART_3     3
        #define CDC_UART_4     4
        #define CDC_UART_5     5
    #elif DEMO_UART == 3
        #define CDC_UART_1     0                                         // UARTs to be used with multiple USB-CDC/serial connections
        #define CDC_UART_2     1
        #define CDC_UART_3     2
        #define CDC_UART_4     4
        #define CDC_UART_5     5
    #elif DEMO_UART == 4
        #define CDC_UART_1     0                                         // UARTs to be used with multiple USB-CDC/serial connections
        #define CDC_UART_2     1
        #define CDC_UART_3     2
        #define CDC_UART_4     3
        #define CDC_UART_5     5
    #else
        #define CDC_UART_1     0                                         // UARTs to be used with multiple USB-CDC/serial connections
        #define CDC_UART_2     1
        #define CDC_UART_3     2
        #define CDC_UART_4     3
        #define CDC_UART_5     4
    #endif
#else
    #define TX_BUFFER_SIZE   (256)
    #define RX_BUFFER_SIZE   (256)
#endif

// CAN
//
#if defined TWR_K60N512 || defined TWR_K60D100M || defined TWR_K60F120M || defined TWR_K70F120M || defined TWR_K80F150M || defined NET_KBED	|| defined NET_K60 // {16}
    #define CAN0_ON_PB                                                   // CAN 0 on PB pins rather than PA
#endif
#if defined TWR_K60N512 || defined TWR_K60D100M || defined TWR_K70F120M || defined TWR_K80F150M || defined NET_KBED	|| defined NET_K60 // {16}
    #define CAN1_ON_PE                                                   // CAN 1 on PE pins rather than PC
#endif

// DAC
//
#if !defined KINETIS_KE                                                  // KE devices have no DAC
    #define SUPPORT_DAC                                                  // {15} enable general DAC support
    #if defined SUPPORT_DAC
        #define SUPPORT_DAC0                                             // enable DAC controller 0 support
      //#define SUPPORT_DAC1                                             // enable DAC controller 1 support
    #endif
#endif

// ADC
//
#define SUPPORT_ADC                                                      // {1}
#define ADC_REFERENCE_VOLTAGE                      3300                  // ADC uses 3.3V reference
#define ADC_SIM_STEP_SIZE                          200                   // 200mV steps when simulating

#if defined SUPPORT_ADC
    #define ADC0_0_START_VOLTAGE                   33                    // initial voltages when simulating (mV)
    #define ADC0_1_START_VOLTAGE                   500
    #define ADC0_2_START_VOLTAGE                   1000
    #define ADC0_3_START_VOLTAGE                   1500
    #define ADC0_4_START_VOLTAGE                   2000
    #define ADC0_5_START_VOLTAGE                   2500
    #define ADC0_6_START_VOLTAGE                   3000
    #define ADC0_7_START_VOLTAGE                   3300
    #define ADC0_8_START_VOLTAGE                   600
    #define ADC0_9_START_VOLTAGE                   1000
    #define ADC0_10_START_VOLTAGE                  1500
    #define ADC0_11_START_VOLTAGE                  2000
    #define ADC0_12_START_VOLTAGE                  2500
    #define ADC0_13_START_VOLTAGE                  3000
    #define ADC0_14_START_VOLTAGE                  3300
    #define ADC0_15_START_VOLTAGE                  1500
    #define ADC0_16_START_VOLTAGE                  2000
    #define ADC0_17_START_VOLTAGE                  2500
    #define ADC0_18_START_VOLTAGE                  3000
    #define ADC0_19_START_VOLTAGE                  3300
    #define ADC0_20_START_VOLTAGE                  1500
    #define ADC0_21_START_VOLTAGE                  2000
    #define ADC0_22_START_VOLTAGE                  2500
    #define ADC0_23_START_VOLTAGE                  3000
    #if ADC_CONTROLLERS > 1
        #define ADC1_0_START_VOLTAGE               33
        #define ADC1_1_START_VOLTAGE               500
        #define ADC1_2_START_VOLTAGE               1000
        #define ADC1_3_START_VOLTAGE               1500
        #define ADC1_4_START_VOLTAGE               2000
        #define ADC1_5_START_VOLTAGE               2500
        #define ADC1_6_START_VOLTAGE               3000
        #define ADC1_7_START_VOLTAGE               3300
        #define ADC1_8_START_VOLTAGE               500
        #define ADC1_9_START_VOLTAGE               1000
        #define ADC1_10_START_VOLTAGE              1500
        #define ADC1_11_START_VOLTAGE              2000
        #define ADC1_12_START_VOLTAGE              2500
        #define ADC1_13_START_VOLTAGE              3000
        #define ADC1_14_START_VOLTAGE              3300
        #define ADC1_15_START_VOLTAGE              1500
        #define ADC1_16_START_VOLTAGE              2000
        #define ADC1_17_START_VOLTAGE              2500
        #define ADC1_18_START_VOLTAGE              3000
        #define ADC1_19_START_VOLTAGE              3300
        #define ADC1_20_START_VOLTAGE              1500
        #define ADC1_21_START_VOLTAGE              2000
        #define ADC1_22_START_VOLTAGE              2500
        #define ADC1_23_START_VOLTAGE              3000
    #endif
    #if ADC_CONTROLLERS > 2
        #define ADC2_0_START_VOLTAGE               33
        #define ADC2_1_START_VOLTAGE               500
        #define ADC2_2_START_VOLTAGE               1000
        #define ADC2_3_START_VOLTAGE               1500
        #define ADC2_4_START_VOLTAGE               2000
        #define ADC2_5_START_VOLTAGE               2500
        #define ADC2_6_START_VOLTAGE               3000
        #define ADC2_7_START_VOLTAGE               3300
        #define ADC2_8_START_VOLTAGE               500
        #define ADC2_9_START_VOLTAGE               1000
        #define ADC2_10_START_VOLTAGE              1500
        #define ADC2_11_START_VOLTAGE              2000
        #define ADC2_12_START_VOLTAGE              2500
        #define ADC2_13_START_VOLTAGE              3000
        #define ADC2_14_START_VOLTAGE              3300
        #define ADC2_15_START_VOLTAGE              1500
        #define ADC2_16_START_VOLTAGE              2000
        #define ADC2_17_START_VOLTAGE              2500
        #define ADC2_18_START_VOLTAGE              3000
        #define ADC2_19_START_VOLTAGE              3300
        #define ADC2_20_START_VOLTAGE              1500
        #define ADC2_21_START_VOLTAGE              2000
        #define ADC2_22_START_VOLTAGE              2500
        #define ADC2_23_START_VOLTAGE              3000
    #endif
    #if ADC_CONTROLLERS > 3
        #define ADC3_0_START_VOLTAGE               33
        #define ADC3_1_START_VOLTAGE               500
        #define ADC3_2_START_VOLTAGE               1000
        #define ADC3_3_START_VOLTAGE               1500
        #define ADC3_4_START_VOLTAGE               2000
        #define ADC3_5_START_VOLTAGE               2500
        #define ADC3_6_START_VOLTAGE               3000
        #define ADC3_7_START_VOLTAGE               3300
        #define ADC3_8_START_VOLTAGE               500
        #define ADC3_9_START_VOLTAGE               1000
        #define ADC3_10_START_VOLTAGE              1500
        #define ADC3_11_START_VOLTAGE              2000
        #define ADC3_12_START_VOLTAGE              2500
        #define ADC3_13_START_VOLTAGE              3000
        #define ADC3_14_START_VOLTAGE              3300
        #define ADC3_15_START_VOLTAGE              1500
        #define ADC3_16_START_VOLTAGE              2000
        #define ADC3_17_START_VOLTAGE              2500
        #define ADC3_18_START_VOLTAGE              3000
        #define ADC3_19_START_VOLTAGE              3300
        #define ADC3_20_START_VOLTAGE              1500
        #define ADC3_21_START_VOLTAGE              2000
        #define ADC3_22_START_VOLTAGE              2500
        #define ADC3_23_START_VOLTAGE              3000
    #endif
#endif


#if !defined KINETIS_KL && !defined KINETIS_KE
  //#define SUPPORT_PDB                                                  // {19} support programmable delay block (can be used as timer and/or for triggering ADC/DAC)
#endif

#if defined KL43Z_256_32_CL
    #define SUPPORT_I2S_SAI                                              // support I2S/SAI
#endif

#if !defined KINETIS_KL02
    #define SUPPORT_PITS                                                 // support PITs
#endif
#if defined MODBUS_RTU && !defined SUPPORT_PITS
    #define SUPPORT_PITS                                                 // support PITs
#endif


#if defined SUPPORT_TIMER
    #define SUPPORT_PWM_MODULE                                           // enable PWM support from FlexTimers
    #if defined KINETIS_KE
      //#define FTM0_0_ON_B
      //#define FTM0_1_ON_B
        #define FTM1_0_ON_H
      //#define FTM1_1_ON_E
      //#define FTM2_0_ON_H
        #if defined KINETIS_KE06
          //#define FTM2_0_ON_F
          //#define FTM2_1_ON_F
          //#define FTM2_2_ON_G
          //#define FTM2_3_ON_G
          //#define FTM2_4_ON_G
          //#define FTM2_5_ON_G
        #endif
      //#define FTM2_1_ON_H
      //#define FTM2_2_ON_D
    #else
        #if defined KL43Z_256_32_CL
            #define TPM0_0_ON_E                                          // TPM0 channel 0 on port E rather than port A
            #define TPM1_0_ON_E                                          // TPM1 channel 0 on port E rather than port A
        #endif
      //#define FTM0_0_ON_C                                              // FTM0 channel 0 on port C rather than port A
      //#define FTM0_1_ON_C                                              // FTM0 channel 1 on port C rather than port A
        #if !defined TWR_KL46Z48M && !defined TWR_K64F120M
            #define FTM0_2_ON_C                                          // FTM0 channel 2 on port C rather than port A
        #endif
      //#define FTM0_3_ON_C                                              // FTM0 channel 3 on port C rather than port A
      //#define FTM0_4_ON_D                                              // FTM0 channel 4 on port D rather than port A
      //#define FTM0_5_ON_D                                              // FTM0 channel 5 on port D rather than port A
      //#define FTM0_6_ON_D                                              // FTM0 channel 6 on port D rather than port A
      //#define FTM0_7_ON_D                                              // FTM0 channel 7 on port D rather than port A

      //#define FTM1_0_ALT_A                                             // FTM1 channel 0 on second port A location
      //#define FTM1_0_ON_B                                              // FTM1 channel 0 on port B rather than port A
      //#define FTM1_1_ALT_C                                             // FTM1 channel 1 on second port A location
      //#define FTM1_1_ON_B                                              // FTM1 channel 1 on port B rather than port A

      //#define FTM2_0_ON_B                                              // FTM2 channel 0 on port B rather than port A
      //#define FTM2_1_ON_B                                              // FTM2 channel 1 on port B rather than port A
      //#define FTM2_0_ON_B_LOW
        #define FTM2_0_ON_B
        #define FTM2_1_ON_B_LOW

        #define FTM3_0_ON_D                                              // FTM3 channel 0 on port D rather than port E
        #define FTM3_1_ON_D                                              // FTM3 channel 1 on port D rather than port E
        #define FTM3_2_ON_D                                              // FTM3 channel 2 on port D rather than port E
        #define FTM3_3_ON_D                                              // FTM3 channel 3 on port D rather than port E
        #define FTM3_4_ON_C                                              // FTM3 channel 4 on port C rather than port E
        #define FTM3_5_ON_C                                              // FTM3 channel 5 on port C rather than port E
        #define FTM3_6_ON_C                                              // FTM3 channel 6 on port C rather than port E
        #define FTM3_7_ON_C                                              // FTM3 channel 7 on port C rather than port E

      //#define FTM_CLKIN_1                                              // use CLKIN1 rather than CLKIN0 as FlexTimer external clock
    #endif
#endif

#define FTM_DEBUG_BEHAVIOUR        FTM_CONF_BDMMODE_3                    // allow timer to continue operating when debugging
                                                                         // FTM_CONF_BDMMODE_2 - FTM counter stops and outputs are frozen in bdm mode
                                                                         // FTM_CONF_BDMMODE_1 - FTM counter stops and outputs set to safe state value in bdm mode
                                                                         // FTM_CONF_BDMMODE_0 - FTM counter stops in bdm mode

#define _TIMER_INTERRUPT_SETUP     PIT_SETUP

#define MODBUS0_PIT_TIMER_CHANNEL  0
#define MODBUS1_PIT_TIMER_CHANNEL  1
#define MODBUS2_PIT_TIMER_CHANNEL  2
#define MODBUS3_PIT_TIMER_CHANNEL  3

#define MODBUS0_PIT_INTERRUPT_PRIORITY  PIT0_INTERRUPT_PRIORITY
#define MODBUS1_PIT_INTERRUPT_PRIORITY  PIT1_INTERRUPT_PRIORITY
#define MODBUS2_PIT_INTERRUPT_PRIORITY  PIT2_INTERRUPT_PRIORITY
#define MODBUS3_PIT_INTERRUPT_PRIORITY  PIT3_INTERRUPT_PRIORITY

#if defined USE_MODBUS
    #if defined KINETIS_KL || defined KINETIS_KE
        #define UART_FRAME_END_COMPLETE                                  // make use of the end of character interrupt to inform of real end of frame
    #else
        #define AUTO_RS485_RTS_SUPPORT                                   // the Kinetis allows automatic RS485 RTS control on all UARTs
    #endif
#endif
#undef UART_FRAME_COMPLETE                                               // this can be disabled if not specifically needed for other purposes to MODBUS RS485 mode


#define SUPPORT_PORT_INTERRUPTS                                          // support code for port interrupts (IRQ for KE/KEA devices)
    #if defined FRDM_KL03Z
        #define NO_PORT_INTERRUPTS_PORTA                                 // remove port interrupt support from port A
      //#define NO_PORT_INTERRUPTS_PORTB                                 // remove port interrupt support from port B
    #else
      //#define NO_PORT_INTERRUPTS_PORTA                                 // remove port interrupt support from port A
      //#define NO_PORT_INTERRUPTS_PORTB                                 // remove port interrupt support from port B
      //#define NO_PORT_INTERRUPTS_PORTC                                 // remove port interrupt support from port C
      //#define NO_PORT_INTERRUPTS_PORTD                                 // remove port interrupt support from port D
      //#define NO_PORT_INTERRUPTS_PORTE                                 // remove port interrupt support from port E
    #endif

#if defined SUPPORT_LOW_POWER
    #define UART_PULL_UPS  (PORT_PS_UP_ENABLE)                           // activate pull-ups on UART lines to avoid them floating in low power modes (which causes leakage currents to flow)
    #define SUPPORT_LLWU                                                 // enable support of the low-leakage wakeup unit (when available)
    #if defined FRDM_KL03Z
      //#define NO_WAKEUP_0_7                                            // no wakeup support of LLWU_P0..P7
        #define NO_WAKEUP_8_15                                           // no wakeup support of LLWU_P8..P15
      //#define NO_WAKEUP_MODULE                                         // no wakeup support of LLWU modules
    #endif
#endif

#define SUPPORT_KEYBOARD_INTERRUPTS                                      // support code for keyboard interrupts (KE/KEA devices)

// I2C Interface
//
#if defined I2C_INTERFACE
    #if defined KINETIS_K80
        #define OUR_I2C_CHANNEL       3                                  // use I2C3 for reference
    #elif defined TWR_KL46Z48M || defined TWR_KL25Z48M || defined TWR_K22F120M || defined TWR_K64F120M || defined TWR_K53N512 || defined TWR_K40X256 || defined TWR_K40D100M || defined TWR_K21D50M || defined TWR_K21F120M || defined FRDM_KL27Z
        #define OUR_I2C_CHANNEL       1                                  // use I2C1 for reference
    #else
        #define OUR_I2C_CHANNEL       0                                  // use I2C0 for reference
    #endif
    #define NUMBER_I2C                I2C_AVAILABLE                      // I2C interfaces available

    #if defined FRDM_K20D50M
        #define I2C0_B_LOW                                               // I2C0_SCL on PB0 and I2C0_SDA on PB1
    #elif defined FRDM_K64F || defined FRDM_KL25Z || defined FRDM_KL26Z || defined FRDM_KL46Z || defined FRDM_KL43Z || defined TWR_K24F120M || defined TWR_K70F120M || defined TWR_K65F180M || defined TWR_K80F150M || defined FreeLON
        #define I2C0_ON_E
    #elif defined BLAZE_K22
        #define I2C0_B_LOW
    #elif defined FRDM_K22F || defined K22F128_100M
      //#define I2C0_B_LOW
      //#define I2C0_ON_D
      //#define I2C1_ON_E
    #elif defined FRDM_KL05Z
      //#define I2C0_A_0
      //#define I2C0_A_1
    #elif defined FRDM_KL02Z || defined FRDM_KL03Z
      //#define I2C0_A_0
      //#define I2C1_A_1
    #elif defined FRDM_KE06Z
      //#define I2C0_B
      //#define I2C1_ON_H
    #elif defined FRDM_KE02Z || defined FRDM_KE02Z40M
      //#define I2C0_B
    #elif defined TWR_K20D50M || defined TWR_K20D72M
      //#define I2C0_B_LOW
    #elif defined FRDM_KL27Z
        #define I2C1_ON_D
    #elif !defined KL43Z_256_32_CL
      //#define I2C0_B_LOW                                               // alternative I2C pin location
        #define I2C0_ON_D
      //#define I2C1_ON_E
    #endif
#endif

// LAN interface
//
#if defined ETH_INTERFACE
    #define NUMBER_OF_TX_BUFFERS_IN_ETHERNET_DEVICE         2            // allocate 2 buffers for transmission
    #define NUMBER_OF_RX_BUFFERS_IN_ETHERNET_DEVICE         6            // allocate 6 (full) buffers for reception
    //#define USE_MULTIPLE_BUFFERS                                       // construct the receive buffer space using multiple 256 byte buffers (improved overrun performance for total memory requirement but reduced efficiency due to extra copy)
#endif
#if defined ETH_INTERFACE || defined USB_CDC_RNDIS
    #if !defined USB_TO_ETHERNET
        #define IP_RX_CHECKSUM_OFFLOAD                                   // allow the HW to perform IPv4/v6 UDP, TCP and ICMP checksum verification so that no such calculation is required in code
        #define IP_TX_CHECKSUM_OFFLOAD                                   // allow the HW to insert IPv4/v6 header checksum and so the software doesn't need to calculate and insert this
            #define IP_TX_PAYLOAD_CHECKSUM_OFFLOAD                       // allow the HW to insert IPv4/v6 payload checksum and so the software doesn't need to calculate and insert this
    #endif

    #if defined USE_IPV6
        #define FORCE_PAYLOAD_ICMPV6_TX                                  // calculate value since the automatic off-loading doesn't do it
        #define FORCE_PAYLOAD_ICMPV6_RX                                  // perform checksum in software since the automatic offloading doesn't do it
    #endif

    // Set the interface handling characteristics based on the checksum offload settings used
    //
    #if (!defined IP_TX_CHECKSUM_OFFLOAD && !defined IP_RX_CHECKSUM_OFFLOAD) || defined _WINDOWS
        #define DEFAULT_INTERFACE_CHARACTERISTICS (INTERFACE_NO_TX_CS_OFFLOADING | INTERFACE_NO_RX_CS_OFFLOADING | INTERFACE_NO_TX_PAYLOAD_CS_OFFLOADING) // all checksum calculations are performed in software
    #elif defined IP_TX_CHECKSUM_OFFLOAD && defined IP_RX_CHECKSUM_OFFLOAD
        #if defined IP_TX_PAYLOAD_CHECKSUM_OFFLOAD
            #define DEFAULT_INTERFACE_CHARACTERISTICS (0)                // all checksum calculations are offloaded
        #else
            #define DEFAULT_INTERFACE_CHARACTERISTICS (INTERFACE_NO_TX_PAYLOAD_CS_OFFLOADING) // only calculate checksums on transmitted payload
        #endif
    #elif defined IP_RX_CHECKSUM_OFFLOAD
        #define DEFAULT_INTERFACE_CHARACTERISTICS (INTERFACE_NO_TX_CS_OFFLOADING | INTERFACE_NO_TX_PAYLOAD_CS_OFFLOADING) // only calculate checksums on transmitted frames
    #elif defined IP_TX_PAYLOAD_CHECKSUM_OFFLOAD
        #define DEFAULT_INTERFACE_CHARACTERISTICS     (0)                // all checksum calculations are offloaded
    #else
        #define DEFAULT_INTERFACE_CHARACTERISTICS (INTERFACE_NO_RX_CS_OFFLOADING) // only calculate checksums on received frames
    #endif

    #if defined USE_BUFFERED_TCP                                         // if using a buffer for TCP to allow interractive data applications (like TELNET)
        #define TCP_BUFFER            2800                               // size of TCP buffer (with USE_BUFFERED_TCP) - generous with Kinetis
        #define TCP_BUFFER_FRAME      1400                               // allow this max. TCP frame size
    #endif

    #if defined USE_HTTP
        #define HTTP_BUFFER_LENGTH    1400                               // we send frames with this maximum amount of payload data - generous with Kinetis
    #endif

    #if defined USE_FTP                                                  // specify FTP support details
        #define FTP_WELCOME_RESPONSE  "220 Welcome KINETIS FTP.\r\n"
        //                                 ^^^^^^^^^^^^^^^^^^^           customise the FTP welcome here
    #endif

    #define _MAGIC_RESET_FRAME        0x1234                             // {11} allow a magic Ethernet reception frame to reset the board to boot loader mode (Ethernet protocol used) followed by three repetitions of the same value (total frame length 61 bytes)

  //#define LAN_REPORT_ACTIVITY                                          // transmissions/receptions and errors are reported to chosen task (for link LED control, etc)
    #if defined LAN_REPORT_ACTIVITY
        #define ACTIVITY_LED_OFF()     
        #define ACTIVITY_LED_ON()      
        #define TURN_LINK_LED_ON()     
        #define SPEED_LED_ON()         
        #define SPEED_LED_OFF()        
        #define LINK_DOWN_LEDS()       

        #define CONFIGURE_LAN_LEDS()
    #endif
#endif

// Special support for this processor type
//
#if defined KINETIS_K_FPU || defined TWR_K20D50M || defined TWR_K21D50M || defined FRDM_K20D50M || defined tinyK20 || (defined KINETIS_KL && !defined KINETIS_KL02)
    #define DMA_MEMCPY_SET                                               // memcpy and memset functions performed by DMA (if supported by processor - uses DMA channel DMA_MEMCPY_CHANNEL)
    #define CONFIGURE_CROSSBAR_SWITCH()  AXBS_CRS0 = AXBS_CRS_ARB_ROUND_ROBIN; AXBS_CRS1 = AXBS_CRS_ARB_ROUND_ROBIN // {27} flash and SRAM slaves priority set to rotating so that SW DMA transfers don't block peripheral transfers
#else                                                                    // avoid using DMA with older devices when certain errata haven't been solved
    #define DMA_MEMCPY_SET                                               // memcpy and memset functions performed by DMA (if supported by processor - uses DMA channel DMA_MEMCPY_CHANNEL)
    #define CONFIGURE_CROSSBAR_SWITCH()  
#endif
//#define RUN_LOOPS_IN_RAM                                               // allow certain routines with tight loops to run from SRAM where it is generally faster than from FLASH - uses slightly more SRAM

// Define DMA channel use (channels and priorities must be unique for used peripherals) - {4}
//
#if defined irq_DMA4_ID
    #define DMA_UART0_TX_CHANNEL   3                                     // use this DMA channel when using UART 0 for transmission driven by DMA
    #define DMA_UART1_TX_CHANNEL   4                                     // use this DMA channel when using UART 1 for transmission driven by DMA
    #define DMA_UART2_TX_CHANNEL   5                                     // use this DMA channel when using UART 2 for transmission driven by DMA
    #define DMA_UART3_TX_CHANNEL   6                                     // use this DMA channel when using UART 3 for transmission driven by DMA
    #define DMA_UART4_TX_CHANNEL   7                                     // use this DMA channel when using UART 4 for transmission driven by DMA
    #define DMA_UART5_TX_CHANNEL   8                                     // use this DMA channel when using UART 5 for transmission driven by DMA

    #define DMA_UART0_RX_CHANNEL   9                                     // use this DMA channel when using UART 0 for reception driven by DMA
    #define DMA_UART1_RX_CHANNEL   10                                    // use this DMA channel when using UART 1 for reception driven by DMA
    #define DMA_UART2_RX_CHANNEL   11                                    // use this DMA channel when using UART 2 for reception driven by DMA
    #define DMA_UART3_RX_CHANNEL   12                                    // use this DMA channel when using UART 3 for reception driven by DMA
    #define DMA_UART4_RX_CHANNEL   13                                    // use this DMA channel when using UART 4 for reception driven by DMA
    #define DMA_UART5_RX_CHANNEL   14                                    // use this DMA channel when using UART 5 for reception driven by DMA

    #define DMA_UART0_TX_INT_PRIORITY  (PRIORITY_DMA3)                   // the interrupts used by the DMA transfer completion need to match with the DMA channel used
    #define DMA_UART1_TX_INT_PRIORITY  (PRIORITY_DMA4)                   // the interrupts used by the DMA transfer completion need to match with the DMA channel used
    #define DMA_UART2_TX_INT_PRIORITY  (PRIORITY_DMA5)                   // the interrupts used by the DMA transfer completion need to match with the DMA channel used
    #define DMA_UART3_TX_INT_PRIORITY  (PRIORITY_DMA6)                   // the interrupts used by the DMA transfer completion need to match with the DMA channel used
    #define DMA_UART4_TX_INT_PRIORITY  (PRIORITY_DMA7)                   // the interrupts used by the DMA transfer completion need to match with the DMA channel used
    #define DMA_UART5_TX_INT_PRIORITY  (PRIORITY_DMA8)                   // the interrupts used by the DMA transfer completion need to match with the DMA channel used

    #define DMA_UART0_RX_INT_PRIORITY  (PRIORITY_DMA9)                   // the interrupts used by the DMA transfer completion need to match with the DMA channel used
    #define DMA_UART1_RX_INT_PRIORITY  (PRIORITY_DMA10)                  // the interrupts used by the DMA transfer completion need to match with the DMA channel used
    #define DMA_UART2_RX_INT_PRIORITY  (PRIORITY_DMA11)                  // the interrupts used by the DMA transfer completion need to match with the DMA channel used
    #define DMA_UART3_RX_INT_PRIORITY  (PRIORITY_DMA12)                  // the interrupts used by the DMA transfer completion need to match with the DMA channel used
    #define DMA_UART4_RX_INT_PRIORITY  (PRIORITY_DMA13)                  // the interrupts used by the DMA transfer completion need to match with the DMA channel used
    #define DMA_UART5_RX_INT_PRIORITY  (PRIORITY_DMA14)                  // the interrupts used by the DMA transfer completion need to match with the DMA channel used

    #define DMA_MEMCPY_CHANNEL     15                                    // use this DMA channel when memory to memory operations are performed (this should lowest priority and can be stalled by higher priority channels)
#elif defined KINETIS_KL
    #define DMA_UART0_TX_CHANNEL   0                                     // use this DMA channel when using UART 0 for transmission driven by DMA
    #define DMA_UART1_TX_CHANNEL   1                                     // use this DMA channel when using UART 1 for transmission driven by DMA
    #define DMA_UART2_TX_CHANNEL   2                                     // use this DMA channel when using UART 2 for transmission driven by DMA

    #define DMA_UART0_TX_INT_PRIORITY  (PRIORITY_DMA3)                   // the interrupts used by the DMA transfer completion need to match with the DMA channel used
    #define DMA_UART1_TX_INT_PRIORITY  (PRIORITY_DMA4)                   // the interrupts used by the DMA transfer completion need to match with the DMA channel used
    #define DMA_UART2_TX_INT_PRIORITY  (PRIORITY_DMA5)                   // the interrupts used by the DMA transfer completion need to match with the DMA channel used

    #define DMA_MEMCPY_CHANNEL     3                                     // use lowest priority DMA channel
#else
    #define DMA_UART0_TX_CHANNEL   1                                     // use this DMA channel when using UART 0 for transmission driven by DMA
    #define DMA_UART1_TX_CHANNEL   2                                     // use this DMA channel when using UART 1 for transmission driven by DMA
    #define DMA_UART2_TX_CHANNEL   3                                     // use this DMA channel when using UART 2 for transmission driven by DMA

    #define DMA_UART0_TX_INT_PRIORITY  (PRIORITY_DMA3)                   // the interrupts used by the DMA transfer completion need to match with the DMA channel used
    #define DMA_UART1_TX_INT_PRIORITY  (PRIORITY_DMA4)                   // the interrupts used by the DMA transfer completion need to match with the DMA channel used
    #define DMA_UART2_TX_INT_PRIORITY  (PRIORITY_DMA5)                   // the interrupts used by the DMA transfer completion need to match with the DMA channel used

    #define DMA_MEMCPY_CHANNEL     0
#endif

#if defined KINETIS_KL || defined KINETIS_KE || defined KINETIS_KV10
    // Define interrupt priorities in the system (kinetis KE and KL support 0..3 - 0 is highest priority and 3 is lowest priority)
    //
    #define SYSTICK_PRIORITY           3                                 // lowest priority
    #define PRIORITY_USB_HS_OTG        2
    #define PRIORITY_UART0             2
    #define PRIORITY_UART1             2
    #define PRIORITY_UART2             2
    #define PRIORITY_UART3             2
    #define PRIORITY_UART4             2
    #define PRIORITY_UART5             2
    #define PRIORITY_LPUART0           2
    #define PRIORITY_LPUART1           2
    #define PRIORITY_DMA15             2
    #define PRIORITY_DMA14             2
    #define PRIORITY_DMA13             2
    #define PRIORITY_DMA12             2
    #define PRIORITY_DMA11             2
    #define PRIORITY_DMA10             2
    #define PRIORITY_DMA9              2
    #define PRIORITY_DMA8              2
    #define PRIORITY_DMA7              2
    #define PRIORITY_DMA6              2
    #define PRIORITY_DMA5              2
    #define PRIORITY_DMA4              2
    #define PRIORITY_DMA3              2
    #define PRIORITY_DMA2              2
    #define PRIORITY_DMA1              2
    #define PRIORITY_DMA0              2
    #define LPTMR0_INTERRUPT_PRIORITY  3
    #define PRIORITY_PORT_A_INT        1
    #define PRIORITY_PORT_B_INT        1
    #define PRIORITY_PORT_C_INT        1
    #define PRIORITY_PORT_D_INT        1
    #define PRIORITY_PORT_E_INT        1
    #define PRIORITY_PHY_PORT_INT      PRIORITY_PORT_A_INT
    #define PRIORITY_USB_OTG           3
    #define PRIORITY_PDB               3
    #define PRIORITY_CAN0_MESSAGE      1
    #define PRIORITY_CAN0_BUS_OFF      1
    #define PRIORITY_CAN0_ERROR        1
    #define PRIORITY_CAN0_TX           1
    #define PRIORITY_CAN0_RX           1
    #define PRIORITY_CAN0_WAKEUP       1
    #define PRIORITY_CAN0_IMEU         1
    #define PRIORITY_CAN1_MESSAGE      1
    #define PRIORITY_CAN1_BUS_OFF      1
    #define PRIORITY_CAN1_ERROR        1
    #define PRIORITY_CAN1_TX           1
    #define PRIORITY_CAN1_RX           1
    #define PRIORITY_CAN1_WAKEUP       1
    #define PRIORITY_CAN1_IMEU         1
    #define PRIORITY_HW_TIMER          2
    #define PRIORITY_TIMERS            3
    #define PRIORITY_I2C0              2
    #define PRIORITY_I2C1              2
    #define USB_PIT_INTERRUPT_PRIORITY 2
    #define PIT0_INTERRUPT_PRIORITY    1
    #define PIT1_INTERRUPT_PRIORITY    1
    #define PIT2_INTERRUPT_PRIORITY    1
    #define PIT3_INTERRUPT_PRIORITY    1
    #define PRIORITY_TICK_TIMER        2
    #define PRIORITY_ADC               2
    #define USB_PRIORITY               3
    #define PRIORITY_RTC               3
    #define PRIORITY_PORT_IRQ_INT      3
    #define PRIORITY_KEYBOARD_INT      3
#else
    // Define interrupt priorities in the system (kinetis supports 0..15 - 0 is highest priority and 15 is lowest priority)
    //
    #define SYSTICK_PRIORITY           15                                // lowest priority
    #define PRIORITY_USB_HS_OTG        14
    #define USB_PIT_INTERRUPT_PRIORITY 13
    #define PRIORITY_UART0             12
    #define PRIORITY_UART1             11
    #define PRIORITY_UART2             10
    #define PRIORITY_UART3             9
    #define PRIORITY_UART4             8
    #define PRIORITY_UART5             7
    #define PRIORITY_LPUART0           7
    #define PRIORITY_LPUART1           7
    #define PRIORITY_LPUART2           7
    #define PRIORITY_LPUART3           7
    #define PRIORITY_LPUART4           7
    #define PRIORITY_DMA15             7
    #define PRIORITY_DMA14             7
    #define PRIORITY_DMA13             7
    #define PRIORITY_DMA12             7
    #define PRIORITY_DMA11             7
    #define PRIORITY_DMA10             7
    #define PRIORITY_DMA9              7
    #define PRIORITY_DMA8              7
    #define PRIORITY_DMA7              7
    #define PRIORITY_DMA6              7
    #define PRIORITY_DMA5              7
    #define PRIORITY_DMA4              7
    #define PRIORITY_DMA3              7
    #define PRIORITY_DMA2              7
    #define PRIORITY_DMA1              7
    #define PRIORITY_DMA0              7
    #define LPTMR0_INTERRUPT_PRIORITY  6                                 // {28}
    #define PRIORITY_PORT_A_INT        6
    #define PRIORITY_PORT_B_INT        6
    #define PRIORITY_PORT_C_INT        6
    #define PRIORITY_PORT_D_INT        6
    #define PRIORITY_PORT_E_INT        6
    #define PRIORITY_PHY_PORT_INT      6
    #define PRIORITY_USB_OTG           6
    #define PRIORITY_PDB               6                                 // {19}
    #define PRIORITY_CAN0_MESSAGE      5
    #define PRIORITY_CAN0_BUS_OFF      5
    #define PRIORITY_CAN0_ERROR        5
    #define PRIORITY_CAN0_TX           5
    #define PRIORITY_CAN0_RX           5
    #define PRIORITY_CAN0_WAKEUP       5
    #define PRIORITY_CAN0_IMEU         5
    #define PRIORITY_CAN1_MESSAGE      5
    #define PRIORITY_CAN1_BUS_OFF      5
    #define PRIORITY_CAN1_ERROR        5
    #define PRIORITY_CAN1_TX           5
    #define PRIORITY_CAN1_RX           5
    #define PRIORITY_CAN1_WAKEUP       5
    #define PRIORITY_CAN1_IMEU         5
    #define PRIORITY_HW_TIMER          5
    #define PRIORITY_TIMERS            5
    #define PRIORITY_I2C0              4
    #define PRIORITY_I2C1              4
    #define PRIORITY_I2C2              4
    #define PRIORITY_I2C3              4
    #define PIT0_INTERRUPT_PRIORITY    4
    #define PIT1_INTERRUPT_PRIORITY    15
    #define PIT2_INTERRUPT_PRIORITY    4
    #define PIT3_INTERRUPT_PRIORITY    4
    #define PRIORITY_TICK_TIMER        3
    #define PRIORITY_ADC               2
    #define USB_PRIORITY               2
    #define PRIORITY_RTC               2
    #define PRIORITY_EMAC              1

  //#define EMAC_PREEMPT_LEVEL         (PIT0_INTERRUPT_PRIORITY << __NVIC_PRIORITY_SHIFT) // don't allow interrupts of this priority and below to pre-empt Ethernet transmissions (such interrupts can then safely send Ethernet frames)
    #define LOWEST_PRIORITY_PREEMPT_LEVEL  0                             // normal level is for all interrupts to be able to operate
#endif

#define ADC_ERR_PRIORITY           PRIORITY_ADC

// Ports
//
#if defined NET_KBED                                                     // {16}
    #define DEMO_LED_1             (PORTD_BIT10)                         // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define DEMO_LED_2             (PORTD_BIT11)                         // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define DEMO_LED_3             (PORTC_BIT3)                          // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define DEMO_LED_4             (PORTC_BIT13)                         // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too

    #if defined ETH_INTERFACE 
        #define LAN_REPORT_ACTIVITY                                      // KBED use NetworkIndicator Task for LAN-ACT LED's (KBED has no direct PHY-LED output)
        #define FORCE_PHY_CONFIG
        #define TURN_LINK_LED_ON()           
        #define LINK_DOWN_LEDS()           
        #define SPEED_LED_ON()         
        #define SPEED_LED_OFF()     
    #endif

    #define KBED_LED1_GREEN        (DEMO_LED_1)                          // KBED LED1 green
    #define KBED_LED2_RED          (DEMO_LED_2)                          // KBED LED2 red
    #define KBEDM_LED_GREN         (DEMO_LED_3)                          // PC3 is KBEDM green front LED
    #define KBEDM_LED_RED          (PORTC_BIT18)                         // PC18 is KBEDM red front LED

  //#define SWITCH_1               (PORTA_BIT6)                          // not connected on KBED

    #define INIT_PHY_RESET()       _CONFIG_DRIVE_PORT_OUTPUT_VALUE(A, (PORTA_BIT29), (PORTA_BIT29), (PORT_ODE | PORT_SRE_SLOW | PORT_DSE_HIGH)) // PA29 is the PHY-/RESET 
    #define PHY_RESET_LOW()        _CLEARBITS(A, PORTA_BIT29)   
    #define PHY_RESET_HIGH()       _SETBITS(A, PORTA_BIT29)   
    #define PHY_RESET_20MS    	   INIT_PHY_RESET(); PHY_RESET_LOW(); fnDelayLoop((unsigned long)((float)20000 * (float)((float)25000000/(float)SYSTEM_CLOCK))); PHY_RESET_HIGH();  // 20ms PHY Reset (during this ext. CPU clock is not available) !
 
    #define INIT_WATCHDOG_DISABLE() 
    #define WATCHDOG_DISABLE()     (0)
    #define ACTIVATE_WATCHDOG()    UNLOCK_WDOG(); WDOG_TOVALL = (2000/5); WDOG_TOVALH = 0; WDOG_STCTRLH = (WDOG_STCTRLH_STNDBYEN | WDOG_STCTRLH_WAITEN | WDOG_STCTRLH_STOPEN | WDOG_STCTRLH_WDOGEN) // watchdog enabled to generate reset on 2s timeout (no further updates allowed)
 
    #define CONFIG_TEST_OUTPUT()                                       
    #define TOGGLE_TEST_OUTPUT()
    #define SET_TEST_OUTPUT()
    #define CLEAR_TEST_OUTPUT()

    #if defined KBEDM_BOARD	                                             // KBEDM
        #define INIT_WATCHDOG_LED()   _CONFIG_DRIVE_PORT_OUTPUT_VALUE(D, (KBED_LED1_GREEN), (KBED_LED1_GREEN), (PORT_ODE | PORT_SRE_SLOW | PORT_DSE_HIGH)); \
                                      _CONFIG_DRIVE_PORT_OUTPUT_VALUE(C, (PORTC_BIT3), (PORTC_BIT3), (PORT_ODE | PORT_SRE_SLOW | PORT_DSE_HIGH)); // PC3 is KBEDM green front LED	
        #if defined KBED_FPGA
            #define TOGGLE_WATCHDOG_LED()  _TOGGLE_PORT(D, KBED_LED1_GREEN); // add toggle front LED over FPGA	   
            #define CONFIGURE_LAN_LEDS()    
            #define ACTIVITY_LED_ON()
            #define ACTIVITY_LED_OFF()
        #else
            #define TOGGLE_WATCHDOG_LED()  _TOGGLE_PORT(D, KBED_LED1_GREEN); _TOGGLE_PORT(C, KBEDM_LED_GREN);
            #define CONFIGURE_LAN_LEDS()   _CONFIG_DRIVE_PORT_OUTPUT_VALUE(D, (PORTD_BIT7), (PORTD_BIT7), (PORT_ODE | PORT_SRE_SLOW | PORT_DSE_HIGH))
            #define ACTIVITY_LED_ON()      _CLEARBITS(D, PORTD_BIT7)     // KBEDM RJ45 LED yellow
            #define ACTIVITY_LED_OFF()     _SETBITS(D, PORTC_BIT7)
        #endif         
    #else	                                                             // default KBED / no board
        #define INIT_WATCHDOG_LED()        _CONFIG_DRIVE_PORT_OUTPUT_VALUE(D, (KBED_LED1_GREEN), (KBED_LED1_GREEN), (PORT_ODE | PORT_SRE_SLOW | PORT_DSE_HIGH)); 
        #define TOGGLE_WATCHDOG_LED()      _TOGGLE_PORT(D, KBED_LED1_GREEN);              
        #define CONFIGURE_LAN_LEDS()                                     // no LAN LED's default
        #define ACTIVITY_LED_ON()
        #define ACTIVITY_LED_OFF()
    #endif

    #define MAPPED_DEMO_LED_1      0                                     // dummy - not used
    #define MAPPED_DEMO_LED_2      0

    #define USER_CHIP_PACKAGE        "KBED_K60//uTasker-embedded-control.bmp" // user defined chip image
        #define ETH_LED_FRAME_LEFT   337                                 // define the Ethernet LED frame location
        #define ETH_LED_FRAME_RIGHT  372
        #define ETH_LED_FRAME_TOP    192
        #define ETH_LED_FRAME_BOTTOM 200
        #define ETH_LED_WIDTH        10                                  // width of each LED

        #define USB_SYMBOL_LEFT      294                                 // define the USB symbol location and size
        #define USB_SYMBOL_TOP       231
        #define USB_SYMBOL_RIGHT     346
        #define USB_SYMBOL_BOTTOM    251
        #define USB_CIRCLE_RADIUS    4

    #if defined NET_I2C_8E8A
        #define KEYPAD               "KBED_K60//kBedM-Pinout-all-8E8A.bmp" // board image
        #define X_OFFSET             (192 + 207)
        #define I2C_LED_SPACE        15
        #define _I2C_OUTPUTS \
            {RGB(0,255,0),     RGB(70,70,70), 1,  { (X_OFFSET - 360),                        (163 + Y_OFFSET), 0, 4 }, _PORT_EXP_0, PORTA_BIT8}, \
            {RGB(0,255,0),     RGB(70,70,70), 1,  {((X_OFFSET + (1 * I2C_LED_SPACE)) - 360), (163 + Y_OFFSET), 0, 4 }, _PORT_EXP_0, PORTA_BIT9}, \
            {RGB(0,255,0),     RGB(70,70,70), 1,  {((X_OFFSET + (2 * I2C_LED_SPACE)) - 360), (163 + Y_OFFSET), 0, 4 }, _PORT_EXP_0, PORTA_BIT10}, \
            {RGB(0,255,0),     RGB(70,70,70), 1,  {((X_OFFSET + (3 * I2C_LED_SPACE)) - 360), (163 + Y_OFFSET), 0, 4 }, _PORT_EXP_0, PORTA_BIT11}, \
            {RGB(0,255,0),     RGB(70,70,70), 1,  {((X_OFFSET + (4 * I2C_LED_SPACE)) - 360), (163 + Y_OFFSET), 0, 4 }, _PORT_EXP_0, PORTA_BIT12}, \
            {RGB(0,255,0),     RGB(70,70,70), 1,  {((X_OFFSET + (5 * I2C_LED_SPACE)) - 360), (163 + Y_OFFSET), 0, 4 }, _PORT_EXP_0, PORTA_BIT13}, \
            {RGB(0,255,0),     RGB(70,70,70), 1,  {((X_OFFSET + (6 * I2C_LED_SPACE)) - 360), (163 + Y_OFFSET), 0, 4 }, _PORT_EXP_0, PORTA_BIT14}, \
            {RGB(0,255,0),     RGB(70,70,70), 1,  {((X_OFFSET + (7 * I2C_LED_SPACE)) - 360), (163 + Y_OFFSET), 0, 4 }, _PORT_EXP_0, PORTA_BIT15}

        #define I2C_INPUT_SPACE      15
        #define _I2C_INPUTS \
            {_PORT_EXP_0, PORTA_BIT0, { 33,                          307,  46,                          317 }}, \
            {_PORT_EXP_0, PORTA_BIT1, {(33 + (1 * I2C_INPUT_SPACE)), 307, (46 + (1 * I2C_INPUT_SPACE)), 317 }}, \
            {_PORT_EXP_0, PORTA_BIT2, {(33 + (2 * I2C_INPUT_SPACE)), 307, (46 + (2 * I2C_INPUT_SPACE)), 317 }}, \
            {_PORT_EXP_0, PORTA_BIT3, {(33 + (3 * I2C_INPUT_SPACE)), 307, (46 + (3 * I2C_INPUT_SPACE)), 317 }}, \
            {_PORT_EXP_0, PORTA_BIT4, {(33 + (4 * I2C_INPUT_SPACE)), 307, (46 + (4 * I2C_INPUT_SPACE)), 317 }}, \
            {_PORT_EXP_0, PORTA_BIT5, {(33 + (5 * I2C_INPUT_SPACE)), 307, (46 + (5 * I2C_INPUT_SPACE)), 317 }}, \
            {_PORT_EXP_0, PORTA_BIT6, {(33 + (6 * I2C_INPUT_SPACE)), 307, (46 + (6 * I2C_INPUT_SPACE)), 317 }}, \
            {_PORT_EXP_0, PORTA_BIT7, {(33 + (7 * I2C_INPUT_SPACE)), 307, (46 + (7 * I2C_INPUT_SPACE)), 317 }}

        #define _EXTERNAL_PORT_COUNT 1                                   // 1 external 16 bit port based on a port expander
        #define PCF8575_CNT          1                                   // port expander type and quantity
        #define _EXT_PORT_16_BIT                                         // external ports are 16 bits wide
        extern void fnSetI2CPort(int port, int change, unsigned long bit); // simulator interface
        #define HANDLE_EXT_PORT(port, change, bit) fnSetI2CPort(port, change, bit)
    #else
        #define KEYPAD               "KBED_K60//kBedM-Pinout-all.bmp"  // board image
        #define X_OFFSET             192
        #define _I2C_OUTPUTS
        #define _I2C_INPUTS
    #endif
        #define Y_OFFSET             52

        #define LED_X_SIZE           10
        #define LED_X_SPACE          0
        #define LED_Y_SIZE           10
        #define LED_Y_SPACE          0
             // '0'          '  1'       input state    center (x,   y)   0 = circle, radius, controlling port, controlling pin 
        #define KEYPAD_LED_DEFINITIONS  \
            {RGB(0,255,0),     RGB(70,70,70), 1,  {(X_OFFSET - 196), (152 + Y_OFFSET), 0, 6 }, _PORTC, PORTC_BIT3}, \
            {RGB(255,0,0),     RGB(70,70,70), 1,  {(X_OFFSET - 180), (152 + Y_OFFSET), 0, 6 }, _PORTC, PORTC_BIT18}, \
            {RGB(0,255,0),     RGB(70,70,70), 1,  {(X_OFFSET + 109), (124 + Y_OFFSET), (X_OFFSET + 122), (132 + Y_OFFSET) }, _PORTD, PORTD_BIT10}, \
            {RGB(255,0,0),     RGB(70,70,70), 1,  {(X_OFFSET + 109), (140 + Y_OFFSET), (X_OFFSET + 122), (148 + Y_OFFSET) }, _PORTD, PORTD_BIT11}, \
            {RGB(255,255,255), RGB(255,0,0),  0,  {(0   + X_OFFSET), (0   + Y_OFFSET), (0 + LED_X_SIZE + X_OFFSET), (LED_Y_SIZE + Y_OFFSET) }, _PORTA, 0}, \
            {RGB(255,255,255), RGB(255,0,0),  0,  {((1 * (LED_X_SIZE + LED_X_SPACE)) + X_OFFSET), (0   + Y_OFFSET), ((2 * LED_X_SIZE) + (1 * LED_X_SPACE) + X_OFFSET), ((1 * LED_Y_SIZE) + Y_OFFSET) }, _PORTE, PORTE_BIT25}, \
            {RGB(255,255,255), RGB(255,0,0),  0,  {((2 * (LED_X_SIZE + LED_X_SPACE)) + X_OFFSET), (0   + Y_OFFSET), ((3 * LED_X_SIZE) + (2 * LED_X_SPACE) + X_OFFSET), ((1 * LED_Y_SIZE) + Y_OFFSET) }, _PORTE, PORTE_BIT24}, \
            {RGB(255,255,255), RGB(255,0,0),  0,  {((3 * (LED_X_SIZE + LED_X_SPACE)) + X_OFFSET), (0   + Y_OFFSET), ((4 * LED_X_SIZE) + (3 * LED_X_SPACE) + X_OFFSET), ((1 * LED_Y_SIZE) + Y_OFFSET) }, _PORTD, PORTD_BIT9}, \
            {RGB(255,255,255), RGB(255,0,0),  0,  {((4 * (LED_X_SIZE + LED_X_SPACE)) + X_OFFSET), (0   + Y_OFFSET), ((5 * LED_X_SIZE) + (4 * LED_X_SPACE) + X_OFFSET), ((1 * LED_Y_SIZE) + Y_OFFSET) }, _PORTD, PORTD_BIT8}, \
            {RGB(255,255,255), RGB(255,0,0),  0,  {((5 * (LED_X_SIZE + LED_X_SPACE)) + X_OFFSET), (0   + Y_OFFSET), ((6 * LED_X_SIZE) + (5 * LED_X_SPACE) + X_OFFSET), ((1 * LED_Y_SIZE) + Y_OFFSET) }, _PORTC, PORTC_BIT1}, \
            {RGB(255,255,255), RGB(255,0,0),  0,  {((6 * (LED_X_SIZE + LED_X_SPACE)) + X_OFFSET), (0   + Y_OFFSET), ((7 * LED_X_SIZE) + (6 * LED_X_SPACE) + X_OFFSET), ((1 * LED_Y_SIZE) + Y_OFFSET) }, _PORTC, PORTC_BIT2}, \
            {RGB(255,255,255), RGB(255,0,0),  0,  {((7 * (LED_X_SIZE + LED_X_SPACE)) + X_OFFSET), (0   + Y_OFFSET), ((8 * LED_X_SIZE) + (7 * LED_X_SPACE) + X_OFFSET), ((1 * LED_Y_SIZE) + Y_OFFSET) }, _PORTA, PORTA_BIT8}, \
            {RGB(255,255,255), RGB(255,0,0),  0,  {((8 * (LED_X_SIZE + LED_X_SPACE)) + X_OFFSET), (0   + Y_OFFSET), ((9 * LED_X_SIZE) + (8 * LED_X_SPACE) + X_OFFSET), ((1 * LED_Y_SIZE) + Y_OFFSET) }, _PORTE, 0}, \
            {RGB(255,255,255), RGB(255,0,0),  0,  {((9 * (LED_X_SIZE + LED_X_SPACE)) + X_OFFSET), (0   + Y_OFFSET), ((10 * LED_X_SIZE) + (9 * LED_X_SPACE) + X_OFFSET),((1 * LED_Y_SIZE) + Y_OFFSET) }, _PORTA, PORTA_BIT9}, \
            {RGB(255,255,255), RGB(255,0,0),  0,  {((10 * (LED_X_SIZE + LED_X_SPACE)) + X_OFFSET), (0   + Y_OFFSET), ((11 * LED_X_SIZE) + (10 * LED_X_SPACE) + X_OFFSET), ((1 * LED_Y_SIZE) + Y_OFFSET) }, _PORTA, PORTA_BIT10}, \
            {RGB(255,255,255), RGB(255,0,0),  0,  {((11 * (LED_X_SIZE + LED_X_SPACE)) + X_OFFSET), (0   + Y_OFFSET), ((12 * LED_X_SIZE) + (11 * LED_X_SPACE) + X_OFFSET), ((1 * LED_Y_SIZE) + Y_OFFSET) }, _PORTA, PORTA_BIT11}, \
            {RGB(255,255,255), RGB(255,0,0),  0,  {((12 * (LED_X_SIZE + LED_X_SPACE)) + X_OFFSET), (0   + Y_OFFSET), ((13 * LED_X_SIZE) + (12 * LED_X_SPACE) + X_OFFSET), ((1 * LED_Y_SIZE) + Y_OFFSET) }, _PORTA, 0}, \
            {RGB(255,255,255), RGB(255,0,0),  0,  {((13 * (LED_X_SIZE + LED_X_SPACE)) + X_OFFSET), (0   + Y_OFFSET), ((14 * LED_X_SIZE) + (13 * LED_X_SPACE) + X_OFFSET), ((1 * LED_Y_SIZE) + Y_OFFSET) }, _PORTB, PORTB_BIT2}, \
            {RGB(255,255,255), RGB(255,0,0),  0,  {((14 * (LED_X_SIZE + LED_X_SPACE)) + X_OFFSET), (0   + Y_OFFSET), ((15 * LED_X_SIZE) + (14 * LED_X_SPACE) + X_OFFSET), ((1 * LED_Y_SIZE) + Y_OFFSET) }, _PORTA, PORTA_BIT4}, \
            {RGB(255,255,255), RGB(255,0,0),  0,  {((15 * (LED_X_SIZE + LED_X_SPACE)) + X_OFFSET), (0   + Y_OFFSET), ((16 * LED_X_SIZE) + (15 * LED_X_SPACE) + X_OFFSET), ((1 * LED_Y_SIZE) + Y_OFFSET) }, _PORTA, PORTA_BIT6}, \
            {RGB(255,255,255), RGB(255,0,0),  0,  {((16 * (LED_X_SIZE + LED_X_SPACE)) + X_OFFSET), (0   + Y_OFFSET), ((17 * LED_X_SIZE) + (16 * LED_X_SPACE) + X_OFFSET), ((1 * LED_Y_SIZE) + Y_OFFSET) }, _PORTE, 0}, \
            {RGB(255,255,255), RGB(255,0,0),  0,  {(0   + X_OFFSET), (LED_Y_SIZE + LED_Y_SPACE + Y_OFFSET), (0 + LED_X_SIZE + X_OFFSET), ((2 * LED_Y_SIZE) + LED_Y_SPACE + Y_OFFSET) }, _PORTE, 0}, \
            {RGB(255,255,255), RGB(255,0,0),  0,  {((1 * (LED_X_SIZE + LED_X_SPACE)) + X_OFFSET), (LED_Y_SIZE + LED_Y_SPACE + Y_OFFSET), ((2 * LED_X_SIZE) + (1 * LED_X_SPACE) + X_OFFSET), ((2 * LED_Y_SIZE) + LED_Y_SPACE + Y_OFFSET) }, _PORTD, PORTD_BIT4}, \
            {RGB(255,255,255), RGB(255,0,0),  0,  {((2 * (LED_X_SIZE + LED_X_SPACE)) + X_OFFSET), (LED_Y_SIZE + LED_Y_SPACE + Y_OFFSET), ((3 * LED_X_SIZE) + (2 * LED_X_SPACE) + X_OFFSET), ((2 * LED_Y_SIZE) + LED_Y_SPACE + Y_OFFSET) }, _PORTD, PORTD_BIT5}, \
            {RGB(255,255,255), RGB(255,0,0),  0,  {((3 * (LED_X_SIZE + LED_X_SPACE)) + X_OFFSET), (LED_Y_SIZE + LED_Y_SPACE + Y_OFFSET), ((4 * LED_X_SIZE) + (3 * LED_X_SPACE) + X_OFFSET), ((2 * LED_Y_SIZE) + LED_Y_SPACE + Y_OFFSET) }, _PORTD, PORTD_BIT2}, \
            {RGB(255,255,255), RGB(255,0,0),  0,  {((4 * (LED_X_SIZE + LED_X_SPACE)) + X_OFFSET), (LED_Y_SIZE + LED_Y_SPACE + Y_OFFSET), ((5 * LED_X_SIZE) + (4 * LED_X_SPACE) + X_OFFSET), ((2 * LED_Y_SIZE) + LED_Y_SPACE + Y_OFFSET) }, _PORTD, PORTD_BIT0}, \
            {RGB(255,255,255), RGB(255,0,0),  0,  {((5 * (LED_X_SIZE + LED_X_SPACE)) + X_OFFSET), (LED_Y_SIZE + LED_Y_SPACE + Y_OFFSET), ((6 * LED_X_SIZE) + (5 * LED_X_SPACE) + X_OFFSET), ((2 * LED_Y_SIZE) + LED_Y_SPACE + Y_OFFSET) }, _PORTD, PORTD_BIT3}, \
            {RGB(255,255,255), RGB(255,0,0),  0,  {((6 * (LED_X_SIZE + LED_X_SPACE)) + X_OFFSET), (LED_Y_SIZE + LED_Y_SPACE + Y_OFFSET), ((7 * LED_X_SIZE) + (6 * LED_X_SPACE) + X_OFFSET), ((2 * LED_Y_SIZE) + LED_Y_SPACE + Y_OFFSET) }, _PORTC, PORTC_BIT0}, \
            {RGB(255,255,255), RGB(255,0,0),  0,  {((7 * (LED_X_SIZE + LED_X_SPACE)) + X_OFFSET), (LED_Y_SIZE + LED_Y_SPACE + Y_OFFSET), ((8 * LED_X_SIZE) + (7 * LED_X_SPACE) + X_OFFSET), ((2 * LED_Y_SIZE) + LED_Y_SPACE + Y_OFFSET) }, _PORTC, PORTC_BIT7}, \
            {RGB(255,255,255), RGB(255,0,0),  0,  {((8 * (LED_X_SIZE + LED_X_SPACE)) + X_OFFSET), (LED_Y_SIZE + LED_Y_SPACE + Y_OFFSET), ((9 * LED_X_SIZE) + (8 * LED_X_SPACE) + X_OFFSET), ((2 * LED_Y_SIZE) + LED_Y_SPACE + Y_OFFSET) }, _PORTC, 0}, \
            {RGB(255,255,255), RGB(255,0,0),  0,  {((9 * (LED_X_SIZE + LED_X_SPACE)) + X_OFFSET), (LED_Y_SIZE + LED_Y_SPACE + Y_OFFSET), ((10 * LED_X_SIZE) + (9 * LED_X_SPACE) + X_OFFSET), ((2 * LED_Y_SIZE) + LED_Y_SPACE + Y_OFFSET) }, _PORTC, PORTC_BIT6}, \
            {RGB(255,255,255), RGB(255,0,0),  0,  {((10 * (LED_X_SIZE + LED_X_SPACE)) + X_OFFSET), (LED_Y_SIZE + LED_Y_SPACE + Y_OFFSET), ((11 * LED_X_SIZE) + (10 * LED_X_SPACE) + X_OFFSET), ((2 * LED_Y_SIZE) + LED_Y_SPACE + Y_OFFSET) }, _PORTC, PORTC_BIT5}, \
            {RGB(255,255,255), RGB(255,0,0),  0,  {((11 * (LED_X_SIZE + LED_X_SPACE)) + X_OFFSET), (LED_Y_SIZE + LED_Y_SPACE + Y_OFFSET), ((12 * LED_X_SIZE) + (11 * LED_X_SPACE) + X_OFFSET), ((2 * LED_Y_SIZE) + LED_Y_SPACE + Y_OFFSET) }, _PORTA, 0}, \
            {RGB(255,255,255), RGB(255,0,0),  0,  {((12 * (LED_X_SIZE + LED_X_SPACE)) + X_OFFSET), (LED_Y_SIZE + LED_Y_SPACE + Y_OFFSET), ((13 * LED_X_SIZE) + (12 * LED_X_SPACE) + X_OFFSET), ((2 * LED_Y_SIZE) + LED_Y_SPACE + Y_OFFSET) }, _PORTA, PORTA_BIT7}, \
            {RGB(255,255,255), RGB(255,0,0),  0,  {((13 * (LED_X_SIZE + LED_X_SPACE)) + X_OFFSET), (LED_Y_SIZE + LED_Y_SPACE + Y_OFFSET), ((14 * LED_X_SIZE) + (13 * LED_X_SPACE) + X_OFFSET), ((2 * LED_Y_SIZE) + LED_Y_SPACE + Y_OFFSET) }, _PORTA, PORTA_BIT3}, \
            {RGB(255,255,255), RGB(255,0,0),  0,  {((14 * (LED_X_SIZE + LED_X_SPACE)) + X_OFFSET), (LED_Y_SIZE + LED_Y_SPACE + Y_OFFSET), ((15 * LED_X_SIZE) + (14 * LED_X_SPACE) + X_OFFSET), ((2 * LED_Y_SIZE) + LED_Y_SPACE + Y_OFFSET) }, _PORTA, PORTA_BIT5}, \
            {RGB(255,255,255), RGB(255,0,0),  0,  {((15 * (LED_X_SIZE + LED_X_SPACE)) + X_OFFSET), (LED_Y_SIZE + LED_Y_SPACE + Y_OFFSET), ((16 * LED_X_SIZE) + (15 * LED_X_SPACE) + X_OFFSET), ((2 * LED_Y_SIZE) + LED_Y_SPACE + Y_OFFSET) }, _PORTB, PORTB_BIT7}, \
            {RGB(255,255,255), RGB(255,0,0),  0,  {((16 * (LED_X_SIZE + LED_X_SPACE)) + X_OFFSET), (LED_Y_SIZE + LED_Y_SPACE + Y_OFFSET), ((17 * LED_X_SIZE) + (16 * LED_X_SPACE) + X_OFFSET), ((2 * LED_Y_SIZE) + LED_Y_SPACE + Y_OFFSET) }, _PORTE, 0}, \
            {RGB(255,255,255), RGB(255,0,0),  0,  { (X_OFFSET - 62),                                         (Y_OFFSET + 255), ((X_OFFSET - 62) + LED_X_SIZE), (Y_OFFSET + LED_Y_SIZE + 255) }, _PORTC, 0}, \
            {RGB(255,255,255), RGB(255,0,0),  0,  {((X_OFFSET - 62) + (1 * LED_X_SIZE) + (1 * LED_X_SPACE)), (Y_OFFSET + 255), ((X_OFFSET - 62) + (2 * LED_X_SIZE) + (1 * LED_X_SPACE)), (Y_OFFSET + LED_Y_SIZE + 255) }, _PORTA, PORTA_BIT24}, \
            {RGB(255,255,255), RGB(255,0,0),  0,  {((X_OFFSET - 62) + (2 * LED_X_SIZE) + (2 * LED_X_SPACE)), (Y_OFFSET + 255), ((X_OFFSET - 62) + (3 * LED_X_SIZE) + (2 * LED_X_SPACE)), (Y_OFFSET + LED_Y_SIZE + 255) }, _PORTA, PORTA_BIT25}, \
            {RGB(255,255,255), RGB(255,0,0),  0,  {((X_OFFSET - 62) + (3 * LED_X_SIZE) + (3 * LED_X_SPACE)), (Y_OFFSET + 255), ((X_OFFSET - 62) + (4 * LED_X_SIZE) + (3 * LED_X_SPACE)), (Y_OFFSET + LED_Y_SIZE + 255) }, _PORTA, PORTA_BIT26}, \
            {RGB(255,255,255), RGB(255,0,0),  0,  {((X_OFFSET - 62) + (4 * LED_X_SIZE) + (4 * LED_X_SPACE)), (Y_OFFSET + 255), ((X_OFFSET - 62) + (5 * LED_X_SIZE) + (4 * LED_X_SPACE)), (Y_OFFSET + LED_Y_SIZE + 255) }, _PORTA, PORTA_BIT27}, \
            {RGB(255,255,255), RGB(255,0,0),  0,  {((X_OFFSET - 62) + (5 * LED_X_SIZE) + (5 * LED_X_SPACE)), (Y_OFFSET + 255), ((X_OFFSET - 62) + (6 * LED_X_SIZE) + (5 * LED_X_SPACE)), (Y_OFFSET + LED_Y_SIZE + 255) }, _PORTA, PORTA_BIT28}, \
            {RGB(255,255,255), RGB(255,0,0),  0,  {((X_OFFSET - 62) + (6 * LED_X_SIZE) + (6 * LED_X_SPACE)), (Y_OFFSET + 255), ((X_OFFSET - 62) + (7 * LED_X_SIZE) + (6 * LED_X_SPACE)), (Y_OFFSET + LED_Y_SIZE + 255) }, _PORTE, PORTE_BIT0}, \
            {RGB(255,255,255), RGB(255,0,0),  0,  {((X_OFFSET - 62) + (7 * LED_X_SIZE) + (7 * LED_X_SPACE)), (Y_OFFSET + 255), ((X_OFFSET - 62) + (8 * LED_X_SIZE) + (7 * LED_X_SPACE)), (Y_OFFSET + LED_Y_SIZE + 255) }, _PORTE, PORTE_BIT1}, \
            {RGB(255,255,255), RGB(255,0,0),  0,  {((X_OFFSET - 62) + (8 * LED_X_SIZE) + (8 * LED_X_SPACE)), (Y_OFFSET + 255), ((X_OFFSET - 62) + (9 * LED_X_SIZE) + (8 * LED_X_SPACE)), (Y_OFFSET + LED_Y_SIZE + 255) }, _PORTC, PORTC_BIT8}, \
            {RGB(255,255,255), RGB(255,0,0),  0,  {((X_OFFSET - 62) + (9 * LED_X_SIZE) + (9 * LED_X_SPACE)), (Y_OFFSET + 255), ((X_OFFSET - 62) + (10 * LED_X_SIZE) + (9 * LED_X_SPACE)), (Y_OFFSET + LED_Y_SIZE + 255) }, _PORTC, PORTC_BIT9}, \
            {RGB(255,200,80),  RGB(255,0,0),  0,  {((X_OFFSET - 62) + (10 * LED_X_SIZE) + (10 * LED_X_SPACE)), (Y_OFFSET + 255), ((X_OFFSET - 62) + (11 * LED_X_SIZE) + (10 * LED_X_SPACE)), (Y_OFFSET + LED_Y_SIZE + 255) }, _PORTC, PORTC_BIT10}, \
            {RGB(255,200,80),  RGB(255,0,0),  0,  {((X_OFFSET - 62) + (11 * LED_X_SIZE) + (11 * LED_X_SPACE)), (Y_OFFSET + 255), ((X_OFFSET - 62) + (12 * LED_X_SIZE) + (11 * LED_X_SPACE)), (Y_OFFSET + LED_Y_SIZE + 255) }, _PORTC, PORTC_BIT11}, \
            {RGB(255,255,255), RGB(255,0,0),  0,  {((X_OFFSET - 62) + (12 * LED_X_SIZE) + (12 * LED_X_SPACE)), (Y_OFFSET + 255), ((X_OFFSET - 62) + (13 * LED_X_SIZE) + (12 * LED_X_SPACE)), (Y_OFFSET + LED_Y_SIZE + 255) }, _PORTE, 0}, \
            {RGB(255,255,255), RGB(255,0,0),  0,  {((X_OFFSET - 62) + (13 * LED_X_SIZE) + (13 * LED_X_SPACE)), (Y_OFFSET + 255), ((X_OFFSET - 62) + (14 * LED_X_SIZE) + (13 * LED_X_SPACE)), (Y_OFFSET + LED_Y_SIZE + 255) }, _PORTE, 0}, \
            {RGB(255,255,255), RGB(255,0,0),  0,  {((X_OFFSET - 62) + (14 * LED_X_SIZE) + (14 * LED_X_SPACE)), (Y_OFFSET + 255), ((X_OFFSET - 62) + (15 * LED_X_SIZE) + (14 * LED_X_SPACE)), (Y_OFFSET + LED_Y_SIZE + 255) }, _PORTE, 0}, \
            {RGB(255,255,255), RGB(255,0,0),  0,  {((X_OFFSET - 62) + (15 * LED_X_SIZE) + (15 * LED_X_SPACE)), (Y_OFFSET + 255), ((X_OFFSET - 62) + (16 * LED_X_SIZE) + (15 * LED_X_SPACE)), (Y_OFFSET + LED_Y_SIZE + 255) }, _PORTE, 0}, \
            {RGB(255,255,255), RGB(255,0,0),  0,  {((X_OFFSET - 62) + (16 * LED_X_SIZE) + (16 * LED_X_SPACE)), (Y_OFFSET + 255), ((X_OFFSET - 62) + (17 * LED_X_SIZE) + (16 * LED_X_SPACE)), (Y_OFFSET + LED_Y_SIZE + 255) }, _PORTE, 0}, \
            {RGB(255,255,255), RGB(255,0,0),  0,  {((X_OFFSET - 62) + (17 * LED_X_SIZE) + (17 * LED_X_SPACE)), (Y_OFFSET + 255), ((X_OFFSET - 62) + (18 * LED_X_SIZE) + (17 * LED_X_SPACE)), (Y_OFFSET + LED_Y_SIZE + 255) }, _PORTE, 0}, \
            _I2C_OUTPUTS

        #define BUTTON_KEY_DEFINITIONS  \
            {_PORTE, PORTE_BIT25, {((1 * (LED_X_SIZE + LED_X_SPACE)) + X_OFFSET), (0   + Y_OFFSET), ((2 * LED_X_SIZE) + (1 * LED_X_SPACE) + X_OFFSET), ((1 * LED_Y_SIZE) + Y_OFFSET) }}, \
            {_PORTE, PORTE_BIT24, {((2 * (LED_X_SIZE + LED_X_SPACE)) + X_OFFSET), (0   + Y_OFFSET), ((3 * LED_X_SIZE) + (2 * LED_X_SPACE) + X_OFFSET), ((1 * LED_Y_SIZE) + Y_OFFSET) }}, \
            {_PORTD, PORTD_BIT9,  {((3 * (LED_X_SIZE + LED_X_SPACE)) + X_OFFSET), (0   + Y_OFFSET), ((4 * LED_X_SIZE) + (3 * LED_X_SPACE) + X_OFFSET), ((1 * LED_Y_SIZE) + Y_OFFSET) }}, \
            {_PORTD, PORTD_BIT8,  {((4 * (LED_X_SIZE + LED_X_SPACE)) + X_OFFSET), (0   + Y_OFFSET), ((5 * LED_X_SIZE) + (4 * LED_X_SPACE) + X_OFFSET), ((1 * LED_Y_SIZE) + Y_OFFSET) }}, \
            {_PORTC, PORTC_BIT1,  {((5 * (LED_X_SIZE + LED_X_SPACE)) + X_OFFSET), (0   + Y_OFFSET), ((6 * LED_X_SIZE) + (5 * LED_X_SPACE) + X_OFFSET), ((1 * LED_Y_SIZE) + Y_OFFSET) }}, \
            {_PORTC, PORTC_BIT2,  {((6 * (LED_X_SIZE + LED_X_SPACE)) + X_OFFSET), (0   + Y_OFFSET), ((7 * LED_X_SIZE) + (6 * LED_X_SPACE) + X_OFFSET), ((1 * LED_Y_SIZE) + Y_OFFSET) }}, \
            {_PORTA, PORTA_BIT8,  {((7 * (LED_X_SIZE + LED_X_SPACE)) + X_OFFSET), (0   + Y_OFFSET), ((8 * LED_X_SIZE) + (7 * LED_X_SPACE) + X_OFFSET), ((1 * LED_Y_SIZE) + Y_OFFSET) }}, \
            {_PORTA, PORTA_BIT9,  {((9 * (LED_X_SIZE + LED_X_SPACE)) + X_OFFSET), (0   + Y_OFFSET), ((10 * LED_X_SIZE) + (9 * LED_X_SPACE) + X_OFFSET), ((1 * LED_Y_SIZE) + Y_OFFSET) }}, \
            {_PORTA, PORTA_BIT10, {((10 * (LED_X_SIZE + LED_X_SPACE)) + X_OFFSET), (0   + Y_OFFSET), ((11 * LED_X_SIZE) + (10 * LED_X_SPACE) + X_OFFSET), ((1 * LED_Y_SIZE) + Y_OFFSET) }}, \
            {_PORTA, PORTA_BIT11, {((11 * (LED_X_SIZE + LED_X_SPACE)) + X_OFFSET), (0   + Y_OFFSET), ((12 * LED_X_SIZE) + (11 * LED_X_SPACE) + X_OFFSET), ((1 * LED_Y_SIZE) + Y_OFFSET) }}, \
            {_PORTB, PORTB_BIT2,  {((13 * (LED_X_SIZE + LED_X_SPACE)) + X_OFFSET), (0   + Y_OFFSET), ((14 * LED_X_SIZE) + (13 * LED_X_SPACE) + X_OFFSET), ((1 * LED_Y_SIZE) + Y_OFFSET) }}, \
            {_PORTA, PORTA_BIT4,  {((14 * (LED_X_SIZE + LED_X_SPACE)) + X_OFFSET), (0   + Y_OFFSET), ((15 * LED_X_SIZE) + (14 * LED_X_SPACE) + X_OFFSET), ((1 * LED_Y_SIZE) + Y_OFFSET) }}, \
            {_PORTA, PORTA_BIT6,  {((15 * (LED_X_SIZE + LED_X_SPACE)) + X_OFFSET), (0   + Y_OFFSET), ((16 * LED_X_SIZE) + (15 * LED_X_SPACE) + X_OFFSET), ((1 * LED_Y_SIZE) + Y_OFFSET) }}, \
            {_PORTD, PORTD_BIT4,  {((1 * (LED_X_SIZE + LED_X_SPACE)) + X_OFFSET), (LED_Y_SIZE + LED_Y_SPACE + Y_OFFSET), ((2 * LED_X_SIZE) + (1 * LED_X_SPACE) + X_OFFSET), ((2 * LED_Y_SIZE) + LED_Y_SPACE + Y_OFFSET) }}, \
            {_PORTD, PORTD_BIT5,  {((2 * (LED_X_SIZE + LED_X_SPACE)) + X_OFFSET), (LED_Y_SIZE + LED_Y_SPACE + Y_OFFSET), ((3 * LED_X_SIZE) + (2 * LED_X_SPACE) + X_OFFSET), ((2 * LED_Y_SIZE) + LED_Y_SPACE + Y_OFFSET) }}, \
            {_PORTD, PORTD_BIT2,  {((3 * (LED_X_SIZE + LED_X_SPACE)) + X_OFFSET), (LED_Y_SIZE + LED_Y_SPACE + Y_OFFSET), ((4 * LED_X_SIZE) + (3 * LED_X_SPACE) + X_OFFSET), ((2 * LED_Y_SIZE) + LED_Y_SPACE + Y_OFFSET) }}, \
            {_PORTD, PORTD_BIT0,  {((4 * (LED_X_SIZE + LED_X_SPACE)) + X_OFFSET), (LED_Y_SIZE + LED_Y_SPACE + Y_OFFSET), ((5 * LED_X_SIZE) + (4 * LED_X_SPACE) + X_OFFSET), ((2 * LED_Y_SIZE) + LED_Y_SPACE + Y_OFFSET) }}, \
            {_PORTD, PORTD_BIT3,  {((5 * (LED_X_SIZE + LED_X_SPACE)) + X_OFFSET), (LED_Y_SIZE + LED_Y_SPACE + Y_OFFSET), ((6 * LED_X_SIZE) + (5 * LED_X_SPACE) + X_OFFSET), ((2 * LED_Y_SIZE) + LED_Y_SPACE + Y_OFFSET) }}, \
            {_PORTC, PORTC_BIT0,  {((6 * (LED_X_SIZE + LED_X_SPACE)) + X_OFFSET), (LED_Y_SIZE + LED_Y_SPACE + Y_OFFSET), ((7 * LED_X_SIZE) + (6 * LED_X_SPACE) + X_OFFSET), ((2 * LED_Y_SIZE) + LED_Y_SPACE + Y_OFFSET) }}, \
            {_PORTC, PORTC_BIT7,  {((7 * (LED_X_SIZE + LED_X_SPACE)) + X_OFFSET), (LED_Y_SIZE + LED_Y_SPACE + Y_OFFSET), ((8 * LED_X_SIZE) + (7 * LED_X_SPACE) + X_OFFSET), ((2 * LED_Y_SIZE) + LED_Y_SPACE + Y_OFFSET) }}, \
            {_PORTC, PORTC_BIT6,  {((9 * (LED_X_SIZE + LED_X_SPACE)) + X_OFFSET), (LED_Y_SIZE + LED_Y_SPACE + Y_OFFSET), ((10 * LED_X_SIZE) + (9 * LED_X_SPACE) + X_OFFSET), ((2 * LED_Y_SIZE) + LED_Y_SPACE + Y_OFFSET) }}, \
            {_PORTC, PORTC_BIT5,  {((10 * (LED_X_SIZE + LED_X_SPACE)) + X_OFFSET), (LED_Y_SIZE + LED_Y_SPACE + Y_OFFSET), ((11 * LED_X_SIZE) + (10 * LED_X_SPACE) + X_OFFSET), ((2 * LED_Y_SIZE) + LED_Y_SPACE + Y_OFFSET) }}, \
            {_PORTA, PORTA_BIT7,  {((12 * (LED_X_SIZE + LED_X_SPACE)) + X_OFFSET), (LED_Y_SIZE + LED_Y_SPACE + Y_OFFSET), ((13 * LED_X_SIZE) + (12 * LED_X_SPACE) + X_OFFSET), ((2 * LED_Y_SIZE) + LED_Y_SPACE + Y_OFFSET) }}, \
            {_PORTA, PORTA_BIT3,  {((13 * (LED_X_SIZE + LED_X_SPACE)) + X_OFFSET), (LED_Y_SIZE + LED_Y_SPACE + Y_OFFSET), ((14 * LED_X_SIZE) + (13 * LED_X_SPACE) + X_OFFSET), ((2 * LED_Y_SIZE) + LED_Y_SPACE + Y_OFFSET) }}, \
            {_PORTA, PORTA_BIT5,  {((14 * (LED_X_SIZE + LED_X_SPACE)) + X_OFFSET), (LED_Y_SIZE + LED_Y_SPACE + Y_OFFSET), ((15 * LED_X_SIZE) + (14 * LED_X_SPACE) + X_OFFSET), ((2 * LED_Y_SIZE) + LED_Y_SPACE + Y_OFFSET) }}, \
            {_PORTB, PORTB_BIT7,  {((15 * (LED_X_SIZE + LED_X_SPACE)) + X_OFFSET), (LED_Y_SIZE + LED_Y_SPACE + Y_OFFSET), ((16 * LED_X_SIZE) + (15 * LED_X_SPACE) + X_OFFSET), ((2 * LED_Y_SIZE) + LED_Y_SPACE + Y_OFFSET) }}, \
            {_PORTA, PORTA_BIT24, {((X_OFFSET - 62) + (1 * LED_X_SIZE) + (1 * LED_X_SPACE)), (Y_OFFSET + 255), ((X_OFFSET - 62) + (2 * LED_X_SIZE) + (1 * LED_X_SPACE)), (Y_OFFSET + LED_Y_SIZE + 255) }}, \
            {_PORTA, PORTA_BIT25, {((X_OFFSET - 62) + (2 * LED_X_SIZE) + (2 * LED_X_SPACE)), (Y_OFFSET + 255), ((X_OFFSET - 62) + (3 * LED_X_SIZE) + (2 * LED_X_SPACE)), (Y_OFFSET + LED_Y_SIZE + 255) }}, \
            {_PORTA, PORTA_BIT26, {((X_OFFSET - 62) + (3 * LED_X_SIZE) + (3 * LED_X_SPACE)), (Y_OFFSET + 255), ((X_OFFSET - 62) + (4 * LED_X_SIZE) + (3 * LED_X_SPACE)), (Y_OFFSET + LED_Y_SIZE + 255) }}, \
            {_PORTA, PORTA_BIT27, {((X_OFFSET - 62) + (4 * LED_X_SIZE) + (4 * LED_X_SPACE)), (Y_OFFSET + 255), ((X_OFFSET - 62) + (5 * LED_X_SIZE) + (4 * LED_X_SPACE)), (Y_OFFSET + LED_Y_SIZE + 255) }}, \
            {_PORTA, PORTA_BIT28, {((X_OFFSET - 62) + (5 * LED_X_SIZE) + (5 * LED_X_SPACE)), (Y_OFFSET + 255), ((X_OFFSET - 62) + (6 * LED_X_SIZE) + (5 * LED_X_SPACE)), (Y_OFFSET + LED_Y_SIZE + 255) }}, \
            {_PORTE, PORTE_BIT0,  {((X_OFFSET - 62) + (6 * LED_X_SIZE) + (6 * LED_X_SPACE)), (Y_OFFSET + 255), ((X_OFFSET - 62) + (7 * LED_X_SIZE) + (6 * LED_X_SPACE)), (Y_OFFSET + LED_Y_SIZE + 255) }}, \
            {_PORTE, PORTE_BIT1,  {((X_OFFSET - 62) + (7 * LED_X_SIZE) + (7 * LED_X_SPACE)), (Y_OFFSET + 255), ((X_OFFSET - 62) + (8 * LED_X_SIZE) + (7 * LED_X_SPACE)), (Y_OFFSET + LED_Y_SIZE + 255) }}, \
            {_PORTC, PORTC_BIT8,  {((X_OFFSET - 62) + (8 * LED_X_SIZE) + (8 * LED_X_SPACE)), (Y_OFFSET + 255), ((X_OFFSET - 62) + (9 * LED_X_SIZE) + (8 * LED_X_SPACE)), (Y_OFFSET + LED_Y_SIZE + 255) }}, \
            {_PORTC, PORTC_BIT9,  {((X_OFFSET - 62) + (9 * LED_X_SIZE) + (9 * LED_X_SPACE)), (Y_OFFSET + 255), ((X_OFFSET - 62) + (10 * LED_X_SIZE) + (9 * LED_X_SPACE)), (Y_OFFSET + LED_Y_SIZE + 255) }}, \
            {_PORTC, PORTC_BIT10, {((X_OFFSET - 62) + (10 * LED_X_SIZE) + (10 * LED_X_SPACE)), (Y_OFFSET + 255), ((X_OFFSET - 62) + (11 * LED_X_SIZE) + (10 * LED_X_SPACE)), (Y_OFFSET + LED_Y_SIZE + 255) }}, \
            {_PORTC, PORTC_BIT11, {((X_OFFSET - 62) + (11 * LED_X_SIZE) + (11 * LED_X_SPACE)), (Y_OFFSET + 255), ((X_OFFSET - 62) + (12 * LED_X_SIZE) + (11 * LED_X_SPACE)), (Y_OFFSET + LED_Y_SIZE + 255) }}, \
            _I2C_INPUTS

#elif defined NET_K60                                                    // {16}
    #define DEMO_LED_1             (PORTD_BIT11)                         // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define DEMO_LED_2             (PORTC_BIT14)                         // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define DEMO_LED_3             (PORTD_BIT0)                          // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define DEMO_LED_4             (PORTC_BIT13)                         // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too

    #define K60_LED_GREEN          (DEMO_LED_1)                          // LED green
    #define K60_LED_YELLOW         (DEMO_LED_2)                          // LED yellow
    #define K60_LED_RED            (DEMO_LED_3)                          // LED red
  //#define SWITCH_1               (PORTB_BIT16)                         // not connected    

    #define INIT_3A4IO_RESET()     _CONFIG_DRIVE_PORT_OUTPUT_VALUE(A, (PORTA_BIT19), (~PORTA_BIT19), (PORT_SRE_SLOW))  // PA19 is the ADuC RESET  (X4 pin 21)
    #define CLEAR_3A4IO_RESET()    _SETBITS(A, PORTA_BIT19)   

    #define INIT_PHY_RESET()       _CONFIG_DRIVE_PORT_OUTPUT_VALUE(A, (PORTA_BIT25), (PORTA_BIT25), (PORT_ODE | PORT_SRE_SLOW | PORT_DSE_HIGH)) // PA25 is the PHY-/RESET 
    #define PHY_RESET_LOW()        _CLEARBITS(A, PORTA_BIT25)   
    #define PHY_RESET_HIGH()       _SETBITS(A, PORTA_BIT25)   
    #define PHY_RESET_20MS         INIT_PHY_RESET(); PHY_RESET_LOW(); fnDelayLoop((unsigned long)((float)20000 * (float)((float)25000000/(float)SYSTEM_CLOCK))); PHY_RESET_HIGH(); // 20ms PHY Reset (during this ext. CPU clock is not available) !

    #define INIT_WATCHDOG_LED()    _CONFIG_DRIVE_PORT_OUTPUT_VALUE(D, (K60_LED_GREEN), (K60_LED_GREEN), (PORT_ODE | PORT_SRE_SLOW | PORT_DSE_HIGH)); // system blink

    #define INIT_WATCHDOG_DISABLE() 
    #define WATCHDOG_DISABLE()     (0)  
    #define ACTIVATE_WATCHDOG()    UNLOCK_WDOG(); WDOG_TOVALL = (2000/5); WDOG_TOVALH = 0; WDOG_STCTRLH = (WDOG_STCTRLH_STNDBYEN | WDOG_STCTRLH_WAITEN | WDOG_STCTRLH_STOPEN | WDOG_STCTRLH_WDOGEN) // watchdog enabled to generate reset on 2s timeout (no further updates allowed)
    #define TOGGLE_WATCHDOG_LED()  _TOGGLE_PORT(D, K60_LED_GREEN)

    #define CONFIG_TEST_OUTPUT()                                       
    #define TOGGLE_TEST_OUTPUT()   _TOGGLE_PORT(D, K60_LED_RED)
    #define SET_TEST_OUTPUT()      _SETBITS(D, K60_LED_RED)
    #define CLEAR_TEST_OUTPUT()    _CLEARBITS(D, K60_LED_RED)

    #if defined ETH_INTERFACE 
        #define LAN_REPORT_ACTIVITY                                      // use NetworkIndicator Task for LAN-ACT LED's no direct PHY-LED output 
        #define TURN_LINK_LED_ON()           
        #define LINK_DOWN_LEDS()        
        #define ACTIVITY_LED_ON()  _CLEARBITS(C, K60_LED_YELLOW)
        #define ACTIVITY_LED_OFF() _SETBITS(C, K60_LED_YELLOW)
        #define SPEED_LED_ON()
        #define SPEED_LED_OFF()
        #define CONFIGURE_LAN_LEDS() _CONFIG_DRIVE_PORT_OUTPUT_VALUE(C, (K60_LED_YELLOW), (K60_LED_YELLOW), (PORT_ODE | PORT_SRE_SLOW | PORT_DSE_HIGH))
    #endif
    #define MAPPED_DEMO_LED_1      0                                     // dummy - not used
    #define MAPPED_DEMO_LED_2      0

    #define USER_CHIP_PACKAGE      "KBED_K60//uTasker-embedded-control.bmp" // user defined chip image
        #define ETH_LED_FRAME_LEFT   337                                 // define the Ethernet LED frame location
        #define ETH_LED_FRAME_RIGHT  372
        #define ETH_LED_FRAME_TOP    192
        #define ETH_LED_FRAME_BOTTOM 200
        #define ETH_LED_WIDTH        10                                  // width of each LED

        #define USB_SYMBOL_LEFT      294                                 // define the USB symbol location and size
        #define USB_SYMBOL_TOP       231
        #define USB_SYMBOL_RIGHT     346
        #define USB_SYMBOL_BOTTOM    251
        #define USB_CIRCLE_RADIUS    4

    #define KEYPAD                 "KBED_K60//ethernode.bmp"             // board image
        // '0'          '1'             input state   center (x,   y)   0 = circle, radius, controlling port, controlling pin 
    #define KEYPAD_LED_DEFINITIONS  \
        {RGB(40,40,40), RGB(255,0,0),   0, {49, 65, 0, 3 }, _PORTD, PORTD_BIT0}, \
        {RGB(40,40,40), RGB(255,255,0), 0, {99, 65, 0, 3 }, _PORTC, PORTC_BIT14},\
        {RGB(40,40,40), RGB(0,255,0),   0, {148,65, 0, 3 }, _PORTD, PORTD_BIT11},

                                    // column, row, key press rectangle
    #define KEYPAD_KEY_DEFINITIONS  {0, 0, {104, 101, 129, 132}}, \
                                    {0, 1, {58,  101, 77,  132}}, \
                                    {0, 2, {9,   101, 32,  132}}, \
                                    {0, 3, {152, 101, 175, 132}}, \
                                    {1, 0, {228, 9,   249, 39 }}, \
                                    {1, 1, {202, 57,  223, 84 }}, \
                                    {1, 2, {249, 57,  270, 84 }}, \
                                    {1, 3, {226, 105, 248, 130}},
#elif defined TWR_K65F180M
    #define LED_GREEN          (PORTA_BIT28)                             // green LED - if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define LED_BLUE           (PORTA_BIT29)                             // green LED - if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define LED_YELLOW         (PORTB_BIT5)                              // orange LED - if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define LED_ORANGE         (PORTB_BIT4)                              // yellow LED - if the port is changed (eg. A to B) the port macros will require appropriate adjustment too

    #define SWITCH_2           (PORTA_BIT4)                              // switch 2 - if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define SWITCH_3           (PORTA_BIT10)                             // switch 3 - if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define SDCARD_DETECT      (PORTA_BIT9)

    #define BLINK_LED          (LED_GREEN)

    #define DEMO_LED_1         (LED_GREEN)
    #define DEMO_LED_2         (LED_BLUE)
    #define DEMO_LED_3         (LED_YELLOW)
    #define DEMO_LED_4         (LED_ORANGE)

    #define SHIFT_DEMO_LED_1    28                                       // since the port bits may be spread out shift each to the lowest 2 bits
    #define SHIFT_DEMO_LED_2    28
    #define SHIFT_DEMO_LED_3    3
    #define SHIFT_DEMO_LED_4    3

    #if defined SDCARD_SUPPORT
        #define INIT_WATCHDOG_LED() _CONFIG_PORT_INPUT(A, (SDCARD_DETECT), PORT_PS_UP_ENABLE);
    #elif !defined USE_MAINTENANCE
        #define INIT_WATCHDOG_LED() _CONFIG_DRIVE_PORT_OUTPUT_VALUE(A, (BLINK_LED), (BLINK_LED), (PORT_SRE_SLOW | PORT_DSE_HIGH))
    #else
        #define INIT_WATCHDOG_LED()                                  // configured according to user parameters
    #endif
    #define INIT_WATCHDOG_DISABLE() _CONFIG_PORT_INPUT_FAST_LOW(A, (SWITCH_2 | SWITCH_3), PORT_PS_UP_ENABLE) // configure as input
    #define WATCHDOG_DISABLE()     (_READ_PORT_MASK(A, SWITCH_3) == 0)   // pull this input down to disable watchdog (hold SW3 at reset)

    #define TOGGLE_WATCHDOG_LED()   _TOGGLE_PORT(A, BLINK_LED)
    #define CONFIG_TEST_OUTPUT()                                         // we use DEMO_LED_2 which is configured by the user code (and can be disabled in parameters if required)
    #define TOGGLE_TEST_OUTPUT()    _TOGGLE_PORT(B, DEMO_LED_2)
    #define SET_TEST_OUTPUT()       _SETBITS(B, DEMO_LED_2)
    #define CLEAR_TEST_OUTPUT()     _CLEARBITS(B, DEMO_LED_2)
 
    #define MAPPED_DEMO_LED_1       (DEMO_LED_1 >> SHIFT_DEMO_LED_1)
    #define MAPPED_DEMO_LED_2       (DEMO_LED_2 >> SHIFT_DEMO_LED_2)
    #define MAPPED_DEMO_LED_3       (DEMO_LED_3 >> SHIFT_DEMO_LED_3)
    #define MAPPED_DEMO_LED_4       (DEMO_LED_4 >> SHIFT_DEMO_LED_4)

    #define ACTIVATE_WATCHDOG()     UNLOCK_WDOG(); WDOG_TOVALL = (2000/5); WDOG_TOVALH = 0; WDOG_STCTRLH = (WDOG_STCTRLH_STNDBYEN | WDOG_STCTRLH_WAITEN | WDOG_STCTRLH_STOPEN | WDOG_STCTRLH_WDOGEN) // watchdog enabled to generate reset on 2s timeout (no further updates allowed)

    #define SD_CONTROLLER_AVAILABLE                                      // use SDHC controller rather than SPI
    #if defined SD_CONTROLLER_AVAILABLE
        #define SET_SD_CS_HIGH()                                         // dummy with SDHC controller
        #define SET_SD_CS_LOW()                                          // dummy with SDHC controller

        #ifdef _WINDOWS
            #define POWER_UP_SD_CARD()  SDHC_SYSCTL |= SDHC_SYSCTL_INITA; SDHC_SYSCTL &= ~SDHC_SYSCTL_INITA; // apply power to the SD card if appropriate (we use this to send 80 clocks - self-clearing bit)
        #else
            #define POWER_UP_SD_CARD()  SDHC_SYSCTL |= SDHC_SYSCTL_INITA; while (SDHC_SYSCTL & SDHC_SYSCTL_INITA) {}; // apply power to the SD card if appropriate (we use this to send 80 clocks)
        #endif

        #define SDHC_SYSCTL_SPEED_SLOW  (SDHC_SYSCTL_SDCLKFS_64 | SDHC_SYSCTL_DVS_5) // 375kHz when 120MHz clock
        #define SDHC_SYSCTL_SPEED_FAST  (SDHC_SYSCTL_SDCLKFS_2 | SDHC_SYSCTL_DVS_3) // 20MHz when 120MHz clock
        #define SET_SPI_SD_INTERFACE_FULL_SPEED() fnSetSD_clock(SDHC_SYSCTL_SPEED_FAST); SDHC_PROCTL |= SDHC_PROCTL_DTW_4BIT
    #else
        #define SPI1_CS0             PORTE_BIT4
        #define INITIALISE_SPI_SD_INTERFACE() POWER_UP(6, SIM_SCGC6_SPI1); \
            _CONFIG_PERIPHERAL(E, 2, PE_2_SPI1_SCK); \
            _CONFIG_PERIPHERAL(E, 3, (PE_3_SPI1_SOUT | PORT_SRE_FAST | PORT_DSE_HIGH)); \
            _CONFIG_PERIPHERAL(E, 1, (PE_1_SPI1_SIN | PORT_PS_UP_ENABLE)); \
            _CONFIG_DRIVE_PORT_OUTPUT_VALUE(E, SPI1_CS0, SPI1_CS0, (PORT_SRE_FAST | PORT_DSE_HIGH)); \
            SPI1_CTAR0 = (SPI_CTAR_ASC_6 | SPI_CTAR_FMSZ_8 | SPI_CTAR_CPHA | SPI_CTAR_CPOL | SPI_CTAR_BR_128); \
            SPI1_MCR = (SPI_MCR_DIS_TXF | SPI_MCR_DIS_RXF | SPI_MCR_MSTR | SPI_MCR_DCONF_SPI | SPI_MCR_CLR_RXF | SPI_MCR_CLR_TXF | SPI_MCR_PCSIS_CS0 | SPI_MCR_PCSIS_CS1 | SPI_MCR_PCSIS_CS2 | SPI_MCR_PCSIS_CS3 | SPI_MCR_PCSIS_CS4 | SPI_MCR_PCSIS_CS5)

        #define SET_SD_DI_CS_HIGH()  _SETBITS(E, SPI1_CS0)               // force DI and CS lines high ready for the initialisation sequence
        #define SET_SD_CS_LOW()      _CLEARBITS(E, SPI1_CS0)             // assert the CS line of the SD card to be read
        #define SET_SD_CS_HIGH()     _SETBITS(E, SPI1_CS0)               // negate the CS line of the SD card to be read
        #define SET_SD_CARD_MODE()
        #define ENABLE_SPI_SD_OPERATION()

        // Set maximum speed
        //
        #define SET_SPI_SD_INTERFACE_FULL_SPEED() SPI1_MCR |= SPI_MCR_HALT; SPI1_CTAR0 = (SPI_CTAR_FMSZ_8 | SPI_CTAR_CPOL | SPI_CTAR_CPHA | SPI_CTAR_BR_2); SPI1_MCR &= ~SPI_MCR_HALT;
        #if defined _WINDOWS
            #define WRITE_SPI_CMD(byte)    SPI1_SR &= ~(SPI_SR_RFDF); SPI1_PUSHR = (byte | SPI_PUSHR_PCS_NONE | SPI_PUSHR_CTAS_CTAR0); SPI1_POPR = _fnSimSD_write((unsigned char)byte)
            #define WAIT_TRANSMISSON_END() while ((SPI1_SR & (SPI_SR_RFDF)) == 0) { SPI1_SR |= (SPI_SR_RFDF); }
            #define READ_SPI_DATA()        (unsigned char)SPI1_POPR
        #else
            #define WRITE_SPI_CMD(byte)    SPI1_SR = (SPI_SR_RFDF); SPI1_PUSHR = (byte | SPI_PUSHR_PCS_NONE | SPI_PUSHR_CTAS_CTAR0) // clear flags before transmitting (and receiving) a single byte
            #define WAIT_TRANSMISSON_END() while ((SPI1_SR & (SPI_SR_RFDF)) == 0) {}
            #define READ_SPI_DATA()        (unsigned char)SPI1_POPR
        #endif
        #define POWER_UP_SD_CARD()                                       // apply power to the SD card if appropriate
    #endif

    #define POWER_DOWN_SD_CARD()                                         // remove power from SD card interface

    #define GET_SDCARD_WP_STATE()   0                                    // no card protection switch available
    #define SDCARD_DETECT_INPUT_INTERRUPT                                // since the board has a card detection switch we use this in interrupt mode
    #define SDCARD_DETECTION()      (_READ_PORT_MASK(A, SDCARD_DETECT) == 0) // card detection input
    #define PRIORITY_SDCARD_DETECT_PORT_INT   PRIORITY_PORT_A_INT        // port priority when using card detect switch interrupt
    #define SDCARD_DETECT_PORT      PORTA                                // interrupt is on this port
    #define SDCARD_DETECT_PIN       SDCARD_DETECT                        // interrupt pin
    #define SDCARD_CONFIG_COMPLETE

    #define BUTTON_KEY_DEFINITIONS  {_PORTA, SWITCH_2,   {31, 251,  49, 268 }}, \
                                    {_PORTA, SWITCH_3,   {31, 220,  49, 239 }},

    #define KEYPAD "KeyPads/TWR_K65F180M.bmp"

        // '0'          '1'           input state   center (x,   y)   0 = circle, radius, controlling port, controlling pin 
    #define KEYPAD_LED_DEFINITIONS  \
        {RGB(0,  255,  0), RGB(0,0,0),  1, {434, 128, 443, 136 }, _PORTA, LED_GREEN}, \
        {RGB(0,  0,  255), RGB(0,0,0),  1, {434, 173, 443, 181 }, _PORTA, LED_BLUE}, \
        {RGB(255,255,  0), RGB(0,0,0),  1, {193, 274, 201, 279 }, _PORTB, LED_ORANGE}, \
        {RGB(128,128,  0), RGB(0,0,0),  1, {193, 283, 201, 290 }, _PORTB, LED_YELLOW}

    #define CONFIGURE_MOUSE_INPUTS() 
    #define MOUSE_LEFT_CLICK()     (_READ_PORT_MASK(A, SWITCH_3) == 0)   // press this button to move mouse up
    #define MOUSE_UP()             0                                     // not used
    #define MOUSE_DOWN()           0                                     // not used
    #define MOUSE_LEFT()           0                                     // not used
    #define MOUSE_RIGHT()          0                                     // not used
#elif defined FRDM_K66F
    #define LED_GREEN          (PORTE_BIT6)                              // green LED - if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define LED_RED            (PORTC_BIT9)                              // red LED - if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define LED_BLUE           (PORTA_BIT11)                             // blue LED - if the port is changed (eg. A to B) the port macros will require appropriate adjustment too

    #define SWITCH_2           (PORTD_BIT11)                             // switch 2 - if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define SWITCH_3           (PORTA_BIT10)                             // switch 3 - if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define SDCARD_DETECT      (PORTD_BIT10)

    #define BLINK_LED          (LED_GREEN)

    #define DEMO_LED_1         (LED_GREEN)
    #define DEMO_LED_2         (LED_RED)
    #define DEMO_LED_3         (LED_BLUE)
    #define DEMO_LED_4         (PORTA_BIT12)

    #define SHIFT_DEMO_LED_1    6                                        // since the port bits may be spread out shift each to the lowest 2 bits
    #define SHIFT_DEMO_LED_2    8
    #define SHIFT_DEMO_LED_3    9
    #define SHIFT_DEMO_LED_4    9

    #if !defined USE_MAINTENANCE
        #define INIT_WATCHDOG_LED() _CONFIG_DRIVE_PORT_OUTPUT_VALUE(E, (BLINK_LED), (BLINK_LED), (PORT_SRE_SLOW | PORT_DSE_HIGH))
    #else
        #define INIT_WATCHDOG_LED()                                      // configured according to user parameters
    #endif
    #define INIT_WATCHDOG_DISABLE() _CONFIG_PORT_INPUT_FAST_LOW(D, (SWITCH_2), PORT_PS_UP_ENABLE); _CONFIG_PORT_INPUT_FAST_LOW(A, (SWITCH_3), PORT_PS_UP_ENABLE) // configure as input
    #define WATCHDOG_DISABLE()     (_READ_PORT_MASK(A, SWITCH_3) == 0)   // pull this input down to disable watchdog (hold SW3 at reset)

    #define TOGGLE_WATCHDOG_LED()   _TOGGLE_PORT(E, BLINK_LED)
    #define CONFIG_TEST_OUTPUT()                                         // we use DEMO_LED_2 which is configured by the user code (and can be disabled in parameters if required)
    #define TOGGLE_TEST_OUTPUT()    _TOGGLE_PORT(C, DEMO_LED_2)
    #define SET_TEST_OUTPUT()       _SETBITS(C, DEMO_LED_2)
    #define CLEAR_TEST_OUTPUT()     _CLEARBITS(C, DEMO_LED_2)
 
    #define MAPPED_DEMO_LED_1       (DEMO_LED_1 >> SHIFT_DEMO_LED_1)
    #define MAPPED_DEMO_LED_2       (DEMO_LED_2 >> SHIFT_DEMO_LED_2)
    #define MAPPED_DEMO_LED_3       (DEMO_LED_3 >> SHIFT_DEMO_LED_3)
    #define MAPPED_DEMO_LED_4       (DEMO_LED_4 >> SHIFT_DEMO_LED_4)

    #define ACTIVATE_WATCHDOG()     UNLOCK_WDOG(); WDOG_TOVALL = (2000/5); WDOG_TOVALH = 0; WDOG_STCTRLH = (WDOG_STCTRLH_STNDBYEN | WDOG_STCTRLH_WAITEN | WDOG_STCTRLH_STOPEN | WDOG_STCTRLH_WDOGEN) // watchdog enabled to generate reset on 2s timeout (no further updates allowed)

    #define SD_CONTROLLER_AVAILABLE                                      // use SDHC controller rather than SPI
    #if defined SD_CONTROLLER_AVAILABLE
        #define SET_SD_CS_HIGH()                                         // dummy with SDHC controller
        #define SET_SD_CS_LOW()                                          // dummy with SDHC controller

        #ifdef _WINDOWS
            #define POWER_UP_SD_CARD()  SDHC_SYSCTL |= SDHC_SYSCTL_INITA; SDHC_SYSCTL &= ~SDHC_SYSCTL_INITA; // apply power to the SD card if appropriate (we use this to send 80 clocks - self-clearing bit)
        #else
            #define POWER_UP_SD_CARD()  SDHC_SYSCTL |= SDHC_SYSCTL_INITA; while (SDHC_SYSCTL & SDHC_SYSCTL_INITA) {}; // apply power to the SD card if appropriate (we use this to send 80 clocks)
        #endif

        #define SDHC_SYSCTL_SPEED_SLOW  (SDHC_SYSCTL_SDCLKFS_64 | SDHC_SYSCTL_DVS_5) // 375kHz when 120MHz clock
        #define SDHC_SYSCTL_SPEED_FAST  (SDHC_SYSCTL_SDCLKFS_2 | SDHC_SYSCTL_DVS_3) // 20MHz when 120MHz clock
        #define SET_SPI_SD_INTERFACE_FULL_SPEED() fnSetSD_clock(SDHC_SYSCTL_SPEED_FAST); SDHC_PROCTL |= SDHC_PROCTL_DTW_4BIT
    #endif

    #define POWER_DOWN_SD_CARD()                                         // remove power from SD card interface

    #define GET_SDCARD_WP_STATE()   0                                    // no card protection switch available
    #define SDCARD_DETECT_INPUT_INTERRUPT                                // since the board has a card detection switch we use this in interrupt mode
    #define SDCARD_DETECTION()      (_READ_PORT_MASK(D, SDCARD_DETECT) == 0) // card detection input
    #define PRIORITY_SDCARD_DETECT_PORT_INT   PRIORITY_PORT_D_INT        // port priority when using card detect switch interrupt
    #define SDCARD_DETECT_PORT      PORTD                                // interrupt is on this port
    #define SDCARD_DETECT_PIN       SDCARD_DETECT                        // interrupt pin
    #define SDCARD_CONFIG_COMPLETE

    #define BUTTON_KEY_DEFINITIONS  {_PORTD, SWITCH_2,   {286,   6, 299,  14 }}, \
                                    {_PORTA, SWITCH_3,   {286, 183, 299, 190 }},

    #define KEYPAD "KeyPads/FRDM_K66F.bmp"

    #define MULTICOLOUR_LEDS        {0, 2}                               // single LED made up of entries 0, 1 and 2

        // '0'          '1'           input state   center (x,   y)   0 = circle, radius, controlling port, controlling pin 
    #define KEYPAD_LED_DEFINITIONS  \
        {RGB(0,  255,0  ), RGB(0,0,0),  1, {316, 10, 0, 5}, _PORTE, LED_GREEN}, \
        {RGB(255,0,  0  ), RGB(0,0,0),  1, {316, 10, 0, 5}, _PORTC, LED_RED}, \
        {RGB(0,  0,  255), RGB(0,0,0),  1, {316, 10, 0, 5}, _PORTA, LED_BLUE},

    #define CONFIGURE_MOUSE_INPUTS() 
    #define MOUSE_LEFT_CLICK()     (_READ_PORT_MASK(A, SWITCH_2) == 0)   // press this button to move mouse up
    #define MOUSE_UP()             0                                     // not used
    #define MOUSE_DOWN()           0                                     // not used
    #define MOUSE_LEFT()           0                                     // not used
    #define MOUSE_RIGHT()          0                                     // not used
#elif defined TEENSY_3_5 || defined TEENSY_3_6
    #define LED_RED            (PORTC_BIT5)                              // red LED - if the port is changed (eg. A to B) the port macros will require appropriate adjustment too

    #define SWITCH_1           (PORTC_BIT1)                              // switch 1 [pin pad 22] - if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define SWITCH_2           (PORTC_BIT2)                              // switch 2 [pin pad 23] - if the port is changed (eg. A to B) the port macros will require appropriate adjustment too

    #define BLINK_LED          (LED_RED)

    #define DEMO_LED_1         (LED_RED)
    #define DEMO_LED_2         (PORTC_BIT6)
    #define DEMO_LED_3         (PORTC_BIT7)
    #define DEMO_LED_4         (PORTC_BIT8)

    #define SHIFT_DEMO_LED_1    5                                       // since the port bits may be spread out shift each to the lowest 2 bits
    #define SHIFT_DEMO_LED_2    5
    #define SHIFT_DEMO_LED_3    5
    #define SHIFT_DEMO_LED_4    5

    #if !defined USE_MAINTENANCE
        #define INIT_WATCHDOG_LED() _CONFIG_DRIVE_PORT_OUTPUT_VALUE(C, (BLINK_LED), (BLINK_LED), (PORT_SRE_SLOW | PORT_DSE_HIGH))
    #else
        #define INIT_WATCHDOG_LED()                                      // configured according to user parameters
    #endif
    #define INIT_WATCHDOG_DISABLE() _CONFIG_PORT_INPUT_FAST_LOW(C, (SWITCH_1 | SWITCH_2), PORT_PS_UP_ENABLE) // configure as input
    #define WATCHDOG_DISABLE()     (_READ_PORT_MASK(C, SWITCH_2) == 0)   // pull this input down to disable watchdog (connect pin pad 23 to GND at reset)

    #define TOGGLE_WATCHDOG_LED()   _TOGGLE_PORT(C, BLINK_LED)
    #define CONFIG_TEST_OUTPUT()                                         // we use DEMO_LED_2 which is configured by the user code (and can be disabled in parameters if required)
    #define TOGGLE_TEST_OUTPUT()    _TOGGLE_PORT(C, DEMO_LED_2)
    #define SET_TEST_OUTPUT()       _SETBITS(C, DEMO_LED_2)
    #define CLEAR_TEST_OUTPUT()     _CLEARBITS(C, DEMO_LED_2)
 
    #define MAPPED_DEMO_LED_1       (DEMO_LED_1 >> SHIFT_DEMO_LED_1)
    #define MAPPED_DEMO_LED_2       (DEMO_LED_2 >> SHIFT_DEMO_LED_2)
    #define MAPPED_DEMO_LED_3       (DEMO_LED_3 >> SHIFT_DEMO_LED_3)
    #define MAPPED_DEMO_LED_4       (DEMO_LED_4 >> SHIFT_DEMO_LED_4)

    #define ACTIVATE_WATCHDOG()     UNLOCK_WDOG(); WDOG_TOVALL = (2000/5); WDOG_TOVALH = 0; WDOG_STCTRLH = (WDOG_STCTRLH_STNDBYEN | WDOG_STCTRLH_WAITEN | WDOG_STCTRLH_STOPEN | WDOG_STCTRLH_WDOGEN) // watchdog enabled to generate reset on 2s timeout (no further updates allowed)

    #define SD_CONTROLLER_AVAILABLE                                      // use SDHC controller rather than SPI
    #if defined SD_CONTROLLER_AVAILABLE
        #define SET_SD_CS_HIGH()                                         // dummy with SDHC controller
        #define SET_SD_CS_LOW()                                          // dummy with SDHC controller

        #ifdef _WINDOWS
            #define POWER_UP_SD_CARD()  SDHC_SYSCTL |= SDHC_SYSCTL_INITA; SDHC_SYSCTL &= ~SDHC_SYSCTL_INITA; // apply power to the SD card if appropriate (we use this to send 80 clocks - self-clearing bit)
        #else
            #define POWER_UP_SD_CARD()  SDHC_SYSCTL |= SDHC_SYSCTL_INITA; while (SDHC_SYSCTL & SDHC_SYSCTL_INITA) {}; // apply power to the SD card if appropriate (we use this to send 80 clocks)
        #endif

        #define SDHC_SYSCTL_SPEED_SLOW  (SDHC_SYSCTL_SDCLKFS_64 | SDHC_SYSCTL_DVS_5) // 375kHz when 120MHz clock
        #define SDHC_SYSCTL_SPEED_FAST  (SDHC_SYSCTL_SDCLKFS_2 | SDHC_SYSCTL_DVS_3) // 20MHz when 120MHz clock
        #define SET_SPI_SD_INTERFACE_FULL_SPEED() fnSetSD_clock(SDHC_SYSCTL_SPEED_FAST); SDHC_PROCTL |= SDHC_PROCTL_DTW_4BIT
    #endif

    #define POWER_DOWN_SD_CARD()                                         // remove power from SD card interface

    #define GET_SDCARD_WP_STATE()   0                                    // no card protection switch available
    #define SDCARD_CONFIG_COMPLETE

    #define BUTTON_KEY_DEFINITIONS  {_PORTC, SWITCH_1,   {31, 251,  49, 268 }}, \
                                    {_PORTC, SWITCH_2,   {31, 220,  49, 239 }},

    #if defined TEENSY_3_6
        #define KEYPAD "KeyPads/TEENSY3.6.bmp"
    #else
        #define KEYPAD "KeyPads/TEENSY3.5.bmp"
    #endif

        // '0'          '1'           input state   center (x,   y)   0 = circle, radius, controlling port, controlling pin 
    #define KEYPAD_LED_DEFINITIONS  \
        {RGB(0,  255,  0), RGB(0,0,0),  1, {364, 39, 372, 45 }, _PORTC, LED_RED}

    #define CONFIGURE_MOUSE_INPUTS() 
    #define MOUSE_LEFT_CLICK()     (_READ_PORT_MASK(C, SWITCH_1) == 0)   // press this button to move mouse up
    #define MOUSE_UP()             0                                     // not used
    #define MOUSE_DOWN()           0                                     // not used
    #define MOUSE_LEFT()           0                                     // not used
    #define MOUSE_RIGHT()          0                                     // not used
#elif defined TWR_K64F120M
    #define LED_GREEN          (PORTE_BIT6)                              // green LED - if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define LED_YELLOW         (PORTE_BIT7)                              // yellow LED - if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define LED_ORANGE         (PORTE_BIT8)                              // orange LED - if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define LED_BLUE           (PORTE_BIT9)                              // blue LED - if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define SWITCH_1           (PORTC_BIT6)                              // switch 1 - if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define SWITCH_3           (PORTA_BIT4)                              // switch 3 - if the port is changed (eg. A to B) the port macros will require appropriate adjustment too

    #define BLINK_LED          (LED_GREEN)

    #define DEMO_LED_1         (LED_GREEN)
    #define DEMO_LED_2         (LED_YELLOW)
    #define DEMO_LED_3         (LED_ORANGE)
    #define DEMO_LED_4         (LED_BLUE)

    #define SHIFT_DEMO_LED_1    6                                        // since the port bits may be spread out shift each to the lowest 2 bits
    #define SHIFT_DEMO_LED_2    6
    #define SHIFT_DEMO_LED_3    6
    #define SHIFT_DEMO_LED_4    6

    #if defined SDCARD_SUPPORT
        #define INIT_WATCHDOG_LED() _CONFIG_DRIVE_PORT_OUTPUT_VALUE(E, (BLINK_LED), (BLINK_LED), (PORT_SRE_SLOW | PORT_DSE_HIGH)); _CONFIG_PORT_INPUT(B, (SDCARD_DETECT | WRITE_PROTECT_INPUT), PORT_PS_UP_ENABLE) // note that the force boot input is configured here and not with the INIT_WATCHDOG_DISABLE() since the watchdog must be disabled as quickly as possible
    #else
        #define INIT_WATCHDOG_LED() _CONFIG_DRIVE_PORT_OUTPUT_VALUE(E, (BLINK_LED), (BLINK_LED), (PORT_SRE_SLOW | PORT_DSE_HIGH))
    #endif
    #define INIT_WATCHDOG_DISABLE() _CONFIG_PORT_INPUT_FAST_LOW(C, (SWITCH_1), PORT_PS_UP_ENABLE); _CONFIG_PORT_INPUT_FAST_LOW(A, (SWITCH_3), PORT_PS_UP_ENABLE) // configure as input

    #define WATCHDOG_DISABLE()     (_READ_PORT_MASK(A, SWITCH_3) == 0)   // pull this input down to disable watchdog (hold SW3 at reset)

    #define TOGGLE_WATCHDOG_LED()   _TOGGLE_PORT(E, BLINK_LED)

    #define SDCARD_DETECT          (PORTB_BIT20)                         // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define WRITE_PROTECT_INPUT    (PORTB_BIT21)                         // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too

    #define MAPPED_DEMO_LED_1       (DEMO_LED_1 >> SHIFT_DEMO_LED_1)
    #define MAPPED_DEMO_LED_2       (DEMO_LED_2 >> SHIFT_DEMO_LED_2)
    #define MAPPED_DEMO_LED_3       (DEMO_LED_3 >> SHIFT_DEMO_LED_3)
    #define MAPPED_DEMO_LED_4       (DEMO_LED_4 >> SHIFT_DEMO_LED_4)

    #define ACTIVATE_WATCHDOG()     UNLOCK_WDOG(); WDOG_TOVALL = (2000/5); WDOG_TOVALH = 0; WDOG_STCTRLH = (WDOG_STCTRLH_STNDBYEN | WDOG_STCTRLH_WAITEN | WDOG_STCTRLH_STOPEN | WDOG_STCTRLH_WDOGEN) // watchdog enabled to generate reset on 2s timeout (no further updates allowed)

    #define SD_CONTROLLER_AVAILABLE                                      // use SDHC controller rather than SPI
    #if defined SD_CONTROLLER_AVAILABLE
        #define SET_SD_CS_HIGH()                                         // dummy with SDHC controller
        #define SET_SD_CS_LOW()                                          // dummy with SDHC controller

        #ifdef _WINDOWS
            #define POWER_UP_SD_CARD()  SDHC_SYSCTL |= SDHC_SYSCTL_INITA; SDHC_SYSCTL &= ~SDHC_SYSCTL_INITA; // apply power to the SD card if appropriate (we use this to send 80 clocks - self-clearing bit)
        #else
            #define POWER_UP_SD_CARD()  SDHC_SYSCTL |= SDHC_SYSCTL_INITA; while (SDHC_SYSCTL & SDHC_SYSCTL_INITA) {}; // apply power to the SD card if appropriate (we use this to send 80 clocks)
        #endif

        #define SDHC_SYSCTL_SPEED_SLOW  (SDHC_SYSCTL_SDCLKFS_64 | SDHC_SYSCTL_DVS_5) // 375kHz when 120MHz clock
        #define SDHC_SYSCTL_SPEED_FAST  (SDHC_SYSCTL_SDCLKFS_2 | SDHC_SYSCTL_DVS_3) // 20MHz when 120MHz clock
        #define SET_SPI_SD_INTERFACE_FULL_SPEED() fnSetSD_clock(SDHC_SYSCTL_SPEED_FAST); SDHC_PROCTL |= SDHC_PROCTL_DTW_4BIT
    #else
        #define SPI1_CS0             PORTE_BIT4
        #define INITIALISE_SPI_SD_INTERFACE() POWER_UP(6, SIM_SCGC6_SPI1); \
            _CONFIG_PERIPHERAL(E, 2, PE_2_SPI1_SCK); \
            _CONFIG_PERIPHERAL(E, 3, (PE_3_SPI1_SOUT | PORT_SRE_FAST | PORT_DSE_HIGH)); \
            _CONFIG_PERIPHERAL(E, 1, (PE_1_SPI1_SIN | PORT_PS_UP_ENABLE)); \
            _CONFIG_DRIVE_PORT_OUTPUT_VALUE(E, SPI1_CS0, SPI1_CS0, (PORT_SRE_FAST | PORT_DSE_HIGH)); \
            SPI1_CTAR0 = (SPI_CTAR_ASC_6 | SPI_CTAR_FMSZ_8 | SPI_CTAR_CPHA | SPI_CTAR_CPOL | SPI_CTAR_BR_128); \
            SPI1_MCR = (SPI_MCR_DIS_TXF | SPI_MCR_DIS_RXF | SPI_MCR_MSTR | SPI_MCR_DCONF_SPI | SPI_MCR_CLR_RXF | SPI_MCR_CLR_TXF | SPI_MCR_PCSIS_CS0 | SPI_MCR_PCSIS_CS1 | SPI_MCR_PCSIS_CS2 | SPI_MCR_PCSIS_CS3 | SPI_MCR_PCSIS_CS4 | SPI_MCR_PCSIS_CS5)

        #define SET_SD_DI_CS_HIGH()  _SETBITS(E, SPI1_CS0)               // force DI and CS lines high ready for the initialisation sequence
        #define SET_SD_CS_LOW()      _CLEARBITS(E, SPI1_CS0)             // assert the CS line of the SD card to be read
        #define SET_SD_CS_HIGH()     _SETBITS(E, SPI1_CS0)               // negate the CS line of the SD card to be read
        #define SET_SD_CARD_MODE()
        #define ENABLE_SPI_SD_OPERATION()

        // Set maximum speed
        //
        #define SET_SPI_SD_INTERFACE_FULL_SPEED() SPI1_MCR |= SPI_MCR_HALT; SPI1_CTAR0 = (SPI_CTAR_FMSZ_8 | SPI_CTAR_CPOL | SPI_CTAR_CPHA | SPI_CTAR_BR_2); SPI1_MCR &= ~SPI_MCR_HALT;
        #if defined _WINDOWS
            #define WRITE_SPI_CMD(byte)    SPI1_SR &= ~(SPI_SR_RFDF); SPI1_PUSHR = (byte | SPI_PUSHR_PCS_NONE | SPI_PUSHR_CTAS_CTAR0); SPI1_POPR = _fnSimSD_write((unsigned char)byte)
            #define WAIT_TRANSMISSON_END() while ((SPI1_SR & (SPI_SR_RFDF)) == 0) { SPI1_SR |= (SPI_SR_RFDF); }
            #define READ_SPI_DATA()        (unsigned char)SPI1_POPR
        #else
            #define WRITE_SPI_CMD(byte)    SPI1_SR = (SPI_SR_RFDF); SPI1_PUSHR = (byte | SPI_PUSHR_PCS_NONE | SPI_PUSHR_CTAS_CTAR0) // clear flags before transmitting (and receiving) a single byte
            #define WAIT_TRANSMISSON_END() while ((SPI1_SR & (SPI_SR_RFDF)) == 0) {}
            #define READ_SPI_DATA()        (unsigned char)SPI1_POPR
        #endif
        #define POWER_UP_SD_CARD()                                       // apply power to the SD card if appropriate
    #endif

    #define POWER_DOWN_SD_CARD()                                         // remove power from SD card interface

    #define GET_SDCARD_WP_STATE()   (_READ_PORT_MASK(B, WRITE_PROTECT_INPUT) == 0) // when the input is read as '0' the card is protected from writes
    #define SDCARD_DETECT_INPUT_INTERRUPT                                // since the board has a card detection switch we use this in interrupt mode
    #define SDCARD_DETECTION()      (_READ_PORT_MASK(B, SDCARD_DETECT) == 0) // card detection input
    #define PRIORITY_SDCARD_DETECT_PORT_INT   PRIORITY_PORT_B_INT        // port priority when using card detect switch interrupt
    #define SDCARD_DETECT_PORT      PORTB                                // interrupt is on this port
    #define SDCARD_DETECT_PIN       SDCARD_DETECT                        // interrupt pin
    #define SDCARD_CONFIG_COMPLETE

    #define CONFIG_TEST_OUTPUT()                                         // we use DEMO_LED_2 which is configured by the user code (and can be disabled in parameters if required)

    #define TOGGLE_TEST_OUTPUT()    _TOGGLE_PORT(E, DEMO_LED_2)
    #define SET_TEST_OUTPUT()       _SETBITS(E, DEMO_LED_2)
    #define CLEAR_TEST_OUTPUT()     _CLEARBITS(E, DEMO_LED_2)

    #define BUTTON_KEY_DEFINITIONS  {_PORTC, SWITCH_1,   {109, 33,  129, 48 }}, \
                                    {_PORTA, SWITCH_3,   {173, 16,  193, 29}},

    #define KEYPAD "KeyPads/TWR_K64F120M.bmp"

        // '0'          '1'           input state   center (x,   y)   0 = circle, radius, controlling port, controlling pin 
    #define KEYPAD_LED_DEFINITIONS  \
        {RGB(0,  255, 0  ), RGB(0,0,0),  1, {203, 27,  0, 4 }, _PORTE, LED_GREEN}, \
        {RGB(255,255, 0  ), RGB(0,0,0),  1, {208, 25,  0, 4 }, _PORTE, LED_YELLOW}, \
        {RGB(255,128, 0  ), RGB(0,0,0),  1, {213, 23,  0, 4 }, _PORTE, LED_ORANGE}, \
        {RGB(0,  0,   255), RGB(0,0,0),  1, {218, 21,  0, 4 }, _PORTE, LED_BLUE},

    #define CONFIGURE_MOUSE_INPUTS() 
    #define MOUSE_LEFT_CLICK()     (_READ_PORT_MASK(C, SWITCH_1) == 0)   // press this button to move mouse up
    #define MOUSE_UP()             0                                     // not used
    #define MOUSE_DOWN()           0                                     // not used
    #define MOUSE_LEFT()           0                                     // not used
    #define MOUSE_RIGHT()          0                                     // not used
#elif defined FRDM_K64F || defined FreeLON                               // {26}
    #define LED_GREEN              (PORTE_BIT26)                         // green LED - if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define LED_BLUE               (PORTB_BIT21)                         // blue LED - if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define LED_RED                (PORTB_BIT22)                         // red LED - if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define SWITCH_2               (PORTC_BIT6)                          // switch 2 - if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define SWITCH_3               (PORTA_BIT4)                          // switch 3 - if the port is changed (eg. A to B) the port macros will require appropriate adjustment too

    #define DEMO_LED_1             (LED_GREEN)
    #define DEMO_LED_2             (PORTC_BIT16)
    #define DEMO_LED_3             (LED_RED)
    #define DEMO_LED_4             (LED_BLUE)

    #define BLINK_LED              (DEMO_LED_1)

    #if defined USE_MAINTENANCE
        #define INIT_WATCHDOG_LED()                                     // let the port set up do this (the user can disable blinking)
    #else
        #define INIT_WATCHDOG_LED() _CONFIG_DRIVE_PORT_OUTPUT_VALUE(E, (BLINK_LED), (BLINK_LED), (PORT_SRE_SLOW | PORT_DSE_HIGH))
    #endif

    #define INIT_WATCHDOG_DISABLE() _CONFIG_PORT_INPUT_FAST_LOW(C, SWITCH_2, PORT_PS_UP_ENABLE)
    #define WATCHDOG_DISABLE()      (_READ_PORT_MASK(C, SWITCH_2) == 0)  // pull this input down to disable watchdog (hold SW2 at reset)
    #define ACTIVATE_WATCHDOG()     UNLOCK_WDOG(); WDOG_TOVALL = (2000/5); WDOG_TOVALH = 0; WDOG_STCTRLH = (WDOG_STCTRLH_STNDBYEN | WDOG_STCTRLH_WAITEN | WDOG_STCTRLH_STOPEN | WDOG_STCTRLH_WDOGEN) // watchdog enabled to generate reset on 2s timeout (no further updates allowed)
    #define TOGGLE_WATCHDOG_LED()   _TOGGLE_PORT(E, BLINK_LED)

    #define CONFIG_TEST_OUTPUT()                                         // we use DEMO_LED_2 which is configured by the user code (and can be disabled in parameters if required)
    #define TOGGLE_TEST_OUTPUT()    _TOGGLE_PORT(C, DEMO_LED_2)
    #define SET_TEST_OUTPUT()       _SETBITS(C, DEMO_LED_2)
    #define CLEAR_TEST_OUTPUT()     _CLEARBITS(C, DEMO_LED_2)

    #define SHIFT_DEMO_LED_1        26                                   // since the port bits are spread out shift each to the lowest 2 bits
    #define SHIFT_DEMO_LED_2        15
    #define SHIFT_DEMO_LED_3        20
    #define SHIFT_DEMO_LED_4        18

    #define MAPPED_DEMO_LED_1       (DEMO_LED_1 >> SHIFT_DEMO_LED_1)
    #define MAPPED_DEMO_LED_2       (DEMO_LED_2 >> SHIFT_DEMO_LED_2)
    #define MAPPED_DEMO_LED_3       (DEMO_LED_3 >> SHIFT_DEMO_LED_3)
    #define MAPPED_DEMO_LED_4       (DEMO_LED_4 >> SHIFT_DEMO_LED_4)

    #if defined I2C_INTERFACE
        #define CONFIGURE_MOUSE_INPUTS() _CONFIG_PORT_INPUT_FAST_LOW(C, SWITCH_2, PORT_PS_UP_ENABLE)
        #define MOUSE_LEFT_CLICK()     (_READ_PORT_MASK(C, SWITCH_2) == 0)
        #define MOUSE_UP()             (iUpTilt/16)                           // use accelerometer (see iic_tests.h and TEST_MMA8451Q, based on I2C)
        #define MOUSE_DOWN()           (iDownTilt/16)
        #define MOUSE_LEFT()           (iLeftTilt/16)
        #define MOUSE_RIGHT()          (iRightTilt/16)
    #else
        #define CONFIGURE_MOUSE_INPUTS() _CONFIG_PORT_INPUT_FAST_LOW(C, SWITCH_2, PORT_PS_UP_ENABLE); _CONFIG_PORT_INPUT_FAST_LOW(A, SWITCH_3, PORT_PS_UP_ENABLE)
        #define MOUSE_LEFT_CLICK()     0
        #define MOUSE_UP()             (_READ_PORT_MASK(C, SWITCH_2) == 0) // press this button to move mouse up
        #define MOUSE_DOWN()           (_READ_PORT_MASK(A, SWITCH_3) == 0) // press this button to move mouse down
        #define MOUSE_LEFT()           0                                 // not used
        #define MOUSE_RIGHT()          0                                 // not used
    #endif

    #define USB_HOST_POWER_CONFIG()                                      // the board needs to be modified for host mode with no switchable control
    #define USB_HOST_POWER_ON()                                          // populate jumper J21 and insert the jumper to connect P5V_USB to P5V_K64_USB
    #define USB_HOST_POWER_OFF()                                         // as the circuit says "electrical protection is not provided - use it at your own risk"

    #if !defined FreeLON
    #define BUTTON_KEY_DEFINITIONS  {_PORTC, SWITCH_2,   {497, 20,  513, 30 }}, \
                                    {_PORTA, SWITCH_3,   {497, 299, 513, 309}}, \
                                    {_PORTE, PORTE_BIT24,{155, 4,   171, 17 }}, \
                                    {_PORTE, PORTE_BIT25,{171, 4,   187, 17 }}, \
                                    {_PORTD, PORTD_BIT1, {219, 4,   235, 17 }}, \
                                    {_PORTD, PORTD_BIT3, {235, 4,   251, 17 }}, \
                                    {_PORTD, PORTD_BIT2, {251, 4,   267, 17 }}, \
                                    {_PORTD, PORTD_BIT0, {267, 4,   283, 17 }}, \
                                    {_PORTC, PORTC_BIT4, {283, 4,   299, 17 }}, \
                                    {_PORTA, PORTA_BIT0, {299, 4,   315, 17 }}, \
                                    {_PORTE, PORTE_BIT26,{299, 17,  315, 30 }}, \
                                    {_PORTC, PORTC_BIT3, {318, 7,   334, 20 }}, \
                                    {_PORTC, PORTC_BIT2, {334, 7,   350, 20 }}, \
                                    {_PORTA, PORTA_BIT2, {350, 7,   366, 20 }}, \
                                    {_PORTB, PORTB_BIT23,{366, 7,   382, 20 }}, \
                                    {_PORTA, PORTA_BIT1, {382, 7,   398, 20 }}, \
                                    {_PORTB, PORTB_BIT9, {398, 7,   414, 20 }}, \
                                    {_PORTC, PORTC_BIT17,{414, 7,   430, 20 }}, \
                                    {_PORTC, PORTC_BIT16,{430, 7,   446, 20 }}, \
                                    {_PORTC, PORTC_BIT5, {318, 20,  334, 32 }}, \
                                    {_PORTC, PORTC_BIT7, {334, 20,  350, 32 }}, \
                                    {_PORTC, PORTC_BIT0, {350, 20,  366, 32 }}, \
                                    {_PORTC, PORTC_BIT9, {366, 20,  382, 32 }}, \
                                    {_PORTC, PORTC_BIT8, {382, 20,  398, 32 }}, \
                                    {_PORTC, PORTC_BIT1, {398, 20,  414, 32 }}, \
                                    {_PORTB, PORTB_BIT19,{414, 20,  430, 32 }}, \
                                    {_PORTB, PORTB_BIT18,{430, 20,  446, 32 }}, \
                                    {_PORTB, PORTB_BIT20,{418, 290, 434, 305}}, \
                                    {_PORTB, PORTB_BIT2, {354, 305, 370, 320}}, \
                                    {_PORTB, PORTB_BIT3, {370, 305, 386, 320}}, \
                                    {_PORTB, PORTB_BIT10,{386, 305, 402, 320}}, \
                                    {_PORTB, PORTB_BIT11,{402, 305, 418, 320}}, \
                                    {_PORTC, PORTC_BIT11,{418, 305, 434, 320}}, \
                                    {_PORTC, PORTC_BIT10,{435, 305, 450, 320}},


    #define MULTICOLOUR_LEDS        {0, 2}                               // single LED made up of entries 0, 1 and 2

        // '0'          '1'           input state   center (x,   y)   0 = circle, radius, controlling port, controlling pin 
    #define KEYPAD_LED_DEFINITIONS  \
        {RGB(255, 0,  0  ), RGB(0,0,0),  1, {526, 264, 0, 8 }, _PORTB, LED_RED}, \
        {RGB(0,   255,0  ), RGB(0,0,0),  1, {526, 264, 0, 8 }, _PORTE, LED_GREEN}, \
        {RGB(0,   0,  255), RGB(0,0,0),  1, {526, 264, 0, 8 }, _PORTB, LED_BLUE}, \
        {RGB(0,0,0), RGB(255,0,0),       0, {162, 8,   0, 3 }, _PORTE, PORTE_BIT24}, \
        {RGB(0,0,0), RGB(255,0,0),       0, {178, 8,   0, 3 }, _PORTE, PORTE_BIT25}, \
        {RGB(0,0,0), RGB(255,0,0),       0, {226, 8,   0, 3 }, _PORTD, PORTD_BIT1}, \
        {RGB(0,0,0), RGB(255,0,0),       0, {242, 8,   0, 3 }, _PORTD, PORTD_BIT3}, \
        {RGB(0,0,0), RGB(255,0,0),       0, {258, 8,   0, 3 }, _PORTD, PORTD_BIT2}, \
        {RGB(0,0,0), RGB(255,0,0),       0, {274, 8,   0, 3 }, _PORTD, PORTD_BIT0}, \
        {RGB(0,0,0), RGB(255,0,0),       0, {290, 8,   0, 3 }, _PORTC, PORTC_BIT4}, \
        {RGB(0,0,0), RGB(255,0,0),       0, {306, 8,   0, 3 }, _PORTA, PORTA_BIT0}, \
        {RGB(0,0,0), RGB(255,0,0),       0, {306, 23,  0, 3 }, _PORTE, PORTE_BIT26}, \
        {RGB(0,0,0), RGB(255,0,0),       0, {328, 10,  0, 3 }, _PORTC, PORTC_BIT3}, \
        {RGB(0,0,0), RGB(255,0,0),       0, {344, 10,  0, 3 }, _PORTC, PORTC_BIT2}, \
        {RGB(0,0,0), RGB(255,0,0),       0, {360, 10,  0, 3 }, _PORTA, PORTA_BIT2}, \
        {RGB(0,0,0), RGB(255,0,0),       0, {376, 10,  0, 3 }, _PORTB, PORTB_BIT23}, \
        {RGB(0,0,0), RGB(255,0,0),       0, {392, 10,  0, 3 }, _PORTA, PORTA_BIT1}, \
        {RGB(0,0,0), RGB(255,0,0),       0, {408, 10,  0, 3 }, _PORTB, PORTB_BIT9}, \
        {RGB(0,0,0), RGB(255,0,0),       0, {424, 10,  0, 3 }, _PORTC, PORTC_BIT17}, \
        {RGB(0,0,0), RGB(255,0,0),       0, {440, 10,  0, 3 }, _PORTC, PORTC_BIT16}, \
        {RGB(0,0,0), RGB(255,0,0),       0, {328, 27,  0, 3 }, _PORTC, PORTC_BIT5}, \
        {RGB(0,0,0), RGB(255,0,0),       0, {344, 27,  0, 3 }, _PORTC, PORTC_BIT7}, \
        {RGB(0,0,0), RGB(255,0,0),       0, {360, 27,  0, 3 }, _PORTC, PORTC_BIT0}, \
        {RGB(0,0,0), RGB(255,0,0),       0, {376, 27,  0, 3 }, _PORTC, PORTC_BIT9}, \
        {RGB(0,0,0), RGB(255,0,0),       0, {392, 27,  0, 3 }, _PORTC, PORTC_BIT8}, \
        {RGB(0,0,0), RGB(255,0,0),       0, {408, 27,  0, 3 }, _PORTC, PORTC_BIT1}, \
        {RGB(0,0,0), RGB(255,0,0),       0, {424, 27,  0, 3 }, _PORTB, PORTB_BIT19}, \
        {RGB(0,0,0), RGB(255,0,0),       0, {440, 27,  0, 3 }, _PORTB, PORTB_BIT18}, \
        {RGB(0,0,0), RGB(255,0,0),       0, {426, 297, 0, 3 }, _PORTB, PORTB_BIT20}, \
        {RGB(0,0,0), RGB(255,0,0),       0, {362, 314, 0, 3 }, _PORTB, PORTB_BIT2 }, \
        {RGB(0,0,0), RGB(255,0,0),       0, {378, 314, 0, 3 }, _PORTB, PORTB_BIT3 }, \
        {RGB(0,0,0), RGB(255,0,0),       0, {394, 314, 0, 3 }, _PORTB, PORTB_BIT10}, \
        {RGB(0,0,0), RGB(255,0,0),       0, {410, 314, 0, 3 }, _PORTB, PORTB_BIT11}, \
        {RGB(0,0,0), RGB(255,0,0),       0, {426, 314, 0, 3 }, _PORTC, PORTC_BIT11}, \
        {RGB(0,0,0), RGB(255,0,0),       0, {442, 314, 0, 3 }, _PORTC, PORTC_BIT10},

    #define KEYPAD "KeyPads/FRDM_K64F.bmp"

    // Accelerometer interrupt configuration
    //
    #define ACC_INT_PRIORITY       PRIORITY_PORT_C_INT
    #define ACC_INT_PORT           PORTC
    #define ACC_INT_BIT            PORTC_BIT13
    #define ACC_INT_ASSERTED()     (_READ_PORT_MASK(C, PORTC_BIT13) == 0)
    #define ACC_USE_INT2                                                 // avoid using INT1 since it is connected to SW2

    // Defines for the WAVE file recording start/stop button
    //
    #define WAVE_DISK_START_STOP_INT_PRIORITY    PRIORITY_PORT_A_INT
    #define WAVE_DISK_START_STOP_INT_PORT        PORTA
    #define WAVE_DISK_START_STOP_INT_BIT         SWITCH_3
    #define RECORDER_WAVE_FILE_NAME              "FRDM_K64F.wav"

    #define MEASURE_LOW_POWER_ON()    //_SETBITS(C, PORTC_BIT16)         // signal when the processor is in sleep mode
    #define MEASURE_LOW_POWER_OFF()   //_CLEARBITS(C, PORTC_BIT16)       // signal when the processor is in active mode

    #if defined ENC424J600_INTERFACE                                     // if the ENC424J600 is being used as SPI connected Ethernet  controller
        #define ENC424J600_IRQ       PORTC_BIT4                          // the IRQ line used
        #define ENC424J600_IRQ_PORT  PORTC                               // the port that the IRQ line is on
        #define ENC424J600_IRQ_PRIORITY PRIORITY_PORT_C_INT              // the interrupt priority
    #endif
    #endif
#elif defined EMCRAFT_K70F120M || defined EMCRAFT_K61F150M               // {9}
    #define LED_DS3                (PORTE_BIT11)                         // LED on mother board - if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define LED_DS4                (PORTE_BIT12)                         // LED on mother board - if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define SWITCH_2               (PORTE_BIT9)                          // switch 2 on mother board - if the port is changed (eg. A to B) the port macros will require appropriate adjustment too

    #define DEMO_LED_1             (LED_DS3)
    #define DEMO_LED_2             (LED_DS4)
    #define DEMO_LED_3             (PORTE_BIT13)                         // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define DEMO_LED_4             (PORTE_BIT14)                         // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too

    #define BLINK_LED              (DEMO_LED_1)

    #if defined USE_MAINTENANCE
        #define INIT_WATCHDOG_LED()                                      // let the port set up do this (the user can disable blinking)
    #else
        #define INIT_WATCHDOG_LED() _CONFIG_DRIVE_PORT_OUTPUT_VALUE(E, (BLINK_LED), (BLINK_LED), (PORT_SRE_SLOW | PORT_DSE_HIGH))
    #endif

    #define INIT_WATCHDOG_DISABLE() 
    #define WATCHDOG_DISABLE()      1 
    #define ACTIVATE_WATCHDOG()     UNLOCK_WDOG(); WDOG_TOVALL = (2000/5); WDOG_TOVALH = 0; WDOG_STCTRLH = (WDOG_STCTRLH_STNDBYEN | WDOG_STCTRLH_WAITEN | WDOG_STCTRLH_STOPEN | WDOG_STCTRLH_WDOGEN) // watchdog enabled to generate reset on 2s timeout (no further updates allowed)
    #define TOGGLE_WATCHDOG_LED()   _TOGGLE_PORT(E, BLINK_LED)

    #define CONFIG_TEST_OUTPUT()                                         // we use DEMO_LED_2 which is configured by the user code (and can be disabled in parameters if required)
    #define TOGGLE_TEST_OUTPUT()    _TOGGLE_PORT(E, DEMO_LED_2)
    #define SET_TEST_OUTPUT()       _SETBITS(E, DEMO_LED_2)
    #define CLEAR_TEST_OUTPUT()     _CLEARBITS(E, DEMO_LED_2)


    #define SHIFT_DEMO_LED_1        11                                   // since the port bits are spread out shift each to the lowest 2 bits
    #define SHIFT_DEMO_LED_2        11
    #define SHIFT_DEMO_LED_3        11
    #define SHIFT_DEMO_LED_4        11

    #define MAPPED_DEMO_LED_1       (DEMO_LED_1 >> SHIFT_DEMO_LED_1)
    #define MAPPED_DEMO_LED_2       (DEMO_LED_2 >> SHIFT_DEMO_LED_2)
    #define MAPPED_DEMO_LED_3       (DEMO_LED_3 >> SHIFT_DEMO_LED_3)
    #define MAPPED_DEMO_LED_4       (DEMO_LED_4 >> SHIFT_DEMO_LED_4)

    #define BUTTON_KEY_DEFINITIONS  {_PORTE, SWITCH_2, {335, 254, 349, 270}}
    #define KEYPAD "KeyPads/emcraftK70.bmp"

    #define CONFIGURE_MOUSE_INPUTS() 
    #define MOUSE_LEFT_CLICK()     0
    #define MOUSE_UP()             0
    #define MOUSE_DOWN()           0
    #define MOUSE_LEFT()           0
    #define MOUSE_RIGHT()          0
#elif defined TWR_K60N512 || defined TWR_K60D100M || defined TWR_K60F120M// tower kit
    #define DEMO_LED_1             (PORTA_BIT11)                         // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define DEMO_LED_2             (PORTA_BIT28)                         // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define DEMO_LED_3             (PORTA_BIT29)                         // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define DEMO_LED_4             (PORTA_BIT10)                         // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define BLINK_LED              (DEMO_LED_1)
    #define SWITCH_1               (PORTA_BIT19)                         // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define SWITCH_2               (PORTE_BIT26)                         // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too

    #define SWITCH_1_PORT          _PORTA
    #define SWITCH_2_PORT          _PORTE

    #if defined USE_MAINTENANCE
        #define INIT_WATCHDOG_LED()                                      // let the port set up do this (the user can disable blinking)
    #else
        #define INIT_WATCHDOG_LED() _CONFIG_DRIVE_PORT_OUTPUT_VALUE(A, (BLINK_LED), (BLINK_LED), (/*PORT_ODE | */PORT_SRE_SLOW | PORT_DSE_HIGH))
    #endif

    #define SHIFT_DEMO_LED_1        11                                   // since the port bits are spread out shift each to the lowest 4 bits
    #define SHIFT_DEMO_LED_2        27
    #define SHIFT_DEMO_LED_3        27
    #define SHIFT_DEMO_LED_4        7

    #define MAPPED_DEMO_LED_1       (DEMO_LED_1 >> SHIFT_DEMO_LED_1)
    #define MAPPED_DEMO_LED_2       (DEMO_LED_2 >> SHIFT_DEMO_LED_2)
    #define MAPPED_DEMO_LED_3       (DEMO_LED_3 >> SHIFT_DEMO_LED_3)
    #define MAPPED_DEMO_LED_4       (DEMO_LED_4 >> SHIFT_DEMO_LED_4)

    #define INIT_WATCHDOG_DISABLE() _CONFIG_PORT_INPUT_FAST_HIGH(A, SWITCH_1, PORT_PS_UP_ENABLE) // use fast access version (beware that this can only operate on half of the 32 bits at a time)
    #define WATCHDOG_DISABLE()      (_READ_PORT_MASK(A, SWITCH_1) == 0)  // pull this input down to disable watchdog (hold SW1 at reset)
    #define ACTIVATE_WATCHDOG()     UNLOCK_WDOG(); WDOG_TOVALL = (2000/5); WDOG_TOVALH = 0; WDOG_STCTRLH = (WDOG_STCTRLH_STNDBYEN | WDOG_STCTRLH_WAITEN | WDOG_STCTRLH_STOPEN | WDOG_STCTRLH_WDOGEN) // watchdog enabled to generate reset on 2s timeout (no further updates allowed)
    #define TOGGLE_WATCHDOG_LED()   _TOGGLE_PORT(A, BLINK_LED)

    #define CONFIG_TEST_OUTPUT()                                         // we use DEMO_LED_2 which is configured by the user code (and can be disabled in parameters if required)
    #define TOGGLE_TEST_OUTPUT()    _TOGGLE_PORT(A, DEMO_LED_2)
    #define SET_TEST_OUTPUT()       _SETBITS(A, DEMO_LED_2)
    #define CLEAR_TEST_OUTPUT()     _CLEARBITS(A, DEMO_LED_2)


    #define CONFIGURE_MOUSE_INPUTS() _CONFIG_PORT_INPUT_FAST_HIGH(E, SWITCH_2, PORT_PS_UP_ENABLE)
    #define MOUSE_LEFT_CLICK()     (!_READ_PORT_MASK(E, SWITCH_2))
    #define MOUSE_UP()             (iUpTilt/4)                           // use accelerometer (see iic_tests.h and TEST_MMA8451Q, based on I2C)
    #define MOUSE_DOWN()           (iDownTilt/4)
    #define MOUSE_LEFT()           (iLeftTilt/4)
    #define MOUSE_RIGHT()          (iRightTilt/4)

    #if defined TWR_K60D100M
        #define KEYPAD "KeyPads/TWR_K60D100M.bmp"

        #define BUTTON_KEY_DEFINITIONS  {SWITCH_1_PORT, SWITCH_1, {339, 62, 355, 80}}, \
                                        {SWITCH_2_PORT, SWITCH_2, {374, 62, 389, 80}}

                                        // '0'           '1'         input state   center (x,   y)   0 = circle, radius, controlling port, controlling pin 
        #define KEYPAD_LED_DEFINITIONS  {RGB(255,75,0), RGB(200,200,200), 1, {352, 148, 362, 157 }, _PORTA, DEMO_LED_1}, \
                                        {RGB(255,128,0),RGB(200,200,200), 1, {352, 186, 362, 194 }, _PORTA, DEMO_LED_2}, \
                                        {RGB(0,255,0),  RGB(200,200,200), 1, {352, 224, 362, 232 }, _PORTA, DEMO_LED_3}, \
                                        {RGB(20,20,255),RGB(200,200,200), 1, {352, 261, 362, 271 }, _PORTA, DEMO_LED_4}
    #else
        #define KEYPAD "KeyPads/TWR_K60.bmp"

        #define BUTTON_KEY_DEFINITIONS  {SWITCH_1_PORT, SWITCH_1, {318, 56, 336, 73}}, \
                                        {SWITCH_2_PORT, SWITCH_2, {354, 56, 372, 73}}
    #endif

    // Accelerometer interrupt configuration
    //
    #define ACC_INT_PRIORITY       PRIORITY_PORT_B_INT
    #define ACC_INT_PORT           PORTB
    #define ACC_INT_BIT            PORTB_BIT4
    #define ACC_INT_ASSERTED()     (_READ_PORT_MASK(B, PORTB_BIT4) == 0)

    // Defines for the WAVE file recording start/stop button
    //
    #define WAVE_DISK_START_STOP_INT_PRIORITY    PRIORITY_PORT_E_INT
    #define WAVE_DISK_START_STOP_INT_PORT        PORTE
    #define WAVE_DISK_START_STOP_INT_BIT         SWITCH_2
    #define RECORDER_WAVE_FILE_NAME              "TWR_K60F120.wav"
#elif defined K60F150M_50M
    #define DEMO_LED_1             (PORTB_BIT6)                          // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define DEMO_LED_2             (PORTB_BIT7)                          // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define DEMO_LED_3             (PORTB_BIT8)                          // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define DEMO_LED_4             (0)
    #define BLINK_LED              (DEMO_LED_1)
    #define SWITCH_1               (PORTA_BIT19)                         // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define SWITCH_2               (PORTE_BIT26)                         // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too

    #define SWITCH_1_PORT          _PORTA
    #define SWITCH_2_PORT          _PORTE

    #if defined USE_MAINTENANCE
        #define INIT_WATCHDOG_LED()                                      // let the port set up do this (the user can disable blinking)
    #else
        #define INIT_WATCHDOG_LED() _CONFIG_DRIVE_PORT_OUTPUT_VALUE(B, (BLINK_LED), (BLINK_LED), (PORT_SRE_SLOW | PORT_DSE_HIGH))
    #endif

    #define SHIFT_DEMO_LED_1        6                                    // since the port bits are spread out shift each to the lowest 4 bits
    #define SHIFT_DEMO_LED_2        6
    #define SHIFT_DEMO_LED_3        6

    #define MAPPED_DEMO_LED_1       (DEMO_LED_1 >> SHIFT_DEMO_LED_1)
    #define MAPPED_DEMO_LED_2       (DEMO_LED_2 >> SHIFT_DEMO_LED_2)
    #define MAPPED_DEMO_LED_3       (DEMO_LED_3 >> SHIFT_DEMO_LED_3)

    #define INIT_WATCHDOG_DISABLE() 
    #define WATCHDOG_DISABLE()      1
    #define ACTIVATE_WATCHDOG()     UNLOCK_WDOG(); WDOG_TOVALL = (2000/5); WDOG_TOVALH = 0; WDOG_STCTRLH = (WDOG_STCTRLH_STNDBYEN | WDOG_STCTRLH_WAITEN | WDOG_STCTRLH_STOPEN | WDOG_STCTRLH_WDOGEN) // watchdog enabled to generate reset on 2s timeout (no further updates allowed)
    #define TOGGLE_WATCHDOG_LED()   _TOGGLE_PORT(B, BLINK_LED)

    #define CONFIG_TEST_OUTPUT()                                         // we use DEMO_LED_2 which is configured by the user code (and can be disabled in parameters if required)
    #define TOGGLE_TEST_OUTPUT()    _TOGGLE_PORT(B, DEMO_LED_2)
    #define SET_TEST_OUTPUT()       _SETBITS(B, DEMO_LED_2)
    #define CLEAR_TEST_OUTPUT()     _CLEARBITS(B, DEMO_LED_2)
#elif defined K70F150M_12M
    #define BLUE_LED               (PORTA_BIT16)                         // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define GREEN_LED              (PORTA_BIT17)                         // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define RED_LED                (PORTA_BIT24)                         // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define SWITCH_2               (PORTA_BIT26)                         // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define SWITCH_3               (PORTA_BIT25)                         // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define USB_HOST_POWER_ENABLE  (PORTB_BIT8)

    #define DEMO_LED_1             BLUE_LED
    #define DEMO_LED_2             GREEN_LED
    #define DEMO_LED_3             RED_LED
    #define DEMO_LED_4             0

    #define BLINK_LED              (BLUE_LED)

    #define CURRENT_EN             PORTA_BIT6

    #define INIT_WATCHDOG_LED()    _CONFIG_DRIVE_PORT_OUTPUT_VALUE(A, (BLINK_LED | CURRENT_EN), (0), (PORT_SRE_SLOW | PORT_DSE_HIGH))

    #define INIT_WATCHDOG_DISABLE() _CONFIG_PORT_INPUT_FAST_HIGH(A, (SWITCH_2 | SWITCH_3), PORT_PS_UP_ENABLE); // configure as input
    #define WATCHDOG_DISABLE()     (_READ_PORT_MASK(A, SWITCH_3) == 0)   // pull this input down to disable watchdog (hold S3 at reset)
    #define TOGGLE_WATCHDOG_LED()  _TOGGLE_PORT(A, BLINK_LED)

    #define SHIFT_DEMO_LED_1        16                                   // since the port bits are spread out shift each to the lowest 4 bits
    #define SHIFT_DEMO_LED_2        16
    #define SHIFT_DEMO_LED_3        22
    #define SHIFT_DEMO_LED_4        0

    #define MAPPED_DEMO_LED_1       (DEMO_LED_1 >> SHIFT_DEMO_LED_1)
    #define MAPPED_DEMO_LED_2       (DEMO_LED_2 >> SHIFT_DEMO_LED_2)
    #define MAPPED_DEMO_LED_3       (DEMO_LED_3 >> SHIFT_DEMO_LED_3)
    #define MAPPED_DEMO_LED_4       (DEMO_LED_4 >> SHIFT_DEMO_LED_4)

    #define ACTIVATE_WATCHDOG()     UNLOCK_WDOG(); WDOG_TOVALL = (2000/5); WDOG_TOVALH = 0; WDOG_STCTRLH = (WDOG_STCTRLH_STNDBYEN | WDOG_STCTRLH_WAITEN | WDOG_STCTRLH_STOPEN | WDOG_STCTRLH_WDOGEN) // watchdog enabled to generate reset on 2s timeout (no further updates allowed)

    #define CONFIG_TEST_OUTPUT()
    #define TOGGLE_TEST_OUTPUT()    _TOGGLE_PORT(A, DEMO_LED_2)
    #define SET_TEST_OUTPUT()       _SETBITS(A, DEMO_LED_2)
    #define CLEAR_TEST_OUTPUT()     _CLEARBITS(A, DEMO_LED_2)


    #define CONFIGURE_MOUSE_INPUTS()
    #define MOUSE_LEFT_CLICK()     0
    #define MOUSE_UP()             (_READ_PORT_MASK(A, SWITCH_2) == 0)   // press this button to move mouse up
    #define MOUSE_DOWN()           (_READ_PORT_MASK(A, SWITCH_3) == 0)   // press this button to move mouse down
    #define MOUSE_LEFT()           0                                     // not used
    #define MOUSE_RIGHT()          0                                     // not used
    #if defined TWR_LCD_RGB_GLCD_MODE                                    // if used together with the TFT module
        #define GLCD_BACKLIGHT_CONTROL                                   // enable backlight intensity control
        #define MAX_LCD_INTENSITY_VALUE     0x92                         // value for maximum intensity
        #define BACK_LIGHT_MAX_INTENSITY()  LCDC_LPCCR = ((LCDC_LPCCR_SCR_LINE_CLK | LCDC_LPCCR_SCR_PIXEL_CLK | LCDC_LPCCR_CC_EN) | MAX_LCD_INTENSITY_VALUE); _CONFIG_PERIPHERAL(D, 11, PD_11_GLCD_GLCD_CONTRAST)
        #define BACK_LIGHT_MIN_INTENSITY()  LCDC_LPCCR = (LCDC_LPCCR_LDMSK) // disable the contrast control PWM so that the backlght is disabled (hold LD[23:0 at '0']
    #endif
    #define TOUCH_HW_TIMER         0                                     // flex timer 0 is used for the touch screen time base interrupt

    #define USB_HOST_POWER_CONFIG() _CONFIG_DRIVE_PORT_OUTPUT_VALUE_FAST_LOW(B, (USB_HOST_POWER_ENABLE), (0), (PORT_SRE_SLOW | PORT_DSE_HIGH))
    #define USB_HOST_POWER_ON()     _SETBITS(B, USB_HOST_POWER_ENABLE)
    #define USB_HOST_POWER_OFF()    _CLEARBITS(B, USB_HOST_POWER_ENABLE)
#elif defined TWR_K70F120M || defined TWR_K80F150M || defined FRDM_K82F || defined TWR_VF65GS10
    #if defined FRDM_K82F
        #define DEMO_LED_1             (PORTC_BIT9)                      // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
        #define DEMO_LED_2             (PORTC_BIT8)                      // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
        #define DEMO_LED_3             (PORTC_BIT10)                     // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
        #define DEMO_LED_4             (PORTC_BIT11)
        #define LED_RED                DEMO_LED_2
        #define LED_GREEN              DEMO_LED_1
        #define LED_BLUE               DEMO_LED_3
        #define BLINK_LED              DEMO_LED_1

        #define SWITCH_2               PORTA_BIT4
        #define SWITCH_3               PORTC_BIT6
        #define SWITCH_2_PORT          _PORTA
        #define SWITCH_3_PORT          _PORTC

        #if defined USE_MAINTENANCE
            #define INIT_WATCHDOG_LED()                                  // let the port set up do this (the user can disable blinking)
        #else
            #define INIT_WATCHDOG_LED() _CONFIG_DRIVE_PORT_OUTPUT_VALUE(C, (BLINK_LED), (BLINK_LED), (PORT_SRE_SLOW | PORT_DSE_HIGH))
        #endif

        #define SHIFT_DEMO_LED_1        9                                // since the port bits are spread out shift each to the lowest 4 bits
        #define SHIFT_DEMO_LED_2        7
        #define SHIFT_DEMO_LED_3        8
        #define SHIFT_DEMO_LED_4        8

        #define MAPPED_DEMO_LED_1       (DEMO_LED_1 >> SHIFT_DEMO_LED_1)
        #define MAPPED_DEMO_LED_2       (DEMO_LED_2 >> SHIFT_DEMO_LED_2)
        #define MAPPED_DEMO_LED_3       (DEMO_LED_3 >> SHIFT_DEMO_LED_3)
        #define MAPPED_DEMO_LED_4       (DEMO_LED_4 >> SHIFT_DEMO_LED_4)

        #define INIT_WATCHDOG_DISABLE() _CONFIG_PORT_INPUT_FAST_LOW(A, SWITCH_2, PORT_PS_UP_ENABLE) // use fast access version (beware that this can only operate on half of the 32 bits at a time)
        #define WATCHDOG_DISABLE()      (_READ_PORT_MASK(A, SWITCH_2) == 0) // pull this input down to disable watchdog (hold SW1 at reset)

        #define TOGGLE_WATCHDOG_LED()   _TOGGLE_PORT(C, BLINK_LED)

        #define CONFIG_TEST_OUTPUT()                                     // we use DEMO_LED_2 which is configured by the user code (and can be disabled in parameters if required)
        #define TOGGLE_TEST_OUTPUT()    _TOGGLE_PORT(C, DEMO_LED_2)
        #define SET_TEST_OUTPUT()       _SETBITS(C, DEMO_LED_2)
        #define CLEAR_TEST_OUTPUT()     _CLEARBITS(C, DEMO_LED_2)
#else
        #define DEMO_LED_1             (PORTA_BIT11)                     // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
        #define DEMO_LED_2             (PORTA_BIT28)                     // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
        #define DEMO_LED_3             (PORTA_BIT29)                     // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
        #define DEMO_LED_4             (PORTA_BIT10)                     // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
        #define BLINK_LED              (DEMO_LED_1)
        #define SWITCH_1               (PORTD_BIT0)                      // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
        #define SWITCH_2               (PORTE_BIT26)                     // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
        #define SWITCH_1_PORT          _PORTD
        #define SWITCH_2_PORT          _PORTE

        #if defined USE_MAINTENANCE
            #define INIT_WATCHDOG_LED()                                  // let the port set up do this (the user can disable blinking)
        #else
            #define INIT_WATCHDOG_LED() _CONFIG_DRIVE_PORT_OUTPUT_VALUE(A, (BLINK_LED), (BLINK_LED), (/*PORT_ODE | */PORT_SRE_SLOW | PORT_DSE_HIGH))
        #endif

        #define SHIFT_DEMO_LED_1        11                               // since the port bits are spread out shift each to the lowest 4 bits
        #define SHIFT_DEMO_LED_2        27
        #define SHIFT_DEMO_LED_3        27
        #define SHIFT_DEMO_LED_4        7

        #define MAPPED_DEMO_LED_1       (DEMO_LED_1 >> SHIFT_DEMO_LED_1)
        #define MAPPED_DEMO_LED_2       (DEMO_LED_2 >> SHIFT_DEMO_LED_2)
        #define MAPPED_DEMO_LED_3       (DEMO_LED_3 >> SHIFT_DEMO_LED_3)
        #define MAPPED_DEMO_LED_4       (DEMO_LED_4 >> SHIFT_DEMO_LED_4)

        #define INIT_WATCHDOG_DISABLE() _CONFIG_PORT_INPUT_FAST_LOW(D, SWITCH_1, PORT_PS_UP_ENABLE) // use fast access version (beware that this can only operate on half of the 32 bits at a time)
        #define WATCHDOG_DISABLE()      (_READ_PORT_MASK(D, SWITCH_1) == 0) // pull this input down to disable watchdog (hold SW1 at reset)

        #define TOGGLE_WATCHDOG_LED()   _TOGGLE_PORT(A, BLINK_LED)

        #define CONFIG_TEST_OUTPUT()                                     // we use DEMO_LED_2 which is configured by the user code (and can be disabled in parameters if required)
        #define TOGGLE_TEST_OUTPUT()    _TOGGLE_PORT(A, DEMO_LED_2)
        #define SET_TEST_OUTPUT()       _SETBITS(A, DEMO_LED_2)
        #define CLEAR_TEST_OUTPUT()     _CLEARBITS(A, DEMO_LED_2)
    #endif

    #if defined TWR_VF65GS10
        #define ACTIVATE_WATCHDOG() _EXCEPTION("TO DO")
    #else
        #define ACTIVATE_WATCHDOG() UNLOCK_WDOG(); WDOG_TOVALL = (2000/5); WDOG_TOVALH = 0; WDOG_STCTRLH = (WDOG_STCTRLH_STNDBYEN | WDOG_STCTRLH_WAITEN | WDOG_STCTRLH_STOPEN | WDOG_STCTRLH_WDOGEN) // watchdog enabled to generate reset on 2s timeout (no further updates allowed)
    #endif

    #define CONFIGURE_MOUSE_INPUTS() _CONFIG_PORT_INPUT_FAST_LOW(D, SWITCH_1, PORT_PS_UP_ENABLE); _CONFIG_PORT_INPUT_FAST_HIGH(E, SWITCH_2, PORT_PS_UP_ENABLE)
    #define MOUSE_LEFT_CLICK()     0
    #define MOUSE_UP()             (_READ_PORT_MASK(D, SWITCH_1) == 0)   // press this button to move mouse up
    #define MOUSE_DOWN()           (_READ_PORT_MASK(E, SWITCH_2) == 0)   // press this button to move mouse down
    #define MOUSE_LEFT()           0                                     // not used
    #define MOUSE_RIGHT()          0                                     // not used

    #if defined TWR_K80F150M
        #define KEYPAD "KeyPads/TWR_K80F150M.bmp"
    #elif defined FRDM_K82F
        #define KEYPAD "KeyPads/FRDM_K82F.bmp"

    #define BUTTON_KEY_DEFINITIONS  {_PORTA, SWITCH_2,   {444, 269, 459, 282 }}, \
                                    {_PORTC, SWITCH_3,   {445, 26,  462, 37 }},

    #define MULTICOLOUR_LEDS        {0, 2}                               // single LED made up of entries 0, 1 and 2

        // '0'          '1'           input state   center (x,   y)   0 = circle, radius, controlling port, controlling pin 
    #define KEYPAD_LED_DEFINITIONS  \
        {RGB(255, 0,  0  ), RGB(0,0,0),  1, {342, 88,  0, 7 }, _PORTC, LED_RED}, \
        {RGB(0,   255,0  ), RGB(0,0,0),  1, {342, 88,  0, 7 }, _PORTC, LED_GREEN}, \
        {RGB(0,   0,  255), RGB(0,0,0),  1, {342, 88,  0, 7 }, _PORTC, LED_BLUE},
    #else
        #define KEYPAD "KeyPads/TWR_K70.bmp"

                                        // '0'            '1'         input state   center (x,   y)   0 = circle, radius, controlling port, controlling pin 
        #define KEYPAD_LED_DEFINITIONS  {RGB(255,75,0), RGB(200,200,200), 1, {338, 123, 346, 133 }, _PORTA, DEMO_LED_1}, \
                                        {RGB(255,128,0),RGB(200,200,200), 1, {338, 160, 346, 169 }, _PORTA, DEMO_LED_2}, \
                                        {RGB(0,255,0),  RGB(200,200,200), 1, {338, 197, 346, 206 }, _PORTA, DEMO_LED_3}, \
                                        {RGB(32,32,255),RGB(200,200,200), 1, {338, 233, 346, 243 }, _PORTA, DEMO_LED_4}

        #define BUTTON_KEY_DEFINITIONS  {SWITCH_1_PORT, SWITCH_1, {325, 58, 339, 73}}, \
                                        {SWITCH_2_PORT, SWITCH_2, {360, 58, 373, 73}}
    #endif

    // Accelerometer interrupt configuration
    //
    #define ACC_INT_PRIORITY       PRIORITY_PORT_B_INT
    #define ACC_INT_PORT           PORTB
    #define ACC_INT_BIT            PORTB_BIT4
    #define ACC_INT_ASSERTED()     (_READ_PORT_MASK(B, PORTB_BIT4) == 0)

    // Defines for the WAVE file recording start/stop button
    //
    #define WAVE_DISK_START_STOP_INT_PRIORITY    PRIORITY_PORT_E_INT
    #define WAVE_DISK_START_STOP_INT_PORT        PORTE
    #define WAVE_DISK_START_STOP_INT_BIT         SWITCH_2                // switch 2 used to start/stop wave file recording
    #define RECORDER_WAVE_FILE_NAME              "TWR_K70F120M.wav"

    // When using HS USB on the TWR-SER2 board the USB tranceiver needs to be taken out of reset by setting PTB8 to '1'
    //
    #define ENABLE_HSUSB_TRANSCEIVER()   _CONFIG_DRIVE_PORT_OUTPUT_VALUE(B, (PORTB_BIT8), (PORTB_BIT8), (PORT_SRE_SLOW | PORT_DSE_LOW)); // take the HS USB transceiver out of reset state
#elif defined TWR_K24F120M
    #define DEMO_LED_1             (PORTD_BIT4)                          // (yellow led) if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define DEMO_LED_2             (PORTD_BIT5)                          // (red led) if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define DEMO_LED_3             (PORTD_BIT6)                          // (orange led) if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define DEMO_LED_4             (PORTD_BIT7)                          // (green led) if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define BLINK_LED              (DEMO_LED_1)
    #define SWITCH_1               (PORTC_BIT7)                          // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define SWITCH_2               (PORTC_BIT6)                          // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too

    #define SWITCH_1_PORT          _PORTC
    #define SWITCH_2_PORT          _PORTC

    #if defined USE_MAINTENANCE
        #define INIT_WATCHDOG_LED()                                      // let the port set up do this (the user can disable blinking)
    #else
        #define INIT_WATCHDOG_LED() _CONFIG_DRIVE_PORT_OUTPUT_VALUE(D, (BLINK_LED | DEMO_LED_2), (BLINK_LED), (PORT_SRE_SLOW | PORT_DSE_HIGH))
    #endif

    #define SHIFT_DEMO_LED_1        4                                    // since the port bits may be spread out shift each to the lowest 4 bits
    #define SHIFT_DEMO_LED_2        4
    #define SHIFT_DEMO_LED_3        4
    #define SHIFT_DEMO_LED_4        4

    #define MAPPED_DEMO_LED_1       (DEMO_LED_1 >> SHIFT_DEMO_LED_1)
    #define MAPPED_DEMO_LED_2       (DEMO_LED_2 >> SHIFT_DEMO_LED_2)
    #define MAPPED_DEMO_LED_3       (DEMO_LED_3 >> SHIFT_DEMO_LED_3)
    #define MAPPED_DEMO_LED_4       (DEMO_LED_4 >> SHIFT_DEMO_LED_4)

    #define INIT_WATCHDOG_DISABLE() _CONFIG_PORT_INPUT_FAST_LOW(C, (SWITCH_1 | SWITCH_2), PORT_PS_UP_ENABLE) // use fast access version (beware that this can only operate on half of the 32 bits at a time)
    #define WATCHDOG_DISABLE()      (_READ_PORT_MASK(C, SWITCH_2) == 0)  // pull this input down to disable watchdog (hold SW2 at reset)
    #define ACTIVATE_WATCHDOG()     UNLOCK_WDOG(); WDOG_TOVALL = (2000/5); WDOG_TOVALH = 0; WDOG_STCTRLH = (WDOG_STCTRLH_STNDBYEN | WDOG_STCTRLH_WAITEN | WDOG_STCTRLH_STOPEN | WDOG_STCTRLH_WDOGEN) // watchdog enabled to generate reset on 2s timeout (no further updates allowed)
    #define TOGGLE_WATCHDOG_LED()   _TOGGLE_PORT(D, BLINK_LED)

    #define CONFIG_TEST_OUTPUT()                                         // we use DEMO_LED_2 which is configured by the user code (and can be disabled in parameters if required)
    #define TOGGLE_TEST_OUTPUT()    _TOGGLE_PORT(D, DEMO_LED_2)
    #define SET_TEST_OUTPUT()       _SETBITS(D, DEMO_LED_2)
    #define CLEAR_TEST_OUTPUT()     _CLEARBITS(D, DEMO_LED_2)

    #define CONFIGURE_MOUSE_INPUTS() 
    #define MOUSE_LEFT_CLICK()     (_READ_PORT_MASK(C, SWITCH_2) == 0)
    #define MOUSE_UP()             0                                     // not used
    #define MOUSE_DOWN()           0                                     // not used
    #define MOUSE_LEFT()           0                                     // not used
    #define MOUSE_RIGHT()          0                                     // not used

    #define BUTTON_KEY_DEFINITIONS  {SWITCH_1_PORT, SWITCH_1, {30,  72,  44,  85}}, \
                                    {SWITCH_2_PORT, SWITCH_2, {30,  99,  44,  111}}

        // '0'          '1'         input state    center (x,   y)   0 = circle, radius, controlling port, controlling pin 
    #define KEYPAD_LED_DEFINITIONS  \
        {RGB(255,255,0), RGB(40,40,40),   1,   {14,  108, 21,  116}, _PORTD, DEMO_LED_1}, \
        {RGB(255,0,0),   RGB(40,40,40),   1,   {14,  96,  21,  102}, _PORTD, DEMO_LED_2}, \
        {RGB(255,128,40),RGB(40,40,40),   1,   {14,  82,  21,  87 }, _PORTD, DEMO_LED_3}, \
        {RGB(0,255,0),   RGB(40,40,40),   1,   {14,  69,  21,  76 }, _PORTD, DEMO_LED_4}

    #define KEYPAD "KeyPads/TWR_K24F120M.bmp"
#elif defined TWR_K21F120M
    #define DEMO_LED_1             (PORTD_BIT4)                          // (yellow/green led) if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define DEMO_LED_2             (PORTD_BIT5)                          // (yellow led) if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define DEMO_LED_3             (PORTD_BIT6)                          // (orange led) if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define DEMO_LED_4             (PORTD_BIT7)                          // (blue led) if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define BLINK_LED              (DEMO_LED_1)
    #define SWITCH_2               (PORTC_BIT7)                          // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define SWITCH_3               (PORTC_BIT6)                          // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define SD_CARD_DETECTION      (PORTC_BIT18)
    #define WRITE_PROTECT_INPUT    (PORTC_BIT19)
    #define USB_HOST_POWER_ENABLE  (PORTC_BIT9)

    #define SWITCH_2_PORT          _PORTC
    #define SWITCH_3_PORT          _PORTC

    #if defined USE_MAINTENANCE
        #define INIT_WATCHDOG_LED()                                      // let the port set up do this (the user can disable blinking)
    #else
        #define INIT_WATCHDOG_LED() _CONFIG_DRIVE_PORT_OUTPUT_VALUE(D, (BLINK_LED | DEMO_LED_2), (BLINK_LED), (PORT_SRE_SLOW | PORT_DSE_HIGH))
    #endif

    #define SHIFT_DEMO_LED_1        4                                    // since the port bits may be spread out shift each to the lowest 4 bits
    #define SHIFT_DEMO_LED_2        4
    #define SHIFT_DEMO_LED_3        4
    #define SHIFT_DEMO_LED_4        4

    #define MAPPED_DEMO_LED_1       (DEMO_LED_1 >> SHIFT_DEMO_LED_1)
    #define MAPPED_DEMO_LED_2       (DEMO_LED_2 >> SHIFT_DEMO_LED_2)
    #define MAPPED_DEMO_LED_3       (DEMO_LED_3 >> SHIFT_DEMO_LED_3)
    #define MAPPED_DEMO_LED_4       (DEMO_LED_4 >> SHIFT_DEMO_LED_4)

    #define INIT_WATCHDOG_DISABLE() _CONFIG_PORT_INPUT_FAST_LOW(C, (SWITCH_2 | SWITCH_3), PORT_PS_UP_ENABLE) // use fast access version (beware that this can only operate on half of the 32 bits at a time)
    #define WATCHDOG_DISABLE()      (_READ_PORT_MASK(C, SWITCH_2) == 0)  // pull this input down to disable watchdog (hold SW2 at reset)
    #define ACTIVATE_WATCHDOG()     UNLOCK_WDOG(); WDOG_TOVALL = (2000/5); WDOG_TOVALH = 0; WDOG_STCTRLH = (WDOG_STCTRLH_STNDBYEN | WDOG_STCTRLH_WAITEN | WDOG_STCTRLH_STOPEN | WDOG_STCTRLH_WDOGEN) // watchdog enabled to generate reset on 2s timeout (no further updates allowed)
    #define TOGGLE_WATCHDOG_LED()   _TOGGLE_PORT(D, BLINK_LED)

    #define CONFIG_TEST_OUTPUT()                                         // we use DEMO_LED_2 which is configured by the user code (and can be disabled in parameters if required)
    #define TOGGLE_TEST_OUTPUT()    _TOGGLE_PORT(D, DEMO_LED_2)
    #define SET_TEST_OUTPUT()       _SETBITS(D, DEMO_LED_2)
    #define CLEAR_TEST_OUTPUT()     _CLEARBITS(D, DEMO_LED_2)

    #define USB_HOST_POWER_CONFIG() _CONFIG_DRIVE_PORT_OUTPUT_VALUE_FAST_LOW(C, (USB_HOST_POWER_ENABLE), (0), (PORT_SRE_SLOW | PORT_DSE_HIGH))
    #define USB_HOST_POWER_ON()     _SETBITS(C, USB_HOST_POWER_ENABLE)
    #define USB_HOST_POWER_OFF()    _CLEARBITS(C, USB_HOST_POWER_ENABLE)

    #define CONFIGURE_MOUSE_INPUTS() _CONFIG_PORT_INPUT_FAST_LOW(C, SWITCH_2, PORT_PS_UP_ENABLE); _CONFIG_PORT_INPUT_FAST_LOW(C, SWITCH_3, PORT_PS_UP_ENABLE)
    #define MOUSE_LEFT_CLICK()     0
    #define MOUSE_UP()             (_READ_PORT_MASK(C, SWITCH_2) == 0)   // press this button to move mouse up
    #define MOUSE_DOWN()           (_READ_PORT_MASK(C, SWITCH_3) == 0)   // press this button to move mouse down
    #define MOUSE_LEFT()           0                                     // not used
    #define MOUSE_RIGHT()          0                                     // not used

    #define BUTTON_KEY_DEFINITIONS  {SWITCH_2_PORT, SWITCH_2, {365, 320, 377, 333}}, \
                                    {SWITCH_3_PORT, SWITCH_3, {415, 320, 428, 333}}

        // '0'          '1'         input state    center (x,   y)   0 = circle, radius, controlling port, controlling pin 
    #define KEYPAD_LED_DEFINITIONS  \
        {RGB(170,255,0), RGB(40,40,40),   1,   {445, 240, 457, 249}, _PORTD, DEMO_LED_1}, \
        {RGB(170,255,0), RGB(40,40,40),   1,   {445, 253, 457, 261}, _PORTD, DEMO_LED_2}, \
        {RGB(255,128,40),RGB(40,40,40),   1,   {445, 266, 457, 275}, _PORTD, DEMO_LED_3}, \
        {RGB(0,0,255),   RGB(40,40,40),   1,   {445, 281, 457, 287}, _PORTD, DEMO_LED_4}

    #define KEYPAD "KeyPads/TWR_K21F120M.bmp"
#elif defined FRDM_K22F || defined K22F128_100M
    #define DEMO_LED_1             (PORTA_BIT2)                          // (green led) if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define DEMO_LED_2             (PORTA_BIT1)                          // (red led) if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define DEMO_LED_3             (PORTD_BIT5)                          // (blue led) if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define DEMO_LED_4             (PORTD_BIT6)                          // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define BLINK_LED              (DEMO_LED_1)
    #define SWITCH_2               (PORTC_BIT1)                          // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define SWITCH_3               (PORTB_BIT17)                         // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define SDCARD_DETECT_PIN      (PORTB_BIT16)
    #define WRITE_PROTECT_INPUT    (0)
    #define USB_HOST_POWER_ENABLE  (0)

    #define SWITCH_2_PORT          _PORTC
    #define SWITCH_3_PORT          _PORTB

    #define LED_RED                DEMO_LED_2
    #define LED_GREEN              DEMO_LED_1
    #define LED_BLUE               DEMO_LED_3

    #if defined USE_MAINTENANCE
        #define INIT_WATCHDOG_LED()                                      // let the port set up do this (the user can disable blinking)
    #else
        #define INIT_WATCHDOG_LED() _CONFIG_DRIVE_PORT_OUTPUT_VALUE(A, (BLINK_LED | DEMO_LED_2), (BLINK_LED), (PORT_SRE_SLOW | PORT_DSE_HIGH))
    #endif

    #define SHIFT_DEMO_LED_1        2                                    // since the port bits may be spread out shift each to the lowest 4 bits
    #define SHIFT_DEMO_LED_2        0
    #define SHIFT_DEMO_LED_3        3
    #define SHIFT_DEMO_LED_4        3

    #define MAPPED_DEMO_LED_1       (DEMO_LED_1 >> SHIFT_DEMO_LED_1)
    #define MAPPED_DEMO_LED_2       (DEMO_LED_2 >> SHIFT_DEMO_LED_2)
    #define MAPPED_DEMO_LED_3       (DEMO_LED_3 >> SHIFT_DEMO_LED_3)
    #define MAPPED_DEMO_LED_4       (DEMO_LED_4 >> SHIFT_DEMO_LED_4)

    #define INIT_WATCHDOG_DISABLE() _CONFIG_PORT_INPUT_FAST_LOW(C, (SWITCH_2), PORT_PS_UP_ENABLE); _CONFIG_PORT_INPUT_FAST_LOW(B, (SWITCH_3), PORT_PS_UP_ENABLE) // use fast access version (beware that this can only operate on half of the 32 bits at a time)
    #define WATCHDOG_DISABLE()      (_READ_PORT_MASK(C, SWITCH_2) == 0)  // pull this input down to disable watchdog (hold SW2 at reset)
    #define ACTIVATE_WATCHDOG()     UNLOCK_WDOG(); WDOG_TOVALL = (2000/5); WDOG_TOVALH = 0; WDOG_STCTRLH = (WDOG_STCTRLH_STNDBYEN | WDOG_STCTRLH_WAITEN | WDOG_STCTRLH_STOPEN | WDOG_STCTRLH_WDOGEN) // watchdog enabled to generate reset on 2s timeout (no further updates allowed)
    #define TOGGLE_WATCHDOG_LED()   _TOGGLE_PORT(A, BLINK_LED)

    #define CONFIG_TEST_OUTPUT()                                         // we use DEMO_LED_2 which is configured by the user code (and can be disabled in parameters if required)
    #define TOGGLE_TEST_OUTPUT()    _TOGGLE_PORT(A, DEMO_LED_2)
    #define SET_TEST_OUTPUT()       _SETBITS(A, DEMO_LED_2)
    #define CLEAR_TEST_OUTPUT()     _CLEARBITS(A, DEMO_LED_2)

    #define CONFIGURE_MOUSE_INPUTS() _CONFIG_PORT_INPUT_FAST_LOW(C, SWITCH_2, PORT_PS_UP_ENABLE); _CONFIG_PORT_INPUT_FAST_LOW(C, SWITCH_B, PORT_PS_UP_ENABLE)
    #define MOUSE_LEFT_CLICK()     0
    #define MOUSE_UP()             (_READ_PORT_MASK(C, SWITCH_2) == 0)   // press this button to move mouse up
    #define MOUSE_DOWN()           (_READ_PORT_MASK(B, SWITCH_3) == 0)   // press this button to move mouse down
    #define MOUSE_LEFT()           0                                     // not used
    #define MOUSE_RIGHT()          0                                     // not used

    #define BUTTON_KEY_DEFINITIONS  {SWITCH_2_PORT, SWITCH_2, {24,  141, 36,  150}}, \
                                    {SWITCH_3_PORT, SWITCH_3, {22,  172, 37,  181}}

    #define MULTICOLOUR_LEDS        {0, 2}                               // single LED made up of entries 0, 1 and 2

        // '0'          '1'           input state   center (x,   y)   0 = circle, radius, controlling port, controlling pin 
    #define KEYPAD_LED_DEFINITIONS  \
        {RGB(255, 0,  0  ), RGB(0,0,0),  1, {295, 194, 0, 5 }, _PORTA, LED_RED}, \
        {RGB(0,   255,0  ), RGB(0,0,0),  1, {295, 194, 0, 5 }, _PORTA, LED_GREEN}, \
        {RGB(0,   0,  255), RGB(0,0,0),  1, {295, 194, 0, 5 }, _PORTD, LED_BLUE}

    #define KEYPAD "KeyPads/FRDM_K22F.bmp"
#elif defined BLAZE_K22
    #define EXT_IO0                (PORTA_BIT0)
    #define EXT_IO1                (PORTA_BIT1)                          // IoT UART
    #define EXT_IO2                (PORTA_BIT2)                          // IoT UART
    #define EXT_IO3                (PORTA_BIT3)
    #define EXT_IO4                (PORTA_BIT12)
    #define EXT_IO5                (PORTA_BIT13)
    #define EXT_IO6                (PORTC_BIT0)
    #define EXT_IO7                (PORTC_BIT1)
    #define EXT_IO8                (PORTC_BIT2)
    #define EXT_IO9                (PORTC_BIT3)
    #define EXT_IO10               (PORTE_BIT0)                          // IoT UART
    #define EXT_IO11               (PORTE_BIT1)                          // IoT UART
    #define TSI_RESET_LINE         (PORTB_BIT2)
    #define TSI_INTERRUPT_LINE     (PORTB_BIT3)
    #define DEMO_LED_1             (PORTB_BIT16)                         // (red led) if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define DEMO_LED_2             (PORTA_BIT0)                          // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define DEMO_LED_3             (PORTA_BIT3)                          // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define DEMO_LED_4             (PORTA_BIT12)                         // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define BLINK_LED              (DEMO_LED_1)
    #define SWITCH_1               (PORTA_BIT3)                          // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define SWITCH_2               (PORTA_BIT12)                         // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define SDCARD_DETECT_PIN      (PORTD_BIT7)                          // '0' when SD card is inserted
    #define WRITE_PROTECT_INPUT    (0)
    #define USB_HOST_POWER_ENABLE  (0)

    #define SWITCH_1_PORT          _PORTA
    #define SWITCH_2_PORT          _PORTA

    #define LED_RED                DEMO_LED_1

    #if defined USE_MAINTENANCE
        #define INIT_WATCHDOG_LED()                                      // let the port set up do this (the user can disable blinking)
    #else
        #define INIT_WATCHDOG_LED() _CONFIG_DRIVE_PORT_OUTPUT_VALUE(B, (BLINK_LED), (BLINK_LED), (PORT_SRE_SLOW | PORT_DSE_HIGH)); _CONFIG_DRIVE_PORT_OUTPUT_VALUE(A, (DEMO_LED_2), (DEMO_LED_2), (PORT_SRE_SLOW | PORT_DSE_HIGH))
    #endif

    #define SHIFT_DEMO_LED_1        16                                    // since the port bits may be spread out shift each to the lowest 4 bits
    #define SHIFT_DEMO_LED_2        1
    #define SHIFT_DEMO_LED_3        1
    #define SHIFT_DEMO_LED_4        9

    #define MAPPED_DEMO_LED_1       (DEMO_LED_1 >> SHIFT_DEMO_LED_1)
    #define MAPPED_DEMO_LED_2       (DEMO_LED_2 << SHIFT_DEMO_LED_2)
    #define MAPPED_DEMO_LED_3       (DEMO_LED_3 >> SHIFT_DEMO_LED_3)
    #define MAPPED_DEMO_LED_4       (DEMO_LED_4 >> SHIFT_DEMO_LED_4)

    #define INIT_WATCHDOG_DISABLE() _CONFIG_PORT_INPUT_FAST_LOW(A, (SWITCH_1 | SWITCH_2), PORT_PS_UP_ENABLE) // use fast access version (beware that this can only operate on half of the 32 bits at a time)
    #define WATCHDOG_DISABLE()      (_READ_PORT_MASK(A, SWITCH_2) == 0)  // pull this input down to disable watchdog (hold SW2 at reset)
    #define ACTIVATE_WATCHDOG()     UNLOCK_WDOG(); WDOG_TOVALL = (2000/5); WDOG_TOVALH = 0; WDOG_STCTRLH = (WDOG_STCTRLH_STNDBYEN | WDOG_STCTRLH_WAITEN | WDOG_STCTRLH_STOPEN | WDOG_STCTRLH_WDOGEN) // watchdog enabled to generate reset on 2s timeout (no further updates allowed)
    #define TOGGLE_WATCHDOG_LED()   _TOGGLE_PORT(B, BLINK_LED)

    #define CONFIG_TEST_OUTPUT()                                         // we use DEMO_LED_2 which is configured by the user code (and can be disabled in parameters if required)
    #define TOGGLE_TEST_OUTPUT()    _TOGGLE_PORT(A, DEMO_LED_2)
    #define SET_TEST_OUTPUT()       _SETBITS(A, DEMO_LED_2)
    #define CLEAR_TEST_OUTPUT()     _CLEARBITS(A, DEMO_LED_2)

    #define TOUCHRESET_H()          _SETBITS(B, TSI_RESET_LINE)          // take touch screen controller out of reset
    #define TC_INT_PRIORTY          PRIORITY_PORT_B_INT                  // touch screen interrupt priority level
    #define TC_INT_PORT             PORTB                                // the port that the touch screen interrupt input is on
    #define TC_INT_PORT_BIT         TSI_INTERRUPT_LINE                   // the touch screen interrupt input

    #define BUTTON_KEY_DEFINITIONS  {_PORTA, EXT_IO0, {544,   4, 633,  30}}, \
                                    {_PORTA, EXT_IO1, {544,  39, 633,  69}}, \
                                    {_PORTA, EXT_IO2, {544,  77, 633, 106}}, \
                                    {_PORTA, EXT_IO3, {544, 114, 633, 144}}, \
                                    {_PORTA, EXT_IO4, {544, 191, 633, 220}}, \
                                    {_PORTA, EXT_IO5, {544, 228, 633, 258}}, \
                                    {_PORTC, EXT_IO6, {544, 266, 633, 294}}, \
                                    {_PORTC, EXT_IO7, {544, 304, 633, 333}}, \
                                    {_PORTC, EXT_IO8, {544, 341, 633, 372}}, \
                                    {_PORTC, EXT_IO9, {544, 381, 633, 409}}, \
                                    {_PORTE, EXT_IO10,{7,   341,  94, 372}}, \
                                    {_PORTE, EXT_IO11,{7,   381,  94, 409}},

        // '0'           '1'           input state   center (x,   y)   0 = circle, radius, controlling port, controlling pin 
    #define KEYPAD_LED_DEFINITIONS \
        {RGB(255, 0, 0), RGB(0,   0,   0), 1, {425, 56,  0, 7 }, _PORTB, LED_RED}, \
        {RGB(0, 0, 0),   RGB(255, 255, 0), 0, {472, 114, 0, 4 }, _PORTA, EXT_IO0}, \
        {RGB(0, 0, 0),   RGB(255, 255, 0), 0, {472, 135, 0, 4 }, _PORTA, EXT_IO1}, \
        {RGB(0, 0, 0),   RGB(255, 255, 0), 0, {472, 155, 0, 4 }, _PORTA, EXT_IO2}, \
        {RGB(0, 0, 0),   RGB(255, 255, 0), 0, {472, 175, 0, 4 }, _PORTA, EXT_IO3}, \
        {RGB(0, 0, 0),   RGB(255, 255, 0), 0, {472, 215, 0, 4 }, _PORTA, EXT_IO4}, \
        {RGB(0, 0, 0),   RGB(255, 255, 0), 0, {472, 235, 0, 4 }, _PORTA, EXT_IO5}, \
        {RGB(0, 0, 0),   RGB(255, 255, 0), 0, {472, 255, 0, 4 }, _PORTC, EXT_IO6}, \
        {RGB(0, 0, 0),   RGB(255, 255, 0), 0, {472, 275, 0, 4 }, _PORTC, EXT_IO7}, \
        {RGB(0, 0, 0),   RGB(255, 255, 0), 0, {472, 295, 0, 4 }, _PORTC, EXT_IO8}, \
        {RGB(0, 0, 0),   RGB(255, 255, 0), 0, {472, 315, 0, 4 }, _PORTC, EXT_IO9}, \
        {RGB(0, 0, 0),   RGB(255, 255, 0), 0, {168, 295, 0, 4 }, _PORTE, EXT_IO10}, \
        {RGB(0, 0, 0),   RGB(255, 255, 0), 0, {168, 315, 0, 4 }, _PORTE, EXT_IO11},

    #define KEYPAD "KeyPads/blaze.bmp"
    #define LCD_ON_KEYPAD                                                // simulator positions the LCD on top of the key pad/panel
        #define LCD_X_OFFSET       252
        #define LCD_Y_OFFSET       50
#elif defined TWR_K22F120M
    #define DEMO_LED_1             (PORTD_BIT4)                          // (green led) if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define DEMO_LED_2             (PORTD_BIT5)                          // (yellow led) if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define DEMO_LED_3             (PORTD_BIT6)                          // (orange led) if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define DEMO_LED_4             (PORTD_BIT7)                          // (blue led) if the port is changed (eg. A to B) the port macros will require appropriate adjustment too    
    
    #define SWITCH_1               (PORTC_BIT6)                          // switch 1 - if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define SWITCH_3               (PORTC_BIT7)                          // switch 3 - if the port is changed (eg. A to B) the port macros will require appropriate adjustment too


    #define SWITCH_1_PORT          _PORTC
    #define SWITCH_3_PORT          _PORTC

    #define LED_GREEN               DEMO_LED_1
    #define LED_YELLOW              DEMO_LED_2
    #define LED_ORANGE              DEMO_LED_3
    #define LED_BLUE                DEMO_LED_4

    #define BLINK_LED               LED_GREEN


    #if defined USE_MAINTENANCE
        #define INIT_WATCHDOG_LED()                                      // let the port set up do this (the user can disable blinking)
    #else
        #define INIT_WATCHDOG_LED() _CONFIG_DRIVE_PORT_OUTPUT_VALUE(D, (BLINK_LED | DEMO_LED_2), (BLINK_LED), (PORT_SRE_SLOW | PORT_DSE_HIGH))
    #endif

    #define SHIFT_DEMO_LED_1        4                                    // since the port bits may be spread out shift each to the lowest 4 bits
    #define SHIFT_DEMO_LED_2        4
    #define SHIFT_DEMO_LED_3        4
    #define SHIFT_DEMO_LED_4        4

    #define MAPPED_DEMO_LED_1       (DEMO_LED_1 >> SHIFT_DEMO_LED_1)
    #define MAPPED_DEMO_LED_2       (DEMO_LED_2 >> SHIFT_DEMO_LED_2)
    #define MAPPED_DEMO_LED_3       (DEMO_LED_3 >> SHIFT_DEMO_LED_3)
    #define MAPPED_DEMO_LED_4       (DEMO_LED_4 >> SHIFT_DEMO_LED_4)

    #define INIT_WATCHDOG_DISABLE() _CONFIG_PORT_INPUT_FAST_LOW(C, (SWITCH_1 | SWITCH_3), PORT_PS_UP_ENABLE) // use fast access version (beware that this can only operate on half of the 32 bits at a time)
    #define WATCHDOG_DISABLE()      (_READ_PORT_MASK(C, SWITCH_3) == 0)  // pull this input down to disable watchdog (hold SW3 at reset)
    #define ACTIVATE_WATCHDOG()     UNLOCK_WDOG(); WDOG_TOVALL = (2000/5); WDOG_TOVALH = 0; WDOG_STCTRLH = (WDOG_STCTRLH_STNDBYEN | WDOG_STCTRLH_WAITEN | WDOG_STCTRLH_STOPEN | WDOG_STCTRLH_WDOGEN) // watchdog enabled to generate reset on 2s timeout (no further updates allowed)
    #define TOGGLE_WATCHDOG_LED()   _TOGGLE_PORT(D, BLINK_LED)

    #define CONFIG_TEST_OUTPUT()                                         // we use DEMO_LED_2 which is configured by the user code (and can be disabled in parameters if required)
    #define TOGGLE_TEST_OUTPUT()    _TOGGLE_PORT(D, DEMO_LED_2)
    #define SET_TEST_OUTPUT()       _SETBITS(D, DEMO_LED_2)
    #define CLEAR_TEST_OUTPUT()     _CLEARBITS(D, DEMO_LED_2)

    #define CONFIGURE_MOUSE_INPUTS() 
    #define MOUSE_LEFT_CLICK()     0
    #define MOUSE_UP()             (_READ_PORT_MASK(C, SWITCH_1) == 0)   // press this button to move mouse up
    #define MOUSE_DOWN()           (_READ_PORT_MASK(C, SWITCH_3) == 0)   // press this button to move mouse down
    #define MOUSE_LEFT()           0                                     // not used
    #define MOUSE_RIGHT()          0                                     // not used

    #define BUTTON_KEY_DEFINITIONS  {SWITCH_1_PORT, SWITCH_1, {204,  117, 219, 130}}, \
                                    {SWITCH_3_PORT, SWITCH_3, {204,  142, 219, 154}}


        // '0'          '1'           input state   center (x,   y)   0 = circle, radius, controlling port, controlling pin 
    #define KEYPAD_LED_DEFINITIONS  \
        {RGB(0,   0,  255), RGB(0,0,0),  1, {10, 74, 14, 82  }, _PORTD, LED_BLUE}, \
        {RGB(255, 165,0  ), RGB(0,0,0),  1, {10, 84, 14, 90  }, _PORTD, LED_ORANGE}, \
        {RGB(255, 255,0  ), RGB(0,0,0),  1, {10, 92, 14, 96  }, _PORTD, LED_YELLOW}, \
        {RGB(0,   255,0  ), RGB(0,0,0),  1, {10, 98, 14, 106 }, _PORTD, LED_GREEN}

    #define KEYPAD "KeyPads/TWR_K22F120M.bmp"
#elif defined TWR_K21D50M
    #define LED_GREEN              (PORTD_BIT4)                          // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define LED_ORANGE             (PORTD_BIT5)                          // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define LED_RED                (PORTD_BIT6)                          // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define LED_BLUE               (PORTD_BIT7)                          // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define DEMO_LED_1             (LED_GREEN)                           // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define DEMO_LED_2             (LED_ORANGE)                          // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define DEMO_LED_3             (LED_RED)                             // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define DEMO_LED_4             (LED_BLUE)                            // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define BLINK_LED              (DEMO_LED_1)
    #define SWITCH_2               (PORTC_BIT7)                          // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define SWITCH_3               (PORTC_BIT8)                          // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define SD_CARD_DETECTION      (PORTC_BIT18)
    #define WRITE_PROTECT_INPUT    (PORTC_BIT19)

    #define SWITCH_2_PORT          _PORTC
    #define SWITCH_3_PORT          _PORTC

    #if defined USE_MAINTENANCE
        #define INIT_WATCHDOG_LED()                                      // let the port set up do this (the user can disable blinking)
    #else
        #define INIT_WATCHDOG_LED() _CONFIG_DRIVE_PORT_OUTPUT_VALUE(D, (BLINK_LED), (BLINK_LED), (PORT_SRE_SLOW | PORT_DSE_HIGH))
    #endif

    #define SHIFT_DEMO_LED_1        4                                    // since the port bits may be spread out shift each to the lowest 4 bits
    #define SHIFT_DEMO_LED_2        4
    #define SHIFT_DEMO_LED_3        4
    #define SHIFT_DEMO_LED_4        4

    #define MAPPED_DEMO_LED_1       (DEMO_LED_1 >> SHIFT_DEMO_LED_1)
    #define MAPPED_DEMO_LED_2       (DEMO_LED_2 >> SHIFT_DEMO_LED_2)
    #define MAPPED_DEMO_LED_3       (DEMO_LED_3 >> SHIFT_DEMO_LED_3)
    #define MAPPED_DEMO_LED_4       (DEMO_LED_4 >> SHIFT_DEMO_LED_4)

    #define INIT_WATCHDOG_DISABLE() _CONFIG_PORT_INPUT_FAST_LOW(C, SWITCH_2, PORT_PS_UP_ENABLE) // use fast access version (beware that this can only operate on half of the 32 bits at a time)
    #define WATCHDOG_DISABLE()      (_READ_PORT_MASK(C, SWITCH_2) == 0)  // pull this input down to disable watchdog (hold SW2 at reset)
    #define ACTIVATE_WATCHDOG()     UNLOCK_WDOG(); WDOG_TOVALL = (2000/5); WDOG_TOVALH = 0; WDOG_STCTRLH = (WDOG_STCTRLH_STNDBYEN | WDOG_STCTRLH_WAITEN | WDOG_STCTRLH_STOPEN | WDOG_STCTRLH_WDOGEN) // watchdog enabled to generate reset on 2s timeout (no further updates allowed)
    #define TOGGLE_WATCHDOG_LED()   _TOGGLE_PORT(D, BLINK_LED)

    #define CONFIG_TEST_OUTPUT()                                         // we use DEMO_LED_2 which is configured by the user code (and can be disabled in parameters if required)
    #define TOGGLE_TEST_OUTPUT()    _TOGGLE_PORT(D, DEMO_LED_2)
    #define SET_TEST_OUTPUT()       _SETBITS(D, DEMO_LED_2)
    #define CLEAR_TEST_OUTPUT()     _CLEARBITS(D, DEMO_LED_2)

    #define CONFIGURE_MOUSE_INPUTS() _CONFIG_PORT_INPUT_FAST_LOW(C, SWITCH_2, PORT_PS_UP_ENABLE); _CONFIG_PORT_INPUT_FAST_LOW(C, SWITCH_3, PORT_PS_UP_ENABLE)
    #define MOUSE_LEFT_CLICK()     0
    #define MOUSE_UP()             (_READ_PORT_MASK(C, SWITCH_2) == 0)   // press this button to move mouse up
    #define MOUSE_DOWN()           (_READ_PORT_MASK(C, SWITCH_3) == 0)   // press this button to move mouse down
    #define MOUSE_LEFT()           0                                     // not used
    #define MOUSE_RIGHT()          0                                     // not used

    #define BUTTON_KEY_DEFINITIONS  {SWITCH_2_PORT, SWITCH_2, {273, 245, 289, 261}}, \
                                    {SWITCH_3_PORT, SWITCH_3, {312, 245, 327, 261}}


        // '0'          '1'           input state   center (x,   y)   0 = circle, radius, controlling port, controlling pin 
    #define KEYPAD_LED_DEFINITIONS  \
        {RGB(0,   255, 255), RGB(60,60,60),  1, {344, 187, 351, 192 }, _PORTD, LED_GREEN}, \
        {RGB(255, 165, 0  ), RGB(60,60,60),  1, {344, 198, 351, 203 }, _PORTD, LED_ORANGE}, \
        {RGB(255, 0,   0  ), RGB(60,60,60),  1, {344, 208, 351, 213 }, _PORTD, LED_RED}, \
        {RGB(0,   0,   255), RGB(60,60,60),  1, {344, 217, 351, 222 }, _PORTD, LED_BLUE}

    #define KEYPAD "KeyPads/TWR_K21D50M.bmp"
#elif defined TEENSY_3_1
    #define DEMO_LED_1             (PORTC_BIT5)                          // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define DEMO_LED_2             (PORTC_BIT6)                          // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define DEMO_LED_3             (PORTC_BIT7)                          // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define DEMO_LED_4             (PORTC_BIT8)                          // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define BLINK_LED              (DEMO_LED_1)
    #define SWITCH_1               (PORTD_BIT5)                          // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define SWITCH_2               (PORTD_BIT6)                          // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too

    #define SWITCH_1_PORT          _PORTD
    #define SWITCH_2_PORT          _PORTD

    #if defined USE_MAINTENANCE
        #define INIT_WATCHDOG_LED()                                      // let the port set up do this (the user can disable blinking)
    #else
        #define INIT_WATCHDOG_LED() _CONFIG_DRIVE_PORT_OUTPUT_VALUE(C, (BLINK_LED), (BLINK_LED), (PORT_SRE_SLOW | PORT_DSE_HIGH))
    #endif

    #define SHIFT_DEMO_LED_1        5                                    // since the port bits are spread out shift each to the lowest 4 bits
    #define SHIFT_DEMO_LED_2        5
    #define SHIFT_DEMO_LED_3        5
    #define SHIFT_DEMO_LED_4        5

    #define MAPPED_DEMO_LED_1       (DEMO_LED_1 >> SHIFT_DEMO_LED_1)
    #define MAPPED_DEMO_LED_2       (DEMO_LED_2 >> SHIFT_DEMO_LED_2)
    #define MAPPED_DEMO_LED_3       (DEMO_LED_3 >> SHIFT_DEMO_LED_3)
    #define MAPPED_DEMO_LED_4       (DEMO_LED_4 >> SHIFT_DEMO_LED_4)

    #define INIT_WATCHDOG_DISABLE() _CONFIG_PORT_INPUT_FAST_LOW(D, (SWITCH_1 | SWITCH_2), PORT_PS_UP_ENABLE) // use fast access version (beware that this can only operate on half of the 32 bits at a time)
    #define WATCHDOG_DISABLE()      (_READ_PORT_MASK(D, SWITCH_2) == 0)  // pull this input down to disable watchdog (pull connector pin pin 21 [A7] to ground)
    #define ACTIVATE_WATCHDOG()     UNLOCK_WDOG(); WDOG_TOVALL = (2000/5); WDOG_TOVALH = 0; WDOG_STCTRLH = (WDOG_STCTRLH_STNDBYEN | WDOG_STCTRLH_WAITEN | WDOG_STCTRLH_STOPEN | WDOG_STCTRLH_WDOGEN) // watchdog enabled to generate reset on 2s timeout (no further updates allowed)
    #define TOGGLE_WATCHDOG_LED()   _TOGGLE_PORT(C, BLINK_LED)

    #define CONFIG_TEST_OUTPUT()                                         // we use DEMO_LED_2 which is configured by the user code (and can be disabled in parameters if required)
    #define TOGGLE_TEST_OUTPUT()    _TOGGLE_PORT(C, DEMO_LED_2)
    #define SET_TEST_OUTPUT()       _SETBITS(C, DEMO_LED_2)
    #define CLEAR_TEST_OUTPUT()     _CLEARBITS(C, DEMO_LED_2)

    #define CONFIGURE_MOUSE_INPUTS() _CONFIG_PORT_INPUT_FAST_LOW(C, SWITCH_1, PORT_PS_UP_ENABLE); _CONFIG_PORT_INPUT_FAST_LOW(C, SWITCH_2, PORT_PS_UP_ENABLE)
    #define MOUSE_LEFT_CLICK()     0
    #define MOUSE_UP()             (_READ_PORT_MASK(C, SWITCH_1) == 0)   // press this button to move mouse up
    #define MOUSE_DOWN()           (_READ_PORT_MASK(C, SWITCH_2) == 0)   // press this button to move mouse down
    #define MOUSE_LEFT()           0                                     // not used
    #define MOUSE_RIGHT()          0                                     // not used

    #define BUTTON_KEY_DEFINITIONS  {_PORTB,        PORTB_BIT16, {340, 14,  353, 27 }}, \
                                    {_PORTB,        PORTB_BIT17, {311, 14,  328, 27 }}, \
                                    {_PORTD,        PORTD_BIT0,  {282, 14,  300, 27 }}, \
                                    {_PORTA,        PORTA_BIT12, {256, 14,  272, 27 }}, \
                                    {_PORTA,        PORTA_BIT13, {228, 14,  246, 27 }}, \
                                    {_PORTD,        PORTD_BIT7,  {200, 14,  217, 27 }}, \
                                    {_PORTD,        PORTD_BIT4,  {172, 14,  189, 27 }}, \
                                    {_PORTD,        PORTD_BIT2,  {145, 14,  161, 27 }}, \
                                    {_PORTD,        PORTD_BIT3,  {118, 14,  134, 27 }}, \
                                    {_PORTC,        PORTC_BIT3,  {91,  14,  106, 27 }}, \
                                    {_PORTC,        PORTC_BIT4,  {64,  14,  78,  27 }}, \
                                    {_PORTC,        PORTC_BIT6,  {36,  14,  52,  27 }}, \
                                    {_PORTC,        PORTC_BIT7,  {9,   14,  24,  27 }}, \
                                    {_PORTD,        PORTD_BIT1,  {38,  182, 51,  192}}, \
                                    {_PORTC,        PORTC_BIT0,  {64,  182, 80,  192}}, \
                                    {_PORTB,        PORTB_BIT0,  {92,  182, 106, 192}}, \
                                    {_PORTB,        PORTB_BIT1,  {120, 182, 134, 192}}, \
                                    {_PORTB,        PORTB_BIT3,  {146, 182, 162, 192}}, \
                                    {_PORTB,        PORTB_BIT2,  {175, 182, 189, 192}}, \
                                    {SWITCH_1_PORT, SWITCH_1,    {203, 182, 214, 192}}, \
                                    {SWITCH_2_PORT, SWITCH_2,    {230, 182, 245, 192}}, \
                                    {_PORTC,        PORTC_BIT1,  {258, 182, 271, 192}}, \
                                    {_PORTC,        PORTC_BIT2,  {284, 182, 299, 192}}, \
                                    {_PORTC,        PORTC_BIT10, {104, 215, 114, 240}}, \
                                    {_PORTC,        PORTC_BIT11, {130, 215, 143, 240}}, \
                                    {_PORTE,        PORTE_BIT0,  {159, 215, 171, 240}}, \
                                    {_PORTB,        PORTB_BIT18, {185, 215, 199, 240}}, \
                                    {_PORTA,        PORTA_BIT4,  {214, 215, 226, 240}}, \
                                    {_PORTC,        PORTC_BIT8,  {104, 275, 114, 301}}, \
                                    {_PORTC,        PORTC_BIT9,  {130, 275, 143, 301}}, \
                                    {_PORTE,        PORTE_BIT1,  {159, 275, 171, 301}}, \
                                    {_PORTB,        PORTB_BIT19, {185, 275, 199, 301}}, \
                                    {_PORTA,        PORTA_BIT5,  {214, 275, 226, 301}}

                                    

        // '0'          '1'                   input state   center (x,   y)   0 = circle, radius, controlling port, controlling pin 
    #define KEYPAD_LED_DEFINITIONS  \
        {RGB(40,40,40), RGB(255,0,0),         0, {37, 162, 57, 171 }, _PORTC, BLINK_LED}, \
        {RGB(255,255,255), RGB(100,100,100),  0, {17, 186, 0,  6   }, _PORTC, BLINK_LED}

    #define KEYPAD "KeyPads/teensy3_1.bmp"

    // Defines for the WAVE file recording start/stop button
    //
    #define WAVE_DISK_START_STOP_INT_PRIORITY    PRIORITY_PORT_D_INT
    #define WAVE_DISK_START_STOP_INT_PORT        PORTD
    #define WAVE_DISK_START_STOP_INT_BIT         PORTD_BIT5
    #define RECORDER_WAVE_FILE_NAME              "Teensy_ADC.wav"

    #if defined ENC424J600_INTERFACE                                     // if the ENC424J600 is being used as SPI connected Ethernet  controller
        #define ENC424J600_IRQ       PORTC_BIT3                          // the IRQ line used
        #define ENC424J600_IRQ_PORT  PORTC                               // the port that the IRQ line is on
        #define ENC424J600_IRQ_PRIORITY PRIORITY_PORT_C_INT              // the interrupt priority
    #endif
#elif defined K20FX512_120
    // Unused ports are initialised as weak outputs, driving '0'
    // Port A - pins 59, 66, 67, 68, 69, 79, 80
    // Port B - pins 81..88, 95..98
    // Port C - pins 103, 104, 105, 106, 125, 126
    // Port D - pins 110, 111, 112, 113, 114, 115, 116, 127, 128, 129, 131, 137, 138, 132, 133, 136, 
    // Port E - pins 9, 10, 15, 45, 46, 47, 48
    //
    #define INITIALISE_UNUSED_PORTS()  _CONFIG_DRIVE_PORT_OUTPUT_VALUE_FAST_LOW(A, (PORTA_BIT7 | PORTA_BIT14 | PORTA_BIT15), 0, (PORT_SRE_SLOW | PORT_DSE_LOW)); \
                                       _CONFIG_DRIVE_PORT_OUTPUT_VALUE_FAST_HIGH(A, (PORTA_BIT16 | PORTA_BIT17 | PORTA_BIT28 | PORTA_BIT29), 0, (PORT_SRE_SLOW | PORT_DSE_LOW)); \
                                       _CONFIG_DRIVE_PORT_OUTPUT_VALUE_FAST_LOW(B, (PORTB_BIT0 | PORTB_BIT1 | PORTB_BIT2 | PORTB_BIT3 | PORTB_BIT4 | PORTB_BIT5 | PORTB_BIT6 | PORTB_BIT7), 0, (PORT_SRE_SLOW | PORT_DSE_LOW)); \
                                       _CONFIG_DRIVE_PORT_OUTPUT_VALUE_FAST_HIGH(B, (PORTB_BIT16 | PORTB_BIT17 | PORTB_BIT18 | PORTB_BIT19), 0, (PORT_SRE_SLOW | PORT_DSE_LOW)); \
                                       _CONFIG_DRIVE_PORT_OUTPUT_VALUE_FAST_LOW(C, (PORTC_BIT0 | PORTC_BIT1 | PORTC_BIT2 | PORTC_BIT5 | PORTC_BIT6 | PORTC_BIT7 | PORTC_BIT8 | PORTC_BIT9 | PORTC_BIT10 | PORTC_BIT11), 0, (PORT_SRE_SLOW | PORT_DSE_LOW)); \
                                       _CONFIG_DRIVE_PORT_OUTPUT_VALUE_FAST_HIGH(C, (PORTC_BIT18 | PORTC_BIT19), 0, (PORT_SRE_SLOW | PORT_DSE_LOW)); \
                                       _CONFIG_DRIVE_PORT_OUTPUT_VALUE_FAST_LOW(D, (PORTD_BIT0 | PORTD_BIT1 |PORTD_BIT4 | PORTD_BIT5 | PORTD_BIT6 | PORTD_BIT7 | PORTD_BIT8 | PORTD_BIT9 | PORTD_BIT10 | PORTD_BIT11 | PORTD_BIT12 | PORTD_BIT13 | PORTD_BIT14 | PORTD_BIT15), 0, (PORT_SRE_SLOW | PORT_DSE_LOW)); \
                                       _CONFIG_DRIVE_PORT_OUTPUT_VALUE_FAST_LOW(E, (PORTE_BIT6 | PORTE_BIT7 | PORTE_BIT12), 0, (PORT_SRE_SLOW | PORT_DSE_LOW)); \
                                       _CONFIG_DRIVE_PORT_OUTPUT_VALUE_FAST_HIGH(E, (PORTE_BIT24 | PORTE_BIT25 | PORTE_BIT26 | PORTE_BIT27), 0, (PORT_SRE_SLOW | PORT_DSE_LOW))

    // Initialise USB host enable, USB host DT and current enable'0'
    //
    #define USB_HOST_ENABLE        PORTA_BIT10
    #define USB_HOST_DT            PORTA_BIT9
    #define BT_CONNECTED           PORTA_BIT8                            // input
    #define CURRENT_ENABLE         PORTA_BIT6
    #define INITIALISE_APPLICATION_PORTS() _CONFIG_DRIVE_PORT_OUTPUT_VALUE_FAST_LOW(A, (USB_HOST_ENABLE | USB_HOST_DT | CURRENT_ENABLE), 0, (PORT_SRE_SLOW | PORT_DSE_LOW)); \
                                           _CONFIG_PORT_INPUT_FAST_LOW(A, (BT_CONNECTED), (PORT_PS_UP_ENABLE))

    #define RED_LED                (PORTA_BIT24)
    #define GREEN_LED              (PORTA_BIT25)
    #define BLUE_LED               (PORTA_BIT26)

    #define DEMO_LED_1             (GREEN_LED)
    #define DEMO_LED_2             (RED_LED)
    #define DEMO_LED_3             (BLUE_LED)
    #define DEMO_LED_4             (0)
  
    #define BLINK_LED              (GREEN_LED)
    #define SWITCH_1               (PORTA_BIT27)

    #define SWITCH_1_PORT          _PORTA

    #if defined USE_MAINTENANCE
        #define INIT_WATCHDOG_LED()                                      // let the port set up do this (the user can disable blinking)
    #else
        #define INIT_WATCHDOG_LED() _CONFIG_DRIVE_PORT_OUTPUT_VALUE(A, (BLINK_LED), (BLINK_LED), (PORT_SRE_SLOW | PORT_DSE_HIGH))
    #endif

    #define SHIFT_DEMO_LED_1        24                                    // since the port bits are spread out shift each to the lowest 4 bits
    #define SHIFT_DEMO_LED_2        24
    #define SHIFT_DEMO_LED_3        24

    #define MAPPED_DEMO_LED_1       (DEMO_LED_1 >> SHIFT_DEMO_LED_1)
    #define MAPPED_DEMO_LED_2       (DEMO_LED_2 >> SHIFT_DEMO_LED_2)
    #define MAPPED_DEMO_LED_3       (DEMO_LED_3 >> SHIFT_DEMO_LED_3)
    #define MAPPED_DEMO_LED_4       0

    #define INIT_WATCHDOG_DISABLE() _CONFIG_PORT_INPUT_FAST_HIGH(A, SWITCH_1, PORT_PS_UP_ENABLE) // use fast access version (beware that this can only operate on half of the 32 bits at a time)
    #define WATCHDOG_DISABLE()      (_READ_PORT_MASK(A, SWITCH_1) == 0)  // pull this input down to disable watchdog (hold SW2 at reset)
    #define ACTIVATE_WATCHDOG()     UNLOCK_WDOG(); WDOG_TOVALL = 2000; WDOG_TOVALH = 0; WDOG_STCTRLH = (WDOG_STCTRLH_STNDBYEN | WDOG_STCTRLH_WAITEN | WDOG_STCTRLH_STOPEN | WDOG_STCTRLH_WDOGEN); // watchdog enabled to generate reset on 2s timeout (no further updates allowed)
    #define TOGGLE_WATCHDOG_LED()   _TOGGLE_PORT(A, BLINK_LED)

    #define CONFIG_TEST_OUTPUT()                                         // we use DEMO_LED_2 which is configured by the user code (and can be disabled in parameters if required)
    #define TOGGLE_TEST_OUTPUT()    _TOGGLE_PORT(A, DEMO_LED_2)
    #define SET_TEST_OUTPUT()       _SETBITS(A, DEMO_LED_2)
    #define CLEAR_TEST_OUTPUT()     _CLEARBITS(A, DEMO_LED_2)

    #define CONFIGURE_MOUSE_INPUTS() 
    #define MOUSE_LEFT_CLICK()     0
    #define MOUSE_UP()             (_READ_PORT_MASK(A, SWITCH_1) == 0)   // press this button to move mouse up
    #define MOUSE_DOWN()           0                                     // not used
    #define MOUSE_LEFT()           0                                     // not used
    #define MOUSE_RIGHT()          0                                     // not used

    #define BUTTON_KEY_DEFINITIONS  {SWITCH_1_PORT, SWITCH_1, {282, 135, 291, 152}}

                                    // '0'            '1'     input state center (x,   y)   0 = circle, radius, controlling port, controlling pin 
    #define KEYPAD_LED_DEFINITIONS  {RGB(160,160,200),RGB(0,255,0), 0,  {164, 229, 0, 13 }, _PORTA, GREEN_LED}

    #define KEYPAD "KeyPads/K20FX512_120.bmp"
#elif defined TWR_K20D50M || defined TWR_K20D72M                         // {2}
    #define DEMO_LED_1             (PORTC_BIT7)                          // (green LED) if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define DEMO_LED_2             (PORTC_BIT8)                          // (blue LED) if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #if defined USB_HOST_SUPPORT
        #define DEMO_LED_3         (0)                                   // use the port for USB host power control and not LED
    #else
        #define DEMO_LED_3         (PORTC_BIT9)                          // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #endif
    #define DEMO_LED_4             (PORTC_BIT10)                         // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define BLINK_LED              (DEMO_LED_1)
    #define SWITCH_2               (PORTC_BIT1)                          // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define SWITCH_3               (PORTC_BIT2)                          // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define USB_HOST_POWER_ENABLE  (PORTC_BIT9)

    #define SWITCH_2_PORT          _PORTC
    #define SWITCH_3_PORT          _PORTC

    #if defined USE_MAINTENANCE
        #define INIT_WATCHDOG_LED()                                      // let the port set up do this (the user can disable blinking)
    #else
        #define INIT_WATCHDOG_LED() _CONFIG_DRIVE_PORT_OUTPUT_VALUE(C, (BLINK_LED), (BLINK_LED), (PORT_SRE_SLOW | PORT_DSE_HIGH))
    #endif

    #define SHIFT_DEMO_LED_1        7                                    // since the port bits are spread out shift each to the lowest 4 bits
    #define SHIFT_DEMO_LED_2        7
    #define SHIFT_DEMO_LED_3        7
    #define SHIFT_DEMO_LED_4        7

    #define MAPPED_DEMO_LED_1       (DEMO_LED_1 >> SHIFT_DEMO_LED_1)
    #define MAPPED_DEMO_LED_2       (DEMO_LED_2 >> SHIFT_DEMO_LED_2)
    #define MAPPED_DEMO_LED_3       (DEMO_LED_3 >> SHIFT_DEMO_LED_3)
    #define MAPPED_DEMO_LED_4       (DEMO_LED_4 >> SHIFT_DEMO_LED_4)

    #define INIT_WATCHDOG_DISABLE() _CONFIG_PORT_INPUT_FAST_LOW(C, SWITCH_3, PORT_PS_UP_ENABLE) // use fast access version (beware that this can only operate on half of the 32 bits at a time)
    #define WATCHDOG_DISABLE()      (_READ_PORT_MASK(C, SWITCH_3) == 0)  // pull this input down to disable watchdog (hold SW2 at reset)
    #define ACTIVATE_WATCHDOG()     UNLOCK_WDOG(); WDOG_TOVALL = (2000/5); WDOG_TOVALH = 0; WDOG_STCTRLH = (WDOG_STCTRLH_STNDBYEN | WDOG_STCTRLH_WAITEN | WDOG_STCTRLH_STOPEN | WDOG_STCTRLH_WDOGEN) // watchdog enabled to generate reset on 2s timeout (no further updates allowed)
    #define TOGGLE_WATCHDOG_LED()   _TOGGLE_PORT(C, BLINK_LED)

    #define CONFIG_TEST_OUTPUT()                                         // we use DEMO_LED_2 which is configured by the user code (and can be disabled in parameters if required)
    #define TOGGLE_TEST_OUTPUT()    _TOGGLE_PORT(C, DEMO_LED_2)
    #define SET_TEST_OUTPUT()       _SETBITS(C, DEMO_LED_2)
    #define CLEAR_TEST_OUTPUT()     _CLEARBITS(C, DEMO_LED_2)

    #define CONFIGURE_MOUSE_INPUTS() _CONFIG_PORT_INPUT_FAST_LOW(C, SWITCH_2, PORT_PS_UP_ENABLE); _CONFIG_PORT_INPUT_FAST_LOW(C, SWITCH_3, PORT_PS_UP_ENABLE)
    #define MOUSE_LEFT_CLICK()     0
    #define MOUSE_UP()             (_READ_PORT_MASK(C, SWITCH_2) == 0)   // press this button to move mouse up
    #define MOUSE_DOWN()           (_READ_PORT_MASK(C, SWITCH_3) == 0)   // press this button to move mouse down
    #define MOUSE_LEFT()           0                                     // not used
    #define MOUSE_RIGHT()          0                                     // not used

    #define USB_HOST_POWER_CONFIG()  _CONFIG_DRIVE_PORT_OUTPUT_VALUE_FAST_LOW(C, (USB_HOST_POWER_ENABLE), (0), (PORT_SRE_SLOW | PORT_DSE_HIGH))
    #define USB_HOST_POWER_ON()      _SETBITS(C, USB_HOST_POWER_ENABLE)
    #define USB_HOST_POWER_OFF()     _CLEARBITS(C, USB_HOST_POWER_ENABLE)

    #if defined TWR_K20D72M
        #define KEYPAD "KeyPads/TWR_K20D72M.bmp"
                                        // '0'            '1'       input state   center (x,   y)   0 = circle, radius, controlling port, controlling pin 
        #define KEYPAD_LED_DEFINITIONS  {RGB(160,160,200),RGB(0,0,255),   0, {12,  116, 19,  121 }, _PORTC, DEMO_LED_2}, \
                                        {RGB(160,160,200),RGB(0,255,0),   0, {12,  123, 19,  128 }, _PORTC, DEMO_LED_1}, \
                                        {RGB(200,200,200),RGB(255,75,0),  0, {225, 90,  230, 97  }, _PORTB, PORTB_BIT1}, \
                                        {RGB(200,200,200),RGB(255,255,0), 0, {225, 114, 230, 122 }, _PORTB, PORTB_BIT0}


        #define BUTTON_KEY_DEFINITIONS  {SWITCH_2_PORT, SWITCH_2, {166, 41, 177, 56}}, \
                                        {SWITCH_3_PORT, SWITCH_3, {194, 41, 208, 56}}


    #else
                                        // '0'            '1'     input state   center (x,   y)   0 = circle, radius, controlling port, controlling pin 
        #define KEYPAD_LED_DEFINITIONS  {RGB(160,160,200),RGB(0,255,0),   0, {393, 106, 402, 112 }, _PORTC, DEMO_LED_1}, \
                                        {RGB(160,160,200),RGB(255,0,0),   0, {393, 114, 402, 120 }, _PORTC, DEMO_LED_2}, \
                                        {RGB(200,200,200),RGB(255,75,0),  0, {386, 141, 394, 149 }, _PORTC, DEMO_LED_3}, \
                                        {RGB(200,200,200),RGB(255,255,0), 0, {386, 183, 394, 191 }, _PORTC, DEMO_LED_4}


        #define BUTTON_KEY_DEFINITIONS  {SWITCH_2_PORT, SWITCH_2, {30, 209, 52, 225}}, \
                                        {SWITCH_3_PORT, SWITCH_3, {30, 237, 52, 252}}

        #define KEYPAD "KeyPads/TWR_K20.bmp"
    #endif
#elif defined FRDM_K20D50M                                               // {21}
    #define DEMO_LED_1             (PORTD_BIT4)                          // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define DEMO_LED_2             (PORTC_BIT3)                          // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define DEMO_LED_3             (PORTA_BIT2)                          // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define DEMO_LED_4             (0)                                   // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define BLINK_LED              (DEMO_LED_1)
    #define SWITCH_1               (PORTE_BIT1)                          // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define SWITCH_2               (PORTE_BIT0)                          // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too

    #if defined USE_MAINTENANCE
        #define INIT_WATCHDOG_LED()                                      // let the port set up do this (the user can disable blinking)
    #else
        #define INIT_WATCHDOG_LED() _CONFIG_DRIVE_PORT_OUTPUT_VALUE(D, (BLINK_LED), (BLINK_LED), (PORT_SRE_SLOW | PORT_DSE_HIGH))
    #endif

    #define SHIFT_DEMO_LED_1        4                                    // since the port bits are spread out shift each to the lowest 4 bits
    #define SHIFT_DEMO_LED_2        2
    #define SHIFT_DEMO_LED_3        0
    #define SHIFT_DEMO_LED_4        0

    #define MAPPED_DEMO_LED_1       (DEMO_LED_1 >> SHIFT_DEMO_LED_1)
    #define MAPPED_DEMO_LED_2       (DEMO_LED_2 >> SHIFT_DEMO_LED_2)
    #define MAPPED_DEMO_LED_3       (DEMO_LED_3 >> SHIFT_DEMO_LED_3)
    #define MAPPED_DEMO_LED_4       (DEMO_LED_4 >> SHIFT_DEMO_LED_4)

    #define INIT_WATCHDOG_DISABLE() _CONFIG_PORT_INPUT_FAST_LOW(E, SWITCH_2, PORT_PS_UP_ENABLE) // use fast access version (beware that this can only operate on half of the 32 bits at a time)
    #define WATCHDOG_DISABLE()      (_READ_PORT_MASK(E, SWITCH_2) == 0)      // pull this input down to disable watchdog (connect J19-4 to GND at reset)
    #define ACTIVATE_WATCHDOG()     UNLOCK_WDOG(); WDOG_TOVALL = (2000/5); WDOG_TOVALH = 0; WDOG_STCTRLH = (WDOG_STCTRLH_STNDBYEN | WDOG_STCTRLH_WAITEN | WDOG_STCTRLH_STOPEN | WDOG_STCTRLH_WDOGEN) // watchdog enabled to generate reset on 2s timeout (no further updates allowed)
    #define TOGGLE_WATCHDOG_LED()   _TOGGLE_PORT(D, BLINK_LED)

    #define CONFIG_TEST_OUTPUT()                                         // we use DEMO_LED_2 which is configured by the user code (and can be disabled in parameters if required)
    #define TOGGLE_TEST_OUTPUT()    _TOGGLE_PORT(C, DEMO_LED_2)
    #define SET_TEST_OUTPUT()       _SETBITS(C, DEMO_LED_2)
    #define CLEAR_TEST_OUTPUT()     _CLEARBITS(C, DEMO_LED_2)

    #define CONFIGURE_MOUSE_INPUTS() 
    #define MOUSE_LEFT_CLICK()     0
    #define MOUSE_UP()             0
    #define MOUSE_DOWN()           0
    #define MOUSE_LEFT()           0
    #define MOUSE_RIGHT()          0

    #define MULTICOLOUR_LEDS        {0, 2}                               // single LED made up of entries 0, 1 and 2

                                    // '0'            '1'     input state   center (x,   y)   0 = circle, radius, controlling port, controlling pin 
    #define KEYPAD_LED_DEFINITIONS  {RGB(0,255,0),    RGB(0,0,0), 1, {66, 115, 0, 7 }, _PORTD, DEMO_LED_1}, \
                                    {RGB(255,0,0),    RGB(0,0,0), 1, {66, 115, 0, 7 }, _PORTC, DEMO_LED_2}, \
                                    {RGB(0,0,255),    RGB(0,0,0), 1, {66, 115, 0, 7 }, _PORTA, DEMO_LED_3}

    #define KEYPAD "KeyPads/FRDM_K20D50M.bmp"

    // Accelerometer interrupt configuration
    //
    #define ACC_INT_PRIORITY       PRIORITY_PORT_C_INT
    #define ACC_INT_PORT           PORTC
    #define ACC_INT_BIT            PORTC_BIT6
    #define ACC_INT_ASSERTED()     (_READ_PORT_MASK(C, PORTC_BIT6) == 0)
#elif defined tinyK20
    #define DEMO_LED_1             (PORTD_BIT4)                          // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define DEMO_LED_2             (PORTD_BIT5)                          // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define DEMO_LED_3             (PORTD_BIT6)                          // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define DEMO_LED_4             (PORTD_BIT7)                          // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define BLINK_LED              (DEMO_LED_1)
    #define SWITCH_1               (PORTB_BIT0)                          // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too

    #if defined USE_MAINTENANCE
        #define INIT_WATCHDOG_LED()                                      // let the port set up do this (the user can disable blinking)
    #else
        #define INIT_WATCHDOG_LED() _CONFIG_DRIVE_PORT_OUTPUT_VALUE(D, (BLINK_LED), (BLINK_LED), (PORT_SRE_SLOW | PORT_DSE_HIGH))
    #endif

    #define SHIFT_DEMO_LED_1        4                                    // since the port bits are spread out shift each to the lowest 4 bits
    #define SHIFT_DEMO_LED_2        4
    #define SHIFT_DEMO_LED_3        4
    #define SHIFT_DEMO_LED_4        4

    #define MAPPED_DEMO_LED_1       (DEMO_LED_1 >> SHIFT_DEMO_LED_1)
    #define MAPPED_DEMO_LED_2       (DEMO_LED_2 >> SHIFT_DEMO_LED_2)
    #define MAPPED_DEMO_LED_3       (DEMO_LED_3 >> SHIFT_DEMO_LED_3)
    #define MAPPED_DEMO_LED_4       (DEMO_LED_4 >> SHIFT_DEMO_LED_4)

    #define INIT_WATCHDOG_DISABLE() _CONFIG_PORT_INPUT_FAST_LOW(B, SWITCH_1, PORT_PS_UP_ENABLE) // use fast access version (beware that this can only operate on half of the 32 bits at a time)
    #define WATCHDOG_DISABLE()      (_READ_PORT_MASK(B, SWITCH_1) == 0)  // pull this input down to disable watchdog (connect J19-4 to GND at reset)
    #define ACTIVATE_WATCHDOG()     UNLOCK_WDOG(); WDOG_TOVALL = (2000/5); WDOG_TOVALH = 0; WDOG_STCTRLH = (WDOG_STCTRLH_STNDBYEN | WDOG_STCTRLH_WAITEN | WDOG_STCTRLH_STOPEN | WDOG_STCTRLH_WDOGEN) // watchdog enabled to generate reset on 2s timeout (no further updates allowed)
    #define TOGGLE_WATCHDOG_LED()   _TOGGLE_PORT(D, BLINK_LED)

    #define CONFIG_TEST_OUTPUT()                                         // we use DEMO_LED_2 which is configured by the user code (and can be disabled in parameters if required)
    #define TOGGLE_TEST_OUTPUT()    _TOGGLE_PORT(D, DEMO_LED_2)
    #define SET_TEST_OUTPUT()       _SETBITS(D, DEMO_LED_2)
    #define CLEAR_TEST_OUTPUT()     _CLEARBITS(D, DEMO_LED_2)

    #define CONFIGURE_MOUSE_INPUTS() 
    #define MOUSE_LEFT_CLICK()     0
    #define MOUSE_UP()             0
    #define MOUSE_DOWN()           0
    #define MOUSE_LEFT()           0
    #define MOUSE_RIGHT()          0

                                    // '0'            '1'          input state   center (x,   y)   0 = circle, radius, controlling port, controlling pin 
    #define KEYPAD_LED_DEFINITIONS  {RGB(0, 0, 255),  RGB(0,   0, 0), 1, {377, 53, 386, 64 }, _PORTD, DEMO_LED_1}, \
                                    {RGB(0, 0, 0),    RGB(255, 0, 0), 0, {51,  28, 0,   4 },  _PORTD, PORTD_BIT7}, \
                                    {RGB(0, 0, 0),    RGB(255, 0, 0), 0, {76,  28, 0,   4 },  _PORTD, PORTD_BIT6}, \
                                    {RGB(0, 0, 0),    RGB(255, 0, 0), 0, {101, 28, 0,   4 },  _PORTD, PORTD_BIT5}, \
                                    {RGB(0, 0, 0),    RGB(255, 0, 0), 0, {101, 28, 0,   4 },  _PORTD, PORTD_BIT5}, \
                                    {RGB(0, 0, 0),    RGB(255, 0, 0), 0, {125, 28, 0,   4 },  _PORTD, PORTD_BIT4}, \
                                    {RGB(0, 0, 0),    RGB(255, 0, 0), 0, {150, 28, 0,   4 },  _PORTD, PORTD_BIT3}, \
                                    {RGB(0, 0, 0),    RGB(255, 0, 0), 0, {175, 28, 0,   4 },  _PORTD, PORTD_BIT2}, \
                                    {RGB(0, 0, 0),    RGB(255, 0, 0), 0, {199, 28, 0,   4 },  _PORTD, PORTD_BIT1}, \
                                    {RGB(0, 0, 0),    RGB(255, 0, 0), 0, {224, 28, 0,   4 },  _PORTD, PORTD_BIT0}, \
                                    {RGB(0, 0, 0),    RGB(255, 0, 0), 0, {246, 28, 0,   4 },  _PORTC, PORTD_BIT7}, \
                                    {RGB(0, 0, 0),    RGB(255, 0, 0), 0, {272, 28, 0,   4 },  _PORTC, PORTD_BIT6}, \
                                    {RGB(0, 0, 0),    RGB(255, 0, 0), 0, {298, 28, 0,   4 },  _PORTC, PORTD_BIT5}, \
                                    {RGB(0, 0, 0),    RGB(255, 0, 0), 0, {321, 28, 0,   4 },  _PORTC, PORTD_BIT4}, \
                                    {RGB(0, 0, 0),    RGB(255, 0, 0), 0, {345, 28, 0,   4 },  _PORTC, PORTD_BIT3}, \
                                    {RGB(0, 0, 0),    RGB(255, 0, 0), 0, {371, 28, 0,   4 },  _PORTC, PORTD_BIT2}, \
                                    {RGB(0, 0, 0),    RGB(255, 0, 0), 0, {96,  175,0,   4 },  _PORTA, PORTA_BIT0}, \
                                    {RGB(0, 0, 0),    RGB(255, 0, 0), 0, {121, 175,0,   4 },  _PORTA, PORTA_BIT1}, \
                                    {RGB(0, 0, 0),    RGB(255, 0, 0), 0, {146, 175,0,   4 },  _PORTA, PORTA_BIT2}, \
                                    {RGB(0, 0, 0),    RGB(255, 0, 0), 0, {172, 175,0,   4 },  _PORTA, PORTA_BIT3}, \
                                    {RGB(0, 0, 0),    RGB(255, 0, 0), 0, {197, 175,0,   4 },  _PORTA, PORTA_BIT4}, \
                                    {RGB(0, 0, 0),    RGB(255, 0, 0), 0, {222, 175,0,   4 },  _PORTB, PORTB_BIT0}, \
                                    {RGB(0, 0, 0),    RGB(255, 0, 0), 0, {247, 175,0,   4 },  _PORTB, PORTB_BIT1}, \
                                    {RGB(0, 0, 0),    RGB(255, 0, 0), 0, {272, 175,0,   4 },  _PORTB, PORTB_BIT2}, \
                                    {RGB(0, 0, 0),    RGB(255, 0, 0), 0, {297, 175,0,   4 },  _PORTB, PORTB_BIT3}, \
                                    {RGB(0, 0, 0),    RGB(255, 0, 0), 0, {322, 175,0,   4 },  _PORTB, PORTB_BIT16}, \
                                    {RGB(0, 0, 0),    RGB(255, 0, 0), 0, {347, 175,0,   4 },  _PORTB, PORTB_BIT17}, \
                                    {RGB(0, 0, 0),    RGB(255, 0, 0), 0, {373, 175,0,   4 },  _PORTC, PORTC_BIT0}, \
                                    {RGB(0, 0, 0),    RGB(255, 0, 0), 0, {398, 175,0,   4 },  _PORTC, PORTC_BIT1}, \

    #define BUTTON_KEY_DEFINITIONS  {_PORTA,        PORTA_BIT0,  {89,  168, 102, 181 }}, \
                                    {_PORTA,        PORTA_BIT1,  {115, 168, 128, 181 }}, \
                                    {_PORTA,        PORTA_BIT2,  {141, 168, 154, 181 }}, \
                                    {_PORTA,        PORTA_BIT3,  {166, 168, 179, 181 }}, \
                                    {_PORTA,        PORTA_BIT4,  {192, 168, 205, 181 }}, \
                                    {_PORTB,        PORTB_BIT0,  {215, 168, 230, 181 }}, \
                                    {_PORTB,        PORTB_BIT1,  {241, 168, 255, 181 }}, \
                                    {_PORTB,        PORTB_BIT2,  {265, 168, 279, 181 }}, \
                                    {_PORTB,        PORTB_BIT3,  {290, 168, 305, 181 }}, \
                                    {_PORTB,        PORTB_BIT16, {316, 168, 330, 181 }}, \
                                    {_PORTB,        PORTB_BIT17, {340, 168, 355, 181 }}, \
                                    {_PORTC,        PORTC_BIT0,  {366, 168, 379, 181 }}, \
                                    {_PORTC,        PORTC_BIT1,  {392, 168, 404, 181 }}, \
                                    {_PORTD,        PORTD_BIT7,  {43,  23,  57,  36 }}, \
                                    {_PORTD,        PORTD_BIT6,  {68,  23,  83,  36 }}, \
                                    {_PORTD,        PORTD_BIT5,  {93,  23,  107, 36 }}, \
                                    {_PORTD,        PORTD_BIT4,  {117, 23,  132, 36 }}, \
                                    {_PORTD,        PORTD_BIT3,  {142, 23,  155, 36 }}, \
                                    {_PORTD,        PORTD_BIT2,  {167, 23,  180, 36 }}, \
                                    {_PORTD,        PORTD_BIT1,  {193, 23,  206, 36 }}, \
                                    {_PORTD,        PORTD_BIT0,  {216, 23,  231, 36 }}, \
                                    {_PORTC,        PORTC_BIT7,  {242, 23,  256, 36 }}, \
                                    {_PORTC,        PORTC_BIT6,  {266, 23,  280, 36 }}, \
                                    {_PORTC,        PORTC_BIT5,  {291, 23,  306, 36 }}, \
                                    {_PORTC,        PORTC_BIT4,  {313, 23,  328, 36 }}, \
                                    {_PORTC,        PORTC_BIT3,  {337, 23,  352, 36 }}, \
                                    {_PORTC,        PORTC_BIT2,  {365, 23,  377, 36 }}, \

    #define KEYPAD "KeyPads/tinyk20-board.bmp"
#elif defined FRDM_KL03Z
    #define DEMO_LED_1             (PORTB_BIT11)                         // (green LED) if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define DEMO_LED_2             (PORTB_BIT10)                         // (red LED) if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define DEMO_LED_3             (PORTB_BIT13)                         // (blue LED) if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define DEMO_LED_4             0
    #define BLINK_LED              DEMO_LED_1

    #define SWITCH_2               (PORTB_BIT0)                          // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define SWITCH_3               (PORTB_BIT5)                          // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too


    #if defined USE_MAINTENANCE
        #define INIT_WATCHDOG_LED()                                      // let the port set up do this (the user can disable blinking)
    #else
        #define INIT_WATCHDOG_LED() _CONFIG_DRIVE_PORT_OUTPUT_VALUE(B, (BLINK_LED), (BLINK_LED), (PORT_SRE_SLOW | PORT_DSE_HIGH))
    #endif

    #define SHIFT_DEMO_LED_1        11                                   // since the port bits are spread out shift each to the lowest 4 bits
    #define SHIFT_DEMO_LED_2        9
    #define SHIFT_DEMO_LED_3        11
    #define SHIFT_DEMO_LED_4        0

    #define MAPPED_DEMO_LED_1       (DEMO_LED_1 >> SHIFT_DEMO_LED_1)
    #define MAPPED_DEMO_LED_2       (DEMO_LED_2 >> SHIFT_DEMO_LED_2)
    #define MAPPED_DEMO_LED_3       (DEMO_LED_3 >> SHIFT_DEMO_LED_3)
    #define MAPPED_DEMO_LED_4       (DEMO_LED_4 >> SHIFT_DEMO_LED_4)

    #define INIT_WATCHDOG_DISABLE() _CONFIG_PORT_INPUT_FAST_LOW(B, (SWITCH_2 | SWITCH_3), PORT_PS_UP_ENABLE) // configure as input
    #define WATCHDOG_DISABLE()      (_READ_PORT_MASK(B, SWITCH_2) == 0)  // hold switch 2 down at reset to disable watchdog
    #define ACTIVATE_WATCHDOG()     SIM_COPC = (SIM_COPC_COPCLKS_1K | SIM_COPC_COPT_LONGEST) // 1.024s watchdog timeout
    #define TOGGLE_WATCHDOG_LED()   _TOGGLE_PORT(B, BLINK_LED)

    #define CONFIG_TEST_OUTPUT()                                         // we use DEMO_LED_2 which is configured by the user code (and can be disabled in parameters if required)
    #define TOGGLE_TEST_OUTPUT()    _TOGGLE_PORT(B, DEMO_LED_2)
    #define SET_TEST_OUTPUT()       _SETBITS(B, DEMO_LED_2)
    #define CLEAR_TEST_OUTPUT()     _CLEARBITS(B, DEMO_LED_2)

    #define MULTICOLOUR_LEDS        {0, 2}                               // single LED made up of entries 0, 1 and 2 [green/red/blue]

        // '0'          '1'       input state   center (x,   y)   0 = circle, radius, controlling port, controlling pin 
    #define KEYPAD_LED_DEFINITIONS  \
        {RGB(0,255,0), RGB(0,0,0),     1, {288, 157, 0,   6   }, _PORTB, PORTB_BIT11}, \
        {RGB(255,0,0), RGB(0,0,0),     1, {288, 157, 0,   6   }, _PORTB, PORTB_BIT10}, \
        {RGB(0,0,255), RGB(0,0,0),     1, {288, 157, 0,   6   }, _PORTB, PORTB_BIT13}

    #define BUTTON_KEY_DEFINITIONS  {_PORTB, (SWITCH_3), {115, 294, 127, 316 }}, \
                                    {_PORTB, (SWITCH_2), {138, 294, 150, 316 }}

    #define KEYPAD "KeyPads/FRDM_KL03Z.bmp"

    // Accelerometer interrupt configuration
    //
    #define ACC_INT_PRIORITY       PRIORITY_PORT_A_INT
    #define ACC_INT_PORT           PORTA
    #define ACC_INT_BIT            PORTA_BIT12
    #define ACC_INT_ASSERTED()     (_READ_PORT_MASK(A, PORTA_BIT12) == 0)
    #define ACC_USE_INT2                                                 // INT1 is not connected on this board so use INT2

  //#define PWM_LED_CONTROL                                              // control RGB led according to accelerometer position (when I2C and accelerometer enabled)

    #define SC16IS7XX_CHIP_SELECT0  PORTB_BIT4
    #define SC16IS7XX_CHIP_SELECT1  PORTB_BIT5
    #define EXT_UART_0_1_INT_BIT    PORTB_BIT3
    #define EXT_UART_2_3_INT_BIT    PORTB_BIT2
    #define SC16IS7XX_CLOCK         48000000                             // IRC48M used to clock the external device on CLKOUT pin

    #define CONFIGURE_SPI_FOR_SC16IS7XX() SIM_SOPT2 |= (SIM_SOPT2_CLKOUTSEL_IRC48M); _CONFIG_PERIPHERAL(A, 12,  PA_12_CLKOUT); \
                                 _CONFIG_DRIVE_PORT_OUTPUT_VALUE_FAST_LOW(B, (SC16IS7XX_CHIP_SELECT0 | SC16IS7XX_CHIP_SELECT1), (SC16IS7XX_CHIP_SELECT0 | SC16IS7XX_CHIP_SELECT1), (PORT_SRE_SLOW | PORT_DSE_LOW));\
                                 _CONFIG_PERIPHERAL(B, 0,  PB_0_SPI0_SCK); \
                                 _CONFIG_PERIPHERAL(A, 7, (PA_7_SPI0_MOSI | PORT_SRE_FAST | PORT_DSE_HIGH)); \
                                 _CONFIG_PERIPHERAL(A, 6, (PA_6_SPI0_MISO | PORT_PS_UP_ENABLE)); \
                                  POWER_UP(4, SIM_SCGC4_SPI0); \
                                  SPI0_C1 = (/*SPI_C1_CPHA | SPI_C1_CPOL | */SPI_C1_MSTR | SPI_C1_SPE); \
                                  SPI0_BR = (SPI_BR_SPPR_PRE_1 | SPI_BR_SPR_DIV_4); \
                                  (unsigned char)SPI0_S; (unsigned char)SPI0_D

    #define ENABLE_CHIP_SELECT_SC16IS7XX_0()    _CLEARBITS(B, SC16IS7XX_CHIP_SELECT0)
    #define ENABLE_CHIP_SELECT_SC16IS7XX_1()    _CLEARBITS(B, SC16IS7XX_CHIP_SELECT1)
    #define DISABLE_CHIP_SELECTS_SC16IS7XX()    _SETBITS(B, (SC16IS7XX_CHIP_SELECT0 | SC16IS7XX_CHIP_SELECT1))
    #define WRITE_SC16IS7XX_SPI(byte)           SPI0_D = (byte)              // write a single byte
    #define WRITE_SC16IS7XX_SPI_LAST(byte)      SPI0_D = (byte)              // write a single (final) byte
    #if defined _WINDOWS
        #define WAIT_SC16IS7XX_SPI_RECEPTION_END()  while (!(SPI0_S & (SPI_S_SPRF))) {SPI0_S |= SPI_S_SPRF;}
    #else
        #define WAIT_SC16IS7XX_SPI_RECEPTION_END()  while (!(SPI0_S & (SPI_S_SPRF))) {}
    #endif
    #define READ_SC16IS7XX_SPI_FLASH_DATA()     (unsigned char)SPI0_D

    #define CHECK_UART_0_1_INT_PENDING()        _READ_PORT_MASK(B, EXT_UART_0_1_INT_BIT)
    #define CHECK_UART_2_3_INT_PENDING()        _READ_PORT_MASK(B, EXT_UART_2_3_INT_BIT)

    #define EXT_UART_0_1_INT_PORT               PORTB
    #define EXT_UART_2_3_INT_PORT               PORTB
    #define PRIORITY_IRQ                        PRIORITY_PORT_B_INT
#elif defined FRDM_KL02Z
    #define DEMO_LED_1             (PORTB_BIT7)                          // (green LED) if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define DEMO_LED_2             (PORTB_BIT6)                          // (red LED) if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define DEMO_LED_3             (PORTB_BIT10)                         // (blue LED) if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define DEMO_LED_4             0
    #define BLINK_LED              (DEMO_LED_1)
    #define INPUT_1                (PORTB_BIT6)                          // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define INPUT_2                (PORTB_BIT7)                          // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too

    #if defined USE_MAINTENANCE
        #define INIT_WATCHDOG_LED()                                      // let the port set up do this (the user can disable blinking)
    #else
        #define INIT_WATCHDOG_LED() _CONFIG_DRIVE_PORT_OUTPUT_VALUE(B, (BLINK_LED), (BLINK_LED), (PORT_SRE_SLOW | PORT_DSE_HIGH))
    #endif

    #define SHIFT_DEMO_LED_1        7                                    // since the port bits are spread out shift each to the lowest 4 bits
    #define SHIFT_DEMO_LED_2        5
    #define SHIFT_DEMO_LED_3        8
    #define SHIFT_DEMO_LED_4        0

    #define MAPPED_DEMO_LED_1       (DEMO_LED_1 >> SHIFT_DEMO_LED_1)
    #define MAPPED_DEMO_LED_2       (DEMO_LED_2 >> SHIFT_DEMO_LED_2)
    #define MAPPED_DEMO_LED_3       (DEMO_LED_3 >> SHIFT_DEMO_LED_3)
    #define MAPPED_DEMO_LED_4       (DEMO_LED_4 >> SHIFT_DEMO_LED_4)

    #define INIT_WATCHDOG_DISABLE() _CONFIG_PORT_INPUT_FAST_LOW(B, (INPUT_1 | INPUT_2), PORT_PS_UP_ENABLE) // configure as input
    #define WATCHDOG_DISABLE()      (_READ_PORT_MASK(B, INPUT_1) == 0)   // pull this input down at reset to disable watchdog [J8-pin 7]
    #define ACTIVATE_WATCHDOG()     SIM_COPC = (SIM_COPC_COPCLKS_1K | SIM_COPC_COPT_LONGEST) // 1.024s watchdog timeout
    #define TOGGLE_WATCHDOG_LED()   _TOGGLE_PORT(B, BLINK_LED)

    #define CONFIG_TEST_OUTPUT()                                         // we use DEMO_LED_2 which is configured by the user code (and can be disabled in parameters if required)
    #define TOGGLE_TEST_OUTPUT()    _TOGGLE_PORT(B, DEMO_LED_2)
    #define SET_TEST_OUTPUT()       _SETBITS(B, DEMO_LED_2)
    #define CLEAR_TEST_OUTPUT()     _CLEARBITS(B, DEMO_LED_2)

    #define MULTICOLOUR_LEDS        {0, 2}                               // single LED made up of entries 0, 1 and 2 [green/red/blue]

        // '0'          '1'       input state   center (x,   y)   0 = circle, radius, controlling port, controlling pin 
    #define KEYPAD_LED_DEFINITIONS  \
        {RGB(0,255,0), RGB(0,0,0),     1, {150, 176, 0,   6   }, _PORTB, PORTB_BIT7}, \
        {RGB(255,0,0), RGB(0,0,0),     1, {150, 176, 0,   6   }, _PORTB, PORTB_BIT6}, \
        {RGB(0,0,255), RGB(0,0,0),     1, {150, 176, 0,   6   }, _PORTB, PORTB_BIT10}

    #define KEYPAD "KeyPads/FRDM_KL02Z.bmp"

    // Accelerometer interrupt configuration
    //
    #define ACC_INT_PRIORITY       PRIORITY_PORT_A_INT
    #define ACC_INT_PORT           PORTA
    #define ACC_INT_BIT            PORTA_BIT10
    #define ACC_INT_ASSERTED()     (_READ_PORT_MASK(A, PORTA_BIT10) == 0)
    #define ACC_USE_INT2                                                 // INT1 is not connected on this board so use INT2

  //#define PWM_LED_CONTROL                                              // control RGB led according to accelerometer position (when I2C and accelerometer enabled)
    #define TPM1_0_ALT                                                   // avoid using SWD line for this PWM output
    #define TPM1_1_ALT_2                                                 // PWM output on red LED

    // RTS control via GPIO output
    //
    #define RTS_0_LINE              PORTB_BIT0
    #define _CONFIGURE_RTS_0_HIGH() _CONFIG_DRIVE_PORT_OUTPUT_VALUE(B, (RTS_0_LINE), (RTS_0_LINE), (PORT_SRE_SLOW | PORT_DSE_HIGH))
    #define _CONFIGURE_RTS_0_LOW()  _CONFIG_DRIVE_PORT_OUTPUT_VALUE(B, (RTS_0_LINE), (0), (PORT_SRE_SLOW | PORT_DSE_HIGH))
    #define _SET_RTS_0_HIGH()       _SETBITS(B, RTS_0_LINE)
    #define _SET_RTS_0_LOW()        _CLEARBITS(B, RTS_0_LINE)
#elif defined TRK_KEA128 || defined TRK_KEA64 || defined TRK_KEA8
    #define DEMO_LED_1             (KE_PORTC_BIT0)                       // (blue LED - PTC0) if the port is changed (eg. A to D) the port macros will require appropriate adjustment too
    #define DEMO_LED_2             (KE_PORTC_BIT1)                       // (blue LED - PTC1) if the port is changed (eg. A to D) the port macros will require appropriate adjustment too
    #define DEMO_LED_3             (KE_PORTC_BIT2)                       // (blue LED - PTC2) if the port is changed (eg. A to D) the port macros will require appropriate adjustment too
    #define DEMO_LED_4             (KE_PORTC_BIT3)                       // (blue LED - PTC3) if the port is changed (eg. A to D) the port macros will require appropriate adjustment too

    #define BLINK_LED              DEMO_LED_1

    #define SWITCH_1               (KE_PORTD_BIT0)                        // SW1 if the port is changed (eg. A to D) the port macros will require appropriate adjustment too
    #define SWITCH_2               (KE_PORTD_BIT1)                        // SW2 if the port is changed (eg. A to D) the port macros will require appropriate adjustment too

    #define INIT_WATCHDOG_LED()    _CONFIG_DRIVE_PORT_OUTPUT_VALUE(A, (BLINK_LED), (BLINK_LED), (PORT_SRE_SLOW | PORT_DSE_HIGH))
    #define INIT_WATCHDOG_DISABLE() _CONFIG_PORT_INPUT(A, (SWITCH_1 | SWITCH_2), PORT_PS_UP_ENABLE) // configure as inputs

    #define WATCHDOG_DISABLE()     (_READ_PORT_MASK(A, SWITCH_2) != 0)   // pull this input down to disable watchdog (hold SW2 at reset)

    #define TOGGLE_WATCHDOG_LED()  _TOGGLE_PORT(A, BLINK_LED)

    #define ACTIVATE_WATCHDOG()    UNLOCK_WDOG(); WDOG_CS2 = (WDOG_CS2_CLK_1kHz | WDOG_CS2_FLG); WDOG_TOVAL = BIG_SHORT_WORD(2000); WDOG_WIN = 0; WDOG_CS1 = (/*WDOG_CS1_UPDATE | */WDOG_CS1_EN); // enable watchdog with 2s timeout

    #define SHIFT_DEMO_LED_1       (((2 * 8) + 0) - 0)                   // since the port bits are spread out shift each to the lowest 4 bits
    #define SHIFT_DEMO_LED_2       (((2 * 8) + 0) - 0)
    #define SHIFT_DEMO_LED_3       (((2 * 8) + 0) - 0)
    #define SHIFT_DEMO_LED_4       (((2 * 8) + 0) - 0)

    #define MAPPED_DEMO_LED_1      (DEMO_LED_1 >> SHIFT_DEMO_LED_1)
    #define MAPPED_DEMO_LED_2      (DEMO_LED_2 >> SHIFT_DEMO_LED_2)
    #define MAPPED_DEMO_LED_3      (DEMO_LED_3 >> SHIFT_DEMO_LED_3)
    #define MAPPED_DEMO_LED_4      (DEMO_LED_4 >> SHIFT_DEMO_LED_4)

    #define MEASURE_LOW_POWER_ON()    _CLEARBITS(A, DEMO_LED_2)          // signal when the processor is in sleep mode
    #define MEASURE_LOW_POWER_OFF()   _SETBITS(A, DEMO_LED_2)            // signal when the processor is in active mode

        // '0'          '1'           input state  rectangle      controlling port, controlling pin (4 LEDs then connector ports)
    #define KEYPAD_LED_DEFINITIONS  \
        {RGB(200,200,200),  RGB(0,0,255), 0, {447, 231, 466, 238 }, (_PORTA * 4), KE_PORTC_BIT0}, \
        {RGB(200,200,200),  RGB(0,0,255), 0, {447, 243, 466, 250 }, (_PORTA * 4), KE_PORTC_BIT1}, \
        {RGB(200,200,200),  RGB(0,0,255), 0, {447, 255, 466, 263 }, (_PORTA * 4), KE_PORTC_BIT2}, \
        {RGB(200,200,200),  RGB(0,0,255), 0, {447, 267, 466, 275 }, (_PORTA * 4), KE_PORTC_BIT3}, \
        {RGB(0,0,0),   RGB(255,0,0),      0, {192, 48,  0,   2   }, (_PORTB * 4), KE_PORTF_BIT7}, \
        {RGB(0,0,0),   RGB(255,0,0),      0, {192, 61,  0,   2   }, (_PORTB * 4), KE_PORTF_BIT6}, \
        {RGB(0,0,0),   RGB(255,0,0),      0, {192, 74,  0,   2   }, (_PORTB * 4), KE_PORTF_BIT5}, \
        {RGB(0,0,0),   RGB(255,0,0),      0, {192, 87,  0,   2   }, (_PORTB * 4), KE_PORTF_BIT4}, \
        {RGB(0,0,0),   RGB(255,0,0),      0, {192, 101, 0,   2   }, (_PORTB * 4), KE_PORTF_BIT3}, \
        {RGB(0,0,0),   RGB(255,0,0),      0, {192, 113, 0,   2   }, (_PORTB * 4), KE_PORTF_BIT2}, \
        {RGB(0,0,0),   RGB(255,0,0),      0, {192, 126, 0,   2   }, (_PORTB * 4), KE_PORTF_BIT1}, \
        {RGB(0,0,0),   RGB(255,0,0),      0, {192, 140, 0,   2   }, (_PORTB * 4), KE_PORTF_BIT0}, \
        {RGB(0,0,0),   RGB(255,0,0),      0, {218, 48,  0,   2   }, (_PORTB * 4), KE_PORTG_BIT0}, \
        {RGB(0,0,0),   RGB(255,0,0),      0, {218, 61,  0,   2   }, (_PORTB * 4), KE_PORTG_BIT1}, \
        {RGB(0,0,0),   RGB(255,0,0),      0, {218, 74,  0,   2   }, (_PORTB * 4), KE_PORTG_BIT2}, \
        {RGB(0,0,0),   RGB(255,0,0),      0, {218, 87,  0,   2   }, (_PORTB * 4), KE_PORTG_BIT3}, \
        {RGB(0,0,0),   RGB(255,0,0),      0, {218, 101, 0,   2   }, (_PORTB * 4), KE_PORTG_BIT4}, \
        {RGB(0,0,0),   RGB(255,0,0),      0, {218, 113, 0,   2   }, (_PORTB * 4), KE_PORTG_BIT5}, \
        {RGB(0,0,0),   RGB(255,0,0),      0, {218, 126, 0,   2   }, (_PORTB * 4), KE_PORTG_BIT6}, \
        {RGB(0,0,0),   RGB(255,0,0),      0, {218, 140, 0,   2   }, (_PORTB * 4), KE_PORTG_BIT7}, \
        {RGB(0,0,0),   RGB(255,0,0),      0, {244, 74,  0,   2   }, (_PORTB * 4), KE_PORTB_BIT2}, \
        {RGB(0,0,0),   RGB(255,0,0),      0, {244, 87,  0,   2   }, (_PORTB * 4), KE_PORTB_BIT3}, \
        {RGB(0,0,0),   RGB(255,0,0),      0, {244, 101, 0,   2   }, (_PORTB * 4), KE_PORTB_BIT4}, \
        {RGB(0,0,0),   RGB(255,0,0),      0, {244, 113, 0,   2   }, (_PORTB * 4), KE_PORTB_BIT5}, \
        {RGB(0,0,0),   RGB(255,0,0),      0, {244, 126, 0,   2   }, (_PORTB * 4), KE_PORTB_BIT6}, \
        {RGB(0,0,0),   RGB(255,0,0),      0, {244, 140, 0,   2   }, (_PORTB * 4), KE_PORTB_BIT7}, \
        {RGB(0,0,0),   RGB(255,0,0),      0, {271, 48,  0,   2   }, (_PORTA * 4), KE_PORTA_BIT0}, \
        {RGB(0,0,0),   RGB(255,0,0),      0, {271, 61,  0,   2   }, (_PORTA * 4), KE_PORTA_BIT1}, \
        {RGB(0,0,0),   RGB(255,0,0),      0, {271, 87,  0,   2   }, (_PORTA * 4), KE_PORTA_BIT2}, \
        {RGB(0,0,0),   RGB(255,0,0),      0, {271, 101, 0,   2   }, (_PORTA * 4), KE_PORTA_BIT3}, \
        {RGB(0,0,0),   RGB(255,0,0),      0, {271, 113, 0,   2   }, (_PORTA * 4), KE_PORTA_BIT4}, \
        {RGB(0,0,0),   RGB(255,0,0),      0, {271, 126, 0,   2   }, (_PORTA * 4), KE_PORTA_BIT6}, \
        {RGB(0,0,0),   RGB(255,0,0),      0, {271, 140, 0,   2   }, (_PORTA * 4), KE_PORTA_BIT7}, \
        {RGB(0,0,0),   RGB(255,0,0),      0, {297, 48,  0,   2   }, (_PORTB * 4), KE_PORTE_BIT0}, \
        {RGB(0,0,0),   RGB(255,0,0),      0, {297, 61,  0,   2   }, (_PORTB * 4), KE_PORTE_BIT1}, \
        {RGB(0,0,0),   RGB(255,0,0),      0, {297, 74,  0,   2   }, (_PORTB * 4), KE_PORTE_BIT2}, \
        {RGB(0,0,0),   RGB(255,0,0),      0, {297, 87,  0,   2   }, (_PORTB * 4), KE_PORTE_BIT3}, \
        {RGB(0,0,0),   RGB(255,0,0),      0, {297, 101, 0,   2   }, (_PORTB * 4), KE_PORTE_BIT4}, \
        {RGB(0,0,0),   RGB(255,0,0),      0, {297, 113, 0,   2   }, (_PORTB * 4), KE_PORTE_BIT5}, \
        {RGB(0,0,0),   RGB(255,0,0),      0, {297, 126, 0,   2   }, (_PORTB * 4), KE_PORTE_BIT6}, \
        {RGB(0,0,0),   RGB(255,0,0),      0, {297, 140, 0,   2   }, (_PORTB * 4), KE_PORTE_BIT7}, \
        {RGB(0,0,0),   RGB(255,0,0),      0, {324, 61,  0,   2   }, (_PORTC * 4), KE_PORTI_BIT0}, \
        {RGB(0,0,0),   RGB(255,0,0),      0, {324, 74,  0,   2   }, (_PORTC * 4), KE_PORTI_BIT1}, \
        {RGB(0,0,0),   RGB(255,0,0),      0, {324, 87,  0,   2   }, (_PORTC * 4), KE_PORTI_BIT2}, \
        {RGB(0,0,0),   RGB(255,0,0),      0, {324, 101, 0,   2   }, (_PORTC * 4), KE_PORTI_BIT3}, \
        {RGB(0,0,0),   RGB(255,0,0),      0, {324, 113, 0,   2   }, (_PORTC * 4), KE_PORTI_BIT4}, \
        {RGB(0,0,0),   RGB(255,0,0),      0, {324, 126, 0,   2   }, (_PORTC * 4), KE_PORTI_BIT5}, \
        {RGB(0,0,0),   RGB(255,0,0),      0, {324, 140, 0,   2   }, (_PORTC * 4), KE_PORTI_BIT6}, \
        {RGB(0,0,0),   RGB(255,0,0),      0, {349, 48,  0,   2   }, (_PORTA * 4), KE_PORTD_BIT0}, \
        {RGB(0,0,0),   RGB(255,0,0),      0, {349, 62,  0,   2   }, (_PORTA * 4), KE_PORTD_BIT1}, \
        {RGB(0,0,0),   RGB(255,0,0),      0, {349, 74,  0,   2   }, (_PORTA * 4), KE_PORTD_BIT2}, \
        {RGB(0,0,0),   RGB(255,0,0),      0, {349, 87,  0,   2   }, (_PORTA * 4), KE_PORTD_BIT3}, \
        {RGB(0,0,0),   RGB(255,0,0),      0, {349, 101, 0,   2   }, (_PORTA * 4), KE_PORTD_BIT4}, \
        {RGB(0,0,0),   RGB(255,0,0),      0, {349, 114, 0,   2   }, (_PORTA * 4), KE_PORTD_BIT5}, \
        {RGB(0,0,0),   RGB(255,0,0),      0, {377, 48,  0,   2   }, (_PORTB * 4), KE_PORTH_BIT7}, \
        {RGB(0,0,0),   RGB(255,0,0),      0, {377, 61,  0,   2   }, (_PORTB * 4), KE_PORTH_BIT6}, \
        {RGB(0,0,0),   RGB(255,0,0),      0, {377, 74,  0,   2   }, (_PORTB * 4), KE_PORTH_BIT5}, \
        {RGB(0,0,0),   RGB(255,0,0),      0, {377, 87,  0,   2   }, (_PORTB * 4), KE_PORTH_BIT4}, \
        {RGB(0,0,0),   RGB(255,0,0),      0, {377, 101, 0,   2   }, (_PORTB * 4), KE_PORTH_BIT3}, \
        {RGB(0,0,0),   RGB(255,0,0),      0, {377, 113, 0,   2   }, (_PORTB * 4), KE_PORTH_BIT2}, \
        {RGB(0,0,0),   RGB(255,0,0),      0, {377, 126, 0,   2   }, (_PORTB * 4), KE_PORTH_BIT1}, \
        {RGB(0,0,0),   RGB(255,0,0),      0, {377, 140, 0,   2   }, (_PORTB * 4), KE_PORTH_BIT0}, \
        {RGB(0,0,0),   RGB(255,0,0),      0, {404, 48,  0,   2   }, (_PORTA * 4), KE_PORTC_BIT7}, \
        {RGB(0,0,0),   RGB(255,0,0),      0, {404, 61,  0,   2   }, (_PORTA * 4), KE_PORTC_BIT6}, \
        {RGB(0,0,0),   RGB(255,0,0),      0, {404, 74,  0,   2   }, (_PORTA * 4), KE_PORTC_BIT5}, \
        {RGB(0,0,0),   RGB(255,0,0),      0, {404, 87,  0,   2   }, (_PORTA * 4), KE_PORTC_BIT4}, \
        {RGB(0,0,0),   RGB(255,0,0),      0, {404, 101, 0,   2   }, (_PORTA * 4), KE_PORTC_BIT3}, \
        {RGB(0,0,0),   RGB(255,0,0),      0, {404, 113, 0,   2   }, (_PORTA * 4), KE_PORTC_BIT2}, \
        {RGB(0,0,0),   RGB(255,0,0),      0, {404, 126, 0,   2   }, (_PORTA * 4), KE_PORTC_BIT1}, \
        {RGB(0,0,0),   RGB(255,0,0),      0, {404, 140, 0,   2   }, (_PORTA * 4), KE_PORTC_BIT0}, \


    #define BUTTON_KEY_DEFINITIONS  {_PORTD, (KE_PORTD_BIT0 >> (3 * 8)), {25,  140, 45,  150 }}, \
                                    {_PORTD, (KE_PORTD_BIT1 >> (3 * 8)), {25,  188, 45,  198 }}, \
                                    {_PORTF, (KE_PORTF_BIT7 >> (1 * 8)), {188, 42,  197, 52  }}, \
                                    {_PORTF, (KE_PORTF_BIT6 >> (1 * 8)), {188, 56,  197, 67  }}, \
                                    {_PORTF, (KE_PORTF_BIT5 >> (1 * 8)), {188, 70,  197, 79  }}, \
                                    {_PORTF, (KE_PORTF_BIT4 >> (1 * 8)), {188, 83,  197, 90  }}, \
                                    {_PORTF, (KE_PORTF_BIT3 >> (1 * 8)), {188, 96,  197, 104 }}, \
                                    {_PORTF, (KE_PORTF_BIT2 >> (1 * 8)), {188, 109, 197, 118 }}, \
                                    {_PORTF, (KE_PORTF_BIT1 >> (1 * 8)), {188, 123, 197, 131 }}, \
                                    {_PORTF, (KE_PORTF_BIT0 >> (1 * 8)), {188, 138, 197, 146 }}, \
                                    {_PORTG, (KE_PORTG_BIT0 >> (2 * 8)), {213, 42,  222, 52  }}, \
                                    {_PORTG, (KE_PORTG_BIT1 >> (2 * 8)), {213, 56,  222, 67  }}, \
                                    {_PORTG, (KE_PORTG_BIT2 >> (2 * 8)), {213, 70,  222, 79  }}, \
                                    {_PORTG, (KE_PORTG_BIT3 >> (2 * 8)), {213, 83,  222, 90  }}, \
                                    {_PORTG, (KE_PORTG_BIT4 >> (2 * 8)), {213, 96,  222, 104 }}, \
                                    {_PORTG, (KE_PORTG_BIT5 >> (2 * 8)), {213, 109, 222, 118 }}, \
                                    {_PORTG, (KE_PORTG_BIT6 >> (2 * 8)), {213, 123, 222, 131 }}, \
                                    {_PORTG, (KE_PORTG_BIT7 >> (2 * 8)), {213, 138, 222, 146 }}, \
                                    {_PORTB, (KE_PORTB_BIT2 >> (1 * 8)), {238, 70,  247, 79  }}, \
                                    {_PORTB, (KE_PORTB_BIT3 >> (1 * 8)), {238, 83,  247, 90  }}, \
                                    {_PORTB, (KE_PORTB_BIT4 >> (1 * 8)), {238, 96,  247, 104 }}, \
                                    {_PORTB, (KE_PORTB_BIT5 >> (1 * 8)), {238, 109, 247, 118 }}, \
                                    {_PORTB, (KE_PORTB_BIT6 >> (1 * 8)), {238, 123, 247, 131 }}, \
                                    {_PORTB, (KE_PORTB_BIT7 >> (1 * 8)), {238, 138, 247, 146 }}, \
                                    {_PORTA, (KE_PORTA_BIT0 >> (0 * 8)), {267, 42,  274, 52  }}, \
                                    {_PORTA, (KE_PORTA_BIT1 >> (0 * 8)), {267, 56,  274, 67  }}, \
                                    {_PORTA, (KE_PORTA_BIT2 >> (0 * 8)), {267, 83,  274, 90  }}, \
                                    {_PORTA, (KE_PORTA_BIT3 >> (0 * 8)), {267, 96,  274, 104 }}, \
                                    {_PORTA, (KE_PORTA_BIT4 >> (0 * 8)), {267, 109, 274, 118 }}, \
                                    {_PORTA, (KE_PORTA_BIT6 >> (0 * 8)), {267, 123, 274, 131 }}, \
                                    {_PORTA, (KE_PORTA_BIT7 >> (0 * 8)), {267, 138, 274, 146 }}, \
                                    {_PORTE, (KE_PORTE_BIT0 >> (0 * 8)), {293, 42,  299, 52  }}, \
                                    {_PORTE, (KE_PORTE_BIT1 >> (0 * 8)), {293, 56,  299, 67  }}, \
                                    {_PORTE, (KE_PORTE_BIT2 >> (0 * 8)), {293, 70,  299, 79  }}, \
                                    {_PORTE, (KE_PORTE_BIT3 >> (0 * 8)), {293, 83,  299, 90  }}, \
                                    {_PORTE, (KE_PORTE_BIT4 >> (0 * 8)), {293, 96,  299, 104 }}, \
                                    {_PORTE, (KE_PORTE_BIT5 >> (0 * 8)), {293, 109, 299, 118 }}, \
                                    {_PORTE, (KE_PORTE_BIT6 >> (0 * 8)), {293, 123, 299, 131 }}, \
                                    {_PORTE, (KE_PORTE_BIT7 >> (0 * 8)), {293, 138, 299, 146 }}, \
                                    {_PORTI, (KE_PORTI_BIT0 >> (0 * 8)), {319, 56,  327, 67  }}, \
                                    {_PORTI, (KE_PORTI_BIT1 >> (0 * 8)), {319, 70,  327, 79  }}, \
                                    {_PORTI, (KE_PORTI_BIT2 >> (0 * 8)), {319, 83,  327, 90  }}, \
                                    {_PORTI, (KE_PORTI_BIT3 >> (0 * 8)), {319, 96,  327, 104 }}, \
                                    {_PORTI, (KE_PORTI_BIT4 >> (0 * 8)), {319, 109, 327, 118 }}, \
                                    {_PORTI, (KE_PORTI_BIT5 >> (0 * 8)), {319, 123, 327, 131 }}, \
                                    {_PORTI, (KE_PORTI_BIT6 >> (0 * 8)), {319, 138, 327, 146 }}, \
                                    {_PORTD, (KE_PORTD_BIT0 >> (3 * 8)), {345, 43,  356, 52  }}, \
                                    {_PORTD, (KE_PORTD_BIT1 >> (3 * 8)), {345, 57,  356, 65  }}, \
                                    {_PORTD, (KE_PORTD_BIT2 >> (3 * 8)), {345, 71,  356, 79  }}, \
                                    {_PORTD, (KE_PORTD_BIT3 >> (3 * 8)), {345, 83,  356, 91  }}, \
                                    {_PORTD, (KE_PORTD_BIT4 >> (3 * 8)), {345, 97,  356, 105 }}, \
                                    {_PORTD, (KE_PORTD_BIT5 >> (3 * 8)), {345, 110, 356, 119 }}, \
                                    {_PORTH, (KE_PORTH_BIT7 >> (3 * 8)), {372, 42,  382, 52  }}, \
                                    {_PORTH, (KE_PORTH_BIT6 >> (3 * 8)), {372, 56,  382, 67  }}, \
                                    {_PORTH, (KE_PORTH_BIT5 >> (3 * 8)), {372, 70,  382, 79  }}, \
                                    {_PORTH, (KE_PORTH_BIT4 >> (3 * 8)), {372, 83,  382, 90  }}, \
                                    {_PORTH, (KE_PORTH_BIT3 >> (3 * 8)), {372, 96,  382, 104 }}, \
                                    {_PORTH, (KE_PORTH_BIT2 >> (3 * 8)), {372, 109, 382, 118 }}, \
                                    {_PORTH, (KE_PORTH_BIT1 >> (3 * 8)), {372, 123, 382, 131 }}, \
                                    {_PORTH, (KE_PORTH_BIT0 >> (3 * 8)), {372, 138, 382, 146 }}, \
                                    {_PORTC, (KE_PORTC_BIT7 >> (2 * 8)), {400, 42,  407, 52  }}, \
                                    {_PORTC, (KE_PORTC_BIT6 >> (2 * 8)), {400, 56,  407, 67  }}, \
                                    {_PORTC, (KE_PORTC_BIT5 >> (2 * 8)), {400, 70,  407, 79  }}, \
                                    {_PORTC, (KE_PORTC_BIT4 >> (2 * 8)), {400, 83,  407, 90  }}, \
                                    {_PORTC, (KE_PORTC_BIT3 >> (2 * 8)), {400, 96,  407, 104 }}, \
                                    {_PORTC, (KE_PORTC_BIT2 >> (2 * 8)), {400, 109, 407, 118 }}, \
                                    {_PORTC, (KE_PORTC_BIT1 >> (2 * 8)), {400, 123, 407, 131 }}, \
                                    {_PORTC, (KE_PORTC_BIT0 >> (2 * 8)), {400, 138, 407, 146 }}, \


    #define KEYPAD "KeyPads/TRK-KEA.bmp"

    #define CONFIG_TEST_OUTPUT()
    #define TOGGLE_TEST_OUTPUT()
    #define SET_TEST_OUTPUT()
    #define CLEAR_TEST_OUTPUT()
#elif defined FRDM_KEAZ128Q80 || defined FRDM_KEAZ64Q64 || defined FRDM_KEAZN32Q64
    #define DEMO_LED_1             (KE_PORTH_BIT1)                       // (green LED - PTC1) if the port is changed (eg. A to D) the port macros will require appropriate adjustment too
    #define DEMO_LED_2             (KE_PORTH_BIT0)                       // (red LED - PTC0) if the port is changed (eg. A to D) the port macros will require appropriate adjustment too
    #define DEMO_LED_3             (KE_PORTE_BIT7)                       // (blue LED - PTC2) if the port is changed (eg. A to D) the port macros will require appropriate adjustment too
    #define DEMO_LED_4             (0)                                   //  if the port is changed (eg. A to D) the port macros will require appropriate adjustment too

    #define BLINK_LED              DEMO_LED_1

    #define INIT_WATCHDOG_LED()    _CONFIG_DRIVE_PORT_OUTPUT_VALUE(B, (BLINK_LED), (BLINK_LED), (PORT_SRE_SLOW | PORT_DSE_HIGH))

    #if defined FRDM_KEAZN32Q64
        #define SWITCH_2           (KE_PORTA_BIT0)                       // SW2 if the port is changed (eg. A to D) the port macros will require appropriate adjustment too
        #define SWITCH_3           (KE_PORTA_BIT1)                       // SW3 if the port is changed (eg. A to D) the port macros will require appropriate adjustment too
        #define INIT_WATCHDOG_DISABLE() _CONFIG_PORT_INPUT(A, (SWITCH_2 | SWITCH_3), PORT_PS_UP_ENABLE) // configure as inputs
        #define WATCHDOG_DISABLE() (_READ_PORT_MASK(A, SWITCH_3) != 0)   // pull this input down to disable watchdog (hold SW3 at reset)
    #else
        #define SWITCH_2           (KE_PORTE_BIT4)                       // SW2 if the port is changed (eg. A to D) the port macros will require appropriate adjustment too
        #define SWITCH_3           (KE_PORTE_BIT5)                       // SW3 if the port is changed (eg. A to D) the port macros will require appropriate adjustment too
        #define INIT_WATCHDOG_DISABLE() _CONFIG_PORT_INPUT(B, (SWITCH_2 | SWITCH_3), PORT_PS_UP_ENABLE) // configure as inputs
        #define WATCHDOG_DISABLE() (_READ_PORT_MASK(B, SWITCH_3) != 0)   // pull this input down to disable watchdog (hold SW3 at reset)
    #endif

    #define TOGGLE_WATCHDOG_LED()  _TOGGLE_PORT(B, BLINK_LED)

    #define ACTIVATE_WATCHDOG()    UNLOCK_WDOG(); WDOG_CS2 = (WDOG_CS2_CLK_1kHz | WDOG_CS2_FLG); WDOG_TOVAL = BIG_SHORT_WORD(2000); WDOG_WIN = 0; WDOG_CS1 = (/*WDOG_CS1_UPDATE | */WDOG_CS1_EN); // enable watchdog with 2s timeout

    #define SHIFT_DEMO_LED_1       (((3 * 8) + 1) - 0)                   // since the port bits are spread out shift each to the lowest 4 bits
    #define SHIFT_DEMO_LED_2       (((3 * 8) + 0) - 1)
    #define SHIFT_DEMO_LED_3       (((0 * 8) + 7) - 2)
    #define SHIFT_DEMO_LED_4       (0)

    #define MAPPED_DEMO_LED_1      (DEMO_LED_1 >> SHIFT_DEMO_LED_1)
    #define MAPPED_DEMO_LED_2      (DEMO_LED_2 >> SHIFT_DEMO_LED_2)
    #define MAPPED_DEMO_LED_3      (DEMO_LED_3 >> SHIFT_DEMO_LED_3)
    #define MAPPED_DEMO_LED_4      (DEMO_LED_4 >> SHIFT_DEMO_LED_4)

    #define MEASURE_LOW_POWER_ON()  //_CLEARBITS(B, DEMO_LED_2)          // signal when the processor is in sleep mode
    #define MEASURE_LOW_POWER_OFF() //_SETBITS(B, DEMO_LED_2)            // signal when the processor is in active mode

    #define MULTICOLOUR_LEDS        {0, 2}                               // single LED made up of entries 0, 1 and 2 [green/red/blue]

        // '0'          '1'           input state  center (x,   y)   0 = circle, radius   controlling port, controlling pin (4 LEDs then connector ports)
    #define KEYPAD_LED_DEFINITIONS  \
        {RGB(255,0,  0),   RGB(0,0,0), 1, {415, 211, 0, 8 }, _PORTH, (KE_PORTH_BIT0 >> (8 * 3))}, \
        {RGB(0,  255,0),   RGB(0,0,0), 1, {415, 211, 0, 8 }, _PORTH, (KE_PORTH_BIT1 >> (8 * 3))}, \
        {RGB(0,  0,  255), RGB(0,0,0), 1, {415, 211, 0, 8 }, _PORTE, (KE_PORTE_BIT7 >> (8 * 3))}

    #if defined FRDM_KEAZN32Q64
        #define BUTTON_KEY_DEFINITIONS  {_PORTA, (SWITCH_2 >> (0 * 8)), {415, 105, 424, 116 }}, \
                                            {_PORTA, (SWITCH_3 >> (0 * 8)), {415, 152, 424, 165 }}
    #else
        #define BUTTON_KEY_DEFINITIONS  {_PORTB, (SWITCH_2 >> (0 * 8)), {415, 105, 424, 116 }}, \
                                            {_PORTB, (SWITCH_3 >> (0 * 8)), {415, 152, 424, 165 }}
    #endif

    #define KEYPAD "KeyPads/FRDM-KEA128.bmp"

    #define CONFIG_TEST_OUTPUT()
    #define TOGGLE_TEST_OUTPUT()
    #define SET_TEST_OUTPUT()
    #define CLEAR_TEST_OUTPUT()

    // RTS control via GPIO output
    //
    #define RTS_0_LINE              PORTB_BIT0
    #define _CONFIGURE_RTS_0_HIGH() _CONFIG_DRIVE_PORT_OUTPUT_VALUE(B, (RTS_0_LINE), (RTS_0_LINE), (PORT_SRE_SLOW | PORT_DSE_HIGH))
    #define _CONFIGURE_RTS_0_LOW()  _CONFIG_DRIVE_PORT_OUTPUT_VALUE(B, (RTS_0_LINE), (0), (PORT_SRE_SLOW | PORT_DSE_HIGH))
    #define _SET_RTS_0_HIGH()       _SETBITS(B, RTS_0_LINE)
    #define _SET_RTS_0_LOW()        _CLEARBITS(B, RTS_0_LINE)

    #define _CONFIGURE_RTS_1_HIGH()
    #define _CONFIGURE_RTS_1_LOW()
    #define _SET_RTS_1_HIGH()
    #define _SET_RTS_1_LOW()

    #define _CONFIGURE_RTS_2_HIGH()
    #define _CONFIGURE_RTS_2_LOW()
    #define _SET_RTS_2_HIGH()
    #define _SET_RTS_2_LOW()
#elif defined FRDM_KE06Z                                                 // {30}
    #define DEMO_LED_1             (KE_PORTG_BIT6)                       // (green LED - PTG6) if the port is changed (eg. A to D) the port macros will require appropriate adjustment too
    #define DEMO_LED_2             (KE_PORTG_BIT5)                       // (red LED - PTG5) if the port is changed (eg. A to D) the port macros will require appropriate adjustment too
    #define DEMO_LED_3             (KE_PORTG_BIT7)                       // (blue LED - PTG7) if the port is changed (eg. A to D) the port macros will require appropriate adjustment too
    #define DEMO_LED_4             0

    #define BLINK_LED              DEMO_LED_1

    #define SWITCH_2               (KE_PORTH_BIT4)                        // SW2 if the port is changed (eg. A to D) the port macros will require appropriate adjustment too
    #define SWITCH_3               (KE_PORTH_BIT3)                        // SW3 if the port is changed (eg. A to D) the port macros will require appropriate adjustment too

    #define INIT_WATCHDOG_LED()    _CONFIG_DRIVE_PORT_OUTPUT_VALUE(B, (BLINK_LED), (BLINK_LED), (PORT_SRE_SLOW | PORT_DSE_HIGH))
    #define INIT_WATCHDOG_DISABLE() _CONFIG_PORT_INPUT(B, (SWITCH_2 | SWITCH_3), PORT_PS_UP_ENABLE) // configure as inputs

    #define WATCHDOG_DISABLE()     (!_READ_PORT_MASK(B, SWITCH_2))       // pull this input down to disable watchdog (hold SW2 at reset)

    #define TOGGLE_WATCHDOG_LED()  _TOGGLE_PORT(B, BLINK_LED)

    #define ACTIVATE_WATCHDOG()    UNLOCK_WDOG(); WDOG_CS2 = (WDOG_CS2_CLK_1kHz | WDOG_CS2_FLG); WDOG_TOVAL = BIG_SHORT_WORD(2000); WDOG_WIN = 0; WDOG_CS1 = (/*WDOG_CS1_UPDATE | */WDOG_CS1_EN); // enable watchdog with 2s timeout

    #define SHIFT_DEMO_LED_1       (((2 * 8) + 6) - 0)                   // since the port bits are spread out shift each to the lowest 4 bits
    #define SHIFT_DEMO_LED_2       (((2 * 8) + 5) - 1)
    #define SHIFT_DEMO_LED_3       (((2 * 8) + 7) - 2)
    #define SHIFT_DEMO_LED_4       0

    #define MAPPED_DEMO_LED_1      (DEMO_LED_1 >> SHIFT_DEMO_LED_1)
    #define MAPPED_DEMO_LED_2      (DEMO_LED_2 >> SHIFT_DEMO_LED_2)
    #define MAPPED_DEMO_LED_3      (DEMO_LED_3 >> SHIFT_DEMO_LED_3)
    #define MAPPED_DEMO_LED_4      (DEMO_LED_4 >> SHIFT_DEMO_LED_4)

    #define MEASURE_LOW_POWER_ON()    _SETBITS(B, DEMO_LED_2)            // signal when the processor is in sleep mode
    #define MEASURE_LOW_POWER_OFF()   _CLEARBITS(B, DEMO_LED_2)          // signal when the processor is in active mode

    #define MULTICOLOUR_LEDS        {0, 2}                               // single LED made up of entries 0, 1 and 2 [green/red/blue]

        // '0'          '1'       input state   center (x,   y)   0 = circle, radius, controlling port, controlling pin 
    #define KEYPAD_LED_DEFINITIONS  \
        {RGB(0,255,0), RGB(0,0,0),     1, {153, 129, 0,   6   }, (_PORTB * 4), KE_PORTG_BIT6}, \
        {RGB(255,0,0), RGB(0,0,0),     1, {153, 129, 0,   6   }, (_PORTB * 4), KE_PORTG_BIT5}, \
        {RGB(0,0,255), RGB(0,0,0),     1, {153, 129, 0,   6   }, (_PORTB * 4), KE_PORTG_BIT7}

    #define BUTTON_KEY_DEFINITIONS  {_PORTH, (KE_PORTH_BIT4 >> (3 * 8)), {289, 31,  305, 39 }}, \
                                    {_PORTH, (KE_PORTH_BIT3 >> (3 * 8)), {289, 48,  305, 59 }}

    #define KEYPAD "KeyPads/FRDM_KE06Z.bmp"

    #define CONFIG_TEST_OUTPUT()
    #define TOGGLE_TEST_OUTPUT()
    #define SET_TEST_OUTPUT()
    #define CLEAR_TEST_OUTPUT()
#elif defined FRDM_KE04Z
    #define DEMO_LED_1             (KE_PORTC_BIT4)                       // (green LED - PTC4) if the port is changed (eg. A to D) the port macros will require appropriate adjustment too
    #define DEMO_LED_2             (KE_PORTC_BIT5)                       // (red LED - PTC5) if the port is changed (eg. A to D) the port macros will require appropriate adjustment too
    #define DEMO_LED_3             (KE_PORTB_BIT3)                       // (blue LED - PTB3) if the port is changed (eg. A to D) the port macros will require appropriate adjustment too
    #define DEMO_LED_4             0

    #define BLINK_LED              DEMO_LED_1

    #define SWITCH_1               (KE_PORTA_BIT0)                       // (J1-2) if the port is changed (eg. A to D) the port macros will require appropriate adjustment too

    #define INIT_WATCHDOG_LED()    _CONFIG_DRIVE_PORT_OUTPUT_VALUE(A, (BLINK_LED), (BLINK_LED), (PORT_SRE_SLOW | PORT_DSE_HIGH))
    #define INIT_WATCHDOG_DISABLE() _CONFIG_PORT_INPUT(A, (SWITCH_1), PORT_PS_UP_ENABLE); // configure as input with pullup enabled

    #define WATCHDOG_DISABLE()     (!_READ_PORT_MASK(A, SWITCH_1))       // pull this input down to disable watchdog (J1-2)

    #define ACTIVATE_WATCHDOG()    UNLOCK_WDOG(); WDOG_CS2 = (WDOG_CS2_CLK_1kHz | WDOG_CS2_FLG); WDOG_TOVAL = BIG_SHORT_WORD(2000); WDOG_WIN = 0; WDOG_CS1 = (/*WDOG_CS1_UPDATE | */WDOG_CS1_EN); // enable watchdog with 2s timeout

    #define SHIFT_DEMO_LED_1       (((2 * 8) + 4) - 0)                   // since the port bits are spread out shift each to the lowest 4 bits
    #define SHIFT_DEMO_LED_2       (((2 * 8) + 5) - 1)
    #define SHIFT_DEMO_LED_3       (((1 * 8) + 3) - 2)
    #define SHIFT_DEMO_LED_4       0

    #define MAPPED_DEMO_LED_1      (DEMO_LED_1 >> SHIFT_DEMO_LED_1)
    #define MAPPED_DEMO_LED_2      (DEMO_LED_2 >> SHIFT_DEMO_LED_2)
    #define MAPPED_DEMO_LED_3      (DEMO_LED_3 >> SHIFT_DEMO_LED_3)
    #define MAPPED_DEMO_LED_4      (DEMO_LED_4 >> SHIFT_DEMO_LED_4)

    #define CONFIG_TEST_OUTPUT()

    #define TOGGLE_WATCHDOG_LED()  _TOGGLE_PORT(A, BLINK_LED)
    #define TOGGLE_TEST_OUTPUT()   _TOGGLE_PORT(A, DEMO_LED_2)
    #define SET_TEST_OUTPUT()      _SETBITS(A, DEMO_LED_2)
    #define CLEAR_TEST_OUTPUT()    _CLEARBITS(A, DEMO_LED_2)

    #define MULTICOLOUR_LEDS        {0, 2}                               // single LED made up of entries 0, 1 and 2 [green/red/blue]

        // '0'          '1'       input state   center (x,   y)   0 = circle, radius, controlling port, controlling pin 
    #define KEYPAD_LED_DEFINITIONS  \
        {RGB(0,255,0), RGB(0,0,0),     1, {236, 130, 0,   9   }, (_PORTA * 4), DEMO_LED_1}, \
        {RGB(255,0,0), RGB(0,0,0),     1, {236, 130, 0,   9   }, (_PORTA * 4), DEMO_LED_2}, \
        {RGB(0,0,255), RGB(0,0,0),     1, {236, 130, 0,   9   }, (_PORTA * 4), DEMO_LED_3}

    #define KEYPAD "KeyPads/FRDM_KE04Z.bmp"
#elif defined FRDM_KE02Z || defined FRDM_KE02Z40M                        // {25}{30}
    #define DEMO_LED_1             (KE_PORTH_BIT2)                       // (green LED - PTH2) if the port is changed (eg. A to D) the port macros will require appropriate adjustment too
    #define DEMO_LED_2             (KE_PORTH_BIT1)                       // (red LED - PTH1) if the port is changed (eg. A to D) the port macros will require appropriate adjustment too
    #define DEMO_LED_3             (KE_PORTE_BIT7)                       // (blue LED - PTE7) if the port is changed (eg. A to D) the port macros will require appropriate adjustment too
    #define DEMO_LED_4             0

    #define BLINK_LED              DEMO_LED_1

    #define SWITCH_1               (KE_PORTB_BIT3)                       // if the port is changed (eg. A to D) the port macros will require appropriate adjustment too

    #define INIT_WATCHDOG_LED()    _CONFIG_DRIVE_PORT_OUTPUT_VALUE(B, (BLINK_LED), (BLINK_LED), (PORT_DSE_LOW))
    #define INIT_WATCHDOG_DISABLE() _CONFIG_PORT_INPUT(B, (SWITCH_1), PORT_PS_UP_ENABLE); // configure as input with pullup enabled

    #define WATCHDOG_DISABLE()     (_READ_PORT_MASK(B, SWITCH_1) == 0)   // pull this input down to disable watchdog (J9-15)

    #define ACTIVATE_WATCHDOG()    UNLOCK_WDOG(); WDOG_CS2 = (WDOG_CS2_CLK_1kHz | WDOG_CS2_FLG); WDOG_TOVAL = BIG_SHORT_WORD(2000); WDOG_WIN = 0; WDOG_CS1 = (/*WDOG_CS1_UPDATE | */WDOG_CS1_EN); // enable watchdog with 2s timeout

    #define SHIFT_DEMO_LED_1       (((3 * 8) + 2) - 0)                   // since the port bits are spread out shift each to the lowest 4 bits
    #define SHIFT_DEMO_LED_2       (((3 * 8) + 1) - 1)
    #define SHIFT_DEMO_LED_3       (((0 * 8) + 7) - 2)
    #define SHIFT_DEMO_LED_4       0

    #define MAPPED_DEMO_LED_1      (DEMO_LED_1 >> SHIFT_DEMO_LED_1)
    #define MAPPED_DEMO_LED_2      (DEMO_LED_2 >> SHIFT_DEMO_LED_2)
    #define MAPPED_DEMO_LED_3      (DEMO_LED_3 >> SHIFT_DEMO_LED_3)
    #define MAPPED_DEMO_LED_4      (DEMO_LED_4 >> SHIFT_DEMO_LED_4)

    #define CONFIG_TEST_OUTPUT()

    #define TOGGLE_WATCHDOG_LED()  _TOGGLE_PORT(B, BLINK_LED)
    #define TOGGLE_TEST_OUTPUT()   _TOGGLE_PORT(B, DEMO_LED_2)
    #define SET_TEST_OUTPUT()      _SETBITS(B, DEMO_LED_2)
    #define CLEAR_TEST_OUTPUT()    _CLEARBITS(B, DEMO_LED_2)

    #define MULTICOLOUR_LEDS        {0, 2}                               // single LED made up of entries 0, 1 and 2 [green/red/blue]

        // '0'          '1'       input state   center (x,   y)   0 = circle, radius, controlling port, controlling pin 
    #define KEYPAD_LED_DEFINITIONS  \
        {RGB(0,255,0), RGB(0,0,0),     1, {277, 138, 0,   5   }, (_PORTB * 4), KE_PORTH_BIT2}, \
        {RGB(255,0,0), RGB(0,0,0),     1, {277, 138, 0,   5   }, (_PORTB * 4), KE_PORTH_BIT1}, \
        {RGB(0,0,255), RGB(0,0,0),     1, {277, 138, 0,   5   }, (_PORTB * 4), KE_PORTE_BIT7}

    #define KEYPAD "KeyPads/FRDM_KE02Z.bmp"

    #define PWM_LED_CONTROL                                              // control RGB led according to accelerometer position (when I2C and accelerometer enabled)
        #define FTM2_1_ON_H                                              // PWM channel 2-1 is on PTH2 (red-led)
        #define FTM1_1_ON_E                                              // PWM channel 1-1 is on PTE7 (blue-led)
#elif defined FRDM_KL05Z                                                 // {25}
    #define DEMO_LED_1             (PORTB_BIT9)                          // (green LED) if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define DEMO_LED_2             (PORTB_BIT8)                          // (red LED) if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define DEMO_LED_3             (PORTB_BIT10)                         // (blue LED) if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define DEMO_LED_4             0
    #define BLINK_LED              (DEMO_LED_1)
    #define INPUT_1                (PORTB_BIT6)                          // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define INPUT_2                (PORTB_BIT7)                          // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too

    #if defined USE_MAINTENANCE
        #define INIT_WATCHDOG_LED()                                      // let the port set up do this (the user can disable blinking)
    #else
        #define INIT_WATCHDOG_LED() _CONFIG_DRIVE_PORT_OUTPUT_VALUE(B, (BLINK_LED), (BLINK_LED), (PORT_SRE_SLOW | PORT_DSE_HIGH))
    #endif

    #define SHIFT_DEMO_LED_1        9                                    // since the port bits are spread out shift each to the lowest 4 bits
    #define SHIFT_DEMO_LED_2        7
    #define SHIFT_DEMO_LED_3        8
    #define SHIFT_DEMO_LED_4        0

    #define MAPPED_DEMO_LED_1       (DEMO_LED_1 >> SHIFT_DEMO_LED_1)
    #define MAPPED_DEMO_LED_2       (DEMO_LED_2 >> SHIFT_DEMO_LED_2)
    #define MAPPED_DEMO_LED_3       (DEMO_LED_3 >> SHIFT_DEMO_LED_3)
    #define MAPPED_DEMO_LED_4       (DEMO_LED_4 >> SHIFT_DEMO_LED_4)

    #define INIT_WATCHDOG_DISABLE() _CONFIG_PORT_INPUT_FAST_LOW(B, (INPUT_1 | INPUT_2), PORT_PS_UP_ENABLE) // configure as input
    #define WATCHDOG_DISABLE()      (_READ_PORT_MASK(B, INPUT_1) == 0)   // pull this input down at reset to disable watchdog [J8-pin 7]
    #define ACTIVATE_WATCHDOG()     SIM_COPC = (SIM_COPC_COPCLKS_1K | SIM_COPC_COPT_LONGEST) // 1.024s watchdog timeout
    #define TOGGLE_WATCHDOG_LED()   _TOGGLE_PORT(B, BLINK_LED)

    #define CONFIG_TEST_OUTPUT()                                         // we use DEMO_LED_2 which is configured by the user code (and can be disabled in parameters if required)
    #define TOGGLE_TEST_OUTPUT()    _TOGGLE_PORT(B, DEMO_LED_2)
    #define SET_TEST_OUTPUT()       _SETBITS(B, DEMO_LED_2)
    #define CLEAR_TEST_OUTPUT()     _CLEARBITS(B, DEMO_LED_2)

    #define MULTICOLOUR_LEDS        {0, 2}                               // single LED made up of entries 0, 1 and 2 [green/red/blue]

        // '0'          '1'       input state   center (x,   y)   0 = circle, radius, controlling port, controlling pin 
    #define KEYPAD_LED_DEFINITIONS  \
        {RGB(0,255,0), RGB(0,0,0),     1, {166, 186, 0,   6   }, _PORTB, PORTB_BIT9}, \
        {RGB(255,0,0), RGB(0,0,0),     1, {166, 186, 0,   6   }, _PORTB, PORTB_BIT8}, \
        {RGB(0,0,255), RGB(0,0,0),     1, {166, 186, 0,   6   }, _PORTB, PORTB_BIT10}

    #define KEYPAD "KeyPads/FRDM_KL05Z.bmp"
#elif defined TWR_KL25Z48M                                               // {24}
    #define DEMO_LED_1             (PORTA_BIT17)                         // (green LED) if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define DEMO_LED_2             (PORTB_BIT8)                          // (red LED) if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define DEMO_LED_3             (PORTA_BIT5)                          // (orange LED) if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define DEMO_LED_4             (PORTA_BIT16)                         // (yellow LED) if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define BLINK_LED              (DEMO_LED_1)
    #define SWITCH_3               (PORTA_BIT4)                          // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define SWITCH_4               (PORTC_BIT3)                          // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too


    #if defined USE_MAINTENANCE
        #define INIT_WATCHDOG_LED()                                      // let the port set up do this (the user can disable blinking)
    #else
        #define INIT_WATCHDOG_LED() _CONFIG_DRIVE_PORT_OUTPUT_VALUE(A, (BLINK_LED), (BLINK_LED), (PORT_SRE_SLOW | PORT_DSE_HIGH))
    #endif

    #define SHIFT_DEMO_LED_1        17                                   // since the port bits are spread out shift each to the lowest 4 bits
    #define SHIFT_DEMO_LED_2        7
    #define SHIFT_DEMO_LED_3        3
    #define SHIFT_DEMO_LED_4        13

    #define MAPPED_DEMO_LED_1       (DEMO_LED_1 >> SHIFT_DEMO_LED_1)
    #define MAPPED_DEMO_LED_2       (DEMO_LED_2 >> SHIFT_DEMO_LED_2)
    #define MAPPED_DEMO_LED_3       (DEMO_LED_3 >> SHIFT_DEMO_LED_3)
    #define MAPPED_DEMO_LED_4       (DEMO_LED_4 >> SHIFT_DEMO_LED_4)

    #define INIT_WATCHDOG_DISABLE() _CONFIG_PORT_INPUT_FAST_LOW(A, (SWITCH_3), PORT_PS_UP_ENABLE) // configure as input
    #define WATCHDOG_DISABLE()      (_READ_PORT_MASK(A, SWITCH_3) == 0)  // pull this input down at reset to disable watchdog [SW3]
    #define ACTIVATE_WATCHDOG()     SIM_COPC = (SIM_COPC_COPCLKS_1K | SIM_COPC_COPT_LONGEST) // 1.024s watchdog timeout
    #define TOGGLE_WATCHDOG_LED()  _TOGGLE_PORT(A, BLINK_LED)

    #define CONFIG_TEST_OUTPUT()                                         // we use DEMO_LED_2 which is configured by the user code (and can be disabled in parameters if required)
    #define TOGGLE_TEST_OUTPUT()    _TOGGLE_PORT(B, DEMO_LED_2)
    #define SET_TEST_OUTPUT()       _SETBITS(B, DEMO_LED_2)
    #define CLEAR_TEST_OUTPUT()     _CLEARBITS(B, DEMO_LED_2)

    #define CONFIGURE_MOUSE_INPUTS() 
    #define MOUSE_LEFT_CLICK()     (_READ_PORT_MASK(A, SWITCH_3) == 0)
    #define MOUSE_UP()             0
    #define MOUSE_DOWN()           0
    #define MOUSE_LEFT()           0
    #define MOUSE_RIGHT()          0

    #define BUTTON_KEY_DEFINITIONS  {_PORTA,        SWITCH_3, {286, 302, 298, 315 }}, \
                                    {_PORTC,        SWITCH_4, {318, 302, 333, 315 }},

        // '0'          '1'       input state   center (x,   y)   0 = circle, radius, controlling port, controlling pin 
    #define KEYPAD_LED_DEFINITIONS  \
        {RGB(0,255,0),   RGB(0,0,0),   1, {395, 150, 404, 157 }, _PORTA, DEMO_LED_1}, \
        {RGB(255,0,0),   RGB(0,0,0),   1, {395, 166, 404, 172 }, _PORTB, DEMO_LED_2}, \
        {RGB(255,128,0), RGB(0,0,0),   1, {373, 235, 382, 242 }, _PORTA, DEMO_LED_3}, \
        {RGB(255,255,0), RGB(0,0,0),   1, {373, 283, 382, 289 }, _PORTA, DEMO_LED_4}

    #define KEYPAD "KeyPads/TWR-KL25Z48M.bmp"
#elif defined FRDM_KL25Z                                                 // {24}
    #define DEMO_LED_1             (PORTB_BIT19)                         // (green LED) if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define DEMO_LED_2             (PORTD_BIT1)                          // (blue LED) if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define DEMO_LED_3             (PORTB_BIT18)                         // (red LED) if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define DEMO_LED_4             (0)                                   // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define BLINK_LED              (DEMO_LED_1)
    #define SWITCH_1               (PORTD_BIT4)                          // [J1 pin 6]
    #define SWITCH_2               (PORTA_BIT12)                         // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too

    #define SWITCH_2_PORT          _PORTA

    #if defined USE_MAINTENANCE
        #define INIT_WATCHDOG_LED()                                      // let the port set up do this (the user can disable blinking)
    #else
        #define INIT_WATCHDOG_LED() _CONFIG_DRIVE_PORT_OUTPUT_VALUE(B, (BLINK_LED), (BLINK_LED), (PORT_SRE_SLOW | PORT_DSE_HIGH))
    #endif

    #define SHIFT_DEMO_LED_1        19                                    // since the port bits are spread out shift each to the lowest 4 bits
    #define SHIFT_DEMO_LED_2        0
    #define SHIFT_DEMO_LED_3        16
    #define SHIFT_DEMO_LED_4        

    #define MAPPED_DEMO_LED_1       (DEMO_LED_1 >> SHIFT_DEMO_LED_1)
    #define MAPPED_DEMO_LED_2       (DEMO_LED_2 >> SHIFT_DEMO_LED_2)
    #define MAPPED_DEMO_LED_3       (DEMO_LED_3 >> SHIFT_DEMO_LED_3)
    #define MAPPED_DEMO_LED_4       (DEMO_LED_4 >> SHIFT_DEMO_LED_4)

    #define INIT_WATCHDOG_DISABLE() _CONFIG_PORT_INPUT_FAST_LOW(A, (SWITCH_2), PORT_PS_UP_ENABLE) // configure as input
    #define WATCHDOG_DISABLE()      (_READ_PORT_MASK(B, SWITCH_2) == 0)  // pull this input down at reset to disable watchdog [J1 pin 8]
    #define ACTIVATE_WATCHDOG()     SIM_COPC = (SIM_COPC_COPCLKS_1K | SIM_COPC_COPT_LONGEST) // 1.024s watchdog timeout
    #define TOGGLE_WATCHDOG_LED()  _TOGGLE_PORT(B, BLINK_LED)

    #define CONFIG_TEST_OUTPUT()                                         // we use DEMO_LED_2 which is configured by the user code (and can be disabled in parameters if required)
    #define TOGGLE_TEST_OUTPUT()    _TOGGLE_PORT(D, DEMO_LED_2)
    #define SET_TEST_OUTPUT()       _SETBITS(D, DEMO_LED_2)
    #define CLEAR_TEST_OUTPUT()     _CLEARBITS(D, DEMO_LED_2)

    #define CONFIGURE_MOUSE_INPUTS() 
    #define MOUSE_LEFT_CLICK()     0                                     // board has no button for click
    #define MOUSE_UP()             (iUpTilt/32)                           // use accelerometer (see iic_tests.h and TEST_MMA8451Q, based on I2C)
    #define MOUSE_DOWN()           (iDownTilt/32)
    #define MOUSE_LEFT()           (iLeftTilt/32)
    #define MOUSE_RIGHT()          (iRightTilt/32)

    #define BUTTON_KEY_DEFINITIONS  {_PORTC,        PORTC_BIT12, {257, 30,  265, 38 }}, \
                                    {_PORTC,        PORTC_BIT13, {243, 30,  252, 38 }}, \
                                    {_PORTC,        PORTC_BIT16, {228, 30,  236, 38 }}, \
                                    {_PORTC,        PORTC_BIT17, {213, 30,  222, 38 }}, \
                                    {_PORTA,        PORTA_BIT16, {198, 30,  207, 38 }}, \
                                    {_PORTA,        PORTA_BIT17, {183, 30,  193, 38 }}, \
                                    {_PORTE,        PORTE_BIT31, {168, 30,  178, 38 }}, \
                                    {_PORTD,        PORTD_BIT6,  {141, 30,  149, 38 }}, \
                                    {_PORTD,        PORTD_BIT7,  {126, 30,  134, 38 }}, \
                                    {_PORTA,        PORTA_BIT13, {257, 15,  265, 24 }}, \
                                    {_PORTD,        PORTD_BIT5,  {243, 15,  252, 24 }}, \
                                    {_PORTD,        PORTD_BIT0,  {228, 15,  236, 24 }}, \
                                    {_PORTD,        PORTD_BIT2,  {213, 15,  222, 24 }}, \
                                    {_PORTD,        PORTD_BIT3,  {198, 15,  207, 24 }}, \
                                    {_PORTD,        PORTD_BIT1,  {183, 15,  193, 24 }}, \
                                    {_PORTE,        PORTE_BIT0,  {141, 15,  149, 24 }}, \
                                    {_PORTE,        PORTE_BIT1,  {126, 15,  134, 24 }}, \
                                    {_PORTC,        PORTC_BIT9,  {278, 15,  289, 24 }}, \
                                    {_PORTC,        PORTC_BIT8,  {294, 15,  304, 24 }}, \
                                    {_PORTA,        PORTA_BIT5,  {309, 15,  319, 24 }}, \
                                    {_PORTA,        PORTA_BIT4,  {324, 15,  334, 24 }}, \
                                    {_PORTA,        PORTA_BIT12, {338, 15,  348, 24 }}, \
                                    {_PORTD,        PORTD_BIT4,  {353, 15,  363, 24 }}, \
                                    {_PORTA,        PORTA_BIT2,  {369, 15,  379, 24 }}, \
                                    {_PORTA,        PORTA_BIT1,  {383, 15,  393, 24 }}, \
                                    {_PORTC,        PORTC_BIT11, {278, 30,  289, 38 }}, \
                                    {_PORTC,        PORTC_BIT10, {294, 30,  304, 38 }}, \
                                    {_PORTC,        PORTC_BIT6,  {309, 30,  319, 38 }}, \
                                    {_PORTC,        PORTC_BIT5,  {324, 30,  334, 38 }}, \
                                    {_PORTC,        PORTC_BIT4,  {338, 30,  348, 38 }}, \
                                    {_PORTC,        PORTC_BIT3,  {353, 30,  363, 38 }}, \
                                    {_PORTC,        PORTC_BIT0,  {369, 30,  379, 38 }}, \
                                    {_PORTC,        PORTC_BIT7,  {383, 30,  393, 38 }}, \
                                    {_PORTB,        PORTB_BIT8,  {175, 280, 185, 288}}, \
                                    {_PORTB,        PORTB_BIT9,  {191, 280, 201, 288}}, \
                                    {_PORTB,        PORTB_BIT10, {204, 280, 214, 288}}, \
                                    {_PORTB,        PORTB_BIT11, {221, 280, 231, 288}}, \
                                    {_PORTE,        PORTE_BIT2,  {235, 280, 245, 288}}, \
                                    {_PORTE,        PORTE_BIT3,  {250, 280, 260, 288}}, \
                                    {_PORTE,        PORTE_BIT4,  {263, 280, 273, 288}}, \
                                    {_PORTE,        PORTE_BIT5,  {280, 280, 290, 288}}, \
                                    {_PORTD,        PORTD_BIT5,  {175, 296, 185, 306}}, \
                                    {_PORTE,        PORTE_BIT20, {309, 280, 319, 288}}, \
                                    {_PORTE,        PORTE_BIT21, {323, 280, 333, 288}}, \
                                    {_PORTE,        PORTE_BIT22, {338, 280, 348, 288}}, \
                                    {_PORTE,        PORTE_BIT23, {353, 280, 363, 288}}, \
                                    {_PORTE,        PORTE_BIT29, {369, 280, 379, 288}}, \
                                    {_PORTE,        PORTE_BIT30, {382, 280, 392, 288}}, \
                                    {_PORTB,        PORTB_BIT0,  {309, 297, 319, 307}}, \
                                    {_PORTB,        PORTB_BIT1,  {323, 297, 333, 307}}, \
                                    {_PORTB,        PORTB_BIT2,  {338, 297, 348, 307}}, \
                                    {_PORTB,        PORTB_BIT3,  {353, 297, 363, 307}}, \
                                    {_PORTC,        PORTC_BIT2,  {369, 297, 379, 307}}, \
                                    {_PORTC,        PORTC_BIT1,  {382, 297, 392, 307}}, \
                                    

    #define MULTICOLOUR_LEDS        {0, 2}                               // single LED made up of entries 0, 1 and 2 [green/red/blue]

        // '0'          '1'               input state   center (x,   y)   0 = circle, radius, controlling port, controlling pin 
    #define KEYPAD_LED_DEFINITIONS  \
        {RGB(0,255,0), RGB(40,40,40),     1, {320, 210, 0,   8   }, _PORTB, BLINK_LED}, \
        {RGB(0,0,255), RGB(20,20,20),     1, {320, 210, 0,   8   }, _PORTD, PORTD_BIT1}, \
        {RGB(255,0,0), RGB(20,20,20),     1, {320, 210, 0,   8   }, _PORTB, PORTB_BIT18}

    #define KEYPAD "KeyPads/FRDM_KL25Z.bmp"

    // Accelerometer interrupt configuration
    //
    #define ACC_INT_PRIORITY       PRIORITY_PORT_A_INT
    #define ACC_INT_PORT           PORTA
    #define ACC_INT_BIT            PORTA_BIT14
    #define ACC_INT_ASSERTED()     (_READ_PORT_MASK(A, PORTA_BIT14) == 0)

  //#define PWM_LED_CONTROL                                              // control RGB led according to accelerometer position (when I2C and accelerometer enabled)
    #define FTM2_0_ON_B                                                  // when using PWM on channel 0 of TPM2 use PTB18
    #define FTM2_1_ON_B                                                  // when using PWM on channel 1 of TPM2 use PTB19
    #define TPM0_1_ON_D                                                  // when using PWM on channel 1 of TPM0 use PTD1
#elif defined CAPUCCINO_KL27
    #define DEMO_LED_1             (PORTE_BIT1)                          // (green LED) if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define DEMO_LED_2             (PORTE_BIT20)                         // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define DEMO_LED_3             (PORTD_BIT1)                          // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define DEMO_LED_4             (0)                                   // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define BLINK_LED              (DEMO_LED_1)
    #define SWITCH_2               (PORTA_BIT4)                          // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too

    #define SWITCH_2_PORT          _PORTA

    #if defined USE_MAINTENANCE
        #define INIT_WATCHDOG_LED()                                      // let the port set up do this (the user can disable blinking)
    #else
        #define INIT_WATCHDOG_LED() _CONFIG_DRIVE_PORT_OUTPUT_VALUE(E, (BLINK_LED), (BLINK_LED), (PORT_SRE_SLOW | PORT_DSE_HIGH))
    #endif

    #define SHIFT_DEMO_LED_1        1                                    // since the port bits are spread out shift each to the lowest 4 bits
    #define SHIFT_DEMO_LED_2        19
    #define SHIFT_DEMO_LED_3        1
    #define SHIFT_DEMO_LED_4        0

    #define MAPPED_DEMO_LED_1       (DEMO_LED_1 >> SHIFT_DEMO_LED_1)
    #define MAPPED_DEMO_LED_2       (DEMO_LED_2 >> SHIFT_DEMO_LED_2)
    #define MAPPED_DEMO_LED_3       (DEMO_LED_3 >> SHIFT_DEMO_LED_3)
    #define MAPPED_DEMO_LED_4       (DEMO_LED_4 >> SHIFT_DEMO_LED_4)

    #define INIT_WATCHDOG_DISABLE() _CONFIG_PORT_INPUT_FAST_LOW(A, (SWITCH_2), PORT_PS_UP_ENABLE) // configure as input
    #define WATCHDOG_DISABLE()      (_READ_PORT_MASK(A, SWITCH_2) == 0) // pull this input down at reset to disable watchdog (hold SW2 down)
    #define ACTIVATE_WATCHDOG()     SIM_COPC = (SIM_COPC_COPCLKS_1K | SIM_COPC_COPT_LONGEST) // 1.024s watchdog timeout
    #define TOGGLE_WATCHDOG_LED()   _TOGGLE_PORT(E, BLINK_LED)

    #define CONFIG_TEST_OUTPUT()                                         // we use DEMO_LED_2 which is configured by the user code (and can be disabled in parameters if required)
    #define TOGGLE_TEST_OUTPUT()    _TOGGLE_PORT(B, DEMO_LED_2)
    #define SET_TEST_OUTPUT()       _SETBITS(B, DEMO_LED_2)
    #define CLEAR_TEST_OUTPUT()     _CLEARBITS(B, DEMO_LED_2)


    #define BUTTON_KEY_DEFINITIONS  {SWITCH_2_PORT,  SWITCH_2,  {370, 281, 393, 301}}
                                    
        // '0'          '1'       input state   center (x,   y)   0 = circle, radius, controlling port, controlling pin 
    #define KEYPAD_LED_DEFINITIONS  \
        {RGB(255,0,0), RGB(0,0,0),   1,          {156, 30,        0,           5   }, _PORTE, BLINK_LED}

    #define KEYPAD "KeyPads/Capuccino_KL27.bmp"
#elif defined FRDM_KL27Z                                                 // {24}
    #define DEMO_LED_1             (PORTB_BIT19)                         // (green LED) if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define DEMO_LED_2             (PORTB_BIT18)                         // (red LED) if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define DEMO_LED_3             (PORTA_BIT13)                         // (blue LED) if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define DEMO_LED_4             (0)                                   // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define BLINK_LED              (DEMO_LED_1)
    #define SWITCH_1               (PORTA_BIT4)                          // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define SWITCH_2               (PORTC_BIT1)                          // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too

    #define SWITCH_1_PORT          _PORTA
    #define SWITCH_2_PORT          _PORTC

    #if defined USE_MAINTENANCE
        #define INIT_WATCHDOG_LED()                                      // let the port set up do this (the user can disable blinking)
    #else
        #define INIT_WATCHDOG_LED() _CONFIG_DRIVE_PORT_OUTPUT_VALUE(B, (BLINK_LED), (BLINK_LED), (PORT_SRE_SLOW | PORT_DSE_HIGH))
    #endif

    #define SHIFT_DEMO_LED_1        19                                   // since the port bits are spread out shift each to the lowest 4 bits
    #define SHIFT_DEMO_LED_2        17
    #define SHIFT_DEMO_LED_3        11
    #define SHIFT_DEMO_LED_4        0

    #define MAPPED_DEMO_LED_1       (DEMO_LED_1 >> SHIFT_DEMO_LED_1)
    #define MAPPED_DEMO_LED_2       (DEMO_LED_2 >> SHIFT_DEMO_LED_2)
    #define MAPPED_DEMO_LED_3       (DEMO_LED_3 >> SHIFT_DEMO_LED_3)
    #define MAPPED_DEMO_LED_4       (DEMO_LED_4 >> SHIFT_DEMO_LED_4)

    #define INIT_WATCHDOG_DISABLE() _CONFIG_PORT_INPUT_FAST_LOW(C, (SWITCH_2), PORT_PS_UP_ENABLE) // configure as input
    #define WATCHDOG_DISABLE()      (_READ_PORT_MASK(C, SWITCH_2) == 0) // pull this input down at reset to disable watchdog [J1 pin 8]
    #define ACTIVATE_WATCHDOG()     SIM_COPC = (SIM_COPC_COPCLKS_1K | SIM_COPC_COPT_LONGEST) // 1.024s watchdog timeout
    #define TOGGLE_WATCHDOG_LED()  _TOGGLE_PORT(B, BLINK_LED)

    #define CONFIG_TEST_OUTPUT()                                         // we use DEMO_LED_2 which is configured by the user code (and can be disabled in parameters if required)
    #define TOGGLE_TEST_OUTPUT()    _TOGGLE_PORT(B, DEMO_LED_2)
    #define SET_TEST_OUTPUT()       _SETBITS(B, DEMO_LED_2)
    #define CLEAR_TEST_OUTPUT()     _CLEARBITS(B, DEMO_LED_2)

    #define CONFIGURE_MOUSE_INPUTS() 
    #define MOUSE_LEFT_CLICK()     0
    #define MOUSE_UP()             0
    #define MOUSE_DOWN()           0
    #define MOUSE_LEFT()           (_READ_PORT_MASK(A, SWITCH_1) == 0)   // SW1 used as mouse movement left
    #define MOUSE_RIGHT()          (_READ_PORT_MASK(C, SWITCH_2) == 0)   // SW2 used as right mouse movement right

    #define BUTTON_KEY_DEFINITIONS {SWITCH_1_PORT,  SWITCH_1,  {304, 18,  317, 29 }}, \
                                   {SWITCH_2_PORT,  SWITCH_2,  {304, 190, 317, 201}},
                                    
    #define MULTICOLOUR_LEDS       {0, 2}                               // single LED made up of entries 0, 1 and 2 [green/red/blue]

        // '0'          '1'       input state   center (x,   y)   0 = circle, radius, controlling port, controlling pin 
    #define KEYPAD_LED_DEFINITIONS  \
        {RGB(0,255,0), RGB(0,0,0),     1, {259, 122, 0,   6   }, _PORTB, BLINK_LED}, \
        {RGB(255,0,0), RGB(0,0,0),     1, {259, 122, 0,   6   }, _PORTB, DEMO_LED_2}, \
        {RGB(0,0,255), RGB(0,0,0),     1, {259, 122, 0,   6   }, _PORTA, DEMO_LED_3}
    #define KEYPAD "KeyPads/FRDM_KL27Z.bmp"

    // Accelerometer interrupt configuration
    //
    #define ACC_INT_PRIORITY       PRIORITY_PORT_C_INT
    #define ACC_INT_PORT           PORTC
    #define ACC_INT_BIT            PORTC_BIT3
    #define ACC_INT_ASSERTED()     (_READ_PORT_MASK(C, PORTC_BIT3) == 0)

    // Defines for the WAVE file recording start/stop button
    //
    #define WAVE_DISK_START_STOP_INT_PRIORITY    PRIORITY_PORT_C_INT
    #define WAVE_DISK_START_STOP_INT_PORT        PORTC
    #define WAVE_DISK_START_STOP_INT_BIT         SWITCH_2
    #define RECORDER_WAVE_FILE_NAME              "FRDM_KL27Z.wav"
#elif defined FRDM_KL82Z
    #define DEMO_LED_1             (PORTC_BIT2)                          // (green LED) if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define DEMO_LED_2             (PORTC_BIT1)                          // (red LED) if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define DEMO_LED_3             (PORTC_BIT0)                          // (blue LED) if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define DEMO_LED_4             (0)                                   // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define BLINK_LED              (DEMO_LED_1)
    #define SWITCH_2               (PORTA_BIT4)                          // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define SWITCH_3               (PORTD_BIT0)                          // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too

    #define SWITCH_2_PORT          _PORTA
    #define SWITCH_3_PORT          _PORTD

    #if defined USE_MAINTENANCE
        #define INIT_WATCHDOG_LED()                                      // let the port set up do this (the user can disable blinking)
    #else
        #define INIT_WATCHDOG_LED() _CONFIG_DRIVE_PORT_OUTPUT_VALUE(C, (BLINK_LED), (BLINK_LED), (PORT_SRE_SLOW | PORT_DSE_HIGH))
    #endif

    #define SHIFT_DEMO_LED_1        2                                   // since the port bits are spread out shift each to the lowest 4 bits
    #define SHIFT_DEMO_LED_2        0
    #define SHIFT_DEMO_LED_3        2
    #define SHIFT_DEMO_LED_4        0

    #define MAPPED_DEMO_LED_1       (DEMO_LED_1 >> SHIFT_DEMO_LED_1)
    #define MAPPED_DEMO_LED_2       (DEMO_LED_2 >> SHIFT_DEMO_LED_2)
    #define MAPPED_DEMO_LED_3       (DEMO_LED_3 << SHIFT_DEMO_LED_3)
    #define MAPPED_DEMO_LED_4       (DEMO_LED_4 >> SHIFT_DEMO_LED_4)

    #define INIT_WATCHDOG_DISABLE() _CONFIG_PORT_INPUT_FAST_LOW(A, (SWITCH_2), PORT_PS_UP_ENABLE) // configure as input
    #define WATCHDOG_DISABLE()      (_READ_PORT_MASK(A, SWITCH_2) == 0) // pull this input down at reset to disable watchdog [J1 pin 8]
    #define ACTIVATE_WATCHDOG()     UNLOCK_WDOG(); WDOG_TOVALL = (2000/5); WDOG_TOVALH = 0; WDOG_STCTRLH = (WDOG_STCTRLH_STNDBYEN | WDOG_STCTRLH_WAITEN | WDOG_STCTRLH_STOPEN | WDOG_STCTRLH_WDOGEN) // 1.024s watchdog timeout
    #define TOGGLE_WATCHDOG_LED()  _TOGGLE_PORT(C, BLINK_LED)

    #if defined USE_MAINTENANCE
        #define CONFIG_TEST_OUTPUT()                                     // we use DEMO_LED_2 which is configured by the user code (and can be disabled in parameters if required)
    #else
        #define CONFIG_TEST_OUTPUT() _CONFIG_DRIVE_PORT_OUTPUT_VALUE(B, (DEMO_LED_2), (DEMO_LED_2), (PORT_SRE_FAST | PORT_DSE_HIGH))
    #endif
    #define TOGGLE_TEST_OUTPUT()    _TOGGLE_PORT(C, DEMO_LED_2)
    #define SET_TEST_OUTPUT()       _SETBITS(C, DEMO_LED_2)
    #define CLEAR_TEST_OUTPUT()     _CLEARBITS(C, DEMO_LED_2)

    #define CONFIGURE_MOUSE_INPUTS() 
    #define MOUSE_LEFT_CLICK()     0
    #define MOUSE_UP()             0
    #define MOUSE_DOWN()           0
    #define MOUSE_LEFT()           (_READ_PORT_MASK(A, SWITCH_2) == 0)   // SW2 used as mouse movement left
    #define MOUSE_RIGHT()          (_READ_PORT_MASK(D, SWITCH_3) == 0)   // SW3 used as right mouse movement right

    #define BUTTON_KEY_DEFINITIONS {SWITCH_2_PORT,  SWITCH_2,  {328, 204, 345, 215}}, \
                                   {SWITCH_3_PORT,  SWITCH_3,  {330, 16,  343, 30 }},
                                    
    #define MULTICOLOUR_LEDS       {0, 2}                               // single LED made up of entries 0, 1 and 2 [green/red/blue]

        // '0'          '1'       input state   center (x,   y)   0 = circle, radius, controlling port, controlling pin 
    #define KEYPAD_LED_DEFINITIONS  \
        {RGB(0,255,0), RGB(0,0,0),     1, {251, 65, 0,   6   }, _PORTC, BLINK_LED}, \
        {RGB(255,0,0), RGB(0,0,0),     1, {251, 65, 0,   6   }, _PORTC, DEMO_LED_2}, \
        {RGB(0,0,255), RGB(0,0,0),     1, {251, 65, 0,   6   }, _PORTC, DEMO_LED_3}

    #define KEYPAD "KeyPads/FRDM_KL82Z.bmp"

    // Accelerometer interrupt configuration
    //
    #define ACC_INT_PRIORITY       PRIORITY_PORT_C_INT
    #define ACC_INT_PORT           PORTC
    #define ACC_INT_BIT            PORTC_BIT3
    #define ACC_INT_ASSERTED()     (_READ_PORT_MASK(C, PORTC_BIT3) == 0)

    // Defines for the WAVE file recording start/stop button
    //
    #define WAVE_DISK_START_STOP_INT_PRIORITY    PRIORITY_PORT_C_INT
    #define WAVE_DISK_START_STOP_INT_PORT        PORTC
    #define WAVE_DISK_START_STOP_INT_BIT         SWITCH_2
    #define RECORDER_WAVE_FILE_NAME              "FRDM_KL28Z.wav"
#elif defined rcARM_KL26
    #define DEMO_LED_1             (PORTD_BIT0)                          // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define DEMO_LED_2             (PORTD_BIT5)                          // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define DEMO_LED_3             (PORTD_BIT7)                          // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define DEMO_LED_4             (0)                                   // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define BLINK_LED              (DEMO_LED_1)
    #define SWITCH_1               (PORTE_BIT20)                         // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too

    #define SWITCH_1_PORT          _PORTE

    #if defined USE_MAINTENANCE
        #define INIT_WATCHDOG_LED()                                      // let the port set up do this (the user can disable blinking)
    #else
        #define INIT_WATCHDOG_LED() _CONFIG_DRIVE_PORT_OUTPUT_VALUE(D, (BLINK_LED), (BLINK_LED), (PORT_SRE_SLOW | PORT_DSE_HIGH))
    #endif

    #define SHIFT_DEMO_LED_1       0                                     // since the port bits are spread out shift each to the lowest 4 bits
    #define SHIFT_DEMO_LED_2       4
    #define SHIFT_DEMO_LED_3       5
    #define SHIFT_DEMO_LED_4       0

    #define MAPPED_DEMO_LED_1      (DEMO_LED_1 >> SHIFT_DEMO_LED_1)
    #define MAPPED_DEMO_LED_2      (DEMO_LED_2 >> SHIFT_DEMO_LED_2)
    #define MAPPED_DEMO_LED_3      (DEMO_LED_3 >> SHIFT_DEMO_LED_3)
    #define MAPPED_DEMO_LED_4      (DEMO_LED_4 >> SHIFT_DEMO_LED_4)

    #define INIT_WATCHDOG_DISABLE() _CONFIG_PORT_INPUT_FAST_LOW(E, (SWITCH_1), PORT_PS_UP_ENABLE) // configure as input
    #define WATCHDOG_DISABLE()     (_READ_PORT_MASK(E, SWITCH_1) == 0)  // pull this input down at reset to disable watchdog
    #define ACTIVATE_WATCHDOG()    SIM_COPC = (SIM_COPC_COPCLKS_1K | SIM_COPC_COPT_LONGEST) // 1.024s watchdog timeout
    #define TOGGLE_WATCHDOG_LED()  _TOGGLE_PORT(D, BLINK_LED)

    #define CONFIG_TEST_OUTPUT()                                         // we use DEMO_LED_2 which is configured by the user code (and can be disabled in parameters if required)
    #define TOGGLE_TEST_OUTPUT()   _TOGGLE_PORT(D, DEMO_LED_2)
    #define SET_TEST_OUTPUT()      _SETBITS(D, DEMO_LED_2)
    #define CLEAR_TEST_OUTPUT()    _CLEARBITS(D, DEMO_LED_2)

    #define BACKLIGHT_PIN          PORTB_BIT0
    #define AUDIO_PIN              PORTB_BIT2
    #define FRAM_HOLD_PIN          PORTC_BIT0
    #define FRAM_WP_PIN            PORTB_BI19

    #define KEYPAD "KeyPads/rcARM.bmp"
#elif defined FRDM_KL26Z                                                 // {24}
    #define DEMO_LED_1             (PORTE_BIT31)                         // (green LED) if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define DEMO_LED_2             (PORTE_BIT29)                         // (red LED) if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define DEMO_LED_3             (PORTD_BIT5)                          // (blue LED) if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define DEMO_LED_4             (0)                                   // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define BLINK_LED              (DEMO_LED_1)
    #define SWITCH_1               (PORTC_BIT3)                          // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define SWITCH_2               (PORTA_BIT12)                         // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too

    #define SWITCH_1_PORT          _PORTC
    #define SWITCH_2_PORT          _PORTA

    #if defined USE_MAINTENANCE
        #define INIT_WATCHDOG_LED() _CONFIG_PORT_INPUT_FAST_LOW(C, (SWITCH_1), PORT_PS_UP_ENABLE) // let the port set up do this (the user can disable blinking)
    #else
        #define INIT_WATCHDOG_LED() _CONFIG_DRIVE_PORT_OUTPUT_VALUE(E, (BLINK_LED), (BLINK_LED), (PORT_SRE_SLOW | PORT_DSE_HIGH))
    #endif

    #define SHIFT_DEMO_LED_1        31                                   // since the port bits are spread out shift each to the lowest 4 bits
    #define SHIFT_DEMO_LED_2        28
    #define SHIFT_DEMO_LED_3        3
    #define SHIFT_DEMO_LED_4        0

    #define MAPPED_DEMO_LED_1       (DEMO_LED_1 >> SHIFT_DEMO_LED_1)
    #define MAPPED_DEMO_LED_2       (DEMO_LED_2 >> SHIFT_DEMO_LED_2)
    #define MAPPED_DEMO_LED_3       (DEMO_LED_3 >> SHIFT_DEMO_LED_3)
    #define MAPPED_DEMO_LED_4       (DEMO_LED_4 >> SHIFT_DEMO_LED_4)

    #define INIT_WATCHDOG_DISABLE() _CONFIG_PORT_INPUT_FAST_LOW(A, (SWITCH_2), PORT_PS_UP_ENABLE) // configure as input
    #define WATCHDOG_DISABLE()      (_READ_PORT_MASK(A, SWITCH_2) == 0)  // pull this input down at reset to disable watchdog [J1 pin 8]
    #define ACTIVATE_WATCHDOG()     SIM_COPC = (SIM_COPC_COPCLKS_1K | SIM_COPC_COPT_LONGEST) // 1.024s watchdog timeout
    #define TOGGLE_WATCHDOG_LED()  _TOGGLE_PORT(E, BLINK_LED)

    #define CONFIG_TEST_OUTPUT()                                         // we use DEMO_LED_2 which is configured by the user code (and can be disabled in parameters if required)
    #define TOGGLE_TEST_OUTPUT()   _TOGGLE_PORT(E, DEMO_LED_2)
    #define SET_TEST_OUTPUT()      _SETBITS(E, DEMO_LED_2)
    #define CLEAR_TEST_OUTPUT()    _CLEARBITS(E, DEMO_LED_2)

    #define CONFIGURE_MOUSE_INPUTS() 
    #define MOUSE_LEFT_CLICK()     (_READ_PORT_MASK(C, SWITCH_1) == 0)   // SW1 used as left mouse click
    #define MOUSE_UP()             0
    #define MOUSE_DOWN()           0
    #define MOUSE_LEFT()           0
    #define MOUSE_RIGHT()          0

    #define BUTTON_KEY_DEFINITIONS  {_PORTC,        PORTC_BIT3,  {233, 76,  245, 91 }}, // SW1
                                    
    #define MULTICOLOUR_LEDS        {0, 2}                           // single LED made up of entries 0, 1 and 2 [green/red/blue]


    #if defined SUPPORT_KEY_SCAN
                                        // column, row, key press rectangle
        #define KEYPAD_KEY_DEFINITIONS  {0, 0, {256, 36,  300, 101}}, \
                                        {0, 1, {256, 37,  300, 179}}
    #endif

        // '0'          '1'       input state   center (x,   y)   0 = circle, radius, controlling port, controlling pin 
    #define KEYPAD_LED_DEFINITIONS  \
        {RGB(0,255,0), RGB(0,0,0),     1, {198, 136, 0,   6   }, _PORTE, BLINK_LED}, \
        {RGB(255,0,0), RGB(0,0,0),     1, {198, 136, 0,   6   }, _PORTE, PORTE_BIT29}, \
        {RGB(0,0,255), RGB(0,0,0),     1, {198, 136, 0,   6   }, _PORTB, PORTB_BIT18}

    #define KEYPAD "KeyPads/FRDM_KL26Z.bmp"

    // Accelerometer interrupt configuration
    //
    #define ACC_INT_PRIORITY       PRIORITY_PORT_D_INT
    #define ACC_INT_PORT           PORTD
    #define ACC_INT_BIT            PORTD_BIT1
    #define ACC_INT_ASSERTED()     (_READ_PORT_MASK(D, PORTD_BIT1) == 0)
    #define ACC_USE_INT2                                                 // INT1 is shared with the push button on this board so use INT2

  //#define PWM_LED_CONTROL                                              // control RGB led according to accelerometer position (when I2C and accelerometer enabled - needs PWM)
    #define TPM0_2_ON_E                                                  // when using PWM on channel 1 of TPM0 use PTE25
    #define TPM0_4_ON_E                                                  // when using PWM on channel 4 of TPM2 use PTE31
    #define FTM0_5_ON_D                                                  // when using PWM on channel 5 of TPM0 use PTD5
#elif defined TEENSY_LC
    #define DEMO_LED_1             (PORTC_BIT5)                          // (green LED) if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define DEMO_LED_2             (PORTC_BIT4)                          // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define DEMO_LED_3             (PORTC_BIT6)                          // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define DEMO_LED_4             (PORTC_BIT7)                          // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define BLINK_LED              (DEMO_LED_1)
    #define SWITCH_1               (PORTB_BIT0)                          // (pin 16) if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define SWITCH_2               (PORTB_BIT1)                          // (pin 17) if the port is changed (eg. A to B) the port macros will require appropriate adjustment too

    #if defined USE_MAINTENANCE
        #define INIT_WATCHDOG_LED()                                      // let the port set up do this (the user can disable blinking)
    #else
        #define INIT_WATCHDOG_LED() _CONFIG_DRIVE_PORT_OUTPUT_VALUE(C, (BLINK_LED), (BLINK_LED), (PORT_SRE_SLOW | PORT_DSE_HIGH))
    #endif

    #define SHIFT_DEMO_LED_1        5                                   // since the port bits are spread out shift each to the lowest 4 bits
    #define SHIFT_DEMO_LED_2        3
    #define SHIFT_DEMO_LED_3        4
    #define SHIFT_DEMO_LED_4        4

    #define MAPPED_DEMO_LED_1       (DEMO_LED_1 >> SHIFT_DEMO_LED_1)
    #define MAPPED_DEMO_LED_2       (DEMO_LED_2 >> SHIFT_DEMO_LED_2)
    #define MAPPED_DEMO_LED_3       (DEMO_LED_3 >> SHIFT_DEMO_LED_3)
    #define MAPPED_DEMO_LED_4       (DEMO_LED_4 >> SHIFT_DEMO_LED_4)

    #define INIT_WATCHDOG_DISABLE() _CONFIG_PORT_INPUT_FAST_LOW(B, (SWITCH_1 | SWITCH_2), PORT_PS_UP_ENABLE) // configure as input
    #define WATCHDOG_DISABLE()      (_READ_PORT_MASK(B, SWITCH_2) == 0)  // pull this input down at reset to disable watchdog [pin 17]
    #define ACTIVATE_WATCHDOG()     SIM_COPC = (SIM_COPC_COPCLKS_1K | SIM_COPC_COPT_LONGEST) // 1.024s watchdog timeout
    #define TOGGLE_WATCHDOG_LED()  _TOGGLE_PORT(C, BLINK_LED)

    #define CONFIG_TEST_OUTPUT()                                         // we use DEMO_LED_2 which is configured by the user code (and can be disabled in parameters if required)
    #define TOGGLE_TEST_OUTPUT()    _TOGGLE_PORT(C, DEMO_LED_2)
    #define SET_TEST_OUTPUT()       _SETBITS(D, DEMO_LED_2)
    #define CLEAR_TEST_OUTPUT()     _CLEARBITS(D, DEMO_LED_2)

    #define CONFIGURE_MOUSE_INPUTS() 
    #define MOUSE_LEFT_CLICK()     0
    #define MOUSE_UP()             0
    #define MOUSE_DOWN()           0
    #define MOUSE_LEFT()           (_READ_PORT_MASK(B, SWITCH_1) == 0)   // used as left mouse movement
    #define MOUSE_RIGHT()          (_READ_PORT_MASK(B, SWITCH_1) == 0)   // used as right mouse movement

        // '0'          '1'       input state   center (x,   y)   0 = circle, radius, controlling port, controlling pin 
    #define KEYPAD_LED_DEFINITIONS  \
        {RGB(255,30,0), RGB(40,40,40),1,{335, 40,  344, 57 }, _PORTC, BLINK_LED}, \
        {RGB(25,25,25), RGB(255,0,0),0, {56,  179, 0,   5  }, _PORTB, PORTB_BIT16}, \
        {RGB(25,25,25), RGB(255,0,0),0, {84,  179, 0,   5  }, _PORTB, PORTB_BIT17}, \
        {RGB(25,25,25), RGB(255,0,0),0, {111, 179, 0,   5  }, _PORTD, PORTD_BIT0}, \
        {RGB(25,25,25), RGB(255,0,0),0, {139, 179, 0,   5  }, _PORTA, PORTA_BIT1}, \
        {RGB(25,25,25), RGB(255,0,0),0, {164, 179, 0,   5  }, _PORTA, PORTA_BIT2}, \
        {RGB(25,25,25), RGB(255,0,0),0, {192, 179, 0,   5  }, _PORTD, PORTD_BIT7}, \
        {RGB(25,25,25), RGB(255,0,0),0, {219, 179, 0,   5  }, _PORTD, PORTD_BIT4}, \
        {RGB(25,25,25), RGB(255,0,0),0, {246, 179, 0,   5  }, _PORTD, PORTD_BIT2}, \
        {RGB(25,25,25), RGB(255,0,0),0, {273, 179, 0,   5  }, _PORTD, PORTD_BIT3}, \
        {RGB(25,25,25), RGB(255,0,0),0, {300, 179, 0,   5  }, _PORTC, PORTC_BIT3}, \
        {RGB(25,25,25), RGB(255,0,0),0, {327, 179, 0,   5  }, _PORTC, PORTC_BIT4}, \
        {RGB(25,25,25), RGB(255,0,0),0, {353, 179, 0,   5  }, _PORTC, PORTC_BIT6}, \
        {RGB(25,25,25), RGB(255,0,0),0, {380, 179, 0,   5  }, _PORTC, PORTC_BIT7}, \
        {RGB(25,25,25), RGB(255,0,0),0, {380, 17,  0,   5  }, _PORTC, BLINK_LED}, \
        {RGB(25,25,25), RGB(255,0,0),0, {353, 17, 0,   5  }, _PORTD, PORTD_BIT1}, \
        {RGB(25,25,25), RGB(255,0,0),0, {326, 17, 0,   5  }, _PORTC, PORTC_BIT0}, \
        {RGB(25,25,25), RGB(255,0,0),0, {299, 17, 0,   5  }, _PORTB, PORTB_BIT0}, \
        {RGB(25,25,25), RGB(255,0,0),0, {273, 17, 0,   5  }, _PORTB, PORTB_BIT1}, \
        {RGB(25,25,25), RGB(255,0,0),0, {246, 17, 0,   5  }, _PORTB, PORTB_BIT3}, \
        {RGB(25,25,25), RGB(255,0,0),0, {219, 17, 0,   5  }, _PORTB, PORTB_BIT2}, \
        {RGB(25,25,25), RGB(255,0,0),0, {192, 17, 0,   5  }, _PORTD, PORTD_BIT5}, \
        {RGB(25,25,25), RGB(255,0,0),0, {164, 17, 0,   5  }, _PORTD, PORTD_BIT6}, \
        {RGB(25,25,25), RGB(255,0,0),0, {138, 17, 0,   5  }, _PORTC, PORTC_BIT1}, \
        {RGB(25,25,25), RGB(255,0,0),0, {111, 17, 0,   5  }, _PORTC, PORTC_BIT2}, \
        {RGB(25,25,25), RGB(255,0,0),0, {138, 44, 0,   5  }, _PORTE, PORTE_BIT20}, \
        {RGB(25,25,25), RGB(255,0,0),0, {165, 44, 0,   5  }, _PORTE, PORTE_BIT21}, \
        {RGB(25,25,25), RGB(255,0,0),0, {380, 44, 0,   5  }, _PORTE, PORTE_BIT30},

    #define BUTTON_KEY_DEFINITIONS  {_PORTB,  PORTB_BIT16,  {48,  171,  65,  187 }}, \
                                    {_PORTB,  PORTB_BIT17,  {76,  171,  92,  187 }}, \
                                    {_PORTD,  PORTD_BIT0,   {103, 171,  121, 187 }}, \
                                    {_PORTA,  PORTA_BIT1,   {129, 171,  147, 187 }}, \
                                    {_PORTA,  PORTA_BIT2,   {157, 171,  172, 187 }}, \
                                    {_PORTD,  PORTD_BIT7,   {183, 171,  200, 187 }}, \
                                    {_PORTD,  PORTD_BIT4,   {210, 171,  227, 187 }}, \
                                    {_PORTD,  PORTD_BIT2,   {238, 171,  253, 187 }}, \
                                    {_PORTD,  PORTD_BIT3,   {264, 171,  281, 187 }}, \
                                    {_PORTC,  PORTC_BIT3,   {292, 171,  307, 187 }}, \
                                    {_PORTC,  PORTC_BIT4,   {318, 171,  335, 187 }}, \
                                    {_PORTC,  PORTC_BIT6,   {347, 171,  363, 187 }}, \
                                    {_PORTC,  PORTC_BIT7,   {372, 171,  391, 187 }}, \
                                    {_PORTC,  BLINK_LED,    {372, 9,    391, 23  }}, \
                                    {_PORTD,  PORTD_BIT1,   {347, 9,    363, 23  }}, \
                                    {_PORTC,  PORTC_BIT0,   {318, 9,    335, 23  }}, \
                                    {_PORTB,  PORTB_BIT0,   {292, 9,    307, 23  }}, \
                                    {_PORTB,  PORTB_BIT1,   {264, 9,    281, 23  }}, \
                                    {_PORTB,  PORTB_BIT3,   {238, 9,    253, 23  }}, \
                                    {_PORTB,  PORTB_BIT2,   {210, 9,    227, 23  }}, \
                                    {_PORTD,  PORTD_BIT5,   {183, 9,    200, 23  }}, \
                                    {_PORTD,  PORTD_BIT6,   {157, 9,    172, 23  }}, \
                                    {_PORTC,  PORTC_BIT1,   {129, 9,    147, 23  }}, \
                                    {_PORTC,  PORTC_BIT2,   {103, 9,    121, 23  }}, \
                                    {_PORTE,  PORTE_BIT20,  {130, 33,   145, 51  }}, \
                                    {_PORTE,  PORTE_BIT21,  {155, 33,   173, 51  }}, \
                                    {_PORTE,  PORTE_BIT30,  {373, 37,   388, 52  }},

    #define KEYPAD "KeyPads/TEENSY_LC.bmp"


  //#define PWM_LED_CONTROL                                              // control RGB led according to accelerometer position (when I2C and accelerometer enabled - needs PWM)
    #define TPM0_2_ON_E                                                  // when using PWM on channel 1 of TPM0 use PTE25
    #define TPM0_4_ON_E                                                  // when using PWM on channel 4 of TPM2 use PTE31
    #define FTM0_5_ON_D                                                  // when using PWM on channel 5 of TPM0 use PTD5

    #if defined ENC424J600_INTERFACE                                     // if the ENC424J600 is being used as SPI connected Ethernet  controller
        #define ENC424J600_IRQ       PORTC_BIT3                          // the IRQ line used
        #define ENC424J600_IRQ_PORT  PORTC                               // the port that the IRQ line is on
        #define ENC424J600_IRQ_PRIORITY PRIORITY_PORT_C_INT              // the interrupt priority
    #endif
#elif defined TWR_KL43Z48M
    #define DEMO_LED_1             (PORTA_BIT12)                         // (green LED1) if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define DEMO_LED_2             (PORTA_BIT13)                         // (red LED2) if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define DEMO_LED_3             (PORTB_BIT0)                          // (green LED3) if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define DEMO_LED_4             (PORTB_BIT19)                         // (red LED4) if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define BLINK_LED              (DEMO_LED_1)
    #define SWITCH_2               (PORTA_BIT4)                          // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define SWITCH_3               (PORTA_BIT5)                          // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too

    #define SWITCH_2_PORT          _PORTA
    #define SWITCH_3_PORT          _PORTA

    #if defined USE_MAINTENANCE
        #define INIT_WATCHDOG_LED()                                      // let the port set up do this (the user can disable blinking)
    #else
        #define INIT_WATCHDOG_LED() _CONFIG_DRIVE_PORT_OUTPUT_VALUE(A, (BLINK_LED), (BLINK_LED), (PORT_SRE_SLOW | PORT_DSE_HIGH))
    #endif

    #define SHIFT_DEMO_LED_1        12                                   // since the port bits are spread out shift each to the lowest 4 bits
    #define SHIFT_DEMO_LED_2        12
    #define SHIFT_DEMO_LED_3        2                                    // left shift
    #define SHIFT_DEMO_LED_4        16

    #define MAPPED_DEMO_LED_1       (DEMO_LED_1 >> SHIFT_DEMO_LED_1)
    #define MAPPED_DEMO_LED_2       (DEMO_LED_2 >> SHIFT_DEMO_LED_2)
    #define MAPPED_DEMO_LED_3       (DEMO_LED_3 << SHIFT_DEMO_LED_3)
    #define MAPPED_DEMO_LED_4       (DEMO_LED_4 >> SHIFT_DEMO_LED_4)

    #define INIT_WATCHDOG_DISABLE() _CONFIG_PORT_INPUT_FAST_LOW(A, (SWITCH_3), PORT_PS_UP_ENABLE); _CONFIG_PORT_INPUT_FAST_LOW(C, (SWITCH_3), PORT_PS_UP_ENABLE) // configure as input
    #define WATCHDOG_DISABLE()      (_READ_PORT_MASK(A, SWITCH_3) == 0)  // hold switch 3 down at reset to disable watchdog
    #define ACTIVATE_WATCHDOG()     SIM_COPC = (SIM_COPC_COPCLKS_1K | SIM_COPC_COPT_LONGEST) // 1.024s watchdog timeout
    #if defined SUPPORT_SLCD && !defined SUPPORT_RTC
        #define TOGGLE_WATCHDOG_LED()  _TOGGLE_PORT(A, BLINK_LED); if (IS_POWERED_UP(5, SIM_SCGC5_SLCD)) { TOGGLE_SLCD(44, 0x01); } // toggle LED and blink LCD ":"
    #else
        #define TOGGLE_WATCHDOG_LED()  _TOGGLE_PORT(A, BLINK_LED)
    #endif

    #define CONFIG_TEST_OUTPUT()                                         // we use DEMO_LED_2 which is configured by the user code (and can be disabled in parameters if required)
    #define TOGGLE_TEST_OUTPUT()   _TOGGLE_PORT(A, DEMO_LED_2)
    #define SET_TEST_OUTPUT()      _SETBITS(A, DEMO_LED_2)
    #define CLEAR_TEST_OUTPUT()    _CLEARBITS(A, DEMO_LED_2)

    #define CONFIGURE_MOUSE_INPUTS()                                     // SW2 already configured by INIT_WATCHDOG_DISABLE as input
    #define MOUSE_LEFT_CLICK()     (_READ_PORT_MASK(A, SWITCH_2) == 0)   // SW2 used as left mouse click
    #define MOUSE_UP()             (iUpTilt/4)                           // use accelerometer (see iic_tests.h and TEST_MMA8451Q, based on I2C)
    #define MOUSE_DOWN()           (iDownTilt/4)
    #define MOUSE_LEFT()           (iLeftTilt/4)
    #define MOUSE_RIGHT()          (iRightTilt/4) 

    #define KEYPAD "KeyPads/TWR_KL43Z48M.bmp"

        // '0'          '1'            input state   center (x,   y)   0 = circle, radius, controlling port, controlling pin 
    #define KEYPAD_LED_DEFINITIONS  \
        {RGB(0,255,0), RGB(40,40,40),  1, {23,  169, 30,  180 }, _PORTA, DEMO_LED_1}, \
        {RGB(255,0,0), RGB(40,40,40),  1, {23,  143, 30,  152 }, _PORTA, DEMO_LED_2}, \
        {RGB(0,255,0), RGB(40,40,40),  1, {23,  114, 30,  128 }, _PORTB, DEMO_LED_3}, \
        {RGB(255,0,0), RGB(40,40,40),  1, {23,  87,  30,  99  }, _PORTB, DEMO_LED_4}

    #define BUTTON_KEY_DEFINITIONS  {SWITCH_2_PORT, SWITCH_2, {28,  197, 42, 213}}, \
                                    {SWITCH_3_PORT, SWITCH_3, {28,  235, 42, 245}}
#elif defined KL43Z_256_32_CL
    #define INIT_WATCHDOG_LED()
    #define INIT_WATCHDOG_DISABLE()
    #define WATCHDOG_DISABLE()     0
    #define ACTIVATE_WATCHDOG()    SIM_COPC = (SIM_COPC_COPCLKS_1K | SIM_COPC_COPT_LONGEST) // 1.024s watchdog timeout
    #define MAPPED_DEMO_LED_1      0
    #define MAPPED_DEMO_LED_2      0

    #define CONFIG_TEST_OUTPUT()
    #define TOGGLE_TEST_OUTPUT()  _TOGGLE_PORT(C, (EN_1V8))

    #define SW_UPDATE              PORTA_BIT4
    #define BT_MAIN                PORTA_BIT12
    #define BT_RESET               PORTA_BIT13
    #define TC_RST                 PORTB_BIT1
    #define TC_ERROR               PORTB_BIT19
    #define TC_INTOUT              PORTB_BIT0
    #define ALARM_BATTERY          PORTC_BIT4
    #define DSP_RST                PORTE_BIT0
    #define PWM_CTRL_0             PORTE_BIT31
    #define LEDCTL_RED_DS          PORTD_BIT6
    #define LEDCTL_RED_OE          PORTD_BIT7
    #define LEDCTL_RED_RST         PORTE_BIT21
    #define LEDCTL_RED_STCP        PORTE_BIT1
    #define LEDCTL_RED_SHCP        PORTE_BIT25
    #define BAT_ALR                PORTA_BIT5
    #define BAT_LED_CTRL           PORTC_BIT6
    #define BAT_CHG                PORTC_BIT7
    #define EN_1V8                 PORTC_BIT0
    #define EN_12V                 PORTE_BIT30
    #define VBAT_MON               PORTE_BIT29
    #define INIT_PORTS()           _CONFIG_DRIVE_PORT_OUTPUT_VALUE_FAST_LOW(A, (BT_MAIN | BT_RESET | BAT_ALR), (0), (PORT_SRE_SLOW | PORT_DSE_HIGH)); \
                                   _CONFIG_DRIVE_PORT_OUTPUT_VALUE_FAST_LOW(B, (TC_RST), (0), (PORT_SRE_SLOW | PORT_DSE_HIGH)); \
                                   _CONFIG_DRIVE_PORT_OUTPUT_VALUE_FAST_LOW(C, (BAT_LED_CTRL | BAT_CHG | EN_1V8), (0), (PORT_SRE_SLOW | PORT_DSE_HIGH)); \
                                   _CONFIG_DRIVE_PORT_OUTPUT_VALUE_FAST_LOW(D, (LEDCTL_RED_DS | LEDCTL_RED_OE), (0), (PORT_SRE_SLOW | PORT_DSE_HIGH)); \
                                   _CONFIG_DRIVE_PORT_OUTPUT_VALUE_FAST_LOW(E, (DSP_RST | LEDCTL_RED_STCP), (0), (PORT_SRE_SLOW | PORT_DSE_HIGH)); \
                                   _CONFIG_DRIVE_PORT_OUTPUT_VALUE_FAST_HIGH(E, (PWM_CTRL_0 | LEDCTL_RED_RST | LEDCTL_RED_SHCP | EN_12V), (PWM_CTRL_0), (PORT_SRE_SLOW | PORT_DSE_HIGH)); \
                                   _CONFIG_PORT_INPUT_FAST_LOW(A, (SW_UPDATE), PORT_PS_UP_ENABLE); \
                                   _CONFIG_PORT_INPUT_FAST_LOW(B, (TC_INTOUT), PORT_PS_UP_ENABLE); \
                                   _CONFIG_PORT_INPUT_FAST_HIGH(B, (TC_ERROR), PORT_PS_UP_ENABLE); \
                                   _CONFIG_PORT_INPUT_FAST_LOW(C, (ALARM_BATTERY), PORT_PS_UP_ENABLE); \
                                   _CONFIG_PORT_INPUT_FAST_HIGH(E, (VBAT_MON), PORT_PS_UP_ENABLE)

    #define TOGGLE_WATCHDOG_LED()

#elif defined FRDM_KL43Z
    #define DEMO_LED_1             (PORTE_BIT31)                         // (red LED) if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define DEMO_LED_2             (PORTD_BIT5)                          // (green LED) if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define DEMO_LED_3             (PORTB_BIT16)                         // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define DEMO_LED_4             (PORTB_BIT17)                         // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define BLINK_LED              (DEMO_LED_1)
    #define SWITCH_1               (PORTA_BIT4)                          // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define SWITCH_3               (PORTC_BIT3)                          // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too

    #define SWITCH_1_PORT          _PORTA
    #define SWITCH_3_PORT          _PORTC

    #if defined USE_MAINTENANCE
        #define INIT_WATCHDOG_LED()                                      // let the port set up do this (the user can disable blinking)
    #else
        #define INIT_WATCHDOG_LED() _CONFIG_DRIVE_PORT_OUTPUT_VALUE(D, (BLINK_LED), (BLINK_LED), (PORT_SRE_SLOW | PORT_DSE_HIGH))
    #endif

    #define SHIFT_DEMO_LED_1        31                                    // since the port bits are spread out shift each to the lowest 4 bits
    #define SHIFT_DEMO_LED_2        4
    #define SHIFT_DEMO_LED_3        14
    #define SHIFT_DEMO_LED_4        14

    #define MAPPED_DEMO_LED_1       (DEMO_LED_1 >> SHIFT_DEMO_LED_1)
    #define MAPPED_DEMO_LED_2       (DEMO_LED_2 >> SHIFT_DEMO_LED_2)
    #define MAPPED_DEMO_LED_3       (DEMO_LED_3 >> SHIFT_DEMO_LED_3)
    #define MAPPED_DEMO_LED_4       (DEMO_LED_4 >> SHIFT_DEMO_LED_4)

    #define INIT_WATCHDOG_DISABLE() _CONFIG_PORT_INPUT_FAST_LOW(A, (SWITCH_1), PORT_PS_UP_ENABLE); _CONFIG_PORT_INPUT_FAST_LOW(C, (SWITCH_3), PORT_PS_UP_ENABLE) // configure as input
    #define WATCHDOG_DISABLE()      (_READ_PORT_MASK(C, SWITCH_3) == 0) // hold switch 3 down at reset to disable watchdog
    #define ACTIVATE_WATCHDOG()     SIM_COPC = (SIM_COPC_COPCLKS_1K | SIM_COPC_COPT_LONGEST) // 1.024s watchdog timeout
    #if defined SUPPORT_SLCD && !defined SUPPORT_RTC
        #define TOGGLE_WATCHDOG_LED()  _TOGGLE_PORT(E, BLINK_LED); if (IS_POWERED_UP(5, SIM_SCGC5_SLCD)) { TOGGLE_SLCD(44, 0x01); } // toggle LED and blink LCD ":"
    #else
        #define TOGGLE_WATCHDOG_LED()  _TOGGLE_PORT(E, BLINK_LED)
    #endif

    #define CONFIG_TEST_OUTPUT()                                         // we use DEMO_LED_2 which is configured by the user code (and can be disabled in parameters if required)
    #define TOGGLE_TEST_OUTPUT()    _TOGGLE_PORT(D, DEMO_LED_2)
    #define SET_TEST_OUTPUT()       _SETBITS(D, DEMO_LED_2)
    #define CLEAR_TEST_OUTPUT()     _CLEARBITS(D, DEMO_LED_2)

    #define CONFIGURE_MOUSE_INPUTS()                                     // SW1 already configured by INIT_WATCHDOG_DISABLE as input
    #define MOUSE_LEFT_CLICK()     (_READ_PORT_MASK(A, SWITCH_1) == 0)   // SW1 used as left mouse click
    #define MOUSE_UP()             (iUpTilt/4)                           // use accelerometer (see iic_tests.h and TEST_MMA8451Q, based on I2C)
    #define MOUSE_DOWN()           (iDownTilt/4)
    #define MOUSE_LEFT()           (iLeftTilt/4)
    #define MOUSE_RIGHT()          (iRightTilt/4) 

    #define KEYPAD "KeyPads/FRDM_KL43Z.bmp"

        // '0'          '1'            input state   center (x,   y)   0 = circle, radius, controlling port, controlling pin 
    #define KEYPAD_LED_DEFINITIONS  \
        {RGB(0,255,0), RGB(40,40,40),  1, {185, 47,  191, 53  }, _PORTE, DEMO_LED_1}, \
        {RGB(255,0,0), RGB(40,40,40),  1, {197, 67,  206, 74  }, _PORTD, DEMO_LED_2}

    #define BUTTON_KEY_DEFINITIONS  {SWITCH_1_PORT, SWITCH_1, {262, 20, 271, 29}}, \
                                    {SWITCH_3_PORT, SWITCH_3, {262, 164,271,174}}
#elif defined FRDM_KL46Z                                                 // {22}
    #define DEMO_LED_1             (PORTD_BIT5)                          // (green LED) if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define DEMO_LED_2             (PORTE_BIT29)                         // (red LED) if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define DEMO_LED_3             (PORTC_BIT11)                         // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define DEMO_LED_4             (PORTC_BIT16)                         // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define BLINK_LED              (DEMO_LED_1)
    #define SWITCH_1               (PORTC_BIT3)                          // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define SWITCH_3               (PORTC_BIT12)                         // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too

    #define SWITCH_1_PORT          _PORTC
    #define SWITCH_3_PORT          _PORTC

    #if defined USE_MAINTENANCE
        #define INIT_WATCHDOG_LED()                                      // let the port set up do this (the user can disable blinking)
    #else
        #define INIT_WATCHDOG_LED() _CONFIG_DRIVE_PORT_OUTPUT_VALUE(D, (BLINK_LED), (BLINK_LED), (PORT_SRE_SLOW | PORT_DSE_HIGH))
    #endif

    #define SHIFT_DEMO_LED_1        5                                    // since the port bits are spread out shift each to the lowest 4 bits
    #define SHIFT_DEMO_LED_2        28
    #define SHIFT_DEMO_LED_3        9
    #define SHIFT_DEMO_LED_4        13

    #define MAPPED_DEMO_LED_1       (DEMO_LED_1 >> SHIFT_DEMO_LED_1)
    #define MAPPED_DEMO_LED_2       (DEMO_LED_2 >> SHIFT_DEMO_LED_2)
    #define MAPPED_DEMO_LED_3       (DEMO_LED_3 >> SHIFT_DEMO_LED_3)
    #define MAPPED_DEMO_LED_4       (DEMO_LED_4 >> SHIFT_DEMO_LED_4)

    #define INIT_WATCHDOG_DISABLE() _CONFIG_PORT_INPUT(C, (SWITCH_1 | SWITCH_3), PORT_PS_UP_ENABLE) // configure as input
    #define WATCHDOG_DISABLE()      (_READ_PORT_MASK(C, SWITCH_3) == 0)  // pull this input down at reset to disable watchdog
    #define ACTIVATE_WATCHDOG()     SIM_COPC = (SIM_COPC_COPCLKS_1K | SIM_COPC_COPT_LONGEST) // 1.024s watchdog timeout
    #if defined SUPPORT_SLCD && !defined SUPPORT_RTC
        #define TOGGLE_WATCHDOG_LED()  _TOGGLE_PORT(D, BLINK_LED); if (IS_POWERED_UP(5, SIM_SCGC5_SLCD)) { TOGGLE_SLCD(11, 0x01); } // toggle LED and blink LCD ":"
    #else
        #define TOGGLE_WATCHDOG_LED()  _TOGGLE_PORT(D, BLINK_LED)
    #endif

    #define CONFIG_TEST_OUTPUT()                                         // we use DEMO_LED_2 which is configured by the user code (and can be disabled in parameters if required)
    #define TOGGLE_TEST_OUTPUT()    _TOGGLE_PORT(E, DEMO_LED_2)
    #define SET_TEST_OUTPUT()       _SETBITS(E, DEMO_LED_2)
    #define CLEAR_TEST_OUTPUT()     _CLEARBITS(E, DEMO_LED_2)

    #if defined I2C_INTERFACE
        #define CONFIGURE_MOUSE_INPUTS()                                 // SW1 already configured by INIT_WATCHDOG_DISABLE as input
        #define MOUSE_LEFT_CLICK()     (_READ_PORT_MASK(C, SWITCH_1) == 0) // SW1 used as left mouse click
        #define MOUSE_UP()             (iUpTilt/4)                       // use accelerometer (see iic_tests.h and TEST_MMA8451Q, based on I2C)
        #define MOUSE_DOWN()           (iDownTilt/4)
        #define MOUSE_LEFT()           (iLeftTilt/4)
        #define MOUSE_RIGHT()          (iRightTilt/4)
    #else
        #define CONFIGURE_MOUSE_INPUTS()                                 // SW1 and SW3 already configured by INIT_WATCHDOG_DISABLE as input
        #define MOUSE_LEFT_CLICK()     0
        #define MOUSE_UP()             0
        #define MOUSE_DOWN()           0
        #define MOUSE_LEFT()           (_READ_PORT_MASK(C, SWITCH_3) == 0)
        #define MOUSE_RIGHT()          (_READ_PORT_MASK(C, SWITCH_1) == 0)
    #endif

    #define KEYPAD "KeyPads/FRDM_KL46Z.bmp"

        // '0'          '1'            input state   center (x,   y)   0 = circle, radius, controlling port, controlling pin 
    #define KEYPAD_LED_DEFINITIONS  \
        {RGB(0,255,0), RGB(40,40,40),  1, {63, 143, 68, 154 }, _PORTD, PORTD_BIT5}, \
        {RGB(255,0,0), RGB(40,40,40),  1, {89, 115, 94, 125 }, _PORTE, PORTE_BIT29}

    #define BUTTON_KEY_DEFINITIONS  {SWITCH_1_PORT, SWITCH_1, {23,  27, 34,  44}}, \
                                    {SWITCH_3_PORT, SWITCH_3, {222, 30, 232, 46}}

    #define USB_HOST_POWER_CONFIG()                                      // the board needs to be modified for host mode with no switchable control
    #define USB_HOST_POWER_ON()                                          // populate jumper J16 and insert the jumper to connect P5V_USB to P5V_KL46Z
    #define USB_HOST_POWER_OFF()                                         // as the circuit says "electrical protection is not provided - use it at your own risk"
#elif defined TWR_KL46Z48M                                               // {23}
    #define DEMO_LED_1             (PORTA_BIT17)                         // (green LED) if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define DEMO_LED_2             (PORTB_BIT8)                          // (red LED) if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define DEMO_LED_3             (PORTE_BIT26)                         // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define DEMO_LED_4             (PORTA_BIT16)                         // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define BLINK_LED              (DEMO_LED_1)
    #define SWITCH_2               (PORTC_BIT3)                          // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define SWITCH_4               (PORTA_BIT4)                          // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too

    #define SWITCH_1_PORT          _PORTC
    #define SWITCH_2_PORT          _PORTA

    #if defined USE_MAINTENANCE
        #define INIT_WATCHDOG_LED()                                      // let the port set up do this (the user can disable blinking)
    #else
        #define INIT_WATCHDOG_LED() _CONFIG_DRIVE_PORT_OUTPUT_VALUE(A, (BLINK_LED), (BLINK_LED), (PORT_SRE_SLOW | PORT_DSE_HIGH))
    #endif

    #define SHIFT_DEMO_LED_1        17                                   // since the port bits are spread out shift each to the lowest 4 bits
    #define SHIFT_DEMO_LED_2        7
    #define SHIFT_DEMO_LED_3        24
    #define SHIFT_DEMO_LED_4        13

    #define MAPPED_DEMO_LED_1       (DEMO_LED_1 >> SHIFT_DEMO_LED_1)
    #define MAPPED_DEMO_LED_2       (DEMO_LED_2 >> SHIFT_DEMO_LED_2)
    #define MAPPED_DEMO_LED_3       (DEMO_LED_3 >> SHIFT_DEMO_LED_3)
    #define MAPPED_DEMO_LED_4       (DEMO_LED_4 >> SHIFT_DEMO_LED_4)

    #define INIT_WATCHDOG_DISABLE() _CONFIG_PORT_INPUT_FAST_LOW(A, (SWITCH_4), PORT_PS_UP_ENABLE) // configure as input
    #define WATCHDOG_DISABLE()      (!_READ_PORT_MASK(A, SWITCH_4))      // pull this input down at reset to disable watchdog [hold SW4]
    #define ACTIVATE_WATCHDOG()     SIM_COPC = (SIM_COPC_COPCLKS_1K | SIM_COPC_COPT_LONGEST) // 1.024s watchdog timeout
    #if defined SUPPORT_SLCD
        #define TOGGLE_WATCHDOG_LED()   _TOGGLE_PORT(A, BLINK_LED); if (IS_POWERED_UP(5, SIM_SCGC5_SLCD)) { TOGGLE_SLCD(24, 0x08); } // toggle LED and freescale logo in SLCD
    #else
        #define TOGGLE_WATCHDOG_LED()   _TOGGLE_PORT(A, BLINK_LED)
    #endif

    #define CONFIG_TEST_OUTPUT()                                         // we use DEMO_LED_2 which is configured by the user code (and can be disabled in parameters if required)
    #define TOGGLE_TEST_OUTPUT()   _TOGGLE_PORT(B, DEMO_LED_2)
    #define SET_TEST_OUTPUT()      _SETBITS(B, DEMO_LED_2)
    #define CLEAR_TEST_OUTPUT()    _CLEARBITS(B, DEMO_LED_2)

    #define CONFIGURE_MOUSE_INPUTS()                                     // SW4 already configured by INIT_WATCHDOG_DISABLE as input
    #define MOUSE_LEFT_CLICK()     (_READ_PORT_MASK(A, SWITCH_4) == 0)   // SW4 used as left mouse click
    #define MOUSE_UP()             (iUpTilt/4)                           // use accelerometer (see iic_tests.h and TEST_MMA8451Q, based on I2C)
    #define MOUSE_DOWN()           (iDownTilt/4)
    #define MOUSE_LEFT()           (iRightTilt/4)
    #define MOUSE_RIGHT()          (iLeftTilt/4)

    #define BUTTON_KEY_DEFINITIONS  {_PORTC,    SWITCH_2,  {7,   106, 19,  115 }}, \
                                    {_PORTA,    SWITCH_4,  {196, 164, 211, 177 }}
                                    

        // '0'          '1'           input state   center (x,   y)   0 = circle, radius, controlling port, controlling pin 
    #define KEYPAD_LED_DEFINITIONS  \
        {RGB(0,255,0),   RGB(20,20,20), 1, {162, 154, 171, 162 }, _PORTA, BLINK_LED}, \
        {RGB(255,0,0),   RGB(20,20,20), 1, {162, 162, 171, 170 }, _PORTB, DEMO_LED_2}, \
        {RGB(180,180,20),RGB(40,40,40), 1, {221, 125, 226, 133 }, _PORTE, DEMO_LED_3}, \
        {RGB(255,255,20),RGB(40,40,40), 1, {221, 146, 226, 153 }, _PORTA, DEMO_LED_4}

    #define KEYPAD "KeyPads/TWR_KL46ZM48.bmp"
#elif defined TWR_KV10Z32
    #define DEMO_LED_1             (PORTC_BIT1)                          // (yellow/green LED) if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define DEMO_LED_2             (PORTE_BIT25)                         // (yellow LED) if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define DEMO_LED_3             (PORTC_BIT3)                          // (yellow/green LED) if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define DEMO_LED_4             (PORTC_BIT4)                          // (yellow LED) if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define DEMO_LED_5             (PORTD_BIT4)                          // (yellow/green LED) if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define DEMO_LED_6             (PORTD_BIT5)                          // (yellow LED) if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define DEMO_LED_7             (PORTD_BIT6)                          // (yellow/orange LED) if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define DEMO_LED_8             (PORTD_BIT7)                          // (red LED) if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define BLINK_LED              (DEMO_LED_1)
    #define SWITCH_1               (PORTA_BIT4)                          // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define SWITCH_2               (PORTB_BIT0)                          // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too

    #define SWITCH_1_PORT          _PORTA
    #define SWITCH_2_PORT          _PORTB

    #if defined USE_MAINTENANCE
        #define INIT_WATCHDOG_LED()                                      // let the port set up do this (the user can disable blinking)
    #else
        #define INIT_WATCHDOG_LED() _CONFIG_DRIVE_PORT_OUTPUT_VALUE(C, (BLINK_LED), (BLINK_LED), (PORT_SRE_SLOW | PORT_DSE_HIGH))
    #endif

    #define SHIFT_DEMO_LED_1        1                                    // since the port bits are spread out shift each to the lowest 4 bits
    #define SHIFT_DEMO_LED_2        24
    #define SHIFT_DEMO_LED_3        1
    #define SHIFT_DEMO_LED_4        1

    #define MAPPED_DEMO_LED_1       (DEMO_LED_1 >> SHIFT_DEMO_LED_1)
    #define MAPPED_DEMO_LED_2       (DEMO_LED_2 >> SHIFT_DEMO_LED_2)
    #define MAPPED_DEMO_LED_3       (DEMO_LED_3 >> SHIFT_DEMO_LED_3)
    #define MAPPED_DEMO_LED_4       (DEMO_LED_4 >> SHIFT_DEMO_LED_4)

    #define INIT_WATCHDOG_DISABLE() _CONFIG_PORT_INPUT_FAST_LOW(B, (SWITCH_2), PORT_PS_UP_ENABLE) // configure as input
    #define WATCHDOG_DISABLE()      (!_READ_PORT_MASK(B, SWITCH_2))      // pull this input down at reset to disable watchdog [hold SW2]
    #define ACTIVATE_WATCHDOG()     UNLOCK_WDOG(); WDOG_TOVALL = (2000/5); WDOG_TOVALH = 0; WDOG_STCTRLH = (WDOG_STCTRLH_STNDBYEN | WDOG_STCTRLH_WAITEN | WDOG_STCTRLH_STOPEN | WDOG_STCTRLH_WDOGEN) // 1.024s watchdog timeout
    #define TOGGLE_WATCHDOG_LED()   _TOGGLE_PORT(C, BLINK_LED)

    #define CONFIG_TEST_OUTPUT()                                         // we use DEMO_LED_2 which is configured by the user code (and can be disabled in parameters if required)
    #define TOGGLE_TEST_OUTPUT()    _TOGGLE_PORT(E, DEMO_LED_2)
    #define SET_TEST_OUTPUT()       _SETBITS(E, DEMO_LED_2)
    #define CLEAR_TEST_OUTPUT()     _CLEARBITS(E, DEMO_LED_2)


    #define BUTTON_KEY_DEFINITIONS  {SWITCH_1_PORT,    SWITCH_1,  {7,   106, 19,  115 }}, \
                                    {SWITCH_2_PORT,    SWITCH_2,  {196, 164, 211, 177 }}
                                    

        // '0'          '1'           input state   center (x,   y)   0 = circle, radius, controlling port, controlling pin 
    #define KEYPAD_LED_DEFINITIONS  \
        {RGB(128,255,0), RGB(20,20,20), 1, {376, 135, 389, 139 }, _PORTC, BLINK_LED}, \
        {RGB(255,255,0), RGB(20,20,20), 1, {376, 126, 389, 130 }, _PORTE, DEMO_LED_2}, \
        {RGB(128,255,0), RGB(20,20,20), 1, {376, 118, 389, 123 }, _PORTC, DEMO_LED_3}, \
        {RGB(255,255,0), RGB(20,20,20), 1, {376, 111, 389, 115 }, _PORTE, DEMO_LED_4}, \
        {RGB(128,255,0), RGB(20,20,20), 1, {376, 102, 389, 106 }, _PORTD, DEMO_LED_5}, \
        {RGB(255,255,0), RGB(20,20,20), 1, {376, 144, 389, 149 }, _PORTD, DEMO_LED_6}, \
        {RGB(255,128,0), RGB(20,20,20), 1, {376, 153, 389, 158 }, _PORTD, DEMO_LED_7}, \
        {RGB(255,0,0),   RGB(20,20,20), 1, {376, 163, 389, 167 }, _PORTD, DEMO_LED_8}, \

    #define KEYPAD "KeyPads/TWR_KV10Z32.bmp"
#elif defined FRDM_KV31F
    #define DEMO_LED_1             (PORTD_BIT7)                          // (green LED) if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define DEMO_LED_2             (PORTD_BIT1)                          // (red LED) if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define DEMO_LED_3             (PORTE_BIT25)                         // (red BLUE) if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define DEMO_LED_4             (PORTE_BIT1)                          // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define BLINK_LED              (DEMO_LED_1)
    #define SWITCH_2               (PORTA_BIT4)                          // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define SWITCH_3               (PORTE_BIT4)                          // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too

    #define SWITCH_2_PORT          _PORTA
    #define SWITCH_3_PORT          _PORTE

    #if defined USE_MAINTENANCE && !defined REMOVE_PORT_INITIALISATIONS
        #define INIT_WATCHDOG_LED()                                      // let the port set up do this (the user can disable blinking)
    #else
        #define INIT_WATCHDOG_LED() _CONFIG_DRIVE_PORT_OUTPUT_VALUE(D, (BLINK_LED), (BLINK_LED), (PORT_SRE_SLOW | PORT_DSE_HIGH))
    #endif

    #define SHIFT_DEMO_LED_1        7                                    // since the port bits are spread out shift each to the lowest 4 bits
    #define SHIFT_DEMO_LED_2        0
    #define SHIFT_DEMO_LED_3        23
    #define SHIFT_DEMO_LED_4        2

    #define MAPPED_DEMO_LED_1       (DEMO_LED_1 >> SHIFT_DEMO_LED_1)
    #define MAPPED_DEMO_LED_2       (DEMO_LED_2 >> SHIFT_DEMO_LED_2)
    #define MAPPED_DEMO_LED_3       (DEMO_LED_3 >> SHIFT_DEMO_LED_3)
    #define MAPPED_DEMO_LED_4       (DEMO_LED_4 << SHIFT_DEMO_LED_4)

    #define INIT_WATCHDOG_DISABLE() _CONFIG_PORT_INPUT_FAST_LOW(A, (SWITCH_2), PORT_PS_UP_ENABLE) // configure as input
    #define WATCHDOG_DISABLE()      (_READ_PORT_MASK(A, SWITCH_2) == 0)  // pull this input down at reset to disable watchdog [hold SW2]
    #define ACTIVATE_WATCHDOG()     UNLOCK_WDOG(); WDOG_TOVALL = (2000/5); WDOG_TOVALH = 0; WDOG_STCTRLH = (WDOG_STCTRLH_STNDBYEN | WDOG_STCTRLH_WAITEN | WDOG_STCTRLH_STOPEN | WDOG_STCTRLH_WDOGEN) // 1.024s watchdog timeout
    #define TOGGLE_WATCHDOG_LED()   _TOGGLE_PORT(D, BLINK_LED)

    #define CONFIG_TEST_OUTPUT()                                         // we use DEMO_LED_2 which is configured by the user code (and can be disabled in parameters if required)
    #define TOGGLE_TEST_OUTPUT()    _TOGGLE_PORT(D, DEMO_LED_2)
    #define SET_TEST_OUTPUT()       _SETBITS(D, DEMO_LED_2)
    #define CLEAR_TEST_OUTPUT()     _CLEARBITS(D, DEMO_LED_2)


    #define BUTTON_KEY_DEFINITIONS  {SWITCH_2_PORT,    SWITCH_2,  {515, 24,  538, 41  }}, \
                                    {SWITCH_3_PORT,    SWITCH_3,  {515, 341, 538, 357 }}
                                    

        // '0'          '1'           input state   center (x,   y)   0 = circle, radius, controlling port, controlling pin 
    #define KEYPAD_LED_DEFINITIONS  \
        {RGB(0,   255, 0), RGB(20,20,20), 1, {541, 264, 0, 10 }, _PORTD, BLINK_LED}, \
        {RGB(255, 0,   0), RGB(20,20,20), 1, {541, 264, 0, 10 }, _PORTD, DEMO_LED_2}, \
        {RGB(0,   0, 255), RGB(20,20,20), 1, {541, 264, 0, 10 }, _PORTE, DEMO_LED_3}

    #define MULTICOLOUR_LEDS        {0, 2}                               // single LED made up of entries 0, 1 and 6

    #define KEYPAD "KeyPads/FRDM_KV31F.bmp"
#elif defined TWR_KV31F120M
    #define DEMO_LED_1             (PORTD_BIT7)                          // (green LED) if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define DEMO_LED_2             (PORTB_BIT19)                         // (orange LED) if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define DEMO_LED_3             (PORTE_BIT0)                          // (red LED) if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define DEMO_LED_4             (PORTE_BIT1)                          // (yellow LED) if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define BLINK_LED              (DEMO_LED_1)
    #define SWITCH_1               (PORTC_BIT6)                          // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define SWITCH_2               (PORTC_BIT11)                         // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define SWITCH_3               (PORTA_BIT4)                          // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define SWITCH_4               (PORTE_BIT25)                         // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too

    #define SWITCH_1_PORT          _PORTC
    #define SWITCH_2_PORT          _PORTC
    #define SWITCH_3_PORT          _PORTA
    #define SWITCH_4_PORT          _PORTE

    #if defined USE_MAINTENANCE
        #define INIT_WATCHDOG_LED()                                      // let the port set up do this (the user can disable blinking)
    #else
        #define INIT_WATCHDOG_LED() _CONFIG_DRIVE_PORT_OUTPUT_VALUE(D, (BLINK_LED), (BLINK_LED), (PORT_SRE_SLOW | PORT_DSE_HIGH))
    #endif

    #define SHIFT_DEMO_LED_1        7                                    // since the port bits are spread out shift each to the lowest 4 bits
    #define SHIFT_DEMO_LED_2        18
    #define SHIFT_DEMO_LED_3        2
    #define SHIFT_DEMO_LED_4        2

    #define MAPPED_DEMO_LED_1       (DEMO_LED_1 >> SHIFT_DEMO_LED_1)
    #define MAPPED_DEMO_LED_2       (DEMO_LED_2 >> SHIFT_DEMO_LED_2)
    #define MAPPED_DEMO_LED_3       (DEMO_LED_3 << SHIFT_DEMO_LED_3)
    #define MAPPED_DEMO_LED_4       (DEMO_LED_4 << SHIFT_DEMO_LED_4)

    #define INIT_WATCHDOG_DISABLE() _CONFIG_PORT_INPUT_FAST_LOW(C, (SWITCH_2), PORT_PS_UP_ENABLE) // configure as input
    #define WATCHDOG_DISABLE()      (_READ_PORT_MASK(C, SWITCH_2) == 0)  // pull this input down at reset to disable watchdog [hold SW2]
    #define ACTIVATE_WATCHDOG()     UNLOCK_WDOG(); WDOG_TOVALL = (2000/5); WDOG_TOVALH = 0; WDOG_STCTRLH = (WDOG_STCTRLH_STNDBYEN | WDOG_STCTRLH_WAITEN | WDOG_STCTRLH_STOPEN | WDOG_STCTRLH_WDOGEN) // 1.024s watchdog timeout
    #define TOGGLE_WATCHDOG_LED()   _TOGGLE_PORT(D, BLINK_LED)

    #define CONFIG_TEST_OUTPUT()                                         // we use DEMO_LED_2 which is configured by the user code (and can be disabled in parameters if required)
    #define TOGGLE_TEST_OUTPUT()    _TOGGLE_PORT(B, DEMO_LED_2)
    #define SET_TEST_OUTPUT()       _SETBITS(B, DEMO_LED_2)
    #define CLEAR_TEST_OUTPUT()     _CLEARBITS(B, DEMO_LED_2)


    #define BUTTON_KEY_DEFINITIONS  {SWITCH_1_PORT,    SWITCH_1,  {301, 79,  322, 98  }}, \
                                    {SWITCH_2_PORT,    SWITCH_2,  {301, 115, 322, 134 }}, \
                                    {SWITCH_3_PORT,    SWITCH_3,  {301, 147, 322, 167 }}, \
                                    {SWITCH_4_PORT,    SWITCH_4,  {301, 180, 322, 200 }}
                                    

        // '0'          '1'           input state   center (x,   y)   0 = circle, radius, controlling port, controlling pin 
    #define KEYPAD_LED_DEFINITIONS  \
        {RGB(0,   255, 0), RGB(20,20,20), 1, {278, 188, 283, 195 }, _PORTD, BLINK_LED}, \
        {RGB(128, 255, 0), RGB(20,20,20), 1, {278, 155, 283, 162 }, _PORTB, DEMO_LED_2}, \
        {RGB(255, 0,   0), RGB(20,20,20), 1, {278, 121, 283, 128 }, _PORTE, DEMO_LED_3}, \
        {RGB(255, 255, 0), RGB(20,20,20), 1, {278, 86,  283, 94  }, _PORTE, DEMO_LED_4}, \
        {RGB(255, 0,   0), RGB(20,20,20), 1, {280, 140, 0, 4 }, _PORTE, DEMO_LED_3}, \
        {RGB(0,   255, 0), RGB(20,20,20), 1, {280, 140, 0, 4 }, _PORTD, BLINK_LED}, \
        {RGB(128, 255, 0), RGB(20,20,20), 1, {280, 140, 0, 4 }, _PORTB, DEMO_LED_2}

    #define MULTICOLOUR_LEDS        {4, 6}                               // single LED made up of entries 4, 5 and 6

    #define KEYPAD "KeyPads/TWR_KV31.bmp"
#elif defined TWR_KW21D256 || defined TWR_KW24D512
    #define DEMO_LED_1             (PORTD_BIT4)                          // (blue LED) if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define DEMO_LED_2             (PORTD_BIT5)                          // (blue LED) if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define DEMO_LED_3             (PORTD_BIT6)                          // (blue LED) if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define DEMO_LED_4             (PORTD_BIT7)                          // (blue LED) if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define BLINK_LED              (DEMO_LED_1)

    #define SWITCH_1               (PORTC_BIT4)                          // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define SWITCH_2               (PORTC_BIT5)                          // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define SWITCH_3               (PORTC_BIT6)                          // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define SWITCH_4               (PORTC_BIT7)                          // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too

    #define SWITCH_1_PORT          _PORTC
    #define SWITCH_2_PORT          _PORTC
    #define SWITCH_3_PORT          _PORTC
    #define SWITCH_4_PORT          _PORTC

    #if defined USE_MAINTENANCE
        #define INIT_WATCHDOG_LED()                                      // let the port set up do this (the user can disable blinking)
    #else
        #define INIT_WATCHDOG_LED() _CONFIG_DRIVE_PORT_OUTPUT_VALUE(D, (BLINK_LED), (BLINK_LED), (PORT_SRE_SLOW | PORT_DSE_HIGH))
    #endif

    #define SHIFT_DEMO_LED_1        4                                    // since the port bits may be spread out shift each to the lowest 4 bits
    #define SHIFT_DEMO_LED_2        4
    #define SHIFT_DEMO_LED_3        4
    #define SHIFT_DEMO_LED_4        4

    #define MAPPED_DEMO_LED_1       (DEMO_LED_1 >> SHIFT_DEMO_LED_1)
    #define MAPPED_DEMO_LED_2       (DEMO_LED_2 >> SHIFT_DEMO_LED_2)
    #define MAPPED_DEMO_LED_3       (DEMO_LED_3 << SHIFT_DEMO_LED_3)
    #define MAPPED_DEMO_LED_4       (DEMO_LED_4 << SHIFT_DEMO_LED_4)

    #define INIT_WATCHDOG_DISABLE() _CONFIG_PORT_INPUT_FAST_LOW(C, (SWITCH_2), PORT_PS_UP_ENABLE) // configure as input
    #define WATCHDOG_DISABLE()      (_READ_PORT_MASK(C, SWITCH_2) == 0)  // pull this input down at reset to disable watchdog [hold SW2]
    #define ACTIVATE_WATCHDOG()     UNLOCK_WDOG(); WDOG_TOVALL = (2000/5); WDOG_TOVALH = 0; WDOG_STCTRLH = (WDOG_STCTRLH_STNDBYEN | WDOG_STCTRLH_WAITEN | WDOG_STCTRLH_STOPEN | WDOG_STCTRLH_WDOGEN) // 1.024s watchdog timeout
    #define TOGGLE_WATCHDOG_LED()   _TOGGLE_PORT(D, BLINK_LED)

    #define CONFIG_TEST_OUTPUT()                                         // we use DEMO_LED_2 which is configured by the user code (and can be disabled in parameters if required)
    #define TOGGLE_TEST_OUTPUT()    _TOGGLE_PORT(D, DEMO_LED_2)
    #define SET_TEST_OUTPUT()       _SETBITS(D, DEMO_LED_2)
    #define CLEAR_TEST_OUTPUT()     _CLEARBITS(D, DEMO_LED_2)


    #define BUTTON_KEY_DEFINITIONS  {SWITCH_1_PORT,    SWITCH_1,  {213, 118, 224, 132 }}, \
                                    {SWITCH_2_PORT,    SWITCH_2,  {213, 144, 224, 161 }}, \
                                    {SWITCH_3_PORT,    SWITCH_3,  {213, 184, 224, 197 }}, \
                                    {SWITCH_4_PORT,    SWITCH_4,  {213, 227, 224, 243 }}
                                    

        // '0'          '1'           input state   center (x,   y)   0 = circle, radius, controlling port, controlling pin 
    #define KEYPAD_LED_DEFINITIONS  \
        {RGB(0, 0, 255), RGB(20,20,20), 1, {197, 232, 203, 242 }, _PORTD, BLINK_LED}, \
        {RGB(0, 0, 255), RGB(20,20,20), 1, {197, 184, 203, 194 }, _PORTD, DEMO_LED_2}, \
        {RGB(0, 0, 255), RGB(20,20,20), 1, {197, 141, 203, 151 }, _PORTD, DEMO_LED_3}, \
        {RGB(0, 0, 255), RGB(20,20,20), 1, {197, 112, 203, 122 }, _PORTD, DEMO_LED_4},

    #define KEYPAD "KeyPads/TWR_KW2X.bmp"
#elif defined KWIKSTIK
    #define DEMO_LED_1             (0)
    #define DEMO_LED_2             (0)
    #define DEMO_LED_3             (0)
    #define DEMO_LED_4             (0)
    #define BLINK_LED              (DEMO_LED_1)
    #define AUDIO_AMPLIFIER_OUTPUT (PORTE_BIT28)                         // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too

    #define INIT_WATCHDOG_LED()                                          // no LED available

    #define SHIFT_DEMO_LED_1        7                                    // since the port bits are spread out shift each to the lowest 4 bits
    #define SHIFT_DEMO_LED_2        8
    #define SHIFT_DEMO_LED_3        9
    #define SHIFT_DEMO_LED_4        11

    #define MAPPED_DEMO_LED_1       (DEMO_LED_1 >> SHIFT_DEMO_LED_1)
    #define MAPPED_DEMO_LED_2       (DEMO_LED_2 >> SHIFT_DEMO_LED_2)
    #define MAPPED_DEMO_LED_3       (DEMO_LED_3 >> SHIFT_DEMO_LED_3)
    #define MAPPED_DEMO_LED_4       (DEMO_LED_4 >> SHIFT_DEMO_LED_4)

    #define INIT_WATCHDOG_DISABLE() _CONFIG_PORT_INPUT_FAST_LOW(E, (PORTE_BIT0 | PORTE_BIT1), 0); // configure as input
    #define WATCHDOG_DISABLE()      (!_READ_PORT_MASK(E, PORTE_BIT0))    // right side rear connector - short pins 4 and 2 together to disable watchdog
    #define ACTIVATE_WATCHDOG()     UNLOCK_WDOG(); WDOG_TOVALL = (2000/5); WDOG_TOVALH = 0; WDOG_STCTRLH = (WDOG_STCTRLH_STNDBYEN | WDOG_STCTRLH_WAITEN | WDOG_STCTRLH_STOPEN | WDOG_STCTRLH_WDOGEN) // watchdog enabled to generate reset on 2s timeout (no further updates allowed)
    #if defined SUPPORT_SLCD
        #define TOGGLE_WATCHDOG_LED()   if (IS_POWERED_UP(3, SIM_SCGC3_SLCD)) { TOGGLE_SLCD(3TO0, 0x1000); } // blink freescale logo in the SLCD
    #else
        #define TOGGLE_WATCHDOG_LED()
    #endif

    #define CONFIG_TEST_OUTPUT()
    #define TOGGLE_TEST_OUTPUT()
    #define SET_TEST_OUTPUT()
    #define CLEAR_TEST_OUTPUT()

    #define CONFIGURE_MOUSE_INPUTS()
    #define MOUSE_LEFT_CLICK()     0
    #define MOUSE_UP()             0
    #define MOUSE_DOWN()           0
    #define MOUSE_LEFT()           0
    #define MOUSE_RIGHT()          0
#else                                                                    // TWR_K40X256 / TWR_K53N512 / TWR_K40D100M
    #define DEMO_LED_1             (PORTC_BIT7)                          // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define DEMO_LED_2             (PORTC_BIT8)                          // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #if defined TWR_K53N512
        #define DEMO_LED_3         0
        #define DEMO_LED_4         0
    #else
        #define DEMO_LED_3         (PORTC_BIT9)                          // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
        #define DEMO_LED_4         (PORTB_BIT11)                         // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #endif
    #define BLINK_LED              (DEMO_LED_1)
    #define SWITCH_1               (PORTC_BIT5)                          // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define SWITCH_2               (PORTC_BIT13)                      // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define USB_HOST_POWER_ENABLE  (PORTB_BIT9)
    #if defined USE_MAINTENANCE
        #define INIT_WATCHDOG_LED()                                      // let the port set up do this (the user can disable blinking)
    #else
        #define INIT_WATCHDOG_LED() _CONFIG_DRIVE_PORT_OUTPUT_VALUE(C, (BLINK_LED), (BLINK_LED), (/*PORT_ODE | */PORT_SRE_SLOW | PORT_DSE_HIGH))
    #endif

    #define SHIFT_DEMO_LED_1        7                                    // since the port bits are spread out shift each to the lowest 4 bits
    #define SHIFT_DEMO_LED_2        8
    #define SHIFT_DEMO_LED_3        9
    #define SHIFT_DEMO_LED_4        11

    #define MAPPED_DEMO_LED_1       (DEMO_LED_1 >> SHIFT_DEMO_LED_1)
    #define MAPPED_DEMO_LED_2       (DEMO_LED_2 >> SHIFT_DEMO_LED_2)
    #define MAPPED_DEMO_LED_3       (DEMO_LED_3 >> SHIFT_DEMO_LED_3)
    #define MAPPED_DEMO_LED_4       (DEMO_LED_4 >> SHIFT_DEMO_LED_4)

    #define INIT_WATCHDOG_DISABLE() _CONFIG_PORT_INPUT_FAST_LOW(C, SWITCH_1, PORT_PS_UP_ENABLE); // configure as input
    #define WATCHDOG_DISABLE()      (!_READ_PORT_MASK(C, SWITCH_1))      // pull this input down to disable watchdog (hold SW1 at reset)
    #define ACTIVATE_WATCHDOG()     UNLOCK_WDOG(); WDOG_TOVALL = (2000/5); WDOG_TOVALH = 0; WDOG_STCTRLH = (WDOG_STCTRLH_STNDBYEN | WDOG_STCTRLH_WAITEN | WDOG_STCTRLH_STOPEN | WDOG_STCTRLH_WDOGEN) // watchdog enabled to generate reset on 2s timeout (no further updates allowed)
    #if defined SUPPORT_SLCD
        #define TOGGLE_WATCHDOG_LED() _TOGGLE_PORT(C, BLINK_LED); if (IS_POWERED_UP(3, SIM_SCGC3_SLCD)) { TOGGLE_SLCD(15TO12, 0x8000000); } // blink freescale logo in the SLCD
    #else
        #define TOGGLE_WATCHDOG_LED() _TOGGLE_PORT(C, BLINK_LED);
    #endif

    #define CONFIG_TEST_OUTPUT()                                         // we use DEMO_LED_2 which is configured by the user code (and can be disabled in parameters if required)
    #define TOGGLE_TEST_OUTPUT()    _TOGGLE_PORT(C, DEMO_LED_2)
    #define SET_TEST_OUTPUT()       _SETBITS(C, DEMO_LED_2)
    #define CLEAR_TEST_OUTPUT()     _CLEARBITS(C, DEMO_LED_2)

    #define CONFIGURE_MOUSE_INPUTS()
    #define MOUSE_LEFT_CLICK()     0
    #define MOUSE_UP()             0
    #define MOUSE_DOWN()           0
    #define MOUSE_LEFT()           0
    #define MOUSE_RIGHT()          0

    #define USB_HOST_POWER_CONFIG() _CONFIG_DRIVE_PORT_OUTPUT_VALUE_FAST_LOW(B, (USB_HOST_POWER_ENABLE), (0), (PORT_SRE_SLOW | PORT_DSE_HIGH))
    #define USB_HOST_POWER_ON()     _SETBITS(B, USB_HOST_POWER_ENABLE)
    #define USB_HOST_POWER_OFF()    _CLEARBITS(B, USB_HOST_POWER_ENABLE)

    #if defined TWR_K53N512
        #define RESETOUT           PORTC_BIT15
        #define KEYPAD "KeyPads/TWR_K53N512.bmp"

            // '0'          '1'             input state   center (x,   y)   0 = circle, radius, controlling port, controlling pin 
        #define KEYPAD_LED_DEFINITIONS  \
            {RGB(255,128,0), RGB(70,70,70), 1, {318, 87,  327, 99  }, _PORTC, DEMO_LED_1}, \
            {RGB(255,255,0), RGB(70,70,70), 1, {318, 122, 327, 133 }, _PORTC, DEMO_LED_2},

        #define BUTTON_KEY_DEFINITIONS  {_PORTC,    SWITCH_1,  {308, 51, 323, 67 }}, \
                                        {_PORTC,    SWITCH_2,  {338, 51, 353, 67 }}
    #elif defined TWR_K40D100M
        #define KEYPAD "KeyPads/TWR_K40D100M.bmp"

            // '0'          '1'          input state   center (x,   y)   0 = circle, radius, controlling port, controlling pin 
        #define KEYPAD_LED_DEFINITIONS  \
            {RGB(255,128,0), RGB(70,70,70), 1, {213, 90,  218, 98  }, _PORTC, DEMO_LED_1}, \
            {RGB(255,255,0), RGB(70,70,70), 1, {213, 113, 218, 118 }, _PORTC, DEMO_LED_2}, \
            {RGB(0,255,0),   RGB(70,70,70), 1, {213, 135, 218, 140 }, _PORTC, DEMO_LED_3}, \
            {RGB(255,0,0),   RGB(70,70,70), 1, {213, 159, 218, 164 }, _PORTB, DEMO_LED_4}

        #define BUTTON_KEY_DEFINITIONS  {_PORTC,    SWITCH_1,  {205, 39, 214, 49 }}, \
                                        {_PORTC,    SWITCH_2,  {224, 39, 235, 49 }}
    #elif defined TWR_K40X256
        #define KEYPAD "KeyPads/TWR_K40X256.bmp"

            // '0'          '1'          input state   center (x,   y)   0 = circle, radius, controlling port, controlling pin 
        #define KEYPAD_LED_DEFINITIONS  \
            {RGB(255,128,0), RGB(70,70,70), 1, {358, 133, 365, 141 }, _PORTC, DEMO_LED_1}, \
            {RGB(255,255,0), RGB(70,70,70), 1, {358, 170, 365, 178 }, _PORTC, DEMO_LED_2}, \
            {RGB(0,255,0),   RGB(70,70,70), 1, {358, 207, 365, 215 }, _PORTC, DEMO_LED_3}, \
            {RGB(255,0,0),   RGB(70,70,70), 1, {358, 245, 365, 254 }, _PORTB, DEMO_LED_4}

        #define BUTTON_KEY_DEFINITIONS  {_PORTC,    SWITCH_1,  {336, 62, 352, 80 }}, \
                                        {_PORTC,    SWITCH_2,  {373, 62, 390, 80 }}
    #endif
#endif


#define CONFIG_TIMER_TEST_LEDS()   
#define TIMER_TEST_LED_ON()        
#define TIMER_TEST_LED2_ON()       
#define TIMER_TEST_LED_OFF()       
#define TIMER_TEST_LED2_OFF()  


#define PORT0_DEFAULT_INPUT        0xffffffff                            // initial input states for port simulator (port A)
#define PORT1_DEFAULT_INPUT        0xffffffff                            // port B
#define PORT2_DEFAULT_INPUT        0xffffffff                            // port C
#if defined BLAZE_K22 && defined SDCARD_SUPPORT
    #define PORT3_DEFAULT_INPUT    0xffffff7f                            // port D - PTD7 low to detect SD card by default
#elif defined FRDM_K66F && defined SDCARD_SUPPORT
    #define PORT3_DEFAULT_INPUT    0xfffffbff                            // port D - PTD10 low to detect SD card by default
#else
    #define PORT3_DEFAULT_INPUT    0xffffffff                            // port D
#endif
#define PORT4_DEFAULT_INPUT        0xffffffff                            // port E
#define PORT5_DEFAULT_INPUT        0xffffffff                            // port F


// User port mapping
//
#if defined NET_KBED	                                                 // {16}
    #define USER_OUT_1_BIT         PORTC_BIT1                            // 1...4 24V outputs if KBEDM4I4O connected     
    #define USER_OUT_2_BIT         PORTC_BIT2
    #define USER_OUT_3_BIT         PORTA_BIT8
    #define USER_OUT_4_BIT         PORTA_BIT9
    #define USER_OUT_5_BIT         PORTA_BIT10                           // relais KBEDM4I4O
    #define USER_OUT_6_BIT         PORTA_BIT11                           // red KBED4I4O LED
    #define USER_OUT_7_BIT         PORTC_BIT18                           // red KBEDM front LED
    #define USER_OUT_8_BIT         0
    #define USER_OUT_9_BIT         0
    #define USER_OUT_10_BIT        0
    #define USER_OUT_11_BIT        0
    #define USER_OUT_12_BIT        0
    #define USER_OUT_13_BIT        0
    #define USER_OUT_14_BIT        0
    #define USER_OUT_15_BIT        0
    #define USER_OUT_16_BIT        0

    #define USER_IN_1_BIT          PORTB_BIT4                            // 1...4 24V inputs if KBEDM4I4O connected   
    #define USER_IN_2_BIT          PORTB_BIT5
    #define USER_IN_3_BIT          PORTB_BIT6
    #define USER_IN_4_BIT          PORTB_BIT7

    #define USER_PORT_1_BIT        PORTA_BIT24                           // 1...5 TTL I/Os     
    #define USER_PORT_2_BIT        PORTA_BIT25
    #define USER_PORT_3_BIT        PORTA_BIT26
    #define USER_PORT_4_BIT        PORTA_BIT27
    #define USER_PORT_5_BIT        PORTA_BIT28
    #define USER_PORT_6_BIT        0
    #define USER_PORT_7_BIT        0
    #define USER_PORT_8_BIT        0
    #define USER_PORT_9_BIT        0
    #define USER_PORT_10_BIT       0
    #define USER_PORT_11_BIT       0
    #define USER_PORT_12_BIT       0
    #define USER_PORT_13_BIT       0
    #define USER_PORT_14_BIT       0
    #define USER_PORT_15_BIT       0
    #define USER_PORT_16_BIT       0

    #define _CONFIG_OUTPUT_PORT_1()    _CONFIG_PORT_OUTPUT(A, PORTA_BIT24, (PORT_SRE_SLOW))
    #define _CONFIG_OUTPUT_PORT_2()    _CONFIG_PORT_OUTPUT(A, PORTA_BIT25, (PORT_SRE_SLOW))
    #define _CONFIG_OUTPUT_PORT_3()    _CONFIG_PORT_OUTPUT(A, PORTA_BIT26, (PORT_SRE_SLOW))
    #define _CONFIG_OUTPUT_PORT_4()    _CONFIG_PORT_OUTPUT(A, PORTA_BIT27, (PORT_SRE_SLOW))
    #define _CONFIG_OUTPUT_PORT_5()    _CONFIG_PORT_OUTPUT(A, PORTA_BIT28, (PORT_SRE_SLOW))
    #define _CONFIG_OUTPUT_PORT_6()
    #define _CONFIG_OUTPUT_PORT_7()
    #define _CONFIG_OUTPUT_PORT_8()
    #define _CONFIG_OUTPUT_PORT_9()
    #define _CONFIG_OUTPUT_PORT_10()
    #define _CONFIG_OUTPUT_PORT_11()
    #define _CONFIG_OUTPUT_PORT_12()
    #define _CONFIG_OUTPUT_PORT_13()
    #define _CONFIG_OUTPUT_PORT_14()
    #define _CONFIG_OUTPUT_PORT_15()
    #define _CONFIG_OUTPUT_PORT_16()

    // Port use definitions
    //
    #define USER_SET_PORT_1        GPIOA_PSOR
    #define USER_CLEAR_PORT_1      GPIOA_PCOR
    #define USER_PORT_1            GPIOA_PDOR
    #define USER_TOGGLE_PORT_1     GPIOA_PTOR
    #define USER_DDR_1             GPIOA_PDDR

    #define USER_SET_PORT_2        GPIOA_PSOR
    #define USER_CLEAR_PORT_2      GPIOA_PCOR
    #define USER_PORT_2            GPIOA_PDOR
    #define USER_TOGGLE_PORT_2     GPIOA_PTOR
    #define USER_DDR_2             GPIOA_PDDR

    #define USER_SET_PORT_3        GPIOA_PSOR
    #define USER_CLEAR_PORT_3      GPIOA_PCOR
    #define USER_PORT_3            GPIOA_PDOR
    #define USER_TOGGLE_PORT_3     GPIOA_PTOR
    #define USER_DDR_3             GPIOA_PDDR

    #define USER_SET_PORT_4        GPIOA_PSOR
    #define USER_TOGGLE_PORT_4     GPIOA_PTOR
    #define USER_CLEAR_PORT_4      GPIOA_PCOR
    #define USER_PORT_4            GPIOA_PDOR
    #define USER_DDR_4             GPIOA_PDDR

    #define USER_SET_PORT_5        GPIOA_PSOR
    #define USER_TOGGLE_PORT_5     GPIOA_PTOR
    #define USER_CLEAR_PORT_5      GPIOA_PCOR
    #define USER_PORT_5            GPIOA_PDOR
    #define USER_DDR_5             GPIOA_PDDR

    #define USER_SET_PORT_6        GPIOA_PSOR
    #define USER_TOGGLE_PORT_6     GPIOA_PTOR
    #define USER_CLEAR_PORT_6      GPIOA_PCOR
    #define USER_PORT_6            GPIOA_PDOR
    #define USER_DDR_6             GPIOA_PDDR

    #define USER_SET_PORT_7        GPIOC_PSOR
    #define USER_TOGGLE_PORT_7     GPIOC_PTOR
    #define USER_CLEAR_PORT_7      GPIOC_PCOR
    #define USER_PORT_7            GPIOC_PDOR
    #define USER_DDR_7             GPIOC_PDDR
#elif defined NET_K60                                                    // {16}
    #define USER_OUT_1_BIT         PORTE_BIT6                            // 1...4 24V outputs if 3A4IO connected     
    #define USER_OUT_2_BIT         PORTE_BIT7
    #define USER_OUT_3_BIT         PORTE_BIT4
    #define USER_OUT_4_BIT         PORTE_BIT5
    #define USER_OUT_5_BIT         0
    #define USER_OUT_6_BIT         0
    #define USER_OUT_7_BIT         0
    #define USER_OUT_8_BIT         0
    #define USER_OUT_9_BIT         0
    #define USER_OUT_10_BIT        0
    #define USER_OUT_11_BIT        0
    #define USER_OUT_12_BIT        0
    #define USER_OUT_13_BIT        0
    #define USER_OUT_14_BIT        0
    #define USER_OUT_15_BIT        0
    #define USER_OUT_16_BIT        0

    #define _CONFIG_OUTPUT_PORT_1()    _CONFIG_PORT_OUTPUT(E, PORTE_BIT6,  (PORT_SRE_SLOW))
    #define _CONFIG_OUTPUT_PORT_2()    _CONFIG_PORT_OUTPUT(E, PORTE_BIT7,  (PORT_SRE_SLOW))
    #define _CONFIG_OUTPUT_PORT_3()    _CONFIG_PORT_OUTPUT(E, PORTE_BIT4,  (PORT_SRE_SLOW))
    #define _CONFIG_OUTPUT_PORT_4()    _CONFIG_PORT_OUTPUT(E, PORTE_BIT5,  (PORT_SRE_SLOW))
    #define _CONFIG_OUTPUT_PORT_5()
    #define _CONFIG_OUTPUT_PORT_6()
    #define _CONFIG_OUTPUT_PORT_7()
    #define _CONFIG_OUTPUT_PORT_8()
    #define _CONFIG_OUTPUT_PORT_9()
    #define _CONFIG_OUTPUT_PORT_10()
    #define _CONFIG_OUTPUT_PORT_11()
    #define _CONFIG_OUTPUT_PORT_12()
    #define _CONFIG_OUTPUT_PORT_13()
    #define _CONFIG_OUTPUT_PORT_14()
    #define _CONFIG_OUTPUT_PORT_15()
    #define _CONFIG_OUTPUT_PORT_16()


    #define USER_IN_1_BIT          PORTD_BIT14                           // 1...4 24V inputs if 3A4IO connected
    #define USER_IN_2_BIT          PORTD_BIT15
    #define USER_IN_3_BIT          PORTD_BIT12
    #define USER_IN_4_BIT          PORTD_BIT13

    #define USER_PORT_1_BIT        0
    #define USER_PORT_2_BIT        0
    #define USER_PORT_3_BIT        0
    #define USER_PORT_4_BIT        0
    #define USER_PORT_5_BIT        0
    #define USER_PORT_6_BIT        0
    #define USER_PORT_7_BIT        0
    #define USER_PORT_8_BIT        0
    #define USER_PORT_9_BIT        0
    #define USER_PORT_10_BIT       0
    #define USER_PORT_11_BIT       0
    #define USER_PORT_12_BIT       0
    #define USER_PORT_13_BIT       0
    #define USER_PORT_14_BIT       0
    #define USER_PORT_15_BIT       0
    #define USER_PORT_16_BIT       0

    // Port use definitions
    //
    #define USER_SET_PORT_1        GPIOE_PSOR
    #define USER_CLEAR_PORT_1      GPIOE_PCOR
    #define USER_PORT_1            GPIOE_PDOR
    #define USER_TOGGLE_PORT_1     GPIOE_PTOR
    #define USER_DDR_1             GPIOE_PDDR

    #define USER_SET_PORT_2        GPIOE_PSOR
    #define USER_CLEAR_PORT_2      GPIOE_PCOR
    #define USER_PORT_2            GPIOE_PDOR
    #define USER_TOGGLE_PORT_2     GPIOE_PTOR
    #define USER_DDR_2             GPIOE_PDDR

    #define USER_SET_PORT_3        GPIOE_PSOR
    #define USER_CLEAR_PORT_3      GPIOE_PCOR
    #define USER_PORT_3            GPIOE_PDOR
    #define USER_TOGGLE_PORT_3     GPIOE_PTOR
    #define USER_DDR_3             GPIOE_PDDR

    #define USER_SET_PORT_4        GPIOE_PSOR
    #define USER_CLEAR_PORT_4      GPIOE_PCOR
    #define USER_PORT_4            GPIOE_PDOR
    #define USER_TOGGLE_PORT_4     GPIOE_PTOR
    #define USER_DDR_4             GPIOE_PDDR

    #define USER_SET_PORT_5        GPIOA_PSOR
    #define USER_TOGGLE_PORT_5     GPIOA_PTOR
    #define USER_CLEAR_PORT_5      GPIOA_PCOR
    #define USER_PORT_5            GPIOA_PDOR
    #define USER_DDR_5             GPIOA_PDDR

    #define USER_SET_PORT_6        GPIOA_PSOR
    #define USER_TOGGLE_PORT_6     GPIOA_PTOR
    #define USER_CLEAR_PORT_6      GPIOA_PCOR
    #define USER_PORT_6            GPIOA_PDOR
    #define USER_DDR_6             GPIOA_PDDR

    #define USER_SET_PORT_7        GPIOC_PSOR
    #define USER_TOGGLE_PORT_7     GPIOC_PTOR
    #define USER_CLEAR_PORT_7      GPIOC_PCOR
    #define USER_PORT_7            GPIOC_PDOR
    #define USER_DDR_7             GPIOC_PDDR
#else
    #define USER_PORT_1_BIT        PORTD_BIT8                             // use free PA pins on Eval board
    #define USER_PORT_2_BIT        PORTD_BIT9
    #if defined TWR_K53N512 && defined TFT2N0369_GLCD_MODE
        #define USER_PORT_3_BIT    0
    #else
        #define USER_PORT_3_BIT    PORTD_BIT10
    #endif
    #if (defined TWR_K60D100M && defined SPI_FILE_SYSTEM) || (defined TWR_K53N512 && defined TFT2N0369_GLCD_MODE) // avoid reconfiguring SPI Flash lines and TFT signals
        #define USER_PORT_4_BIT    0
        #define USER_PORT_5_BIT    0
        #define USER_PORT_6_BIT    0
        #define USER_PORT_7_BIT    0
    #else
        #define USER_PORT_4_BIT    PORTD_BIT11
        #define USER_PORT_5_BIT    PORTD_BIT12
        #define USER_PORT_6_BIT    PORTD_BIT13
        #define USER_PORT_7_BIT    PORTD_BIT14
    #endif
    #if defined TWR_K64F120M
        #define USER_PORT_8_BIT    0                                     // avoid PHY interrupt line
    #else
        #define USER_PORT_8_BIT    PORTD_BIT15
    #endif
    #if defined FRDM_KL46Z
        #define USER_PORT_9_BIT    0
    #else
        #define USER_PORT_9_BIT    PORTC_BIT12
    #endif
    #if defined TWR_K40X256 || defined TWR_K40D100M
        #define USER_PORT_10_BIT   0
    #else
        #define USER_PORT_10_BIT   PORTC_BIT13
    #endif
    #define USER_PORT_11_BIT       PORTC_BIT14
    #if defined TWR_K53N512                                              // PTC15 is used to control the reset to the backplane of the tower system - we leave it as high impedant input to that it doesn't assert it
        #define USER_PORT_12_BIT   0
    #else
        #define USER_PORT_12_BIT   PORTC_BIT15
    #endif
    #define USER_PORT_13_BIT       PORTD_BIT3                            // {13}
    #define USER_PORT_14_BIT       PORTD_BIT4                            // {13}
    #define USER_PORT_15_BIT       PORTC_BIT18
    #define USER_PORT_16_BIT       PORTC_BIT19

    #if defined TWR_K60F120M                                             // keep I2C lines free for accelerometer use
        #define _CONFIG_OUTPUT_PORT_1()
        #define _CONFIG_OUTPUT_PORT_2()
    #else
        #define _CONFIG_OUTPUT_PORT_1()    _CONFIG_PORT_OUTPUT(D, PORTD_BIT8,  (PORT_SRE_SLOW))
        #define _CONFIG_OUTPUT_PORT_2()    _CONFIG_PORT_OUTPUT(D, PORTD_BIT9,  (PORT_SRE_SLOW))
    #endif
    #if defined TWR_K53N512 && defined TFT2N0369_GLCD_MODE
        #define _CONFIG_OUTPUT_PORT_3()
    #else
        #define _CONFIG_OUTPUT_PORT_3()    _CONFIG_PORT_OUTPUT(D, PORTD_BIT10, (PORT_SRE_SLOW))
    #endif
    #if (defined TWR_K60D100M && defined SPI_FILE_SYSTEM) || (defined TWR_K53N512 && defined TFT2N0369_GLCD_MODE) // avoid reconfiguring SPI Flash lines and TFT signals
        #define _CONFIG_OUTPUT_PORT_4()
        #define _CONFIG_OUTPUT_PORT_5()
        #define _CONFIG_OUTPUT_PORT_6()
        #define _CONFIG_OUTPUT_PORT_7()
    #else
        #define _CONFIG_OUTPUT_PORT_4()    _CONFIG_PORT_OUTPUT(D, PORTD_BIT11, (PORT_SRE_SLOW))
        #define _CONFIG_OUTPUT_PORT_5()    _CONFIG_PORT_OUTPUT(D, PORTD_BIT12, (PORT_SRE_SLOW))
        #define _CONFIG_OUTPUT_PORT_6()    _CONFIG_PORT_OUTPUT(D, PORTD_BIT13, (PORT_SRE_SLOW))
        #define _CONFIG_OUTPUT_PORT_7()    _CONFIG_PORT_OUTPUT(D, PORTD_BIT14, (PORT_SRE_SLOW))
    #endif
    #if defined TWR_K64F120M
        #define _CONFIG_OUTPUT_PORT_8()                                  // avoid PHY interrupt line
    #else
        #define _CONFIG_OUTPUT_PORT_8() _CONFIG_PORT_OUTPUT(D, PORTD_BIT15, (PORT_SRE_SLOW))
    #endif
    #if defined FRDM_KL46Z
        #define _CONFIG_OUTPUT_PORT_9()
    #else
        #define _CONFIG_OUTPUT_PORT_9()    _CONFIG_PORT_OUTPUT(C, PORTC_BIT12, (PORT_SRE_SLOW))
    #endif
    #if defined TWR_K40X256 || defined TWR_K40D100M
        #define _CONFIG_OUTPUT_PORT_10()
    #else
        #define _CONFIG_OUTPUT_PORT_10()   _CONFIG_PORT_OUTPUT(C, PORTC_BIT13, (PORT_SRE_SLOW))
    #endif
    #define _CONFIG_OUTPUT_PORT_11()   _CONFIG_PORT_OUTPUT(C, PORTC_BIT14, (PORT_SRE_SLOW))
    #if defined TWR_K53N512                                              // PTC15 is used to control the reset to the backplane of the tower system - we leave it as high impedant input to that it doesn't assert it
        #define _CONFIG_OUTPUT_PORT_12()
    #else
        #define _CONFIG_OUTPUT_PORT_12()   _CONFIG_PORT_OUTPUT(C, PORTC_BIT15, (PORT_SRE_SLOW))
    #endif
    #if defined FRDM_K20D50M || defined TWR_K20D50M || defined FRDM_KL25Z || defined FRDM_KL26Z || defined CAPUCCINO_KL27 || defined FRDM_KL27Z || defined TEENSY_3_1 || defined FRDM_K64F || defined FRDM_KL43Z || defined rcARM_KL26 || defined FreeLON // {21}
        #define _CONFIG_OUTPUT_PORT_13()
        #define _CONFIG_OUTPUT_PORT_14()
    #else
        #define _CONFIG_OUTPUT_PORT_13()   _CONFIG_PORT_OUTPUT(D, PORTD_BIT3,  (PORT_SRE_SLOW)) // {13}
        #define _CONFIG_OUTPUT_PORT_14()   _CONFIG_PORT_OUTPUT(D, PORTD_BIT4,  (PORT_SRE_SLOW)) // {13}
    #endif
    #define _CONFIG_OUTPUT_PORT_15()   _CONFIG_PORT_OUTPUT(C, PORTC_BIT18, (PORT_SRE_SLOW))
    #define _CONFIG_OUTPUT_PORT_16()   _CONFIG_PORT_OUTPUT(C, PORTC_BIT19, (PORT_SRE_SLOW))

    // Port use definitions
    //
    #define USER_SET_PORT_1        GPIOD_PSOR
    #define USER_CLEAR_PORT_1      GPIOD_PCOR
    #define USER_PORT_1            GPIOD_PDOR
    #define USER_TOGGLE_PORT_1     GPIOD_PTOR
    #define USER_DDR_1             GPIOD_PDDR

    #define USER_SET_PORT_2        GPIOD_PSOR
    #define USER_CLEAR_PORT_2      GPIOD_PCOR
    #define USER_PORT_2            GPIOD_PDOR
    #define USER_TOGGLE_PORT_2     GPIOD_PTOR
    #define USER_DDR_2             GPIOD_PDDR

    #define USER_SET_PORT_3        GPIOD_PSOR
    #define USER_CLEAR_PORT_3      GPIOD_PCOR
    #define USER_PORT_3            GPIOD_PDOR
    #define USER_TOGGLE_PORT_3     GPIOD_PTOR
    #define USER_DDR_3             GPIOD_PDDR

    #define USER_SET_PORT_4    GPIOD_PSOR
    #define USER_TOGGLE_PORT_4 GPIOD_PTOR
    #define USER_CLEAR_PORT_4  GPIOD_PCOR
    #define USER_PORT_4        GPIOD_PDOR
    #define USER_DDR_4         GPIOD_PDDR

    #define USER_SET_PORT_5    GPIOD_PSOR
    #define USER_TOGGLE_PORT_5 GPIOD_PTOR
    #define USER_CLEAR_PORT_5  GPIOD_PCOR
    #define USER_PORT_5        GPIOD_PDOR
    #define USER_DDR_5         GPIOD_PDDR

    #define USER_SET_PORT_6    GPIOD_PSOR
    #define USER_TOGGLE_PORT_6 GPIOD_PTOR
    #define USER_CLEAR_PORT_6  GPIOD_PCOR
    #define USER_PORT_6        GPIOD_PDOR
    #define USER_DDR_6         GPIOD_PDDR

    #define USER_SET_PORT_7    GPIOD_PSOR
    #define USER_TOGGLE_PORT_7 GPIOD_PTOR
    #define USER_CLEAR_PORT_7  GPIOD_PCOR
    #define USER_PORT_7        GPIOD_PDOR
    #define USER_DDR_7         GPIOD_PDDR
#endif

#define USER_SET_PORT_8        GPIOD_PSOR
#define USER_TOGGLE_PORT_8     GPIOD_PTOR
#define USER_CLEAR_PORT_8      GPIOD_PCOR
#define USER_PORT_8            GPIOD_PDOR
#define USER_DDR_8             GPIOD_PDDR

#define USER_SET_PORT_9        GPIOC_PSOR
#define USER_TOGGLE_PORT_9     GPIOC_PTOR
#define USER_CLEAR_PORT_9      GPIOC_PCOR
#define USER_PORT_9            GPIOC_PDOR
#define USER_DDR_9             GPIOC_PDDR

#define USER_SET_PORT_10       GPIOC_PSOR
#define USER_TOGGLE_PORT_10    GPIOC_PTOR
#define USER_CLEAR_PORT_10     GPIOC_PCOR
#define USER_PORT_10           GPIOC_PDOR
#define USER_DDR_10            GPIOC_PDDR

#define USER_SET_PORT_11       GPIOC_PSOR
#define USER_TOGGLE_PORT_11    GPIOC_PTOR
#define USER_CLEAR_PORT_11     GPIOC_PCOR
#define USER_PORT_11           GPIOC_PDOR
#define USER_DDR_11            GPIOC_PDDR

#define USER_SET_PORT_12       GPIOC_PSOR
#define USER_TOGGLE_PORT_12    GPIOC_PTOR
#define USER_CLEAR_PORT_12     GPIOC_PCOR
#define USER_PORT_12           GPIOC_PDOR
#define USER_DDR_12            GPIOC_PDDR

#define USER_SET_PORT_13       GPIOD_PSOR                                // {13}
#define USER_TOGGLE_PORT_13    GPIOD_PTOR
#define USER_CLEAR_PORT_13     GPIOD_PCOR
#define USER_PORT_13           GPIOD_PDOR
#define USER_DDR_13            GPIOD_PDDR

#define USER_SET_PORT_14       GPIOD_PSOR                                // {13}
#define USER_TOGGLE_PORT_14    GPIOD_PTOR
#define USER_CLEAR_PORT_14     GPIOD_PCOR
#define USER_PORT_14           GPIOD_PDOR
#define USER_DDR_14            GPIOD_PDDR

#define USER_SET_PORT_15       GPIOC_PSOR
#define USER_TOGGLE_PORT_15    GPIOC_PTOR
#define USER_CLEAR_PORT_15     GPIOC_PCOR
#define USER_PORT_15           GPIOC_PDOR
#define USER_DDR_15            GPIOC_PDDR

#define USER_SET_PORT_16       GPIOC_PSOR
#define USER_TOGGLE_PORT_16    GPIOC_PTOR
#define USER_CLEAR_PORT_16     GPIOC_PCOR
#define USER_PORT_16           GPIOC_PDOR
#define USER_DDR_16            GPIOC_PDDR

#if defined KL43Z_256_32_CL
    #define USB_DOWNLOAD_FILE          "0"
#else
    #define USB_DOWNLOAD_FILE          "z"                               // software upload via USB-CDC is saved to this internal file location
#endif

#if defined NAND_FLASH_FAT
    #define MT29F1G08ABADAH4                                             // 8 bit
  //#define MT29F2G16ABAEAWP                                             // 16-bit default
    #define GET_SDCARD_WP_STATE() 0                                      // write protection disabled (change to read switch is required)
    #define SET_SD_CS_HIGH()
    #define POWER_DOWN_SD_CARD()
#endif
#if defined SDCARD_SUPPORT
    #define SDCARD_SIM_SIZE   SDCARD_SIZE_4G                             // the size of SD card when simulating
  //#define _NO_SD_CARD_INSERTED                                         // simulate no SD card inserted initially

    #if !defined SDCARD_CONFIG_COMPLETE
        #if defined tinyK20 || defined FRDM_K22F || defined BLAZE_K22 || defined FreeLON
            #define SDCARD_DETECT_INPUT_INTERRUPT                        // use card detect interrupt rather than polling the card
        #else
            #define T_CHECK_CARD_REMOVAL    ((DELAY_LIMIT)(SEC * 10))    // if the card has no detection switch it can be polled to detect removal
          //#define SDCARD_DETECT_INPUT_POLL                             // {8} use card detect switch for detection polling (use together with T_CHECK_CARD_REMOVAL)
          //#define SDCARD_DETECT_INPUT_INTERRUPT                        // {8} use card detect switch for detection by interrupt (T_CHECK_CARD_REMOVAL and SDCARD_DETECT_INPUT_POLL should be disabled)
       #endif
    #endif
    #if defined SDCARD_DETECT_INPUT_INTERRUPT && !defined SUPPORT_PORT_INTERRUPTS
        #define SUPPORT_PORT_INTERRUPTS                                  // ensure port interrupt support is enabled
    #endif

    #if defined NET_KBED	                                             // {16}
        #if defined SD_CONTROLLER_AVAILABLE	                             // only if the SD-card is connected to SDHC-Pins
            #define SET_SD_CS_HIGH()
            #define SET_SD_CS_LOW()
            #define WRITE_PROTECT_INPUT   PORTE_BIT27	                 // PE27 is not connectet on KBED !
            #if defined _WINDOWS
                #define POWER_UP_SD_CARD()  _CONFIG_PORT_INPUT(E, (WRITE_PROTECT_INPUT), (PORT_PS_UP_ENABLE)); SDHC_SYSCTL |= SDHC_SYSCTL_INITA; SDHC_SYSCTL &= ~SDHC_SYSCTL_INITA; // apply power to the SD card if appropriate (we use this to send 80 clocks - self-clearing bit)
            #else
                #define POWER_UP_SD_CARD()  _CONFIG_PORT_INPUT(E, (WRITE_PROTECT_INPUT), (PORT_PS_UP_ENABLE)); SDHC_SYSCTL |= SDHC_SYSCTL_INITA; while (SDHC_SYSCTL & SDHC_SYSCTL_INITA) {}; // apply power to the SD card if appropriate (we use this to send 80 clocks)
            #endif
            #define POWER_DOWN_SD_CARD()                                 // remove power from SD card interface

            #define SDHC_SYSCTL_SPEED_SLOW  (SDHC_SYSCTL_SDCLKFS_128 | SDHC_SYSCTL_DVS_2) // 390kHz when 100MHz clock
            #define SDHC_SYSCTL_SPEED_FAST  (SDHC_SYSCTL_SDCLKFS_2 | SDHC_SYSCTL_DVS_2) // 25MHz when 100MHz clock
            #define SET_SPI_SD_INTERFACE_FULL_SPEED() fnSetSD_clock(SDHC_SYSCTL_SPEED_FAST); SDHC_PROCTL |= SDHC_PROCTL_DTW_4BIT
                 #define GET_SDCARD_WP_STATE() (!_READ_PORT_MASK(E, WRITE_PROTECT_INPUT)) // when the input is read as '0' the card is protected from writes
        #elif defined TESTBED_BOARD
            // Configure to suit SD card SPI mode on SPI1
            //
            #define SPI1_CS0       PORTB_BIT10
            #define INITIALISE_SPI_SD_INTERFACE() POWER_UP(6, SIM_SCGC6_SPI1); \
            _CONFIG_PERIPHERAL(B, 11, PB_11_SPI1_SCK); _CONFIG_PERIPHERAL(B, 16, (PB_16_SPI1_SOUT | PORT_SRE_FAST | PORT_DSE_HIGH)); _CONFIG_PERIPHERAL(B, 17, (PB_17_SPI1_SIN | PORT_PS_UP_ENABLE)); \
            _CONFIG_DRIVE_PORT_OUTPUT_VALUE(B, SPI1_CS0, SPI1_CS0, (PORT_SRE_FAST | PORT_DSE_HIGH)); \
            SPI1_CTAR0 = (SPI_CTAR_ASC_6 | SPI_CTAR_FMSZ_8 | SPI_CTAR_CPHA | SPI_CTAR_CPOL | SPI_CTAR_BR_128); SPI1_MCR = (SPI_MCR_DIS_TXF | SPI_MCR_DIS_RXF | SPI_MCR_MSTR | SPI_MCR_DCONF_SPI | SPI_MCR_CLR_RXF | SPI_MCR_CLR_TXF | SPI_MCR_PCSIS_CS0 | SPI_MCR_PCSIS_CS1 | SPI_MCR_PCSIS_CS2 | SPI_MCR_PCSIS_CS3 | SPI_MCR_PCSIS_CS4 | SPI_MCR_PCSIS_CS5)

            #define ENABLE_SPI_SD_OPERATION()
            #define SET_SD_CARD_MODE()

            // Set maximum speed
            //
            #define SET_SPI_SD_INTERFACE_FULL_SPEED() SPI1_MCR |= SPI_MCR_HALT; SPI1_CTAR0 = (SPI_CTAR_FMSZ_8 | SPI_CTAR_CPOL | SPI_CTAR_CPHA | SPI_CTAR_BR_2); SPI1_MCR &= ~SPI_MCR_HALT;
            #if defined _WINDOWS
                #define WRITE_SPI_CMD(byte)    SPI1_SR &= ~(SPI_SR_RFDF); SPI1_PUSHR = (byte | SPI_PUSHR_PCS_NONE | SPI_PUSHR_CTAS_CTAR0); SPI1_POPR = _fnSimSD_write((unsigned char)byte)
                #define WAIT_TRANSMISSON_END() while (!(SPI1_SR & (SPI_SR_RFDF))) { SPI1_SR |= (SPI_SR_RFDF); }
                #define READ_SPI_DATA()        (unsigned char)SPI1_POPR
            #else
                #define WRITE_SPI_CMD(byte)    SPI1_SR = (SPI_SR_RFDF); SPI1_PUSHR = (byte | SPI_PUSHR_PCS_NONE | SPI_PUSHR_CTAS_CTAR0) // clear flags before transmitting (and receiving) a single byte
                #define WAIT_TRANSMISSON_END() while (!(SPI1_SR & (SPI_SR_RFDF))) {}
                #define READ_SPI_DATA()        (unsigned char)SPI1_POPR
            #endif
            #define SET_SD_DI_CS_HIGH()        _SETBITS(B, SPI1_CS0)     // force DI and CS lines high ready for the initialisation sequence
            #define SET_SD_CS_LOW()            _CLEARBITS(B, SPI1_CS0)   // assert the CS line of the SD card to be read
            #define SET_SD_CS_HIGH()           _SETBITS(B, SPI1_CS0)     // negate the CS line of the SD card to be read
             #define GET_SDCARD_WP_STATE()     (0)                       // there is no write protect at microSD
            #define POWER_UP_SD_CARD()                                   // apply power to the SD card if appropriate
            #define POWER_DOWN_SD_CARD()                                 // remove power from SD card interface
        #elif defined KBEDM_BOARD 
            // Configure to suit SD card SPI mode on SPI2
            //
            #if defined KBED_FPGA	                                    // FPGA version has other CD pin access
                #define INITIALISE_SPI_SD_INTERFACE() POWER_UP(3, SIM_SCGC3_SPI2); \
            	_CONFIG_PERIPHERAL(D, 12, PD_12_SPI2_SCK); _CONFIG_PERIPHERAL(D, 13, (PD_13_SPI2_SOUT | PORT_SRE_FAST | PORT_DSE_HIGH)); _CONFIG_PERIPHERAL(D, 14, (PD_14_SPI2_SIN | PORT_PS_UP_ENABLE)); \
            	_CONFIG_DRIVE_PORT_OUTPUT_VALUE(D, SPI2_CS1, SPI2_CS1, (PORT_SRE_FAST | PORT_DSE_HIGH)); \
            	SPI2_CTAR0 = (SPI_CTAR_ASC_6 | SPI_CTAR_FMSZ_8 | SPI_CTAR_CPHA | SPI_CTAR_CPOL | SPI_CTAR_BR_128); SPI2_MCR = (SPI_MCR_DIS_TXF | SPI_MCR_DIS_RXF | SPI_MCR_MSTR | SPI_MCR_DCONF_SPI | SPI_MCR_CLR_RXF | SPI_MCR_CLR_TXF | SPI_MCR_PCSIS_CS0 | SPI_MCR_PCSIS_CS1 | SPI_MCR_PCSIS_CS2 | SPI_MCR_PCSIS_CS3 | SPI_MCR_PCSIS_CS4 | SPI_MCR_PCSIS_CS5)
            #else
                #define SD_CARD_DETECTION      PORTB_BIT18 
                #define SDCARD_DETECTION()     (_READ_PORT_MASK(B, SD_CARD_DETECTION) == 0) // card detection input
                #define PRIORITY_SDCARD_DETECT_PORT_INT   PRIORITY_PORT_B_INT // port priority when using card detect switch interrupt
                #define SDCARD_DETECT_PORT     PORTB                     // interrupt is on this port
                #define SDCARD_DETECT_PIN      SD_CARD_DETECTION         // interrupt pin

                #define INITIALISE_SPI_SD_INTERFACE() POWER_UP(3, SIM_SCGC3_SPI2); \
            	_CONFIG_PORT_INPUT(B, (SD_CARD_DETECTION), (PORT_PS_UP_ENABLE)); \
            	_CONFIG_PERIPHERAL(D, 12, PD_12_SPI2_SCK); _CONFIG_PERIPHERAL(D, 13, (PD_13_SPI2_SOUT | PORT_SRE_FAST | PORT_DSE_HIGH)); _CONFIG_PERIPHERAL(D, 14, (PD_14_SPI2_SIN | PORT_PS_UP_ENABLE)); \
            	_CONFIG_DRIVE_PORT_OUTPUT_VALUE(D, SPI2_CS1, SPI2_CS1, (PORT_SRE_FAST | PORT_DSE_HIGH)); \
            	SPI2_CTAR0 = (SPI_CTAR_ASC_6 | SPI_CTAR_FMSZ_8 | SPI_CTAR_CPHA | SPI_CTAR_CPOL | SPI_CTAR_BR_128); SPI2_MCR = (SPI_MCR_DIS_TXF | SPI_MCR_DIS_RXF | SPI_MCR_MSTR | SPI_MCR_DCONF_SPI | SPI_MCR_CLR_RXF | SPI_MCR_CLR_TXF | SPI_MCR_PCSIS_CS0 | SPI_MCR_PCSIS_CS1 | SPI_MCR_PCSIS_CS2 | SPI_MCR_PCSIS_CS3 | SPI_MCR_PCSIS_CS4 | SPI_MCR_PCSIS_CS5)
            #endif  
            #define SPI2_CS1                  PORTD_BIT15
            #define ENABLE_SPI_SD_OPERATION()
            #define SET_SD_CARD_MODE()

            // Set maximum speed
            //
            #define SET_SPI_SD_INTERFACE_FULL_SPEED() SPI2_MCR |= SPI_MCR_HALT; SPI2_CTAR0 = (SPI_CTAR_FMSZ_8 | SPI_CTAR_CPOL | SPI_CTAR_CPHA | SPI_CTAR_BR_2); SPI2_MCR &= ~SPI_MCR_HALT;
            #if defined _WINDOWS
                #define WRITE_SPI_CMD(byte)   SPI2_SR &= ~(SPI_SR_RFDF); SPI2_PUSHR = (byte | SPI_PUSHR_PCS_NONE | SPI_PUSHR_CTAS_CTAR0); SPI2_POPR = _fnSimSD_write((unsigned char)byte)
                #define WAIT_TRANSMISSON_END() while (!(SPI2_SR & (SPI_SR_RFDF))) { SPI2_SR |= (SPI_SR_RFDF); }
                #define READ_SPI_DATA()       (unsigned char)SPI2_POPR
            #else
                #define WRITE_SPI_CMD(byte)   SPI2_SR = (SPI_SR_RFDF); SPI2_PUSHR = (byte | SPI_PUSHR_PCS_NONE | SPI_PUSHR_CTAS_CTAR0) // clear flags before transmitting (and receiving) a single byte
                #define WAIT_TRANSMISSON_END() while (!(SPI2_SR & (SPI_SR_RFDF))) {}
                #define READ_SPI_DATA()       (unsigned char)SPI2_POPR
            #endif
            #define SET_SD_DI_CS_HIGH()       _SETBITS(D, SPI2_CS1)      // force DI and CS lines high ready for the initialisation sequence
            #define SET_SD_CS_LOW()           _CLEARBITS(D, SPI2_CS1)    // assert the CS line of the SD card to be read
            #define SET_SD_CS_HIGH()          _SETBITS(D, SPI2_CS1)      // negate the CS line of the SD card to be read
            #define GET_SDCARD_WP_STATE()     (0)                        // there is no write protect at microSD
            #define POWER_UP_SD_CARD()                                   // apply power to the SD card if appropriate
            #define POWER_DOWN_SD_CARD()                                 // remove power from SD card interface
        #endif
    #elif defined NET_K60                                                // {16}
        // Configure to suit SD card SPI mode on SPI2 
        //
        #define SPI2_CS0          PORTB_BIT20
        #define INITIALISE_SPI_SD_INTERFACE() POWER_UP(3, SIM_SCGC3_SPI2); \
        _CONFIG_PERIPHERAL(B, 21, PB_21_SPI2_SCK); _CONFIG_PERIPHERAL(B, 22, (PB_22_SPI2_SOUT | PORT_SRE_FAST | PORT_DSE_HIGH)); _CONFIG_PERIPHERAL(B, 23, (PB_23_SPI2_SIN | PORT_PS_UP_ENABLE)); \
        _CONFIG_DRIVE_PORT_OUTPUT_VALUE(B, SPI2_CS0, SPI2_CS0, (PORT_SRE_FAST | PORT_DSE_HIGH)); \
        SPI2_CTAR0 = (SPI_CTAR_ASC_6 | SPI_CTAR_FMSZ_8 | SPI_CTAR_CPHA | SPI_CTAR_CPOL | SPI_CTAR_BR_128); SPI2_MCR = (SPI_MCR_DIS_TXF | SPI_MCR_DIS_RXF | SPI_MCR_MSTR | SPI_MCR_DCONF_SPI | SPI_MCR_CLR_RXF | SPI_MCR_CLR_TXF | SPI_MCR_PCSIS_CS0 | SPI_MCR_PCSIS_CS1 | SPI_MCR_PCSIS_CS2 | SPI_MCR_PCSIS_CS3 | SPI_MCR_PCSIS_CS4 | SPI_MCR_PCSIS_CS5)

        #define ENABLE_SPI_SD_OPERATION()
        #define SET_SD_CARD_MODE()

        // Set maximum speed
        //
        #define SET_SPI_SD_INTERFACE_FULL_SPEED() SPI2_MCR |= SPI_MCR_HALT; SPI2_CTAR0 = (SPI_CTAR_FMSZ_8 | SPI_CTAR_CPOL | SPI_CTAR_CPHA | SPI_CTAR_BR_2); SPI2_MCR &= ~SPI_MCR_HALT;
        #if defined _WINDOWS
            #define WRITE_SPI_CMD(byte)       SPI2_SR &= ~(SPI_SR_RFDF); SPI2_PUSHR = (byte | SPI_PUSHR_PCS_NONE | SPI_PUSHR_CTAS_CTAR0); SPI2_POPR = _fnSimSD_write((unsigned char)byte)
            #define WAIT_TRANSMISSON_END()    while (!(SPI2_SR & (SPI_SR_RFDF))) { SPI2_SR |= (SPI_SR_RFDF); }
            #define READ_SPI_DATA()           (unsigned char)SPI2_POPR
        #else
            #define WRITE_SPI_CMD(byte)       SPI2_SR = (SPI_SR_RFDF); SPI2_PUSHR = (byte | SPI_PUSHR_PCS_NONE | SPI_PUSHR_CTAS_CTAR0) // clear flags before transmitting (and receiving) a single byte
            #define WAIT_TRANSMISSON_END()    while (!(SPI2_SR & (SPI_SR_RFDF))) {}
            #define READ_SPI_DATA()           (unsigned char)SPI2_POPR
        #endif
        #define SET_SD_DI_CS_HIGH()           _SETBITS(B, SPI2_CS0)      // force DI and CS lines high ready for the initialisation sequence
        #define SET_SD_CS_LOW()               _CLEARBITS(B, SPI2_CS0)    // assert the CS line of the SD card to be read
        #define SET_SD_CS_HIGH()              _SETBITS(B, SPI2_CS0)      // negate the CS line of the SD card to be read
        #define POWER_UP_SD_CARD()                                       // apply power to the SD card if appropriate
        #define GET_SDCARD_WP_STATE()         (0)                        // there is no write protect at microSD
  
        #define POWER_DOWN_SD_CARD()                                     // remove power from SD card interface
    #elif defined TWR_K60N512 || defined TWR_K60D100M || defined TWR_K60F120M || defined TWR_K70F120M || defined TWR_K80F150M
      //#define SD_CONTROLLER_AVAILABLE                                  // use SDHC controller rather than SPI
        #if defined TWR_K60N512 || defined TWR_K60D100M                  // TWR_K60F120M/TWR_K70F120M have no write protect input on the SD card socket
            #define WRITE_PROTECT_INPUT     PORTE_BIT27
        #else
            #define WRITE_PROTECT_INPUT     0
        #endif
        #define SD_CARD_DETECTION           PORTE_BIT28                  // {8}
        #if defined SD_CONTROLLER_AVAILABLE                              // use SDHC controller
            #define SET_SD_CS_HIGH()
            #define SET_SD_CS_LOW()
            #if defined TWR_K60N512 || defined TWR_K60D100M
                #if defined _WINDOWS
                    #define POWER_UP_SD_CARD()    _CONFIG_PORT_INPUT(E, (WRITE_PROTECT_INPUT | SD_CARD_DETECTION), (PORT_PS_UP_ENABLE)); SDHC_SYSCTL |= SDHC_SYSCTL_INITA; SDHC_SYSCTL &= ~SDHC_SYSCTL_INITA; // apply power to the SD card if appropriate (we use this to send 80 clocks - self-clearing bit)
                #else
                    #define POWER_UP_SD_CARD()    _CONFIG_PORT_INPUT(E, (WRITE_PROTECT_INPUT | SD_CARD_DETECTION), (PORT_PS_UP_ENABLE)); SDHC_SYSCTL |= SDHC_SYSCTL_INITA; while (SDHC_SYSCTL & SDHC_SYSCTL_INITA) {}; // apply power to the SD card if appropriate (we use this to send 80 clocks)
                #endif
            #else
                #if defined _WINDOWS
                    #define POWER_UP_SD_CARD()    _CONFIG_PORT_INPUT(E, (SD_CARD_DETECTION), (PORT_PS_UP_ENABLE)); SDHC_SYSCTL |= SDHC_SYSCTL_INITA; SDHC_SYSCTL &= ~SDHC_SYSCTL_INITA; // apply power to the SD card if appropriate (we use this to send 80 clocks - self-clearing bit)
                #else
                    #define POWER_UP_SD_CARD()    _CONFIG_PORT_INPUT(E, (SD_CARD_DETECTION), (PORT_PS_UP_ENABLE)); SDHC_SYSCTL |= SDHC_SYSCTL_INITA; while (SDHC_SYSCTL & SDHC_SYSCTL_INITA) {}; // apply power to the SD card if appropriate (we use this to send 80 clocks)
                #endif
            #endif
            #if defined TWR_K60F120M || defined TWR_K70F120M || defined TWR_K80F150M
                #define SDHC_SYSCTL_SPEED_SLOW        (SDHC_SYSCTL_SDCLKFS_64 | SDHC_SYSCTL_DVS_5) // 375kHz when 120MHz clock
                #define SDHC_SYSCTL_SPEED_FAST        (SDHC_SYSCTL_SDCLKFS_2 | SDHC_SYSCTL_DVS_3) // 20MHz when 120MHz clock
            #else
                #define SDHC_SYSCTL_SPEED_SLOW        (SDHC_SYSCTL_SDCLKFS_128 | SDHC_SYSCTL_DVS_2) // 390kHz when 100MHz clock
                #define SDHC_SYSCTL_SPEED_FAST        (SDHC_SYSCTL_SDCLKFS_2 | SDHC_SYSCTL_DVS_2) // 25MHz when 100MHz clock
            #endif
            #define SET_SPI_SD_INTERFACE_FULL_SPEED() fnSetSD_clock(SDHC_SYSCTL_SPEED_FAST); SDHC_PROCTL |= SDHC_PROCTL_DTW_4BIT
        #else
            // Configure to suit SD card SPI mode at between 100k and 400k
            //
            #define SPI_CS1_0                  PORTE_BIT4
            #if defined TWR_K60F120M || defined TWR_K70F120M || defined TWR_K80F150M // swap SIN and SOUT pins for compatibility with SDHC connection
                #define INITIALISE_SPI_SD_INTERFACE() POWER_UP(6, SIM_SCGC6_SPI1); \
                _CONFIG_PORT_INPUT(E, (WRITE_PROTECT_INPUT | SD_CARD_DETECTION), (PORT_PS_UP_ENABLE)); \
                _CONFIG_PORT_INPUT(E, (PORTE_BIT0), (PORT_NO_PULL)); \
                _CONFIG_PERIPHERAL(E, 2, PE_2_SPI1_SCK); \
                _CONFIG_PERIPHERAL(E, 3, (PE_3_SPI1_SOUT | PORT_SRE_FAST | PORT_DSE_HIGH)); _CONFIG_PERIPHERAL(E, 1, (PE_1_SPI1_SIN | PORT_PS_UP_ENABLE)); \
                _CONFIG_DRIVE_PORT_OUTPUT_VALUE(E, SPI_CS1_0, SPI_CS1_0, (PORT_SRE_FAST | PORT_DSE_HIGH)); \
                SPI1_CTAR0 = (SPI_CTAR_ASC_6 | SPI_CTAR_FMSZ_8 | SPI_CTAR_CPHA | SPI_CTAR_CPOL | SPI_CTAR_BR_128); SPI1_MCR = (SPI_MCR_DIS_TXF | SPI_MCR_DIS_RXF | SPI_MCR_MSTR | SPI_MCR_DCONF_SPI | SPI_MCR_CLR_RXF | SPI_MCR_CLR_TXF | SPI_MCR_PCSIS_CS0 | SPI_MCR_PCSIS_CS1 | SPI_MCR_PCSIS_CS2 | SPI_MCR_PCSIS_CS3 | SPI_MCR_PCSIS_CS4 | SPI_MCR_PCSIS_CS5)
            #else
                #define INITIALISE_SPI_SD_INTERFACE() POWER_UP(6, SIM_SCGC6_SPI1); \
                _CONFIG_PORT_INPUT(E, (WRITE_PROTECT_INPUT | SD_CARD_DETECTION), (PORT_PS_UP_ENABLE)); \
                _CONFIG_PORT_INPUT(E, (PORTE_BIT0), (PORT_NO_PULL)); \
                _CONFIG_PERIPHERAL(E, 2, PE_2_SPI1_SCK); \
                _CONFIG_PERIPHERAL(E, 1, (PE_1_SPI1_SOUT | PORT_SRE_FAST | PORT_DSE_HIGH)); _CONFIG_PERIPHERAL(E, 3, (PE_3_SPI1_SIN | PORT_PS_UP_ENABLE)); \
                _CONFIG_DRIVE_PORT_OUTPUT_VALUE(E, SPI_CS1_0, SPI_CS1_0, (PORT_SRE_FAST | PORT_DSE_HIGH)); \
                SPI1_CTAR0 = (SPI_CTAR_ASC_6 | SPI_CTAR_FMSZ_8 | SPI_CTAR_CPHA | SPI_CTAR_CPOL | SPI_CTAR_BR_128); SPI1_MCR = (SPI_MCR_DIS_TXF | SPI_MCR_DIS_RXF | SPI_MCR_MSTR | SPI_MCR_DCONF_SPI | SPI_MCR_CLR_RXF | SPI_MCR_CLR_TXF | SPI_MCR_PCSIS_CS0 | SPI_MCR_PCSIS_CS1 | SPI_MCR_PCSIS_CS2 | SPI_MCR_PCSIS_CS3 | SPI_MCR_PCSIS_CS4 | SPI_MCR_PCSIS_CS5)
            #endif

            #define ENABLE_SPI_SD_OPERATION()
            #define SET_SD_CARD_MODE()

            // Set maximum speed
            //
            #define SET_SPI_SD_INTERFACE_FULL_SPEED() SPI1_MCR |= SPI_MCR_HALT; SPI1_CTAR0 = (SPI_CTAR_FMSZ_8 | SPI_CTAR_CPOL | SPI_CTAR_CPHA | SPI_CTAR_BR_2); SPI1_MCR &= ~SPI_MCR_HALT;
            #if defined _WINDOWS
                #define WRITE_SPI_CMD(byte)     SPI1_SR &= ~(SPI_SR_RFDF); SPI1_PUSHR = (byte | SPI_PUSHR_PCS_NONE | SPI_PUSHR_CTAS_CTAR0); SPI1_POPR = _fnSimSD_write((unsigned char)byte)
                #define WAIT_TRANSMISSON_END() while (!(SPI1_SR & (SPI_SR_RFDF))) { SPI1_SR |= (SPI_SR_RFDF); }
                #define READ_SPI_DATA()        (unsigned char)SPI1_POPR
            #else
                #define WRITE_SPI_CMD(byte)    SPI1_SR = (SPI_SR_RFDF); SPI1_PUSHR = (byte | SPI_PUSHR_PCS_NONE | SPI_PUSHR_CTAS_CTAR0) // clear flags before transmitting (and receiving) a single byte
                #define WAIT_TRANSMISSON_END() while (!(SPI1_SR & (SPI_SR_RFDF))) {}
                #define READ_SPI_DATA()        (unsigned char)SPI1_POPR
            #endif
            #define SET_SD_DI_CS_HIGH()  _SETBITS(E, SPI_CS1_0)          // force DI and CS lines high ready for the initialisation sequence
            #define SET_SD_CS_LOW()      _CLEARBITS(E, SPI_CS1_0)        // assert the CS line of the SD card to be read
            #define SET_SD_CS_HIGH()     _SETBITS(E, SPI_CS1_0)          // negate the CS line of the SD card to be read
            #define POWER_UP_SD_CARD()                                   // apply power to the SD card if appropriate
        #endif

        #define POWER_DOWN_SD_CARD()                                     // remove power from SD card interface
        #if defined TWR_K60F120M || defined TWR_K70F120M || defined TWR_K80F150M
            #define GET_SDCARD_WP_STATE() 0                              // TWR_K60F120M/TWR_K70F120M have no write protect input on the SD card socket
        #else
            #define GET_SDCARD_WP_STATE() (_READ_PORT_MASK(E, WRITE_PROTECT_INPUT)) // when the input is read as '1' the card is protected from writes
        #endif
        #define SDCARD_DETECTION()     (_READ_PORT_MASK(E, SD_CARD_DETECTION) == 0) // card detection input
        #define PRIORITY_SDCARD_DETECT_PORT_INT   PRIORITY_PORT_E_INT    // port priority when using card detect switch interrupt
        #define SDCARD_DETECT_PORT      PORTE                            // interrupt is on this port
        #define SDCARD_DETECT_PIN       SD_CARD_DETECTION                // interrupt pin
    #elif defined TWR_K21F120M
        #define SD_CONTROLLER_AVAILABLE
        #define SET_SD_CS_HIGH()                                         // dummy with SDHC controller
        #define SET_SD_CS_LOW()                                          // dummy with SDHC controller

        #if defined _WINDOWS
            #define POWER_UP_SD_CARD()    SDHC_SYSCTL |= SDHC_SYSCTL_INITA; SDHC_SYSCTL &= ~SDHC_SYSCTL_INITA; _CONFIG_PORT_INPUT(C, (WRITE_PROTECT_INPUT | SD_CARD_DETECTION), (PORT_PS_UP_ENABLE)); // apply power to the SD card if appropriate (we use this to send 80 clocks - self-clearing bit)
        #else
            #define POWER_UP_SD_CARD()    SDHC_SYSCTL |= SDHC_SYSCTL_INITA; while (SDHC_SYSCTL & SDHC_SYSCTL_INITA) {}; _CONFIG_PORT_INPUT(C, (WRITE_PROTECT_INPUT | SD_CARD_DETECTION), (PORT_PS_UP_ENABLE)); // apply power to the SD card if appropriate (we use this to send 80 clocks)
        #endif

        #define SDHC_SYSCTL_SPEED_SLOW    (SDHC_SYSCTL_SDCLKFS_64 | SDHC_SYSCTL_DVS_5) // 375kHz when 120MHz clock
        #define SDHC_SYSCTL_SPEED_FAST    (SDHC_SYSCTL_SDCLKFS_2 | SDHC_SYSCTL_DVS_3) // 20MHz when 120MHz clock
        #define SET_SPI_SD_INTERFACE_FULL_SPEED() fnSetSD_clock(SDHC_SYSCTL_SPEED_FAST); SDHC_PROCTL |= SDHC_PROCTL_DTW_4BIT

        #define POWER_DOWN_SD_CARD()                                     // remove power from SD card interface

        #define GET_SDCARD_WP_STATE()     (_READ_PORT_MASK(C, WRITE_PROTECT_INPUT) == 0) // card detection input
    #elif defined TWR_K53N512
        #define SD_CONTROLLER_AVAILABLE
        #define WRITE_PROTECT_INPUT     PORTC_BIT9
        #if defined SD_CONTROLLER_AVAILABLE
            #define SET_SD_CS_HIGH()
            #define SET_SD_CS_LOW()
            #if defined _WINDOWS
                #define POWER_UP_SD_CARD()  _CONFIG_PORT_INPUT(C, (WRITE_PROTECT_INPUT), (PORT_PS_UP_ENABLE)); SDHC_SYSCTL |= SDHC_SYSCTL_INITA; SDHC_SYSCTL &= ~SDHC_SYSCTL_INITA; // apply power to the SD card if appropriate (we use this to send 80 clocks - self-clearing bit)
            #else
                #define POWER_UP_SD_CARD()  _CONFIG_PORT_INPUT(C, (WRITE_PROTECT_INPUT), (PORT_PS_UP_ENABLE)); SDHC_SYSCTL |= SDHC_SYSCTL_INITA; while (SDHC_SYSCTL & SDHC_SYSCTL_INITA) {}; // apply power to the SD card if appropriate (we use this to send 80 clocks)
            #endif
            #define SDHC_SYSCTL_SPEED_SLOW        (SDHC_SYSCTL_SDCLKFS_128 | SDHC_SYSCTL_DVS_2) // 390kHz when 100MHz clock
            #define SDHC_SYSCTL_SPEED_FAST        (SDHC_SYSCTL_SDCLKFS_2 | SDHC_SYSCTL_DVS_2)   // 25MHz when 100MHz clock
            #define SET_SPI_SD_INTERFACE_FULL_SPEED() fnSetSD_clock(SDHC_SYSCTL_SPEED_FAST); SDHC_PROCTL |= SDHC_PROCTL_DTW_4BIT

            #define GET_SDCARD_WP_STATE()         (_READ_PORT_MASK(C, WRITE_PROTECT_INPUT) != 0) // when the input is read as '1' the card is protected from writes
        #else
            // Configure to suit SD card SPI mode at between 100k and 400k
            //
            #define SPI_CS1_0                  PORTE_BIT4
            #define INITIALISE_SPI_SD_INTERFACE() POWER_UP(6, SIM_SCGC6_SPI1); \
            _CONFIG_PORT_INPUT(E, (WRITE_PROTECT_INPUT), (PORT_PS_UP_ENABLE)); \
            _CONFIG_PORT_INPUT(E, (PORTE_BIT0), (PORT_NO_PULL)); \
            _CONFIG_PERIPHERAL(E, 2, PE_2_SPI1_SCK); _CONFIG_PERIPHERAL(E, 1, (PE_1_SPI1_SOUT | PORT_SRE_FAST | PORT_DSE_HIGH)); _CONFIG_PERIPHERAL(E, 3, (PE_3_SPI1_SIN | PORT_PS_UP_ENABLE)); \
            _CONFIG_DRIVE_PORT_OUTPUT_VALUE(E, SPI_CS1_0, SPI_CS1_0, (PORT_SRE_FAST | PORT_DSE_HIGH)); \
            SPI1_CTAR0 = (SPI_CTAR_ASC_6 | SPI_CTAR_FMSZ_8 | SPI_CTAR_CPHA | SPI_CTAR_CPOL | SPI_CTAR_BR_128); SPI1_MCR = (SPI_MCR_DIS_TXF | SPI_MCR_DIS_RXF | SPI_MCR_MSTR | SPI_MCR_DCONF_SPI | SPI_MCR_CLR_RXF | SPI_MCR_CLR_TXF | SPI_MCR_PCSIS_CS0 | SPI_MCR_PCSIS_CS1 | SPI_MCR_PCSIS_CS2 | SPI_MCR_PCSIS_CS3 | SPI_MCR_PCSIS_CS4 | SPI_MCR_PCSIS_CS5)

            #define ENABLE_SPI_SD_OPERATION()
            #define SET_SD_CARD_MODE()

            // Set maximum speed
            //
            #define SET_SPI_SD_INTERFACE_FULL_SPEED() SPI1_MCR |= SPI_MCR_HALT; SPI1_CTAR0 = (SPI_CTAR_FMSZ_8 | SPI_CTAR_CPOL | SPI_CTAR_CPHA | SPI_CTAR_BR_2); SPI1_MCR &= ~SPI_MCR_HALT;
            #if defined _WINDOWS
                #define WRITE_SPI_CMD(byte)     SPI1_SR &= ~(SPI_SR_RFDF); SPI1_PUSHR = (byte | SPI_PUSHR_PCS_NONE | SPI_PUSHR_CTAS_CTAR0); SPI1_POPR = _fnSimSD_write((unsigned char)byte)
                #define WAIT_TRANSMISSON_END() while (!(SPI1_SR & (SPI_SR_RFDF))) { SPI1_SR |= (SPI_SR_RFDF); }
                #define READ_SPI_DATA()        (unsigned char)SPI1_POPR
            #else
                #define WRITE_SPI_CMD(byte)    SPI1_SR = (SPI_SR_RFDF); SPI1_PUSHR = (byte | SPI_PUSHR_PCS_NONE | SPI_PUSHR_CTAS_CTAR0) // clear flags before transmitting (and receiving) a single byte
                #define WAIT_TRANSMISSON_END() while (!(SPI1_SR & (SPI_SR_RFDF))) {}
                #define READ_SPI_DATA()        (unsigned char)SPI1_POPR
            #endif
            #define SET_SD_DI_CS_HIGH()        _SETBITS(E, SPI_CS1_0)    // force DI and CS lines high ready for the initialisation sequence
            #define SET_SD_CS_LOW()            _CLEARBITS(E, SPI_CS1_0)  // assert the CS line of the SD card to be read
            #define SET_SD_CS_HIGH()           _SETBITS(E, SPI_CS1_0)    // negate the CS line of the SD card to be read
            #define POWER_UP_SD_CARD()                                   // apply power to the SD card if appropriate
            #define GET_SDCARD_WP_STATE()     (!_READ_PORT_MASK(C, WRITE_PROTECT_INPUT)) // when the input is read as '0' the card is protected from writes
        #endif
        #define POWER_DOWN_SD_CARD()                                     // remove power from SD card interface
    #elif defined FRDM_KE02Z || defined FRDM_KE02Z40M || defined FRDM_KE06Z || defined FRDM_KE04Z || defined TRK_KEA128 || defined TRK_KEA64 || defined TRK_KEA8 || defined FRDM_KEAZN32Q64 || defined FRDM_KEAZ64Q64 || defined FRDM_KEAZ128Q80
        // Configure to suit special connection SPI mode at between 100k and 400k (SPI0)
        // - SPI0_CS   PTE-3 (J9-7) [VDD J9-4 / 0V J9-14]
        // - SPI0_SCK  PTE-0 (J9-1)
        // - SPI0_MOSI PTE-1 (J9-3)
        // - SPI0_MISO PTE-2 (J9-5)
        //
        #define SPI_CS1_0              PORTE_BIT3
        #define INITIALISE_SPI_SD_INTERFACE() POWER_UP(4, SIM_SCGC4_SPI0); \
        SIM_PINSEL0 |= SIM_PINSEL_SPI0PS; \
        _CONFIG_PERIPHERAL(E, 0, (PE_0_SPI0_SCK)); \
        _CONFIG_PERIPHERAL(E, 1, (PE_1_SPI0_MOSI | PORT_SRE_FAST | PORT_DSE_HIGH)); \
        _CONFIG_DRIVE_PORT_OUTPUT_VALUE(B, SPI_CS1_0, SPI_CS1_0, (PORT_SRE_FAST | PORT_DSE_HIGH)); \
        SPI0_C1 = (SPI_C1_CPHA | SPI_C1_CPOL | SPI_C1_MSTR | SPI_C1_SPE); \
        _CONFIG_PERIPHERAL(E, 2, (PE_2_SPI0_MISO | PORT_PS_UP_ENABLE)); \
        SPI0_BR = (SPI_BR_SPPR_PRE_8 | SPI_BR_SPR_DIV_16); \
        (void)SPI0_S; (void)SPI0_D
        #define ENABLE_SPI_SD_OPERATION()
        #define SET_SD_CARD_MODE()

        // Set maximum speed
        //
        #define SET_SPI_SD_INTERFACE_FULL_SPEED() SPI0_BR = (SPI_BR_SPPR_PRE_1 | SPI_BR_SPR_DIV_2)
        #if defined _WINDOWS
            #define WRITE_SPI_CMD(byte)    SPI0_D = (byte); SPI0_D = _fnSimSD_write((unsigned char)byte)
            #define WAIT_TRANSMISSON_END() while (!(SPI0_S & (SPI_S_SPRF))) { SPI0_S |= (SPI_S_SPRF); }
            #define READ_SPI_DATA()        (unsigned char)SPI0_D
        #else
            #define WRITE_SPI_CMD(byte)    SPI0_D = (byte)
            #define WAIT_TRANSMISSON_END() while (!(SPI0_S & (SPI_S_SPRF))) {}
            #define READ_SPI_DATA()        (unsigned char)SPI0_D
        #endif
        #define SET_SD_DI_CS_HIGH()  _SETBITS(B, SPI_CS1_0)              // force DI and CS lines high ready for the initialisation sequence
        #define SET_SD_CS_LOW()      _CLEARBITS(B, SPI_CS1_0)            // assert the CS line of the SD card to be read
        #define SET_SD_CS_HIGH()     _SETBITS(B, SPI_CS1_0)              // negate the CS line of the SD card to be read
        #define POWER_UP_SD_CARD()                                       // apply power to the SD card if appropriate
        #define POWER_DOWN_SD_CARD()
        #define GET_SDCARD_WP_STATE()  0                                 // never write protect
    #elif defined TEENSY_LC
        // Configure to suit special connection SPI mode at between 100k and 400k (SPI0)
        //
        #define SPI_CS1_0              PORTA_BIT2
        #define INITIALISE_SPI_SD_INTERFACE() POWER_UP(4, SIM_SCGC4_SPI0); \
        _CONFIG_PERIPHERAL(C, 5, PC_5_SPI0_SCK); \
        _CONFIG_PERIPHERAL(C, 6, (PC_6_SPI0_MOSI | PORT_SRE_FAST | PORT_DSE_HIGH)); \
        _CONFIG_PERIPHERAL(C, 7, (PC_7_SPI0_MISO | PORT_PS_UP_ENABLE)); \
        _CONFIG_DRIVE_PORT_OUTPUT_VALUE(A, SPI_CS1_0, SPI_CS1_0, (PORT_SRE_FAST | PORT_DSE_HIGH)); \
        SPI0_C1 = (SPI_C1_CPHA | SPI_C1_CPOL | SPI_C1_MSTR | SPI_C1_SPE); \
        SPI0_BR = (SPI_BR_SPPR_PRE_8 | SPI_BR_SPR_DIV_16); \
        (void)SPI0_S; (void)SPI0_D
        #define ENABLE_SPI_SD_OPERATION()
        #define SET_SD_CARD_MODE()

        // Set maximum speed
        //
        #define SET_SPI_SD_INTERFACE_FULL_SPEED() SPI0_BR = (SPI_BR_SPPR_PRE_1 | SPI_BR_SPR_DIV_2)
        #if defined _WINDOWS
            #define WRITE_SPI_CMD(byte)    SPI0_D = (byte); SPI0_D = _fnSimSD_write((unsigned char)byte)
            #define WAIT_TRANSMISSON_END() while ((SPI0_S & (SPI_S_SPRF)) == 0) { SPI0_S |= (SPI_S_SPRF); }
            #define READ_SPI_DATA()        (unsigned char)SPI0_D
        #else
            #define WRITE_SPI_CMD(byte)    SPI0_D = (byte)
            #define WAIT_TRANSMISSON_END() while ((SPI0_S & (SPI_S_SPRF)) == 0) {}
            #define READ_SPI_DATA()        (unsigned char)SPI0_D
        #endif
        #define SET_SD_DI_CS_HIGH()  _SETBITS(A, SPI_CS1_0)              // force DI and CS lines high ready for the initialisation sequence
        #define SET_SD_CS_LOW()      _CLEARBITS(A, SPI_CS1_0)            // assert the CS line of the SD card to be read
        #define SET_SD_CS_HIGH()     _SETBITS(A, SPI_CS1_0)              // negate the CS line of the SD card to be read
        #define POWER_UP_SD_CARD()                                       // apply power to the SD card if appropriate
        #define POWER_DOWN_SD_CARD()
        #define GET_SDCARD_WP_STATE()  0                                 // never write protect
    #elif defined tinyK20 || defined BLAZE_K22 || defined TWR_KV10Z32
        // Configure to suit special connection SPI mode at between 100k and 400k (SPI0)
        //
        #if defined BLAZE_K22
            #define SPI_CS1_0             PORTC_BIT4
        #else
            #define SPI_CS1_0             PORTC_BIT1
            #define SDCARD_DETECT_PIN     PORTC_BIT0                     // '1' when SD card is inserted
        #endif
        #define INITIALISE_SPI_SD_INTERFACE() POWER_UP(6, SIM_SCGC6_SPI0); \
        _CONFIG_PERIPHERAL(C, 5, PC_5_SPI0_SCK | PORT_SRE_FAST | PORT_DSE_HIGH); \
        _CONFIG_PERIPHERAL(C, 6, (PC_6_SPI0_SOUT | PORT_SRE_FAST | PORT_DSE_HIGH)); \
        _CONFIG_PERIPHERAL(C, 7, (PC_7_SPI0_SIN | PORT_PS_UP_ENABLE)); \
        _CONFIG_DRIVE_PORT_OUTPUT_VALUE(C, SPI_CS1_0, SPI_CS1_0, (PORT_SRE_FAST | PORT_DSE_HIGH)); \
        SPI0_MCR |= SPI_MCR_HALT; \
        SPI0_CTAR0 = (SPI_CTAR_ASC_6 | SPI_CTAR_FMSZ_8 | SPI_CTAR_CPHA | SPI_CTAR_CPOL | SPI_CTAR_BR_64); \
        SPI0_MCR = (SPI_MCR_HALT | (SPI_MCR_DIS_TXF | SPI_MCR_DIS_RXF | SPI_MCR_MSTR | SPI_MCR_DCONF_SPI | SPI_MCR_CLR_RXF | SPI_MCR_CLR_TXF | SPI_MCR_PCSIS_CS0 | SPI_MCR_PCSIS_CS1 | SPI_MCR_PCSIS_CS2 | SPI_MCR_PCSIS_CS3 | SPI_MCR_PCSIS_CS4 | SPI_MCR_PCSIS_CS5)); \
        SPI0_MCR = (0 | (SPI_MCR_DIS_TXF | SPI_MCR_DIS_RXF | SPI_MCR_MSTR | SPI_MCR_DCONF_SPI | SPI_MCR_CLR_RXF | SPI_MCR_CLR_TXF | SPI_MCR_PCSIS_CS0 | SPI_MCR_PCSIS_CS1 | SPI_MCR_PCSIS_CS2 | SPI_MCR_PCSIS_CS3 | SPI_MCR_PCSIS_CS4 | SPI_MCR_PCSIS_CS5));

        #define SET_SD_DI_CS_HIGH()  _SETBITS(C, SPI_CS1_0)              // force DI and CS lines high ready for the initialisation sequence
        #define SET_SD_CS_LOW()      _CLEARBITS(C, SPI_CS1_0)            // assert the CS line of the SD card to be read
        #define SET_SD_CS_HIGH()     _SETBITS(C, SPI_CS1_0)              // negate the CS line of the SD card to be read

        #define ENABLE_SPI_SD_OPERATION()
        #define SET_SD_CARD_MODE()

        // Set maximum speed
        //
        #define SET_SPI_SD_INTERFACE_FULL_SPEED() SPI0_MCR |= SPI_MCR_HALT; SPI0_CTAR0 = (SPI_CTAR_FMSZ_8 | SPI_CTAR_CPOL | SPI_CTAR_CPHA | SPI_CTAR_BR_2 | SPI_CTAR_DBR); SPI0_MCR &= ~SPI_MCR_HALT;
        #if defined _WINDOWS
            #define WRITE_SPI_CMD(byte)    SPI0_SR &= ~(SPI_SR_RFDF); SPI0_PUSHR = (byte | SPI_PUSHR_PCS_NONE | SPI_PUSHR_CTAS_CTAR0); SPI0_POPR = _fnSimSD_write((unsigned char)byte)
            #define WAIT_TRANSMISSON_END() while ((SPI0_SR & (SPI_SR_RFDF)) == 0) { SPI0_SR |= (SPI_SR_RFDF); }
            #define READ_SPI_DATA()        (unsigned char)SPI0_POPR
        #else
            #define WRITE_SPI_CMD(byte)    SPI0_SR = (SPI_SR_RFDF); SPI0_PUSHR = (byte | SPI_PUSHR_PCS_NONE | SPI_PUSHR_CTAS_CTAR0) // clear flags before transmitting (and receiving) a single byte
            #define WAIT_TRANSMISSON_END() while ((SPI0_SR & (SPI_SR_RFDF)) == 0) {}
            #define READ_SPI_DATA()        (unsigned char)SPI0_POPR
        #endif
        #define POWER_UP_SD_CARD()                                       // apply power to the SD card if appropriate
        #define POWER_DOWN_SD_CARD()                                     // remove power from SD card interface
        #define GET_SDCARD_WP_STATE()     0                              // no write protect switch (always write enabled)

        #if defined BLAZE_K22
            #define SDCARD_DETECTION()    (_READ_PORT_MASK(D, SDCARD_DETECT_PIN) == 0) // card detection input
            #define SDCARD_DETECT_PORT    PORTD
            #define PRIORITY_SDCARD_DETECT_PORT_INT     PRIORITY_PORT_D_INT
        #else
            #define SDCARD_DETECTION()    (_READ_PORT_MASK(C, SDCARD_DETECT_PIN) != 0) // card detection input
            #define SDCARD_DETECT_PORT    PORTC
            #define PRIORITY_SDCARD_DETECT_PORT_INT     PRIORITY_PORT_C_INT
        #endif
    #elif defined FRDM_KL27Z
        // Configure to suit special connection SPI mode at between 100k and 400k (SPI0)
        //
        #define SPI_CS1_0              PORTC_BIT4
        #define INITIALISE_SPI_SD_INTERFACE() POWER_UP(4, SIM_SCGC4_SPI0); \
        _CONFIG_PERIPHERAL(C, 5, PC_5_SPI0_SCK); \
        _CONFIG_PERIPHERAL(C, 6, (PC_6_SPI0_MOSI | PORT_SRE_FAST | PORT_DSE_HIGH)); \
        _CONFIG_PERIPHERAL(C, 7, (PC_7_SPI0_MISO | PORT_PS_UP_ENABLE)); \
        _CONFIG_DRIVE_PORT_OUTPUT_VALUE(C, SPI_CS1_0, SPI_CS1_0, (PORT_SRE_FAST | PORT_DSE_HIGH)); \
        SPI0_C1 = (SPI_C1_CPHA | SPI_C1_CPOL | SPI_C1_MSTR | SPI_C1_SPE); \
        SPI0_BR = (SPI_BR_SPPR_PRE_8 | SPI_BR_SPR_DIV_16); \
        (void)SPI0_S; (void)SPI0_D
        #define ENABLE_SPI_SD_OPERATION()
        #define SET_SD_CARD_MODE()

        // Set maximum speed
        //
        #define SET_SPI_SD_INTERFACE_FULL_SPEED() SPI0_BR = (SPI_BR_SPPR_PRE_1 | SPI_BR_SPR_DIV_2)
        #if defined _WINDOWS
            #define WRITE_SPI_CMD(byte)    SPI0_D = (byte); SPI0_D = _fnSimSD_write((unsigned char)byte)
            #define WAIT_TRANSMISSON_END() while ((SPI0_S & (SPI_S_SPRF)) == 0) { SPI0_S |= (SPI_S_SPRF); }
            #define READ_SPI_DATA()        (unsigned char)SPI0_D
        #else
            #define WRITE_SPI_CMD(byte)    SPI0_D = (byte)
            #define WAIT_TRANSMISSON_END() while ((SPI0_S & (SPI_S_SPRF)) == 0) {}
            #define READ_SPI_DATA()        (unsigned char)SPI0_D
        #endif
        #define SET_SD_DI_CS_HIGH()  _SETBITS(C, SPI_CS1_0)              // force DI and CS lines high ready for the initialisation sequence
        #define SET_SD_CS_LOW()      _CLEARBITS(C, SPI_CS1_0)            // assert the CS line of the SD card to be read
        #define SET_SD_CS_HIGH()     _SETBITS(C, SPI_CS1_0)              // negate the CS line of the SD card to be read
        #define POWER_UP_SD_CARD()                                       // apply power to the SD card if appropriate
        #define POWER_DOWN_SD_CARD()
        #define GET_SDCARD_WP_STATE()  0                                 // never write protect
    #elif defined CAPUCCINO_KL27 || defined KL43Z_256_32_CL
        // Configure to suit special connection SPI mode at between 100k and 400k (SPI1)
        //
        #if defined KL43Z_256_32_CL
            #define SPI_CS1_0              PORTD_BIT4
            #define INITIALISE_SPI_SD_INTERFACE() POWER_UP(4, SIM_SCGC4_SPI1); \
            _CONFIG_PERIPHERAL(D, 5, PD_5_SPI1_SCK); \
            _CONFIG_PERIPHERAL(B, 16, (PB_16_SPI1_MOSI | PORT_SRE_FAST | PORT_DSE_HIGH)); \
            _CONFIG_PERIPHERAL(B, 17, (PB_17_SPI1_MISO | PORT_PS_UP_ENABLE)); \
            _CONFIG_DRIVE_PORT_OUTPUT_VALUE(D, SPI_CS1_0, SPI_CS1_0, (PORT_SRE_FAST | PORT_DSE_HIGH)); \
            SPI1_C1 = (SPI_C1_CPHA | SPI_C1_CPOL | SPI_C1_MSTR | SPI_C1_SPE); \
            SPI1_BR = (SPI_BR_SPPR_PRE_8 | SPI_BR_SPR_DIV_16); \
            (void)SPI1_S; (void)SPI1_D
        #else
            #define SD_CARD_DETECTION      PORTB_BIT3
            #define SPI_CS1_0              PORTD_BIT0
            #define INITIALISE_SPI_SD_INTERFACE() POWER_UP(4, SIM_SCGC4_SPI1); \
            _CONFIG_PORT_INPUT_FAST_LOW(B, (SD_CARD_DETECTION), PORT_PS_UP_ENABLE); \
            _CONFIG_PERIPHERAL(C, 3, PC_3_SPI1_SCK); \
            _CONFIG_PERIPHERAL(B, 16, (PB_16_SPI1_MOSI | PORT_SRE_FAST | PORT_DSE_HIGH)); \
            _CONFIG_PERIPHERAL(B, 17, (PB_17_SPI1_MISO | PORT_PS_UP_ENABLE)); \
            _CONFIG_DRIVE_PORT_OUTPUT_VALUE(D, SPI_CS1_0, SPI_CS1_0, (PORT_SRE_FAST | PORT_DSE_HIGH)); \
            SPI1_C1 = (SPI_C1_CPHA | SPI_C1_CPOL | SPI_C1_MSTR | SPI_C1_SPE); \
            SPI1_BR = (SPI_BR_SPPR_PRE_8 | SPI_BR_SPR_DIV_16); \
            (void)SPI1_S; (void)SPI1_D

            #define SDCARD_DETECTION()     (_READ_PORT_MASK(B, SD_CARD_DETECTION) == 0) // card detection input
        #endif
        #define ENABLE_SPI_SD_OPERATION()
        #define SET_SD_CARD_MODE()

        // Set maximum speed
        //
        #define SET_SPI_SD_INTERFACE_FULL_SPEED() SPI1_BR = (SPI_BR_SPPR_PRE_1 | SPI_BR_SPR_DIV_2)
        #if defined _WINDOWS
            #define WRITE_SPI_CMD(byte)    SPI1_D = (byte); SPI1_D = _fnSimSD_write((unsigned char)byte)
            #define WAIT_TRANSMISSON_END() while ((SPI1_S & (SPI_S_SPRF)) == 0) { SPI1_S |= (SPI_S_SPRF); }
            #define READ_SPI_DATA()        (unsigned char)SPI1_D
        #else
            #define WRITE_SPI_CMD(byte)    SPI1_D = (byte)
            #define WAIT_TRANSMISSON_END() while ((SPI1_S & (SPI_S_SPRF)) == 0) {}
            #define READ_SPI_DATA()        (unsigned char)SPI1_D
        #endif
        #define SET_SD_DI_CS_HIGH()  _SETBITS(D, SPI_CS1_0)              // force DI and CS lines high ready for the initialisation sequence
        #define SET_SD_CS_LOW()      _CLEARBITS(D, SPI_CS1_0)            // assert the CS line of the SD card to be read
        #define SET_SD_CS_HIGH()     _SETBITS(D, SPI_CS1_0)              // negate the CS line of the SD card to be read
        #define POWER_UP_SD_CARD()                                       // apply power to the SD card if appropriate
        #define POWER_DOWN_SD_CARD()
        #define GET_SDCARD_WP_STATE()  0                                 // never write protect
    #elif defined FRDM_KL25Z && defined FRDM_FXS_MULTI_B
        // Configure to suit FRDM-FXS-MULTI-B SPI mode at between 100k and 400k (SPI0)
        // - SPI0_CS   PTB-2 (J10-6)
        // - SPI0_SCK  PTD-1 (J2-12)
        // - SPI0_MOSI PTD-2 (J2-8)
        // - SPI0_MISO PTD-3 (J2-10)
        //
        #define SPI_CS1_0              PORTB_BIT2
        #define INITIALISE_SPI_SD_INTERFACE() POWER_UP(4, SIM_SCGC4_SPI0); \
        _CONFIG_PERIPHERAL(D, 1, PD_1_SPI0_SCK); \
        _CONFIG_PERIPHERAL(D, 2, (PD_2_SPI0_MOSI | PORT_SRE_FAST | PORT_DSE_HIGH)); \
        _CONFIG_PERIPHERAL(D, 3, (PD_3_SPI0_MISO | PORT_PS_UP_ENABLE)); \
        _CONFIG_DRIVE_PORT_OUTPUT_VALUE(B, SPI_CS1_0, SPI_CS1_0, (PORT_SRE_FAST | PORT_DSE_HIGH)); \
        SPI0_C1 = (SPI_C1_CPHA | SPI_C1_CPOL | SPI_C1_MSTR | SPI_C1_SPE); \
        SPI0_BR = (SPI_BR_SPPR_PRE_8 | SPI_BR_SPR_DIV_16); \
        (void)SPI0_S; (void)SPI0_D
        #define ENABLE_SPI_SD_OPERATION()
        #define SET_SD_CARD_MODE()

        // Set maximum speed
        //
        #define SET_SPI_SD_INTERFACE_FULL_SPEED() SPI0_BR = (SPI_BR_SPPR_PRE_1 | SPI_BR_SPR_DIV_2)
        #if defined _WINDOWS
            #define WRITE_SPI_CMD(byte)    SPI0_D = (byte); SPI0_D = _fnSimSD_write((unsigned char)byte)
            #define WAIT_TRANSMISSON_END() while (!(SPI0_S & (SPI_S_SPRF))) { SPI0_S |= (SPI_S_SPRF); }
            #define READ_SPI_DATA()        (unsigned char)SPI0_D
        #else
            #define WRITE_SPI_CMD(byte)    SPI0_D = (byte)
            #define WAIT_TRANSMISSON_END() while (!(SPI0_S & (SPI_S_SPRF))) {}
            #define READ_SPI_DATA()        (unsigned char)SPI0_D
        #endif
        #define SET_SD_DI_CS_HIGH()  _SETBITS(B, SPI_CS1_0)              // force DI and CS lines high ready for the initialisation sequence
        #define SET_SD_CS_LOW()      _CLEARBITS(B, SPI_CS1_0)            // assert the CS line of the SD card to be read
        #define SET_SD_CS_HIGH()     _SETBITS(B, SPI_CS1_0)              // negate the CS line of the SD card to be read
        #define POWER_UP_SD_CARD()                                       // apply power to the SD card if appropriate
        #define POWER_DOWN_SD_CARD()
        #define GET_SDCARD_WP_STATE()  0                                 // never write protect
    #elif defined FRDM_KL25Z
        // Configure to suit special connection SPI mode at between 100k and 400k (SPI1)
        // - SPI1_CS   PTD-4 (J1-6) [VDD J9-4 / 0V J9-14]
        // - SPI1_SCK  PTD-5 (J2-4)
        // - SPI1_MOSI PTD-6 (J2-17)
        // - SPI1_MISO PTD-7 (J2-19)
        //
        #define SPI_CS1_0              PORTD_BIT4
        #define INITIALISE_SPI_SD_INTERFACE() POWER_UP(4, SIM_SCGC4_SPI1); \
        _CONFIG_PERIPHERAL(D, 5, PD_5_SPI1_SCK); \
        _CONFIG_PERIPHERAL(D, 6, (PD_6_SPI1_MOSI | PORT_SRE_FAST | PORT_DSE_HIGH)); \
        _CONFIG_PERIPHERAL(D, 7, (PD_7_SPI1_MISO | PORT_PS_UP_ENABLE)); \
        _CONFIG_DRIVE_PORT_OUTPUT_VALUE(D, SPI_CS1_0, SPI_CS1_0, (PORT_SRE_FAST | PORT_DSE_HIGH)); \
        SPI1_C1 = (SPI_C1_CPHA | SPI_C1_CPOL | SPI_C1_MSTR | SPI_C1_SPE); \
        SPI1_BR = (SPI_BR_SPPR_PRE_8 | SPI_BR_SPR_DIV_16); \
        (void)SPI1_S; (void)SPI1_D
        #define ENABLE_SPI_SD_OPERATION()
        #define SET_SD_CARD_MODE()

        // Set maximum speed
        //
        #define SET_SPI_SD_INTERFACE_FULL_SPEED() SPI1_BR = (SPI_BR_SPPR_PRE_1 | SPI_BR_SPR_DIV_2)
        #if defined _WINDOWS
            #define WRITE_SPI_CMD(byte)    SPI1_D = (byte); SPI1_D = _fnSimSD_write((unsigned char)byte)
            #define WAIT_TRANSMISSON_END() while (!(SPI1_S & (SPI_S_SPRF))) { SPI1_S |= (SPI_S_SPRF); }
            #define READ_SPI_DATA()        (unsigned char)SPI1_D
        #else
            #define WRITE_SPI_CMD(byte)    SPI1_D = (byte)
            #define WAIT_TRANSMISSON_END() while (!(SPI1_S & (SPI_S_SPRF))) {}
            #define READ_SPI_DATA()        (unsigned char)SPI1_D
        #endif
        #define SET_SD_DI_CS_HIGH()  _SETBITS(D, SPI_CS1_0)              // force DI and CS lines high ready for the initialisation sequence
        #define SET_SD_CS_LOW()      _CLEARBITS(D, SPI_CS1_0)            // assert the CS line of the SD card to be read
        #define SET_SD_CS_HIGH()     _SETBITS(D, SPI_CS1_0)              // negate the CS line of the SD card to be read
        #define POWER_UP_SD_CARD()                                       // apply power to the SD card if appropriate
        #define POWER_DOWN_SD_CARD()
        #define GET_SDCARD_WP_STATE()  0                                 // never write protect
    #elif defined FRDM_KL26Z || defined TWR_KL46Z48M || defined FRDM_KL46Z || defined FRDM_KL43Z || defined rcARM_KL26
        // Configure to suit special connection SPI mode at between 100k and 400k (SPI1)
        // - SPI1_CS   PTD-4 (J2-6) [VDD J3-4 / 0V J3-14]
        // - SPI1_SCK  PTD-5 (J2-12)
        // - SPI1_MOSI PTD-6 (J2-8)
        // - SPI1_MISO PTD-7 (J2-10)
        //
        #define SPI_CS1_0              PORTD_BIT4
        #define INITIALISE_SPI_SD_INTERFACE() POWER_UP(4, SIM_SCGC4_SPI1); \
        _CONFIG_PERIPHERAL(D, 5, PD_5_SPI1_SCK); \
        _CONFIG_PERIPHERAL(D, 6, (PD_6_SPI1_MOSI | PORT_SRE_FAST | PORT_DSE_HIGH)); \
        _CONFIG_PERIPHERAL(D, 7, (PD_7_SPI1_MISO | PORT_PS_UP_ENABLE)); \
        _CONFIG_DRIVE_PORT_OUTPUT_VALUE(D, SPI_CS1_0, SPI_CS1_0, (PORT_SRE_FAST | PORT_DSE_HIGH)); \
        SPI1_C1 = (SPI_C1_CPHA | SPI_C1_CPOL | SPI_C1_MSTR | SPI_C1_SPE); \
        SPI1_BR = (SPI_BR_SPPR_PRE_8 | SPI_BR_SPR_DIV_16); \
        (void)SPI1_S; (void)SPI1_D
        #define ENABLE_SPI_SD_OPERATION()
        #define SET_SD_CARD_MODE()

        // Set maximum speed
        //
        #define SET_SPI_SD_INTERFACE_FULL_SPEED() SPI1_BR = (SPI_BR_SPPR_PRE_1 | SPI_BR_SPR_DIV_2)
        #if defined _WINDOWS
            #define WRITE_SPI_CMD(byte)    SPI1_D = (byte); SPI1_D = _fnSimSD_write((unsigned char)byte)
            #define WAIT_TRANSMISSON_END() while (!((unsigned char)SPI1_S & (SPI_S_SPRF))) { SPI1_S |= (SPI_S_SPRF); }
            #define READ_SPI_DATA()        (unsigned char)SPI1_D
        #else
            #define WRITE_SPI_CMD(byte)    SPI1_D = (byte)
            #define WAIT_TRANSMISSON_END() while (!((unsigned char)SPI1_S & (SPI_S_SPRF))) {}
            #define READ_SPI_DATA()        (unsigned char)SPI1_D
        #endif
        #define SET_SD_DI_CS_HIGH()  _SETBITS(D, SPI_CS1_0)              // force DI and CS lines high ready for the initialisation sequence
        #define SET_SD_CS_LOW()      _CLEARBITS(D, SPI_CS1_0)            // assert the CS line of the SD card to be read
        #define SET_SD_CS_HIGH()     _SETBITS(D, SPI_CS1_0)              // negate the CS line of the SD card to be read
        #define POWER_UP_SD_CARD()                                       // apply power to the SD card if appropriate
        #define POWER_DOWN_SD_CARD()
        #define GET_SDCARD_WP_STATE()  0                                 // never write protect
    #elif defined FRDM_K22F || defined TWR_K22F120M
        // Configure to suit SD card SPI mode at between 100k and 400k - use SPI0
        //
        #define SPI_CS1_0                  PORTC_BIT4
        #define INITIALISE_SPI_SD_INTERFACE() POWER_UP(6, SIM_SCGC6_SPI0); \
        _CONFIG_PERIPHERAL(D, 1, PD_1_SPI0_SCK); _CONFIG_PERIPHERAL(D, 2, (PD_2_SPI0_SOUT | PORT_SRE_FAST | PORT_DSE_HIGH)); _CONFIG_PERIPHERAL(D, 3, (PD_3_SPI0_SIN | PORT_PS_UP_ENABLE)); \
        _CONFIG_DRIVE_PORT_OUTPUT_VALUE(C, SPI_CS1_0, SPI_CS1_0, (PORT_SRE_FAST | PORT_DSE_HIGH)); \
        SPI0_CTAR0 = (SPI_CTAR_ASC_6 | SPI_CTAR_FMSZ_8 | SPI_CTAR_CPHA | SPI_CTAR_CPOL | SPI_CTAR_BR_128); SPI0_MCR = (SPI_MCR_DIS_TXF | SPI_MCR_DIS_RXF | SPI_MCR_MSTR | SPI_MCR_DCONF_SPI | SPI_MCR_CLR_RXF | SPI_MCR_CLR_TXF | SPI_MCR_PCSIS_CS0 | SPI_MCR_PCSIS_CS1 | SPI_MCR_PCSIS_CS2 | SPI_MCR_PCSIS_CS3 | SPI_MCR_PCSIS_CS4 | SPI_MCR_PCSIS_CS5)

        #define ENABLE_SPI_SD_OPERATION()
        #define SET_SD_CARD_MODE()

        // Set maximum speed
        //
        #define SET_SPI_SD_INTERFACE_FULL_SPEED() SPI0_MCR |= SPI_MCR_HALT; SPI0_CTAR0 = (SPI_CTAR_FMSZ_8 | SPI_CTAR_CPOL | SPI_CTAR_CPHA | SPI_CTAR_BR_2 | SPI_CTAR_DBR); SPI0_MCR &= ~SPI_MCR_HALT;
        #if defined _WINDOWS
            #define WRITE_SPI_CMD(byte)     SPI0_SR &= ~(SPI_SR_RFDF); SPI0_PUSHR = (byte | SPI_PUSHR_PCS_NONE | SPI_PUSHR_CTAS_CTAR0); SPI0_POPR = _fnSimSD_write((unsigned char)byte)
            #define WAIT_TRANSMISSON_END() while (!(SPI0_SR & (SPI_SR_RFDF))) { SPI0_SR |= (SPI_SR_RFDF); }
            #define READ_SPI_DATA()        (unsigned char)SPI0_POPR
        #else
            #define WRITE_SPI_CMD(byte)    SPI0_SR = (SPI_SR_RFDF); SPI0_PUSHR = (byte | SPI_PUSHR_PCS_NONE | SPI_PUSHR_CTAS_CTAR0) // clear flags before transmitting (and receiving) a single byte
            #define WAIT_TRANSMISSON_END() while (!(SPI0_SR & (SPI_SR_RFDF))) {}
            #define READ_SPI_DATA()        (unsigned char)SPI0_POPR
        #endif
        #define SET_SD_DI_CS_HIGH()  _SETBITS(C, SPI_CS1_0)              // force DI and CS lines high ready for the initialisation sequence
        #define SET_SD_CS_LOW()      _CLEARBITS(C, SPI_CS1_0)            // assert the CS line of the SD card to be read
        #define SET_SD_CS_HIGH()     _SETBITS(C, SPI_CS1_0)              // negate the CS line of the SD card to be read
        #define POWER_UP_SD_CARD()                                       // apply power to the SD card if appropriate

        #define POWER_DOWN_SD_CARD()                                     // remove power from SD card interface
        #define GET_SDCARD_WP_STATE() 0                                  // no write protect switch available
        #if defined FRDM_K22F
            #define SDCARD_DETECTION()     (_READ_PORT_MASK(B, SDCARD_DETECT_PIN) == 0) // card detection input
            #define PRIORITY_SDCARD_DETECT_PORT_INT   PRIORITY_PORT_B_INT// port priority when using card detect switch interrupt
            #define SDCARD_DETECT_PORT     PORTB                         // interrupt is on this port
        #else
            #define SDCARD_DETECTION()    0                              // card detection input not present
        #endif
    #elif defined TEENSY_3_1 || defined TWR_K20D72M
        // Configure to suit SD card SPI mode at between 100k and 400k (use SPI0)
        //
        #if defined TEENSY_3_1
            #define SPI_CS1_0             PORTA_BIT13
            #define INITIALISE_SPI_SD_INTERFACE() POWER_UP(6, SIM_SCGC6_SPI0); \
            _CONFIG_PERIPHERAL(C, 5, PC_5_SPI0_SCK | PORT_SRE_FAST | PORT_DSE_HIGH); \
            _CONFIG_PERIPHERAL(C, 6, (PC_6_SPI0_SOUT | PORT_SRE_FAST | PORT_DSE_HIGH)); \
            _CONFIG_PERIPHERAL(C, 7, (PC_7_SPI0_SIN | PORT_PS_UP_ENABLE)); \
            _CONFIG_DRIVE_PORT_OUTPUT_VALUE(A, SPI_CS1_0, SPI_CS1_0, (PORT_SRE_FAST | PORT_DSE_HIGH)); \
            SPI0_MCR |= SPI_MCR_HALT; \
            SPI0_CTAR0 = (SPI_CTAR_ASC_6 | SPI_CTAR_FMSZ_8 | SPI_CTAR_CPHA | SPI_CTAR_CPOL | SPI_CTAR_BR_64); \
            SPI0_MCR = (SPI_MCR_HALT | (SPI_MCR_DIS_TXF | SPI_MCR_DIS_RXF | SPI_MCR_MSTR | SPI_MCR_DCONF_SPI | SPI_MCR_CLR_RXF | SPI_MCR_CLR_TXF | SPI_MCR_PCSIS_CS0 | SPI_MCR_PCSIS_CS1 | SPI_MCR_PCSIS_CS2 | SPI_MCR_PCSIS_CS3 | SPI_MCR_PCSIS_CS4 | SPI_MCR_PCSIS_CS5)); \
            SPI0_MCR = (0 | (SPI_MCR_DIS_TXF | SPI_MCR_DIS_RXF | SPI_MCR_MSTR | SPI_MCR_DCONF_SPI | SPI_MCR_CLR_RXF | SPI_MCR_CLR_TXF | SPI_MCR_PCSIS_CS0 | SPI_MCR_PCSIS_CS1 | SPI_MCR_PCSIS_CS2 | SPI_MCR_PCSIS_CS3 | SPI_MCR_PCSIS_CS4 | SPI_MCR_PCSIS_CS5));


            #define SET_SD_DI_CS_HIGH()  _SETBITS(A, SPI_CS1_0)          // force DI and CS lines high ready for the initialisation sequence
            #define SET_SD_CS_LOW()      _CLEARBITS(A, SPI_CS1_0)        // assert the CS line of the SD card to be read
            #define SET_SD_CS_HIGH()     _SETBITS(A, SPI_CS1_0)          // negate the CS line of the SD card to be read
        #else                                                            // TWR_K20D72M
            #define SPI_CS1_0            PORTD_BIT4
            #define INITIALISE_SPI_SD_INTERFACE() POWER_UP(6, SIM_SCGC6_SPI0); \
            _CONFIG_PERIPHERAL(D, 1, PD_1_SPI0_SCK); _CONFIG_PERIPHERAL(D, 2, (PD_2_SPI0_SOUT | PORT_SRE_FAST | PORT_DSE_HIGH)); _CONFIG_PERIPHERAL(D, 3, (PD_3_SPI0_SIN | PORT_PS_UP_ENABLE)); \
            _CONFIG_DRIVE_PORT_OUTPUT_VALUE(D, SPI_CS1_0, SPI_CS1_0, (PORT_SRE_FAST | PORT_DSE_HIGH)); \
            SPI0_CTAR0 = (SPI_CTAR_ASC_6 | SPI_CTAR_FMSZ_8 | SPI_CTAR_CPHA | SPI_CTAR_CPOL | SPI_CTAR_BR_128); SPI0_MCR = (SPI_MCR_DIS_TXF | SPI_MCR_DIS_RXF | SPI_MCR_MSTR | SPI_MCR_DCONF_SPI | SPI_MCR_CLR_RXF | SPI_MCR_CLR_TXF | SPI_MCR_PCSIS_CS0 | SPI_MCR_PCSIS_CS1 | SPI_MCR_PCSIS_CS2 | SPI_MCR_PCSIS_CS3 | SPI_MCR_PCSIS_CS4 | SPI_MCR_PCSIS_CS5)

            #define SET_SD_DI_CS_HIGH()  _SETBITS(D, SPI_CS1_0)          // force DI and CS lines high ready for the initialisation sequence
            #define SET_SD_CS_LOW()      _CLEARBITS(D, SPI_CS1_0)        // assert the CS line of the SD card to be read
            #define SET_SD_CS_HIGH()     _SETBITS(D, SPI_CS1_0)          // negate the CS line of the SD card to be read
        #endif

        #define ENABLE_SPI_SD_OPERATION()
        #define SET_SD_CARD_MODE()

        // Set maximum speed
        //
        #define SET_SPI_SD_INTERFACE_FULL_SPEED() SPI0_MCR |= SPI_MCR_HALT; SPI0_CTAR0 = (SPI_CTAR_FMSZ_8 | SPI_CTAR_CPOL | SPI_CTAR_CPHA | SPI_CTAR_BR_2 | SPI_CTAR_DBR); SPI0_MCR &= ~SPI_MCR_HALT;
        #if defined _WINDOWS
            #define WRITE_SPI_CMD(byte)    SPI0_SR &= ~(SPI_SR_RFDF); SPI0_PUSHR = (byte | SPI_PUSHR_PCS_NONE | SPI_PUSHR_CTAS_CTAR0); SPI0_POPR = _fnSimSD_write((unsigned char)byte)
            #define WAIT_TRANSMISSON_END() while ((SPI0_SR & (SPI_SR_RFDF)) == 0) { SPI0_SR |= (SPI_SR_RFDF); }
            #define READ_SPI_DATA()        (unsigned char)SPI0_POPR
        #else
            #define WRITE_SPI_CMD(byte)    SPI0_SR = (SPI_SR_RFDF); SPI0_PUSHR = (byte | SPI_PUSHR_PCS_NONE | SPI_PUSHR_CTAS_CTAR0) // clear flags before transmitting (and receiving) a single byte
            #define WAIT_TRANSMISSON_END() while ((SPI0_SR & (SPI_SR_RFDF)) == 0) {}
            #define READ_SPI_DATA()        (unsigned char)SPI0_POPR
        #endif
        #define POWER_UP_SD_CARD()                                       // apply power to the SD card if appropriate
        #define POWER_DOWN_SD_CARD()                                     // remove power from SD card interface
        #define GET_SDCARD_WP_STATE()     0                              // no write protect switch (always write enabled)
    #elif defined K70F150M_12M
        #define SD_CONTROLLER_AVAILABLE                                  // use SDHC controller rather than SPI
        #define SET_SD_CS_HIGH()                                         // dummy since not required by SD controller
        #define SET_SD_CS_LOW()                                          // dummy since not required by SD controller
        #if defined _WINDOWS
            #define POWER_UP_SD_CARD()     SDHC_SYSCTL |= SDHC_SYSCTL_INITA; SDHC_SYSCTL &= ~SDHC_SYSCTL_INITA; // apply power to the SD card if appropriate (we use this to send 80 clocks - self-clearing bit)
        #else
            #define POWER_UP_SD_CARD()     SDHC_SYSCTL |= SDHC_SYSCTL_INITA; while (SDHC_SYSCTL & SDHC_SYSCTL_INITA) {}; // apply power to the SD card if appropriate (we use this to send 80 clocks)
        #endif
        #define SDHC_SYSCTL_SPEED_SLOW     (SDHC_SYSCTL_SDCLKFS_128 | SDHC_SYSCTL_DVS_3)  // 390kHz when 150MHz clock
        #define SDHC_SYSCTL_SPEED_FAST     (SDHC_SYSCTL_SDCLKFS_2 | SDHC_SYSCTL_DVS_2)    // 25MHz when 150MHz clock
        #define SET_SPI_SD_INTERFACE_FULL_SPEED() fnSetSD_clock(SDHC_SYSCTL_SPEED_FAST); SDHC_PROCTL |= SDHC_PROCTL_DTW_4BIT

        #undef T_CHECK_CARD_REMOVAL
        #undef SDCARD_DETECT_INPUT_POLL
        #define SDCARD_DETECT_INPUT_INTERRUPT                            // use card detect switch for detection by interrupt (T_CHECK_CARD_REMOVAL and SDCARD_DETECT_INPUT_POLL should be disabled)

        #define SD_PRESENT                 PORTA_BIT7
        #define SDCARD_DETECTION()         (_READ_PORT_MASK(A, SD_PRESENT) == 0) // card detection input
        #define PRIORITY_SDCARD_DETECT_PORT_INT   PRIORITY_PORT_A_INT    // port priority when using card detect switch interrupt
        #define SDCARD_DETECT_PORT         PORTA                         // interrupt is on this port
        #define SDCARD_DETECT_PIN          SD_PRESENT                    // interrupt pin

        #define GET_SDCARD_WP_STATE()      0
        #define POWER_DOWN_SD_CARD()                                     // dummy when SD controller used
    #elif defined K20FX512_120
        #define SD_CONTROLLER_AVAILABLE                                  // use SDHC controller rather than SPI
        #define WRITE_PROTECT_INPUT        0
        #define SD_CARD_DETECTION          PORTA_BIT11
        #define SET_SD_CS_HIGH()
        #define SET_SD_CS_LOW()
        #ifdef _WINDOWS
            #define POWER_UP_SD_CARD()     _CONFIG_PORT_INPUT(A, (SD_CARD_DETECTION), (PORT_PS_UP_ENABLE)); SDHC_SYSCTL |= SDHC_SYSCTL_INITA; SDHC_SYSCTL &= ~SDHC_SYSCTL_INITA; // apply power to the SD card if appropriate (we use this to send 80 clocks - self-clearing bit)
        #else
            #define POWER_UP_SD_CARD()     _CONFIG_PORT_INPUT(A, (SD_CARD_DETECTION), (PORT_PS_UP_ENABLE)); SDHC_SYSCTL |= SDHC_SYSCTL_INITA; while (SDHC_SYSCTL & SDHC_SYSCTL_INITA) {}; // apply power to the SD card if appropriate (we use this to send 80 clocks)
        #endif
        #define SDHC_SYSCTL_SPEED_SLOW     (SDHC_SYSCTL_SDCLKFS_64 | SDHC_SYSCTL_DVS_5) // 375kHz when 120MHz clock
        #define SDHC_SYSCTL_SPEED_FAST     (SDHC_SYSCTL_SDCLKFS_2 | SDHC_SYSCTL_DVS_3) // 20MHz when 120MHz clock
        #define SET_SPI_SD_INTERFACE_FULL_SPEED() fnSetSD_clock(SDHC_SYSCTL_SPEED_FAST); SDHC_PROCTL |= SDHC_PROCTL_DTW_4BIT

        #define POWER_DOWN_SD_CARD()                                     // remove power from SD card interface
        #define GET_SDCARD_WP_STATE()      0

        #define SDCARD_DETECTION()         (_READ_PORT_MASK(A, SD_CARD_DETECTION) == 0) // card detection input
        #define PRIORITY_SDCARD_DETECT_PORT_INT  PRIORITY_PORT_A_INT     // port priority when using card detect switch interrupt
        #define SDCARD_DETECT_PORT         PORTA                         // interrupt is on this port
        #define SDCARD_DETECT_PIN          SD_CARD_DETECTION             // interrupt pin
    #elif !defined SDCARD_CONFIG_COMPLETE                                // TWR_K40X256 / KWIKSTIK / TWR_K40D100M
        #if !defined KINETIS_KL && !defined KINETIS_KE
            #define SD_CONTROLLER_AVAILABLE                              // use SDHC controller rather than SPI
            #if defined KWIKSTIK_V3_V4
                #error "SD card can not be used on the KWIKSTIK revisions 3 or 4 due to a incorrect wiring - only possible on rev. 5"
            #endif
        #endif
        #define WRITE_PROTECT_INPUT            PORTE_BIT27
        #if defined SD_CONTROLLER_AVAILABLE
            #define SET_SD_CS_HIGH()                                     // dummy since not required by SD controller
            #define SET_SD_CS_LOW()                                      // dummy since not required by SD controller
            #if defined FRDM_K64F  || defined FreeLON                    // card detect switch has positive logic and needs a pull-down
                #if defined _WINDOWS
                    #define POWER_UP_SD_CARD()     _CONFIG_PORT_INPUT(E, (WRITE_PROTECT_INPUT), (PORT_PS_DOWN_ENABLE)); SDHC_SYSCTL |= SDHC_SYSCTL_INITA; SDHC_SYSCTL &= ~SDHC_SYSCTL_INITA; // apply power to the SD card if appropriate (we use this to send 80 clocks - self-clearing bit)
                #else
                    #define POWER_UP_SD_CARD()     _CONFIG_PORT_INPUT(E, (WRITE_PROTECT_INPUT), (PORT_PS_DOWN_ENABLE)); SDHC_SYSCTL |= SDHC_SYSCTL_INITA; while (SDHC_SYSCTL & SDHC_SYSCTL_INITA) {}; // apply power to the SD card if appropriate (we use this to send 80 clocks)
                #endif
            #else
                #if defined _WINDOWS
                    #define POWER_UP_SD_CARD()     _CONFIG_PORT_INPUT(E, (WRITE_PROTECT_INPUT), (PORT_PS_UP_ENABLE)); SDHC_SYSCTL |= SDHC_SYSCTL_INITA; SDHC_SYSCTL &= ~SDHC_SYSCTL_INITA; // apply power to the SD card if appropriate (we use this to send 80 clocks - self-clearing bit)
                #else
                    #define POWER_UP_SD_CARD()     _CONFIG_PORT_INPUT(E, (WRITE_PROTECT_INPUT), (PORT_PS_UP_ENABLE)); SDHC_SYSCTL |= SDHC_SYSCTL_INITA; while (SDHC_SYSCTL & SDHC_SYSCTL_INITA) {}; // apply power to the SD card if appropriate (we use this to send 80 clocks)
                #endif
            #endif
            #define SDHC_SYSCTL_SPEED_SLOW     (SDHC_SYSCTL_SDCLKFS_128 | SDHC_SYSCTL_DVS_2)  // 390kHz when 100MHz clock
            #define SDHC_SYSCTL_SPEED_FAST     (SDHC_SYSCTL_SDCLKFS_2 | SDHC_SYSCTL_DVS_2)    // 25MHz when 100MHz clock
            #define SET_SPI_SD_INTERFACE_FULL_SPEED() fnSetSD_clock(SDHC_SYSCTL_SPEED_FAST); SDHC_PROCTL |= SDHC_PROCTL_DTW_4BIT
        #else
            // Configure to suit SD card SPI mode at between 100k and 400k
            //
            #define SPI_CS1_0                  PORTE_BIT4
            #define INITIALISE_SPI_SD_INTERFACE() POWER_UP(6, SIM_SCGC6_SPI1); \
            _CONFIG_PORT_INPUT(E, (WRITE_PROTECT_INPUT), (PORT_PS_UP_ENABLE)); \
            _CONFIG_PORT_INPUT(E, (PORTE_BIT0), (PORT_NO_PULL)); \
            _CONFIG_PERIPHERAL(E, 2, PE_2_SPI1_SCK); _CONFIG_PERIPHERAL(E, 1, (PE_1_SPI1_SOUT | PORT_SRE_FAST | PORT_DSE_HIGH)); _CONFIG_PERIPHERAL(E, 3, (PE_3_SPI1_SIN | PORT_PS_UP_ENABLE)); \
            _CONFIG_DRIVE_PORT_OUTPUT_VALUE(E, SPI_CS1_0, SPI_CS1_0, (PORT_SRE_FAST | PORT_DSE_HIGH)); \
            SPI1_CTAR0 = (SPI_CTAR_ASC_6 | SPI_CTAR_FMSZ_8 | SPI_CTAR_CPHA | SPI_CTAR_CPOL | SPI_CTAR_BR_128); SPI1_MCR = (SPI_MCR_DIS_TXF | SPI_MCR_DIS_RXF | SPI_MCR_MSTR | SPI_MCR_DCONF_SPI | SPI_MCR_CLR_RXF | SPI_MCR_CLR_TXF | SPI_MCR_PCSIS_CS0 | SPI_MCR_PCSIS_CS1 | SPI_MCR_PCSIS_CS2 | SPI_MCR_PCSIS_CS3 | SPI_MCR_PCSIS_CS4 | SPI_MCR_PCSIS_CS5)

            #define ENABLE_SPI_SD_OPERATION()
            #define SET_SD_CARD_MODE()

            // Set maximum speed
            //
            #define SET_SPI_SD_INTERFACE_FULL_SPEED() SPI1_MCR |= SPI_MCR_HALT; SPI1_CTAR0 = (SPI_CTAR_FMSZ_8 | SPI_CTAR_CPOL | SPI_CTAR_CPHA | SPI_CTAR_BR_2); SPI1_MCR &= ~SPI_MCR_HALT;
            #if defined _WINDOWS
                #define WRITE_SPI_CMD(byte)     SPI1_SR &= ~(SPI_SR_RFDF); SPI1_PUSHR = (byte | SPI_PUSHR_PCS_NONE | SPI_PUSHR_CTAS_CTAR0); SPI1_POPR = _fnSimSD_write((unsigned char)byte)
                #define WAIT_TRANSMISSON_END() while ((SPI1_SR & (SPI_SR_RFDF)) == 0) { SPI1_SR |= (SPI_SR_RFDF); }
                #define READ_SPI_DATA()        (unsigned char)SPI1_POPR
            #else
                #define WRITE_SPI_CMD(byte)    SPI1_SR = (SPI_SR_RFDF); SPI1_PUSHR = (byte | SPI_PUSHR_PCS_NONE | SPI_PUSHR_CTAS_CTAR0) // clear flags before transmitting (and receiving) a single byte
                #define WAIT_TRANSMISSON_END() while ((SPI1_SR & (SPI_SR_RFDF)) == 0) {}
                #define READ_SPI_DATA()        (unsigned char)SPI1_POPR
            #endif
            #define SET_SD_DI_CS_HIGH()  _SETBITS(E, SPI_CS1_0)          // force DI and CS lines high ready for the initialisation sequence
            #define SET_SD_CS_LOW()      _CLEARBITS(E, SPI_CS1_0)        // assert the CS line of the SD card to be read
            #define SET_SD_CS_HIGH()     _SETBITS(E, SPI_CS1_0)          // negate the CS line of the SD card to be read
            #define POWER_UP_SD_CARD()                                   // apply power to the SD card if appropriate
        #endif
        #define POWER_DOWN_SD_CARD()                                     // remove power from SD card interface
        #if defined FreeLON
            #define GET_SDCARD_WP_STATE()  0                             // no write protect input
            #define SD_CARD_DETECTION      PORTD_BIT7
            #define SDCARD_DETECTION()     (_READ_PORT_MASK(D, SD_CARD_DETECTION) == 0) // card detection input
            #define PRIORITY_SDCARD_DETECT_PORT_INT   PRIORITY_PORT_D_INT // port priority when using card detect switch interrupt
            #define SDCARD_DETECT_PORT     PORTD                         // interrupt is on this port
            #define SDCARD_DETECT_PIN      SD_CARD_DETECTION             // interrupt pin
        #elif defined FRDM_K64F
            #define GET_SDCARD_WP_STATE()  0                             // no write protect input
            #define SD_CARD_DETECTION      PORTE_BIT6 
            #define SDCARD_DETECTION()     (_READ_PORT_MASK(E, SD_CARD_DETECTION) != 0 // card detection input
            #define PRIORITY_SDCARD_DETECT_PORT_INT   PRIORITY_PORT_E_INT // port priority when using card detect switch interrupt
            #define SDCARD_DETECT_PORT     PORTE                         // interrupt is on this port
            #define SDCARD_DETECT_PIN      SD_CARD_DETECTION             // interrupt pin
        #elif defined TWR_K40X256
            #define GET_SDCARD_WP_STATE() (_READ_PORT_MASK(E, WRITE_PROTECT_INPUT) != 0) // when the input is read as '1' the card is protected from writes
        #else
            #define GET_SDCARD_WP_STATE() (_READ_PORT_MASK(E, WRITE_PROTECT_INPUT) == 0) // when the input is read as '0' the card is protected from writes
        #endif
    #endif
#endif

#if defined SDRAM_CONTROLLER_AVAILABLE                                   // {14}
    #if defined SUPPORT_TFT || defined TFT_GLCD_MODE                     // TWR-LCD-RGB
        #define USE_SDRAM                                                // enable SDRAM since it is required by the LCD controller
        #define SDRAM_SIZE   (64 * 1024 * 1024)                          // 64 Meg SDRAM
        #define SDRAM_ADDR   0x70000000
    #else
        #define USE_SDRAM                                                // enable SDRAM
    #endif
    #if defined TWR_K70F120M || defined K70F150M_12M
        #define MT47H64M16HR                                             // used on K70 tower kit card
        #define DDRPORT_SYNC                                             // synchronous DDR
    #else
        #define MT46H32M16LFBF                                           // used on EMCRAFT_K70F120M/EMCRAFT_K61F150M
    #endif

    #if defined USE_SDRAM
        #if !defined USB_CLOCK_SOURCE_MCGPLL1CLK                         // {29} if FS USB defines the clock speed we don't determine it here
            #define CLOCK_DIV_1     5                                    // PLL1 uses clock input at 50MHz so divide by 5 to 10MHz
            #if KINETIS_MAX_DDR_SPEED >= 150000000
                #define CLOCK_MUL_1 30                                   // to give 150MHz output (300MHz PLL output is divided by 2)
            #else
                #define CLOCK_MUL_1 25                                   // to give 125MHz output (according to JEDEC DDR2 specification the speed should by >= 125MHz for DDR2)
            #endif
        #endif
      //#define SECONDARY_UMALLOC                                        // secondary uMalloc() area in SDRAM
      //#define HEAP2_ADDRESS     0x70000000                             // DRAM start address in memory map
      //#define HEAP2_SIZE        (64 * 1024 * 1024)                     // 64 Meg SDRAM

        #if defined SECONDARY_UMALLOC
            // Configure SRAM heap utilisation [undefine usage type to put heap to SDRAM rather than internal SRAM]
            // - application code can directly specify uMalloc() or uMalloc2() as required
            //
            #define OS_MALLOC(x)                  uMalloc2((unsigned long)(x))
          //#define USB_FS_MALLOC(x)              uMalloc2((unsigned long)(x))
          //#define USB_FS_MALLOC_ALIGN(x, y)     uMallocAlign2((MAX_MALLOC)(x), (y))
          //#define USB_HS_MALLOC(x)              uMalloc2((unsigned long)(x))
          //#define USB_HS_MALLOC_ALIGN(x, y)     uMallocAlign2((MAX_MALLOC)(x), (y))
            #define ETH_BD_MALLOC_ALIGN(x, y)     uMallocAlign2((MAX_MALLOC)(x), (y))
            #define ETH_RX_BUF_MALLOC_ALIGN(x, y) uMallocAlign2((MAX_MALLOC)(x), (y))
            #define ETH_TX_BUF_MALLOC_ALIGN(x, y) uMallocAlign2((MAX_MALLOC)(x), (y))
          //#define SSC_DRV_MALLOC(x)             uMalloc2((unsigned long)(x))
          //#define SCC_DRV_MALLOC_ALIGN(x, y)    uMallocAlign2((MAX_MALLOC)(x), (y))
            #define ARP_NN_MALLOC(x)              uMalloc2((unsigned long)(x))
          //#define HTTP_MALLOC(x)                uMalloc2((unsigned long)(x))
          //#define TCP_SOCKET_MALLOC(x)          uMalloc2((unsigned long)(x))
          //#define TCP_BUFFER_MALLOC(x)          uMalloc2((unsigned long)(x))
          //#define TELNET_MALLOC(x)              uMalloc2((unsigned long)(x))
          //#define UDP_SOCKET_MALLOC(x)          uMalloc2((unsigned long)(x))
          //#define QUEUE_MALLOC(x)               uMalloc2((unsigned long)(x))
          //#define TTY_DRV_MALLOC(x)             uMalloc2((unsigned long)(x))
          //#define USB_DRV_MALLOC(x)             uMalloc2((unsigned long)(x))
            #define ETH_DRV_MALLOC(x)             uMalloc2((unsigned long)(x))
          //#define UFILE_MALLOC(x)               uMalloc2((unsigned long)(x))
          //#define UNETWORK_MALLOC(x)            uMalloc2((unsigned long)(x))
          //#define NAND_MALLOC(x)                uMalloc2((unsigned long)(x))
          //#define SDCARD_MALLOC(x)              uMalloc2((unsigned long)(x))
        #endif
    #endif
#endif

// Character LCD interface
//
typedef unsigned long LCD_BUS_PORT_SIZE;                                 // we use 32 bit ports
typedef unsigned long LCD_CONTROL_PORT_SIZE;
//#define LCD_BUS_8BIT                                                   // data bus in 8 bit mode
#define LCD_BUS_4BIT                                                     // data bus in 4 bit mode

#if defined FRDM_KL25Z
    #if defined LCD_BUS_8BIT
        #error use 4-bit mode with FRDM-KL25Z
    #else
        #define LCD_BUS_MASK        0x0000f000                           // PTA 15,14,13,12
        #define DATA_SHIFT_RIGHT    0                                   
        #define DATA_SHIFT_LEFT     8                                    // nibble shift down required to bring data into position
    #endif

    #define O_CONTROL_RS            PORTE_BIT0
    #define O_WRITE_READ            PORTE_BIT1
    #define O_CONTROL_EN            PORTE_BIT2
    #define O_LCD_BACKLIGHT         PORTE_BIT3

    #define O_CONTROL_LINES         (O_CONTROL_RS | O_WRITE_READ | O_CONTROL_EN)
    #define IO_BUS_PORT_DAT_IN      GPIOA_PDIR

    #define SET_DATA_LINES_INPUT()  _CONFIG_PORT_INPUT(A, LCD_BUS_MASK, PORT_PS_UP_ENABLE)
    #define SET_DATA_LINES_OUTPUT() _DRIVE_PORT_OUTPUT(A, LCD_BUS_MASK)
    #define SET_CONTROL_LINES_OUTPUT(x) _CONFIG_PORT_OUTPUT(E, x, PORT_SRE_SLOW)
    #define SET_BUS_DATA(x)         _WRITE_PORT_MASK(A, x,  LCD_BUS_MASK)
    #define SET_CONTROL_LINES(x)    _WRITE_PORT_MASK(E, x, O_CONTROL_LINES)
    #define O_SET_CONTROL_LOW(x)    _CLEARBITS(E, x); _CLEARBITS(E, x); _CLEARBITS(E, x); _CLEARBITS(E, x); _SIM_PORTS;
    #define O_SET_CONTROL_HIGH(x)   _SETBITS(E, x); _SETBITS(E, x); _SETBITS(E, x); _SETBITS(E, x); _SETBITS(E, x); _SIM_PORTS;
#elif defined FRDM_KL03Z
    #if defined LCD_BUS_8BIT
        #error use 4-bit mode with FRDM-KL03Z
    #else
        #define LCD_BUS_MASK        0x000003c0                           // PTA 9,8,7,6
        #define DATA_SHIFT_RIGHT    0                                   
        #define DATA_SHIFT_LEFT     2                                    // nibble shift down required to bring data into position
    #endif

    #define O_CONTROL_RS            PORTB_BIT0
    #define O_WRITE_READ            PORTB_BIT5
    #define O_CONTROL_EN            PORTB_BIT6
    #define O_LCD_BACKLIGHT         PORTB_BIT7

    #define O_CONTROL_LINES         (O_CONTROL_RS | O_WRITE_READ | O_CONTROL_EN)
    #define IO_BUS_PORT_DAT_IN      GPIOA_PDIR

    #define SET_DATA_LINES_INPUT()  _CONFIG_PORT_INPUT(A, LCD_BUS_MASK, PORT_PS_UP_ENABLE)
    #define SET_DATA_LINES_OUTPUT() _DRIVE_PORT_OUTPUT(A, LCD_BUS_MASK)
    #define SET_CONTROL_LINES_OUTPUT(x) _CONFIG_PORT_OUTPUT(B, x, PORT_SRE_SLOW)
    #define SET_BUS_DATA(x)         _WRITE_PORT_MASK(A, x,  LCD_BUS_MASK)
    #define SET_CONTROL_LINES(x)    _WRITE_PORT_MASK(B, x, O_CONTROL_LINES)
    #define O_SET_CONTROL_LOW(x)    _CLEARBITS(B, x); _CLEARBITS(B, x); _CLEARBITS(B, x); _CLEARBITS(B, x); _CLEARBITS(B, x); _SIM_PORTS
    #define O_SET_CONTROL_HIGH(x)   _SETBITS(B, x);   _SETBITS(B, x);   _SETBITS(B, x);  _SETBITS(B, x); _SETBITS(B, x); _SIM_PORTS
#elif defined FRDM_K64F || defined FreeLON
    #if defined LCD_BUS_8BIT
        #error use 4-bit mode with FRDM-K64F
    #else
        #define LCD_BUS_MASK        0x00000780                           // PTC 10,9,8,7
        #define DATA_SHIFT_RIGHT    0                                   
        #define DATA_SHIFT_LEFT     3                                    // nibble shift down required to bring data into position
    #endif

    #define O_CONTROL_RS            PORTC_BIT0
    #define O_WRITE_READ            PORTC_BIT1
    #define O_CONTROL_EN            PORTC_BIT2
    #define O_LCD_BACKLIGHT         PORTC_BIT5

    #define O_CONTROL_LINES         (O_CONTROL_RS | O_WRITE_READ | O_CONTROL_EN)
    #define IO_BUS_PORT_DAT_IN      GPIOC_PDIR

    #define SET_DATA_LINES_INPUT()  _CONFIG_PORT_INPUT(C, LCD_BUS_MASK, PORT_PS_UP_ENABLE)
    #define SET_DATA_LINES_OUTPUT() _DRIVE_PORT_OUTPUT(C, LCD_BUS_MASK)
    #define SET_CONTROL_LINES_OUTPUT(x) _CONFIG_PORT_OUTPUT(C, x, PORT_SRE_SLOW)
    #define SET_BUS_DATA(x)         _WRITE_PORT_MASK(C, x,  LCD_BUS_MASK)
    #define SET_CONTROL_LINES(x)    _WRITE_PORT_MASK(C, x, O_CONTROL_LINES)
    #define O_SET_CONTROL_LOW(x)    _CLEARBITS(C, x); _CLEARBITS(C, x); _CLEARBITS(C, x); _CLEARBITS(C, x); _CLEARBITS(C, x); _CLEARBITS(C, x); _CLEARBITS(C, x); _CLEARBITS(C, x); _SIM_PORTS;
    #define O_SET_CONTROL_HIGH(x)   _SETBITS(C, x);   _SETBITS(C, x);   _SETBITS(C, x);  _SETBITS(C, x); _SETBITS(C, x); _SETBITS(C, x);   _SETBITS(C, x);   _SETBITS(C, x);  _SETBITS(C, x); _SETBITS(C, x); _SIM_PORTS;
#elif defined FRDM_KE02Z
    #if defined LCD_BUS_8BIT
        #error use 4-bit mode with FRDM-KE02Z
    #else
        #define LCD_BUS_MASK        0x0000000f                           // PTA 3,2,1,0
        #define DATA_SHIFT_RIGHT    4                                    // nibble shift down required to bring data into position                                   
        #define DATA_SHIFT_LEFT     0
    #endif

    #define O_CONTROL_RS            KE_PORTB_BIT0
    #define O_WRITE_READ            KE_PORTB_BIT1
    #define O_CONTROL_EN            KE_PORTB_BIT2
    #define O_LCD_BACKLIGHT         KE_PORTB_BIT3

    #define O_CONTROL_LINES         (O_CONTROL_RS | O_WRITE_READ | O_CONTROL_EN)
    #define IO_BUS_PORT_DAT_IN      GPIOA_PDIR

    #define SET_DATA_LINES_INPUT()  _CONFIG_PORT_INPUT(A, LCD_BUS_MASK, PORT_PS_UP_ENABLE)
    #define SET_DATA_LINES_OUTPUT() _DRIVE_PORT_OUTPUT(A, LCD_BUS_MASK)
    #define SET_CONTROL_LINES_OUTPUT(x) _CONFIG_PORT_OUTPUT(A, x, PORT_SRE_SLOW)
    #define SET_BUS_DATA(x)         _WRITE_PORT_MASK(A, x,  LCD_BUS_MASK)
    #define SET_CONTROL_LINES(x)    _WRITE_PORT_MASK(A, x, O_CONTROL_LINES)
    #define O_SET_CONTROL_LOW(x)    _CLEARBITS(A, x); _CLEARBITS(A, x); _CLEARBITS(A, x); _CLEARBITS(A, x); _CLEARBITS(A, x); _SIM_PORTS
    #define O_SET_CONTROL_HIGH(x)   _SETBITS(A, x);   _SETBITS(A, x);   _SETBITS(A, x);   _SETBITS(A, x);   _SETBITS(A, x); _SIM_PORTS
#else
    #if defined LCD_BUS_8BIT
        #define LCD_BUS_MASK        0x7f800000
        #define DATA_SHIFT_RIGHT    0    
        #define DATA_SHIFT_LEFT     23                                   // byte shift down required to bring data into position
    #else
        #define LCD_BUS_MASK        0x0f000000
        #define DATA_SHIFT_RIGHT    0                                   
        #define DATA_SHIFT_LEFT     20                                   // nibble shift down required to bring data into position
    #endif

    #define O_CONTROL_RS            PORTC_BIT16
    #define O_WRITE_READ            PORTC_BIT17
    #define O_CONTROL_EN            PORTC_BIT18
    #define O_LCD_BACKLIGHT         PORTC_BIT19

    #define O_CONTROL_LINES         (O_CONTROL_RS | O_WRITE_READ | O_CONTROL_EN)
    #define IO_BUS_PORT_DAT_IN      GPIOC_PDIR

    #define SET_DATA_LINES_INPUT()  _CONFIG_PORT_INPUT(C, LCD_BUS_MASK, PORT_PS_UP_ENABLE)
    #define SET_DATA_LINES_OUTPUT() _DRIVE_PORT_OUTPUT(C, LCD_BUS_MASK)
    #define SET_CONTROL_LINES_OUTPUT(x) _CONFIG_PORT_OUTPUT(C, x, PORT_SRE_SLOW)
    #define SET_BUS_DATA(x)         _WRITE_PORT_MASK(C, x,  LCD_BUS_MASK)
    #define SET_CONTROL_LINES(x)    _WRITE_PORT_MASK(C, x, O_CONTROL_LINES)
    #define O_SET_CONTROL_LOW(x)    _CLEARBITS(C, x); _CLEARBITS(C, x); _CLEARBITS(C, x); _CLEARBITS(C, x); _CLEARBITS(C, x); _CLEARBITS(C, x); _CLEARBITS(C, x); _CLEARBITS(C, x); _SIM_PORTS;
    #define O_SET_CONTROL_HIGH(x)   _SETBITS(C, x); _SETBITS(C, x); _SETBITS(C, x); _SETBITS(C, x); _SETBITS(C, x); _SETBITS(C, x); _SETBITS(C, x); _SETBITS(C, x); _SIM_PORTS;
#endif

// Drive the control lines R/W + LCD Backlight '1', RS + E '0' and the data lines with all high impedance at start up
// enable clocks to port first
//
#define INITIALISE_LCD_CONTROL_LINES()       SET_DATA_LINES_INPUT(); \
                                             O_SET_CONTROL_LOW(O_CONTROL_LINES); O_SET_CONTROL_HIGH(O_LCD_BACKLIGHT); \
                                             _SIM_PORTS; SET_CONTROL_LINES_OUTPUT(O_CONTROL_LINES | O_LCD_BACKLIGHT);

#define LCD_DRIVE_DATA()       SET_DATA_LINES_OUTPUT();  SET_DATA_LINES_OUTPUT(); SET_DATA_LINES_OUTPUT(); SET_DATA_LINES_OUTPUT(); SET_DATA_LINES_OUTPUT(); SET_DATA_LINES_OUTPUT(); SET_DATA_LINES_OUTPUT(); SET_DATA_LINES_OUTPUT();
                               // ensure data bus outputs (delay) by repetitions according to processor speed

#define CLOCK_EN_HIGH()        O_SET_CONTROL_HIGH(O_CONTROL_EN);
                               // clock EN to high state - repreat to slow down (delay)

#define DELAY_ENABLE_CLOCK_HIGH() O_SET_CONTROL_LOW(O_CONTROL_EN);

// SLCD
//
#if defined SUPPORT_SLCD && (defined TWR_K40X256 || defined TWR_K40D100M || defined TWR_K53N512)
    #if defined TWR_K53N512
        #define CONFIGURE_SLCD()  MCG_C1 |= MCG_C1_IRCLKEN; \
                                  POWER_UP(3, SIM_SCGC3_SLCD); \
                                  LCD_GCR = (LCD_GCR_VSUPPLY_VLL3 | LCD_GCR_SOURCE | LCD_GCR_LCLK_4 | LCD_GCR_DUTY_4BP | LCD_GCR_ALTDIV_NONE); \
                                  LCD_PENL =  (SLCD_PIN_10 | SLCD_PIN_11 | SLCD_PIN_2 | SLCD_PIN_3 | SLCD_PIN_20 | SLCD_PIN_21 | SLCD_PIN_22 | SLCD_PIN_12 | SLCD_PIN_13 | SLCD_PIN_14 | SLCD_PIN_15); \
                                  LCD_PENH =  0x00000000; \
                                  LCD_BPENL = (SLCD_PIN_10 | SLCD_PIN_11 | SLCD_PIN_2 | SLCD_PIN_3); \
                                  LCD_BPENH = 0x00000000; \
                                  fnClearSLCD(); \
                                  WRITE_SLCD(3TO0, 0x08040000); WRITE_SLCD(11TO8, 0x02010000); \
                                  LCD_GCR = (LCD_GCR_LCDEN | LCD_GCR_VSUPPLY_VLL3 | LCD_GCR_SOURCE | LCD_GCR_LCLK_4 | LCD_GCR_DUTY_4BP | LCD_GCR_ALTDIV_NONE)
    #else
        #define CONFIGURE_SLCD()  MCG_C1 |= MCG_C1_IRCLKEN; \
                                  POWER_UP(3, SIM_SCGC3_SLCD); \
                                  LCD_GCR = (LCD_GCR_VSUPPLY_VLL3 | LCD_GCR_SOURCE | LCD_GCR_LCLK_4 | LCD_GCR_DUTY_4BP | LCD_GCR_ALTDIV_NONE); \
                                  LCD_PENL = (SLCD_PIN_0 | SLCD_PIN_1 | SLCD_PIN_2 | SLCD_PIN_3 | SLCD_PIN_20 | SLCD_PIN_21 | SLCD_PIN_22 | SLCD_PIN_12 | SLCD_PIN_13 | SLCD_PIN_14 | SLCD_PIN_15); \
                                  LCD_PENH = 0x00000000; \
                                  LCD_BPENL = (SLCD_PIN_0 | SLCD_PIN_1 | SLCD_PIN_2 | SLCD_PIN_3); \
                                  LCD_BPENH = 0x00000000; \
                                  fnClearSLCD(); \
                                  WRITE_SLCD(3TO0, 0x08040201); \
                                  LCD_GCR = (LCD_GCR_LCDEN | LCD_GCR_VSUPPLY_VLL3 | LCD_GCR_SOURCE | LCD_GCR_LCLK_4 | LCD_GCR_DUTY_4BP | LCD_GCR_ALTDIV_NONE)
    #endif

    #define SLCD_FILE             "SLCD\\TWR_K40.lcd"                    // SLCD simulation file in the simulator directory SLCD
  //#define BIG_PIXEL                                                    // show SLCD double size
    #define LCD_ON_COLOUR         (COLORREF)RGB(210, 220, 210)           // RGB colour of LCD when backlight is on
    #define LCD_OFF_COLOUR        (COLORREF)RGB(10, 10, 10)              // RGB colour of LCD when backlight is off
    #define LCD_PIXEL_COLOUR      (COLORREF)RGB(0,0,0)                   // RGB colour of LCD pixels
    #define GLCD_X  380
    #define GLCD_Y  90
    #define DEL_USB_SYMBOL()      CLEAR_SLCD(23TO20, 0x100)              // control display of USB enumeration - clear
    #define SET_USB_SYMBOL()      SET_SLCD(23TO20, 0x100)                // control display of USB enumeration - set
#elif defined SUPPORT_SLCD && (defined KWIKSTIK || defined KINETIS_K30 || defined KINETIS_K51)
    #define CONFIGURE_SLCD()      MCG_C1 |= MCG_C1_IRCLKEN; \
                                  POWER_UP(3, SIM_SCGC3_SLCD); \
                                  LCD_GCR = (LCD_GCR_CPSEL | LCD_GCR_RVEN | LCD_GCR_RVTRIM_MASK | LCD_GCR_LADJ_MASK | LCD_GCR_LCLK_0 | LCD_GCR_VSUPPLY_VLL3_EXT | LCD_GCR_SOURCE | LCD_GCR_DUTY_8BP | LCD_GCR_ALTDIV_NONE); \
                                  LCD_PENL = 0xfffffffe; \
                                  LCD_PENH = 0x0000ffff; \
                                  LCD_BPENL = 0x00000000; \
                                  LCD_BPENH = 0x0000ff00; \
                                  fnClearSLCD(); \
                                  WRITE_SLCD(43TO40, 0x08040201); \
                                  WRITE_SLCD(47TO44, 0x80402010); \
                                  LCD_GCR = (LCD_GCR_LCDEN | LCD_GCR_CPSEL | LCD_GCR_RVEN | LCD_GCR_RVTRIM_MASK | LCD_GCR_LADJ_MASK | LCD_GCR_LCLK_0 | LCD_GCR_VSUPPLY_VLL3_EXT | LCD_GCR_SOURCE | LCD_GCR_DUTY_8BP | LCD_GCR_ALTDIV_NONE)

    #define SLCD_FILE             "SLCD\\KWIKSTIK.lcd"                   // SLCD simulation file in the simulator directory SLCD
  //#define BIG_PIXEL                                                    // show SLCD double size
    #define LCD_ON_COLOUR         (COLORREF)RGB(210, 220, 210)           // RGB colour of LCD when backlight is on
    #define LCD_OFF_COLOUR        (COLORREF)RGB(10, 10, 10)              // RGB colour of LCD when backlight is off
    #define LCD_PIXEL_COLOUR      (COLORREF)RGB(0,0,0)                   // RGB colour of LCD pixels
    #define GLCD_X  720
    #define GLCD_Y  260
    #define DEL_USB_SYMBOL()      CLEAR_SLCD(3TO0, 0x200)                // disable the USB symbol in SLCD
    #define SET_USB_SYMBOL()      SET_SLCD(3TO0, 0x200)                  // enable USB symbol in SLCD
#elif defined SUPPORT_SLCD && defined FRDM_KL43Z
    #define SLCD_FILE             "SLCD\\FRDM-KL43Z.lcd"                 // SLCD simulation file in the simulator directory SLCD
  //#define BIG_PIXEL                                                    // show SLCD double size
    #define LCD_ON_COLOUR         (COLORREF)RGB(210, 220, 210)           // RGB colour of LCD when backlight is on
    #define LCD_OFF_COLOUR        (COLORREF)RGB(10, 10, 10)              // RGB colour of LCD when backlight is off
    #define LCD_PIXEL_COLOUR      (COLORREF)RGB(0,0,0)                   // RGB colour of LCD pixels
    #define GLCD_X  260
    #define GLCD_Y  95

    // SLCD configuration with clock from MCGIRCLK (2MHz) divided by 64
    //
    #define CONFIGURE_SLCD()      MCG_C1 |= MCG_C1_IRCLKEN; \
                                  POWER_UP(5, SIM_SCGC5_SLCD); \
                                  LCD_GCR = ((0x0b000000 & LCD_GCR_RVTRIM_MASK) | LCD_GCR_CPSEL | LCD_GCR_LADJ_MASK | LCD_GCR_VSUPPLY | LCD_GCR_ALTDIV_64 | LCD_GCR_SOURCE | LCD_GCR_LCLK_1 | LCD_GCR_DUTY_4BP); \
                                  LCD_AR = (LCD_AR_BRATE_MASK & 3); \
                                  LCD_BPENL = (SLCD_PIN_14 | SLCD_PIN_15); \
                                  LCD_BPENH = (SLCD_PIN_H_59 | SLCD_PIN_H_60); \
                                  LCD_PENL = ((SLCD_PIN_20 | SLCD_PIN_24 | SLCD_PIN_26 | SLCD_PIN_27) | (SLCD_PIN_14 | SLCD_PIN_15)); \
                                  LCD_PENH = ((SLCD_PIN_H_40 | SLCD_PIN_H_42 | SLCD_PIN_H_43 | SLCD_PIN_H_44) | (SLCD_PIN_H_59 | SLCD_PIN_H_60)); \
                                  fnClearSLCD(); \
                                  WRITE_SLCD(15TO12, 0x08040000); \
                                  WRITE_SLCD(59TO56, 0x01000000); \
                                  WRITE_SLCD(63TO60, 0x00000002); \
                                  LCD_GCR = (LCD_GCR_LCDEN | (0x0b000000 & LCD_GCR_RVTRIM_MASK) | LCD_GCR_CPSEL | LCD_GCR_LADJ_MASK | LCD_GCR_VSUPPLY | LCD_GCR_ALTDIV_64 | LCD_GCR_SOURCE | LCD_GCR_LCLK_1 | LCD_GCR_DUTY_4BP)
                                   
    // Display "USb" in SLCD
    //
    #define DEL_USB_SYMBOL()      CLEAR_SLCD(27TO24, 0x060b0000); CLEAR_SLCD(43TO40, 0x0f0a000d); CLEAR_SLCD(47TO44, 0x00000002)
    #define SET_USB_SYMBOL()      SET_SLCD(27TO24,   0x060b0000); SET_SLCD(43TO40,   0x0f0a000d); SET_SLCD(47TO44,   0x00000002)
#elif defined SUPPORT_SLCD && defined TWR_KL43Z48M                       // {23}
    #define SLCD_FILE             "SLCD\\TWR_KL46Z48M.lcd"               // SLCD simulation file in the simulator directory SLCD
  //#define BIG_PIXEL                                                    // show SLCD double size
    #define LCD_ON_COLOUR         (COLORREF)RGB(210, 220, 210)           // RGB colour of LCD when backlight is on
    #define LCD_OFF_COLOUR        (COLORREF)RGB(10, 10, 10)              // RGB colour of LCD when backlight is off
    #define LCD_PIXEL_COLOUR      (COLORREF)RGB(0,0,0)                   // RGB colour of LCD pixels
    #define GLCD_X    380
    #define GLCD_Y    90

    // SLCD configuration with clock from MCGIRCLK (2MHz) divided by 64
    //
    #define CONFIGURE_SLCD()      MCG_C1 |= MCG_C1_IRCLKEN; \
                                  POWER_UP(5, SIM_SCGC5_SLCD); \
                                  LCD_GCR = (LCD_GCR_CPSEL | LCD_GCR_LADJ_MASK | LCD_GCR_ALTDIV_256 | LCD_GCR_SOURCE | LCD_GCR_LCLK_1 | LCD_GCR_DUTY_4BP); \
                                  LCD_BPENL = (SLCD_PIN_12 | SLCD_PIN_13 | SLCD_PIN_14 | SLCD_PIN_15); \
                                  LCD_BPENH = 0x00000000; \
                                  LCD_PENL = ((SLCD_PIN_12 | SLCD_PIN_13 | SLCD_PIN_14 | SLCD_PIN_15) | (SLCD_PIN_0 | SLCD_PIN_1 | SLCD_PIN_3 | SLCD_PIN_20 | SLCD_PIN_22 | SLCD_PIN_23 | SLCD_PIN_24)); \
                                  LCD_PENH = 0x00000000; \
                                  fnClearSLCD(); \
                                  WRITE_SLCD(15TO12, 0x08040201); \
                                  LCD_GCR = (LCD_GCR_LCDEN | (LCD_GCR_CPSEL | LCD_GCR_LADJ_MASK | LCD_GCR_ALTDIV_256 | LCD_GCR_SOURCE | LCD_GCR_LCLK_1 | LCD_GCR_DUTY_4BP))

    // Display "USb" in SLCD (when time is not being displayed)
    //
    #if !defined SUPPORT_RTC
        #define DEL_USB_SYMBOL()  CLEAR_SLCD(22, 0x0e); CLEAR_SLCD(24, 0x05); CLEAR_SLCD(20, 0x03); CLEAR_SLCD(3, 0x0d); CLEAR_SLCD(1, 0x07); CLEAR_SLCD(23, 0x0c)
        #define SET_USB_SYMBOL()    SET_SLCD(22, 0x0e);   SET_SLCD(24, 0x05);   SET_SLCD(20, 0x03);   SET_SLCD(3, 0x0d);   SET_SLCD(1, 0x07);   SET_SLCD(23, 0x0c)
    #else
        #define DEL_USB_SYMBOL()                                         // control display of USB enumeration - clear
        #define SET_USB_SYMBOL()                                         // control display of USB enumeration - set
    #endif
#elif defined SUPPORT_SLCD && defined FRDM_KL46Z                         // {22}
    #define SLCD_FILE   "SLCD\\FRDM-KL46Z.lcd"                           // SLCD simulation file in the simulator directory SLCD
  //#define BIG_PIXEL                                                    // show SLCD double size
    #define LCD_ON_COLOUR         (COLORREF)RGB(210, 220, 210)           // RGB colour of LCD when backlight is on
    #define LCD_OFF_COLOUR        (COLORREF)RGB(10, 10, 10)              // RGB colour of LCD when backlight is off
    #define LCD_PIXEL_COLOUR      (COLORREF)RGB(0,0,0)                   // RGB colour of LCD pixels
    #define GLCD_X  260
    #define GLCD_Y  95

    // Use MCGIRCLK (fast IRC) as SLCD source
    //
    #define CONFIGURE_SLCD()      MCG_C1 &= ~(MCG_C1_IREFS); MCG_C1 |= (MCG_C1_IRCLKEN | MCG_C1_IREFSTEN); \
                                  POWER_UP(5, SIM_SCGC5_SLCD); \
                                  LCD_GCR = (LCD_GCR_RVEN | (0x08000000 & LCD_GCR_RVTRIM_MASK) | LCD_GCR_CPSEL | LCD_GCR_LADJ_MASK | LCD_GCR_VSUPPLY | LCD_GCR_SOURCE | LCD_GCR_LCLK_1 | LCD_GCR_DUTY_4BP); \
                                  LCD_AR = (LCD_AR_BRATE_MASK & 3); \
                                  LCD_BPENL = (SLCD_PIN_19 | SLCD_PIN_18); \
                                  LCD_BPENH = (SLCD_PIN_H_52 | SLCD_PIN_H_40); \
                                  LCD_PENL = ((SLCD_PIN_17 | SLCD_PIN_11 | SLCD_PIN_10 | SLCD_PIN_8 | SLCD_PIN_7) | (SLCD_PIN_19 | SLCD_PIN_18)); \
                                  LCD_PENH = ((SLCD_PIN_H_53 | SLCD_PIN_H_38 | SLCD_PIN_H_37) | (SLCD_PIN_H_52 | SLCD_PIN_H_40)); \
                                  fnClearSLCD(); \
                                  WRITE_SLCD(43TO40, 0x00000001); \
                                  WRITE_SLCD(55TO52, 0x00000002); \
                                  WRITE_SLCD(19TO16, 0x04080000); \
                                  LCD_GCR = (LCD_GCR_LCDEN | LCD_GCR_RVEN | (0x08000000 & LCD_GCR_RVTRIM_MASK) | LCD_GCR_CPSEL | LCD_GCR_LADJ_MASK | LCD_GCR_VSUPPLY | LCD_GCR_SOURCE | LCD_GCR_LCLK_1 | LCD_GCR_DUTY_4BP)

    // Display "USb" in SLCD
    //
    #define DEL_USB_SYMBOL()      CLEAR_SLCD(7TO4, 0x0b000000); CLEAR_SLCD(11TO8, 0x00000006); CLEAR_SLCD(39TO36, 0x000a0000); CLEAR_SLCD(55TO52, 0x00000d00); CLEAR_SLCD(11TO8, 0x020f0000)
    #define SET_USB_SYMBOL()      SET_SLCD(7TO4, 0x0b000000);   SET_SLCD(11TO8, 0x00000006);   SET_SLCD(39TO36, 0x000a0000);   SET_SLCD(55TO52, 0x00000d00);   SET_SLCD(11TO8, 0x020f0000)
#elif defined SUPPORT_SLCD && defined TWR_KL46Z48M                       // {23}
    #define SLCD_FILE             "SLCD\\TWR_KL46Z48M.lcd"               // SLCD simulation file in the simulator directory SLCD
  //#define BIG_PIXEL                                                    // show SLCD double size
    #define LCD_ON_COLOUR         (COLORREF)RGB(210, 220, 210)           // RGB colour of LCD when backlight is on
    #define LCD_OFF_COLOUR        (COLORREF)RGB(10, 10, 10)              // RGB colour of LCD when backlight is off
    #define LCD_PIXEL_COLOUR      (COLORREF)RGB(0,0,0)                   // RGB colour of LCD pixels
    #define GLCD_X  380
    #define GLCD_Y  90

    #define CONFIGURE_SLCD()      MCG_C1 |= MCG_C1_IRCLKEN; \
                                  POWER_UP(5, SIM_SCGC5_SLCD); \
                                  LCD_GCR = (LCD_GCR_VSUPPLY | LCD_GCR_SOURCE | LCD_GCR_LCLK_4 | LCD_GCR_DUTY_4BP | LCD_GCR_ALTDIV_NONE); \
                                  LCD_BPENL = (SLCD_PIN_12 | SLCD_PIN_13 | SLCD_PIN_14 | SLCD_PIN_15); \
                                  LCD_BPENH = 0x00000000; \
                                  LCD_PENL = ((SLCD_PIN_12 | SLCD_PIN_13 | SLCD_PIN_14 | SLCD_PIN_15) | (SLCD_PIN_0 | SLCD_PIN_1 | SLCD_PIN_3 | SLCD_PIN_20 | SLCD_PIN_22 | SLCD_PIN_23 | SLCD_PIN_24)); \
                                  LCD_PENH = 0x00000000; \
                                  fnClearSLCD(); \
                                  WRITE_SLCD(15TO12, 0x08040201); \
                                  LCD_GCR = (LCD_GCR_LCDEN | LCD_GCR_VSUPPLY | LCD_GCR_SOURCE | LCD_GCR_LCLK_4 | LCD_GCR_DUTY_4BP | LCD_GCR_ALTDIV_NONE)

    // Display "USb" in SLCD (when time is not being displayed)
    //
    #if !defined SUPPORT_RTC
        #define DEL_USB_SYMBOL()   CLEAR_SLCD(22, 0x0e); CLEAR_SLCD(24, 0x05); CLEAR_SLCD(20, 0x03); CLEAR_SLCD(3, 0x0d); CLEAR_SLCD(1, 0x07); CLEAR_SLCD(23, 0x0c)
        #define SET_USB_SYMBOL()     SET_SLCD(22, 0x0e);   SET_SLCD(24, 0x05);   SET_SLCD(20, 0x03);   SET_SLCD(3, 0x0d);   SET_SLCD(1, 0x07);   SET_SLCD(23, 0x0c)
    #else
        #define DEL_USB_SYMBOL()                                         // control display of USB enumeration - clear
        #define SET_USB_SYMBOL()                                         // control display of USB enumeration - set
    #endif
#else
    #define DEL_USB_SYMBOL()                                             // control display of USB enumeration - clear
    #define SET_USB_SYMBOL()                                             // control display of USB enumeration - set
#endif

// Graphic LCD
//
#if defined NET_K60	                                                     // {16} optional ST7565S 128x64 GLCD via Flexbus 
    #define GLCD_BACKLIGHT_CONTROL                                       // (requires SUPPORT_PWM_MODULE)

    #define GLCD_COMMAND_ADDR  0x60400000
    #define GLCD_DATA_ADDR     0x60408000                                // AD15 is the LCD A0 signal

    #define CONFIGURE_GLCD()   _CONFIG_PERIPHERAL(C, 8,  (PC_8_FB_AD7   | PORT_DSE_HIGH | PORT_PS_DOWN_ENABLE)); \
                               _CONFIG_PERIPHERAL(C, 9,  (PC_9_FB_AD6   | PORT_DSE_HIGH | PORT_PS_DOWN_ENABLE)); \
                               _CONFIG_PERIPHERAL(C, 10, (PC_10_FB_AD5  | PORT_DSE_HIGH | PORT_PS_DOWN_ENABLE)); \
                               _CONFIG_PERIPHERAL(D, 2,  (PD_2_FB_AD4   | PORT_DSE_HIGH | PORT_PS_DOWN_ENABLE)); \
                               _CONFIG_PERIPHERAL(D, 3,  (PD_3_FB_AD3   | PORT_DSE_HIGH | PORT_PS_DOWN_ENABLE)); \
                               _CONFIG_PERIPHERAL(D, 4,  (PD_4_FB_AD2   | PORT_DSE_HIGH | PORT_PS_DOWN_ENABLE)); \
                               _CONFIG_PERIPHERAL(D, 5,  (PD_5_FB_AD1   | PORT_DSE_HIGH | PORT_PS_DOWN_ENABLE)); \
                               _CONFIG_PERIPHERAL(D, 6,  (PD_6_FB_AD0   | PORT_DSE_HIGH | PORT_PS_DOWN_ENABLE)); \
                               _CONFIG_PERIPHERAL(D, 1,  (PD_1_FB_CS0   | PORT_DSE_HIGH | PORT_PS_DOWN_ENABLE)); \
                               _CONFIG_PERIPHERAL(C, 11, (PC_11_FB_RW   | PORT_DSE_HIGH | PORT_PS_DOWN_ENABLE)); \
                               _CONFIG_PERIPHERAL(B, 19, (PB_19_FB_OE   | PORT_DSE_HIGH | PORT_PS_DOWN_ENABLE)); \
                               _CONFIG_PERIPHERAL(B, 18, (PB_18_FB_AD15 | PORT_DSE_HIGH | PORT_PS_DOWN_ENABLE)); \
                               CSAR0  = GLCD_COMMAND_ADDR; \
                               CSMR0  = (0x00010000 | FB_CS_VALID); \
                               CSCR0  = (FB_BLS | FB_AA | PORT_SIZE_8);
                             //BACK_LIGHT_MIN_INTENSITY();

    #define GLCD_RST_L()
    #define GLCD_RST_H()
    #define SET_PULL_DOWNS()
    #define REMOVE_PULL_DOWNS()

    #define ENABLE_BACKLIGHT()

    #define _GLCD_BACKLIGHT_TIMER         2                              // PWM channel 2 (TC)
    #define _GLCD_TIMER_MODE_OF_OPERATION (PWM_PRESCALER_128 | PWM_SYS_CLK) // set clock source and prescaler
    #define _GLCD_BACKLIGHT_PWM_FREQUENCY (unsigned char)(PWM_US_DELAY(PWM_FREQUENCY_VALUE(1000))/128) // 2KHz PWM

    #define PWM_US_DELAY(usec)            ((BUS_CLOCK/1000000) * usec)
    #define PWM_FREQUENCY_VALUE(hertz)    (1000000 / (hertz))

    #define BACK_LIGHT_MAX_INTENSITY()    _CONFIG_DRIVE_PORT_OUTPUT_VALUE(C, PORTC_BIT3, PORTC_BIT3, PORT_SRE_SLOW)
    #define BACK_LIGHT_MIN_INTENSITY()    _CONFIG_DRIVE_PORT_OUTPUT_VALUE(C, PORTC_BIT3, 0, PORT_SRE_SLOW)

    #if defined _WINDOWS
        #define MAX_GLCD_WRITE_BURST   10000                             // the maximum number of writes to the GLCD before the task yields
    #else
        #define MAX_GLCD_WRITE_BURST   1024                              // the maximum number of writes to the GLCD before the task yields
    #endif
#elif defined NET_KBED	                                                 // {16} optional ST7565S 128x64 GLCD via SPI 
    #define CONFIGURE_GLCD()   POWER_UP(6, SIM_SCGC6_SPI1);\
                               _CONFIG_PERIPHERAL(B, 10, (PB_10_SPI1_PCS0 | PORT_SRE_FAST | PORT_DSE_HIGH));\
                               _CONFIG_PERIPHERAL(B, 11, (PB_11_SPI1_SCK | PORT_SRE_FAST | PORT_DSE_HIGH));\
                               _CONFIG_PERIPHERAL(B, 16, (PB_16_SPI1_SOUT | PORT_SRE_FAST | PORT_DSE_HIGH));\
                               _CONFIG_PERIPHERAL(B, 17, PB_17_SPI1_SIN);\
                               _CONFIG_DRIVE_PORT_OUTPUT_VALUE(E, (PORTE_BIT3), (PORTE_BIT3), (PORT_SRE_SLOW | PORT_DSE_HIGH));\
                               _SETBITS(E, PORTE_BIT3);\
                               SPI1_MCR = (SPI_MCR_MSTR | SPI_MCR_DCONF_SPI | SPI_MCR_CLR_RXF | SPI_MCR_CLR_TXF | SPI_MCR_PCSIS_CS0 | SPI_MCR_PCSIS_CS1 | SPI_MCR_PCSIS_CS2 | SPI_MCR_PCSIS_CS3 | SPI_MCR_PCSIS_CS4 | SPI_MCR_PCSIS_CS5);\
                               SPI1_CTAR0 = (SPI_CTAR_DBR | SPI_CTAR_FMSZ_8 | SPI_CTAR_PDT_7 | SPI_CTAR_BR_4 | SPI_CTAR_CPHA | SPI_CTAR_CPOL); // for 50MHz bus, 12.5MHz speed and 140ns min de-select time

    #define POWER_DOWN_SPI_LCD_INTERFACE()  POWER_DOWN(6, SIM_SCGC6_SPI1) // power down SPI interface

    #define FLUSH_SPI1_FIFO_AND_FLAGS()     SPI1_MCR |= SPI_MCR_CLR_RXF; SPI1_SR = (SPI_SR_EOQF | SPI_SR_TFUF | SPI_SR_TFFF | SPI_SR_RFOF | SPI_SR_RFDF | SPI_SR_TCF);

    #define WRITE_SPI_LCD_CMD(byte)         FLUSH_SPI1_FIFO_AND_FLAGS();\
                                            _CLEARBITS(E, PORTE_BIT3);\
                                            SPI1_PUSHR = (byte | SPI_PUSHR_EOQ  | SPI_PUSHR_PCS0 | SPI_PUSHR_CTAS_CTAR0);\
                                            while ((SPI1_SR & SPI_SR_TCF) == 0 {});\
                                            _SETBITS(E, PORTE_BIT3)      // write final byte to output FIFO - this will negate the CS line when complete

    #define WRITE_SPI_LCD_DATA(byte)        FLUSH_SPI1_FIFO_AND_FLAGS();\
                                            SPI1_PUSHR = (byte | SPI_PUSHR_EOQ  | SPI_PUSHR_PCS0 | SPI_PUSHR_CTAS_CTAR0);\
                                            while ((SPI1_SR & SPI_SR_TCF) == 0) {}
 
    #define MAX_GLCD_WRITE_BURST      1024                               // the maximum number of writes to the GLCD before the task yields
    #define GLCD_RST_L()
    #define GLCD_RST_H()
    #define SET_PULL_DOWNS()
    #define REMOVE_PULL_DOWNS()
#elif defined rcARM_KL26	                                                // ST7565S 128x64 GLCD via SPI
    #define SPI_GLCD_CS        PORTD_BIT4                                   // the chip select line controlling the GLCD
    #define SPI_GLCD_A0        PORTE_BIT1                                   // the A0 line, used to control data and commands
    #define SPI_GLCD_RESET     PORTE_BIT0                                   // the reset line to the GLCD
    #define CONFIGURE_GLCD()   POWER_UP(4, SIM_SCGC4_SPI0);\
                               _CONFIG_PERIPHERAL(D, 1, (PD_1_SPI0_SCK | PORT_SRE_FAST | PORT_DSE_HIGH));\
                               _CONFIG_PERIPHERAL(D, 2, (PD_2_SPI0_SOUT | PORT_SRE_FAST | PORT_DSE_HIGH));\
                               _CONFIG_PERIPHERAL(D, 3, PD_3_SPI0_SIN);\
                               _CONFIG_DRIVE_PORT_OUTPUT_VALUE(D, (SPI_GLCD_CS), (SPI_GLCD_CS), (PORT_SRE_FAST | PORT_DSE_HIGH));\
                               _CONFIG_DRIVE_PORT_OUTPUT_VALUE(E, (SPI_GLCD_RESET | SPI_GLCD_A0), (SPI_GLCD_A0), (PORT_SRE_SLOW | PORT_DSE_HIGH));\
                               SPI0_C1 = (SPI_C1_CPHA | SPI_C1_CPOL | SPI_C1_MSTR | SPI_C1_SPE); \
                               SPI0_BR = (SPI_BR_SPPR_PRE_4 | SPI_BR_SPR_DIV_2); \
                               (void)SPI0_S; (void)SPI0_D

    #define POWER_DOWN_SPI_LCD_INTERFACE()  POWER_DOWN(4, SIM_SCGC4_SPI0)// power down SPI interface

    #define WRITE_SPI_LCD_DATA(byte)        _CLEARBITS(D, SPI_GLCD_CS);\
                                            SPI0_D = (byte);\
                                            while ((SPI0_S & (SPI_S_SPRF)) == 0) {}\
                                            (void)SPI0_D;\
                                            _SETBITS(D, SPI_GLCD_CS)

    #define WRITE_SPI_LCD_CMD(byte)         _CLEARBITS(E, SPI_GLCD_A0);\
                                            WRITE_SPI_LCD_DATA(byte);\
                                            _SETBITS(E, SPI_GLCD_A0)

    #define MAX_GLCD_WRITE_BURST      512                                // the maximum number of writes to the GLCD before the task yields
    #define GLCD_RST_L()              _CLEARBITS(E, SPI_GLCD_RESET)
    #define GLCD_RST_H()              _SETBITS(E, SPI_GLCD_RESET)
    #define SET_PULL_DOWNS()
    #define REMOVE_PULL_DOWNS()
#elif defined TWR_LCD_RGB_GLCD_MODE                                      // TWR-LCD-RGB
    #if defined _WINDOWS
        #define MAX_GLCD_WRITE_BURST   10000                             // the maximum number of writes to the GLCD before the task yields
    #else
        #define MAX_GLCD_WRITE_BURST   1000                              // the maximum number of writes to the GLCD before the task yields
    #endif
#elif defined BLAZE_K22                                                  // BLAZE watch-format 240 x 240 color display
    #if defined _WINDOWS
        static unsigned char _ucCommand = 0;
        static unsigned char _ucData = 0;
        #define GLCD_COMMAND_ADDR     &_ucCommand
        #define GLCD_DATA_ADDR        &_ucData
    #else
        #define GLCD_COMMAND_ADDR     0x60000000                         // write address
        #define GLCD_DATA_ADDR        0x60010000                         // read address
    #endif
    #define GLCD_RESET_LINE           (PORTC_BIT11)
    #define GLCD_BACK_LIGHT           (PORTA_BIT5)
    #define GLCD_CHIP_SELECT          (PORTB_BIT18)
    #define GLCD_RS                   (PORTB_BIT17)
    #define GLCD_TE                   (PORTB_BIT19)

    #define BACK_LIGHT_MAX_INTENSITY()
    #define BACK_LIGHT_MIN_INTENSITY()

    #define ENABLE_BACKLIGHT()        BACK_LIGHT_MAX_INTENSITY()
    // BLAZE uses 8 bit FlexBus interface using RS, RD and WR. The address range is set to 128K because the DC signal is connected on address wire. FlexBus setup as fast as possible in multiplexed mode
    // the 8 bit data appears at AD0..AD7
    //
    #define CONFIGURE_GLCD()        _CONFIG_DRIVE_PORT_OUTPUT_VALUE_FAST_LOW(C, GLCD_RESET_LINE, 0, (PORT_SRE_SLOW | PORT_DSE_LOW)); \
                                    _CONFIG_DRIVE_PORT_OUTPUT_VALUE_FAST_LOW(A, GLCD_BACK_LIGHT, 0, (PORT_SRE_SLOW | PORT_DSE_LOW)); \
                                    _CONFIG_DRIVE_PORT_OUTPUT_VALUE(B, (GLCD_CHIP_SELECT | GLCD_RS | TSI_RESET_LINE), (GLCD_CHIP_SELECT | GLCD_RS), (PORT_SRE_FAST | PORT_DSE_LOW)); \
                                    _CONFIG_PORT_INPUT_FAST_HIGH(B, GLCD_TE, PORT_PS_UP_ENABLE); \
                                    _CONFIG_PERIPHERAL(C, 8,  (PC_8_FB_AD7   | PORT_DSE_HIGH | PORT_PS_DOWN_ENABLE)); \
                                    _CONFIG_PERIPHERAL(C, 9,  (PC_9_FB_AD6   | PORT_DSE_HIGH | PORT_PS_DOWN_ENABLE)); \
                                    _CONFIG_PERIPHERAL(C, 10, (PC_10_FB_AD5  | PORT_DSE_HIGH | PORT_PS_DOWN_ENABLE)); \
                                    _CONFIG_PERIPHERAL(D, 2,  (PD_2_FB_AD4   | PORT_DSE_HIGH | PORT_PS_DOWN_ENABLE)); \
                                    _CONFIG_PERIPHERAL(D, 3,  (PD_3_FB_AD3   | PORT_DSE_HIGH | PORT_PS_DOWN_ENABLE)); \
                                    _CONFIG_PERIPHERAL(D, 4,  (PD_4_FB_AD2   | PORT_DSE_HIGH | PORT_PS_DOWN_ENABLE)); \
                                    _CONFIG_PERIPHERAL(D, 5,  (PD_5_FB_AD1   | PORT_DSE_HIGH | PORT_PS_DOWN_ENABLE)); \
                                    _CONFIG_PERIPHERAL(D, 6,  (PD_6_FB_AD0   | PORT_DSE_HIGH | PORT_PS_DOWN_ENABLE)); \
                                    _CONFIG_PERIPHERAL(D, 1,  (PD_1_FB_CS0   | PORT_DSE_HIGH | PORT_PS_DOWN_ENABLE)); \
                                    _CONFIG_PERIPHERAL(D, 0,  (PD_0_FB_ALE   | PORT_DSE_HIGH | PORT_PS_DOWN_ENABLE)); \
                                    CSAR0  = (unsigned long)GLCD_COMMAND_ADDR; \
  	                                CSMR0  = (FB_CS_VALID); \
  	                                CSCR0  = (FB_BLS | FB_AA | PORT_SIZE_8); \
                                    BACK_LIGHT_MIN_INTENSITY();

    #define GLCD_RST_H()            _SETBITS(C, GLCD_RESET_LINE)
    #define ASSERT_RS()             _CLEARBITS(B, GLCD_RS)
    #define ASSERT_CS()             _CLEARBITS(B, GLCD_CHIP_SELECT)
    #define NEGATE_CS()             _SETBITS(B, GLCD_CHIP_SELECT)
    #define NEGATE_RS()             _SETBITS(B, GLCD_RS)
    #if defined _WINDOWS
        #define MAX_GLCD_WRITE_BURST   10000                             // the maximum number of writes to the GLCD before the task yields
    #else
        #define MAX_GLCD_WRITE_BURST   1000                              // the maximum number of writes to the GLCD before the task yields
    #endif

// temp
#define GLCD_RST_L()
#define SET_PULL_DOWNS()
#define REMOVE_PULL_DOWNS()

  //#define GLCD_BACKLIGHT_CONTROL                                       // (requires SUPPORT_PWM_MODULE)
  //#define TEMP_HUM_TEST
  //#define VARIABLE_PIXEL_COLOUR                                        // support control of background and pixel colors

    #define _GLCD_BACKLIGHT_TIMER         6                              // PWM channel 6 (TC)
    #define _GLCD_TIMER_MODE_OF_OPERATION (PWM_PRESCALER_128 | PWM_SCALED_CLOCK_INPUT | PWM_POLARITY)
    #define _GLCD_BACKLIGHT_PWM_FREQUENCY (unsigned char)(PWM_US_DELAY(PWM_FREQUENCY_VALUE(2000 * 128)))
#elif defined TFT2N0369_GLCD_MODE                                        // TWR Kit TFT display via FlexBus
    #define GLCD_COMMAND_ADDR         0x60400000
    #define GLCD_DATA_ADDR            0x60410000

    #define BACK_LIGHT_MAX_INTENSITY()    //PTCPAR &= ~(TC_PWM6_FUNCTION); _CONFIG_DRIVE_PORT_OUTPUT_VALUE(TC, PORT_TC_BIT3, PORT_TC_BIT3)
    #define BACK_LIGHT_MIN_INTENSITY()    //PTCPAR &= ~(TC_PWM6_FUNCTION); _CONFIG_DRIVE_PORT_OUTPUT_VALUE(TC, PORT_TC_BIT3, 0)

    #define ENABLE_BACKLIGHT()        BACK_LIGHT_MAX_INTENSITY()
    // Tower kit uses 16 bit FlexBus interface on CS0. The address range is set to 128K because the DC signal is connected on address wire. FlexBus setup as fast as possible in multiplexed mode
    // the 16 bit data appears at AD0..AD15 when the BLS bit is set (rather than AD16..AD31)
    //
    #if defined TWR_K40X256 || defined TWR_K40D100M || defined TWR_K53N512
        #define CONFIGURE_GLCD()      _CONFIG_PERIPHERAL(A, 9,  (PA_9_FB_AD16  | PORT_DSE_HIGH | PORT_PS_DOWN_ENABLE)); \
                                      _CONFIG_PERIPHERAL(A, 10, (PA_10_FB_AD15 | PORT_DSE_HIGH | PORT_PS_DOWN_ENABLE)); \
                                      _CONFIG_PERIPHERAL(A, 24, (PA_24_FB_AD14 | PORT_DSE_HIGH | PORT_PS_DOWN_ENABLE)); \
                                      _CONFIG_PERIPHERAL(A, 25, (PA_25_FB_AD13 | PORT_DSE_HIGH | PORT_PS_DOWN_ENABLE)); \
                                      _CONFIG_PERIPHERAL(A, 26, (PA_26_FB_AD12 | PORT_DSE_HIGH | PORT_PS_DOWN_ENABLE)); \
                                      _CONFIG_PERIPHERAL(A, 27, (PA_27_FB_AD11 | PORT_DSE_HIGH | PORT_PS_DOWN_ENABLE)); \
                                      _CONFIG_PERIPHERAL(A, 28, (PA_28_FB_AD10 | PORT_DSE_HIGH | PORT_PS_DOWN_ENABLE)); \
                                      _CONFIG_PERIPHERAL(D, 10, (PD_10_FB_AD9  | PORT_DSE_HIGH | PORT_PS_DOWN_ENABLE)); \
                                      _CONFIG_PERIPHERAL(D, 11, (PD_11_FB_AD8  | PORT_DSE_HIGH | PORT_PS_DOWN_ENABLE)); \
                                      _CONFIG_PERIPHERAL(D, 12, (PD_12_FB_AD7  | PORT_DSE_HIGH | PORT_PS_DOWN_ENABLE)); \
                                      _CONFIG_PERIPHERAL(D, 13, (PD_13_FB_AD6  | PORT_DSE_HIGH | PORT_PS_DOWN_ENABLE)); \
                                      _CONFIG_PERIPHERAL(D, 14, (PD_14_FB_AD5  | PORT_DSE_HIGH | PORT_PS_DOWN_ENABLE)); \
                                      _CONFIG_PERIPHERAL(E, 8,  (PE_8_FB_AD4   | PORT_DSE_HIGH | PORT_PS_DOWN_ENABLE)); \
                                      _CONFIG_PERIPHERAL(E, 9,  (PE_9_FB_AD3   | PORT_DSE_HIGH | PORT_PS_DOWN_ENABLE)); \
                                      _CONFIG_PERIPHERAL(E, 10, (PE_10_FB_AD2  | PORT_DSE_HIGH | PORT_PS_DOWN_ENABLE)); \
                                      _CONFIG_PERIPHERAL(E, 11, (PE_11_FB_AD1  | PORT_DSE_HIGH | PORT_PS_DOWN_ENABLE)); \
                                      _CONFIG_PERIPHERAL(E, 12, (PE_12_FB_AD0  | PORT_DSE_HIGH | PORT_PS_DOWN_ENABLE)); \
                                      _CONFIG_PERIPHERAL(E, 7,  (PE_7_FB_CS0   | PORT_DSE_HIGH | PORT_PS_DOWN_ENABLE)); \
                                      _CONFIG_PERIPHERAL(D, 15, (PD_15_FB_RW   | PORT_DSE_HIGH | PORT_PS_DOWN_ENABLE)); \
                                      CSAR0  = GLCD_COMMAND_ADDR; \
  	                                  CSMR0  = (0x00010000 | FB_CS_VALID); \
  	                                  CSCR0  = (FB_BLS | FB_AA | PORT_SIZE_16); \
                                      BACK_LIGHT_MIN_INTENSITY();
    #else
        #define CONFIGURE_GLCD()      _CONFIG_PERIPHERAL(B, 17, (PB_17_FB_AD16 | PORT_DSE_HIGH | PORT_PS_DOWN_ENABLE)); \
                                      _CONFIG_PERIPHERAL(B, 18, (PB_18_FB_AD15 | PORT_DSE_HIGH | PORT_PS_DOWN_ENABLE)); \
                                      _CONFIG_PERIPHERAL(C, 0,  (PC_0_FB_AD14  | PORT_DSE_HIGH | PORT_PS_DOWN_ENABLE)); \
                                      _CONFIG_PERIPHERAL(C, 1,  (PC_1_FB_AD13  | PORT_DSE_HIGH | PORT_PS_DOWN_ENABLE)); \
                                      _CONFIG_PERIPHERAL(C, 2,  (PC_2_FB_AD12  | PORT_DSE_HIGH | PORT_PS_DOWN_ENABLE)); \
                                      _CONFIG_PERIPHERAL(C, 4,  (PC_4_FB_AD11  | PORT_DSE_HIGH | PORT_PS_DOWN_ENABLE)); \
                                      _CONFIG_PERIPHERAL(C, 5,  (PC_5_FB_AD10  | PORT_DSE_HIGH | PORT_PS_DOWN_ENABLE)); \
                                      _CONFIG_PERIPHERAL(C, 6,  (PC_6_FB_AD9   | PORT_DSE_HIGH | PORT_PS_DOWN_ENABLE)); \
                                      _CONFIG_PERIPHERAL(C, 7,  (PC_7_FB_AD8   | PORT_DSE_HIGH | PORT_PS_DOWN_ENABLE)); \
                                      _CONFIG_PERIPHERAL(C, 8,  (PC_8_FB_AD7   | PORT_DSE_HIGH | PORT_PS_DOWN_ENABLE)); \
                                      _CONFIG_PERIPHERAL(C, 9,  (PC_9_FB_AD6   | PORT_DSE_HIGH | PORT_PS_DOWN_ENABLE)); \
                                      _CONFIG_PERIPHERAL(C, 10, (PC_10_FB_AD5  | PORT_DSE_HIGH | PORT_PS_DOWN_ENABLE)); \
                                      _CONFIG_PERIPHERAL(D, 2,  (PD_2_FB_AD4   | PORT_DSE_HIGH | PORT_PS_DOWN_ENABLE)); \
                                      _CONFIG_PERIPHERAL(D, 3,  (PD_3_FB_AD3   | PORT_DSE_HIGH | PORT_PS_DOWN_ENABLE)); \
                                      _CONFIG_PERIPHERAL(D, 4,  (PD_4_FB_AD2   | PORT_DSE_HIGH | PORT_PS_DOWN_ENABLE)); \
                                      _CONFIG_PERIPHERAL(D, 5,  (PD_5_FB_AD1   | PORT_DSE_HIGH | PORT_PS_DOWN_ENABLE)); \
                                      _CONFIG_PERIPHERAL(D, 6,  (PD_6_FB_AD0   | PORT_DSE_HIGH | PORT_PS_DOWN_ENABLE)); \
                                      _CONFIG_PERIPHERAL(D, 1,  (PD_1_FB_CS0   | PORT_DSE_HIGH | PORT_PS_DOWN_ENABLE)); \
                                      _CONFIG_PERIPHERAL(C, 11, (PC_11_FB_RW   | PORT_DSE_HIGH | PORT_PS_DOWN_ENABLE)); \
                                      CSAR0  = GLCD_COMMAND_ADDR; \
  	                                  CSMR0  = (0x00010000 | FB_CS_VALID); \
  	                                  CSCR0  = (FB_BLS | FB_AA | PORT_SIZE_16); \
                                      BACK_LIGHT_MIN_INTENSITY();
    #endif

    #define GLCD_RST_H()
    #if defined _WINDOWS
        #define MAX_GLCD_WRITE_BURST   10000                             // the maximum number of writes to the GLCD before the task yields
    #else
        #define MAX_GLCD_WRITE_BURST   1000                              // the maximum number of writes to the GLCD before the task yields
    #endif

  //#define GLCD_BACKLIGHT_CONTROL                                       // (requires SUPPORT_PWM_MODULE)
  //#define TEMP_HUM_TEST
  //#define VARIABLE_PIXEL_COLOUR                                        // support control of background and pixel colors

    #define _GLCD_BACKLIGHT_TIMER         6                              // PWM channel 6 (TC)
    #define _GLCD_TIMER_MODE_OF_OPERATION (PWM_PRESCALER_128 | PWM_SCALED_CLOCK_INPUT | PWM_POLARITY)
    #define _GLCD_BACKLIGHT_PWM_FREQUENCY (unsigned char)(PWM_US_DELAY(PWM_FREQUENCY_VALUE(2000 * 128)))
#endif


// Keypad
//
#if defined KEY_COLUMNS && KEY_COLUMNS > 0                               // matrix keypad
    #if defined NET_K60
        #define KEY_ROW_IN_1           PORTD_BIT7
        #define KEY_ROW_IN_PORT_1      GPIOD_PDIR
        #define KEY_ROW_IN_PORT_1_REF  _PORTD

        #define KEY_ROW_IN_2           PORTD_BIT8
        #define KEY_ROW_IN_PORT_2      GPIOD_PDIR
        #define KEY_ROW_IN_PORT_2_REF  _PORTD

        #define KEY_ROW_IN_3           PORTD_BIT9
        #define KEY_ROW_IN_PORT_3      GPIOD_PDIR
        #define KEY_ROW_IN_PORT_3_REF  _PORTD

        #define KEY_ROW_IN_4           PORTD_BIT10
        #define KEY_ROW_IN_PORT_4      GPIOD_PDIR
        #define KEY_ROW_IN_PORT_4_REF  _PORTD

        #define KEY_COL_OUT_1          PORTC_BIT18
        #define KEY_COL_OUT_PORT_1     (~GPIOC_PDOR)                     // invert since there is a hardware inversion (only used by simulator)
        #define KEY_COL_OUT_DDR_1      GPIOC_PDDR

        #define KEY_COL_OUT_2          PORTC_BIT19
        #define KEY_COL_OUT_PORT_2     (~GPIOC_PDOR)                     // invert since there is a hardware inversion (only used by simulator)
        #define KEY_COL_OUT_DDR_2      GPIOC_PDDR
 
        // Drive each column low (inverted in hardware)
        //
        #define DRIVE_COLUMN_1()       _SETBITS(C, KEY_COL_OUT_1)        // drive output CO1 high (column 1) - this will drive a '0' COL1
	    #define DRIVE_COLUMN_2()       _SETBITS(C, KEY_COL_OUT_2)        // drive output CO2 high (column 2) - this will drive a '0' COL2

        // Drive COL high (inverted in hardware)
	    //
        #define RELEASE_COLUMN_1()     _CLEARBITS(C, KEY_COL_OUT_1); fnDelayLoop(5)
        #define RELEASE_COLUMN_2()     _CLEARBITS(C, KEY_COL_OUT_2)
 
        // Key scan initialisation
        //
        #define INIT_KEY_SCAN()        _CONFIG_PORT_OUTPUT(C, (KEY_COL_OUT_1 | KEY_COL_OUT_2), (PORT_SRE_SLOW)); \
                                       _CONFIG_PORT_INPUT(D, (KEY_ROW_IN_1 | KEY_ROW_IN_2 | KEY_ROW_IN_3 | KEY_ROW_IN_4), (PORT_SRE_SLOW)); \
                                       RESET_SCAN()

        // Reset any changes ready for next scan sequence
        //
        #define RESET_SCAN()           _CLEARBITS(C, (KEY_COL_OUT_1 | KEY_COL_OUT_2)); // prepare outputs low for next time
    #else
        #define KEY_ROW_IN_1           PORTB_BIT20
        #define KEY_ROW_IN_PORT_1      GPIOB_PDIR
        #define KEY_ROW_IN_PORT_1_REF  _PORTB

        #define KEY_ROW_IN_2           PORTB_BIT21
        #define KEY_ROW_IN_PORT_2      GPIOB_PDIR
        #define KEY_ROW_IN_PORT_2_REF  _PORTB

        #define KEY_ROW_IN_3           PORTB_BIT22
        #define KEY_ROW_IN_PORT_3      GPIOB_PDIR
        #define KEY_ROW_IN_PORT_3_REF  _PORTB

        #define KEY_ROW_IN_4           PORTB_BIT23
        #define KEY_ROW_IN_PORT_4      GPIOB_PDIR
        #define KEY_ROW_IN_PORT_4_REF  _PORTB

        #define KEY_COL_OUT_1          PORTA_BIT24
        #define KEY_COL_OUT_PORT_1     GPIOA_PDOR                        // {18}
        #define KEY_COL_OUT_DDR_1      GPIOA_PDDR

        #define KEY_COL_OUT_2          PORTA_BIT25
        #define KEY_COL_OUT_PORT_2     GPIOA_PDOR                        // {18}
        #define KEY_COL_OUT_DDR_2      GPIOA_PDDR

        #define KEY_COL_OUT_3          PORTA_BIT26
        #define KEY_COL_OUT_PORT_3     GPIOA_PDOR                        // {18}
        #define KEY_COL_OUT_DDR_3      GPIOA_PDDR

        #define KEY_COL_OUT_4          PORTA_BIT27
        #define KEY_COL_OUT_PORT_4     GPIOA_PDOR                        // {18}
        #define KEY_COL_OUT_DDR_4      GPIOA_PDDR

        // Drive each column low
        //
        #define DRIVE_COLUMN_1()       _DRIVE_PORT_OUTPUT(A, KEY_COL_OUT_1) // drive output low (column 1) - this will drive a '0' since the output has been prepared
        #define DRIVE_COLUMN_2()       _DRIVE_PORT_OUTPUT(A, KEY_COL_OUT_2) // drive output low (column 2) - this will drive a '0' since the output has been prepared
        #define DRIVE_COLUMN_3()       _DRIVE_PORT_OUTPUT(A, KEY_COL_OUT_3) // drive output low (column 3) - this will drive a '0' since the output has been prepared
        #define DRIVE_COLUMN_4()       _DRIVE_PORT_OUTPUT(A, KEY_COL_OUT_4) // drive output low (column 4) - this will drive a '0' since the output has been prepared

        // Drive high (to avoid slow rise time) then set back as input
        //
        #define RELEASE_COLUMN_1()     _SETBITS(A, KEY_COL_OUT_1); _FLOAT_PORT(A, KEY_COL_OUT_1)
        #define RELEASE_COLUMN_2()     _SETBITS(A, KEY_COL_OUT_2); _FLOAT_PORT(A, KEY_COL_OUT_2)
        #define RELEASE_COLUMN_3()     _SETBITS(A, KEY_COL_OUT_3); _FLOAT_PORT(A, KEY_COL_OUT_3)
        #define RELEASE_COLUMN_4()     _SETBITS(A, KEY_COL_OUT_4); _FLOAT_PORT(A, KEY_COL_OUT_4)

        // Reset any changes ready for next scan sequence
        //
        #define RESET_SCAN()           _CLEARBITS(A, (KEY_COL_OUT_1 | KEY_COL_OUT_2 | KEY_COL_OUT_3 | KEY_COL_OUT_4)); // prepare outputs low for next time

        // Key scan initialisation
        //
        #define INIT_KEY_SCAN()        _CONFIG_PORT_INPUT(A, (KEY_COL_OUT_1 | KEY_COL_OUT_2 | KEY_COL_OUT_3 | KEY_COL_OUT_4), (PORT_PS_UP_ENABLE)); \
                                       _CONFIG_PORT_INPUT(B, (KEY_ROW_IN_1 | KEY_ROW_IN_2 | KEY_ROW_IN_3 | KEY_ROW_IN_4), (PORT_SRE_SLOW)); \
                                       RESET_SCAN()

        // LEDs
        //
                                        // '0'            '1'     input state  center (x,   y)   0 = circle, radius, controlling port, controlling pin 
        #define KEYPAD_LED_DEFINITIONS {RGB(215,215,215), RGB(255,0,0), 0, {265, 20,  0,   7 },   _PORTA, DEMO_LED_1}, \
                                       {RGB(215,215,215), RGB(0,255,0), 0, {265, 88,  0,   7 },   _PORTD, USER_PORT_1_BIT}, \
                                       {RGB(215,215,215), RGB(0,0,255), 0, {265, 156, 0,   7 },   _PORTD, USER_PORT_2_BIT}, \
                                       {RGB(215,215,215), RGB(255,0,0), 0, {260, 216, 270, 226 }, _PORTD, USER_PORT_3_BIT}
    #endif

#elif defined KEY_COLUMNS && KEY_COLUMNS == 0                            // linear keyboard (key connected directly to inputs)
    #define TOUCH_SENSOR_INPUTS                                          // {3} use touch sensor inputs
    #if defined TOUCH_SENSOR_INPUTS
        #if defined TWR_K20D50M || defined TWR_K20D72M
            #define KEY_ROW_IN_1        PORTB_BIT0
            #define KEY_ROW_IN_2        PORTB_BIT1
            #define KEY_ROW_IN_3        0
            #define KEY_ROW_IN_4        0

            #define KEY_1_PORT_REF      _TOUCH_PORTB
            #define KEY_1_PORT_PIN      KEY_ROW_IN_1

            #define KEY_2_PORT_REF      _TOUCH_PORTB
            #define KEY_2_PORT_PIN      KEY_ROW_IN_2

            #define KEY_3_PORT_REF      _TOUCH_PORTB
            #define KEY_3_PORT_PIN      0

            #define KEY_4_PORT_REF      _TOUCH_PORTB
            #define KEY_4_PORT_PIN      0

            #define INIT_KEY_STATE      0x00000000

            #define READ_KEY_INPUTS()   fnReadTouchSensorInputs()

                                                                         // power up the touch sense input module; select TSI channels on the ports
                                                                         // enable TSI channels
            #define INIT_KEY_SCAN()     POWER_UP(5, SIM_SCGC5_TSI); _CONFIG_PERIPHERAL(B, 0,  PB_0_TSI_CH0); _CONFIG_PERIPHERAL(B, 1,  PB_1_TSI_CH6); \
                                        TSI0_PEN = (TSI_PEN_PEN0 | TSI_PEN_PEN6); \
                                        TSI0_SCANC = (TSI_SCANC_SMOD_CONTINUOUS | TSI_SCANC_AMCLKS_BUS_CLK | TSI_SCANC_AMPSC_128); \
                                        TSI0_GENCS = (TSI_GENCS_STM_SW_TRIG | TSI_GENCS_SWTS | TSI_GENCS_TSIEN | TSI_GENCS_PS_32);
        #elif defined FRDM_KL26Z || defined FRDM_KL25Z || defined FRDM_KL03Z
            #define KEY_ROW_IN_1        PORTB_BIT16
            #define KEY_ROW_IN_2        PORTB_BIT17

            #define KEY_1_PORT_REF      _TOUCH_PORTB
            #define KEY_1_PORT_PIN      KEY_ROW_IN_1

            #define KEY_2_PORT_REF      _TOUCH_PORTB
            #define KEY_2_PORT_PIN      KEY_ROW_IN_2

            #define KEY_3_PORT_REF      _TOUCH_PORTB
            #define KEY_3_PORT_PIN      0

            #define INIT_KEY_STATE      0x00000000

            #define READ_KEY_INPUTS()   fnReadTouchSensorInputs()

                                                                         // power up the touch sense input module; select TSI channels on the ports and prepare operating mode; start first measurement
            #define INIT_KEY_SCAN()     POWER_UP(5, SIM_SCGC5_TSI); _CONFIG_PERIPHERAL(B, 16,  PB_16_TSI_CH9); _CONFIG_PERIPHERAL(B, 17,  PB_17_TSI_CH10); \
                                        TSI0_GENCS = (TSI_GENCS_EOSF | TSI_GENCS_STM_SW | TSI_GENCS_NSCN_32 | TSI_GENCS_TSIEN | TSI_GENCS_PS_DIV_1 | TSI_GENCS_EXTCHRG_32uA | TSI_GENCS_DVOLT_1_03 | TSI_GENCS_REFCHRG_32uA | TSI_GENCS_MODE_CAPACITIVE); \
                                        TSI0_DATA = (TSI0_DATA_SWTS | TSI0_DATA_TSICH_9)
            #define FIRST_TSI_INPUT     TSI0_DATA_TSICH_9                // alternate between channels 9 and 10
            #define SECOND_TSI_INPUT    TSI0_DATA_TSICH_10
        #elif defined rcARM_KL26
            #define KEY_ROW_IN_1        PORTB_BIT16
            #define KEY_ROW_IN_2        PORTB_BIT17
            #define KEY_ROW_IN_3        PORTB_BIT18

            #define KEY_1_PORT_REF      _TOUCH_PORTB
            #define KEY_1_PORT_PIN      KEY_ROW_IN_1

            #define KEY_2_PORT_REF      _TOUCH_PORTB
            #define KEY_2_PORT_PIN      KEY_ROW_IN_2

            #define KEY_3_PORT_REF      _TOUCH_PORTB
            #define KEY_3_PORT_PIN      KEY_ROW_IN_3

            #define INIT_KEY_STATE      0x00000000

            #define READ_KEY_INPUTS()   fnReadTouchSensorInputs()

                                                                         // power up the touch sense input module; select TSI channels on the ports and prepare operating mode; start first measurement
            #define INIT_KEY_SCAN()     POWER_UP(5, SIM_SCGC5_TSI); _CONFIG_PERIPHERAL(B, 16,  PB_16_TSI_CH9); _CONFIG_PERIPHERAL(B, 17,  PB_17_TSI_CH10);  _CONFIG_PERIPHERAL(B, 18,  PB_18_TSI_CH11); \
                                        TSI0_GENCS = (TSI_GENCS_EOSF | TSI_GENCS_STM_SW | TSI_GENCS_NSCN_32 | TSI_GENCS_TSIEN | TSI_GENCS_PS_DIV_1 | TSI_GENCS_EXTCHRG_32uA | TSI_GENCS_DVOLT_1_03 | TSI_GENCS_REFCHRG_32uA | TSI_GENCS_MODE_CAPACITIVE); \
                                        TSI0_DATA = (TSI0_DATA_SWTS | TSI0_DATA_TSICH_9)
            #define FIRST_TSI_INPUT     TSI0_DATA_TSICH_9                // alternate between channels 9, 10 and 11
            #define SECOND_TSI_INPUT    TSI0_DATA_TSICH_10
            #define THIRD_TSI_INPUT     TSI0_DATA_TSICH_11
        #else
            #define KEY_ROW_IN_1        PORTA_BIT4
            #define KEY_ROW_IN_2        PORTB_BIT3
            #define KEY_ROW_IN_3        PORTB_BIT2
            #define KEY_ROW_IN_4        PORTB_BIT16

            #define KEY_1_PORT_REF      _TOUCH_PORTA
            #define KEY_1_PORT_PIN      KEY_ROW_IN_1

            #define KEY_2_PORT_REF      _TOUCH_PORTB
            #define KEY_2_PORT_PIN      KEY_ROW_IN_2

            #define KEY_3_PORT_REF      _TOUCH_PORTB
            #define KEY_3_PORT_PIN      KEY_ROW_IN_3

            #define KEY_4_PORT_REF      _TOUCH_PORTB
            #define KEY_4_PORT_PIN      KEY_ROW_IN_4

            #define INIT_KEY_STATE      0x00000000

            #define READ_KEY_INPUTS()   fnReadTouchSensorInputs()

                                                                         // power up the touch sense input module; select TSI channels on the ports
                                                                         // enable TSI channels
            #define INIT_KEY_SCAN()     POWER_UP(5, SIM_SCGC5_TSI); _CONFIG_PERIPHERAL(B, 3,  PB_3_TSI_CH8); _CONFIG_PERIPHERAL(A, 4,  PA_4_TSI_CH5); _CONFIG_PERIPHERAL(B, 2,  PB_2_TSI_CH7); _CONFIG_PERIPHERAL(B, 16,  PB_16_TSI_CH9); \
                                        TSI0_PEN = (TSI_PEN_PEN5 | TSI_PEN_PEN7 | TSI_PEN_PEN8 | TSI_PEN_PEN9); \
                                        TSI0_SCANC = (TSI_SCANC_SMOD_CONTINUOUS | TSI_SCANC_AMCLKS_BUS_CLK | TSI_SCANC_AMPSC_128); \
                                        TSI0_GENCS = (TSI_GENCS_STM_SW_TRIG | TSI_GENCS_SWTS | TSI_GENCS_TSIEN | TSI_GENCS_PS_32);
        #endif
    #else
        #define KEY_ROW_IN_1        PORTB_BIT20
        #define KEY_ROW_IN_2        PORTB_BIT21
        #define KEY_ROW_IN_3        PORTB_BIT22
        #define KEY_ROW_IN_4        PORTB_BIT23

        #define KEY_1_PORT_REF      _PORTB
        #define KEY_1_PORT_PIN      KEY_ROW_IN_1

        #define KEY_2_PORT_REF      _PORTB
        #define KEY_2_PORT_PIN      KEY_ROW_IN_2

        #define KEY_3_PORT_REF      _PORTB
        #define KEY_3_PORT_PIN      KEY_ROW_IN_3

        #define KEY_4_PORT_REF      _PORTB
        #define KEY_4_PORT_PIN      KEY_ROW_IN_4

        #define INIT_KEY_STATE      0x0000000f

        #define READ_KEY_INPUTS()   (_READ_PORT_MASK(B, (KEY_ROW_IN_1 | KEY_ROW_IN_2 | KEY_ROW_IN_3 | KEY_ROW_IN_4)) >> 20)

        #define INIT_KEY_SCAN()     _CONFIG_PORT_INPUT(B, (KEY_ROW_IN_1 | KEY_ROW_IN_2 | KEY_ROW_IN_3 | KEY_ROW_IN_4), (PORT_PS_UP_ENABLE));
    #endif

    // LEDs
    //
    #if !defined TWR_K20D50M && !defined TWR_K20D72M && !defined FRDM_K64F && !defined FRDM_KL26Z && !defined FRDM_KL25Z && !defined FRDM_KL03Z && !defined FreeLON
                                        // '0'            '1'     input state   center (x,   y)   0 = circle, radius, controlling port, controlling pin 
        #define KEYPAD_LED_DEFINITIONS  {RGB(200,200,200),RGB(255,75,0),  0, {340, 123, 348, 131 }, _PORTA, DEMO_LED_1}, \
                                        {RGB(200,200,200),RGB(255,255,0), 0, {340, 160, 348, 168 }, _PORTA, DEMO_LED_2}, \
                                        {RGB(200,200,200),RGB(0,255,0),   0, {340, 195, 348, 203 }, _PORTA, DEMO_LED_3}, \
                                        {RGB(200,200,200),RGB(255,0,0),   0, {340, 231, 348, 239 }, _PORTA, DEMO_LED_4}


                                        // column, row, key press rectangle
        #define KEYPAD_KEY_DEFINITIONS  {0, 0, {317, 114, 372, 140}}, \
                                        {0, 1, {317, 152, 372, 175}}, \
                                        {0, 2, {317, 186, 372, 212}}, \
                                        {0, 3, {317, 222, 372, 249}} 
    #endif
#elif !defined NET_KBED
    // LEDs
    //
    #if defined EMCRAFT_K70F120M || defined EMCRAFT_K61F150M           // {9}
                                        // '0'            '1'     input state   center (x,   y)   0 = circle, radius, controlling port, controlling pin 
        #define KEYPAD_LED_DEFINITIONS  {RGB(200,200,200),RGB(0,255,0), 0, {368, 223, 376, 236 }, _PORTE, DEMO_LED_1}, \
                                        {RGB(200,200,200),RGB(0,255,0), 0, {368, 242, 376, 254 }, _PORTE, DEMO_LED_2}
    #elif !defined KEYPAD_LED_DEFINITIONS && !defined KL43Z_256_32_CL
                                        // '0'            '1'     input state   center (x,   y)   0 = circle, radius, controlling port, controlling pin 
        #define KEYPAD_LED_DEFINITIONS  {RGB(200,200,200),RGB(255,75,0), 0, {340, 123, 348, 131 }, _PORTA, DEMO_LED_1}, \
                                        {RGB(200,200,200),RGB(255,255,0),0, {340, 160, 348, 168 }, _PORTA, DEMO_LED_2}, \
                                        {RGB(200,200,200),RGB(0,255,0),  0, {340, 195, 348, 203 }, _PORTA, DEMO_LED_3}, \
                                        {RGB(200,200,200),RGB(255,0,0),  0, {340, 231, 348, 239 }, _PORTA, DEMO_LED_4}
    #endif
#endif

//#define BUZZER_SOUND                                                   // enable buzzer sound generation in the simulator (needs Winmm.lib added to the simulator linker setting)

#define SENDERS_EMAIL_ADDRESS       "kinetis@uTasker.com"                // fictional Email address of the board being used
#define EMAIL_SUBJECT               "kinetis Test"                       // email subject
#define EMAIL_CONTENT               "Hello!!\r\nThis is an email message from the kinetis demo.\r\nI hope that you have received this test and have fun using the uTasker operating system with integrated TCP/IP stack.\r\r\nRegards your Kinetis demo!!";

// SMTP account details if not in parameters
//
#define SMTP_ACCOUNT_NAME           "User Name";
#define SMTP_PASSWORD               "Password";

// Internal HTML message pages
//
#define SUPPORT_INTERNAL_HTML_FILES                                      // enable the use of the following files
#define UPLOAD_FAILED         "<html><head><title>SW Upload failed</title></head><body bgcolor=#ff9000 text=#000000 topmargin=3 marginheight=3><center><td valign=top class=h><font color=#ff0000 style=font-size:30px><b style='mso-bidi-font-weight:normal'>&micro;Tasker</font></b></td><br></td><td align=left><br><br>Sorry but upload has failed.</font><br><br><a href=""javascript:history.back()"">Return</a></body></html>"
#define SW_UPLOAD_COMPLETED   "<html><head><meta http-equiv=""refresh"" content=""5;URL=0Menu.htm""><title>KINETIS SW Uploaded</title></head><body bgcolor=#ffffff text=#000000 topmargin=3 marginheight=3><center><td valign=top class=h><font color=#ff0000 style=font-size:30px><b style='mso-bidi-font-weight:normal'>&micro;Tasker</font> - SW Update</b></td><br></td><td align=left><br><br>SW Upload successful. The KINETIS target will now reset and start the new program. Please wait 5s...</body></html>"


#if defined USE_TFTP
    #define TFTP_ERROR_MESSAGE     "uTasker TFTP Error"                  // fixed TFTP error test
#endif

#if defined USE_IP || defined USB_TO_ETHERNET
    #define LAN_BUFFER_SIZE        1514                                  // Kinetis has ample space for full IP payload (if VLAN is enabled the size is automatically adjusted to allow tags)
#else
    #define LAN_BUFFER_SIZE        256                                   // if using Ethernet without IP the buffer size can be set here
#endif

#if defined KBED_FPGA
    #define FPGA_ADDR              0x61000000

    #define CONFIGURE_FPGA()      _CONFIG_PERIPHERAL(C, 8,  (PC_8_FB_AD7   | PORT_DSE_HIGH | PORT_PS_DOWN_ENABLE)); \
        _CONFIG_PERIPHERAL(C, 9,  (PC_9_FB_AD6   | PORT_DSE_HIGH | PORT_PS_DOWN_ENABLE)); \
        _CONFIG_PERIPHERAL(C, 10, (PC_10_FB_AD5  | PORT_DSE_HIGH | PORT_PS_DOWN_ENABLE)); \
        _CONFIG_PERIPHERAL(D, 2,  (PD_2_FB_AD4   | PORT_DSE_HIGH | PORT_PS_DOWN_ENABLE)); \
        _CONFIG_PERIPHERAL(D, 3,  (PD_3_FB_AD3   | PORT_DSE_HIGH | PORT_PS_DOWN_ENABLE)); \
        _CONFIG_PERIPHERAL(D, 4,  (PD_4_FB_AD2   | PORT_DSE_HIGH | PORT_PS_DOWN_ENABLE)); \
        _CONFIG_PERIPHERAL(D, 5,  (PD_5_FB_AD1   | PORT_DSE_HIGH | PORT_PS_DOWN_ENABLE)); \
        _CONFIG_PERIPHERAL(D, 6,  (PD_6_FB_AD0   | PORT_DSE_HIGH | PORT_PS_DOWN_ENABLE)); \
        _CONFIG_PERIPHERAL(D, 1,  (PD_1_FB_CS0   | PORT_DSE_HIGH | PORT_PS_DOWN_ENABLE)); \
        _CONFIG_PERIPHERAL(C, 11, (PC_11_FB_RW   | PORT_DSE_HIGH | PORT_PS_DOWN_ENABLE)); \
        _CONFIG_PERIPHERAL(D,  0, (PD_0_FB_ALE   | PORT_DSE_HIGH | PORT_PS_DOWN_ENABLE)); \
        _CONFIG_PERIPHERAL(B, 19, (PB_19_FB_OE   | PORT_DSE_HIGH | PORT_PS_DOWN_ENABLE)); \
        _CONFIG_PERIPHERAL(C,  3, (PC_3_FB_CLKO  | PORT_DSE_HIGH | PORT_PS_DOWN_ENABLE)); \
        CSAR0  = FPGA_ADDR; \
        CSMR0  = (0x00010000 | FB_CS_VALID); \
        CSCR0  = (FB_BLS | FB_AA | PORT_SIZE_8); 

    #define fnFPGA_IO_0_7(data)            *((volatile unsigned char*)FPGA_ADDR + 0x00) = (data)
    #define fnFPGA_IO_8_15(data)           *((volatile unsigned char*)FPGA_ADDR + 0x01) = (data)
    #define fnFPGA_IO_16_23(data)          *((volatile unsigned char*)FPGA_ADDR + 0x02) = (data)
    #define fnFPGA_IO_24_31(data)          *((volatile unsigned char*)FPGA_ADDR + 0x03) = (data)

    #define fnFPGA_IODC_0_7(data)          *((volatile unsigned char*)FPGA_ADDR + 0x80) = (data)
    #define fnFPGA_IODC_8_15(data)         *((volatile unsigned char*)FPGA_ADDR + 0x81) = (data)
    #define fnFPGA_IODC_16_23(data)        *((volatile unsigned char*)FPGA_ADDR + 0x82) = (data)
    #define fnFPGA_IODC_24_31(data)        *((volatile unsigned char*)FPGA_ADDR + 0x83) = (data)

    #define fnFPGA_IOFC_0_7(data)          *((volatile unsigned char*)FPGA_ADDR + 0x84) = (data)
    #define fnFPGA_IOFC_8_15(data)         *((volatile unsigned char*)FPGA_ADDR + 0x85) = (data)
    #define fnFPGA_IOFC_16_23(data)        *((volatile unsigned char*)FPGA_ADDR + 0x86) = (data)
    #define fnFPGA_IOFC_24_31(data)        *((volatile unsigned char*)FPGA_ADDR + 0x87) = (data)

    #define fnFPGA_PWM0_L(data)            *((volatile unsigned char*)FPGA_ADDR + 0x10) = (data)
    #define fnFPGA_PWM1_L(data)            *((volatile unsigned char*)FPGA_ADDR + 0x12) = (data)
    #define fnFPGA_PWM2_L(data)            *((volatile unsigned char*)FPGA_ADDR + 0x14) = (data)
    #define fnFPGA_PWM3_L(data)            *((volatile unsigned char*)FPGA_ADDR + 0x16) = (data)
    #define fnFPGA_PWM4_L(data)            *((volatile unsigned char*)FPGA_ADDR + 0x18) = (data)
    #define fnFPGA_PWM5_L(data)            *((volatile unsigned char*)FPGA_ADDR + 0x1a) = (data)
    #define fnFPGA_PWM6_L(data)            *((volatile unsigned char*)FPGA_ADDR + 0x1c) = (data)
    #define fnFPGA_PWM7_L(data)            *((volatile unsigned char*)FPGA_ADDR + 0x1e) = (data)

    #define fnFPGA_SETBITS_16_23(data)     *((volatile unsigned char*)FPGA_ADDR + 0x06) = (data)
    #define fnFPGA_CLEARBITS_16_23(data)   *((volatile unsigned char*)FPGA_ADDR + 0x0a) = (data)
    #define fnFPGA_TOGGLEBITS_16_23(data)  *((volatile unsigned char*)FPGA_ADDR + 0x0e) = (data)

    #define fnFPGA_ID_INDEX(data)          *((volatile unsigned char*)FPGA_ADDR + 0xff) = (data)
    #define fnFPGA_RD_ID()        	       *((volatile unsigned char*)FPGA_ADDR + 0xff)

    #define fnFPGA_RD(addr)                *((volatile unsigned char*)FPGA_ADDR + addr)
#endif
#endif                                                                   // end of file
