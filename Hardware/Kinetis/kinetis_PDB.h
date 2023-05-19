/***********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher
    
    ---------------------------------------------------------------------
    File:      kinetis_PDB.h
    Project:   Single Chip Embedded Internet
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2016
    *********************************************************************

*/

#if defined _PDB_CODE

static void (*pdb_interrupt_handler)(void) = 0;

static __interrupt void _pdb_Interrupt(void)
{
    PDB0_SC = (PDB0_SC & ~(PDB_SC_LDOK | PDB_SC_SWTRIG | PDB_SC_PDBIF | PDB_SC_PDBIF)); // clear the interrupt flag
    if (pdb_interrupt_handler != 0) {
        uDisable_Interrupt();
            pdb_interrupt_handler();                                     // call handling function
        uEnable_Interrupt();
    }
}
#endif



#if defined _PDB_CONFIG_CODE
        {
            PDB_SETUP *ptr_pdb_setup = (PDB_SETUP *)ptrSettings;
            unsigned long ulMode = ((ptr_pdb_setup->prescaler >> 4) | (((ptr_pdb_setup->prescaler << 12) & PDB_SC_PRESCALER_128) | (ptr_pdb_setup->pdb_trigger << 8)) | PDB_SC_MULT_1 | PDB_SC_LDMOD_IMM | PDB_SC_PDBEN);
            POWER_UP(6, SIM_SCGC6_PDB);                                  // power up the programmable delay block

            PDB0_MOD = ptr_pdb_setup->period;                            // set cycle period - warning: note that the value is not read back from the register until PDB_SC_LDOK has been set

            PDB0_IDLY = ptr_pdb_setup->int_match;                        // enter the interrupt time within the cycle
    #if defined SUPPORT_ADC
            PDB0_CH0DLY0 = ptr_pdb_setup->ch0_delay_0;                   // ADC0 channel A delay
            PDB0_CH0DLY1 = ptr_pdb_setup->ch0_delay_1;                   // ADC1 channel B delay
        #if ADC_CONTROLLERS > 1
            PDB0_CH1DLY0 = ptr_pdb_setup->ch1_delay_0;                   // ADC1 channel A delay
            PDB0_CH1DLY1 = ptr_pdb_setup->ch1_delay_1;                   // ADC1 channel B delay
        #endif
        #if ADC_CONTROLLERS > 2
            PDB0_CH2DLY0 = ptr_pdb_setup->ch2_delay_0;                   // ADC2 channel A delay
            PDB0_CH2DLY1 = ptr_pdb_setup->ch2_delay_1;                   // ADC2 channel B delay
        #endif
        #if ADC_CONTROLLERS > 3
            PDB0_CH3DLY0 = ptr_pdb_setup->ch3_delay_0;                   // ADC3 channel A delay
            PDB0_CH3DLY1 = ptr_pdb_setup->ch3_delay_1;                   // ADC3 channel B delay
        #endif
            if (ptr_pdb_setup->pdb_mode & (PDB_TRIGGER_ADC0_A | PDB_TRIGGER_ADC0_B)) {
                PDB0_CH0C1 |= ((ptr_pdb_setup->pdb_mode & (PDB_TRIGGER_ADC0_A | PDB_TRIGGER_ADC0_B)) | ((ptr_pdb_setup->pdb_mode & (PDB_TRIGGER_ADC0_A | PDB_TRIGGER_ADC0_B)) << 8)); // enable pre-trigger from PDB channel 0 to ADC 0 [PDB_C1_EN and PDB_C1_TOS]
            }
        #if ADC_CONTROLLERS > 1
            if (ptr_pdb_setup->pdb_mode & (PDB_TRIGGER_ADC1_A | PDB_TRIGGER_ADC1_B)) {
                PDB0_CH1C1 |= (((ptr_pdb_setup->pdb_mode & (PDB_TRIGGER_ADC1_A | PDB_TRIGGER_ADC1_B)) >> 2) | ((ptr_pdb_setup->pdb_mode & (PDB_TRIGGER_ADC1_A | PDB_TRIGGER_ADC1_B)) << 6));// enable pre-trigger from PDB channel 0 to ADC 1 [PDB_C1_EN and PDB_C1_TOS]
            }
        #endif
        #if ADC_CONTROLLERS > 2
            if (ptr_pdb_setup->pdb_mode & (PDB_TRIGGER_ADC2_A | PDB_TRIGGER_ADC2_B)) {
                PDB0_CH2C1 |= (((ptr_pdb_setup->pdb_mode & (PDB_TRIGGER_ADC2_A | PDB_TRIGGER_ADC2_B)) >> 4) | ((ptr_pdb_setup->pdb_mode & (PDB_TRIGGER_ADC2_A | PDB_TRIGGER_ADC2_B)) << 4));// enable pre-trigger from PDB channel 0 to ADC 2 [PDB_C1_EN and PDB_C1_TOS]
            }
        #endif
        #if ADC_CONTROLLERS > 3
            if (ptr_pdb_setup->pdb_mode & (PDB_TRIGGER_ADC3_A | PDB_TRIGGER_ADC3_B)) {
                PDB0_CH3C1 |= (((ptr_pdb_setup->pdb_mode & (PDB_TRIGGER_ADC3_A | PDB_TRIGGER_ADC3_B)) >> 6) | ((ptr_pdb_setup->pdb_mode & (PDB_TRIGGER_ADC3_A | PDB_TRIGGER_ADC3_B)) << 2));// enable pre-trigger from PDB channel 0 to ADC 3 [PDB_C1_EN and PDB_C1_TOS]
            }
        #endif
    #endif
            PDB0_CH0S = 0;                                               // reset error status
            if (ptr_pdb_setup->pdb_mode & PDB_PERIODIC_DMA) {
                ulMode |= (PDB_SC_DMAEN | PDB_SC_CONT);                  // continuous mode with DMA
            }
            else {
                if (ptr_pdb_setup->pdb_mode & PDB_PERIODIC_INTERRUPT) {
                    ulMode |= PDB_SC_CONT;                               // continuous mode with optional interrupt
                }
                else {
                    ulMode |= PDB_SC_ONE_SHOT;                           // single cycle with optional interrupt
                }
                if (ptr_pdb_setup->int_handler != 0) {                   // if an interrupt handler is defined
                    pdb_interrupt_handler = ptr_pdb_setup->int_handler;  // enter the user's handler
                    fnEnterInterrupt(irq_PDB_ID, ptr_pdb_setup->int_priority, (void (*)(void))_pdb_Interrupt); // enter interrupt handler
                    ulMode |= PDB_SC_PDBIE;                              // enable interrupt
                }
            }
    #if defined SUPPORT_DAC && !defined KINETIS_KE                       // {61}
            if (ptr_pdb_setup->pdb_mode & PDB_TRIGGER_DAC0) {
                if (ptr_pdb_setup->dac0_delay_0 == 0) {
                    PDB0_DACINT0 = 0;
                }
                else {
                    PDB0_DACINT0 = (ptr_pdb_setup->dac0_delay_0 - 1);    // minus 1 to get the match value
                }
                PDB0_DACINTC0 = PDB_DACINTC0_TOE;                        // enable DAC0 interval trigger
            }
        #if DAC_CONTROLLERS > 1
            if (ptr_pdb_setup->pdb_mode & PDB_TRIGGER_DAC1) {
                if (ptr_pdb_setup->dac1_delay_0 == 0) {
                    PDB0_DACINT1 = 0;
                }
                else {
                    PDB0_DACINT1 = (ptr_pdb_setup->dac1_delay_0 - 1);    // minus 1 to get the match value
                }
                PDB0_DACINTC1 = PDB_DACINTC0_TOE;                        // enable DAC1 interval trigger
            }
        #endif
    #endif
            PDB0_SC = (ulMode | PDB_SC_LDOK);                            // configure operation
            if (ptr_pdb_setup->pdb_trigger == PDB_TRIGGER_SW) {
                PDB0_SC = (ulMode | PDB_SC_SWTRIG);                      // software trigger to start
            }
        }
#endif


