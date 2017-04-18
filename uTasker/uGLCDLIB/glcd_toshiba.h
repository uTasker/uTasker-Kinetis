/***********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher

    ---------------------------------------------------------------------
    File:      glcd_toshiba.h
    Project:   uTasker project
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2016
    *********************************************************************
    12.08.2013 Add ST7565S_GLCD_MODE                                     {1}
       
*/


#if !defined _GLCD_SAMSUNG && !defined OLED_GLCD_MODE && !defined TFT_GLCD_MODE && !defined NOKIA_GLCD_MODE & !defined CGLCD_GLCD_MODE && !defined KITRONIX_GLCD_MODE && !defined MB785_GLCD_MODE && !defined TFT2N0369_GLCD_MODE && !defined ST7565S_GLCD_MODE // {1}
    #if !defined _GLCD_TOSHIBA_DEFINES
        #define GLCD_BUSY()                 ((fnReadGLCD_cmd() & 0x3) != 0x3) // LCD busy check
        #define X_BYTES	                    (GLCD_X/8)                   // the number of bytes holding the X-pixels
        #define Y_BYTES	                    GLCD_Y                       // the number of rows of X_BYTES holding the Y rows
        #define DISPLAY_LEFT_PIXEL          0  
        #define DISPLAY_TOP_PIXEL           0
        #define DISPLAY_RIGHT_PIXEL         (GLCD_X - 1)
        #define DISPLAY_BOTTOM_PIXEL        (GLCD_Y - 1)
        #define UPDATE_WIDTH                ((X_BYTES + 7)/8)
        #define UPDATE_HEIGHT               Y_BYTES

        // GLCD commands - Toshiba
        //
        #define SET_ADDRESS_POINTER         0x24
        #define SET_TEXT_HOME_ADDRESS       0x40
        #define SET_TEXT_HORIZ_BYTES        0x41
        #define SET_GRAPHICS_HOME_ADDRESS   0x42
        #define SET_GRAPHICS_HORIZ_BYTES    0x43
        #define GRAPHIC_MODE_XOR            0x81
        #define DISABLE_TEXT_DISABLE_CURSOR 0x98
        #define WRITE_DATA_INC              0xc0

        // Local prototypes
        //
        static void fnCommandGlcd_1(unsigned char ucCommand, unsigned char  ucParameter);
        static void fnCommandGlcd_2(unsigned char ucCommand, unsigned short usParameter);
        static void fnWriteGLCD_cmd(unsigned char ucByte);
        static void fnWriteGLCD_data(unsigned char ucByte);
        static unsigned char fnReadGLCD_cmd(void);

        #define _GLCD_TOSHIBA_DEFINES                                    // include only once
    #endif

    #ifdef _GLCD_COMMANDS                                                // link in Toshiba specific interface commands


// Read data from the GLCD. It is assumed that RD/WR lines are initially high and that the data bus is being driven
// The data bus is driven again on exit and RD/WR lines set high again
//
static unsigned char fnReadGLCD_cmd(void)
{
    unsigned char ucByte;
    GLCD_CD_H();                                                         // set C/D line high for command
    GLCD_DATAASINPUT();                                                  // set the data bus high impedent
    GLCD_RD_L();                                                         // drive the read line low
    GLCD_DELAY_READ();                                                   // wait tACC so that the GLCD can drive the value on to the data bus
    ucByte = (unsigned char)GLCD_DATAIN();                               // read the value from the bus
    GLCD_RD_H();                                                         // set the read line high again
    GLCD_DATAASOUTPUT();                                                 // drive the data bus again
#if defined _WINDOWS
	return ((unsigned char)ReadDisplay(0));                              // simulate the read
#else
	return (ucByte);                                                     // return the value that was read
#endif
}

// Write an 8 bit data byte. It is assumed that the RD/WR lines are initially high and return then high after completion 
//
static void fnWriteGLCD_data(unsigned char ucByte)
{
    GLCD_CD_L();                                                         // set C/D line low for data
    GLCD_WR_L();                                                         // set the WR line low
    GLCD_DATAOUT(ucByte);                                                // set the data value on the bus
    GLCD_DELAY_WRITE();                                                  // ensure tDS delay before removing the write line
    GLCD_WR_H();
#if defined _WINDOWS
	CollectCommand(0, ucByte);                                           // simulate the data write
#endif
}

// Write an 8 bit command byte. It is assumed that the RD/WR lines are initially high and return them high after completion.
// The data bus is considered to be always driven.
//
static void fnWriteGLCD_cmd(unsigned char ucByte)
{
    GLCD_CD_H();                                                         // set C/D line high for command
    GLCD_WR_L();                                                         // set the WR line low
    GLCD_DATAOUT(ucByte);                                                // set the data value on the bus
    GLCD_DELAY_WRITE();                                                  // ensure tDS delay before removing the write line
    GLCD_WR_H();
#if defined _WINDOWS
    CollectCommand(1, ucByte);                                           // simulate the write
#endif
}

// This routine is used to write two parameter data bytes plus a command byte.
// The GLCD may be busy for some time after certain commands
//
static void fnCommandGlcd_2(unsigned char ucCommand, unsigned short usParameter)
{
    while (GLCD_BUSY()) {}                                               // wait until the GLCD can accept further writes
    fnWriteGLCD_data((unsigned char)usParameter);                        // write low data byte
    while (GLCD_BUSY()) {}                                               // wait until the GLCD can accept further writes
    fnWriteGLCD_data((unsigned char)(usParameter >> 8));                 // write high data byte
    while (GLCD_BUSY()) {}                                               // wait until the GLCD can accept further writes
    fnWriteGLCD_cmd(ucCommand);                                          // write the command
}

// This routine is used to write one parameter data bytes plus a command byte.
// The GLCD may be busy for some time after certain commands.
//
static void fnCommandGlcd_1(unsigned char ucCommand, unsigned char ucParameter)
{
    while (GLCD_BUSY()) {}                                               // wait until the GLCD can accept further writes
    fnWriteGLCD_data(ucParameter);                                       // write parameter byte
    while (GLCD_BUSY()) {}                                               // wait until the GLCD can accept further writes
    fnWriteGLCD_cmd(ucCommand);                                          // write the command
}
    #endif

#endif
