/***********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher
    
    ---------------------------------------------------------------------
    File:      nRF24201.c
    Project:   uTasker project
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2016
    *********************************************************************
    This file contains application operation of nRF24201 (primary or secondary).
    It is intended as a temporary project file to be later incorporated in networking software.

*/


/* =================================================================== */
/*                           include files                             */
/* =================================================================== */

#include "config.h"

#if defined nRF24L01_INTERFACE
/* =================================================================== */
/*                          local definitions                          */
/* =================================================================== */

#define nRF24L01_HANDLE   (NETWORK_HANDLE - 1)


#if defined FRDM_K64F                                                    // SPI1
    #define nRF24L01P_SLAVE()    (_READ_PORT_MASK(B, PORTB_BIT19))       // J1-3 held to ground selects slave mode
    #define nRF24L01P_CS         PORTD_BIT4                              // SPI chip select (active low)
    #define nRF24L01P_TX_ENABLE  PORTC_BIT12                             // chip enable activates rx or tx mode
    #define nRF24L01P_DOUT       PORTD_BIT6                              // SPI data to the nRF24L01+
    #define nRF24L01P_SCLK       PORTD_BIT5                              // SPI clock to the nRF24L01+
    #define nRF24L01P_DIN        PORTD_BIT7                              // SPI data from the nRF24L01+
    #define nRF24L01P_IRQ        PORTC_BIT18                             // maskable interrupt pin from the nRF24L01+, active low
    #define nRF24L01P_IRQ_PORT   PORTC
    #define nRF24L01P_IRQ_PRIORITY PRIORITY_PORT_C_INT

    #define CONFIGURE_INTERFACE_nRF24L01() _CONFIG_PORT_INPUT_FAST_HIGH(B, (PORTB_BIT19), PORT_PS_UP_ENABLE); \
                                 _CONFIG_DRIVE_PORT_OUTPUT_VALUE_FAST_LOW(C, nRF24L01P_TX_ENABLE, 0, (PORT_SRE_SLOW | PORT_DSE_LOW));\
                                 _CONFIG_PERIPHERAL(D, 4, (PD_4_SPI1_PCS0 | PORT_DSE_HIGH | PORT_PS_UP_ENABLE | PORT_SRE_FAST));\
                                 _CONFIG_PERIPHERAL(D, 6, (PD_6_SPI1_SOUT | PORT_DSE_HIGH | PORT_PS_UP_ENABLE | PORT_SRE_FAST));\
                                 _CONFIG_PERIPHERAL(D, 7, (PD_7_SPI1_SIN | PORT_PS_UP_ENABLE));\
                                 _CONFIG_PERIPHERAL(D, 5, (PD_5_SPI1_SCK  | PORT_DSE_HIGH | PORT_PS_UP_ENABLE | PORT_SRE_FAST))

    #define CONFIGURE_nRF24L01_SPI_MODE() POWER_UP(6, SIM_SCGC6_SPI1); \
                                 SPI1_MCR = (SPI_MCR_MSTR | SPI_MCR_DCONF_SPI | SPI_MCR_CLR_RXF | SPI_MCR_CLR_TXF | SPI_MCR_PCSIS_CS0 | SPI_MCR_PCSIS_CS1 | SPI_MCR_PCSIS_CS2 | SPI_MCR_PCSIS_CS3 | SPI_MCR_PCSIS_CS4 | SPI_MCR_PCSIS_CS5);\
                                 SPI1_CTAR0 = (SPI_CTAR_ASC_10 | SPI_CTAR_PBR_3 | SPI_CTAR_DBR | SPI_CTAR_FMSZ_8 | SPI_CTAR_PDT_7 | /*SPI_CTAR_BR_4*/SPI_CTAR_BR_2048 /*| SPI_CTAR_CPHA | SPI_CTAR_CPOL*/); // for 120MHz system, 10MHz speed

    #define FLUSH_nRF24L01_SPI_FIFO_AND_FLAGS()  SPI1_MCR |= SPI_MCR_CLR_RXF; SPI1_SR = (SPI_SR_EOQF | SPI_SR_TFUF | SPI_SR_TFFF | SPI_SR_RFOF | SPI_SR_RFDF)

    #define WRITE_nRF24L01_SPI(byte)            SPI1_PUSHR = (byte | SPI_PUSHR_CONT | SPI_PUSHR_PCS0 | SPI_PUSHR_CTAS_CTAR0) // write a single byte to the output FIFO - assert CS line
    #define WRITE_nRF24L01_SPI_LAST(byte)       SPI1_PUSHR = (byte | SPI_PUSHR_EOQ  | SPI_PUSHR_PCS0 | SPI_PUSHR_CTAS_CTAR0) // write final byte to output FIFO - this will negate the CS line when complete
    #define READ_nRF24L01_SPI_FLASH_DATA()      (unsigned char)SPI1_POPR
    #define WAIT_nRF24L01_SPI_RECEPTION_END()   while (!(SPI1_SR & SPI_SR_RFDF)) {}
    #define CLEAR_nRF24L01_SPI_RECEPTION_FLAG() SPI1_SR |= SPI_SR_RFDF

    #define SPI_FIFO_DEPTH_1                                             // K64 SPI 1 has a FIFO depth of just 1 in SPI1 and SPI 2 !!!!

    #define DISABLE_RX_TX()      _CLEARBITS(C, nRF24L01P_TX_ENABLE); fnRemoteSimulationInterface(REMOTE_RF_INTERFACE, REMOTE_RF_DISABLE_RX_TX, 0, 0, 0)
    #define ENABLE_RX_TX()       _SETBITS(C, nRF24L01P_TX_ENABLE);   fnRemoteSimulationInterface(REMOTE_RF_INTERFACE, REMOTE_RF_ENABLE_RX_TX,  0, 0, 0)
#elif defined FRDM_KL46Z
    #define nRF24L01P_SLAVE()    (_READ_PORT_MASK(B, PORTB_BIT19))       // J1-3 held to ground selects slave mode
    #define nRF24L01P_CS         PORTA_BIT14                             // J2-9
    #define nRF24L01P_TX_ENABLE  PORTA_BIT7                              // J2-5
    #define nRF24L01P_DOUT       PORTA_BIT17                             // J2-15
    #define nRF24L01P_SCLK       PORTA_BIT15                             // J2-11
    #define nRF24L01P_DIN        PORTA_BIT16                             // J2-13
    #define nRF24L01P_IRQ        PORTA_BIT6                              // J2-7
    #define nRF24L01P_IRQ_PORT   PORTA
    #define nRF24L01P_IRQ_PRIORITY PRIORITY_PORT_A_INT

    #define CONFIGURE_INTERFACE_nRF24L01() _CONFIG_PORT_INPUT_FAST_HIGH(B, (PORTB_BIT19), PORT_PS_UP_ENABLE); \
                                 _CONFIG_DRIVE_PORT_OUTPUT_VALUE_FAST_LOW(A, nRF24L01P_TX_ENABLE, 0, (PORT_SRE_SLOW | PORT_DSE_LOW));\
                                 _CONFIG_DRIVE_PORT_OUTPUT_VALUE_FAST_LOW(A, nRF24L01P_CS, nRF24L01P_CS, (PORT_SRE_FAST | PORT_DSE_LOW));\
                                 _CONFIG_PERIPHERAL(A, 15, PA_15_SPI0_SCK); \
                                 _CONFIG_PERIPHERAL(A, 16, (PA_16_SPI0_SOUT | PORT_SRE_FAST | PORT_DSE_HIGH)); \
                                 _CONFIG_PERIPHERAL(A, 17, (PA_17_SPI0_SIN | PORT_PS_UP_ENABLE)); \
                                 _CONFIG_DRIVE_PORT_OUTPUT_VALUE(A, nRF24L01P_CS, nRF24L01P_CS, (PORT_SRE_FAST | PORT_DSE_HIGH)); \

    #define CONFIGURE_nRF24L01_SPI_MODE() POWER_UP(4, SIM_SCGC4_SPI0); \
                                  SPI0_C1 = (/*SPI_C1_CPHA | SPI_C1_CPOL | */SPI_C1_MSTR | SPI_C1_SPE); \
                                  SPI0_BR = (SPI_BR_SPPR_PRE_1 | SPI_BR_SPR_DIV_2); \
                                  (unsigned char)SPI0_S; (unsigned char)SPI0_D

    #define FLUSH_nRF24L01_SPI_FIFO_AND_FLAGS() 

    #define WRITE_nRF24L01_SPI(byte)            SPI0_D = (byte)              // write a single byte
    #define WRITE_nRF24L01_SPI_LAST(byte)       SPI0_D = (byte)              // write a single byte
    #define READ_nRF24L01_SPI_FLASH_DATA()      (unsigned char)SPI0_D
    #if defined _WINDOWS
        #define WAIT_nRF24L01_SPI_RECEPTION_END()    while (!(SPI0_S & (SPI_S_SPRF))) {SPI0_S |= SPI_S_SPRF;}
    #else
        #define WAIT_nRF24L01_SPI_RECEPTION_END()    while (!(SPI0_S & (SPI_S_SPRF))) {}
    #endif
    #define CLEAR_nRF24L01_SPI_RECEPTION_FLAG()
    #define ASSERT_nRF24L01_CS_LINE()   _CLEARBITS(A, nRF24L01P_CS)
    #define NEGATE_nRF24L01_CS_LINE()   _SETBITS(A, nRF24L01P_CS)

    #define DISABLE_RX_TX()      _CLEARBITS(A, nRF24L01P_TX_ENABLE); fnRemoteSimulationInterface(REMOTE_RF_INTERFACE, REMOTE_RF_DISABLE_RX_TX, 0, 0, 0)
    #define ENABLE_RX_TX()       _SETBITS(A, nRF24L01P_TX_ENABLE);   fnRemoteSimulationInterface(REMOTE_RF_INTERFACE, REMOTE_RF_ENABLE_RX_TX,  0, 0, 0)
#elif defined FRDM_KL25Z
    #define nRF24L01P_SLAVE()    (_READ_PORT_MASK(C, PORTC_BIT0))        // J1-3 held to ground selects slave mode
    #define nRF24L01P_CS         PORTE_BIT4                              // J9-13
    #define nRF24L01P_TX_ENABLE  PORTE_BIT0                              // J2-18
    #define nRF24L01P_DOUT       PORTE_BIT3                              // J9-11
    #define nRF24L01P_SCLK       PORTE_BIT2                              // J9-9
    #define nRF24L01P_DIN        PORTE_BIT1                              // J2-20
    #define nRF24L01P_IRQ        PORTA_BIT5                              // J1-12
    #define nRF24L01P_IRQ_PORT   PORTA
    #define nRF24L01P_IRQ_PRIORITY PRIORITY_PORT_A_INT

    #define CONFIGURE_INTERFACE_nRF24L01() _CONFIG_PORT_INPUT_FAST_LOW(C, (PORTC_BIT0), PORT_PS_UP_ENABLE); \
                                 _CONFIG_DRIVE_PORT_OUTPUT_VALUE_FAST_LOW(E, nRF24L01P_TX_ENABLE, 0, (PORT_SRE_SLOW | PORT_DSE_LOW));\
                                 _CONFIG_DRIVE_PORT_OUTPUT_VALUE_FAST_LOW(E, nRF24L01P_CS, nRF24L01P_CS, (PORT_SRE_FAST | PORT_DSE_LOW));\
                                 _CONFIG_PERIPHERAL(E, 2, PE_2_SPI1_SCK); \
                                 _CONFIG_PERIPHERAL(E, 1, (PE_1_SPI1_MOSI | PORT_SRE_FAST | PORT_DSE_HIGH)); \
                                 _CONFIG_PERIPHERAL(E, 3, (PE_3_SPI1_MISO | PORT_PS_UP_ENABLE)); \
                                 _CONFIG_DRIVE_PORT_OUTPUT_VALUE(E, nRF24L01P_CS, nRF24L01P_CS, (PORT_SRE_FAST | PORT_DSE_HIGH)); \

    #define CONFIGURE_nRF24L01_SPI_MODE() POWER_UP(4, SIM_SCGC4_SPI1); \
                                  SPI1_C1 = (/*SPI_C1_CPHA | SPI_C1_CPOL | */SPI_C1_MSTR | SPI_C1_SPE); \
                                  SPI1_BR = (SPI_BR_SPPR_PRE_1 | SPI_BR_SPR_DIV_4); \
                                  (unsigned char)SPI1_S; (unsigned char)SPI1_D

    #define FLUSH_nRF24L01_SPI_FIFO_AND_FLAGS() 

    #define WRITE_nRF24L01_SPI(byte)            SPI1_D = (byte)              // write a single byte
    #define WRITE_nRF24L01_SPI_LAST(byte)       SPI1_D = (byte)              // write a single byte
    #define READ_nRF24L01_SPI_FLASH_DATA()      (unsigned char)SPI1_D
    #if defined _WINDOWS
        #define WAIT_nRF24L01_SPI_RECEPTION_END()    while (!(SPI1_S & (SPI_S_SPRF))) {SPI1_S |= SPI_S_SPRF;}
    #else
        #define WAIT_nRF24L01_SPI_RECEPTION_END()    while (!(SPI1_S & (SPI_S_SPRF))) {}
    #endif
    #define CLEAR_nRF24L01_SPI_RECEPTION_FLAG()
    #define ASSERT_nRF24L01_CS_LINE()   _CLEARBITS(E, nRF24L01P_CS)
    #define NEGATE_nRF24L01_CS_LINE()   _SETBITS(E, nRF24L01P_CS)

    #define DISABLE_RX_TX()      _CLEARBITS(E, nRF24L01P_TX_ENABLE); fnRemoteSimulationInterface(REMOTE_RF_INTERFACE, REMOTE_RF_DISABLE_RX_TX, 0, 0, 0)
    #define ENABLE_RX_TX()       _SETBITS(E, nRF24L01P_TX_ENABLE);   fnRemoteSimulationInterface(REMOTE_RF_INTERFACE, REMOTE_RF_ENABLE_RX_TX,  0, 0, 0)
#elif defined FRDM_KL43Z
    #define nRF24L01P_SLAVE()    (_READ_PORT_MASK(C, PORTC_BIT5))        // J1-15 held to ground selects slave mode
    #define nRF24L01P_CS         PORTB_BIT16                             // J2-19
    #define nRF24L01P_TX_ENABLE  PORTB_BIT17                             // J2-17
    #define nRF24L01P_DOUT       PORTD_BIT7                              // J2-10
    #define nRF24L01P_SCLK       PORTD_BIT5                              // J2-12 (note that the peripheral function in configured with the mode since this port is set as output by demo ports)
    #define nRF24L01P_DIN        PORTD_BIT6                              // J2-8
    #define nRF24L01P_IRQ        PORTA_BIT13                             // J2-2
    #define nRF24L01P_IRQ_PORT   PORTA
    #define nRF24L01P_IRQ_PRIORITY PRIORITY_PORT_A_INT

    #define CONFIGURE_INTERFACE_nRF24L01() _CONFIG_PORT_INPUT_FAST_LOW(C, (PORTC_BIT5), PORT_PS_UP_ENABLE); \
                                 _CONFIG_DRIVE_PORT_OUTPUT_VALUE_FAST_HIGH(B, nRF24L01P_TX_ENABLE, 0, (PORT_SRE_SLOW | PORT_DSE_LOW));\
                                 _CONFIG_DRIVE_PORT_OUTPUT_VALUE_FAST_HIGH(B, nRF24L01P_CS, nRF24L01P_CS, (PORT_SRE_FAST | PORT_DSE_LOW));\
                                 _CONFIG_PERIPHERAL(D, 5,  PD_5_SPI1_SCK); \
                                 _CONFIG_PERIPHERAL(D, 6, (PD_6_SPI1_MOSI | PORT_SRE_FAST | PORT_DSE_HIGH)); \
                                 _CONFIG_PERIPHERAL(D, 7, (PD_7_SPI1_MISO | PORT_PS_UP_ENABLE));

    #define CONFIGURE_nRF24L01_SPI_MODE() POWER_UP(4, SIM_SCGC4_SPI1); \
                                 _CONFIG_DRIVE_PORT_OUTPUT_VALUE_FAST_HIGH(B, nRF24L01P_TX_ENABLE, 0, (PORT_SRE_SLOW | PORT_DSE_LOW));\
                                 _CONFIG_DRIVE_PORT_OUTPUT_VALUE_FAST_HIGH(B, nRF24L01P_CS, nRF24L01P_CS, (PORT_SRE_FAST | PORT_DSE_LOW));\
                                  _CONFIG_PERIPHERAL(D, 5,  PD_5_SPI1_SCK); \
                                  SPI1_C1 = (/*SPI_C1_CPHA | SPI_C1_CPOL | */SPI_C1_MSTR | SPI_C1_SPE); \
                                  SPI1_BR = (SPI_BR_SPPR_PRE_1 | SPI_BR_SPR_DIV_4); \
                                  (unsigned char)SPI1_S; (unsigned char)SPI1_D

    #define FLUSH_nRF24L01_SPI_FIFO_AND_FLAGS() 

    #define WRITE_nRF24L01_SPI(byte)            SPI1_D = (byte)              // write a single byte
    #define WRITE_nRF24L01_SPI_LAST(byte)       SPI1_D = (byte)              // write a single byte
    #define READ_nRF24L01_SPI_FLASH_DATA()      (unsigned char)SPI1_D
    #if defined _WINDOWS
        #define WAIT_nRF24L01_SPI_RECEPTION_END()    while (!(SPI1_S & (SPI_S_SPRF))) {SPI1_S |= SPI_S_SPRF;}
    #else
        #define WAIT_nRF24L01_SPI_RECEPTION_END()    while (!(SPI1_S & (SPI_S_SPRF))) {}
    #endif
    #define CLEAR_nRF24L01_SPI_RECEPTION_FLAG()
    #define ASSERT_nRF24L01_CS_LINE()   _CLEARBITS(B, nRF24L01P_CS)
    #define NEGATE_nRF24L01_CS_LINE()   _SETBITS(B, nRF24L01P_CS)

    #define DISABLE_RX_TX()      _CLEARBITS(B, nRF24L01P_TX_ENABLE); fnRemoteSimulationInterface(REMOTE_RF_INTERFACE, REMOTE_RF_DISABLE_RX_TX, 0, 0, 0)
    #define ENABLE_RX_TX()       _SETBITS(B, nRF24L01P_TX_ENABLE);   fnRemoteSimulationInterface(REMOTE_RF_INTERFACE, REMOTE_RF_ENABLE_RX_TX,  0, 0, 0)
#elif defined FRDM_KL03Z
    #define nRF24L01P_SLAVE()    (_READ_PORT_MASK(B, PORTB_BIT6))        // J1-4 held to ground selects slave mode
    #define nRF24L01P_CS         PORTA_BIT5                              // J2-3
    #define nRF24L01P_TX_ENABLE  PORTB_BIT4                              // J2-9
    #define nRF24L01P_DOUT       PORTA_BIT6                              // J2-5
    #define nRF24L01P_SCLK       PORTB_BIT0                              // J2-6
    #define nRF24L01P_DIN        PORTA_BIT7                              // J2-4
    #define nRF24L01P_IRQ        PORTB_BIT3                              // J2-10
    #define nRF24L01P_IRQ_PORT   PORTB
    #define nRF24L01P_IRQ_PRIORITY PRIORITY_PORT_B_INT

    #define CONFIGURE_INTERFACE_nRF24L01() _CONFIG_PORT_INPUT_FAST_LOW(B, (PORTB_BIT6), PORT_PS_UP_ENABLE); \
                                 _CONFIG_DRIVE_PORT_OUTPUT_VALUE_FAST_LOW(B, nRF24L01P_TX_ENABLE, 0, (PORT_SRE_SLOW | PORT_DSE_LOW));\
                                 _CONFIG_DRIVE_PORT_OUTPUT_VALUE_FAST_LOW(A, nRF24L01P_CS, nRF24L01P_CS, (PORT_SRE_FAST | PORT_DSE_LOW));\
                                 _CONFIG_PERIPHERAL(B, 0,  PB_0_SPI0_SCK); \
                                 _CONFIG_PERIPHERAL(A, 7, (PA_7_SPI0_MOSI | PORT_SRE_FAST | PORT_DSE_HIGH)); \
                                 _CONFIG_PERIPHERAL(A, 6, (PA_6_SPI0_MISO | PORT_PS_UP_ENABLE));

    #define CONFIGURE_nRF24L01_SPI_MODE() POWER_UP(4, SIM_SCGC4_SPI0); \
                                  SPI0_C1 = (/*SPI_C1_CPHA | SPI_C1_CPOL | */SPI_C1_MSTR | SPI_C1_SPE); \
                                  SPI0_BR = (SPI_BR_SPPR_PRE_1 | SPI_BR_SPR_DIV_4); \
                                  (unsigned char)SPI0_S; (unsigned char)SPI0_D

    #define FLUSH_nRF24L01_SPI_FIFO_AND_FLAGS() 

    #define WRITE_nRF24L01_SPI(byte)            SPI0_D = (byte)              // write a single byte
    #define WRITE_nRF24L01_SPI_LAST(byte)       SPI0_D = (byte)              // write a single byte
    #define READ_nRF24L01_SPI_FLASH_DATA()      (unsigned char)SPI0_D
    #if defined _WINDOWS
        #define WAIT_nRF24L01_SPI_RECEPTION_END()    while (!(SPI0_S & (SPI_S_SPRF))) {SPI0_S |= SPI_S_SPRF;}
    #else
        #define WAIT_nRF24L01_SPI_RECEPTION_END()    while (!(SPI0_S & (SPI_S_SPRF))) {}
    #endif
    #define CLEAR_nRF24L01_SPI_RECEPTION_FLAG()
    #define ASSERT_nRF24L01_CS_LINE()   _CLEARBITS(A, nRF24L01P_CS)
    #define NEGATE_nRF24L01_CS_LINE()   _SETBITS(A, nRF24L01P_CS)

    #define DISABLE_RX_TX()      _CLEARBITS(B, nRF24L01P_TX_ENABLE); fnRemoteSimulationInterface(REMOTE_RF_INTERFACE, REMOTE_RF_DISABLE_RX_TX, 0, 0, 0)
    #define ENABLE_RX_TX()       _SETBITS(B, nRF24L01P_TX_ENABLE);   fnRemoteSimulationInterface(REMOTE_RF_INTERFACE, REMOTE_RF_ENABLE_RX_TX,  0, 0, 0)
#else                                                                    // FRDM-K22F
    #define nRF24L01P_SLAVE()    (_READ_PORT_MASK(A, PORTA_BIT13))       // J1-3 held to ground selects slave mode
    #define nRF24L01P_CS         PORTD_BIT4
    #define nRF24L01P_TX_ENABLE  PORTC_BIT11
    #define nRF24L01P_DOUT       PORTD_BIT2                              // SPI0
    #define nRF24L01P_SCLK       PORTD_BIT1
    #define nRF24L01P_DIN        PORTD_BIT3
    #define nRF24L01P_IRQ        PORTD_BIT0
    #define nRF24L01P_IRQ_PORT   PORTD
    #define nRF24L01P_IRQ_PRIORITY PRIORITY_PORT_D_INT

    #define CONFIGURE_INTERFACE_nRF24L01() _CONFIG_PORT_INPUT_FAST_LOW(A, (PORTA_BIT13), PORT_PS_UP_ENABLE); \
                                 _CONFIG_DRIVE_PORT_OUTPUT_VALUE_FAST_LOW(C, nRF24L01P_TX_ENABLE, 0, (PORT_SRE_SLOW | PORT_DSE_LOW));\
                                 _CONFIG_PERIPHERAL(D, 4, (PD_4_SPI0_PCS1 | PORT_DSE_HIGH | PORT_PS_UP_ENABLE | PORT_SRE_FAST));\
                                 _CONFIG_PERIPHERAL(D, 2, (PD_2_SPI0_SOUT | PORT_DSE_HIGH | PORT_PS_UP_ENABLE | PORT_SRE_FAST));\
                                 _CONFIG_PERIPHERAL(D, 3, (PD_3_SPI0_SIN | PORT_PS_UP_ENABLE));\
                                 _CONFIG_PERIPHERAL(D, 1, (PD_1_SPI0_SCK  | PORT_DSE_HIGH | PORT_PS_UP_ENABLE | PORT_SRE_FAST))

    #define CONFIGURE_nRF24L01_SPI_MODE() POWER_UP(6, SIM_SCGC6_SPI0); \
                                 SPI0_MCR = (SPI_MCR_MSTR | SPI_MCR_DCONF_SPI | SPI_MCR_CLR_RXF | SPI_MCR_CLR_TXF | SPI_MCR_PCSIS_CS0 | SPI_MCR_PCSIS_CS1 | SPI_MCR_PCSIS_CS2 | SPI_MCR_PCSIS_CS3 | SPI_MCR_PCSIS_CS4 | SPI_MCR_PCSIS_CS5);\
                                 SPI0_CTAR0 = (SPI_CTAR_ASC_10 | SPI_CTAR_PBR_3 | SPI_CTAR_DBR | SPI_CTAR_FMSZ_8 | SPI_CTAR_PDT_7 | /*SPI_CTAR_BR_4*/SPI_CTAR_BR_2048 /*| SPI_CTAR_CPHA | SPI_CTAR_CPOL*/); // for 120MHz system, 10MHz speed

    #define FLUSH_nRF24L01_SPI_FIFO_AND_FLAGS()  SPI0_MCR |= SPI_MCR_CLR_RXF; SPI0_SR = (SPI_SR_EOQF | SPI_SR_TFUF | SPI_SR_TFFF | SPI_SR_RFOF | SPI_SR_RFDF)

    #define WRITE_nRF24L01_SPI(byte)            SPI0_PUSHR = (byte | SPI_PUSHR_CONT | SPI_PUSHR_PCS1 | SPI_PUSHR_CTAS_CTAR0) // write a single byte to the output FIFO - assert CS line
    #define WRITE_nRF24L01_SPI_LAST(byte)       SPI0_PUSHR = (byte | SPI_PUSHR_EOQ  | SPI_PUSHR_PCS1 | SPI_PUSHR_CTAS_CTAR0) // write final byte to output FIFO - this will negate the CS line when complete
    #define READ_nRF24L01_SPI_FLASH_DATA()      (unsigned char)SPI0_POPR
    #define WAIT_nRF24L01_SPI_RECEPTION_END()   while (!(SPI0_SR & SPI_SR_RFDF)) {}
    #define CLEAR_nRF24L01_SPI_RECEPTION_FLAG() SPI0_SR |= SPI_SR_RFDF

    #define SPI_FIFO_DEPTH_1                                             // to remove once driver corrected for fifo case

    #define DISABLE_RX_TX()      _CLEARBITS(C, nRF24L01P_TX_ENABLE); fnRemoteSimulationInterface(REMOTE_RF_INTERFACE, REMOTE_RF_DISABLE_RX_TX, 0, 0, 0)
    #define ENABLE_RX_TX()       _SETBITS(C, nRF24L01P_TX_ENABLE);   fnRemoteSimulationInterface(REMOTE_RF_INTERFACE, REMOTE_RF_ENABLE_RX_TX,  0, 0, 0)
#endif

/* =================================================================== */
/*                      local structure definitions                    */
/* =================================================================== */

/* =================================================================== */
/*                 local function prototype declarations               */
/* =================================================================== */

static void fnData_nRF24L01(unsigned char *ptrData, unsigned short usDataLength, unsigned char ucTxType);
static unsigned char fnRead_nRF24L01_1(unsigned char ucCommand, unsigned char *ptr_ucData, unsigned char ucLength);
static unsigned char fnCommand_nRF24L01_0(unsigned char ucCommand);
static unsigned char fnWrite_nRF24L01(unsigned char ucCommand, unsigned char *ptr_ucData, unsigned char ucLength);

/* =================================================================== */
/*                             constants                               */
/* =================================================================== */

/* =================================================================== */
/*                     global variable definitions                     */
/* =================================================================== */

/* =================================================================== */
/*                      local variable definitions                     */
/* =================================================================== */

static int iPrimaryTransmitter = 0;





// nRF24L01+ supports 250kbps, 1Mbps and 2Mbps data rates and communicates in the worldwide 2.4GHz ISM band.
// Its modulation is GFSK and it has 126 RF channels that can be chosen from 
// The transmitter power i sprogrammable to be 0dBm, -6dBm, -12dBm or -18dBm whereby the current consumption at 0dBm power is 11.3mA from 3V3.
// When active the receiver consumes 13.5mA at 2Mbps and has a sensitivity of -82dBm at 2Mbps, -85dBm at 1Mbps and -94dBm at 250kbps
// The circuit needs a 16MHz +/- 60ppm crystal and a few passive components only.
// Power down mode consumes 900nA and standby-I mode 26uA.
// Startup time from power down mode is max. 1.5ms and for standby-I mode max. 130us.
// Communication with the chip is via SPI (max. 10Mbps) where the processor is master (MSB first, 
//
// The nRF24L01+ includes enhanced ShockBurst(TM) - a packet based data link layer featuring automatic packet assembly and timing, automatic acknowledgement and retransmission of packets.
// This enables ultra low power and high performance communication in bidirectional and uni-directional systems without added complexity at the processor side.
// - 1 to 32 bytes dynamic payload (rx and tx directions each have 3 fifos)
// - automatic packet handling
// - automatic packet transaction handling with auto-acknowledgement with payload and auto retransmit
// - 6 data pipe MultiCeiver (TM) for 1:6 star networks

#define RF_CHANNEL_NUMBER        15                                      // 0..63

static const unsigned char cAddress[5] = {0x42, 0x76, 0xba, 0x6b, 0x71}; // address for rx/tx pair - endpipe 0
static const unsigned char cBroadcastAddress[5] = {0x43, 0x76, 0xba, 0x6b, 0x72}; // address for rx - endpipe 1
static const unsigned char cMulticastAddress[5] = {0x43, 0x76, 0xba, 0x6b, 0x73}; // address for rx - endpipe 2

#define nRF24L01_POWER_DOWN      0
#define nRF24L01_STANDBY_I       1
#define nRF24L01_STANDBY_II      2
#define nRF24L01_RX_MODE         3
#define nRF24L01_TX_MODE         4

// SPI commands
//
#define R_REGISTER               0x00                                    // read from register
#define R_REGISTER_MASK          0x1f                                    // register value forming part of the read register comamnd
#define W_REGISTER               0x20                                    // write to register
#define W_REGISTER_MASK          0x1f                                    // register value forming part of the read register comamnd
#define R_RX_PAYLOAD             0x61
#define W_TX_PAYLOAD             0xa0
#define CMD_FLUSH_TX             0xe1
#define CMD_FLUSH_RX             0xe2
#define REUSE_TX_PL              0xe3
#define R_RX_PL_WID              0x60                                    // feature must be previously enabled
#define W_ACK_PAYLOAD            0xa8                                    // feature must be previously enabled
#define W_ACK_PAYLOAD_PIPE_MASK  0x07
#define W_ACK_PAYLOAD_NOACK      0xb0                                    // feature must be previously enabled
#define CMD_NOP                  0xff

// nRF24L01+ register
//
#define nRF24L01_CONFIG                      0x00
    #define nRF24L01_CONFIG_MASK_RX_DR          0x40
    #define nRF24L01_CONFIG_MASK_TX_DS          0x20
    #define nRF24L01_CONFIG_MASK_MAX_RT         0x10
    #define nRF24L01_CONFIG_EN_CRC              0x08
    #define nRF24L01_CONFIG_CRC0                0x04
    #define nRF24L01_CONFIG_PRW_UP              0x02
    #define nRF24L01_CONFIG_PRIM_RX             0x01
#define nRF24L01_EN_AA                       0x01
    #define nRF24L01_EN_AA_ENAA_P5              0x20
    #define nRF24L01_EN_AA_ENAA_P4              0x10
    #define nRF24L01_EN_AA_ENAA_P3              0x08
    #define nRF24L01_EN_AA_ENAA_P2              0x04
    #define nRF24L01_EN_AA_ENAA_P1              0x02
    #define nRF24L01_EN_AA_ENAA_P0              0x01
#define nRF24L01_EN_RXADD                    0x02
    #define nRF24L01_EN_RXADD_ERX_P5            0x20
    #define nRF24L01_EN_RXADD_ERX_P4            0x10
    #define nRF24L01_EN_RXADD_ERX_P3            0x08
    #define nRF24L01_EN_RXADD_ERX_P2            0x04
    #define nRF24L01_EN_RXADD_ERX_P1            0x02
    #define nRF24L01_EN_RXADD_ERX_P0            0x01
#define nRF24L01_SETUP_AW                    0x03
    #define nRF24L01_SETUP_AW_3_BYTES           0x01
    #define nRF24L01_SETUP_AW_4_BYTES           0x02
    #define nRF24L01_SETUP_AW_5_BYTES           0x03
#define nRF24L01_SETUP_RETR                  0x04
    #define nRF24L01_SETUP_RETR_ARC_OFF         0x00
    #define nRF24L01_SETUP_RETR_ARC_1           0x01
    #define nRF24L01_SETUP_RETR_ARC_2           0x02
    #define nRF24L01_SETUP_RETR_ARC_3           0x03
    #define nRF24L01_SETUP_RETR_ARC_4           0x04
    #define nRF24L01_SETUP_RETR_ARC_5           0x05
    #define nRF24L01_SETUP_RETR_ARC_6           0x06
    #define nRF24L01_SETUP_RETR_ARC_7           0x07
    #define nRF24L01_SETUP_RETR_ARC_8           0x08
    #define nRF24L01_SETUP_RETR_ARC_9           0x09
    #define nRF24L01_SETUP_RETR_ARC_10          0x0a
    #define nRF24L01_SETUP_RETR_ARC_11          0x0b
    #define nRF24L01_SETUP_RETR_ARC_12          0x0c
    #define nRF24L01_SETUP_RETR_ARC_13          0x0d
    #define nRF24L01_SETUP_RETR_ARC_14          0x0e
    #define nRF24L01_SETUP_RETR_ARC_15          0x0f
    #define nRF24L01_SETUP_RETR_ARD_250         0x00
    #define nRF24L01_SETUP_RETR_ARD_500         0x10
    #define nRF24L01_SETUP_RETR_ARD_75          0x20
    #define nRF24L01_SETUP_RETR_ARD_1000        0x30
    #define nRF24L01_SETUP_RETR_ARD_1250        0x40
    #define nRF24L01_SETUP_RETR_ARD_1500        0x50
    #define nRF24L01_SETUP_RETR_ARD_1750        0x60
    #define nRF24L01_SETUP_RETR_ARD_2000        0x70
    #define nRF24L01_SETUP_RETR_ARD_2250        0x80
    #define nRF24L01_SETUP_RETR_ARD_2500        0x90
    #define nRF24L01_SETUP_RETR_ARD_2750        0xa0
    #define nRF24L01_SETUP_RETR_ARD_3000        0xb0
    #define nRF24L01_SETUP_RETR_ARD_3250        0xc0
    #define nRF24L01_SETUP_RETR_ARD_3500        0xd0
    #define nRF24L01_SETUP_RETR_ARD_3750        0xe0
    #define nRF24L01_SETUP_RETR_ARD_4000        0xf0
#define nRF24L01_RF_CH                       0x05
    #define nRF24L01_RF_CH_MASK                 0x3f
#define nRF24L01_RF_SETUP                    0x06
    #define nRF24L01_RF_SETUP_CONT_WAVE         0x80
    #define nRF24L01_RF_SETUP_RF_DR_1Mbps       0x00
    #define nRF24L01_RF_SETUP_RF_DR_2Mbps       0x08
    #define nRF24L01_RF_SETUP_RF_DR_250kbps     0x20
    #define nRF24L01_RF_SETUP_PLL_LOCK          0x10
    #define nRF24L01_RF_SETUP_RF_PWR_MIN_18     0x00
    #define nRF24L01_RF_SETUP_RF_PWR_MIN_12     0x02
    #define nRF24L01_RF_SETUP_RF_PWR_MIN_6      0x03
    #define nRF24L01_RF_SETUP_RF_PWR_MIN_0      0x06
#define nRF24L01_STATUS                      0x07
    #define nRF24L01_STATUS_RX_DR               0x40
    #define nRF24L01_STATUS_TX_DS               0x20
    #define nRF24L01_STATUS_MAX_RT              0x10
    #define nRF24L01_STATUS_RX_P_NO_MASK        0x0e
    #define nRF24L01_STATUS_TX_FULL             0x01
#define nRF24L01_OBSERVE_TX                  0x08                        // read-only
    #define nRF24L01_OBSERVE_TX_PLOS_CNT_MASK   0xf0
    #define nRF24L01_OBSERVE_TX_ARC_CNT_MASK    0x0f
#define nRF24L01_RPD                         0x09                        // read-only
    #define nRF24L01_RPD_RPD                    0x01
    #define nRF24L01_OBSERVE_TX_ARC_CNT_MASK    0x0f
#define nRF24L01_RX_ADDR_P0                  0x0a
#define nRF24L01_RX_ADDR_P1                  0x0b
#define nRF24L01_RX_ADDR_P2                  0x0c
#define nRF24L01_RX_ADDR_P3                  0x0d
#define nRF24L01_RX_ADDR_P4                  0x0e
#define nRF24L01_RX_ADDR_P5                  0x0f
#define nRF24L01_TX_ADDR                     0x10
#define nRF24L01_RX_PW_P0                    0x11
#define nRF24L01_RX_PW_P1                    0x12
#define nRF24L01_RX_PW_P2                    0x13
#define nRF24L01_RX_PW_P3                    0x14
#define nRF24L01_RX_PW_P4                    0x15
#define nRF24L01_RX_PW_P5                    0x16
#define nRF24L01_FIFO_STATUS                 0x17                        // read-only
    #define nRF24L01_FIFO_STATUS_TX_REUSE       0x40
    #define nRF24L01_FIFO_STATUS_TX_FULL        0x20
    #define nRF24L01_FIFO_STATUS_TX_EMPTY       0x10
    #define nRF24L01_FIFO_STATUS_RX_FULL        0x02
    #define nRF24L01_FIFO_STATUS_RX_EMPTY       0x01
#define nRF24L01_DYNPD                       0x1c
    #define nRF24L01_DYNPD_DPL_P5               0x20
    #define nRF24L01_DYNPD_DPL_P4               0x10
    #define nRF24L01_DYNPD_DPL_P3               0x08
    #define nRF24L01_DYNPD_DPL_P2               0x04
    #define nRF24L01_DYNPD_DPL_P1               0x02
    #define nRF24L01_DYNPD_DPL_P0               0x01
#define nRF24L01_FEATURE                     0x1d
    #define nRF24L01_FEATURE_EN_DPL             0x04
    #define nRF24L01_FEATURE_EN_ACK_PAY         0x02
    #define nRF24L01_FEATURE_EN_DYN_ACK         0x01

#if !defined _WINDOWS || defined REMOTE_SIMULATION_INTERFACE
    #define fnRemoteSimulationInterface(a, b, c, d, e)
#endif


// Interrupt from the nRF24L01+ (reception is available in the rx fifo, transmission was acked or failed)
//
static void fn_nRF24L01_interrupt(void)
{
    #if defined REMOTE_SIMULATION_INTERFACE
    RemoteSimUDP_Frame.ucUDP_Message[0] = REMOTE_RF_INTERFACE;
    RemoteSimUDP_Frame.ucUDP_Message[1] = REMOTE_RF_IRQ_EVENT;           // take a snap-shot of the variable registers in device
    RemoteSimUDP_Frame.ucUDP_Message[2] = fnRead_nRF24L01_1((R_REGISTER | nRF24L01_OBSERVE_TX), &RemoteSimUDP_Frame.ucUDP_Message[3], 1);
    fnRead_nRF24L01_1((R_REGISTER | nRF24L01_RPD), &RemoteSimUDP_Frame.ucUDP_Message[4], 1);
    fnRead_nRF24L01_1((R_REGISTER | nRF24L01_RX_PW_P0), &RemoteSimUDP_Frame.ucUDP_Message[5], 1);
    fnRead_nRF24L01_1((R_REGISTER | nRF24L01_RX_PW_P1), &RemoteSimUDP_Frame.ucUDP_Message[6], 1);
    fnRead_nRF24L01_1((R_REGISTER | nRF24L01_RX_PW_P2), &RemoteSimUDP_Frame.ucUDP_Message[7], 1);
    fnRead_nRF24L01_1((R_REGISTER | nRF24L01_RX_PW_P3), &RemoteSimUDP_Frame.ucUDP_Message[8], 1);
    fnRead_nRF24L01_1((R_REGISTER | nRF24L01_RX_PW_P4), &RemoteSimUDP_Frame.ucUDP_Message[9], 1);
    fnRead_nRF24L01_1((R_REGISTER | nRF24L01_RX_PW_P5), &RemoteSimUDP_Frame.ucUDP_Message[10], 1);
    fnRead_nRF24L01_1((R_REGISTER | nRF24L01_FIFO_STATUS), &RemoteSimUDP_Frame.ucUDP_Message[11], 1);
    fnRead_nRF24L01_1(R_RX_PL_WID, &RemoteSimUDP_Frame.ucUDP_Message[12], 1); // read the payload length (top of fifo)
    fnRead_nRF24L01_1(R_RX_PAYLOAD, &RemoteSimUDP_Frame.ucUDP_Message[13], RemoteSimUDP_Frame.ucUDP_Message[12]); // read the payload (top of fifo)
    fnSendUDP(RemoteSimUDPSocket, ucRemoteSimIP_address, REMOTE_SIM_UDP_PORT, (unsigned char *)&RemoteSimUDP_Frame.tUDP_Header, (unsigned short)(RemoteSimUDP_Frame.ucUDP_Message[12] + 13), 0); // send to the simulator
    #else
    fnInterruptMessage(TASK_APPLICATION, E_nRF24L01_EVENT);              // schedule handling of event by the application task
    #endif
}

extern void fnTest_nRF24L01_Write(int iPingPong)
{
    static const unsigned char ucTestData[] = {1,2,3,4,5,6,7,8,9,10};
    switch (iPingPong) {
    case 0:
        fnWrite_nRF24L01((nRF24L01_TX_ADDR | W_REGISTER), (unsigned char *)cAddress, sizeof(cAddress)); // set the address for transmission
        fnData_nRF24L01((unsigned char *)ucTestData, sizeof(ucTestData), W_TX_PAYLOAD); // send test transmission
        break;
    case 1:
        fnWrite_nRF24L01((nRF24L01_TX_ADDR | W_REGISTER), (unsigned char *)cBroadcastAddress, sizeof(cBroadcastAddress)); // set the address for transmission
        fnData_nRF24L01((unsigned char *)ucTestData, sizeof(ucTestData), W_ACK_PAYLOAD_NOACK); // send test transmission
        break;
    case 2:
        fnWrite_nRF24L01((nRF24L01_TX_ADDR | W_REGISTER), (unsigned char *)cMulticastAddress, sizeof(cMulticastAddress)); // set the address for transmission
        fnData_nRF24L01((unsigned char *)ucTestData, sizeof(ucTestData), W_ACK_PAYLOAD_NOACK); // send test transmission
        break;
    }
}

// Write a single byte command to the nRF24L01 register and return the status register value
//
static unsigned char fnCommand_nRF24L01_0(unsigned char ucCommand)
{
    unsigned char ucStatus;
#if defined KINETIS_KL
    ASSERT_nRF24L01_CS_LINE();                                           // assert CS
    WRITE_nRF24L01_SPI(ucCommand);                                       // send single data byte
    WAIT_nRF24L01_SPI_RECEPTION_END();                                   // wait until the command has been sent and the received byte is available
    ucStatus = READ_nRF24L01_SPI_FLASH_DATA();                           // read the status value
    CLEAR_nRF24L01_SPI_RECEPTION_FLAG();                                 // clear the receive flag
    NEGATE_nRF24L01_CS_LINE();                                           // negate CS
#else
    FLUSH_nRF24L01_SPI_FIFO_AND_FLAGS();                                 // ensure that the SPI FIFOs are empty and the status flags are reset before starting
    WRITE_nRF24L01_SPI_LAST(ucCommand);                                  // send single data byte
    WAIT_nRF24L01_SPI_RECEPTION_END();                                   // wait until the command has been sent and the received byte is available
    ucStatus = READ_nRF24L01_SPI_FLASH_DATA();                           // read the status value
    CLEAR_nRF24L01_SPI_RECEPTION_FLAG();                                 // clear the receive flag
#endif
#if defined _WINDOWS && !defined REMOTE_SIMULATION_INTERFACE
    ucStatus = (unsigned char)fnRemoteSimulationInterface(REMOTE_RF_INTERFACE, REMOTE_RF_W_COMMAND, &ucCommand, 1, 1);
#endif
    return ucStatus;                                                     // return the status read as response to single byte
}

// Write a single byte to an nRF24L01 register and return the status register value
//
static unsigned char fnCommand_nRF24L01_1(unsigned char ucRegister, unsigned char ucData)
{
    unsigned char ucStatus;
#if defined KINETIS_KL
    ASSERT_nRF24L01_CS_LINE();                                           // assert CS
#else
    FLUSH_nRF24L01_SPI_FIFO_AND_FLAGS();                                 // ensure that the SPI FIFOs are empty and the status flags are reset before starting
#endif
    WRITE_nRF24L01_SPI(ucRegister);                                      // send first byte (write command and register reference)
#if defined _WINDOWS && !defined REMOTE_SIMULATION_INTERFACE
    ucStatus = (unsigned char)fnRemoteSimulationInterface(REMOTE_RF_INTERFACE, REMOTE_RF_W_REGISTER, &ucRegister, 1, 1);
#endif
#if defined KINETIS_KL || defined SPI_FIFO_DEPTH_1
    WAIT_nRF24L01_SPI_RECEPTION_END();                                   // wait until the command has been sent and the received byte is available
    ucStatus = READ_nRF24L01_SPI_FLASH_DATA();                           // read the status value
    CLEAR_nRF24L01_SPI_RECEPTION_FLAG();                                 // clear the receive flag
#endif
    WRITE_nRF24L01_SPI_LAST(ucData);                                     // send single data byte
    fnRemoteSimulationInterface(REMOTE_RF_INTERFACE, REMOTE_RF_W_REGISTER_VALUE, &ucData, 1, 1);
#if defined KINETIS_KL
    WAIT_nRF24L01_SPI_RECEPTION_END();                                   // wait until the data has been sent and the received byte is available
    (void)READ_nRF24L01_SPI_FLASH_DATA();                                // clear the receiver
    CLEAR_nRF24L01_SPI_RECEPTION_FLAG();                                 // clear the receive flag
    NEGATE_nRF24L01_CS_LINE();                                           // negate CS
#elif defined SPI_FIFO_DEPTH_1
    WAIT_nRF24L01_SPI_RECEPTION_END();                                   // wait until at least one byte is in the receive FIFO
    (void)READ_nRF24L01_SPI_FLASH_DATA();                                // read the status value
    CLEAR_nRF24L01_SPI_RECEPTION_FLAG();                                 // clear the receive flag
#else
    WAIT_nRF24L01_SPI_RECEPTION_END();                                   // wait until at least one byte is in the receive FIFO
    ucStatus = READ_nRF24L01_SPI_FLASH_DATA();                           // read the status value
    CLEAR_nRF24L01_SPI_RECEPTION_FLAG();                                 // clear the receive flag
#endif
    return ucStatus;                                                     // return the status read as response to first byte
}

// Write multiple byte to the nRF24L01 and return the status register value (do not call with zero data length - use fnCommand_nRF24L01_0() instead)
//
static unsigned char fnWrite_nRF24L01(unsigned char ucCommand, unsigned char *ptr_ucData, unsigned char ucLength)
{
    unsigned char ucStatus;
#if defined KINETIS_KL
    ASSERT_nRF24L01_CS_LINE();                                           // assert CS
#else
    #if !defined SPI_FIFO_DEPTH_1
    int iRxBytes = 0;                                                    // used to monitor fifo depth
    #endif
    FLUSH_nRF24L01_SPI_FIFO_AND_FLAGS();                                 // ensure that the SPI FIFOs are empty and the status flags are reset before starting
#endif
#if defined _WINDOWS
    if (ucLength == 0) {
        _EXCEPTION("Use fnCommand_nRF24L01_0() to send zero data length!!");
    }
#endif
    WRITE_nRF24L01_SPI(ucCommand);                                       // send first byte (write command)
#if defined _WINDOWS && !defined REMOTE_SIMULATION_INTERFACE
    ucStatus = (unsigned char)fnRemoteSimulationInterface(REMOTE_RF_INTERFACE, REMOTE_RF_W_REGISTER, &ucCommand, 1, 1);
    fnRemoteSimulationInterface(REMOTE_RF_INTERFACE, REMOTE_RF_W_REGISTER_VALUE, ptr_ucData, ucLength, (W_TX_PAYLOAD != ucCommand));
#endif
    WAIT_nRF24L01_SPI_RECEPTION_END();                                   // wait until the command has been sent and the received byte is available
    ucStatus = READ_nRF24L01_SPI_FLASH_DATA();                           // read the status value
    CLEAR_nRF24L01_SPI_RECEPTION_FLAG();                                 // clear the receive flag
    while (ucLength-- != 0) {                                            // for each byte of data
#if defined KINETIS_KL
        WRITE_nRF24L01_SPI(*ptr_ucData++);                               // send next data byte
        WAIT_nRF24L01_SPI_RECEPTION_END();                               // wait until the data has been sent and the received byte is available
        (void)READ_nRF24L01_SPI_FLASH_DATA();                            // clear the receiver
        CLEAR_nRF24L01_SPI_RECEPTION_FLAG();                             // clear the receive flag
#elif defined SPI_FIFO_DEPTH_1
        if (ucLength == 0) {
            WRITE_nRF24L01_SPI_LAST(*ptr_ucData);                        // send final data byte
        }
        else {
            WRITE_nRF24L01_SPI(*ptr_ucData++);                           // send next data byte
        }
        WAIT_nRF24L01_SPI_RECEPTION_END();                               // wait until the data has been sent and the received byte is available
        (void)READ_nRF24L01_SPI_FLASH_DATA();                            // clear the receiver
        CLEAR_nRF24L01_SPI_RECEPTION_FLAG();                             // clear the receive flag
#else
        if (ucLength == 0) {
            WRITE_nRF24L01_SPI_LAST(*ptr_ucData);                        // send final data byte
        }
        else {
            WRITE_nRF24L01_SPI(*ptr_ucData);                             // send next data byte
        }
        ptr_ucData++;
        if (iRxBytes >= 3) {                                             // fifo depth of 4 reached
            WAIT_nRF24L01_SPI_RECEPTION_END();                           // wait until the data has been sent and the received byte is available
            (void)READ_nRF24L01_SPI_FLASH_DATA();                        // clear the receiver
            CLEAR_nRF24L01_SPI_RECEPTION_FLAG();                         // clear the receive flag
        }
        else {
            iRxBytes++;
        }
#endif
    }
#if defined KINETIS_KL
    NEGATE_nRF24L01_CS_LINE();                                           // negate CS
#elif !defined SPI_FIFO_DEPTH_1
    while (iRxBytes-- != 0) {                                            // clear rx FIFO
        WAIT_nRF24L01_SPI_RECEPTION_END();                               // wait until the data has been sent and the received byte is available
        (void)READ_nRF24L01_SPI_FLASH_DATA();                            // clear the receiver
        CLEAR_nRF24L01_SPI_RECEPTION_FLAG();                             // clear the receive flag
    }
#endif
    return ucStatus;                                                     // return the status read as response to first byte
}


static unsigned char fnRead_nRF24L01_1(unsigned char ucCommand, unsigned char *ptr_ucData, unsigned char ucLength)
{
    unsigned char ucStatus;
#if defined KINETIS_KL
    ASSERT_nRF24L01_CS_LINE();                                           // assert CS
#else
    FLUSH_nRF24L01_SPI_FIFO_AND_FLAGS();                                 // ensure that the SPI FIFOs are empty and the status flags are reset before starting
#endif
    WRITE_nRF24L01_SPI(ucCommand);                                       // send first byte (write command)
    WAIT_nRF24L01_SPI_RECEPTION_END();                                   // wait until the command has been sent and the received byte is available
    ucStatus = READ_nRF24L01_SPI_FLASH_DATA();                           // read the status value
    CLEAR_nRF24L01_SPI_RECEPTION_FLAG();                                 // clear the receive flag
#if defined _WINDOWS && !defined REMOTE_SIMULATION_INTERFACE
    ucStatus = (unsigned char)fnRemoteSimulationInterface(REMOTE_RF_INTERFACE, REMOTE_RF_W_REGISTER, &ucCommand, 1, 1);
#endif
    while (ucLength-- != 0) {                                            // for each byte of data
#if defined KINETIS_KL
        WRITE_nRF24L01_SPI(0xff);                                        // send dummy byte
        WAIT_nRF24L01_SPI_RECEPTION_END();                               // wait until the data has been sent and the received byte is available
        *ptr_ucData++ = READ_nRF24L01_SPI_FLASH_DATA();                  // read the data byte
        CLEAR_nRF24L01_SPI_RECEPTION_FLAG();                             // clear the receive flag
#else
        if (ucLength == 0) {
            WRITE_nRF24L01_SPI_LAST(0xff);                               // send final dummy byte
        }
        else {
            WRITE_nRF24L01_SPI(0xff);                                    // send next dummy byte
        }
        WAIT_nRF24L01_SPI_RECEPTION_END();                               // wait until the data has been sent and the received byte is available
        *ptr_ucData = READ_nRF24L01_SPI_FLASH_DATA();                  // read the data byte
        CLEAR_nRF24L01_SPI_RECEPTION_FLAG();                             // clear the receive flag
    #if defined _WINDOWS && !defined REMOTE_SIMULATION_INTERFACE
        *ptr_ucData = (unsigned char )fnRemoteSimulationInterface(REMOTE_RF_INTERFACE, REMOTE_RF_W_REGISTER_VALUE, ptr_ucData, 1, 0);
    #endif
        ptr_ucData++;
#endif
    }
#if defined KINETIS_KL
    NEGATE_nRF24L01_CS_LINE();                                           // negate CS
#endif
    return ucStatus;                                                     // return the status read as response to first byte
}

extern void fnHandle_nRF24L01_event(void)
{
    unsigned char ucStatus = fnCommand_nRF24L01_0(CMD_NOP);              // read the cause of the interrupt
    if (ucStatus & nRF24L01_STATUS_TX_DS) {                              // data was acked
        fnDebugMsg("DATA ACKED!!\r\n");
    }
    if (ucStatus & nRF24L01_STATUS_MAX_RT) {                             // maximum repetitions made by transmitter
        fnCommand_nRF24L01_0(CMD_FLUSH_TX);                              // remove the message
        fnDebugMsg("No ACK ;-(\r\n");
    }
    if (ucStatus & nRF24L01_STATUS_RX_DR) {                              // data reception
        int iPipe = ((ucStatus & nRF24L01_STATUS_RX_P_NO_MASK) >> 1);
        unsigned char ucData[32];
        unsigned char ucLength;
        fnDebugMsg("Rx data available - pipe ");
        fnDebugDec(iPipe, 0); 
        fnDebugMsg(" : ");
        fnRead_nRF24L01_1(R_RX_PL_WID, &ucLength, 1);                    // read the payload length (top of fifo)
        fnDebugDec(ucLength, WITH_CR_LF);
        if (ucLength > 32) {
            fnCommand_nRF24L01_0(CMD_FLUSH_RX);                          // flush invalid length
        }
        else {
            int i = 0;
            fnRead_nRF24L01_1(R_RX_PAYLOAD, ucData, ucLength);           // read the payload (top of fifo)
            while (i < ucLength) {
                fnDebugHex(ucData[i++], (sizeof(ucData[0]) | WITH_LEADIN | WITH_SPACE));
            }
            fnDebugMsg("\r\n");
            if (iPrimaryTransmitter == 0) {
                fnWrite_nRF24L01(W_ACK_PAYLOAD, ucData, (unsigned char)(ucLength/2)); // echo data back on next data reception
                ucData[0]++;                                            // modify for test
                fnWrite_nRF24L01(W_ACK_PAYLOAD, ucData, ucLength);      // echo modified data back (test 2 in FIFO)
            }
        }
    }
    fnCommand_nRF24L01_1((W_REGISTER | nRF24L01_STATUS), (nRF24L01_STATUS_RX_DR | nRF24L01_STATUS_TX_DS | nRF24L01_STATUS_MAX_RT)); // reset interrupt cause (clearing the interrupt at the nRF24L01+)
}


// Data can be queued in up to 3 tx fifo buffers (32 byte each)
//
static void fnData_nRF24L01(unsigned char *ptrData, unsigned short usDataLength, unsigned char ucTxType)
{
#if 0
    int i;
    unsigned char ucData = 0;
    fnDebugMsg("Dump nRF24L01+\r\n");
    for (i = 0; i < 10; i++) {
        fnRead_nRF24L01_1((unsigned char)(R_REGISTER | i), &ucData, 1);
        fnDebugHex(ucData, (WITH_LEADIN | WITH_CR_LF | sizeof(ucData)));
    }
    fnRead_nRF24L01_1((R_REGISTER | 0x17), &ucData, 1);
    fnDebugHex(ucData, (WITH_LEADIN | WITH_CR_LF | sizeof(ucData)));
    fnRead_nRF24L01_1((R_REGISTER | 0x1c), &ucData, 1);
    fnDebugHex(ucData, (WITH_LEADIN | WITH_CR_LF | sizeof(ucData)));
    fnRead_nRF24L01_1((R_REGISTER | 0x1d), &ucData, 1);
    fnDebugHex(ucData, (WITH_LEADIN | WITH_CR_LF | sizeof(ucData)));
#else
    if (usDataLength > 32) {                                             // maximum packet payload
        usDataLength = 32;
    }
    fnWrite_nRF24L01(W_TX_PAYLOAD, ptrData, (unsigned char)usDataLength);
#endif
}

static void fnSetMode_nRF24L01(int iMode)
{
    switch (iMode) {
    case nRF24L01_POWER_DOWN:
        fnCommand_nRF24L01_1((W_REGISTER | nRF24L01_CONFIG), 0);         // remove the PWR_UP bit
        DISABLE_RX_TX();                                                 // set the enable line to '0'
        return;
    case nRF24L01_STANDBY_I:
        DISABLE_RX_TX();                                                 // set the enable line to '0'
        fnDebugHex(fnCommand_nRF24L01_1((W_REGISTER | nRF24L01_CONFIG), (nRF24L01_CONFIG_PRW_UP | nRF24L01_CONFIG_EN_CRC)), (1 | WITH_LEADIN | WITH_CR_LF));
        return;
    case nRF24L01_STANDBY_II:
        break;
    case nRF24L01_RX_MODE:
        fnCommand_nRF24L01_1((W_REGISTER | nRF24L01_CONFIG), (nRF24L01_CONFIG_PRW_UP | nRF24L01_CONFIG_PRIM_RX | nRF24L01_CONFIG_EN_CRC));
        break;
    case nRF24L01_TX_MODE:
        fnCommand_nRF24L01_1((W_REGISTER | nRF24L01_CONFIG), (nRF24L01_CONFIG_PRW_UP | nRF24L01_CONFIG_EN_CRC));
        break;
    }
    ENABLE_RX_TX();                                                      // set the enable line to '1' (we use the Enhanced ShockBurst (TM) feature so this can be held high, otherwise it should never be kept high for more than 4ms when controlling the transmitter)
}

#if !defined REMOTE_SIMULATION_INTERFACE
static void fnConfigure_nRF24L01(void)
{
    // Set the air data rate (250kbps, 1Mbps or 2Mbps)
    // Set the operating channel (2.4GHz + RF_CH[MHz])
    // Set the transmitter PA level (0dBm, -6dBm, -12dBm or -18dBm)
    // Configure Enhanced ShockBurst address, dynamic payload option, whether we are PTX (primary transmitter) or PRX (primary receiver),
    // the number of CRC bytes used (1 or 2), auto-acknowledge feature
    //
    fnCommand_nRF24L01_1((W_REGISTER | nRF24L01_RF_SETUP), (nRF24L01_RF_SETUP_RF_DR_1Mbps | nRF24L01_RF_SETUP_RF_PWR_MIN_0)); // set speed and power
    fnCommand_nRF24L01_1((W_REGISTER | nRF24L01_RF_CH), (RF_CHANNEL_NUMBER));             // set channel to be used
    fnWrite_nRF24L01((nRF24L01_RX_ADDR_P0 | W_REGISTER), (unsigned char *)cAddress, sizeof(cAddress)); // set the address for pipe 0 reception
    fnWrite_nRF24L01((nRF24L01_RX_ADDR_P1 | W_REGISTER), (unsigned char *)cBroadcastAddress, sizeof(cBroadcastAddress)); // set the address for pipe 1 broadcast reception
    fnWrite_nRF24L01((nRF24L01_RX_ADDR_P2 | W_REGISTER), (unsigned char *)cMulticastAddress, sizeof(cMulticastAddress)); // set the address for pipe 2 broadcast reception
    fnWrite_nRF24L01((nRF24L01_TX_ADDR | W_REGISTER), (unsigned char *)cAddress, sizeof(cAddress)); // set the address for transmission
    fnCommand_nRF24L01_1((W_REGISTER | nRF24L01_EN_RXADD), (nRF24L01_EN_RXADD_ERX_P0 | nRF24L01_EN_RXADD_ERX_P1 | nRF24L01_EN_RXADD_ERX_P2)); // enable endpipes 0 and 1 for reception
    fnCommand_nRF24L01_1((W_REGISTER | nRF24L01_DYNPD), (nRF24L01_DYNPD_DPL_P0 | nRF24L01_DYNPD_DPL_P1 | nRF24L01_DYNPD_DPL_P2)); // enable dynamic payload length on endpipes 0 and 1
    fnCommand_nRF24L01_1((W_REGISTER | nRF24L01_FEATURE), (nRF24L01_FEATURE_EN_DPL | nRF24L01_FEATURE_EN_ACK_PAY | nRF24L01_FEATURE_EN_DYN_ACK)); // enable dynamic payload, payload with ack and the use of the W_TX_PAYLOAD_NOACK command
}
#endif


extern void fnInit_nRF24L01(void)
{
    INTERRUPT_SETUP interrupt_setup;                                     // interrupt configuration parameters
    interrupt_setup.int_type       = PORT_INTERRUPT;                     // identifier to configure port interrupt
    interrupt_setup.int_handler    = fn_nRF24L01_interrupt;              // handling function
    interrupt_setup.int_priority   = nRF24L01P_IRQ_PRIORITY;             // interrupt priority level
    interrupt_setup.int_port       = nRF24L01P_IRQ_PORT;                 // the port that the interrupt input is on
    interrupt_setup.int_port_bits  = nRF24L01P_IRQ;                      // the IRQ input connected
    interrupt_setup.int_port_sense = (IRQ_FALLING_EDGE | PULLUP_ON);     // interrupt is to be falling edge sensitive
    // Initially, after applying power, the device is in a reset state which prevails for 100ms. The initialisation should therefore take place after 100ms to ensure that it is ready
    //
    CONFIGURE_nRF24L01_SPI_MODE();                                       // prepare the SPI for use

    fnSetMode_nRF24L01(nRF24L01_STANDBY_I);                              // power down mode is always left passing through a standby mode - 5ms delay is required for the standby mode to be entered
    fnDelayLoop(4000);                                                   // allow 4ms for the standby-I mode to be obtained
    if (nRF24L01P_SLAVE() != 0) {                                        // check whether the board is a slave (not primary transmitter)
        iPrimaryTransmitter = 1;
    }
    #if defined REMOTE_SIMULATION_INTERFACE                              // when acting as remote simulation extension no further initialsiation is performed since it is up to the simulator to do this remotely
    if (RemoteSimUDPSocket < 0) {
        RemoteSimUDPSocket = fnGetUDP_socket(TOS_MINIMISE_DELAY, fnRemoteSimUDP_Listner, (UDP_OPT_SEND_CS | UDP_OPT_CHECK_CS));
        fnBindSocket(RemoteSimUDPSocket, REMOTE_SIM_UDP_PORT);           // bind socket and listen
    }
    #else
    fnDebugMsg("nRF24L01P+ ");
    fnConfigure_nRF24L01();                                              // perform the default configuration (this must be performed in a standby or power down mode)
    fnCommand_nRF24L01_0(CMD_FLUSH_RX);                                  // ensure that rx fifo is empty
    fnCommand_nRF24L01_0(CMD_FLUSH_TX);                                  // ensure the tx fifo is empty
    fnCommand_nRF24L01_1((W_REGISTER | nRF24L01_STATUS), (nRF24L01_STATUS_RX_DR | nRF24L01_STATUS_TX_DS | nRF24L01_STATUS_MAX_RT)); // ensure no pending interrupts
    if (iPrimaryTransmitter != 0) {                                      // if we are PTX
        fnDebugMsg("master\r\n");
        fnCommand_nRF24L01_1((W_REGISTER | nRF24L01_SETUP_RETR), (nRF24L01_SETUP_RETR_ARC_8 | nRF24L01_SETUP_RETR_ARD_500)); // enable 8 repetitions with 500us timeout (suitable for 1Mbps operation with maximum payload)
        fnSetMode_nRF24L01(nRF24L01_TX_MODE);                            // set to transmission mode
        uTaskerMonoTimer(TASK_APPLICATION, (DELAY_LIMIT)(5 * SEC), E_nRF24L01_PERIOD); // start polling operation
    }
    else {
        fnDebugMsg("slave\r\n");
        fnSetMode_nRF24L01(nRF24L01_RX_MODE);                            // set to reception mode
    }
        #if IP_INTERFACE_COUNT > 1
    fnEnterInterfaceHandle(RF_IP_INTERFACE, nRF24L01_HANDLE, (INTERFACE_NO_RX_CS_OFFLOADING | INTERFACE_NO_TX_CS_OFFLOADING | INTERFACE_NO_TX_PAYLOAD_CS_OFFLOADING));
        #endif
    #endif
    fnConfigureInterrupt((void *)&interrupt_setup);                      // configure interrupt
}

// Configure the hardware lines immediately after reset so that the device has stabilised by the time the initialisation is called
//
extern void fnPrepare_nRF24L01_signals(void)
{
    CONFIGURE_INTERFACE_nRF24L01();                                      // configure hardware lines to default states
}


// This is the application handle handler which is called when user handles [entered by fnEnterInterfaceHandle()] are matched on writes
//
extern QUEUE_TRANSFER fnUserWrite(QUEUE_HANDLE driver_id, unsigned char *output_buffer, QUEUE_TRANSFER nr_of_bytes)
{
    #if defined ENC424J600_INTERFACE
    static unsigned short usTransmitBufferContent = 0;
    #endif
    switch (driver_id) {
    #if defined ENC424J600_INTERFACE_
    case ENC424J600_HANDLE:                                              // Ethernet frame content to be sent to the ENC424J600 interface
        if (output_buffer == 0) {                                        // message is now completely in the output buffer so we can release it
            enc424j600WriteReg(ENC424J600_ETXLEN, usTransmitBufferContent);
            enc424j600MACFlush();
            usTransmitBufferContent = 0;
        }
        else {
            if (usTransmitBufferContent == 0) {
                enc424j600WriteReg(ENC424J600_EGPWRPT, ENC424J600_TRANSMIT_BUFFER_START); // start at the beginning of the transmit buffer space
            }
            enc424j600WriteN(ENC424J600_WGPDATA, output_buffer, nr_of_bytes); // copy the frame to the output buffer
            usTransmitBufferContent += nr_of_bytes;
            return nr_of_bytes;
        }
        break;
    #endif
    #if defined nRF24L01_INTERFACE
    case nRF24L01_HANDLE:                                                // Ethernet frame content to be sent to the nRF24L01+ interface
        break;
    #endif
    }
    return 0;                                                            // return the number of byte transmitted
}
#endif

