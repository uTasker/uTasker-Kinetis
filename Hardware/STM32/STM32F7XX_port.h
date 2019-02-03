/**********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher

    ---------------------------------------------------------------------
    File:      STM32F7XX_port.h
    Project:   Single Chip Embedded Internet
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2017
    *********************************************************************

*/


#if PIN_COUNT == PIN_COUNT_100_PIN
    #define _PIN_COUNT              0                                    // LQFP100
#elif PIN_COUNT == PIN_COUNT_143_PIN
    #define _PIN_COUNT              1                                    // WLCSP143
#elif PIN_COUNT == PIN_COUNT_144_PIN
    #define _PIN_COUNT              2                                    // LQFP144
#elif PIN_COUNT == PIN_COUNT_176_PIN
    #if PACKAGE_TYPE ==  PACKAGE_LQFP
        #define _PIN_COUNT          4                                    // LQFP176
    #else
        #define _PIN_COUNT          3                                    // UFBGA176
    #endif
#elif PIN_COUNT == PIN_COUNT_208_PIN
    #define _PIN_COUNT              5                                    // LQFP208
#elif PIN_COUNT == PIN_COUNT_216_PIN
    #define _PIN_COUNT              6                                    // TFBGA216
#else
    #error "Package not clearly defined!!"
#endif

#define ALTERNATIVE_FUNCTIONS   16                                   // GPIO plus 16 possible peripheral functions

static const char *cPer[PORTS_AVAILABLE][PORT_WIDTH][1 + ALTERNATIVE_FUNCTIONS + 1] = {
    {
    // PA0..15
    // port      sys       tim1/2             Tim3/4/5  Tim8/9/10/11/LPTIM1/CEC  I2C1/2/3/4/CEC  SPI1/2/3/4/5/6   SPI3/SAI1  SPI2/3/USART1/2/3/UART5/SPDIFRX  SAI2/USART6/UART4/5/7/8/SPDIFRX CAN1/2/Tim12/13/14/QUADSPI/LCD  SAI2/QUADSPI/OTG2_HS/OTG1_FS  ETH/OTG1_FS FMC/SDIO/OTG2_FS  DCMI LCD   SYS   Additional
    {  "PA0",    "-",      "TIM2_CH1/TIM2_ETR",   "TIM5_CH1",     "TIM8_ETR",     "-",           "-",              "-",            "USART2_CTS",     "UART4_TX",      "-",                  "SAI2_SD_B",  "ETH_MII_CRS",     "-",              "-",          "-",         "EVENTOUT", "ADC123_IN0/WKUP" }, // PA0
    {  "PA1",    "-",      "TIM2_CH2",            "TIM5_CH2",     "-",            "-",           "-",              "-",            "USART2_RTS",     "UART4_RX",      "QUADSPI/BK1_IO3",    "SAI2_MCK_B", "ETH_MII_RX_CLK/ETH_RMII_REF_CLK", "-","-",        "LCD_R2",    "EVENTOUT", "ADC123_IN1" }, // PA1
    {  "PA2",    "-",      "TIM2_CH3",            "TIM5_CH3",     "TIM9_CH1",     "-",           "-",              "-",            "USART2_TX",      "SAI2_SCK_B",    "-",                  "-",          "ETH_MDIO",        "-",              "-",          "LCD_R1",    "EVENTOUT", "ADC123_IN2" }, // PA2
    {  "PA3",    "-",      "TIM2_CH4",            "TIM5_CH4",     "TIM9_CH2",     "-",           "-",              "-",            "USART2_RX",      "-",             "-",                  "OTG_HS_ULPI_D0","ETH_MII_COL",  "-",              "-",          "LCD_B5",    "EVENTOUT", "ADC123_IN3" }, // PA3
    {  "PA4",    "-",      "-",                   "-",            "-",            "-",           "SPI1_NSS/I2S1_WS","SPI3_NSS/I2S3_WS","USART2_CK",  "-",             "-",                  "-",          "-",               "OTG_HS_SOF",     "DCMI_HSYNC", "LCD_VSYNC", "EVENTOUT", "ADC12_IN4/DAC1_OUT" }, // PA4
    {  "PA5",    "-",      "TIM2_CH1/TIM2_ETR",   "-",            "TIM8_CH1N",    "-",           "SPI1_SCK/I2S1_CK","-",           "-",              "-",             "-",                  "OTG_HS_ULPI_CK", "-",           "-",              "-",          "LCD_R4",    "EVENTOUT", "ADC12_IN5/DAC2_OUT" }, // PA5
    {  "PA6",    "-",      "TIM1_BKIN",           "TIM3_CH1",     "TIM8_BKIN",    "-",           "SPI1_MISO",      "-",            "-",              "-",             "TIM13_CH1",          "-",          "-",               "-",              "DCMI_PIXCK", "LCD_G2",    "EVENTOUT", "ADC12_IN6" }, // PA6
    {  "PA7",    "-",      "TIM1_CH1N",           "TIM3_CH2",     "TIM8_CH1N",    "-",           "SPI1_MOSI/I2S1_SD","-",          "-",              "-",             "TIM14_CH1",          "-",          "ETH_MII_RX_DV/ETH_RMII_CRC_DV", "FMC_SDNWE","-",  "-",         "EVENTOUT", "ADC12_IN7" }, // PA7
    {  "PA8",    "MCO1",   "TIM1_CH1",            "-",            "TIM8_BKIN2",   "I2C3_SCL",    "-",              "-",            "USART1_CK",      "-",             "-",                  "OTG_FS_SOF", "-",               "-",              "-",          "LCD_R6",    "EVENTOUT", "-" }, // PA8
    {  "PA9",    "-",      "TIM1_CH2",            "-",            "-",            "I2C3_SMBA",   "SPI2_SCK/I2S2_CK","-",           "USART1_TX",      "-",             "-",                  "-",          "-",               "-",              "DCMI_D0",    "-",         "EVENTOUT", "OTG_FS_VBUS" }, // PA9
    {  "PA10",   "-",      "TIM1_CH3",            "-",            "-",            "-",           "-",              "-",            "USART1_RX",      "-",             "-",                  "OTG_FS_ID",  "-",               "-",              "DCMI_D1",    "-",         "EVENTOUT", "-" }, // PA10    
    {  "PA11",   "-",      "TIM1_CH4",            "-",            "-",            "-",           "-",              "-",            "USART1_CTS",     "-",             "CAN1_RX",            "OTG_FS_DM",  "-",               "-",              "-",          "LCD_R4",    "EVENTOUT", "-" }, // PA11
    {  "PA12",   "-",      "TIM1_ETR",            "-",            "-",            "-",           "-",              "-",            "USART1_RTS",     "SAI2_FS_B",     "CAN1_TX",            "OTG_FS_DP",  "-",               "-",              "-",          "LCD_R5",    "EVENTOUT", "-" }, // PA12
    {  "PA13",   "JTMS-SWDIO", "-",               "-",            "-",            "-",           "-",              "-",            "-",              "-",             "-",                  "-",          "-",               "-",              "-",          "-",         "EVENTOUT", "-" }, // PA13
    {  "PA14",   "JTMS-SWDIO", "-",               "-",            "-",            "-",           "-",              "-",            "-",              "-",             "-",                  "-",          "-",               "-",              "-",          "-",         "EVENTOUT", "-" }, // PA14
    {  "PA15",   "JTDI",   "TIM2_CH1/TIM2_ETR",   "-",            "-",            "HDMI_CEC",    "SPI1_NSS/I2S1_WS","SPI3_NSS/I2S3S_WS","-",         "UART4_RTS",     "-",                  "-",          "-",               "-",              "-",          "-",         "EVENTOUT", "-" }, // PA15
    },
    {
    // PB0..15
    // port      sys       tim1/2             Tim3/4/5  Tim8/9/10/11/LPTIM1/CEC  I2C1/2/3/4/CEC  SPI1/2/3/4/5/6   SPI3/SAI1  SPI2/3/USART1/2/3/UART5/SPDIFRX  SAI2/USART6/UART4/5/7/8/SPDIFRX CAN1/2/Tim12/13/14/QUADSPI/LCD  SAI2/QUADSPI/OTG2_HS/OTG1_FS  ETH/OTG1_FS FMC/SDIO/OTG2_FS  DCMI LCD   SYS   Additional
    {  "PB0",    "-",      "TIM1_CH2N",           "TIM3_CH3",     "TIM8_CH2N",    "-",           "-",              "-",            "-",              "UART4_CTS",     "LCD_R3",             "OTG_HS_ULPI_D1","ETH_MII_RXD2", "-",              "-",          "-",         "EVENTOUT", "ADC12_IN8" }, // PB0
    {  "PB1",    "-",      "TIM1_CH3N",           "TIM3_CH4",     "TIM8_CH3N",    "-",           "-",              "-",            "-",              "-",             "LCD_R6",             "OTG_HS_ULPI_D2","ETH_MII_RXD3", "-",              "-",          "-",         "EVENTOUT", "ADC12_IN9" }, // PB1
    {  "PB2",    "-",      "-",                   "-",            "-",            "-",           "-",              "SAI1_SD_A",    "SPI3_MOSI/I2S3_SD","-",           "QUADSPI_CLK",        "-",          "-",               "-",              "-",          "-",         "EVENTOUT", "-" }, // PB2
    {  "PB3",    "JTDO/TRACESWO","TIM2_CH2",      "-",            "-",            "-",           "SPI1_SCK/I2S1_CK","SPI3_SCK/I2S3_CK","-",          "-",             "-",                  "-",          "-",               "-",              "-",          "-",         "EVENTOUT", "-" }, // PB3
    {  "PB4",    "NJTRST", "-",                   "TIM3_CH1",     "-",            "-",           "SPI1_MISO",      "SPI3_MISO",    "SPI2_NSS/I2S2_WS","-",            "-",                  "-",          "-",               "-",              "-",          "-",         "EVENTOUT", "-" }, // PB4
    {  "PB5",    "-",      "-",                   "TIM3_CH2",     "-",            "I2C1_SMBA",   "SPI1_MOSI/I2S1_SD","SPI3_MOSI/I2S3_SD","-",        "-",             "CAN2_RX",            "OTG_HS_ULPI_D7", "ETH_PPS_OUT", "FMC_SDCKE1",     "DCMI_D10",   "-",         "EVENTOUT", "-" }, // PB5
    {  "PB6",    "-",      "-",                   "TIM4_CH1",     "HDMI_CEC",     "I2C1_SCL",    "-",              "-",            "USART1_TX",      "-",             "CAN2_TX",            "QUADSPI_BK1_NCS","-",           "FMC_SDNE1",      "DCMI_D5",    "-",         "EVENTOUT", "-" }, // PB6
    {  "PB7",    "-",      "-",                   "TIM4_CH2",     "-",            "I2C1_SDA",    "-",              "-",            "USART1_RX",      "-",             "-",                  "-",          "-",               "FMC_NL",         "DCMI_VSYNC", "-",         "EVENTOUT", "-" }, // PB7
    {  "PB8",    "-",      "-",                   "TIM4_CH3",     "TIM10_CH1",    "I2C1_SCL",    "-",              "-",            "-",              "-",             "CAN1_RX",            "-",          "ETH_MII_TXD3",    "SDMMC1_D4",      "DCMI_D6",    "LCD_B6",    "EVENTOUT", "-" }, // PB8
    {  "PB9",    "-",      "-",                   "TIM4_CH4",     "TIM11_CH1",    "I2C1_SDA",    "SPI2_NSS/I2S2_WS","-",           "-",              "-",             "CAN1_TX",            "-",          "-",               "SDMMC1_D5",      "DCMI_D7",    "LCD_B7",    "EVENTOUT", "-" }, // PB9
    {  "PB10",   "-",      "TIM2_CH3",            "-",            "-",            "I2C2_SCL",    "SPI2_SCK/I2S2_CK","-",           "USART3_TX",      "-",             "-",                  "OTG_HS_ULPI_D3", "ETH_MII_RX_ER", "-",            "-",          "LCD_G6",    "EVENTOUT", "-" }, // PB10
    {  "PB11",   "-",      "TIM2_CH4",            "-",            "-",            "I2C2_SDA",    "-",              "-",            "USART3_RX",      "-",             "-",                  "OTG_HS_ULPI_D4", "ETH_MII_TX_EN/ETH_RMII_TX_EN",  "-",          "-","LCD_G5","EVENTOUT", "-" }, // PB11
    {  "PB12",   "-",      "TIM1_BKIN",           "-",            "-",            "I2C2_SMA",    "SPI2_NSS/I2S2_WS","-",           "USART3_CK",      "-",             "CAN2_RX",            "OTG_HS_ULPI_D5", "ETH_MII_TXD0/ETH_RMII_TXD0",    "OTG_HS_ID",  "-","-",     "EVENTOUT", "-" }, // PB12
    {  "PB13",   "-",      "TIM1_CH1N",           "-",            "-",            "-",           "SPI2_SCK/I2S2_CK","-",           "USART3_CTS",     "-",             "CAN2_TX",            "OTG_HS_ULPI_D6", "ETH_MII_TXD1/ETH_RMII_TXD1",    "-",          "-","-",     "EVENTOUT", "OTG_HS_VBUS" }, // PB13
    {  "PB14",   "-",      "TIM1_CH2N",           "-",            "TIM8_CH2N",    "-",           "SPI2_MISO",      "-",            "USART3_RTS",     "-",             "TIM12_CH1",          "-",          "-",               "OTG_HS_DM",      "-",          "-",         "EVENTOUT", "-" }, // PB14
    {  "PB15",   "RTC_REFIN","TIM1_CH3N",         "-",            "TIM8_CH3N",    "-",           "SPI2_MOSI/I2S2_SD","-",          "-",              "-",             "TIM12_CH2",          "-",          "-",               "OTG_HS_DP",      "-",          "-",         "EVENTOUT", "-" }, // PB15
    },
    {
    // PC0..15
    // port      sys       tim1/2             Tim3/4/5  Tim8/9/10/11/LPTIM1/CEC  I2C1/2/3/4/CEC  SPI1/2/3/4/5/6   SPI3/SAI1  SPI2/3/USART1/2/3/UART5/SPDIFRX  SAI2/USART6/UART4/5/7/8/SPDIFRX CAN1/2/Tim12/13/14/QUADSPI/LCD  SAI2/QUADSPI/OTG2_HS/OTG1_FS  ETH/OTG1_FS FMC/SDIO/OTG2_FS  DCMI LCD   SYS   Additional
    {  "PC0",    "-",      "-",                   "-",            "-",            "-",           "-",              "-",            "-",              "SAI2_FS_B",     "-",                  "OTG_HS_ULPI_STP","-",           "FMC_SDNWE",      "-",          "LCD_R5",    "EVENTOUT", "ADC123_IN10" }, // PC0
    {  "PC1",    "TRACED0","-",                   "-",            "-",            "-",           "SPI2_MOSI/I2S2_SD","SAI1_SD_A",  "-",              "-",             "-",                  "-",          "ETH_MDC",         "-",              "-",          "-",         "EVENTOUT", "ADC123_IN11" }, // PC1
    {  "PC2",    "-",      "-",                   "-",            "-",            "-",           "SPI2_MISO",      "-",            "-",              "-",             "-",                  "OTG_HS_ULPI_DIR","ETH_MII_TXD2","FMC_SDNE0",      "-",          "-",         "EVENTOUT", "ADC123_IN12" }, // PC2
    {  "PC3",    "-",      "-",                   "-",            "-",            "-",           "SPI2_MOSI/I2S2_SD","-",          "-",              "-",             "-",                  "OTG_HS_ULPI_NXT","ETH_MII_TX_CLK","FMC_SDCKE0",   "-",          "-",         "EVENTOUT", "ADC123_IN13" }, // PC3
    {  "PC4",    "-",      "-",                   "-",            "-",            "-",           "I2S1_MCK",       "-",            "-",              "SPDIFRX_IN2",   "-",                  "-",          "ETH_MII_RXD0/ETH_RMII_RXD0", "FMC_SDNE0", "-",    "-",         "EVENTOUT", "ADC12_IN14" }, // PC4
    {  "PC5",    "-",      "-",                   "-",            "-",            "-",           "-",              "-",            "-",              "SPDIFRX_IN3",   "-",                  "-",          "ETH_MII_RXD1/ETH_RMII_RXD1", "FMC_SDCKE0", "-",   "-",         "EVENTOUT", "ADC12_IN15" }, // PC5
    {  "PC6",    "-",      "-",                   "TIM3_CH1",     "TIM8_CH1",     "-",           "I2S2_MCK",       "-",            "-",              "USART6_TX",     "-",                  "-",          "-",               "SDMMC_D6",       "DCMI_D0",    "LCD_HS_HSYNC","EVENTOUT", "-" }, // PC6
    {  "PC7",    "-",      "-",                   "TIM3_CH2",     "TIM8_CH2",     "-",           "-",              "I2S3_MCK",     "-",              "USART6_RX",     "-",                  "-",          "-",               "SDMMC_D7",       "DCMI_D1",    "LCD_G6",    "EVENTOUT", "-" }, // PC7
    {  "PC8",    "TRACED1","-",                   "TIM3_CH3",     "TIM8_CH3",     "-",           "-",              "-",            "UART5_RTS",      "USART6_CK",     "-",                  "-",          "-",               "SDMMC_D0",       "DCMI_D2",    "-",         "EVENTOUT", "-" }, // PC8
    {  "PC9",    "MCO2",   "-",                   "TIM3_CH4",     "TIM8_CH4",     "I2C3_SDA",    "I2S2_CKIN",      "-",            "UART5_CTS",      "-",             "QUADSPI_BK1_IO0",    "-",          "-",               "SDMMC_D1",       "DCMI_D3",    "-",         "EVENTOUT", "-" }, // PC9
    {  "PC10",   "-",      "-",                   "-",            "-",            "-",           "-",              "SPI3_SCK/I2S3_CK","USART3_TX",   "UART4_TX",      "QUADSPI_BK1_IO1",    "-",          "-",               "SDMMC_D2",       "DCMI_D8",    "LCD_R2",    "EVENTOUT", "-" }, // PC10    
    {  "PC11",   "-",      "-",                   "-",            "-",            "-",           "-",              "SPI3_MISO",    "USART3_RX",      "UART4_RX",      "QUADSPI_BK2_NCS",    "-",          "-",               "SDMMC_D3",       "DCMI_D4",    "-",         "EVENTOUT", "-" }, // PC11
    {  "PC12",   "TRACED3","-",                   "-",            "-",            "-",           "-",              "SPI3_MOSI/I2S3_SD", "USART3_CK", "USART5_TX",     "-",                  "-",          "-",               "SDMMC_CK",       "DCIM_D9",    "-",         "EVENTOUT", "-" }, // PC12
    {  "PC13",   "-",      "-",                   "-",            "-",            "-",           "-",              "-",            "-",              "-",             "-",                  "-",          "-",               "-",              "-",          "-",         "-",        "RTC_AF2" }, // PC13
    {  "PC14",   "-",      "-",                   "-",            "-",            "-",           "-",              "-",            "-",              "-",             "-",                  "-",          "-",               "-",              "-",          "-",         "-",        "OSC32_IN" }, // PC14
    {  "PC15",   "-",      "-",                   "-",            "-",            "-",           "-",              "-",            "-",              "-",             "-",                  "-",          "-",               "-",              "-",          "-",         "-",        "OSC32_OUT" }, // PC15
    },
    {
    // PD0..15
    // port      sys       tim1/2             Tim3/4/5  Tim8/9/10/11/LPTIM1/CEC  I2C1/2/3/4/CEC  SPI1/2/3/4/5/6   SPI3/SAI1  SPI2/3/USART1/2/3/UART5/SPDIFRX  SAI2/USART6/UART4/5/7/8/SPDIFRX CAN1/2/Tim12/13/14/QUADSPI/LCD  SAI2/QUADSPI/OTG2_HS/OTG1_FS  ETH/OTG1_FS FMC/SDIO/OTG2_FS  DCMI LCD   SYS   Additional
    {  "PD0",    "-",      "-",                   "-",            "-",            "-",           "-",              "-",            "-",              "-",             "CAN1_RX",            "-",          "-",               "FMC_D2",         "-",          "-",         "EVENTOUT", "-" }, // PD0
    {  "PD1",    "-",      "-",                   "-",            "-",            "-",           "-",              "-",            "-",              "-",             "CAN1_TX",            "-",          "-",               "FMC_D3",         "-",          "-",         "EVENTOUT", "-" }, // PD1
    {  "PD2",    "TRACED2","-",                   "TIM3_ETR",     "-",            "-",           "-",              "-",            "-",              "UART5_RX",      "-",                  "-",          "-",               "SDMMC1_CMD",     "DCMI_D11",   "-",         "EVENTOUT", "-" }, // PD2
    {  "PD3",    "-",      "-",                   "-",            "-",            "-",           "SPI2_SCK/I2S2_CK","-",           "USART2_CTS",     "-",             "-",                  "-",          "-",               "FMC_CLK",       "DCMI_D5",    "LCD_G7",    "EVENTOUT", "-" }, // PD3
    {  "PD4",    "-",      "-",                   "-",            "-",            "-",           "-",              "-",            "USART2_RTS",     "-",             "-",                  "-",          "-",               "FMC_NOE",       "-",          "-",         "EVENTOUT", "-" }, // PD4
    {  "PD5",    "-",      "-",                   "-",            "-",            "-",           "-",              "-",            "USART2_TX",      "-",             "-",                  "-",          "-",               "FMC_NWE",       "-",          "-",         "EVENTOUT", "-" }, // PD5
    {  "PD6",    "-",      "-",                   "-",            "-",            "-",           "SPI3_MOSI/I2S3_SD","SAI1_SD_A",  "USART2_RX",      "-",             "-",                  "-",          "-",               "FMC_NWAIT",     "DCMI_D10",   "LCD_B2",    "EVENTOUT", "-" }, // PD6
    {  "PD7",    "-",      "-",                   "-",            "-",            "-",           "-",              "-",            "USART2_CK",      "SPDIFRX_IN0",   "-",                  "-",          "-",               "FMC_NE1",       "-",          "-",         "EVENTOUT", "-" }, // PD7
    {  "PD8",    "-",      "-",                   "-",            "-",            "-",           "-",              "-",            "USART3_TX",      "SPDIFRX_IN1",   "-",                  "-",          "-",               "FMC_D13",       "-",          "-",         "EVENTOUT", "-" }, // PD8
    {  "PD9",    "-",      "-",                   "-",            "-",            "-",           "-",              "-",            "USART3_RX",      "-",             "-",                  "-",          "-",               "FMC_D14",       "-",          "-",         "EVENTOUT", "-" }, // PD9
    {  "PD10",   "-",      "-",                   "-",            "-",            "-",           "-",              "-",            "USART3_CK",      "-",             "-",                  "-",          "-",               "FMC_D15",       "-",          "LCD_B3",    "EVENTOUT", "-" }, // PD10
    {  "PD11",   "-",      "-",                   "-",            "-",            "I2C4_SMBA",   "-",              "-",            "USART3_CTS",     "-",             "QUADSPI_BK1_IO0",    "SAI2_SD_A",  "-",               "FMC_A16/FMC_CLE","-",         "-",         "EVENTOUT", "-" }, // PD11
    {  "PD12",   "-",      "-",                   "TIM4_CH1",     "LPTIM_IN1",    "I2C4_SCL",    "-",              "-",            "USART3_RTS",     "-",             "QUADSPI_BK1_IO1",    "SAI2_FS_A",  "-",               "FMC_A17/FMC_ALE","-",         "-",         "EVENTOUT", "-" }, // PD12
    {  "PD13",   "-",      "-",                   "TIM4_CH2",     "LPTIM1_OUT",   "I2C4_SDA",    "-",              "-",            "-",              "-",             "QUADSPI_BK1_IO3",    "SAI2_SCK_A", "-",               "FMC_A18",       "-",          "-",         "EVENTOUT", "-" }, // PD13
    {  "PD14",   "-",      "-",                   "TIM4_CH3",     "-",            "-",           "-",              "-",            "-",              "UART8_CTS",     "-",                  "-",          "-",               "FMC_D0",        "-",          "-",         "EVENTOUT", "-" }, // PD14
    {  "PD15",   "-",      "-",                   "TIM4_CH4",     "-",            "-",           "-",              "-",            "-",              "UART8_RTS",     "-",                  "-",          "-",               "FMC_D1",        "-",          "-",         "EVENTOUT", "-" }, // PD15
    },
    {
    // PE0..15
    // port      sys       tim1/2             Tim3/4/5  Tim8/9/10/11/LPTIM1/CEC  I2C1/2/3/4/CEC  SPI1/2/3/4/5/6   SPI3/SAI1  SPI2/3/USART1/2/3/UART5/SPDIFRX  SAI2/USART6/UART4/5/7/8/SPDIFRX CAN1/2/Tim12/13/14/QUADSPI/LCD  SAI2/QUADSPI/OTG2_HS/OTG1_FS  ETH/OTG1_FS FMC/SDIO/OTG2_FS  DCMI LCD   SYS   Additional
    {  "PE0",    "-",      "-",                   "TIM4_ETR",     "LPTIM1_ETR",   "-",           "-",              "-",            "-",              "UART8_Rx",      "-",                  "SAI2_MCK_A", "-",               "FMC_NBL0",      "DCMI_D2",    "-",         "EVENTOUT", "-" }, // PE0
    {  "PE1",    "-",      "-",                   "-",            "LPTIM1_IN2",   "-",           "-",              "-",            "-",              "UART8_Tx",      "-",                  "-",          "-",               "FMC_NBL1",      "DCMI_D3",    "-",         "EVENTOUT", "-" }, // PE1
    {  "PE2",    "TRACECLK","-",                  "-",            "-",            "-",           "SPI4_SCK",       "SAI1_MCLK_A",  "-",              "-",             "QUADSPI_BK1_IO2",    "-",          "ETH_MII_TXD3",    "FMC_A23",       "-",          "-",         "EVENTOUT", "-" }, // PE2
    {  "PE3",    "TRACED0","-",                   "-",            "-",            "-",           "-",              "SAI1_SD_B",    "-",              "-",             "-",                  "-",          "-",               "FMC_A19",       "-",          "-",         "EVENTOUT", "-" }, // PE3
    {  "PE4",    "TRACED1","-",                   "-",            "-",            "-",           "SPI4_NSS",       "SAI1_FS_A",    "-",              "-",             "-",                  "-",          "-",               "FMC_A20",       "DCMI_D4",    "LCD_B0",    "EVENTOUT", "-" }, // PE4
    {  "PE5",    "TRACED2","-",                   "-",            "TIM9_CH1",     "-",           "SPI4_MISO",      "SAI1_SCK_A",   "-",              "-",             "-",                  "-",          "-",               "FMC_A21",       "DCMI_D6",    "LCD_G0",    "EVENTOUT", "-" }, // PE5
    {  "PE6",    "TRACED3","TIM1_BKIN2",          "-",            "TIM9_CH2",     "-",           "SPI4_MOSI",      "SAI1_SD_A",    "-",              "-",             "-",                  "SAI2_MCK_B", "-",               "FMC_A22",       "DCMI_D7",    "LCD_G1",    "EVENTOUT", "-" }, // PE6
    {  "PE7",    "-",      "TIM1_ETR",            "-",            "-",            "-",           "-",              "-",            "-",              "UART7_Rx",      "-",                  "QUADSPI_BK2_IO0","-",           "FMC_D4",        "-",          "-",         "EVENTOUT", "-" }, // PE7
    {  "PE8",    "-",      "TIM1_CH1N",           "-",            "-",            "-",           "-",              "-",            "-",              "UART7_Tx",      "-",                  "QUADSPI_BK2_IO1","-",           "FMC_D5",        "-",          "-",         "EVENTOUT", "-" }, // PE8
    {  "PE9",    "-",      "TIM1_CH1",            "-",            "-",            "-",           "-",              "-",            "-",              "UART7_RTS",     "-",                  "QUADSPI_BK2_IO2","-",           "FMC_D6",        "-",          "-",         "EVENTOUT", "-" }, // PE9
    {  "PE10",   "-",      "TIM1_CH2N",           "-",            "-",            "-",           "-",              "-",            "-",              "UART7_CTS",     "-",                  "QUADSPI_BK2_IO3","-",           "FMC_D7",        "-",          "-",         "EVENTOUT", "-" }, // PE10
    {  "PE11",   "-",      "TIM1_CH2",            "-",            "-",            "-",           "SPI4_NSS",       "-",            "-",              "-",             "-",                  "SAI2_SD_B",  "-",               "FMC_D8",        "-",          "LCD_G3",    "EVENTOUT", "-" }, // PE11
    {  "PE12",   "-",      "TIM1_CH3N",           "-",            "-",            "-",           "SPI4_SCK",       "-",            "-",              "-",             "-",                  "SAI2_SCK_B", "-",               "FMC_D9",        "-",          "LCD_B4",    "EVENTOUT", "-" }, // PE12
    {  "PE13",   "-",      "TIM1_CH3",            "-",            "-",            "-",           "SPI4_MISO",      "-",            "-",              "-",             "-",                  "SAI2_FS_B",  "-",               "FMC_D10",       "-",          "LCD_DE",    "EVENTOUT", "-" }, // PE13
    {  "PE14",   "-",      "TIM1_CH4",            "-",            "-",            "-",           "SPI4_MOSI",      "-",            "-",              "-",             "-",                  "SAI2_MCK_B", "-",               "FMC_D11",       "-",          "LCD_CLK",   "EVENTOUT", "-" }, // PE14
    {  "PE15",   "-",      "TIM1_BKIN",           "-",            "-",            "-",           "-",              "-",            "-",              "-",             "-",                  "-",          "-",               "FMC_D12",       "-",          "LCD_R7",    "EVENTOUT", "-" }, // PE15
    },
    {
    // PF0..15
    // port      sys       tim1/2             Tim3/4/5  Tim8/9/10/11/LPTIM1/CEC  I2C1/2/3/4/CEC  SPI1/2/3/4/5/6   SPI3/SAI1  SPI2/3/USART1/2/3/UART5/SPDIFRX  SAI2/USART6/UART4/5/7/8/SPDIFRX CAN1/2/Tim12/13/14/QUADSPI/LCD  SAI2/QUADSPI/OTG2_HS/OTG1_FS  ETH/OTG1_FS FMC/SDIO/OTG2_FS  DCMI LCD   SYS   Additional
    {  "PF0",    "-",      "-",                   "-",            "-",            "I2C2_SDA",    "-",              "-",            "-",              "-",             "-",                  "-",          "-",               "FMC_A0",        "-",          "-",         "EVENTOUT", "-" }, // PF0
    {  "PF1",    "-",      "-",                   "-",            "-",            "I2C2_SCL",    "-",              "-",            "-",              "-",             "-",                  "-",          "-",               "FMC_A1",        "-",          "-",         "EVENTOUT", "-" }, // PF1
    {  "PF2",    "-",      "-",                   "-",            "-",            "I2C2_SMBA",   "-",              "-",            "-",              "-",             "-",                  "-",          "-",               "FMC_A2",        "-",          "-",         "EVENTOUT", "-" }, // PF2
    {  "PF3",    "-",      "-",                   "-",            "-",            "-",           "-",              "-",            "-",              "-",             "-",                  "-",          "-",               "FMC_A3",        "-",          "-",         "EVENTOUT", "ADC3_IN9" }, // PF3
    {  "PF4",    "-",      "-",                   "-",            "-",            "-",           "-",              "-",            "-",              "-",             "-",                  "-",          "-",               "FMC_A4",        "-",          "-",         "EVENTOUT", "ADC3_IN14" }, // PF4
    {  "PF5",    "-",      "-",                   "-",            "-",            "-",           "-",              "-",            "-",              "-",             "-",                  "-",          "-",               "FMC_A5",        "-",          "-",         "EVENTOUT", "ADC3_IN15" }, // PF5
    {  "PF6",    "-",      "-",                   "-",            "TIM10_CH1",    "-",           "SPI5_NSS",       "SAI1_SD_B",    "-",              "UART7_Rx",      "QUADSPI_BK1_IO3",    "-",          "-",               "-",             "-",          "-",         "EVENTOUT", "ADC3_IN4" }, // PF6
    {  "PF7",    "-",      "-",                   "-",            "TIM11_CH1",    "-",           "SPI5_SCK",       "SAI1_MCLK_B",  "-",              "UART7_Tx",      "QUADSPI_BK1_IO2",    "-",          "-",               "-",             "-",          "-",         "EVENTOUT", "ADC3_IN5" }, // PF7
    {  "PF8",    "-",      "-",                   "-",            "-",            "-",           "SPI5_MISO",      "SAI1_SCK_B",   "-",              "UART7_RTS",     "TIM13_CH1",          "QUADSPI_BK1_IO0","-",           "-",             "-",          "-",         "EVENTOUT", "ADC3_IN6" }, // PF8
    {  "PF9",    "-",      "-",                   "-",            "-",            "-",           "SPI5_MOSI",      "SAI1_FS_B",    "-",              "UART7_CTS",     "TIM14_CH1",          "QUADSPI_BK1_IO1","-",           "-",             "-",          "-",         "EVENTOUT", "ADC3_IN7" }, // PF9
    {  "PF10",   "-",      "-",                   "-",            "-",            "-",           "-",              "-",            "-",              "-",             "-",                  "-",          "-",               "-",             "DCMI_D11",   "LCD_DE",    "EVENTOUT", "ADC3_IN8" }, // PF10
    {  "PF11",   "-",      "-",                   "-",            "-",            "-",           "SPI5_MOSI",      "-",            "-",              "-",             "-",                  "SAI2_SD_B",  "-",               "FMC_SDNRAS",    "DCMI_D12",   "-",         "EVENTOUT", "-" }, // PF11
    {  "PF12",   "-",      "-",                   "-",            "-",            "-",           "-",              "-",            "-",              "-",             "-",                  "-",          "-",               "FMC_A6",        "-",          "-",         "EVENTOUT", "-" }, // PF12
    {  "PF13",   "-",      "-",                   "-",            "-",            "I2C4_SMBA",   "-",              "-",            "-",              "-",             "-",                  "-",          "-",               "FMC_A7",        "-",          "-",         "EVENTOUT", "-" }, // PF13
    {  "PF14",   "-",      "-",                   "-",            "-",            "I2C4_SCL",    "-",              "-",            "-",              "-",             "-",                  "-",          "-",               "FMC_A8",        "-",          "-",         "EVENTOUT", "-" }, // PF14
    {  "PF15",   "-",      "-",                   "-",            "-",            "I2C4_SDA",    "-",              "-",            "-",              "-",             "-",                  "-",          "-",               "FMC_A9",        "-",          "-",         "EVENTOUT", "-" }, // PF15
    },
    {
    // PG0..15
    // port      sys       tim1/2             Tim3/4/5  Tim8/9/10/11/LPTIM1/CEC  I2C1/2/3/4/CEC  SPI1/2/3/4/5/6   SPI3/SAI1  SPI2/3/USART1/2/3/UART5/SPDIFRX  SAI2/USART6/UART4/5/7/8/SPDIFRX CAN1/2/Tim12/13/14/QUADSPI/LCD  SAI2/QUADSPI/OTG2_HS/OTG1_FS  ETH/OTG1_FS FMC/SDIO/OTG2_FS  DCMI LCD   SYS   Additional
    {  "PG0",    "-",      "-",                   "-",            "-",            "-",           "-",              "-",            "-",              "-",             "-",                  "-",          "-",               "FMC_A10",       "-",          "-",         "EVENTOUT", "-" }, // PG0
    {  "PG1",    "-",      "-",                   "-",            "-",            "-",           "-",              "-",            "-",              "-",             "-",                  "-",          "-",               "FMC_A11",       "-",          "-",         "EVENTOUT", "-" }, // PG1
    {  "PG2",    "-",      "-",                   "-",            "-",            "-",           "-",              "-",            "-",              "-",             "-",                  "-",          "-",               "FMC_A12",       "-",          "-",         "EVENTOUT", "-" }, // PG2
    {  "PG3",    "-",      "-",                   "-",            "-",            "-",           "-",              "-",            "-",              "-",             "-",                  "-",          "-",               "FMC_A13",       "-",          "-",         "EVENTOUT", "-" }, // PG3
    {  "PG4",    "-",      "-",                   "-",            "-",            "-",           "-",              "-",            "-",              "-",             "-",                  "-",          "-",               "FMC_A14/FMC_BA0","-",         "-",         "EVENTOUT", "-" }, // PG4
    {  "PG5",    "-",      "-",                   "-",            "-",            "-",           "-",              "-",            "-",              "-",             "-",                  "-",          "-",               "FMC_A15/FMC_BA1","-",         "-",         "EVENTOUT", "-" }, // PG5
    {  "PG6",    "-",      "-",                   "-",            "-",            "-",           "-",              "-",            "-",              "-",             "-",                  "-",          "-",               "-",             "DCMI_D12",   "LCD_R7",    "EVENTOUT", "-" }, // PG6
    {  "PG7",    "-",      "-",                   "-",            "-",            "-",           "-",              "-",            "-",              "USART6_CK",     "-",                  "-",          "-",               "FMC_INT",       "DCMI_D13",   "LCD_CLK",   "EVENTOUT", "-" }, // PG7
    {  "PG8",    "-",      "-",                   "-",            "-",            "-",           "SPI6_NSS",       "-",            "SPDIFRX_IN2",    "USART6_RTS",    "-",                  "-",          "ETH_PPS_OUT",     "FMC_SDCLK",     "-",          "-",         "EVENTOUT", "-" }, // PG8
    {  "PG9",    "-",      "-",                   "-",            "-",            "-",           "-",              "-",            "SPDIFRX_IN3",    "USART6_RX",     "QUADSPI_BK2_IO2",    "SAI2_FS_B",  "-",               "FMC_NE2/FMC_NCE","DCMI_VSYNC","-",         "EVENTOUT", "-" }, // PG9
    {  "PG10",   "-",      "-",                   "-",            "-",            "-",           "-",              "-",            "-",              "-",             "LCD_G3",             "SAI2_SD_B",  "-",               "FMC_NE3",       "DCMI_D2",    "LCD_B2",    "EVENTOUT", "-" }, // PG10
    {  "PG11",   "-",      "-",                   "-",            "-",            "-",           "-",              "-",            "SPDIFRX_IN0",    "-",             "-",                  "-",          "ETH_MII_TX_EN/ETH_RMII_TX_EN", "-","DCMI_D3",    "LCD_B3",    "EVENTOUT", "-" }, // PG11
    {  "PG12",   "-",      "-",                   "-",            "LPTIM1_IN1",   "-",           "SPI6_MISO",      "-",            "SPDIFRX_IN1",    "USART6_RTS",    "LCD_B4",             "-",          "-",               "FMC_NE4",       "-",          "LCD_B1",    "EVENTOUT", "-" }, // PG12
    {  "PG13",   "TRACED0","-",                   "-",            "LPTIM1_OUT",   "-",           "SPI6_SCK",       "-",            "-",              "USART6_CTS",    "-",                  "-",          "ETH_MII_TXD0/ETH_RMII_TXD0", "FMC_A24", "-",     "LCD_R0",    "EVENTOUT", "-" }, // PG13
    {  "PG14",   "TRACED1","-",                   "-",            "LPTIM1_ETR",   "-",           "SPI6_MOSI",      "-",            "-",              "USART6_TX",     "QUADSPI_BK2_IO3",    "-",          "ETH_MII_TXD1/ETH_RMII_TXD1", "FMC_A25", "-",     "LCD_B0",    "EVENTOUT", "-" }, // PG14
    {  "PG15",   "-",      "-",                   "-",            "-",            "-",           "-",              "-",            "-",              "USART6_CTS",    "-",                  "-",          "-",               "FMC_SDNCAS",    "DCMI_D13",   "-",         "EVENTOUT", "-" }, // PG15
    },
    {
    // PH0..15
    // port      sys       tim1/2             Tim3/4/5  Tim8/9/10/11/LPTIM1/CEC  I2C1/2/3/4/CEC  SPI1/2/3/4/5/6   SPI3/SAI1  SPI2/3/USART1/2/3/UART5/SPDIFRX  SAI2/USART6/UART4/5/7/8/SPDIFRX CAN1/2/Tim12/13/14/QUADSPI/LCD  SAI2/QUADSPI/OTG2_HS/OTG1_FS  ETH/OTG1_FS FMC/SDIO/OTG2_FS  DCMI LCD   SYS   Additional
    {  "PH0",    "-",      "-",                   "-",            "-",            "-",           "-",              "-",            "-",              "-",             "-",                  "-",          "-",               "-",              "-",          "-",         "EVENTOUT", "OSC_IN" }, // PH0
    {  "PH1",    "-",      "-",                   "-",            "-",            "-",           "-",              "-",            "-",              "-",             "-",                  "-",          "-",               "-",              "-",          "-",         "EVENTOUT", "OSC_OUT" }, // PH1
    {  "PH2",    "-",      "-",                   "-",            "LPTIM1_IN2",   "-",           "-",              "-",            "-",              "-",             "QUADSPI_BK2_IO0",    "SAI2_SCK_B", "ETH_MII_CRS",     "FMC_SDCKE0",     "-",          "LCD_R0",    "EVENTOUT", "-" }, // PH2
    {  "PH3",    "-",      "-",                   "-",            "-",            "-",           "-",              "-",            "-",              "-",             "QUADSPI_BK2_IO1",    "SAI2_MCK_B", "ETH_MII_COL",     "FMC_SDNE0",      "-",          "LCD_R1",    "EVENTOUT", "-" }, // PH3
    {  "PH4",    "-",      "-",                   "-",            "-",            "I2C2_SCL",    "-",              "-",            "-",              "-",             "-",                  "OTG_HS_ULPI_NXT", "-",          "-",              "-",          "-",         "EVENTOUT", "-" }, // PH4
    {  "PH5",    "-",      "-",                   "-",            "-",            "I2C2_SDA",    "SPI5_NSS",       "-",            "-",              "-",             "-",                  "-",          "-",               "FMC_SDNWE",      "-",          "-",         "EVENTOUT", "-" }, // PH5
    {  "PH6",    "-",      "-",                   "-",            "-",            "I2C2_SMBA",   "SPI5_SCK",       "-",            "-",              "-",             "TIM12_CH1",          "-",          "ETH_MII_RXD2",    "FMC_SDNE1",      "DCMI_D8",    "-",         "EVENTOUT", "-" }, // PH6
    {  "PH7",    "-",      "-",                   "-",            "-",            "I2C3_SCL",    "SPI5_MISO",      "-",            "-",              "-",             "-",                  "-",          "ETH_MII_RXD3",    "FMC_SDCKE1",     "DCMI_D9",    "-",         "EVENTOUT", "-" }, // PH7
    {  "PH8",    "-",      "-",                   "-",            "-",            "I2C3_SDA",    "-",              "-",            "-",              "-",             "-",                  "-",          "-",               "FMC_D16",        "DCMI_HSYNC", "LCD_R2",    "EVENTOUT", "-" }, // PH8
    {  "PH9",    "-",      "-",                   "-",            "-",            "I2C3_SMBA",   "-",              "-",            "-",              "-",             "TIM12_CH2",          "-",          "-",               "FMC_D17",        "DCMI_D0",    "LCD_R3",    "EVENTOUT", "-" }, // PH9
    {  "PH10",   "-",      "-",                   "TIM5_CH1",     "-",            "I2C4_SMBA",   "-",              "-",            "-",              "-",             "-",                  "-",          "-",               "FMC_D18",        "DCMI_D1",    "LCD_R4",    "EVENTOUT", "-" }, // PH10
    {  "PH11",   "-",      "-",                   "TIM5_CH2",     "-",            "I2C4_SCL",    "-",              "-",            "-",              "-",             "-",                  "-",          "-",               "FMC_D19",        "DCMI_D2",    "LCD_R5",    "EVENTOUT", "-" }, // PH11
    {  "PH12",   "-",      "-",                   "TIM5_CH3",     "-",            "I2C4_SDA",    "-",              "-",            "-",              "-",             "-",                  "-",          "-",               "FMC_D20",        "DCMI_D3",    "LCD_R6",    "EVENTOUT", "-" }, // PH12
    {  "PH13",   "-",      "-",                   "-",            "TIM8_CH1N",    "-",           "-",              "-",            "-",              "-",             "CAN1_TX",            "-",          "-",               "FMC_D21",        "-",          "LCD_G2",    "EVENTOUT", "-" }, // PH13
    {  "PH14",   "-",      "-",                   "-",            "TIM8_CH2N",    "-",           "-",              "-",            "-",              "-",             "-",                  "-",          "-",               "FMC_D22",        "DCMI_D4",    "LCD_G3",    "EVENTOUT", "-" }, // PH14
    {  "PH15",   "-",      "-",                   "-",            "TIM8_CH3N",    "-",           "-",              "-",            "-",              "-",             "-",                  "-",          "-",               "FMC_D23",        "DCMI_D11",   "LCD_G4",    "EVENTOUT", "-" }, // PH15
    },
    {
    // PI0..15
    // port      sys       tim1/2             Tim3/4/5  Tim8/9/10/11/LPTIM1/CEC  I2C1/2/3/4/CEC  SPI1/2/3/4/5/6   SPI3/SAI1  SPI2/3/USART1/2/3/UART5/SPDIFRX  SAI2/USART6/UART4/5/7/8/SPDIFRX CAN1/2/Tim12/13/14/QUADSPI/LCD  SAI2/QUADSPI/OTG2_HS/OTG1_FS  ETH/OTG1_FS FMC/SDIO/OTG2_FS  DCMI LCD   SYS   Additional
    {  "PI0",    "-",      "-",                   "TIM5_CH4",     "-",            "-",           "SPI2_NSS/I2S2_WS","-",           "-",              "-",             "-",                  "-",          "-",               "FMC_D24",        "DCMI_D13",   "LCD_G5",    "EVENTOUT", "-" }, // PI0
    {  "PI1",    "-",      "-",                   "-",            "TIM8_BKIN2",   "-",           "SPI2_SCK/I2S2_CK","-",           "-",              "-",             "-",                  "-",          "-",               "FMC_D25",        "DCMI_D8",    "LCD_G6",    "EVENTOUT", "-" }, // PI1
    {  "PI2",    "-",      "-",                   "-",            "TIM8_CH4",     "-",           "SPI2_MISO",      "-",            "-",              "-",             "-",                  "-",          "-",               "FMC_D26",        "DCMI_D9",    "LCD_G7",    "EVENTOUT", "-" }, // PI2
    {  "PI3",    "-",      "-",                   "-",            "TIM8_ETR",     "-",           "SPI2_MOSI/I2S2_SD","-",          "-",              "-",             "-",                  "-",          "-",               "FMC_D27",        "DCMI_D10",   "-",         "EVENTOUT", "-" }, // PI3
    {  "PI4",    "-",      "-",                   "-",            "TIM8_BKIN",    "-",           "-",              "-",            "-",              "-",             "-",                  "SAI2_MCK_A", "-",               "FMC_NBL2",       "DCMI_D5",    "LCD_B4",    "EVENTOUT", "-" }, // PI4
    {  "PI5",    "-",      "-",                   "-",            "TIM8_CH1",     "-",           "-",              "-",            "-",              "-",             "-",                  "SAI2_SCK_A", "-",               "FMC_NBL3",       "DCIM_VSYNC", "LCD_B5",    "EVENTOUT", "-" }, // PI5
    {  "PI6",    "-",      "-",                   "-",            "TIM8_CH2",     "-",           "-",              "-",            "-",              "-",             "-",                  "SAI2_SD_A",  "-",               "FMC_D28",        "DCMI_D6",    "LCD_B6",    "EVENTOUT", "-" }, // PI6
    {  "PI7",    "-",      "-",                   "-",            "TIM8_CH3",     "-",           "-",              "-",            "-",              "-",             "-",                  "SAI2_FS_A",  "-",               "FMC_D29",        "DCMI_D7",    "LCD_B7",    "EVENTOUT", "-" }, // PI7
    {  "PI8",    "-",      "-",                   "-",            "-",            "-",           "-",              "-",            "-",              "-",             "-",                  "-",          "-",               "-",              "-",          "-",         "EVENTOUT", "RTC_AF2" }, // PI8
    {  "PI9",    "-",      "-",                   "-",            "-",            "-",           "-",              "-",            "-",              "-",             "CAN1_RX",            "-",          "-",               "FMC_D30",        "-",          "LCD_VSYNC", "EVENTOUT", "-" }, // PI9
    {  "PI10",   "-",      "-",                   "-",            "-",            "-",           "-",              "-",            "-",              "-",             "-",                  "-",          "ETH_MII_RX_ER",   "FMC_D31",        "-",          "LCD_HSYNC", "EVENTOUT", "-" }, // PI10
    {  "PI11",   "-",      "-",                   "-",            "-",            "-",           "-",              "-",            "-",              "-",             "-",                  "OTG_HS_ULPI_DIR","-",           "-",              "-",          "-",         "EVENTOUT", "-" }, // PI11
    {  "PI12",   "-",      "-",                   "-",            "-",            "-",           "-",              "-",            "-",              "-",             "-",                  "-",          "-",               "-",              "-",          "LCD_HSYNC", "EVENTOUT", "-" }, // PI12
    {  "PI13",   "-",      "-",                   "-",            "-",            "-",           "-",              "-",            "-",              "-",             "-",                  "-",          "-",               "-",              "-",          "LCD_VSYNC", "EVENTOUT", "-" }, // PI13
    {  "PI14",   "-",      "-",                   "-",            "-",            "-",           "-",              "-",            "-",              "-",             "-",                  "-",          "-",               "-",              "-",          "LCD_CLK",   "EVENTOUT", "-" }, // PI14
    {  "PI15",   "-",      "-",                   "-",            "-",            "-",           "-",              "-",            "-",              "-",             "-",                  "-",          "-",               "-",              "-",          "LCD_R0",    "EVENTOUT", "-" }, // PI15
    },
    {
    // PJ0..15
    // port      sys       tim1/2             Tim3/4/5  Tim8/9/10/11/LPTIM1/CEC  I2C1/2/3/4/CEC  SPI1/2/3/4/5/6   SPI3/SAI1  SPI2/3/USART1/2/3/UART5/SPDIFRX  SAI2/USART6/UART4/5/7/8/SPDIFRX CAN1/2/Tim12/13/14/QUADSPI/LCD  SAI2/QUADSPI/OTG2_HS/OTG1_FS  ETH/OTG1_FS FMC/SDIO/OTG2_FS  DCMI LCD   SYS   Additional
    {  "PJ0",    "-",      "-",                   "-",            "-",            "-",           "-",              "-",            "-",              "-",             "-",                  "-",          "-",               "-",              "-",          "LCD_R1",    "EVENTOUT", "-" }, // PJ0
    {  "PJ1",    "-",      "-",                   "-",            "-",            "-",           "-",              "-",            "-",              "-",             "-",                  "-",          "-",               "-",              "-",          "LCD_R2",    "EVENTOUT", "-" }, // PJ1
    {  "PJ2",    "-",      "-",                   "-",            "-",            "-",           "-",              "-",            "-",              "-",             "-",                  "-",          "-",               "-",              "-",          "LCD_R3",    "EVENTOUT", "-" }, // PJ2
    {  "PJ3",    "-",      "-",                   "-",            "-",            "-",           "-",              "-",            "-",              "-",             "-",                  "-",          "-",               "-",              "-",          "LCD_R4",    "EVENTOUT", "-" }, // PJ3
    {  "PJ4",    "-",      "-",                   "-",            "-",            "-",           "-",              "-",            "-",              "-",             "-",                  "-",          "-",               "-",              "-",          "LCD_R5",    "EVENTOUT", "-" }, // PJ4
    {  "PJ5",    "-",      "-",                   "-",            "-",            "-",           "-",              "-",            "-",              "-",             "-",                  "-",          "-",               "-",              "-",          "LCD_R6",    "EVENTOUT", "-" }, // PJ5
    {  "PJ6",    "-",      "-",                   "-",            "-",            "-",           "-",              "-",            "-",              "-",             "-",                  "-",          "-",               "-",              "-",          "LCD_R7",    "EVENTOUT", "-" }, // PJ6
    {  "PJ7",    "-",      "-",                   "-",            "-",            "-",           "-",              "-",            "-",              "-",             "-",                  "-",          "-",               "-",              "-",          "LCD_G0",    "EVENTOUT", "-" }, // PJ7
    {  "PJ8",    "-",      "-",                   "-",            "-",            "-",           "-",              "-",            "-",              "-",             "-",                  "-",          "-",               "-",              "-",          "LCD_G1",    "EVENTOUT", "-" }, // PJ8
    {  "PJ9",    "-",      "-",                   "-",            "-",            "-",           "-",              "-",            "-",              "-",             "-",                  "-",          "-",               "-",              "-",          "LCD_G2",    "EVENTOUT", "-" }, // PJ9
    {  "PJ10",   "-",      "-",                   "-",            "-",            "-",           "-",              "-",            "-",              "-",             "-",                  "-",          "-",               "-",              "-",          "LCD_G3",    "EVENTOUT", "-" }, // PJ10
    {  "PJ11",   "-",      "-",                   "-",            "-",            "-",           "-",              "-",            "-",              "-",             "-",                  "-",          "-",               "-",              "-",          "LCD_G4",    "EVENTOUT", "-" }, // PJ11
    {  "PJ12",   "-",      "-",                   "-",            "-",            "-",           "-",              "-",            "-",              "-",             "-",                  "-",          "-",               "-",              "-",          "LCD_B0",    "EVENTOUT", "-" }, // PJ12
    {  "PJ13",   "-",      "-",                   "-",            "-",            "-",           "-",              "-",            "-",              "-",             "-",                  "-",          "-",               "-",              "-",          "LCD_B1",    "EVENTOUT", "-" }, // PJ13
    {  "PJ14",   "-",      "-",                   "-",            "-",            "-",           "-",              "-",            "-",              "-",             "-",                  "-",          "-",               "-",              "-",          "LCD_B2",    "EVENTOUT", "-" }, // PJ14
    {  "PJ15",   "-",      "-",                   "-",            "-",            "-",           "-",              "-",            "-",              "-",             "-",                  "-",          "-",               "-",              "-",          "LCD_B3",    "EVENTOUT", "-" }, // PJ15
    },
    {
    // PK0..15
    // port      sys       tim1/2             Tim3/4/5  Tim8/9/10/11/LPTIM1/CEC  I2C1/2/3/4/CEC  SPI1/2/3/4/5/6   SPI3/SAI1  SPI2/3/USART1/2/3/UART5/SPDIFRX  SAI2/USART6/UART4/5/7/8/SPDIFRX CAN1/2/Tim12/13/14/QUADSPI/LCD  SAI2/QUADSPI/OTG2_HS/OTG1_FS  ETH/OTG1_FS FMC/SDIO/OTG2_FS  DCMI LCD   SYS   Additional
    {  "PK0",    "-",      "-",                   "-",            "-",            "-",           "-",              "-",            "-",              "-",             "-",                  "-",          "-",               "-",              "-",          "LCD_G5",    "EVENTOUT", "-" }, // PK0
    {  "PK1",    "-",      "-",                   "-",            "-",            "-",           "-",              "-",            "-",              "-",             "-",                  "-",          "-",               "-",              "-",          "LCD_G6",    "EVENTOUT", "-" }, // PK1
    {  "PK2",    "-",      "-",                   "-",            "-",            "-",           "-",              "-",            "-",              "-",             "-",                  "-",          "-",               "-",              "-",          "LCD_G7",    "EVENTOUT", "-" }, // PK2
    {  "PK3",    "-",      "-",                   "-",            "-",            "-",           "-",              "-",            "-",              "-",             "-",                  "-",          "-",               "-",              "-",          "LCD_B4",    "EVENTOUT", "-" }, // PK3
    {  "PK4",    "-",      "-",                   "-",            "-",            "-",           "-",              "-",            "-",              "-",             "-",                  "-",          "-",               "-",              "-",          "LCD_B5",    "EVENTOUT", "-" }, // PK4
    {  "PK5",    "-",      "-",                   "-",            "-",            "-",           "-",              "-",            "-",              "-",             "-",                  "-",          "-",               "-",              "-",          "LCD_B6",    "EVENTOUT", "-" }, // PK5
    {  "PK6",    "-",      "-",                   "-",            "-",            "-",           "-",              "-",            "-",              "-",             "-",                  "-",          "-",               "-",              "-",          "LCD_B7",    "EVENTOUT", "-" }, // PK6
    {  "PK7",    "-",      "-",                   "-",            "-",            "-",           "-",              "-",            "-",              "-",             "-",                  "-",          "-",               "-",              "-",          "LCD_DE",    "EVENTOUT", "-" }, // PK7
    {  "-",      "-",      "-",                   "-",            "-",            "-",           "-",              "-",            "-",              "-",             "-",                  "-",          "-",               "-",              "-",          "-",         "-",        "-" },
    {  "-",      "-",      "-",                   "-",            "-",            "-",           "-",              "-",            "-",              "-",             "-",                  "-",          "-",               "-",              "-",          "-",         "-",        "-" },
    {  "-",      "-",      "-",                   "-",            "-",            "-",           "-",              "-",            "-",              "-",             "-",                  "-",          "-",               "-",              "-",          "-",         "-",        "-" },
    {  "-",      "-",      "-",                   "-",            "-",            "-",           "-",              "-",            "-",              "-",             "-",                  "-",          "-",               "-",              "-",          "-",         "-",        "-" },
    {  "-",      "-",      "-",                   "-",            "-",            "-",           "-",              "-",            "-",              "-",             "-",                  "-",          "-",               "-",              "-",          "-",         "-",        "-" },
    {  "-",      "-",      "-",                   "-",            "-",            "-",           "-",              "-",            "-",              "-",             "-",                  "-",          "-",               "-",              "-",          "-",         "-",        "-" },
    {  "-",      "-",      "-",                   "-",            "-",            "-",           "-",              "-",            "-",              "-",             "-",                  "-",          "-",               "-",              "-",          "-",         "-",        "-" },
    {  "-",      "-",      "-",                   "-",            "-",            "-",           "-",              "-",            "-",              "-",             "-",                  "-",          "-",               "-",              "-",          "-",         "-",        "-" },
    },
};



static const char *cPinNumber[PORTS_AVAILABLE][PORT_WIDTH][7] = {
    {
        // LQFP100  WLCSP143 LQFP144 UFBGA176 LQFP176 LQFP208 TFBGA216   GPIO A
        {  "22",    "K9",    "34",    "N3",   "40",   "43",   "N3"  },   // PA0
        {  "23",    "K8",    "35",    "N2",   "41",   "44",   "N2"  },   // PA1
        {  "24",    "L9",    "36",    "P2",   "42",   "45",   "P2"  },   // PA2
        {  "25",    "M11",   "37",    "R2",   "47",   "50",   "R2"  },   // PA3
        {  "28",    "M10",   "40",    "N4",   "50",   "53",   "N4"  },   // PA4
        {  "29",    "M9",    "41",    "P4",   "51",   "54",   "P4"  },   // PA5
        {  "30",    "N10",   "42",    "P3",   "52",   "55",   "P3"  },   // PA6
        {  "31",    "L8",    "43",    "R3",   "53",   "56",   "R3"  },   // PA7
        {  "67",    "F1",    "100",   "F15",  "119",  "142",  "F15" },   // PA8
        {  "68",    "E2",    "101",   "E15",  "120",  "143",  "E15" },   // PA9
        {  "69",    "D5",    "102",   "D15",  "121",  "144",  "D15" },   // PA10
        {  "70",    "D4",    "103",   "C15",  "122",  "145",  "C15" },   // PA11
        {  "71",    "E1",    "104",   "B15",  "123",  "146",  "B15" },   // PA12
        {  "72",    "D3",    "105",   "A15",  "124",  "147",  "A15" },   // PA13
        {  "76",    "B1",    "109",   "A14",  "137",  "159",  "A14" },   // PA14
        {  "77",    "C2",    "110",   "A13",  "138",  "160",  "A13" },   // PA15
    },
    {
        // LQFP100  WLCSP143 LQFP144 UFBGA176 LQFP176 LQFP208 TFBGA216   GPIO B
        {  "34",    "N8",    "46",    "R5",   "56",   "61",   "R5"  },   // PB0
        {  "35",    "K7",    "47",    "R4",   "57",   "62",   "R4"  },   // PB1
        {  "36",    "L7",    "48",    "M6",   "58",   "63",   "M5"  },   // PB2
        {  "89",    "B7",    "133",   "A10",  "161",  "192",  "A10" },   // PB3
        {  "90",    "C7",    "134",   "A9",   "162",  "193",  "A9"  },   // PB4
        {  "91",    "C8",    "135",   "A6",   "163",  "194",  "A8"  },   // PB5
        {  "92",    "A8",    "136",   "B6",   "164",  "195",  "B6"  },   // PB6
        {  "93",    "B8",    "137",   "B5",   "165",  "196",  "B5"  },   // PB7
        {  "95",    "A9",    "139",   "A5",   "167",  "198",  "A7"  },   // PB8
        {  "96",    "B9",    "140",   "B4",   "168",  "199",  "B4"  },   // PB9
        {  "46",    "M3",    "69",    "R12",  "79",   "90",   "P12" },   // PB10
        {  "47",    "N3",    "70",    "R13",  "80",   "91",   "R13" },   // PB11
        {  "51",    "M2",    "73",    "P12",  "92",   "104",  "L13" },   // PB12
        {  "52",    "N1",    "74",    "P13",  "93",   "105",  "K14" },   // PB13
        {  "53",    "K3",    "75",    "R14",  "94",   "106",  "R14" },   // PB14
        {  "54",    "J3",    "76",    "R15",  "95",   "107",  "R15" },   // PB15
    },
    {
        // LQFP100  WLCSP143 LQFP144 UFBGA176 LQFP176 LQFP208 TFBGA216   GPIO C
        {  "15",    "H8",    "26",    "M2",   "32",   "35",   "M2"  },   // PC0
        {  "16",    "K11",   "27",    "M3",   "33",   "36",   "M3"  },   // PC1
        {  "17",    "J10",   "28",    "M4",   "34",   "37",   "M4"  },   // PC2
        {  "18",    "J9",    "29",    "M5",   "35",   "38",   "L4"  },   // PC3
        {  "32",    "M8",    "44",    "N5",   "54",   "57",   "N5"  },   // PC4
        {  "33",    "N9",    "45",    "P5",   "55",   "58",   "P5"  },   // PC5
        {  "63",    "F2",    "96",    "H15",  "115",  "138",  "H15" },   // PC6
        {  "64",    "F3",    "97",    "G15",  "116",  "139",  "G15" },   // PC7
        {  "65",    "E4",    "98",    "G14",  "117",  "140",  "G14" },   // PC8
        {  "66",    "E3",    "99",    "F14",  "118",  "141",  "F14" },   // PC9
        {  "78",    "A2",    "111",   "B14",  "139",  "161",  "B14" },   // PC10
        {  "79",    "B2",    "112",   "B13",  "140",  "162",  "B13" },   // PC11
        {  "80",    "C3",    "113",   "A12",  "141",  "163",  "A12" },   // PC12
        {  "7",     "D10",   "7",     "D1",   "8",    "8",    "D1"  },   // PC13
        {  "8",     "D11",   "8",     "E1",   "9",    "9",    "E1"  },   // PC14
        {  "9",     "E11",   "9",     "F1",   "10",   "10",   "F1"  },   // PC15
    },
    {
        // LQFP100  WLCSP143 LQFP144 UFBGA176 LQFP176 LQFP208 TFBGA216   GPIO D
        {  "81",    "B3",    "114",   "B12",  "142",  "164",  "B12" },   // PD0
        {  "82",    "C4",    "115",   "C12",  "143",  "165",  "C12" },   // PD1
        {  "83",    "A3",    "116",   "D12",  "144",  "166",  "D12" },   // PD2
        {  "84",    "B4",    "117",   "D11",  "145",  "167",  "C11" },   // PD3
        {  "85",    "B5",    "118",   "D10",  "146",  "168",  "D11" },   // PD4
        {  "86",    "A4",    "119",   "C11",  "147",  "169",  "C10" },   // PD5
        {  "87",    "F4",    "122",   "B11",  "150",  "172",  "B11" },   // PD6
        {  "88",    "A5",    "123",   "A11",  "151",  "173",  "A11" },   // PD7
        {  "55",    "L2",    "77",    "P15",  "96",   "108",  "L15" },   // PD8
        {  "56",    "M1",    "78",    "P14",  "97",   "109",  "L14" },   // PD9
        {  "57",    "H4",    "79",    "N15",  "98",   "110",  "K15" },   // PD10
        {  "58",    "K2",    "80",    "N14",  "99",   "111",  "N10" },   // PD11
        {  "59",    "H6",    "81",    "N13",  "100",  "112",  "M10" },   // PD12
        {  "60",    "H5",    "82",    "M15",  "101",  "113",  "M11" },   // PD13
        {  "61",    "J2",    "85",    "M14",  "104",  "116",  "L12" },   // PD14
        {  "62",    "K1",    "86",    "L14",  "105",  "117",  "K13" },   // PD15
    },

    {
        // LQFP100  WLCSP143 LQFP144 UFBGA176 LQFP176 LQFP208 TFBGA216   GPIO E
        {  "97",    "B10",   "141",   "A4",   "169",  "200",  "A6"  },   // PE0
        {  "98",    "A10",   "142",   "A3",   "170",  "201",  "A5"  },   // PE1
        {  "1",     "D8",    "1",     "A2",   "1",    "1",    "A3"  },   // PE2
        {  "2",     "C10",   "2",     "A1",   "2",    "2",    "A2"  },   // PE3
        {  "3",     "B11",   "3",     "B1",   "3",    "3",    "A1"  },   // PE4
        {  "4",     "D9",    "4",     "B2",   "4",    "4",    "B1"  },   // PE5
        {  "5",     "E8",    "5",     "B3",   "5",    "5",    "B2"  },   // PE6
        {  "37",    "L5",    "58",    "R8",   "68",   "79",   "R8"  },   // PE7
        {  "38",    "M5",    "59",    "P8",   "69",   "80",   "N9"  },   // PE8
        {  "39",    "N5",    "60",    "P9",   "70",   "81",   "P9"  },   // PE9
        {  "40",    "J4",    "63",    "R9",   "73",   "84",   "R9"  },   // PE10
        {  "41",    "K4",    "64",    "P10",  "74",   "85",   "P10" },   // PE11
        {  "42",    "L4",    "65",    "R10",  "75",   "86",   "R10" },   // PE12
        {  "43",    "N4",    "66",    "N11",  "76",   "87",   "R12" },   // PE13
        {  "44",    "M4",    "67",    "P11",  "77",   "88",   "P11" },   // PE14
        {  "45",    "L3",    "68",    "R11",  "78",   "89",   "R11" },   // PE15
    },
    {
        // LQFP100  WLCSP143 LQFP144 UFBGA176 LQFP176 LQFP208 TFBGA216   GPIO F
        {  "-",     "F11",   "10",    "E2",   "16",   "16",   "D2"  },   // PF0
        {  "-",     "E9",    "11",    "H3",   "17",   "17",   "E2"  },   // PF1
        {  "-",     "F10",   "12",    "H2",   "18",   "18",   "G2"  },   // PF2
        {  "-",     "G11",   "13",    "J2",   "19",   "22",   "H2"  },   // PF3
        {  "-",     "F9",    "14",    "J3",   "20",   "23",   "J2"  },   // PF4
        {  "-",     "F8",    "15",    "K3",   "21",   "24",   "K3"  },   // PF5
        {  "-",     "G10",   "18",    "K2",   "24",   "27",   "K2"  },   // PF6
        {  "-",     "F7",    "19",    "K1",   "25",   "28",   "K1"  },   // PF7
        {  "-",     "H11",   "20",    "L3",   "26",   "29",   "L3"  },   // PF8
        {  "-",     "G8",    "21",    "L2",   "27",   "30",   "L2"  },   // PF9
        {  "-",     "G9",    "22",    "L1",   "28",   "31",   "L1"  },   // PF10
        {  "-",     "M7",    "49",    "R6",   "59",   "70",   "P8"  },   // PF11
        {  "-",     "N7",    "50",    "P6",   "60",   "71",   "M6"  },   // PF12
        {  "-",     "K6",    "53",    "N6",   "63",   "74",   "N6"  },   // PF13
        {  "-",     "L6",    "54",    "R7",   "64",   "75",   "P6"  },   // PF14
        {  "-",     "M6",    "55",    "P7",   "65",   "76",   "M8"  },   // PF15
    },
    {
        // LQFP100  WLCSP143 LQFP144 UFBGA176 LQFP176 LQFP208 TFBGA216   GPIO G
        {  "-",     "N6",    "56",    "N7",   "66",   "77",   "N7"  },   // PG0
        {  "-",     "K5",    "57",    "M7",   "67",   "78",   "M7"  },   // PG1
        {  "-",     "J1",    "87",    "L15",  "106",  "129",  "M13" },   // PG2
        {  "-",     "G3",    "88",    "K15",  "107",  "130",  "M12" },   // PG3
        {  "-",     "G5",    "89",    "K14",  "108",  "131",  "N12" },   // PG4
        {  "-",     "G6",    "90",    "K13",  "109",  "132",  "N11" },   // PG5
        {  "-",     "G4",    "91",    "J15",  "110",  "133",  "J15" },   // PG6
        {  "-",     "H1",    "92",    "J14",  "111",  "134",  "J14" },   // PG7
        {  "-",     "G2",    "93",    "H14",  "112",  "135",  "H14" },   // PG8
        {  "-",     "E5",    "124",   "C10",  "152",  "178",  "D9"  },   // PG9
        {  "-",     "C6",    "125",   "B10",  "153",  "179",  "C8"  },   // PG10
        {  "-",     "B6",    "126",   "B9",   "154",  "180",  "B8"  },   // PG11
        {  "-",     "A6",    "127",   "B8",   "155",  "181",  "C7"  },   // PG12
        {  "-",     "D6",    "128",   "A8",   "156",  "182",  "B3"  },   // PG13
        {  "-",     "F6",    "129",   "A7",   "157",  "183",  "A4"  },   // PG14
        {  "-",     "A7",    "132",   "B7",   "160",  "191",  "B7"  },   // PG15
    },
    {
        // LQFP100  WLCSP143 LQFP144 UFBGA176 LQFP176 LQFP208 TFBGA216   GPIO H
        {  "12",    "J11",   "23",    "G1",   "29",   "32",   "G1"  },   // PH0
        {  "13",    "H10",   "24",    "H1",   "30",   "33",   "H1"  },   // PH1
        {  "-",     "-",     "-",     "F4",   "43",   "46",   "K4"  },   // PH2
        {  "-",     "-",     "-",     "G4",   "44",   "47",   "J4"  },   // PH3
        {  "-",     "-",     "-",     "H4",   "45",   "48",   "H4"  },   // PH4
        {  "-",     "-",     "-",     "J4",   "46",   "49",   "J3"  },   // PH5
        {  "-",     "-",     "-",     "M11",  "83",   "96",   "P13" },   // PH6
        {  "-",     "-",     "-",     "N12",  "84",   "97",   "N13" },   // PH7
        {  "-",     "-",     "-",     "M12",  "85",   "98",   "P14" },   // PH8
        {  "-",     "-",     "-",     "M13",  "86",   "99",   "N14" },   // PH9
        {  "-",     "-",     "-",     "L13",  "87",   "100",  "P15" },   // PH10
        {  "-",     "-",     "-",     "L12",  "88",   "101",  "N15" },   // PH11
        {  "-",     "-",     "-",     "K12",  "89",   "102",  "M15" },   // PH12
        {  "-",     "-",     "-",     "E12",  "128",  "151",  "E12" },   // PH13
        {  "-",     "-",     "-",     "E13",  "129",  "152",  "E13" },   // PH14
        {  "-",     "-",     "-",     "D13",  "130",  "153",  "D13" },   // PH15
    },
    {
        // LQFP100  WLCSP143 LQFP144 UFBGA176 LQFP176 LQFP208 TFBGA216   GPIO I
        {  "-",     "-",     "-",     "E14",  "131",  "154",  "E14" },   // PI0
        {  "-",     "-",     "-",     "D14",  "132",  "155",  "D14" },   // PI1
        {  "-",     "-",     "-",     "C14",  "133",  "156",  "C14" },   // PI2
        {  "-",     "-",     "-",     "C13",  "134",  "157",  "C13" },   // PI3
        {  "-",     "-",     "-",     "D4",   "173",  "205",  "C3"  },   // PI4
        {  "-",     "-",     "-",     "C4",   "174",  "206",  "D3"  },   // PI5
        {  "-",     "-",     "-",     "C3",   "175",  "207",  "D6"  },   // PI6
        {  "-",     "-",     "-",     "C2",   "176",  "208",  "D4"  },   // PI7
        {  "-",     "-",     "-",     "D2",   "7",    "7",    "C2"  },   // PI8
        {  "-",     "-",     "-",     "D3",   "11",   "11",   "E4"  },   // PI9
        {  "-",     "-",     "-",     "E3",   "12",   "12",   "D5"  },   // PI10
        {  "-",     "-",     "-",     "E4",   "13",   "13",   "F3"  },   // PI11
        {  "-",     "-",     "-",     "-",    "-",    "19",   "E3"  },   // PI12
        {  "-",     "-",     "-",     "-",    "-",    "20",   "G3"  },   // PI13
        {  "-",     "-",     "-",     "-",    "-",    "21",   "H3"  },   // PI14
        {  "-",     "-",     "-",     "-",    "-",    "64",   "G4"  },   // PI15
    },
    {
        // LQFP100  WLCSP143 LQFP144 UFBGA176 LQFP176 LQFP208 TFBGA216   GPIO J
        {  "-",     "-",     "-",     "-",    "-",    "65",   "R6"  },   // PJ0
        {  "-",     "-",     "-",     "-",    "-",    "66",   "R7"  },   // PJ1
        {  "-",     "-",     "-",     "-",    "-",    "67",   "P7"  },   // PJ2
        {  "-",     "-",     "-",     "-",    "-",    "68",   "N8"  },   // PJ3
        {  "-",     "-",     "-",     "-",    "-",    "69",   "M9"  },   // PJ4
        {  "-",     "-",     "-",     "-",    "-",    "95 ",  "M14" },   // PJ5
        {  "-",     "-",     "-",     "-",    "-",    "118",  "K12" },   // PJ6
        {  "-",     "-",     "-",     "-",    "-",    "119",  "J12" },   // PJ7
        {  "-",     "-",     "-",     "-",    "-",    "120",  "H12" },   // PJ8
        {  "-",     "-",     "-",     "-",    "-",    "121",  "J13" },   // PJ9
        {  "-",     "-",     "-",     "-",    "-",    "122",  "H13" },   // PJ10
        {  "-",     "-",     "-",     "-",    "-",    "123",  "G12" },   // PJ11
        {  "-",     "-",     "-",     "-",    "-",    "174",  "B10" },   // PJ12
        {  "-",     "-",     "-",     "-",    "-",    "175",  "B9"  },   // PJ13
        {  "-",     "-",     "-",     "-",    "-",    "176",  "C9"  },   // PJ14
        {  "-",     "-",     "-",     "-",    "-",    "177",  "D10" },   // PJ15
    },
    {
        // LQFP100  WLCSP143 LQFP144 UFBGA176 LQFP176 LQFP208 TFBGA216   GPIO K
        {  "-",     "-",     "-",     "-",    "-",    "126",  "G13" },   // PK0
        {  "-",     "-",     "-",     "-",    "-",    "127",  "F12" },   // PK1
        {  "-",     "-",     "-",     "-",    "-",    "128",  "F13" },   // PK2
        {  "-",     "-",     "-",     "-",    "-",    "186",  "D8"  },   // PK3
        {  "-",     "-",     "-",     "-",    "-",    "187",  "D7"  },   // PK4
        {  "-",     "-",     "-",     "-",    "-",    "188",  "C6"  },   // PK5
        {  "-",     "-",     "-",     "-",    "-",    "189",  "C5"  },   // PK6
        {  "-",     "-",     "-",     "-",    "-",    "190",  "C4"  },   // PK7
        {  "-",     "-",     "-",     "-",    "-",    "-",    "-"   },   // PK8
        {  "-",     "-",     "-",     "-",    "-",    "-",    "-"   },   // PK9
        {  "-",     "-",     "-",     "-",    "-",    "-",    "-"   },   // PK10
        {  "-",     "-",     "-",     "-",    "-",    "-",    "-"   },   // PK11
        {  "-",     "-",     "-",     "-",    "-",    "-",    "-"   },   // PK12
        {  "-",     "-",     "-",     "-",    "-",    "-",    "-"   },   // PK13
        {  "-",     "-",     "-",     "-",    "-",    "-",    "-"   },   // PK14
        {  "-",     "-",     "-",     "-",    "-",    "-",    "-"   },   // PK15
    },
};
