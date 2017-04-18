/***********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher

    ---------------------------------------------------------------------
    File:      kinetis_port_K26_K65_K66.h
    Project:   Single Chip Embedded Internet
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2017
    *********************************************************************

*/

#if defined KINETIS_K26
    #if PIN_COUNT == PIN_COUNT_169_PIN
        #if PACKAGE_TYPE == PACKAGE_WLCSP
            #define _PIN_COUNT          0                                // 169 WLCSP
        #else
            #define _PIN_COUNT          1                                // 169 MAPBGA
        #endif
    #else
        #if PACKAGE_TYPE == PACKAGE_LQFP
            #define _PIN_COUNT          2                                // 144 LQFP
        #else
            #define _PIN_COUNT          3                                // 144 MAPBGA
    
        #endif
    #endif
#elif defined KINETIS_K65
    #if PACKAGE_TYPE == PACKAGE_WLCSP
        #define _PIN_COUNT              0                                // 169 WLCSP
    #else
        #define _PIN_COUNT              1                                // 169 MAPBGA
    #endif
#else                                                                    // K66
    #if PACKAGE_TYPE == PACKAGE_LQFP
        #define _PIN_COUNT              2                                // 144 LQFP
    #else
        #define _PIN_COUNT              3                                // 144 MAPBGA
    #endif
#endif

#define ALTERNATIVE_FUNCTIONS   7                                        // GPIO plus 5 possible peripheral/extra/system functions

static const char *cPinNumber[PORTS_AVAILABLE + 1][PORT_WIDTH][4] = {
    {
        // 169WLCSP    169MAPBGA  144LQFP     144MAPBGA                  GPIO A
        {  "N7",       "N8",      "50",       "J5",   },                 // PA0
        {  "M7",       "N9",      "51",       "J6",   },                 // PA1
        {  "L7",       "M9",      "52",       "K6",   },                 // PA2
        {  "J6",       "M8",      "53",       "K7",   },                 // PA3
        {  "K6",       "L9",      "54",       "L7",   },                 // PA4
        {  "N6",       "N10",     "55",       "M8",   },                 // PA5
        {  "N5",       "M10",     "58",       "J7",   },                 // PA6
        {  "L6",       "L10",     "59",       "J8",   },                 // PA7
        {  "M5",       "K9",      "60",       "K8",   },                 // PA8
        {  "J5",       "K10",     "61",       "L8",   },                 // PA9
        {  "K5",       "N11",     "62",       "M9",   },                 // PA10
        {  "N4",       "M11",     "63",       "L9",   },                 // PA11
        {  "M4",       "L12",     "64",       "K9",   },                 // PA12
        {  "L5",       "L11",     "65",       "J9",   },                 // PA13
        {  "N3",       "K13",     "66",       "L10",  },                 // PA14
        {  "L4",       "K12",     "67",       "L11",  },                 // PA15
        {  "K4",       "J13",     "68",       "K10",  },                 // PA16
        {  "L3",       "J12",     "69",       "K11",  },                 // PA17
        {  "N1",       "N13",     "72",       "M12",  },                 // PA18
        {  "N2",       "M13",     "73",       "M11",  },                 // PA19
        {  "-",        "-",       "-",        "-",    },                 // PA20
        {  "-",        "-",       "-",        "-",    },                 // PA21
        {  "-",        "-",       "-",        "-",    },                 // PA22
        {  "-",        "-",       "-",        "-",    },                 // PA23
        {  "K3",       "K11",     "75",       "K12",  },                 // PA24
        {  "J4",       "J11",     "76",       "J12",  },                 // PA25
        {  "J3",       "J10",     "77",       "J11",  },                 // PA26
        {  "L2",       "H13",     "78",       "J10",  },                 // PA27
        {  "L1",       "H12",     "79",       "H12",  },                 // PA28
        {  "K2",       "H11",     "80",       "H11",  },                 // PA29
        {  "K1",       "H10",     "-",        "-",    },                 // PA30
        {  "H5",       "J9",      "-",        "-",    },                 // PA31
    },
    {
        // 169WLCSP    169MAPBGA  144LQFP     144MAPBGA                  GPIO B
        {  "H4",       "G13",     "81",       "H10",  },                 // PB0
        {  "J2",       "G12",     "82",       "H9",   },                 // PB1
        {  "J1",       "G11",     "83",       "G12",  },                 // PB2
        {  "H3",       "G10",     "84",       "G11",  },                 // PB3
        {  "H2",       "H9",      "85",       "G10",  },                 // PB4
        {  "H1",       "H13",     "86",       "G9",   },                 // PB5
        {  "G5",       "F12",     "87",       "F12",  },                 // PB6
        {  "G4",       "F11",     "88",       "F11",  },                 // PB7
        {  "G3",       "F10",     "89",       "F10",  },                 // PB8
        {  "G2",       "F9",      "90",       "F9",   },                 // PB9
        {  "G1",       "G9",      "91",       "E12",  },                 // PB10
        {  "F5",       "E13",     "92",       "E11",  },                 // PB11
        {  "F4",       "E12",     "-",        "-",    },                 // PB12
        {  "F3",       "E11",     "-",        "-",    },                 // PB13
        {  "F2",       "E10",     "-",        "-",    },                 // PB14
        {  "F1",       "E9",      "-",        "-",    },                 // PB15
        {  "E1",       "F8",      "95",       "E10",  },                 // PB16
        {  "E2",       "D13",     "96",       "E9",   },                 // PB17
        {  "E3",       "D12",     "97",       "D12",  },                 // PB18
        {  "E4",       "D11",     "98",       "D11",  },                 // PB19
        {  "E5",       "D10",     "99",       "D10",  },                 // PB20
        {  "D1",       "D9",      "100",      "D9",   },                 // PB21
        {  "D2",       "C13",     "101",      "C12",  },                 // PB22
        {  "D3",       "C12",     "102",      "C11",  },                 // PB23
        {  "-",        "-",       "-",        "-",    },                 // PB24
        {  "-",        "-",       "-",        "-",    },                 // PB25
        {  "-",        "-",       "-",        "-",    },                 // PB26
        {  "-",        "-",       "-",        "-",    },                 // PB27
        {  "-",        "-",       "-",        "-",    },                 // PB28
        {  "-",        "-",       "-",        "-",    },                 // PB29
        {  "-",        "-",       "-",        "-",    },                 // PB30
        {  "-",        "-",       "-",        "-",    },                 // PB31
    },
    {
        // 169WLCSP    169MAPBGA  144LQFP     144MAPBGA                  GPIO C
        {  "C1",       "B13",     "103",      "B12",  },                 // PC0
        {  "C2",       "B12",     "104",      "B11",  },                 // PC1
        {  "D4",       "A13",     "105",      "A12",  },                 // PC2
        {  "B1",       "A12",     "106",      "A11",  },                 // PC3
        {  "A1",       "B11",     "109",      "A9",   },                 // PC4
        {  "B2",       "A11",     "110",      "D8",   },                 // PC5
        {  "C3",       "A10",     "111",      "C8",   },                 // PC6
        {  "D5",       "B10",     "112",      "B8",   },                 // PC7
        {  "C4",       "C10",     "113",      "A8",   },                 // PC8
        {  "A2",       "C9",      "114",      "D7",   },                 // PC9
        {  "B3",       "A8",      "115",      "C7",   },                 // PC10
        {  "D6",       "A9",      "116",      "B7",   },                 // PC11
        {  "C5",       "B9",      "117",      "A7",   },                 // PC12
        {  "A3",       "B8",      "118",      "D6",   },                 // PC13
        {  "B4",       "C8",      "119",      "D6",   },                 // PC14
        {  "A4",       "D8",      "120",      "B6",   },                 // PC15
        {  "A5",       "E8",      "123",      "A6",   },                 // PC16
        {  "B5",       "E7",      "124",      "D5",   },                 // PC17
        {  "C6",       "D7",      "125",      "C5",   },                 // PC18
        {  "B6",       "C7",      "126",      "B5",   },                 // PC19
        {  "-",        "-",       "-",        "-",    },                 // PC20
        {  "-",        "-",       "-",        "-",    },                 // PC21
        {  "-",        "-",       "-",        "-",    },                 // PC22
        {  "-",        "-",       "-",        "-",    },                 // PC23
        {  "A6",       "B7",      "-",        "-",    },                 // PC24
        {  "D7",       "A7",      "-",        "-",    },                 // PC25
        {  "E8",       "E6",      "-",        "-",    },                 // PC26
        {  "A7",       "D6",      "-",        "-",    },                 // PC27
        {  "B7",       "C6",      "-",        "-",    },                 // PC28
        {  "C7",       "B6",      "-",        "-",    },                 // PC29
        {  "-",        "-",       "-",        "-",    },                 // PC30
        {  "-",        "-",       "-",        "-",    },                 // PC31
    },
    {
        // 169WLCSP    169MAPBGA  144LQFP     144MAPBGA                  GPIO D
        {  "D8",       "A6",      "127",      "A5",   },                 // PD0
        {  "A8",       "A5",      "128",      "D4",   },                 // PD1
        {  "B8",       "A4",      "129",      "C4",   },                 // PD2
        {  "C8",       "B4",      "130",      "B4",   },                 // PD3
        {  "F8",       "B5",      "131",      "A4",   },                 // PD4
        {  "A9",       "C4",      "132",      "A3",   },                 // PD5
        {  "B9",       "C5",      "133",      "A2",   },                 // PD6
        {  "A10",      "E5",      "136",      "A1",   },                 // PD7
        {  "C9",       "D5",      "137",      "C9",   },                 // PD8
        {  "B10",      "D4",      "138",      "B9",   },                 // PD9
        {  "A11",      "D3",      "139",      "B3",   },                 // PD10
        {  "D9",       "C2",      "140",      "B2",   },                 // PD11
        {  "C10",      "B2",      "141",      "B1",   },                 // PD12
        {  "A12",      "B3",      "142",      "C3",   },                 // PD13
        {  "B11",      "A2",      "143",      "C2",   },                 // PD14
        {  "D10",      "A3",      "144",      "C1",   },                 // PD15
        {  "-",        "-",       "-",        "-",    },                 // PD16
        {  "-",        "-",       "-",        "-",    },                 // PD17
        {  "-",        "-",       "-",        "-",    },                 // PD18
        {  "-",        "-",       "-",        "-",    },                 // PD19
        {  "-",        "-",       "-",        "-",    },                 // PD20
        {  "-",        "-",       "-",        "-",    },                 // PD21
        {  "-",        "-",       "-",        "-",    },                 // PD22
        {  "-",        "-",       "-",        "-",    },                 // PD23
        {  "-",        "-",       "-",        "-",    },                 // PD24
        {  "-",        "-",       "-",        "-",    },                 // PD25
        {  "-",        "-",       "-",        "-",    },                 // PD26
        {  "-",        "-",       "-",        "-",    },                 // PD27
        {  "-",        "-",       "-",        "-",    },                 // PD28
        {  "-",        "-",       "-",        "-",    },                 // PD29
        {  "-",        "-",       "-",        "-",    },                 // PD30
        {  "-",        "-",       "-",        "-",    },                 // PD31
    },
    {
        // 169WLCSP    169MAPBGA  144LQFP     144MAPBGA                  GPIO E
        {  "C11",      "A1",      "1",        "D3",   },                 // PE0
        {  "A13",      "B1",      "2",        "D2",   },                 // PE1
        {  "B12",      "C1",      "3",        "D1",   },                 // PE2
        {  "B13",      "D1",      "4",        "E4",   },                 // PE3
        {  "C12",      "E1",      "7",        "E3",   },                 // PE4
        {  "D11",      "D2",      "8",        "E2",   },                 // PE5
        {  "C13",      "E2",      "9",        "E1",   },                 // PE6
        {  "E10",      "E3",      "10",       "F4",   },                 // PE7
        {  "D12",      "E4",      "11",       "F3",   },                 // PE8
        {  "D13",      "F3",      "12",       "F2",   },                 // PE9
        {  "D10",      "F4",      "13",       "F1",   },                 // PE10
        {  "E11",      "G4",      "14",       "G4",   },                 // PE11
        {  "E12",      "H4",      "15",       "G3",   },                 // PE12
        {  "-",        "-",       "-",        "-",    },                 // PE13
        {  "-",        "-",       "-",        "-",    },                 // PE14
        {  "-",        "-",       "-",        "-",    },                 // PE15
        {  "G9",       "H3",      "G9",       "H3",   },                 // PE16
        {  "F11",      "F5",      "F11",      "F5",   },                 // PE17
        {  "G10",      "F6",      "G10",      "F6",   },                 // PE18
        {  "F12",      "F7",      "F12",      "F7",   },                 // PE19
        {  "-",        "-",       "-",        "-",    },                 // PE20
        {  "-",        "-",       "-",        "-",    },                 // PE21
        {  "-",        "-",       "-",        "-",    },                 // PE22
        {  "-",        "-",       "-",        "-",    },                 // PE23
        {  "N8",       "L7",      "45",       "M4",   },                 // PE24
        {  "M8",       "K7",      "46",       "K5",   },                 // PE25
        {  "L8",       "K8",      "47",       "K4",   },                 // PE26
        {  "J7",       "L8",      "48",       "J4",   },                 // PE27
        {  "K7",       "M7",      "49",       "H4",   },                 // PE28
        {  "-",        "-",       "-",        "-",    },                 // PE29
        {  "-",        "-",       "-",        "-",    },                 // PE30
        {  "-",        "-",       "-",        "-",    },                 // PE31
    },                  
    {
        // 169WLCSP    169MAPBGA  144LQFP     144MAPBGA                  dedicated ADC pins
        {  "G13",      "G1",      "19",       "H1",   },                 // USB0_DP
        {  "H13",      "F1",      "20",       "H2",   },                 // USB0_DM
        {  "G11",      "G2",      "21",       "G1",   },                 // VREG_OUT
        {  "G12",      "F2",      "22",       "G2",   },                 // VREG_IN0
        {  "H12",      "H2",      "23",       "J2",   },                 // VREG_IN1
        {  "J13",      "J1",      "25",       "J1",   },                 // USB1_DP
        {  "K13",      "H1",      "26",       "K1",   },                 // USB1_DM
        {  "K12",      "J2",      "27",       "L1",   },                 // USB1_VBUS
        {  "J11",      "L1",      "-",        "-",    },                 // ADC1_DP1
        {  "K11",      "M1",      "-",        "-",    },                 // ADC1_DM1
        {  "L13",      "M2",      "-",        "-",    },                 // ADC0_DP0/ADC1_DP3
        {  "M13",      "L2",      "28",       "L2",   },                 // ADC0_DM0/ADC1_DM3
        {  "L12",      "N1",      "29",       "M1",   },                 // ADC1_DP0/ADC0_DP3
        {  "M12",      "N2",      "30",       "M2",   },                 // ADC1_DM0/ADC0_DM3
        {  "M1",       "L13",     "74",       "L12",  },                 // RESET_b
        {  "H11",      "M3",      "35",       "K3",   },                 // ADC1_SE16/CMP2_IN2/ADC0_SE22
        {  "K10",      "L3",      "36",       "J3",   },                 // ADC0_SE16/CMP1_IN2/ADC0_SE21
        {  "L10",      "N3",      "37",       "M3",   },                 // VREF_OUT/CMP1_IN5/CMP0_IN5/ADC1_SE18
        {  "M10",      "M4",      "38",       "L3",   },                 // DAC0_OUT/CMP1_IN3/ADC0_SE23
        {  "N11",      "N4",      "39",       "L4",   },                 // DAC1_OUT/CMP0_IN4/CMP2_IN3/ADC1_SE23
        {  "J10",      "M5",      "-",        "L5",   },                 // TAMPER0/RTC_WAKEUP_B
    #if defined KINETIS_K65
        {  "H10",      "L4",      "-",        "-",    },                 // TAMPER1
        {  "H9",       "L5",      "-",        "-",    },                 // TAMPER2
        {  "J9",       "K5",      "-",        "-",    },                 // TAMPER3
        {  "N10",      "L6",      "-",        "-",    },                 // TAMPER4
        {  "K9",       "K6",      "-",        "-",    },                 // TAMPER5
    #else
        {  "-",        "-",       "-",        "-",    },                 // no contact
        {  "-",        "-",       "-",        "-",    },                 // no contact
        {  "-",        "-",       "-",        "-",    },                 // no contact
        {  "-",        "-",       "-",        "-",    },                 // no contact
        {  "-",        "-",       "-",        "-",    },                 // no contact
    #endif
        {  "M9",       "N5",      "40",       "M7",   },                 // XTAL32
        {  "N9",       "N6",      "41",       "M6",   },                 // EXTAL32
        {  "L9",       "M6",      "42",       "L6",   },                 // VBAT
    #if defined KINETIS_K65
        {  "H8",       "J6",      "-",        "J6",   },                 // TAMPER6
        {  "J8",       "J5",      "-",        "J5",   },                 // TAMPER7
    #else
        {  "-",        "-",       "-",        "-",    },                 // no contact
        {  "-",        "-",       "-",        "-",    },                 // no contact
    #endif
        {  "-",        "-",       "-",        "-",    },
    },
};

#define _DEDICATED_PINS
static CHAR *cDedicated[PORT_WIDTH] = {                                  // dedicated pin functions
    "USB0_DP",
    "USB0_DM",
    "VREG_OUT",
    "VREG_IN0",
    "VREG_IN1",
    "USB1_DP",
    "USB1_DM",
    "USB1_VBUS",
    "ADC1_DP1",
    "ADC1_DM1",
    "ADC0_DP0/ADC1_DP3",
    "ADC0_DM0/ADC1_DM3",
    "ADC1_DP0/ADC0_DP3",
    "ADC1_DM0/ADC0_DM3",
    "RESET_b",
    "ADC1_SE16/CMP2_IN2/ADC0_SE22",
    "ADC0_SE16/CMP1_IN2/ADC0_SE21",
    "VREF_OUT/CMP1_IN5/CMP0_IN5/ADC1_SE18",
    "DAC0_OUT/CMP1_IN3/ADC0_SE23",
    "DAC1_OUT/CMP0_IN4/CMP2_IN3/ADC1_SE23",
    #if defined KINETIS_K65
    "TAMPER0/RTC_WAKEUP_B",
    #else
    "RTC_WAKEUP_B",
    #endif
    "TAMPER1",
    "TAMPER2",
    "TAMPER3",
    "TAMPER4",
    "TAMPER5",
    "XTAL32",
    "EXTAL32",
    "VBAT",
    "TAMPER6",
    "TAMPER7",
};

// Table not yet complete
//
static int ADC_DEDICATED_CHANNEL[PORT_WIDTH] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
static int ADC_DEDICATED_MODULE[PORT_WIDTH]  = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
static int ADC_MUX_CHANNEL[PORTS_AVAILABLE][PORT_WIDTH] = {0};           // not yet controlled


static const char *cPer[PORTS_AVAILABLE][PORT_WIDTH][8] = {              // 169 pin WLCSP/MAPBGA / 144 pin LQFP/MAPBGA
    {
        // ALT 0           ALT 1    ALT2         ALT 3         ALT 4          ALT 5        ALT 6       ALT 7
        {  "TSI0_CH1",     "PTA0",  "UART0_CTS_b/UART0_COL_b","FTM0_CH5","-", "LPUART0_CTS_b","-",     "JTAG_TCLK/SWD_CLK" }, // PORT A
        {  "TSI0_CH2",     "PTA1",  "UART0_RX",  "FTM0_CH6",   "I2C3_SDA",    "LPUART0_RX","-",        "JTAG_TDI"          },
        {  "TSI0_CH3",     "PTA2",  "UART0_TX",  "FTM0_CH7",   "I2C3_SCL",    "LPUART0_TX","-",        "JTAG_TDI/TRACE_SWO"},
        {  "TSI0_CH4",     "PTA3",  "UART0_RTS_b","FTM0_CH0",  "-",           "LPUART0_RTS_b","-",     "JTAG_TMS/SWD_DIO"  },
        {  "TSI0_CH5",     "PTA4/LLWU_P3","-",   "FTM0_CH1",   "-",           "-",         "-",        "NMI_b"             },
        #if defined KINETIS_K26
        {  "-",            "PTA5",  "USB0_CLKIN","FTM0_CH2",   "-",           "CMP2_OUT",  "I2S0_TX_BCLK","JTAG_TRST_b"    },
        {  "-",            "PTA6",  "-",         "FTM0_CH3",   "-",           "CLKOUT",    "-",        "TRACE_CLKOUT"      },
        {  "ADC0_SE10",    "PTA7",  "-",         "FTM0_CH4",   "-",           "MII_MDIO",  "-",        "TRACE_D3"          },
        {  "ADC0_SE11",    "PTA8",  "-",         "FTM1_CH0",   "-",           "MII_MDC",   "FTM1_QD_PHA/TPM1_CH0","TRACE_D2"},
        {  "-",            "PTA9",  "-",         "FTM1_CH1",   "-",           "-",         "FTM1_QD_PHB/TPM1_CH1","TRACE_D1"},
        {  "-",            "PTA10/LLWU_P22","-", "FTM2_CH0",   "-",           "",         "FTM2_QD_PHA/TPM2_CH0","TRACE_D0"},
        {  "-",            "PTA11/LLWU_P23","-", "FTM2_CH1",   "-",           "I2C2_SDA",  "FTM2_QD_PHB/TPM2_CH1","-"      },
        {  "CMP2_IN0",     "PTA12", "CAN0_TX",   "FTM1_CH0",   "-",           "I2C2_SCL",  "I2S0_TXD", "FTM1_QD_PHA/TPM1_CH0"},
        {  "CMP2_IN1",     "PTA13/LLWU_P4","CAN0_RX","FTM1_CH1","-",          "I2C2_SDA",  "I2S0_TX_FS","FTM1_QD_PHB/TPM1_CH1"},
        {  "-",            "PTA14", "SPI0_PCS0", "UART0_TX",   "-",           "I2C2_SCL",  "I2S0_RX_BCLK","I2S0_TXD1"      },
        {  "CMP3_IN1",     "PTA15", "SPI0_SCK",  "UART0_RX",   "-",           "-",         "I2S0_RXD", "-"                 },
        {  "CMP3_IN2",     "PTA16", "SPI0_SOUT", "UART0_CTS_b/UART0_COL_b","-","-",        "I2S0_RX_FS","I2S0_RXD1"        },
        {  "ADC1_SE17",    "PTA17", "SPI0_SIN",  "UART0_RTS_b","-",           "-",         "I2S0_MCLK","-"                 },
        #else
        {  "-",            "PTA5",  "USB0_CLKIN","FTM0_CH2",   "MII_RXER",    "CMP2_OUT",  "I2S0_TX_BCLK","JTAG_TRST_b"    },
        {  "-",            "PTA6",  "-",         "FTM0_CH3",   "-",           "CLKOUT",    "-",        "TRACE_CLKOUT"      },
        {  "ADC0_SE10",    "PTA7",  "-",         "FTM0_CH4",   "-",           "MII_MDIO",  "-",        "TRACE_D3"          },
        {  "ADC0_SE11",    "PTA8",  "-",         "FTM1_CH0",   "-",           "MII_MDC",   "FTM1_QD_PHA/TPM1_CH0","TRACE_D2"},
        {  "-",            "PTA9",  "-",         "FTM1_CH1",   "MII_RXD3",    "-",         "FTM1_QD_PHB/TPM1_CH1","TRACE_D1"},
        {  "-",            "PTA10/LLWU_P22","-", "FTM2_CH0",   "MII_RXD2",    "-",        "FTM2_QD_PHA/TPM2_CH0","TRACE_D0"},
        {  "-",            "PTA11/LLWU_P23","-", "FTM2_CH1",   "MII_RXCLK",   "I2C2_SDA",  "FTM2_QD_PHB/TPM2_CH1","-"      },
        {  "CMP2_IN0",     "PTA12", "CAN0_TX",   "FTM1_CH0",   "MII_RXD1",    "I2C2_SCL",  "I2S0_TXD", "FTM1_QD_PHA/TPM1_CH0"},
        {  "CMP2_IN1",     "PTA13/LLWU_P4","CAN0_RX","FTM1_CH1","MII_RXD0",   "I2C2_SDA",  "I2S0_TX_FS","FTM1_QD_PHB/TPM1_CH1"},
        {  "-",            "PTA14", "SPI0_PCS0", "UART0_TX",   "MII_CRS_DV",  "I2C2_SCL",  "I2S0_RX_BCLK","I2S0_TXD1"      },
        {  "CMP3_IN1",     "PTA15", "SPI0_SCK",  "UART0_RX",   "MII_TXEN",    "-",         "I2S0_RXD", "-"                 },
        {  "CMP3_IN2",     "PTA16", "SPI0_SOUT", "UART0_CTS_b/UART0_COL_b","MII_TXD0","-", "I2S0_RX_FS","I2S0_RXD1"        },
        {  "ADC1_SE17",    "PTA17", "SPI0_SIN",  "UART0_RTS_b","MII_TXD1",    "-",         "I2S0_MCLK","-"                 },
        #endif
        {  "EXTAL0",       "PTA18", "-",         "FTM0_FLT2",  "FTM_CLKIN0",  "-",         "-",        "TPM_CLKIN0"        },
        {  "XTAL0",        "PTA19", "-",         "FTM1_FLT0",  "FTM_CLKIN1",  "-",         "LPT0_ALT1","TPM_CLKIN1"        },
        {  "-",            "-",     "-",         "-",          "-",           "-",         "-",        "-"                 },
        {  "-",            "-",     "-",         "-",          "-",           "-",         "-",        "-"                 },
        {  "-",            "-",     "-",         "-",          "-",           "-",         "-",        "-"                 },
        {  "-",            "-",     "-",         "-",          "-",           "-",         "-",        "-"                 },
        {  "CMP3_IN4",     "PTA24", "-",         "-",          "MII_TXD2",    "FB_A15/SDRAM_D15", "FB_A29", "-"            },
        {  "CMP3_IN5",     "PTA25", "-",         "-",          "MII_TXCLK",   "FB_A14/SDRAM_D14", "FB_A28", "-"            },
        {  "-",            "PTA26", "-",         "-",          "MII_TXD3",    "FB_A13/SDRAM_D13", "FB_A27", "-"            },                
        {  "-",            "PTA27", "-",         "-",          "MII_CRS",     "FB_A12/SDRAM_D12", "FB_A26", "-"            },
        {  "-",            "PTA28", "-",         "-",          "MII_TXER",    "-",         "FB_A25",   "-"                 },
        {  "-",            "PTA29", "-",         "-",          "MII_COL",     "-",         "FB_A24",   "-"                 },
    #if defined KINETIS_K26 || defined KINETIS_K65
        {  "-",            "PTA30", "CAN0_TX",   "-",          "-",           "FB_A11/SDRAM_D11", "-", "-"                 },
        {  "-",            "PTA31", "CAN0_RX",   "-",          "-",           "FB_A10/SDRAM_D10", "-", "-"                 }
    #else
        {  "-",            "-",     "-",         "-",          "-",           "-",         "-",        "-"                 },
        {  "-",            "-",     "-",         "-",          "-",           "-",         "-",        "-"                 },
    #endif
    },
    {
        // ALT 0           ALT 1    ALT2         ALT 3         ALT 4          ALT 5        ALT 6       ALT 7
    #if defined KINETIS_K26
        {  "ADC0_SE8/ADC1_SE8/TSI0_CH1","PTB0/LLWU_P5","I2C0_SCL","FTM1_CH0","-","SDRAM_CAS_b","FTM1_QD_PHA/TPM1_CH0", "-" }, // PORT B
        {  "ADC0_SE9/ADC1_SE9/TSI0_CH6","PTB1","I2C0_SDA","FTM1_CH1","-",     "SDRAM_RAS_b","FTM1_QD_PHB/TPM1_CH1", "-"    },
        {  "ADC0_SE12/TSI0_CH7","PTB2","I2C0_SCL","UART0_RTS_b","-",          "SDRAM_WE","FTM0_FLT3",  "-"                 },
        {  "ADC0_SE13/TSI0_CH8","PTB3","I2C0_SDA","UART0_CTS_b/UART0_COL_b","-","SDRAM_CS0_b","FTM0_FLT0","-"              },
        {  "ADC1_SE10",    "PTB4",  "-",         "-",          "-",           "SDRAM_CS1_b", "FTM1_FLT0","-"               },
        {  "ADC1_SE11",    "PTB5",  "-",         "-",          "-",           "-",       "FTM2_FLT0",  "-"                 },
    #else
        {  "ADC0_SE8/ADC1_SE8/TSI0_CH1","PTB0/LLWU_P5","I2C0_SCL","FTM1_CH0","MII_MDIO","SDRAM_CAS_b","FTM1_QD_PHA/TPM1_CH0", "-" }, // PORT B
        {  "ADC0_SE9/ADC1_SE9/TSI0_CH6","PTB1","I2C0_SDA","FTM1_CH1","MII_MCD","SDRAM_RAS_b","FTM1_QD_PHB/TPM1_CH1", "-"   },
        {  "ADC0_SE12/TSI0_CH7","PTB2","I2C0_SCL","UART0_RTS_b","ENET_1588_TMR0","SDRAM_WE","FTM0_FLT3","-"                },
        {  "ADC0_SE13/TSI0_CH8","PTB3","I2C0_SDA","UART0_CTS_b/UART0_COL_b","ENET_1588_TMR1","SDRAM_CS0_b","FTM0_FLT0","-" },
        {  "ADC1_SE10",    "PTB4",  "-",         "-",          "ENET_1588_TMR2","SDRAM_CS1_b", "FTM1_FLT0","-"             },
        {  "ADC1_SE11",    "PTB5",  "-",         "-",          "ENET_1588_TMR3","-",       "FTM2_FLT0","-"                 },
    #endif
        {  "ADC1_SE12",    "PTB6",  "-",         "-",          "-",           "FB_AD23/SDRAM_D23","-", "-"                 },
        {  "ADC1_SE13",    "PTB7",  "-",         "-",          "-",           "FB_AD22/SDRAM_D22","-", "-"                 },
        {  "-",            "PTB8",  "-",         "UART3_RTS_b","-",           "FB_AD21/SDRAM_D21","-", "-"                 },
        {  "-",            "PTB9",  "SPI1_PCS1", "UART3_CTS_b","-",           "FB_AD20/SDRAM_D20","-", "-"                 },
        {  "ADC1_SE14",    "PTB10", "SPI1_PCS0", "UART3_RX",   "-",           "FB_AD19/SDRAM_D19","FTM0_FLT1","-"          },
        {  "ADC1_SE15",    "PTB11", "SPI1_SCK",  "UART3_TX",   "-",           "FB_AD18/SDRAM_D18","FTM0_FLT2","-"          },
    #if defined KINETIS_K26 || defined KINETIS_K65
        {  "-",            "PTB12", "UART3_RTS_b","FTM1_CH0","FTM0_CH4","FB_A9/SDRAM_D9","FTM1_QD_PHY/TPM1_CH0","-"        },
        {  "-",            "PTB13", "UART3_CTS_b","FTM1_CH1",  "FTM0_CH5",    "FB_A8/SDRAM_D8","FTM1_QD_PHB/TPM1_CH1", "-" },
        {  "-",            "PTB14", "CAN1_TX",   "-",          "-",           "FB_A7/SDRAM_D7","-",    "-"                 },
        {  "-",            "PTB15", "CAN1_RX",   "-",          "-",           "FB_A6/SDRAM_D6","-",    "-"                 },
    #else
        {  "-",            "-",     "-",         "-",          "-",           "-",         "-",        "-"                 },
        {  "-",            "-",     "-",         "-",          "-",           "-",         "-",        "-"                 },
        {  "-",            "-",     "-",         "-",          "-",           "-",         "-",        "-"                 },
        {  "-",            "-",     "-",         "-",          "-",           "-",         "-",        "-"                 },
    #endif
        {  "TSI0_CH9",     "PTB16", "SPI1_SOUT", "UART0_RX",   "FTM_CLKIN0",  "FB_AD17/SDRAM_D17","EWM_IN","TPM_CLKIN0"    },
        {  "TSI0_CH10",    "PTB17", "SPI1_SIN",  "UART0_TX",   "FTM_CLKIN1",  "FB_AD16/SDRAM_D16","EWM_OUT_b","TPM_CLKIN1" },
        {  "TSI0_CH11",    "PTB18", "CAN0_TX",   "FTM2_CH0",   "I2S0_TX_BCLK","FB_AD15/SDRAM_A23","FTM2_QD_PHA/TPM2_CH0","-"},
        {  "TSI0_CH12",    "PTB19", "CAN0_RX",   "FTM2_CH1",   "I2S0_TX_FS",  "FB_OE_b",   "FTM2_QD_PHB/TPM2_CH1","-"      },
        {  "-",            "PTB20", "SPI2_PCS0", "-",          "-",           "FB_AD31/SDRAM_D31","CMP0_OUT", "-"          },
        {  "-",            "PTB21", "SPI2_SCK",  "-",          "-",           "FB_AD30/SDRAM_D30","CMP1_OUT","-"           },
        {  "-",            "PTB22", "SPI2_SOUT", "-",          "-",           "FB_AD29/SDRAM_D29","CMP2_OUT","-"           },
        {  "-",            "PTB23", "SPI2_SIN",  "SPI0_PCS5",  "-",           "FB_AD28/SDRAM_D28","CMP3_OUT", "-"          },
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
        {  "ADC0_SE14/TSI0_CH13","PTC0","SPI0_PCS4", "PDB0_EXTRG","USB0_SOF_OUT","FB_AD14/SDRAM_A22","I2S0_TXD1", "-"      }, // PORT C
        {  "ADC0_SE15/TSI0_CH14","PTC1/LLWU_P6","SPI0_PCS3","UART1_RTS_b","FTM0_CH0","FB_AD13/SDRAM_A21","I2S0_TXD0", "-"  },
        {  "ADC0_SE4b/CMP1_IN0/TSI0_CH15","PTC2","SPI0_PCS2","UART1_CTS_b","FTM0_CH1","FB_AD12/SDRAM_A20","I2S0_TX_FS","-" },
        {  "CMP1_IN1",     "PTC3/LLWU_P7","SPI0_PCS1","UART1_RX","FTM0_CH2",  "CLKOUT",    "I2S0_TX_BCLK","-"              },
        {  "-",            "PTC4/LLWU_P8","SPI0_PCS0","UART1_TX","FTM0_CH3",  "FB_AD11/SDRAM_A19","CMP1_OUT", "-"          },
        {  "-",            "PTC5/LLWU_P9","SPI0_SCK","LPTMR0_ALT2","I2S0_RXD0","FB_AD10/SDRAM_A18","CMP0_OUT", "FTM0_CH2"  },
        {  "CMP0_IN0",     "PTC6/LLWU_P10","SPI0_SOUT","PDB0_EXTRG","I2S0_RX_BCLK","FB_AD9/SDRAM_A17","I2S0_MCLK","-"      },
        {  "CMP0_IN1",     "PTC7",  "SPI0_SIN",  "USB0_SOF_OUT","I2S0_RX_FS", "FB_AD8/SDRAM_A16","-",  "-"                 },
        {  "ADC1_SE4b/CMP0_IN2","PTC8","-",      "FTM3_CH4",   "I2S0_MCLK",   "FB_AD7/SDRAM_A15","-",  "-"                 },
        {  "ADC1_SE5b/CMP0_IN3","PTC9","-",      "FTM3_CH5",   "I2S0_RX_BCLK","FB_AD6/SDRAM_A14","FTM2_FLT0","-"           },
        {  "ADC1_SE6b",    "PTC10","I2C1_SCL",   "FTM3_CH6",   "I2S0_RX_FS",  "FB_AD5/SDRAM_A13","-",  "-"                 },
        {  "ADC1_SE7b",    "PTC11/LLWU_P11","I2C1_SDA","FTM3_CH7","I2S0_RXD", "FB_RW_b",   "-",        "-"                 },
        {  "-",            "PTC12", "-",         "UART4_RTS_b","FTM_CLKIN0",  "FB_AD27/SDRAM_D27","FTM3_FLT3","TPM_CLKIN0" },
        {  "-",            "PTC13", "-",         "UART4_CTS_b","FTM_CLKIN1",  "FB_AD26/SDRAM_D26","-", "TPM_CLKIN1"        },
        {  "-",            "PTC14", "-",         "UART4_RX",   "-",           "FB_AD25/SDRAM_D25","-", "-"                 },
        {  "-",            "PTC15", "-",         "UART4_TX",   "-",           "FB_AD24/SDRAM_D24","-","-"                 },
        {  "-",            "PTC16", "CAN1_RX",   "UART3_RX",   "ENET_1588_TMR0","FB_CS5/FB_TSIZ1/FB_BE23_16_BLS15_8/SDRAM_DQM2","-","-"},
        {  "-",            "PTC17", "CAN1_TX",   "UART3_TX",   "ENET_1588_TMR1","FB_CS4/FB_TSIZ0/FB_BE31_24_BLS7_0/SDRAM_DQM3","-","-"},
        {  "-",            "PTC18", "-",         "UART3_RTS_b","ENET_1588_TMR2","FB_TBST/FB_CS2/FB_BE15_8_BLS23_16/SDRAM_DQM1","-","-"},
        {  "-",            "PTC19", "-",         "UART3_CTS_b","ENET_1588_TMR3","FB_CS3/FB_BE7_0_BLS31_24/SDRAM_DQM0","FB_TA","-"     },
        {  "-",            "-",     "-",         "-",          "-",           "-",         "-",        "-"                 },
        {  "-",            "-",     "-",         "-",          "-",           "-",         "-",        "-"                 },
        {  "-",            "-",     "-",         "-",          "-",           "-",         "-",        "-"                 },
        {  "-",            "-",     "-",         "-",          "-",           "-",         "-",        "-"                 },
    #if defined KINETIS_K65
        {  "-",            "-",     "-",         "-",          "-",           "-",         "-",        "-"                 },
        {  "-",            "-",     "-",         "-",          "-",           "-",         "-",        "-"                 },
        {  "-",            "-",     "-",         "-",          "-",           "-",         "-",        "-"                 },
        {  "-",            "-",     "-",         "-",          "-",           "-",         "-",        "-"                 },
        {  "-",            "-",     "-",         "-",          "-",           "-",         "-",        "-"                 },
        {  "-",            "-",     "-",         "-",          "-",           "-",         "-",        "-"                 },
    #else
        {  "-",            "PTC24", "-",         "LPUART0_TX", "-",           "FB_A5/SDRAM_D5","-",    "-"                 },
        {  "-",            "PTC25", "-",         "LPUART0_RX", "-",           "FB_A4/SDRAM_D4","-",    "-"                 },
        {  "-",            "PTC26", "-",         "LPUART0_CTS_b","ENET0_1588_TMR0","FB_A3/SDRAM_D3","-","-"                },
        {  "-",            "PTC27", "-",         "LPUART0_RTS_b","ENET0_1588_TMR1","FB_A2/SDRAM_D2","-","-"                },
        {  "-",            "PTC28", "I2C3_SDA",  "-",          "ENET0_1588_TMR2","FB_A1/SDRAM_D1","-", "-"                 },
        {  "-",            "PTC29", "I2C3_SCL",  "-",          "ENET0_1588_TMR3","FB_A0/SDRAM_D0","-", "-"                 },
    #endif
        {  "-",            "-",     "-",         "-",          "-",           "-",         "-",        "-"                 },
        {  "-",            "-",     "-",         "-",          "-",           "-",         "-",        "-"                 }
    },
    {
        // ALT 0           ALT 1    ALT2         ALT 3         ALT 4          ALT 5        ALT 6       ALT 7
        {  "-",            "PTD0/LLWU_P12","SPI0_PCS0", "UART2_RTS_b","FTM3_CH0","FB_ALE/FB_CS1/FB_TS","-","-"             }, // PORT D
        {  "ADC0_SE5b",    "PTD1",  "SPI0_SCK",  "UART2_CTS_b", "FTM3_CH1",   "FB_CS0_b",  "-",        "-"                 },
        {  "-",            "PTD2/LLWU_P13","SPI0_SOUT","UART2_RX","FTM3_CH2", "FB_AD4/SDRAM_A12","-",  "I2C0_SCL"          },
        {  "-",            "PTD3",  "SPI0_SIN",  "UART2_TX",   "FTM3_CH3",    "FB_AD3/SDRAM_A11","-",  "I2C0_SDA"          },
        {  "-",            "PTD4/LLWU_P14","SPI0_PCS1","UART0_RTS_b","FTM0_CH4","FB_AD2/SDRAM_A10","EWM_IN","SPI1_PCS0"    },
        {  "ADC0_SE6b",    "PTD5",  "SPI0_PCS2", "UART0_CTS_b/UART0_COL_b","FTM0_CH5","FB_AD1/SDRAM_A9","EWM_OUT_b","SPI1_SCK" },
        {  "ADC0_SE7b",    "PTD6/LLWU_P15","SPI0_PCS3","UART0_RX","FTM0_CH6", "FB_AD0",    "FTM0_FLT0","SPI1_SOUT"         },
        {  "-",            "PTD7",  "CMT_IRO",   "UART0_TX",   "FTM0_CH7",    "SDRAM_CKE", "FTM0_FLT1","SPI1_IN"           },
        {  "-",            "PTD8/LLWU_P24","I2C0_SCL","-",     "-",           "LPUART0_RX","FB_A16",   "-"                 },
        {  "-",            "PTD9",  "I2C0_SDA",  "-",          "-",           "LPUART0_TX","FB_A17",   "-"                 },
        {  "-",            "PTD10", "-",         "-",          "-",           "LPUART0_RTS_b","FB_A18", "-"                },
        {  "-",            "PTD11/LLWU_P25","SPI2_PCS0","-",   "SDHC0_CLKIN", "LPUART0_CTS_b","FB_A19","-"                 },
        {  "-",            "PTD12", "SPI2_SCK",  "FTM3_FLT0",  "SDHC0_D4",    "-",         "FB_A20",   "-"                 },
        {  "-",            "PTD13", "SPI2_SOUT", "-",          "SDHC0_D5",    "-",         "FB_A21",   "-"                 },
        {  "-",            "PTD14", "SPI2_SIN",  "-",          "SDHC0_D6",    "-",         "FB_A22",   "-"                 },
        {  "-",            "PTD15", "SPI2_PCS1", "-",          "SDHC0_D7",    "-",         "FB_A23",   "-"                 },
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
        {  "ADC1_SE4a",    "PTE0",  "SPI1_PCS1", "UART1_TX",   "SDHC0_D1",    "TRACE_CLKOUT","I2C1_SDA","RTC_CLKOUT"       }, // PORT E
        {  "ADC1_SE5a",    "PTE1/LLWU_P0","SPI1_SOUT","UART1_RX","SDHC0_D0",  "TRACE_D3",  "I2C1_SCL", "SPI1_SIN"          }, 
        {  "ADC1_SE6a",    "PTE2/LLWU_P1","SPI1_SCK","UART1_CTS_b","SDHC0_DCLK","TRACE_D2","-",        "-"                 },
        {  "ADC1_SE7a",    "PTE3",  "SPI1_SIN","UART1_RTS_b",  "SDHC0_CMD",   "TRACE_D1",  "-",        "SPI1_SOUT"         },
        {  "-",            "PTE4/LLWU_P2","SPI1_PCS0","UART3_TX","SDHC0_D3",  "TRACE_D0",  "-",        "-"                 },
        {  "-",            "PTE5",  "SPI1_PCS2", "UART3_RX",   "SDHC0_D2",    "-",         "FTM3_CH0", "-"                 },
        {  "-",            "PTE6",  "SPI1_PCS3", "UART3_CTS_b","I2S0_MCLK",   "-",         "FTM3_CH1", "USB_SOF_OUT"       },
        {  "-",            "PTE7",  "-",         "UART3_RTS_b","I2S0_RXD",    "-",         "FTM3_CH2", "-"                 },
        {  "-",            "PTE8",  "I2S0_RXD1", "-",          "I2S0_RX_FS",  "LPUART0_TX","FTM3_CH3", "-"                 },
        {  "-",            "PTE9",  "I2S0_RXD1", "-",          "I2S0_RX_BCLK","LPUART0_RX","FTM3_CH4", "-"                 },
        {  "-",            "PTE10", "I2C3_SDA",  "-",          "I2S0_TXD0",   "LPUART0_CTS_b","FTM3_CH5","USB1_ID"         },
        {  "-",            "PTE11", "I2C3_SCL",  "-",          "I2S0_TX_FS",  "LPUART0_RTS_b","FTM3_CH6","-"               },
        {  "-",            "PTE12", "-",         "-",          "I2S0_TX_BCLK","-",         "FTM3_CH7", "-"                 },
        {  "-",            "-",     "-",         "-",          "-",           "-",         "-",        "-"                 },
        {  "-",            "-",     "-",         "-",          "-",           "-",         "-",        "-"                 },
        {  "-",            "-",     "-",         "-",          "-",           "-",         "-",        "-"                 },
    #if defined KINETIS_K26 || defined KINETIS_K65
        {  "ADC0_SE4a",    "PTE16", "SPI0_PCS0", "UART2_TX",   "FTM_CLKIN0",  "-",         "FTM0_FLT3","TPM_CLKIN0"        },
        {  "ADC0_SE5a",    "PTE17/LLWU_P19","SPI0_SCK","UART2_RX","FTM_CLKIN1","-",        "LPTMR0_ALT3","TPM_CLKIN1"      },
        {  "ADC0_SE6a",    "PTE18/LLWU_P20","SPI0_SOUT","UART2_CTS_b","I2C0_SDA","-",      "-",        "-"                 },
        {  "ADC0_SE7a",    "PTE19", "SPI0_SIN",  "UART2_RTS_b","I2C0_SCL",    "-",         "CMP3_OUT", "-"                 },
    #else
        {  "-",            "-",     "-",         "-",          "-",           "-",         "-",        "-"                 },
        {  "-",            "-",     "-",         "-",          "-",           "-",         "-",        "-"                 },
        {  "-",            "-",     "-",         "-",          "-",           "-",         "-",        "-"                 },
        {  "-",            "-",     "-",         "-",          "-",           "-",         "-",        "-"                 },
    #endif
        {  "-",            "-",     "-",         "-",          "-",           "-",         "-",        "-"                 },
        {  "-",            "-",     "-",         "-",          "-",           "-",         "-",        "-"                 },
        {  "-",            "-",     "-",         "-",          "-",           "-",         "-",        "-"                 },
        {  "-",            "-",     "-",         "-",          "-",           "-",         "-",        "-"                 },
        {  "ADC0_SE17",    "PTE24", "CAN1_TX",   "UART4_TX",   "-",           "I2C0_SCL",  "EWM_OUT_b","-"                 },
        {  "ADC0_SE18",    "PTE25/LLWU_P21","CN1_RX","UART4_RX","-",          "I2C0_SDA",  "EWM_IN",   "-"                 },
        {  "-",            "PTE26", "ENET_1588_CLKIN","UART4_CTS_b","-",      "-",         "RTC_CLKOUT","USB0_CLKIN"       },
        {  "-",            "PTE27", "-",         "UART4_RTS_b","-",           "-",         "-",        "-"                 },
        {  "-",            "PTE28", "-",         "-",          "-",           "-",         "-",        "-"                 },
        {  "-",            "-",     "-",         "-",          "-",           "-",         "-",        "-"                 },
        {  "-",            "-",     "-",         "-",          "-",           "-",         "-",        "-"                 },
        {  "-",            "-",     "-",         "-",          "-",           "-",         "-",        "-"                 }
    }
};

