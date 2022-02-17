/***********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher

    ---------------------------------------------------------------------
    File:      kinetis_port_ke.h
    Project:   Single Chip Embedded Internet
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2019
    *********************************************************************
    Supports KEA08, KEA64, KEA128, KE02, KE04, KE06, KE15 and KE18F (port A complete)

*/



#if defined KINETIS_KE02

#if PIN_COUNT == PIN_COUNT_32_PIN                                        // 32 pin LQFP
    #define _PIN_COUNT      0
#elif PIN_COUNT == PIN_COUNT_44_PIN                                      // 32 pin LQFP
    #define _PIN_COUNT      1
#elif PIN_COUNT == PIN_COUNT_64_PIN                                      // 64 pin LQFP/QFP
    #define _PIN_COUNT      2
#else
    #error "Package not clearly defined!!"
#endif

#define ALTERNATIVE_FUNCTIONS 4

static const char *cPinNumber[PORTS_AVAILABLE_8_BIT][PORT_WIDTH][3] = {
    {
        //  LQFP32  LQFP44  LQFP64/QFP                                   GPIO A
        {  "26",    "35",   "50"   },                                    // PA0
        {  "25",    "34",   "49"   },                                    // PA1
        {  "24",    "33",   "48"   },                                    // PA2
        {  "23",    "32",   "47"   },                                    // PA3
        {  "32",    "44",   "64"   },                                    // PA4
        {  "31",    "43",   "63"   },                                    // PA5
        {  "20",    "26",   "38"   },                                    // PA6
        {  "19",    "25",   "37"   },                                    // PA7
    },
    {
        {  "18",    "24",   "34"  },                                     // PB0
        {  "17",    "23",   "33"  },                                     // PB1
        {  "16",    "22",   "32"  },                                     // PB2
        {  "15",    "21",   "31"  },                                     // PB3
        {  "10",    "13",   "19"  },                                     // PB4
        {  "9",     "12",   "18"  },                                     // PB5
        {  "8",     "10",   "12"  },                                     // PB6
        {  "7",     "9",    "11"  },                                     // PB7
    },
    {
        {  "14",    "20",   "26"  },                                     // PC0
        {  "13",    "19",   "25"  },                                     // PC1
        {  "12",    "15",   "21"  },                                     // PC2
        {  "11",    "14",   "20"  },                                     // PC3
        {  "30",    "42",   "62"  },                                     // PC4
        {  "29",    "41",   "61"  },                                     // PC5
        {  "28",    "37",   "52"  },                                     // PC6
        {  "27",    "36",   "51"  },                                     // PC7
    },
    {
        {  "2",     "2",    "2"   },                                     // PD0
        {  "1",     "1",    "1"   },                                     // PD1
        {  "22",    "31",   "46"  },                                     // PD2
        {  "21",    "30",   "45"  },                                     // PD3
        {  "-",     "29",   "44"  },                                     // PD4
        {  "-",     "18",   "24"  },                                     // PD5
        {  "-",     "17",   "23"  },                                     // PD6
        {  "-",     "16",   "22"  },                                     // PD7
    },
    {
        {  "-",     "40",   "60"  },                                     // PE0
        {  "-",     "39",   "59"  },                                     // PE1
        {  "-",     "38",   "54"  },                                     // PE2
        {  "-",     "-",    "53"  },                                     // PE3
        {  "-",     "-",    "39"  },                                     // PE4
        {  "-",     "-",    "17"  },                                     // PE5
        {  "-",     "-",    "16"  },                                     // PE6
        {  "-",     "3",    "5"   },                                     // PE7
    },
    {
        {  "-",     "-",    "43"  },                                     // PF0
        {  "-",     "-",    "42"  },                                     // PF1
        {  "-",     "-",    "36"  },                                     // PF2
        {  "-",     "-",    "35"  },                                     // PF3
        {  "-",     "-",    "30"  },                                     // PF4
        {  "-",     "-",    "29"  },                                     // PF5
        {  "-",     "-",    "28"  },                                     // PF6
        {  "-",     "-",    "27"  },                                     // PF7
    },
    {
        {  "-",     "-",    "58"  },                                     // PG0
        {  "-",     "-",    "57"  },                                     // PG1
        {  "-",     "-",    "56"  },                                     // PG2
        {  "-",     "-",    "55"  },                                     // PG3
        {  "-",     "-",    "-"   },                                     // PG4
        {  "-",     "-",    "-"   },                                     // PG5
        {  "-",     "-",    "-"   },                                     // PG6
        {  "-",     "-",    "-"   },                                     // PG7
    },
    {
        {  "-",     "-",    "15"  },                                     // PH0
        {  "-",     "-",    "14"  },                                     // PH1
        {  "-",     "4",    "6"   },                                     // PH2
        {  "-",     "-",    "-"   },                                     // PH3
        {  "-",     "-",    "-"   },                                     // PH4
        {  "-",     "-",    "-"   },                                     // PH5
        {  "-",     "-",    "4"   },                                     // PH6
        {  "-",     "-",    "3"   },                                     // PH7
    },
};

static int ADC_DEDICATED_CHANNEL[PORT_WIDTH] = {0, 0, 0, 0, 0, 0, 0, 0};
static int ADC_DEDICATED_MODULE[PORT_WIDTH] = {0, 0, 0, 0, 0, 0, 0, 0};
static int ADC_MUX_CHANNEL[PORTS_AVAILABLE_8_BIT][PORT_WIDTH] = {
    { -1,-1,-1,-1,-1,-1,-1,-1 }, // port A
    { -1,-1,-1,-1,-1,-1,-1,-1 }, // port B
#if PORTS_AVAILABLE_8_BIT > 2
    { -1,-1,-1,-1,-1,-1,-1,-1 }, // port C
#endif
#if PORTS_AVAILABLE_8_BIT > 3
    { -1,-1,-1,-1,-1,-1,-1,-1 }, // port D
#endif
#if PORTS_AVAILABLE_8_BIT > 4
    { -1,-1,-1,-1,-1,-1,-1,-1 }, // port E
#endif
#if PORTS_AVAILABLE_8_BIT > 5
    { -1,-1,-1,-1,-1,-1,-1,-1 }, // port F
#endif
#if PORTS_AVAILABLE_8_BIT > 6
    { -1,-1,-1,-1,-1,-1,-1,-1 }, // port G
#endif
#if PORTS_AVAILABLE_8_BIT > 7
    { -1,-1,-1,-1,-1,-1,-1,-1 }, // port H
#endif
#if PORTS_AVAILABLE_8_BIT > 8
    { -1,-1,-1,-1,-1,-1,-1,-1 }, // port I
#endif
};


static const char *cPer[PORTS_AVAILABLE_8_BIT][PORT_WIDTH * 4][5] = {
    {
        // GPIO            ALT 1           ALT 2             ALT3              ALT 4         (GPIO has lowest priority, ALT4 has highest)
        {  "PTA0",         "KBI0_P0",      "FTM0_CH0",       "ACMP0_IN0",      "ADC0_SE0"    }, // PTA0
        {  "PTA1",         "KBI0_P1",      "FTM0_CH1",       "ACMP0_IN1",      "ADC0_SE1"    }, // PTA1
        {  "PTA2",         "KBI0_P2",      "UART0_RX",       "I2C0_SDA",       "-"           }, // PTA2 (true open-drain output when operating as output)
        {  "PTA3",         "KBI0_P3",      "UART0_TX",       "I2C0_SCL",       "-"           }, // PTA3 (true open-drain output when operating as output)
        {  "PTA4",         "-",            "ACMP0_OUT",      "-",              "SWD_DIO"     }, // PTA4 (SWD_DIO default)
        {  "PTA5",         "IRQ",          "FTM0_CLK",       "-",              "RESET"       }, // PTA5 (RESET default)
        {  "PTA6",         "-",            "FTM2_FLT1",      "ACMP1_IN0",      "ADC0_SE2"    }, // PTA6
        {  "PTA7",         "-",            "FTM0_FLT2",      "ACMP1_IN1",      "ADC0_SE3"    }, // PTA7
    },
    {
        {  "PTB0",         "KBI0_P4",      "UART0_RX",       "-",              "ADC0_SE4"    }, // PTB0
        {  "PTB1",         "KBI0_P5",      "UART0_TX",       "-",              "ADC0_SE5"    }, // PTB1
        {  "PTB2",         "KBI0_P6",      "SPI0_SCK",       "FTM0_CH0",       "ADC0_SE6"    }, // PTB2
        {  "PTB3",         "KBI0_P7",      "SPI0_MOSI",      "FTM0_CH1",       "ADC0_SE7"    }, // PTB3
        {  "PTB4",         "FTM2_CH4",     "SPI0_MISO",      "NMI",            "ACMP1_IN2"   }, // PTB4 (NMI default) (high current drive pin when operated as an output)
        {  "PTB5",         "FTM2_CH5",     "SPI0_PCS0",      "ACMP1_OUT",      "-"           }, // PTB5 (high current drive pin when operated as an output)
        {  "PTB6",         "-",            "I2C0_SDA",       "-",              "XTAL"        }, // PTB6
        {  "PTB7",         "-",            "I2C0_SCL",       "-",              "EXTAL"       }, // PTB7
    },
    {
        {  "PTC0",         "-",            "FTM2_CH0",       "-",              "ADC0_SE8"    }, // PTC0
        {  "PTC1",         "-",            "FTM2_CH1",       "-",              "ADC0_SE9"    }, // PTC1
        {  "PTC2",         "FTM2_CH2",     "-",              "-",              "ADC0_SE10"   }, // PTC2
        {  "PTC3",         "FTM2_CH3",      "-",              "-",             "ADC0_SE11"   }, // PTC3
        {  "PTC4",         "RTCO",         "FTM1_CH0",       "ACMP0_IN2",      "SWD_CLK"     }, // PTC4 (SWD_CLK default)
        {  "PTC5",         "-",            "FTM1_CH1",       "-",              "RTCO"        }, // PTC5
        {  "PTC6",         "-",            "UART1_RX",       "-",              "-"           }, // PTC6
        {  "PTC7",         "-",            "UART1_TX",       "-",              "-"           }, // PTC7
    },
    {
        {  "PTD0",         "KBI1_P0",      "FTM2_CH2",       "-",              "-"           }, // PTD0 (high current drive pin when operated as an output)
        {  "PTD1",         "KBI1_P1",      "FTM2_CH3",       "-",              "-"           }, // PTD1 (high current drive pin when operated as an output)
        {  "PTD2",         "KBI1_P2",      "SPI1_MISO",      "-",              "-"           }, // PTD2
        {  "PTD3",         "KBI1_P3",      "SPI1_PCS0",      "-",              "-"           }, // PTD3
        {  "PTD4",         "KBI1_P4",      "-",              "-",              "-"           }, // PTD4
        {  "PTD5",         "KBI1_P5",      "-",              "-",              "-"           }, // PTD5
        {  "PTD6",         "KBI1_P6",      "UART2_RX",       "-",              "-"           }, // PTD6
        {  "PTD7",         "KBI1_P7",      "UART2_TX",       "-",              "-"           }, // PTD7
    },
    {
        {  "PTE0",         "-",            "SPI0_SCK",       "FTM1_CLK",       "-"           }, // PTE0 (high current drive pin when operated as an output)
        {  "PTE1",         "-",            "SPI0_MOSI",      "-",              "-"           }, // PTE1 (high current drive pin when operated as an output)
        {  "PTE2",         "-",            "SPI0_MISO",      "-",              "-"           }, // PTE2
        {  "PTE3",         "-",            "SPI0_PCS0",      "-",              "-"           }, // PTE3
        {  "PTE4",         "-",            "-",              "-",              "-"           }, // PTE4
        {  "PTE5",         "-",            "-",              "-",              "-"           }, // PTE5
        {  "PTE6",         "-",            "-",              "-",              "-"           }, // PTE6
        {  "PTE7",         "-",            "FTM2_CLK",       "-",              "FTM1_CH1"    }, // PTE7
    },
    {
        {  "PTF0",         "-",            "-",              "-",              "-"           }, // PTF0
        {  "PTF1",         "-",            "-",              "-",              "-"           }, // PTF1
        {  "PTF2",         "-",            "-",              "-",              "-"           }, // PTF2
        {  "PTF3",         "-",            "-",              "-",              "-"           }, // PTF3
        {  "PTF4",         "-",            "-",              "-",              "ADC0_SE12"   }, // PTF4
        {  "PTF5",         "-",            "-",              "-",              "ADC0_SE13"   }, // PTF5
        {  "PTF6",         "-",            "-",              "-",              "ADC0_SE14"   }, // PTF6
        {  "PTF7",         "-",            "-",              "-",              "ADC0_SE15"   }, // PTF7
    },
    {
        {  "PTG0",         "-",            "-",              "-",              "-"           }, // PTG0
        {  "PTG1",         "-",            "-",              "-",              "-"           }, // PTG1
        {  "PTG2",         "-",            "-",              "-",              "-"           }, // PTG2
        {  "PTG3",         "-",            "-",              "-",              "-"           }, // PTG3
        {  "-",            "-",            "-",              "-",              "-"           }, // PTG4
        {  "-",            "-",            "-",              "-",              "-"           }, // PTG5
        {  "-",            "-",            "-",              "-",              "-"           }, // PTG6
        {  "-",            "-",            "-",              "-",              "-"           }, // PTG7
    },
    {
        {  "PTH0",         "-",            "FTM2_CH0",       "-",              "-"           }, // PTH0
        {  "PTH1",         "-",            "FTM2_CH1",       "-",              "-"           }, // PTH1
        {  "PTH2",         "-",            "BUSOUT",         "-",              "FTM1_CH2"    }, // PTH2
        {  "-",            "-",            "-",              "-",              "-"           }, // PTH3
        {  "-",            "-",            "-",              "-",              "-"           }, // PTH4
        {  "-",            "-",            "-",              "-",              "-"           }, // PTH5
        {  "PTH6",         "-",            "-",              "-",              "-"           }, // PTH6
        {  "-",            "-",            "-",              "-",              "-"           }, // PTH7
    },
};
#elif defined KINETIS_KE04

#if PIN_COUNT == PIN_COUNT_16_PIN                                        // 16 pin TSSOP
    #define _PIN_COUNT      2
#elif PIN_COUNT == PIN_COUNT_20_PIN                                      // 20 pin SOIC
    #define _PIN_COUNT      1
#elif PIN_COUNT == PIN_COUNT_24_PIN                                      // 24 pin QFN
    #define _PIN_COUNT      0
#elif PIN_COUNT == PIN_COUNT_44_PIN                                      // 44 pin LQFP
    #define _PIN_COUNT      5
#elif PIN_COUNT == PIN_COUNT_64_PIN                                      // 64 pin (L)QFP
    #define _PIN_COUNT      4
#elif PIN_COUNT == PIN_COUNT_80_PIN                                      // 80 pin LQFP
    #define _PIN_COUNT      3
#else
    #error "Package not clearly defined!!"
#endif

#define ALTERNATIVE_FUNCTIONS 6

static const char *cPinNumber[PORTS_AVAILABLE_8_BIT][PORT_WIDTH][6] = {
    {
        //  QFN24   LQFP20  TSSOP16 LQFP80 LQFP64 LQPF44                 GPIO A
        {  "22",   "20",  "16",  "62",  "50",  "35"  },                  // PA0
        {  "21",   "19",  "15",  "61",  "49",  "34"  },                  // PA1
        {  "20",   "18",  "14",  "60",  "48",  "33"  },                  // PA2
        {  "19",   "17",  "13",  "59",  "47",  "32"  },                  // PA3
        {  "24",   "2",   "2",   "80",  "64",  "44"  },                  // PA4
        {  "23",   "1",   "1",   "79",  "63",  "43"  },                  // PA5
        {  "18",   "-",   "-",   "46",  "38",  "26"  },                  // PA6
        {  "17",   "-",   "-",   "45",  "37",  "25"  },                  // PA7
    },
    {
        {  "16",   "16",  "12",  "42",  "34",  "24"  },                  // PB0
        {  "15",   "15",  "11",  "41",  "33",  "23"  },                  // PB1
        {  "14",   "14",  "10",  "40",  "32",  "22"  },                  // PB2
        {  "13",   "13",  "9",   "39",  "31",  "21"  },                  // PB3
        {  "8",    "8",   "8",   "23",  "19",  "13"  },                  // PB4
        {  "7",    "7",   "7",   "22",  "18",  "12"  },                  // PB5
        {  "6",    "6",   "6",   "14",  "12",  "11"  },                  // PB6
        {  "5",    "5",   "5",   "13",  "11",  "9"   },                  // PB7
    },
    {
        {  "12",   "12",  "-",   "32",  "26",  "20"  },                  // PC0
        {  "11",   "11",  "-",   "31",  "25",  "19"  },                  // PC1
        {  "10",   "10",  "-",   "25",  "21",  "15"  },                  // PC2
        {  "9",    "9",   "-",   "24",  "20",  "14"  },                  // PC3
        {  "2",    "-",   "-",   "78",  "62",  "42"  },                  // PC4
        {  "1",    "-",   "-",   "77",  "61",  "41"  },                  // PC5
        {  "-",    "-",   "-",   "64",  "52",  "37"  },                  // PC6
        {  "-",    "-",   "-",   "63",  "51",  "36"  },                  // PC7
    },
    {
        {  "-",    "-",   "-",   "2",   "2",   "2"   },                  // PD0                
        {  "-",    "-",   "-",   "1",   "1",   "1"   },                  // PD1
        {  "-",    "-",   "-",   "58",  "46",  "31"  },                  // PD2
        {  "-",    "-",   "-",   "57",  "45",  "30"  },                  // PD3
        {  "-",    "-",   "-",   "56",  "44",  "29"  },                  // PD4
        {  "-",    "-",   "-",   "28",  "24",  "18"  },                  // PD5
        {  "-",    "-",   "-",   "27",  "23",  "17"  },                  // PD6
        {  "-",    "-",   "-",   "26",  "22",  "16"  },                  // PD7
    },
    #if PORTS_AVAILABLE > 1
    {
        {  "-",    "-",   "-",   "76",  "60",  "40"  },                  // PE0
        {  "-",    "-",   "-",   "75",  "59",  "39"  },                  // PE1
        {  "-",    "-",   "-",   "68",  "54",  "38"  },                  // PE2
        {  "-",    "-",   "-",   "67",  "53",  "-"   },                  // PE3
        {  "-",    "-",   "-",   "47",  "39",  "-"   },                  // PE4
        {  "-",    "-",   "-",   "21",  "17",  "-"   },                  // PE5
        {  "-",    "-",   "-",   "20",  "16",  "-"   },                  // PE6
        {  "-",    "-",   "-",   "6",   "5",   "3"   },                  // PE7
    },
    {
        {  "-",    "-",   "-",   "55",  "43",  "-"   },                  // PF0
        {  "-",    "-",   "-",   "54",  "42",  "-"   },                  // PF1
        {  "-",    "-",   "-",   "44",  "36",  "-"   },                  // PF2
        {  "-",    "-",   "-",   "43",  "35",  "-"   },                  // PF3
        {  "-",    "-",   "-",   "38",  "30",  "-"   },                  // PF4
        {  "-",    "-",   "-",   "37",  "29",  "-"   },                  // PF5
        {  "-",    "-",   "-",   "36",  "28",  "-"   },                  // PF6
        {  "-",    "-",   "-",   "35",  "27",  "-"   },                  // PF7
    },
    {
        {  "-",    "-",   "-",   "74",  "58",  "-"   },                  // PG0
        {  "-",    "-",   "-",   "73",  "57",  "-"   },                  // PG1
        {  "-",    "-",   "-",   "72",  "56",  "-"   },                  // PG2
        {  "-",    "-",   "-",   "71",  "55",  "-"   },                  // PG3
        {  "-",    "-",   "-",   "53",  "-",   "-"   },                  // PG4
        {  "-",    "-",   "-",   "52",  "-",   "-"   },                  // PG5
        {  "-",    "-",   "-",   "51",  "-",   "-"   },                  // PG6
        {  "-",    "-",   "-",   "50",  "-",   "-"   },                  // PG7
    },
    {
        {  "-",    "-",   "-",   "19",  "15",  "-"   },                  // PH0
        {  "-",    "-",   "-",   "18",  "14",  "-"   },                  // PH1
        {  "-",    "-",   "-",   "7",   "6",   "4"   },                  // PH2
        {  "-",    "-",   "-",   "34",  "-",   "-"   },                  // PH3
        {  "-",    "-",   "-",   "33",  "-",   "-"   },                  // PH4
        {  "-",    "-",   "-",   "5",   "-",   "-"   },                  // PH5
        {  "-",    "-",   "-",   "4",   "4",   "-"   },                  // PH6
        {  "-",    "-",   "-",   "3",   "3",   "-"   },                  // PH7
    },
    #endif
    #if PORTS_AVAILABLE > 2
    {
        {  "-",    "-",   "-",   "17",  "-",   "-"   },                  // PI0
        {  "-",    "-",   "-",   "16",  "-",   "-"   },                  // PI1
        {  "-",    "-",   "-",   "66",  "-",   "-"   },                  // PI2
        {  "-",    "-",   "-",   "65",  "-",   "-"   },                  // PI3
        {  "-",    "-",   "-",   "15",  "13",  "11"  },                  // PI4
        {  "-",    "-",   "-",   "30",  "-",   "-"   },                  // PI5
        {  "-",    "-",   "-",   "29",  "-",   "-"   },                  // PI6
        {  "-",    "-",   "-",   "-",   "-",   "-"   },                  // PI7
    },
    {
        {  "-",    "-",   "-",   "-",   "-",   "-"   },
        {  "-",    "-",   "-",   "-",   "-",   "-"   },
        {  "-",    "-",   "-",   "-",   "-",   "-"   },
        {  "-",    "-",   "-",   "-",   "-",   "-"   },
        {  "-",    "-",   "-",   "-",   "-",   "-"   },
        {  "-",    "-",   "-",   "-",   "-",   "-"   },
        {  "-",    "-",   "-",   "-",   "-",   "-"   },
        {  "-",    "-",   "-",   "-",   "-",   "-"   },
    },
    {
        {  "-",    "-",   "-",   "-",   "-",   "-"   },
        {  "-",    "-",   "-",   "-",   "-",   "-"   },
        {  "-",    "-",   "-",   "-",   "-",   "-"   },
        {  "-",    "-",   "-",   "-",   "-",   "-"   },
        {  "-",    "-",   "-",   "-",   "-",   "-"   },
        {  "-",    "-",   "-",   "-",   "-",   "-"   },
        {  "-",    "-",   "-",   "-",   "-",   "-"   },
        {  "-",    "-",   "-",   "-",   "-",   "-"   },
    },
    {
        {  "-",    "-",   "-",   "-",   "-",   "-"   },
        {  "-",    "-",   "-",   "-",   "-",   "-"   },
        {  "-",    "-",   "-",   "-",   "-",   "-"   },
        {  "-",    "-",   "-",   "-",   "-",   "-"   },
        {  "-",    "-",   "-",   "-",   "-",   "-"   },
        {  "-",    "-",   "-",   "-",   "-",   "-"   },
        {  "-",    "-",   "-",   "-",   "-",   "-"   },
        {  "-",    "-",   "-",   "-",   "-",   "-"   },
    },
    #endif
};

static int ADC_DEDICATED_CHANNEL[PORT_WIDTH] = {0, 0, 0, 0, 0, 0, 0, 0};
static int ADC_DEDICATED_MODULE[PORT_WIDTH] = {0, 0, 0, 0, 0, 0, 0, 0};
static int ADC_MUX_CHANNEL[PORTS_AVAILABLE_8_BIT][PORT_WIDTH] = {
    { -1,-1,-1,-1,-1,-1,-1,-1 }, // port A
    { -1,-1,-1,-1,-1,-1,-1,-1 }, // port B
#if PORTS_AVAILABLE_8_BIT > 2
    { -1,-1,-1,-1,-1,-1,-1,-1 }, // port C
#endif
#if PORTS_AVAILABLE_8_BIT > 3
    { -1,-1,-1,-1,-1,-1,-1,-1 }, // port D
#endif
#if PORTS_AVAILABLE_8_BIT > 4
    { -1,-1,-1,-1,-1,-1,-1,-1 }, // port E
#endif
#if PORTS_AVAILABLE_8_BIT > 5
    { -1,-1,-1,-1,-1,-1,-1,-1 }, // port F
#endif
#if PORTS_AVAILABLE_8_BIT > 6
    { -1,-1,-1,-1,-1,-1,-1,-1 }, // port G
#endif
#if PORTS_AVAILABLE_8_BIT > 7
    { -1,-1,-1,-1,-1,-1,-1,-1 }, // port H
#endif
#if PORTS_AVAILABLE_8_BIT > 8
    { -1,-1,-1,-1,-1,-1,-1,-1 }, // port I
#endif
};


static const char *cPer[PORTS_AVAILABLE_8_BIT][PORT_WIDTH * 4][8] = {
    #if _PIN_COUNT <= 2                                                  // 16, 20, 24 pin
    {
        // GPIO            ALT 1           ALT 2             ALT3          ALT4           ALT5            ALT6                 ALT 7         (GPIO has lowest priority, ALT7 has highest)
        {  "PTA0",         "KBI0_P0",      "FTM0_CH0",       "RTCO",       "ACMP0_IN2",   "ADC0_SE0",     "SWD_CLK",           "-"           }, // PTA0
        {  "PTA1",         "KBI0_P1",      "FTM0_CH1",       "-",          "ACMP0_IN1",   "-",            "ADC0_SE1",          "-"           }, // PTA1
        {  "PTA2",         "KBI0_P2",      "UART0_RX",       "I2C0_SDA",   "-",           "-",            "-",                 "-"           }, // PTA2 (true open-drain output when operating as output)
        {  "PTA3",         "KBI0_P3",      "UART0_TX",       "I2C0_SCL",   "-",           "-",            "-",                 "-"           }, // PTA3 (true open-drain output when operating as output)
        {  "PTA4",         "-",            "-",              "-",          "ACMP0_OUT",   "-",            "SWD_DIO",           "-"           }, // PTA4 (SWD_DIO default)
        {  "PTA5",         "IRQ",          "TCLK0",          "-",          "-",           "-",            "RESET_b",           "-"           }, // PTA5 (RESET default)
        {  "PTA6",         "-",            "FTM2_FLT1",      "SPI0_SCK",   "ACMP1_IN0" ,  "-",            "ADC0_SE2",          "-"           }, // PTA6
        {  "PTA7",         "-",            "FTM2_FLT2",      "SPI0_MOSI",  "ACMP1_IN1",   "-",            "ADC0_SE3",          "-"           }, // PTA7
    },
    #else
    {
        // GPIO            ALT 1           ALT 2             ALT3          ALT4           ALT5            ALT6                 ALT 7         (GPIO has lowest priority, ALT7 has highest)
        {  "PTA0",         "KBI0_P0",      "FTM0_CH0",       "I2C0_4WSCLOUT","ACMP0_IN0", "ADC0_SE0",     "-",                 "-"           }, // PTA0
        {  "PTA1",         "KBI0_P1",      "FTM0_CH1",       "I2C0_4WSDAOUT","ACMP0_IN1", "ADC0_SE1",     "-",                 "-"           }, // PTA1
        {  "PTA2",         "KBI0_P2",      "UART0_RX",       "I2C0_SDA",   "-",           "-",            "-",                 "-"           }, // PTA2 (true open-drain output when operating as output)
        {  "PTA3",         "KBI0_P3",      "UART0_TX",       "I2C0_SCL",   "-",           "-",            "-",                 "-"           }, // PTA3 (true open-drain output when operating as output)
        {  "PTA4",         "KBI0_P4",      "-",              "ACMP0_OUT",  "SWD_DIO",     "-",            "-",                 "-"           }, // PTA4 (SWD_DIO default)
        {  "PTA5",         "KBI0_P5",      "IRQ",            "TCLK0",      "RESET_b",     "-",            "-",                 "-"           }, // PTA5 (RESET default)
        {  "PTA6",         "KBI0_P6",      "FTM2_FLT1",      "ACMP1_IN0",  "ADC0_SE2" ,   "-",            "-",                 "-"           }, // PTA6
        {  "PTA7",         "KBI0_P7",      "FTM2_FLT2",      "ACMP1_IN1",  "ADC0_SE3",    "-",            "-",                 "-"           }, // PTA7
    },
    #endif
    #if _PIN_COUNT <= 2                                                  // 16, 20, 24 pin
    {
        {  "PTB0",         "KBI0_P4",      "UART0_RX",       "SPI0_PC",    "PWT_IN1",     "-",            "ADC0_SE4",          "-"           }, // PTB0
        {  "PTB1",         "KBI0_P5",      "UART0_TX",       "SPI0_MISO",  "TCLK2",       "-",            "ADC0_SE5",          "-"           }, // PTB1
        {  "PTB2",         "KBI0_P6",      "SPI0_SCK",       "FTM0_CH0",   "ACMP0_IN0",   "-",            "ADC0_SE6",          "-"           }, // PTB2
        {  "PTB3",         "KBI0_P7",      "SPI0_MOSI",      "FTM0_CH1",   "-",           "-",            "ADC0_SE7",          "-"           }, // PTB3
        {  "PTB4",         "KBI1_P6",      "FTM2_CH4",       "SPI0_MISO",  "ACMP1_IN2",   "NMI_b",        "-",                 "-"           }, // PTB4 (NMI default) (high current drive pin when operated as an output)
        {  "PTB5",         "KBI1_P7",      "FTM2_CH5",       "SPI0_PCS",   "ACMP1_OUT",   "-",            "-",                 "-"           }, // PTB5 (high current drive pin when operated as an output)
        {  "PTB6",         "-",            "I2C0_SDA",       "-",          "-",           "-",            "XTAL",              "-"           }, // PTB6
        {  "PTB7",         "-",            "I2C0_SCL",       "-",          "-",           "-",            "EXTAL",             "-"           }, // PTB7
    },
    #else
    {
        {  "PTB0",         "KBI0_P8",      "UART0_RX",       "PWT_IN1",    "ADC0_SE4",    "-",            "-",                 "-"           }, // PTB0
        {  "PTB1",         "KBI0_P9",      "UART0_TX",       "-",          "ADC0_SE5",    "-",            "-",                 "-"           }, // PTB1
        {  "PTB2",         "KBI0_P6",      "SPI0_SCK",       "FTM0_CH0",   "ADC0_SE6",    "-",            "-",                 "-"           }, // PTB2
        {  "PTB3",         "KBI0_P7",      "SPI0_MOSI",      "FTM0_CH1",   "ADC0_SE7",    "-",            "-",                 "-"           }, // PTB3
        {  "PTB4",         "KBI0_P12",     "FTM2_CH4",       "SPI0_MISO",  "ACMP1_IN2",   "NMI_b",        "-",                 "-"           }, // PTB4
        {  "PTB5",         "KBI0_P13",     "FTM2_CH5",       "SPI0_PCS",   "ACMP1_OUT",   "-",            "-",                 "-"           }, // PTB5
        {  "PTB6",         "KBI0_P14",     "I2C0_SDA",       "-",          "-",           "-",            "-",                 "XTAL"        }, // PTB6
        {  "PTB7",         "KBI0_P15",     "I2C0_SCL",       "-",          "-",           "-",            "-",                 "EXTAL"       }, // PTB7
    },
    #endif
    #if _PIN_COUNT <= 2                                                  // 16, 20, 24 pin
    {
        {  "PTC0",         "KBI1_P2",      "FTM2_CH0",       "-",          "-",           "-",            "ADC0_SE8",          "-"           }, // PTC0
        {  "PTC1",         "KBI1_P3",      "FTM2_CH1",       "-",          "-",           "-",            "ADC0_SE9",          "-"           }, // PTC1
        {  "PTC2",         "KBI1_P4",      "FTM2_CH2",       "-",          "-",           "-",            "ADC0_SE10",         "-"           }, // PTC2
        {  "PTC3",         "KBI1_P5",      "FTM2_CH3",       "-",          "-",           "-",            "ADC0_SE11",         "-"           }, // PTC3
        {  "PTC4",         "KBI1_P0",      "FTM2_CH2",       "-",          "PWT_IN0",     "-",            "-",                 "-"           }, // PTC4
        {  "PTC5",         "KBI1_P1",      "FTM2_CH3",       "BUSOUT",     "-",           "-",            "-",                 "-"           }, // PTC5
        {  "-",            "-",            "-",              "-",          "-",           "-",            "-",                 "-"           }, // PTC6
        {  "-",             "-",           "-",              "-",          "-",           "-",            "-",                 "-"           }, // PTC7
    },
    #else
    {
        {  "PTC0",         "KBI1_P28",     "FTM2_CH0",       "-",          "ADC0_SE8",    "-",            "-",                 "-"           }, // PTC0
        {  "PTC1",         "KBI0_P17",     "FTM2_CH1",       "-",          "ADC0_SE9",    "-",            "-",                 "-"           }, // PTC1
        {  "PTC2",         "KBI0_P18",     "FTM2_CH2",       "-",          "ADC0_SE10",   "-",            "-",                 "-"           }, // PTC2
        {  "PTC3",         "KBI0_P19",     "FTM2_CH3",       "-",          "ADC0_SE11",   "-",            "-",                 "-"           }, // PTC3
        {  "PTC4",         "KBI0_P20",     "RTC_CLKOUT",     "FTM1_CH0",   "ACMP0_IN2",   "SWD_CLK",      "-",                 "-"           }, // PTC4 (SWD_CLK default)
        {  "PTC5",         "KBI0_P21",     "-",              "FTM1_CH1",   "-",           "RTC_CLKOUT",   "-",                 "-"           }, // PTC5
        {  "PTC6",         "KBI0_P22",     "UART1_RX",       "-",          "-",           "CAN0_RX",      "-",                 "-"           }, // PTC6
        {  "PTC7",         "KBI0_P23",     "UART1_TX",       "-",          "-",           "CAN0_TX",      "-",                 "-"           }, // PTC7
    },
    #endif
    #if _PIN_COUNT <= 2                                                  // 16, 20, 24 pin
    {
        {  "-",            "-",            "-",              "-",          "-",           "-",            "-",                 "-"           }, // PTD0
        {  "-",            "-",            "-",              "-",          "-",           "-",            "-",                 "-"           }, // PTD1
        {  "-",            "-",            "-",              "-",          "-",           "-",            "-",                 "-"           }, // PTD2
        {  "-",            "-",            "-",              "-",          "-",           "-",            "-",                 "-"           }, // PTD3
        {  "-",            "-",            "-",              "-",          "-",           "-",            "-",                 "-"           }, // PTD4
        {  "-",            "-",            "-",              "-",          "-",           "-",            "-",                 "-"           }, // PTD5
        {  "-",            "-",            "-",              "-",          "-",           "-",            "-",                 "-"           }, // PTD6
        {  "-",            "-",            "-",              "-",          "-",           "-",            "-",                 "-"           }, // PTD7
    },
    #else
    {
        {  "PTD0",         "KBI0_P24",     "FTM2_CH2",       "SPI1_SCK",   "-",           "-",            "-",                 "-"           }, // PTD0 (high current drive pin when operated as an output)
        {  "PTD1",         "KBI0_P25",     "FTM2_CH3",       "SPI1_MOSI",  "-",           "-",            "-",                 "-"           }, // PTD1 (high current drive pin when operated as an output)
        {  "PTD2",         "KBI0_P26",     "SPI1_MISO",      "-",          "-",           "-",            "-",                 "-"           }, // PTD2
        {  "PTD3",         "KBI0_P27",     "SPI1_PCS0",      "-",          "-",           "-",            "-",                 "-"           }, // PTD3
        {  "PTD4",         "KBI0_P28",     "-",              "-",          "-",           "-",            "-",                 "-"           }, // PTD4
        {  "PTD5",         "KBI0_P29",     "PWT_IN0",        "-",          "-",           "-",            "-",                 "-"           }, // PTD5
        {  "PTD6",         "KBI0_P30",     "UART2_RX",       "-",          "-",           "-",            "-",                 "-"           }, // PTD6
        {  "PTD7",         "KBI0_P31",     "UART2_TX",       "-",          "-",           "-",            "-",                 "-"           }, // PTD7
    },
    #endif
    #if PORTS_AVAILABLE > 1
    {
        {  "PTE0",         "KBI1_P0",      "SPI0_SCK",       "TCLK1",      "I2C1_SDA",    "-",            "-",                 "-"           }, // PTE0 (high current drive pin when operated as an output)
        {  "PTE1",         "KBI1_P1",      "SPI0_MOSI",      "-",          "I2C1_SCL",    "-",            "-",                 "-"           }, // PTE1 (high current drive pin when operated as an output)
        {  "PTE2",         "KBI1_P2",      "SPI0_MISO",      "PWT_IN0",    "-",           "-",            "-",                 "-"           }, // PTE2
        {  "PTE3",         "KBI1_P3",      "SPI0_PCS0",      "-",          "-",           "-",            "-",                 "-"           }, // PTE3
        {  "PTE4",         "KBI1_P4",      "-",              "-",          "-",           "-",            "-",                 "-"           }, // PTE4
        {  "PTE5",         "KBI1_P5",      "-",              "-",          "-",           "-",            "-",                 "-"           }, // PTE5
        {  "PTE6",         "KBI1_P6",      "-",              "-",          "-",           "-",            "-",                 "-"           }, // PTE6
        {  "PTE7",         "KBI1_P7",      "TCLK2",          "-",          "FTM1_CH1",    "CAN0_TX",      "-",                 "-"           }, // PTE7
    },
    {
        {  "PTF0",         "KBI1_P8",      "FTM2_CH0",       "-",          "-",           "-",            "-",                 "-"           }, // PTF0
        {  "PTF1",         "KBI1_P9",      "FTM2_CH1",       "-",          "-",           "-",            "-",                 "-"           }, // PTF1
        {  "PTF2",         "KBI1_P10",     "UART1_RX",       "-",          "-",           "-",            "-",                 "-"           }, // PTF2
        {  "PTF3",         "KBI1_P11",     "UART1_TX",       "-",          "-",           "-",            "-",                 "-"           }, // PTF3
        {  "PTF4",         "KBI1_P12",     "-",              "-",          "ADC0_SE12",   "-",            "-",                 "-"           }, // PTF4
        {  "PTF5",         "KBI1_P13",     "-",              "-",          "ADC0_SE13",   "-",            "-",                 "-"           }, // PTF5
        {  "PTF6",         "KBI1_P14",     "-",              "-",          "ADC0_SE14",   "-",            "-",                 "-"           }, // PTF6
        {  "PTF7",         "KBI1_P15",     "-",              "-",          "ADC0_SE15",   "-",            "-",                 "-"           }, // PTF7
    },
    {
        {  "PTG0",         "KBI1_P16",     "-",              "-",          "-",           "-",            "-",                 "-"           }, // PTG0
        {  "PTG1",         "KBI1_P17",     "-",              "-",          "-",           "-",            "-",                 "-"           }, // PTG1
        {  "PTG2",         "KBI1_P18",     "-",              "-",          "-",           "-",            "-",                 "-"           }, // PTG2
        {  "PTG3",         "KBI1_P19",     "-",              "-",          "-",           "-",            "-",                 "-"           }, // PTG3
        {  "PTG4",         "KBI1_P20",     "FTM2_CH2",       "SPI1_SCK",   "-",           "-",            "-",                 "-"           }, // PTG4
        {  "PTG5",         "KBI1_P21",     "FTM2_CH3",       "SPI1_MOSI",  "-",           "-",            "-",                 "-"           }, // PTG5
        {  "PTG6",         "KBI1_P22",     "FTM2_CH4",       "SPI1_MISO",  "-",           "-",            "-",                 "-"           }, // PTG6
        {  "PTG7",         "KBI1_P23",     "FTM2_CH5",       "SPI1_PCS",   "-",           "-",            "-",                 "-"           }, // PTG7
    },
    {
        {  "PTH0",         "KBI1_P24",     "FTM2_CH0",       "-",          "-",           "-",            "-",                 "-"           }, // PTH0
        {  "PTH1",         "KBI1_P25",     "FTM2_CH1",       "-",          "-",           "-",            "-",                 "-"           }, // PTH1
        {  "PTH2",         "KBI1_P26",     "BUSOUT",         "-",          "FTM1_CH0",    "CAN0_RX",      "-",                 "-"           }, // PTH2
        {  "PTH3",         "KBI1_P27",     "I2C1_SDA",       "-",          "-",           "-",            "-",                 "-"           }, // PTH3
        {  "PTH4",         "KBI1_P28",     "I2C1_SCL",       "-",          "-",           "-",            "-",                 "-"           }, // PTH4
        {  "PTH5",         "KBI1_P29",     "-",              "-",          "-",           "-",            "-",                 "-"           }, // PTH5
        {  "PTH6",         "KBI1_P30",     "-",              "-",          "-",           "-",            "-",                 "-"           }, // PTH6
        {  "PTH7",         "KBI1_P31",     "PWT_IN1",        "-",          "-",           "-",            "-",                 "-"           }, // PTH7
    }
    #endif
};
#elif defined KINETIS_KEA8
#if PIN_COUNT == PIN_COUNT_24_PIN                                        // 24 pin QFN
    #define _PIN_COUNT      0
#elif PIN_COUNT == PIN_COUNT_16_PIN                                      // 16 pin TSSOP
    #define _PIN_COUNT      1
#else
    #error "Package not clearly defined!!"
#endif

#define ALTERNATIVE_FUNCTIONS 7

static const char *cPinNumber[PORTS_AVAILABLE_8_BIT][PORT_WIDTH][2] = {
    {
        // QFN24   TSSOP16                                               GPIO A
        {  "22",    "16",   },                                           // PA0
        {  "21",    "15",   },                                           // PA1
        {  "20",    "14",   },                                           // PA2
        {  "19",    "13",   },                                           // PA3
        {  "24",    "2",    },                                           // PA4
        {  "23",    "1",    },                                           // PA5
        {  "18",    "-",    },                                           // PA6
        {  "17",    "-",    },                                           // PA7
    },
    {
        {  "16",    "12",  },                                            // PB0
        {  "15",    "11",  },                                            // PB1
        {  "14",    "10",  },                                            // PB2
        {  "13",    "9",   },                                            // PB3
        {  "8",     "8",   },                                            // PB4
        {  "7",     "7",   },                                            // PB5
        {  "6",     "6",   },                                            // PB6
        {  "5",     "5",   },                                            // PB7
    },
    {
        {  "12",    "-",   },                                            // PC0
        {  "11",    "-",   },                                            // PC1
        {  "10",    "-",   },                                            // PC2
        {  "9",     "-",   },                                            // PC3
        {  "2",     "-",   },                                            // PC4
        {  "1",     "-",   },                                            // PC5
        {  "-",     "-",   },                                            // PC6
        {  "-",     "-",   },                                            // PC7
    },

};

static int ADC_DEDICATED_CHANNEL[PORT_WIDTH] = {0, 0, 0, 0, 0, 0, 0, 0};
static int ADC_DEDICATED_MODULE[PORT_WIDTH] = {0, 0, 0, 0, 0, 0, 0, 0};
static int ADC_MUX_CHANNEL[PORTS_AVAILABLE_8_BIT][PORT_WIDTH] = {
    { -1,-1,-1,-1,-1,-1,-1,-1 }, // port A
    { -1,-1,-1,-1,-1,-1,-1,-1 }, // port B
#if PORTS_AVAILABLE_8_BIT > 2
    { -1,-1,-1,-1,-1,-1,-1,-1 }, // port C
#endif
#if PORTS_AVAILABLE_8_BIT > 3
    { -1,-1,-1,-1,-1,-1,-1,-1 }, // port D
#endif
#if PORTS_AVAILABLE_8_BIT > 4
    { -1,-1,-1,-1,-1,-1,-1,-1 }, // port E
#endif
#if PORTS_AVAILABLE_8_BIT > 5
    { -1,-1,-1,-1,-1,-1,-1,-1 }, // port F
#endif
#if PORTS_AVAILABLE_8_BIT > 6
    { -1,-1,-1,-1,-1,-1,-1,-1 }, // port G
#endif
#if PORTS_AVAILABLE_8_BIT > 7
    { -1,-1,-1,-1,-1,-1,-1,-1 }, // port H
#endif
#if PORTS_AVAILABLE_8_BIT > 8
    { -1,-1,-1,-1,-1,-1,-1,-1 }, // port I
#endif
};


static const char *cPer[PORTS_AVAILABLE_8_BIT][PORT_WIDTH * 4][8] = {
    {
        // GPIO            ALT 1           ALT 2             ALT3          ALT4           ALT5            ALT6                 ALT 7         (GPIO has lowest priority, ALT7 has highest)
        {  "PTA0",         "KBI0_P0",      "FTM0_CH0",       "RTCO",       "ACMP0_IN0",   "ADC0_SE0",     "SWD_CLK",           "-"           }, // PTA0 (SWD_CLK default)
        {  "PTA1",         "KBI0_P1",      "FTM0_CH1",       "-",          "ACMP0_IN1",   "-",            "ADC0_SE1",          "-"           }, // PTA1
        {  "PTA2",         "KBI0_P2",      "UART0_RX",       "I2C0_SDA",   "-",           "-",            "-",                 "-"           }, // PTA2 (true open-drain output when operating as output)
        {  "PTA3",         "KBI0_P3",      "UART0_TX",       "I2C0_SCL",   "-",           "-",            "-",                 "-"           }, // PTA3 (true open-drain output when operating as output)
        {  "PTA4",         "-",            "-",              "ACMP0_OUT",  "-",           "-",            "SWD_DIO",           "-"           }, // PTA4 (SWD_DIO default)
        {  "PTA5",         "IRQ",          "TCLK1",          "-",          "-",           "-",            "RESET_b",           "-"           }, // PTA5 (RESET default)
        {  "PTA6",         "-",            "FTM2_FLT1",      "SPI0_SCK",   "ACMP1_IN0" ,  "-",            "ADC0_SE2",          "-"           }, // PTA6
        {  "PTA7",         "-",            "FTM2_FLT2",      "SPI0_MOSI",  "ACMP1_IN1",   "-",            "ADC0_SE3",          "-"           }, // PTA7
    },
    {
        {  "PTB0",         "KBI0_P4",      "UART0_RX",       "SPI0_PCS",   "PWT_IN1",     "-",            "ADC0_SE4",          "-"           }, // PTB0
        {  "PTB1",         "KBI0_P5",      "UART0_TX",       "SPI0_MISO",  "TCLK2",       "-",            "ADC0_SE5",          "-"           }, // PTB1
        {  "PTB2",         "KBI0_P6",      "SPI0_SCK",       "FTM0_CH0",   "ACMP0_IN0",   "-",            "ADC0_SE6",          "-"           }, // PTB2
        {  "PTB3",         "KBI0_P7",      "SPI0_MOSI",      "FTM0_CH1",   "-",           "-",            "ADC0_SE7",          "-"           }, // PTB3
        {  "PTB4",         "KBI1_P6",      "FTM2_CH4",       "SPI0_MISO",  "ACMP1_IN2",   "NMI_b",        "-",                 "-"           }, // PTB4 (NMI default) (high current drive pin when operated as an output)
        {  "PTB5",         "KBI1_P7",      "FTM2_CH5",       "SPI0_PCS",   "ACMP1_OUT",   "-",            "-",                 "-"           }, // PTB5 (high current drive pin when operated as an output)
        {  "PTB6",         "-",            "I2C0_SDA",       "-",          "-",           "-",            "XTAL",              "-"           }, // PTB6
        {  "PTB7",         "-",            "I2C0_SCL",       "-",          "-",           "-",            "EXTAL",             "-"           }, // PTB7
    },
    {
        {  "PTC0",         "KBI1_P2",      "FTM2_CH0",       "-",          "-",           "-",            "ADC0_SE8",          "-"           }, // PTC0
        {  "PTC1",         "KBI1_P3",      "FTM2_CH1",       "-",          "-",           "-",            "ADC0_SE9",          "-"           }, // PTC1
        {  "PTC2",         "KBI1_P4",      "FTM2_CH2",       "-",          "-",           "-",            "ADC0_SE10",         "-"           }, // PTC2
        {  "PTC3",         "KBI1_P5",      "FTM2_CH3",       "-",          "-",           "-",            "ADC0_SE11",         "-"           }, // PTC3
        {  "PTC4",         "KBI1_P0",      "FTM2_CH2",       "-",          "PWT_IN0",     "-",            "-",                 "-"           }, // PTC4
        {  "PTC5",         "KBI1_P1",      "FTM2_CH3",       "BUSOUT",     "-",           "-",            "-",                 "-"           }, // PTC5
        {  "-",            "-",            "-",              "-",          "-",           "-",            "-",                 "-"           }, // PTC6
        {  "-",            "-",            "-",              "-",          "-",           "-",            "-",                 "-"           }, // PTC7
    }
};
#elif defined KINETIS_KE06 || defined KINETIS_KEA
#if PIN_COUNT == PIN_COUNT_44_PIN                                        // 44 pin LQFP
    #define _PIN_COUNT      2
#elif PIN_COUNT == PIN_COUNT_64_PIN                                      // 64 pin (L)QFP
    #define _PIN_COUNT      1
#elif PIN_COUNT == PIN_COUNT_80_PIN                                      // 80 pin LQFP
    #define _PIN_COUNT      0
#else
    #error "Package not clearly defined!!"
#endif

#define ALTERNATIVE_FUNCTIONS 7

static const char *cPinNumber[PORTS_AVAILABLE_8_BIT][PORT_WIDTH][3] = {
    {
        // LQFP80   LQFP64  LQFP44                                       GPIO A
        {  "62",    "50",   "35"   },                                    // PA0
        {  "61",    "49",   "34"   },                                    // PA1
        {  "60",    "48",   "33"   },                                    // PA2
        {  "59",    "47",   "32"   },                                    // PA3
        {  "80",    "64",   "44"   },                                    // PA4
        {  "79",    "63",   "43"   },                                    // PA5
        {  "46",    "38",   "26"   },                                    // PA6
        {  "45",    "37",   "25"   },                                    // PA7
    },
    {
        {  "43",    "34",   "24"  },                                     // PB0
        {  "41",    "33",   "23"  },                                     // PB1
        {  "40",    "32",   "22"  },                                     // PB2
        {  "39",    "31",   "21"  },                                     // PB3
        {  "23",    "19",   "13"  },                                     // PB4
        {  "22",    "18",   "12"  },                                     // PB5
        {  "14",    "12",   "10"  },                                     // PB6
        {  "13",    "11",   "9"   },                                     // PB7
    },
    {
        {  "32",    "26",   "20"  },                                     // PC0
        {  "31",    "25",   "19"  },                                     // PC1
        {  "25",    "21",   "15"  },                                     // PC2
        {  "24",    "20",   "14"  },                                     // PC3
        {  "78",    "62",   "42"  },                                     // PC4
        {  "77",    "61",   "41"  },                                     // PC5
        {  "64",    "52",   "37"  },                                     // PC6
        {  "63",    "51",   "36"  },                                     // PC7
    },
    {
        {  "2",     "2",    "2"   },                                     // PD0
        {  "1",     "1",    "1"   },                                     // PD1
        {  "58",    "46",   "31"  },                                     // PD2
        {  "57",    "45",   "30"  },                                     // PD3
        {  "56",    "44",   "29"  },                                     // PD4
        {  "28",    "24",   "18"  },                                     // PD5
        {  "27",    "23",   "17"  },                                     // PD6
        {  "26",    "22",   "16"  },                                     // PD7
    },
    {
        {  "76",    "60",   "40"  },                                     // PE0
        {  "75",    "59",   "39"  },                                     // PE1
        {  "68",    "54",   "38"  },                                     // PE2
        {  "67",    "53",   "-"   },                                     // PE3
        {  "47",    "39",   "-"   },                                     // PE4
        {  "21",    "17",   "-"   },                                     // PE5
        {  "20",    "16",   "-"   },                                     // PE6
        {  "6",     "5",    "3"   },                                     // PE7
    },
    {
        {  "55",    "43",   "-"   },                                     // PF0
        {  "54",    "42",   "-"   },                                     // PF1
        {  "44",    "36",   "-"   },                                     // PF2
        {  "43",    "35",   "-"   },                                     // PF3
        {  "38",    "30",   "-"   },                                     // PF4
        {  "37",    "29",   "_"   },                                     // PF5
        {  "36",    "28",   "-"   },                                     // PF6
        {  "35",    "27",   "-"   },                                     // PF7
    },
    {
        {  "74",    "58",   "-"   },                                     // PG0
        {  "73",    "57",   "-"   },                                     // PG1
        {  "72",    "56",   "-"   },                                     // PG2
        {  "71",    "55",   "-"   },                                     // PG3
        {  "53",    "-",    "-"   },                                     // PG4
        {  "52",    "-",    "-"   },                                     // PG5
        {  "51",    "-",    "-"   },                                     // PG6
        {  "50",    "-",    "-"   },                                     // PG7
    },
    {
        {  "19",    "15",   "-"   },                                     // PH0
        {  "18",    "14",   "-"   },                                     // PH1
        {  "7",     "6",    "4"   },                                     // PH2
        {  "34",    "-",    "-"   },                                     // PH3
        {  "33",    "-",    "-"   },                                     // PH4
        {  "5",     "-",    "-"   },                                     // PH5
        {  "4",     "4",    "-"   },                                     // PH6
        {  "3",     "3",    "-"   },                                     // PH7
    },
    {
        {  "17",    "-",    "-"   },                                     // PI0
        {  "16",    "-",    "-"   },                                     // PI1
        {  "66",    "-",    "-"   },                                     // PI2
        {  "65",    "-",    "-"   },                                     // PI3
        {  "15",    "13",   "11"  },                                     // PI4
        {  "30",    "-",    "-"   },                                     // PI5
        {  "29",    "-",    "-"   },                                     // PI6
        {  "3",     "3",    "-"   },                                     // PI7
    }
};

static int ADC_DEDICATED_CHANNEL[PORT_WIDTH] = {0, 0, 0, 0, 0, 0, 0, 0};
static int ADC_DEDICATED_MODULE[PORT_WIDTH] = {0, 0, 0, 0, 0, 0, 0, 0};
static int ADC_MUX_CHANNEL[PORTS_AVAILABLE_8_BIT][PORT_WIDTH] = {
    { -1,-1,-1,-1,-1,-1,-1,-1 }, // port A
    { -1,-1,-1,-1,-1,-1,-1,-1 }, // port B
#if PORTS_AVAILABLE_8_BIT > 2
    { -1,-1,-1,-1,-1,-1,-1,-1 }, // port C
#endif
#if PORTS_AVAILABLE_8_BIT > 3
    { -1,-1,-1,-1,-1,-1,-1,-1 }, // port D
#endif
#if PORTS_AVAILABLE_8_BIT > 4
    { -1,-1,-1,-1,-1,-1,-1,-1 }, // port E
#endif
#if PORTS_AVAILABLE_8_BIT > 5
    { -1,-1,-1,-1,-1,-1,-1,-1 }, // port F
#endif
#if PORTS_AVAILABLE_8_BIT > 6
    { -1,-1,-1,-1,-1,-1,-1,-1 }, // port G
#endif
#if PORTS_AVAILABLE_8_BIT > 7
    { -1,-1,-1,-1,-1,-1,-1,-1 }, // port H
#endif
#if PORTS_AVAILABLE_8_BIT > 8
    { -1,-1,-1,-1,-1,-1,-1,-1 }, // port I
#endif
};


static const char *cPer[PORTS_AVAILABLE_8_BIT][PORT_WIDTH * 4][8] = {    // KEA64/KEA32 has GPIO to ALT4
    {
        // GPIO            ALT 1           ALT 2             ALT3          ALT4           ALT5            ALT6                 ALT 7         (GPIO has lowest priority, ALT7 has highest)
        {  "PTA0",         "KBI0_P0",      "FTM0_CH0",       "I2C0_4WSCLOUT","ACMP0_IN0", "ADC0_SE0",     "-",                 "-"           }, // PTA0
        {  "PTA1",         "KBI0_P1",      "FTM0_CH1",       "I2C0_4WSDAOUT","ACMP0_IN1", "ADC0_SE1",     "-",                 "-"           }, // PTA1
        {  "PTA2",         "KBI0_P2",      "UART0_RX",       "I2C0_SDA",   "-",           "-",            "-",                 "-"           }, // PTA2 (true open-drain output when operating as output)
        {  "PTA3",         "KBI0_P3",      "UART0_TX",       "I2C0_SCL",   "-",           "-",            "-",                 "-"           }, // PTA3 (true open-drain output when operating as output)
        {  "PTA4",         "KBI0_P4",      "-",              "ACMP0_OUT",  "SWD_DIO",     "-",            "-",                 "-"           }, // PTA4 (SWD_DIO default)
        {  "PTA5",         "KBI0_P5",      "IRQ",            "TCLK0",      "RESET_b",     "-",            "-",                 "-"           }, // PTA5 (RESET default)
        {  "PTA6",         "KBI0_P6",      "FTM2_FLT1",      "ACMP1_IN0",  "ADC0_SE2" ,   "-",            "-",                 "-"           }, // PTA6
        {  "PTA7",         "KBI0_P7",      "FTM2_FLT2",      "ACMP1_IN1",  "ADC0_SE3",    "-",            "-",                 "-"           }, // PTA7
    },
    {
        {  "PTB0",         "KBI0_P8",      "UART0_RX",       "PWT_IN1",    "ADC0_SE4",    "-",            "-",                 "-"           }, // PTB0
        {  "PTB1",         "KBI0_P9",      "UART0_TX",       "-",          "ADC0_SE5",    "-",            "-",                 "-"           }, // PTB1
        {  "PTB2",         "KBI0_P6",      "SPI0_SCK",       "FTM0_CH0",   "ADC0_SE6",    "-",            "-",                 "-"           }, // PTB2
        {  "PTB3",         "KBI0_P7",      "SPI0_MOSI",      "FTM0_CH1",   "ADC0_SE7",    "-",            "-",                 "-"           }, // PTB3
        {  "PTB4",         "KBI0_P12",     "FTM2_CH4",       "SPI0_MISO",  "ACMP1_IN2",   "NMI_b",        "-",                 "-"           }, // PTB4 (NMI default) (high current drive pin when operated as an output)
        {  "PTB5",         "KBI0_P13",     "FTM2_CH5",       "SPI0_PCS",   "ACMP1_OUT",   "-",            "-",                 "-"           }, // PTB5 (high current drive pin when operated as an output)
    #if defined KINETIS_KEA32
        {  "PTB6",         "-",            "I2C0_SDA",       "-",          "XTAL",        "-",            "-",                 "-"           }, // PTB6
        {  "PTB7",         "-",            "I2C0_SCL",       "-",          "EXTAL",       "-",            "-",                 "-"           }, // PTB7
    #else
        {  "PTB6",         "KBI0_P14",     "I2C0_SDA",       "-",          "-",           "-",            "XTAL",              "-"           }, // PTB6
        {  "PTB7",         "KBI0_P15",     "I2C0_SCL",       "-",          "-",           "-",            "EXTAL",             "-"           }, // PTB7
    #endif
    },
    {
    #if defined KINETIS_KEAN64
        {  "PTC0",         "-",            "FTM2_CH0",       "-",          "ADC0_SE8",    "-",            "-",                 "-"           }, // PTC0
        {  "PTC1",         "-",            "FTM2_CH1",       "-",          "ADC0_SE9",    "-",            "-",                 "-"           }, // PTC1
        {  "PTC2",         "FTM2_CH2",     "-",              "-",          "ADC0_SE10",   "-",            "-",                 "-"           }, // PTC2
        {  "PTC3",         "FTM2_CH3",     "-",              "-",          "ADC0_SE11",   "-",            "-",                 "-"           }, // PTC3
        {  "PTC4",         "RTCO",         "FTM1_CH0",       "ACMP0_IN2",  "SWD_CLK",     "-",            "-",                 "-"           }, // PTC4 (SWD_CLK default)
        {  "PTC5",         "-",            "FTM1_CH1",       "-",          "RTCO",        "-",            "-",                 "-"           }, // PTC5
        {  "PTC6",         "-",            "UART1_RX",       "-",          "-",           "-",            "-",                 "-"           }, // PTC6
        {  "PTC7",         "-",            "UART1_TX",       "-",          "-",           "-",            "-",                 "-"           }, // PTC7
    #else
        {  "PTC0",         "KBI1_P28",     "FTM2_CH0",       "-",          "ADC0_SE8",    "-",            "-",                 "-"           }, // PTC0
        {  "PTC1",         "KBI0_P17",     "FTM2_CH1",       "-",          "ADC0_SE9",    "-",            "-",                 "-"           }, // PTC1
        {  "PTC2",         "KBI0_P18",     "FTM2_CH2",       "-",          "ADC0_SE10",   "-",            "-",                 "-"           }, // PTC2
        {  "PTC3",         "KBI0_P19",     "FTM2_CH3",       "-",          "ADC0_SE11",   "-",            "-",                 "-"           }, // PTC3
        {  "PTC4",         "KBI0_P20",     "RTC_CLKOUT",     "FTM1_CH0",   "ACMP0_IN2",   "SWD_CLK",      "-",                 "-"           }, // PTC4 (SWD_CLK default)
        {  "PTC5",         "KBI0_P21",     "-",              "FTM1_CH1",   "-",           "RTC_CLKOUT",   "-",                 "-"           }, // PTC5
        {  "PTC6",         "KBI0_P22",     "UART1_RX",       "-",          "-",           "CAN0_RX",      "-",                 "-"           }, // PTC6
        {  "PTC7",         "KBI0_P23",     "UART1_TX",       "-",          "-",           "CAN0_TX",      "-",                 "-"           }, // PTC7
    #endif
    },
    {
        {  "PTD0",         "KBI0_P24",     "FTM2_CH2",       "SPI1_SCK",   "-",           "-",            "-",                 "-"           }, // PTD0 (high current drive pin when operated as an output)
        {  "PTD1",         "KBI0_P25",     "FTM2_CH3",       "SPI1_MOSI",  "-",           "-",            "-",                 "-"           }, // PTD1 (high current drive pin when operated as an output)
        {  "PTD2",         "KBI0_P26",     "SPI1_MISO",      "-",          "-",           "-",            "-",                 "-"           }, // PTD2
        {  "PTD3",         "KBI0_P27",     "SPI1_PCS0",      "-",          "-",           "-",            "-",                 "-"           }, // PTD3
        {  "PTD4",         "KBI0_P28",     "-",              "-",          "-",           "-",            "-",                 "-"           }, // PTD4
        {  "PTD5",         "KBI0_P29",     "PWT_IN0",        "-",          "-",           "-",            "-",                 "-"           }, // PTD5
        {  "PTD6",         "KBI0_P30",     "UART2_RX",       "-",          "-",           "-",            "-",                 "-"           }, // PTD6
        {  "PTD7",         "KBI0_P31",     "UART2_TX",       "-",          "-",           "-",            "-",                 "-"           }, // PTD7
    },
    {
        {  "PTE0",         "KBI1_P0",      "SPI0_SCK",       "TCLK1",      "I2C1_SDA",    "-",            "-",                 "-"           }, // PTE0 (high current drive pin when operated as an output)
        {  "PTE1",         "KBI1_P1",      "SPI0_MOSI",      "-",          "I2C1_SCL",    "-",            "-",                 "-"           }, // PTE1 (high current drive pin when operated as an output)
        {  "PTE2",         "KBI1_P2",      "SPI0_MISO",      "PWT_IN0",    "-",           "-",            "-",                 "-"           }, // PTE2
        {  "PTE3",         "KBI1_P3",      "SPI0_PCS0",      "-",          "-",           "-",            "-",                 "-"           }, // PTE3
        {  "PTE4",         "KBI1_P4",      "-",              "-",          "-",           "-",            "-",                 "-"           }, // PTE4
        {  "PTE5",         "KBI1_P5",      "-",              "-",          "-",           "-",            "-",                 "-"           }, // PTE5
        {  "PTE6",         "KBI1_P6",      "-",              "-",          "-",           "-",            "-",                 "-"           }, // PTE6
        {  "PTE7",         "KBI1_P7",      "TCLK2",          "-",          "FTM1_CH1",    "CAN0_TX",      "-",                 "-"           }, // PTE7
    },
    {
        {  "PTF0",         "KBI1_P8",      "FTM2_CH0",       "-",          "-",           "-",            "-",                 "-"           }, // PTF0
        {  "PTF1",         "KBI1_P9",      "FTM2_CH1",       "-",          "-",           "-",            "-",                 "-"           }, // PTF1
        {  "PTF2",         "KBI1_P10",     "UART1_RX",       "-",          "-",           "-",            "-",                 "-"           }, // PTF2
        {  "PTF3",         "KBI1_P11",     "UART1_TX",       "-",          "-",           "-",            "-",                 "-"           }, // PTF3
        {  "PTF4",         "KBI1_P12",     "-",              "-",          "ADC0_SE12",   "-",            "-",                 "-"           }, // PTF4
        {  "PTF5",         "KBI1_P13",     "-",              "-",          "ADC0_SE13",   "-",            "-",                 "-"           }, // PTF5
        {  "PTF6",         "KBI1_P14",     "-",              "-",          "ADC0_SE14",   "-",            "-",                 "-"           }, // PTF6
        {  "PTF7",         "KBI1_P15",     "-",              "-",          "ADC0_SE15",   "-",            "-",                 "-"           }, // PTF7
    },
    {
        {  "PTG0",         "KBI1_P16",     "-",              "-",          "-",           "-",            "-",                 "-"           }, // PTG0
        {  "PTG1",         "KBI1_P17",     "-",              "-",          "-",           "-",            "-",                 "-"           }, // PTG1
        {  "PTG2",         "KBI1_P18",     "-",              "-",          "-",           "-",            "-",                 "-"           }, // PTG2
        {  "PTG3",         "KBI1_P19",     "-",              "-",          "-",           "-",            "-",                 "-"           }, // PTG3
        {  "PTG4",         "KBI1_P20",     "FTM2_CH2",       "SPI1_SCK",   "-",           "-",            "-",                 "-"           }, // PTG4
        {  "PTG5",         "KBI1_P21",     "FTM2_CH3",       "SPI1_MOSI",  "-",           "-",            "-",                 "-"           }, // PTG5
        {  "PTG6",         "KBI1_P22",     "FTM2_CH4",       "SPI1_MISO",  "-",           "-",            "-",                 "-"           }, // PTG6
        {  "PTG7",         "KBI1_P23",     "FTM2_CH5",       "SPI1_PCS",   "-",           "-",            "-",                 "-"           }, // PTG7
    },
    {
    #if defined KINETIS_KEAN64
        {  "PTH0",         "-",            "FTM2_CH0",       "-",          "-",           "-",            "-",                 "-"           }, // PTH0
        {  "PTH1",         "-",            "FTM2_CH1",       "-",          "-",           "-",            "-",                 "-"           }, // PTH1
        {  "PTH2",         "-",            "BUSOUT",         "-",          "-",           "-",            "-",                 "-"           }, // PTH2
        {  "-",            "-",            "-",              "-",          "-",           "-",            "-",                 "-"           }, // PTH3
        {  "-",            "-",            "-",              "-",          "-",           "-",            "-",                 "-"           }, // PTH4
        {  "-",            "-",            "-",              "-",          "-",           "-",            "-",                 "-"           }, // PTH5
        {  "PTH6",         "-",            "-",              "-",          "-",           "-",            "-",                 "-"           }, // PTH6
        {  "PTH7",         "-",            "-",              "-",          "-",           "-",            "-",                 "-"           }, // PTH7
    #else
        {  "PTH0",         "KBI1_P24",     "FTM2_CH0",       "-",          "-",           "-",            "-",                 "-"           }, // PTH0
        {  "PTH1",         "KBI1_P25",     "FTM2_CH1",       "-",          "-",           "-",            "-",                 "-"           }, // PTH1
        {  "PTH2",         "KBI1_P26",     "BUSOUT",         "-",          "FTM1_CH0",    "CAN0_RX",      "-",                 "-"           }, // PTH2
        {  "PTH3",         "KBI1_P27",     "I2C1_SDA",       "-",          "-",           "-",            "-",                 "-"           }, // PTH3
        {  "PTH4",         "KBI1_P28",     "I2C1_SCL",       "-",          "-",           "-",            "-",                 "-"           }, // PTH4
        {  "PTH5",         "KBI1_P29",     "-",              "-",          "-",           "-",            "-",                 "-"           }, // PTH5
        {  "PTH6",         "KBI1_P30",     "-",              "-",          "-",           "-",            "-",                 "-"           }, // PTH6
        {  "PTH7",         "KBI1_P31",     "PWT_IN1",        "-",          "-",           "-",            "-",                 "-"           }, // PTH7
    #endif
    },
    {
        {  "PTI0",         "-",            "IRQ",            "UART2_RX",   "-",           "-",            "-",                 "-"           }, // PTI0
        {  "PTI1",         "-",            "IRQ",            "UART2_TX",   "-",           "-",            "-",                 "-"           }, // PTI1
        {  "PTI2",         "IRQ",          "-",              "-",          "-",           "-",            "-",                 "-"           }, // PTI2
        {  "PTI3",         "IRQ",          "-",              "-",          "-",           "-",            "-",                 "-"           }, // PTI3
        {  "PTI4",         "-",            "IRQ",            "-",          "-",           "-",            "-",                 "-"           }, // PTI4
        {  "PTI5",         "IRQ",          "-",              "-",          "-",           "-",            "-",                 "-"           }, // PTI5
        {  "PTI6",         "IRQ",          "-",              "-",          "-",           "-",            "-",                 "-"           }, // PTI6
        {  "PTI7",         "-",            "-",              "-",          "-",           "-",            "-",                 "-"           }, // PTI7
    }
};
#elif defined KINETIS_KE14 || defined KINETIS_KE15 || defined KINETIS_KE16 || defined KINETIS_KE18
#if PIN_COUNT == PIN_COUNT_100_PIN                                       // 100 pin LQFP
    #define _PIN_COUNT      0
#elif PIN_COUNT == PIN_COUNT_64_PIN                                      // 64 pin LQFP
    #define _PIN_COUNT      1
    #error "Package not clearly defined!!"
#endif

#define ALTERNATIVE_FUNCTIONS 7

static const char *cPinNumber[PORTS_AVAILABLE + 1][PORT_WIDTH][2] = {
    {
        // LQFP100  LQFP64                                               GPIO A
        {  "79",    "50"   },                                            // PA0
        {  "78",    "49"   },                                            // PA1
        {  "73",    "48"   },                                            // PA2
        {  "72",    "47"   },                                            // PA3
        {  "98",    "64"   },                                            // PA4
        {  "97",    "63"   },                                            // PA5
        {  "58",    "38"   },                                            // PA6
        {  "57",    "37"   },                                            // PA7
        {  "100",   "-"    },                                            // PA8
        {  "99",    "-"    },                                            // PA9
        {  "92",    "58"   },                                            // PA10
        {  "91",    "57"   },                                            // PA11
        {  "90",    "56"   },                                            // PA12
        {  "89",    "55"   },                                            // PA13
        {  "88",    "-"    },                                            // PA14
        {  "83",    "-"    },                                            // PA15
        {  "82",    "-"    },                                            // PA16
        {  "62",    "-"    },                                            // PA17
        {  "-",     "-"    },                                            // PA18
        {  "-",     "-"    },                                            // PA19
        {  "-",     "-"    },                                            // PA20
        {  "-",     "-"    },                                            // PA21
        {  "-",     "-"    },                                            // PA22
        {  "-",     "-"    },                                            // PA23
        {  "-",     "-"    },                                            // PA24
        {  "-",     "-"    },                                            // PA25
        {  "-",     "-"    },                                            // PA26
        {  "-",     "-"    },                                            // PA27
        {  "-",     "-"    },                                            // PA28
        {  "-",     "-"    },                                            // PA29
        {  "-",     "-"    },                                            // PA30
        {  "-",     "-"    },                                            // PA31
    },
    {
        // LQFP100  LQFP64                                               GPIO B
        {  "54",    "34"   },                                            // PB0
        {  "53",    "33"   },                                            // PB1
        {  "48",    "32"   },                                            // PB2
        {  "47",    "31"   },                                            // PB3
        {  "28",    "19"   },                                            // PB4
        {  "27",    "18"   },                                            // PB5
        {  "16",    "12"   },                                            // PB6
        {  "15",    "11"   },                                            // PB7
        {  "77",    "-"    },                                            // PB8
        {  "76",    "-"    },                                            // PB9
        {  "75",    "-"    },                                            // PB10
        {  "74",    "-"    },                                            // PB11
        {  "68",    "43"   },                                            // PB12
        {  "67",    "42"   },                                            // PB13
        {  "66",    "-"    },                                            // PB14
        {  "65",    "-"    },                                            // PB15
        {  "64",    "-"    },                                            // PB16
        {  "63",    "-"    },                                            // PB17
        {  "-",     "-"    },                                            // PB18
        {  "-",     "-"    },                                            // PB19
        {  "-",     "-"    },                                            // PB20
        {  "-",     "-"    },                                            // PB21
        {  "-",     "-"    },                                            // PB22
        {  "-",     "-"    },                                            // PB23
        {  "-",     "-"    },                                            // PB24
        {  "-",     "-"    },                                            // PB25
        {  "-",     "-"    },                                            // PB26
        {  "-",     "-"    },                                            // PB27
        {  "-",     "-"    },                                            // PB28
        {  "-",     "-"    },                                            // PB29
        {  "-",     "-"    },                                            // PB30
        {  "-",     "-"    },                                            // PB31
    },
    {
        // LQFP100  LQFP64                                               GPIO C
        {  "60",    "26"   },                                            // PC0
        {  "39",    "25"   },                                            // PC1
        {  "30",    "21"   },                                            // PC2
        {  "29",    "20"   },                                            // PC3
        {  "96",    "62"   },                                            // PC4
        {  "95",    "61"   },                                            // PC5
        {  "81",    "52"   },                                            // PC6
        {  "80",    "51"   },                                            // PC7
        {  "56",    "36"   },                                            // PC8
        {  "55",    "35"   },                                            // PC9
        {  "52",    "-"    },                                            // PC10
        {  "51",    "-"    },                                            // PC11
        {  "50",    "-"    },                                            // PC12
        {  "49",    "-"    },                                            // PC13
        {  "46",    "30"   },                                            // PC14
        {  "45",    "29"   },                                            // PC15
        {  "44",    "28"   },                                            // PC16
        {  "43",    "27"   },                                            // PC17
        {  "-",     "-"    },                                            // PC18
        {  "-",     "-"    },                                            // PC19
        {  "-",     "-"    },                                            // PC20
        {  "-",     "-"    },                                            // PC21
        {  "-",     "-"    },                                            // PC22
        {  "-",     "-"    },                                            // PC23
        {  "-",     "-"    },                                            // PC24
        {  "-",     "-"    },                                            // PC25
        {  "-",     "-"    },                                            // PC26
        {  "-",     "-"    },                                            // PC27
        {  "-",     "-"    },                                            // PC28
        {  "-",     "-"    },                                            // PC29
        {  "-",     "-"    },                                            // PC30
        {  "-",     "-"    },                                            // PC31
    },
    {
        // LQFP100  LQFP64                                               GPIO D
        {  "4",     "2"    },                                            // PD0
        {  "3",     "1"    },                                            // PD1
        {  "71",    "46"   },                                            // PD2
        {  "70",    "45"   },                                            // PD3
        {  "69",    "44"   },                                            // PD4
        {  "33",    "24"   },                                            // PD5
        {  "32",    "23"   },                                            // PD6
        {  "31",    "22"   },                                            // PD7
        {  "42",    "-"    },                                            // PD8
        {  "41",    "-"    },                                            // PD9
        {  "36",    "-"    },                                            // PD10
        {  "35",    "-"    },                                            // PD11
        {  "34",    "-"    },                                            // PD12
        {  "25",    "-"    },                                            // PD13
        {  "24",    "-"    },                                            // PD14
        {  "22",    "15"   },                                            // PD15
        {  "21",    "14"   },                                            // PD16
        {  "20",    "-"    },                                            // PD17
        {  "-",     "-"    },                                            // PD18
        {  "-",     "-"    },                                            // PD19
        {  "-",     "-"    },                                            // PD20
        {  "-",     "-"    },                                            // PD21
        {  "-",     "-"    },                                            // PD22
        {  "-",     "-"    },                                            // PD23
        {  "-",     "-"    },                                            // PD24
        {  "-",     "-"    },                                            // PD25
        {  "-",     "-"    },                                            // PD26
        {  "-",     "-"    },                                            // PD27
        {  "-",     "-"    },                                            // PD28
        {  "-",     "-"    },                                            // PD29
        {  "-",     "-"    },                                            // PD30
        {  "-",     "-"    },                                            // PD31
    },
    {
        // LQFP100  LQFP64                                               GPIO E
        {  "94",    "60"   },                                            // PE0
        {  "93",    "59"   },                                            // PE1
        {  "85",    "54"   },                                            // PE2
        {  "18",    "13"   },                                            // PE3
        {  "9",     "6"    },                                            // PE4
        {  "8",     "5"    },                                            // PE5
        {  "84",    "53"   },                                            // PE6
        {  "59",    "39"   },                                            // PE7
        {  "26",    "17"   },                                            // PE8
        {  "23",    "16"   },                                            // PE9
        {  "6",     "4"    },                                            // PE10
        {  "5",     "3"    },                                            // PE11
        {  "19",    "-"    },                                            // PE12
        {  "7",     "-"    },                                            // PE13
        {  "17",    "-"    },                                            // PE14
        {  "2",     "-"    },                                            // PE15
        {  "1",     "-"    },                                            // PE16
        {  "-",     "-"    },                                            // PE17
        {  "-",     "-"    },                                            // PE18
        {  "-",     "-"    },                                            // PE19
        {  "-",     "-"    },                                            // PE20
        {  "-",     "-"    },                                            // PE21
        {  "-",     "-"    },                                            // PE22
        {  "-",     "-"    },                                            // PE23
        {  "-",     "-"    },                                            // PE24
        {  "-",     "-"    },                                            // PE25
        {  "-",     "-"    },                                            // PE26
        {  "-",     "-"    },                                            // PE27
        {  "-",     "-"    },                                            // PE28
        {  "-",     "-"    },                                            // PE29
        {  "-",     "-"    },                                            // PE30
        {  "-",     "-"    },                                            // PE31
    },
    {
        // LQFP100  LQFP64                                               dedicated ADC pins
        {  "-",     "10"   },                                            // VREFL/VSS
        {  "12",    "9"    },                                            // VREFH
        {  "13",    "-"    },                                            // VREFL
        {  "-",     "-"    },
        {  "-",     "-"    },
        {  "-",     "-"    },
        {  "-",     "-"    },
        {  "-",     "-"    },
        {  "-",     "-"    },
        {  "-",     "-"    },
        {  "-",     "-"    },
        {  "-",     "-"    },
        {  "-",     "-"    },
        {  "-",     "-"    },
        {  "-",     "-"    },
        {  "-",     "-"    },
        {  "-",     "-"    },
        {  "-",     "-"    },
        {  "-",     "-"    },
        {  "-",     "-"    },
        {  "-",     "-"    },
        {  "-",     "-"    },
        {  "-",     "-"    },
        {  "-",     "-"    },
        {  "-",     "-"    },
        {  "-",     "-"    },
        {  "-",     "-"    },
        {  "-",     "-"    },
        {  "-",     "-"    },
        {  "-",     "-"    },
        {  "-",     "-"    },
        {  "-",     "-"    },                
    }                                        
};                                           
                                             
static int ADC_DEDICATED_CHANNEL[PORT_WIDTH] = {0, 0, 0, 0, 0, 0, 0, 0};
static int ADC_DEDICATED_MODULE[PORT_WIDTH] = {0, 0, 0, 0, 0, 0, 0, 0};
#if defined PORTS_AVAILABLE_8_BIT
static int ADC_MUX_CHANNEL[PORTS_AVAILABLE_8_BIT][PORT_WIDTH] = {
    { -1,-1,-1,-1,-1,-1,-1,-1 }, // port A
    { -1,-1,-1,-1,-1,-1,-1,-1 }, // port B
    #if PORTS_AVAILABLE_8_BIT > 2
    { -1,-1,-1,-1,-1,-1,-1,-1 }, // port C
    #endif
    #if PORTS_AVAILABLE_8_BIT > 3
    { -1,-1,-1,-1,-1,-1,-1,-1 }, // port D
    #endif
    #if PORTS_AVAILABLE_8_BIT > 4
    { -1,-1,-1,-1,-1,-1,-1,-1 }, // port E
    #endif
    #if PORTS_AVAILABLE_8_BIT > 5
    { -1,-1,-1,-1,-1,-1,-1,-1 }, // port F
    #endif
    #if PORTS_AVAILABLE_8_BIT > 6
    { -1,-1,-1,-1,-1,-1,-1,-1 }, // port G
    #endif
    #if PORTS_AVAILABLE_8_BIT > 7
    { -1,-1,-1,-1,-1,-1,-1,-1 }, // port H
    #endif
    #if PORTS_AVAILABLE_8_BIT > 8
    { -1,-1,-1,-1,-1,-1,-1,-1 }, // port I
    #endif
};
#else
static int ADC_MUX_CHANNEL[PORTS_AVAILABLE][PORT_WIDTH] = {
    { -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1 }, // port A
    { -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1 }, // port B
    #if PORTS_AVAILABLE > 2
    { -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1 }, // port C
    #endif
    #if PORTS_AVAILABLE > 3
    { -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1 }, // port D
    #endif
    #if PORTS_AVAILABLE > 4
    { -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1 }, // port E
    #endif
    #if PORTS_AVAILABLE > 5
    { -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1 }, // port F
    #endif
};
#endif
                                             

static const char *cPer[PORTS_AVAILABLE][PORT_WIDTH][8] = {
    {
        // ALT0            ALT 1           ALT 2             ALT3          ALT4           ALT5            ALT6                 ALT 7
    #if defined KINETIS_KE14 || defined KINETIS_KE16 || defined KINETIS_KE18
        {  "ADC0_SE0/ACMP0_IN0","PTA0",    "FTM2_CH1",       "LPI2C0_SCLS","FXIO_D2",     "FTM2_QD_PHA",  "LPUART0_CTS",       "TRGMUX_OUT3" }, // PTA0
        {  "ADC0_SE1/ACMP0_IN1","PTA1",    "FTM1_CH1",       "LPI2C0_SDAS","FXIO_D3",     "FTM1_QD_PHA",  "LPUART0_RTS",       "TRGMUX_OUT0" }, // PTA1
        {  "ADC1_SE0",     "PTA2",         "FTM3_CH0",       "LPI2C0_SDA", "EWM_OUT_b",   "-",            "LPUART0_RX",        "-"           }, // PTA2
        {  "ADC1_SE1",     "PTA3",         "FTM3_CH1",       "LPI2C0_SCL", "EWM_IN",      "-",            "LPUART0_TX",        "-"           }, // PTA2
        {  "-",            "PTA4",         "-",              "-",          "ACMP0_OUT",   "EWM_OUT_b",    "-",                 "JTAG_TMS/SWD_DIO" }, // PTA4 (SWD_DIO default)
        {  "-",            "PTA5",         "-",              "TCLK1",      "-",           "-",            "JTAG_TRST_b",       "RESET_b"     }, // PTA5 (RESET default)
        {  "ADC0_SE2/ACMP1_IN0","PTA6",    "FTM0_FLT1",      "LPSPI1_PCS1","-" ,          "-",            "LPUART1_CTS",       "-"           }, // PTA6
        {  "ADC0_SE3/ACMP1_IN1","PTA7",    "FTM0_FLT2",      "-",          "RTC_CLKIN",   "-",            "LPUART1_RTS",       "-"           }, // PTA7
        {  "-",            "PTA8",         "-",              "-",          "FXIO_D6",     "FTM3_FLT3",    "-",                 "-"           }, // PTA8
        {  "-",            "PTA9",         "-",              "-",          "FXIO_D7",     "FTM3_FLT2",    "FTM1_FLT3",         "-"           }, // PTA9
        {  "-",            "PTA10",        "FTM1_CH4",       "LPUART0_TX", "FXIO_D0",     "-",            "-",                 "JTAG_TDO/noetm_Trace_SWO" }, // PTA10
        {  "-",            "PTA11",        "FTM1_CH5",       "LPUART0_RX", "FXIO_D1",     "-",            "-",                 "-"           }, // PTA11
        {  "ADC2_SE5",     "PTA12",        "FTM1_CH6",       "CAN1_RX",    "LPI2C1_SDAS","-",             "-",                 "-"           }, // PTA12
        {  "ADC2_SE4",     "PTA13",        "FTM1_CH7",       "CAN1_TX",    "LPI2C1_SCLS", "-",            "-",                 "-"           }, // PTA13
        {  "-",            "PTA14",        "FTM0_FLT0",      "FTM1_FLT0",  "EMW_IN",      "-",            "FTM1_FLT0",         "BUSOUT"      }, // PTA14
        {  "ADC1_SE12",    "PTA15",        "FTM1_CH2",       "LPSPI0_PCS3","-",           "-",            "-",                 "-"           }, // PTA15
        {  "ADC1_SE13",    "PTA16",        "FTM1_CH3",       "LPSPI0_PCS2","-",           "-",            "-",                 "-"           }, // PTA16
        {  "-",            "PTA17",        "FTM0_CH6",       "FTM3_FLT0",  "EWM_OUT_b",   "-",            "-",                 "-"           }, // PTA17

    #else
        {  "ADC0_SE0/ACMP0_IN0/TSI0_CH17","PTA0","FTM2_CH1", "LPI2C0_SCLS","FXIO_D2",     "FTM2_QD_PHA",  "LPUART0_CTS",       "TRGMUX_OUT3" }, // PTA0
        {  "ADC0_SE1/ACMP0_IN1/TSI0_CH18","PTA1","FTM1_CH1", "LPI2C0_SDAS","FXIO_D3",     "FTM1_QD_PHA",  "LPUART0_RTS",       "TRGMUX_OUT0" }, // PTA1
        {  "ADC1_SE0",     "PTA2",         "-",              "LPI2C0_SDA", "EWM_OUT_b",   "-",            "LPUART0_RX",        "-"           }, // PTA2
        {  "ADC1_SE1",     "PTA3",         "-",              "LPI2C0_SCL", "EWM_IN",      "-",            "LPUART0_TX",        "-"           }, // PTA2
        {  "-",            "PTA4",         "-",              "-",          "ACMP0_OUT",   "EWM_OUT_b",    "-",                 "SWD_DIO"     }, // PTA4 (SWD_DIO default)
        {  "-",            "PTA5",         "-",              "TCLK1",      "-",           "-",            "-",                 "RESET_b"     }, // PTA5 (RESET default)
        {  "ADC0_SE2/ACMP1_IN0","PTA6",    "FTM0_FLT1",      "LPSPI1_PCS1","-" ,          "-",            "LPUART1_CTS",       "-"           }, // PTA6
        {  "ADC0_SE3/ACMP1_IN1","PTA7",    "FTM0_FLT2",      "-",          "RTC_CLKIN",   "-",            "LPUART1_RTS",       "-"           }, // PTA7
        {  "-",            "PTA8",         "-",              "-",          "FXIO_D6",     "-",            "-",                 "-"           }, // PTA8
        {  "-",            "PTA9",         "-",              "-",          "FXIO_D7",     "-",            "FTM1_FLT3",         "-"           }, // PTA9
        {  "-",            "PTA10",        "-",              "LPUART0_TX", "FXIO_D0",     "-",            "-",                 "-"           }, // PTA10
        {  "-",            "PTA11",        "-",              "LPUART0_RX", "FXIO_D1",     "-",            "-",                 "-"           }, // PTA11
        {  "-",            "PTA12",        "-",              "-",          "LPI2C1_SDAS","-",             "-",                 "-"           }, // PTA12
        {  "-",            "PTA13",        "-",              "-",          "LPI2C1_SCLS", "-",            "-",                 "-"           }, // PTA13
        {  "-",            "PTA14",        "FTM0_FLT0",      "-",          "EMW_IN",      "-",            "-",                 "-"           }, // PTA14
        {  "-",            "PTA15",        "FTM1_CH2",       "LPSPI0_PCS3","-",           "-",            "-",                 "-"           }, // PTA15
        {  "-",            "PTA16",        "FTM1_CH3",       "LPSPI0_PCS2","-",           "-",            "-",                 "-"           }, // PTA16
        {  "-",            "PTA17",        "FTM0_CH6",       "-",          "EWM_OUT_b",   "-",            "-",                 "-"           }, // PTA17
    #endif
        {  "-",            "-",            "-",              "-",          "-",           "-",            "-",                 "-"           }, // PTA18
        {  "-",            "-",            "-",              "-",          "-",           "-",            "-",                 "-"           }, // PTA19
        {  "-",            "-",            "-",              "-",          "-",           "-",            "-",                 "-"           }, // PTA20
        {  "-",            "-",            "-",              "-",          "-",           "-",            "-",                 "-"           }, // PTA21
        {  "-",            "-",            "-",              "-",          "-",           "-",            "-",                 "-"           }, // PTA22
        {  "-",            "-",            "-",              "-",          "-",           "-",            "-",                 "-"           }, // PTA23
        {  "-",            "-",            "-",              "-",          "-",           "-",            "-",                 "-"           }, // PTA24
        {  "-",            "-",            "-",              "-",          "-",           "-",            "-",                 "-"           }, // PTA25
        {  "-",            "-",            "-",              "-",          "-",           "-",            "-",                 "-"           }, // PTA26
        {  "-",            "-",            "-",              "-",          "-",           "-",            "-",                 "-"           }, // PTA27
        {  "-",            "-",            "-",              "-",          "-",           "-",            "-",                 "-"           }, // PTA28
        {  "-",            "-",            "-",              "-",          "-",           "-",            "-",                 "-"           }, // PTA29
        {  "-",            "-",            "-",              "-",          "-",           "-",            "-",                 "-"           }, // PTA30
        {  "-",            "-",            "-",              "-",          "-",           "-",            "-",                 "-"           }, // PTA31
    },
    {
        {  "ADC0_SE4",     "PTB0",         "LPUART0_RX",     "LPSPI0_PCS0","LPTMR0_ALT3", "PWT_IN3",      "-",                 "-"           }, // PTB0
        {  "ADC0_SE5",     "PTB1",         "LPUART0_TX",     "LPSPI0_SOUT","TCLK0",       "-",            "-",                 "-"           }, // PTB1
    #if defined KINETIS_KE14 || defined KINETIS_KE16 || defined KINETIS_KE18
        {  "ADC0_SE6",     "PTB2",         "FTM1_CH0",       "LPSPI0_SCK", "FTM1_QD_PHB", "-",            "TRGMUX_IN3",        "-"           }, // PTB2
        {  "ADC0_SE7",     "PTB3",         "FTM1_CH1",       "LPSPI0_SIN", "FTM1_QD_PHA", "-",            "TRGMUX_IN2",        "-"           }, // PTB3
        {  "ACMP1_IN2",    "PTB4",         "FTM0_CH4",       "LPSPI0_SOUT","-",           "-",            "TRGMUX_IN1",        "-"           }, // PTB4
        {  "-",            "PTB5",         "FTM0_CH5",       "LPSPI0_PCS1","-",           "-",            "TRGMUX_IN0",        "ACMP1_OUT"   }, // PTB5
    #else
        {  "ADC0_SE6/TSI0_CH20","PTB2",    "FTM1_CH0",       "LPSPI0_SCK", "FTM1_QD_PHB", "-",            "TRGMUX_IN3",        "-"           }, // PTB2
        {  "ADC0_SE7/TSI0_CH21","PTB3",    "FTM1_CH1",       "LPSPI0_SIN", "FTM1_QD_PHA", "-",            "TRGMUX_IN2",        "-"           }, // PTB3
        {  "ACMP1_IN2/TSI0_CH8","PTB4",    "FTM0_CH4",       "LPSPI0_SOUT","-",           "-",            "TRGMUX_IN1",        "-"           }, // PTB4
        {  "TSI0_CH9",     "PTB5",         "FTM0_CH5",       "LPSPI0_PCS1","-",           "-",            "TRGMUX_IN0",        "ACMP1_OUT"   }, // PTB5
    #endif
        {  "XTAL",         "PTB6",         "LPI2C0_SDA",     "-",          "-",           "-",            "-",                 "-"           }, // PTB6
        {  "EXTAL",        "PTB7",         "LPI2C0_SCL",     "-",          "-",           "-",            "-",                 "-"           }, // PTB7
    #if defined KINETIS_KE14 || defined KINETIS_KE16 || defined KINETIS_KE18
        {  "ADC2_SE11",    "PTB8",         "FTM3_CH0",       "-",          "-",           "-",            "-",                 "-"           }, // PTB8
        {  "ADC2_SE10",    "PTB9",         "FTM3_CH1",       "I2C0_SCLS",  "-",           "-",            "-",                 "-"           }, // PTB9
        {  "ADC2_SE9",     "PTB10",        "FTM3_CH2",       "I2C0_SDAS",  "-",           "-",            "-",                 "-"           }, // PTB10
        {  "ADC2_SE8",     "PTB11",        "FTM3_CH3",       "I2C0_HREQ",  "-",           "-",            "-",                 "-"           }, // PTB11
        {  "ADC1_SE7",     "PTB12",        "FTM0_CH0",       "FTM3_FLT2",  "-",           "-",            "-",                 "-"           }, // PTB12
        {  "ADC1_SE8",     "PTB13",        "FTM0_CH1",       "FTM3_FLT1",  "-",           "-",            "-",                 "-"           }, // PTB13
    #else
        {  "-",            "PTB8",         "-",              "-",          "-",           "-",            "-",                 "-"           }, // PTB8
        {  "-",            "PTB9",         "-",              "I2C0_SCLS",  "-",           "-",            "-",                 "-"           }, // PTB9
        {  "-",            "PTB10",        "-",              "I2C0_SDAS",  "-",           "-",            "-",                 "-"           }, // PTB10
        {  "-",            "PTB11",        "-",              "I2C0_HREQ",  "-",           "-",            "-",                 "-"           }, // PTB11
        {  "ADC1_SE7",     "PTB12",        "FTM0_CH0",       "-",          "-",           "-",            "-",                 "-"           }, // PTB12
        {  "ADC1_SE8",     "PTB13",        "FTM0_CH1",       "-",          "-",           "-",            "-",                 "-"           }, // PTB13
    #endif
        {  "ADC1_SE9",     "PTB14",        "FTM0_CH2",       "LPSPI1_SCK", "-",           "-",            "-",                 "-"           }, // PTB14
    #if defined KINETIS_KE14 || defined KINETIS_KE16 || defined KINETIS_KE18
        {  "ADC1_SE14",    "PTB15",        "FTM0_CH3",       "LPSPI1_SIN", "-",           "-",            "-",                 "-"           }, // PTB15
        {  "ADC1_SE15",    "PTB16",        "FTM0_CH4",       "LPSPI1_SOUT","-",           "-",            "-",                 "-"           }, // PTB16
        {  "ADC2_SE3",     "PTB17",        "FTM0_CH5",       "LPSPI1_PCS3","-",           "-",            "-",                 "-"           }, // PTB17
    #else
        {  "-",            "PTB15",        "FTM0_CH3",       "LPSPI1_SIN", "-",           "-",            "-",                 "-"           }, // PTB15
        {  "-",            "PTB16",        "FTM0_CH4",       "LPSPI1_SOUT","-",           "-",            "-",                 "-"           }, // PTB16
        {  "-",            "PTB17",        "FTM0_CH5",       "LPSPI1_PCS3","-",           "-",            "-",                 "-"           }, // PTB17
    #endif
        {  "-",            "-",            "-",              "-",          "-",           "-",            "-",                 "-"           }, // PTB18
        {  "-",            "-",            "-",              "-",          "-",           "-",            "-",                 "-"           }, // PTB19
        {  "-",            "-",            "-",              "-",          "-",           "-",            "-",                 "-"           }, // PTB20
        {  "-",            "-",            "-",              "-",          "-",           "-",            "-",                 "-"           }, // PTB21
        {  "-",            "-",            "-",              "-",          "-",           "-",            "-",                 "-"           }, // PTB22
        {  "-",            "-",            "-",              "-",          "-",           "-",            "-",                 "-"           }, // PTB23
        {  "-",            "-",            "-",              "-",          "-",           "-",            "-",                 "-"           }, // PTB24
        {  "-",            "-",            "-",              "-",          "-",           "-",            "-",                 "-"           }, // PTB25
        {  "-",            "-",            "-",              "-",          "-",           "-",            "-",                 "-"           }, // PTB26
        {  "-",            "-",            "-",              "-",          "-",           "-",            "-",                 "-"           }, // PTB27
        {  "-",            "-",            "-",              "-",          "-",           "-",            "-",                 "-"           }, // PTB28
        {  "-",            "-",            "-",              "-",          "-",           "-",            "-",                 "-"           }, // PTB29
        {  "-",            "-",            "-",              "-",          "-",           "-",            "-",                 "-"           }, // PTB30
        {  "-",            "-",            "-",              "-",          "-",           "-",            "-",                 "-"           }, // PTB31
    },
    {
    #if defined KINETIS_KE14 || defined KINETIS_KE16 || defined KINETIS_KE18
        {  "ADC0_SE8/ACMP1_IN4","PTC0",    "FTM0_CH0",       "-",          "-",           "-",            "FTM1_CH6",          "-"           }, // PTC0
        {  "ADC0_SE9/ACMP1_IN3","PTC1",    "FTM0_CH1",       "-",          "-",           "-",            "FTM1_CH7",          "-"           }, // PTC1
        {  "ADC0_SE10/ACMP0_IN5/XTAL32",   "PTC2","FTM0_CH2","CAN0_RX",    "-",           "-",            "-",                 "-"           }, // PTC2
        {  "ADC0_SE11/ACMP0_IN4/EXTAL32",  "PTC3","FTM0_CH3","CAN0_TX",    "-",           "-",            "-",                 "-"           }, // PTC3

    #else
        {  "ADC0_SE8/ACMP1_IN4/TSI0_CH22","PTC0","FTM0_CH0", "-",          "-",           "-",            "-",                 "-"           }, // PTC0
        {  "ADC0_SE9/ACMP1_IN3/TSI0_CH23","PTC1","FTM0_CH1", "-",          "-",           "-",            "-",                 "-"           }, // PTC1
        {  "ADC0_SE10/ACMP0_IN5/XTAL32","PTC2","FTM0_CH2",   "-",          "-",           "-",            "-",                 "-"           }, // PTC2
        {  "ADC0_SE11/ACMP0_IN4/EXTAL32","PTC3","FTM0_CH3",  "-",          "-",           "-",            "-",                 "-"           }, // PTC3
    #endif
        {  "ACMP0_IN2",    "PTC4",         "FTM1_CH0",       "RTC_CLKOUT", "-",           "EWM_IN",       "FTM1_QD_PHB",       "SWD_CLK"     }, // PTC4 (SWD_CLK default)
        {  "TSI0_CH12",    "PTC5",         "FTM2_CH0",       "RTC_CLKOUT", "LPI2C1_HREQ", "-",            "FTM2_QD_PHB",       "-"           }, // PTC5
        {  "ADC1_SE4/TSI0_CH15","PTC6",    "LPUART1_RX",     "-",          "-",           "-",            "-",                 "-"           }, // PTC6
        {  "ADC1_SE5/TSI0_CH16","PTC7",    "LPUART1_TX",     "-",          "-",           "-",            "-",                 "-"           }, // PTC7
        {  "-",            "PTC8",         "LPUART1_RX",     "-",          "-",           "-",            "LPUART0_CTS",       "-"           }, // PTC8
        {  "-",            "PTC9",         "LPUART1_TX",     "-",          "-",           "-",            "LPUART0_RTS",       "-"           }, // PTC9
        {  "-",            "PTC10",        "-",              "-",          "-",           "-",            "-",                 "-"           }, // PTC10
        {  "-",            "PTC11",        "-",              "-",          "-",           "-",            "-",                 "-"           }, // PTC11
        {  "-",            "PTC12",        "-",              "-",          "-",           "-",            "-",                 "-"           }, // PTC12
        {  "-",            "PTC13",        "-",              "-",          "-",           "-",            "-",                 "-"           }, // PTC13
        {  "ADC0_SE12",    "PTC14",        "FTM1_CH2",       "-",          "-",           "-",            "-",                 "-"           }, // PTC14
        {  "ADC0_SE13",    "PTC15",        "FTM1_CH3",       "-",          "-",           "-",            "-",                 "-"           }, // PTC15
        {  "ADC0_SE14",    "PTC16",        "FTM1_FLT2",      "-",          "LPI2C1_SDAS", "-",            "-",                 "-"           }, // PTC16
        {  "ADC0_SE15",    "PTC17",        "FTM1_FLT3",      "-",          "LPI2C1_SCLS", "-",            "-",                 "-"           }, // PTC17
        {  "-",            "-",            "-",              "-",          "-",           "-",            "-",                 "-"           }, // PTC18
        {  "-",            "-",            "-",              "-",          "-",           "-",            "-",                 "-"           }, // PTC19
        {  "-",            "-",            "-",              "-",          "-",           "-",            "-",                 "-"           }, // PTC20
        {  "-",            "-",            "-",              "-",          "-",           "-",            "-",                 "-"           }, // PTC21
        {  "-",            "-",            "-",              "-",          "-",           "-",            "-",                 "-"           }, // PTC22
        {  "-",            "-",            "-",              "-",          "-",           "-",            "-",                 "-"           }, // PTC23
        {  "-",            "-",            "-",              "-",          "-",           "-",            "-",                 "-"           }, // PTC24
        {  "-",            "-",            "-",              "-",          "-",           "-",            "-",                 "-"           }, // PTC25
        {  "-",            "-",            "-",              "-",          "-",           "-",            "-",                 "-"           }, // PTC26
        {  "-",            "-",            "-",              "-",          "-",           "-",            "-",                 "-"           }, // PTC27
        {  "-",            "-",            "-",              "-",          "-",           "-",            "-",                 "-"           }, // PTC28
        {  "-",            "-",            "-",              "-",          "-",           "-",            "-",                 "-"           }, // PTC29
        {  "-",            "-",            "-",              "-",          "-",           "-",            "-",                 "-"           }, // PTC30
        {  "-",            "-",            "-",              "-",          "-",           "-",            "-",                 "-"           }, // PTC31
    },
    {
        {  "TSI0_CH4",     "PTD0",         "FTM0_CH2",       "LPSPI1_SCK", "FTM2_CH0",    "-",            "FXIO_D0",           "TRGMUX_OUT1" }, // PTD0
        {  "TSI0_CH5",     "PTD1",         "FTM0_CH3",       "LPSPI1_SIN", "FTM2_CH1",    "-",            "FXIO_D1",           "TRGMUX_OUT2" }, // PTD1
        {  "ADC1_SE2",     "PTD2",         "-",              "LPSPI1_SOUT","FXIO_D4",     "-",            "TRGMUX_IN5",        "-"           }, // PTD2
        {  "ADC1_SE3",     "PTD3",         "-",              "LPSPI1_PCS0","FXIO_D5",     "-",            "TRGMUX_IN4",        "NMI_b"       }, // PTD3
        {  "ADC1_SE6",     "PTD4",         "FTM0_FLT3",      "-",          "-",           "-",            "-",                 "-"           }, // PTD4
        {  "TSI0_CH6",     "PTD5",         "FTM2_CH3",       "LPTMR0_ALT2","-",           "PWT_IN2",      "TRGMUX_IN7",        "-"           }, // PTD5
        {  "TSI0_CH7",     "PTD6",         "LPUART2_RX",     "-",          "FTM2_FLT2",   "-",            "-",                 "-"           }, // PTD6
        {  "TSI0_CH10",    "PTD7",         "LPUART2_TX",     "-",          "FTM2_FLT3",   "-",            "-",                 "-"           }, // PTD7
        {  "-",            "PTD8",         "LPI2C1_SDA",     "-",          "FTM2_FLT2",   "-",            "-",                 "-"           }, // PTD8
        {  "ACMP1_IN5",    "PTD9",         "LPI2C1_SCL",     "-",          "FTM2_FLT3",   "-",            "-",                 "-"           }, // PTD9
        {  "-",            "PTD10",        "FTM2_CH0",       "FTM2_QD_PHB","-",           "-",            "-",                 "-"           }, // PTD10
        {  "-",            "PTD11",        "FTM2_CH1",       "FTM2_QD_PHA","-",           "-",            "LPUART2_CTS",       "-"           }, // PTD11
        {  "-",            "PTD12",        "FTM2_CH2",       "LPI2C1_HREQ","-",           "-",            "LPUART2_RTS",       "-"           }, // PTD12
        {  "-",            "PTD13",        "-",              "-",          "-",           "-",            "-",                 "RTC_CLKOUT"  }, // PTD13
        {  "-",            "PTD14",        "-",              "-",          "-",           "-",            "-",                 "CLKOUT"      }, // PTD14
        {  "-",            "PTD15",        "FTM0_CH0",       "-",          "-",           "-",            "-",                 "-"           }, // PTD15
        {  "-",            "PTD16",        "FTM0_CH1",       "-",          "-",           "-",            "-",                 "-"           }, // PTD16
        {  "-",            "PTD17",        "FTM0_FLT2",      "LPUART2_RX", "-",           "-",            "-",                 "-"           }, // PTD17
        {  "-",            "-",            "-",              "-",          "-",           "-",            "-",                 "-"           }, // PTD18
        {  "-",            "-",            "-",              "-",          "-",           "-",            "-",                 "-"           }, // PTD19
        {  "-",            "-",            "-",              "-",          "-",           "-",            "-",                 "-"           }, // PTD20
        {  "-",            "-",            "-",              "-",          "-",           "-",            "-",                 "-"           }, // PTD21
        {  "-",            "-",            "-",              "-",          "-",           "-",            "-",                 "-"           }, // PTD22
        {  "-",            "-",            "-",              "-",          "-",           "-",            "-",                 "-"           }, // PTD23
        {  "-",            "-",            "-",              "-",          "-",           "-",            "-",                 "-"           }, // PTD24
        {  "-",            "-",            "-",              "-",          "-",           "-",            "-",                 "-"           }, // PTD25
        {  "-",            "-",            "-",              "-",          "-",           "-",            "-",                 "-"           }, // PTD26
        {  "-",            "-",            "-",              "-",          "-",           "-",            "-",                 "-"           }, // PTD27
        {  "-",            "-",            "-",              "-",          "-",           "-",            "-",                 "-"           }, // PTD28
        {  "-",            "-",            "-",              "-",          "-",           "-",            "-",                 "-"           }, // PTD29
        {  "-",            "-",            "-",              "-",          "-",           "-",            "-",                 "-"           }, // PTD30
        {  "-",            "-",            "-",              "-",          "-",           "-",            "-",                 "-"           }, // PTD31
    },
    {
        {  "TSI0_CH13",    "PTE0",         "LPSPI0_SCK",     "TCLK1",      "LPI2C1_SDA",  "-",            "FTM1_FLT2",         "-"           }, // PTE0
        {  "TSI0_CH14",    "PTE1",         "LPSPI0_SIN",     "LPI2C0_HREQ","LPI2C1_SCL",  "-",            "-",                 "-"           }, // PTE1
        {  "ADC1_SE10/TSI0_CH19","PTE2",   "LPSPI0_SOUT",    "LPTMR0_ALT3","-",           "PWT_IN3",      "LPUART1_CTS",       "-"           }, // PTE2
        {  "TSI0_CH24",    "PTE3",         "FTM0_FLT0",      "LPUART2_RTS","-",           "-",            "TRGMUX_IN6",        "-"           }, // PTE3
        {  "TSI0_CH1",     "PTE4",         "BUSOUT",         "FTM2_QD_PHB","FTM2_CH2",    "-",            "FXIO_D6",           "EWM_OUT_b"   }, // PTE4
        {  "TSI0_CH0",     "PTE5",         "TCLK2",          "FTM2_QD_PHA","FTM2_CH3",    "-",            "FXIO_D7",           "EWM_IN"      }, // PTE5
        {  "ADC1_SE11",    "PTE6",         "LPSPI0_PCS2",    "-",          "-",           "-",            "LPUART1_RTS",       "-"           }, // PTE6
        {  "-",            "PTE7",         "FTM0_CH7",       "-",          "-",           "-",            "-",                 "-"           }, // PTE7
        {  "ACMP0_IN3/TSI0_CH11","PTE8",   "FTM0_CH6",       "-",          "-",           "-",            "-",                 "-"           }, // PTE8
        {  "DAC0_OUT",     "PTE9",         "FTM0_CH7",       "LPUART2_CTS","-",           "-",            "-",                 "-"           }, // PTE9
        {  "TSI0_CH2",     "PTE10",        "CLKOUT",         "-",          "-",           "-",            "FXIO_D4",           "TRGMUX_OUT4" }, // PTE10
        {  "TSI0_CH3",     "PTE11",        "PWT_IN1",        "LPTMR0_ALT1","-",           "-",            "FXIO_D5",           "TRGMUX_OUT5" }, // PTE11
        {  "-",            "PTE12",        "FTM0_FLT3",      "LPUART2_TX", "-",           "-",            "-",                 "-"           }, // PTE12
        {  "-",            "PTE13",        "-",              "-",          "-",           "-",            "-",                 "-"           }, // PTE13
        {  "-",            "PTE14",        "FTM0_FLT1",      "-",          "-",           "-",            "-",                 "-"           }, // PTE14
        {  "-",            "PTE15",        "-",              "-",          "-",           "-",            "FXIO_D2",           "TRGMUX_OUT6" }, // PTE15
        {  "-",            "PTE16",        "-",              "-",          "-",           "-",            "FXIO_D3",           "TRGMUX_OUT7" }, // PTE16
        {  "-",            "-",            "-",              "-",          "-",           "-",            "-",                 "-"           }, // PTE17
        {  "-",            "-",            "-",              "-",          "-",           "-",            "-",                 "-"           }, // PTE18
        {  "-",            "-",            "-",              "-",          "-",           "-",            "-",                 "-"           }, // PTE19
        {  "-",            "-",            "-",              "-",          "-",           "-",            "-",                 "-"           }, // PTE20
        {  "-",            "-",            "-",              "-",          "-",           "-",            "-",                 "-"           }, // PTE21
        {  "-",            "-",            "-",              "-",          "-",           "-",            "-",                 "-"           }, // PTE22
        {  "-",            "-",            "-",              "-",          "-",           "-",            "-",                 "-"           }, // PTE23
        {  "-",            "-",            "-",              "-",          "-",           "-",            "-",                 "-"           }, // PTE24
        {  "-",            "-",            "-",              "-",          "-",           "-",            "-",                 "-"           }, // PTE25
        {  "-",            "-",            "-",              "-",          "-",           "-",            "-",                 "-"           }, // PTE26
        {  "-",            "-",            "-",              "-",          "-",           "-",            "-",                 "-"           }, // PTE27
        {  "-",            "-",            "-",              "-",          "-",           "-",            "-",                 "-"           }, // PTE28
        {  "-",            "-",            "-",              "-",          "-",           "-",            "-",                 "-"           }, // PTE29
        {  "-",            "-",            "-",              "-",          "-",           "-",            "-",                 "-"           }, // PTE30
        {  "-",            "-",            "-",              "-",          "-",           "-",            "-",                 "-"           }, // PTE31
    },
};
#endif

