/***********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher

    ---------------------------------------------------------------------
    File:      kinetis_port_k53.h
    Project:   Single Chip Embedded Internet
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2018
    *********************************************************************
    
*/

static const char *cPinNumber[PORTS_AVAILABLE][PORT_WIDTH] = {           // 144 pin
    // PORT A (0..31)
    { "50/J5 {PTA0/TSI0_CH1/UART0_CTS_b/FTM0_CH5/JTAG_TCLK/SWD_CLK/EZP_CLK}", "51/J6 {PTA1/TSI0_CH2/UART0_RX/FTM0_CH6/JTAG_TDI/EZP_DI}", "52/K6 {PTA2/TSI0_CH3/UART0_TX/FTM0_CH7/JTAG_TDO/TRACE_SWO/EZP_DO}", "53/K7 {PTA3/TSI0_CH4/UART0_RTS_b/FTM0_CH0/JTAG_TMS/SWD_DIO}", "54/L7 {PTA4/TSI0_CH5/FTM0_CH1/NMI_b/EZP_CS_b}", "55/M8 {PTA5/FTM0_CH2/MII_RXER/CMP2_OUT/I2S0_RX_BCLK/JTAG_TRST}", "58/J7 {PTA6/FTM0_CH3/TRACE_CLKOUT}", "59/J8 {PTA7/ADC0_SE10/FTM0_CH4/FB_AD18/TRACE_D3}", "60/K8 {PTA8/ADC0_SE11/FTM1_CH0/FB_AD17/FTM1_QD_PHA/TRACE_D2}", "61/L8 {PTA9/FTM1_CH1/MII_RXD3/FB_AD16/FTM1_QD_PHB/TRACE_D1}", "62/M9 {PTA10/FTM2_CH0/MII_RXD2/FB_AD15/FTM2_QD_PHA/TRACE_D0}", "63/L9 {PTA11/FTM2_CH1/MII_RXCLK/FB_OE_b/FTM2_QD_PHB}", "64/K9 {PTA12/CMP2_IN0/FTM1_CH0/MII_RXD1/FB_CS5/FB_TSIZ1/FB_BE23_16_BLS15_8/I2S0_TXD/FTM1_QD_PHA}", "65/J9 {PTA13/CMP2_IN1/FTM1_CH1/MII_RXD0/FB_CS4/FB_TSIZ0/FB_BE31_24_BLS7_0/I2S0_TX_FS/FTM1_QD_PHB}", "66/L10 {PTA14/SPI0_PCS0/UART0_TX/MII_CRS_DV/FB_AD31/I2S0_TX_BCLK}", "67/L11 {PTA15/SPI0_SCK/UART0_RX/MII_TXEN/FB_AD30/I2S0_RXD}", "68/K10 {PTA16/SPI0_SOUT/UART0_CTS_b/MII_TXD0/FB_AD29/I2S0_RX_FS}", "69/K11 {PTA17/ADC1_SE17/SPI0_SIN/UART0_RTS_b/MII_TXD1/FB_AD28/I2S0_MCLK/ISS0_CLKIN}", "72/M12 {PTA18/EXTAL/FTM0_FLT2/FTM_CLKIN0}", "73/M11 {PTA19/XTAL/FTM1_FLT0/FTM_CLKIN1/LPT0_ALT1}", "NA", "NA", "NA", "NA", "75/K12 {PTA24/MII_TXD2/FB_AD14}", "76/J12 {PTA25/MII_TXCLK/FB_AD13}", "77/J11 {PTA26/MII_TXD3/FB_AD12}", "78/J10 {PTA27/MII_CRS/FB_AD11}", "79/H12 {PTA28/MII_TXER/FB_AD10}", "80/H11 {PTA29/MII_COL/FB_AD19}", "NA", "NA"},
    // PORT B (0..31)
    { "81/H10 {PTB0/ADC0_SE8/ADC1_SE8/TSI0_CH0/I2C0_SCL/FTM1_CH0/MII_MDIO/FTM1_QD_PHA/LCD_P0}", "82/H9 {PB.1/ADC0_SE9/ADC1_SE9/TSI0_CH6/I2C0_SDA/FTM1_CH1/MII_MCD/FTM1_QD_PHB/LCD_P1}", "83/G12 {PB.2/ADC0_SE12/TSI0_CH7/I2C0_SCL/UART0_RTS_b/ENET_1588_TMR0/FTM0_FLT3/LCD_P2}", "84/G11 {PB.3/ADC0_SE13/TSI0_CH8/I2C0_SDA/UART0_CTS_b/ENET_1588_TMR1/FTM0_FLT0/LCD_P3}", "85/G10 {PB.4/ADC1_SE10/ENET_1588_TMR2/FTM1_FLT0/LCD_P4}", "86/G9 {PB.5/ADC1_SE11/ENET_1588_TMR3/FTM2_FLT0/LCD_P5}", "87/F12 {PB.6/ADC1_SE12/LCD_P6}", "88/F11 {PB.7/ADC1_SE13/LCD_P7}", "89/F10 {PB.8/UART3_RTS_b/LCD_P8}", "90/F9 {PB.9/SPI1_PCS1/UART3_CTS_b/LCD_P9}", "91/E12 {PB.10/ADC1_SE14/SPI1_PCS0/UART3_RX/FTM0_FLT1/LCD_P10}", "92/E11 {PB.11/ADC1_SE15/SPI1_SCK/UART3_TX/FTM0_FLT2/LCD_P11}", "NA", "NA", "NA", "NA", "95/E10 {PB.16/TSI0_CH9/SPI1_SOUT/UART0_RX/EWM_IN/LCD_P12}", "96/E9 {PB.17/TSI0_CH10/SPI1_SIN/UART0_TX/EWM_OUT_b/LCD_P13}", "97/D12 {PB.18/TSI0_CH11/FTM2_CH0/I2S0_TX_BCLK/FTM2_QD_PHA/LCD_P14}", "98/D11 {PB.19/TSI0_CH12/FTM2_CH1/I2S0_TX_FS/FTM2_QD_PHB/LCD_P15}", "99/D10 {PB.20/SPI2_PCS0/CMP0_OUT/LCD_P16}", "100/D9 {PB.21/SPI2_SCK/CMP1_OUT/LCD_P17}", "101/C12 {PB.22/SPI2_SOUT/CMP2_OUT/LCD_P18}", "102/C11 {PB.23/SPI2_SIN/SPI0_PCS5/LCD_P19}", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA"},
    // PORT C (0..31)
    { "103/B12 {PTC0/ADC0_SE14/TSI0_CH13/SPI0_PCS4/PDB0_EXTRG/I2S0_TXD/LCD_P20}", "104/B11 {PC.1/ADC0_SE15/TSI0_CH14/SPI0_PCS3/UART1_RTS_b/FTM0_CH0/LCD_P21}", "105/A12 {PC.2/ADC0_SE4b/CMP1_IN0/TSI0_CH15/SPI0_PCS2/UART1_CTS_b/FTM0_CH1/LCD_P22}", "106/A11 {PC.3/CMP1_IN1/SPI0_PCS1/UART1_RX/FTM0_CH2/LCD_P23}", "113/A9 {PC.4/SPI0_PCS0/UART1_TX/FTM0_CH3/CMP1_OUT/LCD_P24}", "114/D8 {PC.5/SPI0_SCK/LPT0_ALT2/CMP0_OUT/LCD_P25}", "115/C8 {PC.6/CMP0_IN0/SPI0_SOUT/PDB0_EXTRG/LCD_P26}", "116/B8 {PC.7/CMP0_IN1/SPI0_SIN/LCD_P27}", "117/A8 {PC.8/ADC1_SE4b/CMP0_IN2/I2S0_MCLK/I2S0_CLKIN/LCD_P28}", "118/D7 {PC.9/ADC1_SE5b/CMP0_IN3/I2S0_RX_BCLK/FTM2_FLT0/LCD_P29}", "119/C7 {PC.10/ADC1_SE6b/CMP0_IN4/I2C1_SCL/I2S0_RX_FS/LCD_P30}", "120/B7 {PC.11/ADC1_SE7b/I2C1_SDA/I2S0_RXD/LCD_P31}", "121/A7 {PC.12/UART4_RTS_b/LCD_P32}", "122/D6 {PC.13/UART4_CTS_b/LCD_P33}", "123/C6 {PC.14/UART4_RX/LCD_P34}", "124/B6 {PC.15/UART4_TX/LCD_P35}", "125/A6 {PC.16/UART3_RX/ENET_1588_TMR0/LCD_P36}", "126/D5 {PC.17/UART3_TX/ENET_1588_TMR1/LCD_P37}", "127/C5 {PC.18/UART3_RTS_b/ENET_1588_TMR2/LCD_P38}", "128/B5 {PC.19/UART3_CTS_b/ENET_1588_TMR3/LCD_P39}", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA"},
    // PORT D (0..31)
    { "129/A5 {PTD0/SPI0_PCS0/UART2_RTS_b/LCD_P40}", "130/D4 {PTD1/ADC0_SE5b/SPI0_SCK/UART2_CTS_b/LCD_P41}", "131/C4 {PTD2/SPI0_SOUT/UART2_RX/LCD_P42}", "132/B4 {PTD3/SPI0_SIN/UART2_TX/LCD_P43}", "133/A4 {PTD4/SPI0_PCS1/UART0_RTS_b/FTM0_CH4/EWM_IN/LCD_P44}", "134/A3 {PTD5/ADC0_SE6b/SPI0_PCS2/UART0_CTS_b/FTM0_CH5/EWM_OUT_b/LCD_P45}", "135/A2 {PTD6/ADC0_SE7b/SPI0_PCS3/UART0_RX/FTM0_CH6/FTM0_FLT0/LCD_P46}", "138/A1 {PTD7/CMT_IRO/UART0_TX/FTM0_CH7/FTM0_FLT1/LCD_P47}", "NA", "NA", "139/B3 {PTD10/UART5_RTS_b/FB_A9}", "140/B2 {PTD11/SPI2_PCS0/UART5_CTS_b/SDHC0_CLKIN/FB_A8}", "141/B1 {PTD12/SPI2_SCK/SDHC0_D4/FB_A7}", "142/C3 {PTD13/SPI2_SOUT/SDHC0_D5/FB_A6}", "143/C2 {PTD14/SPI2_SIN/SDHC0_D6/FB_A5}", "144/C1 {PTD15/SPI2_PCS1/SDHC0_D7/FB_RW_b}", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA"},
    // PORT E (0..31)
    { "1/D3 {PTE0/ADC1_SE4a/SPI1_PCS1/UART1_TX/SDHC0_D1/FB_AD27/I2C1_SDA/RTC_CKOUT}", "2/D2 {PTE1/ADC1_SE5a/SPI1_SOUT/UART1_RX/SDHC0_D0/FB_AD26/I2C1_SCL/SPI1_SIN}", "3/D1 {PTE2/ADC1_SE6a/SPI1_SCK/UART1_CTS_b/SDHC0_DCLK/FB_AD25}", "4/E4 {PTE3/ADC1_SE7a/SPI1_SIN/UART1_RTS_b/SDHC0_CMD/FB_AD24/SPI1_SOUT}", "7/E3 {PTE4/SPI1_PCS0/UART3_TX/SDHC0_D3/FB_CS3_b/FB_BE7_0_BLS31_24_b/FB_TA_b}", "8/E2 {PTE5/SPI1_PCS2/UART3_RX/SDHC0_D2/FB_TBST_b/FB_CS2_b/FB_BE15_8_BLS23_16_b}", "9/E1 {PTE6/SPI1_PCS3/UART3_CTS_b/I2S0_MCLK/FB_ALE/FB_CS1_b/FB_TS_b/USB_SOF_OUT}", "10/F4 {PTE7/UART3_RTS_b/I2S0_RXD/FB_CS0_b}", "11/F3 {PTE8/UART5_TX/I2S0_RX_FS/FB_AD4}", "12/F2 {PTE9/UART5_RX/I2S0_RX_BCLK/FB_AD3}", "13/F1 {PTE10/UART5_CTS_b/I2S0_TXD/FB_AD2}", "14/G4 {PTE11/UART5_RTS_b/I2S0_TX_FS/FB_AD1}", "15/G3 {PE.12/I2S0_TX_BCLK/FB_AD0}", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "49/H4 {PTE28/FB_AD20}", "NA", "NA", "NA"}
};


static const char *cPer[PORTS_AVAILABLE][PORT_WIDTH][8] = {              // 144 pin
    {
        // ALT 0           ALT 1    ALT2         ALT 3         ALT 4          ALT 5        ALT 6       ALT 7
        {  "TSI0_CH1",     "PTA0",  "UART0_CTS_b","FTM0_CH5",  "?",           "?",         "?",        "JTAG_TCLK/SWD_CLK" }, // PORT A
        {  "TSI0_CH2",     "PTA1",  "UART0_RX",  "FTM0_CH6",   "?",           "?",         "?",        "JTAG_TDI"          },
        {  "TSI0_CH3",     "PTA2",  "UART0_TX",  "FTM0_CH7",   "?",           "?",         "?",        "JTAG_TDI/TRACE_SWO"},
        {  "TSI0_CH4",     "PTA3",  "UART0_RTS", "FTM0_CH0",   "?",           "?",         "?",        "JTAG_TMS/SWD_DIO"  },
        {  "TSI0_CH5",     "PTA4",  "?",         "FTM0_CH1",   "?",           "?",         "?",        "NMI_b"             },
        {  "D",            "PTA5",  "?",         "FTM0_CH2",   "MII_RXER",    "CMP2_OUT",  "I2S0_RX_BCLK","JTAG_TRST"      },
        {  "D",            "PTA6",  "?",         "FTM0_CH3",   "?",           "?",         "?",        "TRACE_CLKOUT"      },
        {  "ADC0_SE10",    "PTA7",  "?",         "FTM0_CH4",   "?",           "FB_AD18",   "?",        "TRACE_D3"          },
        {  "ADC0_SE11",    "PTA8",  "?",         "FTM1_CH0",   "?",           "FB_AD17",   "FTM1_QD_PHA","TRACE_D2"        },
        {  "D",            "PTA9",  "?",         "FTM1_CH1",   "MII_RXD3",    "FB_AD16",   "FTM1_QD_PHB","TRACE_D1"        },
        {  "D",            "PTA10", "?",         "FTM2_CH0",   "MII_RXD2",    "FB_AD15",   "FTM2_QD_PHA","TRACE_D0"        },
        {  "D",            "PTA11", "?",         "FTM2_CH1",   "MII_RXCLK",   "FB_OE_b",   "FTM2_QD_PHB","?"               },
        {  "CMP2_IN0",     "PTA12", "?",         "FTM1_CH0",   "MII_RXD1",    "FB_CS5/FB_TSIZ1/FB_BE23_16_BLS15_8","I2S0_TXD", "FTM1_QD_PHA" },
        {  "CMP2_IN1",     "PTA13", "?",         "FTM1_CH1",   "MII_RXD0",    "FB_CS4/FB_TSIZ0/FB_BE31_24_BLS7_0", "I2S0_TX_FS","FTM1_QD_PHB"},
        {  "D",            "PTA14", "SPI0_PCS0", "UART0_TX",   "MII_CRS_DV",  "FB_AD31",   "I2S0_TX_BCLK","?"              },
        {  "D",            "PTA15", "SPI0_SCK",  "UART0_RX",   "MII_TXEN",    "FB_AD30",   "I2S0_RXD", "?"                 },
        {  "D",            "PTA16", "SPI0_SOUT", "UART0_CTS",  "MII_TXD0",    "FB_AD29",   "I2S0_RX_FS","?"                },
        {  "ADC1_SE17",    "PTA17", "SPI0_SIN",  "UART0_RTS",  "MII_TXD1",    "FB_AD28",   "I2S0_MCLK","I2S0_CLKIN"        },
        {  "EXTAL",        "PTA18", "?",         "FTM0_FLT2",  "FTM_CLKIN0",  "?",         "?",        "?"                 },
        {  "XTAL",         "PTA19", "?",         "FTM1_FLT0",  "FTM_CLKIN1",  "?",         "LPT0_ALT1","?"                 },
        {  "?",            "?",     "?",         "?",          "?",           "?",         "?",        "?"                 },
        {  "?",            "?",     "?",         "?",          "?",           "?",         "?",        "?"                 },
        {  "?",            "?",     "?",         "?",          "?",           "?",         "?",        "?"                 },
        {  "?",            "?",     "?",         "?",          "?",           "?",         "?",        "?"                 },
        {  "D",            "PTA24", "?",         "?",          "MII_TXD2",    "FB_AD14",   "?",        "?"                 },
        {  "D",            "PTA25", "?",         "?",          "MII_TXCLK",   "FB_AD13",   "?",        "?"                 },
        {  "D",            "PTA26", "?",         "?",          "MII_TXD3",    "FB_AD12",   "?",        "?"                 },                
        {  "D",            "PTA27", "?",         "?",          "MII_CRS",     "FB_AD11",   "?",        "?"                 },
        {  "D",            "PTA28", "?",         "?",          "MII_TXER",    "FB_AD10",   "?",        "?"                 },
        {  "D",            "PTA29", "?",         "?",          "MII_COL",     "FB_AD19",   "?",        "?"                 },
        {  "?",            "?",     "?",         "?",          "?",           "?",         "?",        "?"                 },
        {  "?",            "?",     "?",         "?",          "?",           "?",         "?",        "?"                 }
    },
    {
        // ALT 0           ALT 1    ALT2         ALT 3         ALT 4          ALT 5        ALT 6       ALT 7
        {  "LCD_P0/ADC0_SE8/TSI0_CH0","PTB0","I2C0_SCL","FTM1_CH0", "MII_MDIO",       "?","FTM1_QD_PHA","LCD_P0"           }, // PORT B
        {  "LCD_P1/ADC0_SE9/TSI0_CH6","PTB1","I2C0_SDA","FTM1_CH1", "MII_MCD",        "?","FTM1_QD_PHB","LCD_P1"           },
        {  "LCD_P2/ADC0_SE12/TSI0_CH7","PTB2","I2C0_SCL","UART0_RTS","ENET_1588_TMR0","?","FTM0_FLT3","LCD_P2"             },
        {  "LCD_P3/ADC0_SE13/TSI0_CH8","PTB3","I2C0_SDA","UART0_CTS","ENET_1588_TMR1","?","FTM0_FLT0","LCD_P3"             },
        {  "LCD_P4/ADC1_SE10",  "PTB4",  "?",         "?",          "ENET_1588_TMR2", "?","FTM1_FLT0","LCD_P4"             },
        {  "LCD_P5/ADC1_SE11",  "PTB5",  "?",         "?",          "ENET_1588_TMR3", "?","FTM2_FLT0","LCD_P5"             },
        {  "LCD_P6/ADC1_SE12",  "PTB6",  "?",         "?",          "?",      "?",   "?",             "LCD_P6"             },
        {  "LCD_P7/ADC1_SE13",  "PTB7",  "?",         "?",          "?",      "?",   "?",             "LCD_P7"             },
        {  "LCD_P8",            "PTB8",  "?",         "UART3_RTS",  "?",      "?",   "?",             "LCD_P8"             },
        {  "LCD_P9",            "PTB9",  "SPI1_PCS1", "UART3_CTS",  "?",      "?",   "?",             "LCD_P9"             },
        {  "LCD_P10/ADC1_SE14", "PTB10", "SPI1_PCS0", "UART3_RX",   "?",      "?",   "FTM0_FLT1",     "LCD_P10"            },
        {  "LCD_P11/ADC1_SE15", "PTB11", "SPI1_SCK",  "UART3_TX",   "?",      "?",   "FTM0_FLT2",     "LCD_P11"            },
        {  "?",            "?",     "?",         "?",          "?",           "?",   "?",             "?"                  },
        {  "?",            "?",     "?",         "?",          "?",           "?",   "?",             "?"                  },
        {  "?",            "?",     "?",         "?",          "?",           "?",   "?",             "?"                  },
        {  "?",            "?",     "?",         "?",          "?",           "?",   "?",             "?"                  },
        {  "LCD_P12/TSI0_CH9",  "PTB16", "SPI1_SOUT", "UART0_RX",   "?",      "?",   "EWM_IN",        "LCD_P12"            },
        {  "LCD_P13/TSI0_CH10", "PTB17", "SPI1_SIN",  "UART0_TX",   "?",      "?",   "EWM_OUT_b",     "LCD_P13"            },
        {  "LCD_P14/TSI0_CH11", "PTB18", "?",         "FTM2_CH0",   "I2S0_TX_BCLK",  "?",   "FTM2_QD_PHA","LCD_P14"        },
        {  "LCD_P15/TSI0_CH12", "PTB19", "?",         "FTM2_CH1",   "I2S0_TX_FS",    "?",   "FTM2_QD_PHB","LCD_P15"        },
        {  "LCD_P16",      "PTB20", "SPI2_PCS0", "?",          "?",           "?",   "CMP0_OUT",      "LCD_P16"            },
        {  "LCD_P17",      "PTB21", "SPI2_SCK",  "?",          "?",           "?",   "CMP1_OUT",      "LCD_P17"            },
        {  "LCD_P18",      "PTB22", "SPI2_SOUT", "?",          "?",           "?",   "CMP2_OUT",      "LCD_P18"            },
        {  "LCD_P19",      "PTB23", "SPI2_SIN",  "SPI0_PCS5",  "?",           "?",   "?",             "LCD_P19"            },
        {  "?",            "?",     "?",         "?",          "?",           "?",   "?",             "?"                  },
        {  "?",            "?",     "?",         "?",          "?",           "?",   "?",             "?"                  },
        {  "?",            "?",     "?",         "?",          "?",           "?",   "?",             "?"                  },
        {  "?",            "?",     "?",         "?",          "?",           "?",   "?",             "?"                  },
        {  "?",            "?",     "?",         "?",          "?",           "?",   "?",             "?"                  },
        {  "?",            "?",     "?",         "?",          "?",           "?",   "?",             "?"                  },
        {  "?",            "?",     "?",         "?",          "?",           "?",   "?",             "?"                  },
        {  "?",            "?",     "?",         "?",          "?",           "?",   "?",             "?"                  }
    },
    {
        // ALT 0           ALT 1    ALT2         ALT 3         ALT 4          ALT 5        ALT 6       ALT 7
        {  "LCD_P20/ADC0_SE14/TSI0_CH13","PTC0","SPI0_PCS4","PDB0_EXTRG","I2S0_TXD",  "?", "?",        "LCD_P20"           }, // PORT C
        {  "LCD_P21/ADC0_SE15/TSI0_CH14","PTC1","SPI0_PCS3","UART1_RTS","FTM0_CH0",   "?", "?",        "LCD_P21"           },
        {  "LCD_P22/ADC0_SE4/CMP1_IN0TSI0_CH15","PTC2","SPI0_PCS2","UART1_CTS","FTM0_CH1", "?","?",    "LCD_P22"           },
        {  "LCD_P23/CMP1_IN1",     "PTC3",  "SPI0_PCS1", "UART1_RX",   "FTM0_CH2",    "?", "?",        "LCD_P23"           },
        {  "LCD_P24",      "PTC4",  "SPI0_PCS0", "UART1_TX",   "FTM0_CH3",    "?",    "CMP1_OUT",      "LCD_P24"           },
        {  "LCD_P25",      "PTC5",  "SPI0_SCK",  "?",          "LPT0_ALT2",   "?",    "CMP0_OUT",      "LCD_P25"           },
        {  "LCD_P26/CMP0_IN0",     "PTC6",  "SPI0_SOUT", "PDB0_EXTRG", "?",           "?", "?",        "LCD_P26"           },
        {  "LCD_P27/CMP0_IN1",     "PTC7",  "SPI0_SIN",  "?",          "?",           "?", "?",        "LCD_P27"           },
        {  "LCD_P28/ADC1_SE4/CMP0_IN2","PTC8","?",       "I2S0_MCLK",  "I2S0_CLKIN",  "?", "?",        "LCD_P28"           },
        {  "LCD_P29/ADC1_SE5/CMP0_IN3","PTC9","?",       "?",          "I2S0_RX_BCLK","?", "FTM2_FLT0","LCD_P29"           },
        {  "LCD_P30/ADC1_SE6/CMP0_IN4","PTC10","I2C1_SCL","?",         "I2S0_RX_FS",  "?", "?",        "LCD_P30"           },
        {  "LCD_P31/ADC1_SE7",     "PTC11", "I2C1_SDA",  "?",          "I2S0_RXD",    "?", "?",        "LCD_P31"           },
        {  "LCD_P32",      "PTC12", "?",         "UART4_RTS",  "?",           "?",         "?",        "LCD_P32"           },
        {  "LCD_P33",      "PTC13", "?",         "UART4_CTS",  "?",           "?",         "?",        "LCD_P33"           },
        {  "LCD_P34",      "PTC14", "?",         "UART4_RX",   "?",           "?",         "?",        "LCD_P34"           },
        {  "LCD_P35",      "PTC15", "?",         "UART4_TX",   "?",           "?",         "?",        "LCD_P35"           },
        {  "LCD_P36",      "PTC16", "?",         "UART3_RX",   "ENET_1588_TMR0","?",       "?",        "LCD_P36"           },
        {  "LCD_P37",      "PTC17", "?",         "UART3_TX",   "ENET_1588_TMR1","?",       "?",        "LCD_P37"           },
        {  "LCD_P38",      "PTC18", "?",         "UART3_RTS",  "ENET_1588_TMR2","?",       "?",        "LCD_P38"           },
        {  "LCD_P39",      "PTC19", "?",         "UART3_CTS",  "ENET_1588_TMR3","?",       "?",        "LCD_P39"           },
        {  "?",            "?",     "?",         "?",          "?",           "?",         "?",        "?"                 },
        {  "?",            "?",     "?",         "?",          "?",           "?",         "?",        "?"                 },
        {  "?",            "?",     "?",         "?",          "?",           "?",         "?",        "?"                 },
        {  "?",            "?",     "?",         "?",          "?",           "?",         "?",        "?"                 },
        {  "?",            "?",     "?",         "?",          "?",           "?",         "?",        "?"                 },
        {  "?",            "?",     "?",         "?",          "?",           "?",         "?",        "?"                 },
        {  "?",            "?",     "?",         "?",          "?",           "?",         "?",        "?"                 },
        {  "?",            "?",     "?",         "?",          "?",           "?",         "?",        "?"                 },
        {  "?",            "?",     "?",         "?",          "?",           "?",         "?",        "?"                 },
        {  "?",            "?",     "?",         "?",          "?",           "?",         "?",        "?"                 },
        {  "?",            "?",     "?",         "?",          "?",           "?",         "?",        "?"                 },
        {  "?",            "?",     "?",         "?",          "?",           "?",         "?",        "?"                 }
    },
    {
        // ALT 0           ALT 1    ALT2         ALT 3         ALT 4          ALT 5        ALT 6       ALT 7
        {  "LCD_P40",      "PTD0",  "SPI0_PCS0", "UART2_RTS_b","?",           "?",         "?",        "LCD_P40"           }, // PORT D
        {  "LCD_P41/ADC0_SE5","PTD1","SPI0_SCK", "UART2_CTS_b","?",           "?",         "?",        "LCD_P41"           },
        {  "LCD_P42",      "PTD2",  "SPI0_SOUT", "UART2_RX",   "?",           "?",         "?",        "LCD_P42"           },
        {  "LCD_P43",      "PTD3",  "SPI0_SIN",  "UART2_TX",   "?",           "?",         "?",        "LCD_P43"           },
        {  "LCD_P44",      "PTD4",  "SPI0_PCS1", "UART0_RTS",  "FTM0_CH4",    "?",         "EWM_IN",   "LCD_P44"           },
        {  "LCD_P45/ADC0_SE6","PTD5","SPI0_PCS2","UART0_CTS",  "FTM0_CH5",    "?",         "EWM_OUT_b","LCD_P45"           },
        {  "LCD_P46/ADC0_SE7","PTD6","SPI0_PCS3","UART0_RX",   "FTM0_CH6",    "?",         "FTM0_FLT0","LCD_P46"           },
        {  "LCD_P47",      "PTD7",  "CMT_IRO",   "UART0_TX",   "FTM0_CH7",    "?",         "FTM0_FLT1","LCD_P47"           },
        {  "?",            "?",     "?",         "?",          "?",           "?",         "?",        "?"                 },
        {  "?",            "?",     "?",         "?",          "?",           "?",         "?",        "?"                 },
        {  "D",            "PTD10", "?",         "UART5_RTS",  "?",           "FB_AD9",    "?",        "?"                 },
        {  "D",            "PTD11", "SPI2_PCS0", "UART5_CTS",  "SDHC0_CLKIN", "FB_AD8",    "?",        "?"                 },
        {  "D",            "PTD12", "SPI2_SCK",  "?",          "SDHC0_D4",    "FB_AD7",    "?",        "?"                 },
        {  "D",            "PTD13", "SPI2_SOUT", "?",          "SDHC0_D5",    "FB_AD6",    "?",        "?"                 },
        {  "D",            "PTD14", "SPI2_SIN",  "?",          "SDHC0_D6",    "FB_AD5",    "?",        "?"                 },
        {  "D",            "PTD15", "SPI2_PCS1", "?",          "SDHC0_D7",    "FB_RW_b",   "?",        "?"                 },
        {  "?",            "?",     "?",         "?",          "?",           "?",         "?",        "?"                 },
        {  "?",            "?",     "?",         "?",          "?",           "?",         "?",        "?"                 },
        {  "?",            "?",     "?",         "?",          "?",           "?",         "?",        "?"                 },
        {  "?",            "?",     "?",         "?",          "?",           "?",         "?",        "?"                 },
        {  "?",            "?",     "?",         "?",          "?",           "?",         "?",        "?"                 },
        {  "?",            "?",     "?",         "?",          "?",           "?",         "?",        "?"                 },
        {  "?",            "?",     "?",         "?",          "?",           "?",         "?",        "?"                 },
        {  "?",            "?",     "?",         "?",          "?",           "?",         "?",        "?"                 },
        {  "?",            "?",     "?",         "?",          "?",           "?",         "?",        "?"                 },
        {  "?",            "?",     "?",         "?",          "?",           "?",         "?",        "?"                 },
        {  "?",            "?",     "?",         "?",          "?",           "?",         "?",        "?"                 },
        {  "?",            "?",     "?",         "?",          "?",           "?",         "?",        "?"                 },
        {  "?",            "?",     "?",         "?",          "?",           "?",         "?",        "?"                 },
        {  "?",            "?",     "?",         "?",          "?",           "?",         "?",        "?"                 },
        {  "?",            "?",     "?",         "?",          "?",           "?",         "?",        "?"                 },
        {  "?",            "?",     "?",         "?",          "?",           "?",         "?",        "?"                 }
    },
    {
        // ALT 0           ALT 1    ALT2         ALT 3         ALT 4          ALT 5        ALT 6       ALT 7
        {  "ADC1_SE4a",    "PTE0",  "SPI1_PCS1", "UART1_TX",   "SDHC0_D1",    "FB_AD27",   "I2C1_SDA", "RTC_CKOUT"         }, // PORT E
        {  "ADC1_SE5a",    "PTE1",  "SPI1_SOUT", "UART1_RX",   "SDHC0_D0",    "FB_AD26",   "I2C1_SCL", "SPI1_SIN"          },
        {  "ADC1_SE6a",    "PTE2",  "SPI1_SCK",  "UART1_CTS",  "SDHC0_DCLK",  "FB_AD25",   "?",        "?"                 },
        {  "ADC1_SE7a",    "PTE3",  "SPI1_SIN",  "UART1_RTS",  "SDHC0_CMD",   "FB_AD24",   "?",        "SPI1_SOUT"         },
        {  "D",            "PTE4",  "SPI1_PCS0", "UART3_TX",   "SDHC0_D3",    "FB_CS3_b/FB_BE7_0_BLS31_24_b","FB_TA_b","?" },
        {  "D",            "PTE5",  "SPI1_PCS2", "UART3_RX",   "SDHC0_D2",    "FB_TBST_b/FB_CS2_b/FB_BE15_8_BLS23_16_b","?","?"},
        {  "D",            "PTE6",  "SPI1_PCS3", "UART3_CTS",  "I2S0_MCLK",   "FB_ALE/FB_CS1_b/FB_TS_b","?","USB_SOF_OUT"   },
        {  "D",            "PTE7",  "?",         "UART3_RTS",  "I2S0_RXD",    "FB_CS0_b",  "?",        "?"                 },
        {  "D",            "PTE8",  "?",         "UART5_TX",   "I2S0_RX_FS",  "FB_AD4",    "?",        "?"                 },
        {  "D",            "PTE9",  "?",         "UART5_RX",   "I2S0_RX_BCLK","FB_AD3",    "?",        "?"                 },
        {  "D",            "PTE10", "?",         "UART5_CTS",  "I2S0_TXD",    "FB_AD2",    "?",        "?"                 },
        {  "D",            "PTE11", "?",         "UART5_RTS",  "I2S0_TX_FS",  "FB_AD1",    "?",        "?"                 },
        {  "D",            "PTE12", "?",         "?",          "I2S0_TX_BCLK","FB_AD0",    "?",        "?"                 },
        {  "?",            "?",     "?",         "?",          "?",           "?",         "?",        "?"                 },
        {  "?",            "?",     "?",         "?",          "?",           "?",         "?",        "?"                 },
        {  "?",            "?",     "?",         "?",          "?",           "?",         "?",        "?"                 },
        {  "?",            "?",     "?",         "?",          "?",           "?",         "?",        "?"                 },
        {  "?",            "?",     "?",         "?",          "?",           "?",         "?",        "?"                 },
        {  "?",            "?",     "?",         "?",          "?",           "?",         "?",        "?"                 },
        {  "?",            "?",     "?",         "?",          "?",           "?",         "?",        "?"                 },
        {  "?",            "?",     "?",         "?",          "?",           "?",         "?",        "?"                 },
        {  "?",            "?",     "?",         "?",          "?",           "?",         "?",        "?"                 },
        {  "?",            "?",     "?",         "?",          "?",           "?",         "?",        "?"                 },
        {  "?",            "?",     "?",         "?",          "?",           "?",         "?",        "?"                 },
        {  "?",            "?",     "?",         "?",          "?",           "?",         "?",        "?"                 },
        {  "?",            "?",     "?",         "?",          "?",           "?",         "?",        "?"                 },
        {  "?",            "?",     "?",         "?",          "?",           "?",         "?",        "?"                 },
        {  "?",            "?",     "?",         "?",          "?",           "?",         "?",        "?"                 },
        {  "D",            "PTE28", "?",         "?",          "?",           "FB_AD20",   "?",        "?"                 },
        {  "?",            "?",     "?",         "?",          "?",           "?",         "?",        "?"                 },
        {  "?",            "?",     "?",         "?",          "?",           "?",         "?",        "?"                 },
        {  "?",            "?",     "?",         "?",          "?",           "?",         "?",        "?"                 }
    }
};
