/***********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher
    
    ---------------------------------------------------------------------
    File:      kinetis_PWM.h
    Project:   Single Chip Embedded Internet
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2017
    *********************************************************************
    22.07.2014 Add clock source selection to TPM                         {1}
    04.01.2016 Added DMA buffer to PWM support                           {2}
    05.01.2016 Added optional PWM cycle interrupt                        {3}
    16.12.2016 Correct PWM interrupt clear                               {4}
    04.01.2017 Don't adjust the RC clock setting when the processor is running from it {5}
    05.03.2017 Add PWM_NO_OUTPUT option to allow PWM channel operation without connecting to an output {6}

*/


#if defined _PWM_CODE                                                    // {3}
/* =================================================================== */
/*                 local function prototype declarations               */
/* =================================================================== */

static __interrupt void _PWM_Interrupt_0(void);
    #if FLEX_TIMERS_AVAILABLE > 1
static __interrupt void _PWM_Interrupt_1(void);
    #endif
    #if FLEX_TIMERS_AVAILABLE > 2
static __interrupt void _PWM_Interrupt_2(void);
    #endif
    #if FLEX_TIMERS_AVAILABLE > 3
static __interrupt void _PWM_Interrupt_3(void);
    #endif
    #if FLEX_TIMERS_AVAILABLE > 4
static __interrupt void _PWM_Interrupt_4(void);
    #endif
    #if FLEX_TIMERS_AVAILABLE > 5
static __interrupt void _PWM_Interrupt_5(void);
    #endif

/* =================================================================== */
/*                      local variable definitions                     */
/* =================================================================== */

static void (*_PWM_TimerHandler[FLEX_TIMERS_AVAILABLE])(void) = {0};     // user interrupt handlers

static void (*_PWM_TimerInterrupt[FLEX_TIMERS_AVAILABLE])(void) = {
    _PWM_Interrupt_0,
    #if FLEX_TIMERS_AVAILABLE > 1
    _PWM_Interrupt_1,
    #endif
    #if FLEX_TIMERS_AVAILABLE > 2
    _PWM_Interrupt_2,
    #endif
    #if FLEX_TIMERS_AVAILABLE > 3
    _PWM_Interrupt_3,
    #endif
    #if FLEX_TIMERS_AVAILABLE > 4
    _PWM_Interrupt_4,
    #endif
    #if FLEX_TIMERS_AVAILABLE > 5
    _PWM_Interrupt_5
    #endif
};

/* =================================================================== */
/*                   PWM cycle Interrupt Handlers                      */
/* =================================================================== */

static __interrupt void _PWM_Interrupt_0(void)
{
    FTM0_SC &= ~(FTM_SC_TOF);                                            // {4} clear interrupt (read when set and write 0 to reset)
    if (_PWM_TimerHandler[0] != 0) {                                     // if there is a user handler installed
        uDisable_Interrupt();
            _PWM_TimerHandler[0]();                                      // call user interrupt handler
        uEnable_Interrupt();
    }
}

    #if FLEX_TIMERS_AVAILABLE > 1
static __interrupt void _PWM_Interrupt_1(void)
{
    FTM1_SC &= ~(FTM_SC_TOF);                                            // {4} clear interrupt (read when set and write 0 to reset)
    if (_PWM_TimerHandler[1] != 0) {                                     // if there is a user handler installed
        uDisable_Interrupt();
            _PWM_TimerHandler[1]();                                      // call user interrupt handler
        uEnable_Interrupt();
    }
}
    #endif
    #if FLEX_TIMERS_AVAILABLE > 2
static __interrupt void _PWM_Interrupt_2(void)
{
    FTM2_SC &= ~(FTM_SC_TOF);                                            // {4} clear interrupt (read when set and write 0 to reset)
    if (_PWM_TimerHandler[2] != 0) {                                     // if there is a user handler installed
        uDisable_Interrupt();
            _PWM_TimerHandler[2]();                                      // call user interrupt handler
        uEnable_Interrupt();
    }
}
    #endif
    #if FLEX_TIMERS_AVAILABLE > 3
static __interrupt void _PWM_Interrupt_3(void)
{
    FTM3_SC &= ~(FTM_SC_TOF);                                            // {4} clear interrupt (read when set and write 0 to reset)
    if (_PWM_TimerHandler[3] != 0) {                                     // if there is a user handler installed
        uDisable_Interrupt();
            _PWM_TimerHandler[3]();                                      // call user interrupt handler
        uEnable_Interrupt();
    }
}
    #endif
    #if FLEX_TIMERS_AVAILABLE > 4
static __interrupt void _PWM_Interrupt_4(void)
{
    FTM4_SC &= ~(FTM_SC_TOF);                                            // clear interrupt (read when set and write 0 to reset)
    if (_PWM_TimerHandler[4] != 0) {                                     // if there is a user handler installed
        uDisable_Interrupt();
            _PWM_TimerHandler[4]();                                      // call user interrupt handler
        uEnable_Interrupt();
    }
}
    #endif
    #if FLEX_TIMERS_AVAILABLE > 5
static __interrupt void _PWM_Interrupt_5(void)
{
    FTM5_SC &= ~(FTM_SC_TOF);                                            // clear interrupt (read when set and write 0 to reset)
    if (_PWM_TimerHandler[5] != 0) {                                     // if there is a user handler installed
        uDisable_Interrupt();
            _PWM_TimerHandler[5]();                                      // call user interrupt handler
        uEnable_Interrupt();
    }
}
    #endif
#endif

#if defined _PWM_CONFIG_CODE
        {
    #if FLEX_TIMERS_AVAILABLE > 4 && defined TPMS_AVAILABLE
            int iTPM_type = 0;
    #endif
            PWM_INTERRUPT_SETUP *ptrPWM_settings = (PWM_INTERRUPT_SETUP *)ptrSettings;
            int iInterruptID;
            unsigned long ulMode = ptrPWM_settings->pwm_mode;
            unsigned char ucChannel = (ptrPWM_settings->pwm_reference & ~_TIMER_MODULE_MASK);
            unsigned char ucFlexTimer = (ptrPWM_settings->pwm_reference >> _TIMER_MODULE_SHIFT);
            FLEX_TIMER_MODULE *ptrFlexTimer;
    #if defined KINETIS_KL
        #if defined TPM_CLOCKED_FROM_MCGIRCLK                            // {1}
            #if !defined RUN_FROM_LIRC                                   // {5} if the processor is running from the the internal clock we don't change settings here
            MCG_C1 |= (MCG_C1_IRCLKEN | MCG_C1_IREFSTEN);                // enable internal reference clock and allow it to continue running in stop modes
                #if defined USE_FAST_INTERNAL_CLOCK
            MCG_SC = MCG_SC_FCRDIV_1;                                    // remove fast IRC divider
            MCG_C2 |= MCG_C2_IRCS;                                       // select fast internal reference clock (4MHz [8MHz for devices with MCG Lite]) as MCGIRCLK
                #else
            MCG_C2 &= ~MCG_C2_IRCS;                                      // select slow internal reference clock (32kHz [2MHz for devices with MCG Lite]) as MCGIRCLK
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
            switch (ucFlexTimer) {
            case 0:
                POWER_UP(6, SIM_SCGC6_FTM0);                             // ensure that the FlexTimer module is powered up
    #if defined KINETIS_KL
                iInterruptID = irq_TPM0_ID;
    #else
                iInterruptID = irq_FTM0_ID;
    #endif
                if ((ulMode & PWM_NO_OUTPUT) == 0) {                     // {6}
                    switch (ptrPWM_settings->pwm_reference & ~_TIMER_MODULE_MASK) { // configure appropriate pin for the PWM output signal
                    case 0:                                              // timer 0, channel 0
    #if defined KINETIS_KE
        #if defined FTM0_0_ON_B
                        SIM_PINSEL0 |= SIM_PINSEL_FTM0PS0;
                        _CONFIG_PERIPHERAL(B, 2, (PB_2_FTM0_CH0 | PORT_SRE_FAST | PORT_DSE_HIGH)); // FTM0_CH0 on PB.2 (alt. function 3)
        #else
                        SIM_PINSEL0 &= ~SIM_PINSEL_FTM0PS0;
                        _CONFIG_PERIPHERAL(A, 0, (PA_0_FTM0_CH0 | PORT_SRE_FAST | PORT_DSE_HIGH)); // FTM0_CH0 on PA.0 (alt. function 2)
        #endif
    #elif (defined KINETIS_KL02 || defined KINETIS_KL03) && defined TPM0_0_ON_A
                    _CONFIG_PERIPHERAL(A, 6,(PA_6_TPM0_CH0 | PORT_SRE_FAST | PORT_DSE_HIGH)); // TPM0_CH0 on PA.6 (alt. function 2)
    #elif defined KINETIS_KL02 || defined KINETIS_KL03 || defined KINETIS_KL04 || defined KINETIS_KL05
                        _CONFIG_PERIPHERAL(B, 11,(PB_11_TPM0_CH0 | PORT_SRE_FAST | PORT_DSE_HIGH)); // TPM0_CH0 on PB.11 (alt. function 2)
    #elif (defined KINETIS_KL25 || defined KINETIS_KL26 || defined KINETIS_KL27 || defined KINETIS_KL43) && defined TPM0_0_ON_D
                        _CONFIG_PERIPHERAL(D, 0, (PD_0_FTM0_CH0 | PORT_SRE_FAST | PORT_DSE_HIGH)); // FTM0_CH0 on PD.0 (alt. function 4)
    #elif (defined KINETIS_KL25 || defined KINETIS_KL26 || defined KINETIS_KL27 || defined KINETIS_KL43) && defined TPM0_0_ON_E
                        _CONFIG_PERIPHERAL(E, 24, (PE_24_TPM0_CH0 | PORT_SRE_FAST | PORT_DSE_HIGH)); // TPM0_CH0 on PE.24 (alt. function 3)
    #elif defined FTM0_0_ON_C
                        _CONFIG_PERIPHERAL(C, 1, (PC_1_FTM0_CH0 | PORT_SRE_FAST | PORT_DSE_HIGH)); // FTM0_CH0 on PC.1 (alt. function 4)
    #else
                        _CONFIG_PERIPHERAL(A, 3, (PA_3_FTM0_CH0 | PORT_SRE_FAST | PORT_DSE_HIGH)); // FTM0_CH0 on PA.3 (alt. function 3)
    #endif
                        break;
                    case 1:                                              // timer 0, channel 1
    #if defined KINETIS_KE
        #if defined FTM0_1_ON_B
                        SIM_PINSEL0 |= SIM_PINSEL_FTM0PS1;
                        _CONFIG_PERIPHERAL(B, 3, (PB_3_FTM0_CH1 | PORT_SRE_FAST | PORT_DSE_HIGH)); // FTM0_CH1 on PB.3 (alt. function 3)
        #else
                        SIM_PINSEL0 &= ~SIM_PINSEL_FTM0PS1;
                        _CONFIG_PERIPHERAL(A, 1, (PA_1_FTM0_CH1 | PORT_SRE_FAST | PORT_DSE_HIGH)); // FTM0_CH1 on PA.1 (alt. function 2)
        #endif
    #elif (defined KINETIS_KL02 || defined KINETIS_KL03) && defined TPM0_1_ON_A
                        _CONFIG_PERIPHERAL(A, 5, (PA_5_TPM0_CH1 | PORT_SRE_FAST | PORT_DSE_HIGH)); // TPM0_CH1 on PA.5 (alt. function 2)
    #elif defined KINETIS_KL02 || defined KINETIS_KL03 || defined KINETIS_KL04 || defined KINETIS_KL05
                        _CONFIG_PERIPHERAL(B, 10,(PB_10_TPM0_CH1 | PORT_SRE_FAST | PORT_DSE_HIGH)); // TPM0_CH1 on PB.10 (alt. function 2)
    #elif (defined KINETIS_KL25 || defined KINETIS_KL26 || defined KINETIS_KL27 || defined KINETIS_KL43) && defined TPM0_1_ON_D
                        _CONFIG_PERIPHERAL(D, 1, (PD_1_FTM0_CH1 | PORT_SRE_FAST | PORT_DSE_HIGH)); // FTM0_CH1 on PD.1 (alt. function 4)
    #elif (defined KINETIS_KL25 || defined KINETIS_KL26 || defined KINETIS_KL27 || defined KINETIS_KL43) && defined TPM0_1_ON_E
                        _CONFIG_PERIPHERAL(E, 25, (PE_25_TPM0_CH1 | PORT_SRE_FAST | PORT_DSE_HIGH)); // TPM0_CH1 on PE.25 (alt. function 3)
    #elif defined FTM0_1_ON_C
                        _CONFIG_PERIPHERAL(C, 2, (PC_2_FTM0_CH1 | PORT_SRE_FAST | PORT_DSE_HIGH)); // FTM0_CH1 on PC.2 (alt. function 4)
    #else
                        _CONFIG_PERIPHERAL(A, 4, (PA_4_FTM0_CH1 | PORT_SRE_FAST | PORT_DSE_HIGH)); // FTM0_CH1 on PA.4 (alt. function 3)
    #endif
                        break;
    #if !defined KINETIS_KL02 && !defined KINETIS_KL03 && !defined KINETIS_KE
                    case 2:                                              // timer 0, channel 2
        #if defined KINETIS_KL04 || defined KINETIS_KL05
                        _CONFIG_PERIPHERAL(B, 9, (PB_9_TPM0_CH2 | PORT_SRE_FAST | PORT_DSE_HIGH)); // TPM0_CH2 on PB.9 (alt. function 2)
        #elif (defined KINETIS_KL25 || defined KINETIS_KL26 || defined KINETIS_KL27 || defined KINETIS_KL43) && defined TPM0_2_ON_D
                        _CONFIG_PERIPHERAL(D, 2, (PD_2_FTM0_CH2 | PORT_SRE_FAST | PORT_DSE_HIGH)); // FTM0_CH2 on PD.2 (alt. function 4)
        #elif (defined KINETIS_KL25 || defined KINETIS_KL26 || defined KINETIS_KL27 || defined KINETIS_KL43) && defined TPM0_2_ON_E
                        _CONFIG_PERIPHERAL(E, 29, (PE_29_TPM0_CH2 | PORT_SRE_FAST | PORT_DSE_HIGH)); // TPM0_CH2 on PE.29 (alt. function 3)
        #elif defined FTM0_2_ON_C
                        _CONFIG_PERIPHERAL(C, 3, (PC_3_FTM0_CH2 | PORT_SRE_FAST | PORT_DSE_HIGH)); // FTM0_CH2 on PC.3 (alt. function 4)
        #else
                        _CONFIG_PERIPHERAL(A, 5, (PA_5_FTM0_CH2 | PORT_SRE_FAST | PORT_DSE_HIGH)); // FTM0_CH2 on PA.5 (alt. function 3)
        #endif
                        break;
                    case 3:                                              // timer 0, channel 3
        #if defined KINETIS_KL04 || defined KINETIS_KL05
                        _CONFIG_PERIPHERAL(B, 8, (PB_8_TPM0_CH3 | PORT_SRE_FAST | PORT_DSE_HIGH)); // TPM0_CH3 on PB.8 (alt. function 2)
        #elif (defined KINETIS_KL25 || defined KINETIS_KL26 || defined KINETIS_KL27 || defined KINETIS_KL43) && defined TPM0_3_ON_D
                        _CONFIG_PERIPHERAL(D, 3, (PD_3_FTM0_CH3 | PORT_SRE_FAST | PORT_DSE_HIGH)); // FTM0_CH3 on PD.3 (alt. function 4)
        #elif (defined KINETIS_KL25 || defined KINETIS_KL26 || defined KINETIS_KL27 || defined KINETIS_KL43) && defined TPM0_3_ON_E
                        _CONFIG_PERIPHERAL(E, 30, (PE_30_TPM0_CH3 | PORT_SRE_FAST | PORT_DSE_HIGH)); // TPM0_CH3 on PE.30 (alt. function 3)
        #elif (defined FTM0_3_ON_C && (defined KINETIS_K64  || defined KINETIS_KL43))
                        _CONFIG_PERIPHERAL(C, 4, (PC_4_FTM0_CH3 | PORT_SRE_FAST | PORT_DSE_HIGH)); // FTM0_CH3 on PC.4 (alt. function 4)
        #else
                        _CONFIG_PERIPHERAL(A, 6, (PA_6_FTM0_CH3 | PORT_SRE_FAST | PORT_DSE_HIGH)); // FTM0_CH3 on PA.6 (alt. function 3)
        #endif
                        break;
                    case 4:                                              // timer 0, channel 4
        #if defined KINETIS_KL04 || defined KINETIS_KL05
                        _CONFIG_PERIPHERAL(A, 6, (PA_6_TPM0_CH4 | PORT_SRE_FAST | PORT_DSE_HIGH)); // TPM0_CH4 on PA.6 (alt. function 2)
        #elif (defined KINETIS_KL25 || defined KINETIS_KL26 || defined KINETIS_KL27 || defined KINETIS_KL43) && defined TPM0_4_ON_E
                        _CONFIG_PERIPHERAL(E, 31, (PE_31_TPM0_CH4 | PORT_SRE_FAST | PORT_DSE_HIGH)); // TPM0_CH4 on PE.31 (alt. function 3)
        #elif defined FTM0_4_ON_D
                        _CONFIG_PERIPHERAL(D, 4, (PD_4_FTM0_CH4 | PORT_SRE_FAST | PORT_DSE_HIGH)); // FTM0_CH4 on PD.4 (alt. function 4)
        #else
                        _CONFIG_PERIPHERAL(A, 7, (PA_7_FTM0_CH4 | PORT_SRE_FAST | PORT_DSE_HIGH)); // FTM0_CH4 on PA.7 (alt. function 3)
        #endif
                        break;
                    case 5:                                              // timer 0, channel 5
        #if defined KINETIS_KL04 || defined KINETIS_KL05
                        _CONFIG_PERIPHERAL(A, 5, (PA_5_TPM0_CH5 | PORT_SRE_FAST | PORT_DSE_HIGH)); // TPM0_CH5 on PA.5 (alt. function 2)
        #elif (defined KINETIS_KL25 || defined KINETIS_KL26 || defined KINETIS_KL27) && defined TPM0_5_ON_E
                        _CONFIG_PERIPHERAL(E, 26, (PE_26_TPM0_CH5 | PORT_SRE_FAST | PORT_DSE_HIGH)); // TPM0_CH5 on PE.26 (alt. function 3)
        #elif defined FTM0_5_ON_D
                        _CONFIG_PERIPHERAL(D, 5, (PD_5_FTM0_CH5 | PORT_SRE_FAST | PORT_DSE_HIGH)); // FTM0_CH5 on PD.5 (alt. function 4)
        #else
                        _CONFIG_PERIPHERAL(A, 0, (PA_0_FTM0_CH5 | PORT_SRE_FAST | PORT_DSE_HIGH)); // FTM0_CH5 on PA.0 (alt. function 3)
        #endif
                        break;
        #if !(defined KINETIS_KL04 || defined KINETIS_KL05)
                    case 6:                                              // timer 0, channel 6
            #if defined FTM0_6_ON_D
                        _CONFIG_PERIPHERAL(D, 6, (PD_6_FTM0_CH6 | PORT_SRE_FAST | PORT_DSE_HIGH)); // FTM0_CH6 on PD.6 (alt. function 4)
            #else
                        _CONFIG_PERIPHERAL(A, 1, (PA_1_FTM0_CH6 | PORT_SRE_FAST | PORT_DSE_HIGH)); // FTM0_CH6 on PA.1 (alt. function 3)
            #endif
                        break;
                    case 7:                                              // timer 0, channel 7
            #if defined FTM0_7_ON_D
                        _CONFIG_PERIPHERAL(D, 7, (PD_7_FTM0_CH7 | PORT_SRE_FAST | PORT_DSE_HIGH)); // FTM0_CH7 on PD.7 (alt. function 4)
            #else
                        _CONFIG_PERIPHERAL(A, 6, (PA_6_FTM0_CH7 | PORT_SRE_FAST | PORT_DSE_HIGH)); // FTM0_CH7 on PA.6 (alt. function 3)
            #endif
                        break;
        #endif
    #endif
                    default:
    #if defined _WINDOWS
                        _EXCEPTION("Invalid timer channel!!");
    #endif
                        return;                                          // invalid channel
                    }
                }
                ptrFlexTimer = (FLEX_TIMER_MODULE *)FTM_BLOCK_0;
                break;
    #if FLEX_TIMERS_AVAILABLE > 1
            case 1:
                POWER_UP(6, SIM_SCGC6_FTM1);                             // ensure that the FlexTimer module is powered up
        #if defined KINETIS_KL
                iInterruptID = irq_TPM1_ID;
        #else
                iInterruptID = irq_FTM1_ID;
        #endif
                if ((ulMode & PWM_NO_OUTPUT) == 0) {                     // {6}
                    switch (ptrPWM_settings->pwm_reference & ~_TIMER_MODULE_MASK) { // configure appropriate pin for the PWM output signal
                    case 0:                                              // timer 1, channel 0
        #if defined KINETIS_KE
            #if defined FTM1_0_ON_H
                        SIM_PINSEL0 |= SIM_PINSEL_FTM1PS0;
                        _CONFIG_PERIPHERAL(H, 2, (PH_2_FTM1_CH0 | PORT_SRE_FAST | PORT_DSE_HIGH)); // FTM1_CH0 on PH.2 (alt. function 4)
            #else
                        SIM_PINSEL0 &= ~SIM_PINSEL_FTM1PS0;
                        _CONFIG_PERIPHERAL(C, 4, (PC_4_FTM1_CH0 | PORT_SRE_FAST | PORT_DSE_HIGH)); // FTM1_CH0 on PC.4
            #endif
        #elif (defined KINETIS_KL02 || defined KINETIS_KL03) && defined TPM1_0_ALT_2
                        _CONFIG_PERIPHERAL(B, 7, (PB_7_TPM1_CH0 | PORT_SRE_FAST | PORT_DSE_HIGH)); // TPM1_CH0 on PB.7 (alt. function 2)
        #elif (defined KINETIS_KL02 || defined KINETIS_KL03 || defined KINETIS_KL04 || defined KINETIS_KL05) && defined TPM1_0_ALT
                        _CONFIG_PERIPHERAL(A, 12, (PA_12_TPM1_CH0 | PORT_SRE_FAST | PORT_DSE_HIGH)); // TPM1_CH0 on PA.12 (alt. function 2)
        #elif defined KINETIS_KL02 || defined KINETIS_KL03 || defined KINETIS_KL04 || defined KINETIS_KL05
                        _CONFIG_PERIPHERAL(A, 0, (PA_0_TPM1_CH0 | PORT_SRE_FAST | PORT_DSE_HIGH)); // TPM1_CH0 on PA.0 (alt. function 2)
        #elif (defined KINETIS_KL25 || defined KINETIS_KL26 || defined KINETIS_KL27 || defined KINETIS_KL43) && defined TPM1_0_ON_E
                        _CONFIG_PERIPHERAL(E, 20, (PE_20_TPM1_CH0 | PORT_SRE_FAST | PORT_DSE_HIGH)); // TPM1_CH0 on PE.20 (alt. function 3)
        #elif defined FTM1_0_ON_B
                        _CONFIG_PERIPHERAL(B, 0, (PB_0_FTM1_CH0 | PORT_SRE_FAST | PORT_DSE_HIGH)); // FTM1_CH0 on PB.0 (alt. function 3)
        #elif defined FTM1_0_ALT_A
                        _CONFIG_PERIPHERAL(A, 12, (PA_12_FTM1_CH0 | PORT_SRE_FAST | PORT_DSE_HIGH)); // FTM1_CH0 on PA.12 (alt. function 3)
        #else
                        _CONFIG_PERIPHERAL(A, 8, (PA_8_FTM1_CH0 | PORT_SRE_FAST | PORT_DSE_HIGH)); // FTM1_CH0 on PA.8 (alt. function 3)
        #endif
                        break;
                    case 1:                                              // timer 1, channel 1
        #if defined KINETIS_KE
            #if defined FTM1_1_ON_E
                        SIM_PINSEL0 |= SIM_PINSEL_FTM1PS1;
                        _CONFIG_PERIPHERAL(E, 7, (PE_7_FTM1_CH1 | PORT_SRE_FAST | PORT_DSE_HIGH)); // FTM1_CH1 on PE.7 (alt. function 4)
            #else
                        SIM_PINSEL0 &= ~SIM_PINSEL_FTM1PS1;
                        _CONFIG_PERIPHERAL(C, 5, (PC_5_FTM1_CH1 | PORT_SRE_FAST | PORT_DSE_HIGH)); // FTM1_CH1 on PC.5
            #endif
        #elif (defined KINETIS_KL02 || defined KINETIS_KL03) && defined TPM1_1_ALT_2
                        _CONFIG_PERIPHERAL(B, 6, (PB_6_TPM1_CH0 | PORT_SRE_FAST | PORT_DSE_HIGH)); // TPM1_CH0 on PB.6 (alt. function 2)
        #elif defined KINETIS_KL02 || defined KINETIS_KL03 || defined KINETIS_KL04 || defined KINETIS_KL05 && defined FTM1_1_ALT
                        _CONFIG_PERIPHERAL(B, 13, (PB_13_TPM1_CH1 | PORT_SRE_FAST | PORT_DSE_HIGH)); // TPM1_CH1 on PB.13 (alt. function 2)
        #elif defined KINETIS_KL02 || defined KINETIS_KL03 || defined KINETIS_KL04 || defined KINETIS_KL05
                        _CONFIG_PERIPHERAL(B, 5, (PB_5_TPM1_CH1 | PORT_SRE_FAST | PORT_DSE_HIGH)); // TPM1_CH1 on PB.5 (alt. function 2)
        #elif (defined KINETIS_KL25 || defined KINETIS_KL26 || defined KINETIS_KL27 || defined KINETIS_KL43) && defined TPM1_1_ON_E
                        _CONFIG_PERIPHERAL(E, 21, (PE_21_TPM1_CH1 | PORT_SRE_FAST | PORT_DSE_HIGH)); // TPM1_CH1 on PE.21 (alt. function 3)
        #elif defined FTM1_1_ON_B
                        _CONFIG_PERIPHERAL(B, 1, (PB_1_FTM1_CH1 | PORT_SRE_FAST | PORT_DSE_HIGH)); // FTM1_CH1 on PB.1 (alt. function 3)
        #elif defined FTM1_1_ALT_A
                        _CONFIG_PERIPHERAL(A, 13, (PA_13_FTM1_CH1 | PORT_SRE_FAST | PORT_DSE_HIGH)); // FTM1_CH1 on PA.13 (alt. function 3)
        #else
                        _CONFIG_PERIPHERAL(A, 9, (PA_9_FTM1_CH1 | PORT_SRE_FAST | PORT_DSE_HIGH)); // FTM1_CH1 on PA.9 (alt. function 3)
        #endif
                        break;
                    default:
                        _EXCEPTION("Invalid timer channel!!");
                        return;                                              // invalid channel
                    }
                }
                ptrFlexTimer = (FLEX_TIMER_MODULE *)FTM_BLOCK_1;
                break;
    #endif
    #if FLEX_TIMERS_AVAILABLE > 2
            case 2:
        #if defined KINETIS_KL
                POWER_UP(6, SIM_SCGC6_FTM2);
        #else
                POWER_UP(3, SIM_SCGC3_FTM2);                             // ensure that the FlexTimer module is powered up
        #endif
                ptrFlexTimer = (FLEX_TIMER_MODULE *)FTM_BLOCK_2;
        #if defined KINETIS_KE
                ptrFlexTimer->FTM_CONF = FTM_DEBUG_BEHAVIOUR;            // set the debugging behaviour (whether the counter runs in debug mode and how the outputs react - only available on FlexTimer 2)
        #endif
        #if defined KINETIS_KL
                iInterruptID = irq_TPM2_ID;
        #else
                iInterruptID = irq_FTM2_ID;
        #endif
                if ((ulMode & PWM_NO_OUTPUT) == 0) {                     // {6}
                    switch (ptrPWM_settings->pwm_reference & ~_TIMER_MODULE_MASK) { // configure appropriate pin for the PWM output signal
                    case 0:                                              // timer 2, channel 0
        #if defined KINETIS_KE
            #if defined FTM2_0_ON_H
                        SIM_PINSEL0 |= SIM_PINSEL_FTM1PS1;
                        _CONFIG_PERIPHERAL(H, 0, (PH_0_FTM2_CH0 | PORT_SRE_FAST | PORT_DSE_HIGH)); // FTM2_CH0 on PH.0 (alt. function 2)
            #elif defined FTM2_0_ON_F
                        _CONFIG_PERIPHERAL(F, 0, (PF_0_FTM2_CH0 | PORT_SRE_FAST | PORT_DSE_HIGH)); // FTM2_CH0 on PF.0 (alt. function 2)
            #else
                        SIM_PINSEL0 &= ~SIM_PINSEL_FTM1PS1;
                        _CONFIG_PERIPHERAL(C, 0, (PC_0_FTM2_CH0 | PORT_SRE_FAST | PORT_DSE_HIGH)); // FTM2_CH0 on PC.0 (alt. function 2)
            #endif
        #elif (defined KINETIS_KL25 || defined KINETIS_KL26 || defined KINETIS_KL27) && defined TPM2_0_ON_E
                        _CONFIG_PERIPHERAL(E, 22, (PE_22_TPM2_CH0 | PORT_SRE_FAST | PORT_DSE_HIGH)); // TPM2_CH0 on PE.22 (alt. function 3)
        #elif (defined KINETIS_KL25 || defined KINETIS_KL26 || defined KINETIS_KL27) && defined FTM2_0_ON_B_LOW
                        _CONFIG_PERIPHERAL(B, 2,  (PB_2_FTM2_CH0 | PORT_SRE_FAST | PORT_DSE_HIGH)); // FTM2_CH0 on PB.2 (alt. function 3)
        #elif defined FTM2_0_ON_B
                        _CONFIG_PERIPHERAL(B, 18, (PB_18_FTM2_CH0 | PORT_SRE_FAST | PORT_DSE_HIGH)); // FTM2_CH0 on PB.18 (alt. function 3)
        #else
                        _CONFIG_PERIPHERAL(A, 10, (PA_10_FTM2_CH0 | PORT_SRE_FAST | PORT_DSE_HIGH)); // FTM2_CH0 on PA.10 (alt. function 3)
        #endif
                        break;
                    case 1:                                              // timer 2, channel 1
        #if defined KINETIS_KE
            #if defined FTM2_1_ON_H
                #if defined KINETIS_KE06
                        SIM_PINSEL1 |= SIM_PINSEL1_FTM2PS0_PTH0;
                        SIM_PINSEL1 &= ~SIM_PINSEL1_FTM2PS0_PTF0;
                #else
                        SIM_PINSEL0 |= SIM_PINSEL_FTM2PS1;
                #endif
                        _CONFIG_PERIPHERAL(H, 1, (PH_1_FTM2_CH1 | PORT_SRE_FAST | PORT_DSE_HIGH)); // FTM2_CH1 on PH.1 (alt. function 2)
            #elif defined FTM2_1_ON_F
                        SIM_PINSEL1 &= ~SIM_PINSEL1_FTM2PS0_PTH0;
                        SIM_PINSEL1 |= SIM_PINSEL1_FTM2PS0_PTF0;
                        _CONFIG_PERIPHERAL(F, 1, (PF_1_FTM2_CH1 | PORT_SRE_FAST | PORT_DSE_HIGH)); // FTM2_CH1 on PF.1 (alt. function 2)
            #else
                #if ((defined KINETIS_KE04 && (SIZE_OF_FLASH > (8 * 1024))) || defined KINETIS_KE06) || defined KINETIS_KEA64 || defined KINETIS_KEA128
                        SIM_PINSEL1 &= ~(SIM_PINSEL1_FTM2PS0_PTH0 | SIM_PINSEL1_FTM2PS0_PTF0);
                #else
                        SIM_PINSEL0 &= ~SIM_PINSEL_FTM2PS1;
                #endif
                        _CONFIG_PERIPHERAL(C, 1, (PC_1_FTM2_CH1 | PORT_SRE_FAST | PORT_DSE_HIGH)); // FTM2_CH1 on PC.1 (alt. function 2)
            #endif
        #elif (defined KINETIS_KL25 || defined KINETIS_KL26 || defined KINETIS_KL27) && defined TPM2_1_ON_E
                        _CONFIG_PERIPHERAL(E, 23, (PE_23_TPM2_CH1 | PORT_SRE_FAST | PORT_DSE_HIGH)); // TPM2_CH1 on PE.23 (alt. function 3)
        #elif (defined KINETIS_KL25 || defined KINETIS_KL26 || defined KINETIS_KL27) && defined FTM2_1_ON_B_LOW
                        _CONFIG_PERIPHERAL(B, 3,  (PB_3_FTM2_CH1 | PORT_SRE_FAST | PORT_DSE_HIGH)); // FTM2_CH1 on PB.3 (alt. function 3)
        #elif defined FTM2_1_ON_B
                        _CONFIG_PERIPHERAL(B, 19, (PB_19_FTM2_CH1 | PORT_SRE_FAST | PORT_DSE_HIGH)); // FTM2_CH1 on PB.19 (alt. function 3)
        #else
                        _CONFIG_PERIPHERAL(A, 11, (PA_11_FTM2_CH1 | PORT_SRE_FAST | PORT_DSE_HIGH)); // FTM2_CH1 on PA.11 (alt. function 3)
        #endif
                        break;
        #if defined KINETIS_KE
                    case 2:                                              // timer 2, channel 2
            #if defined FTM2_2_ON_D
                #if defined KINETIS_KE06
                        SIM_PINSEL1 |= SIM_PINSEL1_FTM2PS2_PTD0;
                        SIM_PINSEL1 &= ~SIM_PINSEL1_FTM2PS2_PTG4;
                #else
                        SIM_PINSEL0 |= SIM_PINSEL_FTM2PS2;
                #endif
                        _CONFIG_PERIPHERAL(D, 0, (PD_0_FTM2_CH2 | PORT_SRE_FAST | PORT_DSE_HIGH)); // FTM2_CH2 on PD.0 (alt. function 2)
            #elif defined FTM2_2_ON_G
                        SIM_PINSEL1 |= (SIM_PINSEL1_FTM2PS2_PTD0 | SIM_PINSEL1_FTM2PS2_PTG4);
                        _CONFIG_PERIPHERAL(G, 4, (PG_4_FTM2_CH2 | PORT_SRE_FAST | PORT_DSE_HIGH)); // FTM2_CH2 on PG.4 (alt. function 2)
            #else
                #if ((defined KINETIS_KE04 && (SIZE_OF_FLASH > (8 * 1024))) || defined KINETIS_KE06) || defined KINETIS_KEA64 || defined KINETIS_KEA128
                        SIM_PINSEL1 &= ~(SIM_PINSEL1_FTM2PS2_PTD0 | SIM_PINSEL1_FTM2PS2_PTG4);
                #else
                        SIM_PINSEL0 &= ~SIM_PINSEL_FTM2PS2;
                #endif
                        _CONFIG_PERIPHERAL(C, 2, (PC_2_FTM2_CH2 | PORT_SRE_FAST | PORT_DSE_HIGH)); // FTM2_CH2 on PC.2 (alt. function 1)
            #endif
                        break;
                    case 3:                                              // timer 2, channel 3
            #if defined FTM2_3_ON_D
                #if defined KINETIS_KE06
                        SIM_PINSEL1 |= SIM_PINSEL1_FTM2PS3_PTD1;
                        SIM_PINSEL1 &= ~SIM_PINSEL1_FTM2PS3_PTG5;
                #else
                        SIM_PINSEL0 |= SIM_PINSEL_FTM2PS3;
                #endif
                        _CONFIG_PERIPHERAL(D, 1, (PD_1_FTM2_CH3 | PORT_SRE_FAST | PORT_DSE_HIGH)); // FTM2_CH3 on PD.1 (alt. function 2)
            #elif defined FTM2_3_ON_G
                        SIM_PINSEL1 &= ~(SIM_PINSEL1_FTM2PS3_PTD1);
                        SIM_PINSEL1 |= SIM_PINSEL1_FTM2PS3_PTG5;
                        _CONFIG_PERIPHERAL(G, 5, (PG_5_FTM2_CH3 | PORT_SRE_FAST | PORT_DSE_HIGH)); // FTM2_CH3 on PG.5 (alt. function 2)
            #else
                    #if ((defined KINETIS_KE04 && (SIZE_OF_FLASH > (8 * 1024))) || defined KINETIS_KE06) || defined KINETIS_KEA64 ||  defined KINETIS_KEA128
                        SIM_PINSEL1 &= ~(SIM_PINSEL1_FTM2PS3_PTD1 | SIM_PINSEL1_FTM2PS3_PTG5);
                    #else
                        SIM_PINSEL0 &= ~SIM_PINSEL_FTM2PS3;
                    #endif
                        _CONFIG_PERIPHERAL(C, 3, (PC_3_FTM2_CH3 | PORT_SRE_FAST | PORT_DSE_HIGH)); // FTM2_CH3 on PC.3 (alt. function 1)
            #endif
                        break;
                    case 4:                                                  // timer 2, channel 4
            #if defined FTM2_4_ON_G
                        SIM_PINSEL1 |= SIM_PINSEL1_FTM2PS4;
                        _CONFIG_PERIPHERAL(G, 6, (PG_6_FTM2_CH4 | PORT_SRE_FAST | PORT_DSE_HIGH)); // FTM2_CH4 on PG.6 (alt. function 2)
            #else
                #if defined KINETIS_KE06
                        SIM_PINSEL1 &= ~SIM_PINSEL1_FTM2PS4;
                #endif
                        _CONFIG_PERIPHERAL(B, 4, (PB_4_FTM2_CH4 | PORT_SRE_FAST | PORT_DSE_HIGH)); // FTM2_CH4 on PB.4 (alt. function 1)
            #endif
                        break;
                    case 5:                                                  // timer 2, channel 5
            #if defined FTM2_5_ON_G
                        SIM_PINSEL1 |= SIM_PINSEL1_FTM2PS5;
                        _CONFIG_PERIPHERAL(G, 7, (PG_7_FTM2_CH5 | PORT_SRE_FAST | PORT_DSE_HIGH)); // FTM2_CH5 on PG.7 (alt. function 2)
            #else
                #if defined KINETIS_KE06
                        SIM_PINSEL1 &= ~SIM_PINSEL1_FTM2PS5;
                #endif
                        _CONFIG_PERIPHERAL(B, 5, (PB_5_FTM2_CH5 | PORT_SRE_FAST | PORT_DSE_HIGH)); // FTM2_CH5 on PB.5 (alt. function 1)
            #endif
                        break;
        #endif
                    default:
                        _EXCEPTION("Invalid timer channel!!");
                        return;                                          // invalid channel
                    }
                }
                break;
    #endif
    #if FLEX_TIMERS_AVAILABLE > 3
            case 3:
                POWER_UP(3, SIM_SCGC3_FTM3);                             // ensure that the FlexTimer module is powered up
        #if defined KINETIS_KL
                iInterruptID = irq_TPM3_ID;
        #else
                iInterruptID = irq_FTM3_ID;
        #endif
                ptrFlexTimer = (FLEX_TIMER_MODULE *)FTM_BLOCK_3;
                if ((ulMode & PWM_NO_OUTPUT) == 0) {                     // {6}
                    switch (ptrPWM_settings->pwm_reference & ~_TIMER_MODULE_MASK) { // configure appropriate pin for the PWM output signal
                    case 0:                                              // timer 3, channel 0
        #if defined FTM3_0_ON_D
                        _CONFIG_PERIPHERAL(D, 0, (PD_0_FTM3_CH0 | PORT_SRE_FAST | PORT_DSE_HIGH)); // FTM3_CH0 on PD.0 (alt. function 4)
        #else
                        _CONFIG_PERIPHERAL(E, 5, (PE_5_FTM3_CH0 | PORT_SRE_FAST | PORT_DSE_HIGH)); // FTM3_CH0 on PE.5 (alt. function 6)
        #endif
                        break;
                    case 1:                                              // timer 3, channel 1
        #if defined FTM3_1_ON_D
                        _CONFIG_PERIPHERAL(D, 1, (PD_1_FTM3_CH1 | PORT_SRE_FAST | PORT_DSE_HIGH)); // FTM3_CH1 on PD.1 (alt. function 4)
        #else
                        _CONFIG_PERIPHERAL(E, 6, (PE_6_FTM3_CH1 | PORT_SRE_FAST | PORT_DSE_HIGH)); // FTM3_CH1 on PE.6 (alt. function 6)
        #endif
                        break;
                    case 2:                                              // timer 3, channel 2
        #if defined FTM3_2_ON_D
                        _CONFIG_PERIPHERAL(D, 2, (PD_2_FTM3_CH2 | PORT_SRE_FAST | PORT_DSE_HIGH)); // FTM3_CH2 on PD.2 (alt. function 4)
        #else
                        _CONFIG_PERIPHERAL(E, 7, (PE_7_FTM3_CH2 | PORT_SRE_FAST | PORT_DSE_HIGH)); // FTM3_CH2 on PE.7 (alt. function 6)
        #endif
                        break;
                    case 3:                                              // timer 3, channel 3
        #if defined FTM3_3_ON_D
                        _CONFIG_PERIPHERAL(D, 3, (PD_3_FTM3_CH3 | PORT_SRE_FAST | PORT_DSE_HIGH)); // FTM3_CH3 on PD.3 (alt. function 4)
        #else
                        _CONFIG_PERIPHERAL(E, 8, (PE_8_FTM3_CH3 | PORT_SRE_FAST | PORT_DSE_HIGH)); // FTM3_CH3 on PE.8 (alt. function 6)
        #endif
                        break;
                    case 4:                                              // timer 3, channel 4
        #if defined FTM3_4_ON_C
                        _CONFIG_PERIPHERAL(C, 8, (PC_8_FTM3_CH4 | PORT_SRE_FAST | PORT_DSE_HIGH)); // FTM3_CH4 on PC.8 (alt. function 3)
        #else
                        _CONFIG_PERIPHERAL(E, 9, (PE_9_FTM3_CH4 | PORT_SRE_FAST | PORT_DSE_HIGH)); // FTM3_CH4 on PE.9 (alt. function 6)
        #endif
                        break;
                    case 5:                                              // timer 3, channel 5
        #if defined FTM3_5_ON_C
                        _CONFIG_PERIPHERAL(C, 9, (PC_9_FTM3_CH5 | PORT_SRE_FAST | PORT_DSE_HIGH)); // FTM3_CH5 on PC.9 (alt. function 3)
        #else
                        _CONFIG_PERIPHERAL(E, 10, (PE_10_FTM3_CH5 | PORT_SRE_FAST | PORT_DSE_HIGH)); // FTM3_CH5 on PE.10 (alt. function 6)
        #endif
                        break;
                    case 6:                                              // timer 3, channel 6
        #if defined FTM3_6_ON_C
                        _CONFIG_PERIPHERAL(C, 10, (PC_10_FTM3_CH6 | PORT_SRE_FAST | PORT_DSE_HIGH)); // FTM3_CH6 on PC.10 (alt. function 3)
        #else
                        _CONFIG_PERIPHERAL(E, 11, (PE_11_FTM3_CH6 | PORT_SRE_FAST | PORT_DSE_HIGH)); // FTM3_CH6 on PE.11 (alt. function 6)
        #endif
                        break;
                    case 7:                                              // timer 3, channel 7
        #if defined FTM3_7_ON_C
                        _CONFIG_PERIPHERAL(C, 11, (PC_11_FTM3_CH7 | PORT_SRE_FAST | PORT_DSE_HIGH)); // FTM3_CH7 on PC.11 (alt. function 3)
        #else
                        _CONFIG_PERIPHERAL(E, 12, (PE_5_FTM3_CH7 | PORT_SRE_FAST | PORT_DSE_HIGH)); // FTM3_CH7 on PE.12 (alt. function 6)
        #endif
                        break;
                    default:
                        _EXCEPTION("Invalid timer channel!!");
                        return;                                          // invalid channel
                    }
                }
                break;
    #endif
    #if FLEX_TIMERS_AVAILABLE > 4 && defined TPMS_AVAILABLE
            case 4:
                POWER_UP(2, SIM_SCGC2_TPM1);                             // ensure that the TPM module is powered up
                iInterruptID = irq_FTM1_ID;
                iTPM_type = 1;
                ptrFlexTimer = (FLEX_TIMER_MODULE *)FTM_BLOCK_4;
                if ((ulMode & PWM_NO_OUTPUT) == 0) {                     // {6}
                    switch (ptrPWM_settings->pwm_reference & ~_TIMER_MODULE_MASK) { // configure appropriate pin for the PWM output signal
                    case 0:                                              // TPM 1, channel 0
        #if defined TPM1_0_ON_B
                        _CONFIG_PERIPHERAL(B, 0, (PB_0_TPM1_CH0 | PORT_SRE_FAST | PORT_DSE_HIGH)); // TPM1_CH0 on PB.0 (alt. function 6)
        #elif defined TPM1_0_ON_A_HIGH
                        _CONFIG_PERIPHERAL(A, 12, (PA_12_TPM1_CH0 | PORT_SRE_FAST | PORT_DSE_HIGH)); // TPM1_CH0 on PA.12 (alt. function 7)
        #else
                        _CONFIG_PERIPHERAL(A, 8, (PA_8_TPM1_CH0 | PORT_SRE_FAST | PORT_DSE_HIGH)); // TPM1_CH0 on PA.8 (alt. function 6)
        #endif
                        break;
                    case 1:                                              // TPM 1, channel 1
        #if defined TPM1_1_ON_B
                        _CONFIG_PERIPHERAL(B, 1, (PB_1_TPM1_CH1 | PORT_SRE_FAST | PORT_DSE_HIGH)); // TPM1_CH1 on PB.1 (alt. function 6)
        #elif defined TPM1_1_ON_A_HIGH
                        _CONFIG_PERIPHERAL(A, 13, (PA_13_TPM1_CH1 | PORT_SRE_FAST | PORT_DSE_HIGH)); // TPM1_CH1 on PA.13 (alt. function 7)
        #else
                        _CONFIG_PERIPHERAL(A, 9, (PA_9_TPM1_CH1 | PORT_SRE_FAST | PORT_DSE_HIGH)); // TPM1_CH1 on PA.9 (alt. function 6)
        #endif
                        break;
                    default:
                        _EXCEPTION("Invalid timer channel!!");
                        return;                                          // invalid channel
                    }
                }
                break;

            case 5:
                POWER_UP(2, SIM_SCGC2_TPM2);                             // ensure that the TPM module is powered up
                iInterruptID = irq_FTM2_ID;
                ptrFlexTimer = (FLEX_TIMER_MODULE *)FTM_BLOCK_5;
                iTPM_type = 1;
                if ((ulMode & PWM_NO_OUTPUT) == 0) {                     // {6}
                    switch (ptrPWM_settings->pwm_reference & ~_TIMER_MODULE_MASK) { // configure appropriate pin for the PWM output signal
                    case 0:                                              // TPM 2, channel 0
        #if defined TPM2_0_ON_B
                        _CONFIG_PERIPHERAL(B, 18, (PB_18_TPM2_CH0 | PORT_SRE_FAST | PORT_DSE_HIGH)); // TPM2_CH0 on PB.18 (alt. function 6)
        #else
                        _CONFIG_PERIPHERAL(A, 10, (PA_10_TPM2_CH0 | PORT_SRE_FAST | PORT_DSE_HIGH)); // TPM2_CH0 on PA.10 (alt. function 6)
        #endif
                        break;
                    case 1:                                              // TPM 2, channel 1
        #if defined TPM2_1_ON_B
                        _CONFIG_PERIPHERAL(B, 19, (PB_19_TPM2_CH1 | PORT_SRE_FAST | PORT_DSE_HIGH)); // TPM2_CH1 on PB.19 (alt. function 6)
        #else
                        _CONFIG_PERIPHERAL(A, 11, (PA_11_TPM2_CH1 | PORT_SRE_FAST | PORT_DSE_HIGH)); // TPM2_CH1 on PA.11 (alt. function 6)
        #endif
                        break;
                    default:
                        _EXCEPTION("Invalid timer channel!!");
                        return;                                          // invalid channel
                    }
                }
                break;
    #endif
            default:
    #if defined _WINDOWS
                _EXCEPTION("Invalid timer !!");
    #endif
                return;                                                  // invalid FlexTimer
            }
    #if !defined KINETIS_KE
            ptrFlexTimer->FTM_CONF = FTM_DEBUG_BEHAVIOUR;                // set the debugging behaviour (whether the counter runs in debug mode and how the outputs react)
    #endif
            if (PWM_EXTERNAL_CLK == (ulMode & FTM_SC_CLKS_EXT)) {        // if external clock source is to be used program the clock input
    #if FLEX_TIMERS_AVAILABLE > 4 && defined TPMS_AVAILABLE
                if (iTPM_type != 0) {
                    ulMode &= ~(FTM_SC_CLKS_EXT);                        // convert FTM external clock to TPM external clock setting
                    ulMode |= FTM_SC_CLKS_FIX;
        #if defined TPM_CLKIN_1
                    SIM_SOPT9 |= (SIM_SOPT9_TPM1CLKSEL << (ucFlexTimer - 4)); // select TPM_CLKIN1
            #if defined TPMCLKIN1_ON_A
                    _CONFIG_PERIPHERAL(A, 19, (PA_19_TPM_CLKIN1 | PORT_PS_UP_ENABLE)); // TPM_CKLIN1 on PA.19 (alt. function 7)
            #elif defined TPMCLKIN1_ON_B
                    _CONFIG_PERIPHERAL(B, 17, (PB_17_TPM_CLKIN1 | PORT_PS_UP_ENABLE)); // TPM_CKLIN1 on PB.17 (alt. function 7)
            #else
                    _CONFIG_PERIPHERAL(C, 13, (PC_13_TPM_CLKIN1 | PORT_PS_UP_ENABLE)); // TPM_CKLIN1 on PC.13 (alt. function 7)
            #endif
        #else
                    SIM_SOPT9 &= ~(SIM_SOPT9_TPM1CLKSEL << (ucFlexTimer - 4)); // select TPM_CLKIN0
            #if defined TPMCLKIN0_ON_A
                    _CONFIG_PERIPHERAL(A, 18, (PA_18_TPM_CLKIN0 | PORT_PS_UP_ENABLE)); // TPM_CLKIN0 on PA.18 (alt. function 7)
            #elif defined TPMCLKIN0_ON_B
                    _CONFIG_PERIPHERAL(B, 16, (PB_16_TPM_CLKIN0 | PORT_PS_UP_ENABLE)); // TPM_CLKIN0 on PB.16 (alt. function 7)
            #else
                    _CONFIG_PERIPHERAL(C, 12, (PC_12_TPM_CLKIN0 | PORT_PS_UP_ENABLE)); // TPM_CLKIN0 on PC.12 (alt. function 7)
            #endif
        #endif
                }
                else {
    #endif
    #if defined FTM_CLKIN_1
        #if !defined KINETIS_KE
                    SIM_SOPT4 |= (SIM_SOPT4_FTM0CLKSEL << ucChannel);    // select CLKIN1 to FTN
        #endif
        #if defined KINETIS_KL02
                    _CONFIG_PERIPHERAL(B, 6, (PB_6_TPM_CLKIN1 | PORT_PS_UP_ENABLE)); // TPM_CKLIN1 on PB.6 (alt. function 3)
        #elif defined KINETIS_KL04 || defined KINETIS_KL05
                    _CONFIG_PERIPHERAL(B, 17, (PB_17_TPM_CLKIN1 | PORT_PS_UP_ENABLE)); // TPM_CKLIN1 on PB.17 (alt. function 2)
        #else
                    _CONFIG_PERIPHERAL(A, 19, (PA_19_FTM_CLKIN1 | PORT_PS_UP_ENABLE)); // FTM_CKLIN1 on PA.19 (alt. function 4)
        #endif
    #else
        #if !defined KINETIS_KE && !defined KINETIS_KL82
                    SIM_SOPT4 &= ~(SIM_SOPT4_FTM0CLKSEL << ucChannel);   // select CLKIN0 to FTM
        #endif
        #if defined KINETIS_KL02 && defined TPMCLKIN0_ALT
                    _CONFIG_PERIPHERAL(A, 12,  (PA_12_TPM_CLKIN0 | PORT_PS_UP_ENABLE)); // TPM_CLKIN0 on PA.12 (alt. function 2)
        #elif defined KINETIS_KL02 || defined KINETIS_KL04 || defined KINETIS_KL05
                    _CONFIG_PERIPHERAL(A, 1,  (PA_1_TPM_CLKIN0 | PORT_PS_UP_ENABLE)); // TPM_CLKIN0 on PA.1 (alt. function 2)
        #elif defined KINETIS_K66
                    _CONFIG_PERIPHERAL(C, 12, (PC_12_FTM_CLKIN0 | PORT_PS_UP_ENABLE)); // FTM_CLKIN0 on PC.12 (alt. function 4)
        #elif defined KINETIS_KL26
                    _CONFIG_PERIPHERAL(B, 16, (PB_16_FTM_CLKIN0 | PORT_PS_UP_ENABLE)); // FTM_CLKIN0 on PA.18 (alt. function 4)
        #else
                    _CONFIG_PERIPHERAL(A, 18, (PA_18_FTM_CLKIN0 | PORT_PS_UP_ENABLE)); // FTM_CLKIN0 on PA.18 (alt. function 4)
        #endif
    #endif
    #if FLEX_TIMERS_AVAILABLE > 4 && defined TPMS_AVAILABLE
                }
    #endif
            }
    #if FLEX_TIMERS_AVAILABLE > 4 && defined TPMS_AVAILABLE
            if (iTPM_type != 0) {
        #if defined TPM_CLOCKED_FROM_MCGIRCLK
            #if !defined RUN_FROM_LIRC                                   // if the processor is running from the the internal clock we don't change settings here
                MCG_C1 |= (MCG_C1_IRCLKEN | MCG_C1_IREFSTEN);            // enable internal reference clock and allow it to continue running in stop modes
                #if defined USE_FAST_INTERNAL_CLOCK
                MCG_SC = MCG_SC_FCRDIV_1;                                // remove fast IRC divider
                MCG_C2 |= MCG_C2_IRCS;                                   // select fast internal reference clock (4MHz [8MHz for devices with MCG Lite]) as MCGIRCLK
                #else
                MCG_C2 &= ~MCG_C2_IRCS;                                  // select slow internal reference clock (32kHz [2MHz for devices with MCG Lite]) as MCGIRCLK
                #endif
            #endif
                SIM_SOPT2 |= SIM_SOPT2_TPMSRC_MCGIRCLK;                  // use MCGIRCLK as timer clock source
        #elif defined TPM_CLOCKED_FROM_OSCERCLK
                OSC0_CR |= (OSC_CR_ERCLKEN | OSC_CR_EREFSTEN);           // enable the external reference clock and keep it enabled in stop mode
                SIM_SOPT2 |= (SIM_SOPT2_TPMSRC_OSCERCLK);                // use OSCERCLK as timer clock source
        #else                                                            // use MCGPLLCLK, MCGPPL_CLK, IRC48M or USB1PFDCLK with optional divider
            #if defined TPM_CLOCKED_FROM_MCGFFLCLK
                SIM_SOPT2 |= (SIM_SOPT2_PLLFLLSEL_FLL | SIM_SOPT2_TPMSRC_ALT);
            #elif defined TPM_CLOCKED_FROM_IRC48M
                SIM_SOPT2 |= (SIM_SOPT2_PLLFLLSEL_IRC48M | SIM_SOPT2_TPMSRC_ALT);
            #elif defined TPM_CLOCKED_FROM_USB1_PDF
                SIM_SOPT2 |= (SIM_SOPT2_PLLFLLSEL_USB1_PFD_CLK | SIM_SOPT2_TPMSRC_ALT);
            #else                                                        // MCGPLLCLK by default
                SIM_SOPT2 |= (SIM_SOPT2_PLLFLLSEL_PLL | SIM_SOPT2_TPMSRC_ALT);
            #endif
        #endif
            }
    #endif
            if ((ulMode & PWM_POLARITY) != 0) {                          // polarity
                ptrFlexTimer->FTM_channel[ucChannel].FTM_CSC = FTM_CSC_MS_ELS_PWM_LOW_TRUE_PULSES;
            }
            else {
                ptrFlexTimer->FTM_channel[ucChannel].FTM_CSC = FTM_CSC_MS_ELS_PWM_HIGH_TRUE_PULSES;
            }
            //
            ptrFlexTimer->FTM_channel[ucChannel].FTM_CSC = (FTM_CSC_ELSA | FTM_CSC_MSA);
            //
    #if !defined DEVICE_WITHOUT_DMA
            if ((ulMode & PWM_DMA_CHANNEL_ENABLE) != 0) {
                ptrFlexTimer->FTM_channel[ucChannel].FTM_CSC |= (FTM_CSC_DMA | FTM_CSC_CHIE); // enable DMA trigger from this channel (also the interrupt needs to be enabled for the DMA to operate - interrupt is not generated in this configuration)
            }
    #endif
    #if !defined KINETIS_KL && !defined KINETIS_KE
            ptrFlexTimer->FTM_CNTIN = 0;
    #endif
            if ((ulMode & FTM_SC_CPWMS) != 0) {                          // if center-aligned
                ptrFlexTimer->FTM_MOD = (ptrPWM_settings->pwm_frequency / 2); // set the PWM period - valid for all channels of a single timer
                ptrFlexTimer->FTM_channel[ucChannel].FTM_CV = (ptrPWM_settings->pwm_value / 2); // set the duty cycle for the particular channel
            }
            else {
                ptrFlexTimer->FTM_MOD = (ptrPWM_settings->pwm_frequency - 1); // set the PWM period - valid for all channels of a single timer
                ptrFlexTimer->FTM_channel[ucChannel].FTM_CV = ptrPWM_settings->pwm_value; // set the duty cycle for the particular channel            
            }
    #if !defined DEVICE_WITHOUT_DMA                                      // {2}
            if ((ulMode & (PWM_FULL_BUFFER_DMA | PWM_HALF_BUFFER_DMA)) != 0) { // if DMA is being specified
                unsigned long ulDMA_rules = (DMA_DIRECTION_OUTPUT | DMA_HALF_WORDS);
                void *ptrRegister;
                if ((ulMode & PWM_FULL_BUFFER_DMA_AUTO_REPEAT) != 0) {
                    ulDMA_rules |= DMA_AUTOREPEAT;
                }
                if ((ulMode & PWM_HALF_BUFFER_DMA) != 0) {
                    ulDMA_rules |= DMA_HALF_BUFFER_INTERRUPT;
                }
                if ((ulMode & PWM_DMA_CONTROL_FREQUENCY) != 0) {
                    ptrRegister = (void *)&ptrFlexTimer->FTM_MOD;        // each DMA trigger causes a new frequency to be set
                }
                else {
                    ptrRegister = (void *)&ptrFlexTimer->FTM_channel[ucChannel].FTM_CV; // each DMA trigger causes a new PWM value to be set
                }
                fnConfigDMA_buffer(ptrPWM_settings->ucDmaChannel, ptrPWM_settings->ucDmaTriggerSource, ptrPWM_settings->ulPWM_buffer_length, ptrPWM_settings->ptrPWM_Buffer, ptrRegister, ulDMA_rules, ptrPWM_settings->dma_int_handler, ptrPWM_settings->dma_int_priority); // source is the PWM buffer and destination is the PWM mark-space ratio register
                fnDMA_BufferReset(ptrPWM_settings->ucDmaChannel, DMA_BUFFER_START);
            }
    #endif
            ulMode &= PWM_MODE_SETTINGS_MASK;                            // keep just the user's mode settings for the hardware
            if (ptrPWM_settings->int_handler != 0) {                     // {3} if an interrupt handler is specified it is called at each period
                _PWM_TimerHandler[ucFlexTimer] = ptrPWM_settings->int_handler;
                fnEnterInterrupt(iInterruptID, ptrPWM_settings->int_priority, _PWM_TimerInterrupt[ucFlexTimer]);
    #if defined KINETIS_KL
                ulMode |= (FTM_SC_TOIE | FTM_SC_TOF);                    // enable interrupt [FTM_SC_TOF must be written with 1 to clear]
    #else
                ulMode |= (FTM_SC_TOIE);                                 // enable interrupt 
    #endif
            }
            ptrFlexTimer->FTM_SC = ulMode;                               // note that the mode is shared by all channels in the flex timer
    #if defined KINETIS_KE
            _SIM_PER_CHANGE;                                             // update simulator ports
    #endif
        }
#endif
