/**********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher

    ---------------------------------------------------------------------
    File:        spi_sc16IS7xx.h [Kinetis]
    Project:     Single Chip Embedded Internet 
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2016
    *********************************************************************

*/ 


#if defined _WINDOWS
    extern int fnCheckExtinterrupts(int iChannel);
#endif

#define SC16IS7XX_READ                            0x80

#define SC16IS7XX_CHANNEL_A                       0x00
#define SC16IS7XX_CHANNEL_B                       0x02

#define SC16IS7XX_REG_RHR                         ((0x00 << 3) | SC16IS7XX_READ)
#define SC16IS7XX_REG_THR                         0x00

#define SC16IS7XX_REG_IER                         0x01
#define SC16IS7XX_REG_IER_READ                    ((SC16IS7XX_REG_IER << 3) | SC16IS7XX_READ)
  #define SC16IS7XX_IER_ENABLE_RX_INT             0x01
  #define SC16IS7XX_IER_ENABLE_TX_INT             0x02
  #define SC16IS7XX_IER_ENABLE_RX_LINE_STATUS_INT 0x04
  #define SC16IS7XX_IER_ENABLE_MODEM_STATUS_INT   0x08
  #define SC16IS7XX_IER_ENABLE_SLEEP_MODE         0x10
  #define SC16IS7XX_IER_ENABLE_XOFF_INT           0x20
  #define SC16IS7XX_IER_ENABLE_RTS_INT            0x40
  #define SC16IS7XX_IER_ENABLE_CTS_INT            0x80

#define SC16IS7XX_REG_IIR                         ((0x02 << 3) | SC16IS7XX_READ)
  #define SC16IS7XX_IIR_NO_INT                    0x01
  #define SC16IS7XX_IIR_RX_LINE_STATUS_INT        0x06
  #define SC16IS7XX_IIR_RX_TIMEOUT_INT            0x0c
  #define SC16IS7XX_IIR_RHR_INT                   0x04
  #define SC16IS7XX_IIR_THR_INT                   0x02
  #define SC16IS7XX_IIR_MODEM_STATUS_CHANGE_INT   0x00
  #define SC16IS7XX_IIR_IO_PIN_CHANGE_INT         0x0e
  #define SC16IS7XX_IIR_XOFF_INT                  0x10
  #define SC16IS7XX_IIR_CTS_RTS_INACTIVE_INT      0x20
  #define SC16IS7XX_IIR_INT_MASK                  0x3f

#define SC16IS7XX_REG_FCR                         0x02
  #define SC16IS7XX_FCR_FIFO_ENABLE               0x01
  #define SC16IS7XX_FCR_RX_FIFO_RESET             0x02
  #define SC16IS7XX_FCR_TX_FIFO_RESET             0x04

  #define SC16IS7XX_FCR_TX_FIFO_TRIGGER_8_SPACES  0x00
  #define SC16IS7XX_FCR_TX_FIFO_TRIGGER_16_SPACES 0x10
  #define SC16IS7XX_FCR_TX_FIFO_TRIGGER_32_SPACES 0x20
  #define SC16IS7XX_FCR_TX_FIFO_TRIGGER_56_SPACES 0x30
  #define SC16IS7XX_FCR_RX_FIFO_TRIGGER_8_SPACES  0x00
  #define SC16IS7XX_FCR_RX_FIFO_TRIGGER_16_SPACES 0x40
  #define SC16IS7XX_FCR_RX_FIFO_TRIGGER_32_SPACES 0x80
  #define SC16IS7XX_FCR_RX_FIFO_TRIGGER_56_SPACES 0xc0

#define SC16IS7XX_REG_LCR                         0x03
#define SC16IS7XX_REG_LCR_READ                    ((0x03 << 3) | SC16IS7XX_READ)
  #define SC16IS7XX_WORD_LENGTH_5_BITS            0x00
  #define SC16IS7XX_WORD_LENGTH_6_BITS            0x01
  #define SC16IS7XX_WORD_LENGTH_7_BITS            0x02
  #define SC16IS7XX_WORD_LENGTH_8_BITS            0x03
  #define SC16IS7XX_1_STOP_BIT                    0x00
  #define SC16IS7XX_2_STOP_BITS                   0x04                   // 1.5 stop bits in 5 bit mode
  #define SC16IS7XX_NO_PARITY                     0x00
  #define SC16IS7XX_PARITY_ENABLE                 0x08
  #define SC16IS7XX_EVEN_PARITY_ENABLE            0x10
  #define SC16IS7XX_EVEN_PARITY                   (SC16IS7XX_PARITY_ENABLE | SC16IS7XX_EVEN_PARITY_ENABLE)
  #define SC16IS7XX_ODD_PARITY                    0x08 
  #define SC16IS7XX_LCR_FORCE_BREAK               0x40
  #define SC16IS7XX_LCR_DIVISOR_LATCH_ENABLE      0x80                   // used to enable write to DLL and DLH
  #define SC16IS7XX_LCR_ACCESS_EXTENDED_REGISTERS 0xbf                   // used to enable access to extended registers

#define SC16IS7XX_REG_MCR                         0x04
#define SC16IS7XX_REG_MCR_READ                    ((0x04 << 3) | SC16IS7XX_READ)
  #define SC16IS7XX_MCR_ASSERT_DTR                0x01
  #define SC16IS7XX_MCR_ASSERT_RTS                0x02
  #define SC16IS7XX_MCR_TCR_TLR_ENABLE            0x04                   // only modified when EFR[4] is set

  #define SC16IS7XX_MCR_ENABLE_LOOPBACK           0x10
  #define SC16IS7XX_MCR_ENABLE_XON_ANY            0x20                   // only modified when EFR[4] is set
  #define SC16IS7XX_MCR_IRDA_MODE                 0x40                   // only modified when EFR[4] is set
  #define SC16IS7XX_MCR_DIVIDE_BY_4               0x80                   // only modified when EFR[4] is set

#define SC16IS7XX_REG_LSR                         ((0x05 << 3) | SC16IS7XX_READ)
  #define SC16IS7XX_LSR_DATA_IN_RECEIVER          0x01
  #define SC16IS7XX_LSR_OVERRUN_ERROR             0x02
  #define SC16IS7XX_LSR_PARITY_ERROR              0x04
  #define SC16IS7XX_LSR_FRAMING_ERROR             0x08
  #define SC16IS7XX_LSR_BREAK_INTERRUPT           0x10
  #define SC16IS7XX_LSR_THR_EMPTY                 0x20
  #define SC16IS7XX_LSR_TSR_AND_THR_EMPTY         0x40
  #define SC16IS7XX_LSR_FIFO_DATA_ERROR           0x80

#define SC16IS7XX_REG_TCR                         0x06
#define SC16IS7XX_REG_MSR_READ                    ((0x06 << 3) | SC16IS7XX_READ)
  #define SC16IS7XX_MSR_CTS_CHANGED_STATE         0x01
  #define SC16IS7XX_MSR_DTR_CHANGED_STATE         0x02
  #define SC16IS7XX_MSR_RI_CHANGED_STATE          0x04
  #define SC16IS7XX_MSR_CD_CHANGED_STATE          0x08
  #define SC16IS7XX_MSR_CTS_ASSERTED              0x10
  #define SC16IS7XX_MSR_DSR_ASSERTED              0x20
  #define SC16IS7XX_MSR_RI_ASSERTED               0x40
  #define SC16IS7XX_MSR_CD_ASSERTED               0x80

#define SC16IS7XX_REG_TLR                         0x07
#define SC16IS7XX_REG_TXLVL                       ((0x08 << 3) | SC16IS7XX_READ)
#define SC16IS7XX_REG_RXLVL                       ((0x09 << 3) | SC16IS7XX_READ)
#define SC16IS7XX_REG_IODIR                       0x0a
#define SC16IS7XX_REG_IODIR_READ                  ((0x0a << 3) | SC16IS7XX_READ)
#define SC16IS7XX_REG_IOSTATE                     0x0b
#define SC16IS7XX_REG_IOSTATE_READ                ((0x0b << 3) | SC16IS7XX_READ)
#define SC16IS7XX_REG_IOINTENA                    0x0c
#define SC16IS7XX_REG_IOINTENA_READ               ((0x0c << 3) | SC16IS7XX_READ)

#define SC16IS7XX_REG_IOCONTROL                   0x0e
#define SC16IS7XX_REG_IOCONTROL_READ              ((0x0e << 3) | SC16IS7XX_READ)
#define SC16IS7XX_REG_EFCR                        0x0f
#define SC16IS7XX_REG_EFCR_READ                   ((0x0f << 3) | SC16IS7XX_READ)
  #define SC16IS7XX_EFCR_RTSCON                   0x10
  #define SC16IS7XX_EFCR_RTSINVER                 0x20

#define SC16IS7XX_REG_DLL                         0x00
#define SC16IS7XX_REG_DLL_READ                    ((0x00 << 3) | SC16IS7XX_READ)
#define SC16IS7XX_REG_DLH                         0x01
#define SC16IS7XX_REG_DLH_READ                    ((0x01 << 3) | SC16IS7XX_READ)
#define SC16IS7XX_REG_EFR                         0x02
#define SC16IS7XX_REG_EFR_READ                    ((0x02 << 3) | SC16IS7XX_READ)
  #define SC16IS7XX_EFR_ENABLE_ENHANCED           0x10
#define SC16IS7XX_REG_XON1                        0x04
#define SC16IS7XX_REG_XON1_READ                   ((0x04 << 3) | SC16IS7XX_READ)
#define SC16IS7XX_REG_XON2                        0x05
#define SC16IS7XX_REG_XON2_READ                   ((0x05 << 3) | SC16IS7XX_READ)
#define SC16IS7XX_REG_XOFF1                       0x06
#define SC16IS7XX_REG_XOFF1_READ                  ((0x06 << 3) | SC16IS7XX_READ)
#define SC16IS7XX_REG_XOFF2                       0x07
#define SC16IS7XX_REG_XOFF2_READ                  ((0x07 << 3) | SC16IS7XX_READ)

static unsigned char ucIER[NUMBER_EXTERNAL_SERIAL] = {0};                // backup of the IER value written (to reduce register read requirement)


// Transmit a single byte proceeded by the write address
//
static void fnSendExtSCI_byte(QUEUE_HANDLE Channel, unsigned char ucAddress, unsigned char ucData)
{
    unsigned char ucRegAddress = (((Channel & 0x01) << 1) | (ucAddress << 3));
#if NUMBER_EXTERNAL_SERIAL > 2
    if (Channel & 0x02) {                                                // drive the appropriate chip select line
        ENABLE_CHIP_SELECT_SC16IS7XX_1();
    }
    else {
        ENABLE_CHIP_SELECT_SC16IS7XX_0();
    }
#else
    ENABLE_CHIP_SELECT_SC16IS7XX_0();
#endif
    WRITE_SC16IS7XX_SPI(ucRegAddress);                                   // transmit the address
    WAIT_SC16IS7XX_SPI_RECEPTION_END();                                  // wait until tx byte has been sent and rx byte has been completely received
    (void)READ_SC16IS7XX_SPI_FLASH_DATA();                               // reset receive data flag with dummy read - the rx data is not interesting here
    WRITE_SC16IS7XX_SPI_LAST(ucData);                                    // transmit the single data byte
    WAIT_SC16IS7XX_SPI_RECEPTION_END();                                  // wait until tx byte has been sent and rx byte has been completely received
    (void)READ_SC16IS7XX_SPI_FLASH_DATA();                               // reset receive data flag with dummy read - the rx data is not interesting here
#if defined _WINDOWS
    _fnSimExtSCI(Channel, ucRegAddress, ucData);                         // simulate the transmission
#endif
    DISABLE_CHIP_SELECTS_SC16IS7XX();                                    // always release all chips select lines when exiting
}

// Read a single byte proceeded by the read address
//
static unsigned char fnReadExtSCI_byte(QUEUE_HANDLE Channel, unsigned char ucAddress)
{
    register unsigned long ulRxData;
#if NUMBER_EXTERNAL_SERIAL > 2
    if (Channel & 0x02) {                                                // drive the appropriate chip select line
        ENABLE_CHIP_SELECT_SC16IS7XX_1();
    }
    else {
        ENABLE_CHIP_SELECT_SC16IS7XX_0();
    }
#else
    ENABLE_CHIP_SELECT_SC16IS7XX_0();
#endif
    WRITE_SC16IS7XX_SPI(ucAddress);                                      // transmit the address
    WAIT_SC16IS7XX_SPI_RECEPTION_END();                                  // wait until tx byte has been sent and rx byte has been completely received
    (void)READ_SC16IS7XX_SPI_FLASH_DATA();                               // reset receive data flag with dummy read - the rx data is not interesting here

    WRITE_SC16IS7XX_SPI_LAST(0xff);                                      // send dummy byte (idle line)
    WAIT_SC16IS7XX_SPI_RECEPTION_END();                                  // wait until tx byte has been sent and rx byte has been completely received
    ulRxData = READ_SC16IS7XX_SPI_FLASH_DATA();                          // read the returned data
#ifdef _WINDOWS
    ulRxData = _fnSimExtSCI(Channel, ucAddress, 0);
#endif
    DISABLE_CHIP_SELECTS_SC16IS7XX();                                    // always release all chips select lines
    return (unsigned char)ulRxData;
}

// Handle an interrupt from chip 0
//
static void ext_sci_0_interrupt(void)
{
    unsigned char ucInterrupt;
    while (CHECK_UART_0_1_INT_PENDING() == 0) {                          // while the interrupt input stays low
        ucInterrupt = (fnReadExtSCI_byte(0, (SC16IS7XX_CHANNEL_A | SC16IS7XX_REG_IIR)) & SC16IS7XX_IIR_INT_MASK); // check from internal channel A
        if (ucInterrupt != SC16IS7XX_IIR_NO_INT) {                       // if an interrupt is pending on this channel
            if ((ucInterrupt == SC16IS7XX_IIR_RHR_INT) || (ucInterrupt == SC16IS7XX_IIR_RX_TIMEOUT_INT)) {
                fnSciRxByte(fnReadExtSCI_byte(0, (SC16IS7XX_CHANNEL_A | SC16IS7XX_REG_RHR)), NUMBER_SERIAL); // read and process received character
            }
            else if (ucInterrupt == SC16IS7XX_IIR_THR_INT) {
                fnSciTxByte(NUMBER_SERIAL);                              // transmit next byte
            }
    #if defined SUPPORT_HW_FLOW
            else if (ucInterrupt == SC16IS7XX_IIR_MODEM_STATUS_CHANGE_INT) { // modem status change
                unsigned char ucModemChange = fnReadExtSCI_byte(0, (SC16IS7XX_CHANNEL_A | SC16IS7XX_REG_MSR_READ)); // read the state and clear the interrupt
                if (ucModemChange & SC16IS7XX_MSR_CTS_CHANGED_STATE) {   // change due to CTS on thsi channel
                    fnRTS_change(NUMBER_SERIAL, ((ucModemChange & SC16IS7XX_MSR_CTS_ASSERTED) != 0)); // control transmission according to state 
                }
            }
    #endif
        }
    #if NUMBER_EXTERNAL_SERIAL > 1
        ucInterrupt = (fnReadExtSCI_byte(1, (SC16IS7XX_CHANNEL_B | SC16IS7XX_REG_IIR)) & SC16IS7XX_IIR_INT_MASK);// check from internal channel B
        if (ucInterrupt != SC16IS7XX_IIR_NO_INT) {                       // if an interrupt is pending
            if ((ucInterrupt == SC16IS7XX_IIR_RHR_INT) || (ucInterrupt == SC16IS7XX_IIR_RX_TIMEOUT_INT)) {
                fnSciRxByte(fnReadExtSCI_byte(1, (SC16IS7XX_CHANNEL_B | SC16IS7XX_REG_RHR)), (NUMBER_SERIAL + 1)); // read and process received character
            }
            else if (ucInterrupt == SC16IS7XX_IIR_THR_INT) {
                fnSciTxByte((NUMBER_SERIAL + 1));                        // transmit next byte
            }
        #if defined SUPPORT_HW_FLOW
            else if (ucInterrupt == SC16IS7XX_IIR_MODEM_STATUS_CHANGE_INT) { // modem status change
                unsigned char ucModemChange = fnReadExtSCI_byte(1, (SC16IS7XX_CHANNEL_B | SC16IS7XX_REG_MSR_READ)); // read the state and clear the interrupt
                if (ucModemChange & SC16IS7XX_MSR_CTS_CHANGED_STATE) {   // change due to CTS on thsi channel
                    fnRTS_change((NUMBER_SERIAL + 1), ((ucModemChange & SC16IS7XX_MSR_CTS_ASSERTED) != 0)); // control transmission according to state 
                }
            }
        #endif
        }
    #endif
    #if defined _WINDOWS
        if ((fnCheckExtinterrupts(0) == 0) && (fnCheckExtinterrupts(1) == 0)) {
            extern unsigned char fnMapPortBit(unsigned long ulRealBit);
            fnSimulateInputChange(EXT_UART_0_1_INT_PORT, fnMapPortBit(EXT_UART_0_1_INT_BIT), SET_INPUT); // remove interrupt if no more waiting
        }
    #endif
    }
}

#if NUMBER_EXTERNAL_SERIAL > 2
// Handle an interrupt from chip 1
//
static void ext_sci_1_interrupt(void)
{
    unsigned char ucInterrupt;
    while (CHECK_UART_2_3_INT_PENDING() == 0) {                          // while the interrupt input stays low
        ucInterrupt = (fnReadExtSCI_byte(2, (SC16IS7XX_CHANNEL_A | SC16IS7XX_REG_IIR)) & SC16IS7XX_IIR_INT_MASK); // check from internal channel A
        if (ucInterrupt != SC16IS7XX_IIR_NO_INT) {                       // if an interrupt is pending on this channel
            if ((ucInterrupt == SC16IS7XX_IIR_RHR_INT) || (ucInterrupt == SC16IS7XX_IIR_RX_TIMEOUT_INT)) {
                fnSciRxByte(fnReadExtSCI_byte(2, (SC16IS7XX_CHANNEL_A | SC16IS7XX_REG_RHR)), (NUMBER_SERIAL + 2)); // read and process received character
            }
            else if (ucInterrupt == SC16IS7XX_IIR_THR_INT) {
                fnSciTxByte(NUMBER_SERIAL + 2);                          // transmit next byte
            }
    #if defined SUPPORT_HW_FLOW
            else if (ucInterrupt == SC16IS7XX_IIR_MODEM_STATUS_CHANGE_INT) { // modem status change
                unsigned char ucModemChange = fnReadExtSCI_byte(2, (SC16IS7XX_CHANNEL_A | SC16IS7XX_REG_MSR_READ)); // read the state and clear the interrupt
                if (ucModemChange & SC16IS7XX_MSR_CTS_CHANGED_STATE) {   // change due to CTS on thsi channel
                    fnRTS_change((NUMBER_SERIAL + 2), ((ucModemChange & SC16IS7XX_MSR_CTS_ASSERTED) != 0)); // control transmission according to state 
                }
            }
    #endif
        }
        ucInterrupt = (fnReadExtSCI_byte(3, (SC16IS7XX_CHANNEL_B | SC16IS7XX_REG_IIR)) & SC16IS7XX_IIR_INT_MASK);// check from internal channel B
        if (ucInterrupt != SC16IS7XX_IIR_NO_INT) {                       // if an interrupt is pending
            if ((ucInterrupt == SC16IS7XX_IIR_RHR_INT) || (ucInterrupt == SC16IS7XX_IIR_RX_TIMEOUT_INT)) {
                fnSciRxByte(fnReadExtSCI_byte(3, (SC16IS7XX_CHANNEL_B | SC16IS7XX_REG_RHR)), (NUMBER_SERIAL + 3)); // read and process received character
            }
            else if (ucInterrupt == SC16IS7XX_IIR_THR_INT) {
                fnSciTxByte((NUMBER_SERIAL + 3));                        // transmit next byte
            }
    #if defined SUPPORT_HW_FLOW
            else if (ucInterrupt == SC16IS7XX_IIR_MODEM_STATUS_CHANGE_INT) { // modem status change
                unsigned char ucModemChange = fnReadExtSCI_byte(3, (SC16IS7XX_CHANNEL_B | SC16IS7XX_REG_MSR_READ)); // read the state and clear the interrupt
                if (ucModemChange & SC16IS7XX_MSR_CTS_CHANGED_STATE) {   // change due to CTS on thsi channel
                    fnRTS_change((NUMBER_SERIAL + 3), ((ucModemChange & SC16IS7XX_MSR_CTS_ASSERTED) != 0)); // control transmission according to state 
                }
            }
    #endif
        }
    #if defined _WINDOWS
        if ((fnCheckExtinterrupts(2) == 0) && (fnCheckExtinterrupts(3) == 0)) {
            fnSimulateInputChange(EXT_UART_2_3_INT_PORT, (7 - EXT_UART_2_3_INT_BIT), SET_INPUT); // remove interrupt if no more waiting
        }
    #endif
    }
}
#endif

// Configure the external SCI hardware
//
static void fnConfigExtSCI(QUEUE_HANDLE Channel, TTYTABLE *pars)
{
    INTERRUPT_SETUP interrupt_setup;                                     // interrupt configuration parameters
    unsigned short usBaud;
    unsigned char ucMode;

    CONFIGURE_SPI_FOR_SC16IS7XX();                                       // set SPI ports, speed and mode to suit the project

    fnSendExtSCI_byte(Channel, SC16IS7XX_REG_LCR, SC16IS7XX_LCR_DIVISOR_LATCH_ENABLE); // enable programming of baud rate

    switch (pars->ucSpeed) {
    case SERIAL_BAUD_300:
        usBaud = ((SC16IS7XX_CLOCK/16 + 300/2)/300);                     // set 300
        break;
    case SERIAL_BAUD_600:
        usBaud = ((SC16IS7XX_CLOCK/16 + 600/2)/600);                     // set 600
        break;
    case SERIAL_BAUD_1200:
        usBaud = ((SC16IS7XX_CLOCK/16 + 1200/2)/1200);                   // set 1200
        break;
    case SERIAL_BAUD_2400:
        usBaud = ((SC16IS7XX_CLOCK/16 + 2400/2)/2400);                   // set 2400
        break;
    case SERIAL_BAUD_4800:
        usBaud = ((SC16IS7XX_CLOCK/16 + 4800/2)/4800);                   // set 4800
        break;
    case SERIAL_BAUD_9600:
        usBaud = ((SC16IS7XX_CLOCK/16 + 9600/2)/9600);                   // set 9600
        break;
    case SERIAL_BAUD_14400:
        usBaud = ((SC16IS7XX_CLOCK/16 + 14400/2)/14400);                 // set 14400
        break;
    default:                                                             // if not valid value set this one
    case SERIAL_BAUD_19200:
        usBaud = ((SC16IS7XX_CLOCK/16 + 19200/2)/19200);                 // set 19200
        break;
    case SERIAL_BAUD_38400:
        usBaud = ((SC16IS7XX_CLOCK/16 + 38400/2)/38400);                 // set 38400
        break;
    case SERIAL_BAUD_57600:
        usBaud = ((SC16IS7XX_CLOCK/16 + 57600/2)/57600);                 // set 57600
        break;
    case SERIAL_BAUD_115200:
        usBaud = ((SC16IS7XX_CLOCK/16 + 115200/2)/115200);               // set 115200
        break;
    case SERIAL_BAUD_230400:
        usBaud = ((SC16IS7XX_CLOCK/16 + 230400/2)/230400);               // set 230400
        break;
    }

    fnSendExtSCI_byte(Channel, SC16IS7XX_REG_DLL, (unsigned char)usBaud);
    fnSendExtSCI_byte(Channel, SC16IS7XX_REG_DLH, (unsigned char)(usBaud >> 8));
    fnSendExtSCI_byte(Channel, SC16IS7XX_REG_LCR, SC16IS7XX_LCR_ACCESS_EXTENDED_REGISTERS);
    fnSendExtSCI_byte(Channel, SC16IS7XX_REG_EFR, SC16IS7XX_EFR_ENABLE_ENHANCED);

    if (pars->Config & (RS232_ODD_PARITY | RS232_EVEN_PARITY)) {
        if (pars->Config & RS232_ODD_PARITY) {                  
            ucMode = SC16IS7XX_ODD_PARITY;
        }
        else {
            ucMode = SC16IS7XX_EVEN_PARITY;
        }
    }
    else {
        ucMode = SC16IS7XX_NO_PARITY;
    }

    if (pars->Config & TWO_STOPS) {                             
        ucMode |= SC16IS7XX_2_STOP_BITS;
    }

    if (pars->Config & CHAR_7) {                                
        ucMode |= SC16IS7XX_WORD_LENGTH_7_BITS;
    }
    else {
        ucMode |= SC16IS7XX_WORD_LENGTH_8_BITS;
    }

    fnSendExtSCI_byte(Channel, SC16IS7XX_REG_LCR, ucMode);
    fnSendExtSCI_byte(Channel, SC16IS7XX_REG_FCR, SC16IS7XX_FCR_FIFO_ENABLE);

    interrupt_setup.int_type = PORT_INTERRUPT;                           // identifier when configuring
    interrupt_setup.int_priority = PRIORITY_IRQ;                         // port interrupt priority
    interrupt_setup.int_port_sense = (IRQ_FALLING_EDGE | PULLUP_ON);     // interrupt on falling edge with pullup enabled
#if NUMBER_EXTERNAL_SERIAL > 2
    if (Channel & 0x02) {                                                // enable appropriate interrupt handler
        interrupt_setup.int_port = EXT_UART_2_3_INT_PORT;                // the port used
        interrupt_setup.int_port_bits = EXT_UART_2_3_INT_BIT;            // the input connected
        interrupt_setup.int_handler = ext_sci_1_interrupt;               // handling function
    }
    else {
        interrupt_setup.int_port = EXT_UART_0_1_INT_PORT;                // the port used
        interrupt_setup.int_port_bits = EXT_UART_0_1_INT_BIT;            // the input connected
        interrupt_setup.int_handler = ext_sci_0_interrupt;               // handling function
    }
#else
    interrupt_setup.int_port = EXT_UART_0_1_INT_PORT;                    // the port used
    interrupt_setup.int_port_bits = EXT_UART_0_1_INT_BIT;                // the input connected
    interrupt_setup.int_handler = ext_sci_0_interrupt;                   // handling function
#endif
    fnConfigureInterrupt((void *)&interrupt_setup);                      // configure interrupt

    #if defined _WINDOWS
    fnConfigSimSCI((QUEUE_HANDLE)(Channel + NUMBER_SERIAL), (SC16IS7XX_CLOCK/(usBaud * 16)), pars); // open a serial port on PC if desired
    #endif
}

// Transmit a byte of data over the external serial interface
//
static void fnExtSCI_send(QUEUE_HANDLE Channel, unsigned char ucTxByte)
{
    fnSendExtSCI_byte(Channel, SC16IS7XX_REG_THR, ucTxByte);             // transfer the single byte

    #if defined _WINDOWS                                                 // only for the simulator
    switch (Channel) {
    case 0:
        iInts |= CHANNEL_0_EXT_SERIAL_INT;                               // simulate interrupt
        break;
    case 1:
        iInts |= CHANNEL_1_EXT_SERIAL_INT;                               // simulate interrupt
        break;
    case 2:
        iInts |= CHANNEL_2_EXT_SERIAL_INT;                               // simulate interrupt
        break;
    case 3:
        iInts |= CHANNEL_3_EXT_SERIAL_INT;                               // simulate interrupt
        break;
    }
    #endif
}

// Enable the receiver on the particular UART channel
//
static void fnExtSCIRxOn(QUEUE_HANDLE Channel)
{
    ucIER[Channel] |= SC16IS7XX_IER_ENABLE_RX_INT;
    fnSendExtSCI_byte(Channel, SC16IS7XX_REG_IER, ucIER[Channel]);
}

// Disable the receiver on the particular UART channel
//
static void fnExtSCIRxOff(QUEUE_HANDLE Channel)
{
    ucIER[Channel] &= ~SC16IS7XX_IER_ENABLE_RX_INT;
    fnSendExtSCI_byte(Channel, SC16IS7XX_REG_IER, ucIER[Channel]);
}

// Enable the transmitter on the particular UART channel
//
static void fnExtSCITxOn(QUEUE_HANDLE Channel)
{
    ucIER[Channel] |= SC16IS7XX_IER_ENABLE_TX_INT;
    fnSendExtSCI_byte(Channel, SC16IS7XX_REG_IER, ucIER[Channel]);
}

// Disable the transmitter on the particular UART channel
//
static void fnExtSCITxOff(QUEUE_HANDLE Channel)
{
    ucIER[Channel] &= ~SC16IS7XX_IER_ENABLE_TX_INT;
    fnSendExtSCI_byte(Channel, SC16IS7XX_REG_IER, ucIER[Channel]);
}

#if defined SUPPORT_HW_FLOW
// Control the state of RTS line
//
static void fnSetExtRTS(QUEUE_HANDLE channel, int iState)
{
    unsigned char Channel_read;
    if (channel & 1) {
        Channel_read = (SC16IS7XX_CHANNEL_B | SC16IS7XX_REG_MCR_READ);
    }
    else {
        Channel_read = (SC16IS7XX_CHANNEL_A | SC16IS7XX_REG_MCR_READ);
    }
    Channel_read = fnReadExtSCI_byte(channel, Channel_read);             // get the present register setting
    if (iState != 0) {
        Channel_read |= SC16IS7XX_MCR_ASSERT_RTS;                        // assert RTS
    }
    else {
        Channel_read &= ~SC16IS7XX_MCR_ASSERT_RTS;                       // negate RTS
    }
    fnSendExtSCI_byte(channel, SC16IS7XX_REG_MCR, Channel_read);
}

static QUEUE_TRANSFER fnControlExtLineInterrupt(QUEUE_HANDLE channel, unsigned short usModifications, UART_MODE_CONFIG OperatingMode)
{
    unsigned char Channel_read;
    if (channel & 1) {
        Channel_read = (SC16IS7XX_CHANNEL_B | SC16IS7XX_REG_MSR_READ);
    }
    else {
        Channel_read = (SC16IS7XX_CHANNEL_A | SC16IS7XX_REG_MSR_READ);
    }
    if (ENABLE_CTS_CHANGE & usModifications) {
        ucIER[channel] |= SC16IS7XX_IER_ENABLE_MODEM_STATUS_INT;
    }
    if (DISABLE_CTS_CHANGE & usModifications) {
        ucIER[channel] &= ~SC16IS7XX_IER_ENABLE_MODEM_STATUS_INT;
    }
    fnSendExtSCI_byte(channel, SC16IS7XX_REG_IER, ucIER[channel]);       // set new interrupt configuration
    Channel_read = fnReadExtSCI_byte(channel, Channel_read);             // read present state
    if (Channel_read & SC16IS7XX_MSR_CTS_ASSERTED) {
        return SET_CTS;
    }
    else {
        return 0;
    }
}

// The channel is to operate in RS485 mode so set this to the chip (HW control should not be enabled when this is used)
//
extern void fnSetRS485Mode(QUEUE_HANDLE channel, int iPolarity)
{
    unsigned char ucRegEFCR = fnReadExtSCI_byte(channel, SC16IS7XX_REG_EFCR_READ); // read the original value
    ucRegEFCR &= ~(SC16IS7XX_EFCR_RTSCON | SC16IS7XX_EFCR_RTSINVER);     // mask the mode
    if (iPolarity == 0) {
        ucRegEFCR |= (SC16IS7XX_EFCR_RTSCON | SC16IS7XX_EFCR_RTSINVER);  // set auto RS485 mode with positive polarity
    }
    else {
        ucRegEFCR |= (SC16IS7XX_EFCR_RTSCON);                            // set auto RS485 mode with negative polarity
    }
    fnSendExtSCI_byte(channel, SC16IS7XX_REG_EFCR, ucRegEFCR);           // write the new value
}
#endif
