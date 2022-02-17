/**********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher

    ---------------------------------------------------------------------
    File:      glcd_nokia.h
    Project:   uTasker project
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2016
    *********************************************************************
    20.10.2009 Add pixel format command during Philips controller initialisation {1}
       
*/


#if defined NOKIA_GLCD_MODE && !defined _GLCD_SAMSUNG && !defined OLED_GLCD_MODE && !defined TFT_GLCD_MODE && !defined CGLCD_GLCD_MODE && !defined KITRONIX_GLCD_MODE && !defined MB785_GLCD_MODE
    #if !defined _GLCD_NOKIA_DEFINES
        #define GLCD_BUSY()                 0                            // LCD busy check
        #define X_BYTES	                    ((GLCD_X + 7)/8)             // the number of bytes holding the X-pixels
        #define Y_BYTES	                    GLCD_Y                       // the number of rows of X_BYTES holding the Y rows
        #define DISPLAY_LEFT_PIXEL          0  
        #define DISPLAY_TOP_PIXEL           0
        #define DISPLAY_RIGHT_PIXEL         (GLCD_X - 1)                 // 2 pixels at on far right are not visible
        #define DISPLAY_BOTTOM_PIXEL        (GLCD_Y - 1)                 // 2 bottom pixels are not visible at top
        #define UPDATE_WIDTH                ((X_BYTES + 7)/8)
        #define UPDATE_HEIGHT               Y_BYTES

        #if defined PHILIPS_PCF8833
            #define NOP                     0x00                         // nop
            #define SOFTRST                 0x01                         // software reset
            #define BOOSTVOFF               0x02                         // booster voltage OFF
            #define BOOSTVON                0x03                         // booster voltage ON
            #define TESTMODE1               0x04                         // test mode
            #define DISPSTATUS              0x09                         // display status
            #define SLEEPIN                 0x10                         // sleep in
            #define SLEEPOUT                0x11                         // sleep out
            #define PARTIAL                 0x12                         // partial display mode
            #define NORMALMODE              0x13                         // display normal mode
            #define INVERSIONOFF            0x20                         // inversion OFF
            #define INVERSIONON             0x21                         // inversion ON
            #define ALLPIXELOFF             0x22                         // all pixel OFF
            #define ALLPIXELON              0x23                         // all pixel ON
            #define CONTRAST                0x25                         // write contrast
            #define DISPLAYOFF              0x28                         // display OFF
            #define DISON                   0x29                         // display ON
            #define CASET                   0x2A                         // column address set
            #define PASET                   0x2B                         // page address set
            #define RAMWR                   0x2C                         // memory write
            #define COLORSET                0x2D                         // colour set
            #define READRAMDATA             0x2E                         // RAM data read
            #define PARTIALAREA             0x30                         // partial area
            #define VERTSCROLL              0x33                         // vertical scrolling definition
            #define TESTMODE2               0x34                         // test mode
            #define TESTMODE3               0x35                         // test mode
            #define ACCESSCTRL              0x36                         // memory access control
            #define VSCRLSADDR              0x37                         // vertical scrolling start address
            #define IDLEOFF                 0x38                         // idle mode OFF
            #define IDLEON                  0x39                         // idle mode ON
            #define PIXELFORMAT             0x3A                         // interface pixel format
            #define TESTMODE4               0xDE                         // test mode
            #define NOP2                    0xAA                         // nop
            #define INITESC                 0xC6                         // initial escape
            #define TESTMODE5               0xDA                         // test mode
            #define TESTMODE6               0xDB                         // test mode
            #define TESTMODE7               0xDC                         // test mode
            #define TESTMODE8               0xB2                         // test mode
            #define GRAYSCALE0              0xB3                         // gray scale position set 0
            #define GRAYSCALE1              0xB4                         // gray scale position set 1
            #define GAMMA                   0xB5                         // gamma curve set
            #define DISPCTRL                0xB6                         // display control
            #define TEMPGRADIENT            0xB7                         // temp gradient set
            #define TESTMODE9               0xB8                         // test mode
            #define REFSET                  0xB9                         // refresh set
            #define VOLTCTRL                0xBA                         // voltage control
            #define COMMONDRV               0xBD                         // common driver output select
            #define PWRCTRL                 0xBE                         // power control
        #else                                                            // NOKIA_EPSON_S1D15G00
            #define DISON                   0xAF                         // Display on
            #define DISOFF                  0xAE                         // Display off
            #define DISNOR                  0xA6                         // Normal display
            #define DISINV                  0xA7                         // Inverse display
            #define COMSCN                  0xBB                         // Common scan direction
            #define DISCTL                  0xCA                         // Display control
            #define SLPIN                   0x95                         // Sleep in
            #define SLPOUT                  0x94                         // Sleep out
            #define PASET                   0x75                         // Page address set
            #define CASET                   0x15                         // Column address set
            #define DATCTL                  0xBC                         // Data scan direction, etc.
            #define RGBSET8                 0xCE                         // 256-color position set
            #define RAMWR                   0x5C                         // Writing to memory
            #define RAMRD                   0x5D                         // Reading from memory
            #define PTLIN                   0xA8                         // Partial display in
            #define PTLOUT                  0xA9                         // Partial display out
            #define RMWIN                   0xE0                         // Read and modify write
            #define RMWOUT                  0xEE                         // End
            #define ASCSET                  0xAA                         // Area scroll set
            #define SCSTART                 0xAB                         // Scroll start set
            #define OSCON                   0xD1                         // Internal oscillation on
            #define OSCOFF                  0xD2                         // Internal oscillation off
            #define PWRCTR                  0x20                         // Power control
            #define VOLTCTRL                0x81                         // Electronic volume control
            #define VOLUP                   0xD6                         // Increment electronic control by 1
            #define VOLDOWN                 0xD7                         // Decrement electronic control by 1
            #define TMPGRD                  0x82                         // Temperature gradient set
            #define EPCTIN                  0xCD                         // Control EEPROM
            #define EPCOUT                  0xCC                         // Cancel EEPROM control
            #define EPMWR                   0xFC                         // Write into EEPROM
            #define EPMRD                   0xFD                         // Read from EEPROM
            #define EPSRRD1                 0x7C                         // Read register 1
            #define EPSRRD2                 0x7D                         // Read register 2
            #define NOP                     0x25                         // NOP instruction
        #endif                                 

        static DELAY_LIMIT fnInitLCD(int iState);
        static void WriteSpiCommand(unsigned int data);

        #define _GLCD_NOKIA_DEFINES                                      // include only once
    #endif

    #ifdef _GLCD_COMMANDS                                                // link in Nokia specific interface commands

static void WriteSpiCommand(unsigned int data)
{
    data = (data & ~0x0100);
        #if defined _HW_SAM7X
    while((SPI_SR_0 & SPI_TXEMPTY) == 0) {                               // wait for the transfer to complete
            #ifdef _WINDOWS
        SPI_SR_0 |= (SPI_RDRF | SPI_TXEMPTY);                            // simulate tx and rx interrupt flags being set
            #endif
    }
    SPI_TDR_0 = data;
            #ifdef _WINDOWS
    CollectCommand(((SPI_TDR_0 & 0x100) != 0), (unsigned char)SPI_TDR_0);
            #endif
        #elif defined _LPC23XX || defined _LPC17XX
    while ((SSP0SR & SSP_TFE) == 0) {                                    // wait for the transfer to complete
            #ifdef _WINDOWS
        SSP0SR |= (SSP_TFE);
            #endif
    }
    SSP0DR = data;
            #ifdef _WINDOWS
    CollectCommand(((SSP0DR & 0x100) != 0), (unsigned char)SSP0DR);
            #endif
        #endif
}

static void WriteSpiData(unsigned int data)
{
    data |= 0x0100;
        #if defined _HW_SAM7X
    while((SPI_SR_0 & SPI_TXEMPTY) == 0) {                               // wait for the transfer to complete
            #ifdef _WINDOWS
        SPI_SR_0 |= (SPI_RDRF | SPI_TXEMPTY);                            // simulate tx and rx interrupt flags being set
            #endif
    }
    SPI_TDR_0 = data;
            #ifdef _WINDOWS
    CollectCommand(((SPI_TDR_0 & 0x100) != 0), (unsigned char)SPI_TDR_0);
            #endif
        #elif defined _LPC23XX || defined _LPC17XX
    while ((SSP0SR & SSP_TFE) == 0) {                                    // wait for the transfer to complete
            #ifdef _WINDOWS
        SSP0SR |= (SSP_TFE);
            #endif
    }
    SSP0DR = data;
            #ifdef _WINDOWS
    CollectCommand(((SSP0DR & 0x100) != 0), (unsigned char)SSP0DR);
            #endif
        #endif
}

static void fnSendSPI_Command(const unsigned char *pucbuffer, unsigned long ulCount)
{
    WriteSpiCommand(*pucbuffer++);                                       // send command 
    while (--ulCount != 0) {
        WriteSpiData(*pucbuffer++);                                      // followed by data bytes
    }
}

static void fnSend_SPI_data(const unsigned char *pucbuffer, unsigned long ulCount)
{
    while (ulCount-- != 0) {
        WriteSpiData(*pucbuffer++);                                      // data bytes
    }
}


static DELAY_LIMIT fnInitLCD(int iState)
{
    switch (iState) {
    case STATE_POWER_LCD_0:
        #if defined _HW_SAM7X
        POWER_UP(SPI0);                                                  // enable clocks to SPI in PMC

        PIO_ASR_A = (SPI_MOSI_0 | SPI_MISO_0 | SPI_CLK_0 | SPI_CS0_0);   // configure SPI0 lines
        PIO_PDR_A = (SPI_MOSI_0 | SPI_MISO_0 | SPI_CLK_0 | SPI_CS0_0);

        SPI_MR_0 = (SPI_CS_0 | MODFDIS | SPI_MSTR);                      // master mode 
        SPI_CSR0_0 = (SPI_CPOL | SPI_9_BITS | (((MASTER_CLOCK + 6000000/2)/6000000) << 8)); // 9 bit, 6MHz
        SPI_CR_0 = SPIEN;                                                // enable SPI
        #elif defined _LPC23XX || defined _LPC17XX
        POWER_UP(PCSSP0);                                                // power up the SSP used

        PINSEL3 |= (PINSEL3_P1_20_SCK0 | PINSEL3_P1_21_SSEL0 | PINSEL3_P1_23_MISO0 | PINSEL3_P1_24_MOSI0); // SSEL, MOSI, MISO and SCK pins enabled - on SSP0

        SSP0CR0 = 0;                                                     // ensure disabled (master mode)
        SSP0CPSR = 2;                                                    // set clock prescaler (even prescale 2..254)

        SSP0CR0 = (FRS_FREESCALE_SPI | DSS_9_BIT | ((((SSP_CLOCK/2 + 2000000/2)/2000000) - 1) << SSP_SCR_SHIFT)); // set 2MHz and mode
        SSP0CR1 = SSP_SSE;                                               // enable
        #endif

        #if defined PHILIPS_PCF8833
        WriteSpiCommand(SOFTRST);                                        // software reset
        return ((DELAY_LIMIT)(0.05*SEC));                                    
        #else                                                            // Epson controller
        WriteSpiCommand(DISCTL);                                         // display control
        WriteSpiData(0x00);                                              // default
        WriteSpiData(0x20);                                              // (32 + 1) * 4 = 132 lines (of which 130 are visible)
        WriteSpiData(0x0a);                                              // default

        WriteSpiCommand(COMSCN);                                         // COM scan
        WriteSpiData(0x00);                                              // scan 1-80

        WriteSpiCommand(OSCON);                                          // internal oscilator ON
        return (DELAY_LIMIT)(0.1*SEC);                                   // wait 100ms to stabilise
        #endif

    case STATE_POWER_LCD_1:
        #if defined PHILIPS_PCF8833
        WriteSpiCommand(INITESC);                                        // initial sequence
        return ((DELAY_LIMIT)(0.05*SEC)); 
        #else
        WriteSpiCommand(SLPOUT);                                         // sleep out

        WriteSpiCommand(VOLTCTRL);                                       // voltage control
        WriteSpiData(0x1f);                                              // middle value of V1
        WriteSpiData(0x03);                                              // middle value of resistance value

        WriteSpiCommand(TMPGRD);                                         // temperature gradient
        WriteSpiData(0x00);                                              // default

        WriteSpiCommand(PWRCTR);                                         // power control
        WriteSpiData(0x0f);                                              // referance voltage regulator on, circuit voltage follower on, BOOST ON

        WriteSpiCommand(DISNOR);                                         // normal display
        WriteSpiCommand(DISINV);                                         // inverse display
        WriteSpiCommand(PTLOUT);                                         // partial area off

        WriteSpiCommand(DATCTL);                                         // data control
        WriteSpiData(0x00);                                              // all inversions off, column direction
        WriteSpiData(0x01);                                              // RGB sequence
        WriteSpiData(0x02);                                              // grayscale -> 16
        #endif

        #if defined PHILIPS_PCF8833
        case STATE_POWER_LCD_2:
            WriteSpiCommand(REFSET);                                     // refresh set
            WriteSpiData(0);

            WriteSpiCommand(DISPCTRL);
            WriteSpiData(128);                                           // set the lenght of one selection term
            WriteSpiData(128);                                           // set N inversion -> no N inversion
            WriteSpiData(134);                                           // set frame frequence and bias rate -> 2 devision of frequency and 1/8 bias, 1/67 duty, 96x67 size
            WriteSpiData(84);                                            // set duty parameter
            WriteSpiData(69);                                            // set duty parameter
            WriteSpiData(82);                                            // set duty parameter
            WriteSpiData(67);                                            // set duty parameter

            WriteSpiCommand(GRAYSCALE0);
            WriteSpiData(1);                                             // GCP1 - gray level to be output when the RAM data is "0001"
            WriteSpiData(2);                                             // GCP2 - gray level to be output when the RAM data is "0010"
            WriteSpiData(4);                                             // GCP3 - gray level to be output when the RAM data is "0011"
            WriteSpiData(8);                                             // GCP4 - gray level to be output when the RAM data is "0100"
            WriteSpiData(16);                                            // GCP5 - gray level to be output when the RAM data is "0101"
            WriteSpiData(30);                                            // GCP6 - gray level to be output when the RAM data is "0110"
            WriteSpiData(40);                                            // GCP7 - gray level to be output when the RAM data is "0111"
            WriteSpiData(50);                                            // GCP8 - gray level to be output when the RAM data is "1000"
            WriteSpiData(60);                                            // GCP9 - gray level to be output when the RAM data is "1001"
            WriteSpiData(70);                                            // GCP10 - gray level to be output when the RAM data is "1010"
            WriteSpiData(80);                                            // GCP11 - gray level to be output when the RAM data is "1011"
            WriteSpiData(90);                                            // GCP12 - gray level to be output when the RAM data is "1100"
            WriteSpiData(100);                                           // GCP13 - gray level to be output when the RAM data is "1101"
            WriteSpiData(110);                                           // GCP14 - gray level to be output when the RAM data is "1110"
            WriteSpiData(127);                                           // GCP15 - gray level to be output when the RAM data is "1111"

            WriteSpiCommand(GAMMA);
            WriteSpiData(1);                                             // select grey scale

            WriteSpiCommand(COMMONDRV);
            WriteSpiData(0);                                             // set COM1-COM41 side come first, normal mode

            WriteSpiCommand(NORMALMODE);                                 // set normal mode


            WriteSpiCommand(ACCESSCTRL);
            WriteSpiData(0x40);                                          // memory access controller - horizontal
          //WriteSpiData(0xc8);                                          // memory access controller - mirror x-y (some displays may need this)

            WriteSpiCommand(PWRCTRL);
            WriteSpiData(4);                                             // internal resistance, V1OUT -> high power mode, oscillator devision rate

            WriteSpiCommand(PIXELFORMAT);
            WriteSpiData(3);                                             // {1} select 12 bit colour format

            WriteSpiCommand(SLEEPOUT);

            WriteSpiCommand(VOLTCTRL);                                   // voltage control and write contrast define LCD electronic volume
            WriteSpiData(0x7f);                                          // full voltage control
            WriteSpiData(0x03);                                          // must be "1"

            WriteSpiCommand(CONTRAST);                                   // write contrast
            WriteSpiData(0x3b);
            return ((DELAY_LIMIT)(0.05*SEC));

        case STATE_INITIALISING_1:
            {
                int i;
                WriteSpiCommand(TEMPGRADIENT);                           // temperature gradient
                for(i = 0; i < 14; i++) {
                    WriteSpiData(0);
                }
                WriteSpiCommand(BOOSTVON);                               // booster voltage ON
            }
            break;                                                       // initialisation complete
        #endif
    }
    return 0;
}
    #endif

    #if defined GLCD_INIT                                                // initialisation sequence
            uTaskerMonoTimer(OWN_TASK, (DELAY_LIMIT)(0.01*SEC), E_STABILISE_DELAY); // stabilising delay after taking CGLCD out of reset
            iLCD_State = STATE_POWER_LCD_0;
            return;
        case STATE_POWER_LCD_0:
        case STATE_POWER_LCD_1:
        case STATE_POWER_LCD_2:
        case STATE_INITIALISING_1:
            {
                DELAY_LIMIT delay_before_next = fnInitLCD(iLCD_State);
                if (delay_before_next != 0) {
                    uTaskerMonoTimer( OWN_TASK, delay_before_next, E_STABILISE_DELAY);
                    iLCD_State++;                                        // go to next initialisation state
                    return;
                }
                fnClearScreen();
                if (iLCD_State == STATE_LCD_CLEARING_DISPLAY) {
                    return;                                              // if yielding while clearing the entire display
                }
                WriteSpiCommand(DISON);                                  // display
                ENABLE_BACKLIGHT();                                      // and enable backlight
                fnEventMessage(LCD_PARTNER_TASK, TASK_LCD, E_LCD_INITIALISED); // inform the application that the initialisation has completed
                iLCD_State = STATE_LCD_READY;                            // we are ready to work....
            }
            break;
    #endif
#endif
