/***********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher
    
    ---------------------------------------------------------------------
    File:      kinetis_UART.h
    Project:   Single Chip Embedded Internet
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2017
    *********************************************************************
    03.03.2012 Add K70 UART2 alternative port mapping                    {3}
    05.04.2012 Add UART DMA support for Tx                               {6}
    06.04.2012 Modify control of RTS line when not using automatic RTS control on transmission {7}
    06.04.2012 Add UART DMA support for Rx                               {8}
    08.05.2012 Add UART3 option on port F for K70                        {12}
    24.06.2012 Add SERIAL_SUPPORT_DMA_RX_FREERUN support                 {15}
    24.06.2012 Correct some RTS/CTS pins                                 {16}
    14.09.2012 Add UART alternative port options to K61                  {25}
    23.10.2013 Ensure source address remains stable in free-running UART mode {56}
    06.05.2014 Add KL DMA based UART transmission                        {81}
    05.07.2014 Rework of UART interrupt handler including receiver overrun checking {92}
    19.07.2014 Add UART low power stop mode support                      {96}
    17.11.2014 Add KL03 LPUART support                                   {106}
    04.12.2014 Ensure source and destination addresses remain stable in UART tx DMA mode {107}
    24.03.2015 Peripheral drivers removed to their own include files
    27.08.2015 Add Kinetis KE/KL RTS control via GPIO and option to use the transmit complete interrupt for accurate end of frame call-back (UART_FRAME_END_COMPLETE) {200}
    10.10.2015 Correct KL Baud clock calculation rounding and configure the IRC clock source when selecting it for LPUART use {201}
    17.11.2015 Mask out LPUART clock source when setting IRC48M in case it has already been adjusted by internal ROM boot operation {202}
    01.12.2015 Avoid disturbing DMA when modifying UART configurations on the fly {203}
    06.02.2016 Correct LPUART parity polarity and avoid possible transmit interrupt loss when changing mode on the fly {204}
    20.12.2016 Add break support                                         {205}
    04.01.2017 Don't adjust the RC clock setting when the processor is running from it {206}
    05.01.2017 Add optional midi baud rate                               {207}
    07.01.2017 Add UART_TIMED_TRANSMISSION support for parts with DMA    {208}

*/

#if defined SERIAL_INTERFACE && (NUMBER_EXTERNAL_SERIAL > 0)
    #if defined EXT_UART_SC16IS7XX
        #include "spi_sc16IS7xx.h"                                       // include driver
    #endif
#endif

#if !defined UART_PULL_UPS
    #define UART_PULL_UPS 0
#endif

/* =================================================================== */
/*                             constants                               */
/* =================================================================== */

    #if LPUARTS_AVAILABLE > 0 && UARTS_AVAILABLE > 0                     // device contains both UART and LPUART
#define UART_TYPE_LPUART 0
#define UART_TYPE_UART   1
static const unsigned char uart_type[LPUARTS_AVAILABLE + UARTS_AVAILABLE] = {
        #if defined LPUARTS_PARALLEL
    UART_TYPE_UART,                                                      // UART0
            #if UARTS_AVAILABLE > 1
    UART_TYPE_UART,                                                      // UART1
            #endif
            #if UARTS_AVAILABLE > 2
    UART_TYPE_UART,                                                      // UART2
            #endif
            #if UARTS_AVAILABLE > 3
    UART_TYPE_UART,                                                      // UART1
            #endif
            #if UARTS_AVAILABLE > 4
    UART_TYPE_UART,                                                      // UART2
            #endif
    UART_TYPE_LPUART,                                                    // LPUART0 (numbered starting with UARTS_AVAILABLE)
        #else
    UART_TYPE_LPUART,                                                    // LPUART0
            #if LPUARTS_AVAILABLE > 1
    UART_TYPE_LPUART,                                                    // LPUART1
            #endif
            #if LPUARTS_AVAILABLE > 2
    UART_TYPE_LPUART,                                                    // LPUART2
            #endif
            #if LPUARTS_AVAILABLE > 3
    UART_TYPE_LPUART,                                                    // LPUART3
            #endif
    UART_TYPE_UART,                                                      // UART(LPUART)
        #endif
};
    #endif

    #if defined SERIAL_SUPPORT_DMA                                       // {6}
static const unsigned char UART_DMA_TX_CHANNEL[UARTS_AVAILABLE + LPUARTS_AVAILABLE] = {
    DMA_UART0_TX_CHANNEL, 
        #if (UARTS_AVAILABLE + LPUARTS_AVAILABLE) > 1
    DMA_UART1_TX_CHANNEL,
        #endif
        #if (UARTS_AVAILABLE + LPUARTS_AVAILABLE) > 2
    DMA_UART2_TX_CHANNEL, 
        #endif
        #if (UARTS_AVAILABLE + LPUARTS_AVAILABLE) > 3
    DMA_UART3_TX_CHANNEL,
        #endif
        #if (UARTS_AVAILABLE + LPUARTS_AVAILABLE) > 4
    DMA_UART4_TX_CHANNEL,
        #endif
        #if (UARTS_AVAILABLE + LPUARTS_AVAILABLE) > 5
    DMA_UART5_TX_CHANNEL,
        #endif
};

static const unsigned char UART_DMA_TX_INT_PRIORITY[UARTS_AVAILABLE + LPUARTS_AVAILABLE] = {
    DMA_UART0_TX_INT_PRIORITY, 
        #if (UARTS_AVAILABLE + LPUARTS_AVAILABLE) > 1
    DMA_UART1_TX_INT_PRIORITY, 
        #endif
        #if (UARTS_AVAILABLE + LPUARTS_AVAILABLE) > 2
    DMA_UART2_TX_INT_PRIORITY, 
        #endif
        #if (UARTS_AVAILABLE + LPUARTS_AVAILABLE) > 3
    DMA_UART3_TX_INT_PRIORITY,
        #endif
        #if (UARTS_AVAILABLE + LPUARTS_AVAILABLE) > 4
    DMA_UART4_TX_INT_PRIORITY,
        #endif
        #if (UARTS_AVAILABLE + LPUARTS_AVAILABLE) > 5
    DMA_UART5_TX_INT_PRIORITY,
        #endif
};
    #endif

     #if defined SERIAL_SUPPORT_DMA_RX
static const unsigned char UART_DMA_RX_CHANNEL[UARTS_AVAILABLE + LPUARTS_AVAILABLE] = {
        DMA_UART0_RX_CHANNEL, 
        #if (UARTS_AVAILABLE + LPUARTS_AVAILABLE) > 1
        DMA_UART1_RX_CHANNEL, 
        #endif
        #if (UARTS_AVAILABLE + LPUARTS_AVAILABLE) > 2
        DMA_UART2_RX_CHANNEL, 
        #endif
        #if (UARTS_AVAILABLE + LPUARTS_AVAILABLE) > 3
        DMA_UART3_RX_CHANNEL,
        #endif
        #if (UARTS_AVAILABLE + LPUARTS_AVAILABLE) > 4
        DMA_UART4_RX_CHANNEL,
        #endif
        #if (UARTS_AVAILABLE + LPUARTS_AVAILABLE) > 5
        DMA_UART5_RX_CHANNEL
        #endif
};

static const unsigned char UART_DMA_RX_INT_PRIORITY[UARTS_AVAILABLE + LPUARTS_AVAILABLE] = {
        DMA_UART0_RX_INT_PRIORITY, 
        #if (UARTS_AVAILABLE + LPUARTS_AVAILABLE) > 1
        DMA_UART1_RX_INT_PRIORITY, 
        #endif
        #if (UARTS_AVAILABLE + LPUARTS_AVAILABLE) > 2
        DMA_UART2_RX_INT_PRIORITY, 
        #endif
        #if (UARTS_AVAILABLE + LPUARTS_AVAILABLE) > 3
        DMA_UART3_RX_INT_PRIORITY,
        #endif
        #if (UARTS_AVAILABLE + LPUARTS_AVAILABLE) > 4
        DMA_UART4_RX_INT_PRIORITY,
        #endif
        #if (UARTS_AVAILABLE + LPUARTS_AVAILABLE) > 5
        DMA_UART5_RX_INT_PRIORITY
        #endif
};
    #endif

/* =================================================================== */
/*                      local variable definitions                     */
/* =================================================================== */

static unsigned char ucUART_mask[UARTS_AVAILABLE + LPUARTS_AVAILABLE] = {0};
#if defined TRUE_UART_TX_2_STOPS
    static unsigned char ucStops[UARTS_AVAILABLE + LPUARTS_AVAILABLE] = {0};
#endif
#if defined SERIAL_INTERFACE && defined SERIAL_SUPPORT_DMA_RX && defined SERIAL_SUPPORT_DMA_RX_FREERUN // {15}
    unsigned short usDMA_progress[UARTS_AVAILABLE];
#endif
#if defined SUPPORT_HW_FLOW && (defined KINETIS_KE || defined KINETIS_KL)
    static unsigned char ucRTS_neg[UARTS_AVAILABLE + LPUARTS_AVAILABLE] = {0};
#endif
#if (defined KINETIS_KL || defined KINETIS_KE) && defined UART_FRAME_END_COMPLETE
    static unsigned char ucReportEndOfFrame[UARTS_AVAILABLE + LPUARTS_AVAILABLE] = {0};
#endif
#if defined UART_TIMED_TRANSMISSION                                      // {208}
    static unsigned long ulInterCharTxDelay[UARTS_AVAILABLE + LPUARTS_AVAILABLE] = {0};
    static void fnStopTxTimer(int Channel);
#endif



/* =================================================================== */
/*                    Serial Interface - UART                          */
/* =================================================================== */


// Collect a pointer to defined UART control block
//
static void *fnSelectChannel(QUEUE_HANDLE Channel)
{
    static const void *uart_control_block[UARTS_AVAILABLE + LPUARTS_AVAILABLE] = {
    #if UARTS_AVAILABLE > 0 && (LPUARTS_AVAILABLE < 1 || defined LPUARTS_PARALLEL)
                                                              (void *)UART0_BLOCK,
    #endif
    #if UARTS_AVAILABLE > 1 && (LPUARTS_AVAILABLE < 2 || defined LPUARTS_PARALLEL)
                                                              (void *)UART1_BLOCK,
    #endif
    #if UARTS_AVAILABLE > 2 && (LPUARTS_AVAILABLE < 3 || defined LPUARTS_PARALLEL)
                                                              (void *)UART2_BLOCK,
    #endif
    #if UARTS_AVAILABLE > 3 && (LPUARTS_AVAILABLE < 4 || defined LPUARTS_PARALLEL)
                                                              (void *)UART3_BLOCK,
    #endif
    #if UARTS_AVAILABLE > 4 && (LPUARTS_AVAILABLE < 5 || defined LPUARTS_PARALLEL)
                                                              (void *)UART4_BLOCK,
    #endif
    #if UARTS_AVAILABLE > 5 && (LPUARTS_AVAILABLE < 6 || defined LPUARTS_PARALLEL)
                                                              (void *)UART5_BLOCK,
    #endif
    #if LPUARTS_AVAILABLE > 0
                                                              (void *)LPUART0_BLOCK,
    #endif
    #if LPUARTS_AVAILABLE > 1
                                                              (void *)LPUART1_BLOCK,
    #endif
    #if LPUARTS_AVAILABLE == 2 && UARTS_AVAILABLE == 1        // KL43
                                                              (void *)UART2_BLOCK,
    #elif LPUARTS_AVAILABLE > 2
                                                              (void *)LPUART2_BLOCK,
    #endif
    #if LPUARTS_AVAILABLE > 3
                                                              (void *)LPUART3_BLOCK,
    #endif
    #if LPUARTS_AVAILABLE > 4
                                                              (void *)LPUART4_BLOCK,
    #endif
    };
    return (void *)uart_control_block[Channel];
}


/* =================================================================== */
/*                     LPUART interrupt handlers                       */
/* =================================================================== */

    #if LPUARTS_AVAILABLE > 0
// LPUART 0 interrupt handler
//
static __interrupt void _LPSCI0_Interrupt(void)                          // LPUART 0 interrupt
{
        #if defined LPUARTS_PARALLEL
            #define LPUART0_CH_NUMBER     UARTS_AVAILABLE
        #else
            #define LPUART0_CH_NUMBER     0
        #endif
    unsigned long ulState = LPUART0_STAT;                                // status register on entry to the interrupt routine
    if (((ulState & LPUART_STAT_RDRF) & LPUART0_CTRL) != 0) {            // reception interrupt flag is set and the reception interrupt is enabled
        fnSciRxByte((unsigned char)(LPUART0_DATA & ucUART_mask[LPUART0_CH_NUMBER]), LPUART0_CH_NUMBER); // receive data interrupt - read the byte (masked with character width)
        #if defined _WINDOWS
        LPUART0_STAT &= ~(LPUART_STAT_RDRF);                             // simulate reset of interrupt flag
        #endif
        ulState = LPUART0_STAT;                                          // update the status register
        if ((ulState & LPUART_STAT_OR) != 0) {                           // if the overrun flag is set at this point it means that an overrun took place between reading the status register on entry to the interrupt and reading the data register
            (void)LPUART0_DATA;                                          // read the data register in order to clear the overrun flag and allow the receiver to continue operating
        }
    }

    if (((ulState & LPUART_STAT_TDRE) & LPUART0_CTRL) != 0) {            // transmit buffer is empty and the transmit interrupt is enabled
        fnSciTxByte(LPUART0_CH_NUMBER);                                  // transmit data empty interrupt - write next byte, if waiting
    }
        #if defined SUPPORT_LOW_POWER
    if (((LPUART0_STAT & LPUART_STAT_TC) & LPUART0_CTRL) != 0) {         // transmit complete interrupt after final byte transmission together with low power operation
        LPUART0_CTRL &= ~(LPUART_CTRL_TCIE);                             // disable the interrupt
        ulPeripheralNeedsClock &= ~(UART0_TX_CLK_REQUIRED << LPUART0_CH_NUMBER); // confirmation that the final byte has been sent out on the line so the UART no longer needs a UART clock (stop mode doesn't needed to be blocked)
    }
        #endif
}
    #endif

    #if LPUARTS_AVAILABLE > 1
// LPUART 1 interrupt handler
//
static __interrupt void _LPSCI1_Interrupt(void)                          // LPUART 1 interrupt
{
        #if defined LPUARTS_PARALLEL
            #define LPUART1_CH_NUMBER     (UARTS_AVAILABLE + 1)
        #else
            #define LPUART1_CH_NUMBER     1
        #endif
    unsigned long ulState = LPUART1_STAT;                                // status register on entry to the interrupt routine
    if (((ulState & LPUART_STAT_RDRF) & LPUART1_CTRL) != 0) {            // reception interrupt flag is set and the reception interrupt is enabled
        fnSciRxByte((unsigned char)(LPUART1_DATA & ucUART_mask[LPUART1_CH_NUMBER]), LPUART1_CH_NUMBER); // receive data interrupt - read the byte (masked with character width)
        #if defined _WINDOWS
        LPUART1_STAT &= ~(LPUART_STAT_RDRF);                             // simulate reset of interrupt flag
        #endif
        ulState = LPUART1_STAT;                                          // update the status register
        if ((ulState & LPUART_STAT_OR) != 0) {                           // if the overrun flag is set at this point it means that an overrun took place between reading the status register on entry to the interrupt and reading the data register
            (void )LPUART1_DATA;                                         // read the data register in order to clear the overrun flag and allow the receiver to continue operating
        }
    }

    if (((ulState & LPUART_STAT_TDRE) & LPUART1_CTRL) != 0) {            // transmit buffer is empty and the transmit interrupt is enabled
        fnSciTxByte(LPUART1_CH_NUMBER);                                  // transmit data empty interrupt - write next byte, if waiting
    }
        #if defined SUPPORT_LOW_POWER
    if (((LPUART1_STAT & LPUART_STAT_TC) & LPUART1_CTRL) != 0) {         // transmit complete interrupt after final byte transmission together with low power operation
        LPUART1_CTRL &= ~(LPUART_CTRL_TCIE);                             // disable the interrupt
        ulPeripheralNeedsClock &= ~(UART0_TX_CLK_REQUIRED << LPUART1_CH_NUMBER); // confirmation that the final byte has been sent out on the line so the UART no longer needs a UART clock (stop mode doesn't needed to be blocked)
    }
        #endif
}
    #endif

    #if LPUARTS_AVAILABLE > 2
// LPUART 2 interrupt handler
//
static __interrupt void _LPSCI2_Interrupt(void)                          // LPUART 2 interrupt
{
        #if defined LPUARTS_PARALLEL
            #define LPUART2_CH_NUMBER     (UARTS_AVAILABLE + 2)
        #else
            #define LPUART2_CH_NUMBER     2
        #endif
    unsigned long ulState = LPUART2_STAT;                                // status register on entry to the interrupt routine
    if (((ulState & LPUART_STAT_RDRF) & LPUART2_CTRL) != 0) {            // reception interrupt flag is set and the reception interrupt is enabled
        fnSciRxByte((unsigned char)(LPUART2_DATA & ucUART_mask[LPUART2_CH_NUMBER]), LPUART2_CH_NUMBER); // receive data interrupt - read the byte (masked with character width)
        #if defined _WINDOWS
        LPUART2_STAT &= ~(LPUART_STAT_RDRF);                             // simulate reset of interrupt flag
        #endif
        ulState = LPUART2_STAT;                                          // update the status register
        if ((ulState & LPUART_STAT_OR) != 0) {                           // if the overrun flag is set at this point it means that an overrun took place between reading the status register on entry to the interrupt and reading the data register
            (void )LPUART2_DATA;                                         // read the data register in order to clear the overrun flag and allow the receiver to continue operating
        }
    }

    if (((ulState & LPUART_STAT_TDRE) & LPUART2_CTRL) != 0) {            // transmit buffer is empty and the transmit interrupt is enabled
        fnSciTxByte(LPUART2_CH_NUMBER);                                  // transmit data empty interrupt - write next byte, if waiting
    }
        #if defined SUPPORT_LOW_POWER
    if (((LPUART2_STAT & LPUART_STAT_TC) & LPUART2_CTRL) != 0) {         // transmit complete interrupt after final byte transmission together with low power operation
        LPUART2_CTRL &= ~(LPUART_CTRL_TCIE);                             // disable the interrupt
        ulPeripheralNeedsClock &= ~(UART0_TX_CLK_REQUIRED << LPUART2_CH_NUMBER); // confirmation that the final byte has been sent out on the line so the UART no longer needs a UART clock (stop mode doesn't needed to be blocked)
    }
        #endif
}
    #endif

    #if LPUARTS_AVAILABLE > 3
// LPUART 3 interrupt handler
//
static __interrupt void _LPSCI3_Interrupt(void)                          // LPUART 3 interrupt
{
        #if defined LPUARTS_PARALLEL
            #define LPUART3_CH_NUMBER     (UARTS_AVAILABLE + 3)
        #else
            #define LPUART3_CH_NUMBER     3
        #endif
    unsigned long ulState = LPUART3_STAT;                                // status register on entry to the interrupt routine
    if (((ulState & LPUART_STAT_RDRF) & LPUART3_CTRL) != 0) {            // reception interrupt flag is set and the reception interrupt is enabled
        fnSciRxByte((unsigned char)(LPUART3_DATA & ucUART_mask[LPUART3_CH_NUMBER]), LPUART3_CH_NUMBER); // receive data interrupt - read the byte (masked with character width)
        #if defined _WINDOWS
        LPUART3_STAT &= ~(LPUART_STAT_RDRF);                             // simulate reset of interrupt flag
        #endif
        ulState = LPUART3_STAT;                                          // update the status register
        if ((ulState & LPUART_STAT_OR) != 0) {                           // if the overrun flag is set at this point it means that an overrun took place between reading the status register on entry to the interrupt and reading the data register
            (void )LPUART3_DATA;                                         // read the data register in order to clear the overrun flag and allow the receiver to continue operating
        }
    }

    if (((ulState & LPUART_STAT_TDRE) & LPUART3_CTRL) != 0) {            // transmit buffer is empty and the transmit interrupt is enabled
        fnSciTxByte(LPUART3_CH_NUMBER);                                  // transmit data empty interrupt - write next byte, if waiting
    }
        #if defined SUPPORT_LOW_POWER
    if (((LPUART3_STAT & LPUART_STAT_TC) & LPUART3_CTRL) != 0) {         // transmit complete interrupt after final byte transmission together with low power operation
        LPUART3_CTRL &= ~(LPUART_CTRL_TCIE);                             // disable the interrupt
        ulPeripheralNeedsClock &= ~(UART0_TX_CLK_REQUIRED << LPUART3_CH_NUMBER); // confirmation that the final byte has been sent out on the line so the UART no longer needs a UART clock (stop mode doesn't needed to be blocked)
    }
        #endif
}
    #endif

    #if LPUARTS_AVAILABLE > 4
// LPUART 4 interrupt handler
//
static __interrupt void _LPSCI4_Interrupt(void)                          // LPUART 4 interrupt
{
        #if defined LPUARTS_PARALLEL
            #define LPUART4_CH_NUMBER     (UARTS_AVAILABLE + 4)
        #else
            #define LPUART4_CH_NUMBER     4
        #endif
    unsigned long ulState = LPUART4_STAT;                                // status register on entry to the interrupt routine
    if (((ulState & LPUART_STAT_RDRF) & LPUART4_CTRL) != 0) {            // reception interrupt flag is set and the reception interrupt is enabled
        fnSciRxByte((unsigned char)(LPUART4_DATA & ucUART_mask[LPUART4_CH_NUMBER]), LPUART4_CH_NUMBER); // receive data interrupt - read the byte (masked with character width)
        #if defined _WINDOWS
        LPUART4_STAT &= ~(LPUART_STAT_RDRF);                             // simulate reset of interrupt flag
        #endif
        ulState = LPUART4_STAT;                                          // update the status register
        if ((ulState & LPUART_STAT_OR) != 0) {                           // if the overrun flag is set at this point it means that an overrun took place between reading the status register on entry to the interrupt and reading the data register
            (void )LPUART4_DATA;                                         // read the data register in order to clear the overrun flag and allow the receiver to continue operating
        }
    }

    if (((ulState & LPUART_STAT_TDRE) & LPUART4_CTRL) != 0) {            // transmit buffer is empty and the transmit interrupt is enabled
        fnSciTxByte(LPUART4_CH_NUMBER);                                  // transmit data empty interrupt - write next byte, if waiting
    }
        #if defined SUPPORT_LOW_POWER
    if (((LPUART4_STAT & LPUART_STAT_TC) & LPUART4_CTRL) != 0) {         // transmit complete interrupt after final byte transmission together with low power operation
        LPUART4_CTRL &= ~(LPUART_CTRL_TCIE);                             // disable the interrupt
        ulPeripheralNeedsClock &= ~(UART0_TX_CLK_REQUIRED << LPUART4_CH_NUMBER); // confirmation that the final byte has been sent out on the line so the UART no longer needs a UART clock (stop mode doesn't needed to be blocked)
    }
        #endif
}
    #endif


/* =================================================================== */
/*                      UART interrupt handlers                        */
/* =================================================================== */


    #if UARTS_AVAILABLE > 0 && (LPUARTS_AVAILABLE < 1 || defined LPUARTS_PARALLEL)
        #if !defined fnUART0_HANDLER                                     // default reception handler (this can be defined by user code if required)
            #define fnUART0_HANDLER(data, channel) fnSciRxByte(data, channel)
        #endif
// UART 0 interrupt handler
//
static __interrupt void _SCI0_Interrupt(void)                            // UART 0 interrupt
{
    unsigned char ucState = UART0_S1;                                    // status register on entry to the interrupt routine
        #if defined SERIAL_SUPPORT_DMA                                   // {8}
    if ((UART0_C5 & UART_C5_RDMAS) == 0) {                               // if the receiver is operating in DMA mode ignore reception interrupt flags
        #endif
        if (((ucState & UART_S1_RDRF) & UART0_C2) != 0) {                // reception interrupt flag is set and the reception interrupt is enabled
            fnUART0_HANDLER((unsigned char)(UART0_D & ucUART_mask[0]), 0); // receive data interrupt - read the byte (masked with character width)
            #if defined _WINDOWS
            UART0_S1 &= ~(UART_S1_RDRF);                                 // simulate reset of interrupt flag
            #endif
            ucState = UART0_S1;                                          // {92} update the status register
            if (ucState & UART_S1_OR) {                                  // if the overrun flag is set at this point it means that an overrun took place between reading the status register on entry to the interrupt and reading the data register
                (void)UART0_D;                                           // read the data register in order to clear the overrun flag and allow the receiver to continue operating
            }
        }
        #if defined SERIAL_SUPPORT_DMA
    }
        #endif
        #if defined SERIAL_SUPPORT_DMA                                   // {6}
    if ((UART0_C5 & UART_C5_TDMAS) == 0) {                               // if the transmitter is operating in DMA mode ignore transmission interrupt flags
        #endif
        if ((ucState & (UART_S1_TDRE | UART_S1_TC)) & UART0_C2) {        // transmit buffer or transmit is empty and the corresponding interrupt is enabled
            fnSciTxByte(0);                                              // transmit data empty interrupt - write next byte
        #if defined TRUE_UART_TX_2_STOPS && defined SUPPORT_LOW_POWER
            if (ucStops[0] != 0) {                                       // if the channel is working in true 2 stop bit mode it will always use the transmit complete interrupt and the peripheral idle control is performed in fnClearTxInt() instead
                return;
            }
        #endif
        }
        #if defined SERIAL_SUPPORT_DMA
    }
        #endif
        #if defined SUPPORT_LOW_POWER || ((defined KINETIS_KL || defined KINETIS_KE) && defined UART_FRAME_END_COMPLETE) // {96}
    if ((UART0_C2 & UART_C2_TCIE) && (UART0_S1 & UART_S1_TC)) {          // transmit complete interrupt after final byte transmission together with low power operation
        UART0_C2 &= ~(UART_C2_TCIE);                                     // disable the interrupt
        ulPeripheralNeedsClock &= ~(UART0_TX_CLK_REQUIRED);              // confirmation that the final byte has been sent out on the line so the UART no longer needs a UART clock (stop mode doesn't needed to be blocked)
            #if (defined KINETIS_KL || defined KINETIS_KE) && defined UART_FRAME_END_COMPLETE
        if (ucReportEndOfFrame[0] != 0) {                                // if the end of frame call-back is enabled
            fnUARTFrameTermination(0);                                   // {200}
        }
            #endif
    }
        #endif
}
    #endif
    #if UARTS_AVAILABLE > 1 && (LPUARTS_AVAILABLE < 2 || defined LPUARTS_PARALLEL)
        #if !defined fnUART1_HANDLER                                     // default reception handler (this can be defined by user code if required)
            #define fnUART1_HANDLER(data, channel) fnSciRxByte(data, channel)
        #endif
// UART 1 interrupt handler
//
static __interrupt void _SCI1_Interrupt(void)                            // UART 1 interrupt
{
    unsigned char ucState = UART1_S1;                                    // status register on entry to the interrupt routine

        #if defined SERIAL_SUPPORT_DMA                                   // {8}
            #if defined KINETIS_KL
    if ((UART1_C4 & UART_C4_RDMAS) == 0) 
            #else
    if ((UART1_C5 & UART_C5_RDMAS) == 0) 
            #endif
    {                                                                    // if the receiver is operating in DMA mode ignore reception interrupt flags
        #endif
        if (((ucState & UART_S1_RDRF) & UART1_C2) != 0) {                // reception interrupt flag is set and the reception interrupt is enabled
            fnUART1_HANDLER((unsigned char)(UART1_D & ucUART_mask[1]), 1); // receive data interrupt - read the byte (masked with character width)
        #if defined _WINDOWS
            UART1_S1 &= ~(UART_S1_RDRF);                                 // simulate reset of interrupt flag
        #endif
            ucState = UART1_S1;                                          // {92} update the status register
            if (ucState & UART_S1_OR) {                                  // if the overrun flag is set at this point it means that an overrun took place between reading the status register on entry to the interrupt and reading the data register
                (void)UART1_D;                                           // read the data register in order to clear the overrun flag and allow the receiver to continue operating
            }
        }
        #if defined SERIAL_SUPPORT_DMA
    }
        #endif
        #if defined SERIAL_SUPPORT_DMA                                   // {6}
            #if defined KINETIS_KL
    if ((UART1_C4 & UART_C4_TDMAS) == 0) 
            #else
    if ((UART1_C5 & UART_C5_TDMAS) == 0) 
            #endif
    {                                                                    // if the transmitter is operating in DMA mode ignore transmission interrupt flags
        #endif
        if ((ucState & (UART_S1_TDRE | UART_S1_TC)) & UART1_C2) {        // transmit buffer or transmit is empty and the corresponding interrupt is enabled
            fnSciTxByte(1);                                              // transmit data empty interrupt - write next byte
        #if defined TRUE_UART_TX_2_STOPS && defined SUPPORT_LOW_POWER
            if (ucStops[1] != 0) {                                       // if the channel is working in true 2 stop bit mode it will always use the transmit complete interrupt and the peripheral idle control is performed in fnClearTxInt() instead
                return;
            }
        #endif
        }
        #if defined SERIAL_SUPPORT_DMA
    }
        #endif
        #if defined SUPPORT_LOW_POWER                                    // {96} transmitter using DMA
    if ((UART1_C2 & UART_C2_TCIE) && (UART1_S1 & UART_S1_TC)) {          // transmit complete interrupt after final byte transmission together with low power operation
        UART1_C2 &= ~(UART_C2_TCIE);                                     // disable the interrupt
        ulPeripheralNeedsClock &= ~(UART1_TX_CLK_REQUIRED);              // confirmation that the final byte has been sent out on the line so the UART no longer needs a UART clock (stop mode doesn't needed to be blocked)
    }
        #endif
}
    #endif
    #if (UARTS_AVAILABLE > 2 && (LPUARTS_AVAILABLE < 3 || defined LPUARTS_PARALLEL)) || (UARTS_AVAILABLE == 1 && LPUARTS_AVAILABLE == 2)
        #if !defined fnUART2_HANDLER                                     // default reception handler (this can be defined by user code if required)
            #define fnUART2_HANDLER(data, channel) fnSciRxByte(data, channel)
        #endif
// UART 2 interrupt handler
//
static __interrupt void _SCI2_Interrupt(void)                            // UART 2 interrupt
{
    unsigned char ucState = UART2_S1;                                    // status register on entry to the interrupt routine

        #if defined SERIAL_SUPPORT_DMA                                   // {8}
            #if defined KINETIS_KL
    if ((UART2_C4 & UART_C4_RDMAS) == 0)
            #else
    if ((UART2_C5 & UART_C5_RDMAS) == 0)
            #endif
    {                                                                    // if the receiver is operating in DMA mode ignore reception interrupt flags
        #endif
        if ((ucState & UART_S1_RDRF) & UART2_C2) {                       // reception interrupt flag is set and the reception interrupt is enabled
            fnUART2_HANDLER((unsigned char)(UART2_D & ucUART_mask[2]), 2); // receive data interrupt - read the byte (masked with character width)
        #if defined _WINDOWS
            UART2_S1 &= ~(UART_S1_RDRF);                                 // simulate reset of interrupt flag
        #endif
            ucState = UART2_S1;                                          // {92} update the status register
            if (ucState & UART_S1_OR) {                                  // if the overrun flag is set at this point it means that an overrun took place between reading the status register on entry to the interrupt and reading the data register
                (void)UART2_D;                                           // read the data register in order to clear the overrun flag and allow the receiver to continue operating
            }
        }
        #if defined SERIAL_SUPPORT_DMA
    }
        #endif
        #if defined SERIAL_SUPPORT_DMA                                   // {6}
            #if defined KINETIS_KL
    if ((UART2_C4 & UART_C4_TDMAS) == 0)
            #else
    if ((UART2_C5 & UART_C5_TDMAS) == 0)
            #endif
    {                                                                    // if the transmitter is operating in DMA mode ignore transmission interrupt flags
        #endif
        if ((ucState & (UART_S1_TDRE | UART_S1_TC)) & UART2_C2) {        // transmit buffer or transmit is empty and the corresponding interrupt is enabled
            fnSciTxByte(2);                                              // transmit data empty interrupt - write next byte
        #if defined TRUE_UART_TX_2_STOPS && defined SUPPORT_LOW_POWER
            if (ucStops[2] != 0) {                                       // if the channel is working in true 2 stop bit mode it will always use the transmit complete interrupt and the peripheral idle control is performed in fnClearTxInt() instead
                return;
            }
        #endif
        }
        #if defined SERIAL_SUPPORT_DMA
    }
        #endif
        #if defined SUPPORT_LOW_POWER                                    // {96} transmitter using DMA
    if ((UART2_C2 & UART_C2_TCIE) && (UART2_S1 & UART_S1_TC)) {          // transmit complete interrupt after final byte transmission together with low power operation
        UART2_C2 &= ~(UART_C2_TCIE);                                     // disable the interrupt
        ulPeripheralNeedsClock &= ~(UART2_TX_CLK_REQUIRED);              // confirmation that the final byte has been sent out on the line so the UART no longer needs a UART clock (stop mode doesn't needed to be blocked)
    }
        #endif
}
    #endif
    #if UARTS_AVAILABLE > 3
        #if !defined fnUART3_HANDLER                                     // default reception handler (this can be defined by user code if required)
            #define fnUART3_HANDLER(data, channel) fnSciRxByte(data, channel)
        #endif
// UART 3 interrupt handler
//
static __interrupt void _SCI3_Interrupt(void)                            // UART 3 interrupt
{
    unsigned char ucState = UART3_S1;                                    // status register on entry to the interrupt routine

        #if defined SERIAL_SUPPORT_DMA                                   // {8}
    if ((UART3_C5 & UART_C5_RDMAS) == 0) {                               // if the receiver is operating in DMA mode ignore reception interrupt flags
        #endif
        if ((ucState & UART_S1_RDRF) & UART3_C2) {                       // reception interrupt flag is set and the reception interrupt is enabled
            fnUART3_HANDLER((unsigned char)(UART3_D & ucUART_mask[3]), 3); // receive data interrupt - read the byte (masked with character width)
        #if defined _WINDOWS
            UART3_S1 &= ~(UART_S1_RDRF);                                 // simulate reset of interrupt flag
        #endif
            ucState = UART3_S1;                                          // {92} update the status register
            if (ucState & UART_S1_OR) {                                  // if the overrun flag is set at this point it means that an overrun took place between reading the status register on entry to the interrupt and reading the data register
                (void)UART3_D;                                           // read the data register in order to clear the overrun flag and allow the receiver to continue operating
            }
        }
        #if defined SERIAL_SUPPORT_DMA
    }
        #endif
        #if defined SERIAL_SUPPORT_DMA                                   // {6}
    if ((UART3_C5 & UART_C5_TDMAS) == 0) {                               // if the transmitter is operating in DMA mode ignore transmission interrupt flags
        #endif
        if ((ucState & (UART_S1_TDRE | UART_S1_TC)) & UART3_C2) {        // transmit buffer or transmit is empty and the corresponding interrupt is enabled
            fnSciTxByte(3);                                              // transmit data empty interrupt - write next byte
        #if defined TRUE_UART_TX_2_STOPS && defined SUPPORT_LOW_POWER
            if (ucStops[3] != 0) {                                       // if the channel is working in true 2 stop bit mode it will always use the transmit complete interrupt and the peripheral idle control is performed in fnClearTxInt() instead
                return;
            }
        #endif
        }
        #if defined SERIAL_SUPPORT_DMA
    }
        #endif
        #if defined SUPPORT_LOW_POWER                                    // {96} transmitter using DMA
    if ((UART3_C2 & UART_C2_TCIE) && (UART3_S1 & UART_S1_TC)) {          // transmit complete interrupt after final byte transmission together with low power operation
        UART3_C2 &= ~(UART_C2_TCIE);                                     // disable the interrupt
        ulPeripheralNeedsClock &= ~(UART3_TX_CLK_REQUIRED);              // confirmation that the final byte has been sent out on the line so the UART no longer needs a UART clock (stop mode doesn't needed to be blocked)
    }
        #endif
}
    #endif
    #if UARTS_AVAILABLE > 4
        #if !defined fnUART4_HANDLER                                     // default reception handler (this can be defined by user code if required)
            #define fnUART4_HANDLER(data, channel) fnSciRxByte(data, channel)
        #endif
// UART 4 interrupt handler
//
static __interrupt void _SCI4_Interrupt(void)                            // UART 4 interrupt
{
    unsigned char ucState = UART4_S1;                                    // status register on entry to the interrupt routine

        #if defined SERIAL_SUPPORT_DMA                                   // {8}
    if ((UART4_C5 & UART_C5_RDMAS) == 0) {                               // if the receiver is operating in DMA mode ignore reception interrupt flags
        #endif
        if ((ucState & UART_S1_RDRF) & UART4_C2) {                       // reception interrupt flag is set and the reception interrupt is enabled
            fnUART4_HANDLER((unsigned char)(UART4_D & ucUART_mask[4]), 4); // receive data interrupt - read the byte (masked with character width)
        #if defined _WINDOWS
            UART4_S1 &= ~(UART_S1_RDRF);                                 // simulate reset of interrupt flag
        #endif
            ucState = UART4_S1;                                          // {92} update the status register
            if (ucState & UART_S1_OR) {                                  // if the overrun flag is set at this point it means that an overrun took place between reading the status register on entry to the interrupt and reading the data register
                (void)UART4_D;                                           // read the data register in order to clear the overrun flag and allow the receiver to continue operating
            }
        }
        #if defined SERIAL_SUPPORT_DMA
    }
        #endif
        #if defined SERIAL_SUPPORT_DMA                                   // {6}
    if ((UART4_C5 & UART_C5_TDMAS) == 0) {                               // if the transmitter is operating in DMA mode ignore transmission interrupt flags
        #endif
        if ((ucState & (UART_S1_TDRE | UART_S1_TC)) & UART4_C2) {        // transmit buffer or transmit is empty and the corresponding interrupt is enabled
            fnSciTxByte(4);                                              // transmit data empty interrupt - write next byte
        #if defined TRUE_UART_TX_2_STOPS && defined SUPPORT_LOW_POWER
            if (ucStops[4] != 0) {                                       // if the channel is working in true 2 stop bit mode it will always use the transmit complete interrupt and the peripheral idle control is performed in fnClearTxInt() instead
                return;
            }
        #endif
        }
        #if defined SERIAL_SUPPORT_DMA
    }
        #endif
        #if defined SUPPORT_LOW_POWER                                    // {96} transmitter using DMA
    if ((UART4_C2 & UART_C2_TCIE) && (UART4_S1 & UART_S1_TC)) {          // transmit complete interrupt after final byte transmission together with low power operation
        UART4_C2 &= ~(UART_C2_TCIE);                                     // disable the interrupt
        ulPeripheralNeedsClock &= ~(UART4_TX_CLK_REQUIRED);              // confirmation that the final byte has been sent out on the line so the UART no longer needs a UART clock (stop mode doesn't needed to be blocked)
    }
        #endif
}
    #endif
    #if UARTS_AVAILABLE > 5
        #if !defined fnUART5_HANDLER                                     // default reception handler (this can be defined by user code if required)
            #define fnUART5_HANDLER(data, channel) fnSciRxByte(data, channel)
        #endif
// UART 5 interrupt handler
//
static __interrupt void _SCI5_Interrupt(void)                            // UART 5 interrupt
{
    unsigned char ucState = UART5_S1;                                    // status register on entry to the interrupt routine

        #if defined SERIAL_SUPPORT_DMA                                   // {8}
    if ((UART5_C5 & UART_C5_RDMAS) == 0) {                               // if the receiver is operating in DMA mode ignore reception interrupt flags
        #endif
        if ((ucState & UART_S1_RDRF) & UART5_C2) {                       // reception interrupt flag is set and the reception interrupt is enabled
            fnUART5_HANDLER((unsigned char)(UART5_D & ucUART_mask[5]), 5); // receive data interrupt - read the byte (masked with character width)
        #if defined _WINDOWS
            UART5_S1 &= ~(UART_S1_RDRF);                                 // simulate reset of interrupt flag
        #endif
            ucState = UART5_S1;                                          // {92} update the status register
            if (ucState & UART_S1_OR) {                                  // if the overrun flag is set at this point it means that an overrun took place between reading the status register on entry to the interrupt and reading the data register
                (void)UART5_D;                                           // read the data register in order to clear the overrun flag and allow the receiver to continue operating
            }
        }
        #if defined SERIAL_SUPPORT_DMA
    }
        #endif
        #if defined SERIAL_SUPPORT_DMA                                   // {6}
    if ((UART5_C5 & UART_C5_TDMAS) == 0) {                               // if the transmitter is operating in DMA mode ignore transmission interrupt flags
        #endif
        if ((ucState & (UART_S1_TDRE | UART_S1_TC)) & UART5_C2) {        // transmit buffer or transmit is empty and the corresponding interrupt is enabled
            fnSciTxByte(5);                                              // transmit data empty interrupt - write next byte
        #if defined TRUE_UART_TX_2_STOPS && defined SUPPORT_LOW_POWER
            if (ucStops[5] != 0) {                                       // if the channel is working in true 2 stop bit mode it will always use the transmit complete interrupt and the peripheral idle control is performed in fnClearTxInt() instead
                return;
            }
        #endif
        }
        #if defined SERIAL_SUPPORT_DMA
    }
        #endif
        #if defined SUPPORT_LOW_POWER                                    // {96} transmitter using DMA
    if ((UART5_C2 & UART_C2_TCIE) && (UART5_S1 & UART_S1_TC)) {          // transmit complete interrupt after final byte transmission together with low power operation
        UART5_C2 &= ~(UART_C2_TCIE);                                     // disable the interrupt
        ulPeripheralNeedsClock &= ~(UART5_TX_CLK_REQUIRED);              // confirmation that the final byte has been sent out on the line so the UART no longer needs a UART clock (stop mode doesn't needed to be blocked)
    }
        #endif
}
    #endif



// The TTY driver uses this call to send a byte of data over the serial port
//
extern int fnTxByte(QUEUE_HANDLE Channel, unsigned char ucTxByte)
{
    KINETIS_UART_CONTROL *uart_reg = (KINETIS_UART_CONTROL *)fnSelectChannel(Channel);
    #if NUMBER_EXTERNAL_SERIAL > 0
    if (Channel >= NUMBER_SERIAL) {
        fnExtSCI_send((QUEUE_HANDLE)(Channel - NUMBER_SERIAL), ucTxByte);// pass on to the external interface for transmission
        return 0;
    }
    #endif
    #if LPUARTS_AVAILABLE > 0
        #if UARTS_AVAILABLE > 0
    if (uart_type[Channel] == UART_TYPE_LPUART) {                        // LPUART
        #endif
        if ((((KINETIS_LPUART_CONTROL *)uart_reg)->LPUART_STAT & LPUART_STAT_TDRE) == 0) {
            return 1;                                                    // LPUART transmitter is presently active
        }
        #if defined SUPPORT_LOW_POWER                                    // {96}
        ulPeripheralNeedsClock |= (UART0_TX_CLK_REQUIRED << Channel);    // mark that this UART is in use
        ((KINETIS_LPUART_CONTROL *)uart_reg)->LPUART_STAT;               // read the status register to clear the transmit complete flag when the transmit data register is written
        #endif
        ((KINETIS_LPUART_CONTROL *)uart_reg)->LPUART_DATA = ucTxByte;    // send the character
        #if defined _WINDOWS
        ucTxLast[Channel] = (unsigned char)((KINETIS_LPUART_CONTROL *)uart_reg)->LPUART_DATA; // back up the data written so that it can't get lost when rx data uses the simulated register
        ((KINETIS_LPUART_CONTROL *)uart_reg)->LPUART_STAT &= ~LPUART_STAT_TDRE; // mark transmitter presently not empty
        iInts |= (CHANNEL_0_SERIAL_INT << Channel);
        #endif
        ((KINETIS_LPUART_CONTROL *)uart_reg)->LPUART_CTRL |= (LPUART_CTRL_TIE); // enable the LPUART transmission interrupt
        #if UARTS_AVAILABLE > 0
    }
    else {                                                               // UART
        #endif
    #endif
    #if UARTS_AVAILABLE > 0
        if ((uart_reg->UART_S1 & UART_S1_TDRE) == 0) {
            return 1;                                                    // UART transmitter is presently active
        }
        #if defined SUPPORT_LOW_POWER                                    // {96}
        ulPeripheralNeedsClock |= (UART0_TX_CLK_REQUIRED << Channel);    // mark that this UART is in use
        (void)(uart_reg->UART_S1);                                       // read the status register to clear the transmit complete flag when the transmit data register is written
        #endif
        uart_reg->UART_D = ucTxByte;                                     // send the character
        #if defined _WINDOWS
        ucTxLast[Channel] = (unsigned char)uart_reg->UART_D;             // back up the data written so that it can't get lost when rx data uses the simulated register
        uart_reg->UART_S1 &= ~(UART_S1_TDRE | UART_S1_TC);               // mark transmitter presently not empty
        iInts |= (CHANNEL_0_SERIAL_INT << Channel);
        #endif
        #if defined TRUE_UART_TX_2_STOPS
        if (ucStops[Channel] != 0) {
            uart_reg->UART_C2 |= (UART_C2_TCIE);                         // enable UART transmission interrupt (transmission complete)
        }
        else {
            uart_reg->UART_C2 |= (UART_C2_TIE);                          // enable UART transmission interrupt (buffer empty)   
        }
        #else
        uart_reg->UART_C2 |= (UART_C2_TIE);                              // enable UART transmission interrupt (buffer empty) 
        #endif
    #endif
    #if LPUARTS_AVAILABLE > 0 && UARTS_AVAILABLE > 0
    }
    #endif
    return 0;                                                            // transmission started
}


// The TTY driver uses this call to reset/disable the transmit interrupt on the serial port
//
extern void fnClearTxInt(QUEUE_HANDLE Channel)
{
    KINETIS_UART_CONTROL *uart_reg;
    #if NUMBER_EXTERNAL_SERIAL > 0
    if (Channel >= NUMBER_SERIAL) {
        return;
    }
    #endif
    uart_reg = (KINETIS_UART_CONTROL *)fnSelectChannel(Channel);
    #if defined SERIAL_SUPPORT_DMA                                       // {6}
        #if defined KINETIS_KL && (UARTS_AVAILABLE > 1)
    if (Channel == 0) {
        if (uart_reg->UART_C5 & UART_C5_TDMAS) {                         // if the transmitter is operating in DMA mode
            return;                                                      // leave the transmitter interrupt enabled since it is used by the DMA request, which is not enabled before a transfer starts
        }
    }
    else {
        if (uart_reg->UART_MA1_C4 & UART_C4_TDMAS) {                     // if the transmitter is operating in DMA mode
            return;                                                      // leave the transmitter interrupt enabled since it is used by the DMA request, which is not enabled before a transfer starts
        }
    }
        #elif LPUARTS_AVAILABLE > 0                                      // LPUART(s) available
            #if UARTS_AVAILABLE > 0                                      // both LPUART and UART available
    if (uart_type[Channel] == UART_TYPE_LPUART) {
        if ((((KINETIS_LPUART_CONTROL *)uart_reg)->LPUART_BAUD & LPUART_BAUD_TDMAE) != 0) { // LPUART tx is being used in DMA mode
            return;
        }
    }
    else {
        if (uart_reg->UART_C5 & UART_C5_TDMAS) {                        // if the transmitter is operating in DMA mode
            return;                                                     // leave the transmitter interrupt enabled since it is used by the DMA request, which is not enabled before a transfer starts
        }
    }
            #else
    if ((((KINETIS_LPUART_CONTROL *)uart_reg)->LPUART_BAUD & LPUART_BAUD_TDMAE) != 0) { // LPUART tx is being used in DMA mode
        return;
    }
            #endif
        #else
    if (uart_reg->UART_C5 & UART_C5_TDMAS) {                             // if the transmitter is operating in DMA mode
        return;                                                          // leave the transmitter interrupt enabled since it is used by the DMA request, which is not enabled before a transfer starts
    }
        #endif
    #endif                                                               // end if defined SERIAL_SUPPORT_DMA

    #if LPUARTS_AVAILABLE > 0
        #if UARTS_AVAILABLE > 0
    if (uart_type[Channel] == UART_TYPE_LPUART) {
        #endif
        #if defined SUPPORT_LOW_POWER                                    // {96} no more transmissions are required
        ((KINETIS_LPUART_CONTROL *)uart_reg)->LPUART_CTRL |= (LPUART_CTRL_TCIE); // enable LPUART transmit complete interrupt to signal when the complete last character has been sent
        #endif
        ((KINETIS_LPUART_CONTROL *)uart_reg)->LPUART_CTRL &= ~(LPUART_CTRL_TIE); // disable LPUART transmission interrupt
        return;
        #if UARTS_AVAILABLE > 0
    }
        #endif
    #endif
    #if UARTS_AVAILABLE > 0
        #if defined SUPPORT_LOW_POWER                                    // {96} no more transmissions are required
            #if defined TRUE_UART_TX_2_STOPS
    if (ucStops[Channel] != 0) {                                         // channel operating in true 2 stop bit mode (using the transmit complete interrupt rather than the buffer empty interrupt)
        uart_reg->UART_C2 &= ~(UART_C2_TIE | UART_C2_TCIE);              // disable the transmission interrupts since the buffer has been fully sent
        ulPeripheralNeedsClock &= ~(UART0_TX_CLK_REQUIRED << Channel);   // this transmitter is now idle since its final bit has been sent (stop mode is no longer blocked)
        return;
    }
    else {
        uart_reg->UART_C2 |= (UART_C2_TCIE);                             // enable UART transmit complete interrupt to signal when the complete last character has been sent
    }
            #else
    uart_reg->UART_C2 |= (UART_C2_TCIE);                                 // enable UART transmit complete interrupt to signal when the complete last character has been sent
            #endif
        #elif (defined KINETIS_KL || defined KINETIS_KE) && defined UART_FRAME_END_COMPLETE
    if (ucReportEndOfFrame[Channel] != 0) {                              // if an end of frame interrupt is required
        uart_reg->UART_C2 |= (UART_C2_TCIE);                             // {200} enable UART transmit complete interrupt to signal when the complete last character has been sent
    }
        #endif
    uart_reg->UART_C2 &= ~(UART_C2_TIE);                                 // disable UART transmission interrupt
    #endif
}

/* =================================================================== */
/*                LPUART/UART Tx DMA interrupt handlers                */
/* =================================================================== */

    #if defined SERIAL_SUPPORT_DMA                                       // {6}

// Transmission UART DMA completion interrupt handlers
//
static __interrupt void _uart0_tx_dma_Interrupt(void)
{
    #if defined KINETIS_KL                                               // {81}
    KINETIS_DMA *ptrDMA = (KINETIS_DMA *)DMA_BLOCK;
    ptrDMA += DMA_UART0_TX_CHANNEL;
    ptrDMA->DMA_DSR_BCR = DMA_DSR_BCR_DONE;                              // clear DMA interrupt
    #else
        #if defined _WINDOWS
    DMA_INT &= ~(DMA_INT_INT0 << UART_DMA_TX_CHANNEL[0]);                // clear the DMA interrupt request
        #else
    DMA_INT = (DMA_INT_INT0 << UART_DMA_TX_CHANNEL[0]);                  // clear the interrupt request
        #endif
    #endif
            #if defined UART_TIMED_TRANSMISSION                          // {208}
    if (ulInterCharTxDelay[0] != 0) {
        fnStopTxTimer(0);                                                // stop the periodic timer that controlled byte transmissions
    }
        #endif
    #if defined SUPPORT_LOW_POWER                                        // {96}
    ulPeripheralNeedsClock &= ~(UART0_TX_CLK_REQUIRED);                  // mark that this UART has completed transmission activity
    #endif
    fnSciTxByte(0);                                                      // tty block transferred, send next, if available
    #if defined SUPPORT_LOW_POWER                                        // {96}
    if ((ulPeripheralNeedsClock & UART0_TX_CLK_REQUIRED) == 0) {         // if no further transmission was started we need to wait until the final byte has been transferred
        ulPeripheralNeedsClock |= UART0_TX_CLK_REQUIRED;                 // block stop mode until the final interrupt arrives
        #if LPUARTS_AVAILABLE > 0 && !defined LPUARTS_PARALLEL
        LPUART0_CTRL |= LPUART_CTRL_TCIE;                                // enable the LPUART transmit complete flag interrupt to detect transmission completion
        #else
        UART0_C2 |= UART_C2_TCIE;                                        // enable the UART transmit complete flag interrupt to detect transmission completion
        #endif
    }
    #endif
}
    #if (UARTS_AVAILABLE + LPUARTS_AVAILABLE) > 1
static __interrupt void _uart1_tx_dma_Interrupt(void)
{
        #if defined KINETIS_KL                                           // {81}
    KINETIS_DMA *ptrDMA = (KINETIS_DMA *)DMA_BLOCK;
    ptrDMA += DMA_UART1_TX_CHANNEL;
    ptrDMA->DMA_DSR_BCR = DMA_DSR_BCR_DONE;                              // clear DMA interrupt
        #else
            #if defined _WINDOWS
    DMA_INT &= ~(DMA_INT_INT0 << UART_DMA_TX_CHANNEL[1]);                // clear the DMA interrupt request
            #else
    DMA_INT = (DMA_INT_INT0 << UART_DMA_TX_CHANNEL[1]);                  // clear the interrupt request
            #endif
        #endif
        #if defined UART_TIMED_TRANSMISSION                              // {208}
    if (ulInterCharTxDelay[1] != 0) {
        fnStopTxTimer(1);                                                // stop the periodic timer that controlled byte transmissions
    }
        #endif
        #if defined SUPPORT_LOW_POWER                                    // {96}
    ulPeripheralNeedsClock &= ~(UART1_TX_CLK_REQUIRED);                  // mark that this UART has completed transmission activity
        #endif
    fnSciTxByte(1);                                                      // tty block transferred, send next, if available
        #if defined SUPPORT_LOW_POWER                                    // {96}
    if ((ulPeripheralNeedsClock & UART1_TX_CLK_REQUIRED) == 0) {         // if no further transmission was started we need to wait until the final byte has been transferred
        ulPeripheralNeedsClock |= UART1_TX_CLK_REQUIRED;                 // block stop mode until the final interrupt arrives
            #if LPUARTS_AVAILABLE > 1 && !defined LPUARTS_PARALLEL
        LPUART1_CTRL |= LPUART_CTRL_TCIE;                                // enable the LPUART transmit complete flag interrupt to detect transmission completion
           #elif LPUARTS_AVAILABLE == 1 && !defined LPUARTS_PARALLEL
        UART0_C2 |= UART_C2_TCIE;      
            #else
        UART1_C2 |= UART_C2_TCIE;                                        // enable the transmit complete flag interrupt to detect transmission completion
            #endif
    }
        #endif
}
    #endif

    #if (UARTS_AVAILABLE + LPUARTS_AVAILABLE) > 2
static __interrupt void _uart2_tx_dma_Interrupt(void)
{
        #if defined KINETIS_KL                                           // {81}
    KINETIS_DMA *ptrDMA = (KINETIS_DMA *)DMA_BLOCK;
    ptrDMA += DMA_UART2_TX_CHANNEL;
    ptrDMA->DMA_DSR_BCR = DMA_DSR_BCR_DONE;                              // clear DMA interrupt
        #else
            #if defined _WINDOWS
    DMA_INT &= ~(DMA_INT_INT0 << UART_DMA_TX_CHANNEL[2]);                // clear the DMA interrupt request
            #else
    DMA_INT = (DMA_INT_INT0 << UART_DMA_TX_CHANNEL[2]);                  // clear the interrupt request
            #endif
        #endif
        #if defined UART_TIMED_TRANSMISSION                              // {208}
    if (ulInterCharTxDelay[2] != 0) {
        fnStopTxTimer(2);                                                // stop the periodic timer that controlled byte transmissions
    }
        #endif
        #if defined SUPPORT_LOW_POWER                                    // {96}
    ulPeripheralNeedsClock &= ~(UART2_TX_CLK_REQUIRED);                  // mark that this UART has completed transmission activity
        #endif
    fnSciTxByte(2);                                                      // tty block transferred, send next, if available
        #if defined SUPPORT_LOW_POWER                                    // {96}
    if ((ulPeripheralNeedsClock & UART2_TX_CLK_REQUIRED) == 0) {         // if no further transmission was started we need to wait until the final byte has been transferred
        ulPeripheralNeedsClock |= UART2_TX_CLK_REQUIRED;                 // block stop mode until the final interrupt arrives
            #if LPUARTS_AVAILABLE > 2 && !defined LPUARTS_PARALLEL
        LPUART2_CTRL |= LPUART_CTRL_TCIE;                                // enable the LPUART transmit complete flag interrupt to detect transmission completion
           #elif LPUARTS_AVAILABLE == 2 && defined LPUARTS_PARALLEL
        UART0_C2 |= UART_C2_TCIE;      
            #else
        UART2_C2 |= UART_C2_TCIE;                                        // enable the transmit complete flag interrupt to detect transmission completion
            #endif
    }
        #endif
}
    #endif
    #if (UARTS_AVAILABLE + LPUARTS_AVAILABLE) > 3
static __interrupt void _uart3_tx_dma_Interrupt(void)
{
        #if defined _WINDOWS
    DMA_INT &= ~(DMA_INT_INT0 << UART_DMA_TX_CHANNEL[3]);                // clear the DMA interrupt request
        #else
    DMA_INT = (DMA_INT_INT0 << UART_DMA_TX_CHANNEL[3]);                  // clear the interrupt request
        #endif
        #if defined UART_TIMED_TRANSMISSION                              // {208}
    if (ulInterCharTxDelay[3] != 0) {
        fnStopTxTimer(3);                                                // stop the periodic timer that controlled byte transmissions
    }
        #endif
        #if defined SUPPORT_LOW_POWER                                    // {96}
    ulPeripheralNeedsClock &= ~(UART3_TX_CLK_REQUIRED);                  // mark that this UART has completed transmission activity
        #endif
    fnSciTxByte(3);                                                      // tty block transferred, send next, if available
        #if defined SUPPORT_LOW_POWER                                    // {96}
    if ((ulPeripheralNeedsClock & UART3_TX_CLK_REQUIRED) == 0) {         // if no further transmission was started we need to wait until the final byte has been transferred
        ulPeripheralNeedsClock |= UART3_TX_CLK_REQUIRED;                 // block stop mode until the final interrupt arrives
            #if (LPUARTS_AVAILABLE > 3 && !defined LPUARTS_PARALLEL)
        LPUART3_CTRL |= LPUART_CTRL_TCIE;                                // enable the LPUART transmit complete flag interrupt to detect transmission completion
           #elif LPUARTS_AVAILABLE == 3 && !defined LPUARTS_PARALLEL
        UART0_C2 |= UART_C2_TCIE;
            #elif UARTS_AVAILABLE == 3                                   // K22
        LPUART0_CTRL |= LPUART_CTRL_TCIE;
            #else
        UART3_C2 |= UART_C2_TCIE;                                        // enable the transmit complete flag interrupt to detect transmission completion
            #endif
    }
        #endif
}
    #endif
    #if (UARTS_AVAILABLE + LPUARTS_AVAILABLE) > 4
static __interrupt void _uart4_tx_dma_Interrupt(void)
{
        #if defined _WINDOWS
    DMA_INT &= ~(DMA_INT_INT0 << UART_DMA_TX_CHANNEL[4]);                // clear the interrupt request
        #else
    DMA_INT = (DMA_INT_INT0 << UART_DMA_TX_CHANNEL[4]);                  // clear the interrupt request
        #endif
        #if defined SUPPORT_LOW_POWER                                    // {96}
    ulPeripheralNeedsClock &= ~(UART4_TX_CLK_REQUIRED);                  // mark that this UART has completed transmission activity
        #endif
    fnSciTxByte(4);                                                      // tty block transferred, send next, if available
        #if defined SUPPORT_LOW_POWER                                    // {96}
    if ((ulPeripheralNeedsClock & UART4_TX_CLK_REQUIRED) == 0) {         // if no further transmission was started we need to wait until the final byte has been transferred
        ulPeripheralNeedsClock |= UART4_TX_CLK_REQUIRED;                 // block stop mode until the final interrupt arrives
            #if (LPUARTS_AVAILABLE > 3 && !defined LPUARTS_PARALLEL)
        LPUART4_CTRL |= LPUART_CTRL_TCIE;                                // enable the LPUART transmit complete flag interrupt to detect transmission completion
            #else
        UART4_C2 |= UART_C2_TCIE;                                        // enable the transmit complete flag interrupt to detect transmission completion
            #endif
    }
        #endif
}
    #endif
    #if (UARTS_AVAILABLE + LPUARTS_AVAILABLE) > 5
static __interrupt void _uart5_tx_dma_Interrupt(void)
{
        #if defined _WINDOWS
    DMA_INT &= ~(DMA_INT_INT0 << UART_DMA_TX_CHANNEL[5]);                // clear the interrupt request
        #else
    DMA_INT = (DMA_INT_INT0 << UART_DMA_TX_CHANNEL[5]);                  // clear the interrupt request
        #endif
        #if defined SUPPORT_LOW_POWER                                    // {96}
    ulPeripheralNeedsClock &= ~(UART5_TX_CLK_REQUIRED);                  // mark that this UART has completed transmission activity
        #endif
    fnSciTxByte(5);                                                      // tty block transferred, send next, if available
        #if defined SUPPORT_LOW_POWER                                    // {96}
    if ((ulPeripheralNeedsClock & UART5_TX_CLK_REQUIRED) == 0) {         // if no further transmission was started we need to wait until the final byte has been transferred
        ulPeripheralNeedsClock |= UART5_TX_CLK_REQUIRED;                 // block stop mode until the final interrupt arrives
            #if LPUARTS_AVAILABLE == 1
        LPUART0_CTRL |= LPUART_CTRL_TCIE;                                // enable the LPUART transmit complete flag interrupt to detect transmission completion
            #else
        UART5_C2 |= UART_C2_TCIE;                                        // enable the transmit complete flag interrupt to detect transmission completion
            #endif
    }
        #endif
}
    #endif

static void (*_uart_tx_dma_Interrupt[UARTS_AVAILABLE + LPUARTS_AVAILABLE])(void) = {
    _uart0_tx_dma_Interrupt,
    #if (UARTS_AVAILABLE + LPUARTS_AVAILABLE) > 1
    _uart1_tx_dma_Interrupt,
    #endif
    #if (UARTS_AVAILABLE + LPUARTS_AVAILABLE) > 2
    _uart2_tx_dma_Interrupt,
    #endif
    #if (UARTS_AVAILABLE + LPUARTS_AVAILABLE) > 3
    _uart3_tx_dma_Interrupt,
    #endif
    #if (UARTS_AVAILABLE + LPUARTS_AVAILABLE) > 4
    _uart4_tx_dma_Interrupt,
    #endif
    #if (UARTS_AVAILABLE + LPUARTS_AVAILABLE) > 5
    _uart5_tx_dma_Interrupt
    #endif
};

#if defined UART_TIMED_TRANSMISSION                                      // {208}
// Start the periodic timer that will control subsequent byte transmissions
//
static void fnStartTxTimer(int Channel, unsigned long ulDelay)
{
    PIT_SETUP pit_setup;                                                 // interrupt configuration parameters
    pit_setup.int_type = PIT_INTERRUPT;
    pit_setup.int_priority = 0;                                          // not used
    pit_setup.count_delay = ulDelay;                                     // transmission time base
    pit_setup.mode = (PIT_PERIODIC);                                     // periodic DMA trigger
    pit_setup.int_handler = 0;                                           // no interrupt since the PIT will be used for triggering DMA
    pit_setup.ucPIT = (unsigned char)Channel;                            // use PIT equal to the channel number
    fnConfigureInterrupt((void *)&pit_setup);                            // configure and start PIT
    *(unsigned char *)(DMAMUX0_BLOCK + UART_DMA_TX_CHANNEL[Channel]) = ((DMAMUX0_DMA0_CHCFG_SOURCE_PIT0 + Channel) | DMAMUX_CHCFG_ENBL); // connect PIT to DMA channel
    #if defined _WINDOWS
    if (UART_DMA_TX_CHANNEL[Channel] != pit_setup.ucPIT) {
        _EXCEPTION("PITs can only trigger DMA on their DMA channel number");
    }
    #endif
}

static void fnStopTxTimer(int Channel)
{
    PIT_SETUP pit_setup;                                                 // interrupt configuration parameters
    pit_setup.int_type = PIT_INTERRUPT;
    pit_setup.mode = (PIT_STOP);                                         // stop the timer
    pit_setup.ucPIT = (unsigned char)Channel;                            // the PIT channel equal to the channel number
    fnConfigureInterrupt((void *)&pit_setup);                            // stop PIT
}
#endif

// Start transfer of a block via DMA
//
extern QUEUE_TRANSFER fnTxByteDMA(QUEUE_HANDLE Channel, unsigned char *ptrStart, QUEUE_TRANSFER tx_length)
{
        #if defined UART_TIMED_TRANSMISSION && defined _WINDOWS
    int iNoDMA_int = 0;
        #endif
        #if defined SUPPORT_LOW_POWER || defined UART_TIMED_TRANSMISSION || defined _WINDOWS
    KINETIS_UART_CONTROL *uart_reg = fnSelectChannel(Channel);
        #endif
        #if defined KINETIS_KL                                           // {81}
    KINETIS_DMA *ptrDMA = (KINETIS_DMA *)DMA_BLOCK;
    ptrDMA += UART_DMA_TX_CHANNEL[Channel];
        #else
    KINETIS_DMA_TDC *ptrDMA_TCD = (KINETIS_DMA_TDC *)eDMA_DESCRIPTORS;
    ptrDMA_TCD += UART_DMA_TX_CHANNEL[Channel];
        #endif
        #if LPUARTS_AVAILABLE > 0
            #if UARTS_AVAILABLE > 0
    if (uart_type[Channel] == UART_TYPE_LPUART) {                        // LPUART
            #endif
            #if defined SUPPORT_LOW_POWER
        (void)(((KINETIS_LPUART_CONTROL *)uart_reg)->LPUART_STAT);       // read the status register to clear the transmit complete flag when the transmit data register is written
                #if defined SUPPORT_LOW_POWER    
        ulPeripheralNeedsClock |= (UART0_TX_CLK_REQUIRED << Channel);    // mark that stop mode should be avoided until the transmit activity has completed
                #endif
        ((KINETIS_LPUART_CONTROL *)uart_reg)->LPUART_CTRL &= ~(LPUART_CTRL_TCIE); // disable transmit complete interrupt
            #endif
            #if UARTS_AVAILABLE > 0
    }
    else {                                                               // UART
            #endif
        #endif
        #if UARTS_AVAILABLE > 0
            #if defined SUPPORT_LOW_POWER
        (void)(uart_reg->UART_S1);                                       // read the status register to clear the transmit complete flag when the transmit data register is written
        uart_reg->UART_C2 &= ~(UART_C2_TCIE);                            // disable transmit complete interrupt
            #endif
        #endif
        #if LPUARTS_AVAILABLE > 0 && UARTS_AVAILABLE > 0
    }
        #endif    
        #if defined KINETIS_KL
            #if defined UART_TIMED_TRANSMISSION                          // {208}
    if ((tx_length > 1) && (ulInterCharTxDelay[Channel] != 0)) {         // if timed transmissions are required on this channel (linear buffer is always assumed and 7/8 bit characters)
       ptrDMA->DMA_DSR_BCR = DMA_DSR_BCR_DONE;                           // clear the DONE flag and clear errors etc. before starting the HW timer to trigger transfers
        fnStartTxTimer(Channel, ulInterCharTxDelay[Channel]);            // start the periodic timer that will control subsequent byte transmissions
                #if defined _WINDOWS
        iNoDMA_int = 1;
                #endif
    }
            #endif
    ptrDMA->DMA_DSR_BCR = (tx_length & DMA_DSR_BCR_BCR_MASK);            // the number of service requests (the number of bytes to be transferred)
    ptrDMA->DMA_SAR = (unsigned long)ptrStart;                           // source is tty output buffer
            #if defined SUPPORT_LOW_POWER
    ulPeripheralNeedsClock |= (UART0_TX_CLK_REQUIRED << Channel);        // mark that stop mode should be avoided until the transmit activity has completed
            #endif
    ptrDMA->DMA_DCR |= DMA_DCR_ERQ;                                      // enable request source
        #else
    ptrDMA_TCD->DMA_TCD_BITER_ELINK = ptrDMA_TCD->DMA_TCD_CITER_ELINK = tx_length; // the number of service requests (the number of bytes to be transferred)
    ptrDMA_TCD->DMA_TCD_SADDR = (unsigned long)ptrStart;                 // source is tty output buffer
            #if defined SUPPORT_LOW_POWER
    ulPeripheralNeedsClock |= (UART0_TX_CLK_REQUIRED << Channel);        // mark that stop mode should be avoided until the transmit activity has completed
            #endif
            #if defined UART_TIMED_TRANSMISSION                          // {208}
    if ((tx_length > 1) && (ulInterCharTxDelay[Channel] != 0)) {         // if timed transmissions are required on this channel (linear buffer is always assumed and 7/8 bit characters)
        fnStartTxTimer(Channel, ulInterCharTxDelay[Channel]);            // start the periodic timer that will control subsequent byte transmissions
                #if defined _WINDOWS
        iNoDMA_int = 1;
                #endif
    }
            #endif

    DMA_ERQ |= (DMA_ERQ_ERQ0 << UART_DMA_TX_CHANNEL[Channel]);           // enable request source
        #endif 
        #if defined _WINDOWS                                             // simulation
            #if LPUARTS_AVAILABLE > 0
                #if UARTS_AVAILABLE > 0
    if (uart_type[Channel] == UART_TYPE_LPUART) {
                #endif
        ((KINETIS_LPUART_CONTROL *)uart_reg)->LPUART_STAT &= ~(LPUART_STAT_TDRE | LPUART_STAT_TC); // mark transmitter presently not empty
                #if UARTS_AVAILABLE > 0
    }
    else {
        uart_reg->UART_S1 &= ~(UART_S1_TDRE | UART_S1_TC);               // mark transmitter presently not empty
    }
                #endif
            #elif UARTS_AVAILABLE > 0
    uart_reg->UART_S1 &= ~(UART_S1_TDRE | UART_S1_TC);                   // mark transmitter presently not empty
            #endif
            #if !defined KINETIS_KL
    ptrDMA_TCD->DMA_TCD_CSR |= DMA_TCD_CSR_ACTIVE;                       // trigger activity
            #endif
            #if defined UART_TIMED_TRANSMISSION
    if (iNoDMA_int != 0) {
        return tx_length;
    }
            #endif
    iDMA |= (DMA_CONTROLLER_0 << UART_DMA_TX_CHANNEL[Channel]);          // activate first DMA request
        #endif
    return tx_length;
}

        #if defined SERIAL_SUPPORT_XON_XOFF
extern QUEUE_TRANSFER fnAbortTxDMA(QUEUE_HANDLE channel, QUEQUE *ptrQueue)
{
    return 0;
}
        #endif
    #endif

/* =================================================================== */
/*                LPUART/UART Rx DMA interrupt handlers                */
/* =================================================================== */

     #if defined SERIAL_SUPPORT_DMA_RX
static __interrupt void _uart0_rx_dma_Interrupt(void)
{
    #if defined _WINDOWS
    DMA_INT &= ~(DMA_INT_INT0 << UART_DMA_RX_CHANNEL[0]);                // clear the interrupt request
    #else
    DMA_INT = (DMA_INT_INT0 << UART_DMA_RX_CHANNEL[0]);                  // clear the interrupt request
    #endif
    fnSciRxByte(0, 0);                                                   // tty block ready for read
}
    #if (UARTS_AVAILABLE + LPUARTS_AVAILABLE) > 1
static __interrupt void _uart1_rx_dma_Interrupt(void)
{
        #if defined _WINDOWS
    DMA_INT &= ~(DMA_INT_INT0 << UART_DMA_RX_CHANNEL[1]);                // clear the interrupt request
        #else
    DMA_INT = (DMA_INT_INT0 << UART_DMA_RX_CHANNEL[1]);                  // clear the interrupt request
        #endif
    fnSciRxByte(0, 1);                                                   // tty block ready for read
}
    #endif
    #if (UARTS_AVAILABLE + LPUARTS_AVAILABLE) > 2
static __interrupt void _uart2_rx_dma_Interrupt(void)
{
        #if defined _WINDOWS
    DMA_INT &= ~(DMA_INT_INT0 << UART_DMA_RX_CHANNEL[2]);                // clear the interrupt request
        #else
    DMA_INT = (DMA_INT_INT0 << UART_DMA_RX_CHANNEL[2]);                  // clear the interrupt request
        #endif
    fnSciRxByte(0, 2);                                                   // tty block ready for read
}
    #endif
    #if (UARTS_AVAILABLE + LPUARTS_AVAILABLE) > 3
static __interrupt void _uart3_rx_dma_Interrupt(void)
{
        #if defined _WINDOWS
    DMA_INT &= ~(DMA_INT_INT0 << UART_DMA_RX_CHANNEL[3]);                // clear the interrupt request
        #else
    DMA_INT = (DMA_INT_INT0 << UART_DMA_RX_CHANNEL[3]);                  // clear the interrupt request
        #endif
    fnSciRxByte(0, 3);                                                   // tty block ready for read
}
    #endif
    #if (UARTS_AVAILABLE + LPUARTS_AVAILABLE) > 4
static __interrupt void _uart4_rx_dma_Interrupt(void)
{
        #if defined _WINDOWS
    DMA_INT &= ~(DMA_INT_INT0 << UART_DMA_RX_CHANNEL[4]);                // clear the interrupt request
        #else
    DMA_INT = (DMA_INT_INT0 << UART_DMA_RX_CHANNEL[4]);                  // clear the interrupt request
        #endif
    fnSciRxByte(0, 4);                                                   // tty block ready for read
}
    #endif
    #if (UARTS_AVAILABLE + LPUARTS_AVAILABLE) > 5
static __interrupt void _uart5_rx_dma_Interrupt(void)
{
        #if defined _WINDOWS
    DMA_INT &= ~(DMA_INT_INT0 << UART_DMA_RX_CHANNEL[5]);                // clear the interrupt request
        #else
    DMA_INT = (DMA_INT_INT0 << UART_DMA_RX_CHANNEL[5]);                  // clear the interrupt request
        #endif
    fnSciRxByte(0, 5);                                                   // tty block ready for read
}
    #endif

static void (*_uart_rx_dma_Interrupt[UARTS_AVAILABLE + LPUARTS_AVAILABLE])(void) = {
    _uart0_rx_dma_Interrupt,
    #if (UARTS_AVAILABLE + LPUARTS_AVAILABLE) > 1
    _uart1_rx_dma_Interrupt,
    #endif
    #if (UARTS_AVAILABLE + LPUARTS_AVAILABLE) > 2
    _uart2_rx_dma_Interrupt,
    #endif
    #if (UARTS_AVAILABLE + LPUARTS_AVAILABLE) > 3
    _uart3_rx_dma_Interrupt,
    #endif
    #if (UARTS_AVAILABLE + LPUARTS_AVAILABLE) > 4
    _uart4_rx_dma_Interrupt,
    #endif
    #if (UARTS_AVAILABLE + LPUARTS_AVAILABLE) > 5
    _uart5_rx_dma_Interrupt
    #endif
};

// The Kinetis buffer has been set up to run continuously in circular buffer mode. This routine therefore doesn't use the length passed
//
extern void fnPrepareRxDMA(QUEUE_HANDLE channel, unsigned char *ptrStart, QUEUE_TRANSFER rx_length)
{
        #if defined SERIAL_SUPPORT_DMA_RX
    KINETIS_UART_CONTROL *uart_reg = fnSelectChannel(channel);
    if ((uart_reg->UART_C2 & (UART_C2_RE)) == 0) {                       // if receiver not yet enabled
        KINETIS_DMA_TDC *ptrDMA_TCD = (KINETIS_DMA_TDC *)eDMA_DESCRIPTORS;
        ptrDMA_TCD += UART_DMA_RX_CHANNEL[channel];
        ptrDMA_TCD->DMA_TCD_DADDR = (unsigned long)ptrStart;             // destination is the input tty buffer
        DMA_ERQ |= (DMA_ERQ_ERQ0 << UART_DMA_RX_CHANNEL[channel]);       // enable request source
        fnRxOn(channel);                                                 // configure receiver pin and enable reception and its interrupt/DMA
    }
            #if defined SERIAL_SUPPORT_DMA_RX_FREERUN                    // {15}
    else if (rx_length == 0) {                                           // call to update DMA progress
        unsigned short usDMA_rx;
        QUEQUE *tty_queue = (QUEQUE *)ptrStart;
        KINETIS_DMA_TDC *ptrDMA_TCD = (KINETIS_DMA_TDC *)eDMA_DESCRIPTORS;
        ptrDMA_TCD += UART_DMA_RX_CHANNEL[channel];
        usDMA_rx = ptrDMA_TCD->DMA_TCD_CITER_ELINK;                      // snap-shot of DMA reception progress
        if (usDMA_progress[channel] >= usDMA_rx) {
            tty_queue->chars += (usDMA_progress[channel] - usDMA_rx);    // the extra number of characters received by DMA since last check
        }
        else {
            tty_queue->chars += usDMA_progress[channel];
            tty_queue->chars += (ptrDMA_TCD->DMA_TCD_BITER_ELINK - usDMA_rx ); // the extra number of characters received by DMA since last check
        }
        usDMA_progress[channel] = usDMA_rx;                              // remember the check state
    }
            #endif
        #endif
}
    #endif

    #if defined UART_EXTENDED_MODE
extern unsigned char fnGetMultiDropByte(QUEUE_HANDLE Channel)            // dummy
{
    return 0;
}
    #endif

    #if defined SUPPORT_HW_FLOW
extern void fnControlLine(QUEUE_HANDLE channel, unsigned short usModifications, UART_MODE_CONFIG OperatingMode)
{
        #if NUMBER_EXTERNAL_SERIAL > 0
    if (channel >= NUMBER_SERIAL) {       
        fnSetRTS(channel, 0);                                            // prepare to drive the RTS line in negated state
        return;
    }
        #endif
        #if !defined KINETIS_KE && !defined KINETIS_KL
    KINETIS_UART_CONTROL *uart_reg = fnSelectChannel(channel);
    unsigned char ucMode;
    if (uart_reg == 0) {
        return;                                                          // invalid channel
    }
    ucMode = (uart_reg->UART_MODEM & ~(UART_MODEM_TXRTSE | UART_MODEM_TXRTSPOL)); // read the original modem configuration
        #endif

    if ((usModifications & (CONFIG_RTS_PIN | CONFIG_CTS_PIN)) != 0) {
        if ((usModifications & CONFIG_RTS_PIN) != 0) {
            switch (channel) {
            case 0:                                                      // configure the UART0_RTS pin
                #if defined KINETIS_KE || defined KINETIS_KL             // {200} families without RTS/CTS modem support
                if ((usModifications & SET_RS485_NEG) == 0) {
                    _CONFIGURE_RTS_0_HIGH();                             // configure RTS output and set to '1'
                    ucRTS_neg[0] = 0;                                    // not inverted RTS mode
                }
                else {
                    _CONFIGURE_RTS_0_LOW();                              // configure RTS output and set to '0'
                    ucRTS_neg[0] = 1;                                    // inverted RTS mode
                }
                #elif defined KINETIS_K02
                    #if defined UART0_A_LOW
                _CONFIG_PERIPHERAL(A, 3, (PA_3_UART0_RTS | UART_PULL_UPS)); // UART0_RX on PA1 (alt. function 2)
                    #elif defined UART0_ON_D
                _CONFIG_PERIPHERAL(D, 4, (PD_4_UART0_RTS | UART_PULL_UPS)); // UART0_RX on PD6 (alt. function 3)
                    #else
                _CONFIG_PERIPHERAL(B, 2, (PB_2_UART0_RTS | UART_PULL_UPS)); // UART0_RX on PB16 (alt. function 3)
                    #endif
                #elif defined UART0_A_LOW
                _CONFIG_PERIPHERAL(A, 3, (PA_3_UART0_RTS | UART_PULL_UPS)); // UART0_RTS on PA3 (alt. function 2)
                #elif defined UART0_ON_B
                _CONFIG_PERIPHERAL(B, 2, (PB_2_UART0_RTS | UART_PULL_UPS)); // UART0_RTS on PB2 (alt. function 3)
                #elif defined UART0_ON_D
                _CONFIG_PERIPHERAL(D, 4, (PD_4_UART0_RTS | UART_PULL_UPS)); // UART0_RTS on PD4 (alt. function 3)
                #else
                _CONFIG_PERIPHERAL(A, 17, (PA_17_UART0_RTS | UART_PULL_UPS)); // UART0_RTS on A17 (alt. function 3)
                #endif
                break;

            #if (UARTS_AVAILABLE + LPUARTS_AVAILABLE) > 1
            case 1:                                                      // configure the UART1_RTS pin
                #if defined KINETIS_KE || defined KINETIS_KL             // families without RTS/CTS modem support
                if ((usModifications & SET_RS485_NEG) == 0) {
                    _CONFIGURE_RTS_1_HIGH();                             // configure RTS output and set to '1'
                    ucRTS_neg[1] = 0;                                    // not inverted RTS mode
                }
                else {
                    _CONFIGURE_RTS_1_LOW();                              // configure RTS output and set to '0'
                    ucRTS_neg[1] = 1;                                    // inverted RTS mode
                }
                #elif defined UART1_ON_C
                _CONFIG_PERIPHERAL(C, 1, (PC_1_UART1_RTS | UART_PULL_UPS)); // UART1_RTS on PC1 (alt. function 3) {16}
                #else
                _CONFIG_PERIPHERAL(E, 3, (PE_3_UART1_RTS | UART_PULL_UPS)); // UART1_RTS on PE3 (alt. function 3)
                #endif
                break;
            #endif
            #if (UARTS_AVAILABLE + LPUARTS_AVAILABLE) > 2
            case 2:                                                      // configure the UART2_RTS pin
                #if defined KINETIS_KE || defined KINETIS_KL             // families without RTS/CTS modem support
                if ((usModifications & SET_RS485_NEG) == 0) {
                    _CONFIGURE_RTS_2_HIGH();                             // configure RTS output and set to '1'
                    ucRTS_neg[2] = 0;                                    // not inverted RTS mode
                }
                else {
                    _CONFIGURE_RTS_2_LOW();                              // configure RTS output and set to '0'
                    ucRTS_neg[2] = 1;                                    // inverted RTS mode
                }
                #elif (defined KINETIS_K61 || defined KINETIS_K70 || defined KINETIS_K21 || defined KINETIS_KW2X) && defined UART2_ON_E // {25}
                _CONFIG_PERIPHERAL(E, 19, (PE_19_UART2_RTS | UART_PULL_UPS)); // UART2_RTS on PE19 (alt. function 3)
                #elif (defined KINETIS_K61 || defined KINETIS_K70) && defined UART2_ON_F // {25}
                _CONFIG_PERIPHERAL(F, 11, (PF_11_UART2_RTS | UART_PULL_UPS)); // UART2_RTS on PF11 (alt. function 4)
                #else
                _CONFIG_PERIPHERAL(D, 0, (PD_0_UART2_RTS | UART_PULL_UPS)); // UART2_RTS on PD0 (alt. function 3)
                #endif
                break;
            #endif
            #if (UARTS_AVAILABLE + LPUARTS_AVAILABLE) > 3
            case 3:                                                      // configure the UART Rx 3 pin
                #if defined KINETIS_KE || defined KINETIS_KL             // families without RTS/CTS modem support
                if ((usModifications & SET_RS485_NEG) == 0) {
                    _CONFIGURE_RTS_3_HIGH();                             // configure RTS output and set to '1'
                    ucRTS_neg[3] = 0;                                    // not inverted RTS mode
                }
                else {
                    _CONFIGURE_RTS_3_LOW();                              // configure RTS output and set to '0'
                    ucRTS_neg[3] = 1;                                    // inverted RTS mode
                }
                #elif defined UART3_ON_B
                _CONFIG_PERIPHERAL(B, 8, (PB_8_UART3_RTS | UART_PULL_UPS)); // UART3_RTS on PB8 (alt. function 3)
                #elif defined UART3_ON_C
                _CONFIG_PERIPHERAL(C, 18, (PC_18_UART3_RTS | UART_PULL_UPS)); // UART3_RTS on PC18 (alt. function 3)
                #elif (defined KINETIS_K61 || defined KINETIS_K70) && defined UART3_ON_F // {25}
               _CONFIG_PERIPHERAL(F, 9, (PF_9_UART3_RTS | UART_PULL_UPS)); // UART3_RTS on PF9 (alt. function 4) {12}
                #else
                _CONFIG_PERIPHERAL(E, 7, (PE_7_UART3_RTS | UART_PULL_UPS)); // UART3_RTS on PE7 (alt. function 3)
                #endif
                break;
            #endif
            #if (UARTS_AVAILABLE + LPUARTS_AVAILABLE) > 4
            case 4:                                                      // configure the UART4_RTS pin
                #if defined KINETIS_KE || defined KINETIS_KL             // families without RTS/CTS modem support
                if ((usModifications & SET_RS485_NEG) == 0) {
                    _CONFIGURE_RTS_4_HIGH();                             // configure RTS output and set to '1'
                    ucRTS_neg[4] = 0;                                    // not inverted RTS mode
                }
                else {
                    _CONFIGURE_RTS_4_LOW();                              // configure RTS output and set to '0'
                    ucRTS_neg[4] = 1;                                    // inverted RTS mode
                }
                #elif defined UART4_ON_C
                _CONFIG_PERIPHERAL(C, 12, (PC_12_UART4_RTS | UART_PULL_UPS)); // UART4_RTS on PC12 (alt. function 3)
                #else
                _CONFIG_PERIPHERAL(E, 27, (PE_27_UART4_RTS | UART_PULL_UPS)); // UART4_RTS on PE27 (alt. function 3)
                #endif
                break;
            #endif
            #if (UARTS_AVAILABLE + LPUARTS_AVAILABLE) > 5
            case 5:                                                      // configure the UART5_RTS pin
                #if defined KINETIS_KE || defined KINETIS_KL             // families without RTS/CTS modem support
                if ((usModifications & SET_RS485_NEG) == 0) {
                    _CONFIGURE_RTS_5_HIGH();                             // configure RTS output and set to '1'
                    ucRTS_neg[5] = 0;                                    // not inverted RTS mode
                }
                else {
                    _CONFIGURE_RTS_5_LOW();                              // configure RTS output and set to '0'
                    ucRTS_neg[5] = 1;                                    // inverted RTS mode
                }
                #elif defined UART5_ON_D
                _CONFIG_PERIPHERAL(D, 10, (PD_10_UART5_RTS | UART_PULL_UPS)); // UART5_RTS on PD10 (alt. function 3)
                #else
                _CONFIG_PERIPHERAL(E, 11, (PE_11_UART5_RTS | UART_PULL_UPS)); // UART5_RTS on PE11 (alt. function 3)
                #endif
                break;
            #endif
            }
            #if !defined KINETIS_KE && !defined KINETIS_KL
            if ((usModifications & SET_RS485_MODE) != 0) {               // Kinetis supports automatic control of the RTS line which is used in RS485 mode
                ucMode |= UART_MODEM_TXRTSE;                             // enable automatic RTS control
                if ((usModifications & SET_RS485_NEG) == 0) {            // the polarity of the RTS line is inverted
                    ucMode |= UART_MODEM_TXRTSPOL;                       // positive RTS polarity
                } 
            }
            uart_reg->UART_MODEM = ucMode;                               // set the modem mode
            #endif
        }
            #if !defined KINETIS_KE && !defined KINETIS_KL
        if ((usModifications & CONFIG_CTS_PIN) != 0) {                   // configure CTS for HW flow control
            switch (channel) {
            case 0:                                                      // configure the UART0_CTS pin
            #if defined UART0_A_LOW
                _CONFIG_PERIPHERAL(A, 0, (PA_0_UART0_CTS | UART_PULL_UPS)); // UART0_CTS on PA0 (alt. function 2)
            #elif defined UART0_ON_B
                _CONFIG_PERIPHERAL(B, 3, (PB_3_UART0_CTS | UART_PULL_UPS)); // UART0_CTS on PB3 (alt. function 3)
            #elif defined UART0_ON_D
                _CONFIG_PERIPHERAL(D, 5, (PD_5_UART0_CTS | UART_PULL_UPS)); // UART0_CTS on PD5 (alt. function 3)
            #else
                _CONFIG_PERIPHERAL(A, 16, (PA_16_UART0_CTS | UART_PULL_UPS)); // UART0_CTS on PA16 (alt. function 3)
            #endif
                break;

            case 1:                                                      // configure the UART1_CTS pin
            #if defined UART1_ON_C
                _CONFIG_PERIPHERAL(C, 2, (PC_2_UART1_CTS | UART_PULL_UPS)); // UART1_CTS on PC2 (alt. function 3)
            #else
                _CONFIG_PERIPHERAL(E, 2, (PE_2_UART1_CTS | UART_PULL_UPS)); // UART1_CTS on PE2 (alt. function 3)
            #endif
                break;

            case 2:                                                      // configure the UART2_CTS pin
            #if (defined KINETIS_K61 || defined KINETIS_K70 || defined KINETIS_K21 || defined KINETIS_KW2X) && defined UART2_ON_E // {25}
                _CONFIG_PERIPHERAL(E, 18, (PE_18_UART2_CTS | UART_PULL_UPS)); // UART2_CTS on PE18 (alt. function 3)
            #elif (defined KINETIS_K61 || defined KINETIS_K70) && defined UART2_ON_F // {25}
                _CONFIG_PERIPHERAL(F, 12, (PF_12_UART2_CTS | UART_PULL_UPS)); // UART2_CTS on PF12 (alt. function 4)
            #else
                _CONFIG_PERIPHERAL(D, 1, (PD_1_UART2_CTS | UART_PULL_UPS)); // UART2_CTS on PD1 (alt. function 3)
            #endif
                break;
            #if UARTS_AVAILABLE > 3
            case 3:                                                      // configure the UART3_CTS pin
                #if defined UART3_ON_B
                _CONFIG_PERIPHERAL(B, 9, (PB_9_UART3_CTS | UART_PULL_UPS)); // UART3_CTS on PB9 (alt. function 3)
                #elif defined UART3_ON_C
                _CONFIG_PERIPHERAL(C, 19, (PC_19_UART3_CTS | UART_PULL_UPS)); // UART3_CTS on PC19 (alt. function 3)
                #elif (defined KINETIS_K61 || defined KINETIS_K70) && defined UART3_ON_F // {25}
                _CONFIG_PERIPHERAL(F, 10, (PF_10_UART3_CTS | UART_PULL_UPS)); // UART3_CTS on PF10 (alt. function 4) {12}
                #else
                _CONFIG_PERIPHERAL(E, 6, (PE_6_UART3_CTS | UART_PULL_UPS)); // UART3_CTS on PE6 (alt. function 3)
                #endif
                break;
            #endif
            #if UARTS_AVAILABLE > 4
            case 4:                                                      // configure the UART4_CTS pin
                #if defined UART4_ON_C
                _CONFIG_PERIPHERAL(C, 13, (PC_13_UART4_CTS | UART_PULL_UPS)); // UART4_CTS on PC13 (alt. function 3)
                #else
                _CONFIG_PERIPHERAL(E, 26, (PE_26_UART4_CTS | UART_PULL_UPS)); // UART4_CTS on PE26 (alt. function 3)
                #endif
                break;

            case 5:                                                      // configure the UART5_CTS pin
                #if defined UART5_ON_D
                _CONFIG_PERIPHERAL(D, 11, (PD_11_UART5_CTS | UART_PULL_UPS)); // UART5_CTS on PD11 (alt. function 3)
                #else
                _CONFIG_PERIPHERAL(E, 10, (PE_10_UART5_CTS | UART_PULL_UPS)); // UART5_CTS on PE10 (alt. function 3)
                #endif
                break;
            #endif
            }
        }
            #endif
    }
        #if defined KINETIS_KE || defined KINETIS_KL                     // {200}
        if ((((usModifications & (SET_RTS)) != 0) && (ucRTS_neg[0] == 0)) || (((usModifications & (CLEAR_RTS)) != 0) && (ucRTS_neg[0] != 0))) { // assert RTS output signal by setting output to '0' (or inverted negate)
            switch (channel) {
            case 0:
                _SET_RTS_0_LOW();
                break;
            #if (UARTS_AVAILABLE + LPUARTS_AVAILABLE) > 1
            case 1:
                _SET_RTS_1_LOW();
                break;
            #endif
            #if (UARTS_AVAILABLE + LPUARTS_AVAILABLE) > 2
            case 2:
                _SET_RTS_2_LOW();
                break;
            #endif
            #if (UARTS_AVAILABLE + LPUARTS_AVAILABLE) > 3
            case 3:
                _SET_RTS_3_LOW();
                break;
            #endif
            #if (UARTS_AVAILABLE + LPUARTS_AVAILABLE) > 4
            case 4:
                _SET_RTS_4_LOW();
                break;
            #endif
            #if (UARTS_AVAILABLE + LPUARTS_AVAILABLE) > 5
            case 5:
                _SET_RTS_5_LOW();
                break;
            #endif
            }
        }
        if ((((usModifications & (CLEAR_RTS)) != 0) && (ucRTS_neg[0] == 0)) || (((usModifications & (SET_RTS)) != 0) && (ucRTS_neg[0] != 0))) { // negate RTS output signal by setting output to '0' (or inverted assert)
            switch (channel) {
            case 0:
                _SET_RTS_0_HIGH();
                break;
            #if (UARTS_AVAILABLE + LPUARTS_AVAILABLE) > 1
            case 1:
                _SET_RTS_1_HIGH();
                break;
            #endif
            #if (UARTS_AVAILABLE + LPUARTS_AVAILABLE) > 2
            case 2:
                _SET_RTS_2_HIGH();
                break;
            #endif
            #if (UARTS_AVAILABLE + LPUARTS_AVAILABLE) > 3
            case 3:
                _SET_RTS_3_HIGH();
                break;
            #endif
            #if (UARTS_AVAILABLE + LPUARTS_AVAILABLE) > 4
            case 4:
                _SET_RTS_4_HIGH();
                break;
            #endif
            #if (UARTS_AVAILABLE + LPUARTS_AVAILABLE) > 5
            case 5:
                _SET_RTS_5_HIGH();
                break;
            #endif
            }
        }
        #else
    if ((ucMode & UART_MODEM_TXRTSE) == 0) {                             // control the polarity of the RTS line when not in RS485 mode
        if ((usModifications & (SET_RTS)) != 0) {                        // assert RTS output signal
          //uart_reg->UART_MODEM |= UART_MODEM_TXRTSPOL;
            uart_reg->UART_MODEM |= UART_MODEM_RXRTSE;                   // {7} RTS is asserted as long as the receive FIFO has space to receive
        }
        if ((usModifications & (CLEAR_RTS)) != 0) {                      // negate RTS output signal
          //uart_reg->UART_MODEM &= ~UART_MODEM_TXRTSPOL;
            uart_reg->UART_MODEM &= ~UART_MODEM_RXRTSE;                  // {7} disable control of RTS by receiver space which negates the RTS line
        }
    }
        #endif
}
 
// Modify control line interrupt operation (this is called from entry_tty() with disabled interrupts)
//
extern QUEUE_TRANSFER fnControlLineInterrupt(QUEUE_HANDLE channel, unsigned short usModifications, UART_MODE_CONFIG OperatingMode)
{
        #if !defined KINETIS_KE && !defined KINETIS_KL
    KINETIS_UART_CONTROL *uart_reg = fnSelectChannel(channel);

    if ((usModifications & ENABLE_CTS_CHANGE) != 0) {
        uart_reg->UART_MODEM |= UART_MODEM_TXCTSE;                       // enable CTS line to automatically stop the transmitter when negated (HW flow control)
    }
    if ((usModifications & DISABLE_CTS_CHANGE) != 0) {
        uart_reg->UART_MODEM &= ~UART_MODEM_TXCTSE;                      // disable CTS line flow control
    }
    return SET_CTS;                                                      // the state of the CTS line cannot be read but report that it is asserted since flow control is performed by hardware
        #else
    return 0;
        #endif
}
    #endif                                                               // end SUPPORT_HW_FLOW


// Enable transmission on the defined channel - including configuring the transmit data output
//
extern void fnTxOn(QUEUE_HANDLE Channel)
{
    KINETIS_UART_CONTROL *uart_reg;
    #if NUMBER_EXTERNAL_SERIAL > 0
    if (Channel >= NUMBER_SERIAL) {
        fnExtSCITxOn((QUEUE_HANDLE)(Channel - NUMBER_SERIAL));           // pass on to external UART driver
        return;
    }
    #endif
    uart_reg = fnSelectChannel(Channel);
    #if LPUARTS_AVAILABLE > 0
        #if UARTS_AVAILABLE > 0
    if (uart_type[Channel] == UART_TYPE_LPUART) {                        // LPUART
        #endif
        switch (Channel) {
        #if defined LPUARTS_PARALLEL
        case UARTS_AVAILABLE:
        #else
        case 0:                                                          // LPUART 0
        #endif
        #if defined KINETIS_KL03
            #if defined LPUART0_ON_A
            _CONFIG_PERIPHERAL(A, 3, (PA_3_LPUART0_TX | UART_PULL_UPS)); // LPUART0_TX on PA3 (alt. function 4)
            #elif defined LPUART0_ON_B_HIGH
            _CONFIG_PERIPHERAL(B, 3, (PB_3_LPUART0_TX | UART_PULL_UPS)); // LPUART0_TX on PB2 (alt. function 3)
            #elif defined LPUART0_ON_B_ALT
            _CONFIG_PERIPHERAL(B, 2, (PB_2_LPUART0_TX | UART_PULL_UPS)); // LPUART0_TX on PB2 (alt. function 3)
            #else
            _CONFIG_PERIPHERAL(B, 1, (PB_1_LPUART0_TX | UART_PULL_UPS)); // LPUART0_TX on PB1 (alt. function 2)
            #endif
        #elif defined KINETIS_KL43 || defined KINETIS_KL27 || defined KINETIS_KL82 || defined KINETIS_K80
            #if !defined KINETIS_K80 && defined LPUART0_ON_E
            _CONFIG_PERIPHERAL(E, 20, (PE_20_LPUART0_TX | UART_PULL_UPS)); // LPUART0_TX on PE20 (alt. function 4)
            #elif defined KINETIS_KL43 && defined LPUART0_ON_D
            _CONFIG_PERIPHERAL(D, 7, (PD_7_LPUART0_TX | UART_PULL_UPS)); // LPUART0_TX on PD7 (alt. function 3)
            #elif defined LPUART0_ON_B
            _CONFIG_PERIPHERAL(B, 17, (PB_17_LPUART0_TX | UART_PULL_UPS)); // LPUART0_TX on PB17 (alt. function 3)
            #else
            _CONFIG_PERIPHERAL(A, 2, (PA_2_LPUART0_TX | UART_PULL_UPS)); // LPUART0_TX on PA2 (alt. function 2)
            #endif
        #else                                                            // K22, KV31
            #if defined LPUART0_ON_E
            _CONFIG_PERIPHERAL(E, 4, (PE_4_LPUART0_TX | UART_PULL_UPS)); // LPUART0_TX on PE4 (alt. function 3)
            #elif defined LPUART0_ON_B
            _CONFIG_PERIPHERAL(B, 11, (PB_11_LPUART0_TX | UART_PULL_UPS)); // LPUART0_TX on PB11 (alt. function 3)
            #elif defined LPUART0_ON_D_HIGH && defined KINETIS_K22
            _CONFIG_PERIPHERAL(D, 9, (PD_9_LPUART0_TX | UART_PULL_UPS)); // LPUART0_TX on PD9 (alt. function 5)
            #elif defined LPUART0_ON_D_LOW
            _CONFIG_PERIPHERAL(D, 3, (PD_3_LPUART0_TX | UART_PULL_UPS)); // LPUART0_TX on PD3 (alt. function 6)
            #elif defined LPUART0_ON_C_HIGH
            _CONFIG_PERIPHERAL(C, 17, (PC_17_LPUART0_TX | UART_PULL_UPS)); // LPUART0_TX on PC17 (alt. function 3)
            #else
            _CONFIG_PERIPHERAL(C, 4, (PC_4_LPUART0_TX | UART_PULL_UPS)); // LPUART0_TX on PC4 (alt. function 7)
            #endif
        #endif
            fnEnterInterrupt(irq_LPUART0_ID, PRIORITY_LPUART0, _LPSCI0_Interrupt); // enter LPUART0 interrupt handler
            break;
        #if LPUARTS_AVAILABLE > 1
            #if defined LPUARTS_PARALLEL
        case (UARTS_AVAILABLE + 1):
            #else
        case (1):                                                        // LPUART 1
            #endif
            #if defined KINETIS_KL43 || defined KINETIS_KL27 || defined KINETIS_K80
                #if defined LPUART1_ON_E
                _CONFIG_PERIPHERAL(E, 0, (PE_0_LPUART1_TX | UART_PULL_UPS)); // LPUART1_TX on PE0 (alt. function 3)
                #elif defined LPUART1_ON_C
                _CONFIG_PERIPHERAL(C, 4, (PC_4_LPUART1_TX | UART_PULL_UPS)); // LPUART1_TX on PC4 (alt. function 3)
                #elif !defined KINETIS_K80
                _CONFIG_PERIPHERAL(A, 19, (PA_19_LPUART1_TX | UART_PULL_UPS)); // LPUART1_TX on PA19 (alt. function 3)
                #endif
            #endif
            fnEnterInterrupt(irq_LPUART1_ID, PRIORITY_LPUART1, _LPSCI1_Interrupt); // enter LPUART1 interrupt handler
            break;
        #endif
        #if LPUARTS_AVAILABLE > 2
            #if defined LPUARTS_PARALLEL
        case (UARTS_AVAILABLE + 2):
            #else
        case (2):                                                        // LPUART 2
            #endif
            #if defined KINETIS_K80
               #if defined LPUART2_ON_E_LOW
            _CONFIG_PERIPHERAL(E, 12, (PE_12_LPUART2_TX | UART_PULL_UPS)); // LPUART2_TX on PE12 (alt. function 3)
                #elif defined LPUART2_ON_E_HIGH
            _CONFIG_PERIPHERAL(E, 16, (PE_16_LPUART2_TX | UART_PULL_UPS)); // LPUART2_TX on PE16 (alt. function 3)
                #else
            _CONFIG_PERIPHERAL(D, 3, (PD_3_LPUART2_TX | UART_PULL_UPS)); // LPUART2_TX on PD3 (alt. function 3)
                #endif
            #endif
            fnEnterInterrupt(irq_LPUART2_ID, PRIORITY_LPUART2, _LPSCI2_Interrupt); // enter LPUART2 interrupt handler
            break;
        #endif
        #if LPUARTS_AVAILABLE > 3
            #if defined LPUARTS_PARALLEL
        case (UARTS_AVAILABLE + 3):
            #else
        case (3):                                                        // LPUART 3
            #endif
            #if defined KINETIS_K80
               #if defined LPUART2_ON_E
            _CONFIG_PERIPHERAL(E, 4, (PE_4_LPUART3_TX | UART_PULL_UPS)); // LPUART3_TX on PE4 (alt. function 3)
                #elif defined LPUART3_ON_B
            _CONFIG_PERIPHERAL(B, 11, (PB_11_LPUART3_TX | UART_PULL_UPS)); // LPUART3_TX on PB11 (alt. function 3)
                #else
            _CONFIG_PERIPHERAL(C, 17, (PC_17_LPUART3_TX | UART_PULL_UPS)); // LPUART3_TX on PC17 (alt. function 3)
                #endif
            #endif
            fnEnterInterrupt(irq_LPUART3_ID, PRIORITY_LPUART3, _LPSCI3_Interrupt); // enter LPUART3 interrupt handler
            break;
        #endif
        #if LPUARTS_AVAILABLE > 4
            #if defined LPUARTS_PARALLEL
        case (UARTS_AVAILABLE + 4):
            #else
        case (4):                                                        // LPUART 4
            #endif
            #if defined KINETIS_K80
               #if defined LPUART4_ON_A
            _CONFIG_PERIPHERAL(A, 20, (PA_20_LPUART4_TX | UART_PULL_UPS)); // LPUART4_TX on PA20 (alt. function 3)
                #else
            _CONFIG_PERIPHERAL(C, 15, (PC_15_LPUART4_TX | UART_PULL_UPS)); // LPUART4_TX on PC15 (alt. function 3)
                #endif
            #endif
            fnEnterInterrupt(irq_LPUART4_ID, PRIORITY_LPUART4, _LPSCI4_Interrupt); // enter LPUART4 interrupt handler
            break;
        #endif
        }
        ((KINETIS_LPUART_CONTROL *)uart_reg)->LPUART_CTRL |= LPUART_CTRL_TE; // LPUART transmitter is enabled but not the transmission interrupt
        return;
        #if UARTS_AVAILABLE > 0        
    }
        #endif
    #endif
    #if UARTS_AVAILABLE > 0
    switch (Channel) {
        #if LPUARTS_AVAILABLE < 1 || defined LPUARTS_PARALLEL
    case 0:                                                              // configure the UART Tx 0 pin
        #if defined KINETIS_KL02 || defined KINETIS_KL03 || defined KINETIS_KL04 || defined KINETIS_KL05
            #if defined KINETIS_KL03 && defined UART0_OPTION_3
        _CONFIG_PERIPHERAL(A, 3, (PA_3_UART0_TX | UART_PULL_UPS));       // UART0_TX on PA3 (alt. function 4)
            #elif defined UART0_OPTION_2
        _CONFIG_PERIPHERAL(B, 3, (PB_3_UART0_TX | UART_PULL_UPS));       // UART0_TX on PB3 (alt. function 3)
            #elif defined UART0_OPTION_1
        _CONFIG_PERIPHERAL(B, 2, (PB_2_UART0_TX | UART_PULL_UPS));       // UART0_TX on PB2 (alt. function 3)
            #else
        _CONFIG_PERIPHERAL(B, 1, (PB_1_UART0_TX | UART_PULL_UPS));       // UART0_TX on PB1 (alt. function 2)
            #endif
        #elif defined KINETIS_K02
            #if defined UART0_A_LOW
        _CONFIG_PERIPHERAL(A, 2, (PA_2_UART0_TX | UART_PULL_UPS));       // UART0_TX on PA2 (alt. function 2)
            #elif defined UART0_ON_D
        _CONFIG_PERIPHERAL(D, 7, (PD_7_UART0_TX | UART_PULL_UPS));       // UART0_TX on PD7 (alt. function 3)
            #else
        _CONFIG_PERIPHERAL(B, 17, (PB_17_UART0_TX | UART_PULL_UPS));     // UART0_TX on PB17 (alt. function 3)
            #endif
        #elif defined KINETIS_KE
            #if defined UART0_ON_A
        SIM_PINSEL0 |= (SIM_PINSEL_UART0PS);
        _CONFIG_PERIPHERAL(A, 3, (PA_3_UART0_TX | UART_PULL_UPS));       // UART0_TX on PA3 (alt. function 2)
            #else
        _CONFIG_PERIPHERAL(B, 1, (PB_1_UART0_TX | UART_PULL_UPS));       // UART0_TX on PB1 (alt. function 2)
            #endif
        #else
            #if defined UART0_A_LOW
        _CONFIG_PERIPHERAL(A, 2, (PA_2_UART0_TX | UART_PULL_UPS));       // UART0_TX on PA2 (alt. function 2)
            #elif defined UART0_ON_B_LOW && defined KINETIS_KV31
        _CONFIG_PERIPHERAL(B, 1, (PB_1_UART0_TX | UART_PULL_UPS));       // UART0_TX on PB1 (alt. function 7)
            #elif defined UART0_ON_B
        _CONFIG_PERIPHERAL(B, 17, (PB_17_UART0_TX | UART_PULL_UPS));     // UART0_TX on PB17 (alt. function 3)
            #elif defined UART0_ON_D
        _CONFIG_PERIPHERAL(D, 7, (PD_7_UART0_TX | UART_PULL_UPS));       // UART0_TX on PD7 (alt. function 3)
            #elif defined KINETIS_KL && defined UART0_ON_E
        _CONFIG_PERIPHERAL(E, 20, (PE_20_UART0_TX | UART_PULL_UPS));     // UART0_TX on PE20 (alt. function 4)
            #else
        _CONFIG_PERIPHERAL(A, 14, (PA_14_UART0_TX | UART_PULL_UPS));     // UART0_TX on PA14 (alt. function 3)
            #endif
        #endif
        fnEnterInterrupt(irq_UART0_ID, PRIORITY_UART0, _SCI0_Interrupt); // enter UART0 interrupt handler
        break;
        #endif
        #if UARTS_AVAILABLE > 1 && (LPUARTS_AVAILABLE < 2 || defined LPUARTS_PARALLEL)
    case 1:                                                              // configure the UART Tx 1 pin
            #if defined KINETIS_KE
        _CONFIG_PERIPHERAL(C, 7, (PC_7_UART1_TX | UART_PULL_UPS));       // UART1_TX on PC7 (alt. function 2)
            #elif defined KINETIS_KV10
        _CONFIG_PERIPHERAL(D, 1, (PD_1_UART1_TX | UART_PULL_UPS));       // UART1_TX on PD1 (alt. function 5)
            #elif defined KINETIS_K02
        _CONFIG_PERIPHERAL(C, 4, (PC_4_UART1_TX | UART_PULL_UPS));       // UART1_TX on PC4 (alt. function 3)
            #else
                #if defined UART1_ON_C && !defined UART1_ON_A_TX
        _CONFIG_PERIPHERAL(C, 4, (PC_4_UART1_TX | UART_PULL_UPS));       // UART1_TX on PC4 (alt. function 3)
                #elif defined KINETIS_KL && (defined UART1_ON_A || defined UART1_ON_A_TX)
        _CONFIG_PERIPHERAL(A, 19, (PA_19_UART1_TX | UART_PULL_UPS));     // UART1_TX on PA19 (alt. function 3)
                #else
        _CONFIG_PERIPHERAL(E, 0, (PE_0_UART1_TX | UART_PULL_UPS));       // UART1_TX on PE0 (alt. function 3)
                #endif
            #endif
        fnEnterInterrupt(irq_UART1_ID, PRIORITY_UART1, _SCI1_Interrupt); // enter UART1 interrupt handler
        break;
        #endif
        #if (UARTS_AVAILABLE > 2 && (LPUARTS_AVAILABLE < 3 || defined LPUARTS_PARALLEL)) || (UARTS_AVAILABLE == 1 && LPUARTS_AVAILABLE == 2)
    case 2:                                                              // configure the UART Tx 2 pin
            #if defined KINETIS_KE
        _CONFIG_PERIPHERAL(D, 7, (PD_7_UART2_TX | UART_PULL_UPS));       // UART2_TX on PD7 (alt. function 2)
            #else
                #if (defined KINETIS_K61 || defined KINETIS_K70 || defined KINETIS_K21 || defined KINETIS_KL || defined KINETIS_KV31 || defined KINETIS_KW2X || defined KINETIS_K26 || defined KINETIS_K65) && defined UART2_ON_E // {25}
        _CONFIG_PERIPHERAL(E, 16, (PE_16_UART2_TX | UART_PULL_UPS));     // UART2_TX on PE16 (alt. function 3)
                #elif (defined KINETIS_K61 || defined KINETIS_K70) && defined UART2_ON_F // {25}
        _CONFIG_PERIPHERAL(F, 14, (PF_14_UART2_TX | UART_PULL_UPS));     // UART2_TX on PF14 (alt. function 4)
                #elif defined KINETIS_KL && defined UART2_ON_E_HIGH
        _CONFIG_PERIPHERAL(E, 22, (PE_22_UART2_TX | UART_PULL_UPS));     // UART2_TX on PE22 (alt. function 4)
                #elif defined KINETIS_KL && defined UART2_ON_D_HIGH
        _CONFIG_PERIPHERAL(D, 5, (PE_5_UART2_TX | UART_PULL_UPS));       // UART2_TX on PD5 (alt. function 3)
                #else
        _CONFIG_PERIPHERAL(D, 3, (PD_3_UART2_TX | UART_PULL_UPS));       // UART2_TX on PD3 (alt. function 3)
                #endif
            #endif
        fnEnterInterrupt(irq_UART2_ID, PRIORITY_UART2, _SCI2_Interrupt); // enter UART2 interrupt handler
        break;
        #endif
        #if UARTS_AVAILABLE > 3
    case 3:                                                              // configure the UART Tx 3 pin
            #if defined UART3_ON_B
        _CONFIG_PERIPHERAL(B, 11, (PB_11_UART3_TX | UART_PULL_UPS));     // UART3_TX on PB11 (alt. function 3)
            #elif defined UART3_ON_C
        _CONFIG_PERIPHERAL(C, 17, (PC_17_UART3_TX | UART_PULL_UPS));     // UART3_TX on PC17 (alt. function 3)
            #elif (defined KINETIS_K61 || defined KINETIS_K70) && defined UART3_ON_F // {25}
        _CONFIG_PERIPHERAL(F, 8, (PF_8_UART3_TX | UART_PULL_UPS));       // UART3_TX on PF8 (alt. function 4) {12}
            #else
        _CONFIG_PERIPHERAL(E, 4, (PE_4_UART3_TX | UART_PULL_UPS));       // UART3_TX on PE4 (alt. function 3)
            #endif
        fnEnterInterrupt(irq_UART3_ID, PRIORITY_UART3, _SCI3_Interrupt); // enter UART3 interrupt handler
        break;
        #endif
        #if UARTS_AVAILABLE > 4
    case 4:                                                              // configure the UART Tx 4 pin
            #if defined UART4_ON_C
        _CONFIG_PERIPHERAL(C, 15, (PC_15_UART4_TX | UART_PULL_UPS));     // UART4_TX on PC15 (alt. function 3)
            #else
        _CONFIG_PERIPHERAL(E, 24, (PE_24_UART4_TX | UART_PULL_UPS));     // UART4_TX on PE24 (alt. function 3)
            #endif
        fnEnterInterrupt(irq_UART4_ID, PRIORITY_UART4, _SCI4_Interrupt); // enter UART4 interrupt handler
        break;
        #endif
        #if UARTS_AVAILABLE > 5
    case 5:                                                              // configure the UART Tx 5 pin
            #if defined UART5_ON_D
        _CONFIG_PERIPHERAL(D, 9, (PD_9_UART5_TX | UART_PULL_UPS));       // UART5_TX on PD9 (alt. function 3)
            #else
        _CONFIG_PERIPHERAL(E, 8, (PE_8_UART5_TX | UART_PULL_UPS));       // UART5_TX on PE8 (alt. function 3)
            #endif
        fnEnterInterrupt(irq_UART5_ID, PRIORITY_UART5, _SCI5_Interrupt); // enter UART5 interrupt handler
        break;
        #endif
    }
    uart_reg->UART_C2 |= (UART_C2_TE);                                   // UART transmitter is enabled but not the transmission interrupt
        #if defined KINETIS_KE
    _SIM_PER_CHANGE;
        #endif
    #endif
}

// Disable transmission on the defined channel
//
extern void fnTxOff(QUEUE_HANDLE Channel)
{
    KINETIS_UART_CONTROL *uart_reg;
    #if NUMBER_EXTERNAL_SERIAL > 0
    if (Channel >= NUMBER_SERIAL) {
        fnExtSCITxOff((QUEUE_HANDLE)(Channel - NUMBER_SERIAL));          // pass on to external UART driver
        return;
    }
    #endif
    uart_reg = fnSelectChannel(Channel);
    #if LPUARTS_AVAILABLE > 0                                            // LPUART available in the device
        #if UARTS_AVAILABLE > 0                                          // also UART available
    if (uart_type[Channel] == UART_TYPE_LPUART) {                        // LPUART type
        #endif
        ((KINETIS_LPUART_CONTROL *)uart_reg)->LPUART_CTRL &= ~(LPUART_CTRL_TE | LPUART_CTRL_TIE); // disable LPUART transmitter and transmission interrupt (LPUART)
        return;
        #if UARTS_AVAILABLE > 0
    }
        #endif
    #endif
    #if UARTS_AVAILABLE > 0
    uart_reg->UART_C2 &= ~(UART_C2_TE | UART_C2_TIE | UART_C2_TCIE);     // disable UART transmitter and transmission interrupt
    #endif
}

// Enable reception on the defined channel - including configuring the receive data input
//
extern void fnRxOn(QUEUE_HANDLE Channel)
{
    KINETIS_UART_CONTROL *uart_reg;
    #if NUMBER_EXTERNAL_SERIAL > 0
    if (Channel >= NUMBER_SERIAL) {
        fnExtSCIRxOn((QUEUE_HANDLE)(Channel - NUMBER_SERIAL));           // pass on to external UART driver
        return;
    }
    #endif
    uart_reg = fnSelectChannel(Channel);
    #if LPUARTS_AVAILABLE > 0
        #if UARTS_AVAILABLE > 0
    if (uart_type[Channel] == UART_TYPE_LPUART) {                        // LPUART
        #endif
        switch (Channel) {
        #if defined LPUARTS_PARALLEL
        case UARTS_AVAILABLE:
        #else
        case 0:                                                          // LPUART 0
        #endif
        #if defined KINETIS_KL03
            #if defined LPUART0_ON_A
            _CONFIG_PERIPHERAL(A, 4, (PA_4_LPUART0_RX | UART_PULL_UPS)); // LPUART0_RX on PA4 (alt. function 4)
            #elif defined LPUART0_ON_B_HIGH
            _CONFIG_PERIPHERAL(B, 4, (PB_4_LPUART0_RX | UART_PULL_UPS)); // LPUART0_RX on PB4 (alt. function 3)
            #elif defined LPUART0_ON_B_ALT
            _CONFIG_PERIPHERAL(B, 1, (PB_1_LPUART0_RX | UART_PULL_UPS)); // LPUART0_RX on PB1 (alt. function 3)
            #else
            _CONFIG_PERIPHERAL(B, 2, (PB_2_LPUART0_RX | UART_PULL_UPS)); // LPUART0_RX on PB2 (alt. function 2)
            #endif
        #elif defined KINETIS_KL43 || defined KINETIS_KL27 || defined KINETIS_KL82 || defined KINETIS_K80
            #if !defined KINETIS_K80 && defined LPUART0_ON_E
            _CONFIG_PERIPHERAL(E, 21, (PE_21_LPUART0_RX | UART_PULL_UPS)); // LPUART0_RX on PE21 (alt. function 4)
            #elif (defined KINETIS_KL43 || defined KINETIS_K80) && defined LPUART0_ON_D
            _CONFIG_PERIPHERAL(D, 6, (PD_6_LPUART0_RX | UART_PULL_UPS)); // LPUART0_RX on PD6 (alt. function 3)
            #elif defined LPUART0_ON_B
            _CONFIG_PERIPHERAL(B, 16, (PB_16_LPUART0_RX | UART_PULL_UPS)); // LPUART0_RX on PB16 (alt. function 3)
            #elif defined LPUART0_ON_A_HIGH && defined KINETIS_K80
            _CONFIG_PERIPHERAL(A, 15, (PA_1_LPUART0_RX | UART_PULL_UPS)); // LPUART0_RX on PA15 (alt. function 3)
            #else
            _CONFIG_PERIPHERAL(A, 1, (PA_1_LPUART0_RX | UART_PULL_UPS)); // LPUART0_RX on PA1 (alt. function 2)
            #endif
        #else                                                            // K22, KV31
            #if defined LPUART0_ON_E
            _CONFIG_PERIPHERAL(E, 5, (PE_5_LPUART0_RX | UART_PULL_UPS)); // LPUART0_RX on PE5 (alt. function 3)
            #elif defined LPUART0_ON_B
            _CONFIG_PERIPHERAL(B, 10, (PB_10_LPUART0_RX | UART_PULL_UPS)); // LPUART0_RX on PB10 (alt. function 3)
            #elif defined LPUART0_ON_D_HIGH && defined KINETIS_K22
            _CONFIG_PERIPHERAL(D, 8, (PD_8_LPUART0_RX | UART_PULL_UPS)); // LPUART0_RX on PD8 (alt. function 5)
            #elif defined LPUART0_ON_D_LOW
            _CONFIG_PERIPHERAL(D, 2, (PD_2_LPUART0_RX | UART_PULL_UPS)); // LPUART0_RX on PD2 (alt. function 6)
            #elif defined LPUART0_ON_C_HIGH
            _CONFIG_PERIPHERAL(C, 16, (PC_16_LPUART0_RX | UART_PULL_UPS)); // LPUART0_RX on PC16 (alt. function 3)
            #else
            _CONFIG_PERIPHERAL(C, 3, (PC_3_LPUART0_RX | UART_PULL_UPS)); // LPUART0_RX on PC3 (alt. function 7)
            #endif
        #endif
            fnEnterInterrupt(irq_LPUART0_ID, PRIORITY_LPUART0, _LPSCI0_Interrupt); // enter LPUART0 interrupt handler
            break;
        #if LPUARTS_AVAILABLE > 1
            #if defined LPUARTS_PARALLEL
        case (UARTS_AVAILABLE + 1):
            #else
        case (1):                                                        // LPUART 1
            #endif
            #if defined KINETIS_KL43 || defined KINETIS_KL27 || defined KINETIS_K80
                #if defined LPUART1_ON_E
            _CONFIG_PERIPHERAL(E, 1, (PE_1_LPUART1_RX | UART_PULL_UPS)); // LPUART1_RX on PE1 (alt. function 3)
                #elif defined LPUART1_ON_C
            _CONFIG_PERIPHERAL(C, 3, (PC_3_LPUART1_RX | UART_PULL_UPS)); // LPUART1_RX on PC3 (alt. function 3)
                #elif !defined KINETIS_K80
            _CONFIG_PERIPHERAL(A, 18, (PA_18_LPUART1_RX | UART_PULL_UPS)); // LPUART1_RX on PA18 (alt. function 3)
                #endif
            #endif
            fnEnterInterrupt(irq_LPUART1_ID, PRIORITY_LPUART1, _LPSCI1_Interrupt); // enter LPUART1 interrupt handler
            break;
        #endif
        #if LPUARTS_AVAILABLE > 2
            #if defined LPUARTS_PARALLEL
        case (UARTS_AVAILABLE + 2):
            #else
        case (2):                                                        // LPUART 2
            #endif
            #if defined KINETIS_K80
               #if defined LPUART2_ON_E_LOW
            _CONFIG_PERIPHERAL(E, 13, (PE_13_LPUART2_RX | UART_PULL_UPS)); // LPUART2_RX on PE13 (alt. function 3)
                #elif defined LPUART2_ON_E_HIGH
            _CONFIG_PERIPHERAL(E, 17, (PE_17_LPUART2_RX | UART_PULL_UPS)); // LPUART2_RX on PE17 (alt. function 3)
                #else
            _CONFIG_PERIPHERAL(D, 2, (PD_2_LPUART2_RX | UART_PULL_UPS)); // LPUART2_RX on PD2 (alt. function 3)
                #endif
            #endif
            fnEnterInterrupt(irq_LPUART2_ID, PRIORITY_LPUART2, _LPSCI2_Interrupt); // enter LPUART2 interrupt handler
            break;
        #endif
        #if LPUARTS_AVAILABLE > 3
            #if defined LPUARTS_PARALLEL
        case (UARTS_AVAILABLE + 3):
            #else
        case (3):                                                        // LPUART 3
            #endif
            #if defined KINETIS_K80
               #if defined LPUART2_ON_E
            _CONFIG_PERIPHERAL(E, 5, (PE_3_LPUART3_RX | UART_PULL_UPS)); // LPUART3_RX on PE3 (alt. function 3)
                #elif defined LPUART3_ON_B
            _CONFIG_PERIPHERAL(B, 10, (PB_10_LPUART3_RX | UART_PULL_UPS)); // LPUART3_RX on PB10 (alt. function 3)
                #else
            _CONFIG_PERIPHERAL(C, 16, (PC_16_LPUART3_RX | UART_PULL_UPS)); // LPUART3_RX on PC16 (alt. function 3)
                #endif
            #endif
            fnEnterInterrupt(irq_LPUART3_ID, PRIORITY_LPUART3, _LPSCI3_Interrupt); // enter LPUART3 interrupt handler
            break;
        #endif
        #if LPUARTS_AVAILABLE > 4
            #if defined LPUARTS_PARALLEL
        case (UARTS_AVAILABLE + 4):
            #else
        case (4):                                                        // LPUART 4
            #endif
            #if defined KINETIS_K80
               #if defined LPUART4_ON_A
            _CONFIG_PERIPHERAL(A, 21, (PA_21_LPUART4_RX | UART_PULL_UPS)); // LPUART4_RX on PA21 (alt. function 3)
                #else
            _CONFIG_PERIPHERAL(C, 14, (PC_14_LPUART4_RX | UART_PULL_UPS)); // LPUART4_RX on PC14 (alt. function 3)
                #endif
            #endif
            fnEnterInterrupt(irq_LPUART4_ID, PRIORITY_LPUART4, _LPSCI4_Interrupt); // enter LPUART4 interrupt handler
            break;
        #endif
        }
        ((KINETIS_LPUART_CONTROL *)uart_reg)->LPUART_CTRL |= (LPUART_CTRL_RIE | LPUART_CTRL_RE); // enable LPUART receiver and reception interrupt
        #if defined KINETIS_KE
        _SIM_PER_CHANGE;
        #endif
        return;
        #if UARTS_AVAILABLE > 0
    }
        #endif
    #endif
    #if UARTS_AVAILABLE > 0
    switch (Channel) {
        #if (LPUARTS_AVAILABLE < 1 || defined LPUARTS_PARALLEL)
    case 0:                                                              // configure the UART Rx 0 pin
            #if defined KINETIS_KL02 || defined KINETIS_KL03 || defined KINETIS_KL04 || defined KINETIS_KL05
                #if defined KINETIS_KL03 && defined UART0_OPTION_3
        _CONFIG_PERIPHERAL(A, 4, (PA_4_UART0_RX | UART_PULL_UPS));       // UART0_RX on PA4 (alt. function 4)
                #elif defined UART0_OPTION_2
        _CONFIG_PERIPHERAL(B, 4, (PB_4_UART0_RX | UART_PULL_UPS));       // UART0_RX on PB4 (alt. function 3)
                #elif defined UART0_OPTION_1
        _CONFIG_PERIPHERAL(B, 1, (PB_1_UART0_RX | UART_PULL_UPS));       // UART0_RX on PB1 (alt. function 3)
                #else
        _CONFIG_PERIPHERAL(B, 2, (PB_2_UART0_RX | UART_PULL_UPS));       // UART0_RX on PB2 (alt. function 2)
                #endif
            #elif defined KINETIS_K02
                #if defined UART0_A_LOW
        _CONFIG_PERIPHERAL(A, 1, (PA_1_UART0_RX | UART_PULL_UPS));       // UART0_RX on PA1 (alt. function 2)
                #elif defined UART0_ON_D
        _CONFIG_PERIPHERAL(D, 6, (PD_6_UART0_RX | UART_PULL_UPS));       // UART0_RX on PD6 (alt. function 3)
                #else
        _CONFIG_PERIPHERAL(B, 16, (PB_16_UART0_RX | UART_PULL_UPS));     // UART0_RX on PB16 (alt. function 3)
                #endif
            #elif defined KINETIS_KE
                #if defined UART0_ON_A
        SIM_PINSEL0 |= (SIM_PINSEL_UART0PS);
        _CONFIG_PERIPHERAL(A, 2, (PA_2_UART0_RX | UART_PULL_UPS));       // UART0_RX on PA2 (alt. function 2)
                #else
        _CONFIG_PERIPHERAL(B, 0, (PB_0_UART0_RX | UART_PULL_UPS));       // UART0_RX on PB0 (alt. function 2)
                #endif
            #else
                #if defined UART0_A_LOW
        _CONFIG_PERIPHERAL(A, 1, (PA_1_UART0_RX | UART_PULL_UPS));       // UART0_RX on PA1 (alt. function 2)
                #elif defined UART0_ON_B_LOW && defined KINETIS_KV31
        _CONFIG_PERIPHERAL(B, 0, (PB_0_UART0_RX | UART_PULL_UPS));       // UART0_RX on PB0 (alt. function 7)
                #elif defined UART0_ON_B
        _CONFIG_PERIPHERAL(B, 16, (PB_16_UART0_RX | UART_PULL_UPS));     // UART0_RX on PB16 (alt. function 3)
                #elif defined UART0_ON_D
        _CONFIG_PERIPHERAL(D, 6, (PD_6_UART0_RX | UART_PULL_UPS));       // UART0_RX on PD6 (alt. function 3)
                #elif defined KINETIS_KL && defined UART0_ON_E
        _CONFIG_PERIPHERAL(E, 21, (PE_21_UART0_RX | UART_PULL_UPS));     // UART0_RX on PE21 (alt. function 4)
                #else
        _CONFIG_PERIPHERAL(A, 15, (PA_15_UART0_RX | UART_PULL_UPS));     // UART0_RX on PA15 (alt. function 3)
                #endif
            #endif
        fnEnterInterrupt(irq_UART0_ID, PRIORITY_UART0, _SCI0_Interrupt); // enter UART0 interrupt handler
        break;
        #endif
        #if UARTS_AVAILABLE > 1 && LPUARTS_AVAILABLE < 2
    case 1:                                                              // configure the UART Rx 1 pin
            #if defined KINETIS_KE
        _CONFIG_PERIPHERAL(C, 6, (PC_6_UART1_RX | UART_PULL_UPS));       // UART1_RX on PC6 (alt. function 2)
            #elif defined KINETIS_KV10
        _CONFIG_PERIPHERAL(D, 0, (PD_0_UART1_RX | UART_PULL_UPS));       // UART1_RX on PD0 (alt. function 5)
            #elif defined KINETIS_K02
        _CONFIG_PERIPHERAL(C, 3, (PC_3_UART1_RX | UART_PULL_UPS));       // UART1_RX on PC3 (alt. function 3)
            #else
                #if defined UART1_ON_C
        _CONFIG_PERIPHERAL(C, 3, (PC_3_UART1_RX | UART_PULL_UPS));       // UART1_RX on PC3 (alt. function 3)
                #elif defined KINETIS_KL && defined UART1_ON_A
        _CONFIG_PERIPHERAL(A, 18, (PA_18_UART1_RX | UART_PULL_UPS));     // UART1_RX on PA18 (alt. function 3)
                #else
        _CONFIG_PERIPHERAL(E, 1, (PE_1_UART1_RX | UART_PULL_UPS));       // UART1_RX on PE1 (alt. function 3)
                #endif
            #endif
        fnEnterInterrupt(irq_UART1_ID, PRIORITY_UART1, _SCI1_Interrupt); // enter UART1 interrupt handler
        break;
        #endif
        #if (UARTS_AVAILABLE > 2 && LPUARTS_AVAILABLE < 3) || (UARTS_AVAILABLE == 1 && LPUARTS_AVAILABLE == 2)
    case 2:                                                              // configure the UART Rx 2 pin
            #if defined KINETIS_KE
        _CONFIG_PERIPHERAL(D, 6, (PD_6_UART2_RX | UART_PULL_UPS));       // UART2_RX on PD6 (alt. function 2)
            #else
                #if (defined KINETIS_K61 || defined KINETIS_K70 || defined KINETIS_K21 || defined KINETIS_KL || defined KINETIS_KV31 || defined KINETIS_KW2X || defined KINETIS_K26 || defined KINETIS_K65) && defined UART2_ON_E // {25}
        _CONFIG_PERIPHERAL(E, 17, (PE_17_UART2_RX | UART_PULL_UPS));     // UART2_RX on PE17 (alt. function 3)
                #elif (defined KINETIS_K61 || defined KINETIS_K70) && defined UART2_ON_F // {25}
        _CONFIG_PERIPHERAL(F, 13, (PF_13_UART2_RX | UART_PULL_UPS));     // UART2_RX on PF13 (alt. function 4)
                #elif defined KINETIS_KL && defined UART2_ON_E_HIGH
        _CONFIG_PERIPHERAL(E, 23, (PE_23_UART2_RX | UART_PULL_UPS));     // UART2_RX on PE23 (alt. function 4)
                #elif defined KINETIS_KL && defined UART2_ON_D_HIGH
        _CONFIG_PERIPHERAL(D, 4, (PE_4_UART2_RX | UART_PULL_UPS));       // UART2_RX on PD4 (alt. function 3)
                #else
        _CONFIG_PERIPHERAL(D, 2, (PD_2_UART2_RX | UART_PULL_UPS));       // UART2_RX on PD2 (alt. function 3)
                #endif
            #endif
        fnEnterInterrupt(irq_UART2_ID, PRIORITY_UART2, _SCI2_Interrupt); // enter UART2 interrupt handler
        break;
        #endif
        #if UARTS_AVAILABLE > 3
    case 3:                                                              // configure the UART Rx 3 pin
            #if defined UART3_ON_B
        _CONFIG_PERIPHERAL(B, 10, (PB_10_UART3_RX | UART_PULL_UPS));     // UART3_RX on PB10 (alt. function 3)
            #elif defined UART3_ON_C
        _CONFIG_PERIPHERAL(C, 16, (PC_16_UART3_RX | UART_PULL_UPS));     // UART3_RX on PC16 (alt. function 3)
            #elif (defined KINETIS_K61 || defined KINETIS_K70) && defined UART3_ON_F // {25}
        _CONFIG_PERIPHERAL(F, 7, (PF_7_UART3_RX | UART_PULL_UPS));       // UART3_RX on PF7 (alt. function 4) {12}
            #else
        _CONFIG_PERIPHERAL(E, 5, (PE_5_UART3_RX | UART_PULL_UPS));       // UART3_RX on PE5 (alt. function 3)
            #endif
        fnEnterInterrupt(irq_UART3_ID, PRIORITY_UART3, _SCI3_Interrupt); // enter UART3 interrupt handler
        break;
        #endif
        #if UARTS_AVAILABLE > 4
    case 4:                                                              // configure the UART Rx 4 pin
            #if defined UART4_ON_C
        _CONFIG_PERIPHERAL(C, 14, (PC_14_UART4_RX | UART_PULL_UPS));     // UART4_RX on PC14 (alt. function 3)
            #else
        _CONFIG_PERIPHERAL(E, 25, (PE_25_UART4_RX | UART_PULL_UPS));     // UART4_RX on PE25 (alt. function 3)
            #endif
        fnEnterInterrupt(irq_UART4_ID, PRIORITY_UART4, _SCI4_Interrupt); // enter UART4 interrupt handler
        break;
        #endif
        #if UARTS_AVAILABLE > 5
    case 5:                                                              // configure the UART Rx 5 pin
            #if defined UART5_ON_D
        _CONFIG_PERIPHERAL(D, 8, (PD_8_UART5_RX | UART_PULL_UPS));       // UART5_RX on PD8 (alt. function 3)
            #else
        _CONFIG_PERIPHERAL(E, 9, (PE_9_UART5_RX | UART_PULL_UPS));       // UART5_RX on PE9 (alt. function 3)
            #endif
        fnEnterInterrupt(irq_UART5_ID, PRIORITY_UART5, _SCI5_Interrupt); // enter UART5 interrupt handler
        break;
        #endif
    }
    uart_reg->UART_C2 |= (UART_C2_RE | UART_C2_RIE);                     // enable UART receiver and reception interrupt (or DMA)
        #if defined KINETIS_KE
    _SIM_PER_CHANGE;
        #endif
    #endif
}

// Disable reception on the defined channel
//
extern void fnRxOff(QUEUE_HANDLE Channel)
{
    KINETIS_UART_CONTROL *uart_reg;
    #if NUMBER_EXTERNAL_SERIAL > 0
    if (Channel >= NUMBER_SERIAL) {
        fnExtSCIRxOff((QUEUE_HANDLE)(Channel - NUMBER_SERIAL));          // pass on to external UART driver
        return;
    }
    #endif
    uart_reg = fnSelectChannel(Channel);
    #if LPUARTS_AVAILABLE > 0
        #if UARTS_AVAILABLE > 0
    if (uart_type[Channel] == UART_TYPE_LPUART) {                        // LPUART
        #endif
        ((KINETIS_LPUART_CONTROL *)uart_reg)->LPUART_CTRL &= ~(LPUART_CTRL_RIE | LPUART_CTRL_RE); // disable LPUART receiver and reception interrupts (LPUART)
        return;
        #if UARTS_AVAILABLE > 0
    }
        #endif
    #endif
    #if UARTS_AVAILABLE > 0
    uart_reg->UART_C2 &= ~(UART_C2_RE | UART_C2_RIE);                    // disable UART receiver and reception interrupts (UART)
    #endif
}

#if LPUARTS_AVAILABLE > 0
// LPUART configuration
//
static void fnConfigLPUART(QUEUE_HANDLE Channel, TTYTABLE *pars, KINETIS_LPUART_CONTROL *lpuart_reg, unsigned short usDivider)
{
    lpuart_reg->LPUART_BAUD = ((lpuart_reg->LPUART_BAUD & ~LPUART_BAUD_SBR) | (usDivider | LPUART_BAUD_OSR_16)); // set the (new) baud rate
    uDisable_Interrupt();                                                // protect this region in case transmit interrupt is presently enabled
    {                                                                    // {204}
        unsigned long ulCtrlReg = (lpuart_reg->LPUART_CTRL & (LPUART_CTRL_TCIE | LPUART_CTRL_TIE)); // preserve only the transmit interrupt values (in case a byte is presently being transmitted)
        if ((pars->Config & CHAR_7) != 0) {                              // Kinetis supports only one fixed stop bit on its main UART(s)
            ucUART_mask[Channel] = 0x7f;                                 // set 7 bit mask to remove parity bits
        }
        else {
            ucUART_mask[Channel] = 0xff;                                 // full 8 bit mask
        }
        if ((pars->Config & (RS232_ODD_PARITY | RS232_EVEN_PARITY)) != 0) { // if parity is enabled
            if ((pars->Config & RS232_ODD_PARITY) != 0) {
                ulCtrlReg |= (LPUART_CTRL_PE | LPUART_CTRL_PT_ODD);      // odd parity
            }
            else {                                                       
                ulCtrlReg |= (LPUART_CTRL_PE | LPUART_CTRL_PT_EVEN);     // even parity
            }
        }
        lpuart_reg->LPUART_CTRL = ulCtrlReg;                             // set the new control register vaue
    }
    uEnable_Interrupt();                                                 // enable interrupt again (transmit interrupt enable has been preserved)
    if ((pars->Config & TWO_STOPS) != 0) {                               // LPUART supports 2 stop bits
        lpuart_reg->LPUART_BAUD |= LPUART_BAUD_SBNS_2;                   // set 2 stop bits
    }
    #if defined SERIAL_SUPPORT_DMA                                       // only transmit DMA supported due to limited DMA channels
    if ((pars->ucDMAConfig & UART_TX_DMA) != 0) {
        #if defined KINETIS_KL
        KINETIS_DMA *ptrDMA = (KINETIS_DMA *)DMA_BLOCK;
        if ((lpuart_reg->LPUART_BAUD & LPUART_BAUD_TDMAE) != 0) {
            return;                                                      // if the DMA has already been configured don't disturb it
        }
        ptrDMA += UART_DMA_TX_CHANNEL[Channel];
        ptrDMA->DMA_DSR_BCR = DMA_DSR_BCR_DONE;                          // clear the DONE flag and clear errors etc.
        ptrDMA->DMA_DAR = (unsigned long)&(lpuart_reg->LPUART_DATA);     // destination is the LPUART's data register
        POWER_UP(6, SIM_SCGC6_DMAMUX0);                                  // enable DMA multiplexer 0
        fnEnterInterrupt((irq_DMA0_ID + UART_DMA_TX_CHANNEL[Channel]), UART_DMA_TX_INT_PRIORITY[Channel], (void (*)(void))_uart_tx_dma_Interrupt[Channel]); // enter DMA interrupt handler
        lpuart_reg->LPUART_CTRL &= ~(LPUART_CTRL_TIE | LPUART_CTRL_TCIE);// ensure tx interrupt is not enabled
        lpuart_reg->LPUART_BAUD |= LPUART_BAUD_TDMAE;                    // use DMA rather than interrupts for transmission
        ptrDMA->DMA_DCR = (DMA_DCR_SINC | DMA_DCR_D_REQ | DMA_DCR_DSIZE_8 | DMA_DCR_SSIZE_8 | DMA_DCR_DMOD_OFF | DMA_DCR_SMOD_OFF | DMA_DCR_CS | DMA_DCR_EINT); // set mode and protect from interrupts that could use the function in the process
        *(unsigned char *)(DMAMUX0_BLOCK + UART_DMA_TX_CHANNEL[Channel]) = ((DMAMUX_CHCFG_SOURCE_UART0_TX + (2 * Channel)) | DMAMUX_CHCFG_ENBL); // connect UART tx to DMA channel
        #else
        KINETIS_DMA_TDC *ptrDMA_TCD = (KINETIS_DMA_TDC *)eDMA_DESCRIPTORS;
        ptrDMA_TCD += UART_DMA_TX_CHANNEL[Channel];
        ptrDMA_TCD->DMA_TCD_SOFF = 1;                                    // source increment one byte
        ptrDMA_TCD->DMA_TCD_DOFF = 0;                                    // destination not incremented
      //_SET_DMA_CHANNEL_PRIORITY(UART_DMA_TX_CHANNEL[Channel], UART_DMA_TX_PRIORITY[Channel]); // DMA priority, whereby channel can suspend a lower priority channel (not changed since we use the channel's inherent priority)
        ptrDMA_TCD->DMA_TCD_ATTR = (DMA_TCD_ATTR_DSIZE_8 | DMA_TCD_ATTR_SSIZE_8); // transfer sizes always single bytes
        ptrDMA_TCD->DMA_TCD_DADDR = (unsigned long)&(lpuart_reg->LPUART_DATA);  // destination is the LPUART's data register
        ptrDMA_TCD->DMA_TCD_SLAST = ptrDMA_TCD->DMA_TCD_DLASTSGA = 0;    // {107} no change to address when the buffer has filled
        ptrDMA_TCD->DMA_TCD_NBYTES_ML = 1;                               // each request starts a single transfer
        ptrDMA_TCD->DMA_TCD_CSR = (DMA_TCD_CSR_DREQ | DMA_TCD_CSR_INTMAJOR); // stop after the defined number of service requests and interrupt on completion
        fnEnterInterrupt((irq_DMA0_ID + UART_DMA_TX_CHANNEL[Channel]), UART_DMA_TX_INT_PRIORITY[Channel], (void (*)(void))_uart_tx_dma_Interrupt[Channel]); // enter DMA interrupt handler
        lpuart_reg->LPUART_CTRL &= ~(LPUART_CTRL_TIE | LPUART_CTRL_TCIE);// ensure tx interrupt is not enabled
        lpuart_reg->LPUART_BAUD |= LPUART_BAUD_TDMAE;                    // use DMA rather than interrupts for transmission
        POWER_UP(6, SIM_SCGC6_DMAMUX0);                                  // enable DMA multiplexer 0
        *(unsigned char *)(DMAMUX0_BLOCK + UART_DMA_TX_CHANNEL[Channel]) = ((DMAMUX0_CHCFG_SOURCE_LPUART0_TX + (2 * (Channel - UARTS_AVAILABLE))) | DMAMUX_CHCFG_ENBL); // connect LPUART tx to DMA channel
        #endif
    }
    else {                                                               // interrupt driven transmitter
        lpuart_reg->LPUART_BAUD &= ~LPUART_BAUD_TDMAE;                   // disable tx DMA so that tx interrupt mode can be used
    }
    #endif
}
#endif

#if UARTS_AVAILABLE > 0
// UART configuration
//
static void fnConfigUART(QUEUE_HANDLE Channel, TTYTABLE *pars, KINETIS_UART_CONTROL *uart_reg, unsigned short usDivider)
{
    uart_reg->UART_BDH = (unsigned char)((usDivider >> 8) & 0x1f);       // program speed to divisor latch registers
    uart_reg->UART_BDL = (unsigned char)usDivider;                       // the complete baud rate generator setting is committed when this byte is written

    if ((pars->Config & CHAR_7) != 0) {                                  // Kinetis supports only one fixed stop bit on its main UART(s)
        ucUART_mask[Channel] = 0x7f;                                     // set 7 bit mask to remove parity bits
        if ((pars->Config & (RS232_ODD_PARITY | RS232_EVEN_PARITY)) != 0) {
            if ((pars->Config & RS232_ODD_PARITY) != 0) {
                uart_reg->UART_C1 = (UART_C1_PE | UART_C1_PT_ODD);       // program odd parity
            }
            else {                                                       // program even parity
                uart_reg->UART_C1 = (UART_C1_PE | UART_C1_PT_EVEN);
            }
        }
        else {
            uart_reg->UART_C1 = UART_C1_PARITY_DISABLED;                 // no parity - the UART in the Kinetis will use address mark at the 8th bit position in 7 bit character mode without a parity setting
        }
    }
    else {
        ucUART_mask[Channel] = 0xff;                                     // full 8 bit mask
        if ((pars->Config & (RS232_ODD_PARITY | RS232_EVEN_PARITY)) != 0) {
            if ((pars->Config & RS232_ODD_PARITY) != 0) {
                uart_reg->UART_C1 = (UART_C1_M | UART_C1_PE | UART_C1_PT_ODD); // program odd parity
            }
            else {
                uart_reg->UART_C1 = (UART_C1_M | UART_C1_PE | UART_C1_PT_EVEN); // program even parity
            }
        }
        else {
            uart_reg->UART_C1 = UART_C1_PARITY_DISABLED;                 // no parity - the UART in the Kinetis will use address mark at the 8th bit position in 7 bit character mode without a parity setting
        }
    }
    #if (defined KINETIS_KL || defined KINETIS_KE) && defined UART_FRAME_END_COMPLETE
    if ((pars->Config & INFORM_ON_FRAME_TRANSMISSION) != 0) {            // {200}
        ucReportEndOfFrame[Channel] = 1;                                 // we want to work with a frame completion interrupt
    }
    #endif
    #if defined TRUE_UART_TX_2_STOPS
    ucStops[Channel] = 0;                                                // default is 1 stop bit
    #endif
    #if defined KINETIS_KL && (UARTS_AVAILABLE > 1)
    if ((Channel > 0) && ((pars->Config & TWO_STOPS) != 0)) {            // simplified UARTs (above UART0) support 2 stop bits
        uart_reg->UART_BDH |= (UART_BDH_SBNS);                           // set 2 stop bits
        #if defined SERIAL_SUPPORT_DMA
        goto _configDMA;
        #else
        return;
        #endif
    }
    #endif
    #if defined TRUE_UART_TX_2_STOPS                                     // control transmitter 2 stop bits using interrupt as long as not operating in DMA mode
        #if defined SERIAL_SUPPORT_DMA
    if (((pars->ucDMAConfig & UART_TX_DMA) == 0) && ((pars->Config & TWO_STOPS) != 0)) {
        ucStops[Channel] = 1;                                            // mark that the end of transmission interrupt is to be used instead of the transmit empty interrupt - this causes an extra stop bit to be inserted betwen ncharacters and so 2-stop bits to be acheived
    }
        #else
    if ((pars->Config & TWO_STOPS) != 0) {
        ucStops[Channel] = 1;                                            // mark that the end of transmission interrupt is to be used instead of the transmit empty interrupt - this causes an extra stop bit to be inserted betwen ncharacters and so 2-stop bits to be acheived
    }
        #endif
    #endif
    #if defined SERIAL_SUPPORT_DMA                                       // {6}
        #if defined KINETIS_KL && (UARTS_AVAILABLE > 1)
_configDMA:
        #endif
    if ((pars->ucDMAConfig & UART_TX_DMA) != 0) {
        #if defined KINETIS_KL                                           // {81}
        KINETIS_DMA *ptrDMA = (KINETIS_DMA *)DMA_BLOCK;
            #if UARTS_AVAILABLE > 1
        if (Channel == 0) {
            if ((uart_reg->UART_C5 & UART_C5_TDMAS) != 0) {              // {203} if DMA has already been configured we quit re-configuration
                return;
            }
        }
        else {
            if ((uart_reg->UART_MA1_C4 & UART_C4_TDMAS) != 0) {          // {203} if DMA has already been configured we quit re-configuration
                return;
            }
        }
            #else
        if ((uart_reg->UART_C5 & UART_C5_TDMAS) != 0) {                  // {203} if DMA has already been configured we quit re-configuration
            return;
        }
            #endif
        ptrDMA += UART_DMA_TX_CHANNEL[Channel];
        ptrDMA->DMA_DSR_BCR = DMA_DSR_BCR_DONE;                          // clear the DONE flag and clear errors etc.
        ptrDMA->DMA_DAR = (unsigned long)&(uart_reg->UART_D);            // destination is the UART's data register
        POWER_UP(6, SIM_SCGC6_DMAMUX0);                                  // enable DMA multiplexer 0
        fnEnterInterrupt((irq_DMA0_ID + UART_DMA_TX_CHANNEL[Channel]), UART_DMA_TX_INT_PRIORITY[Channel], (void (*)(void))_uart_tx_dma_Interrupt[Channel]); // enter DMA interrupt handler
        uart_reg->UART_C2 &= ~(UART_C2_TIE | UART_C2_TCIE);              // ensure tx interrupt is not enabled
            #if UARTS_AVAILABLE > 1
        if (Channel == 0) {
            uart_reg->UART_C5 |= UART_C5_TDMAS;                          // use DMA rather than interrupts for transmission
        }
        else {
            uart_reg->UART_MA1_C4 |= UART_C4_TDMAS;                      // use DMA rather than interrupts for transmission
            uart_reg->UART_C2 |= (UART_C2_TIE);                          // enable the tx dma request (DMA not yet enabled) rather than interrupt mode
        }
            #else
                #if defined KINETIS_KL43                                 // the UART in the KL43 behaves like one in the K devies with respect to needing the interrupt enabled for DMA to be triggered
        uart_reg->UART_C2 |= (UART_C2_TIE);                              // enable the tx dma request (DMA not yet enabled) rather than interrupt mode
                #endif
        uart_reg->UART_C5 |= UART_C5_TDMAS;                              // use DMA rather than interrupts for transmission
            #endif
        ptrDMA->DMA_DCR = (DMA_DCR_SINC | DMA_DCR_D_REQ | DMA_DCR_DSIZE_8 | DMA_DCR_SSIZE_8 | DMA_DCR_DMOD_OFF | DMA_DCR_SMOD_OFF | DMA_DCR_CS | DMA_DCR_EINT); // set mode and protect from interrupts that could use the function in the process
        *(unsigned char *)(DMAMUX0_BLOCK + UART_DMA_TX_CHANNEL[Channel]) = ((DMAMUX_CHCFG_SOURCE_UART0_TX + (2 * Channel)) | DMAMUX_CHCFG_ENBL); // connect UART tx to DMA channel
        #else
        KINETIS_DMA_TDC *ptrDMA_TCD = (KINETIS_DMA_TDC *)eDMA_DESCRIPTORS;
        if ((uart_reg->UART_C2 & UART_C2_TIE) != 0) {                    // {203} if DMA has already been configured we quit re-configuration
            return;
        }
        ptrDMA_TCD += UART_DMA_TX_CHANNEL[Channel];
        ptrDMA_TCD->DMA_TCD_SOFF = 1;                                    // source increment one byte
        ptrDMA_TCD->DMA_TCD_DOFF = 0;                                    // destination not incremented
      //_SET_DMA_CHANNEL_PRIORITY(UART_DMA_TX_CHANNEL[Channel], UART_DMA_TX_PRIORITY[Channel]); // DMA priority, whereby channel can suspend a lower priority channel (not changed since we use the channel's inherent priority)
        ptrDMA_TCD->DMA_TCD_ATTR = (DMA_TCD_ATTR_DSIZE_8 | DMA_TCD_ATTR_SSIZE_8); // transfer sizes always single bytes
        ptrDMA_TCD->DMA_TCD_DADDR = (unsigned long)&(uart_reg->UART_D);  // destination is the UART's data register
        ptrDMA_TCD->DMA_TCD_SLAST = ptrDMA_TCD->DMA_TCD_DLASTSGA = 0;    // {107} no change to address when the buffer has filled
        ptrDMA_TCD->DMA_TCD_NBYTES_ML = 1;                               // each request starts a single transfer
        ptrDMA_TCD->DMA_TCD_CSR = (DMA_TCD_CSR_DREQ | DMA_TCD_CSR_INTMAJOR); // stop after the defined number of service requests and interrupt on completion
        fnEnterInterrupt((irq_DMA0_ID + UART_DMA_TX_CHANNEL[Channel]), UART_DMA_TX_INT_PRIORITY[Channel], (void (*)(void))_uart_tx_dma_Interrupt[Channel]); // enter DMA interrupt handler
        uart_reg->UART_C5 |= UART_C5_TDMAS;                              // use DMA rather than interrupts for transmission
        POWER_UP(6, SIM_SCGC6_DMAMUX0);                                  // enable DMA multiplexer 0
            #if ((defined KINETIS_K21 || defined KINETIS_K22) && (UARTS_AVAILABLE > 4)) || defined KINETIS_K64
        if (Channel > 3) {                                               // channels 4 and 5 each share DMA source for TX and RX
            *(unsigned char *)(DMAMUX0_BLOCK + UART_DMA_TX_CHANNEL[Channel]) = ((DMAMUX_CHCFG_SOURCE_UART3_TX + (Channel - 3)) | DMAMUX_CHCFG_ENBL); // connect UART tx to DMA channel
        }
        else {
            *(unsigned char *)(DMAMUX0_BLOCK + UART_DMA_TX_CHANNEL[Channel]) = ((DMAMUX_CHCFG_SOURCE_UART0_TX + (2 * Channel)) | DMAMUX_CHCFG_ENBL); // connect UART tx to DMA channel
        }
            #else
        *(unsigned char *)(DMAMUX0_BLOCK + UART_DMA_TX_CHANNEL[Channel]) = ((DMAMUX_CHCFG_SOURCE_UART0_TX + (2 * Channel)) | DMAMUX_CHCFG_ENBL); // connect UART tx to DMA channel
            #endif
        uart_reg->UART_C2 |= (UART_C2_TIE);                              // enable the tx dma request (DMA not yet enabled) rather than interrupt mode
        #endif
    }
    else {                                                               // interrupt driven transmitter
        #if defined KINETIS_KL && (UARTS_AVAILABLE > 1)
        if (Channel == 0) {
            uart_reg->UART_C5 &= ~(UART_C5_TDMAS);                       // disable tx DMA so that tx interrupt mode can be used
        }
        else {
            uart_reg->UART_MA1_C4 &= ~(UART_C4_TDMAS);                   // disable tx DMA so that tx interrupt mode can be used
        }
        #else
        uart_reg->UART_C5 &= ~(UART_C5_TDMAS);                           // disable tx DMA so that tx interrupt mode can be used
        #endif
    }
        #if defined SERIAL_SUPPORT_DMA_RX
    if ((pars->ucDMAConfig & UART_RX_DMA) != 0) {                        // {8}
        KINETIS_DMA_TDC *ptrDMA_TCD = (KINETIS_DMA_TDC *)eDMA_DESCRIPTORS;
        ptrDMA_TCD += UART_DMA_RX_CHANNEL[Channel];                      // set the DMA channel register
        fnEnterInterrupt((irq_DMA0_ID + UART_DMA_RX_CHANNEL[Channel]), UART_DMA_RX_INT_PRIORITY[Channel], (void (*)(void))_uart_rx_dma_Interrupt[Channel]); // enter DMA interrupt handler
            #if defined KINETIS_KL && (UARTS_AVAILABLE > 1)
        if (Channel == 0) {
            uart_reg->UART_C5 |= UART_C5_RDMAS;                          // use DMA rather than interrupts for reception
        }
        else {
            uart_reg->UART_MA1_C4 |= UART_C4_RDMAS;                      // use DMA rather than interrupts for reception
        }
            #else
        uart_reg->UART_C5 |= UART_C5_RDMAS;                              // use DMA rather than interrupts for reception
            #endif
        POWER_UP(6, SIM_SCGC6_DMAMUX0);                                  // enable DMA multiplexer 0
        *(unsigned char *)(DMAMUX0_BLOCK + UART_DMA_RX_CHANNEL[Channel]) = ((DMAMUX_CHCFG_SOURCE_UART0_RX + (2 * Channel)) | DMAMUX_CHCFG_ENBL); // connect UART rx to DMA channel
        ptrDMA_TCD->DMA_TCD_BITER_ELINK = ptrDMA_TCD->DMA_TCD_CITER_ELINK = pars->Rx_tx_sizes.RxQueueSize; // the length of the input buffer in use
        ptrDMA_TCD->DMA_TCD_SOFF = 0;                                    // source not increment
        ptrDMA_TCD->DMA_TCD_DOFF = 1;                                    // destination incremented
      //_SET_DMA_CHANNEL_PRIORITY(UART_DMA_RX_CHANNEL[Channel], UART_DMA_RX_PRIORITY[Channel]); // DMA priority, whereby channel can suspend a lower priority channel (not changed since we use the channel's inherent priority)
        ptrDMA_TCD->DMA_TCD_ATTR = (DMA_TCD_ATTR_DSIZE_8 | DMA_TCD_ATTR_SSIZE_8); // transfer sizes always single bytes
        ptrDMA_TCD->DMA_TCD_SADDR = (unsigned long)&(uart_reg->UART_D);  // source is the UART's data register
        ptrDMA_TCD->DMA_TCD_NBYTES_ML = 1;                               // each request starts a single transfer
        if ((pars->ucDMAConfig & (UART_RX_DMA_HALF_BUFFER | UART_RX_DMA_FULL_BUFFER)) != 0) { // if operating in half-buffer or full buffer mode
            if ((pars->ucDMAConfig & UART_RX_DMA_HALF_BUFFER) != 0) {
                ptrDMA_TCD->DMA_TCD_CSR = (DMA_TCD_CSR_INTMAJOR | DMA_TCD_CSR_INTHALF); // never disable and inform on half and full buffer
            }
            else {
                ptrDMA_TCD->DMA_TCD_CSR = (DMA_TCD_CSR_INTMAJOR);        // never disable and inform on full buffer
            }
        }
            #if defined SERIAL_SUPPORT_DMA_RX_FREERUN
        else {
            ptrDMA_TCD->DMA_TCD_CSR = 0;                                 // continuous
            usDMA_progress[Channel] = ptrDMA_TCD->DMA_TCD_BITER_ELINK;
        }
            #endif
        ptrDMA_TCD->DMA_TCD_SLAST = 0;                                   // {56} no change to address when the buffer has filled
        ptrDMA_TCD->DMA_TCD_DLASTSGA = (-pars->Rx_tx_sizes.RxQueueSize); // when the buffer has been filled set the destination back to the start of it
    }
    else {                                                               // interrupt driven receiver
            #if defined KINETIS_KL && (UARTS_AVAILABLE > 1)
        if (Channel == 0) {
            uart_reg->UART_C5 &= ~(UART_C5_RDMAS);                       // disable tx DMA so that tx interrupt mode can be used
        }
        else {
            uart_reg->UART_MA1_C4 &= ~(UART_C4_RDMAS);                   // disable tx DMA so that tx interrupt mode can be used
        }
            #else
        uart_reg->UART_C5 &= ~(UART_C5_RDMAS);                           // disable rx DMA so that rx interrupt mode can be used
            #endif
    }
        #endif
    #endif
}
#endif

#if defined UART_BREAK_SUPPORT
extern void fnStartBreak(QUEUE_HANDLE channel)                           // {205}
{
    KINETIS_UART_CONTROL *uart_reg = (KINETIS_UART_CONTROL *)fnSelectChannel(channel);
    #if defined USER_CODE_START_BREAK
    USER_CODE_START_BREAK();
    #endif
    #if LPUARTS_AVAILABLE > 0
    if (uart_type[channel] == UART_TYPE_LPUART) {
        ((KINETIS_LPUART_CONTROL *)uart_reg)->LPUART_CTRL |= LPUART_CTRL_SBK; // start sending break characters
    }
    else {
        uart_reg->UART_C2 |= UART_C2_SBK;                                // start sending break characters
    }
    #else
    uart_reg->UART_C2 |= UART_C2_SBK;                                    // start sending break characters
    #endif
    #if defined _WINDOWS
    fnControlBreak(channel, 1);
    #endif
}

extern void fnStopBreak(QUEUE_HANDLE channel)                            // {205}
{
    KINETIS_UART_CONTROL *uart_reg = (KINETIS_UART_CONTROL *)fnSelectChannel(channel);
#if defined USER_CODE_START_BREAK
    USER_CODE_START_BREAK();
    #endif
    #if LPUARTS_AVAILABLE > 0
    if (uart_type[channel] == UART_TYPE_LPUART) {
        ((KINETIS_LPUART_CONTROL *)uart_reg)->LPUART_CTRL &= ~LPUART_CTRL_SBK; // stop sending break characters
    }
    else {
        uart_reg->UART_C2 &= ~UART_C2_SBK;                               // stop sending break characters
    }
    #else
    uart_reg->UART_C2 &= ~UART_C2_SBK;                                   // stop sending break characters
    #endif
    #if defined _WINDOWS
    fnControlBreak(channel, 0);
    #endif
}
#endif

#if defined _WINDOWS
static void _fnConfigSimSCI(QUEUE_HANDLE Channel, TTYTABLE *pars, unsigned short usDivider, unsigned char ucFraction, unsigned long ulBusClock, unsigned long ulSpecialClock)
{
    unsigned long ulBaudRate;
    #if defined KINETIS_KE
    ulBaudRate = (unsigned long)((float)ulBusClock/((float)usDivider)/16);// theoretical baud rate
    #elif defined KINETIS_KL || defined KINETIS_K80
        #if LPUARTS_AVAILABLE > 0
            #if UARTS_AVAILABLE > 0
    if (uart_type[Channel] == UART_TYPE_LPUART) {
        ulBaudRate = (unsigned long)((float)ulSpecialClock/((float)usDivider)/16);
    }
    else {
        #if defined KINETIS_KL43 || defined KINETIS_KL27
        ulBaudRate = (unsigned long)((float)ulBusClock/((float)usDivider + (((float)ucFraction)/32))/16);
        #else
        ulBaudRate = (unsigned long)((float)ulBusClock/((float)usDivider)/16);
        #endif
    }
            #else
    ulBaudRate = (unsigned long)((float)ulSpecialClock/((float)usDivider)/16);
            #endif
        #else
    if (Channel == 0) {
        ulBaudRate = (unsigned long)((float)ulSpecialClock/((float)usDivider)/16);
    }
    else {
        ulBaudRate = (unsigned long)((float)ulBusClock/((float)usDivider)/16);
    }
        #endif
    #else
        #if defined KINETIS_KV10
    if (Channel < 1)                                                     // UART 0 is clocked from the core clock
        #else
    if (Channel <= 1)                                                    // UARTs 0 and 1 are clocked from the core or a special clock
        #endif
    {
        ulBaudRate = (unsigned long)((float)ulSpecialClock/((float)usDivider + (((float)ucFraction)/32))/16);
    }
    else {                                                               // remaining UARTs are clocked from the bus clock
        ulBaudRate = (unsigned long)((float)ulBusClock/((float)usDivider + (((float)ucFraction)/32))/16);
    }
    #endif
    fnConfigSimSCI(Channel, ulBaudRate, pars);
}
#endif

// General UART/LPUART configuration
//
extern void fnConfigSCI(QUEUE_HANDLE Channel, TTYTABLE *pars)
{
    KINETIS_UART_CONTROL *uart_reg;
    unsigned short usDivider = 0;
    #if (!defined KINETIS_KL && !defined KINETIS_KE && !defined KINETIS_K80) || defined KINETIS_KL43 || defined KINETIS_KL27
    unsigned char ucFraction = 0;
    #endif
    #if NUMBER_EXTERNAL_SERIAL > 0
    if (Channel >= (NUMBER_SERIAL)) {
        fnConfigExtSCI((QUEUE_HANDLE)(Channel - NUMBER_SERIAL), pars);   // pass on to external UART driver
        #if defined SUPPORT_HW_FLOW
        if (pars->Config & RTS_CTS) {                                    // HW flow control defined so configure RTS/CTS pins
            fnControlLine(Channel, (CONFIG_RTS_PIN | CONFIG_CTS_PIN), 0);
        }
        #endif
        return;
    }
    #endif
    uart_reg = (KINETIS_UART_CONTROL *)fnSelectChannel(Channel);         // select the register set for use by this channel
    #if LPUARTS_AVAILABLE > 0                                            // if the device has low power UART
        #if defined LPUART_IRC48M                                        // use the IRC48M clock as UART clock
            #define SPECIAL_LPUART_CLOCK  (48000000)
        #elif defined LPUART_OSCERCLK                                    // clock the UART from the external clock
            #define SPECIAL_LPUART_CLOCK  (_EXTERNAL_CLOCK)
        #else
            #define SPECIAL_LPUART_CLOCK  (MCGIRCLK)
        #endif
        #if UARTS_AVAILABLE > 0                                          // if also UART
    if (uart_type[Channel] == UART_TYPE_LPUART) {
        #endif
        switch (Channel) {
        #if defined LPUARTS_PARALLEL
        case (UARTS_AVAILABLE):
        #else
        case (0):
        #endif
        #if defined KINETIS_KL
            POWER_UP(5, SIM_SCGC5_LPUART0);                              // power up LPUART 0
        #elif defined KINETIS_K80
            POWER_UP(2, SIM_SCGC2_LPUART0);                              // power up LPUART 0
        #else
            POWER_UP(6, SIM_SCGC6_LPUART0);                              // power up LPUART 0
        #endif
        #if defined LPUART_IRC48M                                        // use the IRC48M clock as UART clock
            #if defined KINETIS_WITH_MCG_LITE
            MCG_MC |= MCG_MC_HIRCEN;                                     // ensure that the IRC48M is operating, even when the processor is not in HIRC mode
            #endif
            #if defined KINETIS_K80
            SIM_SOPT2 = ((SIM_SOPT2 & ~(SIM_SOPT2_LPUARTSRC_MGC)) | (SIM_SOPT2_LPUARTSRC_SEL | SIM_SOPT2_PLLFLLSEL_IRC48M)); // {3} select the 48MHz IRC48MHz clock as source for the LPUART
            #else
            SIM_SOPT2 = ((SIM_SOPT2 & ~(SIM_SOPT2_UART0SRC_MCGIRCLK)) | (SIM_SOPT2_UART0SRC_IRC48M | SIM_SOPT2_PLLFLLSEL_IRC48M)); // {3} select the 48MHz IRC48MHz clock as source for the LPUART
            #endif
        #elif defined LPUART_OSCERCLK                                    // clock the UART from the external clock
            SIM_SOPT2 |= (SIM_SOPT2_UART0SRC_OSCERCLK);
        #else                                                            // clock the UART from MCGIRCLK (IRC8M/FCRDIV/LIRC_DIV2)
            #if !defined RUN_FROM_LIRC                                   // {206} if the processor is running from the the internal clock we don't change settings here
            MCG_C1 |= (MCG_C1_IRCLKEN | MCG_C1_IREFSTEN);                // ensure that the internal reference is enabled and runs in stop mode
                #if defined USE_FAST_INTERNAL_CLOCK                      // {201}
            MCG_SC = MCG_SC_FCRDIV_1;                                    // remove fast IRC divider
            MCG_C2 |= MCG_C2_IRCS;                                       // select fast internal reference clock (4MHz [8MHz for devices with MCG Lite]) as MCGIRCLK
                #else
            MCG_C2 &= ~MCG_C2_IRCS;                                      // select slow internal reference clock (32kHz [2MHz for devices with MCG Lite]) as MCGIRCLK
                #endif
            #endif
            SIM_SOPT2 |= (SIM_SOPT2_UART0SRC_MCGIRCLK);
        #endif
            break;
        #if LPUARTS_AVAILABLE > 1
            #if defined LPUARTS_PARALLEL
        case (UARTS_AVAILABLE + 1):
            #else
        case (1):
            #endif
            #if defined KINETIS_KL
            POWER_UP(5, SIM_SCGC5_LPUART1);                              // power up LPUART 1
            #else
            POWER_UP(2, SIM_SCGC2_LPUART1);                              // power up LPUART 1
            #endif
            #if defined LPUART_IRC48M                                    // use the IRC48M clock as UART clock
                #if defined KINETIS_WITH_MCG_LITE
            MCG_MC |= MCG_MC_HIRCEN;                                     // ensure that the IRC48M is operating, even when the processor is not in HIRC mode
                #endif
                #if defined KINETIS_K80
            SIM_SOPT2 = ((SIM_SOPT2 & ~(SIM_SOPT2_LPUARTSRC_MGC)) | (SIM_SOPT2_LPUARTSRC_SEL | SIM_SOPT2_PLLFLLSEL_IRC48M)); // {202} select the 48MHz IRC48MHz clock as source for the LPUART
                #else
            SIM_SOPT2 = ((SIM_SOPT2 & ~(SIM_SOPT2_UART1SRC_MCGIRCLK)) | (SIM_SOPT2_UART1SRC_IRC48M | SIM_SOPT2_PLLFLLSEL_IRC48M)); // {202} select the 48MHz IRC48MHz clock as source for the LPUART
                #endif
            #elif defined LPUART_OSCERCLK                                // clock the UART from the external clock
            SIM_SOPT2 |= (SIM_SOPT2_UART1SRC_OSCERCLK);
            #else                                                        // clock the UART from MCGIRCLK (IRC8M/FCRDIV/LIRC_DIV2)
            SIM_SOPT2 |= (SIM_SOPT2_UART1SRC_MCGIRCLK);
            #endif
            break;
        #endif
        #if LPUARTS_AVAILABLE > 2
            #if defined LPUARTS_PARALLEL
        case (UARTS_AVAILABLE + 2):
            #else
        case (2):
            #endif
            POWER_UP(2, SIM_SCGC2_LPUART2);                              // power up LPUART 2
            #if defined LPUART_IRC48M                                    // use the IRC48M clock as UART clock
                #if defined KINETIS_WITH_MCG_LITE
            MCG_MC |= MCG_MC_HIRCEN;                                     // ensure that the IRC48M is operating, even when the processor is not in HIRC mode
                #endif
                #if defined KINETIS_K80
            SIM_SOPT2 = ((SIM_SOPT2 & ~(SIM_SOPT2_LPUARTSRC_MGC)) | (SIM_SOPT2_LPUARTSRC_SEL | SIM_SOPT2_PLLFLLSEL_IRC48M)); // select the 48MHz IRC48MHz clock as source for the LPUART
                #else
            SIM_SOPT2 = ((SIM_SOPT2 & ~(SIM_SOPT2_UART1SRC_MCGIRCLK)) | (SIM_SOPT2_UART1SRC_IRC48M | SIM_SOPT2_PLLFLLSEL_IRC48M)); // select the 48MHz IRC48MHz clock as source for the LPUART
                #endif
            #elif defined LPUART_OSCERCLK                                // clock the UART from the external clock
            SIM_SOPT2 |= (SIM_SOPT2_UART1SRC_OSCERCLK);
            #else                                                        // clock the UART from MCGIRCLK (IRC8M/FCRDIV/LIRC_DIV2)
            SIM_SOPT2 |= (SIM_SOPT2_UART1SRC_MCGIRCLK);
            #endif
            break;
        #endif
        #if LPUARTS_AVAILABLE > 3
            #if defined LPUARTS_PARALLEL
        case (UARTS_AVAILABLE + 3):
            #else
        case (3):
            #endif
            POWER_UP(2, SIM_SCGC2_LPUART3);                              // power up LPUART 3
            #if defined LPUART_IRC48M                                    // use the IRC48M clock as UART clock
                #if defined KINETIS_WITH_MCG_LITE
            MCG_MC |= MCG_MC_HIRCEN;                                     // ensure that the IRC48M is operating, even when the processor is not in HIRC mode
                #endif
                #if defined KINETIS_K80
            SIM_SOPT2 = ((SIM_SOPT2 & ~(SIM_SOPT2_LPUARTSRC_MGC)) | (SIM_SOPT2_LPUARTSRC_SEL | SIM_SOPT2_PLLFLLSEL_IRC48M)); // select the 48MHz IRC48MHz clock as source for the LPUART
                #else
            SIM_SOPT2 = ((SIM_SOPT2 & ~(SIM_SOPT2_UART1SRC_MCGIRCLK)) | (SIM_SOPT2_UART1SRC_IRC48M | SIM_SOPT2_PLLFLLSEL_IRC48M)); // select the 48MHz IRC48MHz clock as source for the LPUART
                #endif
            #elif defined LPUART_OSCERCLK                                // clock the UART from the external clock
            SIM_SOPT2 |= (SIM_SOPT2_UART3SRC_OSCERCLK);
            #else                                                        // clock the UART from MCGIRCLK (IRC8M/FCRDIV/LIRC_DIV2)
            SIM_SOPT2 |= (SIM_SOPT2_UART3SRC_MCGIRCLK);
            #endif
            break;
        #endif
        #if LPUARTS_AVAILABLE > 4
            #if defined LPUARTS_PARALLEL
        case (UARTS_AVAILABLE + 4):
            #else
        case (4):
            #endif
            POWER_UP(2, SIM_SCGC2_LPUART4);                              // power up LPUART 4
            #if defined LPUART_IRC48M                                    // use the IRC48M clock as UART clock
                #if defined KINETIS_WITH_MCG_LITE
            MCG_MC |= MCG_MC_HIRCEN;                                     // ensure that the IRC48M is operating, even when the processor is not in HIRC mode
                #endif
                #if defined KINETIS_K80
            SIM_SOPT2 = ((SIM_SOPT2 & ~(SIM_SOPT2_LPUARTSRC_MGC)) | (SIM_SOPT2_LPUARTSRC_SEL | SIM_SOPT2_PLLFLLSEL_IRC48M)); // select the 48MHz IRC48MHz clock as source for the LPUART
                #else
            SIM_SOPT2 = ((SIM_SOPT2 & ~(SIM_SOPT2_UART1SRC_MCGIRCLK)) | (SIM_SOPT2_UART1SRC_IRC48M | SIM_SOPT2_PLLFLLSEL_IRC48M)); // select the 48MHz IRC48MHz clock as source for the LPUART
                #endif
            #elif defined LPUART_OSCERCLK                                // clock the UART from the external clock
            SIM_SOPT2 |= (SIM_SOPT2_UART4SRC_OSCERCLK);
            #else                                                        // clock the UART from MCGIRCLK (IRC8M/FCRDIV/LIRC_DIV2)
            SIM_SOPT2 |= (SIM_SOPT2_UART4SRC_MCGIRCLK);
            #endif
            break;
        #endif
        default:
            _EXCEPTION("Trying to configure non-existent LPUART channel!");
            return;
        }
        #if UARTS_AVAILABLE > 0 
    }
        #else
            #define SPECIAL_UART_CLOCK SPECIAL_LPUART_CLOCK
        #endif
        #if UARTS_AVAILABLE > 0
    else {
        #endif
    #endif
    #if UARTS_AVAILABLE > 0
        switch (Channel) {
        #if UARTS_AVAILABLE > 0 && (LPUARTS_AVAILABLE < 1 || defined LPUARTS_PARALLEL)
        case 0:
            POWER_UP(4, SIM_SCGC4_UART0);                                // power up UART 0
            #if defined KINETIS_KL
                #if defined UART0_ClOCKED_FROM_MCGIRCLK                  // clocked from internal 4MHz RC clock
            SIM_SOPT2 |= (SIM_SOPT2_UART0SRC_MCGIRCLK);                  // enable UART0 clock source from MCGIRCLK
                #elif defined UART0_ClOCKED_FROM_OSCERCLK
            SIM_SOPT2 |= (SIM_SOPT2_UART0SRC_OSCERCLK);                  // enable UART0 clock source from OSCERCLK
                #else
                    #if !defined MCG_WITHOUT_PLL && !defined UART0_ClOCKED_FROM_MCGFFLCLK
            SIM_SOPT2 |= (SIM_SOPT2_PLLFLLSEL | SIM_SOPT2_UART0SRC_MCG); // enable UART0 clock source from MCGPLLCLK/2
                    #else
            SIM_SOPT2 |= (SIM_SOPT2_UART0SRC_MCG);                       // enable UART0 clock source from MCGFFLCLK
                    #endif
                #endif
            #endif
            break;
        #endif
        #if UARTS_AVAILABLE > 1 && (LPUARTS_AVAILABLE < 2 || defined LPUARTS_PARALLEL)
        case 1:
            POWER_UP(4, SIM_SCGC4_UART1);                                // power up UART 1
            break;
        #endif
        #if (UARTS_AVAILABLE > 2 && (LPUARTS_AVAILABLE < 3 || defined LPUARTS_PARALLEL)) || (UARTS_AVAILABLE == 1 && LPUARTS_AVAILABLE == 2)
        case 2:
            POWER_UP(4, SIM_SCGC4_UART2);                                // power up UART 2
            break;
        #endif
        #if UARTS_AVAILABLE > 3
        case 3:
            POWER_UP(4, SIM_SCGC4_UART3);                                // power up UART 3
            break;
        #endif
        #if UARTS_AVAILABLE > 4
        case 4:
            POWER_UP(1, SIM_SCGC1_UART4);                                // power up UART 4
            break;
        #endif
        #if UARTS_AVAILABLE > 5
        case 5:
            POWER_UP(1, SIM_SCGC1_UART5);                                // power up UART 5
            break;
        #endif
        default:
            _EXCEPTION("Trying to configure non-existent UART channel!");
            return;
        }
        #if LPUARTS_AVAILABLE > 0
    }
        #endif
    #endif

    #if defined SUPPORT_HW_FLOW
    if (pars->Config & RTS_CTS) {                                        // HW flow control defined so configure RTS/CTS pins
        fnControlLine(Channel, (CONFIG_RTS_PIN | CONFIG_CTS_PIN), 0);
    }
    #endif

    #if (((SYSTEM_CLOCK != BUS_CLOCK) || defined KINETIS_KL || (LPUARTS_AVAILABLE > 0 && UARTS_AVAILABLE > 0))) && !defined KINETIS_KE
        #if defined KINETIS_KL || defined KINETIS_K80
            #if LPUARTS_AVAILABLE == 0
                #if defined UART0_ClOCKED_FROM_MCGIRCLK                  // clocked from internal 4MHz RC clock
                    #define SPECIAL_UART_CLOCK    (MCGIRCLK)
                #elif defined UART0_ClOCKED_FROM_OSCERCLK
                    #define SPECIAL_UART_CLOCK    (OSCERCLK)
                #else
                    #if !defined MCG_WITHOUT_PLL && !defined UART0_ClOCKED_FROM_MCGFFLCLK
                        #define SPECIAL_UART_CLOCK    (MCGPLLCLK/2)
                    #else
                        #define SPECIAL_UART_CLOCK    (MCGFLLCLK)
                    #endif
                #endif
    if (Channel == 0)                                                    // UART 0 is clocked from a selectable source
            #else
                #define SPECIAL_UART_CLOCK  SPECIAL_LPUART_CLOCK
    if (Channel < LPUARTS_AVAILABLE)                                     // LPUART is clocked from a selectable source
            #endif
        #else
            #if UARTS_AVAILABLE == 0
                #define SPECIAL_UART_CLOCK    (SPECIAL_LPUART_CLOCK)
            #else
                #define SPECIAL_UART_CLOCK    (SYSTEM_CLOCK)
            #endif
            #if defined KINETIS_KV10
    if (Channel < 1)                                                     // UART 0 is clocked from the core/system clock and the others from the bus clock
            #else
    if (Channel <= 1)                                                    // UARTs 0 and 1 are clocked from the core/system clock and the others from the bus clock
            #endif
        #endif
    {
        switch (pars->ucSpeed) {           
        case SERIAL_BAUD_300:
        #if defined KINETIS_KL || defined KINETIS_K80
            usDivider = (((SPECIAL_UART_CLOCK/8/300) + 1)/2);            // {201} set 300
        #else
            ucFraction = (unsigned char)((float)((((float)SPECIAL_UART_CLOCK/(float)16/(float)300) - (int)(SPECIAL_UART_CLOCK/16/300)) * 32)); // calculate fraction
            usDivider = (SPECIAL_UART_CLOCK/16/300);                     // set 300
        #endif
            break;
        case SERIAL_BAUD_600:
        #if defined KINETIS_KL || defined KINETIS_K80
            usDivider = (((SPECIAL_UART_CLOCK/8/600) + 1)/2);            // {201} set 600
        #else
            ucFraction = (unsigned char)((float)((((float)SPECIAL_UART_CLOCK/(float)16/(float)600) - (int)(SPECIAL_UART_CLOCK/16/600)) * 32)); // calculate fraction
            usDivider = (SPECIAL_UART_CLOCK/16/600);                     // set 600
        #endif
            break;
        case SERIAL_BAUD_1200:
        #if defined KINETIS_KL || defined KINETIS_K80
            usDivider = (((SPECIAL_UART_CLOCK/8/1200) + 1)/2);           // {201} set 1200
        #else
            ucFraction = (unsigned char)((float)((((float)SPECIAL_UART_CLOCK/(float)16/(float)1200) - (int)(SPECIAL_UART_CLOCK/16/1200)) * 32)); // calculate fraction
            usDivider = (SPECIAL_UART_CLOCK/16/1200);                    // set 1200
        #endif
            break;
        case SERIAL_BAUD_2400:
        #if defined KINETIS_KL || defined KINETIS_K80
            usDivider = (((SPECIAL_UART_CLOCK/8/2400) + 1)/2);           // {201} set 2400
        #else
            ucFraction = (unsigned char)((float)((((float)SPECIAL_UART_CLOCK/(float)16/(float)2400) - (int)(SPECIAL_UART_CLOCK/16/2400)) * 32)); // calculate fraction
            usDivider = (SPECIAL_UART_CLOCK/16/2400);                    // set 2400
        #endif
            break;
        case SERIAL_BAUD_4800:
        #if defined KINETIS_KL || defined KINETIS_K80
            usDivider = (((SPECIAL_UART_CLOCK/8/4800) + 1)/2);           // {201} set 4800
        #else
            ucFraction = (unsigned char)((float)((((float)SPECIAL_UART_CLOCK/(float)16/(float)4800) - (int)(SPECIAL_UART_CLOCK/16/4800)) * 32)); // calculate fraction
            usDivider = (SPECIAL_UART_CLOCK/16/4800);                    // set 4800
        #endif
            break;
        case SERIAL_BAUD_9600:
        #if defined KINETIS_KL || defined KINETIS_K80
            usDivider = (((SPECIAL_UART_CLOCK/8/19600) + 1)/2);          // {201} set 9600
        #else
            ucFraction = (unsigned char)((float)((((float)SPECIAL_UART_CLOCK/(float)16/(float)9600) - (int)(SPECIAL_UART_CLOCK/16/9600)) * 32)); // calculate fraction
            usDivider = (SPECIAL_UART_CLOCK/16/9600);                   // set 9600
        #endif
            break;
        case SERIAL_BAUD_14400:
        #if defined KINETIS_KL || defined KINETIS_K80
            usDivider = (((SPECIAL_UART_CLOCK/8/14400) + 1)/2);          // {201} set 14400
        #else
            ucFraction = (unsigned char)((float)((((float)SPECIAL_UART_CLOCK/(float)16/(float)14400) - (int)(SPECIAL_UART_CLOCK/16/14400)) * 32)); // calculate fraction
            usDivider = (SPECIAL_UART_CLOCK/16/14400);                   // set 14400
        #endif
            break;
        default:                                                         // if not valid value set this speed
        case SERIAL_BAUD_19200:
        #if defined KINETIS_KL || defined KINETIS_K80
            usDivider = (((SPECIAL_UART_CLOCK/8/19200) + 1)/2);          // {201} set 19200
        #else
            ucFraction = (unsigned char)((float)((((float)SPECIAL_UART_CLOCK/(float)16/(float)19200) - (int)(SPECIAL_UART_CLOCK/16/19200)) * 32)); // calculate fraction
            usDivider = (SPECIAL_UART_CLOCK/16/19200);                   // set 19200
        #endif
            break;
        #if defined SUPPORT_MIDI_BAUD_RATE
        case SERIAL_BAUD_31250:                                          // {207} set 31250
            #if defined KINETIS_KL || defined KINETIS_K80
            usDivider = (((SPECIAL_UART_CLOCK/8/31250) + 1)/2);
            #else
            ucFraction = (unsigned char)((float)((((float)SPECIAL_UART_CLOCK/(float)16/(float)31250) - (int)(SPECIAL_UART_CLOCK/16/31250)) * 32)); // calculate fraction
            usDivider = (SPECIAL_UART_CLOCK/16/31250);                   // set 31250
            #endif
            break;
        #endif
        case SERIAL_BAUD_38400:
        #if defined KINETIS_KL || defined KINETIS_K80
            usDivider = (((SPECIAL_UART_CLOCK/8/38400) + 1)/2);          // {201} set 38400
        #else
            ucFraction = (unsigned char)((float)((((float)SPECIAL_UART_CLOCK/(float)16/(float)38400) - (int)(SPECIAL_UART_CLOCK/16/38400)) * 32)); // calculate fraction
            usDivider = (SPECIAL_UART_CLOCK/16/38400);                   // set 38400
        #endif
            break;
        case SERIAL_BAUD_57600:
        #if defined KINETIS_KL || defined KINETIS_K80
            usDivider = (((SPECIAL_UART_CLOCK/8/57600) + 1)/2);          // {201} set 57600
        #else
            ucFraction = (unsigned char)((float)((((float)SPECIAL_UART_CLOCK/(float)16/(float)57600) - (int)(SPECIAL_UART_CLOCK/16/57600)) * 32)); // calculate fraction
            usDivider = (SPECIAL_UART_CLOCK/16/57600);                   // set 57600
        #endif
            break;
        case SERIAL_BAUD_115200:
        #if defined KINETIS_KL || defined KINETIS_K80
            usDivider = (((SPECIAL_UART_CLOCK/8/115200) + 1)/2);         // {201} set 115200
        #else
            ucFraction = (unsigned char)((float)((((float)SPECIAL_UART_CLOCK/(float)16/(float)115200) - (int)(SPECIAL_UART_CLOCK/16/115200)) * 32)); // calculate fraction
            usDivider = (SPECIAL_UART_CLOCK/16/115200);                  // set 115200
        #endif
            break;
        case SERIAL_BAUD_230400:
        #if defined KINETIS_KL || defined KINETIS_K80
            usDivider = (((SPECIAL_UART_CLOCK/8/230400) + 1)/2);         // {201} set 230400
        #else
            ucFraction = (unsigned char)((float)((((float)SPECIAL_UART_CLOCK/(float)16/(float)230400) - (int)(SPECIAL_UART_CLOCK/16/230400)) * 32)); // calculate fraction
            usDivider = (SPECIAL_UART_CLOCK/16/230400);                  // set 230400
        #endif
            break;
        case SERIAL_BAUD_250K:
        #if defined KINETIS_KL || defined KINETIS_K80
            usDivider = (((SPECIAL_UART_CLOCK/8/250000) + 1)/2);         // set 250000
        #else
            ucFraction = (unsigned char)((float)((((float)SPECIAL_UART_CLOCK/(float)16/(float)250000) - (int)(SPECIAL_UART_CLOCK/16/250000)) * 32)); // calculate fraction
            usDivider = (SPECIAL_UART_CLOCK/16/250000);                  // set 250000
        #endif
            break;
        }
    }
    else {
    #else
        #define SPECIAL_UART_CLOCK    (SYSTEM_CLOCK)
    #endif
    #if (UARTS_AVAILABLE > 2 && LPUARTS_AVAILABLE > 0 && defined LPUARTS_PARALLEL) // UARTs above channel 1 use bus clock but LPUARTs use their own special clock
        if (Channel >= UARTS_AVAILABLE) {                                // LPUART using its own special clock
            switch (pars->ucSpeed) {              
            case SERIAL_BAUD_300:
        #if (defined KINETIS_KL || defined KINETIS_KE) && !defined KINETIS_KL43 && !defined KINETIS_KL27
                usDivider = (unsigned short)((((float)((float)SPECIAL_LPUART_CLOCK/(float)(16 * 300)) + (float)0.5) * (float)2)/2); // best divider for 300
        #else
                ucFraction = (unsigned char)((float)((((float)SPECIAL_LPUART_CLOCK/(float)16/(float)300) - (int)(BUS_CLOCK/16/300)) * 32)); // calculate fraction
                usDivider = (SPECIAL_LPUART_CLOCK/16/300);               // set 300
        #endif
                break;
            case SERIAL_BAUD_600:
        #if (defined KINETIS_KL || defined KINETIS_KE) && !defined KINETIS_KL43 && !defined KINETIS_KL27
                usDivider = (unsigned short)((((float)((float)SPECIAL_LPUART_CLOCK/(float)(16 * 600)) + (float)0.5) * (float)2)/2); // best divider for 600
        #else
                ucFraction = (unsigned char)((float)((((float)SPECIAL_LPUART_CLOCK/(float)16/(float)600) - (int)(SPECIAL_LPUART_CLOCK/16/600)) * 32)); // calculate fraction
                usDivider = (SPECIAL_LPUART_CLOCK/16/600);               // set 600
        #endif
                break;
            case SERIAL_BAUD_1200:
        #if (defined KINETIS_KL || defined KINETIS_KE) && !defined KINETIS_KL43 && !defined KINETIS_KL27
                usDivider = (unsigned short)((((float)((float)SPECIAL_LPUART_CLOCK/(float)(16 * 1200)) + (float)0.5) * (float)2)/2); // best divider for 1200
        #else
                ucFraction = (unsigned char)((float)((((float)SPECIAL_LPUART_CLOCK/(float)16/(float)1200) - (int)(SPECIAL_LPUART_CLOCK/16/1200)) * 32)); // calculate fraction
                usDivider = (SPECIAL_LPUART_CLOCK/16/1200);              // set 1200
        #endif
                break;
            case SERIAL_BAUD_2400:
        #if (defined KINETIS_KL || defined KINETIS_KE) && !defined KINETIS_KL43 && !defined KINETIS_KL27
                usDivider = (unsigned short)((((float)((float)SPECIAL_LPUART_CLOCK/(float)(16 * 2400)) + (float)0.5) * (float)2)/2); // best divider for 2400
        #else
                ucFraction = (unsigned char)((float)((((float)SPECIAL_LPUART_CLOCK/(float)16/(float)2400) - (int)(SPECIAL_LPUART_CLOCK/16/2400)) * 32)); // calculate fractio
                usDivider = (SPECIAL_LPUART_CLOCK/16/2400);              // set 2400
        #endif
                break;
            case SERIAL_BAUD_4800:
        #if (defined KINETIS_KL || defined KINETIS_KE) && !defined KINETIS_KL43 && !defined KINETIS_KL27
                usDivider = (unsigned short)((((float)((float)SPECIAL_LPUART_CLOCK/(float)(16 * 4800)) + (float)0.5) * (float)2)/2); // best divider for 4800
        #else
                ucFraction = (unsigned char)((float)((((float)SPECIAL_LPUART_CLOCK/(float)16/(float)4800) - (int)(SPECIAL_LPUART_CLOCK/16/4800)) * 32)); // calculate fraction
                usDivider = (SPECIAL_LPUART_CLOCK/16/4800);              // set 4800
        #endif
                break;
            case SERIAL_BAUD_9600:
        #if (defined KINETIS_KL || defined KINETIS_KE) && !defined KINETIS_KL43 && !defined KINETIS_KL27
                usDivider = (unsigned short)((((float)((float)SPECIAL_LPUART_CLOCK/(float)(16 * 9600)) + (float)0.5) * (float)2)/2); // best divider for 9600
        #else
                ucFraction = (unsigned char)((float)((((float)SPECIAL_LPUART_CLOCK/(float)16/(float)9600) - (int)(SPECIAL_LPUART_CLOCK/16/9600)) * 32)); // calculate fraction
                usDivider = (SPECIAL_LPUART_CLOCK/16/9600);              // set 9600
        #endif
                break;
            case SERIAL_BAUD_14400:
        #if (defined KINETIS_KL || defined KINETIS_KE) && !defined KINETIS_KL43 && !defined KINETIS_KL27
                usDivider = (unsigned short)((((float)((float)SPECIAL_LPUART_CLOCK/(float)(16 * 14400)) + (float)0.5) * (float)2)/2); // best divider for 14400
        #else
                ucFraction = (unsigned char)((float)((((float)SPECIAL_LPUART_CLOCK/(float)16/(float)14400) - (int)(SPECIAL_LPUART_CLOCK/16/14400)) * 32)); // calculate fraction
                usDivider = (SPECIAL_LPUART_CLOCK/16/14400);             // set 14400
        #endif
                break;
            default:                                                     // if not valid value set this speed
            case SERIAL_BAUD_19200:
        #if (defined KINETIS_KL || defined KINETIS_KE) && !defined KINETIS_KL43 && !defined KINETIS_KL27
                usDivider = (unsigned short)((((float)((float)SPECIAL_LPUART_CLOCK/(float)(16 * 19200)) + (float)0.5) * (float)2)/2); // best divider for 19200
        #else
                ucFraction = (unsigned char)((float)((((float)SPECIAL_LPUART_CLOCK/(float)16/(float)19200) - (int)(SPECIAL_LPUART_CLOCK/16/19200)) * 32)); // calculate fraction
                usDivider = (SPECIAL_LPUART_CLOCK/16/19200);             // set 19200
        #endif
                break;
        #if defined SUPPORT_MIDI_BAUD_RATE
            case SERIAL_BAUD_31250:                                      // {207} set 31250
            #if (defined KINETIS_KL || defined KINETIS_KE) && !defined KINETIS_KL43 && !defined KINETIS_KL27
                usDivider = (unsigned short)((((float)((float)SPECIAL_LPUART_CLOCK/(float)(16 * 31250)) + (float)0.5) * (float)2)/2); // best divider for 31250
            #else
                ucFraction = (unsigned char)((float)((((float)SPECIAL_LPUART_CLOCK/(float)16/(float)31250) - (int)(SPECIAL_LPUART_CLOCK/16/ 31250)) * 32)); // calculate fraction
                usDivider = (SPECIAL_LPUART_CLOCK/16/ 31250);            // set 31250
            #endif
                break;
        #endif
            case SERIAL_BAUD_38400:
        #if (defined KINETIS_KL || defined KINETIS_KE) && !defined KINETIS_KL43 && !defined KINETIS_KL27
                usDivider = (unsigned short)((((float)((float)SPECIAL_LPUART_CLOCK/(float)(16 * 38400)) + (float)0.5) * (float)2)/2); // best divider for 38400
        #else
                ucFraction = (unsigned char)((float)((((float)SPECIAL_LPUART_CLOCK/(float)16/(float)38400) - (int)(SPECIAL_LPUART_CLOCK/16/38400)) * 32)); // calculate fraction
                usDivider = (SPECIAL_LPUART_CLOCK/16/38400);             // set 38400
        #endif
                break;
            case SERIAL_BAUD_57600:
        #if (defined KINETIS_KL || defined KINETIS_KE) && !defined KINETIS_KL43 && !defined KINETIS_KL27
                usDivider = (unsigned short)((((float)((float)SPECIAL_LPUART_CLOCK/(float)(16 * 57600)) + (float)0.5) * (float)2)/2); // best divider for 57600
        #else
                ucFraction = (unsigned char)((float)((((float)SPECIAL_LPUART_CLOCK/(float)16/(float)57600) - (int)(SPECIAL_LPUART_CLOCK/16/57600)) * 32)); // calculate fraction
                usDivider = (SPECIAL_LPUART_CLOCK/16/57600);             // set 57600
        #endif
                break;
            case SERIAL_BAUD_115200:
        #if (defined KINETIS_KL || defined KINETIS_KE) && !defined KINETIS_KL43 && !defined KINETIS_KL27
                usDivider = (unsigned short)((((float)((float)SPECIAL_LPUART_CLOCK/(float)(16 * 115200)) + (float)0.5) * (float)2)/2); // best divider for 115200
        #else
                ucFraction = (unsigned char)((float)((((float)SPECIAL_LPUART_CLOCK/(float)16/(float)115200) - (int)(SPECIAL_LPUART_CLOCK/16/115200)) * 32)); // calculate fraction
                usDivider = (SPECIAL_LPUART_CLOCK/16/115200);            // set 115200
        #endif
                break;
            case SERIAL_BAUD_230400:
        #if (defined KINETIS_KL || defined KINETIS_KE) && !defined KINETIS_KL43 && !defined KINETIS_KL27
                usDivider = (unsigned short)((((float)((float)SPECIAL_LPUART_CLOCK/(float)(16 * 230400)) + (float)0.5) * (float)2)/2); // best divider for 230400
        #else
                ucFraction = (unsigned char)((float)((((float)SPECIAL_LPUART_CLOCK/(float)16/(float)230400) - (int)(SPECIAL_LPUART_CLOCK/16/230400)) * 32)); // calculate fraction
                usDivider = (SPECIAL_LPUART_CLOCK/16/230400);            // set 230400
        #endif
                break;
            case SERIAL_BAUD_250K:
        #if (defined KINETIS_KL || defined KINETIS_KE) && !defined KINETIS_KL43 && !defined KINETIS_KL27
                usDivider = (unsigned short)((((float)((float)SPECIAL_LPUART_CLOCK/(float)(16 * 250000)) + (float)0.5) * (float)2)/2); // best divider for 250000
        #else
                ucFraction = (unsigned char)((float)((((float)SPECIAL_LPUART_CLOCK/(float)16/(float)250000) - (int)(SPECIAL_LPUART_CLOCK/16/ 250000)) * 32)); // calculate fraction
                usDivider = (SPECIAL_LPUART_CLOCK/16/ 250000);           // set 250000
        #endif
                break;
            }
        }
        else {
    #endif
    #if (UARTS_AVAILABLE > 2) || (defined KINETIS_K02 || defined KINETIS_KL43 || defined KINETIS_KL27) || (defined KINETIS_KV && UARTS_AVAILABLE > 1)
            switch (pars->ucSpeed) {
            case SERIAL_BAUD_300:
        #if (defined KINETIS_KL || defined KINETIS_KE) && !defined KINETIS_KL43 && !defined KINETIS_KL27
                usDivider = (unsigned short)((((float)((float)BUS_CLOCK/(float)(16 * 300)) + (float)0.5) * (float)2)/2); // best divider for 300
        #else
                ucFraction = (unsigned char)((float)((((float)BUS_CLOCK/(float)16/(float)300) - (int)(BUS_CLOCK/16/300)) * 32)); // calculate fraction
                usDivider = (BUS_CLOCK/16/300);                          // set 300
        #endif
                break;
            case SERIAL_BAUD_600:
        #if (defined KINETIS_KL || defined KINETIS_KE) && !defined KINETIS_KL43 && !defined KINETIS_KL27
                usDivider = (unsigned short)((((float)((float)BUS_CLOCK/(float)(16 * 600)) + (float)0.5) * (float)2)/2); // best divider for 600
        #else
                ucFraction = (unsigned char)((float)((((float)BUS_CLOCK/(float)16/(float)600) - (int)(BUS_CLOCK/16/600)) * 32)); // calculate fraction
                usDivider = (BUS_CLOCK/16/600);                          // set 600
        #endif
                break;
            case SERIAL_BAUD_1200:
        #if (defined KINETIS_KL || defined KINETIS_KE) && !defined KINETIS_KL43 && !defined KINETIS_KL27
                usDivider = (unsigned short)((((float)((float)BUS_CLOCK/(float)(16 * 1200)) + (float)0.5) * (float)2)/2); // best divider for 1200
        #else
                ucFraction = (unsigned char)((float)((((float)BUS_CLOCK/(float)16/(float)1200) - (int)(BUS_CLOCK/16/1200)) * 32)); // calculate fraction
                usDivider = (BUS_CLOCK/16/1200);                         // set 1200
        #endif
                break;
            case SERIAL_BAUD_2400:
        #if (defined KINETIS_KL || defined KINETIS_KE) && !defined KINETIS_KL43 && !defined KINETIS_KL27
                usDivider = (unsigned short)((((float)((float)BUS_CLOCK/(float)(16 * 2400)) + (float)0.5) * (float)2)/2); // best divider for 2400
        #else
                ucFraction = (unsigned char)((float)((((float)BUS_CLOCK/(float)16/(float)2400) - (int)(BUS_CLOCK/16/2400)) * 32)); // calculate fractio
                usDivider = (BUS_CLOCK/16/2400);                         // set 2400
        #endif
                break;
            case SERIAL_BAUD_4800:
        #if (defined KINETIS_KL || defined KINETIS_KE) && !defined KINETIS_KL43 && !defined KINETIS_KL27
                usDivider = (unsigned short)((((float)((float)BUS_CLOCK/(float)(16 * 4800)) + (float)0.5) * (float)2)/2); // best divider for 4800
        #else
                ucFraction = (unsigned char)((float)((((float)BUS_CLOCK/(float)16/(float)4800) - (int)(BUS_CLOCK/16/4800)) * 32)); // calculate fraction
                usDivider = (BUS_CLOCK/16/4800);                         // set 4800
        #endif
                break;
            case SERIAL_BAUD_9600:
        #if (defined KINETIS_KL || defined KINETIS_KE) && !defined KINETIS_KL43 && !defined KINETIS_KL27
                usDivider = (unsigned short)((((float)((float)BUS_CLOCK/(float)(16 * 9600)) + (float)0.5) * (float)2)/2); // best divider for 9600
        #else
                ucFraction = (unsigned char)((float)((((float)BUS_CLOCK/(float)16/(float)9600) - (int)(BUS_CLOCK/16/9600)) * 32)); // calculate fraction
                usDivider = (BUS_CLOCK/16/9600);                         // set 9600
        #endif
                break;
            case SERIAL_BAUD_14400:
        #if (defined KINETIS_KL || defined KINETIS_KE) && !defined KINETIS_KL43 && !defined KINETIS_KL27
                usDivider = (unsigned short)((((float)((float)BUS_CLOCK/(float)(16 * 14400)) + (float)0.5) * (float)2)/2); // best divider for 14400
        #else
                ucFraction = (unsigned char)((float)((((float)BUS_CLOCK/(float)16/(float)14400) - (int)(BUS_CLOCK/16/14400)) * 32)); // calculate fraction
                usDivider = (BUS_CLOCK/16/14400);                        // set 14400
        #endif
                break;
            default:                                                     // if not valid value set this speed
            case SERIAL_BAUD_19200:
        #if (defined KINETIS_KL || defined KINETIS_KE) && !defined KINETIS_KL43 && !defined KINETIS_KL27
                usDivider = (unsigned short)((((float)((float)BUS_CLOCK/(float)(16 * 19200)) + (float)0.5) * (float)2)/2); // best divider for 19200
        #else
                ucFraction = (unsigned char)((float)((((float)BUS_CLOCK/(float)16/(float)19200) - (int)(BUS_CLOCK/16/19200)) * 32)); // calculate fraction
                usDivider = (BUS_CLOCK/16/19200);                        // set 19200
        #endif
                break;
        #if defined SUPPORT_MIDI_BAUD_RATE
            case SERIAL_BAUD_31250:                                      // {207} set 31250
            #if (defined KINETIS_KL || defined KINETIS_KE) && !defined KINETIS_KL43 && !defined KINETIS_KL27
                usDivider = (unsigned short)((((float)((float)BUS_CLOCK/(float)(16 * 31250)) + (float)0.5) * (float)2)/2); // best divider for 31250
            #else
                ucFraction = (unsigned char)((float)((((float)BUS_CLOCK/(float)16/(float)31250) - (int)(BUS_CLOCK/16/ 31250)) * 32)); // calculate fraction
                usDivider = (BUS_CLOCK/16/ 31250);                       // set 31250
            #endif
                break;
        #endif
            case SERIAL_BAUD_38400:
        #if (defined KINETIS_KL || defined KINETIS_KE) && !defined KINETIS_KL43 && !defined KINETIS_KL27
                usDivider = (unsigned short)((((float)((float)BUS_CLOCK/(float)(16 * 38400)) + (float)0.5) * (float)2)/2); // best divider for 38400
        #else
                ucFraction = (unsigned char)((float)((((float)BUS_CLOCK/(float)16/(float)38400) - (int)(BUS_CLOCK/16/38400)) * 32)); // calculate fraction
                usDivider = (BUS_CLOCK/16/38400);                        // set 38400
        #endif
                break;
            case SERIAL_BAUD_57600:
        #if (defined KINETIS_KL || defined KINETIS_KE) && !defined KINETIS_KL43 && !defined KINETIS_KL27
                usDivider = (unsigned short)((((float)((float)BUS_CLOCK/(float)(16 * 57600)) + (float)0.5) * (float)2)/2); // best divider for 57600
        #else
                ucFraction = (unsigned char)((float)((((float)BUS_CLOCK/(float)16/(float)57600) - (int)(BUS_CLOCK/16/57600)) * 32)); // calculate fraction
                usDivider = (BUS_CLOCK/16/57600);                        // set 57600
        #endif
                break;
            case SERIAL_BAUD_115200:
        #if (defined KINETIS_KL || defined KINETIS_KE) && !defined KINETIS_KL43 && !defined KINETIS_KL27
                usDivider = (unsigned short)((((float)((float)BUS_CLOCK/(float)(16 * 115200)) + (float)0.5) * (float)2)/2); // best divider for 115200
        #else
                ucFraction = (unsigned char)((float)((((float)BUS_CLOCK/(float)16/(float)115200) - (int)(BUS_CLOCK/16/115200)) * 32)); // calculate fraction
                usDivider = (BUS_CLOCK/16/115200);                       // set 115200
        #endif
                break;
            case SERIAL_BAUD_230400:
        #if (defined KINETIS_KL || defined KINETIS_KE) && !defined KINETIS_KL43 && !defined KINETIS_KL27
                usDivider = (unsigned short)((((float)((float)BUS_CLOCK/(float)(16 * 230400)) + (float)0.5) * (float)2)/2); // best divider for 230400
        #else
                ucFraction = (unsigned char)((float)((((float)BUS_CLOCK/(float)16/(float)230400) - (int)(BUS_CLOCK/16/230400)) * 32)); // calculate fraction
                usDivider = (BUS_CLOCK/16/230400);                       // set 230400
        #endif
                break;
            case SERIAL_BAUD_250K:
        #if (defined KINETIS_KL || defined KINETIS_KE) && !defined KINETIS_KL43 && !defined KINETIS_KL27
                usDivider = (unsigned short)((((float)((float)BUS_CLOCK/(float)(16 * 250000)) + (float)0.5) * (float)2)/2); // best divider for 250000
        #else
                ucFraction = (unsigned char)((float)((((float)BUS_CLOCK/(float)16/(float)250000) - (int)(BUS_CLOCK/16/250000)) * 32)); // calculate fraction
                usDivider = (BUS_CLOCK/16/250000);                       // set 250000
        #endif
                break;
            }
    #endif
    #if UARTS_AVAILABLE > 2 && LPUARTS_AVAILABLE > 0 && defined LPUARTS_PARALLEL
        }
    #endif
    #if (((SYSTEM_CLOCK != BUS_CLOCK) || defined KINETIS_KL || (LPUARTS_AVAILABLE > 0 && UARTS_AVAILABLE > 0))) && !defined KINETIS_KE
    }
    #endif

    #if defined UART_TIMED_TRANSMISSION && defined SERIAL_SUPPORT_DMA    // {208}
    if ((pars->Config & UART_TIMED_TRANSMISSION_MODE) != 0) {
        ulInterCharTxDelay[Channel] = PIT_US_DELAY(pars->usMicroDelay);
    }
    else {
        ulInterCharTxDelay[Channel] = 0;
    }
    #endif
    
    #if LPUARTS_AVAILABLE > 0                                            // {106}
        #if UARTS_AVAILABLE > 0                                          // device contains both UART and LPUART
    if (uart_type[Channel] == UART_TYPE_LPUART) {                        // configure the low power UART
        #endif
        fnConfigLPUART(Channel, pars, (KINETIS_LPUART_CONTROL *)uart_reg, usDivider);
        #if UARTS_AVAILABLE > 0
    }
    else {
            #if (!defined KINETIS_KL && !defined KINETIS_KE) || defined KINETIS_KL43 || defined KINETIS_KL27
        uart_reg->UART_C4  = ucFraction;
            #endif
        fnConfigUART(Channel, pars, uart_reg, usDivider);                // configure the UART
    }
        #endif
    #else
        #if !defined KINETIS_KL && !defined KINETIS_KE
    uart_reg->UART_C4 = ucFraction;
        #endif
    fnConfigUART(Channel, pars, uart_reg, usDivider);                    // configure the UART
    #endif

    #if defined _WINDOWS
        #if (!defined KINETIS_KL && !defined KINETIS_KE && !defined KINETIS_K80) || defined KINETIS_KL43 || defined KINETIS_KL27
    _fnConfigSimSCI(Channel, pars, usDivider, ucFraction, BUS_CLOCK, SPECIAL_UART_CLOCK); // open a serial port on PC if desired
        #else
    _fnConfigSimSCI(Channel, pars, usDivider, 0, BUS_CLOCK, SPECIAL_UART_CLOCK); // open a serial port on PC if desired
        #endif
    #endif
}
