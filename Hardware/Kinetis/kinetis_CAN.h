/***********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher
    
    ---------------------------------------------------------------------
    File:      kinetis_CAN.c
    Project:   Single Chip Embedded Internet
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2018
    *********************************************************************
    11.01.2013 Set CAN controller clock source before moving to freeze mode {65}
    04.03.2014 Correct CAN time stamp request option                     {70}
    02.03.2017 Move controller clock source configuration to after setting freeze mode (due to problems with at least some parts) {3}
    14.03.2017 Set CAN clock before entering freeze mode but write further setting after moving to freeze mode {4}

*/

/* =================================================================== */
/*                       local structure definitions                   */
/* =================================================================== */

// CAN interface queues
//
typedef struct stCANQue
{
    unsigned long  ulPartnerID;                                          // default transmission ID (msb signifies extended)
    unsigned long  ulOwnID;                                              // our own ID (msb signifies extended)
    QUEUE_HANDLE   DriverID;                                             // handle this buffer belongs to
    unsigned char  ucMode;
    unsigned char  ucErrors;                                             // tx error counter per buffer
    UTASK_TASK     TaskToWake;                                           // task responsible for processing rx frames, errors or tx frame complete
} CANQUE;

/* =================================================================== */
/*                      local variable definitions                     */
/* =================================================================== */

#if defined CAN_INTERFACE
    static CANQUE can_queue[NUMBER_OF_CAN_INTERFACES][NUMBER_CAN_MESSAGE_BUFFERS] = {{{0}}};
#endif


static void fnCAN_Message(int iChannel)
{
#if defined MSCAN_CAN_INTERFACE
    // To do...
    //
#else
    static unsigned char can_int_message[HEADER_LENGTH] = {INTERNAL_ROUTE, INTERNAL_ROUTE, 0, INTERRUPT_EVENT, 0};
    KINETIS_CAN_CONTROL *ptrCAN_control;
    unsigned long ulBufferFlag = 0x00000001;
    CANQUE *ptrCanQue;
    int iNoWake = 0;
    unsigned int iBufferID = 0;
    #if NUMBER_OF_CAN_INTERFACES > 1
    if (iChannel != 0) {
        ptrCAN_control = (KINETIS_CAN_CONTROL *)CAN1_BASE_ADD;
    }
    else {
        ptrCAN_control = (KINETIS_CAN_CONTROL *)CAN0_BASE_ADD;
    }
    #else
    ptrCAN_control = (KINETIS_CAN_CONTROL *)CAN0_BASE_ADD;
    #endif
    while (ptrCAN_control->CAN_IFLAG1 != 0) {                            // while message buffers flagged
        if ((ptrCAN_control->CAN_IFLAG1 & ulBufferFlag) != 0) {          // this buffer is signalling a successful transmission or reception
            ptrCanQue = &can_queue[iChannel][iBufferID];
            can_int_message[MSG_DESTINATION_TASK] = ptrCanQue->TaskToWake; // the task that should be woken when this buffer is ready
            if (ptrCanQue->ucMode & CAN_TX_BUF) {                        // determine whether it is an rx or tx buffer
                if (CAN_RX_REMOTE_RX & ptrCanQue->ucMode) {              // this buffer has just transmitted a remote frame request
                    if (CAN_TX_BUF_ACTIVE & ptrCanQue->ucMode) {         // interrupt due to transmission
                        ptrCanQue->ucMode &= ~CAN_TX_BUF_ACTIVE;
                        iNoWake = 1;
                    }
                    else {                                               // a response has been detected
                        can_int_message[MSG_INTERRUPT_EVENT] = CAN_RX_REMOTE_MSG; // rx message received
                        ptrCanQue->ucMode = (CAN_TX_BUF | CAN_RX_REMOTE_RX | CAN_RX_BUF_FULL); // indicate message waiting
                    }
                }
                else if ((CAN_TX_BUF_REMOTE & ptrCanQue->ucMode) != 0) {
                    if (ptrCanQue->ucMode & CAN_TX_ACK_ON) {
                        can_int_message[MSG_INTERRUPT_EVENT] = CAN_TX_REMOTE_ACK;// remote frame transmission message successfully acked
                    }
                    else {
                        iNoWake = 1;
                    }
                    ptrCanQue->ucMode &= ~CAN_TX_ACK_ON;                 // we only ack once
                }
                else {
                    if ((CAN_TX_ACK_ON & ptrCanQue->ucMode) != 0) {
                        can_int_message[MSG_INTERRUPT_EVENT] = CAN_TX_ACK; // tx message successfully acked
                    }
                    else {
                        iNoWake = 1;                                     // no acknowledgement required
                    }
                    ptrCanQue->ucMode = (CAN_TX_BUF | CAN_TX_BUF_FREE);  // indicate successful transmission, buffer free for further use
                }
            }
            else {                                                       // reception
                KINETIS_CAN_BUF *ptrMessageBuffer;
    #if NUMBER_OF_CAN_INTERFACES > 1
                if (iChannel != 0) {
                    ptrMessageBuffer = MBUFF0_ADD_1;
                }
                else {
                    ptrMessageBuffer = MBUFF0_ADD_0;
                }
    #else
                ptrMessageBuffer = MBUFF0_ADD_0;
    #endif
                ptrMessageBuffer += iBufferID;
                ptrMessageBuffer->ulCode_Len_TimeStamp &= ~CAN_RX_CODE_FIELD; // deactivate buffer so that it doesn't get overwritten

                can_int_message[MSG_INTERRUPT_EVENT] = CAN_RX_MSG;       // rx message received
                ptrCanQue->ucMode |= CAN_RX_BUF_FULL;                    // indicate message waiting
            }
    #if defined _WINDOWS
            ptrCAN_control->CAN_IFLAG1 &= ~(ulBufferFlag);
    #else
            ptrCAN_control->CAN_IFLAG1 = ulBufferFlag;                   // clear interrupt flag
    #endif
            if (iNoWake == 0) {
                uDisable_Interrupt();                                    // ensure interrupts remain blocked when putting message to queue
                    fnWrite(INTERNAL_ROUTE, (unsigned char *)can_int_message, HEADER_LENGTH); // inform the handling task of event
                uEnable_Interrupt();
            }
        }
        ulBufferFlag <<= 1;
        if (++iBufferID >= NUMBER_CAN_MESSAGE_BUFFERS) {                 // last possible buffer processed so start at beginning again
            ulBufferFlag = 0x00000001;
            iBufferID = 0;
        }
    }
#endif
}

static void fnCAN_error(int iChannel)
{
#if defined MSCAN_CAN_INTERFACE
    // To do...
    //
#else
    unsigned char can_error_int_message[HEADER_LENGTH];
    KINETIS_CAN_CONTROL *ptrCAN_control;
    KINETIS_CAN_BUF *ptrMessageBuffer;
    volatile unsigned long ulError;                                      // read the error status register, clearing error bits

    can_error_int_message[MSG_DESTINATION_NODE] = INTERNAL_ROUTE;
    can_error_int_message[MSG_SOURCE_NODE]      = INTERNAL_ROUTE;
    can_error_int_message[MSG_DESTINATION_TASK] = CAN_ERROR_TASK;
    can_error_int_message[MSG_SOURCE_TASK]      = INTERRUPT_EVENT;

    #if NUMBER_OF_CAN_INTERFACES > 1
    if (iChannel != 0) {
        ptrCAN_control = (KINETIS_CAN_CONTROL *)CAN1_BASE_ADD;
        ptrMessageBuffer = MBUFF0_ADD_1;
    }
    else {
        ptrCAN_control = (KINETIS_CAN_CONTROL *)CAN0_BASE_ADD;
        ptrMessageBuffer = MBUFF0_ADD_0;
    }
    #else
    ptrCAN_control = (KINETIS_CAN_CONTROL *)CAN0_BASE_ADD;
    ptrMessageBuffer = MBUFF0_ADD_0;
    #endif

    while (((ulError = ptrCAN_control->CAN_ESR1) & (BIT1ERROR | BIT0ERROR | CAN_CRC_ERR | CAN_ACK_ERR | CAN_FRM_ERR | CAN_STF_ERR)) != 0) { // read the error status register, clearing error bits
    #if defined _WINDOWS
        ptrCAN_control->CAN_ESR1 &= ~ (BIT1ERROR | BIT0ERROR | CAN_CRC_ERR | CAN_ACK_ERR | CAN_FRM_ERR | CAN_STF_ERR);
    #endif
        if ((ulError & CAN_ACK_ERR) != 0) {                              // a transmission received no ack
            CANQUE *ptrCanQue = can_queue[iChannel];                     // we need to search for buffers which are transmitting
            int i = NUMBER_CAN_MESSAGE_BUFFERS;
            can_error_int_message[MSG_INTERRUPT_EVENT] = 0;
            while (i-- != 0) {
                if ((ptrCanQue->ucMode & CAN_TX_BUF_ACTIVE) != 0) {      // this buffer is presently transmitting a message
                    if (++(ptrCanQue->ucErrors) >= MAX_TX_CAN_TRIES) {   // we allow a few attempts before quitting (it also filters counting normal active buffers)
                        ptrMessageBuffer->ulCode_Len_TimeStamp = ((ptrMessageBuffer->ulCode_Len_TimeStamp & ~CAN_CODE_FIELD) | MB_TX_INACTIVE); // stop transmission
                        can_error_int_message[MSG_DESTINATION_TASK] = ptrCanQue->TaskToWake;
                        if ((ptrCanQue->ucMode & CAN_TX_BUF_REMOTE) != 0) {
                            can_error_int_message[MSG_INTERRUPT_EVENT] = CAN_TX_REMOTE_ERROR;
                            ptrCanQue->ucMode = (CAN_TX_BUF | CAN_RX_BUF_FULL | CAN_TX_BUF_REMOTE); // mark that it is an errored transmission buffer
                        }
                        else {
                            can_error_int_message[MSG_INTERRUPT_EVENT] = CAN_TX_ERROR;
                            ptrCanQue->ucMode = (CAN_TX_BUF | CAN_RX_BUF_FULL); // mark that it is an errored transmission buffer
                        }
                        break;
                    }
                }
                ptrCanQue++;
                ptrMessageBuffer++;
            }
        }
        else {
            ptrMessageBuffer->ulCode_Len_TimeStamp = (MB_TX_INACTIVE | (ptrMessageBuffer->ulCode_Len_TimeStamp & CAN_KEEP_CONTENTS)); // free the buffer
            can_error_int_message[MSG_INTERRUPT_EVENT] = CAN_OTHER_ERROR;
        }
        ptrCAN_control->CAN_ESR1 = ERRINT;                               // clear the error interrupt
    #if defined _WINDOWS
        ptrCAN_control->CAN_ESR1 &= ~(ERRINT);
    #endif
        if (can_error_int_message[MSG_INTERRUPT_EVENT] != 0) {
            uDisable_Interrupt();                                        // ensure interrupts remain blocked when putting message to queue
                fnWrite(INTERNAL_ROUTE, can_error_int_message, HEADER_LENGTH); // inform the handling task of event
            uEnable_Interrupt();
        }
    }
#endif
}

#if defined MSCAN_CAN_INTERFACE
// To do...
//
#else
static __interrupt void _CAN0_Message_Interrupt(void)
{
    fnCAN_Message(0);
}
static __interrupt void _CAN0_BusOff_Interrupt(void)
{
    CAN0_ESR1 = BOFFINT;                                                 // clear the interrupt
}
static __interrupt void _CAN0_Error_Interrupt(void)
{
    fnCAN_error(0);
}
static __interrupt void _CAN0_Tx_Interrupt(void)
{
}
static __interrupt void _CAN0_Rx_Interrupt(void)
{
}
static __interrupt void _CAN0_Wakeup_Interrupt(void)
{
}
    #if !defined KINETIS_K64
static __interrupt void _CAN0_IMEU_Interrupt(void)
{
}
    #endif


    #if NUMBER_OF_CAN_INTERFACES > 1
// Interrupt when a message has been successfully transmitted or received from/into any buffer
//
static __interrupt void _CAN1_Message_Interrupt(void)
{
    fnCAN_Message(1);
}
static __interrupt void _CAN1_BusOff_Interrupt(void)
{
    CAN1_ESR1 = BOFFINT;                                                 // clear the interrupt
}
static __interrupt void _CAN1_Error_Interrupt(void)
{
    fnCAN_error(1);
}
static __interrupt void _CAN1_Tx_Interrupt(void)
{
}
static __interrupt void _CAN1_Rx_Interrupt(void)
{
}
static __interrupt void _CAN1_Wakeup_Interrupt(void)
{
}
        #if defined irq_CAN0_IMEU_ID
static __interrupt void _CAN1_IMEU_Interrupt(void)
{
}
        #endif
    #endif
#endif

// The best choice of clock input is from the external crystal (lowest jitter), however this may not always enable the best settings to achieve the required speed.
// The choice of clock source is user-defined but this routine tries to achieve best settings using highest time quanta resolution. 
//
// There are up to 25 time quanta in a CAN bit time and the bit frequency is equal to the clock frequency divided by the quanta number (8..25 time quanta range)
// There is always a single time quanta at the start of a bit period called the SYNC_SEG which cannot be changed (transitions are expected to occur on the bus during this period)
// The sampling occurs after time segment 1, which is made up of a propagation segment (1..8 time quanta) plus a phase buffer segment 1 (1..8 time quanta),
// followed by time segment 2, made up of phase buffer segment 2 (2..8 time quanta)
//
// CAN standard compliant bit segment settings give the following ranges (re-synchronisation jump width of 2 is used since it is compliant with all)
// Time segment 1 should be 5..10 when time segment 2 is 2 (min/max time quanta per bit is 8/13)
// Time segment 1 should be 4..11 when time segment 2 is 3 (min/max time quanta per bit is 8/15)
// Time segment 1 should be 5..12 when time segment 2 is 4 (min/max time quanta per bit is 10/17)
// Time segment 1 should be 6..13 when time segment 2 is 5 (min/max time quanta per bit is 12/19)
// Time segment 1 should be 7..14 when time segment 2 is 6 (min/max time quanta per bit is 14/21)
// Time segment 1 should be 8..15 when time segment 2 is 7 (min/max time quanta per bit is 16/23)
// Time segment 1 should be 9..16 when time segment 2 is 8 (min/max time quanta per bit is 18/25)
//
static unsigned long fnOptimalCAN_clock(unsigned short usMode, unsigned long ulSpeed)
{
    unsigned long ulClockSourceFlag = EXTAL_CLK_SOURCE;
    unsigned long ulClockSpeed;
    unsigned long ulLowestError = 0xffffffff;
    unsigned long ulCanSpeed;
    unsigned long ulError;
    unsigned long ulPrescaler;
    int iTimeQuanta = 25;                                                // highest value for highest control resolution
    int iBestTimeQuanta = 25;
    unsigned long ulBestPrescaler = 0;
    if ((CAN_USER_SETTINGS & usMode) != 0) {
        return ulSpeed;                                                  // the user is passing optimal configuration settings directly
    }
    if ((CAN_PLL_CLOCK & usMode) != 0) {                                 // use the bus clock rather than crystal input directly
        ulClockSpeed = (BUS_CLOCK);                                      // the clock speed for calculation use is the bus speed
        ulClockSourceFlag = CLK_SRC_PERIPH_CLK;                          // peripheral clock will be used as clock source
    }
    else {
        ulClockSpeed = _EXTERNAL_CLOCK;                                  // the clock speed for calculation use is the external/crystal clock speed
        // If the oscillator clock is used its frequency must be lower than the bus clock
        //
    #if defined _WINDOWS
        if (ulClockSpeed > BUS_CLOCK) {
            _EXCEPTION("Oscillator clock must be lower than the bus clock!!");
        }
    #endif
    }
    while (iTimeQuanta >= 8) {                                           // test for best time quanta
        ulCanSpeed = (ulClockSpeed/iTimeQuanta);                         // speed without prescaler
        ulPrescaler = ((ulCanSpeed + (ulSpeed/2))/ulSpeed);              // best prescale value
        if (ulPrescaler > 256) {
            ulPrescaler = 256;                                           // maximum possible prescale divider
        }
        else if (ulPrescaler == 0) {
            ulPrescaler = 1;
        }
        ulCanSpeed /= ulPrescaler;
        if (ulCanSpeed >= ulSpeed) {                                     // determine the absolute error value with this quanta setting
            ulError = (ulCanSpeed - ulSpeed);
        }
        else {
            ulError = (ulSpeed - ulCanSpeed);
        }
        if (ulError < ulLowestError) {                                   // if this is an improvement
            ulLowestError = ulError;
            iBestTimeQuanta = iTimeQuanta;                               // best time quanta value
            ulBestPrescaler = ulPrescaler;
        }
        iTimeQuanta--;
    }
    ulBestPrescaler--;                                                   // convert to register setting value
    ulBestPrescaler <<= 24;                                              // move the prescale value into position
    if (iBestTimeQuanta >= 18) {                                         // determine the phase buffer length value
        ulBestPrescaler |= PHASE_BUF_SEG2_LEN8;
        iBestTimeQuanta -= (8 + 1);                                      // remaining time quanta (time segment 1) after removal of the time segment 2 and the SYN_SEG
    }
    else if (iBestTimeQuanta >= 16) {
        ulBestPrescaler |= PHASE_BUF_SEG2_LEN7;
        iBestTimeQuanta -= (7 + 1);
    }
    else if (iBestTimeQuanta >= 14) {
        ulBestPrescaler |= PHASE_BUF_SEG2_LEN6;
        iBestTimeQuanta -= (6 + 1);
    }
    else if (iBestTimeQuanta >= 12) {
        ulBestPrescaler |= PHASE_BUF_SEG2_LEN5;
        iBestTimeQuanta -= (5 + 1);
    }
    else if (iBestTimeQuanta >= 10) {
        ulBestPrescaler |= PHASE_BUF_SEG2_LEN4;
        iBestTimeQuanta -= (4 + 1);
    }
    else {
        ulBestPrescaler |= PHASE_BUF_SEG2_LEN3;
        iBestTimeQuanta -= (3 + 1);
    }
    if ((iBestTimeQuanta & 0x1) != 0) {                                  // odd
        iBestTimeQuanta /= 2;                                            // PROP_SEG and PSEG1 to achieve time segment 1
        ulBestPrescaler |= iBestTimeQuanta;                              // set propogation bit time (1 more than phase buffer segment 1)
        iBestTimeQuanta--;
        ulBestPrescaler |= (iBestTimeQuanta << 19);                      // set phase buffer segment 1
    }
    else {                                                               // even
        iBestTimeQuanta /= 2;                                            // PROP_SEG and PSEG1 to achieve time segment 1 and phase buffer segment 1
        iBestTimeQuanta--;
        ulBestPrescaler |= ((iBestTimeQuanta << 19) | (iBestTimeQuanta));// set equal propogation bit times
    }
    return (RJW_2 | ulClockSourceFlag | ulBestPrescaler);                // initialise the CAN controller with the required speed and parameters
}

// Hardware initialisation of CAN controller
//
extern void fnInitCAN(CANTABLE *pars)
{
#if defined MSCAN_CAN_INTERFACE
    // To do...
    //
#else
    unsigned long ulCanCtrValue;
    KINETIS_CAN_BUF *ptrMessageBuffer;
    KINETIS_CAN_CONTROL *ptrCAN_control;

    #if defined ERRATA_ID_2583                                           // in early silicon the CAN controllers only work when the OSC is enabled
    if ((OSC0_CR & OSC_CR_ERCLKEN) == 0) {
        OSC0_CR = OSC_CR_ERCLKEN;                _SIM_PER_CHANGE;        // enable the external reference clock source whether the bus clock or external clock is to be used
    }
    #else
    if ((CAN_PLL_CLOCK & pars->usMode) == 0) {                           // if the external clock is to be used enable it
        if ((OSC0_CR & OSC_CR_ERCLKEN) == 0) {                           // if the clock has not been enabled
            OSC0_CR = OSC_CR_ERCLKEN;            _SIM_PER_CHANGE;        // enable the external reference clock source for CAN use
        }
    }
    #endif
    #if NUMBER_OF_CAN_INTERFACES > 1
    if (pars->Channel != 0) {                                            // enable clock to the specific module
        POWER_UP(3, SIM_SCGC3_FLEXCAN1);
        ptrCAN_control = (KINETIS_CAN_CONTROL *)CAN1_BASE_ADD;
        ptrMessageBuffer = MBUFF0_ADD_1;                                 // the first of 16 message buffers in the FlexCan module
        #if defined CAN1_ON_PE
        _CONFIG_PERIPHERAL(E, 24, PE_24_CAN1_TX);                        // CAN1_TX on PE24 (alt. function 2)
        _CONFIG_PERIPHERAL(E, 25, PE_25_CAN1_RX);                        // CAN1_RX on PE25 (alt. function 2)
        #else
        _CONFIG_PERIPHERAL(C, 17, PC_17_CAN1_TX);                        // CAN1_TX on PC17 (alt. function 2)
        _CONFIG_PERIPHERAL(C ,16, PC_16_CAN1_RX);                        // CAN1_RX on PC16 (alt. function 2)
        #endif
    }
    else {
    #endif
        POWER_UP(6, SIM_SCGC6_FLEXCAN0);
        ptrCAN_control = (KINETIS_CAN_CONTROL *)CAN0_BASE_ADD;
        ptrMessageBuffer = MBUFF0_ADD_0;                                 // the first of 16 message buffers in the FlexCan module
    #if defined CAN0_ON_PB
        _CONFIG_PERIPHERAL(B, 18, PB_18_CAN0_TX);                        // CAN0_TX on PB18 (alt. function 2)
        _CONFIG_PERIPHERAL(B, 19, PB_19_CAN0_RX);                        // CAN0_RX on PB19 (alt. function 2)
    #else
        _CONFIG_PERIPHERAL(A, 12, PA_12_CAN0_TX);                        // CAN0_TX on PA12 (alt. function 2)
        _CONFIG_PERIPHERAL(A, 13, PA_13_CAN0_RX);                        // CAN0_RX on PA13 (alt. function 2)
    #endif
    #if NUMBER_OF_CAN_INTERFACES > 1
    }
    #endif

    uMemset(ptrMessageBuffer, 0x00, (sizeof(KINETIS_CAN_BUF) * NUMBER_CAN_MESSAGE_BUFFERS)); // the buffers are not reset so clear here
                                                                         // calculate the settings for the required speed
    ulCanCtrValue = fnOptimalCAN_clock(pars->usMode, pars->ulSpeed);
    ptrCAN_control->CAN_CTRL1 = (CLK_SRC_PERIPH_CLK & ulCanCtrValue);    // {4}{3}{65} select the clock source while the module is in disable mode
                                                                         // note that the crystal can be used and has better jitter performance than the PLL
    ptrCAN_control->CAN_MCR &= ~CAN_MDIS;                                // move from disabled to freeze mode (un-synchronised to the CAN bus)
    while ((ptrCAN_control->CAN_MCR & CAN_LPMACK) != 0) {                // wait for CAN controller to leave disabled mode
    #if defined _WINDOWS
        ptrCAN_control->CAN_MCR &= ~(CAN_LPMACK);
    #endif
    }
    ptrCAN_control->CAN_CTRL1 = ulCanCtrValue;                           // {4} write the clock setting

    if ((pars->usMode & CAN_LOOPBACK) != 0) {
        ptrCAN_control->CAN_CTRL1 |= LPB;                                // set loopback mode
    }

    #if NUMBER_OF_CAN_INTERFACES > 1
    if (pars->Channel != 0) {
        fnEnterInterrupt(irq_CAN1_MESSAGE_ID, PRIORITY_CAN1_MESSAGE, _CAN1_Message_Interrupt); // enter all CAN interrupts
        fnEnterInterrupt(irq_CAN1_BUS_OFF_ID, PRIORITY_CAN1_BUS_OFF, _CAN1_BusOff_Interrupt);
        fnEnterInterrupt(irq_CAN1_ERROR_ID, PRIORITY_CAN1_ERROR, _CAN1_Error_Interrupt);
        fnEnterInterrupt(irq_CAN1_TX_ID, PRIORITY_CAN1_TX, _CAN1_Tx_Interrupt);
        fnEnterInterrupt(irq_CAN1_RX_ID, PRIORITY_CAN1_RX, _CAN1_Rx_Interrupt);
        fnEnterInterrupt(irq_CAN1_WAKE_UP_ID, PRIORITY_CAN1_WAKEUP, _CAN1_Wakeup_Interrupt);
        #if defined irq_CAN0_IMEU_ID
        fnEnterInterrupt(irq_CAN1_IMEU_ID, PRIORITY_CAN1_IMEU, _CAN1_IMEU_Interrupt);
        #endif
    }
    else {
    #endif
        fnEnterInterrupt(irq_CAN0_MESSAGE_ID, PRIORITY_CAN0_MESSAGE, _CAN0_Message_Interrupt); // enter all CAN interrupts
        fnEnterInterrupt(irq_CAN0_BUS_OFF_ID, PRIORITY_CAN0_BUS_OFF, _CAN0_BusOff_Interrupt);
        fnEnterInterrupt(irq_CAN0_ERROR_ID, PRIORITY_CAN0_ERROR, _CAN0_Error_Interrupt);
        fnEnterInterrupt(irq_CAN0_TX_ID, PRIORITY_CAN0_TX, _CAN0_Tx_Interrupt);
        fnEnterInterrupt(irq_CAN0_RX_ID, PRIORITY_CAN0_RX, _CAN0_Rx_Interrupt);
        fnEnterInterrupt(irq_CAN0_WAKE_UP_ID, PRIORITY_CAN0_WAKEUP, _CAN0_Wakeup_Interrupt);
    #if defined irq_CAN0_IMEU_ID
        fnEnterInterrupt(irq_CAN0_IMEU_ID, PRIORITY_CAN0_IMEU, _CAN0_IMEU_Interrupt);
    #endif
    #if NUMBER_OF_CAN_INTERFACES > 1
    }
    #endif

    ptrCAN_control->CAN_IFLAG1 = (CAN_ALL_BUFFERS_INT);                  // ensure no pending interrupts
    ptrCAN_control->CAN_IMASK1 = (CAN_ALL_BUFFERS_INT);                  // enable buffer interrupts on all message boxes

    ptrCAN_control->CAN_ESR1 = (ERRINT | BOFFINT);                       // ensure no pending error interrupts 
    ptrCAN_control->CAN_CTRL1 |= (ERRMSK | BOFFMSK);
   
    ptrCAN_control->CAN_MCR &= ~(CAN_FRZ | CAN_HALT);                    // leave freeze mode and start synchronisation

    #if defined _WINDOWS
    ptrCAN_control->CAN_IFLAG1 = 0;
    ptrCAN_control->CAN_ESR1 = 0;
    fnSimCAN(pars->Channel, 0, CAN_SIM_INITIALISE);
    #endif
#endif
}

// Hardware configuration of CAN controller
//
extern void fnConfigCAN(QUEUE_HANDLE DriverID, CANTABLE *pars)
{
#if defined MSCAN_CAN_INTERFACE
    #if defined MSCAN_ON_PORT_C
    SIM_PINSEL1 &= ~(SIM_PINSEL1_MSCANPS);
    _CONFIG_PERIPHERAL(C, 7, PC_7_CAN0_TX);                              // MSCAN TX on PC7 (alt. function 5)
    _CONFIG_PERIPHERAL(C, 6, PC_6_CAN0_RX);                              // MSCAN RX on PC6 (alt. function 5)
    #else
    SIM_PINSEL1 |= SIM_PINSEL1_MSCANPS;                                  // select alternate pins
    _CONFIG_PERIPHERAL(E, 7, PE_7_CAN0_TX);                              // MSCAN TX on PE7 (alt. function 5)
    _CONFIG_PERIPHERAL(H, 2, PH_2_CAN0_RX);                              // MSCAN RX on PH2 (alt. function 5)
    #endif
    MSCAN_CANCTL1 = MSCAN_CANCTL1_CANE;                                  // enable operation
                                                                         _SIM_PER_CHANGE;
#else
    int i;
    unsigned char ucTxCnt = pars->ucTxBuffers;
    unsigned char ucRxCnt = pars->ucRxBuffers;
    KINETIS_CAN_BUF *ptrMessageBuffer;
    KINETIS_CAN_BUF *ptrFirstAllocated;
    KINETIS_CAN_BUF *ptrSecondAllocated;
    KINETIS_CAN_CONTROL *ptrCAN_control;
    CANQUE *ptrCanQue;

    #if NUMBER_OF_CAN_INTERFACES > 1
    switch (pars->Channel) {                                             // the CAN controller to use
    case 0:
        ptrCanQue = can_queue[0];
        ptrCAN_control = (KINETIS_CAN_CONTROL *)CAN0_BASE_ADD;
        ptrMessageBuffer = MBUFF0_ADD_0;                                 // the first of 16 message buffers in the FlexCan module
        break;
    case 1:
        ptrCanQue = can_queue[1];
        ptrCAN_control = (KINETIS_CAN_CONTROL *)CAN1_BASE_ADD;
        ptrMessageBuffer = MBUFF0_ADD_1;                                 // the first of 16 message buffers in the FlexCan module
        break;
    default:
        return;
    }
    #else
    ptrCanQue = can_queue[0];
    ptrCAN_control = (KINETIS_CAN_CONTROL *)CAN0_BASE_ADD;
    ptrMessageBuffer = MBUFF0_ADD_0;                                     // the first of 16 message buffers in the FlexCan module
    #endif

    ptrCAN_control->CAN_MCR |= (CAN_FRZ | CAN_HALT);                     // move to freeze mode

    for (i = 0; i < NUMBER_CAN_MESSAGE_BUFFERS; i++) {                   // initialise the requested number of transmit buffers
        if (ucTxCnt == 0) {
            break;
        }
        if (ptrCanQue->DriverID == 0) {                                  // not yet allocated
            ptrCanQue->DriverID = DriverID;
            ptrCanQue->TaskToWake = pars->Task_to_wake;
            ptrCanQue->ucMode = (CAN_TX_BUF | CAN_TX_BUF_FREE);
            if ((pars->ulTxID & CAN_EXTENDED_ID) != 0) {
                ptrCanQue->ulPartnerID = pars->ulTxID;                   // enter default destination ID
                ptrMessageBuffer->ulCode_Len_TimeStamp = (MB_TX_INACTIVE | IDE); // enable extended ID transmission
            }
            else {
                ptrCanQue->ulPartnerID = ((pars->ulTxID << CAN_STANDARD_SHIFT) & CAN_STANDARD_BITMASK); // enter reception ID for the buffer
                ptrMessageBuffer->ulCode_Len_TimeStamp = MB_TX_INACTIVE;
            }
            if ((pars->ulRxID & CAN_EXTENDED_ID) != 0) {
                ptrCanQue->ulOwnID = pars->ulRxID;
            }
            else {
                ptrCanQue->ulOwnID = ((pars->ulRxID << CAN_STANDARD_SHIFT) & CAN_STANDARD_BITMASK);
            }
            ucTxCnt--;
        }
        ptrMessageBuffer++;
        ptrCanQue++;
    }
    #if NUMBER_OF_CAN_INTERFACES > 1
    if (pars->Channel != 0) {
        ptrMessageBuffer = MBUFF15_ADD_1;
        ptrFirstAllocated = MBUFF15_ADD_1;
        ptrSecondAllocated = MBUFF14_ADD_1;
    }
    else {
    #endif
        ptrMessageBuffer = MBUFF15_ADD_0;
        ptrFirstAllocated = MBUFF15_ADD_0;
        ptrSecondAllocated = MBUFF14_ADD_0;
    #if NUMBER_OF_CAN_INTERFACES > 1
    }
    #endif
    ptrCanQue = &can_queue[pars->Channel][NUMBER_CAN_MESSAGE_BUFFERS - 1];
    for (i = 0; i < NUMBER_CAN_MESSAGE_BUFFERS; i++) {                   // initialise the requested number of receive buffers
        if (ucRxCnt == 0) {
            break;
        }
        if (0 == ptrCanQue->DriverID) {                                  // not yet allocated
            ucRxCnt--;
            ptrCanQue->DriverID = DriverID;
            ptrCanQue->TaskToWake = pars->Task_to_wake;
            if ((pars->ulRxID & CAN_EXTENDED_ID) != 0) {
                if (ptrFirstAllocated == ptrMessageBuffer) {
                    ptrCAN_control->CAN_RX15MASK = pars->ulRxIDMask;     // first allocated receiver buffer has special mask
                }
                else if (ptrSecondAllocated == ptrMessageBuffer) {
                    ptrCAN_control->CAN_RX14MASK = pars->ulRxIDMask;     // second allocated receiver buffer has special mask
                }
                else {
                    ptrCAN_control->CAN_RXGMASK = pars->ulRxIDMask;      // initialise general acceptance mask for use with subsequent buffers
                }
                ptrMessageBuffer->ulID = (pars->ulRxID & CAN_EXTENDED_MASK); // enter reception ID for the buffer
                ptrMessageBuffer->ulCode_Len_TimeStamp = (MB_RX_EMPTY | IDE); // enable extended ID reception
            }
            else {                                                       // standard ID
                unsigned long ulMask = ((pars->ulRxIDMask << CAN_STANDARD_SHIFT) & CAN_STANDARD_BITMASK);
                if (ptrFirstAllocated == ptrMessageBuffer) {
                    ptrCAN_control->CAN_RX15MASK = ulMask;               // first allocated receiver buffer has special mask
                }
                else if (ptrSecondAllocated == ptrMessageBuffer) {
                    ptrCAN_control->CAN_RX14MASK = ulMask;               // second allocated receiver buffer has special mask
                }
                else {
                    ptrCAN_control->CAN_RXGMASK = ulMask;                // initialise general acceptance mask for use with subsequent buffers
                }
                ptrMessageBuffer->ulID = ((pars->ulRxID << CAN_STANDARD_SHIFT) & CAN_STANDARD_BITMASK); // enter reception ID for the buffer
                ptrMessageBuffer->ulCode_Len_TimeStamp = MB_RX_EMPTY;    // enable reception
            }
        }
        ptrMessageBuffer--;
        ptrCanQue--;
    }
    ptrCAN_control->CAN_MCR &= ~(CAN_FRZ | CAN_HALT);                    // leave freeze mode and start synchronisation
#endif
}

// The CAN driver has called this to send a message
//
extern unsigned char fnCAN_tx(QUEUE_HANDLE Channel, QUEUE_HANDLE DriverID, unsigned char *ptBuffer, QUEUE_TRANSFER Counter)
{
#if defined MSCAN_CAN_INTERFACE
    // To do...
    //
#else
    int i = 0;
    KINETIS_CAN_BUF *ptrMessageBuffer;
    unsigned char ucTxMode = (unsigned char)Counter & CAN_TX_MSG_MASK;
    CANQUE *ptrCanQue;
    unsigned char ucRtnCnt; 
    unsigned long ulExtendedID = 0;

    #if NUMBER_OF_CAN_INTERFACES > 1
    if (Channel != 0) {
        ptrMessageBuffer = MBUFF0_ADD_1;                                 // the first of 16 message buffers in the FlexCan module
        ptrCanQue = can_queue[1];
    }
    else {
        ptrMessageBuffer = MBUFF0_ADD_0;                                 // the first of 16 message buffers in the FlexCan module
        ptrCanQue = can_queue[0];
    }
    #else
    ptrMessageBuffer = MBUFF0_ADD_0;                                     // the first of 16 message buffers in the FlexCan module
    ptrCanQue = can_queue[0];
    #endif

    Counter &= ~CAN_TX_MSG_MASK;
    ucRtnCnt = (unsigned char)Counter;

    if (ucTxMode & (TX_REMOTE_FRAME | TX_REMOTE_STOP)) {                 // only one remote transmit buffer allowed
        for (; i < NUMBER_CAN_MESSAGE_BUFFERS; i++) { 
            if (ptrCanQue->DriverID == DriverID) {                       // find a buffer belonging to us
                if (ptrCanQue->ucMode & CAN_TX_BUF_REMOTE) {             // active remote buffer found
                    if (ucTxMode & TX_REMOTE_STOP) {
                        ptrMessageBuffer->ulCode_Len_TimeStamp = MB_TX_INACTIVE; // disable and free buffer
                        ptrMessageBuffer->ulID = (ptrCanQue->ulPartnerID & ~CAN_EXTENDED_ID); // set tx message buffer to default id
                        ptrCanQue->ucMode = (CAN_TX_BUF | CAN_TX_BUF_FREE); // buffer remain inactive tx buffer
    #if defined _WINDOWS
                        fnSimCAN(Channel, i, CAN_SIM_FREE_BUFFER);
    #endif
                        return 1;                                        // remote frame transmission disabled
                    }
                    else {
                        ptrMessageBuffer->ulCode_Len_TimeStamp = MB_TX_INACTIVE; // disable so that changes can be made
                        ptrCanQue->ucMode = (CAN_TX_BUF | CAN_TX_BUF_FREE); // buffer remain inactive tx buffer
                    }
                    break;                                               // the remote buffer has been found
                }
            }
            ptrMessageBuffer++;
            ptrCanQue++;
        }
        if (i >= NUMBER_CAN_MESSAGE_BUFFERS) {                           // no remote buffer present
            i = 0;                                                       // reset ready for restart
    #if NUMBER_OF_CAN_INTERFACES > 1
            if (Channel != 0) {
                ptrCanQue = can_queue[1];
                ptrMessageBuffer = MBUFF0_ADD_1;
            }
            else {
                ptrCanQue = can_queue[0];
                ptrMessageBuffer = MBUFF0_ADD_0;
            }
    #else
            ptrCanQue = can_queue[0];
            ptrMessageBuffer = MBUFF0_ADD_0;
    #endif
        }
    }

    for (; i < NUMBER_CAN_MESSAGE_BUFFERS; i++) {
        if (ptrCanQue->DriverID == DriverID) {                           // find a buffer belonging to us
            if ((ptrCanQue->ucMode & CAN_TX_BUF_FREE) && (!(TX_REMOTE_STOP & ucTxMode))) {  // if the transmit buffer is free and not stopping transmission
                if (TX_REMOTE_FRAME & ucTxMode) {                        // depositing a message to be sent on a remote request
                    if (ptrCanQue->ulPartnerID & CAN_EXTENDED_ID) {
                        ulExtendedID = (IDE | MB_TX_SEND_ON_REQ | ((unsigned long)Counter << 16)); // use extended destination ID                           
                    }
                    else {
                        ulExtendedID = (MB_TX_SEND_ON_REQ | ((unsigned long)Counter << 16));
                    }
                    ptrMessageBuffer->ulID = (ptrCanQue->ulPartnerID & ~CAN_EXTENDED_ID); // set tx message buffer to default ID address
                    ptrCanQue->ucMode = (CAN_TX_BUF | CAN_TX_BUF_ACTIVE | CAN_TX_BUF_REMOTE | (CAN_TX_ACK_ON & ucTxMode)); // mark that the buffer is in use
                }
                else {
                    if (ucTxMode & SPECIFIED_ID) {                       // is the user specifying a destination ID or can default be used?
                        unsigned long ulPartnerID = 0;
                        ulPartnerID = *ptBuffer++;
                        ulPartnerID <<= 8;
                        ulPartnerID |= *ptBuffer++;
                        ulPartnerID <<= 8;
                        ulPartnerID |= *ptBuffer++;
                        ulPartnerID <<= 8;
                        ulPartnerID |= *ptBuffer++;
                        if (ulPartnerID & CAN_EXTENDED_ID) {
                            ulExtendedID = IDE;
                            ptrMessageBuffer->ulID = (ulPartnerID & ~CAN_EXTENDED_ID); // send to specified extended ID address
                        }
                        else {
                            ptrMessageBuffer->ulID = ((ulPartnerID << CAN_STANDARD_SHIFT) & CAN_STANDARD_BITMASK); // send to specified standard ID address
                        }
                        Counter -= 4;
                    }
                    else {                                               // transmission to default ID
                        if ((ptBuffer == 0) && (Counter == 0)) {         // remote frame request -  after transmission the buffer will become a receiver until read or freed
                            if (ptrCanQue->ulOwnID & CAN_EXTENDED_ID) {
                                ulExtendedID = (MB_TX_SEND_ONCE | RTR | IDE); // use extended destination ID
                            }
                            else {
                                ulExtendedID = (MB_TX_SEND_ONCE | RTR);
                            }
                            ptrMessageBuffer->ulID = (ptrCanQue->ulOwnID & ~CAN_EXTENDED_ID); // request from our default ID address
                            ptrCanQue->ucMode = (CAN_TX_BUF | CAN_RX_REMOTE_RX | CAN_TX_BUF_ACTIVE | CAN_TX_BUF_REMOTE);
                        }
                        else {
                            if (ptrCanQue->ulPartnerID & CAN_EXTENDED_ID) {
                                ulExtendedID = IDE;                          // use extended destination ID
                            }
                            ptrMessageBuffer->ulID = (ptrCanQue->ulPartnerID & ~CAN_EXTENDED_ID); // send to default ID address
                        }
                    }
                    if (Counter > 8) {                                   // limit the count to maximum
                        Counter = 8;
                    }
                    if (Counter != 0) {                                  // data so it is not a remote frame request   
                        ptrCanQue->ucMode = (CAN_TX_BUF | CAN_TX_BUF_ACTIVE | (CAN_TX_ACK_ON & ucTxMode)); // mark that the buffer is in use                    
                        ulExtendedID |= (MB_TX_SEND_ONCE | ((unsigned long)Counter << 16));
                    }
                }
                // The data needs to be stored as long words in big-endian format
                //
                if (ptBuffer != 0) {
                    ptrMessageBuffer->ulData[0] = ((*(ptBuffer) << 24) | (*(ptBuffer + 1) << 16) | (*(ptBuffer + 2) << 8) | (*(ptBuffer + 3)));
                    ptrMessageBuffer->ulData[1] = ((*(ptBuffer + 4) << 24) | (*(ptBuffer + 5) << 16) | (*(ptBuffer + 6) << 8) | (*(ptBuffer + 7)));
                }
                ptrCanQue->ucErrors = 0;
                ptrMessageBuffer->ulCode_Len_TimeStamp = ulExtendedID;   // start the process
    #if defined _WINDOWS
                fnSimCAN(Channel, i, 0);                                 // simulate the buffer operation
    #endif
                return (unsigned char)ucRtnCnt;
            }
        }
        ptrMessageBuffer++;
        ptrCanQue++;
    }
#endif
    return 0;                                                            // no free buffer was found...
}


// The CAN driver calls this to collect a received message
//
extern unsigned char fnCAN_get_rx(QUEUE_HANDLE Channel, QUEUE_HANDLE DriverID, unsigned char *ptBuffer, QUEUE_TRANSFER Counter)
{
#if defined MSCAN_CAN_INTERFACE
    // To do...
    //
#else
    int i;
    unsigned char ucLength;
    KINETIS_CAN_BUF *ptrMessageBuffer;
    CANQUE *ptrCanQue;
    KINETIS_CAN_CONTROL *ptrCAN_control;
    unsigned char ucCommand = (unsigned char)Counter;

    #if NUMBER_OF_CAN_INTERFACES > 1
    if (Channel != 0) {
        ptrCAN_control = (KINETIS_CAN_CONTROL *)CAN1_BASE_ADD;
        ptrMessageBuffer = MBUFF15_ADD_1;                                // the last of 16 message buffers in the FlexCan module
        ptrCanQue = &can_queue[1][NUMBER_CAN_MESSAGE_BUFFERS - 1];
    }
    else {
    #endif
        ptrCAN_control = (KINETIS_CAN_CONTROL *)CAN0_BASE_ADD;
        ptrMessageBuffer = MBUFF15_ADD_0;                                // the last of 16 message buffers in the FlexCan module
        ptrCanQue = &can_queue[0][NUMBER_CAN_MESSAGE_BUFFERS - 1];
    #if NUMBER_OF_CAN_INTERFACES > 1
    }
    #endif

    for (i = 0; i < NUMBER_CAN_MESSAGE_BUFFERS; i++) {                   // search through receive buffers
        if (ptrCanQue->DriverID == DriverID) {                           // find a buffer belonging to us
            if ((ucCommand & FREE_CAN_RX_REMOTE) && (ptrCanQue->ucMode & CAN_RX_REMOTE_RX)) { // release a waiting remote frame receiver buffer
                ptrMessageBuffer->ulCode_Len_TimeStamp = (MB_TX_INACTIVE | (ptrMessageBuffer->ulCode_Len_TimeStamp & CAN_KEEP_CONTENTS));
                ptrCanQue->ucMode = (CAN_TX_BUF | CAN_TX_BUF_FREE);      // buffer is now a normal tx buffer again
    #if defined _WINDOWS
                fnSimCAN(Channel, ((NUMBER_CAN_MESSAGE_BUFFERS - 1) - i), CAN_SIM_FREE_BUFFER);
    #endif
                return 1;
            }

            if (ptrCanQue->ucMode & CAN_RX_BUF_FULL) {                   // if the buffer contains data
                if ((!(ucCommand & (GET_CAN_TX_ERROR | GET_CAN_TX_REMOTE_ERROR | GET_CAN_RX_REMOTE | FREE_CAN_RX_REMOTE))) // normal rx buffer read
                     || ((ucCommand & GET_CAN_TX_REMOTE_ERROR) && (ptrCanQue->ucMode & CAN_TX_BUF_REMOTE)) // requesting remote error details
                     || ((ucCommand & GET_CAN_RX_REMOTE) && (ptrCanQue->ucMode & CAN_RX_REMOTE_RX)) // specifically requesting a remote reception buffer
                     || ((ucCommand & GET_CAN_TX_ERROR) && (ptrCanQue->ucMode & CAN_TX_BUF))) { // requesting errored tx buffer                    

                    if (!(ucCommand & (GET_CAN_TX_ERROR | GET_CAN_TX_REMOTE_ERROR))) { // reception
                        while (ptrMessageBuffer->ulCode_Len_TimeStamp & MB_RX_BUSY_BIT) {} // wait until buffer update has terminated

                        if ((ptrMessageBuffer->ulCode_Len_TimeStamp & CAN_RX_CODE_FIELD) == MB_RX_OVERRUN) { // the buffer is now locked
                            *ptBuffer = CAN_RX_OVERRUN;                  // receiver overrun - we have lost an intermediate message
                        }
                        else {
                            *ptBuffer = 0;
                        }

                        if (ptrCanQue->ucMode & CAN_RX_REMOTE_RX) {      // temporary rx buffer - so set back to tx
                            *ptBuffer++ |= CAN_REMOTE_MSG_RX; 
                            ptrMessageBuffer->ulCode_Len_TimeStamp = (MB_TX_INACTIVE | (ptrMessageBuffer->ulCode_Len_TimeStamp & CAN_LENGTH_AND_TIME));
                            ptrCanQue->ucMode = (CAN_TX_BUF | CAN_TX_BUF_FREE); // buffer is now a normal tx buffer again
                        }
                        else {                                                 
                            *ptBuffer++ |= CAN_MSG_RX;
                        }
                        if ((GET_CAN_RX_TIME_STAMP & ucCommand) != 0) {  // {70}
                            *ptBuffer++ = (unsigned char)(ptrMessageBuffer->ulCode_Len_TimeStamp >> 8); // write time stamp to input buffer
                            *ptBuffer++ = (unsigned char)(ptrMessageBuffer->ulCode_Len_TimeStamp);
                        }
                    }

                    if (ucCommand & (GET_CAN_TX_ERROR | GET_CAN_TX_REMOTE_ERROR | GET_CAN_RX_ID)) {
                        unsigned long ulID = ptrMessageBuffer->ulID;
                        if (ptrMessageBuffer->ulCode_Len_TimeStamp & IDE) {
                            ulID |= CAN_EXTENDED_ID;                     // mark that it is an extended address
                        }
                        else {
                            ulID >>= CAN_STANDARD_SHIFT;                 // the address if a standard address
                        }
                        *ptBuffer++ = (unsigned char)(ulID >> 24);       // write failed ID to buffer or rx ID
                        *ptBuffer++ = (unsigned char)(ulID >> 16); 
                        *ptBuffer++ = (unsigned char)(ulID >> 8);
                        *ptBuffer++ = (unsigned char)(ulID);
                    }

                    ucLength = (unsigned char)((ptrMessageBuffer->ulCode_Len_TimeStamp >> 16) & 0x0f);
                    // The data needs is stored as long words in big-endian format
                    //
                    if (ucLength != 0) {
                        *ptBuffer++ = (unsigned char)(ptrMessageBuffer->ulData[0] >> 24);
                        if (ucLength > 1) {
                            *ptBuffer++ = (unsigned char)(ptrMessageBuffer->ulData[0] >> 16);
                            if (ucLength > 2) {
                                *ptBuffer++ = (unsigned char)(ptrMessageBuffer->ulData[0] >> 8);
                                if (ucLength > 3) {
                                    *ptBuffer++ = (unsigned char)(ptrMessageBuffer->ulData[0]);
                                    if (ucLength > 4) {
                                        *ptBuffer++ = (unsigned char)(ptrMessageBuffer->ulData[1] >> 24);
                                        if (ucLength > 5) {
                                            *ptBuffer++ = (unsigned char)(ptrMessageBuffer->ulData[1] >> 16);
                                            if (ucLength > 6) {
                                                *ptBuffer++ = (unsigned char)(ptrMessageBuffer->ulData[1] >> 8);
                                                if (ucLength > 7) {
                                                    *ptBuffer++ = (unsigned char)(ptrMessageBuffer->ulData[1]);
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }

                    if (ucCommand & (GET_CAN_TX_ERROR | GET_CAN_TX_REMOTE_ERROR)) {
                        ptrCanQue->ucMode = (CAN_TX_BUF | CAN_TX_BUF_FREE); // the buffer may be used again since the lost data has been rescued
                    }
                    else {
                        if (GET_CAN_RX_TIME_STAMP & ucCommand) {         // {70}
                            ucLength += 3;                               // status and time stamp
                        }
                        else {
                            ucLength++;                                  // status only
                        }
                        if ((ptrCanQue->ucMode & CAN_TX_BUF_FREE) == 0) {// if not a remote reception read
                            ptrCanQue->ucMode &= (~CAN_RX_BUF_FULL);
                            ptrMessageBuffer->ulCode_Len_TimeStamp = (MB_RX_EMPTY | (ptrMessageBuffer->ulCode_Len_TimeStamp & ~CAN_CODE_FIELD));// free up buffer for further use
                        }
                    }
                    if (ucCommand & (GET_CAN_TX_ERROR | GET_CAN_TX_REMOTE_ERROR | GET_CAN_RX_ID)) {
                        ucLength += 4;                                   // id length
                    }
                    ptrCAN_control->CAN_TIMER;                           // read the free running timer to release internal lock
                    return ucLength;                                     // the number of bytes copied, including OK byte and timestamp
                }
            }
        }
        ptrMessageBuffer--;
        ptrCanQue--;
    }
#endif
    return 0;                                                            // no data was found...
}

    #if defined _WINDOWS
extern UTASK_TASK fnGetCANOwner(int iChannel, int i)
{
    return (can_queue[iChannel][i].TaskToWake);
}
    #endif

