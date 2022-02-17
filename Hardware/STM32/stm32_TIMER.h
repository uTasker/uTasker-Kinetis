/**********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher

    ---------------------------------------------------------------------
    File:      stm32_TIMER.h
    Project:   Single Chip Embedded Internet
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2018
    *********************************************************************
    17.10.2017 Add timers 9 and 12                                       {1}
    17.10.2017 Correct prescaler adjustment for PCLK2                    {2}

*/

#if defined _TIMER_CODE
/* =================================================================== */
/*                           include files                             */
/* =================================================================== */

/* =================================================================== */
/*                          local definitions                          */
/* =================================================================== */

#define TIMER_2_INTREF     0
#define TIMER_3_INTREF     1
#define TIMER_4_INTREF     2
#define TIMER_5_INTREF     3
#define TIMER_9_INTREF     4
#define TIMER_10_INTREF    5
#define TIMER_11_INTREF    6
#define TIMER_12_INTREF    7
#define TIMER_13_INTREF    8
#define TIMER_14_INTREF    9

/* =================================================================== */
/*                       local structure definitions                   */
/* =================================================================== */

/* =================================================================== */
/*                global function prototype declarations               */
/* =================================================================== */

/* =================================================================== */
/*                 local function prototype declarations               */
/* =================================================================== */

/* =================================================================== */
/*                             constants                               */
/* =================================================================== */

/* =================================================================== */
/*                      local variable definitions                     */
/* =================================================================== */

    #if defined TIMER_10_AVAILABLE                                       // {31}
static void(*_timer_handler[10])(void) = { 0 };                          // timers 2, 3, 4, 5, 9, 10, 11, 12, 13, 14
    #else
static void(*_timer_handler[4])(void) = { 0 };                           // timers 2, 3, 4, 5
    #endif

/* =================================================================== */
/*                     global variable definitions                     */
/* =================================================================== */

/* =================================================================== */
/*                         timer interrupts                            */
/* =================================================================== */


static __interrupt void timer_2_irq(void)
{
    TIM2_SR = 0;                                                         // clear the interrupt
    if ((TIM2_CR1 & TIM_CR1_CEN) == 0) {                                 // in single-shot mode the timer will be automatically disabled
        POWER_DOWN(APB1, (RCC_APB1ENR_TIM2EN));                          // power down the timer
    }
    if (_timer_handler[TIMER_2_INTREF] != 0) {
        uDisable_Interrupt();                                            // ensure interrupts remain blocked during subroutines
            _timer_handler[TIMER_2_INTREF]();                            // call the user timer interrupt handler
        uEnable_Interrupt();                                             // release
    }
}

static __interrupt void timer_3_irq(void)
{
    TIM3_SR = 0;                                                         // clear the interrupt
    if ((TIM3_CR1 & TIM_CR1_CEN) == 0) {                                 // in single-shot mode the timer will be automatically disabled
        POWER_DOWN(APB1, (RCC_APB1ENR_TIM3EN));                          // power down the timer
    }
    if (_timer_handler[TIMER_3_INTREF] != 0) {
        uDisable_Interrupt();                                            // ensure interrupts remain blocked during subroutines
            _timer_handler[TIMER_3_INTREF]();                            // call the user timer interrupt handler
        uEnable_Interrupt();                                             // release
    }
}

static __interrupt void timer_4_irq(void)
{
    TIM4_SR = 0;                                                         // clear the interrupt
    if ((TIM4_CR1 & TIM_CR1_CEN) == 0) {                                 // in single-shot mode the timer will be automatically disabled
        POWER_DOWN(APB1, (RCC_APB1ENR_TIM4EN));                          // power down the timer
    }
    if (_timer_handler[TIMER_4_INTREF] != 0) {
        uDisable_Interrupt();                                            // ensure interrupts remain blocked during subroutines
            _timer_handler[TIMER_4_INTREF]();                            // call the user timer interrupt handler
        uEnable_Interrupt();                                             // release
    }
}  

static __interrupt void timer_5_irq(void)
{
    TIM5_SR = 0;                                                         // clear the interrupt
    if ((TIM5_CR1 & TIM_CR1_CEN) == 0) {                                 // in single-shot mode the timer will be automatically disabled
        POWER_DOWN(APB1, (RCC_APB1ENR_TIM5EN));                          // power down the timer
    }
    if (_timer_handler[TIMER_5_INTREF] != 0) {
        uDisable_Interrupt();                                            // ensure interrupts remain blocked during subroutines
            _timer_handler[TIMER_5_INTREF]();                            // call the user timer interrupt handler
        uEnable_Interrupt();                                             // release
    }
} 

    #if defined TIMER_10_AVAILABLE                                       // {31}
        #if defined TIMER_9_AVAILABLE                                    // {1}
static __interrupt void timer_9_irq(void)
{
    TIM9_SR = 0;                                                         // clear the interrupt
    if ((TIM9_CR1 & TIM_CR1_CEN) == 0) {                                 // in single-shot mode the timer will be automatically disabled
        POWER_DOWN(APB2, (RCC_APB2ENR_TIM9EN));                          // power down the timer
    }
    if (_timer_handler[TIMER_9_INTREF] != 0) {
        uDisable_Interrupt();                                            // ensure interrupts remain blocked during subroutines
            _timer_handler[TIMER_9_INTREF]();                            // call the user timer interrupt handler
        uEnable_Interrupt();                                             // release
    }
}
        #endif

static __interrupt void timer_10_irq(void)
{
    TIM10_SR = 0;                                                        // clear the interrupt
    if ((TIM10_CR1 & TIM_CR1_CEN) == 0) {                                // in single-shot mode the timer will be automatically disabled
        POWER_DOWN(APB2, (RCC_APB2ENR_TIM10EN));                         // power down the timer
    }
    if (_timer_handler[TIMER_10_INTREF] != 0) {
        uDisable_Interrupt();                                            // ensure interrupts remain blocked during subroutines
            _timer_handler[TIMER_10_INTREF]();                           // call the user timer interrupt handler
        uEnable_Interrupt();                                             // release
    }
}

static __interrupt void timer_11_irq(void)
{
    TIM11_SR = 0;                                                        // clear the interrupt
    if ((TIM11_CR1 & TIM_CR1_CEN) == 0) {                                // in single-shot mode the timer will be automatically disabled
        POWER_DOWN(APB2, (RCC_APB2ENR_TIM11EN));                         // power down the timer
    }
    if (_timer_handler[TIMER_11_INTREF] != 0) {
        uDisable_Interrupt();                                            // ensure interrupts remain blocked during subroutines
            _timer_handler[TIMER_11_INTREF]();                           // call the user timer interrupt handler
        uEnable_Interrupt();                                             // release
    }
}

#if defined TIMER_12_AVAILABLE
static __interrupt void timer_12_irq(void)                               // {1}
{
    TIM12_SR = 0;                                                        // clear the interrupt
    if ((TIM12_CR1 & TIM_CR1_CEN) == 0) {                                // in single-shot mode the timer will be automatically disabled
        POWER_DOWN(APB1, (RCC_APB1ENR_TIM12EN));                         // power down the timer
    }
    if (_timer_handler[TIMER_12_INTREF] != 0) {
        uDisable_Interrupt();                                            // ensure interrupts remain blocked during subroutines
            _timer_handler[TIMER_12_INTREF]();                           // call the user timer interrupt handler
        uEnable_Interrupt();                                             // release
    }
}
#endif

#if defined TIMER_13_AVAILABLE
static __interrupt void timer_13_irq(void)
{
    TIM13_SR = 0;                                                        // clear the interrupt
    if ((TIM13_CR1 & TIM_CR1_CEN) == 0) {                                // in single-shot mode the timer will be automatically disabled
        POWER_DOWN(APB1, (RCC_APB1ENR_TIM13EN));                         // power down the timer
    }
    if (_timer_handler[TIMER_13_INTREF] != 0) {
        uDisable_Interrupt();                                            // ensure interrupts remain blocked during subroutines
            _timer_handler[TIMER_13_INTREF]();                           // call the user timer interrupt handler
        uEnable_Interrupt();                                             // release
    }
}
#endif

#if defined TIMER_14_AVAILABLE
static __interrupt void timer_14_irq(void)
{
    TIM14_SR = 0;                                                        // clear the interrupt
    if ((TIM14_CR1 & TIM_CR1_CEN) == 0) {                                // in single-shot mode the timer will be automatically disabled
        POWER_DOWN(APB1, (RCC_APB1ENR_TIM14EN));                         // power down the timer
    }
    if (_timer_handler[TIMER_14_INTREF] != 0) {
        uDisable_Interrupt();                                            // ensure interrupts remain blocked during subroutines
            _timer_handler[TIMER_14_INTREF]();                           // call the user timer interrupt handler
        uEnable_Interrupt();                                             // release
    }
}
#endif
    #endif
#endif

// Timer configuration
//
#if defined _TIMER_CONFIG_CODE
        {
            TIMER_INTERRUPT_SETUP *ptrTimerSetup = (TIMER_INTERRUPT_SETUP *)ptrSettings;
            TIM2_3_4_5_REGS *ptrTimer;
            unsigned long ulCounterMatch = ptrTimerSetup->timer_value;
            int iClock2 = 0;                                             // assume APB1 clock
            int iTimerIntID;
            void (*timer_irq)(void);
            int iInterruptReference = (ptrTimerSetup->timer_reference - 2); // valid for timers 2,3,4,5
            switch (ptrTimerSetup->timer_reference)  {
            case 2:                                                      // clocked by APB1
                if ((ptrTimerSetup->timer_mode & TIMER_STOP_TIMER) != 0) { // {26}
                    POWER_DOWN(APB1, (RCC_APB1ENR_TIM2EN));              // power down the timer
                    return;
                }
                POWER_UP(APB1, (RCC_APB1ENR_TIM2EN));                    // power up the timer module
                ptrTimer = (TIM2_3_4_5_REGS *)TIM2_BLOCK;
                iTimerIntID = irq_TIM2_ID;
                timer_irq = timer_2_irq;
                break;
            case 3:                                                      // clocked by APB1
                if ((ptrTimerSetup->timer_mode & TIMER_STOP_TIMER) != 0) { // {26}
                    POWER_DOWN(APB1, (RCC_APB1ENR_TIM3EN));              // power down the timer
                    return;
                }
                POWER_UP(APB1, (RCC_APB1ENR_TIM3EN));                    // power up the timer module
                ptrTimer = (TIM2_3_4_5_REGS *)TIM3_BLOCK;
                iTimerIntID = irq_TIM3_ID;
                timer_irq = timer_3_irq;
                break;
            case 4:                                                      // clocked by APB1
                if ((ptrTimerSetup->timer_mode & TIMER_STOP_TIMER) != 0) { // {26}
                    POWER_DOWN(APB1, (RCC_APB1ENR_TIM4EN));              // power down the timer
                    return;
                }
                POWER_UP(APB1, (RCC_APB1ENR_TIM4EN));                    // power up the timer module
                ptrTimer = (TIM2_3_4_5_REGS *)TIM4_BLOCK;
                iTimerIntID = irq_TIM4_ID;
                timer_irq = timer_4_irq;
                break;
            case 5:                                                      // clocked by APB1
                if ((ptrTimerSetup->timer_mode & TIMER_STOP_TIMER) != 0) { // {26}
                    POWER_DOWN(APB1, (RCC_APB1ENR_TIM5EN));              // power down the timer
                    return;
                }
                POWER_UP(APB1, (RCC_APB1ENR_TIM5EN));                    // power up the timer module
                ptrTimer = (TIM2_3_4_5_REGS *)TIM5_BLOCK;
                iTimerIntID = irq_TIM5_ID;
                timer_irq = timer_5_irq;
                break;
    #if defined TIMER_9_AVAILABLE                                        // {1}
            case 9:                                                      // clocked by APB2
                if ((ptrTimerSetup->timer_mode & TIMER_STOP_TIMER) != 0) {
                    POWER_DOWN(APB2, (RCC_APB2ENR_TIM9EN));              // power down the timer
                    return;
                }
                POWER_UP(APB2, (RCC_APB2ENR_TIM9EN));                    // power up the timer module
                ptrTimer = (TIM2_3_4_5_REGS *)TIM9_BLOCK;                // use as if general purpose 2..5 type since basic functions are compatible
                iTimerIntID = irq_TIM1_BRK_TIM9_ID;
                timer_irq = timer_9_irq;
                iInterruptReference = TIMER_9_INTREF;
                iClock2 = 1;                                             // APB2 clock
                break;
    #endif
    #if defined TIMER_10_AVAILABLE                                       // {31}
            case 10:                                                      // clocked by APB2
                if ((ptrTimerSetup->timer_mode & TIMER_STOP_TIMER) != 0) {
                    POWER_DOWN(APB2, (RCC_APB2ENR_TIM10EN));             // power down the timer
                    return;
                }
                POWER_UP(APB2, (RCC_APB2ENR_TIM10EN));                   // power up the timer module
                ptrTimer = (TIM2_3_4_5_REGS *)TIM10_BLOCK;               // use as if general purpose 2..5 type since basic functions are compatible
                iTimerIntID = irq_TIM1_UP_TIM10_ID;
                timer_irq = timer_10_irq;
                iInterruptReference = TIMER_10_INTREF;
                iClock2 = 1;                                             // APB2 clock
                break;
    #endif
    #if defined TIMER_11_AVAILABLE
            case 11:                                                     // clocked by APB2
                if ((ptrTimerSetup->timer_mode & TIMER_STOP_TIMER) != 0) {
                    POWER_DOWN(APB2, (RCC_APB2ENR_TIM11EN));             // power down the timer
                    return;
                }
                POWER_UP(APB2, (RCC_APB2ENR_TIM11EN));                   // power up the timer module
                ptrTimer = (TIM2_3_4_5_REGS *)TIM11_BLOCK;               // use as if general purpose 2..5 type since basic functions are compatible
                iTimerIntID = irq_TIM1_TRG_COM_TIM11_ID;
                timer_irq = timer_11_irq;
                iInterruptReference = TIMER_11_INTREF;
                iClock2 = 1;                                             // APB2 clock
                break;
    #endif
    #if defined TIMER_12_AVAILABLE                                       // {1}
            case 12:                                                     // clocked by APB1
                if ((ptrTimerSetup->timer_mode & TIMER_STOP_TIMER) != 0) {
                    POWER_DOWN(APB1, (RCC_APB1ENR_TIM12EN));             // power down the timer
                    return;
                }
                POWER_UP(APB1, (RCC_APB1ENR_TIM12EN));                   // power up the timer module
                ptrTimer = (TIM2_3_4_5_REGS *)TIM12_BLOCK;               // use as if general purpose 2..5 type since basic functions are compatible
                iTimerIntID = irq_TIM8_BRK_TIM12_ID;
                timer_irq = timer_12_irq;
                iInterruptReference = TIMER_12_INTREF;
                break;
    #endif
    #if defined TIMER_13_AVAILABLE
            case 13:                                                     // clocked by APB1
                if ((ptrTimerSetup->timer_mode & TIMER_STOP_TIMER) != 0) {
                    POWER_DOWN(APB1, (RCC_APB1ENR_TIM13EN));             // power down the timer
                    return;
                }
                POWER_UP(APB1, (RCC_APB1ENR_TIM13EN));                   // power up the timer module
                ptrTimer = (TIM2_3_4_5_REGS *)TIM13_BLOCK;               // use as if general purpose 2..5 type since basic functions are compatible
                iTimerIntID = irq_TIM8_UP_TIM13_ID;
                timer_irq = timer_13_irq;
                iInterruptReference = TIMER_13_INTREF;
                break;
    #endif
    #if defined TIMER_14_AVAILABLE
            case 14:                                                     // clocked by APB1
                if ((ptrTimerSetup->timer_mode & TIMER_STOP_TIMER) != 0) { // {26}
                    POWER_DOWN(APB1, (RCC_APB1ENR_TIM14EN));             // power down the timer
                    return;
                }
                POWER_UP(APB1, (RCC_APB1ENR_TIM14EN));                   // power up the timer module
                ptrTimer = (TIM2_3_4_5_REGS *)TIM14_BLOCK;               // use as if general purpose 2..5 type since basic functions are compatible
                iTimerIntID = irq_TIM8_TRG_COM_TIM14_ID;
                timer_irq = timer_14_irq;
                iInterruptReference = TIMER_14_INTREF;
                break;
    #endif
            default:
                _EXCEPTION("Attempt to use invalid timer!!");
                return;                                                  // invalid timer
            }
            if ((ptrTimerSetup->timer_mode & (TIMER_US_VALUE | TIMER_MS_VALUE)) != 0) { // the value passed is not a natural value
                if ((ptrTimerSetup->timer_mode & TIMER_US_VALUE) != 0) { // set prescaler to generate us count pulses
                    if (iClock2 != 0) {                                  // APB2 clock
                        ptrTimer->TIM_PSC = ((PCLK2 * 2)/1000000 - 1);
                    }
                    else {                                               // APB1 clock
                        ptrTimer->TIM_PSC = ((PCLK1 * 2)/1000000 - 1);
                    }
                }
                else {                                                   // set prescaler to generate ms count pulses
                    if (iClock2 != 0) {                                  // APB2 clock
    #if (((PCLK2 * 2)/1000) - 1 ) > (0xffff)                             // if the clock is too fast to generate 1ms use /2
                        ptrTimer->TIM_PSC = ((((PCLK2 * 2)/1000)/2) - 1);
                        ulCounterMatch *= 2;                             // compensation since value is in ms/2
    #else
                        ptrTimer->TIM_PSC = (((PCLK2 * 2)/1000) - 1);
    #endif
                    }
                    else {                                               // APB1 clock
    #if (((PCLK1 * 2)/1000) - 1 ) > (0xffff)                             // if the clock is too fast to generate 1ms use /2
                        ptrTimer->TIM_PSC = ((((PCLK1 * 2)/1000)/2) - 1);
                        ulCounterMatch *= 2;                             // compensation since value is in ms/2
    #else
                        ptrTimer->TIM_PSC = (((PCLK1 * 2)/1000) - 1);
    #endif
                    }
    #if defined _WINDOWS
                    if (ptrTimer->TIM_PSC > 0xffff) {
                        _EXCEPTION("16 bit prescaler overflow!!");
                    }
                    else if (ulCounterMatch > 0xffff) {
                        _EXCEPTION("ms Delay too long for HW !!");
                    }
    #endif
                }
            }
            else {
                if ((ptrTimerSetup->timer_mode & TIMER_PRESCALE) != 0) { // set user-defined prescaler
                    ptrTimer->TIM_PSC = (ptrTimerSetup->prescaler_value - 1);
                }
                else {
                    ptrTimer->TIM_PSC = 0;                               // ensure prescaler set to divide by 1
                }
    #if PCLK2 != PCLK1
                if (iClock2 != 0) {                                      // APB2 clock
                    ulCounterMatch *= (PCLK2/PCLK1);                     // this timer is clocked by PCLK2 so compensate the value (PCLK2 is assumed to be faster than PCLK2 and always a multiple of x2)
                    while (ulCounterMatch > 0x0000ffff) {
                        unsigned long ulOriginalPrescaler = (ptrTimer->TIM_PSC + 1); // {2}
                        ulOriginalPrescaler *= 2;                        // new value
                        ulCounterMatch /= 2;
                        ptrTimer->TIM_PSC = (ulOriginalPrescaler - 1);
                    }
                }
    #endif
            }
            ptrTimer->TIM_ARR = (unsigned short)ulCounterMatch;          // program the base frequency
            if (ptrTimerSetup->timer_mode & (TIMER_FREQUENCY | TIMER_PWM_CH1 | TIMER_PWM_CH2 | TIMER_PWM_CH3 | TIMER_PWM_CH4)) { // valid also for generating frequency
                if ((ptrTimerSetup->timer_mode & TIMER_PWM_CH1) != 0) {  // program PWM mode on channel 1
                    if ((ptrTimerSetup->timer_mode & TIMER_FREQUENCY) != 0) { // generate frequency rather than PWM
                        ptrTimer->TIM_CCMR1 |= (TIM_CCMR_OCM_MATCH_TOGGLE | TIM_CCMR_OCPE);
                    }
                    else {
                        ptrTimer->TIM_CCR1 = ptrTimerSetup->pwm_value;   // program the PWM control count
                        ptrTimer->TIM_CCMR1 |= (TIM_CCMR_OCM_PWM_1 | TIM_CCMR_OCPE);
                    }
                    switch (ptrTimerSetup->timer_reference) {
                    case 2:                                              // pin configuration for timer 2, channel 1
    #if defined TIMER_2_PARTIAL_REMAP_1
                        _PERIPHERAL_REMAP(TIM2_PARTIAL_REMAP_1);
                        _CONFIG_PERIPHERAL_OUTPUT(A, (PERIPHERAL_TIM1_2), (PORTA_BIT15), (OUTPUT_MEDIUM | OUTPUT_PUSH_PULL));
    #elif defined TIMER_2_PARTIAL_REMAP_2
                        _PERIPHERAL_REMAP(TIM2_PARTIAL_REMAP_2);
                        _CONFIG_PERIPHERAL_OUTPUT(A, (PERIPHERAL_TIM1_2), (PORTA_BIT0), (OUTPUT_MEDIUM | OUTPUT_PUSH_PULL));
    #elif defined TIMER_2_FULL_REMAP
                        _PERIPHERAL_REMAP(TIM2_FULL_REMAP);
                        _CONFIG_PERIPHERAL_OUTPUT(A, (PERIPHERAL_TIM1_2), (PORTA_BIT15), (OUTPUT_MEDIUM | OUTPUT_PUSH_PULL));
    #else
                        _CONFIG_PERIPHERAL_OUTPUT(A, (PERIPHERAL_TIM1_2), (PORTA_BIT0), (OUTPUT_MEDIUM | OUTPUT_PUSH_PULL));
    #endif
                        break;
                    case 3:                                              // pin configuration for timer 3, channel 1
    #if defined TIMER_3_PARTIAL_REMAP
                        _PERIPHERAL_REMAP(TIM3_PARTIAL_REMAP);
                        _CONFIG_PERIPHERAL_OUTPUT(B, (PERIPHERAL_TIM3_4_5), (PORTB_BIT4), (OUTPUT_MEDIUM | OUTPUT_PUSH_PULL));
    #elif defined TIMER_3_FULL_REMAP
                        _PERIPHERAL_REMAP(TIM3_FULL_REMAP);
                        _CONFIG_PERIPHERAL_OUTPUT(C, (PERIPHERAL_TIM3_4_5), (PORTC_BIT6), (OUTPUT_MEDIUM | OUTPUT_PUSH_PULL));
    #else
                        _CONFIG_PERIPHERAL_OUTPUT(A, (PERIPHERAL_TIM3_4_5), (PORTA_BIT6), (OUTPUT_MEDIUM | OUTPUT_PUSH_PULL));
    #endif
                        break;
                    case 4:                                              // pin configuration for timer 4, channel 1
    #if defined TIMER_4_REMAP
                        _PERIPHERAL_REMAP(TIM4_REMAP);
                        _CONFIG_PERIPHERAL_OUTPUT(D, (PERIPHERAL_TIM3_4_5), (PORTD_BIT12), (OUTPUT_MEDIUM | OUTPUT_PUSH_PULL));
    #else
                        _CONFIG_PERIPHERAL_OUTPUT(B, (PERIPHERAL_TIM3_4_5), (PORTB_BIT6), (OUTPUT_MEDIUM | OUTPUT_PUSH_PULL));
    #endif
                        break;
                    case 5:                                              // pin configuration for timer 5, channel 1
                        _CONFIG_PERIPHERAL_OUTPUT(A, (PERIPHERAL_TIM3_4_5), (PORTA_BIT0), (OUTPUT_MEDIUM | OUTPUT_PUSH_PULL));
                        break;
    #if defined TIMER_9_AVAILABLE
                    case 9:                                              // pin configuration for timer 9, channel 1
                        _CONFIG_PERIPHERAL_OUTPUT(A, (PERIPHERAL_TIM8_9_10_11), (PORTA_BIT2), (OUTPUT_MEDIUM | OUTPUT_PUSH_PULL));
                      //_CONFIG_PERIPHERAL_OUTPUT(E, (PERIPHERAL_TIM8_9_10_11), (PORTE_BIT5), (OUTPUT_MEDIUM | OUTPUT_PUSH_PULL));
                        break;
    #endif
    #if defined TIMER_10_AVAILABLE
                    case 10:                                             // pin configuration for timer 10, channel 1
                        _CONFIG_PERIPHERAL_OUTPUT(B, (PERIPHERAL_TIM8_9_10_11), (PORTB_BIT8), (OUTPUT_MEDIUM | OUTPUT_PUSH_PULL));
                      //_CONFIG_PERIPHERAL_OUTPUT(F, (PERIPHERAL_TIM8_9_10_11), (PORTF_BIT6), (OUTPUT_MEDIUM | OUTPUT_PUSH_PULL));
                        break;
    #endif
    #if defined TIMER_11_AVAILABLE
                    case 11:                                             // pin configuration for timer 11, channel 1
                        _CONFIG_PERIPHERAL_OUTPUT(B, (PERIPHERAL_TIM8_9_10_11), (PORTB_BIT9), (OUTPUT_MEDIUM | OUTPUT_PUSH_PULL));
                      //_CONFIG_PERIPHERAL_OUTPUT(F, (PERIPHERAL_TIM8_9_10_11), (PORTF_BIT7), (OUTPUT_MEDIUM | OUTPUT_PUSH_PULL));
                        break;
    #endif
    #if defined TIMER_12_AVAILABLE
                    case 12:                                             // pin configuration for timer 12, channel 1
                        _CONFIG_PERIPHERAL_OUTPUT(B, (PERIPHERAL_CAN1_2_TIM12_13_14), (PORTB_BIT14), (OUTPUT_MEDIUM | OUTPUT_PUSH_PULL));
                      //_CONFIG_PERIPHERAL_OUTPUT(H, (PERIPHERAL_CAN1_2_TIM12_13_14), (PORTH_BIT6), (OUTPUT_MEDIUM | OUTPUT_PUSH_PULL));
                        break;
    #endif
    #if defined TIMER_13_AVAILABLE
                    case 13:                                             // pin configuration for timer 13, channel 1
                        _CONFIG_PERIPHERAL_OUTPUT(A, (PERIPHERAL_CAN1_2_TIM12_13_14), (PORTA_BIT6), (OUTPUT_MEDIUM | OUTPUT_PUSH_PULL));
                      //_CONFIG_PERIPHERAL_OUTPUT(F, (PERIPHERAL_CAN1_2_TIM12_13_14), (PORTF_BIT8), (OUTPUT_MEDIUM | OUTPUT_PUSH_PULL));
                        break;
    #endif
                    }
                    ptrTimer->TIM_CCER |= (TIM_CCER_CC1E);               // enable output
                }
                else if ((ptrTimerSetup->timer_mode & TIMER_PWM_CH2) != 0) { // program PWM mode on channel 2
                    ptrTimer->TIM_CCR2 = ptrTimerSetup->pwm_value;       // program the PWM control count
                    ptrTimer->TIM_CCMR1 |= ((TIM_CCMR_OCM_PWM_1 | TIM_CCMR_OCPE) << TIM_CCMR_CHANNEL_SHIFT);
                    switch (ptrTimerSetup->timer_reference) {
                    case 2:                                              // pin configuration for timer 2, channel 2
    #if defined TIMER_2_PARTIAL_REMAP_1
                        _PERIPHERAL_REMAP(TIM2_PARTIAL_REMAP_1);
                        _CONFIG_PERIPHERAL_OUTPUT(B, (PERIPHERAL_TIM1_2), (PORTB_BIT3), (OUTPUT_MEDIUM | OUTPUT_PUSH_PULL));
    #elif defined TIMER_2_PARTIAL_REMAP_2
                        _PERIPHERAL_REMAP(TIM2_PARTIAL_REMAP_2);
                        _CONFIG_PERIPHERAL_OUTPUT(A, (PERIPHERAL_TIM1_2), (PORTA_BIT1), (OUTPUT_MEDIUM | OUTPUT_PUSH_PULL));
    #elif defined TIMER_2_FULL_REMAP
                        _PERIPHERAL_REMAP(TIM2_FULL_REMAP);
                        _CONFIG_PERIPHERAL_OUTPUT(B, (PERIPHERAL_TIM1_2), (PORTB_BIT3), (OUTPUT_MEDIUM | OUTPUT_PUSH_PULL));
    #else
                        _CONFIG_PERIPHERAL_OUTPUT(A, (PERIPHERAL_TIM1_2), (PORTA_BIT1), (OUTPUT_MEDIUM | OUTPUT_PUSH_PULL));
    #endif
                        break;
                    case 3:                                              // pin configuration for timer 3, channel 2
    #if defined TIMER_3_PARTIAL_REMAP
                        _PERIPHERAL_REMAP(TIM3_PARTIAL_REMAP);
                        _CONFIG_PERIPHERAL_OUTPUT(B, (PERIPHERAL_TIM3_4_5), (PORTB_BIT5), (OUTPUT_MEDIUM | OUTPUT_PUSH_PULL));
    #elif defined TIMER_3_FULL_REMAP
                        _PERIPHERAL_REMAP(TIM3_FULL_REMAP);
                        _CONFIG_PERIPHERAL_OUTPUT(C, (PERIPHERAL_TIM3_4_5), (PORTC_BIT7), (OUTPUT_MEDIUM | OUTPUT_PUSH_PULL));
    #else
                        _CONFIG_PERIPHERAL_OUTPUT(A, (PERIPHERAL_TIM3_4_5), (PORTA_BIT7), (OUTPUT_MEDIUM | OUTPUT_PUSH_PULL));
    #endif
                        break;
                    case 4:                                              // pin configuration for timer 4, channel 2
    #if defined TIMER_4_REMAP
                        _PERIPHERAL_REMAP(TIM4_REMAP);
                        _CONFIG_PERIPHERAL_OUTPUT(D, (PERIPHERAL_TIM3_4_5), (PORTD_BIT13), (OUTPUT_MEDIUM | OUTPUT_PUSH_PULL));
    #else
                        _CONFIG_PERIPHERAL_OUTPUT(B, (PERIPHERAL_TIM3_4_5), (PORTB_BIT7), (OUTPUT_MEDIUM | OUTPUT_PUSH_PULL));
    #endif
                        break;
                    case 5:                                              // pin configuration for timer 5, channel 2
                        _CONFIG_PERIPHERAL_OUTPUT(A, (PERIPHERAL_TIM3_4_5), (PORTA_BIT1), (OUTPUT_MEDIUM | OUTPUT_PUSH_PULL));
                        break;
    #if defined TIMER_9_AVAILABLE
                    case 9:                                              // pin configuration for timer 9, channel 2
                        _CONFIG_PERIPHERAL_OUTPUT(A, (PERIPHERAL_TIM8_9_10_11), (PORTA_BIT3), (OUTPUT_MEDIUM | OUTPUT_PUSH_PULL));
                      //_CONFIG_PERIPHERAL_OUTPUT(E, (PERIPHERAL_TIM8_9_10_11), (PORTE_BIT6), (OUTPUT_MEDIUM | OUTPUT_PUSH_PULL));
                        break;
    #endif
    #if defined TIMER_12_AVAILABLE
                    case 12:                                             // pin configuration for timer 12, channel 2
                        _CONFIG_PERIPHERAL_OUTPUT(H, (PERIPHERAL_CAN1_2_TIM12_13_14), (PORTH_BIT9), (OUTPUT_MEDIUM | OUTPUT_PUSH_PULL));
                        break;
    #endif
                    }
                    ptrTimer->TIM_CCER |= (TIM_CCER_CC2E);               // enable output
                }
                else if ((ptrTimerSetup->timer_mode & TIMER_PWM_CH3) != 0) { // program PWM mode on channel 3
                    ptrTimer->TIM_CCR3 = ptrTimerSetup->pwm_value;       // program the PWM control count
                    ptrTimer->TIM_CCMR2 |= (TIM_CCMR_OCM_PWM_1 | TIM_CCMR_OCPE);
                    switch (ptrTimerSetup->timer_reference) {
                    case 2:                                              // pin configuration for timer 2, channel 3
    #if defined TIMER_2_PARTIAL_REMAP_1
                        _PERIPHERAL_REMAP(TIM2_PARTIAL_REMAP_1);
                        _CONFIG_PERIPHERAL_OUTPUT(A, (PERIPHERAL_TIM1_2), (PORTA_BIT2), (OUTPUT_MEDIUM | OUTPUT_PUSH_PULL));
    #elif defined TIMER_2_PARTIAL_REMAP_2
                        _PERIPHERAL_REMAP(TIM2_PARTIAL_REMAP_2);
                        _CONFIG_PERIPHERAL_OUTPUT(B, (PERIPHERAL_TIM1_2), (PORTB_BIT10), (OUTPUT_MEDIUM | OUTPUT_PUSH_PULL));
    #elif defined TIMER_2_FULL_REMAP
                        _PERIPHERAL_REMAP(TIM2_FULL_REMAP);
                        _CONFIG_PERIPHERAL_OUTPUT(B, (PERIPHERAL_TIM1_2), (PORTB_BIT10), (OUTPUT_MEDIUM | OUTPUT_PUSH_PULL));
    #else
                        _CONFIG_PERIPHERAL_OUTPUT(A, (PERIPHERAL_TIM1_2), (PORTA_BIT2), (OUTPUT_MEDIUM | OUTPUT_PUSH_PULL));
    #endif
                        break;
                    case 3:                                              // pin configuration for timer 3, channel 3
    #if defined TIMER_3_PARTIAL_REMAP
                        _PERIPHERAL_REMAP(TIM3_PARTIAL_REMAP);
                        _CONFIG_PERIPHERAL_OUTPUT(B, (PERIPHERAL_TIM3_4_5), (PORTB_BIT0), (OUTPUT_MEDIUM | OUTPUT_PUSH_PULL));
    #elif defined TIMER_3_FULL_REMAP
                        _PERIPHERAL_REMAP(TIM3_FULL_REMAP);
                        _CONFIG_PERIPHERAL_OUTPUT(C, (PERIPHERAL_TIM3_4_5), (PORTC_BIT8), (OUTPUT_MEDIUM | OUTPUT_PUSH_PULL));
    #else
                        _CONFIG_PERIPHERAL_OUTPUT(B, (PERIPHERAL_TIM3_4_5), (PORTB_BIT0), (OUTPUT_MEDIUM | OUTPUT_PUSH_PULL));
    #endif
                        break;
                    case 4:                                              // pin configuration for timer 4, channel 3
    #if defined TIMER_4_REMAP
                        _PERIPHERAL_REMAP(TIM4_REMAP);
                        _CONFIG_PERIPHERAL_OUTPUT(D, (PERIPHERAL_TIM3_4_5), (PORTD_BIT14), (OUTPUT_MEDIUM | OUTPUT_PUSH_PULL));
    #else
                        _CONFIG_PERIPHERAL_OUTPUT(B, (PERIPHERAL_TIM3_4_5), (PORTB_BIT8), (OUTPUT_MEDIUM | OUTPUT_PUSH_PULL));
    #endif
                        break;
                    case 5:                                              // pin configuration for timer 5, channel 3
                        _CONFIG_PERIPHERAL_OUTPUT(A, (PERIPHERAL_TIM3_4_5), (PORTA_BIT2), (OUTPUT_MEDIUM | OUTPUT_PUSH_PULL));
                        break;
                    }
                    ptrTimer->TIM_CCER |= (TIM_CCER_CC3E);               // enable output
                }
                else {                                                   // program PWM mode on channel 4
                    ptrTimer->TIM_CCR4 = ptrTimerSetup->pwm_value;       // program the PWM control count
                    ptrTimer->TIM_CCMR2 |= ((TIM_CCMR_OCM_PWM_1 | TIM_CCMR_OCPE) << TIM_CCMR_CHANNEL_SHIFT);
                    switch (ptrTimerSetup->timer_reference) {
                    case 2:                                              // pin configuration for timer 2, channel 4
    #if defined TIMER_2_PARTIAL_REMAP_1
                        _PERIPHERAL_REMAP(TIM2_PARTIAL_REMAP_1);
                        _CONFIG_PERIPHERAL_OUTPUT(A, (PERIPHERAL_TIM1_2), (PORTA_BIT3), (OUTPUT_MEDIUM | OUTPUT_PUSH_PULL));
    #elif defined TIMER_2_PARTIAL_REMAP_2
                        _PERIPHERAL_REMAP(TIM2_PARTIAL_REMAP_2);
                        _CONFIG_PERIPHERAL_OUTPUT(B, (PERIPHERAL_TIM1_2), (PORTB_BIT11), (OUTPUT_MEDIUM | OUTPUT_PUSH_PULL));
    #elif defined TIMER_2_FULL_REMAP
                        _PERIPHERAL_REMAP(TIM2_FULL_REMAP);
                        _CONFIG_PERIPHERAL_OUTPUT(B, (PERIPHERAL_TIM1_2), (PORTB_BIT11), (OUTPUT_MEDIUM | OUTPUT_PUSH_PULL));
    #else
                        _CONFIG_PERIPHERAL_OUTPUT(A, (PERIPHERAL_TIM1_2), (PORTA_BIT3), (OUTPUT_MEDIUM | OUTPUT_PUSH_PULL));
    #endif
                        break;
                    case 3:                                              // pin configuration for timer 3, channel 4
    #if defined TIMER_3_PARTIAL_REMAP
                        _PERIPHERAL_REMAP(TIM3_PARTIAL_REMAP);
                        _CONFIG_PERIPHERAL_OUTPUT(B, (PERIPHERAL_TIM3_4_5), (PORTB_BIT1), (OUTPUT_MEDIUM | OUTPUT_PUSH_PULL));
    #elif defined TIMER_3_FULL_REMAP
                        _PERIPHERAL_REMAP(TIM3_FULL_REMAP);
                        _CONFIG_PERIPHERAL_OUTPUT(C, (PERIPHERAL_TIM3_4_5), (PORTC_BIT9), (OUTPUT_MEDIUM | OUTPUT_PUSH_PULL));
    #else
                        _CONFIG_PERIPHERAL_OUTPUT(B, (PERIPHERAL_TIM3_4_5), (PORTB_BIT1), (OUTPUT_MEDIUM | OUTPUT_PUSH_PULL));
    #endif
                        break;
                    case 4:                                              // pin configuration for timer 4, channel 4
    #if defined TIMER_4_REMAP
                        _PERIPHERAL_REMAP(TIM4_REMAP);
                        _CONFIG_PERIPHERAL_OUTPUT(D, (PERIPHERAL_TIM3_4_5), (PORTD_BIT15), (OUTPUT_MEDIUM | OUTPUT_PUSH_PULL));
    #else
                        _CONFIG_PERIPHERAL_OUTPUT(B, (PERIPHERAL_TIM3_4_5), (PORTB_BIT9), (OUTPUT_MEDIUM | OUTPUT_PUSH_PULL));
    #endif
                        break;
                    case 5:                                              // pin configuration for timer 5, channel 4
                        _CONFIG_PERIPHERAL_OUTPUT(A, (PERIPHERAL_TIM3_4_5), (PORTA_BIT3), (OUTPUT_MEDIUM | OUTPUT_PUSH_PULL));
                        break;
                    }
                    ptrTimer->TIM_CCER |= (TIM_CCER_CC4E);               // enable output
                }
                ptrTimer->TIM_CR1 = (TIM_CR1_ARPE | TIM_CR1_CEN);        // enable timer in auto-reload mode
                ptrTimer->TIM_EGR = (TIM_EGR_UG);                        // start the operation
                return;
            }
            ptrTimer->TIM_CR1 = 0;                                       // ensure counter is stopped
            ptrTimer->TIM_CNT = 0;                                       // and count value is reset
            if (ptrTimerSetup->int_handler != 0) {                       // interrupt to be entered
                _timer_handler[iInterruptReference] = ptrTimerSetup->int_handler;
                fnEnterInterrupt(iTimerIntID, ptrTimerSetup->int_priority, timer_irq);
                ptrTimer->TIM_DIER = (TIM_DIER_UIE);                     // enable interrupt when update count is reached
            }
            if (ptrTimerSetup->timer_mode & TIMER_PERIODIC) {
                ptrTimer->TIM_CR1 = (TIM_CR1_URS_UNDER_OVERFLOW | TIM_CR1_ARPE | TIM_CR1_CEN); // enable timer in auto-reload mode
            }
            else {                                                       // single shot timer
                ptrTimer->TIM_CR1 = (TIM_CR1_URS_UNDER_OVERFLOW | TIM_CR1_OPM | TIM_CR1_CEN);  // enable timer in one-pulse mode
            }
        }
#endif
