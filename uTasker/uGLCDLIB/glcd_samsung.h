/***********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher

    ---------------------------------------------------------------------
    File:      glcd_samsung.h
    Project:   uTasker project
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2016
    *********************************************************************
    25.02.2011 Correct polling of busy bit in multiple devices           {1}
       
*/


#if defined _GLCD_SAMSUNG && !defined OLED_GLCD_MODE && !defined TFT_GLCD_MODE && !defined CGLCD_GLCD_MODE && !defined KITRONIX_GLCD_MODE && !defined MB785_GLCD_MODE && !defined TFT2N0369_GLCD_MODE
    #if !defined _GLCD_SAMSUNG_DEFINES
        #define GLCD_BUSY()                 (fnReadGLCD_cmd() & 0x80)    // LCD busy check
        #define X_BYTES	                    (GLCD_X)                     // the number of bytes holding the X-pixels
        #define Y_BYTES	                    (GLCD_Y/8)                   // the number of rows of X_BYTES holding the Y rows
        #define DISPLAY_LEFT_PIXEL          0  
        #define DISPLAY_TOP_PIXEL           0
        #define DISPLAY_RIGHT_PIXEL         (GLCD_X - 1)
        #define DISPLAY_BOTTOM_PIXEL        (GLCD_Y - 1)
        #define UPDATE_WIDTH                X_BYTES
        #define UPDATE_HEIGHT               ((Y_BYTES + 7)/8)

        #define GLCD_CHIP0                  0x00
        #define GLCD_CHIP1                  0x01

        #define SINGLE_CHIP_WIDTH           64

        // LCD commands - Samsung
        //
        #define GLCD_ON                     0x3f
        #define GLCD_OFF                    0x3e

        #define GLCD_DISP_START             0xc0                         // 64 Lines per Chip can be 0xC0..0xFF
        #define GLCD_SET_PAGE               0xb8                         // Y-Dir 64 Lines divided into 8 Pages
                                                                         // Pages 0..7 -> CMD 0xB8..0xBF
        #define GLCD_SET_ADD                0x40                         // Set Column Addr 0..63 -> 0x40..0x7F

        // Status read (RS = 0)
        //
        #define LCD_BUSY                    0x80
        #define ADC_NORMAL                  0x40
        #define LCD_OFF_STATE               0x20
        #define LCD_RESET_IN_PROGRESS       0x10

        static void fnWriteGLCD_cmd (unsigned char ucByte, unsigned char ucChip);
        static void fnWriteGLCD_data(unsigned char ucByte, unsigned char ucChip);
        static unsigned char fnReadGLCD_cmd(void);    

        #define _GLCD_SAMSUNG_DEFINES                                    // include only once
    #endif

    #ifdef _GLCD_COMMANDS                                                // link in Samsung specific interface commands

// Read a byte from the GLCD interface
// It is assumed that RD/WR or RWn lines are initially high and that the data
// bus is being driven.The data bus is driven again on exit and RD/WR lines
// set high again.
//
static unsigned char fnReadGLCD_cmd(void)
{
        #if SAMSUNG_CHIPS > 1                                            // if the display has multiple controller, check that all are ready (2 halves supported)
    CHAR cChip = GLCD_CHIP1;
        #endif
    unsigned char ucByte = 0;
        #if SAMSUNG_CHIPS > 1
    while (cChip >= 0) {                                                 // {1} two controllers are polled and the result is ored (the read is intended to monitor the busy bits)
        if (cChip-- == GLCD_CHIP0) {                                     // assert selected chip select
            GLCD_CS0_H();                                                // CS0 High (selected)
            GLCD_CS1_L();                                                // CS1 Low
        } 
        else {
            GLCD_CS1_H();                                                // CS1 High (selected)
            GLCD_CS0_L();                                                // CS0 Low
        }
        #else
        GLCD_CS0_H();                                                    // CS0 High (selected)
        #endif

        GLCD_RS_L();                                                     // set C/D line low for command
        GLCD_DATAASINPUT();                                              // set the dbus high impedent
        GLCD_RW_H();                                                     // set the RD line high

        GLCD_ENA_L();                                                    // enable low
        GLCD_ENA_H();                                                    // enable Low-To-High transition
        GLCD_DELAY_READ();                                               // wait tACC so that the GLCD has driven the data to the bus
        ucByte |= (unsigned char)GLCD_DATAIN();                          // read data from Port
        GLCD_ENA_L();                                                    // enable Low
        #if SAMSUNG_CHIPS > 1
    }
        #endif
    GLCD_DATAASOUTPUT();                                                 // drive the data bus again
        #if defined _WINDOWS
    return ((unsigned char)ReadDisplay(0));                              // simulate the read
        #else
    return (ucByte);                                                     // return the value that was read
        #endif
}

// Generate enable signal for GLCD Chips
//
static void fnGLCD_Enable(void)
{                                                                        // data setup time BEFORE falling edge of Enable is 200 ns minimum
                                                                         // data hold time AFTER falling edge is 10 ns minimum
    GLCD_ENA_L();                                                        // enable Low
    GLCD_ENA_H();                                                        // enable Low-To-High transition
    GLCD_ENA_L();                                                        // enable High-to-Low transition
}

// Write a command to display
//
static void fnWriteGLCD_cmd(unsigned char ucByte, unsigned char ucChip) 
{
        #if SAMSUNG_CHIPS > 1
    if (ucChip == GLCD_CHIP0) {                                          // assert selected chip select
        GLCD_CS0_H();                                                    // CS0 High - selected
        GLCD_CS1_L();                                                    // CS1 Low
    } 
    else {
        GLCD_CS1_H();                                                    // CS1 High - selected
        GLCD_CS0_L();                                                    // CS0 Low
    }
        #else
    GLCD_CS0_H();                                                        // CS0 High - selected
        #endif
    GLCD_RS_L();                                                         // set C/D line low for command
    GLCD_RW_L();                                                         // set the WR line low
    GLCD_DATAOUT(ucByte);                                                // drive the data value on the bus
    GLCD_DELAY_WRITE();                                                  // ensure tDS data setup time!
    fnGLCD_Enable();                                                     // write enable
    GLCD_RW_H();                                                         // set WR high again
        #if defined _WINDOWS
    CollectCommand(1, ((ucChip << 8) | ucByte));// simulate the cmd write
        #endif
}

// Write a single byte of data to display
//
static void fnWriteGLCD_data(unsigned char ucByte, unsigned char ucChip)
{
        #if SAMSUNG_CHIPS > 1
    if (ucChip == GLCD_CHIP0) {                                          // assert selected chip select
        GLCD_CS0_H();                                                    // CS0 High
        GLCD_CS1_L();                                                    // CS1 Low
    } 
    else {
        GLCD_CS1_H();                                                    // CS1 High
        GLCD_CS0_L();                                                    // CS0 Low
    }
        #else
    GLCD_CS0_H();                                                        // CS0 High
        #endif
    GLCD_RS_H();                                                         // set C/D line high for data
    GLCD_RW_L();                                                         // set the WR line low
    GLCD_DATAOUT(ucByte);                                                // set the data value on the bus
    GLCD_DELAY_WRITE();                                                  // ensure tDS data setup time!
    fnGLCD_Enable();                                                     // write Enable
    GLCD_RW_H();                                                         // set WR High Again
        #if defined _WINDOWS
    CollectCommand(0, ((ucChip << 8) | ucByte));                         // simulate the write
        #endif
}
    #endif

#endif
