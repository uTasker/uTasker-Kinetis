/***********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher

    ---------------------------------------------------------------------
    File:      glcd_oled.h
    Project:   uTasker project
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2016
    *********************************************************************
       
*/


#if defined OLED_GLCD_MODE && !defined TFT_GLCD_MODE && !defined CGLCD_GLCD_MODE && !defined KITRONIX_GLCD_MODE
    #if !defined _GLCD_OLED_DEFINES

        #define X_BYTES	                    (GLCD_X/8) 
        #define Y_BYTES	                    GLCD_Y 
        #define DISPLAY_LEFT_PIXEL          0  
        #define DISPLAY_TOP_PIXEL           0

        #define DISPLAY_RIGHT_PIXEL         (GLCD_X - 1)
        #define DISPLAY_BOTTOM_PIXEL        (GLCD_Y - 1)
        #define UPDATE_HEIGHT               Y_BYTES
        #define UPDATE_WIDTH                ((X_BYTES + 7)/8)

        #define SET_ADDRESS_POINTER         0x24
        #define SET_TEXT_HOME_ADDRESS       0x40
        #define SET_TEXT_HORIZ_BYTES        0x41
        #define SET_GRAPHICS_HOME_ADDRESS   0x42
        #define SET_GRAPHICS_HORIZ_BYTES    0x43
        #define GRAPHIC_MODE_XOR            0x81
        #define DISABLE_TEXT_DISABLE_CURSOR 0x98

        #define RIT_INIT_REMAP              0x52                         // app note says 0x51
        #define RIT_INIT_OFFSET             0x00

        #define WRITE_DATA_INC              0xc0

        #define GLCD_BUSY()                 0

        static void RIT128x96x4Init(unsigned long ulFrequency);

        static const unsigned char pucCommand1[] = {0x15, 0, ((GLCD_X - 2)/2)}; // command for setting write window to full screen
        static const unsigned char pucCommand2[] = {0x75, 0, (GLCD_Y - 1)};
        static const unsigned char g_pucRIT128x96x4HorizontalInc[] = { 0xA0, 0x52 }; // command horizontal increment

//*****************************************************************************
//
// The sequence of commands used to initialize the SSD1329 controller.  Each
// command is described as follows:  there is a byte specifying the number of
// bytes in the command sequence, followed by that many bytes of command data.
// Note:  This initialization sequence is derived from RIT App Note for
// the P14201.  Values used are from the RIT app note, except where noted.
//
//*****************************************************************************
static const unsigned char g_pucRIT128x96x4Init[] =
{
    //
    // Unlock commands
    //
    3, 0xFD, 0x12, 0xe3,

    //
    // Display off
    //
    2, 0xAE, 0xe3,

    //
    // Icon off
    //
    3, 0x94, 0, 0xe3,

    //
    // Multiplex ratio
    //
    3, 0xA8, 95, 0xe3,

    //
    // Contrast
    //
    3, 0x81, 0xb7, 0xe3,

    //
    // Pre-charge current
    //
    3, 0x82, 0x3f, 0xe3,

    //
    // Display Re-map
    //
    3, 0xA0, RIT_INIT_REMAP, 0xe3,

    //
    // Display Start Line
    //
    3, 0xA1, 0, 0xe3,

    //
    // Display Offset
    //
    3, 0xA2, RIT_INIT_OFFSET, 0xe3,

    //
    // Display Mode Normal
    //
    2, 0xA4, 0xe3,

    //
    // Phase Length
    //
    3, 0xB1, 0x11, 0xe3,

    //
    // Frame frequency
    //
    3, 0xB2, 0x23, 0xe3,

    //
    // Front Clock Divider
    //
    3, 0xB3, 0xe2, 0xe3,

    //
    // Set gray scale table.  App note uses default command:
    //2, 0xB7, 0xe3,
    // This gray scale attempts some gamma correction to reduce the
    // the brightness of the low levels.
    //
    17, 0xB8, 1, 2, 4, 6, 8, 10, 12, 14, 16, 18, 20, 22, 24, 28, 33, 0xe3,

    //
    // Second pre-charge period. App note uses value 0x04.
    //
    3, 0xBB, 0x01, 0xe3,

    //
    // Pre-charge voltage
    //
    3, 0xBC, 0x3f, 0xe3,

    //
    // Display ON
    //
    2, 0xAF, 0xe3,
};



        #define _GLCD_OLED_DEFINES                                       // include only once
    #endif

    #ifdef _GLCD_COMMANDS                                                // link in OLED specific interface commands

//*****************************************************************************
//
//! \internal
//!
//! Write a sequence of command bytes to the SSD1329 controller.
//!
//! The data is written in a polled fashion; this function will not return
//! until the entire byte sequence has been written to the controller.
//!
//! \return None.
//
//*****************************************************************************
static void fnSendSPI_Command(const unsigned char *pucBuffer, unsigned long ulCount)
{
    volatile unsigned char ucDummy;

    //
    // Clear the command/control bit to enable command mode.
    //
    OLED_DC_L();

    //
    // Loop while there are more bytes left to be transferred.
    //
    while(ulCount != 0)
    {
        //
        // Write the next byte to the controller.
        //
        WRITE_SPI_CMD(*pucBuffer);
#ifdef _WINDOWS
        CollectCommand(0, (unsigned char)SSIDR_0);
#endif
        pucBuffer++;

        WAIT_TRANSMISSON_END();                                          // wait for transfer to complete
        //
        // Dummy read to drain the fifo and time the GPIO signal.
        //
        ucDummy = READ_SPI_DATA();

        //
        // Decrement the BYTE counter.
        //
        ulCount--;
    }
}


static void fnSend_SPI_data(const unsigned char *pucBuffer, unsigned long ulCount)
{
    volatile unsigned char ucDummy;

    //
    // Clear the command/control bit to enable command mode.
    //
    OLED_DC_H();

    //
    // Loop while there are more bytes left to be transferred.
    //
    while (ulCount != 0) {
        //
        // Write the next byte to the controller.
        //
        WRITE_SPI_DATA(*pucBuffer);
#ifdef _WINDOWS
        CollectCommand(1, *pucBuffer);
#endif
        pucBuffer++;

        WAIT_TRANSMISSON_END();                                          // wait for transfer to complete
        //
        // Dummy read to drain the fifo and time the GPIO signal.
        //
        ucDummy = READ_SPI_DATA();

        //
        // Decrement the BYTE counter.
        //
        ulCount--;
    }
}

//*****************************************************************************
//
//! Clears the OLED display.
//!
//! This function will clear the display RAM.  All pixels in the display will
//! be turned off.
//!
//! \return None.
//
//*****************************************************************************
void RIT128x96x4Clear(void)
{
    unsigned long ulRow, ulColumn;
    unsigned char g_pucBuffer[8];

    //
    // Clear out the buffer used for sending bytes to the display.
    //
    uMemset(g_pucBuffer, 0, sizeof(g_pucBuffer));

    //
    // Set the window to fill the entire display.
    //
    fnSendSPI_Command(pucCommand1, sizeof(pucCommand1));
    fnSendSPI_Command(pucCommand2, sizeof(pucCommand2));
    fnSendSPI_Command(g_pucRIT128x96x4HorizontalInc, sizeof(g_pucRIT128x96x4HorizontalInc)); // command horizontal increment mode

    //
    // Loop through the rows
    //
    for (ulRow = 0; ulRow < GLCD_Y; ulRow++) {
        //
        // Loop through the columns.  Each byte is two pixels,
        // and the buffer hold 8 bytes, so 16 pixels are cleared
        // at a time.
        //
        for (ulColumn = 0; ulColumn < GLCD_X; ulColumn += 8 * 2) {
            //
            // Write 8 clearing bytes to the display, which will
            // clear 16 pixels across.
            //
            fnSend_SPI_data(g_pucBuffer, sizeof(g_pucBuffer));
        }
    }
}


static void RIT128x96x4Init(unsigned long ulFrequency)
{
    unsigned long ulIdx;

    //
    //
    // Clear the frame buffer.
    //
    RIT128x96x4Clear();

    //
    // Initialize the SSD1329 controller.  Loop through the initialization
    // sequence array, sending each command "string" to the controller.
    //
    for (ulIdx = 0; ulIdx < sizeof(g_pucRIT128x96x4Init); ulIdx += g_pucRIT128x96x4Init[ulIdx] + 1) {
        //
        // Send this command.
        //
        fnSendSPI_Command(g_pucRIT128x96x4Init + ulIdx + 1, g_pucRIT128x96x4Init[ulIdx] - 1);
    }
}

    #endif

    #ifdef GLCD_INIT
            CONFIGURE_OLED_DC();                                         // configure the data/command line
            ACTIVATE_OLED_POWER();                                       // apply power to the OLED (+15V)
            CONFIGURE_SPI();                                             // configure and enable the SPI interface ready for OLED communication
            RIT128x96x4Init(1000000);
            fnClearScreen();
    #endif
#endif
