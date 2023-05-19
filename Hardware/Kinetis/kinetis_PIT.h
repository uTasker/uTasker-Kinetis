/***********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher
    
    ---------------------------------------------------------------------
    File:      kinetis_PIT.h
    Project:   Single Chip Embedded Internet
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2016
    *********************************************************************
    09.03.2012 Disable PIT before configuring load value so that it is retriggerable {4}
    04.02.2014 Power up PIT module after setting mode to protect from power down from another PIT channel interrupt - also protect mode variable changes from interrupts {68}
    21.04.2014 Add KL PIT support                                        {76}
    21.07.2014 Add PIT errate e2682 workaround for some older devices    {97}
    04.09.2015 Exit KL PIT interrupt if the PIT has been powered down    {1}
    22.11.2015 Move PIT power up to avoid the need to apply errate e7914 workaround {2}

*/

#if defined _PIT_CODE

/* =================================================================== */
/*                 local function prototype declarations               */
/* =================================================================== */

static void fnDisablePIT(int iPIT);

/* =================================================================== */
/*                      local variable definitions                     */
/* =================================================================== */

static void (*pit_interrupt_handler[PITS_AVAILABLE])(void) = {0};        // user callback for each PIT
static unsigned char ucPITmodes = 0;                                     // PIT mode containing flags for all PITs

/* =================================================================== */
/*                        PIT Interrupt Handlers                       */
/* =================================================================== */

    #if defined KINETIS_KL
// KL device PIT have a single interrupt which is shared by all PIT channels
//
static __interrupt void _PIT_Interrupt(void)
{
    int iChannel = 0;
    KINETIS_PIT_CTL *ptrCtl = (KINETIS_PIT_CTL *)PIT_CTL_ADD;            // set PIT control struture pointer to first PIT
    do {
        if ((ptrCtl->PIT_TCTRL & (PIT_TCTRL_TIE | PIT_TCTRL_TEN)) == (PIT_TCTRL_TIE | PIT_TCTRL_TEN)) { // if the channel and its interrupt are enabled
            WRITE_ONE_TO_CLEAR(ptrCtl->PIT_TFLG, PIT_TFLG_TIF);          // clear pending interrupts
            if ((ucPITmodes & (PIT_PERIODIC << (iChannel * 2))) == 0) {  // if not periodic mode (single-shot usage)
                fnDisablePIT(iChannel);                                  // stop PIT operation and power down when no other activity
            }
            uDisable_Interrupt();
                pit_interrupt_handler[iChannel]();                       // call handling function
            uEnable_Interrupt();
            if (IS_POWERED_UP(6, SIM_SCGC6_PIT) == 0) {                  // if the PIT module has been powered down we return rather than checking further channels
                return;
            }
        }
        ptrCtl++;                                                        // move to next PIT control structure
    } while (++iChannel < PITS_AVAILABLE);                               // for each PIT channel that may be interrupting
}
    #else
// Common interrupt handler for all PITs
//
static void _PIT_Handler(int iPIT)
{
    KINETIS_PIT_CTL *ptrCtl = (KINETIS_PIT_CTL *)PIT_CTL_ADD;
    ptrCtl += iPIT;
    WRITE_ONE_TO_CLEAR(ptrCtl->PIT_TFLG, PIT_TFLG_TIF);                  // clear pending interrupts
        #if defined ERRATA_ID_2682                                       // {97}
    (void)(ptrCtl->PIT_TCTRL);                                           // access and PIT register after clearing the interrupt flag to allow subsequent interrupts to operate
        #endif
    if ((ucPITmodes & (PIT_PERIODIC << (iPIT * 2))) == 0) {              // if not periodic mode (single-shot usage)
        fnDisablePIT(iPIT);                                              // stop PIT operation and power down when no other activity
    }
    uDisable_Interrupt();
        pit_interrupt_handler[iPIT]();                                   // call handling function
    uEnable_Interrupt();
}

static __interrupt void _PIT0_Interrupt(void)
{
    _PIT_Handler(0);
}

static __interrupt void _PIT1_Interrupt(void)
{
    _PIT_Handler(1);
}

        #if PITS_AVAILABLE > 2
static __interrupt void _PIT2_Interrupt(void)
{
    _PIT_Handler(2);
}

static __interrupt void _PIT3_Interrupt(void)
{
    _PIT_Handler(3);
}
        #endif
    #endif

// Stop PIT operation and power down when no other activity
//
static void fnDisablePIT(int iPIT)                                                  
{
    KINETIS_PIT_CTL *ptrCtl = (KINETIS_PIT_CTL *)PIT_CTL_ADD;
    ptrCtl += iPIT;
    ptrCtl->PIT_TCTRL = 0;                                               // disable the individual PIT
    uDisable_Interrupt();                                                // {68} protect the mode variable during modification
        ucPITmodes &= ~((PIT_SINGLE_SHOT | PIT_PERIODIC) << (iPIT * 2)); // clear the PIT's mode flags
    #if !defined PIT_TIMER_USED_BY_PERFORMANCE_MONITOR                   // don't power the PITs down if one is being used for performance monitoring
        if (ucPITmodes == 0) {                                           // if not PITs are in use power down the PIT module
            POWER_UP(6, SIM_SCGC6_PIT);                                  // {68} ensure that the module is powered up for the next operation
            PIT_MCR = PIT_MCR_MDIS;                                      // disable clocks to module since no more timers are active
            POWER_DOWN(6, SIM_SCGC6_PIT);                                // power down the PIT module
        }
    #endif
    uEnable_Interrupt();
}
#endif



#if defined _PIT_CONFIG_CODE
// PIT_INTERRUPT case in fnConfigureInterrupt()
//
    {
        PIT_SETUP *PIT_settings = (PIT_SETUP *)ptrSettings;
        KINETIS_PIT_CTL *ptrCtl = (KINETIS_PIT_CTL *)PIT_CTL_ADD;
        unsigned long ulCommand;
        if (PIT_settings->ucPIT >= PITS_AVAILABLE) {
            return;
        }
        if ((PIT_settings->mode & PIT_STOP) != 0) {
            fnDisablePIT(PIT_settings->ucPIT);                           // stop PIT operation and power down when no other activity
            return;
        }
        POWER_UP(6, SIM_SCGC6_PIT);                                      // {2} ensure the PIT module is powered up
        pit_interrupt_handler[PIT_settings->ucPIT] = PIT_settings->int_handler; // enter the user handler
        uDisable_Interrupt();                                            // {68} protect the mode variable during modification
            ucPITmodes = ((ucPITmodes & ~((PIT_SINGLE_SHOT | PIT_PERIODIC) << (PIT_settings->ucPIT * 2))) | ((PIT_settings->mode & (PIT_SINGLE_SHOT | PIT_PERIODIC)) << (PIT_settings->ucPIT * 2))); // {2} [the variable protects from power downs from this point]
        uEnable_Interrupt();
        ptrCtl += PIT_settings->ucPIT;                                   // set the PIT to be configured
      //POWER_UP(6, SIM_SCGC6_PIT);                                      // {68} ensure the PIT module is powered up
  //#if defined ERRATA_ID_7914
      //(void)PIT_MCR;                                                   // dummy read of PIT_MCR to guaranty a minimum delay of two bus cycles after enabling the clock gate and not losing next write
  //#endif
        PIT_MCR = 0;                                                     // ensure the PIT module is clocked
        if (PIT_settings->int_handler != 0) {                            // if an interrupt is required
    #if defined KINETIS_KL                                               // {76} KL devices have a single interrupt from the PIT channels
            fnEnterInterrupt(irq_PIT_ID, PIT_settings->int_priority, _PIT_Interrupt); // ensure that the handler for the PIT module is entered
    #else
            switch (PIT_settings->ucPIT) {
            case 0:
                fnEnterInterrupt(irq_PIT0_ID, PIT_settings->int_priority, _PIT0_Interrupt); // ensure that the handler for this PIT is entered
                break;
            case 1:
                fnEnterInterrupt(irq_PIT1_ID, PIT_settings->int_priority, _PIT1_Interrupt); // ensure that the handler for this PIT is entered
                break;
         #if PITS_AVAILABLE > 2
            case 2:
                fnEnterInterrupt(irq_PIT2_ID, PIT_settings->int_priority, _PIT2_Interrupt); // ensure that the handler for this PIT is entered
                break;
            case 3:
                fnEnterInterrupt(irq_PIT3_ID, PIT_settings->int_priority, _PIT3_Interrupt); // ensure that the handler for this PIT is entered
                break;
        #endif
            }
    #endif
            ulCommand = (PIT_TCTRL_TEN | PIT_TCTRL_TIE);
        }
        else {
            ulCommand = (PIT_TCTRL_TEN);                                 // no interrupt used
        }
        if ((PIT_settings->mode & (PIT_SINGLE_SHOT | PIT_RETRIGGER)) != 0) { // single shot always behaves as retriggerable - periodic change at next timeout by default but can be forced with PIT_RETRIGGER
            ptrCtl->PIT_TCTRL = 0;                                       // {4} disable PIT so that it can be retriggered if needed
        }
        ptrCtl->PIT_LDVAL = PIT_settings->count_delay;                   // load interval value
        ptrCtl->PIT_TFLG  = PIT_TFLG_TIF;                                // clear pending interrupts
        if (PIT_settings->mode & PIT_TRIGGER_ADC0_A) {                   // if the PIT output TIF is to trigger ADC0 conversion on channel A
    #if defined KINETIS_KE
            SIM_SOPT0 |= SIM_SOPT_ADHWT_PIT0;                            // trigger ADC on PIT0 overflow
    #else
            SIM_SOPT7 = ((SIM_SOPT7_ADC0TRGSEL_PIT0 + PIT_settings->ucPIT) | SIM_SOPT7_ADC0PRETRGSEL_A | SIM_SOPT7_ADC0ALTTRGEN); // trigger ADC0 channel A
    #endif
        }
        else if (PIT_settings->mode & PIT_TRIGGER_ADC0_B) {              // if the PIT output TIF is to trigger ADC0 conversion on channel B
    #if defined KINETIS_KE
            SIM_SOPT0 |= SIM_SOPT_ADHWT_PIT0;                            // trigger ADC on PIT0 overflow
    #else
            SIM_SOPT7 = ((SIM_SOPT7_ADC0TRGSEL_PIT0 + PIT_settings->ucPIT) | SIM_SOPT7_ADC0PRETRGSEL_B | SIM_SOPT7_ADC0ALTTRGEN); // trigger ADC0 channel B
    #endif
        }
        ptrCtl->PIT_TCTRL = ulCommand;                                   // start PIT with interrupt enabled, when handler defined
    #if defined _WINDOWS
        ptrCtl->PIT_CVAL = ptrCtl->PIT_LDVAL;                            // load current count value with the load value
    #endif
    }
#endif
