/***********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher
    
    ---------------------------------------------------------------------
    File:      application.c
    Project:   uTasker project
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2016
    *********************************************************************
    16.02.2006 Add SMTP LOGIN support
    18.02.2006 Add SMTP parameter settings
    01.03.2007 Add TFTP demo
    16.03.2007 Improved Global Timer demo
    03.06.2007 Add FTP user controllable FTP timeout                     {1}
    23.06.2007 Add IRQ test                                              {2}
    11.08.2007 Add RTC test (M5223X)                                     {3}
    10.09.2007 Add external RTC IIC test                                 {4}
    15.09.2007 Modify IRQ priorities for M5223X                          {5}
    23.09.2007 Add simple test of SPI FLASH including multiple chips)    {6}
    12.10.2007 Add USB initialisation                                    {7}
    29.10.2007 Add UART RTC control when working in HW flow control mode {8}
    06.11.2007 Add NetBIOS support - start server and specify name       {9}
    20.11.2007 Allow operation from fixed parameters when no parameter system available {10}
    24.12.2007 Add intensive I2C test mode                               {11}
    02.01.2008 Add IRQ test for SAM7X (IRQ and FIQ)                      {12}
    21.01.2008 Add IRQ test for STR91                                    {13}
    18.02.2008 Extend SPI test for external file system                  {14}
    13.03.2008 Correct time server increment on connection close         {15}
    18.03.2008 Correct sizeof() use                                      {16}
    10.04.2008 Add ADC test for M5223X                                   {17}
    14.04.2008 Add optional LCD read test                                {18}
    20.04.2008 Add Coldfire PIT interrupt test                           {19}
    21.04.2008 Add Coldfire DMA TIMER interrupt test                     {20}
    13.05.2008 Add NE64 TIMER interrupt test                             {21}
    13.05.2008 Modified USB initialisation to start USB task             {22}
    02.07.2008 Adjust IRQ test setup for SAM7X                           {23}
    08.08.2008 Add USB demo menu support                                 {24}
    28.08.2008 Correct PORT_INTERRUPT initialisation when DS1307 test defined {25}
    29.10.2008 Add general purpose timer test for M5223X                 {26}
    30.10.2008 Add MODBUS initialisation                                 {27}
    07.12.2008 Modify SAM7X IRQ test to verify IRQ and port interrupts   {28}
    22.12.2008 Use ADC_SAMPLING_SPEED macro                              {29}
    23.12.2008 Adapt ADC test for SAM7X                                  {30}
    21.01.2009 Add fnFilterUserFile() - removed to uFile.c on 07.04.2009 {31}
    05.02.2009 Remove some serial interface code when DEMO_UART not defined {32}
    01.03.2009 Adapt simple timer test                                   {33}
    17.03.2009 IRQ test activated for all processors, with config for LM3Sxxxx {34}
    30.03.2009 Add initialisation of ucSetRTC to avoid GCC using memcpy(){35}
    31.03.2009 Add ADC demo for Luminary                                 {36}
    07.04.2009 Add user file entries                                     {37}
    03.05.2009 Add OLED, GLCD and TFT options                            {38}
    10.05.2009 Add TEST_CAN                                              {39}
    29.05.2009 Add timer priority initialisation                         {40}
    03.06.2009 Add LPC23XX port interrupt test support                   {41}
    04.06.2009 Remove FULL_DUPLEX from LAN default setting               {42}
    10.06.2009 Remove UART DCE setting and rename usConfig to Config in UART configuration {43}
    01.07.2009 Adapt for STRING_OPTIMISATION configuration               {44}
    16.07.2009 Optionally retrieve the MAC address from the LM3Sxxxx user data {45}
    19.07.2009 Optional test code included to reduce file size and aid overview {46}
    06.08.2009 MODBUS USB slave support added                            {47}
    06.08.2009 Add OLED graphical demo                                   {48}
    06.08.2009 Correct missing include of ADC_timers.h                   {49}
    08.08.2009 Add SNTP demo                                             {50}
    09.08.2009 Add further graphic functions                             {51}
    23.11.2009 Add SD-card support                                       {52}
    30.12.2009 Add M522XX NMI handling code                              {53}
    05.02.2010 temp_pars->temp_parameters.ucServers changed to .usServers
    12.02.2010 Move fnInitModbus() into the validated call               {54}
    05.07.2010 Move user file initialisation to be independent of validation phase {55}
    28.09.2010 Add TEST_SENSIRION                                        {56}
    03.02.2011 Renamed fnInitCAN() to fnInitCANInterface()               {57}
    21.05.2011 Add picture frame demo to TWR_K60N512                     {58}
    09.07.2011 Add zero-configuration initialisation                     {59}
    18.07.2011 Add SLCD initialisation and demonstration                 {60}
    30.07.2011 Add RAM test                                              {61}
    04.08.2011 Correct SNTP fraction (us) value                          {62}
    14.08.2011 Synchronise DNS server address to default gateway if it is not controlled independently {63}
    21.08.2011 Adapt RTC test for generic/Gregorian compatibility        {64}
    25.10.2011 Initialise parameters when the parameter version doesn't match {65}
    07.11.2011 remove E_TEST_MODBUS_DELAY when no modbus master functionality {66}
    14.12.2011 Add FTP client settings                                   {67}
    08.01.2012 Add picture frame demo to AVR32_AT32UC3C_EK               {68}
    21.01.2012 Add macros CONFIGURE_SDCARD_DETECT_INPUT() and CHECK_STATE_SDCARD_DETECT() {69}
    05.02.2012 Replace some condition USB_INTERFACE defines with USE_USB_CDC {70}
    04.04.2012 Modify RAM test code to support compilers which put variables on the stack in different orders {71}
    04.04.2012 Add RTC_INITIALISATION when starting RTC                  {72}
    04.04.2012 Add TWR_K53N512 support                                   {73}
    04.02.2012 IPv6 modifications                                        {74}
    16.04.2012 Add fnBridgeEthernetFrame() example                       {75}
    17.04.2012 Add touch sensor input read for Kinetis (moved to KeyScan.c) {76}
    27.05.2012 Adapt network parameters for compatibility with multiple networks {77}
    03.06.2012 Add FTP client IPv6 address                               {78}
    13.08.2012 Add MAC from Kinetis program-once-area                    {79}
    01.12.2012 Add AVR32_UC3_C2_XPLAINED support                         {80}
    16.12.2012 Add VLAN variables and SUPPORT_DYNAMIC_VLAN, SUPPORT_DYNAMIC_VLAN_TX functions {81}
    17.12.2012 Add ALTERNATIVE_VLAN_COUNT support                        {82}
    17.12.2012 Add PHY_POLL_LINK support                                 {83}
    07.04.2013 Rename CHECK_STATE_SDCARD_DETECT() to SDCARD_DETECTION()  {84}
    07.04.2013 Add variable RTC oscillator start-up delay                {85}
    28.06.2013 Add magic frame reset support                             {86}
    08.07.2013 Add ELZET80 device discovery support                      {87}
    02.08.2013 Add fnGetDHCP_host_name()                                 {88}
    23.12.2013 Add fnRestrictGatewayInterface()                          {89}
    17.01.2014 Pass directory pointer to utFAT2.0 version of utOpenFile(){90}
    06.03.2014 Update SNMP configuration                                 {91}
    13.04.2014 Add second network settings and network decision handling {92}
    21.04.2014 Add FRDM-KL46Z and TWR-KL46Z48M time display support      {93}
    04.09.2014 Add user defined behaviour to broadcasts and sub-net broadcasts when multiple networks are available {94}
    06.12.2014 Relocate SLCD time display code to include file slcd_time.h {95}
    07.12.2014 Add FreeMaster run-time debugging support                 {96}
    11.02.2015 RTC, timer server and SNTP operations moved to uTasker\time_keeper.c
    12.02.2015 Automatically validate new setting if it is recognised that the backup set has been lost {97}
    25.10.2015 Add emulated FAT data files and their handling            {98}
    12.12.2015 Modify parameter of fnSetDefaultNetwork()                 {99}

    02.05.2017 Change USE_DHCP to USE_DHCP_CLIENT

*/


/* =================================================================== */
/*                           include files                             */
/* =================================================================== */

#include "config.h"

#define OWN_TASK                  TASK_APPLICATION

#include "application_lcd.h"                                             // {46} LCD tests
#include "ADC_Timers.h"                                                  // {46} ADC and timer tests
#if !defined BLAZE_K22
    #include "iic_tests.h"                                               // {46} iic tests
#endif
#include "Port_Interrupts.h"                                             // {46} port interrupt tests
#include "can_tests.h"                                                   // {46} CAN tests

/* =================================================================== */
/*                          local definitions                          */
/* =================================================================== */

/*****************************************************************************/
// The project includes a variety of quick tests which can be activated here
/*****************************************************************************/

//#define RAM_TEST                                                       // {61} perform a RAM test on startup - if error found, stop
//#define TEST_MSG_MODE                                                  // test UART in message mode
//#define TEST_MSG_CNT_MODE                                              // test UART in message counter mode
#if defined SUPPORT_DISTRIBUTED_NODES
  //#define TEST_DISTRIBUTED_TX                                          // test some uNetwork messages
#endif
#if defined GLOBAL_TIMER_TASK
  //#define TEST_GLOBAL_TIMERS                                           // test global timer operation
#endif
#if defined USE_UDP
  //#define DEMO_UDP                                                     // add UDP echo demo
#endif
#if defined USE_TFTP
  //#define TEST_TFTP                                                    // TFTP test
#endif
#if defined SUPPORT_RTC
  //#define RTC_TEST                                                     // test RTC function
#endif

#define STATE_INIT                0x00                                   // task states
#define STATE_ACTIVE              0x01
#define STATE_DELAYING            0x02
#define STATE_VALIDATING          0x04
#define STATE_BLOCKED             0x08
#define STATE_ESCAPING            0x10
#define STATE_RESTARTING          0x20
#define STATE_TESTING             0x40
#define STATE_POLLING             0x80

#if defined DEMO_UDP
    #define UDP_BUFFER_SIZE        512                                   // buffer size for UDP test message
    #define MY_UDP_PORT            9999                                  // test UDP port
#endif

/* =================================================================== */
/*                      local structure definitions                    */
/* =================================================================== */

#if defined DEMO_UDP
    typedef struct stUDP_MESSAGE
    {
        unsigned short usLength;
        UDP_HEADER     tUDP_Header;                                      // reserve header space
        unsigned char  ucUDP_Message[UDP_BUFFER_SIZE];                   // reserve message space
    } UDP_MESSAGE;
#endif

/* =================================================================== */
/*                 local function prototype declarations               */
/* =================================================================== */

static void fnValidatedInit(void);
#if defined DEMO_UDP
    static void fnConfigUDP(void);
    static int  fnUDPListner(USOCKET c, unsigned char uc, unsigned char *ucIP, unsigned short us, unsigned char *data, unsigned short us2);
#endif
#if defined TEST_GLOBAL_TIMERS
    static void fnStartGlobalTimers(void);
    static void fnHandleGlobalTimers(unsigned char ucTimerEvent);
#endif
#if defined USE_DHCP_CLIENT || defined USE_ZERO_CONFIG
    static void fnConfigureDHCP_ZERO(void);
#endif
#if defined TEST_DISTRIBUTED_TX
    static void fnSendDist(void);
#endif
#if defined USE_SMTP
    static const CHAR *fnEmailTest(unsigned char ucEvent, unsigned short *usData); // e-mail call back handler
    #if defined USE_DNS
        static void fnDNSListner(unsigned char ucEvent, unsigned char *ptrIP);
    #endif
#endif
#if defined TEST_TFTP
    static void tftp_listener(unsigned short usError, CHAR *error_text);
    static void fnTransferTFTP(void);
#endif
#if defined RTC_TEST
    static void fnTestRTC(void);
#endif
#if defined SUPPORT_SLCD && defined SUPPORT_RTC
    static void _rtc_second_interrupt(void);
#endif
#if defined SUPPORT_GLCD && (defined MB785_GLCD_MODE || defined AVR32_EVK1105 || defined AVR32_AT32UC3C_EK || defined IDM_L35_B || defined M52259_TOWER || defined TWR_K60N512 || defined TWR_K60D100M || defined TWR_K70F120M || defined OLIMEX_LPC2478_STK || defined K70F150M_12M || (defined OLIMEX_LPC1766_STK && defined NOKIA_GLCD_MODE)) && defined SDCARD_SUPPORT // {58}{68}
    static void fnDisplayPhoto(int iOpen);
#endif
#if defined FAT_EMULATION                                                // {98}
    static void fnPrepareEmulatedFAT(void);
#endif

/* =================================================================== */
/*                             constants                               */
/* =================================================================== */

// The application is responsible for defining the IP configuration - here are the default settings
//
#if defined ETH_INTERFACE || defined USB_CDC_RNDIS
static const NETWORK_PARAMETERS network_default[IP_NETWORK_COUNT] = {
    {
    #if defined LAN_REPORT_ACTIVITY
        (AUTO_NEGOTIATE /*| FULL_DUPLEX*/ | RX_FLOW_CONTROL),            // {42} usNetworkOptions - see driver.h for other possibilities
    #else
        (AUTO_NEGOTIATE /*| FULL_DUPLEX*/ | RX_FLOW_CONTROL | LAN_LEDS), // {42} usNetworkOptions - see driver.h for other possibilities
    #endif
        {0x00, 0x00, 0x00, 0x00, 0x00, 0x00},                            // ucOurMAC - when no other value can be read from parameters this will be used
        { 192, 168, 0, 3 },                                              // ucOurIP - our default IP address
        { 255, 255, 255, 0 },                                            // ucNetMask - Our default network mask
        { 192, 168, 0, 1 },                                              // ucDefGW - Our default gateway
        { 192, 168, 0, 1 },                                              // ucDNS_server - Our default DNS server
        #if defined USE_IPV6
        { _IP6_ADD_DIGIT(0x2001), _IP6_ADD_DIGIT(0x0470), _IP6_ADD_DIGIT(0x0026), _IP6_ADD_DIGIT(0x0105), _IP6_ADD_DIGIT(0x0000), _IP6_ADD_DIGIT(0x0000), _IP6_ADD_DIGIT(0x0000), _IP6_ADD_DIGIT(0x0030) }, // default global IPV6 address
            #if defined USE_IPV6INV4                                     // {74}
        { 216, 66, 80, 98 },                                             // IPv6 in IPv4 tunneling enabled when the tunnel address is not 0.0.0.0
            #endif
        #endif
    },
    #if (IP_NETWORK_COUNT > 1)                                           // {92} second network IP setting
    {
    #if defined LAN_REPORT_ACTIVITY
        (AUTO_NEGOTIATE /*| FULL_DUPLEX*/ | RX_FLOW_CONTROL),            // {42} usNetworkOptions - see driver.h for other possibilities
    #else
        (AUTO_NEGOTIATE /*| FULL_DUPLEX*/ | RX_FLOW_CONTROL | LAN_LEDS), // {42} usNetworkOptions - see driver.h for other possibilities
    #endif
        {0x00, 0x00, 0x00, 0x00, 0x00, 0x00},                            // ucOurMAC - when no other value can be read from parameters this will be used
        { 192, 168, 0, 4 },                                              // ucOurIP - our default IP address
        { 255, 255, 255, 0 },                                            // ucNetMask - Our default network mask
        { 192, 168, 0, 1 },                                              // ucDefGW - Our default gateway
        { 192, 168, 0, 1 },                                              // ucDNS_server - Our default DNS server
        #if defined USE_IPV6
        { _IP6_ADD_DIGIT(0x2001), _IP6_ADD_DIGIT(0x0470), _IP6_ADD_DIGIT(0x0026), _IP6_ADD_DIGIT(0x0105), _IP6_ADD_DIGIT(0x0000), _IP6_ADD_DIGIT(0x0000), _IP6_ADD_DIGIT(0x0000), _IP6_ADD_DIGIT(0x0031) }, // default global IPV6 address
           #if defined USE_IPV6INV4
        { 216, 66, 80, 98 },                                             // IPv6 in IPv4 tunneling enabled when the tunnel address is not 0.0.0.0
            #endif
        #endif
        },
    #endif
};
#endif

// The default user settings (factory settings)
//
const PARS cParameters = {
    PARAMETER_BLOCK_VERSION,                                             // version number for parameter validity control
    (unsigned short)(2 * 60),                                            // default telnet_timeout - 2 minutes
    (CHAR_8 + NO_PARITY + ONE_STOP + USE_XON_OFF + CHAR_MODE),           // {43} serial interface settings
    23,                                                                  // TELNET port number
    {
        (/*ACTIVE_DHCP + */ACTIVE_LOGIN + ACTIVE_FTP_SERVER /*+ ACTIVE_FTP_LOGIN*/ + ACTIVE_SNTP + ACTIVE_TIME_SERVER + ACTIVE_WEB_SERVER + ACTIVE_TELNET_SERVER + SMTP_LOGIN), // active servers (ACTIVE_DHCP and ACTIVE_FTP_LOGIN disabled)
#if (IP_NETWORK_COUNT > 1)
        (/*ACTIVE_DHCP + */ACTIVE_LOGIN + ACTIVE_FTP_SERVER /*+ ACTIVE_FTP_LOGIN*/ + ACTIVE_SNTP + ACTIVE_TIME_SERVER + ACTIVE_WEB_SERVER + ACTIVE_TELNET_SERVER + SMTP_LOGIN), // active servers (ACTIVE_DHCP and ACTIVE_FTP_LOGIN disabled)
#endif
    },
#if defined FRDM_KL03Z                                                   // this board has a capacitor connected to the LPUART0_RX pin so can not use fast speeds
    SERIAL_BAUD_19200,                                                   // baud rate of serial interface
#else
    SERIAL_BAUD_115200,                                                  // baud rate of serial interface
#endif
    {0, 0, 0, 0},                                                        // trusted dial out IP address (null IP means no checking)
    {'A', 'D', 'M', 'I', 'N', 0, ' ', ' '},                              // default user name - & or null terminator closes sequence
    {'u', 'T', 'a', 's', 'k', 'e', 'r', '&'},                            // default user password - & or null terminator closes sequence
#if defined _M5225X
    {'K', 'I', 'R', 'I', 'N', '3', 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
#elif defined _KINETIS
    {'K', 'I', 'N', 'E', 'T', 'I', 'S', 0,0,0,0,0,0,0,0,0,0,0,0,0,0},
#elif defined _STM32
    {'S', 'T', 'M', '3', '2', 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
#else
    {'u', 'T', 'a', 's', 'k', 'e', 'r', ' ', 'N', 'u', 'm', 'b', 'e', 'r', ' ', '1',0,0,0,0,0},
#endif
    80,                                                                  // flow control at 80% high water
    20,                                                                  // flow control at 20% low water
#if defined _KINETIS || defined AVR32_AT32UC3C_EK || defined AVR32_UC3_C2_XPLAINED // {80}
    (MAPPED_DEMO_LED_1 | MAPPED_DEMO_LED_2),                             // user port DDR value
    (MAPPED_DEMO_LED_1 | MAPPED_DEMO_LED_2),                             // user port value of outputs
#else
    #if defined _LM3SXXXX
    (BLINK_LED),                                                         // user port DDR value
    #else
    (DEMO_LED_1 | DEMO_LED_2),                                           // user port DDR value
    #endif
    (DEMO_LED_1 | DEMO_LED_2),                                           // user port value of outputs
#endif
    0,                                                                   // second set of user defined outputs
#if !defined ETH_INTERFACE
    0,                                                                   // serial number (when Ethernet is enabled it is derived from the MAC address instead)
#endif
#if defined USE_SNTP || defined USE_TIME_SERVER
    (TIME_ZONE_UTC_PLUS_1 | DAYLIGHT_SAVING_WINTER),                     // time zone and daylight saving (UTC + 1)
#endif
#if defined USE_SNTP
    {                                                                    // SNTP server list
        {194, 0, 229, 89},                                               // stratum 1 - ntpstm.netbone-digital.com St. Moritz
		{131, 188, 3, 220},                                              // stratum 1 - ntp0.fau.de University Erlangen-Nuernberg, D-91058 Erlangen, FRG
		{217, 147, 223, 78},                                             // stratum 2 - clock.tix.ch CH-8005 Zurich, Switzerland
		{129, 6, 15, 29},                                                // time-b.nist.gov
    },
#endif
#if defined USE_TIME_SERVER
    {                                                                    // time server list
        {129, 6, 15, 28},                                                // time-a.nist.gov 129.6.15.28 NIST, Gaithersburg, Maryland
        {132, 163, 4, 101},                                              // time-a.timefreq.bldrdoc.gov 132.163.4.101 NIST, Boulder, Colorado
        {216, 200, 93, 8},                                               // nist1-dc.glassey.com 216.200.93.8 Abovenet, Virginia
    },
#endif
#if defined SMTP_PARAMETERS
    {'U', 's', 'e', 'r', ' ', 'n', 'a', 'm', 'e', 0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {'P', 'a', 's', 's', ' ', 'w', 'o', 'r', 'd', 0,0},
    {'M', 'y', 'A', 'd', 'd', 'r', 'e', 's', 's', '@', 'u', 'T', 'a', 's', 'k', 'e', 'r', '.', 'c', 'o', 'm', 0,0,0,0,0,0,0,0,0,0},
    {'m', 'a', 'i', 'l', '.', 'p', 'r', 'o', 'v', 'i', 'd', 'e', 'r', '.', 'c', 'o', 'm', 0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    #if defined USE_DNS
    {0, 0, 0, 0},
    #else
    SMTP_PROVIDER_IP_ADDRESS,
    #endif
#endif
#if defined LCD_CONTRAST_CONTROL
    50,                                                                  // default LCD contrast PWM value (%)
#endif
#if defined GLCD_BACKLIGHT_CONTROL
    95,                                                                  // default LCD backlight PWM intensity (%)
#endif
#if defined SUPPORT_TOUCH_SCREEN
    0, 0, 0, 0,                                                          // default touch screen calibration parameters
#endif
#if defined USE_FTP_CLIENT                                               // {67}
    {"FTP-USER-NAME"},                                                   // default FTP server user name
    {"FTP-PASSWORD"},                                                    // default FTP server user name
    21,                                                                  // FTP port number
    (2 * 60),                                                            // default idle timeout in seconds
    {192, 168, 0, 1},                                                    // default FTP server IPv4 address
    #if defined USE_IPV6                                                 // {78}
    {_IP6_ADD_DIGIT(0xfe80), _IP6_ADD_DIGIT(0x0000), _IP6_ADD_DIGIT(0x0000), _IP6_ADD_DIGIT(0x0000), _IP6_ADD_DIGIT(0x0200), _IP6_ADD_DIGIT(0x00ff), _IP6_ADD_DIGIT(0xfe00), _IP6_ADD_DIGIT(0x0000)}, // default FTP server IPv6 address
  //{_IP6_ADD_DIGIT(0x2001), _IP6_ADD_DIGIT(0x0470), _IP6_ADD_DIGIT(0x0026), _IP6_ADD_DIGIT(0x0105), _IP6_ADD_DIGIT(0), _IP6_ADD_DIGIT(0), _IP6_ADD_DIGIT(0), _IP6_ADD_DIGIT(0x0010)}, // default FTP server IPv6 address
    #endif
#endif
#if defined USE_IPV6INV4 && (defined USE_IPV6INV4_RELAY_DESTINATIONS && (USE_IPV6INV4_RELAY_DESTINATIONS != 0)) // {74} single IPv6in4 relay destination
    {
        {
            {_IP6_ADD_DIGIT(0x2001), _IP6_ADD_DIGIT(0x0470), _IP6_ADD_DIGIT(0x0025), _IP6_ADD_DIGIT(0x0105), _IP6_ADD_DIGIT(0), _IP6_ADD_DIGIT(0), _IP6_ADD_DIGIT(0), _IP6_ADD_DIGIT(0x0002)}, // global IPv6 address of destination
            {192, 168, 0, 99},                                           // ipv4 address of destination
            {0xf0, 0x4d, 0xa2, 0x9d, 0x94, 0xdc}                         // MAC address of distination
        },
    #if USE_IPV6INV4_RELAY_DESTINATIONS > 1
        {
            {_IP6_ADD_DIGIT(0x2001), _IP6_ADD_DIGIT(0x0470), _IP6_ADD_DIGIT(0x0026), _IP6_ADD_DIGIT(0x0105), _IP6_ADD_DIGIT(0), _IP6_ADD_DIGIT(0), _IP6_ADD_DIGIT(0), _IP6_ADD_DIGIT(0x0011)}, // global IPv6 address of destination
            {192, 168, 0, 4},                                            // ipv4 address of destination
            {0x00, 0x00, 0x00, 0x00, 0x00, 0x04}                         // MAC address of distination
        },
    #endif
    }
#endif
};

#if defined SUPPORT_KEY_SCAN                                             // support up to 4 x 4 for test purposes
    static const char *cKey[] = {
      "Key 1 pressed\r\n",                                               // First column, Row 1 press
      "Key 1 released\r\n",                                              //               Row 1 release
      "Key 4 pressed\r\n",                                               //               Row 2 press
      "Key 4 released\r\n",                                              //               Row 2 release
      "Key 7 pressed\r\n",                                               //               Row 3 press
      "Key 7 released\r\n",                                              //               Row 3 release
    #if KEY_ROWS > 3
      "Key * pressed\r\n",                                               //               Row 4 press
      "Key * released\r\n",                                              //               Row 4 release
    #endif

      "Key 2 pressed\r\n",                                               // 2nd column,   Row 1 press
      "Key 2 released\r\n",                                              //               Row 1 release
      "Key 5 pressed\r\n",                                               //               Row 2 press
      "Key 5 released\r\n",                                              //               Row 2 release
      "Key 8 pressed\r\n",                                               //               Row 3 press
      "Key 8 released\r\n",                                              //               Row 3 release
    #if KEY_ROWS > 3
      "Key 0 pressed\r\n",                                               //               Row 4 press
      "Key 0 released\r\n",                                              //               Row 4 release
    #endif

      "Key 3 pressed\r\n",                                               // 3rd column,   Row 1 press
      "Key 3 released\r\n",                                              //               Row 1 release
      "Key 6 pressed\r\n",                                               //               Row 2 press
      "Key 6 released\r\n",                                              //               Row 2 release
      "Key 9 pressed\r\n",                                               //               Row 3 press
      "Key 9 released\r\n",                                              //               Row 3 release
    #if KEY_ROWS > 3
      "Key # pressed\r\n",                                               //               Row 4 press
      "Key # released\r\n",                                              //               Row 4 release
    #endif

      "Key A pressed\r\n",                                               // 4th column, Row 1 press
      "Key A released\r\n",                                              //               Row 1 release
      "Key B pressed\r\n",                                               //               Row 2 press
      "Key B released\r\n",                                              //               Row 2 release
      "Key C pressed\r\n",                                               //               Row 3 press
      "Key C released\r\n",                                              //               Row 3 release
    #if KEY_ROWS > 3
      "Key D pressed\r\n",                                               //               Row 4 press
      "Key D released\r\n"                                               //               Row 4 release
    #endif
    };
#endif

#if defined USE_SMTP
    static const CHAR cUserDomain[]   = OUR_USER_DOMAIN;
    static const CHAR cSubject[]      = EMAIL_SUBJECT;
    static const CHAR cEmailText[]    = EMAIL_CONTENT;
    #if !defined SMTP_PARAMETERS
        static const CHAR cSender[]       = SENDERS_EMAIL_ADDRESS;
        static const CHAR cUserName[]     = SMTP_ACCOUNT_NAME;
        static const CHAR cUserPassword[] = SMTP_PASSWORD;
        #if defined USE_DNS
            const CHAR cSMTP_provider[]   = SMTP_PROVIDER_ADDRESS;       // our smtp provider's server
        #endif
    #endif
#endif

#if defined INTERNAL_USER_FILES                                          // {37}
    #include "app_user_files.h"                                          // include consts as used by the user files
#endif

/* =================================================================== */
/*                     global variable definitions                     */
/* =================================================================== */

TEMPPARS *temp_pars = 0;                                                 // working parameters
#if defined USE_PARAMETER_BLOCK
    PARS *parameters = 0;                                                // back up of original parameters so that they can be restored after unwanted changes
#endif
#if defined ETH_INTERFACE || defined USB_CDC_RNDIS
    NETWORK_PARAMETERS network[IP_NETWORK_COUNT] = {{0}};                // used network values
    NETWORK_PARAMETERS network_flash[IP_NETWORK_COUNT] = {{0}};          // these are the values really in FLASH
#endif
#if defined SERIAL_INTERFACE
    QUEUE_HANDLE SerialPortID = NO_ID_ALLOCATED;                         // serial port handle
#endif
#if defined USE_SMTP
    CHAR cEmailAdd[41] = DEFAULT_DESTINATION_EMAIL_ADDRESS;
#endif
#if defined SUPPORT_VLAN                                                 // {81}
    int vlan_active = 0;                                                 // VLAN initially disabled
    #if defined ALTERNATIVE_VLAN_COUNT && ALTERNATIVE_VLAN_COUNT > 0     // {82}
        unsigned short vlan_vid[1 + ALTERNATIVE_VLAN_COUNT] = {1, 1};    // default VLAN IDs for multiple VLANs
    #else
        unsigned short vlan_vid = 1;                                     // default VLAN ID if enabled
    #endif
#endif


/* =================================================================== */
/*                      local variable definitions                     */
/* =================================================================== */

#if defined TEST_TFTP
    static unsigned char ucTFTP_server_ip[IPV4_LENGTH] = {192, 168, 0, 102};
#endif
#if defined TEST_GPT
    static unsigned long ulCaptureList[GPT_CAPTURES] = {0};              // make space for capture values
#endif
#if defined DEMO_UDP
    static unsigned char ucUDP_IP_Address[IPV4_LENGTH] = {192, 168, 0, 102}; // address to send UDP test frames to
    static USOCKET MyUDP_Socket = -1;
    static UDP_MESSAGE *ptrUDP_Frame = 0;
#endif
#if defined SUPPORT_LCD && !defined USE_TIME_SERVER
    static signed char cShiftTest = LCD_TEST_STARTING;
#endif
#if defined TEST_DISTRIBUTED_TX
    static unsigned long ulLost_uNetFrames = 0;
    static unsigned long ulLost_uNetSync   = 0;
#endif
#if defined USE_SMTP
    #if defined USE_DNS
        unsigned char ucSMTP_server[] = {0,0,0,0};                       // IP address or smtp server after it has been resolved
        static unsigned char ucSMTP_Retry = 0;                           // DNS retry counter
    #else
        unsigned char ucSMTP_server[] = SMTP_PROVIDER_IP_ADDRESS;
    #endif
#endif
#if defined SUPPORT_GLCD && (defined MB785_GLCD_MODE || defined AVR32_EVK1105 || defined AVR32_AT32UC3C_EK || defined IDM_L35_B || defined M52259_TOWER || defined TWR_K60N512 || defined TWR_K60D100M || defined TWR_K70F120M || defined OLIMEX_LPC2478_STK || defined K70F150M_12M || (defined OLIMEX_LPC1766_STK && defined NOKIA_GLCD_MODE)) && defined SDCARD_SUPPORT // {58}{68}
    static UTDIRECTORY *ptr_utDirectory = 0;                             // pointer to a directory object
    static int iPollingMode = 0;
    static UTFILE utFile = {0};
    static UTFILEINFO utFileInfo = {0};
#endif
#if defined FREEMASTER_UART
    static QUEUE_HANDLE FreemasterPortID = NO_ID_ALLOCATED;              // FreeMaster serial port handle
#endif
static QUEUE_HANDLE save_handle = NETWORK_HANDLE;                        // temporary debug handle backup
static int iAppState = STATE_INIT;                                       // task state

// Application task
//
extern void fnApplication(TTASKTABLE *ptrTaskTable)
{
    QUEUE_HANDLE        PortIDInternal = ptrTaskTable->TaskID;           // queue ID for task input
    #if RX_BUFFER_SIZE >= 64
    unsigned char       ucInputMessage[RX_BUFFER_SIZE];                  // reserve space for receiving messages
    #else
    unsigned char       ucInputMessage[64];                              // reserve space for receiving messages (if the UART rx debug size if set less than 64 we use a 64 byte input otherwise it may be too small (and cause buffer overflow when used))
    #endif
    #if (defined SERIAL_INTERFACE && defined DEMO_UART) || (defined CAN_INTERFACE && defined TEST_CAN) || defined TEST_IIC // {32}{39}
    QUEUE_TRANSFER Length = 0;
    #endif

    if (STATE_INIT == iAppState) {
#if defined SUPPORT_SLCD                                                 // {60}
        CONFIGURE_SLCD();                                                // configure ports and start SLCD with blank screen
    #if defined SUPPORT_RTC
        fnStartRTC(_rtc_second_interrupt);                               // start the RTC if it isn't yet operating and enter a seconds callback
    #endif
#else
    #if defined SUPPORT_RTC || defined SUPPORT_SW_RTC
        fnStartRTC(0);                                                   // start the RTC if it isn't yet operating
        #if defined KINETIS_KL && !defined _WINDOWS
        if (RCM_SRS0 & (RCM_SRS0_POR | RCM_SRS0_LVD)) {                  // power on reset/low voltage detector
            fnResetBoard();                                              // temp fix for first alarm that otherwise immediately fires
        }
        #endif
    #endif
#endif
        temp_pars  = (TEMPPARS *)uMalloc(sizeof(TEMPPARS));              // get space for a working set of all modifiable parameters
#if defined USE_PARAMETER_BLOCK
        parameters = (PARS *)uMalloc(sizeof(PARS));                      // get RAM for a local copy of device parameters
#endif
        if (fnGetOurParameters(0) == TEMPORARY_PARAM_SET) {
#if defined USE_PARAMETER_BLOCK && defined USE_PAR_SWAP_BLOCK && (defined ETH_INTERFACE || defined USB_CDC_RNDIS)
            uTaskerMonoTimer(OWN_TASK, (DELAY_LIMIT)(2 * 60 * SEC), E_TIMER_VALIDATE); // we have test parameters and wait for them to be validated else reset
            iAppState = STATE_VALIDATING;
#endif
        }
        else {
            iAppState = STATE_ACTIVE;                                    // not validating so start work
            fnValidatedInit();
        }
#if defined FAT_EMULATION                                                // {98}
        fnPrepareEmulatedFAT();
#endif
#if defined _MAGIC_RESET_FRAME && defined ETH_INTERFACE && defined ETHERNET_AVAILABLE && !defined NO_INTERNAL_ETHERNET // {80}
        fnEnableMagicFrames(1);                                          // {86} enable magic frame checking so that the board can be reset
#endif
#if defined USE_IPV6 && defined USE_IPV6INV4 && (defined USE_IPV6INV4_RELAY_DESTINATIONS && (USE_IPV6INV4_RELAY_DESTINATIONS != 0)) // {74}
        fnSetIPv6in4Destinations(&temp_pars->temp_parameters.relay_destination[0]); // enter ipv6 in ipv4 table for relaying use
#endif
#if defined INTERNAL_USER_FILES                                          // {37}{55}
    #if defined EMBEDDED_USER_FILES
        if (fnActivateEmbeddedUserFiles("1", USER_FILE_IN_INTERNAL_FLASH) == 0) { // if valid embedded user file space is found activate it, else use code embedded version
            fnEnterUserFiles((USER_FILE *)user_files);                   // user_files defined in app_user_files.h
        }
    #else
        fnEnterUserFiles((USER_FILE *)user_files);                       // user_files defined in app_user_files.h
    #endif
#endif
#if defined USE_MAINTENANCE && (!(defined KWIKSTIK && defined SUPPORT_SLCD)) && !defined KL43Z_256_32_CL
        fnInitialisePorts();                                             // set up ports as required by the user
#endif
        uTaskerStateChange(TASK_DEBUG, UTASKER_ACTIVATE);
#if defined SERIAL_INTERFACE && defined DEMO_UART                        // {32} this serial interface is used for debug output and menu based control
        if (NO_ID_ALLOCATED == fnSetNewSerialMode(FOR_I_O)) {            // open serial port for I/O
            return;                                                      // if the serial port could not be opened we quit
        }
        DebugHandle = SerialPortID;                                      // assign our serial interface as debug port
        fnDebugMsg(WELCOME_MESSAGE_UART);
      //fnDebugFloat((float)(12345.123), (WITH_CR_LF | 3));              // test floating point output
#endif
#if defined FREEMASTER_UART
        FreemasterPortID = fnOpenFreeMasterUART();                       // enable UART for FreeMaster use
#endif
#if defined TEST_IIC || defined IIC_SLAVE_MODE || defined TEST_DS1307 || defined TEST_SENSIRION || defined TEST_MMA8451Q || defined TEST_MMA7660F || defined TEST_FXOS8700 // {56}
        fnConfigIIC_Interface();
#endif
#if defined USE_DHCP_CLIENT || defined USE_ZERO_CONFIG
        fnConfigureDHCP_ZERO();
#endif
#if defined USE_HTTP
        fnConfigureAndStartWebServer();
#endif
#if defined USE_FTP
        fnConfigureFtpServer(FTP_TIMEOUT);                               // {1}
#endif
#if defined PHY_POLL_LINK                                                // {83}
        fnInterruptMessage(TASK_NETWORK_INDICATOR, LAN_LINK_DOWN);       // start PHY polling operation
#endif
#if defined _WINDOWS
        fnSimulateLinkUp();                                              // Ethernet link up simulation
#endif
#if defined TEST_GLOBAL_TIMERS
        fnStartGlobalTimers();
#endif
#if defined DEMO_UDP
        fnConfigUDP();                                                   // configure a TEST UDP socket
#endif
#if defined ELZET80_DEVICE_DISCOVERY                                     // {87}
        fnSetupUDPSocket();                                              // configure a UDP listener for discovery purposes
#endif
#if defined USE_NETBIOS
        fnStartNetBIOS_Server(temp_pars->temp_parameters.cDeviceIDName); // {9}
#endif
#if defined TEST_DISTRIBUTED_TX
        OurNetworkNumber = 1;                                            // set a valid network address for ourselves
        fnSendDist();
#endif
#if defined USE_SNMP
        fnInitialiseSNMP();                                              // {91}
        fnSendSNMPTrap(SNMP_COLDSTART, 0, ALL_SNMP_MANAGERS);            // send cold start trap to all possible SNMP managers
#endif
#if defined USE_PPP
        uTaskerStateChange(TASK_PPP, UTASKER_ACTIVATE);                  // start the PPP task
#endif
#if defined USB_INTERFACE
        uTaskerStateChange(TASK_USB, UTASKER_ACTIVATE);                  // start USB task
#endif
#if (defined SDCARD_SUPPORT || defined SPI_FLASH_FAT || defined FLASH_FAT) && !defined MB785_GLCD_MODE
        uTaskerStateChange(TASK_MASS_STORAGE, UTASKER_ACTIVATE);         // {52} start mass storage task
#endif
#if defined IRQ_TEST || defined WAKEUP_TEST
        fnInitIRQ();
#endif
#if defined RTC_TEST                                                     // {3}
        fnTestRTC();
#endif
#define _ADC_TIMER_INIT
    #include "ADC_Timers.h"                                              // ADC and timer initialisation
#undef  _ADC_TIMER_INIT

#if defined CAN_INTERFACE && defined TEST_CAN                            // {39}
        fnInitCANInterface();                                            // {57}
#endif
#if defined nRF24L01_INTERFACE
        fnInit_nRF24L01();
#endif
    }
#if defined SUPPORT_GLCD && (defined MB785_GLCD_MODE || defined AVR32_EVK1105 || defined AVR32_AT32UC3C_EK || defined IDM_L35_B || defined M52259_TOWER || defined TWR_K60N512 || defined TWR_K60D100M || defined TWR_K70F120M || defined OLIMEX_LPC2478_STK || defined K70F150M_12M || (defined OLIMEX_LPC1766_STK && defined NOKIA_GLCD_MODE)) && defined SDCARD_SUPPORT // {58}{68}
    else {
        if (iPollingMode != 0) {
            fnDisplayPhoto(0);                                           // polling operation
        }
    }
#endif

    while (fnRead(PortIDInternal, ucInputMessage, HEADER_LENGTH)) {      // check input queue
        switch (ucInputMessage[MSG_SOURCE_TASK]) {                       // switch depending on message source
        case TIMER_EVENT:
            switch (ucInputMessage[MSG_TIMER_EVENT]) {
            case E_TIMER_SW_DELAYED_RESET:
                fnResetBoard();                                          // delayed reset to allow rest page to be served
                break;
#if defined USE_PARAMETER_BLOCK && defined USE_PAR_SWAP_BLOCK && (defined ETH_INTERFACE || defined USB_CDC_RNDIS)
            case E_TIMER_VALIDATE:
                fnDelPar(INVALIDATE_TEST_PARAMETER_BLOCK);               // validation timer fired before new parameters were verified. We delete the temporary parameters and restart with the original or defaults
                fnResetBoard();
                break;
#endif
#if defined SERIAL_INTERFACE
            case E_QUIT_SERIAL_COMMAND_MODE:
                {
                    static const CHAR ucCOMMAND_MODE_TIMEOUT[] = "Connection timed out\r\n";
                    fnWrite(SerialPortID, (unsigned char *)ucCOMMAND_MODE_TIMEOUT, (sizeof(ucCOMMAND_MODE_TIMEOUT) - 1));
                }
                break;
#endif
#if defined TEST_SENSIRION                                               // {56}
            case E_NEXT_SENSOR_REQUEST:
                fnNextSensorRequest();
                break;
#endif
#if defined nRF24L01_INTERFACE
            case E_nRF24L01_PERIOD:
                {
                    // Periodically send a message so that the receiver can respond if needed
                    //
                    static int iPingPong = 0;
                    if (++iPingPong >= 3) {
                        iPingPong = 0;
                    }
                    fnDebugMsg("Ping RF\r\n");
                    fnTest_nRF24L01_Write(iPingPong);
                    uTaskerMonoTimer(OWN_TASK, (DELAY_LIMIT)(5 * SEC), E_nRF24L01_PERIOD); // next period
                }
                break;
#endif
#if /*defined MODBUS_DELAYED_RESPONSE &&*/ defined USE_MODBUS && defined USE_MODBUS_MASTER // {66}
            case E_TEST_MODBUS_DELAY:
                {
                    extern void fnNextTest(void);
                    fnNextTest();
                  //fnMODBUS_delayed_response(usDelayedReference); // test delayed response
                }
                break;
#endif
#if defined SUPPORT_DELAY_WEB_SERVING
            case E_SERVE_PAGE:
                fnServeDelayed('7', 0);
                break;
#endif
#if defined SUPPORT_LCD || defined SUPPORT_GLCD || defined SUPPORT_OLED  // {38}{48}
    #define HANDLE_TIMERS                                                // messages from the LCD task are handled here - see the file application_lcd.h
    #include "application_lcd.h"                                         // include timer handling from LCD task
    #undef HANDLE_TIMERS
#endif
#define _ADC_TIMER_TIMER_EVENTS                                          // {49}
    #include "ADC_Timers.h"                                              // include timer handling by ADC demo
#undef _ADC_TIMER_TIMER_EVENTS
            default:
#if defined TEST_GLOBAL_TIMERS                                           // assume unhandled timer events belong to global timers
                fnHandleGlobalTimers(ucInputMessage[MSG_TIMER_EVENT]);
#endif
                break;
            }
            break;

        case INTERRUPT_EVENT:
            switch (ucInputMessage[MSG_INTERRUPT_EVENT]) {
            case TX_FREE:
                if (iAppState == STATE_BLOCKED) {                        // the TCP buffer we were waiting for has become free
                    iAppState = STATE_ACTIVE;
                }
                break;
#if defined SUPPORT_RTC && (ALARM_TASK == OWN_TASK)
            case RTC_ALARM_INTERRUPT_EVENT:
    #if defined SUPPORT_LOW_POWER
                fnSetLowPowerMode(WAIT_MODE);
    #endif
                fnDebugMsg("RTC Alarm fired\r\n");
                break;
#endif
#if defined nRF24L01_INTERFACE
            case E_nRF24L01_EVENT:
                fnHandle_nRF24L01_event();
                break;
#endif
#define _CAN_INT_EVENTS
    #include "can_tests.h"                                               // CAN interrupt event handling - specific
#undef _CAN_INT_EVENTS

#if defined USE_DHCP_CLIENT
            case DHCP_SUCCESSFUL:                                        // we can now use the network connection
                fnDebugMsg("DHCP successful: ");
    #if defined USE_MAINTENANCE && (defined USE_TELNET || defined SERIAL_INTERFACE || defined USE_USB_CDC)
                fnDisplayIP(network[DEFAULT_NETWORK].ucOurIP);
    #endif
                fnDebugMsg("\r\n");
    #if defined TEST_TFTP
                fnTransferTFTP();
    #endif
                break;

            case DHCP_MISSING_SERVER:
                fnStopDHCP(DEFAULT_NETWORK);                             // DHCP server is missing so stop and continue with backup address (if available)
    #if defined USE_ZERO_CONFIG                                          // {59}
                fnStartZeroConfig(OWN_TASK);                             // start zero config process as fall-back
    #endif
                break;
#endif
#if defined USE_ZERO_CONFIG                                              // {59}
            case ZERO_CONFIG_SUCCESSFUL:
                fnDebugMsg("Zero config successful\r\n");
                break;
            case ZERO_CONFIG_DEFENDED:
                fnDebugMsg("Zero config defended\r\n");
                break;
            case ZERO_CONFIG_COLLISION:
                fnDebugMsg("Zero config collision\r\n");
                break;
#endif
#if defined TEST_DISTRIBUTED_TX
            case UNETWORK_FRAME_LOSS:
                ulLost_uNetFrames++;
                break;
            case UNETWORK_SYNC_LOSS:
                ulLost_uNetSync++;
                break;
#endif
#define _ADC_TIMER_INT_EVENTS_1
    #include "ADC_Timers.h"                                              // ADC and timer interrupt event handling - specific
#undef _ADC_TIMER_INT_EVENTS_1
            default:
#if defined SUPPORT_KEY_SCAN
                if ((KEY_EVENT_COL_1_ROW_1_PRESSED <= ucInputMessage[MSG_INTERRUPT_EVENT]) && (KEY_EVENT_COL_4_ROW_4_RELEASED >= ucInputMessage[MSG_INTERRUPT_EVENT])) {
                    fnDebugMsg((char *)cKey[ucInputMessage[MSG_INTERRUPT_EVENT] - KEY_EVENT_COL_1_ROW_1_PRESSED]); // key press or release
    #if defined GLCD_MENU_TEST
                    fnHandleKey(&Menu, (ucInputMessage[MSG_INTERRUPT_EVENT] - KEY_EVENT_COL_1_ROW_1_PRESSED));
    #endif
                    break;
                }
#endif
#define _ADC_TIMER_INT_EVENTS_2
    #include "ADC_Timers.h"                                              // ADC and timer interrupt event handling - ranges
#undef _ADC_TIMER_INT_EVENTS_2
#define _PORT_INTS_EVENTS
    #include "Port_Interrupts.h"                                         // port interrupt timer interrupt event handling - ranges
#undef _PORT_INTS_EVENTS
                break;
            }
            break;

#if defined SUPPORT_LCD || defined SUPPORT_GLCD || defined SUPPORT_OLED  // {38}{48}
    #define HANDLE_LCD_MESSAGES                                          // messages from the LCD task are handled here - see the file application_lcd.h
    #include "application_lcd.h"                                         // include message handling from LCD task
    #undef HANDLE_LCD_MESSAGES
#endif

#if defined USE_UDP && defined DEMO_UDP                                  // {50} ARP will only need to resolve if we initiate sending - here we resent the test frame after the destination has been resolved
        case TASK_ARP:
            fnRead(PortIDInternal, ucInputMessage, ucInputMessage[MSG_CONTENT_LENGTH]); // read the message content
            switch (ucInputMessage[0]) {                                 // ARP sends us either ARP resolution success or failed
            case ARP_RESOLUTION_SUCCESS:                                 // IP address has been resolved (repeat UDP frame).
                fnSendUDP(MyUDP_Socket, ucUDP_IP_Address, MY_UDP_PORT, (unsigned char*)&ptrUDP_Frame->tUDP_Header, UDP_BUFFER_SIZE, OWN_TASK);
                break;

            case ARP_RESOLUTION_FAILED:                                  // IP address could not be resolved...
                break;
            }
            break;
#endif
        default:
            fnRead(PortIDInternal, ucInputMessage, ucInputMessage[MSG_CONTENT_LENGTH]); // flush any unexpected messages (assuming they arrived from another task)
            break;
        }
    }

#if defined FREEMASTER_UART
    while ((Length = fnRead(FreemasterPortID, ucInputMessage, sizeof(ucInputMessage))) != 0) { // check for inpt on the FreeMaster UART
        fnHandleFreeMaster(FreemasterPortID, ucInputMessage, Length);    // uTasker handler (interface independent)
    }
#endif

#if defined SERIAL_INTERFACE && defined DEMO_UART                        // {32}
    #if defined TEST_MSG_MODE
        #if defined TEST_MSG_CNT_MODE
    while (fnMsgs(SerialPortID) != 0) {
        unsigned char ucLength;
        fnRead(SerialPortID, &ucLength, 1);                              // get message length
        Length = fnRead(SerialPortID, ucInputMessage, ucLength);
        fnEchoInput(ucInputMessage, ucLength);
    }
        #else
    while (fnMsgs(SerialPortID)) {
        Length = fnRead(SerialPortID, ucInputMessage, MEDIUM_MESSAGE);
        fnEchoInput(ucInputMessage, Length);
    }
        #endif
    #else
    if ((iAppState & (STATE_ACTIVE | STATE_DELAYING | STATE_ESCAPING | STATE_RESTARTING | STATE_VALIDATING)) && ((Length = fnMsgs(SerialPortID)) != 0)) {
        while ((Length = fnRead(SerialPortID, ucInputMessage, MEDIUM_MESSAGE)) != 0) { // handle UART input
        #if defined USE_USB_CDC || defined USB_CDC_HOST                  // {24}{70}
            if (usUSB_state & ES_USB_RS232_MODE) {                       // if in USB-CDC mode
                fnSendToUSB(ucInputMessage, Length);                     // send input to USB interface
                continue;
            }
        #endif
        #if defined USE_FTP_CLIENT && defined USE_MAINTENANCE            // {67}
            if ((iFTP_data_state & (FTP_DATA_STATE_GETTING | FTP_DATA_STATE_PUTTING)) == 0) {
                fnEchoInput(ucInputMessage, Length);
            }
        #else 
            fnEchoInput(ucInputMessage, Length);
        #endif
        #if defined USE_MAINTENANCE
            if (usData_state == ES_NO_CONNECTION) {
                if (fnCommandInput(ucInputMessage, Length, SOURCE_SERIAL)) {
                    if (fnInitiateLogin(ES_SERIAL_LOGIN) == TELNET_ON_LINE) {
                        static const CHAR ucCOMMAND_MODE_BLOCKED[] = "Command line blocked\r\n";
                        fnWrite(SerialPortID, (unsigned char *)ucCOMMAND_MODE_BLOCKED, sizeof(ucCOMMAND_MODE_BLOCKED));
                    }
                }
            }
            else {
                fnCommandInput(ucInputMessage, Length, SOURCE_SERIAL);
            }
        #endif
        }
    }
    #endif
#endif

#define _IIC_READ_CODE
    #if !defined BLAZE_K22 && !defined KL43Z_256_32_CL
    #include "iic_tests.h"                                               // include IIC code to handle reception
    #endif
#undef _IIC_READ_CODE
#define _PORT_NMI_CHECK                                                  // {53}
    #include "Port_Interrupts.h"                                         // port interrupt timer interrupt event handling - ranges
#undef _PORT_NMI_CHECK
}

#if defined USE_DHCP_CLIENT || defined USE_ZERO_CONFIG
static void fnConfigureDHCP_ZERO(void)
{
    int iNetwork = 0;
    while (iNetwork < IP_NETWORK_COUNT) {
    #if defined USE_DHCP_CLIENT
        if ((temp_pars->temp_parameters.usServers[iNetwork] & ACTIVE_DHCP) != 0) {
        #if defined USB_CDC_RNDIS && (IP_NETWORK_COUNT > 1)
            if (iNetwork == SECOND_NETWORK) {
                fnStartDHCP((UTASK_TASK)(FORCE_INIT | OWN_TASK), (RNDIS_INTERFACE | defineNetwork(iNetwork))); // activate DHCP restricted to the RNDIS interface
            }
            else {
                fnStartDHCP((UTASK_TASK)(FORCE_INIT | OWN_TASK), defineNetwork(iNetwork)); // activate DHCP on its network
            }
        #else
            fnStartDHCP((UTASK_TASK)(FORCE_INIT | OWN_TASK), 0);         // activate DHCP
        #endif
        }
        #if defined USE_ZERO_CONFIG                                      // {59}
        else {                                                           // if DHCP
            if ((uMemcmp(network[iNetwork].ucOurIP, cucNullMACIP, IPV4_LENGTH) == 0) || ((network[iNetwork].ucOurIP[0] == 169) && (network[iNetwork].ucOurIP[1] == 254))) { // IP address is set to zero or in link-local range
                fnStartZeroConfig(OWN_TASK);                             // start zero config process
            }
        }
        #endif
    #elif defined USE_ZERO_CONFIG
        if ((uMemcmp(network[iNetwork].ucOurIP, cucNullMACIP, IPV4_LENGTH) == 0) || ((network[iNetwork].ucOurIP[0] == 169) && (network[iNetwork].ucOurIP[1] == 254))) { // IP address is set to zero or in link-local range
            fnStartZeroConfig(OWN_TASK);                                 // start zero config process
        }
    #endif
        iNetwork++;
    }
}
#endif

#if defined ETH_INTERFACE || defined USB_CDC_RNDIS
extern void fnSetDefaultNetwork(void)                                    // {99}
{
    uMemcpy(&network[0], &network_default, sizeof(network_default));     // copy the default network values to the working set
}
#endif

#if defined USE_PARAMETER_BLOCK
extern void fnRestoreFactory(void)
{
    #if defined ETH_INTERFACE || defined USB_CDC_RNDIS
    uMemcpy(temp_pars->temp_network, &network_default, sizeof(network_default));
    uMemcpy(&temp_pars->temp_network[DEFAULT_NETWORK].ucOurMAC[0], &network[DEFAULT_NETWORK].ucOurMAC[0], MAC_LENGTH); // return our mac since we never want to lose it
        #if IP_NETWORK_COUNT > 1
    uMemcpy(&temp_pars->temp_network[SECOND_NETWORK].ucOurMAC[0], &network[SECOND_NETWORK].ucOurMAC[0], MAC_LENGTH); // return our mac since we never want to lose it
        #endif
    #endif
    uMemcpy(&temp_pars->temp_parameters, (unsigned char *)&cParameters, sizeof(PARS)); // copy default parameters to the working set
}
#endif

#if defined SERIAL_INTERFACE
extern void fnFlushSerialRx(void)
{
    #if defined SUPPORT_FLUSH
    fnFlush(SerialPortID, FLUSH_RX);
    #endif
    iAppState = STATE_ACTIVE;
}
#endif

#if defined USE_DHCP_CLIENT && defined DHCP_HOST_NAME                    // {88}
// Supply a DHCP host name
//
extern CHAR *fnGetDHCP_host_name(unsigned char *ptr_ucHostNameLength, int iNetwork)
{
    *ptr_ucHostNameLength = uStrlen(temp_pars->temp_parameters.cDeviceIDName); // length of the name
    return (temp_pars->temp_parameters.cDeviceIDName);                   // return pointer to the name to use
}
#endif

#if defined USE_MAINTENANCE && (defined USE_TELNET || defined SERIAL_INTERFACE || defined USE_USB_CDC)
extern void fnGotoNextState(unsigned short usNextState)
{
    switch (usData_state = usNextState) {
    case ES_SERIAL_COMMAND_MODE:                                         // when we move to serial command state we start an activity timer of fixed 5 minutes so that the link times out if the user forgets to close it using the quit command
        uTaskerMonoTimer(OWN_TASK, (DELAY_LIMIT)(5 * 60 * SEC), E_QUIT_SERIAL_COMMAND_MODE);
        break;

    #if defined SERIAL_INTERFACE
    case ES_SERIAL_LOGIN:                                                // when we move to serial command state we start an activity timer of fixed 1 minute so that the link times out if the user forgets to continue
        uTaskerMonoTimer(OWN_TASK, (DELAY_LIMIT)(1 * 60 * SEC), E_QUIT_SERIAL_COMMAND_MODE);
        DebugHandle = SerialPortID;                                      // we ensure that the debug handle is pointing to the serial interface
        break;
    #endif

    case ES_STARTING_COMMAND_MODE:
    case ES_BINARY_DATA_MODE:
    case ES_DATA_MODE:
    case ES_NO_CONNECTION:
        uTaskerStopTimer(OWN_TASK);
        break;
    }
}
#endif

#if defined ELZET80_DEVICE_DISCOVERY                                     // {87}
// Insert a serial number
//
extern void fnAddDiscoverySerialNumber(CHAR *ptrBuffer, int iMaxLength)
{
    uMemset(ptrBuffer, 0x00, iMaxLength);                                // set serial number 0
}
#endif

#if defined USE_PARAMETER_BLOCK
    #if ((defined ETH_INTERFACE || defined USB_CDC_RNDIS) && !defined NO_INTERNAL_ETHERNET && defined _LM3SXXXX && defined MAC_FROM_USER_REG) // {45}
static void fnGetUserMAC(void)
{
    if ((USER_REG0 != 0xffffffff) && (USER_REG1 != 0xffffffff)) {        // set the MAC address from the LM user registers if not empty
        network[DEFAULT_NETWORK].ucOurMAC[0] = (unsigned char)(USER_REG0); // collect the MAC address as saved by the LP Flasher
        network[DEFAULT_NETWORK].ucOurMAC[1] = (unsigned char)(USER_REG0 >> 8);
        network[DEFAULT_NETWORK].ucOurMAC[2] = (unsigned char)(USER_REG0 >> 16);
        network[DEFAULT_NETWORK].ucOurMAC[3] = (unsigned char)(USER_REG0 >> 24);
        network[DEFAULT_NETWORK].ucOurMAC[4] = (unsigned char)(USER_REG1 >> 8);
        network[DEFAULT_NETWORK].ucOurMAC[5] = (unsigned char)(USER_REG1 >> 16);
    }
}
    #elif ((defined ETH_INTERFACE || defined USB_CDC_RNDIS) && !defined NO_INTERNAL_ETHERNET && defined _KINETIS && defined SUPPORT_PROGRAM_ONCE && defined MAC_FROM_USER_REG) // {79}
static void fnGetUserMAC(void)
{
        unsigned long ulTestBuffer[2];                                   // the MAC address is saved in two long words
        fnProgramOnce(PROGRAM_ONCE_READ, ulTestBuffer, 0, 2);            // read 2 long words from the start of the program-once area
        if (uMemcmp(ulTestBuffer, cucBroadcast, MAC_LENGTH) != 0) {      // if programmed
            uMemcpy(network[DEFAULT_NETWORK].ucOurMAC, ulTestBuffer, MAC_LENGTH); // use the stored value as MAC address
        }
}
    #endif
#endif

#if defined ETH_INTERFACE || defined USB_CDC_RNDIS
extern void fnGetEthernetPars(void)
{
    #if defined USE_PARAMETER_BLOCK
    // First check whether there are temporary values to be tested. If not try to take valid parameters.
    // If there are no parameters the default values will be used
    //
    if (fnGetPar((PAR_NETWORK | TEMPORARY_PARAM_SET), (unsigned char *)&network[0], sizeof(network)) < 0) { // load temporary values (if available)
        if (fnGetPar(PAR_NETWORK, (unsigned char *)&network[0], sizeof(network)) < 0) { // temporary parameters are not available so load active ones (if available)
            fnSetDefaultNetwork();                                       // if no parameters are available, load the default set
        }
    }
        #if (defined ETH_INTERFACE || defined USB_CDC_RNDIS) && !defined NO_INTERNAL_ETHERNET && defined MAC_FROM_USER_REG && (defined _LM3SXXXX || (defined _KINETIS && defined SUPPORT_PROGRAM_ONCE)) // {45}{79}
    fnGetUserMAC();
        #endif
    #else
    fnSetDefaultNetwork();                                               // no parameters block available, load the default set
    #endif
    #if (IP_NETWORK_COUNT > 1) && defined NETWORK_SHARE_MAC              // {92}
    uMemcpy(network[SECOND_NETWORK].ucOurMAC, network[DEFAULT_NETWORK].ucOurMAC, sizeof(network[SECOND_NETWORK].ucOurMAC)); // use the default network's MAC address for both networks
    #endif
    #if defined REMOTE_SIMULATION_INTERFACE
    {
        unsigned char ucSimAddr[] = {192, 168, 0, 98};
        unsigned char ucSimMACAddr[] = {0x00, 0x00, 0x11, 0x22, 0x33, 0x44};
        uMemcpy(network[DEFAULT_NETWORK].ucOurIP, ucSimAddr,  sizeof(network[DEFAULT_NETWORK].ucOurIP));
        uMemcpy(network[DEFAULT_NETWORK].ucOurMAC, ucSimMACAddr,  sizeof(network[DEFAULT_NETWORK].ucOurMAC));
    }
    #endif
}
#endif

#if defined USE_PARAMETER_BLOCK

static unsigned short fnGetOurParameters_1(void)
{
    unsigned short usTemp = TEMPORARY_PARAM_SET;

    // Get our variables from the parameter FLASH:
    // - if there are test variables, use them and inform that we are doing so
    // - if there are only valid variables uses these
    // - if there are no parameters continue using the defaults
    // - note that the network options and the MAC address have already been set by the Ethernet task
    //
    if (fnGetPar((PAR_DEVICE | TEMPORARY_PARAM_SET), (unsigned char *)&temp_pars->temp_parameters, sizeof(temp_pars->temp_parameters)) < 0) { // try to load working parameters from temporary parameters
        if (fnGetPar(PAR_DEVICE, (unsigned char *)&temp_pars->temp_parameters, sizeof(temp_pars->temp_parameters)) < 0) { // else try to load for vaid parameters
            uMemcpy(&temp_pars->temp_parameters, (unsigned char *)&cParameters, sizeof(PARS)); // no valid parameters available - set defaults
    #if defined ETH_INTERFACE || defined USB_CDC_RNDIS
            if ((network[DEFAULT_NETWORK].usNetworkOptions & NETWORK_VALUES_FIXED) == 0) {
                fnSetDefaultNetwork();                                   // if no parameters are available, load the default set
        #if defined MAC_FROM_USER_REG && !defined NO_INTERNAL_ETHERNET && (defined _LM3SXXXX || (defined _KINETIS && defined SUPPORT_PROGRAM_ONCE)) // {45}{79}
                fnGetUserMAC();                                          // load the MAC address form another location
        #endif
            }
        #if defined REMOTE_SIMULATION_INTERFACE
            {
                unsigned char ucSimAddr[] = {192, 168, 0, 98};
                unsigned char ucSimMACAddr[] = {0x00, 0x00, 0x11, 0x22, 0x33, 0x44};
                uMemcpy(network[DEFAULT_NETWORK].ucOurIP, ucSimAddr,  sizeof(network[DEFAULT_NETWORK].ucOurIP));
                uMemcpy(network[DEFAULT_NETWORK].ucOurMAC, ucSimMACAddr,  sizeof(network[DEFAULT_NETWORK].ucOurMAC));
            }
        #endif
    #endif
            return DEFAULT_PARAM_SET;                                    // {97} default parameters were loaded
        }
        usTemp = 0;                                                      // continue using valid set of parameters
    }
    #if defined ETH_INTERFACE || defined USB_CDC_RNDIS
    if ((network[DEFAULT_NETWORK].usNetworkOptions & NETWORK_VALUES_FIXED) == 0) { // if the network values have not been set and fixed
        fnGetPar((unsigned short)(PAR_NETWORK | usTemp), (unsigned char *)&network[DEFAULT_NETWORK], sizeof(network));  // network parameters
        #if defined MAC_FROM_USER_REG && !defined NO_INTERNAL_ETHERNET && (defined _LM3SXXXX || (defined _KINETIS && defined SUPPORT_PROGRAM_ONCE)) // {45}{79}
        fnGetUserMAC();                                                  // load the MAC address from special location
        #endif
    }
        #if defined REMOTE_SIMULATION_INTERFACE
    {
        unsigned char ucSimAddr[] = {192, 168, 0, 98};
        unsigned char ucSimMACAddr[] = {0x00, 0x00, 0x11, 0x22, 0x33, 0x44};
        uMemcpy(network[DEFAULT_NETWORK].ucOurIP, ucSimAddr,  sizeof(network[DEFAULT_NETWORK].ucOurIP));
        uMemcpy(network[DEFAULT_NETWORK].ucOurMAC, ucSimMACAddr,  sizeof(network[DEFAULT_NETWORK].ucOurMAC));
    }
        #endif
    #endif
    return usTemp;                                                       // return indicates whether a temporary or a valid set of parameters is being usd
}
#endif

extern unsigned short fnGetOurParameters(int iCase)
{
#if defined USE_PARAMETER_BLOCK
    unsigned short usTemp;
    #if defined ETH_INTERFACE || defined USB_CDC_RNDIS
    if (iCase == 1) {
        int iNetwork = 0;
        NETWORK_PARAMETERS network_back[IP_NETWORK_COUNT];               // backup of possibly DHCP modified values
        uMemcpy(&network_back, &network[DEFAULT_NETWORK], sizeof(network)); // backup the working network parameters
        usTemp = fnGetOurParameters_1();                                 // get the original set from FLASH
        usTemp &= ~(DEFAULT_PARAM_SET);                                  // {97}
        uMemcpy(&temp_pars->temp_network, &network[DEFAULT_NETWORK], sizeof(network)); // make a backup copy of all parameters for modification
        while (iNetwork < IP_NETWORK_COUNT) {
            if ((parameters->usServers[iNetwork] & ACTIVE_DHCP) != 0) {  // if DHCP operation is defined
                uMemcpy(network[iNetwork].ucDefGW, network_back[iNetwork].ucDefGW, IPV4_LENGTH); // {16} correct sizeof(IPV4_LENGTH) to IPV4_LENGTH
                uMemcpy(network[iNetwork].ucNetMask, network_back[iNetwork].ucNetMask, IPV4_LENGTH);
                uMemcpy(network[iNetwork].ucOurIP, network_back[iNetwork].ucOurIP, IPV4_LENGTH);
            }
            iNetwork++;
        }
    }
    else {
    #endif
    #if defined ETH_INTERFACE || defined USB_CDC_RNDIS
        usTemp = fnGetOurParameters_1();
        #if !defined DNS_SERVER_OWN_ADDRESS                                  // {63}
        uMemcpy(network[DEFAULT_NETWORK].ucDNS_server, network[DEFAULT_NETWORK].ucDefGW, IPV4_LENGTH); // DNS server address follows default gateway address
        #endif
        uMemcpy(&temp_pars->temp_network, &network[DEFAULT_NETWORK], sizeof(network)); // make a backup copy of all parameters for modification
    }
    uMemcpy(&network_flash, &temp_pars->temp_network, sizeof(NETWORK_PARAMETERS));
    #else
        usTemp = fnGetOurParameters_1();
    #endif
    if (temp_pars->temp_parameters.ucParVersion != PARAMETER_BLOCK_VERSION) { // either we have found parameters belonging to another project or else a new version. Take the defaults in this case.
        uMemcpy(&temp_pars->temp_parameters, &cParameters, sizeof(PARS));
        usTemp = 0;
    }
    #if defined USE_PARAMETER_BLOCK
    uMemcpy(parameters, &temp_pars->temp_parameters, sizeof(PARS));      // make a backup of the working parameter set so that changes can be checked for and reverted if needed
    if (TEMPORARY_PARAM_SET == usTemp) {                                 // {97} if temporary parameters are being used it means that we are validating
        unsigned char ucCheck;
        if (fnGetPar((unsigned short)(PAR_DEVICE), &ucCheck, 1) < 0) {   // we try to read one valid parameter to prove that the backup is intact
            // There is no backup (valid set) - this could happen if the new set was being validated and the original was deleted but there was a reset or power failure during the process
            // which leaves the new set marked as temporary but the old set has been erased. If the validation period now times out will will cause also the temporary set to be deleted
            // resulting in the default parameters being returned. This could lose important settings such as the MAC address so must be avoided.
            // Since there is no backup the temporary set is now automatically validated
            //
            fnDelPar(SWAP_PARAMETER_BLOCK);                              // validate the present parameters
            return 0;
        }
    }
    else if (usTemp == DEFAULT_PARAM_SET) {                              // {97}
        fnSaveNewPars(SAVE_NEW_PARAMETERS);                              // copy the default set to flash so that we have a starting set for validation usage
    }
    return usTemp;
    #else
    return 0;
    #endif
#else
    uMemcpy(&temp_pars->temp_parameters, &cParameters, sizeof(PARS));    // {10}
  //uMemcpy(parameters, &cParameters, sizeof(PARS));
    #if defined ETH_INTERFACE || defined USB_CDC_RNDIS
    uMemcpy(&temp_pars->temp_network, &network[DEFAULT_NETWORK], sizeof(temp_pars->temp_network));
    #endif
    return 0;
#endif
}

#if defined SUPPORT_GLCD && (defined MB785_GLCD_MODE || defined AVR32_EVK1105 || defined AVR32_AT32UC3C_EK || defined IDM_L35_B || defined M52259_TOWER || defined TWR_K60N512 || defined TWR_K60D100M || defined TWR_K70F120M || defined OLIMEX_LPC2478_STK || defined K70F150M_12M || (defined OLIMEX_LPC1766_STK && defined NOKIA_GLCD_MODE)) && defined SDCARD_SUPPORT // {58}{68}
static void fnDisplayPhoto(int iOpen)
{
    unsigned char ucTemp[512];                                           // load size equal to SD card sector for best operation
    if (iOpen != 0) {
      //utFile.ptr_utDirObject = ptr_utDirectory;                        // set in utOpenFile() from utFAT2.0
        utOpenFile(utFileInfo.cFileName, &utFile, ptr_utDirectory, (UTFAT_OPEN_FOR_READ)); // {90} open the file for reading
        uTaskerStateChange(OWN_TASK, UTASKER_GO);                        // go to polling mode
        iPollingMode = 1;
    }
    utReadFile(&utFile, ucTemp, sizeof(ucTemp));                         // read a sector
    fnDisplayBitmap(ucTemp, utFile.usLastReadWriteLength);               // display the content
    if (utFile.usLastReadWriteLength < sizeof(ucTemp)) {
        uTaskerStateChange(OWN_TASK, UTASKER_STOP);                      // exit polling mode
        iPollingMode = 0;
    }
}
#endif


// These initialisations are only performed when validated, either at startup or on validation
//
static void fnValidatedInit(void)
{
#if defined USE_TIME_SERVER                                              // do not initiate when validating
    fnStartTimeServer((DELAY_LIMIT)(5.0 * SEC));                         // if timer server is enabled the first synchronisation request is made after a delay
#endif
#if defined USE_SNTP
    fnStartSNTP((DELAY_LIMIT)(5.0 * SEC));                               // if SNTP is enabled the first synchronisation request is made after a delay
#endif
#if defined SUPPORT_LCD || (defined SUPPORT_GLCD && !defined GLCD_COLOR) || defined SUPPORT_OLED || defined SUPPORT_TFT // {38}
    #if defined MB785_GLCD_MODE && (defined SDCARD_SUPPORT || defined SPI_FLASH_FAT || defined FLASH_FAT) // allow the mass storage to initialise before starting the LCD (on same SPI interface on the STM3210C-EVAL)
    if (SDCARD_DETECTION() != 0) {                                       // {69}{84} if the SD card is detected, don't start the LCD task yet but wait until the card has beein initialised
        uTaskerStateChange(TASK_MASS_STORAGE, UTASKER_ACTIVATE);         // start mass storage task
        uTaskerMonoTimer(TASK_LCD, (DELAY_LIMIT)(0.25 * SEC), 0xff);     // start the LCD task after giving the SD card initialisation time to complete
    }
    else {
        uTaskerStateChange(TASK_LCD, UTASKER_ACTIVATE);                  // start LCD task only when not validating
    }
    #elif (defined IDM_L35_B || defined M52259_TOWER || defined TWR_K60N512 || defined TWR_K60D100M || defined TWR_K70F120M || defined AVR32_EVK1105 || defined AVR32_AT32UC3C_EK || defined OLIMEX_LPC2478_STK || defined K70F150M_12M) && defined SDCARD_SUPPORT // {58}{68}
    uTaskerMonoTimer(TASK_LCD, (DELAY_LIMIT)(0.25 * SEC), 0xff);         // start the LCD task after giving the SD card initialisation time to complete
    #else
  //uTaskerMonoTimer(TASK_LCD, (DELAY_LIMIT)(10 * SEC), 0xff);
    uTaskerStateChange(TASK_LCD, UTASKER_ACTIVATE);                      // start LCD task only when not validating
    #endif
#endif
#if defined (USE_SMTP) && !defined (USE_DNS) && defined (SMTP_PARAMETERS)
    uMemcpy(ucSMTP_server, temp_pars->temp_parameters.ucSMTP_server_ip, IPV4_LENGTH);
#endif
#if defined USE_MODBUS && !defined MODBUS_USB_SLAVE                      // {54}{47}
    fnInitModbus();                                                      // {27} initialise MODBUS
#endif
}

extern int fnAreWeValidating(void)
{
    return (iAppState == STATE_VALIDATING);
}

extern void fnWeHaveBeenValidated(void)
{
    iAppState = STATE_ACTIVE;
    uTaskerStopTimer(OWN_TASK);
    fnValidatedInit();
}


extern void fnSaveDebugHandle(int iState)
{
    save_handle = DebugHandle;                                           // push present debug handle
    switch (iState) {                                                    // {24}
#if defined SERIAL_INTERFACE
    case SOURCE_SERIAL:
        DebugHandle = SerialPortID;
        break;
#endif
#if defined USE_USB_CDC && defined USE_MAINTENANCE                       // {70}
    case SOURCE_USB:
        fnSetUSB_debug();                                                // select the USB connection as debug channel
        break;
#endif
    default:
        DebugHandle = NETWORK_HANDLE;
        break;
    }
}

extern void fnRestoreDebugHandle(void)
{
    DebugHandle = save_handle;                                           // pop debug handle
}

#if defined _MAGIC_RESET_FRAME && defined ETH_INTERFACE                  // {86}
// When magic frames are activated the user must supply this routine to handle them
//
extern void fnMagicFrame(unsigned char ucType, unsigned char usOptionalDate[32])
{
    if (ucType == MAGIC_RESET) {                                         // check the magic frame type for reset comand
    #if !defined _WINDOWS
        *(BOOT_MAIL_BOX) = 0x1234;                                       // message in mail box to signal that the commanded reset would like the boot loader to be forced
    #endif
        fnResetBoard();                                                  // command reset
    }
}
#endif

#if defined SERIAL_INTERFACE && defined DEMO_UART                        // {32}
extern QUEUE_HANDLE fnSetNewSerialMode(unsigned char ucDriverMode)
{
    TTYTABLE tInterfaceParameters;                                       // table for passing information to driver
    tInterfaceParameters.Channel = DEMO_UART;                            // set UART channel for serial use
    tInterfaceParameters.ucSpeed = temp_pars->temp_parameters.ucSerialSpeed; // baud rate
    tInterfaceParameters.Rx_tx_sizes.RxQueueSize = RX_BUFFER_SIZE;       // input buffer size
    tInterfaceParameters.Rx_tx_sizes.TxQueueSize = TX_BUFFER_SIZE;       // output buffer size
    tInterfaceParameters.Task_to_wake = OWN_TASK;                        // wake self when messages have been received
    #if defined SUPPORT_FLOW_HIGH_LOW
    tInterfaceParameters.ucFlowHighWater = temp_pars->temp_parameters.ucFlowHigh;// set the flow control high and low water levels in %
    tInterfaceParameters.ucFlowLowWater = temp_pars->temp_parameters.ucFlowLow;
    #endif
    tInterfaceParameters.Config = temp_pars->temp_parameters.SerialMode; // {43}
    #if defined TEST_MSG_MODE
    tInterfaceParameters.Config |= (MSG_MODE);
        #if defined (TEST_MSG_CNT_MODE) && defined (SUPPORT_MSG_CNT)
    tInterfaceParameters.Config |= (MSG_MODE_RX_CNT);
        #endif
    tInterfaceParameters.Config &= ~USE_XON_OFF;
    tInterfaceParameters.ucMessageTerminator = '\r';
    #endif
    #if defined SERIAL_SUPPORT_DMA
        #if defined KL43Z_256_32_CL
    tInterfaceParameters.ucDMAConfig = 0;                                // disable DMA
        #else
    tInterfaceParameters.ucDMAConfig = 0;
  //tInterfaceParameters.ucDMAConfig = UART_TX_DMA;                      // activate DMA on transmission
  //tInterfaceParameters.ucDMAConfig = (UART_RX_DMA /*| UART_RX_DMA_HALF_BUFFER*/); // test half buffer DMA reception
    tInterfaceParameters.ucDMAConfig = (UART_RX_DMA);                    // free-running RX DMA
        #endif
    #endif
    #if defined SUPPORT_HW_FLOW
  //tInterfaceParameters.Config |= RTS_CTS;                              // test RTS/CTS operation
    #endif
    if ((SerialPortID = fnOpen(TYPE_TTY, ucDriverMode, &tInterfaceParameters)) != NO_ID_ALLOCATED) { // open or change the channel with defined configurations (initially inactive)
        fnDriver(SerialPortID, (TX_ON | RX_ON), 0);                      // enable rx and tx
        if (tInterfaceParameters.Config & RTS_CTS) {                     // {8}
            fnDriver(SerialPortID, (MODIFY_INTERRUPT | ENABLE_CTS_CHANGE), 0); // activate CTS interrupt when working with HW flow control (this returns also the present control line states)
            fnDriver(SerialPortID, (MODIFY_CONTROL | SET_RTS), 0);       // activate RTS line when working with HW flow control
        }
    }
    return SerialPortID;
}
#endif


#if defined USE_SMTP
extern void fnSendEmail(int iRepeat)
{
    #if defined USE_DNS
    if (!uMemcmp(ucSMTP_server, cucNullMACIP, IPV4_LENGTH)) {
    #if defined SMTP_PARAMETERS
        fnResolveHostName((CHAR *)temp_pars->temp_parameters.ucSMTP_server, fnDNSListner); // resolve SMTP server address so that we can send
    #else
        fnResolveHostName(cSMTP_provider, fnDNSListner);                 // resolve SMTP server address so that we can send
    #endif
    }
    else {
        fnConnectSMTP(ucSMTP_server, (unsigned char)(temp_pars->temp_parameters.usServers & SMTP_LOGIN), fnEmailTest); // initiate Email transmission
    }
    if (!iRepeat) {
        ucSMTP_Retry = 2;
    }
    #else
    fnConnectSMTP(ucSMTP_server, (unsigned char)(temp_pars->temp_parameters.usServers[DEFAULT_NETWORK] & SMTP_LOGIN), fnEmailTest); // initiate Email transmission
    #endif
}

// Email call back handler
//
static const CHAR *fnEmailTest(unsigned char ucEvent, unsigned short *usData)
{
    switch (ucEvent) {
    case SMTP_GET_DOMAIN:
        return cUserDomain;

    #if defined USE_SMTP_AUTHENTICATION
        #if defined SMTP_PARAMETERS
    case SMTP_USER_NAME:
        return (const CHAR*)temp_pars->temp_parameters.ucSMTP_user_name;

    case SMTP_USER_PASS:
        return (const CHAR*)temp_pars->temp_parameters.ucSMTP_password;
        #else
    case SMTP_USER_NAME:
        return cUserName;

    case SMTP_USER_PASS:
        return cUserPassword;
        #endif
    #endif

    case SMTP_GET_SENDER:
    #ifndef SMTP_PARAMETERS
        return cSender;
    #else
        return (CHAR *)temp_pars->temp_parameters.ucSMTP_user_email;
    #endif

    case SMTP_GET_DESTINATION:
        return cEmailAdd;

    case SMTP_GET_SUBJECT:
        return cSubject;

    case SMTP_SEND_MESSAGE:
        {
        unsigned short usEmailPosition = *usData;
        if (usEmailPosition >= (sizeof(cEmailText) - 1)) {               // here we can send our Email message
            *usData = 0;
            return 0;                                                    // email has been completely sent, inform that there is no more
        }
        else {
            *usData = (sizeof(cEmailText) - 1) - usEmailPosition;        // remaining length
        }
        return (const CHAR *)&cEmailText[usEmailPosition];               // give a pointer to the next part of message
        }

    case SMTP_MAIL_SUCCESSFULLY_SENT:                                    // Our email has been successfully sent!!
        break;

  //case ERROR_SMTP_LOGIN_FAILED:                                        // user name and password incorrect
  //case ERROR_SMTP_LOGIN_NOT_SUPPORTED:                                 // we are trying to use login but the server doesn't support it
  //case ERROR_SMTP_POLICY_REJECT:                                       // we are not using login but the server insists on it
  //case ERROR_SMTP_TIMEOUT:                                             // connection timed out
    default:                                                             // an error has occurred
    #if defined USE_DNS
        uMemcpy(ucSMTP_server, cucNullMACIP, IPV4_LENGTH);               // we reset the IP address since it may be that the server has changed it address
        if (ucSMTP_Retry) {
            ucSMTP_Retry--;
            fnSendEmail(1);                                              // retry
        }
    #endif
        break;
    }
    return 0;
}
#endif                                                                   // #endif USE_SMTP





#if defined (USE_SMTP) && defined (USE_DNS)
static void fnDNSListner(unsigned char ucEvent, unsigned char *ptrIP)
{
    switch (ucEvent) {
    case DNS_EVENT_SUCCESS:
    #if defined USE_SMTP
        uMemcpy(ucSMTP_server, ptrIP, IPV4_LENGTH);                      // save the IP address which has just been resolved
        fnConnectSMTP(ucSMTP_server, (unsigned char)(temp_pars->temp_parameters.usServers & SMTP_LOGIN), fnEmailTest); // initiate Email transmission
    #endif
        break;

    default:                                                             // DNS error message
        break;
    }
}
#endif


#if defined SUPPORT_LCD || (defined SUPPORT_GLCD || defined SUPPORT_OLED) // {38}{48}
    #define LCD_MESSAGE_ROUTINES                                         // message transmission routines to the LCD task - see the file application_lcd.h
    #include "application_lcd.h"                                         // include support routines
    #undef LCD_MESSAGE_ROUTINES
#endif


#if defined TEST_GLOBAL_TIMERS
// Test code allowing the use of global timers to be evaluated
//
static void fnStartGlobalTimers(void)
{
    CONFIG_TIMER_TEST_LEDS();
    TIMER_TEST_LED_ON();
    TIMER_TEST_LED2_ON();
    #if defined GLOBAL_HARDWARE_TIMER
    uTaskerGlobalMonoTimer((UTASK_TASK)(OWN_TASK | HARDWARE_TIMER), (DELAY_LIMIT)(10 * MILLISEC),  E_TIMER_TEST_10MS); // start a 10ms timer
    uTaskerGlobalMonoTimer((UTASK_TASK)(OWN_TASK | HARDWARE_TIMER), (DELAY_LIMIT)(40 * MILLISEC),  E_TIMER_TEST_3MS); // start a 3ms timer
  //uTaskerGlobalMonoTimer((UTASK_TASK)(OWN_TASK | HARDWARE_TIMER), (DELAY_LIMIT)(5 * MILLISEC),   E_TIMER_TEST_5MS); // start a 5ms timer
    #else
    uTaskerGlobalMonoTimer(OWN_TASK, (DELAY_LIMIT)(10 * SEC), E_TIMER_TEST_10S); // start a 10s timer
    uTaskerGlobalMonoTimer(OWN_TASK, (DELAY_LIMIT)(3 * SEC),  E_TIMER_TEST_3S); // start a 3s timer
    uTaskerGlobalMonoTimer(OWN_TASK, (DELAY_LIMIT)(5 * SEC),  E_TIMER_TEST_5S); // start a 5s timer
    #endif
}

// Test timer event handler
//
static void fnHandleGlobalTimers(unsigned char ucTimerEvent)
{
    switch (ucTimerEvent) {
    case E_TIMER_TEST_3S:
        TIMER_TEST_LED_OFF();
    #if defined GLOBAL_HARDWARE_TIMER
        uTaskerGlobalMonoTimer((UTASK_TASK)(OWN_TASK | HARDWARE_TIMER), (DELAY_LIMIT)(3 * MILLISEC),  E_TIMER_TEST_3MS); // restart 3ms timer
    #else
        uTaskerGlobalMonoTimer(OWN_TASK, (DELAY_LIMIT)(3 * SEC), E_TIMER_TEST_3S); // restart 3s timer
    #endif
        break;

    case E_TIMER_TEST_5S:
        TIMER_TEST_LED_ON();
    #if defined GLOBAL_HARDWARE_TIMER
        uTaskerGlobalStopTimer((UTASK_TASK)(OWN_TASK | HARDWARE_TIMER), E_TIMER_TEST_3MS); // kill the 3ms timer
        uTaskerGlobalMonoTimer((UTASK_TASK)(OWN_TASK | HARDWARE_TIMER), (DELAY_LIMIT)(4 * MILLISEC), E_TIMER_TEST_10MS ); // shorten 10 timer
        uTaskerGlobalMonoTimer((UTASK_TASK)(OWN_TASK | HARDWARE_TIMER), (DELAY_LIMIT)(4 * MILLISEC), E_TIMER_TEST_4MS ); // start a new 4ms timer
    #else
        uTaskerGlobalStopTimer(OWN_TASK, E_TIMER_TEST_3S);               // kill the 3s timer
        uTaskerGlobalMonoTimer(OWN_TASK, (DELAY_LIMIT)(4 * SEC), E_TIMER_TEST_10S); // shorten 10s timer
        uTaskerGlobalMonoTimer(OWN_TASK, (DELAY_LIMIT)(4 * SEC), E_TIMER_TEST_4S); // start a new 4s timer
    #endif
        break;

    case E_TIMER_TEST_10S:
        TIMER_TEST_LED_OFF();
        break;

    case E_TIMER_TEST_4S:
        TIMER_TEST_LED2_OFF();
        break;

    default:
        break;
    }
}
#endif

#if defined DEMO_UDP
// configure socket for use with UDP protocol
//
static void fnConfigUDP(void)
{
    if (!((MyUDP_Socket = fnGetUDP_socket(TOS_MINIMISE_DELAY, fnUDPListner, (UDP_OPT_SEND_CS | UDP_OPT_CHECK_CS))) < 0)) {
        fnBindSocket(MyUDP_Socket, MY_UDP_PORT);                         // bind socket
        ptrUDP_Frame    = uMalloc(sizeof(UDP_MESSAGE));                  // get some memory for UDP frame
    }
    else {
        return;                                                          // no socket - this must never happen (ensure that enough user UDP sockets have been defined - USER_UDP_SOCKETS in config.h)!!
    }
}

// UDP data server - reception call back function
//
extern int fnUDPListner(USOCKET SocketNr, unsigned char ucEvent, unsigned char *ucIP, unsigned short usPortNr, unsigned char *data, unsigned short usLength)
{
    switch (ucEvent) {
    case UDP_EVENT_RXDATA:
        //if (usPortNr != MY_UDP_PORT) break;                            // ignore false ports
        //if (uMemcmp(ucIP, ucUDP_IP_Address, IPV4_LENGTH)) break;       // ignore if not from expected IP address

        //if (usLength <= UDP_BUFFER_SIZE) {                             // ignore frames which are too large
            //uMemcpy(&ptrUDP_Frame->ucUDP_Message, data, usLength);     // send the received UDP frame back
            //fnSendUDP(MyUDP_Socket, ucUDP_IP_Address, MY_UDP_PORT, (unsigned char*)&ptrUDP_Frame->tUDP_Header, usLength, OWN_TASK);
        //}
        fnSendUDP(MyUDP_Socket, ucIP, usPortNr, (data - sizeof(UDP_HEADER)), usLength, OWN_TASK); // echo back from transmitting IP and port
        break;

    case UDP_EVENT_PORT_UNREACHABLE:                                     // we have received information that this port is not available at the destination so quit
        break;
    }
    return 0;
}
#endif


#if defined TEST_TFTP
static void tftp_listener(unsigned short usError, CHAR *error_text)
{
    switch (usError) {
    case TFTP_ARP_RESOLVED:                                              // we should repeat the transfer since the TFTP server IP address has been resolved by ARP
        fnTransferTFTP();
        break;

    case TFTP_ARP_RESOLUTION_FAILED:                                     // ARP failed, the server doesn't exist
        break;

    case TFTP_FILE_EQUALITY:                                             // file transfered from TFTP is identical to file already in file system
        break;

    case TFTP_FILE_NOT_EQUAL:                                            // file transfered from TFTP is different from the file already in file system
        break;

    case TFTP_TRANSFER_WRITE_COMPLETE:                                   // write completed successfully
        break;

    case TFTP_TRANSFER_READ_COMPLETE:                                    // read completed successfully
        break;

    case TFTP_TRANSFER_DID_NOT_START:                                    // TFTP server available but it didn't start the transfer
    case TFTP_DESTINATION_UNREACHABLE:
    case TFTP_FILE_NOT_FOUND:                                            // requested file was not found on the server
        fnStopTFTP_server();                                             // abort any activity
        break;
    }
}

// Test TFTP transfer
//
static void fnTransferTFTP()
{
    //fnStartTFTP_client(tftp_listener, ucTFTP_server_ip, TFTP_GET, "test.txt", '0'); // get a file (text.txt) from TFTP server and save it locally (to file '0')
    //fnStartTFTP_client(tftp_listener, ucTFTP_server_ip, TFTP_GET_COMPARE, "test.txt", '0'); // get a file (text.txt) from TFTP server and compare it to local file ('0')
    fnStartTFTP_client(tftp_listener, ucTFTP_server_ip, TFTP_PUT, "test1.txt", '0');  // transfer local file ('0') to TFTP server and save it there as (test1.txt)
}
#endif

#if !defined BLAZE_K22
    #define _IIC_RTC_CODE
        #include "iic_tests.h"                                           // include IIC RTC code to save and retrieve the time and convert format as well as handle a second interrupt
    #undef _IIC_RTC_CODE
    #define _IIC_SENSOR_CODE                                             // {56}
        #include "iic_tests.h"                                           // include IIC sensor routines
    #undef _IIC_SENSOR_CODE
    #define _IIC_INIT_CODE
        #include "iic_tests.h"                                           // include IIC test initialisation routine
    #undef _IIC_INIT_CODE
#endif

#if defined TEST_DISTRIBUTED_TX
static void fnSendDist(void)
{
    unsigned char ucMessage[HEADER_LENGTH + 1];

    ucMessage[MSG_DESTINATION_NODE] = (OurNetworkNumber + 1);            // destination node
    ucMessage[MSG_SOURCE_NODE]      = OurNetworkNumber;                  // own node
    ucMessage[MSG_DESTINATION_TASK] = TASK_LCD;                          // destination task
    ucMessage[MSG_SOURCE_TASK]      = OWN_TASK;                          // own task
    ucMessage[MSG_CONTENT_LENGTH]   = 1;                                 // message length
    ucMessage[MSG_CONTENT_COMMAND]  = 0x55;                              // test data

    fnWrite(INTERNAL_ROUTE, ucMessage, (QUEUE_TRANSFER)(1 + HEADER_LENGTH));// send message to defined task
    ucMessage[MSG_CONTENT_COMMAND]  = 0xaa;                              // test data
    fnWrite(INTERNAL_ROUTE, ucMessage, (QUEUE_TRANSFER)(1 + HEADER_LENGTH));// send message to defined task
    ucMessage[MSG_CONTENT_COMMAND]  = 0xbb;                              // test data
    fnWrite(INTERNAL_ROUTE, ucMessage, (QUEUE_TRANSFER)(1 + HEADER_LENGTH));// send message to defined task
}
#endif

#if defined RAM_TEST                                                     // {61}
    #define RAM_BLOCK_SIZE    128                                        // test block length (should be divisible by 4) - requires maximum stack use of 3 x RAM_BLOCK_SIZE

    #if defined _WINDOWS
        unsigned long ulTestMemory[SIZE_OF_RAM];
        #undef RAM_START_ADDRESS
        #define RAM_START_ADDRESS &ulTestMemory[0]
    #elif !defined RAM_START_ADDRESS
        #define RAM_START_ADDRESS START_OF_SRAM
    #endif

static unsigned long *fnRAM_test(int iBlockNumber, int iBlockCount)
{
    unsigned long ulTestBlockBackup[RAM_BLOCK_SIZE/sizeof(unsigned long)]; // temporary backup block on (top of) stack

    // Test variables (probably) in registers
    //
    register unsigned long i;
    register unsigned long *ptrTop = &ulTestBlockBackup[(RAM_BLOCK_SIZE/sizeof(unsigned long)) - 1];                  // {71}
    register unsigned long *ptrBottom;
    register unsigned long *ptrTestAddress = (unsigned long *)RAM_START_ADDRESS;

    // Test variable on the bottom of stack
    //
    unsigned long *ptrAddressError = (unsigned long *)0xffffffff;        // this address is never in RAM and so indicates no error - last variable on stack


    ptrBottom = (unsigned long *)&ptrAddressError;                       // {71}
    if (ptrBottom > ptrTop) {
        ptrTop = (unsigned long *)&ptrAddressError;                      // ensure that stack top and bottom are correct (some compilers put the variables on the stack in different orders)
        ptrBottom = ulTestBlockBackup;
    }

    ptrTestAddress += (iBlockNumber * (RAM_BLOCK_SIZE/sizeof(unsigned long))); // first block address

    while (iBlockCount--) {                                              // for each block of RAM to be tested
        if (((ptrTestAddress + (RAM_BLOCK_SIZE/sizeof(unsigned long))) > ptrBottom) && (ptrTestAddress <= ptrTop)) {
            // This block in RAM is conflicting with the area of stack that we are using - we do a recursive call of this block test so that our stack is moved down in memory
            // (a second recursive call may then be needed in the worst case)
            //
            ptrAddressError = fnRAM_test(iBlockNumber, 1);               // perform a single block test
            ptrTestAddress += (RAM_BLOCK_SIZE/sizeof(unsigned long));
        }
        else {                                                           // non-conflicting RAM block area so perform the test
            uDisable_Interrupt();                                        // ensure that no memory is changed by interrupts
            for (i = 0; i < (RAM_BLOCK_SIZE/sizeof(unsigned long)); i++) { // backup the block to be tested
                ulTestBlockBackup[i] = *ptrTestAddress++;
            }
            ptrTestAddress -= i;
            for (i = 0; i < (RAM_BLOCK_SIZE/sizeof(unsigned long)); i++) { // set first test pattern
                *ptrTestAddress++ = 0x55555555;
            }
            ptrTestAddress -= i;
            for (i = 0; i < (RAM_BLOCK_SIZE/sizeof(unsigned long)); i++) { // check that the first pattern is intact
                if (*ptrTestAddress != 0x55555555) {
                    ptrAddressError = ptrTestAddress;                    // error detected at this address
                    i = (RAM_BLOCK_SIZE/sizeof(unsigned long));          // {71} set the loop counter value to maximum
                    break;
                }
                ptrTestAddress++;
            }
            ptrTestAddress -= i;
            for (i = 0; i < (RAM_BLOCK_SIZE/sizeof(unsigned long)); i++) { // set second test pattern
                *ptrTestAddress++ = 0xaaaaaaaa;
            }
            ptrTestAddress -= i;
            for (i = 0; i < (RAM_BLOCK_SIZE/sizeof(unsigned long)); i++) { // check that the second pattern is intact
                if (*ptrTestAddress != 0xaaaaaaaa) {
                    ptrAddressError = ptrTestAddress;                    // error detected at this address
                    i = (RAM_BLOCK_SIZE/sizeof(unsigned long));          // {71} set the loop counter value to maximum
                    break;
                }
                ptrTestAddress++;
            }
            ptrTestAddress -= i;
            for (i = 0; i < (RAM_BLOCK_SIZE/sizeof(unsigned long)); i++) { // restore the tested block's original content
                *ptrTestAddress++ = ulTestBlockBackup[i];
            }
            uEnable_Interrupt();                                         // enable interrupt between each block check to allow any pending interrupts to be serviced
        }                                                                // ptrTestAddress automatically points to the start of the following block
        if (ptrAddressError != (unsigned long *)0xffffffff) {            // if an error has been detected in a block we quit further testing and return the errored address
            break;
        }
        iBlockNumber++;
    }
    return ptrAddressError;                                              // return 0xffffffff if successful or else the last detected error address
}
#endif

#define _PORT_INT_CODE
    #include "Port_Interrupts.h"                                         // port interrupt configuration code and interrupt handling
#undef  _PORT_INT_CODE

#if defined RTC_TEST                                                     // {3}
// Interrupt once a minute
//
static void test_minute_tick(void)
{
    static int iBlick = 0;

    RTC_SETUP rtc_setup;
    rtc_setup.command = RTC_GET_TIME;
    fnConfigureRTC((void *)&rtc_setup);                                  // get the present time

    if (iBlick == 0) {
        iBlick = 1;
        TIMER_TEST_LED_OFF();
    }
    else {
        iBlick = 0;
        TIMER_TEST_LED_ON();
    }
}

// Interrupt at alarm time
//
static void test_alarm(void)
{
    RTC_SETUP rtc_setup;
    rtc_setup.command = RTC_GET_TIME;
    fnConfigureRTC((void *)&rtc_setup);                                  // get the present time
    TIMER_TEST_LED_ON();
    TIMER_TEST_LED2_OFF();
}

// Interrupt at stopwatch count down
//
static void test_stopwatch(void)
{
    RTC_SETUP rtc_setup;
    rtc_setup.command = (RTC_TICK_MIN | RTC_DISABLE);

    fnConfigureRTC((void *)&rtc_setup);                                  // disable further minute TICKs

    rtc_setup.command = RTC_TICK_SEC;                                    // change to second TICKs
    rtc_setup.int_handler = test_minute_tick;                            // re-use this interrupt routine

    fnConfigureRTC((void *)&rtc_setup);

    TIMER_TEST_LED2_ON();
}

static void fnTestRTC(void)
{
    RTC_SETUP rtc_setup;

    CONFIG_TIMER_TEST_LEDS();                                            // drive some LEDs for visibility
    TIMER_TEST_LED_ON();
    TIMER_TEST_LED2_ON();

    rtc_setup.command   = RTC_TIME_SETTING;                              // set the present time to the RTC (this could be collected from a timer server and is a fixed value here)
    rtc_setup.usYear    = 2015;                                          // {64} - ucDayOfWeek is not set since it will be calculated
    rtc_setup.ucMonthOfYear = 2;
    rtc_setup.ucDayOfMonth = 5;
    rtc_setup.ucHours   = 3;
    rtc_setup.ucMinutes = 23;
    rtc_setup.ucSeconds = 53;

    fnConfigureRTC((void *)&rtc_setup);                                  // set the time

    rtc_setup.command = RTC_TICK_MIN;                                    // configure periodic interrupts - once a minute
    rtc_setup.int_handler = test_minute_tick;
    fnConfigureRTC((void *)&rtc_setup);                                  // set a minute interrupt rate (first expected after 7 seconds)

    rtc_setup.command = RTC_ALARM_TIME;                                  // set an alarm time
    rtc_setup.int_handler = test_alarm;
    rtc_setup.ucMinutes = 24;
    rtc_setup.ucSeconds = 14;
    fnConfigureRTC((void *)&rtc_setup);                                  // set an alarm interrupt rate (expected after 21 seconds)

    rtc_setup.command = RTC_STOPWATCH_GO;                                // set a stop watch time (minutes to nearest minute)
    rtc_setup.int_handler = test_stopwatch;
    rtc_setup.ucMinutes = 2;
    fnConfigureRTC((void *)&rtc_setup);                                  // set 2 minute stop watch (expected after 67 seconds)
}
#endif

#define _ADC_TIMER_ROUTINES                                              // include ADC configuration and interrupt handlers
    #include "ADC_Timers.h"                                              // as well as PIT configuration and handling
#undef  _ADC_TIMER_ROUTINES                                              // and DMA timer, GPT timer and gstandard timer configuration and handling

#define _CAN_INIT_CODE
    #include "can_tests.h"                                               // CAN initialiation code and transmission routine
#undef _CAN_INIT_CODE

#if defined SUPPORT_SLCD && defined SUPPORT_RTC                          // {60}
    #include "slcd_time.h"                                               // {95} hardware specific time drawing functions
#endif

#if defined FAT_EMULATION                                                // {98}
// Fixed details of test data files in linear memory
//
#define DATA_FILE_1_LENGTH    3722                                       // the number of raw data samples for file 1 (even)
#define DATA_FILE_2_LENGTH    74982                                      // the number of raw data samples for file 2 (even)
#define MAX_DATA_FILE_LENGTH  (128 * 1024)
#define LINEAR_DATA_1         (const unsigned char *)(256 * 1024)        // internal flash address of the start of the file's raw data
#define LINEAR_DATA_2         (const unsigned char *)(LINEAR_DATA_1 + MAX_DATA_FILE_LENGTH) // internal flash address of the start of the file's raw data

static const CHAR cHTML_link[] = "<html><head><title>Emulated FAT</title></head><body bgcolor=#d8d8d8 marginheight=30><center><font color=#ff0000 style=font-size:30px><b style='mso-bidi-font-weight:normal'>&micro;Tasker FAT Emulation</font></b><br><br><br>Full details of this and many other features can be found at <a href=""http://www.uTasker.com/"">the uTasker web site</a>.</body></html>";

#define FORMAT_TYPE_INVALID_FILE  0
#define FORMAT_TYPE_RAW_BINARY    1
#define FORMAT_TYPE_RAW_STRING    2
#define FORMAT_TYPE_CSV_FORMATTED 3

typedef struct stDATA_FILE_INFORMATION
{
    unsigned long ulDataFileLength;
    const unsigned char *ptrFileLocation;
    #if defined EMULATED_FAT_FILE_NAME_CONTROL
        const CHAR *ptrFileName;                                         // short file name (8:3 format) or LFN (when FAT_EMULATION_LFN is enabled) [leave at 0 for default name]
    #endif
    #if defined EMULATED_FAT_FILE_DATE_CONTROL
        unsigned short usCreationTime;
        unsigned short usCreationDate;                                   // [leave at 0 for fixed date/time stamp]
    #endif
    unsigned char ucFormatType;
} DATA_FILE_INFORMATION;

// Formatting algorithm details
//
#define RAW_DATA_SIZE         2                                          // data samples are each 2 bytes in size
#define FORMATTING_FACTOR     4                                          // each short word input is formatted to ASCII decimal "-32000, "
#define DATA_SET_CONTENT      16                                         // after each 16 formatted values a line feed sequence is inserted "    \r\n"
#define CSV_SEPARATOR_VALUE   ',';

#define APPLICATION_DATA_FILES  8

static DATA_FILE_INFORMATION dataFile[APPLICATION_DATA_FILES] = {{0}};

#define CREATION_HOURS1         8
#define CREATION_MINUTES1       23
#define CREATION_SECONDS1       1

#define CREATION_DAY_OF_MONTH1  3
#define CREATION_MONTH_OF_YEAR1 8
#define CREATION_YEAR1          (2015 - 1980)

#define CREATION_HOURS2         13
#define CREATION_MINUTES2       9
#define CREATION_SECONDS2       58

#define CREATION_DAY_OF_MONTH2  3
#define CREATION_MONTH_OF_YEAR2 8
#define CREATION_YEAR2          (2015 - 1980)


// The application must supply this function when using FAT_EMULATION
// - the FAT emulator calls it to obtain referenced file information (formatted length, location in memory, creation date/time and name)
//
extern const unsigned char *fnGetDataFile(int iDisk, int iDataRef, EMULATED_FILE_DETAILS *ptrFileDetails)
{
    if ((iDataRef < APPLICATION_DATA_FILES) && (dataFile[iDataRef].ucFormatType != FORMAT_TYPE_INVALID_FILE)) {
        ptrFileDetails->ulFileLength = dataFile[iDataRef].ulDataFileLength; // raw content length of file
    #if defined EMULATED_FAT_FILE_DATE_CONTROL
        ptrFileDetails->usCreationTime = dataFile[iDataRef].usCreationTime;
        ptrFileDetails->usCreationDate = dataFile[iDataRef].usCreationDate; // (leave at 0 for fixed date/time stamp)
    #endif
    #if defined EMULATED_FAT_FILE_NAME_CONTROL
        ptrFileDetails->ptrFileName = dataFile[iDataRef].ptrFileName;
    #endif
        ptrFileDetails->ucValid = 1;                                     // file is valid
        return (dataFile[iDataRef].ptrFileLocation);                     // memory mapped address of file's raw data
    }
    else {
        ptrFileDetails->ucValid = 0;                                     // file is not valid
        return 0;
    }
}

// The application must supply this function when using FAT_EMULATION
// - the FAT emulator calls it to obtain referenced file content, which can be returned as raw data or formatted
// - this example formats raw binary data to CSV format so that it can be easily opened in various PC programs for viewing and processing
//
extern int uDatacopy(int iDisk, int iDataRef, unsigned char *ptrSectorData, const unsigned char *ptrSourceData, int iLength)
{
    int iAdded = 0;
    unsigned long ulNextDataSetEnd;
    unsigned long ulDataOffset;
    int iStringLength;
    CHAR *ptrBuf;
    unsigned long ulThisDataLength;
    signed short *ptrRawSource;

    if ((iDataRef >= APPLICATION_DATA_FILES) || (dataFile[iDataRef].ucFormatType == FORMAT_TYPE_INVALID_FILE)) {                                                    // data belongs to first disk only
        return 0;                                                        // no data added
    }
    switch (dataFile[iDataRef].ucFormatType) {
    case FORMAT_TYPE_RAW_BINARY:
    case FORMAT_TYPE_RAW_STRING:
        iAdded = (dataFile[iDataRef].ulDataFileLength - (dataFile[iDataRef].ptrFileLocation - ptrSourceData)); // remaining raw content length
        if (iAdded > iLength) {
            iAdded = iLength;
        }
        if (FORMAT_TYPE_RAW_STRING == dataFile[iDataRef].ucFormatType) {
            uMemcpy(ptrSectorData, ptrSourceData, iAdded);               // strings are in code so copy directly from memory
        }
        else {
            fnGetParsFile((unsigned char *)ptrSourceData, ptrSectorData, iAdded); // prepare the raw data
        }
        break;

    case FORMAT_TYPE_CSV_FORMATTED:
        // CSV Formatted files
        //
        ulDataOffset = (ptrSourceData - dataFile[iDataRef].ptrFileLocation); // the physical offset
        ulDataOffset /= (FORMATTING_FACTOR * RAW_DATA_SIZE);             // raw data entry offset
        ulNextDataSetEnd = (ulDataOffset%(DATA_SET_CONTENT + 1));
        ulDataOffset = (((ulDataOffset/(DATA_SET_CONTENT + 1)) * DATA_SET_CONTENT) + ulNextDataSetEnd);
        ptrRawSource = (signed short *)dataFile[iDataRef].ptrFileLocation; // pointer to the raw data in memory
        ptrRawSource += ulDataOffset;
        ptrSourceData = (dataFile[iDataRef].ptrFileLocation + ulDataOffset);    
        ulThisDataLength = (dataFile[iDataRef].ulDataFileLength - (ptrSourceData - dataFile[iDataRef].ptrFileLocation)); // the remaining raw data length
        while (iLength > 0) {                                            // while data is to be added
            if (ulThisDataLength == 0) {                                 // no more data to add
                break;
            }
            if (ulNextDataSetEnd == DATA_SET_CONTENT) {
                uMemcpy(ptrSectorData, "      \r\n", 8);
                ulNextDataSetEnd = 0;
            }
            else {
                signed short sValue;
                fnGetParsFile((unsigned char *)ptrRawSource, (unsigned char *)&sValue, sizeof(sValue)); // get the next raw sample from the linear data area
                ptrBuf = fnBufferDec(sValue, DISPLAY_NEGATIVE, (CHAR *)ptrSectorData); // the ASCII decimal representation of the sample
                iStringLength = (ptrBuf - (CHAR *)ptrSectorData);        
                if (iStringLength < 6) {
                    ptrBuf = (CHAR *)(ptrSectorData + (6 - iStringLength));
                    uReverseMemcpy(ptrBuf, ptrSectorData, iStringLength);
                    ptrBuf = (CHAR *)ptrSectorData;
                    while (iStringLength++ < 6) {
                        *ptrBuf++ = ' ';
                    }
                    ptrBuf = (CHAR *)(ptrSectorData + 6);
                }
                *ptrBuf++ = CSV_SEPARATOR_VALUE;
                *ptrBuf = ' ';
                ptrRawSource++;
                ulThisDataLength -= RAW_DATA_SIZE;
                ulNextDataSetEnd++;
            }
            iLength -= 8;
            iAdded += 8;
            ptrSectorData += 8;
        }
        break;
    default:
        break;
    }
    return iAdded;                                                       // the length added to the buffer
}


    #if SIZE_OF_FLASH >= (512 * 1024)
// Analyse a linear data file in flash to determine its raw content length
//
static unsigned long fnGetDataFileLength(const unsigned char *ptrRawData, unsigned long ulDataFileLength, int iIncrement)
{
    unsigned short *ptrData = (unsigned short *)ptrRawData;
    signed short sValue;
    unsigned long ulFileContentLength = 0;
    while (1) {                                                          // identify the raw data size in data file
        fnGetParsFile((unsigned char *)ptrData, (unsigned char *)&sValue, sizeof(sValue));
        if (sValue == -1) {
            break;                                                       // 0xffff considered as empty data location so quit counting size
        }
        ulFileContentLength += RAW_DATA_SIZE;
        if (ulFileContentLength >= MAX_DATA_FILE_LENGTH) {
            break;
        }
        ptrData++;
    }
    if (ulFileContentLength == 0) {                                      // if empty, prime some data in flash
        unsigned long ulLength;
        sValue = 0;                                                      // start at 0
        ulLength = (ulDataFileLength & ~1);                              // ensure even length
        while (ulLength != 0) {
            ulLength -= sizeof(unsigned short);
            fnWriteBytesFlash((unsigned char *)ptrRawData, (unsigned char *)&sValue, sizeof(sValue));
            ptrRawData += RAW_DATA_SIZE;
            if (sValue == 32766) {                                       // maximum value reached
                sValue = -32768;                                         // overflow to maximum negative value
            }
            else if (sValue == -32768) {                                 // maximum negative value has been reached
                sValue = 32766;                                          // overflow to maximum positive value
            }
            else {
                if (iIncrement != 0) {
                    sValue += RAW_DATA_SIZE;                             // increment next value
                }
                else {
                    sValue -= RAW_DATA_SIZE;                             // decrement next value
                }
            }
        }
        #if defined _FLASH_ROW_SIZE && _FLASH_ROW_SIZE > 0
        fnWriteBytesFlash(0, 0, 0);                                      // close any outstanding FLASH buffer
        #endif
        return (ulDataFileLength & ~1);                                  // the primed data length
    }
    return ulFileContentLength;                                          // the raw data length counted
}
    #endif

// Prepare data files in linear flash (prime them if not already present)
//
static void fnPrepareEmulatedFAT(void)
{
    #if SIZE_OF_FLASH >= (512 * 1024)
    dataFile[0].ulDataFileLength = fnGetDataFileLength(LINEAR_DATA_1, DATA_FILE_1_LENGTH, 1); // identify the present raw size of the first data file (prime data if nothing presently exists)
    dataFile[1].ulDataFileLength = fnGetDataFileLength(LINEAR_DATA_2, DATA_FILE_2_LENGTH, 0); // identify the present raw size of the second data file (prime data if nothing presently exists)
    dataFile[0].ptrFileLocation = LINEAR_DATA_1;                         // memory mapped address of file's raw data
    dataFile[0].ucFormatType = FORMAT_TYPE_RAW_BINARY;
    dataFile[1].ptrFileLocation = LINEAR_DATA_2;                         // memory mapped address of file's raw data
    dataFile[1].ucFormatType = FORMAT_TYPE_RAW_BINARY;
    dataFile[2].ptrFileLocation = dataFile[0].ptrFileLocation;           // memory mapped address of file's raw data
    dataFile[2].ulDataFileLength = dataFile[0].ulDataFileLength + ((dataFile[0].ulDataFileLength/(2 * DATA_SET_CONTENT)) * RAW_DATA_SIZE); // increase due to data group separators
    dataFile[2].ulDataFileLength *= FORMATTING_FACTOR;                   // the increase in size of the file due to the ASCII formatting
    dataFile[2].ucFormatType = FORMAT_TYPE_CSV_FORMATTED;
    dataFile[3].ptrFileLocation = dataFile[1].ptrFileLocation;           // memory mapped address of file's raw data
    dataFile[3].ulDataFileLength = dataFile[1].ulDataFileLength + ((dataFile[1].ulDataFileLength/(2 * DATA_SET_CONTENT)) * RAW_DATA_SIZE); // increase due to data group separators
    dataFile[3].ulDataFileLength *= FORMATTING_FACTOR;                   // the increase in size of the file due to the ASCII formatting
    dataFile[3].ucFormatType = FORMAT_TYPE_CSV_FORMATTED;
    #endif
    dataFile[4].ptrFileLocation  = FLASH_START_ADDRESS;                  // data file locations in memory
    #if SIZE_OF_FLASH >= (512 * 1024)
    dataFile[4].ulDataFileLength = (unsigned long)(LINEAR_DATA_1 - FLASH_START_ADDRESS); // raw data length
    #else
        #if defined FLEXFLASH_DATA
    dataFile[4].ulDataFileLength = (SIZE_OF_FLASH - SIZE_OF_FLEXFLASH);  // raw data length of complete flash
        #else
    dataFile[4].ulDataFileLength = SIZE_OF_FLASH;                        // raw data length of complete flash
        #endif
    #endif
    dataFile[4].ucFormatType = FORMAT_TYPE_RAW_BINARY;
    dataFile[5].ptrFileLocation  = (const unsigned char *)cHTML_link;    // data file locations in memory
    dataFile[5].ulDataFileLength = (sizeof(cHTML_link) - 1);             // raw string content length
    dataFile[5].ucFormatType = FORMAT_TYPE_RAW_STRING;                   // used to avoid flash routines being used for access
    dataFile[6].ptrFileLocation  = (const unsigned char *)RAM_START_ADDRESS;
    dataFile[6].ulDataFileLength = SIZE_OF_RAM;
    dataFile[6].ucFormatType = FORMAT_TYPE_RAW_STRING;                   // used to avoid flash routines being used for access
            #if defined FLEXFLASH_DATA                                   // if kinetis with flex we display a further file with its content
    dataFile[7].ucFormatType = FORMAT_TYPE_RAW_BINARY;
    dataFile[7].ptrFileLocation = (const unsigned char *)(FLASH_START_ADDRESS + (SIZE_OF_FLASH - SIZE_OF_FLEXFLASH)); // set the virtual address of the flex memory
    dataFile[7].ulDataFileLength = SIZE_OF_FLEXFLASH;
            #endif
    #if SIZE_OF_FLASH >= (512 * 1024)
        #if defined EMULATED_FAT_FILE_DATE_CONTROL
    dataFile[0].usCreationTime = dataFile[2].usCreationTime = (CREATION_SECONDS1 | (CREATION_MINUTES1 << 5) | (CREATION_HOURS1 << 11));
    dataFile[0].usCreationDate = dataFile[2].usCreationDate = (CREATION_DAY_OF_MONTH1 | (CREATION_MONTH_OF_YEAR1 << 5) | (CREATION_YEAR1 << 9));
    dataFile[1].usCreationTime = dataFile[3].usCreationTime = (CREATION_SECONDS2 | (CREATION_MINUTES2 << 5) | (CREATION_HOURS2 << 11));
    dataFile[1].usCreationDate = dataFile[3].usCreationDate = (CREATION_DAY_OF_MONTH2 | (CREATION_MONTH_OF_YEAR2 << 5) | (CREATION_YEAR2 << 9));
        #endif
    #endif
    #if defined EMULATED_FAT_FILE_NAME_CONTROL
        #if defined FAT_EMULATION_LFN
            #if SIZE_OF_FLASH >= (512 * 1024)
    dataFile[0].ptrFileName = "Raw Data File 1.bin";
    dataFile[1].ptrFileName = "Raw Data File 2.bin";
    dataFile[2].ptrFileName = "Formatted Data File 1.csv";
    dataFile[3].ptrFileName = "Formatted Data File 2.csv";
            #endif
    dataFile[4].ptrFileName = "software.bin";
    dataFile[5].ptrFileName = "uTasker.html";
    dataFile[6].ptrFileName = "ram.bin";
            #if defined FLEXFLASH_DATA                                   // if kinetis with flex we display a further file with its content
    dataFile[7].ptrFileName = "flex.bin";
            #endif
        #else
            #if SIZE_OF_FLASH >= (512 * 1024)
    dataFile[0].ptrFileName = "DATA-1.BIN";
    dataFile[1].ptrFileName = "DATA-2.BIN";
    dataFile[2].ptrFileName = "DATA-1.CSV";
    dataFile[3].ptrFileName = "DATA-2.CSV";
            #endif
    dataFile[4].ptrFileName = "SOFTWARE.BIN";
    dataFile[5].ptrFileName = "UTASKER.HTM";
    dataFile[6].ptrFileName = "RAM.BIN";
            #if defined FLEXFLASH_DATA                                   // if kinetis with flex we display a further file with its content
    dataFile[7].ptrFileName = "FLEX.BIN";
            #endif
        #endif
    #endif
}
#endif


#if defined ETHERNET_BRIDGING && !defined USB_CDC_RNDIS                  // {75}
// Example of bridging Ethernet reception content to the serial interface
//
extern void fnBridgeEthernetFrame(ETHERNET_FRAME *ptr_rx_frame)
{
    #if defined SERIAL_INTERFACE
    if (SerialPortID != NO_ID_ALLOCATED) {                               // serial interface handle is valid
        unsigned short usEthernetLength = (ptr_rx_frame->frame_size - 4);// frame length
        if (fnWrite(SerialPortID, 0, usEthernetLength) != 0) {           // check that there is enough space for the complete frame (otherwise drop the frame)
            fnWrite(SerialPortID, ptr_rx_frame->ptEth->ethernet_destination_MAC, usEthernetLength); // send the frame to the serial interface
        }
    }
    #endif
}
#endif

#if defined SUPPORT_VLAN && defined SUPPORT_DYNAMIC_VLAN                 // {81}
// This routine needs to be supplied by the application when SUPPORT_DYNAMIC_VLAN is enabled
//
extern int fnVLAN_Accept(ETHERNET_FRAME *ptr_rx_frame)
{
    // Add decisions for accepting individual VLAN tagged frames as required
    //
    return 0;                                                            // reject this VLAN tagged frames
}
#endif

#if defined SUPPORT_VLAN && defined SUPPORT_DYNAMIC_VLAN_TX              // {81}
// This routine needs to be supplied by the application when SUPPORT_DYNAMIC_VLAN_TX is enabled
//
extern int fnVLAN_tag_tx(QUEUE_HANDLE channel)
{
    // Add decisions for tagging transmission on specific interfaces
    //
    if (vlan_active != 0) {
        return 1;                                                        // tag all transmissions when VLAN is enabled
    }
    return 0;                                                            // don't VLAN tag
}
#endif

#if (IP_NETWORK_COUNT > 1)                                               // {92}
// The user can decide which network the reception is assigned to - to reject a frame, return NETWORK_REJECT
//
extern unsigned char fnAssignNetwork(ETHERNET_FRAME *frame, unsigned char *ptrDestinationIP)
{
    #if defined USE_IPV6
    if (frame->ucIPV6_Protocol != 0) {                                   // IPv6 frame reception
        // The user can asign this address to a specific network if required
        //
        return DEFAULT_NETWORK;                                          // assume default network
    }
    #endif
    if (uMemcmp(ptrDestinationIP, network[SECOND_NETWORK].ucOurIP, IPV4_LENGTH) == 0) { // if the destination unicast IP address is for the second network
    #if defined USB_TO_TCP_IP && !defined ETHERNET_BRIDGING && (IP_INTERFACE_COUNT > 1)
        if (frame->ucInterface != (RNDIS_INTERFACE >> INTERFACE_SHIFT)) { // only allow handing second network on the RNDIS interface
            return NETWORK_REJECT;
        }
    #endif
        return SECOND_NETWORK;                                           // handle unicast on the second network
    }
    else {
        if (uMemcmp(ptrDestinationIP, cucBroadcast, IPV4_LENGTH) == 0) { // {94} broadcasts
    #if defined USB_TO_TCP_IP && !defined ETHERNET_BRIDGING && (IP_INTERFACE_COUNT > 1)
            switch (frame->ucInterface) {
            case (ETHERNET_INTERFACE >> INTERFACE_SHIFT):                // broadcast received on Ethernet interface
                frame->ucBroadcastResponse = (1 << DEFAULT_NETWORK);     // restrict responses to the default network
                return DEFAULT_NETWORK;                                  // handle on the default network
            case (RNDIS_INTERFACE >> INTERFACE_SHIFT):                   // broadcast received on RNDIS interface
                frame->ucBroadcastResponse = (1 << SECOND_NETWORK);      // restrict responses to its network
                return SECOND_NETWORK;                                   // handle on the second network
            }
    #else
            frame->ucBroadcastResponse = ((1 << DEFAULT_NETWORK) | (1 << SECOND_NETWORK)); // handle broadcast on both networks
    #endif
        }
    #if defined SUPPORT_SUBNET_BROADCAST                                 // {94} subnet broadcasts
        else {
            if (fnSubnetBroadcast(ptrDestinationIP, &network[DEFAULT_NETWORK].ucOurIP[0], &network[DEFAULT_NETWORK].ucNetMask[0], IPV4_LENGTH) != 0) { // if sub-net broadcast for this network
                frame->ucBroadcastResponse |= (1 << DEFAULT_NETWORK);    // handle on default network
                return DEFAULT_NETWORK;                                  // handle on the default network
            }
            if (fnSubnetBroadcast(ptrDestinationIP, &network[SECOND_NETWORK].ucOurIP[0], &network[SECOND_NETWORK].ucNetMask[0], IPV4_LENGTH) != 0) { // if sub-net broadcast for this network
                frame->ucBroadcastResponse |= (1 << SECOND_NETWORK);     // handle on second network (possibly as well as default)
                return SECOND_NETWORK;                                   // handle on the second network
            }
        }
    #endif
    #if defined USB_TO_TCP_IP && !defined ETHERNET_BRIDGING && (IP_INTERFACE_COUNT > 1)
        if (frame->ucInterface == (RNDIS_INTERFACE >> INTERFACE_SHIFT)) { // don't allow handing first network on the RNDIS interface
            return NETWORK_REJECT;
        }
    #endif
        return DEFAULT_NETWORK;                                          // assume default network
    }
}
#endif

#if (IP_INTERFACE_COUNT > 1) && defined RESTRICTED_GATEWAY_INTERFACE
// This is called when ARP wants to automatically re-resolve the gateway after a periodic timeout
// - if nothing is done it will only re-resolve on the interface where it was previously known to be on
//
extern void fnRestrictGatewayInterface(ARP_TAB *ptrARPTab)               // {89}
{
    ptrARPTab->ucInterface = INTERFACE_MASK;                             // re-resolve on all interfaces (default when RESTRICTED_GATEWAY_INTERFACE is not defined)
    // The user is free to add other decisions such as restricting to certain interfaces
    //
}
#endif


// The user has the chance to configure things very early after startup (Note - heap not yet available!)
//
extern void fnUserHWInit(void)
{
    #if defined USB_HOST_SUPPORT
    USB_HOST_POWER_CONFIG();                                             // configure USB host power supply to default (off) state
    #endif
    #if defined SC16IS7XX_CLOCK_FROM_TIMER
    CONFIG_SC16IS7XX_CLOCK();
    #endif
    #if defined LAN_REPORT_ACTIVITY
    CONFIGURE_LAN_LEDS();                                                // configure and drive ports
    #endif
    #if defined SUPPORT_KEY_SCAN
    INIT_KEY_SCAN();                                                     // general initialisation for key scan
    #endif
    #if (defined SPECIAL_LCD_DEMO || defined SUPPORT_GLCD) && !defined SUPPORT_TFT && !defined TFT_GLCD_MODE // {38} configure GLCD ports and drive RST line if required
    CONFIGURE_GLCD();
    #endif
    #if defined MB785_GLCD_MODE && defined SDCARD_SUPPORT
    CONFIGURE_SDCARD_DETECT_INPUT();                                     // {69}
    #endif
    #if defined ETH_INTERFACE
        #if defined M52259EVB                                            // this board has a DP83640 PHY, which requires a 167ms powerup stabilisation delay. The reset is help low for this period. It is released when configuring the Ethernet connection.
    RESET_RCR |= FRCRSTOUT;                                              // assert RSTO
        #elif defined RESET_PHY
    ASSERT_PHY_RST();                                                    // immediately set PHY to reset state
    CONFIG_PHY_STRAPS();                                                 // configure the required strap options - the reset line will be de-asserted during the Ethernet configuration
        #endif
    #endif
    #if defined RAM_TEST                                                 // {61}
    if (fnRAM_test(0, (SIZE_OF_RAM/RAM_BLOCK_SIZE)) != (unsigned long *)0xffffffff) { // test code of a complete RAM area
        // The return address was the address in RAM that failed
        //
        while (1) {}                                                     // serious error found in RAM - stop here
    }
    #endif
    #if defined nRF24L01_INTERFACE
    fnPrepare_nRF24L01_signals();
    #endif
}
