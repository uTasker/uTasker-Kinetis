/***********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher

    ---------------------------------------------------------------------
    File:      WinSimMain.cpp
    Project:   uTasker project
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2017
    *********************************************************************
    18.01.2007 Correct array size                                        {1}
    26.01.2007 Increase ucDoList[10000]; to ensure no overrun {2} and OVERLAPPED data structure in fnSendSerialMessage
               made static {3} to ensure that it stay in scope during the complete write operation.
    13.02.2007 Solve GDI memory leak {4} / Improve LCD refresh           {5}
    15.09.2007 Add M5222X support
    15.09.2007 Add Luminary LM3SXXXX support
    22.09.2007 Add M5221X support
    21.10.2007 Add Enumeration test support                              {6}
    02.11.2007 Add RTS modem control support                             {7}
    13.11.2007 Add port simulator                                        {8}
    25.12.2007 Extended UART support to 4 channels                       {9}
    26.12.2007 Add LPC21XX support
    08.01.2008 Ethernet code made conditional on Ethernet availability   {10}
    28.04.2008 Add shift key monitor and negative toggle (for ADC use)   {11}
    17.06.2008 Remove TA port for 64 pin M5222X and M5221X               {12}
    19.06.2008 Add USB disconnect support                                {13}
    19.06.2008 USB state display added                                   {14}
    19.06.2008 fnInjectUSB() added                                       {15}
    13.08.2008 Add M5225x support                                        {16}
    19.09.2008 Add USB host support                                      {17}
    08.12.2008 Add file select dialogue on simulation files              {18}
    09.12.2008 Read in and save user files                               {19}
    17.12.2008 Add _LM3S3748 / _LM3S3768 / _LM3S3748 / _LM3S5732 support {20}
    24.12.2008 Add SAM7X ADC                                             {21}
    17.01.2009 Add _LM3S2110 support                                     {22}
    18.01.2009 call new intermediate _main() instead of main(). This protects thread operation for reliability {23}
    03.02.2009 Change strcpy to STRCPY to remove VS2008 warning          {24}
    03.02.2009 Add port injection support for M5223X                     {25}
    24.02.2009 Change UART channel configuration to UART_MODE_CONFIG     {26}
    27.02.2009 Correct Port DD on 80pin M5223X devices                   {27}
    06.03.2009 Add _LM3S9B95 support                                     {28}
    17.03.2009 Extend virtual COM support range (using option SIM_COM_EXTENDED) {29}
    17.03.2009 Correct UART3 reception                                   {30}
    25.03.2009 Add M521XX configuration                                  {31}
    27.03.2009 Add M521X configuration                                   {32}
    01.04.2009 Add Luminary ADC                                          {33}
    26.04.2009 Add AVR32                                                 {34}
    05.05.2009 Add graphic LCD support - SUPPORT_GLCD/SUPPORT_OLED/SUPPORT_TFT {35}
    24.06.2009 Ensure port text is deleted when mouse moved from port area{36}
    19.07.2009 Put script file open in a protected region                {37}
    03.08.2009 Add _LM3S2139 support                                     {38}
    15.08.2009 Avoid refreshing complete screen when IP changes          {39}
    24.09.2009 Correct TA on 64 pin Add M521XX                           {40}
    09.10.2009 GLCD_COLOR independently controls LCD
    22.10.2009 Add additional 4 UARTS                                    {41}
    22.10.2009 Correct the handling of IP configuration display reception{42}
    02.11.2009 Add display of keypad LEDs                                {43}
    04.11.2009 Don't delete LCD when redrawing                           {44}
    18.11.2009 Add AVR32 AT32UC3B support                                {45}
    03.12.2009 Add LPC17XX support                                       {46}
    30.01.2010 Add Flexis32 support                                      {47}
    13.02.2010 Add SAM7S                                                 {48}
    23.02.2010 Add LPC214X                                               {49}
    19.04.2010 Add M5282 types in MAPBGA 256                             {50}
    10.04.2010 Adapt PORT_CHANGE message to support more ports           {51}
    13.05.2010 Adapt PORT_CHANGE message to support more ports           {52}
    21.05.2010 Add M520X                                                 {53}
    20.06.2010 Add STM32                                                 {54}
    13.07.2010 Add _LM3S9B90 support                                     {55}
    05.08.2010 Add SUPPORT_TOUCH_SCREEN support                          {56}
    12.10.2010 Add SAM7SExxx                                             {57}
    22.12.2010 Add Kinetis                                               {58}
    25.01.2011 Don't update the value of port input simulation so that a change causes a port refresh {59}
    30.01.2011 Add LPC213x                                               {60}
    11.06.2011 Correct TA on M521x                                       {61}
    17.06.2011 Add M523X                                                 {62}
    19.06.2011 Add _AT32UC3C 144 pin                                     {63}
    14.07.2011 K40 pin change                                            {64}
    19.07.2011 Add SLCD simulation support                               {65}
    25.07.2011 Correct _M5221X UA port on 64 pin package                 {66}
    31.07.2011 Extend to 6 internal UARTs                                {67}
    03.08.2011 Increase width of IP display field to handle DHCP and zero-config text {68}
    20.09.2011 Add AVR32 UC3C bit map                                    {69}
    11.10.2011 Distinguish between Kinetis parts and packages            {70}
    31.10.2011 Add STM32 extended support                                {71}
    14.11.2011 Add LPC1788 support                                       {72}
    26.11.2011 Add SAM3 support                                          {73}
    24.12.2011 Add Kinetis K61 and K70                                   {74}
    12.01.2012 Add STM32 USB                                             {75}
    12.01.2012 Add STM32 F4 image                                        {76}
    08.03.2012 Add SD card image                                         {77}
    20.03.2012 Add LED control via external ports                        {78}
    20.03.2012 Add support for round LEDs                                {79}
    20.03.2012 Add buzzer sound control                                  {80}
    04.04.2012 Add external I/O                                          {81}
    16.04.2012 Kinetis K20 using new method                              {82}
    26.04.2012 New function fnInjectInputChange() and allow front panel with only KEYPAD / BUTTON_KEY_DEFINITIONS {83}
    27.04.2012 Add status bar and x,y coordinate display on keypad/front-panel {84}
    09.09.2012 Allow image (KEYPAD) to be displayed without keys and LEDs {85}
    09.09.2012 Adjust hight of keypad to avoid overlap with project details {86}
    30.11.2012 Add AVR32UC3C 64 pin                                      {87}
    21.01.2013 Recognise escape key when using BUTTON_KEY_DEFINITIONS    {88}
    21.01.2013 Allow x,y coordinates to be displayed when only the keypad/board is displayed {89}
    21.01.2013 Remove requirement for KEYPAD_LEDS to be defined          {90}
    21.01.2013 Allow 16 and 16 bit external ports                        {91}
    21.01.2012 Kinetis K60 using new method                              {92}
    21.01.2012 Add SAM7L                                                 {93}
    22.01.2012 Add SAM3X                                                 {94}
    15.08.2013 Allow user defined chip picture                           {95}
    25.01.2014 Add Kinetis KL support                                    {96}
    24.02.2014 Control LED state when driving port is configured as an input {97}
    09.04.2014 Display port details when hovering over board inputs      {98}
    14.04.2014 Display multiple network IP/MAC addresses                 {99}
    25.06.2014 Add extra operating information in status bar             {100}
    25.06.2014 Maintain COM port strings                                 {101}
    09.01.2015 When UART settings are changed, edit the display string rather than adding a new UART port entry {102}
    16.02.2015 Redraw SLCD when the window has been resized              {103}
    15.03.2015 Add fast LCD draw support                                 {104}
    28.06.2015 Add VYBRID
    27.07.2015 Add TIVA
    03.10.2016 Always update NIC when a setting changes                  {105}
    07.11.2016 Add software version to window title (if avaiable)        {106}
    15.11.2016 Pass complete change state in fnInjectInputChange()       {107}
    24.12.2016 Add fnInjectI2C()                                         {108}
    02.02.2017 Allow sub-ms tick setting                                 {109}
    19.02.2017 Add FT800 emulation                                       {110}
    28.02.2017 Add UARTs 6 and 7                                         {111}

    */

#include "stdafx.h"
#define _EXCLUDE_WINDOWS_
#include "config.h"
#include "resource.h"
#include "WinSim.h"
#include "WinPcap.h"
#include <winsock2.h>
#include <sys/stat.h>
#include <windows.h>
#include <process.h>

#define _WITH_STATUS_BAR
#define BUFSSIZE        512
#if defined _WITH_STATUS_BAR                                             // {84}
    #include <commctrl.h>
    #define IDC_STATUS_BAR_1  1
    static HWND hStatus;
    static unsigned long keypad_x = 0, keypad_y = 0;
    static int iDisplayCoord = 0;
    static char cOperatingDetails[BUFSSIZE] = {0};
#endif

#if _VC80_UPGRADE >= 0x0600
    #include <share.h>
    #define STRCPY strcpy_s
    #define SPRINTF sprintf_s
    #define STRCAT(a, b) strcat_s(a, BUFSSIZE, b)
#else
    #define STRCPY strcpy
    #define STRCAT strcat
    #define SPRINTF sprintf
#endif

#define MAX_LOADSTRING BUFSSIZE

#if !defined IP_NETWORK_COUNT                                            // {99}
    #define IP_NETWORK_COUNT 1
#endif

#if defined nRF24L01_INTERFACE || defined ENC424J600_INTERFACE
#pragma comment(lib, "Ws2_32.lib")
    static void fnUDP_socket(PVOID pvoid);
#endif

#if (defined FT800_GLCD_MODE && defined FT800_EMULATOR)
    static void fnInitFT800_emulator(void);
#endif

// Global variables
//
HINSTANCE hInst;                                                         // present instance
TCHAR szTitle[MAX_LOADSTRING];
TCHAR szWindowClass[MAX_LOADSTRING];
HWND ghWnd = NULL;

// Prototypes
//
ATOM                MyRegisterClass( HINSTANCE hInstance );
BOOL                InitInstance( HINSTANCE, int );
LRESULT CALLBACK    WndProc( HWND, UINT, WPARAM, LPARAM );
LRESULT CALLBACK    About( HWND, UINT, WPARAM, LPARAM );
#if defined ETH_INTERFACE                                                // {10}
    LRESULT CALLBACK  SetNIC( HWND, UINT, WPARAM, LPARAM );
#endif
LRESULT CALLBACK    CardReset( HWND, UINT, WPARAM, LPARAM );
LRESULT CALLBACK    CardWatchdogReset( HWND, UINT, WPARAM, LPARAM );

#include "WinSim.h"
#include "Fcntl.h"
#include "io.h"
#if defined SUPPORT_LCD || defined SUPPORT_GLCD || defined SUPPORT_OLED || defined SUPPORT_TFT || defined GLCD_COLOR || defined SLCD_FILE // {35}{65}
    #include "lcd/lcd.h"
#endif

static HPEN   hPen;
static HPEN   hRedPen;
static HPEN   hGrayPen;
static HPEN   hGreenPen;
static HBRUSH hGreenBrush;
static HBRUSH hRedBrush;
static HBRUSH hGrayBrush;

#if defined ETH_INTERFACE                                                // {10}
    extern int iRxActivity, iTxActivity;                                 // LAN activity display
    static int iLastRxActivity = 0, iLastTxActivity = 0;
#endif

#if defined SUPPORT_KEY_SCAN || defined BUTTON_KEY_DEFINITIONS || defined KEYPAD // {83}{89}
    extern int fnCheckKeypad(int x, int y, int iPressRelease);
#endif
#if defined SUPPORT_KEY_SCAN || defined BUTTON_KEY_DEFINITIONS || defined KEYPAD // {85}
    extern void fnInitKeyPad(RECT &rt, int iOrigWidth, int iMaxTop);
    extern void DisplayKeyPad(HWND hwnd, RECT rect, RECT refresh_rect);
#endif

#if defined SUPPORT_TOUCH_SCREEN                                         // {56}
    extern int fnPenDown(int x, int y, int iPenState);
#endif

#include <commdlg.h>                                                     // {18}

static TCHAR szEthernetFileName[MAX_PATH + 1] = {0};
static TCHAR szPortFileName[MAX_PATH + 1] = {0};

static TCHAR szEthernet[] = TEXT("Wireshark files (*.pcap)\0*.PCAP\0") \
                            TEXT("Ethereal files (*.eth)\0*.ETH\0");
static TCHAR szPortSim[]  = TEXT("uTasker port sim (*.sim)\0*.SIM\0");


extern int main(int argc, char *argv[]);                                 // our link to the embedded code
static int _main(int argc, char *argv[]);                                // {23}
static int iQuit = 0;

static int iShiftPressed = 0;                                            // {11}
static int iInputChange = 0;

#if defined ETH_INTERFACE
    static int iUserNIC = -1;
#endif

static int iLastPort = -1;
static int iPrevPort = -1;
static int iLastBit;
static int iPrevBit = -1;

#if defined SUPPORT_LCD || defined SUPPORT_GLCD  || defined SUPPORT_OLED || defined SUPPORT_TFT || defined GLCD_COLOR // {104}
static HDC clientDeviceContext = 0;
static HDC lcd_device_context = 0;
unsigned long *pPixels = 0;
#endif

#define TOGGLE_PORT    0
#define POSSIBLE_PORT  1
#define PORT_LOCATION  2

#if !defined _EXTERNAL_PORT_COUNT                                        // {81} for compatibility
    #define _EXTERNAL_PORT_COUNT   0
#endif
#if defined _KINETIS
    #if defined KINETIS_KE
        #define _PORTS_AVAILABLE PORTS_AVAILABLE_8_BIT                   // 8 bit ports
    #else
        #define _PORTS_AVAILABLE (PORTS_AVAILABLE + 1)                   // add dedicated ADC port
    #endif
#else
    #define _PORTS_AVAILABLE PORTS_AVAILABLE
#endif

#if defined _HW_NE64
    #if defined _EXE
        #define CHIP_PACKAGE "ne64.bmp"
    #else
        #define CHIP_PACKAGE "..//..//..//Hardware//NE64//GUI//ne64.bmp"
    #endif
    #define PORT_FIRST_LINE     280
    #define PORT_DISPLAY_LEFT   180
    #define START_PORTS_X (PORT_DISPLAY_LEFT)
    #define PORT_TEXT_LENGTH    8
    static  RECT rect_LAN_LED = {174, 85, 221, 140};
#endif
#if defined _HW_SAM3X                                                    // {73}
    #if defined _SAM3X                                                   // {94}
        #if defined _EXE
            #define CHIP_PACKAGE "atsam3x.bmp"
        #else
            #define CHIP_PACKAGE "..//..//..//Hardware//SAM3X//GUI//atsam3x.bmp"
        #endif
        #define PORT_FIRST_LINE 320
    #elif defined _SAM3NX
        #if defined _EXE
            #define CHIP_PACKAGE "atsam3n.bmp"
        #else
            #define CHIP_PACKAGE "..//..//..//Hardware//SAM3X//GUI//atsam3n.bmp"
        #endif
        #define PORT_FIRST_LINE 260
    #elif defined _SAM3SX
        #if defined _EXE
            #define CHIP_PACKAGE "atsam3s.bmp"
        #else
            #define CHIP_PACKAGE "..//..//..//Hardware//SAM3X//GUI//atsam3s.bmp"
        #endif
        #define PORT_FIRST_LINE 260
    #elif defined _SAM3UX
        #if defined _EXE
            #define CHIP_PACKAGE "atsam3u.bmp"
        #else
            #define CHIP_PACKAGE "..//..//..//Hardware//SAM3X//GUI//atsam3u.bmp"
        #endif
        #define PORT_FIRST_LINE 320
    #endif
    #define PORT_DISPLAY_LEFT 85
    #define START_PORTS_X (PORT_DISPLAY_LEFT)
    #define PORT_TEXT_LENGTH 8
    static  RECT rect_LAN_LED = {227, 234, 274, 289};

    #define USB_LEFT   224
    #define USB_TOP    186
    #define USB_RIGHT  276
    #define USB_BOTTOM 206
#endif
#if defined _HW_SAM7X
    #if defined _HW_SAM7S                                                // {48}
        #if defined _EXE
            #define CHIP_PACKAGE "atmelsam7s.bmp"
        #else
            #define CHIP_PACKAGE "..//..//..//Hardware//SAM7X//GUI//atmelsam7s.bmp"
        #endif
        #define PORT_FIRST_LINE 260
        #define PORT_DISPLAY_LEFT 85
        #define START_PORTS_X (PORT_DISPLAY_LEFT)
        #define PORT_TEXT_LENGTH 8
        static  RECT rect_LAN_LED = {227, 234, 274, 289};

        #define USB_LEFT   224
        #define USB_TOP    186
        #define USB_RIGHT  276
        #define USB_BOTTOM 206
    #elif defined _HW_SAM7SE                                             // {57}
        #if defined _EXE
            #define CHIP_PACKAGE "atmelsam7se.bmp"
        #else
            #define CHIP_PACKAGE "..//..//..//Hardware//SAM7X//GUI//atmelsam7se.bmp"
        #endif
        #define PORT_FIRST_LINE 320
        #define PORT_DISPLAY_LEFT 85
        #define START_PORTS_X (PORT_DISPLAY_LEFT)
        #define PORT_TEXT_LENGTH 8
        static  RECT rect_LAN_LED = {227, 234, 274, 289};

        #define USB_LEFT   110 
        #define USB_TOP    91
        #define USB_RIGHT  180
        #define USB_BOTTOM 117
    #elif defined _HW_SAM7L                                                // {93}
        #if defined _EXE
            #define CHIP_PACKAGE "atmelsam7l.bmp"
        #else
            #define CHIP_PACKAGE "..//..//..//Hardware//SAM7X//GUI//atmelsam7l.bmp"
        #endif
        #define PORT_FIRST_LINE 320
        #define PORT_DISPLAY_LEFT 85
        #define START_PORTS_X (PORT_DISPLAY_LEFT)
        #define PORT_TEXT_LENGTH 8
    #else
        #if defined _EXE
            #define CHIP_PACKAGE "atmelsam7x.bmp"
        #else
            #define CHIP_PACKAGE "..//..//..//Hardware//SAM7X//GUI//atmelsam7x.bmp"
        #endif
        #define PORT_FIRST_LINE 320
        #define PORT_DISPLAY_LEFT 85
        #define START_PORTS_X (PORT_DISPLAY_LEFT)
        #define PORT_TEXT_LENGTH 8
        static  RECT rect_LAN_LED = {227, 234, 274, 289};

        #define USB_LEFT   155 
        #define USB_TOP    91
        #define USB_RIGHT  205
        #define USB_BOTTOM 111
    #endif
#endif
#if defined _HW_AVR32                                                    // {34}
    #if defined _AT32UC3B                                                // {45}
        #if defined _EXE
            #define CHIP_PACKAGE "atmelavr32B.bmp"
        #else
            #define CHIP_PACKAGE "..//..//..//Hardware//AVR32//GUI//atmelavr32B.bmp"
        #endif
        #define PORT_FIRST_LINE 280
        #define USB_LEFT   253 
        #define USB_TOP    116
        #define USB_RIGHT  303
        #define USB_BOTTOM 136
    #else
        #if defined _AT32UC3C                                            // {69}
            #if defined _EXE
                #if defined CHIP_64_PIN                                  // {87}
                    #define CHIP_PACKAGE "atmelavr32c_64.bmp"
                    #define PORT_FIRST_LINE 230
                #else
                    #define CHIP_PACKAGE "atmelavr32c.bmp"
                    #define PORT_FIRST_LINE 320
                #endif
            #else
                #if defined CHIP_64_PIN
                    #define CHIP_PACKAGE "..//..//..//Hardware//AVR32//GUI//atmelavr32c_64.bmp"
                    #define PORT_FIRST_LINE 230
                #else
                    #define CHIP_PACKAGE "..//..//..//Hardware//AVR32//GUI//atmelavr32c.bmp"
                    #define PORT_FIRST_LINE 320
                #endif
            #endif
        #else
            #if defined _EXE
                #define CHIP_PACKAGE "atmelavr32.bmp"
            #else
                #define CHIP_PACKAGE "..//..//..//Hardware//AVR32//GUI//atmelavr32.bmp"
            #endif
            #define PORT_FIRST_LINE 320
        #endif
        #define USB_LEFT   277 
        #define USB_TOP    95
        #define USB_RIGHT  338
        #define USB_BOTTOM 119
    #endif
    #define PORT_DISPLAY_LEFT 85
    #define START_PORTS_X (PORT_DISPLAY_LEFT)
    #define PORT_TEXT_LENGTH 8
    #if defined _AT32UC3C && defined CHIP_64_PIN                          // {87}
        static  RECT rect_LAN_LED = {232, 141, 274, 289};
    #else
        static  RECT rect_LAN_LED = {227, 234, 274, 289};
    #endif
#endif
#if defined _M5223X
    #define PORT_TEXT_LENGTH 8
    #if defined _M5222X
        #if defined _EXE
            #define CHIP_PACKAGE "M5222X.bmp"
        #else
            #define CHIP_PACKAGE "..//..//..//Hardware//M5223X//GUI//M5222X.bmp"
        #endif
        #define USB_LEFT   180 
        #define USB_TOP    96
        #define USB_RIGHT  230
        #define USB_BOTTOM 116
    #elif defined _M521X_SDRAM                                           // {50}
        #if defined _EXE
            #define CHIP_PACKAGE "m5216.bmp"
        #else
            #define CHIP_PACKAGE "..//..//..//Hardware//M5223X//GUI//m5216.bmp"
        #endif
    #elif defined _M520X                                                 // {53}
        #if defined _EXE
            #define CHIP_PACKAGE "m5208.bmp"
        #else
            #define CHIP_PACKAGE "..//..//..//Hardware//M5223X//GUI//m5208.bmp"
        #endif
        static  RECT rect_LAN_LED = {282, 63, 333, 118};
        #undef PORT_TEXT_LENGTH
        #define PORT_TEXT_LENGTH 13
    #elif defined _M523X                                                 // {62}
        #if defined _EXE
            #define CHIP_PACKAGE "m5235.bmp"
        #else
            #define CHIP_PACKAGE "..//..//..//Hardware//M5223X//GUI//m5235.bmp"
        #endif
        static  RECT rect_LAN_LED = {282, 63, 333, 118};
        #undef PORT_TEXT_LENGTH
        #define PORT_TEXT_LENGTH 13        
    #elif defined _M523X                                                 // {62}
        #if defined _EXE
            #define CHIP_PACKAGE "m5282.bmp"
        #else
            #define CHIP_PACKAGE "..//..//..//Hardware//M5223X//GUI//m5282.bmp"
        #endif
        #undef PORT_TEXT_LENGTH
        #define PORT_TEXT_LENGTH 13
        static  RECT rect_LAN_LED = {282, 63, 333, 118};
    #elif defined _M52XX_SDRAM                                           // {50}
        #if defined _EXE
            #define CHIP_PACKAGE "m5282.bmp"
        #else
            #define CHIP_PACKAGE "..//..//..//Hardware//M5223X//GUI//m5282.bmp"
        #endif
        #define DOUBLE_COLUMN_PORTS
        #define SECOND_PORT_COLUMN_OFFSET  170
        static  RECT rect_LAN_LED = {282, 63, 333, 118};
    #elif defined _M521XX                                                // {31}
        #if defined _EXE
            #define CHIP_PACKAGE "M521XX.bmp"
        #else
            #define CHIP_PACKAGE "..//..//..//Hardware//M5223X//GUI//M521XX.bmp"
        #endif
    #elif defined _M521X                                                 // {32}
        #if defined _EXE
            #define CHIP_PACKAGE "M521X.bmp"
        #else
            #define CHIP_PACKAGE "..//..//..//Hardware//M5223X//GUI//M521X.bmp"
        #endif
    #elif defined _M5221X
        #if defined _EXE
            #define CHIP_PACKAGE "M5221X.bmp"
        #else
            #define CHIP_PACKAGE "..//..//..//Hardware//M5223X//GUI//M5221X.bmp"
        #endif
        #define USB_LEFT   180 
        #define USB_TOP    96
        #define USB_RIGHT  230
        #define USB_BOTTOM 116
    #elif defined _M5225X
        #if defined _EXE
            #define CHIP_PACKAGE "kirin3.bmp"
        #else
            #define CHIP_PACKAGE "..//..//..//Hardware//M5223X//GUI//kirin3.bmp"
        #endif
        #define USB_LEFT   225 
        #define USB_TOP    111
        #define USB_RIGHT  275
        #define USB_BOTTOM 131
        static  RECT rect_LAN_LED = {229, 207, 276, 262};
    #else
        #if defined _EXE
            #define CHIP_PACKAGE "M5223X.bmp"
        #else
            #define CHIP_PACKAGE "..//..//..//Hardware//M5223X//GUI//M5223X.bmp"
        #endif
        static  RECT rect_LAN_LED = {173, 85, 220, 140};
    #endif
    #if defined _M52XX_SDRAM                                             // {50}
        #define PORT_FIRST_LINE 250
        #define PORT_DISPLAY_LEFT 100
    #elif defined _M520X                                                 // {53}
        #define PORT_FIRST_LINE 250
        #define PORT_DISPLAY_LEFT 166
    #elif defined _M523X                                                 // {62}
        #define PORT_FIRST_LINE 240
        #define PORT_DISPLAY_LEFT 163
    #else
        #define PORT_FIRST_LINE 280
        #define PORT_DISPLAY_LEFT 187
    #endif
    #define START_PORTS_X (PORT_DISPLAY_LEFT)
#endif
#if defined _FLEXIS32                                                    // {47}
    #if defined ETH_INTERFACE
        #if defined _EXE
            #define CHIP_PACKAGE "flexis32eth.bmp"
        #else
            #define CHIP_PACKAGE "..//..//..//Hardware//Flexis32//GUI//flexis32eth.bmp"
        #endif
    #else
        #if defined _EXE
            #define CHIP_PACKAGE "flexis32.bmp"
        #else
            #define CHIP_PACKAGE "..//..//..//Hardware//Flexis32//GUI//flexis32.bmp"
        #endif
    #endif
    static  RECT rect_LAN_LED = {173, 85, 220, 140};
    #define PORT_FIRST_LINE 280
    #define PORT_DISPLAY_LEFT 184
    #define START_PORTS_X (PORT_DISPLAY_LEFT + 9)
    #define PORT_TEXT_LENGTH 8
#endif
#if defined _STR91XF
    #if defined _EXE
        #define CHIP_PACKAGE "str91xf.bmp"
    #else
        #define CHIP_PACKAGE "..//..//..//Hardware//STR91XF//GUI//str91xf.bmp"
    #endif
    #define PORT_FIRST_LINE 320
    #define PORT_DISPLAY_LEFT 185
    #define START_PORTS_X (PORT_DISPLAY_LEFT)
    #define PORT_TEXT_LENGTH 8
    static  RECT rect_LAN_LED = {301, 85, 348, 140};
#endif
#if defined _STM32                                                       // {54}
    #if defined ST_VALUE                                                 // value line
        #if defined _EXE
            #define CHIP_PACKAGE "stm32f100.bmp"
        #else
            #define CHIP_PACKAGE "..//..//..//Hardware//STM32//GUI//stm32f100.bmp"
        #endif
        #define PORT_FIRST_LINE   270
        #define PORT_DISPLAY_LEFT 149
    #else                                                                // connectivity line
        #if defined _STM32F7XX
            #if defined _EXE
                #define CHIP_PACKAGE "stm32f746.bmp"
            #else
                #define CHIP_PACKAGE "..//..//..//Hardware//STM32//GUI//stm32f746.bmp"
            #endif
        #elif defined _STM32F4XX                                         // {76}
            #if defined _EXE
                #define CHIP_PACKAGE "stm32f407.bmp"
            #else
                #define CHIP_PACKAGE "..//..//..//Hardware//STM32//GUI//stm32f407.bmp"
            #endif
        #else
            #if defined _EXE
                #define CHIP_PACKAGE "stm32f107.bmp"
            #else
                #define CHIP_PACKAGE "..//..//..//Hardware//STM32//GUI//stm32f107.bmp"
            #endif
        #endif
        #define PORT_FIRST_LINE   320
        #define PORT_DISPLAY_LEFT 154
    #endif
    #define START_PORTS_X (PORT_DISPLAY_LEFT)
    #define PORT_TEXT_LENGTH 8
    static  RECT rect_LAN_LED = {303, 84, 350, 139};
    #define USB_LEFT   (294)                                             // {75}
    #define USB_TOP    (159)
    #define USB_RIGHT  (346)
    #define USB_BOTTOM (179)
#endif
#if defined _LPC17XX                                                     // {46}
    #if defined _EXE
        #define CHIP_PACKAGE "LPC17XX.bmp"
    #else
        #define CHIP_PACKAGE "..//..//..//Hardware//LPC17XX//GUI//LPC17XX.bmp"
    #endif
    #define USB_LEFT   229
    #define USB_TOP    85
    #define USB_RIGHT  281
    #define USB_BOTTOM 105
    #define PORT_FIRST_LINE 290
    #define PORT_DISPLAY_LEFT 70
    #define START_PORTS_X (PORT_DISPLAY_LEFT)
    #define PORT_TEXT_LENGTH 9
    static  RECT rect_LAN_LED = {229, 163, 276, 203};
#endif
#if defined _VYBRID
    #if defined _EXE
        #define CHIP_PACKAGE "vybrid.bmp"
    #else
        #define CHIP_PACKAGE "..//..//..//Hardware//Vybrid//GUI//vybrid.bmp"
    #endif
    #define USB_LEFT   334
    #define USB_TOP    75
    #define USB_RIGHT  386
    #define USB_BOTTOM 95
    #define PORT_FIRST_LINE 290
    #define PORT_DISPLAY_LEFT 70
    #define START_PORTS_X (PORT_DISPLAY_LEFT)
    #define PORT_TEXT_LENGTH 9
    static  RECT rect_LAN_LED = {(109), (240), (156), (261)};
#elif defined _TIVA
    #if defined _EXE
        #define CHIP_PACKAGE "TIVA_Eth.bmp"
    #else
        #define CHIP_PACKAGE "..//..//..//Hardware//Tiva//GUI//TIVA_Eth.bmp"
    #endif
    #define USB_LEFT   334
    #define USB_TOP    75
    #define USB_RIGHT  386
    #define USB_BOTTOM 95
    #define PORT_FIRST_LINE 240
    #define PORT_DISPLAY_LEFT 70
    #define START_PORTS_X (PORT_DISPLAY_LEFT)
    #define PORT_TEXT_LENGTH 9
    static  RECT rect_LAN_LED = {(109), (240), (156), (261)};
#elif defined _KINETIS                                                   // {58}
    #if defined ETH_INTERFACE                                            // {70}
        #if defined _EXE
            #define CHIP_PACKAGE "kinetis_lan.bmp"
        #else
            #define CHIP_PACKAGE "..//..//..//Hardware//Kinetis//GUI//kinetis_lan.bmp"
        #endif
    #else
        #if defined KINETIS_KL                                           // {96}
            #if defined _EXE
                #define CHIP_PACKAGE "kinetis_kl.bmp"
            #else
                #define CHIP_PACKAGE "..//..//..//Hardware//Kinetis//GUI//kinetis_kl.bmp"
            #endif
        #elif defined KINETIS_KW2X
            #if defined _EXE
                #define CHIP_PACKAGE "kinetis_kw.bmp"
            #else
                #define CHIP_PACKAGE "..//..//..//Hardware//Kinetis//GUI//kinetis_kw.bmp"
            #endif
        #elif defined KINETIS_KE
            #if defined _EXE
                #define CHIP_PACKAGE "kinetis_ke.bmp"
            #else
                #define CHIP_PACKAGE "..//..//..//Hardware//Kinetis//GUI//kinetis_ke.bmp"
            #endif
        #else
            #if defined _EXE
                #define CHIP_PACKAGE "kinetis.bmp"
            #else
                #define CHIP_PACKAGE "..//..//..//Hardware//Kinetis//GUI//kinetis.bmp"
            #endif
        #endif
    #endif
    #define USB_LEFT   334
    #define USB_TOP    75
    #define USB_RIGHT  386
    #define USB_BOTTOM 95
    #define PORT_FIRST_LINE 290
    #define PORT_DISPLAY_LEFT 70
    #define START_PORTS_X (PORT_DISPLAY_LEFT)
    #define PORT_TEXT_LENGTH 9
    static  RECT rect_LAN_LED = {(339), (209), (386), (261)};
#elif defined _LPC23XX
    #if defined _LPC21XX
        #if defined _EXE
        #define CHIP_PACKAGE "LPC21XX.bmp"
        #else
        #define CHIP_PACKAGE "..//..//..//Hardware//LPC23XX//GUI//LPC21XX.bmp"
        #endif
        #define USB_LEFT   229
        #define USB_TOP    101
        #define USB_RIGHT  281
        #define USB_BOTTOM 121
    #elif defined _LPC24XX
        #if defined _EXE
        #define CHIP_PACKAGE "LPC24XX.bmp"
        #else
        #define CHIP_PACKAGE "..//..//..//Hardware//LPC23XX//GUI//LPC24XX.bmp"
        #endif
        #define USB_LEFT   229
        #define USB_TOP    85
        #define USB_RIGHT  281
        #define USB_BOTTOM 105
    #else
        #if defined _EXE
        #define CHIP_PACKAGE "LPC23XX.bmp"
        #else
        #define CHIP_PACKAGE "..//..//..//Hardware//LPC23XX//GUI//LPC23XX.bmp"
        #endif
        #define USB_LEFT   229
        #define USB_TOP    85
        #define USB_RIGHT  281
        #define USB_BOTTOM 105
    #endif
    #define PORT_FIRST_LINE 280
    #define PORT_DISPLAY_LEFT 70
    #define START_PORTS_X (PORT_DISPLAY_LEFT)
    #define PORT_TEXT_LENGTH 9
    static  RECT rect_LAN_LED = {228, 160, 275, 200};
#endif
#if defined _LM3SXXXX
    #if defined _EXE
        #if defined _LM3S10X
            #define CHIP_PACKAGE "lm3s10x.bmp"
        #else
            #if defined DEVICE_WITHOUT_ETHERNET
                #define CHIP_PACKAGE "lm3sxxxx.bmp"
            #else
                #define CHIP_PACKAGE "lm3sxxxxEth.bmp"
            #endif
        #endif
    #else
        #if defined _LM3S10X
            #define CHIP_PACKAGE "..//..//..//Hardware//LM3SXXXX//GUI//lm3s10x.bmp"
        #else
            #if defined DEVICE_WITHOUT_ETHERNET
                #define CHIP_PACKAGE "..//..//..//Hardware//LM3SXXXX//GUI//lm3sxxxx.bmp"
            #else
                #define CHIP_PACKAGE "..//..//..//Hardware//LM3SXXXX//GUI//lm3sxxxxEth.bmp"
            #endif
        #endif   
    #endif
    #if defined _LM3S10X
        #define PORT_FIRST_LINE 225
    #else
        #define PORT_FIRST_LINE 295
    #endif
    #define PORT_DISPLAY_LEFT 187
    #define START_PORTS_X (PORT_DISPLAY_LEFT)
    #define PORT_TEXT_LENGTH 8
    static  RECT rect_LAN_LED = {228, 205, 275, 260};
    #if defined DEVICE_WITHOUT_ETHERNET
        #define USB_LEFT   226 
        #define USB_TOP    213
        #define USB_RIGHT  276
        #define USB_BOTTOM 233
    #else
        #define USB_LEFT   173 
        #define USB_TOP    83
        #define USB_RIGHT  223
        #define USB_BOTTOM 102
    #endif
#endif
#if defined _RX6XX
    #if defined _EXE
        #define CHIP_PACKAGE "atmelavr32.bmp"
    #else
        #define CHIP_PACKAGE "..//..//..//Hardware//AVR32//GUI//atmelavr32.bmp"
    #endif
    #define PORT_FIRST_LINE 320
    #define USB_LEFT   277 
    #define USB_TOP    95
    #define USB_RIGHT  338
    #define USB_BOTTOM 119
    #define PORT_DISPLAY_LEFT 85
    #define START_PORTS_X (PORT_DISPLAY_LEFT)
    #define PORT_TEXT_LENGTH 8
    static  RECT rect_LAN_LED = {227, 234, 274, 289};
#endif

#if defined SUPPORT_LCD || defined SUPPORT_GLCD || defined SUPPORT_OLED || defined SUPPORT_TFT || defined GLCD_COLOR || defined SLCD_FILE // {35}
    #define DEVICE_X_POS 9
#else
    #define DEVICE_X_POS 2
#endif

#if defined SERIAL_INTERFACE                                             // if we have a serial port we simulate it here
    static HANDLE fnConfigureSerialInterface(char cCom, DWORD com_port_speed, UART_MODE_CONFIG Mode); // {26}
    static HANDLE sm_hComm0 = INVALID_HANDLE_VALUE;
    static HANDLE sm_hComm1 = INVALID_HANDLE_VALUE;
    static HANDLE sm_hComm2 = INVALID_HANDLE_VALUE;
    static HANDLE sm_hComm3 = INVALID_HANDLE_VALUE;                      // {9}
    static HANDLE sm_hComm4 = INVALID_HANDLE_VALUE;                      // {67}
    static HANDLE sm_hComm5 = INVALID_HANDLE_VALUE;
    static HANDLE sm_hComm6 = INVALID_HANDLE_VALUE;                      // {111}
    static HANDLE sm_hComm7 = INVALID_HANDLE_VALUE;
    #if NUMBER_EXTERNAL_SERIAL > 0                                       // {41}
        static HANDLE sm_hCommExt_0 = INVALID_HANDLE_VALUE;
        static HANDLE sm_hCommExt_1 = INVALID_HANDLE_VALUE;
        static HANDLE sm_hCommExt_2 = INVALID_HANDLE_VALUE;
        static HANDLE sm_hCommExt_3 = INVALID_HANDLE_VALUE;
    #endif
    static DWORD fnCheckRx(HANDLE m_hComm, unsigned char *pData);
    static DWORD fnSendSerialMessage(HANDLE m_hComm, const void* lpBuf, DWORD dwCount);
    static void fnProcessRx(unsigned char *ptrData, unsigned short usLength, int iPort);
#endif
static void fnProcessKeyChange(void);
static void fnProcessInputChange(void);
static void fnSimPortInputToggle(int iPort, int iPortBit);

#define UTASKER_WIN_WIDTH  500

#if defined SUPPORT_LCD || defined SUPPORT_GLCD || defined SUPPORT_OLED || defined SUPPORT_TFT || defined GLCD_COLOR || defined SLCD_FILE // {35}
    #define UTASKER_WIN_LCD_WIDTH (22 * LCD_CHARACTERS)
#else
    #define UTASKER_WIN_LCD_WIDTH 0
#endif

#if defined _HW_SAM7X
    #if defined _HW_SAM7S                                                // {48}
        #define UTASKER_WIN_HEIGHT 400
    #elif defined _HW_SAM7SE                                             // {57}
        #define UTASKER_WIN_HEIGHT 500
    #else
        #define UTASKER_WIN_HEIGHT 475                                   // {21}
    #endif
#elif defined _HW_SAM3X                                                  // {73}
    #if defined _SAM3UX
        #define UTASKER_WIN_HEIGHT 480
    #else
        #define UTASKER_WIN_HEIGHT 440
    #endif
#elif defined (_LPC23XX)
    #if defined _LPC21XX
        #if defined _LPC214X                                             // {49}
            #define UTASKER_WIN_HEIGHT 420
        #else
            #define UTASKER_WIN_HEIGHT 395
        #endif
    #else
        #define UTASKER_WIN_HEIGHT 500
    #endif
#elif defined _LPC17XX
    #define UTASKER_WIN_HEIGHT 500
#elif defined (_LM3SXXXX) && defined (_LM3S10X)
    #define UTASKER_WIN_HEIGHT 395
#elif defined (_STR91XF) && !(defined CHIP_80_PIN)
    #define UTASKER_WIN_HEIGHT 600
#elif defined (_M5223X)
    #if defined _M5225X                                                  // {16}
        #define UTASKER_WIN_HEIGHT 672
    #else
        #define UTASKER_WIN_HEIGHT 612
    #endif
#elif defined _AT32UC3B
    #define UTASKER_WIN_HEIGHT 440
#elif defined _AT32UC3C && defined CHIP_64_PIN                           // {87}
    #define UTASKER_WIN_HEIGHT 480
#elif defined _STM32
    #define UTASKER_WIN_HEIGHT  (381 + (24 * _PORTS_AVAILABLE))          // {71}
#else
    #define UTASKER_WIN_HEIGHT 580
#endif

static unsigned long ulPortStates[_PORTS_AVAILABLE + _EXTERNAL_PORT_COUNT]     = {0}; // {81}
static unsigned long ulPortFunction[_PORTS_AVAILABLE + _EXTERNAL_PORT_COUNT]   = {0};
static unsigned long ulPortPeripheral[_PORTS_AVAILABLE + _EXTERNAL_PORT_COUNT] = {0};

static TEXTMETRIC Port_tm;

static RECT present_windows_rect;
static RECT present_ports_rect;

#define PORT_LINE_SPACE 20
#define PORT_NAME_LENGTH 8

#if defined _M5222X || defined _M5221X || defined _M521XX || defined _M521X // {32}
static int fnJumpPort(int i)
{
    switch (i) {
    case 2:                                                              // jump LD port
        return 1;
    #if !defined _M521X                                                  // {32}
    case 6:                                                              // jump TD port
        return 1;
    #endif
    case 11:                                                             // jump GP port
        return 1;
    }
    return 0;
}
#endif

static void fnDisplayPorts(HDC hdc)
{
    extern unsigned long fnGetPortMask(int iPortNumber);
    int i, y;
    unsigned long ulBit;
#if defined _HW_NE64
    unsigned long ulMSB = 0x00000080;
    #if defined CHIP_80_PIN
        unsigned long ulPortMask = 0xff;
    #else
        unsigned long ulPortMask = 0x00;
    #endif
#endif
#if defined _STM32                                                       // {54}
    unsigned long ulMSB = 0x00008000;
    unsigned long ulPortMask = 0;
#endif
#if defined _HW_SAM3X                                                    // {73}
    unsigned long ulMSB = 0x80000000;
    unsigned long ulPortMask = 0;
#endif
#if defined _HW_SAM7X
    #if defined _HW_SAM7S                                                // {48}
        #if defined CHIP_48_PIN
    unsigned long ulMSB = 0x80000000;
    unsigned long ulPortMask = 0xffe00000;
        #else
    unsigned long ulMSB = 0x80000000;
    unsigned long ulPortMask = 0;
        #endif
    #elif defined _HW_SAM7SE                                             // {57}
    unsigned long ulMSB = 0x80000000;
    unsigned long ulPortMask = 0;
    #elif defined _HW_SAM7L                                              // {93}
    unsigned long ulMSB = 0x80000000;
    unsigned long ulPortMask = 0xfe000000;
    #else
    unsigned long ulMSB = 0x40000000;
    unsigned long ulPortMask = 0;
    #endif
#endif
#if defined _HW_AVR32                                                    // {34}
    #if defined _AT32UC3B                                                // {45}
    unsigned long ulMSB = 0x80000000;
        #if defined CHIP_48_PIN
    unsigned long ulPortMask = 0xf0000000;
        #else
    unsigned long ulPortMask = 0x00000000;
        #endif
    #elif defined _AT32UC3C                                              // {63}
        #if defined _AT32UC3C && defined CHIP_64_PIN                     // {87}
    unsigned long ulMSB = 0x80000000;
    unsigned long ulPortMask = 0xff06fc00;
        #else
    unsigned long ulMSB = 0x80000000;
    unsigned long ulPortMask = 0xc0060000;
        #endif
    #else
    unsigned long ulMSB = 0x80000000;
    unsigned long ulPortMask = 0x80000000;
    #endif
#endif
#if defined _RX6XX
    unsigned long ulMSB = 0x80000000;
    unsigned long ulPortMask = 0x80000000;
#endif
#if defined _LM3SXXXX
    unsigned long ulMSB = 0x80;
    #if defined _LM3S10X
    unsigned long ulPortMask = 0xc0;
    #elif defined _LM3S2110                                              // {22}
    unsigned long ulPortMask = 0x80;
    #else
    unsigned long ulPortMask = 0;
    #endif
#endif
#if defined _STR91XF
    unsigned long ulMSB = 0x00000080;
    #if defined CHIP_80_PIN
        unsigned long ulPortMask = 0xff;
    #else
        unsigned long ulPortMask = 0x00;
    #endif
#endif
#if defined _LPC23XX || defined _LPC17XX
    #undef PORT_NAME_LENGTH
    #define PORT_NAME_LENGTH 9
    #if defined _LPC21XX
        unsigned long ulMSB = 0x80000000;
        #if defined _LPC214X                                             // {49}
        unsigned long ulPortMask = 0x0d000000;                           // P0.24, P0.26 and P0.27 not available
        #elif defined _LPC213X                                           // {60}
        unsigned long ulPortMask = 0x01000000;                           // P0.24 not available
        #else
            #if (defined LPC2101 || defined LPC2102 || defined LPC2103) && (defined PLCC44)
        unsigned long ulPortMask = 0x04000000;                           // P0.26 not available in LPCC44 housing
            #else
        unsigned long ulPortMask = 0;
            #endif
        #endif
    #else
        unsigned long ulMSB = 0x80000000;
        #if defined DEVICE_144_PIN || defined _LPC24XX
        unsigned long ulPortMask = 0;
        #elif defined _LPC17XX && defined DEVICE_80_PIN                  // {46}
        unsigned long ulPortMask = 0x99b87030;
        #else
        unsigned long ulPortMask = 0x80007000;
        #endif
    #endif
#endif
#if defined KINETIS_K00 || defined KINETIS_K20 || defined KINETIS_K60 || defined KINETIS_K61 || defined KINETIS_K64 || defined KINETIS_K64 || defined KINETIS_K70 || defined KINETIS_K80 || defined KINETIS_KL || defined KINETIS_KE || defined KINETIS_KV || defined KINETIS_KW2X // {74}{82}{92}{96}
    #undef PORT_NAME_LENGTH
    #define PORT_NAME_LENGTH 9
    unsigned long ulMSB = (1 << (PORT_WIDTH - 1));
    unsigned long ulPortMask;
#elif defined _KINETIS                                                   // {58}
    #if defined KINETIS_K10
        #define KINETIS_DEVICE 0
    #elif defined KINETIS_K20
        #define KINETIS_DEVICE 1
    #elif defined KINETIS_K30
        #define KINETIS_DEVICE 2
    #elif defined KINETIS_K40
        #define KINETIS_DEVICE 3
    #elif defined KINETIS_K50
        #define KINETIS_DEVICE 4
    #elif defined KINETIS_K51
        #define KINETIS_DEVICE 5
    #elif defined KINETIS_K52
        #define KINETIS_DEVICE 6
    #elif defined KINETIS_K53
        #define KINETIS_DEVICE 7
    #elif defined KINETIS_K60
        #define KINETIS_DEVICE 8
    #endif
    #if defined DEVICE_80_PIN
        #define KINETIS_PACKAGE 0
    #elif defined DEVICE_100_PIN
        #define KINETIS_PACKAGE 1
    #elif defined DEVICE_121_PIN
        #define KINETIS_PACKAGE 2
    #else
        #define KINETIS_PACKAGE 3
    #endif
    const unsigned long _kinetis_ports[10][4][5] = {                 // {70}
        {                                                            // K10
            {                                                        // 80/81 pin package
                0xfff00fc0, 0xfff0f3f0, 0xfffcf000, 0xffffff00, 0xfff0ffc0,  // ports A..E
            },
            {                                                        // 100 pin package
                0xfff00fc0, 0xff00f1f0, 0xfff80000, 0xffffff00, 0xf8f0ff80,// ports A..E
            },
            {                                                        // 121 pin package
                0xdff003c0, 0xff00f030, 0xffe00000, 0xffff0000, 0xf8f0ff80,// ports A..E
            },
            {                                                        // 144 pin package
                0xc0f00000, 0xff00f000, 0xfff00000, 0xffff0000, 0xe0f0e000,// ports A..E
            },
        },
        {                                                            // K20
            {                                                        // 80/81 pin package
                0xfff00fc0, 0xfff0f3f0, 0xfffcf000, 0xffffff00, 0xffffffc0,  // ports A..E
            },
            {                                                        // 100 pin package
                0xfff00fc0, 0xff00f1f0, 0xfff80000, 0xffffff00, 0xf8ffff80,// ports A..E
            },
            {                                                        // 121 pin package
                0xdff003c0, 0xff00f030, 0xffe00000, 0xffff0000, 0xf8ffff80,// ports A..E
            },
            {                                                        // 144 pin package
                0xc0f00000, 0xff00f000, 0xfff00000, 0xffff0000, 0xe0ffe000,// ports A..E
            },
        },
        {                                                            // K30
            {                                                        // 80/81 pin package
                0xfff00fc0, 0xfff0f3f0, 0xfffcf000, 0xffff0300, 0xffffffc0,  // ports A..E
            },
            {                                                        // 100 pin package
                0xfff00fc0, 0xff00f1f0, 0xfff80000, 0xffffff00, 0xf8ffff80,// ports A..E
            },
            {                                                        // 121 pin package
                0xdff003c0, 0xff00f030, 0xffe00000, 0xffff0300, 0xf8ffff80,// ports A..E
            },
            {                                                        // 144 pin package
                0xc0f00000, 0xff00f000, 0xfff00000, 0xffff0300, 0xe0ffe000,// ports A..E
            },
        },
        {                                                            // K40
            {                                                        // 80/81 pin package
                0xfff00fc0, 0xfff0f3f0, 0xfffcf000, 0xffff0300, 0xffffffc0,  // ports A..E
            },
            {                                                        // 100 pin package
                0xfff00fc0, 0xff00f1f0, 0xfff80000, 0xffffff00, 0xf8ffff80,// ports A..E
            },
            {                                                        // 121 pin package
                0xdff003c0, 0xff00f030, 0xffe00000, 0xffff0300, 0xf8ffff80,// ports A..E
            },
            {                                                        // 144 pin package
                0xc0f00000, 0xff00f000, 0xfff00000, 0xffff0300, 0xe0ffe000,// ports A..E
            },
        },
        {                                                            // K50
            {                                                        // 80/81 pin package
                0xfff3ffe0, 0xfff0f3f0, 0xfffcf000, 0xffffff00, 0xffffffff,  // ports A..E
            },
            {                                                        // 100 pin package
                0xfff30fe0, 0xff00f1f0, 0xfff80000, 0xffffff00, 0xffffffc0,// ports A..E
            },
            {                                                        // 121 pin package
                0xdff00bc0, 0xff00f030, 0xfff00000, 0xffff0000, 0xfffff000,// ports A..E
            },
            {                                                        // 144 pin package
                0xc0f00000, 0xff00f000, 0xfff00000, 0xffff0000, 0xefffe000,// ports A..E
            },
        },
        {                                                            // K51
            {                                                        // 80/81 pin package
                0xfff3ffe0, 0xfff0f0f0, 0xfffff000, 0xffffff00, 0xffffffff,  // ports A..E
            },
            {                                                        // 100 pin package
                0xfff30fe0, 0xff00f070, 0xfff8f000, 0xffffff00, 0xffffffc0,// ports A..E
            },
            {                                                        // 121 pin package
                0xdff00bc0, 0xff00f030, 0xfff00000, 0xffff0000, 0xffffff80,// ports A..E
            },
            {                                                        // 144 pin package
                0xc0f00000, 0xff00f000, 0xfff00000, 0xffff0300, 0xefffe000,// ports A..E
            },
        },
        {                                                            // K52
            {                                                        // 80/81 pin package
                0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,  // ports A..E
            },
            {                                                        // 100 pin package
                0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,// ports A..E
            },
            {                                                        // 121 pin package
                0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,// ports A..E
            },
            {                                                        // 144 pin package
                0xc0f00000, 0xff00f000, 0xfff00000, 0xffff0000, 0xefffe000,// ports A..E - only 144 pin available
            },
        },
        {                                                            // K53
            {                                                        // 80/81 pin package
                0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,  // ports A..E
            },
            {                                                        // 100 pin package
                0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,// ports A..E
            },
            {                                                        // 121 pin package
                0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,// ports A..E
            },
            {                                                        // 144 pin package
                0xc0f00000, 0xff00f000, 0xfff00000, 0xffff0300, 0xefffe000,// ports A..E - only 144 pin available
            },
        },
        {                                                            // K60
            {                                                        // 80/81 pin package
                0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,  // ports A..E - not available in 80 pin package
            },
            {                                                        // 100 pin package
                0xfff00fc0, 0xff00f1f0, 0xfff80000, 0xffffff00, 0xf8ffff80,// ports A..E
            },
            {                                                        // 121 pin package
                0xdff003c0, 0xff00f030, 0xffe00000, 0xffff0000, 0xf8ffff80,// ports A..E
            },
            {                                                        // 144 pin package
                0xc0f00000, 0xff00f000, 0xfff00000, 0xffff0000, 0xe0ffe000,// ports A..E
            },
        },
        {                                                            // K70
            {                                                        // 80/81 pin package
                0xfff00fc0, 0xfff0f3f0, 0xfffcf000, 0xffffff00, 0xffffffc0,  // ports A..E
            },
            {                                                        // 100 pin package
                0xfff00fc0, 0xff00f1f0, 0xfff80000, 0xffffff00, 0xf8ffff80,// ports A..E
            },
            {                                                        // 121 pin package
                0xdff003c0, 0xff00f030, 0xffe00000, 0xffff0000, 0xf8ffff80,// ports A..E
            },
            {                                                        // 144 pin package
                0xc0f00000, 0xff00f000, 0xfff00000, 0xffff0000, 0xe0ffe000,// ports A..E
            },
        },
    };
    #undef PORT_NAME_LENGTH
    #define PORT_NAME_LENGTH 9
    unsigned long ulMSB = 0x80000000;
    unsigned long ulPortMask = _kinetis_ports[KINETIS_DEVICE][KINETIS_PACKAGE][0];
#endif
#if defined _M5223X
    unsigned long ulMSB = 0x00000080;
    #if defined _M52XX_SDRAM                                             // {50}
    signed char cPorts[] = "PORT  A vvvvvvvv";
    #elif defined _M520X                                                 // {53}
    signed char cPorts[] = "PORT ?ECH     vvvvvvvv";
    #elif defined _M523X                                                 // {62}
    signed char cPorts[] = "PORT A?DR     vvvvvvvv";
    #else
    signed char cPorts[] = "PORT QS vvvvvvvv";
    #endif
    #if defined _M5222X || defined _M5221X || defined _M521XX || defined _M521X //{32}
        #if defined CHIP_64_PIN
        unsigned long ulPortMask = 0xf0;
        #else
        unsigned long ulPortMask = 0x80;
        #endif
    #elif defined _M52XX_SDRAM                                           // {50}
        unsigned long ulPortMask = 0x00;
    #elif defined _M520X                                                 // {53}
        unsigned long ulPortMask = 0x00;
        #undef PORT_NAME_LENGTH
        #define PORT_NAME_LENGTH 13
    #elif defined _M5225X                                                // {16}
        unsigned long ulPortMask = 0x90;
    #elif defined _M523X                                                 // {62}
        unsigned long ulPortMask = 0x1f;
        #undef PORT_NAME_LENGTH
        #define PORT_NAME_LENGTH 13
    #else
        #if defined CHIP_80_PIN
        unsigned long ulPortMask = 0xf0;
        #else
        unsigned long ulPortMask = 0x80;
        #endif
    #endif
#elif defined _FLEXIS32
    #undef PORT_NAME_LENGTH
    #define PORT_NAME_LENGTH 9
    unsigned long ulMSB = 0x00000080;
    signed char cPorts[PORT_WIDTH + PORT_NAME_LENGTH] = "PORT PTA ";
    unsigned long ulPortMask = 0x00;
#else
    #if defined (_STR91XF)
    char cPorts[PORT_WIDTH + PORT_NAME_LENGTH] = "GPIO 0   ";
    #elif defined (_LPC23XX) || defined _LPC17XX
    signed char cPorts[PORT_WIDTH + PORT_NAME_LENGTH] = "P0.31..0 ";
    #elif defined _HW_AVR32 || defined _VYBRID                           // {34}
    signed char cPorts[PORT_WIDTH + PORT_NAME_LENGTH] = "PORT 0   ";
    #else                                                                // SAM7X
    signed char cPorts[PORT_WIDTH + PORT_NAME_LENGTH] = "PORT A   ";
    #endif
#endif
    unsigned char ucPortWidth = PORT_WIDTH;
    char cPortDetails[200];
    HGDIOBJ hFont = GetStockObject(SYSTEM_FIXED_FONT);
    SetTextColor(hdc, RGB(127, 0, 0));
    SelectObject(hdc, hFont);
    GetTextMetrics(hdc, &Port_tm);

    present_ports_rect = present_windows_rect;
    present_windows_rect.top += (PORT_FIRST_LINE + (18 * (IP_NETWORK_COUNT - 1)));
    present_ports_rect.top = present_windows_rect.top;
    present_windows_rect.left = PORT_DISPLAY_LEFT;       
    for (i = 0; i < (_PORTS_AVAILABLE + _EXTERNAL_PORT_COUNT); i++) {     // {81}
#if defined _M5222X || defined _M5221X || defined _M521XX || defined _M521X // {32}
        if (fnJumpPort(i)) {
            goto _jump_entry;
        }
#elif defined _STM32 || defined LPC1788 || defined _HW_SAM3X || defined KINETIS_K00 || defined KINETIS_K20 || defined KINETIS_K60 || defined KINETIS_K61 || defined KINETIS_K64 || defined KINETIS_K70 || defined KINETIS_K80 || defined KINETIS_KL || defined KINETIS_KE || defined KINETIS_KV || defined KINETIS_KW2X // {72}{73}{74}{82}{92}{96}
        ulPortMask = fnGetPortMask(i);                                   // {71}
#elif defined _HW_SAM7X                                                  // {21}
    #if defined _HW_SAM7S                                                // {48}
        if (i == 1)
    #elif defined _HW_SAM7SE || defined _HW_SAM7L                        // {57}{93}
        if (i == 3)
    #else
        if (i == 2)
    #endif
        {
            ucPortWidth = 4;
            cPorts[0] = 'A';
            cPorts[1] = 'D';
            cPorts[2] = 'C';
            memset(&cPorts[3], ' ', (sizeof(cPorts) - 3)); 
        }
    #if defined _HW_SAM7SE                                               // {57}
        else if (i == 2) {
            ulPortMask = 0xff000000;
        }
    #endif
#elif defined _LM3SXXXX && ((_PORTS_AVAILABLE + _EXTERNAL_PORT_COUNT) != _PORTS_AVAILABLE) // {33}{81} ADC is seperate from GPIO
        if (i == ((_PORTS_AVAILABLE + _EXTERNAL_PORT_COUNT) - 1)) {       // {81}
            ulPortMask = (0xff << ADC_CHANNELS);
            cPorts[0] = 'A';
            cPorts[1] = 'D';
            cPorts[2] = 'C';
            memset(&cPorts[3], ' ', (sizeof(cPorts) - 3)); 
        }
#endif
        ulBit = ulMSB;
        for (y = 0; y < ucPortWidth; y++) {                              // draw each port state
            if (ulPortMask & ulBit) {
                cPorts[y + PORT_NAME_LENGTH] = '-';                      // bit with no function
            }
            else {
                if (ulPortPeripheral[i] & ulBit) {
                    if (ulPortStates[i] & ulBit) {
                        cPorts[y + PORT_NAME_LENGTH] = 'P';              // used for peripheral function ('1' state)
                    }
                    else {
                        cPorts[y + PORT_NAME_LENGTH] = 'p';              // used for peripheral function ('0' state)
                    }
                }
                else {
                    if (ulPortFunction[i] & ulBit) {                     // defined as an output
                        if (ulPortStates[i] & ulBit) {
                            cPorts[y + PORT_NAME_LENGTH] = '1';
                        }
                        else {
                            cPorts[y + PORT_NAME_LENGTH] = '0';
                        }
                    }
                    else {
                        if (ulPortStates[i] & ulBit) {                   // display the input state
                            cPorts[y + PORT_NAME_LENGTH] = '^';
                        }
                        else {
                            cPorts[y + PORT_NAME_LENGTH] = 'v';
                        }                                      
                    }
                }
            }
            ulBit >>= 1;
        }
#if defined _KINETIS && !defined KINETIS_KE
        if (i == PORTS_AVAILABLE) {                                      // handle dedicated ADC inputs
            int b;
            cPorts[0] = 'A';
            cPorts[1] = 'D';
            cPorts[2] = 'C';
            cPorts[3] = ' ';
            cPorts[5] = ' ';
            for (b = 9; b < (9 + 32); b++) {
                if (cPorts[b] != '-') {
                    cPorts[b] = 'A';                                     // mark that the pin has a dedicated analogue function
                }
            }
        }
#endif
        ulPortMask = 0;
#if _EXTERNAL_PORT_COUNT > 0                                             // {81}
        if (i >= _PORTS_AVAILABLE) {                                     // handle external ports
            int iSpaces = 5;
            cPorts[0] = 'E';
            cPorts[1] = 'X';
            cPorts[2] = 'T';
            cPorts[3] = '-';
            cPorts[4] = ((i - _PORTS_AVAILABLE) + '0');                  // supports 10 external 8 bit ports
    #if defined _EXT_PORT_16_BIT                                         // {91}
            ulPortMask = 0xffff0000;
    #else
            ulPortMask = 0xffffff00;
    #endif
            while (cPorts[iSpaces] != ' ') {                             // clear out any port name
                cPorts[iSpaces++] = ' ';
            }
            while (cPorts[iSpaces] == ' ') {                             // find the start of the port bit display
                iSpaces++;
            }
            if (cPorts[iSpaces + 1] == ' ') {                            // this is used to find an port number that has been incremented which we don't want to happen for extended ports
                cPorts[iSpaces] = ' ';
            }
            goto _next_port;
        }
#endif
#if defined _STR91XF
    #if defined CHIP_80_PIN
        switch (cPorts[5]) {
        case '0':
            ulPortMask = 0xff;
            break;
        case '6':
            ulPortMask = 0xff;
            break;
        case '7':
            ulPortMask = 0xff;
            break;
        case '8':
            ulPortMask = 0xff;
            break;
        }
    #endif
#endif
#if defined _KINETIS && !(defined KINETIS_K00 || defined KINETIS_K20 || defined KINETIS_K60 || defined KINETIS_K61 || defined KINETIS_K64 || defined KINETIS_K70 || defined KINETIS_K80 || defined KINETIS_KL || defined KINETIS_KE || defined KINETIS_KV || defined KINETIS_KW2X) // {70}{74}{82}{92}{96}
        if (i >= (_PORTS_AVAILABLE - 1)) {                               // {91}
    #if defined _EXT_PORT_16_BIT    
            ulPortMask = 0xffff0000;
    #else
            ulPortMask = 0xffffff00;
    #endif
        }
        else {
            ulPortMask = _kinetis_ports[KINETIS_DEVICE][KINETIS_PACKAGE][i + 1];
        }
#endif
#if defined _HW_AVR32                                                    // {34}
    #if defined _AT32UC3B                                                // {45}
        if (cPorts[5] == '0') {
            ulPortMask = 0xfffff000;
        }
    #elif defined _AT32UC3C && defined CHIP_64_PIN                       // {87}
        if (cPorts[5] == '0') {
            ulPortMask = 0x3ffffffc;
        }
    #endif
    #if defined CHIP_100_PIN
        if (cPorts[5] == '1') {
            ulPortMask = 0xffffffc0;
        }
    #elif defined _AT32UC3C && defined CHIP_64_PIN                       // {87}
        if (cPorts[5] == '1') {
            ulPortMask = 0xff807fc3;
        }
    #endif
        if (cPorts[5] == '2') {
    #if defined _AT32UC3C                                                // {63}
        #if defined _AT32UC3C && defined CHIP_64_PIN                     // {87}
            ulPortMask = 0x87df87f0;
        #else
            ulPortMask = 0x80000000;
        #endif
    #else
        #if defined CHIP_100_PIN
            ulPortMask = 0xffffffff;
        #else
            ulPortMask = 0xffffc000;
        #endif
    #endif
        }
#elif defined _LM3SXXXX
        switch (cPorts[5]) {
    #if defined _LM3S10X
        case 'B':
            ulPortMask = 0xf0;
            break;
    #endif
    #if defined _LM3S1968 || defined _LM3S1958 || defined _LM3S5732      // {52}
        case 'C':
            ulPortMask = 0xf0;
            break;
    #endif
        case 'D':
    #if !defined _LM3S3748 && !defined _LM3S3768 && !defined _LM3S9B95 && !defined _LM3S9B90 // {20}{28}{55}
        #if defined _LM3S5732
            ulPortMask = 0xe0;
        #elif defined _LM3S2110                                          // {22}
            ulPortMask = 0xfc;
        #else
            ulPortMask = 0xf0;
        #endif
    #endif
            break;
        case 'E':
    #if !defined _LM3S1968 && !defined _LM3S1958 && !defined _LM3S3748 && !defined _LM3S3768 && !defined _LM3S2139 // {20}{38}{52}
        #if defined _LM3S9B95 || defined _LM3S9B90                       // {28}{55}
            ulPortMask = 0xc0;
        #elif defined _LM3S2110                                          // {22}
            ulPortMask = 0xf8;
        #else
            ulPortMask = 0xf0;
        #endif
    #endif
            break;
        case 'F':
    #if !defined _LM3S1968 && !defined _LM3S1958 && !defined _LM3S3748 && !defined _LM3S3768 && !defined _LM3S2139 // {20}{38}{52}
        #if defined _LM3S9B95 || defined _LM3S9B90                       // {28}{55}
            ulPortMask = 0x7c;
        #else
            ulPortMask = 0xfc;
        #endif
    #endif
            break;
        case 'G':
    #if defined _LM3S1968 || defined _LM3S1958 || defined _LM3S2139      // {38}{52}
            ulPortMask = 0xf0;
    #elif defined _LM3S3748 || defined _LM3S3768                         // {20}
            ulPortMask = 0xe0;
    #elif defined _LM3S2110                                              // {22}
            ulPortMask = 0xfc;
    #endif
            break;
    #if defined _LM3S9B90
        case 'H':
            ulPortMask = 0xf8;
            break;
    #endif
    #if _PORTS_AVAILABLE >= 9                                            // {28}
        case 'I':
            cPorts[5]++;                                                 // jump port I to go to J
            break;
    #endif
        }
#elif defined _M5223X
        switch (cPorts[6]) {
    #if defined _M520X                                                   // {53}
        case 'U':
            cPorts[6] = 'E';
            cPorts[7] = ' ';
            cPorts[8] = ' ';
            cPorts[9] = ' ';
            cPorts[10] = ' ';
            ulPortMask = 0xf1;            
            break;
        case 'S':
            if (cPorts[5] == 'C') {
                cPorts[5] = 'F';
                cPorts[6] = 'E';
                cPorts[7] = 'C';
                cPorts[8] = 'I';
                cPorts[9] = '2';
                cPorts[10] = 'C';
            }
            else {
                cPorts[5] = 'T';
                cPorts[6] = 'I';
                cPorts[7] = 'M';
                cPorts[8] = 'E';
                cPorts[9] = 'R';
                ulPortMask = 0x6d;
            }
            break;
        case 'A':
            cPorts[5] = 'Q';
            cPorts[6] = 'S';
            cPorts[7] = 'P';
            cPorts[8] = 'I';
            cPorts[9] = ' ';
            cPorts[10] = ' ';
            ulPortMask = 0xf0;
            break;
        case 'I':
            cPorts[5] = 'I';
            cPorts[6] = 'R';
            cPorts[7] = 'Q';
            cPorts[8] = ' ';
            cPorts[9] = ' ';
            cPorts[10] = ' ';
            break;

    #elif defined _M523X                                                 // {62}
        case '?':
            cPorts[6] = 'D';
            ulPortMask = 0x0f;
            break;

        case 'D':
            if (cPorts[5] == 'A') {
                cPorts[5] = 'B';
                cPorts[6] = 'S';
                cPorts[7] = ' ';
                cPorts[8] = ' ';
                ulPortMask = 0x00;
            }
            else {
                cPorts[5] = 'T';
                cPorts[6] = 'I';
                cPorts[7] = 'M';
                cPorts[8] = 'E';
                cPorts[9] = 'R';
                ulPortMask = 0xfc;            
            }
            break;

        case 'S':
            if (cPorts[5] == 'B') {
                cPorts[6] = 'U';
                cPorts[7] = 'S';
                cPorts[8] = 'C';
                cPorts[9] = 'T';
                cPorts[10] = 'L';
                ulPortMask = 0x01;
            }
            else if (cPorts[5] == 'Q') {
                cPorts[5] = 'S';
                cPorts[6] = 'D';
                cPorts[7] = 'R';
                cPorts[8] = 'A';
                cPorts[9] = 'M';
                ulPortMask = 0x00;
            }
            else {
                cPorts[5] = 'D';
                cPorts[6] = 'A';
                cPorts[7] = 'T';
                cPorts[8] = 'A';
                cPorts[9] = 'H';
                ulPortMask = 0x00;
            }
            break;

        case 'U':
            cPorts[5] = 'C';
            cPorts[6] = 'S';
            cPorts[7] = ' ';
            cPorts[8] = ' ';
            cPorts[9] = ' ';
            cPorts[10] = ' ';
            ulPortMask = 0x00;
            break;

        case 'A':
            if (cPorts[9] == 'H') {
                cPorts[9] = 'L';
                if (cPorts[5] == 'D') {
                    ulPortMask = 0xf0;
                }
            }
            else {
                cPorts[5] = 'F';
                cPorts[6] = 'E';
                cPorts[7] = 'C';
                cPorts[8] = 'I';
                cPorts[9] = '2';
                cPorts[10] = 'C';
                ulPortMask = 0xf8;
            }
            break;

        case 'E':
            cPorts[6] = 'T';
            cPorts[7] = 'P';
            cPorts[8] = 'U';
            cPorts[9] = ' ';
            cPorts[10] = ' ';
            ulPortMask = 0x01;
            break;

        case 'T':
            cPorts[5] = 'I';
            cPorts[6] = 'R';
            cPorts[7] = 'Q';
            cPorts[8] = ' ';
            ulPortMask = 0xe0;
            break;

        case 'R':
            cPorts[5] = 'Q';
            cPorts[6] = 'S';
            cPorts[7] = 'P';
            cPorts[8] = 'I';
            ulPortMask = 0xc0;
            break;

        case 'I':
            cPorts[5] = 'U';
            cPorts[6] = 'A';
            cPorts[7] = 'R';
            cPorts[8] = 'T';
            cPorts[9] = 'H';
            break;
    #endif
        case 'Q':
            cPorts[5] = 'D';
            cPorts[6] = 'D';
            break;
    #if !defined _M523X
        case 'T':
            if ('A' == cPorts[5]) {
    #if defined _M5225X                                                  // {16}
                cPorts[5] = 'D';
                cPorts[6] = 'D';
        #if !defined CHIP_144_PIN
                ulPortMask = 0xff;
        #endif
    #elif defined _M52XX_SDRAM                                           // {50}
                cPorts[5] = 'N';
                cPorts[6] = 'Q';
                ulPortMask = 0x000000e4;
    #else
                cPorts[5] = 'G';
                cPorts[6] = 'P';
        #if defined CHIP_80_PIN                                          // {27}
                ulPortMask = 0xff;
        #endif
    #endif
            }
    #if defined _M52XX_SDRAM                                             // {50}
            else if ('Q' == cPorts[5]) {
                cPorts[5] = 'S';
                cPorts[6] = 'D';
                ulPortMask = 0x000000f0;
                ulMSB = 0x00000008;
                ucPortWidth = 4;
            }
    #endif
            else {
                cPorts[5] = 'A';
                cPorts[6] = 'N';
    #if defined _M5225X                                                  // {16}
                ulPortMask = 0x55;
    #elif defined _M5222X || defined _M5221X || defined _M521XX || defined _M521X //{32}
        #if defined CHIP_64_PIN
                ulPortMask = 0x6d;
        #else
                ulPortMask = 0x01;
        #endif
    #else
        #if defined CHIP_80_PIN
                ulPortMask = 0xe0;
        #else
                ulPortMask = 0x80;
        #endif
    #endif
            }
            break;
    #endif

        case 'O':
    #if defined _M5225X                                                  // {16}
            cPorts[5] = 'N';
            cPorts[6] = 'Q';
            ulMSB = 0x00000008;
            ucPortWidth = 4;
    #else
            cPorts[5] = 'L';
            cPorts[6] = 'D';
        #if defined CHIP_80_PIN
            ulPortMask = 0x6d;
        #else
            ulPortMask = 0x01;
        #endif
    #endif
            break;

    #if defined _M5222X || defined _M5221X || defined _M521XX || defined _M521X //{32}
        case 'P':
            cPorts[5] = 'N';
            cPorts[6] = 'Q';
            ulMSB = 0x00000008;
            ucPortWidth = 4;
        #if defined CHIP_64_PIN && !defined _M521XX && !defined _M521X   // {12}{40}{61}
            ulPortMask = 0x0f;
        #endif
            break;

        case 'U':
            cPorts[5] = 'D';
            cPorts[6] = 'D';
            break;
    #endif

    #if !defined _M523X
        case 'E':
    #if defined _M5225X                                                  // {16}
            if (cPorts[5] == 'D') {
                cPorts[5] = 'T';
                cPorts[6] = 'E';
        #if !defined CHIP_144_PIN
                ulPortMask = 0xff;
        #endif
            }
    #elif defined _M52XX_SDRAM                                           // {50}
            if (cPorts[5] == 'D') {
                cPorts[5] = 'E';
                cPorts[6] = 'H';
            }
            else if (cPorts[5] == 'S') {
                cPorts[5] = 'T';
                cPorts[6] = 'A';
                ulPortMask = 0x000000f0;
                cPorts[12] = ' ';
                cPorts[13] = ' ';
                cPorts[14] = ' ';
                cPorts[15] = ' ';
            }
            else if ('T' == cPorts[5]) {
                cPorts[5] = 'U';
                cPorts[6] = 'A';
            }
    #elif defined _M520X                                                 // {53}
            if (cPorts[5] == '?') {
                cPorts[5] = 'F';
            }
            else if (cPorts[5] == 'B') {
                cPorts[5] = 'C';
                cPorts[6] = 'S';
                ulPortMask = 0xf0;
            }
            else if (cPorts[8] == 'H') {
                cPorts[8] = 'L';
                ulPortMask = 0xf0;
            }
            else if (cPorts[8] == 'L') {
                cPorts[5] = 'B';
                cPorts[6] = 'U';
                cPorts[7] = 'S';
                cPorts[8] = 'C';
                cPorts[9] = 'T';
                cPorts[10] = 'L';
                ulPortMask = 0xf0;
            }
            else if (cPorts[8] == 'I') {
                cPorts[5] = 'U';
                cPorts[6] = 'A';
                cPorts[7] = 'R';
                cPorts[8] = 'T';
                cPorts[9] = ' ';
                cPorts[10] = ' ';
                ulPortMask = 0xf0;
            }
    #else
            if (cPorts[5] == 'T') {
                cPorts[5] = 'U';
                cPorts[6] = 'A';
            }
            else {
                cPorts[5] = 'N';
                cPorts[6] = 'Q';
                ulMSB = 0x00000008;
                ucPortWidth = 4;
            }
    #endif
            break;
    #endif

    #if defined _M5225X                                                  // {16}
        case 'F':
            if (cPorts[5] == 'T') {
        #if !defined CHIP_144_PIN
                ulPortMask = 0xff;
        #else
                ulPortMask = 0x10;
        #endif
            }
            break;

        #if !defined CHIP_144_PIN
        case 'G':
            if (cPorts[5] == 'T') {
                ulPortMask = 0xff;
            }
            break;
        #endif
    #endif

    #if defined _M52XX_SDRAM                                             // {50}
        case 'I':
            if ('E' == cPorts[5]) {
                cPorts[6] = 'L';
                ulPortMask = 0x000000c0;
            }
            else {
                cPorts[6] = 'J';
            }
            break;

        case 'K':
            cPorts[5] = 'D';
            cPorts[6] = 'D';
            break;

        case 'M':
            cPorts[5] = 'A';
            cPorts[6] = 'S';
            ulPortMask = 0x00000001;
            break;

        case 'S':
            cPorts[5] = 'N';
            cPorts[6] = 'Q';
            break;

        case 'R':
            cPorts[5] = 'Q';
            cPorts[6] = 'A';
            ulPortMask = 0x000000f0;
            break;
        case 'B':
            if ('Q' == cPorts[5]) {
                ulPortMask = 0x00000080;
            }
            else if ('T' == cPorts[5]) {
                ulPortMask = 0x000000f0;
            }
            break;
    #elif !defined _M520X && !defined _M523X                                                // {53}
        case 'R':
            cPorts[5] = 'T';
            cPorts[6] = 'A';
            cPorts[12] = ' ';
            cPorts[13] = ' ';
            cPorts[14] = ' ';
            cPorts[15] = ' ';
            ulMSB = 0x00000008;
            ucPortWidth = 4;
            break;

        case 'B':
            if ('T' == cPorts[5]) {
                cPorts[6] = 'C';
        #if defined CHIP_80_PIN || (defined CHIP_64_PIN && !defined _M5221X) // {32}{66}
                ulPortMask = 0xff;
        #endif
            }
        #if defined CHIP_80_PIN || defined CHIP_64_PIN || defined CHIP_81_PIN
            else if ('U' == cPorts[5]) {
                ulPortMask = 0xff;                                       // UC not available
            }
        #endif
            break;
    #endif

        case 'C':
    #if defined _M52XX_SDRAM                                             // {50}
            if ('Q' == cPorts[5]) {
                cPorts[5] = 'Q';
                cPorts[6] = 'S';
                ulPortMask = 0x000000c0;
            }
            else if ('T' == cPorts[5]) {
                ulPortMask = 0x000000f0;
            }
    #else
            if ('U' == cPorts[5]) {
                ucPortWidth = 4;
                ulMSB = 0x08;
        #if defined _M5222X || defined _M5221X || defined _M521XX || defined _M521X //{32}
                ulPortMask = 0xfc;
        #elif defined  _M5225X                                           // {16}
            #if defined CHIP_144_PIN
                ulPortMask = 0xf8;
            #else
                ulPortMask = 0xfc;
            #endif
        #endif
            }
    #endif
            break;

    #if !defined _M523X
        case 'D':
            if ('U' == cPorts[5]) {
                cPorts[5] = 'A';
                cPorts[6] = 'S';
                ucPortWidth = 8;
                ulMSB = 0x80;
    #if defined _M5222X || defined _M5221X || defined _M521XX || defined _M521X //{32}
        #if !defined CHIP_100_PIN
                ulPortMask = 0xff;
        #endif
    #elif defined _M5225X                                                // {16}
        #if !defined CHIP_144_PIN
                ulPortMask = 0xff;
        #endif
    #else
        #if defined CHIP_80_PIN
                ulPortMask = 0xf7;
        #endif
    #endif
            }
    #if defined _M52XX_SDRAM                                             // {50}
            else if (cPorts[5] == 'T') {
                ulPortMask = 0x000000f0;
            }
    #endif
    #if defined _M5222X || defined _M5221X || defined _M5225X || defined _M521XX // {16}
            else {
                cPorts[5] = 'U';
                cPorts[6] = 'A';
            }
    #endif
            break;
    #endif
        }
#endif
#if defined _LPC23XX || defined _LPC17XX && (!defined _LPC24XX || defined DEVICE_180_PIN) && !defined LPC1788 // {72}
        switch (cPorts[1]) {
    #if defined _LPC24XX
        #if defined DEVICE_180_PIN
        case '1':
            ulPortMask = 0xccc0c000;
            break;
        case '2':
            ulPortMask = 0xf87f0000;
            break;
        case '3':
            ulPortMask = 0x00f00000;
            break;
        #endif
    #else
        case '0':
        #if defined _LPC17XX && defined DEVICE_80_PIN                    // {46}
            ulPortMask = 0x082338ec;
        #elif defined _LPC214X || defined _LPC213X                       // {49}{60}
            ulPortMask = 0x0000ffff;                                     // P1.00 .. P1.15  not available
        #else
            ulPortMask = 0x000038ec;
        #endif
            break;
        case '1':
        #if defined _LPC17XX && defined DEVICE_80_PIN                    // {46}
            ulPortMask = 0xfffff800;
        #else
            ulPortMask = 0xffffc000;
        #endif
            break;
        case '2':
        #if defined DEVICE_144_PIN
            ulPortMask = 0xf87fff00;
        #else
            #if defined _LPC17XX && defined DEVICE_80_PIN                    // {46}
            ulPortMask = 0xffffffff;
            #else
            ulPortMask = 0xf9ffffff;
            #endif
        #endif
            break;

        case '3':
        #if defined DEVICE_144_PIN
            ulPortMask = 0x0cff0000;
        #else
            ulPortMask = 0xcfffffff;
        #endif
            break;
    #endif
        }
#endif
#if _EXTERNAL_PORT_COUNT > 0                                             // {81}
 _next_port:
#endif
#if defined _HW_SAM7X && defined _HW_SAM7L                               // {93}
        if (i == 0) {
            ulPortMask = 0xff800000;
        }
        else if (i == 1) {
            ulPortMask = 0xc0000000;
        }
#endif
        DrawText(hdc, (const char*)cPorts, (PORT_WIDTH + PORT_NAME_LENGTH), &present_windows_rect, 0);
        present_windows_rect.top += PORT_LINE_SPACE;
#if defined _M5222X || defined _M5221X || defined _M521XX || defined _M521X // {32}
_jump_entry:
#endif
#if defined _HW_NE64                                                     // Ports A,B,E,G,H,J,K,L,S,T
        switch (cPorts[5]) {
        case 'A':
            cPorts[5] = 'B';
    #if defined CHIP_80_PIN
            ulPortMask = 0xff;
    #endif
            break;
        case 'B':
            cPorts[5] = 'E';
    #if defined CHIP_80_PIN
            ulPortMask = 0xec;
    #endif
            break;
        case 'E':
            cPorts[5] = 'G';
    #if defined CHIP_80_PIN
            ulPortMask = 0x80;
    #endif
            break;
        case 'G':
            cPorts[5] = 'H';
            ulPortMask = 0x80;
            break;
        case 'H':
            cPorts[5] = 'J';
            ulPortMask = 0x30;            
            break;
        case 'J':
            cPorts[5] = 'K';
    #if defined CHIP_80_PIN
            ulPortMask = 0xff;
    #endif
            break;
        case 'K':
            cPorts[5] = 'L';
    #if defined CHIP_80_PIN
            ulPortMask = 0xe0;
    #else
            ulPortMask = 0x80;
    #endif
            break;
        case 'L':
            cPorts[5] = 'S';
            break;
        case 'S':
            cPorts[5] = 'T';
            ulPortMask = 0x0f;
            break;
        case 'T':
            cPorts[1] = 'A';
            cPorts[2] = 'D';
            cPorts[3] = '7';
            cPorts[4] = '-';
            cPorts[5] = '0';
            break;
        }
#elif defined _FLEXIS32
        if (cPorts[7] == 'H') {
            cPorts[7] = 'J';
        }
        else {
            cPorts[7]++;                                                 // numerous ports
        }
    #if defined CHIP_48_PIN
        if (cPorts[7] == 'E') {
            ulPortMask = 0xc0;
        }
        else if (cPorts[7] >= 'F') {
            ulPortMask = 0xff;
        }
    #elif defined CHIP_64_PIN
        if (cPorts[7] == 'G') {
            ulPortMask = 0xf0;
        }
        else if (cPorts[7] == 'J') {
            ulPortMask = 0xfc;
        }
        else if (cPorts[7] >= 'H') {
            ulPortMask = 0xff;
        }
    #else
        if (cPorts[7] == 'J') {
            ulPortMask = 0xc0;
        }
    #endif
#else
    #if defined (_M5223X)
        #if !defined _M520X && !defined _M523X                           // {53}
        cPorts[6]++;                                                     // numerous ports..
        #endif
        #if defined DOUBLE_COLUMN_PORTS                                  // {50}
        if (i == (_PORTS_AVAILABLE/2)) {                                 // display the second half of port starting at top again
            present_windows_rect.left = PORT_DISPLAY_LEFT + SECOND_PORT_COLUMN_OFFSET;
            present_windows_rect.top = present_ports_rect.top;
        }
        #endif
    #elif defined (_LPC23XX) || defined _LPC17XX
        cPorts[1]++;
    #else
        cPorts[5]++;                                                     // ports A and B
    #endif
#endif
    }
    present_windows_rect.left = 15;                                      // display Port details
#if defined _WITH_STATUS_BAR                                             // {84}
    if (present_windows_rect.top < present_windows_rect.bottom - 40) {
        present_windows_rect.top = present_windows_rect.bottom - 40;
    }
#else
    if (present_windows_rect.top < present_windows_rect.bottom - 45) {
        present_windows_rect.top = present_windows_rect.bottom - 45;
    }
#endif
    cPortDetails[sizeof(cPortDetails) - 1] = 0;
    memset(cPortDetails, ' ', sizeof(cPortDetails) - 1);
    if (iLastPort >= 0) {
        fnSetPortDetails(cPortDetails, iLastPort, iLastBit, ulPortStates, ulPortFunction, ulPortPeripheral, (sizeof(cPortDetails) - 1));
    }
    SetTextColor(hdc, RGB(0, 0, 255));
    SelectObject(hdc, hFont);
    i = strlen(cPortDetails);
    memset(&cPortDetails[i], ' ', (sizeof(cPortDetails) - 1 - i));
    DrawText(hdc, cPortDetails, (sizeof(cPortDetails) - 1), &present_windows_rect, 0);
}

// Allow the keypad/inputs to define which port information should be displayed
//
extern void fnSetLastPort(int iInputLastPort, int iInputPortBit)
{
    static int iLastBitLocation = -1;
    int iPortBitCount = (1 << (PORT_WIDTH - 1));
    int iPortBitRef = 0;
    if ((iLastPort == iInputLastPort) && (iLastBitLocation == iInputPortBit)) { // filter stable input position
        return;
    }
    iLastPort = iInputLastPort;
    iLastBitLocation = iInputPortBit;
    if (iInputLastPort == -1) {                                          // mouse moved away from an input so invalidate display
        return;
    }
    if (iInputPortBit == 0) {
        _EXCEPTION("Invalid port bit - please correct!!");
    }
    while ((iInputPortBit & iPortBitCount) == 0) {                       // convert input reference format
        iPortBitCount >>= 1;
        iPortBitRef++;
    }
    iLastBit = iPortBitRef;
    InvalidateRect(ghWnd, &present_ports_rect, FALSE);                   // cause a redraw of the ports
    UpdateWindow(ghWnd);
}


// Support mouse click to toggle input state / hover to display port details
//
static int fnToggleInput(int x, int y, int iCheck)
{
#if defined DOUBLE_COLUMN_PORTS
    int iPortColumn = 0;
#endif
    int iPortBit = 0, iPort;
    int iSizeX, iSizeY;
    int iStartPortY = (PORT_FIRST_LINE + (18 * (IP_NETWORK_COUNT - 1)));
#if defined _WITH_STATUS_BAR                                             // {98}
    if (iDisplayCoord != 0) {                                            // if the mouse is on the keypad/board we don't check ports
        return 0;
    }
#endif

    x -= (PORT_TEXT_LENGTH * Port_tm.tmAveCharWidth);

    if (x < START_PORTS_X) {                                             // check whether the mouse was on a port
        iLastPort = -1;
        return 0;
    }
    if (y < iStartPortY)  {
        iLastPort = -1;
        return 0;
    }
    x -= START_PORTS_X;
    if (x > (PORT_WIDTH * Port_tm.tmAveCharWidth))  {
#if defined DOUBLE_COLUMN_PORTS                                          // {50}
        x -= SECOND_PORT_COLUMN_OFFSET;
        if ((x < 0) || (x > (PORT_WIDTH * Port_tm.tmAveCharWidth))) {
            iLastPort = -1;
            return 0;
        }
        iPortColumn = ((_PORTS_AVAILABLE + 1)/2);                        // possibly in second port column
#else
        iLastPort = -1;
        return 0;
#endif
    }
#if defined DOUBLE_COLUMN_PORTS                                          // {50}
    if (y > (iStartPortY + (((_PORTS_AVAILABLE + 1)/2) * PORT_LINE_SPACE))) {
        iLastPort = -1;
        return 0;
    }
#else
    if (y > iStartPortY + ((_PORTS_AVAILABLE + _EXTERNAL_PORT_COUNT) * PORT_LINE_SPACE)) { // {81}
        iLastPort = -1;
        return 0;
    }
#endif
    if (iCheck == POSSIBLE_PORT) {
        return 1;
    }

    y -= (iStartPortY);

    iSizeX = Port_tm.tmAveCharWidth;

    while (iPortBit < PORT_WIDTH) {                                      // which input ?
        if (x < iSizeX) {                                                // port row found
#if defined DOUBLE_COLUMN_PORTS
            iPort = iPortColumn;
#else
            iPort = 0;
#endif
            iSizeY = PORT_LINE_SPACE;
            while (iPort < (_PORTS_AVAILABLE + _EXTERNAL_PORT_COUNT)) {  // {81}
                if (y < iSizeY) {                                        // port bit column found too
#if defined _M5223X && (defined _M5222X || defined _M5221X || defined _M521XX) // compensate for 2 dummy ports
                    if (iPort >= (_PORT_GP - 2)) {
                        iPort += 3;
                    }
                    else if (iPort >= (_PORT_TD - 1)) {
                        iPort += 2;
                    }
                    else if (iPort >= _PORT_LD) {
                        iPort += 1;
                    }
#elif defined _M5223X && defined _M521X
                    if (iPort >= _PORT_LD) {
                        iPort += 1;
                        if (iPort >= _PORT_GP) {
                            iPort += 1;
                        }
                    }
#endif
                    iLastPort = iPort;
                    iLastBit  = iPortBit;
                    if (PORT_LOCATION == iCheck) {
                        return 0;
                    }
                    fnSimPortInputToggle(iPort, iPortBit);
                    return INPUT_CHANGED;
                }
                iPort++;
                iSizeY += PORT_LINE_SPACE;
            }
        }
        iPortBit++;
        iSizeX += Port_tm.tmAveCharWidth;
    }
    return 0;
}

#if defined ETH_INTERFACE
static void fnDisplayLAN_LEDs(HDC hdc, RECT refresh_rect)
{
    HBRUSH hBrush;

    if ((refresh_rect.right < rect_LAN_LED.left) || (refresh_rect.bottom < rect_LAN_LED.top)) {
        return;
    }
    SelectObject(hdc, hPen);                                             // select the pen style

    // Draw a box with rx and tx LEDs. If active red, or else white
    // RoundRect(hdc, rect_LAN_LED.left, rect_LAN_LED.top, rect_LAN_LED.right, rect_LAN_LED.bottom, 10, 10);
    //
    if (iLastRxActivity) {
        hBrush = hGreenBrush;                                            // {4}
    }
    else {
        hBrush = (HBRUSH)GetStockObject(NULL_BRUSH);
    }
    SelectObject(hdc, hBrush);                                           // select the brush style for the first LED
    #if defined USER_CHIP_PACKAGE                                        // {95}
    Rectangle(hdc, rect_LAN_LED.left, (rect_LAN_LED.top + (18 * (IP_NETWORK_COUNT - 1))), (rect_LAN_LED.left + ETH_LED_WIDTH), (rect_LAN_LED.bottom +  + (18 * (IP_NETWORK_COUNT - 1))));
    #else
    Rectangle(hdc, (rect_LAN_LED.left + 2), (rect_LAN_LED.top + 27 + (18 * (IP_NETWORK_COUNT - 1))), (rect_LAN_LED.left + 2 + 10), (rect_LAN_LED.top + 27 + 8 + (18 * (IP_NETWORK_COUNT - 1))));
    #endif
    if (iLastTxActivity) {
        hBrush = hRedBrush;                                              // {4}
    }
    else {
        hBrush = (HBRUSH)GetStockObject(NULL_BRUSH);
    }
    SelectObject(hdc, hBrush);                                           // select the brush style for the second LED
    #if defined USER_CHIP_PACKAGE                                        // {95}
    Rectangle(hdc, (rect_LAN_LED.right - ETH_LED_WIDTH), rect_LAN_LED.top, rect_LAN_LED.right, (rect_LAN_LED.bottom));
    #else
        #if defined _KINETIS
    Rectangle(hdc, (rect_LAN_LED.right - 10), (rect_LAN_LED.top + 27 + (18 * (IP_NETWORK_COUNT - 1))), (rect_LAN_LED.right), (rect_LAN_LED.top + 27 + 8 + (18 * (IP_NETWORK_COUNT - 1))));
        #else
    Rectangle(hdc, (rect_LAN_LED.left + 34), (rect_LAN_LED.top + 27 + (18 * (IP_NETWORK_COUNT - 1))), (rect_LAN_LED.left + 34 + 10), (rect_LAN_LED.top + 27 + 8 + (18 * (IP_NETWORK_COUNT - 1))));
        #endif
    #endif
}
#endif


#if defined USB_INTERFACE                                                // {14}
static int iUSBEnumerated = 0;
static int iUSB_state_changed = 0;

#if defined USER_CHIP_PACKAGE                                            // {95}
    #undef USB_LEFT
    #undef USB_RIGHT
    #undef USB_TOP
    #undef USB_BOTTOM
    #define USB_LEFT   USB_SYMBOL_LEFT
    #define USB_TOP    USB_SYMBOL_TOP
    #define USB_RIGHT  USB_SYMBOL_RIGHT
    #define USB_BOTTOM USB_SYMBOL_BOTTOM
    #define USB_CIRCLE_RADIUS   4
#else
    #define USB_CIRCLE_RADIUS   4
#endif
#define _USB_TOP    (USB_TOP + (18 * (IP_NETWORK_COUNT - 1)))
#define _USB_BOTTOM (USB_BOTTOM + (18 * (IP_NETWORK_COUNT - 1)))
const POINT lUSB[] = {
    {(USB_LEFT + USB_CIRCLE_RADIUS), (_USB_TOP + (_USB_BOTTOM - _USB_TOP)/2)},
    {USB_RIGHT, (_USB_TOP + (_USB_BOTTOM - _USB_TOP)/2)},
    {(USB_RIGHT - (2 * USB_CIRCLE_RADIUS)), ((_USB_TOP + (_USB_BOTTOM - _USB_TOP)/2) - USB_CIRCLE_RADIUS/2)},
    {(USB_RIGHT - (2 * USB_CIRCLE_RADIUS)), ((_USB_TOP + (_USB_BOTTOM - _USB_TOP)/2) + USB_CIRCLE_RADIUS/2)},
    {USB_RIGHT, (_USB_TOP + (_USB_BOTTOM - _USB_TOP)/2)},
};
const POINT lUSB2[] = {
    {(USB_LEFT + (4 * (USB_RIGHT - USB_LEFT)/5)), (_USB_BOTTOM - USB_CIRCLE_RADIUS/2)},
    {(USB_LEFT + (3 * ((USB_RIGHT - USB_LEFT)/5))), (_USB_BOTTOM - USB_CIRCLE_RADIUS/2)},
    {(USB_LEFT + ((USB_RIGHT - USB_LEFT)/2)), (_USB_TOP + (_USB_BOTTOM - _USB_TOP)/2)},
    {(USB_LEFT + ((USB_RIGHT - USB_LEFT)/4)), (_USB_TOP + (_USB_BOTTOM - _USB_TOP)/2)},
    {(USB_LEFT + (2 * ((USB_RIGHT - USB_LEFT)/5))), (_USB_TOP + USB_CIRCLE_RADIUS/2)},
    {(USB_LEFT + (3 * ((USB_RIGHT - USB_LEFT)/5))), (_USB_TOP + USB_CIRCLE_RADIUS/2)},
};
const POINT lCheck[] = {
    {USB_LEFT, _USB_TOP},
    {USB_RIGHT,_USB_TOP},
    {USB_RIGHT,_USB_BOTTOM},
    {USB_LEFT, _USB_BOTTOM},
    {USB_LEFT, _USB_TOP},
};

static const RECT rect_USB_sign = {USB_LEFT, _USB_TOP, USB_RIGHT, _USB_BOTTOM};

extern "C" void fnChangeUSBState(int iNewState)
{
    iUSBEnumerated = iNewState;
    iUSB_state_changed = 1;                                              // ensure sign is refreshed
    InvalidateRect(ghWnd, &rect_USB_sign, FALSE);
}



static void fnDisplayUSB(HDC hdc, RECT refresh_rect)                     // {14}
{
    if ((refresh_rect.right < rect_USB_sign.left) || (refresh_rect.bottom < rect_USB_sign.top)) {
        if (!iUSB_state_changed) {
            return;
        }
    }
    iUSB_state_changed = 0;
    if (iUSBEnumerated) {                                                // draw a USB sign
        SelectObject(hdc, hRedPen);
        SelectObject(hdc, hRedBrush);
    }
    else {
        SelectObject(hdc, hGrayPen);
        SelectObject(hdc, hGrayBrush);
    }
    RoundRect(hdc, USB_LEFT, ((_USB_TOP + ((_USB_BOTTOM - _USB_TOP)/2)) - USB_CIRCLE_RADIUS), (USB_LEFT + (2 * USB_CIRCLE_RADIUS)), ((_USB_TOP + ((_USB_BOTTOM - _USB_TOP)/2)) + USB_CIRCLE_RADIUS), (2 * USB_CIRCLE_RADIUS), (2 * USB_CIRCLE_RADIUS));
    RoundRect(hdc, (USB_LEFT + (3 * ((USB_RIGHT - USB_LEFT)/5))), _USB_TOP, (USB_LEFT + USB_CIRCLE_RADIUS + (3 * ((USB_RIGHT - USB_LEFT)/5))), (_USB_TOP + USB_CIRCLE_RADIUS), (USB_CIRCLE_RADIUS), (USB_CIRCLE_RADIUS));
    Rectangle(hdc, (USB_LEFT + (4 * (USB_RIGHT - USB_LEFT)/5)), _USB_BOTTOM, ((USB_LEFT + (4 * (USB_RIGHT - USB_LEFT)/5)) + USB_CIRCLE_RADIUS), (_USB_BOTTOM - USB_CIRCLE_RADIUS));

    Polyline(hdc, lUSB, (sizeof(lUSB)/sizeof(POINT)));
    Polyline(hdc, lUSB2, (sizeof(lUSB2)/sizeof(POINT)));
}
#endif

#if defined SDCARD_SUPPORT && !defined NAND_FLASH_FAT                    // {77}

#define SDCARD_EDGE     2
#define SDCARD_CORNER   10
#define SDCARD_LEFT     30
#define SDCARD_RIGHT    (SDCARD_LEFT + 50)
#define SDCARD_TOP      70
#define SDCARD_BOTTOM   (SDCARD_TOP + 65)

const POINT lSDcard[] = {
    {(SDCARD_LEFT + SDCARD_CORNER), (SDCARD_TOP)},
    {(SDCARD_RIGHT), (SDCARD_TOP)},
    {(SDCARD_RIGHT), (SDCARD_BOTTOM)},
    {(SDCARD_LEFT), (SDCARD_BOTTOM)},
    {(SDCARD_LEFT), (SDCARD_TOP + SDCARD_CORNER)},
    {(SDCARD_LEFT + SDCARD_CORNER), (SDCARD_TOP)},
};

const POINT lSDcard2[] = {
    {(SDCARD_LEFT + SDCARD_CORNER + SDCARD_EDGE), (SDCARD_TOP + SDCARD_EDGE)},
    {(SDCARD_RIGHT - SDCARD_EDGE), (SDCARD_TOP + SDCARD_EDGE)},
    {(SDCARD_RIGHT - SDCARD_EDGE), (SDCARD_BOTTOM - SDCARD_EDGE)},
    {(SDCARD_LEFT + SDCARD_EDGE), (SDCARD_BOTTOM - SDCARD_EDGE)},
    {(SDCARD_LEFT + SDCARD_EDGE), (SDCARD_TOP + SDCARD_CORNER + SDCARD_EDGE)},
    {(SDCARD_LEFT + SDCARD_CORNER + SDCARD_EDGE), (SDCARD_TOP + SDCARD_EDGE)},
};

static const RECT rect_SDcard = {SDCARD_LEFT, SDCARD_TOP, SDCARD_RIGHT, SDCARD_BOTTOM};

static int iSD_state_changed = 1;                                        // enable initial redraw
#if defined _NO_SD_CARD_INSERTED
    static int iSD_state = SDCARD_REMOVED;                               // card is initially not inserted
#else
    static int iSD_state = SDCARD_INSERTED;                              // card is initially inserted
#endif

extern "C" int SD_card_state(int iSetState, int iClearState)
{
    int iInitialState = iSD_state;
    iSD_state |= iSetState;
    iSD_state &= ~(iClearState);
    if (iSD_state != iInitialState) {
        iSD_state_changed = 1;                                           // single-shot redraw
        InvalidateRect(ghWnd, &rect_SDcard, FALSE);
    }
    return iSD_state;                                                    // return the present SD card state
}

static void fnDisplaySDCARD(HDC hdc, RECT refresh_rect)
{
    if ((refresh_rect.right < rect_SDcard.left) || (refresh_rect.bottom < rect_SDcard.top)) {
        if (iSD_state_changed == 0) {
            return;
        }
    }
    iSD_state_changed = 0;
    if ((iSD_state & SDCARD_INSERTED) == 0) {                            // draw an empty SD card slot
        SelectObject(hdc, GetStockObject(WHITE_BRUSH));
        SelectObject(hdc, GetStockObject(WHITE_PEN));
        RoundRect(hdc, rect_SDcard.left, rect_SDcard.top, rect_SDcard.right, rect_SDcard.bottom, 0, 0);
        SelectObject(hdc, hGrayPen);
    }
    else {
        RECT rect_text;
        memcpy(&rect_text, &rect_SDcard, sizeof(rect_SDcard));
        if (iSD_state &  SDCARD_MOUNTED) {                               // card has been recognised
            SelectObject(hdc, hGreenPen);
            SelectObject(hdc, hGreenBrush);
            rect_text.top += 28;
            rect_text.left += 5;
            if (iSD_state & SDCARD_FORMATTED_32) {
                DrawText(hdc, "FAT32", 6, &rect_text, 0);
            }
            else if (iSD_state & SDCARD_FORMATTED_16) {
                DrawText(hdc, "FAT16", 6, &rect_text, 0);
            }
        }
        else {
            SelectObject(hdc, hRedPen);
            SelectObject(hdc, hRedBrush);
        }
        memcpy(&rect_text, &rect_SDcard, sizeof(rect_SDcard));
        rect_text.top += 10;
        rect_text.left += 18;
    #if SDCARD_SIM_SIZE == SDCARD_SIZE_1G
        DrawText(hdc, "1G", 2, &rect_text, 0);
    #elif SDCARD_SIM_SIZE == SDCARD_SIZE_2G
        DrawText(hdc, "2G", 2, &rect_text, 0);
    #elif SDCARD_SIM_SIZE == SDCARD_SIZE_4G
        DrawText(hdc, "4G", 2, &rect_text, 0);
    #elif SDCARD_SIM_SIZE == SDCARD_SIZE_8G
        DrawText(hdc, "8G", 2, &rect_text, 0);
    #else
        DrawText(hdc, "16G", 3, &rect_text, 0);
    #endif
        if (iSD_state & SDCARD_WR_PROTECTED) {
            rect_text.top += 35;
            DrawText(hdc, "WP", 2, &rect_text, 0);
        }
    }
    Polyline(hdc, lSDcard, (sizeof(lSDcard)/sizeof(POINT)));             // SD card outline
    Polyline(hdc, lSDcard2, (sizeof(lSDcard2)/sizeof(POINT)));           // SD card edge
}


// Mouse key released - check whether the SD card should be inserted/removed
//
static void fnCheckSDCard(int x, int y)
{
    // Check whether the mouse was on the SD card symbol
    //
    if (x < rect_SDcard.left) {
        return;
    }
    if (y < rect_SDcard.top) {
        return;
    }
    if (x > rect_SDcard.right) {
        return;
    }
    if (y > rect_SDcard.bottom) {
        return;
    }
    iSD_state ^= SDCARD_INSERTED;                                        // remove / insert
    iSD_state_changed = 1;                                               // single-shot redraw
    InvalidateRect(ghWnd, &rect_SDcard, FALSE);
}
#endif

static char szIPDetails[IP_NETWORK_COUNT][100];                          // {99}
static char szProjectName[100] = "PROJECT";
static void fnDoDraw(HWND hWnd, HDC hdc, PAINTSTRUCT ps, RECT &rect)
{
    TCHAR szHello[MAX_LOADSTRING];
    static int nInit = 0;
    static BITMAPFILEHEADER *pbmfh = 0;
    static BITMAPINFO *pbmi = 0;
    static BYTE *pBits;
    static int cxDib, cyDib;
    int posx = 0;
    int iStrLen = 0;
    int iWindowsWidth;
    int i;
    
    RECT rt;
    HGDIOBJ hFont = GetStockObject(SYSTEM_FIXED_FONT);
    GetClientRect(hWnd, &rt);
    present_windows_rect = rt;                                           // present window size
    LoadString(hInst, IDS_HELLO, szHello, MAX_LOADSTRING);
    iStrLen = strlen(szHello);
    iWindowsWidth = ((UTASKER_WIN_WIDTH - rt.left)/2);
    rt.left += 30;
    DrawText(hdc, szHello, iStrLen, &rt, 0);
    for (i = 0; i < IP_NETWORK_COUNT; i++) {                             // {99}
        rt.top += 18;
        iStrLen = strlen(szIPDetails[i]);
        DrawText(hdc, szIPDetails[i], iStrLen, &rt, 0);
    }

    rt.top += 18;
    rt.bottom += 36;
    SetTextColor(hdc, RGB(255, 0, 0));
    SelectObject(hdc, hFont);
    iStrLen = strlen(szProjectName);
    DrawText(hdc, szProjectName, iStrLen, &rt, 0);
    rt.left = 0;

    if (nInit == 0) {
        HANDLE hFile;
        DWORD dwFileSize, dwHighSize, dwBytesRead;                       // load  a picture of the device
#if defined USER_CHIP_PACKAGE                                            // {95}
    #if defined ETH_INTERFACE
        rect_LAN_LED.left = ETH_LED_FRAME_LEFT;
        rect_LAN_LED.right = ETH_LED_FRAME_RIGHT;
        rect_LAN_LED.top = ETH_LED_FRAME_TOP;
        rect_LAN_LED.bottom = ETH_LED_FRAME_BOTTOM;
    #endif
        hFile = CreateFile(USER_CHIP_PACKAGE, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, NULL);
#else
        hFile = CreateFile(CHIP_PACKAGE, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, NULL);
#endif
        if (hFile != INVALID_HANDLE_VALUE) {
            dwFileSize = GetFileSize(hFile, &dwHighSize);
            if (pbmfh = (BITMAPFILEHEADER *)malloc(dwFileSize)) {
                ReadFile(hFile, pbmfh, dwFileSize, &dwBytesRead, NULL);
            }
            CloseHandle (hFile);
            pbmi = (BITMAPINFO *)(pbmfh + 1);
            pBits = (BYTE *)pbmfh + pbmfh->bfOffBits;
            if (pbmi->bmiHeader.biSize == sizeof(BITMAPCOREHEADER)) {
                cxDib = ((BITMAPCOREHEADER *)pbmi)->bcWidth;
                cyDib = ((BITMAPCOREHEADER *)pbmi)->bcHeight;
            }
            else {
                cxDib = pbmi->bmiHeader.biWidth;
                cyDib = abs(pbmi->bmiHeader.biHeight);
            }
        }

        nInit = 1;
    }
    if (pbmfh != 0) {
        posx = iWindowsWidth - cxDib/2;
        SetDIBitsToDevice(hdc, posx, (42 + (IP_NETWORK_COUNT * 18)), cxDib, cyDib, 0,0,0, cyDib, pBits, pbmi, DIB_RGB_COLORS);
    }
#if defined ETH_INTERFACE
    fnDisplayLAN_LEDs(hdc, rect);
#endif
#if defined USB_INTERFACE
    fnDisplayUSB(hdc, rect);
#endif
#if defined SDCARD_SUPPORT && !defined NAND_FLASH_FAT                    // {77}
    fnDisplaySDCARD(hdc, rect);
#endif
    fnDisplayPorts(hdc);
    rt.left += (iWindowsWidth * 2);
#if (defined SUPPORT_KEY_SCAN || defined KEYPAD || defined BUTTON_KEY_DEFINITIONS) && defined LCD_ON_KEYPAD
    DisplayKeyPad(hWnd, rt, rect);                                       // draw the keypad if needed
#endif
#if (defined SUPPORT_LCD || defined SUPPORT_GLCD || defined SUPPORT_OLED || defined SUPPORT_TFT || defined GLCD_COLOR || defined SLCD_FILE) && !(defined FT800_GLCD_MODE && defined FT800_EMULATOR) // {35}
    #if defined LCD_ON_KEYPAD
    DisplayLCD(hWnd, rect);                                              // re-draw the LCD if needed
    #else
    rt.top = DisplayLCD(hWnd, rect);                                     // re-draw the LCD if needed {5}
    #endif
    #if defined SUPPORT_KEY_SCAN || defined KEYPAD || defined BUTTON_KEY_DEFINITIONS // {83}
  //rt.top += 15;                                                        {86}
    #endif
#endif
#if defined SLCD_FILE                                                    // {103}
    if ((rect.top == 0) && (rect.left == 0)) {
        LCDinit(1, 0);                                                   // redraw SLCD after the window has been resized
    }
#endif
#if (defined SUPPORT_KEY_SCAN || defined KEYPAD || defined BUTTON_KEY_DEFINITIONS) && !defined LCD_ON_KEYPAD // {83}
    rt.top += 16;                                                        // {86} add space between the display and keypad/panel
    DisplayKeyPad(hWnd, rt, rect);
#endif
#if defined _WITH_STATUS_BAR                                             // {100}
    #if (defined SUPPORT_KEY_SCAN || defined KEYPAD || defined BUTTON_KEY_DEFINITIONS)
    keypad_x = rt.left;
    keypad_y = rt.top;
    #endif
    SendMessage((HWND)hStatus, (UINT)SB_SETTEXT, (WPARAM)(INT) 1 | 0, (LPARAM) (LPSTR)cOperatingDetails);
#endif
}

#if defined KEYPAD_LED_DEFINITIONS                                       // {43}{90}

typedef struct stKEYPAD_LED
{
    COLORREF _led_0_colour;                                              // RGB colour of LED when port drives '0'
    COLORREF _led_1_colour;                                              // RGB colour of LED when port drives '1'
    int      _iInputState;                                               // {97} state that is valid when the port is configured as an input rather than an output
    RECT     _led_pos_size;                                              // LED rectangle referenced to the keypad
    unsigned long _led_port;                                             // port controlling the LED
    unsigned long _led_port_bit;                                         // port line controlling the LED
} KEYPAD_LED;

static const KEYPAD_LED _keypad_leds[] = {
    KEYPAD_LED_DEFINITIONS                                               // project specific LED definitions
};

#if defined MULTICOLOUR_LEDS
typedef struct stMULTI_COLOUR_LED
{
    int iLED_start;                                                      // the first referenced LED in a multicoloured LED
    int iLED_end;                                                        // the final one in a multicoloured LED
} MULTI_COLOUR_LED;

static const MULTI_COLOUR_LED _multiLEDs[] = {
    MULTICOLOUR_LEDS
};
#endif

#define _KEYPAD_LEDS   (sizeof(_keypad_leds)/sizeof(KEYPAD_LED))         // {90}

typedef struct stKEYPAD_LED_SIM
{
    HBRUSH led_0_colour;                                                 // brush colour of LED when port drives '0'
    HBRUSH led_1_colour;                                                 // brush colour of LED when port drives '1'
    int    iInputState;                                                  // {97} state that is valid when the port is configured as an input rather than an output
    RECT   led_pos_size;                                                 // LED rectangle referenced to the keypad
    unsigned long led_port;                                              // port controlling the LED
    unsigned long led_port_bit;                                          // port line controlling the LED
    int    iStyle;                                                       // rectangle or circle
} KEYPAD_LED_SIM;

static KEYPAD_LED_SIM keypad_leds[_KEYPAD_LEDS] = {0};

extern void fnConfigureKeypad_leds(RECT kb_rect)
{
    static int iInit = 0;
    if (iInit == 0) {
        int led = 0;
        while (led < _KEYPAD_LEDS) {
            keypad_leds[led].led_0_colour = (HBRUSH)CreateSolidBrush(_keypad_leds[led]._led_0_colour);
            keypad_leds[led].led_1_colour = (HBRUSH)CreateSolidBrush(_keypad_leds[led]._led_1_colour);
            keypad_leds[led].iInputState = _keypad_leds[led]._iInputState; // {97}
            if (_keypad_leds[led]._led_pos_size.right == 0) {            // {79}
                keypad_leds[led].led_pos_size.bottom = kb_rect.top + (_keypad_leds[led]._led_pos_size.top + _keypad_leds[led]._led_pos_size.bottom);
                keypad_leds[led].led_pos_size.top = kb_rect.top + (_keypad_leds[led]._led_pos_size.top - _keypad_leds[led]._led_pos_size.bottom);
                keypad_leds[led].led_pos_size.left = kb_rect.left + (_keypad_leds[led]._led_pos_size.left - _keypad_leds[led]._led_pos_size.bottom);
                keypad_leds[led].led_pos_size.right = kb_rect.left + (_keypad_leds[led]._led_pos_size.left + _keypad_leds[led]._led_pos_size.bottom);
                keypad_leds[led].led_port = _keypad_leds[led]._led_port;
                keypad_leds[led].led_port_bit = _keypad_leds[led]._led_port_bit;
                keypad_leds[led].iStyle = 1;                             // circle
            }
            else {
                keypad_leds[led].led_pos_size.bottom = _keypad_leds[led]._led_pos_size.bottom + kb_rect.top;
                keypad_leds[led].led_pos_size.top = _keypad_leds[led]._led_pos_size.top + kb_rect.top;
                keypad_leds[led].led_pos_size.left = _keypad_leds[led]._led_pos_size.left + kb_rect.left;
                keypad_leds[led].led_pos_size.right = _keypad_leds[led]._led_pos_size.right + kb_rect.left;
                keypad_leds[led].led_port = _keypad_leds[led]._led_port;
                keypad_leds[led].led_port_bit = _keypad_leds[led]._led_port_bit;
                keypad_leds[led].iStyle = 0;                             // rectangle
            }
            led++;
        }
        iInit = 1;                                                       // initialisation only performed once
    }
}

#if defined BUZZER_SOUND
#include <mmsystem.h>
static HMIDIOUT hMidiOut;

static DWORD MidiOutMessage(HMIDIOUT MidiOut, int iStatus, int iChannel, int iData1, int iData2)
{
    DWORD dwMessage = (iStatus | iChannel | (iData1 << 8) | (iData2 << 16));
    return (midiOutShortMsg(MidiOut, dwMessage));
}

// This routine presently sounds a pre-defined tone and doesn't respect the frequency {80}
//
extern "C" void fnSound(int iFrequency)
{
    if (iFrequency == 0) {
        MidiOutMessage(hMidiOut, 0x80, 0, 46, 127);                      // note off
    }
    else {
        MidiOutMessage(hMidiOut, 0x90, 0, 46, 127);                      // note on
    }
}
#endif


extern void fnDisplayKeypadLEDs(HDC hdc)
{
#if defined MULTICOLOUR_LEDS
    int _led_multi_colour[3] = {0,0,0};
    int iMultiLEDs = 0;
    int iCollectColour = 0;
    int iMultiStop = 0;
#endif
    unsigned long _ulPortFunction;
    unsigned long _ulPortStates;
    int led = 0;
    while (led < _KEYPAD_LEDS) {                                         // for each LED in the list
#if defined MULTICOLOUR_LEDS
        if (_multiLEDs[iMultiLEDs].iLED_start == led) {                  // this LED is a mixture LED so we start collecting colours and only draw the final LED
            _led_multi_colour[0] = 0;
            _led_multi_colour[1] = 0;
            _led_multi_colour[2] = 0;
            iCollectColour = 1;                                          // we are collecting the colour
        }
        else if (iCollectColour == 0) {
            _led_multi_colour[0] = 0;
            _led_multi_colour[1] = 0;
            _led_multi_colour[2] = 0;
        }
        else {
            if (_multiLEDs[iMultiLEDs].iLED_end == led) {
                iMultiStop = 1;
            }
        }
        _ulPortFunction = ulPortFunction[keypad_leds[led].led_port];     // port pins defined as outputs
        _ulPortStates = ulPortStates[keypad_leds[led].led_port];         // port pin state (high or low)
        if (iCollectColour != 0) {
            if ((_ulPortFunction & keypad_leds[led].led_port_bit)) {     // port pin is an output
                if (_ulPortStates & keypad_leds[led].led_port_bit) {     // port driving a '1'
                    _led_multi_colour[0] += GetRValue(_keypad_leds[led]._led_1_colour);
                    _led_multi_colour[1] += GetGValue(_keypad_leds[led]._led_1_colour);
                    _led_multi_colour[2] += GetBValue(_keypad_leds[led]._led_1_colour);
                }
                else {                                                       // port driving a '0'
                    _led_multi_colour[0] += GetRValue(_keypad_leds[led]._led_0_colour);
                    _led_multi_colour[1] += GetGValue(_keypad_leds[led]._led_0_colour);
                    _led_multi_colour[2] += GetBValue(_keypad_leds[led]._led_0_colour);
                }
            }
            else {                                                       // port pin is not an output (assume input)
                if (keypad_leds[led].iInputState != 0) {                 // chose LED brush style depending on setting
                    _led_multi_colour[0] += GetRValue(_keypad_leds[led]._led_1_colour);
                    _led_multi_colour[1] += GetGValue(_keypad_leds[led]._led_1_colour);
                    _led_multi_colour[2] += GetBValue(_keypad_leds[led]._led_1_colour);
                }
                else {
                    _led_multi_colour[0] += GetRValue(_keypad_leds[led]._led_0_colour);
                    _led_multi_colour[1] += GetGValue(_keypad_leds[led]._led_0_colour);
                    _led_multi_colour[2] += GetBValue(_keypad_leds[led]._led_0_colour);
                }
            }
            if (iMultiStop != 0) {                                       // after collecting the draw colour calculate the mixture
                iMultiStop = 0;
                iCollectColour = 0;
                if (_led_multi_colour[0] > 255) {
                    _led_multi_colour[0] = 255;
                }
                if (_led_multi_colour[1] > 255) {
                    _led_multi_colour[1] = 255;
                }
                if (_led_multi_colour[2] > 255) {
                    _led_multi_colour[2] = 255;
                }
            }
            if (iCollectColour == 0) {
                COLORREF colour = RGB(_led_multi_colour[0], _led_multi_colour[1], _led_multi_colour[2]);
                HBRUSH brush = (HBRUSH)CreateSolidBrush(colour);         // create a brush object using the mixture colour
                SelectObject(hdc, brush);                                // select the brush style for the LED draw
                if (keypad_leds[led].iStyle != 0) {                      // draw as circle
                    Ellipse(hdc, keypad_leds[led].led_pos_size.left, keypad_leds[led].led_pos_size.top, keypad_leds[led].led_pos_size.right, keypad_leds[led].led_pos_size.bottom);
                }
                else {
                    Rectangle(hdc, keypad_leds[led].led_pos_size.left, keypad_leds[led].led_pos_size.top, keypad_leds[led].led_pos_size.right, keypad_leds[led].led_pos_size.bottom);
                }
                DeleteObject(brush);                                     // return the brush object
            }
        }
        else {                                                           // use dedicated brush styles for LEDs that have one colour
#endif
            _ulPortFunction = ulPortFunction[keypad_leds[led].led_port]; // port pins defined as outputs
            _ulPortStates = ulPortStates[keypad_leds[led].led_port];     // port pin state (high or low)
            if ((_ulPortFunction & keypad_leds[led].led_port_bit)) {     // {97} port pin is an output
                if (_ulPortStates & keypad_leds[led].led_port_bit) {     // port driving a '1'
                    SelectObject(hdc, keypad_leds[led].led_1_colour);    // select the brush style for the '1' LED
                }
                else {                                                   // port driving a '0'
                    SelectObject(hdc, keypad_leds[led].led_0_colour);    // select the brush style for the '0' LED
                }
            }
            else {                                                       // port pin is not an output (assume input)
                if (keypad_leds[led].iInputState != 0) {                 // chose LED brush style depending on setting
                    SelectObject(hdc, keypad_leds[led].led_1_colour);    // select the brush style for the '1' LED
                }
                else {
                    SelectObject(hdc, keypad_leds[led].led_0_colour);    // select the brush style for the '0' LED
                }
            }
            if (keypad_leds[led].iStyle != 0) {                          // {79} draw as circle
                Ellipse(hdc, keypad_leds[led].led_pos_size.left, keypad_leds[led].led_pos_size.top, keypad_leds[led].led_pos_size.right, keypad_leds[led].led_pos_size.bottom);
            }
            else {
                Rectangle(hdc, keypad_leds[led].led_pos_size.left, keypad_leds[led].led_pos_size.top, keypad_leds[led].led_pos_size.right, keypad_leds[led].led_pos_size.bottom);
            }
#if defined MULTICOLOUR_LEDS
        }
#endif
        led++;
    }
}

#endif

static void fnPortDisplay(unsigned long ulPortValue, unsigned long ulPortDirection, unsigned long ulPeripheral, unsigned char ucPortNumber)
{
    if ((ulPortStates[ucPortNumber] != ulPortValue) || (ulPortFunction[ucPortNumber] != ulPortDirection) || (ulPortPeripheral[ucPortNumber] != ulPeripheral)) {
        ulPortStates[ucPortNumber] = ulPortValue;
        ulPortPeripheral[ucPortNumber] = ulPeripheral;
        ulPortFunction[ucPortNumber] = ulPortDirection;
#if defined SUPPORT_LCD ||  defined SUPPORT_GLCD || defined SUPPORT_OLED || defined SUPPORT_TFT || defined GLCD_COLOR // {35}
        present_ports_rect.right = (UTASKER_WIN_WIDTH - 68);             // don't cause LCD update (when LCD is to the right of the ports)
#endif
        InvalidateRect(ghWnd, &present_ports_rect, FALSE);
        UpdateWindow(ghWnd);
    }
}

unsigned long fnGetValue(unsigned char *ptr, int iLen)
{
    unsigned long ulValue = 0;

    while (iLen--) {
        ulValue <<= 8;
        ulValue |= *ptr++;
    }

    return ulValue;
}


// Get the NIC to be used from the NIC.ini file
//
static int fnGetUserNIC(void)
{
    int iFileIni;
    int iNIC = -1;
#if _VC80_UPGRADE<0x0600
    iFileIni = _open("NIC.ini", (_O_BINARY | _O_RDWR));
#else
    _sopen_s(&iFileIni, "NIC.ini", (_O_BINARY | _O_RDWR), _SH_DENYWR, _S_IREAD);
#endif
    if (iFileIni >= 0) {
        signed char cNIC;
        if (_read(iFileIni, &cNIC, 1) > 0) {
            if (cNIC != 0) {
                cNIC -= '0';
                iNIC = cNIC;
            }
        }
        _close(iFileIni);        
    }
    return iNIC;
}

#include <sys/stat.h>

#if defined ETH_INTERFACE
// Save preferred NIC
//
static void fnSaveUserSettings(void)
{
    int iFileIni;

#if _VC80_UPGRADE < 0x0600
    iFileIni = _open("NIC.ini", (_O_BINARY | _O_TRUNC  | _O_CREAT | _O_RDWR ), _S_IREAD | _S_IWRITE );
#else
    _sopen_s(&iFileIni, "NIC.ini", (_O_BINARY | _O_TRUNC  | _O_CREAT | _O_RDWR ), _SH_DENYWR, _S_IREAD | _S_IWRITE );
#endif

    if (iFileIni >= 0) {
        signed char cNIC = (signed short)iUserNIC + '0';
        if (iUserNIC < 0) {
            cNIC = 0;
        }
        _write(iFileIni, &cNIC, 1);
        _close(iFileIni);        
    }
}
#endif


// Load user file paths or set defaults if it doesn't exist
//
static void fnLoadUserFiles()                                            // {19}
{
    int iFileIni;
    CHAR *filePath;
#if _VC80_UPGRADE < 0x0600
    iFileIni = _open("userfiles.ini", (_O_BINARY | _O_RDWR));
#else
    _sopen_s(&iFileIni, "userfiles.ini", (_O_BINARY | _O_RDWR), _SH_DENYWR, _S_IREAD);
#endif
    if (iFileIni >= 0) {                                                 // if file exists
        filePath = szEthernetFileName;
        while (_read(iFileIni, filePath, 1) > 0) {
            if (*filePath++ == 0) {
                break;
            }
        }
        filePath = szPortFileName;
        while (_read(iFileIni, filePath, 1) > 0) {
            if (*filePath++ == 0) {
                break;
            }
        }
        _close(iFileIni);        
    }
    else {
        STRCPY(szEthernetFileName, "ethernet.eth");                      // default - for compatibility {24}
        STRCPY(szPortFileName,     "port.sim");                          // default - for compatibility {24}
    }
}

static void fnSaveUserFiles()
{
    int iFileIni;

#if _VC80_UPGRADE<0x0600
    iFileIni = _open("userfiles.ini", (_O_BINARY | _O_TRUNC  | _O_CREAT | _O_RDWR ), _S_IREAD | _S_IWRITE );
#else
    _sopen_s(&iFileIni, "userfiles.ini", (_O_BINARY | _O_TRUNC  | _O_CREAT | _O_RDWR ), _SH_DENYWR, _S_IREAD | _S_IWRITE );
#endif

    if (iFileIni >= 0) {
        _write(iFileIni, szEthernetFileName, (strlen(szEthernetFileName) + 1));
        _write(iFileIni, szPortFileName, (strlen(szPortFileName) + 1));
        _close(iFileIni);        
    }
}

#if defined SERIAL_INTERFACE
    #if defined LPUARTS_AVAILABLE
static char cUART[UARTS_AVAILABLE + LPUARTS_AVAILABLE][BUFSSIZE] = {{0}};
    #else
static char cUART[UARTS_AVAILABLE][BUFSSIZE] = {{0}};
    #endif

static void fnInsertUART_entry(char *ptrString, char *ptrNewString)
{
    int iMatchLength = 0;
    int iNewLength = 0;
    char *ptrStringBackup = ptrString;
    char *ptrMatchStart = 0;
    char *ptrMatchEnd = (ptrNewString + 1); 
    char *ptrLocation = 0;
    while (*ptrString != 0) {                                            // search through existing string
        if (*ptrString == *ptrMatchEnd) {
            if (ptrMatchStart == 0) {
                ptrMatchStart = (ptrString - 1);
            }
            ptrMatchEnd = ptrMatchEnd++;
            iMatchLength++;
            if (iMatchLength > 6) {                                      // end of a UART string entry "{UARTx " matched
                ptrLocation = ptrMatchStart;                             // mark the start of the string
                while (*++ptrString != '}') {
                    iMatchLength++;                                      // count the origial new string length
                }
                iMatchLength += 2;
                iNewLength = strlen(ptrNewString);
                if (iMatchLength > iNewLength) {                         // new string is shorter
                    ptrMatchStart += iMatchLength;                       // shift remaining string to fill the space
                    ptrString = (ptrMatchStart - (iMatchLength - iNewLength));
                    do {
                        *ptrString++ = *ptrMatchStart++;
                    } while (*ptrMatchStart != 0);
                    *ptrString = 0;
                }
                else if (iMatchLength < iNewLength) {                    // new string is longer
                    int iShiftLength = 0;
                    ptrMatchStart += iMatchLength;                       // shift remaining string to make space
                    ptrString = (ptrMatchStart + (iNewLength - iMatchLength));
                    while (*ptrMatchStart != 0) {                        // move to the end of the string to be shfter
                        ptrMatchStart++;
                        iShiftLength++;
                    } 
                    ptrString += iShiftLength;
                    iShiftLength++;
                    while (iShiftLength--) {
                        *ptrString-- = *ptrMatchStart--;                 // reverse copy to achieve the shift
                    }
                }
                break;
            }
        }
        else if (iMatchLength != 0) {
            iMatchLength = 0;
            ptrMatchStart = 0;
            ptrMatchEnd = (ptrNewString - 1);
        }
        ptrString++;
    }
    ptrString = ptrStringBackup;
    if (ptrLocation != 0) {                                              // if the UART entry already existed
        memcpy(ptrLocation, ptrNewString, strlen(ptrNewString));         // overwrite original string entry
    }
    else {
        STRCAT(ptrString, ptrNewString);                                 // add the newly initialised COM port details
    }
}

static void fnUART_string(int iUART, int iCOM, DWORD com_port_speed, UART_MODE_CONFIG Mode) // {101}
{
    char *ptrString = cUART[iUART];
    char *par = 0;
    int bits = 8;
    if (CHAR_7 & Mode) {
        bits = 7;
    }
    else {
        bits = 8;
    }
    if (Mode & RS232_EVEN_PARITY) {
        par = "even ";
    }
    else if (Mode & RS232_ODD_PARITY) {
        par = "odd ";
    }
    else {
        par = "";
    }
    if (iCOM == 0) {                                                     // not assigned to COM port
    #if _VC80_UPGRADE >= 0x0600
        SPRINTF(ptrString, BUFSSIZE, " {UART%i %in%i %s - no COM}", iUART, com_port_speed, bits, par);
    #else
        SPRINTF(ptrString, " {UART%i %in%i %s - no COM}", iUART, com_port_speed, bits, par);
    #endif
    }
    else {
    #if _VC80_UPGRADE >= 0x0600
        SPRINTF(ptrString, BUFSSIZE, " {UART%i %in%i %son COM%i}", iUART, com_port_speed, bits, par, iCOM);
    #else
        SPRINTF(ptrString, " {UART%i %in%i %son COM%i}", iUART, com_port_speed, bits, par, iCOM);
    #endif
    }
    fnInsertUART_entry(cOperatingDetails, ptrString);                    // {102} add or overwrite the UART entry
}
#endif

#define USE_DIB

int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow )
{
    MSG msg = {0};
    HACCEL hAccelTable;
#if defined SERIAL_INTERFACE
    int iRxSize;
    unsigned char ucRxBuffer[1000];
#endif
    unsigned char ucDoList[10000];                                       // {2}
    char *doList[2];                                                     // {1}
    int iAction;

    SYSTEMTIME st;

    LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadString(hInstance, IDC_ETHERNET1, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    GetLocalTime(&st);
    doList[0] = (char *)&st;
    doList[1] = szProjectName;
    for (iAction = 0; iAction < IP_NETWORK_COUNT; iAction++) {           // {99}
        STRCPY(szIPDetails[iAction], "IP x.x.x.x : yy-yy-yy-yy-yy-yy");
    }

    main(INITIALISE_OP_SYSTEM, doList);

    STRCAT(szTitle, " - ");
    STRCAT(szTitle, szProjectName);
#if defined SOFTWARE_VERSION                                             // {106}
    STRCAT(szTitle, " / SW-Version ");
    STRCAT(szTitle, SOFTWARE_VERSION);
#endif
#if defined SUPPORT_LCD
    LCDinit(LCD_LINES, LCD_CHARACTERS);
#elif defined SUPPORT_GLCD || defined SUPPORT_OLED || defined SUPPORT_TFT || defined GLCD_COLOR || defined SLCD_FILE // {35}{65}
    #if (defined FT800_GLCD_MODE && defined FT800_EMULATOR)
    fnInitFT800_emulator();
    #else
    LCDinit(0, 0);
    #endif
#endif

    if (InitInstance(hInstance, nCmdShow) == 0) {
        return FALSE;
    }

    hAccelTable = LoadAccelerators(hInstance, (LPCTSTR)IDC_ETHERNET1);

    doList[0] = (char *)ucDoList;
    memset(ucDoList, 0x00, sizeof(ucDoList));

    fnLoadUserFiles();                                                    // {19}

    while (iQuit == 0) {                                                  // main message loop
        if(::PeekMessage(&msg, NULL, 0,0,PM_NOREMOVE)) {
            GetMessage(&msg, NULL, 0, PM_NOREMOVE);
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

#if defined SERIAL_INTERFACE
        if (sm_hComm0 != INVALID_HANDLE_VALUE) {
    #if defined SUPPORT_HW_FLOW
            static DWORD lpModemLastStat0 = 0;
            DWORD lpModemStat0;
            GetCommModemStatus(sm_hComm0, &lpModemStat0);
            if (lpModemStat0 != lpModemLastStat0) {
                char *ptr[2];
                ptr[0] = (char *)&lpModemStat0;
                ptr[1] = (char *)&lpModemLastStat0;
                _main(MODEM_COM_0, ptr);
                lpModemLastStat0 = lpModemStat0;                         // set the new CTS state for reference
            }
    #endif
            while ((iRxSize = fnCheckRx(sm_hComm0, ucRxBuffer)) != 0) {
                fnProcessRx(ucRxBuffer, (unsigned short)iRxSize, 0);     // if we have received something from the serial port (UART0), process it here
            }
        }
    #if NUMBER_SERIAL > 1
        if (sm_hComm1 != INVALID_HANDLE_VALUE) {
        #if defined SUPPORT_HW_FLOW
            static DWORD lpModemLastStat1 = 0;
            DWORD lpModemStat1;
            GetCommModemStatus(sm_hComm1, &lpModemStat1);
            if (lpModemStat1 != lpModemLastStat1) {
                char *ptr[2];
                ptr[0] = (char *)&lpModemStat1;
                ptr[1] = (char *)&lpModemLastStat1;
                _main(MODEM_COM_1, ptr);
                lpModemLastStat1 = lpModemStat1;                         // set the new CTS state for reference
            }
        #endif
            while ((iRxSize = fnCheckRx(sm_hComm1, ucRxBuffer)) != 0) {
                fnProcessRx(ucRxBuffer, (unsigned short)iRxSize, 1);     // if we have received something from the serial port (UART1), process it here
            }
        }
    #endif
    #if NUMBER_SERIAL > 2
        if (sm_hComm2 != INVALID_HANDLE_VALUE) {
        #if defined SUPPORT_HW_FLOW
            static DWORD lpModemLastStat2 = 0;
            DWORD lpModemStat2;
            GetCommModemStatus(sm_hComm2, &lpModemStat2);
            if (lpModemStat2 != lpModemLastStat2) {
                char *ptr[2];
                ptr[0] = (char *)&lpModemStat2;
                ptr[1] = (char *)&lpModemLastStat2;
                _main(MODEM_COM_2, ptr);
                lpModemLastStat2 = lpModemStat2;                         // set the new CTS state for reference
            }
        #endif
            while ((iRxSize = fnCheckRx(sm_hComm2, ucRxBuffer)) != 0) {
                fnProcessRx(ucRxBuffer, (unsigned short)iRxSize, 2);     // if we have received something from the serial port (UART2), process it here
            }
        }
    #endif
    #if NUMBER_SERIAL > 3
        if (sm_hComm3 != INVALID_HANDLE_VALUE) {                         // {9}
        #if defined SUPPORT_HW_FLOW
            static DWORD lpModemLastStat3 = 0;
            DWORD lpModemStat3;
            GetCommModemStatus(sm_hComm3, &lpModemStat3);
            if (lpModemStat3 != lpModemLastStat3) {
                char *ptr[2];
                ptr[0] = (char *)&lpModemStat3;
                ptr[1] = (char *)&lpModemLastStat3;
                _main(MODEM_COM_3, ptr);
                lpModemLastStat3 = lpModemStat3;                         // set the new CTS state for reference
            }
        #endif
            while ((iRxSize = fnCheckRx(sm_hComm3, ucRxBuffer)) != 0) {
                fnProcessRx(ucRxBuffer, (unsigned short)iRxSize, 3);     // {30} if we have received something from the serial port (UART3), process it here
            }
        }
    #endif
    #if NUMBER_SERIAL > 4
        if (sm_hComm4 != INVALID_HANDLE_VALUE) {                         // {67}
        #if defined SUPPORT_HW_FLOW
            static DWORD lpModemLastStat4 = 0;
            DWORD lpModemStat4;
            GetCommModemStatus(sm_hComm4, &lpModemStat4);
            if (lpModemStat4 != lpModemLastStat4) {
                char *ptr[2];
                ptr[0] = (char *)&lpModemStat4;
                ptr[1] = (char *)&lpModemLastStat4;
                _main(MODEM_COM_4, ptr);
                lpModemLastStat4 = lpModemStat4;                         // set the new CTS state for reference
            }
        #endif
            while ((iRxSize = fnCheckRx(sm_hComm4, ucRxBuffer)) != 0) {
                fnProcessRx(ucRxBuffer, (unsigned short)iRxSize, 4);     // if we have received something from the serial port (UART4), process it here
            }
        }
    #endif
    #if NUMBER_SERIAL > 5
        if (sm_hComm5 != INVALID_HANDLE_VALUE) {                         // {67}
        #if defined SUPPORT_HW_FLOW
            static DWORD lpModemLastStat5 = 0;
            DWORD lpModemStat5;
            GetCommModemStatus(sm_hComm5, &lpModemStat5);
            if (lpModemStat5 != lpModemLastStat5) {
                char *ptr[2];
                ptr[0] = (char *)&lpModemStat5;
                ptr[1] = (char *)&lpModemLastStat5;
                _main(MODEM_COM_5, ptr);
                lpModemLastStat5 = lpModemStat5;                         // set the new CTS state for reference
            }
        #endif
            while ((iRxSize = fnCheckRx(sm_hComm5, ucRxBuffer)) != 0) {
                fnProcessRx(ucRxBuffer, (unsigned short)iRxSize, 5);     // if we have received something from the serial port (UART5), process it here
            }
        }
    #endif
    #if NUMBER_SERIAL > 6                                                // {111}
        if (sm_hComm6 != INVALID_HANDLE_VALUE) {
        #if defined SUPPORT_HW_FLOW
            static DWORD lpModemLastStat6 = 0;
            DWORD lpModemStat6;
            GetCommModemStatus(sm_hComm6, &lpModemStat6);
            if (lpModemStat6 != lpModemLastStat6) {
                char *ptr[2];
                ptr[0] = (char *)&lpModemStat6;
                ptr[1] = (char *)&lpModemLastStat6;
                _main(MODEM_COM_6, ptr);
                lpModemLastStat6 = lpModemStat6;                         // set the new CTS state for reference
            }
        #endif
            while ((iRxSize = fnCheckRx(sm_hComm6, ucRxBuffer)) != 0) {
                fnProcessRx(ucRxBuffer, (unsigned short)iRxSize, 5);     // if we have received something from the serial port (UART6), process it here
            }
        }
    #endif
    #if NUMBER_SERIAL > 7                                                // {111}
        if (sm_hComm7 != INVALID_HANDLE_VALUE) {
        #if defined SUPPORT_HW_FLOW
            static DWORD lpModemLastStat7 = 0;
            DWORD lpModemStat7;
            GetCommModemStatus(sm_hComm7, &lpModemStat7);
            if (lpModemStat7 != lpModemLastStat7) {
                char *ptr[2];
                ptr[0] = (char *)&lpModemStat7;
                ptr[1] = (char *)&lpModemLastStat7;
                _main(MODEM_COM_7, ptr);
                lpModemLastStat7 = lpModemStat7;                         // set the new CTS state for reference
            }
        #endif
            while ((iRxSize = fnCheckRx(sm_hComm7, ucRxBuffer)) != 0) {
                fnProcessRx(ucRxBuffer, (unsigned short)iRxSize, 5);     // if we have received something from the serial port (UART7), process it here
            }
        }
    #endif
    #if NUMBER_EXTERNAL_SERIAL > 0                                       // {41}
        if (sm_hCommExt_0 != INVALID_HANDLE_VALUE) {
        #if defined SUPPORT_HW_FLOW
            static DWORD lpModemLastExtStat0 = 0;
            DWORD lpModemStat;
            GetCommModemStatus(sm_hCommExt_0, &lpModemStat);
            if (lpModemStat != lpModemLastExtStat0) {
                char *ptr[2];
                ptr[0] = (char *)&lpModemStat;
                ptr[1] = (char *)&lpModemLastExtStat0;
                _main(MODEM_EXT_COM_0, ptr);
                lpModemLastExtStat0 = lpModemStat;                       // set the new CTS state for reference
            }
        #endif
            while ((iRxSize = fnCheckRx(sm_hCommExt_0, ucRxBuffer)) != 0) {
                fnProcessRx(ucRxBuffer, (unsigned short)iRxSize, NUMBER_SERIAL); // if we have received something from the serial port (UART0), process it here
            }
        }
        if (sm_hCommExt_1 != INVALID_HANDLE_VALUE) {
        #if defined SUPPORT_HW_FLOW
            static DWORD lpModemLastExtStat1 = 0;
            DWORD lpModemStat;
            GetCommModemStatus(sm_hCommExt_1, &lpModemStat);
            if (lpModemStat != lpModemLastExtStat1) {
                char *ptr[2];
                ptr[0] = (char *)&lpModemStat;
                ptr[1] = (char *)&lpModemLastExtStat1;
                _main(MODEM_EXT_COM_1, ptr);
                lpModemLastExtStat1 = lpModemStat;                       // set the new CTS state for reference
            }
        #endif
            while ((iRxSize = fnCheckRx(sm_hCommExt_1, ucRxBuffer)) != 0) {
                fnProcessRx(ucRxBuffer, (unsigned short)iRxSize, (NUMBER_SERIAL + 1)); // if we have received something from the serial port (UART0), process it here
            }
        }
        if (sm_hCommExt_2 != INVALID_HANDLE_VALUE) {
        #if defined SUPPORT_HW_FLOW
            static DWORD lpModemLastExtStat2 = 0;
            DWORD lpModemStat;
            GetCommModemStatus(sm_hCommExt_2, &lpModemStat);
            if (lpModemStat != lpModemLastExtStat2) {
                char *ptr[2];
                ptr[0] = (char *)&lpModemStat;
                ptr[1] = (char *)&lpModemLastExtStat2;
                _main(MODEM_EXT_COM_2, ptr);
                lpModemLastExtStat2 = lpModemStat;                       // set the new CTS state for reference
            }
        #endif
            while ((iRxSize = fnCheckRx(sm_hCommExt_2, ucRxBuffer)) != 0) {
                fnProcessRx(ucRxBuffer, (unsigned short)iRxSize, (NUMBER_SERIAL + 2)); // if we have received something from the serial port (UART0), process it here
            }
        }
        if (sm_hCommExt_3 != INVALID_HANDLE_VALUE) {
        #if defined SUPPORT_HW_FLOW
            static DWORD lpModemLastExtStat3 = 0;
            DWORD lpModemStat;
            GetCommModemStatus(sm_hCommExt_3, &lpModemStat);
            if (lpModemStat != lpModemLastExtStat3) {
                char *ptr[2];
                ptr[0] = (char *)&lpModemStat;
                ptr[1] = (char *)&lpModemLastExtStat3;
                _main(MODEM_EXT_COM_3, ptr);
                lpModemLastExtStat3 = lpModemStat;                       // set the new CTS state for reference
            }
        #endif
            while ((iRxSize = fnCheckRx(sm_hCommExt_3, ucRxBuffer)) != 0) {
                fnProcessRx(ucRxBuffer, (unsigned short)iRxSize, (NUMBER_SERIAL + 3)); // if we have received something from the serial port (UART0), process it here
            }
        }
    #endif
#endif
        if (iInputChange != 0) {
            if (KEY_CHANGED & iInputChange) {
                fnProcessKeyChange();
            }
            if (INPUT_CHANGED & iInputChange) {
                fnProcessInputChange();
            }
            iInputChange = 0;
        }
#if defined ETH_INTERFACE
        if (((iRxActivity > 0) != iLastRxActivity) || ((iTxActivity > 0) != iLastTxActivity)) {
            iLastRxActivity = (iRxActivity > 0);
            iLastTxActivity = (iTxActivity > 0);
            InvalidateRect(ghWnd, &rect_LAN_LED, FALSE);                 // redraw new LAN activity state
        }
        if (iRxActivity) {
            --iRxActivity;
        }
        if (iTxActivity) {
            --iTxActivity;
        }
#endif
#if TICK_RESOLUTION >= 1000                                              // {109}
        Sleep(TICK_RESOLUTION/1000);                                     // we sleep to simulate the basic tick operation
#else
        Sleep(1);                                                        // we sleep to simulate the basic tick operation
#endif
#if !defined BOOT_LOADER
        fnDoPortSim(0, 0);                                               // if we are playing back port simulation script, do it here {8}
#endif
        if (iAction = _main(TICK_CALL, doList)) {
            unsigned char *doPtr = (unsigned char *)doList[0];           // windows has to perform some action(s) for the embedded system

            if (RESET_SIM_CARD == iAction) {
                break;
            }
            if (RESET_CARD_WATCHDOG == iAction) {
                break;
            }
#if defined MULTISTART
            else if (RESTART == iAction) {
                _main(INITIALISE_OP_SYS_2, doList);                
            }
#endif

            while (*doPtr != 0) {
                switch (*doPtr++) {
#if defined SERIAL_INTERFACE
    #if NUMBER_EXTERNAL_SERIAL > 0                                       // {41}
        #if defined SERIAL_PORT_EXT_0
                case OPEN_PC_EXT_COM0:
                    {
                        unsigned long ulSpeed = fnGetValue(doPtr + 1, sizeof(ulSpeed));
                        UART_MODE_CONFIG Mode = (unsigned short)fnGetValue(doPtr + 1 + sizeof(ulSpeed), sizeof(Mode));
                        if (sm_hCommExt_0 != INVALID_HANDLE_VALUE) {
                            CloseHandle(sm_hCommExt_0);                  // if we have an open port we want to reconfigure it - so close it
                        }
                        sm_hCommExt_0 = fnConfigureSerialInterface(SERIAL_PORT_EXT_0, ulSpeed, Mode); // try to open com since the embedded system wants to use it
                    }
                    break;
        #endif
        #if defined SERIAL_PORT_EXT_1
                case OPEN_PC_EXT_COM1:
                    {
                        unsigned long ulSpeed = fnGetValue(doPtr+1, sizeof(ulSpeed));
                        UART_MODE_CONFIG Mode = (unsigned short)fnGetValue(doPtr+1+sizeof(ulSpeed), sizeof(Mode));
                        if (sm_hCommExt_1 != INVALID_HANDLE_VALUE) {
                            CloseHandle(sm_hCommExt_1);                  // if we have an open port we want to reconfigure it - so close it
                        }
                        sm_hCommExt_1 = fnConfigureSerialInterface(SERIAL_PORT_EXT_1, ulSpeed, Mode); // try to open com since the embedded system wants to use it
                    }
                    break;
        #endif
        #if defined SERIAL_PORT_EXT_2
                case OPEN_PC_EXT_COM2:
                    {
                        unsigned long ulSpeed = fnGetValue(doPtr + 1, sizeof(ulSpeed));
                        UART_MODE_CONFIG Mode = (unsigned short)fnGetValue(doPtr + 1 + sizeof(ulSpeed), sizeof(Mode));
                        if (sm_hCommExt_2 != INVALID_HANDLE_VALUE) {
                            CloseHandle(sm_hCommExt_2);                  // if we have an open port we want to reconfigure it - so close it
                        }
                        sm_hCommExt_2 = fnConfigureSerialInterface(SERIAL_PORT_EXT_2, ulSpeed, Mode); // try to open com since the embedded system wants to use it
                    }
                    break;
        #endif
        #if defined SERIAL_PORT_EXT_3
                case OPEN_PC_EXT_COM3:
                    {
                        unsigned long ulSpeed = fnGetValue(doPtr + 1, sizeof(ulSpeed));
                        UART_MODE_CONFIG Mode = (unsigned short)fnGetValue(doPtr + 1 + sizeof(ulSpeed), sizeof(Mode));
                        if (sm_hCommExt_3 != INVALID_HANDLE_VALUE) {
                            CloseHandle(sm_hCommExt_3);                      // if we have an open port we want to reconfigure it - so close it
                        }
                        sm_hCommExt_3 = fnConfigureSerialInterface(SERIAL_PORT_EXT_3, ulSpeed, Mode); // try to open com since the embedded system wants to use it
                    }
                    break;
        #endif
    #endif
                case OPEN_PC_COM0:
                    {
                        unsigned long ulSpeed = fnGetValue(doPtr + 1, sizeof(ulSpeed));
                        UART_MODE_CONFIG Mode = (unsigned short)fnGetValue(doPtr + 1 + sizeof(ulSpeed), sizeof(Mode)); // {26}
                        if (sm_hComm0 != INVALID_HANDLE_VALUE) {
                            CloseHandle(sm_hComm0);                          // if we have an open port we want to reconfigure it - so close it
                        }
                        sm_hComm0 = fnConfigureSerialInterface(SERIAL_PORT_0, ulSpeed, Mode); // try to open com since the embedded system wants to use it
                        if (sm_hComm0 >= 0) {
                            fnUART_string(0, SERIAL_PORT_0, ulSpeed, Mode); // {101} create a UART string that can be displayed on the status bar
                        }
                        else {
                            fnUART_string(0, 0, ulSpeed, Mode);          // {101} create a UART string that can be displayed on the status bar (not assigned to COM port)
                        }
                    }
                    break;
                case OPEN_PC_COM1:
                    {
                        unsigned long ulSpeed = fnGetValue(doPtr + 1, sizeof(ulSpeed));
                        UART_MODE_CONFIG Mode = (unsigned short)fnGetValue(doPtr + 1 + sizeof(ulSpeed), sizeof(Mode)); // {26}
                        if (sm_hComm1 != INVALID_HANDLE_VALUE) {
                            CloseHandle(sm_hComm1);                          // if we have an open port we want to reconfigure it - so close it
                        }
                        sm_hComm1 = fnConfigureSerialInterface(SERIAL_PORT_1, ulSpeed, Mode); // try to open com since the embedded system wants to use it
                        if (sm_hComm1 >= 0) {
                            fnUART_string(1, SERIAL_PORT_1, ulSpeed, Mode); // {101} create a UART string that can be displayed on the status bar
                        }
                        else {
                            fnUART_string(1, 0, ulSpeed, Mode);          // {101} create a UART string that can be displayed on the status bar (not assigned to COM port)
                        }
                    }
                    break;
    #if defined SERIAL_PORT_2
                case OPEN_PC_COM2:
                    {
                        unsigned long ulSpeed = fnGetValue(doPtr + 1, sizeof(ulSpeed));
                        UART_MODE_CONFIG Mode = (unsigned short)fnGetValue(doPtr + 1 + sizeof(ulSpeed), sizeof(Mode)); // {26}
                        if (sm_hComm2 != INVALID_HANDLE_VALUE) {
                            CloseHandle(sm_hComm2);                      // if we have an open port we want to reconfigure it - so close it
                        }
                        sm_hComm2 = fnConfigureSerialInterface(SERIAL_PORT_2, ulSpeed, Mode); // try to open com since the embedded system wants to use it
                        if (sm_hComm2 >= 0) {
                            fnUART_string(2, SERIAL_PORT_2, ulSpeed, Mode); // {101} create a UART string that can be displayed on the status bar
                        }
                        else {
                            fnUART_string(2, 0, ulSpeed, Mode);          // {101} create a UART string that can be displayed on the status bar (not assigned to COM port)
                        }
                    }
                    break;
    #endif
    #if defined SERIAL_PORT_3
                case OPEN_PC_COM3:                                       // {9}
                    {
//int i;
                        unsigned long ulSpeed = fnGetValue(doPtr + 1, sizeof(ulSpeed));
                        UART_MODE_CONFIG Mode = (unsigned short)fnGetValue(doPtr + 1 + sizeof(ulSpeed), sizeof(Mode)); // {26}
                        if (sm_hComm3 != INVALID_HANDLE_VALUE) {
                            CloseHandle(sm_hComm3);                      // if we have an open port we want to reconfigure it - so close it
                        }
    /*
    ulSpeed = 57600;
    for (i = 0; i < 1000 ; i++) {
        sm_hComm3 = fnConfigureSerialInterface(SERIAL_PORT_3, ulSpeed, Mode); // try to open com since the embedded system wants to use it
        CloseHandle(sm_hComm3);
    }*/
                        sm_hComm3 = fnConfigureSerialInterface(SERIAL_PORT_3, ulSpeed, Mode); // try to open com since the embedded system wants to use it
                        if (sm_hComm3 >= 0) {
                            fnUART_string(3, SERIAL_PORT_3, ulSpeed, Mode); // {101} create a UART string that can be displayed on the status bar
                        }
                        else {
                            fnUART_string(3, 0, ulSpeed, Mode);          // {101} create a UART string that can be displayed on the status bar (not assigned to COM port)
                        }
                    }
                    break;
    #endif
    #if defined SERIAL_PORT_4
                case OPEN_PC_COM4:                                       // {67}
                    {
                        unsigned long ulSpeed = fnGetValue(doPtr + 1, sizeof(ulSpeed));
                        UART_MODE_CONFIG Mode = (unsigned short)fnGetValue(doPtr + 1 + sizeof(ulSpeed), sizeof(Mode)); // {26}
                        if (sm_hComm4 != INVALID_HANDLE_VALUE) {
                            CloseHandle(sm_hComm4);                      // if we have an open port we want to reconfigure it - so close it
                        }
                        sm_hComm4 = fnConfigureSerialInterface(SERIAL_PORT_4, ulSpeed, Mode); // try to open com since the embedded system wants to use it
                        if (sm_hComm4 >= 0) {
                            fnUART_string(4, SERIAL_PORT_4, ulSpeed, Mode); // {101} create a UART string that can be displayed on the status bar
                        }
                        else {
                            fnUART_string(4, 0, ulSpeed, Mode);          // {101} create a UART string that can be displayed on the status bar (not assigned to COM port)
                        }
                    }
                    break;
    #endif
    #if defined SERIAL_PORT_5
                case OPEN_PC_COM5:                                       // {67}
                    {
                        unsigned long ulSpeed = fnGetValue(doPtr + 1, sizeof(ulSpeed));
                        UART_MODE_CONFIG Mode = (unsigned short)fnGetValue(doPtr + 1 + sizeof(ulSpeed), sizeof(Mode)); // {26}
                        if (sm_hComm5 != INVALID_HANDLE_VALUE) {
                            CloseHandle(sm_hComm5);                      // if we have an open port we want to reconfigure it - so close it
                        }
                        sm_hComm5 = fnConfigureSerialInterface(SERIAL_PORT_5, ulSpeed, Mode); // try to open com since the embedded system wants to use it
                        if (sm_hComm5 >= 0) {
                            fnUART_string(5, SERIAL_PORT_5, ulSpeed, Mode); // {101} create a UART string that can be displayed on the status bar
                        }
                        else {
                            fnUART_string(5, 0, ulSpeed, Mode);          // {101} create a UART string that can be displayed on the status bar (not assigned to COM port)
                        }
                    }
                    break;
    #endif
    #if defined SERIAL_PORT_6
                case OPEN_PC_COM6:                                       // {111}
                    {
                        unsigned long ulSpeed = fnGetValue(doPtr + 1, sizeof(ulSpeed));
                        UART_MODE_CONFIG Mode = (unsigned short)fnGetValue(doPtr + 1 + sizeof(ulSpeed), sizeof(Mode));
                        if (sm_hComm6 != INVALID_HANDLE_VALUE) {
                            CloseHandle(sm_hComm6);                      // if we have an open port we want to reconfigure it - so close it
                        }
                        sm_hComm6 = fnConfigureSerialInterface(SERIAL_PORT_6, ulSpeed, Mode); // try to open com since the embedded system wants to use it
                        if (sm_hComm6 >= 0) {
                            fnUART_string(6, SERIAL_PORT_6, ulSpeed, Mode); //create a UART string that can be displayed on the status bar
                        }
                        else {
                            fnUART_string(6, 0, ulSpeed, Mode);          // create a UART string that can be displayed on the status bar (not assigned to COM port)
                        }
                    }
                    break;
    #endif
    #if defined SERIAL_PORT_7
                case OPEN_PC_COM7:                                       // {111}
                    {
                        unsigned long ulSpeed = fnGetValue(doPtr + 1, sizeof(ulSpeed));
                        UART_MODE_CONFIG Mode = (unsigned short)fnGetValue(doPtr + 1 + sizeof(ulSpeed), sizeof(Mode));
                        if (sm_hComm7 != INVALID_HANDLE_VALUE) {
                            CloseHandle(sm_hComm7);                      // if we have an open port we want to reconfigure it - so close it
                        }
                        sm_hComm7 = fnConfigureSerialInterface(SERIAL_PORT_7, ulSpeed, Mode); // try to open com since the embedded system wants to use it
                        if (sm_hComm7 >= 0) {
                            fnUART_string(7, SERIAL_PORT_7, ulSpeed, Mode); //create a UART string that can be displayed on the status bar
                        }
                        else {
                            fnUART_string(7, 0, ulSpeed, Mode);          // create a UART string that can be displayed on the status bar (not assigned to COM port)
                        }
                    }
                    break;
    #endif
                case MODEM_SIGNAL_CHANGE:                                // {7}
                    {
                        unsigned char ucComPort = (unsigned char)fnGetValue(doPtr + 1, 1); // port to be changed
                        unsigned char ucRTS_enable = (unsigned char)fnGetValue(doPtr + 2, 1);// state
                        DCB dcb;
                        HANDLE sm_hComm;

                        switch (ucComPort) {
                        case 0:
                            sm_hComm = sm_hComm0;
                            break;
                        case 1:
                            sm_hComm = sm_hComm1;
                            break;
                        case 2:
                            sm_hComm = sm_hComm2;
                            break;
                        case 3:
                            sm_hComm = sm_hComm3;                        // {9}
                            break;
                        }
                        if (sm_hComm != INVALID_HANDLE_VALUE) {
                            GetCommState(sm_hComm, &dcb);                // get the present com port settings
                            if (ucRTS_enable != 0) {
                                dcb.fRtsControl = 1;
                            }
                            else {
                                dcb.fRtsControl = 0;
                            }
                            SetCommState(sm_hComm, &dcb);                // set the new state
                        }
                    }
                    break;

                case SEND_PC_COM0:
                    {
                    unsigned long ulLength = fnGetValue(doPtr + 1, sizeof(ulLength)); // we send embedded system serial data UART 0 over COM
                    fnSendSerialMessage(sm_hComm0, (const void *)fnGetValue(doPtr + 1 + sizeof(ulLength), sizeof(const void *)), ulLength);
                    }
                    break;

                case SEND_PC_COM1:
                    {
                    unsigned long ulLength = fnGetValue(doPtr + 1, sizeof(ulLength)); // we send embedded system serial data UART 1 over COM
                    fnSendSerialMessage(sm_hComm1, (const void *)fnGetValue(doPtr + 1 + sizeof(ulLength), sizeof(const void *)), ulLength);
                    }
                    break;

                case SEND_PC_COM2:
                    {
                    unsigned long ulLength = fnGetValue(doPtr + 1, sizeof(ulLength)); // we send embedded system serial data UART 2 over COM
                    fnSendSerialMessage(sm_hComm2, (const void *)fnGetValue(doPtr + 1 + sizeof(ulLength), sizeof(const void *)), ulLength);
                    }
                    break;

                case SEND_PC_COM3:                                       // {9}
                    {
                    unsigned long ulLength = fnGetValue(doPtr + 1, sizeof(ulLength)); // we send embedded system serial data UART 3 over COM
                    fnSendSerialMessage(sm_hComm3, (const void *)fnGetValue(doPtr + 1 + sizeof(ulLength), sizeof(const void *)), ulLength);
                    }
                    break;

                case SEND_PC_COM4:                                       // {67}
                    {
                    unsigned long ulLength = fnGetValue(doPtr + 1, sizeof(ulLength)); // we send embedded system serial data UART 4 over COM
                    fnSendSerialMessage(sm_hComm4, (const void *)fnGetValue(doPtr + 1 + sizeof(ulLength), sizeof(const void *)), ulLength);
                    }
                    break;
                case SEND_PC_COM5:
                    {
                    unsigned long ulLength = fnGetValue(doPtr + 1, sizeof(ulLength)); // we send embedded system serial data UART 5 over COM
                    fnSendSerialMessage(sm_hComm5, (const void *)fnGetValue(doPtr + 1 + sizeof(ulLength), sizeof(const void *)), ulLength);
                    }
                    break;

                case SEND_PC_COM6:                                       // {111}
                    {
                    unsigned long ulLength = fnGetValue(doPtr + 1, sizeof(ulLength)); // we send embedded system serial data UART 6 over COM
                    fnSendSerialMessage(sm_hComm6, (const void *)fnGetValue(doPtr + 1 + sizeof(ulLength), sizeof(const void *)), ulLength);
                    }
                    break;

                case SEND_PC_COM7:
                    {
                    unsigned long ulLength = fnGetValue(doPtr + 1, sizeof(ulLength)); // we send embedded system serial data UART 7 over COM
                    fnSendSerialMessage(sm_hComm7, (const void *)fnGetValue(doPtr + 1 + sizeof(ulLength), sizeof(const void *)), ulLength);
                    }
                    break;
    #if NUMBER_EXTERNAL_SERIAL > 0                                       // {41}
                case SEND_PC_EXT_COM0:
                    {
                    unsigned long ulLength = fnGetValue(doPtr + 1, sizeof(ulLength)); // we send embedded system serial data Ext UART 0 over COM
                    fnSendSerialMessage(sm_hCommExt_0, (const void *)fnGetValue(doPtr + 1 + sizeof(ulLength), sizeof(const void *)), ulLength);
                    }
                    break;

                case SEND_PC_EXT_COM1:
                    {
                    unsigned long ulLength = fnGetValue(doPtr + 1, sizeof(ulLength)); // we send embedded system serial data Ext UART 1 over COM
                    fnSendSerialMessage(sm_hCommExt_1, (const void *)fnGetValue(doPtr + 1 + sizeof(ulLength), sizeof(const void *)), ulLength);
                    }
                    break;

                case SEND_PC_EXT_COM2:
                    {
                    unsigned long ulLength = fnGetValue(doPtr + 1, sizeof(ulLength)); // we send embedded system serial data Ext UART 2 over COM
                    fnSendSerialMessage(sm_hCommExt_2, (const void *)fnGetValue(doPtr + 1 + sizeof(ulLength), sizeof(const void *)), ulLength);
                    }
                    break;

                case SEND_PC_EXT_COM3:
                    {
                    unsigned long ulLength = fnGetValue(doPtr + 1, sizeof(ulLength)); // we send embedded system serial data Ext UART 3 over COM
                    fnSendSerialMessage(sm_hCommExt_3, (const void *)fnGetValue(doPtr + 1 + sizeof(ulLength), sizeof(const void *)), ulLength);
                    }
                    break;
    #endif

                case SET_COM_BREAK_0:
                    SetCommBreak(sm_hComm0);
                    break;

                case CLR_COM_BREAK_0:
                    ClearCommBreak(sm_hComm0);
                    break;

                case SET_COM_BREAK_1:
                    SetCommBreak(sm_hComm1);
                    break;

                case CLR_COM_BREAK_1:
                    ClearCommBreak(sm_hComm1);
                    break;

                case SET_COM_BREAK_2:
                    SetCommBreak(sm_hComm2);
                    break;

                case CLR_COM_BREAK_2:
                    ClearCommBreak(sm_hComm2);
                    break;

                case SET_COM_BREAK_3:                                    // {9}
                    SetCommBreak(sm_hComm3);
                    break;

                case CLR_COM_BREAK_3:                                    // {9}
                    ClearCommBreak(sm_hComm3);
                    break;
#endif
                case IP_CHANGE:                                          // update IP configuration
                    {
                        int i = 0;
                        unsigned char *ucInputPointer = doPtr;           // {42}
                        unsigned char ucStrLength = (unsigned char)fnGetValue(ucInputPointer++, sizeof(ucStrLength));
                        RECT ip_string;
                        char *ptrIpConfig = &szIPDetails[0][3];
                        memset(&szIPDetails[0][3], ' ', (sizeof(szIPDetails[0]) - 4)); // {68}
                        szIPDetails[0][sizeof(szIPDetails[0]) - 1] = 0;
                        while (ucStrLength--) {
                            /*if (ucStrLength == 0) {
                                fnGetValue(ucInputPointer++, 1);         // {68}
                            }
                            else {*/
                                *ptrIpConfig = (unsigned char)fnGetValue(ucInputPointer++, 1);
                                if (*ptrIpConfig == '|') {               // {99}
                                    *ptrIpConfig = ' ';
                                    if (++i >= IP_NETWORK_COUNT) {
                                        break;
                                    }
                                    ptrIpConfig = &szIPDetails[i][3];
                                    memset(&szIPDetails[i][3], ' ', (sizeof(szIPDetails[i]) - 4)); // {68}
                                    szIPDetails[i][sizeof(szIPDetails[i]) - 1] = 0;
                                }
                                else {
                                    ptrIpConfig++;
                                }
                            //}
                        }
                        ip_string.bottom = 40 + (18 * (IP_NETWORK_COUNT - 1)); // {39}
                        ip_string.left = 0;
                        ip_string.right = 450;                           // {68}
                        ip_string.top = 1;
                        InvalidateRect(ghWnd, &ip_string, FALSE);        // ensure IP text is updated
                    }
                    break;

                case DISPLAY_PORT_CHANGE:
                    {
                        unsigned char ucNumberOfPorts = (unsigned char)fnGetValue(doPtr, sizeof(ucNumberOfPorts));
                        unsigned long ulPort, ulPortDDR, ulPeripheral;
                        unsigned short usPortOffset = 1;
                        unsigned char ucPortNumber = 0;
                      //ucNumberOfPorts /= (3 * sizeof(unsigned long));  // {51}
                        while (ucNumberOfPorts--) {                      // for each port
                            ulPort = fnGetValue(doPtr + usPortOffset, sizeof(ulPort));
                            usPortOffset += sizeof(ulPort);
                            ulPortDDR = fnGetValue(doPtr + usPortOffset, sizeof(ulPort));
                            usPortOffset += sizeof(ulPortDDR);
                            ulPeripheral = fnGetValue(doPtr + usPortOffset, sizeof(ulPeripheral));
                            usPortOffset += sizeof(ulPeripheral);
                            fnPortDisplay(ulPort, ulPortDDR, ulPeripheral, ucPortNumber++);
                        }
                        doPtr += ((*doPtr * 3 * sizeof(unsigned long)) + 1); // {51}
                        continue;
                    }
                    break;

                default:
                    memset(ucDoList, 0x00, sizeof(ucDoList));
                    break;
                }
                doPtr += (*doPtr + 1);
            }
            memset(ucDoList, 0x00, sizeof(ucDoList));
        }
#if defined ETH_INTERFACE                                                // {10}
        fnDoEthereal(0, 0);                                              // {18} if we are playing back ethereal files, inject frames here
#endif
#if defined USB_INTERFACE
        fnInjectUSB(0,0,0);                                              // inject any queued USB data
#endif
    }

    if (iQuit == 0) {                                                    // was break due to reset?
        if (iAction == RESET_CARD_WATCHDOG) {
            DialogBox(hInst, (LPCTSTR)IDD_WATCHDOG, ghWnd, (DLGPROC)CardWatchdogReset);
        }
        else if (iAction == RESET_SIM_CARD) {
            DialogBox(hInst, (LPCTSTR)IDD_RESET, ghWnd, (DLGPROC)CardReset);
        }
    }

    _main(EXITING_CALL, 0);                                              // save the present FLASH file system to a file so that it can be returned on next use
#if defined ETH_INTERFACE                                                // {10}
    fnSaveUserSettings();
#endif
    fnSaveUserFiles();                                                   // {19}
    return msg.wParam;
}

static void fnProcessRx(unsigned char *ptrData, unsigned short usLength, int iPort)
{
    char *ptr[2];

    ptr[0] = (char *)&usLength;
    ptr[1] = (char *)ptrData;

    switch (iPort) {
    case 0:
        _main(RX_COM0, ptr);
        break;
#if NUMBER_SERIAL > 1
    case 1:
        _main(RX_COM1, ptr);
        break;
#endif
#if NUMBER_SERIAL > 2
    case 2:
        _main(RX_COM2, ptr);
        break;
#endif
#if NUMBER_SERIAL > 3
    case 3:                                                              // {9}
        _main(RX_COM3, ptr);
        break;
#endif
#if NUMBER_SERIAL > 4
    case 4:                                                              // {67}
        _main(RX_COM4, ptr);
        break;
#endif
#if NUMBER_SERIAL > 5
    case 5:                                                              // {67}
        _main(RX_COM5, ptr);
        break;
#endif
#if NUMBER_EXTERNAL_SERIAL > 0                                           // {41}
    case NUMBER_SERIAL:
        _main(RX_EXT_COM0, ptr);
        break;

    case (NUMBER_SERIAL + 1):
        _main(RX_EXT_COM1, ptr);
        break;

    case (NUMBER_SERIAL + 2):
        _main(RX_EXT_COM2, ptr);
        break;

    case (NUMBER_SERIAL + 3):
        _main(RX_EXT_COM3, ptr);
        break;
#endif
    }
}

// UART injection for script based simulator                             {8}
//
extern void fnInjectSerial(unsigned char *ptrInputData, unsigned short usLength, int iPortNumber)
{
    if (ptrInputData == 0) {                                             // not data injection
        char *ptr[2];
        ptr[0] = (char *)iPortNumber;
        ptr[1] = (char *)0;
        switch (usLength) {
        case UART_BREAK_CONDITION:                                       // break condition
            _main(SIM_UART_BREAK, ptr);
            break;
        case UART_CTS_ACTIVATED:                                         // CTS change - active
            ptr[1] = (char *)MS_CTS_ON;
        case UART_CTS_NEGATED:                                           // CTS change - disabled
            _main(SIM_UART_CTS, ptr);
            break;
        }
        return;
    }

    if (usLength != 0) {
        fnProcessRx(ptrInputData, usLength, iPortNumber);
    }
}

static void fnProcessRxSPI(unsigned char *ptrData, unsigned short usLength, int iPort)
{
    char *ptr[2];

    ptr[0] = (char *)&usLength;
    ptr[1] = (char *)ptrData;

    switch (iPort) {
    case 0:
        _main(RX_SPI0, ptr);
        break;

    case 1:
        _main(RX_SPI1, ptr);
        break;
    }
}


// SPI injection for script based simulator                              {8}
//
extern void fnInjectSPI(unsigned char *ptrInputData, unsigned short usLength, int iPortNumber)
{
    if (usLength != 0) {
        fnProcessRxSPI(ptrInputData, usLength, iPortNumber);
    }
}

// Inject a simulated USB OUT message                                    {15}
//
extern void fnInjectUSB(unsigned char *ptrInputData, unsigned short usLength, int iPortNumber)
{
    char *ptr[3];
    ptr[0] = (char *)iPortNumber;                                        // endpoint number
    ptr[1] = (char *)usLength;
    ptr[2] = (char *)ptrInputData;
    _main(SIM_USB_OUT, ptr);
}

#if defined I2C_INTERFACE                                                // {108}
extern void fnInjectI2C(unsigned char *ptrInputData, unsigned short usLength, int iPortNumber, int iRepeatedStart)
{
    char *ptr[3];
    ptr[0] = (char *)iPortNumber;
    ptr[1] = (char *)usLength;
    ptr[2] = (char *)ptrInputData;
    if (iRepeatedStart != 0) {
        _main(SIM_I2C_OUT_REPEATED, ptr);
    }
    else {
        _main(SIM_I2C_OUT, ptr);
    }
}
#endif

extern void fnsetKeypadState(char **ptr);

static void fnProcessKeyChange(void)
{
#if defined SUPPORT_KEY_SCAN 
    char *ptr[1];

    fnsetKeypadState(ptr);

    _main(KEY_CHANGE, ptr);
#endif
}


static int iChangedPort;
static int iChangedBit;

static void fnProcessInputChange(void)
{
    char *ptr[2];

    ptr[0] = (char *)&iChangedPort;
    ptr[1] = (char *)&iChangedBit;

    if (iShiftPressed != 0) {                                            // {11}
        _main(INPUT_TOGGLE_NEG, ptr);
    }
    else {
        _main(INPUT_TOGGLE, ptr);
    }
}

extern void fnInjectInputChange(unsigned long ulPortRef, unsigned long ulPortBit, int iState) // {83}
{
    char *ptr[2];
    int iBitRef = (PORT_WIDTH - 1);                                      // MSB
    ulPortBit >>= 1;
    while (ulPortBit != 0) {                                             // convert to the bit location reference
        iBitRef--;
        ulPortBit >>= 1;
    }

    ptr[0] = (char *)&ulPortRef;
    ptr[1] = (char *)&iBitRef;

    _main(iState, ptr);                                                  // {107}
}

static void fnSimPortInputToggle(int iPort, int iPortBit)
{
    iChangedPort = iPort;
    iChangedBit  = iPortBit;
}

extern void fnInjectPortValue(int iPort, unsigned long ulMask, unsigned long ulValue) // {8}
{
#if defined _M5223X                                                      // {25}
    unsigned long ulPortInputs = ((~ulPortFunction[iPort] | ulPortPeripheral[iPort]) & ulMask); // handle only ports which are inputs or peripherals
    unsigned long ulBit = 0x00000080;
    #if !defined _M520X && !defined _M523X                               // {53}
    switch (iPort) {
    case _PORT_TA:
    case _PORT_TC:
    #if !defined _M5225X
    case _PORT_TD:
    #endif
    case _PORT_UA:
    #if defined _M52XX_SDRAM                                             // {50}
    case _PORT_TB:
    #else
    case _PORT_AS:
    case _PORT_UB:
    case _PORT_UC:
    #endif
        ulPortInputs &= 0x0f;
        ulBit >>= 4;                                                     // 4 bit port
        break;
    }
    #endif
#elif defined _HW_SAM7X
    #if defined _HW_SAM7S || defined _HW_SAM7SE                          // {48}{57}
    unsigned long ulPortInputs = ((~ulPortFunction[iPort] | ulPortPeripheral[iPort]) & 0xffffffff); // handle only ports which are inputs or peripherals
    unsigned long ulBit = 0x80000000;
    #else
    unsigned long ulPortInputs = ((~ulPortFunction[iPort] | ulPortPeripheral[iPort]) & 0x7fffffff); // handle only ports which are inputs or peripherals
    unsigned long ulBit = 0x40000000;
    #endif
#elif defined _LM3SXXXX
    unsigned long ulPortInputs = ((~ulPortFunction[iPort] | ulPortPeripheral[iPort]) & 0xff); // handle only ports which are inputs or peripherals
    unsigned long ulBit = 0x80;
#else
    unsigned long ulPortInputs = ((~ulPortFunction[iPort] | ulPortPeripheral[iPort]) & 0xffffffff); // handle only ports which are inputs or peripherals
    unsigned long ulBit = 0x80000000;
#endif
    int iPortBit = 0;

    while (ulPortInputs) {
        if (ulBit & ulMask) {
            if ((ulPortStates[iPort] & ulBit) != (ulValue & ulBit)) {    // if this bit is to be changed
                fnSimPortInputToggle(iPort, iPortBit);
                fnProcessInputChange();
              //ulPortStates[iPort] ^= ulBit;                            // update reference value locally - {59} don't update so that it causes a refresh
            }
        }
        iPortBit++;
        ulPortInputs &= ~ulBit;
        ulBit >>= 1;
    }
}

#if defined ETH_INTERFACE                                                // {10}
extern void fnInjectFrame(unsigned char *ptrData, unsigned short usLength)
{
    char *ptr[3];

    ptr[0] = ( char *)&usLength;
    ptr[1] = ( char *)ptrData;
    ptr[2] = 0;

    if (usLength <= MAX_ETHERNET_BUFFER) {
        _main(RX_ETHERNET, ptr);                                            // send this to the ethernet input
        if (ptr[2]) {
            iRxActivity = 2;                                                // cause activity LED blinking
        }
    }
}
#endif

ATOM MyRegisterClass( HINSTANCE hInstance )
{
    WNDCLASSEX wcex;

    wcex.cbSize = sizeof(WNDCLASSEX); 

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = (WNDPROC)WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, (LPCTSTR)IDI_ETHERNET1);
    wcex.hCursor        = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = (LPCSTR)IDC_ETHERNET1;
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, (LPCTSTR)IDI_SMALL);

    return RegisterClassEx(&wcex);
}


BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    HWND hWnd;
    RECT rt = {0, 0, 0, 0};
#if defined SUPPORT_LCD || defined SUPPORT_GLCD || defined SUPPORT_OLED || defined SUPPORT_TFT || defined GLCD_COLOR || defined SLCD_FILE || defined SUPPORT_KEY_SCAN || defined KEYPAD || defined BUTTON_KEY_DEFINITIONS  // {35}{65}{83}
    int iLCD_Bottom = 0;
#endif

    rt.right = UTASKER_WIN_WIDTH;                                        // basic windows size without LCD or keypad/panel
    rt.bottom = UTASKER_WIN_HEIGHT;

#if (defined SUPPORT_LCD || defined SUPPORT_GLCD  || defined SUPPORT_OLED || defined SUPPORT_TFT || defined GLCD_COLOR || defined SLCD_FILE) && !(defined FT800_GLCD_MODE && defined FT800_EMULATOR) // {35}{65}
    #if defined _M5225X
    iLCD_Bottom = fnInitLCD(rt, UTASKER_WIN_HEIGHT, (UTASKER_WIN_WIDTH + 70));
    #else
    iLCD_Bottom = fnInitLCD(rt, UTASKER_WIN_HEIGHT, UTASKER_WIN_WIDTH);
    #endif
#endif
#if defined SUPPORT_KEY_SCAN || defined BUTTON_KEY_DEFINITIONS || defined KEYPAD // {85}
    fnInitKeyPad(rt, UTASKER_WIN_WIDTH, iLCD_Bottom);
#endif
#if _EXTERNAL_PORT_COUNT > 0                                             // {81}
    rt.bottom += (_EXTERNAL_PORT_COUNT * PORT_LINE_SPACE);               // increase the windows height to accomodate external ports
#endif
#if defined _WITH_STATUS_BAR                                             // {84}
    rt.bottom += (25);                                                   // increase the main window height to include the status bar
#endif

    hInst = hInstance;
    hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, 0, rt.right, rt.bottom, NULL, NULL, hInstance, NULL);

    if (hWnd == 0) {
        return FALSE;
    }

    ShowWindow(hWnd, nCmdShow);                                          // draw the main window frame
    ghWnd = hWnd;
    UpdateWindow(hWnd);

    #if defined SUPPORT_GLCD  || defined SUPPORT_OLED || defined SUPPORT_TFT || defined GLCD_COLOR // {104}
    clientDeviceContext = GetDC(hWnd);                                   // save device context
    {
        HBITMAP hBitmap = 0;
        LONG lBmpSize = (GLCD_X * GLCD_Y * (24/8));
        BITMAPINFO bmpInfo = { 0 };
        bmpInfo.bmiHeader.biBitCount = 24;
        bmpInfo.bmiHeader.biHeight = GLCD_Y;
        bmpInfo.bmiHeader.biWidth = GLCD_X;
        bmpInfo.bmiHeader.biPlanes = 1;
        bmpInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        hBitmap = CreateDIBSection(clientDeviceContext, (BITMAPINFO *)&bmpInfo, DIB_RGB_COLORS, (void **)&pPixels , NULL, 0); // get bit map memory for LCD display usage
        if (hBitmap != 0) {
            lcd_device_context = CreateCompatibleDC(clientDeviceContext);// create a device context for the LCD's bitmap
            SelectObject(lcd_device_context, hBitmap);                   // enter the LCD's bitmap memory in the context
        }
    }
    #endif
    return TRUE;
}

#if defined SUPPORT_LCD || defined SUPPORT_GLCD  || defined SUPPORT_OLED || defined SUPPORT_TFT || defined GLCD_COLOR // {104}
// Redraw LCD bitmap ontent
//
extern void LCD_draw_bmp(int iX, int iY, int iXsize, int iYsize)
{
    BitBlt(clientDeviceContext, iX, iY, iXsize, iYsize, lcd_device_context, 0, 0, SRCCOPY);
}
#endif

static OPENFILENAME ofn;

// Select the file to be opened
//
static BOOL fnOpenDialog(HWND hWnd, int iType)
{
    static CRITICAL_SECTION cs;
    BOOL bInitialised = 0;
    BOOL bReturn = 0;
    memset(&ofn, 0, sizeof(OPENFILENAME));
    ofn.lStructSize = sizeof(OPENFILENAME);
    ofn.hwndOwner = hWnd;
    ofn.nMaxFile  = MAX_PATH;
    ofn.nMaxFileTitle  = MAX_PATH;
    ofn.nFilterIndex = 1;
    ofn.hInstance = hInst;
    ofn.Flags = OFN_NOCHANGEDIR;

    if (bInitialised == 0) {
        InitializeCriticalSection(&cs);                                  // start of critical region
        bInitialised = 1;
    }
    EnterCriticalSection(&cs);                                           // {37} protect from task switching
    switch (iType) {
    case 0:
        ofn.lpstrInitialDir = "Ethereal";
        ofn.lpstrFile   = szEthernetFileName;
        ofn.lpstrFilter = szEthernet;
        ofn.lpstrDefExt = TEXT ("eth");
        bReturn = GetOpenFileName(&ofn);
        break;
    case 1:
        ofn.lpstrInitialDir = "Simulation Files";
        ofn.lpstrFile   = szPortFileName;
        ofn.lpstrFilter = szPortSim;
        ofn.lpstrDefExt = TEXT ("sim");
        bReturn = GetOpenFileName(&ofn);
        break;
    }
    LeaveCriticalSection(&cs);
    return bReturn;
}


LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
#if defined SUPPORT_TOUCH_SCREEN                                         // {56}
    static int iPenDown = 0;
#endif
    int wmId, wmEvent;
    PAINTSTRUCT ps;
    HDC hdc;
    RECT rt;
    LPRECT rect = &rt;

    switch (message) {
        case WM_CREATE:
            {
#if defined _WITH_STATUS_BAR                                             // {84}
            // Create status bar
            //
            int Statwidths[] = {100, -1};                                // create status bar "compartments" one width 100 .. last -1 means that it fills the rest of the window
            hStatus = CreateWindowEx(0, STATUSCLASSNAME, NULL, WS_CHILD | WS_VISIBLE | SBARS_SIZEGRIP, 0, 0, 0, 0, hWnd, (HMENU)IDC_STATUS_BAR_1, GetModuleHandle(NULL), NULL);
            SendMessage(hStatus, SB_SETPARTS, (WPARAM)(sizeof(Statwidths)/sizeof(int)), (LPARAM)Statwidths);
#endif
#if defined BUZZER_SOUND
            if (midiOutOpen(&hMidiOut, MIDIMAPPER, 0, 0, 0) == 0) {      // open standard midi output for use by the buzzer output
                MidiOutMessage(hMidiOut, 0xc0, 0, 123, 0);               // program change
            }
#endif
#if defined ETH_INTERFACE                                                // {10}
            iUserNIC = fnGetUserNIC();
            fnWinPcapSelectLAN(iUserNIC);                                // select the NIC according to the user setting         
#endif
            hPen = (HPEN)CreatePen(0, 0, RGB(180, 180, 180));            // create a pen with the colour for drawing the LAN LEDs {4}
            hGreenBrush = (HBRUSH)CreateSolidBrush(RGB(0, 255, 0));
            hRedBrush = (HBRUSH)CreateSolidBrush(RGB(220, 0, 0));
#if defined USB_INTERFACE                                                // create pens used by USB
            hRedPen = (HPEN)CreatePen(0,(USB_CIRCLE_RADIUS/2), RGB(220, 0, 0));
            hGrayBrush = (HBRUSH)CreateSolidBrush(RGB(140, 140, 140));
            hGrayPen = (HPEN)CreatePen(0, (USB_CIRCLE_RADIUS/2), RGB(140, 140, 140));
            hGreenPen = (HPEN)CreatePen(0, (3), RGB(0, 255, 0));
#elif defined SDCARD_SUPPORT                                             // create pens used by SD card (if not created by USB)
            hRedPen = (HPEN)CreatePen(0, 2, RGB(220, 0, 0));
            hGrayBrush = (HBRUSH)CreateSolidBrush(RGB(140, 140, 140));
            hGrayPen = (HPEN)CreatePen(0, 2, RGB(140, 140, 140));
            hGreenPen = (HPEN)CreatePen(0,(3), RGB(0, 255, 0));
#endif
#if defined nRF24L01_INTERFACE || defined ENC424J600_INTERFACE
            _beginthread(fnUDP_socket, 0, NULL);
#endif
            }
            break;

        case WM_SIZE:
#if defined _WITH_STATUS_BAR                                             // {84}
            SendMessage(GetDlgItem(hWnd, IDC_STATUS_BAR_1), WM_SIZE, 0, 0);
#endif
            break;

        case WM_MOUSEMOVE:
#if defined SUPPORT_TOUCH_SCREEN                                         // {56}
            if (iPenDown != 0) {
                if (fnPenDown(LOWORD(lParam), HIWORD(lParam), iPenDown) != 0) {  // check whether pen stays down on touch screen
                    SetCursor(LoadCursor(NULL, IDC_UPARROW));
                }
                else {
                    iPenDown = 0;
                }
                break;
            }
#endif
#if defined SUPPORT_KEY_SCAN || defined BUTTON_KEY_DEFINITIONS || defined KEYPAD // {83}{89}
            if (fnCheckKeypad(LOWORD(lParam), HIWORD(lParam), 3) != 0) {
    #if defined _WITH_STATUS_BAR                                         // {84}
                char cCoord[20];
        #if _VC80_UPGRADE >= 0x0600
                SPRINTF(cCoord, 20, "X = %i, Y = %i", (LOWORD(lParam) - keypad_x), (HIWORD(lParam) - keypad_y));
        #else
                SPRINTF(cCoord, "X = %i, Y = %i", (LOWORD(lParam) - keypad_x), (HIWORD(lParam) - keypad_y));
        #endif
                SendMessage((HWND)hStatus, (UINT) SB_SETTEXT, (WPARAM)(INT) 0 | 0, (LPARAM) (LPSTR)cCoord);
                iDisplayCoord = 1;
    #endif
                SetCursor(LoadCursor(NULL, IDC_CROSS));
            }
    #if defined _WITH_STATUS_BAR                                         // {84}
            else {
                if (iDisplayCoord != 0) {                                // if leaving the key pad / front-panel region
                    char cCoord[] = {' ', ' ',' ',' ',' ',' ',' ',' ',' ',' ', ' ',' ',' ',' ',' ',' ',' ',' ',' ',0};
                    SendMessage((HWND)hStatus, (UINT) SB_SETTEXT, (WPARAM)(INT) 0 | 0, (LPARAM) (LPSTR)cCoord);
                    iDisplayCoord = 0;
                }
            }
    #endif
#endif
            if (fnToggleInput(LOWORD(lParam), HIWORD(lParam), POSSIBLE_PORT)) {
                fnToggleInput(LOWORD(lParam), HIWORD(lParam), PORT_LOCATION);
            }
            else if (iLastPort != iPrevPort) {                           // {36} check whether mouse moves away from port area
                iPrevPort = iLastPort;
                RECT port_detail = present_ports_rect;
                port_detail.top = (port_detail.bottom - 70);             // just update the port detail line
                InvalidateRect(ghWnd, &port_detail, FALSE);              // update ports to see new details
            }
            if ((iLastPort != iPrevPort) || (iLastBit != iPrevBit)) {    // change of port bit detected
                iPrevPort = iLastPort;
                iPrevBit = iLastBit;
                if (iLastPort >= 0) {
                    RECT port_detail = present_ports_rect;
                    port_detail.top = (port_detail.bottom - 70);         // just update the port detail line
                    InvalidateRect(ghWnd, &port_detail, FALSE);          // update ports to see new details
                }
            }
            break;

        case WM_LBUTTONDOWN:
            iInputChange = fnToggleInput(LOWORD(lParam), HIWORD(lParam), TOGGLE_PORT);
#if defined SUPPORT_TOUCH_SCREEN                                         // {56}
            if (fnPenDown(LOWORD(lParam), HIWORD(lParam), 0) != 0) {     // check whether pen down on touch screen
                SetCursor(LoadCursor(NULL, IDC_UPARROW));
                iPenDown = 1;
                break;
            }
#endif
        case WM_RBUTTONDOWN:
#if defined SUPPORT_KEY_SCAN || defined BUTTON_KEY_DEFINITIONS           // {83}
            if (message == WM_RBUTTONDOWN) {
                iInputChange |= (wmEvent = fnCheckKeypad(LOWORD(lParam), HIWORD(lParam), 2)); // toggle
            }
            else {
                iInputChange |= (wmEvent = fnCheckKeypad(LOWORD(lParam), HIWORD(lParam), 1)); // set
            }
            if (wmEvent = 0) {
                SetCursor(LoadCursor(NULL, IDC_CROSS));
            }
#endif
            break;

        case WM_LBUTTONUP:
#if defined SUPPORT_TOUCH_SCREEN                                         // {56}
            if (iPenDown != 0) {
                fnPenDown(LOWORD(lParam), HIWORD(lParam), -1);           // pen removed
            }
            iPenDown = 0;
#endif
#if defined SUPPORT_KEY_SCAN || defined BUTTON_KEY_DEFINITIONS
            iInputChange |= wmEvent = fnCheckKeypad(LOWORD(lParam), HIWORD(lParam), 0);
            if (wmEvent) {
                SetCursor(LoadCursor(NULL, IDC_CROSS));
            }
#endif
#if defined SDCARD_SUPPORT && !defined NAND_FLASH_FAT                    // {77}
            fnCheckSDCard(LOWORD(lParam), HIWORD(lParam));
#endif
            break;

        case WM_KEYDOWN:
            if (0x10 == wParam) {                                        // shift key down {11}
                iShiftPressed = 1;
            }
#if defined SUPPORT_KEY_SCAN || defined BUTTON_KEY_DEFINITIONS           // {88}
            else if (VK_ESCAPE == wParam) {                              // ESC key pressed
                iInputChange = fnCheckKeypad(-1, -1, 0);
            }
#endif
            break;

        case WM_KEYUP:
            if (0x10 == wParam) {                                        // shift key up {11}
                iShiftPressed = 0;
            }
            break;


        case WM_COMMAND:
            wmId    = LOWORD(wParam); 
            wmEvent = HIWORD(wParam); 
            switch( wmId ) 
            {
                case IDM_ABOUT:
                    DialogBox(hInst, (LPCTSTR)IDD_ABOUTBOX, hWnd, (DLGPROC)About);
                    break;

                case IDM_EXIT:
                    DestroyWindow( hWnd );
                    break;

                case ID_TEST_RX:
                    _main(SIM_TEST_RX_0, 0);
                    break;

                case ID_TEST_R1:
                    _main(SIM_TEST_RX_1, 0);
                    break;

                case ID_TEST_R2:
                    _main(SIM_TEST_RX_2, 0);
                    break;

                case ID_TEST_R3:                                         // {9}
                    _main(SIM_TEST_RX_3, 0);
                    break;

                case ID_TEST_R4:                                         // {41}
                    _main(SIM_TEST_RX_4, 0);
                    break;

                case ID_TEST_R5:
                    _main(SIM_TEST_RX_5, 0);
                    break;

                case ID_TEST_EXT_R0:
                    _main(SIM_TEST_EXT_RX_0, 0);
                    break;

                case ID_TEST_EXT_R1:
                    _main(SIM_TEST_EXT_RX_1, 0);
                    break;

                case ID_TEST_EXT_R2:
                    _main(SIM_TEST_EXT_RX_2, 0);
                    break;

                case ID_TEST_EXT_R3:
                    _main(SIM_TEST_EXT_RX_3, 0);
                    break;

                case ID_TEST_ENUMERATION:                                // {6}
                    _main(SIM_TEST_ENUM, 0);
                    break;

                case ID_USB_DISCONNECT:                                  // {13}
                    _main(SIM_TEST_DISCONNECT, 0);
                    break;

                case ID_USB_CONNECTLOWSPEEDDEVICE:                       // {17}
                    _main(SIM_TEST_LOWSPEED_DEVICE, 0);
                    break;

                case ID_USB_CONNECTFULLSPEEDDEVICE:                      // {17}
                    _main(SIM_TEST_FULLSPEED_DEVICE, 0);
                    break;

                case ID_PORTSIMULATOR_OPENSCRIPT:                        // {8}
                    if (fnOpenDialog(hWnd, 1) == 0) {                    // {18} select the file to be opened
                        break;                                           // file not found or user has quit selection
                    }
                    // Fall through to simulate
                    //
                case ID_PORTSIM_REPEATLASTSCRIPT:                        // {18}
#if !defined BOOT_LOADER
                    fnDoPortSim(1, szPortFileName);                      // open a port script file and play it back
#endif
                    break;

#if defined ETH_INTERFACE                                                // {10}
                case ID_ETHEREAL:
                    if (fnOpenDialog(hWnd, 0) == 0) {                    // {18} select the file to be opened
                        break;                                           // file not found or user has quit selection
                    }
                    // Fall through to start simulation
                    //
                case ID_WIRESHARK_REPLAYLASTFILE:                        // {18}                                                    
                    fnWinPcapStopLink(hWnd);                             // we close any open NIC so that the two sources do not conflict with another
                    fnDoEthereal(1, szEthernetFileName);                 // we load an ethereal file and play it back through our code
#endif
                    break;
#if defined SLCD_FILE                                                    // {65}
                case ID_SLCD_SHOWALLSEGMENTS:                            // turn on all SLCD segments to verify SLCD display
                    LCDinit(2, 0);
                    break;

                case ID_SLCD_RELEASESEGMENTS:                            // leave show all segments mode
                    LCDinit(1, 0);
                    break;
#endif
                case ID_LAN_NIC:
#if defined ETH_INTERFACE                                                // {10}
                    DialogBox(hInst, (LPCTSTR)IDD_NIC, hWnd, (DLGPROC)SetNIC);
#endif
                    break;

                default:
                    return DefWindowProc(hWnd, message, wParam, lParam);
            }
            break;

        case WM_PAINT:
            if (GetUpdateRect(hWnd, rect, FALSE)) {
                hdc = BeginPaint(hWnd, &ps);
                fnDoDraw(hWnd, hdc, ps, rt);                             // we do user specific stuff here
                EndPaint(hWnd, &ps);
            }
            break;

        case WM_DESTROY:
#if defined BUZZER_SOUND
            midiOutReset(hMidiOut);
            midiOutClose(hMidiOut);
#endif
            DeleteObject(hPen);                                          // {4} destroy objects on termination
            DeleteObject(hGreenBrush);
            DeleteObject(hRedBrush); 
            DeleteObject(hGrayBrush);
            iQuit = 1;
            PostQuitMessage( 0 );
            break;

        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
   }
   return 0;
}

LRESULT CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message) {
        case WM_INITDIALOG:
                return TRUE;

        case WM_COMMAND:
            if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) 
            {
                EndDialog(hDlg, LOWORD(wParam));
                return TRUE;
            }
            break;
    }
    return FALSE;
}

LRESULT CALLBACK CardReset(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message) {
        case WM_INITDIALOG:
                return TRUE;

        case WM_COMMAND:
            if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) 
            {
                EndDialog(hDlg, LOWORD(wParam));
                return TRUE;
            }
            break;
    }
    return FALSE;
}

LRESULT CALLBACK CardWatchdogReset(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message) {
        case WM_INITDIALOG:
                return TRUE;

        case WM_COMMAND:
            if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) 
            {
                EndDialog(hDlg, LOWORD(wParam));
                return TRUE;
            }
            break;
    }
    return FALSE;
}

#if defined ETH_INTERFACE                                                // {10}
LRESULT CALLBACK SetNIC(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    HWND pulldown;
    static int iactive = 0;
    static int iNewSelection = 0;

    switch (message) {
        case WM_INITDIALOG:                                              // add the NICs to the drop down list, then select the active one
            pulldown = GetDlgItem(hDlg, IDC_COMBO1);
            iNewSelection = iactive = fnShowNICs(pulldown);
            if (iactive < 0) {
                SendMessage(pulldown, CB_SETCURSEL, 0, (LPARAM) 0); 
            }
            else {
                SendMessage(pulldown, CB_SETCURSEL, iactive+1, (LPARAM) 0);          
            }
            SetFocus(pulldown);
            return TRUE;

        case WM_COMMAND:
            switch (LOWORD(wParam)) {
            case IDC_COMBO1:
                switch (HIWORD(wParam)) {
                case CBN_SELCHANGE:
                   pulldown = GetDlgItem(hDlg, IDC_COMBO1);
                   iNewSelection = SendMessage(pulldown, CB_GETCURSEL, 0, (LPARAM) 0); 
                   iNewSelection--;
                   break;
                }
                break;

            default:
                switch (LOWORD(wParam)) {
                case IDCANCEL:
                    EndDialog(hDlg, LOWORD(wParam));
                    return TRUE;

                case IDOK: 
                    if (iNewSelection != iactive) {
                      //if (iUserNIC >= 0) {                             // {105}
                            iactive = iNewSelection;
                            fnWinPcapClose();                            // close present adapter
                            fnWinPcapSelectLAN(iNewSelection);
                            fnWinPcapOpenAdapter();
                      //}
                        iUserNIC = iNewSelection;
                    }
                    EndDialog(hDlg, LOWORD(wParam));
                    return TRUE;
                }
                break;
            }
            break;
    }
    return FALSE;
}
#endif

#if defined ETH_INTERFACE                                                // {10}
extern void fnReactivateNIC(void)
{
    fnWinPcapSelectLAN(iUserNIC);
    fnWinPcapOpenAdapter();
}
#endif

#if defined SERIAL_INTERFACE

static DWORD fnCheckRx(HANDLE m_hComm, unsigned char *pData)
{
    DWORD dwBytesRead = 0;
    _OVERLAPPED ol = {0};

    ReadFile(m_hComm, pData, 100, &dwBytesRead, &ol);

    return dwBytesRead;
}


static void fnSetComPort(HANDLE m_hComm, DWORD dwBaud, UART_MODE_CONFIG Mode) // {26}
{
    DCB dcb;

    GetCommState(m_hComm, &dcb);                                         // get the present com port settings

    if (dwBaud < 250) {                                                  // often the speed can not be set accurately enough for windows to understand it - se we help out a bit here
        dwBaud = 110;
    }
    else if (dwBaud <400) {
        dwBaud = 300;
    }
    else if (dwBaud < 800) {
        dwBaud = 600;
    }
    else if (dwBaud < 1400) {
        dwBaud = 1200;
    }
    else if (dwBaud < 2800) {
        dwBaud = 2400;
    }
    else if (dwBaud < 5400) {
        dwBaud = 4800;
    }
    else if (dwBaud < 12000) {
        dwBaud = 9600;
    }
    else if (dwBaud < 16000) {
        dwBaud = 14400;
    }
    else if (dwBaud < 22000) {
        dwBaud = 19200;
    }
    else if (dwBaud < 44000) {
        dwBaud = 38400;
    }
    else if (dwBaud < 64000) {
        dwBaud = 57600;
    }
    else {
        dwBaud = 115200;
    }

    dcb.BaudRate = dwBaud;                                               // set the baud rate

    if (Mode & RS232_EVEN_PARITY) {
        dcb.Parity = EVENPARITY;
    }
    else if (Mode & RS232_ODD_PARITY) {
        dcb.Parity = ODDPARITY;
    }
    else {
        dcb.Parity = NOPARITY;
    }
    if (CHAR_7 & Mode) {
        dcb.ByteSize = 7;
    }
    else {
        dcb.ByteSize = 8;
    }

    if (TWO_STOPS & Mode) {
        dcb.StopBits = TWOSTOPBITS; 
    }
    else if (ONE_HALF_STOPS & Mode) {
        dcb.StopBits = ONE5STOPBITS;
    }
    else {
        dcb.StopBits = ONESTOPBIT;
    }

    dcb.fDsrSensitivity = FALSE;                                         // setup the flow control 

    dcb.fOutxCtsFlow = FALSE;
    dcb.fOutxDsrFlow = FALSE;
    dcb.fOutX = FALSE;
    dcb.fInX = FALSE;

    if (Mode & RTS_CTS) {
        dcb.fOutxCtsFlow = TRUE;                                         // enable flow control on the COM port for higher accuracy
        dcb.fRtsControl = 0;                                             // disable the RTS line when starting with HW flow control
    }

    SetCommState(m_hComm, &dcb);                                         // set the new state
}

static HANDLE fnConfigureSerialInterface(char cCom, DWORD com_port_speed, UART_MODE_CONFIG Mode) // {26}
{
    // We send simulation messages over serial interface and receive from serial interface.
    // Call CreateFile to open up the comms port
    //
    char cPort[BUFSSIZE];
    char cComPort[4] = {0};
    HANDLE m_hComm;
    COMMTIMEOUTS Timeouts;
    #if defined SIM_COM_EXTENDED                                         // {29}
    unsigned char ucCom = (unsigned char)cCom;
    if (ucCom == 0x00) {
        return INVALID_HANDLE_VALUE;
    }
    STRCPY(cPort, "\\\\.\\COM");
    if (ucCom >= 10) {
        unsigned char ucHundreds = (ucCom/100);
        if (ucHundreds != 0) {            
            unsigned char ucTens;
            ucCom -= (ucHundreds * 100);
            ucTens = (ucCom/10);
            ucCom -= (ucTens * 10);
            cComPort[0] = (ucHundreds + '0');
            cComPort[1] = (ucTens + '0');
            cComPort[2] = (ucCom + '0');
            cComPort[3] = 0;
        }
        else {
            unsigned char ucTens = (ucCom/10);
            ucCom -= (ucTens * 10);
            cComPort[0] = (ucTens + '0');
            cComPort[1] = (ucCom + '0');
            cComPort[2] = 0;
        }
    }
    else {
        cComPort[0] = (ucCom + '0');
        cComPort[1] = 0;
    }
    STRCAT(cPort, cComPort);
    #else
    STRCPY(cPort, "\\\\.\\COM");
    cComPort[0] = cCom;
    cComPort[1] = 0;
    STRCAT(cPort, cComPort);
    #endif
    m_hComm = CreateFile(cPort, (GENERIC_READ | GENERIC_WRITE), 0, NULL, OPEN_EXISTING, FILE_FLAG_OVERLAPPED , NULL);
    if (m_hComm == INVALID_HANDLE_VALUE) {
        return m_hComm;
    }

    if (com_port_speed > 115200) {                                       // limit COM speed to max supported by interface
        com_port_speed = 115200;
    }
    fnSetComPort(m_hComm, com_port_speed, Mode);                         // we have successfully opened the com port as defined

    ZeroMemory(&Timeouts, sizeof(COMMTIMEOUTS));                         // set read and write time outs: in this case reads and writes are performed immediatly and returns without blocking
    Timeouts.ReadIntervalTimeout = MAXDWORD;
    Timeouts.ReadTotalTimeoutMultiplier = 0;
    Timeouts.ReadTotalTimeoutConstant = 0;
    Timeouts.WriteTotalTimeoutMultiplier = 0;
    Timeouts.WriteTotalTimeoutConstant = 0;

    if (SetCommTimeouts(m_hComm, &Timeouts) == 0) {
        printf(TEXT("Failed in call to SetCommTimeouts\n"));
    }

    return m_hComm;
}

static DWORD fnSendSerialMessage(HANDLE m_hComm, const void *lpBuf, DWORD dwCount)
{
    DWORD dwBytesWritten = 0;
    static _OVERLAPPED ol = {0};                                         // {3}

    WriteFileEx(m_hComm, lpBuf, dwCount, &ol, 0);

    return dwBytesWritten;
}
#endif

// Intermediate call to ensure that the calling thread waits on other active threads
//
static int _main(int argc, char *argv[])                                 // {23}
{
    int iRtn;
    while (WAIT_WHILE_BUSY == (iRtn = main(argc, argv))) {
        Sleep(1);                                                        // yield temporarily until other threads have completed their work
    }
    return iRtn;
}

#if defined SUPPORT_LCD || defined SUPPORT_GLCD || defined SUPPORT_OLED || defined SUPPORT_TFT || defined GLCD_COLOR // {35}
extern void fnRedrawDisplay(void)
{
    InvalidateRect(ghWnd, NULL, FALSE);                                  // {44}
}
#endif

#if defined _WITH_STATUS_BAR                                             // {100}
extern "C" void fnPostOperatingDetails(char *ptrDetails)
{
    #if defined SERIAL_INTERFACE
    int iUART = 0;
    #endif
    STRCPY(cOperatingDetails, ptrDetails);
    #if defined SERIAL_INTERFACE
    while (iUART < UARTS_AVAILABLE) {
        if (cUART[iUART][0] != 0) {
            STRCAT(cOperatingDetails, cUART[iUART]);
        }
        iUART++;
    }
    #endif
}
#endif



#if defined nRF24L01_INTERFACE || defined ENC424J600_INTERFACE

#if defined nRF24L01_INTERFACE
    static unsigned char ucRegisters_nRF24L01[0x1e] = {0x08, 0x3f, 0x03, 0x03, 0x03, 0x02, 0x0e, 0x0e, 0x00, 0x00,
        0xe7, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xe7,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
        0x11,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00
    };

    static unsigned char uc_nRF24L01_RxFifo[32] = {0};
    static unsigned char uc_nRF24L01_PayloadLength = 0;
    static unsigned char ucPayloadRead = 0;
#endif
#if defined ENC424J600_INTERFACE
    extern "C" unsigned char fnMapPortBit(unsigned long ulRealBit);
    static int iSPI_response = 0;
    static unsigned char ucETH_Rx_buffer[1500];
#endif

static SOCKET remote_simulation_socket_server = 0;
static SOCKET remote_simulation_socket_client = 0;
static void fnUDP_socket(PVOID pvoid)
{
#if defined USE_SIMULATION_INTERFACE
    unsigned long rc;
    SOCKADDR_IN addr;
    WSADATA wsa;
    unsigned char ucBuffer[1500];
    rc = WSAStartup(MAKEWORD(2,0),&wsa);
    if (rc != 0) {
        printf("Fehler: startWinsock, error code: %d\n", rc);
        return;
    }
    else {
        printf("Winsock started!\n");
    }
    remote_simulation_socket_server = socket(AF_INET,SOCK_DGRAM,0);      // UDP socket for simulation purposes
    remote_simulation_socket_client = socket(AF_INET,SOCK_DGRAM,0);      // UDP socket for simulation purposes

    if (remote_simulation_socket_server == INVALID_SOCKET) {
        printf("Error: server couldn't be created\n");
    }
    if (remote_simulation_socket_client == INVALID_SOCKET) {
        printf("Error: client couldn't be created\n");
    }

    addr.sin_family = AF_INET;
    addr.sin_port = htons(42989);
    addr.sin_addr.S_un.S_addr = inet_addr("192.168.0.98");

    rc = connect(remote_simulation_socket_client,(SOCKADDR *)&addr, sizeof(SOCKADDR_IN));

    addr.sin_addr.S_un.S_addr = ADDR_ANY;
    rc = bind(remote_simulation_socket_server,(SOCKADDR *)&addr, sizeof(SOCKADDR_IN));

    while (1) {
        rc = recv(remote_simulation_socket_server, (char *)ucBuffer, sizeof(ucBuffer), 0); // wait until data is received
        switch (ucBuffer[0]) {
    #if defined nRF24L01_INTERFACE
        case REMOTE_RF_INTERFACE:                                       // from RF interface
            switch (ucBuffer[1]) {
            case REMOTE_RF_IRQ_EVENT:                                    // interrupt has occurred
                ucRegisters_nRF24L01[7] = ucBuffer[2];                   // status register value before and reads
                ucRegisters_nRF24L01[8] = ucBuffer[3];                   // tx observe
                ucRegisters_nRF24L01[9] = ucBuffer[4];                   // receive power detector
                ucRegisters_nRF24L01[11] = ucBuffer[5];                  // number of bytes in data pipe 0
                ucRegisters_nRF24L01[12] = ucBuffer[6];                  // number of bytes in data pipe 1
                ucRegisters_nRF24L01[13] = ucBuffer[7];                  // number of bytes in data pipe 2
                ucRegisters_nRF24L01[14] = ucBuffer[8];                  // number of bytes in data pipe 3
                ucRegisters_nRF24L01[15] = ucBuffer[9];                  // number of bytes in data pipe 4
                ucRegisters_nRF24L01[16] = ucBuffer[10];                 // number of bytes in data pipe 5
                ucRegisters_nRF24L01[17] = ucBuffer[11];                 // fifo status register
                uc_nRF24L01_PayloadLength = ucBuffer[12];
                if (uc_nRF24L01_PayloadLength <= 32) {
                    memcpy(uc_nRF24L01_RxFifo, &ucBuffer[13], uc_nRF24L01_PayloadLength);
                }
              //fnSimPortInputToggle(2, (31 - 18)); // PORTC_BIT18 (make configurable)
              //fnProcessInputChange();
                fnSimulateInputChange((unsigned char)PORTC, (unsigned char)(31 - 18), CLEAR_INPUT);
                break;
            }
        break;
    #endif
    #if defined ENC424J600_INTERFACE
        case REMOTE_ETH_INTERFACE:
            switch (ucBuffer[1]) {
            case REMOTE_ETH_RX_DATA:
                {
                    unsigned short usLength = ((ucBuffer[2] << 8) | ucBuffer[3]);
                    memcpy(ucETH_Rx_buffer, &ucBuffer[4], usLength);
                    iSPI_response = usLength;
                }
                break;
            case REMOTE_ETH_INTERRUPT:
                fnSimulateInputChange((unsigned char)ENC424J600_IRQ_PORT, fnMapPortBit(ENC424J600_IRQ), CLEAR_INPUT);
                break;
            }            
        break;
    #endif
        }
    }
#endif
}



#if defined ENC424J600_INTERFACE && defined ETH_INTERFACE

extern "C" void fnSimulateEthTx(int, unsigned char *);

static unsigned char ucENC424J600_Memory[24 * 1024]= {0};
static unsigned char ucENC424J600_Registers[0x91] = {0};
static unsigned short usRxPutOffset = 0;
static int iBufferSpace = -1;

static int fnIsENC424J600(unsigned char *ptrData)
{
    if (*ptrData++ != ucENC424J600_Registers[0x64]) {
        return -1;
    }
    if (*ptrData++ != ucENC424J600_Registers[0x65]) {
        return -1;
    }
    if (*ptrData++ != ucENC424J600_Registers[0x62]) {
        return -1;
    }
    if (*ptrData++ != ucENC424J600_Registers[0x63]) {
        return -1;
    }
    if (*ptrData++ != ucENC424J600_Registers[0x60]) {
        return -1;
    }
    if (*ptrData++ != ucENC424J600_Registers[0x61]) {
        return -1;
    }
    return 0;                                                            // successful unicast match
}

static unsigned short fnAddData(unsigned short usRxPutOffset, unsigned char *ptrData, unsigned short usTop, unsigned short usBottom, unsigned short usLength)
{
    if (usTop < ((24 * 1024) - 2)) {
        usTop = ((24 * 1024) - 2);
    }
    while (usLength--) {
        ucENC424J600_Memory[usRxPutOffset++] = *ptrData++;
        if (usRxPutOffset > usTop) {                                     // handle circular buffer
            usRxPutOffset = usBottom;
        }
    }
    return usRxPutOffset;
}

    #if defined USE_IPV6 || defined USE_IGMP
static int fnIsHashMulticast(unsigned char *ucData)
{
    #define CRC_POLY 0xedb88320                                          // CRC-32 polynomial in reverse direction
    unsigned long ulCRC32 = 0xffffffff;
    unsigned short usHashReg = 0x20;                                     // first hash register reference
    int i, bit;
    unsigned char ucByte;
    if ((*ucData & 0x01) == 0) {                                         // if not multicast, don't check
        return 0;
    }
	for (i = 0; i < MAC_LENGTH; i++) {
        ucByte = *ucData++;;
        for (bit = 0; bit < 8; bit++) {
            if ((ulCRC32 ^ ucByte) & 1) {
                ulCRC32 = (ulCRC32 >> 1) ^ CRC_POLY;
            }
            else {
                ulCRC32 >>= 1;
            }
            ucByte >>= 1;
        }
	}
    ulCRC32 >>= 26;                                                      // the upper 6 bits of the CRC32 detemine the hash entry location
    while (ulCRC32 > 7) {
        ulCRC32 -= 8;
        usHashReg++;
    }
    if (ucENC424J600_Registers[usHashReg] & (1 << ulCRC32)) {
        return 1;                                                        // multicast hash match
    }
    return 0;                                                            // not a match
}
    #endif

// Check whether a received Ethernet frame can be accepted
//
extern "C" int fnCheckENC424J600(unsigned char *ucData, unsigned short usLen, int iForce)
{
    static const unsigned char BroadcastMAC[MAC_LENGTH] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
    unsigned long ulAccept = 0;
    if ((ucENC424J600_Registers[0x1e] & 0x01) == 0) {                    // if the receiver is not enabled ignore receptions
        return 0;
    }
    #if !defined _DEVELOP_SIM
        if (fnIsENC424J600(&ucData[MAC_LENGTH]) == 0) {
            return 0;                                                    // we never accept echos of our own transmissions - check before broadcast
        }
    #endif
    if (ucENC424J600_Registers[0x34] & 0x01) {                           // broadcasts accepted
        if (memcmp(ucData, BroadcastMAC, MAC_LENGTH) == 0) {
            ulAccept = 0x200;
        }
    }
    if (ulAccept == 0) {
        if (ucENC424J600_Registers[0x34] & 0x4) {                        // unicast accepted
            if (fnIsENC424J600(ucData) == 0) {
                ulAccept = 0x00100000;
            }
        }
    #if defined USE_IPV6 || defined USE_IGMP
        if ((ulAccept & 0x00100000) == 0) {                              // if not unicast match
            if (ucENC424J600_Registers[0x35] & 0x80) {                   // if multicast enabled
                if (fnIsHashMulticast(ucData) != 0) {                    // check for multicast hash match
                    ulAccept = 0x00020000;                               // hash match
                }
            }
        }
    #endif
    }

    if (ulAccept != 0) {
        unsigned char ucHeader[8];
        unsigned short usNextOffset;
        unsigned short usBottom = ucENC424J600_Registers[4];
        unsigned short usTop = ucENC424J600_Registers[6];
        usTop |= (ucENC424J600_Registers[7] << 8);
        usBottom |= (ucENC424J600_Registers[5] << 8);
        if (iBufferSpace < 0) {
            iBufferSpace = (usTop - usBottom);                           // on first usage set the buffer space available
        }
        usLen += 4;                                                      // add CRC-32 length
        usNextOffset = (usRxPutOffset + usLen + 8 - 2);                  // next offset after present content (header, frame including CRC)
        if ((usNextOffset & 1) != 0) {                                   // always even aligned
            usNextOffset++;
        }
        if (usNextOffset > ((24 * 1024) - 2)) {                          // handle circular buffer
            usNextOffset = (usBottom + (usNextOffset - ((24 * 1024) - 2)));
        }
        if ((usLen + 8 + 1) > iBufferSpace) {                            // if there is no space for the frame we drop it
            return 0;
        }
        ucENC424J600_Registers[0x1a]++;                                  // increment ESTAT packet count
        ucHeader[0] = (unsigned char)usNextOffset;                       // next data offset
        ucHeader[1] = (unsigned char)(usNextOffset >> 8);
        ucHeader[2] = (unsigned char)usLen;                              // length, including CRC-32
        ucHeader[3] = (unsigned char)(usLen >> 8);
        ulAccept |= 0x80;                                                // frame is OK
        ucHeader[4] = (unsigned char)(ulAccept);                         // flags
        ucHeader[5] = (unsigned char)(ulAccept >> 8);
        ucHeader[6] = (unsigned char)(ulAccept >> 16);
        ucHeader[7] = (unsigned char)(ulAccept >> 24);
        usRxPutOffset = fnAddData(usRxPutOffset, ucHeader, usTop, usBottom, 8); // add the header
        usRxPutOffset = fnAddData(usRxPutOffset, ucData, usTop, usBottom, usLen); // add the frame content (with random CRC-32 at end)
        iBufferSpace -= (usLen + 8);
        if ((usRxPutOffset & 1) != 0) {
            usRxPutOffset = fnAddData(usRxPutOffset, ucData, usTop, usBottom, 1); // pad
            iBufferSpace--;
        }
        if (usRxPutOffset == usBottom) {                                 // special case due to buffer wrap-around
            usRxPutOffset = ((24 * 1024) - 2);
        }
        else {
            usRxPutOffset = (usRxPutOffset - 2);
        }
        ucENC424J600_Registers[0x1c] |= 0x40;                            // set the reception interrupt flag
        if (ucENC424J600_Registers[0x72] & 0x40) {                       // if interrupts are enabled
            fnSimulateInputChange((unsigned char)ENC424J600_IRQ_PORT, fnMapPortBit(ENC424J600_IRQ), CLEAR_INPUT);
        }
        return 1;
    }
    else {
        return 0;                                                        // frame rejected due to no matching filter
    }
}
#endif

extern "C" unsigned long fnRemoteSimulationInterface(int iInterfaceReference, unsigned char ucCommand, unsigned char *ptrData, unsigned short usDataLength, int iCollect)
{
#if defined USE_SIMULATION_INTERFACE
    #if defined nRF24L01_INTERFACE
    static unsigned char ucRF_FrameContent[1500];
    static unsigned short usRF_FrameLength = 0;
    static unsigned char ucRF_PresentCommand = 0;
    #endif
    #if defined ENC424J600_INTERFACE
    static unsigned char ucETH_FrameContent[1500];
    static unsigned short usETH_FrameLength = 0;
    static unsigned char ucETH_PresentCommand = 0;
    #endif
    unsigned long ulReturnValue = 0;
    unsigned short usRemainder = 0;
    switch (iInterfaceReference) {
    #if defined nRF24L01_INTERFACE
    case REMOTE_RF_INTERFACE:                                            // RF interface nRF24L01+
        if (REMOTE_RF_W_COMMAND == ucCommand) {
            ulReturnValue = ucRegisters_nRF24L01[7];                     // always return the present status register value
            switch (*ptrData) {
            case 0xff:                                                   // NOP command - don't sent to remote interface but just return the present status register content
                return ulReturnValue;
          //case 0xe1:                                                   // flush command are always sent immediately
          //case 0xe2:
          //    iCollect = 0;
          //    break;
            }
            ucRF_PresentCommand = *ptrData;
        }
        else if (REMOTE_RF_W_REGISTER == ucCommand) {
            ulReturnValue = ucRegisters_nRF24L01[7];                     // always return the present status register value
            ucRF_PresentCommand = *ptrData;
            if (ucRF_PresentCommand == 0x60) {                           // read payload width
                return ulReturnValue;
            }
        }
        else if (REMOTE_RF_W_REGISTER_VALUE == ucCommand) {              // data belonging to a command
            if (ucRF_PresentCommand == 0x60) {                           // read payload width
                ulReturnValue = uc_nRF24L01_PayloadLength;
                break;
            }
            else if (ucRF_PresentCommand == 0x61) {                      // read payload
                ulReturnValue = uc_nRF24L01_RxFifo[ucPayloadRead];
                if (++ucPayloadRead >= uc_nRF24L01_PayloadLength) {
                    uc_nRF24L01_PayloadLength = ucPayloadRead = 0;
                }
                break;
            }
            if ((ucRF_PresentCommand & ~0x1f) == 0x20) {                 // writing a register
                if ((ucRF_PresentCommand & 0x1f) == 0x07) {              // write to status register can only clear certain bits with '1' writes
                    if ((ucRegisters_nRF24L01[7] & 0x70) != 0) {         // an interrupt bit is set
                        ucRegisters_nRF24L01[7] &= ~(*ptrData & 0x70);   // clar bits set to '1'
                        if (*ptrData & 0x70) {
                            iCollect = 0;                                // when resetting interrupt bits always immediately send the remote command
                            if ((ucRegisters_nRF24L01[7] & 0x70) == 0) { // if all interrupt bits have been reset
                                fnSimulateInputChange((unsigned char)ENC424J600_IRQ_PORT, fnMapPortBit(ENC424J600_IRQ), SET_INPUT);
                            }
                        }
                    }
                }
                else {
                    ucRegisters_nRF24L01[ucRF_PresentCommand & 0x1f] = *ptrData;// write first byte to register (some registers have more content)
                }
            }
        }
        ucRF_FrameContent[usRF_FrameLength++] = ucCommand;               // command
        ucRF_FrameContent[usRF_FrameLength++] = (unsigned char)usDataLength;// data length (32 is maximum)
        if ((usDataLength + usRF_FrameLength) <= (1500 - 3)) {           // space enough to prepare complete content
            memcpy(&ucRF_FrameContent[usRF_FrameLength], ptrData, usDataLength);
            usRF_FrameLength += usDataLength;
        }
        else {                                                           // not enough space to add the coomplete content into present frame
            ucRF_FrameContent[usRF_FrameLength - 1] |= 0x80;             // mark that this frame has overflow data to follow
            memcpy(&ucRF_FrameContent[usRF_FrameLength], ptrData, ((1500 - 3) - usRF_FrameLength)); // add as much as we safely can
            usRemainder = (usDataLength - ((1500 - 3) - usRF_FrameLength));
            ptrData += ((1500 - 3) - usRF_FrameLength);
            usRF_FrameLength = (1500 - 3);
            iCollect = 0;
        }
        if (iCollect == 0) {
            send(remote_simulation_socket_client, (const char *)ucRF_FrameContent, usRF_FrameLength, 0);
            usRF_FrameLength = 0;
            if (usRemainder != 0) {
                ucRF_FrameContent[0] = 0x80;                             // extended data
                memcpy(&ucRF_FrameContent[1], ptrData, usRemainder);
                usRemainder++;
                send(remote_simulation_socket_client, (const char *)ucRF_FrameContent, usRemainder, 0);
            }
        }
        break;
    #endif
    #if defined ENC424J600_INTERFACE
    case REMOTE_ETH_INTERFACE:
        if ((REMOTE_ETH_CMD == ucCommand) || (REMOTE_ETH_CMD_WITHOUT_DATA == ucCommand)) {
            ucETH_PresentCommand = *ptrData;
        }
        ucETH_FrameContent[usETH_FrameLength++] = ucCommand;               // command
        ucETH_FrameContent[usETH_FrameLength++] = (unsigned char)(usDataLength >> 8); // data length
        ucETH_FrameContent[usETH_FrameLength++] = (unsigned char)(usDataLength); // data length
        if (REMOTE_ETH_DATA_RX != ucCommand) {
            if ((usDataLength + usETH_FrameLength) <= (1500 - 4)) {           // space enough to prepare complete content
                memcpy(&ucETH_FrameContent[usETH_FrameLength], ptrData, usDataLength);
                usETH_FrameLength += usDataLength;
            }
            else {                                                           // not enough space to add the coomplete content into present frame
                ucETH_FrameContent[usETH_FrameLength - 1] |= 0x80;           // mark that this frame has overflow data to follow
                memcpy(&ucETH_FrameContent[usETH_FrameLength], ptrData, ((1500 - 4) - usETH_FrameLength)); // add as much as we safely can
                usRemainder = (usDataLength - ((1500 - 4) - usETH_FrameLength));
                ptrData += ((1500 - 4) - usETH_FrameLength);
                usETH_FrameLength = (1500 - 4);
                iCollect = 0;
            }
        }
        if (iCollect == 0) {
            send(remote_simulation_socket_client, (const char *)ucETH_FrameContent, usETH_FrameLength, 0);            
            if (usRemainder != 0) {
                ucETH_FrameContent[0] = 0x80;                            // extended data
                memcpy(&ucETH_FrameContent[1], ptrData, usRemainder);
                usRemainder++;
                send(remote_simulation_socket_client, (const char *)ucETH_FrameContent, usRemainder, 0);
            }
            if ((REMOTE_ETH_DATA_RX == ucCommand) || ((REMOTE_ETH_DATA == ucCommand) && ((ucETH_PresentCommand & 0xc0)) == 0)) { // a read was sent so we wait for a response
                while (iSPI_response == 0) {                             // wait until the answer is received
                }
                if (REMOTE_ETH_DATA_RX == ucCommand) {
                    memcpy(ptrData, ucETH_Rx_buffer, iSPI_response);
                }
                else {
                    ulReturnValue = ((ucETH_Rx_buffer[0] << 8) | (ucETH_Rx_buffer[1])); // the returned value
                }
                iSPI_response = 0;
            }
            else if (REMOTE_ETH_DATA == ucCommand) {
                if ((ucETH_PresentCommand == 0xb2) && (ucETH_FrameContent[usETH_FrameLength - 2] & 0x80)) { // detect the command to disable the global interupt, which clears the interrupt input
                   fnSimulateInputChange((unsigned char)ENC424J600_IRQ_PORT, fnMapPortBit(ENC424J600_IRQ), SET_INPUT);
                }
            }
            usETH_FrameLength = 0;
        }
        break;
    }
    #endif
    return ulReturnValue;
#elif defined ENC424J600_INTERFACE && defined ETH_INTERFACE              // remote simulation interface not being used
    static int usTxFrameLength = 0;
    static unsigned char ucTxFrameBuffer[2000];
    static unsigned char ucThisCommand = 0;
    static unsigned char ucPresentBank = 0;
    switch (ucCommand) {
    case REMOTE_ETH_CMD_WITHOUT_DATA:
        ucThisCommand = *ptrData;
        if ((ucThisCommand & 0xc0) == 0xc0) {                            // set the bank
            ucPresentBank = ((ucThisCommand & 0x06) << 4);
        }
        break;
    case REMOTE_ETH_CMD:
        ucThisCommand = *ptrData;
        break;
    case REMOTE_ETH_DATA:
        {
            unsigned short usData = *ptrData++;
            unsigned char ucThisAddress = (ucPresentBank + (ucThisCommand & 0x1f));
            if ((ucThisAddress < 0x80) && ((ucThisAddress & 0x1f) >= 0x16)) {
                ucThisAddress &= ~0x60;                                  // only use bank 0 for storage of registers that can be accessed in all banks 0..3
            }
            usData <<= 8;
            usData |= *ptrData;
            if ((ucThisCommand & 0xe0) == 0) {                           // read
                usData = ucENC424J600_Registers[ucThisAddress];
                usData |= (ucENC424J600_Registers[ucThisAddress + 1] << 8);
                switch (ucThisAddress) {
                case 0x1a:                                               // Ethernet status register - low
                case 0x1b:                                               // Ethernet status register - high
                    ucENC424J600_Registers[0x1b] |= 0x10;                // link up
                    break;
                }
                return usData;
            }
            else {                                                       // write or set
                if ((ucThisCommand & 0xe0) == 0x80) {                    // bit field set
                    ucENC424J600_Registers[ucThisAddress] |= (unsigned char)usData;
                    ucENC424J600_Registers[ucThisAddress + 1] |= (unsigned char)(usData >> 8);
                }
                else if ((ucThisCommand & 0xe0) == 0xa0) {               // bit field clear
                    ucENC424J600_Registers[ucThisAddress] &= ~(unsigned char)usData;
                    ucENC424J600_Registers[ucThisAddress + 1] &= ~(unsigned char)(usData >> 8);
                }
                else {
                    if ((ucThisCommand & 0xe0) == 0x20) {                // unbanked
                        ucThisAddress = (unsigned char)(usData >> 8);
                        usData &= 0xff;
                        ptrData++;
                        usData |= (*ptrData << 8);
                        if (ucThisCommand == 0x20) {                     // read
                            usData = ucENC424J600_Registers[ucThisAddress];
                            usData |= (ucENC424J600_Registers[ucThisAddress + 1] << 8);
                            return usData;
                        }
                    }
                    ucENC424J600_Registers[ucThisAddress] = (unsigned char)usData;
                    ucENC424J600_Registers[ucThisAddress + 1] = (unsigned char)(usData >> 8);
                }
                switch (ucThisAddress) {
                case 0x1e:
                case 0x1f:
                    if (ucENC424J600_Registers[0x1f] & 0x01) {           // ECON1 PKTDEC written with '1'
                        ucENC424J600_Registers[0x1f] &= ~0x01;
                        if (ucENC424J600_Registers[0x1a] != 0) {
                            ucENC424J600_Registers[0x1a]--;              // decrement ESTAT packet count
                        }
                    }
                    if (ucENC424J600_Registers[0x1e] & 0x02) {           // start transmission
                        ucENC424J600_Registers[0x1e] &= ~0x02;
                        fnSimulateEthTx(usTxFrameLength, ucTxFrameBuffer);
                        usTxFrameLength = 0;
                    }
                    break;
                case 0x04:
                case 0x05:
                    usRxPutOffset = usData;                              // write to ERXST which is the offset for the rx pointer
                    break;
                case 0x6e:                                               // Ethernet control 2 - low
                case 0x6f:                                               // Ethernet control 2 - high
                    if (ucENC424J600_Registers[0x6e] & 0x10) {           // reset commanded
                        memset(ucENC424J600_Registers, 0, sizeof(ucENC424J600_Registers)); // reset registers
                        ucENC424J600_Registers[0x1b] = 0x11;             // set clock ready and link-up bits
                        ucENC424J600_Registers[0x34] = 0x05;             // accept unicast and broadcasts by default
                        ucENC424J600_Registers[0x64] = 0x00;             // set factory programmed MAC address
                        ucENC424J600_Registers[0x65] = 0x04;
                        ucENC424J600_Registers[0x62] = 0xa3;
                        ucENC424J600_Registers[0x63] = 0x11;
                        ucENC424J600_Registers[0x60] = 0xd2;
                        ucENC424J600_Registers[0x61] = 0x43;
                        ucPresentBank = 0;                               // reset bank
                        ucThisCommand = 0;
                        usRxPutOffset = 0;
                        iBufferSpace = -1;
                    }
                    break;
                case 0x72:
                case 0x73:
                    if ((ucENC424J600_Registers[0x72] & 0x80) == 0) {    // interrupt cleared, which resets the interrupt output
                        fnSimulateInputChange((unsigned char)ENC424J600_IRQ_PORT, fnMapPortBit(ENC424J600_IRQ), SET_INPUT);
                    }
                }
            }
        }
        break;
    case REMOTE_ETH_DATA_BUFFER:                                         // data transmission
        memcpy(&ucTxFrameBuffer[usTxFrameLength], ptrData, usDataLength);
        usTxFrameLength += usDataLength;
        break;
    case REMOTE_ETH_DATA_RX:
        {
            unsigned short usOffset = ucENC424J600_Registers[0x8a];
            unsigned short usTop = ucENC424J600_Registers[6];
            usTop |= (ucENC424J600_Registers[7] << 8);
            usOffset |= (ucENC424J600_Registers[0x8b] << 8);
            while (usDataLength--) {
                *ptrData++ = ucENC424J600_Memory[usOffset++];            // copy from the reception circular buffer
                iBufferSpace++;
                if (usOffset >= ((24 * 1024) - 2)) {                     // handle wrap around
                    usOffset = ucENC424J600_Registers[6];
                    usOffset |= (ucENC424J600_Registers[7] << 8);
                }
            }
            ucENC424J600_Registers[0x8a] = (unsigned char)usOffset;
            ucENC424J600_Registers[0x8b] = (unsigned char)(usOffset >> 8);
        }
        break;
    }
#endif
    return 0;
}
#endif

#if defined FT800_GLCD_MODE && defined FT800_EMULATOR                    // {110}

#define FT8XXEMU_VERSION_API    9                                        // API version is increased for the library whenever FT8XXEMU_EmulatorParameters format changes or functions are modified

static volatile int iEmulatorReady = 0;                                  // variable used to monitor whether the emulator has competed its initialisaion

typedef unsigned long argb8888;

typedef enum
{
	// frame render has changes since last render
	FT8XXEMU_FrameBufferChanged = 0x01,
	// frame is completely rendered (without degrade)
	FT8XXEMU_FrameBufferComplete = 0x02,
	// frame has changes since last render
	FT8XXEMU_FrameChanged = 0x04,
	// frame rendered right after display list swap
	FT8XXEMU_FrameSwap = 0x08,

	// NOTE: Difference between FrameChanged and FrameBufferChanged is that
	// FrameChanged will only be true if the content of the frame changed,
	// whereas FrameBufferChanged will be true if the rendered buffer changed.
	// For example, when the emulator renders a frame incompletely due to
	// CPU overload, it will then finish the frame in the next callback,
	// and when this is the same frame, FrameChanged will be false,
	// but FrameBufferChanged will be true as the buffer has changed.

	// NOTE: Frames can change even though no frame was swapped, due to
	// several parameters such as REG_MACRO or REG_ROTATE.

	// NOTE: If you only want completely rendered frames, turn OFF
	// the EmulatorEnableDynamicDegrade feature.

	// NOTE: To get the accurate frame after a frame swap, wait for FrameSwap
	// to be set, and get the first frame which has FrameBufferComplete set.

	// NOTE: To get the accurate frame after any frame change, wait for
	// FrameChanged, and get the first frame which has FrameBufferComplete set.
} FT8XXEMU_FrameFlags;

typedef enum
{
	FT8XXEMU_EmulatorFT800 = 0x0800,
	FT8XXEMU_EmulatorFT801 = 0x0801,
	FT8XXEMU_EmulatorFT810 = 0x0810,
	FT8XXEMU_EmulatorFT811 = 0x0811,
	FT8XXEMU_EmulatorFT812 = 0x0812,
	FT8XXEMU_EmulatorFT813 = 0x0813,
} FT8XXEMU_EmulatorMode;

typedef enum
{
	// enables the keyboard to be used as input (default: on)
	FT8XXEMU_EmulatorEnableKeyboard = 0x01,
	// enables audio (default: on)
	FT8XXEMU_EmulatorEnableAudio = 0x02,
	// enables coprocessor (default: on)
	FT8XXEMU_EmulatorEnableCoprocessor = 0x04,
	// enables mouse as touch (default: on)
	FT8XXEMU_EmulatorEnableMouse = 0x08,
	// enable debug shortkeys (default: on)
	FT8XXEMU_EmulatorEnableDebugShortkeys = 0x10,
	// enable graphics processor multithreading (default: on)
	FT8XXEMU_EmulatorEnableGraphicsMultithread = 0x20,
	// enable dynamic graphics quality degrading by interlacing and dropping frames (default: on)
	FT8XXEMU_EmulatorEnableDynamicDegrade = 0x40,
	// enable usage of REG_ROTATE (default: off)
	// FT8XXEMU_EmulatorEnableRegRotate = 0x80, // Now always on
	// enable emulating REG_PWM_DUTY by fading the rendered display to black (default: off)
	FT8XXEMU_EmulatorEnableRegPwmDutyEmulation = 0x100,
	// enable usage of touch transformation matrix (default: on) (should be disabled in editor)
	FT8XXEMU_EmulatorEnableTouchTransformation = 0x200,
} FT8XXEMU_EmulatorFlags;

typedef struct
{
	// Microcontroller function called before loop.
	void(*Setup)();
	// Microcontroller continuous loop.
	void(*Loop)();
	// See EmulatorFlags.
	int Flags;
	// Emulator mode
	FT8XXEMU_EmulatorMode Mode;

	// Called after keyboard update.
	// Supplied function can use Keyboard.isKeyDown(FT8XXEMU_KEY_F3)
	// or FT8XXEMU_isKeyDown(FT8XXEMU_KEY_F3) functions.
	void(*Keyboard)();
	// The default mouse pressure, default 0 (maximum).
	// See REG_TOUCH_RZTRESH, etc.
	unsigned long MousePressure;
	// External frequency. See CLK, etc.
	unsigned long ExternalFrequency;

	// Reduce graphics processor threads by specified number, default 0
	// Necessary when doing very heavy work on the MCU or Coprocessor
	unsigned long ReduceGraphicsThreads;

	// Sleep function for MCU thread usage throttle. Defaults to generic system sleep
	void(*MCUSleep)(int ms);

	// Replaces the default builtin ROM with a custom ROM from a file.
	// NOTE: String is copied and may be deallocated after call to run(...)
	char *RomFilePath;
	// Replaces the default builtin OTP with a custom OTP from a file.
	// NOTE: String is copied and may be deallocated after call to run(...)
	char *OtpFilePath;
	// Replaces the builtin coprocessor ROM.
	// NOTE: String is copied and may be deallocated after call to run(...)
	char *CoprocessorRomFilePath;

	// Graphics driverless mode
	// Setting this callback means no window will be created, and all
	// rendered graphics will be automatically sent to this function.
	// For enabling touch functionality, the functions
	// Memory.setTouchScreenXY and Memory.resetTouchScreenXY must be
	// called manually from the host application.
	// Builtin keyboard functionality is not supported and must be
	// implemented manually when using this mode.
	// The output parameter is false (0) when the display is turned off.
	// The contents of the buffer pointer are undefined after this
	// function returns.
	// Return false (0) when the application must exit, otherwise return true (1).
	int (*Graphics)(int output, const argb8888 *buffer, unsigned long hsize, unsigned long vsize, FT8XXEMU_FrameFlags flags);

	// Interrupt handler
	// void (*Interrupt)();

	// Exception callback
	void(*Exception)(const char *message);

	// Safe exit
	void(*Close)();

} FT8XXEMU_EmulatorParameters;

extern "C" {
    extern void FT8XXEMU_defaults(unsigned long versionApi, FT8XXEMU_EmulatorParameters *params, FT8XXEMU_EmulatorMode mode);
    extern void FT8XXEMU_run(unsigned long versionApi, const FT8XXEMU_EmulatorParameters *params);

    __declspec(dllimport) extern unsigned char (*FT8XXEMU_transfer)(unsigned char data); // transfer data over the imaginary SPI bus. Call from the MCU thread (from the setup/loop callbacks). See FT8XX documentation for SPI transfer protocol
    __declspec(dllimport) extern void (*FT8XXEMU_cs)(int cs);                     // set cable select. Must be set to 1 to start data transfer, 0 to end. See FT8XX documentation for CS_N
    __declspec(dllimport) extern int (*FT8XXEMU_int)();                           // returns 1 if there is an interrupt flag set. Depends on mask. See FT8XX documentation for INT_N
}


// Call back when the emulator has completed its initialisation
//
static void setup(void)
{
    iEmulatorReady = 1;                                                  // this flag is used to stop the LCD task from accessing the FT800 emulation before it has completed its initialisation
}

// Call back when the emulator is idle
//
static void loop(void)
{
}

// Assert the CS line to the FT800
//
extern "C" void _FT8XXEMU_cs(int cs)
{
    while (iEmulatorReady == 0) {                                        // if the emulator has not yet initistaed we wait
        Sleep(10);
    }
    FT8XXEMU_cs(cs);
}

// Send a byte to the emulator
//
extern "C" unsigned char _FT8XXEMU_transfer(unsigned char data)
{
    return FT8XXEMU_transfer(data);
}

static void FT800_emulator_thread(void *hArgs)
{
    FT8XXEMU_EmulatorParameters params;
    FT8XXEMU_EmulatorMode       Ft_GpuEmu_Mode;
    #if defined (FT_800_ENABLE)                                           // select the emulation mode
    Ft_GpuEmu_Mode = FT8XXEMU_EmulatorFT800;
    #elif defined (FT_801_ENABLE)
    Ft_GpuEmu_Mode = FT8XXEMU_EmulatorFT801;
    #elif defined (FT_810_ENABLE)
    Ft_GpuEmu_Mode =  FT8XXEMU_EmulatorFT810;
    #elif defined (FT_811_ENABLE)
    Ft_GpuEmu_Mode = FT8XXEMU_EmulatorFT811;
    #elif defined (FT_812_ENABLE)
    Ft_GpuEmu_Mode = FT8XXEMU_EmulatorFT812;
    #elif defined(FT_813_ENABLE)
    Ft_GpuEmu_Mode = FT8XXEMU_EmulatorFT813;
    #else
    Ft_GpuEmu_Mode = FT8XXEMU_EmulatorFT800;
    #endif

    FT8XXEMU_defaults(FT8XXEMU_VERSION_API, &params, Ft_GpuEmu_Mode);    // get the parameters for the emulation mode
    params.Flags &= (~FT8XXEMU_EmulatorEnableDynamicDegrade & ~FT8XXEMU_EmulatorEnableRegPwmDutyEmulation);
    params.Setup = setup;
    params.Loop = loop;
    FT8XXEMU_run(FT8XXEMU_VERSION_API, &params);                         // start the emulation - this doesn't return
}

static void fnInitFT800_emulator(void)
{
    DWORD ThreadIDRead;
    HANDLE hThreadRead = CreateThread (NULL, 0, (LPTHREAD_START_ROUTINE)FT800_emulator_thread, (LPVOID)0, 0, (LPDWORD)&ThreadIDRead);
}
#endif
