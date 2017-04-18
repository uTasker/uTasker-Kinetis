/**********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher

    ---------------------------------------------------------------------
    File:      GLCD.c [Graphic LCD - mono-color with parallel interface]
    Project:   uTasker project
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2016
    *********************************************************************
    10.08.2009 Rework fnWriteTextBuffer() to handle larger fonts         {1}
    09.08.2009 Add extra graphic functions                               {2}
    26.09.2009 Move to uGLCDLIB folder with Toshiba/Samsung plus OLED, CGLCD and TFT modes {3}
    27.09.2009 Add fnDoLCD_scroll                                        {4}
    27.12.2009 Adjust the alignment of the input buffer to ensure long word alignment {5}
    21.02.2010 Remove old code setting the input buffer (see {5})        {6}
    29.03.2010 Add optional adjustable pixel colour                      {7}
    13.05.2010 Add KITRONIX_GLCD_MODE                                    {8}
    26.06.2010 Add MB785 TFT module                                      {9}
    08.08.2010 Add CGLCD_PIXEL_SIZE to control the GLCD display pixel size {10}
    22.09.2010 Avoid setting invalid window size                         {11}
    23.01.2011 Limit dimension of rectangle to array size                {12}
    05.03.2011 Add windowed picture support                              {13}
    06.03.2011 Change TASK_APPLICATION to LCD_PARTNER_TASK               {14}
    08.03.2011 Increase input buffer size to respect boundary requirements{15}
    21.05.2011 Enable fnDisplayBitmap() generally when SD card active    {16}
    05.09.2012 Enabe fnDisplayBitmap() for SUPPORT_TFT                   {17}
    06.10.2012 Add TFT compatibility with IAR_LPC1788_SK                 {18}
    12.08.2013 Add ST7565S_GLCD_MODE                                     {19}
    12.08.2013 Add inverse mode to bit maps with samsung type display    {20}
    14.08.2013 Protect LCD image uploads to size of the display          {21}
    20.12.2014 Add TWR_LCD_RGB_GLCD_MODE support

*/        

#include "config.h"


#if defined SUPPORT_GLCD && !defined GLCD_COLOR                          // activate this define to support monochrome GLCD

/* =================================================================== */
/*                           include files                             */
/* =================================================================== */

#include "glcd_fonts.h"                                                  // link in the const fonts as used by the project

/* =================================================================== */
/*                          local definitions                          */
/* =================================================================== */

#define MAX_GLCD_READY              100                                  // this many maximum tries will be made to detect the GLCD on power up. If hasn't been detected after this many it will be assumed that no GLCD is connected and the interface doesn't disturb the processor

#include "glcd_samsung.h"                                                // import GLCD (or emulated GLCD) defines
#include "glcd_toshiba.h"
#include "glcd_st7565s.h"                                                // {19}
#include "glcd_oled.h"
#include "glcd_tft.h"
#include "glcd_nokia.h"
#include "glcd_cglcd.h"


#define OWN_TASK                    TASK_LCD

#define STATE_INIT                  0                                    // GLCD task states
#define STATE_INITIALISING          1
#define STATE_POWER_LCD_0           2
#define STATE_POWER_LCD_1           3
#define STATE_POWER_LCD_2           4
#define STATE_INITIALISING_1        5
#define STATE_INITIALISING_2        6
#define STATE_INITIALISING_3        7
#define STATE_INITIALISING_4        8
#define STATE_INITIALISING_5        9
#define STATE_INITIALISING_6        10
#define STATE_LCD_WRITING           11
#define STATE_LCD_CLEARING_DISPLAY  12
#define STATE_LCD_READY             13

#define E_INIT_DELAY                1

#define T_BLINK                     2
#define E_STABILISE_DELAY           3

#define BLINK_NOTHING               0
#define BLINK_NEXT                  1
#define BLINK_RECT                  2
#define BLINK_LINE                  3
#define BLINK_DELETE_ALL            4

#define PEN_STATE_CHANGE            1                                    // interrupt events

#define TOUCH_GET_PEN               0x01
#define TOUCH_POLLING               0x02

#if defined VARIABLE_PIXEL_COLOUR                                        // {7}
    #define _LCD_PIXEL_COLOUR LCD_pixel_colour
    #define _LCD_ON_COLOUR    LCD_on_colour
#else
    #define _LCD_PIXEL_COLOUR LCD_PIXEL_COLOUR
    #define _LCD_ON_COLOUR    LCD_ON_COLOUR
#endif

/* =================================================================== */
/*                      local structure definitions                    */
/* =================================================================== */

typedef struct stLCD_OBJECTS {
    DELAY_LIMIT      blink_interval;
    DELAY_LIMIT      blink_count_down;
    RECT_COORDINATES blink_coordinates;
    unsigned char    ucObjectType;
} LCD_OBJECTS;


/* =================================================================== */
/*                 local function prototype declarations               */
/* =================================================================== */

static int  fnSmartUpdate(int iStart);
static int  fnWritePic(GLCD_PIC *pic);
static int  fnWriteLine(GLCD_LINE_BLINK *line);
static int  fnWriteRect(GLCD_RECT_BLINK *rect);
static int  fnWriteScroll(GLCD_SCROLL *scroll);
#if defined FONTS_AVAILABLE
    static int  fnWriteText(GLCD_TEXT_POSITION *text_position, unsigned char ucLen, QUEUE_HANDLE PortIDInternal);
#endif
#if defined MAX_BLINKING_OBJECTS
    static int  fnConfigureBlink(unsigned char ucObjectType, void *object);
#endif
static void fnClearScreen(void);
#if !defined TFT_GLCD_MODE && !defined CGLCD_GLCD_MODE && !defined KITRONIX_GLCD_MODE && !defined MB785_GLCD_MODE && !defined TFT2N0369_GLCD_MODE
    static int fnCheckLCD(void);
#endif
#if defined SUPPORT_TOUCH_SCREEN && defined MB785_GLCD_MODE
    static void fnHandleTouch(unsigned short usX, unsigned short usY, int iPenDown);
#endif


/* =================================================================== */
/*                             constants                               */
/* =================================================================== */

#if defined SUPPORT_TOUCH_SCREEN && defined MB785_GLCD_MODE              // {9}
    static const unsigned char ucResetInt[]      = {ADD_TOUCH_CONTROLLER_WRITE, STMPE811_INT_STA_OFFSET, (STMPE811_INT_TOUCH_DET)};
    static const unsigned char ucGetTouchData[]  = {ADD_TOUCH_CONTROLLER_WRITE, STMPE811_FIFO_STA_OFFSET}; // read a block of touch data values
    static const unsigned char ucGetData[]       = {7, ADD_TOUCH_CONTROLLER_READ, OWN_TASK}; // read 7 bytes from this address and wake the task when the data is available
    static const unsigned char ucGetTouchState[] = {ADD_TOUCH_CONTROLLER_WRITE, STMPE811_TSC_CTRL_OFFSET}; // first read the touch screen controller state to ensure that a detection exists
    static const unsigned char ucGetValue[]      = {1, ADD_TOUCH_CONTROLLER_READ, OWN_TASK}; // read 1 byte from this address to ensure that a touch has been detected
    static const unsigned char ucResetFIFO1[]    = {ADD_TOUCH_CONTROLLER_WRITE, STMPE811_FIFO_STA_OFFSET, (FIFO_STA_FIFO_RESET)}; // reset FIFO buffer
    static const unsigned char ucResetFIFO2[]    = {ADD_TOUCH_CONTROLLER_WRITE, STMPE811_FIFO_STA_OFFSET, (0)}; // take FIFO buffer out of reset
#endif


/* =================================================================== */
/*                     global variable definitions                     */
/* =================================================================== */

/* =================================================================== */
/*                      local variable definitions                     */
/* =================================================================== */

#if defined VARIABLE_PIXEL_COLOUR                                         // {7}
    static COLORREF LCD_pixel_colour = LCD_PIXEL_COLOUR;
    static COLORREF LCD_on_colour = LCD_ON_COLOUR;
#endif
static unsigned char ucPixelArray[Y_BYTES][X_BYTES];
static unsigned char ucByteUpdateArray[UPDATE_HEIGHT][UPDATE_WIDTH];
static int iSendAck = 0;
static int iLCD_State = STATE_INIT;
#if defined SUPPORT_TOUCH_SCREEN && defined MB785_GLCD_MODE              // {9}
    static QUEUE_HANDLE TouchPortID = 0;
    static int iTouchState = 0;
    static int iPenDown = 0;
#endif


// The GLCD task
//
extern void fnLCD(TTASKTABLE *ptrTaskTable)                              // LCD task
{
#if defined SUPPORT_TOUCH_SCREEN && defined MB785_GLCD_MODE 
    static unsigned char _ucInputMessage[HEADER_LENGTH + 3 + 8 + 7 + 20];// {15}
#else
    static unsigned char _ucInputMessage[HEADER_LENGTH + 3 + 8 + 7];     // {5}{15} reserve space for receiving message headers and extension data
#endif
    static unsigned char *ucInputMessage;
    static unsigned char  ucPictureBoundary = 1;
    QUEUE_HANDLE PortIDInternal = ptrTaskTable->TaskID;                  // queue ID for task input

    if (iLCD_State != STATE_LCD_READY) {
        if ((iLCD_State > STATE_INITIALISING) && (GLCD_BUSY())) {
            return;                                                      // LCD is busy so continue waiting
        }
        switch (iLCD_State) {
        case STATE_INIT:                                                 // initialise the LCD hardware once on startup - reset is assumed to be held low up to this point
            {
            GLCD_PIC *pic;                                               // {6}
            ucInputMessage = _ucInputMessage;
            if (((CAST_POINTER_ARITHMETIC)ucInputMessage & 0x1) == 0) {
                ucInputMessage++;                                        // the input buffer must start on an uneven boundary to ensure struct alignment of read messages
            }
            pic = (GLCD_PIC *)&ucInputMessage[1];                        // since pictures use pointers an additional picture alignment may be necessary too
            while (((CAST_POINTER_ARITHMETIC)&pic->ptrPic & 0x3) != 0) { // {5}{6}
                pic = (GLCD_PIC *)&ucInputMessage[++ucPictureBoundary];  // try next
            }
#if defined TFT_GLCD_MODE                                                // only LPC24xx/LPC17xx/TWR_K70F120M
            #define GLCD_INIT
            #include "glcd_tft.h"                                        // include initialisation code
            #undef GLCD_INIT
#elif (defined CGLCD_GLCD_MODE && (defined AVR32_EVK1105 || defined EK_LM3S3748)) || defined KITRONIX_GLCD_MODE || defined MB785_GLCD_MODE || defined TFT2N0369_GLCD_MODE
            #define GLCD_INIT
            #include "glcd_cglcd.h"                                      // include initialisation code
            #undef GLCD_INIT
#else
            GLCD_RST_H();                                                // release the reset line
            if (fnCheckLCD() != 0) {
                fnEventMessage(LCD_PARTNER_TASK, TASK_LCD, E_LCD_ERROR); // inform the application that the GLCD has not been detected
                return;
            }
    #if defined OLED_GLCD_MODE
            #define GLCD_INIT
            #include "glcd_oled.h"                                       // include initialisation code
            #undef GLCD_INIT
    #elif defined NOKIA_GLCD_MODE
            #define GLCD_INIT
            #include "glcd_nokia.h"                                      // include initialisation code
            #undef GLCD_INIT
    #elif defined _GLCD_SAMSUNG                                          // Samsung controller
          //while (GLCD_BUSY()) {};
            fnWriteGLCD_cmd(GLCD_ON, GLCD_CHIP0);                        // power on first chip
        #if SAMSUNG_CHIPS > 1
            while (GLCD_BUSY()) {};
            fnWriteGLCD_cmd(GLCD_ON, GLCD_CHIP1);                        // power on second chip
        #endif
            while (GLCD_BUSY()) {};
            fnWriteGLCD_cmd(GLCD_DISP_START, GLCD_CHIP0);
        #if SAMSUNG_CHIPS > 1
            while (GLCD_BUSY()) {};
            fnWriteGLCD_cmd(GLCD_DISP_START, GLCD_CHIP1);
        #endif  
            fnClearScreen();                                             // clear display               
    #elif defined ST7565S_GLCD_MODE                                      // {19}
            fnWriteGLCD_cmd(GLCD_RESET);                                 // reset	            (0xe2)
            fnWriteGLCD_cmd(GLCD_ON);                                    // display on          (0xaf)
            fnWriteGLCD_cmd(GLCD_POWER_ON);                              // power on            (0x2f)
            fnWriteGLCD_cmd(GLCD_VERT_INV);                              //                     (0xc8)
            fnWriteGLCD_cmd(GLCD_DEFAULT_CONTRAST);                      // default contrast	(0x26)                                                                                                  
            fnWriteGLCD_cmd(GLCD_DISP_START);
            fnClearScreen();                                             // clear display               
        #if defined GLCD_BACKLIGHT_CONTROL
            fnSetBacklight();
        #endif
    #else                                                                // Toshiba controller
            fnCommandGlcd_2(SET_GRAPHICS_HOME_ADDRESS, 0x0000);          // set graphics start at zero
            fnCommandGlcd_2(SET_GRAPHICS_HORIZ_BYTES, X_BYTES);          // set the number of horizontal bytes
            fnCommandGlcd_2(SET_TEXT_HOME_ADDRESS, X_BYTES*GLCD_Y);      // set text start after graphics memory
            fnCommandGlcd_2(SET_TEXT_HORIZ_BYTES, X_BYTES);              // set number of text characters on a row
            fnClearScreen();
            while (GLCD_BUSY()) {}                                       // wait until the GLCD can accept the next command
            fnWriteGLCD_cmd(GRAPHIC_MODE_XOR);                           // set graphic mode
            while (GLCD_BUSY()) {}                                       // wait until the GLCD can accept the next command
            fnWriteGLCD_cmd(DISABLE_TEXT_DISABLE_CURSOR);                // set graphic mode
            fnCommandGlcd_2(SET_ADDRESS_POINTER, 0x0000);                // set the address pointer to the start of graphic memory
    #endif
#endif                                                                   
            fnEventMessage(LCD_PARTNER_TASK, TASK_LCD, E_LCD_INITIALISED); // inform the application that the initialisation has completed
            iLCD_State = STATE_LCD_READY;                                // we are ready to work....
            }
            break; 

        case STATE_LCD_WRITING:
            iLCD_State = fnSmartUpdate(0);                               // continue with display updating
            if ((iLCD_State == STATE_LCD_READY) && (iSendAck != 0)) {    // if the write has completed
                fnEventMessage(LCD_PARTNER_TASK, TASK_LCD, E_LCD_READY);
                iSendAck = 0;
            }
            break;

#if defined NOKIA_GLCD_MODE || defined CGLCD_GLCD_MODE || defined KITRONIX_GLCD_MODE || defined TFT2N0369_GLCD_MODE || defined MB785_GLCD_MODE
        case STATE_LCD_CLEARING_DISPLAY:
            iLCD_State = fnSmartUpdate(0);                               // continue with display updating
            if (iLCD_State == STATE_LCD_READY) {                         // if the write has completed
    #if defined NOKIA_GLCD_MODE
                WriteSpiCommand(DISON);                                  // display
    #elif defined EK_LM3S3748
                fnWriteGLCD_cmd(ST7637_NORMAL_ON);                       // set the display for normal operation 
    #elif defined MB785_GLCD_MODE
                fnCommandGlcd_2(DISPLAY_CONTROL_1, 0x0173);              // 262k color and display ON
    #endif
                ENABLE_BACKLIGHT();                                      // and enable backlight
                fnEventMessage(LCD_PARTNER_TASK, TASK_LCD, E_LCD_INITIALISED); // inform the application that the initialisation has completed
    #if defined SUPPORT_TOUCH_SCREEN
                fnStartTouch();
    #endif
    #if defined GLCD_BACKLIGHT_CONTROL
                fnSetBacklight();
    #endif
              //iLCD_State = STATE_LCD_READY;                            // we are ready to work....
            }
            else {
                iLCD_State = STATE_LCD_CLEARING_DISPLAY;                 // still in the process or clearing display
            }
            break;
#endif
        }

        if (iLCD_State != STATE_LCD_READY) {
            return;
        }
    }
#if defined SUPPORT_TOUCH_SCREEN && defined MB785_GLCD_MODE              // touch screen via I2C
    if (iTouchState != 0) {
        while (fnMsgs(TouchPortID) != 0) {                               // touch screen message(s) available
            switch (iTouchState) {
            case TOUCH_GET_PEN:                                          // requesting first pen state
                fnWrite(TouchPortID, (unsigned char *)ucResetInt, sizeof(ucResetInt)); // reset the  pending interrupt bit
                fnRead(TouchPortID, ucInputMessage, 1);                  // read the state
                if (ucInputMessage[0] & TCS_CTRL_TOUCH_DETECT) {
                    fnWrite(TouchPortID, (unsigned char *)ucGetTouchData, sizeof(ucGetTouchData)); // set the read address
                    fnRead(TouchPortID,  (unsigned char *)ucGetData, 0);     // start the read process of the block of data
                    fnDebugMsg("PEN DOWN\n\r");
                    iTouchState = TOUCH_POLLING;
                    iPenDown = 1;
                }
                else {
                    iTouchState = 0;
                    fnWrite(TouchPortID, (unsigned char *)ucResetFIFO1, sizeof(ucResetFIFO1)); // flush FIFO
                    fnWrite(TouchPortID, (unsigned char *)ucResetFIFO2, sizeof(ucResetFIFO2)); // release flush FIFO
                    fnDebugMsg("PEN UP\n\r");
                }
                break;
            case (TOUCH_GET_PEN | TOUCH_POLLING):                        // requesting touch location with queued state
                iTouchState = TOUCH_GET_PEN;
                fnWrite(TouchPortID, (unsigned char *)ucGetTouchState, sizeof(ucGetTouchState)); // set the read address
                fnRead(TouchPortID,  (unsigned char *)ucGetValue,      0);   // start the read process of 1 byte
            case TOUCH_POLLING:                                          // request touch location
                {
                  //int x = 0;
                  //QUEUE_TRANSFER Length = fnRead(TouchPortID, ucInputMessage, 7);
                    fnRead(TouchPortID, ucInputMessage, 7);
                    if (!(ucInputMessage[0] & FIFO_STA_FIFO_EMPTY)) {    // only when valid touch position
                        unsigned short usY = ((ucInputMessage[2] << 8) + ucInputMessage[3]);
                        unsigned short usX = ((ucInputMessage[4] << 8) + ucInputMessage[5]);
                        fnHandleTouch(usX, usY, iPenDown);
                        iPenDown = 0;                                    // reset reporting of new pen-down
                      //while (x < Length) {                             // display received bytes
                      //    fnDebugHex(ucInputMessage[x++], (WITH_LEADIN | WITH_SPACE | 1));
                      //}
                      //fnDebugMsg("\r\n");
                    }
                    if (iTouchState == TOUCH_POLLING) {
                        fnWrite(TouchPortID, (unsigned char *)ucGetTouchData, sizeof(ucGetTouchData)); // set the read address
                        fnRead(TouchPortID,  (unsigned char *)ucGetData, 0);     // start the read process of the block of data
                    }
                }
                break;
            }
        }
    }
#endif
    while (fnRead(PortIDInternal, ucInputMessage, HEADER_LENGTH)) {      // check input queue - this only occurs when all previous operation has completed
        switch (ucInputMessage[MSG_SOURCE_TASK]) {
        case TIMER_EVENT:
#if defined MAX_BLINKING_OBJECTS
            if (T_BLINK == ucInputMessage[MSG_TIMER_EVENT]) {            // blink timer fired
                fnConfigureBlink(BLINK_NEXT, 0);                         // next blink
            }
            break;
#endif
        case INTERRUPT_EVENT:
#if defined SUPPORT_TOUCH_SCREEN && defined MB785_GLCD_MODE
            if (PEN_STATE_CHANGE == ucInputMessage[MSG_INTERRUPT_EVENT]) {
                if ((iTouchState & TOUCH_GET_PEN) == 0) {                // if not already in the process of requesting the state
                    if (iTouchState == TOUCH_POLLING) {                  // if pen is down and the movement is being polled
                        iTouchState = (TOUCH_POLLING | TOUCH_GET_PEN);   // mark that the pen state is to be requested
                    }
                    else {
                        fnWrite(TouchPortID, (unsigned char *)ucGetTouchState, sizeof(ucGetTouchState)); // set the read address
                        fnRead(TouchPortID,  (unsigned char *)ucGetValue, 0); // start the read process of 1 byte
                        iTouchState = TOUCH_GET_PEN;                     // expecting the pen state
                    }
                }
                break;
            }
#endif
            break;
        case LCD_PARTNER_TASK:                                            // {14}
            {
                unsigned char ucLen = (ucInputMessage[MSG_CONTENT_LENGTH] - 2);
                fnRead(PortIDInternal, ucInputMessage, 2);               // read the message type and mode
                switch (ucInputMessage[0]) {
#if defined FONTS_AVAILABLE
                case E_LCD_TEXT:
                    {
                        GLCD_TEXT_POSITION *text_position = (GLCD_TEXT_POSITION *)&ucInputMessage[1];
                        if (text_position->ucMode & GIVE_ACK) {          // does the application expect an ack when this operation has completed?
                            iSendAck = 1;
                        }
                        fnRead(PortIDInternal, &ucInputMessage[2], (sizeof(GLCD_TEXT_POSITION) - 1));
                        if ((iLCD_State = fnWriteText(text_position, (unsigned char)(ucLen - (sizeof(GLCD_TEXT_POSITION) - 1)), PortIDInternal)) != STATE_LCD_READY) { // start text write
                            return;
                        }
                    }
                    break;
#endif
#if defined GLCD_PIC_WINDOWING
                case E_LCD_WINDOWED_PIC:                                 // {13}
#endif
                case E_LCD_PIC:
                    {
                        GLCD_PIC *pic = (GLCD_PIC *)&ucInputMessage[ucPictureBoundary];
                        ucInputMessage[ucPictureBoundary] = ucInputMessage[1];
                        if (pic->ucMode & GIVE_ACK) {                    // does the application expect an ack when this operation has completed?
                            iSendAck = 1;
                        }
                        fnRead(PortIDInternal, &ucInputMessage[ucPictureBoundary + 1], ucLen);
                        if ((iLCD_State = fnWritePic(pic)) != STATE_LCD_READY) { // start text image
                            return;
                        }
                    }
                    break;
                case E_LCD_LINE:                                         // {1}
                    {
                        GLCD_LINE_BLINK *line = (GLCD_LINE_BLINK *)&ucInputMessage[1];
                        if (line->ucMode & GIVE_ACK) {                   // does the application expect an ack when this operation has completed?
                            iSendAck = 1;
                        }
                        fnRead(PortIDInternal, &ucInputMessage[2], ucLen);
#if defined MAX_BLINKING_OBJECTS
                        if (line->ucMode & (BLINKING_OBJECT)) {
                            fnConfigureBlink(BLINK_LINE, line);
                        }
#endif
                        if ((iLCD_State = fnWriteLine(line)) != STATE_LCD_READY) { // start drawing line
                            return;
                        }
                    }
                    break;
                case E_LCD_RECT:                                         // {1}
                    {
                        GLCD_RECT_BLINK *rect = (GLCD_RECT_BLINK *)&ucInputMessage[1];
                        if (rect->ucMode & GIVE_ACK) {                   // does the application expect an ack when this operation has completed?
                            iSendAck = 1;
                        }
                        fnRead(PortIDInternal, &ucInputMessage[2], ucLen);
#if defined MAX_BLINKING_OBJECTS
                        if (rect->ucMode & (BLINKING_OBJECT)) {
                            fnConfigureBlink(BLINK_RECT, rect);
                        }
#endif
                        if ((iLCD_State = fnWriteRect(rect)) != STATE_LCD_READY) { // start drawing rectangle
                            return;
                        }
                    }
                    break;
                case E_LCD_SCROLL:
                    {
                        GLCD_SCROLL *scroll = (GLCD_SCROLL *)&ucInputMessage[1];
                        if (scroll->ucMode & GIVE_ACK) {                 // does the application expect an ack when this operation has completed?
                            iSendAck = 1;
                        }
                        fnRead(PortIDInternal, &ucInputMessage[2], ucLen);
                        if ((iLCD_State = fnWriteScroll(scroll)) != STATE_LCD_READY) { // start drawing scrolled display
                            return;
                        }
                    }
                    break;
#if defined VARIABLE_PIXEL_COLOUR
                case E_LCD_STYLE:
                    {
                        GLCD_STYLE *style = (GLCD_STYLE *)&ucInputMessage[1];
                        fnRead(PortIDInternal, &ucInputMessage[2], ucLen);
                        switch (style->ucMode) {
                        case STYLE_PIXEL_COLOR:
                            uMemcpy(&LCD_pixel_colour, &style->color, sizeof(LCD_pixel_colour));
                            break;
                        case STYLE_BACKGROUND_COLOR:
                            uMemcpy(&LCD_on_colour, &style->color, sizeof(LCD_pixel_colour));
                        }
                    }
                    break;
#endif
                }
                if (iSendAck != 0) {
                    iSendAck = 0;
                    fnEventMessage(LCD_PARTNER_TASK, TASK_LCD, E_LCD_READY); // last operation has been completed
                }
            }
            break;
        default:
            while (fnRead( PortIDInternal, ucInputMessage, 1) != 0) {};  // flush unexpected messages
            break;
        }
    }
}

#define _GLCD_COMMANDS                                                   // hardware interface commands depending on display type
    #include "glcd_samsung.h"
    #include "glcd_toshiba.h"
    #include "glcd_oled.h"
    #include "glcd_tft.h"
    #include "glcd_nokia.h"
    #include "glcd_cglcd.h"
#undef _GLCD_COMMANDS


// Bit map writing routine, used for application bit maps and also for font based characters
//
static void fnWriteBitMap(unsigned short x, unsigned short y,
               unsigned short usWidth, unsigned short usHeight,
               const unsigned char *glyph_ptr, unsigned char ucMode,
               RECT_COORDINATES *window)
{
#if (defined _GLCD_SAMSUNG || defined ST7565S_GLCD_MODE) && !defined OLED_GLCD_MODE && !defined TFT_GLCD_MODE && !defined NOKIA_GLCD_MODE && !defined CGLCD_GLCD_MODE && !defined KITRONIX_GLCD_MODE && !defined TFT2N0369_GLCD_MODE && !defined MB785_GLCD_MODE // {19}
    register unsigned char ucMemoryContent;
    register unsigned char ucOriginal_content;
    int iX = x;
    int iY = y/8;
    int iDotY = (0x01 << (y%8));
    int iDotX = 0x80;
    int iWidthScan;
    while (usHeight--) {
        int iProcessed = 0;
        iWidthScan = usWidth;
        iX = x;
        while (iWidthScan--) {
            if (iX < X_BYTES) {                                          // clip to right
                ucMemoryContent = ucOriginal_content = ucPixelArray[iY][iX];
                if ((window == 0) || (((window->usX_start <= iX) && (window->usX_end >= iX)) && ((window->usY_start <= y) && (window->usY_end >= y)))) { // {13} if a window is defined write only within its bounds
                    if (*glyph_ptr & iDotX) {
                        if (ucMode & PAINT_DARK) {                       // {20}
                            ucMemoryContent &= (unsigned char)~iDotY;    // clear the pixel
                        }
                        else {
                            ucMemoryContent |= (unsigned char)iDotY;     // set the pixel
                        }
                    }
                    else {
                        if (ucMode & PAINT_DARK) {                       // {20}
                            ucMemoryContent |= (unsigned char)iDotY;     // set the pixel
                        }
                        else {
                            ucMemoryContent &= (unsigned char)~iDotY;    // clear the pixel
                        }
                    }
                    if (ucMemoryContent != ucOriginal_content) {         // if change to the memory
                        ucPixelArray[iY][iX] = ucMemoryContent;          // write the change to the pixel array
                        ucByteUpdateArray[iY/8][iX] |= (0x01 << iY);     // mark that this byte needs to be updated
                    }
                }
                iX++;
            }
            iDotX >>= 1;
            if (iDotX == 0) {
                iDotX = 0x80;
            }
            if (++iProcessed >= 8) {
                glyph_ptr++;
                iDotX = 0x80;
                iProcessed = 0;
            }
        }
        y++;
        iDotY <<= 1;
        if (iDotY > 0x80) {
            iDotY = 0x01;
            iY++;
            if (iY >= Y_BYTES) {
                return;                                                  // clipping at bottom
            }
        }
        if (iProcessed != 0) {
            glyph_ptr++;
            iDotX = 0x80;
            iProcessed = 0;
        }
    }
#else
    register unsigned char ucMemoryContent;
    register unsigned char ucOriginal_content;
    int iX_array;
    int iX_bytewidth;
    int iX_offset;

    iX_array = (x/8);
    iX_offset = (x%8);
    iX_bytewidth = ((iX_offset + usWidth + 7)/8);

    while (usHeight--) {                                                 // insert the text, overwriting existing content
        int i = 0;
        int iCharacterOffset = iX_offset;
        int iRemaining = usWidth;
        int iProcessed;
        unsigned char ucMask;
        if (y >= Y_BYTES) {
            break;
        }
        while ((i < iX_bytewidth) && ((iX_array + i) < X_BYTES)) {
            ucMask = 0xff;
            ucOriginal_content = ucMemoryContent = ucPixelArray[y][iX_array + i];
            if (iCharacterOffset >= 0) {
                ucMask <<= (8 - iCharacterOffset);
                if ((iRemaining + iCharacterOffset) < 8) {
                    ucMask |= (0xff >> (iRemaining + iCharacterOffset));
                    iProcessed = 8;
                }
                else {
                    iRemaining -= (8 - iCharacterOffset);                // the remaining pixels in the x-direction
                    if (iRemaining == 0) {
                        iProcessed = 8;
                    }
                    else {
                        iProcessed = (8 - iCharacterOffset);
                    }
                }
                ucMemoryContent &= ucMask;                               // clear space for the new text pixels
                if (ucMode & PAINT_DARK) {
                    ucMemoryContent |= ((~(*glyph_ptr) >> iCharacterOffset) & ~ucMask); // insert new pixel content
                }
                else {
                    ucMemoryContent |= ((*glyph_ptr >> iCharacterOffset) & ~ucMask); // insert new pixel content
                }
                ucPixelArray[y][iX_array + i] = ucMemoryContent;
                if (ucMemoryContent != ucOriginal_content) {
                    ucByteUpdateArray[y][(iX_array + i)/8] |= (0x80 >> ((iX_array + i)%8)); // mark that this byte needs to be updated
                }
            }
            else {
                if (iRemaining < (-iCharacterOffset)) {
                    ucMask >>= iRemaining;
                }
                else {
                    ucMask >>= (-iCharacterOffset);
                }
                ucMemoryContent &= ucMask;                               // clear space for the new text pixels
                if (ucMode & PAINT_DARK) {
                    ucMemoryContent |= ((~(*glyph_ptr++) << (8 + iCharacterOffset)) & ~ucMask); // insert new pixel content
                }
                else {
                    ucMemoryContent |= ((*glyph_ptr++ << (8 + iCharacterOffset)) & ~ucMask); // insert new pixel content
                }
                iCharacterOffset = -iCharacterOffset;
                ucPixelArray[y][iX_array + i] = ucMemoryContent;
                if (ucMemoryContent != ucOriginal_content) {
                    ucByteUpdateArray[y][(iX_array + i)/8] |= (0x80 >> ((iX_array + i)%8)); // mark that this byte needs to be updated
                }
                if (iCharacterOffset >= iRemaining) {
                    break;                                               // work done for this row
                }
                iRemaining -= iCharacterOffset;                          // the remaining pixels in the x-direction
                continue;
            }
            if (iProcessed >= 8) {
                glyph_ptr++;
            }
            i++;
            iCharacterOffset = -iCharacterOffset;
        }
        y++;
        if ((iX_array + i) >= X_BYTES) {                                 // check for bitmap clipping to right
            while (i < iX_bytewidth) { 
                glyph_ptr++;                                             // skip non-visible input
                i++;
            }
        }
    }
#endif
}


#if defined FONTS_AVAILABLE
// Write text to the backup buffer at the coordinate using defined font
//
static void fnWriteTextBuffer(GLCD_TEXT_POSITION *text_position, CHAR *str, unsigned char ucStrLength)
{
    unsigned char font = text_position->ucFont;
    unsigned short x = text_position->usX;
    unsigned char ucGlyph;
    unsigned char ucWidth;
    unsigned char ucHeight;
    const unsigned char *glyph_ptr;

    while (ucStrLength--) {                                              // for each string character
        ucGlyph = (unsigned char)*str++;
        if ((ucGlyph < fonts[font].glyph_beg) || (ucGlyph > fonts[font].glyph_end)) { // check to make sure the symbol is a legal one if not then just replace it with the default character 
            ucGlyph = fonts[font].glyph_def;                             // replace by default
        }
        ucGlyph -= fonts[font].glyph_beg;                                // array reference             
        ucHeight = fonts[font].glyph_height;
        glyph_ptr = fonts[font].glyph_table + (ucGlyph * ucHeight);
        if ((ucWidth = fonts[font].fixed_width) == 0) {                  // check if it is a fixed width 
            ucWidth = (fonts[font].width_table[ucGlyph] & 0xff);         // get the variable width instead 
            glyph_ptr += ((fonts[font].width_table[ucGlyph] >> 8) * ucHeight); // compensate for font widths of more than one byte
        }
        else if (fonts[font].width_table != 0) {                        
            glyph_ptr += ((fonts[font].width_table[ucGlyph] >> 8) * ucHeight); // compensate for font widths of more than one byte
            if ((fonts[font].width_table[ucGlyph] & 0xff) > ucWidth) {   // exception - the fixed with is less than real character width so widen
                ucWidth = (fonts[font].width_table[ucGlyph] & 0xff);     // get the variable width instead 
            }
        }
        else if (fonts[font].fixed_width > 8) {                          // if fixed width is more that 8 bits
            glyph_ptr += (ucGlyph * (ucHeight * ((fonts[font].fixed_width)/8)));
        }
        fnWriteBitMap(x, text_position->usY, ucWidth, ucHeight, glyph_ptr, text_position->ucMode, 0);
        x += ucWidth;                                                    // move right for next character                                                                           // point to next character in string 
    }
}

// Read text from the input queue to an intermediate buffer.
//
static int fnWriteText(GLCD_TEXT_POSITION *text_position, unsigned char ucLen, QUEUE_HANDLE PortIDInternal)
{
    CHAR cNewText[MAX_TEXT_LENGTH];                                      // space for the maximum expected text string plus a null terminator
    unsigned char ucLength = ucLen;
    if (ucLength > MAX_TEXT_LENGTH) {                                    // ensure text can not overwrite buffer end
        ucLength = MAX_TEXT_LENGTH;
    }
    fnRead(PortIDInternal, (unsigned char *)cNewText, ucLength);
    while (ucLen > ucLength) {
        unsigned char ucDummy;
        fnRead(PortIDInternal, &ucDummy, 1);                             // dummy read of any excessive text
        ucLen--;
    }
    fnWriteTextBuffer(text_position, cNewText, ucLength);                // convert the ASCII input buffer to characters defined by the font. These are written to the backup graphics buffer
    if (text_position->ucMode & REDRAW) {
        return (fnSmartUpdate(1));                                       // redraw GLCD is commanded to do so
    }
    return STATE_LCD_READY;                                              // operation complete
}
#endif

// Write a bit map as commanded by the application
//
static int fnWritePic(GLCD_PIC *pic)
{
    RECT_COORDINATES *rect_corners = 0;                                  // the window in the picture
    unsigned short usWidth  = ((pic->ptrPic->ucWidth[0] << 8)  | pic->ptrPic->ucWidth[1]);
    unsigned short usHeight = ((pic->ptrPic->ucHeight[0] << 8) | pic->ptrPic->ucHeight[1]);
#if defined GLCD_PIC_WINDOWING
    int iIgnore = 0;
    if (pic->ucMode & WINDOWED_PIC) {                                    // {13} only a windows from the picture is to be displayed
        rect_corners = &((GLCD_PIC_WINDOW *)pic)->rect_corners;          // the window in the picture
        if ((rect_corners->usX_start >= rect_corners->usX_end) || (rect_corners->usY_start >= rect_corners->usY_end)) {
            iIgnore = 1;
        }
        else {
            rect_corners->usX_start += pic->usX;                             // set relative to picture content
            rect_corners->usX_end += pic->usX;
            rect_corners->usY_start += pic->usY;
            rect_corners->usY_end += pic->usY;
        }
    }
    if (iIgnore == 0) {                                                  // don't write if the window is invalid
        fnWriteBitMap(pic->usX, pic->usY, usWidth, usHeight, pic->ptrPic->ucData, pic->ucMode, rect_corners); // copy bit map to backup graphics buffer
    }
#else
    fnWriteBitMap(pic->usX, pic->usY, usWidth, usHeight, pic->ptrPic->ucData, pic->ucMode, rect_corners); // copy bit map to backup graphics buffer
#endif
    if (pic->ucMode & REDRAW) {
        return (fnSmartUpdate(1));                                       // redraw GLCD is commanded to do so
    }
    return STATE_LCD_READY;                                              // operation complete
}

// Draw a line as commanded by the application
//
static int fnWriteLine(GLCD_LINE_BLINK *line)                            // {1}
{
    int iHorizontalDirection;
    int iX_steps = 0x100000;
    int iY_steps = 0x100000;
    int iYpixels, iXpixels;
    unsigned short usX_Start, usY_Start, usX_End, usY_End;
    unsigned char ucPixel;
    unsigned char ucOriginalMemoryContent;
    if (line->line_start_end.usY_start <= line->line_start_end.usY_end) {// always draw from top to bottom
        usY_Start = line->line_start_end.usY_start;
        usY_End   = line->line_start_end.usY_end;
        usX_Start = line->line_start_end.usX_start;
        usX_End   = line->line_start_end.usX_end;
    }
    else {
        usY_Start = line->line_start_end.usY_end;
        usY_End   = line->line_start_end.usY_start;
        usX_Start = line->line_start_end.usX_end;
        usX_End   = line->line_start_end.usX_start;
    }
#if (defined _GLCD_SAMSUNG || defined ST7565S_GLCD_MODE) && !defined OLED_GLCD_MODE && !defined TFT_GLCD_MODE && !defined NOKIA_GLCD_MODE && !defined CGLCD_GLCD_MODE && !defined KITRONIX_GLCD_MODE && !defined TFT2N0369_GLCD_MODE && !defined MB785_GLCD_MODE // {19}    
    iY_steps = (usY_End - usY_Start);                                    // height
    if (usX_Start <= usX_End) {
        iHorizontalDirection = 1;                                        // draw from left to right        
        if (usX_End == usX_Start) {
            iY_steps = 0x7fffffff;                                       // vertical line
        }
        else {
            if ((usX_End - usX_Start) <= iY_steps) {
                iY_steps *= 0x100000;
                iY_steps /= (usX_End - usX_Start);
            }
            else {
                if (usY_End == usY_Start) {
                    iX_steps = 0x7fffffff;                               // horizontal line
                }
                else {
                    iX_steps = (usX_End - usX_Start);                    // width
                    iX_steps *= 0x100000;
                    iX_steps /= iY_steps;
                }
                iY_steps = 0x100000;
            }
        }
    }
    else {
        iHorizontalDirection = -1;                                       // draw from right to left
        if ((usX_Start - usX_End) <= iY_steps) {
            if (usX_Start == usX_End) {                                  // vertical line
                iY_steps = 0x7fffffff;
            }
            else {
                iY_steps *= 0x100000;
                iY_steps /= (usX_Start - usX_End);
            }
            iX_steps = 0x100000;
        }
        else {
            if (iY_steps == 0) {                                         // horizontal line
                iX_steps = 0x7fffffff;
            }
            else {
                iX_steps = (usX_Start - usX_End);                        // width
                iX_steps *= 0x100000;
                iX_steps /= iY_steps;
            }
            iY_steps = 0x100000;
        }
    }
    iYpixels = iY_steps;
    iXpixels = iX_steps;                                                 // the number of pixels to draw in the horizontal direction (left or right)
    do {
        if (usY_Start >= GLCD_Y) {
            break;
        }
        do {
            if (usX_Start >= GLCD_X) {
                break;
            }
            if (iHorizontalDirection > 0) {
                if (usX_Start > usX_End) {
                    break;
                }
            }
            else {
                if (usX_Start < usX_End) {
                    break;
                }
            }
            ucPixel = (0x01 << (usY_Start%8));                           // position of the pixel to be drawn
            ucOriginalMemoryContent = ucPixelArray[usY_Start/8][usX_Start]; // get the original content
            if (line->ucMode & (PAINT_INVERTED | PAINT_DARK)) {
                if (line->ucMode & PAINT_DARK) {
                    ucPixelArray[usY_Start/8][usX_Start] &= ~ucPixel;    // clear the pixel
                }
                else {
                    ucPixelArray[usY_Start/8][usX_Start] = (ucOriginalMemoryContent ^ ucPixel); // set the pixel to the inverted value of background
                }
            }
            else {                                                       // default use
                ucPixelArray[usY_Start/8][usX_Start] |= ucPixel;         // set the pixel
            }
            if (ucPixelArray[usY_Start/8][usX_Start] != ucOriginalMemoryContent) { // check whether it was indeed modified
                ucByteUpdateArray[usY_Start/64][usX_Start] |= (0x01 << (usY_Start/8));// mark that this pixel byte needs to be updated
            }
            iYpixels -= 0x100000;
            if (iYpixels > 0) {
                break;
            }
            iYpixels += iY_steps;
            usX_Start += iHorizontalDirection;
            iXpixels -= 0x100000;
        } while (iXpixels > 0);
        if (iXpixels <= 0) {
            iXpixels += iX_steps;
        }
    } while (++usY_Start <= usY_End);                                    // follow the line from top to bottom
#else
    iY_steps = (usY_End - usY_Start);                                    // height
    if (usX_Start <= usX_End) {
        iHorizontalDirection = 1;                                        // draw from left to right        
        if (usX_End == usX_Start) {
            iY_steps = 0x7fffffff;                                       // vertical line
        }
        else {
            if ((usX_End - usX_Start) <= iY_steps) {
                iY_steps *= 0x100000;
                iY_steps /= (usX_End - usX_Start);
            }
            else {
                if (usY_End == usY_Start) {
                    iX_steps = 0x7fffffff;                               // horizontal line
                }
                else {
                    iX_steps = (usX_End - usX_Start);                    // width
                    iX_steps *= 0x100000;
                    iX_steps /= iY_steps;
                }
                iY_steps = 0x100000;
            }
        }
    }
    else {
        iHorizontalDirection = -1;                                       // draw from right to left
        if ((usX_Start - usX_End) <= iY_steps) {
            if (usX_Start == usX_End) {                                  // vertical line
                iY_steps = 0x7fffffff;
            }
            else {
                iY_steps *= 0x100000;
                iY_steps /= (usX_Start - usX_End);
            }
            iX_steps = 0x100000;
        }
        else {
            if (iY_steps == 0) {                                         // horizontal line
                iX_steps = 0x7fffffff;
            }
            else {
                iX_steps = (usX_Start - usX_End);                        // width
                iX_steps *= 0x100000;
                iX_steps /= iY_steps;
            }
            iY_steps = 0x100000;
        }
    }
    ucPixel = (0x80 >> (usX_Start%8));                                   // starting position of the pixel to be drawn
    iYpixels = iY_steps;
    iXpixels = iX_steps;                                                 // the number of pixels to draw in the horizontal direction (left or right)
    do {
        if (usY_Start >= GLCD_Y) {
            break;
        }
        do {
            if (usX_Start >= GLCD_X) {
                break;
            }
            if (iHorizontalDirection > 0) {
                if (usX_Start > usX_End) {
                    break;
                }
            }
            else {
                if (usX_Start < usX_End) {
                    break;
                }
            }
            ucOriginalMemoryContent = ucPixelArray[usY_Start][usX_Start/8];// get the original content
            if (line->ucMode & (PAINT_INVERTED | PAINT_DARK)) {
                if (line->ucMode & PAINT_DARK) {
                    ucPixelArray[usY_Start][usX_Start/8] &= ~ucPixel;    // clear the pixel
                }
                else {
                    ucPixelArray[usY_Start][usX_Start/8] = (ucOriginalMemoryContent ^ ucPixel); // set the pixel to the inverted value of background
                }
            }
            else {                                                       // default use
                ucPixelArray[usY_Start][usX_Start/8] |= ucPixel;         // set the pixel
            }
            if (ucPixelArray[usY_Start][usX_Start/8] != ucOriginalMemoryContent) { // check whether it was indeed modified
                ucByteUpdateArray[usY_Start][usX_Start/64] |= (0x80 >> ((usX_Start%64)/8));// mark that this pixel byte needs to be updated
            }
            iYpixels -= 0x100000;
            if (iYpixels > 0) {
                break;
            }
            iYpixels += iY_steps;
            if (iHorizontalDirection > 0) {
                ucPixel >>= 1;
                if (ucPixel == 0) {
                    ucPixel = 0x80;
                }
            }
            else {
                ucPixel <<= 1;
                if (ucPixel == 0) {
                    ucPixel = 0x01;
                }
            }
            usX_Start += iHorizontalDirection;
            iXpixels -= 0x100000;
        } while (iXpixels > 0);
        if (iXpixels <= 0) {
            iXpixels += iX_steps;
        }
    } while (++usY_Start <= usY_End);                                    // follow the line from top to bottom
#endif
    if (line->ucMode & REDRAW) {
        return (fnSmartUpdate(1));                                       // redraw GLCD if commanded to do so
    }
    return STATE_LCD_READY;                                              // operation complete
}


// Paint a filled rectangle as commanded by the application
//
static int fnWriteRect(GLCD_RECT_BLINK *rect)                            // {1}
{
    int iHorizontalDirection;
    unsigned short usX_Start, usY_Start, usX_End, usY_End;
#if (!defined _GLCD_SAMSUNG && !defined ST7565S_GLCD_MODE) || defined OLED_GLCD_MODE || defined TFT_GLCD_MODE // {19}
    unsigned char ucPixelStart;
#endif
    unsigned char ucOriginalMemoryContent;
    if (rect->rect_corners.usY_start <= rect->rect_corners.usY_end) {    // always draw from top to bottom
        usY_Start = rect->rect_corners.usY_start;
        usY_End   = rect->rect_corners.usY_end;
        usX_Start = rect->rect_corners.usX_start;
        usX_End   = rect->rect_corners.usX_end;
    }
    else {
        usY_Start = rect->rect_corners.usY_end;
        usY_End   = rect->rect_corners.usY_start;
        usX_Start = rect->rect_corners.usX_end;
        usX_End   = rect->rect_corners.usX_start;
    }
#if (defined _GLCD_SAMSUNG || defined ST7565S_GLCD_MODE) && !defined OLED_GLCD_MODE && !defined TFT_GLCD_MODE && !defined NOKIA_GLCD_MODE && !defined CGLCD_GLCD_MODE && !defined KITRONIX_GLCD_MODE && !defined TFT2N0369_GLCD_MODE && !defined MB785_GLCD_MODE // {19}
    if (usX_Start <= usX_End) {
        iHorizontalDirection = 1;                                        // draw from left to right
    }
    else {
        iHorizontalDirection = -1;                                       // draw from right to left
    }

    do {
        int iNextY = 8;
        unsigned short usX = usX_Start;                                  // start of new line
        unsigned char ucThisPixel = (0xff << (usY_Start%8));
        iNextY -= (usY_Start%8);
        if ((usY_End - usY_Start + 1) < iNextY) {
            ucThisPixel &= (0xff >> (iNextY - (usY_End - usY_Start + 1)));
            iNextY = (usY_End - usY_Start + 1);
        }
        do {
            if (((usY_Start/8) >= Y_BYTES) || (usX >= X_BYTES)) {        // {12} ensure the rectangle is not too large
                break;
            }
            ucOriginalMemoryContent = ucPixelArray[usY_Start/8][usX];    // get the original content
            if (rect->ucMode & (PAINT_INVERTED | PAINT_DARK)) {
                if (rect->ucMode & PAINT_DARK) {
                    ucPixelArray[usY_Start/8][usX] &= ~ucThisPixel;      // clear the pixel(s)
                }
                else {
                    ucPixelArray[usY_Start/8][usX] = (ucOriginalMemoryContent ^ ucThisPixel); // set the pixel(s) to the inverted value of background
                }
            }
            else {                                                       // default use
                ucPixelArray[usY_Start/8][usX] |= ucThisPixel;           // set the pixel(s)
            }
            if (ucPixelArray[usY_Start/8][usX] != ucOriginalMemoryContent) { // check whether it was indeed modified
                ucByteUpdateArray[usY_Start/64][usX] |= (0x01 << (usY_Start/8));// mark that this pixel byte needs to be updated
            }
            if (usX == usX_End) {
                break;
            }
            usX += iHorizontalDirection;
        } while (1);
        usY_Start += iNextY;
    }  while (usY_Start <= usY_End);
#else                                                                    // Toshiba controller
    ucPixelStart = (0x80 >> (usX_Start%8));                              // starting position of the pixel to be drawn
    if (usX_Start <= usX_End) {
        iHorizontalDirection = 1;                                        // draw from left to right
    }
    else {
        iHorizontalDirection = -1;                                       // draw from right to left
    }

    do {
        unsigned short usX = usX_Start;                                  // start of new line
        unsigned char ucPixel = ucPixelStart;
        do {
            unsigned char ucThisPixel = ucPixel;
            int iNextX = iHorizontalDirection;
            if ((usY_Start >= Y_BYTES) || (usX/8 >= X_BYTES)) {          // {12} ensure the rectangle is not too large
                break;
            }
            ucOriginalMemoryContent = ucPixelArray[usY_Start][usX/8];    // get the original content
            if (iHorizontalDirection == 1) {
                if ((ucPixel & 0x80) && ((usX_End - usX) > 8)) {         // drawing from left to right and a complete byte can be written
                    ucThisPixel = 0xff;
                    iNextX = 8;
                }
                else {
                    ucPixel >>= 1;
                    if (ucPixel == 0) {
                        ucPixel = 0x80;
                    }
                }
            }
            else {
                if ((ucPixel & 0x01) && ((usX - usX_End) > 8)) {         // drawing from right to left and a complete byte can be written
                    ucThisPixel = 0xff;
                    iNextX = -8;
                }
                else {
                    ucPixel <<= 1;
                    if (ucPixel == 0) {
                        ucPixel = 0x01;
                    }
                }
            }
            if (rect->ucMode & (PAINT_INVERTED | PAINT_DARK)) {
                if (rect->ucMode & PAINT_DARK) {
                    ucPixelArray[usY_Start][usX/8] &= ~ucThisPixel;      // clear the pixel(s)
                }
                else {
                    ucPixelArray[usY_Start][usX/8] = (ucOriginalMemoryContent ^ ucThisPixel); // set the pixel(s) to the inverted value of background
                }
            }
            else {                                                       // default use
                ucPixelArray[usY_Start][usX/8] |= ucThisPixel;           // set the pixel(s)
            }
            if (ucPixelArray[usY_Start][usX/8] != ucOriginalMemoryContent) { // check whether it was indeed modified
                ucByteUpdateArray[usY_Start][usX/64] |= (0x80 >> ((usX%64)/8));// mark that this pixel byte needs to be updated
            }
            if (usX == usX_End) {
                break;
            }
            usX += iNextX;
        } while (1);
    }  while (usY_Start++ < usY_End);
#endif
    if (rect->ucMode & REDRAW) {
        return (fnSmartUpdate(1));                                       // redraw GLCD if commanded to do so
    }
    return STATE_LCD_READY;                                              // operation complete
}

// Scroll the display content in x and/or y direction
//
static int fnWriteScroll(GLCD_SCROLL *scroll)
{
#if (defined _GLCD_SAMSUNG || defined ST7565S_GLCD_MODE) && !defined OLED_GLCD_MODE && !defined TFT_GLCD_MODE && !defined NOKIA_GLCD_MODE && !defined CGLCD_GLCD_MODE && !defined KITRONIX_GLCD_MODE && !defined TFT2N0369_GLCD_MODE && !defined MB785_GLCD_MODE // {19}
    int iX = 0;
    int iY;
    int iByteShift = (scroll->sY_scroll/8);
    int iBitShift  = (scroll->sY_scroll%8);
    unsigned char ucOriginalMemoryContent;
    #if defined MAX_BLINKING_OBJECTS
    if (KILL_BLINK_OBJECTS & scroll->ucMode) {                           // often it is desired to stop blinking when starting scrolling
        fnConfigureBlink(BLINK_DELETE_ALL, 0);
    }
    #endif
    while (iX < X_BYTES) {
        for (iY = 0; iY < Y_BYTES; iY++) {
            ucOriginalMemoryContent = ucPixelArray[iY][iX];              // get the original content
            if ((iY + iByteShift) >= Y_BYTES) {
                if (scroll->ucMode & PAINT_DARK) {
                    ucPixelArray[iY][iX] = 0xff;
                }
                else {
                    ucPixelArray[iY][iX] = 0;
                }
            }
            else {
                ucPixelArray[iY][iX] = ucPixelArray[iY + iByteShift][iX];
            }
            if (iBitShift != 0) {
                ucPixelArray[iY][iX] >>= iBitShift;
                if ((iY + iByteShift + 1) >= Y_BYTES) {
                    if (scroll->ucMode & PAINT_DARK) {
                        ucPixelArray[iY][iX] |= (0xff << (8 - iBitShift));
                    }
                }
                else {
                    ucPixelArray[iY][iX] |= (ucPixelArray[iY + iByteShift + 1][iX] << (8 - iBitShift));
                }
            }
            if (ucOriginalMemoryContent != ucPixelArray[iY][iX]) {       // change in the byte
                ucByteUpdateArray[iY/8][iX] |= (0x01 << iY);             // mark that this pixel byte needs to be updated
            }
        }
        iX++;
    }
#else
    int iTop = 0;
    int iStart = (signed short)(scroll->sY_scroll);
    int iBottom = ((GLCD_Y/CGLCD_PIXEL_SIZE) - scroll->sY_scroll);
    int iX;
    unsigned char ucOriginalMemoryContent;
    #if defined MAX_BLINKING_OBJECTS
    if (KILL_BLINK_OBJECTS & scroll->ucMode) {                           // often it is desired to stop blinking when starting scrolling
        fnConfigureBlink(BLINK_DELETE_ALL, 0);
    }
    #endif
    while (iTop < iBottom) {
        for (iX = 0; iX < X_BYTES; iX++) {
            ucOriginalMemoryContent = ucPixelArray[iTop][iX];            // get the original content
            ucPixelArray[iTop][iX] = ucPixelArray[iStart][iX];
            if (ucOriginalMemoryContent != ucPixelArray[iTop][iX]) {     // change in the byte
                ucByteUpdateArray[iTop][iX/8] |= (0x80 >> (iX%8));       // mark that this pixel byte needs to be updated
            }
        }
        iStart++;
        iTop++;
    }
    while (iTop < (GLCD_Y/CGLCD_PIXEL_SIZE)) {
        for (iX = 0; iX < X_BYTES; iX++) {
            ucOriginalMemoryContent = ucPixelArray[iTop][iX];            // get the original content
            if (scroll->ucMode & PAINT_DARK) {
                ucPixelArray[iTop][iX] = 0xff;
            }
            else {
                ucPixelArray[iTop][iX] = 0;
            }
            if (ucOriginalMemoryContent != ucPixelArray[iTop][iX]) {     // change in the byte
                ucByteUpdateArray[iTop][iX/8] |= (0x80 >> (iX%8));       // mark that this pixel byte needs to be updated
            }
        }
        iTop++;
    }
#endif
    if (scroll->ucMode & REDRAW) {
        return (fnSmartUpdate(1));                                       // redraw GLCD if commanded to do so
    }
    return STATE_LCD_READY;
}

#if defined MAX_BLINKING_OBJECTS
static int fnFutherBlinkObjects(LCD_OBJECTS blinking_objects[MAX_BLINKING_OBJECTS])
{
    int i = 0;
    while (i < MAX_BLINKING_OBJECTS) {
        if ((blinking_objects[i].ucObjectType != BLINK_NOTHING) && (blinking_objects[i].blink_count_down == 0)) { // this object's blink timer has also fired
            blinking_objects[i].blink_count_down = blinking_objects[i].blink_interval; // set next blink time
            return i;
        }
        i++;
    }
    return -1;                                                           // no further timed out object forund
}

static int fnStartBlinkTimer(LCD_OBJECTS blinking_objects[MAX_BLINKING_OBJECTS], int iNewObject)
{
    UTASK_TICK remaining_tick;
    int i = 0;
    int iLowest = -1;
    int iActiveTimer = 0;
    DELAY_LIMIT next_timer = (DELAY_LIMIT)-1;
    while (i < MAX_BLINKING_OBJECTS) {
        if (blinking_objects[i].ucObjectType != BLINK_NOTHING) {         // active timer
            if (blinking_objects[i].blink_count_down < next_timer) {
                next_timer = blinking_objects[i].blink_count_down;       // lowest value found
                if (iNewObject != i) {
                    iActiveTimer = 1;                                    // timers(s) already active
                }
                iLowest = i;
            }
        }
        i++;
    }
    if (iNewObject < 0) {                                                // a timeout has just occurred and so the lowest next delay must always be set
        i = 0;
        while (i < MAX_BLINKING_OBJECTS) {                               // compensate all others
            if (i != iLowest) {
                blinking_objects[i].blink_count_down -= next_timer;
            }
            i++;
        }
    }
    else {
        if (iLowest == iNewObject) {                                     // the new entry has the shortest next delay value
            if (iActiveTimer != 0) {                                     // a timer is presently active
                remaining_tick = uTaskerRemainingTime(OWN_TASK);         // see how much of the present timer is remaining
                i = 0;
                if (remaining_tick <= next_timer) {
                    while (i < MAX_BLINKING_OBJECTS) {
                        if (i != iNewObject) {
                            blinking_objects[i].blink_count_down -= (DELAY_LIMIT)remaining_tick;
                        }
                        i++;
                    }
                }
                else {                                                   // the new delay is shorter than the existing one's remaining time
                    while (i < MAX_BLINKING_OBJECTS) {
                        if (i != iNewObject) {
                            blinking_objects[i].blink_count_down -= next_timer;
                        }
                        i++;
                    }
                }
            }
        }
        else if (iActiveTimer != 0) {
            blinking_objects[iNewObject].blink_count_down -= next_timer; // adjust first delay of new entry
            return iLowest;                                              // don't disturb present timer
        }
    }
    uTaskerMonoTimer(OWN_TASK, next_timer, T_BLINK);                     // start delay to next blink interval
    return iLowest;
}

static int fnConfigureBlink(unsigned char ucObjectType, void *object)
{
    static LCD_OBJECTS blinking_objects[MAX_BLINKING_OBJECTS] = {{0}};
    static int iLastObject = 0;
    RECT_COORDINATES *object_coordinates = 0;
    DELAY_LIMIT blink_half_period;
    int i = 0;
    switch (ucObjectType) {
    case BLINK_RECT:
        {
            GLCD_RECT_BLINK *rect = (GLCD_RECT_BLINK *)object;
            object_coordinates = &(rect->rect_corners);
            blink_half_period = rect->blink_half_period;
        }
        break;
    case BLINK_LINE:
        {
            GLCD_LINE_BLINK *line = (GLCD_LINE_BLINK *)object;
            object_coordinates = &(line->line_start_end);
            blink_half_period = line->blink_half_period;
        }
        break;
    case BLINK_NEXT:                                                     // blink timer has fired
        blinking_objects[iLastObject].blink_count_down = blinking_objects[iLastObject].blink_interval;
        do {
            switch (blinking_objects[iLastObject].ucObjectType) {
            case BLINK_RECT:
                {
                    GLCD_RECT rect;
                    rect.ucMode = (PAINT_INVERTED | REDRAW);
                    uMemcpy(&rect.rect_corners, &blinking_objects[iLastObject].blink_coordinates, sizeof(RECT_COORDINATES));
                    fnDoLCD_rect((GLCD_RECT_BLINK *)&rect);
                }
                break;
            case BLINK_LINE:
                {
                    GLCD_LINE line;
                    line.ucMode = (PAINT_INVERTED | REDRAW);
                    uMemcpy(&line.line_start_end, &blinking_objects[iLastObject].blink_coordinates, sizeof(RECT_COORDINATES));
                    fnDoLCD_line((GLCD_LINE *)&line);
                }
                break;
            }
        } while ((iLastObject = fnFutherBlinkObjects(blinking_objects)) >= 0);
        iLastObject = fnStartBlinkTimer(blinking_objects, iLastObject);  // start next delay
        return 0;
    case BLINK_DELETE_ALL:
        uMemset(&blinking_objects, 0, sizeof(blinking_objects));
        uTaskerStopTimer(OWN_TASK);                                      // no more blink timers needed
        return 0;
    default:
        return 1;
    }
    while (i < MAX_BLINKING_OBJECTS) {
        if ((blink_half_period != 0) && (blinking_objects[i].ucObjectType == BLINK_NOTHING)) { // empty entry found
            blinking_objects[i].ucObjectType = ucObjectType;
            blinking_objects[i].blink_interval = blinking_objects[i].blink_count_down = blink_half_period; // blink frequency
            uMemcpy(&blinking_objects[i].blink_coordinates, object_coordinates, sizeof(RECT_COORDINATES));
            iLastObject = fnStartBlinkTimer(blinking_objects, i);
            return 0;
        }
        else if ((blinking_objects[i].ucObjectType == ucObjectType) && (!uMemcmp(&blinking_objects[i].blink_coordinates, object_coordinates, sizeof(RECT_COORDINATES)))) {
            if (blink_half_period != 0) {                                // change blink rate on next timeout
                blinking_objects[i].blink_interval = blink_half_period;
                return 0;
            }
            blinking_objects[i].ucObjectType = BLINK_NOTHING;            // delete entry
            if (iLastObject == i) {                                      // timer presently being managed by this object
                i = 0;
                while (i < MAX_BLINKING_OBJECTS) {                       // check whether it can be killed
                    if (blinking_objects[i].ucObjectType != BLINK_NOTHING) { // adapted timer required
                        UTASK_TICK elapsed_tick = (blinking_objects[iLastObject].blink_count_down - uTaskerRemainingTime(OWN_TASK));
                        while (i < MAX_BLINKING_OBJECTS) {
                            blinking_objects[i].blink_count_down += (DELAY_LIMIT)elapsed_tick;
                            i++;
                        }
                        iLastObject = fnStartBlinkTimer(blinking_objects, -1);
                        return 0;
                    }
                    i++;
                }
                uTaskerStopTimer(OWN_TASK);                              // no more blink timers needed
            }
            return 0;
        }
        i++;
    }
    return 1;
}
#endif

#if defined _GLCD_SAMSUNG && !defined OLED_GLCD_MODE && !defined TFT_GLCD_MODE && !defined NOKIA_GLCD_MODE && !defined CGLCD_GLCD_MODE && !defined KITRONIX_GLCD_MODE && !defined TFT2N0369_GLCD_MODE && !defined MB785_GLCD_MODE
// Position XY Address Pointer within the (64 x X) x 64 Display Area
//
static void fnGLCDGotoXY(unsigned char ucXPos, unsigned char ucYPos) 
{
    unsigned char ucChip = GLCD_CHIP0;                                   // 'left' Chip  

    #if SAMSUNG_CHIPS > 1
    if (ucXPos >= SINGLE_CHIP_WIDTH) {                                   // select GLCD_CHIP1, the 'right' Chip
        ucXPos -= SINGLE_CHIP_WIDTH;
        ucChip = GLCD_CHIP1;
    }
    #endif

    while (GLCD_BUSY()) {}                                               // wait until ready
    fnWriteGLCD_cmd((unsigned char)(GLCD_SET_ADD | ucXPos), ucChip);     // set X Coordinate
    while (GLCD_BUSY()) {}                                               // wait until ready
    fnWriteGLCD_cmd((unsigned char)(GLCD_SET_PAGE | ucYPos/8), GLCD_CHIP0); // set Y Coordinate
    #if SAMSUNG_CHIPS > 1
    while (GLCD_BUSY()) {}                                               // wait until ready
    fnWriteGLCD_cmd((unsigned char)(GLCD_SET_PAGE | ucYPos/8), GLCD_CHIP1);
    #endif
}
#elif defined ST7565S_GLCD_MODE                                          // {19}
static void fnGLCDGotoXY(unsigned char ucXPos, unsigned char ucYPos)
{
    while (GLCD_BUSY()) {}                                               // wait until ready
    fnWriteGLCD_cmd((unsigned char)(GLCD_SET_ADDH | ucXPos/16));  		 // set X Coordinate - high nibble
    while (GLCD_BUSY()) {}                                               // wait until ready
    fnWriteGLCD_cmd((unsigned char)(GLCD_SET_ADDL | ucXPos%16));   		 // set X Coordinate - low nibble
    while (GLCD_BUSY()) {}                                               // wait until ready
    fnWriteGLCD_cmd((unsigned char)(GLCD_SET_PAGE | ucYPos/8));			 // set Y Coordinate
}

#endif

// This routine is called when the content of the backup buffer is to be written to the GLCD.
// It only updates changed bytes and only sets the address pointer when the auto-inc value doesn't automatically match
//
static int fnSmartUpdate(int iStart)
{
#if defined OLED_GLCD_MODE || defined NOKIA_GLCD_MODE
    #if defined NOKIA_GLCD_MODE
    #define WINDOW_WIDTH DISPLAY_RIGHT_PIXEL
    static unsigned char pucWindowX[] = {CASET, 0, DISPLAY_RIGHT_PIXEL};
    static unsigned char pucWindowY[] = {PASET, 0, DISPLAY_BOTTOM_PIXEL};
    #else
    #define WINDOW_WIDTH ((GLCD_X - 2)/2)
    static unsigned char pucWindowX[] = {0x15, 0, WINDOW_WIDTH};
    static unsigned char pucWindowY[] = {0x75, 0, (GLCD_Y - 1)};
    #endif
    static unsigned char ucLastX_WindowStart = 0;
    static unsigned char x_stall = 0;
    static unsigned char y_stall = 0;
    static unsigned char ucLastX = 0xff;
    static unsigned char ucLastY = 0xff;
    int iMaxText = MAX_GLCD_WRITE_BURST;                                 // allow this many writes before pausing
    unsigned char x;
    unsigned char y;
    unsigned char g_pucBuffer[12];
    if (iStart != 0) {
        x_stall = DISPLAY_LEFT_PIXEL;
        y_stall = DISPLAY_TOP_PIXEL;
    }
    for (y = y_stall; y <= DISPLAY_BOTTOM_PIXEL; y++) {                  // for each row
        for (x = x_stall; x < (UPDATE_WIDTH); x++) {                     // for each column
            if (ucByteUpdateArray[y][x] != 0) {                          // check if a pixel has changed in this byte
                int iAdd = 0;
                unsigned char ucBit = 0x80;
    #if defined NOKIA_GLCD_MODE                                          // display pixel width is not divisable by 8 so handle last byte differently
                if (x == (UPDATE_WIDTH - 1)) {
                    ucByteUpdateArray[y][x] &= (0xff << (8 - (X_BYTES%8)));
                }
    #endif
                while (ucByteUpdateArray[y][x] != 0) {                   // for each changed pixel in the changed byte
                    if (ucByteUpdateArray[y][x] & ucBit) {               // this byte pixel has changed
                        unsigned char ucNewPixelByte = ucPixelArray[y][(x * 8 + iAdd)];
    #if defined NOKIA_GLCD_MODE
                        pucWindowY[1] = (y + 2);                         // avoid invisible lines
                        pucWindowX[1] = ((x * 64) + iAdd*8);
    #else                                                                // OLED
                        pucWindowY[1] = y;
                        pucWindowX[1] = ((x * 32) + (iAdd * 4));
    #endif
                        if ((ucLastX != pucWindowX[1]) || (ucLastY != pucWindowY[1])) { // if internal address pointer needs to be changed
                            ucLastX_WindowStart = pucWindowX[1];
                            fnSendSPI_Command(pucWindowX, sizeof(pucWindowX));
                            fnSendSPI_Command(pucWindowY, sizeof(pucWindowY));
    #if defined NOKIA_GLCD_MODE
                            WriteSpiCommand(RAMWR);                      // data follows
    #endif
                        }
    #if defined NOKIA_GLCD_MODE
                        if (ucNewPixelByte & 0x80) {                     // set pixel colour for each pixel in the on state
                            g_pucBuffer[0] = (unsigned char)(_LCD_PIXEL_COLOUR >> 4);
                            g_pucBuffer[1] = (unsigned char)(_LCD_PIXEL_COLOUR << 4);
                        }
                        else {
                            g_pucBuffer[0] = (unsigned char)(_LCD_ON_COLOUR >> 4);
                            g_pucBuffer[1] = (unsigned char)(_LCD_ON_COLOUR << 4);
                        }
                        if (ucNewPixelByte & 0x40) {
                            g_pucBuffer[1] |= (unsigned char)(_LCD_PIXEL_COLOUR >> 8);
                            g_pucBuffer[2] = (unsigned char)(_LCD_PIXEL_COLOUR);
                        }
                        else {
                            g_pucBuffer[1] |= (unsigned char)(_LCD_ON_COLOUR >> 8);
                            g_pucBuffer[2] = (unsigned char)(_LCD_ON_COLOUR);
                        }
                        if (pucWindowX[1] == 0x80) {
                            fnSend_SPI_data(g_pucBuffer, 3);             // write 2 pixels to display (involving 3 bytes to be sent)
                            pucWindowX[1] = (WINDOW_WIDTH - 8);
                        }
                        else {
                            if (ucNewPixelByte & 0x20) {
                                g_pucBuffer[3] = (unsigned char)(_LCD_PIXEL_COLOUR >> 4);
                                g_pucBuffer[4] = (unsigned char)(_LCD_PIXEL_COLOUR << 4);
                            }
                            else {
                                g_pucBuffer[3] = (unsigned char)(_LCD_ON_COLOUR >> 4);
                                g_pucBuffer[4] = (unsigned char)(_LCD_ON_COLOUR << 4);
                            }
                            if (ucNewPixelByte & 0x10) {
                                g_pucBuffer[4] |= (unsigned char)(_LCD_PIXEL_COLOUR >> 8);
                                g_pucBuffer[5] = (unsigned char)(_LCD_PIXEL_COLOUR);
                            }
                            else {
                                g_pucBuffer[4] |= (unsigned char)(_LCD_ON_COLOUR >> 8);
                                g_pucBuffer[5] = (unsigned char)(_LCD_ON_COLOUR);
                            }
                            if (ucNewPixelByte & 0x08) {
                                g_pucBuffer[6] = (unsigned char)(_LCD_PIXEL_COLOUR >> 4);
                                g_pucBuffer[7] = (unsigned char)(_LCD_PIXEL_COLOUR << 4);
                            }
                            else {
                                g_pucBuffer[6] = (unsigned char)(_LCD_ON_COLOUR >> 4);
                                g_pucBuffer[7] = (unsigned char)(_LCD_ON_COLOUR << 4);
                            }
                            if (ucNewPixelByte & 0x04) {
                                g_pucBuffer[7] |= (unsigned char)(_LCD_PIXEL_COLOUR >> 8);
                                g_pucBuffer[8] = (unsigned char)(_LCD_PIXEL_COLOUR);
                            }
                            else {
                                g_pucBuffer[7] |= (unsigned char)(_LCD_ON_COLOUR >> 8);
                                g_pucBuffer[8] = (unsigned char)(_LCD_ON_COLOUR);
                            }
                            if (ucNewPixelByte & 0x02) {
                                g_pucBuffer[9] = (unsigned char)(_LCD_PIXEL_COLOUR >> 4);
                                g_pucBuffer[10] = (unsigned char)(_LCD_PIXEL_COLOUR << 4);
                            }
                            else {
                                g_pucBuffer[9] = (unsigned char)(_LCD_ON_COLOUR >> 4);
                                g_pucBuffer[10] = (unsigned char)(_LCD_ON_COLOUR << 4);
                            }
                            if (ucNewPixelByte & 0x01) {
                                g_pucBuffer[10] |= (unsigned char)(_LCD_PIXEL_COLOUR >> 8);
                                g_pucBuffer[11] = (unsigned char)(_LCD_PIXEL_COLOUR);
                            }
                            else {
                                g_pucBuffer[10] |= (unsigned char)(_LCD_ON_COLOUR >> 8);
                                g_pucBuffer[11] = (unsigned char)(_LCD_ON_COLOUR);
                            }
                            fnSend_SPI_data(g_pucBuffer, 12);            // write 8 pixels to display (involving 12 bytes to be sent)
                        }
    #else
                        uMemset(g_pucBuffer, 0, sizeof(g_pucBuffer));    // clear buffer to backgound colour
                        if (ucNewPixelByte & 0x80) {
                            g_pucBuffer[0] = 0xf0;
                        }
                        if (ucNewPixelByte & 0x40) {
                            g_pucBuffer[0] |= 0x0f;
                        }
                        if (ucNewPixelByte & 0x20) {
                            g_pucBuffer[1] = 0xf0;
                        }
                        if (ucNewPixelByte & 0x10) {
                            g_pucBuffer[1] |= 0x0f;
                        }
                        if (ucNewPixelByte & 0x08) {
                            g_pucBuffer[2] = 0xf0;
                        }
                        if (ucNewPixelByte & 0x04) {
                            g_pucBuffer[2] |= 0x0f;
                        }
                        if (ucNewPixelByte & 0x02) {
                            g_pucBuffer[3] = 0xf0;
                        }
                        if (ucNewPixelByte & 0x01) {
                            g_pucBuffer[3] |= 0x0f;
                        }
                        fnSend_SPI_data(g_pucBuffer, 4);                 // write 8 pixels to display (involving 4 bytes to be sent)
        #endif
                        ucLastY = pucWindowY[1];
        #if defined NOKIA_GLCD_MODE
                        ucLastX = (pucWindowX[1] + 8);
        #else
                        ucLastX = (pucWindowX[1] + 4);
        #endif
                        if (ucLastX >= WINDOW_WIDTH) {
                            ucLastX = ucLastX_WindowStart;
                            ucLastY++;
                        }
                        ucByteUpdateArray[y][x] &= ~ucBit;
                        iMaxText--;
                    }
                    iAdd++;
                    ucBit >>= 1;
                    if (ucBit == 0) {
                        ucBit = 0x80;
                    }
                }
                if (iMaxText <= 0) {                                     // maximum bust write has been performed
                    x_stall = (x + 1);                                   // save the update context
                    y_stall = y;
                    uTaskerStateChange(OWN_TASK, UTASKER_GO);            // switch to polling mode of operation
                    return STATE_LCD_WRITING;                            // busy state
                }
            }
        }
        x_stall = 0;
    }
#else
#if defined CGLCD_GLCD_MODE || defined KITRONIX_GLCD_MODE || defined MB785_GLCD_MODE || defined TFT2N0369_GLCD_MODE
    static unsigned short usLastX_pixel = 0xffff;                        // force the initial window to be set
    static unsigned short usLastY_pixel = 0xffff;
#endif
    static unsigned char x_stall = 0;
    static unsigned char y_stall = 0;
#if defined _GLCD_SAMSUNG && !defined OLED_GLCD_MODE && !defined TFT_GLCD_MODE && !defined NOKIA_GLCD_MODE && !defined CGLCD_GLCD_MODE && !defined KITRONIX_GLCD_MODE && !defined MB785_GLCD_MODE && !defined TFT2N0369_GLCD_MODE
    static unsigned short usNextAddress[SAMSUNG_CHIPS] = {0};
#elif defined ST7565S_GLCD_MODE                                          // {19}
    static unsigned short usNextAddress = 0;
#elif defined TFT_GLCD_MODE
    unsigned long *ptDst;
#elif !defined CGLCD_GLCD_MODE && !defined KITRONIX_GLCD_MODE && !defined TFT2N0369_GLCD_MODE && !defined MB785_GLCD_MODE
    static unsigned short usLastAddress = 0xffff;
#endif
    int iMaxText = MAX_GLCD_WRITE_BURST;                                 // allow this many writes before yielding
    unsigned char x;
    unsigned char y;
    if (iStart != 0) {
        x_stall = DISPLAY_LEFT_PIXEL;
        y_stall = DISPLAY_TOP_PIXEL;
    }
#if defined _GLCD_SAMSUNG && !defined OLED_GLCD_MODE && !defined TFT_GLCD_MODE && !defined NOKIA_GLCD_MODE && !defined CGLCD_GLCD_MODE && !defined KITRONIX_GLCD_MODE && !defined TFT2N0369_GLCD_MODE && !defined MB785_GLCD_MODE
    for (y = y_stall; y < GLCD_Y;) {                                     // for each row
        unsigned char ucPixelByteBit = (0x01 << (y/8));
        while (ucPixelByteBit != 0) {
            for (x = x_stall; x < X_BYTES; x++) {                        // for each column
                if (ucByteUpdateArray[y/64][x] & ucPixelByteBit) {       // check if a pixel has changed in this byte
                    ucByteUpdateArray[y/64][x] &= ~ucPixelByteBit;
                    if (x < SINGLE_CHIP_WIDTH) {                         // chip 0
                        if (usNextAddress[0] != ((y * 8) + x)) {
                            fnGLCDGotoXY(x, y);                          // ensure coordinate is correct
                        }
                        while (GLCD_BUSY()) {}                    
                        fnWriteGLCD_data(ucPixelArray[y/8][x], GLCD_CHIP0);
                        usNextAddress[0] = ((y * 8) + x + 1);
                        if ((usNextAddress[0] % SINGLE_CHIP_WIDTH) == 0) {
                            usNextAddress[0] -= SINGLE_CHIP_WIDTH;
                        }
                    }
                    else {                                               // chip 1
                        if (usNextAddress[1] != ((y * 8) + x)) {
                            fnGLCDGotoXY(x, y);                          // ensure coordinate is correct
                        }
                        while (GLCD_BUSY()) {}                    
                        fnWriteGLCD_data(ucPixelArray[y/8][x], GLCD_CHIP1);
                        usNextAddress[1] = ((y * 8) + x + 1);
                        if ((usNextAddress[1] % SINGLE_CHIP_WIDTH) == 0) {
                            usNextAddress[1] -= SINGLE_CHIP_WIDTH;
                        }
                    }
                    if (--iMaxText <= 0) {                               // maximum bust write has been performed
                        y_stall = y;
                        x_stall = (x + 1);                               // save the update context
                        if (x_stall >= X_BYTES) {
                            x_stall = 0;
                        }
                        uTaskerStateChange(OWN_TASK, UTASKER_GO);        // switch to polling mode of operation
                        return STATE_LCD_WRITING;                        // busy state
                    }
                }
            }
            y += 8;;
            ucPixelByteBit <<= 1;
            x_stall = 0;
        }
    }
#elif defined ST7565S_GLCD_MODE											 // {19}	
    for (y = y_stall; y < GLCD_Y;) {                                     // for each row
        unsigned char ucPixelByteBit = (0x01 << (y/8));
        while (ucPixelByteBit != 0) {
            for (x = x_stall; x < X_BYTES; x++) {                        // for each column
                if (ucByteUpdateArray[y/64][x] & ucPixelByteBit) {       // check if a pixel has changed in this byte
                    ucByteUpdateArray[y/64][x] &= ~ucPixelByteBit;
                    if (usNextAddress != ((y * 16) + x)) {
                        fnGLCDGotoXY(x, y);                              // ensure coordinate is correct
                    }
                    while (GLCD_BUSY()) {}                    
                    fnWriteGLCD_data(ucPixelArray[y/8][x]);
                    usNextAddress = ((y * 16) + x + 1);
                    if ((usNextAddress % 128) == 0) {
                        usNextAddress -= 128;
                    }
                    if (--iMaxText  <= 0) {                              // maximum bust write has been performed
                        y_stall = y;
                        x_stall = (x + 1);                               // save the update context
                        if (x_stall >= X_BYTES) {
                            x_stall = 0;
                        }
                        uTaskerStateChange(OWN_TASK, UTASKER_GO);        // switch to polling mode of operation
                        return STATE_LCD_WRITING;                        // busy state
                    }
                }
            }
            y += 8;
            ucPixelByteBit <<= 1;
            x_stall = 0;
        }
    }
#else
    for (y = y_stall; y <= (DISPLAY_BOTTOM_PIXEL/CGLCD_PIXEL_SIZE); y++) { // for each row
        for (x = x_stall; x < (UPDATE_WIDTH); x++) {                     // for each column
            if (ucByteUpdateArray[y][x] != 0) {                          // check if a pixel has changed in this byte
    #if !defined TFT_GLCD_MODE && !defined CGLCD_GLCD_MODE && !defined KITRONIX_GLCD_MODE && !defined TFT2N0369_GLCD_MODE && !defined MB785_GLCD_MODE
                int iOffset = ((y * X_BYTES) + (x * 8)); 
    #endif
    #if defined CGLCD_GLCD_MODE || defined KITRONIX_GLCD_MODE || defined MB785_GLCD_MODE || defined TFT2N0369_GLCD_MODE
                int iRepeatY = 0;
    #endif
                int iAdd = 0;
                unsigned char ucBit = 0x80;
                while (ucByteUpdateArray[y][x] != 0) {                   // for each changed bit in the changed byte
                    if ((ucByteUpdateArray[y][x] & ucBit) != 0) {        // this pixel byte has changed
    #if defined TFT_GLCD_MODE
                        unsigned char ucTFTBit = 0x80;
                        ptDst = (unsigned long *)fnGetSDRAM((unsigned char *)SDRAM_ADDR);
                        ptDst += (((GLCD_X * CGLCD_PIXEL_SIZE) * y) + ((((x * 8) + iAdd) * 8) * CGLCD_PIXEL_SIZE)); // location in LCD display memory
                        while (ucTFTBit != 0) {                          // update 8 pixels
                            if (ucPixelArray[y][(x * 8 + iAdd)] & ucTFTBit) { // on color to be set
        #if CGLCD_PIXEL_SIZE > 1
                                *ptDst = _LCD_PIXEL_COLOUR;
                                *(ptDst + GLCD_X) = _LCD_PIXEL_COLOUR;
            #if CGLCD_PIXEL_SIZE > 2
                                *(ptDst + (2 * GLCD_X)) = _LCD_PIXEL_COLOUR;
            #endif
            #if CGLCD_PIXEL_SIZE > 3
                                *(ptDst + (3 * GLCD_X)) = _LCD_PIXEL_COLOUR;
            #endif
                                ptDst++;
                                *ptDst = _LCD_PIXEL_COLOUR;
                                *(ptDst + GLCD_X) = _LCD_PIXEL_COLOUR;
            #if CGLCD_PIXEL_SIZE > 2
                                *(ptDst + (2 * GLCD_X)) = _LCD_PIXEL_COLOUR;
            #endif
            #if CGLCD_PIXEL_SIZE > 3
                                *(ptDst + (3 * GLCD_X)) = _LCD_PIXEL_COLOUR;
            #endif
                                ptDst++;
            #if CGLCD_PIXEL_SIZE > 2
                                *ptDst = _LCD_PIXEL_COLOUR;
                                *(ptDst + GLCD_X) = _LCD_PIXEL_COLOUR;
                                *(ptDst + (2 * GLCD_X)) = _LCD_PIXEL_COLOUR;
                #if CGLCD_PIXEL_SIZE > 3
                                *(ptDst + (3 * GLCD_X)) = _LCD_PIXEL_COLOUR;
                #endif
                                ptDst++;
            #endif
            #if CGLCD_PIXEL_SIZE > 3
                                *ptDst = _LCD_PIXEL_COLOUR;
                                *(ptDst + GLCD_X) = _LCD_PIXEL_COLOUR;
                                *(ptDst + (2 * GLCD_X)) = _LCD_PIXEL_COLOUR;
                                *(ptDst + (3 * GLCD_X)) = _LCD_PIXEL_COLOUR;
                                ptDst++;
            #endif
        #else
                                *ptDst++ = _LCD_PIXEL_COLOUR;
        #endif
                            }
                            else {                                       // off color to be set (background when backlight is on)
        #if CGLCD_PIXEL_SIZE > 1
                                *ptDst = _LCD_ON_COLOUR;
                                *(ptDst + GLCD_X) = _LCD_ON_COLOUR;
            #if CGLCD_PIXEL_SIZE > 2
                                *(ptDst + (2 * GLCD_X)) = _LCD_ON_COLOUR;
            #endif
            #if CGLCD_PIXEL_SIZE > 3
                                *(ptDst + (3 * GLCD_X)) = _LCD_ON_COLOUR;
            #endif
                                ptDst++;
                                *ptDst = _LCD_ON_COLOUR;
                                *(ptDst + GLCD_X) = _LCD_ON_COLOUR;
            #if CGLCD_PIXEL_SIZE > 2
                                *(ptDst + (2 * GLCD_X)) = _LCD_ON_COLOUR;
            #endif
            #if CGLCD_PIXEL_SIZE > 3
                                *(ptDst + (3 * GLCD_X)) = _LCD_ON_COLOUR;
            #endif
                                ptDst++;
            #if CGLCD_PIXEL_SIZE > 2
                                *ptDst = _LCD_ON_COLOUR;
                                *(ptDst + GLCD_X) = _LCD_ON_COLOUR;
                                *(ptDst + (2 * GLCD_X)) = _LCD_ON_COLOUR;
                #if CGLCD_PIXEL_SIZE > 3
                                *(ptDst + (3 * GLCD_X)) = _LCD_ON_COLOUR;
                #endif
                                ptDst++;
            #endif
            #if CGLCD_PIXEL_SIZE > 3
                                *ptDst = _LCD_ON_COLOUR;
                                *(ptDst + GLCD_X) = _LCD_ON_COLOUR;
                                *(ptDst + (2 * GLCD_X)) = _LCD_ON_COLOUR;
                                *(ptDst + (3 * GLCD_X)) = _LCD_ON_COLOUR;
                                ptDst++;
            #endif
        #else
                                *ptDst++ = _LCD_ON_COLOUR;
        #endif
                            }
                            ucTFTBit >>= 1;
                        }
                        iMaxText--;
                        ucByteUpdateArray[y][x] &= ~ucBit;               // reset since the changed content has been updated
    #elif defined CGLCD_GLCD_MODE || defined KITRONIX_GLCD_MODE || defined MB785_GLCD_MODE || defined TFT2N0369_GLCD_MODE
                        static unsigned short usLastX_window = 0;
                        static unsigned short usLastY_window = 0;
                        unsigned short usNextPixel = (unsigned short)((((x * 8) + iAdd) * 8) * CGLCD_PIXEL_SIZE);
                        unsigned char ucCGLCDBit = 0x80;
                        if (usNextPixel >= GLCD_X) {                     // {11} if there are no more valid screen pixels stop now
                            break;
                        }

                        if ((usNextPixel != usLastX_pixel) || (((y * CGLCD_PIXEL_SIZE) + iRepeatY) != usLastY_pixel)) { // only change the draw window when necessary
                            usLastX_pixel = usLastX_window = usNextPixel;
                            usLastY_pixel = usLastY_window = ((y * CGLCD_PIXEL_SIZE) + iRepeatY);
                            fnSetWindow(usLastX_window, usLastY_window, DISPLAY_RIGHT_PIXEL, DISPLAY_BOTTOM_PIXEL);
        #if defined AVR32_EVK1105
                            SELECT_REG(HIMAX_SRAMWRITE);
        #elif defined EK_LM3S3748
                            fnWriteGLCD_cmd(ST7637_MEMORY_WRITE);
        #endif
                        }
                        while (ucCGLCDBit != 0) {                        // for each of the 8 bits in the pixel byte
                            if ((ucCGLCDBit & ucPixelArray[y][((x * 8) + iAdd)]) != 0) { // pixel is on
        #if defined AVR32_EVK1105
                                WRITE_DATA(_LCD_PIXEL_COLOUR);
            #if CGLCD_PIXEL_SIZE > 1
                                WRITE_DATA(_LCD_PIXEL_COLOUR);
            #endif
        #elif defined EK_LM3S3748 || defined KITRONIX_GLCD_MODE || defined MB785_GLCD_MODE || defined TFT2N0369_GLCD_MODE
                                fnWriteGLCD_data_pair(_LCD_PIXEL_COLOUR);
            #if CGLCD_PIXEL_SIZE > 1
                                fnWriteGLCD_data_pair(_LCD_PIXEL_COLOUR);
            #endif
        #endif
                            }
                            else {                                       // pixel off
        #if defined AVR32_EVK1105
                                WRITE_DATA(_LCD_ON_COLOUR);
            #if CGLCD_PIXEL_SIZE > 1
                                WRITE_DATA(_LCD_ON_COLOUR);
            #endif
        #elif defined EK_LM3S3748 || defined KITRONIX_GLCD_MODE || defined MB785_GLCD_MODE || defined TFT2N0369_GLCD_MODE
                                fnWriteGLCD_data_pair(_LCD_ON_COLOUR);   // write the background color
            #if CGLCD_PIXEL_SIZE > 1
                                fnWriteGLCD_data_pair(_LCD_ON_COLOUR);   // repeat for next physical pixel
            #endif
        #endif
                            }
                            ucCGLCDBit >>= 1;
                        }
                        usLastX_pixel += (8 * CGLCD_PIXEL_SIZE);
                        if (usLastX_pixel > DISPLAY_RIGHT_PIXEL) {
        #if defined MB785_GLCD_MODE
                            usLastX_pixel = 0;
        #else
                            usLastX_pixel = usLastX_window;
        #endif
                            usLastY_pixel++;
                            if (usLastY_pixel > DISPLAY_BOTTOM_PIXEL) {
        #if defined MB785_GLCD_MODE
                                usLastY_pixel = 0;
        #else
                                usLastY_pixel = usLastY_window;
        #endif
                            }
                        }                        
                        iMaxText--;
        #if CGLCD_PIXEL_SIZE > 1
                        if (++iRepeatY >= CGLCD_PIXEL_SIZE) {
                            ucByteUpdateArray[y][x] &= ~ucBit;           // reset since the changed content has been updated
                            iRepeatY = 0;
                        }
                        else {
                            ucCGLCDBit = 0x80;                           // repeat the line byte
                            continue;                                    // repeat for second line of larger pixel
                        }
        #else
                        ucByteUpdateArray[y][x] &= ~ucBit;               // reset since the changed content has been updated
        #endif
    #else
                        if ((iOffset + iAdd) != usLastAddress) {         // if internal address pointer needs to be changed
                            fnCommandGlcd_2(SET_ADDRESS_POINTER, (unsigned short)(iOffset + iAdd));
                        }
                        fnCommandGlcd_1(WRITE_DATA_INC, ucPixelArray[y][(x * 8 + iAdd)]); // write the GLCD display value
                        usLastAddress = ((iOffset + iAdd) + 1);
                        iMaxText--;
                        ucByteUpdateArray[y][x] &= ~ucBit;               // reset since the changed content has been updated
    #endif
                    }
                    iAdd++;
                    ucBit >>= 1;
                    if (ucBit == 0) {
                        ucBit = 0x80;
                    }
                }
                if (iMaxText <= 0) {                                     // maximum bust write has been performed
                    x_stall = (x + 1);                                   // save the update context
                    y_stall = y;
    #if defined MB785_GLCD_MODE && defined SHARE_SPI
                    usLastX_pixel = usLastY_pixel = 0xffff;              // ensure next GRAM write starts with a new cursor coordinate being set and the next GRAM write being initiated correctly
                    fnConcludeGRAMwrite();                               // close present GRAM write phase to ensure final pixel is displayed
    #endif
                    uTaskerStateChange(OWN_TASK, UTASKER_GO);            // switch to polling mode of operation
                    return STATE_LCD_WRITING;                            // busy state
                }
            }
        }
        x_stall = 0;
    }
#endif
#endif
#if defined TFT_GLCD_MODE
    #if defined _WINDOWS
    CollectCommand(0, (unsigned long)fnGetSDRAM((unsigned char *)SDRAM_ADDR));
    #endif
#else
    #if defined MB785_GLCD_MODE
    usLastX_pixel = usLastY_pixel = 0xffff;                              // ensure next GRAM write starts with a new cursor coordinate being set and the next GRAM write being initiated correctly
    fnConcludeGRAMwrite();                                               // close present GRAM write phase to ensure final pixel is displayed
    #endif
    if (iStart == 0) {
        uTaskerStateChange(OWN_TASK, UTASKER_STOP);                      // switch to event mode of operation since write has completed
    }
#endif
    return STATE_LCD_READY;
}


// Clear backup memory and and GLCD memory content. Used only once after reset
//
static void fnClearScreen(void)
{
#if !defined OLED_GLCD_MODE && !defined TFT_GLCD_MODE
    #if defined NOKIA_GLCD_MODE
    int iWrites = ((GLCD_X + 2) * (GLCD_Y + 2));
    unsigned char g_pucBuffer[3];
    #elif defined _GLCD_SAMSUNG
    unsigned char ucPagesY, ucColX;
    unsigned char ucChip = GLCD_CHIP0;
    #elif defined ST7565S_GLCD_MODE                                      // {19}
    unsigned char ucPagesY, ucColX;
    #elif !defined CGLCD_GLCD_MODE && !defined KITRONIX_GLCD_MODE && !defined TFT2N0369_GLCD_MODE && !defined MB785_GLCD_MODE
    int iWrites = (X_BYTES * Y_BYTES) + (X_BYTES * Y_BYTES);             // size of graphic and text areas
    #endif
#endif
    uMemset(ucPixelArray, 0x00, sizeof(ucPixelArray));                   // initialise screen backup memory
#if defined CGLCD_GLCD_MODE || defined KITRONIX_GLCD_MODE || defined MB785_GLCD_MODE || defined TFT2N0369_GLCD_MODE
    uMemset(ucByteUpdateArray, 0xff, sizeof(ucByteUpdateArray));         // cause a complete display refresh
    if (fnSmartUpdate(1) == STATE_LCD_WRITING) {                         // we must yield but aren't complete with the initialisation
        iLCD_State = STATE_LCD_CLEARING_DISPLAY;
    }
#else
    uMemset(ucByteUpdateArray, 0x00, sizeof(ucByteUpdateArray));         // initialise update array
    #if defined NOKIA_GLCD_MODE
    WriteSpiCommand(CASET);
    WriteSpiData(0);                  
    WriteSpiData(DISPLAY_RIGHT_PIXEL + 2);
    WriteSpiCommand(PASET);
    WriteSpiData(0);                  
    WriteSpiData(DISPLAY_BOTTOM_PIXEL + 2);
    WriteSpiCommand(RAMWR);                                              // data follows
    g_pucBuffer[0] = (unsigned char)(_LCD_ON_COLOUR >> 4);
    g_pucBuffer[1] = (unsigned char)((_LCD_ON_COLOUR << 4) | (_LCD_ON_COLOUR >> 8));
    g_pucBuffer[2] = (unsigned char)(_LCD_ON_COLOUR);
    while (iWrites) {
        fnSend_SPI_data(g_pucBuffer, 3);                                 // write 2 pixels to display (involving 3 bytes to be sent)
        iWrites -= 2;
    }
    #elif !defined OLED_GLCD_MODE && !defined TFT_GLCD_MODE
        #if defined _GLCD_SAMSUNG                                        // Samsung controller
    for (ucPagesY = 0; ucPagesY < 8; ucPagesY++) {                       // clear each GLCD display location
        fnGLCDGotoXY(0, (unsigned char)(ucPagesY * 8));                  // pixel position
        for (ucColX = 0; ucColX < GLCD_X; ucColX++) {                    // for each column            
            if (0 == ucColX) {                                           // select appropriate chip
                ucChip = GLCD_CHIP0;
            }
            else if (SINGLE_CHIP_WIDTH == ucColX) {              
                fnGLCDGotoXY(SINGLE_CHIP_WIDTH, (unsigned char)(ucPagesY * 8));
                ucChip = GLCD_CHIP1;
            }            
            while (GLCD_BUSY()) {}
            fnWriteGLCD_data(0, ucChip);
        }
    }
    fnGLCDGotoXY(0, 0);
        #elif defined  ST7565S_GLCD_MODE                                 // {19}
    for (ucPagesY = 0; ucPagesY < 8; ucPagesY++) {                       // clear each GLCD display location
        fnGLCDGotoXY(0, (unsigned char)(ucPagesY * 8));                  // pixel position
        for (ucColX = 0; ucColX < GLCD_X; ucColX++) {                    // for each column            
            while (GLCD_BUSY()) {}
            fnWriteGLCD_data(0x00);
        }
    }
    fnGLCDGotoXY(0, 0);
        #else                                                            // Toshiba controller
    fnCommandGlcd_2(SET_ADDRESS_POINTER, 0x0000);                        // set the address pointer to the start of graphic memory
    while (iWrites--) {
        fnCommandGlcd_1(WRITE_DATA_INC, 0x00);                           // clear the GLDC memory location
    }
        #endif
    #endif
#endif
}



#if !defined TFT_GLCD_MODE && !defined CGLCD_GLCD_MODE && !defined KITRONIX_GLCD_MODE && !defined MB785_GLCD_MODE && !defined TFT2N0369_GLCD_MODE
// Check that the GLCD can be detected after a reset
//
static int fnCheckLCD(void)
{
    int iMaxWait = MAX_GLCD_READY;
    while (GLCD_BUSY() != 0) {                                           // it is assumed that pull downs are set to ensure that the GLCD is not detected when not connected
        if (--iMaxWait == 0) {
            GLCD_RST_L();                                                // activate reset again in case a new attempt should be made later
            SET_PULL_DOWNS();
            return 1;                                                    // no GLCD detected
        }
    }
    REMOVE_PULL_DOWNS();                                                 // disable pull downs during normal operation
    return 0;                                                            // GLCD has been detected
}
#endif




// Text message causing the GLCD to display the text at the defined position, using the defined font
//
extern void fnDoLCD_text(GLCD_TEXT_POSITION *text_pos, const CHAR *cText)
{
    unsigned char ucMessage[HEADER_LENGTH + 1 + sizeof(GLCD_TEXT_MESSAGE)];
    unsigned char ucLength;

    ucMessage[MSG_DESTINATION_NODE] = INTERNAL_ROUTE;                    // destination node
    ucMessage[MSG_SOURCE_NODE]      = INTERNAL_ROUTE;                    // own node
    ucMessage[MSG_DESTINATION_TASK] = TASK_LCD;                          // destination task
    ucMessage[MSG_SOURCE_TASK]      = LCD_PARTNER_TASK;                  // partner task
    ucMessage[MSG_CONTENT_COMMAND]  = E_LCD_TEXT;                        // message type
    uMemcpy(&ucMessage[MSG_CONTENT_COMMAND + 1], text_pos, sizeof(GLCD_TEXT_POSITION));
    ucLength = (unsigned char)((unsigned char *)uStrcpy((CHAR *)&ucMessage[MSG_CONTENT_COMMAND + 1 + sizeof(GLCD_TEXT_POSITION)], cText) - &ucMessage[MSG_CONTENT_COMMAND + sizeof(GLCD_TEXT_POSITION)]);
    ucLength += sizeof(GLCD_TEXT_POSITION);
    ucMessage[MSG_CONTENT_LENGTH]   = ucLength;                          // message content length
    fnWrite(INTERNAL_ROUTE, ucMessage, (QUEUE_TRANSFER)(ucLength + HEADER_LENGTH)); // send message to defined task
}


// Image message causing the GLCD to display the image (pointer to image used) at the defined position
//
extern void fnDoLCD_pic(void *pic)                                       // {13}
{
#if defined GLCD_PIC_WINDOWING
    unsigned char ucMessage[HEADER_LENGTH + 1 + sizeof(GLCD_PIC_WINDOW)];
#else
    unsigned char ucMessage[HEADER_LENGTH + 1 + sizeof(GLCD_PIC)];
#endif

    ucMessage[MSG_DESTINATION_NODE] = INTERNAL_ROUTE;                    // destination node
    ucMessage[MSG_SOURCE_NODE]      = INTERNAL_ROUTE;                    // own node
    ucMessage[MSG_DESTINATION_TASK] = TASK_LCD;                          // destination task
    ucMessage[MSG_SOURCE_TASK]      = LCD_PARTNER_TASK;                  // partner task
#if defined GLCD_PIC_WINDOWING
    if (((GLCD_PIC_WINDOW *)pic)->glcd_pic.ucMode & WINDOWED_PIC) {      // {13}
        ucMessage[MSG_CONTENT_LENGTH] = (1 + sizeof(GLCD_PIC_WINDOW));   // message content length
        ucMessage[MSG_CONTENT_COMMAND] = E_LCD_WINDOWED_PIC;             // message type
        uMemcpy(&ucMessage[MSG_CONTENT_COMMAND + 1], pic, sizeof(GLCD_PIC_WINDOW));
    }
    else {
        ucMessage[MSG_CONTENT_LENGTH] = (1 + sizeof(GLCD_PIC));          // message content length
        ucMessage[MSG_CONTENT_COMMAND] = E_LCD_PIC;                      // message type
        uMemcpy(&ucMessage[MSG_CONTENT_COMMAND + 1], pic, sizeof(GLCD_PIC));
    }
#else
    ucMessage[MSG_CONTENT_LENGTH] = (1 + sizeof(GLCD_PIC));              // message content length
    ucMessage[MSG_CONTENT_COMMAND] = E_LCD_PIC;                          // message type
    uMemcpy(&ucMessage[MSG_CONTENT_COMMAND + 1], pic, sizeof(GLCD_PIC));
#endif
    fnWrite(INTERNAL_ROUTE, ucMessage, (QUEUE_TRANSFER)(HEADER_LENGTH + ucMessage[MSG_CONTENT_LENGTH])); // send message to defined task
}

// Message causing the GLCD to draw a line
//
extern void fnDoLCD_line(void *line)
{
    int iLength = (sizeof(GLCD_LINE) + 1);
    unsigned char ucMessage[HEADER_LENGTH + 1 + sizeof(GLCD_LINE_BLINK)];

    ucMessage[MSG_DESTINATION_NODE] = INTERNAL_ROUTE;                    // destination node
    ucMessage[MSG_SOURCE_NODE]      = INTERNAL_ROUTE;                    // own node
    ucMessage[MSG_DESTINATION_TASK] = TASK_LCD;                          // destination task
    ucMessage[MSG_SOURCE_TASK]      = LCD_PARTNER_TASK;                  // partner task
    ucMessage[MSG_CONTENT_COMMAND]  = E_LCD_LINE;                        // message type
    if (((GLCD_LINE *)line)->ucMode & (BLINKING_OBJECT)) {
        iLength += (sizeof(GLCD_LINE_BLINK) - sizeof(GLCD_LINE));        // extended command
    }
    ucMessage[MSG_CONTENT_LENGTH]   = iLength;                           // message content length
    uMemcpy(&ucMessage[MSG_CONTENT_COMMAND + 1], line, (iLength - 1));
    fnWrite(INTERNAL_ROUTE, ucMessage, (QUEUE_TRANSFER)(HEADER_LENGTH + iLength)); // send message to defined task
}

// Message causing the GLCD to draw a filled rectangle
//
extern void fnDoLCD_rect(void *rect)
{
    int iLength = (sizeof(GLCD_RECT) + 1);
    unsigned char ucMessage[HEADER_LENGTH + 1 + sizeof(GLCD_RECT_BLINK)];

    ucMessage[MSG_DESTINATION_NODE] = INTERNAL_ROUTE;                    // destination node
    ucMessage[MSG_SOURCE_NODE]      = INTERNAL_ROUTE;                    // own node
    ucMessage[MSG_DESTINATION_TASK] = TASK_LCD;                          // destination task
    ucMessage[MSG_SOURCE_TASK]      = LCD_PARTNER_TASK;                  // partner task
    ucMessage[MSG_CONTENT_COMMAND]  = E_LCD_RECT;                        // message type
    if (((GLCD_RECT *)rect)->ucMode & (BLINKING_OBJECT)) {               // extended command
        iLength += (sizeof(GLCD_RECT_BLINK) - sizeof(GLCD_RECT));
    }
    ucMessage[MSG_CONTENT_LENGTH ]   = (unsigned char)iLength;          // message content length
    uMemcpy(&ucMessage[MSG_CONTENT_COMMAND + 1], rect, (iLength - 1));
    fnWrite(INTERNAL_ROUTE, ucMessage, (QUEUE_TRANSFER)(HEADER_LENGTH + iLength)); // send message to defined task
}

// Message causing the display to scroll
//
extern void fnDoLCD_scroll(GLCD_SCROLL *scroll)                          // {4}
{
    unsigned char ucMessage[HEADER_LENGTH + 1 + sizeof(GLCD_SCROLL)];

    ucMessage[MSG_DESTINATION_NODE] = INTERNAL_ROUTE;                    // destination node
    ucMessage[MSG_SOURCE_NODE]      = INTERNAL_ROUTE;                    // own node
    ucMessage[MSG_DESTINATION_TASK] = TASK_LCD;                          // destination task
    ucMessage[MSG_SOURCE_TASK]      = LCD_PARTNER_TASK;                  // partner task
    ucMessage[MSG_CONTENT_COMMAND]  = E_LCD_SCROLL;                      // message type
    ucMessage[MSG_CONTENT_LENGTH]   = (sizeof(GLCD_SCROLL) + 1);         // message content length
    uMemcpy(&ucMessage[MSG_CONTENT_COMMAND + 1], scroll, sizeof(GLCD_SCROLL));
    fnWrite(INTERNAL_ROUTE, ucMessage, (QUEUE_TRANSFER)(HEADER_LENGTH + (sizeof(GLCD_SCROLL) + 1))); // send message to defined task
}


#if defined GLCD_BACKLIGHT_CONTROL
extern void fnSetBacklight(void)
{
    if (temp_pars->temp_parameters.ucGLCDBacklightPWM >= 96) {           // consider as maximum brightness and apply constant '1'
        BACK_LIGHT_MAX_INTENSITY();
    }
    else if (temp_pars->temp_parameters.ucGLCDBacklightPWM == 0) {       // consider as off and apply constant '0'
        BACK_LIGHT_MIN_INTENSITY();
    }
    else {
    #if defined _M5223X || defined _KINETIS
        #if defined K70F150M_12M                                         // use LCD controller's contrast PWM to control the backlight intensity
        LCDC_LPCCR = ((LCDC_LPCCR_SCR_LINE_CLK | LCDC_LPCCR_SCR_PIXEL_CLK | LCDC_LPCCR_CC_EN) | ((temp_pars->temp_parameters.ucGLCDBacklightPWM * MAX_LCD_INTENSITY_VALUE)/100));
        _CONFIG_PERIPHERAL(D, 11, PD_11_GLCD_GLCD_CONTRAST);
        #else
        PWM_INTERRUPT_SETUP timer_setup;
        timer_setup.int_type = PWM_INTERRUPT;                            // PWM setup type
        timer_setup.pwm_reference = _GLCD_BACKLIGHT_TIMER;               // the timer to generate the control signal with
        timer_setup.pwm_mode  = _GLCD_TIMER_MODE_OF_OPERATION;           // generate PWM signal on this timer output port
        timer_setup.pwm_frequency = _GLCD_BACKLIGHT_PWM_FREQUENCY;       // backlight frequency
        timer_setup.pwm_value  = (unsigned char)_PWM_PERCENT(temp_pars->temp_parameters.ucGLCDBacklightPWM, timer_setup.pwm_frequency); // contrast as PWM value
        fnConfigureInterrupt((void *)&timer_setup);                      // configure PWM output for contrast control
        #endif
    #else
        TIMER_INTERRUPT_SETUP timer_setup;                               // PWM Timer Init Struct
        timer_setup.int_type = TIMER_INTERRUPT;                          // timer setup type
        timer_setup.int_priority = 0;
        timer_setup.int_handler = 0;                                     // no interrupts used
        timer_setup.timer_reference = _GLCD_BACKLIGHT_TIMER;             // the timer to generate the control signal with
        timer_setup.timer_mode  = _GLCD_TIMER_MODE_OF_OPERATION;         // generate PWM signal on this timer output port
        timer_setup.timer_value = _GLCD_BACKLIGHT_PWM_FREQUENCY;         // backlight frequency
        timer_setup.pwm_value   = (unsigned short)_PWM_PERCENT(temp_pars->temp_parameters.ucGLCDBacklightPWM, timer_setup.timer_value); // contrast as PWM value
        fnConfigureInterrupt((void *)&timer_setup);                      // configure PWM output for contrast control
    #endif
    }
}        
#endif

#if defined SUPPORT_TOUCH_SCREEN && defined MB785_GLCD_MODE
#define MIN_X_TOUCH        0x00f0                                        // tuned values - for calibration these should be taken from parameters
#define MAX_X_TOUCH        0x0f26
#define MIN_Y_TOUCH        0x0110
#define MAX_Y_TOUCH        0x0f10

#define MAX_MOVEMENT_Y     12
#define MAX_MOVEMENT_X     12

static void fnHandleTouch(unsigned short usX, unsigned short usY, int iPenDown)
{
    static unsigned short usLastPixelX, usLastPixelY;
    unsigned char ucMemoryContent, ucNewContent;

    if (usX < MIN_X_TOUCH) {
        usX = MIN_X_TOUCH;
    }
    else if (usX > MAX_X_TOUCH) {
        usX = MAX_X_TOUCH;
    }
    if (usY < MIN_Y_TOUCH) {
        usY = MIN_Y_TOUCH;
    }
    else if (usY > MAX_Y_TOUCH) {
        usY = MAX_Y_TOUCH;
    }

    usX = (unsigned short)(((unsigned long)((usX - MIN_X_TOUCH) * (GLCD_X - 1)))/(MAX_X_TOUCH - MIN_X_TOUCH)); // convert to pixel location
    usY = (unsigned short)(((unsigned long)((usY - MIN_Y_TOUCH) * (GLCD_Y - 1)))/(MAX_Y_TOUCH - MIN_Y_TOUCH));

    if (!iPenDown) {
    #ifndef _WINDOWS
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
    #endif
    }
    usLastPixelX = usX;                                                  // save last valid pixel location
    usLastPixelY = usY;
    #if CGLCD_PIXEL_SIZE > 1
    usY /= CGLCD_PIXEL_SIZE;
    usX /= CGLCD_PIXEL_SIZE;
    #endif
    ucNewContent = ucMemoryContent = ucPixelArray[usY][usX/8];           // original content
    ucNewContent |= (0x80 >> usX%8);                                     // set the pixel
    if (ucNewContent != ucMemoryContent) {
        ucPixelArray[usY][usX/8] = ucNewContent;
        ucByteUpdateArray[usY][usX/64] |= (0x80 >> (usX/8)%8);           // mark the need for an update
    }
}
#endif

#if defined VARIABLE_PIXEL_COLOUR
// Message causing a change to graphic styles
//
extern void fnDoLCD_style(GLCD_STYLE *style)                             // {7}
{
    unsigned char ucMessage[HEADER_LENGTH + 1 + sizeof(GLCD_STYLE)];

    ucMessage[MSG_DESTINATION_NODE] = INTERNAL_ROUTE;                    // destination node
    ucMessage[MSG_SOURCE_NODE]      = INTERNAL_ROUTE;                    // own node
    ucMessage[MSG_DESTINATION_TASK] = TASK_LCD;                          // destination task
    ucMessage[MSG_SOURCE_TASK]      = LCD_PARTNER_TASK;                  // partner task
    ucMessage[MSG_CONTENT_COMMAND]  = E_LCD_STYLE;                       // message type
    ucMessage[MSG_CONTENT_LENGTH]   = (sizeof(GLCD_STYLE) + 1);          // message content length
    uMemcpy(&ucMessage[MSG_CONTENT_COMMAND + 1], style, sizeof(GLCD_STYLE));
    fnWrite(INTERNAL_ROUTE, ucMessage, (QUEUE_TRANSFER)(HEADER_LENGTH + (sizeof(GLCD_STYLE) + 1))); // send message to defined task
}
#endif

    #if defined USE_HTTP || defined SDCARD_SUPPORT                       // {16}
extern int iGetPixelState(unsigned long ulPixelNumber)
{
    int x = ((ulPixelNumber%GLCD_X)/CGLCD_PIXEL_SIZE);
    int y = ((ulPixelNumber/GLCD_X)/CGLCD_PIXEL_SIZE);
    unsigned char ucBit;
        #if defined ST7565S_GLCD_MODE                                    // {19} ST7565S is a samsung type
    ucBit = (0x01 << y%8);
    y /= 8;
        #else
    ucBit = (0x80 >> x%8);
    x /= 8;
        #endif
    return ((ucPixelArray[y][x] & ucBit) != 0);                          // return the state of the defined pixel
}
    #endif
#endif

#if defined USE_HTTP || defined SDCARD_SUPPORT || defined SUPPORT_TFT    // {17}
    #if !defined SUPPORT_GLCD || defined GLCD_COLOR
        #define _GLCD_COMMANDS
            #include "glcd_tft.h"
        #undef _GLCD_COMMANDS
    #endif

#if (defined MB785_GLCD_MODE || defined AVR32_EVK1105 || defined IDM_L35_B || defined TFT2N0369_GLCD_MODE || defined OLIMEX_LPC1766_STK || defined OLIMEX_LPC2478_STK || defined IAR_LPC1788_SK || defined TWR_LCD_RGB_GLCD_MODE) && defined GLCD_X // {17}{18} accept color images
    #if defined AVR32_EVK1105
extern void et024006_SetLimits(unsigned short x1, unsigned short y1, unsigned short x2, unsigned short y2)
{
    WRITE_REG(HIMAX_COL_ADDR_START2, (x1 >> 8)  );
    WRITE_REG(HIMAX_COL_ADDR_START1, (x1 & 0xff));
    WRITE_REG(HIMAX_COL_ADDR_END2,   (x2 >> 8)  );
    WRITE_REG(HIMAX_COL_ADDR_END1,   (x2 & 0xff));
    WRITE_REG(HIMAX_ROW_ADDR_START2, (y1 >> 8)  );
    WRITE_REG(HIMAX_ROW_ADDR_START1, (y1 & 0xff));
    WRITE_REG(HIMAX_ROW_ADDR_END2,   (y2 >> 8)  );
    WRITE_REG(HIMAX_ROW_ADDR_END1,   (y2 & 0xff));
}
    #endif


// Receive a bit map from an input - like HTTP port, where the data arrives in multiple frames
//
extern int fnDisplayBitmap(unsigned char *ptrData, unsigned short usLength)
{
#define BMP_IDLE         0
#define BMP_HEADER_READY 1
#define BMP_BAD_FILE     2
#if defined OLIMEX_LPC2478_STK || defined IAR_LPC1788_SK || defined TWR_LCD_RGB_GLCD_MODE // {18}
    static unsigned long *ptDst;
#endif
    static int iState = BMP_IDLE;    
    static unsigned long ulImageHeight;
    static unsigned long ulImageWidthBytes;
    static unsigned long BMP_Length;
    static unsigned long usValidData = 0;
    static unsigned short usBitmapWidth;
    static unsigned char ucCorrection;
    static unsigned char ucBuffer[3 * GLCD_X];
    if (ptrData == 0) {
        iState = BMP_IDLE;                                               // reset ready to accept another bit map
        usValidData = 0;
    }
    while (usLength != 0) {
        switch (iState) {
        case BMP_IDLE:                                                   // bit map header needs to be interpreted
            {
                unsigned short usCopyLength = (unsigned short)((sizeof(W_BITMAPHEADER) + sizeof(W_BITMAPINFO) - usValidData));
                if (usCopyLength > usLength) {                           // part header
                    usCopyLength = usLength;                             // limit to available data
                }
                else {
                    iState = BMP_HEADER_READY;
                }
                uMemcpy(&ucBuffer[usValidData], ptrData, usCopyLength);  // first fill the intermediate buffer with the bitmap header
                ptrData += usCopyLength;
                usValidData += usCopyLength;
                usLength -= usCopyLength;
                if (iState == BMP_HEADER_READY) {                        // header complete
                    W_BITMAPHEADER *ptrBMP = (W_BITMAPHEADER *)ucBuffer;
                    W_BITMAPINFO   *ptrBMP_info = (W_BITMAPINFO *)(ucBuffer + sizeof(W_BITMAPHEADER));
                    if (ptrBMP->bmType[0] != 'B') {
                        iState = BMP_BAD_FILE;
                        return -1;                                       // not a bit map
                    }
                    if (ptrBMP->bmType[1] != 'M') {
                        iState = BMP_BAD_FILE;
                        return -1;                                       // not a bit map
                    }
                    if (ptrBMP_info->biBitCount[0] != 24) {
                        iState = BMP_BAD_FILE;
                        return -2;                                       // 24 bit color expected
                    }
                    if (ptrBMP_info->biCompression[0] != 0) {
                        iState = BMP_BAD_FILE;
                        return -3;                                       // compressed image not supported
                    }
                    BMP_Length = (ptrBMP_info->biSizeImage[3] << 24);    // bit map content length
                    BMP_Length |= (ptrBMP_info->biSizeImage[2] << 16);
                    BMP_Length |= (ptrBMP_info->biSizeImage[1] << 8);
                    BMP_Length |= (ptrBMP_info->biSizeImage[0]);
                    if (BMP_Length == 0) {
                        BMP_Length = (ptrBMP->bmLength[3] << 24);        // if the content length is zero, take it from the header
                        BMP_Length |= (ptrBMP->bmLength[2] << 16);
                        BMP_Length |= (ptrBMP->bmLength[1] << 8);
                        BMP_Length |= (ptrBMP->bmLength[0]);
                        BMP_Length -= ptrBMP->bmOffBits[0];
                    }
                    ulImageWidthBytes = (ptrBMP_info->biWidth[1] << 8);
                    ulImageWidthBytes |= (ptrBMP_info->biWidth[0]);
                    ulImageHeight = (ptrBMP_info->biHeight[1] << 8);
                    ulImageHeight |= (ptrBMP_info->biHeight[0]);
                    if ((ulImageWidthBytes > GLCD_X) || (ulImageHeight > GLCD_Y)) { // {21}
                        iState = BMP_BAD_FILE;
                        return -4;                                       // image size too large for the LCD
                    }
                    ucCorrection = (unsigned char)(ulImageWidthBytes%4);
                    usBitmapWidth = (unsigned short)ulImageWidthBytes;
                    ulImageWidthBytes *= 3;                              // three color bytes per pixel
                    usValidData = 0;
#if defined OLIMEX_LPC2478_STK || defined IAR_LPC1788_SK || defined TWR_LCD_RGB_GLCD_MODE // {18}
                    ptDst = (unsigned long *)fnGetSDRAM((unsigned char *)SDRAM_ADDR);
                    ptDst += ((ulImageHeight - 1) * GLCD_X);             // inverted bit maps, so move to bottom of image ready to draw upwards
#endif
                }
            }
            break;

        case BMP_HEADER_READY:
            {
                unsigned short usCopyLength = (unsigned short)(ulImageWidthBytes + ucCorrection - usValidData);
                unsigned long x;
                unsigned short usBufferCopy;
                if (usLength < usCopyLength) {
                    usCopyLength = usLength;
                }
                if (BMP_Length < usCopyLength) {
                    usCopyLength = (unsigned short)BMP_Length;
                }
                if ((usCopyLength + usValidData) > sizeof(ucBuffer)) {   // limit to buffer length
                    usBufferCopy = (unsigned short)(sizeof(ucBuffer) - usValidData);
                }
                else {
                    usBufferCopy = usCopyLength;
                }
                uMemcpy(&ucBuffer[usValidData], ptrData, usBufferCopy);  // first fill the intermediate buffer with the bitmap content
                ptrData += usCopyLength;
                usValidData += usCopyLength;
                usLength -= usCopyLength;
#if defined AVR32_EVK1105
                if (usValidData >= (ulImageWidthBytes + ucCorrection)) { // a complete line has been collected so write it to the display
                    unsigned long ulPixel;
                    ulImageHeight--;
                    et024006_SetLimits(0, (unsigned short)(ulImageHeight), usBitmapWidth, (unsigned short)(ulImageHeight));
                    SELECT_REG(HIMAX_SRAMWRITE);                         // prepare write
                    for (x = 0; x < ulImageWidthBytes; x += 3) {
                        ulPixel = ((ucBuffer[x] >> 3) | ((ucBuffer[x + 1] & 0xfc) << 3) | ((ucBuffer[x + 2] & 0xf8) << 8));
                        WRITE_DATA((unsigned short)ulPixel);
                    }
                    BMP_Length -= (ulImageWidthBytes + ucCorrection);
                    usValidData = 0;                                     // complete buffer emptied
                }
#elif defined IDM_L35_B || defined TFT2N0369_GLCD_MODE
                if (usValidData >= (ulImageWidthBytes + ucCorrection)) { // a complete line has been collected so write it to the display
                    unsigned long ulPixel;
                    ulImageHeight--;
                    fnSetWindow(0, (unsigned short)(ulImageHeight), usBitmapWidth, (unsigned short)(ulImageHeight));
                    for (x = 0; x < ulImageWidthBytes; x += 3) {
                        ulPixel = ((ucBuffer[x] >> 3) | ((ucBuffer[x + 1] & 0xfc) << 3) | ((ucBuffer[x + 2] & 0xf8) << 8));
                        fnWriteGLCD_data_pair((unsigned short)ulPixel);
                    }
                    BMP_Length -= (ulImageWidthBytes + ucCorrection);
                    usValidData = 0;                                     // complete buffer emptied
                }
#elif defined OLIMEX_LPC2478_STK || defined IAR_LPC1788_SK || defined TWR_LCD_RGB_GLCD_MODE // {18}
                if (usValidData >= (ulImageWidthBytes + ucCorrection)) { // a complete line has been collected so write it to the display
                    unsigned long *ptNextLine = ptDst - GLCD_X;          // position of next line, moving upwards
                    unsigned long ulPixel;
                    for (x = 0; x < ulImageWidthBytes; x += 3) {
                        ulPixel = ((ucBuffer[x] << 16) | (ucBuffer[x + 1] << 8) | ucBuffer[x + 2]);
                        *ptDst++ = ulPixel;
                    }
                    ptDst = ptNextLine;
                    BMP_Length -= (ulImageWidthBytes + ucCorrection);
                    usValidData = 0;
            #if defined _WINDOWS
                    CollectCommand(0, (unsigned long)fnGetSDRAM((unsigned char *)SDRAM_ADDR));
            #endif
                }
#elif defined OLIMEX_LPC1766_STK
                while (usValidData >= /*(ulImageWidthBytes + ucCorrection)*/sizeof(ucBuffer)) { // a complete line has been collected so write it to the display
//                  unsigned long ulPixel;
                  //#define WINDOW_WIDTH  DISPLAY_RIGHT_PIXEL
                    static unsigned char pucWindowX[] = {CASET, 0, DISPLAY_RIGHT_PIXEL};
                    static unsigned char pucWindowY[] = {PASET, 0, DISPLAY_BOTTOM_PIXEL};
                    ulImageHeight--;
                    if (ulImageHeight < DISPLAY_BOTTOM_PIXEL) {          // don't write any lines outside of the display area
                        int i = 0;
                        pucWindowY[1] = (unsigned char)ulImageHeight;
                        fnSendSPI_Command(pucWindowX, sizeof(pucWindowX));
                        fnSendSPI_Command(pucWindowY, sizeof(pucWindowY));
                        WriteSpiCommand(RAMWR);                              // data follows
                        for (x = 0; x < GLCD_X;) {
                            ucBuffer[x] = (ucBuffer[i++] >> 4);
                            ucBuffer[x] <<= 4;
                            ucBuffer[x++] |= (ucBuffer[i++] >> 4);
                            ucBuffer[x] <<= 4;
                            ucBuffer[x++] |= (ucBuffer[i++] >> 4);
                        }
                        fnSend_SPI_data(ucBuffer, GLCD_X);
                    }
                    BMP_Length -= (ulImageWidthBytes + ucCorrection);
                    usValidData -= sizeof(ucBuffer);                     // complete buffer emptied
                }
#else
                if (usValidData >= (ulImageWidthBytes + ucCorrection)) { // a complete line has been collected so write it to the display
                    unsigned long ulPixel;                  //unsigned char ucBlue = 0, ucGreen = 0, ucRed = 0;
                //  et024006_SetLimits(0, (unsigned short)(ulImageHeight), usBitmapWidth, (unsigned short)(ulImageHeight));
                    fnSetWindow(0, (unsigned short)(--ulImageHeight), DISPLAY_RIGHT_PIXEL, DISPLAY_BOTTOM_PIXEL); // set to start of line and open write
                    for (x = 0; x < ulImageWidthBytes; x += 3) {
                       /* if (ucBlue < 0x20) {
                            ulPixel = ucBlue++;
                        }
                        else if (ucGreen < 0x40) {
                            ulPixel = (ucGreen++ << 5);
                        }
                        else if (ucRed < 0x20) {
                            ulPixel = (ucRed++ << 11);
                        }
                        else {
                            ulPixel = ((ucBuffer[x] >> 3) | ((ucBuffer[x + 1] & 0xfc) << 3) | ((ucBuffer[x + 2] & 0xf8) << 8)); // convert color format
                        }*/
                        ulPixel = ((ucBuffer[x] >> 3) | ((ucBuffer[x + 1] & 0xfc) << 3) | ((ucBuffer[x + 2] & 0xf8) << 8)); // convert color format
                        fnWriteGLCD_data_pair((unsigned short)ulPixel);  // write to display
                    }
                    fnConcludeGRAMwrite();                               // close present GRAM write phase to ensure final pixel is displayed
                    BMP_Length -= (ulImageWidthBytes + ucCorrection);
                    usValidData = 0;                                     // complete buffer emptied
                }
#endif
                if (BMP_Length == 0) {
                    iState = BMP_IDLE;
                    return 1;                                            // bit map has been completely written to the display
                }
            }
            break;

        case BMP_BAD_FILE:                                               // data is not valid so ignore
            return -1;
        }
    }
    return 0;
}
#elif defined GLCD_X && !defined SUPPORT_SLCD                            // {17}
// Receive a bit map from an input - like HTTP port, where the data arrives in multiple frames
//
extern int fnDisplayBitmap(unsigned char *ptrData, unsigned short usLength)
{
#define BMP_IDLE         0
#define BMP_HEADER_READY 1
#define BMP_BAD_FILE     2
    static int iBMPState = BMP_IDLE;
    static unsigned char *ptDst;
    static unsigned char *ptrChanged;
    static unsigned long ulImageHeight;
    static unsigned long ulImageWidthBytes;
    static unsigned long BMP_Length;
    static unsigned char ucBuffer[3 * GLCD_X];
    static unsigned long usValidData = 0;
    static unsigned char ucCorrection;
    #if defined ST7565S_GLCD_MODE                                        // {19} Samsung type 
    static unsigned char ucPixel = 0x80;
    static unsigned char ucChangedBit = 0x80;
    #endif
    if (ptrData == 0) {
        iBMPState = BMP_IDLE;                                            // reset ready to accept another bit map
        usValidData = 0;
        uTaskerStopTimer(LCD_PARTNER_TASK);                              // stop further display updates by the application
        iSendAck = 0;                                                    // ensure no acknowledgement sent to application on draw completion
    }
    while (usLength != 0) {
        switch (iBMPState) {
        case BMP_IDLE:                                                   // bit map header needs to be interpreted
            {
                unsigned short usCopyLength = (unsigned short)((sizeof(W_BITMAPHEADER) + sizeof(W_BITMAPINFO) - usValidData));
                if (usCopyLength > usLength) {                           // part header
                    usCopyLength = usLength;                             // limit to available data
                }
                else {
                    iBMPState = BMP_HEADER_READY;
                }
                uMemcpy(&ucBuffer[usValidData], ptrData, usCopyLength);  // first fill the intermediate buffer with the bitmap header
                ptrData += usCopyLength;
                usValidData += usCopyLength;
                usLength -= usCopyLength;
                if (iBMPState == BMP_HEADER_READY) {                     // header complete
                    W_BITMAPHEADER *ptrBMP = (W_BITMAPHEADER *)ucBuffer;
                    W_BITMAPINFO   *ptrBMP_info = (W_BITMAPINFO *)(ucBuffer + sizeof(W_BITMAPHEADER));
                    if (ptrBMP->bmType[0] != 'B') {
                        iBMPState = BMP_BAD_FILE;
                        return -1;                                       // not a bit map
                    }
                    if (ptrBMP->bmType[1] != 'M') {
                        iBMPState = BMP_BAD_FILE;
                        return -1;                                       // not a bit map
                    }
                    if (ptrBMP_info->biBitCount[0] != 24) {
                        iBMPState = BMP_BAD_FILE;
                        return -2;                                       // 24 bit color expected
                    }
                    if (ptrBMP_info->biCompression[0] != 0) {
                        iBMPState = BMP_BAD_FILE;
                        return -3;                                       // compressed image not supported
                    }
                    BMP_Length = (ptrBMP_info->biSizeImage[3] << 24);    // bit map content length
                    BMP_Length |= (ptrBMP_info->biSizeImage[2] << 16);
                    BMP_Length |= (ptrBMP_info->biSizeImage[1] << 8);
                    BMP_Length |= (ptrBMP_info->biSizeImage[0]);
                    if (BMP_Length == 0) {
                        BMP_Length = (ptrBMP->bmLength[3] << 24);        // if the content length is zero, take it from the header
                        BMP_Length |= (ptrBMP->bmLength[2] << 16);
                        BMP_Length |= (ptrBMP->bmLength[1] << 8);
                        BMP_Length |= (ptrBMP->bmLength[0]);
                        BMP_Length -= ptrBMP->bmOffBits[0];
                    }
                    ulImageWidthBytes = (ptrBMP_info->biWidth[1] << 8);
                    ulImageWidthBytes |= (ptrBMP_info->biWidth[0]);
                    ulImageHeight = (ptrBMP_info->biHeight[1] << 8);
                    ulImageHeight |= (ptrBMP_info->biHeight[0]);
                    if ((ulImageWidthBytes > GLCD_X) || (ulImageHeight > GLCD_Y)) { // {21}
                        iBMPState = BMP_BAD_FILE;
                        return -4;                                       // image size too large for the LCD
                    }
                    ucCorrection = (unsigned char)(ulImageWidthBytes%4);
                    ulImageWidthBytes *= 3;                              // three color bytes per pixel
                    usValidData = 0;
                    ptDst = &ucPixelArray[0][0];
                    ptrChanged = &ucByteUpdateArray[0][0];
    #if defined ST7565S_GLCD_MODE                                        // {19}
                    ptDst += (((ulImageHeight/8) - 1) * GLCD_X);         // inverted bit maps, so move to bottom of image ready to draw upwards
                    ptrChanged += (((ulImageHeight - 1) / 64) * UPDATE_WIDTH);                   
    #else
                    ptDst += ((ulImageHeight - 1) * (GLCD_X/8));         // inverted bit maps, so move to bottom of image ready to draw upwards
                    ptrChanged += ((ulImageHeight - 1) * UPDATE_WIDTH);                   
    #endif
                }
            }
            break;

        case BMP_HEADER_READY:
            {
                unsigned short usCopyLength = (unsigned short)(ulImageWidthBytes + ucCorrection - usValidData);
                unsigned long x;
                if (usLength < usCopyLength) {
                    usCopyLength = usLength;
                }
                if (BMP_Length < usCopyLength) {
                    usCopyLength = (unsigned short)BMP_Length;
                }
                uMemcpy(&ucBuffer[usValidData], ptrData, usCopyLength);  // first fill the intermediate buffer with the bitmap content
                ptrData += usCopyLength;
                usValidData += usCopyLength;
                usLength -= usCopyLength;
                if (usValidData >= (ulImageWidthBytes + ucCorrection)) { // a complete line has been collected so write it to the display
                    unsigned long ulPixel;
                    unsigned char ucOriginal = *ptDst;
    #if defined ST7565S_GLCD_MODE                                        //  {19}
                    unsigned char *ptNextLine = ptDst;
                    unsigned char *ptNextChange = ptrChanged;
    #else
                    unsigned char ucPixel = 0x80;
                    unsigned char *ptNextLine = ptDst - (GLCD_X/8);      // position of next line, moving upwards
                    unsigned char *ptNextChange = ptrChanged - UPDATE_WIDTH; // position of next change, moving upwards
                    unsigned char ucChangedBit = 0x80;
    #endif
    #if defined ST7565S_GLCD_MODE                                        //  {19}
                    ulImageHeight--;
                    for (x = 0; x < ulImageWidthBytes; x += 3) {         // write a complete pixel line 
                        ulPixel = ucBuffer[x] + ucBuffer[x + 1] + ucBuffer[x + 2];
                        if (ulPixel >= (3 * 0xf0)) {                     // cut-off for white
                            *ptDst |= ucPixel;
                        }
                        else {
                            *ptDst &= ~ucPixel;
                        }
                        if (ucOriginal != *ptDst) {                      // check whether a byte has changed
                            *ptrChanged |= ucChangedBit;                 // mark the byte to be updated
                        }
                        ptDst++;
                        ptrChanged++;
                        ucOriginal = *ptDst;
                    }
                    ptrChanged = ptNextChange;
                    ucPixel >>= 1;                                       // next line upwards
                    if (ucPixel == 0)  {
                        ucPixel = 0x80;                                  // start with the last
                        ptDst = (ptNextLine - GLCD_X);                   // position of next line in memory, moving upwards
                        ucChangedBit >>= 1;                              // next change line, upwards 
                        if (ucChangedBit == 0) {
                            ucChangedBit = 0x80;                         // start with the last
                            ptrChanged = ptNextChange - UPDATE_WIDTH;    // position of next change line in memory, moving upwards
                        }
                    }
                    else {
                        ptDst = ptNextLine;
                    }
    #else
                    ulImageHeight--;
                    for (x = 0; x < ulImageWidthBytes; x += 3) {
                        ulPixel = ucBuffer[x] + ucBuffer[x + 1] + ucBuffer[x + 2];
                        if (ulPixel >= (3 * 0xf0)) {                     // cut-off for white
                            *ptDst |= ucPixel;
                        }
                        else {
                            *ptDst &= ~ucPixel;
                        }
                        ucPixel >>= 1;
                        if (ucPixel == 0) {
                            ucPixel = 0x80;
                            if (ucOriginal != *ptDst) {                  // check whether a byte has changed
                                *ptrChanged |= ucChangedBit;             // mark the byte to be updated
                            }
                            ucChangedBit >>= 1;
                            if (ucChangedBit == 0) {
                                ucChangedBit = 0x80;
                                ptrChanged++;
                            }
                            ptDst++;
                            ucOriginal = *ptDst;
                        }
                    }
                    ptDst = ptNextLine;
                    ptrChanged = ptNextChange;
    #endif
                    BMP_Length -= (ulImageWidthBytes + ucCorrection);
                    usValidData = 0;                                     // complete buffer emptied
                }
                if (BMP_Length == 0) {
                    iBMPState = BMP_IDLE;
                    iLCD_State = fnSmartUpdate(1);                       // command an update of the display
                    return 1;                                            // bit map has been completely written to the display
                }
            }
            break;

        case BMP_BAD_FILE:                                               // data is not valid so ignore
            return -1;
        }
    }
    return 0;
}
#endif
#endif
