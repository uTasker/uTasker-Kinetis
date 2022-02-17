/***********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher

    ---------------------------------------------------------------------
    File:      kinetis_port_k40.h
    Project:   Single Chip Embedded Internet
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2016
    *********************************************************************
    
*/

#if defined DEVICE_80_PIN
static const char *cPinNumber[PORTS_AVAILABLE][PORT_WIDTH] = {           // 81 pin MAPGBA / 80 pin LQPF
    // PORT A (0..31)
    { "26/J6 {PA.0/TSI0_CH1/UART0_CTS_b/FTM0_CH5/JTAG_TCLK/SWD_CLK/EZP_CLK}", "27/H8 {PA.1/TSI0_CH2/UART0_RX/FTM0_CH6/JTAG_TDI/EZP_DI}", "28/J7 {PA.2/TSI0_CH3/UART0_TX/FTM0_CH7/JTAG_TDO/TRACE_SWO/EZP_DO}", "29/H9 {PA.3/TSI0_CH4/UART0_RTS_b/FTM0_CH0/JTAG_TMS/SWD_DIO}", "30/J8 {PA.4/TSI0_CH5/FTM0_CH1/NMI_b/EZP_CS_b}", "31/K7 {PA.5/FTM0_CH2/CMP2_OUT/I2S0_RX_BCLK/JTAG_TRST}", "NA", "NA", "NA", "NA", "NA", "NA", "32/K8 {PA.12/CMP2_IN0/CAN0_TX/FTM1_CH0/I2S0_TXD/FTM1_QD_PHA}", "33/L8 {PA.13/CMP2_IN1/CAN0_RX/FTM1_CH1/I2S0_TX_FS/FTM1_QD_PHB}", "34/K9 {PA.14/SPI0_PCS0/UART0_TX/I2S0_TX_BCLK}", "35/L9 {PA.15/SPI0_SCK/UART0_RX/I2S0_RXD}", "36/J10 {PA.16/SPI0_SOUT/UART0_CTS_b/I2S0_RX_FS}", "37/H10 {PA.17/ADC1_SE17/SPI0_SIN/UART0_RTS_b/I2S0_MCLK/ISS0_CLKIN}", "40/L11 {PA.18/EXTAL/FTM0_FLT2/FTM_CLKIN0}", "41/K11 {PA.19/XTAL/FTM1_FLT0/FTM_CLKIN1/LPT0_ALT1}", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA"},
    // PORT B (0..31)
    { "43/G11 {PB.0/ADC0_SE8/ADC1_SE8/TSI0_CH0/I2C0_SCL/FTM1_CH0/FTM1_QD_PHA/LCD_P0}", "44/G10 {PB.1/ADC0_SE9/ADC1_SE9/TSI0_CH6/I2C0_SDA/FTM1_CH1/FTM1_QD_PHB/LCD_P1}", "45/G9 {PB.2/ADC0_SE12/TSI0_CH7/I2C0_SCL/UART0_RTS_b/FTM0_FLT3/LCD_P2}", "46/G8 {PB.3/ADC0_SE13/TSI0_CH8/I2C0_SDA/UART0_CTS_b/FTM0_FLT0/LCD_P3}", "NA", "NA", "NA", "NA", "47/D11 {PB.8/UART3_RTS_b//LCD_P8}", "48/E10 {PB.9/SPI1_PCS1/UART3_CTS_b/LCD_P9}", "49/D10 {PB.10/ADC1_SE14/SPI1_PCS0/UART3_RX/FTM0_FLT1/LCD_P10}", "50/C10 {PB.11/ADC1_SE15/SPI1_SCK/UART3_TX/FTM0_FLT2/LCD_P11}", "NA", "NA", "NA", "NA", "51/B10 {PB.16/TSI0_CH9/SPI1_SOUT/UART0_RX/EWM_IN/LCD_P12}", "52/E9 {PB.17/TSI0_CH10/SPI1_SIN/UART0_TX/EWM_OUT_b/LCD_P13}", "53/D9 {PB.18/TSI0_CH11/CAN0_TX/FTM2_CH0/I2S0_TX_BCLK/FTM2_QD_PHA/LCD_P14}", "54/C9 {PB.19/TSI0_CH12/CAN0_RX/FTM2_CH1/I2S0_TX_FS/FTM2_QD_PHB/LCD_P15}", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA"},
    // PORT C (0..31)
    { "55/B9 {PC.0/ADC0_SE14/TSI0_CH13/SPI0_PCS4/PDB0_EXTRG/I2S0_TXD/LCD_P20}", "56/D8 {PC.1/ADC0_SE15/TSI0_CH14/SPI0_PCS3/UART1_RTS_b/FTM0_CH0/LCD_P21}", "57/C8 {PC.2/ADC0_SE4b/CMP1_IN0/TSI0_CH15/SPI0_PCS2/UART1_CTS_b/FTM0_CH1/LCD_P22}", "58/B8 {PC.3/CMP1_IN1/SPI0_PCS1/UART1_RX/FTM0_CH2/LCD_P23}", "65/A8 {PC.4/SPI0_PCS0/UART1_TX/FTM0_CH3/CMP1_OUT/LCD_P24}", "66/D7 {PC.5/SPI0_SCK/LPT0_ALT2/CMP0_OUT/LCD_P25}", "67/C7 {PC.6/CMP0_IN0/SPI0_SOUT/PDB0_EXTRG/LCD_P26}", "68/B7 {PC.7/CMP0_IN1/SPI0_SIN/LCD_P27}", "69/A7 {PC.8/ADC1_SE4b/CMP0_IN2/I2S0_MCLK/I2S0_CLKIN/LCD_P28}", "70/D6 {PC.9/ADC1_SE5b/CMP0_IN3/I2S0_RX_BCLK/FTM2_FLT0/LCD_P29}", "71/C6 {PC.10/ADC1_SE6b/CMP0_IN4/I2C1_SCL/I2S0_RX_FS/LCD_P30}", "72/C5 {PC.11/ADC1_SE7b/I2C1_SDA/I2S0_RXD/LCD_P31}", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA"},
    // PORT D (0..31)
    { "73/D4 {PD.0/SPI0_PCS0/UART2_RTS_b}", "74/D3 {PD.1/ADC0_SE5b/SPI0_SCK/UART2_CTS_b}", "75/C3 {PD.2/SPI0_SOUT/UART2_RX}", "76/B3 {PD.3/SPI0_SIN/UART2_TX}", "77/A3 {PD.4/SPI0_PCS1/UART0_RTS_b/FTM0_CH4/EWM_IN}", "78/A2 {PD.5/ADC0_SE6b/SPI0_PCS2/UART0_CTS_b/FTM0_CH5/EWM_OUT_b}", "79/B2 {PD.6/ADC0_SE7b/SPI0_PCS3/UART0_RX/FTM0_CH6/FTM0_FLT0}", "80/A1 {PD.7/CMT_IRO/UART0_TX/FTM0_CH7/FTM0_FLT1}", "NA", "NA", "B1 {PD.10}", "C2 {PD.11/SPI2_PCS0/SDHC0_CLKIN}", "C1 {PD.12/SPI2_SCK/SDHC0_D4}", "D2 {PD.13/SPI2_SOUT/SDHC0_D5}", "D1 {PD.14/SPI2_SIN/SDHC0_D6}", "E1 {PD.15/SPI2_PCS1/SDHC0_D7}", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA"},
    // PORT E (0..31)
    { "1/E4 {PE.0/ADC1_SE4a/SPI1_PCS1/UART1_TX/SDHC0_D1/I2C1_SDA}", "2/E3 {PE.1/ADC1_SE5a/SPI1_SOUT/UART1_RX/SDHC0_D0/I2C1_SCL}", "3/E2 {PE.2/ADC1_SE6a/SPI1_SCK/UART1_CTS_b/SDHC0_DCLK}", "4/F4 {PE.3/ADC1_SE7a/SPI1_SIN/UART1_RTS_b/SDHC0_CMD}", "5/H7 {PE.4/SPI1_PCS0/UART3_TX/SDHC0_D3}", "6/G4 {PE.5/SPI1_PCS2/UART3_RX/SDHC0_D2}", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA"}
};
#elif defined DEVICE_100_PIN
static const char *cPinNumber[PORTS_AVAILABLE][PORT_WIDTH] = {           // 100 pin LQPF
    // PORT A (0..31)
    { "34 {PA.0/TSI0_CH1/UART0_CTS_b/FTM0_CH5/JTAG_TCLK/SWD_CLK/EZP_CLK}", "35 {PA.1/TSI0_CH2/UART0_RX/FTM0_CH6/JTAG_TDI/EZP_DI}", "36 {PA.2/TSI0_CH3/UART0_TX/FTM0_CH7/JTAG_TDO/TRACE_SWO/EZP_DO}", "37 {PA.3/TSI0_CH4/UART0_RTS_b/FTM0_CH0/JTAG_TMS/SWD_DIO}", "38 {PA.4/TSI0_CH5/FTM0_CH1/NMI_b/EZP_CS_b}", "39 {PA.5/FTM0_CH2/CMP2_OUT/I2S0_RX_BCLK/JTAG_TRST}", "NA", "NA", "NA", "NA", "NA", "NA", "42 {PA.12/CMP2_IN0/CAN0_TX/FTM1_CH0/I2S0_TXD/FTM1_QD_PHA}", "43 {PA.13/CMP2_IN1/CAN0_RX/FTM1_CH1/I2S0_TX_FS/FTM1_QD_PHB}", "44 {PA.14/SPI0_PCS0/UART0_TX/I2S0_TX_BCLK}", "45 {PA.15/SPI0_SCK/UART0_RX/I2S0_RXD}", "46 {PA.16/SPI0_SOUT/UART0_CTS_b/I2S0_RX_FS}", "47 {PA.17/ADC1_SE17/SPI0_SIN/UART0_RTS_b/I2S0_MCLK/ISS0_CLKIN}", "50 {PA.18/EXTAL/FTM0_FLT2/FTM_CLKIN0}", "51 {PA.19/XTAL/FTM1_FLT0/FTM_CLKIN1/LPT0_ALT1}", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA"},
    // PORT B (0..31)
    { "53 {PB.0/ADC0_SE8/ADC1_SE8/TSI0_CH0/I2C0_SCL/FTM1_CH0/FTM1_QD_PHA/LCD_P0}", "54 {PB.1/ADC0_SE9/ADC1_SE9/TSI0_CH6/I2C0_SDA/FTM1_CH1/FTM1_QD_PHB/LCD_P1}", "55 {PB.2/ADC0_SE12/TSI0_CH7/I2C0_SCL/UART0_RTS_b/FTM0_FLT3/LCD_P2}", "56 {PB.3/ADC0_SE13/TSI0_CH8/I2C0_SDA/UART0_CTS_b/FTM0_FLT0/LCD_P3}", "NA", "NA", "NA", "57 {PB.7/ADC1_SE13/LCD_P7}", "58 {PB.8/UART3_RTS_b//LCD_P8}", "59 {PB.9/SPI1_PCS1/UART3_CTS_b/LCD_P9}", "60 {PB.10/ADC1_SE14/SPI1_PCS0/UART3_RX/FTM0_FLT1/LCD_P10}", "61 {PB.11/ADC1_SE15/SPI1_SCK/UART3_TX/FTM0_FLT2/LCD_P11}", "NA", "NA", "NA", "NA", "62 {PB.16/TSI0_CH9/SPI1_SOUT/UART0_RX/EWM_IN/LCD_P12}", "63 {PB.17/TSI0_CH10/SPI1_SIN/UART0_TX/EWM_OUT_b/LCD_P13}", "64 {PB.18/TSI0_CH11/CAN0_TX/FTM2_CH0/I2S0_TX_BCLK/FTM2_QD_PHA/LCD_P14}", "65 {PB.19/TSI0_CH12/CAN0_RX/FTM2_CH1/I2S0_TX_FS/FTM2_QD_PHB/LCD_P15}", "66 {PB.20/SPI2_PCS0/CMP0_OUT/LCD_P16}", "67 {PB.21/SPI2_SCK/CMP1_OUT/LCD_P17}", "68 {PB.22/SPI2_SOUT/CMP2_OUT/LCD_P18}", "69 {PB.23/SPI2_SIN/SPI0_PCS5/LCD_P19}", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA"},
    // PORT C (0..31)
    { "70 {PC.0/ADC0_SE14/TSI0_CH13/SPI0_PCS4/PDB0_EXTRG/I2S0_TXD/LCD_P20}", "71 {PC.1/ADC0_SE15/TSI0_CH14/SPI0_PCS3/UART1_RTS_b/FTM0_CH0/LCD_P21}", "72 {PC.2/ADC0_SE4b/CMP1_IN0/TSI0_CH15/SPI0_PCS2/UART1_CTS_b/FTM0_CH1/LCD_P22}", "73 {PC.3/CMP1_IN1/SPI0_PCS1/UART1_RX/FTM0_CH2/LCD_P23}", "80 {PC.4/SPI0_PCS0/UART1_TX/FTM0_CH3/CMP1_OUT/LCD_P24}", "81 {PC.5/SPI0_SCK/LPT0_ALT2/CMP0_OUT/LCD_P25}", "82 {PC.6/CMP0_IN0/SPI0_SOUT/PDB0_EXTRG/LCD_P26}", "83 {PC.7/CMP0_IN1/SPI0_SIN/LCD_P27}", "84 {PC.8/ADC1_SE4b/CMP0_IN2/I2S0_MCLK/I2S0_CLKIN/LCD_P28}", "85 {PC.9/ADC1_SE5b/CMP0_IN3/I2S0_RX_BCLK/FTM2_FLT0/LCD_P29}", "86 {PC.10/ADC1_SE6b/CMP0_IN4/I2C1_SCL/I2S0_RX_FS/LCD_P30}", "87 {PC.11/ADC1_SE7b/I2C1_SDA/I2S0_RXD/LCD_P31}", "NA", "NA", "NA", "NA", "90 {PC.16/CAN1_RX/UART3_RX/LCD_P36}", "91 {PC.17/CAN1_TX/UART3_TX/LCD_P37}", "92 {PC.18/UART3_RTS_b/LCD_P38}", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA"},
    // PORT D (0..31)
    { "93 {PD.0/SPI0_PCS0/UART2_RTS_b}", "94 {PD.1/ADC0_SE5b/SPI0_SCK/UART2_CTS_b}", "95 {PD.2/SPI0_SOUT/UART2_RX}", "96 {PD.3/SPI0_SIN/UART2_TX}", "97 {PD.4/SPI0_PCS1/UART0_RTS_b/FTM0_CH4/EWM_IN}", "98 {PD.5/ADC0_SE6b/SPI0_PCS2/UART0_CTS_b/FTM0_CH5/EWM_OUT_b}", "99 {PD.6/ADC0_SE7b/SPI0_PCS3/UART0_RX/FTM0_CH6/FTM0_FLT0}", "100 {PD.7/CMT_IRO/UART0_TX/FTM0_CH7/FTM0_FLT1}", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA"},
    // PORT E (0..31)
    { "1 {PE.0/ADC1_SE4a/SPI1_PCS1/UART1_TX/SDHC0_D1/I2C1_SDA}", "2 {PE.1/ADC1_SE5a/SPI1_SOUT/UART1_RX/SDHC0_D0/I2C1_SCL}", "3 {PE.2/ADC1_SE6a/SPI1_SCK/UART1_CTS_b/SDHC0_DCLK}", "4 {PE.3/ADC1_SE7a/SPI1_SIN/UART1_RTS_b/SDHC0_CMD}", "5 {PE.4/SPI1_PCS0/UART3_TX/SDHC0_D3}", "6 {PE.5/SPI1_PCS2/UART3_RX/SDHC0_D2}", "7 {PE.6/SPI1_PCS3/UART3_CTS_b/I2S0_MCLK/I2S0_CLKIN}", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "31 {PE.24/ADC0_SE17/CAN1_TX/UART4_TX/EWM_OUT_b}", "32 {PE.25/ADC0_SE18/CAN1_RX/UART4_RX/EWM_IN}", "33 {PE.26/UART4_CTS_b/RTC_CLKOUT/USB_CLKIN}", "NA", "NA", "NA", "NA", "NA"}
};
#elif defined DEVICE_121_PIN
static const char *cPinNumber[PORTS_AVAILABLE][PORT_WIDTH] = {           // 121 pin MAPBGA
    // PORT A (0..31)
    { "J6 {PA.0/TSI0_CH1/UART0_CTS_b/FTM0_CH5/JTAG_TCLK/SWD_CLK/EZP_CLK}", "H8 {PA.1/TSI0_CH2/UART0_RX/FTM0_CH6/JTAG_TDI/EZP_DI}", "J7 {PA.2/TSI0_CH3/UART0_TX/FTM0_CH7/JTAG_TDO/TRACE_SWO/EZP_DO}", "H9 {PA.3/TSI0_CH4/UART0_RTS_b/FTM0_CH0/JTAG_TMS/SWD_DIO}", "J8 {PA.4/TSI0_CH5/FTM0_CH1/NMI_b/EZP_CS_b}", "K7 {PA.5/FTM0_CH2/CMP2_OUT/I2S0_RX_BCLK/JTAG_TRST}", "NA", "NA", "NA", "NA", "J9 {PA.10/FTM2_CH0/FTM2_QD_PHA/TRACE_D0}", "J4 {PA.11/FTM2_CH1/FTM2_QD_PHB}", "K8 {PA.12/CMP2_IN0/CAN0_TX/FTM1_CH0/I2S0_TXD/FTM1_QD_PHA}", "L8 {PA.13/CMP2_IN1/CAN0_RX/FTM1_CH1/I2S0_TX_FS/FTM1_QD_PHB}", "K9 {PA.14/SPI0_PCS0/UART0_TX/I2S0_TX_BCLK}", "L9 {PA.15/SPI0_SCK/UART0_RX/I2S0_RXD}", "L10 {PA.16/SPI0_SOUT/UART0_CTS_b/I2S0_RX_FS}", "H10 {PA.17/ADC1_SE17/SPI0_SIN/UART0_RTS_b/I2S0_MCLK/ISS0_CLKIN}", "L11 {PA.18/EXTAL/FTM0_FLT2/FTM_CLKIN0}", "K11 {PA.19/XTAL/FTM1_FLT0/FTM_CLKIN1/LPT0_ALT1}", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "H11 {PA.29}", "NA", "NA"},
    // PORT B (0..31)
    { "G11 {PB.0/ADC0_SE8/ADC1_SE8/TSI0_CH0/I2C0_SCL/FTM1_CH0/FTM1_QD_PHA/LCD_P0}", "G10 {PB.1/ADC0_SE9/ADC1_SE9/TSI0_CH6/I2C0_SDA/FTM1_CH1/FTM1_QD_PHB/LCD_P1}", "G9 {PB.2/ADC0_SE12/TSI0_CH7/I2C0_SCL/UART0_RTS_b/FTM0_FLT3/LCD_P2}", "G8 {PB.3/ADC0_SE13/TSI0_CH8/I2C0_SDA/UART0_CTS_b/FTM0_FLT0/LCD_P3}", "NA", "NA", "F11 {PB.6/ADC1_SE12/LCD_P6}", "E11 {PB.7/ADC1_SE13/LCD_P7}", "D11 {PB.8/UART3_RTS_b//LCD_P8}", "E10 {PB.9/SPI1_PCS1/UART3_CTS_b/LCD_P9}", "D10 {PB.10/ADC1_SE14/SPI1_PCS0/UART3_RX/FTM0_FLT1/LCD_P10}", "C10 {PB.11/ADC1_SE15/SPI1_SCK/UART3_TX/FTM0_FLT2/LCD_P11}", "NA", "NA", "NA", "NA", "B10 {PB.16/TSI0_CH9/SPI1_SOUT/UART0_RX/EWM_IN/LCD_P12}", "E9 {PB.17/TSI0_CH10/SPI1_SIN/UART0_TX/EWM_OUT_b/LCD_P13}", "D9 {PB.18/TSI0_CH11/CAN0_TX/FTM2_CH0/I2S0_TX_BCLK/FTM2_QD_PHA/LCD_P14}", "C9 {PB.19/TSI0_CH12/CAN0_RX/FTM2_CH1/I2S0_TX_FS/FTM2_QD_PHB/LCD_P15}", "F10 {PB.20/SPI2_PCS0/CMP0_OUT/LCD_P16}", "F9 {PB.21/SPI2_SCK/CMP1_OUT/LCD_P17}", "F8 {PB.22/SPI2_SOUT/CMP2_OUT/LCD_P18}", "E8 {PB.23/SPI2_SIN/SPI0_PCS5/LCD_P19}", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA"},
    // PORT C (0..31)
    { "B9 {PC.0/ADC0_SE14/TSI0_CH13/SPI0_PCS4/PDB0_EXTRG/I2S0_TXD/LCD_P20}", "D8 {PC.1/ADC0_SE15/TSI0_CH14/SPI0_PCS3/UART1_RTS_b/FTM0_CH0/LCD_P21}", "C8 {PC.2/ADC0_SE4b/CMP1_IN0/TSI0_CH15/SPI0_PCS2/UART1_CTS_b/FTM0_CH1/LCD_P22}", "B8 {PC.3/CMP1_IN1/SPI0_PCS1/UART1_RX/FTM0_CH2/LCD_P23}", "A8 {PC.4/SPI0_PCS0/UART1_TX/FTM0_CH3/CMP1_OUT/LCD_P24}", "D7 {PC.5/SPI0_SCK/LPT0_ALT2/CMP0_OUT/LCD_P25}", "C7 {PC.6/CMP0_IN0/SPI0_SOUT/PDB0_EXTRG/LCD_P26}", "B7 {PC.7/CMP0_IN1/SPI0_SIN/LCD_P27}", "A7 {PC.8/ADC1_SE4b/CMP0_IN2/I2S0_MCLK/I2S0_CLKIN/LCD_P28}", "D6 {PC.9/ADC1_SE5b/CMP0_IN3/I2S0_RX_BCLK/FTM2_FLT0/LCD_P29}", "C6 {PC.10/ADC1_SE6b/CMP0_IN4/I2C1_SCL/I2S0_RX_FS/LCD_P30}", "C5 {PC.11/ADC1_SE7b/I2C1_SDA/I2S0_RXD/LCD_P31}", "B6 {PC.12/UART4_RTS_b/LCD_P32}", "A6 {PC.13/UART4_CTS_b/LCD_P33}", "A5 {PC.14/UART4_RX/LCD_P34}", "B5 {PC.15/UART4_TX/LCD_P35}", "D5 {PC.16/CAN1_RX/UART3_RX/LCD_P36}", "C4 {PC.17/CAN1_TX/UART3_TX/LCD_P37}", "B4 {PC.18/UART3_RTS_b/LCD_P38}", "A4 {PC.19/UART3_CTS_b/LCD_P39}", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA"},
    // PORT D (0..31)
    { "D4 {PD.0/SPI0_PCS0/UART2_RTS_b}", "D3 {PD.1/ADC0_SE5b/SPI0_SCK/UART2_CTS_b}", "C3 {PD.2/SPI0_SOUT/UART2_RX}", "B3 {PD.3/SPI0_SIN/UART2_TX}", "A3 {PD.4/SPI0_PCS1/UART0_RTS_b/FTM0_CH4/EWM_IN}", "A2 {PD.5/ADC0_SE6b/SPI0_PCS2/UART0_CTS_b/FTM0_CH5/EWM_OUT_b}", "B2 {PD.6/ADC0_SE7b/SPI0_PCS3/UART0_RX/FTM0_CH6/FTM0_FLT0}", "A1 {PD.7/CMT_IRO/UART0_TX/FTM0_CH7/FTM0_FLT1}", "NA", "NA", "B1 {PD.10/UART5_RTS_b}", "C2 {PD.11/SPI2_PCS0/UART5_CTS_b/SDHC0_CLKIN}", "C1 {PD.12/SPI2_SCK/SDHC0_D4}", "D2 {PD.13/SPI2_SOUT/SDHC0_D5}", "D1 {PD.14/SPI2_SIN/SDHC0_D6}", "E1 {PD.15/SPI2_PCS1/SDHC0_D7}", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA"},
    // PORT E (0..31)
    { "E4 {PE.0/ADC1_SE4a/SPI1_PCS1/UART1_TX/SDHC0_D1/I2C1_SDA}", "E3 {PE.1/ADC1_SE5a/SPI1_SOUT/UART1_RX/SDHC0_D0/I2C1_SCL}", "E2 {PE.2/ADC1_SE6a/SPI1_SCK/UART1_CTS_b/SDHC0_DCLK}", "F4 {PE.3/ADC1_SE7a/SPI1_SIN/UART1_RTS_b/SDHC0_CMD}", "H7 {PE.4/SPI1_PCS0/UART3_TX/SDHC0_D3}", "G4 {PE.5/SPI1_PCS2/UART3_RX/SDHC0_D2}", "F3 {PE.6/SPI1_PCS3/UART3_CTS_b/I2S0_MCLK/I2S0_CLKIN}", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "H5 {PE.24/ADC0_SE17/CAN1_TX/UART4_TX/EWM_OUT_b}", "J5 {PE.25/ADC0_SE18/CAN1_RX/UART4_RX/EWM_IN}", "H6 {PE.26/UART4_CTS_b/RTC_CLKOUT/USB_CLKIN}", "NA", "NA", "NA", "NA", "NA"}
};
#else
static const char *cPinNumber[PORTS_AVAILABLE][PORT_WIDTH] = {           // 144 pin LQFP/MAPBGA
    // PORT A (0..31)
    { "50/J5 {PA.0/TSI0_CH1/UART0_CTS_b/FTM0_CH5/JTAG_TCLK/SWD_CLK/EZP_CLK}", "51/J6 {PA.1/TSI0_CH2/UART0_RX/FTM0_CH6/JTAG_TDI/EZP_DI}", "52/K6 {PA.2/TSI0_CH3/UART0_TX/FTM0_CH7/JTAG_TDO/TRACE_SWO/EZP_DO}", "53/K7 {PA.3/TSI0_CH4/UART0_RTS_b/FTM0_CH0/JTAG_TMS/SWD_DIO}", "54/L7 {PA.4/TSI0_CH5/FTM0_CH1/NMI_b/EZP_CS_b}", "55/M8 {PA.5/FTM0_CH2/CMP2_OUT/I2S0_RX_BCLK/JTAG_TRST}", "58/J7 {PA.6/FTM0_CH3/FB_CLKOUT/TRACE_CLKOUT}", "59/J8 {PA.7/ADC0_SE10/FTM0_CH4/FB_AD18/TRACE_D3}", "60/K8 {PA.8/ADC0_SE11/FTM1_CH0/FB_AD17/FTM1_QD_PHA/TRACE_D2}", "61/L8 {PA.9/FTM1_CH1/FB_AD16/FTM1_QD_PHB/TRACE_D1}", "62/M9 {PA.10/FTM2_CH0/FB_AD15/FTM2_QD_PHA/TRACE_D0}", "63/L9 {PA.11/FTM2_CH1/FB_OE_b/FTM2_QD_PHB}", "64/K9 {PA.12/CMP2_IN0/CAN0_TX/FTM1_CH0/FB_CS5_b/FB_TSIZ1/FB_BE23_16_BLS15_8_b/I2S0_TXD/FTM1_QD_PHA}", "65/J9 {PA.13/CMP2_IN1/CAN0_RX/FTM1_CH1/FB_CS4_b/FB_TSIZ0/FB_BE31_24_BLS7_0_b/I2S0_TX_FS/FTM1_QD_PHB}", "66/L10 {PA.14/SPI0_PCS0/UART0_TX/FB_AD31/I2S0_TX_BCLK}", "67/L11 {PA.15/SPI0_SCK/UART0_RX/FB_AD30/I2S0_RXD}", "68/K10 {PA.16/SPI0_SOUT/UART0_CTS_b/FB_AD29/I2S0_RX_FS}", "69/K11 {PA.17/ADC1_SE17/SPI0_SIN/UART0_RTS_b/FB_AD28/I2S0_MCLK/ISS0_CLKIN}", "72/M12 {PA.18/EXTAL/FTM0_FLT2/FTM_CLKIN0}", "73/M11 {PA.19/XTAL/FTM1_FLT0/FTM_CLKIN1/LPT0_ALT1}", "NA", "NA", "NA", "NA", "75/K12 {PA.24/FB_AD14}", "76/J12 {PA.25/FB_AD13}", "77/J11 {PA.26/FB_AD12}", "78/J10 {PA.27/FB_AD11}", "79/H12 {PA.28/FB_AD10}", "80/H11 {PA.29/FB_AD19}", "NA", "NA"},
    // PORT B (0..31)
    { "81/H10 {PB.0/ADC0_SE8/ADC1_SE8/TSI0_CH0/I2C0_SCL/FTM1_CH0/FTM1_QD_PHA/LCD_P0}", "82/H9 {PB.1/ADC0_SE9/ADC1_SE9/TSI0_CH6/I2C0_SDA/FTM1_CH1/FTM1_QD_PHB/LCD_P1}", "83/G12 {PB.2/ADC0_SE12/TSI0_CH7/I2C0_SCL/UART0_RTS_b/FTM0_FLT3/LCD_P2}", "84/G11 {PB.3/ADC0_SE13/TSI0_CH8/I2C0_SDA/UART0_CTS_b/FTM0_FLT0/LCD_P3}", "85/G10 {PB.4/ADC1_SE10/FTM1_FLT0/LCD_P4}", "86/G9 {PB.5/ADC1_SE11/FTM2_FLT0/LCD_P5}", "87/F12 {PB.6/ADC1_SE12/LCD_P6}", "88/F11 {PB.7/ADC1_SE13//LCD_P7}", "89/F10 {PB.8/UART3_RTS_b//LCD_P8}", "90/F9 {PB.9/SPI1_PCS1/UART3_CTS_b//LCD_P9}", "91/E12 {PB.10/ADC1_SE14/SPI1_PCS0/UART3_RX/FTM0_FLT1/LCD_P10}", "92/E11 {PB.11/ADC1_SE15/SPI1_SCK/UART3_TX/FTM0_FLT2/LCD_P11}", "NA", "NA", "NA", "NA", "95/E10 {PB.16/TSI0_CH9/SPI1_SOUT/UART0_RX/EWM_IN/LCD_P12}", "96/E9 {PB.17/TSI0_CH10/SPI1_SIN/UART0_TX/EWM_OUT_b/LCD_P13}", "97/D12 {PB.18/TSI0_CH11/CAN0_TX/FTM2_CH0/I2S0_TX_BCLK/FTM2_QD_PHA/LCD_P14}", "98/D11 {PB.19/TSI0_CH12/CAN0_RX/FTM2_CH1/I2S0_TX_FS/FTM2_QD_PHB/LCD_P15}", "99/D10 {PB.20/SPI2_PCS0/CMP0_OUT/LCD_P16}", "100/D9 {PB.21/SPI2_SCK/CMP1_OUT/LCD_P17}", "101/C12 {PB.22/SPI2_SOUT/CMP2_OUT/LCD_P18}", "102/C11 {PB.23/SPI2_SIN/SPI0_PCS5/LCD_P19}", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA"},
    // PORT C (0..31)
    { "103/B12 {PC.0/ADC0_SE14/TSI0_CH13/SPI0_PCS4/PDB0_EXTRG/I2S0_TXD/LCD_P20}", "104/B11 {PC.1/ADC0_SE15/TSI0_CH14/SPI0_PCS3/UART1_RTS_b/FTM0_CH0/LCD_P21}", "105/A12 {PC.2/ADC0_SE4b/CMP1_IN0/TSI0_CH15/SPI0_PCS2/UART1_CTS_b/FTM0_CH1/LCD_P22}", "106/A11 {PC.3/CMP1_IN1/SPI0_PCS1/UART1_RX/FTM0_CH2/LCD_P23}", "113/A9 {PC.4/SPI0_PCS0/UART1_TX/FTM0_CH3/CMP1_OUT/LCD_P24}", "114/D8 {PC.5/SPI0_SCK/LPT0_ALT2/CMP0_OUT/LCD_P25}", "115/C8 {PC.6/CMP0_IN0/SPI0_SOUT/PDB0_EXTRG/LCD_P26}", "116/B8 {PC.7/CMP0_IN1/SPI0_SIN/LCD_P27}", "117/A8 {PC.8/ADC1_SE4b/CMP0_IN2/I2S0_MCLK/I2S0_CLKIN/LCD_P28}", "119/D7 {PC.9/ADC1_SE5b/CMP0_IN3/I2S0_RX_BCLK/FTM2_FLT0/LCD_P29}", "119/C7 {PC.10/ADC1_SE6b/CMP0_IN4/I2C1_SCL/I2S0_RX_FS/LCD_P30}", "120/B7 {PC.11/ADC1_SE7b/I2C1_SDA/I2S0_RXD/LCD_P31}", "121/A7 {PC.12/UART4_RTS_b/LCD_P32}", "122/D6 {PC.13/UART4_CTS_b/LCD_P33}", "123/C6 {PC.14/UART4_RX/LCD_P34}", "124/B6 {PC.15/UART4_TX/LCD_P35}", "125/A6 {PC.16/CAN1_RX/UART3_RX/LCD_P36}", "126/D5 {PC.17/CAN1_TX/UART3_TX/LCD_P37}", "127/C5 {PC.18/UART3_RTS_b/LCD_P38}", "128/B5 {PC.19/UART3_CTS_b/LCD_P39}", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA"},
    // PORT D (0..31)
    { "127/A5 {PD.0/SPI0_PCS0/UART2_RTS_b/FB_ALE/FB_CS1_b/FB_TS_b}", "128/D4 {PD.1/ADC0_SE5b/SPI0_SCK/UART2_CTS_b/FB_CS0_b}", "129/C4 {PD.2/SPI0_SOUT/UART2_RX/FB_AD4}", "130/B4 {PD.3/SPI0_SIN/UART2_TX/FB_AD3}", "131/A4 {PD.4/SPI0_PCS1/UART0_RTS_b/FTM0_CH4/FB_AD2/EWM_IN}", "132/A3 {PD.5/ADC0_SE6b/SPI0_PCS2/UART0_CTS_b/FTM0_CH5/FB_AD1/EWM_OUT_b}", "133/A2 {PD.6/ADC0_SE7b/SPI0_PCS3/UART0_RX/FTM0_CH6/FB_AD0/FTM0_FLT0}", "136/A1 {PD.7/CMT_IRO/UART0_TX/FTM0_CH7/FTM0_FLT1}", "NA", "NA", "139/B3 {PD.10/UART5_RTS_b/FB_A18}", "140/B2 {PD.11/SPI2_PCS0/UART5_CTS_b/SDHC0_CLKIN/FB_A19}", "141/B1 {PD.12/SPI2_SCK/SDHC0_D4/FB_A20}", "142/C3 {PD.13/SPI2_SOUT/SDHC0_D5/FB_A21}", "143/C2 {PD.14/SPI2_SIN/SDHC0_D6/FB_A22}", "144/C1 {PD.15/SPI2_PCS1/SDHC0_D7/FB_A23}", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA"},
    // PORT E (0..31)
    { "1/D3 {PE.0/ADC1_SE4a/SPI1_PCS1/UART1_TX/SDHC0_D1/FB_AD27/I2C1_SDA}", "2/D2 {PE.1/ADC1_SE5a/SPI1_SOUT/UART1_RX/SDHC0_D0/FB_AD26/I2C1_SCL}", "3/D1 {PE.2/ADC1_SE6a/SPI1_SCK/UART1_CTS_b/SDHC0_DCLK/FB_AD25}", "4/E4 {PE.3/ADC1_SE7a/SPI1_SIN/UART1_RTS_b/SDHC0_CMD/FB_AD24}", "7/E3 {PE.4/SPI1_PCS0/UART3_TX/SDHC0_D3/FB_CS3_b/FB_BE7_0_BLS31_24b/FB_TA_b}", "8/E2 {PE.5/SPI1_PCS2/UART3_RX/SDHC0_D2/FB_TBST_b/FB_CS2_b/FB_BE15_8_BLS23_16_b}", "9/E1 {PE.6/SPI1_PCS3/UART3_CTS_b/I2S0_MCLK/FB_ALE/FB_CS1_b/FB_TS_b/I2S0_CLKIN}", "10/F4 {PE.7/UART3_RTS_b/I2S0_RXD/FB_CS0_b}", "11/F3 {PE.8/UART5_TX/I2S0_RX_FS/FB_AD4}", "12/F2 {PE.9/UART5_RX/I2S0_RX_BCLK/FB_AD3}", "13/F1 {PE.10/UART5_CTS_b/I2S0_TXD/FB_AD2", "14/G4 {PE.11/UART5_RTS_b/I2S0_TX_FS/FB_AD1", "15/G3 {PE.12/I2S0_TX_BCLK/FB_AD0", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "NA", "45/M4 {PE.24/ADC0_SE17/CAN1_TX/UART4_TX/EWM_OUT_b", "46/K5 {PE.25/ADC0_SE18/CAN1_RX/UART4_RX/FB_AD23/EWM_IN", "47/K4 {PE.26/UART4_CTS_b/FB_AD22/RTC_CLKOUT/USB_CLKIN", "48/J4 {PE.27/UART4_RTS_b/FB_AD21", "49/H4 {PE.28/FB_AD20}", "NA", "NA", "NA"}
};
#endif

#if defined DEVICE_80_PIN
static const char *cPer[PORTS_AVAILABLE][PORT_WIDTH][8] = {              // 81 pin MAPGBA / 80 pin LQPF
    {
        // ALT 0           ALT 1    ALT2         ALT 3         ALT 4          ALT 5        ALT 6       ALT 7
        {  "TSI0_CH1",     "PTA0",  "UART0_CTS", "FTM0_CH5",   "?",           "?",         "?",        "JTAG_TCLK/SWD_CLK" }, // PORT A
        {  "TSI0_CH2",     "PTA1",  "UART0_RX",  "FTM0_CH6",   "?",           "?",         "?",        "JTAG_TDI"          },
        {  "TSI0_CH3",     "PTA2",  "UART0_TX",  "FTM0_CH7",   "?",           "?",         "?",        "JTAG_TDI/TRACE_SWO"},
        {  "TSI0_CH4",     "PTA3",  "UART0_RTS", "FTM0_CH0",   "?",           "?",         "?",        "JTAG_TMS/SWD_DIO"  },
        {  "TSI0_CH5",     "PTA4",  "?",         "FTM0_CH1",   "?",           "?",         "?",        "NMI"               },
        {  "D",            "PTA5",  "?",         "FTM0_CH2",   "?",           "CMP2_OUT",  "I2S0_RX_BCLK","JTAG_TRST"      },
        {  "?",            "?",     "?",         "?",          "?",           "?",         "?",        "?"                 },
        {  "?",            "?",     "?",         "?",          "?",           "?",         "?",        "?"                 },
        {  "?",            "?",     "?",         "?",          "?",           "?",         "?",        "?"                 },
        {  "?",            "?",     "?",         "?",          "?",           "?",         "?",        "?"                 },
        {  "?",            "?",     "?",         "?",          "?",           "?",         "?",        "?"                 },
        {  "?",            "?",     "?",         "?",          "?",           "?",         "?",        "?"                 },
        {  "CMP2_IN0",     "PTA12", "CAN0_TX",   "FTM1_CH0",   "?",           "?",         "I2S0_TXD", "FTM1_QD_PHA"       },
        {  "CMP2_IN1",     "PTA13", "CAN0_RX",   "FTM1_CH1",   "?",           "?",         "I2S0_TX_FS","FTM1_QD_PHB"      },
        {  "D",            "PTA14", "SPI0_PCS0", "UART0_TX",   "?",           "?",         "I2S0_TX_BCLK","?"              },
        {  "D",            "PTA15", "SPI0_SCK",  "UART0_RX",   "?",           "?",         "I2S0_RXD", "?"                 },
        {  "D",            "PTA16", "SPI0_SOUT", "UART0_CTS",  "?",           "?",         "I2S0_RX_FS","?"                },
        {  "ADC1_SE17",    "PTA17", "SPI0_SIN",  "UART0_RTS",  "?",           "?",         "I2S0_MCLK","I2S0_CLKIN"        },
        {  "EXTAL",        "PTA18", "?",         "FTM0_FLT2",  "FTM_CLKIN0",  "?",         "?",        "?"                 },
        {  "XTAL",         "PTA19", "?",         "FTM1_FLT0",  "FTM_CLKIN1",  "?",         "LPT0_ALT1","?"                 },
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
        {  "LCD_P0/ADC0_SE8/TSI0_CH0","PTB0","I2C0_SCL","FTM1_CH0",   "?",    "?",         "FTM1_QD_PHA", "LCD_P0"         }, // PORT B
        {  "LCD_P1/ADC0_SE9/TSI0_CH6","PTB1","I2C0_SDA","FTM1_CH1",   "?",    "?",         "FTM1_QD_PHB", "LCD_P1"         },
        {  "LCD_P2/ADC0_SE12/TSI0_CH7","PTB2","I2C0_SCL","UART0_RTS", "?",    "?",         "FTM0_FLT3",   "LCD_P2"         },
        {  "LCD_P3/ADC0_SE13/TSI0_CH8","PTB3","I2C0_SDA","UART0_CTS", "?",    "?",         "FTM0_FLT0",   "LCD_P3"         },
        {  "?",            "?",     "?",         "?",          "?",           "?",         "?",        "?"                 },
        {  "?",            "?",     "?",         "?",          "?",           "?",         "?",        "?"                 },
        {  "?",            "?",     "?",         "?",          "?",           "?",         "?",        "?"                 },
        {  "?",            "?",     "?",         "?",          "?",           "?",         "?",        "?"                 },
        {  "LCD_P8",       "PTB8",  "?",         "UART3_RTS",  "?",           "?",         "?",           "LCD_P8"         },
        {  "LCD_P9",       "PTB9",  "SPI1_PCS1", "UART3_CTS",  "?",           "?",         "?",           "LCD_P9"         },
        {  "LCD_P10/ADC1_SE14", "PTB10", "SPI1_PCS0", "UART3_RX",   "?",      "?",         "FTM0_FLT1",   "LCD_P10"        },
        {  "LCD_P11/ADC1_SE15", "PTB11", "SPI1_SCK",  "UART3_TX",   "?",      "?",         "FTM0_FLT2",   "LCD_P11"        },
        {  "?",            "?",     "?",         "?",          "?",           "?",         "?",        "?"                 },
        {  "?",            "?",     "?",         "?",          "?",           "?",         "?",        "?"                 },
        {  "?",            "?",     "?",         "?",          "?",           "?",         "?",        "?"                 },
        {  "?",            "?",     "?",         "?",          "?",           "?",         "?",        "?"                 },
        {  "LCD_P12/TSI0_CH9",  "PTB16", "SPI1_SOUT", "UART0_RX",   "?",      "?",         "EWM_IN",      "LCD_P12"        },
        {  "LCD_P13/TSI0_CH10", "PTB17", "SPI1_SIN",  "UART0_TX",   "?",      "?",         "EWM_OUT_b",   "LCD_P13"        },
        {  "LCD_P14/TSI0_CH11", "PTB18", "CAN0_TX",   "FTM2_CH0",   "I2S0_TX_BCLK",  "?",  "FTM2_QD_PHA", "LCD_P14"        },
        {  "LCD_P15/TSI0_CH12", "PTB19", "CAN0_RX",   "FTM2_CH1",   "I2S0_TX_FS",    "?",  "FTM2_QD_PHB", "LCD_P15"        },
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
        {  "LCD_P20/ADC0_SE14/TSI0_CH13","PTC0","SPI0_PCS4","PDB0_EXTRG","I2S0_TXD",  "?", "?",        "LCD_P20"           }, // PORT C
        {  "LCD_P21/ADC0_SE15/TSI0_CH14","PTC1","SPI0_PCS3","UART1_RTS","FTM0_CH0",   "?",     "?",        "LCD_P21"       },
        {  "LCD_P22/ADC0_SE4/CMP1_IN0TSI0_CH15","PTC2","SPI0_PCS2","UART1_CTS","FTM0_CH1","?", "?",        "LCD_P22"       },
        {  "LCD_P23/CMP1_IN1",     "PTC3",  "SPI0_PCS1", "UART1_RX",   "FTM0_CH2",    "?",     "?",        "LCD_P23"       },
        {  "LCD_P24",            "PTC4",  "SPI0_PCS0", "UART1_TX",   "FTM0_CH3",    "?",       "CMP1_OUT", "LCD_P24"       },
        {  "LCD_P25",            "PTC5",  "SPI0_SCK",  "?",          "LPT0_ALT2",   "?",       "CMP0_OUT", "LCD_P25"       },
        {  "LCD_P26/CMP0_IN0",     "PTC6",  "SPI0_SOUT", "PDB0_EXTRG", "?",           "?",     "?",        "LCD_P26"       },
        {  "LCD_P27/CMP0_IN1",     "PTC7",  "SPI0_SIN",  "?",          "?",           "?",     "?",        "LCD_P27"       },
        {  "LCD_P28/ADC1_SE4/CMP0_IN2","PTC8","?",       "I2S0_MCLK",  "I2S0_CLKIN",  "?",     "?",        "LCD_P28"       },
        {  "LCD_P29/ADC1_SE5/CMP0_IN3","PTC9","?",       "?",          "I2S0_RX_BCLK","?",     "FTM2_FLT0","LCD_P29"       },
        {  "LCD_P30/ADC1_SE6/CMP0_IN4","PTC10","I2C1_SCL","?",         "I2S0_RX_FS",  "?",     "?",        "LCD_P30"       },
        {  "LCD_P31/ADC1_SE7",     "PTC11", "I2C1_SDA",  "?",          "I2S0_RXD",    "?",     "?",        "LCD_P31"       },
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
        {  "?",            "?",     "?",         "?",          "?",           "?",         "?",        "?"                 },
        {  "?",            "?",     "?",         "?",          "?",           "?",         "?",        "?"                 },
        {  "?",            "?",     "?",         "?",          "?",           "?",         "?",        "?"                 },
        {  "?",            "?",     "?",         "?",          "?",           "?",         "?",        "?"                 },
        {  "?",            "?",     "?",         "?",          "?",           "?",         "?",        "?"                 }
    },
    {
        // ALT 0           ALT 1    ALT2         ALT 3         ALT 4          ALT 5        ALT 6       ALT 7
        {  "LCD_P40",      "PTD0",  "SPI0_PCS0", "UART2_RTS",  "?",           "?",         "?",        "LCD_P40"           }, // PORT D
        {  "LCD_P41/ADC0_SE5","PTD1", "SPI0_SCK","UART2_CTS",  "?",           "?",         "?",        "LCD_P41"           },
        {  "LCD_P42",      "PTD2",  "SPI0_SOUT", "UART2_RX",   "?",           "?",         "?",        "LCD_P42"           },
        {  "LCD_P43",      "PTD3",  "SPI0_SIN",  "UART2_TX",   "?",           "?",         "?",        "LCD_P43"           },
        {  "LCD_P44",      "PTD4",  "SPI0_PCS1", "UART0_RTS",  "FTM0_CH4",    "?",         "EWM_IN",   "LCD_P44"           },
        {  "LCD_P45/ADC0_SE6", "PTD5",  "SPI0_PCS2", "UART0_CTS",  "FTM0_CH5","?",         "EWM_OUT_b","LCD_P45"           },
        {  "LCD_P46/ADC0_SE7", "PTD6",  "SPI0_PCS3", "UART0_RX",   "FTM0_CH6","?",         "FTM0_FLT0","LCD_P46"           },
        {  "LCD_P47",      "PTD7",  "CMT_IRO",   "UART0_TX",   "FTM0_CH7",    "?",         "FTM0_FLT1","LCD_P47"           },
        {  "?",            "?",     "?",         "?",          "?",           "?",         "?",        "?"                 },
        {  "?",            "?",     "?",         "?",          "?",           "?",         "?",        "?"                 },
        {  "D",            "PTD10", "?",         "?",          "?",           "?",         "?",        "?"                 }, // only 81 MAPBGA
        {  "D",            "PTD11", "SPI2_PCS0", "?",          "SDHC0_CLKIN", "?",         "?",        "?"                 }, // only 81 MAPBGA
        {  "D",            "PTD12", "SPI2_SCK",  "?",          "SDHC0_D4",    "?",         "?",        "?"                 }, // only 81 MAPBGA
        {  "D",            "PTD13", "SPI2_SOUT", "?",          "SDHC0_D5",    "?",         "?",        "?"                 }, // only 81 MAPBGA
        {  "D",            "PTD14", "SPI2_SIN",  "?",          "SDHC0_D6",    "?",         "?",        "?"                 }, // only 81 MAPBGA
        {  "D",            "PTD15", "SPI2_PCS1", "?",          "SDHC0_D7",    "?",         "?",        "?"                 }, // only 81 MAPBGA
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
        {  "D",            "PTE4",  "SPI1_PCS0", "UART3_TX",   "SDHC0_D3",    "?",         "?",        "?"                 },
        {  "D",            "PTE5",  "SPI1_PCS2", "UART3_RX",   "SDHC0_D2",    "?",         "?",        "?"                 },
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
    }
};
#elif defined DEVICE_100_PIN
static const char *cPer[PORTS_AVAILABLE][PORT_WIDTH][8] = {              // 100 pin LQFP
    {
        // ALT 0           ALT 1    ALT2         ALT 3         ALT 4          ALT 5        ALT 6       ALT 7
        {  "TSI0_CH1",     "PTA0",  "UART0_CTS", "FTM0_CH5",   "?",           "?",         "?",        "JTAG_TCLK/SWD_CLK" }, // PORT A
        {  "TSI0_CH2",     "PTA1",  "UART0_RX",  "FTM0_CH6",   "?",           "?",         "?",        "JTAG_TDI"          },
        {  "TSI0_CH3",     "PTA2",  "UART0_TX",  "FTM0_CH7",   "?",           "?",         "?",        "JTAG_TDI/TRACE_SWO"},
        {  "TSI0_CH4",     "PTA3",  "UART0_RTS", "FTM0_CH0",   "?",           "?",         "?",        "JTAG_TMS/SWD_DIO"  },
        {  "TSI0_CH5",     "PTA4",  "?",         "FTM0_CH1",   "?",           "?",         "?",        "NMI"               },
        {  "D",            "PTA5",  "?",         "FTM0_CH2",   "?",           "CMP2_OUT",  "I2S0_RX_BCLK","JTAG_TRST"      },
        {  "?",            "?",     "?",         "?",          "?",           "?",         "?",        "?"                 },
        {  "?",            "?",     "?",         "?",          "?",           "?",         "?",        "?"                 },
        {  "?",            "?",     "?",         "?",          "?",           "?",         "?",        "?"                 },
        {  "?",            "?",     "?",         "?",          "?",           "?",         "?",        "?"                 },
        {  "?",            "?",     "?",         "?",          "?",           "?",         "?",        "?"                 },
        {  "?",            "?",     "?",         "?",          "?",           "?",         "?",        "?"                 },
        {  "CMP2_IN0",     "PTA12", "CAN0_TX",   "FTM1_CH0",   "?",           "?",         "I2S0_TXD", "FTM1_QD_PHA"       },
        {  "CMP2_IN1",     "PTA13", "CAN0_RX",   "FTM1_CH1",   "?",           "?",         "I2S0_TX_FS","FTM1_QD_PHB"      },
        {  "D",            "PTA14", "SPI0_PCS0", "UART0_TX",   "?",           "?",         "I2S0_TX_BCLK","?"              },
        {  "D",            "PTA15", "SPI0_SCK",  "UART0_RX",   "?",           "?",         "I2S0_RXD", "?"                 },
        {  "D",            "PTA16", "SPI0_SOUT", "UART0_CTS",  "?",           "?",         "I2S0_RX_FS","?"                },
        {  "ADC1_SE17",    "PTA17", "SPI0_SIN",  "UART0_RTS",  "?",           "?",         "I2S0_MCLK","I2S0_CLKIN"        },
        {  "EXTAL",        "PTA18", "?",         "FTM0_FLT2",  "FTM_CLKIN0",  "?",         "?",        "?"                 },
        {  "XTAL",         "PTA19", "?",         "FTM1_FLT0",  "FTM_CLKIN1",  "?",         "LPT0_ALT1","?"                 },
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
        {  "LCD_P0/ADC0_SE8/TSI0_CH0","PTB0","I2C0_SCL","FTM1_CH0",   "?",    "?",         "FTM1_QD_PHA", "LCD_P0"         }, // PORT B
        {  "LCD_P1/ADC0_SE9/TSI0_CH6","PTB1","I2C0_SDA","FTM1_CH1",   "?",    "?",         "FTM1_QD_PHB", "LCD_P1"         },
        {  "LCD_P2/ADC0_SE12/TSI0_CH7","PTB2","I2C0_SCL","UART0_RTS", "?",    "?",         "FTM0_FLT3",   "LCD_P2"         },
        {  "LCD_P3/ADC0_SE13/TSI0_CH8","PTB3","I2C0_SDA","UART0_CTS", "?",    "?",         "FTM0_FLT0",   "LCD_P3"         },
        {  "?",            "?",     "?",         "?",          "?",           "?",         "?",        "?"                 },
        {  "?",            "?",     "?",         "?",          "?",           "?",         "?",        "?"                 },
        {  "?",            "?",     "?",         "?",          "?",           "?",         "?",        "?"                 },
        {  "LCD_P7/ADC1_SE13", "PTB7","?",       "?",          "?",           "?",         "?",           "LCD_P7"         },
        {  "LCD_P8",       "PTB8",  "?",         "UART3_RTS",  "?",           "?",         "?",           "LCD_P8"         },
        {  "LCD_P9",       "PTB9",  "SPI1_PCS1", "UART3_CTS",  "?",           "?",         "?",           "LCD_P9"         },
        {  "LCD_P10/ADC1_SE14", "PTB10", "SPI1_PCS0", "UART3_RX",   "?",      "?",         "FTM0_FLT1",   "LCD_P10"        },
        {  "LCD_P11/ADC1_SE15", "PTB11", "SPI1_SCK",  "UART3_TX",   "?",      "?",         "FTM0_FLT2",   "LCD_P11"        },
        {  "?",            "?",     "?",         "?",          "?",           "?",         "?",        "?"                 },
        {  "?",            "?",     "?",         "?",          "?",           "?",         "?",        "?"                 },
        {  "?",            "?",     "?",         "?",          "?",           "?",         "?",        "?"                 },
        {  "?",            "?",     "?",         "?",          "?",           "?",         "?",        "?"                 },
        {  "LCD_P12/TSI0_CH9",  "PTB16", "SPI1_SOUT", "UART0_RX",   "?",      "?",         "EWM_IN",      "LCD_P12"        },
        {  "LCD_P13/TSI0_CH10", "PTB17", "SPI1_SIN",  "UART0_TX",   "?",      "?",         "EWM_OUT",     "LCD_P13"        },
        {  "LCD_P14/TSI0_CH11", "PTB18", "CAN0_TX",   "FTM2_CH0",   "I2S0_TX_BCLK",  "?",  "FTM2_QD_PHA", "LCD_P14"        },
        {  "LCD_P15/TSI0_CH12", "PTB19", "CAN0_RX",   "FTM2_CH1",   "I2S0_TX_FS",    "?",  "FTM2_QD_PHB", "LCD_P15"        },
        {  "LCD_P16",      "PTB20", "SPI2_PCS0", "?",          "?",           "?",         "CMP0_OUT",    "LCD_P16"        },
        {  "LCD_P17",      "PTB21", "SPI2_SCK",  "?",          "?",           "?",         "CMP1_OUT",    "LCD_P17"        },
        {  "LCD_P18",      "PTB22", "SPI2_SOUT", "?",          "?",           "?",         "CMP2_OUT",    "LCD_P18"        },
        {  "LCD_P19",      "PTB23", "SPI2_SIN",  "SPI0_PCS5",  "?",           "?",         "?",           "LCD_P19"        },
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
        {  "LCD_P20/ADC0_SE14/TSI0_CH13","PTC0","SPI0_PCS4","PDB0_EXTRG","I2S0_TXD",  "?", "?",        "LCD_P20"           }, // PORT C
        {  "LCD_P21/ADC0_SE15/TSI0_CH14","PTC1","SPI0_PCS3","UART1_RTS","FTM0_CH0",   "?",     "?",        "LCD_P21"       },
        {  "LCD_P22/ADC0_SE4/CMP1_IN0TSI0_CH15","PTC2","SPI0_PCS2","UART1_CTS","FTM0_CH1","?", "?",        "LCD_P22"       },
        {  "LCD_P23/CMP1_IN1",     "PTC3",  "SPI0_PCS1", "UART1_RX",   "FTM0_CH2",    "?",     "?",        "LCD_P23"       },
        {  "LCD_P24",            "PTC4",  "SPI0_PCS0", "UART1_TX",   "FTM0_CH3",    "?",       "CMP1_OUT", "LCD_P24"       },
        {  "LCD_P25",            "PTC5",  "SPI0_SCK",  "?",          "LPT0_ALT2",   "?",       "CMP0_OUT", "LCD_P25"       },
        {  "LCD_P26/CMP0_IN0",     "PTC6",  "SPI0_SOUT", "PDB0_EXTRG", "?",           "?",     "?",        "LCD_P26"       },
        {  "LCD_P27/CMP0_IN1",     "PTC7",  "SPI0_SIN",  "?",          "?",           "?",     "?",        "LCD_P27"       },
        {  "LCD_P28/ADC1_SE4/CMP0_IN2","PTC8","?",       "I2S0_MCLK",  "I2S0_CLKIN",  "?",     "?",        "LCD_P28"       },
        {  "LCD_P29/ADC1_SE5/CMP0_IN3","PTC9","?",       "?",          "I2S0_RX_BCLK","?",     "FTM2_FLT0","LCD_P29"       },
        {  "LCD_P30/ADC1_SE6/CMP0_IN4","PTC10","I2C1_SCL","?",         "I2S0_RX_FS",  "?",     "?",        "LCD_P30"       },
        {  "LCD_P31/ADC1_SE7",     "PTC11", "I2C1_SDA",  "?",          "I2S0_RXD",    "?",     "?",        "LCD_P31"       },
        {  "?",            "?",     "?",         "?",          "?",           "?",         "?",        "?"                 },
        {  "?",            "?",     "?",         "?",          "?",           "?",         "?",        "?"                 },
        {  "?",            "?",     "?",         "?",          "?",           "?",         "?",        "?"                 },
        {  "?",            "?",     "?",         "?",          "?",           "?",         "?",        "?"                 },
        {  "LCD_P36",      "PTC16", "CAN1_RX",   "UART3_RX",   "?",           "?",         "?",        "LCD_P36"           },
        {  "LCD_P37",      "PTC17", "CAN1_TX",   "UART3_TX",   "?",           "?",         "?",        "LCD_P37"           },
        {  "LCD_P38",      "PTC18", "?",         "UART3_RTS",  "?",           "?",         "?",        "LCD_P38"           },
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
        {  "LCD_P40",      "PTD0",  "SPI0_PCS0", "UART2_RTS",  "?",           "?",         "?",        "LCD_P40"           }, // PORT D
        {  "LCD_P41/ADC0_SE5","PTD1", "SPI0_SCK","UART2_CTS",  "?",           "?",         "?",        "LCD_P41"           },
        {  "LCD_P42",      "PTD2",  "SPI0_SOUT", "UART2_RX",   "?",           "?",         "?",        "LCD_P42"           },
        {  "LCD_P43",      "PTD3",  "SPI0_SIN",  "UART2_TX",   "?",           "?",         "?",        "LCD_P43"           },
        {  "LCD_P44",      "PTD4",  "SPI0_PCS1", "UART0_RTS",  "FTM0_CH4",    "?",         "EWM_IN",   "LCD_P44"           },
        {  "LCD_P45/ADC0_SE6", "PTD5",  "SPI0_PCS2", "UART0_CTS",  "FTM0_CH5","?",         "EWM_OUT_b","LCD_P45"           },
        {  "LCD_P46/ADC0_SE7", "PTD6",  "SPI0_PCS3", "UART0_RX",   "FTM0_CH6","?",         "FTM0_FLT0","LCD_P46"           },
        {  "LCD_P47",      "PTD7",  "CMT_IRO",   "UART0_TX",   "FTM0_CH7",    "?",         "FTM0_FLT1","LCD_P47"           },
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
        {  "D",            "PTE4",  "SPI1_PCS0", "UART3_TX",   "SDHC0_D3",    "?",         "?",        "?"                 },
        {  "D",            "PTE5",  "SPI1_PCS2", "UART3_RX",   "SDHC0_D2",    "?",         "?",        "?"                 },
        {  "D",            "PTE6",  "SPI1_PCS3", "UART3_CTS",  "I2S0_MCLK",   "?",         "I2S0_CLKIN","?"                },
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
        {  "?",            "?",     "?",         "?",          "?",           "?",         "?",        "?"                 },
        {  "?",            "?",     "?",         "?",          "?",           "?",         "?",        "?"                 },
        {  "ADC0_SE17",    "PTE24", "CAN1_TX",   "UART4_TX",   "?",           "?",         "EWM_OUT_b","?"                 },
        {  "ADC0_SE18",    "PTE25", "CAN1_RX",   "UART4_RX",   "?",           "?",         "EWM_IN",   "?"                 },
        {  "D",            "PTE26", "?",         "UART4_CTS_b","?",           "?",         "RTC_CLKOUT","USB_CLKIN"        },
        {  "?",            "?",     "?",         "?",          "?",           "?",         "?",        "?"                 },
        {  "?",            "?",     "?",         "?",          "?",           "?",         "?",        "?"                 },
        {  "?",            "?",     "?",         "?",          "?",           "?",         "?",        "?"                 },
        {  "?",            "?",     "?",         "?",          "?",           "?",         "?",        "?"                 },
        {  "?",            "?",     "?",         "?",          "?",           "?",         "?",        "?"                 }
    }
};
#elif defined DEVICE_121_PIN
static const char *cPer[PORTS_AVAILABLE][PORT_WIDTH][8] = {              // 121 pin MAPBGA
    {
        // ALT 0           ALT 1    ALT2         ALT 3         ALT 4          ALT 5        ALT 6       ALT 7
        {  "TSI0_CH1",     "PTA0",  "UART0_CTS", "FTM0_CH5",   "?",           "?",         "?",        "JTAG_TCLK/SWD_CLK" }, // PORT A
        {  "TSI0_CH2",     "PTA1",  "UART0_RX",  "FTM0_CH6",   "?",           "?",         "?",        "JTAG_TDI"          },
        {  "TSI0_CH3",     "PTA2",  "UART0_TX",  "FTM0_CH7",   "?",           "?",         "?",        "JTAG_TDI/TRACE_SWO"},
        {  "TSI0_CH4",     "PTA3",  "UART0_RTS", "FTM0_CH0",   "?",           "?",         "?",        "JTAG_TMS/SWD_DIO"  },
        {  "TSI0_CH5",     "PTA4",  "?",         "FTM0_CH1",   "?",           "?",         "?",        "NMI"               },
        {  "D",            "PTA5",  "?",         "FTM0_CH2",   "?",           "CMP2_OUT",  "I2S0_RX_BCLK","JTAG_TRST"      },
        {  "?",            "?",     "?",         "?",          "?",           "?",         "?",        "?"                 },
        {  "?",            "?",     "?",         "?",          "?",           "?",         "?",        "?"                 },
        {  "?",            "?",     "?",         "?",          "?",           "?",         "?",        "?"                 },
        {  "?",            "?",     "?",         "?",          "?",           "?",         "?",        "?"                 },
        {  "D",            "PTA10", "?",         "FTM2_CH0",   "?",           "?",         "FTM2_QD_PHA","TRACE_D0"        },
        {  "D",            "PTA11", "?",         "FTM2_CH1",   "?",           "?",         "FTM2_QD_PHB","?"               },
        {  "CMP2_IN0",     "PTA12", "CAN0_TX",   "FTM1_CH0",   "?",           "?",         "I2S0_TXD", "FTM1_QD_PHA"       },
        {  "CMP2_IN1",     "PTA13", "CAN0_RX",   "FTM1_CH1",   "?",           "?",         "I2S0_TX_FS","FTM1_QD_PHB"      },
        {  "D",            "PTA14", "SPI0_PCS0", "UART0_TX",   "?",           "?",         "I2S0_TX_BCLK","?"              },
        {  "D",            "PTA15", "SPI0_SCK",  "UART0_RX",   "?",           "?",         "I2S0_RXD", "?"                 },
        {  "D",            "PTA16", "SPI0_SOUT", "UART0_CTS",  "?",           "?",         "I2S0_RX_FS","?"                },
        {  "ADC1_SE17",    "PTA17", "SPI0_SIN",  "UART0_RTS",  "?",           "?",         "I2S0_MCLK","I2S0_CLKIN"        },
        {  "EXTAL",        "PTA18", "?",         "FTM0_FLT2",  "FTM_CLKIN0",  "?",         "?",        "?"                 },
        {  "XTAL",         "PTA19", "?",         "FTM1_FLT0",  "FTM_CLKIN1",  "?",         "LPT0_ALT1","?"                 },
        {  "?",            "?",     "?",         "?",          "?",           "?",         "?",        "?"                 },
        {  "?",            "?",     "?",         "?",          "?",           "?",         "?",        "?"                 },
        {  "?",            "?",     "?",         "?",          "?",           "?",         "?",        "?"                 },
        {  "?",            "?",     "?",         "?",          "?",           "?",         "?",        "?"                 },
        {  "?",            "?",     "?",         "?",          "?",           "?",         "?",        "?"                 },
        {  "?",            "?",     "?",         "?",          "?",           "?",         "?",        "?"                 },
        {  "?",            "?",     "?",         "?",          "?",           "?",         "?",        "?"                 },
        {  "?",            "?",     "?",         "?",          "?",           "?",         "?",        "?"                 },
        {  "?",            "?",     "?",         "?",          "?",           "?",         "?",        "?"                 },
        {  "D",            "PTA29", "?",         "?",          "?",           "?",         "?",        "?"                 },
        {  "?",            "?",     "?",         "?",          "?",           "?",         "?",        "?"                 },
        {  "?",            "?",     "?",         "?",          "?",           "?",         "?",        "?"                 }
    },
    {
        // ALT 0           ALT 1    ALT2         ALT 3         ALT 4          ALT 5        ALT 6       ALT 7
        {  "LCD_P0/ADC0_SE8/TSI0_CH0","PTB0","I2C0_SCL","FTM1_CH0",   "?",    "?",         "FTM1_QD_PHA", "LCD_P0"         }, // PORT B
        {  "LCD_P1/ADC0_SE9/TSI0_CH6","PTB1","I2C0_SDA","FTM1_CH1",   "?",    "?",         "FTM1_QD_PHB", "LCD_P1"         },
        {  "LCD_P2/ADC0_SE12/TSI0_CH7","PTB2","I2C0_SCL","UART0_RTS", "?",    "?",         "FTM0_FLT3",   "LCD_P2"         },
        {  "LCD_P3/ADC0_SE13/TSI0_CH8","PTB3","I2C0_SDA","UART0_CTS", "?",    "?",         "FTM0_FLT0",   "LCD_P3"         },
        {  "?",            "?",     "?",         "?",          "?",           "?",         "?",        "?"                 },
        {  "?",            "?",     "?",         "?",          "?",           "?",         "?",        "?"                 },
        {  "LCD_P6/ADC1_SE12", "PTB6","?",       "?",          "?",           "?",         "?",           "LCD_P6"         },
        {  "LCD_P7/ADC1_SE13", "PTB7","?",       "?",          "?",           "?",         "?",           "LCD_P7"         },
        {  "LCD_P8",       "PTB8",  "?",         "UART3_RTS",  "?",           "?",         "?",           "LCD_P8"         },
        {  "LCD_P9",       "PTB9",  "SPI1_PCS1", "UART3_CTS",  "?",           "?",         "?",           "LCD_P9"         },
        {  "LCD_P10/ADC1_SE14", "PTB10", "SPI1_PCS0", "UART3_RX",   "?",      "?",         "FTM0_FLT1",   "LCD_P10"        },
        {  "LCD_P11/ADC1_SE15", "PTB11", "SPI1_SCK",  "UART3_TX",   "?",      "?",         "FTM0_FLT2",   "LCD_P11"        },
        {  "?",            "?",     "?",         "?",          "?",           "?",         "?",        "?"                 },
        {  "?",            "?",     "?",         "?",          "?",           "?",         "?",        "?"                 },
        {  "?",            "?",     "?",         "?",          "?",           "?",         "?",        "?"                 },
        {  "?",            "?",     "?",         "?",          "?",           "?",         "?",        "?"                 },
        {  "LCD_P12/TSI0_CH9",  "PTB16", "SPI1_SOUT", "UART0_RX",   "?",      "?",         "EWM_IN",      "LCD_P12"        },
        {  "LCD_P13/TSI0_CH10", "PTB17", "SPI1_SIN",  "UART0_TX",   "?",      "?",         "EWM_OUT",     "LCD_P13"        },
        {  "LCD_P14/TSI0_CH11", "PTB18", "CAN0_TX",   "FTM2_CH0",   "I2S0_TX_BCLK",  "?",  "FTM2_QD_PHA", "LCD_P14"        },
        {  "LCD_P15/TSI0_CH12", "PTB19", "CAN0_RX",   "FTM2_CH1",   "I2S0_TX_FS",    "?",  "FTM2_QD_PHB", "LCD_P15"        },
        {  "LCD_P16",      "PTB20", "SPI2_PCS0", "?",          "?",           "?",         "CMP0_OUT",    "LCD_P16"        },
        {  "LCD_P17",      "PTB21", "SPI2_SCK",  "?",          "?",           "?",         "CMP1_OUT",    "LCD_P17"        },
        {  "LCD_P18",      "PTB22", "SPI2_SOUT", "?",          "?",           "?",         "CMP2_OUT",    "LCD_P18"        },
        {  "LCD_P19",      "PTB23", "SPI2_SIN",  "SPI0_PCS5",  "?",           "?",         "?",           "LCD_P19"        },
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
        {  "LCD_P20/ADC0_SE14/TSI0_CH13","PTC0","SPI0_PCS4","PDB0_EXTRG","I2S0_TXD",  "?", "?",        "LCD_P20"           }, // PORT C
        {  "LCD_P21/ADC0_SE15/TSI0_CH14","PTC1","SPI0_PCS3","UART1_RTS","FTM0_CH0",   "?",     "?",        "LCD_P21"       },
        {  "LCD_P22/ADC0_SE4/CMP1_IN0TSI0_CH15","PTC2","SPI0_PCS2","UART1_CTS","FTM0_CH1","?", "?",        "LCD_P22"       },
        {  "LCD_P23/CMP1_IN1",     "PTC3",  "SPI0_PCS1", "UART1_RX",   "FTM0_CH2",    "?",     "?",        "LCD_P23"       },
        {  "LCD_P24",            "PTC4",  "SPI0_PCS0", "UART1_TX",   "FTM0_CH3",    "?",       "CMP1_OUT", "LCD_P24"       },
        {  "LCD_P25",            "PTC5",  "SPI0_SCK",  "?",          "LPT0_ALT2",   "?",       "CMP0_OUT", "LCD_P25"       },
        {  "LCD_P26/CMP0_IN0",     "PTC6",  "SPI0_SOUT", "PDB0_EXTRG", "?",           "?",     "?",        "LCD_P26"       },
        {  "LCD_P27/CMP0_IN1",     "PTC7",  "SPI0_SIN",  "?",          "?",           "?",     "?",        "LCD_P27"       },
        {  "LCD_P28/ADC1_SE4/CMP0_IN2","PTC8","?",       "I2S0_MCLK",  "I2S0_CLKIN",  "?",     "?",        "LCD_P28"       },
        {  "LCD_P29/ADC1_SE5/CMP0_IN3","PTC9","?",       "?",          "I2S0_RX_BCLK","?",     "FTM2_FLT0","LCD_P29"       },
        {  "LCD_P30/ADC1_SE6/CMP0_IN4","PTC10","I2C1_SCL","?",         "I2S0_RX_FS",  "?",     "?",        "LCD_P30"       },
        {  "LCD_P31/ADC1_SE7",     "PTC11", "I2C1_SDA",  "?",          "I2S0_RXD",    "?",     "?",        "LCD_P31"       },
        {  "LCD_P32",      "PTC12", "?",         "UART4_RTS",  "?",           "?",         "?",        "LCD_P32"           },
        {  "LCD_P33",      "PTC13", "?",         "UART4_CTS",  "?",           "?",         "?",        "LCD_P33"           },
        {  "LCD_P34",      "PTC14", "?",         "UART4_RX",   "?",           "?",         "?",        "LCD_P34"           },
        {  "LCD_P35",      "PTC15", "?",         "UART4_TX",   "?",           "?",         "?",        "LCD_P35"           },
        {  "LCD_P36",      "PTC16", "CAN1_RX",   "UART3_RX",   "?",           "?",         "?",        "LCD_P36"           },
        {  "LCD_P37",      "PTC17", "CAN1_TX",   "UART3_TX",   "?",           "?",         "?",        "LCD_P37"           },
        {  "LCD_P38",      "PTC18", "?",         "UART3_RTS",  "?",           "?",         "?",        "LCD_P38"           },
        {  "LCD_P39",      "PTC19", "?",         "UART3_CTS",  "?",           "?",         "?",        "LCD_P39"           },
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
        {  "LCD_P40",      "PTD0",  "SPI0_PCS0", "UART2_RTS",  "?",           "?",         "?",        "LCD_P40"           }, // PORT D
        {  "LCD_P41/ADC0_SE5","PTD1", "SPI0_SCK","UART2_CTS",  "?",           "?",         "?",        "LCD_P41"           },
        {  "LCD_P42",      "PTD2",  "SPI0_SOUT", "UART2_RX",   "?",           "?",         "?",        "LCD_P42"           },
        {  "LCD_P43",      "PTD3",  "SPI0_SIN",  "UART2_TX",   "?",           "?",         "?",        "LCD_P43"           },
        {  "LCD_P44",      "PTD4",  "SPI0_PCS1", "UART0_RTS",  "FTM0_CH4",    "?",         "EWM_IN",   "LCD_P44"           },
        {  "LCD_P45/ADC0_SE6", "PTD5",  "SPI0_PCS2", "UART0_CTS",  "FTM0_CH5","?",         "EWM_OUT_b","LCD_P45"           },
        {  "LCD_P46/ADC0_SE7", "PTD6",  "SPI0_PCS3", "UART0_RX",   "FTM0_CH6","?",         "FTM0_FLT0","LCD_P46"           },
        {  "LCD_P47",      "PTD7",  "CMT_IRO",   "UART0_TX",   "FTM0_CH7",    "?",         "FTM0_FLT1","LCD_P47"           },
        {  "?",            "?",     "?",         "?",          "?",           "?",         "?",        "?"                 },
        {  "?",            "?",     "?",         "?",          "?",           "?",         "?",        "?"                 },
        {  "D",            "PTD10", "?",         "UART5_RTS_b","?",           "?",         "?",        "?"                 },
        {  "D",            "PTD11", "SPI2_PCS0", "UART5_CTS_b","SDHC0_CLKIN", "?",         "?",        "?"                 },
        {  "D",            "PTD12", "SPI2_SCK",  "?",          "SDHC0_D4",    "?",         "?",        "?"                 },
        {  "D",            "PTD13", "SPI2_SOUT", "?",          "SDHC0_D5",    "?",         "?",        "?"                 },
        {  "D",            "PTD14", "SPI2_SIN",  "?",          "SDHC0_D6",    "?",         "?",        "?"                 },
        {  "D",            "PTD15", "SPI2_PCS1", "?",          "SDHC0_D7",    "?",         "?",        "?"                 },
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
        {  "D",            "PTE4",  "SPI1_PCS0", "UART3_TX",   "SDHC0_D3",    "?",         "?",        "?"                 },
        {  "D",            "PTE5",  "SPI1_PCS2", "UART3_RX",   "SDHC0_D2",    "?",         "?",        "?"                 },
        {  "D",            "PTE6",  "SPI1_PCS3", "UART3_CTS",  "I2S0_MCLK",   "?",         "I2S0_CLKIN","?"                },
        {  "?",            "?",     "?",         "?",          "?",           "?",         "?",        "?"                 },
        {  "?",            "?",     "?",         "?",          "?",           "?",         "?",        "?"                 },
        {  "?",            "?",     "?",         "?",          "?",           "?",         "?",        "?"                 },
        {  "?",            "?",     "?",         "?",          "?",           "?",         "?",        "?"                 },
        {  "?",            "?",     "?",         "?",          "?",           "?",         "?",        "?"                 },
        {  "?",            "?",     "?",         "?",          "?",           "?",         "?",        "?"                 },
        {  "?",            "?",     "?",         "?",          "?",           "?",         "?",        "?"                 },
        {  "?",            "?",     "?",         "?",          "?",           "?",         "?",        "?"                 },
        {  "?",            "?",     "?",         "?",          "?",           "?",         "?",        "?"                 },
        {  "ADC0_SE4a",    "PTE16", "SPI0_PCS0", "UART2_TX",   "FTM_CLKIN0",  "?",         "FTM0_FLT3","?"                 },
        {  "ADC0_SE5a",    "PTE17", "SPI0_SCK",  "UART2_RX",   "FTM_CLKIN1",  "?",         "LPT0_ALT3","?"                 },
        {  "ADC0_SE6a",    "PTE18", "SPI0_SOUT", "UART2_CTS_b","I2C0_SDA",    "?",         "?",        "?"                 },
        {  "ADC0_SE7a",    "PTE19", "SPI0_SIN",  "UART2_RTS_b","I2C_SCL",     "?",         "?",        "?"                 },
        {  "?",            "?",     "?",         "?",          "?",           "?",         "?",        "?"                 },
        {  "?",            "?",     "?",         "?",          "?",           "?",         "?",        "?"                 },
        {  "?",            "?",     "?",         "?",          "?",           "?",         "?",        "?"                 },
        {  "?",            "?",     "?",         "?",          "?",           "?",         "?",        "?"                 },
        {  "ADC0_SE17",    "PTE24", "CAN1_TX",   "UART4_TX",   "?",           "?",         "EWM_OUT_b","?"                 },
        {  "ADC0_SE18",    "PTE25", "CAN1_RX",   "UART4_RX",   "?",           "?",         "EWM_IN",   "?"                 },
        {  "D",            "PTE26", "?",         "UART4_CTS_b","?",           "?",         "RTC_CLKOUT","USB_CLKIN"        },
        {  "?",            "?",     "?",         "?",          "?",           "?",         "?",        "?"                 },
        {  "?",            "?",     "?",         "?",          "?",           "?",         "?",        "?"                 },
        {  "?",            "?",     "?",         "?",          "?",           "?",         "?",        "?"                 },
        {  "?",            "?",     "?",         "?",          "?",           "?",         "?",        "?"                 },
        {  "?",            "?",     "?",         "?",          "?",           "?",         "?",        "?"                 }
    }
};
#else
static const char *cPer[PORTS_AVAILABLE][PORT_WIDTH][8] = {              // 144 pin
    {
        // ALT 0           ALT 1    ALT2         ALT 3         ALT 4          ALT 5        ALT 6       ALT 7
        {  "TSI0_CH1",     "PTA0",  "UART0_CTS", "FTM0_CH5",   "?",           "?",         "?",        "JTAG_TCLK/SWD_CLK" }, // PORT A
        {  "TSI0_CH2",     "PTA1",  "UART0_RX",  "FTM0_CH6",   "?",           "?",         "?",        "JTAG_TDI"          },
        {  "TSI0_CH3",     "PTA2",  "UART0_TX",  "FTM0_CH7",   "?",           "?",         "?",        "JTAG_TDI/TRACE_SWO"},
        {  "TSI0_CH4",     "PTA3",  "UART0_RTS", "FTM0_CH0",   "?",           "?",         "?",        "JTAG_TMS/SWD_DIO"  },
        {  "TSI0_CH5",     "PTA4",  "?",         "FTM0_CH1",   "?",           "?",         "?",        "NMI"               },
        {  "D",            "PTA5",  "?",         "FTM0_CH2",   "?",           "CMP2_OUT",  "I2S0_RX_BCLK","JTAG_TRST"      },
        {  "D",            "PTA6",  "?",         "FTM0_CH3",   "?",           "FB_CLKOUT", "?",        "TRACE_CLKOUT"      },
        {  "ADC0_SE10",    "PTA7",  "?",         "FTM0_CH4",   "?",           "FB_AD18",   "?",        "TRACE_D3"          },
        {  "ADC0_SE11",    "PTA8",  "?",         "FTM1_CH0",   "?",           "FB_AD17",   "FTM1_QD_PHA","TRACE_D2"        },
        {  "D",            "PTA9",  "?",         "FTM1_CH1",   "?",           "FB_AD16",   "FTM1_QD_PHB","TRACE_D1"        },
        {  "D",            "PTA10", "?",         "FTM2_CH0",   "?",           "FB_AD15",   "FTM2_QD_PHA","TRACE_D0"        },
        {  "D",            "PTA11", "?",         "FTM2_CH1",   "?",           "FB_OE_b",   "FTM2_QD_PHB","?"               },
        {  "CMP2_IN0",     "PTA12", "CAN0_TX",   "FTM1_CH0",   "?",           "FB_CS5_b/FB_TSIZ1/FB_BE23_16_BLS15_8_b","I2S0_TXD", "FTM1_QD_PHA"       },
        {  "CMP2_IN1",     "PTA13", "CAN0_RX",   "FTM1_CH1",   "?",           "FB_CS4_b/FB_TSIZ0/FB_BE31_24_BLS7_0_b", "I2S0_TX_FS","FTM1_QD_PHB"      },
        {  "D",            "PTA14", "SPI0_PCS0", "UART0_TX",   "?",           "FB_AD31",   "I2S0_TX_BCLK","?"              },
        {  "D",            "PTA15", "SPI0_SCK",  "UART0_RX",   "?",           "FB_AD30",   "I2S0_RXD", "?"                 },
        {  "D",            "PTA16", "SPI0_SOUT", "UART0_CTS",  "?",           "FB_AD29",   "I2S0_RX_FS","?"                },
        {  "ADC1_SE17",    "PTA17", "SPI0_SIN",  "UART0_RTS",  "?",           "FB_AD28",   "I2S0_MCLK","I2S0_CLKIN"        },
        {  "EXTAL",        "PTA18", "?",         "FTM0_FLT2",  "FTM_CLKIN0",  "?",         "?",        "?"                 },
        {  "XTAL",         "PTA19", "?",         "FTM1_FLT0",  "FTM_CLKIN1",  "?",         "LPT0_ALT1","?"                 },
        {  "?",            "?",     "?",         "?",          "?",           "?",         "?",        "?"                 },
        {  "?",            "?",     "?",         "?",          "?",           "?",         "?",        "?"                 },
        {  "?",            "?",     "?",         "?",          "?",           "?",         "?",        "?"                 },
        {  "?",            "?",     "?",         "?",          "?",           "?",         "?",        "?"                 },
        {  "D",            "PTA24", "?",         "?",          "?",           "FB_AD14",   "?",        "?"                 },
        {  "D",            "PTA25", "?",         "?",          "?",           "FB_AD13",   "?",        "?"                 },
        {  "D",            "PTA26", "?",         "?",          "?",           "FB_AD12",   "?",        "?"                 },                
        {  "D",            "PTA27", "?",         "?",          "?",           "FB_AD11",   "?",        "?"                 },
        {  "D",            "PTA28", "?",         "?",          "?",           "FB_AD10",   "?",        "?"                 },
        {  "D",            "PTA29", "?",         "?",          "?",           "FB_AD19",   "?",        "?"                 },
        {  "?",            "?",     "?",         "?",          "?",           "?",         "?",        "?"                 },
        {  "?",            "?",     "?",         "?",          "?",           "?",         "?",        "?"                 }
    },
    {
        // ALT 0           ALT 1    ALT2         ALT 3         ALT 4          ALT 5        ALT 6       ALT 7
        {  "LCD_P0/ADC0_SE8/TSI0_CH0","PTB0","I2C0_SCL","FTM1_CH0",   "?",    "?",         "FTM1_QD_PHA", "LCD_P0"         }, // PORT B
        {  "LCD_P1/ADC0_SE9/TSI0_CH6","PTB1","I2C0_SDA","FTM1_CH1",   "?",    "?",         "FTM1_QD_PHB", "LCD_P1"         },
        {  "LCD_P2/ADC0_SE12/TSI0_CH7","PTB2","I2C0_SCL","UART0_RTS", "?",    "?",         "FTM0_FLT3",   "LCD_P2"         },
        {  "LCD_P3/ADC0_SE13/TSI0_CH8","PTB3","I2C0_SDA","UART0_CTS", "?",    "?",         "FTM0_FLT0",   "LCD_P3"         },
        {  "LCD_P4/ADC1_SE10", "PTB4","?",       "?",          "?",           "?",         "FTM1_FLT0",   "LCD_P4"         },
        {  "LCD_P5/ADC1_SE11", "PTB5","?",       "?",          "?",           "?",         "FTM2_FLT0",   "LCD_P5"         },
        {  "LCD_P6/ADC1_SE12", "PTB6","?",       "?",          "?",           "FB_AD23",   "?",           "LCD_P6"         },
        {  "LCD_P7/ADC1_SE13", "PTB7","?",       "?",          "?",           "FB_AD22",   "?",           "LCD_P7"         },
        {  "LCD_P8",       "PTB8",  "?",         "UART3_RTS",  "?",           "FB_AD21",   "?",           "LCD_P8"         },
        {  "LCD_P9",       "PTB9",  "SPI1_PCS1", "UART3_CTS",  "?",           "FB_AD20",   "?",           "LCD_P9"         },
        {  "LCD_P10/ADC1_SE14", "PTB10", "SPI1_PCS0", "UART3_RX",   "?",      "FB_AD19",   "FTM0_FLT1",   "LCD_P10"        },
        {  "LCD_P11/ADC1_SE15", "PTB11", "SPI1_SCK",  "UART3_TX",   "?",      "FB_AD18",   "FTM0_FLT2",   "LCD_P11"        },
        {  "?",            "?",     "?",         "?",          "?",           "?",         "?",        "?"                 },
        {  "?",            "?",     "?",         "?",          "?",           "?",         "?",        "?"                 },
        {  "?",            "?",     "?",         "?",          "?",           "?",         "?",        "?"                 },
        {  "?",            "?",     "?",         "?",          "?",           "?",         "?",        "?"                 },
        {  "LCD_P12/TSI0_CH9",  "PTB16", "SPI1_SOUT", "UART0_RX",   "?",      "FB_AD17",   "EWM_IN",      "LCD_P12"        },
        {  "LCD_P13/TSI0_CH10", "PTB17", "SPI1_SIN",  "UART0_TX",   "?",      "FB_AD16",   "EWM_OUT",     "LCD_P13"        },
        {  "LCD_P14/TSI0_CH11", "PTB18", "CAN0_TX",   "FTM2_CH0",   "I2S0_TX_BCLK","FB_AD15",   "FTM2_QD_PHA", "LCD_P14"   },
        {  "LCD_P15/TSI0_CH12", "PTB19", "CAN0_RX",   "FTM2_CH1",   "I2S0_TX_FS",  "FB_OE",     "FTM2_QD_PHB", "LCD_P15"   },
        {  "LCD_P16",      "PTB20", "SPI2_PCS0", "?",          "?",           "FB_AD31",   "CMP0_OUT",    "LCD_P16"        },
        {  "LCD_P17",      "PTB21", "SPI2_SCK",  "?",          "?",           "FB_AD30",   "CMP1_OUT",    "LCD_P17"        },
        {  "LCD_P18",      "PTB22", "SPI2_SOUT", "?",          "?",           "FB_AD29",   "CMP2_OUT",    "LCD_P18"        },
        {  "LCD_P19",      "PTB23", "SPI2_SIN",  "SPI0_PCS5",  "?",           "FB_AD28",   "?",           "LCD_P19"        },
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
        {  "LCD_P20/ADC0_SE14/TSI0_CH13","PTC0","SPI0_PCS4","PDB0_EXTRG","I2S0_TXD",  "?", "?",        "LCD_P20"           }, // PORT C
        {  "LCD_P21/ADC0_SE15/TSI0_CH14","PTC1","SPI0_PCS3","UART1_RTS","FTM0_CH0",   "?",     "?",        "LCD_P21"       },
        {  "LCD_P22/ADC0_SE4/CMP1_IN0TSI0_CH15","PTC2","SPI0_PCS2","UART1_CTS","FTM0_CH1","?", "?",        "LCD_P22"       },
        {  "LCD_P23/CMP1_IN1",     "PTC3",  "SPI0_PCS1", "UART1_RX",   "FTM0_CH2",    "?",     "?",        "LCD_P23"       },
        {  "LCD_P24",            "PTC4",  "SPI0_PCS0", "UART1_TX",   "FTM0_CH3",    "?",       "CMP1_OUT", "LCD_P24"       },
        {  "LCD_P25",            "PTC5",  "SPI0_SCK",  "?",          "LPT0_ALT2",   "?",       "CMP0_OUT", "LCD_P25"       },
        {  "LCD_P26/CMP0_IN0",     "PTC6",  "SPI0_SOUT", "PDB0_EXTRG", "?",           "?",     "?",        "LCD_P26"       },
        {  "LCD_P27/CMP0_IN1",     "PTC7",  "SPI0_SIN",  "?",          "?",           "?",     "?",        "LCD_P27"       },
        {  "LCD_P28/ADC1_SE4/CMP0_IN2","PTC8","?",       "I2S0_MCLK",  "I2S0_CLKIN",  "?",     "?",        "LCD_P28"       },
        {  "LCD_P29/ADC1_SE5/CMP0_IN3","PTC9","?",       "?",          "I2S0_RX_BCLK","?",     "FTM2_FLT0","LCD_P29"       },
        {  "LCD_P30/ADC1_SE6/CMP0_IN4","PTC10","I2C1_SCL","?",         "I2S0_RX_FS",  "?",     "?",        "LCD_P30"       },
        {  "LCD_P31/ADC1_SE7",     "PTC11", "I2C1_SDA",  "?",          "I2S0_RXD",    "?",     "?",        "LCD_P31"       },
        {  "LCD_P32",      "PTC12", "?",         "UART4_RTS",  "?",           "?",         "?",        "LCD_P32"           },
        {  "LCD_P33",      "PTC13", "?",         "UART4_CTS",  "?",           "?",         "?",        "LCD_P33"           },
        {  "LCD_P34",      "PTC14", "?",         "UART4_RX",   "?",           "?",         "?",        "LCD_P34"           },
        {  "LCD_P35",      "PTC15", "?",         "UART4_TX",   "?",           "?",         "?",        "LCD_P35"           },
        {  "LCD_P36",      "PTC16", "CAN1_RX",   "UART3_RX",   "?",           "?",         "?",        "LCD_P36"           },
        {  "LCD_P37",      "PTC17", "CAN1_TX",   "UART3_TX",   "?",           "?",         "?",        "LCD_P37"           },
        {  "LCD_P38",      "PTC18", "?",         "UART3_RTS",  "?",           "?",         "?",        "LCD_P38"           },
        {  "LCD_P39",      "PTC19", "?",         "UART3_CTS",  "?",           "?",         "?",        "LCD_P39"           },
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
        {  "LCD_P40",      "PTD0",  "SPI0_PCS0", "UART2_RTS",  "?",           "?",         "?",        "LCD_P40"           }, // PORT D
        {  "LCD_P41/ADC0_SE5","PTD1", "SPI0_SCK","UART2_CTS",  "?",           "?",         "?",        "LCD_P41"           },
        {  "LCD_P42",      "PTD2",  "SPI0_SOUT", "UART2_RX",   "?",           "?",         "?",        "LCD_P42"           },
        {  "LCD_P43",      "PTD3",  "SPI0_SIN",  "UART2_TX",   "?",           "?",         "?",        "LCD_P43"           },
        {  "LCD_P44",      "PTD4",  "SPI0_PCS1", "UART0_RTS",  "FTM0_CH4",    "?",         "EWM_IN",   "LCD_P44"           },
        {  "LCD_P45/ADC0_SE6", "PTD5",  "SPI0_PCS2", "UART0_CTS",  "FTM0_CH5","?",         "EWM_OUT_b","LCD_P45"           },
        {  "LCD_P46/ADC0_SE7", "PTD6",  "SPI0_PCS3", "UART0_RX",   "FTM0_CH6","?",         "FTM0_FLT0","LCD_P46"           },
        {  "LCD_P47",      "PTD7",  "CMT_IRO",   "UART0_TX",   "FTM0_CH7",    "?",         "FTM0_FLT1","LCD_P47"           },
        {  "?",            "?",     "?",         "?",          "?",           "?",         "?",        "?"                 },
        {  "?",            "?",     "?",         "?",          "?",           "?",         "?",        "?"                 },
        {  "D",            "PTD10", "?",         "UART5_RTS"   "?",           "?",         "?",        "?"                 },
        {  "D",            "PTD11", "SPI2_PCS0", "UART5_CTS"   "SDHC0_CLKIN", "?",         "?",        "?"                 },
        {  "D",            "PTD12", "SPI2_SCK",  "?"           "SDHC0_D4",    "?",         "?",        "?"                 },
        {  "D",            "PTD13", "SPI2_SOUT", "?"           "SDHC0_D5",    "?",         "?",        "?"                 },
        {  "D",            "PTD14", "SPI2_SIN",  "?"           "SDHC0_D6",    "?",         "?",        "?"                 },
        {  "D",            "PTD15", "SPI2_PCS1", "?"           "SDHC0_D7",    "?",         "?",        "?"                 },
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
        {  "ADC1_SE4a",    "PTE0",  "SPI1_PCS1", "UART1_TX",   "SDHC0_D1",    "FB_AD27",   "I2C1_SDA", "?"                 }, // PORT E
        {  "ADC1_SE5a",    "PTE1",  "SPI1_SOUT", "UART1_RX",   "SDHC0_D0",    "FB_AD26",   "I2C1_SCL", "?"                 },
        {  "ADC1_SE6a",    "PTE2",  "SPI1_SCK",  "UART1_CTS",  "SDHC0_DCLK",  "FB_AD25",   "?",        "?"                 },
        {  "ADC1_SEa7",    "PTE3",  "SPI1_SIN",  "UART1_RTS",  "SDHC0_CMD",   "FB_AD24",   "?",        "?"                 },
        {  "D",            "PTE4",  "SPI1_PCS0", "UART3_TX",   "SDHC0_D3",    "FB_BE7_0_BLS31_24b", "FB_TA_b","?"          },
        {  "D",            "PTE5",  "SPI1_PCS2", "UART3_RX",   "SDHC0_D2",    "FB_BE15_8_BLS23_16_b","?","?"               },
        {  "D",            "PTE6",  "SPI1_PCS3", "UART3_CTS",  "I2S0_MCLK",   "FB_CS1_b/FB_TS_b","I2S0_CLKIN","?"          },
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
        {  "ADC0_SE17",    "PTE24", "CAN1_TX",   "UART4_TX",   "?",           "?",         "EWM_OUT",  "?"                 },
        {  "ADC0_SE18",    "PTE25", "CAN1_RX",   "UART4_RX",   "?",           "FB_AD23",   "EWM_IN",   "?"                 },
        {  "D",            "PTE26", "?",         "UART4_CTS",  "?",           "FB_AD22",   "RTC_CLKOUT","USB_CLKIN"        },
        {  "D",            "PTE27", "?",         "UART4_RTS",  "?",           "FB_AD21",   "?",        "?"                 },
        {  "D",            "PTE28", "?",         "?",          "?",           "FB_AD20",   "?",        "?"                 },
        {  "?",            "?",     "?",         "?",          "?",           "?",         "?",        "?"                 },
        {  "?",            "?",     "?",         "?",          "?",           "?",         "?",        "?"                 },
        {  "?",            "?",     "?",         "?",          "?",           "?",         "?",        "?"                 }
    }
};
#endif
