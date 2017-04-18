/***********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher

    ---------------------------------------------------------------------
    File:      LCD.c
    Project:   uTasker project
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2016
    *********************************************************************
    09.10.2007 Add configurable delay to ensure two back to back writes are not too fast in 4 bit mode {1}
    10.10.2007 Correct nibble adjustment in 4-bit read                   {2}
    12.10.2007 T_INIT_WAIT modified to 5ms rather than 1 tick Ticks of greater will default to 1 tick) {3}
    14.04.2007 Add optional command before E_LCD_READ_RAM                {4}
    27.02.2009 Adapt for Luminary-Micro (DDR must be changed before setting driving value) {5}
    02.08.2010 Add LCD contrast and backlight configurations             {6}
    03.08.2011 Add simulated LCD read support                            {7}
    15.10.2011 Decide on port access style based on processor type       {8}

*/        

#include "config.h"



#if defined SUPPORT_LCD                                                  // activate this define to support LCD


/* =================================================================== */
/*                          local definitions                          */
/* =================================================================== */


#define OWN_TASK                 TASK_LCD


#define T_INIT_WAIT               (DELAY_LIMIT)(0.005*SEC)               // > 4,1ms (smallest value will be 1 TICK)

#define E_INIT_CONTINUE           1

#define STATE_INIT                0
#define STATE_INITIALISING        1
#define WRITE_FUNCTION_SET        2
#define STATE_INIT_2              3
#define STATE_INIT_3              4
#define STATE_INIT_4              5

#define STATE_DISPLAY_INITIALISED 6
#define STATE_LCD_SENDING         7
#define STATE_LCD_READY           8


#define DL_BIT 0x10                                                      // data length 8 bits
#define N_BIT  0x8                                                       // 2 line mode 
#define F_BIT  0                                                         // 5 x 7 dot font
#define I_BIT  0x2                                                       // curser increment 
#define S_BIT  0                                                         // display doesn't shift

#define LCD_BUSY 0x80


#if defined _HW_SAM7X || defined _HW_AVR32 || defined _LPC23XX || defined _LPC17XX || defined _STM32 || defined _KINETIS
    #define _ACCESSTYPE1                                                 // define access type best suited to these processor types
#endif
#if defined _HW_SAM7X || defined _HW_AVR32 || defined _LPC23XX || defined _LPC17XX || defined _KINETIS
    #define _ACCESSTYPE2                                                 // define access type best suited to these processor types
#endif
#if defined _LM3SXXXX || defined _STM32
    #define _ACCESSTYPE3                                                 // define access type best suited to these processor types
#endif


/* =================================================================== */
/*                      local structure definitions                    */
/* =================================================================== */


/* =================================================================== */
/*                 local function prototype declarations               */
/* =================================================================== */

static int fnInitDisplay(int iState);
static void fnWriteLine(unsigned char *pMsg, unsigned char ucMsgLen, LCD_CONTROL_PORT_SIZE Mode);
static unsigned char fnReadDisplay(LCD_CONTROL_PORT_SIZE Rs);
static void fnSendAppRead(unsigned char ucData);
static void fnSecureCommands(unsigned char *ucPtr, unsigned char ucLen);
static int fnSendDisplay(void);

/* =================================================================== */
/*                             constants                               */
/* =================================================================== */

/* =================================================================== */
/*                     global variable definitions                     */
/* =================================================================== */


/* =================================================================== */
/*                      local variable definitions                     */
/* =================================================================== */

#if defined _WINDOWS
    static int iCompleted = 1;
#endif



extern void fnLCD(TTASKTABLE *ptrTaskTable)                              // LCD task
{
    static int iState = STATE_INIT;
    QUEUE_HANDLE PortIDInternal = ptrTaskTable->TaskID;                  // queue ID for task input
    unsigned char ucInputMessage[MEDIUM_MESSAGE];                        // reserve space for receiving messages


    if (iState != STATE_LCD_READY) {
        iState = fnInitDisplay(iState);
#if defined _WINDOWS
        while (!iCompleted) {
            iState = fnInitDisplay(iState);
        }
#endif
    }

    while (fnRead( PortIDInternal, ucInputMessage, HEADER_LENGTH)) {     // check input queue
        switch (ucInputMessage[MSG_SOURCE_TASK]) {
        case TIMER_EVENT:
            break;

        case INTERRUPT_EVENT:
            break;

        case TASK_APPLICATION:
            {
                unsigned char ucLen = ucInputMessage[MSG_CONTENT_LENGTH];
                fnRead(PortIDInternal, ucInputMessage, ucLen);           // read the complete message
                ucLen--;
                if (E_LCD_COMMAND == ucInputMessage[0]) {
                    fnSecureCommands(&ucInputMessage[1], ucLen);         // we do a special check to ensure that the user doesn't change our interface mode of operation
                    fnWriteLine(&ucInputMessage[1], ucLen, 0);
                }
                else if (E_LCD_TEXT == ucInputMessage[0]) {
                    fnWriteLine(&ucInputMessage[1], ucLen, O_CONTROL_RS);     
                }
                else if (E_LCD_COMMAND_TEXT == ucInputMessage[0]) {      // first do command and then write text
                    fnSecureCommands(&ucInputMessage[1], 1);             // ensure command is not dangerous
                    fnWriteLine(&ucInputMessage[1], 1, 0);               // do one byte command
                    while (fnSendDisplay() == 0) {};                     // this should never return blocked but we test anyway
                    fnWriteLine(&ucInputMessage[2], (unsigned char)(ucLen-1), O_CONTROL_RS); // write text
                }
                else if (E_LCD_PATTERN == ucInputMessage[0]) {
                    fnWriteLine(&ucInputMessage[2], 1, O_CONTROL_RS);    // we write the pattern to all locations in presently connected LCD
                    uTaskerStateChange(OWN_TASK, UTASKER_GO);            // switch to polling mode of operation
                    iState = fnInitDisplay(iState = STATE_LCD_READY + ucInputMessage[1] - 1); // try to start transmission 
                    break;
                }
                else if (E_LCD_READ_ADDRESS == ucInputMessage[0]) {
                    fnEventMessage(LCD_PARTNER_TASK, TASK_LCD, fnReadDisplay(0)); // we read the value in the address register and return the result
                    fnSendAppRead(fnReadDisplay(0));
                    break;
                }
                else if (E_LCD_READ_RAM == ucInputMessage[0]) {
                    if (ucLen == 1) {                                    // {4}
                        fnSecureCommands(&ucInputMessage[1], 1);         // ensure command is not dangerous
                        fnWriteLine(&ucInputMessage[1], 1, 0);           // do one byte command
                        while (fnSendDisplay() == 0) {};                 // this should never return blocked but we test anyway
                    }
                    fnSendAppRead(fnReadDisplay(O_CONTROL_RS));          // we read the value in the internal RAM and return the result
                    break;
                }
                                                                         // assume only this type is received
                uTaskerStateChange(OWN_TASK, UTASKER_GO);                // switch to polling mode of operation
                iState = fnInitDisplay(iState = STATE_LCD_SENDING);      // try to start transmission 
            }
            break;
        }
    }
}


/**************************************************************************************************************/
#define MAX_TX_DBUF  40
static unsigned char ucTXDisp[MAX_TX_DBUF];                              // display output buffer
static unsigned char ucDispCount = 0;
static unsigned char ucDispSend = 0;                                     // the number of bytes already sent to display
static LCD_CONTROL_PORT_SIZE DisplayTextMode = 0;
#if defined _WINDOWS 
    static unsigned char ucLastRead = 0xff;                              // {7}
#endif

#define INIT_FUNCTION_SET          0x30
#if defined LCD_BUS_4BIT
    #define INIT_FUNCTION_SET_MODE 0x20
#else
    #define _fnWriteDisplay        fnWriteDisplay
    #define INIT_FUNCTION_SET_MODE 0x30
#endif
#define DISPLAY_OFF_NO_CURSOR      0x08
#define CLEAR_DISPLAY              0x01
#define DISPLAY_ON_NO_CURSOR       0x0c


static unsigned char fnReadDisplay(LCD_CONTROL_PORT_SIZE rs)
{
#if defined LCD_BUS_4BIT
    LCD_BUS_PORT_SIZE RdData_lsb;
#endif
    LCD_BUS_PORT_SIZE RdData_msb;

#if defined _ACCESSTYPE1                                                 // {8}
    SET_CONTROL_LINES(rs | O_WRITE_READ);                                // set required rs level and command read
    SET_DATA_LINES_INPUT();                                              // ensure the data lines are inputs
#else
    LCD_CONTROL_PORT_SIZE Control = (LCD_CONTROL_PORT_SIZE)(O_CONTROL_PORT_DAT & ~(O_CONTROL_LINES)); // backup other outputs of control port

    O_CONTROL_PORT_DAT = (Control | rs | O_WRITE_READ);                  // set required rs level and command read
    IO_BUS_PORT_DDR &= ~LCD_BUS_MASK;                                    // ensure the data lines are inputs
#endif

    CLOCK_EN_HIGH();                                                     // then clock EN to high state - tAS >= 40ns
                                                                         // date will be ready within about 100ns
    RdData_msb = (LCD_CONTROL_PORT_SIZE)(IO_BUS_PORT_DAT_IN & LCD_BUS_MASK); // read data in
#if DATA_SHIFT_LEFT > 0
    RdData_msb >>= DATA_SHIFT_LEFT;                                      // shift into position {2}
#elif DATA_SHIFT_RIGHT > 0
    RdData_msb <<= DATA_SHIFT_RIGHT;                                     // shift into position {2}
#endif

#if defined _ACCESSTYPE2                                                 // {8}
    O_SET_CONTROL_LOW(O_CONTROL_EN);                                     // then set clock EN low PWEH >= 220ns
#else
    O_CONTROL_PORT_DAT &= ~(O_CONTROL_EN);                               // then set clock EN low PWEH >= 220ns
#endif

#if defined LCD_BUS_4BIT
    // Since we are in 4 bit mode we must repeat clocking to ensure read is completed
    //
    DELAY_ENABLE_CLOCK_HIGH();                                           // ensure the second read is not too fast when in 4 bit mode {1}
    CLOCK_EN_HIGH();

    RdData_lsb = (LCD_BUS_PORT_SIZE)(IO_BUS_PORT_DAT_IN & LCD_BUS_MASK); // read data in
    #if DATA_SHIFT_LEFT > 0
    RdData_lsb >>= DATA_SHIFT_LEFT;                                      // shift into position {2}
    #elif DATA_SHIFT_RIGHT > 0
    RdData_lsb <<= DATA_SHIFT_RIGHT;                                     // shift into position {2}
    #endif

    #if defined _ACCESSTYPE2                                             // {8}
    O_SET_CONTROL_LOW(O_CONTROL_EN);
    #else
    O_CONTROL_PORT_DAT &= ~(O_CONTROL_EN);
    #endif
#endif

   // RS and R/W can be set back if required 10ns after EN falling edge
   // Data will remain stable at least 20ns
   // EN could then be reactivated with a periodicity of 500ns
    //
#if defined _ACCESSTYPE1                                                 // {8}
    SET_DATA_LINES_OUTPUT();                                             // ensure data bus outputs (between cycles)
#else
    IO_BUS_PORT_DDR |= LCD_BUS_MASK;                                     // ensure data bus outputs (between cycles)
#endif
#if defined _WINDOWS
    if (rs == 0) {
        RdData_msb &= ~LCD_BUSY;                                         // avoid blocking when simulating
    }
    #if defined LCD_BUS_4BIT                                             // {7}
    RdData_msb = (ucLastRead & 0xf0);
    RdData_lsb = (ucLastRead << 4);
    #else
    RdData_msb = ucLastRead;                                             // return the previously read value from last write command
    #endif
    ucLastRead = 0;                                                      // single-shot due to the way that the LCD simulatro works
#endif
#if defined LCD_BUS_4BIT
   return ((unsigned char)(RdData_msb | (RdData_lsb >> 4)));             // return the data byte read
#else
   return ((unsigned char)RdData_msb);
#endif
}

// Write a byte or nibble to the display
//
static void _fnWriteDisplay(LCD_CONTROL_PORT_SIZE rs, unsigned char ucData)
{
#if defined _ACCESSTYPE2                                                 // {8}
    LCD_BUS_PORT_SIZE DataBus = 0;
#else
    LCD_BUS_PORT_SIZE DataBus = (LCD_BUS_PORT_SIZE)IO_BUS_PORT_DAT;      // backup the present data lines (in case of supplimentary bits)
    LCD_CONTROL_PORT_SIZE Control = (LCD_BUS_PORT_SIZE)O_CONTROL_PORT_DAT; // backup present control line state
    DataBus &= ~LCD_BUS_MASK;                                            // mask out data in byte
#endif

#if DATA_SHIFT_LEFT > 0
    DataBus |= (((LCD_BUS_PORT_SIZE)ucData << DATA_SHIFT_LEFT) & LCD_BUS_MASK); // insert the data to be written
#else 
    DataBus |= (((LCD_BUS_PORT_SIZE)ucData >> DATA_SHIFT_RIGHT) & LCD_BUS_MASK);// insert the data to be written
#endif
#if defined _ACCESSTYPE2                                                 // {8}
    SET_BUS_DATA(DataBus);                                               // prepare data (it will be driven on to the bus later)
    SET_CONTROL_LINES(rs);                                               // set the RS bit accordingly, WR at zero, EN zero
    O_SET_CONTROL_HIGH(O_CONTROL_EN);                                    // then clock EN to high state - tAS >= 40ns 
                                                                         // date must have been set >= 60ns before EN falling edge 
#else
    #if defined _ACCESSTYPE3                                             // {5}{8}
    LCD_DRIVE_DATA();                                                    // drive the data bus (it initially drives the present state) - keep it stable for long enough before clocking
    #endif
    IO_BUS_PORT_DAT = DataBus;                                           // prepare data (it will be driven on to the bus later)
    Control &= ~O_CONTROL_LINES;                                         // mask out control bits
    Control |= rs;                                                       // set the RS bit accordingly
                                                                         // first set RS and R/W to write state 
    O_CONTROL_PORT_DAT = Control;                                        // RS as defined, WR at zero, EN zero
    O_CONTROL_PORT_DAT |= O_CONTROL_EN;                                  // then clock EN to high state - tAS >= 40ns 
                                                                         // date must have been set >= 60ns before EN falling edge 
#endif
#if !defined _ACCESSTYPE3                                                // {5}{8}
    LCD_DRIVE_DATA();                                                    // drive the data onto the bus - keep it stable for long enough before clocking
#endif
 
#if defined _WINDOWS 
    #if DATA_SHIFT_LEFT > 0   
        ucLastRead = CollectCommand((rs != 0), (unsigned char)((DataBus & LCD_BUS_MASK) >> DATA_SHIFT_LEFT)); // {7}
    #else
        ucLastRead = CollectCommand((rs != 0), (unsigned char)((DataBus & LCD_BUS_MASK) << DATA_SHIFT_RIGHT)); // {7}
    #endif
#endif

#if defined _ACCESSTYPE2                                                 // {8}
    O_SET_CONTROL_LOW(O_CONTROL_EN);                                     // then set clock EN low PWEH >= 220ns 
#else
    O_CONTROL_PORT_DAT &= ~O_CONTROL_EN;                                 // then set clock EN low PWEH >= 220ns 
#endif
  // RS and R/W can be set back if required 10ns after EN falling edge
  // Data must remain stable >= 10ns
  // EN could then be reactivated with a periodicity of 500ns
    //
}

#if defined LCD_BUS_4BIT
// This is only used in 4 bit mode to feed the byte to be written with two cycles
//
static void fnWriteDisplay(LCD_CONTROL_PORT_SIZE rs, unsigned char ucData)
{
    _fnWriteDisplay(rs, ucData);                                         // write high nibble
    _fnWriteDisplay(rs, (unsigned char)(ucData << 4));                   // write low nibble
}
#endif


// This routine copies the display message to the display buffer, ready for transmission
// It is assumed that the message will be completely sent before this routine is called again !!
//
static void fnWriteLine(unsigned char *pMsg, unsigned char ucMsgLen, LCD_CONTROL_PORT_SIZE Mode)
{
    if (ucDispCount != 0) {
        return;                                                          // the display is busy - must never occur !!
    }

    // The unterminated string or bytes are written to the display as fast as possible from the buffer
    //
    while (ucMsgLen--) {
        ucTXDisp[ucDispCount++] = *pMsg++;
        if (ucDispCount >= MAX_TX_DBUF) {
            break;
        }
    }
  
    DisplayTextMode = Mode;

    // The bytes will be sent to the display in the main loop one at a time
    //
}

static void fnSecureCommands(unsigned char *ucPtr, unsigned char ucLen)
{
    while (ucLen--) {
        if ((*ucPtr & 0xe0) == 0x20) {                                   // check for function set command
            *ucPtr &= ~DL_BIT;                                           // ensure we stay in 4 bit mode of operation by clearing the DL bit
        }
        ucPtr++;
    }
}


// If there is data waiting to be sent to the display and the display is not busy, send the next data using 2 writes in 4 bit mode
//
static int fnSendDisplay(void)
{
    if (ucDispCount != 0) {                                              // there is date to be sent to the display
        if ((fnReadDisplay(0) & LCD_BUSY) == 0) {                        // display is ready to accept data
            fnWriteDisplay(DisplayTextMode, (ucTXDisp[ucDispSend++]));   // write byte
            if (--ucDispCount == 0) {                                    // complete message has been sent
                ucDispSend = 0;
    #if defined _WINDOWS
                iCompleted = 1;
    #endif
                return (1);                                              // all has been set to display
            }
        }
    }
    #if defined _WINDOWS
    iCompleted = 0;
    #endif
    return (0);                                                          // either nothing done or not completed
}

#if defined LCD_CONTRAST_CONTROL                                         // {6}
static void fnSetContrast(void)
{
    TIMER_INTERRUPT_SETUP timer_setup = {0};                             // PWM Timer Init Struct
    timer_setup.int_type = TIMER_INTERRUPT;                              // timer setup type
    timer_setup.int_priority = 0;
    timer_setup.int_handler = 0;                                         // no interrupts used
    timer_setup.timer_reference = _LCD_CONTRAST_TIMER;                   // the timer used
    timer_setup.timer_mode  = _LCD_CONTRAST_TIMER_MODE_OF_OPERATION;     // the mode of operation
    timer_setup.timer_value = _LCD_CONTRAST_PWM_FREQUENCY;               // contrast control frequency
    timer_setup.pwm_value   = _PWM_PERCENT(temp_pars->temp_parameters.ucGLCDContrastPWM, timer_setup.timer_value); // contrast as PWM value
    fnConfigureInterrupt((void *)&timer_setup);                          // configure PWM output for contrast control
}
#endif

#if defined LCD_BACKLIGHT_CONTROL
extern void fnSetBacklight(void)
{
    TIMER_INTERRUPT_SETUP timer_setup = {0};                             // PWM Timer Init Struct
    timer_setup.int_type = TIMER_INTERRUPT;                              // timer setup type
    timer_setup.int_priority = 0;
    timer_setup.int_handler = 0;                                         // no interrupts used
    timer_setup.timer_reference = _LCD_BACKLIGHT_TIMER;                  // use PWM module
    timer_setup.timer_mode  = _LCD_BACKLIGHT_TIMER_MODE_OF_OPERATION;    // use PWM channel 2 on port 1 output location
    timer_setup.timer_value = _LCD_BACKLIGHT_PWM_FREQUENCY;              // 150 Hz backlight frequency
    timer_setup.pwm_value   = _PWM_PERCENT(temp_pars->temp_parameters.ucGLCDBacklightPWM, timer_setup.timer_value); // contrast as PWM value
    fnConfigureInterrupt((void *)&timer_setup);                          // configure PWM output for contrast control
}        
#endif


// General control procedure for display - including initialisation
//
static int fnInitDisplay(int iState)
{
    // After power up the display needs to be idle for >= 30ms. This is ensured by a start up delay of the task
    //
    if ((ucDispCount != 0) && (fnSendDisplay() == 0)) {
        return iState;                                                   // return when display transmitter busy, sending next byte on the way
    }

    if ((iState > WRITE_FUNCTION_SET) && ((fnReadDisplay(0) & LCD_BUSY) != 0)) {
        return iState;                                                   // LCD is busy
    }

    switch (iState) {
    case STATE_INIT:                                                     // initialise the LCD hardware once on startup
        INITIALISE_LCD_CONTROL_LINES();
#if defined LCD_CONTRAST_CONTROL                                         // {6}
        fnSetContrast();
#endif
    case STATE_INITIALISING:
        _fnWriteDisplay(0, INIT_FUNCTION_SET);                           // write function set
        uTaskerMonoTimer( OWN_TASK, T_INIT_WAIT, E_INIT_CONTINUE );      // wait at least 4,1ms the first time and 100us the second time - we do longer waits than necessayr
        break;

    case WRITE_FUNCTION_SET:
        _fnWriteDisplay(0, INIT_FUNCTION_SET);                           // repeat after at least 4,1ms
        _fnWriteDisplay(0, INIT_FUNCTION_SET_MODE);                      // set final value after another 100us (after this the LCD is in the required 4 or 8 bit mode)
        uTaskerStateChange(OWN_TASK, UTASKER_GO);                        // switch to polling mode of operation since BF (Busy Flag) is not valid
        break;

    case STATE_INIT_2:
        fnWriteDisplay(0, (INIT_FUNCTION_SET_MODE | N_BIT | F_BIT));     // 2 line mode, display on
        break;

    case STATE_INIT_3:
        fnWriteDisplay(0, DISPLAY_OFF_NO_CURSOR);
        break;

    case STATE_INIT_4:
        fnWriteDisplay(0, CLEAR_DISPLAY);
        break;

    case STATE_DISPLAY_INITIALISED:
        fnWriteDisplay(0, DISPLAY_ON_NO_CURSOR);

        // At this point the display is clear and the cursor is at the home position.
        // we inform the application that the initialisation has terminated, so that it can start using it
        fnEventMessage(LCD_PARTNER_TASK, TASK_LCD, E_LCD_INITIALISED);
#if defined LCD_BACKLIGHT_CONTROL
        fnSetBacklight();                                                // {6}
#endif
        return STATE_LCD_READY;                                          // we are ready to work....

    case STATE_LCD_SENDING:
        uTaskerStateChange(OWN_TASK, UTASKER_STOP);                      // switch to event mode of operation
        fnEventMessage(LCD_PARTNER_TASK, TASK_LCD, E_LCD_READY);         // we have completed an operation for the application
        break;

    default:                                                             
        if (iState > STATE_LCD_READY) {                                  // writing a pattern
            if (--iState <= STATE_LCD_READY) {                           // last character to be written
                iState = STATE_LCD_SENDING;
            }
            fnWriteLine(ucTXDisp, 1, O_CONTROL_RS);                      // repeat the test pattern 
        }
        return iState;
    }
    return (++iState);
}

// Send data read from the LCD to the application 
//
static void fnSendAppRead(unsigned char ucData)
{
    unsigned char ucMessage[ HEADER_LENGTH + 2];

    ucMessage[MSG_DESTINATION_NODE]   = INTERNAL_ROUTE;                  // destination node 
    ucMessage[MSG_SOURCE_NODE]        = INTERNAL_ROUTE;                  // own node 
    ucMessage[MSG_DESTINATION_TASK]   = LCD_PARTNER_TASK;                // destination task
    ucMessage[MSG_SOURCE_TASK]        = OWN_TASK;                        // own task
    ucMessage[MSG_CONTENT_LENGTH]     = 2;
    ucMessage[MSG_CONTENT_COMMAND]    = E_LCD_READ;
    ucMessage[MSG_CONTENT_DATA_START] = ucData;

    fnWrite(0, ucMessage, (HEADER_LENGTH + 2));                          // send message to defined task
}

#endif

