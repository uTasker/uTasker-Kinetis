/**********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher

    ---------------------------------------------------------------------
    File:      STM32F105X_107X_port.h
    Project:   Single Chip Embedded Internet
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2017
    *********************************************************************/

#if PIN_COUNT == PIN_COUNT_64_PIN
    #define _PIN_COUNT              1
#elif PIN_COUNT == PIN_COUNT_100_PIN
    #if PACKAGE_TYPE == PACKAGE_LQFP
        #define _PIN_COUNT          2
    #else
        #define _PIN_COUNT          0
    #endif
#else
    #error "Package not clearly defined!!"
#endif

#define ALTERNATIVE_FUNCTIONS   6                                        // GPIO plus 6 possible peripheral functions


static const char *cPer[PORTS_AVAILABLE][PORT_WIDTH][1 + ALTERNATIVE_FUNCTIONS] = {
    {
#if defined _CONNECTIVITY_LINE                                           // connectivity line
        //                                                               GPIO A
        {  "PA0", "ETH_MII_CRS_WKUP","TIM5_CH1",   "TIM2_CH1",   "USART2_CTS", "TIM2_ETR", "ADC12_IN0" }, // PA0
        {  "PA1", "ETH_MII_RX_CLK",  "TIM5_CH2",   "TIM2_CH2",   "USART2_RTS", "-",        "ADC12_IN1" }, // PA1
        {  "PA2", "ETH_MII_MDIO",    "TIM5_CH3",   "TIM2_CH3",   "USART2_TX",  "-",        "ADC12_IN2" }, // PA2
        {  "PA3", "ETH_MII_COL",     "TIM5_CH4",   "TIM2_CH4",   "USART2_RX",  "-",        "ADC12_IN3" }, // PA3
        {  "PA4", "SPI1_NSS",        "SPI3_NSS",   "I2S3_WS",    "USART2_CK",  "DAC_OUT1", "ADC12_IN4" }, // PA4
        {  "PA5", "SPI1_SCK",        "-",          "-",          "-",          "DAC_OUT2", "ADC12_IN5" }, // PA5
        {  "PA6", "SPI1_MISO",       "TIM3_CH1",   "TIM1_BKIN",  "-",          "-",        "ADC12_IN6" }, // PA6
        {  "PA7", "SPI1_MOSI",       "TIM3_CH2",   "TIM1_CH1N",  "ETH_MII_RX_DV","-",      "ADC12_IN7" }, // PA7
        {  "PA8", "MCO",             "TIM1_CH1",   "OTG_FS_SOF", "USART1_CK",  "-",        "-"         }, // PA8
        {  "PA9", "TIM1_CH2",        "USART1_TX",  "-",          "OTG_FS_VBUS","-",        "-"         }, // PA9
        {  "PA10","TIM1_CH3",        "USART1_RX",  "-",          "OTG_FS_ID",  "-",        "-"         }, // PA10
        {  "PA11","TIM1_CH4",        "USART1_CTS", "CAN1_RX",    "OTG_FS_DM",  "-",        "-"         }, // PA11
        {  "PA12","TIM1_ETR",        "USART1_RTS", "CAN1_TX",    "OTG_FS_DP",  "-",        "-"         }, // PA12
        {  "PA13","-",               "-",          "-",          "JTMS-SWDIO", "-",        "-"         }, // PA13
        {  "PA14","-",               "-",          "-",          "JTCK-SWCLK", "-",        "-"         }, // PA14
        {  "PA15","SPI1_NSS",        "SPI3_NSS",   "TIM2_CH1_ETR","JTDI",      "I2S3_WS",  "-"         }  // PA15
    },
    {
        //                                                               GPIO B
        {  "PB0", "ETH_MII_RXD2",   "TIM3_CH3",    "TIM1_CH2N",  "-",          "-",        "ADC12_IN8" }, // PB0
        {  "PB1", "ETH_MII_RXD3",   "TIM3_CH4",    "-",          "-",          "-",        "ADC12_IN9" }, // PB1
        {  "PB2", "-",              "-",           "-",          "BOOT1",      "-",        "-"         }, // PB2
        {  "PB3", "TIM2_CH2",       "SPI3_SCK",    "I2S3_CK",    "JTDO",       "TRACESWO", "SPI1_SCK"  }, // PB3
        {  "PB4", "TIM3_CH1",       "SPI1_MISO",   "SPI3_MISO",  "NJTRST",     "-",        "-"         }, // PB4
        {  "PB5", "ETH_MII_PPS_OUT","TIM3_CH2",    "SPI1_MOSI",  "SPI3_MOSI",  "I2C1_SMBA/I2S3_SD",  "CAN2_RX" }, // PB5
        {  "PB6", "I2C1_SCL",       "USART1_TX",   "TIM4_CH1",   "-",          "-",        "CAN2_TX"   }, // PB6
        {  "PB7", "I2C1_SDA",       "USART1_RX",   "TIM4_CH2",   "-",          "-",        "-"         }, // PB7
        {  "PB8", "ETH_MII_TXD3",   "-",           "TIM4_CH3",   "I2C1_SCL",   "-",        "CAN1_RX"   }, // PB8
        {  "PB9", "-",              "-",           "TIM4_CH4",   "I2C1_SDA",   "-",        "CAN1_TX"   }, // PB9
        {  "PB10","ETH_MII_RX_ER",  "USART3_TX",   "TIM2_CH3",   "I2C2_SCL",   "-",        "-"         }, // PB10
        {  "PB11","ETH_MII_TX_EN",  "USART3_RX",   "TIM2_CH4",   "I2C2_SDA",   "-",        "-"         }, // PB11
        {  "PB12","ETH_MII_TXD0",   "USART3_CK",   "TIM1_BKIN",  "SPI2_NSS",   "I2S2_WS/I2C2_SMBA", "CAN2_RX" }, // PB12
        {  "PB13","ETH_MII_TXD1",   "USART3_CTS",  "TIM1_CH1N",  "SPI2_SCK",   "I2S2_CK",  "CAN2_TX"   }, // PB13
        {  "PB14","SPI2_MISO",      "USART3_RTS",  "TIM1_CH2N",  "-",          "-",        "-"         }, // PB14
        {  "PB15","-",              "-",           "TIM1_CH3N",  "SPI2_MOSI",  "I2S2_SD",  "-"         }  // PB15
    },
    {
        //                                                               GPIO C
        {  "PC0", "-",              "-",           "-",          "-",          "-",        "ADC12_IN10"}, // PC0
        {  "PC1", "ETH_MII_MDC",    "-",           "-",          "-",          "-",        "ADC12_IN11"}, // PC1
        {  "PC2", "ETH_MII_TXD2",   "-",           "-",          "-",          "-",        "ADC12_IN12"}, // PC2
        {  "PC3", "ETH_MII_TX_CLK", "-",           "-",          "-",          "-",        "ADC12_IN13"}, // PC3
        {  "PC4", "ETH_MII_RXD0",   "-",           "-",          "-",          "-",        "ADC12_IN14"}, // PC4
        {  "PC5", "ETH_MII_RXD1",   "-",           "-",          "-",          "-",        "ADC12_IN15"}, // PC5
        {  "PC6", "TIM3_CH1",       "I2S2_MCK",    "-",          "-",          "-",        "-"         }, // PC6
        {  "PC7", "TIM3_CH2",       "I2S3_MCK",    "-",          "-",          "-",        "-"         }, // PC7
        {  "PC8", "TIM3_CH3",       "-",           "-",          "-",          "-",        "-"         }, // PC8
        {  "PC9", "TIM3_CH4",       "-",           "-",          "-",          "-",        "-"         }, // PC9
        {  "PC10","USART3_TX",      "UART4_TX",    "SPI3_SCK",   "I2S3_CK",    "-",        "-"         }, // PC10
        {  "PC11","USART3_RX",      "UART4_RX",    "SPI3_MISO",  "-",          "-",        "-"         }, // PC11
        {  "PC12","USART3_CK",      "UART5_TX",    "SPI3_MOSI",  "I2S3_SD",    "-",        "-"         }, // PC12
        {  "PC13","-",              "TAMPER-RTC",  "-",          "-",          "-",        "-"         }, // PC13
        {  "PC14","-",              "OSC32_IN",    "-",          "-",          "-",        "-"         }, // PC14
        {  "PC15","-",              "OSC32_OUT",   "-",          "-",          "-",        "-"         }  // PC15
    },
#else                                                                    // value line (64 pin)
        //                                                               GPIO A
        {  "PA0", "-",              "-",           "-",          "USART2_CTS", "TIM2_CH1_ETR","ADC1_IN0" }, // PA0
        {  "PA1", "-",              "-",           "TIM2_CH2",   "USART2_RTS", "-",        "ADC1_IN1"  }, // PA1
        {  "PA2", "-",              "-",           "TIM2_CH3",   "USART2_TX",  "TIM15_CH1","ADC1_IN2"  }, // PA2
        {  "PA3", "-",              "-",           "TIM2_CH4",   "USART2_RX",  "TIM15_CH2","ADC1_IN3"  }, // PA3
        {  "PA4", "SPI1_NSS",       "-",           "-",          "USART2_CK",  "DAC1_OUT", "ADC1_IN4"  }, // PA4
        {  "PA5", "SPI1_SCK",       "-",           "-",          "-",          "DAC2_OUT", "ADC1_IN5"  }, // PA5
        {  "PA6", "SPI1_MISO",      "TIM3_CH1",    "TIM1_BKIN",  "-",          "TIM16_CH1","ADC1_IN6"  }, // PA6
        {  "PA7", "SPI1_MOSI",      "TIM3_CH2",    "TIM1_CH1N",  "-",          "TIM17_CH1","ADC1_IN7"  }, // PA7
        {  "PA8", "MCO",            "TIM1_CH1",    "-",          "USART1_CK",  "-",        "-"         }, // PA8
        {  "PA9", "TIM1_CH2",       "USART1_TX",   "-",          "-",          "TIM15_BKIN","-"        }, // PA9
        {  "PA10","TIM1_CH3",       "USART1_RX",   "-",          "-",          "TIM17_BKIN","-"        }, // PA10
        {  "PA11","TIM1_CH4",       "USART1_CTS",  "-",          "-",          "-",        "-"         }, // PA11
        {  "PA12","TIM1_ETR",       "USART1_RTS",  "-",          "-",          "-",        "-"         }, // PA12
        {  "PA13","-",              "-",           "-",          "JTMS-SWDIO", "-",        "-"         }, // PA13
        {  "PA14","-",              "-",           "-",          "JTCK-SWCLK", "-",        "-"         }, // PA14
        {  "PA15","-",              "-",           "TIM2_CH1_ETR","JTDI",      "-",        "-"         }  // PA15
    },
    {
        //                                                               GPIO B
        {  "PB0", "-",              "TIM3_CH3",    "TIM1_CH2N",  "-",          "-",        "ADC1_IN8"  }, // PB0
        {  "PB1", "-",              "TIM3_CH4",    "-",          "-",          "TIM1_CH3N","ADC1_IN9"  }, // PB1
        {  "PB2", "-",              "-",           "-",          "BOOT1",      "-",        "-"         }, // PB2
        {  "PB3", "-",              "-",           "-",          "JTDO",       "TRACESWO", "SPI1_SCK"  }, // PB3
        {  "PB4", "TIM3_CH1",       "SPI1_MISO",   "-",          "NJTRST",     "-",        "-"         }, // PB4
        {  "PB5", "-",              "TIM3_CH2",    "SPI1_MOSI",  "TIM16_BKIN", "I2C1_SMBA","-"         }, // PB5
        {  "PB6", "I2C1_SCL",       "USART1_TX",   "TIM4_CH1",   "TIM16_CH1N", "-",        "-"         }, // PB6
        {  "PB7", "I2C1_SDA",       "USART1_RX",   "TIM4_CH2",   "TIM17_CH1N", "-",        "-"         }, // PB7
        {  "PB8", "-",              "-",           "TIM4_CH3",   "I2C1_SCL",   "TIM16_CH1","CEC"       }, // PB8
        {  "PB9", "-",              "-",           "TIM4_CH4",   "I2C1_SDA",   "TIM17_CH1","-"         }, // PB9
        {  "PB10","-",              "USART3_TX",   "TIM2_CH3",   "I2C2_SCL",   "-",        "CEC"       }, // PB10
        {  "PB11","-",              "USART3_RX",   "TIM2_CH4",   "I2C2_SDA",   "-",        "-"         }, // PB11
        {  "PB12","-",              "USART3_CK",   "TIM1_BKIN",  "SPI2_NSS",   "I2C2_SMBA", "-"        }, // PB12
        {  "PB13","-",              "USART3_CTS",  "TIM1_CH1N",  "SPI2_SCK",   "-",        "-"         }, // PB13
        {  "PB14","SPI2_MISO",      "USART3_RTS",  "TIM1_CH2N",  "-",          "-",        "-"         }, // PB14
        {  "PB15","-",              "-",           "TIM1_CH3N",  "SPI2_MOSI",  "TIM15_CH1N","TIM15_CH2"}  // PB15
    },
    {
        //                                                               GPIO C
        {  "PC0", "-",              "-",           "-",          "-",          "-",        "ADC1_IN10" }, // PC0
        {  "PC1", "-",              "-",           "-",          "-",          "-",        "ADC1_IN11" }, // PC1
        {  "PC2", "-",              "-",           "-",          "-",          "-",        "ADC1_IN12" }, // PC2
        {  "PC3", "-",              "-",           "-",          "-",          "-",        "ADC1_IN13" }, // PC3
        {  "PC4", "-",              "-",           "-",          "-",          "-",        "ADC1_IN14" }, // PC4
        {  "PC5", "-",              "-",           "-",          "-",          "-",        "ADC1_IN15" }, // PC5
        {  "PC6", "TIM3_CH1",       "-",           "-",          "-",          "-",        "-"         }, // PC6
        {  "PC7", "TIM3_CH2",       "-",           "-",          "-",          "-",        "-"         }, // PC7
        {  "PC8", "TIM3_CH3",       "-",           "-",          "-",          "-",        "-"         }, // PC8
        {  "PC9", "TIM3_CH4",       "-",           "-",          "-",          "-",        "-"         }, // PC9
        {  "PC10","USART3_TX",      "-",           "-",          "-",          "-",        "-"         }, // PC10
        {  "PC11","USART3_RX",      "-",           "-",          "-",          "-",        "-"         }, // PC11
        {  "PC12","USART3_CK",      "-",           "-",          "-",          "-",        "-"         }, // PC12
        {  "PC13","-",              "TAMPER-RTC",  "-",          "-",          "-",        "-"         }, // PC13
        {  "PC14","-",              "OSC32_IN",    "-",          "-",          "-",        "-"         }, // PC14
        {  "PC15","-",              "OSC32_OUT",   "-",          "-",          "-",        "-"         }  // PC15
    },
#endif
#if PIN_COUNT == PIN_COUNT_100_PIN
    {
        //                                                               GPIO D
        {  "PD0", "-",              "-",           "OSC_IN",     "CAN1_RX",    "-",        "-"         }, // PD0
        {  "PD1", "-",              "-",           "OSC_OUT",    "CAN1_TX",    "-",        "-"         }, // PD1
        {  "PD2", "UART5_RX",       "TIM3_ETR",    "-",          "-",          "-",        "-"         }, // PD2
        {  "PD3", "-",              "USART2_CTS",  "-",          "-",          "-",        "-"         }, // PD3
        {  "PD4", "-",              "USART2_RTS",  "-",          "-",          "-",        "-"         }, // PD4
        {  "PD5", "-",              "USART2_TX",   "-",          "-",          "-",        "-"         }, // PD5
        {  "PD6", "-",              "USART2_RX",   "-",          "-",          "-",        "-"         }, // PD6
        {  "PD7", "-",              "USART2_CK",   "-",          "-",          "-",        "-"         }, // PD7
        {  "PD8", "ETH_MII_RX_DV",  "USART3_TX",   "-",          "-",          "-",        "-"         }, // PD8
        {  "PD9", "ETH_MII_RXD0",   "USART3_RX",   "-",          "-",          "-",        "-"         }, // PD9
        {  "PD10","ETH_MII_RXD1",   "USART3_CK",   "-",          "-",          "-",        "-"         }, // PD10
        {  "PD11","ETH_MII_RXD2",   "USART3_CTS",  "-",          "-",          "-",        "-"         }, // PD11
        {  "PD12","ETH_MII_RXD3",   "USART3_RTS",  "TIM4_CH1",   "-",          "-",        "-"         }, // PD12
        {  "PD13","-",              "-",           "TIM4_CH2",   "-",          "-",        "-"         }, // PD13
        {  "PD14","-",              "-",           "TIM4_CH3",   "-",          "-",        "-"         }, // PD14
        {  "PD15","-",              "-",           "TIM4_CH4",   "-",          "-",        "-"         }  // PD15
    },
    {
        // GPIO E
        {  "PE0", "TIM4_ETR",       "-",           "-",          "-",          "-",        "-"         }, // PE0
        {  "PE1", "-",              "-",           "-",          "-",          "-",        "-"         }, // PE1
        {  "PE2", "-",              "-",           "-",          "-",          "TRACECK",  "-"         }, // PE2
        {  "PE3", "-",              "-",           "-",          "-",          "TRACED0",  "-"         }, // PE3
        {  "PE4", "-",              "-",           "-",          "-",          "TRACED1",  "-"         }, // PE4
        {  "PE5", "-",              "-",           "-",          "-",          "TRACED2",  "-"         }, // PE5
        {  "PE6", "-",              "-",           "-",          "-",          "TRACED3",  "-"         }, // PE6
        {  "PE7", "TIM1_ETR",       "-",           "-",          "-",          "-",        "-"         }, // PE7
        {  "PE8", "TIM1_CH1N",      "-",           "-",          "-",          "-",        "-"         }, // PE8
        {  "PE9", "TIM1_CH1",       "-",           "-",          "-",          "-",        "-"         }, // PE9
        {  "PE10","TIM1_CH2N",      "-",           "-",          "-",          "-",        "-"         }, // PE10
        {  "PE11","TIM1_CH2",       "-",           "-",          "-",          "-",        "-"         }, // PE11
        {  "PE12","TIM1_CH3N",      "-",           "-",          "-",          "-",        "-"         }, // PE12
        {  "PE13","TIM1_CH3",       "-",           "-",          "-",          "-",        "-"         }, // PE13
        {  "PE14","TIM1_CH4",       "-",           "-",          "-",          "-",        "-"         }, // PE14
        {  "PE15","TIM1_BKIN",      "-",           "-",          "-",          "-",        "-"         }  // PE15
    }
#endif
};


static const char *cPinNumber[PORTS_AVAILABLE][PORT_WIDTH][3] = {
    {
        //  BGA100  LQFP64  LQFP100                                      GPIO A
        {  "G2",    "14",    "23"  },                                    // PA0
        {  "H2",    "15",    "24"  },                                    // PA1
        {  "J2",    "16",    "25"  },                                    // PA2
        {  "K2",    "17",    "26"  },                                    // PA3
        {  "G3",    "20",    "29"  },                                    // PA4
        {  "H3",    "21",    "30"  },                                    // PA5
        {  "J3",    "22",    "31"  },                                    // PA6
        {  "K3",    "23",    "32"  },                                    // PA7
        {  "D9",    "41",    "67"  },                                    // PA8
        {  "C9",    "42",    "68"  },                                    // PA9
        {  "D10",   "43",    "69"  },                                    // PA10
        {  "C10",   "44",    "70"  },                                    // PA11
        {  "B10",   "45",    "71"  },                                    // PA12
        {  "A10",   "46",    "72"  },                                    // PA13
        {  "A9",    "49",    "76"  },                                    // PA14
        {  "A8",    "50",    "77"  },                                    // PA15
    },

    {
         //  BGA100  LQFP64  LQFP100                                     GPIO B
        {  "J4",    "26",    "35"  },                                    // PB0
        {  "K4",    "27",    "36"  },                                    // PB1
        {  "G5",    "28",    "37"  },                                    // PB2
        {  "A7",    "55",    "89"  },                                    // PB3
        {  "A6",    "56",    "90"  },                                    // PB4
        {  "C5",    "57",    "91"  },                                    // PB5
        {  "B5",    "58",    "92"  },                                    // PB6
        {  "A5",    "59",    "93"  },                                    // PB7
        {  "B4",    "61",    "95"  },                                    // PB8
        {  "A4",    "62",    "96"  },                                    // PB9
        {  "J7",    "29",    "47"  },                                    // PB10
        {  "K7",    "30",    "48"  },                                    // PB11
        {  "K8",    "33",    "51"  },                                    // PB12
        {  "J8",    "34",    "52"  },                                    // PB13
        {  "H8",    "35",    "53"  },                                    // PB14
        {  "G8",    "36",    "54"  },                                    // PB15
    },

    {
         //  BGA100  LQFP64  LQFP100                                     GPIO C
        {  "F1",    "8",     "15"  },                                    // PC0
        {  "F2",    "9",     "16"  },                                    // PC1
        {  "E2",    "10",    "17"  },                                    // PC2
        {  "F3",    "11",    "18"  },                                    // PC3
        {  "G4",    "24",    "33"  },                                    // PC4
        {  "H4",    "25",    "34"  },                                    // PC5
        {  "F10",   "37",    "63"  },                                    // PC6
        {  "E10",   "38",    "64"  },                                    // PC7
        {  "F9",    "39",    "65"  },                                    // PC8
        {  "E9",    "40",    "66"  },                                    // PC9
        {  "B9",    "51",    "78"  },                                    // PC10
        {  "B8",    "52",    "79"  },                                    // PC11
        {  "C8",    "53",    "80"  },                                    // PC12
        {  "A2",    "2",     "7"   },                                    // PC13
        {  "A1",    "3",     "8"   },                                    // PC14
        {  "B1",    "4",     "9"   },                                    // PC15
    },
#if PORTS_AVAILABLE > 3
    {
         //  BGA100  LQFP64  LQFP100                                     GPIO D
        {  "D8",    "5",     "81"  },                                    // PD0
        {  "E8",    "6",     "83"  },                                    // PD1
        {  "B7",    "54",    "83"  },                                    // PD2
        {  "C7",    "-",     "84"  },                                    // PD3
        {  "D7",    "-",     "85"  },                                    // PD4
        {  "B6",    "-",     "86"  },                                    // PD5
        {  "C6",    "-",     "87"  },                                    // PD6
        {  "D6",    "-",     "88"  },                                    // PD7
        {  "K9",    "-",     "55"  },                                    // PD8
        {  "J9",    "-",     "56"  },                                    // PD9
        {  "H9",    "-",     "57"  },                                    // PD10
        {  "G9",    "-",     "58"  },                                    // PD11
        {  "K10",   "-",     "59"  },                                    // PD12
        {  "J10",   "-",     "60"  },                                    // PD13
        {  "H10",   "-",     "61"  },                                    // PD14
        {  "G10",   "-",     "62"  },                                    // PD15
    },

    {
         //  BGA100  LQFP64  LQFP100                                     GPIO E
        {  "D4",    "-",     "97"  },                                    // PE0
        {  "C4",    "-",     "98"  },                                    // PE1
        {  "A3",    "-",     "1"   },                                    // PE2
        {  "B3",    "-",     "2"   },                                    // PE3
        {  "C3",    "-",     "3"   },                                    // PE4
        {  "D3",    "-",     "4"   },                                    // PE5
        {  "E3",    "-",     "5"   },                                    // PE6
        {  "H5",    "-",     "38"  },                                    // PE7
        {  "J5",    "-",     "39"  },                                    // PE8
        {  "K5",    "-",     "40"  },                                    // PE9
        {  "G6",    "-",     "41"  },                                    // PE10
        {  "H6",    "-",     "42"  },                                    // PE11
        {  "J6",    "-",     "43"  },                                    // PE12
        {  "K6",    "-",     "44"  },                                    // PE13
        {  "G7",    "-",     "45"  },                                    // PE14
        {  "H7",    "-",     "46"  },                                    // PE15
    },
#endif
};