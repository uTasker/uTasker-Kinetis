/**********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher

    ---------------------------------------------------------------------
    File:      STM32.h
    Project:   Single Chip Embedded Internet
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2019
    *********************************************************************
    02.03.2012 Remove USB_FIFO_INTERMEDIATE_BUFFER to correctly handle buffered USB OUT flow control
    06.03.2012 Remove start_application() define
    15.07.2012 Add _FLOAT_PORT() macro for STM32F2/4 devices             {3}
    15.07.2012 Complete _DRIVE_PORT_OUTPUT() and _DRIVE_PORT_OUTPUT_VALUE() macros {4}
    08.09.2012 Adapt RTC for F2/F4 devices                               {5}
    12.09.2012 Add I2C channel 3                                         {6}
    18.10.2012 Add Ethernet RMII mode defines and add alternate ETH pins {7}
    19.02.2013 Correct macros using GPIOX_OTYPER to control open-drain operations {8}
    19.02.2013 Correct I2C3 error interrupt vector name                  {9}
    04.06.2013 Add USB-OTG to F1                                         {10}
    02.10.2013 Correct flash granularity to 2k for all STM32F1xx connectivity line flash sizes {11}
    25.02.2014 Add CAN_CONTROL structures                                {12}
    09.03.2014 Moved RANDOM_SEED_LOCATION and BOOT_MAIL_BOX to this header file {13}
    02.06.2014 Add TIMER_STOP_TIMER                                      {14}
    15.06.2014 Add F2/F4 DMA registers                                   {15}
    12.08.2014 ETH_MACCR made volatile                                   {16}
    21.08.2014 Correct portx_15 characteristic setting in _CONFIG_PORT_INPUT() macro (F2/F4) {17}
    29.01.2015 Adjust the MDC clock speed to respect > 100MHz settings of F2/F4 parts macro (F2/F4) {18}
    21.03.2015 Correct RCC_APB2RSTR, RCC_AHB1RSTR, RCC_AHB2RSTR and RCC_AHB3RSTR bit names {19}
    11.01.2017 Removed divide by 2 from HSI_FREQUENCY to PLL inpt for STM32F2/4/7 parts {20}
    06.05.2017 Modify external port reference from bit number to bit mask{21}
    31.08.2017 Add ADC                                                   {22}
    16.10.2017 Modify the timer frequency value to give the frequency for a square wave cycle rather than a period {23}
    17.10.2017 Timer calculations changed to reference PCLK1 rather than PCLK2 {24}
    28.11.2018 Add fnSetFlashOption() prototype                          {25}
    30.11.2018 Add cortex debug and trace registers                      {26}

*/


#if defined _WINDOWS
    #define _SIM_PORT_CHANGE   fnSimPorts();
    #define _EXCEPTION(x)      *(unsigned char *)0 = 0                   // generate exception when simulating
#else
    #define _EXCEPTION(x)                                                // ignore on target
    #ifdef COMPILE_IAR
        #include <intrinsics.h>                                          // include IAR intrinsic (disable/enable interrupt) V6
    #endif
    #define _SIM_PORT_CHANGE
#endif
#if !defined __CONFIG__
    #include "config.h"
#endif

/* =================================================================== */
/*                           struct packing control {66}               */
/* =================================================================== */

#if defined _GNU
    #define _PACK      __attribute__((__packed__))
#else
    #define _PACK
#endif
#if defined _CODE_WARRIOR_CF  
    #define __PACK_ON  #pragma pack(1) 
    #define __PACK_OFF #pragma pack() 
#elif defined _COMPILE_GHS
    #define __PACK_ON  //#pragma pack(1) 
    #define __PACK_OFF //#pragma pack(0) 
#else
    #define __PACK_ON 
    #define __PACK_OFF 
#endif

#define __interrupt

// Cover functions to alow the application to access the PHY
//
extern unsigned short _fnMIIread(unsigned char _mradr);
extern void _fnMIIwrite(unsigned char _mradr, unsigned short _mwdata);

extern void fnEnterInterrupt(int iInterruptID, unsigned char ucPriority, void(*InterruptFunc)(void));

extern void fnSetFlashOption(unsigned long ulOption, unsigned long ulOption1, unsigned long ulMask); // {25}

#define _MALLOC_ALIGN                                                    // support malloc with align option since LAN memory should be on specific boundary
#define _ALIGN_HEAP_4                                                    // all heap blocks are aligned

#define DMA_REVMEMCPY_NOT_POSSIBLE

#define SDCARD_MALLOC(x) uMalloc(x)                                      // use standard uMalloc()

#define SAVE_COMPLETE_FLASH                                              // when simulating save complete FLASH content rather than only file system content
#define CAST_POINTER_ARITHMETIC unsigned long                            // STM32 uses 32 bit pointers
#define _LITTLE_ENDIAN                                                   // STM32 always works in little endian mode

#define FLASH_START_ADDRESS             0x08000000                       // up to 2M (page 1 up to 512k)

#if defined _STM32F7XX
    #define ITCM_RAM_START_ADDRESS     0x00000000                        // instruction SRAM (accessible only by CPU)
    #define RAM_START_ADDRESS          0x20000000                        // DTCM (tightly coupled memory)
    #define DTCM_RAM_SIZE              (64 * 1024)
    #define SRAM1_SIZE                 (240 * 1024)
    #define SRAM2_SIZE                 (16 * 1024)
    #define ITCM_RAM_SIZE              (16 * 1024)
#else
    #define RAM_START_ADDRESS          0x20000000                        // up to 96k
#endif

#if defined _STM32F031
    #define ARM_MATH_CM0                                                 // cortex-M0 to be used
#elif defined _STM32L0
    #define ARM_MATH_CM0PLUS                                             // cortex-M0+ to be used
#elif defined _STM32F7XX                                                 // cortex-M7
    #define ARM_MATH_CM7
#elif defined _STM32F103X
    #define ARM_MATH_CM3                                                 // cortex-M3 to be used
#else
    #define ARM_MATH_CM4                                                 // cortex-M4 to be used
#endif

#if defined STM32_FPU
    #define __FPU_PRESENT  1
#endif

#define RTC_VALID_PATTERN       0xca35

#if !defined PERSISTENT_RAM_SIZE
    #define PERSISTENT_RAM_SIZE          0
#endif
#if defined SUPPORT_SW_RTC || defined _BOOT_LOADER
     #define NON_INITIALISED_RAM_SIZE    (16 + PERSISTENT_RAM_SIZE)
#else
     #define NON_INITIALISED_RAM_SIZE    (4 + PERSISTENT_RAM_SIZE)
#endif

#if defined _STM32F105X || defined _STM32F107X
    #define _CONNECTIVITY_LINE                                           // connectivity line
#elif defined _STM32F103X
    #define _PERFORMANCE_LINE
#elif defined _STM32F102X
    #define _ACCESS_LINE_WITH_USB
#elif defined _STM32F101X
    #define _ACCESS_LINE
#elif defined _STM32F100X
    #define _VALUE_LINE
#endif

#if defined _STM32F7XX
    #define FLASH_GRANULARITY_BOOT      (32 * 1024)                      // sector delete size (4 sections)
    #define FLASH_GRANULARITY_PARAMETER (128 * 1024)                     // sector delete size (1 section)
    #define FLASH_GRANULARITY           (256 * 1024)                     // sector delete size (3 sections)
    #define BACKUP_SRAM                 (4 * 1024)
    #define NUMBER_OF_BOOT_SECTORS      4
    #define NUMBER_OF_PARAMETER_SECTORS 1

    #define TIMER_10_AVAILABLE
    #define TIMER_11_AVAILABLE
    #define TIMER_13_AVAILABLE
    #define TIMER_14_AVAILABLE
#elif defined _STM32F2XX || defined _STM32F4XX
    #define FLASH_GRANULARITY_BOOT      (16 * 1024)                      // sector delete size (4 sections)
    #define FLASH_GRANULARITY_PARAMETER (64 * 1024)                      // sector delete size (1 section)
    #define FLASH_GRANULARITY           (128 * 1024)                     // sector delete size (7 sections)
    #define BACKUP_SRAM                 (4 * 1024)
    #define NUMBER_OF_BOOT_SECTORS      4
    #define NUMBER_OF_PARAMETER_SECTORS 1

    #define TIMER_9_AVAILABLE
    #define TIMER_10_AVAILABLE
    #define TIMER_11_AVAILABLE
    #define TIMER_12_AVAILABLE
    #define TIMER_13_AVAILABLE
    #define TIMER_14_AVAILABLE
#elif defined _STM32L0x1
    #define FLASH_GRANULARITY           (128)                            // sector delete size
    #define _ST_FLASH_UNIFORM_GRANULARITY
#elif !defined _CONNECTIVITY_LINE && (SIZE_OF_FLASH <= (128 * 1024))     // {11}
    #define FLASH_GRANULARITY           (1 * 1024)                       // sector delete size
    #define _ST_FLASH_UNIFORM_GRANULARITY
#else
    #define FLASH_GRANULARITY           (2 * 1024)                       // sector delete size
    #define _ST_FLASH_UNIFORM_GRANULARITY
#endif

#if defined _STM32F2XX || defined _STM32F4XX || defined _STM32F7XX || defined _STM32L432 || defined _STM32L0x1
    #define HSI_FREQUENCY 16000000                                       // high speed internal RC oscillator speed (+/- 1% at 25°C)
#else
    #define HSI_FREQUENCY 8000000                                        // high speed internal RC oscillator speed (+/- 1% at 25°C)
#endif

// Ethernet configuration
//
#if defined _STM32F407 || defined _STM32F417 || defined _STM32F427 || defined _STM32F429 || defined _STM32F207 || defined _STM32F107X || defined _STM32F746 // devices with Ethernet
    #define ETHERNET_AVAILABLE
#endif

// USB configuration
//
#if defined _STM32F103X
    #define USB_DEVICE_AVAILABLE
    #define USB_CAN_SRAM_SIZE       256                                  // 1024 bytes (structed as 256 short words in long words)
    #define USB_DEVICE_T_STARTUP    10                                   // 1us (10 used for safety)
#else
    #define USB_OTG_AVAILABLE
#endif

// UART configuration
//
#if defined _STM32F429 || defined _STM32F427
    #define USARTS_AVAILABLE   4
    #define UARTS_AVAILABLE    4
    #define LPUARTS_AVAILABLE  0
#elif defined _STM32F401
    #define USARTS_AVAILABLE   6                                         // 3 usable
    #define USART3_NOT_PRESENT                                           // only USART1, 2 and 6 available/usable
    #define UART4_NOT_PRESENT
    #define UART5_NOT_PRESENT
    #define UARTS_AVAILABLE    0
    #define LPUARTS_AVAILABLE  0
#elif defined _STM32L4X5 || defined _STM32L4X6
    #define USARTS_AVAILABLE   3                                         // numbering is USART1, USART2, USART3
    #define UARTS_AVAILABLE    2                                         // numbering is UART4, UART5
    #define LPUARTS_AVAILABLE  1                                         // numbering is LPUART1
#elif defined _STM32L451 || defined _STM32L452 || defined _STM32L462
    #define USARTS_AVAILABLE   3
    #define UARTS_AVAILABLE    1
    #define LPUARTS_AVAILABLE  1
#elif defined _STM32L431 || defined _STM32L433 || defined _STM32L443
    #define USARTS_AVAILABLE   3
    #define UARTS_AVAILABLE    0
    #define LPUARTS_AVAILABLE  1
#elif defined _STM32L432 || defined _STM32L442
    #define USARTS_AVAILABLE   2
    #define UARTS_AVAILABLE    0
    #define LPUARTS_AVAILABLE  1
#elif defined _STM32L0x1
    #define USARTS_AVAILABLE   2
    #define USART1_NOT_PRESENT                                           // only USART2 available/usable
    #define UARTS_AVAILABLE    0
    #define LPUARTS_AVAILABLE  1
#elif defined _STM32F031
    #define USARTS_AVAILABLE   1
    #define UARTS_AVAILABLE    0
    #define LPUARTS_AVAILABLE  0
#elif defined _STM32F103X
    #define USARTS_AVAILABLE   3
    #define UARTS_AVAILABLE    0
    #define LPUARTS_AVAILABLE  0
#else                                                                    // _STM32F4XX
    #define USARTS_AVAILABLE   4                                         // numbering is USART1, USART2, USART3, USART6
    #define UARTS_AVAILABLE    2                                         // numbering is UART4, UART5
    #define LPUARTS_AVAILABLE  0
#endif

#define STM32_UART_1          0
#define STM32_UART_2          1
#define STM32_UART_3          2
#define STM32_UART_4          3
#define STM32_UART_5          4
#define STM32_UART_6          5

// ADC configuration
//
#define ADC_CONTROLLERS 3


// Clock settings
//
#if defined DISABLE_PLL
    #undef PLL_OUTPUT_FREQ
    #if defined USE_HSI_CLOCK
        #define PLL_OUTPUT_FREQ  HSI_FREQUENCY
    #elif defined USE_MSI_CLOCK && defined _STM32L432 || defined _STM32L4X5 || defined _STM32L4X6
        #define PLL_OUTPUT_FREQ  MSI_CLOCK
        #if defined MSI_CLOCK
            #if (MSI_CLOCK == 100000)
                #define RCC_CR_MISRANGE_SETTING      RCC_CR_MSIRANGE_100k
            #elif (MSI_CLOCK == 200000)
                #define RCC_CR_MISRANGE_SETTING      RCC_CR_MSIRANGE_200k
            #elif (MSI_CLOCK == 400000)
                #define RCC_CR_MISRANGE_SETTING      RCC_CR_MSIRANGE_400k
            #elif (MSI_CLOCK == 800000)
                #define RCC_CR_MISRANGE_SETTING      RCC_CR_MSIRANGE_800k
            #elif (MSI_CLOCK == 1000000)
                #define RCC_CR_MISRANGE_SETTING      RCC_CR_MSIRANGE_1M
            #elif (MSI_CLOCK == 2000000)
                #define RCC_CR_MISRANGE_SETTING      RCC_CR_MSIRANGE_2M
            #elif (MSI_CLOCK == 4000000)
                #define RCC_CR_MISRANGE_SETTING      RCC_CR_MSIRANGE_4M
            #elif (MSI_CLOCK == 8000000)
                #define RCC_CR_MISRANGE_SETTING      RCC_CR_MSIRANGE_8M
            #elif (MSI_CLOCK == 16000000)
                #define RCC_CR_MISRANGE_SETTING      RCC_CR_MSIRANGE_16M
            #elif (MSI_CLOCK == 24000000)
                #define RCC_CR_MISRANGE_SETTING      RCC_CR_MSIRANGE_24M
            #elif (MSI_CLOCK == 32000000)
                #define RCC_CR_MISRANGE_SETTING      RCC_CR_MSIRANGE_32M
            #elif (MSI_CLOCK == 48000000)
                #define RCC_CR_MISRANGE_SETTING      RCC_CR_MSIRANGE_48M
            #else
                #error "Invalid MSI frequency!"
            #endif
        #else
            #define MSI_CLOCK    4000000                                 // default MSI speed
        #endif
        #define PLL_OUTPUT_FREQ  MSI_CLOCK
    #elif defined USE_MSI_CLOCK && defined _STM32L0x1
        #define PLL_OUTPUT_FREQ  MSI_CLOCK
        #if defined MSI_CLOCK
            #if (MSI_CLOCK == 65536)
                #define RCC_ICSCR_MISRANGE_SETTING      RCC_ICSCR_MSIRANGE_65536
            #elif (MSI_CLOCK == 131072)
                #define RCC_ICSCR_MISRANGE_SETTING      RCC_ICSCR_MSIRANGE_131072
            #elif (MSI_CLOCK == 262144)
                #define RCC_ICSCR_MISRANGE_SETTING      RCC_ICSCR_MSIRANGE_262144
            #elif (MSI_CLOCK == 524288)
                #define RCC_ICSCR_MISRANGE_SETTING      RCC_ICSCR_MSIRANGE_524288
            #elif (MSI_CLOCK == 1048000)
                #define RCC_ICSCR_MISRANGE_SETTING      RCC_ICSCR_MSIRANGE_1_048M
            #elif (MSI_CLOCK == 2097000)
                #define RCC_ICSCR_MISRANGE_SETTING      RCC_ICSCR_MSIRANGE_2_097M
            #elif (MSI_CLOCK == 4194000)
                #define RCC_ICSCR_MISRANGE_SETTING      RCC_ICSCR_MSIRANGE_4_194M
            #else
                #error "Invalid MSI frequency!"
            #endif
        #else
            #define MSI_CLOCK    4194000                                 // default MSI speed
        #endif
        #define PLL_OUTPUT_FREQ  MSI_CLOCK
    #else
        #define PLL_OUTPUT_FREQ  CRYSTAL_FREQ
    #endif
#endif
#define SYSCLK          PLL_OUTPUT_FREQ

#if defined _STM32L432 || defined _STM32L0x1 || defined _STM32L4X5 || defined _STM32L4X6
    #if defined DISABLE_PLL
    #else
    #endif
    #if HCLK_DIVIDE == 1
        #define _RCC_CFGR_HPRE_SYSCLK    RCC_CFGR_HPRE_SYSCLK
    #elif HCLK_DIVIDE == 2
        #define _RCC_CFGR_HPRE_SYSCLK    RCC_CFGR_HPRE_SYSCLK_DIV2
    #elif HCLK_DIVIDE == 4
        #define _RCC_CFGR_HPRE_SYSCLK    RCC_CFGR_HPRE_SYSCLK_DIV4
    #elif HCLK_DIVIDE == 8
        #define _RCC_CFGR_HPRE_SYSCLK    RCC_CFGR_HPRE_SYSCLK_DIV8
    #elif HCLK_DIVIDE == 16
        #define _RCC_CFGR_HPRE_SYSCLK    RCC_CFGR_HPRE_SYSCLK_DIV16
    #elif HCLK_DIVIDE == 64
        #define _RCC_CFGR_HPRE_SYSCLK    RCC_CFGR_HPRE_SYSCLK_DIV64
    #elif HCLK_DIVIDE == 128
        #define _RCC_CFGR_HPRE_SYSCLK    RCC_CFGR_HPRE_SYSCLK_DIV128
    #elif HCLK_DIVIDE == 256
        #define _RCC_CFGR_HPRE_SYSCLK    RCC_CFGR_HPRE_SYSCLK_DIV256
    #elif HCLK_DIVIDE == 512
        #define _RCC_CFGR_HPRE_SYSCLK    RCC_CFGR_HPRE_SYSCLK_DIV512
    #else
        #error "Invalid HCLK divide value!!"
    #endif

    #if PCLK1_DIVIDE == 1
        #define _RCC_CFGR_PPRE1_HCLK     RCC_CFGR_PPRE1_HCLK
    #elif PCLK1_DIVIDE == 2
        #define _RCC_CFGR_PPRE1_HCLK     RCC_CFGR_PPRE1_HCLK_DIV2
    #elif PCLK1_DIVIDE == 4
        #define _RCC_CFGR_PPRE1_HCLK     RCC_CFGR_PPRE1_HCLK_DIV4
    #elif PCLK1_DIVIDE == 8
        #define _RCC_CFGR_PPRE1_HCLK     RCC_CFGR_PPRE1_HCLK_DIV8
    #elif PCLK1_DIVIDE == 16
        #define _RCC_CFGR_PPRE1_HCLK     RCC_CFGR_PPRE1_HCLK_DIV16
    #else
        #error "Invalid PCLK1 divide value!!"
    #endif

    #if PCLK2_DIVIDE == 1
        #define _RCC_CFGR_PPRE2_HCLK     RCC_CFGR_PPRE2_HCLK
    #elif PCLK2_DIVIDE == 2
        #define _RCC_CFGR_PPRE2_HCLK     RCC_CFGR_PPRE2_HCLK_DIV2
    #elif PCLK2_DIVIDE == 4
        #define _RCC_CFGR_PPRE2_HCLK     RCC_CFGR_PPRE2_HCLK_DIV4
    #elif PCLK2_DIVIDE == 8
        #define _RCC_CFGR_PPRE2_HCLK     RCC_CFGR_PPRE2_HCLK_DIV8
    #elif PCLK2_DIVIDE == 16
        #define _RCC_CFGR_PPRE2_HCLK     RCC_CFGR_PPRE2_HCLK_DIV16
    #else
        #error "Invalid PCLK2 divide value!!"
    #endif
#elif defined _STM32F4XX || defined _STM32F7XX
    #if !defined DISABLE_PLL
        #if PLL_INPUT_DIV < 2 || PLL_INPUT_DIV > 64
            #error "PLL input must be divided by between 2 and 64"
        #endif
        #if defined USE_HSI_CLOCK
            #define PLL_INPUT_FREQUENCY  (HSI_FREQUENCY/PLL_INPUT_DIV)   // {20}
        #else
            #define PLL_INPUT_FREQUENCY  (CRYSTAL_FREQ / PLL_INPUT_DIV)
        #endif
        #if (PLL_INPUT_FREQUENCY > 2000000) || (PLL_INPUT_FREQUENCY < 1000000)
            #error "PLL input must be between 1 and 2 MHz (preference 2MHz)"
        #endif
        #if PLL_VCO_MUL < 64 || PLL_VCO_MUL > 432
            #error "PLL multipler must be between 64 and 432!!"
        #endif
        #define PLL_VCO_FREQUENCY (PLL_INPUT_FREQUENCY * PLL_VCO_MUL)
        #if PLL_VCO_FREQUENCY < 64000000 || PLL_VCO_FREQUENCY > 432000000
            #error "PLL VCO out of range!!"
        #endif

        #if PLL_POST_DIVIDE == 2
            #define PLL_P_VALUE 0
        #elif PLL_POST_DIVIDE == 4
            #define PLL_P_VALUE 1
        #elif PLL_POST_DIVIDE == 6
            #define PLL_P_VALUE 2
        #elif PLL_POST_DIVIDE == 8
            #define PLL_P_VALUE 3
        #else
            #error "PLL post divide must be 2, 4, 6 or 8"
        #endif
        #define PLL_OUTPUT_FREQ   (PLL_VCO_FREQUENCY / PLL_POST_DIVIDE)
        #define _PLL_VCO_MUL      PLL_VCO_MUL
        #if defined USB_INTERFACE                                        // if USB is to be used the ring clock frequency must be 48MHz
            #define PLL_Q_VALUE   (PLL_VCO_FREQUENCY/48000000)
            #if (PLL_Q_VALUE < 2) || (PLL_Q_VALUE > 15)
                #error "Illegal divide value for USB clock!!"
            #elif (PLL_VCO_FREQUENCY/PLL_Q_VALUE) != 48000000
                #error "48MHz USB clock not possible!!"
            #endif
        #elif defined RING_POST_DIVIDE
            #if RING_POST_DIVIDE >= 2 && RING_POST_DIVIDE <= 15
                #define PLL_Q_VALUE  RING_POST_DIVIDE
                #if (PLL_VCO_FREQUENCY/RING_POST_DIVIDE) > 48000000
                    #error "Ring clock (SDIOCLK) too high (<= 48MHz)"
                #endif
            #else
                #error "RING_POST_DIVIDE must be between 2 and 15!!"
            #endif
        #else
            #define PLL_Q_VALUE  15                                      // set lowest speed when no USB
        #endif
        #if HCLK_DIVIDE == 1
            #define _RCC_CFGR_HPRE_SYSCLK    RCC_CFGR_HPRE_SYSCLK
        #elif HCLK_DIVIDE == 2
            #define _RCC_CFGR_HPRE_SYSCLK    RCC_CFGR_HPRE_SYSCLK_DIV2
        #elif HCLK_DIVIDE == 4
            #define _RCC_CFGR_HPRE_SYSCLK    RCC_CFGR_HPRE_SYSCLK_DIV4
        #elif HCLK_DIVIDE == 8
            #define _RCC_CFGR_HPRE_SYSCLK    RCC_CFGR_HPRE_SYSCLK_DIV8
        #elif HCLK_DIVIDE == 16
            #define _RCC_CFGR_HPRE_SYSCLK    RCC_CFGR_HPRE_SYSCLK_DIV16
        #elif HCLK_DIVIDE == 64
            #define _RCC_CFGR_HPRE_SYSCLK    RCC_CFGR_HPRE_SYSCLK_DIV64
        #elif HCLK_DIVIDE == 128
            #define _RCC_CFGR_HPRE_SYSCLK    RCC_CFGR_HPRE_SYSCLK_DIV128
        #elif HCLK_DIVIDE == 256
            #define _RCC_CFGR_HPRE_SYSCLK    RCC_CFGR_HPRE_SYSCLK_DIV256
        #elif HCLK_DIVIDE == 512
            #define _RCC_CFGR_HPRE_SYSCLK    RCC_CFGR_HPRE_SYSCLK_DIV512
        #else
            #error "Invalid HCLK divide value!!"
        #endif

        #if PCLK1_DIVIDE == 1
            #define _RCC_CFGR_PPRE1_HCLK     RCC_CFGR_PPRE1_HCLK
        #elif PCLK1_DIVIDE == 2
            #define _RCC_CFGR_PPRE1_HCLK     RCC_CFGR_PPRE1_HCLK_DIV2
        #elif PCLK1_DIVIDE == 4
            #define _RCC_CFGR_PPRE1_HCLK     RCC_CFGR_PPRE1_HCLK_DIV4
        #elif PCLK1_DIVIDE == 8
            #define _RCC_CFGR_PPRE1_HCLK     RCC_CFGR_PPRE1_HCLK_DIV8
        #elif PCLK1_DIVIDE == 16
            #define _RCC_CFGR_PPRE1_HCLK     RCC_CFGR_PPRE1_HCLK_DIV16
        #else
            #error "Invalid PCLK1 divide value!!"
        #endif

        #if PCLK2_DIVIDE == 1
            #define _RCC_CFGR_PPRE2_HCLK     RCC_CFGR_PPRE2_HCLK
        #elif PCLK2_DIVIDE == 2
            #define _RCC_CFGR_PPRE2_HCLK     RCC_CFGR_PPRE2_HCLK_DIV2
        #elif PCLK2_DIVIDE == 4
            #define _RCC_CFGR_PPRE2_HCLK     RCC_CFGR_PPRE2_HCLK_DIV4
        #elif PCLK2_DIVIDE == 8
            #define _RCC_CFGR_PPRE2_HCLK     RCC_CFGR_PPRE2_HCLK_DIV8
        #elif PCLK2_DIVIDE == 16
            #define _RCC_CFGR_PPRE2_HCLK     RCC_CFGR_PPRE2_HCLK_DIV16
        #else
            #error "Invalid PCLK2 divide value!!"
        #endif
    #else
        #if defined USE_HSI_CLOCK
            #define PLL_OUTPUT_FREQ  HSI_FREQUENCY
        #else
            #define PLL_OUTPUT_FREQ  CRYSTAL_FREQ
        #endif
    #endif
#elif defined _STM32F2XX
    #if !defined DISABLE_PLL
        #if PLL_INPUT_DIV < 2 || PLL_INPUT_DIV > 64
            #error "PLL input must be divided by between 2 and 64"
        #endif
        #if defined USE_HSI_CLOCK
            #define PLL_INPUT_FREQUENCY  (HSI_FREQUENCY/PLL_INPUT_DIV)   // {20}
        #else
            #define PLL_INPUT_FREQUENCY  (CRYSTAL_FREQ / PLL_INPUT_DIV)
        #endif
        #if (PLL_INPUT_FREQUENCY > 2100000) || (PLL_INPUT_FREQUENCY < 950000)
            #error "PLL input must be between 0.95 and 2.1 MHz (preference 1MHz)"
        #endif
        #if PLL_VCO_MUL < 64 || PLL_VCO_MUL > 432
            #error "PLL multipler must be between 64 and 432!!"
        #endif
        #define PLL_VCO_FREQUENCY (PLL_INPUT_FREQUENCY * PLL_VCO_MUL)
        #if PLL_VCO_FREQUENCY < 192000000 || PLL_VCO_FREQUENCY > 432000000
            #error "PLL VCO out of range!!"
        #endif

        #if PLL_POST_DIVIDE == 2
            #define PLL_P_VALUE 0
        #elif PLL_POST_DIVIDE == 4
            #define PLL_P_VALUE 1
        #elif PLL_POST_DIVIDE == 6
            #define PLL_P_VALUE 2
        #elif PLL_POST_DIVIDE == 8
            #define PLL_P_VALUE 3
        #else
            #error "PLL post divide must be 2, 4, 6 or 8"
        #endif
        #define PLL_OUTPUT_FREQ   (PLL_VCO_FREQUENCY / PLL_POST_DIVIDE)
        #define _PLL_VCO_MUL      PLL_VCO_MUL
        #if defined USB_INTERFACE                                        // if USB is to be used the ring clock frequency must be 48MHz
            #define PLL_Q_VALUE   (PLL_VCO_FREQUENCY/48000000)
            #if (PLL_Q_VALUE < 2) || (PLL_Q_VALUE > 15)
                #error "Illegal divide value for USB clock!!"
            #elif (PLL_VCO_FREQUENCY/PLL_Q_VALUE) != 48000000
                #error "48MHz USB clock not possible!!"
            #endif
        #elif defined RING_POST_DIVIDE
            #if RING_POST_DIVIDE >= 2 && RING_POST_DIVIDE <= 15
                #define PLL_Q_VALUE  RING_POST_DIVIDE
                #if (PLL_VCO_FREQUENCY/RING_POST_DIVIDE) > 48000000
                    #error "Ring clock (SDIOCLK) too high (<= 48MHz)"
                #endif
            #else
                #error "RING_POST_DIVIDE must be between 2 and 15!!"
            #endif
        #else
            #define PLL_Q_VALUE  15                                      // set lowest speed when no USB
        #endif
        #if HCLK_DIVIDE == 1
            #define _RCC_CFGR_HPRE_SYSCLK    RCC_CFGR_HPRE_SYSCLK
        #elif HCLK_DIVIDE == 2
            #define _RCC_CFGR_HPRE_SYSCLK    RCC_CFGR_HPRE_SYSCLK_DIV2
        #elif HCLK_DIVIDE == 4
            #define _RCC_CFGR_HPRE_SYSCLK    RCC_CFGR_HPRE_SYSCLK_DIV4
        #elif HCLK_DIVIDE == 8
            #define _RCC_CFGR_HPRE_SYSCLK    RCC_CFGR_HPRE_SYSCLK_DIV8
        #elif HCLK_DIVIDE == 16
            #define _RCC_CFGR_HPRE_SYSCLK    RCC_CFGR_HPRE_SYSCLK_DIV16
        #elif HCLK_DIVIDE == 64
            #define _RCC_CFGR_HPRE_SYSCLK    RCC_CFGR_HPRE_SYSCLK_DIV64
        #elif HCLK_DIVIDE == 128
            #define _RCC_CFGR_HPRE_SYSCLK    RCC_CFGR_HPRE_SYSCLK_DIV128
        #elif HCLK_DIVIDE == 256
            #define _RCC_CFGR_HPRE_SYSCLK    RCC_CFGR_HPRE_SYSCLK_DIV256
        #elif HCLK_DIVIDE == 512
            #define _RCC_CFGR_HPRE_SYSCLK    RCC_CFGR_HPRE_SYSCLK_DIV512
        #else
            #error "Invalid HCLK divide value!!"
        #endif

        #if PCLK1_DIVIDE == 1
            #define _RCC_CFGR_PPRE1_HCLK     RCC_CFGR_PPRE1_HCLK
        #elif PCLK1_DIVIDE == 2
            #define _RCC_CFGR_PPRE1_HCLK     RCC_CFGR_PPRE1_HCLK_DIV2
        #elif PCLK1_DIVIDE == 4
            #define _RCC_CFGR_PPRE1_HCLK     RCC_CFGR_PPRE1_HCLK_DIV4
        #elif PCLK1_DIVIDE == 8
            #define _RCC_CFGR_PPRE1_HCLK     RCC_CFGR_PPRE1_HCLK_DIV8
        #elif PCLK1_DIVIDE == 16
            #define _RCC_CFGR_PPRE1_HCLK     RCC_CFGR_PPRE1_HCLK_DIV16
        #else
            #error "Invalid PCLK1 divide value!!"
        #endif

        #if PCLK2_DIVIDE == 1
            #define _RCC_CFGR_PPRE2_HCLK     RCC_CFGR_PPRE2_HCLK
        #elif PCLK2_DIVIDE == 2
            #define _RCC_CFGR_PPRE2_HCLK     RCC_CFGR_PPRE2_HCLK_DIV2
        #elif PCLK2_DIVIDE == 4
            #define _RCC_CFGR_PPRE2_HCLK     RCC_CFGR_PPRE2_HCLK_DIV4
        #elif PCLK2_DIVIDE == 8
            #define _RCC_CFGR_PPRE2_HCLK     RCC_CFGR_PPRE2_HCLK_DIV8
        #elif PCLK2_DIVIDE == 16
            #define _RCC_CFGR_PPRE2_HCLK     RCC_CFGR_PPRE2_HCLK_DIV16
        #else
            #error "Invalid PCLK2 divide value!!"
        #endif
    #else
        #if defined USE_HSI_CLOCK
            #define PLL_OUTPUT_FREQ  HSI_FREQUENCY
        #else
            #define PLL_OUTPUT_FREQ  CRYSTAL_FREQ
        #endif
    #endif
#elif defined _CONNECTIVITY_LINE
    #if !defined DISABLE_PLL
        #if defined USE_HSI_CLOCK
            #define PLL_INPUT_FREQUENCY  (HSI_FREQUENCY/2)               // when the HSI is used the input to the PLL is fixed at HSI/2
        #else
            #if defined USE_PLL2_CLOCK                                   // PLL2 used as input to the main PLL
                #if PLL2_INPUT_DIV < 1 || PLL2_INPUT_DIV > 16
                    #error "PLL2 input must be divided by between 1 and 16"
                #endif
                #define PLL2_INPUT_FREQ  (CRYSTAL_FREQ / PLL_INPUT_DIV)
                #if (PLL2_VCO_MUL < 8 || PLL2_VCO_MUL > 20) || (PLL2_VCO_MUL == 15) || (PLL2_VCO_MUL == 17) || (PLL2_VCO_MUL == 18) || (PLL2_VCO_MUL == 19)
                    #error "PLL2 multiplier must be between 8 and 14 or 16 or 20"
                #else
                    #if PLL2_VCO_MUL == 16
                        #define _PLL2_VCO_MUL    (0xe + 2)
                    #elif PLL2_VCO_MUL == 20
                        #define _PLL2_VCO_MUL    (0xf + 2)
                    #else
                        #define _PLL2_VCO_MUL    PLL2_VCO_MUL
                    #endif
                #endif
                #define PLL2_FREQ (PLL2_INPUT_FREQ * PLL2_VCO_MUL)
                #define PLL_INPUT_FREQUENCY  (PLL2_FREQ / PLL_INPUT_DIV)
            #else
                #define PLL_INPUT_FREQUENCY  (CRYSTAL_FREQ / PLL_INPUT_DIV)
            #endif
            #if PLL_INPUT_DIV < 1 || PLL_INPUT_DIV > 16
                #error "PLL input must be divided by between 1 and 16"
            #endif
        #endif
        #if (PLL_INPUT_FREQUENCY > 12000000) || (PLL_INPUT_FREQUENCY < 3000000) 
            #error "PLL input must be between 3 and 12 MHz"
        #endif
        #if PLL_VCO_MUL < 4 || PLL_VCO_MUL > 9
            #if PLL_VCO_MUL == 65                                        // special case - handled as 6.5
                #define PLL_VCO_FREQUENCY ((PLL_INPUT_FREQUENCY * 65)/10)
                #define _PLL_VCO_MUL (0xd + 2)
            #else
                #error "PLL multipler must be between 4 and 9!!"
            #endif
        #else
            #define PLL_VCO_FREQUENCY (PLL_INPUT_FREQUENCY * PLL_VCO_MUL)
            #define _PLL_VCO_MUL    PLL_VCO_MUL
        #endif
        #if PLL_VCO_FREQUENCY < 18000000 || PLL_VCO_FREQUENCY > 72000000
            #error "PLL VCO out of range!!"
        #endif
        #define PLL_OUTPUT_FREQ   (PLL_VCO_FREQUENCY)
    #else
        #if defined USE_HSI_CLOCK
            #define PLL_OUTPUT_FREQ  HSI_FREQUENCY
        #else
            #define PLL_OUTPUT_FREQ  CRYSTAL_FREQ
        #endif
    #endif
#elif defined _STM32L432 || defined _STM32L0x1 || defined _STM32L4X5 || defined _STM32L4X6
#elif defined _STM32F031
    #define PCLK1_DIVIDE      PCLK_DIVIDE                                // PCLK1 is equivalent to PCLK
    #define PCLK2_DIVIDE      PCLK_DIVIDE                                // PCLK2 is equivalent to PCLK
    #define PCLK              PCLK1
#else
    #if !defined DISABLE_PLL
        #if defined USE_HSI_CLOCK
            #define PLL_INPUT_FREQUENCY  (HSI_FREQUENCY/2)               // when the HSI is used the input to the PLL is fixed at HSI/2
        #else
            #define PLL_INPUT_FREQUENCY  (CRYSTAL_FREQ / PLL_INPUT_DIV)
            #if PLL_INPUT_DIV < 1 || PLL_INPUT_DIV > 16
                #error "PLL input must be divided by between 1 and 16"
            #endif
        #endif
        #if (PLL_INPUT_FREQUENCY > 24000000) || (PLL_INPUT_FREQUENCY < 1000000) 
            #error "PLL input must be between 1 and 24 MHz (preference 8MHz)"
        #endif
        #if PLL_VCO_MUL < 2 || PLL_VCO_MUL > 16
            #error "PLL multipler must be between 2 and 16!!"
        #endif
        #define PLL_VCO_FREQUENCY (PLL_INPUT_FREQUENCY * PLL_VCO_MUL)
        #define PLL_OUTPUT_FREQ   (PLL_VCO_FREQUENCY)
        #define _PLL_VCO_MUL      PLL_VCO_MUL
    #else
        #if defined USE_HSI_CLOCK
            #define PLL_OUTPUT_FREQ  HSI_FREQUENCY
        #else
            #define PLL_OUTPUT_FREQ  CRYSTAL_FREQ
        #endif
    #endif
#endif

#define PCLK2       (SYSCLK/PCLK2_DIVIDE)
#define PCLK1       (SYSCLK/PCLK1_DIVIDE)
#if defined HCLK_DIVIDE
    #define HCLK    (SYSCLK/HCLK_DIVIDE)
#else
    #define HCLK    (SYSCLK)
#endif

#if defined _STM32F7XX
    #if defined OVERDRIVE_ON
        #if PCLK1 > 54000000
            #error "PCLK1 speed is too high (max. 54MHz)!!"
        #endif
        #if PCLK2 > 108000000
            #error "PCLK1 speed is too high (max. 108MHz)!!"
        #endif
    #else
        #if PCLK1 > 45000000
            #error "PCLK1 speed is too high (max. 45MHz)!!"
        #endif
        #if PCLK2 > 90000000
            #error "PCLK1 speed is too high (max. 90MHz)!!"
        #endif
    #endif
#elif defined _STM32F4XX
    #if PCLK1 > 42000000
        #error "PCLK1 speed is too high (max. 42MHz)!!"
    #endif
    #if PCLK2 > 84000000
        #error "PCLK2 speed is too high (max. 84MHz)!!"
    #endif
#elif defined _STM32F2XX
    #if PCLK1 > 30000000
        #error "PCLK1 speed is too high (max. 30MHz)!!"
    #endif
    #if PCLK2 > 60000000
        #error "PCLK1 speed is too high (max. 60MHz)!!"
    #endif
#endif

#if defined USB_INTERFACE && HCLK < 14200000
    #error "HCLK (AHB) speed is not high enough for USB device operation!!"
#endif


#if defined _STM32F2XX || defined _STM32F4XX || defined _STM32F7XX       // byte writes are possible with these devices
    #define MAX_SECTOR_PARS                 (FLASH_GRANULARITY_BOOT - 2) // the number of user bytes fitting into first parameter block in internal boot sector
#else
    #define NO_ACCUMULATIVE_WORDS                                        // all FLASH writes must be 16 bits wide and 16 bit aligned
    #define MAX_SECTOR_PARS                 ((FLASH_GRANULARITY - 4)/2)  // the number of user bytes fitting into first parameter block in internal flash
#endif

#define SUPPLY_1_8__2_1   0                                              // supplying F4 with reduced voltage reduces the fastest speed and increases required wait states
#define SUPPLY_2_1__2_4   1
#define SUPPLY_2_4__2_7   2
#define SUPPLY_2_7__3_6   3

// STM32L432 can use 1.2V code voltage for maximum speed and minimum wait state (VCORE_RANGE_1)
// 1.0V core reduces maximum operating speed and increased the required wait states (VCORE_RANGE_2)
//
#define VCORE_RANGE_1     0
#define VCORE_RANGE_2     1
#define VCORE_RANGE_3     2

#if defined _STM32L432 || defined _STM32L4X5 || defined _STM32L4X6
    // Determine highest operating frequency and optimal wait states
    //
    #if CORE_VOLTAGE == VCORE_RANGE_2                                    // reduced core voltage operation
        #define PLL_MAX_FREQ         26000000                            // highest speed possible
        #define HSE_MAX_FREQ         26000000                            // highest external clock speed
        #define MSI_MAX_FREQ         24000000                            // highest programmable RC clock speed

        #if HCLK <= 6000000
            #define FLASH_WAIT_STATES   FLASH_ACR_LATENCY_ZERO_WAIT
        #elif SYSCLK <= 12000000
            #define FLASH_WAIT_STATES   FLASH_ACR_LATENCY_ONE_WAIT
        #elif SYSCLK <= 18000000
            #define FLASH_WAIT_STATES   FLASH_ACR_LATENCY_TWO_WAITS
        #elif SYSCLK <= 26000000
            #define FLASH_WAIT_STATES   FLASH_ACR_LATENCY_THREE_WAITS
        #endif
    #else
        #define PLL_MAX_FREQ         80000000                            // highest speed possible
        #define HSE_MAX_FREQ         48000000                            // highest external clock speed
        #define MSI_MAX_FREQ         48000000                            // highest programmable RC clock speed

        #if HCLK <= 16000000
            #define FLASH_WAIT_STATES   FLASH_ACR_LATENCY_ZERO_WAIT
        #elif SYSCLK <= 32000000
            #define FLASH_WAIT_STATES   FLASH_ACR_LATENCY_ONE_WAIT
        #elif SYSCLK <= 48000000
            #define FLASH_WAIT_STATES   FLASH_ACR_LATENCY_TWO_WAITS
        #elif SYSCLK <= 64000000
            #define FLASH_WAIT_STATES   FLASH_ACR_LATENCY_THREE_WAITS
        #else
            #define FLASH_WAIT_STATES   FLASH_ACR_LATENCY_TFOUR_WAITS
        #endif
    #endif
#elif defined _STM32L0x1
    #if CORE_VOLTAGE == VCORE_RANGE_3                                    // reduced core voltage operation (1.05V - 1.35V)
        #define PLL_MAX_FREQ         4000000                             // highest speed possible
        #define FLASH_WAIT_STATES   FLASH_ACR_LATENCY_ZERO_WAIT
    #elif CORE_VOLTAGE == VCORE_RANGE_2                                  // reduced core voltage operation (1.35V - 1.65V)
        #define PLL_MAX_FREQ         16000000                            // highest speed possible
        #if HCLK <= 8000000
            #define FLASH_WAIT_STATES   FLASH_ACR_LATENCY_ZERO_WAIT
        #else
            #define FLASH_WAIT_STATES   FLASH_ACR_LATENCY_ONE_WAIT
        #endif
    #else                                                                // normal - range 1 (1.65V - 1.95V)
        #define PLL_MAX_FREQ         32000000                            // highest speed possible
        #if HCLK <= 16000000
            #define FLASH_WAIT_STATES   FLASH_ACR_LATENCY_ZERO_WAIT
        #else
            #define FLASH_WAIT_STATES   FLASH_ACR_LATENCY_ONE_WAIT
        #endif
    #endif
#elif defined _STM32F4XX || defined _STM32F7XX
    // Determine highest operating frequency and optimal wait states
    //
    #if SUPPLY_VOLTAGE == SUPPLY_2_7__3_6
        #if defined _STM32F401
            #define PLL_MAX_FREQ         84000000                        // highest speed possible
            #if SYSCLK > 60000000
                #define FLASH_WAIT_STATES   FLASH_ACR_LATENCY_TWO_WAITS
            #elif SYSCLK > 30000000
                #define FLASH_WAIT_STATES   FLASH_ACR_LATENCY_TWO_WAITS
            #else
                #define FLASH_WAIT_STATES   FLASH_ACR_LATENCY_ZERO_WAIT
            #endif
        #else
            #define PLL_MAX_FREQ         168000000                       // highest speed possible
            #if SYSCLK > 150000000
                #define FLASH_WAIT_STATES   FLASH_ACR_LATENCY_FIVE_WAITS
            #elif SYSCLK > 120000000
                #define FLASH_WAIT_STATES   FLASH_ACR_LATENCY_FOUR_WAITS
            #elif SYSCLK > 90000000
                #define FLASH_WAIT_STATES   FLASH_ACR_LATENCY_THREE_WAITS
            #elif SYSCLK > 60000000
                #define FLASH_WAIT_STATES   FLASH_ACR_LATENCY_TWO_WAITS
            #elif SYSCLK > 30000000
                #define FLASH_WAIT_STATES   FLASH_ACR_LATENCY_ONE_WAIT
            #else
                #define FLASH_WAIT_STATES   FLASH_ACR_LATENCY_ZERO_WAIT
            #endif
        #endif
    #elif SUPPLY_VOLTAGE == SUPPLY_2_4__2_7
        #define PLL_MAX_FREQ         168000000                           // highest speed possible
        #if SYSCLK > 144000000
            #define FLASH_WAIT_STATES   FLASH_ACR_LATENCY_SIX_WAITS
        #elif SYSCLK > 120000000
            #define FLASH_WAIT_STATES   FLASH_ACR_LATENCY_FIVE_WAITS
        #elif SYSCLK > 96000000
            #define FLASH_WAIT_STATES   FLASH_ACR_LATENCY_FOUR_WAITS
        #elif SYSCLK > 72000000
            #define FLASH_WAIT_STATES   FLASH_ACR_LATENCY_THREE_WAITS
        #elif SYSCLK > 48000000
            #define FLASH_WAIT_STATES   FLASH_ACR_LATENCY_TWO_WAITS
        #elif SYSCLK > 24000000
            #define FLASH_WAIT_STATES   FLASH_ACR_LATENCY_ONE_WAIT
        #else
            #define FLASH_WAIT_STATES   FLASH_ACR_LATENCY_ZERO_WAIT
        #endif
    #elif SUPPLY_VOLTAGE == SUPPLY_2_1__2_4
        #define PLL_MAX_FREQ         138000000                           // highest speed possible
        #if SYSCLK > 120000000
            #define FLASH_WAIT_STATES   FLASH_ACR_LATENCY_SEVEN_WAITS
        #elif SYSCLK > 108000000
            #define FLASH_WAIT_STATES   FLASH_ACR_LATENCY_SIX_WAITS
        #elif SYSCLK > 90000000
            #define FLASH_WAIT_STATES   FLASH_ACR_LATENCY_FIVE_WAITS
        #elif SYSCLK > 72000000
            #define FLASH_WAIT_STATES   FLASH_ACR_LATENCY_FOUR_WAITS
        #elif SYSCLK > 54000000
            #define FLASH_WAIT_STATES   FLASH_ACR_LATENCY_THREE_WAITS
        #elif SYSCLK > 36000000
            #define FLASH_WAIT_STATES   FLASH_ACR_LATENCY_TWO_WAITS
        #elif SYSCLK > 18000000
            #define FLASH_WAIT_STATES   FLASH_ACR_LATENCY_ONE_WAIT
        #else
            #define FLASH_WAIT_STATES   FLASH_ACR_LATENCY_ZERO_WAIT
        #endif
    #elif SUPPLY_VOLTAGE == SUPPLY_1_8__2_1
        #define PLL_MAX_FREQ         128000000                           // highest speed possible
        #if SYSCLK > 112000000
            #define FLASH_WAIT_STATES   FLASH_ACR_LATENCY_SEVEN_WAITS
        #elif SYSCLK > 96000000
            #define FLASH_WAIT_STATES   FLASH_ACR_LATENCY_SIX_WAITS
        #elif SYSCLK > 80000000
            #define FLASH_WAIT_STATES   FLASH_ACR_LATENCY_FIVE_WAITS
        #elif SYSCLK > 64000000
            #define FLASH_WAIT_STATES   FLASH_ACR_LATENCY_FOUR_WAITS
        #elif SYSCLK > 48000000
            #define FLASH_WAIT_STATES   FLASH_ACR_LATENCY_THREE_WAITS
        #elif SYSCLK > 32000000
            #define FLASH_WAIT_STATES   FLASH_ACR_LATENCY_TWO_WAITS
        #elif SYSCLK > 16000000
            #define FLASH_WAIT_STATES   FLASH_ACR_LATENCY_ONE_WAIT
        #else
            #define FLASH_WAIT_STATES   FLASH_ACR_LATENCY_ZERO_WAIT
        #endif
    #endif
#elif defined _STM32F2XX
    #define PLL_MAX_FREQ         120000000                               // highest speed possible
    // Determine highest operating frequency and optimal wait states
    //
    #if SUPPLY_VOLTAGE == SUPPLY_2_7__3_6
        #if SYSCLK > 90000000
            #define FLASH_WAIT_STATES   FLASH_ACR_LATENCY_THREE_WAITS
        #elif SYSCLK > 60000000
            #define FLASH_WAIT_STATES   FLASH_ACR_LATENCY_TWO_WAITS
        #elif SYSCLK > 30000000
            #define FLASH_WAIT_STATES   FLASH_ACR_LATENCY_ONE_WAIT
        #else
            #define FLASH_WAIT_STATES   FLASH_ACR_LATENCY_ZERO_WAIT
        #endif
    #elif SUPPLY_VOLTAGE == SUPPLY_2_4__2_7
        #if SYSCLK > 96000000
            #define FLASH_WAIT_STATES   FLASH_ACR_LATENCY_FOUR_WAITS
        #elif SYSCLK > 72000000
            #define FLASH_WAIT_STATES   FLASH_ACR_LATENCY_THREE_WAITS
        #elif SYSCLK > 48000000
            #define FLASH_WAIT_STATES   FLASH_ACR_LATENCY_TWO_WAITS
        #elif SYSCLK > 24000000
            #define FLASH_WAIT_STATES   FLASH_ACR_LATENCY_ONE_WAIT
        #else
            #define FLASH_WAIT_STATES   FLASH_ACR_LATENCY_ZERO_WAIT
        #endif
    #elif SUPPLY_VOLTAGE == SUPPLY_2_1__2_4
        #if SYSCLK > 108000000
            #define FLASH_WAIT_STATES   FLASH_ACR_LATENCY_SIX_WAITS
        #elif SYSCLK > 90000000
            #define FLASH_WAIT_STATES   FLASH_ACR_LATENCY_FIVE_WAITS
        #elif SYSCLK > 72000000
            #define FLASH_WAIT_STATES   FLASH_ACR_LATENCY_FOUR_WAITS
        #elif SYSCLK > 54000000
            #define FLASH_WAIT_STATES   FLASH_ACR_LATENCY_THREE_WAITS
        #elif SYSCLK > 36000000
            #define FLASH_WAIT_STATES   FLASH_ACR_LATENCY_TWO_WAITS
        #elif SYSCLK > 18000000
            #define FLASH_WAIT_STATES   FLASH_ACR_LATENCY_ONE_WAIT
        #else
            #define FLASH_WAIT_STATES   FLASH_ACR_LATENCY_ZERO_WAIT
        #endif
    #elif SUPPLY_VOLTAGE == SUPPLY_1_8__2_1
        #if SYSCLK > 112000000
            #define FLASH_WAIT_STATES   FLASH_ACR_LATENCY_SEVEN_WAITS
        #elif SYSCLK > 96000000
            #define FLASH_WAIT_STATES   FLASH_ACR_LATENCY_SIX_WAITS
        #elif SYSCLK > 80000000
            #define FLASH_WAIT_STATES   FLASH_ACR_LATENCY_FIVE_WAITS
        #elif SYSCLK > 64000000
            #define FLASH_WAIT_STATES   FLASH_ACR_LATENCY_FOUR_WAITS
        #elif SYSCLK > 48000000
            #define FLASH_WAIT_STATES   FLASH_ACR_LATENCY_THREE_WAITS
        #elif SYSCLK > 32000000
            #define FLASH_WAIT_STATES   FLASH_ACR_LATENCY_TWO_WAITS
        #elif SYSCLK > 16000000
            #define FLASH_WAIT_STATES   FLASH_ACR_LATENCY_ONE_WAIT
        #else
            #define FLASH_WAIT_STATES   FLASH_ACR_LATENCY_ZERO_WAIT
        #endif
    #endif
#elif defined _CONNECTIVITY_LINE || defined _PERFORMANCE_LINE || defined _STM32F031
    #if defined _STM32F031
        #define PLL_MAX_FREQ        48000000                             // highest speed possible
    #else
        #define PLL_MAX_FREQ        72000000                             // highest speed possible
    #endif
    #if SYSCLK > 48000000
        #define FLASH_WAIT_STATES   FLASH_ACR_LATENCY_TWO_WAITS
    #elif SYSCLK > 24000000
        #define FLASH_WAIT_STATES   FLASH_ACR_LATENCY_ONE_WAIT
    #else
        #define FLASH_WAIT_STATES   FLASH_ACR_LATENCY_ZERO_WAIT
    #endif
#else
    #if defined _ACCESS_LINE_WITH_USB
        #define PLL_MAX_FREQ  48000000
    #elif defined _ACCESS_LINE
        #define PLL_MAX_FREQ  36000000
    #else
        #define PLL_MAX_FREQ  24000000                                   // value line
    #endif
#endif

#if PLL_OUTPUT_FREQ > PLL_MAX_FREQ
    #error "System frequency too high!!!!"
#endif

#define RANDOM_SEED_LOCATION (unsigned short *)(RAM_START_ADDRESS + (SIZE_OF_RAM - 4)) // {13} location of a long word which is never initialised and so has a random power on value
#define BOOT_MAIL_BOX        (unsigned short *)(RAM_START_ADDRESS + (SIZE_OF_RAM - 2)) // {13}

/*****************************************************************************************************/


#if defined _WINDOWS
    #include "simSTM32.h"
    #define ENET_BLOCK                  ((unsigned char *)(&STM32.ETH))        // Ethernet Controller
    #define RCC_BLOCK                   ((unsigned char *)(&STM32.RCC))        // Reset and Clock Control
    #define DMA1_BLOCK                  ((unsigned char *)(&STM32.DMAC[0]))    // DMA Controller 1
    #define DMA2_BLOCK                  ((unsigned char *)(&STM32.DMAC[1]))    // DMA Controller 2
    #define ADC_BLOCK                   ((unsigned char *)(&STM32.ADC))        // ADC
    #define SDIO_BLOCK                  ((unsigned char *)(&STM32.SDIO))       // SDIO Controller
    #define FMI_BLOCK                   ((unsigned char *)(&STM32.FMI))        // Flash Memory Interface
    #define EXTI_BLOCK                  ((unsigned char *)(&STM32.EXTI))       // External Interrupt Controller
    #define GPIO_PORTA_BLOCK            ((unsigned char *)(&STM32.Ports[0]))   // GPIO A PORT
    #define GPIO_PORTB_BLOCK            ((unsigned char *)(&STM32.Ports[1]))   // GPIO B PORT
    #define GPIO_PORTC_BLOCK            ((unsigned char *)(&STM32.Ports[2]))   // GPIO C PORT
    #define GPIO_PORTD_BLOCK            ((unsigned char *)(&STM32.Ports[3]))   // GPIO D PORT
    #define GPIO_PORTE_BLOCK            ((unsigned char *)(&STM32.Ports[4]))   // GPIO E PORT
    #define GPIO_PORTF_BLOCK            ((unsigned char *)(&STM32.Ports[5]))   // GPIO F PORT
    #define GPIO_PORTG_BLOCK            ((unsigned char *)(&STM32.Ports[6]))   // GPIO G PORT
    #define GPIO_PORTH_BLOCK            ((unsigned char *)(&STM32.Ports[7]))   // GPIO H PORT
    #define GPIO_PORTI_BLOCK            ((unsigned char *)(&STM32.Ports[8]))   // GPIO I PORT
    #if defined _STM32F7XX
        #define GPIO_PORTJ_BLOCK        ((unsigned char *)(&STM32.Ports[9]))   // GPIO J PORT
        #define GPIO_PORTK_BLOCK        ((unsigned char *)(&STM32.Ports[10]))  // GPIO K PORT
    #endif
    #if !defined _STM32F2XX && !defined _STM32F4XX && !defined _STM32F7XX
        #define AFIO_BLOCK              ((unsigned char *)(&STM32.AFIO))       // Alternative I/O function
    #endif
    #define USART1_BLOCK                ((unsigned char *)(&STM32.USART[0]))   // USART 1
    #define USART2_BLOCK                ((unsigned char *)(&STM32.USART[1]))   // USART 2
    #if USARTS_AVAILABLE > 2
        #define USART3_BLOCK            ((unsigned char *)(&STM32.USART[2]))   // USART 3
    #endif
    #if defined _STM32F2XX || defined _STM32F4XX || defined _STM32F7XX
        #define USART6_BLOCK            ((unsigned char *)(&STM32.USART[3]))   // USART 6
    #endif
    #if UARTS_AVAILABLE > 0
        #define UART4_BLOCK             ((unsigned char *)(&STM32.UART[0]))    // UART 4
    #endif
    #if UARTS_AVAILABLE > 1
        #define UART5_BLOCK             ((unsigned char *)(&STM32.UART[1]))    // UART 5
    #endif
    #if LPUARTS_AVAILABLE > 0
        #define LPUART1_BLOCK           ((unsigned char *)(&STM32.LPUART[0]))  // LPUART 1
    #endif
    #if defined _STM32F7XX || defined _STM32F429 || defined _STM32F427
        #define UART7_BLOCK             ((unsigned char *)(&STM32.UART[2]))    // UART 7
        #define UART8_BLOCK             ((unsigned char *)(&STM32.UART[3]))    // UART 8
    #endif
    #define RTC_BLOCK                   ((unsigned char *)(&STM32.RTC))        // RTC
    #define IWDG_BLOCK                  ((unsigned char *)(&STM32.IWDG))       // Independent Watchdog
    #define I2C1_BLOCK                  ((unsigned char *)(&STM32.I2C[0]))     // I2C1
    #define I2C2_BLOCK                  ((unsigned char *)(&STM32.I2C[1]))     // I2C2
    #if defined _STM32F2XX || defined _STM32F4XX || defined _STM32F7XX         // {6}
        #define I2C3_BLOCK              ((unsigned char *)(&STM32.I2C[2]))     // I2C3
    #endif
    #define TIM2_BLOCK                  ((unsigned char *)(&STM32.TIM2_3_4_5[0])) // TIM2
    #define TIM3_BLOCK                  ((unsigned char *)(&STM32.TIM2_3_4_5[1])) // TIM3
    #define TIM4_BLOCK                  ((unsigned char *)(&STM32.TIM2_3_4_5[2])) // TIM4
    #define TIM5_BLOCK                  ((unsigned char *)(&STM32.TIM2_3_4_5[3])) // TIM5
    #if defined TIMER_9_AVAILABLE
        #define TIM9_BLOCK              ((unsigned char *)(&STM32.TIM9_10_11_12_13_14[0])) // TIM9
    #endif
    #if defined TIMER_10_AVAILABLE
        #define TIM10_BLOCK             ((unsigned char *)(&STM32.TIM9_10_11_12_13_14[1])) // TIM10
    #endif
    #if defined TIMER_11_AVAILABLE
        #define TIM11_BLOCK             ((unsigned char *)(&STM32.TIM9_10_11_12_13_14[2])) // TIM11
    #endif
    #if defined TIMER_12_AVAILABLE
        #define TIM12_BLOCK             ((unsigned char *)(&STM32.TIM9_10_11_12_13_14[3])) // TIM12
    #endif
    #if defined TIMER_13_AVAILABLE
        #define TIM13_BLOCK             ((unsigned char *)(&STM32.TIM9_10_11_12_13_14[4])) // TIM13
    #endif
    #if defined TIMER_14_AVAILABLE
        #define TIM14_BLOCK             ((unsigned char *)(&STM32.TIM9_10_11_12_13_14[5])) // TIM14
    #endif
    #if defined USB_DEVICE_AVAILABLE
        #define USB_CAN_MEM_BLOCK       ((unsigned short *)(STM32.USB_CAN_SRAM)) // shared USB/CAN sSRAM
        #define USB_DEV_FS_BLOCK        ((unsigned char *)(&STM32.USBD))       // USB device
    #endif
    #define PWR_BLOCK                   ((unsigned char *)(&STM32.PWR))        // power control
    #define CAN1_BLOCK                  ((unsigned char *)(&STM32.CAN))        // bxCAN1
    #define CAN2_BLOCK                  ((unsigned char *)(&STM32.CAN_SLAVE))  // bxCAN2
    #if defined _STM32F2XX || defined _STM32F4XX || defined _STM32F7XX || defined _STM32L432 || defined _STM32L0x1 || defined _STM32F031 || defined _STM32L4X5 || defined _STM32L4X6
        #define SPI1_I2S_BLOCK          ((unsigned char *)(&STM32.SPI_I2S[0])) // SPI-I2S
        #define SPI2_I2S_BLOCK          ((unsigned char *)(&STM32.SPI_I2S[1])) // SPI-I2S
        #define SPI3_I2S_BLOCK          ((unsigned char *)(&STM32.SPI_I2S[2])) // SPI-I2S
        #define SYSCFG_BLOCK            ((unsigned char *)(&STM32.SYSCFG))     // System Configuration
        #define SPI1_BLOCK              SPI1_I2S_BLOCK
        #define FSMC_BLOCK              ((unsigned char *)(&STM32.FSMC))       // Flexible Static Memory Controller
        #define QUADSPI_BLOCK           ((unsigned char *)(&STM32.QUADSPI))    // Quad-SPI
    #else
        #define SPI1_BLOCK              ((unsigned char *)(&STM32.SPI))        // SPI
        #define SPI2_I2S_BLOCK          ((unsigned char *)(&STM32.SPI_I2S[0])) // SPI-I2S
        #define SPI3_I2S_BLOCK          ((unsigned char *)(&STM32.SPI_I2S[1])) // SPI-I2S
    #endif
    #if defined USB_OTG_AVAILABLE
        #define USB_OTG_FS_BLOCK        ((unsigned char *)(&STM32.USB_OTG_FS)) // {10} USB OTG FS
    #endif
    #define CORTEX_M3_BLOCK             ((unsigned char *)(&STM32.CORTEX_M3))
    #if defined ARM_MATH_CM4 || defined ARM_MATH_CM7
        #define CORTEX_M4_DEBUG         ((unsigned char *)(&STM32.CORTEX_M4_DEBUG))
        #define CORTEX_M4_DWT           ((unsigned char *)(&STM32.CORTEX_M4_TRACE)) // data watch and trace unit
    #endif
    #define DBG_BLOCK                   ((unsigned char *)(&STM32.DBG))
#else
    #if defined _STM32F2XX || defined _STM32F4XX || defined _STM32F7XX
        // APB1 peripherals
        //
        #define TIM2_BLOCK                  0x40000000
        #define TIM3_BLOCK                  0x40000400
        #define TIM4_BLOCK                  0x40000800
        #define TIM5_BLOCK                  0x40000c00
        #define TIM6_BLOCK                  0x40001000
        #define TIM7_BLOCK                  0x40001400
        #define TIM12_BLOCK                 0x40001800
        #define TIM13_BLOCK                 0x40001c00
        #define TIM14_BLOCK                 0x40002000
        #define RTC_BLOCK                   0x40002800
        #define WWDG_BLOCK                  0x40002c00
        #define IWDG_BLOCK                  0x40003000
        #define I2S2ext_BLOCK               0x40003400
        #define SPI2_I2S_BLOCK              0x40003800
        #define SPI3_I2S_BLOCK              0x40003c00
        #define I2S3ext_BLOCK               0x40004000
        #define USART2_BLOCK                0x40004400
        #define USART3_BLOCK                0x40004800
        #define UART4_BLOCK                 0x40004c00
        #define UART5_BLOCK                 0x40005000
        #if defined _STM32F7XX || defined _STM32F429 || defined _STM32F427
            #define UART7_BLOCK             0x40007800
            #define UART8_BLOCK             0x40007c00
        #endif
        #define I2C1_BLOCK                  0x40005400
        #define I2C2_BLOCK                  0x40005800
        #define I2C3_BLOCK                  0x40005c00
        #define CAN1_BLOCK                  0x40006400
        #define CAN2_BLOCK                  0x40006800
        #define PWR_BLOCK                   0x40007000
        #define DAC_BLOCK                   0x40007400

        // APB2 peripherals
        //
        #define TIM1_BLOCK                  0x40010000
        #define TIM8_BLOCK                  0x40010400
        #define USART1_BLOCK                0x40011000
        #define USART6_BLOCK                0x40011400
        #define ADC_BLOCK                   0x40012000
        #define SDIO_BLOCK                  0x40012c00
        #define SPI1_BLOCK                  0x40013000
        #define SYSCFG_BLOCK                0x40013800
        #define EXTI_BLOCK                  0x40013c00
        #define TIM9_BLOCK                  0x40014000
        #define TIM10_BLOCK                 0x40014400
        #define TIM11_BLOCK                 0x40014800

        // AHB1 peripherals
        //
        #define GPIO_PORTA_BLOCK            0x40020000
        #define GPIO_PORTB_BLOCK            0x40020400
        #define GPIO_PORTC_BLOCK            0x40020800
        #define GPIO_PORTD_BLOCK            0x40020c00
        #define GPIO_PORTE_BLOCK            0x40021000
        #define GPIO_PORTF_BLOCK            0x40021400
        #define GPIO_PORTG_BLOCK            0x40021800
        #define GPIO_PORTH_BLOCK            0x40021c00
        #define GPIO_PORTI_BLOCK            0x40022000
      #if defined _STM32F7XX
        #define GPIO_PORTJ_BLOCK            0x40024000
        #define GPIO_PORTK_BLOCK            0x40028000
      #endif
        #define CRC_BLOCK                   0x40023000
        #define RCC_BLOCK                   0x40023800
        #define FMI_BLOCK                   0x40023c00
        #define BKPSRAM_BLOCK               0x40024000
        #define DMA1_BLOCK                  0x40026000
        #define DMA2_BLOCK                  0x40026400
        #define ENET_BLOCK                  0x40028000
        #define USB_OTG_HS_BLOCK            0x40040000

        // AHB2 peripherals
        //
        #define USB_OTG_FS_BLOCK            0x50000000
        #define DCMI_BLOCK                  0x50050000
        #define CRYP_BLOCK                  0x50060000
        #define HASH_BLOCK                  0x50060400
        #define RNG_BLOCK                   0x50060800

        // AHB3 peripherals
        //
        #define FSMC_BLOCK                  0xa0000000
        #define QUADSPI_BLOCK               0xa0001000

        #define DBG_BLOCK                   0xe0042000
    #elif defined _STM32L432 || defined _STM32F031 || defined _STM32L4X5 || defined _STM32L4X6
        // APB1 peripherals
        //
        #define TIM2_BLOCK                  0x40000000
        #define TIM3_BLOCK                  0x40000400
      //#define TIM4_BLOCK                  0x40000800
      //#define TIM5_BLOCK                  0x40000c00
        #define TIM6_BLOCK                  0x40001000
        #define TIM7_BLOCK                  0x40001400
      //#define TIM12_BLOCK                 0x40001800
      //#define TIM13_BLOCK                 0x40001c00
      //#define TIM14_BLOCK                 0x40002000
        #define LCD_BLOCK                   0x40002400                   // only STM32L4x3xx
        #define RTC_BLOCK                   0x40002800
        #define WWDG_BLOCK                  0x40002c00
        #define IWDG_BLOCK                  0x40003000
        #define SPI2_I2S_BLOCK              0x40003800
        #define SPI3_I2S_BLOCK              0x40003c00
        #define USART2_BLOCK                0x40004400
        #define USART3_BLOCK                0x40004800
        #define UART4_BLOCK                 0x40004c00
        #if defined _STM32L4X5 || defined _STM32L4X6
            #define UART5_BLOCK             0x40005000
        #endif
        #define I2C1_BLOCK                  0x40005400
        #define I2C2_BLOCK                  0x40005800
        #define I2C3_BLOCK                  0x40005c00
        #if !defined _STM32L4X5 && !defined _STM32L4X6
            #define CRS_BLOCK               0x40006000
        #endif
        #define CAN1_BLOCK                  0x40006400
        #if !defined _STM32L4X5 && !defined _STM32L4X6
            #define USB_FS_BLOCK            0x40006800
            #define USB_SRAM_BLOCK          0x40006c00
        #endif
        #define PWR_BLOCK                   0x40007000
        #define DAC1_BLOCK                  0x40007400
        #define OPAMP_BLOCK                 0x40007800
        #define LPTIM1_BLOCK                0x40007c00
        #define LPUART1_BLOCK               0x40008000
        #if !defined _STM32L4X5 && !defined _STM32L4X6
            #define I2C4_BLOCK              0x40008400
        #endif
        #define SWPMI1_BLOCK                0x40008800
        #define LPTIM2_BLOCK                0x40009400

        // APB2 peripherals
        //
        #define SYSCFG_BLOCK                0x40010000
        #define VREFBUF_BLOCK               0x40010030
        #define COMP_BLOCK                  0x40010200
        #define EXTI_BLOCK                  0x40010400
        #define FIREWALL_BLOCK              0x4001c000
        #if defined _STM32F031
            #define ADC_BLOCK               0x40012400
        #endif
        #define SDMMC_BLOCK                 0x40012800
        #define TIM1_BLOCK                  0x40012c00
        #define SPI1_BLOCK                  0x40013000
        #if defined _STM32F031
            #define SPI1_I2S1_BLOCK         0x40013000
        #endif
        #if defined _STM32L4X5 || defined _STM32L4X6
            #define TIM8_BLOCK              0x40013400
        #endif
        #define USART1_BLOCK                0x40013800
        #define TIM15_BLOCK                 0x40014000
        #define TIM16_BLOCK                 0x40014400
        #if defined _STM32F031 || defined _STM32L4X5 || defined _STM32L4X6
            #define TIM17_BLOCK             0x40014800
        #endif
        #define SAI1_BLOCK                  0x40015400
        #if defined _STM32F031 || defined _STM32L4X5 || defined _STM32L4X6
            #define SAI2_BLOCK              0x40015800
        #endif
        #if defined _STM32F031
            #define DBG_BLOCK               0x40015800
        #endif
        #define DFSDM1_BLOCK                0x40016000

        // AHB1 peripherals
        //
        #define DMA1_BLOCK                  0x40020000
        #define DMA2_BLOCK                  0x40020400
        #define RCC_BLOCK                   0x40021000
        #define FMI_BLOCK                   0x40022000
        #define CRC_BLOCK                   0x40023000
        #define TSC_BLOCK                   0x40024000
        #if defined _STM32L4X5 || defined _STM32L4X6
            #define DMA2D_BLOCK             0x4002B000
        #endif

        // AHB2 peripherals
        //
        #define GPIO_PORTA_BLOCK            0x48000000
        #define GPIO_PORTB_BLOCK            0x48000400
        #define GPIO_PORTC_BLOCK            0x48000800
        #define GPIO_PORTD_BLOCK            0x48000c00
        #define GPIO_PORTE_BLOCK            0x48001000
        #if defined _STM32F031 || defined _STM32L4X5 || defined _STM32L4X6
            #define GPIO_PORTF_BLOCK        0x48001400
        #endif
        #if defined _STM32L4X5 || defined _STM32L4X6
            #define GPIO_PORTG_BLOCK        0x48001800
        #endif
        #define GPIO_PORTH_BLOCK            0x48001c00
        #if defined _STM32L4X5 || defined _STM32L4X6
            #define GPIO_PORTI_BLOCK        0x48002000
            #define OTG_FS__BLOCK           0x50000000
        #endif
        #define ADC_BLOCK                   0x50040000
        #if defined _STM32L4X5 || defined _STM32L4X6
            #define DCMI_BLOCK              0x50050000
        #endif
        #define AES_BLOCK                   0x50060000
        #if defined _STM32L4X5 || defined _STM32L4X6
            #define HASH_BLOCK              0x50060400
        #endif
        #define RNG_BLOCK                   0x50060800

        // AHB3 peripherals
        //
        #if defined _STM32L4X5 || defined _STM32L4X6
            #define FMC_BLOCK               0xa0000000
        #endif

        // AHB4 peripherals
        //
        #if defined _STM32L4X5 || defined _STM32L4X6
            #define QUADSPI_BLOCK           0xa0001000
        #endif

        #define DBG_BLOCK                   0xe0042000
    #elif defined _STM32L0x1
        // APB1 peripherals
        //
        #define TIM2_BLOCK                  0x40000000
        #define TIM3_BLOCK                  0x40000400
        #define TIM6_BLOCK                  0x40001000
        #define TIM7_BLOCK                  0x40001400
        #define RTC_BLOCK                   0x40002800
        #define WWDG_BLOCK                  0x40002c00
        #define IWDG_BLOCK                  0x40003000
        #define SPI2_I2S_BLOCK              0x40003800
        #define USART2_BLOCK                0x40004400
        #define LPUART1_BLOCK               0x40004800
        #define USART4_BLOCK                0x40004c00
        #define USART5_BLOCK                0x40005000
        #define I2C1_BLOCK                  0x40005400
        #define I2C2_BLOCK                  0x40005800
        #define PWR_BLOCK                   0x40007000
        #define I2C3_BLOCK                  0x40007800
        #define LPTIM2_BLOCK                0x40007c00

        // APB2 peripherals
        //
        #define SYSCFG_BLOCK                0x40010000
        #define EXTI_BLOCK                  0x40010400
        #define TIM21_BLOCK                 0x40010800
        #define TIM22_BLOCK                 0x40011400
        #define FIREWALL_BLOCK              0x40011c00
        #define ADC1_BLOCK                  0x40012400
        #define SPI1_BLOCK                  0x40013000
        #define USART1_BLOCK                0x40013800
        #define DBG_BLOCK                   0x40015800

        // AHB peripherals
        //
        #define DMA1_BLOCK                  0x40020000
        #define RCC_BLOCK                   0x40021000
        #define FMI_BLOCK                   0x40022000
        #define CRC_BLOCK                   0x40023000
        #define AES_BLOCK                   0x40026000

        // IOPORT peripherals
        //
        #define GPIO_PORTA_BLOCK            0x50000000
        #define GPIO_PORTB_BLOCK            0x50000400
        #define GPIO_PORTC_BLOCK            0x50000800
        #define GPIO_PORTD_BLOCK            0x50000c00
        #define GPIO_PORTE_BLOCK            0x50001000
        #define GPIO_PORTH_BLOCK            0x50001c00
    #else                                                                // F1
        // APB1 peripherals
        //
        #define TIM2_BLOCK                  0x40000000
        #define TIM3_BLOCK                  0x40000400
        #define TIM4_BLOCK                  0x40000800
        #define TIM5_BLOCK                  0x40000c00
        #define TIM6_BLOCK                  0x40001000
        #define TIM7_BLOCK                  0x40001400
        #define TIM12_BLOCK                 0x40001800
        #define TIM13_BLOCK                 0x40001c00
        #define TIM14_BLOCK                 0x40002000
        #define RTC_BLOCK                   0x40002800
        #define WWDG_BLOCK                  0x40002c00
        #define IWDG_BLOCK                  0x40003000
        #define SPI2_I2S_BLOCK              0x40003800
        #define SPI3_I2S_BLOCK              0x40003c00
        #define USART2_BLOCK                0x40004400
        #define USART3_BLOCK                0x40004800
        #define UART4_BLOCK                 0x40004c00
        #define UART5_BLOCK                 0x40005000
        #define I2C1_BLOCK                  0x40005400
        #define I2C2_BLOCK                  0x40005800
        #define USB_DEV_FS_BLOCK            0x40005c00
        #define USB_CAN_MEM_BLOCK           0x40006000                   // not accessible in connectivity line devices and used by either CAN or USB device (but not both at the same time)
        #define CAN1_BLOCK                  0x40006400
        #define CAN2_BLOCK                  0x40006800
        #define BKP_BLOCK                   0x40006c00
        #define PWR_BLOCK                   0x40007000
        #define DAC_BLOCK                   0x40007400

        // APB2 peripherals
        //
        #define AFIO_BLOCK                  0x40010000
        #define EXTI_BLOCK                  0x40010400
        #define GPIO_PORTA_BLOCK            0x40010800
        #define GPIO_PORTB_BLOCK            0x40010c00
        #define GPIO_PORTC_BLOCK            0x40011000
        #define GPIO_PORTD_BLOCK            0x40011400
        #define GPIO_PORTE_BLOCK            0x40011800
        #define GPIO_PORTF_BLOCK            0x40011c00
        #define GPIO_PORTG_BLOCK            0x40012000
        #define ADC1_BLOCK                  0x40012400
        #define ADC2_BLOCK                  0x40012800
        #define TIM1_BLOCK                  0x40012C00
        #define SPI1_BLOCK                  0x40013C00
        #define TIM8_BLOCK                  0x40013400
        #define USART1_BLOCK                0x40013800
        #define ADC3_BLOCK                  0x40013c00
        #define TIM9_BLOCK                  0x40014C00
        #define TIM10_BLOCK                 0x40015000
        #define TIM11_BLOCK                 0x40015400

        // AHB peripherals
        //
        #define SDIO_BLOCK                  0x40018000
        #define DMA1_BLOCK                  0x40020000
        #define DMA2_BLOCK                  0x40020400
        #define RCC_BLOCK                   0x40021000
        #define FMI_BLOCK                   0x40022000
        #define CRC_BLOCK                   0x40023000
        #define ENET_BLOCK                  0x40028000

        #define USB_OTG_FS_BLOCK            0x50000000                   // {10}

        #define DBG_BLOCK                   0xe0042000
    #endif
    #define CORTEX_M3_BLOCK                 0xe000e000
    #if defined ARM_MATH_CM4 || defined ARM_MATH_CM7
        #define CORTEX_M4_DEBUG             0xe000edf0
        #define CORTEX_M4_DWT               0xe0001000                   // data watch and trace unit
    #endif
#endif


// Macro to clear flags by writing '1' to the bit
//
#if defined _WINDOWS                                                     // clear when simulating
    #define WRITE_ONE_TO_CLEAR(reg, flag)    reg &= ~(flag)
#else
    #define WRITE_ONE_TO_CLEAR(reg, flag)    reg = (flag)
#endif
#if defined _WINDOWS                                                     // clear when simulating
    #define OR_ONE_TO_CLEAR(reg, flag)       reg &= ~(flag)
#else
    #define OR_ONE_TO_CLEAR(reg, flag)       reg |= (flag)
#endif


// Flexible Static Memory Controller
//
#define FSMC_BCR1                        *(volatile unsigned long *)(FSMC_BLOCK + 0x0000) // SRAM/NOR-Flash chip-select control register 1
  #define FSMC_BCR_MBKEN                 0x00000001                      // memory bank enable
  #define FSMC_BCR_MUXEN                 0x00000002                      // address/data multiplexing
  #define FSMC_BCR_MTYP_SRAM_ROM         0x00000000                      // SRAM, ROM
  #define FSMC_BCR_MTYP_PSRAM            0x00000004                      // PSRAM (Cellular RAM: CRAM)
  #define FSMC_BCR_MTYP_NOR_FLASH        0x00000008                      // NOR Flash/OneNAND Flash
  #define FSMC_BCR_MWD_8                 0x00000000                      // data bus width - 8 bits
  #define FSMC_BCR_MWD_16                0x00000010                      // data bus width - 16 bits
  #define FSMC_BCR_FACCEN                0x00000040                      // NOR flash access enabled
  #define FSMC_BCR_BURSTEN               0x00000100                      // burst access mode enabled
  #define FSMC_BCR_WAITPOL_LOW           0x00000000                      // NWAIT active low
  #define FSMC_BCR_WAITPOL_HIGH          0x00000200                      // NWAIT active high
  #define FSMC_BCR_WRAPMOD               0x00000400                      // wrapped burst mode support
  #define FSMC_BCR_WAITCFG               0x00000800                      // NWAIT signal is active during wait state (not for Cellular RAM)
  #define FSMC_BCR_WREN                  0x00001000                      // write operations are enabled for the bank
  #define FSMC_BCR_WAITEN                0x00002000                      // NWAIT signal is enabled
  #define FSMC_BCR_EXTMOD                0x00004000                      // FSMC_BWTR register is taken into account
  #define FSMC_BCR_ASCYCWAIT             0x00008000                      // NWAIT signal is taken into account when running an asynchronous protocol
  #define FSMC_BCR_CBURSTRW              0x00080000                      // write burst enable
#define FSMC_BTR1                        *(unsigned long *)(FSMC_BLOCK + 0x0004) // SRAM/NOR-Flash chip-select timing register 1
  #define FSMC_BTR_ADDSET_MASK           0x0000000f                      // address setup phase duration (0..15 HCLK clock cycles)
  #define FSMC_BTR_ADDHLD_MASK           0x000000f0                      // address-hold phase duration (0..15 HCLK clock cycles)
  #define FSMC_BTR_DATAST_MASK           0x0000ff00                      // data phase duration (1..255 HCLK clock cycles)
  #define FSMC_BTR_BUSTURN_MASK          0x000f0000                      // bus turnaround phase duration (0..15 HCLK clock cycles)
  #define FSMC_BTR_CLKDIV_MASK           0x00f00000                      // clock divide ration (for CLK signal) - period of CLK in HCLK cycles (2..16 [+1]) - don't care for NOR Flash, SRAM and ROM
  #define FSMC_BTR_DATLAT_MASK           0x0f000000                      // data latency for synchronous burst NOR flash (2..17 CLK clock cycles [+2])
  #define FSMC_BTR_ACCMOD_A              0x00000000                      // access mode A (only valid in external mode)
  #define FSMC_BTR_ACCMOD_B              0x10000000                      // access mode B (only valid in external mode)
  #define FSMC_BTR_ACCMOD_C              0x20000000                      // access mode C (only valid in external mode)
  #define FSMC_BTR_ACCMOD_D              0x30000000                      // access mode D (only valid in external mode)
#define FSMC_BCR2                        *(volatile unsigned long *)(FSMC_BLOCK + 0x0008) // SRAM/NOR-Flash chip-select control register 2
#define FSMC_BTR2                        *(unsigned long *)(FSMC_BLOCK + 0x000c) // SRAM/NOR-Flash chip-select timing register 2
#define FSMC_BCR3                        *(volatile unsigned long *)(FSMC_BLOCK + 0x0010) // SRAM/NOR-Flash chip-select control register 3
#define FSMC_BTR3                        *(unsigned long *)(FSMC_BLOCK + 0x0014) // SRAM/NOR-Flash chip-select timing register 3
#define FSMC_BCR4                        *(volatile unsigned long *)(FSMC_BLOCK + 0x0018) // SRAM/NOR-Flash chip-select control register 4
#define FSMC_BTR4                        *(unsigned long *)(FSMC_BLOCK + 0x001c) // SRAM/NOR-Flash chip-select timing register 4

#define FSMC_PCR2                        *(unsigned long *)(FSMC_BLOCK + 0x0060)
#define FSMC_PSR2                        *(unsigned long *)(FSMC_BLOCK + 0x0064)
#define FSMC_PMEM2                       *(unsigned long *)(FSMC_BLOCK + 0x0068)
#define FSMC_PATT2                       *(unsigned long *)(FSMC_BLOCK + 0x006c)

#define FSMC_ECCR2                       *(volatile unsigned long *)(FSMC_BLOCK + 0x0074)

#define FSMC_PCR3                        *(unsigned long *)(FSMC_BLOCK + 0x0080)
#define FSMC_PSR3                        *(unsigned long *)(FSMC_BLOCK + 0x0084)
#define FSMC_PMEM3                       *(unsigned long *)(FSMC_BLOCK + 0x0088)
#define FSMC_PATT3                       *(unsigned long *)(FSMC_BLOCK + 0x008c)

#define FSMC_ECCR3                       *(volatile unsigned long *)(FSMC_BLOCK + 0x0094)

#define FSMC_PCR4                        *(unsigned long *)(FSMC_BLOCK + 0x00a0)
#define FSMC_PSR4                        *(unsigned long *)(FSMC_BLOCK + 0x00a4)
#define FSMC_PMEM4                       *(unsigned long *)(FSMC_BLOCK + 0x00a8)
#define FSMC_PATT4                       *(unsigned long *)(FSMC_BLOCK + 0x00ac)
#define FSMC_PIO4                        *(unsigned long *)(FSMC_BLOCK + 0x00b0)

#define FSMC_BWTR1                       *(unsigned long *)(FSMC_BLOCK + 0x0104)
 
#define FSMC_BWTR2                       *(unsigned long *)(FSMC_BLOCK + 0x010c)

#define FSMC_BWTR3                       *(unsigned long *)(FSMC_BLOCK + 0x0114)

#define FSMC_BWTR4                       *(unsigned long *)(FSMC_BLOCK + 0x011c) 

// FSMC macros
//
// Set access timing (values in HCLK cycles apart from d_lat which is in CLK cycles)
//
#define SET_MEMORY_TIMING(bank, a_setup, a_hold, d_setup, bus_t, clk_p, d_lat, mode) FSMC_BTR##bank = (a_setup | (a_setup << 4) | (d_setup << 8) | (bus_t << 16) | ((clk_p - 1) << 20) | ((d_lat - 2) << 24) | FSMC_BTR_ACCMOD_##mode);


// Quad-SPI
//
#define QUADSPI_CR                       *(unsigned long *)(QUADSPI_BLOCK + 0x0000) // Quad-SPI control register
#define QUADSPI_DCR                      *(unsigned long *)(QUADSPI_BLOCK + 0x0004) // Quad-SPI device configuration register
#define QUADSPI_SR                       *(volatile unsigned long *)(QUADSPI_BLOCK + 0x0008) // Quad-SPI sttus register (read-only)
#define QUADSPI_FCR                      *(volatile unsigned long *)(QUADSPI_BLOCK + 0x000c) // Quad-SPI flag clear register (write '1' to clear)
#define QUADSPI_DLR                      *(unsigned long *)(QUADSPI_BLOCK + 0x0010) // Quad-SPI data length register
#define QUADSPI_CCR                      *(unsigned long *)(QUADSPI_BLOCK + 0x0014) // Quad-SPI communication configuration register
#define QUADSPI_AR                       *(unsigned long *)(QUADSPI_BLOCK + 0x0018) // Quad-SPI address register
#define QUADSPI_ABR                      *(unsigned long *)(QUADSPI_BLOCK + 0x001c) // Quad-SPI alternate bytes register
#define QUADSPI_DR                       *(unsigned long *)(QUADSPI_BLOCK + 0x0020) // Quad-SPI data register
#define QUADSPI_PSMKR                    *(unsigned long *)(QUADSPI_BLOCK + 0x0024) // Quad-SPI polling status mask register
#define QUADSPI_PSMAR                    *(unsigned long *)(QUADSPI_BLOCK + 0x0028) // Quad-SPI polling status match register
#define QUADSPI_PIR                      *(unsigned long *)(QUADSPI_BLOCK + 0x002c) // Quad-SPI polling interval register
#define QUADSPI_LPTR                     *(unsigned long *)(QUADSPI_BLOCK + 0x0030) // Quad-SPI low power timeout register

// bxCAN
//
#define CAN1_MCR                         *(unsigned long *)(CAN1_BLOCK + 0x0000) // CAN 1 master control register
  #define CAN_MCR_INRQ                   0x00000001                      // initialisation request
  #define CAN_MCR_SLEEP                  0x00000002                      // sleep mode request
  #define CAN_MCR_TXFP                   0x00000004                      // transmit FIFO priority
  #define CAN_MCR_RFLM                   0x00000008                      // receive FIFO locked mode
  #define CAN_MCR_NART                   0x00000010                      // no automatic retransmission
  #define CAN_MCR_AWUM                   0x00000020                      // automatic wakeup mode
  #define CAN_MCR_ABOM                   0x00000040                      // automatic bus off management
  #define CAN_MCR_TTCM                   0x00000080                      // time triggered communication mode enable
  #define CAN_MCR_RESET                  0x00008000                      // bxCAN software master reset
  #define CAN_MCR_DBF                    0x00010000                      // debug freeze
#define CAN1_MSR                         *(volatile unsigned long *)(CAN1_BLOCK + 0x0004) // CAN 1 master status register
  #define CAN_MSR_INAK                   0x00000001                      // initialisation acknowledge
  #define CAN_MSR_SLAK                   0x00000002                      // sleep acknowledge
  #define CAN_MSR_ERRI                   0x00000004                      // error interrupt
  #define CAN_MSR_WKUI                   0x00000008                      // wakeup interrupt
  #define CAN_MSR_SLAKI                  0x00000010                      // sleep acknowledge interrupt
  #define CAN_MSR_TXM                    0x00000100                      // transmit mode active
  #define CAN_MSR_RXM                    0x00000200                      // receive mode active
  #define CAN_MSR_SAMP                   0x00000400                      // last sample point (current received bit value)
  #define CAN_MSR_RX                     0x00000800                      // CAN 1 Rx signal state
#define CAN1_TSR                         *(volatile unsigned long *)(CAN1_BLOCK + 0x0008) // CAN 1 transmit status register
  #define CAN_TSR_RQCP0                  0x00000001                      // request complete mailbox 0 (cleared by writing '1')
  #define CAN_TSR_TXOK0                  0x00000002                      // transmission OK of mailbox 0
  #define CAN_TSR_ALST0                  0x00000004                      // arbitration lost for mailbox 0
  #define CAN_TSR_TERR0                  0x00000008                      // transmit error of mailbox 0
  #define CAN_TSR_ABRQ0                  0x00000080                      // abort request for mailbox 0
  #define CAN_TSR_RQCP1                  0x00000100                      // request complete mailbox 1 (cleared by writing '1')
  #define CAN_TSR_TXOK1                  0x00000200                      // transmission OK of mailbox 1
  #define CAN_TSR_ALST1                  0x00000400                      // arbitration lost for mailbox 1
  #define CAN_TSR_TERR1                  0x00000800                      // transmit error of mailbox 1
  #define CAN_TSR_ABRQ1                  0x00008000                      // abort request for mailbox 1
  #define CAN_TSR_RQCP2                  0x00010000                      // request complete mailbox 2 (cleared by writing '1')
  #define CAN_TSR_TXOK2                  0x00020000                      // transmission OK of mailbox 2
  #define CAN_TSR_ALST2                  0x00040000                      // arbitration lost for mailbox 2
  #define CAN_TSR_TERR2                  0x00080000                      // transmit error of mailbox 2
  #define CAN_TSR_ABRQ2                  0x00800000                      // abort request for mailbox 2
  #define CAN_TSR_CODE_MASK              0x03000000                      // mailbox code mask
  #define CAN_TSR_TME0                   0x04000000                      // transmit mailbox 0 empty
  #define CAN_TSR_TME1                   0x08000000                      // transmit mailbox 1 empty
  #define CAN_TSR_TME2                   0x10000000                      // transmit mailbox 2 empty
  #define CAN_TSR_LOW0                   0x20000000                      // lowest priority flag for mailbox 0
  #define CAN_TSR_LOW1                   0x40000000                      // lowest priority flag for mailbox 1
  #define CAN_TSR_LOW2                   0x80000000                      // lowest priority flag for mailbox 2
#define CAN1_RF0R                        *(volatile unsigned long *)(CAN1_BLOCK + 0x000c) // CAN 1 receive FIFO 0 register
  #define CAN_RFR_FMP_MASK               0x00000003                      // pending message count
  #define CAN_RFR_FULL                   0x00000008                      // FIFO full
  #define CAN_RFR_FOVR                   0x00000010                      // FIFO overrun
  #define CAN_RFR_RFOM                   0x00000020                      // release FIFO output mailbox
#define CAN1_RF1R                        *(volatile unsigned long *)(CAN1_BLOCK + 0x0010) // CAN 1 receive FIFO 1 register
#define CAN1_IER                         *(unsigned long *)(CAN1_BLOCK + 0x0014) // CAN 1 interrupt enable register
  #define CAN_IER_TMEIE                  0x00000001                      // transmit mailbox empty interrupt enable
  #define CAN_IER_FMPIE0                 0x00000002                      // FIFO message pending interrupt enable - mailbox 0
  #define CAN_IER_FFIE0                  0x00000004                      // FIFO full interrupt enable - mailbox 0
  #define CAN_IER_FOVIE0                 0x00000008                      // FIFO overrun interrupt enable - mailbox 0
  #define CAN_IER_FMPIE1                 0x00000010                      // FIFO message pending interrupt enable - mailbox 1
  #define CAN_IER_FFIE1                  0x00000020                      // FIFO full interrupt enable - mailbox 1
  #define CAN_IER_FOVIE1                 0x00000040                      // FIFO overrun interrupt enable - mailbox 1
  #define CAN_IER_EWGIE                  0x00000100                      // error warning interrupt enable
  #define CAN_IER_EPVIE                  0x00000200                      // error passive interrupt enable
  #define CAN_IER_BOFIE                  0x00000400                      // bus off interrupt enable
  #define CAN_IER_LECIE                  0x00000800                      // last error code interrupt enable
  #define CAN_IER_ERRIE                  0x00008000                      // error interrupt enable
  #define CAN_IER_WKUIE                  0x00010000                      // wakeup interrup enable
  #define CAN_IER_SLKIE                  0x00020000                      // sleep interrup enable
#define CAN1_ESR                         *(volatile unsigned long *)(CAN1_BLOCK + 0x0018) // CAN 1 error status register
  #define CAN_ESR_EWGF                   0x00000001                      // error warning flag
  #define CAN_ESR_EPVF                   0x00000002                      // error passive flag
  #define CAN_ESR_BOFF                   0x00000004                      // bus off flag
  #define CAN_ESR_LEC_NO_ERROR           0x00000000                      // last error code - no error
  #define CAN_ESR_LEC_STUFF_ERROR        0x00000010                      // last error code - stuff error
  #define CAN_ESR_LEC_FORM_ERROR         0x00000020                      // last error code - form error
  #define CAN_ESR_LEC_ACK_ERROR          0x00000030                      // last error code - acknowledgement error
  #define CAN_ESR_LEC_BIT_REC_ERROR      0x00000040                      // last error code - bit recessive error
  #define CAN_ESR_LEC_BIT_DOM_ERROR      0x00000050                      // last error code - bit dominant error
  #define CAN_ESR_LEC_CRC_ERROR          0x00000060                      // last error code - crc error
  #define CAN_ESR_LEC_SW                 0x00000070                      // last error code - set by software
  #define CAN_ESR_TEC_MASK               0x00ff0000                      // transmit error counter (LS byte of 9 bit counter)
  #define CAN_ESR_REC_MASK               0xff000000                      // receive error counter
#define CAN1_BTR                         *(unsigned long *)(CAN1_BLOCK + 0x001c) // CAN 1 bit timing register (write only when in initialising mode)
  #define CAN_BTR_BRP_MASK               0x000003ff                      // baud rate prescaler (1..512)
  #define CAN_BTR_TS1_MASK               0x000f0000                      // time segment 1 quanta (1..16)
  #define CAN_BTR_TS1_SHIFT              16
  #define CAN_BTR_TS2_1                  0x00000000                      // time segment 2 quanta - 1 time quanta
  #define CAN_BTR_TS2_2                  0x00100000                      // time segment 2 quanta - 2 time quanta
  #define CAN_BTR_TS2_3                  0x00200000                      // time segment 2 quanta - 3 time quanta
  #define CAN_BTR_TS2_4                  0x00300000                      // time segment 2 quanta - 4 time quanta
  #define CAN_BTR_TS2_5                  0x00400000                      // time segment 2 quanta - 5 time quanta
  #define CAN_BTR_TS2_6                  0x00500000                      // time segment 2 quanta - 6 time quanta
  #define CAN_BTR_TS2_7                  0x00600000                      // time segment 2 quanta - 7 time quanta
  #define CAN_BTR_TS2_8                  0x00700000                      // time segment 2 quanta - 8 time quanta
  #define CAN_BTR_SJW_1                  0x00000000                      // resynchronisation jump width - 1 time quanta
  #define CAN_BTR_SJW_2                  0x01000000                      // resynchronisation jump width - 2 time quanta
  #define CAN_BTR_SJW_3                  0x02000000                      // resynchronisation jump width - 3 time quanta
  #define CAN_BTR_SJW_4                  0x03000000                      // resynchronisation jump width - 4 time quanta
  #define CAN_BTR_LBKM                   0x40000000                      // loop back mode (debug)
  #define CAN_BTR_SILM                   0x80000000                      // silen mode (debug)

#define ADDR_CAN1_MAILBOX_TX             (CAN_MAILBOX *)(CAN1_BLOCK + 0x0180)
#define CAN1_TI0R                        *(volatile unsigned long *)(CAN1_BLOCK + 0x0180) // CAN 1 TX mailbox identifier register 0
  #define CAN_TIR_TXRQ                   0x00000001                      // transmit mailbox request
  #define CAN_IR_DATA                    0x00000000                      // data frame
  #define CAN_IR_RTR                     0x00000002                      // remote frame
  #define CAN_IR_STD                     0x00000000                      // standard identifier
  #define CAN_IR_IDE                     0x00000004                      // extended identifier
  #define CAN_IR_STANDARD_ID_MASK        0xffe00000                      // standard identifier mask
  #define CAN_IR_STANDARD_ID_SHIFT       21                              // standard identifier shift
  #define CAN_IR_EXTENDED_ID_MASK        0xfffffff8                      // extended identifier mask
  #define CAN_IR_EXTENDED_ID_SHIFT       3                               // extended identifier shift
#define CAN1_TDT0R                       *(volatile unsigned long *)(CAN1_BLOCK + 0x0184) // CAN 1 mailbox data length and time stamp register 0 (only writable when mailbox is in the empty state)
  #define CAN_TDTR_DLC_MASK              0x0000000f                      // data code length mask (0..8)
  #define CAN_TDTR_TGT                   0x00000100                      // transmit global time
  #define CAN_TDTR_TIME_MASK             0xffff0000                      // message time stamp
#define CAN1_TDL0R                       *(unsigned long *)(CAN1_BLOCK + 0x0188) // CAN 1 mailbox data low register 0 (DATA3..0)
#define CAN1_TDH0R                       *(unsigned long *)(CAN1_BLOCK + 0x018c) // CAN 1 mailbox data high register 0 (DATA7..4)
#define CAN1_TI1R                        *(volatile unsigned long *)(CAN1_BLOCK + 0x0190) // CAN 1 TX mailbox identifier register 1
#define CAN1_TDT1R                       *(volatile unsigned long *)(CAN1_BLOCK + 0x0194) // CAN 1 mailbox data length and time stamp register 1 (only writable when mailbox is in the empty state)
#define CAN1_TDL1R                       *(unsigned long *)(CAN1_BLOCK + 0x0198) // CAN 1 mailbox data low register 1 (DATA3..0)
#define CAN1_TDH1R                       *(unsigned long *)(CAN1_BLOCK + 0x019c) // CAN 1 mailbox data high register 1 (DATA7..4)
#define CAN1_TI2R                        *(volatile unsigned long *)(CAN1_BLOCK + 0x01a0) // CAN 1 TX mailbox identifier register 2
#define CAN1_TDT2R                       *(volatile unsigned long *)(CAN1_BLOCK + 0x01a4) // CAN 1 mailbox data length and time stamp register 2 (only writable when mailbox is in the empty state)
#define CAN1_TDL2R                       *(unsigned long *)(CAN1_BLOCK + 0x01a8) // CAN 1 mailbox data low register 2 (DATA3..0)
#define CAN1_TDH2R                       *(unsigned long *)(CAN1_BLOCK + 0x01ac) // CAN 1 mailbox data high register 2 (DATA7..4)
#define ADDR_CAN1_MAILBOX_0_RX           (CAN_MAILBOX *)(CAN1_BLOCK + 0x01b0)
#define CAN1_RI0R                        *(volatile unsigned long *)(CAN1_BLOCK + 0x01b0) // CAN 1 RX FIFO mailbox identifier register 0 (read-only)
  #define CAN_RIR_RTR                    0x00000002                      // remote transmission request
  #define CAN_RIR_IDE                    0x00000004                      // identifier extension
  #define CAN_RIR_STID_MASK              0xffe00000                      // standard identifier mask
  #define CAN_RIR_STID_SHIFT             21                              // standard identifier shift
  #define CAN_RIR_EXID_MASK              0xfffffff8                      // extended identifier mask
  #define CAN_RIR_EXID_SHIFT             3                               // extended identifier shift
#define CAN1_RDT0R                       *(volatile unsigned long *)(CAN1_BLOCK + 0x01b4) // CAN 1 RX FIFO mailbox data length and time stamp register 0 (read-only)
  #define CAN_RDTR_DLC_MASK              0x0000000f                      // data length code
  #define CAN_RDTR_FMI_MASK              0x0000ff00                      // filter mask index
  #define CAN_RDTR_FMI_SHIFT             8
  #define CAN_RDTR_TIME_MASK             0xffff0000                      // message time stamp
#define CAN1_RDL0R                       *(volatile unsigned long *)(CAN1_BLOCK + 0x01b8) // CAN 1 RX FIFO mailbox data low register 0 (DATA3..0) (read-only)
#define CAN1_RDH0R                       *(volatile unsigned long *)(CAN1_BLOCK + 0x01bc) // CAN 1 RX FIFO mailbox data high register 0 (DATA7..4) (read-only)
#define ADDR_CAN1_MAILBOX_1_RX           (CAN_MAILBOX *)(CAN1_BLOCK + 0x01c0)
#define CAN1_RI1R                        *(volatile unsigned long *)(CAN1_BLOCK + 0x01c0) // CAN 1 RX FIFO mailbox identifier register 1 (read-only)
#define CAN1_RDT1R                       *(volatile unsigned long *)(CAN1_BLOCK + 0x01c4) // CAN 1 RX FIFO mailbox data length and time stamp register 1 (read-only)
#define CAN1_RDL1R                       *(volatile unsigned long *)(CAN1_BLOCK + 0x01c8) // CAN 1 RX FIFO mailbox data low register 1 (DATA3..0) (read-only)
#define CAN1_RDH1R                       *(volatile unsigned long *)(CAN1_BLOCK + 0x01cc) // CAN 1 RX FIFO mailbox data high register 1 (DATA7..4) (read-only)

#define CAN1_FMR                         *(unsigned long *)(CAN1_BLOCK + 0x0200) // CAN 1 filter master register
  #define CAN_FMR_FINIT                  0x00000001                      // initialisation mode for the filters
  #define CAN_FMR_CAN2SB_MASK            0x00003f00                      // CAN2 start bank mask (start bank for the CAN2 slave interface (range 1..127))
  #define CAN_FMR_CAN2SB_SHIFT           8
#define CAN1_FM1R                        *(unsigned long *)(CAN1_BLOCK + 0x0204) // CAN 1 filter mode register (only writable when FINIT is set in CAN_FMR)
  #define CAN_FM1R_FBM0_MASK_MODE        0x00000000                      // two 32 bit registers of bank 0 are in identifier mask mode
  #define CAN_FM1R_FBM0_LIST_MODE        0x00000001                      // two 32 bit registers of bank 0 are in identifier list mode
  #define CAN_FM1R_FBM1_MASK_MODE        0x00000000                      // two 32 bit registers of bank 1 are in identifier mask mode
  #define CAN_FM1R_FBM1_LIST_MODE        0x00000002                      // two 32 bit registers of bank 1 are in identifier list mode
  #define CAN_FM1R_FBM2_MASK_MODE        0x00000000                      // two 32 bit registers of bank 2 are in identifier mask mode
  #define CAN_FM1R_FBM2_LIST_MODE        0x00000004                      // two 32 bit registers of bank 2 are in identifier list mode
  #define CAN_FM1R_FBM3_MASK_MODE        0x00000000                      // two 32 bit registers of bank 3 are in identifier mask mode
  #define CAN_FM1R_FBM3_LIST_MODE        0x00000008                      // two 32 bit registers of bank 3 are in identifier list mode
  #define CAN_FM1R_FBM4_MASK_MODE        0x00000000                      // two 32 bit registers of bank 4 are in identifier mask mode
  #define CAN_FM1R_FBM4_LIST_MODE        0x00000010                      // two 32 bit registers of bank 4 are in identifier list mode
  #define CAN_FM1R_FBM5_MASK_MODE        0x00000000                      // two 32 bit registers of bank 5 are in identifier mask mode
  #define CAN_FM1R_FBM5_LIST_MODE        0x00000020                      // two 32 bit registers of bank 5 are in identifier list mode
  #define CAN_FM1R_FBM6_MASK_MODE        0x00000000                      // two 32 bit registers of bank 6 are in identifier mask mode
  #define CAN_FM1R_FBM6_LIST_MODE        0x00000040                      // two 32 bit registers of bank 6 are in identifier list mode
  #define CAN_FM1R_FBM7_MASK_MODE        0x00000000                      // two 32 bit registers of bank 7 are in identifier mask mode
  #define CAN_FM1R_FBM7_LIST_MODE        0x00000080                      // two 32 bit registers of bank 7 are in identifier list mode
  #define CAN_FM1R_FBM8_MASK_MODE        0x00000000                      // two 32 bit registers of bank 8 are in identifier mask mode
  #define CAN_FM1R_FBM8_LIST_MODE        0x00000100                      // two 32 bit registers of bank 8 are in identifier list mode
  #define CAN_FM1R_FBM9_MASK_MODE        0x00000000                      // two 32 bit registers of bank 9 are in identifier mask mode
  #define CAN_FM1R_FBM9_LIST_MODE        0x00000200                      // two 32 bit registers of bank 9 are in identifier list mode
  #define CAN_FM1R_FBM10_MASK_MODE       0x00000000                      // two 32 bit registers of bank 10 are in identifier mask mode
  #define CAN_FM1R_FBM10_LIST_MODE       0x00000400                      // two 32 bit registers of bank 10 are in identifier list mode
  #define CAN_FM1R_FBM11_MASK_MODE       0x00000000                      // two 32 bit registers of bank 11 are in identifier mask mode
  #define CAN_FM1R_FBM11_LIST_MODE       0x00000800                      // two 32 bit registers of bank 11 are in identifier list mode
  #define CAN_FM1R_FBM12_MASK_MODE       0x00000000                      // two 32 bit registers of bank 12 are in identifier mask mode
  #define CAN_FM1R_FBM12_LIST_MODE       0x00001000                      // two 32 bit registers of bank 12 are in identifier list mode
  #define CAN_FM1R_FBM13_MASK_MODE       0x00000000                      // two 32 bit registers of bank 13 are in identifier mask mode
  #define CAN_FM1R_FBM13_LIST_MODE       0x00002000                      // two 32 bit registers of bank 13 are in identifier list mode
  #define CAN_FM1R_FBM14_MASK_MODE       0x00000000                      // two 32 bit registers of bank 14 are in identifier mask mode
  #define CAN_FM1R_FBM14_LIST_MODE       0x00004000                      // two 32 bit registers of bank 14 are in identifier list mode
  #define CAN_FM1R_FBM15_MASK_MODE       0x00000000                      // two 32 bit registers of bank 15 are in identifier mask mode
  #define CAN_FM1R_FBM15_LIST_MODE       0x00008000                      // two 32 bit registers of bank 15 are in identifier list mode
  #define CAN_FM1R_FBM16_MASK_MODE       0x00000000                      // two 32 bit registers of bank 16 are in identifier mask mode
  #define CAN_FM1R_FBM16_LIST_MODE       0x00010000                      // two 32 bit registers of bank 16 are in identifier list mode
  #define CAN_FM1R_FBM17_MASK_MODE       0x00000000                      // two 32 bit registers of bank 17 are in identifier mask mode
  #define CAN_FM1R_FBM17_LIST_MODE       0x00020000                      // two 32 bit registers of bank 17 are in identifier list mode
  #define CAN_FM1R_FBM18_MASK_MODE       0x00000000                      // two 32 bit registers of bank 18 are in identifier mask mode
  #define CAN_FM1R_FBM18_LIST_MODE       0x00040000                      // two 32 bit registers of bank 18 are in identifier list mode
  #define CAN_FM1R_FBM19_MASK_MODE       0x00000000                      // two 32 bit registers of bank 19 are in identifier mask mode
  #define CAN_FM1R_FBM19_LIST_MODE       0x00080000                      // two 32 bit registers of bank 19 are in identifier list mode
  #define CAN_FM1R_FBM20_MASK_MODE       0x00000000                      // two 32 bit registers of bank 20 are in identifier mask mode
  #define CAN_FM1R_FBM20_LIST_MODE       0x00100000                      // two 32 bit registers of bank 20 are in identifier list mode
  #define CAN_FM1R_FBM21_MASK_MODE       0x00000000                      // two 32 bit registers of bank 21 are in identifier mask mode
  #define CAN_FM1R_FBM21_LIST_MODE       0x00200000                      // two 32 bit registers of bank 21 are in identifier list mode
  #define CAN_FM1R_FBM22_MASK_MODE       0x00000000                      // two 32 bit registers of bank 22 are in identifier mask mode
  #define CAN_FM1R_FBM22_LIST_MODE       0x00400000                      // two 32 bit registers of bank 22 are in identifier list mode
  #define CAN_FM1R_FBM23_MASK_MODE       0x00000000                      // two 32 bit registers of bank 23 are in identifier mask mode
  #define CAN_FM1R_FBM23_LIST_MODE       0x00800000                      // two 32 bit registers of bank 23 are in identifier list mode
  #define CAN_FM1R_FBM24_MASK_MODE       0x00000000                      // two 32 bit registers of bank 24 are in identifier mask mode
  #define CAN_FM1R_FBM24_LIST_MODE       0x01000000                      // two 32 bit registers of bank 24 are in identifier list mode
  #define CAN_FM1R_FBM25_MASK_MODE       0x00000000                      // two 32 bit registers of bank 25 are in identifier mask mode
  #define CAN_FM1R_FBM25_LIST_MODE       0x02000000                      // two 32 bit registers of bank 25 are in identifier list mode
  #define CAN_FM1R_FBM26_MASK_MODE       0x00000000                      // two 32 bit registers of bank 26 are in identifier mask mode
  #define CAN_FM1R_FBM26_LIST_MODE       0x04000000                      // two 32 bit registers of bank 26 are in identifier list mode
  #define CAN_FM1R_FBM27_MASK_MODE       0x00000000                      // two 32 bit registers of bank 27 are in identifier mask mode
  #define CAN_FM1R_FBM27_LIST_MODE       0x08000000                      // two 32 bit registers of bank 27 are in identifier list mode
#define CAN1_FS1R                        *(unsigned long *)(CAN1_BLOCK + 0x020c) // CAN 1 filter scale register (only writable when FINIT is set in CAN_FMR)
  #define CAN_FS1R_FSC0_DUAL_16_SCALE    0x00000000                      // dual 16-bit scale configuration
  #define CAN_FS1R_FSC0_SINGLE_32_SCALE  0x00000001                      // single 32-bit scale configuration
  #define CAN_FS1R_FSC1_DUAL_16_SCALE    0x00000000                      // dual 16-bit scale configuration
  #define CAN_FS1R_FSC1_SINGLE_32_SCALE  0x00000002                      // single 32-bit scale configuration
  #define CAN_FS1R_FSC2_DUAL_16_SCALE    0x00000000                      // dual 16-bit scale configuration
  #define CAN_FS1R_FSC2_SINGLE_32_SCALE  0x00000004                      // single 32-bit scale configuration
  #define CAN_FS1R_FSC3_DUAL_16_SCALE    0x00000000                      // dual 16-bit scale configuration
  #define CAN_FS1R_FSC3_SINGLE_32_SCALE  0x00000008                      // single 32-bit scale configuration
  #define CAN_FS1R_FSC4_DUAL_16_SCALE    0x00000000                      // dual 16-bit scale configuration
  #define CAN_FS1R_FSC4_SINGLE_32_SCALE  0x00000010                      // single 32-bit scale configuration
  #define CAN_FS1R_FSC5_DUAL_16_SCALE    0x00000000                      // dual 16-bit scale configuration
  #define CAN_FS1R_FSC5_SINGLE_32_SCALE  0x00000020                      // single 32-bit scale configuration
  #define CAN_FS1R_FSC6_DUAL_16_SCALE    0x00000000                      // dual 16-bit scale configuration
  #define CAN_FS1R_FSC6_SINGLE_32_SCALE  0x00000040                      // single 32-bit scale configuration
  #define CAN_FS1R_FSC7_DUAL_16_SCALE    0x00000000                      // dual 16-bit scale configuration
  #define CAN_FS1R_FSC7_SINGLE_32_SCALE  0x00000080                      // single 32-bit scale configuration
  #define CAN_FS1R_FSC8_DUAL_16_SCALE    0x00000000                      // dual 16-bit scale configuration
  #define CAN_FS1R_FSC8_SINGLE_32_SCALE  0x00000100                      // single 32-bit scale configuration
  #define CAN_FS1R_FSC9_DUAL_16_SCALE    0x00000000                      // dual 16-bit scale configuration
  #define CAN_FS1R_FSC9_SINGLE_32_SCALE  0x00000200                      // single 32-bit scale configuration
  #define CAN_FS1R_FSC10_DUAL_16_SCALE   0x00000000                      // dual 16-bit scale configuration
  #define CAN_FS1R_FSC10_SINGLE_32_SCALE 0x00000400                      // single 32-bit scale configuration
  #define CAN_FS1R_FSC11_DUAL_16_SCALE   0x00000000                      // dual 16-bit scale configuration
  #define CAN_FS1R_FSC11_SINGLE_32_SCALE 0x00000800                      // single 32-bit scale configuration
  #define CAN_FS1R_FSC12_DUAL_16_SCALE   0x00000000                      // dual 16-bit scale configuration
  #define CAN_FS1R_FSC12_SINGLE_32_SCALE 0x00001000                      // single 32-bit scale configuration
  #define CAN_FS1R_FSC13_DUAL_16_SCALE   0x00000000                      // dual 16-bit scale configuration
  #define CAN_FS1R_FSC13_SINGLE_32_SCALE 0x00002000                      // single 32-bit scale configuration
  #define CAN_FS1R_FSC14_DUAL_16_SCALE   0x00000000                      // dual 16-bit scale configuration
  #define CAN_FS1R_FSC14_SINGLE_32_SCALE 0x00004000                      // single 32-bit scale configuration
  #define CAN_FS1R_FSC15_DUAL_16_SCALE   0x00000000                      // dual 16-bit scale configuration
  #define CAN_FS1R_FSC15_SINGLE_32_SCALE 0x00008000                      // single 32-bit scale configuration
  #define CAN_FS1R_FSC16_DUAL_16_SCALE   0x00000000                      // dual 16-bit scale configuration
  #define CAN_FS1R_FSC16_SINGLE_32_SCALE 0x00010000                      // single 32-bit scale configuration
  #define CAN_FS1R_FSC17_DUAL_16_SCALE   0x00000000                      // dual 16-bit scale configuration
  #define CAN_FS1R_FSC17_SINGLE_32_SCALE 0x00020000                      // single 32-bit scale configuration
  #define CAN_FS1R_FSC18_DUAL_16_SCALE   0x00000000                      // dual 16-bit scale configuration
  #define CAN_FS1R_FSC18_SINGLE_32_SCALE 0x00040000                      // single 32-bit scale configuration
  #define CAN_FS1R_FSC19_DUAL_16_SCALE   0x00000000                      // dual 16-bit scale configuration
  #define CAN_FS1R_FSC19_SINGLE_32_SCALE 0x00080000                      // single 32-bit scale configuration
  #define CAN_FS1R_FSC20_DUAL_16_SCALE   0x00000000                      // dual 16-bit scale configuration
  #define CAN_FS1R_FSC20_SINGLE_32_SCALE 0x00100000                      // single 32-bit scale configuration
  #define CAN_FS1R_FSC21_DUAL_16_SCALE   0x00000000                      // dual 16-bit scale configuration
  #define CAN_FS1R_FSC21_SINGLE_32_SCALE 0x00200000                      // single 32-bit scale configuration
  #define CAN_FS1R_FSC22_DUAL_16_SCALE   0x00000000                      // dual 16-bit scale configuration
  #define CAN_FS1R_FSC22_SINGLE_32_SCALE 0x00400000                      // single 32-bit scale configuration
  #define CAN_FS1R_FSC23_DUAL_16_SCALE   0x00000000                      // dual 16-bit scale configuration
  #define CAN_FS1R_FSC23_SINGLE_32_SCALE 0x00800000                      // single 32-bit scale configuration
  #define CAN_FS1R_FSC24_DUAL_16_SCALE   0x00000000                      // dual 16-bit scale configuration
  #define CAN_FS1R_FSC24_SINGLE_32_SCALE 0x01000000                      // single 32-bit scale configuration
  #define CAN_FS1R_FSC25_DUAL_16_SCALE   0x00000000                      // dual 16-bit scale configuration
  #define CAN_FS1R_FSC25_SINGLE_32_SCALE 0x02000000                      // single 32-bit scale configuration
  #define CAN_FS1R_FSC26_DUAL_16_SCALE   0x00000000                      // dual 16-bit scale configuration
  #define CAN_FS1R_FSC26_SINGLE_32_SCALE 0x04000000                      // single 32-bit scale configuration
  #define CAN_FS1R_FSC27_DUAL_16_SCALE   0x00000000                      // dual 16-bit scale configuration
  #define CAN_FS1R_FSC27_SINGLE_32_SCALE 0x08000000                      // single 32-bit scale configuration
#define CAN1_FFA1R                       *(unsigned long *)(CAN1_BLOCK + 0x0214) // CAN 1 filter FIFO assignment register
  #define CAN_FFA1R_FFA0_FIFO0           0x00000000                      // filter associated to FIFO 0
  #define CAN_FFA1R_FFA0_FIFO1           0x00000001                      // filter associated to FIFO 1
  #define CAN_FFA1R_FFA1_FIFO0           0x00000000                      // filter associated to FIFO 0
  #define CAN_FFA1R_FFA1_FIFO1           0x00000002                      // filter associated to FIFO 1
  #define CAN_FFA1R_FFA2_FIFO0           0x00000000                      // filter associated to FIFO 0
  #define CAN_FFA1R_FFA2_FIFO1           0x00000004                      // filter associated to FIFO 1
  #define CAN_FFA1R_FFA3_FIFO0           0x00000000                      // filter associated to FIFO 0
  #define CAN_FFA1R_FFA3_FIFO1           0x00000008                      // filter associated to FIFO 1
  #define CAN_FFA1R_FFA4_FIFO0           0x00000000                      // filter associated to FIFO 0
  #define CAN_FFA1R_FFA4_FIFO1           0x00000010                      // filter associated to FIFO 1
  #define CAN_FFA1R_FFA5_FIFO0           0x00000000                      // filter associated to FIFO 0
  #define CAN_FFA1R_FFA5_FIFO1           0x00000020                      // filter associated to FIFO 1
  #define CAN_FFA1R_FFA6_FIFO0           0x00000000                      // filter associated to FIFO 0
  #define CAN_FFA1R_FFA6_FIFO1           0x00000040                      // filter associated to FIFO 1
  #define CAN_FFA1R_FFA7_FIFO0           0x00000000                      // filter associated to FIFO 0
  #define CAN_FFA1R_FFA7_FIFO1           0x00000080                      // filter associated to FIFO 1
  #define CAN_FFA1R_FFA8_FIFO0           0x00000000                      // filter associated to FIFO 0
  #define CAN_FFA1R_FFA8_FIFO1           0x00000100                      // filter associated to FIFO 1
  #define CAN_FFA1R_FFA9_FIFO0           0x00000000                      // filter associated to FIFO 0
  #define CAN_FFA1R_FFA9_FIFO1           0x00000200                      // filter associated to FIFO 1
  #define CAN_FFA1R_FFA10_FIFO0          0x00000000                      // filter associated to FIFO 0
  #define CAN_FFA1R_FFA10_FIFO1          0x00000400                      // filter associated to FIFO 1
  #define CAN_FFA1R_FFA11_FIFO0          0x00000000                      // filter associated to FIFO 0
  #define CAN_FFA1R_FFA11_FIFO1          0x00000800                      // filter associated to FIFO 1
  #define CAN_FFA1R_FFA12_FIFO0          0x00000000                      // filter associated to FIFO 0
  #define CAN_FFA1R_FFA12_FIFO1          0x00001000                      // filter associated to FIFO 1
  #define CAN_FFA1R_FFA13_FIFO0          0x00000000                      // filter associated to FIFO 0
  #define CAN_FFA1R_FFA13_FIFO1          0x00002000                      // filter associated to FIFO 1
  #define CAN_FFA1R_FFA14_FIFO0          0x00000000                      // filter associated to FIFO 0
  #define CAN_FFA1R_FFA14_FIFO1          0x00004000                      // filter associated to FIFO 1
  #define CAN_FFA1R_FFA15_FIFO0          0x00000000                      // filter associated to FIFO 0
  #define CAN_FFA1R_FFA15_FIFO1          0x00008000                      // filter associated to FIFO 1
  #define CAN_FFA1R_FFA16_FIFO0          0x00000000                      // filter associated to FIFO 0
  #define CAN_FFA1R_FFA16_FIFO1          0x00010000                      // filter associated to FIFO 1
  #define CAN_FFA1R_FFA17_FIFO0          0x00000000                      // filter associated to FIFO 0
  #define CAN_FFA1R_FFA17_FIFO1          0x00020000                      // filter associated to FIFO 1
  #define CAN_FFA1R_FFA18_FIFO0          0x00000000                      // filter associated to FIFO 0
  #define CAN_FFA1R_FFA18_FIFO1          0x00040000                      // filter associated to FIFO 1
  #define CAN_FFA1R_FFA19_FIFO0          0x00000000                      // filter associated to FIFO 0
  #define CAN_FFA1R_FFA19_FIFO1          0x00080000                      // filter associated to FIFO 1
  #define CAN_FFA1R_FFA20_FIFO0          0x00000000                      // filter associated to FIFO 0
  #define CAN_FFA1R_FFA20_FIFO1          0x00100000                      // filter associated to FIFO 1
  #define CAN_FFA1R_FFA21_FIFO0          0x00000000                      // filter associated to FIFO 0
  #define CAN_FFA1R_FFA21_FIFO1          0x00200000                      // filter associated to FIFO 1
  #define CAN_FFA1R_FFA22_FIFO0          0x00000000                      // filter associated to FIFO 0
  #define CAN_FFA1R_FFA22_FIFO1          0x00400000                      // filter associated to FIFO 1
  #define CAN_FFA1R_FFA23_FIFO0          0x00000000                      // filter associated to FIFO 0
  #define CAN_FFA1R_FFA23_FIFO1          0x00800000                      // filter associated to FIFO 1
  #define CAN_FFA1R_FFA24_FIFO0          0x00000000                      // filter associated to FIFO 0
  #define CAN_FFA1R_FFA24_FIFO1          0x01000000                      // filter associated to FIFO 1
  #define CAN_FFA1R_FFA25_FIFO0          0x00000000                      // filter associated to FIFO 0
  #define CAN_FFA1R_FFA25_FIFO1          0x02000000                      // filter associated to FIFO 1
  #define CAN_FFA1R_FFA26_FIFO0          0x00000000                      // filter associated to FIFO 0
  #define CAN_FFA1R_FFA26_FIFO1          0x04000000                      // filter associated to FIFO 1
  #define CAN_FFA1R_FFA27_FIFO0          0x00000000                      // filter associated to FIFO 0
  #define CAN_FFA1R_FFA27_FIFO1          0x08000000                      // filter associated to FIFO 1

#define CAN1_FA1R                        *(unsigned long *)(CAN1_BLOCK + 0x021c) // CAN 1 filter activation register
  #define CAN_FA1R_FACT0OFF              0x00000000                      // filter is not active
  #define CAN_FA1R_FACT0ON               0x00000001                      // filter is active
  #define CAN_FA1R_FACT1OFF              0x00000000                      // filter is not active
  #define CAN_FA1R_FACT1ON               0x00000002                      // filter is active
  #define CAN_FA1R_FACT2OFF              0x00000000                      // filter is not active
  #define CAN_FA1R_FACT2ON               0x00000004                      // filter is active
  #define CAN_FA1R_FACT3OFF              0x00000000                      // filter is not active
  #define CAN_FA1R_FACT3ON               0x00000008                      // filter is active
  #define CAN_FA1R_FACT4OFF              0x00000000                      // filter is not active
  #define CAN_FA1R_FACT4ON               0x00000010                      // filter is active
  #define CAN_FA1R_FACT5OFF              0x00000000                      // filter is not active
  #define CAN_FA1R_FACT5ON               0x00000020                      // filter is active
  #define CAN_FA1R_FACT6OFF              0x00000000                      // filter is not active
  #define CAN_FA1R_FACT6ON               0x00000040                      // filter is active
  #define CAN_FA1R_FACT7OFF              0x00000000                      // filter is not active
  #define CAN_FA1R_FACT7ON               0x00000080                      // filter is active
  #define CAN_FA1R_FACT8OFF              0x00000000                      // filter is not active
  #define CAN_FA1R_FACT8ON               0x00000100                      // filter is active
  #define CAN_FA1R_FACT9OFF              0x00000000                      // filter is not active
  #define CAN_FA1R_FACT9ON               0x00000200                      // filter is active
  #define CAN_FA1R_FACT10OFF             0x00000000                      // filter is not active
  #define CAN_FA1R_FACT10ON              0x00000400                      // filter is active
  #define CAN_FA1R_FACT11OFF             0x00000000                      // filter is not active
  #define CAN_FA1R_FACT11ON              0x00000800                      // filter is active
  #define CAN_FA1R_FACT12OFF             0x00000000                      // filter is not active
  #define CAN_FA1R_FACT12ON              0x00001000                      // filter is active
  #define CAN_FA1R_FACT13OFF             0x00000000                      // filter is not active
  #define CAN_FA1R_FACT13ON              0x00002000                      // filter is active
  #define CAN_FA1R_FACT14OFF             0x00000000                      // filter is not active
  #define CAN_FA1R_FACT14ON              0x00004000                      // filter is active
  #define CAN_FA1R_FACT15OFF             0x00000000                      // filter is not active
  #define CAN_FA1R_FACT15ON              0x00008000                      // filter is active
  #define CAN_FA1R_FACT16OFF             0x00000000                      // filter is not active
  #define CAN_FA1R_FACT16ON              0x00010000                      // filter is active
  #define CAN_FA1R_FACT17OFF             0x00000000                      // filter is not active
  #define CAN_FA1R_FACT17ON              0x00020000                      // filter is active
  #define CAN_FA1R_FACT18OFF             0x00000000                      // filter is not active
  #define CAN_FA1R_FACT18ON              0x00040000                      // filter is active
  #define CAN_FA1R_FACT19OFF             0x00000000                      // filter is not active
  #define CAN_FA1R_FACT19ON              0x00080000                      // filter is active
  #define CAN_FA1R_FACT20OFF             0x00000000                      // filter is not active
  #define CAN_FA1R_FACT20ON              0x00100000                      // filter is active
  #define CAN_FA1R_FACT21OFF             0x00000000                      // filter is not active
  #define CAN_FA1R_FACT21ON              0x00200000                      // filter is active
  #define CAN_FA1R_FACT22OFF             0x00000000                      // filter is not active
  #define CAN_FA1R_FACT22ON              0x00400000                      // filter is active
  #define CAN_FA1R_FACT23OFF             0x00000000                      // filter is not active
  #define CAN_FA1R_FACT23ON              0x00800000                      // filter is active
  #define CAN_FA1R_FACT24OFF             0x00000000                      // filter is not active
  #define CAN_FA1R_FACT24ON              0x01000000                      // filter is active
  #define CAN_FA1R_FACT25OFF             0x00000000                      // filter is not active
  #define CAN_FA1R_FACT25ON              0x02000000                      // filter is active
  #define CAN_FA1R_FACT26OFF             0x00000000                      // filter is not active
  #define CAN_FA1R_FACT26ON              0x04000000                      // filter is active
  #define CAN_FA1R_FACT27OFF             0x00000000                      // filter is not active
  #define CAN_FA1R_FACT27ON              0x08000000                      // filter is active

#define CAN1_F0R1_ADDR                   (unsigned long *)(CAN1_BLOCK + 0x0240)
#define CAN1_F0R1                        *(unsigned long *)(CAN1_BLOCK + 0x0240) // CAN 1 filter bank register 0-1 - only modified when FINIT of CAN_FMR is set or when FACTx bit of CAN_FAxR is cleared
#define CAN1_F0R2                        *(unsigned long *)(CAN1_BLOCK + 0x0244) // CAN 1 filter bank register 0-2
#define CAN1_F1R1                        *(unsigned long *)(CAN1_BLOCK + 0x0248) // CAN 1 filter bank register 1-1
#define CAN1_F1R2                        *(unsigned long *)(CAN1_BLOCK + 0x024c) // CAN 1 filter bank register 1-2
#define CAN1_F2R1                        *(unsigned long *)(CAN1_BLOCK + 0x0250) // CAN 1 filter bank register 2-1
#define CAN1_F2R2                        *(unsigned long *)(CAN1_BLOCK + 0x0254) // CAN 1 filter bank register 2-2
#define CAN1_F3R1                        *(unsigned long *)(CAN1_BLOCK + 0x0258) // CAN 1 filter bank register 3-1
#define CAN1_F3R2                        *(unsigned long *)(CAN1_BLOCK + 0x025c) // CAN 1 filter bank register 3-2
#define CAN1_F4R1                        *(unsigned long *)(CAN1_BLOCK + 0x0260) // CAN 1 filter bank register 4-1
#define CAN1_F4R2                        *(unsigned long *)(CAN1_BLOCK + 0x0264) // CAN 1 filter bank register 4-2
#define CAN1_F5R1                        *(unsigned long *)(CAN1_BLOCK + 0x0268) // CAN 1 filter bank register 5-1
#define CAN1_F5R2                        *(unsigned long *)(CAN1_BLOCK + 0x026c) // CAN 1 filter bank register 5-2
#define CAN1_F6R1                        *(unsigned long *)(CAN1_BLOCK + 0x0270) // CAN 1 filter bank register 6-1
#define CAN1_F6R2                        *(unsigned long *)(CAN1_BLOCK + 0x0274) // CAN 1 filter bank register 6-2
#define CAN1_F7R1                        *(unsigned long *)(CAN1_BLOCK + 0x0278) // CAN 1 filter bank register 7-1
#define CAN1_F7R2                        *(unsigned long *)(CAN1_BLOCK + 0x027c) // CAN 1 filter bank register 7-2
#define CAN1_F8R1                        *(unsigned long *)(CAN1_BLOCK + 0x0280) // CAN 1 filter bank register 8-1
#define CAN1_F8R2                        *(unsigned long *)(CAN1_BLOCK + 0x0284) // CAN 1 filter bank register 8-2
#define CAN1_F9R1                        *(unsigned long *)(CAN1_BLOCK + 0x0288) // CAN 1 filter bank register 9-1
#define CAN1_F9R2                        *(unsigned long *)(CAN1_BLOCK + 0x028c) // CAN 1 filter bank register 9-2
#define CAN1_F10R1                       *(unsigned long *)(CAN1_BLOCK + 0x0290) // CAN 1 filter bank register 10-1
#define CAN1_F10R2                       *(unsigned long *)(CAN1_BLOCK + 0x0294) // CAN 1 filter bank register 10-2
#define CAN1_F11R1                       *(unsigned long *)(CAN1_BLOCK + 0x0298) // CAN 1 filter bank register 11-1
#define CAN1_F11R2                       *(unsigned long *)(CAN1_BLOCK + 0x029c) // CAN 1 filter bank register 11-2
#define CAN1_F12R1                       *(unsigned long *)(CAN1_BLOCK + 0x02a0) // CAN 1 filter bank register 12-1
#define CAN1_F12R2                       *(unsigned long *)(CAN1_BLOCK + 0x02a4) // CAN 1 filter bank register 12-2
#define CAN1_F13R1                       *(unsigned long *)(CAN1_BLOCK + 0x02a8) // CAN 1 filter bank register 13-1
#define CAN1_F13R2                       *(unsigned long *)(CAN1_BLOCK + 0x02ac) // CAN 1 filter bank register 13-2
#define CAN1_F14R1_ADDR                  (unsigned long *)(CAN1_BLOCK + 0x02b0)
#define CAN1_F14R1                       *(unsigned long *)(CAN1_BLOCK + 0x02b0) // CAN 1 filter bank register 14-1
#define CAN1_F14R2                       *(unsigned long *)(CAN1_BLOCK + 0x02b4) // CAN 1 filter bank register 14-2
#define CAN1_F15R1                       *(unsigned long *)(CAN1_BLOCK + 0x02b8) // CAN 1 filter bank register 15-1
#define CAN1_F15R2                       *(unsigned long *)(CAN1_BLOCK + 0x02bc) // CAN 1 filter bank register 15-2
#define CAN1_F16R1                       *(unsigned long *)(CAN1_BLOCK + 0x02c0) // CAN 1 filter bank register 16-1
#define CAN1_F16R2                       *(unsigned long *)(CAN1_BLOCK + 0x02c4) // CAN 1 filter bank register 16-2
#define CAN1_F17R1                       *(unsigned long *)(CAN1_BLOCK + 0x02c8) // CAN 1 filter bank register 17-1
#define CAN1_F17R2                       *(unsigned long *)(CAN1_BLOCK + 0x02cc) // CAN 1 filter bank register 17-2
#define CAN1_F18R1                       *(unsigned long *)(CAN1_BLOCK + 0x02d0) // CAN 1 filter bank register 18-1
#define CAN1_F18R2                       *(unsigned long *)(CAN1_BLOCK + 0x02d4) // CAN 1 filter bank register 18-2
#define CAN1_F19R1                       *(unsigned long *)(CAN1_BLOCK + 0x02d8) // CAN 1 filter bank register 19-1
#define CAN1_F19R2                       *(unsigned long *)(CAN1_BLOCK + 0x02dc) // CAN 1 filter bank register 19-2
#define CAN1_F20R1                       *(unsigned long *)(CAN1_BLOCK + 0x02e0) // CAN 1 filter bank register 20-1
#define CAN1_F20R2                       *(unsigned long *)(CAN1_BLOCK + 0x02e4) // CAN 1 filter bank register 20-2
#define CAN1_F21R1                       *(unsigned long *)(CAN1_BLOCK + 0x02e8) // CAN 1 filter bank register 21-1
#define CAN1_F21R2                       *(unsigned long *)(CAN1_BLOCK + 0x02ec) // CAN 1 filter bank register 21-2
#define CAN1_F22R1                       *(unsigned long *)(CAN1_BLOCK + 0x02f0) // CAN 1 filter bank register 22-1
#define CAN1_F22R2                       *(unsigned long *)(CAN1_BLOCK + 0x02f4) // CAN 1 filter bank register 22-2
#define CAN1_F23R1                       *(unsigned long *)(CAN1_BLOCK + 0x02f8) // CAN 1 filter bank register 23-1
#define CAN1_F23R2                       *(unsigned long *)(CAN1_BLOCK + 0x02fc) // CAN 1 filter bank register 23-2
#define CAN1_F24R1                       *(unsigned long *)(CAN1_BLOCK + 0x0300) // CAN 1 filter bank register 24-1
#define CAN1_F24R2                       *(unsigned long *)(CAN1_BLOCK + 0x0304) // CAN 1 filter bank register 24-2
#define CAN1_F25R1                       *(unsigned long *)(CAN1_BLOCK + 0x0308) // CAN 1 filter bank register 25-1
#define CAN1_F25R2                       *(unsigned long *)(CAN1_BLOCK + 0x030c) // CAN 1 filter bank register 25-2
#define CAN1_F26R1                       *(unsigned long *)(CAN1_BLOCK + 0x0310) // CAN 1 filter bank register 26-1
#define CAN1_F26R2                       *(unsigned long *)(CAN1_BLOCK + 0x0314) // CAN 1 filter bank register 26-2
#define CAN1_F27R1                       *(unsigned long *)(CAN1_BLOCK + 0x0318) // CAN 1 filter bank register 27-1
#define CAN1_F27R2                       *(unsigned long *)(CAN1_BLOCK + 0x031c) // CAN 1 filter bank register 27-2


#define CAN2_MCR                         *(unsigned long *)(CAN2_BLOCK + 0x0000) // CAN 2 master control register
#define CAN2_MSR                         *(volatile unsigned long *)(CAN2_BLOCK + 0x0004) // CAN 1 master status register
#define CAN2_TSR                         *(volatile unsigned long *)(CAN2_BLOCK + 0x0008) // CAN 2 transmit status register
#define CAN2_RF0R                        *(volatile unsigned long *)(CAN2_BLOCK + 0x000c) // CAN 2 receive FIFO 0 register
#define CAN2_RF1R                        *(volatile unsigned long *)(CAN2_BLOCK + 0x0010) // CAN 2 receive FIFO 1 register
#define CAN2_IER                         *(unsigned long *)(CAN2_BLOCK + 0x0014) // CAN 2 interrupt enable register
#define CAN2_ESR                         *(volatile unsigned long *)(CAN2_BLOCK + 0x0018) // CAN 2 error status register
#define CAN2_BTR                         *(unsigned long *)(CAN2_BLOCK + 0x001c) // CAN 2 bit timing register (write only when in initialising mode)

#define ADDR_CAN2_MAILBOX_TX             (CAN_MAILBOX *)(CAN2_BLOCK + 0x0180)
#define CAN2_TI0R                        *(volatile unsigned long *)(CAN2_BLOCK + 0x0180) // CAN 2 TX mailbox identifier register 0
#define CAN2_TDT0R                       *(volatile unsigned long *)(CAN2_BLOCK + 0x0184) // CAN 2 mailbox data length and time stamp register 0 (only writable when mailbox is in the empty state)
#define CAN2_TDL0R                       *(unsigned long *)(CAN2_BLOCK + 0x0188) // CAN 2 mailbox data low register 0 (DATA3..0)
#define CAN2_TDH0R                       *(unsigned long *)(CAN2_BLOCK + 0x018c) // CAN 2 mailbox data high register 0 (DATA7..4)
#define CAN2_TI1R                        *(volatile unsigned long *)(CAN2_BLOCK + 0x0190) // CAN 2 TX mailbox identifier register 1
#define CAN2_TDT1R                       *(volatile unsigned long *)(CAN2_BLOCK + 0x0194) // CAN 2 mailbox data length and time stamp register 1 (only writable when mailbox is in the empty state)
#define CAN2_TDL1R                       *(unsigned long *)(CAN2_BLOCK + 0x0198) // CAN 2 mailbox data low register 1 (DATA3..0)
#define CAN2_TDH1R                       *(unsigned long *)(CAN2_BLOCK + 0x019c) // CAN 2 mailbox data high register 1 (DATA7..4)
#define CAN2_TI2R                        *(volatile unsigned long *)(CAN2_BLOCK + 0x01a0) // CAN 2 TX mailbox identifier register 2
#define CAN2_TDT2R                       *(volatile unsigned long *)(CAN2_BLOCK + 0x01a4) // CAN 2 mailbox data length and time stamp register 2 (only writable when mailbox is in the empty state)
#define CAN2_TDL2R                       *(unsigned long *)(CAN2_BLOCK + 0x01a8) // CAN 2 mailbox data low register 2 (DATA3..0)
#define CAN2_TDH2R                       *(unsigned long *)(CAN2_BLOCK + 0x01ac) // CAN 2 mailbox data high register21 (DATA7..4)
#define ADDR_CAN2_MAILBOX_0_RX           (CAN_MAILBOX *)(CAN2_BLOCK + 0x01b0)
#define CAN2_RI0R                        *(volatile unsigned long *)(CAN2_BLOCK + 0x01b0) // CAN 2 RX FIFO mailbox identifier register 0 (read-only)
#define CAN2_RDT0R                       *(volatile unsigned long *)(CAN2_BLOCK + 0x01b4) // CAN 2 RX FIFO mailbox data length and time stamp register 0 (read-only)
#define CAN2_RDL0R                       *(volatile unsigned long *)(CAN2_BLOCK + 0x01b8) // CAN 2 RX FIFO mailbox data low register 0 (DATA3..0) (read-only)
#define CAN2_RDH0R                       *(volatile unsigned long *)(CAN2_BLOCK + 0x01bc) // CAN 2 RX FIFO mailbox data high register 0 (DATA7..4) (read-only)
#define ADDR_CAN2_MAILBOX_1_RX           (CAN_MAILBOX *)(CAN2_BLOCK + 0x01c0)
#define CAN2_RI1R                        *(volatile unsigned long *)(CAN2_BLOCK + 0x01c0) // CAN 2 RX FIFO mailbox identifier register 1 (read-only)
#define CAN2_RDT1R                       *(volatile unsigned long *)(CAN2_BLOCK + 0x01c4) // CAN 2 RX FIFO mailbox data length and time stamp register 1 (read-only)
#define CAN2_RDL1R                       *(volatile unsigned long *)(CAN2_BLOCK + 0x01c8) // CAN 2 RX FIFO mailbox data low register 1 (DATA3..0) (read-only)
#define CAN2_RDH1R                       *(volatile unsigned long *)(CAN2_BLOCK + 0x01cc) // CAN 2 RX FIFO mailbox data high register 1 (DATA7..4) (read-only)

#define CAN2_FMR                         *(unsigned long *)(CAN2_BLOCK + 0x0200) // CAN 2 filter master register
#define CAN2_FM1R                        *(unsigned long *)(CAN2_BLOCK + 0x0204) // CAN 2 filter mode register

#define CAN2_FS1R                        *(unsigned long *)(CAN2_BLOCK + 0x020c) // CAN 2 filter scale register (only writable when FINIT is set in CAN_FMR)

#define CAN2_FFA1R                       *(unsigned long *)(CAN2_BLOCK + 0x0214) // CAN 2 filter FIFO assignment register

#define CAN2_FA1R                        *(unsigned long *)(CAN2_BLOCK + 0x021c) // CAN 2 filter activation register

#define NUMBER_CAN_TX_MAILBOXES       3                                  // 3 transmit mailboxes available in the bxCAN module
#define NUMBER_CAN_RX_MAILBOXES       14                                 // there are 28 filter banks which are considered as mailboxes and half are allocated to the master (CAN 1) and half to the slave (CAN 2)
                                                                         // there are FIFOs of 3 messages depth to be shared by each filter bank; each odd filter bank is assigned to FIFO 1 and each even filter bank to FIFO 0
#define NUMBER_CAN_RX_FILTERS         28
#define NUMBER_OF_CAN_INTERFACES      2
#define CAN_EXTENDED_MASK             0x1fffffff
#define CAN_STANDARD_MASK             0x7ff

typedef struct stSTM_CAN_CONTROL                                         // {12}
{
    unsigned long CAN_MCR;
    volatile unsigned long CAN_MSR;
    volatile unsigned long CAN_TSR;
    volatile unsigned long CAN_RF0R;
    volatile unsigned long CAN_RF1R;
    unsigned long CAN_IER;
    volatile unsigned long CAN_ESR;
    unsigned long CAN_BTR;
} STM_CAN_CONTROL;

typedef struct stCAN_MAILBOX                                             // {12}
{
    volatile unsigned long CAN_TIR;
    volatile unsigned long CAN_TDTR;
    unsigned long CAN_TDLR;
    unsigned long CAN_TDHR;
} CAN_MAILBOX;


// Ethernet MAC
//
#define ETH_MACCR                        *(volatile unsigned long *)(ENET_BLOCK + 0x0000) // {16} EMAC configuration register
  #define ETH_MACCR_RE                   0x00000004                      // receiver enable
  #define ETH_MACCR_TE                   0x00000008                      // transmitter enable
  #define ETH_MACCR_DC                   0x00000010                      // deferral check
  #define ETH_MACCR_BL_MASK              0x00000060                      // back-off limit (only applicable in half-duplex mode)
  #define ETH_MACCR_APCS                 0x00000080                      // automatic pad/CRC stripping
  #define ETH_MACCR_RD                   0x00000200                      // retry disable (only applicable in half-duplex mode)
  #define ETH_MACCR_IPCO                 0x00000400                      // IPv4 checksum offload
  #define ETH_MACCR_DM                   0x00000800                      // duplex mode
  #define ETH_MACCR_LM                   0x00001000                      // loopback mode
  #define ETH_MACCR_ROD                  0x00002000                      // receiver own disabled (not applicable in full-duplex mode)
  #define ETH_MACCR_FES                  0x00004000                      // fast ethernet speed (100M rather than 10M)
  #define ETH_MACCR_CSD                  0x00010000                      // carrier sense disable
  #define ETH_MACCR_IFG_MASK             0x000e0000                      // interframe gap (b100 recommended for half-duplex mode)
  #define ETH_MACCR_IFG_64               0x00080000
  #define ETH_MACCR_IFG_96               0x00000000
  #define ETH_MACCR_JD                   0x00400000                      // jabber disable
  #define ETH_MACCR_WD                   0x00800000                      // watchdog disable
#define ETH_MACFFR                       *(unsigned long *)(ENET_BLOCK + 0x0004)           // EMA frame filter register
  #define ETH_MACFFR_PM                  0x00000001                      // promiscuous mode
  #define ETH_MACFFR_HU                  0x00000002                      // hash unicast
  #define ETH_MACFFR_HM                  0x00000004                      // hash multicast
  #define ETH_MACFFR_DAIF                0x00000008                      // destination address inverse filtering
  #define ETH_MACFFR_PAM                 0x00000010                      // pass all multicast
  #define ETH_MACFFR_BFD                 0x00000020                      // broadcast frames disable
  #define ETH_MACFFR_PCF_NO_CTRL         0x00000000                      // pass no control frames
  #define ETH_MACFFR_PCF_CTRL_NO_PAUSE   0x00000040                      // pass all control frames except pause control frames
  #define ETH_MACFFR_PCF_ALL             0x00000080                      // pass all control frames
  #define ETH_MACFFR_PCF_MATCHING        0x000000c0                      // pass control frames that pass the address filter
  #define ETH_MACFFR_SAIF                0x00000100                      // source address inverse filtering
  #define ETH_MACFFR_SAF                 0x00000200                      // source address filter
  #define ETH_MACFFR_HPF                 0x00000400                      // hash or perfect filer
  #define ETH_MACFFR_RA                  0x80000000                      // receive all
#define ETH_MACHTHR                      *(unsigned long *)(ENET_BLOCK + 0x0008)           // hash table high register
#define ETH_MACHTLR                      *(unsigned long *)(ENET_BLOCK + 0x000c)           // hash table low register
#define ETH_MACMIIAR                     *(volatile unsigned long *)(ENET_BLOCK + 0x0010)  // MII address register
  #define ETH_MACMIIAR_MB                0x00000001                      // MII busy
  #define ETH_MACMIIAR_MW                0x00000002                      // MII write
 #if defined _STM32F2XX || defined _STM32F4XX || defined _STM32F7XX
  #define ETH_MACMIIAR_HCLK42            0x00000000                      // MDC clock is HCLK/42 - used when HCLK > 60MHz and <= 100MHz
  #define ETH_MACMIIAR_HCLK62            0x00000004                      // MDC clock is HCLK/62 - used when HCLK = 100MHz
  #define ETH_MACMIIAR_HCLK16            0x00000008                      // MDC clock is HCLK/16 - used when HCLK < 35MHz
  #define ETH_MACMIIAR_HCLK26            0x0000000c                      // MDC clock is HCLK/26 - used when HCLK >= 35MHz and <= 60MHz
 #else
  #define ETH_MACMIIAR_HCLK42            0x00000000                      // MDC clock is HCLK/42 - used when HCLK >= 60MHz
  #define ETH_MACMIIAR_HCLK16            0x00000004                      // MDC clock is HCLK/16 - used when HCLK < 35MHz
  #define ETH_MACMIIAR_HCLK26            0x00000008                      // MDC clock is HCLK/26 - used when HCLK >= 35MHz and < 60MHz
  #define ETH_MACMIIAR_HCLK62            ETH_MACMIIAR_HCLK42             // for compatibility
 #endif
  #define ETH_MACMIIAR_MR_SHIFT          6
  #define ETH_MACMIIAR_MR_MASK           0x000007c0                      // MII register mask
  #define ETH_MACMIIAR_PA_SHIFT          11
  #define ETH_MACMIIAR_PA_MASK           0x0000f800                      // PHY address mask
#define ETH_MACMIIDR                     *(volatile unsigned long *)(ENET_BLOCK + 0x0014)  // MII data register
#define ETH_MACFCR                       *(volatile unsigned long *)(ENET_BLOCK + 0x0018)  // EMAC flow control register
  #define ETH_MACFCR_FCB_BPA             0x00000001                      // flow control busy/back pressure active
  #define ETH_MACFCR_TFCE                0x00000002                      // transmit flow control enable
  #define ETH_MACFCR_RFCE                0x00000004                      // receive flow control enable
  #define ETH_MACFCR_UPFD                0x00000008                      // unicast pause frame detect
  #define ETH_MACFCR_PLT_4               0x00000000                      // pause low threshold = paus time minus 4 slot times
  #define ETH_MACFCR_PLT_28              0x00000010                      // pause low threshold = paus time minus 28 slot times
  #define ETH_MACFCR_PLT_144             0x00000020                      // pause low threshold = paus time minus 144 slot times
  #define ETH_MACFCR_PLT_256             0x00000030                      // pause low threshold = paus time minus 256 slot times
  #define ETH_MACFCR_ZQPD                0x00000080                      // zero-quanta pause disable
  #define ETH_MACFCR_PAUSE_TIME_MASK     0xffff0000
#define ETH_MACVLANTR                    *(volatile unsigned long *)(ENET_BLOCK + 0x001c)  // 
#define ETH_MACRWUFFR                    *(volatile unsigned long *)(ENET_BLOCK + 0x0028)  // 
#define ETH_MACPMTCSR                    *(volatile unsigned long *)(ENET_BLOCK + 0x002c)  // 
#define ETH_MACSR                        *(volatile unsigned long *)(ENET_BLOCK + 0x0038)  // 
#define ETH_MACIMR                       *(volatile unsigned long *)(ENET_BLOCK + 0x003c)  // 
#define ETH_MACA0HR                      *(volatile unsigned long *)(ENET_BLOCK + 0x0040)  // 
#define ETH_MACA0LR                      *(volatile unsigned long *)(ENET_BLOCK + 0x0044)  // 
#define ETH_MACA1HR                      *(volatile unsigned long *)(ENET_BLOCK + 0x0048)  // 
#define ETH_MACA1LR                      *(volatile unsigned long *)(ENET_BLOCK + 0x004c)  // 
#define ETH_MACA2HR                      *(volatile unsigned long *)(ENET_BLOCK + 0x0050)  // 
#define ETH_MACA2LR                      *(volatile unsigned long *)(ENET_BLOCK + 0x0054)  // 
#define ETH_MACA3HR                      *(volatile unsigned long *)(ENET_BLOCK + 0x0058)  // 
#define ETH_MACA3LR                      *(volatile unsigned long *)(ENET_BLOCK + 0x005c)  // 
#define ETH_MMCCR                        *(volatile unsigned long *)(ENET_BLOCK + 0x0100)  // 
#define ETH_MMCRIR                       *(volatile unsigned long *)(ENET_BLOCK + 0x0104)  // 
#define ETH_MMCTIR                       *(volatile unsigned long *)(ENET_BLOCK + 0x0108)  // 
#define ETH_MMCRIMR                      *(volatile unsigned long *)(ENET_BLOCK + 0x010c)  // 
#define ETH_MMCTIMR                      *(volatile unsigned long *)(ENET_BLOCK + 0x0110)  // 
#define ETH_MMCTGFSCCR                   *(volatile unsigned long *)(ENET_BLOCK + 0x014c)  // 
#define ETH_MMCTGFMSCCR                  *(volatile unsigned long *)(ENET_BLOCK + 0x0150)  // 
#define ETH_MMCTGFCR                     *(volatile unsigned long *)(ENET_BLOCK + 0x0168)  // 
#define ETH_MMCRFCECR                    *(volatile unsigned long *)(ENET_BLOCK + 0x0194)  // 
#define ETH_MMCRFAECR                    *(volatile unsigned long *)(ENET_BLOCK + 0x0198)  // 
#define ETH_MMCRGUFCR                    *(volatile unsigned long *)(ENET_BLOCK + 0x01c4)  // 
#define ETH_PTPTSCR                      *(volatile unsigned long *)(ENET_BLOCK + 0x0700)  // 
#define ETH_PTPSSIR                      *(volatile unsigned long *)(ENET_BLOCK + 0x0704)  // 
#define ETH_PTPTSHR                      *(volatile unsigned long *)(ENET_BLOCK + 0x0708)  // 
#define ETH_PTPTSLR                      *(volatile unsigned long *)(ENET_BLOCK + 0x070c)  // 
#define ETH_PTPTSHUR                     *(volatile unsigned long *)(ENET_BLOCK + 0x0710)  // 
#define ETH_PTPTSLUR                     *(volatile unsigned long *)(ENET_BLOCK + 0x0714)  // 
#define ETH_PTPTSAR                      *(volatile unsigned long *)(ENET_BLOCK + 0x0718)  // 
#define ETH_PTPTTHR                      *(volatile unsigned long *)(ENET_BLOCK + 0x071c)  // 
#define ETH_PTPTTLR                      *(volatile unsigned long *)(ENET_BLOCK + 0x0720)  // 
#define ETH_DMABMR                       *(volatile unsigned long *)(ENET_BLOCK + 0x1000)  // DMA Bus Mode Register
  #define ETH_DMABMR_SR                  0x00000001                                        // software reset - self resetting
  #define ETH_DMABMR_DA                  0x00000002                                        // DMA arbitration - RX priority
  #define ETH_DMABMR_DSL_MASK            0x0000007c                                        // descriptor skip length
  #define ETH_DMABMR_PBL_MASK            0x00003f00                                        // programmable burst length mask
  #define ETH_DMABMR_PBL_1_BEAT          0x00000100
  #define ETH_DMABMR_RTPR_MASK           0x0000c000                                        // rx/tx priority ration mask
  #define ETH_DMABMR_FB                  0x00010000                                        // fixed burst
  #define ETH_DMABMR_RDP_MASK            0x007e0000                                        // rx DMA programmable burst length mask
  #define ETH_DMABMR_RDP_1_BEAT          0x00020000
  #define ETH_DMABMR_USP                 0x00800000                                        // use separate programmable burst length
  #define ETH_DMABMR_FPM                 0x01000000                                        // 4x programmable burst length mode
  #define ETH_DMABMR_AAB                 0x02000000                                        // address aligned beats
#define ETH_DMATPDR                      *(volatile unsigned long *)(ENET_BLOCK + 0x1004)  // 
#define ETH_DMARPDR                      *(volatile unsigned long *)(ENET_BLOCK + 0x1008)  // 
#define ETH_DMARDLAR                     *(volatile unsigned long *)(ENET_BLOCK + 0x100c)  // DMA receive descriptor list address register
#define ETH_DMATDLAR                     *(volatile unsigned long *)(ENET_BLOCK + 0x1010)  // DMA transmit descriptor list address register
#define ETH_DMASR                        *(volatile unsigned long *)(ENET_BLOCK + 0x1014)  // Ethernet DMA status register
#define ETH_DMAOMR                       *(volatile unsigned long *)(ENET_BLOCK + 0x1018)  // DMA Operation Mode Register
  #define ETH_DMAOMR_SR                  0x00000002                                        // start reception monitoring
  #define ETH_DMAOMR_OSF                 0x00000004                                        // operate on second frame
  #define ETH_DMAOMR_RTC_MASK            0x00000018                                        // receive threshold control mask
  #define ETH_DMAOMR_FUGF                0x00000040                                        // forward undersized good frames
  #define ETH_DMAOMR_FEF                 0x00000080                                        // forward error frames
  #define ETH_DMAOMR_ST                  0x00002000                                        // start transmission monitoring
  #define ETH_DMAOMR_TTC_MASK            0x0001c000                                        // transmit threshold control mask
  #define ETH_DMAOMR_FTF                 0x00100000                                        // flush transmit FIFO
  #define ETH_DMAOMR_TSF                 0x00200000                                        // transmit store and forward
  #define ETH_DMAOMR_DFRF                0x01000000                                        // disable flushing of recieved frames
  #define ETH_DMAOMR_RSF                 0x02000000                                        // receive store and forward
  #define ETH_DMAOMR_DTCEFD              0x04000000                                        // dropping of TCP/IP checksum error frames disabled
#define ETH_DMAIER                       *(volatile unsigned long *)(ENET_BLOCK + 0x101c)  // DMA interrupt enable register
  #define ETH_DMAIER_TIE                 0x00000001                      // transmit interrupt enable
  #define ETH_DMAIER_TPSIE               0x00000002                      // transmit process stopped interrupt enable
  #define ETH_DMAIER_TBUIE               0x00000004                      // transmit buffer unavailable interrupt enable
  #define ETH_DMAIER_TJTIE               0x00000008                      // transmit jabber timeout interrupt enable
  #define ETH_DMAIER_ROTIE               0x00000010                      // receive overflow interrupt enable
  #define ETH_DMAIER_TUIE                0x00000020                      // transmit underflow interrupt enable
  #define ETH_DMAIER_RIE                 0x00000040                      // receive interrupt enable
  #define ETH_DMAIER_RBUIE               0x00000080                      // receive buffer unavailable interrupt enable
  #define ETH_DMAIER_RPSIE               0x00000100                      // receive process stopped interrupt enable
  #define ETH_DMAIER_RWTIE               0x00000200                      // receive watchdog timeout interrupt enable
  #define ETH_DMAIER_ETIE                0x00000400                      // early transmit interrupt enable
  #define ETH_DMAIER_FBEIE               0x00002000                      // fatal bus error interrupt enable
  #define ETH_DMAIER_ERIE                0x00004000                      // early receive interrupt enable
  #define ETH_DMAIER_AISE                0x00008000                      // abnormal interrupt summary enable
  #define ETH_DMAIER_NISE                0x00010000                      // normal interrupt enable
#define ETH_DMAMFBOCR                    *(volatile unsigned long *)(ENET_BLOCK + 0x1020)  // 
#define ETH_DMACHTDR                     *(volatile unsigned long *)(ENET_BLOCK + 0x1048)  // 
#define ETH_DMACHRDR                     *(volatile unsigned long *)(ENET_BLOCK + 0x104c)  // 
#define ETH_DMACHTBAR                    *(volatile unsigned long *)(ENET_BLOCK + 0x1050)  // 
#define ETH_DMACHRBAR                    *(volatile unsigned long *)(ENET_BLOCK + 0x1054)  // 

// Ethernet controller - MAC802.3
//
typedef struct stSTM32_BD
{
    volatile unsigned long    bd_word0;
    volatile unsigned long    bd_word1;
    volatile unsigned char   *bd_dma_buf1_address;
    volatile void            *bd_dma_buf2_address;
} STM32_BD;


// RX word 0
//
#define RX_BD_WORD0_PCE                 0x00000001                       // payload checksum error (TCP/UDP/ICMP)
#define RX_BD_WORD0_CE                  0x00000002                       // CRC error
#define RX_BD_WORD0_DRE                 0x00000004                       // dribble bit error
#define RX_BD_WORD0_RE                  0x00000008                       // receive error
#define RX_BD_WORD0_RWT                 0x00000010                       // receive watchdog timeout
#define RX_BD_WORD0_FT                  0x00000020                       // frame type is Ethernet
#define RX_BD_WORD0_LCO                 0x00000040                       // late collision
#define RX_BD_WORD0_IPHCE               0x00000080                       // IPv4/6 header checksum error
#define RX_BD_WORD0_LS                  0x00000100                       // last descriptor
#define RX_BD_WORD0_FS                  0x00000200                       // first descriptor
#define RX_BD_WORD0_VLAN                0x00000400                       // VLAN tag
#define RX_BD_WORD0_OE                  0x00000800                       // overflow error
#define RX_BD_WORD0_LE                  0x00001000                       // length error
#define RX_BD_WORD0_SAF                 0x00002000                       // source address filter fail
#define RX_BD_WORD0_DE                  0x00004000                       // descriptor error
#define RX_BD_WORD0_ES                  0x00008000                       // error summary
#define RX_BD_WORD0_FL_SHIFT            16
#define RX_BD_WORD0_FL_MASK             0x3fff0000                       // frame length mask
#define RX_BD_WORD0_AFM                 0x40000000                       // destination address filter fail
#define RX_BD_WORD0_OWN                 0x80000000                       // own bit

// RX word 1
//
#define RX_BD_WORD1_RBS1_MASK           0x00001fff                       // receive buffer size 1 mask
#define RX_BD_WORD1_RCH                 0x00004000                       // second address chained
#define RX_BD_WORD1_RER                 0x00008000                       // receive end of ring
#define RX_BD_WORD1_RBS2_SHIFT          16
#define RX_BD_WORD1_RBS2_MASK           0x1fff0000                       // receive buffer size 2 mask
#define RX_BD_WORD1_DIC                 0x80000000                       // disable interrupt on completion


// TX word 0
//
#define TX_BD_WORD0_DB                  0x00000001                       // deferred bit
#define TX_BD_WORD0_UF                  0x00000002                       // underflow error
#define TX_BD_WORD0_ED                  0x00000004                       // excessive deferral
#define TX_BD_WORD0_CC_MASK             0x00000078                       // collision count mask
#define TX_BD_WORD0_VF                  0x00000080                       // VLAN frame
#define TX_BD_WORD0_EC                  0x00000100                       // excessive collision
#define TX_BD_WORD0_LCO                 0x00000200                       // late collision
#define TX_BD_WORD0_NC                  0x00000400                       // no carrier
#define TX_BD_WORD0_LCA                 0x00000800                       // loss of carrier
#define TX_BD_WORD0_IPE                 0x00001000                       // IP payload error
#define TX_BD_WORD0_FF                  0x00002000                       // frame flushed
#define TX_BD_WORD0_JT                  0x00004000                       // jabber timeout
#define TX_BD_WORD0_ES                  0x00008000                       // error summary
#define TX_BD_WORD0_IHE                 0x00010000                       // IP header error
#define TX_BD_WORD0_TTSS                0x00020000                       // transmit time stamp status
#define TX_BD_WORD0_TCH                 0x00100000                       // second address chained
#define TX_BD_WORD0_TER                 0x00200000                       // transmit end of ring
#define TX_BD_WORD0_CIC_DISABLED        0x00000000                       // checksum insertion control disabled
#define TX_BD_WORD0_CIC_IP_HEAD         0x00400000                       // checksum insertion control - only IP header checksum inserted
#define TX_BD_WORD0_CIC_IP_HEAD_PAYLOAD 0x00800000                       // checksum insertion control - both IP header and payload checksums inserted
#define TX_BD_WORD0_CIC_IP_ALL          0x00c00000                       // checksum insertion control - both IP header, pseudo-header and payload checksums inserted
#define TX_BD_WORD0_TTSE                0x02000000                       // transmit time stamp enabled
#define TX_BD_WORD0_DP                  0x04000000                       // disable pad
#define TX_BD_WORD0_DC                  0x08000000                       // disable CRC
#define TX_BD_WORD0_FS                  0x10000000                       // first segment
#define TX_BD_WORD0_LS                  0x20000000                       // last segment
#define TX_BD_WORD0_IC                  0x40000000                       // interrupt on completion
#define TX_BD_WORD0_OWN                 0x80000000                       // own bit

// TX word 1
//
#define TX_BD_WORD1_TBS1_MASK           0x00001fff                       // transmit buffer size 1 mask
#define TX_BD_WORD1_TBS2_SHIFT          16
#define TX_BD_WORD1_TBS2_MASK           0x1fff0000                       // transmit buffer size 2 mask

#if HCLK > 100000000                                                     // {18} set MDC speed accordingly
    #define READ_FROM_PHY               (ETH_MACMIIAR_HCLK62 | ETH_MACMIIAR_MB | (PHY_ADDRESS_ << ETH_MACMIIAR_PA_SHIFT))
    #define WRITE_TO_PHY                (ETH_MACMIIAR_HCLK62 | ETH_MACMIIAR_MW | ETH_MACMIIAR_MB | (PHY_ADDRESS_ << ETH_MACMIIAR_PA_SHIFT))
#elif HCLK > 60000000
    #define READ_FROM_PHY               (ETH_MACMIIAR_HCLK42 | ETH_MACMIIAR_MB | (PHY_ADDRESS_ << ETH_MACMIIAR_PA_SHIFT))
    #define WRITE_TO_PHY                (ETH_MACMIIAR_HCLK42 | ETH_MACMIIAR_MW | ETH_MACMIIAR_MB | (PHY_ADDRESS_ << ETH_MACMIIAR_PA_SHIFT))
#elif HCLK < 35000000 
    #define READ_FROM_PHY               (ETH_MACMIIAR_HCLK16 | ETH_MACMIIAR_MB | (PHY_ADDRESS_ << ETH_MACMIIAR_PA_SHIFT))
    #define WRITE_TO_PHY                (ETH_MACMIIAR_HCLK16 | ETH_MACMIIAR_MW | ETH_MACMIIAR_MB | (PHY_ADDRESS_ << ETH_MACMIIAR_PA_SHIFT))
#else
    #define READ_FROM_PHY               (ETH_MACMIIAR_HCLK26 | ETH_MACMIIAR_MB | (PHY_ADDRESS_ << ETH_MACMIIAR_PA_SHIFT))
    #define WRITE_TO_PHY                (ETH_MACMIIAR_HCLK26 | ETH_MACMIIAR_MW | ETH_MACMIIAR_MB | (PHY_ADDRESS_ << ETH_MACMIIAR_PA_SHIFT))
#endif



// PHYSICAL layer device
//
#define GENERIC_BASIC_CONTROL_REGISTER                 (0x0 << ETH_MACMIIAR_MR_SHIFT)
  #define PHY_DISABLE_TRANSMITTER                     0x0001
  #define PHY_ENABLE_COLLISION_TEST                   0x0080
  #define PHY_FULL_DUPLEX_MODE                        0x0100
  #define PHY_RESTART_AUTO_NEGOTIATION                0x0200
  #define PHY_ELECTRICALLY_ISOLATE                    0x0400
  #define PHY_POWER_DOWN_MODE                         0x0800
  #define PHY_ENABLE_AUTO_NEGOTIATION                 0x1000
  #define PHY_SELECT_100M_SPEED                       0x2000
  #define PHY_LOOP_BACK                               0x4000
  #define PHY_SOFTWARE_RESET                          0x8000
#define GENERIC_BASIC_STATUS_REGISTER                  (0x1 << ETH_MACMIIAR_MR_SHIFT)
  #define PHY_SUPPORT_EXTENDED_CAPABILITIES           0x0001
  #define PHY_JABBER_DETECTED                         0x0002
  #define PHY_LINK_IS_UP                              0x0004
  #define PHY_AUTO_NEGOTIATION_CAPABLE                0x0008
  #define PHY_REMOTE_FAULT_DETECTED                   0x0010
  #define PHY_AUTO_NEGOTIATION_COMPLETE               0x0020
  #define PHY_NO_PREAMBLE                             0x0040
  #define PHY_10MEG_HALF_DUPLEX                       0x0800
  #define PHY_10MEG_FULL_DUPLEX                       0x1000
  #define PHY_100MEG_HALF_DUPLEX                      0x2000
  #define PHY_100MEG_FULL_DUPLEX                      0x4000
  #define PHY_100MEG_T4_CAPABLE                       0x8000
#define GENERIC_PHY_IDENTIFIER_1                       (0x2 << ETH_MACMIIAR_MR_SHIFT)
#define GENERIC_PHY_IDENTIFIER_2                       (0x3 << ETH_MACMIIAR_MR_SHIFT)
  #define PHY_MASK                                    0xfffffff0                           // mask out the revision number
#define GENERIC_AUTO_NEG_ADVERT_REG                    (0x4 << ETH_MACMIIAR_MR_SHIFT)
#define GENERIC_AUTO_NEG_LINK_PARTNER_ABIL_REG         (0x5 << ETH_MACMIIAR_MR_SHIFT)
#define GENERIC_AUTO_NEG_EXPANSION_REG                 (0x6 << ETH_MACMIIAR_MR_SHIFT)
#define GENERIC_AUTO_NEG_NEXT_PAGE_REG                 (0x7 << ETH_MACMIIAR_MR_SHIFT)

#define DP83848_PHY_STATUS_REGISTER                    (0x10 << ETH_MACMIIAR_MR_SHIFT)
  #define VALID_LINK_ESTABLISHED                      0x0001
  #define SPEED_STATUS_10M                            0x0002
  #define DUPLEX_STATUS_FULL_DUPLEX                   0x0004
#define DP83848_MII_INTERRUPT_CONTROL_REGISTER         (0x11 << ETH_MACMIIAR_MR_SHIFT)
  #define DP83848_INT_OE                              0x0001
  #define DP83848_INT_EN                              0x0002
#define DP83848_MII_INTERRUPT_STATUS_REGISTER          (0x12 << ETH_MACMIIAR_MR_SHIFT)
  #define DP83848_ANC_INT_EN                          0x0004
  #define DP83848_LINK_INT_EN                         0x0020
  #define DP83848_ANC_INT                             0x0400
  #define DP83848_LINK_INT                            0x2000
#define DP83848_FALSE_CARRIER_SENSE COUNTER_REGISTER   (0x14 << ETH_MACMIIAR_MR_SHIFT)
#define DP83848_RXER_COUNTER_REGISTER                  (0x15 << ETH_MACMIIAR_MR_SHIFT)
#define DP83848_PCS_CONFIG_STATUS_REGISTER             (0x16 << ETH_MACMIIAR_MR_SHIFT)
#define DP83848_RMII_AND_BYPASS_REGISTER               (0x17 << ETH_MACMIIAR_MR_SHIFT)
#define DP83848_LED_DIRECT_CONTROL_REGISTER            (0x18 << ETH_MACMIIAR_MR_SHIFT)
#define DP83848_PHY_CONTROL_REGISTER                   (0x19 << ETH_MACMIIAR_MR_SHIFT)
#define DP83848_10BASET_STATUS_CONTROL_REGISTER        (0x1a << ETH_MACMIIAR_MR_SHIFT)
#define DP83848_CD_TEST_CONTROL_REGISTER               (0x1b << ETH_MACMIIAR_MR_SHIFT)
#define DP83848_ENERGY_DETECT_CONTROL_REGISTER         (0x1d << ETH_MACMIIAR_MR_SHIFT)

// KS8721 registers
//
#define KS8721_INTERRUPT_CONTROL_STATUS_REGISTER      (0x1b << ETH_MACMIIAR_MR_SHIFT)
  #define PHY_LINK_UP_INT_ENABLE                      0x0100
  #define PHY_REMOTE_FAULT_INT_ENABLE                 0x0200
  #define PHY_LINK_DOWN_INT_ENABLE                    0x0400
  #define PHY_LINK_PARTNER_ACK_INT_ENABLE             0x0800
  #define PHY_PARALLEL_DETECT_FAULT_INT_ENABLE        0x1000
  #define PHY_PAGE_RECEIVED_INT_ENABLE                0x2000
  #define PHY_RECEIVE_ERROR_INT_ENABLE                0x4000
  #define PHY_JABBER_INT_ENABLE                       0x8000
  #define PHY_LINK_UP_INT                             0x0001
  #define PHY_REMOTE_FAULT_INT                        0x0002
  #define PHY_LINK_DOWN_INT                           0x0004
  #define PHY_LINK_PARTNER_ACK_INT                    0x0008
  #define PHY_PARALLEL_DETECT_FAULT_INT               0x0010
  #define PHY_PAGE_RECEIVED_INT                       0x0020
  #define PHY_RECEIVE_ERROR_INT                       0x0040
  #define PHY_JABBER_INT                              0x0080

#define KS8721_100BASETX_PHY_CONTROL_REGISTER         (0x1f << ETH_MACMIIAR_MR_SHIFT)
  #define PHY_DISABLE_SCRAMBLER                       0x0001
  #define PHY_ENABLE_SQE_TEST                         0x0002
  #define PHY_OP_MODE_MASK                            0x001c
  #define PHY_OP_MODE_STILL_AUTONEGOTIATING           0x0000
  #define PHY_OP_MODE_10_BASE_T_HALF_DUPLEX           0x0004
  #define PHY_OP_MODE_100_BASE_T_HALF_DUPLEX          0x0008
  #define PHY_OP_MODE_FULL_DUPLEX                     0x0010
  #define PHY_OP_MODE_10_BASE_T_FULL_DUPLEX           0x0014
  #define PHY_OP_MODE_100_BASE_T_FULL_DUPLEX          0x0018
  #define PHY_OP_MODE_ISOLATED                        0x001c
  #define PHY_IN_ISOLATE_MODE                         0x0020
  #define PHY_FLOW_CONTROL_CAPABLE                    0x0040
  #define PHY_AUTONEGOTIATION_COMPLETE                0x0080
  #define PHY_ENABLE_JABBER_COUNTER                   0x0100
  #define PHY_INTERRUPT_PIN_ACTIVE_HIGH               0x0200
  #define PHY_ENABLE_POWER_SAVING                     0x0400
  #define PHY_FORCE_LINK_PASS                         0x0800
  #define PHY_SIGNAL_PRESENCE_DETECTED                0x1000
  #define PHY_DISABLE_MDI_MDIX                        0x2000

// ST802RT1 registers
//
#define ST802RT1_PHY_STATUS_REGISTER                  (0x11 << ETH_MACMIIAR_MR_SHIFT)
  #define ST802RT1_LINK_DOWN_INTERRUPT                0x0010             // link status changed to fail interrupt pending
  #define ST802RT1_AUTO_NEG_COMPLETE                  0x0040             // auto-negotiation completed interrupt pending
  #define ST802RT1_FULL_DUPLEX                        0x0100             // link is duplex
  #define ST802RT1_SPEED_100M                         0x0200             // speed is 100M

#define ST802RT1_INTERRUPT_CONTROL_REGISTER           (0x12 << ETH_MACMIIAR_MR_SHIFT)
  #define ST802RT1_LK_DWN_EN                          0x0010             // link down interrupt enable
  #define ST802RT1_AN_CMPL_EN                         0x0040             // autonegotiation completed interrupt enable
  #define ST802RT1_INT_EN                             0x0080             // interrupt events enabled
  #define ST802RT1_INT_OE_N                           0x0100             // power down pin input when '1' or interrupt output (open-drain) when '0'



// Reset and clock control
//
#if defined _STM32F2XX || defined _STM32F4XX || defined _STM32F7XX
    #define RCC_CR                           *(volatile unsigned long *)(RCC_BLOCK + 0x00) // clock control register
      #define RCC_CR_HSION                   0x00000001                  // internal high speed clock enable (set by default)
      #define RCC_CR_HSIRDY                  0x00000002                  // internal high speed clock read (read-only)
      #define RCC_CR_HSITRIM_MASK            0x000000f8                  // internal high speed clock trip (0x80 after reset)
      #define RCC_CR_HSICAL_MASK             0x0000ff00                  // internal high speed clock calibration (read-only)
      #define RCC_CR_HSEON                   0x00010000                  // HSE - high speed external clock enable
      #define RCC_CR_HSERDY                  0x00020000                  // high speed external clock (read-only)
      #define RCC_CR_HSEBYP                  0x00040000                  // HSE clock bypass - for external clock input
      #define RCC_CR_CSSON                   0x00080000                  // clock security system enable
      #define RCC_CR_PLLON                   0x01000000                  // main phase lock loop enable
      #define RCC_CR_PLLRDY                  0x02000000                  // main phase lock loop ready flag (read-only)
      #if defined _STM32F7XX || defined _STM32F427 || defined _STM32F429
        #define RCC_CR_PLLI2SON              0x04000000                  // PLL2 enable
        #define RCC_CR_PLLI2SRDY             0x08000000                  // PLL2 ready flag (read-only)
        #define RCC_CR_PLLSAION              0x10000000                  // phase lock loop SAI enable
        #define RCC_CR_PLLSAIRDY             0x20000000                  // phase lock loop SAI enable ready flag (read-only)
      #endif
    #define RCC_PLLCFGR                      *(volatile unsigned long *)(RCC_BLOCK + 0x04) // PLL configuration register
        #define RCC_PLLCFGR_M_MASK           0x0000003f
        #define RCC_PLLCFGR_N_MASK           0x00007fc0
        #define RCC_PLLCFGR_P_MASK           0x00030000
        #define RCC_PLLCFGR_PLLSRC_HSE       0x00400000                  // high speed clock used as PLL source (clock input or crystal)
        #define RCC_PLLCFGR_PLLSRC_HSI       0x00000000                  // internal 16MHz RC used as PLL source
        #define RCC_PLLCFGR_Q_MASK           0x0f000000
        #define RCC_PLLCFGR_RESET_VALUE      0x24003010
    #define RCC_CFGR                         *(volatile unsigned long *)(RCC_BLOCK + 0x08) // clock configuration register
      #define RCC_CFGR_HSI_SELECT            0x00000000                  // HSI selected as system clock
      #define RCC_CFGR_HSE_SELECT            0x00000001                  // HSE selected as system clock
      #define RCC_CFGR_PLL_SELECT            0x00000002                  // PLL selected as system clock
      #define RCC_CFGR_SYSCLK_MASK           0x00000003
      #define RCC_CFGR_HSI_USED              0x00000000                  // HSI used as system clock
      #define RCC_CFGR_HSE_USED              0x00000004                  // HSE used as system clock
      #define RCC_CFGR_PLL_USED              0x00000008                  // PLL used as system clock
      #define RCC_CFGR_SWS_MASK              0x0000000c                  // read-only
      #define RCC_CFGR_HPRE_SYSCLK           0x00000000                  // HCLK speed
      #define RCC_CFGR_HPRE_SYSCLK_DIV2      0x00000080
      #define RCC_CFGR_HPRE_SYSCLK_DIV4      0x00000090
      #define RCC_CFGR_HPRE_SYSCLK_DIV8      0x000000a0
      #define RCC_CFGR_HPRE_SYSCLK_DIV16     0x000000b0
      #define RCC_CFGR_HPRE_SYSCLK_DIV64     0x000000c0
      #define RCC_CFGR_HPRE_SYSCLK_DIV128    0x000000d0
      #define RCC_CFGR_HPRE_SYSCLK_DIV256    0x000000e0
      #define RCC_CFGR_HPRE_SYSCLK_DIV512    0x000000f0
      #define RCC_CFGR_PPRE1_HCLK            0x00000000                  // PCLK1 speed
      #define RCC_CFGR_PPRE1_HCLK_DIV2       0x00001000
      #define RCC_CFGR_PPRE1_HCLK_DIV4       0x00001400
      #define RCC_CFGR_PPRE1_HCLK_DIV8       0x00001800
      #define RCC_CFGR_PPRE1_HCLK_DIV16      0x00001c00
      #define RCC_CFGR_PPRE2_HCLK            0x00000000                  // PCLK2 speed
      #define RCC_CFGR_PPRE2_HCLK_DIV2       0x00008000
      #define RCC_CFGR_PPRE2_HCLK_DIV4       0x0000a000
      #define RCC_CFGR_PPRE2_HCLK_DIV8       0x0000c000
      #define RCC_CFGR_PPRE2_HCLK_DIV16      0x0000e000
      #define RCC_CFGR_RTC_HCLK              0x00000000
      #define RCC_CFGR_RTC_NO_CLOCK          0x00000000
      #define RCC_CFGR_RTC_HSE_DIV2          0x00020000
      #define RCC_CFGR_RTC_HSE_DIV3          0x00030000
      #define RCC_CFGR_RTC_HSE_DIV4          0x00040000
      #define RCC_CFGR_RTC_HSE_DIV5          0x00050000
      #define RCC_CFGR_RTC_HSE_DIV6          0x00060000
      #define RCC_CFGR_RTC_HSE_DIV7          0x00070000
      #define RCC_CFGR_RTC_HSE_DIV8          0x00080000
      #define RCC_CFGR_RTC_HSE_DIV9          0x00090000
      #define RCC_CFGR_RTC_HSE_DIV10         0x000a0000
      #define RCC_CFGR_RTC_HSE_DIV11         0x000b0000
      #define RCC_CFGR_RTC_HSE_DIV12         0x000c0000
      #define RCC_CFGR_RTC_HSE_DIV13         0x000d0000
      #define RCC_CFGR_RTC_HSE_DIV14         0x000e0000
      #define RCC_CFGR_RTC_HSE_DIV15         0x000f0000
      #define RCC_CFGR_RTC_HSE_DIV16         0x00100000
      #define RCC_CFGR_RTC_HSE_DIV17         0x00110000
      #define RCC_CFGR_RTC_HSE_DIV18         0x00120000
      #define RCC_CFGR_RTC_HSE_DIV19         0x00130000
      #define RCC_CFGR_RTC_HSE_DIV20         0x00140000
      #define RCC_CFGR_RTC_HSE_DIV21         0x00150000
      #define RCC_CFGR_RTC_HSE_DIV22         0x00160000
      #define RCC_CFGR_RTC_HSE_DIV23         0x00170000
      #define RCC_CFGR_RTC_HSE_DIV24         0x00180000
      #define RCC_CFGR_RTC_HSE_DIV25         0x00190000
      #define RCC_CFGR_RTC_HSE_DIV26         0x001a0000
      #define RCC_CFGR_RTC_HSE_DIV27         0x001b0000
      #define RCC_CFGR_RTC_HSE_DIV28         0x001c0000
      #define RCC_CFGR_RTC_HSE_DIV29         0x001d0000
      #define RCC_CFGR_RTC_HSE_DIV30         0x001e0000
      #define RCC_CFGR_RTC_HSE_DIV31         0x001f0000
      #define RCC_CFGR_MCO1_HSI              0x00000000                  // select HSI as MCO1 clock
      #define RCC_CFGR_MCO1_LSE              0x00200000                  // select LSE as MCO1 clock
      #define RCC_CFGR_MCO1_XT1              0x00400000                  // select HSE as MCO1 clock (XT1 used for F1 compatibility)
      #define RCC_CFGR_MCO1_PLL              0x00600000                  // select PLL as MCO1 clock
      #define RCC_CFGR_I2SSCR                0x00800000
      #define RCC_CFGR_MCO1_PRE              0x07000000
      #define RCC_CFGR_MCO2_PRE              0x38000000
      #define RCC_CFGR_MCO2                  0xc0000000
    #define RCC_CIR                          *(volatile unsigned long *)(RCC_BLOCK + 0x0c) // clock interrupt register
    #define RCC_AHB1RSTR                     *(volatile unsigned long *)(RCC_BLOCK + 0x10) // AHB1 peripheral reset register
      #define RCC_AHB1RSTR_GPIOARST          0x00000001                  // {19}
      #define RCC_AHB1RSTR_GPIOBRST          0x00000002
      #define RCC_AHB1RSTR_GPIOCSRT          0x00000004
      #define RCC_AHB1RSTR_GPIODSRT          0x00000008
      #define RCC_AHB1RSTR_GPIOERST          0x00000010
      #define RCC_AHB1RSTR_GPIOFRST          0x00000020
      #define RCC_AHB1RSTR_GPIOGRST          0x00000040
      #define RCC_AHB1RSTR_GPIOHRST          0x00000080
      #define RCC_AHB1RSTR_GPIOIRST          0x00000100
      #define RCC_AHB1RSTR_BKPSRAMRST        0x00040000
      #define RCC_AHB1RSTR_CCMDATARAMRST     0x00100000
      #define RCC_AHB1RSTR_DMA1RST           0x00200000
      #define RCC_AHB1RSTR_DMA2RST           0x00400000
      #define RCC_AHB1RSTR_ETHMACRST         0x02000000
      #define RCC_AHB1RSTR_ETHMACTXRST       0x04000000
      #define RCC_AHB1RSTR_ETHMACRXRST       0x08000000
      #define RCC_AHB1RSTR_ETHMACPTPRST      0x10000000
      #define RCC_AHB1RSTR_OTGHSRST          0x20000000
      #define RCC_AHB1RSTR_OTGHSULPIRST      0x40000000
    #define RCC_AHB2RSTR                     *(volatile unsigned long *)(RCC_BLOCK + 0x14) // AHB2 peripheral reset register
      #define RCC_APB2RSTR_DCMIRST           0x00000001                  // {19}
      #define RCC_APB2RSTR_CRYPRST           0x00000010
      #define RCC_APB2RSTR_HASHRST           0x00000020
      #define RCC_APB2RSTR_RNGRST            0x00000040
      #define RCC_APB2RSTR_OTGFSRST          0x00000080
    #define RCC_AHB3RSTR                     *(volatile unsigned long *)(RCC_BLOCK + 0x18) // AHB3 peripheral reset register
      #define RCC_APB3RSTR_FSMCRST           0x00000001                  // {19}
      #define RCC_APB3RSTR_QSPIRST           0x00000002
    #define RCC_APB1RSTR                     *(volatile unsigned long *)(RCC_BLOCK + 0x20) // APB1 peripheral reset register
      #define RCC_APB1RSTR_TIM2RST           0x00000001
      #define RCC_APB1RSTR_TIM3RST           0x00000002
      #define RCC_APB1RSTR_TIM4RST           0x00000004
      #define RCC_APB1RSTR_TIM5RST           0x00000008
      #define RCC_APB1RSTR_TIM6RST           0x00000010
      #define RCC_APB1RSTR_TIM7RST           0x00000020
      #define RCC_APB1RSTR_WWDGRST           0x00000800
      #define RCC_APB1RSTR_SPI2RST           0x00004000
      #define RCC_APB1RSTR_SPI3RST           0x00008000
      #define RCC_APB1RSTR_USART2RST         0x00020000
      #define RCC_APB1RSTR_USART3RST         0x00040000
      #define RCC_APB1RSTR_UART4RST          0x00080000
      #define RCC_APB1RSTR_UART5RST          0x00100000
      #define RCC_APB1RSTR_I2C1RST           0x00200000
      #define RCC_APB1RSTR_I2C2RST           0x00400000
      #define RCC_APB1RSTR_CAN1RST           0x02000000
      #define RCC_APB1RSTR_CAN2RST           0x04000000
      #define RCC_APB1RSTR_BKPRST            0x08000000
      #define RCC_APB1RSTR_PWRRST            0x10000000
      #define RCC_APB1RSTR_DACRST            0x20000000
    #define RCC_APB2RSTR                     *(volatile unsigned long *)(RCC_BLOCK + 0x24) // APB2 peripheral reset register
      #define RCC_APB2RSTR_TIM1RST           0x00000001                  // {19}
      #define RCC_APB2RSTR_TIM8RST           0x00000004
      #define RCC_APB2RSTR_USART1RST         0x00000010
      #define RCC_APB2RSTR_USART6RST         0x00000020
      #define RCC_APB2RSTR_ADC1RST           0x00000100
      #define RCC_APB2RSTR_ADC2RST           0x00000200
      #define RCC_APB2RSTR_ADC3RST           0x00000400
      #define RCC_APB2RSTR_SDIORST           0x00000800
      #define RCC_APB2RSTR_SPI1RST           0x00001000
      #define RCC_APB2RSTR_SYSCFGRST         0x00004000
      #define RCC_APB2RSTR_TIM9RST           0x00010000
      #define RCC_APB2RSTR_TIM10RST          0x00020000
      #define RCC_APB2RSTR_TIM20RST          0x00040000
    #define RCC_AHB1ENR                      *(volatile unsigned long *)(RCC_BLOCK + 0x30) // AHB1 peripheral clock enable register
      #define RCC_AHB1ENR_GPIOAEN            0x00000001
      #define RCC_AHB1ENR_GPIOBEN            0x00000002
      #define RCC_AHB1ENR_GPIOCEN            0x00000004
      #define RCC_AHB1ENR_GPIODEN            0x00000008
      #define RCC_AHB1ENR_GPIOEEN            0x00000010
      #define RCC_AHB1ENR_GPIOFEN            0x00000020
      #define RCC_AHB1ENR_GPIOGEN            0x00000040
      #define RCC_AHB1ENR_GPIOHEN            0x00000080
      #define RCC_AHB1ENR_GPIOIEN            0x00000100
      #if defined _STM32F7XX
        #define RCC_AHB1ENR_GPIOJEN          0x00000200
        #define RCC_AHB1ENR_GPIOKEN          0x00000400
        #define RCC_AHB1ENR_CRCEN            0x00001000
      #endif
      #define RCC_AHB1ENR_BKPSRAMEN          0x00040000
      #define RCC_AHB1ENR_CCMDATARAMEN       0x00100000
      #define RCC_AHB1ENR_DMA1EN             0x00200000
      #define RCC_AHB1ENR_DMA2EN             0x00400000
      #if defined _STM32F7XX
        #define RCC_AHB1ENR_DMA2DEN          0x00800000
      #endif
      #define RCC_AHB1ENR_ETHMACEN           0x02000000
      #define RCC_AHB1ENR_ETHMACTXEN         0x04000000
      #define RCC_AHB1ENR_ETHMACRXEN         0x08000000
      #define RCC_AHB1ENR_ETHMACPTPEN        0x10000000
      #define RCC_AHB1ENR_OTGHSEN            0x20000000
      #define RCC_AHB1ENR_OTGHSULPIEN        0x40000000
    #define RCC_AHB2ENR                      *(volatile unsigned long *)(RCC_BLOCK + 0x34) // AHB2 peripheral clock enable register
      #define RCC_AHB2ENR_DCMIEN             0x00000001
      #define RCC_AHB2ENR_CRYPEN             0x00000010
      #define RCC_AHB2ENR_HASHEN             0x00000020
      #define RCC_AHB2ENR_RNGEN              0x00000040
      #define RCC_AHB2ENR_OTGFSEN            0x00000080
    #define RCC_AHB3ENR                      *(volatile unsigned long *)(RCC_BLOCK + 0x38) // AHB3 peripheral clock enable register
      #define RCC_AHB3ENR_FSMCEN             0x00000001
      #define RCC_AHB3ENR_QSPIEN             0x00000002
    #define RCC_APB1ENR                      *(volatile unsigned long *)(RCC_BLOCK + 0x40) // APB1 peripheral clock enable register
      #define RCC_APB1ENR_TIM2EN             0x00000001
      #define RCC_APB1ENR_TIM3EN             0x00000002
      #define RCC_APB1ENR_TIM4EN             0x00000004
      #define RCC_APB1ENR_TIM5EN             0x00000008
      #define RCC_APB1ENR_TIM6EN             0x00000010
      #define RCC_APB1ENR_TIM7EN             0x00000020
      #define RCC_APB1ENR_TIM12EN            0x00000040
      #define RCC_APB1ENR_TIM13EN            0x00000080
      #define RCC_APB1ENR_TIM14EN            0x00000100
      #define RCC_APB1ENR_WWDGEN             0x00000800
      #define RCC_APB1ENR_SPI2EN             0x00004000
      #define RCC_APB1ENR_SPI3EN             0x00008000
      #define RCC_APB1ENR_USART2EN           0x00020000
      #define RCC_APB1ENR_USART3EN           0x00040000
      #define RCC_APB1ENR_UART4EN            0x00080000
      #define RCC_APB1ENR_UART5EN            0x00100000
      #define RCC_APB1ENR_I2C1EN             0x00200000
      #define RCC_APB1ENR_I2C2EN             0x00400000
      #define RCC_APB1ENR_I2C3EN             0x00800000
      #define RCC_APB1ENR_CAN1EN             0x02000000
      #define RCC_APB1ENR_CAN2EN             0x04000000
      #define RCC_APB1ENR_PWREN              0x10000000
      #define RCC_APB1ENR_DACEN              0x20000000
      #define RCC_APB1ENR_UART7EN            0x40000000
      #define RCC_APB1ENR_UART8EN            0x80000000
    #define RCC_APB2ENR                      *(volatile unsigned long *)(RCC_BLOCK + 0x44) // APB2 peripheral clock enable register
      #define RCC_APB2ENR_TIM1EN             0x00000001
      #define RCC_APB2ENR_TIM8EN             0x00000004
      #define RCC_APB2ENR_USART1EN           0x00000010
      #define RCC_APB2ENR_USART6EN           0x00000020
      #define RCC_APB2ENR_ADC1EN             0x00000100
      #define RCC_APB2ENR_ADC2EN             0x00000200
      #define RCC_APB2ENR_ADC3EN             0x00000400
      #define RCC_APB2ENR_SDIOEN             0x00000800
      #define RCC_APB2ENR_SPI1EN             0x00001000
      #define RCC_APB2ENR_SYSCFGEN           0x00004000
      #define RCC_APB2ENR_TIM9EN             0x00010000
      #define RCC_APB2ENR_TIM10EN            0x00020000
      #define RCC_APB2ENR_TIM11EN            0x00040000
    #define RCC_AHB1LPENR                    *(volatile unsigned long *)(RCC_BLOCK + 0x50) // AHB1 peripheral clock enabled in low power register
    #define RCC_AHB2LPENR                    *(volatile unsigned long *)(RCC_BLOCK + 0x54) // AHB2 peripheral clock enabled in low power register
    #define RCC_AHB3LPENR                    *(volatile unsigned long *)(RCC_BLOCK + 0x58) // AHB3 peripheral clock enabled in low power register

    #define RCC_APB1LPENR                    *(volatile unsigned long *)(RCC_BLOCK + 0x60) // APB1 peripheral clock enabled in low power register
    #define RCC_APB2LPENR                    *(volatile unsigned long *)(RCC_BLOCK + 0x64) // APB2 peripheral clock enabled in low power register

    #define RCC_BDCR                         *(volatile unsigned long *)(RCC_BLOCK + 0x70) // backup domain control register
      #define RCC_BDCR_LSEON                 0x00000001                  // external low speed oscillator enable
      #define RCC_BDCR_LSERDY                0x00000002                  // external low speed oscillator ready (read-only)
      #define RCC_BDCR_LSEBYP                0x00000004                  // external low speed oscillator bypassed
      #define RCC_BDCR_RTCSEC_MASK           0x00000300
      #define RCC_BDCR_RTCSEC_LSE            0x00000100                  // LSE oscillator clock used as the RTC clock
      #define RCC_BDCR_RTCSEC_LSI            0x00000200                  // LSI oscillator clock used as the RTC clock
      #define RCC_BDCR_RTCSEC_HSE            0x00000300                  // HSE oscillator clock divided by RTCPRE[4:0] in RCC:CFGR used as the RTC clock
      #define RCC_BDCR_RTCEN                 0x00008000                  // RTC enable
      #define RCC_BDCR_BDRST                 0x00010000                  // backup domain reset
    #define RCC_CSR                          *(volatile unsigned long *)(RCC_BLOCK + 0x74) // control status register
      #define RCC_CSR_LSION                  0x00000001                  // enable 40kHz RC oscillator
      #define RCC_CSR_LSIRDY                 0x00000002                  // read-only
      #define RCC_CSR_RMVF                   0x01000000                  // remove reset flag (write 1 to clear reset flags)
      #define RCC_CSR_PINRSTF                0x04000000                  // reset pin reset flag
      #define RCC_CSR_PORRSTF                0x08000000                  // power on reset flag
      #define RCC_CSR_SFTRSTF                0x10000000                  // software reset flag
      #define RCC_CSR_IWDGRSTF               0x20000000                  // independent watchdog reset flag
      #define RCC_CSR_WWDGRSTF               0x40000000                  // window watchdog reset flag
      #define RCC_CSR_LPWRRSTF               0x80000000                  // low power reset flag
      #define RESET_CAUSE_FLAGS              (RCC_CSR_RMVF | RCC_CSR_PINRSTF | RCC_CSR_PORRSTF | RCC_CSR_SFTRSTF | RCC_CSR_IWDGRSTF | RCC_CSR_WWDGRSTF | RCC_CSR_LPWRRSTF)
    #define RCC_SSCGR                        *(volatile unsigned long *)(RCC_BLOCK + 0x80) // spread spectrum clock generation register
    #define RCC_PLLI2SCFGR                   *(volatile unsigned long *)(RCC_BLOCK + 0x84) // PLLI2S configuration register
#elif defined _STM32L432 || defined _STM32L4X5 || defined _STM32L4X6
    #define RCC_CR                           *(volatile unsigned long *)(RCC_BLOCK + 0x00) // clock control register
      #define RCC_CR_MSION                   0x00000001
      #define RCC_CR_MSIRDY                  0x00000002                  // read-only
      #define RCC_CR_MSIPLLEN                0x00000004
      #define RCC_CR_MSIRGSEL                0x00000008                  // MSI clock range selection - controlled by value in this register (reset to zero on exit from standby) when set or by the value in RCC_CSR when cleared
      #define RCC_CR_MSIRANGE_100k           0x00000000                  // MSI frequency around 100kHz (MSI range should only be changed when off or ready - not when on AND not-ready)
      #define RCC_CR_MSIRANGE_200k           0x00000010                  // MSI frequency around 200kHz
      #define RCC_CR_MSIRANGE_400k           0x00000020                  // MSI frequency around 400kHz
      #define RCC_CR_MSIRANGE_800k           0x00000030                  // MSI frequency around 800kHz
      #define RCC_CR_MSIRANGE_1M             0x00000040                  // MSI frequency around 1M
      #define RCC_CR_MSIRANGE_2M             0x00000050                  // MSI frequency around 2M
      #define RCC_CR_MSIRANGE_4M             0x00000060                  // MSI frequency around 4M (default after reset)
      #define RCC_CR_MSIRANGE_8M             0x00000070                  // MSI frequency around 8M
      #define RCC_CR_MSIRANGE_16M            0x00000080                  // MSI frequency around 16M
      #define RCC_CR_MSIRANGE_24M            0x00000090                  // MSI frequency around 24M
      #define RCC_CR_MSIRANGE_32M            0x000000a0                  // MSI frequency around 32M
      #define RCC_CR_MSIRANGE_48M            0x000000b0                  // MSI frequency around 48M
      #define RCC_CR_MSIRANGE_MASK           0x000000f0
      #define RCC_CR_HSION                   0x00000100
      #define RCC_CR_HSIKERON                0x00000200
      #define RCC_CR_HSIRDY                  0x00000400                  // read-only
      #define RCC_CR_HSIASFS                 0x00000800
      #define RCC_CR_HSEON                   0x00010000
      #define RCC_CR_HSERDY                  0x00020000                  // read-only
      #define RCC_CR_HSEBYP                  0x00040000
      #define RCC_CR_CSSON                   0x00080000
      #define RCC_CR_PLLON                   0x01000000
      #define RCC_CR_PLLRDY                  0x02000000                  // read-only
      #define RCC_CR_PLLSAI1ON               0x04000000
      #define RCC_CR_PLLSAI1RDY              0x08000000                  // read-only
    #define RCC_ICSCR                        *(volatile unsigned long *)(RCC_BLOCK + 0x04)
    #define RCC_CFGR                         *(volatile unsigned long *)(RCC_BLOCK + 0x08) // clock configuration register
      #define RCC_CFGR_MSI_SELECT            0x00000000                  // MSI selected as system clock
      #define RCC_CFGR_HSI16_SELECT          0x00000001                  // HSI16 selected as system clock
      #define RCC_CFGR_HSE_SELECT            0x00000002                  // HSE selected as system clock
      #define RCC_CFGR_PLL_SELECT            0x00000003                  // PLL selected as system clock
      #define RCC_CFGR_SYSCLK_MASK           0x00000003
      #define RCC_CFGR_MSI_USED              0x00000000                  // MSI used as system clock
      #define RCC_CFGR_HSI16_USED            0x00000004                  // HSI16 used as system clock
      #define RCC_CFGR_HSE_USED              0x00000008                  // HSE used as system clock
      #define RCC_CFGR_PLL_USED              0x0000000c                  // PLL used as system clock
      #define RCC_CFGR_SWS_MASK              0x0000000c                  // read-only
      #define RCC_CFGR_HPRE_SYSCLK           0x00000000                  // AHB prescaler
      #define RCC_CFGR_HPRE_SYSCLK_DIV2      0x00000080
      #define RCC_CFGR_HPRE_SYSCLK_DIV4      0x00000090
      #define RCC_CFGR_HPRE_SYSCLK_DIV8      0x000000a0
      #define RCC_CFGR_HPRE_SYSCLK_DIV16     0x000000b0
      #define RCC_CFGR_HPRE_SYSCLK_DIV64     0x000000c0
      #define RCC_CFGR_HPRE_SYSCLK_DIV128    0x000000d0
      #define RCC_CFGR_HPRE_SYSCLK_DIV256    0x000000e0
      #define RCC_CFGR_HPRE_SYSCLK_DIV512    0x000000f0
      #define RCC_CFGR_PPRE1_HCLK            0x00000000                  // PCLK1 speed
      #define RCC_CFGR_PPRE1_HCLK_DIV2       0x00000400
      #define RCC_CFGR_PPRE1_HCLK_DIV4       0x00000500
      #define RCC_CFGR_PPRE1_HCLK_DIV8       0x00000600
      #define RCC_CFGR_PPRE1_HCLK_DIV16      0x00000800
      #define RCC_CFGR_PPRE2_HCLK            0x00000000                  // PCLK2 speed
      #define RCC_CFGR_PPRE2_HCLK_DIV2       0x00002000
      #define RCC_CFGR_PPRE2_HCLK_DIV4       0x00002800
      #define RCC_CFGR_PPRE2_HCLK_DIV8       0x00003000
      #define RCC_CFGR_PPRE2_HCLK_DIV16      0x00003800
      #define RCC_CFGR_STOPWUCK              0x00008000                  // wakeup from stop and CSS backup clock selection
      #define RCC_CFGR_MCOSEL_DISABLED       0x00000000
      #define RCC_CFGR_MCOSEL_SYSCLK         0x01000000
      #define RCC_CFGR_MCOSEL_MSI            0x02000000
      #define RCC_CFGR_MCOSEL_HSI16          0x03000000
      #define RCC_CFGR_MCOSEL_HSE            0x04000000
      #define RCC_CFGR_MCOSEL_PLL            0x05000000
      #define RCC_CFGR_MCOSEL_LSI            0x06000000
      #define RCC_CFGR_MCOSEL_LSE            0x07000000
      #define RCC_CFGR_MCOSEL_HSI48          0x08000000
      #define RCC_CFGR_MCOSEL_MASK           0x0f000000
      #define RCC_CFGR_MCOPRE_1              0x00000000                  // MCO divided by 1
      #define RCC_CFGR_MCOPRE_2              0x10000000                  // MCO divided by 2
      #define RCC_CFGR_MCOPRE_4              0x20000000                  // MCO divided by 4
      #define RCC_CFGR_MCOPRE_8              0x30000000                  // MCO divided by 8
      #define RCC_CFGR_MCOPRE_16             0x40000000                  // MCO divided by 16
      #define RCC_CFGR_MCOPRE_MASK           0x70000000
    #define RCC_PLLCFGR                      *(volatile unsigned long *)(RCC_BLOCK + 0x0c) // PLL configuration register
    #define RCC_PLLSAI1CFGR                  *(volatile unsigned long *)(RCC_BLOCK + 0x10) // PLL SAI1 configuration register
    #define RCC_CIER                         *(volatile unsigned long *)(RCC_BLOCK + 0x18) // clock interrupt register
    #define RCC_CIFR                         *(volatile unsigned long *)(RCC_BLOCK + 0x1c) // clock interrupt flag register
    #define RCC_CICR                         *(volatile unsigned long *)(RCC_BLOCK + 0x20) // clock interrupt clear register
    #define RCC_AHB1RSTR                     *(volatile unsigned long *)(RCC_BLOCK + 0x28) // AHB1 peripheral reset register
    #define RCC_AHB2RSTR                     *(volatile unsigned long *)(RCC_BLOCK + 0x2c) // AHB2 peripheral reset register
      #define RCC_AHB2RSTR_GPIOARST          0x00000001
      #define RCC_AHB2RSTR_GPIOBRST          0x00000002
      #define RCC_AHB2RSTR_GPIOCRST          0x00000004
      #define RCC_AHB2RSTR_GPIODRST          0x00000008
      #define RCC_AHB2RSTR_GPIOERST          0x00000010
      #define RCC_AHB2RSTR_GPIOFRST          0x00000020
      #define RCC_AHB2RSTR_GPIOGRST          0x00000040
      #define RCC_AHB2RSTR_GPIOHRST          0x00000080
    #define RCC_AHB3RSTR                     *(volatile unsigned long *)(RCC_BLOCK + 0x30) // AHB3 peripheral reset register
    #define RCC_APB1RSTR1                    *(volatile unsigned long *)(RCC_BLOCK + 0x38) // APB1 peripheral reset register 1
    #define RCC_APB1RSTR2                    *(volatile unsigned long *)(RCC_BLOCK + 0x3c) // APB1 peripheral reset register 2
    #define RCC_APB2RSTR                     *(volatile unsigned long *)(RCC_BLOCK + 0x40) // APB2 peripheral reset register
    #define RCC_AHB1ENR                      *(volatile unsigned long *)(RCC_BLOCK + 0x48) // AHB1 peripheral enable register
      #define RCC_AHB1ENR_FLASHEN            0x00000100
    #define RCC_AHB2ENR                      *(volatile unsigned long *)(RCC_BLOCK + 0x4c) // AHB2 peripheral enable register
        #define RCC_AHB2ENR_GPIOAEN          0x00000001
        #define RCC_AHB2ENR_GPIOBEN          0x00000002
        #define RCC_AHB2ENR_GPIOCEN          0x00000004
        #define RCC_AHB2ENR_GPIODEN          0x00000008
        #define RCC_AHB2ENR_GPIOEEN          0x00000010
        #define RCC_AHB2ENR_GPIOFEN          0x00000020
        #define RCC_AHB2ENR_GPIOGEN          0x00000040
        #define RCC_AHB2ENR_GPIOHEN          0x00000080
    #define RCC_AHB3ENR                      *(volatile unsigned long *)(RCC_BLOCK + 0x50) // AHB3 peripheral enable register
    #define RCC_APB1ENR1                     *(volatile unsigned long *)(RCC_BLOCK + 0x58) // APB1 peripheral enable register 1
      #define RCC_APB1ENR1_RTCAPBEN          0x00000400
      #define RCC_APB1ENR1_USART2EN          0x00020000
      #define RCC_APB1ENR1_USART3EN          0x00040000
      #define RCC_APB1ENR1_UART4EN           0x00080000
      #define RCC_APB1ENR1_UART5EN           0x00100000
    #define RCC_APB1ENR2                     *(volatile unsigned long *)(RCC_BLOCK + 0x5c) // APB1 peripheral enable register 2
      #define RCC_APB1ENR2_LPUART1EN         0x00000001
    #define RCC_APB2ENR                      *(volatile unsigned long *)(RCC_BLOCK + 0x60) // APB2 peripheral enable register
      #define RCC_APB2ENR_SYSCFGEN           0x00000001
      #define RCC_APB2ENR_FWEN               0x00000080
      #define RCC_APB2ENR_SDMMC1EN           0x00000400
      #define RCC_APB2ENR_TIM1EN             0x00000800
      #define RCC_APB2ENR_SPI1EN             0x00001000
      #define RCC_APB2ENR_USART1EN           0x00004000
      #define RCC_APB2ENR_TIM15EN            0x00010000
      #define RCC_APB2ENR_TIM16EN            0x00020000
      #define RCC_APB2ENR_SAI1EN             0x00200000
      #define RCC_APB2ENR_DFSDM1EN           0x01000000
    #define RCC_AHB1SMENR                    *(volatile unsigned long *)(RCC_BLOCK + 0x68)
    #define RCC_AHB2SMENR                    *(volatile unsigned long *)(RCC_BLOCK + 0x6c)
    #define RCC_AHB3SMENR                    *(volatile unsigned long *)(RCC_BLOCK + 0x70)
    #define RCC_APB1SMENR1                   *(volatile unsigned long *)(RCC_BLOCK + 0x78)
    #define RCC_APB1SMENR2                   *(volatile unsigned long *)(RCC_BLOCK + 0x7c)
    #define RCC_APB2SMENR                    *(volatile unsigned long *)(RCC_BLOCK + 0x80)
    #define RCC_CCIPR                        *(volatile unsigned long *)(RCC_BLOCK + 0x88)
    #define RCC_BDCR                         *(volatile unsigned long *)(RCC_BLOCK + 0x90) // backup domain control register
    #define RCC_CSR                          *(volatile unsigned long *)(RCC_BLOCK + 0x94) // control status register
      #define RCC_CSR_LSION                  0x00000001                  // enable 32kHz RC oscillator
      #define RCC_CSR_LSIRDY                 0x00000002                  // read-only
      #define RCC_CSR_MSIRANGE_1M            0x00000040                  // MSI frequency around 1M (can only be written when RCC_CR_MSIRGSEL is set and defines the MSI frequency on exit from standby mode, until RCC_CR_MSIRGSEL set again)
      #define RCC_CSR_MSIRANGE_2M            0x00000050                  // MSI frequency around 2M
      #define RCC_CSR_MSIRANGE_4M            0x00000060                  // MSI frequency around 4M (default after reset)
      #define RCC_CSR_MSIRANGE_8M            0x00000070                  // MSI frequency around 8M
      #define RCC_CSR_MSISRANGE_MASK         0x00000f00                  // read/write
      #define RCC_CSR_RMVF                   0x01000000                  // read/write
      #define RCC_CSR_FWRSTF                 0x02000000                  // read-only
      #define RCC_CSR_PINRSTF                0x04000000                  // read-only
      #define RCC_CSR_PORRSTF                0x08000000                  // read-only
      #define RCC_CSR_SFTRSTF                0x10000000                  // read-only
      #define RCC_CSR_IWDGRSTF               0x20000000                  // read-only
      #define RCC_CSR_WWDGRSTF               0x40000000                  // read-only
      #define RCC_CSR_LPWRRSTF               0x80000000                  // read-only
      #define RESET_CAUSE_FLAGS              (RCC_CSR_FWRSTF | RCC_CSR_RMVF | RCC_CSR_PINRSTF | RCC_CSR_PORRSTF | RCC_CSR_SFTRSTF | RCC_CSR_IWDGRSTF | RCC_CSR_WWDGRSTF | RCC_CSR_LPWRRSTF)
    #define RCC_CRRCR                        *(volatile unsigned long *)(RCC_BLOCK + 0x98)
    #define RCC_CCCIPR2                      *(volatile unsigned long *)(RCC_BLOCK + 0x9c)
#elif defined _STM32L0x1
    #define RCC_CR                           *(volatile unsigned long *)(RCC_BLOCK + 0x00) // clock control register
      #define RCC_CR_HSION                   0x00000001                  // high speed internal clock on
      #define RCC_CR_HSIKERON                0x00000002                  // high speed internal clock forced on in stop mode
      #define RCC_CR_HSIRDY                  0x00000004                  // high speed internal clock read flag [is stable] (read-only)
      #define RCC_CR_HSIDIVEN                0x00000008                  // high speed internal clock division by 4 [can be written at any time]
      #define RCC_CR_HSIDIVF                 0x00000010                  // high speed internal clock divided by 4 (read-only)
      #define RCC_CR_HSIOUTEN                0x00000020                  // high speed internal clock is available for connected to TIM2
      #define RCC_CR_MSION                   0x00000100                  // MSI clock enable
      #define RCC_CR_MSIRDY                  0x00000200                  // MSI oscillator ready (read-only)
      #define RCC_CR_HSEON                   0x00010000                  // HSE clock enable 
      #define RCC_CR_HSERDY                  0x00020000                  // HSE oscillator ready (read-only)
      #define RCC_CR_HSEBYP                  0x00040000                  // HSE clock bypass bit
      #define RCC_CR_CSSHSEON                0x00080000                  // clock security system on HSE enable
      #define RCC_CR_RTCPRE_MASK             0x00300000                  // RTC clock prescaler mask
      #define RCC_CR_PLLON                   0x01000000                  // PLL enable
      #define RCC_CR_PLLRDY                  0x02000000                  // PLL clock ready (read-only)
    #define RCC_ICSCR                        *(volatile unsigned long *)(RCC_BLOCK + 0x04) // internal clock source calibration register
      #define RCC_ICSCR_MSIRANGE_65536       0x00000000                  // MSI frequency around 65.536kHz
      #define RCC_ICSCR_MSIRANGE_131072      0x00002000                  // MSI frequency around 131.072kHz
      #define RCC_ICSCR_MSIRANGE_262144      0x00004000                  // MSI frequency around 262.144kHz
      #define RCC_ICSCR_MSIRANGE_524288      0x00006000                  // MSI frequency around 524.288kHz
      #define RCC_ICSCR_MSIRANGE_1_048M      0x00008000                  // MSI frequency around 1.048M
      #define RCC_ICSCR_MSIRANGE_2_097M      0x0000a000                  // MSI frequency around 2.097M (default after reset)
      #define RCC_ICSCR_MSIRANGE_4_194M      0x0000c000                  // MSI frequency around 4.194M
      #define RCC_ICSCR_MSIRANGE_MASK        0x0000e000
    #define RCC_CFGR                         *(volatile unsigned long *)(RCC_BLOCK + 0x0c) // clock configuration register
      #define RCC_CFGR_MSI_SELECT            0x00000000                  // MSI selected as system clock
      #define RCC_CFGR_HSI16_SELECT          0x00000001                  // HSI16 selected as system clock
      #define RCC_CFGR_HSE_SELECT            0x00000003                  // HSE selected as system clock
      #define RCC_CFGR_PLL_SELECT            0x00000004                  // PLL selected as system clock
      #define RCC_CFGR_SYSCLK_MASK           0x00000003
      #define RCC_CFGR_MSI_USED              0x00000000                  // MSI used as system clock
      #define RCC_CFGR_HSI16_USED            0x00000004                  // HSI16 used as system clock
      #define RCC_CFGR_HSE_USED              0x00000008                  // HSE used as system clock
      #define RCC_CFGR_PLL_USED              0x0000000c                  // PLL used as system clock
      #define RCC_CFGR_SWS_MASK              0x0000000c                  // read-only
      #define RCC_CFGR_HPRE_SYSCLK           0x00000000                  // AHB prescaler
      #define RCC_CFGR_HPRE_SYSCLK_DIV2      0x00000080
      #define RCC_CFGR_HPRE_SYSCLK_DIV4      0x00000090
      #define RCC_CFGR_HPRE_SYSCLK_DIV8      0x000000a0
      #define RCC_CFGR_HPRE_SYSCLK_DIV16     0x000000b0
      #define RCC_CFGR_HPRE_SYSCLK_DIV64     0x000000c0
      #define RCC_CFGR_HPRE_SYSCLK_DIV128    0x000000d0
      #define RCC_CFGR_HPRE_SYSCLK_DIV256    0x000000e0
      #define RCC_CFGR_HPRE_SYSCLK_DIV512    0x000000f0
      #define RCC_CFGR_PPRE1_HCLK            0x00000000                  // PCLK1 speed
      #define RCC_CFGR_PPRE1_HCLK_DIV2       0x00000400
      #define RCC_CFGR_PPRE1_HCLK_DIV4       0x00000500
      #define RCC_CFGR_PPRE1_HCLK_DIV8       0x00000600
      #define RCC_CFGR_PPRE1_HCLK_DIV16      0x00000800
      #define RCC_CFGR_PPRE2_HCLK            0x00000000                  // PCLK2 speed
      #define RCC_CFGR_PPRE2_HCLK_DIV2       0x00002000
      #define RCC_CFGR_PPRE2_HCLK_DIV4       0x00002800
      #define RCC_CFGR_PPRE2_HCLK_DIV8       0x00003000
      #define RCC_CFGR_PPRE2_HCLK_DIV16      0x00003800
      #define RCC_CFGR_STOPWUCK              0x00008000                  // wakeup from stop and CSS backup clock selection
      #define RCC_CFGR_MCOSEL_DISABLED       0x00000000
      #define RCC_CFGR_MCOSEL_SYSCLK         0x01000000
      #define RCC_CFGR_MCOSEL_HSI16          0x02000000
      #define RCC_CFGR_MCOSEL_MSI            0x03000000
      #define RCC_CFGR_MCOSEL_HSE            0x04000000
      #define RCC_CFGR_MCOSEL_PLL            0x05000000
      #define RCC_CFGR_MCOSEL_LSI            0x06000000
      #define RCC_CFGR_MCOSEL_LSE            0x07000000
      #define RCC_CFGR_MCOSEL_HSI48          0x08000000
      #define RCC_CFGR_MCOSEL_MASK           0x0f000000
      #define RCC_CFGR_MCOPRE_1              0x00000000                  // MCO divided by 1
      #define RCC_CFGR_MCOPRE_2              0x10000000                  // MCO divided by 2
      #define RCC_CFGR_MCOPRE_4              0x20000000                  // MCO divided by 4
      #define RCC_CFGR_MCOPRE_8              0x30000000                  // MCO divided by 8
      #define RCC_CFGR_MCOPRE_16             0x40000000                  // MCO divided by 16
      #define RCC_CFGR_MCOPRE_MASK           0x70000000
    #define RCC_CIER                         *(volatile unsigned long *)(RCC_BLOCK + 0x10) // clock interrupt enable register
    #define RCC_CIFR                         *(volatile unsigned long *)(RCC_BLOCK + 0x14) // clock interrupt flag register
    #define RCC_CICR                         *(volatile unsigned long *)(RCC_BLOCK + 0x18) // clock interrupt clear register
    #define RCC_IOPRSTR                      *(volatile unsigned long *)(RCC_BLOCK + 0x1c) // 
    #define RCC_AHBRSTR                      *(volatile unsigned long *)(RCC_BLOCK + 0x20) // AHB peripheral reset register
      #define RCC_AHB2RSTR_DMARST            0x00000001
      #define RCC_AHB2RSTR_MIFRST            0x00000100                  // memory interface reset
      #define RCC_AHB2RSTR_CRCRST            0x00001000
      #define RCC_AHB2RSTR_CRYPRST           0x02000000
    #define RCC_APB2RSTR                     *(volatile unsigned long *)(RCC_BLOCK + 0x24) // APB2 peripheral reset register
      #define RCC_APB2RSTR_USART1RST         0x00003000                  // reset USART1
    #define RCC_APB1RSTR                     *(volatile unsigned long *)(RCC_BLOCK + 0x28) // APB1 peripheral reset register
      #define RCC_APB1RSTR_USART2RST         0x00020000                  // reset USART2
      #define RCC_APB1RSTR_LPUART1RST        0x00040000                  // reset LPUART1
      #define RCC_APB1RSTR_USART4RST         0x00080000                  // reset USART4
      #define RCC_APB1RSTR_USART5RST         0x00100000                  // reset USART5
      #define RCC_APB1RSTR_LPTIM1RST         0x80000000                  // reset LPTIM1
    #define RCC_IOPENR                       *(volatile unsigned long *)(RCC_BLOCK + 0x2c) // GPIO clock enable register
      #define RCC_IOPENR_IOPAEN              0x00000001                  // I/O port A clock enable
      #define RCC_IOPENR_IOPBEN              0x00000002                  // I/O port B clock enable
      #define RCC_IOPENR_IOPCEN              0x00000004                  // I/O port C clock enable
      #define RCC_IOPENR_IOPDEN              0x00000008                  // I/O port D clock enable
      #define RCC_IOPENR_IOPEEN              0x00000010                  // I/O port E clock enable
      #define RCC_IOPENR_IOPFEN              0x00000020                  // I/O port F clock enable
      #define RCC_IOPENR_IOPGEN              0x00000040                  // I/O port G clock enable
      #define RCC_IOPENR_IOPHEN              0x00000080                  // I/O port H clock enable
    #define RCC_AHBENR                       *(volatile unsigned long *)(RCC_BLOCK + 0x30) // AHB peripheral enable register
      #define RCC_AHBENR_MIFEN               0x00000100
    #define RCC_APB2ENR                      *(volatile unsigned long *)(RCC_BLOCK + 0x34) // APB2 peripheral enable register
      #define RCC_APB2ENR_SYSCFGEN           0x00000001
      #define RCC_APB2ENR_USART1EN           0x00004000
    #define RCC_APB1ENR                      *(volatile unsigned long *)(RCC_BLOCK + 0x38) // APB1 peripheral enable register
      #define RCC_APB1ENR_USART2EN           0x00020000                  // USART2 clock enable
      #define RCC_APB1ENR_LPUART1EN          0x00040000                  // LPUART1 clock enable
      #define RCC_APB1ENR_USART4EN           0x00080000                  // USART4 clock enable
      #define RCC_APB1ENR_USART5EN           0x00100000                  // USART5 clock enable
      #define RCC_APB1ENR_LPTIM1EN           0x80000000                  // LPTIM1 clock enable
    #define RCC_IOPSMEN                      *(volatile unsigned long *)(RCC_BLOCK + 0x3c) // GPIO clock enable in sleep mode register
    #define RCC_AHBSMENR                     *(volatile unsigned long *)(RCC_BLOCK + 0x40) // AHB peripheral clock enable in sleep mode register
    #define RCC_APB2SMENR                    *(volatile unsigned long *)(RCC_BLOCK + 0x44) // APB2 peripheral clock enable in sleep mode register
    #define RCC_APB1SMENR                    *(volatile unsigned long *)(RCC_BLOCK + 0x48) // APB1 peripheral clock enable in sleep mode register
    #define RCC_CCIPR                        *(volatile unsigned long *)(RCC_BLOCK + 0x4c) // clock configuration register
    #define RCC_CSR                          *(volatile unsigned long *)(RCC_BLOCK + 0x50) // control status register
      #define RCC_CSR_LSION                  0x00000001                  // enable 32kHz RC oscillator
      #define RCC_CSR_LSIRDY                 0x00000002                  // read-only

      #define RCC_CSR_RMVF                   0x00800000                  // remove reset flag (write '1')
      #define RCC_CSR_FWRSTF                 0x01000000                  // read-only
      #define RCC_CSR_OBLRSTF                0x02000000                  // read-only
      #define RCC_CSR_PINRSTF                0x04000000                  // read-only
      #define RCC_CSR_PORRSTF                0x08000000                  // read-only
      #define RCC_CSR_SFTRSTF                0x10000000                  // read-only
      #define RCC_CSR_IWDGRSTF               0x20000000                  // read-only
      #define RCC_CSR_WWDGRSTF               0x40000000                  // read-only
      #define RCC_CSR_LPWRRSTF               0x80000000                  // read-only
      #define RESET_CAUSE_FLAGS              (RCC_CSR_FWRSTF | RCC_CSR_RMVF | RCC_CSR_PINRSTF | RCC_CSR_OBLRSTF | RCC_CSR_PORRSTF | RCC_CSR_SFTRSTF | RCC_CSR_IWDGRSTF | RCC_CSR_WWDGRSTF | RCC_CSR_LPWRRSTF)
#else
    #define RCC_CR                           *(volatile unsigned long *)(RCC_BLOCK + 0x00) // clock control register
        #define RCC_CR_HSION                 0x00000001
        #define RCC_CR_HSIRDY                0x00000002                  // read-only
        #define RCC_CR_HSITRIM_MASK          0x000000f8
        #define RCC_CR_HSICAL_MASK           0x0000ff00                  // read-only
        #define RCC_CR_HSEON                 0x00010000
        #define RCC_CR_HSERDY                0x00020000                  // read-only
        #define RCC_CR_HSEBYP                0x00040000
        #define RCC_CR_CSSON                 0x00080000
        #define RCC_CR_PLLON                 0x01000000
        #define RCC_CR_PLLRDY                0x02000000                  // read-only
        #define RCC_CR_PLL2ON                0x04000000
        #define RCC_CR_PLL2RDY               0x08000000                  // read-only
        #define RCC_CR_PLL3ON                0x10000000
        #define RCC_CR_PLL3RDY               0x20000000                  // read-only
    #define RCC_CFGR                         *(volatile unsigned long *)(RCC_BLOCK + 0x04) // clock configuration register
        #define RCC_CFGR_HSI_SELECT          0x00000000                  // HSI selected as system clock
        #define RCC_CFGR_HSE_SELECT          0x00000001                  // HSE selected as system clock
        #define RCC_CFGR_PLL_SELECT          0x00000002                  // PLL selected as system clock
        #if defined _STM32F031
            #define RCC_CFGR_HSI48_SELECT    0x00000003                  // HSI48 selected as system clock
        #endif
        #define RCC_CFGR_SYSCLK_MASK         0x00000003
        #define RCC_CFGR_HSI_USED            0x00000000                  // HSI used as system clock
        #define RCC_CFGR_HSE_USED            0x00000004                  // HSE used as system clock
        #define RCC_CFGR_PLL_USED            0x00000008                  // PLL used as system clock
        #if defined _STM32F031
            #define RCC_CFGR_HSI48_USED      0x0000000c                  // HSI48 used as system clock
        #endif
        #define RCC_CFGR_SWS_MASK            0x0000000c                  // read-only
        #define RCC_CFGR_HPRE_SYSCLK         0x00000000                  // HCLK speed
        #define RCC_CFGR_HPRE_SYSCLK_DIV2    0x00000080
        #define RCC_CFGR_HPRE_SYSCLK_DIV4    0x00000090
        #define RCC_CFGR_HPRE_SYSCLK_DIV8    0x000000a0
        #define RCC_CFGR_HPRE_SYSCLK_DIV16   0x000000b0
        #define RCC_CFGR_HPRE_SYSCLK_DIV64   0x000000c0
        #define RCC_CFGR_HPRE_SYSCLK_DIV128  0x000000d0
        #define RCC_CFGR_HPRE_SYSCLK_DIV256  0x000000e0
        #define RCC_CFGR_HPRE_SYSCLK_DIV512  0x000000f0
        #define RCC_CFGR_PPRE1_HCLK          0x00000000                  // PCLK1 speed
        #define RCC_CFGR_PPRE1_HCLK_DIV2     0x00000400
        #define RCC_CFGR_PPRE1_HCLK_DIV4     0x00000500
        #define RCC_CFGR_PPRE1_HCLK_DIV8     0x00000600
        #define RCC_CFGR_PPRE1_HCLK_DIV16    0x00000700
        #if defined _STM32F031                                           // PCLK only
            #define RCC_CFGR_PPRE2_HCLK          0x00000000              // must remain at 0
        #else
            #define RCC_CFGR_PPRE2_HCLK          0x00000000              // PCLK2 speed
            #define RCC_CFGR_PPRE2_HCLK_DIV2     0x00002000
            #define RCC_CFGR_PPRE2_HCLK_DIV4     0x00002800
            #define RCC_CFGR_PPRE2_HCLK_DIV8     0x00003000
            #define RCC_CFGR_PPRE2_HCLK_DIV16    0x00003800
        #endif
        #define RCC_CFGR_ADCPRE_PCLK2_DIV2   0x00000000                  // ADC clock speed
        #define RCC_CFGR_ADCPRE_PCLK2_DIV4   0x00004000
        #define RCC_CFGR_ADCPRE_PCLK2_DIV8   0x00008000
        #define RCC_CFGR_ADCPRE_PCLK2_DIV16  0x0000c000
        #define RCC_CFGR_PLL_SRC             0x00010000
        #define RCC_CFGR_PLL_XTPRE           0x00020000
        #define RCC_CFGR_PLLMUL_MASK         0x003c0000
        #define RCC_CFGR_OTGFSPRE            0x00400000
        #if defined _STM32F031
            #define RCC_CFGR_MCO_HSI14       0x01000000
            #define RCC_CFGR_MCO_LSI         0x02000000
            #define RCC_CFGR_MCO_LSE         0x03000000
        #endif
        #define RCC_CFGR_MCOSEL_SYSCLK       0x04000000
        #define RCC_CFGR_MCOSEL_HSICLK       0x05000000
        #define RCC_CFGR_MCOSEL_HSECLK       0x06000000
        #define RCC_CFGR_MCOSEL_PLLDIV2      0x07000000
        #if defined _STM32F031
            #define RCC_CFGR_MCOSEL_HSI48    0x08000000
            #define RCC_CFGR_MCOSEL_MASK     0x0f000000
            #define RCC_CFGR_MCOPRE_1        0x00000000                  // MCO divided by 1
            #define RCC_CFGR_MCOPRE_2        0x10000000                  // MCO divided by 2
            #define RCC_CFGR_MCOPRE_4        0x20000000                  // MCO divided by 4
            #define RCC_CFGR_MCOPRE_8        0x30000000                  // MCO divided by 8
            #define RCC_CFGR_MCOPRE_16       0x40000000                  // MCO divided by 16
            #define RCC_CFGR_MCOPRE_32       0x50000000                  // MCO divided by 32
            #define RCC_CFGR_MCOPRE_64       0x60000000                  // MCO divided by 64
            #define RCC_CFGR_MCOPRE_128      0x70000000                  // MCO divided by 128
            #define RCC_CFGR_MCOPRE_MASK     0x70000000
            #define MCO_DIVIDE_MAX           128
        #else
            #if defined _CONNECTIVITY_LINE
                #define RCC_CFGR_MCOSEL_PLL2      0x08000000
                #define RCC_CFGR_MCOSEL_PLL3DIV2  0x09000000
                #define RCC_CFGR_MCOSEL_XT1       0x0a000000
                #define RCC_CFGR_MCOSEL_PLL3      0x0b000000
                #define RCC_CFGR_MCOSEL_MASK      0x0f000000
            #else
                #define RCC_CFGR_MCOSEL_MASK      0x07000000
            #endif
        #endif
    #define RCC_CIR                          *(volatile unsigned long *)(RCC_BLOCK + 0x08) // clock interrupt register
        #define RCC_CIR_LSIRDYF              0x00000001                  // read-only
        #define RCC_CIR_LSERDYF              0x00000002                  // read-only
        #define RCC_CIR_HSIRDYF              0x00000004                  // read-only
        #define RCC_CIR_HSERDYF              0x00000008                  // read-only
        #define RCC_CIR_PLLRDYF              0x00000010                  // read-only
        #define RCC_CIR_PLL2RDYF             0x00000020                  // read-only
        #define RCC_CIR_PLL3RDYF             0x00000040                  // read-only
        #define RCC_CIR_CSSF                 0x00000080                  // read-only
        #define RCC_CIR_LSIRDYIE             0x00000100
        #define RCC_CIR_LSERDYIE             0x00000200
        #define RCC_CIR_HSIRDYIE             0x00000400
        #define RCC_CIR_HSERDYIE             0x00000800
        #define RCC_CIR_PLLRDYIE             0x00001000
        #define RCC_CIR_PLL2RDYIE            0x00002000
        #define RCC_CIR_PLL3RDYIE            0x00004000
        #define RCC_CIR_LSIRDYC              0x00010000                  // write only
        #define RCC_CIR_LSERDYC              0x00020000                  // write only
        #define RCC_CIR_HSIRDYC              0x00040000                  // write only
        #define RCC_CIR_HSERDYC              0x00080000                  // write only
        #define RCC_CIR_PLLRDYC              0x00100000                  // write only
        #define RCC_CIR_PLL2RDYC             0x00200000                  // write only
        #define RCC_CIR_PLL3RDYC             0x00400000                  // write only
        #define RCC_CIR_CSSC                 0x00800000                  // write only
    #define RCC_APB2RSTR                     *(volatile unsigned long *)(RCC_BLOCK + 0x0c) // APB2 peripheral reset register
    #if defined _STM32F031
        #define RCC_APB2RSTR_SYSCRGRST       0x00000001
        #define RCC_APB2RSTR_USART6RST       0x00000020
        #define RCC_APB2RSTR_USART7RST       0x00000040
        #define RCC_APB2RSTR_USART8RST       0x00000080
    #else
        #define RCC_APB2RSTR_AFIORST         0x00000001
        #define RCC_APB2RSTR_IOPARST         0x00000004
        #define RCC_APB2RSTR_IOPBRST         0x00000008
        #define RCC_APB2RSTR_IOPCRST         0x00000010
        #define RCC_APB2RSTR_IOPDRST         0x00000020
        #define RCC_APB2RSTR_IOPERST         0x00000040
    #endif
        #define RCC_APB2RSTR_ADC1RST         0x00000200
        #define RCC_APB2RSTR_ADC2RST         0x00000400
        #define RCC_APB2RSTR_TIM1RST         0x00000800
        #define RCC_APB2RSTR_SPI1RST         0x00001000
        #define RCC_APB2RSTR_USART1RST       0x00004000
    #define RCC_APB1RSTR                     *(volatile unsigned long *)(RCC_BLOCK + 0x10) // APB1 peripheral reset register
        #define RCC_APB1RSTR_TIM2RST         0x00000001
        #define RCC_APB1RSTR_TIM3RST         0x00000002
        #define RCC_APB1RSTR_TIM4RST         0x00000004
        #define RCC_APB1RSTR_TIM5RST         0x00000008
        #define RCC_APB1RSTR_TIM6RST         0x00000010
        #define RCC_APB1RSTR_TIM7RST         0x00000020
        #define RCC_APB1RSTR_TIM14RST        0x00000100
        #define RCC_APB1RSTR_WWDGRST         0x00000800
        #define RCC_APB1RSTR_SPI2RST         0x00004000
        #define RCC_APB1RSTR_SPI3RST         0x00008000
        #define RCC_APB1RSTR_USART2RST       0x00020000
        #define RCC_APB1RSTR_USART3RST       0x00040000
        #define RCC_APB1RSTR_UART4RST        0x00080000
        #define RCC_APB1RSTR_USART4RST       0x00080000
        #define RCC_APB1RSTR_UART5RST        0x00100000
        #define RCC_APB1RSTR_USART5RST       0x00100000
        #define RCC_APB1RSTR_I2C1RST         0x00200000
        #define RCC_APB1RSTR_I2C2RST         0x00400000
        #define RCC_APB1RSTR_USBRST          0x00800000
        #define RCC_APB1RSTR_CAN1RST         0x02000000
        #define RCC_APB1RSTR_CAN2RST         0x04000000
        #if defined _STM32F031
            #define RCC_APB1RSTR_CRSRST      0x08000000
        #else
            #define RCC_APB1RSTR_BKPRST      0x08000000
        #endif
        #define RCC_APB1RSTR_PWRRST          0x10000000
        #define RCC_APB1RSTR_DACRST          0x20000000
    #define RCC_AHB1ENR                      *(volatile unsigned long *)(RCC_BLOCK + 0x14) // AHB peripheral clock enable register
    #define RCC_AHBENR                       *(volatile unsigned long *)(RCC_BLOCK + 0x14) // AHB peripheral clock enable register (alternative name when only one AHB bus is present)
        #define RCC_AHB1ENR_DMA1EN           0x00000001
        #define RCC_AHB1ENR_DMA2EN           0x00000002
        #define RCC_AHB1ENR_SRAMEN           0x00000004
        #define RCC_AHB1ENR_FLITFEN          0x00000010
        #define RCC_AHB1ENR_CRCEN            0x00000040
        #define RCC_AHB1ENR_OTGFSEN          0x00001000
        #define RCC_AHB1ENR_ETHMACEN         0x00004000
        #define RCC_AHB1ENR_ETHMACTXEN       0x00008000
        #define RCC_AHB1ENR_ETHMACRXEN       0x00010000
        #if defined _STM32F031
            #define RCC_AHBENR_IOPAEN        0x00020000
            #define RCC_AHBENR_IOPBEN        0x00040000
            #define RCC_AHBENR_IOPCEN        0x00080000
            #define RCC_AHBENR_IOPDEN        0x00100000
            #define RCC_AHBENR_IOPEEN        0x00200000
            #define RCC_AHBENR_IOPFEN        0x00400000
            #define RCC_AHBENR_TSCEN         0x01000000
        #endif
    #define RCC_APB2ENR                      *(volatile unsigned long *)(RCC_BLOCK + 0x18) // APB2 peripheral clock enable register
        #if defined _STM32F031
            #define RCC_APB2ENR_SYSCFGEN     0x00000001
            #define RCC_APB2ENR_USART6EN     0x00000020
            #define RCC_APB2ENR_USART7EN     0x00000040
            #define RCC_APB2ENR_USART8EN     0x00000080
            #define RCC_APB2ENR_ADC1EN       0x00000200
            #define RCC_APB2ENR_TIM1EN       0x00000800
            #define RCC_APB2ENR_SPI1EN       0x00001000
            #define RCC_APB2ENR_USART1EN     0x00004000
            #define RCC_APB2ENR_TIM15EN      0x00010000
            #define RCC_APB2ENR_TIM16EN      0x00020000
            #define RCC_APB2ENR_TIM17EN      0x00040000
            #define RCC_APB2ENR_DBGMCUEN     0x00400000
        #else
            #define RCC_APB2ENR_AFIOEN       0x00000001
            #define RCC_APB2ENR_IOPAEN       0x00000004
            #define RCC_APB2ENR_IOPBEN       0x00000008
            #define RCC_APB2ENR_IOPCEN       0x00000010
            #define RCC_APB2ENR_IOPDEN       0x00000020
            #define RCC_APB2ENR_IOPEEN       0x00000040
            #define RCC_APB2ENR_IOPFEN       0x00000080
            #define RCC_APB2ENR_IOPGEN       0x00000100
            #define RCC_APB2ENR_ADC1EN       0x00000200
            #define RCC_APB2ENR_ADC2EN       0x00000400
            #define RCC_APB2ENR_TIM1EN       0x00000800
            #define RCC_APB2ENR_SPI1EN       0x00001000
            #define RCC_APB2ENR_TIM8EN       0x00002000
            #define RCC_APB2ENR_USART1EN     0x00004000
            #define RCC_APB2ENR_ADC3EN       0x00008000
            #define RCC_APB2ENR_TIM9EN       0x00080000
            #define RCC_APB2ENR_TIM10EN      0x00100000
            #define RCC_APB2ENR_TIM11EN      0x00200000
        #endif
    #define RCC_APB1ENR                      *(volatile unsigned long *)(RCC_BLOCK + 0x1c) // APB1 peripheral clock enable register
        #define RCC_APB1ENR_TIM2EN           0x00000001
        #define RCC_APB1ENR_TIM3EN           0x00000002
        #define RCC_APB1ENR_TIM4EN           0x00000004
        #define RCC_APB1ENR_TIM5EN           0x00000008
        #define RCC_APB1ENR_TIM6EN           0x00000010
        #define RCC_APB1ENR_TIM7EN           0x00000020
        #if defined _STM32F031
            #define RCC_APB1ENR_TIM14EN      0x00000100
        #endif
        #define RCC_APB1ENR_WWDGEN           0x00000800
        #define RCC_APB1ENR_SPI2EN           0x00004000
        #define RCC_APB1ENR_SPI3EN           0x00008000
        #define RCC_APB1ENR_USART2EN         0x00020000
        #define RCC_APB1ENR_USART3EN         0x00040000
        #define RCC_APB1ENR_UART4EN          0x00080000
        #define RCC_APB1ENR_USART4EN         0x00080000
        #define RCC_APB1ENR_UART5EN          0x00100000
        #define RCC_APB1ENR_USART5EN         0x00100000
        #define RCC_APB1ENR_I2C1EN           0x00200000
        #define RCC_APB1ENR_I2C2EN           0x00400000
        #define RCC_APB1ENR_USBEN            0x00800000
        #define RCC_APB1ENR_CAN1EN           0x02000000
        #define RCC_APB1ENR_CAN2EN           0x04000000
        #if defined _STM32F031
            #define RCC_APB1ENR_CRSEN        0x08000000
        #else
            #define RCC_APB1ENR_BKPEN        0x08000000
        #endif
        #define RCC_APB1ENR_PWREN            0x10000000
        #define RCC_APB1ENR_DACEN            0x20000000
        #define RCC_APB1ENR_CECEN            0x40000000
    #define RCC_BDCR                         *(volatile unsigned long *)(RCC_BLOCK + 0x20) // backup domain control register
        #define RCC_BDCR_LSEON               0x00000001
        #define RCC_BDCR_LSERDY              0x00000002                  // read-only
        #define RCC_BDCR_LSEBYP              0x00000004
        #define RCC_BDCR_RTCSEC_MASK         0x00000300
        #define RCC_BDCR_RTCSEC_LSE          0x00000100                  // LSE oscillator clock used as the RTC clock
        #define RCC_BDCR_RTCSEC_LSI          0x00000200                  // LSI oscillator clock used as the RTC clock
        #define RCC_BDCR_RTCSEC_HSE          0x00000300                  // HSE oscillator clock divided by RTCPRE[4:0] in RCC:CFGR used as the RTC clock
        #define RCC_BDCR_RTCEN               0x00008000
        #define RCC_BDCR_BDRST               0x00010000
    #define RCC_CSR                          *(volatile unsigned long *)(RCC_BLOCK + 0x24) // control status register
        #define RCC_CSR_LSION                0x00000001                  // internal low-speed oscillator enable (enable 40kHz RC oscillator)
        #define RCC_CSR_LSIRDY               0x00000002                  // internal low-speed oscillator ready (read-only)
        #if defined _STM32F031
            #define RCC_CSR_V18PWRRSTF       0x00800000
        #endif
        #define RCC_CSR_RMVF                 0x01000000                  // remove reset flag (write '1' to clear all flags)
        #if defined _STM32F031
            #define RCC_CSR_OBLRSTF          0x02000000
        #endif
        #define RCC_CSR_PINRSTF              0x04000000                  // pin reset flag (read-only)
        #define RCC_CSR_PORRSTF              0x08000000                  // power on reset/power down reset flag (read-only)
        #define RCC_CSR_SFTRSTF              0x10000000                  // software reset flag (read-only)
        #define RCC_CSR_IWDGRSTF             0x20000000                  // independent watchdog reset flag (read-only)
        #define RCC_CSR_WWDGRSTF             0x40000000                  // window watchdog reset flag (read-only)
        #define RCC_CSR_LPWRRSTF             0x80000000                  // low power reset flag (read-only)
        #if defined _STM32F031
            #define RESET_CAUSE_FLAGS        (RCC_CSR_V18PWRRSTF | RCC_CSR_OBLRSTF | RCC_CSR_RMVF | RCC_CSR_PINRSTF | RCC_CSR_PORRSTF | RCC_CSR_SFTRSTF | RCC_CSR_IWDGRSTF | RCC_CSR_WWDGRSTF | RCC_CSR_LPWRRSTF)
        #else
            #define RESET_CAUSE_FLAGS        (RCC_CSR_RMVF | RCC_CSR_PINRSTF | RCC_CSR_PORRSTF | RCC_CSR_SFTRSTF | RCC_CSR_IWDGRSTF | RCC_CSR_WWDGRSTF | RCC_CSR_LPWRRSTF)
        #endif
    #define RCC_AHB1RSTR                     *(unsigned long *)(RCC_BLOCK + 0x28) // AHB peripheral clock enable register
      #define RCC_AHBRSTR_OTGFSRST           0x00001000
      #define RCC_AHBRSTR_ETHMACRST          0x00004000
      #define RCC_CFGR2                      *(unsigned long *)(RCC_BLOCK + 0x2c) // clock configuration register 2
        #define RCC_CFGR2_PREDIV1_MASK       0x0000000f
    #ifdef _CONNECTIVITY_LINE
          #define RCC_CFGR2_PREDIV2_MASK     0x000000f0
          #define RCC_CFGR2_PLL2MUL_MASK     0x00000f00
          #define RCC_CFGR2_PLL3MUL_MASK     0x0000f000
          #define RCC_CFGR2_PREDIV1SRC       0x00010000
          #define RCC_CFGR2_I2S2SRC          0x00020000
          #define RCC_CFGR2_I2S3SRC          0x00040000
    #endif
    #if defined _STM32F031
        #define RCC_CFGR3                    *(unsigned long *)(RCC_BLOCK + 0x30) // clock configuration register 3
        #define RCC_CR2                      *(unsigned long *)(RCC_BLOCK + 0x34) // clock control register 2
    #endif
#endif
#define SOFTWARE_RESET_DETECTED()            ((RCC_CSR & (RCC_CSR_PORRSTF | RCC_CSR_SFTRSTF | RCC_CSR_IWDGRSTF)) == RCC_CSR_SFTRSTF)
#define CLEAR_RESET_CAUSES()                  WRITE_ONE_TO_CLEAR(RCC_CSR, RCC_CSR_RMVF)

// Macro to reset peripheral module
//
#define _RESET_CYCLE_PERIPHERAL(bus, module) RCC_##bus##RSTR |= (module); RCC_##bus##RSTR &= ~(module);

// DMA controllers
//
#if defined _STM32F2XX || defined _STM32F4XX || defined _STM32F7XX       // {15}
    #define DMA1_LISR                    *(volatile unsigned long *)(DMA1_BLOCK + 0x00)  // DMA 1 Low Interrupt Status Register (read-only)
      #define DMA_LISR_FEIFO0            0x00000001                            // stream 0 FIFO error interrupt flag
      #define DMA_LISR_DMEIF0            0x00000004                            // stream 0 direct mode error interrupt flag
      #define DMA_LISR_TEIF0             0x00000008                            // stream 0 transfer error interrupt flag
      #define DMA_LISR_HTIF0             0x00000010                            // stream 0 half transfer interrupt flag
      #define DMA_LISR_TCIF0             0x00000020                            // stream 0 transfer complete interrupt flag
      #define DMA_LISR_FEIFO1            0x00000040                            // stream 1 FIFO error interrupt flag
      #define DMA_LISR_DMEIF1            0x00000100                            // stream 1 direct mode error interrupt flag
      #define DMA_LISR_TEIF1             0x00000200                            // stream 1 transfer error interrupt flag
      #define DMA_LISR_HTIF1             0x00000400                            // stream 1 half transfer interrupt flag
      #define DMA_LISR_TCIF1             0x00000800                            // stream 1 transfer complete interrupt flag
      #define DMA_LISR_FEIFO2            0x00010000                            // stream 2 FIFO error interrupt flag
      #define DMA_LISR_DMEIF2            0x00040000                            // stream 2 direct mode error interrupt flag
      #define DMA_LISR_TEIF2             0x00080000                            // stream 2 transfer error interrupt flag
      #define DMA_LISR_HTIF2             0x00100000                            // stream 2 half transfer interrupt flag
      #define DMA_LISR_TCIF2             0x00200000                            // stream 2 transfer complete interrupt flag
      #define DMA_LISR_FEIFO3            0x00400000                            // stream 3 FIFO error interrupt flag
      #define DMA_LISR_DMEIF3            0x01000000                            // stream 3 direct mode error interrupt flag
      #define DMA_LISR_TEIF3             0x02000000                            // stream 3 transfer error interrupt flag
      #define DMA_LISR_HTIF3             0x04000000                            // stream 3 half transfer interrupt flag
      #define DMA_LISR_TCIF3             0x08000000                            // stream 3 transfer complete interrupt flag
    #define DMA1_HISR                    *(volatile unsigned long *)(DMA1_BLOCK + 0x04)  // DMA 1 High Interrupt Status Register (read-only)
      #define DMA_HISR_FEIFO4            0x00000001                            // stream 4 FIFO error interrupt flag
      #define DMA_HISR_DMEIF4            0x00000004                            // stream 4 direct mode error interrupt flag
      #define DMA_HISR_TEIF4             0x00000008                            // stream 4 transfer error interrupt flag
      #define DMA_HISR_HTIF4             0x00000010                            // stream 4 half transfer interrupt flag
      #define DMA_HISR_TCIF4             0x00000020                            // stream 4 transfer complete interrupt flag
      #define DMA_HISR_FEIFO5            0x00000040                            // stream 5 FIFO error interrupt flag
      #define DMA_HISR_DMEIF5            0x00000100                            // stream 5 direct mode error interrupt flag
      #define DMA_HISR_TEIF5             0x00000200                            // stream 5 transfer error interrupt flag
      #define DMA_HISR_HTIF5             0x00000400                            // stream 5 half transfer interrupt flag
      #define DMA_HISR_TCIF5             0x00000800                            // stream 5 transfer complete interrupt flag
      #define DMA_HISR_FEIFO6            0x00010000                            // stream 6 FIFO error interrupt flag
      #define DMA_HISR_DMEIF6            0x00040000                            // stream 6 direct mode error interrupt flag
      #define DMA_HISR_TEIF6             0x00080000                            // stream 6 transfer error interrupt flag
      #define DMA_HISR_HTIF6             0x00100000                            // stream 6 half transfer interrupt flag
      #define DMA_HISR_TCIF6             0x00200000                            // stream 6 transfer complete interrupt flag
      #define DMA_HISR_FEIFO7            0x00400000                            // stream 7 FIFO error interrupt flag
      #define DMA_HISR_DMEIF7            0x01000000                            // stream 7 direct mode error interrupt flag
      #define DMA_HISR_TEIF7             0x02000000                            // stream 7 transfer error interrupt flag
      #define DMA_HISR_HTIF7             0x04000000                            // stream 7 half transfer interrupt flag
      #define DMA_HISR_TCIF7             0x08000000                            // stream 7 transfer complete interrupt flag
    #define DMA1_LIFCR                   *(volatile unsigned long *)(DMA1_BLOCK + 0x08)  // DMA 1 Low Interrupt Flag Clear Register (write-'1' to clear)
      #define DMA_LIFCR_FEIFO0           0x00000001                            // stream 0 clear FIFO error interrupt flag
      #define DMA_LIFCR_DMEIF0           0x00000004                            // stream 0 clear direct mode error interrupt flag
      #define DMA_LIFCR_TEIF0            0x00000008                            // stream 0 clear transfer error interrupt flag
      #define DMA_LIFCR_HTIF0            0x00000010                            // stream 0 clear half transfer interrupt flag
      #define DMA_LIFCR_TCIF0            0x00000020                            // stream 0 clear transfer complete interrupt flag
      #define DMA_LIFCR_FEIFO1           0x00000040                            // stream 1 clear FIFO error interrupt flag
      #define DMA_LIFCR_DMEIF1           0x00000100                            // stream 1 clear direct mode error interrupt flag
      #define DMA_LIFCR_TEIF1            0x00000200                            // stream 1 clear transfer error interrupt flag
      #define DMA_LIFCR_HTIF1            0x00000400                            // stream 1 clear half transfer interrupt flag
      #define DMA_LIFCR_TCIF1            0x00000800                            // stream 1 clear transfer complete interrupt flag
      #define DMA_LIFCR_FEIFO2           0x00010000                            // stream 2 clear FIFO error interrupt flag
      #define DMA_LIFCR_DMEIF2           0x00040000                            // stream 2 clear direct mode error interrupt flag
      #define DMA_LIFCR_TEIF2            0x00080000                            // stream 2 clear transfer error interrupt flag
      #define DMA_LIFCR_HTIF2            0x00100000                            // stream 2 clear half transfer interrupt flag
      #define DMA_LIFCR_TCIF2            0x00200000                            // stream 2 clear transfer complete interrupt flag
      #define DMA_LIFCR_FEIFO3           0x00400000                            // stream 3 clear FIFO error interrupt flag
      #define DMA_LIFCR_DMEIF3           0x01000000                            // stream 3 clear direct mode error interrupt flag
      #define DMA_LIFCR_TEIF3            0x02000000                            // stream 3 clear transfer error interrupt flag
      #define DMA_LIFCR_HTIF3            0x04000000                            // stream 3 clear half transfer interrupt flag
      #define DMA_LIFCR_TCIF3            0x08000000                            // stream 3 clear transfer complete interrupt flag
    #define DMA1_HIFCR                   *(volatile unsigned long *)(DMA1_BLOCK + 0x0c)  // DMA 1 High Interrupt Flag Clear Register (write-'1' to clear)
      #define DMA_HIFCR_FEIFO4           0x00000001                            // stream 4 clear FIFO error interrupt flag
      #define DMA_HIFCR_DMEIF4           0x00000004                            // stream 4 clear direct mode error interrupt flag
      #define DMA_HIFCR_TEIF4            0x00000008                            // stream 4 clear transfer error interrupt flag
      #define DMA_HIFCR_HTIF4            0x00000010                            // stream 4 clear half transfer interrupt flag
      #define DMA_HIFCR_TCIF4            0x00000020                            // stream 4 clear transfer complete interrupt flag
      #define DMA_HIFCR_FEIFO5           0x00000040                            // stream 5 clear FIFO error interrupt flag
      #define DMA_HIFCR_DMEIF5           0x00000100                            // stream 5 clear direct mode error interrupt flag
      #define DMA_HIFCR_TEIF5            0x00000200                            // stream 5 clear transfer error interrupt flag
      #define DMA_HIFCR_HTIF5            0x00000400                            // stream 5 clear half transfer interrupt flag
      #define DMA_HIFCR_TCIF5            0x00000800                            // stream 5 clear transfer complete interrupt flag
      #define DMA_HIFCR_FEIFO6           0x00010000                            // stream 6 clear FIFO error interrupt flag
      #define DMA_HIFCR_DMEIF6           0x00040000                            // stream 6 clear direct mode error interrupt flag
      #define DMA_HIFCR_TEIF6            0x00080000                            // stream 6 clear transfer error interrupt flag
      #define DMA_HIFCR_HTIF6            0x00100000                            // stream 6 clear half transfer interrupt flag
      #define DMA_HIFCR_TCIF6            0x00200000                            // stream 6 clear transfer complete interrupt flag
      #define DMA_HIFCR_FEIFO7           0x00400000                            // stream 7 clear FIFO error interrupt flag
      #define DMA_HIFCR_DMEIF7           0x01000000                            // stream 7 clear direct mode error interrupt flag
      #define DMA_HIFCR_TEIF7            0x02000000                            // stream 7 clear transfer error interrupt flag
      #define DMA_HIFCR_HTIF7            0x04000000                            // stream 7 clear half transfer interrupt flag
      #define DMA_HIFCR_TCIF7            0x08000000                            // stream 7 clear transfer complete interrupt flag
    #define DMA1_S0CR                    *(volatile unsigned long *)(DMA1_BLOCK + 0x10)  // DMA 1 Stream 0 Configuration Register
      #define DMA_SxCR_EN                0x00000001                             // channel enable
      #define DMA_SxCR_DMEIE             0x00000002                             // direct mode error interrupt enable
      #define DMA_SxCR_TEIE              0x00000004                             // transfer error interrupt enable
      #define DMA_SxCR_HTIE              0x00000008                             // half transfer interrupt enable
      #define DMA_SxCR_TCIE              0x00000010                             // transfer complete interrupt enable
      #define DMA_SxCR_PFCTRL            0x00000020                             // peripheral flow control
      #define DMA_SxCR_DIR_P2M           0x00000000                             // peripheral to memory transfer
      #define DMA_SxCR_DIR_M2P           0x00000040                             // memory to peripheral transfer
      #define DMA_SxCR_DIR_M2M           0x00000080                             // memory to memory transfer
      #define DMA_SxCR_CIRC              0x00000100                             // circular mode
      #define DMA_SxCR_PINC              0x00000200                             // peripheral increment mode
      #define DMA_SxCR_MINC              0x00000400                             // memory increment mode
      #define DMA_SxCR_PSIZE_8           0x00000000                             // peripheral size 8 bit
      #define DMA_SxCR_PSIZE_16          0x00000800                             // peripheral size 16 bit
      #define DMA_SxCR_PSIZE_32          0x00001000                             // peripheral size 32 bit
      #define DMA_SxCR_MSIZE_8           0x00000000                             // memory size 8 bit
      #define DMA_SxCR_MSIZE_16          0x00002000                             // memory size 16 bit
      #define DMA_SxCR_MSIZE_32          0x00006000                             // memory size 32 bit
      #define DMA_SxCR_PINCOS            0x00008000                             // peripheral increment size offset
      #define DMA_SxCR_PL_LOW            0x00000000                             // channel low priority
      #define DMA_SxCR_PL_MEDIUM         0x00010000                             // channel medium priority
      #define DMA_SxCR_PL_HIGH           0x00020000                             // channel high priority
      #define DMA_SxCR_PL_VERY_HIGH      0x00030000                             // channel very high priority
      #define DMA_SxCR_DBM               0x00040000                             // double buffered mode
      #define DMA_SxCR_CT                0x00080000                             // current target (only in double-buffered mode)
      #define DMA_SxCR_PBURST_SINGLE     0x00000000                             // peripheral burst - single transfer (force in direct mode)
      #define DMA_SxCR_PBURST_INCR4      0x00200000                             // memory burst - incremental burst of 4 beats
      #define DMA_SxCR_PBURST_INCR8      0x00400000                             // memory burst - incremental burst of 8 beats
      #define DMA_SxCR_PBURST_INCR16     0x00600000                             // memory burst - incremental burst of 16 beats
      #define DMA_SxCR_MBURST_SINGLE     0x00000000                             // memory burst - single transfer (force in direct mode)
      #define DMA_SxCR_MBURST_INCR4      0x00800000                             // memory burst - incremental burst of 4 beats
      #define DMA_SxCR_MBURST_INCR8      0x01000000                             // memory burst - incremental burst of 8 beats
      #define DMA_SxCR_MBURST_INCR16     0x01800000                             // memory burst - incremental burst of 16 beats
      #define DMA_SxCR_CHSEL_0           0x00000000                             // channel select - channel 0
      #define DMA_SxCR_CHSEL_1           0x02000000                             // channel select - channel 1
      #define DMA_SxCR_CHSEL_2           0x04000000                             // channel select - channel 2
      #define DMA_SxCR_CHSEL_3           0x06000000                             // channel select - channel 3
      #define DMA_SxCR_CHSEL_4           0x08000000                             // channel select - channel 4
      #define DMA_SxCR_CHSEL_5           0x0a000000                             // channel select - channel 5
      #define DMA_SxCR_CHSEL_6           0x0c000000                             // channel select - channel 6
      #define DMA_SxCR_CHSEL_7           0x0e000000                             // channel select - channel 7
    #define DMA1_S0NDTR                  *(volatile unsigned long *)(DMA1_BLOCK + 0x14)  // DMA 1 Stream 0 Number of Data Register (1..64k - write only when channel disabled)
    #define DMA1_S0PAR                   *(unsigned long *)(DMA1_BLOCK + 0x18)  // DMA 1 Stream 0 Peripheral Address Register (can only be written when the controller is disabled)
    #define DMA1_S0M0AR                  *(unsigned long *)(DMA1_BLOCK + 0x1c)  // DMA 1 Stream 0 Memory 0 Address Register
    #define DMA1_S0M1AR                  *(unsigned long *)(DMA1_BLOCK + 0x20)  // DMA 1 Stream 0 Memory 1 Address Register
    #define DMA1_S0FCR                   *(volatile unsigned long *)(DMA1_BLOCK + 0x24)  // DMA 1 Stream 0 FIFO Control Register
      #define DMA_SxFCR_FTH_1_4          0x00000000                             // FIFI threshold selection - 1/4 full FIFO
      #define DMA_SxFCR_FTH_1_2          0x00000001                             // FIFI threshold selection - 1/2 full FIFO
      #define DMA_SxFCR_FTH_3_4          0x00000002                             // FIFI threshold selection - 3/4 full FIFO
      #define DMA_SxFCR_FTH_FULL         0x00000003                             // FIFI threshold selection - full FIFO
      #define DMA_SxFCR_DMDIS            0x00000004                             // direct mode disable (set by hardware when M2M mode is used)
      #define DMA_SxFCR_FS_0             0x00000000                             // FIFO status (read-only) between 0 and 1/4 full
      #define DMA_SxFCR_FS_1_4           0x00000008                             // FIFO status (read-only) between 1/4 and 1/2 full
      #define DMA_SxFCR_FS_1_2           0x00000010                             // FIFO status (read-only) between 1/2 and 3/4 full
      #define DMA_SxFCR_FS_3_4           0x00000018                             // FIFO status (read-only) between 3/4 and full
      #define DMA_SxFCR_FS_EMPTY         0x00000020                             // FIFO status (read-only) empty
      #define DMA_SxFCR_FS_FULL          0x00000028                             // FIFO status (read-only) full
      #define DMA_SxFCR_FEIE             0x00000080                             // FIFO error interrupt enable
    #define DMA1_S1CR                    *(volatile unsigned long *)(DMA1_BLOCK + 0x28)  // DMA 1 Stream 1 Configuration Register
    #define DMA1_S1NDTR                  *(volatile unsigned long *)(DMA1_BLOCK + 0x2c)  // DMA 1 Stream 1 Number of Data Register (1..64k - write only when channel disabled)
    #define DMA1_S1PAR                   *(unsigned long *)(DMA1_BLOCK + 0x30)  // DMA 1 Stream 1 Peripheral Address Register
    #define DMA1_S1M0AR                  *(unsigned long *)(DMA1_BLOCK + 0x34)  // DMA 1 Stream 1 Memory 0 Address Register
    #define DMA1_S1M1AR                  *(unsigned long *)(DMA1_BLOCK + 0x38)  // DMA 1 Stream 1 Memory 1 Address Register
    #define DMA1_S1FCR                   *(volatile unsigned long *)(DMA1_BLOCK + 0x3c)  // DMA 1 Stream 1 FIFO Control Register
    #define DMA1_S2CR                    *(volatile unsigned long *)(DMA1_BLOCK + 0x40)  // DMA 1 Stream 2 Configuration Register
    #define DMA1_S2NDTR                  *(volatile unsigned long *)(DMA1_BLOCK + 0x44)  // DMA 1 Stream 2 Number of Data Register (1..64k - write only when channel disabled)
    #define DMA1_S2PAR                   *(unsigned long *)(DMA1_BLOCK + 0x48)  // DMA 1 Stream 2 Peripheral Address Register
    #define DMA1_S2M0AR                  *(unsigned long *)(DMA1_BLOCK + 0x4c)  // DMA 1 Stream 2 Memory 0 Address Register
    #define DMA1_S2M1AR                  *(unsigned long *)(DMA1_BLOCK + 0x50)  // DMA 1 Stream 2 Memory 1 Address Register
    #define DMA1_S2FCR                   *(volatile unsigned long *)(DMA1_BLOCK + 0x54)  // DMA 1 Stream 2 FIFO Control Register
    #define DMA1_S3CR                    *(volatile unsigned long *)(DMA1_BLOCK + 0x58)  // DMA 1 Stream 3 Configuration Register
    #define DMA1_S3NDTR                  *(volatile unsigned long *)(DMA1_BLOCK + 0x5c)  // DMA 1 Stream 3 Number of Data Register (1..64k - write only when channel disabled)
    #define DMA1_S3PAR                   *(unsigned long *)(DMA1_BLOCK + 0x60)  // DMA 1 Stream 3 Peripheral Address Register
    #define DMA1_S3M0AR                  *(unsigned long *)(DMA1_BLOCK + 0x64)  // DMA 1 Stream 3 Memory 0 Address Register
    #define DMA1_S3M1AR                  *(unsigned long *)(DMA1_BLOCK + 0x68)  // DMA 1 Stream 3 Memory 1 Address Register
    #define DMA1_S3FCR                   *(volatile unsigned long *)(DMA1_BLOCK + 0x6c)  // DMA 1 Stream 3 FIFO Control Register
    #define DMA1_S4CR                    *(volatile unsigned long *)(DMA1_BLOCK + 0x70) // DMA 1 Stream 4 Configuration Register
    #define DMA1_S4NDTR                  *(volatile unsigned long *)(DMA1_BLOCK + 0x74)  // DMA 1 Stream 4 Number of Data Register (1..64k - write only when channel disabled)
    #define DMA1_S4PAR                   *(unsigned long *)(DMA1_BLOCK + 0x78)  // DMA 1 Stream 4 Peripheral Address Register
    #define DMA1_S4M0AR                  *(unsigned long *)(DMA1_BLOCK + 0x7c)  // DMA 1 Stream 4 Memory 0 Address Register
    #define DMA1_S4M1AR                  *(unsigned long *)(DMA1_BLOCK + 0x80)  // DMA 1 Stream 4 Memory 1 Address Register
    #define DMA1_S4FCR                   *(volatile unsigned long *)(DMA1_BLOCK + 0x84)  // DMA 1 Stream 4 FIFO Control Register
    #define DMA1_S5CR                    *(volatile unsigned long *)(DMA1_BLOCK + 0x88)  // DMA 1 Stream 5 Configuration Register
    #define DMA1_S5NDTR                  *(volatile unsigned long *)(DMA1_BLOCK + 0x8c)  // DMA 1 Stream 5 Number of Data Register (1..64k - write only when channel disabled)
    #define DMA1_S5PAR                   *(unsigned long *)(DMA1_BLOCK + 0x90)  // DMA 1 Stream 5 Peripheral Address Register
    #define DMA1_S5M0AR                  *(unsigned long *)(DMA1_BLOCK + 0x94)  // DMA 1 Stream 5 Memory 0 Address Register
    #define DMA1_S5M1AR                  *(unsigned long *)(DMA1_BLOCK + 0x98)  // DMA 1 Stream 5 Memory 1 Address Register
    #define DMA1_S5FCR                   *(volatile unsigned long *)(DMA1_BLOCK + 0x9c)  // DMA 1 Stream 5 FIFO Control Register
    #define DMA1_S6CR                    *(volatile unsigned long *)(DMA1_BLOCK + 0xa0)  // DMA 1 Stream 6 Configuration Register
    #define DMA1_S6NDTR                  *(volatile unsigned long *)(DMA1_BLOCK + 0xa4)  // DMA 1 Stream 6 Number of Data Register (1..64k - write only when channel disabled)
    #define DMA1_S6PAR                   *(unsigned long *)(DMA1_BLOCK + 0xa8)  // DMA 1 Stream 6 Peripheral Address Register
    #define DMA1_S6M0AR                  *(unsigned long *)(DMA1_BLOCK + 0xac)  // DMA 1 Stream 6 Memory 0 Address Register
    #define DMA1_S6M1AR                  *(unsigned long *)(DMA1_BLOCK + 0xb0)  // DMA 1 Stream 6 Memory 1 Address Register
    #define DMA1_S6FCR                   *(volatile unsigned long *)(DMA1_BLOCK + 0xb4)  // DMA 1 Stream 6 FIFO Control Register
    #define DMA1_S7CR                    *(volatile unsigned long *)(DMA1_BLOCK + 0xb8)  // DMA 1 Stream 7 Configuration Register
    #define DMA1_S7NDTR                  *(volatile unsigned long *)(DMA1_BLOCK + 0xbc)  // DMA 1 Stream 7 Number of Data Register (1..64k - write only when channel disabled)
    #define DMA1_S7PAR                   *(unsigned long *)(DMA1_BLOCK + 0xc0)  // DMA 1 Stream 7 Peripheral Address Register
    #define DMA1_S7M0AR                  *(unsigned long *)(DMA1_BLOCK + 0xc4)  // DMA 1 Stream 7 Memory 0 Address Register
    #define DMA1_S7M1AR                  *(unsigned long *)(DMA1_BLOCK + 0xc8)  // DMA 1 Stream 7 Memory 1 Address Register
    #define DMA1_S7FCR                   *(volatile unsigned long *)(DMA1_BLOCK + 0xcc)  // DMA 1 Stream 7 FIFO Control Register

    #define DMA2_LISR                    *(volatile unsigned long *)(DMA2_BLOCK + 0x00)  // DMA 2 Low Interrupt Status Register (read-only)
    #define DMA2_HISR                    *(volatile unsigned long *)(DMA2_BLOCK + 0x04)  // DMA 2 High Interrupt Status Register (read-only)
    #define DMA2_LIFCR                   *(volatile unsigned long *)(DMA2_BLOCK + 0x08)  // DMA 2 Low Interrupt Flag Clear Register (write-'1' to clear)
    #define DMA2_HIFCR                   *(volatile unsigned long *)(DMA2_BLOCK + 0x0c)  // DMA 2 High Interrupt Flag Clear Register (write-'1' to clear)
    #define DMA2_S0CR                    *(volatile unsigned long *)(DMA2_BLOCK + 0x10)  // DMA 2 Stream 0 Configuration Register
    #define DMA2_S0NDTR                  *(volatile unsigned long *)(DMA2_BLOCK + 0x14)  // DMA 2 Stream 0 Number of Data Register (1..64k - write only when channel disabled)
    #define DMA2_S0PAR                   *(unsigned long *)(DMA2_BLOCK + 0x18)  // DMA 2 Stream 0 Peripheral Address Register
    #define DMA2_S0M0AR                  *(unsigned long *)(DMA2_BLOCK + 0x1c)  // DMA 2 Stream 0 Memory 0 Address Register
    #define DMA2_S0M1AR                  *(unsigned long *)(DMA2_BLOCK + 0x20)  // DMA 2 Stream 0 Memory 1 Address Register
    #define DMA2_S0FCR                   *(volatile unsigned long *)(DMA2_BLOCK + 0x24)  // DMA 2 Stream 0 FIFO Control Register
    #define DMA2_S1CR                    *(volatile unsigned long *)(DMA2_BLOCK + 0x28)  // DMA 2 Stream 1 Configuration Register
    #define DMA2_S1NDTR                  *(volatile unsigned long *)(DMA2_BLOCK + 0x2c)  // DMA 2 Stream 1 Number of Data Register (1..64k - write only when channel disabled)
    #define DMA2_S1PAR                   *(unsigned long *)(DMA2_BLOCK + 0x30)  // DMA 2 Stream 1 Peripheral Address Register
    #define DMA2_S1M0AR                  *(unsigned long *)(DMA2_BLOCK + 0x34)  // DMA 2 Stream 1 Memory 0 Address Register
    #define DMA2_S1M1AR                  *(unsigned long *)(DMA2_BLOCK + 0x38)  // DMA 2 Stream 1 Memory 1 Address Register
    #define DMA2_S1FCR                   *(volatile unsigned long *)(DMA2_BLOCK + 0x3c)  // DMA 2 Stream 1 FIFO Control Register
    #define DMA2_S2CR                    *(volatile unsigned long *)(DMA2_BLOCK + 0x40)  // DMA 2 Stream 2 Configuration Register
    #define DMA2_S2NDTR                  *(volatile unsigned long *)(DMA2_BLOCK + 0x44)  // DMA 2 Stream 2 Number of Data Register (1..64k - write only when channel disabled)
    #define DMA2_S2PAR                   *(unsigned long *)(DMA2_BLOCK + 0x48)  // DMA 2 Stream 2 Peripheral Address Register
    #define DMA2_S2M0AR                  *(unsigned long *)(DMA2_BLOCK + 0x4c)  // DMA 2 Stream 2 Memory 0 Address Register
    #define DMA2_S2M1AR                  *(unsigned long *)(DMA2_BLOCK + 0x50)  // DMA 2 Stream 2 Memory 1 Address Register
    #define DMA2_S2FCR                   *(volatile unsigned long *)(DMA2_BLOCK + 0x54)  // DMA 2 Stream 2 FIFO Control Register
    #define DMA2_S3CR                    *(volatile unsigned long *)(DMA2_BLOCK + 0x58)  // DMA 2 Stream 3 Configuration Register
    #define DMA2_S3NDTR                  *(volatile unsigned long *)(DMA2_BLOCK + 0x5c)  // DMA 2 Stream 3 Number of Data Register (1..64k - write only when channel disabled)
    #define DMA2_S3PAR                   *(unsigned long *)(DMA2_BLOCK + 0x60)  // DMA 2 Stream 3 Peripheral Address Register
    #define DMA2_S3M0AR                  *(unsigned long *)(DMA2_BLOCK + 0x64)  // DMA 2 Stream 3 Memory 0 Address Register
    #define DMA2_S3M1AR                  *(unsigned long *)(DMA2_BLOCK + 0x68)  // DMA 2 Stream 3 Memory 1 Address Register
    #define DMA2_S3FCR                   *(volatile unsigned long *)(DMA2_BLOCK + 0x6c)  // DMA 2 Stream 3 FIFO Control Register
    #define DMA2_S4CR                    *(volatile unsigned long *)(DMA2_BLOCK + 0x70) // DMA 2 Stream 4 Configuration Register
    #define DMA2_S4NDTR                  *(volatile unsigned long *)(DMA2_BLOCK + 0x74)  // DMA 2 Stream 4 Number of Data Register (1..64k - write only when channel disabled)
    #define DMA2_S4PAR                   *(unsigned long *)(DMA2_BLOCK + 0x78)  // DMA 2 Stream 4 Peripheral Address Register
    #define DMA2_S4M0AR                  *(unsigned long *)(DMA2_BLOCK + 0x7c)  // DMA 2 Stream 4 Memory 0 Address Register
    #define DMA2_S4M1AR                  *(unsigned long *)(DMA2_BLOCK + 0x80)  // DMA 2 Stream 4 Memory 1 Address Register
    #define DMA2_S4FCR                   *(volatile unsigned long *)(DMA2_BLOCK + 0x84)  // DMA 2 Stream 4 FIFO Control Register
    #define DMA2_S5CR                    *(volatile unsigned long *)(DMA2_BLOCK + 0x88)  // DMA 2 Stream 5 Configuration Register
    #define DMA2_S5NDTR                  *(volatile unsigned long *)(DMA2_BLOCK + 0x8c)  // DMA 2 Stream 5 Number of Data Register (1..64k - write only when channel disabled)
    #define DMA2_S5PAR                   *(unsigned long *)(DMA2_BLOCK + 0x90)  // DMA 2 Stream 5 Peripheral Address Register
    #define DMA2_S5M0AR                  *(unsigned long *)(DMA2_BLOCK + 0x94)  // DMA 2 Stream 5 Memory 0 Address Register
    #define DMA2_S5M1AR                  *(unsigned long *)(DMA2_BLOCK + 0x98)  // DMA 2 Stream 5 Memory 1 Address Register
    #define DMA2_S5FCR                   *(volatile unsigned long *)(DMA2_BLOCK + 0x9c)  // DMA 2 Stream 5 FIFO Control Register
    #define DMA2_S6CR                    *(volatile unsigned long *)(DMA2_BLOCK + 0xa0)  // DMA 2 Stream 6 Configuration Register
    #define DMA2_S6NDTR                  *(volatile unsigned long *)(DMA2_BLOCK + 0xa4)  // DMA 2 Stream 6 Number of Data Register (1..64k - write only when channel disabled)
    #define DMA2_S6PAR                   *(unsigned long *)(DMA2_BLOCK + 0xa8)  // DMA 2 Stream 6 Peripheral Address Register
    #define DMA2_S6M0AR                  *(unsigned long *)(DMA2_BLOCK + 0xac)  // DMA 2 Stream 6 Memory 0 Address Register
    #define DMA2_S6M1AR                  *(unsigned long *)(DMA2_BLOCK + 0xb0)  // DMA 2 Stream 6 Memory 1 Address Register
    #define DMA2_S6FCR                   *(volatile unsigned long *)(DMA2_BLOCK + 0xb4)  // DMA 2 Stream 6 FIFO Control Register
    #define DMA2_S7CR                    *(volatile unsigned long *)(DMA2_BLOCK + 0xb8)  // DMA 2 Stream 7 Configuration Register
    #define DMA2_S7NDTR                  *(volatile unsigned long *)(DMA2_BLOCK + 0xbc)  // DMA 2 Stream 7 Number of Data Register (1..64k - write only when channel disabled)
    #define DMA2_S7PAR                   *(unsigned long *)(DMA2_BLOCK + 0xc0)  // DMA 2 Stream 7 Peripheral Address Register
    #define DMA2_S7M0AR                  *(unsigned long *)(DMA2_BLOCK + 0xc4)  // DMA 2 Stream 7 Memory 0 Address Register
    #define DMA2_S7M1AR                  *(unsigned long *)(DMA2_BLOCK + 0xc8)  // DMA 2 Stream 7 Memory 1 Address Register
    #define DMA2_S7FCR                   *(volatile unsigned long *)(DMA2_BLOCK + 0xcc)  // DMA 2 Stream 7 FIFO Control Register
#else
    #define DMA1_ISR                     *(volatile unsigned long *)(DMA1_BLOCK + 0x00)  // DMA Interrupt Status Register
      #define DMA1_ISR_GIF1              0x00000001                             // global interrupt channel 1
      #define DMA1_ISR_TCIF1             0x00000002                             // transfer complete channel 1
      #define DMA1_ISR_HTIF1             0x00000004                             // half transfer complete channel 1
      #define DMA1_ISR_TEIF1             0x00000008                             // transfer error channel 1
      #define DMA1_ISR_GIF2              0x00000010                             // global interrupt channel 2
      #define DMA1_ISR_TCIF2             0x00000020                             // transfer complete channel 2
      #define DMA1_ISR_HTIF2             0x00000040                             // half transfer complete channel 2
      #define DMA1_ISR_TEIF2             0x00000080                             // transfer error channel 2
      #define DMA1_ISR_GIF3              0x00000100                             // global interrupt channel 3
      #define DMA1_ISR_TCIF3             0x00000200                             // transfer complete channel 3
      #define DMA1_ISR_HTIF3             0x00000400                             // half transfer complete channel 3
      #define DMA1_ISR_TEIF3             0x00000800                             // transfer error channel 3
      #define DMA1_ISR_GIF4              0x00001000                             // global interrupt channel 4
      #define DMA1_ISR_TCIF4             0x00002000                             // transfer complete channel 4
      #define DMA1_ISR_HTIF4             0x00004000                             // half transfer complete channel 4
      #define DMA1_ISR_TEIF4             0x00008000                             // transfer error channel 4
      #define DMA1_ISR_GIF5              0x00010000                             // global interrupt channel 5
      #define DMA1_ISR_TCIF5             0x00020000                             // transfer complete channel 5
      #define DMA1_ISR_HTIF5             0x00040000                             // half transfer complete channel 5
      #define DMA1_ISR_TEIF5             0x00080000                             // transfer error channel 5
      #define DMA1_ISR_GIF6              0x00100000                             // global interrupt channel 6
      #define DMA1_ISR_TCIF6             0x00200000                             // transfer complete channel 6
      #define DMA1_ISR_HTIF6             0x00400000                             // half transfer complete channel 6
      #define DMA1_ISR_TEIF6             0x00800000                             // transfer error channel 6
      #define DMA1_ISR_GIF7              0x01000000                             // global interrupt channel 7
      #define DMA1_ISR_TCIF7             0x02000000                             // transfer complete channel 7
      #define DMA1_ISR_HTIF7             0x04000000                             // half transfer complete channel 7
      #define DMA1_ISR_TEIF7             0x08000000                             // transfer error channel 7
    #define DMA1_IFCR                    *(volatile unsigned long *)(DMA1_BLOCK + 0x04)  // DMA Interrupt Flag Clear Register
    #define DMA1_CCR1                    *(unsigned long *)(DMA1_BLOCK + 0x08)  // DMA Channel 1 Configuration Register
      #define DMA1_CCR1_EN               0x00000001                             // channel enable
      #define DMA1_CCR1_TCIE             0x00000002                             // transfer complete interrupt enable
      #define DMA1_CCR1_HTIE             0x00000004                             // half transfer interrupt enable
      #define DMA1_CCR1_TEIE             0x00000008                             // transfer error interrupt enable
      #define DMA1_CCR1_DIR              0x00000010                             // transfer mode = read from memory
      #define DMA1_CCR1_CIRC             0x00000020                             // circular mode
      #define DMA1_CCR1_PINC             0x00000040                             // peripheral increment mode
      #define DMA1_CCR1_MINC             0x00000080                             // memory increment mode
      #define DMA1_CCR1_PSIZE_8          0x00000000                             // peripheral size 8 bit
      #define DMA1_CCR1_PSIZE_16         0x00000100                             // peripheral size 16 bit
      #define DMA1_CCR1_PSIZE_32         0x00000200                             // peripheral size 32 bit
      #define DMA1_CCR1_MSIZE_8          0x00000000                             // memory size 8 bit
      #define DMA1_CCR1_MSIZE_16         0x00000400                             // memory size 16 bit
      #define DMA1_CCR1_MSIZE_32         0x00000800                             // memory size 32 bit
      #define DMA1_CCR1_PL_LOW           0x00000000                             // channel low priority
      #define DMA1_CCR1_PL_MEDIUM        0x00001000                             // channel medium priority
      #define DMA1_CCR1_PL_HIGH          0x00002000                             // channel high priority
      #define DMA1_CCR1_PL_VERY_HIGH     0x00003000                             // channel very high priority
      #define DMA1_CCR1_MEM2MEM          0x00004000                             // memory to memory mode enabled
    #define DMA1_CNDTR1                  *(volatile unsigned long *)(DMA1_BLOCK + 0x0c)  // DMA Channel 1 Number of Data Register (1..64k - write only when channel disabled)
    #define DMA1_CPAR1                   *(unsigned long *)(DMA1_BLOCK + 0x10)  // DMA Channel 1 Peripheral Address Register
    #define DMA1_CMAR1                   *(unsigned long *)(DMA1_BLOCK + 0x14)  // DMA Channel 1 Memory Address Register
    #define DMA1_CCR2                    *(unsigned long *)(DMA1_BLOCK + 0x1c)  // DMA Channel 2 Configuration Register
    #define DMA1_CNDTR2                  *(volatile unsigned long *)(DMA1_BLOCK + 0x20)  // DMA Channel 2 Number of Data Register
    #define DMA1_CPAR2                   *(unsigned long *)(DMA1_BLOCK + 0x24)  // DMA Channel 2 Peripheral Address Register
    #define DMA1_CMAR2                   *(unsigned long *)(DMA1_BLOCK + 0x28)  // DMA Channel 2 Memory Address Register
    #define DMA1_CCR3                    *(unsigned long *)(DMA1_BLOCK + 0x30)  // DMA Channel 3 Configuration Register
    #define DMA1_CNDTR3                  *(volatile unsigned long *)(DMA1_BLOCK + 0x34)  // DMA Channel 3 Number of Data Register
    #define DMA1_CPAR3                   *(unsigned long *)(DMA1_BLOCK + 0x38)  // DMA Channel 3 Peripheral Address Register
    #define DMA1_CMAR3                   *(unsigned long *)(DMA1_BLOCK + 0x3c)  // DMA Channel 3 Memory Address Register
    #define DMA1_CCR4                    *(unsigned long *)(DMA1_BLOCK + 0x44)  // DMA Channel 4 Configuration Register
    #define DMA1_CNDTR4                  *(volatile unsigned long *)(DMA1_BLOCK + 0x48)  // DMA Channel 4 Number of Data Register
    #define DMA1_CPAR4                   *(unsigned long *)(DMA1_BLOCK + 0x4c)  // DMA Channel 4 Peripheral Address Register
    #define DMA1_CMAR4                   *(unsigned long *)(DMA1_BLOCK + 0x50)  // DMA Channel 4 Memory Address Register
    #define DMA1_CCR5                    *(unsigned long *)(DMA1_BLOCK + 0x58)  // DMA Channel 5 Configuration Register
    #define DMA1_CNDTR5                  *(volatile unsigned long *)(DMA1_BLOCK + 0x5c)  // DMA Channel 5 Number of Data Register
    #define DMA1_CPAR5                   *(unsigned long *)(DMA1_BLOCK + 0x60)  // DMA Channel 5 Peripheral Address Register
    #define DMA1_CMAR5                   *(unsigned long *)(DMA1_BLOCK + 0x64)  // DMA Channel 5 Memory Address Register
    #define DMA1_CCR6                    *(unsigned long *)(DMA1_BLOCK + 0x6c)  // DMA Channel 6 Configuration Register
    #define DMA1_CNDTR6                  *(volatile unsigned long *)(DMA1_BLOCK + 0x70)  // DMA Channel 6 Number of Data Register
    #define DMA1_CPAR6                   *(unsigned long *)(DMA1_BLOCK + 0x74)  // DMA Channel 6 Peripheral Address Register
    #define DMA1_CMAR6                   *(unsigned long *)(DMA1_BLOCK + 0x78)  // DMA Channel 6 Memory Address Register
    #define DMA1_CCR7                    *(unsigned long *)(DMA1_BLOCK + 0x80)  // DMA Channel 7 Configuration Register
    #define DMA1_CNDTR7                  *(volatile unsigned long *)(DMA1_BLOCK + 0x84)  // DMA Channel 7 Number of Data Register
    #define DMA1_CPAR7                   *(unsigned long *)(DMA1_BLOCK + 0x88)  // DMA Channel 7 Peripheral Address Register
    #define DMA1_CMAR7                   *(unsigned long *)(DMA1_BLOCK + 0x8c)  // DMA Channel 7 Memory Address Register

    #define DMA2_ISR                     *(volatile unsigned long *)(DMA2_BLOCK + 0x00)  // DMA Interrupt Status Register
    #define DMA2_IFCR                    *(volatile unsigned long *)(DMA2_BLOCK + 0x04)  // DMA Interrupt Flag Clear Register
    #define DMA2_CCR1                    *(unsigned long *)(DMA2_BLOCK + 0x08)  // DMA Channel 1 Configuration Register
    #define DMA2_CNDTR1                  *(volatile unsigned long *)(DMA2_BLOCK + 0x0c)  // DMA Channel 1 Number of Data Register
    #define DMA2_CPAR1                   *(unsigned long *)(DMA2_BLOCK + 0x10)  // DMA Channel 1 Peripheral Address Register
    #define DMA2_CMAR1                   *(unsigned long *)(DMA2_BLOCK + 0x14)  // DMA Channel 1 Memory Address Register
    #define DMA2_CCR2                    *(unsigned long *)(DMA2_BLOCK + 0x1c)  // DMA Channel 2 Configuration Register
    #define DMA2_CNDTR2                  *(volatile unsigned long *)(DMA2_BLOCK + 0x20)  // DMA Channel 2 Number of Data Register
    #define DMA2_CPAR2                   *(unsigned long *)(DMA2_BLOCK + 0x24)  // DMA Channel 2 Peripheral Address Register
    #define DMA2_CMAR2                   *(unsigned long *)(DMA2_BLOCK + 0x28)  // DMA Channel 2 Memory Address Register
    #define DMA2_CCR3                    *(unsigned long *)(DMA2_BLOCK + 0x30)  // DMA Channel 3 Configuration Register
    #define DMA2_CNDTR3                  *(volatile unsigned long *)(DMA2_BLOCK + 0x34)  // DMA Channel 3 Number of Data Register
    #define DMA2_CPAR3                   *(unsigned long *)(DMA2_BLOCK + 0x38)  // DMA Channel 3 Peripheral Address Register
    #define DMA2_CMAR3                   *(unsigned long *)(DMA2_BLOCK + 0x3c)  // DMA Channel 3 Memory Address Register
    #define DMA2_CCR4                    *(unsigned long *)(DMA2_BLOCK + 0x44)  // DMA Channel 4 Configuration Register
    #define DMA2_CNDTR4                  *(volatile unsigned long *)(DMA2_BLOCK + 0x48)  // DMA Channel 4 Number of Data Register
    #define DMA2_CPAR4                   *(unsigned long *)(DMA2_BLOCK + 0x4c)  // DMA Channel 4 Peripheral Address Register
    #define DMA2_CMAR4                   *(unsigned long *)(DMA2_BLOCK + 0x50)  // DMA Channel 4 Memory Address Register
    #define DMA2_CCR5                    *(unsigned long *)(DMA2_BLOCK + 0x58)  // DMA Channel 5 Configuration Register
    #define DMA2_CNDTR5                  *(volatile unsigned long *)(DMA2_BLOCK + 0x5c)  // DMA Channel 5 Number of Data Register
    #define DMA2_CPAR5                   *(unsigned long *)(DMA2_BLOCK + 0x60)  // DMA Channel 5 Peripheral Address Register
    #define DMA2_CMAR5                   *(unsigned long *)(DMA2_BLOCK + 0x64)  // DMA Channel 5 Memory Address Register
    #define DMA2_CCR6                    *(unsigned long *)(DMA2_BLOCK + 0x68)  // DMA Channel 6 Configuration Register
    #define DMA2_CNDTR6                  *(volatile unsigned long *)(DMA2_BLOCK + 0x6c)  // DMA Channel 6 Number of Data Register
    #define DMA2_CPAR6                   *(unsigned long *)(DMA2_BLOCK + 0x70)  // DMA Channel 6 Peripheral Address Register
    #define DMA2_CMAR6                   *(unsigned long *)(DMA2_BLOCK + 0x74)  // DMA Channel 6 Memory Address Register
    #define DMA2_CCR7                    *(unsigned long *)(DMA2_BLOCK + 0x78)  // DMA Channel 7 Configuration Register
    #define DMA2_CNDTR7                  *(volatile unsigned long *)(DMA2_BLOCK + 0x7c)  // DMA Channel 7 Number of Data Register
    #define DMA2_CPAR7                   *(unsigned long *)(DMA2_BLOCK + 0x80)  // DMA Channel 7 Peripheral Address Register
    #define DMA2_CMAR7                   *(unsigned long *)(DMA2_BLOCK + 0x84)  // DMA Channel 7 Memory Address Register
#endif

// DMA sources
//
#define DMA1_CHANNEL_1_ADC1
#define DMA1_CHANNEL_1_TIM2_CH3
#define DMA1_CHANNEL_1_TIM4_CH1
#define DMA1_CHANNEL_2_SPI1_RX
#define DMA1_CHANNEL_2_USART3_TX
#define DMA1_CHANNEL_2_TIM1_CH1
#define DMA1_CHANNEL_2_TIM2_UP
#define DMA1_CHANNEL_2_TIM3_CH3
#define DMA1_CHANNEL_3_SPI1_TX
#define DMA1_CHANNEL_3_USART3_RX
#define DMA1_CHANNEL_3_TIM1_CH2
#define DMA1_CHANNEL_3_TIM3_CH4
#define DMA1_CHANNEL_3_TIM3_UP
#define DMA1_CHANNEL_4_SPI2_I2S2_RX
#define DMA1_CHANNEL_4_USART1_TX
#define DMA1_CHANNEL_4_I2C2_TX
#define DMA1_CHANNEL_4_TIM1_CH4
#define DMA1_CHANNEL_4_TIM1_TRIG
#define DMA1_CHANNEL_4_TIM1_COM
#define DMA1_CHANNEL_4_TIM4_CH2
#define DMA1_CHANNEL_5_SPI2_I2S2_TX
#define DMA1_CHANNEL_5_USART1_RX
#define DMA1_CHANNEL_5_I2C2_RX
#define DMA1_CHANNEL_5_TIM1_UP
#define DMA1_CHANNEL_5_TIM2_CH1
#define DMA1_CHANNEL_5_TIM4_CH3
#define DMA1_CHANNEL_6_USART2_RX
#define DMA1_CHANNEL_6_I2C1_TX
#define DMA1_CHANNEL_6_TIM1_CH3
#define DMA1_CHANNEL_6_TIM3_CH1
#define DMA1_CHANNEL_6_TIM3_TRIG
#define DMA1_CHANNEL_7_USART2_TX
#define DMA1_CHANNEL_7_I2C1_RX
#define DMA1_CHANNEL_7_TIM2_CH2
#define DMA1_CHANNEL_7_TIM2_CH4
#define DMA1_CHANNEL_7_TIM4_UP

#define DMA2_CHANNEL_1_SPI_I2S3_RX
#define DMA2_CHANNEL_1_TIM5_CH4
#define DMA2_CHANNEL_1_TIM5_TRIG
#define DMA2_CHANNEL_1_TIM8_CH3
#define DMA2_CHANNEL_1_TIM8_UP
#define DMA2_CHANNEL_2_SPI_I2S3_TX
#define DMA2_CHANNEL_2_TIM5_CH3
#define DMA2_CHANNEL_2_TIM5_UP
#define DMA2_CHANNEL_2_TIM8_CH4
#define DMA2_CHANNEL_2_TIM8_TRIG
#define DMA2_CHANNEL_2_TIM8_COM
#define DMA2_CHANNEL_3_UART4_RX
#define DMA2_CHANNEL_3_TIM6_UP
#define DMA2_CHANNEL_3_DAC_Channel1
#define DMA2_CHANNEL_3_TIM8_CH1
#define DMA2_CHANNEL_4_SDIO
#define DMA2_CHANNEL_4_TIM5_CH2
#define DMA2_CHANNEL_4_TIM7_UP
#define DMA2_CHANNEL_4_DAC_Channel2
#define DMA2_CHANNEL_5_ADC3
#define DMA2_CHANNEL_5_UART4_TX
#define DMA2_CHANNEL_5_TIM5_CH1
#define DMA2_CHANNEL_5_TIM8_CH2

// ADC                                                                   {22}
//
#define ADC1_SR                          *(volatile unsigned long *)(ADC_BLOCK + 0x000) // ADC1 status register (set by hw but an be written to '0' by software)
    #define ADC_SR_AWD                   0x00000001                      // analog watchdog event occurred
    #define ADC_SR_EOC                   0x00000002                      // regular conversion or regular sequence complete
    #define ADC_SR_JEOC                  0x00000004                      // conversion of all injected channels complete
    #define ADC_SR_JSTRT                 0x00000008                      // injected group conversion has started
    #define ADC_SR_STRT                  0x00000010                      // regular channel conversion has started
    #define ADC_SR_OVR                   0x00000020                      // overrun has occurred
#define ADC1_CR1                         *(volatile unsigned long *)(ADC_BLOCK + 0x004) // ADC1 control register 1
    #define ADC_CR1_AWDCH_CH0            0x00000000                      // analoge watchdog channel select - input channel 0 (valid when ADC_CR1_AWDSGL_SINGLE is '1')
    #define ADC_CR1_AWDCH_CH1            0x00000001                      // analoge watchdog channel select - input channel 1
    #define ADC_CR1_AWDCH_CH2            0x00000002                      // analoge watchdog channel select - input channel 2
    #define ADC_CR1_AWDCH_CH3            0x00000003                      // analoge watchdog channel select - input channel 3
    #define ADC_CR1_AWDCH_CH4            0x00000004                      // analoge watchdog channel select - input channel 4
    #define ADC_CR1_AWDCH_CH5            0x00000005                      // analoge watchdog channel select - input channel 5
    #define ADC_CR1_AWDCH_CH6            0x00000006                      // analoge watchdog channel select - input channel 6
    #define ADC_CR1_AWDCH_CH7            0x00000007                      // analoge watchdog channel select - input channel 7
    #define ADC_CR1_AWDCH_CH8            0x00000008                      // analoge watchdog channel select - input channel 8
    #define ADC_CR1_AWDCH_CH9            0x00000009                      // analoge watchdog channel select - input channel 9
    #define ADC_CR1_AWDCH_CH10           0x0000000a                      // analoge watchdog channel select - input channel 10
    #define ADC_CR1_AWDCH_CH11           0x0000000b                      // analoge watchdog channel select - input channel 11
    #define ADC_CR1_AWDCH_CH12           0x0000000c                      // analoge watchdog channel select - input channel 12
    #define ADC_CR1_AWDCH_CH13           0x0000000d                      // analoge watchdog channel select - input channel 13
    #define ADC_CR1_AWDCH_CH14           0x0000000e                      // analoge watchdog channel select - input channel 14
    #define ADC_CR1_AWDCH_CH15           0x0000000f                      // analoge watchdog channel select - input channel 15
    #define ADC_CR1_AWDCH_CH16           0x00000010                      // analoge watchdog channel select - input channel 16
    #define ADC_CR1_AWDCH_CH17           0x00000011                      // analoge watchdog channel select - input channel 17
    #define ADC_CR1_AWDCH_CH18           0x00000012                      // analoge watchdog channel select - input channel 18
    #define ADC_CR1_AWDCH_MASK           0x0000001f                      // analoge watchdog channel select mask
    #define ADC_CR1_EOCIE                0x00000020                      // end of conversion/sequence interrupt enable
    #define ADC_CR1_AWDIE                0x00000040                      // analog watchdog interrupt enable
    #define ADC_CR1_JEOCIE               0x00000080                      // interrupt enable for injected channels
    #define ADC_CR1_SCAN                 0x00000100                      // scan mode enabled
    #define ADC_CR1_AWDSGL_ALL           0x00000000                      // watchdog on all channels in scan mode
    #define ADC_CR1_AWDSGL_SINGLE        0x00000200                      // watchdog on a single channel in scan mode (identified by ADC_CR1_AWDCH[4:0])
    #define ADC_CR1_JAUTO                0x00000400                      // automatic injected group conversion
    #define ADC_CR1_DISCEN               0x00000800                      // discontinuous mode on regular channels enabled
    #define ADC_CR1_JDISCEN              0x00001000                      // discontinuous mode on injected channels enabled
    #define ADC_CR1_DISCNUM_1            0x00000000                      // discontinuous mode channel count - 1
    #define ADC_CR1_DISCNUM_2            0x00002000                      // discontinuous mode channel count - 2
    #define ADC_CR1_DISCNUM_3            0x00004000                      // discontinuous mode channel count - 3
    #define ADC_CR1_DISCNUM_4            0x00006000                      // discontinuous mode channel count - 4
    #define ADC_CR1_DISCNUM_5            0x00008000                      // discontinuous mode channel count - 5
    #define ADC_CR1_DISCNUM_6            0x0000a000                      // discontinuous mode channel count - 6
    #define ADC_CR1_DISCNUM_7            0x0000c000                      // discontinuous mode channel count - 7
    #define ADC_CR1_DISCNUM_8            0x0000e000                      // discontinuous mode channel count - 8
    #define ADC_CR1_JAWDEN               0x00400000                      // analog watchdog enable on injected channels
    #define ADC_CR1_AWDEN                0x00800000                      // analog watchdog enabled on regular channels
    #define ADC_CR1_RES_12_BIT           0x00000000                      // 12-bit resolution (requires 15 ADCCLK cycles)
    #define ADC_CR1_RES_10_BIT           0x01000000                      // 10-bit resolution (requires 13 ADCCLK cycles)
    #define ADC_CR1_RES_8_BIT            0x02000000                      // 8-bit resolution (requires 11 ADCCLK cycles)
    #define ADC_CR1_RES_6_BIT            0x03000000                      // 6-bit resolution (requires 9 ADCCLK cycles)
    #define ADC_CR1_OVRIE                0x04000000                      // overrun interrupt enable
#define ADC1_CR2                         *(volatile unsigned long *)(ADC_BLOCK + 0x008) // ADC1 control register 2
    #define ADC_CR2_ADON                 0x00000001                      // enable ADC
    #define ADC_CR2_SINGLE               0x00000000                      // single conversion mode
    #define ADC_CR2_CONT                 0x00000002                      // continuous conversion mode
    #define ADC_CR2_DMA                  0x00000100                      // DMA mode (for single ADC mode) enabled
    #define ADC_CR2_DDS                  0x00000200                      // DMA disable selection (for single ADC mode) - DMA requests are issued as long as data are converted and DMA is '1'
    #define ADC_CR2_EOCS_SEQUENCE        0x00000000                      // end of conversion selection - EOC set at the end of each sequence of regular conversions
    #define ADC_CR2_EOCS_CONVERSION      0x00000400                      // end of conversion selection - EOC set at the end of each regular conversion
    #define ADC_CR2_ALIGN_RIGHT          0x00000000                      // data alignment - right
    #define ADC_CR2_ALIGN_LEFT           0x00000800                      // data alignment - left
    #define ADC_CR2_JEXTSEL_TIMER1_CC1   0x00000000                      // external event selection for injected group - timer 1 CC1 event
    #define ADC_CR2_JEXTSEL_TIMER1_CC2   0x00010000                      // external event selection for injected group - timer 1 CC2 event
    #define ADC_CR2_JEXTSEL_TIMER1_CC3   0x00020000                      // external event selection for injected group - timer 1 CC3 event
    #define ADC_CR2_JEXTSEL_TIMER2_CC2   0x00030000                      // external event selection for injected group - timer 2 CC2 event
    #define ADC_CR2_JEXTSEL_TIMER2_CC3   0x00040000                      // external event selection for injected group - timer 2 CC3 event
    #define ADC_CR2_JEXTSEL_TIMER2_CC4   0x00050000                      // external event selection for injected group - timer 2 CC4 event
    #define ADC_CR2_JEXTSEL_TIMER2_TRGO  0x00060000                      // external event selection for injected group - timer 2 TRGO event
    #define ADC_CR2_JEXTSEL_TIMER3_CC1   0x00070000                      // external event selection for injected group - timer 3 CC1 event
    #define ADC_CR2_JEXTSEL_TIMER3_TRGO  0x00080000                      // external event selection for injected group - timer 3 TRGO event
    #define ADC_CR2_JEXTSEL_TIMER4_CC4   0x00090000                      // external event selection for injected group - timer 4 CC4 event
    #define ADC_CR2_JEXTSEL_TIMER5_CC1   0x000a0000                      // external event selection for injected group - timer 5 CC1 event
    #define ADC_CR2_JEXTSEL_TIMER5_CC2   0x000b0000                      // external event selection for injected group - timer 5 CC2 event
    #define ADC_CR2_JEXTSEL_TIMER5_CC3   0x000c0000                      // external event selection for injected group - timer 5 CC3 event
    #define ADC_CR2_JEXTSEL_TIMER8_CC1   0x000d0000                      // external event selection for injected group - timer 8 CC1 event
    #define ADC_CR2_JEXTSEL_TIMER8_TRGO  0x000e0000                      // external event selection for injected group - timer 8 TRGO event
    #define ADC_CR2_JEXTSEL_EXTI_15      0x000f0000                      // external event selection for injected group - EXTI line 15
    #define ADC_CR2_JEXTEN_RISING        0x00100000                      // external trigger enable for regular channels - rising edge
    #define ADC_CR2_JEXTEN_FALLING       0x00200000                      // external trigger enable for regular channels - falling edge
    #define ADC_CR2_JEXTEN_BOTH_EDGES    0x00300000                      // external trigger enable for regular channels - both edges
    #define ADC_CR2_JSWSTART             0x00400000                      // start conversion of injected channels  (can only be set when ADC_CR2_ADON is '1')
    #define ADC_CR2_EXTSEL_TIMER1_CC1    0x00000000                      // external event selection for regular group - timer 1 CC1 event
    #define ADC_CR2_EXTSEL_TIMER1_CC2    0x01000000                      // external event selection for regular group - timer 1 CC2 event
    #define ADC_CR2_EXTSEL_TIMER1_CC3    0x02000000                      // external event selection for regular group - timer 1 CC3 event
    #define ADC_CR2_EXTSEL_TIMER2_CC2    0x03000000                      // external event selection for regular group - timer 2 CC2 event
    #define ADC_CR2_EXTSEL_TIMER2_CC3    0x04000000                      // external event selection for regular group - timer 2 CC3 event
    #define ADC_CR2_EXTSEL_TIMER2_CC4    0x05000000                      // external event selection for regular group - timer 2 CC4 event
    #define ADC_CR2_EXTSEL_TIMER2_TRGO   0x06000000                      // external event selection for regular group - timer 2 TRGO event
    #define ADC_CR2_EXTSEL_TIMER3_CC1    0x07000000                      // external event selection for regular group - timer 3 CC1 event
    #define ADC_CR2_EXTSEL_TIMER3_TRGO   0x08000000                      // external event selection for regular group - timer 3 TRGO event
    #define ADC_CR2_EXTSEL_TIMER4_CC4    0x09000000                      // external event selection for regular group - timer 4 CC4 event
    #define ADC_CR2_EXTSEL_TIMER5_CC1    0x0a000000                      // external event selection for regular group - timer 5 CC1 event
    #define ADC_CR2_EXTSEL_TIMER5_CC2    0x0b000000                      // external event selection for regular group - timer 5 CC2 event
    #define ADC_CR2_EXTSEL_TIMER5_CC3    0x0c000000                      // external event selection for regular group - timer 5 CC3 event
    #define ADC_CR2_EXTSEL_TIMER8_CC1    0x0d000000                      // external event selection for regular group - timer 8 CC1 event
    #define ADC_CR2_EXTSEL_TIMER8_TRGO   0x0e000000                      // external event selection for regular group - timer 8 TRGO event
    #define ADC_CR2_EXTSEL_EXTI_11       0x0f000000                      // external event selection for regular group - EXTI line 11
    #define ADC_CR2_EXTEN_RISING         0x10000000                      // external trigger enable for regular channels - rising edge
    #define ADC_CR2_EXTEN_FALLING        0x20000000                      // external trigger enable for regular channels - falling edge
    #define ADC_CR2_EXTEN_BOTH_EDGES     0x30000000                      // external trigger enable for regular channels - both edges
    #define ADC_CR2_SWSTART              0x40000000                      // start conversion of regular channels (can only be set when ADC_CR2_ADON is '1' - cleared by the HW as soon as the conversion starts)
#define ADC1_SMPR1                       *(volatile unsigned long *)(ADC_BLOCK + 0x00c) // ADC1 sample time register 1
    #define ADC_SMPR1_SMP10_MASK         0x00000007                      // channel 10 sampling time selection mask
    #define ADC_SMPR1_SMP11_MASK         0x00000038                      // channel 11 sampling time selection mask
    #define ADC_SMPR1_SMP12_MASK         0x000001c0                      // channel 12 sampling time selection mask
    #define ADC_SMPR1_SMP13_MASK         0x00000e00                      // channel 13 sampling time selection mask
    #define ADC_SMPR1_SMP14_MASK         0x00007000                      // channel 14 sampling time selection mask
    #define ADC_SMPR1_SMP15_MASK         0x00038000                      // channel 15 sampling time selection mask
    #define ADC_SMPR1_SMP16_MASK         0x001c0000                      // channel 16 sampling time selection mask
    #define ADC_SMPR1_SMP17_MASK         0x00e00000                      // channel 17 sampling time selection mask
    #define ADC_SMPR1_SMP18_MASK         0x07000000                      // channel 18 sampling time selection mask
    #define ADC_SMPR_3_CYCLES            0x0                             // 3 cycles sampling time
    #define ADC_SMPR_15_CYCLES           0x1                             // 15 cycles sampling time
    #define ADC_SMPR_28_CYCLES           0x2                             // 28 cycles sampling time
    #define ADC_SMPR_56_CYCLES           0x3                             // 56 cycles sampling time
    #define ADC_SMPR_84_CYCLES           0x4                             // 84 cycles sampling time
    #define ADC_SMPR_122_CYCLES          0x5                             // 112 cycles sampling time
    #define ADC_SMPR_144_CYCLES          0x6                             // 144 cycles sampling time
    #define ADC_SMPR_480_CYCLES          0x7                             // 480 cycles sampling time
#define ADC1_SMPR2                       *(volatile unsigned long *)(ADC_BLOCK + 0x010) // ADC1 sample time register 2
    #define ADC_SMPR2_SMP0_MASK          0x00000007                      // channel 0 sampling time selection mask
    #define ADC_SMPR2_SMP1_MASK          0x00000038                      // channel 1 sampling time selection mask
    #define ADC_SMPR2_SMP2_MASK          0x000001c0                      // channel 2 sampling time selection mask
    #define ADC_SMPR2_SMP3_MASK          0x00000e00                      // channel 3 sampling time selection mask
    #define ADC_SMPR2_SMP4_MASK          0x00007000                      // channel 4 sampling time selection mask
    #define ADC_SMPR2_SMP5_MASK          0x00038000                      // channel 5 sampling time selection mask
    #define ADC_SMPR2_SMP6_MASK          0x001c0000                      // channel 6 sampling time selection mask
    #define ADC_SMPR2_SMP7_MASK          0x00e00000                      // channel 7 sampling time selection mask
    #define ADC_SMPR2_SMP8_MASK          0x07000000                      // channel 8 sampling time selection mask
    #define ADC_SMPR2_SMP9_MASK          0x38000000                      // channel 9 sampling time selection mask
#define ADC1_JOFR1                       *(volatile unsigned long *)(ADC_BLOCK + 0x014) // ADC1 injected channel data offset register 1
#define ADC1_JOFR2                       *(volatile unsigned long *)(ADC_BLOCK + 0x018) // ADC1 injected channel data offset register 2
#define ADC1_JOFR3                       *(volatile unsigned long *)(ADC_BLOCK + 0x01c) // ADC1 injected channel data offset register 3
#define ADC1_JOFR4                       *(volatile unsigned long *)(ADC_BLOCK + 0x020) // ADC1 injected channel data offset register 4
#define ADC1_HTR                         *(volatile unsigned long *)(ADC_BLOCK + 0x024) // ADC1 watchdog higher threshold register
#define ADC1_LTR                         *(volatile unsigned long *)(ADC_BLOCK + 0x028) // ADC1 watchdog lower threshold register
#define ADC1_SQR1                        *(volatile unsigned long *)(ADC_BLOCK + 0x02c) // ADC1 regular sequence register 1
    #define ADC_SQR1_SQ13_MASK           0x0000001f                      // 13th conversion channel number in regular sequence
    #define ADC_SQR1_SQ14_MASK           0x0000003e                      // 14th conversion channel number in regular sequence
    #define ADC_SQR1_SQ15_MASK           0x000007c0                      // 15th conversion channel number in regular sequence
    #define ADC_SQR1_SQ16_MASK           0x000f8000                      // 16th conversion channel number in regular sequence
    #define ADC_SQR1_L_MASK              0x00f00000                      // regular channel sequence length mask
    #define ADC_SQR1_L_SHIFT             20
#define ADC1_SQR2                        *(volatile unsigned long *)(ADC_BLOCK + 0x030) // ADC1 regular sequence register 2
    #define ADC_SQR2_SQ7_MASK            0x0000001f                      // 7th conversion channel number in regular sequence
    #define ADC_SQR2_SQ8_MASK            0x0000003e                      // 8th conversion channel number in regular sequence
    #define ADC_SQR2_SQ9_MASK            0x000007c0                      // 9th conversion channel number in regular sequence
    #define ADC_SQR2_SQ10_MASK           0x000f8000                      // 10th conversion channel number in regular sequence
    #define ADC_SQR2_SQ11_MASK           0x01f00000                      // 11th conversion channel number in regular sequence
    #define ADC_SQR2_SQ12_MASK           0x3e000000                      // 12th conversion channel number in regular sequence
#define ADC1_SQR3                        *(volatile unsigned long *)(ADC_BLOCK + 0x034) // ADC1 regular sequence register 3
    #define ADC_SQR3_SQ1_MASK            0x0000001f                      // 1st conversion channel number in regular sequence
    #define ADC_SQR3_SQ2_MASK            0x0000003e                      // 2nd conversion channel number in regular sequence
    #define ADC_SQR3_SQ3_MASK            0x000007c0                      // 3rd conversion channel number in regular sequence
    #define ADC_SQR3_SQ4_MASK            0x000f8000                      // 4th conversion channel number in regular sequence
    #define ADC_SQR3_SQ5_MASK            0x01f00000                      // 5th conversion channel number in regular sequence
    #define ADC_SQR3_SQ6_MASK            0x3e000000                      // 6th conversion channel number in regular sequence
#define ADC1_JSQR                        *(volatile unsigned long *)(ADC_BLOCK + 0x038) // ADC1 injected sequence register
#define ADC1_JDR1                        *(volatile unsigned long *)(ADC_BLOCK + 0x03c) // ADC1 injected data register 1
#define ADC1_JDR2                        *(volatile unsigned long *)(ADC_BLOCK + 0x040) // ADC1 injected data register 2
#define ADC1_JDR3                        *(volatile unsigned long *)(ADC_BLOCK + 0x044) // ADC1 injected data register 3
#define ADC1_JDR4                        *(volatile unsigned long *)(ADC_BLOCK + 0x048) // ADC1 injected data register 4
#define ADC1_DR_ADD                       (volatile unsigned long *)(ADC_BLOCK + 0x04c) // ADC1 regular data register address
#define ADC1_DR                          *(volatile unsigned long *)(ADC_BLOCK + 0x04c) // ADC1 regular data register

#define ADC2_SR                          *(volatile unsigned long *)(ADC_BLOCK + 0x100) // ADC2 status register
#define ADC2_CR1                         *(volatile unsigned long *)(ADC_BLOCK + 0x104) // ADC2 control register 1
#define ADC2_CR2                         *(volatile unsigned long *)(ADC_BLOCK + 0x108) // ADC2 control register 2
#define ADC2_SMPR1                       *(volatile unsigned long *)(ADC_BLOCK + 0x10c) // ADC2 sample time register 1
#define ADC2_SMPR2                       *(volatile unsigned long *)(ADC_BLOCK + 0x110) // ADC2 sample time register 2
#define ADC2_JOFR1                       *(volatile unsigned long *)(ADC_BLOCK + 0x114) // ADC2 injected channel data offset register 1
#define ADC2_JOFR2                       *(volatile unsigned long *)(ADC_BLOCK + 0x118) // ADC2 injected channel data offset register 2
#define ADC2_JOFR3                       *(volatile unsigned long *)(ADC_BLOCK + 0x11c) // ADC2 injected channel data offset register 3
#define ADC2_JOFR4                       *(volatile unsigned long *)(ADC_BLOCK + 0x120) // ADC2 injected channel data offset register 4
#define ADC2_HTR                         *(volatile unsigned long *)(ADC_BLOCK + 0x124) // ADC2 watchdog higher threshold register
#define ADC2_LTR                         *(volatile unsigned long *)(ADC_BLOCK + 0x128) // ADC2 watchdog lower threshold register
#define ADC2_SQR1                        *(volatile unsigned long *)(ADC_BLOCK + 0x12c) // ADC2 regular sequence register 1
#define ADC2_SQR2                        *(volatile unsigned long *)(ADC_BLOCK + 0x130) // ADC2 regular sequence register 2
#define ADC2_SQR3                        *(volatile unsigned long *)(ADC_BLOCK + 0x134) // ADC2 regular sequence register 3
#define ADC2_JSQR                        *(volatile unsigned long *)(ADC_BLOCK + 0x138) // ADC2 injected sequence register
#define ADC2_JDR1                        *(volatile unsigned long *)(ADC_BLOCK + 0x13c) // ADC2 injected data register 1
#define ADC2_JDR2                        *(volatile unsigned long *)(ADC_BLOCK + 0x140) // ADC2 injected data register 2
#define ADC2_JDR3                        *(volatile unsigned long *)(ADC_BLOCK + 0x144) // ADC2 injected data register 3
#define ADC2_JDR4                        *(volatile unsigned long *)(ADC_BLOCK + 0x148) // ADC2 injected data register 4
#define ADC2_DR_ADD                       (volatile unsigned long *)(ADC_BLOCK + 0x14c) // ADC2 regular data register address
#define ADC2_DR                          *(volatile unsigned long *)(ADC_BLOCK + 0x14c) // ADC2 regular data register

#define ADC3_SR                          *(volatile unsigned long *)(ADC_BLOCK + 0x200) // ADC3 status register
#define ADC3_CR1                         *(volatile unsigned long *)(ADC_BLOCK + 0x204) // ADC3 control register 1
#define ADC3_CR2                         *(volatile unsigned long *)(ADC_BLOCK + 0x208) // ADC3 control register 2
#define ADC3_SMPR1                       *(volatile unsigned long *)(ADC_BLOCK + 0x20c) // ADC3 sample time register 1
#define ADC3_SMPR2                       *(volatile unsigned long *)(ADC_BLOCK + 0x210) // ADC3 sample time register 2
#define ADC3_JOFR1                       *(volatile unsigned long *)(ADC_BLOCK + 0x214) // ADC3 injected channel data offset register 1
#define ADC3_JOFR2                       *(volatile unsigned long *)(ADC_BLOCK + 0x218) // ADC3 injected channel data offset register 2
#define ADC3_JOFR3                       *(volatile unsigned long *)(ADC_BLOCK + 0x21c) // ADC3 injected channel data offset register 3
#define ADC3_JOFR4                       *(volatile unsigned long *)(ADC_BLOCK + 0x220) // ADC3 injected channel data offset register 4
#define ADC3_HTR                         *(volatile unsigned long *)(ADC_BLOCK + 0x224) // ADC3 watchdog higher threshold register
#define ADC3_LTR                         *(volatile unsigned long *)(ADC_BLOCK + 0x228) // ADC3 watchdog lower threshold register
#define ADC3_SQR1                        *(volatile unsigned long *)(ADC_BLOCK + 0x22c) // ADC3 regular sequence register 1
#define ADC3_SQR2                        *(volatile unsigned long *)(ADC_BLOCK + 0x230) // ADC3 regular sequence register 2
#define ADC3_SQR3                        *(volatile unsigned long *)(ADC_BLOCK + 0x234) // ADC3 regular sequence register 3
#define ADC3_JSQR                        *(volatile unsigned long *)(ADC_BLOCK + 0x238) // ADC3 injected sequence register
#define ADC3_JDR1                        *(volatile unsigned long *)(ADC_BLOCK + 0x23c) // ADC3 injected data register 1
#define ADC3_JDR2                        *(volatile unsigned long *)(ADC_BLOCK + 0x240) // ADC3 injected data register 2
#define ADC3_JDR3                        *(volatile unsigned long *)(ADC_BLOCK + 0x244) // ADC3 injected data register 3
#define ADC3_JDR4                        *(volatile unsigned long *)(ADC_BLOCK + 0x248) // ADC3 injected data register 4
#define ADC3_DR_ADD                       (volatile unsigned long *)(ADC_BLOCK + 0x24c) // ADC3 regular data register address
#define ADC3_DR                          *(volatile unsigned long *)(ADC_BLOCK + 0x24c) // ADC3 regular data register

#define ADC_CSR                          *(volatile unsigned long *)(ADC_BLOCK + 0x300) // ADC common status register (read-only)
    #define ADC_CSR_AWD1                 0x00000001                      // analog watchdog event occurred on ADC1
    #define ADC_CSR_EOC1                 0x00000002                      // conversion of all injected channels complete on ADC1
    #define ADC_CSR_JEOC1                0x00000004                      // injected channel end of conversion of ADC1
    #define ADC_CSR_JSTRT1               0x00000008                      // injected group conversion has started on ADC1
    #define ADC_CSR_STRT1                0x00000010                      // regular channel conversion has started on ADC1
    #define ADC_CSR_OV1                  0x00000020                      // overrun has occurred on ADC1
    #define ADC_CSR_AWD2                 0x00000100                      // analog watchdog event occurred on ADC2
    #define ADC_CSR_EOC2                 0x00000200                      // conversion of all injected channels complete on ADC2
    #define ADC_CSR_JEOC2                0x00000400                      // injected channel end of conversion of ADC2
    #define ADC_CSR_JSTRT2               0x00000800                      // injected group conversion has started on ADC2
    #define ADC_CSR_STRT2                0x00001000                      // regular channel conversion has started on ADC2
    #define ADC_CSR_OV2                  0x00002000                      // overrun has occurred on ADC2
    #define ADC_CSR_AWD3                 0x00010000                      // analog watchdog event occurred on ADC3
    #define ADC_CSR_EOC3                 0x00020000                      // conversion of all injected channels complete on ADC3
    #define ADC_CSR_JEOC3                0x00040000                      // injected channel end of conversion of ADC3
    #define ADC_CSR_JSTRT3               0x00080000                      // injected group conversion has started on ADC3
    #define ADC_CSR_STRT3                0x00100000                      // regular channel conversion has started on ADC3
    #define ADC_CSR_OV3                  0x00200000                      // overrun has occurred on ADC3
#define ADC_CCR                          *(volatile unsigned long *)(ADC_BLOCK + 0x304) // ADC common control register
    #define ADC_CSR_CCR_MULTI_INDEPENDENT  0x00000000                    // independent mode
    #define ADC_CSR_CCR_MULTI_DUAL_COM_INJ 0x00000001                    // dual mode ADC1 and 2 work together, ADC3 is independent - combined regular simultaneous and injected trigger mode
    #define ADC_CSR_CCR_MULTI_DUAL_COM_ALT 0x00000002                    // dual mode ADC1 and 2 work together, ADC3 is independent - combined regular simultaneous and alterate trigger mode
    #define ADC_CSR_CCR_MULTI_DIAL_INJ   0x00000005                      // dual mode ADC1 and 2 work together, ADC3 is independent - injected simultaneous mode only
    #define ADC_CSR_CCR_MULTI_DUAL_REG   0x00000006                      // dual mode ADC1 and 2 work together, ADC3 is independent - regular simultaneous mode only
    #define ADC_CSR_CCR_MULTI_DUAL_INT   0x00000007                      // dual mode ADC1 and 2 work together, ADC3 is independent - interleaved mode only
    #define ADC_CSR_CCR_MULTI_DUAL_ALT   0x00000009                      // dual mode ADC1 and 2 work together, ADC3 is independent - alternate trigger mode only
    #define ADC_CSR_CCR_MULTI_TRIP_COM_INJ 0x00000011                    // tripple mode ADC1, 2 and 3 work together - combined regular simultaneous and injected trigger mode
    #define ADC_CSR_CCR_MULTI_TRIP_COM_ALT 0x00000012                    // tripple mode ADC1, 2 and 3 work together - combined regular simultaneous and alterate trigger mode
    #define ADC_CSR_CCR_MULTI_TRIP_INJ   0x00000015                      // tripple mode ADC1, 2 and 3 work together - injected simultaneous mode only
    #define ADC_CSR_CCR_MULTI_TRIP_REG   0x00000016                      // tripple mode ADC1, 2 and 3 work together - regular simultaneous mode only
    #define ADC_CSR_CCR_MULTI_TRIP_INT   0x00000017                      // tripple mode ADC1, 2 and 3 work together - interleaved mode only
    #define ADC_CSR_CCR_MULTI_TRIP_ALT   0x00000019                      // tripple mode ADC1, 2 and 3 work together - alternate trigger mode only
    #define ADC_CSR_CCR_DELAY_5          0x00000000                      // delay between 2 sampling phases (dual or tripple interleaved mode) - 5 * TADCCLK
    #define ADC_CSR_CCR_DELAY_6          0x00000100                      // delay - 6 * TADCCLK
    #define ADC_CSR_CCR_DELAY_7          0x00000200                      // delay - 7 * TADCCLK
    #define ADC_CSR_CCR_DELAY_8          0x00000300                      // delay - 8 * TADCCLK
    #define ADC_CSR_CCR_DELAY_9          0x00000400                      // delay - 9 * TADCCLK
    #define ADC_CSR_CCR_DELAY_10         0x00000500                      // delay - 10 * TADCCLK
    #define ADC_CSR_CCR_DELAY_11         0x00000600                      // delay - 11 * TADCCLK
    #define ADC_CSR_CCR_DELAY_12         0x00000700                      // delay - 12 * TADCCLK
    #define ADC_CSR_CCR_DELAY_13         0x00000800                      // delay - 13 * TADCCLK
    #define ADC_CSR_CCR_DELAY_14         0x00000900                      // delay - 14 * TADCCLK
    #define ADC_CSR_CCR_DELAY_15         0x00000a00                      // delay - 15 * TADCCLK
    #define ADC_CSR_CCR_DELAY_16         0x00000b00                      // delay - 16 * TADCCLK
    #define ADC_CSR_CCR_DELAY_17         0x00000c00                      // delay - 17 * TADCCLK
    #define ADC_CSR_CCR_DELAY_18         0x00000d00                      // delay - 18 * TADCCLK
    #define ADC_CSR_CCR_DELAY_19         0x00000e00                      // delay - 19 * TADCCLK
    #define ADC_CSR_CCR_DELAY_20         0x00000f00                      // delay - 20 * TADCCLK
    #define ADC_CSR_CCR_DDS              0x00002000                      // DMA disable selection (fo multi-ADC mode) - DMA requests are issued as long as data are converted and a DMA mode is set
    #define ADC_CSR_CCR_DMA_DISABLED     0x00000000                      // DMA disabled
    #define ADC_CSR_CCR_DMA_MODE_1       0x00004000                      // DMA mode 1 enabled (2/3 half-words one by one - 1 then 2 then 3)
    #define ADC_CSR_CCR_DMA_MODE_2       0x00008000                      // DMA mode 2 enabled (2/3 half-words by pairs - 2&1 then 1&3 then 3&2)
    #define ADC_CSR_CCR_DMA_MODE_3       0x0000c000                      // DMA mode 3 enabled (2/3 bytes by pairs - 2&1 then 1&3 then 3&2)
    #define ADC_CSR_CCR_ADCPRE_PCLK_2    0x00000000                      // ADC clock prescaler - PCLK/2
    #define ADC_CSR_CCR_ADCPRE_PCLK_4    0x00010000                      // ADC clock prescaler - PCLK/4
    #define ADC_CSR_CCR_ADCPRE_PCLK_6    0x00020000                      // ADC clock prescaler - PCLK/6
    #define ADC_CSR_CCR_ADCPRE_PCLK_8    0x00030000                      // ADC clock prescaler - PCLK/8
    #define ADC_CSR_CCR_VBATE            0x00400000                      // VBAT channel enable
    #define ADC_CSR_CCR_TSVREFE          0x00800000                      // temperature sensor and VREFINT channel enable (VBATE must be disabled when TSVREFE is set on certain processors)
#define ADC_CDR                          *(volatile unsigned long *)(ADC_BLOCK + 0x308) // ADC common regular data register for dual and triple modes


typedef struct stSTM32_ADC_REGS
{
    volatile unsigned long ADC_SR;                                       // ADC status register
    volatile unsigned long ADC_CR1;                                      // ADC control register 1
    volatile unsigned long ADC_CR2;                                      // ADC control register 2
    volatile unsigned long ADC_SMPR1;                                    // ADC sample time register 1
    volatile unsigned long ADC_SMPR2;                                    // ADC sample time register 2
    volatile unsigned long ADC_JOFR1;                                    // ADC injected channel data offset register 1
    volatile unsigned long ADC_JOFR2;                                    // ADC injected channel data offset register 2
    volatile unsigned long ADC_JOFR3;                                    // ADC injected channel data offset register 3
    volatile unsigned long ADC_JOFR4;                                    // ADC injected channel data offset register 4
    volatile unsigned long ADC_HTR;                                      // ADC watchdog higher threshold register
    volatile unsigned long ADC_LTR;                                      // ADC watchdog lower threshold register
    volatile unsigned long ADC_SQR1;                                     // ADC regular sequence register 1
    volatile unsigned long ADC_SQR2;                                     // ADC regular sequence register 2
    volatile unsigned long ADC_SQR3;                                     // ADC regular sequence register 3
    volatile unsigned long ADC_JSQR;                                     // ADC injected sequence register
    volatile unsigned long ADC_JDR1;                                     // ADC injected data register 1
    volatile unsigned long ADC_JDR2;                                     // ADC injected data register 2
    volatile unsigned long ADC_JDR3;                                     // ADC injected data register 3
    volatile unsigned long ADC_JDR4;                                     // ADC injected data register 4
    volatile unsigned long ADC_DR;                                       // ADC regular data register
} STM32_ADC_REGS;

// ADC input names (used as internal reference directly)
//

// ADC1 only
//
#define ADC1_TEMPERATURE_SENSOR  16                                      // -40°C..°125°C with +/-1.5% precision
#define ADC1_VREFINT             17
#define ADC1_VBAT                18
#if defined _STM32F42X || defined _STM32F43X
    #define ADC1_TEMPERATURE_SENSOR_  18
#else
    #define ADC1_TEMPERATURE_SENSOR_  16
#endif

// ADC1 and 2 only
//
#define ADC12_IN4             4
#define ADC12_IN5             5
#define ADC12_IN6             6
#define ADC12_IN7             7
#define ADC12_IN8             8
#define ADC12_IN9             9
#define ADC12_IN14            14
#define ADC12_IN15            15

//ADC1, 2 and 3
//
#define ADC123_IN0            0
#define ADC123_IN1            1
#define ADC123_IN2            2
#define ADC123_IN3            3

#define ADC123_IN10           10
#define ADC123_IN11           11
#define ADC123_IN12           12
#define ADC123_IN13           13

// ADC3 only
//
#define ADC3_IN4              4
#define ADC3_IN5              5
#define ADC3_IN6              6
#define ADC3_IN7              7
#define ADC3_IN8              8
#define ADC3_IN9              9
#define ADC3_IN14             14
#define ADC3_IN15             15

#define ADC_ANALOG_WATCHDOG_ALL   31                                     // analog watchog active on all scanned channels (rather than a single one)



// SDIO controller (high-density and XL-density performance line devices only)
//
#define SDIO_POWER                       *(volatile unsigned long *)(SDIO_BLOCK + 0x00) // SDIO Power Control Register
  #define SDIO_POWER_POWER_OFF           0x00000000
  #define SDIO_POWER_POWER_UP            0x00000002
  #define SDIO_POWER_POWER_ON            0x00000003
#define SDIO_CLKCR                       *(volatile unsigned long *)(SDIO_BLOCK + 0x04)  // SDIO Clock Control Register
  #define SDIO_POWER_CLKDIV_MASK         0x000000ff                             // SDIO_CK = SDIOCLK/(SDIO_POWER_CLKDIV + 2)
  #define SDIO_POWER_CLKEN               0x00000100                             // enable SDIO_CK
  #define SDIO_POWER_PWRSAV              0x00000200                             // SDIO_CK is only enabled when the bus is active
  #define SDIO_POWER_BYPASS              0x00000400                             // SDIO_CK equal to SDIOCLK
  #define SDIO_POWER_BUS_1BIT            0x00000000                             // bus width 1 bits
  #define SDIO_POWER_BUS_4BIT            0x00000800                             // bus width 4 bits
  #define SDIO_POWER_BUS_8BIT            0x00001000                             // bus width 8 bits
  #define SDIO_POWER_POSEDGE             0x00000000                             // SDIO_CK is generate on rising edge of SDIOCLK
  #define SDIO_POWER_NEGEDGE             0x00002000                             // SDIO_CK is generate on falling edge of SDIOCLK
  #define SDIO_POWER_HWFC_EN             0x00004000                             // hardware flow control enable
#define SDIO_ARG                         *(volatile unsigned long *)(SDIO_BLOCK + 0x08) // SDIO Argument Register
#define SDIO_CMD                         *(unsigned long *)(SDIO_BLOCK + 0x0c)  // SDIO Command Register
  #define SDIO_CMD_CMDINDEX_MASK         0x0000003f                             // command index sent as part of command message
  #define SDIO_CMD_NO_RESPONSE           0x00000000                             // no response - expect CMDSENT flag
  #define SDIO_CMD_SHORT_RESPONSE        0x00000040                             // short response - expect CMDREND or CCRCFAIL flag
  #define SDIO_CMD_LONG_RESPONSE         0x000000c0                             // long response - expect CMDREND or CCRCFAIL flag
  #define SDIO_CMD_WAITINT               0x00000100                             // disable command timeout and wait for interrupt request
  #define SDIO_CMD_NO_WAIT               0x00000000
  #define SDIO_CMD_WAITPEND              0x00000200                             // wait for end of data transfer before sending a command
  #define SDIO_CMD_CPSMEN                0x00000400                             // CPSM state event machine enabled
  #define SDIO_CMD_SDIOSuspend           0x00000800                             // suspend command (only used with SDIo cards)
  #define SDIO_CMD_ENCMDcompl            0x00001000                             // enable command completion signal
  #define SDIO_CMD_nIEN                  0x00002000                             // interrupt enabled in CE-ATA when this is '0'
  #define SDIO_CMD_ATACMD                0x00004000                             // if ATMCMD is set send CMD61
#define SDIO_RESPCMD                     *(volatile unsigned long *)(SDIO_BLOCK + 0x10)  // SDIO Command Response Register
#define SDIO_RESP1                       *(volatile unsigned long *)(SDIO_BLOCK + 0x14)  // SDIO Response Register 1
#define SDIO_RESP2                       *(volatile unsigned long *)(SDIO_BLOCK + 0x18)  // SDIO Response Register 2
#define SDIO_RESP3                       *(volatile unsigned long *)(SDIO_BLOCK + 0x1c)  // SDIO Response Register 3
#define SDIO_RESP4                       *(volatile unsigned long *)(SDIO_BLOCK + 0x20)  // SDIO Response Register 4
#define SDIO_DTIMER                      *(unsigned long *)(SDIO_BLOCK + 0x24)           // SDIO Data Timer Register
#define SDIO_DLEN                        *(unsigned long *)(SDIO_BLOCK + 0x28)           // SDIO Data Length Register
  #define SDIO_DLEN_LENGTH_MASK          0x01ffffff
#define SDIO_DCTRL                       *(unsigned long *)(SDIO_BLOCK + 0x2c)           // SDIO Data Control Register
  #define SDIO_DCTRL_DTEN                0x00000001
  #define SDIO_DCTRL_DTDIR_WRITE         0x00000000
  #define SDIO_DCTRL_DTDIR_READ          0x00000002
  #define SDIO_DCTRL_DTMODE_BLOCK        0x00000000
  #define SDIO_DCTRL_DTMODE_STREAM       0x00000004
  #define SDIO_DCTRL_DMAEN               0x00000008
  #define SDIO_DCTRL_DBLOCKSIZE_MASK     0x000000f0
  #define SDIO_DCTRL_DBLOCKSIZE_512      0x00000090
  #define SDIO_DCTRL_RWSTART             0x00000100
  #define SDIO_DCTRL_RWSTOP              0x00000200
  #define SDIO_DCTRL_RWMOD               0x00000400
  #define SDIO_DCTRL_SDIOEN              0x00000800
#define SDIO_DCOUNT                      *(volatile unsigned long *)(SDIO_BLOCK + 0x30)  // SDIO Data Counter Register
#define SDIO_STA                         *(volatile unsigned long *)(SDIO_BLOCK + 0x34)  // SDIO Status Register (read-only)
  #define SDIO_STA_CCRFAIL               0x00000001                             // command response crc check failed
  #define SDIO_STA_DCRCFAIL              0x00000002                             // data block received crc check failed
  #define SDIO_STA_CTIMEOUT              0x00000004                             // command response timeout (64 SDIO_CK clocks)
  #define SDIO_STA_DTIMEOUT              0x00000008                             // data timeout
  #define SDIO_STA_TXUNDERR              0x00000010                             // transmit FIFO underrun error
  #define SDIO_STA_RXOVERR               0x00000020                             // receive FIFO overrun error
  #define SDIO_STA_CMDREND               0x00000040                             // command response received (CRC passed)
  #define SDIO_STA_CMDSENT               0x00000080                             // command sent (no response required)
  #define SDIO_STA_DATAEND               0x00000100                             // data end (data counter, SDIDCOUNT, is zero)
  #define SDIO_STA_STBIRERR              0x00000200                             // start bit not detected on all data signals in wide bus mode
  #define SDIO_STA_BDCKEND               0x00000400                             // data block sent/received (CRC check passed)
  #define SDIO_STA_CMDACT                0x00000800                             // command transfer in progress
  #define SDIO_STA_TXACT                 0x00001000                             // data transmit in progress
  #define SDIO_STA_RXACT                 0x00002000                             // data receive in progress
  #define SDIO_STA_TXFIFOHE              0x00004000                             // transmit FIFO half empty: at least 8 words can be written into the FIFO
  #define SDIO_STA_RXFIFOHE              0x00008000                             // receive FIFO is half full: there are at least 8 words in the FIFO
  #define SDIO_STA_TXFIFOF               0x00010000                             // transmit FIFO full
  #define SDIO_STA_RXFIFOF               0x00020000                             // receive FIFO full (when HW flow control is enabled this is set two words before full)
  #define SDIO_STA_TXFIFOE               0x00040000                             // transmit FIFO empty (when HW flow control is enabled this is set when 2 words are in the FIFO)
  #define SDIO_STA_RXFIFOE               0x00080000                             // receive FIFO empty
  #define SDIO_STA_TXDAVL                0x00100000                             // data available in transmit FIFO
  #define SDIO_STA_RXDAVL                0x00200000                             // data available in receive FIFO
  #define SDIO_STA_SDIOIT                0x00400000                             // SDIO interrupt received
  #define SDIO_STA_CEATAEND              0x00800000                             // DE-ATA command completion signal received for CMD61
#define SDIO_ICR                         *(volatile unsigned long *)(SDIO_BLOCK + 0x38)  // SDIO Interrupt Clear Register (write-only)
#define SDIO_MASK                        *(unsigned long *)(SDIO_BLOCK + 0x3c)           // SDIO Mask Register

#define SDIO_FIFOCNT                     *(volatile unsigned long *)(SDIO_BLOCK + 0x48)  // SDIO FIFO Counter Register

#define SDIO_FIFO0_ADD                   (volatile unsigned long *)(SDIO_BLOCK + 0x80)   // Address of SDIO FIFO 0
#define SDIO_FIFO0                       *(volatile unsigned long *)(SDIO_BLOCK + 0x80)  // SDIO FIFO 0
#define SDIO_FIFO1                       *(volatile unsigned long *)(SDIO_BLOCK + 0x84)  // SDIO FIFO 1
#define SDIO_FIFO2                       *(volatile unsigned long *)(SDIO_BLOCK + 0x88)  // SDIO FIFO 2
#define SDIO_FIFO3                       *(volatile unsigned long *)(SDIO_BLOCK + 0x8c)  // SDIO FIFO 3
#define SDIO_FIFO4                       *(volatile unsigned long *)(SDIO_BLOCK + 0x90)  // SDIO FIFO 4
#define SDIO_FIFO5                       *(volatile unsigned long *)(SDIO_BLOCK + 0x94)  // SDIO FIFO 5
#define SDIO_FIFO6                       *(volatile unsigned long *)(SDIO_BLOCK + 0x98)  // SDIO FIFO 6
#define SDIO_FIFO7                       *(volatile unsigned long *)(SDIO_BLOCK + 0x9c)  // SDIO FIFO 7
#define SDIO_FIFO8                       *(volatile unsigned long *)(SDIO_BLOCK + 0xa0)  // SDIO FIFO 8
#define SDIO_FIFO9                       *(volatile unsigned long *)(SDIO_BLOCK + 0xa4)  // SDIO FIFO 9
#define SDIO_FIFO10                      *(volatile unsigned long *)(SDIO_BLOCK + 0xa8)  // SDIO FIFO 10
#define SDIO_FIFO11                      *(volatile unsigned long *)(SDIO_BLOCK + 0xac)  // SDIO FIFO 11
#define SDIO_FIFO12                      *(volatile unsigned long *)(SDIO_BLOCK + 0xb0)  // SDIO FIFO 12
#define SDIO_FIFO13                      *(volatile unsigned long *)(SDIO_BLOCK + 0xb4)  // SDIO FIFO 13
#define SDIO_FIFO14                      *(volatile unsigned long *)(SDIO_BLOCK + 0xb8)  // SDIO FIFO 14
#define SDIO_FIFO15                      *(volatile unsigned long *)(SDIO_BLOCK + 0xbc)  // SDIO FIFO 15
#define SDIO_FIFO16                      *(volatile unsigned long *)(SDIO_BLOCK + 0xc0)  // SDIO FIFO 16
#define SDIO_FIFO17                      *(volatile unsigned long *)(SDIO_BLOCK + 0xc4)  // SDIO FIFO 17
#define SDIO_FIFO18                      *(volatile unsigned long *)(SDIO_BLOCK + 0xc8)  // SDIO FIFO 18
#define SDIO_FIFO19                      *(volatile unsigned long *)(SDIO_BLOCK + 0xcc)  // SDIO FIFO 19
#define SDIO_FIFO20                      *(volatile unsigned long *)(SDIO_BLOCK + 0xd0)  // SDIO FIFO 20
#define SDIO_FIFO21                      *(volatile unsigned long *)(SDIO_BLOCK + 0xd4)  // SDIO FIFO 21
#define SDIO_FIFO22                      *(volatile unsigned long *)(SDIO_BLOCK + 0xd8)  // SDIO FIFO 22
#define SDIO_FIFO23                      *(volatile unsigned long *)(SDIO_BLOCK + 0xdc)  // SDIO FIFO 23
#define SDIO_FIFO24                      *(volatile unsigned long *)(SDIO_BLOCK + 0xe0)  // SDIO FIFO 24
#define SDIO_FIFO25                      *(volatile unsigned long *)(SDIO_BLOCK + 0xe4)  // SDIO FIFO 25
#define SDIO_FIFO26                      *(volatile unsigned long *)(SDIO_BLOCK + 0xe8)  // SDIO FIFO 26
#define SDIO_FIFO27                      *(volatile unsigned long *)(SDIO_BLOCK + 0xec)  // SDIO FIFO 27
#define SDIO_FIFO28                      *(volatile unsigned long *)(SDIO_BLOCK + 0xf0)  // SDIO FIFO 28
#define SDIO_FIFO29                      *(volatile unsigned long *)(SDIO_BLOCK + 0xf4)  // SDIO FIFO 29
#define SDIO_FIFO30                      *(volatile unsigned long *)(SDIO_BLOCK + 0xf8)  // SDIO FIFO 30
#define SDIO_FIFO31                      *(volatile unsigned long *)(SDIO_BLOCK + 0xfc)  // SDIO FIFO 31




// Flash memory interface
//
#if defined _STM32L0x1
    #define FLASH_ACR                    *(volatile unsigned long *)(FMI_BLOCK + 0x00)  // Flash Access Control Register
        #define FLASH_ACR_LATENCY_ZERO_WAIT  0x00000000          // zero wait states
        #define FLASH_ACR_LATENCY_ONE_WAIT   0x00000001          // one wait state
        #define FLASH_ACR_PRFTEN         0x00000002              // prefetch enable
        #define FLASH_ACR_SLEEP_PD       0x00000008              // NVM power down in sleep mode
        #define FLASH_ACR_RUN_PD         0x00000010              // NVM power down in RUN mode (write only possible when unlocked)
        #define FLASH_ACR_DISAB_BUF      0x00000020              // disable cache buffer
        #define FLASH_ACR_PRE_READ       0x00000040              // pre-read enable
    #define FLASH_PECR                   *(volatile unsigned long *)(FMI_BLOCK + 0x04)
    #define FLASH_PDKEYR                 *(volatile unsigned long *)(FMI_BLOCK + 0x08)
    #define FLASH_PKEYR                  *(volatile unsigned long *)(FMI_BLOCK + 0x0c)
    #define FLASH_PRGKEYR                *(volatile unsigned long *)(FMI_BLOCK + 0x10)
    #define FLASH_OPTKEYR                *(volatile unsigned long *)(FMI_BLOCK + 0x14)
    #define FLASH_SR                     *(volatile unsigned long *)(FMI_BLOCK + 0x18)
    #define FLASH_OPTR                   *(volatile unsigned long *)(FMI_BLOCK + 0x1c)
    #define FLASH_WRPROT1                *(volatile unsigned long *)(FMI_BLOCK + 0x20)
    #define FLASH_WRPROT2                *(volatile unsigned long *)(FMI_BLOCK + 0x80)
#else
    #define FLASH_ACR                    *(volatile unsigned long *)(FMI_BLOCK + 0x00)  // Flash Access Control Register
    #if defined _STM32F7XX
        #define FLASH_ACR_LATENCY_ZERO_WAIT      0x00000000              // zero wait states when SYSCLK <= 30MHz (valid only for highest voltage)
        #define FLASH_ACR_LATENCY_ONE_WAIT       0x00000001              // one wait state when SYSCLK > 30MHz
        #define FLASH_ACR_LATENCY_TWO_WAITS      0x00000002              // two wait states when SYSCLK > 60MHz
        #define FLASH_ACR_LATENCY_THREE_WAITS    0x00000003              // three wait states when SYSCLK > 90MHz
        #define FLASH_ACR_LATENCY_FOUR_WAITS     0x00000004              // four wait state when SYSCLK > 120MHz
        #define FLASH_ACR_LATENCY_FIVE_WAITS     0x00000005              // five wait states when SYSCLK > 150MHz
        #define FLASH_ACR_LATENCY_SIX_WAITS      0x00000006              // six wait states (required at reduced voltage range only)
        #define FLASH_ACR_LATENCY_SEVEN_WAITS    0x00000007              // seven wait states (required at reduced voltage range only)
        #define FLASH_ACR_LATENCY_EIGHT_WAITS    0x00000008              // eight wait states (required at reduced voltage range only)
        #define FLASH_ACR_LATENCY_NINE_WAITS     0x00000009              // nine wait states (required at reduced voltage range only)
        #define FLASH_ACR_LATENCY_TEN_WAITS      0x0000000a              // ten wait states (required at reduced voltage range only)
        #define FLASH_ACR_LATENCY_ELEVEN_WAITS   0x0000000b              // eleven wait states (required at reduced voltage range only)
        #define FLASH_ACR_LATENCY_TWELVE_WAITS   0x0000000c              // twelve wait states (required at reduced voltage range only)
        #define FLASH_ACR_LATENCY_THIRTEEN_WAITS 0x0000000d              // thirteen wait states (required at reduced voltage range only)
        #define FLASH_ACR_LATENCY_FORTEEN_WAITS  0x0000000e              // forteen wait states (required at reduced voltage range only)
        #define FLASH_ACR_LATENCY_FIFTEEN_WAITS  0x0000000f              // fifteen wait states (required at reduced voltage range only)
        #define FLASH_ACR_PRFTEN              0x00000100                 // prefetch buffer enable
        #define FLASH_ACR_ARTEN               0x00000200                 // ART accelerator enable
        #define FLASH_ACR_ARTRS               0x00000800                 // ART accelerator reset
    #elif defined _STM32F2XX || defined _STM32F4XX || defined _STM32L432 || defined _STM32L4X5 || defined _STM32L4X6
        #define FLASH_ACR_LATENCY_ZERO_WAIT   0x00000000                 // zero wait states when SYSCLK <= 30MHz (valid only for highest voltage)
        #define FLASH_ACR_LATENCY_ONE_WAIT    0x00000001                 // one wait state when SYSCLK > 30MHz
        #define FLASH_ACR_LATENCY_TWO_WAITS   0x00000002                 // two wait states when SYSCLK > 60MHz
        #define FLASH_ACR_LATENCY_THREE_WAITS 0x00000003                 // three wait states when SYSCLK > 90MHz
        #define FLASH_ACR_LATENCY_FOUR_WAITS  0x00000004                 // four wait state when SYSCLK > 120MHz
        #define FLASH_ACR_LATENCY_FIVE_WAITS  0x00000005                 // five wait states when SYSCLK > 150MHz
        #define FLASH_ACR_LATENCY_SIX_WAITS   0x00000006                 // six wait states (required at reduced voltage range only)
        #define FLASH_ACR_LATENCY_SEVEN_WAITS 0x00000007                 // seven wait states (required at reduced voltage range only)
        #define FLASH_ACR_PRFTEN              0x00000100                 // prefetch buffer enable
        #define FLASH_ACR_ICEN                0x00000200                 // instruction cache enable
        #define FLASH_ACR_DCEN                0x00000400                 // data cache enable
        #define FLASH_ACR_ICRST               0x00000800                 // instruction cache reset (write only when D cache is disabled)
        #define FLASH_ACR_DCRST               0x00001000                 // data cache reset (write only when D cache is disabled)
        #if defined _STM32L432 || defined _STM32L4X5 || defined _STM32L4X6
            #define FLASH_ACR_RUN_PD          0x00002000                 // flash power-down during run or low power run mode
            #define FLASH_ACR_SLEEP_PD        0x00004000                 // flash power-down during sleep or low power sleep mode
        #endif
    #elif defined _CONNECTIVITY_LINE || defined _PERFORMANCE_LINE || defined _STM32F031
        #define FLASH_ACR_LATENCY_ZERO_WAIT  0x00000000                  // zero wait states when SYSCLK <= 24MHz
        #define FLASH_ACR_LATENCY_ONE_WAIT   0x00000001                  // one wait state when SYSCLK > 24MHz
        #define FLASH_ACR_LATENCY_TWO_WAITS  0x00000002                  // two wait states when SYSCLK > 48MHz
        #define FLASH_ACR_HLFCYA             0x00000008                  // flash half-cycle access enable
        #define FLASH_ACR_PRFTBE             0x00000010                  // prefetch buffer enable
        #define FLASH_ACR_PRFTBS             0x00000020                  // prefetch buffer status (read-only)
    #else
        #define FLASH_ACR_HLFCYA             0x00000008                  // half-cycle enabled
    #endif
    #define FLASH_KEYR                       *(volatile unsigned long *)(FMI_BLOCK + 0x04) // key register to allow access to the flash control register (and thus program and erase operations)
        #define FLASH_KEYR_KEY1                0x45670123
        #define FLASH_KEYR_KEY2                0xcdef89ab
    #define FLASH_OPTKEYR                    *(unsigned long *)(FMI_BLOCK + 0x08) // option key register to alow programming and erase operations in the user configuration sector
        #define FLASH_OPTKEYR_KEY1             0x08192a3b
        #define FLASH_OPTKEYR_KEY2             0x4c5d6e7f
    #define FLASH_SR                         *(volatile unsigned long *)(FMI_BLOCK + 0x0c)
    #if defined _STM32F2XX || defined _STM32F4XX || defined _STM32F7XX
        #define FLASH_SR_EOP                 0x00000001                  // end of operation (clear by writing 1)
        #define FLASH_SR_OPERR               0x00000002                  // operation error (only set when ERRIE is 1)
        #define FLASH_SR_WRPERR              0x00000010                  // write protection error (clear by writing 1)
        #define FLASH_SR_PGAERR              0x00000020                  // programming alignment error (clear by writing 1)
        #define FLASH_SR_PGPERR              0x00000040                  // programming parallelism error (clear by writing 1)
        #define FLASH_SR_PGSERR              0x00000080                  // programming sequence error (clear by writing 1)
        #define FLASH_SR_BSY                 0x00010000                  // busy
        #define FLASH_ERROR_FLAGS            (FLASH_SR_OPERR | FLASH_SR_WRPERR | FLASH_SR_PGAERR | FLASH_SR_PGPERR | FLASH_SR_PGSERR)
        #define FLASH_STATUS_FLAGS           (FLASH_ERROR_FLAGS | FLASH_SR_EOP)
    #else
        #define FLASH_SR_BSY                 0x00000001                  // busy
        #define FLASH_SR_PGERR               0x00000004                  // programming error
        #define FLASH_SR_WRPRTERR            0x00000010                  // write protection error
        #define FLASH_SR_EOP                 0x00000020                  // end of operation
        #define FLASH_ERROR_FLAGS            (FLASH_SR_PGERR | FLASH_SR_WRPRTERR)
        #define FLASH_STATUS_FLAGS           (FLASH_ERROR_FLAGS | FLASH_SR_EOP)
    #endif
    #define FLASH_CR                         *(volatile unsigned long *)(FMI_BLOCK + 0x10)
    #if defined _STM32F2XX || defined _STM32F4XX || defined _STM32F7XX
        #define FLASH_CR_PG                  0x00000001                  // programming - FLASH programming chosen
        #define FLASH_CR_SER                 0x00000002                  // sector erase
        #define FLASH_CR_MER                 0x00000004                  // mass erase (bank 1)
        #if SIZE_OF_FLASH >= (2 * 1024 * 1024)                           // 2Meg Flash part
            #define FLASH_CR_SNB_MASK        0x000000f8                  // 24 sectors
        #else
            #define FLASH_CR_SNB_MASK        0x00000078                  // 12 sectors
        #endif
        #define FLASH_CR_SNB_SHIFT           3
        #define FLASH_CR_PSIZE_8             0x00000000                  // parallelism size 8 bits
        #define FLASH_CR_PSIZE_16            0x00000100                  // parallelism size 16 bits
        #define FLASH_CR_PSIZE_32            0x00000200                  // parallelism size 32 bits
        #define FLASH_CR_PSIZE_64            0x00000300                  // parallelism size 64 bits
        #if SIZE_OF_FLASH >= (2 * 1024 * 1024)                           // 2Meg Flash part
            #define FLASH_CR_MER1            0x00008000                  // mass erase (bank 2)
        #endif
        #define FLASH_CR_STRT                0x00010000                  // start - triggers an erase operation
        #define FLASH_CR_EOPIE               0x01000000                  // end of operation interrupt enable
        #define FLASH_CR_ERRIE               0x02000000                  // error interrupt enable
        #define FLASH_CR_LOCK                0x80000000                  // lock - reset by hardware when detecting the unlock sequence
        #if SUPPLY_VOLTAGE == SUPPLY_1_8__2_1                            // only 8 bit parallelism when erasing possible
            #define MAXIMUM_PARALLELISM      FLASH_CR_PSIZE_8
        #elif SUPPLY_VOLTAGE < SUPPLY_2_7__3_6                           // 16 bit parallelism when erasing possible
            #define MAXIMUM_PARALLELISM      FLASH_CR_PSIZE_16
        #else                                                            // 32 bit parallelism when erasing possible
            #define MAXIMUM_PARALLELISM      FLASH_CR_PSIZE_32
        #endif
    #else
        #define FLASH_CR_PG              0x00000001                      // programming - FLASH programming chosen
        #define FLASH_CR_PER             0x00000002                      // page erase
        #define FLASH_CR_MER             0x00000004                      // mass erase
        #define FLASH_CR_OPTPG           0x00000010                      // option byte programming
        #define FLASH_CR_OPTER           0x00000020                      // option byte erase
        #define FLASH_CR_STRT            0x00000040                      // start - triggers an erase operation
        #define FLASH_CR_LOCK            0x00000080                      // lock - reset by hardware when detecting the unlock sequence
        #define FLASH_CR_OPTWRE          0x00000200                      // option bytes write enable (set by key entry, cleared by SW)
        #define FLASH_CR_ERRIE           0x00000400                      // error interrupt enable
        #define FLASH_CR_EOPIE           0x00001000                      // end of operation interrupt enable
    #endif
    #if defined _STM32F2XX || defined _STM32F4XX || defined _STM32F7XX
        #define FLASH_OPTCR              *(volatile unsigned long *)(FMI_BLOCK + 0x14) // FLASH Option Control Register (loaded from flash options at reset)
            #define FLASH_OPTCR_OPTLOCK  0x00000001                      // FLASH_OPTCR register is locked (write to '1' only)
            #define FLASH_OPTCR_OPTSTRT  0x00000002                      // option start (write to '1' only)
            #define FLASH_OPTCR_BOR_LEV_3       0x00000000               // brown-out reset level 3
            #define FLASH_OPTCR_BOR_LEV_2       0x00000004               // brown-out reset level 2
            #define FLASH_OPTCR_BOR_LEV_1       0x00000008               // brown-out reset level 1
            #define FLASH_OPTCR_BOR_LEV_OFF     0x0000000c               // brown-out reset off
            #if defined _STM32F42X || defined _STM32F43X
                #define FLASH_OPTCR_BFB2 0x00000010                      // dual-bank boot option (this must be kept cleared when DB1M is 0)
            #endif
            #define FLASH_OPTCR_USER_WDG_SW     0x00000020               // user option bytes - watchdog software mode
            #define FLASH_OPTCR_USER_nRST_STOP  0x00000040               // user option bytes
            #define FLASH_OPTCR_USER_nRST_STDBY 0x00000080               // user option bytes
            #define FLASH_OPTCR_RDP_LEVEL_0     0x0000aa00               // read protect level 0
            #define FLASH_OPTCR_RDP_LEVEL_1     0x00000000               // read protect level 1
            #define FLASH_OPTCR_RDP_LEVEL_2     0x0000cc00               // read protect level 2
            #define FLASH_OPTCR_RDP_MASK        0x0000ff00               // read protect level mask
            #define FLASH_OPTCR_nWRP0    0x00010000                      // sector 0 not write protected
            #define FLASH_OPTCR_nWRP1    0x00020000                      // sector 1 not write protected
            #define FLASH_OPTCR_nWRP2    0x00040000                      // sector 2 not write protected
            #define FLASH_OPTCR_nWRP3    0x00080000                      // sector 3 not write protected
            #define FLASH_OPTCR_nWRP4    0x00100000                      // sector 4 not write protected
            #define FLASH_OPTCR_nWRP5    0x00200000                      // sector 5 not write protected
            #define FLASH_OPTCR_nWRP6    0x00400000                      // sector 6 not write protected
            #define FLASH_OPTCR_nWRP7    0x00800000                      // sector 7 not write protected
            #define FLASH_OPTCR_nWRP8    0x01000000                      // sector 8 not write protected
            #define FLASH_OPTCR_nWRP9    0x02000000                      // sector 9 not write protected
            #define FLASH_OPTCR_nWRP10   0x04000000                      // sector 10 not write protected
            #define FLASH_OPTCR_nWRP11   0x08000000                      // sector 11 not write protected
        #if defined _STM32F42X || defined _STM32F43X
            #define FLASH_OPTCR_DB1M     0x40000000                      // dual bank on 1Mbyte flash memory devices
            #define FLASH_OPTCR_SPRMOD   0x80000000                      // nWPRi used for PCROP (read-out) protection instead of write protection
            #define FLASH_OPTCR_SETTING_MASK    (0xcffffffc)
        #else
            #define FLASH_OPTCR_SETTING_MASK    (0x0fffffec)
        #endif
            #define DEFAULT_FLASH_OPTION_SETTING (FLASH_OPTCR_OPTLOCK | FLASH_OPTCR_BOR_LEV_OFF | FLASH_OPTCR_USER_nRST_STDBY | FLASH_OPTCR_USER_nRST_STOP | FLASH_OPTCR_USER_WDG_SW | FLASH_OPTCR_RDP_LEVEL_0 | FLASH_OPTCR_nWRP0 | FLASH_OPTCR_nWRP1 | FLASH_OPTCR_nWRP2 | FLASH_OPTCR_nWRP3 | FLASH_OPTCR_nWRP4 | FLASH_OPTCR_nWRP5 | FLASH_OPTCR_nWRP6 | FLASH_OPTCR_nWRP7 | FLASH_OPTCR_nWRP8 | FLASH_OPTCR_nWRP9 | FLASH_OPTCR_nWRP10 | FLASH_OPTCR_nWRP11)
    #else
        #define FLASH_AR                 *(volatile unsigned long *)(FMI_BLOCK + 0x14) // FLASH Address Register (write-only) - no access when FLASH busy bit is set
        #define FLASH_OBR                *(volatile unsigned long *)(FMI_BLOCK + 0x1c) // Option Byte Register
        #define FLASH_WRPR               *(volatile unsigned long *)(FMI_BLOCK + 0x20) // Write Protection Register (read-only)
            #define FLASH_WRPR_WRP0      0x00000001                      // flash sector not write protected when read as '1'
        #ifdef XL_DENSITY
            #define FLASH_KEYR2          *(unsigned long *)(FMI_BLOCK + 0x44)
            #define FLASH_SR2            *(unsigned long *)(FMI_BLOCK + 0x4c)
            #define FLASH_CR2            *(unsigned long *)(FMI_BLOCK + 0x50)
            #define FLASH_AR2            *(unsigned long *)(FMI_BLOCK + 0x54)
        #endif
    #endif
    #if defined _STM32F42X || defined _STM32F43X
        #define FLASH_OPTCR1             *(volatile unsigned long *)(FMI_BLOCK + 0x18) // FLASH Option Control Register 1 (loaded from flash options at reset)
            #define FLASH_OPTCR1_nWRP0   0x00010000                      // sector 12 not write protected
            #define FLASH_OPTCR1_nWRP1   0x00020000                      // sector 13 not write protected
            #define FLASH_OPTCR1_nWRP2   0x00040000                      // sector 14 not write protected
            #define FLASH_OPTCR1_nWRP3   0x00080000                      // sector 15 not write protected
            #define FLASH_OPTCR1_nWRP4   0x00100000                      // sector 16 not write protected
            #define FLASH_OPTCR1_nWRP5   0x00200000                      // sector 17 not write protected
            #define FLASH_OPTCR1_nWRP6   0x00400000                      // sector 18 not write protected
            #define FLASH_OPTCR1_nWRP7   0x00800000                      // sector 19 not write protected
            #define FLASH_OPTCR1_nWRP8   0x01000000                      // sector 20 not write protected
            #define FLASH_OPTCR1_nWRP9   0x02000000                      // sector 21 not write protected
            #define FLASH_OPTCR1_nWRP10  0x04000000                      // sector 22 not write protected
            #define FLASH_OPTCR1_nWRP11  0x08000000                      // sector 23 not write protected
            #define DEFAULT_FLASH_OPTION_SETTING_1     (FLASH_OPTCR1_nWRP0 | FLASH_OPTCR1_nWRP1 | FLASH_OPTCR1_nWRP2 | FLASH_OPTCR1_nWRP3 | FLASH_OPTCR1_nWRP4 | FLASH_OPTCR1_nWRP5 | FLASH_OPTCR1_nWRP6 | FLASH_OPTCR1_nWRP7 | FLASH_OPTCR1_nWRP8 | FLASH_OPTCR1_nWRP9 | FLASH_OPTCR1_nWRP10 | FLASH_OPTCR1_nWRP11)
            #define FLASH_OPTCR1_SETTING_MASK          (DEFAULT_FLASH_OPTION_SETTING_1)
    #else
            #define DEFAULT_FLASH_OPTION_SETTING_1      0
    #endif
#endif

typedef struct st_STM32_FMI
{
#if defined _STM32L0x1
    unsigned long _FLASH_ACR;
    unsigned long _FLASH_PECR;
    volatile unsigned long _FLASH_PDKEYR;
    volatile unsigned long _FLASH_PKEYR;
    volatile unsigned long _FLASH_PRGKEYR;
    volatile unsigned long _FLASH_OPTKEYR;
    volatile unsigned long _FLASH_SR;
    volatile unsigned long _FLASH_OPTR;
    volatile unsigned long _FLASH_WRPROT1;
    unsigned long ulRes0[17];
    volatile unsigned long _FLASH_WRPROT2;
#else
    unsigned long _FLASH_ACR;
    volatile unsigned long _FLASH_KEYR;
    volatile unsigned long _FLASH_OPTKEYR;
    volatile unsigned long _FLASH_SR;
    unsigned long _FLASH_CR;
    #if defined _STM32F2XX || defined _STM32F4XX || defined _STM32F7XX
    volatile unsigned long _FLASH_OPTCR;
        #if defined _STM32F42X || defined _STM32F43X
    volatile unsigned long _FLASH_OPTC1;
        #endif
    #else
    unsigned long _FLASH_AR;
    unsigned long ulRes0;
    unsigned long _FLASH_OBR;
    unsigned long _FLASH_WRPR;
        #if defined XL_DENSITY
        volatile unsigned long _FLASH_KEYR2;
        unsigned long _FLASH_SR2;
        unsigned long _FLASH_CR2;
        unsigned long _FLASH_AR2;
        #endif
    #endif
#endif
} _STM32_FMI;


// USARTs
//
#if defined _STM32F7XX || defined _STM32L432 || defined _STM32L0x1 || defined _STM32F031 || defined _STM32L4X5 || defined _STM32L4X6
    #define USART1_CR1                   *(volatile unsigned long *)(USART1_BLOCK + 0x00)  // USART1 control register 1
      #define USART_CR1_UE               0x00000001                      // USART enable
      #define USART_CR1_RE               0x00000004                      // receiver enable
      #define USART_CR1_TE               0x00000008                      // transmitter enable
      #define USART_CR1_IDLEIE           0x00000010                      // idle interrupt enable
      #define USART_CR1_RXNEIE           0x00000020                      // RXNE interrupt enable
      #define USART_CR1_TCIE             0x00000040                      // transmit complete interrupt enable
      #define USART_CR1_TXEIE            0x00000080                      // transmitter empty interrupt enable
      #define USART_CR1_PEIE             0x00000100                      // parity error interrupt enable
      #define USART_CR1_PS               0x00000200                      // parity select - odd
      #define USART_CR1_PCE              0x00000400                      // parity control enable
      #define USART_CR1_WAKE             0x00000800                      // wakeup method
      #define USART_CR1_M0               0x00001000                      // word length 0
      #define USART_CR1_MME              0x00002000                      // mute mode enable
      #define USART_CR1_CMIE             0x00004000                      // character match interrupt enable
      #define USART_CR1_OVER16           0x00000000                      // oversampling mode x16
      #define USART_CR1_OVER8            0x00008000                      // oversampling mode x8

      #define USART_CR1_DEDT             0x001f0000                      // driver enable de-assertion time
      #define USART_CR1_DEAT             0x03e00000                      // driver enable assertion time
      #define USART_CR1_RTOIE            0x04000000                      // receiver timeout interrupt enable
      #define USART_CR1_EOBIE            0x08000000                      // end of block interrupt enable
      #define USART_CR1_M1               0x10000000                      // word length 1

      #define USART_CR1_8BIT             0x00000000                      // 8 bit word length
      #define USART_CR1_7BIT             (USART_CR1_M1)                  // 7 bit word length
      #define USART_CR1_9BIT             (USART_CR1_M0)                  // 9 bit word length
    #define USART1_CR2                   *(volatile unsigned long *)(USART1_BLOCK + 0x04)  // USART1 control register 2
      #define USART_CR2_ADDM7            0x00000010                      // 
      #define USART_CR2_LBDL             0x00000020                      // LIN break detection length
      #define USART_CR2_LBDIE            0x00000040                      // LIN break detection interrupt enable
      #define USART_CR2_LBCL             0x00000100                      // last bit clock pulse - don't write when transmitter is enabled!
      #define USART_CR2_CPHA             0x00000200                      // clock phase - don't write when transmitter is enabled!
      #define USART_CR2_CPOL             0x00000400                      // clock polarity - don't write when transmitter is enabled!
      #define USART_CR2_CLKEN            0x00000800                      // enable CK clock (not UART4 and UART5)
      #define USART_CR2_1_STOP           0x00000000
      #define USART_CR2_HALF_STOP        0x00001000
      #define USART_CR2_2_STOP           0x00002000
      #define USART_CR2_1_5_STOP         0x00003000                      // 0.5 and 1.5 stops not available for UARTs 4 and 5
      #define USART_CR2_LINEN            0x00004000                      // LIN mode enable
      #define USART_CR2_SWAP             0x00008000                      // 
      #define USART_CR2_RXINV            0x00010000                      // 
      #define USART_CR2_TXINV            0x00020000                      // 
      #define USART_CR2_DATAINV          0x00040000                      // 
      #define USART_CR2_MSBFIRST         0x00080000                      // 
      #define USART_CR2_ABREN            0x00100000                      // 
      #define USART_CR2_ABRMOD           0x00600000                      // 
      #define USART_CR2_RTOEN            0x00800000                      // 
      #define USART_CR2_ADD_MASK         0xff000000
    #define USART1_CR3                   *(volatile unsigned long *)(USART1_BLOCK + 0x08)  // USART1 control register 3
      #define USART_CR3_EIE              0x00000001
      #define USART_CR3_IREN             0x00000002
      #define USART_CR3_IRLP             0x00000004
      #define USART_CR3_HDSEL            0x00000008
      #define USART_CR3_NACK             0x00000010
      #define USART_CR3_SCEN             0x00000020
      #define USART_CR3_DMAR             0x00000040
      #define USART_CR3_DMAT             0x00000080
      #define USART_CR3_RTSE             0x00000100
      #define USART_CR3_CTSE             0x00000200
      #define USART_CR3_CTSIE            0x00000400
      #define USART_CR3_ONEBIT           0x00000800
      #define USART_CR3_OVRDIS           0x00001000
      #define USART_CR3_DDRE             0x00002000
      #define USART_CR3_DEM              0x00004000
      #define USART_CR3_DEP              0x00008000
      #define USART_CR3_SCARCNT          0x000e0000
    #define USART1_BRR                   *(volatile unsigned long *)(USART1_BLOCK + 0x0c)  // USART1 baud rate register
    #define USART1_GTPR                  *(volatile unsigned long *)(USART1_BLOCK + 0x10)  // USART1 guard time and prescaler register
      #define USART_GTPR_PSC_MASK        0x000000ff
      #define USART_GTPR_GT_MASK         0x0000ff00
    #define USART1_RTOR                  *(volatile unsigned long *)(USART1_BLOCK + 0x14)  // USART1 receiver timeout register
    #define USART1_RQR                   *(volatile unsigned long *)(USART1_BLOCK + 0x18)  // USART1 request register
    #define USART1_ISR                   *(volatile unsigned long *)(USART1_BLOCK + 0x1c)  // USART1 interrupt and status register
      #define USART_ISR_PE                0x00000001
      #define USART_ISR_FE                0x00000002
      #define USART_ISR_NE                0x00000004
      #define USART_ISR_ORE               0x00000008
      #define USART_ISR_IDLE              0x00000010
      #define USART_ISR_RXNE              0x00000020
      #define USART_ISR_TC                0x00000040
      #define USART_ISR_TXE               0x00000080
      #define USART_ISR_LBDF              0x00000100
      #define USART_ISR_CTSIF             0x00000200
      #define USART_ISR_CTS               0x00000400
      #define USART_ISR_RTOF              0x00000800
      #define USART_ISR_EOBF              0x00001000
      #define USART_ISR_ABRE              0x00004000
      #define USART_ISR_ABRF              0x00008000
      #define USART_ISR_BUSY              0x00010000
      #define USART_ISR_CMF               0x00020000
      #define USART_ISR_SBKF              0x00040000
      #define USART_ISR_TEACK             0x00200000
    #define USART1_ICR                   *(volatile unsigned long *)(USART1_BLOCK + 0x20)  // USART1 interrupt flag clear register (write '1' to clear)
    #define USART1_RDR                   *(volatile unsigned long *)(USART1_BLOCK + 0x24)  // USART1 receive data register
    #define USART1_TDR                   *(volatile unsigned long *)(USART1_BLOCK + 0x28)  // USART1 transmit data register

    #define USART2_CR1                   *(volatile unsigned long *)(USART2_BLOCK + 0x00)  // USART2 control register 1
    #define USART2_CR2                   *(volatile unsigned long *)(USART2_BLOCK + 0x04)  // USART2 control register 2
    #define USART2_CR3                   *(volatile unsigned long *)(USART2_BLOCK + 0x08)  // USART2 control register 3
    #define USART2_BRR                   *(volatile unsigned long *)(USART2_BLOCK + 0x0c)  // USART2 baud rate register
    #define USART2_GTPR                  *(volatile unsigned long *)(USART2_BLOCK + 0x10)  // USART2 guard time and prescaler register
    #define USART2_RTOR                  *(volatile unsigned long *)(USART2_BLOCK + 0x14)  // USART2 receiver timeout register
    #define USART2_RQR                   *(volatile unsigned long *)(USART2_BLOCK + 0x18)  // USART2 request register
    #define USART2_ISR                   *(volatile unsigned long *)(USART2_BLOCK + 0x1c)  // USART2 interrupt and status register
    #define USART2_ICR                   *(volatile unsigned long *)(USART2_BLOCK + 0x20)  // USART2 interrupt flag clear register (write '1' to clear)
    #define USART2_RDR                   *(volatile unsigned long *)(USART2_BLOCK + 0x24)  // USART2 receive data register
    #define USART2_TDR                   *(volatile unsigned long *)(USART2_BLOCK + 0x28)  // USART2 transmit data register

    #define USART3_CR1                   *(volatile unsigned long *)(USART3_BLOCK + 0x00)  // USART3 control register 1
    #define USART3_CR2                   *(volatile unsigned long *)(USART3_BLOCK + 0x04)  // USART3 control register 2
    #define USART3_CR3                   *(volatile unsigned long *)(USART3_BLOCK + 0x08)  // USART3 control register 3
    #define USART3_BRR                   *(volatile unsigned long *)(USART3_BLOCK + 0x0c)  // USART3 baud rate register
    #define USART3_GTPR                  *(volatile unsigned long *)(USART3_BLOCK + 0x10)  // USART3 guard time and prescaler register
    #define USART3_RTOR                  *(volatile unsigned long *)(USART3_BLOCK + 0x14)  // USART3 receiver timeout register
    #define USART3_RQR                   *(volatile unsigned long *)(USART3_BLOCK + 0x18)  // USART3 request register
    #define USART3_ISR                   *(volatile unsigned long *)(USART3_BLOCK + 0x1c)  // USART3 interrupt and status register
    #define USART3_ICR                   *(volatile unsigned long *)(USART3_BLOCK + 0x20)  // USART3 interrupt flag clear register (write '1' to clear)
    #define USART3_RDR                   *(volatile unsigned long *)(USART3_BLOCK + 0x24)  // USART3 receive data register
    #define USART3_TDR                   *(volatile unsigned long *)(USART3_BLOCK + 0x28)  // USART3 transmit data register

    #define UART4_CR1                    *(volatile unsigned long *)(UART4_BLOCK + 0x00)  // UART4 control register 1
    #define UART4_CR2                    *(volatile unsigned long *)(UART4_BLOCK + 0x04)  // UART4 control register 2
    #define UART4_CR3                    *(volatile unsigned long *)(UART4_BLOCK + 0x08)  // UART4 control register 3
    #define UART4_BRR                    *(volatile unsigned long *)(UART4_BLOCK + 0x0c)  // UART4 baud rate register
    #define UART4_GTPR                   *(volatile unsigned long *)(UART4_BLOCK + 0x10)  // UART4 guard time and prescaler register
    #define UART4_RTOR                   *(volatile unsigned long *)(UART4_BLOCK + 0x14)  // UART4 receiver timeout register
    #define UART4_RQR                    *(volatile unsigned long *)(UART4_BLOCK + 0x18)  // UART4 request register
    #define UART4_ISR                    *(volatile unsigned long *)(UART4_BLOCK + 0x1c)  // UART4 interrupt and status register
    #define UART4_ICR                    *(volatile unsigned long *)(UART4_BLOCK + 0x20)  // UART4 interrupt flag clear register (write '1' to clear)
    #define UART4_RDR                    *(volatile unsigned long *)(UART4_BLOCK + 0x24)  // UART4 receive data register
    #define UART4_TDR                    *(volatile unsigned long *)(UART4_BLOCK + 0x28)  // UART4 transmit data register

    #define UART5_CR1                    *(volatile unsigned long *)(UART5_BLOCK + 0x00)  // UART5 control register 1
    #define UART5_CR2                    *(volatile unsigned long *)(UART5_BLOCK + 0x04)  // UART5 control register 2
    #define UART5_CR3                    *(volatile unsigned long *)(UART5_BLOCK + 0x08)  // UART5 control register 3
    #define UART5_BRR                    *(volatile unsigned long *)(UART5_BLOCK + 0x0c)  // UART5 baud rate register
    #define UART5_GTPR                   *(volatile unsigned long *)(UART5_BLOCK + 0x10)  // UART5 guard time and prescaler register
    #define UART5_RTOR                   *(volatile unsigned long *)(UART5_BLOCK + 0x14)  // UART5 receiver timeout register
    #define UART5_RQR                    *(volatile unsigned long *)(UART5_BLOCK + 0x18)  // UART5 request register
    #define UART5_ISR                    *(volatile unsigned long *)(UART5_BLOCK + 0x1c)  // UART5 interrupt and status register
    #define UART5_ICR                    *(volatile unsigned long *)(UART5_BLOCK + 0x20)  // UART5 interrupt flag clear register (write '1' to clear)
    #define UART5_RDR                    *(volatile unsigned long *)(UART5_BLOCK + 0x24)  // UART5 receive data register
    #define UART5_TDR                    *(volatile unsigned long *)(UART5_BLOCK + 0x28)  // UART5 transmit data register
    #if defined USART6_BLOCK
        #define USART6_CR1               *(volatile unsigned long *)(USART6_BLOCK + 0x00)  // USART6 control register 1
        #define USART6_CR2               *(volatile unsigned long *)(USART6_BLOCK + 0x04)  // USART6 control register 2
        #define USART6_CR3               *(volatile unsigned long *)(USART6_BLOCK + 0x08)  // USART6 control register 3
        #define USART6_BRR               *(volatile unsigned long *)(USART6_BLOCK + 0x0c)  // USART6 baud rate register
        #define USART6_GTPR              *(volatile unsigned long *)(USART6_BLOCK + 0x10)  // USART6 guard time and prescaler register
        #define USART6_RTOR              *(volatile unsigned long *)(USART6_BLOCK + 0x14)  // USART6 receiver timeout register
        #define USART6_RQR               *(volatile unsigned long *)(USART6_BLOCK + 0x18)  // USART6 request register
        #define USART6_ISR               *(volatile unsigned long *)(USART6_BLOCK + 0x1c)  // USART6 interrupt and status register
        #define USART6_ICR               *(volatile unsigned long *)(USART6_BLOCK + 0x20)  // USART6 interrupt flag clear register (write '1' to clear)
        #define USART6_RDR               *(volatile unsigned long *)(USART6_BLOCK + 0x24)  // USART6 receive data register
        #define USART6_TDR               *(volatile unsigned long *)(USART6_BLOCK + 0x28)  // USART6 transmit data register
    #endif
    #if defined UART7_BLOCK
        #define UART7_CR1                *(volatile unsigned long *)(UART7_BLOCK + 0x00)  // UART7 control register 1
        #define UART7_CR2                *(volatile unsigned long *)(UART7_BLOCK + 0x04)  // UART7 control register 2
        #define UART7_CR3                *(volatile unsigned long *)(UART7_BLOCK + 0x08)  // UART7 control register 3
        #define UART7_BRR                *(volatile unsigned long *)(UART7_BLOCK + 0x0c)  // UART7 baud rate register
        #define UART7_GTPR               *(volatile unsigned long *)(UART7_BLOCK + 0x10)  // UART7 guard time and prescaler register
        #define UART7_RTOR               *(volatile unsigned long *)(UART7_BLOCK + 0x14)  // UART7 receiver timeout register
        #define UART7_RQR                *(volatile unsigned long *)(UART7_BLOCK + 0x18)  // UART7 request register
        #define UART7_ISR                *(volatile unsigned long *)(UART7_BLOCK + 0x1c)  // UART7 interrupt and status register
        #define UART7_ICR                *(volatile unsigned long *)(UART7_BLOCK + 0x20)  // UART7 interrupt flag clear register (write '1' to clear)
        #define UART7_RDR                *(volatile unsigned long *)(UART7_BLOCK + 0x24)  // UART7 receive data register
        #define UART7_TDR                *(volatile unsigned long *)(UART7_BLOCK + 0x28)  // UART7 transmit data register
    #endif
    #if defined UART8_BLOCK
        #define UART8_CR1                *(volatile unsigned long *)(UART8_BLOCK + 0x00) // UART8 control register 1
        #define UART8_CR2                *(volatile unsigned long *)(UART8_BLOCK + 0x04) // UART8 control register 2
        #define UART8_CR3                *(volatile unsigned long *)(UART8_BLOCK + 0x08) // UART8 control register 3
        #define UART8_BRR                *(volatile unsigned long *)(UART8_BLOCK + 0x0c) // UART8 baud rate register
        #define UART8_GTPR               *(volatile unsigned long *)(UART8_BLOCK + 0x10) // UART8 guard time and prescaler register
        #define UART8_RTOR               *(volatile unsigned long *)(UART8_BLOCK + 0x14) // UART8 receiver timeout register
        #define UART8_RQR                *(volatile unsigned long *)(UART8_BLOCK + 0x18) // UART8 request register
        #define UART8_ISR                *(volatile unsigned long *)(UART8_BLOCK + 0x1c) // UART8 interrupt and status register
        #define UART8_ICR                *(volatile unsigned long *)(UART8_BLOCK + 0x20) // UART8 interrupt flag clear register (write '1' to clear)
        #define UART8_RDR                *(volatile unsigned long *)(UART8_BLOCK + 0x24) // UART8 receive data register
        #define UART8_TDR                *(volatile unsigned long *)(UART8_BLOCK + 0x28) // UART8 transmit data register
    #endif
    #define LPUART1_CR1                  *(volatile unsigned long *)(LPUART1_BLOCK + 0x00) // LPUART1 control register 1
    #define LPUART1_CR2                  *(volatile unsigned long *)(LPUART1_BLOCK + 0x04) // LPUART1 control register 2
    #define LPUART1_CR3                  *(volatile unsigned long *)(LPUART1_BLOCK + 0x08) // LPUART1 control register 3
    #define LPUART1_BRR                  *(volatile unsigned long *)(LPUART1_BLOCK + 0x0c) // LPUART1 baud rate register
    #define LPUART1_RQR                  *(volatile unsigned long *)(LPUART1_BLOCK + 0x18) // LPUART1 request register
    #define LPUART1_ISR                  *(volatile unsigned long *)(LPUART1_BLOCK + 0x1c) // LPUART1 interrupt and status register
    #define LPUART1_ICR                  *(volatile unsigned long *)(LPUART1_BLOCK + 0x20) // LPUART1 interrupt flag clear register (write '1' to clear)
    #define LPUART1_RDR                  *(volatile unsigned long *)(LPUART1_BLOCK + 0x24) // LPUART1 receive data register
    #define LPUART1_TDR                  *(volatile unsigned long *)(LPUART1_BLOCK + 0x28) // LPUART1 transmit data register
#else
    #define USART1_SR                    *(volatile unsigned long *)(USART1_BLOCK + 0x00)  // USART1 status register
    #define USART1_ISR                   USART1_SR                       // for compatibility
      #define USART_SR_PE                0x00000001
      #define USART_SR_FE                0x00000002
      #define USART_SR_NE                0x00000004
      #define USART_SR_ORE               0x00000008
      #define USART_SR_IDLE              0x00000010
      #define USART_SR_RXNE              0x00000020
      #define USART_SR_TC                0x00000040
      #define USART_SR_TXE               0x00000080
      #define USART_SR_LDB               0x00000100
      #define USART_SR_CTS               0x00000200
      #define USART_ISR_TXE              USART_SR_TXE                    // for compatibilty
      #define USART_ISR_RXNE             USART_SR_RXNE                   // for compatibility
    #define USART1_DR                    *(volatile unsigned long *)(USART1_BLOCK + 0x04)  // USART1 data register
    #define USART1_TDR                   USART1_DR                       // for compatibility
    #define USART1_RDR                   USART1_DR
    #define USART1_BRR                   *(volatile unsigned long *)(USART1_BLOCK + 0x08)  // USART1 baud rate register
      #define USART_BRR_DIV_FRACT_MASK   0x0000000f                      // DIV fraction
      #define USART_BRR_DIV_MANT_MASK    0x0000fff0                      // DIV mantissa
    #define USART1_CR1                   *(volatile unsigned long *)(USART1_BLOCK + 0x0c)  // USART1 control register 1
      #define USART_CR1_SBK              0x00000001                      // send break
      #define USART_CR1_RWU              0x00000002                      // receiver wakeup
      #define USART_CR1_RE               0x00000004                      // receiver enable
      #define USART_CR1_TE               0x00000008                      // transmitter enable
      #define USART_CR1_IDLEIE           0x00000010                      // idle interrupt enable
      #define USART_CR1_RXNEIE           0x00000020                      // RXNE interrupt enable
      #define USART_CR1_TCIE             0x00000040                      // transmit complete interrupt enable
      #define USART_CR1_TXEIE            0x00000080                      // transmitter empty interrupt enable
      #define USART_CR1_PEIE             0x00000100                      // parity error interrupt enable
      #define USART_CR1_PS               0x00000200                      // parity select - odd
      #define USART_CR1_PCE              0x00000400                      // parity control enable
      #define USART_CR1_WAKE             0x00000800                      // wakeup method
      #define USART_CR1_M                0x00001000                      // word length 9 bit
      #define USART_CR1_UE               0x00002000                      // USART enable
    #define USART1_CR2                   *(volatile unsigned long *)(USART1_BLOCK + 0x10)  // USART1 control register 2
      #define USART_CR2_ADD_MASK         0x0000000f
      #define USART_CR2_LBDL             0x00000020                      // LIN break detection length
      #define USART_CR2_LBDIE            0x00000040                      // LIN break detection interrupt enable
      #define USART_CR2_LBCL             0x00000100                      // last bit clock pulse - don't write when transmitter is enabled!
      #define USART_CR2_CPHA             0x00000200                      // clock phase - don't write when transmitter is enabled!
      #define USART_CR2_CPOL             0x00000400                      // clock polarity - don't write when transmitter is enabled!
      #define USART_CR2_CLKEN            0x00000800                      // enable CK clock (not UART4 and UART5)
      #define USART_CR2_1_STOP           0x00000000
      #define USART_CR2_HALF_STOP        0x00001000
      #define USART_CR2_2_STOP           0x00002000
      #define USART_CR2_1_5_STOP         0x00003000                      // 0.5 and 1.5 stops not available for UARTs 4 and 5
      #define USART_CR2_LINEN            0x00004000                      // LIN mode enable
    #define USART1_CR3                   *(volatile unsigned long *)(USART1_BLOCK + 0x14)  // USART1 control register 3
      #define USART_CR3_EIE              0x00000001
      #define USART_CR3_IREN             0x00000002
      #define USART_CR3_IRLP             0x00000004
      #define USART_CR3_HDSEL            0x00000008
      #define USART_CR3_NACK             0x00000010
      #define USART_CR3_SCEN             0x00000020
      #define USART_CR3_DMAR             0x00000040
      #define USART_CR3_DMAT             0x00000080
      #define USART_CR3_RTSE             0x00000100
      #define USART_CR3_CTSE             0x00000200
      #define USART_CR3_CTSIE            0x00000400
    #define USART1_GTPR                  *(volatile unsigned long *)(USART1_BLOCK + 0x18)  // USART1 guard time and prescaler register
      #define USART_GTPR_PSC_MASK        0x000000ff
      #define USART_GTPR_GT_MASK         0x0000ff00

    #define CR_OFFSET                    0x0c

    #define USART2_SR                    *(volatile unsigned long *)(USART2_BLOCK + 0x00)  // USART2 status register
    #define USART2_ISR                   USART2_SR                       // for compatibility
    #define USART2_DR                    *(volatile unsigned long *)(USART2_BLOCK + 0x04)  // USART2 data register
    #define USART2_TDR                   USART2_DR                       // for compatibility
    #define USART2_RDR                   USART2_DR
    #define USART2_BRR                   *(volatile unsigned long *)(USART2_BLOCK + 0x08)  // USART2 baud rate register
    #define USART2_CR1                   *(volatile unsigned long *)(USART2_BLOCK + 0x0c)  // USART2 control register 1
    #define USART2_CR2                   *(volatile unsigned long *)(USART2_BLOCK + 0x10)  // USART2 control register 2
    #define USART2_CR3                   *(volatile unsigned long *)(USART2_BLOCK + 0x14)  // USART2 control register 3
    #define USART2_GTPR                  *(volatile unsigned long *)(USART2_BLOCK + 0x18)  // USART2 guard time and prescaler register
    #if !defined USART3_NOT_PRESENT
        #define USART3_SR                *(volatile unsigned long *)(USART3_BLOCK + 0x00)  // USART3 status register
        #define USART3_ISR               USART3_SR                       // for compatibility
        #define USART3_DR                *(volatile unsigned long *)(USART3_BLOCK + 0x04)  // USART3 data register
        #define USART3_TDR               USART3_DR                       // for compatibility
        #define USART3_RDR               USART3_DR
        #define USART3_BRR               *(volatile unsigned long *)(USART3_BLOCK + 0x08)  // USART3 baud rate register
        #define USART3_CR1               *(volatile unsigned long *)(USART3_BLOCK + 0x0c)  // USART3 control register 1
        #define USART3_CR2               *(volatile unsigned long *)(USART3_BLOCK + 0x10)  // USART3 control register 2
        #define USART3_CR3               *(volatile unsigned long *)(USART3_BLOCK + 0x14)  // USART3 control register 3
        #define USART3_GTPR              *(volatile unsigned long *)(USART3_BLOCK + 0x18)  // USART3 guard time and prescaler register
    #endif
    #if !defined UART4_NOT_PRESENT
        #define UART4_SR                 *(volatile unsigned long *)(UART4_BLOCK + 0x00)   // UART4 status register
        #define UART4_ISR                UART4_SR                        // for compatibility
        #define UART4_DR                 *(volatile unsigned long *)(UART4_BLOCK + 0x04)   // UART4 data register
        #define UART4_TDR                UART4_DR                        // for compatibility
        #define UART4_RDR                UART4_DR
        #define UART4_BRR                *(volatile unsigned long *)(UART4_BLOCK + 0x08)   // UART4 baud rate register
        #define UART4_CR1                *(volatile unsigned long *)(UART4_BLOCK + 0x0c)   // UART4 control register 1
        #define UART4_CR2                *(volatile unsigned long *)(UART4_BLOCK + 0x10)   // UART4 control register 2
        #define UART4_CR3                *(volatile unsigned long *)(UART4_BLOCK + 0x14)   // UART4 control register 3
        #define UART4_GTPR               *(volatile unsigned long *)(UART4_BLOCK + 0x18)   // UART4 guard time and prescaler register
    #endif
    #if !defined UART5_NOT_PRESENT
        #define UART5_SR                 *(volatile unsigned long *)(UART5_BLOCK + 0x00)   // UART5 status register
        #define UART5_ISR                UART5_SR                        // for compatibility
        #define UART5_DR                 *(volatile unsigned long *)(UART5_BLOCK + 0x04)   // UART5 data register
        #define UART5_TDR                UART5_DR                        // for compatibility
        #define UART5_RDR                UART5_DR
        #define UART5_BRR                *(volatile unsigned long *)(UART5_BLOCK + 0x08)   // UART5 baud rate register
        #define UART5_CR1                *(volatile unsigned long *)(UART5_BLOCK + 0x0c)   // UART5 control register 1
        #define UART5_CR2                *(volatile unsigned long *)(UART5_BLOCK + 0x10)   // UART5 control register 2
        #define UART5_CR3                *(volatile unsigned long *)(UART5_BLOCK + 0x14)   // UART5 control register 3
        #define UART5_GTPR               *(volatile unsigned long *)(UART5_BLOCK + 0x18)   // UART5 guard time and prescaler register
    #endif
    #if defined _STM32F2XX || defined _STM32F4XX
        #define USART6_SR                *(volatile unsigned long *)(USART6_BLOCK + 0x00)  // USART6 status register
        #define USART6_ISR               USART6_SR                       // for compatibility
        #define USART6_DR                *(volatile unsigned long *)(USART6_BLOCK + 0x04)  // USART6 data register
        #define USART6_TDR               USART6_DR                       // for compatibility
        #define USART6_RDR               USART6_DR
        #define USART6_BRR               *(volatile unsigned long *)(USART6_BLOCK + 0x08)  // USART6 baud rate register
        #define USART6_CR1               *(volatile unsigned long *)(USART6_BLOCK + 0x0c)  // USART6 control register 1
        #define USART6_CR2               *(volatile unsigned long *)(USART6_BLOCK + 0x10)  // USART6 control register 2
        #define USART6_CR3               *(volatile unsigned long *)(USART6_BLOCK + 0x14)  // USART6 control register 3
        #define USART6_GTPR              *(volatile unsigned long *)(USART6_BLOCK + 0x18)  // USART6 guard time and prescaler register
    #endif
    #if defined _STM32F429 || defined _STM32F427
        #define UART7_SR                 *(volatile unsigned long *)(UART7_BLOCK + 0x00)   // UART7 status register
        #define UART7_ISR                UART7_SR                        // for compatibility
        #define UART7_DR                 *(volatile unsigned long *)(UART7_BLOCK + 0x04)   // UART7 data register
        #define UART7_TDR                UART7_DR                        // for compatibility
        #define UART7_RDR                UART7_DR
        #define UART7_BRR                *(volatile unsigned long *)(UART7_BLOCK + 0x08)   // UART7 baud rate register
        #define UART7_CR1                *(volatile unsigned long *)(UART7_BLOCK + 0x0c)   // UART7 control register 1
        #define UART7_CR2                *(volatile unsigned long *)(UART7_BLOCK + 0x10)   // UART7 control register 2
        #define UART7_CR3                *(volatile unsigned long *)(UART7_BLOCK + 0x14)   // UART7 control register 3
        #define UART7_GTPR               *(volatile unsigned long *)(UART7_BLOCK + 0x18)   // UART7 guard time and prescaler register

        #define UART8_SR                 *(volatile unsigned long *)(UART8_BLOCK + 0x00)   // UART8 status register
        #define UART8_ISR                UART8_SR                        // for compatibility
        #define UART8_DR                 *(volatile unsigned long *)(UART8_BLOCK + 0x04)   // UART8 data register
        #define UART8_TDR                UART8_DR                        // for compatibility
        #define UART8_RDR                UART8_DR
        #define UART8_BRR                *(volatile unsigned long *)(UART8_BLOCK + 0x08)   // UART8 baud rate register
        #define UART8_CR1                *(volatile unsigned long *)(UART8_BLOCK + 0x0c)   // UART8 control register 1
        #define UART8_CR2                *(volatile unsigned long *)(UART8_BLOCK + 0x10)   // UART8 control register 2
        #define UART8_CR3                *(volatile unsigned long *)(UART8_BLOCK + 0x14)   // UART8 control register 3
        #define UART8_GTPR               *(volatile unsigned long *)(UART8_BLOCK + 0x18)   // UART8 guard time and prescaler register
    #endif
#endif


typedef struct stUSART_REG
{
#if defined _STM32F7XX || defined _STM32L432 || defined _STM32L0x1 || defined _STM32F031 || defined _STM32L4X5 || defined _STM32L4X6
    unsigned long UART_CR1;
    unsigned long UART_CR2;
    unsigned long UART_CR3;
    unsigned long UART_BRR;
    unsigned long UART_GTPR;
    unsigned long UART_RTOR;
    unsigned long UART_RQR;
    unsigned long UART_ISR;
    unsigned long UART_ICR;
    unsigned long UART_RDR;
    unsigned long UART_TDR;
#else
    unsigned long UART_SR;
    unsigned long UART_DR;
    unsigned long UART_BRR;
    unsigned long UART_CR1;
    unsigned long UART_CR2;
    unsigned long UART_CR3;
    unsigned long UART_GTPR;
#endif
} USART_REG;



// I2C
//
#define I2C1_CR1                        *(volatile unsigned long *)(I2C1_BLOCK + 0x00) // I2C Control Register 1 I2C 1
  #define I2C_CR1_PE                    0x0001                                         // peripheral enable
  #define I2C_CR1_SMBUS                 0x0002                                         // SMBus mode
  #define I2C_CR1_SMBTYPE               0x0008                                         // SMBus type - host
  #define I2C_CR1_ENARP                 0x0010                                         // ARP enable
  #define I2C_CR1_ENPEC                 0x0020                                         // PEC calculation enable
  #define I2C_CR1_ENGC                  0x0040                                         // general call enable
  #define I2C_CR1_NOSTRETCH             0x0080                                         // disable clock stretching (slave mode)
  #define I2C_CR1_START                 0x0100                                         // start generation
  #define I2C_CR1_STOP                  0x0200                                         // stop generation
  #define I2C_CR1_ACK                   0x0400                                         // acknowledge enable
  #define I2C_CR1_POS                   0x0800                                         // acknowledge/PEC position (for data reception)
  #define I2C_CR1_PEC                   0x1000                                         // packed error checking
  #define I2C_CR1_ALERT                 0x2000                                         // SMBus alert
  #define I2C_CR1_SWRST                 0x8000                                         // software reset
#define I2C1_CR2                        *(volatile unsigned long *)(I2C1_BLOCK + 0x04) // I2C Control Register 2 I2C 1
  #define I2C_CR2_FREQ_MASK             0x003f                                         // peripheral clock frequency mask
  #define I2C_CR2_ITERREN               0x0100                                         // error interrupt enable
  #define I2C_CR2_ITEVTEN               0x0200                                         // event interrupt enable
  #define I2C_CR2_ITBUFEN               0x0400                                         // buffer interrupt enable
  #define I2C_CR2_DMAEN                 0x0800                                         // DMA requests enable
  #define I2C_CR2_LAST                  0x1000                                         // DMA last transfer
#define I2C1_OAR1                       *(volatile unsigned long *)(I2C1_BLOCK + 0x08) // I2C Own Address Register 1 I2C 1
#define I2C1_OAR2                       *(volatile unsigned long *)(I2C1_BLOCK + 0x0c) // I2C Own Address Register 2 I2C 1
#define I2C1_DR                         *(volatile unsigned long *)(I2C1_BLOCK + 0x10) // I2C Data Register I2C 1
#define I2C1_SR1                        *(volatile unsigned long *)(I2C1_BLOCK + 0x14) // I2C Status Register 1 I2C 1
  #define I2C_SR1_SB                    0x0001                                         // start bit (master mode)
  #define I2C_SR1_ADDR                  0x0002                                         // address sent/matched
  #define I2C_SR1_BTF                   0x0004                                         // byte transfer finished
  #define I2C_SR1_ADD10                 0x0008                                         // 10-bit header sent (master mode)
  #define I2C_SR1_STOPF                 0x0010                                         // stop detection (slave mode)
  #define I2C_SR1_RxNE                  0x0040                                         // data register not empty (receivers)
  #define I2C_SR1_TxE                   0x0080                                         // data register empty (transmitters)
  #define I2C_SR1_BERR                  0x0100                                         // bus error (cleared by writing 0)
  #define I2C_SR1_ARLO                  0x0200                                         // arbitration lost (master mode) (cleared by writing 0)
  #define I2C_SR1_AF                    0x0400                                         // acknowledge failure (cleared by writing 0)
  #define I2C_SR1_OVR                   0x0800                                         // overrun/underrun (cleared by writing 0)
  #define I2C_SR1_PECERR                0x1000                                         // PEC error in reception (cleared by writing 0)
  #define I2C_SR1_TIMEOUT               0x4000                                         // timeout or Tlow error (cleared by writing 0)
  #define I2C_SR1_SMBALERT              0x8000                                         // SMBus alert (cleared by writing 0)
#define I2C1_SR2                        *(volatile unsigned long *)(I2C1_BLOCK + 0x18) // I2C Status Register 2 I2C 1 (read-only))
  #define I2C_SR2_MSL                   0x0001                                         // master mode
  #define I2C_SR2_BUSY                  0x0002                                         // bus busy
  #define I2C_SR2_TRA                   0x0004                                         // transmitter/receiver
  #define I2C_SR2_GENCALL               0x0010                                         // general call address (slave mode)
  #define I2C_SR2_SMBDEFAULT            0x0020                                         // SMBus device default address (slave mode)
  #define I2C_SR2_SMBHOST               0x0040                                         // SMBus host header (slave mode)
  #define I2C_SR2_DUALF                 0x0080                                         // dual flag (slave mode)
  #define I2C_SR2_PEC_MASK              0xff00                                         // packet error checking register
#define I2C1_CCR                        *(volatile unsigned long *)(I2C1_BLOCK + 0x1c) // I2C Clock Control Register I2C 1 (configure only when I2C controller is disabled)
  #define I2C_CCR_DUTY_16_9             0x4000                                         // fast mode duty cycle
  #define I2C_CCR_FS                    0x8000                                         // fast I2C Mode
#define I2C1_TRISE                      *(unsigned long *)(I2C1_BLOCK + 0x20)          // I2C Rise Time Control Register I2C 1 (configure only when I2C controller is disabled)

#define I2C2_CR1                        *(volatile unsigned long *)(I2C2_BLOCK + 0x00) // I2C Control Register 1 I2C 2
#define I2C2_CR2                        *(volatile unsigned long *)(I2C2_BLOCK + 0x04) // I2C Control Register 2 I2C 2
#define I2C2_OAR1                       *(volatile unsigned long *)(I2C2_BLOCK + 0x08) // I2C Own Address Register 1 I2C 2
#define I2C2_OAR2                       *(volatile unsigned long *)(I2C2_BLOCK + 0x0c) // I2C Own Address Register 2 I2C 2
#define I2C2_DR                         *(volatile unsigned long *)(I2C2_BLOCK + 0x10) // I2C Data Register I2C 2
#define I2C2_SR1                        *(volatile unsigned long *)(I2C2_BLOCK + 0x14) // I2C Status Register 1 I2C 2
#define I2C2_SR2                        *(volatile unsigned long *)(I2C2_BLOCK + 0x18) // I2C Status Register 2 I2C 2
#define I2C2_CCR                        *(volatile unsigned long *)(I2C2_BLOCK + 0x1c) // I2C Clock Control Register I2C 2
#define I2C2_TRISE                      *(unsigned long *)(I2C2_BLOCK + 0x20)          // I2C Rise Time Control Register I2C 2 (configure only when I2C controller is disabled)

#define I2C3_CR1                        *(volatile unsigned long *)(I2C3_BLOCK + 0x00) // I2C Control Register 1 I2C 3
#define I2C3_CR2                        *(volatile unsigned long *)(I2C3_BLOCK + 0x04) // I2C Control Register 2 I2C 3
#define I2C3_OAR1                       *(volatile unsigned long *)(I2C3_BLOCK + 0x08) // I2C Own Address Register 1 I2C 3
#define I2C3_OAR2                       *(volatile unsigned long *)(I2C3_BLOCK + 0x0c) // I2C Own Address Register 2 I2C 3
#define I2C3_DR                         *(volatile unsigned long *)(I2C3_BLOCK + 0x10) // I2C Data Register I2C 3
#define I2C3_SR1                        *(volatile unsigned long *)(I2C3_BLOCK + 0x14) // I2C Status Register 1 I2C 3
#define I2C3_SR2                        *(volatile unsigned long *)(I2C3_BLOCK + 0x18) // I2C Status Register 2 I2C 3
#define I2C3_CCR                        *(volatile unsigned long *)(I2C3_BLOCK + 0x1c) // I2C Clock Control Register I2C 3
#define I2C3_TRISE                      *(unsigned long *)(I2C3_BLOCK + 0x20)          // I2C Rise Time Control Register I2C 3 (configure only when I2C controller is disabled)


typedef struct I2C_REGS
{
volatile unsigned long I2C_CR1;
volatile unsigned long I2C_CR2;
volatile unsigned long I2C_OAR1;
volatile unsigned long I2C_OAR2;
volatile unsigned long I2C_DR;
volatile unsigned long I2C_SR1;
volatile unsigned long I2C_SR2;
volatile unsigned long I2C_CCR;
unsigned long I2C_TRISE;
} I2C_REGS;


// Power Control Block
//
#define PWR_CR                          *(volatile unsigned long *)(PWR_BLOCK + 0x00)  // Power Control Register
  #define PWR_CR_LPDS                   0x00000001                       // low power deep sleep
  #define PWR_CR_PDDS                   0x00000002                       // power down deep sleep
  #if defined _STM32F7XX
    #define PWR_CR_CSBF                 0x00000008                       // clear standby flag (write 1 to clear)
  #else
    #define PWR_CR_CWUF                 0x00000004                       // clear wakeup flag (write 1 to clear - after 2 system clock cycles)
  #endif
  #define PWR_CR_PVDE                   0x00000010                       // power voltage detector enabled
  #define PWR_CR_PLS_2V                 0x00000000                       // power voltage detector level selection - 2.0V
  #define PWR_CR_PLS_2_1V               0x00000020                       // power voltage detector level selection - 2.1V
  #define PWR_CR_PLS_2_3V               0x00000040                       // power voltage detector level selection - 2.3V
  #define PWR_CR_PLS_2_5V               0x00000060                       // power voltage detector level selection - 2.5V
  #define PWR_CR_PLS_2_6V               0x00000080                       // power voltage detector level selection - 2.6V
  #define PWR_CR_PLS_2_7V               0x00000090                       // power voltage detector level selection - 2.7V
  #define PWR_CR_PLS_2_8V               0x000000c0                       // power voltage detector level selection - 2.8V
  #define PWR_CR_PLS_2_9V               0x000000e0                       // power voltage detector level selection - 2.9V
  #define PWR_CR_DBP                    0x00000100                       // disable backup domain write protection
  #define PWR_CR_FPDS                   0x00000200                       // flash power down in stop mode
  #if defined _STM32F7XX
    #define PWR_CR_LPUDS                0x00000400                       // low-power reguator in deep-sleep under-drive mode
    #define PWR_CR_MRUDS                0x00000800                       // main reguator in deep-sleep under-drive mode
    #define PWR_CR_ADCDC1               0x00002000                       // see AN4073 (only set in 2.7..3.6V voltage range)
    #define PWR_CR_VOS_3                0x00004000                       // regulator voltage scaling mode 3
    #define PWR_CR_VOS_2                0x00008000                       // regulator voltage scaling mode 2
    #define PWR_CR_VOS_1                0x0000c000                       // regulator voltage scaling mode 1 (default)
    #define PWR_CR_ODEN                 0x00010000                       // over-drive enable
    #define PWR_CR_ODSWEN               0x00020000                       // over-drive switching enable
    #define PWR_CR_UDEN                 0x000c0000                       // under-drive enable in stop mode
  #else
    #define PWR_CR_VOS                  0x00004000                       // regulator voltage scaling mode 1 (default)
  #endif

// Timers 2..5 - available on all parts (general purpose)
//
#define TIM2_CR1                        *(unsigned long *)(TIM2_BLOCK + 0x00)  // TIM2 Control Register 1
  #define TIM_CR1_CEN                   0x00000001                       // counter enable
  #define TIM_CR1_UDIS                  0x00000002                       // update disable
  #define TIM_CR1_URS_ALL               0x00000000                       // update request source - all
  #define TIM_CR1_URS_UNDER_OVERFLOW    0x00000004                       // update request source - only under/overflow
  #define TIM_CR1_OPM                   0x00000008                       // one pulse mode
  #define TIM_CR1_DIR_UP                0x00000000                       // upcounter
  #define TIM_CR1_DIR_DOWN              0x00000010                       // downcounter
  #define TIM_CR1_CMS_EDGE              0x00000000                       // edge-aligned mode
  #define TIM_CR1_CMS_CENTER_1          0x00000020                       // center-aligned mode 1
  #define TIM_CR1_CMS_CENTER_2          0x00000040                       // center-aligned mode 2
  #define TIM_CR1_CMS_CENTER_3          0x00000060                       // center-aligned mode 3
  #define TIM_CR1_ARPE                  0x00000080                       // auto-reload preload enable
  #define TIM_CR1_CK_INT                0x00000000
  #define TIM_CR1_2CK_INT               0x00000100
  #define TIM_CR1_4CK_INT               0x00000200
#define TIM2_CR2                        *(unsigned long *)(TIM2_BLOCK + 0x04)  // TIM2 Control Register 2
  #define TIM_CR2_CCDS                  0x00000008                       // capture/compare DMA select
  #define TIM_CR2_MMS_RESET             0x00000000                       // master mode selection - reset
  #define TIM_CR2_MMS_ENABLE            0x00000010                       // master mode selection - enable
  #define TIM_CR2_MMS_UPDATE            0x00000020                       // master mode selection - update
  #define TIM_CR2_MMS_COMP_PULSE        0x00000030                       // master mode selection - compare pulse
  #define TIM_CR2_MMS_COMP_OC1REF       0x00000040                       // master mode selection - compare OC1REF
  #define TIM_CR2_MMS_COMP_OC2REF       0x00000050                       // master mode selection - compare OC2REF
  #define TIM_CR2_MMS_COMP_OC3REF       0x00000060                       // master mode selection - compare OC3REF
  #define TIM_CR2_MMS_COMP_OC4REF       0x00000070                       // master mode selection - compare OC4REF
  #define TIM_CR2_TI1S                  0x00000080                       // TI1 selection
#define TIM2_SMCR                       *(unsigned long *)(TIM2_BLOCK + 0x08)  // TIM2 Slave Mode Control Register
#define TIM2_DIER                       *(unsigned long *)(TIM2_BLOCK + 0x0c)  // TIM2 DMA/Interrupt Enable Register
  #define TIM_DIER_UIE                  0x00000001                       // update interrupt enable
  #define TIM_DIER_CC1IE                0x00000002                       // capture/compare channel 1 interrupt enable
  #define TIM_DIER_CC2IE                0x00000004                       // capture/compare channel 2 interrupt enable
  #define TIM_DIER_CC3IE                0x00000008                       // capture/compare channel 3 interrupt enable
  #define TIM_DIER_CC4IE                0x00000010                       // capture/compare channel 4 interrupt enable
  #define TIM_DIER_TIE                  0x00000040                       // trigger interrupt enable
  #define TIM_DIER_UDE                  0x00000100                       // update DMA request enable
  #define TIM_DIER_CC1DE                0x00000200                       // capture/compare channel 1 DMA request enable
  #define TIM_DIER_CC2DE                0x00000400                       // capture/compare channel 2 DMA request enable
  #define TIM_DIER_CC3DE                0x00000800                       // capture/compare channel 3 DMA request enable
  #define TIM_DIER_CC4DE                0x00001000                       // capture/compare channel 4 DMA request enable
  #define TIM_DIER_TDE                  0x00004000                       // trigger DMA request enable
#define TIM2_SR                         *(volatile unsigned long *)(TIM2_BLOCK + 0x10)  // TIM1 Status Register
  #define TIM_SR_UIF                    0x00000001                       // update interrupt flag
  #define TIM_SR_CC1IF                  0x00000002                       // capture/compare channel 1 interrupt flag
  #define TIM_SR_CC2IF                  0x00000004                       // capture/compare channel 2 interrupt flag
  #define TIM_SR_CC3IF                  0x00000008                       // capture/compare channel 3 interrupt flag
  #define TIM_SR_CC4IF                  0x00000010                       // capture/compare channel 4 interrupt flag
  #define TIM_SR_TIF                    0x00000040                       // trigger interrupt flag
  #define TIM_SR_CC1OF                  0x00000200                       // capture/compare channel 1 overcapture flag
  #define TIM_SR_CC2OF                  0x00000400                       // capture/compare channel 2 overcapture flag
  #define TIM_SR_CC3OF                  0x00000800                       // capture/compare channel 3 overcapture flag
  #define TIM_SR_CC4OF                  0x00001000                       // capture/compare channel 4 overcapture flag
#define TIM2_EGR                        *(volatile unsigned long *)(TIM2_BLOCK + 0x14)  // TIM2 Event Generation Register (write-only)
  #define TIM_EGR_UG                    0x00000001                       // update generation
  #define TIM_EGR_CC1G                  0x00000002                       // capture/compare 1 generation
  #define TIM_EGR_CC2G                  0x00000004                       // capture/compare 2 generation
  #define TIM_EGR_CC3G                  0x00000008                       // capture/compare 3 generation
  #define TIM_EGR_CC4G                  0x00000010                       // capture/compare 4 generation
  #define TIM_EGR_TG                    0x00000040                       // trigger generation
#define TIM2_CCMR1                      *(unsigned long *)(TIM2_BLOCK + 0x18)  // TIM2 Capture/Compare Mode Register 1
  #define TIM_CCMR_CCSS_OUTPUT          0x00000000                       // channel is configured as output
  #define TIM_CCMR_CCSS_IN_TI1          0x00000001                       // channel is configured as input, IC1 is mapped on TI1
  #define TIM_CCMR_CCSS_IN_TI2          0x00000002                       // channel is configured as input, IC1 is mapped on TI2
  #define TIM_CCMR_CCSS_IN_TRC          0x00000003                       // channel is configured as input, IC1 is mapped on TRC
  #define TIM_CCMR_OCFE                 0x00000004                       // output compare fast enable
  #define TIM_CCMR_OCPE                 0x00000008                       // output compare pre-load enable
  #define TIM_CCMR_OCM_FROZEN           0x00000000
  #define TIM_CCMR_OCM_MATCH_HIGH       0x00000010                       // force output high on match
  #define TIM_CCMR_OCM_MATCH_LOW        0x00000020                       // force output low on match
  #define TIM_CCMR_OCM_MATCH_TOGGLE     0x00000030                       // force output toggle on match
  #define TIM_CCMR_OCM_PWM_1            0x00000060                       // PWM mode 1
  #define TIM_CCMR_OCM_PWM_2            0x00000070                       // PWM mode 2
  #define TIM_CCMR_OCCE                 0x00000080                       // output compare clear enable
  #define TIM_CCMR_CHANNEL_SHIFT        8
#define TIM2_CCMR2                      *(unsigned long *)(TIM2_BLOCK + 0x1c)  // TIM2 Capture/Compare Mode Register 2
#define TIM2_CCER                       *(unsigned long *)(TIM2_BLOCK + 0x20)  // TIM2 Capture/Compare Enable Register
  #define TIM_CCER_CC1E                 0x00000001
  #define TIM_CCER_CC1P                 0x00000002
  #define TIM_CCER_CC2E                 0x00000010
  #define TIM_CCER_CC2P                 0x00000020
  #define TIM_CCER_CC3E                 0x00000100
  #define TIM_CCER_CC3P                 0x00000200
  #define TIM_CCER_CC4E                 0x00001000
  #define TIM_CCER_CC4P                 0x00002000
  #define TIM_CCER_CHANNEL_SHIFT        4
#define TIM2_CNT                        *(volatile unsigned long *)(TIM2_BLOCK + 0x24)  // TIM1 Counter
#define TIM2_PSC                        *(unsigned long *)(TIM2_BLOCK + 0x28)  // TIM2 Prescaler
#define TIM2_ARR                        *(unsigned long *)(TIM2_BLOCK + 0x2c)  // TIM2 Auto-Reload Register

#define TIM2_CCR1                       *(unsigned long *)(TIM2_BLOCK + 0x34)  // TIM2 Capture/Compare Register 1
#define TIM2_CCR2                       *(unsigned long *)(TIM2_BLOCK + 0x38)  // TIM2 Capture/Compare Register 2
#define TIM2_CCR3                       *(unsigned long *)(TIM2_BLOCK + 0x3c)  // TIM2 Capture/Compare Register 3
#define TIM2_CCR4                       *(unsigned long *)(TIM2_BLOCK + 0x40)  // TIM2 Capture/Compare Register 4

#define TIM2_DCR                        *(unsigned long *)(TIM2_BLOCK + 0x48)  // TIM2 DMA Control Register
#define TIM2_DMAR                       *(unsigned long *)(TIM2_BLOCK + 0x4c)  // TIM2 DMA Address for full Transfer


#define TIM3_CR1                        *(unsigned long *)(TIM3_BLOCK + 0x00)  // TIM3 Control Register 1
#define TIM3_CR2                        *(unsigned long *)(TIM3_BLOCK + 0x04)  // TIM3 Control Register 2
#define TIM3_SMCR                       *(unsigned long *)(TIM3_BLOCK + 0x08)  // TIM3 Slave Mode Control Register
#define TIM3_DIER                       *(unsigned long *)(TIM3_BLOCK + 0x0c)  // TIM3 DMA/Interrupt Enable Register
#define TIM3_SR                         *(volatile unsigned long *)(TIM3_BLOCK + 0x10)  // TIM1 Status Register
#define TIM3_EGR                        *(volatile unsigned long *)(TIM3_BLOCK + 0x14)  // TIM3 Event Generation Register (write-only)
#define TIM3_CCMR1                      *(unsigned long *)(TIM3_BLOCK + 0x18)  // TIM3 Capture/Compare Mode Register 1
#define TIM3_CCMR2                      *(unsigned long *)(TIM3_BLOCK + 0x1c)  // TIM3 Capture/Compare Mode Register 2
#define TIM3_CCER                       *(unsigned long *)(TIM3_BLOCK + 0x20)  // TIM3 Capture/Compare Enable Register
#define TIM3_CNT                        *(volatile unsigned long *)(TIM3_BLOCK + 0x24)  // TIM1 Counter
#define TIM3_PSC                        *(unsigned long *)(TIM3_BLOCK + 0x28)  // TIM3 Prescaler
#define TIM3_ARR                        *(unsigned long *)(TIM3_BLOCK + 0x2c)  // TIM3 Auto-Reload Register

#define TIM3_CCR1                       *(unsigned long *)(TIM3_BLOCK + 0x34)  // TIM3 Capture/Compare Register 1
#define TIM3_CCR2                       *(unsigned long *)(TIM3_BLOCK + 0x38)  // TIM3 Capture/Compare Register 2
#define TIM3_CCR3                       *(unsigned long *)(TIM3_BLOCK + 0x3c)  // TIM3 Capture/Compare Register 3
#define TIM3_CCR4                       *(unsigned long *)(TIM3_BLOCK + 0x40)  // TIM3 Capture/Compare Register 4

#define TIM3_DCR                        *(unsigned long *)(TIM3_BLOCK + 0x48)  // TIM3 DMA Control Register
#define TIM3_DMAR                       *(unsigned long *)(TIM3_BLOCK + 0x4c)  // TIM3 DMA Address for full Transfer

#define TIM4_CR1                        *(unsigned long *)(TIM4_BLOCK + 0x00)  // TIM4 Control Register 1
#define TIM4_CR2                        *(unsigned long *)(TIM4_BLOCK + 0x04)  // TIM4 Control Register 2
#define TIM4_SMCR                       *(unsigned long *)(TIM4_BLOCK + 0x08)  // TIM4 Slave Mode Control Register
#define TIM4_DIER                       *(unsigned long *)(TIM4_BLOCK + 0x0c)  // TIM4 DMA/Interrupt Enable Register
#define TIM4_SR                         *(volatile unsigned long *)(TIM4_BLOCK + 0x10) // TIM1 Status Register
#define TIM4_EGR                        *(volatile unsigned long *)(TIM4_BLOCK + 0x14) // TIM4 Event Generation Register (write-only)
#define TIM4_CCMR1                      *(unsigned long *)(TIM4_BLOCK + 0x18)  // TIM4 Capture/Compare Mode Register 1
#define TIM4_CCMR2                      *(unsigned long *)(TIM4_BLOCK + 0x1c)  // TIM4 Capture/Compare Mode Register 2
#define TIM4_CCER                       *(unsigned long *)(TIM4_BLOCK + 0x20)  // TIM4 Capture/Compare Enable Register
#define TIM4_CNT                        *(volatile unsigned long *)(TIM4_BLOCK + 0x24)  // TIM1 Counter
#define TIM4_PSC                        *(unsigned long *)(TIM4_BLOCK + 0x28)  // TIM4 Prescaler
#define TIM4_ARR                        *(unsigned long *)(TIM4_BLOCK + 0x2c)  // TIM4 Auto-Reload Register

#define TIM4_CCR1                       *(unsigned long *)(TIM4_BLOCK + 0x34)  // TIM4 Capture/Compare Register 1
#define TIM4_CCR2                       *(unsigned long *)(TIM4_BLOCK + 0x38)  // TIM4 Capture/Compare Register 2
#define TIM4_CCR3                       *(unsigned long *)(TIM4_BLOCK + 0x3c)  // TIM4 Capture/Compare Register 3
#define TIM4_CCR4                       *(unsigned long *)(TIM4_BLOCK + 0x40)  // TIM4 Capture/Compare Register 4

#define TIM4_DCR                        *(unsigned long *)(TIM4_BLOCK + 0x48)  // TIM4 DMA Control Register
#define TIM4_DMAR                       *(unsigned long *)(TIM4_BLOCK + 0x4c)  // TIM4 DMA Address for full Transfer

#define TIM5_CR1                        *(unsigned long *)(TIM5_BLOCK + 0x00)  // TIM5 Control Register 1
#define TIM5_CR2                        *(unsigned long *)(TIM5_BLOCK + 0x04)  // TIM5 Control Register 2
#define TIM5_SMCR                       *(unsigned long *)(TIM5_BLOCK + 0x08)  // TIM5 Slave Mode Control Register
#define TIM5_DIER                       *(unsigned long *)(TIM5_BLOCK + 0x0c)  // TIM5 DMA/Interrupt Enable Register
#define TIM5_SR                         *(volatile unsigned long *)(TIM5_BLOCK + 0x10)  // TIM1 Status Register
#define TIM5_EGR                        *(volatile unsigned long *)(TIM5_BLOCK + 0x14)  // TIM5 Event Generation Register (write-only)
#define TIM5_CCMR1                      *(unsigned long *)(TIM5_BLOCK + 0x18)  // TIM5 Capture/Compare Mode Register 1
#define TIM5_CCMR2                      *(unsigned long *)(TIM5_BLOCK + 0x1c)  // TIM5 Capture/Compare Mode Register 2
#define TIM5_CCER                       *(unsigned long *)(TIM5_BLOCK + 0x20)  // TIM5 Capture/Compare Enable Register
#define TIM5_CNT                        *(volatile unsigned long *)(TIM5_BLOCK + 0x24)  // TIM1 Counter
#define TIM5_PSC                        *(unsigned long *)(TIM5_BLOCK + 0x28)  // TIM5 Prescaler
#define TIM5_ARR                        *(unsigned long *)(TIM5_BLOCK + 0x2c)  // TIM5 Auto-Reload Register

#define TIM5_CCR1                       *(unsigned long *)(TIM5_BLOCK + 0x34)  // TIM5 Capture/Compare Register 1
#define TIM5_CCR2                       *(unsigned long *)(TIM5_BLOCK + 0x38)  // TIM5 Capture/Compare Register 2
#define TIM5_CCR3                       *(unsigned long *)(TIM5_BLOCK + 0x3c)  // TIM5 Capture/Compare Register 3
#define TIM5_CCR4                       *(unsigned long *)(TIM5_BLOCK + 0x40)  // TIM5 Capture/Compare Register 4

#define TIM5_DCR                        *(unsigned long *)(TIM5_BLOCK + 0x48)  // TIM5 DMA Control Register
#define TIM5_DMAR                       *(unsigned long *)(TIM5_BLOCK + 0x4c)  // TIM5 DMA Address for full Transfer

typedef struct stTIM2_3_4_5_REGS
{
    unsigned long TIM_CR1;
    unsigned long TIM_CR2;
    unsigned long TIM_SMCR;
    unsigned long TIM_DIER;
    volatile unsigned long TIM_SR;
    volatile unsigned long TIM_EGR;
    unsigned long TIM_CCMR1;
    unsigned long TIM_CCMR2;
    unsigned long TIM_CCER;
    volatile unsigned long TIM_CNT;
    unsigned long TIM_PSC;
    unsigned long TIM_ARR;
    unsigned long ulRes0;
    unsigned long TIM_CCR1;
    unsigned long TIM_CCR2;
    unsigned long TIM_CCR3;
    unsigned long TIM_CCR4;
    unsigned long ulRes1;
    unsigned long TIM_DCR;
    unsigned long TIM_DMAR;
} TIM2_3_4_5_REGS;

#if defined TIMER_9_AVAILABLE
    #define TIM9_CR1                   *(unsigned long *)(TIM9_BLOCK + 0x00)   // TIM9 Control Register 1

    #define TIM9_SMCR                  *(unsigned long *)(TIM9_BLOCK + 0x08)   // TIM9 Slave Mode Control Register
    #define TIM9_DIER                  *(unsigned long *)(TIM9_BLOCK + 0x0c)   // TIM9 DMA/Interrupt Enable Register
    #define TIM9_SR                    *(volatile unsigned long *)(TIM9_BLOCK + 0x10)  // TIM9 Status Register
    #define TIM9_EGR                   *(volatile unsigned long *)(TIM9_BLOCK + 0x14)  // TIM9 Event Generation Register (write-only)
    #define TIM9_CCMR1                 *(unsigned long *)(TIM9_BLOCK + 0x18)   // TIM9 Capture/Compare Mode Register 1

    #define TIM9_CCER                  *(unsigned long *)(TIM9_BLOCK + 0x20)   // TIM9 Capture/Compare Enable Register
    #define TIM9_CNT                   *(volatile unsigned long *)(TIM9_BLOCK + 0x24) // TIM9 Counter
    #define TIM9_PSC                   *(unsigned long *)(TIM9_BLOCK + 0x28)   // TIM9 Prescaler
    #define TIM9_ARR                   *(unsigned long *)(TIM9_BLOCK + 0x2c)   // TIM9 Auto-Reload Register

    #define TIM9_CCR1                  *(unsigned long *)(TIM9_BLOCK + 0x34)   // TIM9 Capture/Compare Register 1
#endif

#if defined TIMER_10_AVAILABLE
    #define TIM10_CR1                   *(unsigned long *)(TIM10_BLOCK + 0x00)  // TIM10 Control Register 1

    #define TIM10_SMCR                  *(unsigned long *)(TIM10_BLOCK + 0x08)  // TIM10 Slave Mode Control Register
    #define TIM10_DIER                  *(unsigned long *)(TIM10_BLOCK + 0x0c)  // TIM10 DMA/Interrupt Enable Register
    #define TIM10_SR                    *(volatile unsigned long *)(TIM10_BLOCK + 0x10)  // TIM10 Status Register
    #define TIM10_EGR                   *(volatile unsigned long *)(TIM10_BLOCK + 0x14)  // TIM10 Event Generation Register (write-only)
    #define TIM10_CCMR1                 *(unsigned long *)(TIM10_BLOCK + 0x18)  // TIM10 Capture/Compare Mode Register 1

    #define TIM10_CCER                  *(unsigned long *)(TIM10_BLOCK + 0x20)  // TIM10 Capture/Compare Enable Register
    #define TIM10_CNT                   *(volatile unsigned long *)(TIM10_BLOCK + 0x24) // TIM10 Counter
    #define TIM10_PSC                   *(unsigned long *)(TIM10_BLOCK + 0x28)  // TIM10 Prescaler
    #define TIM10_ARR                   *(unsigned long *)(TIM10_BLOCK + 0x2c)  // TIM10 Auto-Reload Register

    #define TIM10_CCR1                  *(unsigned long *)(TIM10_BLOCK + 0x34)  // TIM10 Capture/Compare Register 1
#endif

#if defined TIMER_11_AVAILABLE
    #define TIM11_CR1                   *(unsigned long *)(TIM11_BLOCK + 0x00)  // TIM11 Control Register 1

    #define TIM11_SMCR                  *(unsigned long *)(TIM11_BLOCK + 0x08)  // TIM11 Slave Mode Control Register
    #define TIM11_DIER                  *(unsigned long *)(TIM11_BLOCK + 0x0c)  // TIM11 DMA/Interrupt Enable Register
    #define TIM11_SR                    *(volatile unsigned long *)(TIM11_BLOCK + 0x10)  // TIM11 Status Register
    #define TIM11_EGR                   *(volatile unsigned long *)(TIM11_BLOCK + 0x14)  // TIM11 Event Generation Register (write-only)
    #define TIM11_CCMR1                 *(unsigned long *)(TIM11_BLOCK + 0x18)  // TIM11 Capture/Compare Mode Register 1

    #define TIM11_CCER                  *(unsigned long *)(TIM11_BLOCK + 0x20)  // TIM11 Capture/Compare Enable Register
    #define TIM11_CNT                   *(volatile unsigned long *)(TIM11_BLOCK + 0x24) // TIM11 Counter
    #define TIM11_PSC                   *(unsigned long *)(TIM11_BLOCK + 0x28)  // TIM11 Prescaler
    #define TIM11_ARR                   *(unsigned long *)(TIM11_BLOCK + 0x2c)  // TIM11 Auto-Reload Register

    #define TIM11_CCR1                  *(unsigned long *)(TIM11_BLOCK + 0x34)  // TIM11 Capture/Compare Register 1

    #define TIM11_OR                    *(unsigned long *)(TIM11_BLOCK + 0x50)  // TIM11 Option Register 1
#endif

#if defined TIMER_12_AVAILABLE
    #define TIM12_CR1                   *(unsigned long *)(TIM12_BLOCK + 0x00)  // TIM12 Control Register 1

    #define TIM12_SMCR                  *(unsigned long *)(TIM12_BLOCK + 0x08)  // TIM12 Slave Mode Control Register
    #define TIM12_DIER                  *(unsigned long *)(TIM12_BLOCK + 0x0c)  // TIM12 DMA/Interrupt Enable Register
    #define TIM12_SR                    *(volatile unsigned long *)(TIM12_BLOCK + 0x10) // TIM12 Status Register
    #define TIM12_EGR                   *(volatile unsigned long *)(TIM12_BLOCK + 0x14) // TIM12 Event Generation Register (write-only)
    #define TIM12_CCMR1                 *(unsigned long *)(TIM12_BLOCK + 0x18)  // TIM12 Capture/Compare Mode Register 1

    #define TIM12_CCER                  *(unsigned long *)(TIM12_BLOCK + 0x20)  // TIM12 Capture/Compare Enable Register
    #define TIM12_CNT                   *(volatile unsigned long *)(TIM12_BLOCK + 0x24) // TIM12 Counter
    #define TIM12_PSC                   *(unsigned long *)(TIM12_BLOCK + 0x28)  // TIM12 Prescaler
    #define TIM12_ARR                   *(unsigned long *)(TIM12_BLOCK + 0x2c)  // TIM12 Auto-Reload Register

    #define TIM12_CCR1                  *(unsigned long *)(TIM12_BLOCK + 0x34)  // TIM12 Capture/Compare Register 1
#endif

#if defined TIMER_13_AVAILABLE
    #define TIM13_CR1                   *(unsigned long *)(TIM13_BLOCK + 0x00)  // TIM13 Control Register 1

    #define TIM13_SMCR                  *(unsigned long *)(TIM13_BLOCK + 0x08)  // TIM13 Slave Mode Control Register
    #define TIM13_DIER                  *(unsigned long *)(TIM13_BLOCK + 0x0c)  // TIM13 DMA/Interrupt Enable Register
    #define TIM13_SR                    *(volatile unsigned long *)(TIM13_BLOCK + 0x10)  // TIM13 Status Register
    #define TIM13_EGR                   *(volatile unsigned long *)(TIM13_BLOCK + 0x14)  // TIM13 Event Generation Register (write-only)
    #define TIM13_CCMR1                 *(unsigned long *)(TIM13_BLOCK + 0x18)  // TIM13 Capture/Compare Mode Register 1

    #define TIM13_CCER                  *(unsigned long *)(TIM13_BLOCK + 0x20)  // TIM13 Capture/Compare Enable Register
    #define TIM13_CNT                   *(volatile unsigned long *)(TIM13_BLOCK + 0x24) // TIM13 Counter
    #define TIM13_PSC                   *(unsigned long *)(TIM13_BLOCK + 0x28)  // TIM13 Prescaler
    #define TIM13_ARR                   *(unsigned long *)(TIM13_BLOCK + 0x2c)  // TIM13 Auto-Reload Register

    #define TIM13_CCR1                  *(unsigned long *)(TIM13_BLOCK + 0x34)  // TIM13 Capture/Compare Register 1
#endif

#if defined TIMER_14_AVAILABLE
    #define TIM14_CR1                   *(unsigned long *)(TIM14_BLOCK + 0x00)  // TIM14 Control Register 1

    #define TIM14_SMCR                  *(unsigned long *)(TIM14_BLOCK + 0x08)  // TIM14 Slave Mode Control Register
    #define TIM14_DIER                  *(unsigned long *)(TIM14_BLOCK + 0x0c)  // TIM14 DMA/Interrupt Enable Register
    #define TIM14_SR                    *(volatile unsigned long *)(TIM14_BLOCK + 0x10)  // TIM14 Status Register
    #define TIM14_EGR                   *(volatile unsigned long *)(TIM14_BLOCK + 0x14)  // TIM14 Event Generation Register (write-only)
    #define TIM14_CCMR1                 *(unsigned long *)(TIM14_BLOCK + 0x18)  // TIM14 Capture/Compare Mode Register 1

    #define TIM14_CCER                  *(unsigned long *)(TIM14_BLOCK + 0x20)  // TIM14 Capture/Compare Enable Register
    #define TIM14_CNT                   *(volatile unsigned long *)(TIM14_BLOCK + 0x24) // TIM14 Counter
    #define TIM14_PSC                   *(unsigned long *)(TIM14_BLOCK + 0x28)  // TIM14 Prescaler
    #define TIM14_ARR                   *(unsigned long *)(TIM14_BLOCK + 0x2c)  // TIM14 Auto-Reload Register

    #define TIM14_CCR1                  *(unsigned long *)(TIM14_BLOCK + 0x34)  // TIM14 Capture/Compare Register 1
#endif

typedef struct stTIM9_10_11_13_12_14_REGS
{
    unsigned long TIM9_10_11_12_13_14_CR1;
    unsigned long ulRes0;
    unsigned long TIM9_10_11_12_13_14_SMCR;
    unsigned long TIM9_10_11_12_13_14_DIER;
    volatile unsigned long TIM9_10_11_12_13_14_SR;
    volatile unsigned long TIM9_10_11_12_13_14_EGR;
    unsigned long TIM9_10_11_12_13_14_CCMR1;
    unsigned long ulRes1;
    unsigned long TIM9_10_11_12_13_14_CCER;
    volatile unsigned long TIM9_10_11_12_13_14_CNT;
    unsigned long TIM9_10_11_12_13_14_PSC;
    unsigned long TIM9_10_11_12_13_14_ARR;
    unsigned long ulRes2;
    unsigned long TIM9_10_11_12_13_14_CCR1;
    unsigned long ulRes3[6];
    unsigned long TIM11OR;                                               // only timer 11
} TIM9_10_11_12_13_14_REGS;


// External Interrupt/Event Controller
//
#define EXTI_IMR                        *(unsigned long *)(EXTI_BLOCK + 0x00)  // EXTI Interrupt Mask Register
  #define EXTI_LINE_0                   0x00000001
  #define EXTI_LINE_1                   0x00000002
  #define EXTI_LINE_2                   0x00000004
  #define EXTI_LINE_3                   0x00000008
  #define EXTI_LINE_4                   0x00000010
  #define EXTI_LINE_5                   0x00000020
  #define EXTI_LINE_6                   0x00000040
  #define EXTI_LINE_7                   0x00000080
  #define EXTI_LINE_8                   0x00000100
  #define EXTI_LINE_9                   0x00000200
  #define EXTI_LINE_10                  0x00000400
  #define EXTI_LINE_11                  0x00000800
  #define EXTI_LINE_12                  0x00001000
  #define EXTI_LINE_13                  0x00002000
  #define EXTI_LINE_14                  0x00004000
  #define EXTI_LINE_15                  0x00008000
  #define EXTI_LINE_16                  0x00010000
  #define EXTI_LINE_17                  0x00020000
  #define EXTI_LINE_18                  0x00040000
  #define EXTI_LINE_19                  0x00080000
#if defined _STM32F2XX || defined _STM32F4XX || defined _STM32F7XX
  #define EXTI_LINE_20                  0x00100000
  #define EXTI_LINE_21                  0x00200000
  #define EXTI_LINE_22                  0x00400000
#endif
#define EXTI_EMR                        *(unsigned long *)(EXTI_BLOCK + 0x04)  // EXTI Event Mask Register
#define EXTI_RTSR                       *(unsigned long *)(EXTI_BLOCK + 0x08)  // EXTI Rising Trigger Selection Register
#define EXTI_FTSR                       *(unsigned long *)(EXTI_BLOCK + 0x0c)  // EXTI Falling Trigger Selection Register
#define EXTI_SWIER                      *(unsigned long *)(EXTI_BLOCK + 0x10)  // EXTI Software Interrupt Event Register
#define EXTI_PR                         *(volatile unsigned long *)(EXTI_BLOCK + 0x14) // EXTI Pending Register

// Ports
//
#if defined _STM32F2XX || defined _STM32F4XX || defined _STM32F7XX || defined _STM32L432 || defined _STM32L0x1 || defined _STM32F031 || defined _STM32L4X5 || defined _STM32L4X6
    #define GPIOA_MODER                 *(unsigned long *)(GPIO_PORTA_BLOCK + 0x00)           // Port A Mode Register
      #define GPIO_MODER_INPUT          0x0
      #define GPIO_MODER_OUTPUT         0x1
      #define GPIO_MODER_ALTERNATE_FUNC 0x2
      #define GPIO_MODER_ANALOG         0x3
    #define GPIOA_OTYPER                *(unsigned long *)(GPIO_PORTA_BLOCK + 0x04)           // Port A Output Type Register
      #define GPIO_OTYPER_PUSH_PULL     0x0
      #define GPIO_OTYPER_OPEN_DRAIN    0x1
    #define GPIOA_OSPEEDR               *(unsigned long *)(GPIO_PORTA_BLOCK + 0x08)           // Port A Output Speed Register
      #define GPIO_OSPEEDR_2MHz         0x0
      #define GPIO_OSPEEDR_25MHz        0x1
      #define GPIO_OSPEEDR_50MHz        0x2
      #define GPIO_OSPEEDR_100MHz       0x3
    #define GPIOA_PUPDR                 *(unsigned long *)(GPIO_PORTA_BLOCK + 0x0c)           // Port A Pullup/Pulldown Register
      #define GPIO_PUPDR_NONE           0x0
      #define GPIO_PUPDR_PULL_UP        0x1
      #define GPIO_PUPDR_PULL_DOWN      0x2
    #define GPIOA_IDR                   *(volatile unsigned long *)(GPIO_PORTA_BLOCK + 0x10)  // Port A Input Data Register (read-only)
    #define GPIOA_ODR                   *(volatile unsigned long *)(GPIO_PORTA_BLOCK + 0x14)  // Port A Output Data Register
    #define GPIOA_BSRR                  *(volatile unsigned long *)(GPIO_PORTA_BLOCK + 0x18)  // Port A Bit Set/Reset Register (write-only)
    #define GPIOA_LCKR                  *(unsigned long *)(GPIO_PORTA_BLOCK + 0x1c)           // Port A Configuration Lock Register
    #define GPIOA_AFRL                  *(unsigned long *)(GPIO_PORTA_BLOCK + 0x20)           // Port A Alternate Function Low Register
    #define GPIOA_AFRH                  *(unsigned long *)(GPIO_PORTA_BLOCK + 0x24)           // Port A Alternate Function High Register
    #if defined _STM32L432 || defined _STM32L0x1 || defined _STM32F031 || defined _STM32L4X5 || defined _STM32L4X6
        #define GPIOA_BRR               *(unsigned long *)(GPIO_PORTA_BLOCK + 0x28)           // Port A port bit reset register
    #endif
    #if defined _STM32L4X5 || defined _STM32L4X6
        #define GPIOA_ASCR              *(unsigned long *)(GPIO_PORTA_BLOCK + 0x2c)           // Port A port analog switch control register
    #endif

    #define GPIOB_MODER                 *(unsigned long *)(GPIO_PORTB_BLOCK + 0x00)           // Port B Mode Register
    #define GPIOB_OTYPER                *(unsigned long *)(GPIO_PORTB_BLOCK + 0x04)           // Port B Output Type Register
    #define GPIOB_OSPEEDR               *(unsigned long *)(GPIO_PORTB_BLOCK + 0x08)           // Port B Output Speed Register
    #define GPIOB_PUPDR                 *(unsigned long *)(GPIO_PORTB_BLOCK + 0x0c)           // Port B Pullup/Pulldown Register
    #define GPIOB_IDR                   *(volatile unsigned long *)(GPIO_PORTB_BLOCK + 0x10)  // Port B Input Data Register (read-only)
    #define GPIOB_ODR                   *(volatile unsigned long *)(GPIO_PORTB_BLOCK + 0x14)  // Port B Output Data Register
    #define GPIOB_BSRR                  *(volatile unsigned long *)(GPIO_PORTB_BLOCK + 0x18)  // Port B Bit Set/Reset Register (write-only)
    #define GPIOB_LCKR                  *(unsigned long *)(GPIO_PORTB_BLOCK + 0x1c)           // Port B Configuration Lock Register
    #define GPIOB_AFRL                  *(unsigned long *)(GPIO_PORTB_BLOCK + 0x20)           // Port B Alternate Function Low Register
    #define GPIOB_AFRH                  *(unsigned long *)(GPIO_PORTB_BLOCK + 0x24)           // Port B Alternate Function High Register
    #if defined _STM32L432 || defined _STM32L0x1 || defined _STM32F031 || defined _STM32L4X5 || defined _STM32L4X6
        #define GPIOB_BRR               *(unsigned long *)(GPIO_PORTB_BLOCK + 0x28)           // Port B port bit reset register
    #endif
    #if defined _STM32L4X5 || defined _STM32L4X6
        #define GPIOB_ASCR              *(unsigned long *)(GPIO_PORTB_BLOCK + 0x2c)           // Port B port analog switch control register
    #endif

    #define GPIOC_MODER                 *(unsigned long *)(GPIO_PORTC_BLOCK + 0x00)           // Port C Mode Register
    #define GPIOC_OTYPER                *(unsigned long *)(GPIO_PORTC_BLOCK + 0x04)           // Port C Output Type Register
    #define GPIOC_OSPEEDR               *(unsigned long *)(GPIO_PORTC_BLOCK + 0x08)           // Port C Output Speed Register
    #define GPIOC_PUPDR                 *(unsigned long *)(GPIO_PORTC_BLOCK + 0x0c)           // Port C Pullup/Pulldown Register
    #define GPIOC_IDR                   *(volatile unsigned long *)(GPIO_PORTC_BLOCK + 0x10)  // Port C Input Data Register (read-only)
    #define GPIOC_ODR                   *(volatile unsigned long *)(GPIO_PORTC_BLOCK + 0x14)  // Port C Output Data Register
    #define GPIOC_BSRR                  *(volatile unsigned long *)(GPIO_PORTC_BLOCK + 0x18)  // Port C Bit Set/Reset Register (write-only)
    #define GPIOC_LCKR                  *(unsigned long *)(GPIO_PORTC_BLOCK + 0x1c)           // Port C Configuration Lock Register
    #define GPIOC_AFRL                  *(unsigned long *)(GPIO_PORTC_BLOCK + 0x20)           // Port C Alternate Function Low Register
    #define GPIOC_AFRH                  *(unsigned long *)(GPIO_PORTC_BLOCK + 0x24)           // Port C Alternate Function High Register
    #if defined _STM32L432 || defined _STM32L0x1 || defined _STM32F031 || defined _STM32L4X5 || defined _STM32L4X6
        #define GPIOC_BRR               *(unsigned long *)(GPIO_PORTC_BLOCK + 0x28)           // Port C port bit reset register
    #endif
    #if defined _STM32L4X5 || defined _STM32L4X6
        #define GPIOC_ASCR              *(unsigned long *)(GPIO_PORTC_BLOCK + 0x2c)           // Port C port analog switch control register
    #endif

    #define GPIOD_MODER                 *(unsigned long *)(GPIO_PORTD_BLOCK + 0x00)           // Port D Mode Register
    #define GPIOD_OTYPER                *(unsigned long *)(GPIO_PORTD_BLOCK + 0x04)           // Port D Output Type Register
    #define GPIOD_OSPEEDR               *(unsigned long *)(GPIO_PORTD_BLOCK + 0x08)           // Port D Output Speed Register
    #define GPIOD_PUPDR                 *(unsigned long *)(GPIO_PORTD_BLOCK + 0x0c)           // Port D Pullup/Pulldown Register
    #define GPIOD_IDR                   *(volatile unsigned long *)(GPIO_PORTD_BLOCK + 0x10)  // Port D Input Data Register (read-only)
    #define GPIOD_ODR                   *(volatile unsigned long *)(GPIO_PORTD_BLOCK + 0x14)  // Port D Output Data Register
    #define GPIOD_BSRR                  *(volatile unsigned long *)(GPIO_PORTD_BLOCK + 0x18)  // Port D Bit Set/Reset Register (write-only)
    #define GPIOD_LCKR                  *(unsigned long *)(GPIO_PORTD_BLOCK + 0x1c)           // Port D Configuration Lock Register
    #define GPIOD_AFRL                  *(unsigned long *)(GPIO_PORTD_BLOCK + 0x20)           // Port D Alternate Function Low Register
    #define GPIOD_AFRH                  *(unsigned long *)(GPIO_PORTD_BLOCK + 0x24)           // Port D Alternate Function High Register
    #if defined _STM32L432 || defined _STM32L0x1 || defined _STM32F031 || defined _STM32L4X5 || defined _STM32L4X6
        #define GPIOD_BRR               *(unsigned long *)(GPIO_PORTD_BLOCK + 0x28)           // Port D port bit reset register
    #endif
    #if defined _STM32L4X5 || defined _STM32L4X6
        #define GPIOD_ASCR              *(unsigned long *)(GPIO_PORTD_BLOCK + 0x2c)           // Port D port analog switch control register
    #endif

    #define GPIOE_MODER                 *(unsigned long *)(GPIO_PORTE_BLOCK + 0x00)           // Port E Mode Register
    #define GPIOE_OTYPER                *(unsigned long *)(GPIO_PORTE_BLOCK + 0x04)           // Port E Output Type Register
    #define GPIOE_OSPEEDR               *(unsigned long *)(GPIO_PORTE_BLOCK + 0x08)           // Port E Output Speed Register
    #define GPIOE_PUPDR                 *(unsigned long *)(GPIO_PORTE_BLOCK + 0x0c)           // Port E Pullup/Pulldown Register
    #define GPIOE_IDR                   *(volatile unsigned long *)(GPIO_PORTE_BLOCK + 0x10)  // Port E Input Data Register (read-only)
    #define GPIOE_ODR                   *(volatile unsigned long *)(GPIO_PORTE_BLOCK + 0x14)  // Port E Output Data Register
    #define GPIOE_BSRR                  *(volatile unsigned long *)(GPIO_PORTE_BLOCK + 0x18)  // Port E Bit Set/Reset Register (write-only)
    #define GPIOE_LCKR                  *(unsigned long *)(GPIO_PORTE_BLOCK + 0x1c)           // Port E Configuration Lock Register
    #define GPIOE_AFRL                  *(unsigned long *)(GPIO_PORTE_BLOCK + 0x20)           // Port E Alternate Function Low Register
    #define GPIOE_AFRH                  *(unsigned long *)(GPIO_PORTE_BLOCK + 0x24)           // Port E Alternate Function High Register
    #if defined _STM32L432 || defined _STM32L0x1 || defined _STM32F031 || defined _STM32L4X5 || defined _STM32L4X6
        #define GPIOE_BRR               *(unsigned long *)(GPIO_PORTE_BLOCK + 0x28)           // Port E port bit reset register
    #endif
    #if defined _STM32L4X5 || defined _STM32L4X6
        #define GPIOE_ASCR              *(unsigned long *)(GPIO_PORTE_BLOCK + 0x2c)           // Port E port analog switch control register
    #endif

    #define GPIOF_MODER                 *(unsigned long *)(GPIO_PORTF_BLOCK + 0x00)           // Port F Mode Register
    #define GPIOF_OTYPER                *(unsigned long *)(GPIO_PORTF_BLOCK + 0x04)           // Port F Output Type Register
    #define GPIOF_OSPEEDR               *(unsigned long *)(GPIO_PORTF_BLOCK + 0x08)           // Port F Output Speed Register
    #define GPIOF_PUPDR                 *(unsigned long *)(GPIO_PORTF_BLOCK + 0x0c)           // Port F Pullup/Pulldown Register
    #define GPIOF_IDR                   *(volatile unsigned long *)(GPIO_PORTF_BLOCK + 0x10)  // Port F Input Data Register (read-only)
    #define GPIOF_ODR                   *(volatile unsigned long *)(GPIO_PORTF_BLOCK + 0x14)  // Port F Output Data Register
    #define GPIOF_BSRR                  *(volatile unsigned long *)(GPIO_PORTF_BLOCK + 0x18)  // Port F Bit Set/Reset Register (write-only)
    #define GPIOF_LCKR                  *(unsigned long *)(GPIO_PORTF_BLOCK + 0x1c)           // Port F Configuration Lock Register
    #define GPIOF_AFRL                  *(unsigned long *)(GPIO_PORTF_BLOCK + 0x20)           // Port F Alternate Function Low Register
    #define GPIOF_AFRH                  *(unsigned long *)(GPIO_PORTF_BLOCK + 0x24)           // Port F Alternate Function High Register
    #if defined _STM32L432 || defined _STM32L0x1 || defined _STM32F031 || defined _STM32L4X5 || defined _STM32L4X6
        #define GPIOF_BRR               *(unsigned long *)(GPIO_PORTF_BLOCK + 0x28)           // Port F port bit reset register
    #endif
    #if defined _STM32L4X5 || defined _STM32L4X6
        #define GPIOF_ASCR              *(unsigned long *)(GPIO_PORTF_BLOCK + 0x2c)           // Port F port analog switch control register
    #endif

    #define GPIOG_MODER                 *(unsigned long *)(GPIO_PORTG_BLOCK + 0x00)           // Port G Mode Register
    #define GPIOG_OTYPER                *(unsigned long *)(GPIO_PORTG_BLOCK + 0x04)           // Port G Output Type Register
    #define GPIOG_OSPEEDR               *(unsigned long *)(GPIO_PORTG_BLOCK + 0x08)           // Port G Output Speed Register
    #define GPIOG_PUPDR                 *(unsigned long *)(GPIO_PORTG_BLOCK + 0x0c)           // Port G Pullup/Pulldown Register
    #define GPIOG_IDR                   *(volatile unsigned long *)(GPIO_PORTG_BLOCK + 0x10)  // Port G Input Data Register (read-only)
    #define GPIOG_ODR                   *(volatile unsigned long *)(GPIO_PORTG_BLOCK + 0x14)  // Port G Output Data Register
    #define GPIOG_BSRR                  *(volatile unsigned long *)(GPIO_PORTG_BLOCK + 0x18)  // Port G Bit Set/Reset Register (write-only)
    #define GPIOG_LCKR                  *(unsigned long *)(GPIO_PORTG_BLOCK + 0x1c)           // Port G Configuration Lock Register
    #define GPIOG_AFRL                  *(unsigned long *)(GPIO_PORTG_BLOCK + 0x20)           // Port G Alternate Function Low Register
    #define GPIOG_AFRH                  *(unsigned long *)(GPIO_PORTG_BLOCK + 0x24)           // Port G Alternate Function High Register
    #if defined _STM32L432 || defined _STM32L0x1 || defined _STM32F031 || defined _STM32L4X5 || defined _STM32L4X6
        #define GPIOG_BRR               *(unsigned long *)(GPIO_PORTG_BLOCK + 0x28)           // Port G port bit reset register
    #endif
    #if defined _STM32L4X5 || defined _STM32L4X6
        #define GPIOG_ASCR              *(unsigned long *)(GPIO_PORTG_BLOCK + 0x2c)           // Port G port analog switch control register
    #endif

    #define GPIOH_MODER                 *(unsigned long *)(GPIO_PORTH_BLOCK + 0x00)           // Port H Mode Register
    #define GPIOH_OTYPER                *(unsigned long *)(GPIO_PORTH_BLOCK + 0x04)           // Port H Output Type Register
    #define GPIOH_OSPEEDR               *(unsigned long *)(GPIO_PORTH_BLOCK + 0x08)           // Port H Output Speed Register
    #define GPIOH_PUPDR                 *(unsigned long *)(GPIO_PORTH_BLOCK + 0x0c)           // Port H Pullup/Pulldown Register
    #define GPIOH_IDR                   *(volatile unsigned long *)(GPIO_PORTH_BLOCK + 0x10)  // Port H Input Data Register (read-only)
    #define GPIOH_ODR                   *(volatile unsigned long *)(GPIO_PORTH_BLOCK + 0x14)  // Port H Output Data Register
    #define GPIOH_BSRR                  *(volatile unsigned long *)(GPIO_PORTH_BLOCK + 0x18)  // Port H Bit Set/Reset Register (write-only)
    #define GPIOH_LCKR                  *(unsigned long *)(GPIO_PORTH_BLOCK + 0x1c)           // Port H Configuration Lock Register
    #define GPIOH_AFRL                  *(unsigned long *)(GPIO_PORTH_BLOCK + 0x20)           // Port H Alternate Function Low Register
    #define GPIOH_AFRH                  *(unsigned long *)(GPIO_PORTH_BLOCK + 0x24)           // Port H Alternate Function High Register
    #if defined _STM32L432 || defined _STM32L0x1 || defined _STM32F031 || defined _STM32L4X5 || defined _STM32L4X6
        #define GPIOH_BRR               *(unsigned long *)(GPIO_PORTH_BLOCK + 0x28)           // Port H port bit reset register
    #endif
    #if defined _STM32L4X5 || defined _STM32L4X6
        #define GPIOH_ASCR              *(unsigned long *)(GPIO_PORTH_BLOCK + 0x2c)           // Port H port analog switch control register
    #endif

    #define GPIOI_MODER                 *(unsigned long *)(GPIO_PORTI_BLOCK + 0x00)           // Port I Mode Register
    #define GPIOI_OTYPER                *(unsigned long *)(GPIO_PORTI_BLOCK + 0x04)           // Port I Output Type Register
    #define GPIOI_OSPEEDR               *(unsigned long *)(GPIO_PORTI_BLOCK + 0x08)           // Port I Output Speed Register
    #define GPIOI_PUPDR                 *(unsigned long *)(GPIO_PORTI_BLOCK + 0x0c)           // Port I Pullup/Pulldown Register
    #define GPIOI_IDR                   *(volatile unsigned long *)(GPIO_PORTI_BLOCK + 0x10)  // Port I Input Data Register (read-only)
    #define GPIOI_ODR                   *(volatile unsigned long *)(GPIO_PORTI_BLOCK + 0x14)  // Port I Output Data Register
    #define GPIOI_BSRR                  *(volatile unsigned long *)(GPIO_PORTI_BLOCK + 0x18)  // Port I Bit Set/Reset Register (write-only)
    #define GPIOI_LCKR                  *(unsigned long *)(GPIO_PORTI_BLOCK + 0x1c)           // Port I Configuration Lock Register
    #define GPIOI_AFRL                  *(unsigned long *)(GPIO_PORTI_BLOCK + 0x20)           // Port I Alternate Function Low Register
    #define GPIOI_AFRH                  *(unsigned long *)(GPIO_PORTI_BLOCK + 0x24)           // Port I Alternate Function High Register
    #if defined _STM32L432 || defined _STM32L0x1 || defined _STM32F031 || defined _STM32L4X5 || defined _STM32L4X6
        #define GPIOI_BRR               *(unsigned long *)(GPIO_PORTI_BLOCK + 0x28)           // Port I port bit reset register
    #endif
    #if defined _STM32L4X5 || defined _STM32L4X6
        #define GPIOI_ASCR              *(unsigned long *)(GPIO_PORTI_BLOCK + 0x2c)           // Port I port analog switch control register
    #endif

  #if defined _STM32F7XX
    #define GPIOJ_MODER                 *(unsigned long *)(GPIO_PORTJ_BLOCK + 0x00)           // Port J Mode Register
    #define GPIOJ_OTYPER                *(unsigned long *)(GPIO_PORTJ_BLOCK + 0x04)           // Port J Output Type Register
    #define GPIOJ_OSPEEDR               *(unsigned long *)(GPIO_PORTJ_BLOCK + 0x08)           // Port J Output Speed Register
    #define GPIOJ_PUPDR                 *(unsigned long *)(GPIO_PORTJ_BLOCK + 0x0c)           // Port J Pullup/Pulldown Register
    #define GPIOJ_IDR                   *(volatile unsigned long *)(GPIO_PORTJ_BLOCK + 0x10)  // Port J Input Data Register (read-only)
    #define GPIOJ_ODR                   *(volatile unsigned long *)(GPIO_PORTJ_BLOCK + 0x14)  // Port J Output Data Register
    #define GPIOJ_BSRR                  *(volatile unsigned long *)(GPIO_PORTJ_BLOCK + 0x18)  // Port J Bit Set/Reset Register (write-only)
    #define GPIOJ_LCKR                  *(unsigned long *)(GPIO_PORTJ_BLOCK + 0x1c)           // Port J Configuration Lock Register
    #define GPIOJ_AFRL                  *(unsigned long *)(GPIO_PORTJ_BLOCK + 0x20)           // Port J Alternate Function Low Register
    #define GPIOJ_AFRH                  *(unsigned long *)(GPIO_PORTJ_BLOCK + 0x24)           // Port J Alternate Function High Register

    #define GPIOK_MODER                 *(unsigned long *)(GPIO_PORTK_BLOCK + 0x00)           // Port K Mode Register
    #define GPIOK_OTYPER                *(unsigned long *)(GPIO_PORTK_BLOCK + 0x04)           // Port K Output Type Register
    #define GPIOK_OSPEEDR               *(unsigned long *)(GPIO_PORTK_BLOCK + 0x08)           // Port K Output Speed Register
    #define GPIOK_PUPDR                 *(unsigned long *)(GPIO_PORTK_BLOCK + 0x0c)           // Port K Pullup/Pulldown Register
    #define GPIOK_IDR                   *(volatile unsigned long *)(GPIO_PORTK_BLOCK + 0x10)  // Port K Input Data Register (read-only)
    #define GPIOK_ODR                   *(volatile unsigned long *)(GPIO_PORTK_BLOCK + 0x14)  // Port K Output Data Register
    #define GPIOK_BSRR                  *(volatile unsigned long *)(GPIO_PORTK_BLOCK + 0x18)  // Port K Bit Set/Reset Register (write-only)
    #define GPIOK_LCKR                  *(unsigned long *)(GPIO_PORTK_BLOCK + 0x1c)           // Port K Configuration Lock Register
    #define GPIOK_AFRL                  *(unsigned long *)(GPIO_PORTK_BLOCK + 0x20)           // Port K Alternate Function Low Register
    #define GPIOK_AFRH                  *(unsigned long *)(GPIO_PORTK_BLOCK + 0x24)           // Port K Alternate Function High Register
  #endif
#else
    #define GPIOA_CRL                   *(volatile unsigned long *)(GPIO_PORTA_BLOCK + 0x00)  // Port Control Register Low
    #define GPIOA_CRH                   *(volatile unsigned long *)(GPIO_PORTA_BLOCK + 0x04)  // Port Control Register High
    #define GPIOA_IDR                   *(volatile unsigned long *)(GPIO_PORTA_BLOCK + 0x08)  // Port Input Data Register (read-only)
    #define GPIOA_ODR                   *(volatile unsigned long *)(GPIO_PORTA_BLOCK + 0x0c)  // Port Output Data Register
    #define GPIOA_BSRR                  *(volatile unsigned long *)(GPIO_PORTA_BLOCK + 0x10)  // Port Bit Set/Reset Register (write-only)
    #define GPIOA_BRR                   *(volatile unsigned long *)(GPIO_PORTA_BLOCK + 0x14)  // Port Bit Reset Register (write-only)
    #define GPIOA_LCKR                  *(unsigned long *)(GPIO_PORTA_BLOCK + 0x18)           // Port Configuration Lock Register

    #define GPIOB_CRL                   *(volatile unsigned long *)(GPIO_PORTB_BLOCK + 0x00)  // Port Control Register Low
    #define GPIOB_CRH                   *(volatile unsigned long *)(GPIO_PORTB_BLOCK + 0x04)  // Port Control Register High
    #define GPIOB_IDR                   *(volatile unsigned long *)(GPIO_PORTB_BLOCK + 0x08)  // Port Input Data Register (read-only)
    #define GPIOB_ODR                   *(volatile unsigned long *)(GPIO_PORTB_BLOCK + 0x0c)  // Port Output Data Register
    #define GPIOB_BSRR                  *(volatile unsigned long *)(GPIO_PORTB_BLOCK + 0x10)  // Port Bit Set/Reset Register (write-only)
    #define GPIOB_BRR                   *(volatile unsigned long *)(GPIO_PORTB_BLOCK + 0x14)  // Port Bit Reset Register (write-only)
    #define GPIOB_LCKR                  *(unsigned long *)(GPIO_PORTB_BLOCK + 0x18)           // Port Configuration Lock Register

    #define GPIOC_CRL                   *(volatile unsigned long *)(GPIO_PORTC_BLOCK + 0x00)  // Port Control Register Low
    #define GPIOC_CRH                   *(volatile unsigned long *)(GPIO_PORTC_BLOCK + 0x04)  // Port Control Register High
    #define GPIOC_IDR                   *(volatile unsigned long *)(GPIO_PORTC_BLOCK + 0x08)  // Port Input Data Register (read-only)
    #define GPIOC_ODR                   *(volatile unsigned long *)(GPIO_PORTC_BLOCK + 0x0c)  // Port Output Data Register
    #define GPIOC_BSRR                  *(volatile unsigned long *)(GPIO_PORTC_BLOCK + 0x10)  // Port Bit Set/Reset Register (write-only)
    #define GPIOC_BRR                   *(volatile unsigned long *)(GPIO_PORTC_BLOCK + 0x14)  // Port Bit Reset Register (write-only)
    #define GPIOC_LCKR                  *(unsigned long *)(GPIO_PORTC_BLOCK + 0x18)           // Port Configuration Lock Register

    #define GPIOD_CRL                   *(volatile unsigned long *)(GPIO_PORTD_BLOCK + 0x00)  // Port Control Register Low
    #define GPIOD_CRH                   *(volatile unsigned long *)(GPIO_PORTD_BLOCK + 0x04)  // Port Control Register High
    #define GPIOD_IDR                   *(volatile unsigned long *)(GPIO_PORTD_BLOCK + 0x08)  // Port Input Data Register (read-only)
    #define GPIOD_ODR                   *(volatile unsigned long *)(GPIO_PORTD_BLOCK + 0x0c)  // Port Output Data Register
    #define GPIOD_BSRR                  *(volatile unsigned long *)(GPIO_PORTD_BLOCK + 0x10)  // Port Bit Set/Reset Register (write-only)
    #define GPIOD_BRR                   *(volatile unsigned long *)(GPIO_PORTD_BLOCK + 0x14)  // Port Bit Reset Register (write-only)
    #define GPIOD_LCKR                  *(unsigned long *)(GPIO_PORTD_BLOCK + 0x18)           // Port Configuration Lock Register

    #define GPIOE_CRL                   *(volatile unsigned long *)(GPIO_PORTE_BLOCK + 0x00)  // Port Control Register Low
    #define GPIOE_CRH                   *(volatile unsigned long *)(GPIO_PORTE_BLOCK + 0x04)  // Port Control Register High
    #define GPIOE_IDR                   *(volatile unsigned long *)(GPIO_PORTE_BLOCK + 0x08)  // Port Input Data Register (read-only)
    #define GPIOE_ODR                   *(volatile unsigned long *)(GPIO_PORTE_BLOCK + 0x0c)  // Port Output Data Register
    #define GPIOE_BSRR                  *(volatile unsigned long *)(GPIO_PORTE_BLOCK + 0x10)  // Port Bit Set/Reset Register (write-only)
    #define GPIOE_BRR                   *(volatile unsigned long *)(GPIO_PORTE_BLOCK + 0x14)  // Port Bit Reset Register (write-only)
    #define GPIOE_LCKR                  *(unsigned long *)(GPIO_PORTE_BLOCK + 0x18)           // Port Configuration Lock Register

    #define GPIOF_CRL                   *(volatile unsigned long *)(GPIO_PORTF_BLOCK + 0x00)  // Port Control Register Low
    #define GPIOF_CRH                   *(volatile unsigned long *)(GPIO_PORTF_BLOCK + 0x04)  // Port Control Register High
    #define GPIOF_IDR                   *(volatile unsigned long *)(GPIO_PORTF_BLOCK + 0x08)  // Port Input Data Register (read-only)
    #define GPIOF_ODR                   *(volatile unsigned long *)(GPIO_PORTF_BLOCK + 0x0c)  // Port Output Data Register
    #define GPIOF_BSRR                  *(volatile unsigned long *)(GPIO_PORTF_BLOCK + 0x10)  // Port Bit Set/Reset Register (write-only)
    #define GPIOF_BRR                   *(volatile unsigned long *)(GPIO_PORTF_BLOCK + 0x14)  // Port Bit Reset Register (write-only)
    #define GPIOF_LCKR                  *(unsigned long *)(GPIO_PORTF_BLOCK + 0x18)           // Port Configuration Lock Register

    #define GPIOG_CRL                   *(volatile unsigned long *)(GPIO_PORTG_BLOCK + 0x00)  // Port Control Register Low
    #define GPIOG_CRH                   *(volatile unsigned long *)(GPIO_PORTG_BLOCK + 0x04)  // Port Control Register High
    #define GPIOG_IDR                   *(volatile unsigned long *)(GPIO_PORTG_BLOCK + 0x08)  // Port Input Data Register (read-only)
    #define GPIOG_ODR                   *(volatile unsigned long *)(GPIO_PORTG_BLOCK + 0x0c)  // Port Output Data Register
    #define GPIOG_BSRR                  *(volatile unsigned long *)(GPIO_PORTG_BLOCK + 0x10)  // Port Bit Set/Reset Register (write-only)
    #define GPIOG_BRR                   *(volatile unsigned long *)(GPIO_PORTG_BLOCK + 0x14)  // Port Bit Reset Register (write-only)
    #define GPIOG_LCKR                  *(unsigned long *)(GPIO_PORTG_BLOCK + 0x18)           // Port Configuration Lock Register

    // Alternate-function I/O 
    //
    #define AFIO_EVCR                   *(unsigned long *)(AFIO_BLOCK + 0x00)                 // Event Control Register
    #define AFIO_MAPR                   *(volatile unsigned long *)(AFIO_BLOCK + 0x04)        // AF Remap and Debug I/O Register
      #define SPI1_REMAP                0x00000001
      #define I2C1_REMAP                0x00000002
      #define USART1_REMAPPED           0x00000004
      #define USART2_REMAPPED           0x00000008
      #define USART3_PARTIALLY_REMAPPED 0x00000010
      #define USART3_FULLY_REMAPPED     0x00000030
      #define TIM1_0_REMAP              0x00000040
      #define TIM1_1_REMAP              0x00000080
      #define TIM2_PARTIAL_REMAP_1      0x00000100
      #define TIM2_PARTIAL_REMAP_2      0x00000200
      #define TIM2_FULL_REMAP           0x00000300
      #define TIM3_PARTIAL_REMAP        0x00000800
      #define TIM3_FULL_REMAP           0x00000c00
      #define TIM4_REMAP                0x00001000
      #define CAN1_0_REMAP              0x00002000
      #define CAN1_1_REMAP              0x00004000
      #define PD01_REMAP                0x00008000
      #define TIM5CH4_IREMAP            0x00010000
    #ifdef _CONNECTIVITY_LINE
      #define ETH_REMAP                 0x00200000
      #define CAN2_REMAP                0x00400000
      #define MII_RMII_SEL              0x00800000
    #else
      #define ADC1_ETRGINJ_REMAP        0x00020000
      #define ADC1_ETRGREG_REMAP        0x00040000
      #define ADC2_ETRGINJ_REMAP        0x00080000
      #define ADC2_ETRGREG_REMAP        0x00100000
    #endif
      #define SWJ_CFG0_SEL              0x01000000
      #define SWJ_CFG1_SEL              0x02000000
      #define SWJ_CFG2_SEL              0x04000000
    #ifdef _CONNECTIVITY_LINE
      #define SPI3_REMAP                0x10000000
      #define TIM2ITR1_IREMAP           0x20000000
      #define PTP_PPS_REMAP             0x40000000
    #endif
    #define AFIO_EXTICR1_ADD            (unsigned long *)(AFIO_BLOCK + 0x08)
    #define AFIO_EXTICR1                *(unsigned long *)(AFIO_BLOCK + 0x08) // External Interrupt Configuration Register 1
    #define AFIO_EXTICR2                *(unsigned long *)(AFIO_BLOCK + 0x0c) // External Interrupt Configuration Register 2
    #define AFIO_EXTICR3                *(unsigned long *)(AFIO_BLOCK + 0x10) // External Interrupt Configuration Register 3
    #define AFIO_EXTICR4                *(unsigned long *)(AFIO_BLOCK + 0x14) // External Interrupt Configuration Register 4

    #define AFIO_MAPR2                  *(unsigned long *)(AFIO_BLOCK + 0x1c) // AF Remap and Debug I/O Register 2
#endif


#if defined USB_DEVICE_AVAILABLE
    #define USB_CAN_SRAM_ADDR            (volatile unsigned long *)(USB_CAN_MEM_BLOCK) // start of USB/CAN SRAM

    // USB device (F1)
    //
    #define USB_EP0R_ADD                 (volatile unsigned long *)(USB_DEV_FS_BLOCK + 0x00) // USB endpoint 0 register address
    #define USB_EP0R                     *(volatile unsigned long *)(USB_DEV_FS_BLOCK + 0x00) // USB endpoint 0 register
        #define USB_EPR_CTR_CTR_RX       0x8000                          // correct transfer for reception (write '0' to clear)
        #define USB_EPR_CTR_DTOG_RX      0x4000                          // data toggle, for reception transfers (write '1' to toggle)
        #define USB_EPR_CTR_STAT_RX_MASK 0x3000                          // status bits, for reception transfers (write '1' to toggle)
        #define USB_EPR_CTR_STAT_RX_DISABLED 0x0000                      // reception status - disabled
        #define USB_EPR_CTR_STAT_RX_STALL    0x1000                      // reception status - stalled
        #define USB_EPR_CTR_STAT_RX_NAK      0x2000                      // reception status - nacked
        #define USB_EPR_CTR_STAT_RX_VALID    0x3000                      // reception status - valid (enabled for reception)
        #define USB_EPR_CTR_SETUP        0x0800                          // setup transaction completed (read-only)
        #define USB_EPR_CTR_EP_TYPE_MASK 0x0600                          // endpoint type
        #define USB_EPR_CTR_EP_TYPE_BULK 0x0000                          // endpoint type - bulk
        #define USB_EPR_CTR_EP_TYPE_CONTROL 0x0200                       // endpoint type - control
        #define USB_EPR_CTR_EP_TYPE_ISO  0x0400                          // endpoint type - ISO
        #define USB_EPR_CTR_EP_TYPE_INTERRUPT 0x0600                     // endpoint type - interrupt
        #define USB_EPR_CTR_EP_TYPE_KIND_BULK 0x0000                     // endpoint type - bulk
        #define USB_EPR_CTR_EP_TYPE_KIND_CONTROL 0x0200                  // endpoint type - control
        #define USB_EPR_CTR_EP_KIND      0x0100                          // endpoint kind
        #define USB_EPR_CTR_CTR_TX       0x0080                          // correct transfer for transmission (write '0' to clear)
        #define USB_EPR_CTR_DTOG_TX      0x0040                          // data toggle, for transmission transfers (write '1' to toggle)
        #define USB_EPR_CTR_STAT_TX_MASK 0x0030                          // status bits, for transmission transfers (write '1' to toggle)
        #define USB_EPR_CTR_STAT_TX_DISABLED 0x0000                      // transmission status - disabled
        #define USB_EPR_CTR_STAT_TX_STALL    0x0010                      // transmission status - stalled
        #define USB_EPR_CTR_STAT_TX_NAK      0x0020                      // transmission status - nacked
        #define USB_EPR_CTR_STAT_TX_VALID    0x0030                      // transmission status - enabled for transmission
        #define USB_EPR_CTR_ES_MASK      0x000f                          // endpoint address
    #define USB_EP1R                     *(volatile unsigned long *)(USB_DEV_FS_BLOCK + 0x04) // USB Endpoint 1 Register
    #define USB_EP2R                     *(volatile unsigned long *)(USB_DEV_FS_BLOCK + 0x08) // USB Endpoint 2 Register
    #define USB_EP3R                     *(volatile unsigned long *)(USB_DEV_FS_BLOCK + 0x0c) // USB Endpoint 3 Register
    #define USB_EP4R                     *(volatile unsigned long *)(USB_DEV_FS_BLOCK + 0x10) // USB Endpoint 4 Register
    #define USB_EP5R                     *(volatile unsigned long *)(USB_DEV_FS_BLOCK + 0x14) // USB Endpoint 5 Register
    #define USB_EP6R                     *(volatile unsigned long *)(USB_DEV_FS_BLOCK + 0x18) // USB Endpoint 6 Register
    #define USB_EP7R                     *(volatile unsigned long *)(USB_DEV_FS_BLOCK + 0x1c) // USB Endpoint 7 Register
    #define USB_CNTR                     *(unsigned long *)(USB_DEV_FS_BLOCK + 0x40) // USB Control Register
        #define USB_CNTR_CTRM            0x8000                          // CTR interrupt enabled
        #define USB_CNTR_PMAOVRM         0x4000                          // PMAOVR interrupt enabled
        #define USB_CNTR_ERRM            0x2000                          // ERR interrupt enabled
        #define USB_CNTR_WKUPM           0x1000                          // WKUP interrupt enabled
        #define USB_CNTR_SUSPM           0x0800                          // SUSPM interrupt enabled
        #define USB_CNTR_RESETM          0x0400                          // RESETM interrupt enabled
        #define USB_CNTR_SOFM            0x0200                          // SOFM interrupt enabled
        #define USB_CNTR_ESOFM           0x0100                          // ESOFM interrupt enabled
        #define USB_CNTR_RESUME          0x0010                          // resume request
        #define USB_CNTR_FSUSP           0x0008                          // force suspend
        #define USB_CNTR_LP_MODE         0x0004                          // low power mode
        #define USB_CNTR_PDWN            0x0002                          // power down
        #define USB_CNTR_FRES            0x0001                          // force USB reset
    #define USB_ISTR                     *(volatile unsigned long *)(USB_DEV_FS_BLOCK + 0x44) // USB Interrupt Status Register
        #define USB_ISTR_CTR             0x8000                          // correct transfer (read-only)
        #define USB_ISTR_PMAOVR          0x4000                          // packet memory area over/underrun (write '0' to clear)
        #define USB_ISTR_ERR             0x2000                          // error (write '0' to clear)
        #define USB_ISTR_WKUP            0x1000                          // wakeup (write '0' to clear)
        #define USB_ISTR_SUSP            0x0800                          // suspend mode request (write '0' to clear)
        #define USB_ISTR_RESET           0x0400                          // USB reset request (write '0' to clear)
        #define USB_ISTR_SOF             0x0200                          // start of frame (write '0' to clear)
        #define USB_ISTR_ESOF            0x0100                          // expected start of frame (write '0' to clear)
        #define USB_ISTR_DIR             0x0010                          // direction of transaction (read-only) - '1' is OUT and '0' is IN
        #define USB_ISTR_EP_ID_MASK      0x000f                          // endpoint identifier
    #define USB_FNR                      *(volatile unsigned long *)(USB_DEV_FS_BLOCK + 0x48) // USB Frame Number Register
        #define USB_FNR_RXDP             0x8000                          // receive data + line status
        #define USB_FNR_RXDM             0x4000                          // receive data - line status
        #define USB_FNR_LCK              0x2000                          // locked
        #define USB_FNR_LSOF_MASK        0x1800                          // lost SOF
        #define USB_FNR_FN_MASK          0x07ff                          // frame number
    #define USB_DADDR                    *(unsigned long *)(USB_DEV_FS_BLOCK + 0x4c) // USB Device Address Register
        #define USB_DADDR_EF             0x0080                          // enable function
        #define USB_DADDR_ADD_MASK       0x007f                          // device address
    #define USB_BTABLE                   *(unsigned long *)(USB_DEV_FS_BLOCK + 0x50) // USB Buffer Table Address
        #define USB_BTABLE_MASK          0xfff8                          // buffer table

    #define USB_FIFO_BUFFER_DEPTH        1

    #define NUMBER_OF_USB_ENDPOINTS      8                               // device supports endpoint 0 plus seven other endpoints

    typedef struct stUSB_END_POINT
    {
        unsigned long ulNextRxData0;
        unsigned long ulNextTxData0;
        unsigned long ulEndpointSize;                                    // contains size of endpoint plus some control flags
    } USB_END_POINT;

    typedef struct _PACK stUSB_BD_TABLE
    {
        unsigned short usUSB_ADDR_TX;                                    // transmission buffer address in USB SRAM
        unsigned short usRes0;
        unsigned short usUSB_COUNT_TX_0;                                 // transmission byte count 0
        unsigned short usRes1;
      //unsigned short usUSB_COUNT_TX_0;                                 // transmission byte count 1
      //unsigned short usRes2;
        unsigned short usUSB_ADDR_RX;                                    // reception buffer address in USB SRAM
        unsigned short usRes3;
        unsigned short usUSB_COUNT_RX_0;                                 // reception byte count 0
        unsigned short usRes4;
      //unsigned short usUSB_COUNT_RX_0;                                 // reception byte count 1
      //unsigned short usRes5;
    } USB_BD_TABLE;

    #define USB_COUNT_RX_BL_SIZE        0x8000                           // allocated buffer size ranges from 32 to 1024 bytes (else from 2 to 62 bytes)
    #define USB_COUNT_RX_NUM_BLOCK_MASK 0x7c00                           // number of memory blocks allocated to this packet buffer
    #define USB_COUNT_COUNT_MASK        0x02ff                           // transmission/reception byte count mask
    #define USB_COUNT_SHIFT             10

    typedef struct stUSB_HW
    {
        unsigned long  ulUSB_buffer[NUMBER_OF_USB_ENDPOINTS][64/sizeof(unsigned long)]; // linear, word aligned buffer large enough to hold largest single reception (there is one for each endpoint in case it is necessary to hold the input)
        USB_END_POINT *ptrEndpoint;
        unsigned char **ptrRxDatBuffer;                                  // pointer to the next reception buffer pointer
        unsigned char **ptrTxDatBuffer;                                  // pointer to the next transmission buffer pointer
        unsigned short usLength;                                         // length of present input being handled
        unsigned short usStoredLength[NUMBER_OF_USB_ENDPOINTS];          // length information being held in the input buffer
        unsigned long  ucUSBAddress;                                     // our USB address                       
    } USB_HW;

    extern void fnSendUSB_data(unsigned char *pData, unsigned short Len, int iEndpoint, USB_HW *ptrUSB_HW);
    extern void fnSendZeroData(USB_HW *ptrUSB_HW, int iEndpoint);
    extern void fnPutToFIFO(int iLength, volatile unsigned long *ptrRxFIFO, unsigned char *ptrBuffer);
    extern void fnPrepareTx(int Len, unsigned char *pData);


    #define FNSEND_USB_DATA(pData, Len, iEndpoint, ptrUSB_HW) fnSendUSB_data(pData, Len, iEndpoint, ptrUSB_HW)
    #define FNSEND_ZERO_DATA(ptrUSB_HW, iEndpoint)            fnSendZeroData(ptrUSB_HW, iEndpoint)
    #define SET_CONTROL_DIRECTION(a,b)
    #define CLEAR_SETUP(a)                                               // for compatibility with generic driver
    #define VALIDATE_NEW_CONFIGURATION()
#endif

#if defined USB_OTG_AVAILABLE
    // USB Device (F4)
    //
    // Core Global Control and Status Registers
    //
    #define OTG_FS_GOTGCTL                   *(volatile unsigned long *)(USB_OTG_FS_BLOCK + 0x000) // OTG_FS control and status register
      #define OTG_FS_GOTGCTL_SRQSCS          0x00000001                  // session request success (only device mode)
      #define OTG_FS_GOTGCTL_SRQ             0x00000002                  // session request
      #define OTG_FS_GOTGCTL_HNGSCS          0x00000100                  // host negotiation success
      #define OTG_FS_GOTGCTL_HNPRG           0x00000200                  // HNP request
      #define OTG_FS_GOTGCTL_HSHNPEN         0x00000400                  // host set HNP is enabled
      #define OTG_FS_GOTGCTL_DHNPEN          0x00000800                  // device HNP is enabled
      #define OTG_FS_GOTGCTL_CIDSTS_A        0x00000000                  // connector ID - A-device mode
      #define OTG_FS_GOTGCTL_CIDSTS_B        0x00010000                  // connector ID - B-device mode
      #define OTG_FS_GOTGCTL_DBCT_LONG       0x00000000                  // long debounce time (100ms + 2.5us)
      #define OTG_FS_GOTGCTL_DBCT_SHORT      0x00020000                  // short debounce time (2.5us)
      #define OTG_FS_GOTGCTL_ASVLD           0x00040000                  // A-session is valid
      #define OTG_FS_GOTGCTL_BSVLD           0x00080000                  // B-session is valid
    #define OTG_FS_GOTGINT                   *(volatile unsigned long *)(USB_OTG_FS_BLOCK + 0x004) // OTG_FS interrupt register
    #define OTG_FS_GAHBCFG                   *(volatile unsigned long *)(USB_OTG_FS_BLOCK + 0x008) // OTG_FS AHB configuration register
      #define OTG_FS_GAHBCFG_GINTMSK         0x00000001                  // global interrupt enable
      #define OTG_FS_GAHBCFG_TXFELVL_HALF_E  0x00000000                  // IN endpoint TxFIFO empty level - half empty
      #define OTG_FS_GAHBCFG_TXFELVL_EMPTY   0x00000080                  // IN endpoint TxFIFO empty level - completely empty
      #define OTG_FS_GAHBCFG_PTXFELVL_HALF_E 0x00000000                  // periodic TxFIFO empty level - half empty
      #define OTG_FS_GAHBCFG_PTXFELVL_EMPTY  0x00000100                  // periodic TxFIFO empty level - completely empty
    #define OTG_FS_GUSBCFG                   *(volatile unsigned long *)(USB_OTG_FS_BLOCK + 0x00c) // OTG_FS USB configuration register (do not change after initial configuration)
      #define OTG_FS_GUSBCFG_TOCAL           0x00000007                  // timeout calibration
      #define OTG_FS_GUSBCFG_PHYSEL          0x00000080                  // write only access - always '1'
      #define OTG_FS_GUSBCFG_SRPCAP          0x00000100                  // SRP capable
      #define OTG_FS_GUSBCFG_HNPCAP          0x00000200                  // HNP capable
      #define OTG_FS_GUSBCFG_TRDT_MASK       0x00003c00                  // USB turnaround time in PHY clocks (only device mode)
      #define OTG_FS_GUSBCFG_TRDT_SHIFT      10
      #define OTG_FS_GUSBCFG_TRDT_MAX        0x0f
      #define OTG_FS_GUSBCFG_FHMOD           0x20000000                  // force host mode (irrespective of OTG_FS_ID pin state) - takes 25ms to take effect
      #define OTG_FS_GUSBCFG_FDMOD           0x40000000                  // force device mode (irrespective of OTG_FS_ID pin state) - takes 25ms to take effect
      #define OTG_FS_GUSBCFG_CTXPKT          0x80000000                  // corrupt tx packet (for debugging only)
    #define OTG_FS_GRSTCTL                   *(volatile unsigned long *)(USB_OTG_FS_BLOCK + 0x010) // OTG_FS reset register
      #define OTG_FS_GRSTCTL_CSRST           0x00000001                  // core soft reset
      #define OTG_FS_GRSTCTL_HSRST           0x00000002                  // HCLK soft reset
      #define OTG_FS_GRSTCTL_FCRST           0x00000004                  // host frame counter reset (host mode only)
      #define OTG_FS_GRSTCTL_RXFFLSH         0x00000010                  // flush entire rx FIFO                     
      #define OTG_FS_GRSTCTL_TXFFLSH         0x00000020                  // selective flush of single or all tx FIFOs
      #define OTG_FS_GRSTCTL_TXFNUM_FIFO0    0x00000000                  // define flush of Tx FIFO 0
      #define OTG_FS_GRSTCTL_TXFNUM_FIFO1    0x00000040                  // define flush of Tx FIFO 1
      #define OTG_FS_GRSTCTL_TXFNUM_FIFO2    0x00000080                  // define flush of Tx FIFO 2
      #define OTG_FS_GRSTCTL_TXFNUM_ALL      0x00000400                  // define flush all Tx FIFOs
      #define OTG_FS_GRSTCTL_AHBIDL          0x80000000                  // AHB master state machine is in idle condition
    #define OTG_FS_GINTSTS                   *(volatile unsigned long *)(USB_OTG_FS_BLOCK + 0x014) // OTG_FS core interrupt register
      #define OTG_FS_GINTSTS_CMOD_DEVICE     0x00000000                  // current mode of operation is device
      #define OTG_FS_GINTSTS_CMOD_HOST       0x00000001                  // current mode of operation is host
      #define OTG_FS_GINTSTS_MMIS            0x00000002                  // mode mismatch interrupt
      #define OTG_FS_GINTSTS_OTGINT          0x00000004                  // OTG interrupt
      #define OTG_FS_GINTSTS_SOF             0x00000008                  // start of frame interrupt
      #define OTG_FS_GINTSTS_RXFLVL          0x00000010                  // receive FIFO non-empty
      #define OTG_FS_GINTSTS_NPTXFE          0x00000020                  // non-periodic TxFIFO empty interrupt
      #define OTG_FS_GINTSTS_GINAKEFF        0x00000040                  // global non-periodic IN NAK effective interrupt
      #define OTG_FS_GINTSTS_GONAKEFF        0x00000080                  // global OUT NAK effective interrupt
      #define OTG_FS_GINTSTS_ESUSP           0x00000400                  // early suspend interrupt
      #define OTG_FS_GINTSTS_USBSUSP         0x00000800                  // USB suspend interrupt
      #define OTG_FS_GINTSTS_USBRST          0x00001000                  // USB reset interrupt
      #define OTG_FS_GINTSTS_ENUMDNE         0x00002000                  // enumeration done interrupt
      #define OTG_FS_GINTSTS_ISOODRP         0x00004000                  // isochronous OUT packet dropped interrupt
      #define OTG_FS_GINTSTS_EOPF            0x00008000                  // end of periodic frame interrupt
      #define OTG_FS_GINTSTS_IEPINT          0x00040000                  // IN endpoints interrupt
      #define OTG_FS_GINTSTS_OEPINT          0x00080000                  // OUT endpoints interrupt
      #define OTG_FS_GINTSTS_IISOIXFR        0x00100000                  // incomplete isochronous IN transfer interrupt
      #define OTG_FS_GINTSTS_IPXFR           0x00200000                  // incomplete periodic transfer interrupt
      #define OTG_FS_GINTSTS_HPRTINT         0x01000000                  // host port interrupt
      #define OTG_FS_GINTSTS_HCINT           0x02000000                  // host channels interrupt
      #define OTG_FS_GINTSTS_PTXFE           0x04000000                  // periodic TxFIFO empty interrupt
      #define OTG_FS_GINTSTS_CIDSCHG         0x10000000                  // connector ID status change interrupt
      #define OTG_FS_GINTSTS_DISCINT         0x20000000                  // disconnected detected interrupt
      #define OTG_FS_GINTSTS_SRQINT          0x40000000                  // session request/new session detected interrupt
      #define OTG_FS_GINTSTS_WKUINT          0x80000000                  // resume/remote wakeup detected interrupt
    #define OTG_FS_GINTMSK                   *(unsigned long *)(USB_OTG_FS_BLOCK + 0x018) // OTG_FS interrupt mask register
      #define OTG_FS_GINTMSK_MMISM           0x00000002                  // mode mismatch interrupt enabled
      #define OTG_FS_GINTMSK_OTGINT          0x00000004                  // OTG interrupt enabled
      #define OTG_FS_GINTMSK_SOFM            0x00000008                  // start of frame interrupt enabled
      #define OTG_FS_GINTMSK_RXFLVLM         0x00000010                  // receive FIFO non-empty interrupt enabled
      #define OTG_FS_GINTMSK_NPTXFEM         0x00000020                  // non-periodic TxFIFO empty interrupt enabled
      #define OTG_FS_GINTMSK_GINAKEFFM       0x00000040                  // global non-periodic IN NAK effective interrupt enabled
      #define OTG_FS_GINTMSK_GONAKEFFM       0x00000080                  // global OUT NAK effective interrupt enabled
      #define OTG_FS_GINTMSK_ESUSPM          0x00000400                  // early suspend interrupt enabled
      #define OTG_FS_GINTMSK_USBSUSPM        0x00000800                  // USB suspend interrupt enabled
      #define OTG_FS_GINTMSK_USBRST          0x00001000                  // USB reset interrupt enabled
      #define OTG_FS_GINTMSK_ENUMDNEM        0x00002000                  // enumeration done interrupt enabled
      #define OTG_FS_GINTMSK_ISOODRPM        0x00004000                  // isochronous OUT packet dropped interrupt enabled
      #define OTG_FS_GINTMSK_EOPFM           0x00008000                  // end of periodic frame interrupt enabled
      #define OTG_FS_GINTMSK_EPMISM          0x00020000                  // endpoint mismatch interrupt enabled
      #define OTG_FS_GINTMSK_IEPINT          0x00040000                  // IN endpoints interrupt enabled
      #define OTG_FS_GINTMSK_OEPINT          0x00080000                  // OUT endpoints interrupt enabled
      #define OTG_FS_GINTMSK_IISOIXFRM       0x00100000                  // incomplete isochronous IN transfer interrupt enabled
      #define OTG_FS_GINTMSK_IISOOXFRM       0x00200000                  // incomplete isochronous OUT transfer interrupt enabled
      #define OTG_FS_GINTMSK_PRTIM           0x01000000                  // host port interrupt enabled
      #define OTG_FS_GINTMSK_HCIM            0x02000000                  // host channels interrupt enabled
      #define OTG_FS_GINTMSK_PTXFEM          0x04000000                  // periodic TxFIFO empty interrupt enabled
      #define OTG_FS_GINTMSK_CIDSCHGM        0x10000000                  // connector ID status change interrupt enabled
      #define OTG_FS_GINTMSK_DISCINT         0x20000000                  // disconnected detected interrupt enabled
      #define OTG_FS_GINTMSK_SRQIM           0x40000000                  // session request/new session detected interrupt enabled
      #define OTG_FS_GINTMSK_WUIM            0x80000000                  // resume/remote wakeup detected interrupt enabled
    #define OTG_FS_GRXSTSR                   *(volatile unsigned long *)(USB_OTG_FS_BLOCK + 0x01c) // OTG_FS receive status debug read register (read-only)
      #define OTG_FS_GRXSTSR_CHNUM           0x0000000f                  // channel number to which current packet belongs (host mode)
      #define OTG_FS_GRXSTSR_EPNUM           0x0000000f                  // endpoint to which current packet belongs (device mode)
      #define OTG_FS_GRXSTSR_BCNT_MASK       0x00007ff0                  // byte count of received IN data packet
      #define OTG_FS_GRXSTSR_BCNT_SHIFT      4
      #define OTG_FS_GRXSTSR_DPID_DATA0      0x00000000                  // data PID - DATA0
      #define OTG_FS_GRXSTSR_DPID_DATA1      0x00010000                  // data PID - DATA1
      #define OTG_FS_GRXSTSR_DPID_DATA2      0x00008000                  // data PID - DATA2
      #define OTG_FS_GRXSTSR_DPID_MDATA      0x00018000                  // data PID - MDATA
      #define OTG_FS_GRXSTSR_PKTSTS_MASK     0x001e0000
      #define OTG_FS_GRXSTSR_PKTSTS_IN_RX    0x00040000                  // packet status - IN data packet received (host)
      #define OTG_FS_GRXSTSR_PKTSTS_IN_OUT   0x00060000                  // packet status - IN transfer completed (host)
      #define OTG_FS_GRXSTSR_PKTSTS_DT_ERR   0x000a0000                  // packet status - data toggle error (host)
      #define OTG_FS_GRXSTSR_PKTSTS_HALT     0x000e0000                  // packet status - channel halted (host)
      #define OTG_FS_GRXSTSR_PKTSTS_OUT_NAK  0x00020000                  // packet status - global OUT NAK (device)
      #define OTG_FS_GRXSTSR_PKTSTS_OUT_RX   0x00040000                  // packet status - OUT data packet received (device)
      #define OTG_FS_GRXSTSR_PKTSTS_OUT_OK   0x00060000                  // packet status - OUT transfer completed (device)
      #define OTG_FS_GRXSTSR_PKTSTS_SETUP_OK 0x00080000                  // packet status - SETUP transaction completed (device)
      #define OTG_FS_GRXSTSR_PKTSTS_SETUP_RX 0x000c0000                  // packet status - SETUP data packet received (device)
      #define OTG_FS_GRXSTSR_FRMNUM          0x01e00000                  // frame number (device - only isochronous OUTs)
    #define OTG_FS_GRXSTSP                   *(volatile unsigned long *)(USB_OTG_FS_BLOCK + 0x020) // OTG_FS status read and pop register
    #define OTG_FS_GRXFSIZ                   *(volatile unsigned long *)(USB_OTG_FS_BLOCK + 0x024) // OTG_FS receive FIFO size register (16..256) in 32 bit words
    #define OTG_FS_DIEPTXF0                  *(volatile unsigned long *)(USB_OTG_FS_BLOCK + 0x028) // OTG_FS endpoint 0 transmit FIFO size
      #define OTG_FS_DIEPTXF_TX0FD_MASK     0xffff0000                   // endpoint TxFIFO depth (16..256)
      #define OTG_FS_DIEPTXF_TX0FSA_MASK    0x0000ffff                   // memory start address for the endpoint transmit FIFO RAM
    #define OTG_FS_HNPTXSTS                  *(volatile unsigned long *)(USB_OTG_FS_BLOCK + 0x02c) // OTG_FS non-periodic transmit FIFO/queue status register

    #define OTG_FS_GCCFG                     *(volatile unsigned long *)(USB_OTG_FS_BLOCK + 0x038) // OTG_FS general core configuration register
      #define OTG_FS_GCCFG_PWRDWN            0x00010000                  // power down deactivated (transceiver active)
      #define OTG_FS_GCCFG_VBUSASEN          0x00040000                  // enable Vbus sening "A" device
      #define OTG_FS_GCCFG_VBUSBSEN          0x00080000                  // enable Vbus sening "B" device
      #define OTG_FS_GCCFG_SOFOUTEN          0x00100000                  // SOF output enable
      #define OTG_FS_GCCFG_NOVBUSSENS        0x00200000                  // Vbus sensing disable option
    #define OTG_FS_CID                       *(volatile unsigned long *)(USB_OTG_FS_BLOCK + 0x03c) // OTG_FS core ID register

    #define OTG_FS_HPTXFSIZ                  *(volatile unsigned long *)(USB_OTG_FS_BLOCK + 0x100) // OTG_FS host periodic transmit FIFO size register
    #define OTG_FS_DIEPTXF1                  *(volatile unsigned long *)(USB_OTG_FS_BLOCK + 0x104) // OTG_FS device IN1 endpoint transmit FIFO size register
    #define OTG_FS_DIEPTXF2                  *(volatile unsigned long *)(USB_OTG_FS_BLOCK + 0x108) // OTG_FS device IN2 endpoint transmit FIFO size register
    #define OTG_FS_DIEPTXF3                  *(volatile unsigned long *)(USB_OTG_FS_BLOCK + 0x10c) // OTG_FS device IN3 endpoint transmit FIFO size register



    #define OTG_FS_HCFG                      *(volatile unsigned long *)(USB_OTG_FS_BLOCK + 0x400) // 
    #define OTG_FS_HFIR                      *(volatile unsigned long *)(USB_OTG_FS_BLOCK + 0x404) // 
    #define OTG_FS_HFNUM                     *(volatile unsigned long *)(USB_OTG_FS_BLOCK + 0x408) //
 
    #define OTG_FS_HPTXSTS                   *(volatile unsigned long *)(USB_OTG_FS_BLOCK + 0x410) // 
    #define OTG_FS_HAINT                     *(volatile unsigned long *)(USB_OTG_FS_BLOCK + 0x414) // 
    #define OTG_FS_HAINTMSK                  *(volatile unsigned long *)(USB_OTG_FS_BLOCK + 0x418) // 

    #define OTG_FS_HPRT                      *(volatile unsigned long *)(USB_OTG_FS_BLOCK + 0x440) // 

    #define OTG_FS_HCCHAR0                   *(volatile unsigned long *)(USB_OTG_FS_BLOCK + 0x500) // 

    #define OTG_FS_HCINT0                    *(volatile unsigned long *)(USB_OTG_FS_BLOCK + 0x508) // 
    #define OTG_FS_HCINTMSK0                 *(volatile unsigned long *)(USB_OTG_FS_BLOCK + 0x50c) //
    #define OTG_FS_HCTSIZ0                   *(volatile unsigned long *)(USB_OTG_FS_BLOCK + 0x510) //  

    #define OTG_FS_HCCHAR1                   *(volatile unsigned long *)(USB_OTG_FS_BLOCK + 0x520) //

    #define OTG_FS_HCINT1                    *(volatile unsigned long *)(USB_OTG_FS_BLOCK + 0x528) // 
    #define OTG_FS_HCINTMSK1                 *(volatile unsigned long *)(USB_OTG_FS_BLOCK + 0x52c) //  
    #define OTG_FS_HCTSIZ1                   *(volatile unsigned long *)(USB_OTG_FS_BLOCK + 0x530) // 

    #define OTG_FS_HCCHAR2                   *(volatile unsigned long *)(USB_OTG_FS_BLOCK + 0x540) //

    #define OTG_FS_HCINT2                    *(volatile unsigned long *)(USB_OTG_FS_BLOCK + 0x548) // 
    #define OTG_FS_HCINTMSK2                 *(volatile unsigned long *)(USB_OTG_FS_BLOCK + 0x54c) // 
    #define OTG_FS_HCTSIZ2                   *(volatile unsigned long *)(USB_OTG_FS_BLOCK + 0x550) //

    #define OTG_FS_HCCHAR3                   *(volatile unsigned long *)(USB_OTG_FS_BLOCK + 0x560) // 

    #define OTG_FS_HCINT3                    *(volatile unsigned long *)(USB_OTG_FS_BLOCK + 0x568) // 
    #define OTG_FS_HCINTMSK3                 *(volatile unsigned long *)(USB_OTG_FS_BLOCK + 0x56c) // 
    #define OTG_FS_HCTSIZ3                   *(volatile unsigned long *)(USB_OTG_FS_BLOCK + 0x570) //

    #define OTG_FS_HCCHAR4                   *(volatile unsigned long *)(USB_OTG_FS_BLOCK + 0x580) //

    #define OTG_FS_HCINT4                    *(volatile unsigned long *)(USB_OTG_FS_BLOCK + 0x588) // 
    #define OTG_FS_HCINTMSK4                 *(volatile unsigned long *)(USB_OTG_FS_BLOCK + 0x58c) // 
    #define OTG_FS_HCTSIZ4                   *(volatile unsigned long *)(USB_OTG_FS_BLOCK + 0x590) //

    #define OTG_FS_HCCHAR5                   *(volatile unsigned long *)(USB_OTG_FS_BLOCK + 0x5a0) //

    #define OTG_FS_HCINT5                    *(volatile unsigned long *)(USB_OTG_FS_BLOCK + 0x5a8) // 
    #define OTG_FS_HCINTMSK5                 *(volatile unsigned long *)(USB_OTG_FS_BLOCK + 0x5ac) // 
    #define OTG_FS_HCTSIZ5                   *(volatile unsigned long *)(USB_OTG_FS_BLOCK + 0x5b0) //

    #define OTG_FS_HCCHAR6                   *(volatile unsigned long *)(USB_OTG_FS_BLOCK + 0x5c0) //

    #define OTG_FS_HCINT6                    *(volatile unsigned long *)(USB_OTG_FS_BLOCK + 0x5c8) // 
    #define OTG_FS_HCINTMSK6                 *(volatile unsigned long *)(USB_OTG_FS_BLOCK + 0x5cc) // 
    #define OTG_FS_HCTSIZ6                   *(volatile unsigned long *)(USB_OTG_FS_BLOCK + 0x5d0) //

    #define OTG_FS_HCCHAR7                   *(volatile unsigned long *)(USB_OTG_FS_BLOCK + 0x5e0) //

    #define OTG_FS_HCINT7                    *(volatile unsigned long *)(USB_OTG_FS_BLOCK + 0x5e8) // 
    #define OTG_FS_HCINTMSK7                 *(volatile unsigned long *)(USB_OTG_FS_BLOCK + 0x5ec) // 
    #define OTG_FS_HCTSIZ7                   *(volatile unsigned long *)(USB_OTG_FS_BLOCK + 0x5f0) // 

    #define OTG_FS_DCFG                      *(volatile unsigned long *)(USB_OTG_FS_BLOCK + 0x800) // OTG_FS device configuration register (do not change after initial programming)
      #define OTG_FS_DCFG_DSPD_FULL_SPEED    0x00000003                  // device speed
      #define OTG_FS_DCFG_NZLSOHSK_OUT       0x00000000                  // non-zero-length status OUT handshake - send OUT
      #define OTG_FS_DCFG_NZLSOHSK_STALL     0x00000004                  // non-zero-length status OUT handshake - send STALL
      #define OTG_FS_DCFG_DAD_MASK           0x000007f0                  // device address - programmed after every SetAddress control command
      #define OTG_FS_DCFG_PFIVL_80_PER       0x00000000                  // periodic frame interval - interrupt at 80% of the frame interval
      #define OTG_FS_DCFG_PFIVL_85_PER       0x00000800                  // periodic frame interval - interrupt at 85% of the frame interval
      #define OTG_FS_DCFG_PFIVL_90_PER       0x00001000                  // periodic frame interval - interrupt at 90% of the frame interval
      #define OTG_FS_DCFG_PFIVL_95_PER       0x00001800                  // periodic frame interval - interrupt at 95% of the frame interval
    #define OTG_FS_DCTL                      *(volatile unsigned long *)(USB_OTG_FS_BLOCK + 0x804) // device control register
      #define OTG_FS_DCTL_RWUSIG             0x00000001                  // remote wakeup signaling
      #define OTG_FS_DCTL_SDIS               0x00000002                  // soft disconnect 
      #define OTG_FS_DCTL_GINSTS             0x00000004                  // global IN NAK status (read-only)
      #define OTG_FS_DCTL_GONSTS             0x00000008                  // global OUT NAK status (read-only)
      #define OTG_FS_DCTL_TCTL_DISABLED      0x00000000                  // test control disabled
      #define OTG_FS_DCTL_TCTL_TEST_J        0x00000010                  // test_J mode enabled
      #define OTG_FS_DCTL_TCTL_TEST_K        0x00000020                  // test_K mode enabled
      #define OTG_FS_DCTL_TCTL_TEST_SE0_NAK  0x00000030                  // test_SE0_NAK mode enabled
      #define OTG_FS_DCTL_TCTL_TEST_PACKET   0x00000040                  // test_Packet mode enabled
      #define OTG_FS_DCTL_TCTL_TEST_FORCE_EN 0x00000050                  // test_Force_Enable mode enabled
      #define OTG_FS_DCTL_SGINAK             0x00000080                  // set global IN NAK (write-only)
      #define OTG_FS_DCTL_CGINAK             0x00000100                  // clear global IN NAK (write only)
      #define OTG_FS_DCTL_SGONAK             0x00000200                  // set global OUT NAK (write-only)
      #define OTG_FS_DCTL_CGONAK             0x00000400                  // clear global OUT NAK (write-only)
      #define OTG_FS_DCTL_POPRGDNE           0x00000800                  // power-on programming done
    #define OTG_FS_DSTS                      *(volatile unsigned long *)(USB_OTG_FS_BLOCK + 0x808) // device status register (read-only)
      #define OTG_FS_DSTS_SUSPSTS            0x00000001                  // suspend status
      #define OTG_FS_DSTS_ENUMSPD_FULL_SPEED 0x00000006                  // enumerated speed is full speed (PHY clock is running at 48MHz)
      #define OTG_FS_DSTS_EERR               0x00000008                  // erratic error
      #define OTG_FS_DSTS_SUSPSTS_MASK       0x003fff00                  // frame number of the received SOF
    #define OTG_FS_DIEPMSK                   *(volatile unsigned long *)(USB_OTG_FS_BLOCK + 0x810) // OTG_FS device IN endpoint common interrupt mask register
      #define OTG_FS_DIEPMSK_XFRCM           0x00000001                  // transfer completed interrupt enabled
      #define OTG_FS_DIEPMSK_EPDM            0x00000002                  // endpoint disabled interrupt enabled
      #define OTG_FS_DIEPMSK_TOM             0x00000008                  // timeout condition mask (non-isochronous endpoints) interrupt enabled
      #define OTG_FS_DIEPMSK_ITTXFEMSK       0x00000010                  // IN token received when TxFIFO empty interrupt enabled
      #define OTG_FS_DIEPMSK_INEPNMM         0x00000020                  // IN token received with EP mismatch interrupt enabled
      #define OTG_FS_DIEPMSK_INEPNEM         0x00000040                  // IN endpoint NAK interrupt enabled
    #define OTG_FS_DOEPMSK                   *(unsigned long *)(USB_OTG_FS_BLOCK + 0x814) // OTG_FS device OUT endpoint common interrupt mask register
      #define OTG_FS_DOEPMSK_XFRCM           0x00000001                  // transfer completed interrupt enabled
      #define OTG_FS_DOEPMSK_EPDM            0x00000002                  // endpoint disabled interrupt enabled
      #define OTG_FS_DOEPMSK_STUPM           0x00000008                  // SETUP phase done interrupt enabled
      #define OTG_FS_DOEPMSK_OTEPDM          0x00000010                  // OUT token received when endpoint disabled interrupt enabled
    #define OTG_FS_DAINT                     *(volatile unsigned long *)(USB_OTG_FS_BLOCK + 0x818) // OTG_FS device all endpoints interrupt register (read-only - write '1' to clear)
      #define OTG_FS_DAINT_IEPINT0           0x00000001                  // IN endpoint 0 interrupt
      #define OTG_FS_DAINT_IEPINT1           0x00000002                  // IN endpoint 1 interrupt
      #define OTG_FS_DAINT_IEPINT2           0x00000004                  // IN endpoint 2 interrupt
      #define OTG_FS_DAINT_IEPINT3           0x00000008                  // IN endpoint 3 interrupt
      #define OTG_FS_DAINT_OEPINT0           0x00010000                  // OUT endpoint 0 interrupt
      #define OTG_FS_DAINT_OEPINT1           0x00020000                  // OUT endpoint 1 interrupt
      #define OTG_FS_DAINT_OEPINT2           0x00040000                  // OUT endpoint 2 interrupt
      #define OTG_FS_DAINT_OEPINT3           0x00080000                  // OUT endpoint 3 interrupt
    #define OTG_FS_DAINTMSK                  *(unsigned long *)(USB_OTG_FS_BLOCK + 0x81c) // OTG_FS all endpoint interrupt mask register
      #define OTG_FS_DAINTMSK_IEPM0          0x00000001                  // IN endpoint 0 interrupt enabled
      #define OTG_FS_DAINTMSK_IEPM1          0x00000002                  // IN endpoint 1 interrupt enabled
      #define OTG_FS_DAINTMSK_IEPM2          0x00000004                  // IN endpoint 2 interrupt enabled
      #define OTG_FS_DAINTMSK_IEPM3          0x00000008                  // IN endpoint 3 interrupt enabled
      #define OTG_FS_DAINTMSK_OEPM0          0x00010000                  // OUT endpoint 0 interrupt enabled
      #define OTG_FS_DAINTMSK_OEPM1          0x00020000                  // OUT endpoint 1 interrupt enabled
      #define OTG_FS_DAINTMSK_OEPM2          0x00040000                  // OUT endpoint 2 interrupt enabled
      #define OTG_FS_DAINTMSK_OEPM3          0x00080000                  // OUT endpoint 3 interrupt enabled
    #define OTG_FS_DVBUSDIS                  *(volatile unsigned long *)(USB_OTG_FS_BLOCK + 0x828) //
    #define OTG_FS_DVBUSPULSE                *(volatile unsigned long *)(USB_OTG_FS_BLOCK + 0x82c) //

    #define OTG_FS_DIEPEMPMSK                *(volatile unsigned long *)(USB_OTG_FS_BLOCK + 0x834) //OTG_FS device IN endpoint FIFO empty interrupt mask register
      #define OTG_FS_DIEPEMPMSK_INEPTXFEM0   0x00000001                  // enable IN endpoint 0 Tx FIFO empty interrupt
      #define OTG_FS_DIEPEMPMSK_INEPTXFEM1   0x00000002                  // enable IN endpoint 1 Tx FIFO empty interrupt
      #define OTG_FS_DIEPEMPMSK_INEPTXFEM2   0x00000004                  // enable IN endpoint 2 Tx FIFO empty interrupt
      #define OTG_FS_DIEPEMPMSK_INEPTXFEM3   0x00000008                  // enable IN endpoint 3 Tx FIFO empty interrupt

    #define OTG_FS_DIEPCTL0_ADD              (unsigned long *)(USB_OTG_FS_BLOCK + 0x900)
    #define OTG_FS_DIEPCTL0                  *(volatile unsigned long *)(USB_OTG_FS_BLOCK + 0x900) // OTG_FS device control IN endpoint 0 control register
      #define OTG_FS_DIEPCTL_MPSIZ_64_BYTES  0x00000000                  // maximum packet size on endpoint 0 - 64 bytes
      #define OTG_FS_DIEPCTL_MPSIZ_32_BYTES  0x00000001                  // maximum packet size on endpoint 0 - 32 bytes
      #define OTG_FS_DIEPCTL_MPSIZ_16_BYTES  0x00000002                  // maximum packet size on endpoint 0 - 16 bytes
      #define OTG_FS_DIEPCTL_MPSIZ_8_BYTES   0x00000003                  // maximum packet size on endpoint 0 - 8 bytes
      #define OTG_FS_DIEPCTL_USBAEP_ACTIVE   0x00008000                  // endpoint is active
      #define OTG_FS_DIEPCTL_NAKSTS          0x00020000                  // NAK status
      #define OTG_FS_DIEPCTL_EPTYP_CONTROL   0x00000000                  // control endpoint
      #define OTG_FS_DIEPCTL_EPTYP_ISO       0x00040000                  // isochronous endpoint
      #define OTG_FS_DIEPCTL_EPTYP_BULK      0x00080000                  // bulk endpoint
      #define OTG_FS_DIEPCTL_EPTYP_INTERRUPT 0x000c0000                  // interrupt endpoint
      #define OTG_FS_DIEPCTL_STALL           0x00200000                  // write '1' only to stall endpoint (cleared when SETUP received)
      #define OTG_FS_DIEPCTL_TXFNUM          0x03c00000                  // FIFO number assigned to endpoint 0
      #define OTG_FS_DIEPCTL_CNAK            0x04000000                  // clear NAK for endpoint (write-only)
      #define OTG_FS_DIEPCTL_SNAK            0x08000000                  // set NAK for endpoint (write-only)
      #define OTG_FS_DIEPCTL_EPDIS           0x40000000                  // disable transmitting data on endpoint 0 (cleared by USB controller)
      #define OTG_FS_DIEPCTL_EPENA           0x80000000                  // start transmitting data on endpoint 0 (cleared by USB controller)

    #define OTG_FS_DIEPINT0                  *(volatile unsigned long *)(USB_OTG_FS_BLOCK + 0x908) // OTG_FS device IN endpoint 0 interrupt register
      #define OTG_FS_DIEPINT_XFRC            0x00000001                  // transfer completed interrupt
      #define OTG_FS_DIEPINT_EPDISD          0x00000002                  // endpoint disabled interrupt
      #define OTG_FS_DIEPINT_TOC             0x00000008                  // timeout condition
      #define OTG_FS_DIEPINT_ITTXFE          0x00000010                  // IN token received when TxFIFO is empty
      #define OTG_FS_DIEPINT_INEPNE          0x00000040                  // IN endpoint NAK effective
      #define OTG_FS_DIEPINT_TXFE            0x00000080                  // TxFIFO empty

    #define OTG_FS_DIEPTSIZ0                 *(volatile unsigned long *)(USB_OTG_FS_BLOCK + 0x910) // OTG_FS device IN endpoint 0 transfer size register
      #define OTG_FS_DIEPTSIZ_XFRSIZ_MASK   0x0000007f                   // transfer size
      #define OTG_FS_DIEPTSIZ_PKTCNT_MASK   0x00180000                   // the total number of USB packets that constitute the transfer size amount of data
      #define OTG_FS_DIEPTSIZ_PKTCNT_1      0x00080000                   // one packet
      #define OTG_FS_DIEPTSIZ_PKTCNT_2      0x00100000                   // two packets
      #define OTG_FS_DIEPTSIZ_PKTCNT_3      0x00180000                   // three packets

    #define OTG_FS_DTXFSTS0                  *(volatile unsigned long *)(USB_OTG_FS_BLOCK + 0x918) // OTG_FS device IN endpoint 0 transmit FIFO status register
      #define OTG_FS_DTXFSTS0_INEPTFSAV      0x0000ffff

    #define OTG_FS_DIEPCTL1                  *(volatile unsigned long *)(USB_OTG_FS_BLOCK + 0x920) // OTG_FS device control IN endpoint 1 control register
      #define OTG_FS_DIEPCTLX_MPSIZ_MASK     0x000007ff
      #define OTG_FS_DIEPCTLX_EONUM_DPID     0x00010000                  // even frame (isochronous)/ DATA1 (interrupt/bulk)
      #define OTG_FS_DIEPCTLX_SD0PID_SEVNFRM 0x10000000                  // set DATA0 PID (interrupt/bulk) or set even frame (isochronous)
      #define OTG_FS_DIEPCTLX_SODDFRM        0x20000000                  // set odd frame (applies to isochronous IN/OUT endpoints only)

    #define OTG_FS_DIEPINT1                  *(volatile unsigned long *)(USB_OTG_FS_BLOCK + 0x928) // OTG_FS device IN endpoint 1 interrupt register

    #define OTG_FS_DIEPTSIZ1                 *(volatile unsigned long *)(USB_OTG_FS_BLOCK + 0x930) // OTG_FS device IN endpoint 1 transfer size register

    #define OTG_FS_DTXFSTS1                  *(volatile unsigned long *)(USB_OTG_FS_BLOCK + 0x938) // OTG_FS device IN endpoint 1 transmit FIFO status register

    #define OTG_FS_DIEPCTL2                  *(volatile unsigned long *)(USB_OTG_FS_BLOCK + 0x940) // OTG_FS device control IN endpoint 2 control register

    #define OTG_FS_DIEPINT2                  *(volatile unsigned long *)(USB_OTG_FS_BLOCK + 0x948) // OTG_FS device IN endpoint 2 interrupt register

    #define OTG_FS_DIEPTSIZ2                 *(volatile unsigned long *)(USB_OTG_FS_BLOCK + 0x950) // OTG_FS device IN endpoint 2 transfer size register

    #define OTG_FS_DTXFSTS2                  *(volatile unsigned long *)(USB_OTG_FS_BLOCK + 0x958) // OTG_FS device IN endpoint 2 transmit FIFO status register

    #define OTG_FS_DIEPCTL3                  *(volatile unsigned long *)(USB_OTG_FS_BLOCK + 0x960) // OTG_FS device control IN endpoint 3 control register

    #define OTG_FS_DIEPINT3                  *(volatile unsigned long *)(USB_OTG_FS_BLOCK + 0x968) // OTG_FS device IN endpoint 3 interrupt register

    #define OTG_FS_DIEPTSIZ3                 *(volatile unsigned long *)(USB_OTG_FS_BLOCK + 0x970) // OTG_FS device IN endpoint 3 transfer size register

    #define OTG_FS_DTXFSTS3                  *(volatile unsigned long *)(USB_OTG_FS_BLOCK + 0x978) // OTG_FS device IN endpoint 3 transmit FIFO status register

    #define OTG_FS_DOEPCTL0_ADD              (unsigned long *)(USB_OTG_FS_BLOCK + 0xb00)
    #define OTG_FS_DOEPCTL0                  *(volatile unsigned long *)(USB_OTG_FS_BLOCK + 0xb00) // OTG_FS device control OUT endpoint 0 control register
      #define OTG_FS_DOEPCTL_MPSIZ_64        0x00000000                  // (read-only) endpoint size 64 bytes
      #define OTG_FS_DOEPCTL_MPSIZ_32        0x00000001                  // (read-only) endpoint size 32 bytes
      #define OTG_FS_DOEPCTL_MPSIZ_16        0x00000002                  // (read-only) endpoint size 16 bytes
      #define OTG_FS_DOEPCTL_MPSIZ_8         0x00000003                  // (read-only) endpoint size 8 bytes
      #define OTG_FS_DOEPCTL_USBAEP          0x00008000                  // (read-only) endpoint active
      #define OTG_FS_DOEPCTL_NAKSTS          0x00020000                  // (read-only) NAK status
      #define OTG_FS_DOEPCTL_EPTYP           0x000c0000                  // (read-only)
      #define OTG_FS_DOEPCTL_SNPM            0x00100000                  // snoop mode
      #define OTG_FS_DOEPCTL_STALL           0x00200000                  // (write-only - set 1) stall handshake
      #define OTG_FS_DOEPCTL_CNAK            0x04000000                  // (write-only - set 1) clear NAK on endpoint
      #define OTG_FS_DOEPCTL_SNAK            0x08000000                  // (write-only - set 1) set NAK on endpoint
      #define OTG_FS_DOEPCTL_EPDIS           0x40000000                  // (read-only) endpoint 0 can not be disabled
      #define OTG_FS_DOEPCTL_EPENA           0x80000000                  // (write-only) enable endpoint

    #define OTG_FS_DOEPINT0                  *(volatile unsigned long *)(USB_OTG_FS_BLOCK + 0xb08) // OTG_FS device OUT endpoint 0 interrupt register
      #define OTG_FS_DOEPINT_XFRC            0x00000001                  // transfer completed interrupt
      #define OTG_FS_DOEPINT_EPDISD          0x00000002                  // endpoint disabled interrupt
      #define OTG_FS_DOEPINT_STUP            0x00000008                  // SETUP phase done
      #define OTG_FS_DOEPINT_OTEPDIS         0x00000010                  // OUT token received when endpoint is disabled
      #define OTG_FS_DOEPINT_B2BSTUP         0x00000040                  // back-to-back SETUP packets received

    #define OTG_FS_DOEPTSIZ0                 *(volatile unsigned long *)(USB_OTG_FS_BLOCK + 0xb10) // OTG_FS device OUT endpoint 0 transfer size register
      #define OTG_FS_DOEPTSIZ_XFRSIZ_MASK    0x0000007f
      #define OTG_FS_DOEPTSIZ_PKTCNT         0x00080000
      #define OTG_FS_DOEPTSIZ_STUPCNT_1      0x20000000                  // SETUP packet count - 1 packet (the number of back-to-back SETUP data packets that the endpoint can receive)
      #define OTG_FS_DOEPTSIZ_STUPCNT_2      0x40000000                  // SETUP packet count - 2 packets
      #define OTG_FS_DOEPTSIZ_STUPCNT_3      0x60000000                  // SETUP packet count - 3 packets

    #define OTG_FS_DOEPCTL1                  *(volatile unsigned long *)(USB_OTG_FS_BLOCK + 0xb20) // OTG_FS device control OUT endpoint 1 control register

    #define OTG_FS_DOEPINT1                  *(volatile unsigned long *)(USB_OTG_FS_BLOCK + 0xb28) // OTG_FS device OUT endpoint 1 interrupt register

    #define OTG_FS_DOEPTSIZ1                 *(volatile unsigned long *)(USB_OTG_FS_BLOCK + 0xb30) // OTG_FS device OUT endpoint 1 transfer size register

    #define OTG_FS_DOEPCTL2                  *(volatile unsigned long *)(USB_OTG_FS_BLOCK + 0xb40) // OTG_FS device control OUT endpoint 2 control register

    #define OTG_FS_DOEPINT2                  *(volatile unsigned long *)(USB_OTG_FS_BLOCK + 0xb48) // OTG_FS device OUT endpoint 2 interrupt register

    #define OTG_FS_DOEPTSIZ2                 *(volatile unsigned long *)(USB_OTG_FS_BLOCK + 0xb50) // OTG_FS device OUT endpoint 2 transfer size register

    #define OTG_FS_DOEPCTL3                  *(volatile unsigned long *)(USB_OTG_FS_BLOCK + 0xb60) // OTG_FS device control OUT endpoint 3 control register

    #define OTG_FS_DOEPINT3                  *(volatile unsigned long *)(USB_OTG_FS_BLOCK + 0xb68) // OTG_FS device OUT endpoint 3 interrupt register

    #define OTG_FS_DOEPTSIZ3                 *(volatile unsigned long *)(USB_OTG_FS_BLOCK + 0xb70) // OTG_FS device OUT endpoint 3 transfer size register

    #define OTG_FS_PCGCCTL                   *(volatile unsigned long *)(USB_OTG_FS_BLOCK + 0xe00) // OTG_FS power and clock gating control register
      #define OTG_FS_PCGCCTL_STPPCLK         0x00000001                  // stop PHY clock (when USB is suspended)
      #define OTG_FS_PCGCCTL_GATEHCLK        0x00000002                  // gate HCLK (to other AHB slave and masters and wakeup logic when USB is suspended or the session is not valid)
      #define OTG_FS_PCGCCTL_PHYSUSP         0x00000010                  // PHY suspended

    #define OTG_FS_DFIFO0_ADDR               (volatile unsigned long *)(USB_OTG_FS_BLOCK + 0x1000) // Data FIFO endpoint 0

    #define OTG_FS_DFIFO1_ADDR               (volatile unsigned long *)(USB_OTG_FS_BLOCK + 0x2000) // Data FIFO endpoint 1

    #define OTG_FS_DFIFO2_ADDR               (volatile unsigned long *)(USB_OTG_FS_BLOCK + 0x3000) // Data FIFO endpoint 2

    #define OTG_FS_DFIFO3_ADDR               (volatile unsigned long *)(USB_OTG_FS_BLOCK + 0x4000) // Data FIFO endpoint 3

    #define MULTIPLE_TX                      3                           // allow three packets to be queued

    #define USB_FIFO_BUFFER_DEPTH            1

    #define NUMBER_OF_USB_ENDPOINTS          4                           // device supports endpoint 0 plus three other endpoints

    typedef struct stUSB_END_POINT
    {
        unsigned long ulNextRxData0;
        unsigned long ulNextTxData0;
        unsigned long ulEndpointSize;                                    // contains size of endpoint plus some control flags
    } USB_END_POINT;


    typedef struct stUSB_HW
    {
        unsigned long  ulUSB_buffer[NUMBER_OF_USB_ENDPOINTS][64/sizeof(unsigned long)]; // linear, word aligned buffer large enough to hold largest single reception (there is one for each endpoint in case it is necessary to hold the input)
        USB_END_POINT *ptrEndpoint;
        unsigned char **ptrRxDatBuffer;                                  // pointer to the next reception buffer pointer
        unsigned char **ptrTxDatBuffer;                                  // pointer to the next transmission buffer pointer
        unsigned short usLength;                                         // length of present input being handled
        unsigned short usStoredLength[NUMBER_OF_USB_ENDPOINTS];          // length information being held in the input buffer
        unsigned long  ucUSBAddress;                                     // our USB address                       
    } USB_HW;

    extern void fnSendUSB_data(unsigned char *pData, unsigned short Len, int iEndpoint, USB_HW *ptrUSB_HW);
    extern void fnSendZeroData(USB_HW *ptrUSB_HW, int iEndpoint);
    extern void fnPutToFIFO(int iLength, volatile unsigned long *ptrRxFIFO, unsigned char *ptrBuffer);
    extern void fnPrepareTx(int Len, unsigned char *pData);


    #define FNSEND_USB_DATA(pData, Len, iEndpoint, ptrUSB_HW) fnSendUSB_data(pData, Len, iEndpoint, ptrUSB_HW)
    #define FNSEND_ZERO_DATA(ptrUSB_HW, iEndpoint)            fnSendZeroData(ptrUSB_HW, iEndpoint)
    #define SET_CONTROL_DIRECTION(a,b)
    #define CLEAR_SETUP(a)                                               // for compatibility with generic driver
    #define VALIDATE_NEW_CONFIGURATION()
#endif

// SPI (I2S)
//
#define SPI1_CR1                         *(volatile unsigned short *)(SPI1_BLOCK + 0x00)       // SPI Control Register
  #define  SPICR1_CPHA                   0x0001
  #define  SPICR1_CPOL                   0x0002
  #define  SPICR1_MSTR                   0x0004
  #define  SPICR1_BR_PCLK2_DIV2          0x0000
  #define  SPICR1_BR_PCLK2_DIV4          0x0008
  #define  SPICR1_BR_PCLK2_DIV8          0x0010
  #define  SPICR1_BR_PCLK2_DIV16         0x0018
  #define  SPICR1_BR_PCLK2_DIV32         0x0020
  #define  SPICR1_BR_PCLK2_DIV64         0x0028
  #define  SPICR1_BR_PCLK2_DIV128        0x0030
  #define  SPICR1_BR_PCLK2_DIV256        0x0038

  #define  SPICR1_BRMASK                 0x0038
  #define  SPICR1_SPE                    0x0040
  #define  SPICR1_LSB_FIRST              0x0080
  #define  SPICR1_SSI                    0x0100
  #define  SPICR1_SSM                    0x0200
  #define  SPICR1_RXONLY                 0x0400
  #define  SPICR1_DFF                    0x0800
  #define  SPICR1_CRCNEXT                0x1000
  #define  SPICR1_CRCEN                  0x2000
  #define  SPICR1_BIDIOE                 0x4000
  #define  SPICR1_BIDIMODE               0x8000
#define SPI1_CR2                         *(unsigned short *)(SPI1_BLOCK + 0x04)                // SPI Control Register 2
#define SPI1_SR                          *(volatile unsigned short *)(SPI1_BLOCK + 0x08)       // SPI Status Register
  #define SPISR_RXNE                     0x0001
  #define SPISR_TXE                      0x0002
  #define SPISR_CHSIDE                   0x0004
  #define SPISR_UDR                      0x0008
  #define SPISR_CRCER                    0x0010
  #define SPISR_MODF                     0x0020
  #define SPISR_OVR                      0x0040
  #define SPISR_BSY                      0x0080
#define SPI1_DR                          *(volatile unsigned short *)(SPI1_BLOCK + 0x0c)       // SPI Data Register
#define SPI1_CRCPR                       *(unsigned short *)(SPI1_BLOCK + 0x10)                // SPI CRC Polynomial Register
#define SPI1_RXCRCR                      *(volatile unsigned short *)(SPI1_BLOCK + 0x14)       // SPI Rx CRC Register
#define SPI1_TXCRCR                      *(volatile unsigned short *)(SPI1_BLOCK + 0x18)       // SPI Tx CRC Register
#define SPI1_I2SCFGR                     *(unsigned short *)(SPI1_I2S_BLOCK + 0x1c)            // I2S Configuration Register
#define SPI1_I2SPR                       *(unsigned short *)(SPI1_I2S_BLOCK + 0x20)            // I2S Prescaler Register

#define SPI2_CR1                         *(volatile unsigned short *)(SPI2_I2S_BLOCK + 0x00)   // SPI Control Register
#define SPI2_CR2                         *(unsigned short *)(SPI2_I2S_BLOCK + 0x04)            // SPI Control Register 2
#define SPI2_SR                          *(volatile unsigned short *)(SPI2_I2S_BLOCK + 0x08)   // SPI Status Register
#define SPI2_DR                          *(volatile unsigned short *)(SPI2_I2S_BLOCK + 0x0c)   // SPI Data Register
#define SPI2_CRCPR                       *(unsigned short *)(SPI2_I2S_BLOCK + 0x10)            // SPI CRC Polynomial Register
#define SPI2_RXCRCR                      *(volatile unsigned short *)(SPI2_I2S_BLOCK + 0x14)   // SPI Rx CRC Register
#define SPI2_TXCRCR                      *(volatile unsigned short *)(SPI2_I2S_BLOCK + 0x18)   // SPI Tx CRC Register
#define SPI2_I2SCFGR                     *(unsigned short *)(SPI2_I2S_BLOCK + 0x1c)            // I2S Configuration Register
#define SPI2_I2SPR                       *(unsigned short *)(SPI2_I2S_BLOCK + 0x20)            // I2S Prescaler Register

#define SPI3_CR1                         *(volatile unsigned short *)(SPI3_I2S_BLOCK + 0x00)   // SPI Control Register
#define SPI3_CR2                         *(unsigned short *)(SPI3_I2S_BLOCK + 0x04)            // SPI Control Register 2
#define SPI3_SR                          *(volatile unsigned short *)(SPI3_I2S_BLOCK + 0x08)   // SPI Status Register
#define SPI3_DR                          *(volatile unsigned short *)(SPI3_I2S_BLOCK + 0x0c)   // SPI Data Register
#define SPI3_CRCPR                       *(unsigned short *)(SPI3_I2S_BLOCK + 0x10)            // SPI CRC Polynomial Register
#define SPI3_RXCRCR                      *(volatile unsigned short *)(SPI3_I2S_BLOCK + 0x14)   // SPI Rx CRC Register
#define SPI3_TXCRCR                      *(volatile unsigned short *)(SPI3_I2S_BLOCK + 0x18)   // SPI Tx CRC Register
#define SPI3_I2SCFGR                     *(unsigned short *)(SPI3_I2S_BLOCK + 0x1c)            // I2S Configuration Register
#define SPI3_I2SPR                       *(unsigned short *)(SPI3_I2S_BLOCK + 0x20)            // I2S Prescaler Register


// System Configuration
//
#define SYSCFG_MEMRM                     *(unsigned long *)(SYSCFG_BLOCK + 0x00)
#define SYSCFG_PMC                       *(unsigned long *)(SYSCFG_BLOCK + 0x04)
    #define SYSCFG_PMC_MII_RMII_SEL      0x00800000                      // {7} select RMII mode rather than MII mode (this configuration needs to be performed while the MAC is under reset and before MAC clocks have been enabled)
#define SYSCFG_EXTICR1_ADDR              (unsigned long *)(SYSCFG_BLOCK + 0x08)
#define SYSCFG_EXTICR1                   *(unsigned long *)(SYSCFG_BLOCK + 0x08)
#define SYSCFG_EXTICR2                   *(unsigned long *)(SYSCFG_BLOCK + 0x0c)
#define SYSCFG_EXTICR3                   *(unsigned long *)(SYSCFG_BLOCK + 0x10)
#define SYSCFG_EXTICR4                   *(unsigned long *)(SYSCFG_BLOCK + 0x14)

#define SYSCFG_CMPCR                     *(unsigned long *)(SYSCFG_BLOCK + 0x20)


typedef struct stRESET_VECTOR
{
    void  *ptrResetSP;                                                   // initial stack pointer
    void  (*ptrResetPC)(void);                                           // initial program counter
} RESET_VECTOR;

// STM32 interrupts
//
typedef struct stPROCESSOR_IRQ
{
#if defined _STM32L4X5 || defined _STM32L4X6
    void  (*irq_WindowsWatchdog)(void);                                  // 0
    void  (*irq_PVD)(void);                                              // 1
    void  (*irq_RTC_TAMPER)(void);                                       // 2
    void  (*irq_RTC_WKUP)(void);                                         // 3
    void  (*irq_FLASH)(void);                                            // 4
    void  (*irq_RCC)(void);                                              // 5
    void  (*irq_EXTI0)(void);                                            // 6
    void  (*irq_EXTI1)(void);                                            // 7
    void  (*irq_EXTI2)(void);                                            // 8
    void  (*irq_EXTI3)(void);                                            // 9
    void  (*irq_EXTI4)(void);                                            // 10
    void  (*irq_DMA1_Channel1)(void);                                    // 11
    void  (*irq_DMA1_Channel2)(void);                                    // 12
    void  (*irq_DMA1_Channel3)(void);                                    // 13
    void  (*irq_DMA1_Channel4)(void);                                    // 14
    void  (*irq_DMA1_Channel5)(void);                                    // 15
    void  (*irq_DMA1_Channel6)(void);                                    // 16
    void  (*irq_DMA1_Channel7)(void);                                    // 17
    void  (*irq_ADC1_2)(void);                                           // 18
    void  (*irq_CAN1_TX)(void);                                          // 19
    void  (*irq_CAN1_RX0)(void);                                         // 20
    void  (*irq_CAN1_RX1)(void);                                         // 21
    void  (*irq_CAN1_SCE)(void);                                         // 22
    void  (*irq_EXTI9_5)(void);                                          // 23
    void  (*irq_TIM1_BRK_TIM15)(void);                                   // 24
    void  (*irq_TIM1_UP_TIM16)(void);                                    // 25
    void  (*irq_TIM1_TRG_COM_TIM17)(void);                               // 26
    void  (*irq_TIM1_CC)(void);                                          // 27
    void  (*irq_TIM2)(void);                                             // 28
    void  (*irq_TIM3)(void);                                             // 29
    void  (*irq_TIM4)(void);                                             // 30
    void  (*irq_I2C1_EV)(void);                                          // 31
    void  (*irq_I2C1_ER)(void);                                          // 32
    void  (*irq_I2C2_EV)(void);                                          // 33
    void  (*irq_I2C2_ER)(void);                                          // 34
    void  (*irq_SPI1)(void);                                             // 35
    void  (*irq_SPI2)(void);                                             // 36
    void  (*irq_USART1)(void);                                           // 37
    void  (*irq_USART2)(void);                                           // 38
    void  (*irq_USART3)(void);                                           // 39
    void  (*irq_EXTI15_10)(void);                                        // 40
    void  (*irq_RTC_ALARM)(void);                                        // 41
    void  (*irq_DFSDM1_FLT3)(void);                                      // 42
    void  (*irq_TIM8_BRK)(void);                                         // 43
    void  (*irq_TIM8_UP)(void);                                          // 44
    void  (*irq_TIM8_TRG_COM)(void);                                     // 45
    void  (*irq_TIM8_CC)(void);                                          // 46
    void  (*irq_ADC3)(void);                                             // 47
    void  (*irq_FCM)(void);                                              // 48
    void  (*irq_SDMMC1)(void);                                           // 49
    void  (*irq_TIM5)(void);                                             // 50
    void  (*irq_SPI3)(void);                                             // 51
    void  (*irq_UART4)(void);                                            // 52
    void  (*irq_UART5)(void);                                            // 53
    void  (*irq_TIM6_DACUNDER)(void);                                    // 54
    void  (*irq_TIM7)(void);                                             // 55
    void  (*irq_DMA2_CH1)(void);                                         // 56
    void  (*irq_DMA2_CH2)(void);                                         // 57
    void  (*irq_DMA2_CH3)(void);                                         // 58
    void  (*irq_DMA2_CH4)(void);                                         // 59
    void  (*irq_DMA2_CH5)(void);                                         // 60
    void  (*irq_DFSDM1_FLT0)(void);                                      // 61
    void  (*irq_DFSDM1_FLT1)(void);                                      // 62
    void  (*irq_DFSDM1_FLT2)(void);                                      // 63
    void  (*irq_COMP)(void);                                             // 64
    void  (*irq_LPTIM1)(void);                                           // 65
    void  (*irq_LPTIM2)(void);                                           // 66
    void  (*irq_USB_FS)(void);                                           // 67
    void  (*irq_DMA2_CH6)(void);                                         // 68
    void  (*irq_DMA2_CH7)(void);                                         // 69
    void  (*irq_LPUART1)(void);                                          // 70
    void  (*irq_QUADSPI)(void);                                          // 71
    void  (*irq_I2C3_EV)(void);                                          // 72
    void  (*irq_I2C3_ER)(void);                                          // 73
    void  (*irq_SAI1)(void);                                             // 74
    void  (*irq_SAI2)(void);                                             // 75
    void  (*irq_SWPMI1)(void);                                           // 76
    void  (*irq_TSC)(void);                                              // 77
    void  (*irq_LCD)(void);                                              // 78
    void  (*irq_AES)(void);                                              // 79
    void  (*irq_RND)(void);                                              // 80
    void  (*irq_FPU)(void);                                              // 81
    #if defined _STM32L496 || defined _STM32L4A6
        void  (*irq_HASH_CRS)(void);                                     // 82
        void  (*irq_I2C4_EV)(void);                                      // 83
        void  (*irq_I2C4_ER)(void);                                      // 84
        void  (*irq_DCMI)(void);                                         // 85
        void  (*irq_CAN2_TX)(void);                                      // 86
        void  (*irq_CAN2_RX0)(void);                                     // 87
        void  (*irq_CAN2_RX1)(void);                                     // 88
        void  (*irq_CAN2_SCE)(void);                                     // 89
        void  (*irq_DMA2D)(void);                                        // 90
    #endif
#elif defined _STM32L0x1 || defined _STM32F031
    void  (*irq_WindowsWatchdog)(void);                                  // 0
    void  (*irq_PVD)(void);                                              // 1
    void  (*irq_RTC)(void);                                              // 2
    void  (*irq_FLASH)(void);                                            // 3
    void  (*irq_RCC)(void);                                              // 4
    void  (*irq_EXTI0_1)(void);                                          // 5
    void  (*irq_EXTI2_3)(void);                                          // 6
    void  (*irq_EXTI4_15)(void);                                         // 7
    #if defined _STM32F031
        void(*irq_TSC)(void);                                            // 8
    #else
        void(*irq_res0)(void);                                           // 8
    #endif
    void  (*irq_DMA1_Channel1)(void);                                    // 9
    void  (*irq_DMA1_Channel2_3)(void);                                  // 10
    void  (*irq_DMA1_Channel4_7)(void);                                  // 11
    void  (*irq_ADC_COMP)(void);                                         // 12
    void  (*irq_LPTIM1)(void);                                           // 13
    void  (*irq_USART4_5)(void);                                         // 14
    void  (*irq_TIM2)(void);                                             // 15
    void  (*irq_TIM3)(void);                                             // 16
    void  (*irq_TIM6)(void);                                             // 17
    void  (*irq_TIM7)(void);                                             // 18
    #if defined _STM32F031
        void (*irq_TIM14)(void);                                         // 19
    #else
        void  (*irq_res1)(void);                                         // 19
    #endif
    void  (*irq_TIM21)(void);                                            // 20
    void  (*irq_I2C3)(void);                                             // 21
    void  (*irq_TIM22)(void);                                            // 22
    void  (*irq_I2C1)(void);                                             // 23
    void  (*irq_I2C2)(void);                                             // 24
    void  (*irq_SPI1)(void);                                             // 25
    void  (*irq_SPI2)(void);                                             // 26
    void  (*irq_USART1)(void);                                           // 27
    void  (*irq_USART2)(void);                                           // 28
    void  (*irq_LPUART1_AES)(void);                                      // 29
    #if defined _STM32F031
        void  (*irq_CEC_CAN)(void);                                      // 30
        void  (*irq_USB)(void);                                          // 31
    #endif
#else
    void  (*irq_WindowsWatchdog)(void);                                  // 0
    void  (*irq_PVD)(void);                                              // 1
    void  (*irq_Tamper)(void);                                           // 2
    void  (*irq_RTC)(void);                                              // 3
    void  (*irq_FLASH)(void);                                            // 4
    void  (*irq_RCC)(void);                                              // 5
    void  (*irq_EXTI0)(void);                                            // 6
    void  (*irq_EXTI1)(void);                                            // 7
    void  (*irq_EXTI2)(void);                                            // 8
    void  (*irq_EXTI3)(void);                                            // 9
    void  (*irq_EXTI4)(void);                                            // 10
    #if defined _STM32F2XX || defined _STM32F4XX || defined _STM32F7XX
    void  (*irq_DMA1_Stream0)(void);                                     // 11
    void  (*irq_DMA1_Stream1)(void);                                     // 12
    void  (*irq_DMA1_Stream2)(void);                                     // 13
    void  (*irq_DMA1_Stream3)(void);                                     // 14
    void  (*irq_DMA1_Stream4)(void);                                     // 15
    void  (*irq_DMA1_Stream5)(void);                                     // 16
    void  (*irq_DMA1_Stream6)(void);                                     // 17
    #else
    void  (*irq_DMA1_Channel1)(void);                                    // 11
    void  (*irq_DMA1_Channel2)(void);                                    // 12
    void  (*irq_DMA1_Channel3)(void);                                    // 13
    void  (*irq_DMA1_Channel4)(void);                                    // 14
    void  (*irq_DMA1_Channel5)(void);                                    // 15
    void  (*irq_DMA1_Channel6)(void);                                    // 16
    void  (*irq_DMA1_Channel7)(void);                                    // 17
    #endif
    #if defined _STM32F7XX
    void  (*irq_ADC1_2_3)(void);                                         // 18
    #else
    void  (*irq_ADC1_2_3)(void);                                         // 18
    #endif
    #if defined USB_DEVICE_AVAILABLE
        void  (*irq_USB_HP_CAN1_TX)(void);                               // 19
        void  (*irq_USB_LP_CAN1_RX0)(void);                              // 20
    #else
        void  (*irq_CAN1_TX)(void);                                      // 19
        void  (*irq_CAN1_RX0)(void);                                     // 20
    #endif
    void  (*irq_CAN1_RX1)(void);                                         // 21
    void  (*irq_CAN1_SCE)(void);                                         // 22
    void  (*irq_EXTI9_5)(void);                                          // 23
    void  (*irq_TIM1_BRK_TIM9)(void);                                    // 24
    void  (*irq_TIM1_UP_TIM10)(void);                                    // 25
    void  (*irq_TIM1_TRG_COM_TIM11)(void);                               // 26
    void  (*irq_TIM1_CC)(void);                                          // 27
    void  (*irq_TIM2)(void);                                             // 28
    void  (*irq_TIM3)(void);                                             // 29
    void  (*irq_TIM4)(void);                                             // 30
    void  (*irq_I2C1_EV)(void);                                          // 31
    void  (*irq_I2C1_ER)(void);                                          // 32
    void  (*irq_I2C2_EV)(void);                                          // 33
    void  (*irq_I2C2_ER)(void);                                          // 34
    void  (*irq_SPI1)(void);                                             // 35
    void  (*irq_SPI2)(void);                                             // 36
    void  (*irq_USART1)(void);                                           // 37
    void  (*irq_USART2)(void);                                           // 38
    void  (*irq_USART3)(void);                                           // 39
    void  (*irq_EXTI15_10)(void);                                        // 40
    void  (*irq_RTCAlarm)(void);                                         // 41
    void  (*irq_OTG_FS_WKUP)(void);                                      // 42
    #if defined _STM32F2XX || defined _STM32F4XX || defined _STM32F7XX
    void  (*irq_TIM8_BRK_TIM12)(void);                                   // 43
    void  (*irq_TIM8_UP_TIM13)(void);                                    // 44
    void  (*irq_TIM8_TRG_COM_TIM14)(void);                               // 45
    void  (*irq_TIM8_CC)(void);                                          // 46
    void  (*irq_DMA1_Stream7)(void);                                     // 47
    void  (*irq_FSMC)(void);                                             // 48
    void  (*irq_SDIO)(void);                                             // 49
    #else
    void  (*irq_res1)(void);                                             // 43
    void  (*irq_res2)(void);                                             // 44
    void  (*irq_res3)(void);                                             // 45
    void  (*irq_res4)(void);                                             // 46
    void  (*irq_res5)(void);                                             // 47
    void  (*irq_res6)(void);                                             // 48
    void  (*irq_res7)(void);                                             // 49
    #endif
    void  (*irq_TIM5)(void);                                             // 50
    void  (*irq_SPI3)(void);                                             // 51
    void  (*irq_UART4)(void);                                            // 52
    void  (*irq_UART5)(void);                                            // 53
    #if defined _STM32F2XX || defined _STM32F4XX || defined _STM32F7XX
    void  (*irq_TIM6_DAC)(void);                                         // 54
    #else
    void  (*irq_TIM6)(void);                                             // 54
    #endif
    void  (*irq_TIM7)(void);                                             // 55
    #if defined _STM32F2XX || defined _STM32F4XX || defined _STM32F7XX
    void  (*irq_DMA2_Stream0)(void);                                     // 56
    void  (*irq_DMA2_Stream1)(void);                                     // 57
    void  (*irq_DMA2_Stream2)(void);                                     // 58
    void  (*irq_DMA2_Stream3)(void);                                     // 59
    void  (*irq_DMA2_Stream4)(void);                                     // 60
    #else
    void  (*irq_DMA2_Channel1)(void);                                    // 56
    void  (*irq_DMA2_Channel2)(void);                                    // 57
    void  (*irq_DMA2_Channel3)(void);                                    // 58
    void  (*irq_DMA2_Channel4)(void);                                    // 59
    void  (*irq_DMA2_Channel5)(void);                                    // 60
    #endif
    void  (*irq_ETH)(void);                                              // 61
    void  (*irq_ETH_WKUP)(void);                                         // 62
    void  (*irq_CAN2_TX)(void);                                          // 63
    void  (*irq_CAN2_RX0)(void);                                         // 64
    void  (*irq_CAN2_RX1)(void);                                         // 65
    void  (*irq_CAN2_SCE)(void);                                         // 66
    void  (*irq_OTG_FS)(void);                                           // 67
    #if defined _STM32F2XX || defined _STM32F4XX || defined _STM32F7XX
    void  (*irq_DMA2_Stream5)(void);                                     // 68
    void  (*irq_DMA2_Stream6)(void);                                     // 69
    void  (*irq_DMA2_Stream7)(void);                                     // 70
    void  (*irq_USART6)(void);                                           // 71
    void  (*irq_I2C3_EV)(void);                                          // 72
    void  (*irq_I2C3_ER)(void);                                          // 73
    void  (*irq_OTG_HS_EP1_OUT)(void);                                   // 74
    void  (*irq_OTG_HS_EP1_IN)(void);                                    // 75
    void  (*irq_OTG_HS_WKUP)(void);                                      // 76
    void  (*irq_OTG_HS)(void);                                           // 77
    void  (*irq_DCMI)(void);                                             // 78
    void  (*irq_CRYP)(void);                                             // 79
    void  (*irq_HASH_RNG)(void);                                         // 80
    void  (*irq_FPU)(void);                                              // 81
    #endif
    #if defined _STM32F7XX || defined _STM32F429 || defined _STM32F427
    void  (*irq_UART7)(void);                                            // 82
    void  (*irq_UART8)(void);                                            // 83
    #endif
    #if defined _STM32F7XX
    void  (*irq_SPI4)(void);                                             // 84
    void  (*irq_SPI5)(void);                                             // 85
    void  (*irq_SPI6)(void);                                             // 86
    void  (*irq_SAI1)(void);                                             // 87
    void  (*irq_LCD_TFT)(void);                                          // 88
    void  (*irq_LCD_TFT_ERR)(void);                                      // 89
    void  (*irq_DMA2D)(void);                                            // 90
    void  (*irq_SAI2)(void);                                             // 91
    void  (*irq_QUAD_SPI)(void);                                         // 92
    void  (*irq_LPTIMER_1)(void);                                        // 93
    void  (*irq_HDMI_CEC)(void);                                         // 94
    void  (*irq_I2C4_EV)(void);                                          // 95
    void  (*irq_I2C4_ER)(void);                                          // 96
    void  (*irq_SPDIFRX)(void);                                          // 97
    #endif
#endif
} PROCESSOR_IRQ;


typedef struct stVECTOR_TABLE
{
    RESET_VECTOR  reset_vect;
    void  (*ptrNMI)(void);
    void  (*ptrHardFault)(void);
    void  (*ptrMemManagement)(void);
    void  (*ptrBusFault)(void);
    void  (*ptrUsageFault)(void);
    unsigned long ptrReserved1[4];
    void  (*ptrSVCall)(void);
    void  (*ptrDebugMonitor)(void);
    unsigned long ptrReserved2;
    void  (*ptrPendSV)(void);
    void  (*ptrSysTick)(void);
    PROCESSOR_IRQ processor_interrupts;                                  // length is processor specific
} VECTOR_TABLE;

#define VECTOR_SIZE                      (sizeof(VECTOR_TABLE))

#if defined _STM32L0x1
    #define LAST_PROCESSOR_IRQ  irq_LPUART1_AES
    #define CHECK_VECTOR_SIZE            184                             // (16 + 29 + 1) = 46) * 4 - adequate for this processor
#elif defined _STM32L4X5 || defined _STM32L4X6
    #if defined _STM32L496 || defined _STM32L4
        #define LAST_PROCESSOR_IRQ  irq_DMA2D
        #define CHECK_VECTOR_SIZE        428                             // (16 + 90 + 1) = 107) * 4 - adequate for this processor
    #else
        #define LAST_PROCESSOR_IRQ  irq_FPU
        #define CHECK_VECTOR_SIZE        392                             // (16 + 81 + 1) = 98) * 4 - adequate for this processor
    #endif
#elif defined _STM32F031
    #define LAST_PROCESSOR_IRQ  irq_USB
    #define CHECK_VECTOR_SIZE            192                             // (16 + 31 + 1) = 48) * 4 - adequate for this processor
#elif defined _STM32F7XX
    #define LAST_PROCESSOR_IRQ  irq_SPDIFRX
    #define CHECK_VECTOR_SIZE            456                             // (16 + 97 + 1) = 114) * 4 - adequate for this processor
#elif defined _STM32F429 || defined _STM32F427
    #define LAST_PROCESSOR_IRQ  irq_UART8
    #define CHECK_VECTOR_SIZE            400                             // (16 + 83 + 1) = 100) * 4 - adequate for this processor
#elif defined _STM32F2XX || defined _STM32F4XX
    #define LAST_PROCESSOR_IRQ  irq_FPU
    #define CHECK_VECTOR_SIZE            392                             // (16 + 81 + 1) = 98) * 4 - adequate for this processor
#else
    #define LAST_PROCESSOR_IRQ  irq_OTG_FS
    #define CHECK_VECTOR_SIZE            336                             // (16 + 67 + 1) = 84) * 4 - adequate for this processor
#endif



// Cortex M3 private registers
//
// NVIC
//
#define INT_CONT_TYPE               *(const unsigned long*)(CORTEX_M3_BLOCK + 0x04)    // NVIC Interrupt Controller Type Register (read only)
#if defined ARM_MATH_CM0PLUS || defined ARM_MATH_CM0
    #define __NVIC_PRIORITY_SHIFT   6                                    // 4 levels of priority so shifted by (8 - 2 (number of implemented bits))
#else
    #define __NVIC_PRIORITY_SHIFT   4                                    // 16 levels of priority so shifted by (8 - 4 (number of implemented bits))
#endif

// SYSTICK
//
#define SYSTICK_CSR                 *(volatile unsigned long*)(CORTEX_M3_BLOCK + 0x10) // SYSTICK Control and Status Register
  #define SYSTICK_ENABLE            0x00000001
  #define SYSTICK_TICKINT           0x00000002
  #define SYSTICK_CORE_CLOCK        0x00000004
  #define SYSTICK_COUNTFLAG         0x00010000
#define SYSTICK_RELOAD              *(unsigned long*)(CORTEX_M3_BLOCK + 0x14)          // SYSTICK Reload value
#define SYSTICK_CURRENT             *(unsigned long*)(CORTEX_M3_BLOCK + 0x18)          // SYSTICK Current value
  #define SYSTICK_COUNT_MASK        0x00ffffff                                         // valid count width in registers
#define SYSTICK_CALIB               *(const unsigned long*)(CORTEX_M3_BLOCK + 0x1c)    // SYSTICK Calibration value (read-only)

// NVIC
//
#define IRQ0_31_SER_ADD             ( unsigned long*)(CORTEX_M3_BLOCK + 0x100)
#define IRQ0_31_SER                 *(volatile unsigned long*)(CORTEX_M3_BLOCK + 0x100)// NVIC IRQ0..31    Set Enable Register
#define IRQ32_63_SER                *(volatile unsigned long*)(CORTEX_M3_BLOCK + 0x104)// NVIC IRQ32..64   Set Enable Register
#define IRQ64_95_SER                *(volatile unsigned long*)(CORTEX_M3_BLOCK + 0x108)// NVIC IRQ64..95   Set Enable Register
#define IRQ96_127_SER               *(volatile unsigned long*)(CORTEX_M3_BLOCK + 0x10c)// NVIC IRQ96..127  Set Enable Register
#define IRQ128_159_SER              *(volatile unsigned long*)(CORTEX_M3_BLOCK + 0x110)// NVIC IRQ128..159 Set Enable Register
#define IRQ160_191_SER              *(volatile unsigned long*)(CORTEX_M3_BLOCK + 0x114)// NVIC IRQ160..191 Set Enable Register
#define IRQ192_223_SER              *(volatile unsigned long*)(CORTEX_M3_BLOCK + 0x118)// NVIC IRQ192..223 Set Enable Register
#define IRQ224_239_SER              *(volatile unsigned long*)(CORTEX_M3_BLOCK + 0x11c)// NVIC IRQ224..239 Set Enable Register

#define IRQ0_31_CER                 *(volatile unsigned long*)(CORTEX_M3_BLOCK + 0x180)// NVIC IRQ0..31    Clear Enable Register
#define IRQ32_63_CER                *(volatile unsigned long*)(CORTEX_M3_BLOCK + 0x184)// NVIC IRQ32..64   Clear Enable Register
#define IRQ64_95_CER                *(volatile unsigned long*)(CORTEX_M3_BLOCK + 0x188)// NVIC IRQ64..95   Clear Enable Register
#define IRQ96_127_CER               *(volatile unsigned long*)(CORTEX_M3_BLOCK + 0x18c)// NVIC IRQ96..127  Clear Enable Register
#define IRQ128_159_CER              *(volatile unsigned long*)(CORTEX_M3_BLOCK + 0x190)// NVIC IRQ128..159 Clear Enable Register
#define IRQ160_191_CER              *(volatile unsigned long*)(CORTEX_M3_BLOCK + 0x194)// NVIC IRQ160..191 Clear Enable Register
#define IRQ192_223_CER              *(volatile unsigned long*)(CORTEX_M3_BLOCK + 0x198)// NVIC IRQ192..223 Clear Enable Register
#define IRQ224_239_CER              *(volatile unsigned long*)(CORTEX_M3_BLOCK + 0x19c)// NVIC IRQ224..239 Clear Enable Register

#define IRQ0_31_SPR                 *( unsigned long*)(CORTEX_M3_BLOCK + 0x200)        // NVIC IRQ0..31    Set Pending Register
#define IRQ32_63_SPR                *( unsigned long*)(CORTEX_M3_BLOCK + 0x204)        // NVIC IRQ32..64   Set Pending Register
#define IRQ64_95_SPR                *( unsigned long*)(CORTEX_M3_BLOCK + 0x208)        // NVIC IRQ64..95   Set Pending Register
#define IRQ96_127_SPR               *( unsigned long*)(CORTEX_M3_BLOCK + 0x20c)        // NVIC IRQ96..127  Set Pending Register
#define IRQ128_159_SPR              *( unsigned long*)(CORTEX_M3_BLOCK + 0x210)        // NVIC IRQ128..159 Set Pending Register
#define IRQ160_191_SPR              *( unsigned long*)(CORTEX_M3_BLOCK + 0x214)        // NVIC IRQ160..191 Set Pending Register
#define IRQ192_223_SPR              *( unsigned long*)(CORTEX_M3_BLOCK + 0x218)        // NVIC IRQ192..223 Set Pending Register
#define IRQ224_239_SPR              *( unsigned long*)(CORTEX_M3_BLOCK + 0x21c)        // NVIC IRQ224..239 Set Pending Register

#define IRQ0_31_CPR                 *( unsigned long*)(CORTEX_M3_BLOCK + 0x280)        // NVIC IRQ0..31    Clear Pending Register
#define IRQ32_63_CPR                *( unsigned long*)(CORTEX_M3_BLOCK + 0x284)        // NVIC IRQ32..64   Clear Pending Register
#define IRQ64_95_CPR                *( unsigned long*)(CORTEX_M3_BLOCK + 0x288)        // NVIC IRQ64..95   Clear Pending Register
#define IRQ96_127_CPR               *( unsigned long*)(CORTEX_M3_BLOCK + 0x28c)        // NVIC IRQ96..127  Clear Pending Register
#define IRQ128_159_CPR              *( unsigned long*)(CORTEX_M3_BLOCK + 0x290)        // NVIC IRQ128..159 Clear Pending Register
#define IRQ160_191_CPR              *( unsigned long*)(CORTEX_M3_BLOCK + 0x294)        // NVIC IRQ160..191 Clear Pending Register
#define IRQ192_223_CPR              *( unsigned long*)(CORTEX_M3_BLOCK + 0x298)        // NVIC IRQ192..223 Clear Pending Register
#define IRQ224_239_CPR              *( unsigned long*)(CORTEX_M3_BLOCK + 0x29c)        // NVIC IRQ224..239 Clear Pending Register

#define IRQ0_31_ABR                 *(volatile unsigned long*)(CORTEX_M3_BLOCK + 0x300)// NVIC IRQ0..31    Active Bit Register (read only)
#define IRQ32_63_ABR                *(volatile unsigned long*)(CORTEX_M3_BLOCK + 0x304)// NVIC IRQ32..64   Active Bit Register (read only)
#define IRQ64_95_ABR                *(volatile unsigned long*)(CORTEX_M3_BLOCK + 0x308)// NVIC IRQ64..95   Active Bit Register (read only)
#define IRQ96_127_ABR               *(volatile unsigned long*)(CORTEX_M3_BLOCK + 0x30c)// NVIC IRQ96..127  Active Bit Register (read only)
#define IRQ128_159_ABR              *(volatile unsigned long*)(CORTEX_M3_BLOCK + 0x310)// NVIC IRQ128..159 Active Bit Register (read only)
#define IRQ160_191_ABR              *(volatile unsigned long*)(CORTEX_M3_BLOCK + 0x314)// NVIC IRQ160..191 Active Bit Register (read only)
#define IRQ192_223_ABR              *(volatile unsigned long*)(CORTEX_M3_BLOCK + 0x318)// NVIC IRQ192..223 Active Bit Register (read only)
#define IRQ224_239_ABR              *(volatile unsigned long*)(CORTEX_M3_BLOCK + 0x31c)// NVIC IRQ224..239 Active Bit Register (read only)

#define IRQ0_3_PRIORITY_REGISTER_ADD ( unsigned char*)(CORTEX_M3_BLOCK + 0x400)
#define IRQ0_3_PRIORITY_REGISTER    *( unsigned long*)(CORTEX_M3_BLOCK + 0x400)        // NVIC IRQ0..3     Priority Register
#define IRQ4_7_PRIORITY_REGISTER    *( unsigned long*)(CORTEX_M3_BLOCK + 0x404)        // NVIC IRQ4..7     Priority Register
#define IRQ8_11_PRIORITY_REGISTER   *( unsigned long*)(CORTEX_M3_BLOCK + 0x408)        // NVIC IRQ8..11    Priority Register
#define IRQ12_15_PRIORITY_REGISTER  *( unsigned long*)(CORTEX_M3_BLOCK + 0x40c)        // NVIC IRQ12..15   Priority Register
#define IRQ16_19_PRIORITY_REGISTER  *( unsigned long*)(CORTEX_M3_BLOCK + 0x410)        // NVIC IRQ16..19   Priority Register
#define IRQ20_23_PRIORITY_REGISTER  *( unsigned long*)(CORTEX_M3_BLOCK + 0x414)        // NVIC IRQ20..23   Priority Register
#define IRQ24_27_PRIORITY_REGISTER  *( unsigned long*)(CORTEX_M3_BLOCK + 0x418)        // NVIC IRQ24..27   Priority Register
#define IRQ28_31_PRIORITY_REGISTER  *( unsigned long*)(CORTEX_M3_BLOCK + 0x41c)        // NVIC IRQ28..31   Priority Register
#define IRQ32_35_PRIORITY_REGISTER  *( unsigned long*)(CORTEX_M3_BLOCK + 0x420)        // NVIC IRQ32..35   Priority Register
#define IRQ36_39_PRIORITY_REGISTER  *( unsigned long*)(CORTEX_M3_BLOCK + 0x424)        // NVIC IRQ36..39   Priority Register
#define IRQ40_43_PRIORITY_REGISTER  *( unsigned long*)(CORTEX_M3_BLOCK + 0x428)        // NVIC IRQ40..43   Priority Register
#define IRQ44_47_PRIORITY_REGISTER  *( unsigned long*)(CORTEX_M3_BLOCK + 0x42c)        // NVIC IRQ44..47   Priority Register
#define IRQ48_51_PRIORITY_REGISTER  *( unsigned long*)(CORTEX_M3_BLOCK + 0x430)        // NVIC IRQ48..51   Priority Register
#define IRQ52_55_PRIORITY_REGISTER  *( unsigned long*)(CORTEX_M3_BLOCK + 0x434)        // NVIC IRQ52..55   Priority Register
#define IRQ56_59_PRIORITY_REGISTER  *( unsigned long*)(CORTEX_M3_BLOCK + 0x438)        // NVIC IRQ56..59   Priority Register
#define IRQ60_63_PRIORITY_REGISTER  *( unsigned long*)(CORTEX_M3_BLOCK + 0x43c)        // NVIC IRQ60..63   Priority Register
#define IRQ64_67_PRIORITY_REGISTER  *( unsigned long*)(CORTEX_M3_BLOCK + 0x440)        // NVIC IRQ64..67   Priority Register
#define IRQ68_71_PRIORITY_REGISTER  *( unsigned long*)(CORTEX_M3_BLOCK + 0x444)        // NVIC IRQ68..71   Priority Register
#define IRQ72_75_PRIORITY_REGISTER  *( unsigned long*)(CORTEX_M3_BLOCK + 0x448)        // NVIC IRQ72..75   Priority Register
#define IRQ76_79_PRIORITY_REGISTER  *( unsigned long*)(CORTEX_M3_BLOCK + 0x44c)        // NVIC IRQ76..79   Priority Register
#define IRQ80_83_PRIORITY_REGISTER  *( unsigned long*)(CORTEX_M3_BLOCK + 0x450)        // NVIC IRQ80..83   Priority Register
#define IRQ84_87_PRIORITY_REGISTER  *( unsigned long*)(CORTEX_M3_BLOCK + 0x454)        // NVIC IRQ84..87   Priority Register
#define IRQ88_91_PRIORITY_REGISTER  *( unsigned long*)(CORTEX_M3_BLOCK + 0x458)        // NVIC IRQ88..91   Priority Register
#define IRQ92_95_PRIORITY_REGISTER  *( unsigned long*)(CORTEX_M3_BLOCK + 0x45c)        // NVIC IRQ92..95   Priority Register
#define IRQ96_99_PRIORITY_REGISTER  *( unsigned long*)(CORTEX_M3_BLOCK + 0x460)        // NVIC IRQ96..99   Priority Register
#define IRQ100_103_PRIORITY_REGISTER  *( unsigned long*)(CORTEX_M3_BLOCK + 0x464)      // NVIC IRQ100..103   Priority Register
#define IRQ104_107_PRIORITY_REGISTER  *( unsigned long*)(CORTEX_M3_BLOCK + 0x468)      // NVIC IRQ104..107   Priority Register
#define IRQ108_111_PRIORITY_REGISTER  *( unsigned long*)(CORTEX_M3_BLOCK + 0x46c)      // NVIC IRQ108..111   Priority Register
#define IRQ112_115_PRIORITY_REGISTER  *( unsigned long*)(CORTEX_M3_BLOCK + 0x470)      // NVIC IRQ112..115   Priority Register
#define IRQ116_119_PRIORITY_REGISTER  *( unsigned long*)(CORTEX_M3_BLOCK + 0x474)      // NVIC IRQ116..119   Priority Register
#define IRQ120_123_PRIORITY_REGISTER  *( unsigned long*)(CORTEX_M3_BLOCK + 0x478)      // NVIC IRQ120..123   Priority Register
#define IRQ124_127_PRIORITY_REGISTER  *( unsigned long*)(CORTEX_M3_BLOCK + 0x47c)      // NVIC IRQ124..127   Priority Register
#define IRQ128_131_PRIORITY_REGISTER  *( unsigned long*)(CORTEX_M3_BLOCK + 0x480)      // NVIC IRQ128..131   Priority Register
#define IRQ132_135_PRIORITY_REGISTER  *( unsigned long*)(CORTEX_M3_BLOCK + 0x484)      // NVIC IRQ132..135   Priority Register
#define IRQ136_139_PRIORITY_REGISTER  *( unsigned long*)(CORTEX_M3_BLOCK + 0x488)      // NVIC IRQ136..139   Priority Register
#define IRQ140_143_PRIORITY_REGISTER  *( unsigned long*)(CORTEX_M3_BLOCK + 0x48c)      // NVIC IRQ140..143   Priority Register
#define IRQ144_147_PRIORITY_REGISTER  *( unsigned long*)(CORTEX_M3_BLOCK + 0x490)      // NVIC IRQ144..147   Priority Register
#define IRQ148_151_PRIORITY_REGISTER  *( unsigned long*)(CORTEX_M3_BLOCK + 0x494)      // NVIC IRQ148..151   Priority Register
#define IRQ152_155_PRIORITY_REGISTER  *( unsigned long*)(CORTEX_M3_BLOCK + 0x498)      // NVIC IRQ152..155   Priority Register
#define IRQ156_159_PRIORITY_REGISTER  *( unsigned long*)(CORTEX_M3_BLOCK + 0x49c)      // NVIC IRQ156..159   Priority Register
#define IRQ160_163_PRIORITY_REGISTER  *( unsigned long*)(CORTEX_M3_BLOCK + 0x4a0)      // NVIC IRQ160..163   Priority Register
#define IRQ164_167_PRIORITY_REGISTER  *( unsigned long*)(CORTEX_M3_BLOCK + 0x4a4)      // NVIC IRQ164..167   Priority Register
#define IRQ168_171_PRIORITY_REGISTER  *( unsigned long*)(CORTEX_M3_BLOCK + 0x4a8)      // NVIC IRQ168..171   Priority Register
#define IRQ172_175_PRIORITY_REGISTER  *( unsigned long*)(CORTEX_M3_BLOCK + 0x4ac)      // NVIC IRQ172..175   Priority Register
#define IRQ176_179_PRIORITY_REGISTER  *( unsigned long*)(CORTEX_M3_BLOCK + 0x4b0)      // NVIC IRQ176..179   Priority Register
#define IRQ180_183_PRIORITY_REGISTER  *( unsigned long*)(CORTEX_M3_BLOCK + 0x4b4)      // NVIC IRQ180..183   Priority Register
#define IRQ184_187_PRIORITY_REGISTER  *( unsigned long*)(CORTEX_M3_BLOCK + 0x4b8)      // NVIC IRQ184..187   Priority Register
#define IRQ188_191_PRIORITY_REGISTER  *( unsigned long*)(CORTEX_M3_BLOCK + 0x4bc)      // NVIC IRQ188..191   Priority Register
#define IRQ192_195_PRIORITY_REGISTER  *( unsigned long*)(CORTEX_M3_BLOCK + 0x4c0)      // NVIC IRQ192..195   Priority Register
#define IRQ196_199_PRIORITY_REGISTER  *( unsigned long*)(CORTEX_M3_BLOCK + 0x4c4)      // NVIC IRQ196..199   Priority Register
#define IRQ200_203_PRIORITY_REGISTER  *( unsigned long*)(CORTEX_M3_BLOCK + 0x4c8)      // NVIC IRQ200..203   Priority Register
#define IRQ204_207_PRIORITY_REGISTER  *( unsigned long*)(CORTEX_M3_BLOCK + 0x4cc)      // NVIC IRQ204..207   Priority Register
#define IRQ208_211_PRIORITY_REGISTER  *( unsigned long*)(CORTEX_M3_BLOCK + 0x4d0)      // NVIC IRQ208..211   Priority Register
#define IRQ212_215_PRIORITY_REGISTER  *( unsigned long*)(CORTEX_M3_BLOCK + 0x4d4)      // NVIC IRQ212..215   Priority Register
#define IRQ216_219_PRIORITY_REGISTER  *( unsigned long*)(CORTEX_M3_BLOCK + 0x4d8)      // NVIC IRQ216..219   Priority Register
#define IRQ220_223_PRIORITY_REGISTER  *( unsigned long*)(CORTEX_M3_BLOCK + 0x4dc)      // NVIC IRQ220..223   Priority Register
#define IRQ224_227_PRIORITY_REGISTER  *( unsigned long*)(CORTEX_M3_BLOCK + 0x4e0)      // NVIC IRQ224..227   Priority Register
#define IRQ228_231_PRIORITY_REGISTER  *( unsigned long*)(CORTEX_M3_BLOCK + 0x4e4)      // NVIC IRQ228..231   Priority Register
#define IRQ232_235_PRIORITY_REGISTER  *( unsigned long*)(CORTEX_M3_BLOCK + 0x4e8)      // NVIC IRQ232..235   Priority Register
#define IRQ236_239_PRIORITY_REGISTER  *( unsigned long*)(CORTEX_M3_BLOCK + 0x4ec)      // NVIC IRQ236..239   Priority Register

#define CPUID_BASE_REGISTER           *(unsigned long*)(CORTEX_M3_BLOCK + 0xd00)       // (read only)
#define INT_CONT_STATE_REG            *(volatile unsigned long*)(CORTEX_M3_BLOCK + 0xd04) // Interrupt Control State Register
  #define NMIPENDSET                  0x80000000                                       // set pending NMI bit
  #define PENDSVSET                   0x10000000                                       // set pending pendSV bit
  #define PENDSVCLR                   0x08000000                                       // clear pending pendSV bit
  #define PENDSTSET                   0x04000000                                       // set pending sysTick bit
  #define PENDSTCLR                   0x02000000                                       // clear pending sysTick bit
  #define ISRPREEMPT                  0x00800000                                       //
  #define ISRPENDING                  0x00400000                                       // Interrupt Pending Flag
  #define VECT_PENDING_MASK           0x003ff000                                       // Pending ISR number field
  #define RETTOBASE                   0x00000800                                       //
  #define VECT_ACTIVE_MASK            0x00000010                                       // Active ISR number field
#define VECTOR_TABLE_OFFSET_REG       *(unsigned long*)(CORTEX_M3_BLOCK + 0xd08)       // Interrupt Control State Register
  #define TBLBASE_IN_RAM              0x20000000                                       // vector table base is in RAM
  #define TBLBASE_IN_CODE             0x00000000
  #define TBLOFF_MASK                 0x1fffff80                                       // table offset from bottom of Code / RAM
#define APPLICATION_INT_RESET_CTR_REG *(unsigned long*)(CORTEX_M3_BLOCK + 0xd0c)       // Application Interrupt and Reset Control Register
  #define VECTKEY                     0x05fa0000
  #define ENDIANESS_BIG               0x00008000
  #define ENDIANESS_LITTLE            0x00000000
  #define PRIGROUP_7_1                0x00000100
  #define PRIGROUP_6_2                0x00000200
  #define PRIGROUP_5_3                0x00000300
  #define PRIGROUP_4_4                0x00000400
  #define PRIGROUP_3_5                0x00000500
  #define PRIGROUP_2_6                0x00000600
  #define PRIGROUP_1_7                0x00000700
  #define PRIGROUP_0_8                0x00000800
  #define SYSRESETREQ                 0x00000004
  #define VECTCLRACTIVE               0x00000002
  #define VECTRESET                   0x00000001
#define SYSTEM_CONTROL_REGISTER       *(unsigned long*)(CORTEX_M3_BLOCK + 0xd10)       // System Control Register
  #define SLEEPONEXIT                 0x00000002
  #define SLEEPDEEP                   0x00000004
  #define SEVONPEND                   0x00000010
#define CONFIGURATION_CONTROL_REGISTER *(unsigned long*)(CORTEX_M3_BLOCK + 0xd14)      // Configuration Control Register

#define SYSTEM_HANDLER_4_7_PRIORITY_REGISTER *(unsigned long*)(CORTEX_M3_BLOCK + 0xd18) // System Handler Priority Register 4..7
#define SYSTEM_HANDLER_8_11_PRIORITY_REGISTER *(unsigned long*)(CORTEX_M3_BLOCK + 0xd1c) // System Handler Priority Register 8..11
#define SYSTEM_HANDLER_12_15_PRIORITY_REGISTER *(unsigned long*)(CORTEX_M3_BLOCK + 0xd20) // System Handler Priority Register 12..15

#define CPACR *(unsigned long *)(CORTEX_M3_BLOCK + 0xd88)                // co-processor access control register

#if defined STM32_FPU
    #define FPCCR *(unsigned long *)(CORTEX_M3_BLOCK + 0xf34)            // floating point context control register
    #define __FPU_PRESENT  1
#endif

#if defined ARM_MATH_CM4 || defined ARM_MATH_CM7                         // {26}
    // Cortex debug registers
    //
    #define DHCSR                     *(volatile unsigned long *)(CORTEX_M4_DEBUG + 0x0)
        #define DHCSR_TRCENA          0x01000000                         // trace enable
    #define DCRSR                     *(volatile unsigned long *)(CORTEX_M4_DEBUG + 0x4)
    #define DCRDR                     *(volatile unsigned long *)(CORTEX_M4_DEBUG + 0x8)
    #define DEMCR                     *(volatile unsigned long *)(CORTEX_M4_DEBUG + 0xc)

    // Cortex data watch and trace unit
    //
    #define DWT_CTRL                  *(volatile unsigned long *)(CORTEX_M4_DWT + 0x00) // control register
        #define DWT_CTRL_CYCCNTENA    0x00000001                         // enable the cycle counter
    #define DWT_CYCCNT                *(volatile unsigned long *)(CORTEX_M4_DWT + 0x04) // cycle count register
    #define DWT_CPICNT                *(volatile unsigned long *)(CORTEX_M4_DWT + 0x08) // CPI count register
    #define DWT_EXCCNT                *(volatile unsigned long *)(CORTEX_M4_DWT + 0x0c) // exception overhead count register
    #define DWT_SLEEPVNT              *(volatile unsigned long *)(CORTEX_M4_DWT + 0x10) // sleep count register
    #define DWT_LSUCNT                *(volatile unsigned long *)(CORTEX_M4_DWT + 0x14) // LSU count register
    #define DWT_FOLDCNT               *(volatile unsigned long *)(CORTEX_M4_DWT + 0x18) // folder-instruction count register
    #define DWT_PCSR                  *(volatile unsigned long *)(CORTEX_M4_DWT + 0x1c) // program counter sample regster - read-only
    #define DWT_COMP0                 *(volatile unsigned long *)(CORTEX_M4_DWT + 0x20) // compare register 0
    #define DWT_MASK0                 *(volatile unsigned long *)(CORTEX_M4_DWT + 0x24) // mask register 0
    #define DWT_FUNCTION0             *(volatile unsigned long *)(CORTEX_M4_DWT + 0x28) // function register 0
    #define DWT_COMP1                 *(volatile unsigned long *)(CORTEX_M4_DWT + 0x30) // compare register 1
    #define DWT_MASK1                 *(volatile unsigned long *)(CORTEX_M4_DWT + 0x34) // mask register 1
    #define DWT_FUNCTION1             *(volatile unsigned long *)(CORTEX_M4_DWT + 0x38) // function register 1
    #define DWT_COMP2                 *(volatile unsigned long *)(CORTEX_M4_DWT + 0x40) // compare register 2
    #define DWT_MASK2                 *(volatile unsigned long *)(CORTEX_M4_DWT + 0x44) // mask register 2
    #define DWT_FUNCTION2             *(volatile unsigned long *)(CORTEX_M4_DWT + 0x48) // function register 2
    #define DWT_COMP3                 *(volatile unsigned long *)(CORTEX_M4_DWT + 0x50) // compare register 3
    #define DWT_MASK3                 *(volatile unsigned long *)(CORTEX_M4_DWT + 0x54) // mask register 3
    #define DWT_FUNCTION3             *(volatile unsigned long *)(CORTEX_M4_DWT + 0x58) // function register 3
    #if defined ARM_MATH_CM7
        #define DWT_LAR               *(volatile unsigned long *)(CORTEX_M4_DWT + 0xfb0) // lock access register
            #define DWT_LAR_UNLOCK    0xc5acce55                         // unlock code
        #define DWT_LSR               *(volatile unsigned long *)(CORTEX_M4_DWT + 0xfb4) // lock access register - read-only
    #endif
    #define DWT_PID4                  *(volatile unsigned long *)(CORTEX_M4_DWT + 0xfd0) // peripheral identification register 4 - read-only
    #define DWT_PID5                  *(volatile unsigned long *)(CORTEX_M4_DWT + 0xfd4) // peripheral identification register 5 - read-only
    #define DWT_PID6                  *(volatile unsigned long *)(CORTEX_M4_DWT + 0xfd8) // peripheral identification register 6 - read-only
    #define DWT_PID7                  *(volatile unsigned long *)(CORTEX_M4_DWT + 0xfdc) // peripheral identification register 7 - read-only
    #define DWT_PID0                  *(volatile unsigned long *)(CORTEX_M4_DWT + 0xfe0) // peripheral identification register 0 - read-only
    #define DWT_PID1                  *(volatile unsigned long *)(CORTEX_M4_DWT + 0xfe4) // peripheral identification register 1 - read-only
    #define DWT_PID2                  *(volatile unsigned long *)(CORTEX_M4_DWT + 0xfe8) // peripheral identification register 2 - read-only
    #define DWT_PID3                  *(volatile unsigned long *)(CORTEX_M4_DWT + 0xfec) // peripheral identification register 3 - read-only
    #define DWT_CID0                  *(volatile unsigned long *)(CORTEX_M4_DWT + 0xff0) // component identification register 0 - read-only
    #define DWT_CID1                  *(volatile unsigned long *)(CORTEX_M4_DWT + 0xff4) // component identification register 1 - read-only
    #define DWT_CID2                  *(volatile unsigned long *)(CORTEX_M4_DWT + 0xff8) // component identification register 2 - read-only
    #define DWT_CID3                  *(volatile unsigned long *)(CORTEX_M4_DWT + 0xffc) // component identification register 3 - read-only
#endif

// Interrupt sources
//
#if defined _STM32L4X5 || defined _STM32L4X6
    #define irq_WindowsWatchdog_ID        0
    #define irq_PVD_ID                    1
    #define irq_RTC_TAMPER_ID             2
    #define irq_RTC_WKUP_ID               3
    #define irq_FLASH_ID                  4
    #define irq_RCC_ID                    5
    #define irq_EXTI0_ID                  6
    #define irq_EXTI1_ID                  7
    #define irq_EXTI2_ID                  8
    #define irq_EXTI3_ID                  9
    #define irq_EXTI4_ID                  10
    #define irq_DMA1_Channel1_ID          11
    #define irq_DMA1_Channel2_ID          12
    #define irq_DMA1_Channel3_ID          13
    #define irq_DMA1_Channel4_ID          14
    #define irq_DMA1_Channel5_ID          15
    #define irq_DMA1_Channel6_ID          16
    #define irq_DMA1_Channel7_ID          17
    #define irq_ADC1_2                    18
    #define irq_CAN1_TX                   19
    #define irq_CAN1_RX0                  20
    #define irq_CAN1_RX1                  21
    #define irq_CAN1_SCE                  22
    #define irq_EXTI9_5_ID                23
    #define irq_TIM1_BRK_TIM15_ID         24
    #define irq_TIM1_UP_TIM16_ID          25
    #define irq_TIM1_TRG_COM_TIM17_ID     26
    #define irq_TIM1_CC_ID                27
    #define irq_TIM2_ID                   28
    #define irq_TIM3_ID                   29
    #define irq_TIM4_ID                   30
    #define irq_I2C1_EV_ID                31
    #define irq_I2C1_ER_ID                32
    #define irq_I2C2_EV_ID                33
    #define irq_I2C2_ER_ID                34
    #define irq_SPI1_ID                   35
    #define irq_SPI2_ID                   36
    #define irq_USART1_ID                 37
    #define irq_USART2_ID                 38
    #define irq_USART3_ID                 39
    #define irq_EXTI15_10_ID              40
    #define irq_RTC_ALARM_ID              41
    #define irq_DFSDM1_FLT3_ID            42
    #define irq_TIM8_BRK_ID               43
    #define irq_TIM8_UP_ID                44
    #define irq_TIM8_TRG_COM_ID           45
    #define irq_TIM8_CC_ID                46
    #define irq_ADC3                      47
    #define irq_FCM                       48
    #define irq_SDMMC1                    49
    #define irq_TIM5_ID                   50
    #define irq_SPI3_ID                   51
    #define irq_UART4_ID                  52
    #define irq_UART5_ID                  53
    #define irq_TIM6_DACUNDER_ID          54
    #define irq_TIM7_ID                   55
    #define irq_DMA2_CH1_ID               56
    #define irq_DMA2_CH2_ID               57
    #define irq_DMA2_CH3_ID               58
    #define irq_DMA2_CH4_ID               59
    #define irq_DMA2_CH5_ID               60
    #define irq_DFSDM1_FLT0_ID            61
    #define irq_DFSDM1_FLT1_ID            62
    #define irq_DFSDM1_FLT2_ID            63
    #define irq_COMP_ID                   64
    #define irq_LPTIM1_ID                 65
    #define irq_LPTIM2_ID                 66
    #define irq_USB_FS_ID                 67
    #define irq_DMA2_CH6_ID               68
    #define irq_DMA2_CH7_ID               69
    #define irq_LPUART1_ID                70
    #define irq_QUADSPI_ID                71
    #define irq_I2C3_EV_ID                72
    #define irq_I2C3_ER_ID                73
    #define irq_SAI1_ID                   74
    #define irq_SAI2_ID                   75
    #define irq_SWPMI1_ID                 76
    #define irq_TSC_ID                    77
    #define irq_LCD_ID                    78
    #define irq_AES_ID                    79
    #define irq_RND_ID                    80
    #define irq_FPU_ID                    81
    #if defined _STM32L496 || defined _STM32L4A6
        #define irq_HASH_CRS_ID           82
        #define irq_I2C4_EV_ID            83
        #define irq_I2C4_ER_ID            84
        #define irq_DCMI_ID               85
        #define irq_CAN2_TX_ID            86
        #define irq_CAN2_RX0_ID           87
        #define irq_CAN2_RX1_ID           88
        #define irq_CAN2_SCE_ID           89
        #define irq_DMA2D_ID              90
    #endif
#elif defined _STM32L0x1 || defined _STM32F031
    #define irq_WindowsWatchdog_ID        0
    #define irq_PVD_ID                    1
    #define irq_RTC_ID                    2
    #define irq_FLASH_ID                  3
    #define irq_RCC_ID                    4
    #define irq_EXTI0_1_ID                5
    #define irq_EXTI2_3_ID                6
    #define irq_EXTI4_15_ID               7
    #if defined _STM32F031
        #define irq_TSC_ID                8
    #endif
    #define irq_DMA1_Channel1_ID          9
    #define irq_DMA1_Channel2_3_ID        10
    #define irq_DMA1_Channel4_7_ID        11
    #define irq_ADC_COMP_ID               12
    #define irq_LPTIM1_ID                 13
    #define irq_USART4_5_ID               14
    #define irq_TIM2_ID                   15
    #define irq_TIM3_ID                   16
    #define irq_TIM6_ID                   17
    #define irq_TIM7_ID                   18
    #if defined _STM32F031
        #define irq_TIM14_ID              19
    #endif
    #define irq_TIM21_ID                  20
    #define irq_I2C3_ID                   21
    #define irq_TIM22_ID                  22
    #define irq_I2C1_ID                   23
    #define irq_I2C2_ID                   24
    #define irq_SPI1_ID                   25
    #define irq_SPI2_ID                   26
    #define irq_USART1_ID                 27
    #define irq_USART2_ID                 28
    #define irq_LPUART1_AES_ID            29
    #if defined _STM32F031
        #define irq_CEC_CAN_ID            30
        #define irq_USB_ID                31
    #endif
#else
    #define irq_WindowsWatchdog_ID        0
    #define irq_PVD_ID                    1
    #define irq_Tamper_ID                 2
    #define irq_RTC_ID                    3
    #define irq_FLASH_ID                  4
    #define irq_RCC_ID                    5
    #define irq_EXTI0_ID                  6
    #define irq_EXTI1_ID                  7
    #define irq_EXTI2_ID                  8
    #define irq_EXTI3_ID                  9
    #define irq_EXTI4_ID                  10
    #define irq_DMA1_Channel1_ID          11
    #define irq_DMA1_Stream0_ID           11
    #define irq_DMA1_Channel2_ID          12
    #define irq_DMA1_Stream1_ID           12
    #define irq_DMA1_Channel3_ID          13
    #define irq_DMA1_Stream2_ID           13
    #define irq_DMA1_Channel4_ID          14
    #define irq_DMA1_Stream3_ID           14
    #define irq_DMA1_Channel5_ID          15
    #define irq_DMA1_Stream4_ID           15
    #define irq_DMA1_Channel6_ID          16
    #define irq_DMA1_Stream5_ID           16
    #define irq_DMA1_Channel7_ID          17
    #define irq_DMA1_Stream6_ID           17
    #define irq_ADC_ID                    18                             // ADC global interrupts
    #define irq_ADC1_2_ID                 18
    #define irq_CAN1_TX_ID                19
    #define irq_USB_HP_CAN_TX_ID          19
    #define irq_CAN1_RX0_ID               20
    #define irq_USB_LP_CAN_RX0_ID         20
    #define irq_CAN1_RX1_ID               21
    #define irq_CAN1_SCE_ID               22
    #define irq_EXTI9_5_ID                23
    #define irq_TIM1_BRK_TIM9_ID          24
    #define irq_TIM1_UP_TIM10_ID          25
    #define irq_TIM1_TRG_COM_TIM11_ID     26
    #define irq_TIM1_CC_ID                27
    #define irq_TIM2_ID                   28
    #define irq_TIM3_ID                   29
    #define irq_TIM4_ID                   30
    #define irq_I2C1_EV_ID                31
    #define irq_I2C1_ER_ID                32
    #define irq_I2C2_EV_ID                33
    #define irq_I2C2_ER_ID                34
    #define irq_SPI1_ID                   35
    #define irq_SPI2_ID                   36
    #define irq_USART1_ID                 37
    #define irq_USART2_ID                 38
    #define irq_USART3_ID                 39
    #define irq_EXTI15_10_ID              40
    #define irq_RTCAlarm_ID               41
    #define irq_OTG_FS_WKUP_ID            42
    #define irq_TIM8_BRK_TIM12_ID         43
    #define irq_TIM8_UP_TIM13_ID          44
    #define irq_TIM8_TRG_COM_TIM14_ID     45
    #define irq_TIM8_CC_ID                46
    #define irq_DMA1_Stream7_ID           47
    #define irq_FSMC_ID                   48
    #define irq_SDIO_ID                   49
    #define irq_TIM5_ID                   50
    #define irq_SPI3_ID                   51
    #define irq_UART4_ID                  52
    #define irq_UART5_ID                  53
    #define irq_TIM6_ID                   54
    #define irq_TIM6_DAC_ID               54
    #define irq_TIM7_ID                   55
    #define irq_DMA2_Channel1_ID          56
    #define irq_DMA2_Stream0_ID           56
    #define irq_DMA2_Channel2_ID          57
    #define irq_DMA2_Stream1_ID           57
    #define irq_DMA2_Channel3_ID          58
    #define irq_DMA2_Stream2_ID           58
    #define irq_DMA2_Channel4_ID          59
    #define irq_DMA2_Stream3_ID           59
    #define irq_DMA2_Channel5_ID          60
    #define irq_DMA2_Stream4_ID           60
    #define irq_ETH_ID                    61
    #define irq_ETH_WKUP_ID               62
    #define irq_CAN2_TX_ID                63
    #define irq_CAN2_RX0_ID               64
    #define irq_CAN2_RX1_ID               65
    #define irq_CAN2_SCE_ID               66
    #define irq_OTG_FS_ID                 67
    #define irq_DMA2_Stream5_ID           68
    #define irq_DMA2_Stream6_ID           69
    #define irq_DMA2_Stream7_ID           70
    #define irq_USART6_ID                 71
    #define irq_I2C3_EV_ID                72
    #define irq_I2C3_ER_ID                73                             // {9}
    #define irq_OTG_HS_EP1_OUT_ID         74
    #define irq_OTG_HS_EP1_IN_ID          75
    #define irq_OTG_HS_WKUP_ID            76
    #define irq_OTG_HS_ID                 77
    #define irq_DCMI_ID                   78
    #define irq_CRYP_ID                   79
    #define irq_HASH_RNG_ID               80
    #define irq_FPU_ID                    81
    #define irq_UART7_ID                  82
    #define irq_UART8_ID                  83
#endif



#define CHANGE_BIT_0                    0x004
#define CHANGE_BIT_1                    0x008
#define CHANGE_BIT_2                    0x010
#define CHANGE_BIT_3                    0x020
#define CHANGE_BIT_4                    0x040
#define CHANGE_BIT_5                    0x080
#define CHANGE_BIT_6                    0x100
#define CHANGE_BIT_7                    0x200


#define PORT_BIT0                       0x0001                           // general bit defines
#define PORT_BIT1                       0x0002
#define PORT_BIT2                       0x0004
#define PORT_BIT3                       0x0008
#define PORT_BIT4                       0x0010
#define PORT_BIT5                       0x0020
#define PORT_BIT6                       0x0040
#define PORT_BIT7                       0x0080
#define PORT_BIT8                       0x0100
#define PORT_BIT9                       0x0200
#define PORT_BIT10                      0x0400
#define PORT_BIT11                      0x0800
#define PORT_BIT12                      0x1000
#define PORT_BIT13                      0x2000
#define PORT_BIT14                      0x4000
#define PORT_BIT15                      0x8000

#define PORTA_BIT0                      0x0001                           // port specific bit defines (for stronger code relationship, but same effect)
#define PORTA_BIT1                      0x0002
#define PORTA_BIT2                      0x0004
#define PORTA_BIT3                      0x0008
#define PORTA_BIT4                      0x0010
#define PORTA_BIT5                      0x0020
#define PORTA_BIT6                      0x0040
#define PORTA_BIT7                      0x0080
#define PORTA_BIT8                      0x0100
#define PORTA_BIT9                      0x0200
#define PORTA_BIT10                     0x0400
#define PORTA_BIT11                     0x0800
#define PORTA_BIT12                     0x1000
#define PORTA_BIT13                     0x2000
#define PORTA_BIT14                     0x4000
#define PORTA_BIT15                     0x8000

#if defined _STM32F2XX || defined _STM32F4XX || defined _STM32F7XX
    #define ETH_MDIO_A_2                PORTA_BIT2
    #define MCO1_A_8                    PORTA_BIT8
    #define ETH_CRS_H_2                 PORTH_BIT2
    #define ETH_COL_H_3                 PORTH_BIT3
    #define ETH_TXEN_G_11               PORTG_BIT11
    #define ETH_TXD0_G_13               PORTG_BIT13
    #define ETH_TXD1_G_14               PORTG_BIT14
    #define ETH_TXEN_G_11               PORTG_BIT11
    #define ETH_RX_ER_I_10              PORTI_BIT10
    #define ETH_RX_DV_A_7               PORTA_BIT7
    #define ETH_RXD0_C_4                PORTC_BIT4
    #define ETH_RXD1_C_5                PORTC_BIT5
    #define ETH_RXD2_H_6                PORTH_BIT6
    #define ETH_RXD3_H_7                PORTH_BIT7

    #define ETH_CRS_A_0                 PORTA_BIT0                       // {7} alternative set of pins
    #define ETH_COL_A_3                 PORTA_BIT3
    #define ETH_TXD0_B_12               PORTB_BIT12
    #define ETH_TXD1_B_13               PORTB_BIT13
    #define ETH_RX_ER_B_10              PORTB_BIT10
    #define ETH_TXEN_B_11               PORTB_BIT11
    #define ETH_RXD2_B_0                PORTB_BIT0
    #define ETH_RXD3_B_1                PORTB_BIT1
    #define ETH_TXD3_E_2                PORTE_BIT2
    #define ETH_PPS_OUT_G_8             PORTG_BIT8
    #define ETH_PPS_OUT_B_5             PORTB_BIT5
#else
    #define ETH_MDIO_A_2                PORTA_BIT2                       // Ethernet MDIO alternative function on port A bit 2
    #define MCO_A_8                     PORTA_BIT8                       // microcontroller clock output alternative function on port A bit 8
    #define ETH_CRS_A_0                 PORTA_BIT0
    #define ETH_COL_A_3                 PORTA_BIT3
    #define ETH_TXD0_B_12               PORTB_BIT12
    #define ETH_TXD1_B_13               PORTB_BIT13
    #define ETH_TXEN_B_11               PORTB_BIT11
    #define ETH_RX_DV_D_8               PORTD_BIT8
    #define ETH_RXD0_D_10               PORTD_BIT10
    #define ETH_RXD1_D_9                PORTD_BIT9
    #define ETH_RXD2_D_11               PORTD_BIT11
    #define ETH_RXD3_D_12               PORTD_BIT12
#endif
#define ETH_RX_ER_B_10                  PORTB_BIT10
#define ETH_TXD2_C_2                    PORTC_BIT2
#define ETH_TXD3_B_8                    PORTB_BIT8
#define ETH_MDC_C_1                     PORTC_BIT1
#define ETH_RXCLK_A_1                   PORTA_BIT1
#define ETH_TXCLK_C_3                   PORTC_BIT3

#define SPI1_CLK_A_5                    PORTA_BIT5
#define SPI1_MOSI_B_5                   PORTB_BIT5
#define SPI1_MISO_A_6                   PORTA_BIT6
#define SPI1_MOSI_A_7                   PORTA_BIT7

#define SPI2_CLK_B_10                   PORTB_BIT10
#define SPI2_MOSI_B_15                  PORTB_BIT15
#define SPI2_MISO_B_14                  PORTA_BIT14

#define SPI3_CLK_B_3                    PORTB_BIT3
#define SPI3_MISO_B_4                   PORTB_BIT4
#define SPI3_MOSI_B_5                   PORTB_BIT5

#define PORTB_BIT0                      0x0001
#define PORTB_BIT1                      0x0002
#define PORTB_BIT2                      0x0004
#define PORTB_BIT3                      0x0008
#define PORTB_BIT4                      0x0010
#define PORTB_BIT5                      0x0020
#define PORTB_BIT6                      0x0040
#define PORTB_BIT7                      0x0080
#define PORTB_BIT8                      0x0100
#define PORTB_BIT9                      0x0200
#define PORTB_BIT10                     0x0400
#define PORTB_BIT11                     0x0800
#define PORTB_BIT12                     0x1000
#define PORTB_BIT13                     0x2000
#define PORTB_BIT14                     0x4000
#define PORTB_BIT15                     0x8000

#define PORTC_BIT0                      0x0001
#define PORTC_BIT1                      0x0002
#define PORTC_BIT2                      0x0004
#define PORTC_BIT3                      0x0008
#define PORTC_BIT4                      0x0010
#define PORTC_BIT5                      0x0020
#define PORTC_BIT6                      0x0040
#define PORTC_BIT7                      0x0080
#define PORTC_BIT8                      0x0100
#define PORTC_BIT9                      0x0200
#define PORTC_BIT10                     0x0400
#define PORTC_BIT11                     0x0800
#define PORTC_BIT12                     0x1000
#define PORTC_BIT13                     0x2000
#define PORTC_BIT14                     0x4000
#define PORTC_BIT15                     0x8000

#define PORTD_BIT0                      0x0001
#define PORTD_BIT1                      0x0002
#define PORTD_BIT2                      0x0004
#define PORTD_BIT3                      0x0008
#define PORTD_BIT4                      0x0010
#define PORTD_BIT5                      0x0020
#define PORTD_BIT6                      0x0040
#define PORTD_BIT7                      0x0080
#define PORTD_BIT8                      0x0100
#define PORTD_BIT9                      0x0200
#define PORTD_BIT10                     0x0400
#define PORTD_BIT11                     0x0800
#define PORTD_BIT12                     0x1000
#define PORTD_BIT13                     0x2000
#define PORTD_BIT14                     0x4000
#define PORTD_BIT15                     0x8000

#define PORTE_BIT0                      0x0001
#define PORTE_BIT1                      0x0002
#define PORTE_BIT2                      0x0004
#define PORTE_BIT3                      0x0008
#define PORTE_BIT4                      0x0010
#define PORTE_BIT5                      0x0020
#define PORTE_BIT6                      0x0040
#define PORTE_BIT7                      0x0080
#define PORTE_BIT8                      0x0100
#define PORTE_BIT9                      0x0200
#define PORTE_BIT10                     0x0400
#define PORTE_BIT11                     0x0800
#define PORTE_BIT12                     0x1000
#define PORTE_BIT13                     0x2000
#define PORTE_BIT14                     0x4000
#define PORTE_BIT15                     0x8000

#define PORTF_BIT0                      0x0001
#define PORTF_BIT1                      0x0002
#define PORTF_BIT2                      0x0004
#define PORTF_BIT3                      0x0008
#define PORTF_BIT4                      0x0010
#define PORTF_BIT5                      0x0020
#define PORTF_BIT6                      0x0040
#define PORTF_BIT7                      0x0080
#define PORTF_BIT8                      0x0100
#define PORTF_BIT9                      0x0200
#define PORTF_BIT10                     0x0400
#define PORTF_BIT11                     0x0800
#define PORTF_BIT12                     0x1000
#define PORTF_BIT13                     0x2000
#define PORTF_BIT14                     0x4000
#define PORTF_BIT15                     0x8000

#define PORTG_BIT0                      0x0001
#define PORTG_BIT1                      0x0002
#define PORTG_BIT2                      0x0004
#define PORTG_BIT3                      0x0008
#define PORTG_BIT4                      0x0010
#define PORTG_BIT5                      0x0020
#define PORTG_BIT6                      0x0040
#define PORTG_BIT7                      0x0080
#define PORTG_BIT8                      0x0100
#define PORTG_BIT9                      0x0200
#define PORTG_BIT10                     0x0400
#define PORTG_BIT11                     0x0800
#define PORTG_BIT12                     0x1000
#define PORTG_BIT13                     0x2000
#define PORTG_BIT14                     0x4000
#define PORTG_BIT15                     0x8000

#define PORTH_BIT0                      0x0001
#define PORTH_BIT1                      0x0002
#define PORTH_BIT2                      0x0004
#define PORTH_BIT3                      0x0008
#define PORTH_BIT4                      0x0010
#define PORTH_BIT5                      0x0020
#define PORTH_BIT6                      0x0040
#define PORTH_BIT7                      0x0080
#define PORTH_BIT8                      0x0100
#define PORTH_BIT9                      0x0200
#define PORTH_BIT10                     0x0400
#define PORTH_BIT11                     0x0800
#define PORTH_BIT12                     0x1000
#define PORTH_BIT13                     0x2000
#define PORTH_BIT14                     0x4000
#define PORTH_BIT15                     0x8000

#define PORTI_BIT0                      0x0001
#define PORTI_BIT1                      0x0002
#define PORTI_BIT2                      0x0004
#define PORTI_BIT3                      0x0008
#define PORTI_BIT4                      0x0010
#define PORTI_BIT5                      0x0020
#define PORTI_BIT6                      0x0040
#define PORTI_BIT7                      0x0080
#define PORTI_BIT8                      0x0100
#define PORTI_BIT9                      0x0200
#define PORTI_BIT10                     0x0400
#define PORTI_BIT11                     0x0800
#define PORTI_BIT12                     0x1000
#define PORTI_BIT13                     0x2000
#define PORTI_BIT14                     0x4000
#define PORTI_BIT15                     0x8000


#define PULLUP_BIT0                     (0x0001 << 16)
#define PULLUP_BIT1                     (0x0002 << 16)
#define PULLUP_BIT2                     (0x0004 << 16)
#define PULLUP_BIT3                     (0x0008 << 16)
#define PULLUP_BIT4                     (0x0010 << 16)
#define PULLUP_BIT5                     (0x0020 << 16)
#define PULLUP_BIT6                     (0x0040 << 16)
#define PULLUP_BIT7                     (0x0080 << 16)
#define PULLUP_BIT8                     (0x0100 << 16)
#define PULLUP_BIT9                     (0x0200 << 16)
#define PULLUP_BIT10                    (0x0400 << 16)
#define PULLUP_BIT11                    (0x0800 << 16)
#define PULLUP_BIT12                    (0x1000 << 16)
#define PULLUP_BIT13                    (0x2000 << 16)
#define PULLUP_BIT14                    (0x4000 << 16)
#define PULLUP_BIT15                    (0x8000 << 16)

#define PULLDOWN_BIT0                   0x0000
#define PULLDOWN_BIT1                   0x0000
#define PULLDOWN_BIT2                   0x0000
#define PULLDOWN_BIT3                   0x0000
#define PULLDOWN_BIT4                   0x0000
#define PULLDOWN_BIT5                   0x0000
#define PULLDOWN_BIT6                   0x0000
#define PULLDOWN_BIT7                   0x0000
#define PULLDOWN_BIT8                   0x0000
#define PULLDOWN_BIT9                   0x0000
#define PULLDOWN_BIT10                  0x0000
#define PULLDOWN_BIT11                  0x0000
#define PULLDOWN_BIT12                  0x0000
#define PULLDOWN_BIT13                  0x0000
#define PULLDOWN_BIT14                  0x0000
#define PULLDOWN_BIT15                  0x0000

#if defined _STM32F2XX || defined _STM32F4XX || defined _STM32F7XX
    #define __POWER_UP_GPIO(ref)    RCC_AHB1ENR |= (RCC_AHB1ENR_GPIO##ref##EN)
    #define __GPIO_IS_POWERED(ref)  (RCC_AHB1ENR & (RCC_AHB1ENR_GPIOAEN << ref))
    #define __GPIO_IS_IN_RESET(ref) (RCC_AHB1RSTR & (RCC_AHB1RSTR_GPIOARST << ref))
#elif defined _STM32L0x1
    #define __POWER_UP_GPIO(ref)    RCC_IOPENR |= (RCC_IOPENR_IOP##ref##EN)
    #define __GPIO_IS_POWERED(ref)  (RCC_IOPENR & (RCC_IOPENR_IOPAEN << ref))
    #define __GPIO_IS_IN_RESET(ref) (0)
#elif defined _STM32L432 || defined _STM32L4X5 || defined _STM32L4X6
    #define __POWER_UP_GPIO(ref)    RCC_AHB2ENR |= (RCC_AHB2ENR_GPIO##ref##EN)
    #define __GPIO_IS_POWERED(ref)  (RCC_AHB2ENR & (RCC_AHB2ENR_GPIOAEN << ref))
    #define __GPIO_IS_IN_RESET(ref) (RCC_AHB2RSTR & (RCC_AHB2RSTR_GPIOARST << ref))
#elif defined _STM32F031
    #define __POWER_UP_GPIO(ref)    RCC_AHBENR |= (RCC_AHBENR_IOP##ref##EN)
    #define __GPIO_IS_POWERED(ref)  (RCC_AHBENR & (RCC_AHBENR << ref))
    #define __GPIO_IS_IN_RESET(ref) (0)
#else
    #define __POWER_UP_GPIO(ref)    RCC_APB2ENR |= (RCC_APB2ENR_IOP##ref##EN)
    #define __GPIO_IS_POWERED(ref)  (RCC_APB2ENR & (RCC_APB2ENR << ref))
    #define __GPIO_IS_IN_RESET(ref) (RCC_APB2RSTR & (RCC_APB2ENR_IOPAEN << ref))
#endif


// Port macros
//
// Configure pins as output, including enabling power eg. _CONFIG_PORT_OUTPUT(D, (PORTD_BIT4), (OUTPUT_SLOW | OUTPUT_PUSH_PULL));
//
#if defined _STM32F2XX || defined _STM32F4XX || defined _STM32F7XX || defined _STM32L432 || defined _STM32L0x1 || defined _STM32F031 || defined _STM32L4X5 || defined _STM32L4X6
    // First the port is powered and each pin set as output, then the characteristics are set to the speed and type registers {8}
    //
    #define _CONFIG_PORT_OUTPUT(ref, pins, characteristics) __POWER_UP_GPIO(ref); \
    GPIO##ref##_MODER = ((GPIO##ref##_MODER & \
     ~((0x0001 & pins)        | ((0x0001 & pins) << 1)  | ((0x0002 & pins) << 1)  | ((0x0002 & pins) << 2)    | \
     (((0x0004 & pins) << 2)  | ((0x0004 & pins) << 3)  | ((0x0008 & pins) << 3)  | ((0x0008 & pins) << 4))   | \
     (((0x0010 & pins) << 4)  | ((0x0010 & pins) << 5)  | ((0x0020 & pins) << 5)  | ((0x0020 & pins) << 6))   | \
     (((0x0040 & pins) << 6)  | ((0x0040 & pins) << 7)  | ((0x0080 & pins) << 7)  | ((0x0080 & pins) << 8))   | \
     (((0x0100 & pins) << 8)  | ((0x0100 & pins) << 9)  | ((0x0200 & pins) << 9)  | ((0x0200 & pins) << 10))  | \
     (((0x0400 & pins) << 10) | ((0x0400 & pins) << 11) | ((0x0800 & pins) << 11) | ((0x0800 & pins) << 12))  | \
     (((0x1000 & pins) << 12) | ((0x1000 & pins) << 13) | ((0x2000 & pins) << 13) | ((0x2000 & pins) << 14))  | \
     (((0x4000 & pins) << 14) | ((0x4000 & pins) << 15) | ((0x8000 & pins) << 15) | ((0x8000 & pins) << 16))))| \
     (((0x0001 & pins)) | ((0x0002 & pins) << 1) | ((0x0004 & pins) << 2) | ((0x0008 & pins) << 3) | ((0x0010 & pins) << 4) | ((0x0020 & pins) << 5) | \
      ((0x0040 & pins) << 6) | ((0x0080 & pins) << 7) | ((0x0100 & pins) << 8) | ((0x0200 & pins) << 9) | ((0x0400 & pins) << 10)| ((0x0800 & pins) << 11)| \
      ((0x1000 & pins) << 12)| ((0x2000 & pins) << 13)| ((0x4000 & pins) << 14)| ((0x8000 & pins) << 15))); \
    GPIO##ref##_OSPEEDR = ((GPIO##ref##_OSPEEDR & \
     ~((0x0001 & pins)        | ((0x0001 & pins) << 1)  | ((0x0002 & pins) << 1)  | ((0x0002 & pins) << 2)    | \
     (((0x0004 & pins) << 2)  | ((0x0004 & pins) << 3)  | ((0x0008 & pins) << 3)  | ((0x0008 & pins) << 4))   | \
     (((0x0010 & pins) << 4)  | ((0x0010 & pins) << 5)  | ((0x0020 & pins) << 5)  | ((0x0020 & pins) << 6))   | \
     (((0x0040 & pins) << 6)  | ((0x0040 & pins) << 7)  | ((0x0080 & pins) << 7)  | ((0x0080 & pins) << 8))   | \
     (((0x0100 & pins) << 8)  | ((0x0100 & pins) << 9)  | ((0x0200 & pins) << 9)  | ((0x0200 & pins) << 10))  | \
     (((0x0400 & pins) << 10) | ((0x0400 & pins) << 11) | ((0x0800 & pins) << 11) | ((0x0800 & pins) << 12))  | \
     (((0x1000 & pins) << 12) | ((0x1000 & pins) << 13) | ((0x2000 & pins) << 13) | ((0x2000 & pins) << 14))  | \
     (((0x4000 & pins) << 14) | ((0x4000 & pins) << 15) | ((0x8000 & pins) << 15) | ((0x8000 & pins) << 16))))| \
     ((characteristics) & ((0x0001 & pins) | ((0x0001 & pins) << 1))) | \
     ((characteristics << 2)  & (((0x0002 & pins) << 1) | ((0x0002 & pins) << 2))) | \
     ((characteristics << 4)  & (((0x0004 & pins) << 2) | ((0x0004 & pins) << 3))) | \
     ((characteristics << 6)  & (((0x0008 & pins) << 3) | ((0x0008 & pins) << 4))) | \
     ((characteristics << 8)  & (((0x0010 & pins) << 4) | ((0x0010 & pins) << 5))) | \
     ((characteristics << 10) & (((0x0020 & pins) << 5) | ((0x0020 & pins) << 6))) | \
     ((characteristics << 12) & (((0x0040 & pins) << 6) | ((0x0040 & pins) << 7))) | \
     ((characteristics << 14) & (((0x0080 & pins) << 7) | ((0x0080 & pins) << 8))) | \
     ((characteristics << 16) & (((0x0100 & pins) << 8) | ((0x0100 & pins) << 9))) | \
     ((characteristics << 18) & (((0x0200 & pins) << 9) | ((0x0200 & pins) << 10)))| \
     ((characteristics << 20) & (((0x0400 & pins) << 10)| ((0x0400 & pins) << 11)))| \
     ((characteristics << 22) & (((0x0800 & pins) << 11)| ((0x0800 & pins) << 12)))| \
     ((characteristics << 24) & (((0x1000 & pins) << 12)| ((0x1000 & pins) << 13)))| \
     ((characteristics << 26) & (((0x2000 & pins) << 13)| ((0x2000 & pins) << 14)))| \
     ((characteristics << 28) & (((0x4000 & pins) << 14)| ((0x4000 & pins) << 15)))| \
     ((characteristics << 30) & (((0x8000 & pins) << 15)| ((0x8000 & pins) << 16)))); \
    GPIO##ref##_OTYPER = ((GPIO##ref##_OTYPER & ~(pins)) | \
    (((characteristics >> 2)  & (0x0001 & pins)) | \
     ((characteristics >> 1)  & (0x0002 & pins)) | \
     ((characteristics)       & (0x0004 & pins)) | \
     ((characteristics << 1)  & (0x0008 & pins)) | \
     ((characteristics << 2)  & (0x0010 & pins)) | \
     ((characteristics << 3)  & (0x0020 & pins)) | \
     ((characteristics << 4)  & (0x0040 & pins)) | \
     ((characteristics << 5)  & (0x0080 & pins)) | \
     ((characteristics << 6)  & (0x0100 & pins)) | \
     ((characteristics << 7)  & (0x0200 & pins)) | \
     ((characteristics << 8)  & (0x0400 & pins)) | \
     ((characteristics << 9)  & (0x0800 & pins)) | \
     ((characteristics << 10) & (0x1000 & pins)) | \
     ((characteristics << 11) & (0x2000 & pins)) | \
     ((characteristics << 12) & (0x4000 & pins)) | \
     ((characteristics << 13) & (0x8000 & pins))));   _SIM_PORT_CHANGE

  #define OUTPUT_SLOW                 (GPIO_OSPEEDR_2MHz)                // 2 MHz
  #define OUTPUT_MEDIUM               (GPIO_OSPEEDR_25MHz)               // 25 MHz
  #define OUTPUT_FAST                 (GPIO_OSPEEDR_50MHz)               // 50 MHz
  #define OUTPUT_ULTRA_FAST           (GPIO_OSPEEDR_100MHz)              // 100 MHz
  #define OUTPUT_PUSH_PULL            (GPIO_OTYPER_PUSH_PULL << 2)
  #define OUTPUT_OPEN_DRAIN           (GPIO_OTYPER_OPEN_DRAIN << 2)
  #define FLOATING_INPUT              (GPIO_PUPDR_NONE << 6)
  #define INPUT_PULL_UP               (GPIO_PUPDR_PULL_UP << 6)
  #define INPUT_PULL_DOWN             (GPIO_PUPDR_PULL_DOWN << 6)
#else
    // First the port is powered and enabled, then the peripheral function selection is masked, port out function set, inputs disabled and the output is finally driven (it drives the original port line state)
    //
    #define _CONFIG_PORT_OUTPUT(ref, pins, characteristics) __POWER_UP_GPIO(ref); \
     GPIO##ref##_CRL = ((GPIO##ref##_CRL & \
     ~((0x0001 & pins)        | ((0x0001 & pins) << 1)  | ((0x0001 & pins) << 2)  | ((0x0001 & pins) << 3)    | \
     (((0x0002 & pins) << 3)  | ((0x0002 & pins) << 4)  | ((0x0002 & pins) << 5)  | ((0x0002 & pins) << 6))   | \
     (((0x0004 & pins) << 6)  | ((0x0004 & pins) << 7)  | ((0x0004 & pins) << 8)  | ((0x0004 & pins) << 9))   | \
     (((0x0008 & pins) << 9)  | ((0x0008 & pins) << 10) | ((0x0008 & pins) << 11) | ((0x0008 & pins) << 12))  | \
     (((0x0010 & pins) << 12) | ((0x0010 & pins) << 13) | ((0x0010 & pins) << 14) | ((0x0010 & pins) << 15))  | \
     (((0x0020 & pins) << 15) | ((0x0020 & pins) << 16) | ((0x0020 & pins) << 17) | ((0x0020 & pins) << 18))  | \
     (((0x0040 & pins) << 18) | ((0x0040 & pins) << 19) | ((0x0040 & pins) << 20) | ((0x0040 & pins) << 21))  | \
     (((0x0080 & pins) << 21) | ((0x0080 & pins) << 22) | ((0x0080 & pins) << 23) | ((0x0080 & pins) << 24))))| \
      (((0x0001 & pins)       | ((0x0001 & pins) << 1)  | ((0x0001 & pins) << 2)  | ((0x0001 & pins) << 3))  & (characteristics)) | \
     ((((0x0002 & pins) << 3) | ((0x0002 & pins) << 4)  | ((0x0002 & pins) << 5)  | ((0x0002 & pins) << 6))  & ((characteristics) << 4)) | \
     ((((0x0004 & pins) << 6) | ((0x0004 & pins) << 7)  | ((0x0004 & pins) << 8)  | ((0x0004 & pins) << 9))  & ((characteristics) << 8)) | \
     ((((0x0008 & pins) << 9) | ((0x0008 & pins) << 10) | ((0x0008 & pins) << 11) | ((0x0008 & pins) << 12)) & ((characteristics) << 12)) | \
     ((((0x0010 & pins) << 12)| ((0x0010 & pins) << 13) | ((0x0010 & pins) << 14) | ((0x0010 & pins) << 15)) & ((characteristics) << 16)) | \
     ((((0x0020 & pins) << 15)| ((0x0020 & pins) << 16) | ((0x0020 & pins) << 17) | ((0x0020 & pins) << 18)) & ((characteristics) << 20)) | \
     ((((0x0040 & pins) << 18)| ((0x0040 & pins) << 19) | ((0x0040 & pins) << 20) | ((0x0040 & pins) << 21)) & ((characteristics) << 24)) | \
     ((((0x0080 & pins) << 21)| ((0x0080 & pins) << 22) | ((0x0080 & pins) << 23) | ((0x0080 & pins) << 24)) & ((characteristics) << 28))); \
     GPIO##ref##_CRH = ((GPIO##ref##_CRH & \
     ~(((0x0100 & pins) >> 8) | ((0x0100 & pins) >> 7)  | ((0x0100 & pins) >> 6)  | ((0x0100 & pins) >> 5)    | \
     (((0x0200 & pins) >> 5)  | ((0x0200 & pins) >> 4)  | ((0x0200 & pins) >> 3)  | ((0x0200 & pins) >> 2))   | \
     (((0x0400 & pins) >> 2)  | ((0x0400 & pins) >> 1)  | ((0x0400 & pins))       | ((0x0400 & pins) << 1))   | \
     (((0x0800 & pins) << 1)  | ((0x0800 & pins) << 2)  | ((0x0800 & pins) << 3)  | ((0x0800 & pins) << 4))   | \
     (((0x1000 & pins) << 4)  | ((0x1000 & pins) << 5)  | ((0x1000 & pins) << 6)  | ((0x1000 & pins) << 7))   | \
     (((0x2000 & pins) << 7)  | ((0x2000 & pins) << 8)  | ((0x2000 & pins) << 9)  | ((0x2000 & pins) << 10))  | \
     (((0x4000 & pins) << 10) | ((0x4000 & pins) << 11) | ((0x4000 & pins) << 12) | ((0x4000 & pins) << 13))  | \
     (((0x8000 & pins) << 13) | ((0x8000 & pins) << 14) | ((0x8000 & pins) << 15) | ((0x8000 & pins) << 16))))| \
     ((((0x0100 & pins) >> 8) | ((0x0100 & pins) >> 7)  | ((0x0100 & pins) >> 6)  | ((0x0100 & pins) >> 5))  & (characteristics)) | \
     ((((0x0200 & pins) >> 5) | ((0x0200 & pins) >> 4)  | ((0x0200 & pins) >> 3)  | ((0x0200 & pins) >> 2))  & ((characteristics) << 4)) | \
     ((((0x0400 & pins) >> 2) | ((0x0400 & pins) >> 1)  | ((0x0400 & pins))       | ((0x0400 & pins) << 1))  & ((characteristics) << 8)) | \
     ((((0x0800 & pins) << 1) | ((0x0800 & pins) << 2)  | ((0x0800 & pins) << 3)  | ((0x0800 & pins) << 4))  & ((characteristics) << 12)) | \
     ((((0x1000 & pins) << 4) | ((0x1000 & pins) << 5)  | ((0x1000 & pins) << 6)  | ((0x1000 & pins) << 7))  & ((characteristics) << 16)) | \
     ((((0x2000 & pins) << 7) | ((0x2000 & pins) << 8)  | ((0x2000 & pins) << 9)  | ((0x2000 & pins) << 10)) & ((characteristics) << 20)) | \
     ((((0x4000 & pins) << 10)| ((0x4000 & pins) << 11) | ((0x4000 & pins) << 12) | ((0x4000 & pins) << 13)) & ((characteristics) << 24)) | \
     ((((0x8000 & pins) << 13)| ((0x8000 & pins) << 14) | ((0x8000 & pins) << 15) | ((0x8000 & pins) << 16)) & ((characteristics) << 28))); _SIM_PORT_CHANGE

    #define OUTPUT_SLOW                 0x2                              // 2 MHz
    #define OUTPUT_MEDIUM               0x1                              // 10 MHz
    #define OUTPUT_FAST                 0x3                              // 50 MHz
    #define OUTPUT_ULTRA_FAST           OUTPUT_FAST                      // F1 doesn't have ultra fast so consider the same as fast
    #define OUTPUT_PUSH_PULL            0x0
    #define OUTPUT_OPEN_DRAIN           0x4
    #define ALTERNATIVE_FUNCTION        0x8
    #define FLOATING_INPUT              0x4
    #if defined _STM32F103X
        #define INPUT_PULL_UP           0xffff0008
        #define INPUT_PULL_DOWN         0x00000008
    #else
        #define INPUT_PULL_UP           0x8
        #define INPUT_PULL_DOWN         0x8
    #endif
#endif

// Configure pins as input, including enabling power and digital use. eg. _CONFIG_PORT_INPUT(A, PORTA_BIT4, FLOATING_INPUT);
//
#if defined _STM32F2XX || defined _STM32F4XX || defined _STM32F7XX || defined _STM32L432 || defined _STM32L0x1 || defined _STM32F031 || defined _STM32L4X5 || defined _STM32L4X6
    // First the port is powered and each pin set as input, then the characteristics are set to the pullup/down register
    //
    #define _CONFIG_PORT_INPUT(ref, pins, characteristics) __POWER_UP_GPIO(ref); \
    GPIO##ref##_MODER = ((GPIO##ref##_MODER & \
     ~((0x0001 & pins)        | ((0x0001 & pins) << 1)  | ((0x0002 & pins) << 1)  | ((0x0002 & pins) << 2)    | \
     (((0x0004 & pins) << 2)  | ((0x0004 & pins) << 3)  | ((0x0008 & pins) << 3)  | ((0x0008 & pins) << 4))   | \
     (((0x0010 & pins) << 4)  | ((0x0010 & pins) << 5)  | ((0x0020 & pins) << 5)  | ((0x0020 & pins) << 6))   | \
     (((0x0040 & pins) << 6)  | ((0x0040 & pins) << 7)  | ((0x0080 & pins) << 7)  | ((0x0080 & pins) << 8))   | \
     (((0x0100 & pins) << 8)  | ((0x0100 & pins) << 9)  | ((0x0200 & pins) << 9)  | ((0x0200 & pins) << 10))  | \
     (((0x0400 & pins) << 10) | ((0x0400 & pins) << 11) | ((0x0800 & pins) << 11) | ((0x0800 & pins) << 12))  | \
     (((0x1000 & pins) << 12) | ((0x1000 & pins) << 13) | ((0x2000 & pins) << 13) | ((0x2000 & pins) << 14))  | \
     (((0x4000 & pins) << 14) | ((0x4000 & pins) << 15) | ((0x8000 & pins) << 15) | ((0x8000 & pins) << 16))))); \
    GPIO##ref##_PUPDR = ((GPIO##ref##_PUPDR & \
     ~((0x0001 & pins)        | ((0x0001 & pins) << 1)  | ((0x0002 & pins) << 1)  | ((0x0002 & pins) << 2)    | \
     (((0x0004 & pins) << 2)  | ((0x0004 & pins) << 3)  | ((0x0008 & pins) << 3)  | ((0x0008 & pins) << 4))   | \
     (((0x0010 & pins) << 4)  | ((0x0010 & pins) << 5)  | ((0x0020 & pins) << 5)  | ((0x0020 & pins) << 6))   | \
     (((0x0040 & pins) << 6)  | ((0x0040 & pins) << 7)  | ((0x0080 & pins) << 7)  | ((0x0080 & pins) << 8))   | \
     (((0x0100 & pins) << 8)  | ((0x0100 & pins) << 9)  | ((0x0200 & pins) << 9)  | ((0x0200 & pins) << 10))  | \
     (((0x0400 & pins) << 10) | ((0x0400 & pins) << 11) | ((0x0800 & pins) << 11) | ((0x0800 & pins) << 12))  | \
     (((0x1000 & pins) << 12) | ((0x1000 & pins) << 13) | ((0x2000 & pins) << 13) | ((0x2000 & pins) << 14))  | \
     (((0x4000 & pins) << 14) | ((0x4000 & pins) << 15) | ((0x8000 & pins) << 15) | ((0x8000 & pins) << 16))))| \
     ((characteristics >> 6)  & ((0x0001 & pins) | ((0x0001 & pins) << 1))) | \
     ((characteristics >> 4)  & (((0x0002 & pins) << 1) | ((0x0002 & pins) << 2))) | \
     ((characteristics >> 2)  & (((0x0004 & pins) << 2) | ((0x0004 & pins) << 3))) | \
     ((characteristics)       & (((0x0008 & pins) << 3) | ((0x0008 & pins) << 4))) | \
     ((characteristics << 2)  & (((0x0010 & pins) << 4) | ((0x0010 & pins) << 5))) | \
     ((characteristics << 4)  & (((0x0020 & pins) << 5) | ((0x0020 & pins) << 6))) | \
     ((characteristics << 6)  & (((0x0040 & pins) << 6) | ((0x0040 & pins) << 7))) | \
     ((characteristics << 8)  & (((0x0080 & pins) << 7) | ((0x0080 & pins) << 8))) | \
     ((characteristics << 10) & (((0x0100 & pins) << 8) | ((0x0100 & pins) << 9))) | \
     ((characteristics << 12) & (((0x0200 & pins) << 9) | ((0x0200 & pins) << 10)))| \
     ((characteristics << 14) & (((0x0400 & pins) << 10)| ((0x0400 & pins) << 11)))| \
     ((characteristics << 16) & (((0x0800 & pins) << 11)| ((0x0800 & pins) << 12)))| \
     ((characteristics << 18) & (((0x1000 & pins) << 12)| ((0x1000 & pins) << 13)))| \
     ((characteristics << 20) & (((0x2000 & pins) << 13)| ((0x2000 & pins) << 14)))| \
     ((characteristics << 22) & (((0x4000 & pins) << 14)| ((0x4000 & pins) << 15)))| \
     ((characteristics << 24) & (((0x8000 & pins) << 15)| ((0x8000 & pins) << 16))));             _SIM_PORT_CHANGE // {17}
#elif defined _STM32F103X
    #define _CONFIG_PORT_INPUT(ref, pins, characteristics)  __POWER_UP_GPIO(ref); GPIO##ref##_ODR |= ((characteristics >> 16) & pins); GPIO##ref##_ODR &= ~((~characteristics >> 16) & pins); \
         GPIO##ref##_CRL = ((GPIO##ref##_CRL & \
         ~((0x0001 & pins)        | ((0x0001 & pins) << 1)  | ((0x0001 & pins) << 2)  | ((0x0001 & pins) << 3)    | \
         (((0x0002 & pins) << 3)  | ((0x0002 & pins) << 4)  | ((0x0002 & pins) << 5)  | ((0x0002 & pins) << 6))   | \
         (((0x0004 & pins) << 6)  | ((0x0004 & pins) << 7)  | ((0x0004 & pins) << 8)  | ((0x0004 & pins) << 9))   | \
         (((0x0008 & pins) << 9)  | ((0x0008 & pins) << 10) | ((0x0008 & pins) << 11) | ((0x0008 & pins) << 12))  | \
         (((0x0010 & pins) << 12) | ((0x0010 & pins) << 13) | ((0x0010 & pins) << 14) | ((0x0010 & pins) << 15))  | \
         (((0x0020 & pins) << 15) | ((0x0020 & pins) << 16) | ((0x0020 & pins) << 17) | ((0x0020 & pins) << 18))  | \
         (((0x0040 & pins) << 18) | ((0x0040 & pins) << 19) | ((0x0040 & pins) << 20) | ((0x0040 & pins) << 21))  | \
         (((0x0080 & pins) << 21) | ((0x0080 & pins) << 22) | ((0x0080 & pins) << 23) | ((0x0080 & pins) << 24))))| \
          (((0x0001 & pins)       | ((0x0001 & pins) << 1)  | ((0x0001 & pins) << 2)  | ((0x0001 & pins) << 3))  & (characteristics & 0x0f)) | \
         ((((0x0002 & pins) << 3) | ((0x0002 & pins) << 4)  | ((0x0002 & pins) << 5)  | ((0x0002 & pins) << 6))  & ((characteristics & 0x0f) << 4)) | \
         ((((0x0004 & pins) << 6) | ((0x0004 & pins) << 7)  | ((0x0004 & pins) << 8)  | ((0x0004 & pins) << 9))  & ((characteristics & 0x0f) << 8)) | \
         ((((0x0008 & pins) << 9) | ((0x0008 & pins) << 10) | ((0x0008 & pins) << 11) | ((0x0008 & pins) << 12)) & ((characteristics & 0x0f) << 12)) | \
         ((((0x0010 & pins) << 12)| ((0x0010 & pins) << 13) | ((0x0010 & pins) << 14) | ((0x0010 & pins) << 15)) & ((characteristics & 0x0f) << 16)) | \
         ((((0x0020 & pins) << 15)| ((0x0020 & pins) << 16) | ((0x0020 & pins) << 17) | ((0x0020 & pins) << 18)) & ((characteristics & 0x0f) << 20)) | \
         ((((0x0040 & pins) << 18)| ((0x0040 & pins) << 19) | ((0x0040 & pins) << 20) | ((0x0040 & pins) << 21)) & ((characteristics & 0x0f) << 24)) | \
         ((((0x0080 & pins) << 21)| ((0x0080 & pins) << 22) | ((0x0080 & pins) << 23) | ((0x0080 & pins) << 24)) & ((characteristics & 0x0f) << 28))); \
         GPIO##ref##_CRH = ((GPIO##ref##_CRH & \
         ~(((0x0100 & pins) >> 8) | ((0x0100 & pins) >> 7)  | ((0x0100 & pins) >> 6)  | ((0x0100 & pins) >> 5)    | \
         (((0x0200 & pins) >> 5)  | ((0x0200 & pins) >> 4)  | ((0x0200 & pins) >> 3)  | ((0x0200 & pins) >> 2))   | \
         (((0x0400 & pins) >> 2)  | ((0x0400 & pins) >> 1)  | ((0x0400 & pins))       | ((0x0400 & pins) << 1))   | \
         (((0x0800 & pins) << 1)  | ((0x0800 & pins) << 2)  | ((0x0800 & pins) << 3)  | ((0x0800 & pins) << 4))   | \
         (((0x1000 & pins) << 4)  | ((0x1000 & pins) << 5)  | ((0x1000 & pins) << 6)  | ((0x1000 & pins) << 7))   | \
         (((0x2000 & pins) << 7)  | ((0x2000 & pins) << 8)  | ((0x2000 & pins) << 9)  | ((0x2000 & pins) << 10))  | \
         (((0x4000 & pins) << 10) | ((0x4000 & pins) << 11) | ((0x4000 & pins) << 12) | ((0x4000 & pins) << 13))  | \
         (((0x8000 & pins) << 13) | ((0x8000 & pins) << 14) | ((0x8000 & pins) << 15) | ((0x8000 & pins) << 16))))| \
         ((((0x0100 & pins) >> 8) | ((0x0100 & pins) >> 7)  | ((0x0100 & pins) >> 6)  | ((0x0100 & pins) >> 5))  & (characteristics & 0x0f)) | \
         ((((0x0200 & pins) >> 5) | ((0x0200 & pins) >> 4)  | ((0x0200 & pins) >> 3)  | ((0x0200 & pins) >> 2))  & ((characteristics & 0x0f) << 4)) | \
         ((((0x0400 & pins) >> 2) | ((0x0400 & pins) >> 1)  | ((0x0400 & pins))       | ((0x0400 & pins) << 1))  & ((characteristics & 0x0f) << 8)) | \
         ((((0x0800 & pins) << 1) | ((0x0800 & pins) << 2)  | ((0x0800 & pins) << 3)  | ((0x0800 & pins) << 4))  & ((characteristics & 0x0f) << 12)) | \
         ((((0x1000 & pins) << 4) | ((0x1000 & pins) << 5)  | ((0x1000 & pins) << 6)  | ((0x1000 & pins) << 7))  & ((characteristics & 0x0f) << 16)) | \
         ((((0x2000 & pins) << 7) | ((0x2000 & pins) << 8)  | ((0x2000 & pins) << 9)  | ((0x2000 & pins) << 10)) & ((characteristics & 0x0f) << 20)) | \
         ((((0x4000 & pins) << 10)| ((0x4000 & pins) << 11) | ((0x4000 & pins) << 12) | ((0x4000 & pins) << 13)) & ((characteristics & 0x0f) << 24)) | \
         ((((0x8000 & pins) << 13)| ((0x8000 & pins) << 14) | ((0x8000 & pins) << 15) | ((0x8000 & pins) << 16)) & ((characteristics & 0x0f) << 28))); _SIM_PORT_CHANGE

#else
    #define _CONFIG_PORT_INPUT(ref, pins, characteristics)  __POWER_UP_GPIO(ref); GPIO##ref##_ODR |= (characteristics >> 16); \
         GPIO##ref##_CRL = ((GPIO##ref##_CRL & \
         ~((0x0001 & pins)        | ((0x0001 & pins) << 1)  | ((0x0001 & pins) << 2)  | ((0x0001 & pins) << 3)    | \
         (((0x0002 & pins) << 3)  | ((0x0002 & pins) << 4)  | ((0x0002 & pins) << 5)  | ((0x0002 & pins) << 6))   | \
         (((0x0004 & pins) << 6)  | ((0x0004 & pins) << 7)  | ((0x0004 & pins) << 8)  | ((0x0004 & pins) << 9))   | \
         (((0x0008 & pins) << 9)  | ((0x0008 & pins) << 10) | ((0x0008 & pins) << 11) | ((0x0008 & pins) << 12))  | \
         (((0x0010 & pins) << 12) | ((0x0010 & pins) << 13) | ((0x0010 & pins) << 14) | ((0x0010 & pins) << 15))  | \
         (((0x0020 & pins) << 15) | ((0x0020 & pins) << 16) | ((0x0020 & pins) << 17) | ((0x0020 & pins) << 18))  | \
         (((0x0040 & pins) << 18) | ((0x0040 & pins) << 19) | ((0x0040 & pins) << 20) | ((0x0040 & pins) << 21))  | \
         (((0x0080 & pins) << 21) | ((0x0080 & pins) << 22) | ((0x0080 & pins) << 23) | ((0x0080 & pins) << 24))))| \
          (((0x0001 & pins)       | ((0x0001 & pins) << 1)  | ((0x0001 & pins) << 2)  | ((0x0001 & pins) << 3))  & (characteristics & 0x0f)) | \
         ((((0x0002 & pins) << 3) | ((0x0002 & pins) << 4)  | ((0x0002 & pins) << 5)  | ((0x0002 & pins) << 6))  & ((characteristics & 0x0f) << 4)) | \
         ((((0x0004 & pins) << 6) | ((0x0004 & pins) << 7)  | ((0x0004 & pins) << 8)  | ((0x0004 & pins) << 9))  & ((characteristics & 0x0f) << 8)) | \
         ((((0x0008 & pins) << 9) | ((0x0008 & pins) << 10) | ((0x0008 & pins) << 11) | ((0x0008 & pins) << 12)) & ((characteristics & 0x0f) << 12)) | \
         ((((0x0010 & pins) << 12)| ((0x0010 & pins) << 13) | ((0x0010 & pins) << 14) | ((0x0010 & pins) << 15)) & ((characteristics & 0x0f) << 16)) | \
         ((((0x0020 & pins) << 15)| ((0x0020 & pins) << 16) | ((0x0020 & pins) << 17) | ((0x0020 & pins) << 18)) & ((characteristics & 0x0f) << 20)) | \
         ((((0x0040 & pins) << 18)| ((0x0040 & pins) << 19) | ((0x0040 & pins) << 20) | ((0x0040 & pins) << 21)) & ((characteristics & 0x0f) << 24)) | \
         ((((0x0080 & pins) << 21)| ((0x0080 & pins) << 22) | ((0x0080 & pins) << 23) | ((0x0080 & pins) << 24)) & ((characteristics & 0x0f) << 28))); \
         GPIO##ref##_CRH = ((GPIO##ref##_CRH & \
         ~(((0x0100 & pins) >> 8) | ((0x0100 & pins) >> 7)  | ((0x0100 & pins) >> 6)  | ((0x0100 & pins) >> 5)    | \
         (((0x0200 & pins) >> 5)  | ((0x0200 & pins) >> 4)  | ((0x0200 & pins) >> 3)  | ((0x0200 & pins) >> 2))   | \
         (((0x0400 & pins) >> 2)  | ((0x0400 & pins) >> 1)  | ((0x0400 & pins))       | ((0x0400 & pins) << 1))   | \
         (((0x0800 & pins) << 1)  | ((0x0800 & pins) << 2)  | ((0x0800 & pins) << 3)  | ((0x0800 & pins) << 4))   | \
         (((0x1000 & pins) << 4)  | ((0x1000 & pins) << 5)  | ((0x1000 & pins) << 6)  | ((0x1000 & pins) << 7))   | \
         (((0x2000 & pins) << 7)  | ((0x2000 & pins) << 8)  | ((0x2000 & pins) << 9)  | ((0x2000 & pins) << 10))  | \
         (((0x4000 & pins) << 10) | ((0x4000 & pins) << 11) | ((0x4000 & pins) << 12) | ((0x4000 & pins) << 13))  | \
         (((0x8000 & pins) << 13) | ((0x8000 & pins) << 14) | ((0x8000 & pins) << 15) | ((0x8000 & pins) << 16))))| \
         ((((0x0100 & pins) >> 8) | ((0x0100 & pins) >> 7)  | ((0x0100 & pins) >> 6)  | ((0x0100 & pins) >> 5))  & (characteristics & 0x0f)) | \
         ((((0x0200 & pins) >> 5) | ((0x0200 & pins) >> 4)  | ((0x0200 & pins) >> 3)  | ((0x0200 & pins) >> 2))  & ((characteristics & 0x0f) << 4)) | \
         ((((0x0400 & pins) >> 2) | ((0x0400 & pins) >> 1)  | ((0x0400 & pins))       | ((0x0400 & pins) << 1))  & ((characteristics & 0x0f) << 8)) | \
         ((((0x0800 & pins) << 1) | ((0x0800 & pins) << 2)  | ((0x0800 & pins) << 3)  | ((0x0800 & pins) << 4))  & ((characteristics & 0x0f) << 12)) | \
         ((((0x1000 & pins) << 4) | ((0x1000 & pins) << 5)  | ((0x1000 & pins) << 6)  | ((0x1000 & pins) << 7))  & ((characteristics & 0x0f) << 16)) | \
         ((((0x2000 & pins) << 7) | ((0x2000 & pins) << 8)  | ((0x2000 & pins) << 9)  | ((0x2000 & pins) << 10)) & ((characteristics & 0x0f) << 20)) | \
         ((((0x4000 & pins) << 10)| ((0x4000 & pins) << 11) | ((0x4000 & pins) << 12) | ((0x4000 & pins) << 13)) & ((characteristics & 0x0f) << 24)) | \
         ((((0x8000 & pins) << 13)| ((0x8000 & pins) << 14) | ((0x8000 & pins) << 15) | ((0x8000 & pins) << 16)) & ((characteristics & 0x0f) << 28))); _SIM_PORT_CHANGE
#endif

// Configure peripheral functions
//
#if defined _STM32F2XX || defined _STM32F4XX || defined _STM32F7XX || defined _STM32L432 || defined _STM32L0x1 || defined _STM32F031 || defined _STM32L4X5 || defined _STM32L4X6
    // Enable power to port, clear the pins to inputs and set alternative function and output characteristics, then set the specific function type {8}
    //
    #define _CONFIG_PERIPHERAL_OUTPUT(ref, per_func, pins, characteristics) __POWER_UP_GPIO(ref); \
    GPIO##ref##_MODER = ((GPIO##ref##_MODER & \
     ~((0x0001 & pins)        | ((0x0001 & pins) << 1)  | ((0x0002 & pins) << 1)  | ((0x0002 & pins) << 2)    | \
     (((0x0004 & pins) << 2)  | ((0x0004 & pins) << 3)  | ((0x0008 & pins) << 3)  | ((0x0008 & pins) << 4))   | \
     (((0x0010 & pins) << 4)  | ((0x0010 & pins) << 5)  | ((0x0020 & pins) << 5)  | ((0x0020 & pins) << 6))   | \
     (((0x0040 & pins) << 6)  | ((0x0040 & pins) << 7)  | ((0x0080 & pins) << 7)  | ((0x0080 & pins) << 8))   | \
     (((0x0100 & pins) << 8)  | ((0x0100 & pins) << 9)  | ((0x0200 & pins) << 9)  | ((0x0200 & pins) << 10))  | \
     (((0x0400 & pins) << 10) | ((0x0400 & pins) << 11) | ((0x0800 & pins) << 11) | ((0x0800 & pins) << 12))  | \
     (((0x1000 & pins) << 12) | ((0x1000 & pins) << 13) | ((0x2000 & pins) << 13) | ((0x2000 & pins) << 14))  | \
     (((0x4000 & pins) << 14) | ((0x4000 & pins) << 15) | ((0x8000 & pins) << 15) | ((0x8000 & pins) << 16))))| \
     (((0x0001 & pins) << 1) | ((0x0002 & pins) << 2) | ((0x0004 & pins) << 3) | ((0x0008 & pins) << 4) | ((0x0010 & pins) << 5) | ((0x0020 & pins) << 6) | \
      ((0x0040 & pins) << 7) | ((0x0080 & pins) << 8) | ((0x0100 & pins) << 9) | ((0x0200 & pins) << 10)| ((0x0400 & pins) << 11)| ((0x0800 & pins) << 12)| \
      ((0x1000 & pins) << 13)| ((0x2000 & pins) << 14)| ((0x4000 & pins) << 15)| ((0x8000 & pins) << 16))); \
    GPIO##ref##_OSPEEDR = ((GPIO##ref##_OSPEEDR & \
     ~((0x0001 & pins)        | ((0x0001 & pins) << 1)  | ((0x0002 & pins) << 1)  | ((0x0002 & pins) << 2)    | \
     (((0x0004 & pins) << 2)  | ((0x0004 & pins) << 3)  | ((0x0008 & pins) << 3)  | ((0x0008 & pins) << 4))   | \
     (((0x0010 & pins) << 4)  | ((0x0010 & pins) << 5)  | ((0x0020 & pins) << 5)  | ((0x0020 & pins) << 6))   | \
     (((0x0040 & pins) << 6)  | ((0x0040 & pins) << 7)  | ((0x0080 & pins) << 7)  | ((0x0080 & pins) << 8))   | \
     (((0x0100 & pins) << 8)  | ((0x0100 & pins) << 9)  | ((0x0200 & pins) << 9)  | ((0x0200 & pins) << 10))  | \
     (((0x0400 & pins) << 10) | ((0x0400 & pins) << 11) | ((0x0800 & pins) << 11) | ((0x0800 & pins) << 12))  | \
     (((0x1000 & pins) << 12) | ((0x1000 & pins) << 13) | ((0x2000 & pins) << 13) | ((0x2000 & pins) << 14))  | \
     (((0x4000 & pins) << 14) | ((0x4000 & pins) << 15) | ((0x8000 & pins) << 15) | ((0x8000 & pins) << 16))))| \
     ((characteristics) & ((0x0001 & pins) | ((0x0001 & pins) << 1))) | \
     ((characteristics << 2)  & (((0x0002 & pins) << 1) | ((0x0002 & pins) << 2))) | \
     ((characteristics << 4)  & (((0x0004 & pins) << 2) | ((0x0004 & pins) << 3))) | \
     ((characteristics << 6)  & (((0x0008 & pins) << 3) | ((0x0008 & pins) << 4))) | \
     ((characteristics << 8)  & (((0x0010 & pins) << 4) | ((0x0010 & pins) << 5))) | \
     ((characteristics << 10) & (((0x0020 & pins) << 5) | ((0x0020 & pins) << 6))) | \
     ((characteristics << 12) & (((0x0040 & pins) << 6) | ((0x0040 & pins) << 7))) | \
     ((characteristics << 14) & (((0x0080 & pins) << 7) | ((0x0080 & pins) << 8))) | \
     ((characteristics << 16) & (((0x0100 & pins) << 8) | ((0x0100 & pins) << 9))) | \
     ((characteristics << 18) & (((0x0200 & pins) << 9) | ((0x0200 & pins) << 10)))| \
     ((characteristics << 20) & (((0x0400 & pins) << 10)| ((0x0400 & pins) << 11)))| \
     ((characteristics << 22) & (((0x0800 & pins) << 11)| ((0x0800 & pins) << 12)))| \
     ((characteristics << 24) & (((0x1000 & pins) << 12)| ((0x1000 & pins) << 13)))| \
     ((characteristics << 26) & (((0x2000 & pins) << 13)| ((0x2000 & pins) << 14)))| \
     ((characteristics << 28) & (((0x4000 & pins) << 14)| ((0x4000 & pins) << 15)))| \
     ((characteristics << 30) & (((0x8000 & pins) << 15)| ((0x8000 & pins) << 16)))); \
    GPIO##ref##_OTYPER = ((GPIO##ref##_OTYPER & ~(pins)) | \
    (((characteristics >> 2)  & (0x0001 & pins)) | \
     ((characteristics >> 1)  & (0x0002 & pins)) | \
     ((characteristics)       & (0x0004 & pins)) | \
     ((characteristics << 1)  & (0x0008 & pins)) | \
     ((characteristics << 2)  & (0x0010 & pins)) | \
     ((characteristics << 3)  & (0x0020 & pins)) | \
     ((characteristics << 4)  & (0x0040 & pins)) | \
     ((characteristics << 5)  & (0x0080 & pins)) | \
     ((characteristics << 6)  & (0x0100 & pins)) | \
     ((characteristics << 7)  & (0x0200 & pins)) | \
     ((characteristics << 8)  & (0x0400 & pins)) | \
     ((characteristics << 9)  & (0x0800 & pins)) | \
     ((characteristics << 10) & (0x1000 & pins)) | \
     ((characteristics << 11) & (0x2000 & pins)) | \
     ((characteristics << 12) & (0x4000 & pins)) | \
     ((characteristics << 13) & (0x8000 & pins))));  \
    GPIO##ref##_PUPDR = ((GPIO##ref##_PUPDR & \
     ~((0x0001 & pins)        | ((0x0001 & pins) << 1)  | ((0x0002 & pins) << 1)  | ((0x0002 & pins) << 2)    | \
     (((0x0004 & pins) << 2)  | ((0x0004 & pins) << 3)  | ((0x0008 & pins) << 3)  | ((0x0008 & pins) << 4))   | \
     (((0x0010 & pins) << 4)  | ((0x0010 & pins) << 5)  | ((0x0020 & pins) << 5)  | ((0x0020 & pins) << 6))   | \
     (((0x0040 & pins) << 6)  | ((0x0040 & pins) << 7)  | ((0x0080 & pins) << 7)  | ((0x0080 & pins) << 8))   | \
     (((0x0100 & pins) << 8)  | ((0x0100 & pins) << 9)  | ((0x0200 & pins) << 9)  | ((0x0200 & pins) << 10))  | \
     (((0x0400 & pins) << 10) | ((0x0400 & pins) << 11) | ((0x0800 & pins) << 11) | ((0x0800 & pins) << 12))  | \
     (((0x1000 & pins) << 12) | ((0x1000 & pins) << 13) | ((0x2000 & pins) << 13) | ((0x2000 & pins) << 14))  | \
     (((0x4000 & pins) << 14) | ((0x4000 & pins) << 15) | ((0x8000 & pins) << 15) | ((0x8000 & pins) << 16))))| \
     ((characteristics >> 6)  & ((0x0001 & pins) | ((0x0001 & pins) << 1))) | \
     ((characteristics >> 4)  & (((0x0002 & pins) << 1) | ((0x0002 & pins) << 2))) | \
     ((characteristics >> 2)  & (((0x0004 & pins) << 2) | ((0x0004 & pins) << 3))) | \
     ((characteristics)       & (((0x0008 & pins) << 3) | ((0x0008 & pins) << 4))) | \
     ((characteristics << 2)  & (((0x0010 & pins) << 4) | ((0x0010 & pins) << 5))) | \
     ((characteristics << 4)  & (((0x0020 & pins) << 5) | ((0x0020 & pins) << 6))) | \
     ((characteristics << 6)  & (((0x0040 & pins) << 6) | ((0x0040 & pins) << 7))) | \
     ((characteristics << 8)  & (((0x0080 & pins) << 7) | ((0x0080 & pins) << 8))) | \
     ((characteristics << 10) & (((0x0100 & pins) << 8) | ((0x0100 & pins) << 9))) | \
     ((characteristics << 12) & (((0x0200 & pins) << 9) | ((0x0200 & pins) << 10)))| \
     ((characteristics << 14) & (((0x0400 & pins) << 10)| ((0x0400 & pins) << 11)))| \
     ((characteristics << 16) & (((0x0800 & pins) << 11)| ((0x0800 & pins) << 12)))| \
     ((characteristics << 28) & (((0x1000 & pins) << 12)| ((0x1000 & pins) << 13)))| \
     ((characteristics << 20) & (((0x2000 & pins) << 13)| ((0x2000 & pins) << 14)))| \
     ((characteristics << 22) & (((0x4000 & pins) << 14)| ((0x4000 & pins) << 15)))| \
     ((characteristics << 24) & (((0x8000 & pins) << 15)| ((0x8000 & pins) << 16)))); \
     GPIO##ref##_AFRL = ((GPIO##ref##_AFRL & \
     ~((0x0001 & pins)        | ((0x0001 & pins) << 1)  | ((0x0001 & pins) << 2)  | ((0x0001 & pins) << 3)    | \
     (((0x0002 & pins) << 3)  | ((0x0002 & pins) << 4)  | ((0x0002 & pins) << 5)  | ((0x0002 & pins) << 6))   | \
     (((0x0004 & pins) << 6)  | ((0x0004 & pins) << 7)  | ((0x0004 & pins) << 8)  | ((0x0004 & pins) << 9))   | \
     (((0x0008 & pins) << 9)  | ((0x0008 & pins) << 10) | ((0x0008 & pins) << 11) | ((0x0008 & pins) << 12))  | \
     (((0x0010 & pins) << 12) | ((0x0010 & pins) << 13) | ((0x0010 & pins) << 14) | ((0x0010 & pins) << 15))  | \
     (((0x0020 & pins) << 15) | ((0x0020 & pins) << 16) | ((0x0020 & pins) << 17) | ((0x0020 & pins) << 18))  | \
     (((0x0040 & pins) << 18) | ((0x0040 & pins) << 19) | ((0x0040 & pins) << 20) | ((0x0040 & pins) << 21))  | \
     (((0x0080 & pins) << 21) | ((0x0080 & pins) << 22) | ((0x0080 & pins) << 23) | ((0x0080 & pins) << 24))))| \
      (((0x0001 & pins)       | ((0x0001 & pins) << 1)  | ((0x0001 & pins) << 2)  | ((0x0001 & pins) << 3))  & (per_func)) | \
     ((((0x0002 & pins) << 3) | ((0x0002 & pins) << 4)  | ((0x0002 & pins) << 5)  | ((0x0002 & pins) << 6))  & ((per_func) << 4)) | \
     ((((0x0004 & pins) << 6) | ((0x0004 & pins) << 7)  | ((0x0004 & pins) << 8)  | ((0x0004 & pins) << 9))  & ((per_func) << 8)) | \
     ((((0x0008 & pins) << 9) | ((0x0008 & pins) << 10) | ((0x0008 & pins) << 11) | ((0x0008 & pins) << 12)) & ((per_func) << 12)) | \
     ((((0x0010 & pins) << 12)| ((0x0010 & pins) << 13) | ((0x0010 & pins) << 14) | ((0x0010 & pins) << 15)) & ((per_func) << 16)) | \
     ((((0x0020 & pins) << 15)| ((0x0020 & pins) << 16) | ((0x0020 & pins) << 17) | ((0x0020 & pins) << 18)) & ((per_func) << 20)) | \
     ((((0x0040 & pins) << 18)| ((0x0040 & pins) << 19) | ((0x0040 & pins) << 20) | ((0x0040 & pins) << 21)) & ((per_func) << 24)) | \
     ((((0x0080 & pins) << 21)| ((0x0080 & pins) << 22) | ((0x0080 & pins) << 23) | ((0x0080 & pins) << 24)) & ((per_func) << 28))); \
     GPIO##ref##_AFRH = ((GPIO##ref##_AFRH & \
     ~(((0x0100 & pins) >> 8) | ((0x0100 & pins) >> 7)  | ((0x0100 & pins) >> 6)  | ((0x0100 & pins) >> 5)    | \
     (((0x0200 & pins) >> 5)  | ((0x0200 & pins) >> 4)  | ((0x0200 & pins) >> 3)  | ((0x0200 & pins) >> 2))   | \
     (((0x0400 & pins) >> 2)  | ((0x0400 & pins) >> 1)  | ((0x0400 & pins))       | ((0x0400 & pins) << 1))   | \
     (((0x0800 & pins) << 1)  | ((0x0800 & pins) << 2)  | ((0x0800 & pins) << 3)  | ((0x0800 & pins) << 4))   | \
     (((0x1000 & pins) << 4)  | ((0x1000 & pins) << 5)  | ((0x1000 & pins) << 6)  | ((0x1000 & pins) << 7))   | \
     (((0x2000 & pins) << 7)  | ((0x2000 & pins) << 8)  | ((0x2000 & pins) << 9)  | ((0x2000 & pins) << 10))  | \
     (((0x4000 & pins) << 10) | ((0x4000 & pins) << 11) | ((0x4000 & pins) << 12) | ((0x4000 & pins) << 13))  | \
     (((0x8000 & pins) << 13) | ((0x8000 & pins) << 14) | ((0x8000 & pins) << 15) | ((0x8000 & pins) << 16))))| \
     ((((0x0100 & pins) >> 8) | ((0x0100 & pins) >> 7)  | ((0x0100 & pins) >> 6)  | ((0x0100 & pins) >> 5))  & (per_func)) | \
     ((((0x0200 & pins) >> 5) | ((0x0200 & pins) >> 4)  | ((0x0200 & pins) >> 3)  | ((0x0200 & pins) >> 2))  & ((per_func) << 4)) | \
     ((((0x0400 & pins) >> 2) | ((0x0400 & pins) >> 1)  | ((0x0400 & pins))       | ((0x0400 & pins) << 1))  & ((per_func) << 8)) | \
     ((((0x0800 & pins) << 1) | ((0x0800 & pins) << 2)  | ((0x0800 & pins) << 3)  | ((0x0800 & pins) << 4))  & ((per_func) << 12)) | \
     ((((0x1000 & pins) << 4) | ((0x1000 & pins) << 5)  | ((0x1000 & pins) << 6)  | ((0x1000 & pins) << 7))  & ((per_func) << 16)) | \
     ((((0x2000 & pins) << 7) | ((0x2000 & pins) << 8)  | ((0x2000 & pins) << 9)  | ((0x2000 & pins) << 10)) & ((per_func) << 20)) | \
     ((((0x4000 & pins) << 10)| ((0x4000 & pins) << 11) | ((0x4000 & pins) << 12) | ((0x4000 & pins) << 13)) & ((per_func) << 24)) | \
     ((((0x8000 & pins) << 13)| ((0x8000 & pins) << 14) | ((0x8000 & pins) << 15) | ((0x8000 & pins) << 16)) & ((per_func) << 28))); _SIM_PORT_CHANGE

    // Enable power to port, clear the pins' to inputs and set alternative function and output characteristics, then set the specific function type
    //
    #define _CONFIG_PERIPHERAL_INPUT(ref, per_func, pins, characteristics) __POWER_UP_GPIO(ref); \
    GPIO##ref##_MODER = ((GPIO##ref##_MODER & \
     ~((0x0001 & pins)        | ((0x0001 & pins) << 1)  | ((0x0002 & pins) << 1)  | ((0x0002 & pins) << 2)    | \
     (((0x0004 & pins) << 2)  | ((0x0004 & pins) << 3)  | ((0x0008 & pins) << 3)  | ((0x0008 & pins) << 4))   | \
     (((0x0010 & pins) << 4)  | ((0x0010 & pins) << 5)  | ((0x0020 & pins) << 5)  | ((0x0020 & pins) << 6))   | \
     (((0x0040 & pins) << 6)  | ((0x0040 & pins) << 7)  | ((0x0080 & pins) << 7)  | ((0x0080 & pins) << 8))   | \
     (((0x0100 & pins) << 8)  | ((0x0100 & pins) << 9)  | ((0x0200 & pins) << 9)  | ((0x0200 & pins) << 10))  | \
     (((0x0400 & pins) << 10) | ((0x0400 & pins) << 11) | ((0x0800 & pins) << 11) | ((0x0800 & pins) << 12))  | \
     (((0x1000 & pins) << 12) | ((0x1000 & pins) << 13) | ((0x2000 & pins) << 13) | ((0x2000 & pins) << 14))  | \
     (((0x4000 & pins) << 14) | ((0x4000 & pins) << 15) | ((0x8000 & pins) << 15) | ((0x8000 & pins) << 16))))| \
     (((0x0001 & pins) << 1) | ((0x0002 & pins) << 2) | ((0x0004 & pins) << 3) | ((0x0008 & pins) << 4) | ((0x0010 & pins) << 5) | ((0x0020 & pins) << 6) | \
      ((0x0040 & pins) << 7) | ((0x0080 & pins) << 8) | ((0x0100 & pins) << 9) | ((0x0200 & pins) << 10)| ((0x0400 & pins) << 11)| ((0x0800 & pins) << 12)| \
      ((0x1000 & pins) << 13)| ((0x2000 & pins) << 14)| ((0x4000 & pins) << 15)| ((0x8000 & pins) << 16))); \
    GPIO##ref##_PUPDR = ((GPIO##ref##_PUPDR & \
     ~((0x0001 & pins)        | ((0x0001 & pins) << 1)  | ((0x0002 & pins) << 1)  | ((0x0002 & pins) << 2)    | \
     (((0x0004 & pins) << 2)  | ((0x0004 & pins) << 3)  | ((0x0008 & pins) << 3)  | ((0x0008 & pins) << 4))   | \
     (((0x0010 & pins) << 4)  | ((0x0010 & pins) << 5)  | ((0x0020 & pins) << 5)  | ((0x0020 & pins) << 6))   | \
     (((0x0040 & pins) << 6)  | ((0x0040 & pins) << 7)  | ((0x0080 & pins) << 7)  | ((0x0080 & pins) << 8))   | \
     (((0x0100 & pins) << 8)  | ((0x0100 & pins) << 9)  | ((0x0200 & pins) << 9)  | ((0x0200 & pins) << 10))  | \
     (((0x0400 & pins) << 10) | ((0x0400 & pins) << 11) | ((0x0800 & pins) << 11) | ((0x0800 & pins) << 12))  | \
     (((0x1000 & pins) << 12) | ((0x1000 & pins) << 13) | ((0x2000 & pins) << 13) | ((0x2000 & pins) << 14))  | \
     (((0x4000 & pins) << 14) | ((0x4000 & pins) << 15) | ((0x8000 & pins) << 15) | ((0x8000 & pins) << 16))))| \
     ((characteristics >> 6)  & ((0x0001 & pins) | ((0x0001 & pins) << 1))) | \
     ((characteristics >> 4)  & (((0x0002 & pins) << 1) | ((0x0002 & pins) << 2))) | \
     ((characteristics >> 2)  & (((0x0004 & pins) << 2) | ((0x0004 & pins) << 3))) | \
     ((characteristics)       & (((0x0008 & pins) << 3) | ((0x0008 & pins) << 4))) | \
     ((characteristics << 2)  & (((0x0010 & pins) << 4) | ((0x0010 & pins) << 5))) | \
     ((characteristics << 4)  & (((0x0020 & pins) << 5) | ((0x0020 & pins) << 6))) | \
     ((characteristics << 6)  & (((0x0040 & pins) << 6) | ((0x0040 & pins) << 7))) | \
     ((characteristics << 8)  & (((0x0080 & pins) << 7) | ((0x0080 & pins) << 8))) | \
     ((characteristics << 10) & (((0x0100 & pins) << 8) | ((0x0100 & pins) << 9))) | \
     ((characteristics << 12) & (((0x0200 & pins) << 9) | ((0x0200 & pins) << 10)))| \
     ((characteristics << 14) & (((0x0400 & pins) << 10)| ((0x0400 & pins) << 11)))| \
     ((characteristics << 16) & (((0x0800 & pins) << 11)| ((0x0800 & pins) << 12)))| \
     ((characteristics << 28) & (((0x1000 & pins) << 12)| ((0x1000 & pins) << 13)))| \
     ((characteristics << 20) & (((0x2000 & pins) << 13)| ((0x2000 & pins) << 14)))| \
     ((characteristics << 22) & (((0x4000 & pins) << 14)| ((0x4000 & pins) << 15)))| \
     ((characteristics << 24) & (((0x8000 & pins) << 15)| ((0x8000 & pins) << 16)))); \
     GPIO##ref##_AFRL = ((GPIO##ref##_AFRL & \
     ~((0x0001 & pins)        | ((0x0001 & pins) << 1)  | ((0x0001 & pins) << 2)  | ((0x0001 & pins) << 3)    | \
     (((0x0002 & pins) << 3)  | ((0x0002 & pins) << 4)  | ((0x0002 & pins) << 5)  | ((0x0002 & pins) << 6))   | \
     (((0x0004 & pins) << 6)  | ((0x0004 & pins) << 7)  | ((0x0004 & pins) << 8)  | ((0x0004 & pins) << 9))   | \
     (((0x0008 & pins) << 9)  | ((0x0008 & pins) << 10) | ((0x0008 & pins) << 11) | ((0x0008 & pins) << 12))  | \
     (((0x0010 & pins) << 12) | ((0x0010 & pins) << 13) | ((0x0010 & pins) << 14) | ((0x0010 & pins) << 15))  | \
     (((0x0020 & pins) << 15) | ((0x0020 & pins) << 16) | ((0x0020 & pins) << 17) | ((0x0020 & pins) << 18))  | \
     (((0x0040 & pins) << 18) | ((0x0040 & pins) << 19) | ((0x0040 & pins) << 20) | ((0x0040 & pins) << 21))  | \
     (((0x0080 & pins) << 21) | ((0x0080 & pins) << 22) | ((0x0080 & pins) << 23) | ((0x0080 & pins) << 24))))| \
      (((0x0001 & pins)       | ((0x0001 & pins) << 1)  | ((0x0001 & pins) << 2)  | ((0x0001 & pins) << 3))  & (per_func)) | \
     ((((0x0002 & pins) << 3) | ((0x0002 & pins) << 4)  | ((0x0002 & pins) << 5)  | ((0x0002 & pins) << 6))  & ((per_func) << 4)) | \
     ((((0x0004 & pins) << 6) | ((0x0004 & pins) << 7)  | ((0x0004 & pins) << 8)  | ((0x0004 & pins) << 9))  & ((per_func) << 8)) | \
     ((((0x0008 & pins) << 9) | ((0x0008 & pins) << 10) | ((0x0008 & pins) << 11) | ((0x0008 & pins) << 12)) & ((per_func) << 12)) | \
     ((((0x0010 & pins) << 12)| ((0x0010 & pins) << 13) | ((0x0010 & pins) << 14) | ((0x0010 & pins) << 15)) & ((per_func) << 16)) | \
     ((((0x0020 & pins) << 15)| ((0x0020 & pins) << 16) | ((0x0020 & pins) << 17) | ((0x0020 & pins) << 18)) & ((per_func) << 20)) | \
     ((((0x0040 & pins) << 18)| ((0x0040 & pins) << 19) | ((0x0040 & pins) << 20) | ((0x0040 & pins) << 21)) & ((per_func) << 24)) | \
     ((((0x0080 & pins) << 21)| ((0x0080 & pins) << 22) | ((0x0080 & pins) << 23) | ((0x0080 & pins) << 24)) & ((per_func) << 28))); \
     GPIO##ref##_AFRH = ((GPIO##ref##_AFRH & \
     ~(((0x0100 & pins) >> 8) | ((0x0100 & pins) >> 7)  | ((0x0100 & pins) >> 6)  | ((0x0100 & pins) >> 5)    | \
     (((0x0200 & pins) >> 5)  | ((0x0200 & pins) >> 4)  | ((0x0200 & pins) >> 3)  | ((0x0200 & pins) >> 2))   | \
     (((0x0400 & pins) >> 2)  | ((0x0400 & pins) >> 1)  | ((0x0400 & pins))       | ((0x0400 & pins) << 1))   | \
     (((0x0800 & pins) << 1)  | ((0x0800 & pins) << 2)  | ((0x0800 & pins) << 3)  | ((0x0800 & pins) << 4))   | \
     (((0x1000 & pins) << 4)  | ((0x1000 & pins) << 5)  | ((0x1000 & pins) << 6)  | ((0x1000 & pins) << 7))   | \
     (((0x2000 & pins) << 7)  | ((0x2000 & pins) << 8)  | ((0x2000 & pins) << 9)  | ((0x2000 & pins) << 10))  | \
     (((0x4000 & pins) << 10) | ((0x4000 & pins) << 11) | ((0x4000 & pins) << 12) | ((0x4000 & pins) << 13))  | \
     (((0x8000 & pins) << 13) | ((0x8000 & pins) << 14) | ((0x8000 & pins) << 15) | ((0x8000 & pins) << 16))))| \
     ((((0x0100 & pins) >> 8) | ((0x0100 & pins) >> 7)  | ((0x0100 & pins) >> 6)  | ((0x0100 & pins) >> 5))  & (per_func)) | \
     ((((0x0200 & pins) >> 5) | ((0x0200 & pins) >> 4)  | ((0x0200 & pins) >> 3)  | ((0x0200 & pins) >> 2))  & ((per_func) << 4)) | \
     ((((0x0400 & pins) >> 2) | ((0x0400 & pins) >> 1)  | ((0x0400 & pins))       | ((0x0400 & pins) << 1))  & ((per_func) << 8)) | \
     ((((0x0800 & pins) << 1) | ((0x0800 & pins) << 2)  | ((0x0800 & pins) << 3)  | ((0x0800 & pins) << 4))  & ((per_func) << 12)) | \
     ((((0x1000 & pins) << 4) | ((0x1000 & pins) << 5)  | ((0x1000 & pins) << 6)  | ((0x1000 & pins) << 7))  & ((per_func) << 16)) | \
     ((((0x2000 & pins) << 7) | ((0x2000 & pins) << 8)  | ((0x2000 & pins) << 9)  | ((0x2000 & pins) << 10)) & ((per_func) << 20)) | \
     ((((0x4000 & pins) << 10)| ((0x4000 & pins) << 11) | ((0x4000 & pins) << 12) | ((0x4000 & pins) << 13)) & ((per_func) << 24)) | \
     ((((0x8000 & pins) << 13)| ((0x8000 & pins) << 14) | ((0x8000 & pins) << 15) | ((0x8000 & pins) << 16)) & ((per_func) << 28))); _SIM_PORT_CHANGE

    // Enable power to port, set the pins' to analog inputs
    //
    #define _CONFIG_ANALOG_INPUT(ref, pins) __POWER_UP_GPIO(ref); \
    GPIO##ref##_MODER = (GPIO##ref##_MODER | \
    ((0x0001 & pins) | ((0x0001 & pins) << 1) | ((0x0002 & pins) << 1) | ((0x0002 & pins) << 2) | ((0x0004 & pins) << 2) | ((0x0004 & pins) << 3) | ((0x0008 & pins) << 3) | ((0x0008 & pins) << 4) | \
    ((0x0010 & pins) << 4) | ((0x0010 & pins) << 5) | ((0x0020 & pins) << 5) | ((0x0020 & pins) << 6) | ((0x0040 & pins) << 6) | ((0x0040 & pins) << 7) | ((0x0080 & pins) << 7) | ((0x0080 & pins) << 8) | \
    ((0x0100 & pins) << 8) | ((0x0100 & pins) << 9) | ((0x0200 & pins) << 9) | ((0x0200 & pins) << 10) | ((0x0400 & pins) << 10)  | ((0x0400 & pins) << 11) | ((0x0800 & pins) << 11) | ((0x0800 & pins) << 12) | \
    ((0x1000 & pins) << 12) | ((0x1000 & pins) << 13) | ((0x2000 & pins) << 13) | ((0x2000 & pins) << 14) | ((0x4000 & pins) << 14)  | ((0x4000 & pins) << 15) | ((0x8000 & pins) << 15)) | ((0x8000 & pins) << 16)); _SIM_PORT_CHANGE

    #if defined _STM32L432
        #define PERIPHERAL_SYS                0x0
        #define PERIPHERAL_TIM1_2_LPTIM1      0x1
        #define PERIPHERAL_TIM1_2             0x2
        #define PERIPHERAL_USART2             0x3
        #define PERIPHERAL_I2C1_2_3           0x4
        #define PERIPHERAL_SPI1_2             0x5
        #define PERIPHERAL_SPI3               0x6
        #define PERIPHERAL_USART1_2_3         0x7
        #define PERIPHERAL_LPUART1            0x8
        #define PERIPHERAL_CAN1_TSC           0x9
        #define PERIPHERAL_USB_QUADSPI        0xa

        #define PERIPHERAL_COMP1_COMP2_SWPMI1 0xc
        #define PERIPHERAL_SAI1               0xd
        #define PERIPHERAL_TIM2_15_16_LPTIM2  0xe
        #define PERIPHERAL_EVENTOUT           0xf
    #elif defined _STM32L4X5 || defined _STM32L4X6
        #define PERIPHERAL_SYS                0x0
        #define PERIPHERAL_TIM1_2_LPTIM1      0x1
        #define PERIPHERAL_TIM1_2             0x2
        #define PERIPHERAL_USART2             0x3
        #define PERIPHERAL_I2C1_2_3           0x4
        #define PERIPHERAL_SPI1_2             0x5
        #define PERIPHERAL_SPI3               0x6
        #define PERIPHERAL_USART1_2_3         0x7
        #define PERIPHERAL_UART4_5            0x8
        #define PERIPHERAL_LPUART1            0x8
        #define PERIPHERAL_CAN1_TSC           0x9
        #define PERIPHERAL_USB_QUADSPI        0xa

        #define PERIPHERAL_COMP1_COMP2_SWPMI1 0xc
        #define PERIPHERAL_SAI1               0xd
        #define PERIPHERAL_TIM2_15_16_LPTIM2  0xe
        #define PERIPHERAL_EVENTOUT           0xf
    #elif defined _STM32L0x1
        #define PERIPHERAL_SPI1_USART2_LPTIM1_TIM21_EVENTOUT_SYS 0x0
        #define PERIPHERAL_SPI1_I2C1_LPTIM                       0x1
        #define PERIPHERAL_LPTIM_TIM2_EVENTOUT_SYS               0x2
        #define PERIPHERAL_I2C1_EVENTOUT                         0x3
        #define PERIPHERAL_I2C1_USART2_LPUART1_TIM22_EVENTOUT    0x4
        #define PERIPHERAL_TIM2_21_22                            0x5
        #define PERIPHERAL_LPUART1_EVENTOUT                      0x6
        #define PERIPHERAL_COMP1_COMP2                           0x7
        // For compatibility
        //
        #define PERIPHERAL_SYS          PERIPHERAL_SPI1_USART2_LPTIM1_TIM21_EVENTOUT_SYS
        #define PERIPHERAL_USART2       PERIPHERAL_I2C1_USART2_LPUART1_TIM22_EVENTOUT
        #define PERIPHERAL_USART1_2_3   PERIPHERAL_I2C1_USART2_LPUART1_TIM22_EVENTOUT
    #elif defined _STM32F031
        #define PERIPHERAL_SYS                0x0
        #define PERIPHERAL_USART1             0x1
    #else
        #define PERIPHERAL_SYS                0x0
        #define PERIPHERAL_TIM1_2             0x1
        #define PERIPHERAL_TIM3_4_5           0x2
        #define PERIPHERAL_TIM8_9_10_11       0x3
        #define PERIPHERAL_I2C1_2_3           0x4
        #define PERIPHERAL_SPI1_2_I2S2ext     0x5
        #define PERIPHERAL_SPI3_I2S3ext       0x6
        #define PERIPHERAL_USART1_2_3         0x7
        #define PERIPHERAL_USART4_5_6         0x8
        #define PERIPHERAL_USART4_5_6_7_8     0x8                        // valid for STM32F42XXX and STM32F43XXX
        #define PERIPHERAL_CAN1_2_TIM12_13_14 0x9
        #define PERIPHERAL_USB                0xa
        #define PERIPHERAL_ETH                0xb
        #define PERIPHERAL_FSMC_SDIO_OTG      0xc
        #define PERIPHERAL_DCMI               0xd
        #define PERIPHERAL_AF14               0xe
        #define PERIPHERAL_AF15               0x5
    #endif
    #define _PERIPHERAL_REMAP(remap_peripheral)                          // not used 
    #define _PERIPHERAL_REMOVE_REMAP(remap_peripheral)
#else
    #define _CONFIG_PERIPHERAL_OUTPUT(ref, per_func, pins, characteristics) _CONFIG_PORT_OUTPUT(ref, pins, (characteristics | ALTERNATIVE_FUNCTION))
    #define _CONFIG_PERIPHERAL_INPUT(ref, per_func, pins, characteristics) _CONFIG_PORT_OUTPUT(ref, pins, characteristics)
    #define _PERIPHERAL_REMAP(remap_peripheral)         POWER_UP(APB2, (RCC_APB2ENR_AFIOEN)); AFIO_MAPR |= remap_peripheral;
    #define _PERIPHERAL_REMOVE_REMAP(remap_peripheral)  POWER_UP(APB2, (RCC_APB2ENR_AFIOEN)); AFIO_MAPR &= ~(remap_peripheral);
#endif


// Write full port width
//
#define _WRITE_PORT(ref, value)        GPIO##ref##_ODR = (value); _SIM_PORT_CHANGE


// Write to a port with a mask
//
#define _WRITE_PORT_MASK(ref, value, mask)  GPIO##ref##_ODR = ((GPIO##ref##_ODR & ~mask) | (value & mask)); _SIM_PORT_CHANGE

// Toggle a port with a mask
//
#define _TOGGLE_PORT(ref, mask)        GPIO##ref##_ODR ^= (mask); _SIM_PORT_CHANGE

// Read full port width
//
#define _READ_PORT(ref)                GPIO##ref##_IDR;

// Read from a port with a mask
//
#define _READ_PORT_MASK(ref, mask)    (GPIO##ref##_IDR & (mask))

// Configure outputs and then set a value to them
//
#define _CONFIG_DRIVE_PORT_OUTPUT_VALUE(ref, pins, characteristics, value) _CONFIG_PORT_OUTPUT(ref, (pins), (characteristics)); \
                                      _WRITE_PORT_MASK(ref, (value), (pins))
                                      


// Set from outputs to inputs
//
#if defined _STM32F2XX || defined _STM32F4XX || defined _STM32F7XX       // {3}
    #define _FLOAT_PORT(ref, pins)    GPIO##ref##_MODER = ((GPIO##ref##_MODER & \
         ~((0x0001 & pins)        | ((0x0001 & pins) << 1)  | ((0x0002 & pins) << 1)  | ((0x0002 & pins) << 2)    | \
         (((0x0004 & pins) << 2)  | ((0x0004 & pins) << 3)  | ((0x0008 & pins) << 3)  | ((0x0008 & pins) << 4))   | \
         (((0x0010 & pins) << 4)  | ((0x0010 & pins) << 5)  | ((0x0020 & pins) << 5)  | ((0x0020 & pins) << 6))   | \
         (((0x0040 & pins) << 6)  | ((0x0040 & pins) << 7)  | ((0x0080 & pins) << 7)  | ((0x0080 & pins) << 8))   | \
         (((0x0100 & pins) << 8)  | ((0x0100 & pins) << 9)  | ((0x0200 & pins) << 9)  | ((0x0200 & pins) << 10))  | \
         (((0x0400 & pins) << 10) | ((0x0400 & pins) << 11) | ((0x0800 & pins) << 11) | ((0x0800 & pins) << 12))  | \
         (((0x1000 & pins) << 12) | ((0x1000 & pins) << 13) | ((0x2000 & pins) << 13) | ((0x2000 & pins) << 14))  | \
         (((0x4000 & pins) << 14) | ((0x4000 & pins) << 15) | ((0x8000 & pins) << 15) | ((0x8000 & pins) << 16))))); _SIM_PORT_CHANGE
#else
    #define _FLOAT_PORT(ref, pins)         GPIO##ref##_CRL &= ~(((pins & 0x0001) << 2)  | ((pins & 0x0001) << 3)  | \
                                                                ((pins & 0x0002) << 5)  | ((pins & 0x0002) << 6)  | \
                                                                ((pins & 0x0004) << 8)  | ((pins & 0x0004) << 9)  | \
                                                                ((pins & 0x0008) << 11) | ((pins & 0x0008) << 12) | \
                                                                ((pins & 0x0010) << 14) | ((pins & 0x0010) << 15) | \
                                                                ((pins & 0x0020) << 17) | ((pins & 0x0020) << 18) | \
                                                                ((pins & 0x0040) << 20) | ((pins & 0x0040) << 21) | \
                                                                ((pins & 0x0080) << 23) | ((pins & 0x0080) << 24)); \
                                            GPIO##ref##_CRH &= ~(((pins & 0x0100) >> 6)  | ((pins & 0x0100) >> 5) | \
                                                                ((pins & 0x0200) >> 3)  | ((pins & 0x0200) >> 2)  | \
                                                                 (pins & 0x0400)        | ((pins & 0x0400) << 1)  | \
                                                                ((pins & 0x0800) << 3)  | ((pins & 0x0800) << 4)  | \
                                                                ((pins & 0x1000) << 6)  | ((pins & 0x1000) << 7)  | \
                                                                ((pins & 0x2000) << 9)  | ((pins & 0x2000) << 10) | \
                                                                ((pins & 0x4000) << 12) | ((pins & 0x4000) << 13) | \
                                                                ((pins & 0x8000) << 15) | ((pins & 0x8000) << 16)); _SIM_PORT_CHANGE
#endif

// Set from inputs to outputs 
//
#if defined _STM32F2XX || defined _STM32F4XX || defined _STM32F7XX       // {4}
    #define _DRIVE_PORT_OUTPUT(ref, pins)  GPIO##ref##_MODER = (GPIO##ref##_MODER | \
         (((0x0001 & pins)) | ((0x0002 & pins) << 1) | ((0x0004 & pins) << 2) | ((0x0008 & pins) << 3) | ((0x0010 & pins) << 4) | ((0x0020 & pins) << 5) | \
         ((0x0040 & pins) << 6) | ((0x0080 & pins) << 7) | ((0x0100 & pins) << 8) | ((0x0200 & pins) << 9) | ((0x0400 & pins) << 10)| ((0x0800 & pins) << 11)| \
         ((0x1000 & pins) << 12)| ((0x2000 & pins) << 13)| ((0x4000 & pins) << 14)| ((0x8000 & pins) << 15))); _SIM_PORT_CHANGE
#else
    #define _DRIVE_PORT_OUTPUT(ref, pins)   GPIO##ref##_CRL |=  (((pins & 0x0001) << 2)  | ((pins & 0x0001) << 3)  | \
                                                                ((pins & 0x0002) << 5)  | ((pins & 0x0002) << 6)  | \
                                                                ((pins & 0x0004) << 8)  | ((pins & 0x0004) << 9)  | \
                                                                ((pins & 0x0008) << 11) | ((pins & 0x0008) << 12) | \
                                                                ((pins & 0x0010) << 14) | ((pins & 0x0010) << 15) | \
                                                                ((pins & 0x0020) << 17) | ((pins & 0x0020) << 18) | \
                                                                ((pins & 0x0040) << 20) | ((pins & 0x0040) << 21) | \
                                                                ((pins & 0x0080) << 23) | ((pins & 0x0080) << 24)); \
                                            GPIO##ref##_CRH |=  (((pins & 0x0100) >> 6)  | ((pins & 0x0100) >> 5) | \
                                                                ((pins & 0x0200) >> 3)  | ((pins & 0x0200) >> 2)  | \
                                                                 (pins & 0x0400)        | ((pins & 0x0400) << 1)  | \
                                                                ((pins & 0x0800) << 3)  | ((pins & 0x0800) << 4)  | \
                                                                ((pins & 0x1000) << 6)  | ((pins & 0x1000) << 7)  | \
                                                                ((pins & 0x2000) << 9)  | ((pins & 0x2000) << 10) | \
                                                                ((pins & 0x4000) << 12) | ((pins & 0x4000) << 13) | \
                                                                ((pins & 0x8000) << 15) | ((pins & 0x8000) << 16)); _SIM_PORT_CHANGE
#endif

// Set from inputs to outputs and set a value to them
//
#define _DRIVE_PORT_OUTPUT_VALUE(ref, pins, value)  _DRIVE_PORT_OUTPUT(ref, pins); _WRITE_PORT_MASK(ref, (value), (pins));


// Set and clear individual bits of a port
//
#ifdef _WINDOWS
    #define _SETBITS(ref, mask)        GPIO##ref##_ODR |= (mask); _SIM_PORT_CHANGE
    #define _CLEARBITS(ref, mask)      GPIO##ref##_ODR &= ~(mask); _SIM_PORT_CHANGE
#else
    #if defined _STM32F2XX || defined _STM32F4XX || defined _STM32F7XX
        #define _SETBITS(ref, mask)        GPIO##ref##_BSRR = (mask & 0xffff)
        #define _CLEARBITS(ref, mask)      GPIO##ref##_BSRR = ((unsigned long)(mask & 0xffff) << 16) // the F2/F4 don't have a BRR register so the BSRR register needs to be used
    #else
        #define _SETBITS(ref, mask)        GPIO##ref##_BSRR = (mask & 0xffff)
        #define _CLEARBITS(ref, mask)      GPIO##ref##_BRR  = (mask)
    #endif
#endif



// SSP
//
#define SSP0_CR_0                       *(unsigned long *)(SSP0_BLOCK + 0x00)  // Control Register 0 SSP 0
  #define SCR_MASK                      0xff00                                 // Serial Clock Rate mask
  #define CPHA                          0x0080                                 // Serial Clock Phase
  #define CPOL                          0x0040                                 // Serial Clock Polarity
  #define FRAME_FORMAT_MOT              0x0000
  #define FRAME_FORMAT_TI               0x0010
  #define FRAME_FORMAT_NAT              0x0020
  #define SSP_DATA_SIZE_MASK            0x000f
#define SSP0_CR_1                       *(unsigned long *)(SSP0_BLOCK + 0x04)  // Control Register 1 SSP 0
  #define SSP_SOD                       0x0008                                 // Slave Mode Output Disable
  #define SSP_MS                        0x0004                                 // nMaster/Slave Mode
  #define SSP_SSE                       0x0002                                 // SSP enable
  #define SSP_LBM                       0x0001                                 // Loop back mode
#define SSP0_DR                         *(volatile unsigned long *)(SSP0_BLOCK + 0x08)  // Data Register SSP 0
#define SSP0_SR                         *(volatile unsigned long *)(SSP0_BLOCK + 0x0c)  // Status Register SSP 0
  #define SSP_BUSY                      0x0010                                 // SSP Busy
  #define SSP_RFF                       0x0008                                 // Receive FIFO Full
  #define SSP_RNE                       0x0004                                 // Receive FIFO not empty
  #define SSP_TNF                       0x0002                                 // Transmit FIFO not full
  #define SSP_TFE                       0x0001                                 // Transmit FIFO empty
#define SSP0_PR                         *(unsigned long *)(SSP0_BLOCK + 0x10)  // Clock Prescaler Register SSP 0
  #define SSP_MAX_CLOCK                 2
#define SSP0_IMSCR                      *(unsigned long *)(SSP0_BLOCK + 0x14)  // Interrupt Mask Set and Clear Register SSP 0
  #define SSP_TXIM                      0x0008                                 // Transmit FIFO Interrupt Mask
  #define SSP_RXIM                      0x0004                                 // Receive FIFO Interrupt Mask
  #define SSP_RTIM                      0x0002                                 // Receive Timeout Interrupt Mask
  #define SSP_RORIM                     0x0001                                 // Receive Overrun Interrupt Mask
#define SSP0_RISR                       *(volatile unsigned long *)(SSP0_BLOCK + 0x18) // RAW Interrupt Status Register SSP 0
  #define SSP_TXRIS                     0x0008                                 // Transmit FIFO RAW Status Flag
  #define SSP_RXRIS                     0x0004                                 // Receive FIFO RAW Status Flag
  #define SSP_RTRIS                     0x0002                                 // Receive Timeout RAW Status Flag
  #define SSP_RORIS                     0x0001                                 // Receive Overrun RAW Status Flag
#define SSP0_MISR                       *(volatile unsigned long *)(SSP0_BLOCK + 0x1c) // Masked Interrupt Status Register SSP 0
  #define SSP_TXMIS                     0x0008                                 // Transmit FIFO Masked Status Flag
  #define SSP_RXMIS                     0x0004                                 // Receive FIFO Masked Status Flag
  #define SSP_RTMIS                     0x0002                                 // Receive Timeout Masked Status Flag
  #define SSP_ROMIS                     0x0001                                 // Receive Overrun Masked Status Flag
#define SSP0_ICR                        *(volatile unsigned long *)(SSP0_BLOCK + 0x20) // Interrupt Clear Register SSP 0
  #define SSP_RTIC                      0x0002                                 // Clear RX Timeout Interrupt
  #define SSP_RORIC                     0x0001                                 // Clear Rx Overrun Interrupt
#define SSP0_DMACR                      *(unsigned long *)(SSP0_BLOCK + 0x20) // Interrupt Clear Register SSP 0
  #define SSP_TXDMAE                    0x0002                                 // Transmit DMA Enabled
  #define SSP_RXDMAE                    0x0001                                 // Receive DMA Enabled

#define SSP1_CR_0                       *(unsigned long *)(SSP1_BLOCK + 0x00)  // Control Register 0 SSP 1
#define SSP1_CR_1                       *(unsigned long *)(SSP1_BLOCK + 0x04)  // Control Register 1 SSP 1
#define SSP1_DR                         *(volatile unsigned long *)(SSP1_BLOCK + 0x08)  // Data Register SSP 1
#define SSP1_SR                         *(volatile unsigned long *)(SSP1_BLOCK + 0x0c)  // Status Register SSP 1
#define SSP1_PR                         *(unsigned long *)(SSP1_BLOCK + 0x10)  // Clock Prescaler Register SSP 1
#define SSP1_IMSCR                      *(unsigned long *)(SSP1_BLOCK + 0x14)  // Interrupt Mask Set and Clear Register SSP 1
#define SSP1_RISR                       *(volatile unsigned long *)(SSP1_BLOCK + 0x18) // RAW Interrupt Status Register SSP 1
#define SSP1_MISR                       *(volatile unsigned long *)(SSP1_BLOCK + 0x1c) // Masked Interrupt Status Register SSP 1
#define SSP1_ICR                        *(volatile unsigned long *)(SSP1_BLOCK + 0x20 ) // Interrupt Clear Register SSP 1
#define SSP1_DMACR                      *(unsigned long *)(SSP1_BLOCK + 0x20) // Interrupt Clear Register SSP 1

// Independent Watchdog
//
#define IWDG_KR                         *(volatile unsigned long *)(IWDG_BLOCK + 0x00) // IWDG Key Register (write-only)
  #define IWDG_KR_START                 0xcccc
  #define IWDG_KR_RETRIGGER             0xaaaa
  #define IWDG_KR_MODIFY                0x5555
#define IWDG_PR                         *(unsigned long *)(IWDG_BLOCK + 0x04) // IWDG Prescale Register
  #define IWDG_PR_PRESCALE_4            0x0000
  #define IWDG_PR_PRESCALE_8            0x0001
  #define IWDG_PR_PRESCALE_16           0x0002
  #define IWDG_PR_PRESCALE_32           0x0003
  #define IWDG_PR_PRESCALE_64           0x0004
  #define IWDG_PR_PRESCALE_128          0x0005
  #define IWDG_PR_PRESCALE_256          0x0006
#define IWDG_RLR                        *(unsigned long *)(IWDG_BLOCK + 0x08) // IWDG Reload Register
  #define IWDG_RLR_MASK                 0xfff
#define IWDG_SR                         *(volatile unsigned long *)(IWDG_BLOCK + 0x0c) // IWDG Status Register (read-only)
  #define IWDG_SR_PVU                   0x00000001                       // watchdog prescale value update
  #define IWDG_SR_RVU                   0x00000002                       // watchdog reload value update
#if defined _STM32L432 || defined _STM32L0x1
    #define IWDG_WINR                   *(unsigned long *)(IWDG_BLOCK + 0x10)  // IWDG Reload Register
#endif

typedef struct st_STM32_IWDG
{
    volatile unsigned long _IWDG_KR;
    unsigned long _IWDG_PR;
    unsigned long _IWDG_RLR;
    volatile unsigned long _IWDG_SR;
#if defined _STM32L432 || defined _STM32L0x1
    unsigned long IWDG_WINR;
#endif
} _STM32_IWDG;

#define RETRIGGER_WATCHDOG()            IWDG_KR = IWDG_KR_RETRIGGER

// RTC
//
#if defined _STM32F2XX || defined _STM32F4XX || defined _STM32F7XX       // {5}
    #define RTC_TR                      *(volatile unsigned long *)(RTC_BLOCK + 0x00) // time register
      #define RTC_TR_SU                 0x0000000f
      #define RTC_TR_ST                 0x00000070
      #define RTC_TR_MNU                0x00000f00
      #define RTC_TR_MNT                0x00007000
      #define RTC_TR_HU                 0x000f0000
      #define RTC_TR_HT                 0x00300000
      #define RTC_TR_PM                 0x00400000
    #define RTC_DR                      *(volatile unsigned long *)(RTC_BLOCK + 0x04) // date register
      #define RTC_DR_DU                 0x0000000f
      #define RTC_DR_DT                 0x00000030
      #define RTC_DR_MU                 0x00000f00
      #define RTC_DR_MT                 0x00001000
      #define RTC_DR_WDU                0x0000e000
      #define RTC_DR_YU                 0x000f0000
      #define RTC_DR_YT                 0x00f00000
    #define RTC_CR                      *(unsigned long *)(RTC_BLOCK + 0x08)
      #define RTC_CR_WUCKSEL            0x00000007
      #define RTC_CR_TSEDGE             0x00000008
      #define RTC_CR_REFCKON            0x00000010
      #define RTC_CR_BYPSHAD            0x00000020
      #define RTC_CR_FMT_24_HOUR        0x00000000                       // 24 hour mode
      #define RTC_CR_FMT_AM_PM          0x00000040                       // 12 hour AM/PM mode
      #define RTC_CR_DCE                0x00000080
      #define RTC_CR_ALRAE              0x00000100
      #define RTC_CR_ALRBE              0x00000200
      #define RTC_CR_WUTE               0x00000400
      #define RTC_CR_TSE                0x00000800
      #define RTC_CR_ALRAIE             0x00001000
      #define RTC_CR_ALRBIE             0x00002000
      #define RTC_CR_WUTIE              0x00004000
      #define RTC_CR_TSIE               0x00008000
      #define RTC_CR_ADD1H              0x00010000
      #define RTC_CR_SUB1H              0x00020000
      #define RTC_CR_BKUP               0x00040000
      #define RTC_CR_COSEL              0x00080000
      #define RTC_CR_POL                0x00100000
      #define RTC_CR_OSEL               0x00600000
      #define RTC_CR_COE                0x00800000
    #define RTC_ISR                     *(volatile unsigned long *)(RTC_BLOCK + 0x0c) // RTC initialisation and status register
      #define RTC_ISR_ALRAWF            0x00000001                       // alarm A write flag
      #define RTC_ISR_ALRBWF            0x00000002                       // alarm B write flag
      #define RTC_ISR_WUTWF             0x00000004                       // wakeup timer write flag
      #define RTC_ISR_SHPF              0x00000008                       // shift operation pending
      #define RTC_ISR_INITS             0x00000010                       // initialisation status register
      #define RTC_ISR_RSF               0x00000020                       // registers synchronisation flag
      #define RTC_ISR_INITF             0x00000040                       // initialisation flag
      #define RTC_ISR_INIT              0x00000080                       // initialisation mode
      #define RTC_ISR_ALRAF             0x00000100                       // alarm A flag
      #define RTC_ISR_ALRBF             0x00000200                       // alarm B flag
      #define RTC_ISR_WUTF              0x00000400                       // wakeup timer flag
      #define RTC_ISR_TSF               0x00000800                       // timestamp flag
      #define RTC_ISR_TSOVF             0x00001000                       // timestamp overflow flag
      #define RTC_ISR_TAMP1F            0x00002000                       // tamper 1 detection flag
      #define RTC_ISR_TAMP2F            0x00004000                       // tamper 2 detection flag
      #define RTC_ISR_RECALPF           0x00010000                       // recalibration pending flag
    #define RTC_PRER                    *(volatile unsigned long *)(RTC_BLOCK + 0x10) // prescale register
      #define RTC_PRER_ASYN_SHIFT       16
    #define RTC_WUTR                    *(unsigned long *)(RTC_BLOCK + 0x14)
    #define RTC_CALIBR                  *(unsigned long *)(RTC_BLOCK + 0x18)
    #define RTC_ALRMAR                  *(unsigned long *)(RTC_BLOCK + 0x1c)
      #define RTC_ALRMAR_SU             0x0000000f
      #define RTC_ALRMAR_ST             0x00000070
      #define RTC_ALRMAR_MSK1           0x00000080                       // seconds don't care in comparison
      #define RTC_ALRMAR_MNU            0x00000f00
      #define RTC_ALRMAR_MNT            0x00007000
      #define RTC_ALRMAR_MSK2           0x00008000                       // minutes don't care in comparison
      #define RTC_ALRMAR_HU             0x000f0000
      #define RTC_ALRMAR_HUT            0x00300000
      #define RTC_ALRMAR_PM             0x00400000
      #define RTC_ALRMAR_MSK3           0x00800000                       // hours don't care in comparison
      #define RTC_ALRMAR_DU             0x0f000000
      #define RTC_ALRMAR_DT             0x30000000
      #define RTC_ALRMAR_WDSEL          0x40000000
      #define RTC_ALRMAR_MSK4           0x80000000                       // date/day don't care in comparison
    #define RTC_ALRMBR                  *(unsigned long *)(RTC_BLOCK + 0x20)
    #define RTC_WPR                     *(volatile unsigned long *)(RTC_BLOCK + 0x24) // RTC write protection register (write-only)
      #define RTC_WPR_KEY1              0x000000ca
      #define RTC_WPR_KEY2              0x00000053
    #define RTC_SSR                     *(volatile unsigned long *)(RTC_BLOCK + 0x28)
    #define RTC_SHIFTR                  *(unsigned long *)(RTC_BLOCK + 0x2c)
    #define RTC_TSTR                    *(unsigned long *)(RTC_BLOCK + 0x30)

    #define RTC_TSSSR                   *(unsigned long *)(RTC_BLOCK + 0x38)
    #define RTC_CALR                    *(unsigned long *)(RTC_BLOCK + 0x3c)
    #define RTC_TAFCR                   *(unsigned long *)(RTC_BLOCK + 0x40)
    #define RTC_ALRMASSR                *(unsigned long *)(RTC_BLOCK + 0x44)
    #define RTC_ALRMBSSR                *(unsigned long *)(RTC_BLOCK + 0x48)

    #define RTC_BK0R                    *(unsigned long *)(RTC_BLOCK + 0x50)
    #define RTC_BK1R                    *(unsigned long *)(RTC_BLOCK + 0x54)
    #define RTC_BK2R                    *(unsigned long *)(RTC_BLOCK + 0x58)
    #define RTC_BK3R                    *(unsigned long *)(RTC_BLOCK + 0x5c)
    #define RTC_BK4R                    *(unsigned long *)(RTC_BLOCK + 0x60)
    #define RTC_BK5R                    *(unsigned long *)(RTC_BLOCK + 0x64)
    #define RTC_BK6R                    *(unsigned long *)(RTC_BLOCK + 0x68)
    #define RTC_BK7R                    *(unsigned long *)(RTC_BLOCK + 0x6c)
    #define RTC_BK8R                    *(unsigned long *)(RTC_BLOCK + 0x70)
    #define RTC_BK9R                    *(unsigned long *)(RTC_BLOCK + 0x74)
    #define RTC_BK10R                   *(unsigned long *)(RTC_BLOCK + 0x78)
    #define RTC_BK11R                   *(unsigned long *)(RTC_BLOCK + 0x7c)
    #define RTC_BK12R                   *(unsigned long *)(RTC_BLOCK + 0x80)
    #define RTC_BK13R                   *(unsigned long *)(RTC_BLOCK + 0x84)
    #define RTC_BK14R                   *(unsigned long *)(RTC_BLOCK + 0x88)
    #define RTC_BK15R                   *(unsigned long *)(RTC_BLOCK + 0x8c)
    #define RTC_BK16R                   *(unsigned long *)(RTC_BLOCK + 0x90)
    #define RTC_BK17R                   *(unsigned long *)(RTC_BLOCK + 0x94)
    #define RTC_BK18R                   *(unsigned long *)(RTC_BLOCK + 0x98)
    #define RTC_BK19R                   *(unsigned long *)(RTC_BLOCK + 0x9c)
#else
    #define RTC_CRH                     *(unsigned short *)(RTC_BLOCK + 0x00) // RTC control register high (write-only)
      #define RTC_CRH_SECIE             0x0001                           // second interrupt enable
      #define RTC_CRH_ALRIE             0x0002                           // alarm interrupt enable
      #define RTC_CRH_OWIE              0x0004                           // overflow interrupt enable
    #define RTC_CRL                     *(volatile unsigned short *)(RTC_BLOCK + 0x04) // RTC control register high (various)
      #define RTC_CRL_SECF              0x0001                           // second flag (clear only)
      #define RTC_CRL_ALRF              0x0002                           // alarm flag (clear only)
      #define RTC_CRL_OWF               0x0004                           // overflow flag (clear only)
      #define RTC_CRL_RSF               0x0008                           // registers synchronised flag (clear only)
      #define RTC_CRL_CNF               0x0010                           // configuration flag
      #define RTC_CRL_RTOFF             0x0020                           // RTC operation off (read-only)
    #define RTC_PRLH                    *(volatile unsigned short *)(RTC_BLOCK + 0x08) // RTC prescaler load register high (write-only)
    #define RTC_PRLL                    *(volatile unsigned short *)(RTC_BLOCK + 0x0c) // RTC prescaler load register low (write-only)
    #define RTC_DIVH                    *(volatile unsigned short *)(RTC_BLOCK + 0x10) // RTC presclaler divider register high (read-only)
    #define RTC_DIVL                    *(volatile unsigned short *)(RTC_BLOCK + 0x14) // RTC prescaler divider register low (read-only)
    #define RTC_CNTH                    *(volatile unsigned short *)(RTC_BLOCK + 0x18) // RTC counter register high
    #define RTC_CNTL                    *(volatile unsigned short *)(RTC_BLOCK + 0x1c) // RTC counter register low
    #define RTC_ALRH                    *(volatile unsigned short *)(RTC_BLOCK + 0x20) // RTC alarm register high (write-only)
    #define RTC_ALRL                    *(volatile unsigned short *)(RTC_BLOCK + 0x24) // RTC alarm register low (write-only)
#endif

// Debug
//
#define DBGMCU_IDCODE                   *(unsigned long *)(DBG_BLOCK + 0x0) // Debug MCU Configuration Register
#define DBGMCU_CR                       *(unsigned long *)(DBG_BLOCK + 0x4) // Debug MCU Configuration Register
  #define DBG_SLEEP                     0x00000001
  #define DBG_STOP                      0x00000002
  #define DBG_STANDBY                   0x00000004
  #define TRACE_IOEN                    0x00000020
  #define TRACE_MODE_MASK               0x000000c0
  #define DBG_IWDGSTOP                  0x00000100
  #define DBG_WWDGSTOP                  0x00000200
  #define DBG_TIM1_STOP                 0x00000400
  #define DBG_TIM2_STOP                 0x00000800
  #define DBG_TIM3_STOP                 0x00001000
  #define DBG_TIM4_STOP                 0x00002000
  #define DBG_CAN1_STOP                 0x00004000
  #define DBG_I2C1_SMBUS_TIMEOUT        0x00008000
  #define DBG_I2C2_SMBUS_TIMEOUT        0x00010000
  #define DBG_TIM8_STOP                 0x00020000
  #define DBG_TIM5_STOP                 0x00040000
  #define DBG_TIM6_STOP                 0x00080000
  #define DBG_TIM7_STOP                 0x00100000
  #define DBG_CAB2_STOP                 0x00200000
  #define DBG_TIM12_STOP                0x02000000
  #define DBG_TIM13_STOP                0x04000000
  #define DBG_TIM14_STOP                0x08000000
  #define DBG_TIM9_STOP                 0x10000000
  #define DBG_TIM10_STOP                0x20000000
  #define DBG_TIM11_STOP                0x40000000


#define POWER_UP(bus, module)           RCC_##bus##ENR |= (module);
#define POWER_DOWN(bus, module)         RCC_##bus##ENR &= ~(module);
#define IS_POWERED_UP(bus, module)      (RCC_##bus##ENR & (module))





// Software references used when setting up interrupts
//
#define PORT_INTERRUPT            1
#define PORT_CHANNEL_INTERRUPT    2
#define TIMER_INTERRUPT           3
#define ADC_INTERRUPT             4

#define IRQ_RISING_EDGE           0x01
#define IRQ_FALLING_EDGE          0x02

#define PORTA                     0
#define PORTB                     1
#define PORTC                     2
#define PORTD                     3
#define PORTE                     4
#define PORTF                     5
#define PORTG                     6
#define PORTH                     7
#define PORTI                     8
#define PORTJ                     9
#define PORTK                     10

// Define interrupt setup structures to suit this processor
//
typedef struct stINTERRUPT_SETUP
{
    void (*int_handler)(void);                                           // interrupt handler to be configured
    unsigned char    int_type;                                           // identifier for when configuring
    unsigned char    int_priority;                                       // priority the user wants to set
    unsigned short   int_port_bit;                                       // {21} the input bit (eg. PORTA_BIT4)
    unsigned char    int_port;                                           // input port eg. PORTB
    unsigned char    int_port_sense;                                     // level sensitive, falling, rising, both
} INTERRUPT_SETUP;

typedef struct stTIMER_INTERRUPT_SETUP
{
    void (*int_handler)(void);                                           // interrupt handler to be configured
    unsigned char    int_type;                                           // identifier for when configuring
    unsigned char    int_priority;                                       // priority the user wants to set
    unsigned short   timer_mode;                                         // the mode
    unsigned short   pwm_value;
    unsigned short   timer_value;                                        // the timer value (depending on mode)
    unsigned short   prescaler_value;                                    // optional prescaler value
    unsigned char    timer_reference;                                    // hardware timer to use
} TIMER_INTERRUPT_SETUP;

#define TIMER_NATURAL_VALUE   0x0000                                     // the value passed can be set directly to the timer register
#define TIMER_US_VALUE        0x0001                                     // the value passed is in us (1..64k)
#define TIMER_MS_VALUE        0x0002                                     // the value passed is in ms (1..64k)
#define TIMER_SINGLE_SHOT     0x0000                                     // the timer should perform a single-shot delay interrupt
#define TIMER_PERIODIC        0x0004                                     // the timer should perform periodic interrupts
#define TIMER_FREQUENCY       0x0008                                     // the timer should generate a frequency rather than periodic interrupts
#define TIMER_PRESCALE        0x0010                                     // the pre-scale parameter should be used (only together with TIMER_NATURAL_VALUE)
#define TIMER_TRIGGER_ADC     0x0020                                     // the timer should be used to trigger ADC
#define TIMER_PWM_CH1         0x0040
#define TIMER_PWM_CH2         0x0080
#define TIMER_PWM_CH3         0x0100
#define TIMER_PWM_CH4         0x0200
#define TIMER_GEN_CH1         TIMER_PWM_CH1
#define TIMER_GEN_CH2         TIMER_PWM_CH2
#define TIMER_GEN_CH3         TIMER_PWM_CH3
#define TIMER_GEN_CH4         TIMER_PWM_CH4
#define TIMER_STOP_TIMER      0x8000                                     // {14}
#define TIMER_STOP_PWM        0x8000                                     // stop PWM channel(s)


#define _PWM_PERCENT(percent_pwm, frequency_value)       ((frequency_value * percent_pwm)/100)
#define _PWM_TENTH_PERCENT(percent_pwm, frequency_value) ((frequency_value * percent_pwm)/1000)

#define TIMER_US_DELAY(usec)                             (((PCLK1 * 2)/1000000) * usec) // {24}
#define TIMER_US_DELAY_PRESCALE(usec, pre)               (((PCLK1 * 2)/pre/1000000) * usec)
#define TIMER_MS_DELAY(msec)                             (((PCLK1 * 2)/1000) * msec)
#define TIMER_MS_DELAY_PRESCALE(msec, pre)               (((PCLK1 * 2)/pre/1000) * msec)
#define TIMER_FREQUENCY_VALUE(hertz)                     ((1000000/2)/hertz) // {23}

#define ACCURATE_TIMER_US_DELAY(usec)                    (unsigned short)(((float)(PCLK1 * 2)/(float)1000000) * (float)usec)
#define ACCURATE_TIMER_US_DELAY_PRESCALE(usec, pre)      (unsigned short)(((float)(PCLK1 * 2)/(float)pre/(float)1000000) * (float)usec)
#define ACCURATE_TIMER_MS_DELAY(msec)                    (unsigned short)(((float)(PCLK1 * 2)/(float)1000) * (float)msec)
#define ACCURATE_TIMER_MS_DELAY_PRESCALE(msec, pre)      (unsigned short)(((float)(PCLK1 * 2)/(float)pre/(float)1000) * (float)msec)
#define ACCURATE_TIMER_FREQUENCY_VALUE(hertz)            (unsigned short)((float)(1000000/2)/(float)hertz) // {23}


typedef struct stRTC_SETUP
{
    void (*int_handler)(void);                                           // interrupt handler to be configured
    unsigned short   usYear;
    unsigned char    ucMonthOfYear;
    unsigned char    ucDayOfMonth;
    unsigned char    ucDayOfWeek;
    unsigned char    ucHours;
    unsigned char    ucMinutes;
    unsigned char    ucSeconds;
    unsigned char    command;                                            // identifier for command to perform
} RTC_SETUP;


#define RTC_TIME_SETTING    0x01
#define RTC_TICK_SEC        0x02
#define RTC_TICK_MIN        0x03
#define RTC_TICK_HOUR       0x04
#define RTC_TICK_DAY        0x05
#define RTC_ALARM_TIME      0x06
#define RTC_STOPWATCH_GO    0x07
#define RTC_GET_TIME        0x08
#define RTC_DISABLE         0x80
#define RTC_INITIALISATION  0x40


#define ADC_CHANNELS          16                                         // single ended inputs               
#define ADC_CHANNELS_DIF      0                                          // differential inputs

typedef struct stADC_INTERRUPT_RESULT                                    // {22}
{
  //unsigned short   usADC_value[ADC_CHANNELS];                          // present ADC sample values
    unsigned long    usADC_value[ADC_CHANNELS];                          // temporarily long due to problem with DMA not respecting short word writes....
    unsigned char    ucADC_status[ADC_CHANNELS];
    unsigned char    ucADC_channel;                                      // ADC channel status
    unsigned char    ucSample;
    unsigned char    ucSequenceLength;
} ADC_INTERRUPT_RESULT;

typedef struct stADC_RESULTS
{
    unsigned short   usADC_value[ADC_CHANNELS];                          // present ADC sample values
    unsigned char    ucADC_status[ADC_CHANNELS];                         // present ADC channel status
} ADC_RESULTS;

// Define interrupt setup structure to suit this processor
//
typedef struct stADC_SETUP
{
    void (*int_handler)(ADC_INTERRUPT_RESULT *);                         // interrupt handler to be configured
    unsigned char    int_type;                                           // identifier for when configuring
    unsigned char    int_priority;                                       // priority the user wants to set
    unsigned char    int_adc_bit;                                        // the ADC input number (0..18)
    unsigned char    int_adc_int_type;                                   // interrupt type (ADC_LOW_LIMIT_INT, ADC_HIGH_LIMIT_INT, etc.)
    unsigned long    int_adc_mode;                                       // parallel, sequencial, single-ended, differential, start/stop, etc.
    unsigned short   int_adc_offset;                                     // offset for input
    unsigned short   int_high_level_trigger;                             // trigger when higher than this level
    unsigned short   int_low_level_trigger;                              // trigger when lower than this level
    unsigned char    int_adc_controller;                                 // the ADC controller to be used
    unsigned char    int_watchdog_bit;                                   // the ADC input number (0..19) to be monitored by the analog watchdog function (if not all)
    ADC_RESULTS     *int_adc_result;                                     // results structure to be filled
} ADC_SETUP;

// Temp
//
#define ADC_FULL_SCALE                  (0x0fff)
#define ADC_VOLT                        (unsigned short)((ADC_FULL_SCALE * 1000) / ADC_REFERENCE_VOLTAGE)

#define ADC_ENABLE_INTS                 0x00
#define ADC_END_OF_SCAN_INT             0x01
#define ADC_ZERO_CROSSING_INT_POSITIVE  0x02
#define ADC_ZERO_CROSSING_INT_NEGATIVE  0x04
#define ADC_LOW_LIMIT_INT               0x08
#define ADC_SINGLE_SHOT_CROSSING_INT    0x10
#define ADC_HIGH_LIMIT_INT              0x20
#define ADC_SINGLE_SHOT_TRIGGER_INT     0x40
#define ADC_DISABLE_INTS                0x80

#define ADC_RESULT_NOT_READY            0x00
#define ADC_RESULT_VALID                0x01
#define ADC_INT_ZERO_CROSSING           0x02
#define ADC_INT_HIGH_LEVEL              0x04
#define ADC_INT_LOW_LEVEL               0x08


// Valid
//
#define ADC_CALIBRATE                   0x00000000                       // dummy for compatibility
#define ADC_SINGLE_ENDED_INPUT          0x00000000                       // dummy for compatibility
#define ADC_HW_TRIGGERED                0x00000000                       // dummy for compatibility
#define ADC_SET_CHANNEL_OFFSET          0x00000000                       // dummy for compatibility
#define ADC_CHANNEL_CYCLES_3            0x00000000                       // ADC_SMPR_3_CYCLES
#define ADC_CHANNEL_CYCLES_15           0x00000001                       // ADC_SMPR_15_CYCLES
#define ADC_CHANNEL_CYCLES_28           0x00000002                       // ADC_SMPR_28_CYCLES
#define ADC_CHANNEL_CYCLES_56           0x00000003                       // ADC_SMPR_56_CYCLES
#define ADC_CHANNEL_CYCLES_84           0x00000004                       // ADC_SMPR_84_CYCLES
#define ADC_CHANNEL_CYCLES_122          0x00000005                       // ADC_SMPR_122_CYCLES
#define ADC_CHANNEL_CYCLES_144          0x00000006                       // ADC_SMPR_144_CYCLES
#define ADC_CHANNEL_CYCLES_480          0x00000007                       // ADC_SMPR_480_CYCLES
#define ADC_SINGLE_SHOT_MODE            0x00000000
#define ADC_SEQUENTIAL_MODE             0x00000010
#define ADC_START_OPERATION             0x00000020
#define ADC_FULL_BUFFER_DMA             0x00000100                       // ADC_CR2_DMA
#define ADC_RESULT_LEFT_ALIGNED         0x00000800                       // ADC_CR2_ALIGN_LEFT
#define ADC_RESULT_RIGHT_ALIGNED        0x00000000
#define ADC_CLOCK_PCLK_DIV_2            0x00000000                       // ADC_CSR_CCR_ADCPRE_PCLK_2
#define ADC_CLOCK_PCLK_DIV_4            0x00010000                       // ADC_CSR_CCR_ADCPRE_PCLK_4
#define ADC_CLOCK_PCLK_DIV_6            0x00020000                       // ADC_CSR_CCR_ADCPRE_PCLK_6
#define ADC_CLOCK_PCLK_DIV_8            0x00030000                       // ADC_CSR_CCR_ADCPRE_PCLK_8
#define ADC_GET_RESULT                  0x00100000
#define ADC_READ_ONLY                   0x00200000
#define ADC_CONFIGURE_CHANNEL           0x00400000
#define ADC_CONFIGURE_ADC               0x00800000
#define ADC_12_BIT_MODE                 0x00000000                       // ADC_CR1_RES_12_BIT
#define ADC_10_BIT_MODE                 0x01000000                       // ADC_CR1_RES_10_BIT
#define ADC_8_BIT_MODE                  0x02000000                       // ADC_CR1_RES_8_BIT
#define ADC_6_BIT_MODE                  0x03000000                       // ADC_CR1_RES_6_BIT
#define ADC_DISABLE_ADC                 0x04000000
#define ADC_LOOP_MODE                   0x08000000

// Temp reference
//
//#define ADC_CLOCK_ALTERNATE_SOURCE      0x00000002                       // ADC_CFG1_ADICLK_ALT
//#define ADC_CLOCK_ASYNCHRONOUS          0x00000003                       // ADC_CFG1_ADICLK_ASY
//#define ADC_SAMPLE_ACTIVATE_LONG        0x00000010                       // ADC_CFG1_ADLSMP_LONG
//#define ADC_LOW_POWER_CONFIG            0x00000080                       // ADC_CFG1_ADLPC
//#define ADC_REFERENCE_VREF              0x00000000
//#define ADC_SET_CHANNEL_OFFSET          0x00000100
//#define ADC_REFERENCE_VALT              0x00000100                       // (ADC_SC2_REFSEL_ALT << 8)
//#define ADC_SW_TRIGGERED                0x00000000
//#define ADC_HW_TRIGGERED                0x00004000                       // (ADC_SC2_ADTRG_HW << 8)
//#define ADC_FULL_BUFFER_DMA_AUTO_REPEAT 0x00010000
//#define ADC_DIFFERENTIAL_INPUT          0x04000000
//#define ADC_SELECT_INPUTS_A             0x00000000
//#define ADC_SELECT_INPUTS_B             0x10000000
//#define ADC_HALF_BUFFER_DMA             0x40000000
//#define ADC_DIFFERENTIAL_B              0x80000000




#if defined _STM32F7XX
    #define PORTS_AVAILABLE      11
    #define CHIP_HAS_UARTS       8
    #define CHIP_HAS_LPUARTS     0
    #define CHIP_HAS_I2C         4
#elif defined _STM32F429 || defined _STM32F427
    #define PORTS_AVAILABLE      11
    #define CHIP_HAS_UARTS       8
    #define CHIP_HAS_LPUARTS     0
    #define CHIP_HAS_I2C         3
#elif defined _STM32F2XX || defined _STM32F4XX
    #define PORTS_AVAILABLE      9
    #define CHIP_HAS_UARTS       6
    #define CHIP_HAS_LPUARTS     0
    #define CHIP_HAS_I2C         3
#elif defined _STM32L432 || defined _STM32L0x1
    #define PORTS_AVAILABLE      8
    #define CHIP_HAS_UARTS       2
    #define CHIP_HAS_LPUARTS     1
    #define CHIP_HAS_I2C         3
    #define CHIP_HAS_NO_I2C2
#elif defined _STM32F031
    #define PORTS_AVAILABLE      6
    #define CHIP_HAS_UARTS       1
    #define CHIP_HAS_LPUARTS     0
    #define CHIP_HAS_I2C         3
    #define CHIP_HAS_NO_I2C2
#elif defined _STM32L4X5 || defined _STM32L4X6
    #define PORTS_AVAILABLE      9
    #define CHIP_HAS_UARTS       6
    #define CHIP_HAS_LPUARTS     0
    #define CHIP_HAS_I2C         3
#else
    #define PORTS_AVAILABLE      5
    #define CHIP_HAS_UARTS       5
    #define CHIP_HAS_LPUARTS     0
    #define CHIP_HAS_I2C         2
#endif

#define I2C_AVAILABLE            CHIP_HAS_I2C                            // for compatibility
#define LPI2C_AVAILABLE          0                                       // for compatibility (low power I2C interfaces)

#define PORT_WIDTH       16


