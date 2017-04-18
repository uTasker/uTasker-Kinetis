/***********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher

    ---------------------------------------------------------------------
    File:      kinetis_errata.h
    Project:   Single Chip Embedded Internet
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2016
    *********************************************************************

*/

// Mask/errata management
//
#if defined MASK_2N45K                                                   // Kinetis KE masks
    #define ERRATA_ID_6946                                               // Core: a debugger write to the I/O port might be corrupted during a processor write
    #define ERRATA_ID_6945                                               // Core: processor executing at HardFault priority might enter Lockup state if an NMI occurs during a waited debugger transaction
#elif defined MASK_3N40J
    #define ERRATA_ID_6946                                               // Core: a debugger write to the I/O port might be corrupted during a processor write
    #define ERRATA_ID_6945                                               // Core: processor executing at HardFault priority might enter Lockup state if an NMI occurs during a waited debugger transaction
    #define ERRATA_ID_6749                                               // I2C: the I2C_C1[MST] bit is not automatically cleared when arbitration is lost
#elif defined MASK_0N22J                                                 // KE02Z
    #define ERRATA_ID_6946                                               // Core: a debugger write to the I/O port might be corrupted during a processor write
    #define ERRATA_ID_6945                                               // Core: processor executing at HardFault priority might enter Lockup state if an NMI occurs during a waited debugger transaction
    #define ERRATA_ID_7331                                               // IO: high current drive pins not in high-Z state during power up
    #define ERRATA_ID_7040                                               // SOC: slow VDD ramp-up might cause unstable startup on some devices during power up at cold temperatures
#elif defined MASK_1N74G                                                 // Kinetis M masks
    #define ERRATA_ID_6946                                               // Core: a debugger write to the I/O port might be corrupted during a processor write
    #define ERRATA_ID_6945                                               // Core: processor executing at HardFault priority might enter Lockup state if an NMI occurs during a waited debugger transaction
    #define ERRATA_ID_6749                                               // I2C: the I2C_C1[MST] bit is not automatically cleared when arbitration is lost
    #define ERRATA_ID_6665                                               // Operating requirements: Limitation of the device operating range
    #define ERRATA_ID_7288                                               // SOC : iRTC current increases when fast IRC is enabled
    #define ERRATA_ID_7289                                               // SOC : iRTC current through Vbat increases when CPU accesses any peripheral registers and is proportional to the BUS frequency
    #define ERRATA_ID_7027                                               // UART: during ISO-7816 T=0 initial character detection invalid initial characters are stored in the RxFIFO
    #define ERRATA_ID_7028                                               // UART: during ISO-7816 initial character detection the parity, framing, and noise error flags can set
    #define ERRATA_ID_6472                                               // UART: ETU compensation needed for ISO-7816 wait time (WT) and block wait time (BWT)
    #define ERRATA_ID_4647                                               // UART: flow control timing issue can result in loss of characters if FIFO is not enabled
    #define ERRATA_ID_7029                                               // UART: in ISO-7816 T=1 mode, CWT interrupts assert at both character and block boundaries
    #define ERRATA_ID_7090                                               // UART: in ISO-7816 mode, timer interrupts flags do not clear
    #define ERRATA_ID_7031                                               // UART: in single wire receive mode UART will attempt to transmit if data is written to UART_D
    #define ERRATA_ID_5704                                               // UART: TC bit in UARTx_S1 register is set before the last character is sent out in ISO7816 T=0 mode
    #define ERRATA_ID_7091                                               // UART: UART_S1[NF] and UART_S1[PE] can set erroneously while UART_S1[FE] is set
    #define ERRATA_ID_7092                                               // UART: UART_S1[TC] is not cleared by queuing a preamble or break character
#elif defined MASK_0N33H                                                 // Kinetis KL masks
    #define ERRATA_ID_6665                                               // Operating requirements: Limitation of the device operating range
#elif defined MASK_0N40H
    #define ERRATA_ID_6609                                               // I2C1: DMA transfers using certain OUTDIV1 and OUTDIV4 clock divider combinations will result in corrupted data
    #define ERRATA_ID_6395                                               // MCG: fast IRC fine trim bit is not used by the MCG auto trim machine
    #define ERRATA_ID_6665                                               // Operating requirements: Limitation of the device operating range
    #define ERRATA_ID_6580                                               // SPI1: DMA transfers using certain OUTDIV1 and OUTDIV4 clock divider combinations will result in corrupted data
    #define ERRATA_ID_6060                                               // TSI: out of range interrupt shows incorrect behavior with some configurations
    #define ERRATA_ID_6396                                               // sLCD: LCD_GCR[RVTRIM] bits are in reverse order
#elif defined MASK_0N50M
    #define ERRATA_ID_6939                                               // Core: interrupted loads to SP can cause erroneous behavior
    #define ERRATA_ID_6940                                               // Core: VDIV or VSQRT instructions might not complete correctly when very short ISRs are used
    #define ERRATA_ID_8010                                               // LLWU: CMP flag in LLWU_Fx register cleared by multiple CMP out toggles when exiting LLSx or VLLSx modes
    #define ERRATA_ID_7950                                               // LLWU: when exiting from Low Leakage Stop (LLS) mode using the comparator, the comparator ISR is serviced before the LLWU ISR
    #define ERRATA_ID_7993                                               // MCG: FLL frequency may be incorrect after changing the FLL reference clock
    #define ERRATA_ID_7735                                               // MCG: IREFST status bit may set before the IREFS multiplexor switches the FLL reference clock
    #define ERRATA_ID_7914                                               // PIT: after enabling the Periodic Interrupt Timer (PIT) clock gate, an attempt to immediately enable the PIT module may not be successful
    #define ERRATA_ID_4647                                               // UART: flow control timing issue can result in loss of characters if FIFO is not enabled
    #define ERRATA_ID_7857                                               // UART: WT timer in T=0 mode and CWT timer in T=1 mode can expire between 0.2 ETU to 0.8 ETU earlier than programmed
    #define ERRATA_ID_7919                                               // USBOTG: in certain situations, software updates to the Start of Frame Threshold Register (USBx_SOFTHLD) may lead to an End of Frame error condition
#elif defined MASK_0N51M
    #define ERRATA_ID_6939                                               // Core: interrupted loads to SP can cause erroneous behavior
    #define ERRATA_ID_6940                                               // Core: VDIV or VSQRT instructions might not complete correctly when very short ISRs are used
    #define ERRATA_ID_8096                                               // DAC12: DNL for DAC12 operating in Low-Power Mode larger than specification for some devices
    #define ERRATA_ID_8010                                               // LLWU: CMP flag in LLWU_Fx register cleared by multiple CMP out toggles when exiting LLSx or VLLSx modes
    #define ERRATA_ID_7950                                               // LLWU: when exiting from Low Leakage Stop (LLS) mode using the comparator, the comparator ISR is serviced before the LLWU ISR
    #define ERRATA_ID_7986                                               // LPUART: the LPUART_TX pin is tri-stated when the transmitter is disabled
    #define ERRATA_ID_7993                                               // MCG: FLL frequency may be incorrect after changing the FLL reference clock
    #define ERRATA_ID_7735                                               // MCG: IREFST status bit may set before the IREFS multiplexor switches the FLL reference clock
    #define ERRATA_ID_7914                                               // PIT: after enabling the Periodic Interrupt Timer (PIT) clock gate, an attempt to immediately enable the PIT module may not be successful
    #define ERRATA_ID_4647                                               // UART: flow control timing issue can result in loss of characters if FIFO is not enabled
    #define ERRATA_ID_7857                                               // UART: WT timer in T=0 mode and CWT timer in T=1 mode can expire between 0.2 ETU to 0.8 ETU earlier than programmed
    #define ERRATA_ID_7919                                               // USBOTG: in certain situations, software updates to the Start of Frame Threshold Register (USBx_SOFTHLD) may lead to an End of Frame error condition
    #define ERRATA_ID_8101                                               // USBOTG: USB host signal crossover voltage higher than specification at low temperature
#elif defined MASK_0N78M
    #define ERRATA_ID_6990                                               // CJTAG: possible incorrect TAP state machine advance during Check Packet
    #define ERRATA_ID_6939                                               // Core: interrupted loads to SP can cause erroneous behavior
    #define ERRATA_ID_6940                                               // Core: VDIV or VSQRT instructions might not complete correctly when very short ISRs are used
    #define ERRATA_ID_6749                                               // I2C: the I2C_C1[MST] bit is not automatically cleared when arbitration is lost
    #define ERRATA_ID_8010                                               // LLWU: CMP flag in LLWU_Fx register cleared by multiple CMP out toggles when exiting LLSx or VLLSx modes
    #define ERRATA_ID_7950                                               // LLWU: when exiting from Low Leakage Stop (LLS) mode using the comparator, the comparator ISR is serviced before the LLWU ISR
    #define ERRATA_ID_7993                                               // MCG: FLL frequency may be incorrect after changing the FLL reference clock
    #define ERRATA_ID_7735                                               // MCG: IREFST status bit may set before the IREFS multiplexor switches the FLL reference clock
    #define ERRATA_ID_7914                                               // PIT: after enabling the Periodic Interrupt Timer (PIT) clock gate, an attempt to immediately enable the PIT module may not be successful
    #define ERRATA_ID_7027                                               // UART: during ISO-7816 T=0 initial character detection invalid initial characters are stored in the RxFIFO
    #define ERRATA_ID_7028                                               // UART: during ISO-7816 initial character detection the parity, framing, and noise error flags can set
    #define ERRATA_ID_6472                                               // UART: ETU compensation needed for ISO-7816 wait time (WT) and block wait time (BWT)
    #define ERRATA_ID_4647                                               // UART: flow control timing issue can result in loss of characters if FIFO is not enabled
    #define ERRATA_ID_7029                                               // UART: in ISO-7816 T=1 mode, CWT interrupts assert at both character and block boundaries
    #define ERRATA_ID_7090                                               // UART: in ISO-7816 mode, timer interrupts flags do not clear
    #define ERRATA_ID_7031                                               // UART: in single wire receive mode UART will attempt to transmit if data is written to UART_D
    #define ERRATA_ID_5704                                               // UART: TC bit in UARTx_S1 register is set before the last character is sent out in ISO7816 T=0 mode
    #define ERRATA_ID_7091                                               // UART: UART_S1[NF] and UART_S1[PE] can set erroneously while UART_S1[FE] is set
    #define ERRATA_ID_7092                                               // UART: UART_S1[TC] is not cleared by queuing a preamble or break character
    #define ERRATA_ID_7919                                               // USBOTG: in certain situations, software updates to the Start of Frame Threshold Register (USBx_SOFTHLD) may lead to an End of Frame error condition
    #define ERRATA_ID_8101                                               // USBOTG: USB host signal crossover voltage higher than specification at low temperature
#elif defined MASK_1N15J
    #define ERRATA_ID_6946                                               // Core: a debugger write to the I/O port might be corrupted during a processor write
    #define ERRATA_ID_6945                                               // Core: processor executing at HardFault priority might enter Lockup state if an NMI occurs during a waited debugger transaction
    #define ERRATA_ID_6395                                               // MCG: fast IRC fine trim bit is not used by the MCG auto trim machine
#elif defined MASK_1N40H
    #define ERRATA_ID_6396                                               // sLCD: LCD_GCR[RVTRIM] bits are in reverse order
#elif defined MASK_1N41k
#elif defined MASK_1N71K
    #define ERRATA_ID_3863                                               // ADC: in 16-bit differential mode, ADC may result in a conversion error when positive input is near upper rail reference voltage
    #define ERRATA_ID_7950                                               // LLWU: when exiting from Low Leakage Stop (LLS) mode using the comparator, the comparator ISR is serviced before the LLWU ISR
    #define ERRATA_ID_7986                                               // LPUART: the LPUART_TX pin is tri-stated when the transmitter is disabled
    #define ERRATA_ID_7914                                               // PIT: after enabling the Periodic Interrupt Timer (PIT) clock gate, an attempt to immediately enable the PIT module may not be successful
    #define ERRATA_ID_2580                                               // UART: start bit sampling not compliant with LIN 2.1 specification
    #define ERRATA_ID_7857                                               // UART: WT timer in T=0 mode and CWT timer in T=1 mode can expire between 0.2 ETU to 0.8 ETU earlier than programmed
    #define ERRATA_ID_7919                                               // USBOTG: in certain situations, software updates to the Start of Frame Threshold Register (USBx_SOFTHLD) may lead to an End of Frame error condition
    #define ERRATA_ID_7822                                               // USBReg: some devices have VREG_OUT trim value incorrectly programmed
    #define ERRATA_ID_6396                                               // sLCD: LCD_GCR[RVTRIM] bits are in reverse order
#elif defined MASK_1N96F
    #define ERRATA_ID_6070                                               // I2C: repeat start cannot be generated if the I2Cx_F[MULT] field is set to a non-zero value
    #define ERRATA_ID_6665                                               // Operating requirements: Limitation of the device operating range
    #define ERRATA_ID_5472                                               // SMC: mode transition VLPR->VLLS0 (POR disabled)->RUN, will cause POR & LVD
    #define ERRATA_ID_6060                                               // TSI: out of range interrupt shows incorrect behavior with some configurations
#elif defined MASK_1N97K
#elif defined MASK_2N97F
#elif defined MASK_0M33Z                                                 // Kinetis K masks
    #define ERRATA_ID_2550                                               // ADC: ADC abort conversion logic error
    #define ERRATA_ID_3863                                               // ADC: conversion error when positive input is near upper rail reference voltage
    #define ERRATA_ID_2776                                               // CRC: may have incorrect CDC rsult when performing CRC 8 or 16 bit writes with transpose enabled
    #define ERRATA_ID_2547                                               // DAC: 12-bit DAC buffer registers cannot be read
    #define ERRATA_ID_2670                                               // ENET: IEEE 1588 TS_AVAIL interrut is incorrectly mapped to NVIC vector 94
    #define ERRATA_ID_2579                                               // ENET: no support for IEEE 1588 TS_TIMER timestamp timer overflow interrupt
    #define ERRATA_ID_2596                                               // EzPort and FTFL: the 64-bit flash programming command PGMSEC is not fully implemented
    #define ERRATA_ID_2647                                               // FMC: cache aliasing is not supported on 512k and 384k program flash onyl devices
    #define ERRATA_ID_2687                                               // FMC: flash clock divider setting for divide-by-1 is not allowed
    #define ERRATA_ID_2448                                               // FMC: flash prefetch could result in incorect read data
    #define ERRATA_ID_2671                                               // FMC: incorrect data returned during speculative access
    #define ERRATA_ID_2590                                               // FMC: master access protection encoding for write only access does not work as specified
    #define ERRATA_ID_2683                                               // FMC: program flash only configuration cache/page buffer bank control does not follow specification
    #define ERRATA_ID_2644                                               // FMC: ppeculation logic is not supported on program flash only or program flash only with swap feature devices
    #define ERRATA_ID_2668                                               // FTFL: brown-out recovery during a 32-bit EEE write operation is not supported
    #define ERRATA_ID_3854                                               // FTFL: cannot connect to MCU via JTAG or EzPort interfaces after a Reset, POR, LVD event during a program sequence
    #define ERRATA_ID_2667                                               // FTFL: EEE brownout recovery after SETRAM command is not supported
    #define ERRATA_ID_2592                                               // FTFL: EEESPLIT feature not fully implemented
    #define ERRATA_ID_3374                                               // FTFL: erase operation is not reliable if VDD is approximately 1.9 V or less
    #define ERRATA_ID_3372                                               // FTFL: reset during an EEE program operation may result in an invalid EEE read access
    #define ERRATA_ID_3855                                               // FTFL: SWAP feature is not supported
    #define ERRATA_ID_2597                                               // FTFL: version ID field is not supported
    #define ERRATA_ID_2784                                               // FTFL: warm reset during EEE copy-down prevents EEE copy down from successfully completing
    #define ERRATA_ID_2781                                               // FlexBus: false bus error on back-to-back writes when flash memory is secure
    #define ERRATA_ID_2616                                               // FlexCAN: module receives data frames sent by itself although the self reception feature is disabled
    #define ERRATA_ID_2583                                               // FlexCAN: OSCERCLK clock source must be enabled for CAN to function properly
    #define ERRATA_ID_2522                                               // FlexCAN: Rx FIFO Overflow bit is not set when one frame was read after a match when the FIFO is full
    #define ERRATA_ID_2545                                               // GPIO: PTB[0:11], PTB[16:23], PTC[0:19], and PTD[0:7] are not 5 V tolerant
    #define ERRATA_ID_3402                                               // GPIO: XTAL pin cannot be used as GPIO if the ERCLKEN bit is set
    #define ERRATA_ID_2793                                               // I2C: MCU does not wake from STOP mode on subsequent address matches if previous address is mismatched
    #define ERRATA_ID_3795                                               // I2S: fractional divider in SIM_CLKDIV2 is not reset when recovering from VLLSx low power modes
    #define ERRATA_ID_3714                                               // I2S: MCLK output is disabled in asynchronous mode
    #define ERRATA_ID_2613                                               // LLWU: MCU may not exit properly from VLLS3, VLLS2 or VLLS1 modes via a pin reset or other wakeup sources
    #define ERRATA_ID_2674                                               // LLWU: LLWU glitch filter for pin and reset is not supported
    #define ERRATA_ID_2680                                               // MC: after recovery from LLS low power mode triggered by assertion of the RESET pin, the MC_SRSL[WAKEUP] bit is not set as expected
    #define ERRATA_ID_2678                                               // MC: MC_SRS[PIN] is not always set after exiting a VLLS mode due to a RESET pin assertion
    #define ERRATA_ID_2676                                               // MC: when waking the system from VLLS modes via a RESET pin, the I/O are not immediately released to their reset state
    #define ERRATA_ID_3801                                               // MCG: in FEE and FEI Modes, the FLL output may stop when the DCO range is changed
    #define ERRATA_ID_2553                                               // MCG: MCGPLLCLK stalls for 2 PLL cycles if MCG_C5 is written to after the PLL is enabled
    #define ERRATA_ID_2556                                               // MCG: PLLCLKEN may not always enable the PLL
    #define ERRATA_ID_2796                                               // MCG: slow internal reference clock operating range does not meet specification
    #define ERRATA_ID_2555                                               // MCG: DCO within the FLL cannot reach the minimum specified frequency
    #define ERRATA_ID_2660                                               // MCG: PLL can be enabled in bypassed low power mode (BLPE) when the system is in normal stop mode
    #define ERRATA_ID_3580                                               // MCG: total deviation of trimmed average DCO output frequency over voltage and temperature does not meet specification
    #define ERRATA_ID_2554                                               // MCG: when trimming the slow Internal Reference Clock (IRC) using the auto trim machine (ATM), the IRC is not automatically enabled
    #define ERRATA_ID_3794                                               // NVIC: NMI interrupt does not wakeup MCU from STOP and VLPS
    #define ERRATA_ID_2682                                               // PIT: does not generate a subsequent interrupt after clearing the interrupt flag
    #define ERRATA_ID_2548                                               // PMC: Power-on reset (POR) sensitive to noise on Vdd
    #define ERRATA_ID_4482                                               // PMC: STOP mode recovery unstable
    #define ERRATA_ID_2756                                               // PMC: static low power modes including STOP, VLPS, LLS, VLLSx are higher than specified
    #define ERRATA_ID_2706                                               // PMC: VLLS3 and VLLS2 recovery time is longer than specified
    #define ERRATA_ID_2542                                               // PMC: Very Low Power Run (VLPR) and Very Low Power Wait (VLPW) power modes are not supported
    #define ERRATA_ID_2541                                               // PORT: JTAG debug connectivity can not be achieved when using the RMII_RXER/MII_RXER alternate pin function on port pin PTA5 to connect to an Ethernet PHY
    #define ERRATA_ID_2578                                               // RTC: do not set RTC Update Mode via the RTC_CR[UM] bit
    #define ERRATA_ID_2573                                               // RTC: RTC interrupt is gated by the RTC_SR[TCE] bit - the interrupt will not assert on VBAT POR, VBAT power down, or when TCE = 0
    #define ERRATA_ID_2575                                               // RTC: Time Alarm Flag is cleared whenever the Seconds Counter increments and the Seconds Counter does not equal the Alarm Register
    #define ERRATA_ID_2655                                               // RTC: when RTC clock gating is disabled, accessing the VBAT register file causes the bus to hang
    #define ERRATA_ID_2577                                               // RTC: when RTC_CR[CLKO] = 1, the 32 kHz RTC clock to the rest of the device is not disabled
    #define ERRATA_ID_2576                                               // RTC: when the RTC is configured to allow supervisor access only, the write and read access registers can be modified in user mode
    #define ERRATA_ID_2574                                               // RTC: writing RTC_TAR[TAR] = 0 does not disable RTC alarm
    #define ERRATA_ID_3997                                               // Reset and Boot: MCU may fail to exit reset correctly
    #define ERRATA_ID_3981                                               // SDHC: ADMA fails when data length in the last descriptor is less or equal to 4 bytes
    #define ERRATA_ID_3982                                               // SDHC: ADMA transfer error when the block size is not a multiple of four
    #define ERRATA_ID_4624                                               // SDHC: AutoCMD12 and R1b polling problem
    #define ERRATA_ID_3977                                               // SDHC: does not support Infinite Block Transfer Mode
    #define ERRATA_ID_4627                                               // SDHC: erroneous CMD CRC error and CMD Index error may occur on sending new CMD during data transfer
    #define ERRATA_ID_3980                                               // SDHC: glitch is generated on card clock with software reset or clock divider change
    #define ERRATA_ID_3983                                               // SDHC: problem when ADMA2 last descriptor is LINK or NOP
    #define ERRATA_ID_3978                                               // SDHC: software cannot clear DMA interrupt status bit after read operation
    #define ERRATA_ID_3984                                               // SDHC: eSDHC misses SDIO interrupt when CINT is disabled
    #define ERRATA_ID_2572                                               // TPIU: Trace Port Interface Unit (TPIU) data setup and hold times do not conform to the ARM timing specification, ARM IHI0014O, section 8.4, "Timing specifications"
    #define ERRATA_ID_2591                                               // TSI: TSI_SCANC[SMOD] behaves as an inactive time instead of a scan period value
    #define ERRATA_ID_2638                                               // TSI: the counter registers are not immediately updated after the EOSF bit is set
    #define ERRATA_ID_2582                                               // UART: flow control timing issue can result in loss of characters
    #define ERRATA_ID_3892                                               // UART: ISO-7816 automatic initial character detect feature not working correctly
    #define ERRATA_ID_2584                                               // UART: possible conflicts between UART interrupt service routines and DMA requests
    #define ERRATA_ID_2544                                               // USB Voltage Regulator: universal serial bus (USB) regulator standby mode is not supported
    #define ERRATA_ID_2666                                               // VBAT: applying a fast ramp (>50V/ms) on the VBAT pin can cause both VBAT and VDD to latch up
    #define ERRATA_ID_2686                                               // WDOG: a watchdog reset while the system is in STOP or VLPS modes causes an incorrect wakeup sequence
#elif defined MASK_0N03G
#elif defined MASK_0N36M
    #define ERRATA_ID_6939                                               // Core: interrupted loads to SP can cause erroneous behavior
    #define ERRATA_ID_6940                                               // Core: VDIV or VSQRT instructions might not complete correctly when very short ISRs are used
    #define ERRATA_ID_8010                                               // LLWU: CMP flag in LLWU_Fx register cleared by multiple CMP out toggles when exiting LLSx or VLLSx modes
    #define ERRATA_ID_7950                                               // LLWU: when exiting from Low Leakage Stop (LLS) mode using the comparator, the comparator ISR is serviced before the LLWU ISR
    #define ERRATA_ID_7993                                               // MCG: FLL frequency may be incorrect after changing the FLL reference clock
    #define ERRATA_ID_7735                                               // MCG: IREFST status bit may set before the IREFS multiplexor switches the FLL reference clock
    #define ERRATA_ID_7914                                               // PIT: after enabling the Periodic Interrupt Timer (PIT) clock gate, an attempt to immediately enable the PIT module may not be successful
    #define ERRATA_ID_4647                                               // UART: flow control timing issue can result in loss of characters if FIFO is not enabled
    #define ERRATA_ID_8011                                               // eDMA: possible corruption when writing an eDMA descriptor while the eDMA is active
#elif defined MASK_0N41J
#elif defined MASK_0N62J
#elif defined MASK_0N74K
    #define ERRATA_ID_6939                                               // Core: interrupted loads to SP can cause erroneous behavior
    #define ERRATA_ID_6940                                               // Core: VDIV or VSQRT instructions might not complete correctly when very short ISRs are used
    #define ERRATA_ID_8010                                               // LLWU: CMP flag in LLWU_Fx register cleared by multiple CMP out toggles when exiting LLSx or VLLSx modes
    #define ERRATA_ID_7950                                               // LLWU: when exiting from Low Leakage Stop (LLS) mode using the comparator, the comparator ISR is serviced before the LLWU ISR
    #define ERRATA_ID_7986                                               // LPUART: the LPUART_TX pin is tri-stated when the transmitter is disabled
    #define ERRATA_ID_7993                                               // MCG: FLL frequency may be incorrect after changing the FLL reference clock
    #define ERRATA_ID_7998                                               // USB: USB host signal crossover voltage higher than specification at low temp or high voltage
    #define ERRATA_ID_7735                                               // MCG: IREFST status bit may set before the IREFS multiplexor switches the FLL reference clock
    #define ERRATA_ID_7914                                               // PIT: after enabling the Periodic Interrupt Timer (PIT) clock gate, an attempt to immediately enable the PIT module may not be successful
    #define ERRATA_ID_4647                                               // UART: flow control timing issue can result in loss of characters if FIFO is not enabled
    #define ERRATA_ID_7857                                               // UART: WT timer in T=0 mode and CWT timer in T=1 mode can expire between 0.2 ETU to 0.8 ETU earlier than programmed
    #define ERRATA_ID_7919                                               // USBOTG: in certain situations, software updates to the Start of Frame Threshold Register (USBx_SOFTHLD) may lead to an End of Frame error condition
    #define ERRATA_ID_8011                                               // eDMA: possible corruption when writing an eDMA descriptor while the eDMA is active
#elif defined MASK_0N89E
#elif defined MASK_0N96B
    #define ERRATA_ID_7166                                               // SOC: SDHC, NFC, USBOTG, and cache modules are not clocked correctly in low-power modes (this errata is not listed but found to be vaid)
    #define ERRATA_ID_2550                                               // ADC: ADC abort conversion logic error
    #define ERRATA_ID_3863                                               // ADC: in 16-bit differential mode, ADC may result in a conversion error when positive input is near upper rail reference voltage
    #define ERRATA_ID_4588                                               // DMAMUX: when using PIT with "always enabled" request, DMA request does not deassert correctly
    #define ERRATA_ID_4154                                               // DryIce/RTC: analog clock tamper detect circuit is not operational
    #define ERRATA_ID_4170                                               // DryIce/RTC: analog temperature tamper detect circuit is not operational
    #define ERRATA_ID_3937                                               // EWM: interrupt not connected
    #define ERRATA_ID_5234                                               // GPIO: PORTF registers are not accessible unless SIM_SCGC5[PORTE] is set
    #define ERRATA_ID_2793                                               // I2C: MCU does not wake as expected from STOP or VLPS mode on subsequent address matches if previous address is mismatched
    #define ERRATA_ID_3925                                               // JTAG/OSC: JTAG enables digital input buffers
    #define ERRATA_ID_4553                                               // LCDC: Cursor Color Limited in 24bpp Mode
    #define ERRATA_ID_4569                                               // LCDC: Graphic Window Color Key Limited in 24bpp Mode
    #define ERRATA_ID_4570                                               // LCDC: Graphic Window May Shift Pixel Data
    #define ERRATA_ID_4571                                               // LCDC: LCDC can only use half of internal SRAM for frame buffer
    #define ERRATA_ID_3929                                               // MC: LLS and VLPS low power modes do not meet specified values
    #define ERRATA_ID_3898                                               // MCG: setting the MCG_C6[PLLS] bit will enable both OSC0 and OSC1.
    #define ERRATA_ID_4176                                               // NMI: NMI interrupt service routine (ISR) might not be called when MCU wakes up from VLLSx modes.
    #define ERRATA_ID_3794                                               // NVIC: NMI interrupt does not wakeup MCU from STOP and VLPS
    #define ERRATA_ID_4481                                               // PMC: STOP mode recovery unstable
    #define ERRATA_ID_4065                                               // PTD4 pin is not 5V tolerant
    #define ERRATA_ID_5138                                               // RTC: the RTC_WAKEUP pin does not operate as specified and the WPON bit in RTC_IER register does not work
    #define ERRATA_ID_5176                                               // Reset and Boot: Device may not exit the power on reset (POR) event correctly
    #define ERRATA_ID_5130                                               // SAI: under certain conditions, the CPU cannot reenter STOP mode via an asynchronous interrupt wakeup event
    #define ERRATA_ID_3981                                               // SDHC: ADMA fails when data length in the last descriptor is less or equal to 4 bytes
    #define ERRATA_ID_3982                                               // SDHC: ADMA transfer error when the block size is not a multiple of four
    #define ERRATA_ID_4624                                               // SDHC: AutoCMD12 and R1b polling problem
    #define ERRATA_ID_3977                                               // SDHC: does not support Infinite Block Transfer Mode
    #define ERRATA_ID_4627                                               // SDHC: erroneous CMD CRC error and CMD Index error may occur on sending new CMD during data transfer
    #define ERRATA_ID_3980                                               // SDHC: glitch is generated on card clock with software reset or clock divider change
    #define ERRATA_ID_6934                                               // SDHC: issues with card removal/insertion detection
    #define ERRATA_ID_3978                                               // SDHC: software cannot clear DMA interrupt status bit after read operation
    #define ERRATA_ID_3984                                               // SDHC: eSDHC misses SDIO interrupt when CINT is disabled
    #define ERRATA_ID_3941                                               // SIM/DDR: SIM_SOPT2[FBSL] does not determine allowable DDR controller accesses when security is enabled
    #define ERRATA_ID_3936                                               // SIM/ENET: Incorrect reset value for SIM_SCGC2[ENET]
    #define ERRATA_ID_4218                                               // SIM/FLEXBUS: SIM_SCGC7[FLEXBUS] bit should be cleared when the FlexBus is not being used
    #define ERRATA_ID_3935                                               // SIM: incorrect reset value for SIM_SCGC5[1]
    #define ERRATA_ID_3942                                               // SIM: incorrect signal output on CLKOUT pin when SIM_SOPT2[CLKOUTSEL] = 011
    #define ERRATA_ID_4189                                               // SOC: limitation of operating temperature range
    #define ERRATA_ID_3928                                               // TSI: delta voltage is 400 mV instead of 600 mV
    #define ERRATA_ID_3926                                               // TSI: The TSI will run several scan cycles during reference clock instead of scanning each electrode once
    #define ERRATA_ID_2638                                               // TSI: the counter registers are not immediately updated after the EOSF bit is set
    #define ERRATA_ID_4181                                               // SOC: limitation of operating temperature range
    #define ERRATA_ID_4935                                               // UART: CEA709.1 features not supported
    #define ERRATA_ID_2582                                               // UART: flow control timing issue can result in loss of characters
    #define ERRATA_ID_4945                                               // UART: ISO-7816 T=1 mode receive data format with a single stop bit is not supported
    #define ERRATA_ID_3892                                               // UART: ISO-7816 automatic initial character detect feature not working correctly
    #define ERRATA_ID_3964                                               // JTAGC: when debug is active a wakeup from STOP or VLPS with interrupt causes a hard fault interrupt
#elif defined MASK_1N30D_2N30D
#elif defined MASK_1N83J
    #define ERRATA_ID_6990                                               // CJTAG: possible incorrect TAP state machine advance during Check Packet
    #define ERRATA_ID_6939                                               // Core: interrupted loads to SP can cause erroneous behavior
    #define ERRATA_ID_6940                                               // Core: VDIV or VSQRT instructions might not complete correctly when very short ISRs are used
    #define ERRATA_ID_6749                                               // I2C: the I2C_C1[MST] bit is not automatically cleared when arbitration is lost
    #define ERRATA_ID_3981                                               // SDHC: ADMA fails when data length in the last descriptor is less or equal to 4 bytes
    #define ERRATA_ID_3982                                               // SDHC: ADMA transfer error when the block size is not a multiple of four
    #define ERRATA_ID_4627                                               // SDHC: erroneous CMD CRC error and CMD Index error may occur on sending new CMD during data transfer
    #define ERRATA_ID_3983                                               // SDHC: problem when ADMA2 last descriptor is LINK or NOP
    #define ERRATA_ID_7534                                               // SUBFAMID read back incorrect sub-family of the Kinetis device
    #define ERRATA_ID_7027                                               // UART: during ISO-7816 T=0 initial character detection invalid initial characters are stored in the RxFIFO
    #define ERRATA_ID_7028                                               // UART: during ISO-7816 initial character detection the parity, framing, and noise error flags can set
    #define ERRATA_ID_6472                                               // UART: ETU compensation needed for ISO-7816 wait time (WT) and block wait time (BWT)
    #define ERRATA_ID_4647                                               // UART: flow control timing issue can result in loss of characters if FIFO is not enabled
    #define ERRATA_ID_7029                                               // UART: in ISO-7816 T=1 mode, CWT interrupts assert at both character and block boundaries
    #define ERRATA_ID_7090                                               // UART: in ISO-7816 mode, timer interrupts flags do not clear
    #define ERRATA_ID_7031                                               // UART: in single wire receive mode UART will attempt to transmit if data is written to UART_D
    #define ERRATA_ID_5704                                               // UART: TC bit in UARTx_S1 register is set before the last character is sent out in ISO7816 T=0 mode
    #define ERRATA_ID_7091                                               // UART: UART_S1[NF] and UART_S1[PE] can set erroneously while UART_S1[FE] is set
    #define ERRATA_ID_7092                                               // UART: UART_S1[TC] is not cleared by queuing a preamble or break character
    #define ERRATA_ID_6933                                               // eDMA: possible misbehavior of a preempted channel when using continuous link mode
    // The next errata is not included in the list in the document but has been found to be required with optimising compilers
    //
    #define ERRATA_ID_7914                                               // PIT: after enabling the Periodic Interrupt Timer (PIT) clock gate, an attempt to immediately enable the PIT module may not be successful
#elif defined MASK_1N89E
#elif defined MASK_1N96B
#elif defined MASK_2N03G
#elif defined MASK_2N22D
#elif defined MASK_3N03G
#elif defined MASK_3N96B
    #define ERRATA_ID_6378                                               // Cache: cache write buffer error enable (MCM_ISCR[CWBEE]) does not work
    #define ERRATA_ID_6939                                               // Core: interrupted loads to SP can cause erroneous behavior
    #define ERRATA_ID_6940                                               // Core: VDIV or VSQRT instructions might not complete correctly when very short ISRs are used
    #define ERRATA_ID_5243                                               // DDRMC: it is possible for data corruption to occur If the value programmed in RDLATADJ is larger than the CAS latency
    #define ERRATA_ID_5241                                               // DDRMC: putting the memory into a power down mode while DDR_CR11[AREFMODE] is set can prevent the DDRMC from providing refreshes to the memory while it is powered down
    #define ERRATA_ID_5263                                               // DDRMC: while the memory is in a power down state, setting DDR_CR11[AREF] in an attempt to force a refresh command will cause a refresh cycle to be missed
    #define ERRATA_ID_4588                                               // DMAMUX: when using PIT with "always enabled" request, DMA request does not deassert correctly
    #define ERRATA_ID_6358                                               // ENET: write to Transmit Descriptor Active Register (ENET_TDAR) is ignored
    #define ERRATA_ID_5861                                               // FTM: FTM2 and FTM3 do not correctly enter BDM mode when a debugger connection is active
    #define ERRATA_ID_4710                                               // FTM: FTMx_PWMLOAD register does not support 8-/16-bit accesses
    #define ERRATA_ID_6484                                               // FTM: the process of clearing the FTMx_SC[TOF] bit does not work as expected under a certain condition when the FTM counter reaches FTM_MOD value
    #define ERRATA_ID_6573                                               // JTAG: JTAG TDO function on the PTA2 disables the pull resistor
    #define ERRATA_ID_3964                                               // JTAGC: when debug is active a wakeup from STOP or VLPS with interrupt causes a hard fault interrupt
    #define ERRATA_ID_4553                                               // LCDC: cursor color limited in 24bpp Mode
    #define ERRATA_ID_4569                                               // LCDC: graphic window color key limited in 24bpp Mode
    #define ERRATA_ID_4570                                               // LCDC: graphic window may shift pixel data
    #define ERRATA_ID_4571                                               // LCDC: LCDC can only use half of internal SRAM for frame buffer
    #define ERRATA_ID_3898                                               // MCG: setting the MCG_C6[PLLS] bit will enable both OSC0 and OSC1
    #define ERRATA_ID_4590                                               // MCG: transitioning from VLPS to VLPR low power modes while in BLPI clock mode is not supported
    #define ERRATA_ID_4176                                               // NMI: NMI interrupt service routine (ISR) might not be called when MCU wakes up from VLLSx modes
    #define ERRATA_ID_3794                                               // NVIC: NMI interrupt does not wakeup MCU from STOP and VLPS
    #define ERRATA_ID_5927                                               // Operating requirements: change to minimum VDD spec
    #define ERRATA_ID_5667                                               // PMC: when used as an input to ADC or CMP modules, the PMC bandgap 1-V voltage reference is not available in VLPx, LLS, or VLLSx modes
    #define ERRATA_ID_5130                                               // SAI: under certain conditions, the CPU cannot reenter STOP mode via an asynchronous interrupt wakeup event
    #define ERRATA_ID_3981                                               // SDHC: ADMA fails when data length in the last descriptor is less or equal to 4 bytes
    #define ERRATA_ID_3982                                               // SDHC: ADMA transfer error when the block size is not a multiple of four
    #define ERRATA_ID_4624                                               // SDHC: AutoCMD12 and R1b polling problem
    #define ERRATA_ID_3977                                               // SDHC: does not support Infinite Block Transfer Mode
    #define ERRATA_ID_4627                                               // SDHC: erroneous CMD CRC error and CMD Index error may occur on sending new CMD during data transfer
    #define ERRATA_ID_3980                                               // SDHC: glitch is generated on card clock with software reset or clock divider change
    #define ERRATA_ID_6934                                               // SDHC: issues with card removal/insertion detection
    #define ERRATA_ID_3983                                               // SDHC: problem when ADMA2 last descriptor is LINK or NOP
    #define ERRATA_ID_3978                                               // SDHC: software cannot clear DMA interrupt status bit after read operation
    #define ERRATA_ID_3984                                               // SDHC: eSDHC misses SDIO interrupt when CINT is disabled
    #define ERRATA_ID_3941                                               // SIM/DDR: SIM_SOPT2[FBSL] does not determine allowable DDR controller accesses when security is enabled
    #define ERRATA_ID_4218                                               // SIM/FLEXBUS: SIM_SCGC7[FLEXBUS] bit should be cleared when the FlexBus is not being used
    #define ERRATA_ID_5952                                               // SMC: wakeup via the LLWU from LLS/VLLS to RUN to VLPR incorrectly triggers an immediate wakeup from the next low power mode entry
    #define ERRATA_ID_7166                                               // SOC: SDHC, NFC, USBOTG, and cache modules are not clocked correctly in low-power modes
    #define ERRATA_ID_3926                                               // TSI: the TSI will run several scan cycles during reference clock instead of scanning each electrode once
    #define ERRATA_ID_2638                                               // TSI: the counter registers are not immediately updated after the EOSF bit is set
    #define ERRATA_ID_4546                                               // TSI: the counter values reported from TSI increase when in low power modes (LLS, VLLS1, VLLS2, VLLS3)
    #define ERRATA_ID_4181                                               // TSI: when the overrun flag is set, the TSI scanning sequence will exhibit undefined behavior
    #define ERRATA_ID_4935                                               // UART: CEA709.1 features not supported
    #define ERRATA_ID_7027                                               // UART: during ISO-7816 T=0 initial character detection invalid initial characters are stored in the RxFIFO
    #define ERRATA_ID_7028                                               // UART: during ISO-7816 initial character detection the parity, framing, and noise error flags can set
    #define ERRATA_ID_6472                                               // UART: ETU compensation needed for ISO-7816 wait time (WT) and block wait time (BWT)
    #define ERRATA_ID_4647                                               // UART: flow control timing issue can result in loss of characters if FIFO is not enabled
    #define ERRATA_ID_4945                                               // UART: ISO-7816 T=1 mode receive data format with a single stop bit is not supported
    #define ERRATA_ID_3892                                               // UART: ISO-7816 automatic initial character detect feature not working correctly
    #define ERRATA_ID_7029                                               // UART: in ISO-7816 T=1 mode, CWT interrupts assert at both character and block boundaries
    #define ERRATA_ID_7090                                               // UART: in ISO-7816 mode, timer interrupts flags do not clear
    #define ERRATA_ID_7031                                               // UART: in single wire receive mode UART will attempt to transmit if data is written to UART_D
    #define ERRATA_ID_5704                                               // UART: TC bit in UARTx_S1 register is set before the last character is sent out in ISO7816 T=0 mode
    #define ERRATA_ID_7091                                               // UART: UART_S1[NF] and UART_S1[PE] can set erroneously while UART_S1[FE] is set
    #define ERRATA_ID_7092                                               // UART: UART_S1[TC] is not cleared by queuing a preamble or break character
    #define ERRATA_ID_5928                                               // USBOTG: USBx_USBTRC0[USBRESET] bit does not operate as expected in all cases
    #define ERRATA_ID_6933                                               // eDMA: possible misbehavior of a preempted channel when using continuous link mode
#elif defined MASK_1N86K                                                 // KL03
    #define ERRATA_ID_8060                                               // ROM: bytes sent from host over I2C and SPI interfaces may be lost
    #define ERRATA_ID_8058                                               // ROM: COP can't be re-enabled in application code due to being disabled by ROM boot code
    #define ERRATA_ID_8086                                               // ROM: fail to setup connection by UART interface if RX pin is low after POR
    #define ERRATA_ID_8059                                               // ROM: using UART at 57600 bits/s or greater with a core clock of 8 MHz may cause lost bytes
    #define ERRATA_ID_8068                                               // RTC: fail to enter low power mode if RTC time invalid flag (TIF) is not cleared after POR
    #define ERRATA_ID_8085                                               // TPM: writing the TPMx_MOD or TPMx_CnV registers more than once may fail when the timer is disabled
#elif defined MASK_0N65N                                                 // K65
    #define ERRATA_ID_8992                                               // AWIC: early NMI wakeup not detected upon entry to stop mode from VLPR mode
    #define ERRATA_ID_6939                                               // Core: interrupted loads to SP can cause erroneous behavior
    #define ERRATA_ID_6940                                               // Core: VDIV or VSQRT instructions might not complete correctly when very short ISRs are used
    #define ERRATA_ID_7993                                               // MCG: FLL frequency may be incorrect after changing the FLL reference clock
    #define ERRATA_ID_7735                                               // MCG: IREFST status bit may set before the IREFS multiplexor switches the FLL reference clock
    #define ERRATA_ID_3981                                               // SDHC: ADMA fails when data length in the last descriptor is less or equal to 4 bytes
    #define ERRATA_ID_3982                                               // SDHC: ADMA transfer error when the block size is not a multiple of four
    #define ERRATA_ID_4624                                               // SDHC: AutoCMD12 and R1b polling problem
    #define ERRATA_ID_3977                                               // SDHC: does not support Infinite Block Transfer Mode
    #define ERRATA_ID_4627                                               // SDHC: erroneous CMD CRC error and CMD Index error may occur on sending new CMD during data transfer
    #define ERRATA_ID_3984                                               // SDHC: eSDHC misses SDIO interrupt when CINT is disabled
    #define ERRATA_ID_3983                                               // SDHC: problem when ADMA2 last descriptor is LINK or NOP
    #define ERRATA_ID_3978                                               // SDHC: software cannot clear DMA interrupt status bit after read operation
    #define ERRATA_ID_4647                                               // UART: flow control timing issue can result in loss of characters if FIFO is not enabled
    #define ERRATA_ID_8807                                               // USB: in host mode, transmission errors may occur when communicating with a low speed (LS) device through a USB hub
    #define ERRATA_ID_7919                                               // USBOTG: in certain situations, software updates to the Start of Frame Threshold Register (USBx_SOFTHLD) may lead to an End of Frame error condition
    #define ERRATA_ID_9359                                               // USBReg: in some conditions when using both VREG_INn inputs, the USB voltage regulator current limit can fall below specification
#elif defined MASK_4N22D
#elif defined MASK_4N30D
#elif defined MASK_4N96B
#elif defined MASK_5N22D
#elif defined MASK_8N30D
#elif defined MASK_1N86B                                                 // Kinetis K - 50MHz masks
#elif defined MASK_1N36B                                                 // Kinetis K - 72MHz masks
#endif
