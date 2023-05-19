/**********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher

    ---------------------------------------------------------------------
    File:      glcd.h
    Project:   uTasker project
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2016
    *********************************************************************
    11.08.2009 Add FONT_CHAR_LCD                                         {1}     
    11.08.2009 Add GLCD_LINE and GLCD_RECT                               {2}
    15.08.2009 Add GLCD_LINE_BLINK and GLCD_RECT_BLINK                   {3}
    26.09.2009 Add GLCD message routines prototypes                      {4}
    23.11.2009 Struct packing control removed to driver.h
    13.05.2010 Add KITRONIX_GLCD_MODE
    26.06.2010 Add MB785_GLCD_MODE
    05.03.2011 Add windowed picture                                      {5}
    15.03.2015 Add TOUCH_MOUSE_EVENT_MESSAGE struct                      {6}
       
*/

#ifndef _EXCLUDE_WINDOWS_

#ifndef MAX_TEXT_LENGTH
    #define MAX_TEXT_LENGTH             64                               // default text length
#endif

#if defined TFT_GLCD_MODE && !defined CGLCD_GLCD_MODE
    #define COLORREF unsigned long
    #define RGB(r,g,b)                  (COLORREF)((b << 16) | (g << 8) | r)
#else
    #define COLORREF unsigned short
    #if defined _HX8347 || defined KITRONIX_GLCD_MODE || defined TFT2N0369_GLCD_MODE || defined MB785_GLCD_MODE
        #define BLACK 0x0000
        #define BLUE  0x001f
        #define RED   0xf800
        #define GREEN 0x07e0
        #define WHITE 0xffff
        #define RGB(r,g,b)              (COLORREF)(((r << 8) & RED) | ((g << 3) & GREEN) | ((b >> 3) & BLUE))
    #else
        #define RGB(r,g,b)              (COLORREF)(((b << 4) & 0x0f00) | (g & 0x00f0) | (r >> 4))
    #endif
#endif

typedef struct stGLCD_TEXT_POSITION {
    unsigned char  ucMode;
    unsigned short usX;
    unsigned short usY;
    unsigned char  ucFont;
} GLCD_TEXT_POSITION;

__PACK_ON                                                                // compilers using pragmas to control packing will start struct packing from here
typedef struct _PACK stGLCD_TEXT_MESSAGE {
    GLCD_TEXT_POSITION text_position;
	CHAR text[MAX_TEXT_LENGTH];
} GLCD_TEXT_MESSAGE;

typedef struct _PACK stGLCD_SCROLL {
    unsigned char ucMode;
    signed short  sX_scroll;
    signed short  sY_scroll;
} GLCD_SCROLL;

typedef struct _PACK stGLCD_STYLE {
    unsigned char ucMode;
    COLORREF      color;
} GLCD_STYLE;

typedef struct _PACK stRECT_COORDINATES {
    unsigned short usX_start;
    unsigned short usY_start;
    unsigned short usX_end;
    unsigned short usY_end;
} RECT_COORDINATES;

typedef struct _PACK stGLCD_LINE {                                       // {2}
    unsigned char ucMode;
    RECT_COORDINATES line_start_end;
} GLCD_LINE;

typedef struct _PACK stGLCD_LINE_BLINK {                                 // {3}
    unsigned char ucMode;
    RECT_COORDINATES line_start_end;
    DELAY_LIMIT   blink_half_period;
} GLCD_LINE_BLINK;

typedef struct _PACK stGLCD_RECT {                                       // {2}
    unsigned char ucMode;
    RECT_COORDINATES rect_corners;
} GLCD_RECT;

typedef struct _PACK stGLCD_RECT_BLINK {                                 // {3}
    unsigned char ucMode;
    RECT_COORDINATES rect_corners;
    DELAY_LIMIT   blink_half_period;
} GLCD_RECT_BLINK;


typedef struct _PACK stGBITMAP {
    unsigned char ucType;
    unsigned char ucWidth[2];
    unsigned char ucHeight[2];
    unsigned char ucData[1];
} GBITMAP;

#define GBITMAP_

typedef struct _PACK stGLCD_PIC {
    unsigned char  ucMode;
    unsigned short usX;
    unsigned short usY;
    GBITMAP       *ptrPic;                                               // this is guaranteed to be on a long word boundary
} GLCD_PIC;


typedef struct _PACK stGLCD_PIC_WINDOW {                                 // {5}
    GLCD_PIC      glcd_pic;                                              // GLCD_PIC element
    RECT_COORDINATES rect_corners;                                       // rectangle of the part of the picture to be written
} GLCD_PIC_WINDOW;

typedef struct stTOUCH_MOUSE_EVENT_MESSAGE                               // {6}
{
    unsigned short usX;                                                  // present X coordinate
    unsigned short usY;                                                  // present Y coordinate
    unsigned char  ucEvent;                                              // event type
}   TOUCH_MOUSE_EVENT_MESSAGE;

#define TOUCH_MOUSE_PRESS          1
#define TOUCH_MOUSE_RELEASE        2
#define TOUCH_MOUSE_MOVE           3
#define TOUCH_MOUSE_CALIBRATION    4

// Define number labels for the font selections 
//
typedef enum
{
#ifdef EN_FIVE_DOT
	FONT_FIVE_DOT,
#endif
#ifdef EN_FIVE_DOT_FIX
	FONT_FIVE_DOT_FIX,
#endif
#ifdef EN_SIX_DOT
	FONT_SIX_DOT,
#endif
#ifdef EN_SIX_DOT_FIX
	FONT_SIX_DOT_FIX,
#endif
#ifdef EN_SEVEN_DOT
	FONT_SEVEN_DOT,
#endif
#ifdef EN_CHAR_LCD_DOT                                                   // {1}
    FONT_CHAR_LCD,
#endif
#ifdef EN_NINE_DOT
	FONT_NINE_DOT,
#endif
#ifdef EN_TEN_DOT
	FONT_TEN_DOT,
#endif
#ifdef EN_FIFTEEN_DOT
	FONT_FIFTEEN_DOT,
#endif
#ifdef EN_EIGHTEEN_DOT
	FONT_EIGHTEEN_DOT,
#endif
    FONT_COUNT                                                           // this enum is equal to the number of active fonts
} FONT_BASE;

#if defined EN_FIVE_DOT || defined EN_FIVE_DOT_FIX || defined EN_SIX_DOT || defined EN_SIX_DOT_FIX || defined EN_SEVEN_DOT || defined EN_CHAR_LCD_DOT || defined EN_NINE_DOT || defined EN_TEN_DOT || defined EN_FIFTEEN_DOT || defined EN_EIGHTEEN_DOT
    #define FONTS_AVAILABLE
#endif

// Bit map structures as used by Windows
//
typedef struct _PACK stBITMAPHEADER {
    unsigned char  bmType[2];
    unsigned char  bmLength[4]; 
    unsigned char  bmReserved[4];
    unsigned char  bmOffBits[4];
} W_BITMAPHEADER;

typedef struct _PACK stBITMAPINFO { 
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
__PACK_OFF

#define REDRAW                  0x80
#define GIVE_ACK                0x40
#define EXTENDED_MODE           0x20                                     // additional control bytes
#define PAINT_LIGHT             0x00
#define PAINT_DARK              0x01
#define PAINT_INVERTED          0x02
#define BLINKING_OBJECT         0x04
#define KILL_BLINK_OBJECTS      0x04                                     // used together with scrolling command
#define WINDOWED_PIC            0x08                                     // {5} GLCD_PIC_WINDOW type instead of GLCD_PIC

#define STYLE_MASK              0x0f
#define STYLE_BACKGROUND_COLOR  0x00
#define STYLE_PIXEL_COLOR       0x01


extern void fnDoLCD_text(GLCD_TEXT_POSITION *text_pos, const CHAR *cText); // {4}
extern void fnDoLCD_pic(void *pic);                                      // {5}
extern void fnDoLCD_line(void *line);
extern void fnDoLCD_rect(void *rect);
extern void fnDoLCD_scroll(GLCD_SCROLL *scroll);
extern void fnDoLCD_style(GLCD_STYLE *style);


#endif
