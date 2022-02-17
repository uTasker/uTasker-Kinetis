/***********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher

    ---------------------------------------------------------------------
    File:      lcd.h
    Project:   Simulation file header for LCD in uTasker project
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2016
    *********************************************************************
    
*/

extern int fnInitLCD(RECT &, int iHeight, int iWidth);
extern void LCDinit(int iLines, int iChars);
extern void LCD_draw_bmp(int iX, int iY, int iXsize, int iYsize);
extern int DisplayLCD(HWND hwnd, RECT rect);

extern unsigned long *pPixels;                                           // point to bit map content for fast drawing

typedef struct tagLCDFONT
{
	unsigned int font_y0;
	unsigned int font_y1;
	unsigned int font_y2;
	unsigned int font_y3;
	unsigned int font_y4;
	unsigned int font_y5;
	unsigned int font_y6;
	unsigned int font_y7;
}tLCDFONT;

typedef struct tagLCD_MEM
{
	unsigned char	ddrRam[1024];			                             // display Zeichenspeicher (more than enough)
	unsigned char	ucCGRAMaddr;			                             // adresszeiger in fonttabelle
	unsigned char	ucLocX;					                             // cursor X position
	unsigned char	ucLocY;					                             // cursor Y position
	unsigned char	ucRAMselect;			                             // read/write in CGRAM oder DDRAM
	unsigned char	lmode;					                             // zeilen bit: 0 = 1 zeile 1 = 2 zeilen 2 = 4 Zeilen
	unsigned char	bmode;					                             // bit modus: 0 = 4 bit 1 = 8 bit
	unsigned char	ucLCDshiftEnable;		                             // ganze Anzeige wird beim Schreiben verschoben
	unsigned char	ucLCDon;				                             // 1 = LCD on, 0 = LCD off
	unsigned char	ucCursorBlinkOn;		                             // 1 = blinking cursor, 0 = steady cursor
	unsigned char	ucCursorOn;				                             // lcd/cursor ein/aus bit
	unsigned char	ucCursorInc__Dec;		                             // 1 = inkrementiere Cursor Position beim Schreiben, 0 = dekrementieren
	unsigned char	ucShiftPosition;		                             // anzahl Stellen der LCD Verschiebung
	unsigned char	ucFontType;				                             // font bit: 1 = 5x8 2 = 5x11
	unsigned char	init;					                             // zumindest VORLÄUFIG: 0 = LCD Struktur noch nicht initialisiert
	unsigned char	ucLines;				                             // 1 oder 2
	unsigned char	dummy;
	unsigned char	ucDDRAMLineLength;
	unsigned int	uiDDRamLength;
} tLCD_MEM;


typedef struct
{
	unsigned int	uiCGRamStart;			                             // start der frei definierbaren zeichen in der Fonttabelle 
	unsigned int	uiCGRamLength;			                             // länge dieses Speichers
	unsigned char	ucNrOfDDRAMLines;		                             // anzahl der Speicherzeilen des Display-Speichers (max 2 momentan)
	unsigned char	ucNrOfLCDLines;			                             // anzahl der sichtbaren Zeilen des LCDs (maximal 4 momentan)
	unsigned int	uiDDRAMStartLine[4];	                             // startpunkte der RAM Zeilen
	unsigned int	uiLCDStartLine[4];		                             // startpunkte der LCD Zeilen
	unsigned char	ucNrOfVisibleCharacters;                             // character pro LCD Zeile
	unsigned char	ucDDRAMLineLength[4];	                             // länge der Speicherzeilen
	unsigned int	uiDDRamLength[2];		                             // max Speicherzelle für [0] einzeiligen Modus, [1] zweizeiligen Modus
	unsigned int	uiType;					                             // typ des Displays, wird für Sonderfälle verwendet
											                             // 0 = generischer K0066 Typ ohne Macken
											                             // 1 = Ampire AC164A (4zeiler): Cursor springt im ShiftLeft-Modus von
											                             //     0 auf 7
											                             // 2 = .... andere Marken hier anhängen und im INI-File eintragen

} tLCD_Info;




// Low level display commands
//
#define LCD_CLEAR				0x01                                     // löscht ddram inhalt
#define LCD_RETURN_HOME			0x02                                     // zeile 1 pos 1 

#define LCD_ENTRY_MODE			0x04 
#define LCD_CURSOR_INCREMENT	0x02                                     // 1 = increment, 0 = decrement
#define LCD_DISPLAY_SHIFT		0x01                                     // shift display while writing

#define LCD_ONOFF				0x08 
#define LCD_ON					0x04                                     // 1: display on, 2: display off
#define LCD_CURSOR_ON			0x02                                     // 1: show cursor
#define LCD_CURSOR_BLINK_ON		0x01                                     // 1: blinking cursor

#define UPDATE_CURSOR           0x80                                     // pseudo - to update cursor after turning off


#define LCD_CURSOR_OR_LCD_SHIFT	0x10 
#define LCD_DDRRAM_DISPLAYSHIFT	0x08                                     // 1: shift display, 0: move only cursor
#define LCD_DDRRAM_SHIFT_RIGHT	0x04                                     // 1: shift to right, 0: shift to left


#define LCD_FUNCTION_SET		0x20 
#define LCD_8_BIT_MODE			0x10                                     // 1: 8 bit modus, 0: 4 bit modus
									                                     // warning: ändert die konfiguration für die kommunikation 
									                                     // mit dem proz 
#define LCD_2_LINE				0x08                                     // 1: 2-zeiliges lcd, 0: 1-zeiliges LCD
#define LCD_FONT_TYPE			0x04                                     // 1: larger font (5x11), 0: smaller font (5x8)
#define LCD_4_BIT_MODE			0x00                                     // 4 bit modus! nur dieser ist erlaubt!

#define LCD_CG_RAM_ADR			0x40
#define LCD_DDR_RAM_ADR			0x80

#define N_BIT                   0x8                                      // 2 line mode 
#define F_BIT                   0/*0x4*/                                 // 5 x 7 dot font
#define I_BIT                   0x2                                      // curser increment 
#define S_BIT                   0/*0x1*/                                 // display doesn't shift

#define FONT_5X8	            1
#define FONT_5X11	            2

#define CGRAM	                1
#define DDRAM	                0
