/************************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher

    ---------------------------------------------------------------------
    File:      application.h
    Project:   uTasker project
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2019
    *********************************************************************
    18.02.2006 Add SMTP parameter settings                               {1}
    03.06.2007 Add FTP user definable FTP timeout and ACTIVE_FTP_LOGIN   {2}
    29.06.2007 Add usUSB_state, SOURCE_USB, fnSendToUSB(), fnSetUSB_debug(),
                       ES_USB_LOGIN, ES_USB_COMMAND_MODE and USB_ON_LINE {3}
    30.10.2008 Add MODBUS (and version numbers incremented)              {4}
    20.01.2009 Add fnAddResetCause()                                     {5}
    03.02.2009 Add MODBUS parameter version number                       {6}
    11.02.2009 Add fnInitModbus()                                        {7}
    01.07.2009 Add fnDisplayBitmap()                                     {8}
    06.08.2009 MODBUS USB slave support added                            {9}
    11.08.2009 Add extra GLCD events                                     {10}
    05.02.2010 Add IPV6 settings - change ucServers to usServers         {11}
    02.10.2010 Add E_LCD_STYLE                                           {12}
    14.11.2011 Add fnSetShowTime()                                       {13}
    13.12.2011 Add ftp client parameters and iFTP_data_state             {14}
    27.12.2011 Add fnSendCAN_message()                                   {15}
    26.03.2012 Add ipv6in4 relay destinations                            {16}
    27.05.2012 Add multiple network support                              {17}
    03.06.2012 Add FTP server IPv6 address                               {18}
    08.07.2013 Add fnSetupUDPSocket() and fnAddDiscoverySerialNumber()   {19}
    01.03.2014 Add message MOUNT_USB_MSD                                 {20}
    06.03.2014 Add fnInitialiseSNMP()                                    {21}
    15.04.2014 Add fnDisplayIP()                                         {22}
    20.04.2014 Add tilt variables                                        {23}
    15.07.2014 Add fnShowLowPowerMode()                                  {24}
    07.12.2014 Add fnHandleFreeMaster()                                  {25}
    27.12.2014 Add fnSetBacklight()                                      {26}
    01.03.2015 Add ES_USB_KEYBOARD_MODE                                  {27}
    19.03.2015 Add touch screen calibration parameters                   {28}
    15.05.2015 Modify USBPortID_comms to array                           {29}
    08.06.2015 Add EVENT_USB_KEYBOARD_INPUT                              {30}
    12.12.2015 Modify parameter of fnSetDefaultNetwork()                 {31}
    12.12.2015 Changed PAR_DEVICE and PAR_MODBUS to respect the number of networks present {32}
    12.12.2015 usServer added for each network                           {33}
    26.06.2017 Add fnDisplayMemoryUsage()                                {34}

*/

#define SOFTWARE_VERSION                    "V1.4.011"

#if defined USE_IPV6INV4 && (defined USE_IPV6INV4_RELAY_DESTINATIONS && (USE_IPV6INV4_RELAY_DESTINATIONS != 0))
    #define PARAMETER_BLOCK_VERSION         (unsigned char)(38)
#elif defined USE_FTP_CLIENT && defined SMTP_PARAMETERS                  // {14} change parameter block version depending on project settings
    #define PARAMETER_BLOCK_VERSION         (unsigned char)(37)
#elif defined USE_FTP_CLIENT
    #define PARAMETER_BLOCK_VERSION         (unsigned char)(36)
#elif defined SMTP_PARAMETERS
    #define PARAMETER_BLOCK_VERSION         (unsigned char)(35)
#elif defined ETH_INTERFACE
    #define PARAMETER_BLOCK_VERSION         (unsigned char)(34)
#else
    #define PARAMETER_BLOCK_VERSION         (unsigned char)(33)
#endif                                                                   // a change in settings will cause default parameters to be used

#define MODBUS_PAR_BLOCK_VERSION            (unsigned char )1            // {6}

#define MY_PROJECT_NAME                     "uTasker project"

#define VALID_VERSION_MAGIC_NUMBER          0x1234                       // {3} - check used when downloading firmware via USB

extern const CHAR cSoftwareVersion[];

#if defined MODBUS_USB_SLAVE                                             // {9}
    extern QUEUE_HANDLE USBPortID_comms[USB_CDC_COUNT];                  // {29}
#endif

extern QUEUE_TRANSFER fnSendToUSB(unsigned char *ptrData, QUEUE_TRANSFER Length); // {3}
extern void fnDownload(unsigned char *ptrData, QUEUE_TRANSFER Length);   // {3}
extern CHAR *fnSkipWhiteSpace(CHAR *ptr_input);
extern void fnEstablishDataConnection(void);
extern void fnTerminateDataConnection(void);
extern void fnEstablishManualDataConnection(unsigned char *ucDialinIP, unsigned short usSerialDialinPort);
extern void fnConfigureTelnetServer(void);
extern QUEUE_HANDLE fnSetNewSerialMode(unsigned char ucDriverMode);
extern unsigned short fnGetOurParameters(int iCase);
extern void fnConfigureFtpServer(unsigned short usTimeout);
extern void fnFlushSerialRx(void);
extern void fnSaveDebugHandle(int iState);
extern void fnRestoreDebugHandle(void);
extern void fnSetUSB_debug(void);                                        // {3}
extern void fnSetBacklight(void);
extern int  fnCommandInput(unsigned char *ptrData, unsigned short usLen, int iSource);
    #define SOURCE_SERIAL      0
    #define SOURCE_NETWORK     1
    #define SOURCE_USB         2                                         // {3}

extern int  fnCheckTelnet(QUEUE_TRANSFER Length, UTASK_TASK Owner);
extern void fnConfigureAndStartWebServer(void);
extern int  fnInitiateLogin(unsigned short usNextState);
    #define DIRECT_LOGIN       0
    #define DO_PASSWORD_ENTRY  1
    #define TELNET_ON_LINE     2
    #define USB_ON_LINE        3                                         // {3}

extern void fnInitModbus(void);                                          // {7}
extern void fnGotoNextState(unsigned short usNextState);
extern void fnEchoInput(unsigned char *ucInputMessage, QUEUE_TRANSFER Length);
extern void fnSetDefaultNetwork(void);                                   // {31}
extern int  fnConfigPort(CHAR cPortBit, CHAR cType);
extern void fnSetPortOut(unsigned char ucPortOutputs, int iInitialisation);
extern int  fnTogglePortOut(CHAR cPortBit);
extern int  fnUserPortState(CHAR cPortBit);
extern int  fnPortInputConfig(CHAR cPortBit);
extern void fnSendEmail(int iRepeat);
extern void fnSavePorts(void);
#if defined USE_PARAMETER_BLOCK
    extern void fnResetChanges(void);
    extern void fnRestoreFactory(void);
#endif
extern void fnInitialisePorts(void);
extern int  fnPortState(CHAR cPortBit);
extern int  fnPortAnalogConfig(CHAR cPortBit);
extern int  fnAreWeValidating(void);
extern void fnWeHaveBeenValidated(void);
extern int  fnSaveNewPars(int iTemp);
extern CHAR *fnShowSN(CHAR *cValue);
extern void fnDisplayMemoryUsage(void);                                       // {34}
extern void fnDoLCD_com_text(unsigned char ucType, unsigned char *ptrInput, unsigned char ucLength);
extern unsigned char fnAddResetCause(CHAR *ptrBuffer);                   // {5}
extern int  fnDisplayBitmap(unsigned char *ptrData, unsigned short usLength); // {8}
extern void fnSetNewValue(int iType, CHAR *ptr_input);
    #define WAIT_BUFFER_SIZE        1
    #define WAIT_BUFFER_DELAY       2
    #define FLOW_CONTROL_HIGH_WATER 3
    #define FLOW_CONTROL_LOW_WATER  4
    #define DIAL_OUT_PORT           5
    #define DIAL_IN_PORT            6
    #define TELNET_PORT             7
extern int iGetPixelState(unsigned long ulPixelNumber);
extern void fnSendCAN_message(int iChannel, unsigned char ucType, unsigned char *ptrData, unsigned char ucMessageLength); // {15}
extern void fnSetupUDPSocket(void);                                      // {19}
extern void fnAddDiscoverySerialNumber(CHAR *ptrBuffer, int iMaxLength); // {19}
extern void fnInitialiseSNMP(void);                                      // {21}
extern void fnDisplayIP(unsigned char *ptrIP);                           // {22}
extern void fnShowLowPowerMode(void);                                    // {24}
extern void fnHandleFreeMaster(QUEUE_HANDLE comHandle, unsigned char *ptr_ucBuffer, QUEUE_TRANSFER Length);
#if defined SUPPORT_SLCD && defined STOP_WATCH_APPLICATION
    extern void fnStopWatchApplication(void);
#endif
#if defined USE_USB_AUDIO && defined AUDIO_FFT && defined BLAZE_K22
    extern void fnDisplayFFT(float *fft_magnitude_buffer, signed short *sRawInput);
#endif
#if defined FREEMASTER_UART
    extern QUEUE_HANDLE fnOpenFreeMasterUART(void);
#endif
#if defined nRF24L01_INTERFACE
    extern void fnPrepare_nRF24L01_signals(void);
    extern void fnInit_nRF24L01(void);
    extern void fnHandle_nRF24L01_event(void);
    extern void fnTest_nRF24L01_Write(int iPingPong);
    #if defined USE_MQTT_CLIENT && defined USE_MAINTENANCE
        extern unsigned char *fnSetLast_nRF24201_data(unsigned char *ptrData);
    #endif
#endif
#if defined USE_MAINTENANCE && defined USB_INTERFACE && defined USE_USB_CDC
    extern int fnUSB_CDC_TX(int iStart);
#endif

typedef struct stPARS
{
    unsigned char  ucParVersion;                                         // version number to avoid loading incorrect structure after major changes
    unsigned short telnet_timeout;                                       // TELNET timeout in seconds (0xffff means no timeout) - max 18 hours
    UART_MODE_CONFIG SerialMode;                                         // serial settings
    unsigned short usTelnetPort;                                         // the port number of our TELNET service
    unsigned short usServers[IP_NETWORK_COUNT];                          // {11}{33} active servers
    unsigned char  ucSerialSpeed;                                        // Baud rate of serial interface
    unsigned char  ucTrustedIP[IPV4_LENGTH];                             // Trusted IP address
    CHAR           cUserName[8];                                         // The single user administrator
    CHAR           cUserPass[8];                                         // The administrator's password
    CHAR           cDeviceIDName[21];                                    // 20 characters for recognition plus null terminator
    unsigned char  ucFlowHigh;                                           // High water flow level for XOFF/CTS (%)
    unsigned char  ucFlowLow;                                            // Low water flow level for XON/CTS (%)
    unsigned char  ucUserOutputs;                                        // ports set as outputs
    unsigned char  ucUserOutputValues;                                   // port values    
    unsigned short usUserDefinedOutputs;                                 // second set of user defined outputs
#if !defined ETH_INTERFACE
    unsigned long  ulSerialNumber;                                       // serial number (when Ethernet is enabled it is derived from the MAC address instead)
#endif
#if defined USE_SNTP || defined USE_TIME_SERVER
    unsigned char  ucTimeZoneFlags;
#endif
#if defined USE_SNTP
    unsigned char  ucSNTP_server_ip[SNTP_SERVERS][IPV4_LENGTH];
#endif
#if defined USE_TIME_SERVER
    unsigned char  ucTime_server_ip[SNTP_SERVERS][IPV4_LENGTH];
#endif
#if defined SMTP_PARAMETERS                                              // {1}
    unsigned char  ucSMTP_user_name[23];                                 // SMTP user's name plus terminator
    unsigned char  ucSMTP_password[11];                                  // SMTP user's password plus terminator
    unsigned char  ucSMTP_user_email[31];                                // SMTP user's email address plus terminator
    unsigned char  ucSMTP_server[31];                                    // SMTP server address plus terminator
    unsigned char  ucSMTP_server_ip[IPV4_LENGTH];                        // SMTP server IP address - either fixed or resolved by DNS
#endif
#if defined LCD_CONTRAST_CONTROL
    unsigned char  ucGLCDContrastPWM;                                    // contast percentage value
#endif
#if defined GLCD_BACKLIGHT_CONTROL
    unsigned char  ucGLCDBacklightPWM;                                   // intensity percentage value
#endif
#if defined SUPPORT_TOUCH_SCREEN
    signed short   sTouchXminCal;
    signed short   sTouchXmaxCal;
    signed short   sTouchYminCal;
    signed short   sTouchYmaxCal;
#endif
#if defined USE_FTP_CLIENT                                               // {14}
    CHAR           cFTPUserName[16 + 1];                                 // the default FTP user name (up to 16 characters plus null terminator)
    CHAR           cFTPUserPass[16 + 1];                                 // the default FTP user password (up to 16 characters plus null terminator)
    unsigned short usFTPcommandPort;                                     // default FTP command port number (normally 21)
    unsigned short ftp_idle_seconds;                                     // default FTP idle timeout
    unsigned char  ucFTP_server_IP[IPV4_LENGTH];                         // default FTP server IP address
    #if defined USE_IPV6
    unsigned char  ucFTP_server_IPv6[IPV6_LENGTH];                       
    #endif
#endif
#if defined USE_IPV6INV4 && (defined USE_IPV6INV4_RELAY_DESTINATIONS && (USE_IPV6INV4_RELAY_DESTINATIONS != 0))
    IPV6INV4_RELAY_DESTINATION relay_destination[USE_IPV6INV4_RELAY_DESTINATIONS]; // {16}
#endif
#if defined DUSK_AND_DAWN
    LOCATION ourLocation;                                                // our geographical coordinates
#endif
#if defined USE_USB_HID_KEYBOARD && defined USB_KEYBOARD_DELAY
    unsigned char ucKeyboardInterCharacterDelay;                         // minimum inter-character delay between keyboard inputs (ms)
#endif
} PARS;


typedef struct stTEMPPARS
{
#if defined ETH_INTERFACE || defined USB_CDC_RNDIS || defined USE_PPP
    NETWORK_PARAMETERS temp_network[IP_NETWORK_COUNT];                   // {17}
#endif
    PARS               temp_parameters;
#if defined USE_MODBUS                                                   // {4}
    MODBUS_PARS        modbus_parameters;
#endif
} TEMPPARS;


                                                                         // parameters
#define PAR_SERIAL_NUMBER 0                                              // location of serial number
#define PAR_NETWORK (PAR_SERIAL_NUMBER + sizeof(unsigned long))          // location of network parameters
#define PAR_DEVICE  (PAR_NETWORK + (sizeof(NETWORK_PARAMETERS) * IP_NETWORK_COUNT)) // {32} location of the device parameter block
#define PAR_MODBUS  (PAR_NETWORK + (sizeof(NETWORK_PARAMETERS) * IP_NETWORK_COUNT) + sizeof(PARS)) // {4}{32} location of the MODBUS parameter block


// ucTransferMode defines
//
#define AUTO_SERIAL_TO_ETHERNET         0x01
#define ACTIVE_ESCAPE_SEQUENCE_SCANNING 0x02
#define AUTO_ETHERNET_TO_SERIAL         0x04
#define RAW_SOCKET_MODE                 0x08
#define ALLOW_SERIAL_ETHERNET_DIALIN    0x10
#define ALLOW_TELNET_DIALOUT            0x20
#define TELNET_ESCAPE_SEQUENCE_SCANNING 0x80

// ucRAWMode defines
//
#define RX_TREATS_IAC                   0x01
#define TX_TREATS_IAC                   0x02

// Server defines
//
#define SMTP_LOGIN                      0x0001
#define ACTIVE_DHCP                     0x0002                           // not a local server but means get info from a DHCP server
#define ACTIVE_FTP_SERVER               0x0004
#define ACTIVE_WEB_SERVER               0x0008
#define ACTIVE_TELNET_SERVER            0x0010
#define AUTHENTICATE_WEB_SERVER         0x0020
#define ACTIVE_LOGIN                    0x0040
#define ACTIVE_FTP_LOGIN                0x0080
#define ACTIVE_DHCPV6                   0x0100                           // {11}
#define PASSIVE_MODE_FTP_CLIENT         0x0200                           // {14} active mode rather than passive mode
#define ACTIVE_SNMP_0                   0x0400
#define ACTIVE_SNMP_1                   0x0800
#define ACTIVE_SNMP_2                   0x1000
#define ACTIVE_TIME_SERVER              0x2000
#define ACTIVE_SNTP                     0x4000
#define SNTP_BROADCAST                  0x8000

#if defined USE_PARAMETER_BLOCK
    extern PARS *parameters;
#endif
extern const PARS cParameters;
extern TEMPPARS *temp_pars;
extern NETWORK_PARAMETERS network_flash[IP_NETWORK_COUNT];
extern QUEUE_HANDLE SerialPortID;
extern CHAR cEmailAdd[41];
extern unsigned short usEthernet_Serial_state;
extern unsigned short usData_state;
extern unsigned short usUSB_state;                                       // {3}
extern int            iFTP_data_state;                                   // {14}
    #define FTP_DATA_STATE_CONNECTED 0x01
    #define FTP_DATA_STATE_GETTING   0x02
    #define FTP_DATA_STATE_PUTTING   0x04
    #define FTP_DATA_STATE_PAUSE     0x08
    #define FTP_DATA_STATE_CRITICAL  0x10
    #define FTP_DATA_STATE_CLOSING   0x20
    #define FTP_DATA_STATE_SENDING   0x40

extern int iLeftTilt;                                                    // {23} variables controlled by accelerometer and used by USB HID tilt-mouse
extern int iRightTilt;
extern int iUpTilt;
extern int iDownTilt;
extern int iAccelOutput;

#if defined USE_USB_HID_KEYBOARD && defined SUPPORT_FIFO_QUEUES
    extern QUEUE_HANDLE keyboardQueue;
#endif
#if defined MODBUS_DELAYED_RESPONSE && defined USE_MODBUS
    extern unsigned short usDelayedReference;
#endif
#if defined USE_TIME_SERVER
    extern unsigned long ulPresentTime;
#endif
#if defined USE_SMTP
    extern unsigned char ucSMTP_server[IPV4_LENGTH];
#endif
#if defined USB_INTERFACE && defined USE_USB_AUDIO
    extern signed long slDelta;                                          // present USB audio (host/audio deviation from ideal delay)
#endif

#define PASSWORD_IDLE             0
#define START_PASSWORD            1
#define ENTER_USER_NAME           2
#define ENTER_NEW_PASSWORD        3
#define CONFIRM_NEW_PASS          4
#define TELNET_LOGIN              5

// Ethernet - Serial modes
//
#define ES_NO_CONNECTION          0x0000
#define ES_CONNECTING_SERVER      0x0001
#define ES_CONNECTING_CLIENT      0x0002
#define ES_NETWORK_COMMAND_MODE   0x0004
#define ES_DATA_MODE              0x0008
#define ES_TERMINATE_CONNECTION   0x0010
#define ES_SERIAL_COMMAND_MODE    0x0020
#define ES_STARTING_COMMAND_MODE  0x0040
#define ES_BINARY_DATA_MODE       0x0080

#define ES_SERIAL_LOGIN           0x0100
#define ES_NETWORK_LOGIN          0x0200
#define ES_SERIAL_SUSPENDED       0x0400
#define ES_USB_LOGIN              0x0800                                 // {3}
#define ES_USB_COMMAND_MODE       0x1000                                 // {3}
#define ES_USB_DOWNLOAD_MODE      0x2000                                 // {3}
#define ES_USB_RS232_MODE         0x4000                                 // {3}
#define ES_USB_KEYBOARD_MODE      0x8000                                 // {27}


#define SAVE_NEW_PARAMETERS                0
#define SAVE_NEW_PARAMETERS_VALIDATE       1
#define SAVE_NEW_PARAMETERS_CHECK_CRITICAL 2

#define WARN_BEFORE_SAVE   0                                             // the web page warning of critical changes - deactivated
#define BAD_PASSWORD_PAGE  0                                             // the web page informing of bad passowrd entry - deactivated

#define FTP_TIMEOUT               60                                     // FTP connection times out after this delay with no activity {2}

// LCD events
// Application to LCD
//
#define E_LCD_COMMAND             1
#define E_LCD_TEXT                2
#define E_LCD_PATTERN             3
#define E_LCD_READ_ADDRESS        4
#define E_LCD_READ_RAM            5
#define E_LCD_COMMAND_TEXT        6
#define E_LCD_PIC                 7
#define E_LCD_LINE                8                                      // {10}
#define E_LCD_RECT                9                                      // {10}
#define E_LCD_SCROLL              10                                     // {10}
#define E_LCD_STYLE               11                                     // {12}

// LDC to application
//
#define E_LCD_INITIALISED         1
#define E_LCD_READY               2
#define E_LCD_READ                3
#define E_LCD_ERROR               4
#define E_TOUCH_MOUSE_EVENT       5

// USB to mass storage
//
#define MOUNT_USB_MSD             1                                      // {20}
#define USB_MSD_REMOVED           2

// Application to USB
//
#define EVENT_USB_KEYBOARD_INPUT  1                                      // {30}

// Key pad events
// Keypad to application - sent as interrupt events!!
//
#define KEY_EVENT_COL_1_ROW_1_PRESSED  1
#define KEY_EVENT_COL_1_ROW_1_RELEASED 2
#define KEY_EVENT_COL_1_ROW_2_PRESSED  3
#define KEY_EVENT_COL_1_ROW_2_RELEASED 4
#define KEY_EVENT_COL_1_ROW_3_PRESSED  5
#define KEY_EVENT_COL_1_ROW_3_RELEASED 6
#define KEY_EVENT_COL_1_ROW_4_PRESSED  7
#define KEY_EVENT_COL_1_ROW_4_RELEASED 8

#define KEY_EVENT_COL_2_ROW_1_PRESSED  9
#define KEY_EVENT_COL_2_ROW_1_RELEASED 10
#define KEY_EVENT_COL_2_ROW_2_PRESSED  11
#define KEY_EVENT_COL_2_ROW_2_RELEASED 12
#define KEY_EVENT_COL_2_ROW_3_PRESSED  13
#define KEY_EVENT_COL_2_ROW_3_RELEASED 14
#define KEY_EVENT_COL_2_ROW_4_PRESSED  15
#define KEY_EVENT_COL_2_ROW_4_RELEASED 16

#define KEY_EVENT_COL_3_ROW_1_PRESSED  17
#define KEY_EVENT_COL_3_ROW_1_RELEASED 18
#define KEY_EVENT_COL_3_ROW_2_PRESSED  19
#define KEY_EVENT_COL_3_ROW_2_RELEASED 20
#define KEY_EVENT_COL_3_ROW_3_PRESSED  21
#define KEY_EVENT_COL_3_ROW_3_RELEASED 22
#define KEY_EVENT_COL_3_ROW_4_PRESSED  23
#define KEY_EVENT_COL_3_ROW_4_RELEASED 24

#define KEY_EVENT_COL_4_ROW_1_PRESSED  25
#define KEY_EVENT_COL_4_ROW_1_RELEASED 26
#define KEY_EVENT_COL_4_ROW_2_PRESSED  27
#define KEY_EVENT_COL_4_ROW_2_RELEASED 28
#define KEY_EVENT_COL_4_ROW_3_PRESSED  29
#define KEY_EVENT_COL_4_ROW_3_RELEASED 30
#define KEY_EVENT_COL_4_ROW_4_PRESSED  31
#define KEY_EVENT_COL_4_ROW_4_RELEASED 32

//..... up to KEY_EVENT_COL_16_ROW_16_RELEASED 128!!

// Application events (timer and interrupt
//
#define E_TIMER_1                  1                                     // local events
#define E_TIMER_SEND_ETHERNET      2
#define E_TIMER_VALIDATE           3
#define E_TIMER_FREE_SCAN          4
#define E_TIMER_FREE_SCAN_SERIAL   5
#define E_QUIT_SERIAL_COMMAND_MODE 6
#define E_DEBUG                    7
#define E_SENT_TEST_EMAIL          8
#define E_SHIFT_DISPLAY            9
#define E_NEXT_PIC                 10
#define E_NEXT_PHOTO               11
#define E_TIMER_START_USB_TX       12

#define E_TIMER_TEST_10S           20
#define E_TIMER_TEST_10MS          E_TIMER_TEST_10S                      // use same event number as 10s timer
#define E_TIMER_TEST_3S            21
#define E_TIMER_TEST_3MS           E_TIMER_TEST_3S                       // use same event number as 3s timer
#define E_TIMER_TEST_5S            22
#define E_TIMER_TEST_5MS           E_TIMER_TEST_5S                       // use same event number as 5s timer
#define E_TIMER_TEST_4S            23
#define E_TIMER_TEST_4MS           E_TIMER_TEST_4S                       // use same event number as second 4s timer
#define IRQ1_EVENT                 40
#define IRQ4_EVENT                 41
#define IRQ5_EVENT                 42
#define IRQ7_EVENT                 43
#define IRQ11_EVENT                44
#define CAPTURE_COMPLETE_EVENT     45

#define E_TIMER_TX_NOW             60

#define ADC_HIGH_0                 70
#define ADC_HIGH_1                 71
#define ADC_HIGH_2                 72
#define ADC_HIGH_3                 73
#define ADC_HIGH_4                 74
#define ADC_HIGH_5                 75
#define ADC_HIGH_6                 76
#define ADC_HIGH_7                 77
#define ADC_LOW_0                  78
#define ADC_LOW_1                  79
#define ADC_LOW_2                  80
#define ADC_LOW_3                  81
#define ADC_LOW_4                  82
#define ADC_LOW_5                  83
#define ADC_LOW_6                  84
#define ADC_LOW_7                  85
#define ADC_ZERO_CROSS_0           86
#define ADC_ZERO_CROSS_1           87
#define ADC_ZERO_CROSS_2           88
#define ADC_ZERO_CROSS_3           89
#define ADC_ZERO_CROSS_4           90
#define ADC_ZERO_CROSS_5           91
#define ADC_ZERO_CROSS_6           92
#define ADC_ZERO_CROSS_7           93
#define ADC_TRIGGER                94

#define ADC_TRIGGER_5_ZERO         100
#define ADC_TRIGGER_6_ZERO         101
#define ADC_TRIGGER_5_LOW          102
#define ADC_TRIGGER_6_LOW          103
#define ADC_TRIGGER_5_HIGH         104
#define ADC_TRIGGER_6_HIGH         105

#define E_NEXT_SAMPLE              108

#define ADC_TRIGGER_0              110
#define ADC_TRIGGER_1              111
#define ADC_TRIGGER_2              112
#define ADC_TRIGGER_3              113
#define ADC_TRIGGER_4              114
#define ADC_TRIGGER_5              115
#define ADC_TRIGGER_6              116
#define ADC_TRIGGER_7              117

#define E_NEXT_SSC_TEST            120
#define E_NEXT_TIME_SYNC           121

#define E_NEXT_SENSOR_REQUEST      122

#define E_SERVE_PAGE               130                                   // global application events
#define E_TIMER_SW_DELAYED_RESET   131
#define E_TEST_MODBUS_DELAY        133
#define E_nRF24L01_PERIOD          134
#define E_nRF24L01_EVENT           135

#define E_USB_TX_CONTINUE          136



