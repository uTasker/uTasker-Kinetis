/***********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher

    ---------------------------------------------------------------------
    File:      LCDSim.cpp
    Project:   Single Chip Embedded Internet
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2016
    *********************************************************************
    13.02.2007 Improve LCD re-draw {1} and correct initialisation in 4-line mode {2}
    05.05.2009 Add graphic LCD support - T6963C controller - SUPPORT_GLCD
    15.05.2009 Add OLED support - SSD1323 controller - SUPPORT_OLED
    16.05.2009 Restrict LCD redraw to window size changes                {3}
    02.09.2009 Add Samsung compatible display                            {4}
    10.09.2009 Remove incorrect and unnecessary initialisation of LCD dram sizes {5}
    26.09.2009 Allow OLED to be activated when OLED_GLCD_MODE is defined {6}
    27.09.2009 Allow TFT to be activated when TFT_GLCD_MODE is defined   {7}
    28.09.2009 Add Nokia GLCD and CGLCD compatibility mode               {8}
    06.10.2009 Initialise the graphical LCD pixel buffer with a pattern  {9}
    09.10.2009 GLCD_COLOR independently controls LCD
    04.11.2009 Add backlight control interface                           {10}
    16.01.2010 Remove OLED_X/Y and use only GLCD_X/Y                     {11}
    13.05.2010 Add KITRONIX_GLCD_MODE                                    {12}
    27.06.2010 Add MB785_GLCD_MODE                                       {13}
    28.08.2010 Add Cyrillic LCD font                                     {14}
    24.01.2011 Correct some colours when redrawing after rescale         {15}
    19.07.2011 Add SLCD simulation                                       {16}
    03.08.2011 Add return value to CollectCommand() and return LCD RAM content on address write {17}
    13.08.2013 Add ST7565S_GLCD_MODE                                     {18}
    15.03.2015 Introduce faster bitmap based LCD                         {19}

    */


#include <windows.h>
#include <sys/stat.h>
#if _VC80_UPGRADE >= 0x0600
    #include <share.h>
#endif
#define _EXCLUDE_WINDOWS_
#include "config.h"
#include "lcd.h"


#if defined SUPPORT_LCD || defined SUPPORT_GLCD || defined SUPPORT_OLED || defined SUPPORT_TFT || defined GLCD_COLOR || defined SLCD_FILE // {16}

#if defined SLCD_FILE
    static void fnLoadSLCD(void);
    static void fnDrawSegment(int iSegmentRef, int iOnOff);
    static void fnDrawAllSegments(int iDrawType);
    static int iSLCD_show_segments = 0;
#endif

extern void fnRedrawDisplay(void);

extern HWND ghWnd;

#if (defined SUPPORT_GLCD || defined SUPPORT_TFT || defined TFT_GLCD_MODE || defined GLCD_COLOR || defined SLCD_FILE) && !defined OLED_GLCD_MODE  // {6}{7}
    #define NON_DISPLAYED_X  0                                           // no non-visible pixels at the start of the display field
    #define LCD_PIXEL_X_REAL (GLCD_X + NON_DISPLAYED_X)
    //                                      [ page addr.  ][ Col. addr.]
    #if defined GLCD_COLOR || defined TFT_GLCD_MODE || defined NOKIA_GLCD_MODE || defined CGLCD_GLCD_MODE || defined KITRONIX_GLCD_MODE || defined TFT2N0369_GLCD_MODE || defined MB785_GLCD_MODE || defined SLCD_FILE // {7}{8}{13}
        static unsigned long ulGraphicPixels[GLCD_Y][GLCD_X];
    #else
        static unsigned char ucGraphicPixels[GLCD_Y/8][GLCD_X];
    #endif
    #if defined _GLCD_SAMSUNG || defined ST7565S_GLCD_MODE               // {18}
        #if defined ST7565S_GLCD_MODE
            #define SAMSUNG_CHIPS     1
        #endif
        static unsigned char ucColumn[SAMSUNG_CHIPS]    = {0};
        static int iGraphicFlags[SAMSUNG_CHIPS] = {0};
        static unsigned char ucPageAddress[SAMSUNG_CHIPS] = {0};
    #endif
    static unsigned char ucPageBlink = 0;
    static unsigned char ucPageBlinkMask = 0;

    #define DISPLAY_IS_ON             0x01
    #define DISPLAY_IS_REVERSE        0x02
    #define DISPLAY_ALL_PIXELS_ON     0x04
    #define RMW_ON                    0x08
    #define DISPLAY_COMMON_OUTPUT_REV 0x10

    #define PIXEL_16_BIT    0x05
    #if defined GLCD_COLOR || defined NOKIA_GLCD_MODE || defined CGLCD_GLCD_MODE || defined KITRONIX_GLCD_MODE || defined TFT2N0369_GLCD_MODE || defined MB785_GLCD_MODE // {8}{13}
        #define PIXEL_8_BIT     0x02
        #define PIXEL_12_BITA   0x03
        #define PIXEL_12_BITB   0x04
        #define PIXEL_18_BIT    0x06
        #define PIXEL_24_BIT    0x07
        static unsigned char  ucAutoloadSet = 0;
        static unsigned char  ucOTP_control = 0;
        static unsigned char  ucBias = 0;
        static unsigned char  ucBooster = 0;
        static unsigned char  ucComSegScan = 0;
        static unsigned short usVopSetting = 0;
        static unsigned short usColumnAddress = 0;
        static unsigned short usRowAddress = 0;
        static int iCommand2 = 0;
        static unsigned char ucCmd;
    #endif
    static unsigned short usPresentColumn = 0;
    static unsigned short usPresetRow = 0;
    static unsigned char  ucPixelFormat = PIXEL_16_BIT;
    static unsigned long ulNewPixel;
    static int iDataCnt = 0;
    static unsigned char  ucMemoryDataAccessControl = 0;
    static unsigned short usEndRow = 0x83;
    static unsigned short usStartRow = 0;
    static unsigned short usEndColumn = 0x83;
    static unsigned short usStartColumn = 0;
#elif (defined SUPPORT_OLED || defined OLED_GLCD_MODE)                   // {6}
    #define NON_DISPLAYED_X  0                                           // no non-visible pixels at the start of the display field
    #define ICON_ALL_OFF   0
    #define ICON_ALL_ON    1
    #define ICON_ALL_BLINK 2

    #define MODE_NORMAL    0
    #define MODE_INVERSE   1
    #define MODE_ALL_OFF   2
    #define MODE_ALL_ON    4

    static unsigned long ulGraphicAdd = 0;
    static unsigned char ucColumnStart = 0;
    static unsigned char ucColumnEnd = 0;
    static unsigned char ucRowStart = 0;
    static unsigned char ucRowEnd = 0;
    static unsigned char ucDisplayRamStart = 0;
    static unsigned char ucDisplayOffset = 0;
    static unsigned char ucRemap = 0;
    static unsigned char ucMCU_protection_status = 0x12;
    static unsigned char ucMuxRatio = 15;
    static unsigned char ucContrast = 0x80;
    static unsigned char ucSecondPrechargeSpeed = 0;
    static unsigned char ucSecondPrechargePeriod = 0;
    static unsigned char PrechargeVoltage = 0;
    static unsigned char ucPhaseLength = 0x53;
    static unsigned char ucFrameFrequency = 0x23;
    static unsigned char ucOscillatorFreq = 0;
    static unsigned char ucGrayScalePulseWidth[15] = {2, 4, 6, 8, 10, 12, 14, 16, 18, 20, 22, 24, 26, 28, 30};
    static int           iIcon = ICON_ALL_OFF;
    static int           iDisplayMode = MODE_NORMAL;

    static unsigned char OLED_graphical_memory[(GLCD_X/2) * GLCD_Y] = {0}; // {11}
#endif

static int iDisplayOn = 0;
static CRITICAL_SECTION cs;
static tLCD_MEM   tDisplayMem;
static tLCD_Info  LCD_Info;
static RECT rectLcd;

static tLCDFONT       font_tbl[(16*16)];
static unsigned int   font5x11[(16*16)][8];
#if defined LCD_SIMULATE_BACKLIGHT
    static int        nNewBacklight = 0;
    static int        nBackLightOn = 0;
    static int        nBackLightPercentage = 0;                          // when percentage is set to 0 the light is fully contolled by the port
#else
    static int        nBackLightOn = 1;
    static int        nBackLightPercentage = 0;
#endif
static unsigned int   uiCharStart[40];
static unsigned char  ucBlinkCursor = 0;
static unsigned short usOldValue[4][40];

static RECT rectLines[4];

static ULONG cmd = 0;
static int nibbel = 1;

static void Initfont(void);    
static void DrawLcdLine(HWND hwnd);
extern void fnSizeLCD(int iProcessorHeight, int iProcessorWidth);

static int iLCD_initialise = 0;



extern int fnInitLCD(RECT &rt, int iHeight, int iWidth)
{
#if (defined SUPPORT_GLCD || defined SUPPORT_TFT || defined TFT_GLCD_MODE || defined GLCD_COLOR || defined SLCD_FILE) && !defined OLED_GLCD_MODE
    #if defined GLCD_COLOR || defined TFT_GLCD_MODE || defined NOKIA_GLCD_MODE || defined CGLCD_GLCD_MODE || defined KITRONIX_GLCD_MODE || defined TFT2N0369_GLCD_MODE || defined MB785_GLCD_MODE || defined SLCD_FILE // {13}
    memset(ulGraphicPixels, 0x55, sizeof(ulGraphicPixels));              // {9} fill the graphic buffer with a pattern so that uninitialised pixels are visible
    #else
    memset(ucGraphicPixels, 0x55, sizeof(ucGraphicPixels));              // {9} fill the graphic buffer with a pattern so that uninitialised pixels are visible
    #endif
#endif
    fnSizeLCD(iHeight, iWidth);                                          // calculate the LCD coordinates
#if defined LCD_ON_KEYPAD
    return 0;                                                            // don't change the window size based on the LCD since it will be positioned over the keypad/panel
#else
    rt = rectLcd;

    rt.left = 0;
    if (iHeight > rt.bottom) {
        rt.bottom = iHeight;
    }
    rt.right += 20;
    return (rectLcd.bottom + 20);
#endif
}


extern void LCDinit(int iLines, int iChars)
{
#if defined SLCD_FILE
    if (iLines == 2) {                                                   // display all segments
        iSLCD_show_segments = 1;
        fnDrawAllSegments(1);                                            // draw all segments on
    }
    else if (iLines == 1) {                                              // allow normal mode again
        iSLCD_show_segments = 0;
        fnDrawAllSegments(0);                                            // draw all segments with their present values
        return;
    }
    else {
        fnLoadSLCD();
    }
#endif
    LCD_Info.ucNrOfLCDLines             = iLines;
    LCD_Info.ucNrOfVisibleCharacters    = iChars;
    LCD_Info.ucDDRAMLineLength[0]       = 40;
    LCD_Info.ucDDRAMLineLength[1]       = 40;
    
    LCD_Info.uiDDRAMStartLine[0]        = 0;
    LCD_Info.uiDDRAMStartLine[1]        = 0x40;

    LCD_Info.uiDDRamLength[0]           = 80;
    LCD_Info.uiDDRamLength[1]           = (0x40 + 40);

    LCD_Info.uiLCDStartLine[0]          = 0;
    LCD_Info.uiLCDStartLine[1]          = 0x40;
    LCD_Info.uiLCDStartLine[2]          = 20;
    LCD_Info.uiLCDStartLine[3]          = (0x40 + 20);

    tDisplayMem.bmode = 0;                                               // the LCD starts in 4 bit mode

    tDisplayMem.ucDDRAMLineLength = 0;// LCD_Info.ucDDRAMLineLength[tDisplayMem.ucLines-1]; {5}
    tDisplayMem.uiDDRamLength = 0; //LCD_Info.uiDDRamLength[tDisplayMem.ucLines-1]; {5} 

    tDisplayMem.ucFontType = 1;
    tDisplayMem.ucLCDshiftEnable = 0;
    tDisplayMem.ucCursorInc__Dec = 1;

    InitializeCriticalSection(&cs);                                      // start of critical region

    Initfont();                                                          // initialise the font table
    
    memset(tDisplayMem.ddrRam, ' ', sizeof(tDisplayMem.ddrRam));         // clear contents of character RAM
    
    tDisplayMem.init = 1;
    iLCD_initialise = 1;
}


static void fnInvalidateLCD(void)
{
    for (int x = 0; x < 4; x++) {                                        // we mark all characters as invalid to ensure that they re-draw
        for (int y = 0; y < 40; y++) {
            usOldValue[x][y] = 0xffff;                                   // mark value invalid and must be re-freshed
        }
    }
}


#if defined SUPPORT_TOUCH_SCREEN

extern "C" void fnPenPressed(int iX, int iY);
extern "C" void fnPenMoved(int iX, int iY);
extern "C" void fnPenLifted(void);

// Check whether pen down on touch screen
//
extern int fnPenDown(int x, int y, int iPenState)
{
    if (iPenState < 0) {                                                 // pen has just been lifted
        fnPenLifted();
        return 0;
    }
    if ((x < rectLcd.left) || (x >= rectLcd.right)) {
        if (iPenState != 0) {                                            // pen has left screen - treat as lifted
            fnPenLifted();
        }
        return 0;
    }
    if ((y < rectLcd.top) || (y >= rectLcd.bottom)) {
        if (iPenState != 0) {                                            // pen has left screen - treat as lifted
            fnPenLifted();
        }
        return 0;
    }
    if (iPenState == 0) {                                                // pen has just been applied
        fnPenPressed((x - rectLcd.left), (y - rectLcd.top));
        return 1;
    }
    fnPenMoved((x - rectLcd.left), (y - rectLcd.top));
    return 1;
}
#endif

// Draw LCD frame
//
extern int DisplayLCD(HWND hwnd, RECT rect)
{
    if ((NULL == hwnd) || (iLCD_initialise == 0)) {
        return rectLcd.bottom;
    }

    if ((rect.top != 0) || (rect.left != 0)) {                           // {3}
        return rectLcd.bottom;
    }
/*
    if ((rect.bottom < rectLcd.top) || (rect.top > rectLcd.bottom)) {    // only redraw when invalidated {1}{3} removed
        return rectLcd.bottom;
    }
    if ((rect.right < rectLcd.left) || (rect.left > rectLcd.right)) {    // only redraw when invalidated
        return rectLcd.bottom;
    }*/

    HDC hdc = GetDC(hwnd);
    DWORD error;

    fnInvalidateLCD();                                                   // ensure complete text is re-drawn

    HBRUSH hBrush, hOldBrush;
#if defined LCD_SIMULATE_BACKLIGHT
    nBackLightOn = nNewBacklight;                                        // new backlight state is only accepted when the redraw takes place
#endif
    if (nBackLightOn != 0) {
        hBrush = CreateSolidBrush(LCD_ON_COLOUR);                        // set ON colour
    }
    else {
        hBrush = CreateSolidBrush(LCD_OFF_COLOUR);                       // OFF colour
    }

    if (hBrush == 0) {
        error = GetLastError(); 
    }
    
    hOldBrush = (HBRUSH)SelectObject(hdc, hBrush);
    if (hOldBrush == 0) {
        error = GetLastError(); 
    }
    
    RoundRect(hdc, rectLcd.left, rectLcd.top, rectLcd.right, rectLcd.bottom, 5, 5); // draw an LCD frame
    DrawLcdLine(ghWnd);
    SelectObject(hdc, hOldBrush);
    DeleteObject(hBrush);
    ReleaseDC(hwnd, hdc);
    return rectLcd.bottom;
}

//
// Redefine the size of the frame when the window is moved or resized
//
static void fnSizeLCD(int iProcessorHeight, int iProcessorWidth)
{
#define CHAR_PIXEL_WIDTH  22
#define CHAR_PIXEL_HEIGHT 38
#define FRAME_PIXEL_EDGE 13
#define LIN_PIXEL_SPACE  5

    static POINT pLCD;

    pLCD.x = ((iProcessorWidth * 7) / 8);                                // start left of LCD
    pLCD.y = 75;

#if defined LCD_ON_KEYPAD
    pLCD.x += LCD_X_OFFSET;
    pLCD.y += LCD_Y_OFFSET;
#endif

#if (defined SUPPORT_GLCD || defined SUPPORT_TFT || defined GLCD_COLOR || defined SLCD_FILE) && !defined OLED_GLCD_MODES // {6}{16}
    #if defined BIG_PIXEL
        #define LCD_PIXEL_X  2
        #define LCD_PIXEL_Y  2
        #define SIDE_SPACE   20
        #define TOP_SPACE    14
    #else
        #define SIDE_SPACE   10
        #define TOP_SPACE    7
        #define LCD_PIXEL_X  1
        #define LCD_PIXEL_Y  1
    #endif
    rectLcd.left = pLCD.x;
    rectLcd.top = pLCD.y;
    rectLcd.right = (rectLcd.left + (GLCD_X * LCD_PIXEL_X) + (SIDE_SPACE * 2));
    rectLcd.bottom = (rectLcd.top + (GLCD_Y * LCD_PIXEL_Y) + (TOP_SPACE * 2));
    #if defined BIG_PIXEL
    rectLcd.right += LCD_PIXEL_X;
    rectLcd.bottom += LCD_PIXEL_Y;
    #endif
    rectLines[0].left = (rectLcd.left + SIDE_SPACE);
    rectLines[0].top = (rectLcd.top + TOP_SPACE);
#elif defined SUPPORT_OLED || defined OLED_GLCD_MODE                     // {6}
    #if defined BIG_PIXEL
        #define LCD_PIXEL_X  2
        #define LCD_PIXEL_Y  2
        #define SIDE_SPACE   20
        #define TOP_SPACE    14
    #else
        #define SIDE_SPACE   10
        #define TOP_SPACE    7
        #define LCD_PIXEL_X  1
        #define LCD_PIXEL_Y  1
    #endif
    rectLcd.left = pLCD.x;
    rectLcd.top = pLCD.y;
    rectLcd.right = rectLcd.left + (GLCD_X * LCD_PIXEL_X) + SIDE_SPACE*2;// {11}
    rectLcd.bottom = rectLcd.top + (GLCD_Y * LCD_PIXEL_Y) + TOP_SPACE*2;
    #if defined BIG_PIXEL
    rectLcd.right += LCD_PIXEL_X;
    rectLcd.bottom += LCD_PIXEL_Y;
    #endif
    rectLines[0].left = rectLcd.left + SIDE_SPACE;        
    rectLines[0].top = rectLcd.top + TOP_SPACE;
#else
    rectLines[0].left = pLCD.x;        
    rectLines[0].right = pLCD.x + (LCD_CHARACTERS * CHAR_PIXEL_WIDTH);
    rectLines[1].left = rectLines[0].left;
    rectLines[1].right = rectLines[0].right;
    rectLines[2].left = rectLines[0].left;
    rectLines[2].right = rectLines[0].right;
    rectLines[3].left = rectLines[0].left;
    rectLines[3].right = rectLines[0].right;

    rectLcd.left = rectLines[0].left - FRAME_PIXEL_EDGE;
    rectLcd.right = rectLines[0].right + FRAME_PIXEL_EDGE;

    rectLines[0].top = pLCD.y;
    rectLines[0].bottom = rectLines[0].top + CHAR_PIXEL_HEIGHT;

    rectLines[1].top = rectLines[0].bottom + LIN_PIXEL_SPACE;
    rectLines[1].bottom = rectLines[1].top + CHAR_PIXEL_HEIGHT;          // {2}

    rectLines[2].top = rectLines[1].bottom + LIN_PIXEL_SPACE;
    rectLines[2].bottom = rectLines[2].top + CHAR_PIXEL_HEIGHT;

    rectLines[3].top = rectLines[2].bottom + LIN_PIXEL_SPACE;
    rectLines[3].bottom = rectLines[3].top + CHAR_PIXEL_HEIGHT;

    rectLines[0].left = rectLcd.left + 15;        
    rectLines[0].right = rectLcd.right - 15;
    rectLines[1].left = rectLines[0].left;
    rectLines[1].right = rectLines[0].right;
    rectLines[2].left = rectLines[0].left;
    rectLines[2].right = rectLines[0].right;
    rectLines[3].left = rectLines[0].left;
    rectLines[3].right = rectLines[0].right;

    rectLcd.left = rectLines[0].left - FRAME_PIXEL_EDGE;
    rectLcd.top = rectLines[0].top - FRAME_PIXEL_EDGE;
    rectLcd.right = rectLines[0].left + (LCD_CHARACTERS * CHAR_PIXEL_WIDTH) + FRAME_PIXEL_EDGE;
    rectLcd.bottom = rectLines[0].top + (LCD_LINES * CHAR_PIXEL_HEIGHT) + FRAME_PIXEL_EDGE;


    for (int j = 0; j < 40; j++) {
        uiCharStart[j] = rectLines[0].left + (j*CHAR_PIXEL_WIDTH);       // define the starting point of each individual character
    }
#endif
}


// Character LCD Font initialisation
//
static void Initfont(void)
{
    font_tbl[0x00].font_y0 = 0x1f;                                       // invalid pattern    
    font_tbl[0x00].font_y1 = 0x1b;        
    font_tbl[0x00].font_y2 = 0x15;        
    font_tbl[0x00].font_y3 = 0x1b;        
    font_tbl[0x00].font_y4 = 0x15;        
    font_tbl[0x00].font_y5 = 0x1b;        
    font_tbl[0x00].font_y6 = 0x15;        
    font_tbl[0x00].font_y7 = 0x1f;    
#if defined LCD_CYRILLIC_FONT                                            // {14}
    // space                          // 0                              // @                              // P                              // '                              // p                              
    font_tbl[0x20].font_y0 = 0x00;    font_tbl[0x30].font_y0 = 0x0e;    font_tbl[0x40].font_y0 = 0x0e;    font_tbl[0x50].font_y0 = 0x1e;    font_tbl[0x60].font_y0 = 0x08;    font_tbl[0x70].font_y0 = 0x00;    font_tbl[0xA0].font_y0 = 0x1f;    font_tbl[0xB0].font_y0 = 0x12;    font_tbl[0xC0].font_y0 = 0x00;    font_tbl[0xD0].font_y0 = 0x00;    font_tbl[0xE0].font_y0 = 0x0e;    font_tbl[0xF0].font_y0 = 0x11;
    font_tbl[0x20].font_y1 = 0x00;    font_tbl[0x30].font_y1 = 0x11;    font_tbl[0x40].font_y1 = 0x11;    font_tbl[0x50].font_y1 = 0x11;    font_tbl[0x60].font_y1 = 0x04;    font_tbl[0x70].font_y1 = 0x00;    font_tbl[0xA0].font_y1 = 0x11;    font_tbl[0xB0].font_y1 = 0x15;    font_tbl[0xC0].font_y1 = 0x00;    font_tbl[0xD0].font_y1 = 0x00;    font_tbl[0xE0].font_y1 = 0x05;    font_tbl[0xF0].font_y1 = 0x12;
    font_tbl[0x20].font_y2 = 0x00;    font_tbl[0x30].font_y2 = 0x13;    font_tbl[0x40].font_y2 = 0x01;    font_tbl[0x50].font_y2 = 0x11;    font_tbl[0x60].font_y2 = 0x02;    font_tbl[0x70].font_y2 = 0x1e;    font_tbl[0xA0].font_y2 = 0x10;    font_tbl[0xB0].font_y2 = 0x15;    font_tbl[0xC0].font_y2 = 0x11;    font_tbl[0xD0].font_y2 = 0x00;    font_tbl[0xE0].font_y2 = 0x05;    font_tbl[0xF0].font_y2 = 0x15;
    font_tbl[0x20].font_y3 = 0x00;    font_tbl[0x30].font_y3 = 0x15;    font_tbl[0x40].font_y3 = 0x0d;    font_tbl[0x50].font_y3 = 0x1e;    font_tbl[0x60].font_y3 = 0x00;    font_tbl[0x70].font_y3 = 0x11;    font_tbl[0xA0].font_y3 = 0x1e;    font_tbl[0xB0].font_y3 = 0x1d;    font_tbl[0xC0].font_y3 = 0x11;    font_tbl[0xD0].font_y3 = 0x00;    font_tbl[0xE0].font_y3 = 0x05;    font_tbl[0xF0].font_y3 = 0x0b;
    font_tbl[0x20].font_y4 = 0x00;    font_tbl[0x30].font_y4 = 0x19;    font_tbl[0x40].font_y4 = 0x15;    font_tbl[0x50].font_y4 = 0x10;    font_tbl[0x60].font_y4 = 0x00;    font_tbl[0x70].font_y4 = 0x1e;    font_tbl[0xA0].font_y4 = 0x11;    font_tbl[0xB0].font_y4 = 0x15;    font_tbl[0xC0].font_y4 = 0x0f;    font_tbl[0xD0].font_y4 = 0x00;    font_tbl[0xE0].font_y4 = 0x09;    font_tbl[0xF0].font_y4 = 0x17;
    font_tbl[0x20].font_y5 = 0x00;    font_tbl[0x30].font_y5 = 0x11;    font_tbl[0x40].font_y5 = 0x15;    font_tbl[0x50].font_y5 = 0x10;    font_tbl[0x60].font_y5 = 0x00;    font_tbl[0x70].font_y5 = 0x10;    font_tbl[0xA0].font_y5 = 0x11;    font_tbl[0xB0].font_y5 = 0x15;    font_tbl[0xC0].font_y5 = 0x01;    font_tbl[0xD0].font_y5 = 0x04;    font_tbl[0xE0].font_y5 = 0x11;    font_tbl[0xF0].font_y5 = 0x01;
    font_tbl[0x20].font_y6 = 0x00;    font_tbl[0x30].font_y6 = 0x0e;    font_tbl[0x40].font_y6 = 0x0e;    font_tbl[0x50].font_y6 = 0x10;    font_tbl[0x60].font_y6 = 0x00;    font_tbl[0x70].font_y6 = 0x10;    font_tbl[0xA0].font_y6 = 0x1e;    font_tbl[0xB0].font_y6 = 0x12;    font_tbl[0xC0].font_y6 = 0x01;    font_tbl[0xD0].font_y6 = 0x04;    font_tbl[0xE0].font_y6 = 0x1f;    font_tbl[0xF0].font_y6 = 0x01;
    font_tbl[0x20].font_y7 = 0x00;    font_tbl[0x30].font_y7 = 0x00;    font_tbl[0x40].font_y7 = 0x00;    font_tbl[0x50].font_y7 = 0x00;    font_tbl[0x60].font_y7 = 0x00;    font_tbl[0x70].font_y7 = 0x00;    font_tbl[0xA0].font_y7 = 0x00;    font_tbl[0xB0].font_y7 = 0x00;    font_tbl[0xC0].font_y7 = 0x00;    font_tbl[0xD0].font_y7 = 0x00;    font_tbl[0xE0].font_y7 = 0x11;    font_tbl[0xF0].font_y7 = 0x00;

    // !                              // 1                              // A                              // Q                              // a                              // q                              
    font_tbl[0x21].font_y0 = 0x04;    font_tbl[0x31].font_y0 = 0x04;    font_tbl[0x41].font_y0 = 0x0e;    font_tbl[0x51].font_y0 = 0x0e;    font_tbl[0x61].font_y0 = 0x00;    font_tbl[0x71].font_y0 = 0x00;    font_tbl[0xA1].font_y0 = 0x1f;    font_tbl[0xB1].font_y0 = 0x0f;    font_tbl[0xC1].font_y0 = 0x00;    font_tbl[0xD1].font_y0 = 0x00;    font_tbl[0xE1].font_y0 = 0x11;    font_tbl[0xF1].font_y0 = 0x11;
    font_tbl[0x21].font_y1 = 0x04;    font_tbl[0x31].font_y1 = 0x0c;    font_tbl[0x41].font_y1 = 0x11;    font_tbl[0x51].font_y1 = 0x11;    font_tbl[0x61].font_y1 = 0x00;    font_tbl[0x71].font_y1 = 0x00;    font_tbl[0xA1].font_y1 = 0x11;    font_tbl[0xB1].font_y1 = 0x11;    font_tbl[0xC1].font_y1 = 0x00;    font_tbl[0xD1].font_y1 = 0x00;    font_tbl[0xE1].font_y1 = 0x11;    font_tbl[0xF1].font_y1 = 0x12;
    font_tbl[0x21].font_y2 = 0x04;    font_tbl[0x31].font_y2 = 0x04;    font_tbl[0x41].font_y2 = 0x11;    font_tbl[0x51].font_y2 = 0x11;    font_tbl[0x61].font_y2 = 0x0e;    font_tbl[0x71].font_y2 = 0x0d;    font_tbl[0xA1].font_y2 = 0x10;    font_tbl[0xB1].font_y2 = 0x11;    font_tbl[0xC1].font_y2 = 0x15;    font_tbl[0xD1].font_y2 = 0x00;    font_tbl[0xE1].font_y2 = 0x11;    font_tbl[0xF1].font_y2 = 0x17;
    font_tbl[0x21].font_y3 = 0x04;    font_tbl[0x31].font_y3 = 0x04;    font_tbl[0x41].font_y3 = 0x1f;    font_tbl[0x51].font_y3 = 0x11;    font_tbl[0x61].font_y3 = 0x01;    font_tbl[0x71].font_y3 = 0x13;    font_tbl[0xA1].font_y3 = 0x10;    font_tbl[0xB1].font_y3 = 0x0f;    font_tbl[0xC1].font_y3 = 0x15;    font_tbl[0xD1].font_y3 = 0x00;    font_tbl[0xE1].font_y3 = 0x11;    font_tbl[0xF1].font_y3 = 0x09;
    font_tbl[0x21].font_y4 = 0x00;    font_tbl[0x31].font_y4 = 0x04;    font_tbl[0x41].font_y4 = 0x11;    font_tbl[0x51].font_y4 = 0x15;    font_tbl[0x61].font_y4 = 0x0f;    font_tbl[0x71].font_y4 = 0x0f;    font_tbl[0xA1].font_y4 = 0x10;    font_tbl[0xB1].font_y4 = 0x05;    font_tbl[0xC1].font_y4 = 0x15;    font_tbl[0xD1].font_y4 = 0x04;    font_tbl[0xE1].font_y4 = 0x11;    font_tbl[0xF1].font_y4 = 0x13;
    font_tbl[0x21].font_y5 = 0x00;    font_tbl[0x31].font_y5 = 0x04;    font_tbl[0x41].font_y5 = 0x11;    font_tbl[0x51].font_y5 = 0x12;    font_tbl[0x61].font_y5 = 0x11;    font_tbl[0x71].font_y5 = 0x01;    font_tbl[0xA1].font_y5 = 0x10;    font_tbl[0xB1].font_y5 = 0x09;    font_tbl[0xC1].font_y5 = 0x15;    font_tbl[0xD1].font_y5 = 0x04;    font_tbl[0xE1].font_y5 = 0x11;    font_tbl[0xF1].font_y5 = 0x01;
    font_tbl[0x21].font_y6 = 0x04;    font_tbl[0x31].font_y6 = 0x0e;    font_tbl[0x41].font_y6 = 0x11;    font_tbl[0x51].font_y6 = 0x0d;    font_tbl[0x61].font_y6 = 0x0f;    font_tbl[0x71].font_y6 = 0x01;    font_tbl[0xA1].font_y6 = 0x10;    font_tbl[0xB1].font_y6 = 0x11;    font_tbl[0xC1].font_y6 = 0x1f;    font_tbl[0xD1].font_y6 = 0x04;    font_tbl[0xE1].font_y6 = 0x1f;    font_tbl[0xF1].font_y6 = 0x07;
    font_tbl[0x21].font_y7 = 0x00;    font_tbl[0x31].font_y7 = 0x00;    font_tbl[0x41].font_y7 = 0x00;    font_tbl[0x51].font_y7 = 0x00;    font_tbl[0x61].font_y7 = 0x00;    font_tbl[0x71].font_y7 = 0x00;    font_tbl[0xA1].font_y7 = 0x00;    font_tbl[0xB1].font_y7 = 0x00;    font_tbl[0xC1].font_y7 = 0x00;    font_tbl[0xD1].font_y7 = 0x00;    font_tbl[0xE1].font_y7 = 0x01;    font_tbl[0xF1].font_y7 = 0x00;    

    // "                              // 2                              // B                              // R                              // b                              // r                                                  
    font_tbl[0x22].font_y0 = 0x0a;    font_tbl[0x32].font_y0 = 0x0e;    font_tbl[0x42].font_y0 = 0x1e;    font_tbl[0x52].font_y0 = 0x1e;    font_tbl[0x62].font_y0 = 0x10;    font_tbl[0x72].font_y0 = 0x00;    font_tbl[0xA2].font_y0 = 0x0a;    font_tbl[0xB2].font_y0 = 0x03;    font_tbl[0xC2].font_y0 = 0x00;    font_tbl[0xD2].font_y0 = 0x00;    font_tbl[0xE2].font_y0 = 0x15;    font_tbl[0xF2].font_y0 = 0x11;
    font_tbl[0x22].font_y1 = 0x0a;    font_tbl[0x32].font_y1 = 0x11;    font_tbl[0x42].font_y1 = 0x11;    font_tbl[0x52].font_y1 = 0x11;    font_tbl[0x62].font_y1 = 0x10;    font_tbl[0x72].font_y1 = 0x00;    font_tbl[0xA2].font_y1 = 0x00;    font_tbl[0xB2].font_y1 = 0x0c;    font_tbl[0xC2].font_y1 = 0x00;    font_tbl[0xD2].font_y1 = 0x00;    font_tbl[0xE2].font_y1 = 0x15;    font_tbl[0xF2].font_y1 = 0x12;
    font_tbl[0x22].font_y2 = 0x0a;    font_tbl[0x32].font_y2 = 0x01;    font_tbl[0x42].font_y2 = 0x11;    font_tbl[0x52].font_y2 = 0x11;    font_tbl[0x62].font_y2 = 0x16;    font_tbl[0x72].font_y2 = 0x16;    font_tbl[0xA2].font_y2 = 0x1e;    font_tbl[0xB2].font_y2 = 0x10;    font_tbl[0xC2].font_y2 = 0x18;    font_tbl[0xD2].font_y2 = 0x0a;    font_tbl[0xE2].font_y2 = 0x15;    font_tbl[0xF2].font_y2 = 0x17;
    font_tbl[0x22].font_y3 = 0x00;    font_tbl[0x32].font_y3 = 0x02;    font_tbl[0x42].font_y3 = 0x1e;    font_tbl[0x52].font_y3 = 0x1e;    font_tbl[0x62].font_y3 = 0x19;    font_tbl[0x72].font_y3 = 0x19;    font_tbl[0xA2].font_y3 = 0x10;    font_tbl[0xB2].font_y3 = 0x1e;    font_tbl[0xC2].font_y3 = 0x08;    font_tbl[0xD2].font_y3 = 0x0a;    font_tbl[0xE2].font_y3 = 0x15;    font_tbl[0xF2].font_y3 = 0x09;
    font_tbl[0x22].font_y4 = 0x00;    font_tbl[0x32].font_y4 = 0x04;    font_tbl[0x42].font_y4 = 0x11;    font_tbl[0x52].font_y4 = 0x14;    font_tbl[0x62].font_y4 = 0x11;    font_tbl[0x72].font_y4 = 0x10;    font_tbl[0xA2].font_y4 = 0x1c;    font_tbl[0xB2].font_y4 = 0x11;    font_tbl[0xC2].font_y4 = 0x0e;    font_tbl[0xD2].font_y4 = 0x0a;    font_tbl[0xE2].font_y4 = 0x15;    font_tbl[0xF2].font_y4 = 0x13;
    font_tbl[0x22].font_y5 = 0x00;    font_tbl[0x32].font_y5 = 0x08;    font_tbl[0x42].font_y5 = 0x11;    font_tbl[0x52].font_y5 = 0x12;    font_tbl[0x62].font_y5 = 0x11;    font_tbl[0x72].font_y5 = 0x10;    font_tbl[0xA2].font_y5 = 0x10;    font_tbl[0xB2].font_y5 = 0x11;    font_tbl[0xC2].font_y5 = 0x09;    font_tbl[0xD2].font_y5 = 0x00;    font_tbl[0xE2].font_y5 = 0x15;    font_tbl[0xF2].font_y5 = 0x04;
    font_tbl[0x22].font_y6 = 0x00;    font_tbl[0x32].font_y6 = 0x1f;    font_tbl[0x42].font_y6 = 0x1e;    font_tbl[0x52].font_y6 = 0x11;    font_tbl[0x62].font_y6 = 0x1e;    font_tbl[0x72].font_y6 = 0x10;    font_tbl[0xA2].font_y6 = 0x1e;    font_tbl[0xB2].font_y6 = 0x0e;    font_tbl[0xC2].font_y6 = 0x0e;    font_tbl[0xD2].font_y6 = 0x0e;    font_tbl[0xE2].font_y6 = 0x1f;    font_tbl[0xF2].font_y6 = 0x07;
    font_tbl[0x22].font_y7 = 0x00;    font_tbl[0x32].font_y7 = 0x00;    font_tbl[0x42].font_y7 = 0x00;    font_tbl[0x52].font_y7 = 0x00;    font_tbl[0x62].font_y7 = 0x00;    font_tbl[0x72].font_y7 = 0x00;    font_tbl[0xA2].font_y7 = 0x00;    font_tbl[0xB2].font_y7 = 0x00;    font_tbl[0xC2].font_y7 = 0x00;    font_tbl[0xD2].font_y7 = 0x00;    font_tbl[0xE2].font_y7 = 0x01;    font_tbl[0xF2].font_y7 = 0x00;    

    // #                              // 3                              // C                              // S                              // c                              // s                                                  
    font_tbl[0x23].font_y0 = 0x0a;    font_tbl[0x33].font_y0 = 0x1f;    font_tbl[0x43].font_y0 = 0x0e;    font_tbl[0x53].font_y0 = 0x0f;    font_tbl[0x63].font_y0 = 0x00;    font_tbl[0x73].font_y0 = 0x00;    font_tbl[0xA3].font_y0 = 0x15;    font_tbl[0xB3].font_y0 = 0x00;    font_tbl[0xC3].font_y0 = 0x00;    font_tbl[0xD3].font_y0 = 0x00;    font_tbl[0xE3].font_y0 = 0x00;    font_tbl[0xF3].font_y0 = 0x18;
    font_tbl[0x23].font_y1 = 0x0a;    font_tbl[0x33].font_y1 = 0x02;    font_tbl[0x43].font_y1 = 0x11;    font_tbl[0x53].font_y1 = 0x10;    font_tbl[0x63].font_y1 = 0x00;    font_tbl[0x73].font_y1 = 0x00;    font_tbl[0xA3].font_y1 = 0x15;    font_tbl[0xB3].font_y1 = 0x00;    font_tbl[0xC3].font_y1 = 0x00;    font_tbl[0xD3].font_y1 = 0x00;    font_tbl[0xE3].font_y1 = 0x00;    font_tbl[0xF3].font_y1 = 0x08;
    font_tbl[0x23].font_y2 = 0x1f;    font_tbl[0x33].font_y2 = 0x04;    font_tbl[0x43].font_y2 = 0x10;    font_tbl[0x53].font_y2 = 0x10;    font_tbl[0x63].font_y2 = 0x0e;    font_tbl[0x73].font_y2 = 0x0e;    font_tbl[0xA3].font_y2 = 0x15;    font_tbl[0xB3].font_y2 = 0x1c;    font_tbl[0xC3].font_y2 = 0x11;    font_tbl[0xD3].font_y2 = 0x0a;    font_tbl[0xE3].font_y2 = 0x0f;    font_tbl[0xF3].font_y2 = 0x18;
    font_tbl[0x23].font_y3 = 0x0a;    font_tbl[0x33].font_y3 = 0x02;    font_tbl[0x43].font_y3 = 0x10;    font_tbl[0x53].font_y3 = 0x0e;    font_tbl[0x63].font_y3 = 0x10;    font_tbl[0x73].font_y3 = 0x10;    font_tbl[0xA3].font_y3 = 0x0e;    font_tbl[0xB3].font_y3 = 0x12;    font_tbl[0xC3].font_y3 = 0x11;    font_tbl[0xD3].font_y3 = 0x0a;    font_tbl[0xE3].font_y3 = 0x05;    font_tbl[0xF3].font_y3 = 0x09;
    font_tbl[0x23].font_y4 = 0x1f;    font_tbl[0x33].font_y4 = 0x01;    font_tbl[0x43].font_y4 = 0x10;    font_tbl[0x53].font_y4 = 0x01;    font_tbl[0x63].font_y4 = 0x10;    font_tbl[0x73].font_y4 = 0x0e;    font_tbl[0xA3].font_y4 = 0x15;    font_tbl[0xB3].font_y4 = 0x1c;    font_tbl[0xC3].font_y4 = 0x19;    font_tbl[0xD3].font_y4 = 0x0a;    font_tbl[0xE3].font_y4 = 0x09;    font_tbl[0xF3].font_y4 = 0x1b;
    font_tbl[0x23].font_y5 = 0x0a;    font_tbl[0x33].font_y5 = 0x11;    font_tbl[0x43].font_y5 = 0x11;    font_tbl[0x53].font_y5 = 0x01;    font_tbl[0x63].font_y5 = 0x10;    font_tbl[0x73].font_y5 = 0x01;    font_tbl[0xA3].font_y5 = 0x15;    font_tbl[0xB3].font_y5 = 0x12;    font_tbl[0xC3].font_y5 = 0x15;    font_tbl[0xD3].font_y5 = 0x00;    font_tbl[0xE3].font_y5 = 0x11;    font_tbl[0xF3].font_y5 = 0x07;
    font_tbl[0x23].font_y6 = 0x0a;    font_tbl[0x33].font_y6 = 0x0e;    font_tbl[0x43].font_y6 = 0x0e;    font_tbl[0x53].font_y6 = 0x1e;    font_tbl[0x63].font_y6 = 0x0e;    font_tbl[0x73].font_y6 = 0x1e;    font_tbl[0xA3].font_y6 = 0x15;    font_tbl[0xB3].font_y6 = 0x1c;    font_tbl[0xC3].font_y6 = 0x19;    font_tbl[0xD3].font_y6 = 0x0a;    font_tbl[0xE3].font_y6 = 0x1f;    font_tbl[0xF3].font_y6 = 0x01;
    font_tbl[0x23].font_y7 = 0x00;    font_tbl[0x33].font_y7 = 0x00;    font_tbl[0x43].font_y7 = 0x00;    font_tbl[0x53].font_y7 = 0x00;    font_tbl[0x63].font_y7 = 0x00;    font_tbl[0x73].font_y7 = 0x00;    font_tbl[0xA3].font_y7 = 0x00;    font_tbl[0xB3].font_y7 = 0x00;    font_tbl[0xC3].font_y7 = 0x00;    font_tbl[0xD3].font_y7 = 0x00;    font_tbl[0xE3].font_y7 = 0x11;    font_tbl[0xF3].font_y7 = 0x01;    
																																																										
    // $                              // 4                              // D                              // T                              // d                              // t                                                  
    font_tbl[0x24].font_y0 = 0x04;    font_tbl[0x34].font_y0 = 0x02;    font_tbl[0x44].font_y0 = 0x1c;    font_tbl[0x54].font_y0 = 0x1f;    font_tbl[0x64].font_y0 = 0x01;    font_tbl[0x74].font_y0 = 0x08;    font_tbl[0xA4].font_y0 = 0x1e;    font_tbl[0xB4].font_y0 = 0x00;    font_tbl[0xC4].font_y0 = 0x00;    font_tbl[0xD4].font_y0 = 0x14;    font_tbl[0xE4].font_y0 = 0x00;    font_tbl[0xF4].font_y0 = 0x11;
    font_tbl[0x24].font_y1 = 0x0f;    font_tbl[0x34].font_y1 = 0x06;    font_tbl[0x44].font_y1 = 0x12;    font_tbl[0x54].font_y1 = 0x04;    font_tbl[0x64].font_y1 = 0x01;    font_tbl[0x74].font_y1 = 0x08;    font_tbl[0xA4].font_y1 = 0x01;    font_tbl[0xB4].font_y1 = 0x00;    font_tbl[0xC4].font_y1 = 0x00;    font_tbl[0xD4].font_y1 = 0x00;    font_tbl[0xE4].font_y1 = 0x04;    font_tbl[0xF4].font_y1 = 0x1f;
    font_tbl[0x24].font_y2 = 0x14;    font_tbl[0x34].font_y2 = 0x0a;    font_tbl[0x44].font_y2 = 0x11;    font_tbl[0x54].font_y2 = 0x04;    font_tbl[0x64].font_y2 = 0x0d;    font_tbl[0x74].font_y2 = 0x1c;    font_tbl[0xA4].font_y2 = 0x01;    font_tbl[0xB4].font_y2 = 0x1f;    font_tbl[0xC4].font_y2 = 0x10;    font_tbl[0xD4].font_y2 = 0x1d;    font_tbl[0xE4].font_y2 = 0x04;    font_tbl[0xF4].font_y2 = 0x11;
    font_tbl[0x24].font_y3 = 0x0e;    font_tbl[0x34].font_y3 = 0x12;    font_tbl[0x44].font_y3 = 0x11;    font_tbl[0x54].font_y3 = 0x04;    font_tbl[0x64].font_y3 = 0x13;    font_tbl[0x74].font_y3 = 0x08;    font_tbl[0xA4].font_y3 = 0x0e;    font_tbl[0xB4].font_y3 = 0x11;    font_tbl[0xC4].font_y3 = 0x10;    font_tbl[0xD4].font_y3 = 0x02;    font_tbl[0xE4].font_y3 = 0x0e;    font_tbl[0xF4].font_y3 = 0x1f;
    font_tbl[0x24].font_y4 = 0x05;    font_tbl[0x34].font_y4 = 0x1f;    font_tbl[0x44].font_y4 = 0x11;    font_tbl[0x54].font_y4 = 0x04;    font_tbl[0x64].font_y4 = 0x11;    font_tbl[0x74].font_y4 = 0x08;    font_tbl[0xA4].font_y4 = 0x01;    font_tbl[0xB4].font_y4 = 0x10;    font_tbl[0xC4].font_y4 = 0x1c;    font_tbl[0xD4].font_y4 = 0x04;    font_tbl[0xE4].font_y4 = 0x15;    font_tbl[0xF4].font_y4 = 0x11;
    font_tbl[0x24].font_y5 = 0x1e;    font_tbl[0x34].font_y5 = 0x02;    font_tbl[0x44].font_y5 = 0x12;    font_tbl[0x54].font_y5 = 0x04;    font_tbl[0x64].font_y5 = 0x11;    font_tbl[0x74].font_y5 = 0x09;    font_tbl[0xA4].font_y5 = 0x01;    font_tbl[0xB4].font_y5 = 0x10;    font_tbl[0xC4].font_y5 = 0x12;    font_tbl[0xD4].font_y5 = 0x08;    font_tbl[0xE4].font_y5 = 0x15;    font_tbl[0xF4].font_y5 = 0x1f;    
    font_tbl[0x24].font_y6 = 0x04;    font_tbl[0x34].font_y6 = 0x02;    font_tbl[0x44].font_y6 = 0x1c;    font_tbl[0x54].font_y6 = 0x04;    font_tbl[0x64].font_y6 = 0x0f;    font_tbl[0x74].font_y6 = 0x06;    font_tbl[0xA4].font_y6 = 0x1e;    font_tbl[0xB4].font_y6 = 0x10;    font_tbl[0xC4].font_y6 = 0x1c;    font_tbl[0xD4].font_y6 = 0x10;    font_tbl[0xE4].font_y6 = 0x0e;    font_tbl[0xF4].font_y6 = 0x11;    
    font_tbl[0x24].font_y7 = 0x00;    font_tbl[0x34].font_y7 = 0x00;    font_tbl[0x44].font_y7 = 0x00;    font_tbl[0x54].font_y7 = 0x00;    font_tbl[0x64].font_y7 = 0x00;    font_tbl[0x74].font_y7 = 0x00;    font_tbl[0xA4].font_y7 = 0x00;    font_tbl[0xB4].font_y7 = 0x00;    font_tbl[0xC4].font_y7 = 0x00;    font_tbl[0xD4].font_y7 = 0x00;    font_tbl[0xE4].font_y7 = 0x04;    font_tbl[0xF4].font_y7 = 0x00;        

    // %                              // 5                              // E                              // U                              // e                              // u                                                  
    font_tbl[0x25].font_y0 = 0x18;    font_tbl[0x35].font_y0 = 0x1f;    font_tbl[0x45].font_y0 = 0x1f;    font_tbl[0x55].font_y0 = 0x11;    font_tbl[0x65].font_y0 = 0x00;    font_tbl[0x75].font_y0 = 0x00;    font_tbl[0xA5].font_y0 = 0x11;    font_tbl[0xB5].font_y0 = 0x0a;    font_tbl[0xC5].font_y0 = 0x00;    font_tbl[0xD5].font_y0 = 0x00;    font_tbl[0xE5].font_y0 = 0x00;    font_tbl[0xF5].font_y0 = 0x18;
    font_tbl[0x25].font_y1 = 0x19;    font_tbl[0x35].font_y1 = 0x10;    font_tbl[0x45].font_y1 = 0x10;    font_tbl[0x55].font_y1 = 0x11;    font_tbl[0x65].font_y1 = 0x00;    font_tbl[0x75].font_y1 = 0x00;    font_tbl[0xA5].font_y1 = 0x11;    font_tbl[0xB5].font_y1 = 0x00;    font_tbl[0xC5].font_y1 = 0x00;    font_tbl[0xD5].font_y1 = 0x1b;    font_tbl[0xE5].font_y1 = 0x00;    font_tbl[0xF5].font_y1 = 0x00;
    font_tbl[0x25].font_y2 = 0x02;    font_tbl[0x35].font_y2 = 0x1e;    font_tbl[0x45].font_y2 = 0x10;    font_tbl[0x55].font_y2 = 0x11;    font_tbl[0x65].font_y2 = 0x0e;    font_tbl[0x75].font_y2 = 0x11;    font_tbl[0xA5].font_y2 = 0x13;    font_tbl[0xB5].font_y2 = 0x0e;    font_tbl[0xC5].font_y2 = 0x0e;    font_tbl[0xD5].font_y2 = 0x0e;    font_tbl[0xE5].font_y2 = 0x11;    font_tbl[0xF5].font_y2 = 0x03;
    font_tbl[0x25].font_y3 = 0x04;    font_tbl[0x35].font_y3 = 0x01;    font_tbl[0x45].font_y3 = 0x1e;    font_tbl[0x55].font_y3 = 0x11;    font_tbl[0x65].font_y3 = 0x11;    font_tbl[0x75].font_y3 = 0x11;    font_tbl[0xA5].font_y3 = 0x15;    font_tbl[0xB5].font_y3 = 0x11;    font_tbl[0xC5].font_y3 = 0x11;    font_tbl[0xD5].font_y3 = 0x04;    font_tbl[0xE5].font_y3 = 0x11;    font_tbl[0xF5].font_y3 = 0x00;
    font_tbl[0x25].font_y4 = 0x08;    font_tbl[0x35].font_y4 = 0x01;    font_tbl[0x45].font_y4 = 0x10;    font_tbl[0x55].font_y4 = 0x11;    font_tbl[0x65].font_y4 = 0x1f;    font_tbl[0x75].font_y4 = 0x11;    font_tbl[0xA5].font_y4 = 0x19;    font_tbl[0xB5].font_y4 = 0x1f;    font_tbl[0xC5].font_y4 = 0x07;    font_tbl[0xD5].font_y4 = 0x0e;    font_tbl[0xE5].font_y4 = 0x11;    font_tbl[0xF5].font_y4 = 0x0c;
    font_tbl[0x25].font_y5 = 0x13;    font_tbl[0x35].font_y5 = 0x11;    font_tbl[0x45].font_y5 = 0x10;    font_tbl[0x55].font_y5 = 0x11;    font_tbl[0x65].font_y5 = 0x10;    font_tbl[0x75].font_y5 = 0x13;    font_tbl[0xA5].font_y5 = 0x11;    font_tbl[0xB5].font_y5 = 0x10;    font_tbl[0xC5].font_y5 = 0x11;    font_tbl[0xD5].font_y5 = 0x1b;    font_tbl[0xE5].font_y5 = 0x11;    font_tbl[0xF5].font_y5 = 0x00;    
    font_tbl[0x25].font_y6 = 0x03;    font_tbl[0x35].font_y6 = 0x0e;    font_tbl[0x45].font_y6 = 0x1f;    font_tbl[0x55].font_y6 = 0x0e;    font_tbl[0x65].font_y6 = 0x0e;    font_tbl[0x75].font_y6 = 0x0d;    font_tbl[0xA5].font_y6 = 0x11;    font_tbl[0xB5].font_y6 = 0x0e;    font_tbl[0xC5].font_y6 = 0x0e;    font_tbl[0xD5].font_y6 = 0x00;    font_tbl[0xE5].font_y6 = 0x1f;    font_tbl[0xF5].font_y6 = 0x00;    
    font_tbl[0x25].font_y7 = 0x00;    font_tbl[0x35].font_y7 = 0x00;    font_tbl[0x45].font_y7 = 0x00;    font_tbl[0x55].font_y7 = 0x00;    font_tbl[0x65].font_y7 = 0x00;    font_tbl[0x75].font_y7 = 0x00;    font_tbl[0xA5].font_y7 = 0x00;    font_tbl[0xB5].font_y7 = 0x00;    font_tbl[0xC5].font_y7 = 0x00;    font_tbl[0xD5].font_y7 = 0x00;    font_tbl[0xE5].font_y7 = 0x01;    font_tbl[0xF5].font_y7 = 0x00;        

    // &                              // 6                              // F                              // V                              // f                              // v                                                  
    font_tbl[0x26].font_y0 = 0x0c;    font_tbl[0x36].font_y0 = 0x06;    font_tbl[0x46].font_y0 = 0x1f;    font_tbl[0x56].font_y0 = 0x11;    font_tbl[0x66].font_y0 = 0x06;    font_tbl[0x76].font_y0 = 0x00;    font_tbl[0xA6].font_y0 = 0x0a;    font_tbl[0xB6].font_y0 = 0x00;    font_tbl[0xC6].font_y0 = 0x00;    font_tbl[0xD6].font_y0 = 0x14;    font_tbl[0xE6].font_y0 = 0x00;    font_tbl[0xF6].font_y0 = 0x08;
    font_tbl[0x26].font_y1 = 0x12;    font_tbl[0x36].font_y1 = 0x08;    font_tbl[0x46].font_y1 = 0x10;    font_tbl[0x56].font_y1 = 0x11;    font_tbl[0x66].font_y1 = 0x09;    font_tbl[0x76].font_y1 = 0x00;    font_tbl[0xA6].font_y1 = 0x04;    font_tbl[0xB6].font_y1 = 0x00;    font_tbl[0xC6].font_y1 = 0x00;    font_tbl[0xD6].font_y1 = 0x08;    font_tbl[0xE6].font_y1 = 0x00;    font_tbl[0xF6].font_y1 = 0x14;
    font_tbl[0x26].font_y2 = 0x14;    font_tbl[0x36].font_y2 = 0x10;    font_tbl[0x46].font_y2 = 0x10;    font_tbl[0x56].font_y2 = 0x11;    font_tbl[0x66].font_y2 = 0x08;    font_tbl[0x76].font_y2 = 0x11;    font_tbl[0xA6].font_y2 = 0x11;    font_tbl[0xB6].font_y2 = 0x15;    font_tbl[0xC6].font_y2 = 0x12;    font_tbl[0xD6].font_y2 = 0x15;    font_tbl[0xE6].font_y2 = 0x15;    font_tbl[0xF6].font_y2 = 0x1c;
    font_tbl[0x26].font_y3 = 0x08;    font_tbl[0x36].font_y3 = 0x1e;    font_tbl[0x46].font_y3 = 0x1e;    font_tbl[0x56].font_y3 = 0x11;    font_tbl[0x66].font_y3 = 0x1c;    font_tbl[0x76].font_y3 = 0x11;    font_tbl[0xA6].font_y3 = 0x13;    font_tbl[0xB6].font_y3 = 0x15;    font_tbl[0xC6].font_y3 = 0x15;    font_tbl[0xD6].font_y3 = 0x02;    font_tbl[0xE6].font_y3 = 0x15;    font_tbl[0xF6].font_y3 = 0x15;
    font_tbl[0x26].font_y4 = 0x15;    font_tbl[0x36].font_y4 = 0x11;    font_tbl[0x46].font_y4 = 0x10;    font_tbl[0x56].font_y4 = 0x11;    font_tbl[0x66].font_y4 = 0x08;    font_tbl[0x76].font_y4 = 0x11;    font_tbl[0xA6].font_y4 = 0x15;    font_tbl[0xB6].font_y4 = 0x0e;    font_tbl[0xC6].font_y4 = 0x1d;    font_tbl[0xD6].font_y4 = 0x04;    font_tbl[0xE6].font_y4 = 0x15;    font_tbl[0xF6].font_y4 = 0x01;
    font_tbl[0x26].font_y5 = 0x12;    font_tbl[0x36].font_y5 = 0x11;    font_tbl[0x46].font_y5 = 0x10;    font_tbl[0x56].font_y5 = 0x0a;    font_tbl[0x66].font_y5 = 0x08;    font_tbl[0x76].font_y5 = 0x0a;    font_tbl[0xA6].font_y5 = 0x19;    font_tbl[0xB6].font_y5 = 0x15;    font_tbl[0xC6].font_y5 = 0x15;    font_tbl[0xD6].font_y5 = 0x08;    font_tbl[0xE6].font_y5 = 0x15;    font_tbl[0xF6].font_y5 = 0x05;    
    font_tbl[0x26].font_y6 = 0x0d;    font_tbl[0x36].font_y6 = 0x0e;    font_tbl[0x46].font_y6 = 0x10;    font_tbl[0x56].font_y6 = 0x04;    font_tbl[0x66].font_y6 = 0x08;    font_tbl[0x76].font_y6 = 0x04;    font_tbl[0xA6].font_y6 = 0x11;    font_tbl[0xB6].font_y6 = 0x15;    font_tbl[0xC6].font_y6 = 0x12;    font_tbl[0xD6].font_y6 = 0x10;    font_tbl[0xE6].font_y6 = 0x1f;    font_tbl[0xF6].font_y6 = 0x1f;    
    font_tbl[0x26].font_y7 = 0x00;    font_tbl[0x36].font_y7 = 0x00;    font_tbl[0x46].font_y7 = 0x00;    font_tbl[0x56].font_y7 = 0x00;    font_tbl[0x66].font_y7 = 0x00;    font_tbl[0x76].font_y7 = 0x00;    font_tbl[0xA6].font_y7 = 0x00;    font_tbl[0xB6].font_y7 = 0x00;    font_tbl[0xC6].font_y7 = 0x00;    font_tbl[0xD6].font_y7 = 0x00;    font_tbl[0xE6].font_y7 = 0x01;    font_tbl[0xF6].font_y7 = 0x04;        
																																																										
    // '                              // 7                              // G                              // W                              // g                              // w                                                  
    font_tbl[0x27].font_y0 = 0x0c;    font_tbl[0x37].font_y0 = 0x1f;    font_tbl[0x47].font_y0 = 0x0e;    font_tbl[0x57].font_y0 = 0x11;    font_tbl[0x67].font_y0 = 0x00;    font_tbl[0x77].font_y0 = 0x00;    font_tbl[0xA7].font_y0 = 0x0f;    font_tbl[0xB7].font_y0 = 0x00;    font_tbl[0xC7].font_y0 = 0x00;    font_tbl[0xD7].font_y0 = 0x0e;    font_tbl[0xE7].font_y0 = 0x02;    font_tbl[0xF7].font_y0 = 0x08;
    font_tbl[0x27].font_y1 = 0x04;    font_tbl[0x37].font_y1 = 0x01;    font_tbl[0x47].font_y1 = 0x11;    font_tbl[0x57].font_y1 = 0x11;    font_tbl[0x67].font_y1 = 0x0f;    font_tbl[0x77].font_y1 = 0x00;    font_tbl[0xA7].font_y1 = 0x05;    font_tbl[0xB7].font_y1 = 0x00;    font_tbl[0xC7].font_y1 = 0x00;    font_tbl[0xD7].font_y1 = 0x04;    font_tbl[0xE7].font_y1 = 0x04;    font_tbl[0xF7].font_y1 = 0x0e;
    font_tbl[0x27].font_y2 = 0x08;    font_tbl[0x37].font_y2 = 0x02;    font_tbl[0x47].font_y2 = 0x10;    font_tbl[0x57].font_y2 = 0x11;    font_tbl[0x67].font_y2 = 0x11;    font_tbl[0x77].font_y2 = 0x11;    font_tbl[0xA7].font_y2 = 0x05;    font_tbl[0xB7].font_y2 = 0x1e;    font_tbl[0xC7].font_y2 = 0x0f;    font_tbl[0xD7].font_y2 = 0x04;    font_tbl[0xE7].font_y2 = 0x00;    font_tbl[0xF7].font_y2 = 0x08;
    font_tbl[0x27].font_y3 = 0x00;    font_tbl[0x37].font_y3 = 0x04;    font_tbl[0x47].font_y3 = 0x17;    font_tbl[0x57].font_y3 = 0x15;    font_tbl[0x67].font_y3 = 0x11;    font_tbl[0x77].font_y3 = 0x11;    font_tbl[0xA7].font_y3 = 0x05;    font_tbl[0xB7].font_y3 = 0x01;    font_tbl[0xC7].font_y3 = 0x11;    font_tbl[0xD7].font_y3 = 0x04;    font_tbl[0xE7].font_y3 = 0x00;    font_tbl[0xF7].font_y3 = 0x0f;
    font_tbl[0x27].font_y4 = 0x00;    font_tbl[0x37].font_y4 = 0x08;    font_tbl[0x47].font_y4 = 0x11;    font_tbl[0x57].font_y4 = 0x15;    font_tbl[0x67].font_y4 = 0x0f;    font_tbl[0x77].font_y4 = 0x15;    font_tbl[0xA7].font_y4 = 0x05;    font_tbl[0xB7].font_y4 = 0x0e;    font_tbl[0xC7].font_y4 = 0x0f;    font_tbl[0xD7].font_y4 = 0x04;    font_tbl[0xE7].font_y4 = 0x00;    font_tbl[0xF7].font_y4 = 0x08;
    font_tbl[0x27].font_y5 = 0x00;    font_tbl[0x37].font_y5 = 0x08;    font_tbl[0x47].font_y5 = 0x11;    font_tbl[0x57].font_y5 = 0x15;    font_tbl[0x67].font_y5 = 0x01;    font_tbl[0x77].font_y5 = 0x15;    font_tbl[0xA7].font_y5 = 0x15;    font_tbl[0xB7].font_y5 = 0x01;    font_tbl[0xC7].font_y5 = 0x05;    font_tbl[0xD7].font_y5 = 0x0e;    font_tbl[0xE7].font_y5 = 0x00;    font_tbl[0xF7].font_y5 = 0x0e;    
    font_tbl[0x27].font_y6 = 0x00;    font_tbl[0x37].font_y6 = 0x08;    font_tbl[0x47].font_y6 = 0x0f;    font_tbl[0x57].font_y6 = 0x0a;    font_tbl[0x67].font_y6 = 0x0e;    font_tbl[0x77].font_y6 = 0x0a;    font_tbl[0xA7].font_y6 = 0x09;    font_tbl[0xB7].font_y6 = 0x1e;    font_tbl[0xC7].font_y6 = 0x09;    font_tbl[0xD7].font_y6 = 0x00;    font_tbl[0xE7].font_y6 = 0x00;    font_tbl[0xF7].font_y6 = 0x08;    
    font_tbl[0x27].font_y7 = 0x00;    font_tbl[0x37].font_y7 = 0x00;    font_tbl[0x47].font_y7 = 0x00;    font_tbl[0x57].font_y7 = 0x00;    font_tbl[0x67].font_y7 = 0x00;    font_tbl[0x77].font_y7 = 0x00;    font_tbl[0xA7].font_y7 = 0x00;    font_tbl[0xB7].font_y7 = 0x00;    font_tbl[0xC7].font_y7 = 0x00;    font_tbl[0xD7].font_y7 = 0x00;    font_tbl[0xE7].font_y7 = 0x00;    font_tbl[0xF7].font_y7 = 0x1c;        
																																																										
    // (                              // 8                              // H                              // X                              // h                              // x                                                  
    font_tbl[0x28].font_y0 = 0x02;    font_tbl[0x38].font_y0 = 0x0e;    font_tbl[0x48].font_y0 = 0x11;    font_tbl[0x58].font_y0 = 0x11;    font_tbl[0x68].font_y0 = 0x10;    font_tbl[0x78].font_y0 = 0x00;    font_tbl[0xA8].font_y0 = 0x1f;    font_tbl[0xB8].font_y0 = 0x00;    font_tbl[0xC8].font_y0 = 0x00;    font_tbl[0xD8].font_y0 = 0x1f;    font_tbl[0xE8].font_y0 = 0x0a;    font_tbl[0xF8].font_y0 = 0x04;
    font_tbl[0x28].font_y1 = 0x04;    font_tbl[0x38].font_y1 = 0x11;    font_tbl[0x48].font_y1 = 0x11;    font_tbl[0x58].font_y1 = 0x11;    font_tbl[0x68].font_y1 = 0x10;    font_tbl[0x78].font_y1 = 0x00;    font_tbl[0xA8].font_y1 = 0x11;    font_tbl[0xB8].font_y1 = 0x00;    font_tbl[0xC8].font_y1 = 0x00;    font_tbl[0xD8].font_y1 = 0x0a;    font_tbl[0xE8].font_y1 = 0x00;    font_tbl[0xF8].font_y1 = 0x0e;
    font_tbl[0x28].font_y2 = 0x08;    font_tbl[0x38].font_y2 = 0x11;    font_tbl[0x48].font_y2 = 0x11;    font_tbl[0x58].font_y2 = 0x0a;    font_tbl[0x68].font_y2 = 0x16;    font_tbl[0x78].font_y2 = 0x11;    font_tbl[0xA8].font_y2 = 0x11;    font_tbl[0xB8].font_y2 = 0x11;    font_tbl[0xC8].font_y2 = 0x04;    font_tbl[0xD8].font_y2 = 0x0a;    font_tbl[0xE8].font_y2 = 0x00;    font_tbl[0xF8].font_y2 = 0x04;
    font_tbl[0x28].font_y3 = 0x08;    font_tbl[0x38].font_y3 = 0x0e;    font_tbl[0x48].font_y3 = 0x1f;    font_tbl[0x58].font_y3 = 0x04;    font_tbl[0x68].font_y3 = 0x19;    font_tbl[0x78].font_y3 = 0x0a;    font_tbl[0xA8].font_y3 = 0x11;    font_tbl[0xB8].font_y3 = 0x13;    font_tbl[0xC8].font_y3 = 0x09;    font_tbl[0xD8].font_y3 = 0x0a;    font_tbl[0xE8].font_y3 = 0x00;    font_tbl[0xF8].font_y3 = 0x1f;
    font_tbl[0x28].font_y4 = 0x08;    font_tbl[0x38].font_y4 = 0x11;    font_tbl[0x48].font_y4 = 0x11;    font_tbl[0x58].font_y4 = 0x0a;    font_tbl[0x68].font_y4 = 0x11;    font_tbl[0x78].font_y4 = 0x04;    font_tbl[0xA8].font_y4 = 0x11;    font_tbl[0xB8].font_y4 = 0x15;    font_tbl[0xC8].font_y4 = 0x12;    font_tbl[0xD8].font_y4 = 0x0a;    font_tbl[0xE8].font_y4 = 0x00;    font_tbl[0xF8].font_y4 = 0x04;
    font_tbl[0x28].font_y5 = 0x04;    font_tbl[0x38].font_y5 = 0x11;    font_tbl[0x48].font_y5 = 0x11;    font_tbl[0x58].font_y5 = 0x11;    font_tbl[0x68].font_y5 = 0x11;    font_tbl[0x78].font_y5 = 0x0a;    font_tbl[0xA8].font_y5 = 0x11;    font_tbl[0xB8].font_y5 = 0x19;    font_tbl[0xC8].font_y5 = 0x09;    font_tbl[0xD8].font_y5 = 0x1f;    font_tbl[0xE8].font_y5 = 0x00;    font_tbl[0xF8].font_y5 = 0x0e;    
    font_tbl[0x28].font_y6 = 0x02;    font_tbl[0x38].font_y6 = 0x0e;    font_tbl[0x48].font_y6 = 0x11;    font_tbl[0x58].font_y6 = 0x11;    font_tbl[0x68].font_y6 = 0x11;    font_tbl[0x78].font_y6 = 0x11;    font_tbl[0xA8].font_y6 = 0x11;    font_tbl[0xB8].font_y6 = 0x11;    font_tbl[0xC8].font_y6 = 0x04;    font_tbl[0xD8].font_y6 = 0x00;    font_tbl[0xE8].font_y6 = 0x00;    font_tbl[0xF8].font_y6 = 0x04;    
    font_tbl[0x28].font_y7 = 0x00;    font_tbl[0x38].font_y7 = 0x00;    font_tbl[0x48].font_y7 = 0x00;    font_tbl[0x58].font_y7 = 0x00;    font_tbl[0x68].font_y7 = 0x00;    font_tbl[0x78].font_y7 = 0x00;    font_tbl[0xA8].font_y7 = 0x00;    font_tbl[0xB8].font_y7 = 0x00;    font_tbl[0xC8].font_y7 = 0x00;    font_tbl[0xD8].font_y7 = 0x00;    font_tbl[0xE8].font_y7 = 0x00;    font_tbl[0xF8].font_y7 = 0x0e;        

    // )                              // 9                              // I                              // Y                              // i                              // y                                                  
    font_tbl[0x29].font_y0 = 0x08;    font_tbl[0x39].font_y0 = 0x0e;    font_tbl[0x49].font_y0 = 0x0e;    font_tbl[0x59].font_y0 = 0x11;    font_tbl[0x69].font_y0 = 0x04;    font_tbl[0x79].font_y0 = 0x00;    font_tbl[0xA9].font_y0 = 0x11;    font_tbl[0xB9].font_y0 = 0x00;    font_tbl[0xC9].font_y0 = 0x00;    font_tbl[0xD9].font_y0 = 0x04;    font_tbl[0xE9].font_y0 = 0x09;    font_tbl[0xF9].font_y0 = 0x00;
    font_tbl[0x29].font_y1 = 0x04;    font_tbl[0x39].font_y1 = 0x11;    font_tbl[0x49].font_y1 = 0x04;    font_tbl[0x59].font_y1 = 0x11;    font_tbl[0x69].font_y1 = 0x00;    font_tbl[0x79].font_y1 = 0x00;    font_tbl[0xA9].font_y1 = 0x11;    font_tbl[0xB9].font_y1 = 0x0a;    font_tbl[0xC9].font_y1 = 0x00;    font_tbl[0xD9].font_y1 = 0x0e;    font_tbl[0xE9].font_y1 = 0x16;    font_tbl[0xF9].font_y1 = 0x0d;
    font_tbl[0x29].font_y2 = 0x02;    font_tbl[0x39].font_y2 = 0x11;    font_tbl[0x49].font_y2 = 0x04;    font_tbl[0x59].font_y2 = 0x11;    font_tbl[0x69].font_y2 = 0x0c;    font_tbl[0x79].font_y2 = 0x11;    font_tbl[0xA9].font_y2 = 0x11;    font_tbl[0xB9].font_y2 = 0x04;    font_tbl[0xC9].font_y2 = 0x04;    font_tbl[0xD9].font_y2 = 0x15;    font_tbl[0xE9].font_y2 = 0x00;    font_tbl[0xF9].font_y2 = 0x00;
    font_tbl[0x29].font_y3 = 0x02;    font_tbl[0x39].font_y3 = 0x0f;    font_tbl[0x49].font_y3 = 0x04;    font_tbl[0x59].font_y3 = 0x0a;    font_tbl[0x69].font_y3 = 0x04;    font_tbl[0x79].font_y3 = 0x11;    font_tbl[0xA9].font_y3 = 0x0a;    font_tbl[0xB9].font_y3 = 0x11;    font_tbl[0xC9].font_y3 = 0x12;    font_tbl[0xD9].font_y3 = 0x04;    font_tbl[0xE9].font_y3 = 0x00;    font_tbl[0xF9].font_y3 = 0x1d;
    font_tbl[0x29].font_y4 = 0x02;    font_tbl[0x39].font_y4 = 0x01;    font_tbl[0x49].font_y4 = 0x04;    font_tbl[0x59].font_y4 = 0x04;    font_tbl[0x69].font_y4 = 0x04;    font_tbl[0x79].font_y4 = 0x0f;    font_tbl[0xA9].font_y4 = 0x04;    font_tbl[0xB9].font_y4 = 0x13;    font_tbl[0xC9].font_y4 = 0x09;    font_tbl[0xD9].font_y4 = 0x04;    font_tbl[0xE9].font_y4 = 0x00;    font_tbl[0xF9].font_y4 = 0x00;
    font_tbl[0x29].font_y5 = 0x04;    font_tbl[0x39].font_y5 = 0x02;    font_tbl[0x49].font_y5 = 0x04;    font_tbl[0x59].font_y5 = 0x04;    font_tbl[0x69].font_y5 = 0x04;    font_tbl[0x79].font_y5 = 0x01;    font_tbl[0xA9].font_y5 = 0x08;    font_tbl[0xB9].font_y5 = 0x15;    font_tbl[0xC9].font_y5 = 0x12;    font_tbl[0xD9].font_y5 = 0x04;    font_tbl[0xE9].font_y5 = 0x00;    font_tbl[0xF9].font_y5 = 0x0d;    
    font_tbl[0x29].font_y6 = 0x08;    font_tbl[0x39].font_y6 = 0x0c;    font_tbl[0x49].font_y6 = 0x0e;    font_tbl[0x59].font_y6 = 0x04;    font_tbl[0x69].font_y6 = 0x0e;    font_tbl[0x79].font_y6 = 0x0e;    font_tbl[0xA9].font_y6 = 0x10;    font_tbl[0xB9].font_y6 = 0x19;    font_tbl[0xC9].font_y6 = 0x04;    font_tbl[0xD9].font_y6 = 0x04;    font_tbl[0xE9].font_y6 = 0x00;    font_tbl[0xF9].font_y6 = 0x02;    
    font_tbl[0x29].font_y7 = 0x00;    font_tbl[0x39].font_y7 = 0x00;    font_tbl[0x49].font_y7 = 0x00;    font_tbl[0x59].font_y7 = 0x00;    font_tbl[0x69].font_y7 = 0x00;    font_tbl[0x79].font_y7 = 0x00;    font_tbl[0xA9].font_y7 = 0x00;    font_tbl[0xB9].font_y7 = 0x00;    font_tbl[0xC9].font_y7 = 0x00;    font_tbl[0xD9].font_y7 = 0x00;    font_tbl[0xE9].font_y7 = 0x00;    font_tbl[0xF9].font_y7 = 0x07;        

    // *                              // :                              // J                              // Z                              // j                              // z                                                  
    font_tbl[0x2A].font_y0 = 0x00;    font_tbl[0x3A].font_y0 = 0x00;    font_tbl[0x4A].font_y0 = 0x07;    font_tbl[0x5A].font_y0 = 0x1f;    font_tbl[0x6A].font_y0 = 0x02;    font_tbl[0x7A].font_y0 = 0x00;    font_tbl[0xAA].font_y0 = 0x04;    font_tbl[0xBA].font_y0 = 0x00;    font_tbl[0xCA].font_y0 = 0x00;    font_tbl[0xDA].font_y0 = 0x04;    font_tbl[0xEA].font_y0 = 0x02;    font_tbl[0xFA].font_y0 = 0x02;
    font_tbl[0x2A].font_y1 = 0x04;    font_tbl[0x3A].font_y1 = 0x0c;    font_tbl[0x4A].font_y1 = 0x02;    font_tbl[0x5A].font_y1 = 0x01;    font_tbl[0x6A].font_y1 = 0x00;    font_tbl[0x7A].font_y1 = 0x00;    font_tbl[0xAA].font_y1 = 0x0e;    font_tbl[0xBA].font_y1 = 0x00;    font_tbl[0xCA].font_y1 = 0x00;    font_tbl[0xDA].font_y1 = 0x04;    font_tbl[0xEA].font_y1 = 0x04;    font_tbl[0xFA].font_y1 = 0x0e;
    font_tbl[0x2A].font_y2 = 0x15;    font_tbl[0x3A].font_y2 = 0x0c;    font_tbl[0x4A].font_y2 = 0x02;    font_tbl[0x5A].font_y2 = 0x02;    font_tbl[0x6A].font_y2 = 0x06;    font_tbl[0x7A].font_y2 = 0x1f;    font_tbl[0xAA].font_y2 = 0x15;    font_tbl[0xBA].font_y2 = 0x12;    font_tbl[0xCA].font_y2 = 0x00;    font_tbl[0xDA].font_y2 = 0x04;    font_tbl[0xEA].font_y2 = 0x0e;    font_tbl[0xFA].font_y2 = 0x02;
    font_tbl[0x2A].font_y3 = 0x0e;    font_tbl[0x3A].font_y3 = 0x00;    font_tbl[0x4A].font_y3 = 0x02;    font_tbl[0x5A].font_y3 = 0x04;    font_tbl[0x6A].font_y3 = 0x02;    font_tbl[0x7A].font_y3 = 0x02;    font_tbl[0xAA].font_y3 = 0x15;    font_tbl[0xBA].font_y3 = 0x14;    font_tbl[0xCA].font_y3 = 0x00;    font_tbl[0xDA].font_y3 = 0x04;    font_tbl[0xEA].font_y3 = 0x11;    font_tbl[0xFA].font_y3 = 0x1e;
    font_tbl[0x2A].font_y4 = 0x15;    font_tbl[0x3A].font_y4 = 0x0c;    font_tbl[0x4A].font_y4 = 0x02;    font_tbl[0x5A].font_y4 = 0x08;    font_tbl[0x6A].font_y4 = 0x02;    font_tbl[0x7A].font_y4 = 0x04;    font_tbl[0xAA].font_y4 = 0x15;    font_tbl[0xBA].font_y4 = 0x18;    font_tbl[0xCA].font_y4 = 0x09;    font_tbl[0xDA].font_y4 = 0x15;    font_tbl[0xEA].font_y4 = 0x1f;    font_tbl[0xFA].font_y4 = 0x02;
    font_tbl[0x2A].font_y5 = 0x04;    font_tbl[0x3A].font_y5 = 0x0c;    font_tbl[0x4A].font_y5 = 0x12;    font_tbl[0x5A].font_y5 = 0x10;    font_tbl[0x6A].font_y5 = 0x12;    font_tbl[0x7A].font_y5 = 0x08;    font_tbl[0xAA].font_y5 = 0x0e;    font_tbl[0xBA].font_y5 = 0x14;    font_tbl[0xCA].font_y5 = 0x12;    font_tbl[0xDA].font_y5 = 0x0e;    font_tbl[0xEA].font_y5 = 0x10;    font_tbl[0xFA].font_y5 = 0x0e;    
    font_tbl[0x2A].font_y6 = 0x00;    font_tbl[0x3A].font_y6 = 0x00;    font_tbl[0x4A].font_y6 = 0x0c;    font_tbl[0x5A].font_y6 = 0x1f;    font_tbl[0x6A].font_y6 = 0x0c;    font_tbl[0x7A].font_y6 = 0x1f;    font_tbl[0xAA].font_y6 = 0x04;    font_tbl[0xBA].font_y6 = 0x12;    font_tbl[0xCA].font_y6 = 0x1b;    font_tbl[0xDA].font_y6 = 0x04;    font_tbl[0xEA].font_y6 = 0x0e;    font_tbl[0xFA].font_y6 = 0x02;    
    font_tbl[0x2A].font_y7 = 0x00;    font_tbl[0x3A].font_y7 = 0x00;    font_tbl[0x4A].font_y7 = 0x00;    font_tbl[0x5A].font_y7 = 0x00;    font_tbl[0x6A].font_y7 = 0x00;    font_tbl[0x7A].font_y7 = 0x00;    font_tbl[0xAA].font_y7 = 0x00;    font_tbl[0xBA].font_y7 = 0x00;    font_tbl[0xCA].font_y7 = 0x00;    font_tbl[0xDA].font_y7 = 0x00;    font_tbl[0xEA].font_y7 = 0x00;    font_tbl[0xFA].font_y7 = 0x07;        
																																																										
    // +                              // ;                              // K                              // [                              // k                              // {                                                  
    font_tbl[0x2B].font_y0 = 0x00;    font_tbl[0x3B].font_y0 = 0x00;    font_tbl[0x4B].font_y0 = 0x11;    font_tbl[0x5B].font_y0 = 0x0e;    font_tbl[0x6B].font_y0 = 0x10;    font_tbl[0x7B].font_y0 = 0x02;    font_tbl[0xAB].font_y0 = 0x11;    font_tbl[0xBB].font_y0 = 0x00;    font_tbl[0xCB].font_y0 = 0x1b;    font_tbl[0xDB].font_y0 = 0x10;    font_tbl[0xEB].font_y0 = 0x00;    font_tbl[0xFB].font_y0 = 0x04;
    font_tbl[0x2B].font_y1 = 0x04;    font_tbl[0x3B].font_y1 = 0x0c;    font_tbl[0x4B].font_y1 = 0x12;    font_tbl[0x5B].font_y1 = 0x08;    font_tbl[0x6B].font_y1 = 0x10;    font_tbl[0x7B].font_y1 = 0x04;    font_tbl[0xAB].font_y1 = 0x11;    font_tbl[0xBB].font_y1 = 0x00;    font_tbl[0xCB].font_y1 = 0x09;    font_tbl[0xDB].font_y1 = 0x12;    font_tbl[0xEB].font_y1 = 0x00;    font_tbl[0xFB].font_y1 = 0x0e;
    font_tbl[0x2B].font_y2 = 0x04;    font_tbl[0x3B].font_y2 = 0x0c;    font_tbl[0x4B].font_y2 = 0x14;    font_tbl[0x5B].font_y2 = 0x08;    font_tbl[0x6B].font_y2 = 0x12;    font_tbl[0x7B].font_y2 = 0x04;    font_tbl[0xAB].font_y2 = 0x11;    font_tbl[0xBB].font_y2 = 0x0f;    font_tbl[0xCB].font_y2 = 0x12;    font_tbl[0xDB].font_y2 = 0x16;    font_tbl[0xEB].font_y2 = 0x0e;    font_tbl[0xFB].font_y2 = 0x04;
    font_tbl[0x2B].font_y3 = 0x1f;    font_tbl[0x3B].font_y3 = 0x00;    font_tbl[0x4B].font_y3 = 0x18;    font_tbl[0x5B].font_y3 = 0x08;    font_tbl[0x6B].font_y3 = 0x14;    font_tbl[0x7B].font_y3 = 0x08;    font_tbl[0xAB].font_y3 = 0x0f;    font_tbl[0xBB].font_y3 = 0x05;    font_tbl[0xCB].font_y3 = 0x00;    font_tbl[0xDB].font_y3 = 0x1f;    font_tbl[0xEB].font_y3 = 0x10;    font_tbl[0xFB].font_y3 = 0x1f;
    font_tbl[0x2B].font_y4 = 0x04;    font_tbl[0x3B].font_y4 = 0x0c;    font_tbl[0x4B].font_y4 = 0x14;    font_tbl[0x5B].font_y4 = 0x08;    font_tbl[0x6B].font_y4 = 0x18;    font_tbl[0x7B].font_y4 = 0x04;    font_tbl[0xAB].font_y4 = 0x01;    font_tbl[0xBB].font_y4 = 0x05;    font_tbl[0xCB].font_y4 = 0x00;    font_tbl[0xDB].font_y4 = 0x16;    font_tbl[0xEB].font_y4 = 0x11;    font_tbl[0xFB].font_y4 = 0x04;
    font_tbl[0x2B].font_y5 = 0x04;    font_tbl[0x3B].font_y5 = 0x04;    font_tbl[0x4B].font_y5 = 0x12;    font_tbl[0x5B].font_y5 = 0x08;    font_tbl[0x6B].font_y5 = 0x14;    font_tbl[0x7B].font_y5 = 0x04;    font_tbl[0xAB].font_y5 = 0x01;    font_tbl[0xBB].font_y5 = 0x15;    font_tbl[0xCB].font_y5 = 0x00;    font_tbl[0xDB].font_y5 = 0x12;    font_tbl[0xEB].font_y5 = 0x0e;    font_tbl[0xFB].font_y5 = 0x0e;    
    font_tbl[0x2B].font_y6 = 0x00;    font_tbl[0x3B].font_y6 = 0x08;    font_tbl[0x4B].font_y6 = 0x11;    font_tbl[0x5B].font_y6 = 0x0e;    font_tbl[0x6B].font_y6 = 0x12;    font_tbl[0x7B].font_y6 = 0x02;    font_tbl[0xAB].font_y6 = 0x01;    font_tbl[0xBB].font_y6 = 0x09;    font_tbl[0xCB].font_y6 = 0x00;    font_tbl[0xDB].font_y6 = 0x10;    font_tbl[0xEB].font_y6 = 0x04;    font_tbl[0xFB].font_y6 = 0x04;    
    font_tbl[0x2B].font_y7 = 0x00;    font_tbl[0x3B].font_y7 = 0x00;    font_tbl[0x4B].font_y7 = 0x00;    font_tbl[0x5B].font_y7 = 0x00;    font_tbl[0x6B].font_y7 = 0x00;    font_tbl[0x7B].font_y7 = 0x00;    font_tbl[0xAB].font_y7 = 0x00;    font_tbl[0xBB].font_y7 = 0x00;    font_tbl[0xCB].font_y7 = 0x00;    font_tbl[0xDB].font_y7 = 0x00;    font_tbl[0xEB].font_y7 = 0x0c;    font_tbl[0xFB].font_y7 = 0x00;        

    // ,                              // <                              // L                              // ¥                              // l                              // |                                                  
    font_tbl[0x2C].font_y0 = 0x00;    font_tbl[0x3C].font_y0 = 0x02;    font_tbl[0x4C].font_y0 = 0x10;    font_tbl[0x5C].font_y0 = 0x11;    font_tbl[0x6C].font_y0 = 0x0c;    font_tbl[0x7C].font_y0 = 0x04;    font_tbl[0xAC].font_y0 = 0x15;    font_tbl[0xBC].font_y0 = 0x00;    font_tbl[0xCC].font_y0 = 0x12;    font_tbl[0xDC].font_y0 = 0x01;    font_tbl[0xEC].font_y0 = 0x0a;    font_tbl[0xFC].font_y0 = 0x02;
    font_tbl[0x2C].font_y1 = 0x00;    font_tbl[0x3C].font_y1 = 0x04;    font_tbl[0x4C].font_y1 = 0x10;    font_tbl[0x5C].font_y1 = 0x0a;    font_tbl[0x6C].font_y1 = 0x04;    font_tbl[0x7C].font_y1 = 0x04;    font_tbl[0xAC].font_y1 = 0x15;    font_tbl[0xBC].font_y1 = 0x00;    font_tbl[0xCC].font_y1 = 0x1a;    font_tbl[0xDC].font_y1 = 0x09;    font_tbl[0xEC].font_y1 = 0x00;    font_tbl[0xFC].font_y1 = 0x12;
    font_tbl[0x2C].font_y2 = 0x00;    font_tbl[0x3C].font_y2 = 0x08;    font_tbl[0x4C].font_y2 = 0x10;    font_tbl[0x5C].font_y2 = 0x1f;    font_tbl[0x6C].font_y2 = 0x04;    font_tbl[0x7C].font_y2 = 0x04;    font_tbl[0xAC].font_y2 = 0x15;    font_tbl[0xBC].font_y2 = 0x11;    font_tbl[0xCC].font_y2 = 0x16;    font_tbl[0xDC].font_y2 = 0x0d;    font_tbl[0xEC].font_y2 = 0x0a;    font_tbl[0xFC].font_y2 = 0x0a;
    font_tbl[0x2C].font_y3 = 0x00;    font_tbl[0x3C].font_y3 = 0x10;    font_tbl[0x4C].font_y3 = 0x10;    font_tbl[0x5C].font_y3 = 0x04;    font_tbl[0x6C].font_y3 = 0x04;    font_tbl[0x7C].font_y3 = 0x04;    font_tbl[0xAC].font_y3 = 0x15;    font_tbl[0xBC].font_y3 = 0x1b;    font_tbl[0xCC].font_y3 = 0x12;    font_tbl[0xDC].font_y3 = 0x1f;    font_tbl[0xEC].font_y3 = 0x0a;    font_tbl[0xFC].font_y3 = 0x06;
    font_tbl[0x2C].font_y4 = 0x0c;    font_tbl[0x3C].font_y4 = 0x08;    font_tbl[0x4C].font_y4 = 0x10;    font_tbl[0x5C].font_y4 = 0x1f;    font_tbl[0x6C].font_y4 = 0x04;    font_tbl[0x7C].font_y4 = 0x04;    font_tbl[0xAC].font_y4 = 0x15;    font_tbl[0xBC].font_y4 = 0x15;    font_tbl[0xCC].font_y4 = 0x07;    font_tbl[0xDC].font_y4 = 0x0d;    font_tbl[0xEC].font_y4 = 0x0a;    font_tbl[0xFC].font_y4 = 0x0a;
    font_tbl[0x2C].font_y5 = 0x04;    font_tbl[0x3C].font_y5 = 0x04;    font_tbl[0x4C].font_y5 = 0x10;    font_tbl[0x5C].font_y5 = 0x04;    font_tbl[0x6C].font_y5 = 0x04;    font_tbl[0x7C].font_y5 = 0x04;    font_tbl[0xAC].font_y5 = 0x15;    font_tbl[0xBC].font_y5 = 0x11;    font_tbl[0xCC].font_y5 = 0x05;    font_tbl[0xDC].font_y5 = 0x09;    font_tbl[0xEC].font_y5 = 0x02;    font_tbl[0xFC].font_y5 = 0x12;    
    font_tbl[0x2C].font_y6 = 0x08;    font_tbl[0x3C].font_y6 = 0x02;    font_tbl[0x4C].font_y6 = 0x1f;    font_tbl[0x5C].font_y6 = 0x04;    font_tbl[0x6C].font_y6 = 0x0e;    font_tbl[0x7C].font_y6 = 0x04;    font_tbl[0xAC].font_y6 = 0x1f;    font_tbl[0xBC].font_y6 = 0x11;    font_tbl[0xCC].font_y6 = 0x07;    font_tbl[0xDC].font_y6 = 0x01;    font_tbl[0xEC].font_y6 = 0x0c;    font_tbl[0xFC].font_y6 = 0x02;    
    font_tbl[0x2C].font_y7 = 0x00;    font_tbl[0x3C].font_y7 = 0x00;    font_tbl[0x4C].font_y7 = 0x00;    font_tbl[0x5C].font_y7 = 0x00;    font_tbl[0x6C].font_y7 = 0x00;    font_tbl[0x7C].font_y7 = 0x00;    font_tbl[0xAC].font_y7 = 0x00;    font_tbl[0xBC].font_y7 = 0x00;    font_tbl[0xCC].font_y7 = 0x00;    font_tbl[0xDC].font_y7 = 0x00;    font_tbl[0xEC].font_y7 = 0x00;    font_tbl[0xFC].font_y7 = 0x07;        

    // -                              // =                              // M                              // ]                              // m                              // }                                                  
    font_tbl[0x2D].font_y0 = 0x00;    font_tbl[0x3D].font_y0 = 0x00;    font_tbl[0x4D].font_y0 = 0x11;    font_tbl[0x5D].font_y0 = 0x0e;    font_tbl[0x6D].font_y0 = 0x00;    font_tbl[0x7D].font_y0 = 0x08;    font_tbl[0xAD].font_y0 = 0x18;    font_tbl[0xBD].font_y0 = 0x00;    font_tbl[0xCD].font_y0 = 0x04;    font_tbl[0xDD].font_y0 = 0x10;    font_tbl[0xED].font_y0 = 0x00;    font_tbl[0xFD].font_y0 = 0x06;
    font_tbl[0x2D].font_y1 = 0x00;    font_tbl[0x3D].font_y1 = 0x00;    font_tbl[0x4D].font_y1 = 0x1b;    font_tbl[0x5D].font_y1 = 0x02;    font_tbl[0x6D].font_y1 = 0x00;    font_tbl[0x7D].font_y1 = 0x04;    font_tbl[0xAD].font_y1 = 0x08;    font_tbl[0xBD].font_y1 = 0x00;    font_tbl[0xCD].font_y1 = 0x00;    font_tbl[0xDD].font_y1 = 0x11;    font_tbl[0xED].font_y1 = 0x04;    font_tbl[0xFD].font_y1 = 0x09;
    font_tbl[0x2D].font_y2 = 0x00;    font_tbl[0x3D].font_y2 = 0x1f;    font_tbl[0x4D].font_y2 = 0x15;    font_tbl[0x5D].font_y2 = 0x02;    font_tbl[0x6D].font_y2 = 0x1a;    font_tbl[0x7D].font_y2 = 0x04;    font_tbl[0xAD].font_y2 = 0x08;    font_tbl[0xBD].font_y2 = 0x11;    font_tbl[0xCD].font_y2 = 0x04;    font_tbl[0xDD].font_y2 = 0x15;    font_tbl[0xED].font_y2 = 0x0e;    font_tbl[0xFD].font_y2 = 0x04;
    font_tbl[0x2D].font_y3 = 0x1f;    font_tbl[0x3D].font_y3 = 0x00;    font_tbl[0x4D].font_y3 = 0x15;    font_tbl[0x5D].font_y3 = 0x02;    font_tbl[0x6D].font_y3 = 0x15;    font_tbl[0x7D].font_y3 = 0x02;    font_tbl[0xAD].font_y3 = 0x0e;    font_tbl[0xBD].font_y3 = 0x11;    font_tbl[0xCD].font_y3 = 0x08;    font_tbl[0xDD].font_y3 = 0x1f;    font_tbl[0xED].font_y3 = 0x0e;    font_tbl[0xFD].font_y3 = 0x0a;
    font_tbl[0x2D].font_y4 = 0x00;    font_tbl[0x3D].font_y4 = 0x1f;    font_tbl[0x4D].font_y4 = 0x11;    font_tbl[0x5D].font_y4 = 0x02;    font_tbl[0x6D].font_y4 = 0x15;    font_tbl[0x7D].font_y4 = 0x04;    font_tbl[0xAD].font_y4 = 0x09;    font_tbl[0xBD].font_y4 = 0x1f;    font_tbl[0xCD].font_y4 = 0x10;    font_tbl[0xDD].font_y4 = 0x15;    font_tbl[0xED].font_y4 = 0x0e;    font_tbl[0xFD].font_y4 = 0x0a;
    font_tbl[0x2D].font_y5 = 0x00;    font_tbl[0x3D].font_y5 = 0x00;    font_tbl[0x4D].font_y5 = 0x11;    font_tbl[0x5D].font_y5 = 0x02;    font_tbl[0x6D].font_y5 = 0x11;    font_tbl[0x7D].font_y5 = 0x04;    font_tbl[0xAD].font_y5 = 0x09;    font_tbl[0xBD].font_y5 = 0x11;    font_tbl[0xCD].font_y5 = 0x11;    font_tbl[0xDD].font_y5 = 0x11;    font_tbl[0xED].font_y5 = 0x1f;    font_tbl[0xFD].font_y5 = 0x04;    
    font_tbl[0x2D].font_y6 = 0x00;    font_tbl[0x3D].font_y6 = 0x00;    font_tbl[0x4D].font_y6 = 0x11;    font_tbl[0x5D].font_y6 = 0x0e;    font_tbl[0x6D].font_y6 = 0x11;    font_tbl[0x7D].font_y6 = 0x08;    font_tbl[0xAD].font_y6 = 0x0e;    font_tbl[0xBD].font_y6 = 0x11;    font_tbl[0xCD].font_y6 = 0x0e;    font_tbl[0xDD].font_y6 = 0x10;    font_tbl[0xED].font_y6 = 0x04;    font_tbl[0xFD].font_y6 = 0x12;    
    font_tbl[0x2D].font_y7 = 0x00;    font_tbl[0x3D].font_y7 = 0x00;    font_tbl[0x4D].font_y7 = 0x00;    font_tbl[0x5D].font_y7 = 0x00;    font_tbl[0x6D].font_y7 = 0x00;    font_tbl[0x7D].font_y7 = 0x00;    font_tbl[0xAD].font_y7 = 0x00;    font_tbl[0xBD].font_y7 = 0x00;    font_tbl[0xCD].font_y7 = 0x00;    font_tbl[0xDD].font_y7 = 0x00;    font_tbl[0xED].font_y7 = 0x00;    font_tbl[0xFD].font_y7 = 0x0c;        
																																																										
    // .                              // >                              // N                              // ^                              // n                              // ->                                                 
    font_tbl[0x2E].font_y0 = 0x00;    font_tbl[0x3E].font_y0 = 0x08;    font_tbl[0x4E].font_y0 = 0x11;    font_tbl[0x5E].font_y0 = 0x04;    font_tbl[0x6E].font_y0 = 0x00;    font_tbl[0x7E].font_y0 = 0x00;    font_tbl[0xAE].font_y0 = 0x11;    font_tbl[0xBE].font_y0 = 0x00;    font_tbl[0xCE].font_y0 = 0x03;    font_tbl[0xDE].font_y0 = 0x10;    font_tbl[0xEE].font_y0 = 0x00;    font_tbl[0xFE].font_y0 = 0x0f;
    font_tbl[0x2E].font_y1 = 0x00;    font_tbl[0x3E].font_y1 = 0x04;    font_tbl[0x4E].font_y1 = 0x11;    font_tbl[0x5E].font_y1 = 0x0a;    font_tbl[0x6E].font_y1 = 0x00;    font_tbl[0x7E].font_y1 = 0x04;    font_tbl[0xAE].font_y1 = 0x11;    font_tbl[0xBE].font_y1 = 0x00;    font_tbl[0xCE].font_y1 = 0x04;    font_tbl[0xDE].font_y1 = 0x1c;    font_tbl[0xEE].font_y1 = 0x0a;    font_tbl[0xFE].font_y1 = 0x15;
    font_tbl[0x2E].font_y2 = 0x00;    font_tbl[0x3E].font_y2 = 0x02;    font_tbl[0x4E].font_y2 = 0x19;    font_tbl[0x5E].font_y2 = 0x11;    font_tbl[0x6E].font_y2 = 0x16;    font_tbl[0x7E].font_y2 = 0x02;    font_tbl[0xAE].font_y2 = 0x11;    font_tbl[0xBE].font_y2 = 0x1f;    font_tbl[0xCE].font_y2 = 0x04;    font_tbl[0xDE].font_y2 = 0x11;    font_tbl[0xEE].font_y2 = 0x00;    font_tbl[0xFE].font_y2 = 0x15;
    font_tbl[0x2E].font_y3 = 0x00;    font_tbl[0x3E].font_y3 = 0x01;    font_tbl[0x4E].font_y3 = 0x15;    font_tbl[0x5E].font_y3 = 0x00;    font_tbl[0x6E].font_y3 = 0x19;    font_tbl[0x7E].font_y3 = 0x1f;    font_tbl[0xAE].font_y3 = 0x19;    font_tbl[0xBE].font_y3 = 0x11;    font_tbl[0xCE].font_y3 = 0x0e;    font_tbl[0xDE].font_y3 = 0x02;    font_tbl[0xEE].font_y3 = 0x11;    font_tbl[0xFE].font_y3 = 0x0f;
    font_tbl[0x2E].font_y4 = 0x00;    font_tbl[0x3E].font_y4 = 0x02;    font_tbl[0x4E].font_y4 = 0x13;    font_tbl[0x5E].font_y4 = 0x00;    font_tbl[0x6E].font_y4 = 0x11;    font_tbl[0x7E].font_y4 = 0x02;    font_tbl[0xAE].font_y4 = 0x15;    font_tbl[0xBE].font_y4 = 0x11;    font_tbl[0xCE].font_y4 = 0x04;    font_tbl[0xDE].font_y4 = 0x04;    font_tbl[0xEE].font_y4 = 0x00;    font_tbl[0xFE].font_y4 = 0x05;
    font_tbl[0x2E].font_y5 = 0x0c;    font_tbl[0x3E].font_y5 = 0x04;    font_tbl[0x4E].font_y5 = 0x11;    font_tbl[0x5E].font_y5 = 0x00;    font_tbl[0x6E].font_y5 = 0x11;    font_tbl[0x7E].font_y5 = 0x04;    font_tbl[0xAE].font_y5 = 0x15;    font_tbl[0xBE].font_y5 = 0x11;    font_tbl[0xCE].font_y5 = 0x04;    font_tbl[0xDE].font_y5 = 0x08;    font_tbl[0xEE].font_y5 = 0x0a;    font_tbl[0xFE].font_y5 = 0x05;    
    font_tbl[0x2E].font_y6 = 0x0c;    font_tbl[0x3E].font_y6 = 0x08;    font_tbl[0x4E].font_y6 = 0x11;    font_tbl[0x5E].font_y6 = 0x00;    font_tbl[0x6E].font_y6 = 0x11;    font_tbl[0x7E].font_y6 = 0x00;    font_tbl[0xAE].font_y6 = 0x19;    font_tbl[0xBE].font_y6 = 0x11;    font_tbl[0xCE].font_y6 = 0x18;    font_tbl[0xDE].font_y6 = 0x10;    font_tbl[0xEE].font_y6 = 0x00;    font_tbl[0xFE].font_y6 = 0x05;    
    font_tbl[0x2E].font_y7 = 0x00;    font_tbl[0x3E].font_y7 = 0x00;    font_tbl[0x4E].font_y7 = 0x00;    font_tbl[0x5E].font_y7 = 0x00;    font_tbl[0x6E].font_y7 = 0x00;    font_tbl[0x7E].font_y7 = 0x00;    font_tbl[0xAE].font_y7 = 0x00;    font_tbl[0xBE].font_y7 = 0x00;    font_tbl[0xCE].font_y7 = 0x00;    font_tbl[0xDE].font_y7 = 0x00;    font_tbl[0xEE].font_y7 = 0x00;    font_tbl[0xFE].font_y7 = 0x00;        

    // /                              // ?                              // O                              // _                              // o                              // <-                                                 
    font_tbl[0x2F].font_y0 = 0x00;    font_tbl[0x3F].font_y0 = 0x0e;    font_tbl[0x4F].font_y0 = 0x0e;    font_tbl[0x5F].font_y0 = 0x00;    font_tbl[0x6F].font_y0 = 0x00;    font_tbl[0x7F].font_y0 = 0x00;    font_tbl[0xAF].font_y0 = 0x0e;    font_tbl[0xBF].font_y0 = 0x00;    font_tbl[0xCF].font_y0 = 0x04;    font_tbl[0xDF].font_y0 = 0x00;    font_tbl[0xEF].font_y0 = 0x00;    font_tbl[0xFF].font_y0 = 0x1f;
    font_tbl[0x2F].font_y1 = 0x01;    font_tbl[0x3F].font_y1 = 0x11;    font_tbl[0x4F].font_y1 = 0x11;    font_tbl[0x5F].font_y1 = 0x00;    font_tbl[0x6F].font_y1 = 0x00;    font_tbl[0x7F].font_y1 = 0x04;    font_tbl[0xAF].font_y1 = 0x11;    font_tbl[0xBF].font_y1 = 0x00;    font_tbl[0xCF].font_y1 = 0x0a;    font_tbl[0xDF].font_y1 = 0x00;    font_tbl[0xEF].font_y1 = 0x0e;    font_tbl[0xFF].font_y1 = 0x1f;
    font_tbl[0x2F].font_y2 = 0x02;    font_tbl[0x3F].font_y2 = 0x01;    font_tbl[0x4F].font_y2 = 0x11;    font_tbl[0x5F].font_y2 = 0x00;    font_tbl[0x6F].font_y2 = 0x0e;    font_tbl[0x7F].font_y2 = 0x08;    font_tbl[0xAF].font_y2 = 0x01;    font_tbl[0xBF].font_y2 = 0x1f;    font_tbl[0xCF].font_y2 = 0x08;    font_tbl[0xDF].font_y2 = 0x03;    font_tbl[0xEF].font_y2 = 0x11;    font_tbl[0xFF].font_y2 = 0x1f;
    font_tbl[0x2F].font_y3 = 0x04;    font_tbl[0x3F].font_y3 = 0x02;    font_tbl[0x4F].font_y3 = 0x11;    font_tbl[0x5F].font_y3 = 0x00;    font_tbl[0x6F].font_y3 = 0x11;    font_tbl[0x7F].font_y3 = 0x1f;    font_tbl[0xAF].font_y3 = 0x07;    font_tbl[0xBF].font_y3 = 0x04;    font_tbl[0xCF].font_y3 = 0x1f;    font_tbl[0xDF].font_y3 = 0x03;    font_tbl[0xEF].font_y3 = 0x11;    font_tbl[0xFF].font_y3 = 0x1f;
    font_tbl[0x2F].font_y4 = 0x08;    font_tbl[0x3F].font_y4 = 0x04;    font_tbl[0x4F].font_y4 = 0x11;    font_tbl[0x5F].font_y4 = 0x00;    font_tbl[0x6F].font_y4 = 0x11;    font_tbl[0x7F].font_y4 = 0x08;    font_tbl[0xAF].font_y4 = 0x01;    font_tbl[0xBF].font_y4 = 0x04;    font_tbl[0xCF].font_y4 = 0x08;    font_tbl[0xDF].font_y4 = 0x00;    font_tbl[0xEF].font_y4 = 0x11;    font_tbl[0xFF].font_y4 = 0x1f;
    font_tbl[0x2F].font_y5 = 0x10;    font_tbl[0x3F].font_y5 = 0x00;    font_tbl[0x4F].font_y5 = 0x11;    font_tbl[0x5F].font_y5 = 0x00;    font_tbl[0x6F].font_y5 = 0x11;    font_tbl[0x7F].font_y5 = 0x04;    font_tbl[0xAF].font_y5 = 0x11;    font_tbl[0xBF].font_y5 = 0x04;    font_tbl[0xCF].font_y5 = 0x08;    font_tbl[0xDF].font_y5 = 0x00;    font_tbl[0xEF].font_y5 = 0x0e;    font_tbl[0xFF].font_y5 = 0x1f;
    font_tbl[0x2F].font_y6 = 0x00;    font_tbl[0x3F].font_y6 = 0x04;    font_tbl[0x4F].font_y6 = 0x0e;    font_tbl[0x5F].font_y6 = 0x1f;    font_tbl[0x6F].font_y6 = 0x0e;    font_tbl[0x7F].font_y6 = 0x00;    font_tbl[0xAF].font_y6 = 0x0e;    font_tbl[0xBF].font_y6 = 0x04;    font_tbl[0xCF].font_y6 = 0x0f;    font_tbl[0xDF].font_y6 = 0x00;    font_tbl[0xEF].font_y6 = 0x00;    font_tbl[0xFF].font_y6 = 0x1f;
    font_tbl[0x2F].font_y7 = 0x00;    font_tbl[0x3F].font_y7 = 0x00;    font_tbl[0x4F].font_y7 = 0x00;    font_tbl[0x5F].font_y7 = 0x00;    font_tbl[0x6F].font_y7 = 0x00;    font_tbl[0x7F].font_y7 = 0x00;    font_tbl[0xAF].font_y7 = 0x00;    font_tbl[0xBF].font_y7 = 0x00;    font_tbl[0xCF].font_y7 = 0x00;    font_tbl[0xDF].font_y7 = 0x00;    font_tbl[0xEF].font_y7 = 0x00;    font_tbl[0xFF].font_y7 = 0x1f;
#else
    // space                          // 0                              // @                              // P                              // '                              // p                              // invalid                        // -                              // japan                          // japan                          // alpha                          // p
    font_tbl[0x20].font_y0 = 0x00;    font_tbl[0x30].font_y0 = 0x0e;    font_tbl[0x40].font_y0 = 0x0e;    font_tbl[0x50].font_y0 = 0x1e;    font_tbl[0x60].font_y0 = 0x08;    font_tbl[0x70].font_y0 = 0x00;    font_tbl[0xA0].font_y0 = 0x00;    font_tbl[0xB0].font_y0 = 0x00;    font_tbl[0xC0].font_y0 = 0x00;    font_tbl[0xD0].font_y0 = 0x00;    font_tbl[0xE0].font_y0 = 0x00;    font_tbl[0xF0].font_y0 = 0x00;
    font_tbl[0x20].font_y1 = 0x00;    font_tbl[0x30].font_y1 = 0x11;    font_tbl[0x40].font_y1 = 0x11;    font_tbl[0x50].font_y1 = 0x11;    font_tbl[0x60].font_y1 = 0x04;    font_tbl[0x70].font_y1 = 0x00;    font_tbl[0xA0].font_y1 = 0x00;    font_tbl[0xB0].font_y1 = 0x00;    font_tbl[0xC0].font_y1 = 0x0f;    font_tbl[0xD0].font_y1 = 0x0e;    font_tbl[0xE0].font_y1 = 0x00;    font_tbl[0xF0].font_y1 = 0x00;
    font_tbl[0x20].font_y2 = 0x00;    font_tbl[0x30].font_y2 = 0x13;    font_tbl[0x40].font_y2 = 0x01;    font_tbl[0x50].font_y2 = 0x11;    font_tbl[0x60].font_y2 = 0x02;    font_tbl[0x70].font_y2 = 0x1e;    font_tbl[0xA0].font_y2 = 0x00;    font_tbl[0xB0].font_y2 = 0x00;    font_tbl[0xC0].font_y2 = 0x09;    font_tbl[0xD0].font_y2 = 0x00;    font_tbl[0xE0].font_y2 = 0x09;    font_tbl[0xF0].font_y2 = 0x16;
    font_tbl[0x20].font_y3 = 0x00;    font_tbl[0x30].font_y3 = 0x15;    font_tbl[0x40].font_y3 = 0x0d;    font_tbl[0x50].font_y3 = 0x1e;    font_tbl[0x60].font_y3 = 0x00;    font_tbl[0x70].font_y3 = 0x11;    font_tbl[0xA0].font_y3 = 0x00;    font_tbl[0xB0].font_y3 = 0x1f;    font_tbl[0xC0].font_y3 = 0x15;    font_tbl[0xD0].font_y3 = 0x0e;    font_tbl[0xE0].font_y3 = 0x15;    font_tbl[0xF0].font_y3 = 0x19;
    font_tbl[0x20].font_y4 = 0x00;    font_tbl[0x30].font_y4 = 0x19;    font_tbl[0x40].font_y4 = 0x15;    font_tbl[0x50].font_y4 = 0x10;    font_tbl[0x60].font_y4 = 0x00;    font_tbl[0x70].font_y4 = 0x1e;    font_tbl[0xA0].font_y4 = 0x00;    font_tbl[0xB0].font_y4 = 0x00;    font_tbl[0xC0].font_y4 = 0x03;    font_tbl[0xD0].font_y4 = 0x00;    font_tbl[0xE0].font_y4 = 0x12;    font_tbl[0xF0].font_y4 = 0x11;
    font_tbl[0x20].font_y5 = 0x00;    font_tbl[0x30].font_y5 = 0x11;    font_tbl[0x40].font_y5 = 0x15;    font_tbl[0x50].font_y5 = 0x10;    font_tbl[0x60].font_y5 = 0x00;    font_tbl[0x70].font_y5 = 0x10;    font_tbl[0xA0].font_y5 = 0x00;    font_tbl[0xB0].font_y5 = 0x00;    font_tbl[0xC0].font_y5 = 0x02;    font_tbl[0xD0].font_y5 = 0x0e;    font_tbl[0xE0].font_y5 = 0x12;    font_tbl[0xF0].font_y5 = 0x11;    
    font_tbl[0x20].font_y6 = 0x00;    font_tbl[0x30].font_y6 = 0x0e;    font_tbl[0x40].font_y6 = 0x0e;    font_tbl[0x50].font_y6 = 0x10;    font_tbl[0x60].font_y6 = 0x00;    font_tbl[0x70].font_y6 = 0x10;    font_tbl[0xA0].font_y6 = 0x00;    font_tbl[0xB0].font_y6 = 0x00;    font_tbl[0xC0].font_y6 = 0x0c;    font_tbl[0xD0].font_y6 = 0x01;    font_tbl[0xE0].font_y6 = 0x0d;    font_tbl[0xF0].font_y6 = 0x1e;    
    font_tbl[0x20].font_y7 = 0x00;    font_tbl[0x30].font_y7 = 0x00;    font_tbl[0x40].font_y7 = 0x00;    font_tbl[0x50].font_y7 = 0x00;    font_tbl[0x60].font_y7 = 0x00;    font_tbl[0x70].font_y7 = 0x00;    font_tbl[0xA0].font_y7 = 0x00;    font_tbl[0xB0].font_y7 = 0x00;    font_tbl[0xC0].font_y7 = 0x00;    font_tbl[0xD0].font_y7 = 0x00;    font_tbl[0xE0].font_y7 = 0x00;    font_tbl[0xF0].font_y7 = 0x10;    

    // !                              // 1                              // A                              // Q                              // a                              // q                              // japan                          // japan                          // japan                          // japan                          // ä                              // q
    font_tbl[0x21].font_y0 = 0x04;    font_tbl[0x31].font_y0 = 0x04;    font_tbl[0x41].font_y0 = 0x0e;    font_tbl[0x51].font_y0 = 0x0e;    font_tbl[0x61].font_y0 = 0x00;    font_tbl[0x71].font_y0 = 0x00;    font_tbl[0xA1].font_y0 = 0x00;    font_tbl[0xB1].font_y0 = 0x1f;    font_tbl[0xC1].font_y0 = 0x02;    font_tbl[0xD1].font_y0 = 0x04;    font_tbl[0xE1].font_y0 = 0x0a;    font_tbl[0xF1].font_y0 = 0x00;
    font_tbl[0x21].font_y1 = 0x04;    font_tbl[0x31].font_y1 = 0x0c;    font_tbl[0x41].font_y1 = 0x11;    font_tbl[0x51].font_y1 = 0x11;    font_tbl[0x61].font_y1 = 0x00;    font_tbl[0x71].font_y1 = 0x00;    font_tbl[0xA1].font_y1 = 0x00;    font_tbl[0xB1].font_y1 = 0x01;    font_tbl[0xC1].font_y1 = 0x1c;    font_tbl[0xD1].font_y1 = 0x08;    font_tbl[0xE1].font_y1 = 0x00;    font_tbl[0xF1].font_y1 = 0x00;
    font_tbl[0x21].font_y2 = 0x04;    font_tbl[0x31].font_y2 = 0x04;    font_tbl[0x41].font_y2 = 0x11;    font_tbl[0x51].font_y2 = 0x11;    font_tbl[0x61].font_y2 = 0x0e;    font_tbl[0x71].font_y2 = 0x0d;    font_tbl[0xA1].font_y2 = 0x00;    font_tbl[0xB1].font_y2 = 0x05;    font_tbl[0xC1].font_y2 = 0x04;    font_tbl[0xD1].font_y2 = 0x10;    font_tbl[0xE1].font_y2 = 0x0e;    font_tbl[0xF1].font_y2 = 0x0d;
    font_tbl[0x21].font_y3 = 0x04;    font_tbl[0x31].font_y3 = 0x04;    font_tbl[0x41].font_y3 = 0x1f;    font_tbl[0x51].font_y3 = 0x11;    font_tbl[0x61].font_y3 = 0x01;    font_tbl[0x71].font_y3 = 0x13;    font_tbl[0xA1].font_y3 = 0x00;    font_tbl[0xB1].font_y3 = 0x06;    font_tbl[0xC1].font_y3 = 0x1f;    font_tbl[0xD1].font_y3 = 0x10;    font_tbl[0xE1].font_y3 = 0x01;    font_tbl[0xF1].font_y3 = 0x13;
    font_tbl[0x21].font_y4 = 0x00;    font_tbl[0x31].font_y4 = 0x04;    font_tbl[0x41].font_y4 = 0x11;    font_tbl[0x51].font_y4 = 0x15;    font_tbl[0x61].font_y4 = 0x0f;    font_tbl[0x71].font_y4 = 0x0f;    font_tbl[0xA1].font_y4 = 0x1c;    font_tbl[0xB1].font_y4 = 0x04;    font_tbl[0xC1].font_y4 = 0x04;    font_tbl[0xD1].font_y4 = 0x11;    font_tbl[0xE1].font_y4 = 0x0f;    font_tbl[0xF1].font_y4 = 0x11;
    font_tbl[0x21].font_y5 = 0x00;    font_tbl[0x31].font_y5 = 0x04;    font_tbl[0x41].font_y5 = 0x11;    font_tbl[0x51].font_y5 = 0x12;    font_tbl[0x61].font_y5 = 0x11;    font_tbl[0x71].font_y5 = 0x01;    font_tbl[0xA1].font_y5 = 0x14;    font_tbl[0xB1].font_y5 = 0x04;    font_tbl[0xC1].font_y5 = 0x04;    font_tbl[0xD1].font_y5 = 0x1f;    font_tbl[0xE1].font_y5 = 0x11;    font_tbl[0xF1].font_y5 = 0x11;
    font_tbl[0x21].font_y6 = 0x04;    font_tbl[0x31].font_y6 = 0x0e;    font_tbl[0x41].font_y6 = 0x11;    font_tbl[0x51].font_y6 = 0x0d;    font_tbl[0x61].font_y6 = 0x0f;    font_tbl[0x71].font_y6 = 0x01;    font_tbl[0xA1].font_y6 = 0x1c;    font_tbl[0xB1].font_y6 = 0x08;    font_tbl[0xC1].font_y6 = 0x08;    font_tbl[0xD1].font_y6 = 0x01;    font_tbl[0xE1].font_y6 = 0x0f;    font_tbl[0xF1].font_y6 = 0x0f;
    font_tbl[0x21].font_y7 = 0x00;    font_tbl[0x31].font_y7 = 0x00;    font_tbl[0x41].font_y7 = 0x00;    font_tbl[0x51].font_y7 = 0x00;    font_tbl[0x61].font_y7 = 0x00;    font_tbl[0x71].font_y7 = 0x00;    font_tbl[0xA1].font_y7 = 0x00;    font_tbl[0xB1].font_y7 = 0x00;    font_tbl[0xC1].font_y7 = 0x00;    font_tbl[0xD1].font_y7 = 0x00;    font_tbl[0xE1].font_y7 = 0x00;    font_tbl[0xF1].font_y7 = 0x01;    

    // "                              // 2                              // B                              // R                              // b                              // r                              // japan                          // japan                          // japan                          // japan                          // Sharp S                        // o
    font_tbl[0x22].font_y0 = 0x0a;    font_tbl[0x32].font_y0 = 0x0e;    font_tbl[0x42].font_y0 = 0x1e;    font_tbl[0x52].font_y0 = 0x1e;    font_tbl[0x62].font_y0 = 0x10;    font_tbl[0x72].font_y0 = 0x00;    font_tbl[0xA2].font_y0 = 0x07;    font_tbl[0xB2].font_y0 = 0x01;    font_tbl[0xC2].font_y0 = 0x00;    font_tbl[0xD2].font_y0 = 0x00;    font_tbl[0xE2].font_y0 = 0x00;    font_tbl[0xF2].font_y0 = 0x00;
    font_tbl[0x22].font_y1 = 0x0a;    font_tbl[0x32].font_y1 = 0x11;    font_tbl[0x42].font_y1 = 0x11;    font_tbl[0x52].font_y1 = 0x11;    font_tbl[0x62].font_y1 = 0x10;    font_tbl[0x72].font_y1 = 0x00;    font_tbl[0xA2].font_y1 = 0x04;    font_tbl[0xB2].font_y1 = 0x02;    font_tbl[0xC2].font_y1 = 0x15;    font_tbl[0xD2].font_y1 = 0x01;    font_tbl[0xE2].font_y1 = 0x00;    font_tbl[0xF2].font_y1 = 0x0e;
    font_tbl[0x22].font_y2 = 0x0a;    font_tbl[0x32].font_y2 = 0x01;    font_tbl[0x42].font_y2 = 0x11;    font_tbl[0x52].font_y2 = 0x11;    font_tbl[0x62].font_y2 = 0x16;    font_tbl[0x72].font_y2 = 0x16;    font_tbl[0xA2].font_y2 = 0x04;    font_tbl[0xB2].font_y2 = 0x04;    font_tbl[0xC2].font_y2 = 0x15;    font_tbl[0xD2].font_y2 = 0x01;    font_tbl[0xE2].font_y2 = 0x0e;    font_tbl[0xF2].font_y2 = 0x11;
    font_tbl[0x22].font_y3 = 0x00;    font_tbl[0x32].font_y3 = 0x02;    font_tbl[0x42].font_y3 = 0x1e;    font_tbl[0x52].font_y3 = 0x1e;    font_tbl[0x62].font_y3 = 0x19;    font_tbl[0x72].font_y3 = 0x19;    font_tbl[0xA2].font_y3 = 0x04;    font_tbl[0xB2].font_y3 = 0x0c;    font_tbl[0xC2].font_y3 = 0x15;    font_tbl[0xD2].font_y3 = 0x0a;    font_tbl[0xE2].font_y3 = 0x11;    font_tbl[0xF2].font_y3 = 0x1f;
    font_tbl[0x22].font_y4 = 0x00;    font_tbl[0x32].font_y4 = 0x04;    font_tbl[0x42].font_y4 = 0x11;    font_tbl[0x52].font_y4 = 0x14;    font_tbl[0x62].font_y4 = 0x11;    font_tbl[0x72].font_y4 = 0x10;    font_tbl[0xA2].font_y4 = 0x00;    font_tbl[0xB2].font_y4 = 0x14;    font_tbl[0xC2].font_y4 = 0x01;    font_tbl[0xD2].font_y4 = 0x04;    font_tbl[0xE2].font_y4 = 0x1e;    font_tbl[0xF2].font_y4 = 0x11;
    font_tbl[0x22].font_y5 = 0x00;    font_tbl[0x32].font_y5 = 0x08;    font_tbl[0x42].font_y5 = 0x11;    font_tbl[0x52].font_y5 = 0x12;    font_tbl[0x62].font_y5 = 0x11;    font_tbl[0x72].font_y5 = 0x10;    font_tbl[0xA2].font_y5 = 0x00;    font_tbl[0xB2].font_y5 = 0x04;    font_tbl[0xC2].font_y5 = 0x02;    font_tbl[0xD2].font_y5 = 0x0a;    font_tbl[0xE2].font_y5 = 0x11;    font_tbl[0xF2].font_y5 = 0x11;
    font_tbl[0x22].font_y6 = 0x00;    font_tbl[0x32].font_y6 = 0x1f;    font_tbl[0x42].font_y6 = 0x1e;    font_tbl[0x52].font_y6 = 0x11;    font_tbl[0x62].font_y6 = 0x1e;    font_tbl[0x72].font_y6 = 0x10;    font_tbl[0xA2].font_y6 = 0x00;    font_tbl[0xB2].font_y6 = 0x04;    font_tbl[0xC2].font_y6 = 0x04;    font_tbl[0xD2].font_y6 = 0x10;    font_tbl[0xE2].font_y6 = 0x1e;    font_tbl[0xF2].font_y6 = 0x0e;
    font_tbl[0x22].font_y7 = 0x00;    font_tbl[0x32].font_y7 = 0x00;    font_tbl[0x42].font_y7 = 0x00;    font_tbl[0x52].font_y7 = 0x00;    font_tbl[0x62].font_y7 = 0x00;    font_tbl[0x72].font_y7 = 0x00;    font_tbl[0xA2].font_y7 = 0x00;    font_tbl[0xB2].font_y7 = 0x00;    font_tbl[0xC2].font_y7 = 0x00;    font_tbl[0xD2].font_y7 = 0x00;    font_tbl[0xE2].font_y7 = 0x10;    font_tbl[0xF2].font_y7 = 0x00;    

    // #                              // 3                              // C                              // S                              // c                              // s                              // japan                          // japan                          // japan                          // japan                          // Backward E                     // ~
    font_tbl[0x23].font_y0 = 0x0a;    font_tbl[0x33].font_y0 = 0x1f;    font_tbl[0x43].font_y0 = 0x0e;    font_tbl[0x53].font_y0 = 0x0f;    font_tbl[0x63].font_y0 = 0x00;    font_tbl[0x73].font_y0 = 0x00;    font_tbl[0xA3].font_y0 = 0x00;    font_tbl[0xB3].font_y0 = 0x04;    font_tbl[0xC3].font_y0 = 0x0e;    font_tbl[0xD3].font_y0 = 0x00;    font_tbl[0xE3].font_y0 = 0x00;    font_tbl[0xF3].font_y0 = 0x00;
    font_tbl[0x23].font_y1 = 0x0a;    font_tbl[0x33].font_y1 = 0x02;    font_tbl[0x43].font_y1 = 0x11;    font_tbl[0x53].font_y1 = 0x10;    font_tbl[0x63].font_y1 = 0x00;    font_tbl[0x73].font_y1 = 0x00;    font_tbl[0xA3].font_y1 = 0x00;    font_tbl[0xB3].font_y1 = 0x1f;    font_tbl[0xC3].font_y1 = 0x00;    font_tbl[0xD3].font_y1 = 0x1f;    font_tbl[0xE3].font_y1 = 0x00;    font_tbl[0xF3].font_y1 = 0x00;
    font_tbl[0x23].font_y2 = 0x1f;    font_tbl[0x33].font_y2 = 0x04;    font_tbl[0x43].font_y2 = 0x10;    font_tbl[0x53].font_y2 = 0x10;    font_tbl[0x63].font_y2 = 0x0e;    font_tbl[0x73].font_y2 = 0x0e;    font_tbl[0xA3].font_y2 = 0x00;    font_tbl[0xB3].font_y2 = 0x11;    font_tbl[0xC3].font_y2 = 0x1f;    font_tbl[0xD3].font_y2 = 0x08;    font_tbl[0xE3].font_y2 = 0x0e;    font_tbl[0xF3].font_y2 = 0x00;
    font_tbl[0x23].font_y3 = 0x0a;    font_tbl[0x33].font_y3 = 0x02;    font_tbl[0x43].font_y3 = 0x10;    font_tbl[0x53].font_y3 = 0x0e;    font_tbl[0x63].font_y3 = 0x10;    font_tbl[0x73].font_y3 = 0x10;    font_tbl[0xA3].font_y3 = 0x04;    font_tbl[0xB3].font_y3 = 0x11;    font_tbl[0xC3].font_y3 = 0x04;    font_tbl[0xD3].font_y3 = 0x1f;    font_tbl[0xE3].font_y3 = 0x10;    font_tbl[0xF3].font_y3 = 0x0b;
    font_tbl[0x23].font_y4 = 0x1f;    font_tbl[0x33].font_y4 = 0x01;    font_tbl[0x43].font_y4 = 0x10;    font_tbl[0x53].font_y4 = 0x01;    font_tbl[0x63].font_y4 = 0x10;    font_tbl[0x73].font_y4 = 0x0e;    font_tbl[0xA3].font_y4 = 0x04;    font_tbl[0xB3].font_y4 = 0x01;    font_tbl[0xC3].font_y4 = 0x04;    font_tbl[0xD3].font_y4 = 0x08;    font_tbl[0xE3].font_y4 = 0x0c;    font_tbl[0xF3].font_y4 = 0x15;
    font_tbl[0x23].font_y5 = 0x0a;    font_tbl[0x33].font_y5 = 0x11;    font_tbl[0x43].font_y5 = 0x11;    font_tbl[0x53].font_y5 = 0x01;    font_tbl[0x63].font_y5 = 0x10;    font_tbl[0x73].font_y5 = 0x01;    font_tbl[0xA3].font_y5 = 0x04;    font_tbl[0xB3].font_y5 = 0x02;    font_tbl[0xC3].font_y5 = 0x04;    font_tbl[0xD3].font_y5 = 0x08;    font_tbl[0xE3].font_y5 = 0x11;    font_tbl[0xF3].font_y5 = 0x1a;
    font_tbl[0x23].font_y6 = 0x0a;    font_tbl[0x33].font_y6 = 0x0e;    font_tbl[0x43].font_y6 = 0x0e;    font_tbl[0x53].font_y6 = 0x1e;    font_tbl[0x63].font_y6 = 0x0e;    font_tbl[0x73].font_y6 = 0x1e;    font_tbl[0xA3].font_y6 = 0x1c;    font_tbl[0xB3].font_y6 = 0x04;    font_tbl[0xC3].font_y6 = 0x08;    font_tbl[0xD3].font_y6 = 0x07;    font_tbl[0xE3].font_y6 = 0x0e;    font_tbl[0xF3].font_y6 = 0x00;
    font_tbl[0x23].font_y7 = 0x00;    font_tbl[0x33].font_y7 = 0x00;    font_tbl[0x43].font_y7 = 0x00;    font_tbl[0x53].font_y7 = 0x00;    font_tbl[0x63].font_y7 = 0x00;    font_tbl[0x73].font_y7 = 0x00;    font_tbl[0xA3].font_y7 = 0x00;    font_tbl[0xB3].font_y7 = 0x00;    font_tbl[0xC3].font_y7 = 0x00;    font_tbl[0xD3].font_y7 = 0x00;    font_tbl[0xE3].font_y7 = 0x00;    font_tbl[0xF3].font_y7 = 0x00;    

    // $                              // 4                              // D                              // T                              // d                              // t                              // japan                          // japan                          // japan                          // japan                          // pico                           // omega
    font_tbl[0x24].font_y0 = 0x04;    font_tbl[0x34].font_y0 = 0x02;    font_tbl[0x44].font_y0 = 0x1c;    font_tbl[0x54].font_y0 = 0x1f;    font_tbl[0x64].font_y0 = 0x01;    font_tbl[0x74].font_y0 = 0x08;    font_tbl[0xA4].font_y0 = 0x00;    font_tbl[0xB4].font_y0 = 0x00;    font_tbl[0xC4].font_y0 = 0x08;    font_tbl[0xD4].font_y0 = 0x08;    font_tbl[0xE4].font_y0 = 0x00;    font_tbl[0xF4].font_y0 = 0x00;
    font_tbl[0x24].font_y1 = 0x0f;    font_tbl[0x34].font_y1 = 0x06;    font_tbl[0x44].font_y1 = 0x12;    font_tbl[0x54].font_y1 = 0x04;    font_tbl[0x64].font_y1 = 0x01;    font_tbl[0x74].font_y1 = 0x08;    font_tbl[0xA4].font_y1 = 0x00;    font_tbl[0xB4].font_y1 = 0x1f;    font_tbl[0xC4].font_y1 = 0x08;    font_tbl[0xD4].font_y1 = 0x08;    font_tbl[0xE4].font_y1 = 0x00;    font_tbl[0xF4].font_y1 = 0x00;
    font_tbl[0x24].font_y2 = 0x14;    font_tbl[0x34].font_y2 = 0x0a;    font_tbl[0x44].font_y2 = 0x11;    font_tbl[0x54].font_y2 = 0x04;    font_tbl[0x64].font_y2 = 0x0d;    font_tbl[0x74].font_y2 = 0x1c;    font_tbl[0xA4].font_y2 = 0x00;    font_tbl[0xB4].font_y2 = 0x04;    font_tbl[0xC4].font_y2 = 0x08;    font_tbl[0xD4].font_y2 = 0x1f;    font_tbl[0xE4].font_y2 = 0x11;    font_tbl[0xF4].font_y2 = 0x0e;
    font_tbl[0x24].font_y3 = 0x0e;    font_tbl[0x34].font_y3 = 0x12;    font_tbl[0x44].font_y3 = 0x11;    font_tbl[0x54].font_y3 = 0x04;    font_tbl[0x64].font_y3 = 0x13;    font_tbl[0x74].font_y3 = 0x08;    font_tbl[0xA4].font_y3 = 0x00;    font_tbl[0xB4].font_y3 = 0x04;    font_tbl[0xC4].font_y3 = 0x0c;    font_tbl[0xD4].font_y3 = 0x09;    font_tbl[0xE4].font_y3 = 0x11;    font_tbl[0xF4].font_y3 = 0x11;
    font_tbl[0x24].font_y4 = 0x05;    font_tbl[0x34].font_y4 = 0x1f;    font_tbl[0x44].font_y4 = 0x11;    font_tbl[0x54].font_y4 = 0x04;    font_tbl[0x64].font_y4 = 0x11;    font_tbl[0x74].font_y4 = 0x08;    font_tbl[0xA4].font_y4 = 0x10;    font_tbl[0xB4].font_y4 = 0x04;    font_tbl[0xC4].font_y4 = 0x0a;    font_tbl[0xD4].font_y4 = 0x0a;    font_tbl[0xE4].font_y4 = 0x11;    font_tbl[0xF4].font_y4 = 0x11;
    font_tbl[0x24].font_y5 = 0x1e;    font_tbl[0x34].font_y5 = 0x02;    font_tbl[0x44].font_y5 = 0x12;    font_tbl[0x54].font_y5 = 0x04;    font_tbl[0x64].font_y5 = 0x11;    font_tbl[0x74].font_y5 = 0x09;    font_tbl[0xA4].font_y5 = 0x08;    font_tbl[0xB4].font_y5 = 0x04;    font_tbl[0xC4].font_y5 = 0x08;    font_tbl[0xD4].font_y5 = 0x08;    font_tbl[0xE4].font_y5 = 0x13;    font_tbl[0xF4].font_y5 = 0x0a;    
    font_tbl[0x24].font_y6 = 0x04;    font_tbl[0x34].font_y6 = 0x02;    font_tbl[0x44].font_y6 = 0x1c;    font_tbl[0x54].font_y6 = 0x04;    font_tbl[0x64].font_y6 = 0x0f;    font_tbl[0x74].font_y6 = 0x06;    font_tbl[0xA4].font_y6 = 0x04;    font_tbl[0xB4].font_y6 = 0x1f;    font_tbl[0xC4].font_y6 = 0x08;    font_tbl[0xD4].font_y6 = 0x08;    font_tbl[0xE4].font_y6 = 0x1d;    font_tbl[0xF4].font_y6 = 0x1b;    
    font_tbl[0x24].font_y7 = 0x00;    font_tbl[0x34].font_y7 = 0x00;    font_tbl[0x44].font_y7 = 0x00;    font_tbl[0x54].font_y7 = 0x00;    font_tbl[0x64].font_y7 = 0x00;    font_tbl[0x74].font_y7 = 0x00;    font_tbl[0xA4].font_y7 = 0x00;    font_tbl[0xB4].font_y7 = 0x00;    font_tbl[0xC4].font_y7 = 0x00;    font_tbl[0xD4].font_y7 = 0x00;    font_tbl[0xE4].font_y7 = 0x10;    font_tbl[0xF4].font_y7 = 0x00;        

    // %                              // 5                              // E                              // U                              // e                              // u                              // japan                          // japan                          // japan                          // japan                          // unknown character              // ü
    font_tbl[0x25].font_y0 = 0x18;    font_tbl[0x35].font_y0 = 0x1f;    font_tbl[0x45].font_y0 = 0x1f;    font_tbl[0x55].font_y0 = 0x11;    font_tbl[0x65].font_y0 = 0x00;    font_tbl[0x75].font_y0 = 0x00;    font_tbl[0xA5].font_y0 = 0x00;    font_tbl[0xB5].font_y0 = 0x02;    font_tbl[0xC5].font_y0 = 0x04;    font_tbl[0xD5].font_y0 = 0x00;    font_tbl[0xE5].font_y0 = 0x00;    font_tbl[0xF5].font_y0 = 0x0a;
    font_tbl[0x25].font_y1 = 0x19;    font_tbl[0x35].font_y1 = 0x10;    font_tbl[0x45].font_y1 = 0x10;    font_tbl[0x55].font_y1 = 0x11;    font_tbl[0x65].font_y1 = 0x00;    font_tbl[0x75].font_y1 = 0x00;    font_tbl[0xA5].font_y1 = 0x00;    font_tbl[0xB5].font_y1 = 0x1f;    font_tbl[0xC5].font_y1 = 0x04;    font_tbl[0xD5].font_y1 = 0x0e;    font_tbl[0xE5].font_y1 = 0x00;    font_tbl[0xF5].font_y1 = 0x00;
    font_tbl[0x25].font_y2 = 0x02;    font_tbl[0x35].font_y2 = 0x1e;    font_tbl[0x45].font_y2 = 0x10;    font_tbl[0x55].font_y2 = 0x11;    font_tbl[0x65].font_y2 = 0x0e;    font_tbl[0x75].font_y2 = 0x11;    font_tbl[0xA5].font_y2 = 0x00;    font_tbl[0xB5].font_y2 = 0x02;    font_tbl[0xC5].font_y2 = 0x1f;    font_tbl[0xD5].font_y2 = 0x02;    font_tbl[0xE5].font_y2 = 0x0f;    font_tbl[0xF5].font_y2 = 0x11;
    font_tbl[0x25].font_y3 = 0x04;    font_tbl[0x35].font_y3 = 0x01;    font_tbl[0x45].font_y3 = 0x1e;    font_tbl[0x55].font_y3 = 0x11;    font_tbl[0x65].font_y3 = 0x11;    font_tbl[0x75].font_y3 = 0x11;    font_tbl[0xA5].font_y3 = 0x0c;    font_tbl[0xB5].font_y3 = 0x06;    font_tbl[0xC5].font_y3 = 0x04;    font_tbl[0xD5].font_y3 = 0x02;    font_tbl[0xE5].font_y3 = 0x14;    font_tbl[0xF5].font_y3 = 0x11;
    font_tbl[0x25].font_y4 = 0x08;    font_tbl[0x35].font_y4 = 0x01;    font_tbl[0x45].font_y4 = 0x10;    font_tbl[0x55].font_y4 = 0x11;    font_tbl[0x65].font_y4 = 0x1f;    font_tbl[0x75].font_y4 = 0x11;    font_tbl[0xA5].font_y4 = 0x0c;    font_tbl[0xB5].font_y4 = 0x0a;    font_tbl[0xC5].font_y4 = 0x04;    font_tbl[0xD5].font_y4 = 0x02;    font_tbl[0xE5].font_y4 = 0x12;    font_tbl[0xF5].font_y4 = 0x11;
    font_tbl[0x25].font_y5 = 0x13;    font_tbl[0x35].font_y5 = 0x11;    font_tbl[0x45].font_y5 = 0x10;    font_tbl[0x55].font_y5 = 0x11;    font_tbl[0x65].font_y5 = 0x10;    font_tbl[0x75].font_y5 = 0x13;    font_tbl[0xA5].font_y5 = 0x00;    font_tbl[0xB5].font_y5 = 0x12;    font_tbl[0xC5].font_y5 = 0x08;    font_tbl[0xD5].font_y5 = 0x02;    font_tbl[0xE5].font_y5 = 0x11;    font_tbl[0xF5].font_y5 = 0x13;    
    font_tbl[0x25].font_y6 = 0x03;    font_tbl[0x35].font_y6 = 0x0e;    font_tbl[0x45].font_y6 = 0x1f;    font_tbl[0x55].font_y6 = 0x0e;    font_tbl[0x65].font_y6 = 0x0e;    font_tbl[0x75].font_y6 = 0x0d;    font_tbl[0xA5].font_y6 = 0x00;    font_tbl[0xB5].font_y6 = 0x02;    font_tbl[0xC5].font_y6 = 0x10;    font_tbl[0xD5].font_y6 = 0x1f;    font_tbl[0xE5].font_y6 = 0x0e;    font_tbl[0xF5].font_y6 = 0x0d;    
    font_tbl[0x25].font_y7 = 0x00;    font_tbl[0x35].font_y7 = 0x00;    font_tbl[0x45].font_y7 = 0x00;    font_tbl[0x55].font_y7 = 0x00;    font_tbl[0x65].font_y7 = 0x00;    font_tbl[0x75].font_y7 = 0x00;    font_tbl[0xA5].font_y7 = 0x00;    font_tbl[0xB5].font_y7 = 0x00;    font_tbl[0xC5].font_y7 = 0x00;    font_tbl[0xD5].font_y7 = 0x00;    font_tbl[0xE5].font_y7 = 0x00;    font_tbl[0xF5].font_y7 = 0x00;        

    // &                              // 6                              // F                              // V                              // f                              // v                              // japan                          // japan                          // japan                          // japan                          // unknown character              // sum
    font_tbl[0x26].font_y0 = 0x0c;    font_tbl[0x36].font_y0 = 0x06;    font_tbl[0x46].font_y0 = 0x1f;    font_tbl[0x56].font_y0 = 0x11;    font_tbl[0x66].font_y0 = 0x06;    font_tbl[0x76].font_y0 = 0x00;    font_tbl[0xA6].font_y0 = 0x00;    font_tbl[0xB6].font_y0 = 0x08;    font_tbl[0xC6].font_y0 = 0x00;    font_tbl[0xD6].font_y0 = 0x00;    font_tbl[0xE6].font_y0 = 0x00;    font_tbl[0xF6].font_y0 = 0x1f;
    font_tbl[0x26].font_y1 = 0x12;    font_tbl[0x36].font_y1 = 0x08;    font_tbl[0x46].font_y1 = 0x10;    font_tbl[0x56].font_y1 = 0x11;    font_tbl[0x66].font_y1 = 0x09;    font_tbl[0x76].font_y1 = 0x00;    font_tbl[0xA6].font_y1 = 0x1f;    font_tbl[0xB6].font_y1 = 0x1f;    font_tbl[0xC6].font_y1 = 0x0e;    font_tbl[0xD6].font_y1 = 0x1f;    font_tbl[0xE6].font_y1 = 0x00;    font_tbl[0xF6].font_y1 = 0x10;
    font_tbl[0x26].font_y2 = 0x14;    font_tbl[0x36].font_y2 = 0x10;    font_tbl[0x46].font_y2 = 0x10;    font_tbl[0x56].font_y2 = 0x11;    font_tbl[0x66].font_y2 = 0x08;    font_tbl[0x76].font_y2 = 0x11;    font_tbl[0xA6].font_y2 = 0x01;    font_tbl[0xB6].font_y2 = 0x09;    font_tbl[0xC6].font_y2 = 0x00;    font_tbl[0xD6].font_y2 = 0x01;    font_tbl[0xE6].font_y2 = 0x06;    font_tbl[0xF6].font_y2 = 0x08;
    font_tbl[0x26].font_y3 = 0x08;    font_tbl[0x36].font_y3 = 0x1e;    font_tbl[0x46].font_y3 = 0x1e;    font_tbl[0x56].font_y3 = 0x11;    font_tbl[0x66].font_y3 = 0x1c;    font_tbl[0x76].font_y3 = 0x11;    font_tbl[0xA6].font_y3 = 0x1f;    font_tbl[0xB6].font_y3 = 0x09;    font_tbl[0xC6].font_y3 = 0x00;    font_tbl[0xD6].font_y3 = 0x1f;    font_tbl[0xE6].font_y3 = 0x09;    font_tbl[0xF6].font_y3 = 0x04;
    font_tbl[0x26].font_y4 = 0x15;    font_tbl[0x36].font_y4 = 0x11;    font_tbl[0x46].font_y4 = 0x10;    font_tbl[0x56].font_y4 = 0x11;    font_tbl[0x66].font_y4 = 0x08;    font_tbl[0x76].font_y4 = 0x11;    font_tbl[0xA6].font_y4 = 0x01;    font_tbl[0xB6].font_y4 = 0x09;    font_tbl[0xC6].font_y4 = 0x00;    font_tbl[0xD6].font_y4 = 0x01;    font_tbl[0xE6].font_y4 = 0x11;    font_tbl[0xF6].font_y4 = 0x08;
    font_tbl[0x26].font_y5 = 0x12;    font_tbl[0x36].font_y5 = 0x11;    font_tbl[0x46].font_y5 = 0x10;    font_tbl[0x56].font_y5 = 0x0a;    font_tbl[0x66].font_y5 = 0x08;    font_tbl[0x76].font_y5 = 0x0a;    font_tbl[0xA6].font_y5 = 0x02;    font_tbl[0xB6].font_y5 = 0x09;    font_tbl[0xC6].font_y5 = 0x00;    font_tbl[0xD6].font_y5 = 0x01;    font_tbl[0xE6].font_y5 = 0x11;    font_tbl[0xF6].font_y5 = 0x10;    
    font_tbl[0x26].font_y6 = 0x0d;    font_tbl[0x36].font_y6 = 0x0e;    font_tbl[0x46].font_y6 = 0x10;    font_tbl[0x56].font_y6 = 0x04;    font_tbl[0x66].font_y6 = 0x08;    font_tbl[0x76].font_y6 = 0x04;    font_tbl[0xA6].font_y6 = 0x04;    font_tbl[0xB6].font_y6 = 0x12;    font_tbl[0xC6].font_y6 = 0x1f;    font_tbl[0xD6].font_y6 = 0x1f;    font_tbl[0xE6].font_y6 = 0x1e;    font_tbl[0xF6].font_y6 = 0x1f;    
    font_tbl[0x26].font_y7 = 0x00;    font_tbl[0x36].font_y7 = 0x00;    font_tbl[0x46].font_y7 = 0x00;    font_tbl[0x56].font_y7 = 0x00;    font_tbl[0x66].font_y7 = 0x00;    font_tbl[0x76].font_y7 = 0x00;    font_tbl[0xA6].font_y7 = 0x00;    font_tbl[0xB6].font_y7 = 0x00;    font_tbl[0xC6].font_y7 = 0x00;    font_tbl[0xD6].font_y7 = 0x00;    font_tbl[0xE6].font_y7 = 0x10;    font_tbl[0xF6].font_y7 = 0x00;        

    // '                              // 7                              // G                              // W                              // g                              // w                              // japan                          // japan                          // japan                          // japan                          // q                              // pi
    font_tbl[0x27].font_y0 = 0x0c;    font_tbl[0x37].font_y0 = 0x1f;    font_tbl[0x47].font_y0 = 0x0e;    font_tbl[0x57].font_y0 = 0x11;    font_tbl[0x67].font_y0 = 0x00;    font_tbl[0x77].font_y0 = 0x00;    font_tbl[0xA7].font_y0 = 0x00;    font_tbl[0xB7].font_y0 = 0x04;    font_tbl[0xC7].font_y0 = 0x00;    font_tbl[0xD7].font_y0 = 0x0e;    font_tbl[0xE7].font_y0 = 0x00;    font_tbl[0xF7].font_y0 = 0x00;
    font_tbl[0x27].font_y1 = 0x04;    font_tbl[0x37].font_y1 = 0x01;    font_tbl[0x47].font_y1 = 0x11;    font_tbl[0x57].font_y1 = 0x11;    font_tbl[0x67].font_y1 = 0x0f;    font_tbl[0x77].font_y1 = 0x00;    font_tbl[0xA7].font_y1 = 0x00;    font_tbl[0xB7].font_y1 = 0x1f;    font_tbl[0xC7].font_y1 = 0x1f;    font_tbl[0xD7].font_y1 = 0x00;    font_tbl[0xE7].font_y1 = 0x00;    font_tbl[0xF7].font_y1 = 0x00;
    font_tbl[0x27].font_y2 = 0x08;    font_tbl[0x37].font_y2 = 0x02;    font_tbl[0x47].font_y2 = 0x10;    font_tbl[0x57].font_y2 = 0x11;    font_tbl[0x67].font_y2 = 0x11;    font_tbl[0x77].font_y2 = 0x11;    font_tbl[0xA7].font_y2 = 0x1f;    font_tbl[0xB7].font_y2 = 0x04;    font_tbl[0xC7].font_y2 = 0x01;    font_tbl[0xD7].font_y2 = 0x1f;    font_tbl[0xE7].font_y2 = 0x0f;    font_tbl[0xF7].font_y2 = 0x1f;
    font_tbl[0x27].font_y3 = 0x00;    font_tbl[0x37].font_y3 = 0x04;    font_tbl[0x47].font_y3 = 0x17;    font_tbl[0x57].font_y3 = 0x15;    font_tbl[0x67].font_y3 = 0x11;    font_tbl[0x77].font_y3 = 0x11;    font_tbl[0xA7].font_y3 = 0x01;    font_tbl[0xB7].font_y3 = 0x1f;    font_tbl[0xC7].font_y3 = 0x0a;    font_tbl[0xD7].font_y3 = 0x01;    font_tbl[0xE7].font_y3 = 0x11;    font_tbl[0xF7].font_y3 = 0x0a;
    font_tbl[0x27].font_y4 = 0x00;    font_tbl[0x37].font_y4 = 0x08;    font_tbl[0x47].font_y4 = 0x11;    font_tbl[0x57].font_y4 = 0x15;    font_tbl[0x67].font_y4 = 0x0f;    font_tbl[0x77].font_y4 = 0x15;    font_tbl[0xA7].font_y4 = 0x06;    font_tbl[0xB7].font_y4 = 0x04;    font_tbl[0xC7].font_y4 = 0x04;    font_tbl[0xD7].font_y4 = 0x01;    font_tbl[0xE7].font_y4 = 0x11;    font_tbl[0xF7].font_y4 = 0x0a;
    font_tbl[0x27].font_y5 = 0x00;    font_tbl[0x37].font_y5 = 0x08;    font_tbl[0x47].font_y5 = 0x11;    font_tbl[0x57].font_y5 = 0x15;    font_tbl[0x67].font_y5 = 0x01;    font_tbl[0x77].font_y5 = 0x15;    font_tbl[0xA7].font_y5 = 0x04;    font_tbl[0xB7].font_y5 = 0x04;    font_tbl[0xC7].font_y5 = 0x0a;    font_tbl[0xD7].font_y5 = 0x02;    font_tbl[0xE7].font_y5 = 0x11;    font_tbl[0xF7].font_y5 = 0x0a;    
    font_tbl[0x27].font_y6 = 0x00;    font_tbl[0x37].font_y6 = 0x08;    font_tbl[0x47].font_y6 = 0x0f;    font_tbl[0x57].font_y6 = 0x0a;    font_tbl[0x67].font_y6 = 0x0e;    font_tbl[0x77].font_y6 = 0x0a;    font_tbl[0xA7].font_y6 = 0x08;    font_tbl[0xB7].font_y6 = 0x04;    font_tbl[0xC7].font_y6 = 0x10;    font_tbl[0xD7].font_y6 = 0x04;    font_tbl[0xE7].font_y6 = 0x0f;    font_tbl[0xF7].font_y6 = 0x13;    
    font_tbl[0x27].font_y7 = 0x00;    font_tbl[0x37].font_y7 = 0x00;    font_tbl[0x47].font_y7 = 0x00;    font_tbl[0x57].font_y7 = 0x00;    font_tbl[0x67].font_y7 = 0x00;    font_tbl[0x77].font_y7 = 0x00;    font_tbl[0xA7].font_y7 = 0x00;    font_tbl[0xB7].font_y7 = 0x00;    font_tbl[0xC7].font_y7 = 0x00;    font_tbl[0xD7].font_y7 = 0x00;    font_tbl[0xE7].font_y7 = 0x01;    font_tbl[0xF7].font_y7 = 0x00;        

    // (                              // 8                              // H                              // X                              // h                              // x                              // japan                          // japan                          // japan                          // japan                          // root                           // x with bar
    font_tbl[0x28].font_y0 = 0x02;    font_tbl[0x38].font_y0 = 0x0e;    font_tbl[0x48].font_y0 = 0x11;    font_tbl[0x58].font_y0 = 0x11;    font_tbl[0x68].font_y0 = 0x10;    font_tbl[0x78].font_y0 = 0x00;    font_tbl[0xA8].font_y0 = 0x00;    font_tbl[0xB8].font_y0 = 0x00;    font_tbl[0xC8].font_y0 = 0x04;    font_tbl[0xD8].font_y0 = 0x12;    font_tbl[0xE8].font_y0 = 0x00;    font_tbl[0xF8].font_y0 = 0x1f;
    font_tbl[0x28].font_y1 = 0x04;    font_tbl[0x38].font_y1 = 0x11;    font_tbl[0x48].font_y1 = 0x11;    font_tbl[0x58].font_y1 = 0x11;    font_tbl[0x68].font_y1 = 0x10;    font_tbl[0x78].font_y1 = 0x00;    font_tbl[0xA8].font_y1 = 0x00;    font_tbl[0xB8].font_y1 = 0x0f;    font_tbl[0xC8].font_y1 = 0x1f;    font_tbl[0xD8].font_y1 = 0x12;    font_tbl[0xE8].font_y1 = 0x00;    font_tbl[0xF8].font_y1 = 0x00;
    font_tbl[0x28].font_y2 = 0x08;    font_tbl[0x38].font_y2 = 0x11;    font_tbl[0x48].font_y2 = 0x11;    font_tbl[0x58].font_y2 = 0x0a;    font_tbl[0x68].font_y2 = 0x16;    font_tbl[0x78].font_y2 = 0x11;    font_tbl[0xA8].font_y2 = 0x02;    font_tbl[0xB8].font_y2 = 0x09;    font_tbl[0xC8].font_y2 = 0x02;    font_tbl[0xD8].font_y2 = 0x12;    font_tbl[0xE8].font_y2 = 0x07;    font_tbl[0xF8].font_y2 = 0x11;
    font_tbl[0x28].font_y3 = 0x08;    font_tbl[0x38].font_y3 = 0x0e;    font_tbl[0x48].font_y3 = 0x1f;    font_tbl[0x58].font_y3 = 0x04;    font_tbl[0x68].font_y3 = 0x19;    font_tbl[0x78].font_y3 = 0x0a;    font_tbl[0xA8].font_y3 = 0x04;    font_tbl[0xB8].font_y3 = 0x11;    font_tbl[0xC8].font_y3 = 0x04;    font_tbl[0xD8].font_y3 = 0x12;    font_tbl[0xE8].font_y3 = 0x04;    font_tbl[0xF8].font_y3 = 0x0a;
    font_tbl[0x28].font_y4 = 0x08;    font_tbl[0x38].font_y4 = 0x11;    font_tbl[0x48].font_y4 = 0x11;    font_tbl[0x58].font_y4 = 0x0a;    font_tbl[0x68].font_y4 = 0x11;    font_tbl[0x78].font_y4 = 0x04;    font_tbl[0xA8].font_y4 = 0x0c;    font_tbl[0xB8].font_y4 = 0x01;    font_tbl[0xC8].font_y4 = 0x0e;    font_tbl[0xD8].font_y4 = 0x02;    font_tbl[0xE8].font_y4 = 0x04;    font_tbl[0xF8].font_y4 = 0x04;
    font_tbl[0x28].font_y5 = 0x04;    font_tbl[0x38].font_y5 = 0x11;    font_tbl[0x48].font_y5 = 0x11;    font_tbl[0x58].font_y5 = 0x11;    font_tbl[0x68].font_y5 = 0x11;    font_tbl[0x78].font_y5 = 0x0a;    font_tbl[0xA8].font_y5 = 0x14;    font_tbl[0xB8].font_y5 = 0x02;    font_tbl[0xC8].font_y5 = 0x15;    font_tbl[0xD8].font_y5 = 0x04;    font_tbl[0xE8].font_y5 = 0x14;    font_tbl[0xF8].font_y5 = 0x0a;    
    font_tbl[0x28].font_y6 = 0x02;    font_tbl[0x38].font_y6 = 0x0e;    font_tbl[0x48].font_y6 = 0x11;    font_tbl[0x58].font_y6 = 0x11;    font_tbl[0x68].font_y6 = 0x11;    font_tbl[0x78].font_y6 = 0x11;    font_tbl[0xA8].font_y6 = 0x04;    font_tbl[0xB8].font_y6 = 0x0c;    font_tbl[0xC8].font_y6 = 0x04;    font_tbl[0xD8].font_y6 = 0x08;    font_tbl[0xE8].font_y6 = 0x08;    font_tbl[0xF8].font_y6 = 0x11;    
    font_tbl[0x28].font_y7 = 0x00;    font_tbl[0x38].font_y7 = 0x00;    font_tbl[0x48].font_y7 = 0x00;    font_tbl[0x58].font_y7 = 0x00;    font_tbl[0x68].font_y7 = 0x00;    font_tbl[0x78].font_y7 = 0x00;    font_tbl[0xA8].font_y7 = 0x00;    font_tbl[0xB8].font_y7 = 0x00;    font_tbl[0xC8].font_y7 = 0x00;    font_tbl[0xD8].font_y7 = 0x00;    font_tbl[0xE8].font_y7 = 0x00;    font_tbl[0xF8].font_y7 = 0x00;        

    // )                              // 9                              // I                              // Y                              // i                              // y                              // japan                          // japan                          // japan                          // japan                          // unknown character              // unknown character
    font_tbl[0x29].font_y0 = 0x08;    font_tbl[0x39].font_y0 = 0x0e;    font_tbl[0x49].font_y0 = 0x0e;    font_tbl[0x59].font_y0 = 0x11;    font_tbl[0x69].font_y0 = 0x04;    font_tbl[0x79].font_y0 = 0x00;    font_tbl[0xA9].font_y0 = 0x00;    font_tbl[0xB9].font_y0 = 0x08;    font_tbl[0xC9].font_y0 = 0x02;    font_tbl[0xD9].font_y0 = 0x00;    font_tbl[0xE9].font_y0 = 0x02;    font_tbl[0xF9].font_y0 = 0x00;
    font_tbl[0x29].font_y1 = 0x04;    font_tbl[0x39].font_y1 = 0x11;    font_tbl[0x49].font_y1 = 0x04;    font_tbl[0x59].font_y1 = 0x11;    font_tbl[0x69].font_y1 = 0x00;    font_tbl[0x79].font_y1 = 0x00;    font_tbl[0xA9].font_y1 = 0x00;    font_tbl[0xB9].font_y1 = 0x0f;    font_tbl[0xC9].font_y1 = 0x02;    font_tbl[0xD9].font_y1 = 0x04;    font_tbl[0xE9].font_y1 = 0x1a;    font_tbl[0xF9].font_y1 = 0x00;
    font_tbl[0x29].font_y2 = 0x02;    font_tbl[0x39].font_y2 = 0x11;    font_tbl[0x49].font_y2 = 0x04;    font_tbl[0x59].font_y2 = 0x11;    font_tbl[0x69].font_y2 = 0x0c;    font_tbl[0x79].font_y2 = 0x11;    font_tbl[0xA9].font_y2 = 0x04;    font_tbl[0xB9].font_y2 = 0x12;    font_tbl[0xC9].font_y2 = 0x02;    font_tbl[0xD9].font_y2 = 0x14;    font_tbl[0xE9].font_y2 = 0x02;    font_tbl[0xF9].font_y2 = 0x11;
    font_tbl[0x29].font_y3 = 0x02;    font_tbl[0x39].font_y3 = 0x0f;    font_tbl[0x49].font_y3 = 0x04;    font_tbl[0x59].font_y3 = 0x0a;    font_tbl[0x69].font_y3 = 0x04;    font_tbl[0x79].font_y3 = 0x11;    font_tbl[0xA9].font_y3 = 0x0f;    font_tbl[0xB9].font_y3 = 0x02;    font_tbl[0xC9].font_y3 = 0x02;    font_tbl[0xD9].font_y3 = 0x14;    font_tbl[0xE9].font_y3 = 0x00;    font_tbl[0xF9].font_y3 = 0x11;
    font_tbl[0x29].font_y4 = 0x02;    font_tbl[0x39].font_y4 = 0x01;    font_tbl[0x49].font_y4 = 0x04;    font_tbl[0x59].font_y4 = 0x04;    font_tbl[0x69].font_y4 = 0x04;    font_tbl[0x79].font_y4 = 0x0f;    font_tbl[0xA9].font_y4 = 0x11;    font_tbl[0xB9].font_y4 = 0x02;    font_tbl[0xC9].font_y4 = 0x02;    font_tbl[0xD9].font_y4 = 0x15;    font_tbl[0xE9].font_y4 = 0x00;    font_tbl[0xF9].font_y4 = 0x11;
    font_tbl[0x29].font_y5 = 0x04;    font_tbl[0x39].font_y5 = 0x02;    font_tbl[0x49].font_y5 = 0x04;    font_tbl[0x59].font_y5 = 0x04;    font_tbl[0x69].font_y5 = 0x04;    font_tbl[0x79].font_y5 = 0x01;    font_tbl[0xA9].font_y5 = 0x01;    font_tbl[0xB9].font_y5 = 0x02;    font_tbl[0xC9].font_y5 = 0x04;    font_tbl[0xD9].font_y5 = 0x15;    font_tbl[0xE9].font_y5 = 0x00;    font_tbl[0xF9].font_y5 = 0x11;    
    font_tbl[0x29].font_y6 = 0x08;    font_tbl[0x39].font_y6 = 0x0c;    font_tbl[0x49].font_y6 = 0x0e;    font_tbl[0x59].font_y6 = 0x04;    font_tbl[0x69].font_y6 = 0x0e;    font_tbl[0x79].font_y6 = 0x0e;    font_tbl[0xA9].font_y6 = 0x06;    font_tbl[0xB9].font_y6 = 0x04;    font_tbl[0xC9].font_y6 = 0x08;    font_tbl[0xD9].font_y6 = 0x16;    font_tbl[0xE9].font_y6 = 0x00;    font_tbl[0xF9].font_y6 = 0x0f;    
    font_tbl[0x29].font_y7 = 0x00;    font_tbl[0x39].font_y7 = 0x00;    font_tbl[0x49].font_y7 = 0x00;    font_tbl[0x59].font_y7 = 0x00;    font_tbl[0x69].font_y7 = 0x00;    font_tbl[0x79].font_y7 = 0x00;    font_tbl[0xA9].font_y7 = 0x00;    font_tbl[0xB9].font_y7 = 0x00;    font_tbl[0xC9].font_y7 = 0x00;    font_tbl[0xD9].font_y7 = 0x00;    font_tbl[0xE9].font_y7 = 0x00;    font_tbl[0xF9].font_y7 = 0x01;        

    // *                              // :                              // J                              // Z                              // j                              // z                              // japan                          // japan                          // japan                          // japan                          // long j                         // unknown character
    font_tbl[0x2A].font_y0 = 0x00;    font_tbl[0x3A].font_y0 = 0x00;    font_tbl[0x4A].font_y0 = 0x07;    font_tbl[0x5A].font_y0 = 0x1f;    font_tbl[0x6A].font_y0 = 0x02;    font_tbl[0x7A].font_y0 = 0x00;    font_tbl[0xAA].font_y0 = 0x00;    font_tbl[0xBA].font_y0 = 0x00;    font_tbl[0xCA].font_y0 = 0x00;    font_tbl[0xDA].font_y0 = 0x00;    font_tbl[0xEA].font_y0 = 0x02;    font_tbl[0xFA].font_y0 = 0x00;
    font_tbl[0x2A].font_y1 = 0x04;    font_tbl[0x3A].font_y1 = 0x0c;    font_tbl[0x4A].font_y1 = 0x02;    font_tbl[0x5A].font_y1 = 0x01;    font_tbl[0x6A].font_y1 = 0x00;    font_tbl[0x7A].font_y1 = 0x00;    font_tbl[0xAA].font_y1 = 0x00;    font_tbl[0xBA].font_y1 = 0x1f;    font_tbl[0xCA].font_y1 = 0x04;    font_tbl[0xDA].font_y1 = 0x10;    font_tbl[0xEA].font_y1 = 0x00;    font_tbl[0xFA].font_y1 = 0x01;
    font_tbl[0x2A].font_y2 = 0x15;    font_tbl[0x3A].font_y2 = 0x0c;    font_tbl[0x4A].font_y2 = 0x02;    font_tbl[0x5A].font_y2 = 0x02;    font_tbl[0x6A].font_y2 = 0x06;    font_tbl[0x7A].font_y2 = 0x1f;    font_tbl[0xAA].font_y2 = 0x00;    font_tbl[0xBA].font_y2 = 0x01;    font_tbl[0xCA].font_y2 = 0x02;    font_tbl[0xDA].font_y2 = 0x10;    font_tbl[0xEA].font_y2 = 0x06;    font_tbl[0xFA].font_y2 = 0x1e;
    font_tbl[0x2A].font_y3 = 0x0e;    font_tbl[0x3A].font_y3 = 0x00;    font_tbl[0x4A].font_y3 = 0x02;    font_tbl[0x5A].font_y3 = 0x04;    font_tbl[0x6A].font_y3 = 0x02;    font_tbl[0x7A].font_y3 = 0x02;    font_tbl[0xAA].font_y3 = 0x1f;    font_tbl[0xBA].font_y3 = 0x01;    font_tbl[0xCA].font_y3 = 0x11;    font_tbl[0xDA].font_y3 = 0x11;    font_tbl[0xEA].font_y3 = 0x02;    font_tbl[0xFA].font_y3 = 0x04;
    font_tbl[0x2A].font_y4 = 0x15;    font_tbl[0x3A].font_y4 = 0x0c;    font_tbl[0x4A].font_y4 = 0x02;    font_tbl[0x5A].font_y4 = 0x08;    font_tbl[0x6A].font_y4 = 0x02;    font_tbl[0x7A].font_y4 = 0x04;    font_tbl[0xAA].font_y4 = 0x04;    font_tbl[0xBA].font_y4 = 0x01;    font_tbl[0xCA].font_y4 = 0x11;    font_tbl[0xDA].font_y4 = 0x12;    font_tbl[0xEA].font_y4 = 0x02;    font_tbl[0xFA].font_y4 = 0x1f;
    font_tbl[0x2A].font_y5 = 0x04;    font_tbl[0x3A].font_y5 = 0x0c;    font_tbl[0x4A].font_y5 = 0x12;    font_tbl[0x5A].font_y5 = 0x10;    font_tbl[0x6A].font_y5 = 0x12;    font_tbl[0x7A].font_y5 = 0x08;    font_tbl[0xAA].font_y5 = 0x04;    font_tbl[0xBA].font_y5 = 0x01;    font_tbl[0xCA].font_y5 = 0x11;    font_tbl[0xDA].font_y5 = 0x14;    font_tbl[0xEA].font_y5 = 0x02;    font_tbl[0xFA].font_y5 = 0x04;    
    font_tbl[0x2A].font_y6 = 0x00;    font_tbl[0x3A].font_y6 = 0x00;    font_tbl[0x4A].font_y6 = 0x0c;    font_tbl[0x5A].font_y6 = 0x1f;    font_tbl[0x6A].font_y6 = 0x0c;    font_tbl[0x7A].font_y6 = 0x1f;    font_tbl[0xAA].font_y6 = 0x1f;    font_tbl[0xBA].font_y6 = 0x1f;    font_tbl[0xCA].font_y6 = 0x11;    font_tbl[0xDA].font_y6 = 0x18;    font_tbl[0xEA].font_y6 = 0x02;    font_tbl[0xFA].font_y6 = 0x04;    
    font_tbl[0x2A].font_y7 = 0x00;    font_tbl[0x3A].font_y7 = 0x00;    font_tbl[0x4A].font_y7 = 0x00;    font_tbl[0x5A].font_y7 = 0x00;    font_tbl[0x6A].font_y7 = 0x00;    font_tbl[0x7A].font_y7 = 0x00;    font_tbl[0xAA].font_y7 = 0x00;    font_tbl[0xBA].font_y7 = 0x00;    font_tbl[0xCA].font_y7 = 0x00;    font_tbl[0xDA].font_y7 = 0x00;    font_tbl[0xEA].font_y7 = 0x02;    font_tbl[0xFA].font_y7 = 0x00;        

    // +                              // ;                              // K                              // [                              // k                              // {                              // japan                          // japan                          // japan                          // japan                          // star                           // unknown character
    font_tbl[0x2B].font_y0 = 0x00;    font_tbl[0x3B].font_y0 = 0x00;    font_tbl[0x4B].font_y0 = 0x11;    font_tbl[0x5B].font_y0 = 0x0e;    font_tbl[0x6B].font_y0 = 0x10;    font_tbl[0x7B].font_y0 = 0x02;    font_tbl[0xAB].font_y0 = 0x00;    font_tbl[0xBB].font_y0 = 0x0a;    font_tbl[0xCB].font_y0 = 0x10;    font_tbl[0xDB].font_y0 = 0x00;    font_tbl[0xEB].font_y0 = 0x00;    font_tbl[0xFB].font_y0 = 0x00;
    font_tbl[0x2B].font_y1 = 0x04;    font_tbl[0x3B].font_y1 = 0x0c;    font_tbl[0x4B].font_y1 = 0x12;    font_tbl[0x5B].font_y1 = 0x08;    font_tbl[0x6B].font_y1 = 0x10;    font_tbl[0x7B].font_y1 = 0x04;    font_tbl[0xAB].font_y1 = 0x00;    font_tbl[0xBB].font_y1 = 0x1f;    font_tbl[0xCB].font_y1 = 0x10;    font_tbl[0xDB].font_y1 = 0x1f;    font_tbl[0xEB].font_y1 = 0x14;    font_tbl[0xFB].font_y1 = 0x00;
    font_tbl[0x2B].font_y2 = 0x04;    font_tbl[0x3B].font_y2 = 0x0c;    font_tbl[0x4B].font_y2 = 0x14;    font_tbl[0x5B].font_y2 = 0x08;    font_tbl[0x6B].font_y2 = 0x12;    font_tbl[0x7B].font_y2 = 0x04;    font_tbl[0xAB].font_y2 = 0x02;    font_tbl[0xBB].font_y2 = 0x0a;    font_tbl[0xCB].font_y2 = 0x1f;    font_tbl[0xDB].font_y2 = 0x11;    font_tbl[0xEB].font_y2 = 0x08;    font_tbl[0xFB].font_y2 = 0x1f;
    font_tbl[0x2B].font_y3 = 0x1f;    font_tbl[0x3B].font_y3 = 0x00;    font_tbl[0x4B].font_y3 = 0x18;    font_tbl[0x5B].font_y3 = 0x08;    font_tbl[0x6B].font_y3 = 0x14;    font_tbl[0x7B].font_y3 = 0x08;    font_tbl[0xAB].font_y3 = 0x1f;    font_tbl[0xBB].font_y3 = 0x0a;    font_tbl[0xCB].font_y3 = 0x10;    font_tbl[0xDB].font_y3 = 0x11;    font_tbl[0xEB].font_y3 = 0x14;    font_tbl[0xFB].font_y3 = 0x08;
    font_tbl[0x2B].font_y4 = 0x04;    font_tbl[0x3B].font_y4 = 0x0c;    font_tbl[0x4B].font_y4 = 0x14;    font_tbl[0x5B].font_y4 = 0x08;    font_tbl[0x6B].font_y4 = 0x18;    font_tbl[0x7B].font_y4 = 0x04;    font_tbl[0xAB].font_y4 = 0x06;    font_tbl[0xBB].font_y4 = 0x02;    font_tbl[0xCB].font_y4 = 0x10;    font_tbl[0xDB].font_y4 = 0x11;    font_tbl[0xEB].font_y4 = 0x00;    font_tbl[0xFB].font_y4 = 0x0f;
    font_tbl[0x2B].font_y5 = 0x04;    font_tbl[0x3B].font_y5 = 0x04;    font_tbl[0x4B].font_y5 = 0x12;    font_tbl[0x5B].font_y5 = 0x08;    font_tbl[0x6B].font_y5 = 0x14;    font_tbl[0x7B].font_y5 = 0x04;    font_tbl[0xAB].font_y5 = 0x0a;    font_tbl[0xBB].font_y5 = 0x04;    font_tbl[0xCB].font_y5 = 0x10;    font_tbl[0xDB].font_y5 = 0x11;    font_tbl[0xEB].font_y5 = 0x00;    font_tbl[0xFB].font_y5 = 0x09;    
    font_tbl[0x2B].font_y6 = 0x00;    font_tbl[0x3B].font_y6 = 0x08;    font_tbl[0x4B].font_y6 = 0x11;    font_tbl[0x5B].font_y6 = 0x0e;    font_tbl[0x6B].font_y6 = 0x12;    font_tbl[0x7B].font_y6 = 0x02;    font_tbl[0xAB].font_y6 = 0x12;    font_tbl[0xBB].font_y6 = 0x08;    font_tbl[0xCB].font_y6 = 0x0f;    font_tbl[0xDB].font_y6 = 0x1f;    font_tbl[0xEB].font_y6 = 0x00;    font_tbl[0xFB].font_y6 = 0x11;    
    font_tbl[0x2B].font_y7 = 0x00;    font_tbl[0x3B].font_y7 = 0x00;    font_tbl[0x4B].font_y7 = 0x00;    font_tbl[0x5B].font_y7 = 0x00;    font_tbl[0x6B].font_y7 = 0x00;    font_tbl[0x7B].font_y7 = 0x00;    font_tbl[0xAB].font_y7 = 0x00;    font_tbl[0xBB].font_y7 = 0x00;    font_tbl[0xCB].font_y7 = 0x00;    font_tbl[0xDB].font_y7 = 0x00;    font_tbl[0xEB].font_y7 = 0x00;    font_tbl[0xFB].font_y7 = 0x00;        

    // ,                              // <                              // L                              // ¥                              // l                              // |                              // japan                          // japan                          // japan                          // japan                          // unknown character              // unknown character
    font_tbl[0x2C].font_y0 = 0x00;    font_tbl[0x3C].font_y0 = 0x02;    font_tbl[0x4C].font_y0 = 0x10;    font_tbl[0x5C].font_y0 = 0x11;    font_tbl[0x6C].font_y0 = 0x0c;    font_tbl[0x7C].font_y0 = 0x04;    font_tbl[0xAC].font_y0 = 0x00;    font_tbl[0xBC].font_y0 = 0x00;    font_tbl[0xCC].font_y0 = 0x00;    font_tbl[0xDC].font_y0 = 0x00;    font_tbl[0xEC].font_y0 = 0x00;    font_tbl[0xFC].font_y0 = 0x00;
    font_tbl[0x2C].font_y1 = 0x00;    font_tbl[0x3C].font_y1 = 0x04;    font_tbl[0x4C].font_y1 = 0x10;    font_tbl[0x5C].font_y1 = 0x0a;    font_tbl[0x6C].font_y1 = 0x04;    font_tbl[0x7C].font_y1 = 0x04;    font_tbl[0xAC].font_y1 = 0x00;    font_tbl[0xBC].font_y1 = 0x18;    font_tbl[0xCC].font_y1 = 0x1f;    font_tbl[0xDC].font_y1 = 0x1f;    font_tbl[0xEC].font_y1 = 0x04;    font_tbl[0xFC].font_y1 = 0x00;
    font_tbl[0x2C].font_y2 = 0x00;    font_tbl[0x3C].font_y2 = 0x08;    font_tbl[0x4C].font_y2 = 0x10;    font_tbl[0x5C].font_y2 = 0x1f;    font_tbl[0x6C].font_y2 = 0x04;    font_tbl[0x7C].font_y2 = 0x04;    font_tbl[0xAC].font_y2 = 0x08;    font_tbl[0xBC].font_y2 = 0x01;    font_tbl[0xCC].font_y2 = 0x01;    font_tbl[0xDC].font_y2 = 0x11;    font_tbl[0xEC].font_y2 = 0x0e;    font_tbl[0xFC].font_y2 = 0x1f;
    font_tbl[0x2C].font_y3 = 0x00;    font_tbl[0x3C].font_y3 = 0x10;    font_tbl[0x4C].font_y3 = 0x10;    font_tbl[0x5C].font_y3 = 0x04;    font_tbl[0x6C].font_y3 = 0x04;    font_tbl[0x7C].font_y3 = 0x04;    font_tbl[0xAC].font_y3 = 0x1f;    font_tbl[0xBC].font_y3 = 0x19;    font_tbl[0xCC].font_y3 = 0x01;    font_tbl[0xDC].font_y3 = 0x11;    font_tbl[0xEC].font_y3 = 0x14;    font_tbl[0xFC].font_y3 = 0x15;
    font_tbl[0x2C].font_y4 = 0x0c;    font_tbl[0x3C].font_y4 = 0x08;    font_tbl[0x4C].font_y4 = 0x10;    font_tbl[0x5C].font_y4 = 0x1f;    font_tbl[0x6C].font_y4 = 0x04;    font_tbl[0x7C].font_y4 = 0x04;    font_tbl[0xAC].font_y4 = 0x09;    font_tbl[0xBC].font_y4 = 0x01;    font_tbl[0xCC].font_y4 = 0x01;    font_tbl[0xDC].font_y4 = 0x01;    font_tbl[0xEC].font_y4 = 0x15;    font_tbl[0xFC].font_y4 = 0x1f;
    font_tbl[0x2C].font_y5 = 0x04;    font_tbl[0x3C].font_y5 = 0x04;    font_tbl[0x4C].font_y5 = 0x10;    font_tbl[0x5C].font_y5 = 0x04;    font_tbl[0x6C].font_y5 = 0x04;    font_tbl[0x7C].font_y5 = 0x04;    font_tbl[0xAC].font_y5 = 0x0a;    font_tbl[0xBC].font_y5 = 0x02;    font_tbl[0xCC].font_y5 = 0x02;    font_tbl[0xDC].font_y5 = 0x02;    font_tbl[0xEC].font_y5 = 0x0e;    font_tbl[0xFC].font_y5 = 0x11;    
    font_tbl[0x2C].font_y6 = 0x08;    font_tbl[0x3C].font_y6 = 0x02;    font_tbl[0x4C].font_y6 = 0x1f;    font_tbl[0x5C].font_y6 = 0x04;    font_tbl[0x6C].font_y6 = 0x0e;    font_tbl[0x7C].font_y6 = 0x04;    font_tbl[0xAC].font_y6 = 0x08;    font_tbl[0xBC].font_y6 = 0x1c;    font_tbl[0xCC].font_y6 = 0x0c;    font_tbl[0xDC].font_y6 = 0x04;    font_tbl[0xEC].font_y6 = 0x04;    font_tbl[0xFC].font_y6 = 0x11;    
    font_tbl[0x2C].font_y7 = 0x00;    font_tbl[0x3C].font_y7 = 0x00;    font_tbl[0x4C].font_y7 = 0x00;    font_tbl[0x5C].font_y7 = 0x00;    font_tbl[0x6C].font_y7 = 0x00;    font_tbl[0x7C].font_y7 = 0x00;    font_tbl[0xAC].font_y7 = 0x00;    font_tbl[0xBC].font_y7 = 0x00;    font_tbl[0xCC].font_y7 = 0x00;    font_tbl[0xDC].font_y7 = 0x00;    font_tbl[0xEC].font_y7 = 0x00;    font_tbl[0xFC].font_y7 = 0x00;        

    // -                              // =                              // M                              // ]                              // m                              // }                              // japan                          // japan                          // japan                          // japan                          // £                              // ungültig
    font_tbl[0x2D].font_y0 = 0x00;    font_tbl[0x3D].font_y0 = 0x00;    font_tbl[0x4D].font_y0 = 0x11;    font_tbl[0x5D].font_y0 = 0x0e;    font_tbl[0x6D].font_y0 = 0x00;    font_tbl[0x7D].font_y0 = 0x08;    font_tbl[0xAD].font_y0 = 0x00;    font_tbl[0xBD].font_y0 = 0x00;    font_tbl[0xCD].font_y0 = 0x00;    font_tbl[0xDD].font_y0 = 0x00;    font_tbl[0xED].font_y0 = 0x08;    font_tbl[0xFD].font_y0 = 0x00;
    font_tbl[0x2D].font_y1 = 0x00;    font_tbl[0x3D].font_y1 = 0x00;    font_tbl[0x4D].font_y1 = 0x1b;    font_tbl[0x5D].font_y1 = 0x02;    font_tbl[0x6D].font_y1 = 0x00;    font_tbl[0x7D].font_y1 = 0x04;    font_tbl[0xAD].font_y1 = 0x00;    font_tbl[0xBD].font_y1 = 0x1f;    font_tbl[0xCD].font_y1 = 0x08;    font_tbl[0xDD].font_y1 = 0x18;    font_tbl[0xED].font_y1 = 0x08;    font_tbl[0xFD].font_y1 = 0x00;
    font_tbl[0x2D].font_y2 = 0x00;    font_tbl[0x3D].font_y2 = 0x1f;    font_tbl[0x4D].font_y2 = 0x15;    font_tbl[0x5D].font_y2 = 0x02;    font_tbl[0x6D].font_y2 = 0x1a;    font_tbl[0x7D].font_y2 = 0x04;    font_tbl[0xAD].font_y2 = 0x00;    font_tbl[0xBD].font_y2 = 0x01;    font_tbl[0xCD].font_y2 = 0x14;    font_tbl[0xDD].font_y2 = 0x00;    font_tbl[0xED].font_y2 = 0x1c;    font_tbl[0xFD].font_y2 = 0x04;
    font_tbl[0x2D].font_y3 = 0x1f;    font_tbl[0x3D].font_y3 = 0x00;    font_tbl[0x4D].font_y3 = 0x15;    font_tbl[0x5D].font_y3 = 0x02;    font_tbl[0x6D].font_y3 = 0x15;    font_tbl[0x7D].font_y3 = 0x02;    font_tbl[0xAD].font_y3 = 0x0e;    font_tbl[0xBD].font_y3 = 0x02;    font_tbl[0xCD].font_y3 = 0x02;    font_tbl[0xDD].font_y3 = 0x01;    font_tbl[0xED].font_y3 = 0x08;    font_tbl[0xFD].font_y3 = 0x00;
    font_tbl[0x2D].font_y4 = 0x00;    font_tbl[0x3D].font_y4 = 0x1f;    font_tbl[0x4D].font_y4 = 0x11;    font_tbl[0x5D].font_y4 = 0x02;    font_tbl[0x6D].font_y4 = 0x15;    font_tbl[0x7D].font_y4 = 0x04;    font_tbl[0xAD].font_y4 = 0x02;    font_tbl[0xBD].font_y4 = 0x04;    font_tbl[0xCD].font_y4 = 0x01;    font_tbl[0xDD].font_y4 = 0x01;    font_tbl[0xED].font_y4 = 0x1c;    font_tbl[0xFD].font_y4 = 0x1f;
    font_tbl[0x2D].font_y5 = 0x00;    font_tbl[0x3D].font_y5 = 0x00;    font_tbl[0x4D].font_y5 = 0x11;    font_tbl[0x5D].font_y5 = 0x02;    font_tbl[0x6D].font_y5 = 0x11;    font_tbl[0x7D].font_y5 = 0x04;    font_tbl[0xAD].font_y5 = 0x02;    font_tbl[0xBD].font_y5 = 0x0a;    font_tbl[0xCD].font_y5 = 0x01;    font_tbl[0xDD].font_y5 = 0x02;    font_tbl[0xED].font_y5 = 0x08;    font_tbl[0xFD].font_y5 = 0x00;    
    font_tbl[0x2D].font_y6 = 0x00;    font_tbl[0x3D].font_y6 = 0x00;    font_tbl[0x4D].font_y6 = 0x11;    font_tbl[0x5D].font_y6 = 0x0e;    font_tbl[0x6D].font_y6 = 0x11;    font_tbl[0x7D].font_y6 = 0x08;    font_tbl[0xAD].font_y6 = 0x1f;    font_tbl[0xBD].font_y6 = 0x11;    font_tbl[0xCD].font_y6 = 0x00;    font_tbl[0xDD].font_y6 = 0x1c;    font_tbl[0xED].font_y6 = 0x0f;    font_tbl[0xFD].font_y6 = 0x04;    
    font_tbl[0x2D].font_y7 = 0x00;    font_tbl[0x3D].font_y7 = 0x00;    font_tbl[0x4D].font_y7 = 0x00;    font_tbl[0x5D].font_y7 = 0x00;    font_tbl[0x6D].font_y7 = 0x00;    font_tbl[0x7D].font_y7 = 0x00;    font_tbl[0xAD].font_y7 = 0x00;    font_tbl[0xBD].font_y7 = 0x00;    font_tbl[0xCD].font_y7 = 0x00;    font_tbl[0xDD].font_y7 = 0x00;    font_tbl[0xED].font_y7 = 0x00;    font_tbl[0xFD].font_y7 = 0x00;        

    // .                              // >                              // N                              // ^                              // n                              // ->                             // japan                          // japan                          // japan                          // japan                          // ñ                              // underline
    font_tbl[0x2E].font_y0 = 0x00;    font_tbl[0x3E].font_y0 = 0x08;    font_tbl[0x4E].font_y0 = 0x11;    font_tbl[0x5E].font_y0 = 0x04;    font_tbl[0x6E].font_y0 = 0x00;    font_tbl[0x7E].font_y0 = 0x00;    font_tbl[0xAE].font_y0 = 0x00;    font_tbl[0xBE].font_y0 = 0x08;    font_tbl[0xCE].font_y0 = 0x04;    font_tbl[0xDE].font_y0 = 0x04;    font_tbl[0xEE].font_y0 = 0x0e;    font_tbl[0xFE].font_y0 = 0x00;
    font_tbl[0x2E].font_y1 = 0x00;    font_tbl[0x3E].font_y1 = 0x04;    font_tbl[0x4E].font_y1 = 0x11;    font_tbl[0x5E].font_y1 = 0x0a;    font_tbl[0x6E].font_y1 = 0x00;    font_tbl[0x7E].font_y1 = 0x04;    font_tbl[0xAE].font_y1 = 0x00;    font_tbl[0xBE].font_y1 = 0x1f;    font_tbl[0xCE].font_y1 = 0x1f;    font_tbl[0xDE].font_y1 = 0x12;    font_tbl[0xEE].font_y1 = 0x00;    font_tbl[0xFE].font_y1 = 0x00;
    font_tbl[0x2E].font_y2 = 0x00;    font_tbl[0x3E].font_y2 = 0x02;    font_tbl[0x4E].font_y2 = 0x19;    font_tbl[0x5E].font_y2 = 0x11;    font_tbl[0x6E].font_y2 = 0x16;    font_tbl[0x7E].font_y2 = 0x02;    font_tbl[0xAE].font_y2 = 0x1e;    font_tbl[0xBE].font_y2 = 0x09;    font_tbl[0xCE].font_y2 = 0x04;    font_tbl[0xDE].font_y2 = 0x08;    font_tbl[0xEE].font_y2 = 0x16;    font_tbl[0xFE].font_y2 = 0x00;
    font_tbl[0x2E].font_y3 = 0x00;    font_tbl[0x3E].font_y3 = 0x01;    font_tbl[0x4E].font_y3 = 0x15;    font_tbl[0x5E].font_y3 = 0x00;    font_tbl[0x6E].font_y3 = 0x19;    font_tbl[0x7E].font_y3 = 0x1f;    font_tbl[0xAE].font_y3 = 0x02;    font_tbl[0xBE].font_y3 = 0x0a;    font_tbl[0xCE].font_y3 = 0x04;    font_tbl[0xDE].font_y3 = 0x00;    font_tbl[0xEE].font_y3 = 0x19;    font_tbl[0xFE].font_y3 = 0x00;
    font_tbl[0x2E].font_y4 = 0x00;    font_tbl[0x3E].font_y4 = 0x02;    font_tbl[0x4E].font_y4 = 0x13;    font_tbl[0x5E].font_y4 = 0x00;    font_tbl[0x6E].font_y4 = 0x11;    font_tbl[0x7E].font_y4 = 0x02;    font_tbl[0xAE].font_y4 = 0x1e;    font_tbl[0xBE].font_y4 = 0x08;    font_tbl[0xCE].font_y4 = 0x15;    font_tbl[0xDE].font_y4 = 0x00;    font_tbl[0xEE].font_y4 = 0x11;    font_tbl[0xFE].font_y4 = 0x00;
    font_tbl[0x2E].font_y5 = 0x0c;    font_tbl[0x3E].font_y5 = 0x04;    font_tbl[0x4E].font_y5 = 0x11;    font_tbl[0x5E].font_y5 = 0x00;    font_tbl[0x6E].font_y5 = 0x11;    font_tbl[0x7E].font_y5 = 0x04;    font_tbl[0xAE].font_y5 = 0x02;    font_tbl[0xBE].font_y5 = 0x08;    font_tbl[0xCE].font_y5 = 0x15;    font_tbl[0xDE].font_y5 = 0x00;    font_tbl[0xEE].font_y5 = 0x11;    font_tbl[0xFE].font_y5 = 0x00;    
    font_tbl[0x2E].font_y6 = 0x0c;    font_tbl[0x3E].font_y6 = 0x08;    font_tbl[0x4E].font_y6 = 0x11;    font_tbl[0x5E].font_y6 = 0x00;    font_tbl[0x6E].font_y6 = 0x11;    font_tbl[0x7E].font_y6 = 0x00;    font_tbl[0xAE].font_y6 = 0x1e;    font_tbl[0xBE].font_y6 = 0x07;    font_tbl[0xCE].font_y6 = 0x04;    font_tbl[0xDE].font_y6 = 0x00;    font_tbl[0xEE].font_y6 = 0x11;    font_tbl[0xFE].font_y6 = 0x00;    
    font_tbl[0x2E].font_y7 = 0x00;    font_tbl[0x3E].font_y7 = 0x00;    font_tbl[0x4E].font_y7 = 0x00;    font_tbl[0x5E].font_y7 = 0x00;    font_tbl[0x6E].font_y7 = 0x00;    font_tbl[0x7E].font_y7 = 0x00;    font_tbl[0xAE].font_y7 = 0x00;    font_tbl[0xBE].font_y7 = 0x00;    font_tbl[0xCE].font_y7 = 0x00;    font_tbl[0xDE].font_y7 = 0x00;    font_tbl[0xEE].font_y7 = 0x00;    font_tbl[0xFE].font_y7 = 0x00;        

    // /                              // ?                              // O                              // _                              // o                              // <-                             // japan                          // japan                          // japan                          // japan                          // ö                              // test
    font_tbl[0x2F].font_y0 = 0x00;    font_tbl[0x3F].font_y0 = 0x0e;    font_tbl[0x4F].font_y0 = 0x0e;    font_tbl[0x5F].font_y0 = 0x00;    font_tbl[0x6F].font_y0 = 0x00;    font_tbl[0x7F].font_y0 = 0x00;    font_tbl[0xAF].font_y0 = 0x00;    font_tbl[0xBF].font_y0 = 0x00;    font_tbl[0xCF].font_y0 = 0x00;    font_tbl[0xDF].font_y0 = 0x1c;    font_tbl[0xEF].font_y0 = 0x0a;    font_tbl[0xFF].font_y0 = 0x1f;
    font_tbl[0x2F].font_y1 = 0x01;    font_tbl[0x3F].font_y1 = 0x11;    font_tbl[0x4F].font_y1 = 0x11;    font_tbl[0x5F].font_y1 = 0x00;    font_tbl[0x6F].font_y1 = 0x00;    font_tbl[0x7F].font_y1 = 0x04;    font_tbl[0xAF].font_y1 = 0x00;    font_tbl[0xBF].font_y1 = 0x11;    font_tbl[0xCF].font_y1 = 0x1f;    font_tbl[0xDF].font_y1 = 0x14;    font_tbl[0xEF].font_y1 = 0x00;    font_tbl[0xFF].font_y1 = 0x1f;
    font_tbl[0x2F].font_y2 = 0x02;    font_tbl[0x3F].font_y2 = 0x01;    font_tbl[0x4F].font_y2 = 0x11;    font_tbl[0x5F].font_y2 = 0x00;    font_tbl[0x6F].font_y2 = 0x0e;    font_tbl[0x7F].font_y2 = 0x08;    font_tbl[0xAF].font_y2 = 0x00;    font_tbl[0xBF].font_y2 = 0x11;    font_tbl[0xCF].font_y2 = 0x01;    font_tbl[0xDF].font_y2 = 0x1c;    font_tbl[0xEF].font_y2 = 0x0e;    font_tbl[0xFF].font_y2 = 0x1f;
    font_tbl[0x2F].font_y3 = 0x04;    font_tbl[0x3F].font_y3 = 0x02;    font_tbl[0x4F].font_y3 = 0x11;    font_tbl[0x5F].font_y3 = 0x00;    font_tbl[0x6F].font_y3 = 0x11;    font_tbl[0x7F].font_y3 = 0x1f;    font_tbl[0xAF].font_y3 = 0x15;    font_tbl[0xBF].font_y3 = 0x09;    font_tbl[0xCF].font_y3 = 0x01;    font_tbl[0xDF].font_y3 = 0x00;    font_tbl[0xEF].font_y3 = 0x11;    font_tbl[0xFF].font_y3 = 0x1f;
    font_tbl[0x2F].font_y4 = 0x08;    font_tbl[0x3F].font_y4 = 0x04;    font_tbl[0x4F].font_y4 = 0x11;    font_tbl[0x5F].font_y4 = 0x00;    font_tbl[0x6F].font_y4 = 0x11;    font_tbl[0x7F].font_y4 = 0x08;    font_tbl[0xAF].font_y4 = 0x15;    font_tbl[0xBF].font_y4 = 0x01;    font_tbl[0xCF].font_y4 = 0x0a;    font_tbl[0xDF].font_y4 = 0x00;    font_tbl[0xEF].font_y4 = 0x11;    font_tbl[0xFF].font_y4 = 0x1f;
    font_tbl[0x2F].font_y5 = 0x10;    font_tbl[0x3F].font_y5 = 0x00;    font_tbl[0x4F].font_y5 = 0x11;    font_tbl[0x5F].font_y5 = 0x00;    font_tbl[0x6F].font_y5 = 0x11;    font_tbl[0x7F].font_y5 = 0x04;    font_tbl[0xAF].font_y5 = 0x01;    font_tbl[0xBF].font_y5 = 0x02;    font_tbl[0xCF].font_y5 = 0x04;    font_tbl[0xDF].font_y5 = 0x00;    font_tbl[0xEF].font_y5 = 0x11;    font_tbl[0xFF].font_y5 = 0x1f;    
    font_tbl[0x2F].font_y6 = 0x00;    font_tbl[0x3F].font_y6 = 0x04;    font_tbl[0x4F].font_y6 = 0x0e;    font_tbl[0x5F].font_y6 = 0x1f;    font_tbl[0x6F].font_y6 = 0x0e;    font_tbl[0x7F].font_y6 = 0x00;    font_tbl[0xAF].font_y6 = 0x06;    font_tbl[0xBF].font_y6 = 0x0c;    font_tbl[0xCF].font_y6 = 0x02;    font_tbl[0xDF].font_y6 = 0x00;    font_tbl[0xEF].font_y6 = 0x0e;    font_tbl[0xFF].font_y6 = 0x1f;    
    font_tbl[0x2F].font_y7 = 0x00;    font_tbl[0x3F].font_y7 = 0x00;    font_tbl[0x4F].font_y7 = 0x00;    font_tbl[0x5F].font_y7 = 0x00;    font_tbl[0x6F].font_y7 = 0x00;    font_tbl[0x7F].font_y7 = 0x00;    font_tbl[0xAF].font_y7 = 0x00;    font_tbl[0xBF].font_y7 = 0x00;    font_tbl[0xCF].font_y7 = 0x00;    font_tbl[0xDF].font_y7 = 0x00;    font_tbl[0xEF].font_y7 = 0x00;    font_tbl[0xFF].font_y7 = 0x1f;        
#endif
                   
    for(UINT i = 0; i<16; i++) {                                         // invalid pattern 
        font_tbl[i] = font_tbl[0x00];
        font_tbl[i+0x80] = font_tbl[0x20];
        font_tbl[i+0x90] = font_tbl[0x20];        
    }
   
    font5x11[0xF0][0] = 0x10;    font5x11[0xF0][1] =    0x10, font5x11[0xF0][2] = 0x10; // extended charcters
    font5x11[0xF1][0] = 0x01;    font5x11[0xF1][1] =    0x01, font5x11[0xF1][2] = 0x01;
    font5x11[0xE2][0] = 0x10;    font5x11[0xE2][1] =    0x10, font5x11[0xE2][2] = 0x10;
    font5x11[0xE4][0] = 0x10;    font5x11[0xE4][1] =    0x10, font5x11[0xE4][2] = 0x10;
    font5x11[0xE6][0] = 0x10;    font5x11[0xE6][1] =    0x10, font5x11[0xE6][2] = 0x10;
    font5x11[0xE7][0] = 0x01;    font5x11[0xE7][1] =    0x01, font5x11[0xE7][2] = 0x0E;
    font5x11[0xF9][0] = 0x01;    font5x11[0xF9][1] =    0x01, font5x11[0xF9][2] = 0x0E;
    font5x11[0xEA][0] = 0x02;    font5x11[0xEA][1] =    0x12, font5x11[0xEA][2] = 0x0C;
    font5x11[0xFF][0] = 0x1F;    font5x11[0xFF][1] =    0x1F, font5x11[0xFF][2] = 0x1F;
}


// Interpretation of received command
//
static unsigned char LCDCommand(BOOL bRS, ULONG ulCmd)
{
    unsigned char ucAddr;

    if (bRS == 0) {                                                      // is is a command ?
        unsigned char i;
        if (ulCmd & LCD_DDR_RAM_ADR) {                                   // set the RAM address
            tDisplayMem.ucRAMselect = DDRAM;
            ucAddr = (unsigned char) (((ulCmd - LCD_DDR_RAM_ADR) & 0xff));
           
            tDisplayMem.ucLocY = 0;
            for (i = 0; i < (tDisplayMem.ucLines-1); i++) {
                if (ucAddr >= LCD_Info.uiDDRAMStartLine[i + 1]) {
                    tDisplayMem.ucLocY++;
                    ucAddr -= LCD_Info.uiDDRAMStartLine[i + 1];
                }
                else {
                    break;
                }
            }
            tDisplayMem.ucLocX = ucAddr;
            return (tDisplayMem.ddrRam[ucAddr]);                         // {17} return the RAM content in order to support reading this value
        }
        else if(ulCmd & LCD_CG_RAM_ADR) {
            tDisplayMem.ucRAMselect = CGRAM;
            tDisplayMem.ucCGRAMaddr = (unsigned char)(ulCmd - LCD_CG_RAM_ADR);
        }
        else if (ulCmd & LCD_FUNCTION_SET) {
            if((ulCmd & LCD_FONT_TYPE) && !(ulCmd & LCD_2_LINE)) {
                tDisplayMem.ucFontType = FONT_5X11;                      // 1 line                                                                
            }
            else {                
                tDisplayMem.ucFontType = FONT_5X8;                       // 2 line                                                               
            }

            if(ulCmd & LCD_2_LINE) {
                tDisplayMem.ucLines = 2;
            }
            else {
                tDisplayMem.ucLines = 1;
            }
            tDisplayMem.bmode = ((ulCmd & 0x10) != 0);                   // set 4 or 8 bit mode
            tDisplayMem.ucDDRAMLineLength = LCD_Info.ucDDRAMLineLength[tDisplayMem.ucLines-1];
            tDisplayMem.uiDDRamLength = LCD_Info.uiDDRamLength[tDisplayMem.ucLines-1];
        }
        else if (ulCmd & LCD_CURSOR_OR_LCD_SHIFT) {                      // shift right
            if (!(ulCmd & LCD_DDRRAM_SHIFT_RIGHT)) {
                if (ulCmd & LCD_DDRRAM_DISPLAYSHIFT) {
                    if (++tDisplayMem.ucShiftPosition > (tDisplayMem.ucDDRAMLineLength-1)) { //shift whole display
                        tDisplayMem.ucShiftPosition = 0;
                    }
                }
                else{
                    if (tDisplayMem.ucLocX > 0)                          // shift cursor only
                        tDisplayMem.ucLocX--;
                    else {
                        tDisplayMem.ucLocX = (tDisplayMem.ucDDRAMLineLength - 1);
                        if (tDisplayMem.ucLocY > 0) {
                            tDisplayMem.ucLocY--;
                        }
                        else {
                            tDisplayMem.ucLocY = tDisplayMem.ucLines - 1;
                        }
                    }
                }
            }
            else {                                                       //shift left
                if (ulCmd & LCD_DDRRAM_DISPLAYSHIFT) {
                    if (tDisplayMem.ucShiftPosition > 0) {               //shift whole display
                        tDisplayMem.ucShiftPosition--;
                    }
                    else {
                        tDisplayMem.ucShiftPosition = (tDisplayMem.ucDDRAMLineLength - 1);
                    }
                }
                else {                                                   //shift cursor only
                    if (++tDisplayMem.ucLocX > (tDisplayMem.ucDDRAMLineLength - 1)) {
                        tDisplayMem.ucLocX = 0;
                        if (++tDisplayMem.ucLocY >= tDisplayMem.ucLines) {
                            tDisplayMem.ucLocY = 0;
                        }
                    }
                }
            }
        }
        else if (ulCmd & LCD_ONOFF) {    
            unsigned char ucOldCursor = tDisplayMem.ucCursorOn;
            unsigned char ucOldBlink = tDisplayMem.ucCursorBlinkOn;
            unsigned char ucOldOn = tDisplayMem.ucLCDon;

            tDisplayMem.ucLCDon = ((ulCmd & LCD_ON) == LCD_ON);          // turn lcd on/off
            if (ucOldOn != tDisplayMem.ucLCDon) {
                fnInvalidateLCD();                                       // invalidate complete display to cause redraw
            }
            
            tDisplayMem.ucCursorBlinkOn = ((ulCmd & LCD_CURSOR_BLINK_ON) == LCD_CURSOR_BLINK_ON);
            tDisplayMem.ucCursorOn = ((ulCmd & LCD_CURSOR_ON) == LCD_CURSOR_ON);

            if (ucOldCursor != tDisplayMem.ucCursorOn) {
                fnInvalidateLCD();
            }

            if (!tDisplayMem.ucCursorBlinkOn) {
                ucBlinkCursor = 0;
            }
        }
        else if (ulCmd & LCD_ENTRY_MODE) {
            tDisplayMem.ucLCDshiftEnable = ((ulCmd & LCD_DISPLAY_SHIFT) == LCD_DISPLAY_SHIFT); // set shift mode
            tDisplayMem.ucCursorInc__Dec = ((ulCmd & LCD_CURSOR_INCREMENT) == LCD_CURSOR_INCREMENT);
        }
        else if(ulCmd & LCD_RETURN_HOME) {
            tDisplayMem.ucLocX = 0;
            tDisplayMem.ucLocY = 0;
            tDisplayMem.ucCursorInc__Dec = 1;
            tDisplayMem.ucShiftPosition = 0;
        }
        else if(ulCmd == LCD_CLEAR) {                                    // clear display and return to home position
            memset((void*)&tDisplayMem.ddrRam[0], ' ', tDisplayMem.uiDDRamLength) ;
            tDisplayMem.ucLocX = 0;
            tDisplayMem.ucLocY = 0;
            tDisplayMem.ucCursorInc__Dec = 1;            
            tDisplayMem.ucShiftPosition = 0x00;
            tDisplayMem.ucRAMselect = DDRAM;
            fnInvalidateLCD();
        }
    }
    else {                                                               //read/write command
        if (tDisplayMem.ucRAMselect == CGRAM) {
            unsigned int *pCharLine = (UINT*)&font_tbl[tDisplayMem.ucCGRAMaddr/8].font_y0;
            pCharLine += (tDisplayMem.ucCGRAMaddr % 8);

            *pCharLine = (unsigned char)(ulCmd & 0x1f);
            pCharLine += (8 * 8);                                        // the pattern repeats 0..7 and 8..15 so set also the upper pattern
            *pCharLine = (unsigned char)(ulCmd & 0x1f);
                
            if (tDisplayMem.ucCGRAMaddr++ > 64) {
                tDisplayMem.ucCGRAMaddr = 0;
            }
        }        
        else {
            ulCmd &= 0xFF;           
                                                                         // save character
            tDisplayMem.ddrRam[LCD_Info.uiDDRAMStartLine[tDisplayMem.ucLocY]+tDisplayMem.ucLocX] = (unsigned char)(ulCmd);

            if (tDisplayMem.ucLCDshiftEnable) {
                if (!tDisplayMem.ucCursorInc__Dec) {
                    if (tDisplayMem.ucShiftPosition > 0) {
                        tDisplayMem.ucShiftPosition--;
                    }
                    else {
                        tDisplayMem.ucShiftPosition = (tDisplayMem.ucDDRAMLineLength-1);
                    }
                }
                else {
                    if (++tDisplayMem.ucShiftPosition > (tDisplayMem.ucDDRAMLineLength-1)) {
                        tDisplayMem.ucShiftPosition = 0;
                    }
                }
            }
        }

        if (tDisplayMem.ucCursorInc__Dec) {                              //new cursor position
            if (++tDisplayMem.ucLocX > (tDisplayMem.ucDDRAMLineLength-1)) {
                tDisplayMem.ucLocX = 0;
                if (++tDisplayMem.ucLocY >= tDisplayMem.ucLines) {
                    tDisplayMem.ucLocY = 0;
                }
            }
        }
        else {                                                           //shift cursor only
            if (tDisplayMem.ucLocX > 0) {
                tDisplayMem.ucLocX--;
            }
            else {
                switch (LCD_Info.uiType) {
                    case 0:
                        tDisplayMem.ucLocX = (tDisplayMem.ucDDRAMLineLength-1);
                        break;

                    case 1:                                              // Eg.. Ampire AC164A
                        tDisplayMem.ucLocX = 7;
                        break;

                    default:
                        break;
                }
                if (tDisplayMem.ucLocY > 0) {
                    tDisplayMem.ucLocY--;
                }
                else {
                    tDisplayMem.ucLocY = 0;                              // tDisplayMem.ucLines - 1;
                }
            }
        }
    }
    return 0;
}

#if defined SUPPORT_GLCD || defined SUPPORT_TFT || defined GLCD_COLOR
    static unsigned short usAddressPointer = 0;
    static unsigned short usTextHomeAddress = 0;
    static unsigned short usGraphicHomeAddress = 0;
    static unsigned char  ucTextColumns = 0;
    static unsigned char  ucGraphicColumns = 0;
    static unsigned char  ucExternalDisplayRAM[8*1024];
    static unsigned char  ucGraphicsMode = 0;

    #define XOR_MODE  0x01
#endif

static unsigned int    uiBlockCursor[8] = {0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f};
static unsigned int    uiLargeCursor[8] = {0x1f, 0x1f, 0x1f, 0x00, 0x00, 0x00, 0x00, 0x00};
static unsigned int    uiLowCursor[8]   = {0x00, 0x00, 0x1f, 0x00, 0x00, 0x00, 0x00, 0x00};

static void DisplayCharNew(HDC hdc, RECT rect, POINT point, UINT *pBits, UINT nPixels, UINT nFactor, int iShort)
{
    DWORD dwTextColor = LCD_PIXEL_COLOUR, dwCurColor = LCD_ON_COLOUR, dwThisCol;
    if (nBackLightOn == 0) {
        dwCurColor = LCD_OFF_COLOUR;
    }

    UINT z1 =0, z2 = 0;
                                                                         // draw a character
    for (UINT k = 0; k < 8; k++) {                                       // lines
        for (UINT shift = 0x10; shift>0; shift>>=1) {                    // columns
            dwThisCol = ((*pBits & shift) ? dwTextColor :  dwCurColor);
            for (z1 = 0; z1 < nFactor; z1++) {
                for (z2 = 0; z2 < nPixels; z2++) {
                    SetPixelV(hdc, point.x+z2, point.y+z1, dwThisCol);
                }
            }
            point.x += nPixels;;                                         // new pixel for the column
        }
        *pBits++;                                                        // font_y1 to font_y7 (font_y0 already treated)
        point.y += nFactor;                                              // new pixel for the line
        point.x = rect.left;                                             // next pixel for the column 
    }
}


// Redraw the LCD
//
static void DrawLcdLine(HWND hwnd)
{
#if defined SUPPORT_GLCD || defined SUPPORT_OLED || defined SUPPORT_TFT || defined GLCD_COLOR
    #if defined GLCD_COLOR || defined TFT_GLCD_MODE || defined NOKIA_GLCD_MODE || defined CGLCD_GLCD_MODE || defined KITRONIX_GLCD_MODE || defined TFT2N0369_GLCD_MODE // {7}{8}
        #if !defined SUPPORT_TFT && !defined TFT_GLCD_MODE               // {7}
    unsigned long ulPixel;
        #endif
    DWORD dwPixelColor = 0;
    #else
    unsigned char k;
    unsigned char ucBit;
    unsigned char ucColBit = 0x01;
    #endif
#else
    unsigned int save;
#endif
    int i,j;

    HDC hdc = GetDC(hwnd);

    UINT nFactor = 4;                                                    // 5 = width of fonts
    UINT nPixels = 3;    

    RECT rect;
    POINT point;

    UINT *pBits = NULL; 
#if defined SUPPORT_GLCD || defined SUPPORT_OLED || defined SUPPORT_OLED || defined SUPPORT_TFT || defined GLCD_COLOR
	unsigned long ulGraphicAdd = 0;
#else
    UCHAR pCurrent;
    int iHandleCursor;
#endif

    nFactor = 4;
    nPixels = 3;
    
    if (NULL == hwnd) {
        return;
    }

    EnterCriticalSection(&cs);                                           // protect from task switching
    
#if (defined SUPPORT_GLCD || defined SUPPORT_TFT || defined TFT_GLCD_MODE || defined GLCD_COLOR || defined KITRONIX_GLCD_MODE || defined TFT2N0369_GLCD_MODE) && !defined OLED_GLCD_MODE || defined CGLCD_GLCD_MODE // {6}{7}
    #if defined GLCD_COLOR || defined TFT_GLCD_MODE || defined NOKIA_GLCD_MODE || defined CGLCD_GLCD_MODE || defined KITRONIX_GLCD_MODE || defined TFT2N0369_GLCD_MODE // {7}{8}
    if (pPixels != 0) {                                                  // {19}
        LCD_draw_bmp((rectLcd.left + 7), (rectLcd.top + 6),  GLCD_X, GLCD_Y); // redraw complete bitmap
        LeaveCriticalSection(&cs);
    
        ReleaseDC(hwnd, hdc);
        return;
    }

	rect = rectLines[0];
    point.y = rect.top;

    for (i = 0; i < GLCD_Y; i++) {                                       // for each line
		#if defined BIG_PIXEL
		point.x = (rect.left + 2*NON_DISPLAYED_X);
		#else
		point.x = (rect.left + NON_DISPLAYED_X);
		#endif
        for (j = 0; j < GLCD_X; j++) {                                   // for each pixel in line
        #if defined SUPPORT_TFT || defined TFT_GLCD_MODE                 // {7}
            dwPixelColor = ulGraphicPixels[i][j];
        #else
            ulPixel = ulGraphicPixels[i][j];
            #if defined NOKIA_GLCD_MODE                                  // {8}
            dwPixelColor = (((ulPixel & 0xf00) << 12) | ((ulPixel & 0x0f0) << 8) | ((ulPixel & 0x00f) << 4));
            #else
            if (ucPixelFormat == PIXEL_16_BIT) {
                dwPixelColor = ((unsigned char)(ulPixel << 3) | ((ulPixel << 5) & 0x0000fc00) | ((ulPixel << 8) & 0x00f80000));
                #if defined _HX8347 || defined KITRONIX_GLCD_MODE || defined TFT2N0369_GLCD_MODE || defined MB785_GLCD_MODE // {15}
                dwPixelColor = ((dwPixelColor << 16) | (dwPixelColor & 0x0000ff00) | (dwPixelColor >> 16));
                #endif
            }
            #endif
        #endif
        #if defined BIG_PIXEL
            SetPixelV(hdc, point.x, point.y, dwPixelColor);
            SetPixelV(hdc, point.x + 1, point.y, dwPixelColor);
            SetPixelV(hdc, point.x , (point.y + 1), dwPixelColor);
            SetPixelV(hdc, (point.x + 1), (point.y + 1), dwPixelColor);
            point.x += 2;
        #else
            SetPixelV(hdc, point.x, point.y, dwPixelColor);
            point.x++;
        #endif
        }
        #if defined BIG_PIXEL
		point.y += 2;
        #else
		point.y++;
        #endif
    }
    #elif defined _GLCD_SAMSUNG || defined ST7565S_GLCD_MODE             // {18}
    rect = rectLines[0];
        #if defined BIG_PIXEL
    point.x = (rect.left + 2*NON_DISPLAYED_X);
        #else
    point.x = (rect.left + NON_DISPLAYED_X);
        #endif
    point.y = rect.top;

    for (i = 0; i < (GLCD_Y/8); i++) {                                   // for each page
        for (j = NON_DISPLAYED_X; j < (LCD_PIXEL_X_REAL); j++) {         // for each visible column
            int iChip = 0;
            if (j >= (LCD_PIXEL_X_REAL/SAMSUNG_CHIPS)) {
                iChip = 1;
            }
            if (iGraphicFlags[iChip] & DISPLAY_IS_ON) {
                ucBit = 0x01;
            }
            else {
                ucBit = 0;
            }
            for (k = 0; k < 8; k++) {
                if (ucGraphicPixels[i][j] & ucBit) {
        #if defined BIG_PIXEL
                    SetPixelV(hdc, point.x + (j - NON_DISPLAYED_X)*2, (point.y + k*2) + i*8*2, LCD_PIXEL_COLOUR);
                    SetPixelV(hdc, point.x + 1 + (j - NON_DISPLAYED_X)*2, (point.y + k*2) + i*8*2, LCD_PIXEL_COLOUR);
                    SetPixelV(hdc, point.x + (j - NON_DISPLAYED_X)*2, (point.y + 1 + k*2) + i*8*2, LCD_PIXEL_COLOUR);
                    SetPixelV(hdc, point.x + 1 + (j - NON_DISPLAYED_X)*2, (point.y + 1 + k*2) + i*8*2, LCD_PIXEL_COLOUR);
        #else
                    SetPixelV(hdc, point.x + (j - NON_DISPLAYED_X), (point.y + k) + i*8, LCD_PIXEL_COLOUR);
        #endif
                }
                else {
        #if defined BIG_PIXEL
                    SetPixelV(hdc, point.x + (j - NON_DISPLAYED_X)*2, (point.y + k*2) + i*8*2, LCD_ON_COLOUR);
                    SetPixelV(hdc, point.x + 1 + (j - NON_DISPLAYED_X)*2, (point.y + k*2) + i*8*2, LCD_ON_COLOUR);
                    SetPixelV(hdc, point.x + (j - NON_DISPLAYED_X)*2, (point.y + 1 + k*2) + i*8*2, LCD_ON_COLOUR);
                    SetPixelV(hdc, point.x + 1 + (j - NON_DISPLAYED_X)*2, (point.y + 1 + k*2) + i*8*2, LCD_ON_COLOUR);
        #else
                    SetPixelV(hdc, point.x + (j - NON_DISPLAYED_X), (point.y + k) + i*8, LCD_ON_COLOUR);
        #endif
                }
                ucBit <<= 1;
            }
        }
        ucColBit <<= 1;
    }
    #else
	rect = rectLines[0];
    point.y = rect.top;
    for (i = 0; i < GLCD_Y; i++) {                                       // for each line
		#if defined BIG_PIXEL
		point.x = (rect.left + 2*NON_DISPLAYED_X);
		#else
		point.x = (rect.left + NON_DISPLAYED_X);
		#endif
        for (j = 0; j < (GLCD_X/8); j++) {                               // for each byte in line
            ucBit = 0x80;
            for (k = 0; k < 8; k++) {				                     // for each pixel in the byte
                if (ucExternalDisplayRAM[ulGraphicAdd] & ucBit) {        // if on
        #if defined BIG_PIXEL
                    SetPixelV(hdc, point.x, point.y, LCD_PIXEL_COLOUR);
                    SetPixelV(hdc, point.x + 1, point.y, LCD_PIXEL_COLOUR);
                    SetPixelV(hdc, point.x , (point.y + 1), LCD_PIXEL_COLOUR);
                    SetPixelV(hdc, (point.x + 1), (point.y + 1), LCD_PIXEL_COLOUR);
        #else
                    SetPixelV(hdc, point.x, point.y, LCD_PIXEL_COLOUR);
        #endif
                }
                else {
        #if defined BIG_PIXEL
                    SetPixelV(hdc, point.x, point.y, LCD_ON_COLOUR);
                    SetPixelV(hdc, point.x + 1, point.y, LCD_ON_COLOUR);
                    SetPixelV(hdc, point.x , (point.y + 1), LCD_ON_COLOUR);
                    SetPixelV(hdc, (point.x + 1), (point.y + 1), LCD_ON_COLOUR);
        #else
                    SetPixelV(hdc, point.x, point.y, LCD_ON_COLOUR);
        #endif
                }
                ucBit >>= 1;
        #if defined BIG_PIXEL
				point.x += 2;
        #else
				point.x++;
        #endif
            }
			ulGraphicAdd++;
        }
        #if defined BIG_PIXEL
		point.y += 2;
        #else
		point.y++;
        #endif
    }
    #endif
#elif defined SUPPORT_OLED || defined OLED_GLCD_MODE                     // {6}
	rect = rectLines[0];
    point.y = rect.top;

    for (i = 0; i < GLCD_Y; i++) {                                       // {11} for each line
		#if defined BIG_PIXEL
		point.x = (rect.left + 2*NON_DISPLAYED_X);
		#else
		point.x = (rect.left + NON_DISPLAYED_X);
		#endif
        for (j = 0; j < (GLCD_X/2); j++) {                               // {11} for each byte in line
            ucBit = 0xf0;
            for (k = 0; k < 2; k++) {				                     // for each pixel in the byte
                if (OLED_graphical_memory[ulGraphicAdd] & ucBit) {       // if on
                    DWORD dwTextColor = LCD_PIXEL_COLOUR;
                    dwTextColor /= 15;
                    if (k == 0) {
                        dwTextColor *= (OLED_graphical_memory[ulGraphicAdd] >> 4);
                    }
                    else {
                        dwTextColor *= (OLED_graphical_memory[ulGraphicAdd] & ucBit);
                    }
    #if defined BIG_PIXEL
                    SetPixelV(hdc, point.x, point.y, dwTextColor);
                    SetPixelV(hdc, point.x + 1, point.y, dwTextColor);
                    SetPixelV(hdc, point.x , (point.y + 1), dwTextColor);
                    SetPixelV(hdc, (point.x + 1), (point.y + 1), dwTextColor);
    #else
                    SetPixelV(hdc, point.x, point.y, dwTextColor);
    #endif
                }
                else {
    #if defined BIG_PIXEL
                    SetPixelV(hdc, point.x, point.y, LCD_ON_COLOUR);
                    SetPixelV(hdc, point.x + 1, point.y, LCD_ON_COLOUR);
                    SetPixelV(hdc, point.x , (point.y + 1), LCD_ON_COLOUR);
                    SetPixelV(hdc, (point.x + 1), (point.y + 1), LCD_ON_COLOUR);
    #else
                    SetPixelV(hdc, point.x, point.y, LCD_ON_COLOUR);
    #endif
                }
                ucBit >>= 4;
    #if defined BIG_PIXEL
				point.x += 2;
    #else
				point.x++;
    #endif
            }
			ulGraphicAdd++;
        }
    #if defined BIG_PIXEL
		point.y += 2;
    #else
		point.y++;
    #endif
    }
#else    
    if(tDisplayMem.ucLCDon) {
        SetBkMode(hdc, OPAQUE);                                          // display values
    }
    else {
        SetBkMode(hdc, TRANSPARENT);                                     // hide values
    }
    
    for (i = 0; i < LCD_Info.ucNrOfLCDLines; i++) {
        if ((tDisplayMem.ucLines == 1) && (i & 0x01)) {
            continue;
        }

        pCurrent = tDisplayMem.ucShiftPosition+LCD_Info.uiLCDStartLine[i];

        if (pCurrent >= (LCD_Info.uiDDRAMStartLine[i%2]+tDisplayMem.ucDDRAMLineLength)) {
                pCurrent = pCurrent-tDisplayMem.ucDDRAMLineLength;
        }

        rect = rectLines[i];
        point.x = rect.left;
        point.y = rect.top;

        for (j = 0; j < LCD_Info.ucNrOfVisibleCharacters; j++) {
            if ((pCurrent == (LCD_Info.uiDDRAMStartLine[tDisplayMem.ucLocY]+tDisplayMem.ucLocX)) && (tDisplayMem.ucCursorOn || tDisplayMem.ucCursorBlinkOn)) {
                iHandleCursor = 1;
            }
            else {
                iHandleCursor = 0;
            }

            // If 5x11 font is used, even RAM locations are used for upper half of the character
            // and odd RAM locations are used for lower half
            //
            if ((tDisplayMem.ucFontType == 2) && (tDisplayMem.ddrRam[pCurrent] < 0x10)) {
                pBits = (UINT*)&font_tbl[tDisplayMem.ddrRam[pCurrent]  & 0xFFFE];
            }
            else {
                if ((usOldValue[i][j] == (unsigned short)((unsigned char)(tDisplayMem.ddrRam[pCurrent]))) &&
                    !iHandleCursor) goto dont_display;                   // don't bother drawing this one
                usOldValue[i][j] = (unsigned short)((unsigned char)(tDisplayMem.ddrRam[pCurrent])); // validate
                pBits = (UINT*)&font_tbl[tDisplayMem.ddrRam[pCurrent]];
            }

            rect.left = uiCharStart[j];                                  // point.x = rect.left; 
            point.x = uiCharStart[j];
            point.y = rect.top;                        
            
            if (iHandleCursor) {
                usOldValue[i][j] |= 0x8000;                              // ensure updated next time around to avoid cursor left on
                if (ucBlinkCursor) {
                    pBits = uiBlockCursor;
                    DisplayCharNew(hdc, rect, point, pBits, nPixels, nFactor, 0);

                        if ((tDisplayMem.ucLines == 1) && (tDisplayMem.ucFontType == FONT_5X11)) {
                            point.y = rectLines[i+1].top;
                            pBits = uiLargeCursor;
                            DisplayCharNew(hdc, rect, point, pBits, nPixels, nFactor, 0);
                        }
                }
                else {
                    if (tDisplayMem.ucFontType == FONT_5X11) {
                        DisplayCharNew(hdc, rect, point, pBits, nPixels, nFactor, 0);
                        point.y = rectLines[i+1].top;

                        if (tDisplayMem.ddrRam[pCurrent & 0xFFFE] < 0x10) {
                            pBits = (UINT*)&font_tbl[tDisplayMem.ddrRam[pCurrent | 0x01]];
                        }
                        else {                        
                            pBits = (UINT*)&font5x11[tDisplayMem.ddrRam[pCurrent]];
                        }
                        if (tDisplayMem.ucCursorOn) {
                            save = *(pBits+2);
                            *(pBits+2) = 0xFF;
                            DisplayCharNew(hdc, rect, point, pBits, nPixels, nFactor, 1);
                            *(pBits+2) = save;
                        }
                        else {
                            DisplayCharNew(hdc, rect, point, pBits, nPixels, nFactor, 1);
                        }
                    }
                    else {
                       if (tDisplayMem.ucCursorOn) {
                           save = *(pBits+7);
                           *(pBits+7) = 0xFF;
                           DisplayCharNew(hdc, rect, point, pBits, nPixels, nFactor, 0);
                           *(pBits+7) = save;
                       }
                       else {
                           DisplayCharNew(hdc, rect, point, pBits, nPixels, nFactor, 0);
                       }
                    }
                }
            }
            else {
                DisplayCharNew(hdc, rect, point, pBits, nPixels, nFactor, 0);

                if (tDisplayMem.ucFontType == 2) {                       // show also lower part of the character
                    point.y = rectLines[i+1].top;
                    
                    if (tDisplayMem.ddrRam[pCurrent & 0xFFFE] < 0x10) {  // special handling for user defined characters
                        pBits = (UINT*)&font_tbl[tDisplayMem.ddrRam[pCurrent] | 0x01];
                    }
                    else {                    
                        pBits = (UINT*)&font5x11[tDisplayMem.ddrRam[pCurrent]];
                    }
                    
                    DisplayCharNew(hdc, rect, point, pBits, nPixels, nFactor, 1);
                }
            }
        
dont_display:
            if (++pCurrent == (LCD_Info.uiDDRAMStartLine[i%2]+tDisplayMem.ucDDRAMLineLength)) {
                pCurrent = LCD_Info.uiDDRAMStartLine[i%2];
            }
        }
    }
#endif
    
    LeaveCriticalSection(&cs);
    
    ReleaseDC(hwnd, hdc);
}

#if defined SUPPORT_OLED || defined OLED_GLCD_MODE                       // {6}
static void GraphicOLEDCommand(bool bRS, unsigned char ucByte)
{
    static int iCmdCnt = 0;
    static unsigned char ucCmd;
    if (bRS != 0) {                                                      // data
		unsigned char ucChangedBits = (OLED_graphical_memory[ulGraphicAdd] ^ (unsigned char)ucByte);
		unsigned char ucNewByte = (unsigned char)ucByte;
		OLED_graphical_memory[ulGraphicAdd] = ucNewByte;
        if (ucChangedBits != 0) {
			RECT rect = rectLines[0];
            POINT point;
            HDC hdc = GetDC(ghWnd);
            int k;
            unsigned char ucBit = 0xf0;
			unsigned short usXoffset = (unsigned short)(ulGraphicAdd % (GLCD_X/2));// {11} 
			unsigned short usYoffset = (unsigned short)(ulGraphicAdd / (GLCD_X/2));// {11} 
			#if defined BIG_PIXEL
			usXoffset *= 4;
			usYoffset *= 2;
            #else
            usXoffset *= 2;
            #endif
            point.x = rect.left + usXoffset;                 // initial pixel location
            point.y = rect.top + usYoffset;
            for (k = 0; k < 2; k++) {                        // for each pixel in byte
				if (ucChangedBits & ucBit) {
					if (ucNewByte & ucBit) {                 // display on
                        DWORD dwTextColor = LCD_PIXEL_COLOUR;
                        dwTextColor /= 15;
                        if (k == 0) {
                            dwTextColor *= (ucNewByte >> 4);
                        }
                        else {
                            dwTextColor *= (ucNewByte & ucBit);
                        }
						#if defined BIG_PIXEL
						SetPixelV(hdc, point.x, point.y, dwTextColor);
						SetPixelV(hdc, point.x + 1, point.y, dwTextColor);
						SetPixelV(hdc, point.x, (point.y + 1), dwTextColor);
						SetPixelV(hdc, point.x + 1, (point.y + 1), dwTextColor);
						#else
						SetPixelV(hdc, point.x, point.y, dwTextColor);
						#endif
					}
					else {                                        // display off
						#if defined BIG_PIXEL
						SetPixelV(hdc, point.x, point.y, LCD_ON_COLOUR);
						SetPixelV(hdc, point.x + 1, point.y, LCD_ON_COLOUR);
						SetPixelV(hdc, point.x, (point.y + 1), LCD_ON_COLOUR);
						SetPixelV(hdc, point.x + 1, (point.y + 1), LCD_ON_COLOUR);
						#else
						SetPixelV(hdc, point.x, point.y, LCD_ON_COLOUR);
						#endif
					}
				}
				#if defined BIG_PIXEL
				point.x += 2;
                #else
				point.x++;
                #endif
				ucBit >>= 4;
			}
			ReleaseDC(ghWnd, hdc);  
		} 

        if (!(ucRemap & 0x04)) {                                         // horizontal address increment enabled
            ulGraphicAdd++;
            if ((ulGraphicAdd % (GLCD_X/2)) > ucColumnEnd) {             // {11}
                ulGraphicAdd += ((ulGraphicAdd % (GLCD_X/2) - ucColumnEnd) * 2);// {11}

            }
            if ((ulGraphicAdd % (GLCD_X/2)) < ucColumnStart) {           // {11}
                ulGraphicAdd += ucColumnStart;
            }
            if (ulGraphicAdd >= sizeof(OLED_graphical_memory)) {
                ulGraphicAdd = 0;
            }
        }
    }
    else {                                                               // command
        if (iCmdCnt != 0) {
            iCmdCnt--;
            switch (ucCmd) {
            case 0xb8:                                                   // gray scale pulse width lookup table
                ucGrayScalePulseWidth[14 - iCmdCnt] = ucByte;
                break;
            case 0x15:                                                   // set up column start and end address
                if (iCmdCnt == 1) {
                    ucColumnStart = ucByte;
                }
                else {
                    ucColumnEnd = ucByte;
                }
                break;
            case 0x75:                                                   // set up row start and end address
                if (iCmdCnt == 1) {
                    ucRowStart = ucByte;
                }
                else {
                    ucRowEnd = ucByte;
                }
                ulGraphicAdd = ((ucRowStart * (GLCD_X/2)) + ucColumnStart); // {11} when is this really set and reset?
                if (ulGraphicAdd >= sizeof(OLED_graphical_memory)) {
                    ulGraphicAdd -= sizeof(OLED_graphical_memory);
                }
                break;
            case 0xfd:                                                   // set command lock
                ucMCU_protection_status = ucByte;
                break;
            case 0xbc:                                                   // first pre-charge voltage
                PrechargeVoltage = ucByte;
                break;
            case 0xbb:                                                   // second pre-charge period
                ucSecondPrechargePeriod = ucByte;
                break;
            case 0xb3:                                                   // set oscillator frequency
                ucOscillatorFreq = ucByte;
                break;
            case 0xb2:                                                   // set frame frequency
                ucFrameFrequency = ucByte;
                break;
            case 0xb1:                                                   // set phase length
                ucPhaseLength = ucByte;
                break;
            case 0xa8:                                                   // set MUX ratio
                ucMuxRatio = ucByte;
                break;
            case 0xa2:                                                   // set display offset
                ucDisplayOffset = ucByte;
                break;
            case 0xa1:                                                   // set display start line
                ucDisplayRamStart = ucByte;
                break;
            case 0xa0:                                                   // remap setting in graphic display data ram
                ucRemap = ucByte;
                break;
            case 0x94:                                                   // icon register
                switch (ucByte & 0xc0) {
                case 0x00:
                    iIcon = ICON_ALL_OFF;
                    break;
                case 0x40:
                    iIcon = ICON_ALL_ON;
                    break;
                case 0xc0:
                    iIcon = ICON_ALL_BLINK;
                    break;
                }
                break;
            case 0x82:                                                   // second pre-charge speed
                ucSecondPrechargeSpeed = ucByte;
                break;
            case 0x81:                                                   // set contrast
                ucContrast = ucByte;
                break;
            }
        }
        else {
            ucCmd = ucByte;
            switch (ucByte) {
            case 0xb8:                                                   // gray scale pulse width lookup table
                iCmdCnt = 15;                                            // command accepted, now collect fifteen command data bytes
                break;
            case 0x75:                                                   // set up row start and end address
            case 0x15:                                                   // set up column start and end address
                iCmdCnt = 2;                                             // command accepted, now collect two command data bytes
                break;
            case 0xfd:                                                   // set command lock
            case 0xbc:                                                   // first pre-charge voltage
            case 0xbb:                                                   // second pre-charge period
            case 0xb3:                                                   // set oscillator frequency
            case 0xb2:                                                   // set frame frequency
            case 0xb1:                                                   // set phase length
            case 0xa8:                                                   // set MUX ratio
            case 0xa2:                                                   // set display offset
            case 0xa1:                                                   // set display start line
            case 0xa0:                                                   // remap setting in graphic display data ram
            case 0x94:                                                   // icon register
            case 0x82:                                                   // second pre-charge speed
            case 0x81:                                                   // set contrast
                iCmdCnt = 1;                                             // command accepted, now collect one command data byte
                break;
            case 0xb7:                                                   // set default gray scale pulse width values
                ucGrayScalePulseWidth[0]  = 2;
                ucGrayScalePulseWidth[1]  = 4;
                ucGrayScalePulseWidth[2]  = 6;
                ucGrayScalePulseWidth[3]  = 8;
                ucGrayScalePulseWidth[4]  = 10;
                ucGrayScalePulseWidth[5]  = 12;
                ucGrayScalePulseWidth[6]  = 14;
                ucGrayScalePulseWidth[7]  = 16;
                ucGrayScalePulseWidth[8]  = 18;
                ucGrayScalePulseWidth[9]  = 20;
                ucGrayScalePulseWidth[10] = 22;
                ucGrayScalePulseWidth[11] = 24;
                ucGrayScalePulseWidth[12] = 26;
                ucGrayScalePulseWidth[13] = 28;
                ucGrayScalePulseWidth[14] = 30;
                break;
            case 0xae:                                                   // set sleep mode on (display OFF)
                iDisplayOn = 0;
                break;
            case 0xaf:                                                   // set sleep mode off (display ON)
                iDisplayOn = 1;
                break;
            case 0xa7:                                                   // set display mode - normal
                iDisplayMode = MODE_INVERSE;
                break;
            case 0xa6:                                                   // set display mode - normal
                iDisplayMode = MODE_ALL_OFF;
                break;
            case 0xa5:                                                   // set display mode - normal
                iDisplayMode = MODE_ALL_ON;
                break;
            case 0xa4:                                                   // set display mode - normal
                iDisplayMode = MODE_NORMAL;
                break;
            default:
                *(unsigned char *)0 = 0;                                 // non-implemented command
                return;
            }
        }
    }
}
#endif


#if (defined SUPPORT_GLCD || defined GLCD_COLOR) && !defined SUPPORT_TFT && !defined OLED_GLCD_MODE && !defined TFT_GLCD_MODE // {6}{7}
static void fnWriteDisplay(unsigned long ulByte)
{
    if ((usPresentColumn < GLCD_Y) && (usPresetRow < GLCD_X)) {
    #if defined MB785_GLCD_MODE
        if (1) 
    #else
        if ((ucPixelFormat == PIXEL_16_BIT) && (iDataCnt == 1)) 
    #endif
        {
			unsigned long ulChangedBits;
    #if (defined _HX8347 || defined KITRONIX_GLCD_MODE || defined TFT2N0369_GLCD_MODE || defined MB785_GLCD_MODE) && !defined ST7789S_GLCD_MODE
            ulNewPixel = ulByte;
    #else
            ulNewPixel <<= 8;
            ulNewPixel |= (unsigned char)(ulByte);
    #endif
    #if defined GLCD_COLOR || defined TFT_GLCD_MODE || defined NOKIA_GLCD_MODE || defined CGLCD_GLCD_MODE || defined KITRONIX_GLCD_MODE || defined TFT2N0369_GLCD_MODE || defined MB785_GLCD_MODE // {7}{8}{13}
            ulChangedBits = (ulGraphicPixels[usPresentColumn][usPresetRow] ^ ulNewPixel);
            ulGraphicPixels[usPresentColumn][usPresetRow] = ulNewPixel;
    #else
            ulChangedBits = (ucGraphicPixels[usPresentColumn][usPresetRow] ^ ulNewPixel);
            ucGraphicPixels[usPresentColumn][usPresetRow] = (unsigned char)ulNewPixel;
    #endif
            iDataCnt = 0;
            if (ulChangedBits != 0) {                                    // pixel has changed colour
				RECT rect = rectLines[0];
                POINT point;
                HDC hdc = GetDC(ghWnd);
                DWORD dwPixelColor = ((unsigned char)(ulNewPixel << 3) | ((ulNewPixel << 5) & 0x0000fc00) | ((ulNewPixel << 8) & 0x00f80000));
    #if defined _HX8347 || defined KITRONIX_GLCD_MODE || defined TFT2N0369_GLCD_MODE || defined MB785_GLCD_MODE
                dwPixelColor = ((dwPixelColor << 16) | (dwPixelColor & 0x0000ff00) | (dwPixelColor >> 16));
    #endif
    #if defined BIG_PIXEL
                point.x = rect.left + (2 *usPresetRow);                  // pixel location
                point.y = rect.top + (2 * usPresentColumn);
    #else
                point.x = rect.left + usPresetRow;                       // pixel location
                point.y = rect.top + usPresentColumn;
    #endif
    #if defined BIG_PIXEL
				SetPixelV(hdc, point.x, point.y, dwPixelColor);
				SetPixelV(hdc, point.x + 1, point.y, dwPixelColor);
				SetPixelV(hdc, point.x, (point.y + 1), dwPixelColor);
				SetPixelV(hdc, point.x + 1, (point.y + 1), dwPixelColor);
    #else
				SetPixelV(hdc, point.x, point.y, dwPixelColor);
    #endif
				ReleaseDC(ghWnd, hdc);  
			}
        }
        else {
            iDataCnt++;
            ulNewPixel = ulByte;                                         // collect first byte
            return;
        }
        if (ucMemoryDataAccessControl == 0) {
            usPresetRow++;
            if (usPresetRow > usEndRow) {
                usPresetRow = usStartRow;
                usPresentColumn++;
                if (usPresentColumn > usEndColumn) {
                    usPresentColumn = usStartColumn;
                }
            }
        }
    }
    else {
     // *(unsigned char *)0 = 0;
    }
}

static void GraphicLCDCommand(bool bRS, unsigned long ulByte)
{
    unsigned char ucByte = (unsigned char)ulByte;
    #if defined GLCD_COLOR || defined NOKIA_GLCD_MODE || defined CGLCD_GLCD_MODE || defined KITRONIX_GLCD_MODE || defined TFT2N0369_GLCD_MODE || defined MB785_GLCD_MODE // {8}{13}
        #if defined MB785_GLCD_MODE
    static unsigned char ucState = 0xff;                                 // state controlled via SPI data value
    static unsigned char ucIndex = 0;
    if (bRS != 0) {                                                      // interpret as CS negated so leave write mode
        ucState = 0xff;
        return;
    }
    if ((ucState == 0xff) && ((ulByte & 0xfc) == 0x70)) {                // device ID matches
        ucState = ((unsigned char)ulByte & 0x03);                        // access type now known
        return;
    }
    else {
        if (ucState & 0x02) {                                            // RS bit
            bRS = 1;
        }
        else {
            bRS = 0;
        }
    }
        #endif
    if (bRS == 0) {                                                      // index register write
        #if defined NOKIA_GLCD_MODE
        ucCmd = ucByte;
        #endif
        #if defined MB785_GLCD_MODE                                      // {13}
        ucIndex = ucByte;                                                // save the index
        ucState = 0xff;                                                  // reset state
        return;
        #endif
		switch (ucCmd) {
        #if (defined KITRONIX_GLCD_MODE || defined TFT2N0369_GLCD_MODE) && !defined ST7789S_GLCD_MODE // {12}
        case 0x01:                                                       // driver output control
            break;
        case 0x02:                                                       // LCD driver AC control
            break;
        case 0x07:                                                       // display control
            break;
        case 0x0c:                                                       // power control 2
            break;
        case 0x0d:                                                       // power control 3
            break;
        case 0x0e:                                                       // power control 4
            break;
        case 0x10:                                                       // sleep mode
            break;
        case 0x11:                                                       // entry mode
            break;
        case 0x12:                                                       // sleep mode 2
            break;
        case 0x1e:                                                       // power control
            break;
        case 0x26:                                                       // analogue setting
            break;
        case 0x28:                                                       // VCOM OTP
            break;
        case 0x30:                                                       // gamma control 1
        case 0x31:                                                       // gamma control 2
        case 0x32:                                                       // gamma control 3
        case 0x33:                                                       // gamma control 4
        case 0x34:                                                       // gamma control 5
        case 0x35:                                                       // gamma control 6
        case 0x36:                                                       // gamma control 7
        case 0x37:                                                       // gamma control 8
        case 0x3a:                                                       // gamma control 9
        case 0x3b:                                                       // gamma control 10
            break;
        case 0x44:                                                       // vertical RAM address position
            usStartColumn = (unsigned short)(ulByte & 0xff);
            usEndColumn = (unsigned short)(ulByte >> 8);
            break;
        case 0x45:                                                       // horizontal RAM address start position
            usStartRow = (unsigned short)ulByte;
            break;
        case 0x46:                                                       // horizontal RAM address end position
            usEndRow = (unsigned short)ulByte;
            break;
            #if defined TFT2N0369_GLCD_MODE
        case 0x4f:                                                       // set GDDRAM X address counter
            usPresetRow = (unsigned short)ulByte;
            break;
        case 0x4e:                                                       // set GDDRAM Y address counter
            usPresentColumn = (unsigned short)ulByte;
            break;
            #else
        case 0x4e:                                                       // set GDDRAM X address counter
            usPresetRow = (unsigned short)ulByte;
            break;
        case 0x4f:                                                       // set GDDRAM Y address counter
            usPresentColumn = (unsigned short)ulByte;
            break;
            #endif
        #elif defined NOKIA_GLCD_MODE
            #if defined PHILIPS_PCF8833
        case 0x29:                                                       // display on
            iDisplayOn = 1;
            fnRedrawDisplay();                                           // refresh display
            break;
        case 0x01:                                                       // soft reset
        case 0xc6:                                                       // init sequence
        case 0x13:                                                       // normal mode
        case 0x11:                                                       // sleep out
        case 0xba:                                                       // voltage control
        case 0x03:                                                       // booste voltage on
        case 0x2c:                                                       // ram write
            break;
        case 0xbe:                                                       // power control
        case 0xbd:                                                       // common drive
        case 0xb9:                                                       // refresh set
        case 0xb5:                                                       // gamma
        case 0x36:                                                       // access control
        case 0x25:                                                       // contrast
            iCommand2 = 1;                                               // 1 parameter follows
            break;
        case 0x2a:                                                       // column address set
        case 0x2b:                                                       // page address set
            iCommand2 = 2;                                               // 2 parameters follows
            break;
        case 0xb6:                                                       // display set
            iCommand2 = 7;                                               // 7 parameters follows
            break;
        case 0xb3:                                                       // gray scale set
            iCommand2 = 15;                                              // 15 parameters follows
            break;
        case 0xb7:                                                       // temperatire gradient
            iCommand2 = 14;                                              // 14 parameters follows
            break;
            #else                                                        // Epson type
        case 0xca:                                                       // display control
        case 0xbc:                                                       // display control
            iCommand2 = 3;                                               // 3 parameters follow
            break;
        case 0x81:                                                       // volume control
        case 0x75:                                                       // page address set
        case 0x15:                                                       // column address set
            iCommand2 = 2;                                               // 2 parameters follow
            break;
        case 0xbb:                                                       // display control
        case 0x82:                                                       // temperature gradient
        case 0x20:                                                       // power control
            iCommand2 = 1;                                               // 1 parameter follows
            break;
        case 0xaf:                                                       // display on
            iDisplayOn = 1;
            fnRedrawDisplay();                                           // refresh display
            break;
        case 0xd1:                                                       // oscillator on
        case 0x94:                                                       // sleep out
        case 0xa6:                                                       // normal display
        case 0xa7:                                                       // inverse display
        case 0xa9:                                                       // partial area off
        case 0x5c:                                                       // ram write
            break;
            #endif
        default:
            *(unsigned char *)0 = 0;
            break;
        #elif defined _HX8347
        case 0x06:                                                       // column address start (high byte)
            usStartColumn = ucByte;
            usStartColumn <<= 8;
            break;
        case 0x07:                                                       // column address start (low byte)
            usStartColumn |= ucByte;
            break;
        case 0x08:                                                       // column address end (high byte)
            usEndColumn = ucByte;
            usEndColumn <<= 8;
            break;
        case 0x09:                                                       // column address end (low byte)
            usEndColumn |= ucByte;
            break;
        case 0x02:                                                       // row address start (high byte)
            usStartRow = ucByte;
            usStartRow <<= 8;
            break;
        case 0x03:                                                       // row address start (low byte)
            usStartRow |= ucByte;
            break;
        case 0x04:                                                       // row address end (high byte)
            usEndRow = ucByte;
            usEndRow <<= 8;
            break;
        case 0x05:                                                       // row address end (low byte)
            usEndRow |= ucByte;
            break;
        #else
        case 0x3c:                                                       // write to display memory
            fnWriteDisplay(ulByte);                                      // draw new display content
            break;
        case 0x2a:                                                       // column address start
            #if defined ST7789S_GLCD_MODE
            switch (iCommand2--) {
            case 4:
                usStartColumn = (ucByte << 8);
                break;
            case 3:
                usStartColumn |= (ucByte);
                break;
            case 2:
                usEndColumn = (ucByte << 8);
                break;
            case 1:
                usEndColumn |= (ucByte);
                break;
            }
            #else
            if (iCommand2-- != 0) {
                usStartRow = ucByte;
            }
            else {
                usEndRow = ucByte;
            }
            #endif
            break;
        case 0x2b:                                                       // row address start
            #if defined ST7789S_GLCD_MODE
            switch (iCommand2--) {
            case 4:
                usStartRow = (ucByte << 8);
                break;
            case 3:
                usStartRow |= (ucByte);
                break;
            case 2:
                usEndRow = (ucByte << 8);
                break;
            case 1:
                usEndRow |= (ucByte);
                break;
            }
            #else
            if (iCommand2-- != 0) {
                usStartColumn = ucByte;
                #if !defined _HX8347
                usStartColumn--;
                #endif
            }
            else {
                usEndColumn = ucByte;
                #if !defined _HX8347
                usEndColumn--;
                #endif
            }
            #endif
            break;
        #endif
        #if !defined NOKIA_GLCD_MODE
            #if defined _HX8347 || defined KITRONIX_GLCD_MODE || defined TFT2N0369_GLCD_MODE
        case 0x22:                                                       // write to display memory
            iDataCnt = 1;                                                // always 16 bit mode
            #else
        case 0x2c:                                                       // write to display memory
            #endif
            #if defined _HX8347 || defined KITRONIX_GLCD_MODE || defined TFT2N0369_GLCD_MODE
            fnWriteDisplay(ulByte);                                      // draw new display content
            #else
            fnWriteDisplay(ucByte);                                      // draw new display content
            #endif
            break;
            #if !defined KITRONIX_GLCD_MODE && !defined TFT2N0369_GLCD_MODE
        case 0x36:                                                       // memory data access control
            ucMemoryDataAccessControl = ucByte;
            break;
        case 0x3a:                                                       // interface pixel format
            ucPixelFormat = (ucByte & 0x07);
            break;
        case 0xb7:                                                       // COM/seg scan direction
            ucComSegScan = ucByte;
            break;
        case 0xc0:                                                       // Vop setting
            if (iCommand2-- != 0) {
                usVopSetting = (ucByte << 8);
            }
            else {
                usVopSetting |= ucByte;
            }
            break;
        case 0xc3:                                                       // bias setting
            ucBias = ucByte;
            break;
        case 0xc4:                                                       // booster setting
            ucBooster = ucByte;
            break;
        case 0xd7:                                                       // autoload set
            ucAutoloadSet = ucByte;
            break;
        case 0xe0:                                                       // control OTP read/write
            ucOTP_control = ucByte;
            break;
            #endif
        #endif
        }
    }
    else {                                                               // interpret command
        #if !defined NOKIA_GLCD_MODE
        ucCmd = ucByte;
        #endif
        #if defined MB785_GLCD_MODE                                      // {13}
		switch (ucIndex)
        #else
        iCommand2--;
		switch (ucCmd) 
        #endif
        {
        #if defined KITRONIX_GLCD_MODE || defined TFT2N0369_GLCD_MODE && !defined ST7789S_GLCD_MODE // {12}
        case 0x00:                                                       // oscillator start
        case 0x01:                                                       // driver output control
        case 0x02:                                                       // LCD driver AC control
        case 0x03:                                                       // power control 1
        case 0x05:                                                       // compare register 1
        case 0x06:                                                       // compare register 2
        case 0x07:                                                       // display control
        case 0x0b:                                                       // frame cycle control
        case 0x0c:                                                       // power control 2
        case 0x0d:                                                       // power control 3
        case 0x0e:                                                       // power control 4
        case 0x0f:                                                       // gate scan
        case 0x10:                                                       // sleep mode
        case 0x11:                                                       // entry mode
        case 0x12:                                                       // sleep mode 2
        case 0x16:                                                       // horizontal porch
        case 0x17:                                                       // vertical porch
        case 0x1e:                                                       // power control 5
        case 0x23:                                                       // ram write data mask 1
        case 0x24:                                                       // ram write data mask 2
        case 0x26:                                                       // analogue setting
        case 0x28:                                                       // VCOM OTP
        case 0x30:                                                       // gamma control 1
        case 0x31:                                                       // gamma control 2
        case 0x32:                                                       // gamma control 3
        case 0x33:                                                       // gamma control 4
        case 0x34:                                                       // gamma control 5
        case 0x35:                                                       // gamma control 6
        case 0x36:                                                       // gamma control 7
        case 0x37:                                                       // gamma control 8
        case 0x3a:                                                       // gamma control 9
        case 0x3b:                                                       // gamma control 10
        case 0x41:                                                       // vertical scroll 1
        case 0x42:                                                       // vertical scroll 2
        case 0x44:                                                       // vertical RAM address position
        case 0x45:                                                       // horizontal RAM address start position
        case 0x46:                                                       // horizontal RAM address end position
        case 0x48:                                                       // first screen x
        case 0x49:                                                       // first screen y
        case 0x4a:                                                       // second screen x
        case 0x4b:                                                       // second screen y
        case 0x4e:                                                       // set GDDRAM X address counter
        case 0x4f:                                                       // set GDDRAM Y address counter
        case 0x22:                                                       // graphic RAM data write
            break;
        #elif defined MB785_GLCD_MODE                                    // {13}
        case 0x00:                                                       // start oscillation
        case 0x01:                                                       // driver output control 1
        case 0x02:                                                       // LCD driving control
        case 0x03:                                                       // entry mode
        case 0x04:                                                       // resize control
        case 0x07:                                                       // display control 1
        case 0x08:                                                       // display control 2
        case 0x09:                                                       // display control 3
        case 0x0a:                                                       // display control 4
        case 0x0c:                                                       // RGB display interface control 1
        case 0x0d:                                                       // frame marker position
        case 0x0f:                                                       // RGB display interface control 2
        case 0x10:                                                       // power control 1
        case 0x11:                                                       // power control 2
        case 0x12:                                                       // power control 3
        case 0x13:                                                       // power control 4
            break;
        case 0x20:                                                       // horizontal GRAM address set
            usPresentColumn = (unsigned short)ulByte;
            break;
        case 0x21:                                                       // vertical GRAM address set
            usPresetRow = (unsigned short)(0x013F - ulByte);
            break;
        case 0x22:
            fnWriteDisplay(ulByte);                                      // draw new display content
            return;                                                      // remain in write mode until CS negated
        case 0x29:                                                       // power control 7
        case 0x2b:                                                       // frame rate and color control
        case 0x30:                                                       // gamma control 1
        case 0x31:                                                       // gamma control 2
        case 0x32:                                                       // gamma control 3
        case 0x35:                                                       // gamma control 4
        case 0x36:                                                       // gamma control 5
        case 0x37:                                                       // gamma control 6
        case 0x38:                                                       // gamma control 7
        case 0x39:                                                       // gamma control 8
        case 0x3c:                                                       // gamma control 9
        case 0x3d:                                                       // gamma control 10
            break;
        case 0x50:                                                       // horizontal address start position
            usStartColumn = (unsigned short)(ulByte & 0xff);
            break;
        case 0x51:                                                       // horizontal address end position
            usEndColumn = (unsigned short)(ulByte & 0xff);
            break;
        case 0x52:                                                       // vertical address start position
            usStartRow = (unsigned short)(ulByte & 0x1ff);
            break;
        case 0x53:                                                       // vertical address end position
            usEndRow = (unsigned short)(ulByte & 0x1ff);
            break;
        case 0xe5:                                                       // not documented but used to set internal vcore voltage
            break;
        case 0x60:                                                       // drive output control 2
        case 0x61:                                                       // base image display control
        case 0x6a:                                                       // vertical scroll
        case 0x80:                                                       // partial image 1 display position
        case 0x81:                                                       // partial image 1 area (start line)
        case 0x82:                                                       // partial image 1 area (end line)
        case 0x83:                                                       // partial image 2 display position
        case 0x84:                                                       // partial image 2 area (start line)
        case 0x85:                                                       // partial image 2 area (end line)
        case 0x90:                                                       // panel interface control 1
        case 0x92:                                                       // panel interface control 2
        case 0x93:                                                       // panel interface control 3
        case 0x95:                                                       // panel interface control 4
        case 0x97:                                                       // panel interface control 5
        case 0x98:                                                       // panel interface control 6
            break;
        #elif defined NOKIA_GLCD_MODE
            #if defined PHILIPS_PCF8833
        case 0xbe:                                                       // power control
        case 0xbd:                                                       // common drive
        case 0xb9:                                                       // refresh set
        case 0xb6:                                                       // display set
        case 0xb5:                                                       // gamma
        case 0xb3:                                                       // gray scale set
        case 0x36:                                                       // access control
        case 0x25:                                                       // contrast
        case 0xb7:                                                       // temperature gradient
            break;
        case 0x2a:                                                       // column address set
            if (iCommand2 == 1) {
                usPresentColumn = usStartColumn = ucByte;
            }
            else {
                usEndColumn = ucByte;
            }
            break;
        case 0x2b:                                                       // page address set
            if (iCommand2 == 1) {
                if (ucByte < 2) {
                    ucByte = 2;
                }
                usPresetRow = usStartRow = (ucByte - 2);
            }
            else {
                usEndRow = ucByte;
            }
            break;
        case 0x2c:                                                       // write to memory
            #else
        case 0xca:                                                       // display control
        case 0xbc:                                                       // display control
            break;
        case 0x81:                                                       // volume control
            break;
        case 0x75:                                                       // page address set
            if (iCommand2 == 1) {
                if (ucByte < 2) {
                    ucByte = 2;
                }
                usPresetRow = usStartRow = (ucByte - 2);
            }
            else {
                usEndRow = ucByte;
            }
            break;
        case 0x15:                                                       // column address set
            if (iCommand2 == 1) {
                usPresentColumn = usStartColumn = ucByte;
            }
            else {
                usEndColumn = ucByte;
            }
            break;
        case 0xbb:                                                       // display control
        case 0x82:                                                       // temperature gradient
        case 0x20:                                                       // power control
            break;
        case 0x5c:                                                       // memory write
            #endif
            {
                static int iPixelCnt = 0;
                static unsigned short usPixelColour = 0;
                unsigned long ulNewPixel;
                switch (iPixelCnt++) {
                case 0:
                    usPixelColour = (ucByte << 4);                       // rrrrgggg content
                    return;
                case 1:
                    usPixelColour |= (ucByte >> 4);                      // bbbb content
                    ulNewPixel = usPixelColour;
                    usPixelColour = (ucByte << 8);                       // next rrrr content
                    break;
                case 2:
                    usPixelColour |= ucByte;                             // ggggbbbb content
                    iPixelCnt = 0;
                    ulNewPixel = usPixelColour;
                    break;
                }
                if ((usPresetRow < GLCD_Y) && (usPresentColumn < GLCD_X)) {
					unsigned long ulChangedBits;
                    ulChangedBits = (ulGraphicPixels[usPresetRow][usPresentColumn] ^ ulNewPixel);
                    ulGraphicPixels[usPresetRow][usPresentColumn] = ulNewPixel;
                    iDataCnt = 0;
                    if ((ulChangedBits != 0) && (iDisplayOn != 0)) { // pixel has changed colour and display is on
					    RECT rect = rectLines[0];
                        POINT point;
                        HDC hdc = GetDC(ghWnd);
                        DWORD dwPixelColor = (((ulNewPixel & 0xf00) << 12) | ((ulNewPixel & 0x0f0) << 8) | ((ulNewPixel & 0x00f) << 4));
						#if defined BIG_PIXEL
                        point.x = rect.left + (2 * usPresentColumn);     // pixel location
                        point.y = rect.top + (2 * usPresetRow);
                        #else
                        point.x = rect.left + usPresentColumn;           // pixel location
                        point.y = rect.top + usPresetRow;
                        #endif
						#if defined BIG_PIXEL
						SetPixelV(hdc, point.x, point.y, dwPixelColor);
						SetPixelV(hdc, point.x + 1, point.y, dwPixelColor);
						SetPixelV(hdc, point.x, (point.y + 1), dwPixelColor);
						SetPixelV(hdc, point.x + 1, (point.y + 1), dwPixelColor);
						#else
						SetPixelV(hdc, point.x, point.y, dwPixelColor);
						#endif
					    ReleaseDC(ghWnd, hdc);  
                    }
                }
                if (ucMemoryDataAccessControl == 0) {
                    usPresentColumn++;
                    if (usPresentColumn > usEndColumn) {
                        usPresentColumn = usStartColumn;
                        usPresetRow++;
                        if (usPresetRow > usEndRow) {
                            usPresetRow = usStartRow;
                        }
                    }
                }
            }
            break;
        #elif defined _HX8347
        case 0x22:
            if (ucMemoryDataAccessControl == 0) {
                usPresentColumn = usStartColumn;
                usPresetRow = usStartRow;
            }
            iDataCnt = 0;
            break;
        case 0x46:                                                       // gamma control
        case 0x47:
        case 0x48:
        case 0x49:
        case 0x4a:
        case 0x4b:
        case 0x4c:
        case 0x4d:
        case 0x4e:
        case 0x4f:
        case 0x50:
        case 0x51:
            break;
        case 0x3e:                                                       // cycle control
            break;
        case 0x1c:                                                       // power control
            break;
        case 0x90:                                                       // SAP
            break;
        case 0x16:                                                       // memory access control
            break;
        case 0x02:                                                       // column-start (high byte)
        case 0x03:                                                       // column-start (low byte)
            break;
        case 0x04:                                                       // column-end (high byte)
        case 0x05:                                                       // column-end (low byte)
            break;
        case 0x06:                                                       // row-start (high byte)
        case 0x07:                                                       // row-start (low byte)
            break;
        case 0x08:                                                       // row-end (high byte)
        case 0x09:                                                       // row-end (low byte)
            break;
        #else
        case 0x01:                                                       // reset
            ucCmd = 0;
            break;
        case 0x11:                                                       // sleep out and boost on
            break;
        case 0x13:                                                       // normal display mode
            break;
        case 0x28:                                                       // display off
            break;
        case 0x29:                                                       // display on
            break;
        case 0x36:                                                       // memory data access control
            break;
        case 0x3a:                                                       // interface pixel format
            break;
        case 0xd7:                                                       // autoload set
            break;
        case 0xe3:                                                       // read from OTP
            break;
            #if defined ST7789S_GLCD_MODE
        case 0x3c:                                                       // write to display memory
            if (ucMemoryDataAccessControl == 0) {
                usPresentColumn = usStartColumn;
                usPresetRow = usStartRow;
            }
            iDataCnt = 0;
            break;
        case 0x2a:                                                       // column address start
            iCommand2 = 4;
            break;
        case 0x2b:                                                       // row address start
            iCommand2 = 4;
            break;
        case 0xb2:                                                       // porch control
            break;
        case 0xb3:                                                       // frame rate control
            break;
        case 0xb7:                                                       // gate control
            break;
        case 0xbb:                                                       // VCOM setting
            break;
        case 0xc0:                                                       // LCM control
            break;
        case 0xc2:                                                       // VDV and VRH command enable
            break;
        case 0xc3:                                                       // VRH set
            break;
        case 0xc4:                                                       // VDV set
            break;
        case 0xc6:                                                       // frame rate control in normal mode
            break;
        case 0xd0:                                                       // power control 1
            break;
        case 0xe0:                                                       // positive voltage gamma control
            break;
        case 0xe1:                                                       // negative voltage gamma control
            break;
        case 0xb4:                                                       // unknown commands
        case 0xbc:
            break;
            #else
        case 0x2a:                                                       // column address start
            iCommand2 = 1;
            break;
        case 0x2b:                                                       // row address start
            iCommand2 = 1;
            break;
        case 0x2c:                                                       // memory write
            if (ucMemoryDataAccessControl == 0) {
                usPresentColumn = usStartColumn;
                usPresetRow = usStartRow;
            }
            iDataCnt = 0;
            break;
        case 0xb7:                                                       // COM/seg scan direction
            break;
        case 0xc0:                                                       // Vop setting
            iCommand2 = 1;
            break;
        case 0xc3:                                                       // bias setting
            break;
        case 0xc4:                                                       // booster setting
            break;
        case 0xe0:                                                       // control OTP read/write
            break;
        case 0xe1:                                                       // OTP control cancel
            break;
            #endif
        #endif
		default:
            _EXCEPTION("Non-implemented command!");                      // exception to warn of error or non-implemented command
			break;
        }
        #if defined MB785_GLCD_MODE                                      // {13}
        ucState = 0xff;                                                  // reset state
        #endif
    }
    #else
	static int iDoubleByteCommand = 0;
    static unsigned char ucPresentCommand;
	static int iParameter = 0;
	static unsigned short usParameter;
#if defined _GLCD_SAMSUNG || defined ST7565S_GLCD_MODE                   // {4}{18}
    bRS ^= 1;                                                            // for temporary compatibility
    ulByte >>= 8;
    if (iDoubleByteCommand != 0) {
        iDoubleByteCommand = 0;
        switch (ucPresentCommand) {
        case 0x81:                                                       // V5 output voltage control
            break;
        case 0xad:                                                       // static indicator on
            break;
        case 0xd5:                                                       // page blink
            ucPageBlink = ucByte;
            break;
        case 0xd2:                                                       // driving mode capability
            break;
        case 0xf0:                                                       // Test - this should not be used
            break;
        }
        return;
    }
#endif

    if (bRS == 0) {
#if defined _GLCD_SAMSUNG || defined ST7565S_GLCD_MODE                   // {4}{18}
        switch (ucByte) {
    #if defined ST7565S_GLCD_MODE 
        case 0x2f:                                                       // power on
        case 0xc8:                                                       // vertical invert
        case 0x26:                                                       // default contrast
        case 0x40:                                                       // display start
            break;
    #endif
        case 0xe2: // reset LCD
            iGraphicFlags[ulByte] &= ~DISPLAY_IS_ON;
            break;
        case 0xaf: // display on
            iGraphicFlags[ulByte] |= DISPLAY_IS_ON;
            break;
        case 0xa4: // static drive on
            break;
        case 0xb8: // set page 0
            ucPageAddress[ulByte] = 0;
            break;
        case 0xb9: // set page 1
            ucPageAddress[ulByte] = 1;
            break;
        case 0xba: // set page 2
            ucPageAddress[ulByte] = 2;
            break;
        case 0xbb: // set page 3
            ucPageAddress[ulByte] = 3;
            break;
    #if !defined ST7565S_GLCD_MODE
        case 0x00: // address (after a page) - ulByte contains chip number
            if (ulByte == 0) {
                ucColumn[ulByte] = (GLCD_X/SAMSUNG_CHIPS) - 3;
            }
            else {
                ucColumn[ulByte] = 0;
            }
            break;
    #endif
        case 0x3e:                                                       // power OFF
            iGraphicFlags[ulByte] = 0;
            fnRedrawDisplay();                                           // refresh display
            break;
        case 0x3f:                                                       // power ON
            iGraphicFlags[ulByte] = DISPLAY_IS_ON;
            fnRedrawDisplay();                                           // refresh display
            break;
        default:
            switch (ucByte & 0xf8) {
            case 0xc0:                                                   // common output mode select normal
                break;
            case 0xb8:
                ucPageAddress[ulByte] = (ucByte & 0x07);                        
                break;
            default:
                switch (ucByte & 0xf0) {
    #if defined ST7565S_GLCD_MODE
                case 0x10:                                               // set x high address
                    ucColumn[0] = (ucByte << 4);
                    break;
                case 0x00:                                               // set x low address
                    ucColumn[0] |= (ucByte & 0x0f);
                    break;
                case 0xb0:                                               // page address                    
                    ucPageAddress[0] = (ucByte & 0x07);
                    break;
    #else
                case 0xb0:                                               // page address                    
                    break;
    #endif
                default:
                    if ((ucByte & 0xc0) == 0x40) {                       // display start line
                        ucColumn[ulByte] = (ucByte & 0x3f) + ((unsigned char)ulByte * (GLCD_X/SAMSUNG_CHIPS));                        
                    }
                    else {                                               // unknown command
                        _EXCEPTION("Unknown command");                   // exception to warn
                    }
                    break;
                }
                break;
            }
            break;
        }
#else
        switch (iParameter) {                                            // collect parameter(s)
		case 0:
			usParameter = ucByte;
			iParameter++;
			break;
		case 1:
			usParameter |= (ucByte << 8);
			iParameter++;
			break;
		}
#endif
	}
    else {                                                               
#if defined _GLCD_SAMSUNG || defined ST7565S_GLCD_MODE                   // {4}{18}
        RECT rect = rectLines[0];
        POINT point;
        HDC hdc = GetDC(ghWnd);
        int k;
        unsigned char ucBit;
        if (iGraphicFlags[ulByte] & DISPLAY_IS_ON) {
            ucBit = 0x01;
        }
        else {
            ucBit = 0x00;
        }
        ucGraphicPixels[ucPageAddress[ulByte]][ucColumn[ulByte]] = ucByte;
        point.x = rect.left;
        point.y = rect.top;
        for (k = 0; k < 8; k++) {
            if (ucByte & ucBit) {
    #if defined BIG_PIXEL
                SetPixelV(hdc, point.x + ucColumn[ulByte]*2, (point.y + k*2) + ucPageAddress[ulByte]*8*2, LCD_PIXEL_COLOUR);
                SetPixelV(hdc, point.x + 1 + ucColumn[ulByte]*2, (point.y + k*2) + ucPageAddress[ulByte]*8*2, LCD_PIXEL_COLOUR);
                SetPixelV(hdc, point.x + ucColumn[ulByte]*2, (point.y + 1 + k*2) + ucPageAddress[ulByte]*8*2, LCD_PIXEL_COLOUR);
                SetPixelV(hdc, point.x + 1 + ucColumn[ulByte]*2, (point.y + 1 + k*2) + ucPageAddress[ulByte]*8*2, LCD_PIXEL_COLOUR);
    #else
                SetPixelV(hdc, point.x + ucColumn[ulByte], (point.y + k) + ucPageAddress[ulByte]*8, LCD_PIXEL_COLOUR);
    #endif
            }
            else {
    #if defined BIG_PIXEL
                SetPixelV(hdc, point.x + ucColumn[ulByte]*2, (point.y + k*2) + ucPageAddress[ulByte]*8*2, LCD_ON_COLOUR);
                SetPixelV(hdc, point.x + 1 + ucColumn[ulByte]*2, (point.y + k*2) + ucPageAddress[ulByte]*8*2, LCD_ON_COLOUR);
                SetPixelV(hdc, point.x + ucColumn[ulByte]*2, (point.y + 1 + k*2) + ucPageAddress[ulByte]*8*2, LCD_ON_COLOUR);
                SetPixelV(hdc, point.x + 1 + ucColumn[ulByte]*2, (point.y + 1 + k*2) + ucPageAddress[ulByte]*8*2, LCD_ON_COLOUR);
    #else
                SetPixelV(hdc, point.x + ucColumn[ulByte], (point.y + k) + ucPageAddress[ulByte]*8, LCD_ON_COLOUR);
    #endif
            }
            ucBit <<= 1;
        }
        ReleaseDC(ghWnd, hdc);

        if (++ucColumn[ulByte] >= LCD_PIXEL_X_REAL) {
            ucColumn[ulByte] = 0;                                        // note that the page doesn't increment
        }
#else
        switch (ucByte) {                                                // interpret command
			case 0x40:                                                   // text home address set (text area start)
                usTextHomeAddress = usParameter;
				break;
			case 0x41:                                                   // text area set (text character on row)
				ucTextColumns = (unsigned char)usParameter;
				break;
			case 0x42:                                                   // graphic home address set
				usGraphicHomeAddress = usParameter;
				break;
			case 0x43:                                                   // graphic area set (number of horizontal bytes)
				ucGraphicColumns = (unsigned char)usParameter;
				break;
			case 0x24:                                                   // set address pointer
				usAddressPointer = usParameter;
				break;
			case 0xc0:                                                   // data auto write set - auto increment address pointer
				{
					unsigned char ucChangedBits = (ucExternalDisplayRAM[usAddressPointer] ^ (unsigned char)usParameter);
				    unsigned char ucNewByte = (unsigned char)usParameter;
		            ucExternalDisplayRAM[usAddressPointer] = ucNewByte;
                    if ((ucChangedBits != 0) && (usAddressPointer < usTextHomeAddress)) { // don't write into text area
					    RECT rect = rectLines[0];
                        POINT point;
                        HDC hdc = GetDC(ghWnd);
                        int k;
                        unsigned char ucBit = 0x80;
						unsigned short usXoffset = (((usAddressPointer - usGraphicHomeAddress) % ucGraphicColumns) * 8);
						unsigned short usYoffset = ((usAddressPointer - usGraphicHomeAddress) / ucGraphicColumns);
						#if defined BIG_PIXEL
						usXoffset *= 2;
						usYoffset *= 2;
                        #endif
                        point.x = rect.left + usXoffset;                 // initial pixel location
                        point.y = rect.top + usYoffset;
                        for (k = 0; k < 8; k++) {                        // for each pixel in byte
							if (ucChangedBits & ucBit) {
								if (ucNewByte & ucBit) {                 // display on
									#if defined BIG_PIXEL
									SetPixelV(hdc, point.x, point.y, LCD_PIXEL_COLOUR);
									SetPixelV(hdc, point.x + 1, point.y, LCD_PIXEL_COLOUR);
									SetPixelV(hdc, point.x, (point.y + 1), LCD_PIXEL_COLOUR);
									SetPixelV(hdc, point.x + 1, (point.y + 1), LCD_PIXEL_COLOUR);
									#else
									SetPixelV(hdc, point.x, point.y, LCD_PIXEL_COLOUR);
									#endif
								}
								else {                                        // display off
									#if defined BIG_PIXEL
									SetPixelV(hdc, point.x, point.y, LCD_ON_COLOUR);
									SetPixelV(hdc, point.x + 1, point.y, LCD_ON_COLOUR);
									SetPixelV(hdc, point.x, (point.y + 1), LCD_ON_COLOUR);
									SetPixelV(hdc, point.x + 1, (point.y + 1), LCD_ON_COLOUR);
									#else
									SetPixelV(hdc, point.x, point.y, LCD_ON_COLOUR);
									#endif
								}
							}
						    #if defined BIG_PIXEL
							point.x += 2;
                            #else
							point.x++;
                            #endif
							ucBit >>= 1;
						}
					    ReleaseDC(ghWnd, hdc);  
					}
					usAddressPointer++;      
				}
				break;
			case 0x81:                                                   // set XOR graphics mode
                ucGraphicsMode = XOR_MODE;
				break;
			case 0x98:                                                   // no text, no cursor
				break;
			default:
                _EXCEPTION("Non-implemented command!!");                 // exception to warn of error or non-implemented command
				break;
		}
#endif
		iParameter = 0; 
    }
    #endif
}
#elif defined SUPPORT_TFT || defined TFT_GLCD_MODE                       // {7}
static void GraphicLCDCommand(bool bRS, unsigned long ulByte)
{
    unsigned long dwPixelColor;
    int i, j;
    POINT point;
	RECT rect = rectLines[0];
    point.y = rect.top;
    unsigned long *ptrSDRAM = (unsigned long *)ulByte;
    unsigned long ulPixelTest;
    unsigned char *ptrPix = (unsigned char *)pPixels;

    if (pPixels != 0) {                                                  // {19} fast method - draw complete LCD content
        for (i = (GLCD_Y - 1); i >= 0; i--) {                            // for each line
            for (j = 0; j < GLCD_X; j++) {
                ulPixelTest = *(ptrSDRAM + j +(GLCD_X * i));
                *ptrPix++ = (unsigned char)(ulPixelTest >> 16);
                *ptrPix++ = (unsigned char)(ulPixelTest >> 8);
                *ptrPix++ = (unsigned char)(ulPixelTest);
            }
        }
        LCD_draw_bmp((rectLcd.left + 7), (rectLcd.top + 6),  GLCD_X, GLCD_Y); // redraw complete bitmap
    }
    else {                                                               // original pixel based drawing - to be phased out

    HDC hdc = GetDC(ghWnd);

    for (i = 0; i < GLCD_Y; i++) {                                       // for each line
		#if defined BIG_PIXEL
		point.x = (rect.left + (2 * NON_DISPLAYED_X));
		#else
		point.x = (rect.left + NON_DISPLAYED_X);
		#endif
        for (j = 0; j < GLCD_X; j++) {                                   // for each pixel in line
            dwPixelColor = ulGraphicPixels[i][j];
            if (*ptrSDRAM != dwPixelColor) {
                ulGraphicPixels[i][j] = dwPixelColor = *ptrSDRAM;
            #if defined BIG_PIXEL
                SetPixelV(hdc, point.x, point.y, dwPixelColor);
                SetPixelV(hdc, point.x + 1, point.y, dwPixelColor);
                SetPixelV(hdc, point.x , (point.y + 1), dwPixelColor);
                SetPixelV(hdc, (point.x + 1), (point.y + 1), dwPixelColor);
            #else
                SetPixelV(hdc, point.x, point.y, dwPixelColor);
            #endif
            }
            ptrSDRAM++;

        #if defined BIG_PIXEL
            point.x += 2;
        #else
            point.x++;
        #endif
        }
        #if defined BIG_PIXEL
		point.y += 2;
        #else
		point.y++;
        #endif
    }
    }
}
#elif defined SLCD_FILE
static unsigned long ulSLCD_segments[SEGMENT_REGISTER_COUNT] = {0};
static void GraphicLCDCommand(bool bRS, unsigned long ulByte)
{
    unsigned long ulChanges = (ulSLCD_segments[bRS] ^ ulByte);
    int iSegmentNumber = (bRS * 32);
    unsigned long ulSegmentBit = 0x00000001;
    ulSLCD_segments[bRS] = ulByte; 
    if (iSLCD_show_segments != 0) {                                      // when in segment display mode don't react to display changes
        return;
    }
    while (ulChanges != 0) {
        if (ulSegmentBit & ulChanges) {
            if (ulByte & ulSegmentBit) {
                fnDrawSegment(iSegmentNumber, 1);                        // segment to be drawn on
            }
            else {
                fnDrawSegment(iSegmentNumber, 0);                        // segment to be drawn off
            }
            ulChanges &= ~ulSegmentBit;
        }
        iSegmentNumber++;
        ulSegmentBit <<= 1;
    }
}
#endif

// Collect and interprete bytes arriving at the display
//
extern "C" int CollectCommand(bool bRS, unsigned long ulByte)            // {17}
{
#if defined SUPPORT_OLED || defined OLED_GLCD_MODE                       // {6}
    GraphicOLEDCommand(bRS, (unsigned char)ulByte);                      // OLED interface
    return 0;
#elif defined SUPPORT_GLCD || defined SUPPORT_TFT || defined GLCD_COLOR || defined SLCD_FILE
    GraphicLCDCommand(bRS, ulByte);
    return 0;
#else                                                                    // character LCD
    int iReturn = 0xff;
    if (tDisplayMem.bmode) {                                             // 8-bit mode - byte complete
        iReturn = LCDCommand(bRS, (unsigned char)ulByte);
        DrawLcdLine(ghWnd);
    }
    else {
        if (nibbel) {                                                    // save most significant nibble
            nibbel = 0;
            cmd = (unsigned char)(ulByte & 0xf0);
        }
        else {
            nibbel = 1;
            cmd |= (unsigned char)((ulByte >> 4) & 0x0f);
            iReturn = LCDCommand(bRS, cmd);
            DrawLcdLine(ghWnd);
        }
    }
    return iReturn;                                                      // {17} return the read value (in read cases)
#endif
}

#if defined SUPPORT_GLCD || defined GLCD_COLOR
static unsigned long fnGetLCDMemory(unsigned char ucType)
{
    #if defined GLCD_COLOR || defined CGLCD_GLCD_MODE || defined KITRONIX_GLCD_MODE || defined TFT2N0369_GLCD_MODE // {8}
    static int iColor = 0;
    unsigned long ulNewPixel;
    ulNewPixel = ulGraphicPixels[usPresentColumn][usPresetRow];
    ulNewPixel = ((unsigned char)(ulNewPixel << 3) | ((ulNewPixel << 5) & 0x0000fc00) | ((ulNewPixel << 8) & 0x00f80000));
    switch (++iColor) {
    case 1:
        ulNewPixel >>= 16;
        break;
    case 2:
        ulNewPixel >>= 8;
        break;
    case 3:
        iColor = 0;
        if (++usPresetRow >= GLCD_X) {                                       // auto-increment
            usPresetRow = 0;
            usPresentColumn++;
            if (usPresentColumn >= GLCD_Y) {
                usPresentColumn = 0;
            }
        }
        break;
    }
        #if defined _HX8347
  //ulNewPixel = (((ulNewPixel & 0xff) << 16) | ((ulNewPixel & 0xff00) >> 0) | ((ulNewPixel & 0xff0000) >> 16));
        #endif
    return ulNewPixel;
    #else
    return 0;
    #endif
}
#else
// Return either the CGRAM/DDRAM address pointer or the DDRAM content
//
static unsigned long fnGetLCDMemory(unsigned char ucType)
{
    if (ucType == 'A') {
        if (tDisplayMem.ucRAMselect == CGRAM) {
            return tDisplayMem.ucCGRAMaddr + 0x40;                       // return GCRAM address
        }
        else {
            return tDisplayMem.ucLocX;
        }
    }
    else {
        if (tDisplayMem.ucRAMselect == CGRAM) {
            unsigned int *pCharLine = (UINT*)&font_tbl[tDisplayMem.ucCGRAMaddr/8].font_y0; // return GCRAM address
            pCharLine += tDisplayMem.ucCGRAMaddr%8;

            return (*pCharLine &  0x1f);
        }
        else {
            // This is simplified, so possibly not good for general LCD simulation use...
            unsigned char ucContent = tDisplayMem.ddrRam[(tDisplayMem.ucLocY*64) + tDisplayMem.ucLocX];
            tDisplayMem.ucLocX++;
            return ucContent;
        }
    }
}
#endif

// Simulator interface to read from the display
//
extern "C" unsigned long ReadDisplay(bool bRS)
{
#if defined SUPPORT_GLCD || defined GLCD_COLOR
    if (bRS != 0) {                                                      // read data
        return fnGetLCDMemory(0);
    }
    else {
	    return 0x03;                                                     // ready status
    }
#else
    if (!bRS) {
        return 0;                                                        // simulate ready
    }
    else {
        return (fnGetLCDMemory(0));                                      // return the value of data at the present location
    }
#endif
}

// Simulator interface to control backlight intensity based on a port or a PWM output
//
extern "C" void fnSimBackLights(bool iLightState, int iPercentage)       // {10}
{
#if defined LCD_SIMULATE_BACKLIGHT
    if (nBackLightOn == (int)iLightState) {
        return;                                                          // ignore redraws when no change
    }
    nNewBacklight = iLightState;
    nBackLightPercentage = iPercentage;
  //fnInvalidateLCD();                                                   // ensure all characters are redrawn
    fnRedrawDisplay();                                                   // provoke a redraw
#endif
}

#if defined SLCD_FILE                                                    // {16}
#include "conio.h"
#include "Fcntl.h"
#include "io.h"

typedef struct stSEGMENT_MAPPING
{
    int iBitmapRef;
    int iX_location;
    int iY_location;
} SEGMENT_MAPPING;

typedef struct stBMP_CONTENT
{
    int iWidth;
    int iHeight;
    unsigned char *ptrData;
} BMP_CONTENT;

// Bit map structures as used by Windows
//
typedef struct stBITMAPHEADER {
    unsigned char  bmType[2];
    unsigned char  bmLength[4]; 
    unsigned char  bmReserved[4];
    unsigned char  bmOffBits[4];
} W_BITMAPHEADER;

typedef struct stBITMAPINFO { 
    unsigned char biSize[4]; 
    unsigned char biWidth[4]; 
    unsigned char biHeight[4]; 
    unsigned char biPlanes[2];
    unsigned char biBitCount[2];
    unsigned char biCompression[4];
    unsigned char biSizeImage[4];
    unsigned char biXPelsPerMeter[4];
    unsigned char biYPelsPerMeter[4];
    unsigned char biClrUsed[4];
    unsigned char biClrImportant[4]; 
} W_BITMAPINFO;

#define NEGATIVE_BM        0x00000001
#define INVERTED_BM        0x00000002

#define MAX_BMP_FILE_NAME_LENGTH 256
#define MAX_SEGMENT_COUNT        512
#define MAX_BMP_COUNT            100

static int iBMP_count = 0;

static SEGMENT_MAPPING front_segment_list[MAX_SEGMENT_COUNT];
static BMP_CONTENT     bmp_data[MAX_BMP_COUNT];


#define MAX_MEMORY (1024*1024)                                            // max 1M memory to hold a BMP content before saving
static unsigned char ucInputContent[MAX_MEMORY];

static void fnAddBMP(int refFile, int iBMP_number, unsigned long BMP_Length, W_BITMAPINFO *ptr_bm_info, unsigned long ulPars)
{
    unsigned char ucByte[5] = {'0', 'x', '0', '0', ','};
    unsigned char ucNextByte[4];
    unsigned char ucNextBits = 0;
    int iLine = 0;
    unsigned long ulInput = 0;
    unsigned long ulByteWidth = ptr_bm_info->biBitCount[0];
    unsigned long ulDiscard = 0;
    int iBitWidth = 1;
    int iBitsReady = 0;
    int iByte = 0;
    int iBytesToWrite = 0;
    unsigned char ucBit = 0x80;
    unsigned char ucOutBit = 0x80;
    unsigned short usWidth = ((ptr_bm_info->biWidth[1] << 8) | (ptr_bm_info->biWidth[0]));
    unsigned short usHeight = ((ptr_bm_info->biHeight[1] << 8) | (ptr_bm_info->biHeight[0]));
    unsigned short usLineLength = usWidth;
    switch (ulByteWidth) {
    case 24:
        ulByteWidth = 3;
        break;

    case 1:
        ulByteWidth = 4;
        if (usWidth >= 32) {
            iBitsReady = 32;
        }
        else {
            iBitsReady = usWidth;
        }
        usLineLength -= iBitsReady;
        iBitWidth = iBitsReady;
        break;
    }

    if (iBitWidth > 1) {
        _read(refFile, ucNextByte, ulByteWidth);                         // read first full byte
    }
    if (BMP_Length > ((unsigned long)usLineLength * (unsigned long)usHeight * ulByteWidth)) {
        ulDiscard = ((BMP_Length - (usLineLength * usHeight * ulByteWidth))/usHeight);
    }
    while (BMP_Length >= ulByteWidth) {
        if (iBitWidth > 1) {
            while (iBitsReady-- != 0) {
                if (ucNextByte[iByte] & ucBit) {
                    ucNextBits |= ucOutBit;                              // collect the output byte
                }
                iBytesToWrite++;
                ucBit >>= 1;
                if (ucBit == 0) {
                    ucBit = 0x80;
                    iByte++;
                }
                ucOutBit >>= 1;
                if (ucOutBit == 0) {
                    ucOutBit = 0x80;
                    if (ulPars & NEGATIVE_BM) {
                        ucNextBits = ~ucNextBits;                        // negative
                    }
                    if (ulPars & INVERTED_BM) {
                        ucInputContent[ulInput++] = ucNextBits;
                    }
                    iBytesToWrite = 0;
                    ucNextBits = 0;
                }
            }
            if (iBytesToWrite != 0) {
                if (ulPars & NEGATIVE_BM) {
                    ucNextBits = ~ucNextBits;                            // negative
                }
                if (ulPars & INVERTED_BM) {
                    ucInputContent[ulInput++] = ucNextBits;
                }
            }
            _read(refFile, ucNextByte, ulByteWidth);                     // read next full byte
            ucBit = 0x80;
            ucOutBit = 0x80;
            ucNextBits = 0;
            iByte = 0;
            if (usLineLength != 0) {
                if (usLineLength >= 32) {
                    iBitsReady = 32;
                }
                else {
                    iBitsReady = usLineLength;
                }
            }
            else {
                usLineLength = usWidth;
                if (usWidth >= 32) {
                    iBitsReady = 32;
                }
                else {
                    iBitsReady = usWidth;
                }
            }
            usLineLength -= iBitsReady;
        }
        else {                                                           // 24 bit bitmap
            _read(refFile, ucNextByte, ulByteWidth);                     // read next input
            iBitsReady = 0;
            if (ucNextByte[0] != 0) {
                ucNextBits |= ucBit;
            }
            ucBit >>= 1;
            if ((--usLineLength == 0) || (ucBit == 0)) {
                if (ulPars & NEGATIVE_BM) {
                    ucNextBits = ~ucNextBits;                            // negative
                }
                if (ulPars & INVERTED_BM) {
                    ucInputContent[ulInput++] = ucNextBits;
                }
                if (ucBit != 0) {
                    _read(refFile, ucNextByte, ulDiscard);               // discard to end of line                  
                }
                ucBit = 0x80;
                if (usLineLength == 0) {
                    usLineLength = usWidth;
                }
                ucNextBits = 0;
            }
        }
        BMP_Length -= ulByteWidth;
    }
    if (ulPars & INVERTED_BM) {
        unsigned char *ptrData;
        unsigned short usByteWidth = ((usWidth + 7)/8);
        unsigned long  ulRow = ((ulInput / usByteWidth) - 1);
        unsigned long  ulOutput = (ulRow * usByteWidth);
        bmp_data[iBMP_number].iWidth  = usWidth;                         // pixel width of segment bit map
        bmp_data[iBMP_number].iHeight = ulRow;
        bmp_data[iBMP_number].ptrData = ptrData = (unsigned char *)malloc(ulInput); // get memory for the data content
        while (ulInput--) {
            *ptrData++ = ucInputContent[ulOutput++];                     // enter data
            if (--usByteWidth == 0) {
                ulRow--;
                usByteWidth = ((usWidth + 7)/8);
                ulOutput = (ulRow * usByteWidth);
            }
        }
    }
}

static void fnCreateSegmentBMP(int iBMP_number, CHAR *ptr_file_name)
{
    int iBMP_file;
    W_BITMAPHEADER bm_header;
    W_BITMAPINFO bm_info;
    int iLen = sizeof(bm_header);
    unsigned long BMP_Length;
    unsigned short usOffset;
    unsigned long ulPars = INVERTED_BM;

    #if _VC80_UPGRADE < 0x0600
    iBMP_file = _open(ptr_file_name, (_O_BINARY | _O_RDWR));
    #else
    _sopen_s(&iBMP_file, ptr_file_name, (_O_BINARY | _O_RDWR), _SH_DENYWR, _S_IREAD);
    #endif
    if (iBMP_file < 0) {
        return;                                                          // no file found
    }
    _read(iBMP_file, &bm_header, iLen);                                  // read BMP header
    if ((bm_header.bmType[0] != 'B') || (bm_header.bmType[1] != 'M')) {
        _close(iBMP_file);
        return;                                                          // not a bit map
    }
    _read(iBMP_file, &bm_info, sizeof(bm_info));
    if (bm_info.biCompression[0] != 0) {                                 // compressed BMP not supported
        _close(iBMP_file);
        return;
    }
    switch (bm_info.biBitCount[0]) {
    case 24:
        break;
    case 1:
        break;
    default:
        _close(iBMP_file);
        return;                                                          // not supported color format
    }
    BMP_Length = (bm_info.biSizeImage[3] << 24);                         // bit map content length
    BMP_Length |= (bm_info.biSizeImage[2] << 16);
    BMP_Length |= (bm_info.biSizeImage[1] << 8);
    BMP_Length |= (bm_info.biSizeImage[0]);
    if (BMP_Length == 0) {
        BMP_Length = (bm_header.bmLength[3] << 24);                      // if the content length is zero, take it from the header
        BMP_Length |= (bm_header.bmLength[2] << 16);
        BMP_Length |= (bm_header.bmLength[1] << 8);
        BMP_Length |= (bm_header.bmLength[0]);
        BMP_Length -= bm_header.bmOffBits[0];
    }
    usOffset = ((bm_header.bmOffBits[1] << 8) | bm_header.bmOffBits[0]);
    if (usOffset > (sizeof(W_BITMAPHEADER) + sizeof(W_BITMAPINFO))) {    // skip colour palette info
        unsigned char ucJunkByte;
        usOffset -= (sizeof(W_BITMAPHEADER) + sizeof(W_BITMAPINFO));
        if (usOffset == 8) {                                             // black-white only will have two colours
            while (usOffset--) {
                _read(iBMP_file, &ucJunkByte, 1);
                if (usOffset == 5) {                                     // a 0x00 here is the case that an active pixel is white
                    if (ucJunkByte != 0) {                               // inverted content
                        ulPars ^= NEGATIVE_BM;                           // invert the image colour
                    }
                }
            }
        }
        else {
            while (usOffset--) {
                _read(iBMP_file, &ucJunkByte, 1);
            }
        }
    }
    fnAddBMP(iBMP_file, iBMP_number, BMP_Length, &bm_info, ulPars);                   // generate the mono-chrome bit map content
    _close(iBMP_file);
}

// Initialse the SLCD simulation memory
//
static void fnLoadSLCD(void)
{
    int iLastSpace = 0;
    CHAR *ptrDir;
    int iDirLen = 0;
    int iSLCD;
    CHAR file_name[MAX_BMP_FILE_NAME_LENGTH];
    CHAR cInput;
    int iLineLength;
    int iBMP_number;
    int iSegment_number;
    int iX_location, iY_location;
    int iState = 0;
    #if _VC80_UPGRADE < 0x0600
	iSLCD = _open(SLCD_FILE, (_O_BINARY | _O_RDWR));
    #else
	_sopen_s(&iSLCD, SLCD_FILE, (_O_BINARY | _O_RDWR), _SH_DENYWR, _S_IREAD);
    #endif

    ptrDir = SLCD_FILE;
    while (*ptrDir != 0) {
        if (*ptrDir == '\\') {
            break;
        }
        iDirLen++;
        ptrDir++;
    }
    if (*ptrDir == '\\') {
        memcpy(file_name, (ptrDir - iDirLen), (iDirLen + 1));
        iDirLen++;
    }
    else {
        iDirLen = 0;
    }

    if (iSLCD >= 0) {                                                    // if the file could be found parse it
        while (_read(iSLCD, &cInput, 1) > 0) {                           // while file content
            if (iState < 2) {                                            // start of line
                switch (cInput) {
                case ' ':                                                // white space
                case 0x0f:                                               // tab
                case 0x0a:                                               // CR
                case 0x0d:                                               // LF
                    break;
                case '/':
                    iState++;                                            // first back slach of comment presumed
                    continue;
                case 'B':                                                // BMP
                    iState = 10;
                    iLineLength = iDirLen;
                    continue;
                case 'b':                                                // backplane
                    iState = 20;
                    iLineLength = iDirLen;
                    continue;
                case 'f':                                                // frontplane
                    iState = 30;
                    iSegment_number = 0;
                    iX_location = 0;
                    iY_location = 0;
                    iBMP_number = 0;
                    iLineLength = iDirLen;
                    continue;
                default:                                                 // anything else causes the line to be ignored
                    break;
                }
                iState = 0;
            }
            else {
                if (iState >= 30) {                                      // frontplane
                    switch (cInput) {
                    case 0x0a:                                           // CR
                    case 0x0d:                                           // LF
                        if (iState >= 33) {                              // complete information
                            front_segment_list[iSegment_number].iBitmapRef  = (iBMP_number + 1);
                            front_segment_list[iSegment_number].iX_location = iX_location;
                            front_segment_list[iSegment_number].iY_location = iY_location;
                        }
                        iState = 0;                                      // next line
                        break;
                    case ' ':                                            // next field
                        if (iLastSpace == 0) {
                            if (iState < 34) {
                                iState++;
                            }
                        }
                        iLastSpace = 1;
                        continue;
                    default:
                        if ((iState == 30) && (cInput >= '0') && (cInput <= '9')) { // collect segment number
                            if (iSegment_number) {                       // following digit
                                iSegment_number *= 10;
                                iSegment_number += (cInput - '0');
                            }
                            else {
                                iSegment_number = (cInput - '0');
                            }
                        }
                        else if ((iState == 31) && (cInput >= '0') && (cInput <= '9')) { // collect bitmap number
                            if (iBMP_number) {                           // second digit (0..99 supported)
                                iBMP_number *= 10;
                                iBMP_number += (cInput - '0');
                            }
                            else {
                                iBMP_number = (cInput - '0');
                            }
                        }
                        else if ((iState == 32) && (cInput >= '0') && (cInput <= '9')) { // collect x-coordinate
                            if (iX_location) {                           // following digit
                                iX_location *= 10;
                                iX_location += (cInput - '0');
                            }
                            else {
                                iX_location = (cInput - '0');
                            }
                        }
                        else if ((iState == 33) && (cInput >= '0') && (cInput <= '9')) { // collect y-coordinate
                            if (iY_location) {                           // following digit
                                iY_location *= 10;
                                iY_location += (cInput - '0');
                            }
                            else {
                                iY_location = (cInput - '0');
                            }
                        }
                        break;
                    }
                    iLastSpace = 0;
                }
                else if (iState >= 20) {                                 // backplane
                    switch (cInput) {
                    case 0x0a:                                           // CR
                    case 0x0d:                                           // LF
                        iState = 0;                                      // next line
                        break;
                    }
                }
                else if (iState >= 10) {                                 // bitmap
                    switch (cInput) {
                    case 'M':
                        if (iState == 10) {
                            iState = 11;
                            continue;
                        }
                        break;
                    case 'P':
                        if (iState == 11) {
                            iBMP_number = 0;
                            iState = 12;                                 // BMP type recognised - expect the BMP number (decimal)
                            continue;
                        }
                        break;
                    case '"':
                        if (iState == 12) {                              // start BMP file name
                            iState = 13;
                            iLineLength = iDirLen;
                            continue;
                        }
                        else if (iState == 13) {                         // end of BMP file name
                            file_name[iLineLength++] = 0;                // terminate the file name string
                            fnCreateSegmentBMP(iBMP_number, file_name);
                            iState = 0;
                            continue;
                        }
                        break;
                    case 0x0a:                                           // CR
                    case 0x0d:                                           // LF
                        iState = 0;                                      // next line
                        break;
                    default:
                        if ((iState == 12) && (cInput >= '0') && (cInput <= '9')) {
                            if (iBMP_number) {                           // second digit (0..99 supported)
                                iBMP_number *= 10;
                                iBMP_number += (cInput - '0');
                            }
                            else {
                                iBMP_number = (cInput - '0');
                            }
                            continue;
                        }
                        break;
                    }
                    if (iState == 13) {                                  // collecting file name
                        file_name[iLineLength++] = cInput;
                    }
                }
                else {
                    switch (cInput) {
                    case 0x0a:                                           // CR
                    case 0x0d:                                           // LF
                        iState = 0;                                      // next line
                        break;
                    }
                }
            }
        }
    }
    _close(iSLCD);
}


static void fnDrawSegment(int iSegmentRef, int iOnOff)
{
    int iSegRef = front_segment_list[iSegmentRef].iBitmapRef;
    if (iSegRef == 0) {
        return;                                                          // nothing assigned
    }
    iSegRef--;
    if (bmp_data[iSegRef].ptrData != 0) {                                // if this segment is assigned to a bitmap
        HDC hdc = GetDC(ghWnd);
        RECT rect;
        POINT point;
        int iWidth = bmp_data[iSegRef].iWidth;
        int iHeight = bmp_data[iSegRef].iHeight;
        unsigned char *ptrNextByte = bmp_data[iSegRef].ptrData;
        unsigned char ucNextByte = *ptrNextByte++;
        int iHorizontal = iWidth;
        unsigned char ucBit = 0x80;
        long lOriginalX;

	    rect = rectLines[0];
        point.y = rect.top;

		#if defined BIG_PIXEL
		point.x = (rect.left + 2*NON_DISPLAYED_X);
        point.x += (front_segment_list[iSegmentRef].iX_location * 2);
        point.y += (front_segment_list[iSegmentRef].iY_location * 2);
		#else
		point.x = (rect.left + NON_DISPLAYED_X);
        point.x += front_segment_list[iSegmentRef].iX_location;
        point.y += front_segment_list[iSegmentRef].iY_location;
		#endif
        lOriginalX = point.x;

        while (iHeight-- != 0) {                                         // each line
            while (iHorizontal-- != 0) {                                 // each row
                if ((ucBit & ucNextByte) == 0) {
                    if (iOnOff == 0) {
        #if defined BIG_PIXEL
                        SetPixelV(hdc, point.x, point.y, LCD_ON_COLOUR);
                        SetPixelV(hdc, (point.x + 1), point.y, LCD_ON_COLOUR);
                        SetPixelV(hdc, point.x, (point.y + 1), LCD_ON_COLOUR);
                        SetPixelV(hdc, (point.x + 1), (point.y + 1), LCD_ON_COLOUR);
        #else
                        SetPixelV(hdc, point.x, point.y, LCD_ON_COLOUR);
        #endif
                    }
                    else {
        #if defined BIG_PIXEL
                        SetPixelV(hdc, point.x, point.y, LCD_PIXEL_COLOUR);
                        SetPixelV(hdc, (point.x + 1), point.y, LCD_PIXEL_COLOUR);
                        SetPixelV(hdc, point.x, (point.y + 1), LCD_PIXEL_COLOUR);
                        SetPixelV(hdc, (point.x + 1), (point.y + 1), LCD_PIXEL_COLOUR);
        #else
                        SetPixelV(hdc, point.x, point.y, LCD_PIXEL_COLOUR);
        #endif
                    }
                }
                ucBit >>= 1;
        #if defined BIG_PIXEL
                point.x += 2;
        #else
                point.x += 1;
        #endif
                if (ucBit == 0) {
                    ucNextByte = *ptrNextByte++;
                    ucBit = 0x80;
                }
            }
            if (ucBit != 0x80) {
                ucNextByte = *ptrNextByte++;
                ucBit = 0x80;
            }
            iHorizontal = iWidth;
        #if defined BIG_PIXEL
            point.y += 2;
        #else
            point.y += 1;
        #endif
            point.x = lOriginalX;
        }
        ReleaseDC(ghWnd, hdc);
    }
}

static void fnDrawAllSegments(int iDrawType)
{
    int iSegmentNumber = 0;
    int iSegmentRegister = 0;
    unsigned long ulSegmentBit;
    unsigned long ulByte;
    while (iSegmentRegister < SEGMENT_REGISTER_COUNT) {                  // for each possible reguster
        ulSegmentBit = 0x00000001;
        if (iDrawType != 0) {
            ulByte = 0xffffffff;                                         // force all segments on
        }
        else {
            ulByte = ulSLCD_segments[iSegmentRegister];                  // take segment setting from backup register
        }
        while (ulSegmentBit != 0) {                                      // for each bit in the segment register
            if (ulByte & ulSegmentBit) {
                fnDrawSegment(iSegmentNumber, 1);                        // segment to be drawn on
            }
            else {
                fnDrawSegment(iSegmentNumber, 0);                        // segment to be drawn off
            }
            iSegmentNumber++;
            ulSegmentBit <<= 1;
        }
        iSegmentRegister++;
    }
}

#endif
#endif