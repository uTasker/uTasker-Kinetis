/***********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher

    ---------------------------------------------------------------------
    File:      kinetis_port_k52.h
    Project:   Single Chip Embedded Internet
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2016
    *********************************************************************
    
*/

static const char *cPinNumber[PORTS_AVAILABLE][PORT_WIDTH] = {           // 144 pin
    // PORT A (0..31)
    { "50/J5 {PA.0/TSI0_CH1/UART0_CTS_b/FTM0_CH5/JTAG_TCLK/SWD_CLK/EZP_CLK}", "51/J6 {PA.1/TSI0_CH2/UART0_RX/FTM0_CH6/JTAG_TDI/EZP_DI}", "52/K6 {PA.2/TSI0_CH3/UART0_TX/FTM0_CH7/JTAG_TDO/TRACE_SWO/EZP_DO}", "53/K7 {PA.3/TSI0_CH4/UART0_RTS_b/FTM0_CH0/JTAG_TMS/SWD_DIO}", "54/L7 {PA.4/TSI0_CH5/FTM0_CH1/NMI_b/EZP_CS_b}", "55/M8 {PA.5/FTM0_CH2/MII0_RXER/CMP2_OUT/I2S0_RX_BCLK/JTAG_TRST}", "58/J7 {PA.6/FTM0_CH3/TRACE_CLKOUT}", "59/J8 {PA.7/ADC0_SE10/FTM0_CH4/TRACE_D3}", "60/K8 {PA.8/ADC0_SE11/FTM1_CH0/FTM1_QD_PHA/TRACE_D2}", "61/L8 {PA.9/FTM1_CH1/MII0_RXD3/FTM1_QD_PHB/TRACE_D1}", "62/M9 {PA.10/FTM2_CH0/MII0_RXD2/FTM2_QD_PHA/TRACE_D0}", "63/L9 {PA.11/FTM2_CH1/MII0_RXCLK/FTM2_QD_PHB}", "64/K9 {PA.12/CMP2_IN0/FTM1_CH0/MII0_RXD1/I2S0_TXD/FTM1_QD_PHA}", "65/J9 {PA.13/CMP2_IN1/FTM1_CH1/MII0_RXD0/I2S0_TX_FS/FTM1_QD_PHB}", "66/L10 {PA.14/SPI0_PCS0/UART0_TX/RMII0_CRS_DV/MII0_RXDV/I2S0_TX_BCLK}", "67/L11 {PA.15/SPI0_SCK/UART0_RX/MII0_TXEN/I2S0_RXD}", "68/K10 {PA.16/SPI0_SOUT/UART0_CTS_b/MII0_TXD0/I2S0_RX_FS}", "69/K11 {PA.17/ADC1_SE17/SPI0_SIN/UART0_RTS_b/MII0_TXD1/I2S0_MCLK/ISS0_CLKIN}", "72/M12 {PA.18/EXTAL/FTM0_FLT2/FTM_CLKIN0}", "73/M11 {PA.19/XTAL/FTM1_FLT0/FTM_CLKIN1/LPT0_ALT1}", "NA", "NA", "NA", "NA", "75/K12 {PA.24/MII_TXD2/FB_A29}", "76/J12 {PA.25/MII_TXCLK/FB_A28}", "77/J11 {PA.26/MII_TXD3/FB_A27}", "78/J10 {PA.27/MII_CRS/FB_A26}", "79/H12 {PA.28/MII_TXER/FB_A25}", "80/H11 {PA.29/MII_COL/FB_A24}", "NA", "NA"},
    // PORT B (0..31)
    { "81/H10 {PB.0/ADC0_SE8/ADC1_SE8/TSI0_CH0/I2C0_SCL/FTM1_CH0/MII0_MDIO/FTM1_QD_PHA}", "82/H9 {PB.1/ADC0_SE9/ADC1_SE9/TSI0_CH6/I2C0_SDA/FTM1_CH1/MII0_MDC/FTM1_QD_PHB}", "83/G12 {PB.2/ADC0_SE12/TSI0_CH7/I2C0_SCL/UART0_RTS_b/ENET0_1588_TMR0/FTM0_FLT3}", "84/G11 {PB.3/ADC0_SE13/TSI0_CH8/I2C0_SDA/UART0_CTS_b/ENET0_1588_TMR1/FTM0_FLT0}", "85/G10 {PB.4/ADC1_SE10/ENET0_1588_TMR2/FTM1_FLT0}", "86/G9 {PB.5/ADC1_SE11/ENET0_1588_TMR3/FTM2_FLT0}", "87/F12 {PB.6/ADC1_SE12/FB_AD23}", "88/F11 {PB.7/ADC1_SE13/FB_AD22}", "89/F10 {PB.8/UART3_RTS_b/FB_AD21}", "90/F9 {PB.9/SPI1_PCS1/UART3_CTS_b/FB_AD20}", "91/E12 {PB.10/ADC1_SE14/SPI1_PCS0/UART3_RX/FB_AD19/FTM0_FLT1}", "92/E11 {PB.11/ADC1_SE15/SPI1_SCK/UART3_TX/FB_AD18/FTM0_FLT2}", "NA", "NA", "NA", "NA", "95/E10 {PB.16/TSI0_CH9/SPI1_SOUT/UART0_RX/FB_AD17/EWM_IN}", "96/E9 {PB.17/TSI0_CH10/SPI1_SIN/UART0_TX/FB_AD16/EWM_OUT_b}", "97/D12 {PB.18/TSI0_CH11/FTM2_CH0/I2S0_TX_BCLK/FB_AD15/FTM2_QD_PHA}", "98/D11 {PB.19/TSI0_CH12/FTM2_CH1/I2S0_TX_FS/FB_OE_b/FTM2_QD_PHB}", "99/D10 {PB.20/SPI2_PCS0/FB_AD31/CMP0_OUT}", "100/D9 {PB.21/SPI2_SCK/FB_AD30/CMP1_OUT}", "101/C12 {PB.22/SPI2_SOUT/FB_AD29/CMP2_OUT}", "102/C11 {PB.23/SPI2_SIN/SPI0_PCS5/FB_AD28}", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA"},
    // PORT C (0..31)
    { "103/B12 {PC.0/ADC0_SE14/TSI0_CH13/SPI0_PCS4/PDB0_EXTRG/I2S0_TXD/FB_AD14}", "104/B11 {PC.1/ADC0_SE15/TSI0_CH14/SPI0_PCS3/UART1_RTS_b/FTM0_CH0/FB_AD13}", "105/A12 {PC.2/ADC0_SE4b/CMP1_IN0/TSI0_CH15/SPI0_PCS2/UART1_CTS_b/FTM0_CH1/FB_AD12}", "106/A11 {PC.3/CMP1_IN1/SPI0_PCS1/UART1_RX/FTM0_CH2/FB_CLKOUT}", "109/A9 {PC.4/SPI0_PCS0/UART1_TX/FTM0_CH3/FB_AD11/CMP1_OUT}", "110/D8 {PC.5/SPI0_SCK/LPT0_ALT2/FB_AD10/CMP0_OUT}", "111/C8 {PC.6/CMP0_IN0/SPI0_SOUT/PDB0_EXTRG/FB_AD9}", "112/B8 {PC.7/CMP0_IN1/SPI0_SIN/FB_AD8}", "113/A8 {PC.8/ADC1_SE4b/CMP0_IN2/I2S0_MCLK/I2S0_CLKIN/FB_AD7}", "114/D7 {PC.9/ADC1_SE5b/CMP0_IN3/I2S0_RX_BCLK/FB_AD6/FTM2_FLT0}", "115/C7 {PC.10/ADC1_SE6b/CMP0_IN4/I2C1_SCL/I2S0_RX_FS/FB_AD5}", "116/B7 {PC.11/ADC1_SE7b/I2C1_SDA/I2S0_RXD/FB_RW_b}", "117/A7 {PC.12/UART4_RTS_b/FB_AD27}", "118/D6 {PC.13/UART4_CTS_b/FB_AD26}", "119/C6 {PC.14/UART4_RX/FB_AD25}", "120/B6 {PC.15/UART4_TX/FB_AD24}", "123/A6 {PC.16/UART3_RX/ENET0_1588_TMR0/FB_CS5_b/FB_TSIZ1/FB_BE23_16_BLS15_8_b}", "124/D5 {PC.17/UART3_TX/ENET0_1588_TMR1/FB_CS4_b/FB_TSIZ0/FB_BE31_24_BLS7_0_b}", "125/C5 {PC.18/UART3_RTS_b/ENET0_1588_TMR2/FB_TBST_b/FB_CS2_b/FB_BE15_8_BLS23_16_b}", "126/B5 {PC.19/UART3_CTS_b/ENET0_1588_TMR3/FB_CS3_b/FB_BE7_0_BLS31_24_b/FB_TA_b}", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA"},
    // PORT D (0..31)
    { "127/A5 {PD.0/SPI0_PCS0/UART2_RTS_b/FB_ALE/FB_CS1_b/FB_TS_b}", "128/D4 {PD.1/ADC0_SE5b/SPI0_SCK/UART2_CTS_b/FB_CS0_b}", "129/C4 {PD.2/SPI0_SOUT/UART2_RX/FB_AD4}", "130/B4 {PD.3/SPI0_SIN/UART2_TX/FB_AD3}", "131/A4 {PD.4/SPI0_PCS1/UART0_RTS_b/FTM0_CH4/FB_AD2/EWM_IN}", "132/A3 {PD.5/ADC0_SE6b/SPI0_PCS2/UART0_CTS_b/FTM0_CH5/FB_AD1/EWM_OUT_b}", "133/A2 {PD.6/ADC0_SE7b/SPI0_PCS3/UART0_RX/FTM0_CH6/FB_AD0/FTM0_FLT0}", "136/A1 {PD.7/CMT_IRO/UART0_TX/FTM0_CH7/FTM0_FLT1}", "137/C9 {PD.8/I2C0_SCL/UART5_RX/FB_A16}", "138/B9 {PD.9/I2C0_SDA/UART5_TX/FB_A17}", "139/B3 {PD.10/UART5_RTS_b/FB_A18}", "140/B2 {PD.11/SPI2_PCS0/UART5_CTS_b/SDHC0_CLKIN/FB_A19}", "141/B1 {PD.12/SPI2_SCK/SDHC0_D4/FB_A20}", "142/C3 {PD.13/SPI2_SOUT/SDHC0_D5/FB_A21}", "143/C2 {PD.14/SPI2_SIN/SDHC0_D6/FB_A22}", "144/C1 {PD.15/SPI2_PCS1/SDHC0_D7/FB_A23}", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA"},
    // PORT E (0..31)
    { "1/D3 {PE.0/ADC1_SE4a/SPI1_PCS1/UART1_TX/SDHC0_D1/I2C1_SDA}", "2/D2 {PE.1/ADC1_SE5a/SPI1_SOUT/UART1_RX/SDHC0_D0/I2C1_SCL}", "3/D1 {PE.2/ADC1_SE6a/SPI1_SCK/UART1_CTS_b/SDHC0_DCLK}", "4/E4 {PE.3/ADC1_SE7a/SPI1_SIN/UART1_RTS_b/SDHC0_CMD}", "7/E3 {PE.4/SPI1_PCS0/UART3_TX/SDHC0_D3}", "8/E2 {PE.5/SPI1_PCS2/UART3_RX/SDHC0_D2}", "9/E1 {PE.6/SPI1_PCS3/UART3_CTS_b/I2S0_MCLK/I2S0_CLKIN}", "10/F4 {PE.7/UART3_RTS_b/I2S0_RXD}", "11/F3 {PE.8/UART5_TX/I2S0_RX_FS}", "12/F2 {PE.9/UART5_RX/I2S0_RX_BCLK}", "13/F1 {PE.10/UART5_CTS_b/I2S0_TXD}", "14/G4 {PE.11/UART5_RTS_b/I2S0_TX_FS}", "15/G3 {PE.12/I2S0_TX_BCLK}", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "49/H4 {PE.28}", "NA", "NA", "NA"}
};

static const char *cPer[PORTS_AVAILABLE][PORT_WIDTH][8] = {              // 144 pin
    {
        // ALT 0           ALT 1    ALT2         ALT 3         ALT 4          ALT 5        ALT 6       ALT 7
        {  "TSI0_CH1",     "PTA0",  "UART0_CTS", "FTM0_CH5",   "?",           "?",         "?",        "JTAG_TCLK/SWD_CLK" }, // PORT A
        {  "TSI0_CH2",     "PTA1",  "UART0_RX",  "FTM0_CH6",   "?",           "?",         "?",        "JTAG_TDI"          },
        {  "TSI0_CH3",     "PTA2",  "UART0_TX",  "FTM0_CH7",   "?",           "?",         "?",        "JTAG_TDI/TRACE_SWO"},
        {  "TSI0_CH4",     "PTA3",  "UART0_RTS", "FTM0_CH0",   "?",           "?",         "?",        "JTAG_TMS/SWD_DIO"  },
        {  "TSI0_CH5",     "PTA4",  "?",         "FTM0_CH1",   "?",           "?",         "?",        "NMI"               },
        {  "?",            "PTA5",  "?",         "FTM0_CH2",   "MII_RXER",    "CMP2_OUT",  "I2S0_RX_BCLK","JTAG_TRST"      },
        {  "?",            "PTA6",  "?",         "FTM0_CH3",   "?",           "?",         "?",        "TRACE_CLKOUT"      },
        {  "ADC0_SE10",    "PTA7",  "?",         "FTM0_CH4",   "?",           "?",         "?",        "TRACE_D3"          },
        {  "ADC0_SE11",    "PTA8",  "?",         "FTM1_CH0",   "?",           "?",         "FTM1_QD_PHA","TRACE_D2"        },
        {  "?",            "PTA9",  "?",         "FTM1_CH1",   "MII_RXD3",    "?",         "FTM1_QD_PHB","TRACE_D1"        },
        {  "?",            "PTA10", "?",         "FTM2_CH0",   "MII_RXD2",    "?",         "FTM2_QD_PHA","TRACE_D0"        },
        {  "?",            "PTA11", "?",         "FTM2_CH1",   "MII_RXCLK",   "?",         "FTM2_QD_PHB","?"               },
        {  "CMP2_IN0",     "PTA12", "?",         "FTM1_CH0",   "MII_RXD1",    "?",         "I2S0_TXD", "FTM1_QD_PHA"       },
        {  "CMP2_IN1",     "PTA13", "?",         "FTM1_CH1",   "MII_RXD0",    "?",         "I2S0_TX_FS","FTM1_QD_PHB"      },
        {  "?",            "PTA14", "SPI0_PCS0", "UART0_TX",   "MII_CRS_DV",  "?",         "I2S0_TX_BCLK","?"              },
        {  "?",            "PTA15", "SPI0_SCK",  "UART0_RX",   "MII_TXEN",    "?",         "I2S0_RXD", "?"                 },
        {  "?",            "PTA16", "SPI0_SOUT", "UART0_CTS",  "MII_TXD0",    "?",         "I2S0_RX_FS","?"                },
        {  "ADC1_SE17",    "PTA17", "SPI0_SIN",  "UART0_RTS",  "MII_TXD1",    "?",         "I2S0_MCLK","I2S0_CLKIN"        },
        {  "EXTAL",        "PTA18", "?",         "FTM0_FLT2",  "FTM_CLKIN0",  "?",         "?",        "?"                 },
        {  "XTAL",         "PTA19", "?",         "FTM1_FLT0",  "FTM_CLKIN1",  "?",         "LPT0_ALT1","?"                 },
        {  "?",            "?",     "?",         "?",          "?",           "?",         "?",        "?"                 },
        {  "?",            "?",     "?",         "?",          "?",           "?",         "?",        "?"                 },
        {  "?",            "?",     "?",         "?",          "?",           "?",         "?",        "?"                 },
        {  "?",            "?",     "?",         "?",          "?",           "?",         "?",        "?"                 },
        {  "?",            "PTA24", "?",         "?",          "MII_TXD2",    "?",         "FB_A29",   "?"                 },
        {  "?",            "PTA25", "?",         "?",          "MII_TXCLK",   "?",         "FB_A28",   "?"                 },
        {  "?",            "PTA26", "?",         "?",          "MII_TXD3",    "?",         "FB_A27",   "?"                 },                
        {  "?",            "PTA27", "?",         "?",          "MII_CRS",     "?",         "FB_A26",   "?"                 },
        {  "?",            "PTA28", "?",         "?",          "MII_TXER",    "?",         "FB_A25",   "?"                 },
        {  "?",            "PTA29", "?",         "?",          "MII_COL",     "?",         "FB_A24",   "?"                 },
        {  "?",            "?",     "?",         "?",          "?",           "?",         "?",        "?"                 },
        {  "?",            "?",     "?",         "?",          "?",           "?",         "?",        "?"                 }
    },
    {
        // ALT 0           ALT 1    ALT2         ALT 3         ALT 4          ALT 5        ALT 6       ALT 7
        {  "ADC0_SE8/TSI0_CH0","PTB0","I2C0_SCL","FTM1_CH0",   "MII_MDIO",    "?",         "FTM1_QD_PHA", "?"              }, // PORT B
        {  "ADC0_SE9/TSI0_CH6","PTB1","I2C0_SDA","FTM1_CH1",   "MII_MCD",     "?",         "FTM1_QD_PHB", "?"              },
        {  "ADC0_SE12/TSI0_CH7","PTB2","I2C0_SCL","UART0_RTS", "ENET_1588_TMR0","?",       "FTM0_FLT3","?"                 },
        {  "ADC0_SE13/TSI0_CH8","PTB3","I2C0_SDA","UART0_CTS", "ENET_1588_TMR1","?",       "FTM0_FLT0","?"                 },
        {  "ADC1_SE10",    "PTB4",  "?",         "?",          "ENET_1588_TMR2","?",       "FTM1_FLT0","?"                 },
        {  "ADC1_SE11",    "PTB5",  "?",         "?",          "ENET_1588_TMR3","?",       "FTM2_FLT0","?"                 },
        {  "ADC1_SE12",    "PTB6",  "?",         "?",          "?",           "FB_AD23",   "?",        "?"                 },
        {  "ADC1_SE13",    "PTB7",  "?",         "?",          "?",           "FB_AD22",   "?",        "?"                 },
        {  "?",            "PTB8",  "?",         "UART3_RTS",  "?",           "FB_AD21",   "?",        "?"                 },
        {  "?",            "PTB9",  "SPI1_PCS1", "UART3_CTS",  "?",           "FB_AD20",   "?",        "?"                 },
        {  "ADC1_SE14",    "PTB10", "SPI1_PCS0", "UART3_RX",   "?",           "FB_AD19",   "FTM0_FLT1","?"                 },
        {  "ADC1_SE15",    "PTB11", "SPI1_SCK",  "UART3_TX",   "?",           "FB_AD18",   "FTM0_FLT2","?"                 },
        {  "?",            "?",     "?",         "?",          "?",           "?",         "?",        "?"                 },
        {  "?",            "?",     "?",         "?",          "?",           "?",         "?",        "?"                 },
        {  "?",            "?",     "?",         "?",          "?",           "?",         "?",        "?"                 },
        {  "?",            "?",     "?",         "?",          "?",           "?",         "?",        "?"                 },
        {  "TSI0_CH9",     "PTB16", "SPI1_SOUT", "UART0_RX",   "?",           "FB_AD17",   "EWM_IN",   "?"                 },
        {  "TSI0_CH10",    "PTB17", "SPI1_SIN",  "UART0_TX",   "?",           "FB_AD16",   "EWM_OUT",  "?"                 },
        {  "TSI0_CH11",    "PTB18", "?",         "FTM2_CH0",   "I2S0_TX_BCLK","FB_AD15",   "FTM2_QD_PHA","?"               },
        {  "TSI0_CH12",    "PTB19", "?",         "FTM2_CH1",   "I2S0_TX_FS",  "FB_OE",     "FTM2_QD_PHB","?"               },
        {  "?",            "PTB20", "SPI2_PCS0", "?",          "?",           "FB_AD31",   "CMP0_OUT", "?"                 },
        {  "?",            "PTB21", "SPI2_SCK",  "?",          "?",           "FB_AD30",   "CMP1_OUT", "?"                 },
        {  "?",            "PTB22", "SPI2_SOUT", "?",          "?",           "FB_AD29",   "CMP2_OUT", "?"                 },
        {  "?",            "PTB23", "SPI2_SIN",  "SPI0_PCS5",  "?",           "FB_AD28",   "?",        "?"                 },
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
        {  "ADC0_SE14/TSI0_CH13","PTC0","SPI0_PCS4","PDB0_EXTRG","I2S0_TXD",  "FB_AD14",   "?",        "?"                 }, // PORT C
        {  "ADC0_SE15/TSI0_CH14","PTC1","SPI0_PCS3","UART1_RTS","FTM0_CH0",   "FB_AD13",   "?",        "?"                 },
        {  "ADC0_SE4/CMP1_IN0TSI0_CH15","PTC2","SPI0_PCS2","UART1_CTS","FTM0_CH1","FB_AD12","?",       "?"                 },
        {  "CMP1_IN1",     "PTC3",  "SPI0_PCS1", "UART1_RX",   "FTM0_CH2",    "FB_CLKOUT", "?",        "?"                 },
        {  "?",            "PTC4",  "SPI0_PCS0", "UART1_TX",   "FTM0_CH3",    "FB_AD11",   "CMP1_OUT", "?"                 },
        {  "?",            "PTC5",  "SPI0_SCK",  "?",          "LPT0_ALT2",   "FB_AD10",   "CMP0_OUT", "?"                 },
        {  "CMP0_IN0",     "PTC6",  "SPI0_SOUT", "PDB0_EXTRG", "?",           "FB_AD9",    "?",        "?"                 },
        {  "CMP0_IN1",     "PTC7",  "SPI0_SIN",  "?",          "?",           "FB_AD8",    "?",        "?"                 },
        {  "ADC1_SE4/CMP0_IN2","PTC8","?",       "I2S0_MCLK",  "I2S0_CLKIN",  "FB_AD7",    "?",        "?"                 },
        {  "ADC1_SE5/CMP0_IN3","PTC9","?",       "?",          "I2S0_RX_BCLK","FB_AD6",    "FTM2_FLT0","?"                 },
        {  "ADC1_SE6/CMP0_IN4","PTC10","I2C1_SCL","?",         "I2S0_RX_FS",  "FB_AD5",    "?",        "?"                 },
        {  "ADC1_SE7",     "PTC11", "I2C1_SDA",  "?",          "I2S0_RXD",    "FB_RW",     "?",        "?"                 },
        {  "?",            "PTC12", "?",         "UART4_RTS",  "?",           "FB_AD27",   "?",        "?"                 },
        {  "?",            "PTC13", "?",         "UART4_CTS",  "?",           "FB_AD26",   "?",        "?"                 },
        {  "?",            "PTC14", "?",         "UART4_RX",   "?",           "FB_AD25",   "?",        "?"                 },
        {  "?",            "PTC15", "?",         "UART4_TX",   "?",           "FB_AD24",   "?",        "?"                 },
        {  "?",            "PTC16", "?",         "UART3_RX",   "ENET_1588_TMR0","FB_CS5/FB_TSIZ1/FB_BE23_16_BLS15_8","?","?"},
        {  "?",            "PTC17", "?",         "UART3_TX",   "ENET_1588_TMR1","FB_CS4/FB_TSIZ0/FB_BE31_24_BLS7_0","?","?"},
        {  "?",            "PTC18", "?",         "UART3_RTS",  "ENET_1588_TMR2","FB_TBST/FB_CS2/FB_BE15_8_BLS23_16","?","?"},
        {  "?",            "PTC19", "?",         "UART3_CTS",  "ENET_1588_TMR3","FB_CS3/FB_BE7_0_BLS31_24","FB_TA","?"     },
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
        {  "?",            "PTD0",  "SPI0_PCS0", "UART2_RTS",  "?",           "FB_ALE/FB_CS1/FB_TS","?","?"                }, // PORT D
        {  "ADC0_SE5",     "PTD1",  "SPI0_SCK",  "UART2_CTS",  "?",           "FB_CS0",    "?",        "?"                 },
        {  "?",            "PTD2",  "SPI0_SOUT", "UART2_RX",   "?",           "FB_AD4",    "?",        "?"                 },
        {  "?",            "PTD3",  "SPI0_SIN",  "UART2_TX",   "?",           "FB_AD3",    "?",        "?"                 },
        {  "?",            "PTD4",  "SPI0_PCS1", "UART0_RTS",  "FTM0_CH4",    "FB_AD2",    "EWM_IN",   "?"                 },
        {  "ADC0_SE6",     "PTD5",  "SPI0_PCS2", "UART0_CTS",  "FTM0_CH5",    "FB_AD1",    "EWM_OUT",  "?"                 },
        {  "ADC0_SE7",     "PTD6",  "SPI0_PCS3", "UART0_RX",   "FTM0_CH6",    "FB_AD0",    "FTM0_FLT0","?"                 },
        {  "?",            "PTD7",  "CMT_IRO",   "UART0_TX",   "FTM0_CH7",    "?",         "FTM0_FLT1","?"                 },
        {  "?",            "PTD8",  "I2C0_SCL",  "UART5_RX",   "?",           "?",         "FB_A16",   "?"                 },
        {  "?",            "PTD9",  "I2C0_SDA",  "UART5_TX",   "?",           "?",         "FB_A17",   "?"                 },
        {  "?",            "PTD10", "?",         "UART5_RTS"   "?",           "?",         "FB_A18",   "?"                 },
        {  "?",            "PTD11", "SPI2_PCS0", "UART5_CTS"   "SDHC0_CLKIN", "?",         "FB_A19",   "?"                 },
        {  "?",            "PTD12", "SPI2_SCK",  "?"           "SDHC0_D4",    "?",         "FB_A20",   "?"                 },
        {  "?",            "PTD13", "SPI2_SOUT", "?"           "SDHC0_D5",    "?",         "FB_A21",   "?"                 },
        {  "?",            "PTD14", "SPI2_SIN",  "?"           "SDHC0_D6",    "?",         "FB_A22",   "?"                 },
        {  "?",            "PTD15", "SPI2_PCS1", "?"           "SDHC0_D7",    "?",         "FB_A23",   "?"                 },
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
        {  "ADC1_SE4a",    "PTE0",  "SPI1_PCS1", "UART1_TX",   "SDHC0_D1",    "?",         "I2C1_SDA", "?"                 }, // PORT E
        {  "ADC1_SE5a",    "PTE1",  "SPI1_SOUT", "UART1_RX",   "SDHC0_D0",    "?",         "I2C1_SCL", "?"                 },
        {  "ADC1_SE6a",    "PTE2",  "SPI1_SCK",  "UART1_CTS",  "SDHC0_DCLK",  "?",         "?",        "?"                 },
        {  "ADC1_SE7a",    "PTE3",  "SPI1_SIN",  "UART1_RTS",  "SDHC0_CMD",   "?",         "?",        "?"                 },
        {  "?",            "PTE4",  "SPI1_PCS0", "UART3_TX",   "SDHC0_D3",    "?",         "?",        "?"                 },
        {  "?",            "PTE5",  "SPI1_PCS2", "UART3_RX",   "SDHC0_D2",    "?",         "?",        "?"                 },
        {  "?",            "PTE6",  "SPI1_PCS3", "UART3_CTS",  "I2S0_MCLK",   "?",         "I2S0_CLKIN","?"                },
        {  "?",            "PTE7",  "?",         "UART3_RTS",  "I2S0_RXD",    "?",         "?",        "?"                 },
        {  "?",            "PTE8",  "?",         "UART5_TX",   "I2S0_RX_FS",  "?",         "?",        "?"                 },
        {  "?",            "PTE9",  "?",         "UART5_RX",   "I2S0_RX_BCLK","?",         "?",        "?"                 },
        {  "?",            "PTE10", "?",         "UART5_CTS",  "I2S0_TXD",    "?",         "?",        "?"                 },
        {  "?",            "PTE11", "?",         "UART5_RTS",  "I2S0_TX_FS",  "?",         "?",        "?"                 },
        {  "?",            "PTE12", "?",         "?",          "I2S0_TX_BCLK","?",         "?",        "?"                 },
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
        {  "?",            "PTE28", "?",         "?",          "?",           "?",         "?",        "?"                 },
        {  "?",            "?",     "?",         "?",          "?",           "?",         "?",        "?"                 },
        {  "?",            "?",     "?",         "?",          "?",           "?",         "?",        "?"                 },
        {  "?",            "?",     "?",         "?",          "?",           "?",         "?",        "?"                 }
    }
};

