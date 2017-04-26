/***********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher
    
    ---------------------------------------------------------------------
    File:      kinetis_FLEXTIMER.h
    Project:   Single Chip Embedded Internet
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2017
    *********************************************************************
    21.06.2014 Adjust FlexTimer/TPM use of FTM_SC_TOF to clear interrupt correctly {1}
    22.07.2014 Add clock source selection to TPM                         {2}
    04.01.2017 Don't adjust the RC clock setting when the processor is running from it {3}
    26.01.2017 Add external clock selection for KL parts                 {4}
    26.04.2017 Add KL82 TPM clock input selection                        {5}

*/

#if defined _FLEXTIMER_CODE

/* =================================================================== */
/*                 local function prototype declarations               */
/* =================================================================== */

static __interrupt void _flexTimerInterrupt_0(void);
    #if FLEX_TIMERS_AVAILABLE > 1
static __interrupt void _flexTimerInterrupt_1(void);
    #endif
    #if FLEX_TIMERS_AVAILABLE > 2
static __interrupt void _flexTimerInterrupt_2(void);
    #endif
    #if FLEX_TIMERS_AVAILABLE > 3
static __interrupt void _flexTimerInterrupt_3(void);
    #endif

/* =================================================================== */
/*                      local variable definitions                     */
/* =================================================================== */

static unsigned short usFlexTimerMode[FLEX_TIMERS_AVAILABLE] = {0};      // operating mode details of each FlexTimer
static void (*_flexTimerHandler[FLEX_TIMERS_AVAILABLE])(void) = {0};     // user interrupt handlers

static void (*_flexTimerInterrupt[FLEX_TIMERS_AVAILABLE])(void) = {
    _flexTimerInterrupt_0,
    #if FLEX_TIMERS_AVAILABLE > 1
    _flexTimerInterrupt_1,
    #endif
    #if FLEX_TIMERS_AVAILABLE > 2
    _flexTimerInterrupt_2,
    #endif
    #if FLEX_TIMERS_AVAILABLE > 3
    _flexTimerInterrupt_3
    #endif
};


/* =================================================================== */
/*                  FlexTimer Interrupt Handlers                       */
/* =================================================================== */

// Generic interrupt handling
//
static void fnHandleFlexTimer(FLEX_TIMER_MODULE *ptrFlexTimer, int iFlexTimerReference)
{
    if ((ptrFlexTimer->FTM_SC & FTM_SC_TOF) != 0) {                      // flag will always be set but it has to be read at '1' before it can be reset
        if (usFlexTimerMode[iFlexTimerReference] & FLEX_TIMER_PERIODIC) {// if the timer is being used in periodic mode
            ptrFlexTimer->FTM_SC = (usFlexTimerMode[iFlexTimerReference] & FTM_SC_USED_MASK); // reset interrupt and allow the FlexTimer to continue running for periodic interrupts
        }
        else {
            ptrFlexTimer->FTM_SC = FTM_SC_TOF;                           // stop further activity (single-shot mode)
            switch (iFlexTimerReference) {                               // power down the FlexTimer after single-shot use
            case 0:
                POWER_DOWN(6, SIM_SCGC6_FTM0);
                break;
    #if FLEX_TIMERS_AVAILABLE > 1
            case 1:
                POWER_DOWN(6, SIM_SCGC6_FTM1);
                break;
    #endif
    #if FLEX_TIMERS_AVAILABLE > 2
            case 2:
        #if defined KINETIS_KL
                POWER_DOWN(6, SIM_SCGC6_FTM2);
        #else
                POWER_DOWN(3, SIM_SCGC3_FTM2);
        #endif
                break;
    #endif
    #if FLEX_TIMERS_AVAILABLE > 3
            case 3:
                POWER_DOWN(3, SIM_SCGC3_FTM3);
                break;
    #endif
            }
        }
    }
    #if defined _WINDOWS
    ptrFlexTimer->FTM_SC &= ~FTM_SC_TOF;
    #endif
    if (_flexTimerHandler[iFlexTimerReference] != 0) {                   // if there is a user handler
        uDisable_Interrupt();
            _flexTimerHandler[iFlexTimerReference]();                    // call user handler
        uEnable_Interrupt();
    }
}

static __interrupt void _flexTimerInterrupt_0(void)
{
    fnHandleFlexTimer((FLEX_TIMER_MODULE *)FTM_BLOCK_0, 0);
}

    #if FLEX_TIMERS_AVAILABLE > 1
static __interrupt void _flexTimerInterrupt_1(void)
{
    fnHandleFlexTimer((FLEX_TIMER_MODULE *)FTM_BLOCK_1, 1);
}
    #endif
    #if FLEX_TIMERS_AVAILABLE > 2
static __interrupt void _flexTimerInterrupt_2(void)
{
    fnHandleFlexTimer((FLEX_TIMER_MODULE *)FTM_BLOCK_2, 2);
}
    #endif
    #if FLEX_TIMERS_AVAILABLE > 3
static __interrupt void _flexTimerInterrupt_3(void)
{
    fnHandleFlexTimer((FLEX_TIMER_MODULE *)FTM_BLOCK_3, 3);
}
    #endif
#endif


/* =================================================================== */
/*                     FlexTimer Configuration                         */
/* =================================================================== */


#if defined _FLEXTIMER_CONFIG_CODE
        {
            TIMER_INTERRUPT_SETUP *ptrTimerSetup = (TIMER_INTERRUPT_SETUP *)ptrSettings;
            int iInterruptID;
            register int iTimerReference = ptrTimerSetup->timer_reference;
            register unsigned long ulDelay = ptrTimerSetup->timer_value;
            register int iPrescaler = 0;
            FLEX_TIMER_MODULE *ptrFlexTimer;
    #if defined KINETIS_KL
            unsigned long ulExtSelect;
        #if defined TPM_CLOCKED_FROM_MCGIRCLK                            // {2}
            #if !defined RUN_FROM_LIRC                                   // {3} if the processor is running from the the internal clock we don't adjust settings here
            MCG_C1 |= (MCG_C1_IRCLKEN | MCG_C1_IREFSTEN);                // enable internal reference clock and allow it to continue running in stop modes
                #if defined USE_FAST_INTERNAL_CLOCK
            MCG_SC = 0;                                                  // remove fast IRC divider
            MCG_C2 |= MCG_C2_IRCS;                                       // select fast internal reference clock (4MHz)
                #else
            MCG_C2 &= ~MCG_C2_IRCS;                                      // select slow internal reference clock (32kHz)
                #endif
            #endif
            SIM_SOPT2 |= SIM_SOPT2_TPMSRC_MCGIRCLK;                      // use MCGIRCLK as timer clock source
        #elif defined TPM_CLOCKED_FROM_OSCERCLK
            OSC0_CR |= (OSC_CR_ERCLKEN | OSC_CR_EREFSTEN);               // enable the external reference clock and keep it enabled in stop mode
            SIM_SOPT2 |= (SIM_SOPT2_TPMSRC_OSCERCLK);                    // use OSCERCLK as timer clock source
        #else
            SIM_SOPT2 |= (SIM_SOPT2_PLLFLLSEL | SIM_SOPT2_TPMSRC_MCG);   // use MCGPLLCLK/2 (or MCGFLL if FLL is used)
        #endif
    #endif
            switch (iTimerReference) {                                   // FlexTimer to be used
            case 0:
                if ((ptrTimerSetup->timer_mode & TIMER_STOP) != 0) {
                    POWER_DOWN(6, SIM_SCGC6_FTM0);
                    return;
                }
                POWER_UP(6, SIM_SCGC6_FTM0);                             // ensure that the FlexTimer module is powered up
                ptrFlexTimer = (FLEX_TIMER_MODULE *)FTM_BLOCK_0;         // KL and KE parts actually use the TPM which is however very similar to the FlexTimer
    #if defined KINETIS_KL
                iInterruptID = irq_TPM0_ID;
        #if defined KINETIS_KL82                                         // {5}
                ulExtSelect = SIM_SOPT9_TPM0CLKSEL;
        #else
                ulExtSelect = SIM_SOPT4_FTM0CLKSEL;
        #endif
    #else
                iInterruptID = irq_FTM0_ID;
    #endif
                break;
    #if FLEX_TIMERS_AVAILABLE > 1
            case 1:
                if ((ptrTimerSetup->timer_mode & TIMER_STOP) != 0) {
                    POWER_DOWN(6, SIM_SCGC6_FTM1);
                    return;
                }
                POWER_UP(6, SIM_SCGC6_FTM1);                             // ensure that the FlexTimer module is powered up
                ptrFlexTimer = (FLEX_TIMER_MODULE *)FTM_BLOCK_1;         // KL and KE parts actually use the TPM which is however very similar to the FlexTimer
        #if defined KINETIS_KL
                iInterruptID = irq_TPM1_ID;
            #if defined KINETIS_KL82                                     // {5}
                ulExtSelect = SIM_SOPT9_TPM1CLKSEL;
            #else
                ulExtSelect = SIM_SOPT4_FTM1CLKSEL;
            #endif
        #else
                iInterruptID = irq_FTM1_ID;
        #endif
                break;
    #endif
    #if FLEX_TIMERS_AVAILABLE > 2
            case 2:
                if ((ptrTimerSetup->timer_mode & TIMER_STOP) != 0) {
        #if defined KINETIS_KL
                    POWER_DOWN(6, SIM_SCGC6_FTM2);
        #else
                    POWER_DOWN(3, SIM_SCGC3_FTM2); 
        #endif
                    return;
                }
        #if defined KINETIS_KL
                POWER_UP(6, SIM_SCGC6_FTM2);
        #else
                POWER_UP(3, SIM_SCGC3_FTM2);                             // ensure that the FlexTimer module is powered up
        #endif
                ptrFlexTimer = (FLEX_TIMER_MODULE *)FTM_BLOCK_2;         // KL and KE parts actually use the TPM which is however very similar to the FlexTimer
        #if defined KINETIS_KL
                iInterruptID = irq_TPM2_ID;
            #if defined KINETIS_KL82                                     // {5}
                ulExtSelect = SIM_SOPT9_TPM2CLKSEL;
            #else
                ulExtSelect = SIM_SOPT4_FTM2CLKSEL;
            #endif
        #else
                iInterruptID = irq_FTM2_ID;
        #endif
                break;
    #endif
    #if FLEX_TIMERS_AVAILABLE > 3
            case 3:
                if ((ptrTimerSetup->timer_mode & TIMER_STOP) != 0) {
                    POWER_DOWN(3, SIM_SCGC3_FTM3); 
                    return;
                }
                POWER_UP(3, SIM_SCGC3_FTM3);                             // ensure that the FlexTimer module is powered up
                ptrFlexTimer = (FLEX_TIMER_MODULE *)FTM_BLOCK_3;
                iInterruptID = irq_FTM3_ID;
                break;
    #endif
            default:
                _EXCEPTION("FlexTimer is unavailable!!");
                return;
            }
            ptrFlexTimer->FTM_SC = 0;                                    // ensure not operating
            if ((ptrFlexTimer->FTM_SC & FTM_SC_TOF) != 0) {              // ensure no pending interrupt
                ptrFlexTimer->FTM_SC = 0;                                // clear pending interrupt (requires a read of the interrupt bit at '1' beforehand)
            }
            ptrFlexTimer->FTM_CONF = FTM_DEBUG_BEHAVIOUR;                // set the debugging behaviour (whether the counter runs in debug mode and how the outputs react)
    #if !defined KINETIS_KL && !defined KINETIS_KE
            ptrFlexTimer->FTM_CNTIN = 0;                                 // counter start value
    #endif
            while (ulDelay > 0xffff) {                                   // calculate the prescaler setting
                if (iPrescaler >= 7) {
                    ulDelay = 0xffff;                                    // set maximum delay
                    break;
                }
                iPrescaler++;
                ulDelay /= 2;
            }
            usFlexTimerMode[iTimerReference] = (unsigned short)iPrescaler;
            if ((ptrTimerSetup->timer_mode & TIMER_PERIODIC) != 0) {     // if periodic operation required
                usFlexTimerMode[iTimerReference] |= FLEX_TIMER_PERIODIC; // mark that periodic mode is being used
            }
            ptrFlexTimer->FTM_MOD = ulDelay;                             // set upper count value
    #if defined KINETIS_KL
            if ((ptrTimerSetup->timer_mode & (TIMER_EXT_CLK_0 | TIMER_EXT_CLK_1)) != 0) { // {4} the external clock source is to be used
                usFlexTimerMode[iTimerReference] |= (FTM_SC_CLKS_EXT | FTM_SC_TOIE | FTM_SC_TOF); // select external clock (which should be half the speed of the module's clock due to synchronisation requirements)
                if ((ptrTimerSetup->timer_mode & (TIMER_EXT_CLK_1)) != 0) {
        #if defined KINETIS_KL82                                         // {5}
                    SIM_SOPT9 |= ulExtSelect;                            // select CLKIN1 source to this timer
        #else
                    SIM_SOPT4 |= ulExtSelect;                            // select CLKIN1 source to this timer
        #endif
        #if defined KINETIS_KL03
                    _CONFIG_PERIPHERAL(B, 6, (PB_6_TPM_CLKIN1 | PORT_PS_UP_ENABLE)); // TPM_CLKIN1 on PB.6 (alt. function 3)
        #else
                    _CONFIG_PERIPHERAL(E, 30, (PE_30_TPM_CLKIN1 | PORT_PS_UP_ENABLE)); // TPM_CLKIN1 on PE.30 (alt. function 4)
        #endif
                }
                else {
        #if defined KINETIS_KL82                                         // {5}
                    SIM_SOPT9 &= ~(ulExtSelect);                         // select CLKIN0 source to this timer
        #else
                    SIM_SOPT4 &= ~(ulExtSelect);                         // select CLKIN0 source to this timer
        #endif
        #if defined KINETIS_KL03
                    _CONFIG_PERIPHERAL(A, 12, (PA_12_TPM_CLKIN0 | PORT_PS_UP_ENABLE)); // TPM_CLKIN0 on PA.12 (alt. function 3)
                  //_CONFIG_PERIPHERAL(A, 1, (PA_1_TPM_CLKIN0 | PORT_PS_UP_ENABLE)); // TPM_CLKIN0 on PA.1 (alt. function 2)
        #else
                    _CONFIG_PERIPHERAL(E, 29, (PE_29_TPM_CLKIN0 | PORT_PS_UP_ENABLE)); // TPM_CLKIN0 on PE.29 (alt. function 4)
        #endif
                }
            }
            else {
                usFlexTimerMode[iTimerReference] |= (FTM_SC_CLKS_SYS | FTM_SC_TOF); // set mode to start (shared by all channels) - system clock with overflow interrupt enabled [FTM_SC_TOF must be written with 1 to clear]
            }
    #else
            usFlexTimerMode[iTimerReference] |= (FTM_SC_CLKS_SYS);       // {1} set mode to start (shared by all channels) - system clock with overflow interrupt enabled [FTM_SC_TOF must be written with 0 to clear]
    #endif
            if ((_flexTimerHandler[iTimerReference] = ptrTimerSetup->int_handler) != 0) { // enter the user interrupt handler
                fnEnterInterrupt(iInterruptID, ptrTimerSetup->int_priority, _flexTimerInterrupt[iTimerReference]); // enter flex timer interrupt handler
                usFlexTimerMode[iTimerReference] |= FTM_SC_TOIE;         // enable interrupt
            }
    #if !defined DEVICE_WITHOUT_DMA
            if ((ptrTimerSetup->timer_mode & TIMER_DMA_TRIGGER) != 0) {  // when DMA required
                ptrFlexTimer->FTM_channel[0].FTM_CV = 0;                 // set the match value for channel 0
                ptrFlexTimer->FTM_channel[0].FTM_CSC = FTM_CSC_DMA;      // enable DMA trigger on match
            }
    #endif
            ptrFlexTimer->FTM_SC = (usFlexTimerMode[iTimerReference] & FTM_SC_USED_MASK); // start timer
        }
#endif

