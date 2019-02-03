/***********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher

    ---------------------------------------------------------------------
    File:        app_hw_stm32.h
    Project:     uTasker serial loader
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2019
    *********************************************************************

    Application specific hardware configuration

*/

#if defined _STM32 && !defined __APP_HW_STM32__
#define __APP_HW_STM32__

#if defined _WINDOWS
    #define _SIM_PORTS fnSimPorts()
#else
    #define _SIM_PORTS
#endif

#define SD_CONTROLLER_AVAILABLE                                          // older kwikstik's can't use this (default then to SPI interface) but from Rev. 5 they need it


#if defined STM3210C_EVAL                                                // STM32F107VCT (72MHz)
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
#elif defined STM3240G_EVAL                                              // STM32F407IGH6 (168MHz)
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
#elif defined ST_MB997A_DISCOVERY || defined STM32F405_BOARD             // STM32F407VGT6 (168MHz)
    #define CRYSTAL_FREQ        8000000
  //#define DISABLE_PLL                                                  // run from clock source directly
  //#define USE_HSI_CLOCK                                                // use internal HSI clock source
    #define PLL_INPUT_DIV       4                                        // 2..64 - should set the input to pll in the range 1..2MHz (with preference near to 2MHz)
    #define PLL_VCO_MUL         168                                      // 64 ..432 where VCO must be 64..432MHz
    #define PLL_POST_DIVIDE     2                                        // post divide VCO by 2, 4, 6, or 8 to get the system clock speed
    #if defined FINAL_HW
        #define PIN_COUNT       PIN_COUNT_64_PIN
    #else
        #define PIN_COUNT       PIN_COUNT_100_PIN
    #endif
    #define PACKAGE_TYPE        PACKAGE_LQFP
    #define SIZE_OF_RAM         (128 * 1024)                             // 128k SRAM
    #define SIZE_OF_CCM         (64 * 1024)                              // 64k Core Coupled Memory
    #define SIZE_OF_FLASH       (1024 * 1024)                            // 1M FLASH
    #define SUPPLY_VOLTAGE      SUPPLY_2_7__3_6                          // power supply is in the range 2.7V..3.6V
    #define PCLK1_DIVIDE        4
    #define PCLK2_DIVIDE        2
    #define HCLK_DIVIDE         1
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
#else
                                                                         // other configurations can be added here
#endif


// Clock settings
//
#if defined DISABLE_PLL
    #undef PLL_OUTPUT_FREQ
    #if defined USE_HSI_CLOCK
        #define PLL_OUTPUT_FREQ  HSI_FREQUENCY
    #else
        #define PLL_OUTPUT_FREQ  CRYSTAL_FREQ
    #endif
#endif
#define SYSCLK          PLL_OUTPUT_FREQ 


#include "../../Hardware/STM32/STM32.h"

#define _DELETE_BOOT_MAILBOX()     *(BOOT_MAIL_BOX) = 0; CLEAR_RESET_CAUSES()

// F4 needs 4..24MHz external clock
// F4 starts with internal 16MHz 1% RC oscillator (F1 and F2 2%)
//
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
#elif defined SPI_FLASH_MX66L
    #define SPI_FLASH_S25FL164K                                          // specific type used
    #define SPI_FLASH_SIZE               (64 * 1024 * 1024)              // 512 Mbits/64 MBytes
    #define SPI_FLASH_PAGE_LENGTH        (256)
    #define SPI_FLASH_PAGES              (SPI_FLASH_SIZE/SPI_FLASH_PAGE_LENGTH)
    #define SPI_FLASH_SECTOR_LENGTH      (4 * 1024)                      // sector size of SPI FLASH
    #define SPI_FLASH_SECTORS            (SPI_FLASH_SIZE/SPI_FLASH_SECTOR_LENGTH)
    #define SPI_FLASH_BLOCK_LENGTH       SPI_FLASH_SECTOR_LENGTH         // for compatibility - file system granularity
  //#define SUPPORT_ERASE_SUSPEND                                        // automatically suspend an erase that is in progress when a write or a read is performed in a different sector (advised when FAT used in SPI Flash with block mnagement/wear-levelling)
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
        #define SPI_FLASH_PAGES             (8 * 1024)                   // 8Meg part {23}
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

#if defined STM3240G_EVAL
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
#elif defined STM32F405_BOARD
    // SPI FLASH system setup
    //
    //#define SPI_FLASH_MULTIPLE_CHIPS                                   // activate when multiple physical chips are used
    #define CS0_LINE                            PORTA_BIT4               // CS0 line used when SPI FLASH is enabled
    #define CS1_LINE                                                     // CS1 line used when extended SPI FLASH is enabled
    #define CS2_LINE                                                     // CS2 line used when extended SPI FLASH is enabled
    #define CS3_LINE                                                     // CS3 line used when extended SPI FLASH is enabled

    #define SPI_CS0_PORT                        GPIOA_ODR                // for simulator
    #define SPI_TX_BYTE                         SPI1_DR                  // for simulator
    #define SPI_RX_BYTE                         SPI1_DR                  // for simulator

    #define SSPDR_X                             SPI1_DR
    #define SSPSR_X                             SPI1_SR

    #define __ASSERT_CS(cs_line)                _CLEARBITS(A, cs_line)
    #define __NEGATE_CS(cs_line)                _SETBITS(A, cs_line)

    // SPI 1 used for SPI Flash interface - speed set to 22MHz (PCLK1 168MHz/4)
    //
    #define POWER_UP_SPI_FLASH_INTERFACE()      POWER_UP(APB2, (RCC_APB2ENR_SPI1EN))
    #define POWER_DOWN_SPI_FLASH_INTERFACE()    POWER_DOWN(APB2, (RCC_APB2ENR_SPI1EN))
    #define CONFIGURE_SPI_FLASH_INTERFACE()     _CONFIG_DRIVE_PORT_OUTPUT_VALUE(A, CS0_LINE, (OUTPUT_FAST | OUTPUT_PUSH_PULL), CS0_LINE); \
    _CONFIG_PERIPHERAL_OUTPUT(A, (PERIPHERAL_SPI1_2_I2S2ext), (SPI1_CLK_A_5 | SPI1_MOSI_A_7), (OUTPUT_FAST | OUTPUT_PUSH_PULL | INPUT_PULL_DOWN)); \
    _CONFIG_PERIPHERAL_INPUT(A,  (PERIPHERAL_SPI1_2_I2S2ext), (SPI1_MISO_A_6), INPUT_PULL_DOWN); \
    _RESET_CYCLE_PERIPHERAL(APB2, RCC_APB2RSTR_SPI1RST); \
    SPI1_CR1 = (SPICR1_BR_PCLK2_DIV2 | SPICR1_MSTR | SPICR1_SSI | SPICR1_CPOL | SPICR1_CPHA | SPICR1_SSM); \
    SPI1_CR1 = (SPICR1_SPE | SPICR1_BR_PCLK2_DIV2 | SPICR1_MSTR | SPICR1_SSI | SPICR1_CPOL | SPICR1_CPHA | SPICR1_SSM)
#endif

#define SPI_DATA_FLASH_0_SIZE       (SPI_FLASH_PAGES * SPI_FLASH_PAGE_LENGTH) 
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

#define TICK_INTERRUPT()                                                 // user callback from system TICK


#define SPI_FLASH_START        (FLASH_START_ADDRESS + SIZE_OF_FLASH)     // SPI FLASH starts immediately after FLASH


// FLASH based File System setup
//
#if defined FLASH_FILE_SYSTEM
    #if defined SPI_FILE_SYSTEM                                          // this is a test setup for external SPI FLASH, with the parameters at the end of internal FLASH
        #define uFILE_START      (SPI_FLASH_START)                       // FLASH location at at end of internal FLASH
        #define FILE_SYSTEM_SIZE (32 * FILE_GRANULARITY)                 // 512k reserved for file system
    #else
        #define uFILE_START      (FLASH_START_ADDRESS + FLASH_SECTOR_18_OFFSET)// FLASH location at 128k start
        #define FILE_GRANULARITY (1 * FLASH_GRANULARITY_LARGE)           // each file a multiple of 32k
        #define FILE_SYSTEM_SIZE (4 * FLASH_GRANULARITY_LARGE)           // 128k reserved for file system
        #define FILES_ADDRESSABLE                                        // file system is addressable (doesn't have to be retrieved)
    #endif
#endif


// Serial interfaces
//
#if defined SERIAL_INTERFACE
    #define NUMBER_EXTERNAL_SERIAL 0
    #define NUMBER_SERIAL  CHIP_HAS_UARTS                                // the number of physical queue needed for serial interface(s)
    #define SIM_COM_EXTENDED                                             // COM ports defined from 1..255
    #define SERIAL_PORT_0  9                                             // if we open UART channel 0 we simulate using this com port on the PC (0 for no mapping)
    #define SERIAL_PORT_1  9                                             // if we open UART channel 1 we simulate using this com port on the PC
    #define SERIAL_PORT_2  0                                             // if we open UART channel 2 we simulate using this com port on the PC
    #define SERIAL_PORT_3  9                                             // if we open UART channel 3 we simulate using this com port on the PC
    #define SERIAL_PORT_4  0                                             // if we open UART channel 4 we simulate using this com port on the PC
    #define SERIAL_PORT_5  9                                             // if we open UART channel 5 we simulate using this com port on the PC

  //#define SERIAL_SUPPORT_DMA                                           // enable UART DMA support
  //#define SUPPORT_HW_FLOW                                              // enable hardware flow control support

    #if defined ST_MB913C_DISCOVERY || defined ARDUINO_BLUE_PILL
        #define LOADER_UART    (STM32_UART_3)                            // use UART channel 2 (USART 3 since ST USARTs count from 1)
    #elif defined STM3240G_EVAL || defined STM32_P207 || defined STM32F407ZG_SK
        #define LOADER_UART    2                                         // use UART channel 2 (USART 3 since ST USARTs count from 1) - the board can't use USART 4 and SD card at the same time so needs a modification for this
      //#define LOADER_UART    3                                         // use UART channel 3 (USART 4 since ST USARTs count from 1)
    #elif defined ST_MB997A_DISCOVERY && defined EMBEST_BASE_BOARD       // {6}
        #define LOADER_UART    5                                         // use UART channel 5 (USART 6 since ST USARTs count from 1)
    #else
        #define LOADER_UART    1                                         // use UART channel 1 (USART 2 since ST USARTs count from 1)
    #endif

    #define TX_BUFFER_SIZE   (256)                                       // the size of RS232 input and output buffers
    #define RX_BUFFER_SIZE   (512)

  //#define USART1_REMAP                                                 // use USART1 on remapped pins (note that this is channel 0)
    #define USART2_REMAP                                                 // use USART2 on remapped pins (note that this is channel 1)
    #if defined STM32_P207 || defined STM32F407ZG_SK
        #define USART3_FULL_REMAP                                        // use USART3 on second set of remapped pins (note that this is channel 2)
    #elif !defined STM3240G_EVAL && !defined ARDUINO_BLUE_PILL
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


#if defined STM3240G_EVAL
    #define USER_KEY_BUTTON            PORTG_BIT15
    #define WAKEUP_BUTTON              PORTA_BIT0
    #define TAMPER_BUTTON              PORTC_BIT13
    #define LED1                       PORTG_BIT6
    #define LED2                       PORTG_BIT8
    #define LED3                       PORTI_BIT9
    #define LED4                       PORTC_BIT7

    #define BLINK_LED                  LED1

    #define INIT_WATCHDOG_DISABLE()    _CONFIG_PORT_INPUT(G, (USER_KEY_BUTTON), (INPUT_PULL_UP)); _CONFIG_PORT_INPUT(A, (WAKEUP_BUTTON), (INPUT_PULL_UP)) // PG15 and PA0 configured as inputs with pull-ups
    #define WATCHDOG_DISABLE()         (_READ_PORT_MASK(G, (USER_KEY_BUTTON)) == 0) // disable watchdog by holding the user button down at reset
    #define INIT_WATCHDOG_LED()        _CONFIG_PORT_OUTPUT(G, BLINK_LED, (OUTPUT_SLOW | OUTPUT_PUSH_PULL))
    #define TOGGLE_WATCHDOG_LED()      _TOGGLE_PORT(G, BLINK_LED)        // blink the LED, if set as output
    #define FORCE_BOOT()               (_READ_PORT_MASK(A, (WAKEUP_BUTTON))) // hold user key at reset to force boot loader mode
    #define RETAIN_LOADER_MODE()       FORCE_BOOT()                      // force retaining boot loader mode after SD card update

    // Power down the USB controller and disable interrupts before jumping to the application
    //
    #define RESET_PERIPHERALS()        IRQ0_31_CER  = 0xffffffff; \
                                       IRQ32_63_CER = 0xffffffff; \
                                       IRQ64_95_CER = 0xffffffff; \
                                       IRQ0_31_CPR  = 0xffffffff; \
                                       IRQ32_63_CPR = 0xffffffff; \
                                       IRQ64_95_CPR = 0xffffffff; \
                                       POWER_DOWN(AHB2, RCC_AHB2ENR_OTGFSEN); \
                                       POWER_DOWN(APB2, RCC_APB2ENR_SYSCFGEN); \
                                       SDIO_POWER = SDIO_POWER_POWER_OFF;
#elif defined STM3210C_EVA
    #define USER_KEY_BUTTON            PORTB_BIT9
    #define LED1                       PORTD_BIT7
    #define LED2                       PORTD_BIT13
    #define LED3                       PORTD_BIT3
    #define LED4                       PORTD_BIT4

    #define BLINK_LED                  LED4

    #define INIT_WATCHDOG_DISABLE()    _CONFIG_PORT_INPUT(B, (USER_KEY_BUTTON), (INPUT_PULL_UP)) // PB9 configured as input with pull-up
    #define WATCHDOG_DISABLE()         (!(_READ_PORT_MASK(B, (USER_KEY_BUTTON)))) // disable watchdog by holding the user button down at reset
    #define INIT_WATCHDOG_LED()        _CONFIG_PORT_OUTPUT(D, BLINK_LED, (OUTPUT_SLOW | OUTPUT_PUSH_PULL))
    #define TOGGLE_WATCHDOG_LED()      _TOGGLE_PORT(D, BLINK_LED)        // blink the LED, if set as output

    #define RETAIN_LOADER_MODE()       0                                 // force retaining boot loader mode after update
#elif defined ST_MB997A_DISCOVERY || defined STM32F405_BOARD             // F4
    #define USER_KEY_BUTTON            PORTA_BIT0
    #define LED1                       PORTD_BIT12                       // green LED
    #define LED2                       PORTD_BIT13                       // orange LED
    #define LED3                       PORTD_BIT14                       // red LED
    #define LED4                       PORTD_BIT15                       // blue LED

    #if defined FINAL_HW
        #define BLINK_LED              PORTC_BIT4
    #else
        #define BLINK_LED              LED1
    #endif

    #define WATCHDOG_DISABLE()         (_READ_PORT_MASK(A, (USER_KEY_BUTTON))) // disable watchdog by holding the user button down at reset
    #if defined FINAL_HW
        #define INIT_WATCHDOG_LED()    _CONFIG_PORT_OUTPUT(C, BLINK_LED, (OUTPUT_SLOW | OUTPUT_PUSH_PULL))
        #define TOGGLE_WATCHDOG_LED()  _TOGGLE_PORT(C, BLINK_LED)        // blink the LED, if set as output
    #else
        #define INIT_WATCHDOG_LED()    _CONFIG_PORT_OUTPUT(D, BLINK_LED, (OUTPUT_SLOW | OUTPUT_PUSH_PULL))
        #define TOGGLE_WATCHDOG_LED()  _TOGGLE_PORT(D, BLINK_LED)        // blink the LED, if set as output
    #endif
    #define INIT_WATCHDOG_DISABLE()    _CONFIG_PORT_INPUT(A, (USER_KEY_BUTTON), (FLOATING_INPUT)) // PA0 configured as input (pull-down on board)

    #if defined STM32F405_BOARD
        #define FORCE_BOOT()           1                                 // always start in loader mode
    #else
        #define FORCE_BOOT()           (_READ_PORT_MASK(A, (USER_KEY_BUTTON))) // hold user key at reset to force boot loader mode
    #endif

    #define RETAIN_LOADER_MODE()       FORCE_BOOT()                      // force retaining boot loader mode after update

    // Power down the USB controller and disable interrupts before jumping to the application
    //
    #define RESET_PERIPHERALS()        IRQ0_31_CER  = 0xffffffff; \
                                       IRQ32_63_CER = 0xffffffff; \
                                       IRQ64_95_CER = 0xffffffff; \
                                       IRQ0_31_CPR  = 0xffffffff; \
                                       IRQ32_63_CPR = 0xffffffff; \
                                       IRQ64_95_CPR = 0xffffffff; \
                                       POWER_DOWN(AHB2, RCC_AHB2ENR_OTGFSEN); \
                                       POWER_DOWN(APB2, RCC_APB2ENR_SYSCFGEN); \
                                       SDIO_POWER = SDIO_POWER_POWER_OFF;
#elif defined STM32F407ZG_SK                                             // F4
    #define USER_BUTTON                PORTG_BIT6
    #define TAMPER_BUTTON              PORTC_BIT13
    #define WKUP_BUTTON                PORTA_BIT0

    #define LED1                       PORTF_BIT6                        // green LED
    #define LED2                       PORTF_BIT7                        // yellow LED
    #define LED3                       PORTF_BIT8                        // red LED
    #define LED4                       PORTF_BIT9                        // green LED

    #define BLINK_LED                  LED1

    #define INIT_WATCHDOG_LED()        _CONFIG_PORT_OUTPUT(F, BLINK_LED, (OUTPUT_SLOW | OUTPUT_PUSH_PULL))
    #define TOGGLE_WATCHDOG_LED()      _TOGGLE_PORT(F, BLINK_LED)        // blink the LED, if set as output

    #define INIT_WATCHDOG_DISABLE()    _CONFIG_PORT_INPUT(A, (WKUP_BUTTON), (FLOATING_INPUT)); _CONFIG_PORT_INPUT(G, (USER_BUTTON), (FLOATING_INPUT)) // configure inputs (pull-up on board)
    #define WATCHDOG_DISABLE()         (_READ_PORT_MASK(A, (WKUP_BUTTON)) == 0) // disable watchdog by holding the user button down at reset


    #define FORCE_BOOT()               (_READ_PORT_MASK(G, (USER_BUTTON)) == 0) // hold user key at reset to force boot loader mode

    #define RETAIN_LOADER_MODE()       FORCE_BOOT()                      // force retaining boot loader mode after update

    // Power down the USB controller and disable interrupts before jumping to the application
    //
    #define RESET_PERIPHERALS()        IRQ0_31_CER  = 0xffffffff; \
                                       IRQ32_63_CER = 0xffffffff; \
                                       IRQ64_95_CER = 0xffffffff; \
                                       IRQ0_31_CPR  = 0xffffffff; \
                                       IRQ32_63_CPR = 0xffffffff; \
                                       IRQ64_95_CPR = 0xffffffff; \
                                       POWER_DOWN(AHB2, RCC_AHB2ENR_OTGFSEN); \
                                       POWER_DOWN(APB2, RCC_APB2ENR_SYSCFGEN); \
                                       SDIO_POWER = SDIO_POWER_POWER_OFF;
#elif defined STM32_P207                                                 // F2
    #define TAMPER_BUTTON              PORTC_BIT13
    #define WKUP_BUTTON                PORTA_BIT0

    #define LED1                       PORTF_BIT6                        // green LED
    #define LED2                       PORTF_BIT7                        // yellow LED
    #define LED3                       PORTF_BIT8                        // red LED
    #define LED4                       PORTF_BIT9                        // green LED

    #define BLINK_LED                  LED1

    #define INIT_WATCHDOG_LED()        _CONFIG_PORT_OUTPUT(F, BLINK_LED, (OUTPUT_SLOW | OUTPUT_PUSH_PULL))
    #define TOGGLE_WATCHDOG_LED()      _TOGGLE_PORT(F, BLINK_LED)        // blink the LED, if set as output

    #define INIT_WATCHDOG_DISABLE()    _CONFIG_PORT_INPUT(A, (WKUP_BUTTON), (FLOATING_INPUT)); _CONFIG_PORT_INPUT(C, (TAMPER_BUTTON), (FLOATING_INPUT)); // configure inputs (pull-up on board)
    #define WATCHDOG_DISABLE()         (_READ_PORT_MASK(A, (WKUP_BUTTON)) == 0) // disable watchdog by holding the user button down at reset


    #define FORCE_BOOT()               (_READ_PORT_MASK(C, (TAMPER_BUTTON)) == 0) // hold user key at reset to force boot loader mode

    #define RETAIN_LOADER_MODE()       FORCE_BOOT()                      // force retaining boot loader mode after update

    // Power down the USB controller and disable interrupts before jumping to the application
    //
    #define RESET_PERIPHERALS()        IRQ0_31_CER  = 0xffffffff; \
                                       IRQ32_63_CER = 0xffffffff; \
                                       IRQ64_95_CER = 0xffffffff; \
                                       IRQ0_31_CPR  = 0xffffffff; \
                                       IRQ32_63_CPR = 0xffffffff; \
                                       IRQ64_95_CPR = 0xffffffff; \
                                       POWER_DOWN(AHB2, RCC_AHB2ENR_OTGFSEN); \
                                       POWER_DOWN(APB2, RCC_APB2ENR_SYSCFGEN); \
                                       SDIO_POWER = SDIO_POWER_POWER_OFF;
#elif defined ARDUINO_BLUE_PILL                                          // STM32F103
    #define BLINK_LED                  PORTC_BIT13                       // green LED

#define INIT_WATCHDOG_DISABLE()        _CONFIG_PORT_INPUT(B, (PORTB_BIT12), (INPUT_PULL_UP)); _CONFIG_PORT_INPUT(B, (PORTB_BIT2), (FLOATING_INPUT)); _CONFIG_DRIVE_PORT_OUTPUT_VALUE(A, (PORTA_BIT11 | PORTA_BIT12), (OUTPUT_SLOW | OUTPUT_PUSH_PULL), 0); // PB2 and PB12 configured as input (PTB2 floating due to HW design)
    #define WATCHDOG_DISABLE()         (_READ_PORT_MASK(B, (PORTB_BIT12)) == 0) // disable watchdog by pulling pin 20 to ground at reset
    #define INIT_WATCHDOG_LED()        _CONFIG_PORT_OUTPUT(C, BLINK_LED, (OUTPUT_SLOW | OUTPUT_PUSH_PULL))
    #define TOGGLE_WATCHDOG_LED()      _TOGGLE_PORT(C, BLINK_LED)        // blink the LED, if set as output

    #define FORCE_BOOT()               ((_READ_PORT_MASK(B, (PORTB_BIT2)) != 0) || (SOFTWARE_RESET_DETECTED() && (*(BOOT_MAIL_BOX) == RESET_TO_SERIAL_LOADER))) // set BOOT1 to '1' at reset to force boot loader mode
    #define RETAIN_LOADER_MODE()       0                                 // force retaining boot loader mode after update

                                        // '0'            '1'    input state center (x,   y)   0 = circle, radius, controlling port, controlling pin 
    #define KEYPAD_LED_DEFINITIONS     {RGB(0,255,0),RGB(20,20,20), 1, {650, 215, 666, 244}, _PORTC, BLINK_LED},

    #define KEYPAD "../../uTaskerV1.4/Simulator/KeyPads/BluePill.bmp"

    // Power down the USB controller and disable interrupts before jumping to the application
    //
    #define RESET_PERIPHERALS()        IRQ0_31_CER  = 0xffffffff; \
                                       IRQ32_63_CER = 0xffffffff; \
                                       IRQ64_95_CER = 0xffffffff; \
                                       IRQ0_31_CPR  = 0xffffffff; \
                                       IRQ32_63_CPR = 0xffffffff; \
                                       IRQ64_95_CPR = 0xffffffff;
#elif defined ST_MB913C_DISCOVERY                                        // F1
    #define USER_KEY_BUTTON            PORTA_BIT0
    #define LED3                       PORTC_BIT9                        // green LED
    #define LED4                       PORTC_BIT8                        // blue LED

    #define BLINK_LED                  LED3

    #define INIT_WATCHDOG_DISABLE()    _CONFIG_PORT_INPUT(A, (USER_KEY_BUTTON), (INPUT_PULL_UP)) // PA0 configured as input with pull-up
    #define WATCHDOG_DISABLE()         (_READ_PORT_MASK(A, (USER_KEY_BUTTON)) == 0) // disable watchdog by holding the user button down at reset
    #define INIT_WATCHDOG_LED()        _CONFIG_PORT_OUTPUT(C, BLINK_LED, (OUTPUT_SLOW | OUTPUT_PUSH_PULL))
    #define TOGGLE_WATCHDOG_LED()      _TOGGLE_PORT(C, BLINK_LED)        // blink the LED, if set as output

    #define FORCE_BOOT()               (_READ_PORT_MASK(A, (USER_KEY_BUTTON))) // hold user key at reset to force boot loader mode
    #define RETAIN_LOADER_MODE()       0                                 // force retaining boot loader mode after update

    // Power down the USB controller and disable interrupts before jumping to the application
    //
    #define RESET_PERIPHERALS()        IRQ0_31_CER  = 0xffffffff; \
                                       IRQ32_63_CER = 0xffffffff; \
                                       IRQ64_95_CER = 0xffffffff; \
                                       IRQ0_31_CPR  = 0xffffffff; \
                                       IRQ32_63_CPR = 0xffffffff; \
                                       IRQ64_95_CPR = 0xffffffff;
#endif

#if defined SDCARD_SUPPORT
    #if defined STM3240G_EVAL || defined ST_MB997A_DISCOVERY
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
                    RCC_APB1RSTR |= RCC_APB1RSTR_SPI3RST; RCC_APB1RSTR &= ~RCC_APB1RSTR_SPI3RST; \
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

        #if defined EMBEST_BASE_BOARD && defined ST_MB997A_DISCOVERY
            #define CONFIGURE_SDCARD_DETECT_INPUT() _CONFIG_PORT_INPUT(C, PORTC_BIT10, (INPUT_PULL_UP | PULLUP_BIT10))
            #define SDCARD_DETECTION()               (!_READ_PORT_MASK(C, PORTC_BIT10))
        #else
            #define CONFIGURE_SDCARD_DETECT_INPUT() _CONFIG_PORT_INPUT(H, PORTH_BIT13, (INPUT_PULL_UP | PULLUP_BIT13))
            #define SDCARD_DETECTION()               (!_READ_PORT_MASK(H, PORTH_BIT13))
        #endif
    #elif defined STM3210C_EVAL || defined ST_MB913C_DISCOVERY
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
        #define SDCARD_DETECTION()            (!_READ_PORT_MASK(E, PORTE_BIT0))
    #endif
#endif

#if !defined OWN_SD_CARD_DISPLAY                                         // {3}
    #if defined SERIAL_INTERFACE                                         // when UART interface is available
        #define _DISPLAY_SD_CARD_NOT_PRESENT()    fnDebugMsg("SD-Card not present\r\n")
        #define _DISPLAY_SD_CARD_NOT_FORMATTED()  fnDebugMsg("SD-Card not formatted\r\n")
        #define _DISPLAY_NO_FILE()                fnDebugMsg("No file on card\r\n")
        #define _DISPLAY_SD_CARD_PRESENT()        fnDebugMsg("SD-Card present\r\n")
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

// Ethernet PHY (MII mode)
//
#if defined ETH_INTERFACE
    #define IP_RX_CHECKSUM_OFFLOAD                                       // allow the HW to perform IPv4/v6 UDP, TCP and ICMP checksum verification so that no such calculation is required in code
    #define IP_TX_CHECKSUM_OFFLOAD                                       // allow the HW to insert IPv4/v6 header checksum and so the software doesn't need to calculate and insert this
    #define IP_TX_PAYLOAD_CHECKSUM_OFFLOAD                               // allow the HW to insert IPv4/v6 payload checksum and so the software doesn't need to calculate and insert this (overrides IP_TX_CHECKSUM_OFFLOAD)

  //#define _MAGIC_RESET_FRAME        0x1234                             // allow a magic Ethernet reception frame to reset the board to boot loader mode (Ethernet protocol used) followed by three repetitions of the same value (total frame length 61 bytes)

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

    #define NUMBER_OF_TX_BUFFERS_IN_ETHERNET_DEVICE 4
    #define NUMBER_OF_RX_BUFFERS_IN_ETHERNET_DEVICE 6

    #define LAN_BUFFER_SIZE           1536                               // STM32 has ample space for full tx buffer
    #define HTTP_BUFFER_LENGTH        1400                               // we send frames with this maximum amount of payload data

    #if defined STM3240G_EVAL
        #define _DP83848
        #define PHY_ADDRESS_           0x01                              // address of PHY on STM3210C_EVAL board (MII mode)
        #define VNDR_MDL               0x09                              // vendor model number
        #define MDL_REV                0x00                              // model revision number
        #define PHY_IDENTIFIER         (0x20005c00 | (VNDR_MDL << 4) | MDL_REV) // NATIONAL DP83848 identifier
        #define PHY_INTERRUPT                                            // use PHY interrupt
        #define PHY_INT_PORT           PORT_B
        #define PHY_INT_PIN            14
        #define PHY_INT_PIN_STATE()    _READ_PORT_MASK(B, PORTB_BIT14)
        #define SUPPORT_PORT_INTERRUPTS                                  // support code for port interrupts due to the PHY interrupt
    #elif defined ST_MB997A_DISCOVERY && defined EMBEST_BASE_BOARD
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
      //#define INTERRUPT_TASK_PHY     TASK_NETWORK_INDICATOR            // link status reported to this task (do not use together with LAN_REPORT_ACTIVITY)
    #elif defined STM32F407ZG_SK
        #define ETHERNET_RMII                                            // use RMII Ethernet interface instead of MII
        #define _ST802RT1B                                               // ST802RT1B PHY used on the board
        #define PHY_ADDRESS_           (0x01)                            // address of PHY on board
        #define PHY_IDENTIFIER         0x02038461                        // ST802RT1B identifier
        #define ETH_TXD_G                                                // locate TXD0 and TXD1 on port G rather than port B
        #define PHY_INTERRUPT                                            // use PHY interrupt
        #define SUPPORT_PORT_INTERRUPTS                                  // support code for port interrupts due to the PHY interrupt
        #define PHY_INT_PORT           PORT_A                            // interrupt on PA3
        #define PHY_INT_PIN            3
        #define PHY_INT_PIN_STATE()    _READ_PORT_MASK(A, PORTA_BIT3)
    #elif defined STM32_P207
        #define ETHERNET_RMII                                            // use RMII Ethernet interface instead of MII
        #define _KS8721                                                  // Micrel KS8721BLMM PHY
        #define PHY_ADDRESS_           (0x01)                            // address of PHY on board
        #define PHY_IDENTIFIER         0x00221619                        // MICREL identifier
        #define ETH_TXD_G                                                // locate TXD0 and TXD1 on port G rather than port B
        #define PHY_INTERRUPT                                            // use PHY interrupt
        #define SUPPORT_PORT_INTERRUPTS                                  // support code for port interrupts due to the PHY interrupt
        #define PHY_INT_PORT           PORT_A                            // interrupt on PA3
        #define PHY_INT_PIN            3
        #define PHY_INT_PIN_STATE()    _READ_PORT_MASK(A, PORTA_BIT3)
    #else
        #define _DP83848
        #define PHY_ADDRESS_           0x01                              // address of PHY on STM3210C_EVAL board
        #define ETHERNET_RMII                                            // use RMII Ethernet interface instead of MII
        #define VNDR_MDL               0x09                              // vendor model number
        #define MDL_REV                0x00                              // model revision number
        #define PHY_IDENTIFIER         (0x20005c00 | (VNDR_MDL << 4) | MDL_REV) // NATIONAL DP83848 identifier
        #define PHY_INTERRUPT                                            // use PHY interrupt
        #define PHY_INT_PORT           PORT_C
        #define PHY_INT_PIN            13
        #define PHY_INT_PIN_STATE()    _READ_PORT_MASK(C, PORTC_BIT13)
    #endif

    #define ETHERNET_DRIVE_PHY_25MHZ                                     // set to drive 25MHz from MCO output for use as external PHY clock (saves 25MHz crystal requirement) [used also to specify driving 50MHz clock in RMII mode]
#endif


// Special support for this processor type
//
//#define DMA_MEMCPY_SET                                                 // memcpy and memset functions performed by DMA (if supported by processor - uses one DMA channel)
#define MEMCPY_CHANNEL             1                                     // use channel 1 of DMA controller 1 (avoid using this channel for other DMA purpose)
                                                                         // MEMCPY_CHANNEL > 7 means from DMA controller 2 (eg. MEMCPY_CHANNEL 8 is channel 1 of DMA controller 2)
#define DMA_CNDTR_MEMCPY  DMA1_CNDTR1                                    // define the DMA controller registers to be used
#define DMA_CMAR_MEMCPY   DMA1_CMAR1
#define DMA_CPAR_MEMCPY   DMA1_CPAR1
#define DMA_CCR_MEMCPY    DMA1_CCR1
#define DMA_CNDTR1_MEMCPY DMA1_CNDTR1

// Define interrupt priorities in the system (STM32 support 0..15 - 0 is highest priority and 15 is lowest priority)
//
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
#define PRIORITY_USART1            6
#define PRIORITY_USART2            6
#define PRIORITY_USART3            6
#define PRIORITY_HW_TIMER          5
#define PRIORITY_TIMERS            5
#define PRIORITY_DEVICE_LP_FS      4
#define PRIORITY_USB_OTG           4
#define PRIORITY_I2C1              4
#define PRIORITY_I2C2              4
#define PRIORITY_TWI               4
#define PRIORITY_TICK_TIMER        3
#define PRIORITY_ADC               2
#define PRIORITY_EMAC              1
#define PRIORITY_DEVICE_HP_FS      1
#define PRIORITY_OTG_FS            1


// Initial port input states for simulator
//
#define GPIO_DEFAULT_INPUT_A       0xffff
#define GPIO_DEFAULT_INPUT_B       0xffff
#define GPIO_DEFAULT_INPUT_C       0xffff
#define GPIO_DEFAULT_INPUT_D       0xffff
#define GPIO_DEFAULT_INPUT_E       0xffff
#define GPIO_DEFAULT_INPUT_F       0xffff
#define GPIO_DEFAULT_INPUT_G       0xffff
#define GPIO_DEFAULT_INPUT_H       0xffff
#define GPIO_DEFAULT_INPUT_I       0xffff

#endif
