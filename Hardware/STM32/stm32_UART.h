/**********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher

    ---------------------------------------------------------------------
    File:      stm32_UART.h
    Project:   Single Chip Embedded Internet
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2018
    *********************************************************************
    03.03.2012 Correct UART fraction calculation                         {1}
    28.02.2017 Add UARTs 7 and 8                                         {2}
    27.09.2018 Respect fraction when calculating the UART baud rate to display in simulator {3}

*/



// Get a pointer to the start of the corresponding UART control block
//
static USART_REG *fnSelectChannel(QUEUE_HANDLE Channel)
{
    switch (Channel) {
#if !defined USART1_NOT_PRESENT
    case 0:
        return (USART_REG *)(USART1_BLOCK);
#endif
#if USARTS_AVAILABLE > 1
    case 1:
        return (USART_REG *)(USART2_BLOCK);
#endif
#if USARTS_AVAILABLE > 2
    case 2:
        return (USART_REG *)(USART3_BLOCK);
#endif
#if UARTS_AVAILABLE > 0
    case 3:
        return (USART_REG *)(UART4_BLOCK);
#endif
#if UARTS_AVAILABLE > 1
    case 4:
        return (USART_REG *)(UART5_BLOCK);
#endif
#if defined _STM32F2XX || defined _STM32F4XX || defined _STM32F7XX
    case 5:
        return (USART_REG *)(USART6_BLOCK);
#elif defined _STM32L4X5 || defined _STM32L4X6
    case 5:
        return (USART_REG *)(LPUART1_BLOCK);
#endif
#if defined _STM32F7XX || defined _STM32F429 || defined _STM32F427       // {2}
    case 6:
        return (USART_REG *)(UART7_BLOCK);
    case 7:
        return (USART_REG *)(UART8_BLOCK);
#endif
    default:
        _EXCEPTION("Invalid UART!");
        return 0;
    }
}

/* =================================================================== */
/*                     USART interrupt handlers                        */
/* =================================================================== */

#if !defined USART1_NOT_PRESENT
static __interrupt void SCI1_Interrupt(void)
{
    while (((USART1_CR1 & USART_CR1_RXNEIE) != 0) && ((USART1_ISR & USART_ISR_RXNE) != 0)) { // if an enabled reception interrupt
        fnSciRxByte((unsigned char)USART1_RDR, 0);                       // receive data interrupt
    #if defined _WINDOWS
        USART1_ISR &= ~USART_ISR_RXNE;
    #endif
    }
    while (((USART1_CR1 & USART_CR1_TXEIE) != 0) && ((USART1_ISR & USART_ISR_TXE) != 0)) { // if an enabled transmission interrupt
        fnSciTxByte(0);                                                  // transmit data empty interrupt
    }
}
#endif

static __interrupt void SCI2_Interrupt(void)
{
    while (((USART2_CR1 & USART_CR1_RXNEIE) != 0) && ((USART2_ISR & USART_ISR_RXNE) != 0)) { // if an enabled reception interrupt
        fnSciRxByte((unsigned char)USART2_RDR, 1);                       // receive data interrupt
    #if defined _WINDOWS
        USART2_ISR &= ~USART_ISR_RXNE;
    #endif
    }
    while (((USART2_CR1 & USART_CR1_TXEIE) != 0) && ((USART2_ISR & USART_ISR_TXE) != 0)) { // if an enabled transmission interrupt
        fnSciTxByte(1);                                                  // transmit data empty interrupt
    }
}
#if (USARTS_AVAILABLE > 2) && !defined USART3_NOT_PRESENT
static __interrupt void SCI3_Interrupt(void)
{
    while (((USART3_CR1 & USART_CR1_RXNEIE) != 0) && ((USART3_ISR & USART_ISR_RXNE) != 0)) { // if an enabled reception interrupt
        fnSciRxByte((unsigned char)USART3_RDR, 2);                       // receive data interrupt
    #if defined _WINDOWS
        USART3_ISR &= ~USART_ISR_RXNE;
    #endif
    }
    while (((USART3_CR1 & USART_CR1_TXEIE) != 0) && ((USART3_ISR & USART_ISR_TXE) != 0)) { // if an enabled transmission interrupt
        fnSciTxByte(2);                                                  // transmit data empty interrupt
    }
}
#endif
#if UARTS_AVAILABLE > 0
static __interrupt void SCI4_Interrupt(void)
{
    while (((UART4_CR1 & USART_CR1_RXNEIE) != 0) && ((UART4_ISR & USART_ISR_RXNE) != 0)) { // if an enabled reception interrupt
        fnSciRxByte((unsigned char)UART4_RDR, 3);                        // receive data interrupt
    #if defined _WINDOWS
        UART4_ISR &= ~USART_ISR_RXNE;
    #endif
    }
    while (((UART4_CR1 & USART_CR1_TXEIE) != 0) && ((UART4_ISR & USART_ISR_TXE) != 0)) { // if an enabled transmission interrupt
        fnSciTxByte(3);                                                  // transmit data empty interrupt
    }
}
#endif
#if UARTS_AVAILABLE > 1
static __interrupt void SCI5_Interrupt(void)
{
    while (((UART5_CR1 & USART_CR1_RXNEIE) != 0) && ((UART5_ISR & USART_ISR_RXNE) != 0)) { // if an enabled reception interrupt
        fnSciRxByte((unsigned char)UART5_RDR, 4);                        // receive data interrupt
    #if defined _WINDOWS
        UART5_ISR &= ~USART_ISR_RXNE;
    #endif
    }
    while (((UART5_CR1 & USART_CR1_TXEIE) != 0) && ((UART5_ISR & USART_ISR_TXE) != 0)) { // if an enabled transmission interrupt
        fnSciTxByte(4);                                                  // transmit data empty interrupt
    }
}
#endif
#if defined _STM32F2XX || defined _STM32F4XX || defined _STM32F7XX
static __interrupt void SCI6_Interrupt(void)
{
    while (((USART6_CR1 & USART_CR1_RXNEIE) != 0) && ((USART6_ISR & USART_ISR_RXNE) != 0)) { // if an enabled reception interrupt
        fnSciRxByte((unsigned char)USART6_RDR, 5);                       // receive data interrupt
    #if defined _WINDOWS
        USART6_ISR &= ~USART_ISR_RXNE;
    #endif
    }
    while (((USART6_CR1 & USART_CR1_TXEIE) != 0) && ((USART6_ISR & USART_ISR_TXE) != 0)) { // if an enabled transmission interrupt
        fnSciTxByte(5);                                                  // transmit data empty interrupt
    }
}
#elif defined _STM32L4X5 || defined _STM32L4X6
static __interrupt void SCI6_Interrupt(void)
{
    while (((LPUART1_CR1 & USART_CR1_RXNEIE) != 0) && ((LPUART1_ISR & USART_ISR_RXNE) != 0)) { // if an enabled reception interrupt
        fnSciRxByte((unsigned char)LPUART1_RDR, 5);                      // receive data interrupt
#if defined _WINDOWS
        LPUART1_ISR &= ~USART_ISR_RXNE;
#endif
    }
    while (((LPUART1_CR1 & USART_CR1_TXEIE) != 0) && ((LPUART1_ISR & USART_ISR_TXE) != 0)) { // if an enabled transmission interrupt
        fnSciTxByte(5);                                                  // transmit data empty interrupt
    }
}
#endif
#if defined _STM32F7XX || defined _STM32F429 || defined _STM32F427       // {2}
static __interrupt void SCI7_Interrupt(void)
{
    while (((UART7_CR1 & USART_CR1_RXNEIE) != 0) && ((UART7_ISR & USART_ISR_RXNE) != 0)) { // if an enabled reception interrupt
        fnSciRxByte((unsigned char)UART7_RDR, 6);                        // receive data interrupt
    #if defined _WINDOWS
        UART7_ISR &= ~USART_ISR_RXNE;
    #endif
    }
    while (((UART7_CR1 & USART_CR1_TXEIE) != 0) && ((UART7_ISR & USART_ISR_TXE) != 0)) { // if an enabled transmission interrupt
        fnSciTxByte(6);                                                  // transmit data empty interrupt
    }
}

static __interrupt void SCI8_Interrupt(void)
{
    while (((UART8_CR1 & USART_CR1_RXNEIE) != 0) && ((UART8_ISR & USART_ISR_RXNE) != 0)) { // if an enabled reception interrupt
        fnSciRxByte((unsigned char)UART8_RDR, 7);                        // receive data interrupt
    #if defined _WINDOWS
        UART8_ISR &= ~USART_ISR_RXNE;
    #endif
    }
    while (((UART8_CR1 & USART_CR1_TXEIE) != 0) && ((UART8_ISR & USART_ISR_TXE) != 0)) { // if an enabled transmission interrupt
        fnSciTxByte(7);                                                  // transmit data empty interrupt
    }
}
#endif

#if defined UART_EXTENDED_MODE && defined SERIAL_MULTIDROP_RX            // {18a}
extern unsigned char fnGetMultiDropByte(QUEUE_HANDLE channel)
{
    USART_REG *USART_regs = fnSelectChannel(channel);
    return (unsigned char)(USART_regs->UART_DR >> 8);                    // return the 9th bit of the received word
}
#endif

// Configure the SCI hardware
//
extern void fnConfigSCI(QUEUE_HANDLE Channel, TTYTABLE *pars)
{
    USART_REG *USART_regs = fnSelectChannel(Channel);                    // collect a pointer to the USART/UART/LPUART being used
    unsigned long ulSpeed;
#if !defined _STM32F7XX_ && !defined _STM32L432 && !defined _STM32L0x1
    unsigned char ucFraction;
#endif

    switch (Channel) {
#if !defined USART1_NOT_PRESENT
    case 0:
        POWER_UP(APB2, RCC_APB2ENR_USART1EN);                            // enable clocks to USART1
        fnEnterInterrupt(irq_USART1_ID, PRIORITY_USART1, SCI1_Interrupt);// enter USART interrupt handler
        break;
#endif
    case 1:
#if defined _STM32L432 || defined _STM32L4X5 || defined _STM32L4X6
        RCC_APB1ENR1 |= (RCC_APB1ENR1_USART2EN);                         // enable clocks to USART2
#else
        POWER_UP(APB1, RCC_APB1ENR_USART2EN);                            // enable clocks to USART2
#endif
        fnEnterInterrupt(irq_USART2_ID, PRIORITY_USART2, SCI2_Interrupt);// enter USART interrupt handler
        break;
#if (USARTS_AVAILABLE > 2) && !defined USART3_NOT_PRESENT
    case 2:
    #if defined _STM32L432 || defined _STM32L4X5 || defined _STM32L4X6
        RCC_APB1ENR1 |= (RCC_APB1ENR1_USART3EN);                         // enable clocks to USART3
    #else
        POWER_UP(APB1, RCC_APB1ENR_USART3EN);                            // enable clocks to USART3
    #endif
        fnEnterInterrupt(irq_USART3_ID, PRIORITY_USART3, SCI3_Interrupt);// enter USART interrupt handler
        break;
#endif
#if UARTS_AVAILABLE > 0
    case 3:
    #if defined _STM32L432 || defined _STM32L4X5 || defined _STM32L4X6
        RCC_APB1ENR1 |= (RCC_APB1ENR1_UART4EN);                          // enable clocks to UART4
    #else
        POWER_UP(APB1, RCC_APB1ENR_UART4EN);                             // enable clocks to UART4
    #endif
        fnEnterInterrupt(irq_UART4_ID, PRIORITY_UART4, SCI4_Interrupt);  // enter UART interrupt handler
        break;
#endif
#if UARTS_AVAILABLE > 1
    case 4:
    #if defined _STM32L432                                               // LPUART1
        RCC_APB1ENR2 |= (RCC_APB1ENR2_LPUART1EN);                        // enable clocks to LPUART1
    #elif defined _STM32L4X5 || defined _STM32L4X6
        RCC_APB1ENR1 |= (RCC_APB1ENR1_UART5EN);                          // enable clocks to UART5
    #else
        POWER_UP(APB1, RCC_APB1ENR_UART5EN);                             // enable clocks to UART5
    #endif
        fnEnterInterrupt(irq_UART5_ID, PRIORITY_UART5, SCI5_Interrupt);  // enter UART interrupt handler
        break;
#endif
#if defined _STM32F2XX || defined _STM32F4XX || defined _STM32F7XX
    case 5:
        POWER_UP(APB2, RCC_APB2ENR_USART6EN);                            // enable clocks to USART6
        fnEnterInterrupt(irq_USART6_ID, PRIORITY_USART6, SCI6_Interrupt);// enter UART interrupt handler
        break;
#elif defined _STM32L4X5 || defined _STM32L4X6
    case 5:
        RCC_APB1ENR2 |= (RCC_APB1ENR2_LPUART1EN);                        // enable clocks to LPUART1
        fnEnterInterrupt(irq_LPUART1_ID, PRIORITY_LPUART1, SCI6_Interrupt);// enter LPUART interrupt handler
        break;
#endif
#if defined _STM32F7XX || defined _STM32F429 || defined _STM32F427       // {2}
    case 6:
        POWER_UP(APB1, RCC_APB1ENR_UART7EN);                             // enable clocks to UART7
        fnEnterInterrupt(irq_UART7_ID, PRIORITY_UART7, SCI7_Interrupt);  // enter UART interrupt handler
        break;
    case 7:
        POWER_UP(APB1, RCC_APB1ENR_UART8EN);                             // enable clocks to UART8
        fnEnterInterrupt(irq_UART8_ID, PRIORITY_UART7, SCI8_Interrupt);  // enter UART interrupt handler
        break;
#endif
    default:
        _EXCEPTION("Invalid U(S)ART!");
        return;
    }

#if defined SUPPORT_HW_FLOW
    if ((pars->Config & RTS_CTS) != 0) {                                 // HW flow control defined so configure RTS/CTS pins
        fnControlLine(Channel, (CONFIG_RTS_PIN | CONFIG_CTS_PIN), 0);
    }
#endif

    USART_regs->UART_CR1 = 0;                                            // start with uart disabled

#if defined _STM32F2XX || defined _STM32F4XX || defined _STM32F7XX
    if ((Channel == 0) || (Channel == 5))                                // USART1 and USART6 clocked from PCLK2
#else
    if (Channel == 0)                                                    // USART1 clocked from PCLK2 (note that _STM32L432 has configurable clock source but PCLK2 is is used for compatibility)
#endif
    {
#if !defined USART1_NOT_PRESENT
        switch (pars->ucSpeed) {
        case SERIAL_BAUD_300:
#if defined _STM32L432
            ulSpeed = (PCLK2/300);                                       // set 300
#else
            ulSpeed = (PCLK2/16/300);                                    // set 300
    #if !defined _STM32F7XX_
            ucFraction = (unsigned char)((float)((float)PCLK2/16/300 - (int)(PCLK2/16/300)) * 16); // {1}
    #endif
#endif
            break;
        case SERIAL_BAUD_600:
#if defined _STM32L432
            ulSpeed = (PCLK2/600);                                       // set 600
#else
            ulSpeed = (PCLK2/16/600);                                    // set 600
    #if !defined _STM32F7XX_
            ucFraction = (unsigned char)((float)((float)PCLK2/16/600 - (int)(PCLK2/16/600)) * 16); // {1}
    #endif
#endif
            break;
        case SERIAL_BAUD_1200:
#if defined _STM32L432
            ulSpeed = (PCLK2/1200);                                      // set 1200
#else
            ulSpeed = (PCLK2/16/1200);                                   // set 1200
    #if !defined _STM32F7XX_
            ucFraction = (unsigned char)((float)((float)PCLK2/16/1200 - (int)(PCLK2/16/1200)) * 16);
    #endif
#endif
            break;
        case SERIAL_BAUD_2400:
#if defined _STM32L432
            ulSpeed = (PCLK2/2400);                                      // set 2400
#else
            ulSpeed = (PCLK2/16/2400);                                   // set 2400
    #if !defined _STM32F7XX_
            ucFraction = (unsigned char)((float)((float)PCLK2/16/2400 - (int)(PCLK2/16/2400)) * 16);
    #endif
#endif
            break;
        case SERIAL_BAUD_4800:
#if defined _STM32L432
            ulSpeed = (PCLK2/4800);                                      // set 4800
#else
            ulSpeed = (PCLK2/16/4800);                                   // set 4800
    #if !defined _STM32F7XX_
            ucFraction = (unsigned char)((float)((float)PCLK2/16/4800 - (int)(PCLK2/16/4800)) * 16);
    #endif
#endif
            break;
        case SERIAL_BAUD_9600:
#if defined _STM32L432
            ulSpeed = (PCLK2/9600);                                      // set 9600
#else
            ulSpeed = (PCLK2/16/9600);                                   // set 9600
    #if !defined _STM32F7XX_
            ucFraction = (unsigned char)((float)((float)PCLK2/16/9600 - (int)(PCLK2/16/9600)) * 16);
    #endif
#endif
            break;
        case SERIAL_BAUD_14400:
#if defined _STM32L432
            ulSpeed = (PCLK2/14400);                                     // set 14400
#else
            ulSpeed = (PCLK2/16/14400);                                  // set 14400
    #if !defined _STM32F7XX_
            ucFraction = (unsigned char)((float)((float)PCLK2/16/14400 - (int)(PCLK2/16/14400)) * 16);
    #endif
#endif
            break;
        default:                                                         // if not valid value set this one
        case SERIAL_BAUD_19200:
#if defined _STM32L432
            ulSpeed = (PCLK2/19200);                                     // set 19200
#else
            ulSpeed = (PCLK2/16/19200);                                  // set 19200
    #if !defined _STM32F7XX_
            ucFraction = (unsigned char)((float)((float)PCLK2/16/19200 - (int)(PCLK2/16/19200)) * 16);
    #endif
#endif
            break;
        case SERIAL_BAUD_38400:
#if defined _STM32L432
            ulSpeed = (PCLK2/38400);                                     // set 38400
#else
            ulSpeed = (PCLK2/16/38400);                                  // set 38400
    #if !defined _STM32F7XX_
            ucFraction = (unsigned char)((float)((float)PCLK2/16/38400 - (int)(PCLK2/16/38400)) * 16);
    #endif
#endif
            break;
        case SERIAL_BAUD_57600:
#if defined _STM32L432
            ulSpeed = (PCLK2/57600);                                     // set 57600
#else
            ulSpeed = (PCLK2/16/57600);                                  // set 57600
    #if !defined _STM32F7XX_
            ucFraction = (unsigned char)((float)((float)PCLK2/16/57600 - (int)(PCLK2/16/57600)) * 16);
    #endif
#endif
            break;
        case SERIAL_BAUD_115200:
#if defined _STM32L432
            ulSpeed = (PCLK2/115200);                                    // set 115200
#else
            ulSpeed = (PCLK2/16/115200);                                 // set 115200
    #if !defined _STM32F7XX_
            ucFraction = (unsigned char)((float)((float)PCLK2/16/115200 - (int)(PCLK2/16/115200)) * 16);
    #endif
#endif
            break;
        case SERIAL_BAUD_230400:
#if defined _STM32L432
            ulSpeed = (PCLK2/230400);                                    // set 230400
#else
            ulSpeed = (PCLK2/16/230400);                                 // set 230400
    #if !defined _STM32F7XX_
            ucFraction = (unsigned char)((float)((float)PCLK2/16/230400 - (int)(PCLK2/16/230400)) * 16);
    #endif
#endif
            break;
        }
#endif
    }
#if USARTS_AVAILABLE > 1
    else {                                                               // else clocked from PCLK1 (note that _STM32L432 has configurable clock source but PCLK1 is is used for compatibility)
        switch (pars->ucSpeed) {
        case SERIAL_BAUD_300:
#if defined _STM32L432 || defined _STM32L0x1
            ulSpeed = (PCLK1/300);                                       // set 300
#else
    #if defined _STM32L4X5 || defined _STM32L4X6
            if (Channel == 5) {                                          // LPUART1
                ulSpeed = (unsigned long)((unsigned long long)((unsigned long long)PCLK1 * (unsigned long long)256) / 300); // set 300
                break;
            }
    #endif
            ulSpeed = (PCLK1/16/300);                                    // set 300
    #if !defined _STM32F7XX_
            ucFraction = (unsigned char)((float)((float)PCLK1/16/300 - (int)(PCLK1/16/300)) * 16); // {1}
    #endif
#endif
            break;
        case SERIAL_BAUD_600:
#if defined _STM32L432 || defined _STM32L0x1
            ulSpeed = (PCLK1/600);                                       // set 600
#else
    #if defined _STM32L4X5 || defined _STM32L4X6
            if (Channel == 5) {                                          // LPUART1
                ulSpeed = (unsigned long)((unsigned long long)((unsigned long long)PCLK1 * (unsigned long long)256) / 600); // set 600
                break;
            }
    #endif
            ulSpeed = (PCLK1/16/600);                                    // set 600
    #if !defined _STM32F7XX_
            ucFraction = (unsigned char)((float)((float)PCLK1/16/600 - (int)(PCLK1/16/600)) * 16); // {1}
    #endif
#endif
            break;
        case SERIAL_BAUD_1200:
#if defined _STM32L432 || defined _STM32L0x1
            ulSpeed = (PCLK1/1200);                                      // set 1200
#else
    #if defined _STM32L4X5 || defined _STM32L4X6
            if (Channel == 5) {                                          // LPUART1
                ulSpeed = (unsigned long)((unsigned long long)((unsigned long long)PCLK1 * (unsigned long long)256) / 1200); // set 1200
                break;
            }
    #endif
            ulSpeed = (PCLK1/16/1200);                                   // set 1200
    #if !defined _STM32F7XX_
            ucFraction = (unsigned char)((float)((float)PCLK1/16/1200 - (int)(PCLK1/16/1200)) * 16);
    #endif
#endif
            break;
        case SERIAL_BAUD_2400:
#if defined _STM32L432 || defined _STM32L0x1
            ulSpeed = (PCLK1/2400);                                      // set 2400
#else
    #if defined _STM32L4X5 || defined _STM32L4X6
            if (Channel == 5) {                                          // LPUART1
                ulSpeed = (unsigned long)((unsigned long long)((unsigned long long)PCLK1 * (unsigned long long)256) / 2400); // set 2400
                break;
            }
    #endif
            ulSpeed = (PCLK1/16/2400);                                   // set 2400
    #if !defined _STM32F7XX_
            ucFraction = (unsigned char)((float)((float)PCLK1/16/2400 - (int)(PCLK1/16/2400)) * 16);
    #endif
#endif
            break;
        case SERIAL_BAUD_4800:
#if defined _STM32L432 || defined _STM32L0x1
            ulSpeed = (PCLK1/4800);                                      // set 4800
#else
    #if defined _STM32L4X5 || defined _STM32L4X6
            if (Channel == 5) {                                          // LPUART1
                ulSpeed = (unsigned long)((unsigned long long)((unsigned long long)PCLK1 * (unsigned long long)256) / 4800); // set 4800
                break;
            }
    #endif
            ulSpeed = (PCLK1/16/4800);                                   // set 4800
    #if !defined _STM32F7XX_
            ucFraction = (unsigned char)((float)((float)PCLK1/16/4800 - (int)(PCLK1/16/4800)) * 16);
    #endif
#endif
            break;
        case SERIAL_BAUD_9600:
#if defined _STM32L432 || defined _STM32L0x1
            ulSpeed = (PCLK1/9600);                                      // set 9600
#else
    #if defined _STM32L4X5 || defined _STM32L4X6
            if (Channel == 5) {                                          // LPUART1
                ulSpeed = (unsigned long)((unsigned long long)((unsigned long long)PCLK1 * (unsigned long long)256) / 9600); // set 9600
                break;
            }
    #endif
            ulSpeed = (PCLK1/16/9600);                                   // set 9600
    #if !defined _STM32F7XX_
            ucFraction = (unsigned char)((float)((float)PCLK1/16/9600 - (int)(PCLK1/16/9600)) * 16);
    #endif
#endif
            break;
        case SERIAL_BAUD_14400:
#if defined _STM32L432 || defined _STM32L0x1
            ulSpeed = (PCLK1/14400);                                     // set 14400
#else
    #if defined _STM32L4X5 || defined _STM32L4X6
            if (Channel == 5) {                                          // LPUART1
                ulSpeed = (unsigned long)((unsigned long long)((unsigned long long)PCLK1 * (unsigned long long)256) / 14400); // set 14400
                break;
            }
    #endif
            ulSpeed = (PCLK1/16/14400);                                  // set 14400
    #if !defined _STM32F7XX_
            ucFraction = (unsigned char)((float)((float)PCLK1/16/14400 - (int)(PCLK1/16/14400)) * 16);
    #endif
#endif
            break;
        default:                                                         // if not valid value set this one
        case SERIAL_BAUD_19200:
#if defined _STM32L432 || defined _STM32L0x1
            ulSpeed = (PCLK1/19200);                                     // set 19200
#else
    #if defined _STM32L4X5 || defined _STM32L4X6
            if (Channel == 5) {                                          // LPUART1
                ulSpeed = (unsigned long)((unsigned long long)((unsigned long long)PCLK1 * (unsigned long long)256) / 19200); // set 19200
                break;
            }
    #endif
            ulSpeed = (PCLK1/16/19200);                                  // set 19200
    #if !defined _STM32F7XX_
            ucFraction = (unsigned char)((float)((float)PCLK1/16/19200 - (int)(PCLK1/16/19200)) * 16);
    #endif
#endif
            break;
        case SERIAL_BAUD_38400:
#if defined _STM32L432 || defined _STM32L0x1
            ulSpeed = (PCLK1/38400);                                     // set 38400
#else
    #if defined _STM32L4X5 || defined _STM32L4X6
            if (Channel == 5) {                                          // LPUART1
                ulSpeed = (unsigned long)((unsigned long long)((unsigned long long)PCLK1 * (unsigned long long)256) / 38400); // set 38400
                break;
            }
    #endif
            ulSpeed = (PCLK1/16/38400);                                  // set 38400
    #if !defined _STM32F7XX_
            ucFraction = (unsigned char)((float)((float)PCLK1/16/38400 - (int)(PCLK1/16/38400)) * 16);
    #endif
#endif
            break;
        case SERIAL_BAUD_57600:
#if defined _STM32L432 || defined _STM32L0x1
            ulSpeed = (PCLK1/57600);                                     // set 57600
#else
    #if defined _STM32L4X5 || defined _STM32L4X6
            if (Channel == 5) {                                          // LPUART1
                ulSpeed = (unsigned long)((unsigned long long)((unsigned long long)PCLK1 * (unsigned long long)256) / 57600); // set 57600
                break;
            }
    #endif
            ulSpeed = (PCLK1/16/57600);                                  // set 57600
    #if !defined _STM32F7XX_
            ucFraction = (unsigned char)((float)((float)PCLK1/16/57600 - (int)(PCLK1/16/57600)) * 16);
    #endif
#endif
            break;
        case SERIAL_BAUD_115200:
#if defined _STM32L432 || defined _STM32L0x1
            ulSpeed = (PCLK1/115200);                                    // set 115200
#else
    #if defined _STM32L4X5 || defined _STM32L4X6
            if (Channel == 5) {                                          // LPUART1
                ulSpeed = (unsigned long)((unsigned long long)((unsigned long long)PCLK1 * (unsigned long long)256) / 115200); // set 115200
                break;
            }
    #endif
            ulSpeed = (PCLK1/16/115200);                                 // set 115200
    #if !defined _STM32F7XX_
            ucFraction = (unsigned char)((float)((float)PCLK1/16/115200 - (int)(PCLK1/16/115200)) * 16);
    #endif
#endif
            break;
        case SERIAL_BAUD_230400:
#if defined _STM32L432 || defined _STM32L0x1
            ulSpeed = (PCLK1/230400);                                    // set 230400
#else
    #if defined _STM32L4X5 || defined _STM32L4X6
            if (Channel == 5) {                                          // LPUART1
                ulSpeed = (unsigned long)((unsigned long long)((unsigned long long)PCLK1 * (unsigned long long)256) / 230400); // set 230400
                break;
            }
    #endif
            ulSpeed = (PCLK1/16/230400);                                 // set 230400
    #if !defined _STM32F7XX_
            ucFraction = (unsigned char)((float)((float)PCLK1/16/230400 - (int)(PCLK1/16/230400)) * 16);
    #endif
#endif
            break;
        }
    }
#endif
#if defined _STM32F7XX_ || defined _STM32L432 || defined _STM32L0x1
    USART_regs->UART_BRR = ulSpeed;                                      // set baud rate value
#elif defined _STM32L4X5 || defined _STM32L4X6
    if (5 == Channel) {                                                  // LPUART
#if defined _WINDOWS
        if (ulSpeed < 0x300) {
            _EXCEPTION("It is forbidden to write vales less that 0x300");
        }
#endif
        USART_regs->UART_BRR = ulSpeed;                                  // set baud rate value
    }
    else {
        USART_regs->UART_BRR = ((ulSpeed << 4) | ucFraction);            // set baud rate value
    }
#else
    USART_regs->UART_BRR = ((ulSpeed << 4) | ucFraction);                // set baud rate value
#endif

    if ((pars->Config & (RS232_ODD_PARITY | RS232_EVEN_PARITY)) != 0) {  // parity is to be enabled
        if ((pars->Config & RS232_ODD_PARITY) != 0) {
            USART_regs->UART_CR1 = (USART_CR1_PCE | USART_CR1_PS);       // enable odd parity - set at MSB (8th bit in 7 bit mode or 9th bit in 8 bit mode)
        }
        else {
            USART_regs->UART_CR1 = (USART_CR1_PCE);                      // enable odd parity - set at MSB (8th bit in 7 bit mode or 9th bit in 8 bit mode)
        }
    }
    else {
      //USART_regs->UART_CR1 = 0;                                        // no parity
    }

    if ((pars->Config & TWO_STOPS) != 0) {
        USART_regs->UART_CR2 = USART_CR2_2_STOP;                         // 2 stop bits
    }
    else if ((pars->Config & ONE_HALF_STOPS) != 0) {
        USART_regs->UART_CR2 = USART_CR2_1_5_STOP;                       // 1.5 stop bits
#if defined _WINDOWS
        if (Channel > 2) {                                               // 0.5 and 1.5 stop bits not available on UARTs 4 and 5
            pars->Config &= ~(ONE_HALF_STOPS);                           // simulator performs one stop bit instead
        }
#endif
    }
    else {
        USART_regs->UART_CR2 = 0;                                        // 1 stop bit
    }

    if ((pars->Config & CHAR_7) == 0) {                                  // 7 bits is only possible when parity is enabled
        if ((pars->Config & (RS232_ODD_PARITY | RS232_EVEN_PARITY)) != 0) { // if parity is enable in 8 bit mode set 9 bit mode so that it is inserted at the 9th bit position
#if defined _STM32F7XX || defined _STM32L432 || defined _STM32L0x1 || defined _STM32F031 || defined _STM32L4X5 || defined _STM32L4X6
            USART_regs->UART_CR1 |= USART_CR1_9BIT;
#else
            USART_regs->UART_CR1 |= USART_CR1_M;
#endif
        }
    }
#if defined UART_EXTENDED_MODE && defined SERIAL_MULTIDROP_TX            // {18a}
    if ((pars->Config & MULTIDROP_MODE_TX) != 0) {
        USART_regs->UART_CR1 |= USART_CR1_M;                             // enable 9 bit mode
        USART_regs->UART_CR1 &= ~(USART_CR1_PCE | USART_CR1_PS);         // disable parity generation so that the 9th bit can be inserted in the parity location
    }
#endif
#if defined _WINDOWS
    if ((Channel == 0) || (Channel == 5)) {
    #if defined _STM32L432
        fnConfigSimSCI(Channel, (PCLK2/USART_regs->UART_BRR), pars); // open a serial port on PC if desired
    #elif defined _STM32F7XX_
        fnConfigSimSCI(Channel, (PCLK2/((USART_regs->UART_BRR) * 16)), pars); // open a serial port on PC if desired
    #elif defined _STM32F031
        fnConfigSimSCI(Channel, (PCLK/((USART_regs->UART_BRR >> 4) * 16)), pars); // open a serial port on PC if desired
    #elif defined _STM32L4X5 || defined _STM32L4X6
        if (Channel == 5) {                                              // LPUART1
            fnConfigSimSCI(Channel, (PCLK1 / ((USART_regs->UART_BRR) / 256)), pars); // open a serial port on PC if desired
        }
        else {
            fnConfigSimSCI(Channel, (PCLK2 / ((USART_regs->UART_BRR >> 4) * 16)), pars); // open a serial port on PC if desired
        }
    #else
        fnConfigSimSCI(Channel, (unsigned long)((float)PCLK2/(16 * ((float)(USART_regs->UART_BRR >> 4) + (float)((1.0/16)*(USART_regs->UART_BRR & 0xf))))), pars); // {3} open a serial port on PC if desired
    #endif
    }
    #if !defined _STM32F031
    else {
        #if defined _STM32L432 || defined _STM32L0x1
            #if defined _STM32L0x1
        if (USART_regs->UART_BRR <= 16) {
            _EXCEPTION("BBR value is too small - this results in very slow or no speeds: >16 MUST be used!!");
        }
            #endif
        fnConfigSimSCI(Channel, (PCLK1/USART_regs->UART_BRR), pars); // open a serial port on PC if desired
        #elif defined _STM32F7XX_
        fnConfigSimSCI(Channel, (PCLK1/((USART_regs->UART_BRR) * 16)), pars); // open a serial port on PC if desired
        #else
        fnConfigSimSCI(Channel, (unsigned long)((float)PCLK1/(16 * ((float)(USART_regs->UART_BRR >> 4) + (float)((1.0/16)*(USART_regs->UART_BRR & 0xf))))), pars); // {3} open a serial port on PC if desired
        #endif
    }
    #endif
#endif
}

// Enable reception on the defined channel
//
extern void fnRxOn(QUEUE_HANDLE Channel)
{
    switch (Channel) {
#if !defined USART1_NOT_PRESENT
    case 0:                                                              // USART1
    #if defined USART1_PARTIAL_REMAP && defined _STM32F031
        _CONFIG_PERIPHERAL_INPUT(A, (PERIPHERAL_USART1), (PORTA_BIT15), (UART_RX_INPUT_TYPE)); // RX 2 on PA15
    #elif defined USART1_REMAP
        _PERIPHERAL_REMAP(USART1_REMAPPED);
        _CONFIG_PERIPHERAL_INPUT(B, (PERIPHERAL_USART1_2_3), (PORTB_BIT7), (UART_RX_INPUT_TYPE)); // RX 1 on PB7
    #else
        _CONFIG_PERIPHERAL_INPUT(A, (PERIPHERAL_USART1_2_3), (PORTA_BIT10), (UART_RX_INPUT_TYPE)); // RX 1 on PA10
    #endif
        USART1_CR1 |= (USART_CR1_UE | USART_CR1_RE | USART_CR1_RXNEIE);  // enable the receiver with Rx interrupts
        break;
#endif
#if USARTS_AVAILABLE > 1
    case 1:                                                              // USART2
    #if defined USART2_REMAP
        _PERIPHERAL_REMAP(USART2_REMAPPED);
        _CONFIG_PERIPHERAL_INPUT(D, (PERIPHERAL_USART1_2_3), (PORTD_BIT6), (UART_RX_INPUT_TYPE)); // RX 2 on PD6
    #elif defined USART2_PARTIAL_REMAP && (defined _STM32L432 || defined _STM32L0x1)
        _CONFIG_PERIPHERAL_INPUT(A, (PERIPHERAL_USART2), (PORTA_BIT15), (UART_RX_INPUT_TYPE)); // RX 2 on PA15
    #else
        _CONFIG_PERIPHERAL_INPUT(A, (PERIPHERAL_USART1_2_3), (PORTA_BIT3), (UART_RX_INPUT_TYPE)); // RX 2 on PA3
    #endif
        USART2_CR1 |= (USART_CR1_UE | USART_CR1_RE | USART_CR1_RXNEIE);  // enable the receiver with Rx interrupts
        break;
#endif
#if (USARTS_AVAILABLE > 2) && !defined USART3_NOT_PRESENT
    case 2:                                                              // USART3
    #if defined USART3_FULL_REMAP
        _PERIPHERAL_REMAP(USART3_FULLY_REMAPPED);
        _CONFIG_PERIPHERAL_INPUT(D, (PERIPHERAL_USART1_2_3), (PORTD_BIT9), (UART_RX_INPUT_TYPE)); // RX 3 on PD9
    #elif defined USART3_PARTIAL_REMAP
         _PERIPHERAL_REMAP(USART3_PARTIALLY_REMAPPED);
        _CONFIG_PERIPHERAL_INPUT(C, (PERIPHERAL_USART1_2_3), (PORTC_BIT11), (UART_RX_INPUT_TYPE)); // RX 3 on PC11
    #else
        _CONFIG_PERIPHERAL_INPUT(B, (PERIPHERAL_USART1_2_3), (PORTB_BIT11), (UART_RX_INPUT_TYPE)); // RX 3 on PB11
    #endif
        USART3_CR1 |= (USART_CR1_UE | USART_CR1_RE | USART_CR1_RXNEIE);  // enable the receiver with Rx interrupts
        break;
#endif
#if UARTS_AVAILABLE > 0
    case 3:
    #if defined _STM32L432                                               // LPUART1
        _CONFIG_PERIPHERAL_INPUT(A, (PERIPHERAL_LPUART1), (PORTA_BIT3), (UART_RX_INPUT_TYPE)); // RX 4 on PA3
    #elif defined _STM32L4X5 || defined _STM32L4X6
        _CONFIG_PERIPHERAL_INPUT(C, (PERIPHERAL_UART4_5), (PORTC_BIT11), (UART_RX_INPUT_TYPE)); // RX 4 on PC11
        UART4_CR1 |= (USART_CR1_UE | USART_CR1_RE | USART_CR1_RXNEIE);   // enable the receiver with Rx interrupts
    #else
        _CONFIG_PERIPHERAL_INPUT(C, (PERIPHERAL_USART4_5_6), (PORTC_BIT11), (UART_RX_INPUT_TYPE)); // RX 4 on PC11
        UART4_CR1 |= (USART_CR1_UE | USART_CR1_RE | USART_CR1_RXNEIE);   // enable the receiver with Rx interrupts
    #endif
        break;
#endif
#if UARTS_AVAILABLE > 1
    case 4:
    #if defined _STM32L4X5 || defined _STM32L4X6
        _CONFIG_PERIPHERAL_INPUT(D, (PERIPHERAL_UART4_5), (PORTD_BIT2), (UART_RX_INPUT_TYPE)); // RX 5 on PD2
    #else
        _CONFIG_PERIPHERAL_INPUT(D, (PERIPHERAL_USART4_5_6), (PORTD_BIT2), (UART_RX_INPUT_TYPE)); // RX 5 on PD2
    #endif
        UART5_CR1 |= (USART_CR1_UE | USART_CR1_RE | USART_CR1_RXNEIE);   // enable the receiver with Rx interrupts
        break;
#endif
#if defined _STM32F2XX || defined _STM32F4XX || defined _STM32F7XX
    case 5:
    #if defined USART6_REMAP
        _CONFIG_PERIPHERAL_INPUT(G, (PERIPHERAL_USART4_5_6), (PORTG_BIT9), (UART_RX_INPUT_TYPE)); // RX 6 on PG9
    #else
        _CONFIG_PERIPHERAL_INPUT(C, (PERIPHERAL_USART4_5_6), (PORTC_BIT7), (UART_RX_INPUT_TYPE)); // RX 6 on PC7
    #endif
        USART6_CR1 |= (USART_CR1_UE | USART_CR1_RE | USART_CR1_RXNEIE);   // enable the receiver with Rx interrupts
        break;
#elif defined _STM32L4X5 || defined _STM32L4X6
    case 5:
    #if defined LPUART_REMAP_B
        _CONFIG_PERIPHERAL_INPUT(B, (PERIPHERAL_LPUART1), (PORTB_BIT10), (UART_RX_INPUT_TYPE)); // LP RX 1 on PB10
    #elif defined LPUART_REMAP_C
        _CONFIG_PERIPHERAL_INPUT(B, (PERIPHERAL_LPUART1), (PORTC_BIT0), (UART_RX_INPUT_TYPE)); // LP RX 1 on PC0
    #elif defined LPUART_REMAP_G
        _CONFIG_PERIPHERAL_INPUT(G, (PERIPHERAL_LPUART1), (PORTG_BIT8), (UART_RX_INPUT_TYPE)); // LP RX 1 on PG8
    #else
        _CONFIG_PERIPHERAL_INPUT(A, (PERIPHERAL_LPUART1), (PORTA_BIT3), (UART_RX_INPUT_TYPE)); // LP RX 1 on PA3
    #endif
        LPUART1_CR1 |= (USART_CR1_UE | USART_CR1_RE | USART_CR1_RXNEIE); // enable the receiver with Rx interrupts
        break;
#endif
#if defined _STM32F7XX || defined _STM32F429 || defined _STM32F427       // {2}
    case 6:
    #if defined USART7_REMAP
        _CONFIG_PERIPHERAL_INPUT(F, (PERIPHERAL_USART4_5_6_7_8), (PORTF_BIT6), (UART_RX_INPUT_TYPE)); // RX 7 on PT6
    #else
        _CONFIG_PERIPHERAL_INPUT(E, (PERIPHERAL_USART4_5_6_7_8), (PORTE_BIT7), (UART_RX_INPUT_TYPE)); // RX 7 on PE7
    #endif
        UART7_CR1 |= (USART_CR1_UE | USART_CR1_RE | USART_CR1_RXNEIE);  // enable the receiver with Rx interrupts
        break;
    case 7:
        _CONFIG_PERIPHERAL_INPUT(E, (PERIPHERAL_USART4_5_6_7_8), (PORTE_BIT0), (UART_RX_INPUT_TYPE)); // RX 8 on PE0
        UART8_CR1 |= (USART_CR1_UE | USART_CR1_RE | USART_CR1_RXNEIE);  // enable the receiver with Rx interrupts
        break;
#endif
    }
}

// Disable reception on the defined channel
//
extern void fnRxOff(QUEUE_HANDLE Channel)
{
    USART_REG *USART_regs = fnSelectChannel(Channel);
    USART_regs->UART_CR1 &= ~(USART_CR1_RE);                             // disable the receiver
    if ((USART_regs->UART_CR1 & USART_CR1_TE) == 0) {                    // if transmitter is also disabled
        USART_regs->UART_CR1 &= ~(USART_CR1_UE);                         // stop UART clocks
    }
}

// Enable transmission on the defined channel
//
extern void fnTxOn(QUEUE_HANDLE Channel)
{
    switch (Channel) {
#if !defined USART1_NOT_PRESENT
    case 0:
    #if defined _WINDOWS
        USART1_CR1 |= (USART_CR1_UE | USART_CR1_TE);                     // enable the transmitter earlier when simulating so that the peripheral function can be detected
    #endif
    #if defined USART1_PARTIAL_REMAP && defined _STM32F031
        _CONFIG_PERIPHERAL_OUTPUT(A, (PERIPHERAL_USART1),     (PORTA_BIT2), (OUTPUT_MEDIUM | OUTPUT_PUSH_PULL)); // TX 1 on PA2
    #elif defined USART1_REMAP && !defined USART1_NOREMAP_TX
        _PERIPHERAL_REMAP(USART1_REMAPPED);
        _CONFIG_PERIPHERAL_OUTPUT(B, (PERIPHERAL_USART1_2_3), (PORTB_BIT6), (OUTPUT_MEDIUM | OUTPUT_PUSH_PULL)); // TX 1 on PB6
    #else
        _CONFIG_PERIPHERAL_OUTPUT(A, (PERIPHERAL_USART1_2_3), (PORTA_BIT9), (OUTPUT_MEDIUM | OUTPUT_PUSH_PULL)); // TX 1 on PA9
    #endif
        USART1_CR1 |= (USART_CR1_UE | USART_CR1_TE);                     // enable the transmitter
        break;
#endif
#if USARTS_AVAILABLE > 1
    case 1:                                                              // USART2
    #if defined _WINDOWS
        USART2_CR1 |= (USART_CR1_UE | USART_CR1_TE);                     // enable the transmitter earlier when simulating so that the peripheral function can be detected
    #endif
    #if defined USART2_REMAP
        _PERIPHERAL_REMAP(USART2_REMAPPED);
        _CONFIG_PERIPHERAL_OUTPUT(D, (PERIPHERAL_USART1_2_3), (PORTD_BIT5), (OUTPUT_MEDIUM | OUTPUT_PUSH_PULL)); // TX 2 on PD5
    #else
        _CONFIG_PERIPHERAL_OUTPUT(A, (PERIPHERAL_USART1_2_3), (PORTA_BIT2), (OUTPUT_MEDIUM | OUTPUT_PUSH_PULL)); // TX 2 on PA2
    #endif
        USART2_CR1 |= (USART_CR1_UE | USART_CR1_TE);                     // enable the transmitter
        break;
#endif
#if (USARTS_AVAILABLE > 2) && !defined USART3_NOT_PRESENT
    case 2:
    #if defined _WINDOWS
        USART3_CR1 |= (USART_CR1_UE | USART_CR1_TE);                     // enable the transmitter earlier when simulating so that the peripheral function can be detected
    #endif
    #if defined USART3_FULL_REMAP
        _PERIPHERAL_REMAP(USART3_FULLY_REMAPPED);
        _CONFIG_PERIPHERAL_OUTPUT(D, (PERIPHERAL_USART1_2_3), (PORTD_BIT8), (OUTPUT_MEDIUM | OUTPUT_PUSH_PULL)); // TX 3 on PD8
    #elif defined USART3_PARTIAL_REMAP
        _PERIPHERAL_REMAP(USART3_PARTIALLY_REMAPPED);
        _CONFIG_PERIPHERAL_OUTPUT(C, (PERIPHERAL_USART1_2_3), (PORTC_BIT10), (OUTPUT_MEDIUM | OUTPUT_PUSH_PULL)); // TX 3 on PC10
    #else
        _CONFIG_PERIPHERAL_OUTPUT(B, (PERIPHERAL_USART1_2_3), (PORTB_BIT10), (OUTPUT_MEDIUM | OUTPUT_PUSH_PULL)); // TX 3 on PB10
    #endif
        USART3_CR1 |= (USART_CR1_UE | USART_CR1_TE);                     // enable the transmitter
        break;
#endif
#if UARTS_AVAILABLE > 0
    case 3:
    #if defined _STM32L432
        _CONFIG_PERIPHERAL_OUTPUT(A, (PERIPHERAL_LPUART1), (PORTA_BIT2), (OUTPUT_MEDIUM | OUTPUT_PUSH_PULL)); // TX 4 on PA2
    #elif defined _STM32L4X5 || defined _STM32L4X6
        _CONFIG_PERIPHERAL_OUTPUT(C, (PERIPHERAL_UART4_5), (PORTC_BIT10), (OUTPUT_MEDIUM | OUTPUT_PUSH_PULL)); // TX 4 on PC10
    #else
        _CONFIG_PERIPHERAL_OUTPUT(C, (PERIPHERAL_USART4_5_6), (PORTC_BIT10), (OUTPUT_MEDIUM | OUTPUT_PUSH_PULL)); // TX 4 on PC10
    #endif
        UART4_CR1 |= (USART_CR1_UE | USART_CR1_TE);                      // enable the transmitter
        break;
#endif
#if UARTS_AVAILABLE > 1
    case 4:
    #if defined _STM32L4X5 || defined _STM32L4X6
        _CONFIG_PERIPHERAL_OUTPUT(C, (PERIPHERAL_UART4_5), (PORTC_BIT12), (OUTPUT_MEDIUM | OUTPUT_PUSH_PULL)); // TX 5 on PC12
    #else
        _CONFIG_PERIPHERAL_OUTPUT(C, (PERIPHERAL_USART4_5_6), (PORTC_BIT12), (OUTPUT_MEDIUM | OUTPUT_PUSH_PULL)); // TX 5 on PC12
    #endif
        UART5_CR1 |= (USART_CR1_UE | USART_CR1_TE);                      // enable the transmitter
        break;
#endif
#if defined _STM32F2XX || defined _STM32F4XX || defined _STM32F7XX
    case 5:
    #if defined USART6_REMAP
        _CONFIG_PERIPHERAL_OUTPUT(G, (PERIPHERAL_USART4_5_6), (PORTG_BIT14), (OUTPUT_MEDIUM | OUTPUT_PUSH_PULL)); // TX 6 on PG14
    #else
        _CONFIG_PERIPHERAL_OUTPUT(C, (PERIPHERAL_USART4_5_6), (PORTC_BIT6), (OUTPUT_MEDIUM | OUTPUT_PUSH_PULL)); // TX 6 on PC6
    #endif
        USART6_CR1 |= (USART_CR1_UE | USART_CR1_TE);                     // enable the transmitter
        break;
#elif defined _STM32L4X5 || defined _STM32L4X6
    case 5:
    #if defined LPUART_REMAP_B
        _CONFIG_PERIPHERAL_OUTPUT(B, (PERIPHERAL_LPUART1), (PORTB_BIT11), (OUTPUT_MEDIUM | OUTPUT_PUSH_PULL)); // LP TX 1 on PB11
    #elif defined LPUART_REMAP_C
        _CONFIG_PERIPHERAL_OUTPUT(C, (PERIPHERAL_LPUART1), (PORTC_BIT1), (OUTPUT_MEDIUM | OUTPUT_PUSH_PULL)); // LP TX 1 on PC1
    #elif defined LPUART_REMAP_G
        _CONFIG_PERIPHERAL_OUTPUT(G, (PERIPHERAL_LPUART1), (PORTG_BIT7), (OUTPUT_MEDIUM | OUTPUT_PUSH_PULL)); // LP TX 1 on PG7
    #else
        _CONFIG_PERIPHERAL_OUTPUT(A, (PERIPHERAL_LPUART1), (PORTA_BIT2), (OUTPUT_MEDIUM | OUTPUT_PUSH_PULL)); // LP TX 1 on PA2
    #endif
        LPUART1_CR1 |= (USART_CR1_UE | USART_CR1_TE);                    // enable the transmitter
        break;
#endif
#if defined _STM32F7XX || defined _STM32F429 || defined _STM32F427       // {2}
    case 6:
    #if defined USART7_REMAP
        _CONFIG_PERIPHERAL_OUTPUT(F, (PERIPHERAL_USART4_5_6_7_8), (PORTF_BIT7), (OUTPUT_MEDIUM | OUTPUT_PUSH_PULL)); // TX 7 on PF7
    #else
        _CONFIG_PERIPHERAL_OUTPUT(E, (PERIPHERAL_USART4_5_6_7_8), (PORTE_BIT8), (OUTPUT_MEDIUM | OUTPUT_PUSH_PULL)); // TX 7 on PE8
    #endif
        UART7_CR1 |= (USART_CR1_UE | USART_CR1_TE);                      // enable the transmitter
        break;
    case 7:
        _CONFIG_PERIPHERAL_OUTPUT(E, (PERIPHERAL_USART4_5_6_7_8), (PORTE_BIT1), (OUTPUT_MEDIUM | OUTPUT_PUSH_PULL)); // TX 8 on PE1
        UART8_CR1 |= (USART_CR1_UE | USART_CR1_TE);                      // enable the transmitter
        break;
#endif
    }
}

// Disable transmission on the defined channel
//
extern void fnTxOff(QUEUE_HANDLE Channel)
{
    USART_REG *USART_regs = fnSelectChannel(Channel);
    USART_regs->UART_CR1 &= ~(USART_CR1_TE);                             // disable the transmitter
    if ((USART_regs->UART_CR1 & USART_CR1_RE) == 0) {                    // if receiver is also disabled
        USART_regs->UART_CR1 &= ~(USART_CR1_UE);                         // stop UART clocks
    }
}

// The TTY driver uses this call to disable the transmit interrupt of the serial port
//
extern void fnClearTxInt(QUEUE_HANDLE channel)
{
    switch (channel) {
    case 0:
        USART1_CR1 &= ~(USART_CR1_TXEIE);                                // disable transmit interrupts
        break;
    case 1:
        USART2_CR1 &= ~(USART_CR1_TXEIE);                                // disable transmit interrupts
        break;
#if (USARTS_AVAILABLE > 2) && !defined USART3_NOT_PRESENT
    case 2:
        USART3_CR1 &= ~(USART_CR1_TXEIE);                                // disable transmit interrupts
        break;
#endif
#if UARTS_AVAILABLE > 0
    case 3:
        UART4_CR1 &= ~(USART_CR1_TXEIE);                                 // disable transmit interrupts
        break;
#endif
#if UARTS_AVAILABLE > 1
    case 4:
        UART5_CR1 &= ~(USART_CR1_TXEIE);                                 // disable transmit interrupts
        break;
#endif
#if defined _STM32F2XX || defined _STM32F4XX || defined _STM32F7XX
    case 5:
        USART6_CR1 &= ~(USART_CR1_TXEIE);                                // disable transmit interrupts
        break;
#elif defined _STM32L4X5 || defined _STM32L4X6
    case 5:
        LPUART1_CR1 &= ~(USART_CR1_TXEIE);                               // disable transmit interrupts
        break;
#endif
#if defined _STM32F7XX || defined _STM32F429 || defined _STM32F427       // {2}
    case 6:
        UART7_CR1 &= ~(USART_CR1_TXEIE);                                 // disable transmit interrupts
        break;
    case 7:
        UART8_CR1 &= ~(USART_CR1_TXEIE);                                 // disable transmit interrupts
        break;
#endif
    }
}

#if defined UART_EXTENDED_MODE && defined SERIAL_MULTIDROP_TX            // {18a}
    #if defined _STM32F2XX || defined _STM32F4XX || defined _STM32F7XX
    static unsigned char ucExtendedWithTx[6];
    #else
    static unsigned char ucExtendedWithTx[5];
    #endif

// Collect 9th bit of data ready for subsequent transmission together with further 8 bits
//
extern void fnSetMultiDropByte(QUEUE_HANDLE channel, unsigned char ucMSB)
{
    ucExtendedWithTx[channel] = ucMSB;
}
#endif

// The TTY driver uses this call to send a byte of data over the serial port
//
extern int fnTxByte(QUEUE_HANDLE channel, unsigned char ucTxByte)
{
    switch (channel) {
#if !defined USART1_NOT_PRESENT
    case 0:                                                              // USART 1
        if ((USART1_ISR & USART_ISR_TXE) == 0) {
            return 1;                                                    // busy, wait
        }
        USART1_CR1 |= (USART_CR1_TXEIE);                                 // ensure Tx interrupt is enabled
    #if defined UART_EXTENDED_MODE && defined SERIAL_MULTIDROP_TX        // {18a}
        USART1_TDR = ((ucExtendedWithTx[channel] << 8) | ucTxByte);      // send the byte with extended bits
    #else
        USART1_TDR = ucTxByte;                                           // send the byte
    #endif
    #if defined _WINDOWS
        USART1_ISR &= ~USART_ISR_TXE;
        iInts |= CHANNEL_0_SERIAL_INT;                                   // simulate interrupt
    #endif
        break;
#endif
    case 1:                                                              // USART 2
        if ((USART2_ISR & USART_ISR_TXE) == 0) {
            return 1;                                                    // busy, wait
        }
        USART2_CR1 |= (USART_CR1_TXEIE);                                 // ensure Tx interrupt is enabled
#if defined UART_EXTENDED_MODE && defined SERIAL_MULTIDROP_TX            // {18a}
        USART2_DR = ((ucExtendedWithTx[channel] << 8) | ucTxByte);       // send the byte with extended bits
#else
        USART2_TDR = ucTxByte;                                           // send the byte
#endif
#if defined _WINDOWS
        USART2_ISR &= ~USART_ISR_TXE;
        iInts |= CHANNEL_1_SERIAL_INT;                                   // simulate interrupt
#endif
        break;
#if (USARTS_AVAILABLE > 2) && !defined USART3_NOT_PRESENT
    case 2:                                                              // USART 3
        if ((USART3_ISR & USART_ISR_TXE) == 0) {
            return 1;                                                    // busy, wait
        }
        USART3_CR1 |= (USART_CR1_TXEIE);                                 // ensure Tx interrupt is enabled
    #if defined UART_EXTENDED_MODE && defined SERIAL_MULTIDROP_TX        // {18a}
        USART3_TDR = ((ucExtendedWithTx[channel] << 8) | ucTxByte);      // send the byte with extended bits
    #else
        USART3_TDR = ucTxByte;                                           // send the byte
    #endif
    #if defined _WINDOWS
        USART3_ISR &= ~USART_ISR_TXE;
        iInts |= CHANNEL_2_SERIAL_INT;                                   // simulate interrupt
    #endif
        break;
#endif
#if UARTS_AVAILABLE > 0
    case 3:                                                              // UART 4
        if ((UART4_ISR & USART_ISR_TXE) == 0) {
            return 1;                                                    // busy, wait
        }
        UART4_CR1 |= (USART_CR1_TXEIE);                                  // ensure Tx interrupt is enabled
    #if defined UART_EXTENDED_MODE && defined SERIAL_MULTIDROP_TX        // {18a}
        UART4_TDR = ((ucExtendedWithTx[channel] << 8) | ucTxByte);       // send the byte with extended bits
    #else
        UART4_TDR = ucTxByte;                                            // send the byte
    #endif
    #if defined _WINDOWS
        UART4_ISR &= ~USART_ISR_TXE;
        iInts |= CHANNEL_3_SERIAL_INT;                                   // simulate interrupt
    #endif
        break;
#endif
#if UARTS_AVAILABLE > 1
    case 4:                                                              // UART 5
        if ((UART5_ISR & USART_ISR_TXE) == 0) {
            return 1;                                                    // busy, wait
        }
        UART5_CR1 |= (USART_CR1_TXEIE);                                  // ensure Tx interrupt is enabled
    #if defined UART_EXTENDED_MODE && defined SERIAL_MULTIDROP_TX        // {18a}
        UART5_TDR = ((ucExtendedWithTx[channel] << 8) | ucTxByte);       // send the byte with extended bits
    #else
        UART5_TDR = ucTxByte;                                            // send the byte
    #endif
    #if defined _WINDOWS
        UART5_ISR &= ~USART_ISR_TXE;
        iInts |= CHANNEL_4_SERIAL_INT;                                   // simulate interrupt
    #endif
        break;
#endif
#if defined _STM32F2XX || defined _STM32F4XX || defined _STM32F7XX
    case 5:                                                              // USART 6
        if ((USART6_ISR & USART_ISR_TXE) == 0) {
            return 1;                                                    // busy, wait
        }
        USART6_CR1 |= (USART_CR1_TXEIE);                                 // ensure Tx interrupt is enabled
    #if defined UART_EXTENDED_MODE && defined SERIAL_MULTIDROP_TX        // {18a}
        USART6_TDR = ((ucExtendedWithTx[channel] << 8) | ucTxByte);      // send the byte with extended bits
    #else
        USART6_TDR = ucTxByte;                                           // send the byte
    #endif
    #if defined _WINDOWS
        USART6_ISR &= ~USART_ISR_TXE;
        iInts |= CHANNEL_5_SERIAL_INT;                                   // simulate interrupt
    #endif
        break;
#elif defined _STM32L4X5 || defined _STM32L4X6
    case 5:                                                              // LPUART1
        if ((LPUART1_ISR & USART_ISR_TXE) == 0) {
            return 1;                                                    // busy, wait
        }
        LPUART1_CR1 |= (USART_CR1_TXEIE);                                // ensure Tx interrupt is enabled
    #if defined UART_EXTENDED_MODE && defined SERIAL_MULTIDROP_TX        // {18a}
        LPUART1_TDR = ((ucExtendedWithTx[channel] << 8) | ucTxByte);     // send the byte with extended bits
    #else
        LPUART1_TDR = ucTxByte;                                          // send the byte
    #endif
    #if defined _WINDOWS
        LPUART1_ISR &= ~USART_ISR_TXE;
        iInts |= CHANNEL_5_SERIAL_INT;                                   // simulate interrupt
    #endif
        break;
#endif
#if defined _STM32F7XX || defined _STM32F429 || defined _STM32F427       // {2}
    case 6:                                                              // UART 7
        if ((UART7_ISR & USART_ISR_TXE) == 0) {
            return 1;                                                    // busy, wait
        }
        UART7_CR1 |= (USART_CR1_TXEIE);                                  // ensure Tx interrupt is enabled
    #if defined UART_EXTENDED_MODE && defined SERIAL_MULTIDROP_TX        // {18a}
        UART7_TDR = ((ucExtendedWithTx[channel] << 8) | ucTxByte);       // send the byte with extended bits
    #else
        UART7_TDR = ucTxByte;                                            // send the byte
    #endif
    #if defined _WINDOWS
        UART7_ISR &= ~USART_ISR_TXE;
        iInts |= CHANNEL_6_SERIAL_INT;                                   // simulate interrupt
    #endif
        break;
    case 7:                                                              // UART 8
        if ((UART8_ISR & USART_ISR_TXE) == 0) {
            return 1;                                                    // busy, wait
        }
        UART8_CR1 |= (USART_CR1_TXEIE);                                  // ensure Tx interrupt is enabled
    #if defined UART_EXTENDED_MODE && defined SERIAL_MULTIDROP_TX        // {18a}
        UART8_TDR = ((ucExtendedWithTx[channel] << 8) | ucTxByte);       // send the byte with extended bits
    #else
        UART8_TDR = ucTxByte;                                            // send the byte
    #endif
    #if defined _WINDOWS
        UART8_ISR &= ~USART_ISR_TXE;
        iInts |= CHANNEL_7_SERIAL_INT;                                   // simulate interrupt
    #endif
        break;
#endif
    default:
        return 1;
    }
    return 0;                                                            // OK - byte sent
}

#if defined SUPPORT_HW_FLOW
static unsigned char ucRS485Mode = 0;

// This routine presently only supports the control of RS485 (RTS) via port line (not RTS as peripheral)
//
static void fnSetRTS(QUEUE_HANDLE channel, int iState)
{
    unsigned char ucChannelMode = (1 << channel);
    switch (channel) {
    case 0:
        if (iState != 0) {
            if ((ucChannelMode & ucRS485Mode) != 0) {
                _CLEARBITS(A, PORTA_BIT12);                              // assert RTS
            }
            else {
            }
        }
        else {
            if ((ucChannelMode & ucRS485Mode) != 0) {
                _SETBITS(A, PORTA_BIT12);                                // negate RTS
            }
            else {
            }
        }            
        break;
    case 1:
        if (iState != 0) {
            if ((ucChannelMode & ucRS485Mode) != 0) {
    #if defined USART2_REMAP
                _CLEARBITS(D, PORTD_BIT4);                               // assert RTS
    #else
                _CLEARBITS(A, PORTA_BIT1);                               // assert RTS                
    #endif
            }
            else {
            }
        }
        else {
            if ((ucChannelMode & ucRS485Mode) != 0) {
    #if defined USART2_REMAP
                _SETBITS(D, PORTD_BIT4);                                 // negate RTS
    #else
                _SETBITS(A, (PORTA_BIT1);                                // negate RTS                
    #endif
            }
            else {
            }
        }            
        break;
    case 2:
        if (iState != 0) {
            if ((ucChannelMode & ucRS485Mode) != 0) {
    #if defined USART3_FULL_REMAP
                _CLEARBITS(D, PORTD_BIT12);                              // assert RTS
    #elif defined USART3_PARTIAL_REMAP
                _CLEARBITS(B, PORTB_BIT14);                              // assert RTS
    #else
                _CLEARBITS(B, PORTB_BIT14);                              // assert RTS
    #endif
            }
            else {
            }
        }
        else {
            if ((ucChannelMode & ucRS485Mode) != 0) {
    #if defined USART3_FULL_REMAP
                _SETBITS(D, PORTD_BIT12);                                // negate RTS
    #elif defined USART3_PARTIAL_REMAP
                _SETBITS(B, PORTB_BIT14);                                // negate RTS
    #else
                _SETBITS(B, PORTB_BIT14);                                // negate RTS
    #endif
            }
            else {
            }
        }            
        break;
    case 3:                                                              // UART4 - no support in UART for peripheral function
    case 4:                                                              // UART5 - no support in UART for peripheral function
        break;
#if defined _STM32F2XX || defined _STM32F4XX || defined _STM32F7XX
    case 5:
        if (iState != 0) {
            if ((ucChannelMode & ucRS485Mode) != 0) {
    #if defined USART6_REMAP
                _CLEARBITS(G, PORTG_BIT12);                              // assert RTS
    #else
                _CLEARBITS(G, PORTG_BIT8);                               // assert RTS                
    #endif
            }
            else {
            }
        }
        else {
            if ((ucChannelMode & ucRS485Mode) != 0) {
    #if defined USART6_REMAP
                _SETBITS(G, PORTG_BIT12);                                // negate RTS
    #else
                _SETBITS(G, PORTG_BIT8);                                 // negate RTS                
    #endif
            }
            else {
            }
        }    
        break;
#endif
#if defined _STM32F7XX || defined _STM32F429 || defined _STM32F427       // {2}
    case 6:
        if (iState != 0) {
            if ((ucChannelMode & ucRS485Mode) != 0) {
              //_CLEARBITS(G, PORTG_BIT8);                               // assert RTS                
            }
            else {
            }
        }
        else {
            if ((ucChannelMode & ucRS485Mode) != 0) {
              //_SETBITS(G, PORTG_BIT8);                                 // negate RTS                
            }
            else {
            }
        }    
        break;
    case 7:
        if (iState != 0) {
            if ((ucChannelMode & ucRS485Mode) != 0) {
                //_CLEARBITS(G, PORTG_BIT8);                               // assert RTS                
            }
            else {
            }
        }
        else {
            if ((ucChannelMode & ucRS485Mode) != 0) {
                //_SETBITS(G, PORTG_BIT8);                                 // negate RTS                
            }
            else {
            }
        }
        break;
#endif
    default:
        break;
    }
}

// This routine controls the configuration and state of the RTS line as peripheral or RS485 port
//
extern void fnControlLine(QUEUE_HANDLE channel, unsigned short usModifications, UART_MODE_CONFIG OperatingMode)
{
    if ((usModifications & (CONFIG_RTS_PIN | CONFIG_CTS_PIN)) != 0) {
        if ((usModifications & CONFIG_RTS_PIN) != 0) {
            if ((usModifications & SET_RS485_MODE) != 0) {
                ucRS485Mode |= (1 << channel);
            }
            else {
                ucRS485Mode &= ~(1 << channel);
            }
            switch (channel) {
            case 0:                                                      // USART 1
                if ((usModifications & SET_RS485_MODE) != 0) {
                    _CONFIG_PORT_OUTPUT(A, PORTA_BIT12, (OUTPUT_MEDIUM | OUTPUT_PUSH_PULL)); // control as port instead of using automatic RTS line
                }
                else {
                    _CONFIG_PERIPHERAL_OUTPUT(A, (PERIPHERAL_USART1_2_3), (PORTA_BIT12), (OUTPUT_MEDIUM | OUTPUT_PUSH_PULL)); // USART1 RTS
                }
                break;
            case 1:                                                      // USART 2
    #if defined USART2_REMAP
                if ((usModifications & SET_RS485_MODE) != 0) {
                    _CONFIG_PORT_OUTPUT(D, PORTD_BIT4, (OUTPUT_MEDIUM | OUTPUT_PUSH_PULL)); // control as port instead of using automatic RTS line
                }
                else {
                    _PERIPHERAL_REMAP(USART1_REMAPPED);
                    _CONFIG_PERIPHERAL_OUTPUT(D, (PERIPHERAL_USART1_2_3), (PORTD_BIT4), (OUTPUT_MEDIUM | OUTPUT_PUSH_PULL));
                }
    #else
                if ((usModifications & SET_RS485_MODE) != 0) {
                    _CONFIG_PORT_OUTPUT(A, PORTA_BIT1, (OUTPUT_MEDIUM | OUTPUT_PUSH_PULL)); // control as port instead of using automatic RTS line
                }
                else {
                    _CONFIG_PERIPHERAL_OUTPUT(A, (PERIPHERAL_USART1_2_3), (PORTA_BIT1), (OUTPUT_MEDIUM | OUTPUT_PUSH_PULL)); // USART2 RTS
                }
    #endif
                break;
            case 2:                                                      // USART 3
    #if defined USART3_FULL_REMAP
                if ((usModifications & SET_RS485_MODE) != 0) {
                    _CONFIG_PORT_OUTPUT(D, PORTD_BIT12, (OUTPUT_MEDIUM | OUTPUT_PUSH_PULL)); // control as port instead of using automatic RTS line
                }
                else {
                    _PERIPHERAL_REMAP(USART3_FULLY_REMAPPED);
                    _CONFIG_PERIPHERAL_OUTPUT(D, (PERIPHERAL_USART1_2_3), (PORTD_BIT12), (OUTPUT_MEDIUM | OUTPUT_PUSH_PULL));
                }
    #else
                if ((usModifications & SET_RS485_MODE) != 0) {
                    _CONFIG_PORT_OUTPUT(B, PORTB_BIT14, (OUTPUT_MEDIUM | OUTPUT_PUSH_PULL)); // control as port instead of using automatic RTS line
                }
                else {
                    _CONFIG_PERIPHERAL_OUTPUT(B, (PERIPHERAL_USART1_2_3), (PORTB_BIT14), (OUTPUT_MEDIUM | OUTPUT_PUSH_PULL)); // USART3 RTS
                }
    #endif
                break;
            case 3:                                                      // UART 4 - no support in UART for peripheral function
            case 4:                                                      // UART 5 - no support in UART for peripheral function
                break;
    #if defined _STM32F2XX || defined _STM32F4XX || defined _STM32F7XX
            case 5:                                                      // USART 6
        #if defined USART6_REMAP
                if ((usModifications & SET_RS485_MODE) != 0) {
                    _CONFIG_PORT_OUTPUT(G, (PORTG_BIT12), (OUTPUT_MEDIUM | OUTPUT_PUSH_PULL));
                }
                else {
                    _CONFIG_PERIPHERAL_OUTPUT(G, (PERIPHERAL_USART4_5_6), (PORTG_BIT12), (OUTPUT_MEDIUM | OUTPUT_PUSH_PULL));
                }
        #else
                if ((usModifications & SET_RS485_MODE) != 0) {
                    _CONFIG_PORT_OUTPUT(G, (PORTG_BIT8), (OUTPUT_MEDIUM | OUTPUT_PUSH_PULL));
                }
                else {
                    _CONFIG_PERIPHERAL_OUTPUT(G, (PERIPHERAL_USART4_5_6), (PORTG_BIT8), (OUTPUT_MEDIUM | OUTPUT_PUSH_PULL)); // USART6 RTS
                }
        #endif
                break;
    #endif
    #if defined _STM32F7XX || defined _STM32F429 || defined _STM32F427   // {2}
            case 6:                                                      // UART 7
                if ((usModifications & SET_RS485_MODE) != 0) {
                  //_CONFIG_PORT_OUTPUT(G, (PORTG_BIT8), (OUTPUT_MEDIUM | OUTPUT_PUSH_PULL));
                }
                else {
                  //_CONFIG_PERIPHERAL_OUTPUT(G, (PERIPHERAL_USART4_5_6), (PORTG_BIT8), (OUTPUT_MEDIUM | OUTPUT_PUSH_PULL)); // USART6 RTS
                }
                break;
            case 7:                                                      // UART 8
                if ((usModifications & SET_RS485_MODE) != 0) {
                    //_CONFIG_PORT_OUTPUT(G, (PORTG_BIT8), (OUTPUT_MEDIUM | OUTPUT_PUSH_PULL));
                }
                else {
                    //_CONFIG_PERIPHERAL_OUTPUT(G, (PERIPHERAL_USART4_5_6), (PORTG_BIT8), (OUTPUT_MEDIUM | OUTPUT_PUSH_PULL)); // USART6 RTS
                }
                break;
    #endif
            }
        }
        if ((usModifications & CONFIG_CTS_PIN) != 0) {
            switch (channel) {
            case 0:                                                      // USART 1
                _CONFIG_PERIPHERAL_INPUT(A, (PERIPHERAL_USART1_2_3), (PORTA_BIT11), UART_CTS_INPUT_TYPE); // USART1 CTS
                break;
            case 1:                                                      // USART 2
    #if defined USART2_REMAP
                _PERIPHERAL_REMAP(USART1_REMAPPED);
                _CONFIG_PERIPHERAL_INPUT(D, (PERIPHERAL_USART1_2_3), (PORTD_BIT3), UART_CTS_INPUT_TYPE);
    #else
                _CONFIG_PERIPHERAL_INPUT(A, (PERIPHERAL_USART1_2_3), (PORTA_BIT0), UART_CTS_INPUT_TYPE); // USART2 CTS
    #endif
                break;
            case 2:                                                      // USART 3
    #if defined USART3_FULL_REMAP
                _PERIPHERAL_REMAP(USART3_FULLY_REMAPPED);
                _CONFIG_PERIPHERAL_INPUT(D, (PERIPHERAL_USART1_2_3), (PORTD_BIT11), UART_CTS_INPUT_TYPE);
    #else
                _CONFIG_PERIPHERAL_INPUT(B, (PERIPHERAL_USART1_2_3), (PORTB_BIT13), UART_CTS_INPUT_TYPE); // USART3 CTS
    #endif
                break;
            case 3:                                                      // UART 4 - no support in UART for peripheral function
            case 4:                                                      // UART 5 - no support in UART for peripheral function
                return;
    #if defined _STM32F2XX || defined _STM32F4XX || defined _STM32F7XX
            case 5:                                                      // USART 6
        #if defined USART6_REMAP
                _CONFIG_PERIPHERAL_INPUT(G, (PERIPHERAL_USART4_5_6), (PORTG_BIT15), (UART_CTS_INPUT_TYPE));
        #else
                _CONFIG_PERIPHERAL_INPUT(G, (PERIPHERAL_USART4_5_6), (PORTG_BIT13), (UART_CTS_INPUT_TYPE)); // USART6 CTS
        #endif
                break;
    #endif
    #if defined _STM32F7XX || defined _STM32F429 || defined _STM32F427   // {2}
            case 6:                                                      // UART 6 - no support in UART for peripheral function
            case 7:                                                      // UART 7 - no support in UART for peripheral function
                return;
    #endif
            }
        }
    }

    if ((usModifications & (SET_RTS | SET_CTS)) != 0) {                  // SET_CTS may be used in DCE mode and results in the same action as SET_RTS
        fnSetRTS(channel, 1);                                            // drive asserted RTS
    }
    else if ((usModifications & (CLEAR_RTS | CLEAR_CTS)) != 0) {         // deactivate RTS
        fnSetRTS(channel, 0);                                            // drive negated RTS
    }
}

// Modify control line interrupt operation (this is called from entry_tty() with disabled interrupts)
//
extern QUEUE_TRANSFER fnControlLineInterrupt(QUEUE_HANDLE channel, unsigned short usModifications, UART_MODE_CONFIG OperatingMode)
{
    return SET_CTS;                                                      // report that the CTS is asserted
}
#endif
