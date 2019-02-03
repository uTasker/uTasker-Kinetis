/**********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher

    ---------------------------------------------------------------------
    File:      stm32_I2C.h
    Project:   Single Chip Embedded Internet
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2018
    *********************************************************************
    20.11.2018 Reworked to avoid polling during start condition wait and with option [NO_REPEATED_START_ON_WRITE] to force STOP/START instead of repeated start between back-to-back messages

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

static const I2C_REGS *i2C_channel_reg[I2C_AVAILABLE] = {
    (I2C_REGS *)I2C1_BLOCK,
#if I2C_AVAILABLE > 1
    (I2C_REGS *)I2C2_BLOCK,
#endif
#if I2C_AVAILABLE > 2
    (I2C_REGS *)I2C3_BLOCK,
#endif
};

/* =================================================================== */
/*                      local variable definitions                     */
/* =================================================================== */

/* =================================================================== */
/*                     global variable definitions                     */
/* =================================================================== */

/* =================================================================== */
/*                            I2C Interface                            */
/* =================================================================== */



// I2C interrupt routine - shared by all channels
//
static void _I2C_Interrupt(QUEUE_HANDLE Channel)
{
    I2C_REGS *ptrRegs = (I2C_REGS *)i2C_channel_reg[Channel];
    I2CQue *tx_control = I2C_tx_control[Channel];
    I2CQue *rx_control = I2C_rx_control[Channel];
    volatile unsigned long ulStatus1_rx, ulStatus2;

    ulStatus1_rx = ptrRegs->I2C_SR1;                                     // read the status register

    switch (tx_control->ucState & (TX_WAIT | TX_ACTIVE | WAIT_FINAL_BYTE_TRANSMISSION)) {
    case (TX_WAIT | TX_ACTIVE):                                          // waiting for start condition to be sent
        fnTxI2C(I2C_tx_control[Channel], Channel);
        return;
    case (TX_WAIT | TX_ACTIVE | WAIT_FINAL_BYTE_TRANSMISSION):           // waiting for last transmitted byte to complete
        ptrRegs->I2C_CR2 &= ~(I2C_CR2_ITBUFEN | I2C_CR2_ITEVTEN | I2C_CR2_ITERREN); // disable further interrupts
        if (0 == tx_control->I2C_queue.chars) {                          // transmission complete
            ptrRegs->I2C_CR1 = (I2C_CR1_PE | I2C_CR1_ACK | I2C_CR1_STOP);// send stop condition (after final byte transmission)
            tx_control->ucState &= ~(TX_WAIT | TX_ACTIVE | RX_ACTIVE | WAIT_FINAL_BYTE_TRANSMISSION);
            if (tx_control->wake_task != 0) {
                uTaskerStateChange(tx_control->wake_task, UTASKER_ACTIVATE); // wake up owner task since the transmission has terminated
            }
        }
        else {                                                           // following message waiting
#if defined NO_REPEATED_START_ON_WRITE
            ptrRegs->I2C_CR1 = (I2C_CR1_PE | I2C_CR1_ACK | I2C_CR1_START | I2C_CR1_STOP); // set transmit mode and command start condition to be sent
#else
            ptrRegs->I2C_CR1 = (I2C_CR1_PE | I2C_CR1_ACK | I2C_CR1_START); // set transmit mode and command start condition to be sent
#endif
            ptrRegs->I2C_DR = 0xff;                                      // dummy - will not be sent due to the start/stop condition already being set but it will stop the BTF flag from remaining pending
            ptrRegs->I2C_CR2 |= (I2C_CR2_ITEVTEN | I2C_CR2_ITERREN);     // re-enable event interrupt
            tx_control->ucState &= ~(WAIT_FINAL_BYTE_TRANSMISSION);      // mark that we are waiting for a stop/start or repeated start to complete
#if defined _WINDOWS
            iInts |= (I2C_INT0 << Channel);
#endif
        }
        return;
    }

    if ((ulStatus1_rx & I2C_SR1_AF) != 0) {                              // {9} the slave has not acknowledged so we abort the transfer, sending either a stop condition or a repeated start
        ptrRegs->I2C_SR1 = 0;                                            // clear the error flag
        ptrRegs->I2C_CR2 &= ~(I2C_CR2_ITBUFEN | I2C_CR2_ITEVTEN | I2C_CR2_ITERREN); // {9} disable interrupts
        if (tx_control->ucState & RX_ACTIVE) {                           // if this is during a reception sequence
            int iDummyReception = (tx_control->ucPresentLen != 0);
            while (tx_control->ucPresentLen) {
                tx_control->ucPresentLen--;
                *rx_control->I2C_queue.put++ = 0xff;                     // put a dummy reception to the input buffer for compatibility
                rx_control->I2C_queue.chars++;                           // and put it into the rx buffer
                if (rx_control->I2C_queue.put >= rx_control->I2C_queue.buffer_end) {
                    rx_control->I2C_queue.put = rx_control->I2C_queue.QUEbuffer;
                }
            }
            if (iDummyReception != 0) {
                rx_control->msgs++;
                if (rx_control->wake_task) {                             // wake up the receiver task if desired
                    uTaskerStateChange(rx_control->wake_task, UTASKER_ACTIVATE); // wake up owner task
                }
            }
        }
        else {
            while (tx_control->ucPresentLen != 0) {                      // flush the transmission queue
                tx_control->ucPresentLen--;
                tx_control->I2C_queue.get++;
                if (tx_control->I2C_queue.get >= tx_control->I2C_queue.buffer_end) { // flush the 
                    tx_control->I2C_queue.get = tx_control->I2C_queue.QUEbuffer; // handle the ring buffer
                }
                if (tx_control->ucPresentLen != 0) {
                    tx_control->I2C_queue.chars--;
                }
            }
            if (tx_control->I2C_queue.chars == 0) {
                if (tx_control->wake_task != 0) {
                   uTaskerStateChange(tx_control->wake_task, UTASKER_ACTIVATE); // wake up owner task since the transmission has terminated (failed)
                }
            }
        }
        tx_control->ucState &= ~(TX_WAIT | TX_ACTIVE | RX_ACTIVE | RX_ACTIVE_FIRST_READ);
        if (tx_control->I2C_queue.chars != 0) {
            fnTxI2C(tx_control, Channel);                                // we have another message to send so we can send a repeated start condition
        }
        else {
            ptrRegs->I2C_CR1 = (I2C_CR1_PE | I2C_CR1_STOP);              // command stop condition
        }
        return;
    }

    if ((tx_control->ucState & RX_ACTIVE) != 0) {
        unsigned char ucFirstRead = (tx_control->ucState & RX_ACTIVE_FIRST_READ);
        tx_control->ucState &= ~(RX_ACTIVE_FIRST_READ);
        __disable_interrupt();                                           // disable interrupts to ensure that the write to CR1 can not be delayed by a higher priority interrupt routine
        ulStatus2 = ptrRegs->I2C_SR2;                                    // this allows the receiver to receive up to 2 bytes
        if (tx_control->ucPresentLen == 1) {
            ptrRegs->I2C_CR1 = (I2C_CR1_PE | I2C_CR1_STOP);              // we don't acknowledge last byte - command stop
            __enable_interrupt();                                        // critical region end
        }
        else if (tx_control->ucPresentLen == 0) {                        // we have completed the read
            ptrRegs->I2C_CR2 &= ~(I2C_CR2_ITBUFEN | I2C_CR2_ITEVTEN | I2C_CR2_ITERREN); // {9} disable further I2C interrupts
            ptrRegs->I2C_CR1 = (I2C_CR1_PE | I2C_CR1_ACK | I2C_CR1_STOP);// send end condition
            __enable_interrupt();                                        // critical region end
            tx_control->ucState &= ~(TX_WAIT | TX_ACTIVE | RX_ACTIVE);
            rx_control->msgs++;
            if (rx_control->wake_task != 0) {                            // wake up the receiver task if desired
                uTaskerStateChange(rx_control->wake_task, UTASKER_ACTIVATE); // wake up owner task
            }
        }
        else {
            __enable_interrupt();                                        // critical region end
        }

        if (ucFirstRead == 0) {                                          // if we haven't just sent the slave address?
            ulStatus1_rx = ptrRegs->I2C_DR;                              // read received byte
            *rx_control->I2C_queue.put++ = (unsigned char)ulStatus1_rx;  // save the byte
            rx_control->I2C_queue.chars++;                               // and put it into the rx buffer
            if (rx_control->I2C_queue.put >= rx_control->I2C_queue.buffer_end) {
                rx_control->I2C_queue.put = rx_control->I2C_queue.QUEbuffer;
            }
        }

        if (tx_control->ucPresentLen != 0) {
            tx_control->ucPresentLen--;
    #if defined _WINDOWS
            ptrRegs->I2C_DR = fnSimI2C_devices(I2C_RX_DATA, 0);          // simulate the interrupt directly
            iInts |= (I2C_INT0 << Channel);
    #endif
        }
        else {                                                           // read sequence complete. Continue with next write if something is waiting.
            if (tx_control->I2C_queue.chars != 0) {
                fnTxI2C(tx_control, Channel);                            // we have another message to send so we can send a repeated start condition
            }
        }
        return;
    }

    ulStatus2 = ptrRegs->I2C_SR2;                                        // transmission case
    if (tx_control->ucPresentLen-- != 0) {                               // send next byte to send if available
        ptrRegs->I2C_DR = *tx_control->I2C_queue.get++;
        if (tx_control->I2C_queue.get >= tx_control->I2C_queue.buffer_end) {
            tx_control->I2C_queue.get = tx_control->I2C_queue.QUEbuffer; // handle the ring buffer
        }
    #if defined _WINDOWS
        fnSimI2C_devices(I2C_TX_DATA, (unsigned char)ptrRegs->I2C_DR);
        iInts |= (I2C_INT0 << Channel);
    #endif
    }
    else {
        ptrRegs->I2C_CR2 &= ~(I2C_CR2_ITBUFEN);                          // disable transmit empty interrupt but leave transmit complete interrupt enabled
        tx_control->ucState |= (TX_WAIT | TX_ACTIVE | WAIT_FINAL_BYTE_TRANSMISSION); // we need to wait until the final byte transmission completes
    }
}



// Channel 1 I2C interrupt
//
static __interrupt void _I2C_Interrupt_1(void)
{
    _I2C_Interrupt(0);                                                   // call general handler
}

#if I2C_AVAILABLE > 1
// Channel 2 I2C interrupt
//
static __interrupt void _I2C_Interrupt_2(void)
{
    _I2C_Interrupt(1);                                                   // call general handler
}
#endif

#if I2C_AVAILABLE > 2                                                    // {5}
// Channel 3 I2C interrupt
//
static __interrupt void _I2C_Interrupt_3(void)
{
    _I2C_Interrupt(2);                                                   // call general handler
}
#endif

// Configure the I2C hardware
//
extern void fnConfigI2C(I2CTABLE *pars)
{
    I2C_REGS *ptrRegs;
    switch (pars->Channel) {
    case 0:
        POWER_UP(APB1, (RCC_APB1ENR_I2C1EN));                            // enable clocks to I2C1
    #if defined _STM32F2XX || defined _STM32F4XX || defined _STM32F7XX
        #if defined I2C1_ALT_PINS_3                                      // {30}
        _CONFIG_PERIPHERAL_OUTPUT(B, (PERIPHERAL_I2C1_2_3), (PORTB_BIT6 | PORTB_BIT7), (OUTPUT_MEDIUM | OUTPUT_OPEN_DRAIN));
        #elif defined I2C1_ALT_PINS_2                                    // {28}
        _CONFIG_PERIPHERAL_OUTPUT(B, (PERIPHERAL_I2C1_2_3), (PORTB_BIT8 | PORTB_BIT9), (OUTPUT_MEDIUM | OUTPUT_OPEN_DRAIN));
        #elif defined I2C1_ALT_PINS                                      // {5}
        _CONFIG_PERIPHERAL_OUTPUT(B, (PERIPHERAL_I2C1_2_3), (PORTB_BIT8 | PORTB_BIT7), (OUTPUT_MEDIUM | OUTPUT_OPEN_DRAIN));
        #else
        _CONFIG_PERIPHERAL_OUTPUT(B, (PERIPHERAL_I2C1_2_3), (PORTB_BIT6 | PORTB_BIT9), (OUTPUT_MEDIUM | OUTPUT_OPEN_DRAIN));
        #endif
    #else
        _CONFIG_PERIPHERAL_OUTPUT(B, (PERIPHERAL_I2C1_2_3), (PORTB_BIT6 | PORTB_BIT7), (OUTPUT_MEDIUM | OUTPUT_OPEN_DRAIN));
    #endif
        fnEnterInterrupt(irq_I2C1_EV_ID, PRIORITY_I2C1, _I2C_Interrupt_1); // enter I2C interrupt handlers
        fnEnterInterrupt(irq_I2C1_ER_ID, PRIORITY_I2C1, _I2C_Interrupt_1); // {9}
        break;

    #if I2C_AVAILABLE > 1
    case 1:                                                              // channel 1 (I2C2)
        POWER_UP(APB1, (RCC_APB1ENR_I2C2EN));                            // enable clocks to I2C2
        #if defined _STM32F2XX || defined _STM32F4XX || defined _STM32F7XX
            #if defined I2C2_ALT_PINS_2                                  // {5}
        _CONFIG_PERIPHERAL_OUTPUT(H, (PERIPHERAL_I2C1_2_3), (PORTH_BIT4 | PORTH_BIT5), (OUTPUT_MEDIUM | OUTPUT_OPEN_DRAIN));
            #elif defined I2C2_ALT_PINS_1
        _CONFIG_PERIPHERAL_OUTPUT(F, (PERIPHERAL_I2C1_2_3), (PORTF_BIT0 | PORTF_BIT1), (OUTPUT_MEDIUM | OUTPUT_OPEN_DRAIN));
            #else
        _CONFIG_PERIPHERAL_OUTPUT(B, (PERIPHERAL_I2C1_2_3), (PORTB_BIT10 | PORTB_BIT11), (OUTPUT_MEDIUM | OUTPUT_OPEN_DRAIN));
            #endif
        #else
        _CONFIG_PERIPHERAL_OUTPUT(B, (PERIPHERAL_I2C1_2_3), (PORTB_BIT10 | PORTB_BIT11), (OUTPUT_MEDIUM | OUTPUT_OPEN_DRAIN));
        #endif
        fnEnterInterrupt(irq_I2C2_EV_ID, PRIORITY_I2C2, _I2C_Interrupt_2); // enter I2C interrupt handler
        fnEnterInterrupt(irq_I2C2_ER_ID, PRIORITY_I2C2, _I2C_Interrupt_2); // {9}
        break;
    #endif

    #if I2C_AVAILABLE > 2                                                // {5}
    case 2:
        POWER_UP(APB1, (RCC_APB1ENR_I2C3EN));                            // enable clocks to I2C3
        #if defined I2C3_ALT_PINS
        _CONFIG_PERIPHERAL_OUTPUT(H, (PERIPHERAL_I2C1_2_3), (PORTH_BIT7 | PORTH_BIT8), (OUTPUT_MEDIUM | OUTPUT_OPEN_DRAIN));
        #else
        _CONFIG_PERIPHERAL_OUTPUT(C, (PERIPHERAL_I2C1_2_3), (PORTC_BIT9), (OUTPUT_MEDIUM | OUTPUT_OPEN_DRAIN));
        _CONFIG_PERIPHERAL_OUTPUT(A, (PERIPHERAL_I2C1_2_3), (PORTA_BIT8), (OUTPUT_MEDIUM | OUTPUT_OPEN_DRAIN));
        #endif
        fnEnterInterrupt(irq_I2C3_EV_ID, PRIORITY_I2C3, _I2C_Interrupt_3); // enter I2C interrupt handler
        fnEnterInterrupt(irq_I2C3_ER_ID, PRIORITY_I2C3, _I2C_Interrupt_3); // {9}
        break;
    #endif
    default:
        _EXCEPTION("Invalid I2C channel!");
        return;
    }
    ptrRegs = (I2C_REGS *)i2C_channel_reg[pars->Channel];
    while ((ptrRegs->I2C_SR2 & I2C_SR2_BUSY) != 0) {                     // of the the busy bit is set due to initial state of ports
        ptrRegs->I2C_CR1 = I2C_CR1_SWRST;                                // command software reset of the channel to clear the busy bit
    }
    ptrRegs->I2C_CR1 = 0;                                                // remove reset
    ptrRegs->I2C_CR2 = (PCLK1/1000000);
    if (pars->usSpeed == 400) {                                          // high speed 400k (peripheral input clock frequency must be at least 4 MHz)
        ptrRegs->I2C_TRISE = ((((PCLK1/1000000) * 300)/1000) + 1);       // set maximum rise time to 300ns in fast mode
        ptrRegs->I2C_CCR = (I2C_CCR_FS | I2C_CCR_DUTY_16_9 | ((PCLK1 + (25 * 400000))/(25 * 400000)));
    }
    else {                                                               // 100k (peripheral input clock frequency must be at least 2 MHz)
        ptrRegs->I2C_TRISE = ((PCLK1/1000000) + 1);                      // set maximum rise time to 1us in normal mode
        ptrRegs->I2C_CCR = ((PCLK1 + 2 * 100000)/(2 * 100000));          // set the clock speed to 100kHz
    }
    ptrRegs->I2C_CR1 = (I2C_CR1_ACK | I2C_CR1_PE);                       // activate I2C interface
    #if defined _WINDOWS
    fnConfigSimI2C(pars->Channel, (pars->usSpeed * 1000));
    #endif
}


// Send a first byte to I2C bus
//
extern void fnTxI2C(I2CQue *ptI2CQue, QUEUE_HANDLE Channel)
{
    I2C_REGS *ptrRegs = (I2C_REGS *)i2C_channel_reg[Channel];
    unsigned char ucAddress;
    I2CQue *tx_control = I2C_tx_control[Channel];
    I2CQue *rx_control = I2C_rx_control[Channel];

    if ((tx_control->ucState & (TX_WAIT | TX_ACTIVE | WAIT_FINAL_BYTE_TRANSMISSION)) == 0) {
        if ((ptrRegs->I2C_SR2 & (I2C_SR2_BUSY | I2C_SR2_MSL)) == I2C_SR2_BUSY) { // {8} check whether the bus is busy before initiating data transfer (the master bit is set if this is a repeated start, where the SCL line will be at '0')
            // If the bus is busy it means that SDA and SCL are not both at '1' - since we support only master mode this probably means a hardware error or missing pull-up resistors
            //
            return;                                                      // we return to avoid the processor waiting indefinitely for the start bit to be send but the problem should be resolved before futher operation is possible
        }

        ptrRegs->I2C_CR1 = (I2C_CR1_PE | I2C_CR1_ACK | I2C_CR1_START);   // set transmit mode and command start condition to be sent
        ptrRegs->I2C_CR2 |= (I2C_CR2_ITEVTEN);                           // generate interrupt when start condition has been sent
    #if defined _WINDOWS
        ptrRegs->I2C_SR1 |= I2C_SR1_SB;
        iInts |= (I2C_INT0 << Channel);
    #endif
        tx_control->ucState |= (TX_WAIT | TX_ACTIVE);                    // mark that we are waiting for the start condition to be sent
        return;
    }
    tx_control->ucState &= ~(TX_WAIT | TX_ACTIVE | WAIT_FINAL_BYTE_TRANSMISSION);

    ptI2CQue->ucPresentLen = *ptI2CQue->I2C_queue.get++;                 // get present length
    if (ptI2CQue->I2C_queue.get >= ptI2CQue->I2C_queue.buffer_end) {
        ptI2CQue->I2C_queue.get = ptI2CQue->I2C_queue.QUEbuffer;         // handle circular buffer
    }
    ucAddress = *ptI2CQue->I2C_queue.get++;                              // get the address to send
    ptrRegs->I2C_DR = ucAddress;                                         // send the slave address (this includes the rd/wr bit and the action clears I2C1_SR1_SB)
    if ((ucAddress & 0x01) != 0) {                                       // reading
        if (ptI2CQue->I2C_queue.get >= ptI2CQue->I2C_queue.buffer_end) {
            ptI2CQue->I2C_queue.get = ptI2CQue->I2C_queue.QUEbuffer;     // handle circular buffer
        }

        tx_control->ucState |= (RX_ACTIVE | RX_ACTIVE_FIRST_READ | TX_ACTIVE);
        ptI2CQue->I2C_queue.chars -= 3;
        rx_control->wake_task = *ptI2CQue->I2C_queue.get++;              // enter task to be woken when reception has completed
        if (ptI2CQue->I2C_queue.get >= ptI2CQue->I2C_queue.buffer_end) {
            ptI2CQue->I2C_queue.get = ptI2CQue->I2C_queue.QUEbuffer;     // handle circular buffer
        }
    }
    else {                                                               // writing
        if (ptI2CQue->I2C_queue.get >= ptI2CQue->I2C_queue.buffer_end) {
            ptI2CQue->I2C_queue.get = ptI2CQue->I2C_queue.QUEbuffer;     // handle circular buffer
        }

        tx_control->ucState |= (TX_ACTIVE);                              // writing
        ptI2CQue->I2C_queue.chars -= (ptI2CQue->ucPresentLen + 1);
    }
    ptrRegs->I2C_CR2 |= (I2C_CR2_ITBUFEN | I2C_CR2_ITEVTEN | I2C_CR2_ITERREN); // {9} generate interrupt when address has been sent or on error
    #if defined _WINDOWS
    ptrRegs->I2C_SR1 = (I2C_SR1_TxE | I2C_SR1_ADDR);
    fnSimI2C_devices(I2C_ADDRESS, (unsigned char)ptrRegs->I2C_DR);       // simulate address transmission
    iInts |= (I2C_INT0 << Channel);
    #endif
}
