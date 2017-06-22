/***********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher
    
    ---------------------------------------------------------------------
    File:      kinetis_low_power.h
    Project:   Single Chip Embedded Internet
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2017
    *********************************************************************
    22.07.2014 Add fnGetLowPowerMode() and fnSetLowPowerMode()           {1}
    02.07.2015 Always re-synchronise the RTC counters after waking from a low leakage state {2}
    26.07.2015 Respect that SMC_PMPROT is one-time write by setting maximum level once {3}
    03.01.2017 Display VLPR mode based purely on the settings and not the low power task {4}
    31.01.2017 Add LOW_POWER_CYCLING_MODE                                {5}
*/

#if defined LOW_POWER_CYCLING_MODE
    int iLowPowerLoopMode = 0;
#endif

// This routine switches to low power mode. It is always called with disabled interrupts
// - when the routine is called there were no system events pending so the system can be set to a low power mode until a masked interrupt becomes pending
//
extern void fnDoLowPower(void)
{
    #if !defined FLL_FACTOR && !defined RUN_FROM_EXTERNAL_CLOCK && !defined RUN_FROM_LIRC && !defined RUN_FROM_DEFAULT_CLOCK && !defined KINETIS_KE && !defined RUN_FROM_HIRC && !defined RUN_FROM_LIRC && !defined KINETIS_WITH_MCG_LITE
    register unsigned char ucMCG_C1;
    #endif
    register unsigned long ulDeepSleepMode = SYSTEM_CONTROL_REGISTER;    // present stop mode type
    if ((ulDeepSleepMode & SLEEPDEEP) != 0) {                            // the present mode is a stop based mode
        if (ulPeripheralNeedsClock != 0) {                               // don't enter stop mode if there is a peripheral that hasn't completed use of the clocks that will be stopped
            SYSTEM_CONTROL_REGISTER &= ~(SLEEPDEEP);                     // use wait mode until the transmission has completed
        }
        else {                                                           // there is no active transmission so we check that there is no reception starting and enable reception wakeup interrupts
    #if defined SERIAL_INTERFACE 
        #if UARTS_AVAILABLE > 0 && LPUARTS_AVAILABLE < 1
            if (IS_POWERED_UP(4, SIM_SCGC4_UART0)) {                     // if UART0 is enabled
                UART0_S2 |= UART_S2_RXEDGIF;                             // clear edge flag
                UART0_BDH |= UART_BDH_RXEDGIE;                           // enable wakeup on RxD falling edge
                if (UART0_S2 & UART_S2_RAF) {                            // if the receiver active flag is set it means that reception has already started so we don't enter stop mode
                    SYSTEM_CONTROL_REGISTER &= ~SLEEPDEEP;               // use wait mode until the reception has completed
                }
            }
        #elif LPUARTS_AVAILABLE > 0
            #if defined KINETIS_KL
            if (IS_POWERED_UP(5, SIM_SCGC5_LPUART0))                     // if LPUART0 is enabled
            #elif defined KINETIS_K80
            if (IS_POWERED_UP(2, SIM_SCGC2_LPUART0))                     // if LPUART0 is enabled
            #else
            if (IS_POWERED_UP(6, SIM_SCGC6_LPUART0))                     // if LPUART0 is enabled
            #endif
            {
                LPUART0_STAT |= LPUART_STAT_RXEDGIF;                     // clear edge flag
                LPUART0_BAUD |= LPUART_BAUD_RXEDGIE;                     // enable wakeup on RxD falling edge
                if (LPUART0_STAT & LPUART_STAT_RAF) {                    // if the receiver active flag is set it means that reception has already started so we don't enter stop mode
                    SYSTEM_CONTROL_REGISTER &= ~SLEEPDEEP;               // use wait mode until the reception has completed
                }
            }
        #endif
        #if UARTS_AVAILABLE > 1 && LPUARTS_AVAILABLE < 2
            if (IS_POWERED_UP(4, SIM_SCGC4_UART1)) {                     // if UART1 is enabled
                UART1_S2 |= UART_S2_RXEDGIF;                             // clear edge flag
                UART1_BDH |= UART_BDH_RXEDGIE;                           // enable wakeup on RxD falling edge
                if (UART1_S2 & UART_S2_RAF) {                            // if the receiver active flag is set it means that reception has already started so we don't enter stop mode
                    SYSTEM_CONTROL_REGISTER &= ~SLEEPDEEP;               // use wait mode until the reception has completed
                }
            }
        #elif LPUARTS_AVAILABLE > 1
            #if defined KINETIS_KL
            if (IS_POWERED_UP(5, SIM_SCGC5_LPUART1))                     // if LPUART1 is enabled
            #else
            if (IS_POWERED_UP(2, SIM_SCGC2_LPUART1))                     // if LPUART1 is enabled
            #endif
            {
                LPUART1_STAT |= LPUART_STAT_RXEDGIF;                     // clear edge flag
                LPUART1_BAUD |= LPUART_BAUD_RXEDGIE;                     // enable wakeup on RxD falling edge
                if (LPUART1_STAT & LPUART_STAT_RAF) {                    // if the receiver active flag is set it means that reception has already started so we don't enter stop mode
                    SYSTEM_CONTROL_REGISTER &= ~SLEEPDEEP;               // use wait mode until the reception has completed
                }
            }
        #endif
        #if (UARTS_AVAILABLE > 2 && LPUARTS_AVAILABLE < 3) || (UARTS_AVAILABLE == 1 && LPUARTS_AVAILABLE == 2)
            if (IS_POWERED_UP(4, SIM_SCGC4_UART2)) {                     // if UART2 is enabled
                UART2_S2 |= UART_S2_RXEDGIF;                             // clear edge flag
                UART2_BDH |= UART_BDH_RXEDGIE;                           // enable wakeup on RxD falling edge
                if (UART2_S2 & UART_S2_RAF) {                            // if the receiver active flag is set it means that reception has already started so we don't enter stop mode
                    SYSTEM_CONTROL_REGISTER &= ~SLEEPDEEP;               // use wait mode until the reception has completed
                }
            }
        #endif
        #if UARTS_AVAILABLE > 3
            if (IS_POWERED_UP(4, SIM_SCGC4_UART3)) {                     // if UART3 is enabled
                UART3_S2 |= UART_S2_RXEDGIF;                             // clear edge flag
                UART3_BDH |= UART_BDH_RXEDGIE;                           // enable wakeup on RxD falling edge
                if (UART3_S2 & UART_S2_RAF) {                            // if the receiver active flag is set it means that reception has already started so we don't enter stop mode
                    SYSTEM_CONTROL_REGISTER &= ~SLEEPDEEP;               // use wait mode until the reception has completed
                }
            }
        #endif
        #if UARTS_AVAILABLE > 4
            if (IS_POWERED_UP(1, SIM_SCGC1_UART4)) {                     // if UART4 is enabled
                UART4_S2 |= UART_S2_RXEDGIF;                             // clear edge flag
                UART4_BDH |= UART_BDH_RXEDGIE;                           // enable wakeup on RxD falling edge
                if (UART4_S2 & UART_S2_RAF) {                            // if the receiver active flag is set it means that reception has already started so we don't enter stop mode
                    SYSTEM_CONTROL_REGISTER &= ~SLEEPDEEP;               // use wait mode until the reception has completed
                }
            }
        #endif
        #if UARTS_AVAILABLE > 5
            if (IS_POWERED_UP(1, SIM_SCGC1_UART5)) {                     // if UART5 is enabled
                UART5_S2 |= UART_S2_RXEDGIF;                             // clear edge flag
                UART5_BDH |= UART_BDH_RXEDGIE;                           // enable wakeup on RxD falling edge
                if (UART5_S2 & UART_S2_RAF) {                            // if the receiver active flag is set it means that reception has already started so we don't enter stop mode
                    SYSTEM_CONTROL_REGISTER &= ~SLEEPDEEP;               // use wait mode until the reception has completed
                }
            }
        #endif
    #endif
        }
    }
    #if defined LOW_POWER_CYCLING_MODE                                   // {5}
    if (iLowPowerLoopMode == LOW_POWER_CYCLING_PAUSED) {                 // if the low power cycling had been paused we enable it before sleeping
        iLowPowerLoopMode = LOW_POWER_CYCLING_ENABLED;
    }
    do {
    #endif
    #if !defined FLL_FACTOR && !defined RUN_FROM_EXTERNAL_CLOCK && !defined RUN_FROM_LIRC && !defined RUN_FROM_DEFAULT_CLOCK && !defined KINETIS_KE && !defined RUN_FROM_HIRC && !defined RUN_FROM_LIRC && !defined KINETIS_WITH_MCG_LITE
    ucMCG_C1 = MCG_C1;                                                   // backup the original MCG_C1 setting
    #endif
    #if defined KINETIS_KL && defined SUPPORT_RTC && defined RTC_USES_LPO_1kHz
    if (fnPresentLP_mode() >= LLS_MODE) {                                // if we are going to a low leakage mode that will not respond to RTC interrupts therefore adjust the RTC alarm value to compensate for this fact
        fnSlowRTC();                                                     // adjust RTC settings to handle slow clock during low leakage mode
    }
    #endif
    #if !defined _WINDOWS
  //TOGGLE_TEST_OUTPUT();
    __sleep_mode();                                                      // enter low power mode using wait for interrupt processor state
  //TOGGLE_TEST_OUTPUT();
    #endif
    // The processor will continue after being woken by any pending interrupt (also when the global interrupt mask is still set)
    // - this mean that the processor has woken again when the code execution reaches this location
    //
    #if !defined FLL_FACTOR && !defined RUN_FROM_EXTERNAL_CLOCK && !defined RUN_FROM_LIRC && !defined RUN_FROM_DEFAULT_CLOCK && !defined KINETIS_KE && !defined RUN_FROM_HIRC && !defined RUN_FROM_LIRC && !defined KINETIS_WITH_MCG_LITE
        #if defined KINETIS_K_FPU || defined KINETIS_KL || defined KINETIS_REVISION_2 || (KINETIS_MAX_SPEED > 100000000)
    if ((SYSTEM_CONTROL_REGISTER & SLEEPDEEP) && (SMC_PMPROT & (SMC_PMPROT_AVLP | SMC_PMPROT_ALLS)))
        #else
    if ((SYSTEM_CONTROL_REGISTER & SLEEPDEEP) && (MC_PMPROT & MC_PMPROT_AVLP))
        #endif
    {                                                                    // LLS/VLPS sleep mode exit requires the PLL to be reconfigured
        MCG_C5 = ((CLOCK_DIV - 1) | MCG_C5_PLLSTEN0);                    // move from state FEE to state PBE (or FBE) PLL remains enabled in normal stop modes
        MCG_C6 = ((CLOCK_MUL - MCG_C6_VDIV0_LOWEST) | MCG_C6_PLLS);
        while ((MCG_S & MCG_S_PLLST) == 0) {                             // loop until the PLLS clock source becomes valid
        }
        while ((MCG_S & MCG_S_LOCK) == 0) {                              // loop until PLL locks
        }
        MCG_C1 = ucMCG_C1;                                               // finally move from PBE to PEE mode - switch to PLL clock (the original settings are returned)
        while ((MCG_S & MCG_S_CLKST_MASK) != MCG_S_CLKST_PLL) {          // loop until the PLL clock is selected
        }
    }
    #endif
    #if defined KINETIS_KL && defined SUPPORT_RTC
    if (fnPresentLP_mode() >= LOW_LEAKAGE_MODES) {                       // if we are waking from a low leakage mode we need to resynchronise the RTC due to the fact that the seconds interrupt was not serviced in the meantime
        #if defined RTC_USES_LPO_1kHz
        fnRestoreRTC(1);                                                 // convert RTC setting back to 1s interrupt rate and adjust for any lost time
        #else                                                            // external clock assumed that was clocking the RTC at normal 32kHz during low leakage mode
        fnConvertSecondsTime(0, RTC_TSR);                                // {2} take the present RTC seconds count value, convert and set to time and date
        #endif
    }
    #endif
    #if defined LOW_POWER_CYCLING_MODE                                   // {5}
    } while ((iLowPowerLoopMode >= LOW_POWER_CYCLING_ENABLED) && (fnVirtualWakeupInterruptHandler((ulDeepSleepMode & SLEEPDEEP) != 0))); // allow the user to optionally quickly handle wakeup event without quitting the sleep loop
    #endif
    SYSTEM_CONTROL_REGISTER = ulDeepSleepMode;                           // ensure present stop mode has been returned (we may have used wait mode instead due to present peripheral activity)
    #if defined SERIAL_INTERFACE
        #if UARTS_AVAILABLE > 0 && LPUARTS_AVAILABLE < 1
    if (IS_POWERED_UP(4, SIM_SCGC4_UART0)) {                             // if UART0 is enabled
        UART0_BDH &= ~(UART_BDH_RXEDGIE);                                // disable edge interrupt on RxD since we never want to handle the actual interrupt (used just for waking)
    }
        #elif LPUARTS_AVAILABLE > 0
            #if defined KINETIS_KL
    if (IS_POWERED_UP(5, SIM_SCGC5_LPUART0))                             // if LPUART0 is enabled
        #elif defined KINETIS_K80
    if (IS_POWERED_UP(2, SIM_SCGC2_LPUART0))                             // if LPUART0 is enabled
        #else
    if (IS_POWERED_UP(6, SIM_SCGC6_LPUART0))                             // if LPUART0 is enabled
            #endif
    {
        LPUART0_BAUD &= ~LPUART_BAUD_RXEDGIE;                            // disable edge interrupt on RxD since we never want to handle the actual interrupt (used just for waking)
    }
        #endif
        #if UARTS_AVAILABLE > 1 && LPUARTS_AVAILABLE < 2
    if (IS_POWERED_UP(4, SIM_SCGC4_UART1)) {                             // if UART1 is enabled
        UART1_BDH &= ~(UART_BDH_RXEDGIE);                                // disable edge interrupt on RxD since we never want to handle the actual interrupt (used just for waking)
    }
        #elif LPUARTS_AVAILABLE > 1
            #if defined KINETIS_KL
    if (IS_POWERED_UP(5, SIM_SCGC5_LPUART1))                             // if LPUART1 is enabled
            #else
    if (IS_POWERED_UP(2, SIM_SCGC2_LPUART1))                             // if LPUART1 is enabled
            #endif
    {
        LPUART1_BAUD &= ~LPUART_BAUD_RXEDGIE;                            // disable edge interrupt on RxD since we never want to handle the actual interrupt (used just for waking)
    }
        #endif
        #if (UARTS_AVAILABLE > 2 && LPUARTS_AVAILABLE < 3) || (UARTS_AVAILABLE == 1 && LPUARTS_AVAILABLE == 2)
    if (IS_POWERED_UP(4, SIM_SCGC4_UART2)) {                             // if UART2 is enabled
        UART2_BDH &= ~(UART_BDH_RXEDGIE);                                // disable edge interrupt on RxD since we never want to handle the actual interrupt (used just for waking)
    }
        #endif
        #if UARTS_AVAILABLE > 3
    if (IS_POWERED_UP(4, SIM_SCGC4_UART3)) {                             // if UART3 is enabled
        UART3_BDH &= ~(UART_BDH_RXEDGIE);                                // disable edge interrupt on RxD since we never want to handle the actual interrupt (used just for waking)
    }
        #endif
        #if UARTS_AVAILABLE > 4
    if (IS_POWERED_UP(1, SIM_SCGC1_UART4)) {                             // if UART4 is enabled
        UART4_BDH &= ~(UART_BDH_RXEDGIE);                                // disable edge interrupt on RxD since we never want to handle the actual interrupt (used just for waking)
    }
        #endif
        #if UARTS_AVAILABLE > 5
    if (IS_POWERED_UP(1, SIM_SCGC1_UART5)) {                             // if UART5 is enabled
        UART5_BDH &= ~(UART_BDH_RXEDGIE);                                // disable edge interrupt on RxD since we never want to handle the actual interrupt (used just for waking)
    }
        #endif
    #endif
  //TOGGLE_TEST_OUTPUT();
    uEnable_Interrupt();                                                 // enable interrupts so that the masked interrupt that became pending can be taken
  //TOGGLE_TEST_OUTPUT();
}

static int fnPresentLP_mode(void)
{
    #if defined KINETIS_KE                                               // KE devices
    if (SYSTEM_CONTROL_REGISTER & SLEEPDEEP) {                           // if the sleep deep flag is set the stop mode is active
        return STOP_MODE;
    }
    #else
        #if defined KINETIS_K_FPU || defined KINETIS_KL || defined KINETIS_REVISION_2 || (KINETIS_MAX_SPEED > 100000000)
    if (((SMC_PMPROT & SMC_PMPROT_AVLP) != 0) && ((SMC_PMCTRL & SMC_PMCTRL_RUNM_VLPR) != 0)) {
        return VLPR_MODE;
    }
    else if (SYSTEM_CONTROL_REGISTER & SLEEPDEEP) {
        if ((SMC_PMCTRL & (SMC_PMCTRL_RUNM_VLPR | SMC_PMCTRL_STOPM_VLLSx | SMC_PMCTRL_STOPM_LLS | SMC_PMCTRL_STOPM_VLPS)) == 0) {
            return STOP_MODE;
        }
        else if (((SMC_PMPROT & SMC_PMPROT_AVLP) != 0) && ((SMC_PMCTRL & (SMC_PMCTRL_STOPM_VLLSx | SMC_PMCTRL_STOPM_LLS)) == SMC_PMCTRL_STOPM_VLPS)) {
            return VLPS_MODE;
        }
        #if defined LLS_MODE
        else if (((SMC_PMPROT & SMC_PMPROT_ALLS) != 0) && ((SMC_PMCTRL & (SMC_PMCTRL_STOPM_VLLSx | SMC_PMCTRL_STOPM_LLS)) == SMC_PMCTRL_STOPM_LLS)) {
            return LLS_MODE;
        }
        #endif
        #if defined LLS2_MODE
        else if (((SMC_PMPROT & SMC_PMPROT_ALLS) != 0) && ((SMC_PMCTRL & (SMC_PMCTRL_STOPM_VLLSx | SMC_PMCTRL_STOPM_LLS)) == SMC_PMCTRL_STOPM_LLS)) {
            return LLS2_MODE;
        }
        #endif
        #if defined LLS3_MODE
        else if (((SMC_PMPROT & SMC_PMPROT_ALLS) != 0) && ((SMC_PMCTRL & (SMC_PMCTRL_STOPM_VLLSx | SMC_PMCTRL_STOPM_LLS)) == SMC_PMCTRL_STOPM_LLS)) {
            return LLS3_MODE;
        }
        #endif
        else if (((SMC_PMPROT & SMC_PMPROT_AVLLS) != 0) && ((SMC_PMCTRL & (SMC_PMCTRL_STOPM_VLLSx | SMC_PMCTRL_STOPM_LLS)) == SMC_PMCTRL_STOPM_VLLSx)) {
            #if defined KINETIS_KL || defined KINETIS_K22                // KL devices
            switch (SMC_STOPCTRL & SMC_STOPCTRL_VLLSM_VLLS3) {
            case SMC_STOPCTRL_VLLSM_VLLS0:
                return VLLS0_MODE;
            case SMC_STOPCTRL_VLLSM_VLLS1:
                return VLLS1_MODE;
                #if defined KINETIS_K22
            case SMC_STOPCTRL_VLLSM_VLLS2:
                return VLLS2_MODE;
                #endif
            case SMC_STOPCTRL_VLLSM_VLLS3:
                return VLLS3_MODE;
            }
            #else
            switch (SMC_VLLSCTRL & SMC_VLLSCTRL_VLLSM_VLLS3) {           // K devices
            case SMC_VLLSCTRL_VLLSM_VLLS0:
                return VLLS0_MODE;
            case SMC_VLLSCTRL_VLLSM_VLLS1:
                return VLLS1_MODE;
            case SMC_VLLSCTRL_VLLSM_VLLS2:
                return VLLS2_MODE;
            case SMC_VLLSCTRL_VLLSM_VLLS3:
                return VLLS3_MODE;
            }
            #endif
        }
    }
        #else
    if (((MC_PMPROT & MC_PMPROT_AVLP) != 0) && ((MC_PMCTRL & MC_PMCTRL_RUNM_VLP_RUN) != 0)) {
        return VLPR_MODE;
    }
    else if ((SYSTEM_CONTROL_REGISTER & SLEEPDEEP) != 0) {
        if ((MC_PMCTRL & (MC_PMCTRL_RUNM_VLP_RUN | MC_PMCTRL_LPLLSM_VLLS1 | MC_PMCTRL_LPLLSM_VLLS2 | MC_PMCTRL_LPLLSM_VLLS3 | MC_PMCTRL_LPLLSM_LLS | MC_PMCTRL_LPLLSM_VLPS)) == 0) {
            return STOP_MODE;
        }
        else if (((MC_PMPROT & MC_PMPROT_AVLP) != 0) && ((MC_PMCTRL & MC_PMCTRL_LPLLSM_VLPS) != 0)) {
            return VLPS_MODE;
        }
        else if (((MC_PMPROT & MC_PMPROT_ALLS) != 0) && ((MC_PMCTRL & MC_PMCTRL_LPLLSM_LLS) != 0)) {
            return LLS_MODE;
        }
        else if ((MC_PMPROT & (MC_PMPROT_AVLLS1 | MC_PMPROT_AVLLS2 | MC_PMPROT_AVLLS3)) != 0) {
            switch (MC_PMCTRL & MC_PMCTRL_LPLLSM_VLLS1) {
            case MC_PMCTRL_LPLLSM_VLLS1:
                return VLLS1_MODE;
            case MC_PMCTRL_LPLLSM_VLLS2:
                return VLLS2_MODE;
            case MC_PMCTRL_LPLLSM_VLLS3:
                return VLLS3_MODE;
            }
        }
    }
        #endif
    #endif
    return WAIT_MODE;
}

// Determine the present low power mode that is presently configured
//
extern int fnGetLowPowerMode(void)                                       // {1}
{
    // Determine the present low power mode of operation from the SMC settings
    //
#if defined KINETIS_KE
    if ((uGetTaskState(TASK_LOW_POWER) & (UTASKER_GO | UTASKER_POLLING)) == 0) {
        return RUN_MODE;                                                 // low power task not operating
    }
    return (fnPresentLP_mode());
#else
    int iLPmode = fnPresentLP_mode();                                    // {4}
    if (iLPmode == VLPR_MODE) {                                          // VLPR mode is an exception since it doesn't use the processor's stop mode
    #if defined VLPW_MODE
        if ((uGetTaskState(TASK_LOW_POWER) & (UTASKER_GO | UTASKER_POLLING)) == 0) {
            return VLPR_MODE;                                            // low power task not operating
        }
        return VLPW_MODE;                                                // low power task is operating so that VLPW is ued
    #else
        return VLPR_MODE;
    #endif
    }
    if ((uGetTaskState(TASK_LOW_POWER) & (UTASKER_GO | UTASKER_POLLING)) == 0) {
        return RUN_MODE;                                                 // low power task not operating
    }
    return iLPmode;
#endif
}


#if defined KINETIS_KE
    #define fnSetStopMode()    SYSTEM_CONTROL_REGISTER |= SLEEPDEEP
#elif defined KINETIS_K_FPU || defined KINETIS_KL || defined KINETIS_REVISION_2 || (KINETIS_MAX_SPEED > 100000000)
    #define fnSetStopMode()    SMC_PMCTRL = (SMC_PMCTRL_STOPM_NORMAL | SMC_PMCTRL_RUNM_NORMAL); SYSTEM_CONTROL_REGISTER |= SLEEPDEEP
#else
    #define fnSetStopMode()    MC_PMCTRL = (MC_PMCTRL_LPLLSM_NORMAL_STOP | MC_PMCTRL_RUNM_NORMAL_RUN); SYSTEM_CONTROL_REGISTER |= SLEEPDEEP
#endif


// Set a new low power mode
// - note that some modes will not be able to fully operate or changes between certain modes may not be possible
//
extern void fnSetLowPowerMode(int new_lp_mode)                           // {1}
{
    int next_lp_mode = (new_lp_mode & ~(LOW_POWER_OPTIONS));
    switch (next_lp_mode) {                                              // the new mode to be used
    case RUN_MODE:                                                       // no low power operation
        uTaskerStateChange(TASK_LOW_POWER, UTASKER_STOP);                // stop low power task from operating
        // Fall through intentionally
        //
    case WAIT_MODE:                                                      // wait mode
    #if !defined KINETIS_KE
        #if defined KINETIS_K_FPU || defined KINETIS_KL || defined KINETIS_REVISION_2 || (KINETIS_MAX_SPEED > 100000000)
        SMC_PMCTRL = (SMC_PMCTRL_STOPM_NORMAL | SMC_PMCTRL_RUNM_NORMAL);
        #else
        MC_PMCTRL = (MC_PMCTRL_LPLLSM_NORMAL_STOP | MC_PMCTRL_RUNM_NORMAL_RUN);
        #endif
    #endif
        SYSTEM_CONTROL_REGISTER &= ~(SLEEPDEEP);
        if (RUN_MODE == next_lp_mode) {
            return;
        }
        break;
    case STOP_MODE:                                                      // stop mode
        #if defined KINETIS_KL03
        SMC_STOPCTRL = (SMC_STOPCTRL & ~(SMC_STOPCTRL_PSTOPO_PSTOP1 | SMC_STOPCTRL_PSTOPO_PSTOP2)); // ensure normal stop mode
        #endif
        fnSetStopMode();
        break;
    #if defined KINETIS_KL03
    case PSTOP1_MODE:                                                    // partical stop with both system and bus clocks stopped
        SMC_STOPCTRL = ((SMC_STOPCTRL & ~(SMC_STOPCTRL_PSTOPO_PSTOP1 | SMC_STOPCTRL_PSTOPO_PSTOP2)) | SMC_STOPCTRL_PSTOPO_PSTOP1);
        fnSetStopMode();
        break;
    case PSTOP2_MODE:                                                    // partical stop with system clock disabled and bus clock enabled
        SMC_STOPCTRL = ((SMC_STOPCTRL & ~(SMC_STOPCTRL_PSTOPO_PSTOP1 | SMC_STOPCTRL_PSTOPO_PSTOP2)) | SMC_STOPCTRL_PSTOPO_PSTOP2);
        fnSetStopMode();
        break;
    #endif
    #if !defined KINETIS_KE
        #if defined VLPW_MODE
    case VLPW_MODE:                                                      // VLPW
        #endif
    case VLPR_MODE:                                                      // VLPR
        #if defined KINETIS_K_FPU || defined KINETIS_KL || defined KINETIS_REVISION_2 || (KINETIS_MAX_SPEED > 100000000)
      //SMC_PMPROT |= SMC_PMPROT_AVLP;                                   // {3} - set once in kinetis.c
        SMC_PMCTRL |= (SMC_PMCTRL_RUNM_VLPR | SMC_PMCTRL_LPWUI);         // VLPR is entered immediately and VLPW results if the sleep instruction is later executed
        #else
      //MC_PMPROT |= MC_PMPROT_AVLP;                                     // {3} - set once in kinetis.c
        MC_PMCTRL |= (MC_PMCTRL_RUNM_VLP_RUN | MC_PMCTRL_LPWUI);
        #endif
        SYSTEM_CONTROL_REGISTER &= ~(SLEEPDEEP);
        if (VLPR_MODE == next_lp_mode) {
            uTaskerStateChange(TASK_LOW_POWER, UTASKER_STOP);            // stop low power task from operating
            return;
        }
        break;
    case VLPS_MODE:                                                      // VLPS
        #if defined KINETIS_K_FPU || defined KINETIS_KL || defined KINETIS_REVISION_2 || (KINETIS_MAX_SPEED > 100000000)
      //SMC_PMPROT |= SMC_PMPROT_AVLP;                                   // {3} - set once in kinetis.c
        SMC_PMCTRL = (SMC_PMCTRL_RUNM_NORMAL | SMC_PMCTRL_STOPM_VLPS | SMC_PMCTRL_LPWUI);
        #else
      //MC_PMPROT |= MC_PMPROT_AVLP;                                     // {3} - set once in kinetis.c
        MC_PMCTRL |= (MC_PMCTRL_RUNM_NORMAL_RUN | MC_PMCTRL_LPLLSM_VLPS | MC_PMCTRL_LPWUI);
        #endif
        SYSTEM_CONTROL_REGISTER |= SLEEPDEEP;
        break;
        #if defined LLS_MODE
    case LLS_MODE:                                                       // LLS
        #if defined KINETIS_K_FPU || defined KINETIS_KL || defined KINETIS_REVISION_2 || (KINETIS_MAX_SPEED > 100000000)
      //SMC_PMPROT |= SMC_PMPROT_ALLS;                                   // {3} - set once in kinetis.c
        SMC_PMCTRL = (SMC_PMCTRL_RUNM_NORMAL | SMC_PMCTRL_STOPM_LLS);
        #else
      //MC_PMPROT |= MC_PMPROT_ALLS;                                     // {3} - set once in kinetis.c
        MC_PMCTRL |= (MC_PMCTRL_RUNM_NORMAL_RUN | MC_PMCTRL_LPLLSM_LLS);
        #endif
        SYSTEM_CONTROL_REGISTER |= SLEEPDEEP;
        break;
        #endif
        #if defined LLS3_MODE
    case LLS3_MODE:                                                      // LLS3
        #if defined KINETIS_K_FPU || defined KINETIS_KL || defined KINETIS_REVISION_2 || (KINETIS_MAX_SPEED > 100000000)
        SMC_STOPCTRL = (unsigned char)(SMC_STOPCTRL_VLLSM_VLLS3 | (new_lp_mode & LOW_POWER_OPTIONS));
        SMC_PMCTRL = (SMC_PMCTRL_RUNM_NORMAL | SMC_PMCTRL_STOPM_LLS);
        #else
        MC_PMCTRL |= (MC_PMCTRL_RUNM_NORMAL_RUN | MC_PMCTRL_LPLLSM_LLS);
        #endif
        SYSTEM_CONTROL_REGISTER |= SLEEPDEEP;
        break;
        #endif
        #if defined LLS2_MODE
    case LLS2_MODE:                                                      // LLS2
        #if defined KINETIS_K_FPU || defined KINETIS_KL || defined KINETIS_REVISION_2 || (KINETIS_MAX_SPEED > 100000000)
        SMC_STOPCTRL = (unsigned char)(SMC_STOPCTRL_VLLSM_VLLS2 | (new_lp_mode & LOW_POWER_OPTIONS));
        SMC_PMCTRL = (SMC_PMCTRL_RUNM_NORMAL | SMC_PMCTRL_STOPM_LLS);
        #else
        MC_PMCTRL |= (MC_PMCTRL_RUNM_NORMAL_RUN | MC_PMCTRL_LPLLSM_LLS);
        #endif
        SYSTEM_CONTROL_REGISTER |= SLEEPDEEP;
        break;
        #endif
    case VLLS0_MODE:                                                     // VLLS0
        #if defined KINETIS_K_FPU || defined KINETIS_KL || defined KINETIS_REVISION_2 || (KINETIS_MAX_SPEED > 100000000)
      //SMC_PMPROT |= SMC_PMPROT_AVLLS;                                  // {3} - set once in kinetis.c
            #if defined KINETIS_KL || defined KINETIS_K22
        SMC_STOPCTRL = (unsigned char)(SMC_STOPCTRL_VLLSM_VLLS0 | (new_lp_mode & LOW_POWER_OPTIONS));
            #else
        SMC_VLLSCTRL = (unsigned char)(SMC_VLLSCTRL_VLLSM_VLLS0 | (new_lp_mode & LOW_POWER_OPTIONS));
            #endif
        SMC_PMCTRL = (SMC_PMCTRL_RUNM_NORMAL | SMC_PMCTRL_STOPM_VLLSx);
        #else
      //MC_PMPROT |= MC_PMPROT_ALLS;                                     // {3} - set once in kinetis.c
        MC_PMCTRL |= (MC_PMCTRL_RUNM_NORMAL_RUN | MC_PMPROT_AVLLS1);
        #endif
        SYSTEM_CONTROL_REGISTER |= SLEEPDEEP;
        break;
    case VLLS1_MODE:                                                     // VLLS1
        #if defined KINETIS_K_FPU || defined KINETIS_KL || defined KINETIS_REVISION_2 || (KINETIS_MAX_SPEED > 100000000)
      //SMC_PMPROT |= SMC_PMPROT_AVLLS;                                  // {3} - set once in kinetis.c
            #if defined KINETIS_KL || defined KINETIS_K22
        SMC_STOPCTRL = (unsigned char)(SMC_STOPCTRL_VLLSM_VLLS1 | (new_lp_mode & LOW_POWER_OPTIONS));
            #else
        SMC_VLLSCTRL = (unsigned char)(SMC_VLLSCTRL_VLLSM_VLLS1 | (new_lp_mode & LOW_POWER_OPTIONS));
            #endif
        SMC_PMCTRL = (SMC_PMCTRL_RUNM_NORMAL | SMC_PMCTRL_STOPM_VLLSx);
        #else
      //MC_PMPROT |= MC_PMPROT_ALLS;                                     // {3} - set once in kinetis.c
        MC_PMCTRL |= (MC_PMCTRL_RUNM_NORMAL_RUN | MC_PMPROT_AVLLS1);
        #endif
        SYSTEM_CONTROL_REGISTER |= SLEEPDEEP;
        break;
        #if !defined KINETIS_KL
    case VLLS2_MODE:                                                     // VLLS2
        #if defined KINETIS_K_FPU || defined KINETIS_KL || defined KINETIS_REVISION_2 || (KINETIS_MAX_SPEED > 100000000)
      //SMC_PMPROT |= SMC_PMPROT_AVLLS;                                   // {3} - set once in kinetis.c
            #if defined KINETIS_KL || defined KINETIS_K22
        SMC_STOPCTRL = (unsigned char)(SMC_STOPCTRL_VLLSM_VLLS2 | (new_lp_mode & LOW_POWER_OPTIONS));
            #else
        SMC_VLLSCTRL = (unsigned char)(SMC_VLLSCTRL_VLLSM_VLLS2 | (new_lp_mode & LOW_POWER_OPTIONS));
            #endif
        SMC_PMCTRL = (SMC_PMCTRL_RUNM_NORMAL | SMC_PMCTRL_STOPM_VLLSx);
        #else
      //MC_PMPROT |= MC_PMPROT_ALLS;                                     // {3} - set once in kinetis.c
        MC_PMCTRL |= (MC_PMCTRL_RUNM_NORMAL_RUN | MC_PMPROT_AVLLS2);
        #endif
        SYSTEM_CONTROL_REGISTER |= SLEEPDEEP;
        break;
        #endif
    case VLLS3_MODE:                                                     // VLLS3
        #if defined KINETIS_K_FPU || defined KINETIS_KL || defined KINETIS_REVISION_2 || (KINETIS_MAX_SPEED > 100000000)
      //SMC_PMPROT |= SMC_PMPROT_AVLLS;                                  // {3} - set once in kinetis.c
            #if defined KINETIS_KL || defined KINETIS_K22
        SMC_STOPCTRL = (unsigned char)(SMC_STOPCTRL_VLLSM_VLLS3 | (new_lp_mode & LOW_POWER_OPTIONS));
            #else
        SMC_VLLSCTRL = (unsigned char)(SMC_VLLSCTRL_VLLSM_VLLS3 | (new_lp_mode & LOW_POWER_OPTIONS));
            #endif
        SMC_PMCTRL = (SMC_PMCTRL_RUNM_NORMAL | SMC_PMCTRL_STOPM_VLLSx);
        #else
      //MC_PMPROT |= MC_PMPROT_ALLS;                                     // {3} - set once in kinetis.c
        MC_PMCTRL |= (MC_PMCTRL_RUNM_NORMAL_RUN | MC_PMPROT_AVLLS3);
        #endif
        SYSTEM_CONTROL_REGISTER |= SLEEPDEEP;
        break;
    #endif
    }
    uTaskerStateChange(TASK_LOW_POWER, UTASKER_GO);                      // ensure low power task is operating
}
