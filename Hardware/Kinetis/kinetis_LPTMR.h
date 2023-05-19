/***********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher
    
    ---------------------------------------------------------------------
    File:      kinetis_LPTRM.h
    Project:   Single Chip Embedded Internet
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2016
    *********************************************************************

*/

#if defined _LPTMR_CODE


static void (*LPTMR_interrupt_handler)(void) = 0;

static __interrupt void _LPTMR_periodic(void)
{
    LPTMR0_CSR = LPTMR0_CSR;                                             // clear pending interrupt
    uDisable_Interrupt();
        LPTMR_interrupt_handler();                                       // call handling function
    uEnable_Interrupt();
}

static __interrupt void _LPTMR_single(void)
{
    LPTMR0_CSR = 0;                                                      // clear pending interrupt and stop the timer
    POWER_DOWN(5, SIM_SCGC5_LPTIMER);                                    // power down the timer
    uDisable_Interrupt();
        LPTMR_interrupt_handler();                                       // call handling function
    uEnable_Interrupt();
}
#endif



#if defined _LPTMR_CONFIG_CODE
        {
            LPTMR_SETUP *lptmr_setup = (LPTMR_SETUP *)ptrSettings;
            POWER_UP(5, SIM_SCGC5_LPTIMER);                              // ensure that the timer can be accessed
            LPTMR0_CSR = 0;                                              // reset the timer and ensure no pending interrupts
            if (lptmr_setup->mode & LPTMR_STOP) {
                POWER_DOWN(5, SIM_SCGC5_LPTIMER);
                return;
            }
        #if defined LPTMR_CLOCK_LPO                                      // define the low power clock speed for calculations
            LPTMR0_PSR = (LPTMR_PSR_PCS_LPO | LPTMR_PSR_PBYP);
        #elif defined LPTMR_CLOCK_INTERNAL_30_40kHz
            MCG_C2 &= ~MCG_C2_IRCS;                                      // select slow internal reference clock
            LPTMR0_PSR = (LPTMR_PSR_PCS_MCGIRCLK; | LPTMR_PSR_PBYP);
        #elif defined LPTMR_CLOCK_INTERNAL_4MHz
            MCG_C2 |= MCG_C2_IRCS;                                       // select fast internal reference clock
            LPTMR0_PSR = (LPTMR_PSR_PCS_MCGIRCLK | LPTMR_PSR_PBYP);
        #elif defined LPTMR_CLOCK_EXTERNAL_32kHz
            LPTMR0_PSR = (LPTMR_PSR_PCS_ERCLK32K | LPTMR_PSR_PBYP);
        #else                                                            // LPTMR_CLOCK_OSCERCLK
            OSC0_CR |= (OSC_CR_ERCLKEN | OSC_CR_EREFSTEN);               // enable the external reference clock and keep it enabled in stop mode
            #if defined LPTMR_PRESCALE
            LPTMR0_PSR = (LPTMR_PSR_PCS_OSC0ERCLK | (LPTMR_PRESCALE_VALUE << LPTMR_PSR_PRESCALE_SHIFT)); // program prescaler
            #else
            LPTMR0_PSR = (LPTMR_PSR_PCS_OSC0ERCLK | LPTMR_PSR_PBYP);
            #endif
        #endif
            if ((LPTMR_interrupt_handler = lptmr_setup->int_handler) != 0) { // enter the user's interrupt handler
                if (lptmr_setup->mode & LPTMR_PERIODIC) {                // periodic mode
                    fnEnterInterrupt(irq_LPT_ID, lptmr_setup->int_priority, (void (*)(void))_LPTMR_periodic); // enter interrupt handler
                }
                else {                                                   // single-shot mode
                    fnEnterInterrupt(irq_LPT_ID, lptmr_setup->int_priority, (void (*)(void))_LPTMR_single); // enter interrupt handler
                }
                LPTMR0_CSR |= LPTMR_CSR_TIE;                             // enable timer interrupt
            }
            LPTMR0_CMR = lptmr_setup->count_delay;                       // set the match value
            if (lptmr_setup->mode & LPTMR_TRIGGER_ADC0_A) {              // if the LPTMR is to trigger ADC 0 A conversion
                SIM_SOPT7 = (SIM_SOPT7_ADC0TRGSEL_LPTMR0 | SIM_SOPT7_ADC0PRETRGSEL_A | SIM_SOPT7_ADC0ALTTRGEN);
            }
            else if (lptmr_setup->mode & LPTMR_TRIGGER_ADC0_B) {         // if the LPTMR is to trigger ADC 0 B conversion
                SIM_SOPT7 = (SIM_SOPT7_ADC0TRGSEL_LPTMR0 | SIM_SOPT7_ADC0PRETRGSEL_B | SIM_SOPT7_ADC0ALTTRGEN);
            }
            LPTMR0_CSR |= LPTMR_CSR_TEN;                                 // enable the timer
        }

#endif