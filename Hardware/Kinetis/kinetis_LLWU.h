/***********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher
    
    ---------------------------------------------------------------------
    File:      kinetis_LLWU.h
    Project:   Single Chip Embedded Internet
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2016
    *********************************************************************

*/

#if defined _LLWU_INTERRUPT_CODE

static const unsigned char cWakeupPorts[PORTS_AVAILABLE][PORT_WIDTH] = { // warning - when this is modified, the mirror version in kinetisSim.c must be updated accordingly
    #if defined KINETIS_KL03
    // PTA0..PTA31
    //
    { LLWU_P7, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP },
    // PTB0..PTB31
    //
    { LLWU_P4, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, LLWU_P4, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP },
    #elif defined KINETIS_KL05
    // PTA0..PTA31
    //
    { LLWU_P7, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, LLWU_P0, LLWU_P1, LLWU_P2, LLWU_P3, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP },
    // PTB0..PTB31
    //
    { LLWU_P4, NO_WAKEUP, LLWU_P5, NO_WAKEUP, LLWU_P6, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, LLWU_P4, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP },

    #elif defined KINETIS_KL46
    // PTA0..PTA31
    //
    { LLWU_P7, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, LLWU_P0, LLWU_P1, LLWU_P2, LLWU_P3, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, LLWU_P4, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP },
        #if PORTS_AVAILABLE > 1
    // PTB0..PTB31
    //
    { LLWU_P4, NO_WAKEUP, LLWU_P5, NO_WAKEUP, LLWU_P6, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP },
        #endif
        #if PORTS_AVAILABLE > 2
    // PTC0..PTC31
    //
    { NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP },
        #endif
        #if PORTS_AVAILABLE > 3
    // PTD0..PTD31
    //
    { NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP },
        #endif
        #if PORTS_AVAILABLE > 4
    // PTE0..PTE31
    //
    { NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP },
        #endif
        #if PORTS_AVAILABLE > 5
    // PTF0..PTF31
    //
    { NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP },
        #endif
    #elif defined KINETIS_KL
    // PTA0..PTA31
    //
    { NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP },
        #if PORTS_AVAILABLE > 1
    // PTB0..PTB31
    //
    { LLWU_P5, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP },
        #endif
        #if PORTS_AVAILABLE > 2
    // PTC0..PTC31
    //
    { NO_WAKEUP, LLWU_P6, NO_WAKEUP, LLWU_P7, LLWU_P8, LLWU_P9, LLWU_P10, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP },
        #endif
        #if PORTS_AVAILABLE > 3
    // PTD0..PTD31
    //
    { NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, LLWU_P14, NO_WAKEUP, LLWU_P15, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP },
        #endif
        #if PORTS_AVAILABLE > 4
    // PTE0..PTE31
    //
    { NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP },
        #endif
        #if PORTS_AVAILABLE > 5
    // PTF0..PTF31
    //
    { NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP },
        #endif
    #else
    // PTA0..PTA31
    //
    { NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, LLWU_P3, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, LLWU_P4, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP },
        #if PORTS_AVAILABLE > 1
    // PTB0..PTB31
    //
    { LLWU_P5, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP },
        #endif
        #if PORTS_AVAILABLE > 2
    // PTC0..PTC31
    //
    { NO_WAKEUP, LLWU_P6, NO_WAKEUP, LLWU_P7, LLWU_P8, LLWU_P9, LLWU_P10, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, LLWU_P11, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP },
        #endif
        #if PORTS_AVAILABLE > 3
    // PTD0..PTD31
    //
    { LLWU_P12, NO_WAKEUP, LLWU_P13, NO_WAKEUP, LLWU_P14, NO_WAKEUP, LLWU_P15, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP },
        #endif
        #if PORTS_AVAILABLE > 4
    // PTE0..PTE31
    //
    { NO_WAKEUP, LLWU_P0, LLWU_P1, NO_WAKEUP, LLWU_P2, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP },
        #endif
        #if PORTS_AVAILABLE > 5
    // PTF0..PTF31
    //
    { NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP, NO_WAKEUP },
        #endif
    #endif
};

#if !defined NO_WAKEUP_0_7
    #define WAKEUP_SOURCES_0_7     8
#else
    #define WAKEUP_SOURCES_0_7     0
#endif
#if !defined NO_WAKEUP_8_15 && (!defined KINETIS_KL03 && !defined KINETIS_KL05)
    #define WAKEUP_SOURCES_8_15    8
#else
    #define WAKEUP_SOURCES_8_15    0
#endif
#if !defined NO_WAKEUP_MODULE
    #define WAKEUP_SOURCES_MODULES 8
#else
    #define WAKEUP_SOURCES_MODULES 0
#endif

#define WAKEUP_SOURCES   (WAKEUP_SOURCES_0_7 + WAKEUP_SOURCES_8_15 + WAKEUP_SOURCES_MODULES)

static void (*wakeup_handlers[WAKEUP_SOURCES])(void) = {0};              // support a user wakeup handler of each wakeup pin and module source

static void fnHandleWakeupSources(volatile unsigned char *prtFlagRegister, int iSouceStart)
{
    register unsigned char ucBit;
    register unsigned char ucFlags = *prtFlagRegister;                   // check whether a source woke the processor
    if (ucFlags == 0) {
    #if defined KINETIS_KL25 && defined SUPPORT_RTC && !defined KINETIS_KL02 && !defined KINETIS_KE
        if ((iSouceStart != (WAKEUP_SOURCES_0_7 + WAKEUP_SOURCES_8_15)) || ((LLWU_ME_WUME7 & MODULE_RTC_SECONDS) == 0)) { // if not a module wakeup check and the RTC seconds source is not enabled as wakeup
            return;
        }
        // The RTC seconds interrupt doesn't have a status bit and so doesn't cause the source to be set in the wakeup module register
        //
        ucFlags = MODULE_RTC_SECONDS;                                    // set the source bit assuming that the wakeup was due to the seconds interrupt if no other module's flag is set
    #else
        return;
    #endif
    }
    ucBit = 0x01;
    while (ucFlags != 0) {                                               // while sources are flagged
        if ((ucFlags & ucBit) != 0) {
            ucFlags &= ~ucBit;
            if (iSouceStart >= (WAKEUP_SOURCES_0_7 + WAKEUP_SOURCES_8_15)) { // wakeup module interrupts
    #if !defined KINETIS_WITHOUT_RTC
                switch (1 << (iSouceStart - (WAKEUP_SOURCES_0_7 + WAKEUP_SOURCES_8_15))) { // wakeup module interrupts must be cleard at the source
                case MODULE_LPTMR0:
        #if defined TICK_USES_LPTMR
                    _RealTimeInterrupt();                                // call the TICK interrupt handler to clear the interrupt source
        #else
                    LPTMR0_CSR = LPTMR0_CSR;                             // clear pending interrupt
        #endif
                    break;
        #if defined SUPPORT_RTC && !defined KINETIS_KL02 && !defined KINETIS_KE
                case MODULE_RTC_ALARM:
            #if defined irq_RTC_SECONDS_ID && !defined SUPPORT_SW_RTC
                    _rtc_alarm_handler();                                // call the RTC alarm handler to clear the interrupt source
            #else
                    _rtc_handler();                                      // call the RTC seconds handler to clear the interrupt source
            #endif
                    break;
            #if defined irq_RTC_SECONDS_ID
                case MODULE_RTC_SECONDS:                                 // there is no wakeup source to clear in the case of the seconds interrupt
              //    _rtc_handler();                                      // call the RTC seconds handler to clear the interrupt source
                    break;
            #endif
        #endif
                default:
                    _EXCEPTION("Wakeup source clear to be added!!");
                    break;
                }
        #if defined _WINDOWS
                *prtFlagRegister &= ~ucBit;
        #endif
    #endif
            }
            else {
    #if defined _WINDOWS
                *prtFlagRegister &= ~ucBit;
    #else
                *prtFlagRegister = ucBit;                                // reset the interrupt flag (write '1' to clear)
    #endif
            }
            if (wakeup_handlers[iSouceStart] != 0) {                     // if there is a user handler for the source
                uDisable_Interrupt();                                    // ensure interrupts remain blocked when user callback operates
                    wakeup_handlers[iSouceStart]();
                uEnable_Interrupt();
            }
        }
        iSouceStart++;
        ucBit <<= 1;
    }
}

static __interrupt void _wakeup_isr(void)
{
    int iSource = 0;
    #if !defined NO_WAKEUP_0_7
    fnHandleWakeupSources(LLWU_FLAG_ADDRESS, iSource);                   // handle the LLWU_P0..P7 input source(s) that woke the processor
    iSource += 8;
    #endif
    #if !defined KINETIS_KL03 && !defined KINETIS_KL05
        #if !defined NO_WAKEUP_8_15
    fnHandleWakeupSources((LLWU_FLAG_ADDRESS + 1), iSource);             // handle the LLWU_P8..P15 input source(s) that woke the processor
    iSource += 8;
        #endif
    #endif
    #if !defined NO_WAKEUP_MODULE
        #if defined KINETIS_KL05
    fnHandleWakeupSources((LLWU_FLAG_ADDRESS + 1), iSource);             // handle the LLWU_M0..7 peripheral source(s) that woke the processor
        #elif !defined KINETIS_KL03
    fnHandleWakeupSources((LLWU_FLAG_ADDRESS + 2), iSource);             // handle the LLWU_M0..7 peripheral source(s) that woke the processor
        #endif
    #endif
}
#endif


#if defined _LLWU_CONFIG_CODE
        {
            INTERRUPT_SETUP *wakeup_interrupt = (INTERRUPT_SETUP *)ptrSettings;
            int iBitRef = 0;
            unsigned long ulBit = 0x00000001;
            unsigned long ulPortBits = wakeup_interrupt->int_port_bits;
            if (wakeup_interrupt->int_port == PORT_MODULE) {             // wakeup module rather than port pin
    #if !defined KINETIS_KL03                                            // KL03 has no module wakeup support
                // The wakeup interrupts are now mapped to modules
                //
                if (wakeup_interrupt->int_port_bits & ~(WAKEUP_MODULES)) {
                    _EXCEPTION("Invalid wakeup module being selected!");
                }
                LLWU_ME = (LLWU_ME & (unsigned char)(~(wakeup_interrupt->int_port_bits))); // disable the wakeup functionality
                LLWU_F3 = (unsigned char)(wakeup_interrupt->int_port_bits); // reset pending flags (the pending flags are cleared before enabling the interrupt source due to the fact that it may still be pending due to a wakeup from VLLSx, which entered via reset
        #if defined _WINDOWS
                LLWU_F3 = 0;
        #endif
                while (ulPortBits != 0) {                                // handle each module
                    if (wakeup_interrupt->int_port_bits & ulBit) {       // if the module wakeup is to be enabled
                        wakeup_handlers[iBitRef + (WAKEUP_SOURCES_0_7 + WAKEUP_SOURCES_8_15)] = wakeup_interrupt->int_handler; // enter the user interrupt handler for this wakeup input
                        ulPortBits &= ~ulBit;
                    }
                    ulBit <<= 1;
                    iBitRef++;
                }
                fnEnterInterrupt(irq_LL_wakeup_ID, wakeup_interrupt->int_priority, _wakeup_isr); // ensure that the handler is entered
                LLWU_ME = (LLWU_ME | (unsigned char)(wakeup_interrupt->int_port_bits));
    #endif
            }
            else {
                unsigned long ulCharacteristics = 0;
                unsigned char ucInterruptType;
                switch (wakeup_interrupt->int_port_sense & (IRQ_FALLING_EDGE | IRQ_RISING_EDGE)) {
                case IRQ_FALLING_EDGE:
                    ucInterruptType = LLWU_PE_WUPE_FALLING;
                    break;
                case IRQ_RISING_EDGE:
                    ucInterruptType = LLWU_PE_WUPE_RISING;
                    break;
                case (IRQ_FALLING_EDGE | IRQ_RISING_EDGE):
                    ucInterruptType = LLWU_PE_WUPE_CHANGE;
                    break;
                default:
                    ucInterruptType = LLWU_PE_WUPE_OFF;
                    break;
                }
                if (wakeup_interrupt->int_port_sense & PULLUP_ON) {
                    ulCharacteristics |= (PORT_PS_UP_ENABLE | PORT_PSEUDO_FLAG_SET_ONLY_PULLS);
                }
                else if (wakeup_interrupt->int_port_sense & PULLDOWN_ON) {
                    ulCharacteristics |= (PORT_PS_DOWN_ENABLE | PORT_PSEUDO_FLAG_SET_ONLY_PULLS);
                }
                if ((ulCharacteristics != 0) || (ENABLE_PORT_MODE & wakeup_interrupt->int_port_sense)) { // if a pull-up / down is specified or port enable desired
                    if (wakeup_interrupt->int_port >= PORTS_AVAILABLE) {
                        _EXCEPTION("Invalid port!!");
                        return;
                    }
                    POWER_UP(5, (SIM_SCGC5_PORTA << wakeup_interrupt->int_port)); // ensure that the port is powered
                    if ((ENABLE_PORT_MODE & wakeup_interrupt->int_port_sense) != 0) {
                        ulCharacteristics &= ~(PORT_PSEUDO_FLAG_SET_ONLY_PULLS);
                        ulCharacteristics |= (PORT_MUX_GPIO);
                    }
                    fnConnectGPIO(wakeup_interrupt->int_port, ulPortBits, ulCharacteristics); // configure the port pins with the specified characteristics
                }
                // The port inputs are now mapped to available LLWU pins (pins that do not have LLWU functionality will not be configured)
                //
                while (ulPortBits != 0) {                                // handle each bit on the port
                    if (wakeup_interrupt->int_port_bits & ulBit) {       // if the port bit is to be enabled
                        if (cWakeupPorts[wakeup_interrupt->int_port][iBitRef] != NO_WAKEUP) {
                            int iShift = ((cWakeupPorts[wakeup_interrupt->int_port][iBitRef]%4) * LLWU_PE_WUPE_SHIFT);
                            volatile unsigned char *ptrFlagRegister = (LLWU_FLAG_ADDRESS + (cWakeupPorts[wakeup_interrupt->int_port][iBitRef]/8));
                            unsigned char *ptrWakeupEnable = (unsigned char *)LLWU_BLOCK + (cWakeupPorts[wakeup_interrupt->int_port][iBitRef]/4); // set the enable register pointer
                            unsigned char ucValueMask = (LLWU_PE_WUPE_MASK << iShift); // set the mask in the enable register
                            *ptrWakeupEnable &= ~ucValueMask;            // disable the wakeup functionality
                            wakeup_handlers[cWakeupPorts[wakeup_interrupt->int_port][iBitRef]] = wakeup_interrupt->int_handler; // enter the user interrupt handler for this wakeup input
                            *ptrFlagRegister = (LLWU_F_WUF0 << (cWakeupPorts[wakeup_interrupt->int_port][iBitRef]%8)); // reset pending flags (the pending flag is cleared before enabling the interrupt source due to the fact that it may still be pending due to a wakeup from VLLSx, which entered via reset)
    #if defined _WINDOWS
                            *ptrFlagRegister = 0;
    #endif
                            fnEnterInterrupt(irq_LL_wakeup_ID, wakeup_interrupt->int_priority, _wakeup_isr); // ensure that the handler is entered
                            *ptrWakeupEnable |= (ucInterruptType << iShift); // set/enable the type required
                        }
                        else {
                            _EXCEPTION("Invalid wakeup port bit being selected!");
                        }
                        ulPortBits &= ~ulBit;
                    }
                    ulBit <<= 1;
                    iBitRef++;
                }
            }
        }
#endif
