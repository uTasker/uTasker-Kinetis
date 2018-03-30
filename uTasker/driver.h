/**********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher

    ---------------------------------------------------------------------
    File:      driver.h
    Project:   Single Chip Embedded Internet
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2018
    *********************************************************************
    01.03.2007 Added uCompareFile()
    19.05.2007 Add FLASH protection functions fnProtectFile(), fnUnprotectFile()
    24.05.2007 Add fnGetFileType()
    12.10.2007 Add USB table and fnOpenUSB()                             {1}
    12.10.2007 uMallocAlign() align value changed from unsigned char to unsigned short {2}
    29.10.2007 Add ENABLE_CTS_CHANGE, DISABLE_CTS_CHANGE and GET_CONTROL_STATUS defines
    17.11.2007 FILE_HEADER declaration moved here from config.h          {3}
    20.12.2007 fnInUserInterrupt()                                       {4}
    09.06.2008 uNetwork Broadcast address made definable                 {6}
    03.07.2008 Add fnGetUSBEndpoint()                                    {7}
    19.09.2008 Add USB host and OTG mode defines                         {8}
    18.10.2008 Add fnFillBuf_FIFO()                                      {9}
    31.10.2008 Add serial RTU_RX_MODE                                    {10}
    27.12.2008 Add lastDMA_block_length to TTY struct when using DMA     {11}
    11.01.2009 Add BUFFER_CONSUMED_EXPECT_MORE                           {12}
    21.01.2009 Add fnFilterUserFile() and fnFilterUserFileLength() - removed 07.04.2009 {13}
    24.02.2009 Add CONFIG_RTS_PIN, CONFIG_CTS_PIN, SET_RS485_MODE        {14}
    24.02.2009 TTYTABLE configuration extended to configurable tye. TTYQUE opn_mode also changed to same type {15}
    24.02.2009 Add INFORM_ON_FRAME_TRANSMISSION                          {16}
    07.04.2009 Add fnGetUserMimeType(), fnEnterUserFiles() and USER_FILE struct {17}
    16.04.2009 Add WITH_CR_LF flag for use with string functions         {18}
    26.05.2009 Add uMemset_long() and uMemcpy_long()                     {19}
    10.06.2009 DCE_MODE and DTE_MODE removed (replaced by INFORM_ON_FRAME_TRANSMISSION) {20}
    01.07.2009 Standardise string routines when STRING_OPTIMISATION is defined {21}
    16.08.2009 Add SSCTABLE, TYPE_SSC, SSCQUE and fnOpenSSC()            {22}
    11.09.2009 Add MULTIDROP_MODE                                        {23}
    15.10.2009 Add optional tty tx wakup level                           {24}
    13.11.2009 Move structure packing control and little-/big-endian macros to this file {25}
    11.12.2009 Add additional ASCII character defines                    {26}
    23.12.2009 Correct LITTLE_LONG_WORD() macro for big-endian processors{27}
    02.01.2010 Add UTFAT_OPERATION_COMPLETED                             {28}
    02.02.2010 Add optional uFree()                                      {29}
    21.03.2010 Add USB_ENDPOINT_TERMINATES, TRANSPARENT_CALLBACK, ENDPOINT_REQUEST_TYPE and ENDPOINT_CLEARED {30}
    31.03.2010 Add ucParameters and ucPaired_IN to USB_ENDPOINT and mode USB_TERMINATING_ENDPOINT {31}
    02.04.2010 Add fnGetPairedIN()                                       {32}
    05.04.2010 Add USB_ALT_PORT_MAPPING                                  {33}
    20.06.2010 Add CRITICAL_OUT                                          {34}
    23.09.2010 Activate msgchars when serial DMA is used                 {35}
    10.10.2010 Add SET_RS485_NEG                                         {36}
    30.10.2010 Add uOpenUserFile()                                       {37}
    10.06.2011 Add MULTIDROP_MODE_TX                                     {38}
    09.07.2011 Add ZERO_CONFIG_SUCCESSFUL, ZERO_CONFIG_DEFENDED and ZERO_CONFIG_COLLISION {39}
    27.08.2011 Add EXTENDED_UFILESYSTEM support                          {40}
    07.10.2011 Add UFILESYSTEM_APPEND support                            {41}
    04.11.2011 Add fnGetQueueHandle()                                    {42}
    07.11.2011 Add Additional key codes                                  {43}
    06.12.2011 Add CALL_PEEK, fnPeekInput() and fnGetBufPeek() - remove fnScan() {44}
    07.12.2011 Add Additional ASCII codes                                {45}
    03.01.2012 Add additional CAN modes                                  {46}
    09.02.2012 Add reserved count values to QUEQUE                       {47}
    19.04.2012 Add iFetchingInternalMemory                               {48}
    18.06.2012 Add Ethernet option PASS_CRC32_RX                         {49}
    16.12.2012 Modify Ethernet events to include duplex information      {50}
    04.02.2013 Add USB_HIGH_SPEED and adjust USB flags                   {51}
    29.05.2013 Add optional second heap in fixed sized memory area       {52}
    06.08.2013 Add fnUserWrite()                                         {53}
    12.09.2013 Add uReverseMemcpy()                                      {54}
    07.01.2014 Correct BIG_LONG_WORD() macro for little-endian processors{55}
    04.02.2014 Add INITIATE_IN_TOKEN and USB_HOST_ERROR                  {56}
    04.02.2014 Add USB host detection and removal events                 {57}
    16.04.2014 Add multicast filter actions                              {58}
    22.08.2014 Add uMask_Interrupt()                                     {59}
    08.02.2015 Add time keeping prototypes                               {60}
    01.03.2015 Add TYPE_FIFO                                             {61}
    04.04.2015 Add Ethernet option USE_FACTORY_MAC and flag NETWORK_VALUES_FIXED {62}
    08.06.2015 Add IN_COMPLETE_CALLBACK entries for USB device           {63}
    10.06.2015 Moved struct packing control to processor header
    28.06.2015 Add fnDebugFloat()                                        {64}
    24.09.2015 Add I2C slave mode support                                {65}
    05.11.2015 Change DISPLAY_NEGATIVE to avoid conflict with NO_TERMINATOR {66}
    23.12.2015 Add USB zero copy options                                 {67}
    14.01.2016 Add LITTLE_SHORT_24BIT_BYTES()                            {68}
    09.07.2016 Move network interrupt events to Ethernet events          {69}
    10.07.2016 Add fnFloatStrFloat()                                     {70}
    05.01.2017 Add optional midi baud rate                               {71}
    07.01.2017 Add UART_TIMED_TRANSMISSION_MODE                          {72}
    16.02.2017 Add crypography AES defines                               {73}
    17.01.2017 Add DSP FFT defines                                       {74}
    03.05.2017 Add UART_RX_MODULO and UART_TX_MODULO flags               {75}
    09.05.2017 Add PAUSE_TX                                              {76}
    26.11.2017 Added FOREVER_LOOP()                                      {77}
    29.11.2017 Add UART_INVERT_TX                                        {78}
    16.12.2017 Add optional uCalloc() and uCFree()                       {79}
    17.12.2017 Change uMemset() to match memset() parameters             {80}
    13.03.2018 Add UART_IDLE_LINE_INTERRUPT                              {81}
    16.03.2018 Add CONTROL_QUESTION_MARK                                 {82}

*/

#if !defined __DRIVER__
#define __DRIVER__


/* =================================================================== */
/*                           global defines                            */
/* =================================================================== */

#define FOREVER_LOOP()    while ((int)1 != (int)0)                       // {77}

// Queues and physical queues supported
//
#define TYPE_TTY          (unsigned char)0
#define TYPE_ARCNET       (unsigned char)1
#define TYPE_EEPROM       (unsigned char)2
#define TYPE_QUEUE        (unsigned char)3
#define TYPE_ETHERNET     (unsigned char)4
#define TYPE_USB          (unsigned char)5
#define TYPE_CAN          (unsigned char)6
#define TYPE_SPI          (unsigned char)7
#define TYPE_I2C          (unsigned char)8
#define TYPE_SSC          (unsigned char)9                               // {22}
#define TYPE_FIFO         (unsigned char)10                              // {61}



// Call types
//
#define CALL_OPEN         0
#define CALL_CLOSE        1
#define CALL_DRIVER       2
#define CALL_WRITE        3
#define CALL_READ         4
#define CALL_INPUT        5
#define CALL_FLUSH        6
#define CALL_READ_LINEAR  7
#define CALL_FREE         8
#define CALL_SCAN         9
#define CALL_STATS        10
#define CALL_PEEK_OLDEST  11                                             // {44}
#define CALL_PEEK_NEWEST  12


// Open modes
//
#define FOR_READ          (unsigned char)0x01
#define FOR_WRITE         (unsigned char)0x02
#define FOR_I_O           (unsigned char)(FOR_READ | FOR_WRITE)
#define MODIFY_CONFIG     (unsigned char)0x04


// Driver commands - on a bit basis
//
#define RX_ON             0x01                                           // turn receiver on
#define SET_RTS           0x01
#define ENABLE_DSR_CHANGE 0x01

#define RX_OFF            0x02                                           // turn receiver off
#define CLEAR_RTS         0x02
#define DISABLE_DSR_CHANGE 0x02

#define TX_ON             0x04                                           // turn transmitter on
#define ENABLE_CTS_CHANGE 0x04

#define SET_DTR           0x04
#define TX_OFF            0x08                                           // turn transmitter off
#define CLEAR_DTR         0x08
#define DISABLE_CTS_CHANGE 0x08

#define ECHO_ON           0x10                                           // turn echo (rx->tx) on
#define SET_RING          0x10
#define ECHO_OFF          0x20                                           // turn echo off
#define CLEAR_RING        0x20
#define MODIFY_WAKEUP     0x40                                           // set a new owner task to wake on messages
#define MODIFY_CONTROL    0x80
#define SET_CTS           0x100
#define CLEAR_CTS         0x200
#define MODIFY_INTERRUPT  0x400
#define GET_CONTROL_STATUS 0x400
#define SET_MSG_CNT_MODE  0x800
#define CONFIG_RTS_PIN    0x800                                          // {14}
#define SET_CHAR_MODE     0x1000
#define CONFIG_CTS_PIN    0x1000                                         // {14}
#define SET_RS485_MODE    0x2000                                         // {14}
#define SET_RS485_NEG     0x4000                                         // {36}
#define PAUSE_TX          0x8000                                         // {76}

#define MODIFY_TX         0x1000
#define MODIFY_RX         0x0000


// Serial interface ASCII character defines
//
#define ASCII_NUL                    0x00                                // {45}
#define ASCII_SOH                    0x01
#define ASCII_STX                    0x02
#define ASCII_ETX                    0x03
#define ASCII_EOT                    0x04
#define ASCII_ENQ                    0x05
#define ASCII_ACK                    0x06
#define ASCII_BEL                    0x07
#define DELETE_KEY                   0x08                                // back space (control-H)
#define ASCII_HT                     0x09
#define LINE_FEED                    0x0a
#define ASCII_VT                     0x0b
#define ASCII_FF                     0x0c
#define CARRIAGE_RETURN              0x0d
#define ASCII_SO                     0x0e
#define TAB_KEY                      0x0f                                // SI
#define ASCII_DLE                    0x10
#define XON_CODE                     0x11                                // DC1
#define ASCII_DC2                    0x12
#define XOFF_CODE                    0x13                                // DC3
#define ASCII_DC4                    0x14
#define ASCII_NAK                    0x15
#define ASCII_SYN                    0x16
#define ASCII_ETB                    0x17
#define ESCAPE_KEY                   0x18                                // CAN
#define ASCII_EM                     0x19
#define ASCII_SUB                    0x1a
#define ESCAPE_SEQUENCE_START        0x1b                                // {43} ESC
#define ASCII_FS                     0x1c
#define ASCII_GS                     0x1d
#define ASCII_RS                     0x1e
#define ASCII_US                     0x1f
#define FORWARD_SLASH                0x2f
#define ARROW_UP_SEQUENCE            0x41                                // {43}
#define ARROW_DOWN_SEQUENCE          0x42                                // {43}
#define ARROW_RIGHT_SEQUENCE         0x43                                // {43}
#define ARROW_LEFT_SEQUENCE          0x44                                // {43}
#define ESCAPE_ARROWS                0x5b                                // {43}
#define BACK_SLASH                   0x5c                                // {26}
#define CONTROL_QUESTION_MARK        0x7f                                // {82} control-? (this is used by putty as back space if not set to control-H)


// Serial configuration
//
#define CHAR_MODE                    0x0000                              // character configuration
#define CHAR_8                       0x0000
#define NO_PARITY                    0x0000
#define ONE_STOP                     0x0000
#define NO_HANDSHAKE                 0x0000
#define CHAR_7                       0x0001
#define RS232_EVEN_PARITY            0x0002
#define RS232_ODD_PARITY             0x0004
#define ONE_STOP                     0x0000
#define ONE_HALF_STOPS               0x0008
#define TWO_STOPS                    0x0010

#define USE_XON_OFF                  0x0020                              // flow control modes
#define RTS_CTS                      0x0040

#define MSG_MODE                     0x0080                              // frames are grouped as messages with terminator
#define MSG_MODE_RX_CNT              0x0100                              // message mode with content counter added to input buffer
#define RTU_RX_MODE                  0x0200                              // {10} RTU mode, with inter-character space timer
#define TX_ESC_MODE                  0x0400                              // automatic character escaping in transmission direction
#define RX_ESC_MODE                  0x0800                              // automatic character escaping in reception direction
#define ECHO_RX_CHARS                0x1000                              // automatically echo received characters
//#define DCE_MODE                   0x2000                              // {20} the interface simulates a modem for example
//#define DTE_MODE                   0x0000                              // generally the interface works in this mode
#define INFORM_ON_FRAME_TRANSMISSION 0x2000                              // {16} transmitter informs on transmission completion (no more data waiting) in output buffer
#define BREAK_AFTER_TX               0x4000                              // send a break once the tx buffer has emptied
#define MSG_BREAK_MODE               0x8000                              // message framing using break
#if defined UART_EXTENDED_MODE
    // The following options are available when UART_EXTENDED_MODE is enabled
    //
    #define MULTIDROP_MODE_RX            0x00010000                      // {23} extended mode - also known as 9-bit mode (reception)
    #define MULTIDROP_MODE_TX            0x00020000                      // {38} extended mode - also known as 9-bit mode (transmission)
    #define UART_TIMED_TRANSMISSION_MODE 0x00040000                      // {72}
    #define UART_INVERT_TX               0x00080000                      // {78}
    #define UART_IDLE_LINE_INTERRUPT     0x00100000                      // {81}
#endif

#define UART_TX_DMA                  0x01                                // UART uses DMA for transmission
#define UART_RX_DMA                  0x02                                // UART uses DMA for reception
#define UART_RX_DMA_FULL_BUFFER      0x04                                // DMA complete when rx buffer full
#define UART_RX_DMA_HALF_BUFFER      0x08                                // DMA complete when rx buffer half full
#define UART_RX_DMA_BREAK            0x10                                // DMA complete when break detected
#define UART_RX_MODULO               0x20                                // {75} reception memory must be moduo aligned
#define UART_TX_MODULO               0x40                                // {75} transmission memory must be moduo aligned

#define FLUSH_RX          0
#define FLUSH_TX          1

// Pre-defined Baud rates for serial interface
//
#define SERIAL_BAUD_300      0
#define SERIAL_BAUD_600      1
#define SERIAL_BAUD_1200     2
#define SERIAL_BAUD_2400     3
#define SERIAL_BAUD_4800     4
#define SERIAL_BAUD_9600     5
#define SERIAL_BAUD_14400    6
#define SERIAL_BAUD_19200    7
#if defined SUPPORT_MIDI_BAUD_RATE                                       // {71}
    #define SERIAL_BAUD_31250    8                                       // midi baud rate
    #define SERIAL_BAUD_38400    9
    #define SERIAL_BAUD_57600    10
    #define SERIAL_BAUD_115200   11
    #define SERIAL_BAUD_230400   12
    #define SERIAL_BAUD_250K     13
#else
    #define SERIAL_BAUD_38400    8
    #define SERIAL_BAUD_57600    9
    #define SERIAL_BAUD_115200   10
    #define SERIAL_BAUD_230400   11
    #define SERIAL_BAUD_250K     12
#endif


// Serial states
//
#define SEND_XON          0x01
#define SEND_XOFF         0x02
#define ESCAPE_SEQUENCE   0x04
#define XON_XOFF          0x08
#define RX_HIGHWATER      0x10
#define TX_WAIT           0x20
#define TX_ACTIVE         0x40
#define RX_ACTIVE         0x80

// USB shared states
//
#define USB_ENDPOINT_ACTIVE          0x0001
#define USB_ENDPOINT_SUSPENDED       0x0002
#define USB_ENDPOINT_BLOCKED         0x0004
#define USB_ENDPOINT_STALLED         0x0008
#define USB_CONTROL_ENDPOINT         0x0010
#define USB_ENDPOINT_TERMINATES      0x0020                              // {30}
#define USB_ENDPOINT_ZERO_COPY_IN    0x0040                              // {67}
#define USB_ENDPOINT_ZERO_COPY_OUT   0x0080                              // {67}
#define USB_ENDPOINT_IN_MESSAGE_MODE 0x0100

// I2C shared states
//
#define RX_ACTIVE_FIRST_READ        SEND_XON
#define I2C_SLAVE_TX_BUFFER_MODE    SEND_XOFF
#define I2C_SLAVE_RX_MESSAGE_MODE   SEND_XOFF                            // {65}

// I2C speeds
//
#define I2C_SLAVE_SPEED   0x00
#define I2C_100K          0x01


// I2C modes
//
#define I2C_MASTER        0x08

// SPI interface defines
//
#define SPI_SLAVE         0x00
#define SPI_MASTER        0x80
#define SPI_TRANSPARENT   0x33

// SPI speed defines
//
#define MAX_SPI           1
#define SPI_8MEG          2
#define SPI_4MEG          3
#define SPI_2MEG          4
#define SPI_1MEG          5
#define SPI_100K          6

// Ethernet interface modes
//
#define AUTO_NEGOTIATE    0x0000
#define LAN_10M           0x0001
#define LAN_100M          0x0002
#define LAN_LEDS          0x0004
#define FULL_DUPLEX       0x0008
#define RX_FLOW_CONTROL   0x0010
#define PROMISCUOUS       0x0020
#define NO_BROADCAST      0x0040
#define CON_MULTICAST     0x0080
#define LOOPBACK          0x0100
#define EXTERNAL_PHY_USED 0x0200
#define PASS_CRC32_RX     0x0400                                         // {49}
#define USE_FACTORY_MAC   0x0800                                         // {62}
#define NETWORK_VALUES_FIXED 0x8000

// CAN interface modes
//
#define CAN_LOOPBACK      0x0001
#define CAN_CRYSTAL_CLOCK 0x0000                                         // {46} crystal oscillator clock has lowest jitter and is preferred when possible
#define CAN_PLL_CLOCK     0x0002                                         // {46} PLL clock may be more flexible in some cases
#define CAN_USER_SETTINGS 0x0004                                         // {46} the user passes the native speed settings directly rather than the speed in Hz


// CAN message controls
//
#define DEFAULT_MSG       0x00
#define SPECIFIED_ID      0x80
#define GET_CAN_TX_ERROR  0x80
#define TX_REMOTE_FRAME   0x40                                           // transmit data in response to a remote frame
#define GET_CAN_RX_ID     0x40
#define TX_REMOTE_STOP    0x20
#define GET_CAN_RX_TIME_STAMP 0x20
#define GET_CAN_RX_REMOTE 0x10
#define CAN_TX_ACK_ON     0x10                                           // ack when tx successful
#define GET_CAN_TX_REMOTE_ERROR 0x08
#define FREE_CAN_RX_REMOTE 0x04
#define CAN_TX_MSG_MASK   0xf0


// CAN reception frames
//
#define CAN_RX_OVERRUN    0x01
#define CAN_MSG_RX        0x02
#define CAN_REMOTE_MSG_RX 0x04

// CAN buffer type
//
#define CAN_RX_BUF        0x00
#define CAN_TX_BUF        0x01
#define CAN_TX_BUF_FREE   0x04
#define CAN_TX_BUF_ACTIVE 0x08
#define CAN_ACK_TX_SUCCESS 0x10                                          // warning  must match with CAN_TX_ACK_ON value!!
#define CAN_RX_BUF_FULL   0x20
#define CAN_TX_BUF_REMOTE 0x40
#define CAN_RX_REMOTE_RX  0x80                                           // tx buffer sending remote frame and converting to temporary receiver afterwards

#define CAN_EXTENDED_ID_BYTE  0x80                                       // mark buffer uses extended CAN ID (when writing single bytes - MSB)
#define CAN_EXTENDED_ID  0x80000000                                      // mark buffer uses extended CAN ID

// Possible Ethernet types to be accepted
//
#define ALL_ETYPES        0x00                                           // accept all frame types
#define EMWARE            0x01                                           // accept specific frame types
#define IPV6              0x02
#define ARP               0x04
#define IPV4              0x08
#define IEEE              0x10


#define NETWORK_HANDLE (QUEUE_HANDLE)(0 - 1)                             // -1 is the network handle


/* =================================================================== */
/*                           error codes                               */
/* =================================================================== */

#define NO_ID_ALLOCATED   0
#define NO_MEMORY         0
#define ID_ERROR_STATE    (QUEUE_HANDLE)(0 - 1)


/* =================================================================== */
/*                   little-/big-endian access macros {25}             */
/* =================================================================== */

// Macro to define little-endian values - little endian use is specified by USB and SD-cards
//
#define LITTLE_SHORT_WORD_BYTES(x)   (unsigned char)(x), (unsigned char)(x >> 8)
#define LITTLE_SHORT_24BIT_BYTES(x)  (unsigned char)(x), (unsigned char)(x >> 8), (unsigned char)(x >> 16) // {28}
#define LITTLE_LONG_WORD_BYTES(x)    (unsigned char)(x), (unsigned char)(x >> 8), (unsigned char)(x >> 16), (unsigned char)(x >> 24)

#if defined _LITTLE_ENDIAN || defined _WINDOWS
    #define LITTLE_SHORT_WORD(x)     (unsigned short)(x)
    #define LITTLE_LONG_WORD(x)      (unsigned long)(x)
#else
    #define LITTLE_SHORT_WORD(x)     (unsigned short)((x << 8) | (x >> 8))
    #define LITTLE_LONG_WORD(x)      (unsigned long)((x << 24) | ((x >> 8) & 0x0000ff00) | ((x << 8) & 0x00ff0000) | (x >> 24)) // {27}
#endif

// Macro to define big-endian values - big endian use is specified by Ethernet
//
#define BIG_SHORT_WORD_BYTES(x)      (unsigned char)(x >> 8), (unsigned char)(x)
#define BIG_LONG_WORD_BYTES(x)       (unsigned char)(x >> 24), (unsigned char)(x >> 16), (unsigned char)(x >> 8), (unsigned char)(x)

#if defined _LITTLE_ENDIAN || defined _WINDOWS
    #define BIG_SHORT_WORD(x)     (unsigned short)((x << 8) | (x >> 8))
    #define BIG_LONG_WORD(x)      (unsigned long)((x << 24) | ((x >> 8) & 0x0000ff00) | ((x << 8) & 0x00ff0000) | (x >> 24)) // {55}
#else
    #define BIG_SHORT_WORD(x)     (unsigned short)(x)
    #define BIG_LONG_WORD(x)      (unsigned long)(x)
#endif


/* =================================================================== */
/*                     global structure definitions                    */
/* =================================================================== */

// Queue structure defines used as basis for all interfaces
//
typedef struct stQueueDimensions
{
    QUEUE_TRANSFER RxQueueSize;                                          // the size of the RX queue we desire
    QUEUE_TRANSFER TxQueueSize;                                          // the size of the TX queue we desire
}   QUEUE_DIMENSIONS;

// TTY table structure used to configure an SCI (serial) interface
//
typedef struct stTTYtable {
    UART_MODE_CONFIG Config;                                             // {15} bits per character, parity, stop bits plus mode
    QUEUE_DIMENSIONS Rx_tx_sizes;                                        // the desired rx and tx queue sizes
    #if defined WAKE_BLOCKED_TX && defined WAKE_BLOCKED_TX_BUFFER_LEVEL
    QUEUE_TRANSFER tx_wake_level;                                        // {24} specify exact output queue level to cause a transmitter wake up
    #endif
    QUEUE_HANDLE   Channel;                                              // channel number 0, 1, ...
    #if defined (SUPPORT_MSG_MODE) || defined (SERIAL_SUPPORT_ESCAPE)
    unsigned char  ucMessageTerminator;                                  // message terminator in message mode
    #endif
    #if defined SERIAL_SUPPORT_ESCAPE
    unsigned char  ucMessageFilter;                                      // used as escape sequencer
    #endif
    unsigned char  ucSpeed;                                              // asynch speed
    UTASK_TASK     Task_to_wake;                                         // 0 = don't wake any, else task to wake when receive message available
    #if defined SERIAL_SUPPORT_DMA
    unsigned char  ucDMAConfig;                                          // DMA configuration
        #if defined UART_TIMED_TRANSMISSION                              // {72}
    unsigned short usMicroDelay;                                         // the time base (us) between each character transmission
        #endif
    #endif
    #if defined SUPPORT_FLOW_HIGH_LOW
    unsigned char  ucFlowHighWater;                                      // % of buffer full to stall flow
    unsigned char  ucFlowLowWater;                                       // % of buffer full to restart flow
    #endif
} TTYTABLE;


// Synchronous Serial Controller table structure                         {22}
//
typedef struct stSSCtable {        
    QUEUE_DIMENSIONS Rx_tx_sizes;                                        // the desired rx and tx queue sizes
    unsigned short usConfig;                                             // polarity, frame type, etc.
    unsigned short usSpeed;                                              // data speed (clock divide value)
    QUEUE_HANDLE   Channel;                                              // channel number 0, 1, ...
    UTASK_TASK     Task_to_wake;                                         // 0 = don't wake any, else task to wake when receive message available
    unsigned char  ucWordSize;                                           // data bit length of a single word
    unsigned char  ucFrameLength;                                        // number of individual words in a single frame
    unsigned char  ucTxDataShift;
    unsigned char  ucRxDataShift;
    #if defined SSC_SUPPORT_DMA
    unsigned char  ucDMAConfig;                                          // DMA configuration
    #endif
} SSCTABLE;

#define RX_CLOCK_INVERTED       0x0001
#define TX_CLOCK_INVERTED       0x0002
#define RX_SYNC_RISING          0x0000
#define RX_SYNC_FALLING         0x0004
#define TX_LSB_FIRST            0x0000
#define TX_MSB_FIRST            0x0008
#define TX_POSITIVE_FRAME_PULSE 0x0000
#define TX_NEGATIVE_FRAME_PULSE 0x0010
#define RX_LSB_FIRST            0x0000
#define RX_MSB_FIRST            0x0020

#if defined USB_INTERFACE && !defined _COMPILE_COSMIC
    #include "usb.h"
#endif
// USB table structure used to configure a USB interface
//
typedef struct stUSBtable {                                              // {1}
    int (*usb_callback)(unsigned char *ptrData, unsigned short length, int iSetup); // optional call back
#if defined IN_COMPLETE_CALLBACK                                         // {63}
    void (*INcallback)(unsigned char ucEndpoint);                        // optional call back on IN completion
#endif
    QUEUE_DIMENSIONS queue_sizes;                                        // optional input/output queue sizes
#if defined WAKE_BLOCKED_USB_TX
    QUEUE_TRANSFER low_water_level;                                      // low water trigger level
#endif
    unsigned short usConfig;
    unsigned short usEndpointSize;
    QUEUE_HANDLE   Endpoint;                                             // endpoint number 0, 1, ...
    QUEUE_HANDLE   Paired_RxEndpoint;                                    // optional paired endpoint receive channel (zero when not used)
#if defined USE_USB_OTG_CHARGE_PUMP
    QUEUE_HANDLE   OTG_I2C_Channel;                                      // I2C interface to use to activate OTG charge pump
#endif
    unsigned char  ucClockSource;
    unsigned char  ucEndPoints;                                          // number of endpoints required
    UTASK_TASK     owner_task;                                           // task to receive messages on events
} USBTABLE;

#define EXTERNAL_USB_CLOCK       0
#define INTERNAL_USB_CLOCK       1
#define SPECIAL_USB_CLOCK        2

#define USB_SPEED_MASK           0x0003
#define USB_LOW_SPEED            0x0000
#define USB_FULL_SPEED           0x0001
#define USB_HIGH_SPEED           0x0002                                  // {51}
#define USB_DEVICE_MODE          0x0000                                  // {8}
#define USB_HOST_MODE            0x0004
#define USB_OTG_MODE             0x0008
#define USB_TERMINATING_ENDPOINT 0x0010                                  // {31} block transmissions equal to the size of the endpoint should be terminated with a zero data frame o termination
#define USB_ALT_PORT_MAPPING     0x0020                                  // {33}
#define USB_OUT_ZERO_COPY        0x0040                                  // {67}
#define USB_IN_ZERO_COPY         0x0080                                  // {67}
#define USB_IN_MESSAGE_MODE      0x0100
#define USB_IN_FIFO_MASK         0xf000
#define USB_IN_FIFO_SHIFT        12

// I2C table structure used to configure an I2C interface
//
typedef struct stI2CTABLE {
    unsigned short usSpeed;                                              // I2C speed (in kHz) - 0 for slave
    QUEUE_DIMENSIONS Rx_tx_sizes;                                        // the desired rx and tx queue sizes
    UTASK_TASK     Task_to_wake;                                         // default task to wake when receive message available
    QUEUE_HANDLE   Channel;                                              // physical channel number 1,2,3...
#if defined I2C_SLAVE_MODE                                               // {65}
    unsigned char  ucSlaveAddress;                                       // address to be used by slave
    int (*fnI2C_SlaveCallback)(int iChannel, unsigned char *ptrDataByte, int iType); // optional callback to prepare a byte to transmit
#endif
} I2CTABLE;

#define I2C_SLAVE_BUFFER              0                                  // the next byte is to be taken from the I2C output buffer
#define I2C_SLAVE_TX_PREPARED         1                                  // the next byte has been prepared by the application callback
#define I2C_SLAVE_RX_CONSUMED         2                                  // the byte has been consumed by the application callback

#define I2C_SLAVE_ADDRESSED_FOR_READ  0                                  // the slave has just been address for a read transaction (slave is to return data)
#define I2C_SLAVE_ADDRESSED_FOR_WRITE 1                                  // the slave has just been address for a write transaction (slave is to receive data)
#define I2C_SLAVE_READ                2                                  // the next data to return to the master is to be prepared
#define I2C_SLAVE_WRITE               3                                  // a byte of data for the slave has just been received from the master
#define I2C_SLAVE_READ_COMPLETE       4                                  // the master has completed a read (from the slave) transaction
#define I2C_SLAVE_WRITE_COMPLETE      5                                  // the master has completed a write (to the slave) transaction

// SPI table structure used to configure an SPI interface
//
typedef struct stSPITABLE {
    unsigned char  ucSpeed;                                              // SPI speed (set fixed defines) - 0 for slave
    QUEUE_DIMENSIONS Rx_tx_sizes;                                        // the desired rx and tx queue sizes
    UTASK_TASK     Task_to_wake;                                         // default task to wake when receive message available
    QUEUE_HANDLE   Channel;                                              // physical channel number 1,2,3...
    unsigned char  ucMode;                                               // mode details
} SPITABLE;




// Ethernet table structure used to configure an Ethernet interface
//
typedef struct stETHtable {
#if !defined ETHERNET_AVAILABLE || defined NO_INTERNAL_ETHERNET || (ETHERNET_INTERFACES > 1) || (defined USB_CDC_RNDIS && defined USB_TO_TCP_IP) || defined USE_PPP
    void *ptrEthernetFunctions;                                          // function table used when there is an external controller available
#endif
    QUEUE_HANDLE   Channel;                                              // channel number 0, 1, ...
    unsigned short usMode;                                               // mode of operation
    unsigned short usSizeRx;                                             // size to set for Rx buffer
    unsigned short usSizeTx;                                             // size to set for Tx buffer
    UTASK_TASK     Task_to_wake;                                         // 0 = don't wake any, else first letter
#if !defined USE_IPV6
    unsigned char  ucMAC[6];                                             // MAC address of device
#endif
    unsigned char  ucEthTypes;                                           // frame types supported
    unsigned short usExtEthTypes;                                        // extra ethernet frames to be accepted
} ETHTABLE;

typedef struct stETHERNET_FUNCTIONS {
    int (*fnConfigEthernet)(ETHTABLE *);                                 // configuration function for the Ethernet interface
    int (*fnGetQuantityRxBuf)(void);                                     // call-back used to get the number of available receive buffers
    unsigned char *(*fnGetTxBufferAdd)(int);                             // call-back used to get a memory-mapped buffer address
    int (*fnWaitTxFree)(void);                                           // call-back used to allow waiting on transmit buffer availability
    void (*fnPutInBuffer)(unsigned char *, unsigned char *, QUEUE_TRANSFER); // call-back used to prepare transmit data to the output buffer
    QUEUE_TRANSFER (*fnStartEthTx)(QUEUE_TRANSFER, unsigned char *);     // call-back used to release a prepared transmit buffer
    void (*fnFreeEthernetBuffer)(int);                                   // call-back used to free a used reception buffer
#if defined USE_IGMP
    void (*fnModifyMulticastFilter)(QUEUE_TRANSFER, unsigned char *);    // call-back used to setup the multicast filter
#endif
} ETHERNET_FUNCTIONS;

// CAN table structure used to configure the CAN interface
//
typedef struct stCANtable {
    unsigned long  ulTxID;                                               // address to send messages to
    unsigned long  ulRxID;                                               // address to accept messages on
    unsigned long  ulRxIDMask;                                           // address mask for reception messages
    unsigned long  ulSpeed;                                              // asynch CAN speed
    unsigned short usMode;                                               // mode of operation
    QUEUE_HANDLE   Channel;                                              // channel number 0, 1, ...
    unsigned char  ucTxBuffers;                                          // the numbr of buffers to reserve for transmission
    unsigned char  ucRxBuffers;                                          // the numbr of buffers to reserve for reception
    UTASK_TASK     Task_to_wake;                                         // 0 = don't wake any, else first letter
} CANTABLE;



// The queue control structure is used as basis by all driver interfaces
//
typedef struct stQUEQue
{
    unsigned char *QUEbuffer;                                            // pointer to the input/output buffer where the data is
    unsigned char *get;                                                  // pointer to present get position in the buffer
    unsigned char *put;                                                  // pointer to present put position in the buffer
    unsigned char *buffer_end;                                           // pointer to location after the end of the buffer
    QUEUE_TRANSFER chars;                                                // the present number of bytes in the buffer
    QUEUE_TRANSFER reserved_chars;                                       // {47} additional count value reserved during unprotected copies
    QUEUE_TRANSFER new_chars;                                            // {47} final count value after nested writes
    QUEUE_TRANSFER buf_length;                                           // length of the input/output buffer
} QUEQUE;


// I2C queue
//
typedef struct stI2CQue
{
    QUEQUE         I2C_queue;
    QUEUE_TRANSFER msgs;
    volatile unsigned char  ucPresentLen;
    unsigned char  ucState;
    UTASK_TASK     wake_task;
} I2CQue;

#define SERIAL_COUNTERS    2
#define SERIAL_TX_STATS    0
#define SERIAL_RX_STATS    SERIAL_COUNTERS
#define DELETE_ALL_STATS   (QUEUE_TRANSFER)(0-1)

#define SERIAL_STATS_CHARS 0                                             // The number of characters sent
#define SERIAL_STATS_FLOW  1                                             // The number of times the transmitter was stalled by flow control


// Serial interface queue
//
typedef struct stTTYQue
{
    #if defined SERIAL_STATS
    unsigned long  ulSerialCounter[SERIAL_COUNTERS];                     // counters for serial interface statistics
    #endif
    QUEQUE         tty_queue;                                            // the standard queue belonging to the TTY
    QUEUE_TRANSFER msgs;                                                 // the number of reception messages waiting in the input queue
    #if (defined (SUPPORT_MSG_CNT) && defined (SUPPORT_MSG_MODE)) || defined SERIAL_SUPPORT_DMA // {35}
    QUEUE_TRANSFER msgchars;                                             // the number of characters in the present reception message
    #endif
    #if defined SUPPORT_FLOW_HIGH_LOW
    QUEUE_TRANSFER high_water_level;                                     // flow control high water character count
    QUEUE_TRANSFER low_water_level;                                      // flow control low water character count
    #endif
    #if defined SERIAL_SUPPORT_DMA
    QUEUE_TRANSFER lastDMA_block_length;                                 // {11} the last DMA transmission block length
    #endif
    UART_MODE_CONFIG opn_mode;                                           // {15} operating mode details of the TTY
    unsigned char  ucState;
    #if defined (SUPPORT_MSG_MODE) || defined (SERIAL_SUPPORT_ESCAPE)
    unsigned char  ucMessageTerminator;                                  // the character being used as a message terminator when operating in message mode
    #endif
    #if defined SERIAL_SUPPORT_ESCAPE
    unsigned char  ucMessageFilter;                                      // escape message character when operating in escape mode
    #endif
    UTASK_TASK     wake_task;                                            // the task to be woken on character or message reception (depending on mode)
    #if defined SERIAL_SUPPORT_DMA
    unsigned char  ucDMA_mode;                                           // DMA operating mode details of the TTY
        #if defined UART_TIMED_TRANSMISSION                              // {72}
    unsigned short usMicroDelay;                                         // the time base (us) between each character transmission
        #endif
    #endif
} TTYQUE;


// Synchronous Serial Controller interface queue                         // {22}
//
typedef struct stSSCQue
{
    QUEQUE         ssc_queue;                                            // the standard queue belonging to the driver
    #if defined (SUPPORT_MSG_CNT) && defined (SUPPORT_MSG_MODE)
    QUEUE_TRANSFER msgchars;                                             // the number of characters in the present reception message
    #endif
    #if defined SSC_SUPPORT_DMA
    QUEUE_TRANSFER lastDMA_block_length;                                 // the last DMA transmission block length
    #endif
    unsigned short usOpn_mode;                                           // operating mode details of the SSC
    unsigned char  ucBytesPerWord;
    unsigned char  ucWordsPerFrame;
    unsigned char  ucState;
    UTASK_TASK     wake_task;                                            // the task to be woken on character or message reception (depending on mode)
    #if defined SSC_SUPPORT_DMA
    unsigned char  ucDMA_mode;                                           // DMA operating mode details of the SSC
    #endif
} SSCQUE;

typedef struct stETHERNETQue
{
    QUEQUE         ETH_queue;
    void          *NextTTYbuffer;                                        // pointer to the next input/output buffer
    QUEUE_TRANSFER msgs;
    UTASK_TASK     wake_task;
} ETHERNETQue;

#if defined USB_INTERFACE

typedef struct stUSB_MESSAGE_QUEUE
{
    unsigned char ucQuantity;
    unsigned char ucInIndex;
    unsigned char ucOutIndex;
    unsigned short usLength[1];
} USB_MESSAGE_QUEUE;

typedef struct stUSB_ENDPOINT                                            // each end point uses a management queue of this type
{
    int          (*usb_callback)(unsigned char *, unsigned short, int);
    #if defined IN_COMPLETE_CALLBACK                                     // {63}
    void         (*fnINcomplete)(unsigned char);                         // optional callbck on successful IN buffer completion (useful for interrupt endpoints to prepare next data)
    #endif
    unsigned char *ptrStart;
    void          *ptrEndpointOutCtr;
    void          *ptrEndpointInCtr;
    USB_MESSAGE_QUEUE *messageQueue;
    #if defined WAKE_BLOCKED_USB_TX
    QUEUE_TRANSFER low_water_level;                                      // low water trigger level
    #endif
    unsigned short usSent;
    unsigned short usMax_frame_length;                                   // endpoint length
    unsigned short usCompleteMessage;
    unsigned short usLimitLength;
    unsigned short usLength[USB_FIFO_BUFFER_DEPTH];
    unsigned short usParameters;                                         // {31}
    UTASK_TASK     event_task;
    #if defined USB_SIMPLEX_ENDPOINTS || defined SUPPORT_USB_SIMPLEX_HOST_ENDPOINTS
    UTASK_TASK     event_task_in;
    #endif
    unsigned char  ucEndpointNumber;
    unsigned char  ucState;
    unsigned char  ucPaired_IN;
    unsigned char  ucFIFO_depth;
} USB_ENDPOINT;

typedef struct stUSBQUE
{
    QUEQUE         USB_queue;
    USB_ENDPOINT  *endpoint_control;
} USBQUE;
#endif

#if !defined ETHERNET_INTERFACES
    #define ETHERNET_INTERFACES   1
#endif

// This structure is a basic driver entity specifying which driver call is used to process this interface and
// the control structures for receiving and transmitting data.
//
typedef struct stIDinfo
{
#if (!defined ETH_INTERFACE && (ETHERNET_INTERFACES == 1)) || !defined ETHERNET_AVAILABLE || defined NO_INTERNAL_ETHERNET || (ETHERNET_INTERFACES > 1) || defined USB_CDC_RNDIS || defined USE_PPP
    void *ptrDriverFunctions;                                            // list of functions that are used to handle certain driver types
#endif
    QUEUE_TRANSFER (*CallAddress)(QUEUE_HANDLE, unsigned char *, QUEUE_TRANSFER, unsigned char, QUEUE_HANDLE); // address of driver for all interraction
    QUEQUE *input_buffer_control;                                        // pointer to the input queue control block
    QUEQUE *output_buffer_control;                                       // pointer to the output queue control block
    QUEUE_HANDLE qHandle;                                                // the allocated channel number - can be a hardware interface or a task identifier 1, 2, 3 etc.
} IDINFO;

#define FIFO_LENGTH  void *

/* =================================================================== */
/*                      global variables definition                    */
/* =================================================================== */

extern IDINFO *que_ids;                                                  // memory for queues IDs
extern QUEUE_HANDLE DebugHandle;                                         // queue handle for debug interface
extern QUEUE_HANDLE Ethernet_handle[ETHERNET_INTERFACES];                // Ethernet interface handle


/* =================================================================== */
/*                 global function prototype declarations              */
/* =================================================================== */

extern QUEUE_TRANSFER fnDebugMsg (CHAR *ucToSend);                       // send string to debug interface
extern unsigned long  fnHexStrHex(CHAR *ucNewAdd);                       // converts an ASCII hex byte sequence to its hex value (can be up to a 32 bit value in length)
extern unsigned long  fnDecStrHex(CHAR *ucNewAdd);                       // converts an ASCII decimal input to its binary hex value

extern CHAR          *fnBufferHex(unsigned long ulValue, unsigned char uLen, CHAR *pBuf); // take a value and convert it to a string in a buffer
extern void           fnDebugHex(unsigned long ulValue, unsigned char ucLen); // take a value and send it as hex string over the debug interface
  #define CODE_SMALL              0x00                                   // don't use capitals for coding
  #define NO_LEADIN               0x00                                   // don't add "0x" before hex value
  #define WITH_TERMINATOR         0x00                                   // terminate the string with null terminator
  #define WITH_LEADIN             0x08                                   // add "0x" before hex value
  #define NO_TERMINATOR           0x10                                   // don't add a null terminator to string output
  #define WITH_SPACE              0x20                                   // insert a space before the value
  #define CODE_CAPITALS           0x40                                   // code using capitals
  #define WITH_CR_LF              0x80                                   // {18}

#if defined STRING_OPTIMISATION                                          // {21} standardisation of string routines
    extern CHAR *fnBufferDec(signed long slNumberToConvert, unsigned char ucStyle, CHAR *ptrBuf); // take a value and convert it to a string in a buffer
    extern void  fnDebugDec(signed long slNumberToConvert, unsigned char ucStyle); // take a (signed) long value and send it as decimal string over the debug interface
    extern void  fnDebugFloat(float floatToConvert, unsigned char ucStyle); // {64} take a single precision float value and it as a decimal string over the debug interface
    extern float fnFloatStrFloat(CHAR *cNewAdd);                         // {70} extract a float from a string input
#else
    extern CHAR *fnDebugDec(signed long slNumberToConvert, unsigned char ucStyle, CHAR *ptrBuf); // take a value and send it as decimal string over the debug interface or put in buffer
#endif
  #define DISPLAY_NEGATIVE        0x04                                   // {66} display value as negative number
  #define LEADING_SPACE           0x08                                   // ensure display of time with 2 digits - space as filler
  #define LEADING_ZERO            0x40                                   // ensure display of time with 2 digits - '0' as filler
//#define WITH_TERMINATOR  - same as for fnDebugHex case
//#define NO_TERMINATOR
//#define WITH_SPACE
//#define WITH_CR_LF

extern QUEUE_HANDLE   fnAllocateQueue(QUEUE_DIMENSIONS *ptrQueueDimensions, QUEUE_HANDLE channel_handle, QUEUE_TRANSFER (*entry_add)(QUEUE_HANDLE channel, unsigned char *ptBuffer, QUEUE_TRANSFER Counter, unsigned char ucCallType, QUEUE_HANDLE riverID), QUEUE_TRANSFER FullCntLength);
extern QUEUE_HANDLE   fnOpenTTY(TTYTABLE *pars, unsigned char driver_mode);
extern QUEUE_HANDLE   fnOpenCAN(CANTABLE *pars, unsigned char driver_mode);
#if defined USB_INTERFACE
    extern QUEUE_HANDLE   fnOpenUSB(USBTABLE *pars, unsigned char driver_mode); // {1}
    extern int fnUSB_handle_frame(unsigned char ucType, unsigned char *ptrData, int iEndpoint, USB_HW *ptrUSB_HW);
      #define USB_SETUP_FRAME             0
      #define USB_OUT_FRAME               1
      #define USB_CONTROL_OUT_FRAME       2
      #define USB_TX_ACKED                3
      #define USB_RESET_DETECTED          4
      #define USB_SUSPEND_DETECTED        5
      #define USB_RESUME_DETECTED         6
      #define USB_DATA_REPEAT             7
      #define USB_DEVICE_DETECTED         8                              // {8}
      #define USB_DEVICE_REMOVED          9
      #define USB_HOST_SOF                10
      #define USB_HOST_STALL_DETECTED     11
      #define USB_DEVICE_TIMEOUT          12                             // possibly temporary error/debug codes from here
      #define USB_HOST_ACK_PID_DETECTED   13
      #define USB_HOST_NACK_PID_DETECTED  14
      #define USB_HOST_BUS_TIMEOUT_DETECTED 15
      #define USB_HOST_DATA_ERROR_DETECTED 16

      #define USB_HOST_ERROR             -1
      #define BUFFER_CONSUMED             0
      #define MAINTAIN_OWNERSHIP          1                              // return values
      #define ACTIVATE_ENDPOINT           2
      #define TERMINATE_ZERO_DATA         3
      #define STALL_ENDPOINT              4
      #define SEND_SETUP                  5                              // {8}
      #define BUFFER_CONSUMED_EXPECT_MORE 6                              // {12}
      #define TRANSPARENT_CALLBACK        7                              // {30}
      #define CRITICAL_OUT                8                              // {34}
      #define INITIATE_IN_TOKEN           9                              // {56}

    extern int fnEndpointData(int iEndpoint, unsigned char *ptrData, unsigned short usLength, int iControl, unsigned char **ptrNextBuffer);
      #define SETUP_DATA_RECEPTION        0
      #define OUT_DATA_RECEPTION          1
      #define STATUS_STAGE_RECEPTION      2
      #define ENDPOINT_REQUEST_TYPE       3
      #define ENDPOINT_CLEARED            4                              // {30}
    extern int fnSetUSBConfigState(int iCommand, unsigned char ucConfig);
      #define USB_CONFIG_ACTIVATE         0
      #define USB_DEVICE_SUSPEND          1
      #define USB_DEVICE_RESUME           2

    extern USB_ENDPOINT_DESCRIPTOR *fnGetUSBEndpoint(unsigned char ucConfig, unsigned char *ptrDesc, unsigned short *usLength, unsigned short *usMaxLength); // {8}
    extern void fnSetUSBEndpointState(int iEndpoint, unsigned char ucStateSet);
    extern int  fnGetPairedIN(int iEndpoint_OUT);                        // {32}
    extern QUEUE_TRANSFER entry_usb(QUEUE_HANDLE channel, unsigned char *ptBuffer, QUEUE_TRANSFER Counter, unsigned char ucCallType, QUEUE_HANDLE DriverID);
    extern QUEUE_TRANSFER fnStartUSB_send(QUEUE_HANDLE channel, USBQUE *ptrUsbQueue, QUEUE_TRANSFER txLength);
#endif
extern QUEUE_HANDLE   fnOpenETHERNET(ETHTABLE *pars, unsigned short driver_mode);
extern QUEUE_HANDLE   fnOpenSPI(SPITABLE *pars, unsigned char driver_mode);
extern QUEUE_HANDLE   fnOpenI2C(I2CTABLE *pars);
extern QUEUE_HANDLE   fnOpenSSC(SSCTABLE *pars, unsigned char driver_mode); // {22}
extern QUEUE_HANDLE   fnOpen(unsigned char type_of_driver, unsigned char driver_mode, void *pars);
extern QUEUE_TRANSFER fnDriver(QUEUE_HANDLE driver_id, unsigned short state , unsigned short rx_or_tx);
extern QUEUE_TRANSFER fnRead(QUEUE_HANDLE driver_id, unsigned char *input_buffer, QUEUE_TRANSFER nr_of_bytes); // read contents of input queue to a buffer
extern QUEUE_TRANSFER fnMsgs(QUEUE_HANDLE driver_id);
extern QUEUE_TRANSFER fnWrite(QUEUE_HANDLE driver_id, unsigned char *output_buffer, QUEUE_TRANSFER nr_of_bytes);
    #define REMOVE_IPV6_FILTER (QUEUE_TRANSFER)(0 - 1)                   // {58}
    #define ADD_IPV6_FILTER    (QUEUE_TRANSFER)(0 - 2)
    #define REMOVE_IPV4_FILTER (QUEUE_TRANSFER)(0 - 3)
    #define ADD_IPV4_FILTER    (QUEUE_TRANSFER)(0 - 4)
extern QUEUE_TRANSFER fnGetDataBlock(QUEUE_HANDLE driver_id, unsigned char ucChannel, unsigned char **ptrBuf);
extern QUEUE_TRANSFER fnFreeBuffer(QUEUE_HANDLE driver_id, signed char cChannel);
//extern QUEUE_TRANSFER fnScan(QUEUE_HANDLE driver_id, unsigned char *ucSequence, unsigned char ucSeqLength); // not recommended - use fnPeekInput() instead
extern QUEUE_TRANSFER fnPeekInput(QUEUE_HANDLE driver_id, unsigned char *input_buffer, QUEUE_TRANSFER nr_of_bytes, int iOldestNewest); // {44}
  #define PEEK_NEWEST_INPUT 0
  #define PEEK_OLDEST_INPUT 1
extern QUEUE_TRANSFER fnFlush(QUEUE_HANDLE driver_id, unsigned char ucTxRx);
extern void           fnStats(QUEUE_HANDLE driver_id, QUEUE_TRANSFER CounterNumber, unsigned long *ulCountValue);
extern void           fnNeedQueues(QUEUE_LIMIT queues_needed);
extern QUEUE_HANDLE   fnSearchID(QUEUE_TRANSFER(*ulSearchAddress)(QUEUE_HANDLE, unsigned char *, unsigned short, unsigned char, QUEUE_HANDLE), QUEUE_HANDLE channel_mask);
extern QUEUE_HANDLE   fnGetQueueHandle(QUEUE_HANDLE HandleId);           //  {42}
extern QUEUE_TRANSFER fnPrint(unsigned char *ucToSend, QUEUE_HANDLE ucID);
extern QUEUE_TRANSFER fnInterruptMessage(UTASK_TASK Task, unsigned char ucIntEvent); // send an interrupt event to a task
extern QUEUE_TRANSFER fnEventMessage(UTASK_TASK DestTask, UTASK_TASK SourceTask, unsigned char ucEvent);
extern QUEUE_TRANSFER fnNetworkTx(unsigned char *output_buffer, QUEUE_TRANSFER nr_of_bytes);
extern QUEUE_TRANSFER fnGetBuf(QUEQUE *ptQUEQue, unsigned char *output_buffer, QUEUE_TRANSFER nr_of_bytes);
extern QUEUE_TRANSFER fnGetBufPeek(QUEQUE *ptQUEQue, unsigned char *output_buffer, QUEUE_TRANSFER nr_of_bytes, int iNewest); // {44}
extern QUEUE_TRANSFER fnFillBuf(QUEQUE *ptQUEQue, unsigned char *input_buffer, QUEUE_TRANSFER nr_of_bytes);
extern QUEUE_TRANSFER fnFillBuf_FIFO(QUEQUE *ptQUEQue, unsigned char *fifo_buffer, QUEUE_TRANSFER nr_of_bytes); // {9}

extern void fnInitUNetwork(void);
#if defined VARIABLE_PROTOCOL_UNETWORK                                   // {6}
    extern unsigned char uc_uNetworkProtocol[2];
#endif
#if defined USER_WRITE_HANDLERS && (USER_WRITE_HANDLERS > 0)
    extern QUEUE_TRANSFER fnUserWrite(QUEUE_HANDLE driver_id, unsigned char *output_buffer, QUEUE_TRANSFER nr_of_bytes); // {53}
#endif
extern QUEUE_TRANSFER fnDistributedTx(unsigned char *output_buffer, QUEUE_TRANSFER nr_of_bytes);
extern void fnHandle_unet(unsigned char *ptrData);

extern void fnRTS_change(QUEUE_HANDLE Channel, int iState);


extern void *uMalloc(MAX_MALLOC);
extern void *uMallocAlign(MAX_MALLOC __size, unsigned short ucAlign);    // {2}
#if defined SUPPORT_UFREE
    extern MAX_MALLOC uFree(int iFreeRegion);                            // {29}
#endif
#if defined SECONDARY_UMALLOC                                            // {52}
    extern void *uMalloc2(unsigned long);
    extern void *uMallocAlign2(unsigned long __size, unsigned short ucAlign);
    extern unsigned long uFree2(int iFreeRegion);
#endif
#if defined SUPPORT_UCALLOC                                              // {79}
    extern void uCFree(void *ptr);
    extern void *uCalloc(size_t n, size_t size);
#endif


#if defined RUN_LOOPS_IN_RAM
    extern void  fnInitDriver(void);
    extern int (*uMemcmp)(const void *ptrTo, const void *ptrFrom, size_t Size);
    extern int (*uStrcmp)(const CHAR *ptrTo, const CHAR *ptrFrom);
    extern CHAR *(*uStrcpy)(CHAR *ptrTo, const CHAR *ptrFrom);
    extern int (*uStrlen)(const CHAR *ptrStr);
    #if defined DMA_MEMCPY_SET && !defined DEVICE_WITHOUT_DMA
        extern void *uMemcpy(void *ptrTo, const void *ptrFrom, size_t Size);
        extern void *uMemset(void *ptrTo, int iValue, size_t Size);      // {80} use int as second parameter to match memset()
    #else
        extern void (*uMemcpy)(void *ptrTo, const void *ptrFrom, size_t Size);
        extern void (*uMemset)(void *ptrTo, int iValue, size_t Size);    // {80} use int as second parameter to match memset()
    #endif
#else
    extern int   uMemcmp(const void *ptrTo, const void *ptrFrom, size_t Size);
    extern void *uMemcpy(void *ptrTo, const void *ptrFrom, size_t Size);
    extern void *uMemset(void *ptrTo, int iValue, size_t Size);          // {80} use int as second parameter to match memset()
    extern int   uStrcmp(const CHAR *ptrTo, const CHAR *ptrFrom);
    extern CHAR *uStrcpy(CHAR *ptrTo, const CHAR *ptrFrom);
    extern int   uStrlen(const CHAR *ptrStr);
#endif

extern void uMemset_long(unsigned long *ptrTo, unsigned long ulValue, size_t Size); // {19}
extern void uMemcpy_long(unsigned long *ptrTo, const unsigned long *ptrFrom, size_t Size); // {19}
extern void *uReverseMemcpy(void *ptrTo, const void *ptrFrom, size_t Size); // {54}

extern unsigned short uStrEquiv(const CHAR *ptrTo, const CHAR *ptrFrom);


extern void uTaskerGlobalMonoTimer(UTASK_TASK OwnerTask, DELAY_LIMIT delay, unsigned char time_out_event);
extern void uTaskerGlobalStopTimer(UTASK_TASK OwnerTask, unsigned char time_out_event);
#define HARDWARE_TIMER 0x80
extern void fnSetFuncTask(void (*function)(unsigned char ucEvent));


extern void uEnable_Interrupt(void);
extern void uDisable_Interrupt(void);
extern void fnInUserInterrupt(int iEntering);                            // {4}
extern void uMask_Interrupt(unsigned char iMaskLevel);                   // {59}

// Parameter block support
//
extern int fnSetPar(unsigned short usParameterReference, unsigned char *ucValue, unsigned short usLength);
extern int fnGetPar(unsigned short usParameterReference, unsigned char *ucValue, unsigned short usLength);
   #define DEFAULT_PARAM_SET   0x4000
   #define TEMPORARY_PARAM_SET 0x8000
extern int fnDelPar(unsigned char ucDeleteType);
   #define INVALIDATE_PARAMETER_BLOCK 1
   #define SWAP_PARAMETER_BLOCK       2
   #define INVALIDATE_TEST_PARAMETER_BLOCK 3

// uFile system support
//
extern MEMORY_RANGE_POINTER uOpenFile(CHAR *ptrfileName);
extern MAX_FILE_LENGTH uGetFileLength(MEMORY_RANGE_POINTER ptrFile);
extern MAX_FILE_LENGTH uGetFileData(MEMORY_RANGE_POINTER ptrFile, MAX_FILE_SYSTEM_OFFSET FileOffset, unsigned char *ucData, MAX_FILE_LENGTH DataLength);
#if defined SUB_FILE_SIZE
    extern MAX_FILE_LENGTH uFileWrite(MEMORY_RANGE_POINTER ptrFile, unsigned char *ptrData, MAX_FILE_LENGTH DataLength, unsigned char ucSubFile);
    extern unsigned char fnGetFileType(CHAR *ptrFileName);
        #define SUB_FILE_TYPE 0x01
    extern MEMORY_RANGE_POINTER uOpenNextFile(MEMORY_RANGE_POINTER ptrfileLocation, MAX_FILE_LENGTH *FileLength, unsigned char ucSubfile);
    extern MEMORY_RANGE_POINTER uOpenNextMimeFile(MEMORY_RANGE_POINTER ptrfileLocation, MAX_FILE_LENGTH *FileLength, unsigned char *ucMimeType, unsigned char ucSubfile);
    extern CHAR uGetSubFileName(MEMORY_RANGE_POINTER ptrFile);
#else
    extern MAX_FILE_LENGTH uFileWrite(MEMORY_RANGE_POINTER ptrFile, unsigned char *ptrData, MAX_FILE_LENGTH DataLength);
    extern MEMORY_RANGE_POINTER uOpenNextFile(MEMORY_RANGE_POINTER ptrfileLocation, MAX_FILE_LENGTH *FileLength);
    extern MEMORY_RANGE_POINTER uOpenNextMimeFile(MEMORY_RANGE_POINTER ptrfileLocation, MAX_FILE_LENGTH *FileLength, unsigned char *ucMimeType);
#endif
extern MEMORY_RANGE_POINTER uOpenUserFile(CHAR *file_name);              // {37}
extern int             uCompareFile(MEMORY_RANGE_POINTER ptrFile, unsigned char *ptrData, MAX_FILE_LENGTH DataLength);
#if defined EXTENDED_UFILESYSTEM                                         // {40}
    extern int         uGetFileName(MEMORY_RANGE_POINTER ptrFile, CHAR *ptrFileName);
    #if defined EXTENSION_FILE_COUNT_VARIABLE
        extern void fnExtendFileSystem(unsigned long ulExtendedSpace);
    #endif
#else
    extern CHAR        uGetFileName(MEMORY_RANGE_POINTER ptrFile);
#endif
extern unsigned char  *fnGetEndOf_uFileSystem(void);
#if defined UFILESYSTEM_APPEND                                           // {41}
    extern unsigned char  *uOpenFileAppend(CHAR *ptrfileName);
#endif
extern MAX_FILE_LENGTH uFileClose(MEMORY_RANGE_POINTER ptrFile);
extern MAX_FILE_LENGTH uFileCloseMime(MEMORY_RANGE_POINTER ptrFile, unsigned char *ucMimeType);
extern int             uFileErase(MEMORY_RANGE_POINTER ptrFile, MAX_FILE_LENGTH FileLength);
extern void            fnProtectFile(void);
extern void            fnUnprotectFile(void);
extern unsigned char   fnConvertName(CHAR cName);

#if defined SUPPORT_MIME_IDENTIFIER                                      // {3}
    #define FILE_HEADER (sizeof(MAX_FILE_LENGTH) + 1)                    // file length followed by MIME identifier
#else
    #define FILE_HEADER (sizeof(MAX_FILE_LENGTH))
#endif

typedef struct stUSER_FILE                                               // {17}
{
    CHAR           *fileName;                                            // file name string
    unsigned char  *file_content;                                        // pointer to file in internal program space
    MAX_FILE_LENGTH file_length;                                         // the length of the file
    unsigned char  ucMimeType;                                           // the type of content
    unsigned char  ucProperties;                                         // special properties
} USER_FILE;

#define FILE_VISIBLE           0x00
#define FILE_INVISIBLE         0x01
#define FILE_ADD_EXT           0x02
#define FILE_NOT_CODE          0x80                                      // used for simulator compatibility

extern int fnGetUserMimeType(unsigned char *ptrfile, unsigned char *ptrMimeType); // {17}
extern void fnEnterUserFiles(USER_FILE *ptrUserFileList);
#if defined _WINDOWS
    extern unsigned char fnUserFileProperties(unsigned char *ptrfile);
#endif
extern USER_FILE *fnActivateEmbeddedUserFiles(CHAR *cFile, int iType);
    #define USER_FILE_IN_INTERNAL_FLASH  0
    #define USER_FILE_IN_EXTERNAL_SPACE  1

#if defined _WINDOWS
    extern int iFetchingInternalMemory;                                  // {48}
    #define _ACCESS_NOT_IN_CODE        0
    #define _ACCESS_IN_CODE            1
    #define _ACCESS_FROM_EXT_FLASH     2
#endif

// Cryptography                                                          {73}
//
extern int fnAES_Init(int iInstanceCommand, const unsigned char *ptrKey, int iKeyLength);
extern int fnAES_Cipher(int iInstanceCommand, const unsigned char *ptrTextIn, unsigned char *ptrTextOut, unsigned long ulDataLength);

#define AES_BLOCK_LENGTH               16
#define AES_INVALID_INSTANCE_REFERENCE -1
#define AES_INSTANCE_NOT_INITIALISED   -2
#define AES_ENCRYPT_BAD_LENGTH         -3
#define AES_ENCRYPT_BAD_ALIGNMENT      -4
#define AES_INVALID_KEY_LENGTH         -5

#define AES_INSTANCE_MASK               0x00ff
#define AES_COMMAND_AES_ENCRYPT         0x0100
#define AES_COMMAND_AES_DECRYPT         0x0200
#define AES_COMMAND_AES_SET_KEY_ENCRYPT 0x0400
#define AES_COMMAND_AES_SET_KEY_DECRYPT 0x0800
#define AES_COMMAND_AES_RESET_IV        0x1000
#define AES_COMMAND_AES_PRIME_IV        0x2000

// DSP                                                                   {74}
//
extern float fnGenerateWindowFloat(float *ptrWindowBuffer, int iInputSamples, int iWindowType);
    #define BLACKMANN_HARRIS_WINDOW        0
extern int fnFFT(void *ptrInputBuffer, void *ptrOutputBuffer, int iInputSamples, int iSampleOffset, int iInputBufferSize, float *ptrWindowingBuffer, float window_conversionFactor, int iInputOutputType);
    #define FFT_INPUT_MASK                 0x00f
    #define FFT_INPUT_BYTES_UNSIGNED       0x000
    #define FFT_INPUT_BYTES_SIGNED         0x001
    #define FFT_INPUT_HALF_WORDS_UNSIGNED  0x002
    #define FFT_INPUT_HALF_WORDS_SIGNED    0x003
    #define FFT_INPUT_LONG_WORDS_UNSIGNED  0x004
    #define FFT_INPUT_LONG_WORDS_SIGNED    0x005
    #define FFT_INPUT_FLOATS               0x006
    #define FFT_OUTPUT_MASK                0x0f0
    #define FFT_OUTPUT_BYTES_UNSIGNED      0x000
    #define FFT_OUTPUT_BYTES_SIGNED        0x010
    #define FFT_OUTPUT_HALF_WORDS_UNSIGNED 0x020
    #define FFT_OUTPUT_HALF_WORDS_SIGNED   0x030
    #define FFT_OUTPUT_LONG_WORDS_UNSIGNED 0x040
    #define FFT_OUTPUT_LONG_WORDS_SIGNED   0x050
    #define FFT_OUTPUT_FLOATS              0x060
    #define FFT_COMPLEX_RESULT             0x000
    #define FFT_MAGNITUDE_RESULT           0x100
#if defined _WINDOWS
extern void fnInjectSine(int instance, int iType, void *ptrData, unsigned short usLength);
    #define INJECT_SINE_BYTES_UNSIGNED      0
    #define INJECT_SINE_BYTES_SIGNED        1
    #define INJECT_SINE_HALF_WORDS_UNSIGNED 2
    #define INJECT_SINE_HALF_WORDS_SIGNED   3
    #define INJECT_SINE_LONG_WORDS_UNSIGNED 4
    #define INJECT_SINE_LONG_WORDS_SIGNED   5
    #define INJECT_SINE_FLOATS              6
#endif

// Driver Interrupt Events
//
#define UNKNOWN_INTERRUPT           0
#define LAN_LINK_UP_100_FD          0xff                                 // {50}
#define LAN_LINK_UP_100             0xfe
#define LAN_LINK_UP_10_FD           0xfd                                 // {50}
#define LAN_LINK_UP_10              0xfc
#define LAN_LINK_DOWN               0xfb
#define EMAC_RX_INTERRUPT           0xfa
#define ETHERNET_COLLISION          0xf9
#define ETHERNET_EXESSIVE_COLLISION 0xf8
#define BABBLING_RX                 0xf7
#define ETHERNET_RX_OVERRUN         0xf6
#define ETHERNET_RX_ERROR           0xf5
#define ETHERNET_FLOW_CONTROL       0xf4
#define EMAC_TX_INTERRUPT           0xf3
#define EMAC_RXA_INTERRUPT          0xf2
#define EMAC_RXB_INTERRUPT          0xf1
#define CAN_TX_ERROR                0xf0
#define CAN_TX_REMOTE_ERROR         0xef
#define CAN_OTHER_ERROR             0xee
#define CAN_TX_ACK                  0xed
#define CAN_RX_MSG                  0xec
#define CAN_TX_REMOTE_ACK           0xeb
#define CAN_RX_REMOTE_MSG           0xea
#define DTR_CHANGE_ACTIVE_0         0xe9
#define DTR_CHANGE_INACTIVE_0       0xe8
#define DTR_CHANGE_ACTIVE_1         0xe7
#define DTR_CHANGE_INACTIVE_1       0xe6
#define DTR_CHANGE_ACTIVE_2         0xe5
#define DTR_CHANGE_INACTIVE_2       0xe4
#define CTS_CHANGE_ACTIVE_0         0xe3
#define CTS_CHANGE_INACTIVE_0       0xe2
#define CTS_CHANGE_ACTIVE_1         0xe1
#define CTS_CHANGE_INACTIVE_1       0xe0
#define CTS_CHANGE_ACTIVE_2         0xdf
#define CST_CHANGE_INACTIVE_2       0xde
#define TX_FREE                     0xdd                                 // interrupt event informing of free serial / TCP buffer

#define UNETWORK_FRAME_LOSS         0xd8
#define UNETWORK_SYNC_LOSS          0xd7
                                                                         // interrupt debugging messages for PHY
#define AUTO_NEG_CHANGED            0xd6
#define AUTO_NEG_COMPLETE           0xd5
#define AUTO_NEG_COMMON_MODE        0xd4
#define ACK_BIT_REC                 0xd3
#define PAR_DET_FAULT               0xd2
#define REMOTE_FAULT                0xd1
#define JABBER_INT                  0xd0
#define AUTO_FULL_DUPLEX            0xcf
#define AUTO_PAUSE_RESOLVED         0xce
#define PAGE_REC                    0xcd

#define EVENT_USB_RESET             0xcc                                 // USB reset when in enumerated state
#define EVENT_USB_SUSPEND           0xcb                                 // suspend condition detected
#define EVENT_USB_RESUME            0xca                                 // resume sequence detected

#define EVENT_USB_DETECT_HS         0xc9                                 // USB device detected (high speed)
#define EVENT_USB_DETECT_FS         0xc8                                 // USB device detected (full speed)
#define EVENT_USB_DETECT_LS         0xc7                                 // {57} USB device detected (low speed)
#define EVENT_USB_REMOVAL           0xc6                                 // USB device removed
    
#define UTFAT_OPERATION_COMPLETED   0xbf                                 // {28}

#define ZERO_CONFIG_SUCCESSFUL      0xba                                 // {39}
#define ZERO_CONFIG_DEFENDED        0xb9                                 // {39}
#define ZERO_CONFIG_COLLISION       0xb8                                 // {39}

#define RTC_ALARM_INTERRUPT_EVENT   0xb7

// Networking events (from Ethernet)                                     // {69}
//
#define DHCP_SUCCESSFUL             0x01                                 // DHCP was successful - the system can use TCP/IP
#define DHCP_COLLISION              0x02                                 // DHCP received infomation but there is an IP conflict - retry?
#define DHCP_LEASE_TERMINATED       0x03                                 // DHCP lease has terminated without being able to establish new lease - presently no network capability
#define DHCP_MISSING_SERVER         0x04                                 // DHCP repetition timer has been increased to a level indicating no server present


// Time keeping                                                          // {60}
//
extern void fnStartRTC(void (*seconds_callback)(void));
extern void fnGetRTC(RTC_SETUP *ptrSetup);
extern CHAR *fnUpTime(CHAR *cValue);
extern unsigned char fnSetShowTime(int iSetDisplay, CHAR *ptrInput);
    #define DISPLAY_RTC_TIME        0x01
    #define DISPLAY_RTC_DATE        0x02
    #define DISPLAY_RTC_TIME_DATE   0x03
    #define DISPLAY_RTC_ALARM       0x04
    #define SET_RTC_TIME            0x08
    #define SET_RTC_ALARM_TIME      0x10
    #define SET_RTC_DATE            0x20
extern void fnConvertSecondsTime(RTC_SETUP *ptr_rtc_setup, unsigned long ulSecondsTime);
extern unsigned long fnConvertTimeSeconds(RTC_SETUP *ptr_rtc_setup, int iSetTime);
extern unsigned short fnConvertTimeDays(RTC_SETUP *ptr_rtc_setup, int iSetTime);
extern void fnSecondsTick(void (*rtc_interrupt_handler[6])(void), int rtc_interrupts);
extern void fnAdjustLocalTime(unsigned char ucNewTimeZone, int iSNTP_active);

#define DAYLIGHT_SAVING_WINTER     0x00
#define DAYLIGHT_SAVING_SUMMER     0x80
#define TIME_ZONE_MASK             0x3f                                  // 34 (0x22) different time zones exist
#define TIME_ZONE_UTC_MINUS_12     0
#define TIME_ZONE_UTC_MINUS_11     1
#define TIME_ZONE_UTC_MINUS_10     2
#define TIME_ZONE_UTC_MINUS_9      3
#define TIME_ZONE_UTC_MINUS_8      4
#define TIME_ZONE_UTC_MINUS_7      5
#define TIME_ZONE_UTC_MINUS_6      6
#define TIME_ZONE_UTC_MINUS_5      7
#define TIME_ZONE_UTC_MINUS_4_HALF 8                                     // Caracas
#define TIME_ZONE_UTC_MINUS_4      9
#define TIME_ZONE_UTC_MINUS_3_HALF 10                                    // Newfoundland
#define TIME_ZONE_UTC_MINUS_3      11
#define TIME_ZONE_UTC_MINUS_2      12
#define TIME_ZONE_UTC_MINUS_1      13
#define TIME_ZONE_UTC_0            14
#define TIME_ZONE_UTC_PLUS_1       15
#define TIME_ZONE_UTC_PLUS_2       16
#define TIME_ZONE_UTC_PLUS_3       17
#define TIME_ZONE_UTC_PLUS_3_HALF  18                                    // Tehran
#define TIME_ZONE_UTC_PLUS_4       19
#define TIME_ZONE_UTC_PLUS_4_HALF  20                                    // Kabul
#define TIME_ZONE_UTC_PLUS_5       21
#define TIME_ZONE_UTC_PLUS_5_HALF  22                                    // New Delhi
#define TIME_ZONE_UTC_PLUS_5_3Q    23                                    // Catmandu
#define TIME_ZONE_UTC_PLUS_6       24
#define TIME_ZONE_UTC_PLUS_6_HALF  25                                    // Rangoon
#define TIME_ZONE_UTC_PLUS_7       26
#define TIME_ZONE_UTC_PLUS_8       27
#define TIME_ZONE_UTC_PLUS_9       28
#define TIME_ZONE_UTC_PLUS_9_HALF  29                                    // Darwin
#define TIME_ZONE_UTC_PLUS_10      30
#define TIME_ZONE_UTC_PLUS_11      31
#define TIME_ZONE_UTC_PLUS_12      32
#define TIME_ZONE_UTC_PLUS_13      33

// Virtual interrupts with compatibility with Coldfire RTCs
//
#define RTC_STOPWATCH       0x00000001                                   // stopwatch timer out interrupt
#define RTC_MINUTE_INT      0x00000002                                   // minute interrupt
#define RTC_ALARM_INT       0x00000004                                   // alarm interrupt
#define RTC_DAY_INT         0x00000008                                   // day interrupt
#define RTC_SECOND_INT      0x00000010                                   // second interrupt
#define RTC_HOUR_INT        0x00000020                                   // hour interrupt

#define RTC_ALARM_INT_MATCH 0x00000100
#define RTC_STOPWATCH_MATCH 0x00000200



/* References:
 * http://en.wikipedia.org/wiki/Twilight
 * http://de.wikipedia.org/wiki/Dämmerung
 * http://www.timeanddate.com/worldclock/sunrise.html
 * http://aa.usno.navy.mil/data/docs/RS_OneYear.php
 * http://williams.best.vwh.net/sunrise_sunset_algorithm.htm
 */


enum twilight
{
	UNKNOWN = 0,
	OFFICIAL,				// official sunrise and sunset
	CIVIL,					// use civil twilight
	NAUTICAL,				// use nautical twilight
	ASTRONOMICAL			// use astronomical twilght
};
/*
enum _direction
{
	NORTH = 0,
	EAST,
	SOUTH,
	WEST
};*/

typedef struct
{
    signed char   scDegree;                                              // degree -90°..90° positive for north or east and negative for south or west
    unsigned char ucMinute;                                              // minutes 0'..90'
    unsigned char ucSecond;                                              // seconds 0''..90''
    //enum _direction cdir;
} COORDINATE;

typedef struct
{
	COORDINATE latitude;                                                    // north (positive), south (negative)
	COORDINATE longitude;                                                   // east (positive), west (negative)
} LOCATION;

/*
 * double dark(enum twilight)
 *
 * Check if it is dark or not.
 *
 * Pararmeter   : enum twilight   Determines the twilight standard to use, 
 *                                official, civil, nautical or astronomical.
 *
 * Return       : double          Returns the time as a double in hours until
 *                                the next change of darkness state.
 *                                The returned value is positive if it is dark.
 *                                The returned value is negative if it is not 
 *                                dark.
 *
 * Dependencies : Uses time and date from RTC (assumes this is UTC).
 *                Uses location stored in the parameter structure.
 *
 */
double dark(enum twilight tw);

/*
 * double dusk(enum twilight)
 *
 * Get the time of dusk for today.
 *
 * Pararmeter   : enum twilight   Determines the twilight standard to use, 
 *                                official, civil, nautical or astronomical.
 *
 * Return       : double          Returns the time (UTC) of dusk as a double 
 *                                in hours.
 *                                
 *
 * Dependencies : Uses date from RTC.
 *                Uses location stored in the parameter structure.
 *
 */
double dusk(enum twilight tw);

/*
 * double dawn(enum twilight)
 *
 * Get the time of dawn for today.
 *
 * Pararmeter   : enum twilight   Determines the twilight standard to use, 
 *                                official, civil, nautical or astronomical.
 *
 * Return       : double          Returns the time (UTC) of dawn as a double 
 *                                in hours.
 *                                
 *
 * Dependencies : Uses date from RTC.
 *                Uses location stored in the parameter structure.
 *
 */
double dawn(enum twilight tw);

/* helper functions */
enum twilight c2tw(char c);
int days_in_month(int month, int year);

#endif
