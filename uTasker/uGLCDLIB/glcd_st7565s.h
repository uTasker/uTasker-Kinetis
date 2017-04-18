/**********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher

    ---------------------------------------------------------------------
    File:      glcd_st7565.h
    Project:   uTasker project
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2016
    *********************************************************************
    With thanks to ELZET80 for development
    *********************************************************************
       
*/

#if defined ST7565S_GLCD_MODE && !defined SAMSUNG_GLCD_MODE && !defined OLED_GLCD_MODE && !defined TFT_GLCD_MODE && !defined CGLCD_GLCD_MODE && !defined KITRONIX_GLCD_MODE && !defined MB785_GLCD_MODE && !defined TFT2N0369_GLCD_MODE
    #if !defined _GLCD_ST7565S_DEFINES
        #if defined SPI_LCD                                              // SPI interface without read support
            #define GLCD_BUSY()                  0
        #else 
            #define GLCD_BUSY()                  (fnReadGLCD_cmd() & 0x80) // LCD busy check
        #endif
        #define X_BYTES                          (GLCD_X)                // the number of bytes holding the x pixels
        #define Y_BYTES                          (GLCD_Y/8)              // the number of rows of X_BYTES holding the y rows
        #define DISPLAY_LEFT_PIXEL               0  
        #define DISPLAY_TOP_PIXEL                0
        #define DISPLAY_RIGHT_PIXEL              (GLCD_X - 1)
        #define DISPLAY_BOTTOM_PIXEL             (GLCD_Y - 1)
        #define UPDATE_WIDTH                     X_BYTES
        #define UPDATE_HEIGHT                    ((Y_BYTES + 7)/8)

        // LCD commands - ST7565S
        //
        #define GLCD_RESET                       0xe2
        #define GLCD_ON                          0xaf
        #define GLCD_OFF                         0xae
        #define GLCD_HORZ_INV                    0xa1                    // invert horizontal
        #define GLCD_VERT_INV                    0xc8                    // invert vertical      
        #define GLCD_POWER_ON                    0x2f                    // voltage regulator and booster on  
        #define GLCD_DEFAULT_CONTRAST            0x26                    // default contrast  
        #define GLCD_DISP_START                  0x40                    // 64 lines can be 0x40..0x7f
        #define GLCD_SET_PAGE                    0xb0                    // y direction 64 lines divided into 8 pages, pages 0..7 -> command 0xb0..0xb7
        #define GLCD_SET_ADDH                    0x10                    // set column address high nibble  0x10...0x17
        #define GLCD_SET_ADDL                    0x00                    // set column address low nibble   0x00...0x0f

        // Status read (RS = 0)
        //
        #define LCD_BUSY                         0x80
        #define ADC_NORMAL                       0x40
        #define LCD_OFF_STATE                    0x20
        #define LCD_RESET_IN_PROGRESS            0x10

        #if defined _WINDOWS
            #define fnWriteGLCD_cmd(command)     CollectCommand(1, command); // simulate the cmd write
            #define fnReadGLCD_cmd()             0
            #define fnWriteGLCD_data(data)       CollectCommand(0, data); // simulate the data write
        #elif defined SPI_LCD
            #define fnWriteGLCD_cmd(command)     WRITE_SPI_LCD_CMD(command) // send command via SPI
            #define fnWriteGLCD_data(data)       WRITE_SPI_LCD_DATA(data) // send data byte via SPI
        #else
            #define fnWriteGLCD_cmd(command)     *((volatile unsigned char*)GLCD_COMMAND_ADDR) = (command)
            #define fnWriteGLCD_data(data)       *((volatile unsigned char*)GLCD_DATA_ADDR) = (data)
            #define fnReadGLCD_cmd()             *((volatile unsigned char*)GLCD_COMMAND_ADDR)
            #define fnReadGLCD_data()            *((volatile unsigned char*)GLCD_DATA_ADDR)
        #endif

        #define _GLCD_ST7565S_DEFINES                                    // include only once
    #endif
#endif
