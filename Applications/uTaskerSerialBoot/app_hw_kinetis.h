/***********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher

    ---------------------------------------------------------------------
    File:      app_hw_kinetis.h
    Project:   uTasker serial loader
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2016
    *********************************************************************
    22.01.2012 Add SD card loader support to Kwikstik
    03.03.2012 Add TWR_K70F120M and TWR_K53N512 support
    20.04.2012 Add TWR_K20D50M
    26.10.2012 Change READ_SPI_DATA() to use volatile access (avoid loss due to optimisation) {1}
    18.12.2013 Use SD card detect input to force boot loader mode        {2}
    18.12.2013 Use UART for debug messages when SD card loader is used together with the UART loader {3}
    16.01.2014 Add FRDM_K20D50M                                          {4}
    25.01.2014 Add FRDM_KL46Z                                            {5}
    28.01.2014 Add TWR_KL46Z48M                                          {6}
    29.01.2014 Add FRDM_KL25Z, FRDM_KL26Z and TWR_KL25Z48M               {7}
    01.02.2014 Add FRDM_KL02Z, FRDM_KL05Z and FRDM_KE02Z                 {8}
    24.02.2014 Add LED state when driving pin is an input to all configurations
    15.04.2014 Add FRDM_K64F and TWR_K64F120M                            {9}
    15.04.2014 Add web server based software upload support              {10}
    12.07.2014 Add FRDM_KE02Z40M, FRDM_KE04Z and FRDM_KE06Z              {11}
    04.11.2014 Add FRDM_K22F, FRDM_KL03Z and TWR_K22F120M
    04.12.2014 Add FRDM_KL43Z, TWR_KL43Z48M and TWR_KM34Z50M
    21.01.2015 Disable SYSTICK_CSR when jumping to the application since it may use alternative TICK source
    09.05.2015 Add TRK_KEA128, TRK_KEA64
    06.01.2016 Add TWR_K80F150M and FRDM_K82F
    29.07.2017 Add FRDM_K66F
    03.12.2017 Added TRK_KEA64, TRK_KEA128, FRDM_KEAZN32Q64, FRDM_KEAZ64Q64 and FRDM_KEAZ128Q80

    Application specific hardware configuration

*/

#if defined _KINETIS && !defined __APP_HW_KINETIS__
#define __APP_HW_KINETIS__

#if defined KINETIS_K_FPU || defined K02F100M || defined TWR_K20D50M || defined tinyK20 || defined FRDM_K20D50M || defined TWR_K21D50M || defined KINETIS_KL || defined KINETIS_KE || defined KINETIS_K64 || defined FRDM_K22F || defined KINETIS_KV || defined KINETIS_KW2X // newer devices have these errate solved
    #define ERRATA_E2583_SOLVED                                          // in early silicon the CAN controllers only work when the OSC is enabled (enable if the chip revision used doesn't suffer from the problem)
    #define ERRATA_E2644_SOLVED                                          // early devices without flex memory doesn't support speculation logic and this should be disabled
    #define ERRATE_E2647_SOLVED                                          // early 512k and 384k flash-only devices don't support cache aliasing and this needs to be disabled
    #define ERRATE_E2448_SOLVED                                          // early devices can have a flash pre-fetch problem when writing the SIM_DIV1 register during the clock change sequence
    #define ERRATE_E3402_SOLVED                                          // early devices cannot use the XTAL pin as GPIO when the ERCLKEN bit is enabled (simulator blocks its operation in this case)
    #if !defined FRDM_K64F && !defined TWR_K64F120M
        #define LAN_TX_FPU_WORKAROUND                                    // when ERRATE_E2647_SOLVED is enabled it is found that the Ethernet Tx can miss a waiting frame in an output buffer. This workaround enables the transmit frame interrupt, which retriggers buffer polling
    #endif
    #define ERRATE_E2776_SOLVED                                          // write accesses to the CRC should always be 32-bit when transpose is enabled
#else                                                                    // when using older devices they can be set accoring to the revision used
  //#define ERRATA_E2583_SOLVED                                          // in early silicon the CAN controllers only work when the OSC is enabled (enable if the chip revision used doesn't suffer from the problem)
  //#define ERRATA_E2644_SOLVED                                          // early devices without flex memory doesn't support speculation logic and this should be disabled
  //#define ERRATE_E2647_SOLVED                                          // early 512k and 384k flash-only devices don't support cache aliasing and this needs to be disabled
  //#define ERRATE_E2448_SOLVED                                          // early devices can have a flash pre-fetch problem when writing the SIM_DIV1 register during the clock change sequence
  //#define ERRATE_E3402_SOLVED                                          // early devices cannot use the XTAL pin as GPIO when the ERCLKEN bit is enabled (simulator blocks its operation in this case)
#endif

// Define clock settings
//
#if defined FRDM_K64F || defined TWR_K64F120M  || defined TEENSY_3_5   // {9}
    #define MASK_1N83J
  //#define RUN_FROM_DEFAULT_CLOCK                                       // default mode is FLL Engaged Internal - the 32kHz IRC is multiplied by FLL factor of 640 to obtain 20.9715MHz nominal frequency (20MHz..25MHz)
  //#define RUN_FROM_HIRC                                                // clock directly from internal 48MHz RC clock
  //#define RUN_FROM_HIRC_PLL                                            // use 48MHz RC clock as input to the PLL
  //#define RUN_FROM_HIRC_FLL                                            // use 48MHz RC clock as input to the FLL
  //#define RUN_FROM_RTC_FLL                                             // use 32.76kHz crystal clock as input to the FLL
    #if defined RUN_FROM_DEFAULT_CLOCK
      //#define FLL_FACTOR           2929                                // use FLL (factors available are 640, 732, 1280, 1464, 1920, 2197, 2560 and 2929)
        #define FLEX_CLOCK_DIVIDE    1                                   // approx. 20.9715MHz
        #define FLASH_CLOCK_DIVIDE   1                                   // approx. 20.9715MHz
        #define BUS_CLOCK_DIVIDE     1                                   // approx. 20.9715MHz
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
  //#define USB_CRYSTAL_LESS                                             // use 48MHz IRC as USB source (according to Freescale AN4905 - only possible in device mode)
  //#define USB_CLOCK_GENERATED_INTERNALLY                               // use USB clock from internal source rather than external pin - 120MHz is suitable from PLL
    #define SUPPORT_SWAP_BLOCK                                           // support flash swap block
#elif defined TWR_K65F180M || defined K26FN2_180 || defined FRDM_K66F || defined TEENSY_3_6 || defined FRDM_K28F
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
        #if defined FRDM_K66F || defined FRDM_KL82Z || defined TWR_KL82Z72M || defined FRDM_K28F
            #define CRYSTAL_FREQUENCY    12000000                        // 12 MHz crystal
            #define CLOCK_DIV            1                               // input must be divided to 8MHz..16MHz range (/1 to /8)
        #else
            #define CRYSTAL_FREQUENCY    16000000                        // 16 MHz crystal
            #define CLOCK_DIV            2                               // input must be divided to 8MHz..16MHz range (/1 to /8)
        #endif
        #define OSC_LOW_GAIN_MODE
        #define _EXTERNAL_CLOCK      CRYSTAL_FREQUENCY
      //#define USE_HIGH_SPEED_RUN_MODE                                  // note that flash programmin is not possible in high speed run mode and so it is not used by the serial loader
        #if defined USE_HIGH_SPEED_RUN_MODE
            #if defined FRDM_KL82Z || defined TWR_KL82Z72M
                #define CLOCK_MUL        16                              // the PLL multiplication factor to achieve operating frequency of 96MHz (x16 to x47 possible) [PLL output range 90..180MHz - VCO is PLL * 2]
            #elif defined FRDM_K66F || defined TWR_K65F180M || defined K66FX1M0 || defined FRDM_K28F
                #define CLOCK_MUL        30                              // the PLL multiplication factor to achieve operating frequency of 180MHz (x16 to x47 possible) [PLL output range 90..180MHz - VCO is PLL * 2]
            #else
                #define CLOCK_MUL        45                              // the PLL multiplication factor to achieve operating frequency of 180MHz (x16 to x47 possible) [PLL output range 90..180MHz - VCO is PLL * 2]
            #endif
            #define BUS_CLOCK_DIVIDE     3                               // 176/3 to give 58.67MHz (max. 60MHz)
            #define FLEX_CLOCK_DIVIDE    3                               // 176/3 to give 58.67MHz (max. 60MHz)
            #define FLASH_CLOCK_DIVIDE   7                               // 176/7 to give 25.14MHz (max. 28MHz)
        #else
            #if defined FRDM_KL82Z || defined TWR_KL82Z72M || defined FRDM_K28F
                #define CLOCK_MUL        24                              // the PLL multiplication factor to achieve operating frequency of 144MHz (x16 to x47 possible) [PLL output range 90..180MHz - VCO is PLL * 2]
            #elif defined TEENSY_3_6 || defined TWR_K65F180M || defined K66FX1M0
                #define CLOCK_MUL        30                              // the PLL multiplication factor to achieve operating frequency of 120MHz (x16 to x47 possible) [PLL output range 90..180MHz - VCO is PLL * 2]
            #else
                #define CLOCK_MUL        20                              // the PLL multiplication factor to achieve operating frequency of 120MHz (x16 to x47 possible) [PLL output range 90..180MHz - VCO is PLL * 2]
            #endif
            #if defined FRDM_KL82Z || defined TWR_KL82Z72M
                #define SYSTEM_CLOCK_DIVIDE  2                           // 144/2 to give 72MHz
                #define BUS_CLOCK_DIVIDE     6                           // 144/6 to give 24MHz (max. 24MHz)
                #define QSPI_CLOCK_DIVIDE    2                           // 144/2 to give 72MHz (max. 72MHz)
                #define FLASH_CLOCK_DIVIDE   6                           // 144/6 to give 24MHz (max. 24MHz)
            #elif defined FRDM_K28F
                #define BUS_CLOCK_DIVIDE     2                           // 150/3 to give 50MHz
                #define FLEX_CLOCK_DIVIDE    3                           // 150/3 to give 50MHz
                #define FLASH_CLOCK_DIVIDE   6                           // 150/6 to give 25MHz
            #else
                #define BUS_CLOCK_DIVIDE     2                           // 120/2 to give 60MHz (max. 60MHz)
                #define FLEX_CLOCK_DIVIDE    2                           // 120/2 to give 60MHz (max. 60MHz)
                #define FLASH_CLOCK_DIVIDE   5                           // 120/7 to give 24MHz (max. 28MHz)
            #endif 
        #endif
    #endif
    #define USB_CRYSTAL_LESS                                             // use 48MHz IRC as USB source (according to Freescale AN4905 - only possible in device mode)
  //#define USB_CLOCK_GENERATED_INTERNALLY                               // use USB clock from internal source rather than external pin - 120MHz is suitable from PLL
#elif defined TWR_KV31F120M
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
    #define USB_CRYSTAL_LESS                                             // use 48MHz IRC as USB source (according to Freescale AN4905 - only possibel in device mode)
  //#define USB_CLOCK_GENERATED_INTERNALLY                               // use USB clock from internal source rather than external pin - 120MHz is suitable
  //#define USB_CLOCK_SOURCE_MCGPLL0CLK                                  // the clock source for the USB clock
#elif defined FRDM_K22F || defined TWR_K22F120M || defined tinyK22
  //#define RUN_FROM_DEFAULT_CLOCK                                       // default mode is FLL Engaged Internal - the 32kHz IRC is multiplied by FLL factor of 640 to obtain 20.9715MHz nominal frequency (20MHz..25MHz)
  //#define RUN_FROM_HIRC                                                // clock directly from internal 48MHz RC clock
    #if !defined RUN_FROM_DEFAULT_CLOCK
        #define OSC_LOW_GAIN_MODE
        #define CRYSTAL_FREQUENCY    16000000                             // 8 MHz crystal
        #define _EXTERNAL_CLOCK      CRYSTAL_FREQUENCY
      //#define USE_HIGH_SPEED_RUN_MODE
        #if defined RUN_FROM_HIRC                                        // use IRC48M internal oscillator directly (no PLL or FLL)
            #define FLEX_CLOCK_DIVIDE    2                               // approx. 24MHz
            #define FLASH_CLOCK_DIVIDE   2                               // approx. 24MHz 
            #define BUS_CLOCK_DIVIDE     1                               // approx. 48MHz
        #elif defined USE_HIGH_SPEED_RUN_MODE                            // 120 MHz requires use of the high speed run mode (with restriction of not being able to program flash in that mode)
            #define CLOCK_DIV            2                               // input must be divided to 2MHz..4MHz range (/1 to /24)
            #define CLOCK_MUL            30                              // the PLL multiplication factor to achieve operating frequency of 120MHz (x24 to x55 possible) - > 80MHz requires high speed run mode, which doesn't allow flash programming
            #define FLEX_CLOCK_DIVIDE    3                               // 120/3 to give 40MHz
            #define FLASH_CLOCK_DIVIDE   5                               // 120/5 to give 24MHz
            #define BUS_CLOCK_DIVIDE     2                               // 120/2 to give 60MHz
        #else                                                            // faster run mode operation of 80MHz
            #define CLOCK_DIV            8                               // input must be divided to 2MHz..4MHz range (/1 to /24)
            #define CLOCK_MUL            40                              // the PLL multiplication factor to achieve operating frequency of 80MHz (x24 to x55 possible) - uses normal run mode and can program flash
            #define FLASH_CLOCK_DIVIDE   3                               // 80/3 to give 26.7MHz
            #define FLEX_CLOCK_DIVIDE    4                               // 80/3 to give 20MHz
            #define BUS_CLOCK_DIVIDE     2                               // 80/2 to give 40MHz
        #endif
    #else
        #define FLEX_CLOCK_DIVIDE    1                                   // no dividers required since the default speed is low enough
        #define FLASH_CLOCK_DIVIDE   1                                   // 
        #define BUS_CLOCK_DIVIDE     1                                   // 
    #endif
    #define USB_CRYSTAL_LESS                                             // use 48MHz IRC as USB source (according to Freescale AN4905 - only possible in device mode)
  //#define USB_CLOCK_GENERATED_INTERNALLY                               // use USB clock from internal source rather than external pin - 120MHz is suitable
  //#define USB_CLOCK_SOURCE_MCGPLL0CLK                                  // the clock source for the USB clock
#elif defined K70F150M_12M
    #define CRYSTAL_FREQUENCY    12000000                                // 12 MHz crystal
    #define _EXTERNAL_CLOCK      CRYSTAL_FREQUENCY
    #define CLOCK_DIV            1                                       // input must be divided to 8MHz..16MHz range (/1 to /8 for 150MHz parts)
    #define CLOCK_MUL            25                                      // the PLL multiplication factor to achieve operating frequency of 120MHz (x16 to x47 possible - divided by 2 at VCO output)
    #define USB_CLOCK_GENERATED_INTERNALLY                               // use USB clock from internal source rather than external pin
    #define FLEX_CLOCK_DIVIDE    3                                       // 150/3 to give 50MHz
    #define FLASH_CLOCK_DIVIDE   6                                       // 150/6 to give 25MHz
    #define USB_CLOCK_GENERATED_INTERNALLY                               // use USB clock from internal source rather than external pin - 150MHz is suitable from PLL1
    #define USB_CLOCK_SOURCE_MCGPLL1CLK                                  // the clock source for the USB clock is dedicated to the FS USB interface (48MHz)
    #define CLOCK_DIV_1          1                                       // input must be divided to 8MHz..16MHz range (/1 to /8 for FPU parts)
    #define CLOCK_MUL_1          16                                      // PLL1 multiplication factor to achieve operating frequency of 96MHz [suitable for FS USB] (x16 to x47 possible - divided by 2 at VCC output)
#elif defined TWR_K60F120M || defined TWR_K70F120M
    #define EXTERNAL_CLOCK       50000000                                // this must be 50MHz in order to use Ethernet in RMII mode
    #define _EXTERNAL_CLOCK      EXTERNAL_CLOCK
    #define CLOCK_DIV            5                                       // input must be divided to 8MHz..16MHz range (/1 to /8 for FPU parts)
    #define CLOCK_MUL            30                                      // the PLL multiplication factor to achieve operating frequency of 120MHz (x16 to x47 possible - divided by 2 at VCC output)
    #define FLEX_CLOCK_DIVIDE    3                                       // 120/3 to give 40MHz
    #define FLASH_CLOCK_DIVIDE   6                                       // 120/5 to give 24MHz
    #define USB_CLOCK_GENERATED_INTERNALLY                               // use USB clock from internal source rather than external pin - 120MHz is suitable
 // #define USB_CLOCK_SOURCE_MCGPLL0CLK                                  // the clock source for the USB clock
#define USB_CLOCK_SOURCE_MCGPLL1CLK                                  // the clock source for the USB clock is dedicated to the FS USB interface (48MHz)
#define CLOCK_DIV_1          5                                       // input must be divided to 8MHz..16MHz range (/1 to /8 for FPU parts)
#define CLOCK_MUL_1          24                                      // PLL1 multiplication factor to achieve operating frequency of 96MHz [suitable for FS USB] (x16 to x47 possible - divided by 2 at VCC output)

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
#elif defined TWR_K60N512 || defined TWR_K60D100M || defined TWR_K53N512
    #define EXTERNAL_CLOCK       50000000                                // this must be 50MHz in order to use Ethernet in RMII mode
    #define _EXTERNAL_CLOCK      EXTERNAL_CLOCK
    #ifdef USB_INTERFACE                                                 // when using USB generate 96MHz clock so that a 48Mhz clock can be generated from it
        #define CLOCK_DIV        25                                      // input must be divided to 2MHz..4MHz range (/1 to /25 possible - /1 to /8 for FPU parts)
        #define CLOCK_MUL        48                                      // the PLL multiplication factor to achieve operating frequency of 100MHz (x24 to x55 possible)
    #else
        #define CLOCK_DIV        16                                      // input must be divided to 2MHz..4MHz range (/1 to /25 possible - /1 to /8 for FPU parts)
        #define CLOCK_MUL        32                                      // the PLL multiplication factor to achieve operating frequency of 100MHz (x24 to x55 possible)
    #endif
#elif defined KWIKSTIK
    #define CRYSTAL_FREQUENCY    4000000                                 // 4 MHz crystal
    #define _EXTERNAL_CLOCK      CRYSTAL_FREQUENCY
    #define CLOCK_DIV            2                                       // input must be divided to 2MHz..4MHz range (/1 to /25 possible - /1 to /8 for FPU parts)
    #define CLOCK_MUL            48                                      // the PLL multiplication factor to achieve operating frequency of 96MHz (x24 to x55 possible)
#elif defined TEENSY_3_1
    #define OSC_LOW_GAIN_MODE
    #define CRYSTAL_FREQUENCY    16000000                                // 16 MHz crystal
    #define _EXTERNAL_CLOCK      CRYSTAL_FREQUENCY
    #define CLOCK_DIV            8                                       // input must be divided to 2MHz..4MHz range (/1 to /25 possible)
    #define CLOCK_MUL            36                                      // the PLL multiplication factor to achieve operating frequency of 48MHz (x24 to x55 possible)
    #define USB_CLOCK_GENERATED_INTERNALLY                               // use USB clock from internal source rather than external pin
#elif defined TWR_K20D72M
    #define CRYSTAL_FREQUENCY    8000000                                 // 8 MHz crystal
    #define _EXTERNAL_CLOCK      CRYSTAL_FREQUENCY
    #define CLOCK_DIV            4                                       // input must be divided to 2MHz..4MHz range (/1 to /25 possible)
    #define CLOCK_MUL            36                                      // the PLL multiplication factor to achieve operating frequency of 48MHz (x24 to x55 possible)
    #define USB_CLOCK_GENERATED_INTERNALLY                               // use USB clock from internal source rather than external pin
#elif defined TWR_KM34Z50M
    #define CRYSTAL_FREQUENCY    8000000                                 // 8 MHz crystal
    #define _EXTERNAL_CLOCK      CRYSTAL_FREQUENCY
    #define CLOCK_DIV            4                                       // input must be divided to 2MHz..4MHz range (/1 to /25 possible)
    #define CLOCK_MUL            24                                      // the PLL multiplication factor to achieve operating frequency of 48MHz (x24 to x55 possible)
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
#elif defined TWR_KL43Z48M
    #define OSC_LOW_GAIN_MODE
    #define CRYSTAL_FREQUENCY    8000000                                 // 8MHz crystal
    #define _EXTERNAL_CLOCK      CRYSTAL_FREQUENCY
    #define RUN_FROM_HIRC                                                // clock from internal 48MHz RC clock
  //#define RUN_FROM_LIRC                                                // clock from internal 8MHz RC clock
    #define USB_CRYSTAL_LESS                                             // use 48MHz IRC as USB source (according to Freescale AN4905 - only possible in device mode) - rather than external pin
#elif defined FRDM_KL43Z || defined FRDM_KL27Z
    #define OSC_LOW_GAIN_MODE
    #define CRYSTAL_FREQUENCY    32768                                   // 32kHz crystal
    #define _EXTERNAL_CLOCK      CRYSTAL_FREQUENCY
    #define RUN_FROM_HIRC                                                // clock from internal 48MHz RC clock
  //#define RUN_FROM_LIRC                                                // clock from internal 8MHz RC clock
    #define USB_CRYSTAL_LESS                                             // use 48MHz IRC as USB source (according to Freescale AN4905 - only possible in device mode) - rather than external pin
#elif defined TWR_K20D50M || defined FRDM_K20D50M || defined TWR_K21D50M || defined tinyK20 || defined FRDM_KL46Z || defined TWR_KL46Z48M || defined FRDM_KL25Z || defined FRDM_KL26Z || defined TWR_KL25Z48M // {4}{5}{6}{7}
    #if defined FRDM_K20D50M || defined TWR_KL46Z48M || defined FRDM_KL25Z || defined FRDM_KL26Z || defined TWR_KL25Z48M || defined TWR_K21D50M || defined tinyK20
        #define OSC_LOW_GAIN_MODE                                        // oscillator without feedback resistor or load capacitors so use low gain mode
    #endif
  //#define RUN_FROM_DEFAULT_CLOCK
    #define CRYSTAL_FREQUENCY    8000000                                 // 8 MHz crystal
    #define _EXTERNAL_CLOCK      CRYSTAL_FREQUENCY
    #define CLOCK_DIV            4                                       // input must be divided to 2MHz..4MHz range (/1 to /25 possible)
    #if defined FRDM_KL46Z || defined TWR_KL46Z48M || defined FRDM_KL25Z || defined FRDM_KL26Z || defined TWR_KL25Z48M
        #define CLOCK_MUL        48                                      // the PLL multiplication factor to achieve MCGPLLCLK operating frequency of 98MHz (x24 to x55 possible)
        #define SYSTEM_CLOCK_DIVIDE 2                                    // divide to get core clock of 48MHz (MCGPLLCLK/2 is 48MHz - required by USB)
    #else
        #define CLOCK_MUL        24                                      // the PLL multiplication factor to achieve operating frequency of 48MHz (x24 to x55 possible)
    #endif
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
#elif defined FRDM_KE04Z || defined FRDM_KEAZN32Q64                      // {11}
    #define CRYSTAL_FREQUENCY    8000000                                 // 8 MHz crystal
    #define _EXTERNAL_CLOCK      CRYSTAL_FREQUENCY
    #define CLOCK_DIV            256                                     // input must be divided to 31.25kHz..39.06525kHz range (/1, 2, 4, 8, 16, 32, 64, 128, 256, 512 or 1024 possible)
                                                                         // this input is multipled by 1024 to 32MHz..40MHz at the FLL output
    #define SYSTEM_CLOCK_DIVIDE  1                                       // divide the FLL output to give the system clock (maximum 40MHz) (/1, 2, 4, 8, 16, 32, 64 or 128 possible)
    #define BUS_CLOCK_DIVIDE     2                                       // divide by 1 or 2 to give bus and flash clock (maximum 20MHz)
#elif defined TRK_KEA64
    #define RUN_FROM_DEFAULT_CLOCK                                       // default mode is FLL Engaged Internal - the 31.25kHz IRC is multiplied by FLL factor of 1024 to obtain 32MHz nominal frequency
    #define SYSTEM_CLOCK_DIVIDE  1                                       // divide the clock output to give the system clock (maximum 40MHz) (/1, 2, 4, 8, 16, 32, 64 or 128 possible)
    #define BUS_CLOCK_DIVIDE     2                                       // divide by 1 or 2 to give bus and flash clock (maximum 20MHz)
#elif defined TRK_KEA128 || defined FRDM_KEAZ64Q64 || defined FRDM_KEAZ128Q80
    #define CRYSTAL_FREQUENCY    8000000                                 // 8 MHz crystal
  //#define RUN_FROM_EXTERNAL_CLOCK                                      // run directly from external 8MHz clock (without FLL)
    #define _EXTERNAL_CLOCK      CRYSTAL_FREQUENCY
    #define CLOCK_DIV            256                                     // input must be divided to 31.25kHz..39.06525kHz range (/1, 2, 4, 8, 16, 32, 64, 128, 256, 512 or 1024 possible)
                                                                         // this input is multiplied by 1280 to 40MHz..50MHz at the FLL output
    #define SYSTEM_CLOCK_DIVIDE  1                                       // divide the clock output to give the system clock (maximum 48MHz) (/1, 2, 4, 8, 16, 32, 64 or 128 possible)
    #if defined RUN_FROM_EXTERNAL_CLOCK
        #define BUS_CLOCK_DIVIDE 1                                       // divide by 1 or 2 to give bus and flash clock (maximum 24MHz)
    #else
        #define BUS_CLOCK_DIVIDE 2                                       // divide by 1 or 2 to give bus and flash clock (maximum 24MHz)
    #endif
#elif defined FRDM_KE06Z                                                 // {11}
    #define CRYSTAL_FREQUENCY    8000000                                 // 8 MHz crystal
    #define _EXTERNAL_CLOCK      CRYSTAL_FREQUENCY
    #define CLOCK_DIV            256                                     // input must be divided to 31.25kHz..39.06525kHz range (/1, 2, 4, 8, 16, 32, 64, 128, 256, 512 or 1024 possible)
                                                                         // this input is multipled by 1024 to 32MHz..40MHz at the FLL output
    #define SYSTEM_CLOCK_DIVIDE  1                                       // divide the FLL output to give the system clock (maximum 40MHz) (/1, 2, 4, 8, 16, 32, 64 or 128 possible)
    #define BUS_CLOCK_DIVIDE     2                                       // divide by 1 or 2 to give bus and flash clock (maximum 20MHz)
#elif defined FRDM_KE02Z                                                 // {8}
    #define CRYSTAL_FREQUENCY    10000000                                // 10 MHz crystal
    #define _EXTERNAL_CLOCK      CRYSTAL_FREQUENCY
    #define CLOCK_DIV            256                                     // input must be divided to 31.25kHz..39.06525kHz range (/1, 2, 4, 8, 16, 32, 64, 128, 256, 512 or 1024 possible)
                                                                         // this input is multipled by 1024 to 32MHz..40MHz at the FLL output
    #define SYSTEM_CLOCK_DIVIDE  2                                       // divide the FLL output to give the system clock (maximum 20MHz) (/1, 2, 4, 8, 16, 32, 64 or 128 possible)
    #define BUS_CLOCK_DIVIDE     1                                       // divide by 1 or 2 to give bus and flash clock (maximum 20MHz)
#elif defined FRDM_KE02Z40M                                              // {11}
    #define CRYSTAL_FREQUENCY    10000000                                // 10 MHz crystal
    #define _EXTERNAL_CLOCK      CRYSTAL_FREQUENCY
    #define CLOCK_DIV            256                                     // input must be divided to 31.25kHz..39.06525kHz range (/1, 2, 4, 8, 16, 32, 64, 128, 256, 512 or 1024 possible)
                                                                         // this input is multipled by 1024 to 32MHz..40MHz at the FLL output
    #define SYSTEM_CLOCK_DIVIDE  1                                       // divide the FLL output to give the system clock (maximum 40MHz) (/1, 2, 4, 8, 16, 32, 64 or 128 possible)
    #define BUS_CLOCK_DIVIDE     2                                       // divide by 1 or 2 to give bus and flash clock (maximum 20MHz)
#elif defined FRDM_KL03Z
    #define CRYSTAL_FREQUENCY    32768                                   // 32768 Hz crystal (if no different clock options are set this is used directly as system clock)
    #define _EXTERNAL_CLOCK      CRYSTAL_FREQUENCY
    #define RUN_FROM_HIRC                                                // clock from internal 48MHz RC clock
  //#define RUN_FROM_LIRC                                                // clock from internal 8MHz RC clock
#elif defined FRDM_KL02Z || defined FRDM_KL05Z     // {8}
    #define OSC_LOW_GAIN_MODE                                            // oscillator without feedback resistor or load capacitors so use low gain mode
    #define CRYSTAL_FREQUENCY    32768                                   // 32768 Hz crystal
    #define _EXTERNAL_CLOCK      CRYSTAL_FREQUENCY
    #define FLL_FACTOR           1464                                    // 48MHz these devices have no PLL so use FLL (factors available are 640, 732, 1280, 1464, 1920, 2197, 2560 and 2929)
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
#elif defined K20FX512_120 || defined TWR_K21F120M
    #if defined TWR_K21F120M
        #define OSC_LOW_GAIN_MODE                                        // oscillator without feedback resistor or load capacitors so use low gain mode
    #endif
    #define CRYSTAL_FREQUENCY    8000000                                 // 8 MHz crystal
    #define _EXTERNAL_CLOCK      CRYSTAL_FREQUENCY
    #if defined TWR_K21F120M
        #define CLOCK_DIV        2                                       // input must be divided to 2MHz..4MHz range (/1 to /24 for 120MHz parts)
        #define CLOCK_MUL        30                                      // the PLL multiplication factor to achieve operating frequency of 120MHz (x24 to x55 possible)
    #else
        #define CLOCK_DIV        1                                       // input must be divided to 8MHz..16MHz range (/1 to /8 for 120MHz K20 parts)
        #define CLOCK_MUL        30                                      // the PLL multiplication factor to achieve operating frequency of 120MHz (x16 to x47 possible - divided by 2 at VCO output)
    #endif
    #define FLEX_CLOCK_DIVIDE    3                                       // 120/3 to give 40MHz
    #define FLASH_CLOCK_DIVIDE   5                                       // 120/5 to give 24MHz
    #define USB_CLOCK_GENERATED_INTERNALLY                               // use USB clock from internal source rather than external pin - 120MHz is suitable
    #define USB_CLOCK_SOURCE_MCGPLL0CLK                                  // the clock source for the USB clock
#elif defined BLAZE_K22
    #define OSC_LOW_GAIN_MODE                                            // oscillator without feedback resistor or load capacitors so use low gain mode
    #define CRYSTAL_FREQUENCY    16000000                                // 16 MHz crystal
    #define _EXTERNAL_CLOCK      CRYSTAL_FREQUENCY
    #define CLOCK_DIV            4                                       // input must be divided to 2MHz..4MHz range (/1 to /24 for 120MHz parts)
    #define CLOCK_MUL            30                                      // the PLL multiplication factor to achieve operating frequency of 120MHz (x24 to x55 possible)
    #define FLEX_CLOCK_DIVIDE    3                                       // 120/3 to give 40MHz
    #define FLASH_CLOCK_DIVIDE   5                                       // 120/5 to give 24MHz
    #define USB_CLOCK_GENERATED_INTERNALLY                               // use USB clock from internal source rather than external pin - 120MHz is suitable
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
        #define FLEX_CLOCK_DIVIDE    2                                   // 120/2 to give 60MHz
        #define FLASH_CLOCK_DIVIDE   5                                   // 120/5 to give 24MHz
    #endif
  //#define USB_CRYSTAL_LESS                                             // use 48MHz IRC as USB source (according to Freescale AN4905 - only possible in device mode)
    #define USB_CLOCK_GENERATED_INTERNALLY                               // use USB clock from internal source rather than external pin - 120MHz is suitable
#else                                                                    // TWR_K40X256 and TWR_K40D100M
    #define CRYSTAL_FREQUENCY    8000000                                 // 8 MHz crystal
    #define _EXTERNAL_CLOCK      CRYSTAL_FREQUENCY
    #define CLOCK_DIV            4                                       // input must be divided to 2MHz..4MHz range (/1 to /25 possible - /1 to /8 for FPU parts)
    #define CLOCK_MUL            48                                      // the PLL multiplication factor to achieve operating frequency of 96MHz (x24 to x55 possible)
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

#if defined TWR_K70F120M
  //#define KINETIS_FLEX                                                 // X part with flex memory rather than N part with program Flash only
    #define PIN_COUNT           PIN_COUNT_256_PIN
    #define SIZE_OF_FLASH       (1024 * 1024)                            // 1M FLASH
    #define SIZE_OF_RAM         (128 * 1024)                             // 128k SRAM
#elif defined K70F150M_12M
    #define PIN_COUNT           PIN_COUNT_256_PIN
    #define SIZE_OF_FLASH       (1024 * 1024)                            // 1M FLASH
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
#elif defined TWR_K65F180M || defined K26FN2_180 || defined FRDM_K66F || defined K66FX1M0 || defined FRDM_K28F
    #define MASK_0N65N
    #if defined FRDM_K66F || defined K66FX1M0
        #define PIN_COUNT       PIN_COUNT_144_PIN                        // 144 pin package
    #else
        #define PIN_COUNT       PIN_COUNT_169_PIN                        // 169 pin package
    #endif
    #define PACKAGE_TYPE        PACKAGE_MAPBGA
  //#define PACKAGE_TYPE        PACKAGE_WLCSP
    #if defined K66FX1M0
        #define KINETIS_FLEX                                             // X part with flex memory rather than N part with program Flash only
    #endif
    #if defined FRDM_K28F
        #define SIZE_OF_FLASH       (2 * 1024 * 1024)                    // 2M FLASH
        #define SIZE_OF_RAM         (1024 * 1024)                        // 1M SRAM
    #else
        #define SIZE_OF_FLASH       (2 * 1024 * 1024)                    // 2M FLASH
        #define SIZE_OF_RAM         (256 * 1024)                         // 256k SRAM
    #endif
#elif defined TEENSY_3_6
    #define MASK_0N65N
    #define PIN_COUNT           PIN_COUNT_144_PIN                        // 169 pin package
    #define PACKAGE_TYPE        PACKAGE_MAPBGA
  //#define PACKAGE_TYPE        PACKAGE_LQFP
    #define KINETIS_FLEX                                                 // X part with flex memory rather than N part with program Flash only
    #define SIZE_OF_FLASH       (1 * 1024 * 1024)                        // 1M FLASH
    #define SIZE_OF_RAM         (256 * 1024)                             // 256k SRAM
#elif defined FRDM_K64F
  //#define KINETIS_FLEX                                                 // X part with flex memory rather than N part with program Flash only
    #define PIN_COUNT           PIN_COUNT_100_PIN                        // 100 LQFP pin package
  //#define PIN_COUNT           PIN_COUNT_121_PIN                        // 121 XFBGA
  //#define PIN_COUNT           PIN_COUNT_144_PIN                        // 144 LQFP/MAPBGA pin package
    #define PACKAGE_TYPE        PACKAGE_LQFP
  //#define PACKAGE_TYPE        PACKAGE_MAPBGA
    #define SIZE_OF_FLASH       (1024 * 1024)                            // 1M FLASH
    #define SIZE_OF_RAM         (256 * 1024)                             // 256k SRAM
#elif defined TEENSY_3_5
    #define KINETIS_FLEX                                                 // X part with flex memory rather than N part with program Flash only
    #define PIN_COUNT           PIN_COUNT_144_PIN                        // 144 LQFP/MAPBGA pin package
    #define PACKAGE_TYPE        PACKAGE_MAPBGA
    #define SIZE_OF_FLASH       (512 * 1024)                             // 512 program FLASH
    #define SIZE_OF_RAM         (256 * 1024)                             // 256k SRAM
#elif defined FRDM_K22F || defined tinyK22
    #define PIN_COUNT           PIN_COUNT_64_PIN                         // 64 LQFP pin package
    #define PACKAGE_TYPE        PACKAGE_LQFP
    #define SIZE_OF_FLASH       (512 * 1024)                             // 512k FLASH
    #define SIZE_OF_RAM         (128 * 1024)                             // 128k SRAM
#elif defined BLAZE_K22
    #define MASK_0N50M                                                   // mask relevant to this device
    #define PIN_COUNT           PIN_COUNT_64_PIN                         // 64 LQFP pin package
    #define PACKAGE_TYPE        PACKAGE_LQFP
    #define SIZE_OF_FLASH       (1024 * 1024)                            // 1M FLASH
    #define SIZE_OF_RAM         (128 * 1024)                             // 128k SRAM
#elif defined TWR_K22F120M
    #define PIN_COUNT           PIN_COUNT_121_PIN                        // 121 XFBGA package
    #define SIZE_OF_FLASH       (512 * 1024)                             // 512k FLASH
    #define SIZE_OF_RAM         (128 * 1024)                             // 128k SRAM
#elif defined TWR_K24F120M
  //#define PIN_COUNT           PIN_COUNT_100_PIN                        // 100 pin LQFP
    #define PIN_COUNT           PIN_COUNT_121_PIN                        // XBGA121
  //#define PIN_COUNT           PIN_COUNT_144_PIN                        // 144 pin LQFP
  //#define SIZE_OF_FLASH       (1024 * 1024)                            // 1024k program Flash
    #define SIZE_OF_FLASH       (256 * 1024)                             // 256k program Flash
    #define SIZE_OF_RAM         (256 * 1024)                             // 128k SRAM
#elif defined TWR_K60F120M || defined K60F150M_50M
    #define PIN_COUNT           PIN_COUNT_144_PIN
  //#define KINETIS_FLEX                                                 // X part with flex memory rather than N part with program Flash only
  //#define DEVICE_100_PIN
  //#define DEVICE_121_PIN
    #define DEVICE_144_PIN                                               // 144 pin package
    #define SIZE_OF_FLASH       (1024 * 1024)                            // 1M FLASH
    #define SIZE_OF_RAM         (128 * 1024)                             // 128k SRAM
#elif defined TWR_K60N512 || defined TWR_K60D100M
    #define PIN_COUNT           PIN_COUNT_144_PIN
    #define PACKAGE_TYPE        PACKAGE_MAPBGA
  //#define KINETIS_FLEX                                                 // X part with flex memory rather than N part with program Flash only
  //#define DEVICE_100_PIN
  //#define DEVICE_121_PIN
    #define DEVICE_144_PIN                                               // 144 pin package
    #define SIZE_OF_FLASH       (512 * 1024)                             // 512k FLASH
    #define SIZE_OF_RAM         (128 * 1024)                             // 128k SRAM
#elif defined TWR_K53N512
  //#define KINETIS_FLEX                                                 // X part with flex memory rather than N part with program Flash only
  //#define DEVICE_100_PIN
  //#define DEVICE_121_PIN
    #define DEVICE_144_PIN                                               // 144 pin package
    #define SIZE_OF_FLASH       (512 * 1024)                             // 512k FLASH
    #define SIZE_OF_RAM         (128 * 1024)                             // 128k SRAM
#elif defined TWR_K40X256 || defined KWIKSTIK || defined TWR_K40D100M
    #define KINETIS_FLEX                                                 // X part with flex memory rather than N part with program Flash only
  //#define DEVICE_80_PIN
  //#define DEVICE_100_PIN
  //#define DEVICE_121_PIN
    #define DEVICE_144_PIN                                               // 144 pin package
    #define SIZE_OF_FLASH       (256 * 1024)                             // 256k program FLASH
    #define SIZE_OF_FLEXFLASH   (256 * 1024)                             // 256k Flex
    #define SIZE_OF_RAM         (64 * 1024)                              // 64k SRAM
    #define SIZE_OF_EEPROM      (4 * 1024)                               // 4k EEPROM
#elif defined TWR_K20D72M
    #define KINETIS_FLEX                                                 // X part with flex memory rather than N part with program Flash only
    #define PIN_COUNT           PIN_COUNT_100_PIN                        // 100 pin LQFP
    #define SIZE_OF_FLASH       (256 * 1024)                             // 256k program FLASH
    #define SIZE_OF_FLEXFLASH   (256 * 1024)                             // 256 Flex
    #define SIZE_OF_RAM         (64 * 1024)                              // 64k SRAM
    #define SIZE_OF_EEPROM      (2 * 1024)                               // 4k EEPROM
#elif defined K20F120M
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
#elif defined TEENSY_3_1
    #define KINETIS_FLEX                                                 // X part with flex memory rather than N part with program Flash only
    #define PIN_COUNT           PIN_COUNT_64_PIN                         // 64 pin LQFP
    #define PACKAGE_TYPE        PACKAGE_LQFP
    #define SIZE_OF_FLEXFLASH   (32 * 1024)                              // 32 Flex
    #define SIZE_OF_RAM         (64 * 1024)                              // 64k SRAM
    #define SIZE_OF_EEPROM      (2 * 1024)                               // 2k EEPROM
  //#define FLEXFLASH_DATA
    #if defined FLEXFLASH_DATA
        #define SIZE_OF_FLASH   ((256 * 1024) + SIZE_OF_FLEXFLASH)       // 256k program FLASH plus data flash
    #else
        #define SIZE_OF_FLASH   (256 * 1024)                             // 256k program FLASH
    #endif
#elif defined K02F100M
    #define MASK_0N36M                                                   // enable errata workarounds for this mask
  //#define PIN_COUNT           PIN_COUNT_32_PIN                         // 32 pin QFN package
  //#define PIN_COUNT           PIN_COUNT_48_PIN                         // 48 pin LQFP package
    #define PIN_COUNT           PIN_COUNT_64_PIN                         // 64 pin LQFP package
    #define SIZE_OF_RAM         (16 * 1024)                              // 16k SRAM
  //#define SIZE_OF_FLASH       (64 * 1024)                              // 64k program FLASH
    #define SIZE_OF_FLASH       (128 * 1024)                             // 128k program FLASH
#elif defined TWR_K21F120M
  //#define KINETIS_FLEX
    #define PIN_COUNT           PIN_COUNT_121_PIN                        // MAPBGA121
    #define SIZE_OF_FLASH       (1024 * 1024)                            // 1024k program Flash
    #define SIZE_OF_RAM         (128 * 1024)                             // 128k SRAM
#elif defined TWR_K21D50M
    #define PIN_COUNT           PIN_COUNT_121_PIN                        // MAPBGA121
    #define SIZE_OF_FLASH       (512 * 1024)                             // 512k program Flash
    #define SIZE_OF_RAM         (64 * 1024)                              // 64k SRAM
#elif defined TWR_K20D50M || defined FRDM_K20D50M                        // TWR_K20D50M and FRDM_K20D50M
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
    #define SIZE_OF_FLASH       (128 * 1024)                             // 128k program Flash
    #define SIZE_OF_FLEXFLASH   (32 * 1024)                              // 32 Flex
    #define SIZE_OF_RAM         (16 * 1024)                              // 16k SRAM
    #define SIZE_OF_EEPROM      (2 * 1024)                               // 2k EEPROM
#elif defined tinyK20
    #define KINETIS_FLEX                                                 // X part with flex memory rather than N part with program Flash only
    #define PIN_COUNT           PIN_COUNT_48_PIN
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
#elif defined TWR_KM34Z50M
    #define PIN_COUNT           PIN_COUNT_100_PIN                        // 100 pin LQFP
  //#define PIN_COUNT           PIN_COUNT_64_PIN                         // 64 pin LQFP
  //#define PIN_COUNT           PIN_COUNT_44_PIN                         // LGA pin LGA
    #define SIZE_OF_FLASH       (128 * 1024)                             // 128k program Flash
  //#define SIZE_OF_FLASH       (64 * 1024)
    #define SIZE_OF_RAM         (16 * 1024)                              // 16k SRAM
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
#elif defined FRDM_KL43Z || defined TWR_KL43Z48M
    #define PIN_COUNT           PIN_COUNT_64_PIN                         // 64 pin LQFP or MAPBGA
    #define PACKAGE_TYPE        PACKAGE_LQFP                             // LQFP
  //#define PACKAGE_TYPE        PACKAGE_MAPBGA
    #define SIZE_OF_FLASH       (256 * 1024)                             // 256k program Flash
  //#define SIZE_OF_FLASH       (128 * 1024)
    #define SIZE_OF_RAM         (32 * 1024)                              // 32k SRAM
  //#define SIZE_OF_RAM         (16 * 1024)
#elif defined FRDM_KL46Z || defined TWR_KL46Z48M                         // {5}{6}
  //#define PIN_COUNT           PIN_COUNT_64_PIN
    #define PIN_COUNT           PIN_COUNT_100_PIN                        // 100 pin package
  //#define PIN_COUNT           PIN_COUNT_121_PIN
    #define PACKAGE_TYPE        PACKAGE_LQFP                             // LQFP
  //#define PACKAGE_TYPE        PACKAGE_MAPBGA
  //#define PACKAGE_TYPE        PACKAGE_BGA
    #define SIZE_OF_FLASH       (256 * 1024)                             // 256k program Flash
    #define SIZE_OF_RAM         (32 * 1024)                              // 32k SRAM
#elif defined TWR_KV31F120M
  //#define PIN_COUNT           PIN_COUNT_64_PIN
    #define PIN_COUNT           PIN_COUNT_100_PIN                        // 48 pin package
    #define SIZE_OF_FLASH       (512 * 1024)                             // 512k program Flash
    #define SIZE_OF_RAM         (96 * 1024)                              // 96k SRAM
#elif defined TWR_KV10Z32
  //#define PIN_COUNT           PIN_COUNT_32_PIN
    #define PIN_COUNT           PIN_COUNT_48_PIN                         // 48 pin package
    #define PACKAGE_TYPE        PACKAGE_LQFP                             // LQFP
  //#define PACKAGE_TYPE        PACKAGE_QFN
    #define SIZE_OF_FLASH       (32 * 1024)                              // 32k program Flash
    #define SIZE_OF_RAM         (8 * 1024)                               // 8k SRAM
#elif defined FRDM_KE04Z                                                 // {11}
  //#define PIN_COUNT           PIN_COUNT_16_PIN                         // 16 pin TSSOP
  //#define PIN_COUNT           PIN_COUNT_20_PIN                         // 20 pin SOIC
    #define PIN_COUNT           PIN_COUNT_24_PIN                         // 24 pin QFN
  //#define PIN_COUNT           PIN_COUNT_44_PIN                         // 44 pin LQFP
  //#define PIN_COUNT           PIN_COUNT_64_PIN                         // 64 pin (L)QFP
  //#define PIN_COUNT           PIN_COUNT_80_PIN                         // 80 pin LQFP
    #define SIZE_OF_FLASH       (8 * 1024)                               // 8k program Flash
  //#define SIZE_OF_FLASH       (64 * 1024)
  //#define SIZE_OF_FLASH       (128 * 1024)
    #define SIZE_OF_RAM         (1 * 1024)                               // 1k SRAM
    #define INTERRUPT_VECTORS_IN_FLASH                                   // when RAM is very limited interrupt vectors are fixed in flash
  //#define SIZE_OF_RAM         (8 * 1024)
  //#define SIZE_OF_RAM         (16 * 1024)
#elif defined TRK_KEA64 || defined FRDM_KEAZ64Q64
  //#define PIN_COUNT           PIN_COUNT_32_PIN                         // 32 pin LQFP
    #define PIN_COUNT           PIN_COUNT_64_PIN                         // 64 pin LQFP
  //#define SIZE_OF_FLASH       (32 * 1024)
    #define SIZE_OF_FLASH       (64 * 1024)                              // 64k program Flash
  //#define SIZE_OF_RAM         (2 * 1024)
    #define SIZE_OF_RAM         (4 * 1024)                               // 4k SRAM
#elif defined FRDM_KE06Z || defined TRK_KEA128 || defined FRDM_KEAZ128Q80 // {11}
  //#define PIN_COUNT           PIN_COUNT_44_PIN                         // 44 pin LQFP
  //#define PIN_COUNT           PIN_COUNT_64_PIN                         // 64 pin (L)QFP
    #define PIN_COUNT           PIN_COUNT_80_PIN                         // 80 pin LQFP
  //#define SIZE_OF_FLASH       (64 * 1024)
    #define SIZE_OF_FLASH       (128 * 1024)                             // 128k Flash
  //#define SIZE_OF_RAM         (8 * 1024)
    #define SIZE_OF_RAM         (16 * 1024)                              // 16k SRAM
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
#elif defined FRDM_KE02Z || defined FRDM_KE02Z40M
  //#define PIN_COUNT           PIN_COUNT_32_PIN                         // 32 pin LQFP
  //#define PIN_COUNT           PIN_COUNT_44_PIN                         // 44 pin LQFP
    #define PIN_COUNT           PIN_COUNT_64_PIN                         // 64 pin
  //#define SIZE_OF_FLASH       (16 * 1024)
  //#define SIZE_OF_FLASH       (32 * 1024)
    #define SIZE_OF_FLASH       (64 * 1024)                              // 64k program Flash
  //#define SIZE_OF_RAM         (2 * 1024)
    #define SIZE_OF_RAM         (4 * 1024)                               // 4k SRAM
#elif defined FRDM_KL02Z
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
#elif defined FRDM_KL03Z
  //#define PIN_COUNT           PIN_COUNT_16_PIN                         // 16 pin QFN
  //#define PIN_COUNT           PIN_COUNT_20_PIN                         // 20 pin WLCSP
    #define PIN_COUNT           PIN_COUNT_24_PIN                         // 24 pin QFN
  //#define SIZE_OF_FLASH       (8 * 1024)
  //#define SIZE_OF_FLASH       (16 * 1024)
    #define SIZE_OF_FLASH       (32 * 1024)                              // 32k program Flash
    #define SIZE_OF_RAM         (2 * 1024)                               // 2k SRAM
#elif defined FRDM_KL05Z                                                 // {8}
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
#elif defined FRDM_KL25Z || defined TWR_KL25Z48M                         // {7}
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
#elif defined FRDM_KL26Z                                                 // {7}
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
#elif defined FRDM_KL27Z
  //#define PIN_COUNT           PIN_COUNT_32_PIN
  //#define PIN_COUNT           PIN_COUNT_48_PIN
    #define PIN_COUNT           PIN_COUNT_64_PIN                         // 64 pin package
  //#define PIN_COUNT           PIN_COUNT_100_PIN
  //#define PIN_COUNT           PIN_COUNT_121_PIN
    #define PACKAGE_TYPE        PACKAGE_LQFP                             // LQFP
  //#define PACKAGE_TYPE        PACKAGE_QFN
  //#define PACKAGE_TYPE        PACKAGE_BGA
  //#define SIZE_OF_FLASH       (32 * 1024)                              // 32k program Flash
    #define SIZE_OF_FLASH       (64 * 1024)                              // 64k program Flash
  //#define SIZE_OF_FLASH       (128 * 1024)                             // 128k program Flash
  //#define SIZE_OF_FLASH       (256 * 1024)                             // 256k program Flash
  //#define SIZE_OF_RAM         (4 * 1024)                               // 4k SRAM
  //#define SIZE_OF_RAM         (8 * 1024)                               // 8k SRAM
    #define SIZE_OF_RAM         (16 * 1024)                              // 16k SRAM
  //#define SIZE_OF_RAM         (32 * 1024)                              // 32k SRAM
#elif defined TEENSY_LC
    #define PIN_COUNT           PIN_COUNT_64_PIN                         // 64 pin package
    #define PACKAGE_TYPE        PACKAGE_QFN                              // QFN
    #define SIZE_OF_FLASH       (64 * 1024)                              // 64k program Flash
    #define SIZE_OF_RAM         (8 * 1024)                               // 8k SRAM
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
#endif

#include "../../Hardware/Kinetis/kinetis.h"

#define TICK_INTERRUPT()                                                 // user callback from system TICK



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
        #if defined KINETIS_KL03 || defined KINETIS_KL43 || defined KINETIS_KL27
            #if defined TWR_KL43Z48M || defined FRDM_KL43Z || defined FRDM_KL03Z || defined FRDM_KL27Z
                #define KINETIS_FLASH_CONFIGURATION_NONVOL_OPTION  (FTFL_FOPT_LPBOOT_CLK_DIV_0 | FTFL_FOPT_RESET_PIN_ENABLED | FTFL_FOPT_BOOTSRC_SEL_FLASH | FTFL_FOPT_BOOTPIN_OPT_DISABLE | FTFL_FOPT_NMI_DISABLED)
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
            #elif defined TWR_K65F180M
                #define KINETIS_FLASH_CONFIGURATION_NONVOL_OPTION  (FTFL_FOPT_EZPORT_DISABLED | FTFL_FOPT_LPBOOT_NORMAL | FTFL_FOPT_NMI_DISABLED) // disable the EzPort and NMI so that the switch can be used to force debug mode
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
    #define NUMBER_EXTERNAL_SERIAL 0
    #define NUMBER_SERIAL   (UARTS_AVAILABLE + LPUARTS_AVAILABLE)        // the number of physical queues needed for serial interface(s)
    #define SIM_COM_EXTENDED                                             // COM ports defined from 1..255 {50}
    #define SERIAL_PORT_0   4                                            // if we open UART channel 0 we simulate using comx on the PC
    #define SERIAL_PORT_1   4                                            // if we open UART channel 1 we simulate using comx on the PC
    #define SERIAL_PORT_2   4                                            // if we open UART channel 2 we simulate using comx on the PC
    #define SERIAL_PORT_3   4                                            // if we open UART channel 3 we simulate using comx on the PC
    #define SERIAL_PORT_4   4                                            // if we open UART channel 4 we simulate using comx on the PC
    #define SERIAL_PORT_5   4                                            // if we open UART channel 5 we simulate using comx on the PC

    #if defined KWIKSTIK || defined TWR_K60F120M || defined K20FX512_120 || defined TWR_K21F120M || (defined TWR_K64F120M && defined TWR_SER)
        #define LOADER_UART           5                                  // the serial interface used by the serial loader
    #elif defined FRDM_K82F
        #define LOADER_UART           4                                  // use UART 4
    #elif defined TWR_K70F120M || defined TWR_KL46Z48M || defined TWR_K21D50M || defined TWR_KL43Z48M || defined TRK_KEA128 || defined TRK_KEA64 || defined KL25_TEST_BOARD || defined TWR_K65F180M || defined K26FN2_180 || defined FRDM_KEAZN32Q64 || defined FRDM_KEAZ64Q64 || defined FRDM_KEAZ128Q80 || defined TEENSY_3_5 || defined TEENSY_3_6
        #define LOADER_UART           2                                  // the serial interface used by the serial loader
    #elif defined TWR_K20D50M || defined TWR_K80F150M || defined tinyK20 || defined TWR_K20D72M || defined FRDM_KE02Z || defined FRDM_KE02Z40M || defined FRDM_KE06Z || defined FRDM_K22F || defined TWR_K22F120M || defined TWR_K24F120M || defined TWR_K64F120M || defined TWR_KW21D256 || defined TWR_KW24D512 || defined BLAZE_K22 || defined tinyK22 || defined FRDM_KE15Z
        #define LOADER_UART           1                                  // the serial interface used by the serial loader
    #elif defined K02F100M || defined FRDM_K20D50M || defined FRDM_KL46Z || defined FRDM_KL43Z || defined FRDM_KL25Z || defined FRDM_KL26Z || defined FRDM_KL27Z || defined TEENSY_LC || defined TWR_KL25Z48M || defined FRDM_KL02Z || defined FRDM_KL03Z || defined FRDM_KL05Z || defined TEENSY_3_1 || defined FRDM_K64F || defined FRDM_K66F || defined FRDM_KE04Z || defined TWR_KV10Z32 || defined TWR_KV31F120M || ((defined TWR_K40X256 || defined TWR_K40D100M) && defined DEBUG_ON_VIRT_COM)
        #define LOADER_UART           0                                  // the serial interface used by the serial loader
    #else
        #define LOADER_UART           3                                  // the serial interface used by the serial loader
    #endif
    #if defined FRDM_KL03Z || defined FRDM_KL43Z || defined TWR_KL43Z48M || defined FRDM_KL27Z || defined TWR_K80F150M || defined FRDM_K82F
        #define LPUART_IRC48M                                            // if the 48MHz clock is available clock the UART from it
      //#define LPUART_OSCERCLK                                          // clock the UART from the external clock
      //#define LPUART_MCGIRCLK                                          // clock the UART from MCGIRCLK (IRC8M/FCRDIV/LIRC_DIV2) - default if others are not defined
    #endif

    #if defined FRDM_KE04Z                                               // small RAM size (1k)
        #define TX_BUFFER_SIZE   (128)                                   // the size of UART input and output buffers
        #define RX_BUFFER_SIZE   (8)
    #elif defined FRDM_KL03Z                                             // small RAM size (2k)
        #define TX_BUFFER_SIZE   (360)                                   // the size of UART input and output buffers
        #define RX_BUFFER_SIZE   (64)
    #elif defined TEENSY_LC                                              // small RAM size (8k) and needs USB
        #define TX_BUFFER_SIZE   (360)                                   // the size of UART input and output buffers
        #define RX_BUFFER_SIZE   (64)
    #else
        #if defined DEBUG_MAC
            #define TX_BUFFER_SIZE   (48 * 1024)                         // allow large amount of debug output to be collected
        #elif defined USB_HOST_SUPPORT
            #define TX_BUFFER_SIZE   (1024)                              // the size of UART input and output buffers
        #else
            #define TX_BUFFER_SIZE   (512)                               // the size of UART input and output buffers
        #endif
        #if defined _WINDOWS_
            #define RX_BUFFER_SIZE   (32000)                             // used for simulation to ensure that the rx buffer doesn't overflow
        #else
            #define RX_BUFFER_SIZE   (512)
        #endif
    #endif

  //#define UART0_A_LOW                                                  // alternative UART0 pin mapping
    #if defined FRDM_K20D50M || defined TEENSY_3_1 || defined TEENSY_LC || defined FRDM_K64F || defined FRDM_K66F || defined TWR_KV10Z32 || defined TWR_KV31F120M // {4}{9}
        #define UART0_ON_B                                               // alternative UART0 pin mapping
    #elif defined FRDM_KL46Z || defined FRDM_KL43Z || defined FRDM_KL25Z || defined FRDM_KL26Z || defined FRDM_KL27Z // {5}{7}
        #define UART0_A_LOW
    #elif ((defined TWR_K40X256 || defined TWR_K40D100M) && defined DEBUG_ON_VIRT_COM)
        #define UART0_ON_D                                               // alternative UART0 pin mapping
    #endif
  //#define UART0_ON_D                                                   // alternative UART0 pin mapping
    #if defined TWR_K64F120M || defined tinyK20
        #define UART1_ON_C                                               // alternative UART1 pin mapping
    #endif
    #if defined TWR_K70F120M || defined TWR_KL46Z48M || defined TWR_K21D50M || defined TWR_K65F180M || defined K26FN2_180 // {6}
        #define UART2_ON_E                                               // alternative UART2 pin mapping on port E (default is on port D)
    #endif
    #if defined TWR_KL43Z48M || defined KL25_TEST_BOARD
        #define UART2_ON_E_HIGH
    #endif
  //#define UART3_ON_B                                                   // alternative UART3 pin mapping
    #define UART3_ON_C                                                   // alternative UART3 pin mapping
  //#define UART4_ON_C                                                   // alternative UART4 pin mapping
    #if defined TWR_K21F120M
        #define UART5_ON_D                                               // alternative UART5 pin mapping
    #endif

    #if defined KINETIS_KL
      //#define UART0_ON_E                                               // alternative UART0 pin mapping
      //#define UART1_ON_A                                               // alternative UART1 pin mapping
      //#define UART2_ON_E_HIGH                                          // alternative UART2 pin mapping
      //#define UART2_ON_D_HIGH                                          // alternative UART2 pin mapping
    #endif
#else
    #define TX_BUFFER_SIZE   (256)
    #define RX_BUFFER_SIZE   (256)
#endif

#if defined BLAZE_K22
    #define OUR_I2C_CHANNEL       0                                      // use I2C0 for touch-screen controller
    #define I2C0_B_LOW                                                   // I2C0_SCL on PB0 and I2C0_SDA on PB1
#endif


#if defined KINETIS_KL || defined KINETIS_KE
    // Define interrupt priorities in the system (kinetis KE and KL support 0..3 - 0 is highest priority and 3 is lowest priority)
    //
    #define SYSTICK_PRIORITY           3                                 // lowest priority
    #define USB_PIT_INTERRUPT_PRIORITY 2
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
    #define PRIORITY_IIC0              2
    #define PRIORITY_IIC1              2
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
    // Define interrupt priorities in the system (kinetis support 0..15 - 0 is highest priority and 15 is lowest priority)
    //
    #define SYSTICK_PRIORITY           15
    #define PRIORITY_USB_HS_OTG        14
    #define USB_PIT_INTERRUPT_PRIORITY 10
    #define PRIORITY_PORT_C_INT        7
    #define PRIORITY_PHY_PORT_INT      7
    #define PRIORITY_UART0             6
    #define PRIORITY_UART1             6
    #define PRIORITY_UART2             6
    #define PRIORITY_UART3             6
    #define PRIORITY_UART4             6
    #define PRIORITY_UART5             6
    #define PRIORITY_LPUART5           5
    #define PRIORITY_LPUART4           5
    #define PRIORITY_LPUART3           5
    #define PRIORITY_LPUART2           5
    #define PRIORITY_LPUART1           5
    #define PRIORITY_LPUART0           5
    #define PRIORITY_HW_TIMER          5
    #define PRIORITY_TIMERS            5
    #define PRIORITY_USB_OTG           4
    #define PRIORITY_IIC0              4
    #define PRIORITY_IIC1              4
    #define PRIORITY_IIC2              4
    #define PRIORITY_TWI               4
    #define RIT_INTERRUPT_PRIORITY     4
    #define PRIORITY_TICK_TIMER        3
    #define PRIORITY_ADC               2
    #define USB_PRIORITY               2
    #define PRIORITY_EMAC              1
#endif


#define SDCARD_SIM_SIZE   SDCARD_SIZE_2G                                 // the size of SD card when simulating
//#define _NO_SD_CARD_INSERTED                                           // simulate no SD card inserted

// Ports
//
#if defined FRDM_K82F
    #define LED_RED                PORTC_BIT8
    #define LED_GREEN              PORTC_BIT9
    #define LED_BLUE               PORTC_BIT10
    #define BLINK_LED              LED_GREEN
    #define SWITCH_2               PORTA_BIT4
    #define SWITCH_3               PORTC_BIT6
    #define USB_HOST_POWER_ENABLE  PORTC_BIT7                            // FRDM-K82F uses a jumper to enable USB power - if J1-11 (PTC7) is connected to J21-2 (USBSW_ENABLE) it can be controlled by the software

    #define INIT_WATCHDOG_DISABLE() _CONFIG_PORT_INPUT_FAST_LOW(A, SWITCH_2, PORT_PS_UP_ENABLE) // use fast access version (beware that this can only operate on half of the 32 bits at a time)
    #define WATCHDOG_DISABLE()     (_READ_PORT_MASK(A, SWITCH_2) == 0)   // hold SW2 at reset to disable the watchdog
    #define FORCE_BOOT()           (_READ_PORT_MASK(C, SWITCH_3) == 0)   // hold SW3 at reset to force loader mode
    #define RETAIN_LOADER_MODE()   (_READ_PORT_MASK(C, SWITCH_3) == 0)   // keep SW3 held down to retain loader mode after SD card or memory stick has been checked

    #define INIT_WATCHDOG_LED()    _CONFIG_DRIVE_PORT_OUTPUT_VALUE(C, (BLINK_LED), (BLINK_LED), (PORT_SRE_SLOW | PORT_DSE_HIGH)); _CONFIG_PORT_INPUT_FAST_LOW(C, SWITCH_3, PORT_PS_UP_ENABLE)
    #define TOGGLE_WATCHDOG_LED()  _TOGGLE_PORT(C, BLINK_LED)

    #define USB_HOST_POWER_CONFIG() _CONFIG_DRIVE_PORT_OUTPUT_VALUE_FAST_LOW(C, (USB_HOST_POWER_ENABLE), (0), (PORT_SRE_SLOW | PORT_DSE_HIGH))
    #define USB_HOST_POWER_ON()     _SETBITS(C, USB_HOST_POWER_ENABLE)
    #define USB_HOST_POWER_OFF()    _CLEARBITS(C, USB_HOST_POWER_ENABLE)
#elif defined TWR_K60N512 || defined TWR_K60D100M || defined TWR_K60F120M || defined TWR_K70F120M || defined TWR_K80F150M || defined K60F150M_50M
    #if defined K60F150M_50M
        #define DEMO_LED_1         (PORTB_BIT6)                          // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #else
        #define DEMO_LED_1         (PORTA_BIT11)                         // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #endif
    #define DEMO_LED_2             (PORTA_BIT28)                         // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define DEMO_LED_3             (PORTA_BIT29)                         // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define DEMO_LED_4             (PORTA_BIT10)                         // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define BLINK_LED              (DEMO_LED_1)
    #if defined TWR_K70F120M
        #define SWITCH_1           (PORTD_BIT0)                          // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
        // When using HS USB on the TWR-SER2 board the USB tranceiver needs to be taken out of reset by setting PTB8 to '1'
        //
        #define ENABLE_HSUSB_TRANSCEIVER()   _CONFIG_DRIVE_PORT_OUTPUT_VALUE(B, (PORTB_BIT8), (PORTB_BIT8), (PORT_SRE_SLOW | PORT_DSE_LOW)); // take the HS USB transceiver out of reset state
    #else
        #define SWITCH_1           (PORTA_BIT19)                         // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #endif
    #define SWITCH_2               (PORTE_BIT26)                         // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define SDCARD_DETECT          (PORTE_BIT28)                         // {2} if the port is changed (eg. A to B) the port macros will require appropriate adjustment too

    #if defined SDCARD_SUPPORT || defined SPI_FLASH_FAT                  // {2}
        #define INIT_WATCHDOG_LED() _CONFIG_DRIVE_PORT_OUTPUT_VALUE(A, (BLINK_LED), (BLINK_LED), (PORT_SRE_SLOW | PORT_DSE_HIGH)); _CONFIG_PORT_INPUT(E, (SWITCH_2 | SDCARD_DETECT), PORT_PS_UP_ENABLE); // note that the force boot input is configured here and not with the INIT_WATCHDOG_DISABLE() since the watchdog must be disabled as quickly as possible
    #else
        #if defined K60F150M_50M
            #define INIT_WATCHDOG_LED() _CONFIG_DRIVE_PORT_OUTPUT_VALUE(B, (BLINK_LED), (BLINK_LED), (PORT_SRE_SLOW | PORT_DSE_HIGH))
        #else
            #define INIT_WATCHDOG_LED() _CONFIG_DRIVE_PORT_OUTPUT_VALUE(A, (BLINK_LED), (BLINK_LED), (PORT_SRE_SLOW | PORT_DSE_HIGH)); _CONFIG_PORT_INPUT(E, SWITCH_2, PORT_PS_UP_ENABLE); // note that the force boot input is configured here and not with the INIT_WATCHDOG_DISABLE() since the watchdog must be disabled as quickly as possible
        #endif
    #endif

    #if defined K60F150M_50M
        #define INIT_WATCHDOG_DISABLE()
        #define WATCHDOG_DISABLE()      1
    #elif defined TWR_K70F120M
        #define INIT_WATCHDOG_DISABLE() _CONFIG_PORT_INPUT_FAST_LOW(D, SWITCH_1, PORT_PS_UP_ENABLE); // configure as input
        #define WATCHDOG_DISABLE()      (_READ_PORT_MASK(D, SWITCH_1) == 0) // pull this input down to disable watchdog (hold SW1 at reset)
    #else
        #define INIT_WATCHDOG_DISABLE() _CONFIG_PORT_INPUT_FAST_HIGH(A, SWITCH_1, PORT_PS_UP_ENABLE); // configure as input
        #define WATCHDOG_DISABLE()      (_READ_PORT_MASK(A, SWITCH_1) == 0) // pull this input down to disable watchdog (hold SW1 at reset)
    #endif
    #if defined K60F150M_50M
        #define TOGGLE_WATCHDOG_LED()   _TOGGLE_PORT(B, BLINK_LED)
    #else
        #define TOGGLE_WATCHDOG_LED()   _TOGGLE_PORT(A, BLINK_LED)
    #endif

    #if defined SDCARD_SUPPORT || defined SPI_FLASH_FAT                  // {2}
        #define FORCE_BOOT()       ((_READ_PORT_MASK(E, SWITCH_2) == 0) || (_READ_PORT_MASK(E, SDCARD_DETECT) == 0)) // hold SW2 at reset or with inserted SD card
    #elif defined K60F150M_50M
        #define FORCE_BOOT()       1
    #else
        #define FORCE_BOOT()       (_READ_PORT_MASK(E, SWITCH_2) == 0)   // pull this input down to force boot loader mode (hold SW2 at reset)
    #endif
    #define RETAIN_LOADER_MODE()   (_READ_PORT_MASK(E, SWITCH_2) == 0)

    #define SD_CONTROLLER_AVAILABLE                                      // use SDHC controller rather than SPI
    #if defined TWR_K60N512 || defined TWR_K60D100M                      // TWR_K60F120M/TWR_K70F120M have no write protect input on the SD card socket
        #define WRITE_PROTECT_INPUT     PORTE_BIT27
    #endif
    #if defined SD_CONTROLLER_AVAILABLE
        #define SET_SD_CS_HIGH()                                         // dummy for compatibility
        #define SET_SD_CS_LOW()                                          // dummy for compatibility
        #if defined TWR_K60N512 || defined TWR_K60D100M
            #ifdef _WINDOWS
                #define POWER_UP_SD_CARD()    _CONFIG_PORT_INPUT(E, (WRITE_PROTECT_INPUT), (PORT_PS_UP_ENABLE)); SDHC_SYSCTL |= SDHC_SYSCTL_INITA; SDHC_SYSCTL &= ~SDHC_SYSCTL_INITA; // apply power to the SD card if appropriate (we use this to send 80 clocks - self-clearing bit)
            #else
                #define POWER_UP_SD_CARD()    _CONFIG_PORT_INPUT(E, (WRITE_PROTECT_INPUT), (PORT_PS_UP_ENABLE)); SDHC_SYSCTL |= SDHC_SYSCTL_INITA; while (SDHC_SYSCTL & SDHC_SYSCTL_INITA) {}; // apply power to the SD card if appropriate (we use this to send 80 clocks)
            #endif
        #else
            #ifdef _WINDOWS
                #define POWER_UP_SD_CARD()    SDHC_SYSCTL |= SDHC_SYSCTL_INITA; SDHC_SYSCTL &= ~SDHC_SYSCTL_INITA; // apply power to the SD card if appropriate (we use this to send 80 clocks - self-clearing bit)
            #else
                #define POWER_UP_SD_CARD()    SDHC_SYSCTL |= SDHC_SYSCTL_INITA; while (SDHC_SYSCTL & SDHC_SYSCTL_INITA) {}; // apply power to the SD card if appropriate (we use this to send 80 clocks)
            #endif
        #endif
        #if defined TWR_K60F120M || defined TWR_K70F120M
            #define SDHC_SYSCTL_SPEED_SLOW        (SDHC_SYSCTL_SDCLKFS_64 | SDHC_SYSCTL_DVS_5) // 375kHz when 120MHz clock
            #define SDHC_SYSCTL_SPEED_FAST        (SDHC_SYSCTL_SDCLKFS_2 | SDHC_SYSCTL_DVS_3) // 20MHz when 120MHz clock
        #else
            #define SDHC_SYSCTL_SPEED_SLOW        (SDHC_SYSCTL_SDCLKFS_128 | SDHC_SYSCTL_DVS_2) // 390kHz when 100MHz clock
            #define SDHC_SYSCTL_SPEED_FAST        (SDHC_SYSCTL_SDCLKFS_2 | SDHC_SYSCTL_DVS_2)  // 25MHz when 100MHz clock
        #endif
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
        #ifdef _WINDOWS
            #define WRITE_SPI_CMD(byte)     SPI1_SR &= ~(SPI_SR_RFDF); SPI1_PUSHR = (byte | SPI_PUSHR_PCS_NONE | SPI_PUSHR_CTAS_CTAR0); SPI1_POPR = _fnSimSD_write((unsigned char)byte)
            #define WAIT_TRANSMISSON_END() while (!(SPI1_SR & (SPI_SR_RFDF))) { SPI1_SR |= (SPI_SR_RFDF); }
            #define READ_SPI_DATA()        (unsigned char)SPI1_POPR
        #else
            #define WRITE_SPI_CMD(byte)    SPI1_SR = (SPI_SR_RFDF); SPI1_PUSHR = (byte | SPI_PUSHR_PCS_NONE | SPI_PUSHR_CTAS_CTAR0) // clear flags before transmitting (and receiving) a single byte
            #define WAIT_TRANSMISSON_END() while (!(SPI1_SR & (SPI_SR_RFDF))) {}
            #define READ_SPI_DATA()        (unsigned char)SPI1_POPR
        #endif
        #define SET_SD_DI_CS_HIGH()  _SETBITS(E, SPI_CS1_0)              // force DI and CS lines high ready for the initialisation sequence
        #define SET_SD_CS_LOW()      _CLEARBITS(E, SPI_CS1_0)            // assert the CS line of the SD card to be read
        #define SET_SD_CS_HIGH()     _SETBITS(E, SPI_CS1_0)              // negate the CS line of the SD card to be read
        #define POWER_UP_SD_CARD()                                       // apply power to the SD card if appropriate
    #endif

    #define POWER_DOWN_SD_CARD()                                         // remove power from SD card interface
    #if defined TWR_K60F120M || defined TWR_K70F120M
        #define GET_SDCARD_WP_STATE() 0                                  // TWR_K60F120M/TWR_K70F120M have no write protect input on the SD card socket
    #else
        #define GET_SDCARD_WP_STATE() (_READ_PORT_MASK(E, WRITE_PROTECT_INPUT)) // when the input is read as '1' the card is protected from writes
    #endif

    #define DEL_USB_SYMBOL()                                             // control display of USB enumeration - clear
    #define SET_USB_SYMBOL()                                             // control display of USB enumeration - set

    #if defined TWR_K60D100M
        #define USB_HOST_POWER_ENABLE   PORTB_BIT8
        #define USB_HOST_POWER_CONFIG() _CONFIG_DRIVE_PORT_OUTPUT_VALUE_FAST_LOW(B, (USB_HOST_POWER_ENABLE), (0), (PORT_SRE_SLOW | PORT_DSE_HIGH))
        #define USB_HOST_POWER_ON()     _SETBITS(B, USB_HOST_POWER_ENABLE)
        #define USB_HOST_POWER_OFF()    _CLEARBITS(B, USB_HOST_POWER_ENABLE)
    #endif
#elif defined K70F150M_12M
    #define BLUE_LED               (PORTA_BIT16)                         // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define GREEN_LED              (PORTA_BIT17)                         // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define RED_LED                (PORTA_BIT24)                         // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define BLINK_LED              (GREEN_LED)
    #define SWITCH_2               (PORTA_BIT26)                         // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define SWITCH_3               (PORTA_BIT25)                         // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too

    #define INIT_WATCHDOG_LED()    _CONFIG_DRIVE_PORT_OUTPUT_VALUE(A, (BLINK_LED | PORTA_BIT6), (0), (PORT_SRE_SLOW | PORT_DSE_HIGH))

    #define INIT_WATCHDOG_DISABLE() _CONFIG_PORT_INPUT_FAST_HIGH(A, (SWITCH_2 | SWITCH_3), PORT_PS_UP_ENABLE); // configure as input
    #define WATCHDOG_DISABLE()     (_READ_PORT_MASK(A, SWITCH_3) == 0)   // pull this input down to disable watchdog (hold S3 at reset)
    #define FORCE_BOOT()           (_READ_PORT_MASK(A, SWITCH_2) == 0)   // pull this input down to force boot loader mode (hold S2 at reset)
    #define TOGGLE_WATCHDOG_LED()  _TOGGLE_PORT(A, BLINK_LED)
#elif defined TWR_K21F120M
    #define BLINK_LED              (PORTD_BIT4)
    #define SWITCH_2               (PORTC_BIT7)                          // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define SWITCH_3               (PORTC_BIT6)                          // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define SDCARD_DETECT          (PORTC_BIT18)                         // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define USB_HOST_POWER_ENABLE  (PORTC_BIT9)

    #if defined SDCARD_SUPPORT
        #define INIT_WATCHDOG_LED() _CONFIG_DRIVE_PORT_OUTPUT_VALUE(D, (BLINK_LED), (BLINK_LED), (PORT_SRE_SLOW | PORT_DSE_HIGH)); _CONFIG_PORT_INPUT(C, (SDCARD_DETECT), PORT_PS_UP_ENABLE); // note that the force boot input is configured here and not with the INIT_WATCHDOG_DISABLE() since the watchdog must be disabled as quickly as possible
    #else
        #define INIT_WATCHDOG_LED() _CONFIG_DRIVE_PORT_OUTPUT_VALUE(D, (BLINK_LED), (BLINK_LED), (PORT_SRE_SLOW | PORT_DSE_HIGH))
    #endif
    #define INIT_WATCHDOG_DISABLE() _CONFIG_PORT_INPUT_FAST_LOW(C, (SWITCH_3 | SWITCH_2), PORT_PS_UP_ENABLE); // configure as input

    #define WATCHDOG_DISABLE()     (_READ_PORT_MASK(C, SWITCH_2) == 0)   // pull this input down to disable watchdog (hold SW3 at reset)
    #if defined SDCARD_SUPPORT
        #define FORCE_BOOT()       ((_READ_PORT_MASK(C, SWITCH_3) == 0) || (_READ_PORT_MASK(C, SDCARD_DETECT) == 0)) // pull this input down to force boot loader mode (hold SW2 at reset) or with inserted SD card
    #else
        #define FORCE_BOOT()       (_READ_PORT_MASK(C, SWITCH_3) == 0)       // pull this input down to force boot loader mode (hold SW2 at reset)
    #endif
    #define RETAIN_LOADER_MODE() (_READ_PORT_MASK(C, SWITCH_3) == 0)

    #define TOGGLE_WATCHDOG_LED()   _TOGGLE_PORT(D, BLINK_LED)

    #define SD_CONTROLLER_AVAILABLE                                      // use SDHC controller rather than SPI
    #define SET_SD_CS_HIGH()                                             // dummy with SDHC controller
    #define SET_SD_CS_LOW()                                              // dummy with SDHC controller

    #ifdef _WINDOWS
        #define POWER_UP_SD_CARD()    SDHC_SYSCTL |= SDHC_SYSCTL_INITA; SDHC_SYSCTL &= ~SDHC_SYSCTL_INITA; // apply power to the SD card if appropriate (we use this to send 80 clocks - self-clearing bit)
    #else
        #define POWER_UP_SD_CARD()    SDHC_SYSCTL |= SDHC_SYSCTL_INITA; while (SDHC_SYSCTL & SDHC_SYSCTL_INITA) {}; // apply power to the SD card if appropriate (we use this to send 80 clocks)
    #endif

    #define SDHC_SYSCTL_SPEED_SLOW    (SDHC_SYSCTL_SDCLKFS_64 | SDHC_SYSCTL_DVS_5) // 375kHz when 120MHz clock
    #define SDHC_SYSCTL_SPEED_FAST    (SDHC_SYSCTL_SDCLKFS_2 | SDHC_SYSCTL_DVS_3) // 20MHz when 120MHz clock
    #define SET_SPI_SD_INTERFACE_FULL_SPEED() fnSetSD_clock(SDHC_SYSCTL_SPEED_FAST); SDHC_PROCTL |= SDHC_PROCTL_DTW_4BIT

    #define POWER_DOWN_SD_CARD()                                         // remove power from SD card interface

    #define GET_SDCARD_WP_STATE()  1                                     // always write protect during boot loading

    #define USB_HOST_POWER_CONFIG() _CONFIG_DRIVE_PORT_OUTPUT_VALUE_FAST_LOW(C, (USB_HOST_POWER_ENABLE), (0), (PORT_SRE_SLOW | PORT_DSE_HIGH))
    #define USB_HOST_POWER_ON()     _SETBITS(C, USB_HOST_POWER_ENABLE)
    #define USB_HOST_POWER_OFF()    _CLEARBITS(C, USB_HOST_POWER_ENABLE)

    #define DEL_USB_SYMBOL()
    #define SET_USB_SYMBOL()
#elif defined tinyK22
    #define LED_BLUE               (PORTC_BIT2)                          // blue LED - if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define SWITCH_1               (PORTB_BIT17)                         // switch 1 - if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define SWITCH_2               (PORTC_BIT11)                         // switch 2 - if the port is changed (eg. A to B) the port macros will require appropriate adjustment too

    #define INIT_WATCHDOG_LED()    _CONFIG_DRIVE_PORT_OUTPUT_VALUE(C, (LED_BLUE), (LED_BLUE), (PORT_SRE_SLOW | PORT_DSE_HIGH)); _CONFIG_PORT_INPUT_FAST_LOW(C, (SWITCH_1), PORT_PS_UP_ENABLE);
    #define INIT_WATCHDOG_DISABLE() _CONFIG_PORT_INPUT_FAST_HIGH(B, (SWITCH_1), PORT_PS_UP_ENABLE) // configure as input

    #define WATCHDOG_DISABLE()     (_READ_PORT_MASK(C, SWITCH_2) == 0)   // pull this input low at reset to disable watchdog
    #define FORCE_BOOT()           (_READ_PORT_MASK(B, SWITCH_1) == 0)   // pull this input low at reset to force the boot loader
    #define TOGGLE_WATCHDOG_LED()   _TOGGLE_PORT(C, LED_BLUE)

#elif defined FRDM_K22F
    #define LED_GREEN              (PORTA_BIT2)                          // green LED - if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define LED_BLUE               (PORTD_BIT5)                          // blue LED - if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define LED_RED                (PORTA_BIT1)                          // red LED - if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define SWITCH_2               (PORTC_BIT1)                          // switch 2 - if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define SWITCH_3               (PORTB_BIT7)                          // switch 3 - if the port is changed (eg. A to B) the port macros will require appropriate adjustment too

    #define BLINK_LED              (LED_GREEN)
    #define SDCARD_DETECT          (PORTB_BIT6)                          // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too

    #if defined SDCARD_SUPPORT || defined SPI_FLASH_FAT
        #define INIT_WATCHDOG_LED() _CONFIG_DRIVE_PORT_OUTPUT_VALUE(A, (BLINK_LED), (BLINK_LED), (PORT_SRE_SLOW | PORT_DSE_HIGH)); _CONFIG_PORT_INPUT(B, (SDCARD_DETECT), PORT_PS_DOWN_ENABLE); // note that the force boot input is configured here and not with the INIT_WATCHDOG_DISABLE() since the watchdog must be disabled as quickly as possible
    #else
        #define INIT_WATCHDOG_LED() _CONFIG_DRIVE_PORT_OUTPUT_VALUE(A, (BLINK_LED), (BLINK_LED), (PORT_SRE_SLOW | PORT_DSE_HIGH))
    #endif
    #define INIT_WATCHDOG_DISABLE() _CONFIG_PORT_INPUT_FAST_LOW(C, (SWITCH_2), PORT_PS_UP_ENABLE); _CONFIG_PORT_INPUT_FAST_LOW(B, (SWITCH_3), PORT_PS_UP_ENABLE); // configure as input

    #define WATCHDOG_DISABLE()     (!_READ_PORT_MASK(B, SWITCH_3))       // pull this input down to disable watchdog (hold SW3 at reset)
    #if defined SDCARD_SUPPORT || defined SPI_FLASH_FAT
        #define FORCE_BOOT()       ((!_READ_PORT_MASK(C, SWITCH_2)) || (_READ_PORT_MASK(B, SDCARD_DETECT))) // pull this input down to force boot loader mode (hold SW2 at reset) or with inserted SD card
        #define RETAIN_LOADER_MODE()   (!_READ_PORT_MASK(C, SWITCH_2))
    #else
        #define FORCE_BOOT()       (!_READ_PORT_MASK(C, SWITCH_2))       // pull this input down to force boot loader mode (hold SW2 at reset)
    #endif

    #define TOGGLE_WATCHDOG_LED()   _TOGGLE_PORT(A, BLINK_LED)

    // Configure to suit SD card SPI mode at between 100k and 400k
    //
    #define SPI_CS1_0              PORTC_BIT4
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
    #define SET_SD_DI_CS_HIGH()  _SETBITS(C, SPI_CS1_0)                  // force DI and CS lines high ready for the initialisation sequence
    #define SET_SD_CS_LOW()      _CLEARBITS(C, SPI_CS1_0)                // assert the CS line of the SD card to be read
    #define SET_SD_CS_HIGH()     _SETBITS(C, SPI_CS1_0)                  // negate the CS line of the SD card to be read
    #define POWER_UP_SD_CARD()                                           // apply power to the SD card if appropriate

    #define POWER_DOWN_SD_CARD()                                         // remove power from SD card interface
    #define GET_SDCARD_WP_STATE() 0                                      // no write protect switch available
    #define SDCARD_DETECTION()    0                                      // card detection input not present
#elif defined BLAZE_K22
    #define EXT_IO0                (PORTA_BIT0)                          // warning - this pin is JTAG_TCLK
    #define EXT_IO1                (PORTA_BIT1)                          // IoT UART; warning - this pin is JTAG_TDI
    #define EXT_IO2                (PORTA_BIT2)                          // IoT UART; warning - this pin is JTAG_TDO
    #define EXT_IO3                (PORTA_BIT3)                          // warning - this pin is JTAG_TMS
    #define EXT_IO4                (PORTB_BIT18)
    #define EXT_IO5                (PORTB_BIT19)
    #define EXT_IO6                (PORTC_BIT1)
    #define EXT_IO7                (PORTC_BIT2)
    #define EXT_IO8                (PORTC_BIT3)
    #define EXT_IO9                (PORTC_BIT4)
    #define EXT_IO10               (PORTE_BIT0)                          // IoT UART
    #define EXT_IO11               (PORTE_BIT1)                          // IoT UART
    #define TSI_RESET_LINE         (PORTB_BIT2)
    #define TSI_INTERRUPT_LINE     (PORTB_BIT3)
    #define SDCARD_DETECT_PIN      (PORTD_BIT7)                          // '0' when SD card is inserted
    #define SYS_LED_GREEN          (PORTB_BIT16)

    #define DEMO_LED_1             (SYS_LED_GREEN)                       // (green led) if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define DEMO_LED_2             (EXT_IO4)                             // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define DEMO_LED_3             (EXT_IO5)                             // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define DEMO_LED_4             (EXT_IO6)                             // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define BLINK_LED              (DEMO_LED_1)
    #define SWITCH_1               (EXT_IO7)                             // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define SWITCH_2               (EXT_IO8)                             // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define WRITE_PROTECT_INPUT    (0)
    #define USB_HOST_POWER_ENABLE  (0)

    #define SWITCH_1_PORT          _PORTC
    #define SWITCH_2_PORT          _PORTC

    #define FORCE_BOOT()            1                                    // always start the boot loader

    #define INIT_WATCHDOG_LED() _CONFIG_DRIVE_PORT_OUTPUT_VALUE(B, (BLINK_LED), (BLINK_LED), (PORT_SRE_SLOW | PORT_DSE_HIGH)); _CONFIG_DRIVE_PORT_OUTPUT_VALUE(B, (DEMO_LED_2), (DEMO_LED_2), (PORT_SRE_SLOW | PORT_DSE_HIGH))

    #define SHIFT_DEMO_LED_1        16                                    // since the port bits may be spread out shift each to the lowest 4 bits
    #define SHIFT_DEMO_LED_2        17
    #define SHIFT_DEMO_LED_3        17
    #define SHIFT_DEMO_LED_4        2

    #define MAPPED_DEMO_LED_1       (DEMO_LED_1 >> SHIFT_DEMO_LED_1)
    #define MAPPED_DEMO_LED_2       (DEMO_LED_2 << SHIFT_DEMO_LED_2)
    #define MAPPED_DEMO_LED_3       (DEMO_LED_3 >> SHIFT_DEMO_LED_3)
    #define MAPPED_DEMO_LED_4       (DEMO_LED_4 << SHIFT_DEMO_LED_4)

    #define INIT_WATCHDOG_DISABLE() _CONFIG_PORT_INPUT_FAST_LOW(C, (SWITCH_1 | SWITCH_2), PORT_PS_UP_ENABLE) // use fast access version (beware that this can only operate on half of the 32 bits at a time)
    #define WATCHDOG_DISABLE()      (_READ_PORT_MASK(C, SWITCH_2) == 0)  // pull this input down to disable watchdog (hold SW2 at reset)
    #define TOGGLE_WATCHDOG_LED()   _TOGGLE_PORT(B, BLINK_LED)

    #define CONFIG_TEST_OUTPUT()                                         // we use DEMO_LED_2 which is configured by the user code (and can be disabled in parameters if required)
    #define TOGGLE_TEST_OUTPUT()    _TOGGLE_PORT(B, DEMO_LED_2)
    #define SET_TEST_OUTPUT()       _SETBITS(B, DEMO_LED_2)
    #define CLEAR_TEST_OUTPUT()     _CLEARBITS(B, DEMO_LED_2)

    #define TOUCHRESET_H()          _SETBITS(B, TSI_RESET_LINE)          // take touch screen controller out of reset
    #define TC_INT_PRIORTY          PRIORITY_PORT_B_INT                  // touch screen interrupt priority level
    #define TC_INT_PORT             PORTB                                // the port that the touch screen interrupt input is on
    #define TC_INT_PORT_BIT         TSI_INTERRUPT_LINE                   // the touch screen interrupt input

    #define PEN_DOWN_ACTION()       fnInterruptMessage(TASK_APPLICATION, USER_FORCE_LOADER) // if the touch screen is tapped before the application starts the USB loader will be activated

    #define BUTTON_KEY_DEFINITIONS  {_PORTA, EXT_IO0, {544,   4, 633,  30}}, \
                                    {_PORTA, EXT_IO1, {544,  39, 633,  69}}, \
                                    {_PORTA, EXT_IO2, {544,  77, 633, 106}}, \
                                    {_PORTA, EXT_IO3, {544, 114, 633, 144}}, \
                                    {_PORTB, EXT_IO4, {544, 191, 633, 220}}, \
                                    {_PORTB, EXT_IO5, {544, 228, 633, 258}}, \
                                    {_PORTC, EXT_IO6, {544, 266, 633, 294}}, \
                                    {_PORTC, EXT_IO7, {544, 304, 633, 333}}, \
                                    {_PORTC, EXT_IO8, {544, 341, 633, 372}}, \
                                    {_PORTC, EXT_IO9, {544, 381, 633, 409}}, \
                                    {_PORTE, EXT_IO10,{7,   341,  94, 372}}, \
                                    {_PORTE, EXT_IO11,{7,   381,  94, 409}},

        // '0'           '1'           input state   center (x,   y)   0 = circle, radius, controlling port, controlling pin 
    #define KEYPAD_LED_DEFINITIONS \
        {RGB(0, 255, 0), RGB(0,   0,   0), 1, {425, 56,  0, 7 }, _PORTB, SYS_LED_GREEN}, \
        {RGB(0, 0, 0),   RGB(255, 255, 0), 0, {472, 114, 0, 4 }, _PORTA, EXT_IO0}, \
        {RGB(0, 0, 0),   RGB(255, 255, 0), 0, {472, 135, 0, 4 }, _PORTA, EXT_IO1}, \
        {RGB(0, 0, 0),   RGB(255, 255, 0), 0, {472, 155, 0, 4 }, _PORTA, EXT_IO2}, \
        {RGB(0, 0, 0),   RGB(255, 255, 0), 0, {472, 175, 0, 4 }, _PORTA, EXT_IO3}, \
        {RGB(0, 0, 0),   RGB(255, 255, 0), 0, {472, 215, 0, 4 }, _PORTB, EXT_IO4}, \
        {RGB(0, 0, 0),   RGB(255, 255, 0), 0, {472, 235, 0, 4 }, _PORTB, EXT_IO5}, \
        {RGB(0, 0, 0),   RGB(255, 255, 0), 0, {472, 255, 0, 4 }, _PORTC, EXT_IO6}, \
        {RGB(0, 0, 0),   RGB(255, 255, 0), 0, {472, 275, 0, 4 }, _PORTC, EXT_IO7}, \
        {RGB(0, 0, 0),   RGB(255, 255, 0), 0, {472, 295, 0, 4 }, _PORTC, EXT_IO8}, \
        {RGB(0, 0, 0),   RGB(255, 255, 0), 0, {472, 315, 0, 4 }, _PORTC, EXT_IO9}, \
        {RGB(0, 0, 0),   RGB(255, 255, 0), 0, {168, 295, 0, 4 }, _PORTE, EXT_IO10}, \
        {RGB(0, 0, 0),   RGB(255, 255, 0), 0, {168, 315, 0, 4 }, _PORTE, EXT_IO11},

    #define KEYPAD "../../uTaskerV1.4/Simulator/KeyPads/blaze.bmp"
    #define LCD_ON_KEYPAD                                                // simulator positions the LCD on top of the key pad/panel
        #define LCD_X_OFFSET       252
        #define LCD_Y_OFFSET       50

    #if defined _WINDOWS
        extern unsigned short _ucCommand;
        #define GLCD_COMMAND_ADDR     &_ucCommand
        #define GLCD_DATA_ADDR        &_ucData
    #else
        #define GLCD_COMMAND_ADDR     0x60000000                         // write address
        #define GLCD_DATA_ADDR        0x60010000                         // read address
    #endif

    #define GLCD_RESET_LINE           (PORTA_BIT12)
    #define GLCD_BACK_LIGHT           (PORTA_BIT5)

    #define BACK_LIGHT_MAX_INTENSITY() _CONFIG_DRIVE_PORT_OUTPUT_VALUE(A, GLCD_BACK_LIGHT, GLCD_BACK_LIGHT, PORT_SRE_SLOW)
    #define BACK_LIGHT_MIN_INTENSITY() _CONFIG_DRIVE_PORT_OUTPUT_VALUE(A, GLCD_BACK_LIGHT, 0, PORT_SRE_SLOW)
    #define ENABLE_BACKLIGHT()

    #define FTM_DEBUG_BEHAVIOUR       FTM_CONF_BDMMODE_3                 // allow timer to continue operating when debugging
    #define _GLCD_BACKLIGHT_PWM_FREQUENCY  PWM_FREQUENCY(1000, 16)       // 1000Hz PWM with divide by 16 prescaler
    #define BACK_LIGHT_INTENSITY()    POWER_UP_ATOMIC(6, FTM0); \
                                      _CONFIG_PERIPHERAL(A, 5, (PA_5_FTM0_CH2 | PORT_SRE_FAST | PORT_DSE_HIGH)); \
                                      FTM0_CONF = FTM_DEBUG_BEHAVIOUR; \
                                      FTM0_C2SC = FTM_CSC_MS_ELS_PWM_HIGH_TRUE_PULSES; \
                                      FTM0_CNTIN = 0; \
                                      FTM0_MOD = (_GLCD_BACKLIGHT_PWM_FREQUENCY - 1); \
                                      FTM0_C2V = _PWM_PERCENT(35, _GLCD_BACKLIGHT_PWM_FREQUENCY); \
                                      FTM0_SC = (PWM_SYS_CLK | PWM_PRESCALER_16)

    // BLAZE uses 16 bit FlexBus interface using RS, RD and WR. The address range is set to 128K because the DC signal is connected on address wire. FlexBus setup as fast as possible in multiplexed mode
    // the 8 bit data appears at AD0..AD7
    //
    #define CONFIGURE_GLCD()        BACK_LIGHT_MIN_INTENSITY(); \
                                    if (IS_POWERED_UP(6, FTM0) != 0) {FTM0_SC = 0;} \
                                    _CONFIG_DRIVE_PORT_OUTPUT_VALUE_FAST_LOW(A, GLCD_RESET_LINE, 0, (PORT_SRE_SLOW | PORT_DSE_LOW)); \
                                    _CONFIG_DRIVE_PORT_OUTPUT_VALUE(B, (TSI_RESET_LINE), (0), (PORT_SRE_FAST | PORT_DSE_LOW)); \
                                    POWER_UP_ATOMIC(7, FLEXBUS); \
                                    SIM_SOPT2 |= SIM_SOPT2_FBSL_ALL; \
                                    _CONFIG_PERIPHERAL(B, 17, (PB_17_FB_AD16 | PORT_DSE_HIGH | PORT_PS_DOWN_ENABLE)); \
                                    _CONFIG_PERIPHERAL(C, 11, (PC_11_FB_RW | PORT_DSE_HIGH | PORT_PS_DOWN_ENABLE)); \
                                    _CONFIG_PERIPHERAL(D, 0,  (PD_0_FB_ALE | PORT_DSE_HIGH | PORT_PS_DOWN_ENABLE)); \
                                    _CONFIG_PERIPHERAL(C, 8,  (PC_8_FB_AD7 | PORT_DSE_HIGH | PORT_PS_DOWN_ENABLE)); \
                                    _CONFIG_PERIPHERAL(C, 9,  (PC_9_FB_AD6 | PORT_DSE_HIGH | PORT_PS_DOWN_ENABLE)); \
                                    _CONFIG_PERIPHERAL(C, 10, (PC_10_FB_AD5 | PORT_DSE_HIGH | PORT_PS_DOWN_ENABLE)); \
                                    _CONFIG_PERIPHERAL(D, 2,  (PD_2_FB_AD4 | PORT_DSE_HIGH | PORT_PS_DOWN_ENABLE)); \
                                    _CONFIG_PERIPHERAL(D, 3,  (PD_3_FB_AD3 | PORT_DSE_HIGH | PORT_PS_DOWN_ENABLE)); \
                                    _CONFIG_PERIPHERAL(D, 4,  (PD_4_FB_AD2 | PORT_DSE_HIGH | PORT_PS_DOWN_ENABLE)); \
                                    _CONFIG_PERIPHERAL(D, 5,  (PD_5_FB_AD1 | PORT_DSE_HIGH | PORT_PS_DOWN_ENABLE)); \
                                    _CONFIG_PERIPHERAL(D, 6,  (PD_6_FB_AD0 | PORT_DSE_HIGH | PORT_PS_DOWN_ENABLE)); \
                                    _CONFIG_PERIPHERAL(D, 1,  (PD_1_FB_CS0 | PORT_DSE_HIGH | PORT_PS_DOWN_ENABLE)); \
                                    _CONFIG_PERIPHERAL(D, 0,  (PD_0_FB_ALE | PORT_DSE_HIGH | PORT_PS_DOWN_ENABLE)); \
                                    CSAR0  = (unsigned long)GLCD_COMMAND_ADDR; \
  	                                CSCR0  = (FB_BLS | FB_AA | PORT_SIZE_16); \
  	                                CSMR0  = (FB_CS_VALID | 0x10000);

    #define GLCD_RST_H()            _SETBITS(A, GLCD_RESET_LINE)

    #if defined _WINDOWS
        #define MAX_GLCD_WRITE_BURST   10000                             // the maximum number of writes to the GLCD before the task yields
    #else
        #define MAX_GLCD_WRITE_BURST   1000                              // the maximum number of writes to the GLCD before the task yields
    #endif

    #define SUPPORT_PORT_INTERRUPTS                                      // port interrupts needed for touch screen
        #define NO_PORT_INTERRUPTS_PORTA                                 // remove non-needed ports
        #define NO_PORT_INTERRUPTS_PORTC
        #define NO_PORT_INTERRUPTS_PORTD
        #define NO_PORT_INTERRUPTS_PORTE

#elif defined TWR_K24F120M
    #define LED_YELLOW             (PORTD_BIT4)                          // (yellow led) if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define LED_RED                (PORTD_BIT5)                          // (red led) if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define LED_ORANGE             (PORTD_BIT6)                          // (orange led) if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define LED_GREEN              (PORTD_BIT7)                          // (green led) if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define BLINK_LED              (LED_ORANGE)
    #define SWITCH_1               (PORTC_BIT7)                          // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define SWITCH_2               (PORTC_BIT6)                          // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too

    #define SWITCH_1_PORT          _PORTC
    #define SWITCH_2_PORT          _PORTC

    #define INIT_WATCHDOG_LED() _CONFIG_DRIVE_PORT_OUTPUT_VALUE(D, (BLINK_LED), (BLINK_LED), (PORT_SRE_SLOW | PORT_DSE_HIGH))

    #define INIT_WATCHDOG_DISABLE() _CONFIG_PORT_INPUT_FAST_LOW(C, (SWITCH_1 | SWITCH_2), PORT_PS_UP_ENABLE) // use fast access version (beware that this can only operate on half of the 32 bits at a time)
    #define WATCHDOG_DISABLE()      (!_READ_PORT_MASK(C, SWITCH_2))      // pull this input down to disable watchdog (hold SW2 at reset)
    #define TOGGLE_WATCHDOG_LED()   _TOGGLE_PORT(D, BLINK_LED)

    #define FORCE_BOOT()           (!_READ_PORT_MASK(C, SWITCH_1))       // pull this input down to force boot loader mode (hold SW1 at reset)
#elif defined TWR_K22F120M
    #define LED_GREEN              (PORTD_BIT4)                          // green LED - if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define LED_BLUE               (PORTD_BIT7)                          // blue LED - if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define LED_YELLOW             (PORTD_BIT5)                          // yellow LED - if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define LED_ORANGE             (PORTD_BIT6)                          // orange LED - if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define SWITCH_1               (PORTC_BIT6)                          // switch 1 - if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define SWITCH_3               (PORTC_BIT7)                          // switch 3 - if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define USB_HOST_POWER_ENABLE  (PORTC_BIT9)

    #define BLINK_LED              (LED_GREEN)

    #define INIT_WATCHDOG_LED()    _CONFIG_DRIVE_PORT_OUTPUT_VALUE(D, (BLINK_LED), (BLINK_LED), (PORT_SRE_SLOW | PORT_DSE_HIGH))

    #define INIT_WATCHDOG_DISABLE() _CONFIG_PORT_INPUT_FAST_LOW(C, (SWITCH_1 | SWITCH_3), PORT_PS_UP_ENABLE) // configure as inputs

    #define WATCHDOG_DISABLE()     (_READ_PORT_MASK(C, SWITCH_3) == 0)   // pull this input down to disable watchdog (hold SW1 at reset)
    #define FORCE_BOOT()           (_READ_PORT_MASK(C, SWITCH_1) == 0)   // pull this input down to force boot loader mode (hold SW1 at reset)
    #define RETAIN_LOADER_MODE()   (_READ_PORT_MASK(C, SWITCH_1) == 0)

    #define TOGGLE_WATCHDOG_LED()   _TOGGLE_PORT(D, BLINK_LED)

    // Configure to suit SD card SPI mode at between 100k and 400k
    //
    #define SPI_CS1_0              PORTC_BIT4
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
    #define SET_SD_DI_CS_HIGH()  _SETBITS(C, SPI_CS1_0)                  // force DI and CS lines high ready for the initialisation sequence
    #define SET_SD_CS_LOW()      _CLEARBITS(C, SPI_CS1_0)                // assert the CS line of the SD card to be read
    #define SET_SD_CS_HIGH()     _SETBITS(C, SPI_CS1_0)                  // negate the CS line of the SD card to be read
    #define POWER_UP_SD_CARD()                                           // apply power to the SD card if appropriate

    #define POWER_DOWN_SD_CARD()                                         // remove power from SD card interface
    #define GET_SDCARD_WP_STATE() 0                                      // no write protect switch available
    #define SDCARD_DETECTION()    0                                      // card detection input not present

    #define DEL_USB_SYMBOL()                                             // control display of USB enumeration - clear
    #define SET_USB_SYMBOL()                                             // control display of USB enumeration - set

    #define USB_HOST_POWER_CONFIG() _CONFIG_DRIVE_PORT_OUTPUT_VALUE_FAST_LOW(C, (USB_HOST_POWER_ENABLE), (0), (PORT_SRE_SLOW | PORT_DSE_HIGH))
    #define USB_HOST_POWER_ON()     _SETBITS(C, USB_HOST_POWER_ENABLE)
    #define USB_HOST_POWER_OFF()    _CLEARBITS(C, USB_HOST_POWER_ENABLE)
#elif defined TWR_K65F180M || defined K26FN2_180
    #define LED_ORANGE         (PORTB_BIT5)                              // green LED - if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define LED_YELLOW         (PORTB_BIT4)                              // yellow LED - if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define SWITCH_2           (PORTA_BIT4)                              // switch 2 - if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define SWITCH_3           (PORTA_BIT10)                             // switch 3 - if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define USB_HOST_POWER_ENABLE (PORTD_BIT8)
    #define SDCARD_DETECT      (PORTA_BIT9)
    #define BLINK_LED          (LED_YELLOW)

    #define INIT_WATCHDOG_LED()    _CONFIG_DRIVE_PORT_OUTPUT_VALUE(B, (BLINK_LED), (BLINK_LED), (PORT_SRE_SLOW | PORT_DSE_HIGH))
    #if defined SDCARD_SUPPORT || defined SPI_FLASH_FAT
        #define FORCE_BOOT()       (_READ_PORT_MASK(A, (SWITCH_2 | SDCARD_DETECT)) != (SWITCH_2 | SDCARD_DETECT)) // pull this input down to force boot loader mode (hold SW2 at reset) or with inserted SD card
        #define INIT_WATCHDOG_DISABLE() _CONFIG_PORT_INPUT(A, (SWITCH_2 | SWITCH_3 | SDCARD_DETECT), PORT_PS_UP_ENABLE) // configure as input
    #else
        #define FORCE_BOOT()       (_READ_PORT_MASK(A, SWITCH_2) == 0)   // pull this input down to force boot loader mode (hold SW2 at reset)
        #define INIT_WATCHDOG_DISABLE() _CONFIG_PORT_INPUT_FAST_LOW(A, (SWITCH_2 | SWITCH_3), PORT_PS_UP_ENABLE) // configure as input
    #endif
    #define RETAIN_LOADER_MODE()   (_READ_PORT_MASK(A, SWITCH_2) == 0)


    #define WATCHDOG_DISABLE()     (_READ_PORT_MASK(A, SWITCH_3) == 0)   // pull this input down to disable watchdog (hold SW3 at reset)

    #define TOGGLE_WATCHDOG_LED()   _TOGGLE_PORT(B, BLINK_LED)

    #define USB_HOST_POWER_CONFIG() _CONFIG_DRIVE_PORT_OUTPUT_VALUE_FAST_LOW(D, (USB_HOST_POWER_ENABLE), (0), (PORT_SRE_SLOW | PORT_DSE_HIGH))
    #define USB_HOST_POWER_ON()     _SETBITS(D, USB_HOST_POWER_ENABLE)
    #define USB_HOST_POWER_OFF()    _CLEARBITS(D, USB_HOST_POWER_ENABLE)

    #define GET_SDCARD_WP_STATE()   0                                    // no card protection switch available
    #define SD_CONTROLLER_AVAILABLE                                      // use SDHC controller rather than SPI
    #define SET_SD_CS_HIGH()                                             // dummy with SDHC controller
    #define SET_SD_CS_LOW()                                              // dummy with SDHC controller

    #ifdef _WINDOWS
        #define POWER_UP_SD_CARD()  SDHC_SYSCTL |= SDHC_SYSCTL_INITA; SDHC_SYSCTL &= ~SDHC_SYSCTL_INITA; // apply power to the SD card if appropriate (we use this to send 80 clocks - self-clearing bit)
    #else
        #define POWER_UP_SD_CARD()  SDHC_SYSCTL |= SDHC_SYSCTL_INITA; while (SDHC_SYSCTL & SDHC_SYSCTL_INITA) {}; // apply power to the SD card if appropriate (we use this to send 80 clocks)
    #endif
    #define POWER_DOWN_SD_CARD()                                         // remove power from SD card interface

    #define SDHC_SYSCTL_SPEED_SLOW  (SDHC_SYSCTL_SDCLKFS_64 | SDHC_SYSCTL_DVS_5) // 375kHz when 120MHz clock
    #define SDHC_SYSCTL_SPEED_FAST  (SDHC_SYSCTL_SDCLKFS_2 | SDHC_SYSCTL_DVS_3) // 20MHz when 120MHz clock
    #define SET_SPI_SD_INTERFACE_FULL_SPEED() fnSetSD_clock(SDHC_SYSCTL_SPEED_FAST); SDHC_PROCTL |= SDHC_PROCTL_DTW_4BIT
#elif defined FRDM_K66F || defined K66FX1M0 || defined FRDM_K28F
    #define LED_GREEN          (PORTE_BIT6)                              // green LED - if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define LED_RED            (PORTC_BIT9)                              // red LED - if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define LED_BLUE           (PORTA_BIT11)                             // blue LED - if the port is changed (eg. A to B) the port macros will require appropriate adjustment too

    #define SWITCH_2           (PORTD_BIT11)                             // switch 2 - if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define SWITCH_3           (PORTA_BIT10)                             // switch 3 - if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define SDCARD_DETECT      (PORTD_BIT10)

    #define BLINK_LED          (LED_GREEN)

    #define INIT_WATCHDOG_LED()    _CONFIG_DRIVE_PORT_OUTPUT_VALUE_FAST_LOW(E, (BLINK_LED), (BLINK_LED), (PORT_SRE_SLOW | PORT_DSE_HIGH)); _CONFIG_PORT_INPUT_FAST_LOW(D, (SWITCH_2), PORT_PS_UP_ENABLE); _CONFIG_PORT_INPUT_FAST_LOW(D, (SDCARD_DETECT), PORT_PS_DOWN_ENABLE);
    #define TOGGLE_WATCHDOG_LED()  _TOGGLE_PORT(E, BLINK_LED)
    #if defined SDCARD_SUPPORT || defined SPI_FLASH_FAT
        #define FORCE_BOOT()       (_READ_PORT_MASK(D, (SWITCH_2 | SDCARD_DETECT)) != (SWITCH_2)) // pull this input down to force boot loader mode (hold SW2 at reset) or with inserted SD card (positive logic)
        #define INIT_WATCHDOG_DISABLE() _CONFIG_PORT_INPUT(A, (SWITCH_3), PORT_PS_UP_ENABLE) // configure as input
    #else
        #define FORCE_BOOT()       (_READ_PORT_MASK(D, SWITCH_2) == 0)   // pull this input down to force boot loader mode (hold SW2 at reset)
        #define INIT_WATCHDOG_DISABLE() _CONFIG_PORT_INPUT_FAST_LOW(A, (SWITCH_3), PORT_PS_UP_ENABLE) // configure as input
    #endif
    #define RETAIN_LOADER_MODE()   (_READ_PORT_MASK(D, SWITCH_2) == 0)
    #define WATCHDOG_DISABLE()     (_READ_PORT_MASK(A, SWITCH_3) == 0)   // pull this input down to disable watchdog (hold SW3 at reset)

    #define USB_HOST_POWER_CONFIG()
    #define USB_HOST_POWER_ON()
    #define USB_HOST_POWER_OFF()

    #define GET_SDCARD_WP_STATE()   0                                    // no card protection switch available
    #define SD_CONTROLLER_AVAILABLE                                      // use SDHC controller rather than SPI
    #define SET_SD_CS_HIGH()                                             // dummy with SDHC controller
    #define SET_SD_CS_LOW()                                              // dummy with SDHC controller

    #ifdef _WINDOWS
        #define POWER_UP_SD_CARD()  SDHC_SYSCTL |= SDHC_SYSCTL_INITA; SDHC_SYSCTL &= ~SDHC_SYSCTL_INITA; // apply power to the SD card if appropriate (we use this to send 80 clocks - self-clearing bit)
    #else
        #define POWER_UP_SD_CARD()  SDHC_SYSCTL |= SDHC_SYSCTL_INITA; while (SDHC_SYSCTL & SDHC_SYSCTL_INITA) {}; // apply power to the SD card if appropriate (we use this to send 80 clocks)
    #endif
    #define POWER_DOWN_SD_CARD()                                         // remove power from SD card interface

    #define SDHC_SYSCTL_SPEED_SLOW  (SDHC_SYSCTL_SDCLKFS_64 | SDHC_SYSCTL_DVS_5) // 375kHz when 120MHz clock
    #define SDHC_SYSCTL_SPEED_FAST  (SDHC_SYSCTL_SDCLKFS_2 | SDHC_SYSCTL_DVS_3) // 20MHz when 120MHz clock
    #define SET_SPI_SD_INTERFACE_FULL_SPEED() fnSetSD_clock(SDHC_SYSCTL_SPEED_FAST); SDHC_PROCTL |= SDHC_PROCTL_DTW_4BIT

    #define BUTTON_KEY_DEFINITIONS  {_PORTD, SWITCH_2,   {286,   6, 299,  14 }}, \
                                    {_PORTA, SWITCH_3,   {286, 183, 299, 190 }},

    #if defined FRDM_K28F
        #define KEYPAD "../../uTaskerV1.4/Simulator/KeyPads/FRDM_K28F.bmp"
    #else
        #define KEYPAD "../../uTaskerV1.4/Simulator/KeyPads/FRDM_K66F.bmp"
    #endif

    #define MULTICOLOUR_LEDS        {0, 2}                               // single LED made up of entries 0, 1 and 2

        // '0'          '1'           input state   center (x,   y)   0 = circle, radius, controlling port, controlling pin 
    #define KEYPAD_LED_DEFINITIONS  \
        {RGB(0,  255,0  ), RGB(0,0,0),  1, {316, 10, 0, 5}, _PORTE, LED_GREEN}, \
        {RGB(255,0,  0  ), RGB(0,0,0),  1, {316, 10, 0, 5}, _PORTC, LED_RED}, \
        {RGB(0,  0,  255), RGB(0,0,0),  1, {316, 10, 0, 5}, _PORTA, LED_BLUE},
#elif defined TEENSY_3_5 || defined TEENSY_3_6
    #define LED_RED            (PORTC_BIT5)                              // red LED - if the port is changed (eg. A to B) the port macros will require appropriate adjustment too

    #define SWITCH_1           (PORTC_BIT1)                              // switch 1 [pin pad 22] - if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define SWITCH_2           (PORTC_BIT2)                              // switch 2 [pin pad 23] - if the port is changed (eg. A to B) the port macros will require appropriate adjustment too

    #define BLINK_LED          (LED_RED)

    #define INIT_WATCHDOG_LED() _CONFIG_DRIVE_PORT_OUTPUT_VALUE(C, (BLINK_LED), (BLINK_LED), (PORT_SRE_SLOW | PORT_DSE_HIGH))

    #define INIT_WATCHDOG_DISABLE() _CONFIG_PORT_INPUT_FAST_LOW(C, (SWITCH_1 | SWITCH_2), PORT_PS_UP_ENABLE) // configure as input
    #define WATCHDOG_DISABLE()     (_READ_PORT_MASK(C, SWITCH_2) == 0)   // pull this input down to disable watchdog (connect pin pad 23 to GND at reset)
    #define FORCE_BOOT()           (_READ_PORT_MASK(C, SWITCH_1) == 0)   // pull this input down to force boot loader mode (connect pin pad 22 to GND at reset)
    #define RETAIN_LOADER_MODE()   0                                     

    #define TOGGLE_WATCHDOG_LED()   _TOGGLE_PORT(C, BLINK_LED)

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
#elif defined TWR_K64F120M
    #define LED_GREEN              (PORTE_BIT6)                          // green LED - if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define LED_YELLOW             (PORTE_BIT7)                          // yellow LED - if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define LED_ORANGE             (PORTE_BIT8)                          // orange LED - if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define LED_BLUE               (PORTE_BIT9)                          // blue LED - if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define SWITCH_1               (PORTC_BIT6)                          // switch 2 - if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define SWITCH_3               (PORTA_BIT4)                          // switch 3 - if the port is changed (eg. A to B) the port macros will require appropriate adjustment too

    #define BLINK_LED              (LED_ORANGE)
    #define SDCARD_DETECT          (PORTB_BIT20)                         // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too

    #if defined SDCARD_SUPPORT || defined SPI_FLASH_FAT
        #define INIT_WATCHDOG_LED()    _CONFIG_DRIVE_PORT_OUTPUT_VALUE(E, (BLINK_LED), (BLINK_LED), (PORT_SRE_SLOW | PORT_DSE_HIGH)); _CONFIG_PORT_INPUT(B, (SDCARD_DETECT), PORT_PS_DOWN_ENABLE); // note that the force boot input is configured here and not with the INIT_WATCHDOG_DISABLE() since the watchdog must be disabled as quickly as possible
    #else
        #define INIT_WATCHDOG_LED()    _CONFIG_DRIVE_PORT_OUTPUT_VALUE(E, (BLINK_LED), (BLINK_LED), (PORT_SRE_SLOW | PORT_DSE_HIGH))
    #endif
    #define INIT_WATCHDOG_DISABLE() _CONFIG_PORT_INPUT_FAST_LOW(C, (SWITCH_1), PORT_PS_UP_ENABLE); _CONFIG_PORT_INPUT_FAST_LOW(A, (SWITCH_3), PORT_PS_UP_ENABLE); // configure as input

    #define WATCHDOG_DISABLE()     (_READ_PORT_MASK(A, SWITCH_3) == 0)   // pull this input down to disable watchdog (hold SW3 at reset)
    #if defined SDCARD_SUPPORT || defined SPI_FLASH_FAT
        #define FORCE_BOOT()       ((_READ_PORT_MASK(C, SWITCH_1) == 0) || (_READ_PORT_MASK(B, SDCARD_DETECT))) // pull this input down to force boot loader mode (hold SW1 at reset) or with inserted SD card
        #define RETAIN_LOADER_MODE() (_READ_PORT_MASK(C, SWITCH_1) == 0)
    #else
        #define FORCE_BOOT()       (_READ_PORT_MASK(C, SWITCH_1) == 0)   // pull this input down to force boot loader mode (hold SW1 at reset)
    #endif

    #define TOGGLE_WATCHDOG_LED()   _TOGGLE_PORT(E, BLINK_LED)

    #define SD_CONTROLLER_AVAILABLE                                      // use SDHC controller rather than SPI
    #define SET_SD_CS_HIGH()                                             // dummy with SDHC controller
    #define SET_SD_CS_LOW()                                              // dummy with SDHC controller

    #ifdef _WINDOWS
        #define POWER_UP_SD_CARD()    SDHC_SYSCTL |= SDHC_SYSCTL_INITA; SDHC_SYSCTL &= ~SDHC_SYSCTL_INITA; // apply power to the SD card if appropriate (we use this to send 80 clocks - self-clearing bit)
    #else
        #define POWER_UP_SD_CARD()    SDHC_SYSCTL |= SDHC_SYSCTL_INITA; while (SDHC_SYSCTL & SDHC_SYSCTL_INITA) {}; // apply power to the SD card if appropriate (we use this to send 80 clocks)
    #endif

    #define SDHC_SYSCTL_SPEED_SLOW    (SDHC_SYSCTL_SDCLKFS_64 | SDHC_SYSCTL_DVS_5) // 375kHz when 120MHz clock
    #define SDHC_SYSCTL_SPEED_FAST    (SDHC_SYSCTL_SDCLKFS_2 | SDHC_SYSCTL_DVS_3) // 20MHz when 120MHz clock
    #define SET_SPI_SD_INTERFACE_FULL_SPEED() fnSetSD_clock(SDHC_SYSCTL_SPEED_FAST); SDHC_PROCTL |= SDHC_PROCTL_DTW_4BIT

    #define POWER_DOWN_SD_CARD()                                         // remove power from SD card interface

    #define GET_SDCARD_WP_STATE()  1                                     // always write protect during boot loading
#elif defined FRDM_K64F
    #define LED_GREEN              (PORTE_BIT26)                         // green LED - if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define LED_BLUE               (PORTB_BIT21)                         // blue LED - if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define LED_RED                (PORTB_BIT22)                         // red LED - if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define SWITCH_2               (PORTC_BIT6)                          // switch 2 - if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define SWITCH_3               (PORTA_BIT4)                          // switch 3 - if the port is changed (eg. A to B) the port macros will require appropriate adjustment too

    #define BLINK_LED              (LED_GREEN)
    #define SDCARD_DETECT          (PORTE_BIT6)                          // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too

    #if defined SDCARD_SUPPORT || defined SPI_FLASH_FAT
        #define INIT_WATCHDOG_LED() _CONFIG_DRIVE_PORT_OUTPUT_VALUE(E, (BLINK_LED), (BLINK_LED), (PORT_SRE_SLOW | PORT_DSE_HIGH)); _CONFIG_PORT_INPUT(E, (SDCARD_DETECT), PORT_PS_DOWN_ENABLE); // note that the force boot input is configured here and not with the INIT_WATCHDOG_DISABLE() since the watchdog must be disabled as quickly as possible
    #else
        #define INIT_WATCHDOG_LED() _CONFIG_DRIVE_PORT_OUTPUT_VALUE(E, (BLINK_LED), (BLINK_LED), (PORT_SRE_SLOW | PORT_DSE_HIGH))
    #endif
    #define INIT_WATCHDOG_DISABLE() _CONFIG_PORT_INPUT_FAST_LOW(C, (SWITCH_2), PORT_PS_UP_ENABLE); _CONFIG_PORT_INPUT_FAST_LOW(A, (SWITCH_3), PORT_PS_UP_ENABLE); // configure as input

    #define WATCHDOG_DISABLE()     (_READ_PORT_MASK(A, SWITCH_3) == 0)   // pull this input down to disable watchdog (hold SW3 at reset)
    #if defined SDCARD_SUPPORT || defined SPI_FLASH_FAT
        #define FORCE_BOOT()       ((_READ_PORT_MASK(C, SWITCH_2) == 0) || (_READ_PORT_MASK(E, SDCARD_DETECT))) // pull this input down to force boot loader mode (hold SW2 at reset) or with inserted SD card
    #else
        #define FORCE_BOOT()       (_READ_PORT_MASK(C, SWITCH_2) == 0)   // pull this input down to force boot loader mode (hold SW2 at reset)
    #endif
    #define RETAIN_LOADER_MODE()   (_READ_PORT_MASK(C, SWITCH_2) == 0)

    #define TOGGLE_WATCHDOG_LED()   _TOGGLE_PORT(E, BLINK_LED)

    #define SD_CONTROLLER_AVAILABLE                                      // use SDHC controller rather than SPI
    #define SET_SD_CS_HIGH()                                             // dummy with SDHC controller
    #define SET_SD_CS_LOW()                                              // dummy with SDHC controller

    #ifdef _WINDOWS
        #define POWER_UP_SD_CARD()  SDHC_SYSCTL |= SDHC_SYSCTL_INITA; SDHC_SYSCTL &= ~SDHC_SYSCTL_INITA; // apply power to the SD card if appropriate (we use this to send 80 clocks - self-clearing bit)
    #else
        #define POWER_UP_SD_CARD()  SDHC_SYSCTL |= SDHC_SYSCTL_INITA; while (SDHC_SYSCTL & SDHC_SYSCTL_INITA) {}; // apply power to the SD card if appropriate (we use this to send 80 clocks)
    #endif

    #define SDHC_SYSCTL_SPEED_SLOW  (SDHC_SYSCTL_SDCLKFS_64 | SDHC_SYSCTL_DVS_5) // 375kHz when 120MHz clock
    #define SDHC_SYSCTL_SPEED_FAST  (SDHC_SYSCTL_SDCLKFS_2 | SDHC_SYSCTL_DVS_3) // 20MHz when 120MHz clock
    #define SET_SPI_SD_INTERFACE_FULL_SPEED() fnSetSD_clock(SDHC_SYSCTL_SPEED_FAST); SDHC_PROCTL |= SDHC_PROCTL_DTW_4BIT

    #define POWER_DOWN_SD_CARD()                                         // remove power from SD card interface

    #define GET_SDCARD_WP_STATE()  1                                     // always write protect during boot loading

    #if defined ENC424J600_INTERFACE
        #define SUPPORT_PORT_INTERRUPTS                                  // port interrupt required by the external Ethernet controller
        #define NO_PORT_INTERRUPTS_PORTA                                 // remove port interrupt support from port A
        #define NO_PORT_INTERRUPTS_PORTB                                 // remove port interrupt support from port B
      //#define NO_PORT_INTERRUPTS_PORTC                                 // remove port interrupt support from port C
        #define NO_PORT_INTERRUPTS_PORTD                                 // remove port interrupt support from port D
        #define NO_PORT_INTERRUPTS_PORTE                                 // remove port interrupt support from port E
    #endif

    #define DEL_USB_SYMBOL()
    #define SET_USB_SYMBOL()
    #define USB_HOST_POWER_CONFIG()
    #define USB_HOST_POWER_ON()                                          // the FRDM-K64F doesn't have a USB power supply that can be controlled, instead jumper J21 can be manually shorted so that the 5V power from the OpenSDA circuit is connected (use carefully since there is no protection!)
    #define USB_HOST_POWER_OFF()
#elif defined TWR_K21D50M
    #define BLINK_LED              (PORTD_BIT4)
    #define SWITCH_2               (PORTC_BIT7)                          // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define SWITCH_3               (PORTC_BIT6)                          // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too

    #define INIT_WATCHDOG_LED()    _CONFIG_DRIVE_PORT_OUTPUT_VALUE(D, (BLINK_LED), (BLINK_LED), (PORT_SRE_SLOW | PORT_DSE_HIGH))
    #define INIT_WATCHDOG_DISABLE() _CONFIG_PORT_INPUT_FAST_LOW(C, (SWITCH_3 | SWITCH_2), PORT_PS_UP_ENABLE); // configure as input

    #define WATCHDOG_DISABLE()     (_READ_PORT_MASK(C, SWITCH_2) == 0)   // pull this input down to disable watchdog (hold SW3 at reset)
    #define FORCE_BOOT()           (_READ_PORT_MASK(C, SWITCH_3) == 0)   // pull this input down to force boot loader mode (hold SW2 at reset)
    #define RETAIN_LOADER_MODE()   (_READ_PORT_MASK(C, SWITCH_3) == 0)

    #define TOGGLE_WATCHDOG_LED()   _TOGGLE_PORT(D, BLINK_LED)

    // Configure to suit SD card SPI mode at between 100k and 400k (SPI0)
    //
    #define SPI_CS1_0                  PORTC_BIT4
    #define INITIALISE_SPI_SD_INTERFACE() POWER_UP(6, SIM_SCGC6_SPI0); \
    _CONFIG_PORT_INPUT(E, (PORTE_BIT0), (PORT_NO_PULL)); \
    _CONFIG_PERIPHERAL(C, 5, PC_5_SPI0_SCK); _CONFIG_PERIPHERAL(C, 6, (PC_6_SPI0_SOUT | PORT_SRE_FAST | PORT_DSE_HIGH)); _CONFIG_PERIPHERAL(C, 7, (PC_7_SPI0_SIN | PORT_PS_UP_ENABLE)); \
    _CONFIG_DRIVE_PORT_OUTPUT_VALUE(C, SPI_CS1_0, SPI_CS1_0, (PORT_SRE_FAST | PORT_DSE_HIGH)); \
    SPI0_CTAR0 = (SPI_CTAR_ASC_6 | SPI_CTAR_FMSZ_8 | SPI_CTAR_CPHA | SPI_CTAR_CPOL | SPI_CTAR_BR_128); SPI0_MCR = (SPI_MCR_DIS_TXF | SPI_MCR_DIS_RXF | SPI_MCR_MSTR | SPI_MCR_DCONF_SPI | SPI_MCR_CLR_RXF | SPI_MCR_CLR_TXF | SPI_MCR_PCSIS_CS0 | SPI_MCR_PCSIS_CS1 | SPI_MCR_PCSIS_CS2 | SPI_MCR_PCSIS_CS3 | SPI_MCR_PCSIS_CS4 | SPI_MCR_PCSIS_CS5)

    #define ENABLE_SPI_SD_OPERATION()
    #define SET_SD_CARD_MODE()

    // Set maximum speed
    //
    #define SET_SPI_SD_INTERFACE_FULL_SPEED() SPI0_MCR |= SPI_MCR_HALT; SPI0_CTAR0 = (SPI_CTAR_FMSZ_8 | SPI_CTAR_CPOL | SPI_CTAR_CPHA | SPI_CTAR_BR_2); SPI0_MCR &= ~SPI_MCR_HALT;
    #ifdef _WINDOWS
        #define WRITE_SPI_CMD(byte)    SPI0_SR &= ~(SPI_SR_RFDF); SPI0_PUSHR = (byte | SPI_PUSHR_PCS_NONE | SPI_PUSHR_CTAS_CTAR0); SPI0_POPR = _fnSimSD_write((unsigned char)byte)
        #define WAIT_TRANSMISSON_END() while (!(SPI0_SR & (SPI_SR_RFDF))) { SPI0_SR |= (SPI_SR_RFDF); }
        #define READ_SPI_DATA()        (unsigned char)SPI0_POPR
    #else
        #define WRITE_SPI_CMD(byte)    SPI0_SR = (SPI_SR_RFDF); SPI0_PUSHR = (byte | SPI_PUSHR_PCS_NONE | SPI_PUSHR_CTAS_CTAR0) // clear flags before transmitting (and receiving) a single byte
        #define WAIT_TRANSMISSON_END() while (!(SPI0_SR & (SPI_SR_RFDF))) {}
        #define READ_SPI_DATA()        (unsigned char)SPI0_POPR
    #endif
    #define SET_SD_DI_CS_HIGH()  _SETBITS(C, SPI_CS1_0)                  // force DI and CS lines high ready for the initialisation sequence
    #define SET_SD_CS_LOW()      _CLEARBITS(C, SPI_CS1_0)                // assert the CS line of the SD card to be read
    #define SET_SD_CS_HIGH()     _SETBITS(C, SPI_CS1_0)                  // negate the CS line of the SD card to be read
    #define POWER_UP_SD_CARD()                                           // apply power to the SD card if appropriate
    #define POWER_DOWN_SD_CARD()
    #define GET_SDCARD_WP_STATE()  1                                     // always write protect
#elif defined TEENSY_3_1
    #define BLINK_LED              (PORTC_BIT5)                          // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define SWITCH_1               (PORTD_BIT5)                          // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define SWITCH_2               (PORTD_BIT6)                          // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too

    #define INIT_WATCHDOG_LED()     _CONFIG_DRIVE_PORT_OUTPUT_VALUE(C, (BLINK_LED), (BLINK_LED), (PORT_SRE_SLOW | PORT_DSE_HIGH))
    #define INIT_WATCHDOG_DISABLE() _CONFIG_PORT_INPUT_FAST_LOW(D, (SWITCH_1 | SWITCH_2), PORT_PS_UP_ENABLE); // configure as input

    #define WATCHDOG_DISABLE()     (_READ_PORT_MASK(D, SWITCH_2) == 0)   // pull this input down to disable watchdog (pull connector pin 21 [A6] to ground)
    #define FORCE_BOOT()           (_READ_PORT_MASK(D, SWITCH_1) == 0)   // pull this input down to force boot loader mode (pull connector pin 20 [A7] to ground)
    #define RETAIN_LOADER_MODE()   (_READ_PORT_MASK(D, SWITCH_1) == 0)   // pull this input down to retain the boot loader mode after the SD card check has completed (pull connector pin 20 [A7] to ground)

    #define TOGGLE_WATCHDOG_LED()  _TOGGLE_PORT(C, BLINK_LED)                                   

        // '0'          '1'             input state  center (x,   y)   0 = circle, radius, controlling port, controlling pin 
    #define KEYPAD_LED_DEFINITIONS  \
        {RGB(40,40,40), RGB(255,0,0),        0,  {37, 162, 57, 171 }, _PORTC, BLINK_LED}, \
        {RGB(255,255,255), RGB(100,100,100), 0,  {17, 186, 0,  6   }, _PORTC, BLINK_LED}

    #define KEYPAD "../../uTaskerV1.4/Simulator/KeyPads/teensy3_1.bmp"

    #if defined SDCARD_SUPPORT
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

        #define DEL_USB_SYMBOL()                                         // control display of USB enumeration - clear
        #define SET_USB_SYMBOL()                                         // control display of USB enumeration - set

      //#define START_ON_INTERRUPT                                       // enable optional detection of a push button interrupt to restart update check and to jump to the [new] application)
        #if defined START_ON_INTERRUPT
            #define BUTTON_PORT            PORTD;                        // the port that the interrupt button is on
            #define BUTTON_INPUT           SWITCH_2                      // the port pin that the interrupt button is on
            #define BUTTON_INTERRUPT_EDGE  (IRQ_FALLING_EDGE | PULLUP_ON)// interrupt on falling edge, with pull-up resistor
        #endif
    #endif
    #if defined ENC424J600_INTERFACE || defined START_ON_INTERRUPT       // these options require port interrupts to be enabed
        #define SUPPORT_PORT_INTERRUPTS                                  // port interrupt required by the external Ethernet controller
        #define NO_PORT_INTERRUPTS_PORTA                                 // remove port interrupt support from port A
        #define NO_PORT_INTERRUPTS_PORTB                                 // remove port interrupt support from port B
        #if !defined ENC424J600_INTERFACE
            #define NO_PORT_INTERRUPTS_PORTC                             // remove port interrupt support from port C
        #endif
        #if !defined START_ON_INTERRUPT
            #define NO_PORT_INTERRUPTS_PORTD                             // remove port interrupt support from port D
        #endif
        #define NO_PORT_INTERRUPTS_PORTE                                 // remove port interrupt support from port E
    #endif
#elif defined TWR_K20D50M || defined TWR_K20D72M
    #define DEMO_LED_1             (PORTC_BIT7)                          // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define DEMO_LED_2             (PORTC_BIT8)                          // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define DEMO_LED_3             (PORTC_BIT9)                          // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define DEMO_LED_4             (PORTC_BIT10)                         // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define BLINK_LED              (DEMO_LED_1)
    #define SWITCH_2               (PORTC_BIT1)                          // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define SWITCH_3               (PORTC_BIT2)                          // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too

    #define INIT_WATCHDOG_LED()    _CONFIG_DRIVE_PORT_OUTPUT_VALUE(C, (BLINK_LED), (BLINK_LED), (/*PORT_ODE | */PORT_SRE_SLOW | PORT_DSE_HIGH)); _CONFIG_PORT_INPUT(C, SWITCH_2, PORT_PS_UP_ENABLE); // note that the force boot input is configured here and not with the INIT_WATCHDOG_DISABLE() since the watchdog must be disabled as quickly as possible
    #define INIT_WATCHDOG_DISABLE() _CONFIG_PORT_INPUT_FAST_LOW(C, SWITCH_3, PORT_PS_UP_ENABLE); // configure as input

    #define WATCHDOG_DISABLE()     (_READ_PORT_MASK(C, SWITCH_3) == 0)   // pull this input down to disable watchdog (hold SW3 at reset)
    #define FORCE_BOOT()           (_READ_PORT_MASK(C, SWITCH_2) == 0)   // pull this input down to force boot loader mode (hold SW2 at reset)
    #define RETAIN_LOADER_MODE()   (_READ_PORT_MASK(C, SWITCH_2) == 0)

    #define TOGGLE_WATCHDOG_LED()   _TOGGLE_PORT(C, BLINK_LED)


    // Configure to suit SD card SPI mode at between 100k and 400k (SPI0)
    //
    #define SPI_CS1_0                  PORTC_BIT4
    #define INITIALISE_SPI_SD_INTERFACE() POWER_UP(6, SIM_SCGC6_SPI0); \
    _CONFIG_PORT_INPUT(E, (PORTE_BIT0), (PORT_NO_PULL)); \
    _CONFIG_PERIPHERAL(C, 5, PC_5_SPI0_SCK); _CONFIG_PERIPHERAL(C, 6, (PC_6_SPI0_SOUT | PORT_SRE_FAST | PORT_DSE_HIGH)); _CONFIG_PERIPHERAL(C, 7, (PC_7_SPI0_SIN | PORT_PS_UP_ENABLE)); \
    _CONFIG_DRIVE_PORT_OUTPUT_VALUE(C, SPI_CS1_0, SPI_CS1_0, (PORT_SRE_FAST | PORT_DSE_HIGH)); \
    SPI0_CTAR0 = (SPI_CTAR_ASC_6 | SPI_CTAR_FMSZ_8 | SPI_CTAR_CPHA | SPI_CTAR_CPOL | SPI_CTAR_BR_128); SPI0_MCR = (SPI_MCR_DIS_TXF | SPI_MCR_DIS_RXF | SPI_MCR_MSTR | SPI_MCR_DCONF_SPI | SPI_MCR_CLR_RXF | SPI_MCR_CLR_TXF | SPI_MCR_PCSIS_CS0 | SPI_MCR_PCSIS_CS1 | SPI_MCR_PCSIS_CS2 | SPI_MCR_PCSIS_CS3 | SPI_MCR_PCSIS_CS4 | SPI_MCR_PCSIS_CS5)

    #define ENABLE_SPI_SD_OPERATION()
    #define SET_SD_CARD_MODE()

    // Set maximum speed
    //
    #define SET_SPI_SD_INTERFACE_FULL_SPEED() SPI0_MCR |= SPI_MCR_HALT; SPI0_CTAR0 = (SPI_CTAR_FMSZ_8 | SPI_CTAR_CPOL | SPI_CTAR_CPHA | SPI_CTAR_BR_2); SPI0_MCR &= ~SPI_MCR_HALT;
    #ifdef _WINDOWS
        #define WRITE_SPI_CMD(byte)    SPI0_SR &= ~(SPI_SR_RFDF); SPI0_PUSHR = (byte | SPI_PUSHR_PCS_NONE | SPI_PUSHR_CTAS_CTAR0); SPI0_POPR = _fnSimSD_write((unsigned char)byte)
        #define WAIT_TRANSMISSON_END() while (!(SPI0_SR & (SPI_SR_RFDF))) { SPI0_SR |= (SPI_SR_RFDF); }
        #define READ_SPI_DATA()        (unsigned char)SPI0_POPR
    #else
        #define WRITE_SPI_CMD(byte)    SPI0_SR = (SPI_SR_RFDF); SPI0_PUSHR = (byte | SPI_PUSHR_PCS_NONE | SPI_PUSHR_CTAS_CTAR0) // clear flags before transmitting (and receiving) a single byte
        #define WAIT_TRANSMISSON_END() while (!(SPI0_SR & (SPI_SR_RFDF))) {}
        #define READ_SPI_DATA()        (unsigned char)SPI0_POPR
    #endif
    #define SET_SD_DI_CS_HIGH()  _SETBITS(C, SPI_CS1_0)                  // force DI and CS lines high ready for the initialisation sequence
    #define SET_SD_CS_LOW()      _CLEARBITS(C, SPI_CS1_0)                // assert the CS line of the SD card to be read
    #define SET_SD_CS_HIGH()     _SETBITS(C, SPI_CS1_0)                  // negate the CS line of the SD card to be read
    #define POWER_UP_SD_CARD()                                           // apply power to the SD card if appropriate
    #define POWER_DOWN_SD_CARD()
    #define GET_SDCARD_WP_STATE()  1                                     // always write protect
#elif defined FRDM_K20D50M                                               // {4}
    #define BLINK_LED              (PORTD_BIT4)                          // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define SWITCH_1               (PORTE_BIT0)                          // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define SWITCH_2               (PORTE_BIT1)                          // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too

    #define INIT_WATCHDOG_LED()    _CONFIG_DRIVE_PORT_OUTPUT_VALUE(D, (BLINK_LED), (BLINK_LED), (PORT_SRE_SLOW | PORT_DSE_HIGH))
    #define INIT_WATCHDOG_DISABLE() _CONFIG_PORT_INPUT_FAST_LOW(E, (SWITCH_1 | SWITCH_2), PORT_PS_UP_ENABLE) // configure as input

    #define WATCHDOG_DISABLE()     (_READ_PORT_MASK(E, SWITCH_2) == 0)   // pull this input down to disable watchdog
    #define FORCE_BOOT()           (_READ_PORT_MASK(E, SWITCH_1) == 0)   // pull this input down to force boot loader mode

    #define TOGGLE_WATCHDOG_LED()   _TOGGLE_PORT(D, BLINK_LED)
#elif defined tinyK20
    #define BLINK_LED              (PORTD_BIT4)                          // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define SWITCH_1               (PORTB_BIT0)                          // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define SWITCH_2               (PORTB_BIT1)                          // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define SDCARD_DETECT_PIN      (PORTC_BIT0)                          // '1' when SD card is inserted

    #define INIT_WATCHDOG_LED()    _CONFIG_DRIVE_PORT_OUTPUT_VALUE(D, (BLINK_LED), (BLINK_LED), (PORT_SRE_SLOW | PORT_DSE_HIGH))
    #define INIT_WATCHDOG_DISABLE() _CONFIG_PORT_INPUT_FAST_LOW(B, (SWITCH_1 | SWITCH_2), PORT_PS_UP_ENABLE); _CONFIG_PORT_INPUT_FAST_LOW(C, (SDCARD_DETECT_PIN), PORT_PS_DOWN_ENABLE) // configure as inputs

    #define WATCHDOG_DISABLE()     (_READ_PORT_MASK(B, SWITCH_2) == 0)   // pull this input down to disable watchdog
    #if defined SDCARD_SUPPORT
        #define FORCE_BOOT()       (_READ_PORT_MASK(C, SDCARD_DETECT_PIN) != 0) // inserted SD card forces boot loader mode
    #else
        #define FORCE_BOOT()       (_READ_PORT_MASK(B, SWITCH_1) == 0)   // pull this input down to force boot loader mode
    #endif
    #define RETAIN_LOADER_MODE()   (_READ_PORT_MASK(B, SWITCH_1) == 0)   // pull this input low to retain the boot loader mode after SD card has been checked
    #define TOGGLE_WATCHDOG_LED()  _TOGGLE_PORT(D, BLINK_LED)

    // Configure to suit SD card SPI mode at between 100k and 400k (SPI0)
    //
    #define SPI_CS1_0                  PORTC_BIT1
    #define INITIALISE_SPI_SD_INTERFACE() POWER_UP(6, SIM_SCGC6_SPI0); \
    _CONFIG_PORT_INPUT(E, (PORTE_BIT0), (PORT_NO_PULL)); \
    _CONFIG_PERIPHERAL(C, 5, PC_5_SPI0_SCK); _CONFIG_PERIPHERAL(C, 6, (PC_6_SPI0_SOUT | PORT_SRE_FAST | PORT_DSE_HIGH)); _CONFIG_PERIPHERAL(C, 7, (PC_7_SPI0_SIN | PORT_PS_UP_ENABLE)); \
    _CONFIG_DRIVE_PORT_OUTPUT_VALUE(C, SPI_CS1_0, SPI_CS1_0, (PORT_SRE_FAST | PORT_DSE_HIGH)); \
    SPI0_CTAR0 = (SPI_CTAR_ASC_6 | SPI_CTAR_FMSZ_8 | SPI_CTAR_CPHA | SPI_CTAR_CPOL | SPI_CTAR_BR_128); SPI0_MCR = (SPI_MCR_DIS_TXF | SPI_MCR_DIS_RXF | SPI_MCR_MSTR | SPI_MCR_DCONF_SPI | SPI_MCR_CLR_RXF | SPI_MCR_CLR_TXF | SPI_MCR_PCSIS_CS0 | SPI_MCR_PCSIS_CS1 | SPI_MCR_PCSIS_CS2 | SPI_MCR_PCSIS_CS3 | SPI_MCR_PCSIS_CS4 | SPI_MCR_PCSIS_CS5)

    #define ENABLE_SPI_SD_OPERATION()
    #define SET_SD_CARD_MODE()

    // Set maximum speed
    //
    #define SET_SPI_SD_INTERFACE_FULL_SPEED() SPI0_MCR |= SPI_MCR_HALT; SPI0_CTAR0 = (SPI_CTAR_FMSZ_8 | SPI_CTAR_CPOL | SPI_CTAR_CPHA | SPI_CTAR_BR_2); SPI0_MCR &= ~SPI_MCR_HALT;
    #ifdef _WINDOWS
        #define WRITE_SPI_CMD(byte)    SPI0_SR &= ~(SPI_SR_RFDF); SPI0_PUSHR = (byte | SPI_PUSHR_PCS_NONE | SPI_PUSHR_CTAS_CTAR0); SPI0_POPR = _fnSimSD_write((unsigned char)byte)
        #define WAIT_TRANSMISSON_END() while ((SPI0_SR & (SPI_SR_RFDF)) == 0) { SPI0_SR |= (SPI_SR_RFDF); }
        #define READ_SPI_DATA()        (unsigned char)SPI0_POPR
    #else
        #define WRITE_SPI_CMD(byte)    SPI0_SR = (SPI_SR_RFDF); SPI0_PUSHR = (byte | SPI_PUSHR_PCS_NONE | SPI_PUSHR_CTAS_CTAR0) // clear flags before transmitting (and receiving) a single byte
        #define WAIT_TRANSMISSON_END() while ((SPI0_SR & (SPI_SR_RFDF)) == 0) {}
        #define READ_SPI_DATA()        (unsigned char)SPI0_POPR
    #endif
    #define SET_SD_DI_CS_HIGH()  _SETBITS(C, SPI_CS1_0)                  // force DI and CS lines high ready for the initialisation sequence
    #define SET_SD_CS_LOW()      _CLEARBITS(C, SPI_CS1_0)                // assert the CS line of the SD card to be read
    #define SET_SD_CS_HIGH()     _SETBITS(C, SPI_CS1_0)                  // negate the CS line of the SD card to be read
    #define POWER_UP_SD_CARD()                                           // apply power to the SD card if appropriate
    #define POWER_DOWN_SD_CARD()
    #define GET_SDCARD_WP_STATE()  1                                     // always write protect
#elif defined K02F100M
    #define BLINK_LED              (PORTB_BIT1)                          // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define SWITCH_1               (PORTB_BIT2)                          // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define SWITCH_2               (PORTB_BIT3)                          // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too

    #define INIT_WATCHDOG_LED()    _CONFIG_DRIVE_PORT_OUTPUT_VALUE(B, (BLINK_LED), (BLINK_LED), (PORT_SRE_SLOW | PORT_DSE_HIGH))
    #define INIT_WATCHDOG_DISABLE() _CONFIG_PORT_INPUT_FAST_LOW(B, (SWITCH_1 | SWITCH_2), PORT_PS_UP_ENABLE) // configure as input

    #define WATCHDOG_DISABLE()     (_READ_PORT_MASK(B, SWITCH_2) == 0)   // pull this input down to disable watchdog
    #define FORCE_BOOT()           (_READ_PORT_MASK(B, SWITCH_1) == 0)   // pull this input down to force boot loader mode

    #define TOGGLE_WATCHDOG_LED()   _TOGGLE_PORT(B, BLINK_LED)
#elif defined FRDM_KE06Z
    #define BLINK_LED              (KE_PORTG_BIT6)                        // if the port is changed (eg. A to D) the port macros will require appropriate adjustment too
    #define SWITCH_1               (KE_PORTG_BIT4)                        // if the port is changed (eg. A to D) the port macros will require appropriate adjustment too
    #define SWITCH_2               (KE_PORTF_BIT0)                        // if the port is changed (eg. A to D) the port macros will require appropriate adjustment too
    #define SWITCH_3               (KE_PORTF_BIT1)

    #define INIT_WATCHDOG_LED()    _CONFIG_DRIVE_PORT_OUTPUT_VALUE(B, (BLINK_LED), (BLINK_LED), (PORT_SRE_SLOW | PORT_DSE_HIGH))
    #define INIT_WATCHDOG_DISABLE() _CONFIG_PORT_INPUT(B, (SWITCH_1 | SWITCH_2 | SWITCH_3), PORT_PS_UP_ENABLE) // configure as inputs

    #define WATCHDOG_DISABLE()     (_READ_PORT_MASK(B, SWITCH_1) == 0)   // pull this input down to disable watchdog (J4-15)
    #define FORCE_BOOT()           (_READ_PORT_MASK(B, SWITCH_2) == 0)   // pull this input down to force boot loader mode (J4-13)
    #define RETAIN_LOADER_MODE()   (_READ_PORT_MASK(B, SWITCH_3) == 0)   // pull this input down to force boot loader mode (J4-11)

    #define TOGGLE_WATCHDOG_LED()  _TOGGLE_PORT(B, BLINK_LED)
#elif defined FRDM_KE02Z || defined FRDM_KE02Z40M
    #define BLINK_LED              (KE_PORTH_BIT2)                       // if the port is changed (eg. A to D) the port macros will require appropriate adjustment too
    #define SWITCH_2               (KE_PORTF_BIT0)                       // if the port is changed (eg. A to D) the port macros will require appropriate adjustment too
    #define SWITCH_3               (KE_PORTG_BIT3)                       // if the port is changed (eg. A to D) the port macros will require appropriate adjustment too

    #define INIT_WATCHDOG_LED()    _CONFIG_DRIVE_PORT_OUTPUT_VALUE(B, (BLINK_LED), (BLINK_LED), (PORT_SRE_SLOW | PORT_DSE_HIGH))
    #define INIT_WATCHDOG_DISABLE() _CONFIG_PORT_INPUT(B, (SWITCH_2 | SWITCH_3), PORT_PS_UP_ENABLE) // configure as inputs

    #define WATCHDOG_DISABLE()     (_READ_PORT_MASK(B, SWITCH_2) == 0)   // pull this input down to disable watchdog (connect J9-13 to GND)
    #define FORCE_BOOT()           (_READ_PORT_MASK(B, SWITCH_3) == 0)   // pull this input down to force boot loader mode (connect J9-15 to GND)
    #define RETAIN_LOADER_MODE()   (_READ_PORT_MASK(B, SWITCH_3) == 0)   // pull this input down to force boot loader mode (connect J9-15 to GND)

    #define TOGGLE_WATCHDOG_LED()  _TOGGLE_PORT(B, BLINK_LED)
#elif defined FRDM_KE04Z
    #define BLINK_LED              (KE_PORTC_BIT4)                       // if the port is changed (eg. A to D) the port macros will require appropriate adjustment too
    #define SWITCH_2               (KE_PORTB_BIT1)                       // if the port is changed (eg. A to D) the port macros will require appropriate adjustment too
    #define SWITCH_3               (KE_PORTB_BIT0)                       // if the port is changed (eg. A to D) the port macros will require appropriate adjustment too

    #define INIT_WATCHDOG_LED()    _CONFIG_DRIVE_PORT_OUTPUT_VALUE(A, (BLINK_LED), (BLINK_LED), (PORT_SRE_SLOW | PORT_DSE_HIGH))
    #define INIT_WATCHDOG_DISABLE() _CONFIG_PORT_INPUT(A, (SWITCH_2 | SWITCH_3), PORT_PS_UP_ENABLE) // configure as inputs

    #define WATCHDOG_DISABLE()     (_READ_PORT_MASK(A, SWITCH_2) == 0)   // pull this input down to disable watchdog (connect J1-4 with GND at reset)
    #define FORCE_BOOT()           1 //(!_READ_PORT_MASK(A, SWITCH_3))   // pull this input down to force boot loader mode (connect J1-2 with GND at reset)

    #define TOGGLE_WATCHDOG_LED()  _TOGGLE_PORT(A, BLINK_LED)
#elif defined TRK_KEA128 || defined TRK_KEA64
    #define BLINK_LED              (KE_PORTC_BIT0)                        // if the port is changed (eg. A to D) the port macros will require appropriate adjustment too
    #define SWITCH_1               (KE_PORTD_BIT0)                        // if the port is changed (eg. A to D) the port macros will require appropriate adjustment too
    #define SWITCH_2               (KE_PORTD_BIT1)                        // if the port is changed (eg. A to D) the port macros will require appropriate adjustment too

    #define INIT_WATCHDOG_LED()    _CONFIG_DRIVE_PORT_OUTPUT_VALUE(A, (BLINK_LED), (BLINK_LED), (PORT_SRE_SLOW | PORT_DSE_HIGH))
    #define INIT_WATCHDOG_DISABLE() _CONFIG_PORT_INPUT(A, (SWITCH_1 | SWITCH_2), PORT_NO_PULL) // configure as inputs

    #define WATCHDOG_DISABLE()     (_READ_PORT_MASK(A, SWITCH_2) != 0)   // hold SW2 down at reset to disable watchdog
  //#define FORCE_BOOT()           (_READ_PORT_MASK(A, SWITCH_1) != 0)   // hold SW1 down at reset to force boot loader mode
    // If the application has commanded a reset after writing the pattern 0x6723 into the boot mail box the serial loader mode should be started as well
    //
    #define FORCE_BOOT()           ((_READ_PORT_MASK(A, SWITCH_1) != 0) || (((SIM_SRSID & SIM_SRSID_SW) != 0) && (*(BOOT_MAIL_BOX) == 0x6723)))

    #define TOGGLE_WATCHDOG_LED()  _TOGGLE_PORT(A, BLINK_LED)
#elif defined FRDM_KEAZ128Q80 || defined FRDM_KEAZ64Q64 || defined FRDM_KEAZN32Q64
    #define GREEN_LED              (KE_PORTH_BIT1)                       // (green LED - PTC1) if the port is changed (eg. A to D) the port macros will require appropriate adjustment too
    #define RED_LED                (KE_PORTH_BIT0)                       // (red LED - PTC0) if the port is changed (eg. A to D) the port macros will require appropriate adjustment too
    #define BLUE_LED               (KE_PORTE_BIT7)                       // (blue LED - PTC2) if the port is changed (eg. A to D) the port macros will require appropriate adjustment too

    #define BLINK_LED              GREEN_LED

    #define INIT_WATCHDOG_LED()    _CONFIG_DRIVE_PORT_OUTPUT_VALUE(B, (BLINK_LED), (BLINK_LED), (PORT_SRE_SLOW | PORT_DSE_HIGH))

    #if defined FRDM_KEAZN32Q64
        #define SWITCH_2           (KE_PORTA_BIT0)                       // SW2 if the port is changed (eg. A to D) the port macros will require appropriate adjustment too
        #define SWITCH_3           (KE_PORTA_BIT1)                       // SW3 if the port is changed (eg. A to D) the port macros will require appropriate adjustment too
        #define INIT_WATCHDOG_DISABLE() _CONFIG_PORT_INPUT(A, (SWITCH_2 | SWITCH_3), PORT_PS_UP_ENABLE) // configure as inputs
        #define FORCE_BOOT()       (_READ_PORT_MASK(A, SWITCH_2) != 0)   // pull this input down to force boot loader mode (hold SW2 at reset)
        #define WATCHDOG_DISABLE() (_READ_PORT_MASK(A, SWITCH_3) != 0)   // pull this input down to disable watchdog (hold SW3 at reset)
    #else
        #define SWITCH_2           (KE_PORTE_BIT4)                       // SW2 if the port is changed (eg. A to D) the port macros will require appropriate adjustment too
        #define SWITCH_3           (KE_PORTE_BIT5)                       // SW3 if the port is changed (eg. A to D) the port macros will require appropriate adjustment too
        #define INIT_WATCHDOG_DISABLE() _CONFIG_PORT_INPUT(B, (SWITCH_2 | SWITCH_3), PORT_PS_UP_ENABLE) // configure as inputs
        #define FORCE_BOOT()       (_READ_PORT_MASK(B, SWITCH_2) != 0)   // pull this input down to force boot loader mode (hold SW2 at reset)
        #define WATCHDOG_DISABLE() (_READ_PORT_MASK(B, SWITCH_3) != 0)   // pull this input down to disable watchdog (hold SW3 at reset)
    #endif

    #define TOGGLE_WATCHDOG_LED()  _TOGGLE_PORT(B, BLINK_LED)

#elif defined TRK_KEA128 || defined TRK_KEA64
    #define BLINK_LED              (KE_PORTC_BIT0)                        // if the port is changed (eg. A to D) the port macros will require appropriate adjustment too
    #define SWITCH_1               (KE_PORTD_BIT0)                        // if the port is changed (eg. A to D) the port macros will require appropriate adjustment too
    #define SWITCH_2               (KE_PORTD_BIT1)                        // if the port is changed (eg. A to D) the port macros will require appropriate adjustment too

    #define INIT_WATCHDOG_LED()    _CONFIG_DRIVE_PORT_OUTPUT_VALUE(A, (BLINK_LED), (BLINK_LED), (PORT_SRE_SLOW | PORT_DSE_HIGH))
    #define INIT_WATCHDOG_DISABLE() _CONFIG_PORT_INPUT(A, (SWITCH_1 | SWITCH_2), PORT_NO_PULL) // configure as inputs

    #define WATCHDOG_DISABLE()     (_READ_PORT_MASK(A, SWITCH_2) != 0)   // hold SW2 down at reset to disable watchdog
    #define FORCE_BOOT()           (_READ_PORT_MASK(A, SWITCH_1) != 0)   // hold SW1 down at reset to force boot loader mode

    #define TOGGLE_WATCHDOG_LED()  _TOGGLE_PORT(A, BLINK_LED)
#elif defined FRDM_KL02Z
    #define BLINK_LED              (PORTB_BIT7)                          // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define SWITCH_1               (PORTA_BIT11)                         // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define SWITCH_2               (PORTB_BIT5)                          // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too

    #define INIT_WATCHDOG_LED()    _CONFIG_DRIVE_PORT_OUTPUT_VALUE(B, (BLINK_LED), (BLINK_LED), (PORT_SRE_SLOW | PORT_DSE_HIGH))
    #define INIT_WATCHDOG_DISABLE() _CONFIG_PORT_INPUT_FAST_LOW(B, (SWITCH_2), PORT_PS_UP_ENABLE); _CONFIG_PORT_INPUT_FAST_LOW(A, (SWITCH_1), PORT_PS_UP_ENABLE) // configure as input

    #define WATCHDOG_DISABLE()     (_READ_PORT_MASK(B, SWITCH_2) == 0)   // pull this input down to disable watchdog (J8-4)
    #define FORCE_BOOT()           (_READ_PORT_MASK(A, SWITCH_1) == 0)   // pull this input down to force boot loader mode (J8-3)
    #define RETAIN_LOADER_MODE()   (_READ_PORT_MASK(A, SWITCH_1) == 0)

    #define TOGGLE_WATCHDOG_LED()   _TOGGLE_PORT(B, BLINK_LED)
#elif defined FRDM_KL03Z
    #define BLINK_LED              (PORTB_BIT11)                         // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define SWITCH_2               (PORTB_BIT0)                          // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define SWITCH_3               (PORTB_BIT5)                          // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too

    #define INIT_WATCHDOG_LED()    _CONFIG_DRIVE_PORT_OUTPUT_VALUE_FAST_LOW(B, (BLINK_LED), (BLINK_LED), (PORT_SRE_SLOW | PORT_DSE_HIGH))
    #define INIT_WATCHDOG_DISABLE() _CONFIG_PORT_INPUT_FAST_LOW(B, (SWITCH_2 | SWITCH_3), PORT_PS_UP_ENABLE) // configure as inputs

    #define WATCHDOG_DISABLE()     (_READ_PORT_MASK(B, SWITCH_2) == 0)   // hold SW2 down at reset to disable watchdog
    #define FORCE_BOOT()           (_READ_PORT_MASK(B, SWITCH_3) == 0)   // hold SW3 down at reset to force boot loader mode
    #define RETAIN_LOADER_MODE()   (_READ_PORT_MASK(B, SWITCH_3) == 0)

    #define TOGGLE_WATCHDOG_LED()   _TOGGLE_PORT(B, BLINK_LED)
#elif defined FRDM_KL05Z                                                 // {8}
    #define BLINK_LED              (PORTB_BIT10)                         // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define SWITCH_1               (PORTA_BIT11)                         // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define SWITCH_2               (PORTB_BIT5)                          // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too

    #define INIT_WATCHDOG_LED()    _CONFIG_DRIVE_PORT_OUTPUT_VALUE(B, (BLINK_LED), (BLINK_LED), (PORT_SRE_SLOW | PORT_DSE_HIGH))
    #define INIT_WATCHDOG_DISABLE() _CONFIG_PORT_INPUT_FAST_LOW(B, (SWITCH_2), PORT_PS_UP_ENABLE); _CONFIG_PORT_INPUT_FAST_LOW(A, (SWITCH_1), PORT_PS_UP_ENABLE) // configure as input

    #define WATCHDOG_DISABLE()     (_READ_PORT_MASK(B, SWITCH_2) == 0)   // pull this input down to disable watchdog (J8-4)
    #define FORCE_BOOT()           (_READ_PORT_MASK(A, SWITCH_1) == 0)   // pull this input down to force boot loader mode (J8-3)
    #define RETAIN_LOADER_MODE()   (_READ_PORT_MASK(A, SWITCH_1) == 0)

    #define TOGGLE_WATCHDOG_LED()   _TOGGLE_PORT(B, BLINK_LED)
#elif defined TWR_KL25Z48M                                               // {7}
    #define BLINK_LED              (PORTA_BIT17)                         // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define SWITCH_3               (PORTA_BIT4)                          // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define SWITCH_4               (PORTC_BIT3)                          // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too

    #define INIT_WATCHDOG_LED()    _CONFIG_DRIVE_PORT_OUTPUT_VALUE(A, (BLINK_LED), (BLINK_LED), (PORT_SRE_SLOW | PORT_DSE_HIGH))
    #define INIT_WATCHDOG_DISABLE() _CONFIG_PORT_INPUT_FAST_LOW(C, (SWITCH_4), PORT_PS_UP_ENABLE); _CONFIG_PORT_INPUT_FAST_LOW(A, (SWITCH_3), PORT_PS_UP_ENABLE) // configure as input

    #define WATCHDOG_DISABLE()     (_READ_PORT_MASK(A, SWITCH_3) == 0)   // pull this input down to disable watchdog (SW3)
    #define FORCE_BOOT()           (_READ_PORT_MASK(C, SWITCH_4) == 0)   // pull this input down to force boot loader mode (SW4)
    #define RETAIN_LOADER_MODE()   (_READ_PORT_MASK(C, SWITCH_4) == 0)

    #define TOGGLE_WATCHDOG_LED()   _TOGGLE_PORT(A, BLINK_LED)
#elif defined FRDM_KL25Z                                                 // {7}
    #if defined KL25_TEST_BOARD
        #define BLINK_LED              (PORTE_BIT2)                      // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
        #define INIT_WATCHDOG_LED()    _CONFIG_DRIVE_PORT_OUTPUT_VALUE(E, (BLINK_LED), (BLINK_LED), (PORT_SRE_SLOW | PORT_DSE_HIGH))
        #define INIT_WATCHDOG_DISABLE()
        #define WATCHDOG_DISABLE()     1
        #define FORCE_BOOT()           1
        #define TOGGLE_WATCHDOG_LED()   _TOGGLE_PORT(E, BLINK_LED)
        #define RX_UART_TO_HOLD_LOADER                                   // press key ? within 500ms to hold the serial loader, or else jump to the application
    #else
        #define BLINK_LED              (PORTB_BIT19)                     // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
        #define SWITCH_1               (PORTD_BIT4)                      // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
        #define SWITCH_2               (PORTA_BIT12)                     // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
        #define SDCARD_DETECT          (PORTB_BIT8)                      // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too

        #if defined SDCARD_SUPPORT || defined SPI_FLASH_FAT
            #define INIT_WATCHDOG_LED() _CONFIG_DRIVE_PORT_OUTPUT_VALUE(B, (BLINK_LED), (BLINK_LED), (PORT_SRE_SLOW | PORT_DSE_HIGH)); _CONFIG_PORT_INPUT(B, (SDCARD_DETECT), PORT_PS_UP_ENABLE)
        #else
            #define INIT_WATCHDOG_LED() _CONFIG_DRIVE_PORT_OUTPUT_VALUE(B, (BLINK_LED), (BLINK_LED), (PORT_SRE_SLOW | PORT_DSE_HIGH))
        #endif
        #define INIT_WATCHDOG_DISABLE() _CONFIG_PORT_INPUT_FAST_LOW(D, (SWITCH_1), PORT_PS_UP_ENABLE); _CONFIG_PORT_INPUT_FAST_LOW(A, (SWITCH_2), PORT_PS_UP_ENABLE) // configure as input

        #define WATCHDOG_DISABLE()     (_READ_PORT_MASK(A, SWITCH_2) == 0)   // pull this input down to disable watchdog (J1-8)

        #if defined SDCARD_SUPPORT || defined SPI_FLASH_FAT
            #define FORCE_BOOT()       ((_READ_PORT_MASK(D, SWITCH_1) == 0) || (_READ_PORT_MASK(B, SDCARD_DETECT) == 0)) // pull input on J1-6 down at reset or with inserted SD card
        #else
            #define FORCE_BOOT()       (_READ_PORT_MASK(D, SWITCH_1) == 0)   // pull this input down to force boot loader mode (J1-6)
        #endif
        #define TOGGLE_WATCHDOG_LED()   _TOGGLE_PORT(B, BLINK_LED)
    #endif

    #define RETAIN_LOADER_MODE()   (_READ_PORT_MASK(B, SDCARD_DETECT) == 0)
    // Configure to suit special connection SPI mode at between 100k and 400k (SPI1)
    // - SPI1_CS   PTD-4 (J1-6) [VDD J9-4 / 0V J9-14] card detect input on PTB-8 (J9-1)
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
    #ifdef _WINDOWS
        #define WRITE_SPI_CMD(byte)    SPI1_D = (byte); SPI1_D = _fnSimSD_write((unsigned char)byte)
        #define WAIT_TRANSMISSON_END() while (!(SPI1_S & (SPI_S_SPRF))) { SPI1_S |= (SPI_S_SPRF); }
        #define READ_SPI_DATA()        (unsigned char)SPI1_D
    #else
        #define WRITE_SPI_CMD(byte)    SPI1_D = (byte)
        #define WAIT_TRANSMISSON_END() while (!(SPI1_S & (SPI_S_SPRF))) {}
        #define READ_SPI_DATA()        (unsigned char)SPI1_D
    #endif
    #define SET_SD_DI_CS_HIGH()  _SETBITS(D, SPI_CS1_0)                  // force DI and CS lines high ready for the initialisation sequence
    #define SET_SD_CS_LOW()      _CLEARBITS(D, SPI_CS1_0)                // assert the CS line of the SD card to be read
    #define SET_SD_CS_HIGH()     _SETBITS(D, SPI_CS1_0)                  // negate the CS line of the SD card to be read
    #define POWER_UP_SD_CARD()                                           // apply power to the SD card if appropriate
    #define POWER_DOWN_SD_CARD()
    #define GET_SDCARD_WP_STATE()  1                                     // always write protect

    #define DEL_USB_SYMBOL()
    #define SET_USB_SYMBOL()
    #define USB_HOST_POWER_CONFIG()
    #define USB_HOST_POWER_ON()                                          // the FRDM-KL25Z doesn't have a USB power supply that can be controlled, instead jumper J21 can be manually shorted so that the 5V power from the OpenSDA circuit is connected (use carefully since there is no protection!)
    #define USB_HOST_POWER_OFF()
#elif defined FRDM_KL26Z                                                 // {7}
    #define BLINK_LED              (PORTE_BIT31)                         // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define SWITCH_1               (PORTC_BIT3)                          // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define SWITCH_2               (PORTA_BIT12)                         // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too

    #define INIT_WATCHDOG_LED()    _CONFIG_DRIVE_PORT_OUTPUT_VALUE(E, (BLINK_LED), (BLINK_LED), (PORT_SRE_SLOW | PORT_DSE_HIGH))
    #define INIT_WATCHDOG_DISABLE() _CONFIG_PORT_INPUT_FAST_LOW(C, (SWITCH_1), PORT_PS_UP_ENABLE); _CONFIG_PORT_INPUT_FAST_LOW(A, (SWITCH_2), PORT_PS_UP_ENABLE) // configure as input

    #define WATCHDOG_DISABLE()     (_READ_PORT_MASK(A, SWITCH_2) == 0)   // pull this input down to disable watchdog
    #define FORCE_BOOT()           (_READ_PORT_MASK(C, SWITCH_1) == 0)   // pull this input down to force boot loader mode
    #define RETAIN_LOADER_MODE()   (_READ_PORT_MASK(C, SWITCH_1) == 0)

    #define TOGGLE_WATCHDOG_LED()   _TOGGLE_PORT(E, BLINK_LED)
#elif defined FRDM_KL27Z
    #define BLINK_LED              (PORTB_BIT19)                         // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define SWITCH_1               (PORTA_BIT4)                          // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define SWITCH_2               (PORTC_BIT1)                          // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too

    #define INIT_WATCHDOG_LED()    _CONFIG_DRIVE_PORT_OUTPUT_VALUE(B, (BLINK_LED), (BLINK_LED), (PORT_SRE_SLOW | PORT_DSE_HIGH))
    #define INIT_WATCHDOG_DISABLE() _CONFIG_PORT_INPUT_FAST_LOW(A, (SWITCH_1), PORT_PS_UP_ENABLE); _CONFIG_PORT_INPUT_FAST_LOW(C, (SWITCH_2), PORT_PS_UP_ENABLE) // configure as input

    #define WATCHDOG_DISABLE()     (_READ_PORT_MASK(C, SWITCH_2) == 0)   // pull this input down to disable watchdog
    #define FORCE_BOOT()           (_READ_PORT_MASK(A, SWITCH_1) == 0)   // pull this input down to force boot loader mode
    #define RETAIN_LOADER_MODE()   (_READ_PORT_MASK(A, SWITCH_1) == 0)

    #define TOGGLE_WATCHDOG_LED()   _TOGGLE_PORT(B, BLINK_LED)
#elif defined TEENSY_LC
    #define BLINK_LED              (PORTC_BIT5)                          // (green LED) if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define SWITCH_1               (PORTB_BIT0)                          // (pin 16) if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define SWITCH_2               (PORTB_BIT1)                          // (pin 17) if the port is changed (eg. A to B) the port macros will require appropriate adjustment too

    #define INIT_WATCHDOG_LED()    _CONFIG_DRIVE_PORT_OUTPUT_VALUE(C, (BLINK_LED), (BLINK_LED), (PORT_SRE_SLOW | PORT_DSE_HIGH))
    #define INIT_WATCHDOG_DISABLE() _CONFIG_PORT_INPUT_FAST_LOW(B, (SWITCH_1 | SWITCH_2), PORT_PS_UP_ENABLE) // configure as input

    #if defined SPECIAL_VERSION
        #define WATCHDOG_DISABLE() 1                                     // watchdog is always disabled
    #else
        #define WATCHDOG_DISABLE() (_READ_PORT_MASK(B, SWITCH_2) == 0)   // pull this input down to disable watchdog (pin 17)
    #endif
    #define FORCE_BOOT()           (_READ_PORT_MASK(B, SWITCH_1) == 0)   // pull this input down to force boot loader mode (pin 16)

    #define TOGGLE_WATCHDOG_LED()   _TOGGLE_PORT(C, BLINK_LED)

        // '0'          '1'       input state   center (x,   y)   0 = circle, radius, controlling port, controlling pin 
    #define KEYPAD_LED_DEFINITIONS  \
        {RGB(0,255,0),  RGB(0,0,0),  1, {77,  51,  89,  69 }, _PORTC, BLINK_LED}, \
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

    #define KEYPAD "../../uTaskerV1.4/Simulator/KeyPads/TEENSY_LC.bmp"
#elif defined TWR_KM34Z50M
    #define BLINK_LED              (PORTE_BIT5)                          // (green LED) if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define SWITCH_2               (PORTE_BIT4)                          // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define SWITCH_1               (PORTD_BIT0)                          // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too

    #define INIT_WATCHDOG_LED()     _CONFIG_DRIVE_PORT_OUTPUT_VALUE(E, (BLINK_LED), (BLINK_LED), (PORT_SRE_SLOW | PORT_DSE_HIGH))
    #define INIT_WATCHDOG_DISABLE() _CONFIG_PORT_INPUT_FAST_LOW(D, (SWITCH_1), PORT_PS_UP_ENABLE); _CONFIG_PORT_INPUT_FAST_LOW(E, (SWITCH_2), PORT_PS_UP_ENABLE) // configure as inputs

    #define WATCHDOG_DISABLE()     (_READ_PORT_MASK(E, SWITCH_2) == 0)   // hold switch 2 down at reset to disable watchdog
    #define FORCE_BOOT()           (_READ_PORT_MASK(D, SWITCH_1) == 0)   // hold switch 1 down at reset to force boot loader mode
    #define RETAIN_LOADER_MODE()   (_READ_PORT_MASK(D, SWITCH_1) == 0)   // hold switch 1 down to retain boot loader mode

    #define TOGGLE_WATCHDOG_LED()  _TOGGLE_PORT(E, BLINK_LED); if (SIM_SCGC5 & SIM_SCGC5_SLCD) { TOGGLE_SLCD(23TO20, 0x0000000f); TOGGLE_SLCD(27TO24, 0x00000002); }

    #define SLCD_FILE   "SLCD\\TWR-KM34Z50M.lcd"                         // SLCD simulation file in the simulator directory SLCD
  //#define BIG_PIXEL                                                    // show SLCD double size
    #define LCD_ON_COLOUR      (COLORREF)RGB(210, 220, 210)              // RGB colour of LCD when backlight is on
    #define LCD_OFF_COLOUR     (COLORREF)RGB(10, 10, 10)                 // RGB colour of LCD when backlight is off
    #define LCD_PIXEL_COLOUR   (COLORREF)RGB(0,0,0)                      // RGB colour of LCD pixels
    #define GLCD_X  260
    #define GLCD_Y  95

    // SLCD configuration with clock from MCGIRCLK (2MHz) divided by 64
    //
    #define CONFIGURE_SLCD()       MCG_C1 |= MCG_C1_IRCLKEN; \
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
                                   LCD_GCR = (LCD_GCR_LCDEN | (0x0b000000 & LCD_GCR_RVTRIM_MASK) | LCD_GCR_CPSEL | LCD_GCR_LADJ_MASK | LCD_GCR_VSUPPLY | LCD_GCR_ALTDIV_64 | LCD_GCR_SOURCE | LCD_GCR_LCLK_1 | LCD_GCR_DUTY_4BP);
                                   
    // Display "USb" in SLCD
    //
    #define DEL_USB_SYMBOL()       CLEAR_SLCD(27TO24, 0x060b0000); CLEAR_SLCD(43TO40, 0x0f0a000d); CLEAR_SLCD(47TO44, 0x00000002)
    #define SET_USB_SYMBOL()       SET_SLCD(27TO24,   0x060b0000); SET_SLCD(43TO40,   0x0f0a000d); SET_SLCD(47TO44,   0x00000002)
#elif defined TWR_KW21D256 || defined TWR_KW24D512
    #define BLINK_LED              (PORTD_BIT4)                          // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define SWITCH_1               (PORTC_BIT4)                          // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define SWITCH_2               (PORTC_BIT5)                          // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define SWITCH_3               (PORTC_BIT6)                          // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define SWITCH_4               (PORTC_BIT7)                          // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too

    #define INIT_WATCHDOG_LED()    _CONFIG_DRIVE_PORT_OUTPUT_VALUE_FAST_LOW(D, (BLINK_LED), (BLINK_LED), (PORT_SRE_SLOW | PORT_DSE_HIGH))

    #define INIT_WATCHDOG_DISABLE() _CONFIG_PORT_INPUT_FAST_LOW(C, (SWITCH_1 | SWITCH_2), PORT_PS_UP_ENABLE) // configure as inputs
    #define WATCHDOG_DISABLE()     (_READ_PORT_MASK(C, SWITCH_2) == 0)   // hold SW2 down at reset to disable watchdog
    #define FORCE_BOOT()           (_READ_PORT_MASK(C, SWITCH_1) == 0)   // hold SW3 down at reset to force boot loader mode
    #define RETAIN_LOADER_MODE()   (_READ_PORT_MASK(C, SWITCH_1) == 0)
    #define TOGGLE_WATCHDOG_LED()  _TOGGLE_PORT(D, BLINK_LED)
#elif defined FRDM_KL43Z
    #define BLINK_LED              (PORTD_BIT5)                          // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define SWITCH_3               (PORTC_BIT3)                          // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define SWITCH_1               (PORTA_BIT4)                          // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too

    #define INIT_WATCHDOG_LED()     _CONFIG_DRIVE_PORT_OUTPUT_VALUE(D, (BLINK_LED), (BLINK_LED), (PORT_SRE_SLOW | PORT_DSE_HIGH))
    #define INIT_WATCHDOG_DISABLE() _CONFIG_PORT_INPUT_FAST_LOW(C, (SWITCH_3), PORT_PS_UP_ENABLE); _CONFIG_PORT_INPUT_FAST_LOW(A, (SWITCH_1), PORT_PS_UP_ENABLE) // configure as inputs

    #define WATCHDOG_DISABLE()     (_READ_PORT_MASK(C, SWITCH_3) == 0)   // hold switch 3 down at reset to disable watchdog [may need jumper added]
    #define FORCE_BOOT()           (_READ_PORT_MASK(A, SWITCH_1) == 0)   // hold switch 1 down at reset to force boot loader mode
    #define RETAIN_LOADER_MODE()   (_READ_PORT_MASK(A, SWITCH_1) == 0)   // hold switch 1 down to retain boot loader mode

    #define TOGGLE_WATCHDOG_LED()  _TOGGLE_PORT(D, BLINK_LED); if (SIM_SCGC5 & SIM_SCGC5_SLCD) { TOGGLE_SLCD(23TO20, 0x0000000f); TOGGLE_SLCD(27TO24, 0x00000002); }

    #define SLCD_FILE   "SLCD\\FRDM-KL43Z.lcd"                           // SLCD simulation file in the simulator directory SLCD
  //#define BIG_PIXEL                                                    // show SLCD double size
    #define LCD_ON_COLOUR      (COLORREF)RGB(210, 220, 210)              // RGB colour of LCD when backlight is on
    #define LCD_OFF_COLOUR     (COLORREF)RGB(10, 10, 10)                 // RGB colour of LCD when backlight is off
    #define LCD_PIXEL_COLOUR   (COLORREF)RGB(0,0,0)                      // RGB colour of LCD pixels
    #define GLCD_X  260
    #define GLCD_Y  95

    // SLCD configuration with clock from MCGIRCLK (2MHz) divided by 64
    //
    #define CONFIGURE_SLCD()       MCG_C1 |= MCG_C1_IRCLKEN; \
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
    #define DEL_USB_SYMBOL()       CLEAR_SLCD(27TO24, 0x060b0000); CLEAR_SLCD(43TO40, 0x0f0a000d); CLEAR_SLCD(47TO44, 0x00000002)
    #define SET_USB_SYMBOL()       SET_SLCD(27TO24,   0x060b0000); SET_SLCD(43TO40,   0x0f0a000d); SET_SLCD(47TO44,   0x00000002)
#elif defined TWR_KL43Z48M
    #define BLINK_LED              (PORTA_BIT12)                         // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define SWITCH_2               (PORTA_BIT4)                          // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define SWITCH_3               (PORTA_BIT5)                          // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too

    #define INIT_WATCHDOG_LED()     _CONFIG_DRIVE_PORT_OUTPUT_VALUE(A, (BLINK_LED), (BLINK_LED), (PORT_SRE_SLOW | PORT_DSE_HIGH))
    #define INIT_WATCHDOG_DISABLE() _CONFIG_PORT_INPUT_FAST_LOW(A, (SWITCH_2 | SWITCH_3), PORT_PS_UP_ENABLE) // configure as inputs

    #define WATCHDOG_DISABLE()    (_READ_PORT_MASK(A, SWITCH_3) == 0)    // hold switch 3 down at reset to disable watchdog [may need jumper added]
    #define FORCE_BOOT()          (_READ_PORT_MASK(A, SWITCH_2) == 0)    // hold switch 2 down at reset to force boot loader mode
    #define RETAIN_LOADER_MODE()  (_READ_PORT_MASK(A, SWITCH_2) == 0)    // hold switch 2 down to retain boot loader mode

    #define TOGGLE_WATCHDOG_LED() _TOGGLE_PORT(A, BLINK_LED); if (SIM_SCGC5 & SIM_SCGC5_SLCD) { TOGGLE_SLCD(24, 0x08); }

    #define SLCD_FILE             "SLCD\\TWR_KL46Z48M.lcd"               // SLCD simulation file in the simulator directory SLCD
  //#define BIG_PIXEL                                                    // show SLCD double size
    #define LCD_ON_COLOUR         (COLORREF)RGB(210, 220, 210)           // RGB colour of LCD when backlight is on
    #define LCD_OFF_COLOUR        (COLORREF)RGB(10, 10, 10)              // RGB colour of LCD when backlight is off
    #define LCD_PIXEL_COLOUR      (COLORREF)RGB(0,0,0)                   // RGB colour of LCD pixels
    #define GLCD_X  380
    #define GLCD_Y  90

    // SLCD configuration with clock from MCGIRCLK (8MHz) divided by 256
    //
    #define CONFIGURE_SLCD()       MCG_C1 |= MCG_C1_IRCLKEN; \
                                   POWER_UP(5, SIM_SCGC5_SLCD); \
                                   LCD_GCR = (LCD_GCR_CPSEL | LCD_GCR_LADJ_MASK | LCD_GCR_ALTDIV_256 | LCD_GCR_SOURCE | LCD_GCR_LCLK_1 | LCD_GCR_DUTY_4BP); \
                                   LCD_BPENL = (SLCD_PIN_12 | SLCD_PIN_13 | SLCD_PIN_14 | SLCD_PIN_15); \
                                   LCD_BPENH = 0x00000000; \
                                   LCD_PENL = ((SLCD_PIN_12 | SLCD_PIN_13 | SLCD_PIN_14 | SLCD_PIN_15) | (SLCD_PIN_0 | SLCD_PIN_1 | SLCD_PIN_3 | SLCD_PIN_20 | SLCD_PIN_22 | SLCD_PIN_23 | SLCD_PIN_24)); \
                                   LCD_PENH = 0x00000000; \
                                   fnClearSLCD(); \
                                   WRITE_SLCD(15TO12, 0x08040201); \
                                   LCD_GCR = (LCD_GCR_LCDEN | (LCD_GCR_CPSEL | LCD_GCR_LADJ_MASK | LCD_GCR_ALTDIV_256 | LCD_GCR_SOURCE | LCD_GCR_LCLK_1 | LCD_GCR_DUTY_4BP))

    // Display "USb" in SLCD
    //
    #define DEL_USB_SYMBOL()       CLEAR_SLCD(22, 0x0e); CLEAR_SLCD(24, 0x05); CLEAR_SLCD(20, 0x03); CLEAR_SLCD(3, 0x0d); CLEAR_SLCD(1, 0x07); CLEAR_SLCD(23, 0x0c)
    #define SET_USB_SYMBOL()         SET_SLCD(22, 0x0e);   SET_SLCD(24, 0x05);   SET_SLCD(20, 0x03);   SET_SLCD(3, 0x0d);   SET_SLCD(1, 0x07);   SET_SLCD(23, 0x0c)
#elif defined FRDM_KL46Z                                                 // {5}
    #define BLINK_LED              (PORTD_BIT5)                          // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define SWITCH_1               (PORTC_BIT3)                          // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define SWITCH_2               (PORTC_BIT12)                         // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too

    #define INIT_WATCHDOG_LED()     _CONFIG_DRIVE_PORT_OUTPUT_VALUE(D, (BLINK_LED), (BLINK_LED), (PORT_SRE_SLOW | PORT_DSE_HIGH))
    #define INIT_WATCHDOG_DISABLE() _CONFIG_PORT_INPUT_FAST_LOW(C, (SWITCH_1 | SWITCH_2), PORT_PS_UP_ENABLE) // configure as input

    #define WATCHDOG_DISABLE()     (_READ_PORT_MASK(C, SWITCH_2) == 0)   // pull this input down at reset to disable watchdog [J1 pin 3]
    #define FORCE_BOOT()           (_READ_PORT_MASK(C, SWITCH_1) == 0)   // pull this input down at reset to force boot loader mode [J1 pin 1]
    #define RETAIN_LOADER_MODE()   (_READ_PORT_MASK(C, SWITCH_1) == 0)   // hold this input down to retain boot loader mode [J1 pin 1]

    #define TOGGLE_WATCHDOG_LED()  _TOGGLE_PORT(D, BLINK_LED); if (SIM_SCGC5 & SIM_SCGC5_SLCD) { TOGGLE_SLCD(39TO36, 0x00000f00); TOGGLE_SLCD(19TO16, 0x00000200); }

    #define SLCD_FILE              "SLCD\\FRDM-KL46Z.lcd"                // SLCD simulation file in the simulator directory SLCD
  //#define BIG_PIXEL                                                    // show SLCD double size
    #define LCD_ON_COLOUR          (COLORREF)RGB(210, 220, 210)          // RGB colour of LCD when backlight is on
    #define LCD_OFF_COLOUR         (COLORREF)RGB(10, 10, 10)             // RGB colour of LCD when backlight is off
    #define LCD_PIXEL_COLOUR       (COLORREF)RGB(0,0,0)                  // RGB colour of LCD pixels
    #define GLCD_X  260
    #define GLCD_Y  95

    #define CONFIGURE_SLCD()       MCG_C1 |= MCG_C1_IRCLKEN; \
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
    #define DEL_USB_SYMBOL()       CLEAR_SLCD(7TO4, 0x0b000000); CLEAR_SLCD(11TO8, 0x00000006); CLEAR_SLCD(39TO36, 0x000a0000); CLEAR_SLCD(55TO52, 0x00000d00); CLEAR_SLCD(11TO8, 0x020f0000)
    #define SET_USB_SYMBOL()       SET_SLCD(7TO4, 0x0b000000);   SET_SLCD(11TO8, 0x00000006);   SET_SLCD(39TO36, 0x000a0000);   SET_SLCD(55TO52, 0x00000d00);   SET_SLCD(11TO8, 0x020f0000)
#elif defined TWR_KL46Z48M                                               // {6}
    #define BLINK_LED              (PORTA_BIT17)                         // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define SWITCH_2               (PORTC_BIT3)                          // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define SWITCH_4               (PORTA_BIT4)                          // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define USB_HOST_POWER_ENABLE  (PORTB_BIT11)

    #define INIT_WATCHDOG_LED()     _CONFIG_DRIVE_PORT_OUTPUT_VALUE(A, (BLINK_LED), (BLINK_LED), (PORT_SRE_SLOW | PORT_DSE_HIGH))
    #define INIT_WATCHDOG_DISABLE() _CONFIG_PORT_INPUT_FAST_LOW(A, (SWITCH_4), PORT_PS_UP_ENABLE); _CONFIG_PORT_INPUT_FAST_LOW(C, (SWITCH_2), PORT_PS_UP_ENABLE) // configure as input

    #define WATCHDOG_DISABLE()    (_READ_PORT_MASK(A, SWITCH_4) == 0)    // pull this input down at reset to disable watchdog [SW4]
    #define FORCE_BOOT()          (_READ_PORT_MASK(C, SWITCH_2) == 0)    // pull this input down at reset to force boot loader mode [SW2]
    #define RETAIN_LOADER_MODE()  (_READ_PORT_MASK(C, SWITCH_2) == 0)    // hold this input down to retain boot loader mode [SW2]

    #define TOGGLE_WATCHDOG_LED() _TOGGLE_PORT(A, BLINK_LED); if (SIM_SCGC5 & SIM_SCGC5_SLCD) { TOGGLE_SLCD(24, 0x08); } // toggle LED and freescale logo in SLCD

    #define SLCD_FILE             "SLCD\\TWR_KL46Z48M.lcd"               // SLCD simulation file in the simulator directory SLCD
  //#define BIG_PIXEL                                                    // show SLCD double size
    #define LCD_ON_COLOUR         (COLORREF)RGB(210, 220, 210)           // RGB colour of LCD when backlight is on
    #define LCD_OFF_COLOUR        (COLORREF)RGB(10, 10, 10)              // RGB colour of LCD when backlight is off
    #define LCD_PIXEL_COLOUR      (COLORREF)RGB(0,0,0)                   // RGB colour of LCD pixels
    #define GLCD_X  380
    #define GLCD_Y  90

    #define CONFIGURE_SLCD()       MCG_C1 |= MCG_C1_IRCLKEN; \
                                   POWER_UP(5, SIM_SCGC5_SLCD); \
                                   LCD_GCR = (LCD_GCR_VSUPPLY | LCD_GCR_SOURCE | LCD_GCR_LCLK_4 | LCD_GCR_DUTY_4BP | LCD_GCR_ALTDIV_NONE); \
                                   LCD_BPENL = (SLCD_PIN_12 | SLCD_PIN_13 | SLCD_PIN_14 | SLCD_PIN_15); \
                                   LCD_BPENH = 0x00000000; \
                                   LCD_PENL = ((SLCD_PIN_12 | SLCD_PIN_13 | SLCD_PIN_14 | SLCD_PIN_15) | (SLCD_PIN_0 | SLCD_PIN_1 | SLCD_PIN_3 | SLCD_PIN_20 | SLCD_PIN_22 | SLCD_PIN_23 | SLCD_PIN_24)); \
                                   LCD_PENH = 0x00000000; \
                                   fnClearSLCD(); \
                                   WRITE_SLCD(15TO12, 0x08040201); \
                                   LCD_GCR = (LCD_GCR_LCDEN | LCD_GCR_VSUPPLY | LCD_GCR_SOURCE | LCD_GCR_LCLK_4 | LCD_GCR_DUTY_4BP | LCD_GCR_ALTDIV_NONE)

    // Display "USb" in SLCD
    //
    #define DEL_USB_SYMBOL()       CLEAR_SLCD(22, 0x0e); CLEAR_SLCD(24, 0x05); CLEAR_SLCD(20, 0x03); CLEAR_SLCD(3, 0x0d); CLEAR_SLCD(1, 0x07); CLEAR_SLCD(23, 0x0c)
    #define SET_USB_SYMBOL()         SET_SLCD(22, 0x0e);   SET_SLCD(24, 0x05);   SET_SLCD(20, 0x03);   SET_SLCD(3, 0x0d);   SET_SLCD(1, 0x07);   SET_SLCD(23, 0x0c)

    #define USB_HOST_POWER_CONFIG() _CONFIG_DRIVE_PORT_OUTPUT_VALUE_FAST_LOW(B, (USB_HOST_POWER_ENABLE), (0), (PORT_SRE_SLOW | PORT_DSE_HIGH))
    #define USB_HOST_POWER_ON()     _SETBITS(B, USB_HOST_POWER_ENABLE)
    #define USB_HOST_POWER_OFF()    _CLEARBITS(B, USB_HOST_POWER_ENABLE)
#elif defined TWR_KV10Z32
    #define DEMO_LED_1             (PORTC_BIT1)                          // (yellow/green LED) if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define DEMO_LED_2             (PORTE_BIT25)                         // (yellow LED) if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define DEMO_LED_3             (PORTC_BIT3)                          // (yellow/green LED) if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define DEMO_LED_4             (PORTC_BIT4)                          // (yellow LED) if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define DEMO_LED_5             (PORTD_BIT4)                          // (yellow/green LED) if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define DEMO_LED_6             (PORTD_BIT5)                          // (yellow LED) if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define DEMO_LED_7             (PORTD_BIT6)                          // (yellow/orange LED) if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define DEMO_LED_8             (PORTD_BIT7)                          // (red LED) if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define BLINK_LED              (DEMO_LED_8)
    #define SWITCH_1               (PORTA_BIT4)                          // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define SWITCH_2               (PORTB_BIT0)                          // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too

    #define INIT_WATCHDOG_DISABLE() _CONFIG_PORT_INPUT_FAST_LOW(B, (SWITCH_2), PORT_PS_UP_ENABLE) // configure as input
    #define INIT_WATCHDOG_LED()     _CONFIG_DRIVE_PORT_OUTPUT_VALUE_FAST_LOW(D, (BLINK_LED), (BLINK_LED), (PORT_SRE_SLOW | PORT_DSE_HIGH)); _CONFIG_PORT_INPUT_FAST_LOW(A, (SWITCH_1), PORT_PS_UP_ENABLE)
    #define WATCHDOG_DISABLE()      (_READ_PORT_MASK(B, SWITCH_2) == 0)  // pull this input down at reset to disable watchdog [hold SW2]
    #define TOGGLE_WATCHDOG_LED()   _TOGGLE_PORT(D, BLINK_LED)

    #define FORCE_BOOT()           (_READ_PORT_MASK(A, SWITCH_1) == 0)   // pull this input down at reset to force boot loader mode [SW1]
#elif defined TWR_KV31F120M
    #define DEMO_LED_1             (PORTD_BIT7)                          // (green LED) if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define DEMO_LED_2             (PORTB_BIT19)                         // (orange LED) if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define DEMO_LED_3             (PORTE_BIT0)                          // (red LED) if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define DEMO_LED_4             (PORTE_BIT1)                          // (yellow LED) if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define BLINK_LED              (DEMO_LED_1)
    #define SWITCH_1               (PORTC_BIT6)                          // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define SWITCH_2               (PORTC_BIT11)                         // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define SWITCH_3               (PORTA_BIT4)                         // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define SWITCH_4               (PORTE_BIT25)                         // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too

    #define INIT_WATCHDOG_DISABLE() _CONFIG_PORT_INPUT_FAST_LOW(C, (SWITCH_1 | SWITCH_2), PORT_PS_UP_ENABLE) // configure as input
    #define INIT_WATCHDOG_LED()     _CONFIG_DRIVE_PORT_OUTPUT_VALUE_FAST_LOW(D, (BLINK_LED), (BLINK_LED), (PORT_SRE_SLOW | PORT_DSE_HIGH))
    #define WATCHDOG_DISABLE()      (_READ_PORT_MASK(C, SWITCH_2) == 0)  // pull this input down at reset to disable watchdog [hold SW2]
    #define TOGGLE_WATCHDOG_LED()   _TOGGLE_PORT(D, BLINK_LED)

    #define FORCE_BOOT()           (_READ_PORT_MASK(C, SWITCH_1) == 0)   // pull this input down at reset to force boot loader mode [SW1]
#elif defined KWIKSTIK                                                   // this board doesn't have LEDs or switches
    #if !defined KWIKSTIK_V3_V4
        #define SD_CONTROLLER_AVAILABLE                                  // older kwikstik's can't use this (default then to SPI interface) but from Rev. 5 they need it
    #endif
    #define SDCARD_DETECT          (PORTE_BIT27)                         // {2}

    #define BLINK_LED              

    #if defined SDCARD_SUPPORT || defined SPI_FLASH_FAT                  // {2}
        #define INIT_WATCHDOG_LED() _CONFIG_PORT_INPUT_FAST_HIGH(E, (SDCARD_DETECT), 0)
    #else
        #define INIT_WATCHDOG_LED()                                      // no LED on this board but the freescale logo on the SLCD is used instead
    #endif

    #define SLCD_FILE   "SLCD\\KWIKSTIK.lcd"                             // SLCD simulation file in the simulator directory SLCD
  //#define BIG_PIXEL                                                    // show SLCD double size
    #define LCD_ON_COLOUR      (COLORREF)RGB(210, 220, 210)              // RGB colour of LCD when backlight is on
    #define LCD_OFF_COLOUR     (COLORREF)RGB(10,  10,  10 )              // RGB colour of LCD when backlight is off
    #define LCD_PIXEL_COLOUR   (COLORREF)RGB(0,   0,   0  )              // RGB colour of LCD pixels
    #define GLCD_X  720
    #define GLCD_Y  260

    #define CONFIGURE_SLCD()       MCG_C1 |= MCG_C1_IRCLKEN; \
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

    #define INIT_WATCHDOG_DISABLE() _CONFIG_PORT_INPUT_FAST_LOW(E, (PORTE_BIT0 | PORTE_BIT1), 0) // configure as input
    #define WATCHDOG_DISABLE()     (_READ_PORT_MASK(E, PORTE_BIT0) == 0) // right side rear connector - short pins 4 and 2 together to disable watchdog
    #define TOGGLE_WATCHDOG_LED()  if (SIM_SCGC3 & SIM_SCGC3_SLCD) { TOGGLE_SLCD(3TO0, 0x1000); } // blink freescale logo in the SLCD

    #if defined SDCARD_SUPPORT || defined SPI_FLASH_FAT                  // {2}
        #define FORCE_BOOT()       ((_READ_PORT_MASK(E, PORTE_BIT1) == 0) || (_READ_PORT_MASK(E, SDCARD_DETECT) == 0)) // right side rear connector (pins 3 and 1) or with inserted SD card
        #define RETAIN_LOADER_MODE() (_READ_PORT_MASK(E, PORTE_BIT1) == 0)
    #else
        #define FORCE_BOOT()       (_READ_PORT_MASK(E, PORTE_BIT1) == 0) // right side rear connector - short pins 3 and 1 together to force boot loader mode
    #endif

    #define DEL_USB_SYMBOL()       CLEAR_SLCD(3TO0, 0x200)
    #define SET_USB_SYMBOL()       SET_SLCD(3TO0, 0x200)

    #define WRITE_PROTECT_INPUT    PORTE_BIT27
    #if defined SD_CONTROLLER_AVAILABLE                                  // use SDHC controller rather than SPI
        #define SET_SD_CS_HIGH()
        #define SET_SD_CS_LOW()
        #ifdef _WINDOWS
            #define POWER_UP_SD_CARD()  _CONFIG_DRIVE_PORT_OUTPUT_VALUE(E, PORTE_BIT6, 0, (PORT_SRE_SLOW | PORT_DSE_LOW)); _CONFIG_PORT_INPUT(E, (WRITE_PROTECT_INPUT), (PORT_PS_UP_ENABLE)); SDHC_SYSCTL |= SDHC_SYSCTL_INITA; SDHC_SYSCTL &= ~SDHC_SYSCTL_INITA; // apply power to the SD card if appropriate (we use this to send 80 clocks - self-clearing bit)
        #else
            #define POWER_UP_SD_CARD()  _CONFIG_DRIVE_PORT_OUTPUT_VALUE(E, PORTE_BIT6, 0, (PORT_SRE_SLOW | PORT_DSE_LOW)); _CONFIG_PORT_INPUT(E, (WRITE_PROTECT_INPUT), (PORT_PS_UP_ENABLE)); SDHC_SYSCTL |= SDHC_SYSCTL_INITA; while (SDHC_SYSCTL & SDHC_SYSCTL_INITA) {}; // apply power to the SD card if appropriate (we use this to send 80 clocks)
        #endif
        #define SDHC_SYSCTL_SPEED_SLOW        (SDHC_SYSCTL_SDCLKFS_128 | SDHC_SYSCTL_DVS_2)  // 390kHz when 100MHz clock
        #define SDHC_SYSCTL_SPEED_FAST        (SDHC_SYSCTL_SDCLKFS_2 | SDHC_SYSCTL_DVS_2)    // 25MHz when 100MHz clock
        #define SET_SPI_SD_INTERFACE_FULL_SPEED() fnSetSD_clock(SDHC_SYSCTL_SPEED_FAST); SDHC_PROCTL |= SDHC_PROCTL_DTW_4BIT
    #else
        // Configure to suit SD card SPI mode at between 100k and 400k
        //
        #define SPI_CS1_0                  PORTD_BIT15
        #define INITIALISE_SPI_SD_INTERFACE() POWER_UP(6, SIM_SCGC6_SPI1); \
        _CONFIG_PORT_INPUT(E, (WRITE_PROTECT_INPUT), (PORT_PS_UP_ENABLE)); \
        _CONFIG_PORT_INPUT(E, (PORTE_BIT0), (PORT_NO_PULL)); \
        _CONFIG_PERIPHERAL(E, 2, PE_2_SPI1_SCK); _CONFIG_PERIPHERAL(E, 1, (PE_1_SPI1_SOUT | PORT_SRE_FAST | PORT_DSE_HIGH)); _CONFIG_PERIPHERAL(E, 3, (PE_3_SPI1_SIN | PORT_PS_UP_ENABLE)); \
        _CONFIG_DRIVE_PORT_OUTPUT_VALUE(D, SPI_CS1_0, SPI_CS1_0, (PORT_SRE_FAST | PORT_DSE_HIGH)); \
        SPI1_CTAR0 = (SPI_CTAR_ASC_6 | SPI_CTAR_FMSZ_8 | SPI_CTAR_CPHA | SPI_CTAR_CPOL | SPI_CTAR_BR_128); SPI1_MCR = (SPI_MCR_DIS_TXF | SPI_MCR_DIS_RXF | SPI_MCR_MSTR | SPI_MCR_DCONF_SPI | SPI_MCR_CLR_RXF | SPI_MCR_CLR_TXF | SPI_MCR_PCSIS_CS0 | SPI_MCR_PCSIS_CS1 | SPI_MCR_PCSIS_CS2 | SPI_MCR_PCSIS_CS3 | SPI_MCR_PCSIS_CS4 | SPI_MCR_PCSIS_CS5)

        #define ENABLE_SPI_SD_OPERATION()
        #define SET_SD_CARD_MODE()

        // Set maximum speed
        //
        #define SET_SPI_SD_INTERFACE_FULL_SPEED() SPI1_MCR |= SPI_MCR_HALT; SPI1_CTAR0 = (SPI_CTAR_FMSZ_8 | SPI_CTAR_CPOL | SPI_CTAR_CPHA | SPI_CTAR_BR_2); SPI1_MCR &= ~SPI_MCR_HALT;
        #ifdef _WINDOWS
            #define WRITE_SPI_CMD(byte)     SPI1_SR &= ~(SPI_SR_RFDF); SPI1_PUSHR = (byte | SPI_PUSHR_PCS_NONE | SPI_PUSHR_CTAS_CTAR0); SPI1_POPR = _fnSimSD_write((unsigned char)byte)
            #define WAIT_TRANSMISSON_END() while (!(SPI1_SR & (SPI_SR_RFDF))) { SPI1_SR |= (SPI_SR_RFDF); }
            #define READ_SPI_DATA()        (unsigned char)SPI1_POPR
        #else
            #define WRITE_SPI_CMD(byte)    SPI1_SR = (SPI_SR_RFDF); SPI1_PUSHR = (byte | SPI_PUSHR_PCS_NONE | SPI_PUSHR_CTAS_CTAR0) // clear flags before transmitting (and receiving) a single byte
            #define WAIT_TRANSMISSON_END() while (!(SPI1_SR & (SPI_SR_RFDF))) {}
            #define READ_SPI_DATA()        (unsigned char)SPI1_POPR
        #endif
        #define SET_SD_DI_CS_HIGH()  _SETBITS(D, SPI_CS1_0)              // force DI and CS lines high ready for the initialisation sequence
        #define SET_SD_CS_LOW()      _CLEARBITS(D, SPI_CS1_0)            // assert the CS line of the SD card to be read
        #define SET_SD_CS_HIGH()     _SETBITS(D, SPI_CS1_0)              // negate the CS line of the SD card to be read
        #define POWER_UP_SD_CARD()   _CONFIG_DRIVE_PORT_OUTPUT_VALUE(E, PORTE_BIT6, 0, (PORT_SRE_SLOW | PORT_DSE_LOW)); // apply power to the SD card if appropriate
    #endif
    #define POWER_DOWN_SD_CARD()     _SETBITS(E, PORTE_BIT6)             // remove power from SD card interface
    #define GET_SDCARD_WP_STATE()   (_READ_PORT_MASK(E, WRITE_PROTECT_INPUT) == 0) // when the input is read as '0' the card is protected from writes


    #define QS_3TO0_FREESCALE_LOGO           0x00001000
    #define QS_3TO0_CONNECTION_SYMBOL        0x00000100
    #define QS_3TO0_USB_SYMBOL               0x00000200
    #define QS_3TO0_CLOCK_SYMBOL             0x00000800
    #define QS_3TO0_JLINK_SYMBOL             0x00002000
    #define QS_3TO0_BATTERY_SYMBOL           0x00004000
    #define QS_3TO0_BATTERY_CHARGE_3         0x00008000

    #define QS_39TO36_POUNCE_LOGO            0x20000000
    #define QS_39TO36_BATTERY_CHARGE_2       0x40000000
    #define QS_39TO36_BATTERY_CHARGE_1       0x80000000


    #define OWN_SD_CARD_DISPLAY                                          // override defaults
    #define _DISPLAY_SD_CARD_NOT_PRESENT()   SET_SLCD(39TO36, QS_39TO36_POUNCE_LOGO)
    #define _DISPLAY_SD_CARD_NOT_FORMATTED() SET_SLCD(3TO0,   QS_3TO0_JLINK_SYMBOL)
    #define _DISPLAY_NO_FILE()               SET_SLCD(3TO0,   QS_3TO0_CONNECTION_SYMBOL)
    #define _DISPLAY_SD_CARD_PRESENT()       SET_SLCD(3TO0,   QS_3TO0_BATTERY_SYMBOL)
    #define _DISPLAY_VALID_CONTENT()         SET_SLCD(39TO36, QS_39TO36_BATTERY_CHARGE_1)
    #define _DISPLAY_INVALID_CONTENT()       SET_SLCD(3TO0,   QS_3TO0_BATTERY_CHARGE_3)
    #define _DISPLAY_SW_OK()                 SET_SLCD(39TO36, QS_39TO36_BATTERY_CHARGE_2); SET_SLCD(3TO0, QS_3TO0_BATTERY_CHARGE_3)
    #define _DISPLAY_SW_UPDATED()            SET_SLCD(39TO36, QS_39TO36_BATTERY_CHARGE_2); SET_SLCD(3TO0, QS_3TO0_BATTERY_CHARGE_3)
    #define _DISPLAY_ERROR()                 SET_SLCD(3TO0,   QS_3TO0_CLOCK_SYMBOL)
    #define _DISPLAY_OVERSIZE_CONTENT()      SET_SLCD(3TO0,   (QS_3TO0_CLOCK_SYMBOL | QS_3TO0_JLINK_SYMBOL))
#elif defined TWR_K40X256 || defined TWR_K53N512 || defined TWR_K40D100M
    #define SD_CONTROLLER_AVAILABLE                                      // user SDHC controller instead of SPI interface

    #if defined TWR_K53N512
        #define RESETOUT                  PORTC_BIT15
        #define SDCARD_DETECT             PORTE_BIT28                    // SD card detect input
        #define WRITE_PROTECT_INPUT       PORTC_BIT9
        #define GET_SDCARD_WP_STATE()    (_READ_PORT_MASK(C, WRITE_PROTECT_INPUT)) // when the input is read as '1' the card is protected from writes
        #ifdef _WINDOWS
            #define POWER_UP_SD_CARD()    _CONFIG_PORT_INPUT(C, (WRITE_PROTECT_INPUT), (PORT_PS_UP_ENABLE)); SDHC_SYSCTL |= SDHC_SYSCTL_INITA; SDHC_SYSCTL &= ~SDHC_SYSCTL_INITA; // apply power to the SD card if appropriate (we use this to send 80 clocks - self-clearing bit)
        #else
            #define POWER_UP_SD_CARD()    _CONFIG_PORT_INPUT(C, (WRITE_PROTECT_INPUT), (PORT_PS_UP_ENABLE)); SDHC_SYSCTL |= SDHC_SYSCTL_INITA; while (SDHC_SYSCTL & SDHC_SYSCTL_INITA) {}; // apply power to the SD card if appropriate (we use this to send 80 clocks)
        #endif
    #else
        #define SDCARD_DETECT             PORTA_BIT16                    // SD card detect input
        #define WRITE_PROTECT_INPUT       PORTE_BIT27                    // SD card write protection switch input
        #define GET_SDCARD_WP_STATE()    (_READ_PORT_MASK(E, WRITE_PROTECT_INPUT)) // when the input is read as '1' the card is protected from writes
        #ifdef _WINDOWS
            #define POWER_UP_SD_CARD()    _CONFIG_PORT_INPUT(E, (WRITE_PROTECT_INPUT), (PORT_PS_UP_ENABLE)); SDHC_SYSCTL |= SDHC_SYSCTL_INITA; SDHC_SYSCTL &= ~SDHC_SYSCTL_INITA; // apply power to the SD card if appropriate (we use this to send 80 clocks - self-clearing bit)
        #else
            #define POWER_UP_SD_CARD()    _CONFIG_PORT_INPUT(E, (WRITE_PROTECT_INPUT), (PORT_PS_UP_ENABLE)); SDHC_SYSCTL |= SDHC_SYSCTL_INITA; while (SDHC_SYSCTL & SDHC_SYSCTL_INITA) {}; // apply power to the SD card if appropriate (we use this to send 80 clocks)
        #endif
    #endif

    #define SDHC_SYSCTL_SPEED_SLOW    (SDHC_SYSCTL_SDCLKFS_128 | SDHC_SYSCTL_DVS_2)  // 390kHz when 100MHz clock
    #define SDHC_SYSCTL_SPEED_FAST        (SDHC_SYSCTL_SDCLKFS_2 | SDHC_SYSCTL_DVS_2)  // 25MHz when 100MHz clock
    #define SET_SPI_SD_INTERFACE_FULL_SPEED() fnSetSD_clock(SDHC_SYSCTL_SPEED_FAST); SDHC_PROCTL |= SDHC_PROCTL_DTW_4BIT

    #define SET_SD_CS_HIGH()                                             // not used in SDHC mode
    #define SET_SD_CS_LOW()
    #define POWER_DOWN_SD_CARD()                                         // remove power from SD card interface

    #define DEMO_LED_1             (PORTC_BIT7)                          // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define DEMO_LED_2             (PORTC_BIT8)                          // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define DEMO_LED_3             (PORTC_BIT9)                          // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define DEMO_LED_4             (PORTB_BIT11)                         // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define BLINK_LED              (DEMO_LED_1)
    #define SWITCH_3               (PORTC_BIT5)                          // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define SWITCH_4               (PORTC_BIT13)                         // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too

    #if defined TWR_K53N512
        #define CONFIGURE_SLCD()   MCG_C1 |= MCG_C1_IRCLKEN; \
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
        #define CONFIGURE_SLCD()   MCG_C1 |= MCG_C1_IRCLKEN; \
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

    #define SLCD_FILE   "SLCD\\TWR_K40.lcd"                              // SLCD simulation file in the simulator directory SLCD
  //#define BIG_PIXEL                                                    // show SLCD double size
    #define LCD_ON_COLOUR      (COLORREF)RGB(210, 220, 210)              // RGB colour of LCD when backlight is on
    #define LCD_OFF_COLOUR     (COLORREF)RGB(10, 10, 10)                 // RGB colour of LCD when backlight is off
    #define LCD_PIXEL_COLOUR   (COLORREF)RGB(0,0,0)                      // RGB colour of LCD pixels
    #define GLCD_X  380
    #define GLCD_Y  90

    #define INIT_WATCHDOG_DISABLE() _CONFIG_PORT_INPUT_FAST_LOW(C, SWITCH_3, PORT_PS_UP_ENABLE); // configure as input
    #define WATCHDOG_DISABLE()      (!_READ_PORT_MASK(C, SWITCH_3))      // pull this input down to disable watchdog (hold SW3 at reset)
    #define TOGGLE_WATCHDOG_LED()   _TOGGLE_PORT(C, BLINK_LED); if (SIM_SCGC3 & SIM_SCGC3_SLCD) { TOGGLE_SLCD(15TO12, 0x08000000); }

    #if defined SDCARD_SUPPORT || defined SPI_FLASH_FAT                  // {2}
        #if defined TWR_K53N512
            #define INIT_WATCHDOG_LED() _CONFIG_DRIVE_PORT_OUTPUT_VALUE(C, (BLINK_LED), (BLINK_LED), (PORT_SRE_SLOW | PORT_DSE_HIGH));  _CONFIG_PORT_INPUT(C, SWITCH_4, PORT_PS_UP_ENABLE); _CONFIG_PORT_INPUT(E, SDCARD_DETECT, PORT_PS_UP_ENABLE) // note that the force boot input is configured here and not with the INIT_WATCHDOG_DISABLE() since the watchdog must be disabled as quickly as possible
            #define FORCE_BOOT()       ((!_READ_PORT_MASK(C, SWITCH_4)) || (!_READ_PORT_MASK(E, SDCARD_DETECT))) // hold SW4 at reset or with inserted SD card
        #else
            #define INIT_WATCHDOG_LED() _CONFIG_DRIVE_PORT_OUTPUT_VALUE(C, (BLINK_LED), (BLINK_LED), (PORT_SRE_SLOW | PORT_DSE_HIGH));  _CONFIG_PORT_INPUT(C, SWITCH_4, PORT_PS_UP_ENABLE); _CONFIG_PORT_INPUT(A, SDCARD_DETECT, PORT_PS_UP_ENABLE) // note that the force boot input is configured here and not with the INIT_WATCHDOG_DISABLE() since the watchdog must be disabled as quickly as possible
            #define FORCE_BOOT()       ((!_READ_PORT_MASK(C, SWITCH_4)) || (!_READ_PORT_MASK(A, SDCARD_DETECT))) // hold SW4 at reset or with inserted SD card
        #endif
        #define RETAIN_LOADER_MODE() (_READ_PORT_MASK(C, SWITCH_4) == 0)
    #else
        #define INIT_WATCHDOG_LED() _CONFIG_DRIVE_PORT_OUTPUT_VALUE(C, (BLINK_LED), (BLINK_LED), (PORT_SRE_SLOW | PORT_DSE_HIGH));  _CONFIG_PORT_INPUT(C, SWITCH_4, PORT_PS_UP_ENABLE) // note that the force boot input is configured here and not with the INIT_WATCHDOG_DISABLE() since the watchdog must be disabled as quickly as possible
        #define FORCE_BOOT()       (_READ_PORT_MASK(C, SWITCH_4) == 0)   // pull this input down to force boot loader mode (hold SW4 at reset)
    #endif

    #define DEL_USB_SYMBOL()       CLEAR_SLCD(15TO12, 0x050e030d); CLEAR_SLCD(23TO20, 0x0007000c)
    #define SET_USB_SYMBOL()       SET_SLCD(15TO12, 0x050e030d);   SET_SLCD(23TO20, 0x0007000c)
#elif defined K20FX512_120
    #define RED_LED                (PORTA_BIT24)                         // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define GREEN_LED              (PORTA_BIT25)                         // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too
    #define BLUE_LED               (PORTA_BIT26)                         // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too

    #define SWITCH_1               (PORTA_BIT27)                         // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too

    #define SDCARD_DETECT          (PORTA_BIT11)                         // if the port is changed (eg. A to B) the port macros will require appropriate adjustment too

    #define BLINK_LED              GREEN_LED

    #if defined SDCARD_SUPPORT || defined SPI_FLASH_FAT
        #define INIT_WATCHDOG_LED() _CONFIG_DRIVE_PORT_OUTPUT_VALUE(A, (BLINK_LED), (BLINK_LED), (PORT_SRE_SLOW | PORT_DSE_HIGH)); _CONFIG_PORT_INPUT(A, (SWITCH_1 | SDCARD_DETECT), PORT_PS_UP_ENABLE); // note that the force boot input is configured here and not with the INIT_WATCHDOG_DISABLE() since the watchdog must be disabled as quickly as possible
    #else
        #define INIT_WATCHDOG_LED() _CONFIG_DRIVE_PORT_OUTPUT_VALUE(A, (BLINK_LED), (BLINK_LED), (PORT_SRE_SLOW | PORT_DSE_HIGH)); _CONFIG_PORT_INPUT(A, SWITCH_1, PORT_PS_UP_ENABLE); // note that the force boot input is configured here and not with the INIT_WATCHDOG_DISABLE() since the watchdog must be disabled as quickly as possible
    #endif

    #define INIT_WATCHDOG_DISABLE() 
    #define WATCHDOG_DISABLE()     1                                     // never disable the watchdog
    #define TOGGLE_WATCHDOG_LED()  _TOGGLE_PORT(A, BLINK_LED)

    #if defined SDCARD_SUPPORT || defined SPI_FLASH_FAT
        #define FORCE_BOOT()       ((_READ_PORT_MASK(A, SWITCH_1) == 0) || (_READ_PORT_MASK(A, SDCARD_DETECT) == 0)) // hold SW1 at reset or with inserted SD card
    #else
        #define FORCE_BOOT()       (_READ_PORT_MASK(A, SWITCH_1) == 0)   // pull this input down to force boot loader mode (hold SW1 at reset)
    #endif
    #define RETAIN_LOADER_MODE()   (_READ_PORT_MASK(A, SWITCH_1) == 0)

    #define SD_CONTROLLER_AVAILABLE                                      // use SDHC controller rather than SPI
    #define WRITE_PROTECT_INPUT     0
    #define SET_SD_CS_HIGH()
    #define SET_SD_CS_LOW()
    #ifdef _WINDOWS
        #define POWER_UP_SD_CARD() _CONFIG_PORT_INPUT(A, (SDCARD_DETECT), (PORT_PS_UP_ENABLE)); SDHC_SYSCTL |= SDHC_SYSCTL_INITA; SDHC_SYSCTL &= ~SDHC_SYSCTL_INITA; // apply power to the SD card if appropriate (we use this to send 80 clocks - self-clearing bit)
    #else
        #define POWER_UP_SD_CARD() _CONFIG_PORT_INPUT(A, (SDCARD_DETECT), (PORT_PS_UP_ENABLE)); SDHC_SYSCTL |= SDHC_SYSCTL_INITA; while (SDHC_SYSCTL & SDHC_SYSCTL_INITA) {}; // apply power to the SD card if appropriate (we use this to send 80 clocks)
    #endif
    #define SDHC_SYSCTL_SPEED_SLOW (SDHC_SYSCTL_SDCLKFS_64 | SDHC_SYSCTL_DVS_5) // 375kHz when 120MHz clock
    #define SDHC_SYSCTL_SPEED_FAST (SDHC_SYSCTL_SDCLKFS_2 | SDHC_SYSCTL_DVS_3) // 20MHz when 120MHz clock
    #define SET_SPI_SD_INTERFACE_FULL_SPEED() fnSetSD_clock(SDHC_SYSCTL_SPEED_FAST); SDHC_PROCTL |= SDHC_PROCTL_DTW_4BIT

    #define POWER_DOWN_SD_CARD()                                         // remove power from SD card interface
    #define GET_SDCARD_WP_STATE()   0

    #define SDCARD_DETECTION()     ((_READ_PORT_MASK(A, SDCARD_DETECT)) == 0)// card detection input
    #define PRIORITY_SDCARD_DETECT_PORT_INT   PRIORITY_PORT_A_INT        // port priority when using card detect switch interrupt
    #define SDCARD_DETECT_PORT      PORTA                                // interrupt is on this port
    #define SDCARD_DETECT_PIN       SD_CARD_DETECTION                    // interrupt pin

    #define OWN_SD_CARD_DISPLAY

    #define _DISPLAY_SD_CARD_NOT_PRESENT()
    #define _DISPLAY_SD_CARD_NOT_FORMATTED()
    #define _DISPLAY_NO_FILE()                    _SETBITS(A, (BLUE_LED | RED_LED))
    #define _DISPLAY_SD_CARD_PRESENT()       
    #define _DISPLAY_VALID_CONTENT()         
    #define _DISPLAY_INVALID_CONTENT()            _SETBITS(A, RED_LED)
    #define _DISPLAY_SW_OK()                      _SETBITS(A, BLUE_LED)
    #define _DISPLAY_SW_UPDATED()                 _SETBITS(A, BLUE_LED)
    #define _DISPLAY_ERROR()                      _SETBITS(A, RED_LED)
    #define _DISPLAY_OVERSIZE_CONTENT()           _SETBITS(A, (RED_LED | BLUE_LED))

    #define DEL_USB_SYMBOL()
    #define SET_USB_SYMBOL()
#endif

#if !defined OWN_SD_CARD_DISPLAY                                         // {3}
    #if defined SERIAL_INTERFACE                                         // when UART interface is available
        #if defined USB_MSD_HOST_LOADER
            #define _DISPLAY_SD_CARD_NOT_PRESENT() fnDebugMsg("Mem-Stick not present\r\n")
            #define _DISPLAY_SD_CARD_NOT_FORMATTED() fnDebugMsg("Mem-Stick not formatted\r\n")
            #define _DISPLAY_SD_CARD_PRESENT()    fnDebugMsg("Mem-Stick present\r\n")
            #define _DISPLAY_NO_FILE()            fnDebugMsg("No file on Mem-Stick\r\n")
        #else
            #define _DISPLAY_SD_CARD_NOT_PRESENT() fnDebugMsg("SD-Card not present\r\n")
            #define _DISPLAY_SD_CARD_NOT_FORMATTED() fnDebugMsg("SD-Card not formatted\r\n")
            #define _DISPLAY_SD_CARD_PRESENT()    fnDebugMsg("SD-Card present\r\n")
            #define _DISPLAY_NO_FILE()            fnDebugMsg("No file on card\r\n")
        #endif
        #define _DISPLAY_VALID_CONTENT()          fnDebugMsg("File valid\r\n")
        #define _DISPLAY_INVALID_CONTENT()        fnDebugMsg("File invalid\r\n")
        #define _DISPLAY_SW_OK()                  fnDebugMsg("Software OK\r\n")
        #define _DISPLAY_SW_UPDATED()             fnDebugMsg("Software Updated\r\n")
        #define _DISPLAY_ERROR()                  fnDebugMsg("Update failed\r\n")
        #define _DISPLAY_OVERSIZE_CONTENT()       fnDebugMsg("File oversized!\r\n")
    #else
        #define _DISPLAY_SD_CARD_NOT_PRESENT()   
        #define _DISPLAY_SD_CARD_NOT_FORMATTED() 
        #define _DISPLAY_NO_FILE()               
        #define _DISPLAY_SD_CARD_PRESENT()       
        #define _DISPLAY_VALID_CONTENT()         
        #define _DISPLAY_INVALID_CONTENT()       
        #define _DISPLAY_SW_OK()
        #define _DISPLAY_SW_UPDATED()
        #define _DISPLAY_ERROR()
        #define _DISPLAY_OVERSIZE_CONTENT()
    #endif
#endif


#define _DELETE_BOOT_MAILBOX()     *(BOOT_MAIL_BOX) = 0

#if defined KINETIS_KL                                                   // {5} KL has COP and not watchdog
    #define ACTIVATE_WATCHDOG()    SIM_COPC = (SIM_COPC_COPCLKS_1K | SIM_COPC_COPT_LONGEST) // 1.024s watchdog timeout
#elif defined KINETIS_KE
    #define ACTIVATE_WATCHDOG()    UNLOCK_WDOG(); WDOG_CS2 = (WDOG_CS2_CLK_1kHz | WDOG_CS2_FLG); WDOG_TOVAL = BIG_SHORT_WORD(2000); WDOG_WIN = 0; WDOG_CS1 = (WDOG_CS1_UPDATE | WDOG_CS1_EN); // enable watchdog with 2s timeout (allow updates later if required)
#else
    #define ACTIVATE_WATCHDOG()    UNLOCK_WDOG(); WDOG_TOVALL = (2000/5); WDOG_TOVALH = 0; WDOG_STCTRLH = (WDOG_STCTRLH_ALLOWUPDATE | WDOG_STCTRLH_STNDBYEN | WDOG_STCTRLH_WAITEN | WDOG_STCTRLH_STOPEN | WDOG_STCTRLH_WDOGEN); // watchdog enabled to generate reset on 2s timeout (allow updates later if required)
#endif


#define PORT0_DEFAULT_INPUT        0xffffffff
#define PORT1_DEFAULT_INPUT        0xffffffff
#define PORT2_DEFAULT_INPUT        0xffffffff
#if defined BLAZE_K22 && defined SDCARD_SUPPORT
    #define PORT3_DEFAULT_INPUT    0xffffff7f                            // port D - PTD7 low to detect SD card by default
#else
    #define PORT3_DEFAULT_INPUT    0xffffffff                            // port D
#endif
#define PORT4_DEFAULT_INPUT        0xffffffff
#define PORT5_DEFAULT_INPUT        0xffffffff                            // K70

#if defined KINETIS_KL                                                   // {5} KL doesn't have SIM_SCG1 and SIM_SCG3 registers
    #if defined USB_INTERFACE                                            // disable USB regulator, USB controller module, UARTs and SD card controller, disable peripheral interrupts and clear possible pending
        #define RESET_PERIPHERALS()SYSTICK_CSR = 0; \
                                   POWER_DOWN(4, (SIM_SCGC4_UART0 | SIM_SCGC4_UART1 | SIM_SCGC4_UART2 | SIM_SCGC4_UART3 | SIM_SCGC4_USBOTG)); \
                                   SIM_SOPT1_CLR(SIM_SOPT1_USBREGEN, SIM_SOPT1CFG_URWE); \
                                   IRQ0_31_CER  = 0xffffffff; \
                                   IRQ32_63_CER = 0xffffffff; \
                                   IRQ64_95_CER = 0xffffffff; \
                                   IRQ0_31_CPR  = 0xffffffff; \
                                   IRQ32_63_CPR = 0xffffffff; \
                                   IRQ64_95_CPR = 0xffffffff
    #elif defined KINETIS_KL03                                           // don't disable USB regulator
        #define RESET_PERIPHERALS()SYSTICK_CSR = 0; \
                                   POWER_DOWN(5, (SIM_SCGC5_LPUART0)); \
                                   IRQ0_31_CER  = 0xffffffff; \
                                   IRQ32_63_CER = 0xffffffff; \
                                   IRQ64_95_CER = 0xffffffff; \
                                   IRQ0_31_CPR  = 0xffffffff; \
                                   IRQ32_63_CPR = 0xffffffff; \
                                   IRQ64_95_CPR = 0xffffffff 
    #else                                                                // don't disable USB regulator
        #define RESET_PERIPHERALS()SYSTICK_CSR = 0; \
                                   POWER_DOWN(4, (SIM_SCGC4_UART0 | SIM_SCGC4_UART1 | SIM_SCGC4_UART2 | SIM_SCGC4_UART3)); \
                                   IRQ0_31_CER  = 0xffffffff; \
                                   IRQ32_63_CER = 0xffffffff; \
                                   IRQ64_95_CER = 0xffffffff; \
                                   IRQ0_31_CPR  = 0xffffffff; \
                                   IRQ32_63_CPR = 0xffffffff; \
                                   IRQ64_95_CPR = 0xffffffff 
    #endif
#elif defined KINETIS_KE
        #define RESET_PERIPHERALS()SYSTICK_CSR = 0; \
                                   POWER_DOWN(4, (SIM_SCGC4_UART0 | SIM_SCGC4_UART1 | SIM_SCGC4_UART2)); \
                                   IRQ0_31_CER  = 0xffffffff; \
                                   IRQ32_63_CER = 0xffffffff; \
                                   IRQ64_95_CER = 0xffffffff; \
                                   IRQ0_31_CPR  = 0xffffffff; \
                                   IRQ32_63_CPR = 0xffffffff; \
                                   IRQ64_95_CPR = 0xffffffff 
#else
    #if defined USB_INTERFACE                                            // disable USB regulator, USB controller module, UARTs and SD card controller, disable peripheral interrupts and clear possible pending
        #define RESET_PERIPHERALS()SYSTICK_CSR = 0; \
                                   POWER_DOWN(4, (SIM_SCGC4_UART0 | SIM_SCGC4_UART1 | SIM_SCGC4_UART2 | SIM_SCGC4_UART3 | SIM_SCGC4_USBOTG)); \
                                   POWER_DOWN(1, (SIM_SCGC1_UART4 | SIM_SCGC1_UART5)); \
                                   POWER_DOWN(2, SIM_SCGC2_ENET); \
                                   POWER_DOWN(3, (SIM_SCGC3_SDHC | SIM_SCGC3_USBHS | SIM_SCGC3_USBHSPHY)); \
                                   SIM_SOPT1_CLR(SIM_SOPT1_USBREGEN, SIM_SOPT1CFG_URWE); \
                                   IRQ0_31_CER  = 0xffffffff; \
                                   IRQ32_63_CER = 0xffffffff; \
                                   IRQ64_95_CER = 0xffffffff; \
                                   IRQ0_31_CPR  = 0xffffffff; \
                                   IRQ32_63_CPR = 0xffffffff; \
                                   IRQ64_95_CPR = 0xffffffff
    #else                                                                // don't disable USB regulator
        #define RESET_PERIPHERALS()SYSTICK_CSR = 0; \
                                   POWER_DOWN(4, (SIM_SCGC4_UART0 | SIM_SCGC4_UART1 | SIM_SCGC4_UART2 | SIM_SCGC4_UART3)); \
                                   POWER_DOWN(1, (SIM_SCGC1_UART4 | SIM_SCGC1_UART5)); \
                                   POWER_DOWN(2, SIM_SCGC2_ENET); \
                                   POWER_DOWN(3, (SIM_SCGC3_SDHC)); \
                                   IRQ0_31_CER  = 0xffffffff; \
                                   IRQ32_63_CER = 0xffffffff; \
                                   IRQ64_95_CER = 0xffffffff; \
                                   IRQ0_31_CPR  = 0xffffffff; \
                                   IRQ32_63_CPR = 0xffffffff; \
                                   IRQ64_95_CPR = 0xffffffff 
    #endif
#endif




// LAN interface
//
#if defined ETH_INTERFACE                                                // {10}
    #define NUMBER_OF_TX_BUFFERS_IN_ETHERNET_DEVICE         2            // allocate 2 buffers for transmission
    #define NUMBER_OF_RX_BUFFERS_IN_ETHERNET_DEVICE         6            // allocate 6 (full) buffers for reception
    //#define USE_MULTIPLE_BUFFERS                                       // construct the receive buffer space using multiple 256 byte buffers (improved overrun performance for total memory requirement but reduced efficiency due to extra copy)

    #define IP_RX_CHECKSUM_OFFLOAD                                       // allow the HW to perform IPv4/v6 UDP, TCP and ICMP checksum verification so that no such calculation is required in code
    #define IP_TX_CHECKSUM_OFFLOAD                                       // allow the HW to insert IPv4/v6 header checksum and so the software doesn't need to calculate and insert this
    #define IP_TX_PAYLOAD_CHECKSUM_OFFLOAD                               // allow the HW to insert IPv4/v6 payload checksum and so the software doesn't need to calculate and insert this

    #define FORCE_PAYLOAD_ICMPV6_TX                                      // calculate value since the automatic offloading doesn't do it
    #define FORCE_PAYLOAD_ICMPV6_RX                                      // perform checksum in software since the automatic offloading doesn't do it

    #ifdef USE_BUFFERED_TCP                                              // if using a buffer for TCP to allow interractive data applications (like TELNET)
        #define TCP_BUFFER            2800                               // size of TCP buffer (with USE_BUFFERED_TCP) - generous with Kinetis
        #define TCP_BUFFER_FRAME      1400                               // allow this max. TCP frame size
    #endif

    #ifdef USE_HTTP
        #define HTTP_BUFFER_LENGTH    1400                               // we send frames with this maximum amount of payload data - generous with Kinetis
    #endif


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

    #if defined USE_IP
        #define LAN_BUFFER_SIZE        1514                              // Kinetis has ample space for full IP payload (if VLAN is enabled the size is automatically adjusted to allow tags)
    #else
        #define LAN_BUFFER_SIZE        256                               // if using Ethernet without IP the buffer size can be set here
    #endif

    #define PHY_MASK                   0xfffffff0                        // don't check the revision number

    #if defined TWR_K60F120M || defined TWR_K60N512 || defined TWR_K60D100M || defined TWR_K70F120M || defined TWR_K53N512 || defined K60F150M_50M || defined TWR_K65F180M
      //#define JTAG_DEBUG_IN_USE_ERRATA_2541                            // pull the optional MII0_RXER line down to 0V to avoid disturbing JTAG_TRST - not needed when using SWD for debugging 
        #if defined TWR_K65F180M
            #define ETHERNET_RMII_CLOCK_INPUT                            // the ENET_1588_CLKIN is used as clock since a 50MHz PHY clock is not available on EXTAL
        #endif
        #if defined K60F150M_50M
            #define ETHERNET_RMII                                        // RMII mode of operation instead of MII
            #define FORCE_PHY_CONFIG                                     // activate forced configuration
            #define POLL_PHY               10000                         // PHY detection is unreliable on this board so allow this many attempts
            #define FNFORCE_PHY_CONFIG()   
            #define PHY_ADDRESS            0x01                          // address of external PHY on board
            #define SUPPORT_PORT_INTERRUPTS                              // enable port interrupts
            #define PHY_INTERRUPT_PORT     PORTB
            #define PHY_INTERRUPT          PORTB_BIT7                    // IRQ4 is used as PHY interrupt input (set J6 to position 7-8 on TWR-SER board) - this is connected to PB.7
            #define PHY_IDENTIFIER         0x00221550                    // MICREL KSZ8051RNL identifier
            #define RESET_PHY
            #define ASSERT_PHY_RST()
            #define CONFIG_PHY_STRAPS()    _CONFIG_DRIVE_PORT_OUTPUT_VALUE_FAST_LOW(B, PORTB_BIT9, 0, (PORT_SRE_SLOW | PORT_DSE_LOW))
            #define FNRESETPHY()           _SETBITS(B, PORTB_BIT9); fnDelayLoop(1000)
            #define _KSZ8081RNA                                          // use compatible register set
        #elif defined TWR_SER2                                           // {17}
            #define ETHERNET_RMII                                        // RMII mode of operation instead of MII
            #define PHY_POLL_LINK                                        // poll the link status since there is no interrupt connected
            #define INTERRUPT_TASK_PHY     TASK_NETWORK_INDICATOR        // link status reported to this task (do not use together with LAN_REPORT_ACTIVITY)

            #define PHY_ADDRESS            0x00                          // address of external PHY on board - channel A (channel B is 0x01)
            #define FNRESETPHY()
            #define PHY_IDENTIFIER         0x20005ca2                    // National/TI DP83849I identifier
            #define _DP83849I
        #else                                                            // TWR SER board
            #define ETHERNET_RMII                                        // RMII mode of operation instead of MII
            #define FORCE_PHY_CONFIG                                     // activate forced configuration
            #define FNFORCE_PHY_CONFIG()   
            #if defined TWR_K65F180M
                #define INTERRUPT_TASK_PHY     TASK_NETWORK_INDICATOR    // link status reported to this task (do not use together with LAN_REPORT_ACTIVITY)
                #define PHY_ADDRESS            0x00                      // address of external PHY on board
                #define PHY_INTERRUPT_PORT     PORTE
                #define PHY_INTERRUPT          PORTE_BIT28               // IRQ1 is used as PHY interrupt input (set J6 to position 7-8 on TWR-SER board) - this is connected to PTD15
                #define ETHERNET_MDIO_WITH_PULLUPS                       // there is no pull-up on the tower board so enable one at the MDIO input
            #else
                #define POLL_PHY               10000                     // PHY detection is unreliable on this board so allow this many attempts
                #define PHY_ADDRESS            0x01                      // address of external PHY on board
              //#define PHY_INTERRUPT_PORT     PORTB
              //#define PHY_INTERRUPT          PORTB_BIT7                // IRQ4 is used as PHY interrupt input (set J6 to position 7-8 on TWR-SER board) - this is connected to PB.7
                #define PHY_IDENTIFIER         0x00221512                // MICREL KSZ8041NL identifier
                #define FNRESETPHY()
                #if defined TWR_K53N512                                  // this tower board has a port output controlling the reset line on the elevator - it is set to an output driving '1' to avoid the PHY being held in reset
                    #define RESET_PHY
                    #define ASSERT_PHY_RST() _CONFIG_DRIVE_PORT_OUTPUT_VALUE_FAST_LOW(C, (RESETOUT), (RESETOUT), (PORT_SRE_SLOW | PORT_DSE_LOW))
                    #define CONFIG_PHY_STRAPS()                          // dummy
                #endif
            #endif
        #endif
        #if defined K60F150M_50M
            #define MII_MANAGEMENT_CLOCK_SPEED   2500000                 // 2.5MHz
        #else
            #if SYSTEM_CLOCK > 100000000
                #define MII_MANAGEMENT_CLOCK_SPEED   1000000             // reduced speed due to weak data line pull up resistor and long back-plane distance (warning - too low results in a divider overflow in MSCR)
            #else
                #define MII_MANAGEMENT_CLOCK_SPEED   800000              // reduced speed due to weak data line pull up resistor and long back-plane distance (warning - too low results in a divider overflow in MSCR)
            #endif
        #endif
    #elif defined FRDM_K64F && !defined NO_INTERNAL_ETHERNET
        #define ETHERNET_RMII                                            // RMII mode of operation instead of MII
        #define FORCE_PHY_CONFIG                                         // activate forced configuration
        #define FNFORCE_PHY_CONFIG()   
        #define PHY_ADDRESS            0x00                              // address of external PHY on board
        #define PHY_IDENTIFIER         0x00221560                        // MICREL KSZ8081RNA identifier
        #define FNRESETPHY()
        #define MII_MANAGEMENT_CLOCK_SPEED    2500000                    // typ. 2.5MHz Speed
        #define ETHERNET_MDIO_WITH_PULLUPS                               // there is no pull-up on the FRDM board so enable one at the MDIO input
        #define PHY_POLL_LINK                                            // no interrupt line connected so poll the link state
        #define INTERRUPT_TASK_PHY     TASK_NETWORK_INDICATOR            // enable link state output messages
        #define _KSZ8081RNA
    #elif defined TWR_K64F120M                                           // for ethernet operation the clock settings must be J32-ON (and J33-OFF)
        #define ETHERNET_RMII                                            // RMII mode of operation instead of MII
        #define FORCE_PHY_CONFIG                                         // activate forced configuration
      //#define POLL_PHY               10000                             // PHY detection is unreliable on this board so allow this many attempts
        #define FNFORCE_PHY_CONFIG()   
        #define PHY_ADDRESS            0x00                              // address of external PHY on board
        #define PHY_INTERRUPT_PORT     PORTD
        #define PHY_INTERRUPT          PORTD_BIT15                       // IRQ1 is used as PHY interrupt input (set J6 to position 7-8 on TWR-SER board) - this is connected to PTD15
        #define SUPPORT_PORT_INTERRUPTS                                  // enable port interrupts
        #define PHY_IDENTIFIER         0x00221512                        // MICREL KSZ8041NL identifier
        #define FNRESETPHY()
        #define MII_MANAGEMENT_CLOCK_SPEED    2500000                    // typ. 2,5MHz Speed
        #define ETHERNET_MDIO_WITH_PULLUPS                               // there is no pull-up on the board so enable one at the MDIO input

        // Internal HTML message pages
        //
        #define SUPPORT_INTERNAL_HTML_FILES                              // enable the use of files from code
    #endif
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
      //#define SPI_FLASH_PAGE_LENGTH 1024                               // size when power of 2 mode selected (only possible on D-device)
        #define SPI_FLASH_PAGES             (32 * 1024)                  // 8Meg part
    #elif defined SPI_FLASH_AT45DB321 || defined SPI_FLASH_AT45DB161
        #if defined SPI_FLASH_FAT
            #define SPI_FLASH_PAGE_LENGTH 512                            // size when power of 2 mode selected (only possible on D-device)
        #else
            #define SPI_FLASH_PAGE_LENGTH 528                            // standard page size (B/C-device only allows 528)
        #endif
        #if defined SPI_FLASH_AT45DB161
            #define SPI_FLASH_PAGES         (4 * 1024)                   // 2Meg part
        #else
            #define SPI_FLASH_PAGES         (8 * 1024)                   // 4Meg part
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

#if defined FRDM_K64F
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
    #define WAIT_SPI_RECEPTION_END()        while (!(SPI0_SR & SPI_SR_RFDF)) {}
    #define CLEAR_RECEPTION_FLAG()          SPI0_SR |= SPI_SR_RFDF
    #define SET_SPI_FLASH_MODE()                                         // this can be used to change SPI settings on-the-fly when the SPI is shared with SPI Flash and other devices
    #define REMOVE_SPI_FLASH_MODE()                                      // this can be used to change SPI settings on-the-fly when the SPI is shared with SPI Flash and other devices
#endif

#endif
