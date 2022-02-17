/**********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher
    
    ---------------------------------------------------------------------
    File:      glcd_tft.h [LPC247x / LPC1788 / Kinetis K70]
    Project:   uTasker project
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2016
    *********************************************************************
    09.08.2010 Add CGLCD_PIXEL_SIZE to control effective size             {1}
    10.08.2010 Don't remove pull-up from P1-27 when USB is used           {2}
    11.08.2010 Add touch screen operation                                 {3}
    13.11.2011 Remove prototype CollectCommand()                          {4}
    05.09.2012 Enable code when SUPPORT_TFT                               {5}
    03.10.2012 Add LPC1788 support                                        {6} - merged from a previous version with this already contained
    20.12.2014 Added K70 LCD support                                      {7}
       
*/

#if (defined TFT_GLCD_MODE || defined SUPPORT_TFT) && !defined CGLCD_GLCD_MODE && !defined KITRONIX_GLCD_MODE && !defined MB785_GLCD_MODE // {5}
    #if !defined _GLCD_TFT_DEFINES
        #define GLCD_BUSY()                 0                            // no LCD busy check
        #define X_BYTES	                    (GLCD_X/CGLCD_PIXEL_SIZE/8)  // {1} the number of bytes holding the X-pixels
        #define Y_BYTES	                    (GLCD_Y/CGLCD_PIXEL_SIZE)    // {1} the number of rows of X_BYTES holding the Y rows
        #define DISPLAY_LEFT_PIXEL          0  
        #define DISPLAY_TOP_PIXEL           0
        #define DISPLAY_RIGHT_PIXEL         (GLCD_X - 1)
        #define DISPLAY_BOTTOM_PIXEL        (GLCD_Y - 1)
        #define UPDATE_WIDTH                ((X_BYTES + 7)/8)
        #define UPDATE_HEIGHT               Y_BYTES


        // SDRAM characteristics
        //
        #define SDRAM_REFRESH            7813
        #define SDRAM_TRP                20
        #define SDRAM_TRAS               45
        #define SDRAM_TAPR               1
        #define SDRAM_TDAL               3
        #define SDRAM_TWR                3
        #define SDRAM_TRC                65
        #define SDRAM_TRFC               66
        #define SDRAM_TXSR               67
        #define SDRAM_TRRD               15
        #define SDRAM_TMRD               3

        #define SDRAM_PERIOD             (float)((float)1000000000 / (float)MASTER_CLOCK)
        #define CALC_PERIOD(Period)      (((Period < SDRAM_PERIOD) ? 0:(unsigned long)((float)Period / SDRAM_PERIOD)) + 1)

        // TFT LCD characteristics
        //
        #define PIXEL_CLOCK_RATE         6400000
        #define HORIZONTAL_BACK_PORCH    38
        #define HORIZONTAL_FRONT_PORCH   20
        #define HORIZONTAL_PULSE         30
        #define VERTICAL_BACK_PORCH      15
        #define VERTICAL_FRONT_PORCH     5
        #define VERTICAL_PULSE           3

        #define LCD_VRAM_BASE_ADDR       SDRAM_ADDR                      // LCD ram set to the start of SDRAM
        #if !defined _KINETIS
            static void fnInit_SDRAM(void);
        #endif
        static void fnInitLCD_Controller(void);
        #if defined SUPPORT_TOUCH_SCREEN
            static void fnStartTouch(void);
        #endif
        #if defined _WINDOWS
          //extern void CollectCommand(int bRS, unsigned long ulByte);   // {4}
            extern unsigned char *fnGetSDRAM(unsigned char *ptrSDRAM);
        #else
            #define fnGetSDRAM(x) x
        #endif

        #define _GLCD_TFT_DEFINES                                        // include only once
    #endif

    #if defined _GLCD_COMMANDS                                           // link in TFT specific interface commands

#if defined _WINDOWS
static unsigned char ucSDRAM[SDRAM_SIZE];                                // SDRAM, used to store the display image - automatically refreshed by the LCD controller

extern unsigned char *fnGetSDRAM(unsigned char *ptrSDRAM)
{
    CAST_POINTER_ARITHMETIC offsetSDRAM = (CAST_POINTER_ARITHMETIC)ptrSDRAM - SDRAM_ADDR;
    return (ucSDRAM + offsetSDRAM);
}
#endif


#if defined SUPPORT_TOUCH_SCREEN
static unsigned short usResults[2] = {0};                                // X, Y touch result

#if defined LPC1788
    #define TOUCH_X_DRIVE_PLUS   PORT0_BIT24
    #define TOUCH_X_DRIVE_MINUS  PORT0_BIT19
    #define TOUCH_Y_DRIVE_PLUS   PORT0_BIT23
    #define TOUCH_Y_DRIVE_MINUS  PORT0_BIT21

    #define MIN_X_TOUCH          0x2000                                  // tuned values - for calibration these should be taken from parameters
    #define MAX_X_TOUCH          0xe800
    #define MIN_Y_TOUCH          0x2900
    #define MAX_Y_TOUCH          0xe000
#elif defined K70F150M_12M
    #define TOUCH_X_DRIVE_PLUS   PORTA_BIT9
    #define TOUCH_X_DRIVE_MINUS  PORTA_BIT8
    #define TOUCH_Y_DRIVE_PLUS   PORTA_BIT11
    #define TOUCH_Y_DRIVE_MINUS  PORTA_BIT10

    #define MIN_X_TOUCH          0x0230                                  // tuned values - for calibration these should be taken from parameters
    #define MAX_X_TOUCH          0x0e90
    #define MIN_Y_TOUCH          0x03c5
    #define MAX_Y_TOUCH          0x0d30
    #define MIN_MOUSE_MOVE       4                                       // pixel movements of less than this are filtered
#else
    #define TOUCH_X_DRIVE_PLUS   PORT0_BIT24
    #define TOUCH_X_DRIVE_MINUS  PORT0_BIT22
    #define TOUCH_Y_DRIVE_PLUS   PORT0_BIT21
    #define TOUCH_Y_DRIVE_MINUS  PORT0_BIT23

    #define MIN_X_TOUCH          0x2000                                  // tuned values - for calibration these should be taken from parameters
    #define MAX_X_TOUCH          0xe800
    #define MIN_Y_TOUCH          0x2900
    #define MAX_Y_TOUCH          0xe000
#endif

#if defined _WINDOWS
    #define MAX_MOVEMENT_Y       (GLCD_Y/4)
    #define MAX_MOVEMENT_X       (GLCD_X/4)
#else
    #define MAX_MOVEMENT_Y       6
    #define MAX_MOVEMENT_X       6
#endif

#define PEN_DOWN_DEBOUNCE        6                                       // pen detection debounce delay (number of samples)

static int iTouchState = 0;                                              // touch screen polling state
static int iPenDown = 0;                                                 // initially pen is up (not touching touch screen)
static int iCalibrationActive = 0;

extern void fnStartTouchCalibration(void)
{
    iCalibrationActive = 20;
    iPenDown = 0;
}


#if defined TOUCH_MOUSE_TASK
    #if !defined SUPPORT_DOUBLE_QUEUE_WRITES
        #error "SUPPORT_DOUBLE_QUEUE_WRITES should be enabled when using mouse events"
    #endif
static void fnReportTouchMouse(TOUCH_MOUSE_EVENT_MESSAGE *ptmEvent)
{
    unsigned char ucMessage[HEADER_LENGTH + 1 + sizeof(TOUCH_MOUSE_EVENT_MESSAGE)]; // buffer with required event space
    ucMessage[MSG_DESTINATION_NODE] = INTERNAL_ROUTE;                    // destination node
    ucMessage[MSG_SOURCE_NODE]      = INTERNAL_ROUTE;                    // own node
    ucMessage[MSG_DESTINATION_TASK] = TOUCH_MOUSE_TASK;                  // destination task
    ucMessage[MSG_SOURCE_TASK]      = TASK_LCD;                          // own task
    ucMessage[MSG_CONTENT_LENGTH]   = (sizeof(TOUCH_MOUSE_EVENT_MESSAGE) + 1); // message length
    ucMessage[MSG_CONTENT_COMMAND]  = E_TOUCH_MOUSE_EVENT;               // command is a touch mouse event
    if (fnWrite(INTERNAL_ROUTE, ucMessage, 0) != 0) {                    // send message header
        fnWrite(INTERNAL_ROUTE, (unsigned char *)ptmEvent, sizeof(TOUCH_MOUSE_EVENT_MESSAGE)); // add data
    }
}

    #if defined MIN_MOUSE_MOVE && MIN_MOUSE_MOVE > 0
static int fnFilterMovement(unsigned short usX, unsigned short usY, unsigned short usLastMovementX, unsigned short usLastMovementY)
{
    int iDeviation;
    if (usX > usLastMovementX) {
        iDeviation = (usX - usLastMovementX);
    }
    else {
        iDeviation = (usLastMovementX - usX);
    }
    if (usY > usLastMovementY) {
        iDeviation += (usY - usLastMovementY);
    }
    else {
        iDeviation += (usLastMovementY - usY);
    }
    if (iDeviation > MIN_MOUSE_MOVE) {
        return 0;
    }
    return 1;
}
    #endif
#endif

// 4ms touch screen interrupt (from ADC result)
//
static void fnTouchValue(unsigned short usX, unsigned short usY)         // x and y values ready
{
static int iDebugOut = 0;
    static unsigned short usLastPixelX, usLastPixelY;
#if defined TOUCH_MOUSE_TASK
    static unsigned short usLastMovementX, usLastMovementY;
    TOUCH_MOUSE_EVENT_MESSAGE tmEvent;
#endif
    int iMove = 0;
    unsigned char ucMemoryContent;
    unsigned char ucNewContent;

    if (++iDebugOut > 500) {
        iDebugOut = 0;
        fnDebugHex(usX, (sizeof(usX) | WITH_LEADIN));
        fnDebugHex(usY, (sizeof(usY) | WITH_SPACE | WITH_LEADIN | WITH_CR_LF));
    }

    if (iPenDown < (PEN_DOWN_DEBOUNCE + iCalibrationActive)) {           // presently the pen is not down and a touch point is being waited for
        if (usX > MAX_X_TOUCH) {                                         // since the ADC default to high input value when not connected check with maximum values
            iPenDown = 0;                                                // no touch value detected so pen is not down
            return;
        }
        if (usY > MAX_Y_TOUCH) {                                         // since the ADC default to high input value when not connected check with maximum values
            iPenDown = 0;                                                // no touch value detected so pen is not down
            return;
        }
    #if defined _WINDOWS
        iPenDown = (PEN_DOWN_DEBOUNCE + iCalibrationActive - 1);
    #endif
        if (++iPenDown < (PEN_DOWN_DEBOUNCE + iCalibrationActive)) {     // the touch needs to remain stable for a short time before it is accepted
            return;
        }                                                                // just detected pen being applied
    }
    else {                                                               // pen presently down and we are expecting either movement or release
        if (usX > MAX_X_TOUCH) {
            if (usY > MAX_Y_TOUCH) {
                iPenDown = 0;                                            // just detected pen being removed
    #if defined TOUCH_MOUSE_TASK
                tmEvent.ucEvent = TOUCH_MOUSE_RELEASE;
                tmEvent.usX = usLastPixelX;
                tmEvent.usY = usLastPixelY;
                fnReportTouchMouse(&tmEvent);
    #else
                // Test output to show a click location on the display
                //
                fnDebugMsg("Release detect: ");
                fnDebugDec(usLastPixelX, 0);
                fnDebugMsg(", ");
                fnDebugDec(usLastPixelY, WITH_CR_LF);
    #endif
            }
            return;
        }
        else if (usY > MAX_Y_TOUCH) {                                    // pen probably being lifted but wait until both x and y values confirm this
            return;
        }
        iMove = 1;                                                       // the new location is considered to be a movement
    }
    if (usX <= MIN_X_TOUCH) {                                            // limit
        usX = 0;                                                         // left of screen
    }
    else {
        usX = (unsigned short)(((unsigned long)((usX - MIN_X_TOUCH - temp_pars->temp_parameters.sTouchXminCal) * (GLCD_X - 1)))/(MAX_X_TOUCH - MIN_X_TOUCH - temp_pars->temp_parameters.sTouchXminCal + temp_pars->temp_parameters.sTouchXmaxCal)); // convert to pixel location
        if (usX >= GLCD_X) {
            usX = (GLCD_X - 1);
        }
    }
    if (usY <= MIN_Y_TOUCH) {
        usY = 0;                                                         // top of screen
    }
    else {
        usY = (unsigned short)(((unsigned long)((usY - MIN_Y_TOUCH - temp_pars->temp_parameters.sTouchYminCal) * (GLCD_Y - 1)))/(MAX_Y_TOUCH - MIN_Y_TOUCH - temp_pars->temp_parameters.sTouchYminCal + temp_pars->temp_parameters.sTouchYmaxCal));
        if (usY >= GLCD_Y) {
            usY = (GLCD_Y - 1);
        }
    }
    if (iMove != 0) {                                                    // the new coordinates are due to a movement and not a pen detection/release
        if (usX > usLastPixelX) {                                        // movement to right
            if ((usX - usLastPixelX) > MAX_MOVEMENT_X) {
                return;                                                  // step too large so ignore
            }
        }
        else {
            if ((usLastPixelX - usX) > MAX_MOVEMENT_X) {
                return;                                                  // step too large so ignore
            }
        }
        if (usY > usLastPixelY) {                                        // movement down
            if ((usY - usLastPixelY) > MAX_MOVEMENT_Y) {
                return;                                                  // step too large so ignore
            }
        }
        else {
            if ((usLastPixelY - usY) > MAX_MOVEMENT_Y) {
                return;                                                  // step too large so ignore
            }
        }
        if ((usX == usLastPixelX) && (usY == usLastPixelY)) {            // if the location remains unchanged
            return;
        }
    #if defined TOUCH_MOUSE_TASK
        #if defined MIN_MOUSE_MOVE && MIN_MOUSE_MOVE > 0
        if (fnFilterMovement(usX, usY, usLastMovementX, usLastMovementY) == 0) {
        #endif
            tmEvent.ucEvent = TOUCH_MOUSE_MOVE;
            tmEvent.usX = usX;
            tmEvent.usY = usY;
            fnReportTouchMouse(&tmEvent);
            usLastMovementX = usX;
            usLastMovementY = usY;
        #if defined MIN_MOUSE_MOVE && MIN_MOUSE_MOVE > 0
        }
        #endif
    #endif
    }
    else {
    #if defined TOUCH_MOUSE_TASK
        if (iCalibrationActive != 0) {
            tmEvent.ucEvent = TOUCH_MOUSE_CALIBRATION;
            iCalibrationActive = 0;
        }
        else {
            tmEvent.ucEvent = TOUCH_MOUSE_PRESS;
        }
        tmEvent.usX = usX;
        tmEvent.usY = usY;
        fnReportTouchMouse(&tmEvent);
        usLastMovementX = usX;
        usLastMovementY = usY;
    #else
        // Test output to show a click location on the display
        //
        fnDebugMsg("Touch detect: ");
        fnDebugDec(usX, 0);
        fnDebugMsg(", ");
        fnDebugDec(usY, WITH_CR_LF);
    #endif
    }
    usLastPixelX = usX;                                                  // save last valid pixel location
    usLastPixelY = usY;
    #if CGLCD_PIXEL_SIZE > 1
    usY /= CGLCD_PIXEL_SIZE;
    usX /= CGLCD_PIXEL_SIZE;
    #endif
    // This code paints the present touch position directly to the image
    //
    ucNewContent = ucMemoryContent = ucPixelArray[usY][usX/8];           // original content at the touch location
    ucNewContent |= (0x80 >> usX%8);                                     // set the pixel
    if (ucNewContent != ucMemoryContent) {                               // if this causes a content change
        ucPixelArray[usY][usX/8] = ucNewContent;
        ucByteUpdateArray[usY][usX/64] |= (0x80 >> (usX/8)%8);           // mark the need for an update due to content change
    }
}

// Periodic interrupt used to control the resistive touch screen (this is called from an interrupt routine)
//
static void fnTouchInterrupt(void)
{
    #if defined _WINDOWS
    extern void fnGetPenSamples(unsigned short *ptrX, unsigned short *ptrY);
    #endif
    #if defined _KINETIS
    static unsigned long ulCalibrate = ADC_CALIBRATE;
    #endif
    ADC_SETUP adc_setup; 
    switch (iTouchState++) {
    case 0:                                                              // delay to allow stabilisation ready for Y measurement
    #if defined _KINETIS
        _CONFIG_PORT_INPUT_FAST_LOW(A, ((TOUCH_Y_DRIVE_PLUS | TOUCH_Y_DRIVE_MINUS)), PORT_PS_UP_ENABLE/*PORT_NO_PULL*/); // set Y outputs to high impedance state
        _DRIVE_PORT_OUTPUT_VALUE(A, (TOUCH_X_DRIVE_PLUS | TOUCH_X_DRIVE_MINUS), TOUCH_X_DRIVE_PLUS); // apply a voltage across X
    #else
        _FLOAT_PORT(0, (TOUCH_Y_DRIVE_PLUS | TOUCH_Y_DRIVE_MINUS));      // set Y outputs to high impedance state
        _DRIVE_PORT_OUTPUT_VALUE(0, (TOUCH_X_DRIVE_PLUS | TOUCH_X_DRIVE_MINUS), TOUCH_X_DRIVE_PLUS); // apply a voltage across X
    #endif
        break;
    case 1:                                                              // start adc measurement of Y input
        adc_setup.int_type = ADC_INTERRUPT;                              // identifier when configuring
        adc_setup.int_handler = 0;                                       // no interrupt on completion
    #if defined _KINETIS
        adc_setup.int_adc_controller = 3;                                // ADC 3
        adc_setup.int_adc_bit = ADC_SE4_SINGLE;                          // Y input to be measured
        adc_setup.pga_gain = PGA_GAIN_OFF;
        adc_setup.int_adc_offset = 0;
        adc_setup.int_adc_mode = (ulCalibrate | ADC_SELECT_INPUTS_A | ADC_CLOCK_BUS_DIV_2 | ADC_CLOCK_DIVIDE_8 | ADC_SAMPLE_ACTIVATE_LONG | ADC_CONFIGURE_ADC | ADC_REFERENCE_VREF | ADC_CONFIGURE_CHANNEL | ADC_SINGLE_ENDED | ADC_SINGLE_SHOT_MODE | ADC_12_BIT_MODE | ADC_SW_TRIGGERED); // note that the first configuration should calibrate the ADC - single shot with interrupt on completion
        adc_setup.int_adc_sample = (ADC_SAMPLE_LONG_PLUS_12 | ADC_SAMPLE_AVERAGING_32); // additional sampling clocks
        adc_setup.int_adc_result = 0;                                    // no result is requested
        ulCalibrate = 0;                                                 // calibration performed only once
    #else
        adc_setup.int_adc_single_ended_inputs = (ADC_CHANNEL_0);         // ADC channel 0 as single ended inputs (only one channel should be selected in SW mode)
        adc_setup.int_adc_mode = (ADC_SINGLE_SHOT_SW | ADC_CONFIGURE_ADC); // configure and start single conversion
        adc_setup.SamplingSpeed = ADC_SAMPLING_SPEED(10000);             // slow sampling speed
    #endif
        fnConfigureInterrupt((void *)&adc_setup);                        // configure and start sequence
        break;
    case 2:
    #if defined _KINETIS
        usResults[0] = (unsigned short)ADC3_RA;                          // read the X value
        _CONFIG_PORT_INPUT_FAST_LOW(A, ((TOUCH_X_DRIVE_PLUS | TOUCH_X_DRIVE_MINUS)), PORT_PS_UP_ENABLE/*PORT_NO_PULL*/); // set X outputs to high impedance state
        _DRIVE_PORT_OUTPUT_VALUE(A, (TOUCH_Y_DRIVE_PLUS | TOUCH_Y_DRIVE_MINUS), TOUCH_Y_DRIVE_PLUS); // apply a voltage across Y (this removes ADC input mux)
    #else
        usResults[0] = (unsigned short)AD0GDR;                           // read X value
        #if defined LPC1788
        IOCON_P0_23 = 0;                                                 // remove the AD pin function
        #else
        PINSEL1 &= ~(PINSEL1_P0_23_RESET);                               // remove the AD pin function
        #endif
        _FLOAT_PORT(0, (TOUCH_X_DRIVE_PLUS | TOUCH_X_DRIVE_MINUS));      // set X outputs to high impedance state
        _DRIVE_PORT_OUTPUT_VALUE(0, (TOUCH_Y_DRIVE_PLUS | TOUCH_Y_DRIVE_MINUS), TOUCH_Y_DRIVE_PLUS); // apply a voltage across Y
    #endif
        break;
    case 3:
        adc_setup.int_type = ADC_INTERRUPT;                              // identifier when configuring
        adc_setup.int_handler = 0;                                       // no interrupt on completion
    #if defined _KINETIS
        adc_setup.int_adc_controller = 3;                                // ADC 3
        adc_setup.int_adc_bit = ADC_SE5_SINGLE;                          // X input to be measured
        adc_setup.pga_gain = PGA_GAIN_OFF;
        adc_setup.int_adc_offset = 0;
        adc_setup.int_adc_mode = (ADC_SELECT_INPUTS_A | ADC_CONFIGURE_CHANNEL | ADC_SINGLE_ENDED | ADC_SINGLE_SHOT_MODE | ADC_12_BIT_MODE | ADC_SW_TRIGGERED);
        adc_setup.int_adc_result = 0;                                    // no result is requested
    #else
        adc_setup.int_adc_single_ended_inputs = (ADC_CHANNEL_1);         // ADC channel 1 as single ended inputs (only one channel should be selected in SW mode
        adc_setup.int_adc_mode = (ADC_SINGLE_SHOT_SW | ADC_CONFIGURE_ADC); // configure and start single conversion
        adc_setup.SamplingSpeed = ADC_SAMPLING_SPEED(10000);             // slow sampling speed
    #endif
        fnConfigureInterrupt((void *)&adc_setup);                        // configure and start sequence
        break;
    case 4:
    #if defined _KINETIS
        usResults[1] = (unsigned short)ADC3_RA;                          // read the Y value
        _CONFIG_PORT_INPUT_FAST_LOW(A, ((TOUCH_Y_DRIVE_PLUS | TOUCH_Y_DRIVE_MINUS)), PORT_PS_UP_ENABLE/*PORT_NO_PULL*/); // set Y outputs to high impedance state
        _DRIVE_PORT_OUTPUT_VALUE(A, (TOUCH_X_DRIVE_PLUS | TOUCH_X_DRIVE_MINUS), TOUCH_X_DRIVE_PLUS); // apply a voltage across X (this removes ADC input mux)
    #else
        usResults[1] = (unsigned short)AD0GDR;                           // read Y value
        #if defined LPC1788
        IOCON_P0_23 = IOCON_P0_24;                                       // remove ADC function
        #else
        PINSEL1 &= ~(PINSEL1_P0_24_RESET);                               // remove the AD pin function
        #endif
        _FLOAT_PORT(0, (TOUCH_Y_DRIVE_PLUS | TOUCH_Y_DRIVE_MINUS));      // set Y outputs to high impedance state
        _DRIVE_PORT_OUTPUT_VALUE(0, (TOUCH_X_DRIVE_PLUS | TOUCH_X_DRIVE_MINUS), TOUCH_X_DRIVE_PLUS); // apply a voltage across X
    #endif
    #if defined _WINDOWS
        fnGetPenSamples(&usResults[0], &usResults[1]);
    #endif
        fnTouchValue(usResults[0], usResults[1]);                        // process the input co-ordinate
        iTouchState = 1;                                                 // start the process again
        break;
    }
}

// The function is presently dedicated to LPC24xx/LPC17xx/Kinetis K70
//
static void fnStartTouch(void)                                           // {3}
{
    TIMER_INTERRUPT_SETUP timer_setup;                                   // interrupt configuration parameters

    #if defined _KINETIS
    _CONFIG_DRIVE_PORT_OUTPUT_VALUE(A, (TOUCH_X_DRIVE_PLUS | TOUCH_X_DRIVE_MINUS | TOUCH_Y_DRIVE_PLUS | TOUCH_Y_DRIVE_MINUS), 0, (PORT_NO_PULL | PORT_DSE_LOW | PORT_SRE_SLOW)); // configure and drive initial line state
    #else
    _CONFIG_DRIVE_PORT_OUTPUT_VALUE(0, (TOUCH_X_DRIVE_PLUS | TOUCH_X_DRIVE_MINUS), 0); // configure and drive initial line state
    _CONFIG_DRIVE_PORT_OUTPUT_VALUE(0, (TOUCH_Y_DRIVE_PLUS | TOUCH_Y_DRIVE_MINUS), 0);
    #endif

    timer_setup.int_type = TIMER_INTERRUPT;                              // a hardware timer is used to generate the interrupt used for resistive touch operation
    timer_setup.int_priority = PRIORITY_TIMERS;
    timer_setup.int_handler = fnTouchInterrupt;                          // the handling interrupt routine
    timer_setup.timer_reference = TOUCH_HW_TIMER;                        // HW timer channel for touch time base
    #if defined _HW_TIMER_MODE || defined _KINETIS
    timer_setup.timer_mode = (TIMER_PERIODIC | TIMER_MS_VALUE);          // period timer
    timer_setup.timer_value = TIMER_MS_DELAY(1);                         // 1ms interrupt rate
    #else
    timer_setup.timer_value = 1;                                         // 1ms interrupt rate
    #endif
    fnConfigureInterrupt((void *)&timer_setup);                          // enter interrupt for timer test
}
#endif

#if !defined _KINETIS
// SDRAM initialisation
//
static void fnInit_SDRAM(void)
{
    volatile unsigned long ulDelay;

    #if defined LPC1788                                                  // {4}
    #define EMC_DELAY ((10 * MASTER_CLOCK)/120000000)                    // delay of 10 is suitable for 120MHz operation - less delay calculated for slower speed
    unsigned long *ptrPin;
    ptrPin = IOCON_P3_ADD;
    for (ulDelay = 0; ulDelay <= 31; ulDelay++) {                        // configure port 3 as EMC D0..D31
        *ptrPin++ = _EMC_P3_0_DATA_0;                                    // all EMC functions use the same function number
    }
    ptrPin = IOCON_P4_ADD;
    for (ulDelay = 0; ulDelay <= 14; ulDelay++) {                        // configure port 4 as EMC A0..A14
        *ptrPin++ = _EMC_P4_0_ADDR_0;                                    // all EMC functions use the same function number
    }
    IOCON_P4_25 = _EMC_P4_25_WE;                                         // write enable
    IOCON_P2_16 = _EMC_P2_16_CAS;
    IOCON_P2_17 = _EMC_P2_17_RAS;
    IOCON_P2_18 = _EMC_P2_18_CLK0;
    IOCON_P2_20 = _EMC_P2_20_DYCS0;
    IOCON_P2_24 = _EMC_P2_24_CKE0;
    IOCON_P2_28 = _EMC_P2_28_DQM0;
    IOCON_P2_29 = _EMC_P2_29_DQM1;
    IOCON_P2_30 = _EMC_P2_30_DQM2;
    IOCON_P2_31 = _EMC_P2_31_DQM3;

    EMCCLKSEL = 0;                                                       // clock EMC at the same rate as the CPU
    EMCDLYCTL = ((EMC_DELAY << 16) | (EMC_DELAY << 8) | EMC_DELAY);      // for 120MHz operation

    EMCControl           = EMC_ENABLE;                                   // initialise the SDRAM controller and enable EMC clock
    EMCDynamicReadConfig = READ_DATA_STRATEGY_CMD_DLY;
    EMCDynamicRasCas0    = (RAS_LATENCY_THREE_CCLK | CAS_LATENCY_THREE_CCLK);
    EMCDynamicRP         = CALC_PERIOD(SDRAM_TRP);
    EMCDynamicRAS        = CALC_PERIOD(SDRAM_TRAS);
    EMCDynamicSREX       = CALC_PERIOD(SDRAM_TXSR);
    EMCDynamicAPR        = SDRAM_TAPR;
    EMCDynamicDAL        = SDRAM_TDAL + CALC_PERIOD(SDRAM_TRP);
    EMCDynamicWR         = SDRAM_TWR;
    EMCDynamicRC         = CALC_PERIOD(SDRAM_TRC);
    EMCDynamicRFC        = CALC_PERIOD(SDRAM_TRFC);
    EMCDynamicXSR        = CALC_PERIOD(SDRAM_TXSR);
    EMCDynamicRRD        = CALC_PERIOD(SDRAM_TRRD);
    EMCDynamicMRD        = SDRAM_TMRD;
    EMCDynamicConfig0    = ADDRESS_MAP_256M_32BIT;                        // 13 row, 9 - col, SDRAM

    // SDRAM initialisation sequence
    //
    EMCDynamicControl = (SDRAM_NOP_CMD | DYNAMIC_CLKOUT_CONTINUOUS | DYNAMIC_MEM_CLK_EN);
    ulDelay = 200*30;
    while (ulDelay--) {}
    EMCDynamicControl = (SDRAM_PRECHARGE_ALL_CMD | DYNAMIC_CLKOUT_CONTINUOUS | DYNAMIC_MEM_CLK_EN); // PALL
    EMCDynamicRefresh = 1;
    ulDelay = 256;
    while (ulDelay--) {}                                                 // > 128 clock delay
    EMCDynamicRefresh = CALC_PERIOD(SDRAM_REFRESH) >> 4;
    EMCDynamicControl = (SDRAM_MODE_CMD | DYNAMIC_CLKOUT_CONTINUOUS | DYNAMIC_MEM_CLK_EN); // COMM
    ulDelay = *(volatile unsigned short *)fnGetSDRAM((unsigned char *)(SDRAM_ADDR + (0x32UL << (13)))); // burst 8, sequential, CAS-2
    EMCDynamicControl = 0;                                               // NORM
    EMCDynamicConfig0 |= CS_BUFFER_ENABLE;
    #else                                                                // LPC24xx
    // Configure the SDRAM pins 
    //
    PINSEL5  |= (PINSEL5_P2_16_CAS | PINSEL5_P2_17_RAS | PINSEL5_P2_18_CLKOUT0 | PINSEL5_P2_20_DYCS0 | PINSEL5_P2_24_CKEOUT1 | PINSEL5_P2_28_DQMOUT0 | PINSEL5_P2_29_DQMOUT1); // assign the pins required by the SDRAM controller
    PINMODE5 |= (PINMODE_NO_PULLS_16 | PINMODE_NO_PULLS_17 | PINMODE_NO_PULLS_18 | PINMODE_NO_PULLS_20 | PINMODE_NO_PULLS_24 | PINMODE_NO_PULLS_28 | PINMODE_NO_PULLS_29);
    PINSEL6  = (PINSEL6_P3_0_D0 | PINSEL6_P3_1_D1 | PINSEL6_P3_2_D2 | PINSEL6_P3_3_D3 | PINSEL6_P3_4_D4 | PINSEL6_P3_5_D5 | PINSEL6_P3_6_D6 | PINSEL6_P3_7_D7 | PINSEL6_P3_8_D8 | PINSEL6_P3_9_D9 | PINSEL6_P3_10_D10 | PINSEL6_P3_11_D11 | PINSEL6_P3_12_D12 | PINSEL6_P3_13_D13 | PINSEL6_P3_14_D14 | PINSEL6_P3_15_D15); // data lines D0..D15
    PINMODE6 = (PINMODE_NO_PULLS_0 | PINMODE_NO_PULLS_1 | PINMODE_NO_PULLS_2 | PINMODE_NO_PULLS_3 | PINMODE_NO_PULLS_4 | PINMODE_NO_PULLS_5 | PINMODE_NO_PULLS_6 | PINMODE_NO_PULLS_7 | PINMODE_NO_PULLS_8 | PINMODE_NO_PULLS_9 | PINMODE_NO_PULLS_10 | PINMODE_NO_PULLS_11 | PINMODE_NO_PULLS_12 | PINMODE_NO_PULLS_13 | PINMODE_NO_PULLS_14 | PINMODE_NO_PULLS_15);
    PINSEL8  |= (PINSEL8_P4_0_A0 | PINSEL8_P4_1_A1 | PINSEL8_P4_2_A2 | PINSEL8_P4_3_A3 | PINSEL8_P4_4_A4 | PINSEL8_P4_5_A5 | PINSEL8_P4_6_A6 | PINSEL8_P4_7_A7 | PINSEL8_P4_8_A8 | PINSEL8_P4_9_A9 | PINSEL8_P4_10_A10 | PINSEL8_P4_11_A11 | PINSEL8_P4_12_A12 | PINSEL8_P4_13_A13 | PINSEL8_P4_14_A14); // address lines A0..A14
    PINMODE8 |= (PINMODE_NO_PULLS_0 | PINMODE_NO_PULLS_1 | PINMODE_NO_PULLS_2 | PINMODE_NO_PULLS_3 | PINMODE_NO_PULLS_4 | PINMODE_NO_PULLS_5 | PINMODE_NO_PULLS_6 | PINMODE_NO_PULLS_7 | PINMODE_NO_PULLS_8 | PINMODE_NO_PULLS_9 | PINMODE_NO_PULLS_10 | PINMODE_NO_PULLS_11 | PINMODE_NO_PULLS_12 | PINMODE_NO_PULLS_13 | PINMODE_NO_PULLS_14);
    PINSEL9  |= (PINSEL9_P4_25_WE);
    PINMODE9 |= (PINMODE_NO_PULLS_25);
    
    EMCControl           = EMC_ENABLE;                                   // initialise the SDRAM controller and enable EMC clock
    EMCDynamicReadConfig = READ_DATA_STRATEGY_CMD_DLY;
    EMCDynamicRasCas0    = (RAS_LATENCY_THREE_CCLK | CAS_LATENCY_THREE_CCLK);
    EMCDynamicRP         = CALC_PERIOD(SDRAM_TRP);
    EMCDynamicRAS        = CALC_PERIOD(SDRAM_TRAS);
    EMCDynamicSREX       = CALC_PERIOD(SDRAM_TXSR);
    EMCDynamicAPR        = SDRAM_TAPR;
    EMCDynamicDAL        = SDRAM_TDAL + CALC_PERIOD(SDRAM_TRP);
    EMCDynamicWR         = SDRAM_TWR;
    EMCDynamicRC         = CALC_PERIOD(SDRAM_TRC);
    EMCDynamicRFC        = CALC_PERIOD(SDRAM_TRFC);
    EMCDynamicXSR        = CALC_PERIOD(SDRAM_TXSR);
    EMCDynamicRRD        = CALC_PERIOD(SDRAM_TRRD);
    EMCDynamicMRD        = SDRAM_TMRD;
    EMCDynamicConfig0    = ADDRESS_MAP_16M_16BIT;                        // 13 row, 9 - col, SDRAM

    // SDRAM initialisation sequence
    //
    EMCDynamicControl = (SDRAM_NOP_CMD | DYNAMIC_CLKOUT_CONTINUOUS | DYNAMIC_MEM_CLK_EN);
    ulDelay = 200*30;
    while (ulDelay--) {}
    EMCDynamicControl = (SDRAM_PRECHARGE_ALL_CMD | DYNAMIC_CLKOUT_CONTINUOUS | DYNAMIC_MEM_CLK_EN); // PALL
    EMCDynamicRefresh = 1;
    ulDelay = 128;
    while (ulDelay--) {}                                                 // 128 clock delay
    EMCDynamicRefresh = CALC_PERIOD(SDRAM_REFRESH) >> 4;
    EMCDynamicControl = (SDRAM_MODE_CMD | DYNAMIC_CLKOUT_CONTINUOUS | DYNAMIC_MEM_CLK_EN); // COMM
    ulDelay = *(volatile unsigned short *)fnGetSDRAM((unsigned char *)(SDRAM_ADDR + (0x33UL << (12)))); // burst 8, sequential, CAS-2
    EMCDynamicControl = 0;                                               // NORM
    EMCDynamicConfig0 |= CS_BUFFER_ENABLE;
    #endif
}
#endif

// Initialise the LCD pins and also configure the LCD controller
//
static void fnInitLCD_Controller(void)
{
        #if defined KINETIS_K70                                          // {7}
            #if defined K70F150M_12M
                #define LCD_SHIFT_CLOCK_POLARITY  (0)
                #define H_WIDTH  42
                #define H_WAIT_1 2
                #define H_WAIT_2 83
                #define V_WIDTH  2
                #define V_WAIT_1 1
                #define V_WAIT_2 1
            #elif defined TWR_RGB_LCD_REV_E
                #define LCD_SHIFT_CLOCK_POLARITY  (LCDC_LPCR_CLKPOL)
                #define H_WIDTH  42
                #define H_WAIT_1 2
                #define H_WAIT_2 3
                #define V_WIDTH  2
                #define V_WAIT_1 1
                #define V_WAIT_2 1
            #else
                #define LCD_SHIFT_CLOCK_POLARITY  (0)
                #define H_WIDTH  41
                #define H_WAIT_1 2
                #define H_WAIT_2 3
                #define V_WIDTH  10
                #define V_WAIT_1 2
                #define V_WAIT_2 2
            #endif
    _CONFIG_PERIPHERAL(F, 0, PF_0_GLCD_PCLK);                            // configure the LCD pins
    _CONFIG_PERIPHERAL(F, 1, PF_1_GLCD_DE);
    _CONFIG_PERIPHERAL(F, 2, PF_2_GLCD_HFS);
    _CONFIG_PERIPHERAL(F, 3, PF_3_GLCD_VFS);
    _CONFIG_PERIPHERAL(F, 4, PF_4_GLCD_D0);
    _CONFIG_PERIPHERAL(F, 5, PF_5_GLCD_D1);
    _CONFIG_PERIPHERAL(F, 6, PF_6_GLCD_D2);
    _CONFIG_PERIPHERAL(F, 7, PF_7_GLCD_D3);
    _CONFIG_PERIPHERAL(F, 8, PF_8_GLCD_D4);
    _CONFIG_PERIPHERAL(F, 9, PF_9_GLCD_D5);
    _CONFIG_PERIPHERAL(F, 10, PF_10_GLCD_D6);
    _CONFIG_PERIPHERAL(F, 11, PF_11_GLCD_D7);
    _CONFIG_PERIPHERAL(F, 12, PF_12_GLCD_D8);
    _CONFIG_PERIPHERAL(F, 13, PF_13_GLCD_D9);
    _CONFIG_PERIPHERAL(F, 14, PF_14_GLCD_D10);
    _CONFIG_PERIPHERAL(F, 15, PF_15_GLCD_D11);
    _CONFIG_PERIPHERAL(F, 16, PF_16_GLCD_D12);
    _CONFIG_PERIPHERAL(F, 17, PF_17_GLCD_D13);
    _CONFIG_PERIPHERAL(F, 18, PF_18_GLCD_D14);
    _CONFIG_PERIPHERAL(F, 19, PF_19_GLCD_D15);
    _CONFIG_PERIPHERAL(F, 20, PF_20_GLCD_D16);
    _CONFIG_PERIPHERAL(F, 21, PF_21_GLCD_D17);
    _CONFIG_PERIPHERAL(F, 22, PF_22_GLCD_D18);
    _CONFIG_PERIPHERAL(F, 23, PF_23_GLCD_D19);
    _CONFIG_PERIPHERAL(F, 24, PF_24_GLCD_D20);
    _CONFIG_PERIPHERAL(F, 25, PF_25_GLCD_D21);
    _CONFIG_PERIPHERAL(F, 26, PF_26_GLCD_D22);
    _CONFIG_PERIPHERAL(F, 27, PF_27_GLCD_D23);
        #elif defined LPC1788                                            // {6}
    /*
    Matrix_Arb = 0                                                       // Set AHB Matrix priorities [0..3] with 3 being highest priority
               | (1 <<  0)                                               // PRI_ICODE : I-Code bus priority. Should be lower than PRI_DCODE for proper operation.
               | (2 <<  2)                                               // PRI_DCODE : D-Code bus priority.
               | (0 <<  4)                                               // PRI_SYS   : System bus priority.
               | (0 <<  6)                                               // PRI_GPDMA : General Purpose DMA controller priority.
               | (0 <<  8)                                               // PRI_ETH   : Ethernet: DMA priority.
               | (3 << 10)                                               // PRI_LCD   : LCD DMA priority.
               | (0 << 12)                                               // PRI_USB   : USB DMA priority.
              ;*/

    Matrix_Arb |= (3 << 10);                                             // change just LCD priority

    IOCON_P0_04 = _LCD_P0_4_VD0;                                         // LCD port pin initialisation
    IOCON_P0_05 = _LCD_P0_5_VD1;
    IOCON_P0_06 = _LCD_P0_6_VD8;
    IOCON_P0_07 = _LCD_P0_7_VD9;
    IOCON_P0_08 = _LCD_P0_8_VD16;
    IOCON_P0_09 = _LCD_P0_9_VD17;

    IOCON_P1_20 = _LCD_P1_20_VD10;
    IOCON_P1_21 = _LCD_P1_21_VD11;
    IOCON_P1_22 = _LCD_P1_22_VD12;
    IOCON_P1_23 = _LCD_P1_23_VD13;
    IOCON_P1_24 = _LCD_P1_24_VD14;
    IOCON_P1_25 = _LCD_P1_25_VD15;
    IOCON_P1_26 = _LCD_P1_26_VD20;
    IOCON_P1_27 = _LCD_P1_27_VD21;
    IOCON_P1_28 = _LCD_P1_28_VD22;
    IOCON_P1_29 = _LCD_P1_29_VD23;

    IOCON_P2_02 = _LCD_P2_2_DCLK;
    IOCON_P2_03 = _LCD_P2_3_FP;
    IOCON_P2_04 = _LCD_P2_4_ENAB_M;
    IOCON_P2_05 = _LCD_P2_5_LP;
    IOCON_P2_06 = _LCD_P2_6_VD4;
    IOCON_P2_07 = _LCD_P2_7_VD5;
    IOCON_P2_08 = _LCD_P2_8_VD6;
    IOCON_P2_09 = _LCD_P2_9_VD7;

    IOCON_P2_12 = _LCD_P2_12_VD18;
    IOCON_P2_13 = _LCD_P2_13_VD19;

    IOCON_P4_28 = _LCD_P4_28_VD2;
    IOCON_P4_29 = _LCD_P4_29_VD3;
        #else
    PINSEL0   |= (PINSEL0_P0_4_LCD | PINSEL0_P0_5_LCD | PINSEL0_P0_6_LCD | PINSEL0_P0_7_LCD | PINSEL0_P0_8_LCD | PINSEL0_P0_9_LCD);
    PINMODE0  |= (PINMODE_NO_PULLS_4 | PINMODE_NO_PULLS_5 | PINMODE_NO_PULLS_6 | PINMODE_NO_PULLS_7 | PINMODE_NO_PULLS_8 | PINMODE_NO_PULLS_9);
    PINSEL3   |= (PINSEL3_P1_20_LCD | PINSEL3_P1_21_LCD | PINSEL3_P1_22_LCD | PINSEL3_P1_23_LCD | PINSEL3_P1_24_LCD | PINSEL3_P1_25_LCD | PINSEL3_P1_26_LCD | PINSEL3_P1_27_LCD | PINSEL3_P1_28_LCD | PINSEL3_P1_29_LCD);
            #if defined USB_INTERFACE                                                     // {2}
    PINMODE3  |= (PINMODE_NO_PULLS_20 | PINMODE_NO_PULLS_21 | PINMODE_NO_PULLS_22 | PINMODE_NO_PULLS_23 | PINMODE_NO_PULLS_24 | PINMODE_NO_PULLS_25 | PINMODE_NO_PULLS_26 /*| PINMODE_NO_PULLS_27*/ | PINMODE_NO_PULLS_28 | PINMODE_NO_PULLS_29); // when USB is used, disabling the pull-up on P1-27 causes various problems, therefore just leave it connected...
            #else
    PINMODE3  |= (PINMODE_NO_PULLS_20 | PINMODE_NO_PULLS_21 | PINMODE_NO_PULLS_22 | PINMODE_NO_PULLS_23 | PINMODE_NO_PULLS_24 | PINMODE_NO_PULLS_25 | PINMODE_NO_PULLS_26 | PINMODE_NO_PULLS_27 | PINMODE_NO_PULLS_28 | PINMODE_NO_PULLS_29);
            #endif
    PINSEL4   |= (PINSEL4_P2_0_LCD | PINSEL4_P2_1_LCD | PINSEL4_P2_2_LCD | PINSEL4_P2_3_LCD | PINSEL4_P2_4_LCD | PINSEL4_P2_5_LCD | PINSEL4_P2_6_LCD | PINSEL4_P2_7_LCD | PINSEL4_P2_8_LCD | PINSEL4_P2_9_LCD | PINSEL4_P2_11_LCD | PINSEL4_P2_12_LCD | PINSEL4_P2_13_LCD);
    PINMODE4  |= (PINMODE_NO_PULLS_0 | PINMODE_NO_PULLS_1 | PINMODE_NO_PULLS_2 | PINMODE_NO_PULLS_3 | PINMODE_NO_PULLS_4 | PINMODE_NO_PULLS_5 | PINMODE_NO_PULLS_6 | PINMODE_NO_PULLS_7 | PINMODE_NO_PULLS_8 | PINMODE_NO_PULLS_9 | PINMODE_NO_PULLS_11 | PINMODE_NO_PULLS_12 | PINMODE_NO_PULLS_13);
    PINSEL9   |= (PINSEL9_P4_28_LCD | PINSEL9_P4_29_LCD);
    PINMODE9  |= (PINMODE_NO_PULLS_28 | PINMODE_NO_PULLS_29);
    PINSEL11   = (LCD_MODE_TFT_24_BIT | LCDPE);                          // set TFT mode and enable LCD port
        #endif
        #if defined KINETIS_K70                                          // {7}
    MPU_CESR = 0;                                                        // disable memory protection unit
    LCDC_LSSAR = SDRAM_ADDR;                                             // point the LCD controller to the start address in SDRAM that is used as main window (the complete image must fit in a 4 MB memory boundary)
    LCDC_LSR = (((GLCD_X/16) << 20) | GLCD_Y);                           // specify the height and width of the LCD
    LCDC_LVPWR = GLCD_X;                                                 // virtual page width (the number of 32 bit words required to hold one virtual line - 24 bit mode needs one long word per pixel)
    LCDC_LCPR = 0;                                                       // disable cursor
    LCDC_LCWHB = 0;
            #if defined K70F150M_12M
    LCDC_LPCR = (LCDC_LPCR_TFT_MODE | LCD_SHIFT_CLOCK_POLARITY | LCDC_LPCR_BPIX_24 | LCDC_LPCR_PIXPOL_H | LCDC_LPCR_FLMPOL_L | LCDC_LPCR_LPPOL_L | LCDC_LPCR_OEPOL_H | LCDC_LPCR_SCLKIDLE | LCDC_LPCR_END_SEL_B | LCDC_LPCR_SWAP_SEL | LCDC_LPCR_SCLKSEL | ((MCGOUTCLK/16000000) - 1));
            #else
    LCDC_LPCR = (LCDC_LPCR_TFT_MODE | LCD_SHIFT_CLOCK_POLARITY | LCDC_LPCR_BPIX_24 | LCDC_LPCR_PIXPOL_H | LCDC_LPCR_FLMPOL_L | LCDC_LPCR_LPPOL_L | LCDC_LPCR_OEPOL_H | LCDC_LPCR_SCLKIDLE | LCDC_LPCR_END_SEL_L | LCDC_LPCR_SWAP_SEL | LCDC_LPCR_SCLKSEL | ((MCGOUTCLK/10000000) - 1));
            #endif
    LCDC_LHCR = (((H_WIDTH - 1) << 26) | ((H_WAIT_1 - 1) << 8) | (H_WAIT_2 - 3)); // horizontal sync pulse timing
    LCDC_LVCR = ((V_WIDTH << 26) | (V_WAIT_1 << 8) | (V_WAIT_2));        // vertical sync pulse timing
    LCDC_LPOR = 0;                                                       // LCD panning offfset is not used in 24 bit mode
    LCDC_LICR = 0;                                                       // no interrupts used
    LCDC_LIER = 0;
    LCDC_LGWCR = 0;                                                      // graphic windows disabled
    LCDC_LDCR &= ~(LCDC_LDCR_BURST);                                     // set dynamic burst length for background plane DMA
    LCDC_LGWDCR &= ~(LCDC_LGWDCR_GWBT);                                  // set dynamic burst length for graphic window DMA
        #else
    LCD_CTRL   = (PIXEL_24 | TFT_DISPLAY);                               // configure the LCD controller itself
    LCD_CFG    = (MASTER_CLOCK / PIXEL_CLOCK_RATE);
    LCD_POL    = (BYPASS_PIXEL_CLOCK_DIVIDER | INVERT_VERT_SYNC | INVERT_HORIZ_SYNC | INVERT_PANEL_CLOCK | ((GLCD_X - 1) << 16));
    LCD_TIMH   = (((HORIZONTAL_BACK_PORCH - 1) << 24) | ((HORIZONTAL_FRONT_PORCH - 1) << 16) | ((HORIZONTAL_PULSE - 1) << 8) | (((GLCD_X/16) - 1) << 2)); // horizontal timing
    LCD_TIMV   = ((VERTICAL_BACK_PORCH << 24) | (VERTICAL_FRONT_PORCH << 16) | (VERTICAL_PULSE << 10) | (GLCD_Y - 1)); // vertical timing
    LCD_LPBASE = LCD_UPBASE = (LCD_VRAM_BASE_ADDR & 0xfffffff8);         // frame base double word aligned
        #endif
}
    #endif

    #if defined GLCD_INIT
        #if defined _KINETIS
            POWER_UP(3, SIM_SCGC3_LCDC);                                 // power up the LCD controller
            fnInitLCD_Controller();                                      // initialise the LCD controller
        #else
            POWER_UP(PCEMC | PCLCD);                                     // power up the External Memory Controller and enable LCD controller clock
            fnInit_SDRAM();                                              // initialise the SDRAM controller
            LCD_CTRL &= ~LCD_PWR;                                        // disable power
            uTaskerMonoTimer(OWN_TASK, (DELAY_LIMIT)(0.01 * SEC), E_STABILISE_DELAY);
            iLCD_State = STATE_POWER_LCD_0;
            return;

        case STATE_POWER_LCD_0:
            LCD_CTRL &= ~LCD_EN;
            fnInitLCD_Controller();                                      // initialise the LCD controller
            uTaskerMonoTimer(OWN_TASK, (DELAY_LIMIT)(0.01 * SEC), E_STABILISE_DELAY);
            iLCD_State = STATE_POWER_LCD_1;
            return;
        #endif
        case STATE_POWER_LCD_1:
            {
                unsigned long *ptrSDRAM = (unsigned long *)fnGetSDRAM((unsigned char *)SDRAM_ADDR);
                int i = (GLCD_X * GLCD_Y);
                while (i--) {
                    *ptrSDRAM++ = LCD_ON_COLOUR;
                }
        #if !defined _KINETIS
                LCD_CTRL |= LCD_EN;                                      // enable but don't power until after the stabilising delay
        #endif
                uTaskerMonoTimer(OWN_TASK, (DELAY_LIMIT)(0.01 * SEC), E_STABILISE_DELAY);
                iLCD_State = STATE_POWER_LCD_2;
            }
            return;

        case STATE_POWER_LCD_2:
            fnClearScreen();
        #if defined GLCD_BACKLIGHT_CONTROL
            fnSetBacklight();
        #endif
        #if defined _KINETIS
            SIM_MCR |= SIM_MCR_LCDSTART;                                 // start LCD operation 
        #else
            LCD_CTRL |= LCD_PWR;                                         // enable power
        #endif
        #if defined SUPPORT_TOUCH_SCREEN
            fnStartTouch();                                              // {3} initialise and start touch screen operation
        #endif
    #endif
#endif
