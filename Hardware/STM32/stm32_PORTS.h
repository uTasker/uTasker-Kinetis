/**********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher

    ---------------------------------------------------------------------
    File:      stm32_PORTS.h
    Project:   Single Chip Embedded Internet
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2018
    *********************************************************************

*/


/* =================================================================== */
/*                           include files                             */
/* =================================================================== */

/* =================================================================== */
/*                          local definitions                          */
/* =================================================================== */

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

#if defined _PORT_INTERRUPT_CODE
static void (*exti_handler[16])(void) = {0};                             // port change interrupt handlers

// Port change interrupt handler for EXTI0
//
__interrupt static void _exti0_handler(void)
{
    static unsigned long ulPending;
    while ((ulPending = (EXTI_PR & 0x0001)) != 0) {
    #if defined _WINDOWS
        EXTI_PR &= ~ulPending;                                           // clear interrupt source(s)
    #else
        EXTI_PR = ulPending;                                             // clear interrupt source(s)
    #endif
        uDisable_Interrupt();                                            // ensure call cannot be interrupted
        exti_handler[0]();                                               // call the user interrupt handler
        uEnable_Interrupt();                                             // release
    }
}

// Port change interrupt handler for EXTI1
//
__interrupt static void _exti1_handler(void)
{
    static unsigned long ulPending;
    while ((ulPending = (EXTI_PR & 0x0002)) != 0) {
    #if defined _WINDOWS
        EXTI_PR &= ~ulPending;                                           // clear interrupt source(s)
    #else
        EXTI_PR = ulPending;                                             // clear interrupt source(s)
    #endif
        uDisable_Interrupt();                                            // ensure call can not be interrupted
        exti_handler[1]();                                               // call the user interrupt handler
        uEnable_Interrupt();                                             // release
    }
}

// Port change interrupt handler for EXTI2
//
__interrupt static void _exti2_handler(void)
{
    static unsigned long ulPending;
    while ((ulPending = (EXTI_PR & 0x0004)) != 0) {
    #if defined _WINDOWS
        EXTI_PR &= ~ulPending;                                           // clear interrupt source(s)
    #else
        EXTI_PR = ulPending;                                             // clear interrupt source(s)
    #endif
        uDisable_Interrupt();                                            // ensure call can not be interrupted
        exti_handler[2]();                                               // call the user interrupt handler
        uEnable_Interrupt();                                             // release
    }
}

// Port change interrupt handler for EXTI3
//
__interrupt static void _exti3_handler(void)
{
    static unsigned long ulPending;
    while ((ulPending = (EXTI_PR & 0x0008)) != 0) {
    #if defined _WINDOWS
        EXTI_PR &= ~ulPending;                                           // clear interrupt source(s)
    #else
        EXTI_PR = ulPending;                                             // clear interrupt source(s)
    #endif
        uDisable_Interrupt();                                            // ensure call can not be interrupted
        exti_handler[3]();                                               // call the user interrupt handler
        uEnable_Interrupt();                                             // release
    }
}

// Port change interrupt handler for EXTI4
//
__interrupt static void _exti4_handler(void)
{
    static unsigned long ulPending;
    while ((ulPending = (EXTI_PR & 0x0010)) != 0) {
    #if defined _WINDOWS
        EXTI_PR &= ~ulPending;                                           // clear interrupt source(s)
    #else
        EXTI_PR = ulPending;                                             // clear interrupt source(s)
    #endif
        uDisable_Interrupt();                                            // ensure call can not be interrupted
        exti_handler[4]();                                               // call the user interrupt handler
        uEnable_Interrupt();                                             // release
    }
}

// Port change interrupt handler for EXTI5..9
//
__interrupt static void _exti5_9_handler(void)
{
    unsigned long ulPending;
    int iInterrupt;
    while ((ulPending = (EXTI_PR & 0x03e0)) != 0) {
    #if defined _WINDOWS
        EXTI_PR &= ~ulPending;                                           // clear interrupt source(s)
    #else
        EXTI_PR = ulPending;                                             // clear interrupt source(s)
    #endif
        ulPending >>= 5;
        iInterrupt = 5;
        while (ulPending != 0) {
            if (ulPending & 0x00000001) {
                uDisable_Interrupt();                                    // ensure call can not be interrupted
                exti_handler[iInterrupt]();                              // call the user interrupt handler
                uEnable_Interrupt();                                     // release
            }
            ulPending >>= 1;
            iInterrupt++;
        }
    }
}

// Port change interrupt handler for EXTI10..15
//
__interrupt static void _exti10_15_handler(void)
{
    unsigned long ulPending;
    int iInterrupt;
    while ((ulPending = (EXTI_PR & 0xfc00)) != 0) {
    #if defined _WINDOWS
        EXTI_PR &= ~ulPending;                                           // clear interrupt source(s)
    #else
        EXTI_PR = ulPending;                                             // clear interrupt source(s)
    #endif
        ulPending >>= 10;
        iInterrupt = 10;
        while (ulPending != 0) {
            if (ulPending & 0x00000001) {
                uDisable_Interrupt();                                    // ensure call can not be interrupted
                    exti_handler[iInterrupt]();                          // call the user interrupt handler
                uEnable_Interrupt();                                     // release
            }
            ulPending >>= 1;
            iInterrupt++;
        }
    }
}
#endif


#if defined _PORT_INT_CONFIG_CODE
        {
            INTERRUPT_SETUP *ptrSetup = (INTERRUPT_SETUP *)ptrSettings;
    #if defined _STM32F2XX || defined _STM32F4XX || defined _STM32F7XX || defined _STM32L432 || defined _STM32L0x1 || defined _STM32F031 || defined _STM32L4X5 || defined _STM32L4X6
            unsigned long   *ptrMux = SYSCFG_EXTICR1_ADDR;
    #else
            unsigned long   *ptrMux = AFIO_EXTICR1_ADD;
    #endif
            unsigned short   usPortBit = (ptrSetup->int_port_bit);
            int iInputRef = 0;
    #if defined _WINDOWS
            if (usPortBit == 0) {
                _EXCEPTION("Check interrupt reference! (use PORTC_BIT4 for example)");
            }
    #endif
            while ((usPortBit & 0x0001) == 0) {
                usPortBit >>= 1;
                iInputRef++;
            }
    #if defined _WINDOWS
            if (usPortBit != 0x0001) {
                _EXCEPTION("Check interrupt reference! (use single reference)");
            }
    #endif
            usPortBit = (ptrSetup->int_port_bit);
    #if defined _STM32F2XX || defined _STM32F4XX || defined _STM32F7XX
            POWER_UP(AHB1, (RCC_AHB1ENR_GPIOAEN << ptrSetup->int_port)); // ensure that the port is clocked
            POWER_UP(APB2, RCC_APB2ENR_SYSCFGEN);                        // power up the system configuration controller so that it can correctly multiplex the inputs to the external interrupt controller
    #elif defined _STM32L432 || defined _STM32L4X5 || defined _STM32L4X6
            POWER_UP(AHB1, (RCC_AHB2ENR_GPIOAEN << ptrSetup->int_port)); // ensure that the port is clocked
            POWER_UP(APB2, RCC_APB2ENR_SYSCFGEN);                        // power up the system configuration controller so that it can correctly multiplex the inputs to the external interrupt controller
    #elif defined _STM32L0x1
            RCC_IOPENR |= ((RCC_IOPENR_IOPAEN) << ptrSetup->int_port);   // ensure that the port is clocked
            POWER_UP(APB2, RCC_APB2ENR_SYSCFGEN);                        // power up the system configuration controller so that it can correctly multiplex the inputs to the external interrupt controller
    #elif defined _STM32F031
            POWER_UP(AHB, (RCC_AHBENR_IOPAEN << ptrSetup->int_port));    // ensure that the port is clocked
            POWER_UP(APB2, RCC_APB2ENR_SYSCFGEN);                        // power up the system configuration controller so that it can correctly multiplex the inputs to the external interrupt controller
    #else
            POWER_UP(APB2, (RCC_APB2ENR_IOPAEN << ptrSetup->int_port));  // ensure that the port is clocked
            POWER_UP(APB2, (RCC_APB2ENR_AFIOEN));                        // power up the alternate-function I/O controller so that it can correctly multiplex the inputs to the external interrupt controller
    #endif
            ptrMux += (iInputRef/4);
            *ptrMux &= ~(0x0000000f << (4 * (iInputRef%4)));             // select the input for the channel - mask out the position
            *ptrMux |= (ptrSetup->int_port << (4 * (iInputRef%4)));      // select the port
            if ((ptrSetup->int_port_sense & IRQ_RISING_EDGE) != 0) {
                EXTI_RTSR |= usPortBit;                                  // enable rising edge trigger
            }
            if ((ptrSetup->int_port_sense & IRQ_FALLING_EDGE) != 0) {
                EXTI_FTSR |= usPortBit;                                  // enable falling edge trigger
            }
            exti_handler[iInputRef] = ptrSetup->int_handler;             // enter the user handler
            switch (iInputRef) {
    #if defined irq_EXTI0_ID
            case 0:
                fnEnterInterrupt(irq_EXTI0_ID, ptrSetup->int_priority, _exti0_handler); // enter interrupt handler
                break;
            case 1:
                fnEnterInterrupt(irq_EXTI1_ID, ptrSetup->int_priority, _exti1_handler); // enter interrupt handler
                break;
            case 2:
                fnEnterInterrupt(irq_EXTI2_ID, ptrSetup->int_priority, _exti2_handler); // enter interrupt handler
                break;
            case 3:
                fnEnterInterrupt(irq_EXTI3_ID, ptrSetup->int_priority, _exti3_handler); // enter interrupt handler
                break;
            case 4:
                fnEnterInterrupt(irq_EXTI4_ID, ptrSetup->int_priority, _exti4_handler); // enter interrupt handler
                break;
            case 5:
            case 6:
            case 7:
            case 8:
            case 9:
                fnEnterInterrupt(irq_EXTI9_5_ID, ptrSetup->int_priority, _exti5_9_handler); // enter interrupt handler
                break;
            case 10:
            case 11:
            case 12:
            case 13:
            case 14:
            case 15:
                fnEnterInterrupt(irq_EXTI15_10_ID, ptrSetup->int_priority, _exti10_15_handler); // enter interrupt handler
                break;
    #endif
            default:
    #if defined _WINDOWS
            if (usPortBit != 0) {
                _EXCEPTION("Check interrupt reference!");
            }
    #endif
                return;
            }
            EXTI_IMR |= usPortBit;                                       // enable interrupt
        }
        break;
#endif

