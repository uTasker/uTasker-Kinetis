/**********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher

    ---------------------------------------------------------------------
    File:      application_lcd.h
    Project:   Single Chip Embedded Internet
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2016
    *********************************************************************
    06.08.2009 Add OLED graphical demo                                    {1}
    11.08.2009 Add line and rectangle drawing tests                       {2}
    26.09.2009 Move GLCD message routines to GLCD.c in uGLCDLIB           {3}
    27.09.2009 Add scrolling test to the demo                             {4}
    16.01.2010 Remove OLED_X/Y and use only GLCD_X/Y
    21.05.2011 Add picture frame demo to TWR_K60N512                      {5}
    18.11.2013 Change a string initialisation to avoid a problem with newer Codewarrior versions {6}

The file is otherwise not specifically linked in to the project since it
is included by application.c when needed.

*/        


//#define TEST_DISPLAY_TTY                                               // test using LCD as simple debug output


#if defined SUPPORT_LCD && !defined _SUPPORT_LCD
  //#define LCD_READ_TEST                                                // test read of character LCD content

    #define LCD_TEST_MAX_SHIFT   5
    #define LCD_TEST_STARTING    (LCD_TEST_MAX_SHIFT + 1)

    #define _SUPPORT_LCD                                                 // include just once
    #if defined TEST_DISPLAY_TTY
    #define MAX_MESSAGES 10
    static CHAR message_queue[MAX_MESSAGES][LCD_CHARACTERS + 1];
    static int iQueueIn = 0;
    static int iQueueOut = 0;
    static CHAR cLine = '\x01';
    static int iLCD_state = 0;

    static void fnCheckLCD(void)
    {
        if (iLCD_state == 0) {
            return;
        }
        if (iQueueIn == iQueueOut) {                                     // nothing waiting
            return;
        }
        fnDoLCD_com_text(E_LCD_COMMAND_TEXT, (unsigned char *)message_queue[iQueueOut++], (LCD_CHARACTERS + 1));
        if (iQueueOut >= MAX_MESSAGES) {
            iQueueOut = 0;
        }
        iLCD_state = 0;                                                  // LCD busy
    }

    extern void fnDebugLcd(CHAR *message, QUEUE_TRANSFER nr_of_bytes)    // put messages into queue
    {
        message_queue[iQueueIn][0] = cLine;
        if (nr_of_bytes > LCD_CHARACTERS) {
            nr_of_bytes = LCD_CHARACTERS;
        }
        uMemcpy(&message_queue[iQueueIn][1], message, nr_of_bytes);
        uMemset(&message_queue[iQueueIn][nr_of_bytes + 1], ' ', (LCD_CHARACTERS - nr_of_bytes));
        iQueueIn++;
        if (iQueueIn >= MAX_MESSAGES) {
            iQueueIn = 0;
        }
        if (cLine == '\x01') {                                           // if first line, set second line, and reverse
            cLine = '\xc0';
        }
        else {
            cLine = '\x01';
        }
        fnCheckLCD();                                                    // send if not busy
    }
    #endif
#elif (defined SUPPORT_GLCD || defined SUPPORT_OLED) && (!defined GLCD_COLOR && !defined SUPPORT_TFT && !defined _SUPPORT_GLCD)
    #include "widgets.h"                                                 // widgets and images used by the project
    #define _SUPPORT_GLCD                                                // include just once
#endif


/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////
//                 Character LCD (see below for GLCD)                  //
/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////


// Application task message reception from the LCD task - character LCD
//
#if defined SUPPORT_LCD && defined HANDLE_LCD_MESSAGES
        case TASK_LCD:
            fnRead(PortIDInternal, ucInputMessage, ucInputMessage[MSG_CONTENT_LENGTH]); // read the complete message
            if (E_LCD_INITIALISED == ucInputMessage[0]) {
    #if defined USE_TIME_SERVER
                static const CHAR cWelcomeLCD[] = " Fetching Time";      // display a welcome message from present cursor position
    #else
                static const CHAR cWelcomeLCD[] = "Hi \xe4Tasker!";      // display a welcome message from present cursor position
    #endif
    #if defined TEST_DISPLAY_TTY
                fnDebugLcd("Test Line 1", 11);
                fnDebugLcd("Test Line 2", 11);
                iLCD_state = 1;
                fnCheckLCD();                                            // send if anything is waiting
    #else
                fnDoLCD_com_text(E_LCD_TEXT, (unsigned char *)cWelcomeLCD, (sizeof(cWelcomeLCD) - 1));
    #endif
            }
            else if (E_LCD_READY == ucInputMessage[0]) {
    #if defined TEST_DISPLAY_TTY
                iLCD_state = 1;
                fnCheckLCD();                                            // send if anything is waiting
    #else
                static const CHAR cSecondLineLCD[] = "\xc0****************"; // set second line
        #if defined USE_TIME_SERVER
              /*if (ucTimeServerTry != 0) {
                    break;
                }*/
                fnDoLCD_com_text(E_LCD_COMMAND_TEXT, (unsigned char *)cSecondLineLCD, (sizeof(cSecondLineLCD) - 1));
        #else
                static const CHAR cFillSecondLineLCD[] = "\xe3*****";    // hidden before second line
                if (cShiftTest == LCD_TEST_STARTING) {
                    fnDoLCD_com_text(E_LCD_COMMAND_TEXT, (unsigned char *)cSecondLineLCD, (sizeof(cSecondLineLCD) - 1));
                    uTaskerMonoTimer( OWN_TASK, (DELAY_LIMIT)(0.5 * SEC), E_SHIFT_DISPLAY );
                    cShiftTest++;
                }
                else if (cShiftTest == (LCD_TEST_STARTING + 1)) {
                    cShiftTest = 0;
                    fnDoLCD_com_text(E_LCD_COMMAND_TEXT, (unsigned char *)cFillSecondLineLCD, (sizeof(cFillSecondLineLCD) - 1));
                    uTaskerMonoTimer( OWN_TASK, (DELAY_LIMIT)(0.5 * SEC), E_SHIFT_DISPLAY );
                }
            #if defined LCD_READ_TEST
                else {                                                   // ack after successfully writing new text
                    static unsigned char ucAddress = 0x80;
                    fnDoLCD_com_text(E_LCD_READ_RAM, (unsigned char *)&ucAddress, sizeof(ucAddress)); // request the content of the first LCD location
                    if (++ucAddress > 0x90) {
                        ucAddress = 0x80;
                    }
                }
            #endif
        #endif
    #endif
            }
    #if defined LCD_READ_TEST
            else if (E_LCD_READ == ucInputMessage[0]) {                  // answer from read of LCD display content
                fnDebugMsg("LCD = ");
                fnDebugHex(ucInputMessage[1], (WITH_LEADIN | 1));
                fnDebugMsg(" = ");
                fnWrite(DebugHandle, &ucInputMessage[1], 1);             // display the read value
                fnDebugMsg("\r\n");
            }
    #endif
            break;
#endif


// Application timer events for the character LCD
//
#if defined SUPPORT_LCD && defined HANDLE_TIMERS && !defined USE_TIME_SERVER
        case E_SHIFT_DISPLAY:                                            // timer event to command content movement
            { 
                static const unsigned char ucShiftRightCommand = 0x1c;   // command to shift content one position to right
                static const unsigned char ucShiftLeftCommand  = 0x18;   // command to shift content one position to left

                if (++cShiftTest > LCD_TEST_MAX_SHIFT) {                 // if maximum shift in one direction has been reached, change the direction
                    cShiftTest = -(LCD_TEST_MAX_SHIFT - 1);              // move to shift left
                }
                if (cShiftTest > 0) {
                    fnDoLCD_com_text(E_LCD_COMMAND, (unsigned char *)&ucShiftRightCommand, sizeof(ucShiftRightCommand));
                }
                else {
                    fnDoLCD_com_text(E_LCD_COMMAND, (unsigned char *)&ucShiftLeftCommand, sizeof(ucShiftLeftCommand));
                }
                uTaskerMonoTimer(OWN_TASK, (DELAY_LIMIT)(0.5 * SEC), E_SHIFT_DISPLAY); // start a timer for the next movement
            }
            break;
#endif


// Message transmission routine used to send text and commands to the LCD task - character LCD
//
#if defined SUPPORT_LCD && defined LCD_MESSAGE_ROUTINES
    #if defined SUPPORT_DOUBLE_QUEUE_WRITES
// Demonstration of improved efficiency using double write support
//
extern void fnDoLCD_com_text(unsigned char ucType, unsigned char *ptrInput, unsigned char ucLength)
{
    unsigned char ucMessage[HEADER_LENGTH + 1];

    ucMessage[MSG_DESTINATION_NODE] = INTERNAL_ROUTE;                    // destination node
    ucMessage[MSG_SOURCE_NODE]      = INTERNAL_ROUTE;                    // own node
    ucMessage[MSG_DESTINATION_TASK] = TASK_LCD;                          // destination task
    ucMessage[MSG_SOURCE_TASK]      = OWN_TASK;                          // own task
    ucMessage[MSG_CONTENT_LENGTH]   = ucLength+1;                        // message length
    ucMessage[MSG_CONTENT_COMMAND]  = ucType;                            // command(s) or text for LCD

    if (fnWrite(INTERNAL_ROUTE, ucMessage, 0) != 0) {                    // send message header
        fnWrite(INTERNAL_ROUTE, ptrInput, ucLength);                     // add data
    }
}
    #else
extern void fnDoLCD_com_text(unsigned char ucType, unsigned char *ptrInput, unsigned char ucLength)
{
    #define MAX_LCD_MSG 50
    unsigned char ucMessage[HEADER_LENGTH + 1 + MAX_LCD_MSG];

    uMemcpy(&ucMessage[HEADER_LENGTH + 1], ptrInput, ucLength);

    ucMessage[MSG_DESTINATION_NODE] = INTERNAL_ROUTE;                    // destination node
    ucMessage[MSG_SOURCE_NODE]      = INTERNAL_ROUTE;                    // own node
    ucMessage[MSG_DESTINATION_TASK] = TASK_LCD;                          // destination task
    ucMessage[MSG_SOURCE_TASK]      = OWN_TASK;                          // own task
    ucMessage[MSG_CONTENT_LENGTH]   = ++ucLength;                        // message length
    ucMessage[MSG_CONTENT_COMMAND]  = ucType;                            // command(s) or text for LCD

    fnWrite(INTERNAL_ROUTE, ucMessage, (QUEUE_TRANSFER)(ucLength + HEADER_LENGTH)); // send message to defined task
}
    #endif
#endif








/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////
//                       Graphical mono-GLCD                           //
/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////





// Application task message reception from the LCD task - character mono-GLCD
//
#if defined HANDLE_LCD_MESSAGES && (defined SUPPORT_GLCD || defined SUPPORT_OLED) && !defined GLCD_COLOR // {1}
        case TASK_LCD:
            fnRead(PortIDInternal, ucInputMessage, ucInputMessage[MSG_CONTENT_LENGTH]); // read the complete message
            if (E_LCD_INITIALISED == ucInputMessage[0]) {                // the LCD initialisation has completed and use may start
    #if defined SUPPORT_GLCD
                GLCD_TEXT_POSITION text_pos;// = {PAINT_LIGHT, 2, 0, FONT_NINE_DOT};
        #if defined SUPPORT_TOUCH_SCREEN && defined USE_PARAMETER_BLOCK && !defined TOUCH_FT6206 // only calibrate when it is possible to save parameters
                if ((temp_pars->temp_parameters.sTouchXminCal == 0) && (temp_pars->temp_parameters.sTouchXmaxCal == 0) && (temp_pars->temp_parameters.sTouchYminCal == 0) && (temp_pars->temp_parameters.sTouchYmaxCal == 0)) { // all zeros is recognised as not caibrated
                    extern void fnStartTouchCalibration(void);
#if 0
                    GLCD_LINE line;
#endif
                    GLCD_RECT box;
                    fnStartTouchCalibration();
                    text_pos.ucMode = (PAINT_LIGHT);
                    text_pos.usX = 10;
                    text_pos.usY = 15;
                    text_pos.ucFont = FONT_NINE_DOT;
                    fnDoLCD_text(&text_pos, "TOUCH SCREEN");
                    text_pos.ucMode = (PAINT_LIGHT);
                    text_pos.usY += 12;
                    fnDoLCD_text(&text_pos, "CALIBRATION REQUIRED");
                    box.ucMode = (PAINT_LIGHT | REDRAW);
                    box.rect_corners.usX_start = (GLCD_X/CGLCD_PIXEL_SIZE - 2);
                    box.rect_corners.usX_end = (GLCD_X/CGLCD_PIXEL_SIZE - 1);
                    box.rect_corners.usY_start = (GLCD_Y/CGLCD_PIXEL_SIZE - 2);
                    box.rect_corners.usY_end = (GLCD_Y/CGLCD_PIXEL_SIZE - 1);
#if 0
                    box.rect_corners.usX_start = 0;
                    box.rect_corners.usX_end = 1;
                    box.rect_corners.usY_start = 0;
                    box.rect_corners.usY_end = 1;
#endif
                    fnDoLCD_rect(&box);
#if 0
                    box.ucMode = PAINT_LIGHT;
                    box.rect_corners.usX_start = (GLCD_X/CGLCD_PIXEL_SIZE/2 - 5);
                    box.rect_corners.usX_end = (GLCD_X/CGLCD_PIXEL_SIZE/2 + 5);
                    box.rect_corners.usY_start = (GLCD_Y/CGLCD_PIXEL_SIZE/2 - 5);
                    box.rect_corners.usY_end = (GLCD_Y/CGLCD_PIXEL_SIZE/2 + 5);
                    fnDoLCD_rect(&box);
                    box.ucMode = PAINT_DARK;
                    line.line_start_end.usX_start = box.rect_corners.usX_start;
                    line.line_start_end.usX_end = box.rect_corners.usX_end;
                    line.line_start_end.usY_start = box.rect_corners.usY_start;
                    line.line_start_end.usY_end = box.rect_corners.usY_end;
                    box.rect_corners.usX_start += 1;
                    box.rect_corners.usX_end -= 1;
                    box.rect_corners.usY_start += 1;
                    box.rect_corners.usY_end -= 1;
                    fnDoLCD_rect(&box);
                    line.ucMode = PAINT_LIGHT;
                    fnDoLCD_line(&line);
                    line.ucMode = (PAINT_LIGHT | REDRAW);
                    line.line_start_end.usY_start += 10;
                    line.line_start_end.usY_end -= 10;
                    fnDoLCD_line(&line);
#endif
                    break;
                }
        #endif
        #if defined TEMP_HUM_TEST
                GLCD_LINE line1;
                GLCD_STYLE graphic_style;
                graphic_style.ucMode = STYLE_PIXEL_COLOR;
                graphic_style.color = (COLORREF)RGB(192,192,192);
                fnDoLCD_style(&graphic_style);
                line1.ucMode = PAINT_LIGHT;
                line1.line_start_end.usX_start = 10;
                line1.line_start_end.usY_start = 48;
                line1.line_start_end.usX_end = 150;
                line1.line_start_end.usY_end = 48;
                fnDoLCD_line(&line1);
                line1.line_start_end.usY_start += 3;
                line1.line_start_end.usY_end += 3;

                fnDoLCD_line(&line1);
                text_pos.ucMode = (PAINT_LIGHT);
                text_pos.usX = 32;
                text_pos.usY = 5;
                text_pos.ucFont = FONT_TEN_DOT;
                fnDoLCD_text(&text_pos, "Temperature");
                text_pos.usY = 17;
                text_pos.usX = 68;
                fnDoLCD_text(&text_pos, "and");
                text_pos.usY = 30;
                text_pos.usX = 25;
                text_pos.ucMode = (PAINT_LIGHT | REDRAW);
                fnDoLCD_text(&text_pos, "Humidity Test");
                graphic_style.ucMode = STYLE_PIXEL_COLOR;
                graphic_style.color = (COLORREF)RGB(255,255,0);
                fnDoLCD_style(&graphic_style);
        #else
            #if (defined MB785_GLCD_MODE || defined AVR32_EVK1105 || defined IDM_L35_B || defined M52259_TOWER || defined TWR_K60N512 || defined TWR_K60D100M || defined TWR_K70F120M || defined OLIMEX_LPC1766_STK || defined OLIMEX_LPC2478_STK || defined K70F150M_12M) && defined SDCARD_SUPPORT // {5}
                ptr_utDirectory = utAllocateDirectory(DISK_D, 0);        // allocate a directory for use by this module associated with D: - no path string
                if (utOpenDirectory("pics", ptr_utDirectory) == UTFAT_SUCCESS)  { // open the root directory of disk D
			        ptr_utDirectory->usDirectoryFlags |= UTDIR_REFERENCED; // all accesses are referenced to the root directory
                    if (ptr_utDirectory->usDirectoryFlags & UTDIR_VALID) { // if the directory exists
                        uTaskerMonoTimer(OWN_TASK, (DELAY_LIMIT)(0.05 * SEC), E_NEXT_PHOTO); // start a timer which is used to display various images   
                        break;
                    }                                                    // else fall through to standard demo
                }
                SET_SPI_SD_INTERFACE_FULL_SPEED();                       // assume that the SD card is now valid so ensure that high speed is set for the LCD interface
            #endif
            #if defined GLCD_MENU_TEST
                fnDrawMenu(&Menu);
            #else
                text_pos.ucMode = PAINT_LIGHT;
                text_pos.usX = 2;
                text_pos.usY = 0;
                text_pos.ucFont = FONT_NINE_DOT;
                fnDoLCD_text(&text_pos, "Welcome to the");
                text_pos.usY = 15;
                fnDoLCD_text(&text_pos, "uTasker GLCD demo!");
                text_pos.usY = 40;
                text_pos.ucFont = (FONT_FIVE_DOT);
                text_pos.ucMode = (REDRAW | GIVE_ACK);                   // an ack is requested when this has been completely displayed
                fnDoLCD_text(&text_pos, "Starting...");
            #endif
        #endif
    #endif
            }
            else if (E_LCD_READY == ucInputMessage[0]) {                 // an acknowledgment from the display task (last job has been completed)
                uTaskerMonoTimer(OWN_TASK, (DELAY_LIMIT)(1.0 * SEC), E_NEXT_PIC); // start a timer which is used to display various images
            }
    #if defined SUPPORT_TOUCH_SCREEN && defined TOUCH_MOUSE_TASK
            else if (E_TOUCH_MOUSE_EVENT == ucInputMessage[0]) {
                TOUCH_MOUSE_EVENT_MESSAGE tmEvent;
                uMemcpy(&tmEvent, &ucInputMessage[1], sizeof(TOUCH_MOUSE_EVENT_MESSAGE));
                switch (tmEvent.ucEvent) {
                case TOUCH_MOUSE_RELEASE:
                    fnDebugMsg("Release detect: ");
                    fnDebugDec(tmEvent.usX, 0);
                    fnDebugMsg(", ");
                    fnDebugDec(tmEvent.usY, WITH_CR_LF);
                    break;
        #if defined SUPPORT_TOUCH_SCREEN && defined USE_PARAMETER_BLOCK
                case TOUCH_MOUSE_CALIBRATION:
                    fnDebugMsg("Touch Calibration: ");
                    fnDebugDec(tmEvent.usX, 0);
                    fnDebugMsg(", ");
                    fnDebugDec(tmEvent.usY, WITH_CR_LF);
                    break;
    #endif
                case TOUCH_MOUSE_PRESS:
                    fnDebugMsg("Touch detect: ");
                    fnDebugDec(tmEvent.usX, 0);
                    fnDebugMsg(", ");
                    fnDebugDec(tmEvent.usY, WITH_CR_LF);
                    break;
                case TOUCH_MOUSE_MOVE:
                    fnDebugMsg("M");
                    break;
                }
            }
    #endif
            break;
#endif


// Application timer events for the graphic mono-GLCD
//
#if defined HANDLE_TIMERS && defined SUPPORT_GLCD && !defined GLCD_COLOR && !defined SUPPORT_TFT // {1}
        case E_NEXT_PIC:                                                 // timer event
            {
                static int iCnt = 0;
                GLCD_PIC test_pic;// = {(REDRAW | GIVE_ACK), 0, 0, 0};
                test_pic.usX = 0;
                test_pic.usY = 0;
                test_pic.ucMode = (REDRAW | GIVE_ACK);
                switch (iCnt++) {                                        // cycle through some demonstration bitmap images
                case 0:                    
                    test_pic.ptrPic = (GBITMAP *)uTaskerPic1;
                    fnDoLCD_pic(&test_pic);                              // send the command to the GLCD task to draw the image
                    break;
                case 1:
                    test_pic.ptrPic = (GBITMAP *)uTaskerPic3;
                    fnDoLCD_pic(&test_pic);                              // send the command to the GLCD task to draw the image
                    break;
                case 2:
                    test_pic.ptrPic = (GBITMAP *)uTaskerPic2;
                    fnDoLCD_pic(&test_pic);                              // send the command to the GLCD task to draw the image
                    break;
                case 3:                                                  // {2}
                    {
                        #define ABOVE_LEFT_X   0
                        #define ABOVE_LEFT_Y   0
                        #define BOTTOM_RIGHT_X ((GLCD_X/CGLCD_PIXEL_SIZE) - 1)
                        #define BOTTOM_RIGHT_Y ((GLCD_Y/CGLCD_PIXEL_SIZE) - 1)
                        GLCD_LINE line1;// = {PAINT_LIGHT, {ABOVE_LEFT_X, ABOVE_LEFT_Y, BOTTOM_RIGHT_X, ABOVE_LEFT_Y}}; // first line from top left to top right
                        GLCD_LINE line2;// = {PAINT_LIGHT, {BOTTOM_RIGHT_X, ABOVE_LEFT_Y, BOTTOM_RIGHT_X, BOTTOM_RIGHT_Y}}; // second line from top right to bottom right
                        GLCD_LINE line3;// = {PAINT_LIGHT, {BOTTOM_RIGHT_X, BOTTOM_RIGHT_Y, ABOVE_LEFT_X, BOTTOM_RIGHT_Y}}; // third line from bottom right to bottom left
                        GLCD_LINE line4;// = {PAINT_LIGHT, {ABOVE_LEFT_X, BOTTOM_RIGHT_Y, ABOVE_LEFT_X, ABOVE_LEFT_Y}}; // fourth line from bottom left to top left
                        GLCD_LINE diagonal1;// = {PAINT_INVERTED, {ABOVE_LEFT_X, ABOVE_LEFT_Y, BOTTOM_RIGHT_X, BOTTOM_RIGHT_Y}}; // draw a cross in the middle
                        GLCD_LINE diagonal2;// = {(PAINT_INVERTED), {BOTTOM_RIGHT_X, ABOVE_LEFT_Y, ABOVE_LEFT_X, BOTTOM_RIGHT_Y}};
                        GLCD_RECT_BLINK rect1;// = {(BLINKING_OBJECT | PAINT_INVERTED), {42, 54, 103, 68}, (DELAY_LIMIT)(0.10 * SEC)}; // draw a blinking rectangle
                        GLCD_RECT_BLINK rect2;// = {(BLINKING_OBJECT | PAINT_INVERTED), {82, 9, 100, 42},  (DELAY_LIMIT)(0.20 * SEC)}; // draw a blinking rectangle
                        GLCD_LINE_BLINK line5;//   = {(BLINKING_OBJECT | PAINT_INVERTED | REDRAW | GIVE_ACK), {8, 54, 41, 68},   (DELAY_LIMIT)(0.35 * SEC)}; // draw a blinking line
                        rect1.ucMode = (BLINKING_OBJECT | PAINT_INVERTED);
                        rect1.rect_corners.usX_start = 42;
                        rect1.rect_corners.usY_start = 54;
                        rect1.rect_corners.usX_end = 103;
                        rect1.rect_corners.usY_end = 68;
                        rect1.blink_half_period = (DELAY_LIMIT)(0.10 * SEC);
                        fnDoLCD_rect(&rect1);
                        rect2.ucMode = (BLINKING_OBJECT | PAINT_INVERTED);
                        rect2.rect_corners.usX_start = 82;
                        rect2.rect_corners.usY_start = 9;
                        rect2.rect_corners.usX_end = 100;
                        rect2.rect_corners.usY_end = 42;
                        rect2.blink_half_period = (DELAY_LIMIT)(0.20 * SEC);
                        fnDoLCD_rect(&rect2);
                        line1.ucMode = PAINT_LIGHT;
                        line1.line_start_end.usX_start = ABOVE_LEFT_X;
                        line1.line_start_end.usY_start = ABOVE_LEFT_Y;
                        line1.line_start_end.usX_end = BOTTOM_RIGHT_X;
                        line1.line_start_end.usY_end = ABOVE_LEFT_Y;
                        fnDoLCD_line(&line1);
                        line2.ucMode = PAINT_LIGHT;
                        line2.line_start_end.usX_start = BOTTOM_RIGHT_X;
                        line2.line_start_end.usY_start = ABOVE_LEFT_Y;
                        line2.line_start_end.usX_end = BOTTOM_RIGHT_X;
                        line2.line_start_end.usY_end = BOTTOM_RIGHT_Y;
                        fnDoLCD_line(&line2);
                        line3.ucMode = PAINT_LIGHT;
                        line3.line_start_end.usX_start = BOTTOM_RIGHT_X;
                        line3.line_start_end.usY_start = BOTTOM_RIGHT_Y;
                        line3.line_start_end.usX_end = ABOVE_LEFT_X;
                        line3.line_start_end.usY_end = BOTTOM_RIGHT_Y;
                        fnDoLCD_line(&line3);
                        line4.ucMode = PAINT_LIGHT;
                        line4.line_start_end.usX_start = ABOVE_LEFT_X;
                        line4.line_start_end.usY_start = BOTTOM_RIGHT_Y;
                        line4.line_start_end.usX_end = ABOVE_LEFT_X;
                        line4.line_start_end.usY_end = ABOVE_LEFT_Y;
                        fnDoLCD_line(&line4);
                        diagonal1.ucMode = PAINT_INVERTED;
                        diagonal1.line_start_end.usX_start = ABOVE_LEFT_X;
                        diagonal1.line_start_end.usY_start = ABOVE_LEFT_Y;
                        diagonal1.line_start_end.usX_end = BOTTOM_RIGHT_X;
                        diagonal1.line_start_end.usY_end = BOTTOM_RIGHT_Y;
                        fnDoLCD_line(&diagonal1);
                        diagonal2.ucMode = PAINT_INVERTED;
                        diagonal2.line_start_end.usX_start = BOTTOM_RIGHT_X;
                        diagonal2.line_start_end.usY_start = ABOVE_LEFT_Y;
                        diagonal2.line_start_end.usX_end = ABOVE_LEFT_X;
                        diagonal2.line_start_end.usY_end = BOTTOM_RIGHT_Y;
                        fnDoLCD_line(&diagonal2);
                        line5.ucMode = (BLINKING_OBJECT | PAINT_INVERTED | REDRAW | GIVE_ACK);
                        line5.line_start_end.usX_start = 8;
                        line5.line_start_end.usY_start = 54;
                        line5.line_start_end.usX_end = 41;
                        line5.line_start_end.usY_end = 68;
                        line5.blink_half_period = (DELAY_LIMIT)(0.35 * SEC);
                        fnDoLCD_line(&line5);
                    }
                    break;
                case 4:                                                  // {2}
                    {
                        GLCD_LINE diagonal1;// = {PAINT_INVERTED, {ABOVE_LEFT_X, ABOVE_LEFT_Y, BOTTOM_RIGHT_X, BOTTOM_RIGHT_Y}}; // draw a cross in the middle
                        GLCD_LINE diagonal2;// = {(PAINT_INVERTED | REDRAW | GIVE_ACK), {BOTTOM_RIGHT_X, ABOVE_LEFT_Y, ABOVE_LEFT_X, BOTTOM_RIGHT_Y}};
                        GLCD_RECT_BLINK rect1;// = {(BLINKING_OBJECT | PAINT_LIGHT | REDRAW), {42, 54, 103, 68}, 0}; // cancel a blinking rectangle and paint it dark
                        GLCD_RECT_BLINK rect2;// = {(BLINKING_OBJECT | PAINT_DARK | REDRAW), {44, 56, 101, 66}, (DELAY_LIMIT)(1 * SEC)}; // restart a blinking rect and paint it dark
                        diagonal1.ucMode = PAINT_INVERTED;
                        diagonal1.line_start_end.usX_start = ABOVE_LEFT_X;
                        diagonal1.line_start_end.usY_start = ABOVE_LEFT_Y;
                        diagonal1.line_start_end.usX_end = BOTTOM_RIGHT_X;
                        diagonal1.line_start_end.usY_end = BOTTOM_RIGHT_Y;
                        fnDoLCD_line(&diagonal1);
                        diagonal2.ucMode = (PAINT_INVERTED | REDRAW | GIVE_ACK);
                        diagonal2.line_start_end.usX_start = BOTTOM_RIGHT_X;
                        diagonal2.line_start_end.usY_start = ABOVE_LEFT_Y;
                        diagonal2.line_start_end.usX_end = ABOVE_LEFT_X;
                        diagonal2.line_start_end.usY_end = BOTTOM_RIGHT_Y;
                        fnDoLCD_line(&diagonal2);
                        rect1.ucMode = (BLINKING_OBJECT | PAINT_LIGHT | REDRAW); // ensure object painted light
                        rect1.rect_corners.usX_start = 42;
                        rect1.rect_corners.usY_start = 54;
                        rect1.rect_corners.usX_end = 103;
                        rect1.rect_corners.usY_end = 68;
                        rect1.blink_half_period = 0;                     // stop this object blinking
                        fnDoLCD_rect(&rect1);
                        rect2.ucMode = (BLINKING_OBJECT | PAINT_DARK | REDRAW);
                        rect2.rect_corners.usX_start = 44;
                        rect2.rect_corners.usY_start = 56;
                        rect2.rect_corners.usX_end = 101;
                        rect2.rect_corners.usY_end = 66;
                        rect2.blink_half_period = (DELAY_LIMIT)(1 * SEC);
                        fnDoLCD_rect(&rect2);
                    }
                    break;
                case 5:                                                  // {2}
                    {
                        static GLCD_RECT rect = {(PAINT_INVERTED | REDRAW | GIVE_ACK), {12, 8, 12, 8}}; // draw a rect
                        fnDoLCD_rect(&rect);
                        if (++rect.rect_corners.usX_end < 60) {
                            rect.rect_corners.usY_end++;
                            iCnt = 5;
                        }
                        else {
                            rect.rect_corners.usX_end = 12;
                            rect.rect_corners.usY_end = 8;
                        }
                    }
                    break;
                case 6:                                                  // {4}
                case 7:
                    {
                        GLCD_SCROLL scroll;// = {(REDRAW | GIVE_ACK | KILL_BLINK_OBJECTS), 0, 9};
                        GLCD_TEXT_POSITION text_pos;// = {PAINT_LIGHT, X, Y, FONT_SIX_DOT};
                        scroll.ucMode = (REDRAW | GIVE_ACK | KILL_BLINK_OBJECTS);
                        scroll.sX_scroll = 0;
                        scroll.sY_scroll = 9;
                        fnDoLCD_scroll(&scroll);                        
                        text_pos.ucMode = (PAINT_LIGHT | REDRAW | GIVE_ACK);
                        text_pos.usX = 0;
                        text_pos.usY = ((GLCD_Y/CGLCD_PIXEL_SIZE) - 8);  // position on very bottom of display
                        text_pos.ucFont = (FONT_SIX_DOT);
                        if (iCnt == 6) {
                            fnDoLCD_text(&text_pos, "Test of scrolling up");
                        }
                        else {
                          //static CHAR cTest[] = "abcdefghijklmnop"; declare as array to avoid newer versions of CodeWarrior from putting the string in flash
                            static CHAR cTest[] = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 0}; // {6}

                            int i = 0;
                            fnDoLCD_text(&text_pos, cTest);
                            while (i < 16) {
                                if (cTest[i] == 'z') {
                                    cTest[i] = 'a';
                                }
                                else {
                                    cTest[i]++;
                                }
                                i++;
                            }
                        }
                        iCnt = 6;
                    }
                    break;
                }
            }
            break;
    #if (defined MB785_GLCD_MODE || defined AVR32_EVK1105 || defined IDM_L35_B || defined M52259_TOWER || defined TWR_K60N512 || defined TWR_K60D100M || defined TWR_K70F120M || defined OLIMEX_LPC1766_STK || defined OLIMEX_LPC2478_STK || defined K70F150M_12M) && defined SDCARD_SUPPORT // {5}
        case E_NEXT_PHOTO:
            {
                static UTLISTDIRECTORY utListDirectory;                  // list directory object for a single user               
                if (utListDirectory.ptr_utDirObject == 0) {
                    utListDirectory.ptr_utDirObject = ptr_utDirectory;
                    utLocateDirectory(0, &utListDirectory);              // open a list referenced to the main directory
                }
                if (utReadDirectory(&utListDirectory, &utFileInfo) == UTFAT_END_OF_DIRECTORY) {
                    utListDirectory.ptr_utDirObject = 0;                 // end of the directory content
                    uTaskerMonoTimer(OWN_TASK, (DELAY_LIMIT)(0), E_NEXT_PHOTO);
                    break;
                }
                if (uMatchFileExtension(&utFileInfo, "BMP") == 0) {      // if BMP assume content is correct and display the new image
                    fnDisplayPhoto(1);
                }
                uTaskerMonoTimer(OWN_TASK, (DELAY_LIMIT)(5.0 * SEC), E_NEXT_PHOTO); // start a timer which is used to display various images   
            }
            break;
    #endif
#endif


// Message transmission routine used to send text and images to the LCD task - mono-GLCD
//
#if defined LCD_MESSAGE_ROUTINES && (defined SUPPORT_OLED)  && !defined GLCD_COLOR && !defined SUPPORT_TFT // {1}{3}

// Text message causing the GLCD to display the text at the defined position, using the defined font
//
extern void fnDoLCD_text(GLCD_TEXT_POSITION *text_pos, const CHAR *cText)
{
    unsigned char ucMessage[ HEADER_LENGTH + 1 + sizeof(GLCD_TEXT_MESSAGE)];
    unsigned char ucLength;

    ucMessage[MSG_DESTINATION_NODE] = INTERNAL_ROUTE;                    // destination node
    ucMessage[MSG_SOURCE_NODE]      = INTERNAL_ROUTE;                    // own node
    ucMessage[MSG_DESTINATION_TASK] = TASK_LCD;                          // destination task
    ucMessage[MSG_SOURCE_TASK]      = OWN_TASK;                          // own task
    ucMessage[MSG_CONTENT_COMMAND]  = E_LCD_TEXT;                        // message type
    uMemcpy(&ucMessage[ MSG_CONTENT_COMMAND + 1], text_pos, sizeof(GLCD_TEXT_POSITION));
    ucLength = (unsigned char)((unsigned char *)uStrcpy((CHAR *)&ucMessage[ MSG_CONTENT_COMMAND + 1 + sizeof(GLCD_TEXT_POSITION)], cText) - &ucMessage[ MSG_CONTENT_COMMAND + sizeof(GLCD_TEXT_POSITION)]);
    ucLength += sizeof(GLCD_TEXT_POSITION);
    ucMessage[ MSG_CONTENT_LENGTH ]   = ucLength;                        // message content length
    fnWrite( INTERNAL_ROUTE, ucMessage, (QUEUE_TRANSFER)(ucLength + HEADER_LENGTH));// send message to defined task
}


// Image message causing the GLCD to display the image (pointer to image used) at the defined position
//
extern void fnDoLCD_pic(GLCD_PIC *pic)
{
    unsigned char ucMessage[ HEADER_LENGTH + 1 + sizeof(GLCD_PIC)];

    ucMessage[MSG_DESTINATION_NODE] = INTERNAL_ROUTE;                    // destination node
    ucMessage[MSG_SOURCE_NODE]      = INTERNAL_ROUTE;                    // own node
    ucMessage[MSG_DESTINATION_TASK] = TASK_LCD;                          // destination task
    ucMessage[MSG_SOURCE_TASK]      = OWN_TASK;                          // own task
    ucMessage[MSG_CONTENT_LENGTH]   = (1 + sizeof(GLCD_PIC));            // message content length
    ucMessage[MSG_CONTENT_COMMAND]  = E_LCD_PIC;                         // message type
    uMemcpy(&ucMessage[MSG_CONTENT_COMMAND + 1], pic, sizeof(GLCD_PIC));
    fnWrite(INTERNAL_ROUTE, ucMessage, (QUEUE_TRANSFER)(HEADER_LENGTH + 1 + sizeof(GLCD_PIC))); // send message to defined task
}

// Message causing the GLCD to draw a line                               {2}
//
extern void fnDoLCD_line(void *line)
{
    int iLength = (sizeof(GLCD_LINE) + 1);
    unsigned char ucMessage[ HEADER_LENGTH + 1 + sizeof(GLCD_LINE_BLINK)];

    ucMessage[MSG_DESTINATION_NODE] = INTERNAL_ROUTE;                    // destination node
    ucMessage[MSG_SOURCE_NODE]      = INTERNAL_ROUTE;                    // own node
    ucMessage[MSG_DESTINATION_TASK] = TASK_LCD;                          // destination task
    ucMessage[MSG_SOURCE_TASK]      = OWN_TASK;                          // own task
    ucMessage[MSG_CONTENT_COMMAND]  = E_LCD_LINE;                        // message type
    if (((GLCD_LINE *)line)->ucMode & (BLINKING_OBJECT)) {
        iLength += (sizeof(GLCD_LINE_BLINK) - sizeof(GLCD_LINE));        // extended command
    }
    ucMessage[MSG_CONTENT_LENGTH]   = iLength;                           // message content length
    uMemcpy(&ucMessage[ MSG_CONTENT_COMMAND + 1], line, (iLength - 1));
    fnWrite(INTERNAL_ROUTE, ucMessage, (QUEUE_TRANSFER)(HEADER_LENGTH + iLength)); // send message to defined task
}

// Message causing the GLCD to draw a filled rect                      {2}
//
extern void fnDoLCD_rect(void *rect)
{
    int iLength = (sizeof(GLCD_RECT) + 1);
    unsigned char ucMessage[ HEADER_LENGTH + 1 + sizeof(GLCD_RECT_BLINK)];

    ucMessage[ MSG_DESTINATION_NODE ] = INTERNAL_ROUTE;                  // destination node
    ucMessage[ MSG_SOURCE_NODE ]      = INTERNAL_ROUTE;                  // own node
    ucMessage[ MSG_DESTINATION_TASK ] = TASK_LCD;                        // destination task
    ucMessage[ MSG_SOURCE_TASK ]      = OWN_TASK;                        // own task
    ucMessage[ MSG_CONTENT_COMMAND ]  = E_LCD_rect;                      // message type
    if (((GLCD_RECT *)rect)->ucMode & (BLINKING_OBJECT)) {               // extended command
        iLength += (sizeof(GLCD_RECT_BLINK) - sizeof(GLCD_RECT));
    }
    ucMessage[ MSG_CONTENT_LENGTH ]   = (unsigned char)iLength;          // message content length
    uMemcpy(&ucMessage[ MSG_CONTENT_COMMAND + 1], rect, (iLength - 1));
    fnWrite( INTERNAL_ROUTE, ucMessage, (QUEUE_TRANSFER)(HEADER_LENGTH + iLength));// send message to defined task
}

// Message causing the display to scroll
//
extern void fnDoLCD_scroll(GLCD_SCROLL *scroll)
{
    unsigned char ucMessage[ HEADER_LENGTH + 1 + sizeof(GLCD_SCROLL)];

    ucMessage[ MSG_DESTINATION_NODE ] = INTERNAL_ROUTE;                  // destination node
    ucMessage[ MSG_SOURCE_NODE ]      = INTERNAL_ROUTE;                  // own node
    ucMessage[ MSG_DESTINATION_TASK ] = TASK_LCD;                        // destination task
    ucMessage[ MSG_SOURCE_TASK ]      = LCD_PARTNER_TASK;                // partner task
    ucMessage[ MSG_CONTENT_COMMAND ]  = E_LCD_SCROLL;                    // message type
    ucMessage[ MSG_CONTENT_LENGTH ]   = (sizeof(GLCD_SCROLL) + 1);       // message content length
    uMemcpy(&ucMessage[ MSG_CONTENT_COMMAND + 1], scroll, sizeof(GLCD_SCROLL));
    fnWrite( INTERNAL_ROUTE, ucMessage, (QUEUE_TRANSFER)(HEADER_LENGTH + (sizeof(GLCD_SCROLL) + 1)));// send message to defined task
}
#endif
