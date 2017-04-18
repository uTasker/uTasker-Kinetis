/***********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher

    ---------------------------------------------------------------------
    File:      kinetis_port_k70.h
    Project:   Single Chip Embedded Internet
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..201
    *********************************************************************
    
*/


#if PIN_COUNT == PIN_COUNT_196_PIN                                       // 196 MAPBGA
    #define _PIN_COUNT              0
#elif PIN_COUNT == PIN_COUNT_256_PIN                                     // 256 MAPBGA
    #define _PIN_COUNT              1
#else
    #error "Package not clearly defined!!"
#endif

#define ALTERNATIVE_FUNCTIONS   7                                        // GPIO plus 5 possible peripheral/extra/system functions

static const char *cPinNumber[PORTS_AVAILABLE + 1][PORT_WIDTH][2] = {
    {
        //  MAPBGA196   MAPBGA256                                        GPIO A
        {  "N6",       "T7",    },                                       // PA0
        {  "P7",       "N8",    },                                       // PA1
        {  "N7",       "T8",    },                                       // PA2
        {  "M7",       "P8",    },                                       // PA3
        {  "L7",       "R8",    },                                       // PA4
        {  "P8",       "T12",   },                                       // PA5
        {  "M10",      "R12",   },                                       // PA6
        {  "L10",      "P12",   },                                       // PA7
        {  "P11",      "N12",   },                                       // PA8
        {  "N11",      "T13",   },                                       // PA9
        {  "N12",      "P13",   },                                       // PA10
        {  "N13",      "R13",   },                                       // PA11
        {  "K6",       "M10",   },                                       // PA12
        {  "K7",       "N10",   },                                       // PA13
        {  "J6",       "R11",   },                                       // PA14
        {  "K8",       "P11",   },                                       // PA15
        {  "K12",      "N11",   },                                       // PA16
        {  "K11",      "T11",   },                                       // PA17
        {  "P14",      "T15",   },                                       // PA18
        {  "P13",      "T16",   },                                       // PA19
        {  "-",        "-",     },                                       // PA20
        {  "-",        "-",     },                                       // PA21
        {  "-",        "-",     },                                       // PA22
        {  "-",        "-",     },                                       // PA23
        {  "M12",      "N13",   },                                       // PA24
        {  "M11",      "R14",   },                                       // PA25
        {  "L12",      "M13",   },                                       // PA26
        {  "L11",      "R15",   },                                       // PA27
        {  "M13",      "P14",   },                                       // PA28
        {  "L13",      "N14",   },                                       // PA29
        {  "-",        "-",     },                                       // PA30
        {  "-",        "-",     },                                       // PA31
    },
    {
        //  MAPBGA196   MAPBGA256                                        GPIO B
        {  "M14",      "M12",   },                                       // PB0
        {  "L14",      "M11",   },                                       // PB1
        {  "K13",      "P15",   },                                       // PB2
        {  "J12",      "M14",   },                                       // PB3
        {  "K14",      "N15",   },                                       // PB4
        {  "J11",      "M15",   },                                       // PB5
        {  "J13",      "L14",   },                                       // PB6
        {  "J14",      "L15",   },                                       // PB7
        {  "H14",      "K14",   },                                       // PB8
        {  "H13",      "K15",   },                                       // PB9
        {  "H12",      "J13",   },                                       // PB10
        {  "H11",      "J14",   },                                       // PB11
        {  "-",        "-",     },                                       // PB12
        {  "-",        "-",     },                                       // PB13
        {  "-",        "-",     },                                       // PB14
        {  "-",        "-",     },                                       // PB15
        {  "G12",      "J15",   },                                       // PB16
        {  "G11",      "H13",   },                                       // PB17
        {  "F14",      "H14",   },                                       // PB18
        {  "F12",      "H15",   },                                       // PB19
        {  "E14",      "G13",   },                                       // PB20
        {  "F11",      "G14",   },                                       // PB21
        {  "D14",      "G15",   },                                       // PB22
        {  "E12",      "H16",   },                                       // PB23
        {  "-",        "-",     },                                       // PB24
        {  "-",        "-",     },                                       // PB25
        {  "-",        "-",     },                                       // PB26
        {  "-",        "-",     },                                       // PB27
        {  "-",        "-",     },                                       // PB28
        {  "-",        "-",     },                                       // PB29
        {  "-",        "-",     },                                       // PB30
        {  "-",        "-",     },                                       // PB31
    },
    {
        //  MAPBGA196   MAPBGA256                                        GPIO C
        {  "C14",      "G16",   },                                       // PC0
        {  "C13",      "F13",   },                                       // PC1
        {  "B14",      "F14",   },                                       // PC2
        {  "C12",      "E13",   },                                       // PC3
        {  "C11",      "E14",   },                                       // PC4
        {  "B13",      "E15",   },                                       // PC5
        {  "B12",      "F12",   },                                       // PC6
        {  "A13",      "G12",   },                                       // PC7
        {  "C10",      "H12",   },                                       // PC8
        {  "B11",      "F11",   },                                       // PC9
        {  "A12",      "G11",   },                                       // PC10
        {  "B10",      "H11",   },                                       // PC11
        {  "A11",      "J12",   },                                       // PC12
        {  "A10",      "K13",   },                                       // PC13
        {  "B9",       "J11",   },                                       // PC14
        {  "C9",       "F10",   },                                       // PC15
        {  "A9",       "F9",    },                                       // PC16
        {  "B8",       "E9",    },                                       // PC17
        {  "A8",       "M9",    },                                       // PC18
        {  "A7",       "M8",    },                                       // PC19
        {  "-",        "-",     },                                       // PC20
        {  "-",        "-",     },                                       // PC21
        {  "-",        "-",     },                                       // PC22
        {  "-",        "-",     },                                       // PC23
        {  "-",        "-",     },                                       // PC24
        {  "-",        "-",     },                                       // PC25
        {  "-",        "-",     },                                       // PC26
        {  "-",        "-",     },                                       // PC27
        {  "-",        "-",     },                                       // PC28
        {  "-",        "-",     },                                       // PC29
        {  "-",        "-",     },                                       // PC30
        {  "-",        "-",     },                                       // PC31
    },
    {
        //  MAPBGA196   MAPBGA256                                        GPIO D
        {  "B7",       "L8",    },                                       // PD0
        {  "A6",       "F8",    },                                       // PD1
        {  "B6",       "K6",    },                                       // PD2
        {  "A5",       "J6",    },                                       // PD3
        {  "B5",       "K5",    },                                       // PD4
        {  "C6",       "J5",    },                                       // PD5
        {  "A4",       "K4",    },                                       // PD6
        {  "B4",       "E7",    },                                       // PD7
        {  "C5",       "J4",    },                                       // PD8
        {  "A3",       "F7",    },                                       // PD9
        {  "A2",       "E6",    },                                       // PD10
        {  "B3",       "G5",    },                                       // PD11
        {  "C4",       "F5",    },                                       // PD12
        {  "B2",       "F4",    },                                       // PD13
        {  "B1",       "E5",    },                                       // PD14
        {  "C3",       "E4",    },                                       // PD15
        {  "-",        "-",     },                                       // PD16
        {  "-",        "-",     },                                       // PD17
        {  "-",        "-",     },                                       // PD18
        {  "-",        "-",     },                                       // PD19
        {  "-",        "-",     },                                       // PD20
        {  "-",        "-",     },                                       // PD21
        {  "-",        "-",     },                                       // PD22
        {  "-",        "-",     },                                       // PD23
        {  "-",        "-",     },                                       // PD24
        {  "-",        "-",     },                                       // PD25
        {  "-",        "-",     },                                       // PD26
        {  "-",        "-",     },                                       // PD27
        {  "-",        "-",     },                                       // PD28
        {  "-",        "-",     },                                       // PD29
        {  "-",        "-",     },                                       // PD30
        {  "-",        "-",     },                                       // PD31
    },
    {
        //  MAPBGA196   MAPBGA256                                        GPIO E
        {  "C2",       "E2",    },                                       // PE0
        {  "C1",       "F2",    },                                       // PE1
        {  "D3",       "F3",    },                                       // PE2
        {  "E3",       "G2",    },                                       // PE3
        {  "D2",       "G3",    },                                       // PE4
        {  "D1",       "G4",    },                                       // PE5
        {  "F3",       "H2",    },                                       // PE6
        {  "E2",       "H3",    },                                       // PE7
        {  "G3",       "H4",    },                                       // PE8
        {  "E1",       "J1",    },                                       // PE9
        {  "F2",       "J2",    },                                       // PE10
        {  "F1",       "K1",    },                                       // PE11
        {  "G2",       "K3",    },                                       // PE12
        {  "-",        "-",     },                                       // PE13
        {  "-",        "-",     },                                       // PE14
        {  "-",        "-",     },                                       // PE15
        {  "G1",       "J4",    },                                       // PE16
        {  "H1",       "K2",    },                                       // PE17
        {  "H3",       "L4",    },                                       // PE18
        {  "H2",       "M3",    },                                       // PE19
        {  "-",        "-",     },                                       // PE20
        {  "-",        "-",     },                                       // PE21
        {  "-",        "-",     },                                       // PE22
        {  "-",        "-",     },                                       // PE23
        {  "L5",       "P7",    },                                       // PE24
        {  "M5",       "R7",    },                                       // PE25
        {  "H5",       "M7",    },                                       // PE26
        {  "G5",       "K7",    },                                       // PE27
        {  "H6",       "L7",    },                                       // PE28
        {  "-",        "-",     },                                       // PE29
        {  "-",        "-",     },                                       // PE30
        {  "-",        "-",     },                                       // PE31
    },
    {
        //  MAPBGA196   MAPBGA256                                        GPIO F
        {  "K10",      "P16",   },                                       // PF0
        {  "K9",       "L13",   },                                       // PF1
        {  "G14",      "N16",   },                                       // PF2
        {  "G13",      "M16",   },                                       // PF3
        {  "F13",      "L16",   },                                       // PF4
        {  "E13",      "K16",   },                                       // PF5
        {  "D13",      "J16",   },                                       // PF6
        {  "D12",      "F15",   },                                       // PF7
        {  "D11",      "F16",   },                                       // PF8
        {  "D10",      "K12",   },                                       // PF9
        {  "D9",       "L12",   },                                       // PF10
        {  "C8",       "K11",   },                                       // PF11
        {  "C7",       "L11",   },                                       // PF12
        {  "D6",       "H6",    },                                       // PF13
        {  "D8",       "G6",    },                                       // PF14
        {  "D5",       "F6",    },                                       // PF15
        {  "D4",       "E1",    },                                       // PF16
        {  "E4",       "F1",    },                                       // PF17
        {  "E5",       "G1",    },                                       // PF18
        {  "D7",       "H1",    },                                       // PF19
        {  "E6",       "H5",    },                                       // PF20
        {  "M8",       "P9",    },                                       // PF21
        {  "N8",       "N9",    },                                       // PF22
        {  "L9",       "P10",   },                                       // PF23
        {  "M9",       "R10",   },                                       // PF24
        {  "N9",       "R9",    },                                       // PF25
        {  "N10",      "T9",    },                                       // PF26
        {  "P10",      "T10",   },                                       // PF27
        {  "-",        "-",     },                                       // PF28
        {  "-",        "-",     },                                       // PF29
        {  "-",        "-",     },                                       // PF30
        {  "-",        "-",     },                                       // PF31
    },
    { 
        // MAPBGA196, MAPBGA256                                          dedicated ADC pins
        {  "-",       "-",   },                                          // ADC0_DP0
        {  "-",       "-",   },                                          // ADC0_DM0
        {  "-",       "-",   },                                          // ADC0_DP1
        {  "-",       "-",   },                                          // ADC0_DM1
        {  "N1",      "R1",  },                                          // PGA0_DP
        {  "N2",      "R2",  },                                          // PGA0_DM
        {  "-",       "-",   },                                          // ADC0_DP3
        {  "-",       "-",   },                                          // ADC0_DM3
        {  "-",       "-",   },                                          // ADC1_DP0
        {  "-",       "-",   },                                          // ADC1_DM0
        {  "-",       "-",   },                                          // ADC1_DP1
        {  "-",       "-",   },                                          // ADC1_DM1
        {  "P1",      "T1",  },                                          // PGA1_DP
        {  "P2",      "T2",  },                                          // PGA1_DM
        {  "-",       "-",   },                                          // ADC1_DP3
        {  "-",       "-",   },                                          // ADC1_DM3
        {  "-",       "-",   },                                          // ADC2_DP0
        {  "-",       "-",   },                                          // ADC2_DM0
        {  "-",       "-",   },                                          // ADC2_DP1
        {  "-",       "-",   },                                          // ADC2_DM1
        {  "L1",      "N1",  },                                          // PGA2_DP
        {  "L2",      "N2",  },                                          // PGA2_DM
        {  "-",       "-",   },                                          // ADC2_DP3
        {  "-",       "-",   },                                          // ADC2_DM3
        {  "-",       "-",   },                                          // ADC3_DP0
        {  "-",       "-",   },                                          // ADC3_DM0
        {  "-",       "-",   },                                          // ADC3_DP1
        {  "-",       "-",   },                                          // ADC3_DM1
        {  "M1",      "P1",  },                                          // PGA3_DP
        {  "M2",      "P2",  },                                          // PGA3_DM / ADC1_DM1 / ADC2_DM3 / ADC3_DM0
        {  "-",       "-",   },                                          // ADC3_DP3
        {  "-",       "-",   },                                          // ADC3_DM3
    }
};

static int ADC_DEDICATED_CHANNEL[PORT_WIDTH] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, ADC_DM1_SINGLE, 0, 0};
static int ADC_DEDICATED_MODULE[PORT_WIDTH] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0};
static int ADC_MUX_CHANNEL[PORTS_AVAILABLE][PORT_WIDTH] = {0};


#if defined DEVICE_196_PIN
static const char *cPer[PORTS_AVAILABLE][PORT_WIDTH][8] = {              // 196 pin MAPBGA
    {
        // ALT 0           ALT 1    ALT2         ALT 3         ALT 4          ALT 5        ALT 6       ALT 7
        {  "TSI0_CH1",     "PTA0",  "UART0_CTS_b/UART0_COL_b", "FTM0_CH5","-","-",         "-",        "JTAG_TCLK/SWD_CLK" }, // PORT A
        {  "TSI0_CH2",     "PTA1",  "UART0_RX",  "FTM0_CH6",   "-",           "-",         "-",        "JTAG_TDI"          },
        {  "TSI0_CH3",     "PTA2",  "UART0_TX",  "FTM0_CH7",   "-",           "-",         "-",        "JTAG_TDI/TRACE_SWO"},
        {  "TSI0_CH4",     "PTA3",  "UART0_RTS_b","FTM0_CH0",  "-",           "-",         "-",        "JTAG_TMS/SWD_DIO"  },
        {  "TSI0_CH5",     "PTA4",  "-",         "FTM0_CH1",   "-",           "-",         "-",        "NMI_b"             },
        {  "-",            "PTA5",  "USB_CLKIN", "FTM0_CH2",   "MII_RXER",    "CMP2_OUT",  "I2S0_TX_BCLK","JTAG_TRST"      },
        {  "ADC3_SE6a",    "PTA6",  "ULPI_CLK",  "FTM0_CH3",   "I2S1_RXD0",   "-",         "-",        "TRACE_CLKOUT"      },
        {  "ADC0_SE10",    "PTA7",  "ULPI_DIR",  "FTM0_CH4",   "I2S1_RX_BCLK","-",         "-",        "TRACE_D3"          },
        {  "ADC0_SE11",    "PTA8",  "ULPI_NXT",  "FTM1_CH0",   "I2S1_RX_FS",  "-",         "FTM1_QD_PHA","TRACE_D2"        },
        {  "ADC3_SE5a",    "PTA9",  "ULPI_STP",  "FTM1_CH1",   "MII_RXD3",    "-",         "FTM1_QD_PHB","TRACE_D1"        },
        {  "ADC3_SE4a",    "PTA10", "ULPI_DATA0","FTM2_CH0",   "MII_RXD2",    "-",         "FTM2_QD_PHA","TRACE_D0"        },
        {  "ADC3_SE15",    "PTA11", "ULPI_DATA1","FTM2_CH1",   "MII_RXCLK",   "-",         "FTM2_QD_PHB","-"               },
        {  "CMP2_IN0",     "PTA12", "CAN0_TX",   "FTM1_CH0",   "MII_RXD1",    "-",         "I2S0_TXD", "FTM1_QD_PHA"       },
        {  "CMP2_IN1",     "PTA13", "CAN0_RX",   "FTM1_CH1",   "MII_RXD0",    "-",         "I2S0_TX_FS","FTM1_QD_PHB"      },
        {  "CMP3_IN0",     "PTA14", "SPI0_PCS0", "UART0_TX",   "MII_RXDV",    "-",         "I2S0_RX_BCLK","I2S0_TXD1"      },
        {  "CMP3_IN1",     "PTA15", "SPI0_SCK",  "UART0_RX",   "MII_TXEN",    "-",         "I2S0_RXD0","-"                 },
        {  "CMP3_IN2",     "PTA16", "SPI0_SOUT", "UART0_CTS_b/UART0_COL_b","MII_TXD0", "-","I2S0_RX_FS","I2S0_RXD1"        },
        {  "ADC1_SE17",    "PTA17", "SPI0_SIN",  "UART0_RTS_b","MII_TXD1",    "-",         "I2S0_MCLK","-"                 },
        {  "EXTAL0",       "PTA18", "-",         "FTM0_FLT2",  "FTM_CLKIN0",  "-",         "-",        "-"                 },
        {  "XTAL0",        "PTA19", "-",         "FTM1_FLT0",  "FTM_CLKIN1",  "-",         "LPT0_ALT1","-"                 },
        {  "-",            "-",     "-",         "-",          "-",           "-",         "-",        "-"                 },
        {  "-",            "-",     "-",         "-",          "-",           "-",         "-",        "-"                 },
        {  "-",            "-",     "-",         "-",          "-",           "-",         "-",        "-"                 },
        {  "-",            "-",     "-",         "-",          "-",           "-",         "-",        "-"                 },
        {  "CMP3_IN4",     "PTA24", "ULPI_DATA2","-",          "MII_TXD2",    "-",         "FB_A29",   "-"                 },
        {  "CMP3_IN5",     "PTA25", "ULPI_DATA3","-",          "MII_TXCLK",   "-",         "FB_A28",   "-"                 },
        {  "ADC2_SE15",    "PTA26", "ULPI_DATA4","-",          "MII_TXD3",    "-",         "FB_A27",   "-"                 },                
        {  "ADC2_SE14",    "PTA27", "ULPI_DATA5","-",          "MII_CRS",     "-",         "FB_A26",   "-"                 },
        {  "ADC2_SE13",    "PTA28", "ULPI_DATA6","-",          "MII_TXER",    "-",         "FB_A25",   "-"                 },
        {  "ADC2_SE12",    "PTA29", "ULPI_DATA7","-",          "MII_COL",     "-",         "FB_A24",   "-"                 },
        {  "-",            "-",     "-",         "-",          "-",           "-",         "-",        "-"                 },
        {  "-",            "-",     "-",         "-",          "-",           "-",         "-",        "-"                 }
    },
    {
        // ALT 0           ALT 1    ALT2         ALT 3         ALT 4          ALT 5        ALT 6       ALT 7
        {  "ADC0_SE8/TSI0_CH0","PTB0","I2C0_SCL","FTM1_CH0",   "MII_MDIO",    "-",         "FTM1_QD_PHA", "-"              }, // PORT B
        {  "ADC0_SE9/TSI0_CH6","PTB1","I2C0_SDA","FTM1_CH1",   "MII_MCD",     "-",         "FTM1_QD_PHB", "-"              },
        {  "ADC0_SE12/TSI0_CH7","PTB2","I2C0_SCL","UART0_RTS_b","ENET_1588_TMR0","-",      "FTM0_FLT3","-"                 },
        {  "ADC0_SE13/TSI0_CH8","PTB3","I2C0_SDA","UART0_CTS_b/UART0_COL_b","ENET_1588_TMR1","-","FTM0_FLT0","-"           },
        {  "ADC1_SE10",    "PTB4",  "GLCD_CONTRAST","-",       "ENET_1588_TMR2","-",       "FTM1_FLT0","-"                 },
        {  "ADC1_SE11",    "PTB5",  "-",         "-",          "ENET_1588_TMR3","-",       "FTM2_FLT0","-"                 },
        {  "ADC1_SE12",    "PTB6",  "-",         "-",          "-",           "FB_AD23",   "-",        "-"                 },
        {  "ADC1_SE13",    "PTB7",  "-",         "-",          "-",           "FB_AD22",   "-",        "-"                 },
        {  "-",            "PTB8",  "-",         "UART3_RTS_b","-",           "FB_AD21",   "-",        "-"                 },
        {  "-",            "PTB9",  "SPI1_PCS1", "UART3_CTS_b","-",           "FB_AD20",   "-",        "-"                 },
        {  "ADC1_SE14",    "PTB10", "SPI1_PCS0", "UART3_RX",   "I2S1_TX_BCLK","FB_AD19",   "FTM0_FLT1","-"                 },
        {  "ADC1_SE15",    "PTB11", "SPI1_SCK",  "UART3_TX",   "I2S1_TX_FS",  "FB_AD18",   "FTM0_FLT2","-"                 },
        {  "-",            "-",     "-",         "-",          "-",           "-",         "-",        "-"                 },
        {  "-",            "-",     "-",         "-",          "-",           "-",         "-",        "-"                 },
        {  "-",            "-",     "-",         "-",          "-",           "-",         "-",        "-"                 },
        {  "-",            "-",     "-",         "-",          "-",           "-",         "-",        "-"                 },
        {  "TSI0_CH9",     "PTB16", "SPI1_SOUT", "UART0_RX",   "I2S1_TXD0",    "FB_AD17",   "EWM_IN",   "-"                },
        {  "TSI0_CH10",    "PTB17", "SPI1_SIN",  "UART0_TX",   "I2S1_TXD1",    "FB_AD16",   "EWM_OUT_b","-"                },
        {  "TSI0_CH11",    "PTB18", "CAN0_TX",   "FTM2_CH0",   "I2S0_TX_BCLK", "FB_AD15",   "FTM2_QD_PHA","-"              },
        {  "TSI0_CH12",    "PTB19", "CAN0_RX",   "FTM2_CH1",   "I2S0_TX_FS",   "FB_OE_b",   "FTM2_QD_PHB","-"              },
        {  "ADC2_SE4a",    "PTB20", "SPI2_PCS0", "-",          "-",            "FB_AD31/NFC_DATA15","CMP0_OUT","-"         },
        {  "ADC2_SE5a",    "PTB21", "SPI2_SCK",  "-",          "-",            "FB_AD30/NFC_DATA14","CMP1_OUT","-"         },
        {  "-",            "PTB22", "SPI2_SOUT", "-",          "-",            "FB_AD29/NFC_DATA13","CMP2_OUT","-"         },
        {  "-",            "PTB23", "SPI2_SIN",  "SPI0_PCS5",  "-",            "FB_AD28/NFC_DATA12","CMP3_OUT","-"         },
        {  "-",            "-",     "-",         "-",          "-",           "-",         "-",        "-"                 },
        {  "-",            "-",     "-",         "-",          "-",           "-",         "-",        "-"                 },
        {  "-",            "-",     "-",         "-",          "-",           "-",         "-",        "-"                 },
        {  "-",            "-",     "-",         "-",          "-",           "-",         "-",        "-"                 },
        {  "-",            "-",     "-",         "-",          "-",           "-",         "-",        "-"                 },
        {  "-",            "-",     "-",         "-",          "-",           "-",         "-",        "-"                 },
        {  "-",            "-",     "-",         "-",          "-",           "-",         "-",        "-"                 },
        {  "-",            "-",     "-",         "-",          "-",           "-",         "-",        "-"                 }
    },
    {
        // ALT 0           ALT 1    ALT2         ALT 3         ALT 4          ALT 5        ALT 6       ALT 7
        {  "ADC0_SE14/TSI0_CH13","PTC0","SPI0_PCS4","PDB0_EXTRG","-",         "FB_AD14/NFC_DATA11","I2S0_TXD1","-"         }, // PORT C
        {  "ADC0_SE15/TSI0_CH14","PTC1","SPI0_PCS3","UART1_RTS_b","FTM0_CH0", "FB_AD13/NFC_DATA10","I2S0_TXD0","-"         },
        {  "ADC0_SE4b/CMP1_IN0/TSI0_CH15","PTC2","SPI0_PCS2","UART1_CTS_b","FTM0_CH1","FB_AD12/NFC_DATA9","I2S0_TX_FS","-" },
        {  "CMP1_IN1",     "PTC3",  "SPI0_PCS1", "UART1_RX",   "FTM0_CH2",    "-",         "I2S0_TX_BCLK","-"              },
        {  "-",            "PTC4",  "SPI0_PCS0", "UART1_TX",   "FTM0_CH3",    "FB_AD11/NFC_DATA8","CMP1_OUT", "I2S1_TX_BCLK" },
        {  "-",            "PTC5",  "SPI0_SCK",  "LPTMR0_ALT2","I2S0_RXD0",   "FB_AD10/NFC_DATA7","CMP0_OUT", "I2S1_TX_FS" },
        {  "CMP0_IN0",     "PTC6",  "SPI0_SOUT", "PDB0_EXTRG", "I2S0_RX_BCLK","FB_AD9/NFC_DATA6","I2S0_MCLK", "-"          },
        {  "CMP0_IN1",     "PTC7",  "SPI0_SIN",  "USB_SOF_OUT","I2S0_RX_FS",  "FB_AD8/NFC_DATA5","-",  "-"                 },
        {  "ADC1_SE4b/CMP0_IN2","PTC8","-",      "FTM3_CH4",   "I2S0_MCLK",   "FB_AD7/NFC_DATA4","-",  "-"                 },
        {  "ADC1_SE5b/CMP0_IN3","PTC9","-",      "FTM3_CH5",   "I2S0_RX_BCLK","FB_AD6/NFC_DATA3","FTM2_FLT0","-"           },
        {  "ADC1_SE6b",    "PTC10", "I2C1_SCL",  "FTM3_CH6",   "I2S0_RX_FS",  "FB_AD5/NFC_DATA2","I2S1_MCLK","-"           },
        {  "ADC1_SE7b",    "PTC11", "I2C1_SDA",  "FTM3_CH7",   "I2S0_RXD1",   "FB_RW_b/NFC_WE","-",     "-"                },
        {  "-",            "PTC12", "-",         "UART4_RTS_b","-",           "FB_AD27",   "FTM3_FLT0", "-"                },
        {  "-",            "PTC13", "-",         "UART4_CTS_b","-",           "FB_AD26",   "-",        "-"                 },
        {  "-",            "PTC14", "-",         "UART4_RX",   "-",           "FB_AD25",   "-",        "-"                 },
        {  "-",            "PTC15", "-",         "UART4_TX",   "-",           "FB_AD24",   "-",        "-"                 },
        {  "-",            "PTC16", "CAN1_RX",   "UART3_RX",   "ENET_1588_TMR0","FB_CS5_b/FB_TSIZ1/FB_BE23_16_BLS15_8_b","NFC_RB","-"},
        {  "-",            "PTC17", "CAN1_TX",   "UART3_TX",   "ENET_1588_TMR1","FB_CS4_b/FB_TSIZ0/FB_BE31_24_BLS7_0_b","NFC_CE0_b","-"},
        {  "-",            "PTC18", "-",         "UART3_RTS_b","ENET_1588_TMR2","FB_TBST_b/FB_CS2_b/FB_BE15_8_BLS23_16_b","NFC_CE1_b","-"},
        {  "-",            "PTC19", "-",         "UART3_CTS_b","ENET_1588_TMR3","FB_CS3_b/FB_BE7_0_BLS31_24_b","FB_TA_b","-"     },
        {  "-",            "-",     "-",         "-",          "-",           "-",         "-",        "-"                 },
        {  "-",            "-",     "-",         "-",          "-",           "-",         "-",        "-"                 },
        {  "-",            "-",     "-",         "-",          "-",           "-",         "-",        "-"                 },
        {  "-",            "-",     "-",         "-",          "-",           "-",         "-",        "-"                 },
        {  "-",            "-",     "-",         "-",          "-",           "-",         "-",        "-"                 },
        {  "-",            "-",     "-",         "-",          "-",           "-",         "-",        "-"                 },
        {  "-",            "-",     "-",         "-",          "-",           "-",         "-",        "-"                 },
        {  "-",            "-",     "-",         "-",          "-",           "-",         "-",        "-"                 },
        {  "-",            "-",     "-",         "-",          "-",           "-",         "-",        "-"                 },
        {  "-",            "-",     "-",         "-",          "-",           "-",         "-",        "-"                 },
        {  "-",            "-",     "-",         "-",          "-",           "-",         "-",        "-"                 },
        {  "-",            "-",     "-",         "-",          "-",           "-",         "-",        "-"                 }
    },
    {
        // ALT 0           ALT 1    ALT2         ALT 3         ALT 4          ALT 5        ALT 6       ALT 7
        {  "-",            "PTD0",  "SPI0_PCS0", "UART2_RTS_b","FTM3_CH0",    "FB_ALE/FB_CS1_b/FB_TS_b","I2S1_RXD1","-"    }, // PORT D
        {  "ADC0_SE5b",    "PTD1",  "SPI0_SCK",  "UART2_CTS_b","FTM3_CH1",    "FB_CS0_b",  "I2S1_RXD0","-"                 },
        {  "-",            "PTD2",  "SPI0_SOUT", "UART2_RX",   "FTM3_CH2",    "FB_AD4",    "I2S1_RX_FS","-"                },
        {  "-",            "PTD3",  "SPI0_SIN",  "UART2_TX",   "FTM3_CH3",    "FB_AD3",    "I2S1_RX_BCLK","-"              },
        {  "-",            "PTD4",  "SPI0_PCS1", "UART0_RTS_b","FTM0_CH4",    "FB_AD2/NFC_DATA1","EWM_IN","-"              },
        {  "ADC0_SE6b",    "PTD5",  "SPI0_PCS2", "UART0_CTS_b/UART0_COL_b","FTM0_CH5","FB_AD1/NFC_DATA0","EWM_OUT_b","-"   },
        {  "ADC0_SE7b",    "PTD6",  "SPI0_PCS3", "UART0_RX",   "FTM0_CH6",    "FB_AD0",    "FTM0_FLT0","-"                 },
        {  "-",            "PTD7",  "CMT_IRO",   "UART0_TX",   "FTM0_CH7",    "-",         "FTM0_FLT1","-"                 },
        {  "-",            "PTD8",  "I2C0_SCL",  "UART5_RX",   "-",           "-",         "FB_A16/NFC_CLE","-"            },
        {  "-",            "PTD9",  "I2C0_SDA",  "UART5_TX",   "-",           "-",         "FB_A17/NFC_ALE","-"            },
        {  "-",            "PTD10", "-",         "UART5_RTS_b","-",           "-",         "FB_A18/NFC_RE","-"             },
        {  "-",            "PTD11", "SPI2_PCS0", "UART5_CTS_b","SDHC0_CLKIN", "-",         "FB_A19",   "GLCD_CONTRAST"     },
        {  "-",            "PTD12", "SPI2_SCK",  "FTM3_FLT0",  "SDHC0_D4",    "-",         "FB_A20",   "GLCD_PCLK"         },
        {  "-",            "PTD13", "SPI2_SOUT", "-",          "SDHC0_D5",    "-",         "FB_A21",   "GLCD_DE"           },
        {  "-",            "PTD14", "SPI2_SIN",  "-",          "SDHC0_D6",    "-",         "FB_A22",   "GLCD_HFS"          },
        {  "-",            "PTD15", "SPI2_PCS1", "-",          "SDHC0_D7",    "-",         "FB_A23",   "GLCD_VFS"          },
        {  "-",            "-",     "-",         "-",          "-",           "-",         "-",        "-"                 },
        {  "-",            "-",     "-",         "-",          "-",           "-",         "-",        "-"                 },
        {  "-",            "-",     "-",         "-",          "-",           "-",         "-",        "-"                 },
        {  "-",            "-",     "-",         "-",          "-",           "-",         "-",        "-"                 },
        {  "-",            "-",     "-",         "-",          "-",           "-",         "-",        "-"                 },
        {  "-",            "-",     "-",         "-",          "-",           "-",         "-",        "-"                 },
        {  "-",            "-",     "-",         "-",          "-",           "-",         "-",        "-"                 },
        {  "-",            "-",     "-",         "-",          "-",           "-",         "-",        "-"                 },
        {  "-",            "-",     "-",         "-",          "-",           "-",         "-",        "-"                 },
        {  "-",            "-",     "-",         "-",          "-",           "-",         "-",        "-"                 },
        {  "-",            "-",     "-",         "-",          "-",           "-",         "-",        "-"                 },
        {  "-",            "-",     "-",         "-",          "-",           "-",         "-",        "-"                 },
        {  "-",            "-",     "-",         "-",          "-",           "-",         "-",        "-"                 },
        {  "-",            "-",     "-",         "-",          "-",           "-",         "-",        "-"                 },
        {  "-",            "-",     "-",         "-",          "-",           "-",         "-",        "-"                 },
        {  "-",            "-",     "-",         "-",          "-",           "-",         "-",        "-"                 }
    },
    {
        // ALT 0           ALT 1    ALT2         ALT 3         ALT 4          ALT 5        ALT 6       ALT 7
        {  "ADC1_SE4a",    "PTE0",  "SPI1_PCS1", "UART1_TX",   "SDHC0_D1",    "GLCD_D0",   "I2C1_SDA", "RTC_CLOKUT"        }, // PORT E
        {  "ADC1_SE5a",    "PTE1",  "SPI1_SOUT", "UART1_RX",   "SDHC0_D0",    "GLCD_D1",   "I2C1_SCL", "SPI1_SIN"          },
        {  "ADC1_SE6a",    "PTE2",  "SPI1_SCK",  "UART1_CTS_b","SDHC0_DCLK",  "GLCD_D2",   "-",        "-"                 },
        {  "ADC1_SE7a",    "PTE3",  "SPI1_SIN",  "UART1_RTS_b","SDHC0_CMD",   "GLCD_D3",   "-",        "SPI1_SOUT"         },
        {  "-",            "PTE4",  "SPI1_PCS0", "UART3_TX",   "SDHC0_D3",    "GLCD_D4",   "-",        "-"                 },
        {  "-",            "PTE5",  "SPI1_PCS2", "UART3_RX",   "SDHC0_D2",    "GLCD_D5",   "FTM3_CH0", "-"                 },
        {  "-",            "PTE6",  "SPI1_PCS3", "UART3_CTS_b","I2S0_MCLK",   "GLCD_D6",   "FTM3_CH1", "USB_SOF_OUT"       },
        {  "-",            "PTE7",  "-",         "UART3_RTS_b","I2S0_RXD0",   "GLCD_D7",   "FTM3_CH2", "-"                 },
        {  "ADC2_SE16",    "PTE8",  "I2S0_RXD1", "UART5_TX",   "I2S0_RX_FS",  "GLCD_D8",   "FTM3_CH3", "-"                 },
        {  "ADC2_SE17",    "PTE9",  "I2S0_TXD1", "UART5_RX",   "I2S0_RX_BCLK","GLCD_D9",   "FTM3_CH4", "-"                 },
        {  "-",            "PTE10", "-",         "UART5_CTS_b","I2S0_TXD",    "GLCD_D10",  "FTM3_CH5", "-"                 },
        {  "ADC3_SE16",    "PTE11", "-",         "UART5_RTS_b","I2S0_TX_FS",  "GLCD_D11",  "FTM3_CH6", "-"                 },
        {  "ADC3_SE17",    "PTE12", "-",         "-",          "I2S0_TX_BCLK","GLCD_D12",  "FTM3_CH7", "-"                 },
        {  "-",            "-",     "-",         "-",          "-",           "-",         "-",        "-"                 },
        {  "-",            "-",     "-",         "-",          "-",           "-",         "-",        "-"                 },
        {  "-",            "-",     "-",         "-",          "-",           "-",         "-",        "-"                 },
        {  "ADC0_SE4a",    "PTE16", "SPI0_PCS0", "UART2_TX",   "FTM_CLKIN0",  "-",         "FTM0_FLT3","-"                 },
        {  "ADC0_SE5a",    "PTE17", "SPI0_SCK",  "UART2_RX",   "FTM_CLKIN1",  "-",         "LPTMR0_ALT3","-"               },
        {  "ADC0_SE6a",    "PTE18", "SPI0_SOUT", "UART2_CTS_b","I2C0_SDA",    "-",         "-",        "-"                 },
        {  "ADC0_SE7a",    "PTE19", "SPI0_SIN",  "UART2_RTS_b","I2C0_SCL",    "-",         "CMP3_OUT", "-"                 },
        {  "-",            "-",     "-",         "-",          "-",           "-",         "-",        "-"                 },
        {  "-",            "-",     "-",         "-",          "-",           "-",         "-",        "-"                 },
        {  "-",            "-",     "-",         "-",          "-",           "-",         "-",        "-"                 },
        {  "-",            "-",     "-",         "-",          "-",           "-",         "-",        "-"                 },
        {  "ADC0_SE17/EXTAL1","PTE24","CAN1_TX", "UART4_TX",   "I2S1_TX_FS",  "GLCD_D13",  "EWM_OUT_b","I2S1_RXD1"         },
        {  "ADC0_SE18/XTAL1","PTE25","CAN1_RX",  "UART4_RX",   "I2S1_TX_CLK", "GLCD_D14",  "EWM_IN",   "I2S1_TXD1"         },
        {  "ADC3_SE5b",    "PTE26", "ENET_1588_CLKIN","UART4_CTS_b","I2S1_TXD0","GLCD_D15","RTC_CLKOUT","USB_CLKIN"        },
        {  "ADC3_SE4b",    "PTE27", "-",         "UART4_RTS_b","I2S1_MCLK",   "GLCD_D16",  "-",        "-"                 },
        {  "ADC3_SE7a",    "PTE28", "-",         "-",          "-",           "GLCD_D17",  "-",        "-"                 },
        {  "-",            "-",     "-",         "-",          "-",           "-",         "-",        "-"                 },
        {  "-",            "-",     "-",         "-",          "-",           "-",         "-",        "-"                 },
        {  "-",            "-",     "-",         "-",          "-",           "-",         "-",        "-"                 }
    },
    {
        // ALT 0           ALT 1    ALT2         ALT 3         ALT 4          ALT 5        ALT 6       ALT 7
        {  "ADC2_SE11",    "PTF0",  "CAN0_TX",   "FTM3_CH0",   "-",           "I2S1_RXD1", "-",        "GLCD_PCLK"         }, // PORT F
        {  "ADC2_SE10",    "PTF1",  "CAN0_RX",   "FTM3_CH1",   "-",           "I2S1_RX_BCLK","-",      "GLCD_DE"           },
        {  "ADC2_SE6a",    "PTF2",  "I2C1_SCL",  "FTM3_CH2",   "-",           "I2S1_RX_FS", "-",       "GLCD_HFS"          },
        {  "ADC2_SE7a",    "PTF3",  "I2C1_SDA",  "FTM3_CH3",   "-",           "I2S1_RXD0",  "-",       "GLCD_VFS"          },
        {  "ADC2_SE4b",    "PTF4",  "-",         "FTM3_CH4",   "-",           "I2S1_TXD0",  "-",       "GLCD_D0"           },
        {  "ADC2_SE5b",    "PTF5",  "-",         "FTM3_CH5",   "-",           "I2S1_TX_FS", "-",       "GLCD_D1"           },
        {  "ADC2_SE6b",    "PTF6",  "-",         "FTM3_CH6",   "-",           "I2S1_TX_BCLK","-",      "GLCD_D2"           },
        {  "ADC2_SE7b",    "PTF7",  "-",         "FTM3_CH7",   "UART3_RX",    "I2S1_TXD1",  "-",       "GLCD_D3"           },
        {  "-",            "PTF8",  "-",         "FTM3_FLT0",  "UART3_TX",    "I2S1_MCLK",  "-",       "GLCD_D4"           },
        {  "CMP2_IN4",     "PTF9",  "-",         "-",          "UART3_RTS_b", "-",          "-",       "GLCD_D5"           },
        {  "CMP2_IN5",     "PTF10", "-",         "-",          "UART3_CTS_b", "-",          "-",       "GLCD_D6"           },
        {  "-",            "PTF11", "-",         "-",          "UART2_RTS_b", "-",          "-",       "GLCD_D7"           },
        {  "-",            "PTF12", "-",         "-",          "UART2_CTS_b", "-",          "-",       "GLCD_D8"           },
        {  "-",            "PTF13", "-",         "-",          "UART2_RX",    "-",          "-",       "GLCD_D9"           },
        {  "-",            "PTF14", "-",         "-",          "UART2_TX",    "-",          "-",       "GLCD_D10"          },
        {  "-",            "PTF15", "-",         "-",          "UART0_RTS_b", "-",          "-",       "GLCD_D11"          },
        {  "-",            "PTF16", "SPI2_PCS0", "FTM0_CH3",   "UART0_CTS_b/UART0_COL_b",   "GLCD_D12","-",   "-"          },
        {  "-",            "PTF17", "SPI2_SCK",  "FTM0_CH4",   "UART0_RX",    "GLCD_D13",  "-",        "-"                 },
        {  "-",            "PTF18", "SPI2_SOUT", "FTM1_CH0",   "UART0_TX",    "GLCD_D14",  "-",        "-"                 },
        {  "-",            "PTF19", "SPI2_SIN",  "FTM1_CH1",   "UART5_RX",    "GLCD_D15",  "-",        "-"                 },
        {  "-",            "PTF20", "SPI2_PCS1", "FTM2_CH0",   "UART5_TX",    "GLCD_D16",  "-",        "-"                 },
        {  "ADC3_SE6b",    "PTF21", "-",         "FTM2_CH1",   "UART5_RTS_b", "-",         "-",        "GLCD_D17"          },
        {  "ADC3_SE7b",    "PTF22", "I2C0_SCL",  "FTM1_CH0",   "UART5_CTS_b", "-",         "-",        "GLCD_D18"          },
        {  "ADC3_SE10",    "PTF23", "I2C0_SDA",  "FTM1_CH1",   "-",           "-",         "TRACE_CLKOUT","GLCD_D19"       },
        {  "ADC3_SE11",    "PTF24", "CAN1_RX",   "FTM1_QD_PHA","-",           "-",         "TRACE_D3", "GLCD_D20"          },
        {  "ADC3_SE12",    "PTF25", "CAN1_TX",   "FTM1_QD_PHB","-",           "-",         "TRACE_D2", "GLCD_D21"          },
        {  "ADC3_SE13",    "PTF26", "-",         "FTM2_QD_PHA","-",           "-",         "TRACE_D1", "GLCD_D22"          },
        {  "ADC3_SE14",    "PTF27", "-",         "FTM2_QD_PHB","-",           "-",         "TRACE_D0", "GLCD_D23"          },
        {  "-",            "-",     "-",         "-",          "-",           "-",         "-",        "-"                 },
        {  "-",            "-",     "-",         "-",          "-",           "-",         "-",        "-"                 },
        {  "-",            "-",     "-",         "-",          "-",           "-",         "-",        "-"                 },
        {  "-",            "-",     "-",         "-",          "-",           "-",         "-",        "-"                 }
    }
};
#else
static const char *cPer[PORTS_AVAILABLE][PORT_WIDTH][8] = {              // 256 pin MAPBGA
    {
        // ALT 0           ALT 1    ALT2         ALT 3         ALT 4          ALT 5        ALT 6       ALT 7
        {  "TSI0_CH1",     "PTA0",  "UART0_CTS_b/UART0_COL_b", "FTM0_CH5","-","-",         "-",        "JTAG_TCLK/SWD_CLK" }, // PORT A
        {  "TSI0_CH2",     "PTA1",  "UART0_RX",  "FTM0_CH6",   "-",           "-",         "-",        "JTAG_TDI"          },
        {  "TSI0_CH3",     "PTA2",  "UART0_TX",  "FTM0_CH7",   "-",           "-",         "-",        "JTAG_TDI/TRACE_SWO"},
        {  "TSI0_CH4",     "PTA3",  "UART0_RTS_b","FTM0_CH0",  "-",           "-",         "-",        "JTAG_TMS/SWD_DIO"  },
        {  "TSI0_CH5",     "PTA4",  "-",         "FTM0_CH1",   "-",           "-",         "-",        "NMI_b"             },
        {  "-",            "PTA5",  "USB_CLKIN", "FTM0_CH2",   "MII_RXER",    "CMP2_OUT",  "I2S0_TX_BCLK","JTAG_TRST"      },
        {  "ADC3_SE6a",    "PTA6",  "ULPI_CLK",  "FTM0_CH3",   "I2S1_RXD0",   "-",         "-",        "TRACE_CLKOUT"      },
        {  "ADC0_SE10",    "PTA7",  "ULPI_DIR",  "FTM0_CH4",   "I2S1_RX_BCLK","-",         "-",        "TRACE_D3"          },
        {  "ADC0_SE11",    "PTA8",  "ULPI_NXT",  "FTM1_CH0",   "I2S1_RX_FS",  "-",         "FTM1_QD_PHA","TRACE_D2"        },
        {  "ADC3_SE5a",    "PTA9",  "ULPI_STP",  "FTM1_CH1",   "MII_RXD3",    "-",         "FTM1_QD_PHB","TRACE_D1"        },
        {  "ADC3_SE4a",    "PTA10", "ULPI_DATA0","FTM2_CH0",   "MII_RXD2",    "-",         "FTM2_QD_PHA","TRACE_D0"        },
        {  "ADC3_SE15",    "PTA11", "ULPI_DATA1","FTM2_CH1",   "MII_RXCLK",   "-",         "FTM2_QD_PHB","-"               },
        {  "CMP2_IN0",     "PTA12", "CAN0_TX",   "FTM1_CH0",   "MII_RXD1",    "-",         "I2S0_TXD", "FTM1_QD_PHA"       },
        {  "CMP2_IN1",     "PTA13", "CAN0_RX",   "FTM1_CH1",   "MII_RXD0",    "-",         "I2S0_TX_FS","FTM1_QD_PHB"      },
        {  "CMP3_IN0",     "PTA14", "SPI0_PCS0", "UART0_TX",   "MII_RXDV",    "-",         "I2S0_RX_BCLK","I2S0_TXD1"      },
        {  "CMP3_IN1",     "PTA15", "SPI0_SCK",  "UART0_RX",   "MII_TXEN",    "-",         "I2S0_RXD0","-"                 },
        {  "CMP3_IN2",     "PTA16", "SPI0_SOUT", "UART0_CTS_b/UART0_COL_b","MII_TXD0", "-","I2S0_RX_FS","I2S0_RXD1"        },
        {  "ADC1_SE17",    "PTA17", "SPI0_SIN",  "UART0_RTS_b","MII_TXD1",    "-",         "I2S0_MCLK","-"                 },
        {  "EXTAL0",       "PTA18", "-",         "FTM0_FLT2",  "FTM_CLKIN0",  "-",         "-",        "-"                 },
        {  "XTAL0",        "PTA19", "-",         "FTM1_FLT0",  "FTM_CLKIN1",  "-",         "LPT0_ALT1","-"                 },
        {  "-",            "-",     "-",         "-",          "-",           "-",         "-",        "-"                 },
        {  "-",            "-",     "-",         "-",          "-",           "-",         "-",        "-"                 },
        {  "-",            "-",     "-",         "-",          "-",           "-",         "-",        "-"                 },
        {  "-",            "-",     "-",         "-",          "-",           "-",         "-",        "-"                 },
        {  "CMP3_IN4",     "PTA24", "ULPI_DATA2","-",          "MII_TXD2",    "-",         "FB_A29",   "-"                 },
        {  "CMP3_IN5",     "PTA25", "ULPI_DATA3","-",          "MII_TXCLK",   "-",         "FB_A28",   "-"                 },
        {  "ADC2_SE15",    "PTA26", "ULPI_DATA4","-",          "MII_TXD3",    "-",         "FB_A27",   "-"                 },                
        {  "ADC2_SE14",    "PTA27", "ULPI_DATA5","-",          "MII_CRS",     "-",         "FB_A26",   "-"                 },
        {  "ADC2_SE13",    "PTA28", "ULPI_DATA6","-",          "MII_TXER",    "-",         "FB_A25",   "-"                 },
        {  "ADC2_SE12",    "PTA29", "ULPI_DATA7","-",          "MII_COL",     "-",         "FB_A24",   "-"                 },
        {  "-",            "-",     "-",         "-",          "-",           "-",         "-",        "-"                 },
        {  "-",            "-",     "-",         "-",          "-",           "-",         "-",        "-"                 }
    },
    {
        // ALT 0           ALT 1    ALT2         ALT 3         ALT 4          ALT 5        ALT 6       ALT 7
        {  "ADC0_SE8/TSI0_CH0","PTB0","I2C0_SCL","FTM1_CH0",   "MII_MDIO",    "-",         "FTM1_QD_PHA", "-"              }, // PORT B
        {  "ADC0_SE9/TSI0_CH6","PTB1","I2C0_SDA","FTM1_CH1",   "MII_MCD",     "-",         "FTM1_QD_PHB", "-"              },
        {  "ADC0_SE12/TSI0_CH7","PTB2","I2C0_SCL","UART0_RTS_b","ENET_1588_TMR0","-",      "FTM0_FLT3","-"                 },
        {  "ADC0_SE13/TSI0_CH8","PTB3","I2C0_SDA","UART0_CTS_b/UART0_COL_b","ENET_1588_TMR1","-","FTM0_FLT0","-"           },
        {  "ADC1_SE10",    "PTB4",  "GLCD_CONTRAST","-",       "ENET_1588_TMR2","-",       "FTM1_FLT0","-"                 },
        {  "ADC1_SE11",    "PTB5",  "-",         "-",          "ENET_1588_TMR3","-",       "FTM2_FLT0","-"                 },
        {  "ADC1_SE12",    "PTB6",  "-",         "-",          "-",           "FB_AD23",   "-",        "-"                 },
        {  "ADC1_SE13",    "PTB7",  "-",         "-",          "-",           "FB_AD22",   "-",        "-"                 },
        {  "-",            "PTB8",  "-",         "UART3_RTS_b","-",           "FB_AD21",   "-",        "-"                 },
        {  "-",            "PTB9",  "SPI1_PCS1", "UART3_CTS_b","-",           "FB_AD20",   "-",        "-"                 },
        {  "ADC1_SE14",    "PTB10", "SPI1_PCS0", "UART3_RX",   "I2S1_TX_BCLK","FB_AD19",   "FTM0_FLT1","-"                 },
        {  "ADC1_SE15",    "PTB11", "SPI1_SCK",  "UART3_TX",   "I2S1_TX_FS",  "FB_AD18",   "FTM0_FLT2","-"                 },
        {  "-",            "-",     "-",         "-",          "-",           "-",         "-",        "-"                 },
        {  "-",            "-",     "-",         "-",          "-",           "-",         "-",        "-"                 },
        {  "-",            "-",     "-",         "-",          "-",           "-",         "-",        "-"                 },
        {  "-",            "-",     "-",         "-",          "-",           "-",         "-",        "-"                 },
        {  "TSI0_CH9",     "PTB16", "SPI1_SOUT", "UART0_RX",   "I2S1_TXD0",    "FB_AD17",   "EWM_IN",   "-"                },
        {  "TSI0_CH10",    "PTB17", "SPI1_SIN",  "UART0_TX",   "I2S1_TXD1",    "FB_AD16",   "EWM_OUT_b","-"                },
        {  "TSI0_CH11",    "PTB18", "CAN0_TX",   "FTM2_CH0",   "I2S0_TX_BCLK", "FB_AD15",   "FTM2_QD_PHA","-"              },
        {  "TSI0_CH12",    "PTB19", "CAN0_RX",   "FTM2_CH1",   "I2S0_TX_FS",   "FB_OE_b",   "FTM2_QD_PHB","-"              },
        {  "ADC2_SE4a",    "PTB20", "SPI2_PCS0", "-",          "-",            "FB_AD31/NFC_DATA15","CMP0_OUT","-"         },
        {  "ADC2_SE5a",    "PTB21", "SPI2_SCK",  "-",          "-",            "FB_AD30/NFC_DATA14","CMP1_OUT","-"         },
        {  "-",            "PTB22", "SPI2_SOUT", "-",          "-",            "FB_AD29/NFC_DATA13","CMP2_OUT","-"         },
        {  "-",            "PTB23", "SPI2_SIN",  "SPI0_PCS5",  "-",            "FB_AD28/NFC_DATA12","CMP3_OUT","-"         },
        {  "-",            "-",     "-",         "-",          "-",           "-",         "-",        "-"                 },
        {  "-",            "-",     "-",         "-",          "-",           "-",         "-",        "-"                 },
        {  "-",            "-",     "-",         "-",          "-",           "-",         "-",        "-"                 },
        {  "-",            "-",     "-",         "-",          "-",           "-",         "-",        "-"                 },
        {  "-",            "-",     "-",         "-",          "-",           "-",         "-",        "-"                 },
        {  "-",            "-",     "-",         "-",          "-",           "-",         "-",        "-"                 },
        {  "-",            "-",     "-",         "-",          "-",           "-",         "-",        "-"                 },
        {  "-",            "-",     "-",         "-",          "-",           "-",         "-",        "-"                 }
    },
    {
        // ALT 0           ALT 1    ALT2         ALT 3         ALT 4          ALT 5        ALT 6       ALT 7
        {  "ADC0_SE14/TSI0_CH13","PTC0","SPI0_PCS4","PDB0_EXTRG","-",         "FB_AD14/NFC_DATA11","I2S0_TXD1","-"         }, // PORT C
        {  "ADC0_SE15/TSI0_CH14","PTC1","SPI0_PCS3","UART1_RTS_b","FTM0_CH0", "FB_AD13/NFC_DATA10","I2S0_TXD0","-"         },
        {  "ADC0_SE4b/CMP1_IN0/TSI0_CH15","PTC2","SPI0_PCS2","UART1_CTS_b","FTM0_CH1","FB_AD12/NFC_DATA9","I2S0_TX_FS","-" },
        {  "CMP1_IN1",     "PTC3",  "SPI0_PCS1", "UART1_RX",   "FTM0_CH2",    "-",         "I2S0_TX_BCLK","-"              },
        {  "-",            "PTC4",  "SPI0_PCS0", "UART1_TX",   "FTM0_CH3",    "FB_AD11/NFC_DATA8","CMP1_OUT", "I2S1_TX_BCLK" },
        {  "-",            "PTC5",  "SPI0_SCK",  "LPTMR0_ALT2","I2S0_RXD0",   "FB_AD10/NFC_DATA7","CMP0_OUT", "I2S1_TX_FS" },
        {  "CMP0_IN0",     "PTC6",  "SPI0_SOUT", "PDB0_EXTRG", "I2S0_RX_BCLK","FB_AD9/NFC_DATA6","I2S0_MCLK", "-"          },
        {  "CMP0_IN1",     "PTC7",  "SPI0_SIN",  "USB_SOF_OUT","I2S0_RX_FS",  "FB_AD8/NFC_DATA5","-",  "-"                 },
        {  "ADC1_SE4b/CMP0_IN2","PTC8","-",      "FTM3_CH4",   "I2S0_MCLK",   "FB_AD7/NFC_DATA4","-",  "-"                 },
        {  "ADC1_SE5b/CMP0_IN3","PTC9","-",      "FTM3_CH5",   "I2S0_RX_BCLK","FB_AD6/NFC_DATA3","FTM2_FLT0","-"           },
        {  "ADC1_SE6b",    "PTC10", "I2C1_SCL",  "FTM3_CH6",   "I2S0_RX_FS",  "FB_AD5/NFC_DATA2","I2S1_MCLK","-"           },
        {  "ADC1_SE7b",    "PTC11", "I2C1_SDA",  "FTM3_CH7",   "I2S0_RXD1",   "FB_RW_b/NFC_WE","-",     "-"                },
        {  "-",            "PTC12", "-",         "UART4_RTS_b","-",           "FB_AD27",   "FTM3_FLT0", "-"                },
        {  "-",            "PTC13", "-",         "UART4_CTS_b","-",           "FB_AD26",   "-",        "-"                 },
        {  "-",            "PTC14", "-",         "UART4_RX",   "-",           "FB_AD25",   "-",        "-"                 },
        {  "-",            "PTC15", "-",         "UART4_TX",   "-",           "FB_AD24",   "-",        "-"                 },
        {  "-",            "PTC16", "CAN1_RX",   "UART3_RX",   "ENET_1588_TMR0","FB_CS5_b/FB_TSIZ1/FB_BE23_16_BLS15_8_b","NFC_RB","-"},
        {  "-",            "PTC17", "CAN1_TX",   "UART3_TX",   "ENET_1588_TMR1","FB_CS4_b/FB_TSIZ0/FB_BE31_24_BLS7_0_b","NFC_CE0_b","-"},
        {  "-",            "PTC18", "-",         "UART3_RTS_b","ENET_1588_TMR2","FB_TBST_b/FB_CS2_b/FB_BE15_8_BLS23_16_b","NFC_CE1_b","-"},
        {  "-",            "PTC19", "-",         "UART3_CTS_b","ENET_1588_TMR3","FB_CS3_b/FB_BE7_0_BLS31_24_b","FB_TA_b","-"     },
        {  "-",            "-",     "-",         "-",          "-",           "-",         "-",        "-"                 },
        {  "-",            "-",     "-",         "-",          "-",           "-",         "-",        "-"                 },
        {  "-",            "-",     "-",         "-",          "-",           "-",         "-",        "-"                 },
        {  "-",            "-",     "-",         "-",          "-",           "-",         "-",        "-"                 },
        {  "-",            "-",     "-",         "-",          "-",           "-",         "-",        "-"                 },
        {  "-",            "-",     "-",         "-",          "-",           "-",         "-",        "-"                 },
        {  "-",            "-",     "-",         "-",          "-",           "-",         "-",        "-"                 },
        {  "-",            "-",     "-",         "-",          "-",           "-",         "-",        "-"                 },
        {  "-",            "-",     "-",         "-",          "-",           "-",         "-",        "-"                 },
        {  "-",            "-",     "-",         "-",          "-",           "-",         "-",        "-"                 },
        {  "-",            "-",     "-",         "-",          "-",           "-",         "-",        "-"                 },
        {  "-",            "-",     "-",         "-",          "-",           "-",         "-",        "-"                 }
    },
    {
        // ALT 0           ALT 1    ALT2         ALT 3         ALT 4          ALT 5        ALT 6       ALT 7
        {  "-",            "PTD0",  "SPI0_PCS0", "UART2_RTS_b","FTM3_CH0",    "FB_ALE/FB_CS1_b/FB_TS_b","I2S1_RXD1","-"    }, // PORT D
        {  "ADC0_SE5b",    "PTD1",  "SPI0_SCK",  "UART2_CTS_b","FTM3_CH1",    "FB_CS0_b",  "I2S1_RXD0","-"                 },
        {  "-",            "PTD2",  "SPI0_SOUT", "UART2_RX",   "FTM3_CH2",    "FB_AD4",    "I2S1_RX_FS","-"                },
        {  "-",            "PTD3",  "SPI0_SIN",  "UART2_TX",   "FTM3_CH3",    "FB_AD3",    "I2S1_RX_BCLK","-"              },
        {  "-",            "PTD4",  "SPI0_PCS1", "UART0_RTS_b","FTM0_CH4",    "FB_AD2/NFC_DATA1","EWM_IN","-"              },
        {  "ADC0_SE6b",    "PTD5",  "SPI0_PCS2", "UART0_CTS_b/UART0_COL_b","FTM0_CH5","FB_AD1/NFC_DATA0","EWM_OUT_b","-"   },
        {  "ADC0_SE7b",    "PTD6",  "SPI0_PCS3", "UART0_RX",   "FTM0_CH6",    "FB_AD0",    "FTM0_FLT0","-"                 },
        {  "-",            "PTD7",  "CMT_IRO",   "UART0_TX",   "FTM0_CH7",    "-",         "FTM0_FLT1","-"                 },
        {  "-",            "PTD8",  "I2C0_SCL",  "UART5_RX",   "-",           "-",         "FB_A16/NFC_CLE","-"            },
        {  "-",            "PTD9",  "I2C0_SDA",  "UART5_TX",   "-",           "-",         "FB_A17/NFC_ALE","-"            },
        {  "-",            "PTD10", "-",         "UART5_RTS_b","-",           "-",         "FB_A18/NFC_RE","-"             },
        {  "-",            "PTD11", "SPI2_PCS0", "UART5_CTS_b","SDHC0_CLKIN", "-",         "FB_A19",   "GLCD_CONTRAST"     },
        {  "-",            "PTD12", "SPI2_SCK",  "FTM3_FLT0",  "SDHC0_D4",    "-",         "FB_A20",   "GLCD_PCLK"         },
        {  "-",            "PTD13", "SPI2_SOUT", "-",          "SDHC0_D5",    "-",         "FB_A21",   "GLCD_DE"           },
        {  "-",            "PTD14", "SPI2_SIN",  "-",          "SDHC0_D6",    "-",         "FB_A22",   "GLCD_HFS"          },
        {  "-",            "PTD15", "SPI2_PCS1", "-",          "SDHC0_D7",    "-",         "FB_A23",   "GLCD_VFS"          },
        {  "-",            "-",     "-",         "-",          "-",           "-",         "-",        "-"                 },
        {  "-",            "-",     "-",         "-",          "-",           "-",         "-",        "-"                 },
        {  "-",            "-",     "-",         "-",          "-",           "-",         "-",        "-"                 },
        {  "-",            "-",     "-",         "-",          "-",           "-",         "-",        "-"                 },
        {  "-",            "-",     "-",         "-",          "-",           "-",         "-",        "-"                 },
        {  "-",            "-",     "-",         "-",          "-",           "-",         "-",        "-"                 },
        {  "-",            "-",     "-",         "-",          "-",           "-",         "-",        "-"                 },
        {  "-",            "-",     "-",         "-",          "-",           "-",         "-",        "-"                 },
        {  "-",            "-",     "-",         "-",          "-",           "-",         "-",        "-"                 },
        {  "-",            "-",     "-",         "-",          "-",           "-",         "-",        "-"                 },
        {  "-",            "-",     "-",         "-",          "-",           "-",         "-",        "-"                 },
        {  "-",            "-",     "-",         "-",          "-",           "-",         "-",        "-"                 },
        {  "-",            "-",     "-",         "-",          "-",           "-",         "-",        "-"                 },
        {  "-",            "-",     "-",         "-",          "-",           "-",         "-",        "-"                 },
        {  "-",            "-",     "-",         "-",          "-",           "-",         "-",        "-"                 },
        {  "-",            "-",     "-",         "-",          "-",           "-",         "-",        "-"                 }
    },
    {
        // ALT 0           ALT 1    ALT2         ALT 3         ALT 4          ALT 5        ALT 6       ALT 7
        {  "ADC1_SE4a",    "PTE0",  "SPI1_PCS1", "UART1_TX",   "SDHC0_D1",    "GLCD_D0",   "I2C1_SDA", "RTC_CLOKUT"        }, // PORT E
        {  "ADC1_SE5a",    "PTE1",  "SPI1_SOUT", "UART1_RX",   "SDHC0_D0",    "GLCD_D1",   "I2C1_SCL", "SPI1_SIN"          },
        {  "ADC1_SE6a",    "PTE2",  "SPI1_SCK",  "UART1_CTS_b","SDHC0_DCLK",  "GLCD_D2",   "-",        "-"                 },
        {  "ADC1_SE7a",    "PTE3",  "SPI1_SIN",  "UART1_RTS_b","SDHC0_CMD",   "GLCD_D3",   "-",        "SPI1_SOUT"         },
        {  "-",            "PTE4",  "SPI1_PCS0", "UART3_TX",   "SDHC0_D3",    "GLCD_D4",   "-",        "-"                 },
        {  "-",            "PTE5",  "SPI1_PCS2", "UART3_RX",   "SDHC0_D2",    "GLCD_D5",   "FTM3_CH0", "-"                 },
        {  "-",            "PTE6",  "SPI1_PCS3", "UART3_CTS_b","I2S0_MCLK",   "GLCD_D6",   "FTM3_CH1", "USB_SOF_OUT"       },
        {  "-",            "PTE7",  "-",         "UART3_RTS_b","I2S0_RXD0",   "GLCD_D7",   "FTM3_CH2", "-"                 },
        {  "ADC2_SE16",    "PTE8",  "I2S0_RXD1", "UART5_TX",   "I2S0_RX_FS",  "GLCD_D8",   "FTM3_CH3", "-"                 },
        {  "ADC2_SE17",    "PTE9",  "I2S0_TXD1", "UART5_RX",   "I2S0_RX_BCLK","GLCD_D9",   "FTM3_CH4", "-"                 },
        {  "-",            "PTE10", "-",         "UART5_CTS_b","I2S0_TXD",    "GLCD_D10",  "FTM3_CH5", "-"                 },
        {  "ADC3_SE16",    "PTE11", "-",         "UART5_RTS_b","I2S0_TX_FS",  "GLCD_D11",  "FTM3_CH6", "-"                 },
        {  "ADC3_SE17",    "PTE12", "-",         "-",          "I2S0_TX_BCLK","GLCD_D12",  "FTM3_CH7", "-"                 },
        {  "-",            "-",     "-",         "-",          "-",           "-",         "-",        "-"                 },
        {  "-",            "-",     "-",         "-",          "-",           "-",         "-",        "-"                 },
        {  "-",            "-",     "-",         "-",          "-",           "-",         "-",        "-"                 },
        {  "ADC0_SE4a",    "PTE16", "SPI0_PCS0", "UART2_TX",   "FTM_CLKIN0",  "-",         "FTM0_FLT3","-"                 },
        {  "ADC0_SE5a",    "PTE17", "SPI0_SCK",  "UART2_RX",   "FTM_CLKIN1",  "-",         "LPTMR0_ALT3","-"               },
        {  "ADC0_SE6a",    "PTE18", "SPI0_SOUT", "UART2_CTS_b","I2C0_SDA",    "-",         "-",        "-"                 },
        {  "ADC0_SE7a",    "PTE19", "SPI0_SIN",  "UART2_RTS_b","I2C0_SCL",    "-",         "CMP3_OUT", "-"                 },
        {  "-",            "-",     "-",         "-",          "-",           "-",         "-",        "-"                 },
        {  "-",            "-",     "-",         "-",          "-",           "-",         "-",        "-"                 },
        {  "-",            "-",     "-",         "-",          "-",           "-",         "-",        "-"                 },
        {  "-",            "-",     "-",         "-",          "-",           "-",         "-",        "-"                 },
        {  "ADC0_SE17/EXTAL1","PTE24","CAN1_TX", "UART4_TX",   "I2S1_TX_FS",  "GLCD_D13",  "EWM_OUT_b","I2S1_RXD1"         },
        {  "ADC0_SE18/XTAL1","PTE25","CAN1_RX",  "UART4_RX",   "I2S1_TX_CLK", "GLCD_D14",  "EWM_IN",   "I2S1_TXD1"         },
        {  "ADC3_SE5b",    "PTE26", "ENET_1588_CLKIN","UART4_CTS_b","I2S1_TXD0","GLCD_D15","RTC_CLKOUT","USB_CLKIN"        },
        {  "ADC3_SE4b",    "PTE27", "-",         "UART4_RTS_b","I2S1_MCLK",   "GLCD_D16",  "-",        "-"                 },
        {  "ADC3_SE7a",    "PTE28", "-",         "-",          "-",           "GLCD_D17",  "-",        "-"                 },
        {  "-",            "-",     "-",         "-",          "-",           "-",         "-",        "-"                 },
        {  "-",            "-",     "-",         "-",          "-",           "-",         "-",        "-"                 },
        {  "-",            "-",     "-",         "-",          "-",           "-",         "-",        "-"                 }
    },
    {
        // ALT 0           ALT 1    ALT2         ALT 3         ALT 4          ALT 5        ALT 6       ALT 7
        {  "ADC2_SE11",    "PTF0",  "CAN0_TX",   "FTM3_CH0",   "-",           "I2S1_RXD1", "-",        "GLCD_PCLK"         }, // PORT F
        {  "ADC2_SE10",    "PTF1",  "CAN0_RX",   "FTM3_CH1",   "-",           "I2S1_RX_BCLK","-",      "GLCD_DE"           },
        {  "ADC2_SE6a",    "PTF2",  "I2C1_SCL",  "FTM3_CH2",   "-",           "I2S1_RX_FS", "-",       "GLCD_HFS"          },
        {  "ADC2_SE7a",    "PTF3",  "I2C1_SDA",  "FTM3_CH3",   "-",           "I2S1_RXD0",  "-",       "GLCD_VFS"          },
        {  "ADC2_SE4b",    "PTF4",  "-",         "FTM3_CH4",   "-",           "I2S1_TXD0",  "-",       "GLCD_D0"           },
        {  "ADC2_SE5b",    "PTF5",  "-",         "FTM3_CH5",   "-",           "I2S1_TX_FS", "-",       "GLCD_D1"           },
        {  "ADC2_SE6b",    "PTF6",  "-",         "FTM3_CH6",   "-",           "I2S1_TX_BCLK","-",      "GLCD_D2"           },
        {  "ADC2_SE7b",    "PTF7",  "-",         "FTM3_CH7",   "UART3_RX",    "I2S1_TXD1",  "-",       "GLCD_D3"           },
        {  "-",            "PTF8",  "-",         "FTM3_FLT0",  "UART3_TX",    "I2S1_MCLK",  "-",       "GLCD_D4"           },
        {  "CMP2_IN4",     "PTF9",  "-",         "-",          "UART3_RTS_b", "-",          "-",       "GLCD_D5"           },
        {  "CMP2_IN5",     "PTF10", "-",         "-",          "UART3_CTS_b", "-",          "-",       "GLCD_D6"           },
        {  "-",            "PTF11", "-",         "-",          "UART2_RTS_b", "-",          "-",       "GLCD_D7"           },
        {  "-",            "PTF12", "-",         "-",          "UART2_CTS_b", "-",          "-",       "GLCD_D8"           },
        {  "-",            "PTF13", "-",         "-",          "UART2_RX",    "-",          "-",       "GLCD_D9"           },
        {  "-",            "PTF14", "-",         "-",          "UART2_TX",    "-",          "-",       "GLCD_D10"          },
        {  "-",            "PTF15", "-",         "-",          "UART0_RTS_b", "-",          "-",       "GLCD_D11"          },
        {  "-",            "PTF16", "SPI2_PCS0", "FTM0_CH3",   "UART0_CTS_b/UART0_COL_b",   "GLCD_D12","-",   "-"          },
        {  "-",            "PTF17", "SPI2_SCK",  "FTM0_CH4",   "UART0_RX",    "GLCD_D13",  "-",        "-"                 },
        {  "-",            "PTF18", "SPI2_SOUT", "FTM1_CH0",   "UART0_TX",    "GLCD_D14",  "-",        "-"                 },
        {  "-",            "PTF19", "SPI2_SIN",  "FTM1_CH1",   "UART5_RX",    "GLCD_D15",  "-",        "-"                 },
        {  "-",            "PTF20", "SPI2_PCS1", "FTM2_CH0",   "UART5_TX",    "GLCD_D16",  "-",        "-"                 },
        {  "ADC3_SE6b",    "PTF21", "-",         "FTM2_CH1",   "UART5_RTS_b", "-",         "-",        "GLCD_D17"          },
        {  "ADC3_SE7b",    "PTF22", "I2C0_SCL",  "FTM1_CH0",   "UART5_CTS_b", "-",         "-",        "GLCD_D18"          },
        {  "ADC3_SE10",    "PTF23", "I2C0_SDA",  "FTM1_CH1",   "-",           "-",         "TRACE_CLKOUT","GLCD_D19"       },
        {  "ADC3_SE11",    "PTF24", "CAN1_RX",   "FTM1_QD_PHA","-",           "-",         "TRACE_D3", "GLCD_D20"          },
        {  "ADC3_SE12",    "PTF25", "CAN1_TX",   "FTM1_QD_PHB","-",           "-",         "TRACE_D2", "GLCD_D21"          },
        {  "ADC3_SE13",    "PTF26", "-",         "FTM2_QD_PHA","-",           "-",         "TRACE_D1", "GLCD_D22"          },
        {  "ADC3_SE14",    "PTF27", "-",         "FTM2_QD_PHB","-",           "-",         "TRACE_D0", "GLCD_D23"          },
        {  "-",            "-",     "-",         "-",          "-",           "-",         "-",        "-"                 },
        {  "-",            "-",     "-",         "-",          "-",           "-",         "-",        "-"                 },
        {  "-",            "-",     "-",         "-",          "-",           "-",         "-",        "-"                 },
        {  "-",            "-",     "-",         "-",          "-",           "-",         "-",        "-"                 }
    }
};
#endif
