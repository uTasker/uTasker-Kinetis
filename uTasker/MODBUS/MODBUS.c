/**********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher

    ---------------------------------------------------------------------
    File:      MODBUS.c
    Project:   Single Chip Embedded Internet
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2017
    *********************************************************************
    28.07.2009 Completed AVR32 configuration and reset RTU state to idle after inter-character space error {V1.01}
    06.08.2009 MODBUS USB slave support added, plus additional dependency for event counter use {V1.02}
    12.08.2009 Add RTU/ASCII decision for reverse routing to UARTs        {V1.03}
    22.08.2009 Dimensions of tcp_master_timeout and tcp_master_broadcast_timeout corrected to MODBUS_TCP_MASTERS - see modbus.h {V1.04}
    25.08.2009 Correct V1.03 reverse route and add additional route capability {V1.05}
    02.09.2009 Add reverse routing to TCP masters and always reduce length by CRC length when routing {V1.06}
    22.09.2009 Delete routing queue on TCP connection close, return fnMODBUS_route() result to TCP caller and release MODBUS master socket on close to ensure next connection is correctly established {V1.07}
    02.10.2009 Add fnClose_MODBUS_port() to close a MODBUS master TCP connection on demand and fnGet_MODBUS_TCP_port_status() {V1.07}
    08.12.2009 Allow routing when only MODBUS_TCP_SERIAL_GATEWAY mode is set at a TCP slave {V1.08}
    08.12.2009 Make content of fnClose_MODBUS_port dependent on TCP master {V1.08}
    20.01.2010 Accept V1.07B2..B4 changes
    20.01.2010 Correct response function value from MODBUS_READ_WRITE_MULTIPLE_REGISTER {V1.09}
    12.05.2010 Respect LPC2XXX timer modes and activate RTS control without RTS delay being necessary {V1.10}
    17.05.2010 Correct MODBUS_SERIAL_INTERFACES > 0 to allow single ASCII interface and only reset modbus_session->usOutstandingData when ACK is received, not data {V1.11}
    15.06.2010 Remove incompatible parameter                              {V1.12}
    23.08.2010 Reset TCP outstanding data count when a TCP connection is reset by the peer {V1.13}
    25.08.2010 Extra configuration definitions for more verified configurations as well as LPC17XX and STM32 support {V1.14}
    30.08.2010 Ensure correct unit address returned via TCP/serial gateway {V1.15}
    10.10.2010 Allow LM3SXXXX to use one 16 bit timer channel per interface (rather than on full 32 bit timer) {V1.16}
    10.10.2010 Configure external UARTs to operate in automatic RS485 mode {V1.16}
    24.11.2010 Ensure transaction ID and port are correct when sending exceptions to bad queries from TCP {V1.17}
    28.03.2011 Add Kinetis support (used PITs for RTU timer interrupts)  {V1.18}
    12.05.2011 Allow devices with automatic RS485 RTS control to remove unnecessary code {V1.19}
    02.01.2012 Allow correct serial queuing operation without needing MODBUS_GATE_WAY_ROUTING defined, plus STM32 RTU timer mode correction {V1.20}
    19.03.2012 Allow Kinetis to control RTS line automatically           {V1.21}
    13.06.2012 Silently ignore reverse routing to TCP masters whos connections have been lost/reset {V1.22}
    01.04.2014 Allow MODBUS/TCP to work on multiple networks             {V1.23}
    07.03.2015 Use software FIFO for gateway queues                      {V1.24}
    15.05.2015 Extend USB support to multiple CDC interfaces             {V1.25}
    13.08.2015 Allow MODBUS TCP masters that are not all in operation. Also ensure correct uTaskerGlobalStopTimer()/uTaskerStopTimer() usage {V1.26}
    27.08.2015 Add RTS control to Kinetis KE and KL devices, using UART_FRAME_END_COMPLETE option {V1.27}
    10.09.2015 Protect against attempting to access single non-existing register content at address 0 {V1.28}
    18.03.2016 Correct fnPostFunction() parameters when NOTIFY_ONLY_COIL_CHANGES is used {V1.29}

*/

#include "config.h"


#if defined USE_MODBUS


#define _V1_18B1                                                         // distinguish between broadcast and addressed serial timer delay from queued master transmission


/* =================================================================== */
/*                          local definitions                          */
/* =================================================================== */

#define OWN_TASK                    TASK_MODBUS

#define MODBUS_OFF                      0
#define MODBUS_IDLE                     1
#define MODBUS_FRAME_OK                 2
#define MODBUS_CHARACTER_TERMINATING    3
#define MODBUS_FRAME_NOT_OK             4
#define MODBUS_ASCII_HUNTING            5
#define MODBUS_ASCII_SYNCHRONISED       6
#define MODBUS_ASCII_ADDRESSED          7
#define MODBUS_ASCII_ADDRESSED_2        8
#define MODBUS_ASCII_TERMINATING        9

#define T_ASCII_INTER_CHAR_TIMEOUT      10

#define T_TIMER_BROADCAST               40

#define T_TIMER_SLAVE                   70


#define EVENT_VALID_FRAME_RECEIVED      10
#define EVENT_INCOMPLETE_FRAME_RECEIVED 20
#define EVENT_BAD_FRAME_TERMINATED      30

#if !defined DEBUG_MODBUS                                                // debug output disabled when not required
    #define fnDebugMsg(x)
    #define fnDebugDec(x,y)
    #define fnDebugHex(x,y)
#endif

#define REFERENCE_BIT_TIME (float)8.681                                  // bit period at 115200 Baud in us

#if defined SERIAL_SUPPORT_DMA
    #define RTS_TIMES 4                                                  // 8 bit int, 7 bit int, 8 bit DMA, 7 bit DMA
#else
    #define RTS_TIMES 2                                                  // 8 bit int, 7 bit int
#endif

#if defined MODBUS_RS485_SUPPORT
    #if defined _HW_SAM7X || defined _HW_AVR32                           // these delays are only actually used by DBGU (UART2) on the SAM7X since the USARTs 0 and 1
                                                                         // have an RS485 mode which is used instead. The DBGU can only operate in 8 bit mode so only supports RTU mode of operation.
        #define RTS_BIT_DELAY_8BIT_INT (float)9.6                        // the last transmit character interrupt arrives this many bit periods before the RTS should be negated
        #define RTS_BIT_DELAY_7BIT_INT (float)8.6
        #define RTS_BIT_DELAY_8BIT_DMA (float)20.8
        #define RTS_BIT_DELAY_7BIT_DMA (float)19.8
        #if defined _HW_SAM7X && defined DBGU_UART                       // if the DBGU is enable in the project
            #define MODBUS_RS485_RTS_SUPPORT                             // enable RTS support based on a port output controlled by a timer
        #endif
        #if defined _HW_AVR32 && !defined _AVR32_RS485_ONLY_USART1       // only needed when MODBUS RS484 is used on a USART other than UART1
            #define MODBUS_RS485_RTS_SUPPORT                             // enable RTS support based on a port output controlled by a timer (USARTs 0, 2, 3)
        #endif
    #elif defined _M5223X
        #define RTS_BIT_DELAY_8BIT_INT (float)10.9                       // the last transmit character interrupt arrives this many bit periods before the RTS should be negated
        #define RTS_BIT_DELAY_7BIT_INT (float)9.9
        #define RTS_BIT_DELAY_8BIT_DMA (float)21.7
        #define RTS_BIT_DELAY_7BIT_DMA (float)20.7
        #define MODBUS_RS485_RTS_SUPPORT                                 // enable RTS support based on a port output controlled by a timer on each MODBUS serial port
    #elif defined _LM3SXXXX
        #define RTS_BIT_DELAY_8BIT_INT (float)11.9                       // the last transmit character interrupt arrives this many bit periods before the RTS should be negated
        #define RTS_BIT_DELAY_7BIT_INT (float)10.9
        #define RTS_BIT_DELAY_8BIT_DMA (float)11.9
        #define RTS_BIT_DELAY_7BIT_DMA (float)10.8
        #define MODBUS_RS485_RTS_SUPPORT                                 // enable RTS support based on a port output controlled by a timer on each MODBUS serial port
    #elif defined _STR91XF || defined _STM32
        #define RTS_BIT_DELAY_8BIT_INT (float)11.9                       // the last transmit character interrupt arrives this many bit periods before the RTS should be negated
        #define RTS_BIT_DELAY_7BIT_INT (float)10.9
        #define RTS_BIT_DELAY_8BIT_DMA (float)11.9
        #define RTS_BIT_DELAY_7BIT_DMA (float)10.8
        #define MODBUS_RS485_RTS_SUPPORT                                 // enable RTS support based on a port output controlled by a timer on each MODBUS serial port
    #endif
#endif

// Test corrections and improvements accepted
//
#define _V1_07B2                                                         // improves gateway queue checking efficiency
#define _V1_07B3                                                         // correct incorrectly entered reverse path during race-state between internal master and external gateway
#define _V1_07B4                                                         // new master call back gateway from serial
#define _V1_07B4_1                                                       // enter optional routing information in serial queue

/* =================================================================== */
/*                       local structure definitions                   */
/* =================================================================== */

/* =================================================================== */
/*                             constants                               */
/* =================================================================== */

#if defined MODBUS_RS485_RTS_SUPPORT
static const unsigned short usRTSTimes[SERIAL_BAUD_115200 - SERIAL_BAUD_600][RTS_TIMES] = { // this table holds the required delay from the last interrupt in us for each speed/character length/int/dma combination
    {                                                                    // 1200 Baud time
    (unsigned short)((RTS_BIT_DELAY_8BIT_INT * REFERENCE_BIT_TIME) * (float)(115200 / 1200)),
    (unsigned short)((RTS_BIT_DELAY_7BIT_INT * REFERENCE_BIT_TIME) * (float)(115200 / 1200)),
    #if defined SERIAL_SUPPORT_DMA
    (unsigned short)((RTS_BIT_DELAY_8BIT_DMA * REFERENCE_BIT_TIME) * (float)(115200 / 1200)),
    (unsigned short)((RTS_BIT_DELAY_7BIT_DMA * REFERENCE_BIT_TIME) * (float)(115200 / 1200))
    #endif
    },
    {                                                                    // 2400 Baud time
    (unsigned short)((RTS_BIT_DELAY_8BIT_INT * REFERENCE_BIT_TIME) * (float)(115200 / 2400)),
    (unsigned short)((RTS_BIT_DELAY_7BIT_INT * REFERENCE_BIT_TIME) * (float)(115200 / 2400)),
    #if defined SERIAL_SUPPORT_DMA
    (unsigned short)((RTS_BIT_DELAY_8BIT_DMA * REFERENCE_BIT_TIME) * (float)(115200 / 2400)),
    (unsigned short)((RTS_BIT_DELAY_7BIT_DMA * REFERENCE_BIT_TIME) * (float)(115200 / 2400))
    #endif
    },
    {                                                                    // 4800 Baud time
    (unsigned short)((RTS_BIT_DELAY_8BIT_INT * REFERENCE_BIT_TIME) * (float)(115200 / 4800)),
    (unsigned short)((RTS_BIT_DELAY_7BIT_INT * REFERENCE_BIT_TIME) * (float)(115200 / 4800)),
    #if defined SERIAL_SUPPORT_DMA
    (unsigned short)((RTS_BIT_DELAY_8BIT_DMA * REFERENCE_BIT_TIME) * (float)(115200 / 4800)),
    (unsigned short)((RTS_BIT_DELAY_7BIT_DMA * REFERENCE_BIT_TIME) * (float)(115200 / 4800))
    #endif
    },
    {                                                                    // 9600 Baud time
    (unsigned short)((RTS_BIT_DELAY_8BIT_INT * REFERENCE_BIT_TIME) * (float)(115200 / 9600)),
    (unsigned short)((RTS_BIT_DELAY_7BIT_INT * REFERENCE_BIT_TIME) * (float)(115200 / 9600)),
    #if defined SERIAL_SUPPORT_DMA
    (unsigned short)((RTS_BIT_DELAY_8BIT_DMA * REFERENCE_BIT_TIME) * (float)(115200 / 9600)),
    (unsigned short)((RTS_BIT_DELAY_7BIT_DMA * REFERENCE_BIT_TIME) * (float)(115200 / 9600))
    #endif
    },
    {                                                                    // 14400 Baud time
    (unsigned short)((RTS_BIT_DELAY_8BIT_INT * REFERENCE_BIT_TIME) * (float)(115200 / 14400)),
    (unsigned short)((RTS_BIT_DELAY_7BIT_INT * REFERENCE_BIT_TIME) * (float)(115200 / 14400)),
    #if defined SERIAL_SUPPORT_DMA
    (unsigned short)((RTS_BIT_DELAY_8BIT_DMA * REFERENCE_BIT_TIME) * (float)(115200 / 14400)),
    (unsigned short)((RTS_BIT_DELAY_7BIT_DMA * REFERENCE_BIT_TIME) * (float)(115200 / 14400))
    #endif
    },
    {                                                                    // 19200 Baud time
    (unsigned short)((RTS_BIT_DELAY_8BIT_INT * REFERENCE_BIT_TIME) * (float)(115200 / 19200)),
    (unsigned short)((RTS_BIT_DELAY_7BIT_INT * REFERENCE_BIT_TIME) * (float)(115200 / 19200)),
    #if defined SERIAL_SUPPORT_DMA
    (unsigned short)((RTS_BIT_DELAY_8BIT_DMA * REFERENCE_BIT_TIME) * (float)(115200 / 19200)),
    (unsigned short)((RTS_BIT_DELAY_7BIT_DMA * REFERENCE_BIT_TIME) * (float)(115200 / 19200))
    #endif
    },
    {                                                                    // 38400 Baud time
    (unsigned short)((RTS_BIT_DELAY_8BIT_INT * REFERENCE_BIT_TIME) * (float)(115200 / 38400)),
    (unsigned short)((RTS_BIT_DELAY_7BIT_INT * REFERENCE_BIT_TIME) * (float)(115200 / 38400)),
    #if defined SERIAL_SUPPORT_DMA
    (unsigned short)((RTS_BIT_DELAY_8BIT_DMA * REFERENCE_BIT_TIME) * (float)(115200 / 38400)),
    (unsigned short)((RTS_BIT_DELAY_7BIT_DMA * REFERENCE_BIT_TIME) * (float)(115200 / 38400))
    #endif
    },
    {                                                                    // 57600 Baud time
    (unsigned short)((RTS_BIT_DELAY_8BIT_INT * REFERENCE_BIT_TIME) * (float)(115200 / 57600)),
    (unsigned short)((RTS_BIT_DELAY_7BIT_INT * REFERENCE_BIT_TIME) * (float)(115200 / 57600)),
    #if defined SERIAL_SUPPORT_DMA
    (unsigned short)((RTS_BIT_DELAY_8BIT_DMA * REFERENCE_BIT_TIME) * (float)(115200 / 57600)),
    (unsigned short)((RTS_BIT_DELAY_7BIT_DMA * REFERENCE_BIT_TIME) * (float)(115200 / 57600))
    #endif
    },
    {                                                                    // 115200 Baud time
    (unsigned short)((RTS_BIT_DELAY_8BIT_INT * REFERENCE_BIT_TIME) * (float)(115200 / 115200)),
    (unsigned short)((RTS_BIT_DELAY_7BIT_INT * REFERENCE_BIT_TIME) * (float)(115200 / 115200)),
    #if defined SERIAL_SUPPORT_DMA
    (unsigned short)((RTS_BIT_DELAY_8BIT_DMA * REFERENCE_BIT_TIME) * (float)(115200 / 115200)),
    (unsigned short)((RTS_BIT_DELAY_7BIT_DMA * REFERENCE_BIT_TIME) * (float)(115200 / 115200))
    #endif
    },
};
#endif

/* =================================================================== */
/*                      local variable definitions                     */
/* =================================================================== */

#if defined MODBUS_SERIAL_INTERFACES && defined SERIAL_INTERFACE         // serial port variables
    static QUEUE_HANDLE    SerialHandle[MODBUS_SERIAL_INTERFACES] = {NO_ID_ALLOCATED};
    #if defined MODBUS_RS485_SUPPORT                                     // {V1.10}
    static unsigned char   ucAssertRTS[MODBUS_SERIAL_INTERFACES];
    static unsigned char   ucNegateRTS[MODBUS_SERIAL_INTERFACES];
    #endif
    static int             iModbusSerialState[MODBUS_SERIAL_INTERFACES] = {MODBUS_OFF};
    static QUEUE_TRANSFER  rxFrameLength[MODBUS_SERIAL_INTERFACES] = {0};

    #if defined MODBUS_RTU
    static _TIMER_INTERRUPT_SETUP timer_setup_1_5[MODBUS_SERIAL_INTERFACES];
    static _TIMER_INTERRUPT_SETUP timer_setup_3_5[MODBUS_SERIAL_INTERFACES];
    #endif

    #if defined MODBUS_RS485_RTS_SUPPORT
    static _TIMER_INTERRUPT_SETUP timer_setup_RTS_negate[MODBUS_SERIAL_INTERFACES];
    #endif

    #if !defined MODBUS_SHARED_SERIAL_INTERFACES
        #define MODBUS_SHARED_SERIAL_INTERFACES 0
    #endif

    #if defined MODBUS_SUPPORT_SERIAL_LINE_FUNCTIONS && defined MODBUS_SUPPORT_SERIAL_LINE_DIAGNOSTICS
    static unsigned char  ucListenOnlyMode[MODBUS_SERIAL_INTERFACES + MODBUS_SHARED_SERIAL_INTERFACES] = {0};
    static unsigned short usDiagnosticRegister[MODBUS_SERIAL_INTERFACES + MODBUS_SHARED_SERIAL_INTERFACES] = {0};
    static DIAGNOSTIC_COUNTERS DiagnosticCounters[MODBUS_SERIAL_INTERFACES + MODBUS_SHARED_SERIAL_INTERFACES] = {{0}};
        #if defined MODBUS_ASCII
        static unsigned char  ucEndOfMessageDelimiter[MODBUS_SERIAL_INTERFACES];
        #define END_OF_MESSAGE_DELIMITER(port) ucEndOfMessageDelimiter[port]
        #endif
    #else
        #define END_OF_MESSAGE_DELIMITER(port) ptrMODBUS_pars->ucLineFeedCharacter[port]
    #endif
#else
    #define MODBUS_SERIAL_INTERFACES        0
#endif
#if !defined MODBUS_TCP_SERVERS
    #define MODBUS_TCP_SERVERS              0
#endif
#if !defined MODBUS_TCP_MASTERS
    #define MODBUS_TCP_MASTERS              0
#endif

#if !defined MODBUS_SHARED_SERIAL_INTERFACES
    #define MODBUS_SHARED_SERIAL_INTERFACES 0
#endif
#if !defined MODBUS_SHARED_TCP_INTERFACES
    #define MODBUS_SHARED_TCP_INTERFACES 0
#endif

#define MODBUS_DELAY_LIST_LENGTH  (MODBUS_TCP_SERVERS + MODBUS_SERIAL_INTERFACES) // entry for each possible MODBUS slave
#define MODBUS_SHARED_INTERFACES  (MODBUS_SHARED_SERIAL_INTERFACES + MODBUS_SHARED_TCP_INTERFACES)

#if defined USE_MODBUS_SLAVE
    static MODBUS_CONFIG *ptrMODBUS_table[MODBUS_SERIAL_INTERFACES + MODBUS_TCP_SERVERS + MODBUS_SHARED_INTERFACES] = {0};
    static int ( *fnPreFunction[MODBUS_SERIAL_INTERFACES  + MODBUS_TCP_SERVERS + MODBUS_SHARED_INTERFACES] )( int, MODBUS_RX_FUNCTION *) = {0};
    static int ( *fnPostFunction[MODBUS_SERIAL_INTERFACES + MODBUS_TCP_SERVERS + MODBUS_SHARED_INTERFACES] )( int, MODBUS_RX_FUNCTION *) = {0};
    static int ( *fnUserFunction[MODBUS_SERIAL_INTERFACES + MODBUS_TCP_SERVERS + MODBUS_SHARED_INTERFACES] )( int, MODBUS_RX_FUNCTION *) = {0};

    #if MODBUS_SHARED_SERIAL_INTERFACES > 0 || MODBUS_SHARED_TCP_INTERFACES > 0
        static SHARED_MODBUS_PORT *ptrSharedPort = 0;
    #endif
    #if !defined NO_SLAVE_MODBUS_GET_COMM_EVENT_LOG || !defined NO_SLAVE_MODBUS_GET_COMM_EVENT_COUNTER
        static unsigned short usEventCounter[MODBUS_SERIAL_INTERFACES + MODBUS_TCP_SERVERS + MODBUS_SHARED_INTERFACES] = {0};
    #endif
#endif

#if defined USE_MODBUS_MASTER
    #if defined TCP_SLAVE_ROUTER
    int ( *fnMasterCallback[MODBUS_SERIAL_INTERFACES + MODBUS_TCP_SERVERS + MODBUS_TCP_MASTERS] )( int, MODBUS_RX_FUNCTION *) = {0};
    #else
    int ( *fnMasterCallback[MODBUS_SERIAL_INTERFACES + MODBUS_TCP_MASTERS] )( int, MODBUS_RX_FUNCTION *) = {0};
    #endif
    #if defined MODBUS_GATE_WAY_QUEUE
    static MODBUS_QUEUE modbus_queue[MODBUS_SERIAL_INTERFACES + MODBUS_TCP_SERVERS + MODBUS_TCP_MASTERS] = {{0}};
    #endif
#endif

#if defined MODBUS_TCP
    #if defined USE_MODBUS_SLAVE
        static MODBUS_TCP_CTL *ptrMODBUS_TCP = 0;
        #if MODBUS_TCP_SERVERS > 0
        static unsigned char usMODBUS_sessions_allocated = 0;
        #endif
    #endif
    #if MODBUS_TCP_MASTERS > 0
        static MODBUS_TCP_CTL *ptrMODBUS_master_TCP[MODBUS_TCP_MASTERS];
        static MODBUS_TCP_TX_FRAME *master_frame[MODBUS_TCP_MASTERS];
    #endif
#endif

#if (MODBUS_SERIAL_INTERFACES + MODBUS_TCP_MASTERS) > 1
    #define MULTIPLE_SW_TIMERS
    #if !defined GLOBAL_TIMER_TASK
        #error The MODBUS module requires GLOBAL_TIMER_TASK to be defined (in config.h) so that it can use multiple SW timers!!!
    #endif
#endif
    
#if defined MODBUS_GATE_WAY_ROUTING || defined MODBUS_GATE_WAY_QUEUE
    static ROUTE_ENTRY open_routes[MODBUS_SERIAL_INTERFACES + MODBUS_TCP_SERVERS + MODBUS_TCP_MASTERS] = {{0}};
#endif

#if defined MODBUS_DELAYED_RESPONSE
    static MODBUS_DELAY_LIST modbus_delay_entries[MODBUS_DELAY_LIST_LENGTH] = {{0}};
#endif

/* =================================================================== */
/*                 local function prototype declarations               */
/* =================================================================== */

#if MODBUS_SERIAL_INTERFACES > 0
    static QUEUE_HANDLE   fnOpenSerialInterface(unsigned char ucMODBUSport);
    #if defined MODBUS_ASCII
    static unsigned char  fnACSIIDecode2(unsigned char *ptrBytePair);
    static void           fnConvertToASCII(unsigned char ucValue, unsigned char *ptrBuf);
    static void           fnHandleASCIIInputs(unsigned char ucMODBUSport, unsigned char ucModbusSerialInputBuffer[MODBUS_RX_BUFFER_SIZE + 2]);
    #endif
    #if defined MODBUS_SUPPORT_SERIAL_LINE_FUNCTIONS && defined MODBUS_SUPPORT_SERIAL_LINE_DIAGNOSTICS
    static void fnDeleteEventLog(unsigned char ucMODBUSport);
    #endif
    #if defined MODBUS_SUPPORT_SERIAL_LINE_FUNCTIONS && defined USE_MODBUS_SLAVE
        #if !defined NO_SLAVE_MODBUS_GET_COMM_EVENT_LOG || !defined NO_SLAVE_MODBUS_GET_COMM_EVENT_COUNTER // {V1.02}
    static unsigned char  fnGetEvents(unsigned char *ptrBuf);
        #endif
    static void fnSendMODBUS_transparent_response(MODBUS_RX_FUNCTION *modbus_rx_function, unsigned char *ptrData, unsigned short usLength);
    #endif
    #if defined MODBUS_RTU
    static void           fnConfigureInterspaceTimes(QUEUE_HANDLE Channel);
    static unsigned short fnCRCRTUFrame(unsigned char *ptrFrame, QUEUE_TRANSFER rxFrameLength);
    #endif
    #if defined MODBUS_RS485_RTS_SUPPORT
    static void fnConfigRTS_delay(unsigned char ucMODBUSport, TTYTABLE *InterfaceParameters);
    #endif
    #if defined MODBUS_GATE_WAY_QUEUE && defined USE_MODBUS_MASTER
        static void fnNextSerialQueue(MODBUS_RX_FUNCTION *modbus_rx_function);
    #endif
#endif

#if defined MODBUS_GATE_WAY_ROUTING
    static int fnReverseSerialRoute(MODBUS_RX_FUNCTION *modbus_rx_function);
#endif

#if MODBUS_TCP_SERVERS > 0 && defined USE_MODBUS_SLAVE
    static int fnInitialiseMODBUStcp(unsigned char ucModbusPort, unsigned char ucSessions);
#endif

static int fnHandleMODBUS_input(MODBUS_RX_FUNCTION *modbus_rx_function);

#if defined USE_MODBUS_SLAVE
    #if defined MODBUS_TCP
        static int fnMODBUSListener(USOCKET Socket, unsigned char ucEvent, unsigned char *ucIp_Data, unsigned short usPortLen);
    #endif
    static int fnSendMODBUS_response(MODBUS_RX_FUNCTION *modbus_rx_function, void *ptrData, unsigned short usLength, unsigned char ucShift);
    #define SHIFT_BYTE_ALIGNED       0x00
    #define SHIFT_SHORT_WORD_ALIGNED 0x08
    #define SHIFT_LONG_WORD_ALIGNED  0x10
    #define NO_SHIFT_LENGTH_MUL_8    0x20
    #define NO_SHIFT_REGISTER_VALUE  0x40
    #define NO_SHIFT_FIFO_VALUE      0x80
    static int fnSendMODBUS_exception(MODBUS_RX_FUNCTION *modbus_rx_function, unsigned char ucExceptionCode);
#endif

#if defined USE_MODBUS_SLAVE && MODBUS_SHARED_INTERFACES > 0
    static unsigned char fnCheckSlaveAddresses(unsigned char ucMODBUSport, unsigned char ucAddress);
#endif


// MODBUS serial task
//
extern void fnMODBUS(TTASKTABLE *ptrTaskTable)
{
#if MODBUS_SERIAL_INTERFACES > 0
    static unsigned char ucModbusSerialInputBuffer[MODBUS_SERIAL_INTERFACES][MODBUS_RX_BUFFER_SIZE + 2];
#endif
    QUEUE_HANDLE PortIDInternal = ptrTaskTable->TaskID;                  // queue ID for task input
    unsigned char ucInputMessage[HEADER_LENGTH];                         // reserve space for receiving messages

    while (fnRead(PortIDInternal, ucInputMessage, HEADER_LENGTH)) {      // check input queue
        switch (ucInputMessage[MSG_SOURCE_TASK]) {
        case TIMER_EVENT:
            {
#if defined USE_MODBUS_MASTER
                int iReport;
#endif
                if (ucInputMessage[MSG_TIMER_EVENT] <= T_ASCII_INTER_CHAR_TIMEOUT) {
#if defined MODBUS_ASCII
                    iModbusSerialState[T_ASCII_INTER_CHAR_TIMEOUT - ucInputMessage[MSG_TIMER_EVENT]] = MODBUS_ASCII_HUNTING; // no character received within expected time when receiving in ASCII mode
#endif
                    break;
                }
#if defined USE_MODBUS_MASTER
                if (ucInputMessage[MSG_TIMER_EVENT] <= T_TIMER_BROADCAST) { // broadcast timeout
                    iReport = MODBUS_BROADCAST_TIMEOUT;
                    ucInputMessage[MSG_TIMER_EVENT] += (T_TIMER_SLAVE - T_TIMER_BROADCAST); // convert to conventional slave timeout
                }
                else {
                    iReport = MODBUS_NO_SLAVE_RESPONSE;                  // no response received back from a slave
                }
    #if MODBUS_TCP_MASTERS > 0
                if (ucInputMessage[MSG_TIMER_EVENT] <= (T_TIMER_SLAVE - MODBUS_SERIAL_INTERFACES)) { // TCP master timeout - no response from remote TCP slave
                    fnMODBUS_event_report(iReport, (unsigned char)(T_TIMER_SLAVE + MODBUS_TCP_SERVERS - ucInputMessage[MSG_TIMER_EVENT])); // report the failure on the MODBUS TCP port
                    break;
                }
    #endif
    #if MODBUS_SERIAL_INTERFACES > 0
                {                                                        // no answer from the serial port slave
        #if defined MODBUS_GATE_WAY_QUEUE && MODBUS_SERIAL_INTERFACES > 0
                    MODBUS_RX_FUNCTION modbus_rx_function;
        #endif
        #if defined MODBUS_GATE_WAY_ROUTING
                    open_routes[T_TIMER_SLAVE - ucInputMessage[MSG_TIMER_EVENT]].Valid = 0; // cancel return routing information on failure
        #endif
        #if defined MODBUS_GATE_WAY_QUEUE
                    modbus_rx_function.ucMODBUSport = (unsigned char)(T_TIMER_SLAVE - ucInputMessage[MSG_TIMER_EVENT]);
            #if defined MODBUS_ASCII
                    if (iModbusSerialState[modbus_rx_function.ucMODBUSport] >= MODBUS_ASCII_HUNTING) {
                        modbus_rx_function.ucSourceType = ASCII_SERIAL_INPUT;
                    }
                    else {
                        modbus_rx_function.ucSourceType = RTU_SERIAL_INPUT;
                    }
            #else
                    modbus_rx_function.ucSourceType = RTU_SERIAL_INPUT;
            #endif
                    fnNextSerialQueue(&modbus_rx_function);              // initiate following queued transmissions
        #endif
                    fnMODBUS_event_report(iReport, (unsigned char)(T_TIMER_SLAVE - ucInputMessage[MSG_TIMER_EVENT]));
                }
    #endif
#endif
            }
            break;

        case INTERRUPT_EVENT:
#if defined MODBUS_RTU
            if (ucInputMessage[MSG_INTERRUPT_EVENT] <= EVENT_VALID_FRAME_RECEIVED) { // valid RTU frame ready
                unsigned char ucMODBUSport = (EVENT_VALID_FRAME_RECEIVED - ucInputMessage[MSG_INTERRUPT_EVENT]);
    #if MODBUS_SHARED_SERIAL_INTERFACES > 0
                unsigned char ucSharedSlave = 0;
    #endif
                fnDebugMsg("RTU frame ");
                fnDebugDec(ucMODBUSport, 0);
                fnDebugMsg(" : ");
                fnDebugDec(rxFrameLength[ucMODBUSport], 0);
                if (rxFrameLength[ucMODBUSport] > MODBUS_RX_BUFFER_SIZE) { // frame too large
                    fnDebugMsg(" too long\n\r");
                    do {
                        fnRead(SerialHandle[ucMODBUSport], ucModbusSerialInputBuffer[ucMODBUSport], 1); // flush the over-length frame
                    } while (--rxFrameLength[ucMODBUSport]);
                    break;
                }
                fnRead(SerialHandle[ucMODBUSport], ucModbusSerialInputBuffer[ucMODBUSport], rxFrameLength[ucMODBUSport]); // read the present frame
                if (rxFrameLength[ucMODBUSport] < 4) {                   // invalid frame length - too short
                    fnDebugMsg(" too short\n\r");
                    break;
                }
    #if defined MODBUS_SUPPORT_SERIAL_LINE_FUNCTIONS && defined MODBUS_SUPPORT_SERIAL_LINE_DIAGNOSTICS
                DiagnosticCounters[ucMODBUSport].usMessageCounter++;     // count the number of messags detected on the bus, irrespecifive of whether addressed or not
    #endif
    #if defined USE_MODBUS_SLAVE && MODBUS_SHARED_SERIAL_INTERFACES > 0
                if ((ptrMODBUS_pars->ucModbusSerialPortMode[ucMODBUSport] & MODBUS_SERIAL_SLAVE) && // only slaves can have shared serial interfaces
                    ((ucSharedSlave = fnCheckSlaveAddresses(ucMODBUSport, ucModbusSerialInputBuffer[ucMODBUSport][0])) != 0)) {
                    goto _check_frame;                                   // a slave sharing the port is being addressed
                }
    #endif
                if (
    #if defined USE_MODBUS_MASTER
                    (!(ptrMODBUS_pars->ucModbusSerialPortMode[ucMODBUSport] & (MODBUS_SERIAL_MASTER | MODBUS_SERIAL_GATEWAY))) && // master or gateway receives all frames
    #endif
                    (ucModbusSerialInputBuffer[ucMODBUSport][0] != ptrMODBUS_pars->ucModbus_slave_address[ucMODBUSport]) && (ucModbusSerialInputBuffer[ucMODBUSport][0] != BROADCAST_MODBUS_ADDRESS)) {
                    fnDebugMsg(" not addressed\n\r");
                    break;                                               // not addressed so ignore
                }
    #if defined MODBUS_SUPPORT_SERIAL_LINE_FUNCTIONS && defined MODBUS_SUPPORT_SERIAL_LINE_DIAGNOSTICS
                DiagnosticCounters[ucMODBUSport].usSlaveMessageCounter++; // count the number of messages addressing slave
    #endif
    #if defined USE_MODBUS_SLAVE && MODBUS_SHARED_SERIAL_INTERFACES > 0
_check_frame:
    #endif
                if (fnCRCRTUFrame(ucModbusSerialInputBuffer[ucMODBUSport], (unsigned short)(rxFrameLength[ucMODBUSport])) != 0) {
    #if defined MODBUS_SUPPORT_SERIAL_LINE_FUNCTIONS && defined MODBUS_SUPPORT_SERIAL_LINE_DIAGNOSTICS
        #if defined USE_MODBUS_SLAVE && MODBUS_SHARED_SERIAL_INTERFACES > 0
                    if (ucSharedSlave != 0) {
                        ucSharedSlave -= MODBUS_TCP_SERVERS;
                    }
                    DiagnosticCounters[ucMODBUSport + ucSharedSlave].usCRC_errorCounter++; // count CRC errors on this serial interface
        #else
                    DiagnosticCounters[ucMODBUSport].usCRC_errorCounter++; // count CRC errors on this serial interface
        #endif
    #endif
                    fnDebugMsg(" bad CRC\n\r");
                    break;                                               // reject if check sum error
                }
                else {
                    MODBUS_RX_FUNCTION modbus_rx_function;               // build a temporary function block for subroutine use
                    modbus_rx_function.ucMODBUSport = ucMODBUSport;
    #if defined USE_MODBUS_SLAVE && MODBUS_SHARED_SERIAL_INTERFACES > 0
                    if (ucSharedSlave != 0) {
                        modbus_rx_function.ucMODBUS_Slaveport = ucSharedSlave;
        #if defined MODBUS_SUPPORT_SERIAL_LINE_DIAGNOSTICS
                        modbus_rx_function.ucMODBUS_Diagnostics_SlavePort = (ucSharedSlave - MODBUS_TCP_SERVERS);
        #endif
                    }
                    else {
                        modbus_rx_function.ucMODBUS_Slaveport = ucMODBUSport;
        #if defined MODBUS_SUPPORT_SERIAL_LINE_DIAGNOSTICS
                        modbus_rx_function.ucMODBUS_Diagnostics_SlavePort = ucMODBUSport;
        #endif
                    }
    #elif MODBUS_SHARED_INTERFACES > 0
                    modbus_rx_function.ucMODBUS_Slaveport = ucMODBUSport;
    #endif
                    modbus_rx_function.ucSourceAddress = ucModbusSerialInputBuffer[ucMODBUSport][0];
                    modbus_rx_function.ucMappedAddress = modbus_rx_function.ucSourceAddress;
                    modbus_rx_function.ucFunctionCode = ucModbusSerialInputBuffer[ucMODBUSport][1];
                    modbus_rx_function.ucSourceType = RTU_SERIAL_INPUT;
                    modbus_rx_function.data_content.usUserDataLength = (unsigned short)(rxFrameLength[ucMODBUSport] - 4);
                    modbus_rx_function.data_content.user_data = &ucModbusSerialInputBuffer[ucMODBUSport][2];
    #if defined MODBUS_DELAYED_RESPONSE
                    modbus_rx_function.ucDelayed = 0;
    #endif
    #if defined MODBUS_TCP
                    modbus_rx_function.modbus_session = 0;               // not originating from TCP path
    #endif
                    fnHandleMODBUS_input(&modbus_rx_function);           // perform generic MODBUS frame handling
    #if defined USE_MODBUS_SLAVE && MODBUS_SHARED_SERIAL_INTERFACES > 0
                    if (modbus_rx_function.ucSourceAddress == BROADCAST_MODBUS_ADDRESS) { // allow all shared slaves to process a broadcast
                        if (ptrSharedPort != 0) {
                            SHARED_MODBUS_PORT *ptrPort = ptrSharedPort;
                            while (ptrPort->ucMODBUSPort != 0xff) {
                                if (ptrPort->ucMODBUSPort == ucMODBUSport) {
                                    modbus_rx_function.ucMODBUS_Slaveport = (ucSharedSlave + MODBUS_SERIAL_INTERFACES + MODBUS_TCP_SERVERS);
        #if defined MODBUS_SUPPORT_SERIAL_LINE_DIAGNOSTICS
                                    modbus_rx_function.ucMODBUS_Diagnostics_SlavePort = (ucSharedSlave + MODBUS_SERIAL_INTERFACES);
        #endif
                                    fnHandleMODBUS_input(&modbus_rx_function); // perform generic MODBUS frame handling
                                }
                                if (++ucSharedSlave >= MODBUS_SHARED_INTERFACES) {
                                    break;
                                }
                                ptrPort++;
                            }
                        }
                    }
    #endif
                }
            }
            else if (ucInputMessage[MSG_INTERRUPT_EVENT] <= EVENT_INCOMPLETE_FRAME_RECEIVED) {
                fnDebugMsg("Incomplete frame on MODBUS port ");
                fnDebugDec((EVENT_INCOMPLETE_FRAME_RECEIVED - ucInputMessage[MSG_INTERRUPT_EVENT]), 0);
                fnDebugMsg("\n\r");
            }
            else if (ucInputMessage[MSG_INTERRUPT_EVENT] <= EVENT_BAD_FRAME_TERMINATED) {
                fnDebugMsg("Bad frame complete on MODBUS port ");
                fnDebugDec((EVENT_BAD_FRAME_TERMINATED - ucInputMessage[MSG_INTERRUPT_EVENT]), 0);
                fnDebugMsg("\n\r");
            }
#endif
            break;

        default:
            break;
        }
    }

#if defined MODBUS_ASCII                                                 // the task is woken on ASCII serial character receptions so handle inputs
    #if MODBUS_SERIAL_INTERFACES > 0                                     // {V1.11} corrected from MODBUS_SERIAL_INTERFACES > 1
    fnHandleASCIIInputs(0, ucModbusSerialInputBuffer[0]);
    #endif
    #if MODBUS_SERIAL_INTERFACES > 1
    fnHandleASCIIInputs(1, ucModbusSerialInputBuffer[1]);
    #endif
    #if MODBUS_SERIAL_INTERFACES > 2
    fnHandleASCIIInputs(2, ucModbusSerialInputBuffer[2]);
    #endif
    #if MODBUS_SERIAL_INTERFACES > 3
    fnHandleASCIIInputs(3, ucModbusSerialInputBuffer[3]);
    #endif
#endif
}                                                                        // end of MODBUS task

#if defined MODBUS_GATE_WAY_QUEUE
static QUEUE_HANDLE fnOpenGatewayQueue(unsigned char ucMODBUSport, QUEUE_TRANSFER queue_size)
{
    if (queue_size == 0) {
        return 0;
    }
    else {
    #if defined SUPPORT_FIFO_QUEUES                                      // {V1.24}
        return (fnOpen(TYPE_FIFO, FOR_I_O, (FIFO_LENGTH)queue_size));    // use inbuilt software FIFO 
    #else
        TTASKTABLE table;
        CHAR queue_name = (0xff - ucMODBUSport);
        table.pcTaskName = &queue_name;
        table.QueLength = queue_size;
        return (fnOpen(TYPE_QUEUE, FOR_READ, &table));
    #endif
    }
}
#endif

#if defined USE_MODBUS_SLAVE && MODBUS_SHARED_INTERFACES > 0
static unsigned char fnCheckSlaveAddresses(unsigned char ucMODBUSport, unsigned char ucAddress)
{
    unsigned char ucPortAdressed = 0;
    if (ptrSharedPort != 0) {
        SHARED_MODBUS_PORT *ptrPort = ptrSharedPort;
        int iPortReference = 0;
        while (ptrPort->ucMODBUSPort != 0xff) {
            if (ptrPort->ucMODBUSPort == ucMODBUSport) {
        #if defined MODBUS_SUPPORT_SERIAL_LINE_FUNCTIONS && defined MODBUS_SUPPORT_SERIAL_LINE_DIAGNOSTICS
                if (ucMODBUSport < MODBUS_SERIAL_INTERFACES) {
                    DiagnosticCounters[iPortReference + MODBUS_SERIAL_INTERFACES].usMessageCounter++; // count the number of messags detected on the bus, irrespecifive of whether addressed or not
                }
        #endif
                if (ucAddress == BROADCAST_MODBUS_ADDRESS) {             // global reception for all shared ports
        #if defined MODBUS_SUPPORT_SERIAL_LINE_FUNCTIONS && defined MODBUS_SUPPORT_SERIAL_LINE_DIAGNOSTICS
                    if (ucMODBUSport < MODBUS_SERIAL_INTERFACES) {
                        DiagnosticCounters[iPortReference + MODBUS_SERIAL_INTERFACES].usSlaveMessageCounter++; // count the number of messages addressing slave
                    }
        #endif
                }
                else if (ucAddress == ptrPort->ucSlaveAdress) {          // message for this port
        #if defined MODBUS_SUPPORT_SERIAL_LINE_FUNCTIONS && defined MODBUS_SUPPORT_SERIAL_LINE_DIAGNOSTICS
                    if (ucMODBUSport < MODBUS_SERIAL_INTERFACES) {
                        DiagnosticCounters[iPortReference + MODBUS_SERIAL_INTERFACES].usSlaveMessageCounter++; // count the number of messages addressing slave
                    }
        #endif
                    ucPortAdressed = (iPortReference + MODBUS_SERIAL_INTERFACES + MODBUS_TCP_SERVERS);
        #if defined MODBUS_SUPPORT_SERIAL_LINE_DIAGNOSTICS
                    if (ucMODBUSport >= MODBUS_SERIAL_INTERFACES) {
                        break;                                           // when no diagnostic counter to be maintained return the shared port immediately
                    }
        #else
                    break;
        #endif
                }
            }
            if (++iPortReference >= MODBUS_SHARED_INTERFACES) {
                break;
            }
            ptrPort++;
        }
    }
    return (ucPortAdressed);                                             // return the addressed port reference
}
#endif

// This routine is called to initialise the MODBUS serial interface and set the user interface
//
extern int fnInitialiseMODBUS_port(unsigned char ucMODBUSport, const MODBUS_CONFIG *ptrMODBUS_config, const MODBUS_CALLBACKS *ptrModbus_callbacks, int ( *master_callback )( int iType, MODBUS_RX_FUNCTION *))
{
    if (ucMODBUSport < MODBUS_SERIAL_INTERFACES) {                       // modbus serial interface port being initialise
    #if MODBUS_SERIAL_INTERFACES > 0
        #if defined MODBUS_ASCII
            #if defined MODBUS_SUPPORT_SERIAL_LINE_FUNCTIONS && defined MODBUS_SUPPORT_SERIAL_LINE_DIAGNOSTICS
        ucEndOfMessageDelimiter[ucMODBUSport] = ptrMODBUS_pars->ucLineFeedCharacter[ucMODBUSport]; // initialise variable delimiter
            #endif
        if (ptrMODBUS_pars->ucModbusSerialPortMode[ucMODBUSport] & MODBUS_MODE_ASCII) {
            iModbusSerialState[ucMODBUSport] = MODBUS_ASCII_HUNTING;     // ASCII
        }
        else {
            iModbusSerialState[ucMODBUSport] = MODBUS_IDLE;              // RTU idle
        }
        #endif
        SerialHandle[ucMODBUSport] = fnOpenSerialInterface(ucMODBUSport);// configure and open the serial interface
        if (SerialHandle[ucMODBUSport] == NO_ID_ALLOCATED) {
            return -1;                                                   // port could not be opened
        }
        fnDriver(SerialHandle[ucMODBUSport], (TX_ON | RX_ON), 0);        // enable rx and tx
        #if defined MODBUS_RS485_SUPPORT                                 // {V1.10}
        if (ptrMODBUS_pars->ucModbusSerialPortMode[ucMODBUSport] & (MODBUS_RS485_NEGATIVE | MODBUS_RS485_POSITIVE)) {
            unsigned short usMode;
            if (ptrMODBUS_pars->ucModbusSerialPortMode[ucMODBUSport] & MODBUS_RS485_POSITIVE) {
                usMode = (MODIFY_CONTROL | CONFIG_RTS_PIN | SET_RS485_MODE);
                ucAssertRTS[ucMODBUSport] = (MODIFY_CONTROL | CLEAR_RTS);
                ucNegateRTS[ucMODBUSport] = (MODIFY_CONTROL | SET_RTS);
            }
            else {
                usMode = (MODIFY_CONTROL | CONFIG_RTS_PIN | SET_RS485_MODE | SET_RS485_NEG);
                ucAssertRTS[ucMODBUSport] = (MODIFY_CONTROL | SET_RTS);
                ucNegateRTS[ucMODBUSport] = (MODIFY_CONTROL | CLEAR_RTS);
            }
            fnDriver(SerialHandle[ucMODBUSport], usMode, 0);             // configure RTS pin for control use
            if (ptrMODBUS_pars->ucModbusSerialPortMode[ucMODBUSport] & MODBUS_RS485_POSITIVE) {
                fnDriver(SerialHandle[ucMODBUSport], (MODIFY_CONTROL | SET_RTS), 0); // initially '0'
            }
        }
        #endif
        #if defined USE_MODBUS_MASTER
        fnMasterCallback[ucMODBUSport] = master_callback;
            #if defined MODBUS_GATE_WAY_QUEUE
        modbus_queue[ucMODBUSport].queue_handle = fnOpenGatewayQueue(ucMODBUSport, ptrMODBUS_pars->serial_master_queue_size[ucMODBUSport]);
            #endif
        #endif
    #endif
    }
    #if MODBUS_TCP_MASTERS > 0
    else if (ucMODBUSport >= (MODBUS_SERIAL_INTERFACES + MODBUS_TCP_SERVERS)){ // modbus TCP master port being initialised
        unsigned char ucMasterTCP_port = (ucMODBUSport - (MODBUS_SERIAL_INTERFACES + MODBUS_TCP_SERVERS));
        ptrMODBUS_master_TCP[ucMasterTCP_port] = uMalloc(sizeof(MODBUS_TCP_CTL)); // allocate a control structure to each TCP master port
        master_frame[ucMasterTCP_port] = uMalloc(sizeof(MODBUS_TCP_TX_FRAME)); // allocate transmission buffer space for each TCP master port
        master_frame[ucMasterTCP_port]->modbus_header.ucTransactionIdentifier[0] = 0xff;
        master_frame[ucMasterTCP_port]->modbus_header.ucTransactionIdentifier[1] = 0xff;
        ptrMODBUS_master_TCP[ucMasterTCP_port]->OwnerTCPSocket = -1;
        ptrMODBUS_master_TCP[ucMasterTCP_port]->ucPort = ucMODBUSport;
        ptrMODBUS_master_TCP[ucMasterTCP_port]->tx_modbus_frame = master_frame[ucMasterTCP_port];
        #if defined TCP_SLAVE_ROUTER
        fnMasterCallback[ucMODBUSport] = master_callback;
        #else
        fnMasterCallback[ucMODBUSport - MODBUS_TCP_SERVERS] = master_callback;
        #endif
        #if defined MODBUS_GATE_WAY_QUEUE
        modbus_queue[ucMODBUSport].queue_handle = fnOpenGatewayQueue(ucMODBUSport, ptrMODBUS_pars->tcp_queue_size[ucMODBUSport - MODBUS_SERIAL_INTERFACES]);
        #endif
        return 0;                                                        // return to avoid further initialisations
    }
    #endif
    else {                                                               // modbus TCP slave port being initialised
    #if MODBUS_TCP_SERVERS > 0 && defined USE_MODBUS_SLAVE
        unsigned char ucSessions;
        unsigned char ucTCP_slave_port = (ucMODBUSport - MODBUS_SERIAL_INTERFACES);
        switch (ucTCP_slave_port) {
        #if defined MODBUS_SOCKETS_1
        case 1:
            ucSessions = MODBUS_SOCKETS_1;
            break;
        #endif
        #if defined MODBUS_SOCKETS_2
        case 2:
            ucSessions = MODBUS_SOCKETS_2;
            break;
        #endif
        #if defined MODBUS_SOCKETS_3
        case 3:
            ucSessions = MODBUS_SOCKETS_3;
            break;
        #endif
        #if defined MODBUS_SOCKETS_4
        case 4:
            ucSessions = MODBUS_SOCKETS_4;
            break;
        #endif
        default:
            ucSessions = MODBUS_SOCKETS_0;
            break;
        }
        #if defined TCP_SLAVE_ROUTER
        fnMasterCallback[ucMODBUSport] = master_callback;
        #endif
        fnInitialiseMODBUStcp(ucMODBUSport, ucSessions);
        #if defined MODBUS_GATE_WAY_QUEUE
        modbus_queue[ucMODBUSport].queue_handle = fnOpenGatewayQueue(ucMODBUSport, ptrMODBUS_pars->tcp_queue_size[ucMODBUSport - MODBUS_SERIAL_INTERFACES]);
        #endif
    #endif
    }
    #if defined USE_MODBUS_SLAVE
    if (ptrModbus_callbacks != 0) {
        fnPreFunction[ucMODBUSport] = ptrModbus_callbacks->pre_function_call;
        fnPostFunction[ucMODBUSport] = ptrModbus_callbacks->post_function_call;
        fnUserFunction[ucMODBUSport] = ptrModbus_callbacks->user_function_call;
    }
    ptrMODBUS_table[ucMODBUSport] = (MODBUS_CONFIG *)ptrMODBUS_config;   // save pointer to MODBUS table
    #endif
    return 0;                                                            // successful
}

#if defined USE_MODBUS_SLAVE && MODBUS_SHARED_INTERFACES > 0
// Add a slave interface to an existing MODBUS port (serial or TCP)
//
extern int fnShareMODBUS_port(unsigned char ucMODBUSport, const MODBUS_CONFIG *ptrMODBUS_config, const MODBUS_CALLBACKS *ptrModbus_callbacks, unsigned char ucSlaveAddress)
{
    static unsigned char ucSharedPortCnt = 0;
    SHARED_MODBUS_PORT *ptrPort;
    if (ucSharedPortCnt >= MODBUS_SHARED_INTERFACES) {
        return MODBUS_MAXIMUM_SHARES_EXCEEDED;
    }
    if (ptrSharedPort == 0) {
        ptrSharedPort = uMalloc(MODBUS_SHARED_INTERFACES*(sizeof(SHARED_MODBUS_PORT)));
        uMemset(ptrSharedPort, 0xff, MODBUS_SHARED_INTERFACES*(sizeof(SHARED_MODBUS_PORT))); // initialise to invalid ports
    }
    ptrPort = (ptrSharedPort + ucSharedPortCnt);
    if (ptrModbus_callbacks != 0) {
        fnPreFunction[MODBUS_SERIAL_INTERFACES + MODBUS_TCP_SERVERS + ucSharedPortCnt] = ptrModbus_callbacks->pre_function_call;
        fnPostFunction[MODBUS_SERIAL_INTERFACES + MODBUS_TCP_SERVERS + ucSharedPortCnt] = ptrModbus_callbacks->post_function_call;
        fnUserFunction[MODBUS_SERIAL_INTERFACES + MODBUS_TCP_SERVERS + ucSharedPortCnt] = ptrModbus_callbacks->user_function_call;
    }
    ptrMODBUS_table[MODBUS_SERIAL_INTERFACES + MODBUS_TCP_SERVERS + ucSharedPortCnt] = (MODBUS_CONFIG *)ptrMODBUS_config; // save pointer to MODBUS table
    ptrPort->ucMODBUSPort = ucMODBUSport;                                // enter the sharing details
    ptrPort->ucSlaveAdress = ucSlaveAddress;
    ucSharedPortCnt++;
    return 0;
}
#endif

#if MODBUS_SERIAL_INTERFACES > 0

// Convert between MODBUS UART port and the UART hardware channel
//
static QUEUE_HANDLE fnGetUARTChannel(unsigned char ucMODBUSport)
{
    #if MODBUS_SERIAL_INTERFACES > 1
    if (ucMODBUSport == 1) {
        return MODBUS_UART_1;                                            // HW UART channel used by MODBUS UART port 1
    }
        #if MODBUS_SERIAL_INTERFACES > 2
    else if (ucMODBUSport == 2) {
        return MODBUS_UART_2;                                            // HW UART channel used by MODBUS UART port 2
    }
            #if MODBUS_SERIAL_INTERFACES > 3
    else if (ucMODBUSport == 3) {
        return MODBUS_UART_3;                                            // HW UART channel used by MODBUS UART port 3
    }
            #endif
        #endif
    #endif
    return MODBUS_UART_0;                                                // HW UART channel used by MODBUS UART port 0
}


// Configure and open a MODBUS serial interface
//
static QUEUE_HANDLE fnOpenSerialInterface(unsigned char ucMODBUSport)
{
    TTYTABLE tInterfaceParameters;                                       // table for passing information to driver
    tInterfaceParameters.Channel = fnGetUARTChannel(ucMODBUSport);       // get the HW UART channel used for this MODBUS UART port
#if defined MODBUS_USB_SLAVE                                             // {V1.02}
    if (tInterfaceParameters.Channel >= NUMBER_SERIAL) {
        return USBPortID_comms[tInterfaceParameters.Channel - NUMBER_SERIAL + MODBUS_USB_INTERFACE_BASE]; // {V1.25} use USB interface instead of a UART
    }
#endif
    tInterfaceParameters.ucSpeed = ptrMODBUS_pars->ucSerialSpeed[ucMODBUSport]; // baud rate
    tInterfaceParameters.Rx_tx_sizes.RxQueueSize = MODBUS_RX_ASCII_SIZE; // input buffer size
    tInterfaceParameters.Rx_tx_sizes.TxQueueSize = MODBUS_TX_ASCII_SIZE; // output buffer size
    #if defined SUPPORT_FLOW_HIGH_LOW
    tInterfaceParameters.ucFlowHighWater = 100;                          // set the flow control high and low water levels in %
    tInterfaceParameters.ucFlowLowWater  = 0;
    #endif
    tInterfaceParameters.Config = ptrMODBUS_pars->usSerialMode[ucMODBUSport];
    if (ptrMODBUS_pars->ucModbusSerialPortMode[ucMODBUSport] & MODBUS_MODE_ASCII) {
    #if defined MODBUS_ASCII
        #if defined STRICT_MODBUS_SERIAL_MODE
        tInterfaceParameters.Config |= (CHAR_7);                         // ASCII must use 7 bit characters
        #endif
        tInterfaceParameters.Config &= ~RTU_RX_MODE;                     // no RTU receiver timing
        tInterfaceParameters.Task_to_wake = OWN_TASK;                    // wake self when complete message have been received
    #endif
    }
    #if defined MODBUS_RTU
    else {
        #if defined STRICT_MODBUS_SERIAL_MODE
        tInterfaceParameters.Config &= ~(CHAR_7);                        // RTU must use 8 bit characters
        #endif
        tInterfaceParameters.Config |= RTU_RX_MODE;                      // activate RTU receiver timing
        fnConfigureInterspaceTimes(ucMODBUSport);
        tInterfaceParameters.Task_to_wake = 0;                           // no wake up since it is performed by timer
    }
    #endif
    #if defined STRICT_MODBUS_SERIAL_MODE
    if ((tInterfaceParameters.Config & (RS232_EVEN_PARITY | RS232_ODD_PARITY)) == 0) { // if no parity selected, force two stop bits
        tInterfaceParameters.Config &= ~ONE_HALF_STOPS;
        tInterfaceParameters.Config |= TWO_STOPS;
    }
    #endif
    #if defined SERIAL_SUPPORT_DMA
    tInterfaceParameters.ucDMAConfig = UART_TX_DMA;                      // activate DMA on transmission
    #endif
    #if defined MODBUS_RS485_SUPPORT                                     // {V1.10}
    if (ptrMODBUS_pars->ucModbusSerialPortMode[ucMODBUSport] & (MODBUS_RS485_NEGATIVE | MODBUS_RS485_POSITIVE)) {
        #if defined _HW_SAM7X
        if (tInterfaceParameters.Channel == 2) {                         // only required when DBGU channel is used, which has no automatic RTS control
            tInterfaceParameters.Config |= INFORM_ON_FRAME_TRANSMISSION;
        }
        #elif defined _HW_AVR32
        if (tInterfaceParameters.Channel != 1) {                         // only required when USART1 is not used, since they have has no automatic RTS control
            tInterfaceParameters.Config |= INFORM_ON_FRAME_TRANSMISSION;
        }
        #else
            #if NUMBER_EXTERNAL_SERIAL > 0                               // {V1.16}
        if (tInterfaceParameters.Channel < NUMBER_SERIAL) {              // not required when using external UART (assumes that this supports automatic control)
            tInterfaceParameters.Config |= INFORM_ON_FRAME_TRANSMISSION;
        }
            #else
        tInterfaceParameters.Config |= INFORM_ON_FRAME_TRANSMISSION;
            #endif
        #endif
        #if !defined AUTO_RS485_RTS_SUPPORT && (!defined _LPC23XX && !defined _LPC17XX && !defined KINETIS_KE && !defined KINETIS_KL) // {V1.19} {V1.27}
        fnConfigRTS_delay(ucMODBUSport, &tInterfaceParameters);
        #endif
    }
    #endif
    return (fnOpen(TYPE_TTY, FOR_I_O, &tInterfaceParameters));
}

    #if defined MODBUS_ASCII                                             // only when ASCII support is enabled

// Handle possible ASCII input on the MODBUS serial port
//
static void fnHandleASCIIInputs(unsigned char ucMODBUSport, unsigned char ucModbusSerialInputBuffer[MODBUS_RX_BUFFER_SIZE + 2])
{
    static unsigned short usModbusInputLength[MODBUS_SERIAL_INTERFACES] = {0};
    if (iModbusSerialState[ucMODBUSport] >= MODBUS_ASCII_HUNTING) {      // only if in ASCII mode
    #if defined USE_MODBUS_SLAVE && MODBUS_SHARED_SERIAL_INTERFACES > 0
        static unsigned char ucSharedASCIISlave[MODBUS_SERIAL_INTERFACES];
    #endif
        static unsigned char ucLRC[MODBUS_SERIAL_INTERFACES];
        static unsigned char ucLastByte[MODBUS_SERIAL_INTERFACES];
        unsigned char ucInputByte;
        while (fnRead(SerialHandle[ucMODBUSport], &ucInputByte, 1)) {    // read each received byte from bus
        #if defined MULTIPLE_SW_TIMERS
            uTaskerGlobalMonoTimer(OWN_TASK, ptrMODBUS_pars->inter_character_limit[ucMODBUSport],  (unsigned char)(T_ASCII_INTER_CHAR_TIMEOUT + ucMODBUSport));  // start an inter-character timer
        #else
            uTaskerMonoTimer(OWN_TASK, ptrMODBUS_pars->inter_character_limit[ucMODBUSport], (unsigned char)(T_ASCII_INTER_CHAR_TIMEOUT + ucMODBUSport)); // start an inter-character timer
        #endif
            switch (iModbusSerialState[ucMODBUSport]) {
            case MODBUS_ASCII_HUNTING:                                   // searching for the start character
                if (ucInputByte == MODBUS_START_BYTE) {
                    usModbusInputLength[ucMODBUSport] = 0;
                    iModbusSerialState[ucMODBUSport] = MODBUS_ASCII_SYNCHRONISED; // start byte has been found
                }
                else {
        #if defined MULTIPLE_SW_TIMERS                                   // {V1.26}
                    uTaskerGlobalStopTimer(OWN_TASK, (unsigned char)(T_ASCII_INTER_CHAR_TIMEOUT + ucMODBUSport));
        #else
                    uTaskerStopTimer(OWN_TASK);                          // ignore
        #endif
                }
                continue;

            case MODBUS_ASCII_SYNCHRONISED:                              // collecting the start address
                ucModbusSerialInputBuffer[usModbusInputLength[ucMODBUSport]] = ucInputByte;
                if (usModbusInputLength[ucMODBUSport] == 1) {            // the address has been collected
                    unsigned char ucAddress = fnACSIIDecode2(ucModbusSerialInputBuffer); // extract the binary value
        #if defined MODBUS_SUPPORT_SERIAL_LINE_FUNCTIONS && defined MODBUS_SUPPORT_SERIAL_LINE_DIAGNOSTICS
                    DiagnosticCounters[ucMODBUSport].usMessageCounter++; // count the number of messags detected on the bus, irrespecifive of whether being addressed or not
        #endif
        #if defined USE_MODBUS_SLAVE && MODBUS_SHARED_SERIAL_INTERFACES > 0
                    if (ptrMODBUS_pars->ucModbusSerialPortMode[ucMODBUSport] & MODBUS_SERIAL_SLAVE) { // only slaves can have shared serial interfaces
                        ucSharedASCIISlave[ucMODBUSport] = fnCheckSlaveAddresses(ucMODBUSport, ucAddress);
                    }
                    else {
                        ucSharedASCIISlave[ucMODBUSport] = 0;
                    }
        #endif
                    if (
        #if defined USE_MODBUS_SLAVE && MODBUS_SHARED_SERIAL_INTERFACES > 0
                        (ucSharedASCIISlave[ucMODBUSport] == 0) &&
        #endif
        #if defined USE_MODBUS_MASTER
                        (!(ptrMODBUS_pars->ucModbusSerialPortMode[ucMODBUSport] & (MODBUS_SERIAL_MASTER | MODBUS_SERIAL_GATEWAY))) && // master or gateway receives all frames
        #endif
                        ((ucAddress != ptrMODBUS_pars->ucModbus_slave_address[ucMODBUSport]) && (ucAddress != BROADCAST_MODBUS_ADDRESS))) {
                        iModbusSerialState[ucMODBUSport] = MODBUS_ASCII_HUNTING; // neither addresses directly nor as broadcast so revert to hunting state
        #if defined MULTIPLE_SW_TIMERS                                   // {V1.26}
                        uTaskerGlobalStopTimer(OWN_TASK, (unsigned char)(T_ASCII_INTER_CHAR_TIMEOUT + ucMODBUSport));
        #else
                        uTaskerStopTimer(OWN_TASK);                      // ignore
        #endif
                    }
                    else {
                        ucModbusSerialInputBuffer[0] = ucAddress;
                        ucLRC[ucMODBUSport] =  ucAddress;
                        iModbusSerialState[ucMODBUSport] = MODBUS_ASCII_ADDRESSED;
        #if defined MODBUS_SUPPORT_SERIAL_LINE_FUNCTIONS && defined MODBUS_SUPPORT_SERIAL_LINE_DIAGNOSTICS
            #if defined USE_MODBUS_SLAVE && MODBUS_SHARED_SERIAL_INTERFACES > 0
                        if (ucSharedASCIISlave[ucMODBUSport] == 0) {
            #endif
                            DiagnosticCounters[ucMODBUSport].usSlaveMessageCounter++; // count the number of messages addressing slave
            #if defined USE_MODBUS_SLAVE && MODBUS_SHARED_SERIAL_INTERFACES > 0
                        }
            #endif
        #endif
                    }
                    continue;
                }
                break;

            case MODBUS_ASCII_ADDRESSED:                                 // collect content
            case MODBUS_ASCII_ADDRESSED_2:
                if (ucInputByte == MODBUS_CARRIAGE_RETURN) {             // input message terminating
                    iModbusSerialState[ucMODBUSport] = MODBUS_ASCII_TERMINATING;
                    continue;
                }
                ucModbusSerialInputBuffer[usModbusInputLength[ucMODBUSport]] = ucInputByte;
                if (MODBUS_ASCII_ADDRESSED == iModbusSerialState[ucMODBUSport]) {
                    if ((usModbusInputLength[ucMODBUSport] & 0x01) == 0) {
                        usModbusInputLength[ucMODBUSport]--;
                        ucLastByte[ucMODBUSport] = fnACSIIDecode2(&ucModbusSerialInputBuffer[usModbusInputLength[ucMODBUSport]]); // extract the binary value
                        ucModbusSerialInputBuffer[usModbusInputLength[ucMODBUSport]] = ucLastByte[ucMODBUSport];
                        ucLRC[ucMODBUSport] += ucLastByte[ucMODBUSport];
                        iModbusSerialState[ucMODBUSport] = MODBUS_ASCII_ADDRESSED_2;
                    }
                }
                else {
                    if (usModbusInputLength[ucMODBUSport] & 0x01) {
                        usModbusInputLength[ucMODBUSport]--;
                        ucLastByte[ucMODBUSport] = fnACSIIDecode2(&ucModbusSerialInputBuffer[usModbusInputLength[ucMODBUSport]]); // extract the binary value
                        ucModbusSerialInputBuffer[usModbusInputLength[ucMODBUSport]] = ucLastByte[ucMODBUSport];
                        ucLRC[ucMODBUSport] += ucLastByte[ucMODBUSport];
                        iModbusSerialState[ucMODBUSport] = MODBUS_ASCII_ADDRESSED;
                    }
                }
                break;

            case MODBUS_ASCII_TERMINATING:
                if (ucInputByte == END_OF_MESSAGE_DELIMITER(ucMODBUSport)) { // check the termination character
                    ucLRC[ucMODBUSport] -= ucLastByte[ucMODBUSport];
                    ucLRC[ucMODBUSport] = ~ucLRC[ucMODBUSport];
                    if ((unsigned char)(ucLRC[ucMODBUSport] + 1) == ucLastByte[ucMODBUSport]) {
                        MODBUS_RX_FUNCTION modbus_rx_function;            // build a temporary function block for subroutine use
                        modbus_rx_function.ucMODBUSport = ucMODBUSport;
        #if defined USE_MODBUS_SLAVE && MODBUS_SHARED_SERIAL_INTERFACES > 0
                        if (ucSharedASCIISlave[ucMODBUSport] != 0) {
                            modbus_rx_function.ucMODBUS_Slaveport = ucSharedASCIISlave[ucMODBUSport];
            #if defined MODBUS_SUPPORT_SERIAL_LINE_DIAGNOSTICS
                            modbus_rx_function.ucMODBUS_Diagnostics_SlavePort = (ucSharedASCIISlave[ucMODBUSport] - MODBUS_TCP_SERVERS);
            #endif
                        }
                        else {
                            modbus_rx_function.ucMODBUS_Slaveport = ucMODBUSport;
            #if defined MODBUS_SUPPORT_SERIAL_LINE_DIAGNOSTICS
                            modbus_rx_function.ucMODBUS_Diagnostics_SlavePort = ucMODBUSport;
            #endif
                        }
        #endif
                        modbus_rx_function.ucSourceAddress = ucModbusSerialInputBuffer[0];
                        modbus_rx_function.ucMappedAddress = modbus_rx_function.ucSourceAddress;
                        modbus_rx_function.ucFunctionCode = ucModbusSerialInputBuffer[1];
                        modbus_rx_function.ucSourceType = ASCII_SERIAL_INPUT;
                        modbus_rx_function.data_content.usUserDataLength = (usModbusInputLength[ucMODBUSport] - 3);
                        modbus_rx_function.data_content.user_data = &ucModbusSerialInputBuffer[2];
        #if defined MODBUS_DELAYED_RESPONSE
                        modbus_rx_function.ucDelayed = 0;
        #endif
        #if defined MODBUS_TCP
                        modbus_rx_function.modbus_session = 0;           // not originating from TCP path
        #endif
                        fnHandleMODBUS_input(&modbus_rx_function);       // perform generic MODBUS frame handling
        #if defined USE_MODBUS_SLAVE && MODBUS_SHARED_SERIAL_INTERFACES > 0
                        if (modbus_rx_function.ucSourceAddress == BROADCAST_MODBUS_ADDRESS) { // allow all shared slaves to process a broadcast
                            if (ptrSharedPort != 0) {
                                SHARED_MODBUS_PORT *ptrPort = ptrSharedPort;
                                unsigned char ucSharedSlave = 0;
                                while (ptrPort->ucMODBUSPort != 0xff) {
                                    if (ptrPort->ucMODBUSPort == ucMODBUSport) {
                                        modbus_rx_function.ucMODBUS_Slaveport = (ucSharedSlave + MODBUS_SERIAL_INTERFACES + MODBUS_TCP_SERVERS);
            #if defined MODBUS_SUPPORT_SERIAL_LINE_DIAGNOSTICS
                                        modbus_rx_function.ucMODBUS_Diagnostics_SlavePort = (ucSharedSlave + MODBUS_SERIAL_INTERFACES);
            #endif
                                        fnHandleMODBUS_input(&modbus_rx_function); // perform generic MODBUS frame handling
                                    }
                                    if (++ucSharedSlave >= MODBUS_SHARED_INTERFACES) {
                                        break;
                                    }
                                    ptrPort++;
                                }
                            }
                        }
        #endif
                    }
    #if defined MODBUS_SUPPORT_SERIAL_LINE_FUNCTIONS && defined MODBUS_SUPPORT_SERIAL_LINE_DIAGNOSTICS
                    else {
        #if defined USE_MODBUS_SLAVE && MODBUS_SHARED_SERIAL_INTERFACES > 0
                        DiagnosticCounters[ucMODBUSport + ucSharedASCIISlave[ucMODBUSport] - MODBUS_TCP_SERVERS].usCRC_errorCounter++; // count CRC errors on this serial interface
        #else
                        DiagnosticCounters[ucMODBUSport].usCRC_errorCounter++; // count CRC errors on this serial interface
        #endif
                    }
    #endif
                }
                iModbusSerialState[ucMODBUSport] = MODBUS_ASCII_HUNTING;
        #if defined MULTIPLE_SW_TIMERS                                   // {V1.26}
                uTaskerGlobalStopTimer(OWN_TASK, (unsigned char)(T_ASCII_INTER_CHAR_TIMEOUT + ucMODBUSport));
        #else
                uTaskerStopTimer(OWN_TASK);                              // ignore
        #endif
                break;
            }

            if (++usModbusInputLength[ucMODBUSport] > (MODBUS_RX_BUFFER_SIZE + 2)) { // protect from overlong reception frames
        #if defined MULTIPLE_SW_TIMERS                                   // {V1.26}
                uTaskerGlobalStopTimer(OWN_TASK, (unsigned char)(T_ASCII_INTER_CHAR_TIMEOUT + ucMODBUSport));
        #else
                uTaskerStopTimer(OWN_TASK);                              // ignore
        #endif
                iModbusSerialState[ucMODBUSport] = MODBUS_ASCII_HUNTING; // ignore all received content and start looking for the next frame
            }
        }
    }
}


// Extract the binary value from two ASCII input bytes
//
static unsigned char fnACSIIDecode2(unsigned char *ptrInput)
{
    unsigned char ucReturn;
    unsigned char ucBinary = (*ptrInput - '0');
    if (ucBinary > 9) {
        ucBinary = ((*ptrInput - 'A') + 10);
    }
    ptrInput++;
    ucBinary <<= 4;
    ucReturn = (*ptrInput - '0');
    if (ucReturn > 9) {
        ucReturn = ((*ptrInput - 'A') + 10);
    }
    return (ucBinary | ucReturn);
}

// Convert a byte to its ASCII hex equivalent and put it input the output buffer
//
static void fnConvertToASCII(unsigned char ucValue, unsigned char *ptrBuf)
{
    unsigned char ucOutput = ((ucValue >> 4) + '0');
    if (ucOutput > '9') {
        ucOutput += ('A' - ('9' + 1));
    }
    *ptrBuf++ = ucOutput;
    ucOutput = ((ucValue & 0x0f) + '0');
    if (ucOutput > '9') {
        ucOutput += ('A' - ('9' + 1));
    }
    *ptrBuf = ucOutput;
}
    #endif                                                               // endif MODBUS_ASCII

    #if defined MODBUS_RTU                                               // only when RTU support is enabled
static unsigned char fnMapMODBUSport(QUEUE_HANDLE Channel)
{
        #if defined MODBUS_UART_1
    if (Channel == MODBUS_UART_1) {
        return 1;
    }
        #endif
        #if defined MODBUS_UART_2
    if (Channel == MODBUS_UART_2) {
        return 2;
    }
        #endif
        #if defined MODBUS_UART_3
    if (Channel == MODBUS_UART_3) {
        return 3;
    }
        #endif
    return 0;
}

// Calculate the check sum of an RTU type frame
//
static unsigned short fnCRCRTUFrame(unsigned char *ptrFrame, QUEUE_TRANSFER rxFrameLength)
{
        #if defined _KINETIS && defined MODBUS_CRC_USING_INTERNAL_CRC     // untested kinetis CRC usage            
    /*void ModbusSlave::_crc_append(uint8_t* buffer, uint8_t* length)  
    { */ 
        /*CRC0->GPOLY_ACCESS16BIT.GPOLYL*/ CRC_GPOLY = 0x00008005;       // reversed polynomial
        /*CRC0->CTRL = CRC_CTRL_WAS_MASK | CRC_CTRL_TOT(1) | CRC_CTRL_TOTR(1);*/ CRC_CTRL = (CRC_CTRL_TOTR_BITS | CRC_CTRL_TOT_BITS | CRC_CTRL_WAS);
        /*CRC0->ACCESS16BIT.DATAL = 0xFFFF;  */ CRC_CRC_LL_SHORT_WORD_ACCESS = 0xffff; // set seed vaue
        /*CRC0->CTRL &= ~CRC_CTRL_WAS_MASK;  */CRC_CTRL = (CRC_CTRL_TOTR_BITS | CRC_CTRL_TOT_BITS);
        while (rxFrameLength-- != 0) { 
           /* CRC0->ACCESS8BIT.DATALL = buffer[i];  */ CRC_CRC_LL = *ptrFrame++;
        }  
      //buffer[*length] = CRC0->ACCESS8BIT.DATALU;  
      //buffer[*length+1] = CRC0->ACCESS8BIT.DATALL;  
      //*length = *length + 2;  
    //}  
        return CRC_CRC_LL_SHORT_WORD_ACCESS;
        #elif defined MODBUS_CRC_FROM_LOOKUP_TABLE                       // look-up table method requiring more memory but faster
    unsigned char ucCRC_Lo = 0xff;
    unsigned char ucCRC_Hi = 0xff;
    unsigned char ucIndex;
    static const unsigned char ucCRC_lookup[512] = {
        0x00,0xC1,0x81,0x40,0x01,0xC0,0x80,0x41,0x01,0xC0,0x80,0x41,0x00,0xC1,0x81,0x40, 0x01,0xC0,0x80,0x41,0x00,0xC1,0x81,0x40,0x00,0xC1,0x81,0x40,0x01,0xC0,0x80,0x41,
        0x01,0xC0,0x80,0x41,0x00,0xC1,0x81,0x40,0x00,0xC1,0x81,0x40,0x01,0xC0,0x80,0x41, 0x00,0xC1,0x81,0x40,0x01,0xC0,0x80,0x41,0x01,0xC0,0x80,0x41,0x00,0xC1,0x81,0x40,
        0x01,0xC0,0x80,0x41,0x00,0xC1,0x81,0x40,0x00,0xC1,0x81,0x40,0x01,0xC0,0x80,0x41, 0x00,0xC1,0x81,0x40,0x01,0xC0,0x80,0x41,0x01,0xC0,0x80,0x41,0x00,0xC1,0x81,0x40,
        0x00,0xC1,0x81,0x40,0x01,0xC0,0x80,0x41,0x01,0xC0,0x80,0x41,0x00,0xC1,0x81,0x40, 0x01,0xC0,0x80,0x41,0x00,0xC1,0x81,0x40,0x00,0xC1,0x81,0x40,0x01,0xC0,0x80,0x41,
        0x01,0xC0,0x80,0x41,0x00,0xC1,0x81,0x40,0x00,0xC1,0x81,0x40,0x01,0xC0,0x80,0x41, 0x00,0xC1,0x81,0x40,0x01,0xC0,0x80,0x41,0x01,0xC0,0x80,0x41,0x00,0xC1,0x81,0x40,
        0x00,0xC1,0x81,0x40,0x01,0xC0,0x80,0x41,0x01,0xC0,0x80,0x41,0x00,0xC1,0x81,0x40, 0x01,0xC0,0x80,0x41,0x00,0xC1,0x81,0x40,0x00,0xC1,0x81,0x40,0x01,0xC0,0x80,0x41,
        0x00,0xC1,0x81,0x40,0x01,0xC0,0x80,0x41,0x01,0xC0,0x80,0x41,0x00,0xC1,0x81,0x40, 0x01,0xC0,0x80,0x41,0x00,0xC1,0x81,0x40,0x00,0xC1,0x81,0x40,0x01,0xC0,0x80,0x41,
        0x01,0xC0,0x80,0x41,0x00,0xC1,0x81,0x40,0x00,0xC1,0x81,0x40,0x01,0xC0,0x80,0x41, 0x00,0xC1,0x81,0x40,0x01,0xC0,0x80,0x41,0x01,0xC0,0x80,0x41,0x00,0xC1,0x81,0x40,

        0x00,0xC0,0xC1,0x01,0xC3,0x03,0x02,0xC2,0xC6,0x06,0x07,0xC7,0x05,0xC5,0xC4,0x04, 0xCC,0x0C,0x0D,0xCD,0x0F,0xCF,0xCE,0x0E,0x0A,0xCA,0xCB,0x0B,0xC9,0x09,0x08,0xC8,
        0xD8,0x18,0x19,0xD9,0x1B,0xDB,0xDA,0x1A,0x1E,0xDE,0xDF,0x1F,0xDD,0x1D,0x1C,0xDC, 0x14,0xD4,0xD5,0x15,0xD7,0x17,0x16,0xD6,0xD2,0x12,0x13,0xD3,0x11,0xD1,0xD0,0x10,
        0xF0,0x30,0x31,0xF1,0x33,0xF3,0xF2,0x32,0x36,0xF6,0xF7,0x37,0xF5,0x35,0x34,0xF4, 0x3C,0xFC,0xFD,0x3D,0xFF,0x3F,0x3E,0xFE,0xFA,0x3A,0x3B,0xFB,0x39,0xF9,0xF8,0x38,
        0x28,0xE8,0xE9,0x29,0xEB,0x2B,0x2A,0xEA,0xEE,0x2E,0x2F,0xEF,0x2D,0xED,0xEC,0x2C, 0xE4,0x24,0x25,0xE5,0x27,0xE7,0xE6,0x26,0x22,0xE2,0xE3,0x23,0xE1,0x21,0x20,0xE0,
        0xA0,0x60,0x61,0xA1,0x63,0xA3,0xA2,0x62,0x66,0xA6,0xA7,0x67,0xA5,0x65,0x64,0xA4, 0x6C,0xAC,0xAD,0x6D,0xAF,0x6F,0x6E,0xAE,0xAA,0x6A,0x6B,0xAB,0x69,0xA9,0xA8,0x68,
        0x78,0xB8,0xB9,0x79,0xBB,0x7B,0x7A,0xBA,0xBE,0x7E,0x7F,0xBF,0x7D,0xBD,0xBC,0x7C, 0xB4,0x74,0x75,0xB5,0x77,0xB7,0xB6,0x76,0x72,0xB2,0xB3,0x73,0xB1,0x71,0x70,0xB0,
        0x50,0x90,0x91,0x51,0x93,0x53,0x52,0x92,0x96,0x56,0x57,0x97,0x55,0x95,0x94,0x54, 0x9C,0x5C,0x5D,0x9D,0x5F,0x9F,0x9E,0x5E,0x5A,0x9A,0x9B,0x5B,0x99,0x59,0x58,0x98,
        0x88,0x48,0x49,0x89,0x4B,0x8B,0x8A,0x4A,0x4E,0x8E,0x8F,0x4F,0x8D,0x4D,0x4C,0x8C, 0x44,0x84,0x85,0x45,0x87,0x47,0x46,0x86,0x82,0x42,0x43,0x83,0x41,0x81,0x80,0x40,
    };
    do {
        ucIndex = (ucCRC_Lo ^ *ptrFrame++);
        ucCRC_Lo = (ucCRC_Hi ^ ucCRC_lookup[ucIndex]);
        ucCRC_Hi = ucCRC_lookup[ucIndex + 256];
    } while (--rxFrameLength);
    return ((ucCRC_Hi << 8) | ucCRC_Lo);
        #else                                                            // shift register method
    #define MODBUS_CRC_POLY 0xa001
    int iBit;
    unsigned short usCRC = 0xffff;
    do {
        usCRC ^= *ptrFrame;
        for (iBit = 0; iBit < 8; iBit++) {
            if (usCRC & 0x0001) {
                usCRC >>= 1;
                usCRC ^= MODBUS_CRC_POLY;
            }
            else {
                usCRC >>= 1;
            }
        }
        ptrFrame++;
    } while (--rxFrameLength);
    return usCRC;
        #endif
}


// An inter-character space of greater than 3.5 characters has been detected - this is the end of a valid RTU frame
//
static void fnTimerT3_5_fired(unsigned char ucMODBUSport)
{
    fnDebugMsg("TIM-");
    fnDebugHex(ucMODBUSport, sizeof(ucMODBUSport));
    fnDebugMsg("\r\n");
    if (iModbusSerialState[ucMODBUSport] != MODBUS_FRAME_NOT_OK) {
        iModbusSerialState[ucMODBUSport] = MODBUS_FRAME_OK;              // 3.5 characters of idle - this is the end of a valid frame
        rxFrameLength[ucMODBUSport] = fnMsgs(SerialHandle[ucMODBUSport]);// save the length of the frame
        fnInterruptMessage(OWN_TASK, (unsigned char)(EVENT_VALID_FRAME_RECEIVED - ucMODBUSport)); // schedule task to handle valid RTU frame on this MODBUS port
    }
    else {
        fnFlush(SerialHandle[ucMODBUSport], FLUSH_RX);
        iModbusSerialState[ucMODBUSport] = MODBUS_IDLE;                  // {V1.01}
        fnInterruptMessage(OWN_TASK, (unsigned char)(EVENT_BAD_FRAME_TERMINATED - ucMODBUSport)); // schedule task to handle statistics of bad frame terminations
    }
}

static void fnTimer_T_3_5_fired_0(void)
{
    fnTimerT3_5_fired(0);
}
        #if MODBUS_SERIAL_INTERFACES > 1
static void fnTimer_T_3_5_fired_1(void)
{
    fnTimerT3_5_fired(1);
}
        #endif
        #if MODBUS_SERIAL_INTERFACES > 2
static void fnTimer_T_3_5_fired_2(void)
{
    fnTimerT3_5_fired(2);
}
        #endif
        #if MODBUS_SERIAL_INTERFACES > 3
static void fnTimer_T_3_5_fired_3(void)
{
    fnTimerT3_5_fired(3);
}
        #endif

// An inter-character space of greater than 1.5 characters has been detected
// Now start the T3.5 timer to detect whether it is an end of frame or if it is an illegal gap
//
static void fnTimer_T_1_5_fired_0(void)
{
    iModbusSerialState[0] = MODBUS_CHARACTER_TERMINATING;
    fnConfigureInterrupt((void *)&timer_setup_3_5[0]);                   // start T3.5 timer
}
        #if MODBUS_SERIAL_INTERFACES > 1
static void fnTimer_T_1_5_fired_1(void)
{
    iModbusSerialState[1] = MODBUS_CHARACTER_TERMINATING;
    fnConfigureInterrupt((void *)&timer_setup_3_5[1]);                   // start T3.5 timer
}
        #endif
        #if MODBUS_SERIAL_INTERFACES > 2
static void fnTimer_T_1_5_fired_2(void)
{
    iModbusSerialState[2] = MODBUS_CHARACTER_TERMINATING;
    fnConfigureInterrupt((void *)&timer_setup_3_5[2]);                   // start T3.5 timer
}
        #endif
        #if MODBUS_SERIAL_INTERFACES > 3
static void fnTimer_T_1_5_fired_3(void)
{
    iModbusSerialState[3] = MODBUS_CHARACTER_TERMINATING;
    fnConfigureInterrupt((void *)&timer_setup_3_5[3]);                   // {V1.16} start T3.5 timer
}
        #endif

// The tty driver calls this routine each time that it has received a character
// The inter-character space is monitored to detect an RTU frame termination
//
extern void fnRetrigger_T1_5_monitor(QUEUE_HANDLE Channel)
{
    unsigned char ucMODBUSport = fnMapMODBUSport(Channel);
        #if !defined _WINDOWS
    if (iModbusSerialState[ucMODBUSport] == MODBUS_CHARACTER_TERMINATING) { // if a character is received after a space of T1.5 but before T3.5 it represents a corrupted reception
        iModbusSerialState[ucMODBUSport] = MODBUS_FRAME_NOT_OK;          // inter-character gap violation (between 1.5 and 3.5 characters)
        fnFlush(SerialHandle[ucMODBUSport], FLUSH_RX);
        fnInterruptMessage(OWN_TASK, (unsigned char)(EVENT_INCOMPLETE_FRAME_RECEIVED - ucMODBUSport)); // schedule task to handle statistics of invalid character gaps
        return;
    }
        #endif
        #if defined _HW_SAM7X
    if (Channel < 2) {                                                   // this device performs internal idle timing on USARTs (but not DBGU)
        return;
    }
        #elif defined _HW_AVR32                                          // this device performs internal idle timing on all channels
    return;
        #endif
    fnConfigureInterrupt((void *)&timer_setup_1_5[ucMODBUSport]);        // retrigger / start T1.5 timer
}

        #if defined _HW_SAM7X || defined _HW_AVR32                       // internal HW timer without additional timer support
// An idle period has been detected
//
extern int fnSciRxIdle(QUEUE_HANDLE Channel)
{
    unsigned char ucMODBUSport = fnMapMODBUSport(Channel);
    if (iModbusSerialState[ucMODBUSport] >= MODBUS_CHARACTER_TERMINATING) { // we are expecting the final idle delay
        fnTimerT3_5_fired(ucMODBUSport);
        return -17;                                                      // set back T1.5 delay and wait for next character
    }
    iModbusSerialState[ucMODBUSport] = MODBUS_CHARACTER_TERMINATING;     // we expect that we are terminating
    return 21;                                                           // set next delay until T3.5
}
        #endif

// Configure the inter-space timers based on the Baud rate
//
static void fnConfigureInterspaceTimes(unsigned char ucMODBUSport)
{
    #define FAST_T1_5_US_TIME ((750 * 2.5)/1.5)                          // fixed period for baud rates greater than 19'200
    #define FAST_T3_5_US_TIME (((1750 * 4.5)/3.5) - FAST_T1_5_US_TIME)

    #define T1_5_19600_US_TIME ((2.5 * 1000000 * 11)/19200)              // 2.5 RTU character period without rx interrupt means 1.5 periods of idle
    #define T3_5_19600_US_TIME ((2*1000000 * 11)/19200)                  // extra 2 character periods before end of frame is recognised

    _TIMER_INTERRUPT_SETUP *ptrSetup1_5 = &timer_setup_1_5[ucMODBUSport];
    _TIMER_INTERRUPT_SETUP *ptrSetup3_5 = &timer_setup_3_5[ucMODBUSport];
        #if defined _KINETIS                                             // use PIT timers (up to 4 possible)
    ptrSetup1_5->int_type = PIT_INTERRUPT;                               // configure the timer struct once for efficiency
    ptrSetup3_5->int_type = PIT_INTERRUPT;
    ptrSetup1_5->ucPIT = MODBUS0_PIT_TIMER_CHANNEL;                      // PIT timer channel
    ptrSetup3_5->ucPIT = MODBUS0_PIT_TIMER_CHANNEL;                      // PIT timer channel
    ptrSetup1_5->int_priority = MODBUS0_PIT_INTERRUPT_PRIORITY;          // define interrupt priority
    ptrSetup3_5->int_priority = MODBUS0_PIT_INTERRUPT_PRIORITY;          // define interrupt priority
    ptrSetup1_5->int_handler = fnTimer_T_1_5_fired_0;
    ptrSetup3_5->int_handler = fnTimer_T_3_5_fired_0;
    ptrSetup1_5->mode = PIT_SINGLE_SHOT;
    ptrSetup3_5->mode = PIT_SINGLE_SHOT;
            #if MODBUS_SERIAL_INTERFACES > 1
    if (ucMODBUSport == 1) {
        ptrSetup1_5->ucPIT = MODBUS1_PIT_TIMER_CHANNEL;                  // PIT timer channel
        ptrSetup3_5->ucPIT = MODBUS1_PIT_TIMER_CHANNEL;                  // PIT timer channel
        ptrSetup1_5->int_priority = MODBUS1_PIT_INTERRUPT_PRIORITY;      // define interrupt priority
        ptrSetup3_5->int_priority = MODBUS1_PIT_INTERRUPT_PRIORITY;      // define interrupt priority
        ptrSetup1_5->int_handler = fnTimer_T_1_5_fired_1;
        ptrSetup3_5->int_handler = fnTimer_T_3_5_fired_1;
    }
                #if MODBUS_SERIAL_INTERFACES > 2
    else if (ucMODBUSport == 2) {
        ptrSetup1_5->ucPIT = MODBUS2_PIT_TIMER_CHANNEL;                  // PIT timer channel
        ptrSetup3_5->ucPIT = MODBUS2_PIT_TIMER_CHANNEL;                  // PIT timer channel
        ptrSetup1_5->int_priority = MODBUS2_PIT_INTERRUPT_PRIORITY;      // define interrupt priority
        ptrSetup3_5->int_priority = MODBUS2_PIT_INTERRUPT_PRIORITY;      // define interrupt priority
        ptrSetup1_5->int_handler = fnTimer_T_1_5_fired_2;
        ptrSetup3_5->int_handler = fnTimer_T_3_5_fired_2;
    }
                #endif
                #if MODBUS_SERIAL_INTERFACES > 3                         // maximum four MODBUS serial interfaces on Kinetis
    else if (ucMODBUSport == 3) {
        ptrSetup1_5->ucPIT = MODBUS3_PIT_TIMER_CHANNEL;                  // PIT timer channel
        ptrSetup3_5->ucPIT = MODBUS3_PIT_TIMER_CHANNEL;                  // PIT timer channel
        ptrSetup1_5->int_priority = MODBUS3_PIT_INTERRUPT_PRIORITY;      // define interrupt priority
        ptrSetup3_5->int_priority = MODBUS3_PIT_INTERRUPT_PRIORITY;      // define interrupt priority
        ptrSetup1_5->int_handler = fnTimer_T_1_5_fired_2;
        ptrSetup3_5->int_handler = fnTimer_T_3_5_fired_2;
    }
                #endif
            #endif
    switch (ptrMODBUS_pars->ucSerialSpeed[ucMODBUSport]) {
    case SERIAL_BAUD_1200:
        ptrSetup1_5->count_delay = PIT_US_DELAY((unsigned long)((T1_5_19600_US_TIME*19200)/1200)); // set 1200 Baud time
        ptrSetup3_5->count_delay = PIT_US_DELAY((unsigned long)((T3_5_19600_US_TIME*19200)/1200));
        break;
    case SERIAL_BAUD_2400:
        ptrSetup1_5->count_delay = PIT_US_DELAY((unsigned long)((T1_5_19600_US_TIME*19200)/2400)); // set 2400 Baud time
        ptrSetup3_5->count_delay = PIT_US_DELAY(((T3_5_19600_US_TIME*19200)/2400));
        break;
    case SERIAL_BAUD_4800:
        ptrSetup1_5->count_delay = PIT_US_DELAY((unsigned long)((T1_5_19600_US_TIME*19200)/4800)); // set 4800 Baud time
        ptrSetup3_5->count_delay = PIT_US_DELAY((unsigned long)((T3_5_19600_US_TIME*19200)/4800));
        break;
    case SERIAL_BAUD_9600:
        ptrSetup1_5->count_delay = PIT_US_DELAY((unsigned long)((T1_5_19600_US_TIME*19200)/9600)); // set 9600 Baud time
        ptrSetup3_5->count_delay = PIT_US_DELAY((unsigned long)((T3_5_19600_US_TIME*19200)/9600));
        break;
    case SERIAL_BAUD_14400:
        ptrSetup1_5->count_delay = PIT_US_DELAY((unsigned long)((T1_5_19600_US_TIME*19200)/14400)); // set 14400 Baud time
        ptrSetup3_5->count_delay = PIT_US_DELAY((unsigned long)((T3_5_19600_US_TIME*19200)/14400));
        break;
    case SERIAL_BAUD_19200:
        ptrSetup1_5->count_delay = PIT_US_DELAY((unsigned long)T1_5_19600_US_TIME); // set 19200 (reference) Baud time
        ptrSetup3_5->count_delay = PIT_US_DELAY((unsigned long)T3_5_19600_US_TIME);
        break;
            #if defined FAST_MODBUS_RTU
    case SERIAL_BAUD_38400:
        ptrSetup1_5->count_delay = PIT_US_DELAY((unsigned long)((T1_5_19600_US_TIME*192)/384)); // set 38400 Baud time
        ptrSetup3_5->count_delay = PIT_US_DELAY((unsigned long)((T3_5_19600_US_TIME*192)/384));
        break;
    case SERIAL_BAUD_57600:
        ptrSetup1_5->count_delay = PIT_US_DELAY((unsigned long)((T1_5_19600_US_TIME*192)/576)); // set 57600 Baud time
        ptrSetup3_5->count_delay = PIT_US_DELAY((unsigned long)((T3_5_19600_US_TIME*192)/576));
        break;
    case SERIAL_BAUD_115200:
        ptrSetup1_5->count_delay = PIT_US_DELAY((unsigned long)((T1_5_19600_US_TIME*192)/1152)); // set 115200 Baud time
        ptrSetup3_5->count_delay = PIT_US_DELAY((unsigned long)((T3_5_19600_US_TIME*192)/1152));
        break;
            #endif
    default:                                                         // assumed faster than 19200
        ptrSetup1_5->count_delay = PIT_US_DELAY((unsigned long)FAST_T1_5_US_TIME); // serial line specifications requires fix value to be set for higher speeds
        ptrSetup3_5->count_delay = PIT_US_DELAY((unsigned long)FAST_T3_5_US_TIME);
        break;
    }
        #elif defined _M5223X                                            // M5223x specific DMA timer code
    ptrSetup1_5->int_type = DMA_TIMER_INTERRUPT;                         // configure the timer struct once for efficiency
    ptrSetup3_5->int_type = DMA_TIMER_INTERRUPT;
    ptrSetup1_5->channel = MODBUS0_DMA_TIMER_CHANNEL;                    // DMA timer channel
    ptrSetup3_5->channel = MODBUS0_DMA_TIMER_CHANNEL;                    // DMA timer channel
    ptrSetup1_5->int_priority = MODBUS0_DMA_TIMER_INTERRUPT_PRIORITY;    // define interrupt priority
    ptrSetup3_5->int_priority = MODBUS0_DMA_TIMER_INTERRUPT_PRIORITY;    // define interrupt priority
    ptrSetup1_5->int_handler = fnTimer_T_1_5_fired_0;
    ptrSetup3_5->int_handler = fnTimer_T_3_5_fired_0;
            #if MODBUS_SERIAL_INTERFACES > 1
    if (ucMODBUSport == 1) {
        ptrSetup1_5->channel = MODBUS1_DMA_TIMER_CHANNEL;                // DMA timer channel
        ptrSetup3_5->channel = MODBUS1_DMA_TIMER_CHANNEL;                // DMA timer channel
        ptrSetup1_5->int_priority = MODBUS1_DMA_TIMER_INTERRUPT_PRIORITY;// define interrupt priority
        ptrSetup3_5->int_priority = MODBUS1_DMA_TIMER_INTERRUPT_PRIORITY;// define interrupt priority
        ptrSetup1_5->int_handler = fnTimer_T_1_5_fired_1;
        ptrSetup3_5->int_handler = fnTimer_T_3_5_fired_1;
    }
                #if MODBUS_SERIAL_INTERFACES > 2                         // maximum three MODBUS serial interfaces on M5223X
    else if (ucMODBUSport == 2) {
        ptrSetup1_5->channel = MODBUS2_DMA_TIMER_CHANNEL;                // DMA timer channel
        ptrSetup3_5->channel = MODBUS2_DMA_TIMER_CHANNEL;                // DMA timer channel
        ptrSetup1_5->int_priority = MODBUS2_DMA_TIMER_INTERRUPT_PRIORITY;// define interrupt priority
        ptrSetup3_5->int_priority = MODBUS2_DMA_TIMER_INTERRUPT_PRIORITY;// define interrupt priority
        ptrSetup1_5->int_handler = fnTimer_T_1_5_fired_2;
        ptrSetup3_5->int_handler = fnTimer_T_3_5_fired_2;
    }
                #endif
            #endif
    ptrSetup1_5->mode = (DMA_TIMER_INTERNAL_CLOCK | DMA_TIMER_SINGLE_SHOT_INTERRUPT);
    ptrSetup3_5->mode = (DMA_TIMER_INTERNAL_CLOCK | DMA_TIMER_SINGLE_SHOT_INTERRUPT);
    switch (ptrMODBUS_pars->ucSerialSpeed[ucMODBUSport]) {
    case SERIAL_BAUD_1200:
        ptrSetup3_5->mode |= DMA_TIMER_ENABLE_CLOCK_DIVIDER;
        ptrSetup3_5->clock_divider = (2 - 1);                            // use divide by 2 to achieve the delay interval
        ptrSetup1_5->count_delay = DMA_TIMER_US_DELAY(1,1,(unsigned long)((T1_5_19600_US_TIME*19200)/1200)); // set 1200 Baud time
        ptrSetup3_5->count_delay = DMA_TIMER_US_DELAY(2,1,(unsigned long)((T3_5_19600_US_TIME*19200)/1200));
        break;
    case SERIAL_BAUD_2400:
        ptrSetup1_5->count_delay = DMA_TIMER_US_DELAY(1,1,(unsigned long)((T1_5_19600_US_TIME*19200)/2400)); // set 2400 Baud time
        ptrSetup3_5->count_delay = DMA_TIMER_US_DELAY(1,1,((T3_5_19600_US_TIME*19200)/2400));
        break;
    case SERIAL_BAUD_4800:
        ptrSetup1_5->count_delay = DMA_TIMER_US_DELAY(1,1,(unsigned long)((T1_5_19600_US_TIME*19200)/4800)); // set 4800 Baud time
        ptrSetup3_5->count_delay = DMA_TIMER_US_DELAY(1,1,(unsigned long)((T3_5_19600_US_TIME*19200)/4800));
        break;
    case SERIAL_BAUD_9600:
        ptrSetup1_5->count_delay = DMA_TIMER_US_DELAY(1,1,(unsigned long)((T1_5_19600_US_TIME*19200)/9600)); // set 9600 Baud time
        ptrSetup3_5->count_delay = DMA_TIMER_US_DELAY(1,1,(unsigned long)((T3_5_19600_US_TIME*19200)/9600));
        break;
    case SERIAL_BAUD_14400:
        ptrSetup1_5->count_delay = DMA_TIMER_US_DELAY(1,1,(unsigned long)((T1_5_19600_US_TIME*19200)/14400)); // set 14400 Baud time
        ptrSetup3_5->count_delay = DMA_TIMER_US_DELAY(1,1,(unsigned long)((T3_5_19600_US_TIME*19200)/14400));
        break;
    case SERIAL_BAUD_19200:
        ptrSetup1_5->count_delay = DMA_TIMER_US_DELAY(1,1,(unsigned long)T1_5_19600_US_TIME); // set 19200 (reference) Baud time
        ptrSetup3_5->count_delay = DMA_TIMER_US_DELAY(1,1,(unsigned long)T3_5_19600_US_TIME);
        break;
            #if defined FAST_MODBUS_RTU
    case SERIAL_BAUD_38400:
        ptrSetup1_5->count_delay = DMA_TIMER_US_DELAY(1,1,(unsigned long)((T1_5_19600_US_TIME*192)/384)); // set 38400 Baud time
        ptrSetup3_5->count_delay = DMA_TIMER_US_DELAY(1,1,(unsigned long)((T3_5_19600_US_TIME*192)/384));
        break;
    case SERIAL_BAUD_57600:
        ptrSetup1_5->count_delay = DMA_TIMER_US_DELAY(1,1,(unsigned long)((T1_5_19600_US_TIME*192)/576)); // set 57600 Baud time
        ptrSetup3_5->count_delay = DMA_TIMER_US_DELAY(1,1,(unsigned long)((T3_5_19600_US_TIME*192)/576));
        break;
    case SERIAL_BAUD_115200:
        ptrSetup1_5->count_delay = DMA_TIMER_US_DELAY(1,1,(unsigned long)((T1_5_19600_US_TIME*192)/1152)); // set 115200 Baud time
        ptrSetup3_5->count_delay = DMA_TIMER_US_DELAY(1,1,(unsigned long)((T3_5_19600_US_TIME*192)/1152));
        break;
            #endif
    default:                                                             // assumed faster than 19200
        ptrSetup1_5->count_delay = DMA_TIMER_US_DELAY(1,1,(unsigned long)FAST_T1_5_US_TIME); // serial line specifications requires fix value to be set for higher speeds
        ptrSetup3_5->count_delay = DMA_TIMER_US_DELAY(1,1,(unsigned long)FAST_T3_5_US_TIME);
        break;
    }
        #else                                                            // general timer interface
    ptrSetup1_5->int_type = TIMER_INTERRUPT;                             // configure the timer struct once for efficiency
    ptrSetup3_5->int_type = TIMER_INTERRUPT;
    ptrSetup1_5->timer_reference = MODBUS0_TIMER_CHANNEL;                // timer channel
    ptrSetup3_5->timer_reference = MODBUS0_TIMER_CHANNEL;                // timer channel
    ptrSetup1_5->int_priority = PRIORITY_HW_TIMER;
    ptrSetup3_5->int_priority = PRIORITY_HW_TIMER;
    ptrSetup1_5->int_handler = fnTimer_T_1_5_fired_0;
    ptrSetup3_5->int_handler = fnTimer_T_3_5_fired_0;
            #if defined _LM3SXXXX && defined MODBUS_TIMER_16BIT          // {V1.16}
    ptrSetup1_5->timer_mode = MODBUS0_TIMER_MODE;
    ptrSetup3_5->timer_mode = MODBUS0_TIMER_MODE;
            #elif defined _LM3SXXXX || defined _STM32 || (defined _HW_TIMER_MODE && (defined _LPC23XX || defined _LPC17XX)) // {V1.10}{V1.20}
    ptrSetup1_5->timer_mode = (TIMER_SINGLE_SHOT | TIMER_US_VALUE);
    ptrSetup3_5->timer_mode = (TIMER_SINGLE_SHOT | TIMER_US_VALUE);
            #else
    ptrSetup1_5->timer_value = 0;
    ptrSetup3_5->timer_value = 0;
            #endif
            #if MODBUS_SERIAL_INTERFACES > 1
    if (ucMODBUSport == 1) {
                #if defined _LM3SXXXX && defined MODBUS_TIMER_16BIT      // {V1.16}
        ptrSetup1_5->timer_mode = MODBUS1_TIMER_MODE;
        ptrSetup3_5->timer_mode = MODBUS1_TIMER_MODE;
                #endif
        ptrSetup1_5->timer_reference = MODBUS1_TIMER_CHANNEL;            // timer channel
        ptrSetup3_5->timer_reference = MODBUS1_TIMER_CHANNEL;            // timer channel
        ptrSetup1_5->int_handler = fnTimer_T_1_5_fired_1;
        ptrSetup3_5->int_handler = fnTimer_T_3_5_fired_1;
    }
                #if MODBUS_SERIAL_INTERFACES > 2
    else if (ucMODBUSport == 2) {
                    #if defined _LM3SXXXX && defined MODBUS_TIMER_16BIT  // {V1.16}
        ptrSetup1_5->timer_mode = MODBUS2_TIMER_MODE;
        ptrSetup3_5->timer_mode = MODBUS2_TIMER_MODE;
                    #endif
        ptrSetup1_5->timer_reference = MODBUS2_TIMER_CHANNEL;            // timer channel
        ptrSetup3_5->timer_reference = MODBUS2_TIMER_CHANNEL;            // timer channel
        ptrSetup1_5->int_handler = fnTimer_T_1_5_fired_2;
        ptrSetup3_5->int_handler = fnTimer_T_3_5_fired_2;
    }
                #endif
                #if MODBUS_SERIAL_INTERFACES > 3
    else if (ucMODBUSport == 3) {
                    #if defined _LM3SXXXX && defined MODBUS_TIMER_16BIT  // {V1.16}
        ptrSetup1_5->timer_mode = MODBUS3_TIMER_MODE;
        ptrSetup3_5->timer_mode = MODBUS3_TIMER_MODE;
                    #endif
        ptrSetup1_5->timer_reference = MODBUS3_TIMER_CHANNEL;            // timer channel
        ptrSetup3_5->timer_reference = MODBUS3_TIMER_CHANNEL;            // timer channel
        ptrSetup1_5->int_handler = fnTimer_T_1_5_fired_3;
        ptrSetup3_5->int_handler = fnTimer_T_3_5_fired_3;
    }
                #endif
            #endif
    switch (ptrMODBUS_pars->ucSerialSpeed[ucMODBUSport]) {
            #if defined _LM3SXXXX || defined _STM32 || (defined _HW_TIMER_MODE && (defined _LPC23XX || defined _LPC17XX)) // {V1.10}
    case SERIAL_BAUD_1200:
        ptrSetup1_5->timer_value = (unsigned short)((T1_5_19600_US_TIME*19200)/1200); // set 1200 Baud time
        ptrSetup3_5->timer_value = (unsigned short)((T3_5_19600_US_TIME*19200)/1200);
        break;
    case SERIAL_BAUD_2400:
        ptrSetup1_5->timer_value = (unsigned short)((T1_5_19600_US_TIME*19200)/2400); // set 2400 Baud time
        ptrSetup3_5->timer_value = (unsigned short)((T3_5_19600_US_TIME*19200)/2400);
        break;
    case SERIAL_BAUD_4800:
        ptrSetup1_5->timer_value = (unsigned short)((T1_5_19600_US_TIME*19200)/4800); // set 4800 Baud time
        ptrSetup3_5->timer_value = (unsigned short)((T3_5_19600_US_TIME*19200)/4800);
        break;
    case SERIAL_BAUD_9600:
        ptrSetup1_5->timer_value = (unsigned short)((T1_5_19600_US_TIME*19200)/9600); // set 9600 Baud time
        ptrSetup3_5->timer_value = (unsigned short)((T3_5_19600_US_TIME*19200)/9600);
        break;
    case SERIAL_BAUD_14400:
        ptrSetup1_5->timer_value = (unsigned short)((T1_5_19600_US_TIME*19200)/14400); // set 14400 Baud time
        ptrSetup3_5->timer_value = (unsigned short)((T3_5_19600_US_TIME*19200)/14400);
        break;
    case SERIAL_BAUD_19200:
        ptrSetup1_5->timer_value = (unsigned short)T1_5_19600_US_TIME;   // set 19200 (reference) Baud time
        ptrSetup3_5->timer_value = (unsigned short)T3_5_19600_US_TIME;
        break;
                #if defined FAST_MODBUS_RTU
    case SERIAL_BAUD_38400:
        ptrSetup1_5->timer_value = (unsigned short)((T1_5_19600_US_TIME*192)/384); // set 38400 Baud time
        ptrSetup3_5->timer_value = (unsigned short)((T3_5_19600_US_TIME*192)/384);
        break;
    case SERIAL_BAUD_57600:
        ptrSetup1_5->timer_value = (unsigned short)((T1_5_19600_US_TIME*192)/576); // set 57600 Baud time
        ptrSetup3_5->timer_value = (unsigned short)((T3_5_19600_US_TIME*192)/576);
        break;
    case SERIAL_BAUD_115200:
        ptrSetup1_5->timer_value = (unsigned short)((T1_5_19600_US_TIME*192)/1152); // set 115200 Baud time
        ptrSetup3_5->timer_value = (unsigned short)((T3_5_19600_US_TIME*192)/1152);
        break;
                #endif
    default:                                                             // assumed faster than 19200
        ptrSetup1_5->timer_value = (unsigned short) FAST_T1_5_US_TIME;   // serial line specifications requires fix value to be set for higher speeds
        ptrSetup3_5->timer_value = (unsigned short)FAST_T3_5_US_TIME;
        break;
            #else
    case SERIAL_BAUD_1200:
        ptrSetup1_5->timer_value = (unsigned short)((T1_5_19600_US_TIME*19200)/1200000); // set 1200 Baud time (use ms timer)
        ptrSetup3_5->timer_value = (unsigned short)((T3_5_19600_US_TIME*19200)/1200000);
        break;
    case SERIAL_BAUD_2400:
        ptrSetup1_5->timer_value = (unsigned short)((T1_5_19600_US_TIME*19200)/2400000); // set 2400 Baud time (use ms timer)
        ptrSetup3_5->timer_value = (unsigned short)((T3_5_19600_US_TIME*19200)/2400000);
        break;
    case SERIAL_BAUD_4800:
        ptrSetup1_5->timer_us_value = (unsigned short)((T1_5_19600_US_TIME*19200)/4800); // set 4800 Baud time
        ptrSetup3_5->timer_us_value = (unsigned short)((T3_5_19600_US_TIME*19200)/4800);
        break;
    case SERIAL_BAUD_9600:
        ptrSetup1_5->timer_us_value = (unsigned short)((T1_5_19600_US_TIME*19200)/9600); // set 9600 Baud time
        ptrSetup3_5->timer_us_value = (unsigned short)((T3_5_19600_US_TIME*19200)/9600);
        break;
    case SERIAL_BAUD_14400:
        ptrSetup1_5->timer_us_value = (unsigned short)((T1_5_19600_US_TIME*19200)/14400); // set 14400 Baud time
        ptrSetup3_5->timer_us_value = (unsigned short)((T3_5_19600_US_TIME*19200)/14400);
        break;
    case SERIAL_BAUD_19200:
        ptrSetup1_5->timer_us_value = (unsigned short)T1_5_19600_US_TIME; // set 19200 (reference) Baud time
        ptrSetup3_5->timer_us_value = (unsigned short)T3_5_19600_US_TIME;
        break;
                #if defined FAST_MODBUS_RTU
    case SERIAL_BAUD_38400:
        ptrSetup1_5->timer_us_value = (unsigned short)((T1_5_19600_US_TIME*192)/384); // set 38400 Baud time
        ptrSetup3_5->timer_us_value = (unsigned short)((T3_5_19600_US_TIME*192)/384);
        break;
    case SERIAL_BAUD_57600:
        ptrSetup1_5->timer_us_value = (unsigned short)((T1_5_19600_US_TIME*192)/576); // set 57600 Baud time
        ptrSetup3_5->timer_us_value = (unsigned short)((T3_5_19600_US_TIME*192)/576);
        break;
    case SERIAL_BAUD_115200:
        ptrSetup1_5->timer_us_value = (unsigned short)((T1_5_19600_US_TIME*192)/1152); // set 115200 Baud time
        ptrSetup3_5->timer_us_value = (unsigned short)((T3_5_19600_US_TIME*192)/1152);
        break;
                #endif
    default:                                                             // assumed faster than 19200
        ptrSetup1_5->timer_us_value = (unsigned short) FAST_T1_5_US_TIME;// serial line specifications requires fix value to be set for higher speeds
        ptrSetup3_5->timer_us_value = (unsigned short)FAST_T3_5_US_TIME;
        break;
            #endif
    }
        #endif
}
    #endif                                                               // endif MODBUS_RTU

    #if defined MODBUS_RS485_RTS_SUPPORT
static void fnTimer_RTS_0(void)
{
    fnDriver(SerialHandle[0], ucNegateRTS[0], 0);                        // negate RTS line since transmission has terminated
}

        #if MODBUS_SERIAL_INTERFACES > 1
static void fnTimer_RTS_1(void)
{
    fnDriver(SerialHandle[1], ucNegateRTS[1], 0);                        // negate RTS line since transmission has terminated
}
        #endif
        #if MODBUS_SERIAL_INTERFACES > 2
static void fnTimer_RTS_2(void)
{
    fnDriver(SerialHandle[2], ucNegateRTS[2], 0);                        // negate RTS line since transmission has terminated
}
        #endif
        #if MODBUS_SERIAL_INTERFACES > 3
static void fnTimer_RTS_3(void)
{
    fnDriver(SerialHandle[3], ucNegateRTS[3], 0);                        // negate RTS line since transmission has terminated
}
        #endif

// Configure time delay from last character interrupt in a frame transmission to the RTS negation point.
// This should be optimally synchronous to the end of the last stop bit but may also be within the last stop bit.
//
static void fnConfigRTS_delay(unsigned char ucMODBUSport, TTYTABLE *InterfaceParameters)
{
    unsigned char ucSpeed;
    unsigned char ucType;
    _TIMER_INTERRUPT_SETUP *ptrSetupRTS = &timer_setup_RTS_negate[ucMODBUSport];
    ucSpeed = (ptrMODBUS_pars->ucSerialSpeed[ucMODBUSport] - SERIAL_BAUD_1200);
    if (ucSpeed >= (SERIAL_BAUD_115200 - SERIAL_BAUD_600)) {
        ucSpeed = 0;
    }
    ucType = 0;                                                          // 8 bit interrupt
    if (InterfaceParameters->Config & CHAR_7) {
        ucType++;                                                        // 7 bit interrupt
    }
        #if defined SERIAL_SUPPORT_DMA
    if (InterfaceParameters->ucDMAConfig & UART_TX_DMA) {
        ucType += 2;                                                     // DMA used on transmissions
    }
        #endif
        #if defined _M5223X                                              // M5223x specific dma timer code
    ptrSetupRTS->int_type = DMA_TIMER_INTERRUPT;                         // configure the timer struct once for efficiency
    ptrSetupRTS->channel = MODBUS0_DMA_TIMER_CHANNEL;                    // timer channel is the same as RTU timer (never required at the same time)
    ptrSetupRTS->int_priority = MODBUS0_DMA_TIMER_INTERRUPT_PRIORITY;
    ptrSetupRTS->int_handler = fnTimer_RTS_0;
    ptrSetupRTS->mode = (DMA_TIMER_INTERNAL_CLOCK | DMA_TIMER_SINGLE_SHOT_INTERRUPT);
            #if MODBUS_SERIAL_INTERFACES > 1
    if (ucMODBUSport == 1) {
        ptrSetupRTS->channel = MODBUS1_DMA_TIMER_CHANNEL;                // DMA timer channel
        ptrSetupRTS->int_priority = MODBUS1_DMA_TIMER_INTERRUPT_PRIORITY;
        ptrSetupRTS->int_handler = fnTimer_RTS_1;
    }
                #if MODBUS_SERIAL_INTERFACES > 2
    else if (ucMODBUSport == 2) {
        ptrSetupRTS->channel = MODBUS2_DMA_TIMER_CHANNEL;                // DMA timer channel
        ptrSetupRTS->int_priority = MODBUS2_DMA_TIMER_INTERRUPT_PRIORITY;
        ptrSetupRTS->int_handler = fnTimer_RTS_2;
    }
                #endif
            #endif
    ptrSetupRTS->count_delay = DMA_TIMER_US_DELAY(1, 1, usRTSTimes[ucSpeed][ucType]);
        #else                                                            // general timer interface
    ptrSetupRTS->int_type = TIMER_INTERRUPT;                             // configure the timer struct once for efficiency
    ptrSetupRTS->timer_reference = MODBUS0_TIMER_CHANNEL;                // timer channel is the same as RTU timer (never required at the same time)
    ptrSetupRTS->int_priority = PRIORITY_HW_TIMER;
    ptrSetupRTS->int_handler = fnTimer_RTS_0;
    ptrSetupRTS->timer_value = 0;
            #if MODBUS_SERIAL_INTERFACES > 1
    if (ucMODBUSport == 1) {
        ptrSetupRTS->timer_reference = MODBUS1_TIMER_CHANNEL;            // timer channel
        ptrSetupRTS->int_handler = fnTimer_RTS_1;
    }
                #if MODBUS_SERIAL_INTERFACES > 2
    else if (ucMODBUSport == 2) {
        ptrSetupRTS->timer_reference = MODBUS2_TIMER_CHANNEL;            // timer channel
        ptrSetupRTS->int_handler = fnTimer_RTS_2;
    }
                #endif
            #endif
            #if defined _LM3SXXXX && defined MODBUS_TIMER_16BIT          // {V1.16}
    switch (ucMODBUSport) {
    case 0:
        ptrSetupRTS->timer_mode = MODBUS0_TIMER_MODE;
        break;
                #if MODBUS_SERIAL_INTERFACES > 2
    case 1:
        ptrSetupRTS->timer_mode = MODBUS1_TIMER_MODE;
        break;
                #endif
                #if MODBUS_SERIAL_INTERFACES > 2
    case 2:
        ptrSetupRTS->timer_mode = MODBUS2_TIMER_MODE;
        break;
                #endif
                #if MODBUS_SERIAL_INTERFACES > 2
    case 3:
        ptrSetupRTS->timer_mode = MODBUS3_TIMER_MODE;
        break;
                #endif
    }
    ptrSetupRTS->timer_value = usRTSTimes[ucSpeed][ucType];
            #elif defined _LM3SXXXX || defined _STM32
    ptrSetupRTS->timer_mode = (TIMER_SINGLE_SHOT | TIMER_US_VALUE);
    ptrSetupRTS->timer_value = usRTSTimes[ucSpeed][ucType];
            #else
    ptrSetupRTS->timer_us_value = usRTSTimes[ucSpeed][ucType];
            #endif
        #endif
}
    #endif                                                               // end if defined MODBUS_RS485_RTS_SUPPORT

    #if defined MODBUS_RS485_SUPPORT && (defined UART_FRAME_COMPLETE || defined UART_FRAME_END_COMPLETE) // {V1.10}{V1.19}{V1.27}
// The UART is informing that it has just completed transmission of the last character in a frame.
// This is used to control RTS de-assertion, usually after a delay
//		
extern void fnUARTFrameTermination(QUEUE_HANDLE Channel)
{
        #if !defined AUTO_RS485_RTS_SUPPORT                              // {V1.19} dummy function if the device automatically controls RTS termination
    unsigned char ucMODBUSport = fnMapMODBUSport(Channel);
            #if defined _LPC23XX || defined _LPC17XX || defined KINETIS_KE || defined KINETIS_KL // {V1.27} since the signal arrives during the last stop bit it is possible to immediately negate the RTS for this device
    fnDriver(SerialHandle[ucMODBUSport], ucNegateRTS[ucMODBUSport], 0);  // negate RTS line since transmission has terminated
            #else
    fnConfigureInterrupt((void *)&timer_setup_RTS_negate[ucMODBUSport]); // start last delay to negation
            #endif
        #endif
}
    #elif !defined MODBUS_RS485_SUPPORT && defined UART_FRAME_COMPLETE   // {V1.19}
extern void fnUARTFrameTermination(QUEUE_HANDLE Channel)                 // dummy in case of chips not requiring this handler and UART_FRAME_COMPLETE nevertheless active
{
}
    #endif                                                               // endif MODBUS_RS485_RTS_SUPPORT

    #if defined MODBUS_SUPPORT_SERIAL_LINE_FUNCTIONS && defined MODBUS_SUPPORT_SERIAL_LINE_DIAGNOSTICS
static void fnDeleteEventLog(unsigned char ucMODBUSport)
{
    // TO DO - delete events from non-volatile memory (if available)
}
    #endif
#endif                                                                   // endif MODBUS_SERIAL_INTERFACES

#if defined _V1_18B1 && (defined MODBUS_GATE_WAY_QUEUE && defined USE_MODBUS_MASTER && MODBUS_SERIAL_INTERFACES > 0)
static int fnMODBUS_transmit_master_timer(MODBUS_RX_FUNCTION *modbus_rx_function, unsigned char *ptrBuf, unsigned short usLength)
{
    int iRtn;
    if ((iRtn = fnMODBUS_transmit(modbus_rx_function, ptrBuf, usLength)) >= usLength) {
        unsigned char ucModbusPort = modbus_rx_function->ucMODBUSport;
        // This is a serial MODBUS transmission from the bus master to a single slave
        //
            #if defined MULTIPLE_SW_TIMERS
        uTaskerGlobalMonoTimer(OWN_TASK, ptrMODBUS_pars->serial_master_timeout[ucModbusPort], (unsigned char)(T_TIMER_SLAVE - ucModbusPort)); // monitor the transaction
            #else
        uTaskerMonoTimer(OWN_TASK, ptrMODBUS_pars->serial_master_timeout[ucModbusPort], T_TIMER_SLAVE); // monitor the transaction
            #endif
        return 0;                                                        // OK
    }
    if (MODBUS_BROADCAST_TRANSMITTED == iRtn) {                          // the broadcast transmission was sent to a MODBUS serial port, the next can be sent either after transmission (ASCII) or after transmission plus 3.5T inter-character space (RTU)
        unsigned char ucModbusPort = modbus_rx_function->ucMODBUSport;
            #if defined MULTIPLE_SW_TIMERS
        uTaskerGlobalMonoTimer(OWN_TASK, ptrMODBUS_pars->serial_master_broadcast_timeout[ucModbusPort], (unsigned char)(T_TIMER_BROADCAST - ucModbusPort)); // give the broadcast a timeout period
            #else
        uTaskerMonoTimer(OWN_TASK, ptrMODBUS_pars->serial_master_broadcast_timeout[ucModbusPort], T_TIMER_BROADCAST); // give the broadcast a timeout period
            #endif
        return 0;
    }
    return iRtn;                                                         // TCP masters don't start a timer here, but instead when the TCP ack has been received
}
#endif

#if defined MODBUS_GATE_WAY_QUEUE && defined USE_MODBUS_MASTER && MODBUS_SERIAL_INTERFACES > 0
// A master serial transaction has terminated so check whether there are queued serial MODBUS frames for this serial port (either gatewayed from another serial port or from a TCP port).
// Extract next waiting frame and send it.
//
static void fnNextSerialQueue(MODBUS_RX_FUNCTION *modbus_rx_function)
{
    unsigned char ucMODBUSport = modbus_rx_function->ucMODBUSport;
    #if defined _V1_07B2
    if (modbus_queue[ucMODBUSport].ucOutstanding > 1)
    #else
    if (modbus_queue[ucMODBUSport].ucOutstanding != 0)
    #endif
    {
        unsigned char ucType;
        if (fnRead(modbus_queue[ucMODBUSport].queue_handle, &ucType, sizeof(ucType)) != 0) {
            QUEUE_TRANSFER length;
            unsigned char ucData[MODBUS_RX_BUFFER_SIZE + 4];
            fnRead(modbus_queue[ucMODBUSport].queue_handle, (unsigned char *)&length, sizeof(length));
    #if defined MODBUS_TCP && defined MODBUS_GATE_WAY_QUEUE
            if (ucType & (TCP_ROUTE_INFO | TCP_ROUTE_INFO_WITH_MAPPED_ADDRESS)) {
                if (ucType & TCP_ROUTE_INFO_WITH_MAPPED_ADDRESS) {
                    fnRead(modbus_queue[ucMODBUSport].queue_handle, &open_routes[ucMODBUSport].ucMappedAddress, 1);
                    length--;
                }
                fnRead(modbus_queue[ucMODBUSport].queue_handle, (unsigned char *)&open_routes[ucMODBUSport].modbus_session, sizeof(MODBUS_TCP_CTL *));
                fnRead(modbus_queue[ucMODBUSport].queue_handle, ucData, sizeof(MODBUS_TCP_HEADER) - 1);
                open_routes[ucMODBUSport].ucID[0] = ucData[0];           // save the transaction identifier
                open_routes[ucMODBUSport].ucID[1] = ucData[1];
                length -= (sizeof(MODBUS_TCP_HEADER) - 1 + sizeof(MODBUS_TCP_CTL *));
                open_routes[ucMODBUSport].Valid = 1;                     // we are expecting the response from this port
            }
    #endif
            fnRead(modbus_queue[ucMODBUSport].queue_handle, ucData, length); // extract the backed up data
    #if defined _V1_07B4_1 && defined MODBUS_GATE_WAY_ROUTING && defined MODBUS_GATE_WAY_QUEUE
            if (ucType & SERIAL_ROUTE_INFO) {                            // there is routing information contained in the data
        #if defined MODBUS_TCP
                open_routes[ucMODBUSport].modbus_session = 0;            // not TCP
        #endif
                length -= 2;                                             // remove the routing information
                open_routes[ucMODBUSport].ucDestinationMODBUSport = *(ucData + length);
                open_routes[ucMODBUSport].ucMappedAddress = *(ucData + length + 1);
                open_routes[ucMODBUSport].ucSourceMODBUSport = ucMODBUSport;
                open_routes[ucMODBUSport].Valid = 1;
            }
    #endif
    #if defined MODBUS_TCP && defined MODBUS_GATE_WAY_QUEUE              // {V1.15}
            if ((ucType & (TCP_ROUTE_INFO | TCP_ROUTE_INFO_WITH_MAPPED_ADDRESS)) == TCP_ROUTE_INFO) {
                open_routes[ucMODBUSport].ucMappedAddress = *ucData;     // when no address mapping takes place, take the unit address from the slave's response
            }
    #endif
    #if defined _V1_18B1
            fnMODBUS_transmit_master_timer(modbus_rx_function, ucData, (unsigned short)(length + 2));
    #else
            fnMODBUS_transmit(modbus_rx_function, ucData, (unsigned short)(length + 2)); // transmit the serial data
        #if defined MULTIPLE_SW_TIMERS
            uTaskerGlobalMonoTimer(OWN_TASK, ptrMODBUS_pars->serial_master_timeout[ucMODBUSport], (unsigned char)(T_TIMER_SLAVE - ucMODBUSport)); // monitor the transaction
        #else
            uTaskerMonoTimer(OWN_TASK, ptrMODBUS_pars->serial_master_timeout[ucMODBUSport], T_TIMER_SLAVE); // monitor the transaction
        #endif
    #endif
        }
        modbus_queue[ucMODBUSport].ucOutstanding--;
    }
    #if defined _V1_07B2
    else {
        modbus_queue[ucMODBUSport].ucOutstanding = 0;
    }
    #endif
}
#endif

#if defined MODBUS_DELAYED_RESPONSE

static MODBUS_DELAY_LIST *fnGetDelayEntry(int iType)
{
    int iEntry = 0;
    while (iEntry < MODBUS_DELAY_LIST_LENGTH) {
        if (modbus_delay_entries[iEntry].usValidLength == 0) {
            return &modbus_delay_entries[iEntry];                        // free entry found
        }
        iEntry++;
    }
    return 0;
}

// This is called to delete any queued application requests which haven't responded fast enough, or are being responded to now
//
static void fnResetDelayEntry(MODBUS_RX_FUNCTION *modbus_rx_function)
{
    int iEntry = 0;
    while (iEntry < MODBUS_DELAY_LIST_LENGTH) {
        if (modbus_delay_entries[iEntry].usValidLength != 0) {           // a valid entry to be checked
            if (modbus_delay_entries[iEntry].ucMODBUSport == modbus_rx_function->ucMODBUSport) {
    #if defined MODBUS_TCP
                if (modbus_delay_entries[iEntry].modbus_session == modbus_rx_function->modbus_session) {
                    modbus_delay_entries[iEntry].usValidLength = 0;      // entry deleted
                }
    #else
                modbus_delay_entries[iEntry].usValidLength = 0;          // entry deleted
    #endif
            }
            return;
        }
        iEntry++;
    }
}

// Filter exceptions from application delays and save delayed requests for later completion
//
static unsigned char fnWaitException(int iExceptionStatus, MODBUS_RX_FUNCTION *modbus_rx_function, unsigned char ucModbusExceptionCode)
{
    if (iExceptionStatus == MODBUS_APP_DELAYED_RESPONSE) {               // application wants time to prepare data
        unsigned short usDataLength;
        MODBUS_DELAY_LIST *ptrDelayEntry = fnGetDelayEntry(0);           // obtain a free delay entry
        if (ptrDelayEntry == 0) {                                        // no space for further delay to be entered
            return MODBUS_EXCEPTION_CODE_4;                              // error processing the request
        }
        usDataLength = modbus_rx_function->data_content.usUserDataLength;
        if (usDataLength > MAX_QUEUED_REQUEST_LENGTH) {                  // check that the request data length can be queued
            return MODBUS_EXCEPTION_CODE_4;                              // error processing the request
        }
        ptrDelayEntry->ucMODBUSport = modbus_rx_function->ucMODBUSport;  // the MODBUS port that the request arrived from
    #if defined MODBUS_TCP
        ptrDelayEntry->modbus_session = modbus_rx_function->modbus_session;
        if (ptrDelayEntry->modbus_session != 0) {                        // TCP source
            ptrDelayEntry->ucID[0] = *(modbus_rx_function->data_content.user_data - (sizeof(MODBUS_TCP_HEADER) + 1)); // save the transaction identifier
            ptrDelayEntry->ucID[1] = *(modbus_rx_function->data_content.user_data - sizeof(MODBUS_TCP_HEADER));
        }
    #endif
        ptrDelayEntry->ucAddress = modbus_rx_function->ucSourceAddress;
        ptrDelayEntry->ucFunction = modbus_rx_function->ucFunctionCode;  // the function request being queued
        uMemcpy(ptrDelayEntry->ucRequest, modbus_rx_function->data_content.user_data, usDataLength); // backup the request
        ptrDelayEntry->usDelayedReference = modbus_rx_function->usReference; // enter the unique message reference
        ptrDelayEntry->usValidLength = usDataLength;                     // mark that this entry is now occupied
        return 0;                                                        // no error
    }
    #if defined MODBUS_GATE_WAY_ROUTING
    else if (iExceptionStatus <= MODBUS_APP_GATEWAY_FUNCTION) {          // the user wants this received message to be passed to a gateway
        MODBUS_ROUTE routing_table;
        routing_table.ucNextRange = 0xff;
        routing_table.ucMODBUSPort = (MODBUS_APP_GATEWAY_FUNCTION - iExceptionStatus);
        #if defined MODBUS_TCP
        if (modbus_rx_function->modbus_session != 0) {                   // routing from TCP
            return (fnMODBUS_route(TCP_ROUTE_FROM_SLAVE, modbus_rx_function, &routing_table)); // {V1.07}
        }
            #if MODBUS_SERIAL_INTERFACES > 0
        else {
            fnMODBUS_route(SERIAL_ROUTE_FROM_SLAVE, modbus_rx_function, &routing_table);
        }
            #endif
        #else
        fnMODBUS_route(SERIAL_ROUTE_FROM_SLAVE, modbus_rx_function, &routing_table);
        #endif
        return 0;                                                        // no exception
    }
    #endif
    return ucModbusExceptionCode;                                        // return the original exception
}

// The application can send a delayed response by calling this function. Usually it will have not been able to answer directly
// because it had first to collect external data. The parameters have been updated and now the MODBUS response can be completed.
//
extern void fnMODBUS_delayed_response(unsigned short usDelayedReference)
{
    int iEntry = 0;
    while (iEntry < MODBUS_DELAY_LIST_LENGTH) {
        if (modbus_delay_entries[iEntry].usValidLength != 0) {           // a valid entry to be checked
            if (modbus_delay_entries[iEntry].usDelayedReference == usDelayedReference) {
                MODBUS_RX_FUNCTION modbus_rx_function;                   // build a temporary function block for subroutine use
                modbus_rx_function.ucMODBUSport = modbus_delay_entries[iEntry].ucMODBUSport;
                modbus_rx_function.ucSourceAddress = modbus_delay_entries[iEntry].ucAddress;
                modbus_rx_function.ucFunctionCode = modbus_delay_entries[iEntry].ucFunction;
    #if defined MODBUS_TCP
                if (modbus_delay_entries[iEntry].modbus_session != 0) {
                    modbus_rx_function.modbus_session = modbus_delay_entries[iEntry].modbus_session;
                    modbus_rx_function.ucSourceType = TCP_BINARY_INPUT;
                    modbus_rx_function.modbus_session->tx_modbus_frame->modbus_header.ucTransactionIdentifier[0] = modbus_delay_entries[iEntry].ucID[0]; // enter the corresponding transaction identifier in the transmissoin frame
                    modbus_rx_function.modbus_session->tx_modbus_frame->modbus_header.ucTransactionIdentifier[1] = modbus_delay_entries[iEntry].ucID[1];
                }
                else {
                    modbus_rx_function.modbus_session = 0;               // not originating from TCP path
    #endif
    #if MODBUS_SERIAL_INTERFACES > 0
                    if (iModbusSerialState[modbus_rx_function.ucMODBUSport] >= MODBUS_ASCII_HUNTING) {
                        modbus_rx_function.ucSourceType = ASCII_SERIAL_INPUT;
                    }
                    else {
                        modbus_rx_function.ucSourceType = RTU_SERIAL_INPUT;
                    }
    #endif
    #if defined MODBUS_TCP
                }
    #endif
                modbus_rx_function.data_content.usUserDataLength = modbus_delay_entries[iEntry].usValidLength;
                modbus_rx_function.data_content.user_data = modbus_delay_entries[iEntry].ucRequest;
    #if defined MODBUS_DELAYED_RESPONSE
                modbus_rx_function.ucDelayed = 1;                        // this response has been delayed and so doesn't need to be pre-posted to the application
    #endif
                fnHandleMODBUS_input(&modbus_rx_function);               // perform generic MODBUS frame handling of delayed requests
                return;
            }
        }
        iEntry++;
    }
}
#else
    #define fnWaitException(i, rx, code)    code
#endif

// Generic input frame handler - this can be called by a serial interface in RTU or ASCII mode or a MODBUS TCP client / listener
//
static int fnHandleMODBUS_input(MODBUS_RX_FUNCTION *modbus_rx_function)
{
#if defined USE_MODBUS_SLAVE && MODBUS_SHARED_INTERFACES > 0
    #define _USER_PORT    (modbus_rx_function->ucMODBUS_Slaveport)       // possibly a shared slave on the port
#else
    #define _USER_PORT    (modbus_rx_function->ucMODBUSport)             // only main port
#endif
#if defined USE_MODBUS_SLAVE && defined MODBUS_SUPPORT_SERIAL_LINE_DIAGNOSTICS && MODBUS_SHARED_SERIAL_INTERFACES > 0
    #define _DIAG_PORT    (modbus_rx_function->ucMODBUS_Diagnostics_SlavePort) // possibly a shared slave on the port
#else
    #define _DIAG_PORT    (modbus_rx_function->ucMODBUSport)             // only main port
#endif
    static unsigned short usMessageIndex = 0;                            // unique message index for each reception
    int iRtn = 0;
#if defined USE_MODBUS_SLAVE
    int iExceptionStatus;
    #if !defined NO_SLAVE_MODBUS_GET_COMM_EVENT_LOG || !defined NO_SLAVE_MODBUS_GET_COMM_EVENT_COUNTER
    int iEventToCount = 1;
    #endif
    unsigned char *ptrData;
    unsigned short rxDataLength;
    unsigned char  ucExceptionCode = 0;
    #if !defined NO_SLAVE_MODBUS_WRITE_MULTIPLE_COILS || !defined NO_SLAVE_MODBUS_WRITE_SINGLE_COIL || !defined NO_SLAVE_MODBUS_READ_DISCRETE_INPUTS || !defined NO_SLAVE_MODBUS_READ_COILS
    unsigned char  ucShift;
    #endif
    unsigned short usAddress;
    unsigned short usLength;
#endif
    fnDebugMsg(" function = ");
    fnDebugHex(modbus_rx_function->ucFunctionCode, sizeof(modbus_rx_function->ucFunctionCode));
    fnDebugMsg("\n\r");
#if defined USE_MODBUS_MASTER                                            // filter out master receptions to pass to user callback
    #if MODBUS_TCP_MASTERS > 0
    if (modbus_rx_function->ucMODBUSport >= (MODBUS_SERIAL_INTERFACES + MODBUS_TCP_SERVERS)) { // TCP master is receiving the frame
        #if defined MULTIPLE_SW_TIMERS                                   // {V1.26}
        uTaskerGlobalStopTimer(OWN_TASK, (unsigned char)(T_TIMER_SLAVE - (modbus_rx_function->ucMODBUSport - MODBUS_TCP_SERVERS))); // stop monitioring since there has been a response
        #else
        uTaskerStopTimer(OWN_TASK);
        #endif
        #if defined MODBUS_GATE_WAY_ROUTING                              // {V1.06}
        if (fnReverseSerialRoute(modbus_rx_function) != 0) {             // if the response is to be routed back to source do it here
            return 0;                                                    // routed back to source so completed
        }
        #endif
        #if defined TCP_SLAVE_ROUTER
        if (fnMasterCallback[modbus_rx_function->ucMODBUSport] != 0) {
            fnMasterCallback[modbus_rx_function->ucMODBUSport](MODBUS_SLAVE_RESPONSE, modbus_rx_function);
        }
        #else
        if (fnMasterCallback[modbus_rx_function->ucMODBUSport - MODBUS_TCP_SERVERS] != 0) {
            fnMasterCallback[modbus_rx_function->ucMODBUSport - MODBUS_TCP_SERVERS](MODBUS_SLAVE_RESPONSE, modbus_rx_function);
        }
        #endif
        return 0;
    }
    #endif
    #if MODBUS_SERIAL_INTERFACES > 0
    if ((modbus_rx_function->ucMODBUSport < MODBUS_SERIAL_INTERFACES) && (ptrMODBUS_pars->ucModbusSerialPortMode[modbus_rx_function->ucMODBUSport] & (MODBUS_SERIAL_MASTER | MODBUS_SERIAL_GATEWAY))) { // master or gateway serial port
        #if defined MODBUS_GATE_WAY_ROUTING
        if (ptrMODBUS_pars->ucModbusSerialPortMode[modbus_rx_function->ucMODBUSport] & MODBUS_SERIAL_MASTER) {
            int iRoutedToSource = 0;
            #if defined MULTIPLE_SW_TIMERS                               // {V1.26}
            uTaskerGlobalStopTimer(OWN_TASK, (unsigned char)(T_TIMER_SLAVE - modbus_rx_function->ucMODBUSport));
            #else
            uTaskerStopTimer(OWN_TASK);
            #endif
            if (fnReverseSerialRoute(modbus_rx_function) != 0) {
                iRoutedToSource = 1;                                     // routed back to source
            }
            #if defined MODBUS_GATE_WAY_QUEUE
            fnNextSerialQueue(modbus_rx_function);                       // initiate queued transmission
            #endif
            if (iRoutedToSource != 0) {
                return 0;
            }
        }
        else  {
            if ((!(ptrMODBUS_pars->ucModbusSerialPortMode[modbus_rx_function->ucMODBUSport] & MODBUS_SERIAL_SLAVE)) || ((modbus_rx_function->ucSourceAddress != ptrMODBUS_pars->ucModbus_slave_address[modbus_rx_function->ucMODBUSport])
            #if defined USE_MODBUS_SLAVE && MODBUS_SHARED_INTERFACES > 0
                && (modbus_rx_function->ucMODBUSport == modbus_rx_function->ucMODBUS_Slaveport)
            #endif
                )) {
            #if defined _V1_07B4
                int iRtn = fnMasterCallback[modbus_rx_function->ucMODBUSport](SERIAL_ROUTE_FROM_SLAVE, modbus_rx_function);
                if (iRtn <= MODBUS_APP_GATEWAY_FUNCTION) {               // the user wants this received message to be passed to a gateway
                    return (fnWaitException(iRtn, modbus_rx_function, 0)); // route
                }
                return iRtn;
            #else
                return (fnMasterCallback[modbus_rx_function->ucMODBUSport](SERIAL_ROUTE_FROM_SLAVE, modbus_rx_function));
            #endif
            }
            goto _handle_local_slave;
        }
        #else                                                            // {V1.20}
        if (ptrMODBUS_pars->ucModbusSerialPortMode[modbus_rx_function->ucMODBUSport] & MODBUS_SERIAL_MASTER) { // if master receiving
            #if defined MULTIPLE_SW_TIMERS                               // {V1.26}
            uTaskerGlobalStopTimer(OWN_TASK, (unsigned char)(T_TIMER_SLAVE - modbus_rx_function->ucMODBUSport)); // stop monitoring timer
            #else
            uTaskerStopTimer(OWN_TASK);
            #endif
            #if defined MODBUS_GATE_WAY_QUEUE
            fnNextSerialQueue(modbus_rx_function);                       // initiate queued transmission
            #endif
        }
        #endif
        if (fnMasterCallback[modbus_rx_function->ucMODBUSport] != 0) {
            fnMasterCallback[modbus_rx_function->ucMODBUSport](MODBUS_SLAVE_RESPONSE, modbus_rx_function);
        }
    #if defined MODBUS_TCP
        else {                                                           // master port with no master callback, therefore pass on to TCP slave
            if (modbus_rx_function->modbus_session != 0) {               // if there is a TCP session
                modbus_rx_function->ucSourceType = TCP_BINARY_INPUT;     // switch to TCP
                modbus_rx_function->ucMODBUSport += MODBUS_SERIAL_INTERFACES;
                fnMODBUS_transmit(modbus_rx_function, (modbus_rx_function->data_content.user_data - 5), (unsigned short)(modbus_rx_function->data_content.usUserDataLength + 4));
            }
        }
    #endif
        return 0;
    }
        #if defined MODBUS_GATE_WAY_ROUTING
_handle_local_slave:
        #endif
    #endif
#endif
    modbus_rx_function->usReference = ++usMessageIndex;                  // put the unique frame reference to the structure
#if defined MODBUS_DELAYED_RESPONSE
    fnResetDelayEntry(modbus_rx_function);                               // delete any waiting application delay on this MODBUS port since the master has timed out. This also deleted delayed message continuations
#endif
#if defined USE_MODBUS_SLAVE
    ptrData = modbus_rx_function->data_content.user_data;
    rxDataLength = modbus_rx_function->data_content.usUserDataLength;
    usAddress = *ptrData++;                                              // the majority of functions use this format to read as standard
    usAddress <<= 8;
    usAddress |= *ptrData++;
    usLength = *ptrData++;
    usLength <<= 8;
    usLength |= *ptrData;
    modbus_rx_function->usElementAddress = usAddress;
    modbus_rx_function->usElementLength = usLength;
    modbus_rx_function->ptrElementContent = 0;
    if (ptrMODBUS_table[_USER_PORT] == 0) {                              // slave has no parameters
        if (fnUserFunction[_USER_PORT] != 0) {
            if ((iExceptionStatus = fnUserFunction[_USER_PORT](USER_RECEIVING_ALL_MODBUS_DATA, modbus_rx_function)) != 0) { // allow the application to handle the reception
                ucExceptionCode = fnWaitException(iExceptionStatus, modbus_rx_function, MODBUS_EXCEPTION_CODE_4); // it is not possible to read the requested information, or it will be delayed
            }
        }
        else {
            ucExceptionCode = MODBUS_EXCEPTION_CODE_3;
        }
    }
    else {
        int iHandlingType = USER_RECEIVING_ALL_MODBUS_TYPE;
        switch (modbus_rx_function->ucFunctionCode) {                    // check for generic function code received by slave
    #if !defined NO_SLAVE_MODBUS_READ_COILS
        case MODBUS_READ_COILS:
    #endif
    #if !defined NO_SLAVE_MODBUS_READ_DISCRETE_INPUTS
        case MODBUS_READ_DISCRETE_INPUTS:
    #endif
    #if !defined NO_SLAVE_MODBUS_WRITE_SINGLE_COIL
        case MODBUS_WRITE_SINGLE_COIL:
    #endif
    #if !defined NO_SLAVE_MODBUS_WRITE_MULTIPLE_COILS
        case MODBUS_WRITE_MULTIPLE_COILS:
    #endif
    #if !defined NO_SLAVE_MODBUS_WRITE_MULTIPLE_COILS || !defined NO_SLAVE_MODBUS_WRITE_SINGLE_COIL || !defined NO_SLAVE_MODBUS_READ_DISCRETE_INPUTS || !defined NO_SLAVE_MODBUS_READ_COILS
            if ((rxDataLength != 4) && (MODBUS_WRITE_MULTIPLE_COILS != modbus_rx_function->ucFunctionCode)) { // check for valid length
                ucExceptionCode = MODBUS_EXCEPTION_CODE_2;
                break;
            }
            else {
                int iType;
                MODBUS_BITS *bits;
                if (modbus_rx_function->ucFunctionCode != MODBUS_READ_DISCRETE_INPUTS) {
                    iType = PREPARE_COILS;
                    bits = ptrMODBUS_table[_USER_PORT]->modbus_coils;
                }
                else {
                    iType = PREPARE_DISCRETE_INPUTS;
                    bits = ptrMODBUS_table[_USER_PORT]->modbus_discrete_inputs;
                }
                if (modbus_rx_function->ucFunctionCode == MODBUS_WRITE_SINGLE_COIL) {
                    if ((usLength != 0x0000) && (usLength != 0xff00)) {  // neither on nor off
                        ucExceptionCode = MODBUS_EXCEPTION_CODE_3;
                        break;
                    }
                    if ((bits == 0) || (bits->ptrBits == 0) || (usAddress < bits->address_range.usStartAddress) || (usAddress > bits->address_range.usEndAddress)) { // {V1.28}
                        if (fnUserFunction[_USER_PORT] != 0) {
                            if (bits != 0) {
                                iHandlingType = USER_RECEIVING_MISSED_RANGE; // the reason for the user call
                            }
                            if ((iExceptionStatus = fnUserFunction[_USER_PORT](iHandlingType, modbus_rx_function)) != 0) { // allow the application to handle the reception
                                ucExceptionCode = fnWaitException(iExceptionStatus, modbus_rx_function, MODBUS_EXCEPTION_CODE_2); // it is not possible to read the requested information, or it will be delayed
                            }
                        }
                        else {
                            ucExceptionCode = MODBUS_EXCEPTION_CODE_2;
                        }
                        break;
                    }
                    else {
                        MODBUS_BITS_ELEMENT *ptrBits = (bits->ptrBits + ((usAddress - bits->address_range.usStartAddress)/MODBUS_BITS_ELEMENT_WIDTH));
                        MODBUS_BITS_ELEMENT Bit = (0x01 << ((usAddress -bits->address_range.usStartAddress)%MODBUS_BITS_ELEMENT_WIDTH));
        #if defined NOTIFY_ONLY_COIL_CHANGES
                        MODBUS_BITS_ELEMENT oldState = *ptrBits;
        #endif
                        if (usLength != 0) {
                            *ptrBits |= Bit;                             // set the coil
                        }
                        else {
                            *ptrBits &= ~Bit;                            // clear the coil
                        }
        #if defined NOTIFY_ONLY_COIL_CHANGES
                        if (*ptrBits != oldState) {                      // if the coil  has been modified
                          //unsigned short usNewState = (unsigned short)((*ptrBits & Bit) != 0); {V1.29}
                            modbus_rx_function->usElementLength = (unsigned short)((*ptrBits & Bit) != 0); // pass the new coil state in the length field
                            if ((fnPostFunction[_USER_PORT] != 0) && (fnPostFunction[_USER_PORT](UPDATE_COILS, modbus_rx_function/*, usAddress, usNewState*/) != 0)) { // allow the application to react to change if necessary
                                ucExceptionCode = MODBUS_EXCEPTION_CODE_4; // it is not possible to change the requested coil
                                break;
                            }
                        }
        #else
                        modbus_rx_function->usElementLength = 1;
                        if ((fnPostFunction[_USER_PORT] != 0) && (fnPostFunction[_USER_PORT](UPDATE_COILS, modbus_rx_function) != 0)) { // allow the application to react to change if necessary
                            ucExceptionCode = MODBUS_EXCEPTION_CODE_4;   // it is not possible to change the requested coil
                            break;
                        }
        #endif
                        iRtn = fnSendMODBUS_response(modbus_rx_function, (ptrData - 3), 4, NO_SHIFT_LENGTH_MUL_8);
                        break;
                    }
                }
                else {
                    if ((usLength < MINIMUM_COILS) || (usLength > MAXIMUM_COILS)) {
                        ucExceptionCode = MODBUS_EXCEPTION_CODE_3;
                        break;
                    }
                    if (MODBUS_WRITE_MULTIPLE_COILS == modbus_rx_function->ucFunctionCode) {
                        ++ptrData;
                        if (*ptrData++ != _MODBUS_BITS_BYTE_SIZE(usLength)) {
                            ucExceptionCode = MODBUS_EXCEPTION_CODE_3;   // content length is incorrect
                            break;
                        }
                    }
                    if ((bits == 0) || (bits->ptrBits == 0) || (usAddress < bits->address_range.usStartAddress) || ((usAddress + (usLength - 1)) > bits->address_range.usEndAddress)) { // {V1.28}
                        if (fnUserFunction[_USER_PORT] != 0) {
                            if (bits != 0) {
                                iHandlingType = USER_RECEIVING_MISSED_RANGE; // the reason for the user call
                            }
                            if ((iExceptionStatus = fnUserFunction[_USER_PORT](iHandlingType, modbus_rx_function)) != 0) { // allow the application to handle the reception
                                ucExceptionCode = fnWaitException(iExceptionStatus, modbus_rx_function, MODBUS_EXCEPTION_CODE_2); // it is not possible to read the requested information, or it will be delayed
                                break;
                            }
                            else if (modbus_rx_function->ptrElementContent != 0) { // if the user has exchanged the function element allow the handling to continue
                                bits = (MODBUS_BITS *)modbus_rx_function->ptrElementContent;
                            }
                        }
                        else {
                            ucExceptionCode = MODBUS_EXCEPTION_CODE_2;
                            break;
                        }
                    }
                    if (MODBUS_WRITE_MULTIPLE_COILS == modbus_rx_function->ucFunctionCode) { // change multiple coils
                        MODBUS_BITS_ELEMENT *ptrBits = (bits->ptrBits + ((usAddress - bits->address_range.usStartAddress)/MODBUS_BITS_ELEMENT_WIDTH));
                        MODBUS_BITS_ELEMENT Bit = (0x01 << ((usAddress - bits->address_range.usStartAddress)%MODBUS_BITS_ELEMENT_WIDTH));
                        MODBUS_BITS_ELEMENT max_bit = (0x1 << (MODBUS_BITS_ELEMENT_WIDTH-1));
                        unsigned char ucChangeBit = 0x01;
                        unsigned char ucInput = *ptrData++;
                        while ((usLength--) != 0) {                      // for each coil
        #if defined NOTIFY_ONLY_COIL_CHANGES
                            MODBUS_BITS_ELEMENT oldState = *ptrBits;
        #endif
                            if ((ucInput & ucChangeBit) != 0) {
                                *ptrBits |= Bit;                         // set the coil
                            }
                            else {
                                *ptrBits &= ~Bit;                        // clear the coil
                            }
        #if defined NOTIFY_ONLY_COIL_CHANGES
                            if (*ptrBits != oldState) {                  // if the coil in question has been modified
                              //unsigned short usNewState = (unsigned short)((*ptrBits & Bit) != 0); {V1.29}
                                modbus_rx_function->usElementLength = (unsigned short)((*ptrBits & Bit) != 0); // pass the new coil state in the length field
                                if ((fnPostFunction[_USER_PORT] != 0) && (fnPostFunction[_USER_PORT](UPDATE_COILS, modbus_rx_function/*usAddress, usNewState*/) != 0)) { // allow the application to react to change if necessary
                                    ucExceptionCode = MODBUS_EXCEPTION_CODE_4; // it is not possible to change the requested coil
                                }
                            }
                            modbus_rx_function->usElementAddress++;
                            usAddress++;
        #endif
                            if (usLength == 0) {
                                break;
                            }
                            if ((ucChangeBit & 0x80) != 0) {
                                ucChangeBit = 0x01;                      // next byte to be treated
                                ucInput = *ptrData++;
                            }
                            else {
                                ucChangeBit <<= 1;
                            }
                            if ((Bit & max_bit) != 0) {
                                Bit = 0x01;
                                ptrBits++;
                            }
                            else {
                                Bit <<= 1;
                            }
                        }
        #if defined NOTIFY_ONLY_COIL_CHANGES
                        if (ucExceptionCode != 0) {
                            break;
                        }
        #else
                        if ((fnPostFunction[_USER_PORT] != 0) && (fnPostFunction[_USER_PORT](UPDATE_COILS, modbus_rx_function) != 0)) {// allow the application to react to change if necessary
                            ucExceptionCode = MODBUS_EXCEPTION_CODE_4;   // it is not possible to change the requested coil
                            break;
                        }
        #endif
                        iRtn = fnSendMODBUS_response(modbus_rx_function, (ptrData - rxDataLength), 4, NO_SHIFT_LENGTH_MUL_8);
                        break;
                    }
                    else {
        #if defined MODBUS_DELAYED_RESPONSE
                        if (modbus_rx_function->ucDelayed == 0) {
        #endif
                            if ((fnPreFunction[_USER_PORT] != 0) && ((iExceptionStatus = fnPreFunction[_USER_PORT](iType, modbus_rx_function)) != 0)) {// allow the application to prepare information if necessary
                                ucExceptionCode = fnWaitException(iExceptionStatus, modbus_rx_function, MODBUS_EXCEPTION_CODE_4); // it is not possible to read the requested information, or it will be delayed
                                break;
                            }
        #if defined MODBUS_DELAYED_RESPONSE
                        }
        #endif
                    }
                }
                usAddress -= bits->address_range.usStartAddress;         // with reference to the first user coil/discrete address
                ucShift = (usAddress % 8);                               // byte shift required
                usAddress /= 8;                                          // element address
                iRtn = fnSendMODBUS_response(modbus_rx_function, ((unsigned char *)bits->ptrBits + usAddress), usLength, (unsigned char)(ucShift | ((MODBUS_BITS_ELEMENT_WIDTH/2) & 0xf8)));
            }
            break;
    #endif
    #if !defined NO_SLAVE_MODBUS_READ_HOLDING_REGISTERS
        case MODBUS_READ_HOLDING_REGISTERS:
    #endif
    #if !defined NO_SLAVE_MODBUS_READ_INPUT_REGISTERS
        case MODBUS_READ_INPUT_REGISTERS:
    #endif
    #if !defined NO_SLAVE_MODBUS_WRITE_SINGLE_REGISTER
        case MODBUS_WRITE_SINGLE_REGISTER:
    #endif
    #if !defined NO_SLAVE_MODBUS_WRITE_MULTIPLE_REGISTERS
        case MODBUS_WRITE_MULTIPLE_REGISTERS:
    #endif
    #if !defined NO_SLAVE_MODBUS_MASK_WRITE_REGISTER
        case MODBUS_MASK_WRITE_REGISTER:
    #endif
    #if !defined NO_SLAVE_MODBUS_MASK_WRITE_REGISTER || !defined NO_SLAVE_MODBUS_WRITE_MULTIPLE_REGISTERS || !defined NO_SLAVE_MODBUS_WRITE_SINGLE_REGISTER || !defined NO_SLAVE_MODBUS_READ_INPUT_REGISTERS || !defined NO_SLAVE_MODBUS_READ_HOLDING_REGISTERS
            if ((rxDataLength != 4) && (modbus_rx_function->ucFunctionCode != MODBUS_WRITE_MULTIPLE_REGISTERS) && (modbus_rx_function->ucFunctionCode != MODBUS_MASK_WRITE_REGISTER)) { // check for valid length
                ucExceptionCode = MODBUS_EXCEPTION_CODE_2;
                break;
            }
            else {
                int iType;
                MODBUS_REGISTERS *regs;
                unsigned char ucFunctionCode = modbus_rx_function->ucFunctionCode;            // {V1.09} use original function code when entering function
                if (modbus_rx_function->ucSourceType & ADJUST_READ_WRITE_MULTIPLE_REGISTER) { // {V1.09}
                    modbus_rx_function->ucSourceType &= ~ADJUST_READ_WRITE_MULTIPLE_REGISTER; // although we treat MODBUS_READ_HOLDING_REGISTERS we must answer with MODBUS_READ_WRITE_MULTIPLE_REGISTER
                    modbus_rx_function->ucFunctionCode = MODBUS_READ_WRITE_MULTIPLE_REGISTER; // adjust function code for response
                }
                if ((ucFunctionCode == MODBUS_MASK_WRITE_REGISTER) && (rxDataLength != 6)) { // verify the correct length of the MODBUS_MASK_WRITE_REGISTER function content
                    ucExceptionCode = MODBUS_EXCEPTION_CODE_2;
                    break;
                }
                if (ucFunctionCode != MODBUS_READ_INPUT_REGISTERS) {
                    iType = PREPARE_HOLDING_REGISTERS;
                    regs = ptrMODBUS_table[_USER_PORT]->modbus_holding_regs;
                }
                else {
                    iType = PREPARE_INPUT_REGISTERS;
                    regs = ptrMODBUS_table[_USER_PORT]->modbus_input_regs;
                }
                if ((ucFunctionCode == MODBUS_WRITE_SINGLE_REGISTER) || (MODBUS_MASK_WRITE_REGISTER == ucFunctionCode)) {
                    unsigned short usAnswerLength;
                    if ((regs == 0) || (regs->ptrRegisters == 0) || (usAddress < regs->address_range.usStartAddress) || (usAddress > regs->address_range.usEndAddress)) { // {V1.28}
                        if (fnUserFunction[_USER_PORT] != 0) {
                            if (regs != 0) {
                                iHandlingType = USER_RECEIVING_MISSED_RANGE; // the reason for the user call
                            }
                            if ((iExceptionStatus = fnUserFunction[_USER_PORT](iHandlingType, modbus_rx_function)) != 0) { // allow the application to handle the reception
                                ucExceptionCode = fnWaitException(iExceptionStatus, modbus_rx_function, MODBUS_EXCEPTION_CODE_2); // it is not possible to read the requested information, or it will be delayed
                            }
                        }
                        else {
                            ucExceptionCode = MODBUS_EXCEPTION_CODE_2;
                        }
                        break;
                    }
                    if (MODBUS_MASK_WRITE_REGISTER == ucFunctionCode) {
                        unsigned short usOR_mask;
                        unsigned short usRegisterContent;
                        ptrData++;
                        usOR_mask = *ptrData++;
                        usOR_mask <<= 8;
                        usOR_mask |= *ptrData;
                        usRegisterContent = *(regs->ptrRegisters + (usAddress - regs->address_range.usStartAddress)); // original register content
                        usRegisterContent &= usLength;                   // AND with the AND mask
                        usRegisterContent |= (usOR_mask & ~usLength);    // OR with the OR mask, masked with the inverted AND mask
                        *(regs->ptrRegisters + (usAddress - regs->address_range.usStartAddress)) = usRegisterContent; // modify the register
                        usAnswerLength = 6;
                        ptrData -= 5;
                    }
                    else {
                        *(regs->ptrRegisters + (usAddress - regs->address_range.usStartAddress)) = usLength; // modify the register
                        ptrData -= 3;
                        usAnswerLength = 4;
                    }
                    modbus_rx_function->usElementLength = 1;
                    if ((fnPostFunction[_USER_PORT] != 0) && (fnPostFunction[_USER_PORT](UPDATE__HOLDING_REGISTERS, modbus_rx_function) != 0)) { // allow the application to react to change if necessary
                        ucExceptionCode = MODBUS_EXCEPTION_CODE_4;       // it is not possible to change the requested register
                        break;
                    }
                    iRtn = fnSendMODBUS_response(modbus_rx_function, ptrData, usAnswerLength, NO_SHIFT_LENGTH_MUL_8);
                    break;
                }
                else {
                    if (ucFunctionCode == MODBUS_WRITE_MULTIPLE_REGISTERS) {
                        ptrData++;
                        if ((usLength < MINIMUM_REGISTERS) || (usLength > MAXIMUM_MULTI_REGISTERS) || (*ptrData++ != (usLength * 2))) {
                            ucExceptionCode = MODBUS_EXCEPTION_CODE_3;
                            break;
                        }
                    }
                    else {
                        if ((usLength < MINIMUM_REGISTERS) || (usLength > MAXIMUM_REGISTERS)) {
                            ucExceptionCode = MODBUS_EXCEPTION_CODE_3;
                            break;
                        }
                    }
                    if ((regs == 0) || (regs->ptrRegisters == 0) || (usAddress < regs->address_range.usStartAddress) || ((usAddress + (usLength - 1)) > regs->address_range.usEndAddress)) { // {V1.28}
                        if (fnUserFunction[_USER_PORT] != 0) {
                            if (regs != 0) {
                                iHandlingType = USER_RECEIVING_MISSED_RANGE; // the reason for the user call
                            }
                            if ((iExceptionStatus = fnUserFunction[_USER_PORT](iHandlingType, modbus_rx_function)) != 0) { // allow the application to handle the reception
                                ucExceptionCode = fnWaitException(iExceptionStatus, modbus_rx_function, MODBUS_EXCEPTION_CODE_2); // it is not possible to read the requested information, or it will be delayed
                            }
                        }
                        else {
                            ucExceptionCode = MODBUS_EXCEPTION_CODE_2;
                        }
                        break;
                    }
                    if (ucFunctionCode == MODBUS_WRITE_MULTIPLE_REGISTERS) {
                        unsigned short usValue;
                        usAddress -= regs->address_range.usStartAddress;
                        while (usLength-- != 0) {
                            usValue = *ptrData++;
                            usValue <<= 8;
                            usValue |= *ptrData++;
                            *(regs->ptrRegisters + usAddress++) = usValue; // modify the register
                        }

                        if ((fnPostFunction[_USER_PORT] != 0) && (fnPostFunction[_USER_PORT](UPDATE__HOLDING_REGISTERS, modbus_rx_function) != 0)) { // allow the application to react to change if necessary
                            ucExceptionCode = MODBUS_EXCEPTION_CODE_4;   // it is not possible to change the requested register
                            break;
                        }
                        if (INTERMEDIATE_STEP_WITHOUT_RESPONSE == modbus_rx_function->ucSourceType) {
                            return 0;                                    // change made without a response
                        }
                        iRtn = fnSendMODBUS_response(modbus_rx_function, (ptrData - (modbus_rx_function->usElementLength * 2) - 5), 4, NO_SHIFT_LENGTH_MUL_8);
                        break;
                    }
                    else {
        #if defined MODBUS_DELAYED_RESPONSE
                        if (modbus_rx_function->ucDelayed == 0) {
        #endif
                            if ((fnPreFunction[_USER_PORT] != 0) && ((iExceptionStatus = fnPreFunction[_USER_PORT](iType, modbus_rx_function)) != 0)) {// allow the application to prepare information if necessary
                                ucExceptionCode = fnWaitException(iExceptionStatus, modbus_rx_function, MODBUS_EXCEPTION_CODE_4); // it is not possible to read the requested information, or it will be delayed
                                break;
                            }
        #if defined MODBUS_DELAYED_RESPONSE
                        }
        #endif
                    }
                }
                usAddress -= regs->address_range.usStartAddress;         // with reference to the first user register address
                iRtn = fnSendMODBUS_response(modbus_rx_function, (unsigned char *)(regs->ptrRegisters + usAddress), usLength, NO_SHIFT_REGISTER_VALUE);
            }
            break;
    #endif
    #if !defined NO_SLAVE_MODBUS_READ_WRITE_MULTIPLE_REGISTER
        case MODBUS_READ_WRITE_MULTIPLE_REGISTER:                        // write multiple holding registers, followed by read multiple registers
            {
                unsigned short usWriteStart;
                unsigned short usWriteQuantity;
                MODBUS_REGISTERS *regs = ptrMODBUS_table[_USER_PORT]->modbus_holding_regs;
                unsigned char ucSourceTypeBackup;
                if ((usLength < MINIMUM_REGISTERS) || (usLength > MAXIMUM_REGISTERS)) {
                    ucExceptionCode = MODBUS_EXCEPTION_CODE_3;
                    break;
                }
                ++ptrData;
                usWriteStart = *ptrData++;
                usWriteStart <<= 8;
                usWriteStart |= *ptrData++;
                usWriteQuantity = *ptrData++;
                usWriteQuantity <<= 8;
                usWriteQuantity |= *ptrData++;
                if ((usWriteQuantity < MINIMUM_REGISTERS) || (usWriteQuantity > MAXIMUM_MULTI_REGISTERS_WITH_READ) || (*ptrData != (usWriteQuantity * 2))) {
                    ucExceptionCode = MODBUS_EXCEPTION_CODE_3;
                    break;
                }
                if (((regs == 0) || (regs->ptrRegisters == 0) || (usWriteStart < regs->address_range.usStartAddress) || ((usWriteStart + (usWriteQuantity - 1)) > regs->address_range.usEndAddress))
                    || ((usAddress < regs->address_range.usStartAddress) || ((usAddress + (usLength - 1)) > regs->address_range.usEndAddress))) { // {V1.28}
                    if (fnUserFunction[_USER_PORT] != 0) {
                        if (regs != 0) {
                            iHandlingType = USER_RECEIVING_MISSED_RANGE; // the reason for the user call
                        }
                        if ((iExceptionStatus = fnUserFunction[_USER_PORT](iHandlingType, modbus_rx_function)) != 0) { // allow the application to handle the reception
                            ucExceptionCode = fnWaitException(iExceptionStatus, modbus_rx_function, MODBUS_EXCEPTION_CODE_2); // it is not possible to read the requested information, or it will be delayed
                        }
                    }
                    else {
                        ucExceptionCode = MODBUS_EXCEPTION_CODE_2;
                    }
                    break;
                }
                modbus_rx_function->ucFunctionCode = MODBUS_WRITE_MULTIPLE_REGISTERS; // modify the function
                ucSourceTypeBackup = modbus_rx_function->ucSourceType;   // backup the original source type
                modbus_rx_function->ucSourceType = INTERMEDIATE_STEP_WITHOUT_RESPONSE;
                modbus_rx_function->data_content.user_data += 4;
                modbus_rx_function->data_content.usUserDataLength -= 4;
                if (fnHandleMODBUS_input(modbus_rx_function) < 0) {      // process the write (recursive call) - we know that the parameters are OK
                    return -1;                                           // application failed to write
                }
                modbus_rx_function->ucFunctionCode = MODBUS_READ_HOLDING_REGISTERS; // modify the function
                modbus_rx_function->ucSourceType = (ucSourceTypeBackup | ADJUST_READ_WRITE_MULTIPLE_REGISTER); // return the original source type so that response is sent {V1.09}
                modbus_rx_function->data_content.user_data -= 4;
                modbus_rx_function->data_content.usUserDataLength = 4;
                if (fnHandleMODBUS_input(modbus_rx_function) < 0) {      // process the read  (recursive call), allowing a response to be returned
                    return -1;                                           // application failed to write
                }
            }
            break;
    #endif
    #if !defined NO_SLAVE_MODBUS_READ_FIFO_QUEUE
        case MODBUS_READ_FIFO_QUEUE:
            modbus_rx_function->usElementLength = MAX_FIFO_COUNT;
            if (rxDataLength != 2) {                                     // check for valid length
                ucExceptionCode = MODBUS_EXCEPTION_CODE_2;
                break;
            }
            else if (fnPreFunction[_USER_PORT] == 0) {
                ucExceptionCode = MODBUS_EXCEPTION_CODE_1;               // unsupported function code
            }
            else {
                int iFIFO_length;
                int iFIFOdata;
                unsigned short usFifoData[MAX_FIFO_COUNT];
        #if defined MODBUS_DELAYED_RESPONSE
                unsigned char ucPrepareType;
                if (modbus_rx_function->ucDelayed == 0) {
                    ucPrepareType = PREPARE_FIFO_READ;
                }
                else {
                    ucPrepareType = PREPARE_FIFO_READ_DELAYED;
                }
                iFIFO_length = fnPreFunction[_USER_PORT](ucPrepareType, modbus_rx_function);// allow the application to prepare information if necessary. It must return the length.
        #else
                iFIFO_length = fnPreFunction[_USER_PORT](PREPARE_FIFO_READ, modbus_rx_function /*, usMessageIndex*/);// {V1.12} allow the application to prepare information if necessary. It must return the length.
        #endif
                if ((iFIFO_length <= 0) || (iFIFO_length > MAX_FIFO_COUNT)) { // check for error or maximum value
                    ucExceptionCode = fnWaitException(iFIFO_length, modbus_rx_function, MODBUS_EXCEPTION_CODE_3); // value error, or it will be delayed
                    break;
                }
                modbus_rx_function->usElementLength = 0;
                while (iFIFO_length--) {
                    iFIFOdata = fnPreFunction[_USER_PORT](DO_FIFO_READ, modbus_rx_function);
                    if (iFIFOdata < 0) {                                 // if application processing error
                        fnSendMODBUS_exception(modbus_rx_function, MODBUS_EXCEPTION_CODE_4);// send exception response
                        return -1;
                    }
                    usFifoData[modbus_rx_function->usElementLength++] = (unsigned short)iFIFOdata;  // put each FIFO data value to buffer
                }
                iRtn = fnSendMODBUS_response(modbus_rx_function, (unsigned char *)usFifoData, (unsigned short)(2 * (modbus_rx_function->usElementLength + 1)), NO_SHIFT_FIFO_VALUE);
            }
            break;
    #endif
    #if defined MODBUS_SUPPORT_SERIAL_LINE_FUNCTIONS && defined SERIAL_INTERFACE
        #if defined MODBUS_SUPPORT_SERIAL_LINE_DIAGNOSTICS && !defined NO_SLAVE_MODBUS_DIAGNOSTICS
        case MODBUS_DIAGNOSTICS:                                         // serial line only
            {
                unsigned short usSubFunction;
                unsigned short usData;
                unsigned char ucTxBuffer[8];
                usSubFunction = modbus_rx_function->data_content.user_data[0];
                usSubFunction <<= 8;
                usSubFunction |= modbus_rx_function->data_content.user_data[1];
                usData = modbus_rx_function->data_content.user_data[2];
                usData <<= 8;
                usData |= modbus_rx_function->data_content.user_data[3];
                ucTxBuffer[0] = modbus_rx_function->ucSourceAddress;     // enter the address
                ucTxBuffer[1] = MODBUS_DIAGNOSTICS;                      // enter the function code
                uMemcpy(&ucTxBuffer[2], &modbus_rx_function->data_content.user_data[0], 4);
                switch (usSubFunction) {
            #if !defined NO_SLAVE_MODBUS_DIAG_SUB_RETURN_QUERY_DATA
                case MODBUS_DIAG_SUB_RETURN_QUERY_DATA:                  // echo back data content
                    iRtn = fnMODBUS_transmit(modbus_rx_function, ucTxBuffer, 8);
                    break;
            #endif
            #if !defined NO_SLAVE_MODBUS_DIAG_SUB_RESTART_COMS
                case MODBUS_DIAG_SUB_RESTART_COMS:
                    if ((modbus_rx_function->ucMODBUSport >= MODBUS_SERIAL_INTERFACES) || ((RESTART_LEAVE_LOG != usData) && (RESTART_CLEAR_LOG != usData))) {
                        ucExceptionCode = MODBUS_EXCEPTION_CODE_3;
                        break;
                    }
                    if (ucListenOnlyMode[_DIAG_PORT] == 0) {             // only answer when not in listen-only state
                        iRtn = fnMODBUS_transmit(modbus_rx_function, ucTxBuffer, 8);
                    }
                    if (RESTART_CLEAR_LOG == usData) {                   // restart and also delete event log
                        fnDeleteEventLog(_DIAG_PORT);
                    }
                    uTaskerMonoTimer( TASK_APPLICATION, (DELAY_LIMIT)(1*SEC), E_TIMER_SW_DELAYED_RESET ); // reset after short delay
                    break;
            #endif
            #if !defined NO_SLAVE_MODBUS_DIAG_SUB_RETURN_DIAG_REG
                case MODBUS_DIAG_SUB_RETURN_DIAG_REG:
                    if (modbus_rx_function->ucMODBUSport >= MODBUS_SERIAL_INTERFACES) {
                        ucExceptionCode = MODBUS_EXCEPTION_CODE_3;
                        break;
                    }
                    ucTxBuffer[4] = (unsigned char)(usDiagnosticRegister[_DIAG_PORT] << 8);
                    ucTxBuffer[5] = (unsigned char)usDiagnosticRegister[_DIAG_PORT];
                    iRtn = fnMODBUS_transmit(modbus_rx_function, ucTxBuffer, 8);
                    break;
            #endif
            #if defined MODBUS_ASCII && !defined NO_SLAVE_MODBUS_DIAG_SUB_CHANGE_ASCII_DELIM
                case MODBUS_DIAG_SUB_CHANGE_ASCII_DELIM:
                    if (modbus_rx_function->ucMODBUSport >= MODBUS_SERIAL_INTERFACES) {
                        ucExceptionCode = MODBUS_EXCEPTION_CODE_3;
                        break;
                    }
                    ucEndOfMessageDelimiter[modbus_rx_function->ucMODBUSport] = (unsigned char)(usData >> 8); // set new ASCII end of message delimiter
                    iRtn = fnMODBUS_transmit(modbus_rx_function, ucTxBuffer, 8);
                    break;
            #endif
            #if !defined NO_SLAVE_MODBUS_DIAG_SUB_FORCE_LISTEN_ONLY
                case MODBUS_DIAG_SUB_FORCE_LISTEN_ONLY:
                    if (modbus_rx_function->ucMODBUSport >= MODBUS_SERIAL_INTERFACES) {
                        ucExceptionCode = MODBUS_EXCEPTION_CODE_3;
                        break;
                    }
                    ucListenOnlyMode[_DIAG_PORT] = 1;                    // set listen only mode and don't return a response
                    break;
            #endif
            #if !defined NO_SLAVE_MODBUS_DIAG_SUB_CLEAR_DISGNOSTICS
                case MODBUS_DIAG_SUB_CLEAR_DISGNOSTICS:
                    if ((modbus_rx_function->ucMODBUSport >= MODBUS_SERIAL_INTERFACES) || (usData != 0)) {
                        ucExceptionCode = MODBUS_EXCEPTION_CODE_3;
                        break;
                    }
                    usDiagnosticRegister[_DIAG_PORT] = 0;
                    uMemset(&DiagnosticCounters[_DIAG_PORT], 0, sizeof(DIAGNOSTIC_COUNTERS)); // reset all counters
                    iRtn = fnMODBUS_transmit(modbus_rx_function, ucTxBuffer, 8);
                    break;
            #endif
            #if !defined NO_SLAVE_MODBUS_DIAG_SUB_RTN_BUS_MSG_CNT
                case MODBUS_DIAG_SUB_RTN_BUS_MSG_CNT:
                    if ((modbus_rx_function->ucMODBUSport >= MODBUS_SERIAL_INTERFACES) || (usData != 0)) {
                        ucExceptionCode = MODBUS_EXCEPTION_CODE_3;
                        break;
                    }
                    ucTxBuffer[4] = (unsigned char)(DiagnosticCounters[_DIAG_PORT].usMessageCounter << 8);
                    ucTxBuffer[5] = (unsigned char)DiagnosticCounters[_DIAG_PORT].usMessageCounter;
                    iRtn = fnMODBUS_transmit(modbus_rx_function, ucTxBuffer, 8);
                    break;
            #endif
            #if !defined NO_SLAVE_MODBUS_DIAG_SUB_RTN_BUS_COM_ERR_CNT
                case MODBUS_DIAG_SUB_RTN_BUS_COM_ERR_CNT:
                    if ((modbus_rx_function->ucMODBUSport >= MODBUS_SERIAL_INTERFACES) || (usData != 0)) {
                        ucExceptionCode = MODBUS_EXCEPTION_CODE_3;
                        break;
                    }
                    ucTxBuffer[4] = (unsigned char)(DiagnosticCounters[_DIAG_PORT].usCRC_errorCounter << 8);
                    ucTxBuffer[5] = (unsigned char)DiagnosticCounters[_DIAG_PORT].usCRC_errorCounter;
                    iRtn = fnMODBUS_transmit(modbus_rx_function, ucTxBuffer, 8);
                    break;
            #endif
            #if !defined NO_SLAVE_MODBUS_DIAG_SUB_RTN_BUS_EXC_ERR_CNT
                case MODBUS_DIAG_SUB_RTN_BUS_EXC_ERR_CNT:
                    if ((modbus_rx_function->ucMODBUSport >= MODBUS_SERIAL_INTERFACES) || (usData != 0)) {
                        ucExceptionCode = MODBUS_EXCEPTION_CODE_3;
                        break;
                    }
                    ucTxBuffer[4] = (unsigned char)(DiagnosticCounters[_DIAG_PORT].usException_errorCounter << 8);
                    ucTxBuffer[5] = (unsigned char)DiagnosticCounters[_DIAG_PORT].usException_errorCounter;
                    iRtn = fnMODBUS_transmit(modbus_rx_function, ucTxBuffer, 8);
                    break;
            #endif
            #if !defined NO_SLAVE_MODBUS_DIAG_SUB_RTN_SLAVE_MSG_CNT
                case MODBUS_DIAG_SUB_RTN_SLAVE_MSG_CNT:
                    if ((modbus_rx_function->ucMODBUSport >= MODBUS_SERIAL_INTERFACES) || (usData != 0)) {
                        ucExceptionCode = MODBUS_EXCEPTION_CODE_3;
                        break;
                    }
                    ucTxBuffer[4] = (unsigned char)(DiagnosticCounters[_DIAG_PORT].usSlaveMessageCounter << 8);
                    ucTxBuffer[5] = (unsigned char)DiagnosticCounters[_DIAG_PORT].usSlaveMessageCounter;
                    iRtn = fnMODBUS_transmit(modbus_rx_function, ucTxBuffer, 8);
                    break;
            #endif
            #if !defined NO_SLAVE_MODBUS_DIAG_SUB_RTN_SLAVE_NO_RSP_CNT
                case MODBUS_DIAG_SUB_RTN_SLAVE_NO_RSP_CNT:
                    if ((modbus_rx_function->ucMODBUSport >= MODBUS_SERIAL_INTERFACES) || (usData != 0)) {
                        ucExceptionCode = MODBUS_EXCEPTION_CODE_3;
                        break;
                    }
                    ucTxBuffer[4] = (unsigned char)(DiagnosticCounters[_DIAG_PORT].usNoResponseCounter << 8);
                    ucTxBuffer[5] = (unsigned char)DiagnosticCounters[_DIAG_PORT].usNoResponseCounter;
                    iRtn = fnMODBUS_transmit(modbus_rx_function, ucTxBuffer, 8);
                    break;
            #endif
            #if !defined NO_SLAVE_MODBUS_DIAG_SUB_RTN_SLAVE_NAK_CNT
                case MODBUS_DIAG_SUB_RTN_SLAVE_NAK_CNT:
                    if ((modbus_rx_function->ucMODBUSport >= MODBUS_SERIAL_INTERFACES) || (usData != 0)) {
                        ucExceptionCode = MODBUS_EXCEPTION_CODE_3;
                        break;
                    }
                    ucTxBuffer[4] = (unsigned char)(DiagnosticCounters[_DIAG_PORT].usNAKCounter << 8);
                    ucTxBuffer[5] = (unsigned char)DiagnosticCounters[_DIAG_PORT].usNAKCounter;
                    iRtn = fnMODBUS_transmit(modbus_rx_function, ucTxBuffer, 8);
                    break;
            #endif
            #if !defined NO_SLAVE_MODBUS_DIAG_SUB_RTN_SLAVE_BSY_CNT
                case MODBUS_DIAG_SUB_RTN_SLAVE_BSY_CNT:
                    if ((modbus_rx_function->ucMODBUSport >= MODBUS_SERIAL_INTERFACES) || (usData != 0)) {
                        ucExceptionCode = MODBUS_EXCEPTION_CODE_3;
                        break;
                    }
                    ucTxBuffer[4] = (unsigned char)(DiagnosticCounters[_DIAG_PORT].usSlaveBusyCounter << 8);
                    ucTxBuffer[5] = (unsigned char)DiagnosticCounters[_DIAG_PORT].usSlaveBusyCounter;
                    iRtn = fnMODBUS_transmit(modbus_rx_function, ucTxBuffer, 8);
                    break;
            #endif
            #if !defined NO_SLAVE_MODBUS_DIAG_SUB_RTN_BUS_CHR_ORUN_CNT
                case MODBUS_DIAG_SUB_RTN_BUS_CHR_ORUN_CNT:
                    if ((modbus_rx_function->ucMODBUSport >= MODBUS_SERIAL_INTERFACES) || (usData != 0)) {
                        ucExceptionCode = MODBUS_EXCEPTION_CODE_3;
                        break;
                    }
                    ucTxBuffer[4] = (unsigned char)(DiagnosticCounters[_DIAG_PORT].usOverrunCounter << 8);
                    ucTxBuffer[5] = (unsigned char)DiagnosticCounters[_DIAG_PORT].usOverrunCounter;
                    iRtn = fnMODBUS_transmit(modbus_rx_function, ucTxBuffer, 8);
                    break;
            #endif
            #if !defined NO_SLAVE_MODBUS_DIAG_SUB_CLEAR_ORUN_AND_FLG
                case MODBUS_DIAG_SUB_CLEAR_ORUN_AND_FLG:
                    if ((modbus_rx_function->ucMODBUSport >= MODBUS_SERIAL_INTERFACES) || (usData != 0)) {
                        ucExceptionCode = MODBUS_EXCEPTION_CODE_3;
                        break;
                    }
                    DiagnosticCounters[_DIAG_PORT].usOverrunCounter = 0;
                    iRtn = fnMODBUS_transmit(modbus_rx_function, ucTxBuffer, 8);
                    break;
            #endif
                default:
                    ucExceptionCode = MODBUS_EXCEPTION_CODE_1;
                    break;
                }
            }
            break;
        #endif
        #if !defined NO_SLAVE_MODBUS_READ_EXCEPTION_STATUS
        case MODBUS_READ_EXCEPTION_STATUS:                               // serial line only - return the 8 bit exception status
            {
                unsigned char ucExceptionStatus[ADD_AND_FUNCTION + 1 + MODBUS_CRC_LENGTH];
                if (fnPreFunction[_USER_PORT] == 0) {
                    iExceptionStatus = 0;
                }
                else {
                    iExceptionStatus = fnPreFunction[_USER_PORT](GET_EXCEPTION_STATUS, modbus_rx_function);
                }
                if (iExceptionStatus < 0) {
                    ucExceptionCode = fnWaitException(iExceptionStatus, modbus_rx_function, MODBUS_EXCEPTION_CODE_4); // it is not possible to read the requested information, or it will be delayed
                    break;
                }
                ucExceptionStatus[ADD_AND_FUNCTION] = (unsigned char)iExceptionStatus;
                fnSendMODBUS_transparent_response(modbus_rx_function, ucExceptionStatus, sizeof(ucExceptionStatus));
            }
            break;
        #endif
        #if !defined NO_SLAVE_MODBUS_REPORT_SLAVE_ID
        case MODBUS_REPORT_SLAVE_ID:                                     // serial line only - return type and current status of slave
            {
                unsigned char uctxbuf[ADD_AND_FUNCTION + SLAVE_ID_LENGTH + 2 + MODBUS_CRC_LENGTH]; // space for slave ID and run indicator
                CHAR *id = ptrMODBUS_pars->cSlaveID;
                unsigned short uslength = (ADD_AND_FUNCTION + 1);
                if (fnPreFunction[_USER_PORT] == 0) {
                    iExceptionStatus = 0;
                }
                else {
                    iExceptionStatus = fnPreFunction[_USER_PORT](GET_OPERATING_STATE, modbus_rx_function);
                }
                if ((iExceptionStatus < 0) || (((unsigned char)iExceptionStatus != 0x00) && ((unsigned char)iExceptionStatus != 0xff))) {
                    ucExceptionCode = fnWaitException(iExceptionStatus, modbus_rx_function, MODBUS_EXCEPTION_CODE_4); // it is not possible to read the requested information, or it will be delayed
                    break;
                }

                while (uslength < (ADD_AND_FUNCTION + SLAVE_ID_LENGTH + 1)) {
                    if (*id == 0) {
                        break;
                    }
                    uctxbuf[uslength++] = *(unsigned char *)id++;
                }
                uctxbuf[ADD_AND_FUNCTION] = (unsigned char)(uslength - 2);
                uctxbuf[uslength++] = (unsigned char)iExceptionStatus;
                fnSendMODBUS_transparent_response(modbus_rx_function, uctxbuf, (unsigned short)(uslength + 2));
            }
            break;
        #endif
        #if !defined NO_SLAVE_MODBUS_GET_COMM_EVENT_COUNTER
        case MODBUS_GET_COMM_EVENT_COUNTER:                              // serial line only
        #endif
        #if !defined NO_SLAVE_MODBUS_GET_COMM_EVENT_LOG
        case MODBUS_GET_COMM_EVENT_LOG:                                  // serial line only
        #endif
        #if !defined NO_SLAVE_MODBUS_GET_COMM_EVENT_LOG || !defined NO_SLAVE_MODBUS_GET_COMM_EVENT_COUNTER
            {
                unsigned short usLen;
                unsigned char ucBuf[ADD_AND_FUNCTION + 64 + 7 + MODBUS_CRC_LENGTH]; // largest message (64 events)
                if (fnPreFunction[_USER_PORT] == 0) {
                    iExceptionStatus = 0;
                }
                else {
                    if ((iExceptionStatus = fnPreFunction[_USER_PORT](GET_STATUS_WORD, modbus_rx_function)) < 0) {
                        ucExceptionCode = fnWaitException(iExceptionStatus, modbus_rx_function, MODBUS_EXCEPTION_CODE_4); // it is not possible to read the requested information, or it will be delayed
                        break;
                    }
                }
                ucBuf[ADD_AND_FUNCTION] = (unsigned char)(iExceptionStatus >> 8);
                ucBuf[ADD_AND_FUNCTION + 1] = (unsigned char)(iExceptionStatus);
                ucBuf[ADD_AND_FUNCTION + 2] = (unsigned char)(usEventCounter[_USER_PORT] >> 8);
                ucBuf[ADD_AND_FUNCTION + 3] = (unsigned char)(usEventCounter[_USER_PORT]);
                iEventToCount = 0;                                       // don't count this request as event
                if (MODBUS_GET_COMM_EVENT_LOG == modbus_rx_function->ucFunctionCode) {
                    unsigned char ucEvents;
                    ucBuf[ADD_AND_FUNCTION + 4] = ucBuf[ADD_AND_FUNCTION + 3];
                    ucBuf[ADD_AND_FUNCTION + 3] = ucBuf[ADD_AND_FUNCTION + 2];
                    ucBuf[ADD_AND_FUNCTION + 2] = ucBuf[ADD_AND_FUNCTION + 1];
                    ucBuf[ADD_AND_FUNCTION + 1] = ucBuf[ADD_AND_FUNCTION];
                    ucBuf[ADD_AND_FUNCTION + 5] = (unsigned char)(DiagnosticCounters[_DIAG_PORT].usMessageCounter >> 8);
                    ucBuf[ADD_AND_FUNCTION + 6] = (unsigned char)(DiagnosticCounters[_DIAG_PORT].usMessageCounter);
                    ucEvents = fnGetEvents(&ucBuf[ADD_AND_FUNCTION + 7]);
                    ucBuf[ADD_AND_FUNCTION] = (unsigned char)(6 + ucEvents);
                    usLen = (unsigned short)(MODBUS_CRC_LENGTH + 9 + ucEvents);
                }
                else {
                    usLen = (unsigned short)(6 + MODBUS_CRC_LENGTH);
                }
                fnSendMODBUS_transparent_response(modbus_rx_function, ucBuf, usLen);
            }
            break;
        #endif
    #endif
      //case MODBUS_READ_FILE_RECORD:                                    // not supported
      //case MODBUS_WRITE_FILE_RECORD:                                   // not supported
      //case MODBUS_ENCAPSUL_INTERFACE_TRANSPORT:                        // not supported
        default:
            if (fnUserFunction[_USER_PORT] != 0) {
                ucExceptionCode = fnUserFunction[_USER_PORT](USER_RECEIVING_MODBUS_UNDEFINED_FUNCTION, modbus_rx_function); // let the user handle the content
            }
            else {
                ucExceptionCode = MODBUS_EXCEPTION_CODE_1;               // unsupported function code
            }
            break;
        }
    }
    if (ucExceptionCode != 0) {
    #if defined MODBUS_SUPPORT_SERIAL_LINE_FUNCTIONS && defined MODBUS_SUPPORT_SERIAL_LINE_DIAGNOSTICS
        if (modbus_rx_function->ucMODBUSport < MODBUS_SERIAL_INTERFACES) { // only count on serial interfaces
            DiagnosticCounters[_DIAG_PORT].usException_errorCounter++;   // count exception errors on this serial interface
        }
    #endif
        fnSendMODBUS_exception(modbus_rx_function, ucExceptionCode);     // send exception response
        return -1;
    }
    #if !defined NO_SLAVE_MODBUS_GET_COMM_EVENT_LOG || !defined NO_SLAVE_MODBUS_GET_COMM_EVENT_COUNTER // {V1.02}
    if (iEventToCount != 0) {
        usEventCounter[_USER_PORT]++;                                    // successful event counted
    }
    #endif
#endif
    return iRtn;
}


#if MODBUS_TCP_MASTERS > 0                                               // MODBUS TCP master(s)

// MODBUS TCP master client
//
static int fnMODBUSClient(USOCKET Socket, unsigned char ucEvent, unsigned char *ucIp_Data, unsigned short usPortLen)
{
    MODBUS_TCP_CTL *modbus_session = ptrMODBUS_master_TCP[0];
    int iSessionNumber;

    for (iSessionNumber = 0; iSessionNumber < MODBUS_TCP_MASTERS; ) {
        if (modbus_session == 0) {                                       // {V1.26}
            modbus_session = ptrMODBUS_master_TCP[++iSessionNumber];     // move to next TCP master session
            continue;
        }
        if (modbus_session->OwnerTCPSocket == Socket) {                  // search for the session to handle this event
            switch (ucEvent) {
            case TCP_EVENT_CONREQ:                                       // session requested
                return APP_REJECT;                                       // signal TCP to send a RST back to the connection request

            case TCP_EVENT_ABORT:
                modbus_session->usOutstandingData = 0;                   // {V1.13}
    #if defined TCP_SLAVE_ROUTER
                fnMODBUS_event_report(MODBUS_CONNECTION_ABORTED, (unsigned char)(modbus_session->ucPort));
    #else
                fnMODBUS_event_report(MODBUS_CONNECTION_ABORTED, (unsigned char)(modbus_session->ucPort - MODBUS_TCP_SERVERS));
    #endif
                // Fall through intentional                              {V1.07}
                //
            case TCP_EVENT_CLOSED:
                modbus_session->ucState = MODBUS_STATE_FREE;
                if (ucEvent == TCP_EVENT_CLOSED) {
    #if defined TCP_SLAVE_ROUTER
                    fnMODBUS_event_report(MODBUS_CONNECTION_CLOSED, (unsigned char)(modbus_session->ucPort));
    #else
                    fnMODBUS_event_report(MODBUS_CONNECTION_CLOSED, (unsigned char)(modbus_session->ucPort - MODBUS_TCP_SERVERS));
    #endif
                }
    #if defined MODBUS_GATE_WAY_QUEUE
                if (modbus_queue[modbus_session->ucPort].ucOutstanding != 0) { // {V1.07} flush routing queue if not empty
                    fnFlush(modbus_queue[modbus_session->ucPort].queue_handle, FLUSH_RX);
                    modbus_queue[modbus_session->ucPort].ucOutstanding = 0;
                    open_routes[modbus_session->ucPort].Valid = 0;
                }
    #endif
                modbus_session->usOutstandingData = 0;                   // cancel any outstanding data in case the close was due to an error
                fnReleaseTCP_Socket(modbus_session->OwnerTCPSocket);     // release the TCP socket 
                modbus_session->OwnerTCPSocket = -1;                     // set the state so that a new socket will be obtained when necessary
                break;

            case TCP_EVENT_CONNECTED:
                modbus_session->ucState = MODBUS_STATE_ACTIVE;
    #if defined TCP_SLAVE_ROUTER
                fnMODBUS_event_report(MODBUS_CONNECTION_ESTABLISHED, (unsigned char)(modbus_session->ucPort));
    #else
                fnMODBUS_event_report(MODBUS_CONNECTION_ESTABLISHED, (unsigned char)(modbus_session->ucPort - MODBUS_TCP_SERVERS));
    #endif
                if (modbus_session->usOutstandingData != 0) {
                    return (fnSendTCP(modbus_session->OwnerTCPSocket, (unsigned char *)&modbus_session->tx_modbus_frame->tTCP_Header, modbus_session->usOutstandingData, TCP_FLAG_PUSH) > 0);
                }
                break;

            case TCP_EVENT_ACK:                                          // the last transmission was successful - we can continue sending next data
                if (modbus_session->usOutstandingData != 0) {
                    modbus_session->usOutstandingData = 0;               // data acknowledged
                    if (modbus_session->tx_modbus_frame->modbus_header.ucUnitIdentifier == BROADCAST_MODBUS_ADDRESS) {
    #if defined MULTIPLE_SW_TIMERS                                       // {V1.26}
                        uTaskerGlobalMonoTimer(OWN_TASK, ptrMODBUS_pars->tcp_master_broadcast_timeout[modbus_session->ucPort - MODBUS_SERIAL_INTERFACES - MODBUS_TCP_SERVERS], (unsigned char)(T_TIMER_BROADCAST - (modbus_session->ucPort - MODBUS_TCP_SERVERS))); // timeout on broadcast before continuing
    #else
                        uTaskerMonoTimer(OWN_TASK, ptrMODBUS_pars->tcp_master_broadcast_timeout[modbus_session->ucPort - MODBUS_SERIAL_INTERFACES - MODBUS_TCP_SERVERS], (unsigned char)(T_TIMER_BROADCAST - (modbus_session->ucPort - MODBUS_TCP_SERVERS))); // timeout on broadcast before continuing
    #endif
                    }
                    else {
    #if defined MULTIPLE_SW_TIMERS                                       // {V1.26}
                        uTaskerGlobalMonoTimer(OWN_TASK, ptrMODBUS_pars->tcp_master_timeout[modbus_session->ucPort - MODBUS_SERIAL_INTERFACES - MODBUS_TCP_SERVERS], (unsigned char)(T_TIMER_SLAVE - (modbus_session->ucPort - MODBUS_TCP_SERVERS))); // monitor the transaction
    #else
                        uTaskerMonoTimer(OWN_TASK, ptrMODBUS_pars->tcp_master_timeout[modbus_session->ucPort - MODBUS_SERIAL_INTERFACES - MODBUS_TCP_SERVERS], (unsigned char)(T_TIMER_SLAVE - (modbus_session->ucPort - MODBUS_TCP_SERVERS))); // monitor the transaction
    #endif
                    }
    #if defined MODBUS_GATE_WAY_QUEUE
                    if (modbus_queue[modbus_session->ucPort].ucOutstanding != 0) {
                        QUEUE_TRANSFER length;
                        unsigned char ucType;
                        modbus_queue[modbus_session->ucPort].ucOutstanding--;
                        if (fnRead(modbus_queue[modbus_session->ucPort].queue_handle, &ucType, sizeof(ucType)) != 0) {
                            unsigned char ucData[MODBUS_RX_BUFFER_SIZE];
                            MODBUS_RX_FUNCTION modbus_rx_function;
                            fnRead(modbus_queue[modbus_session->ucPort].queue_handle, (unsigned char *)&length, sizeof(length));
                            modbus_rx_function.ucSourceType = TCP_BINARY_INPUT;
                            modbus_rx_function.ucMODBUSport = modbus_session->ucPort;
                            modbus_rx_function.modbus_session = modbus_session;
                            fnRead(modbus_queue[modbus_session->ucPort].queue_handle, ucData, length); // extract the backed up data
                            if (fnMODBUS_transmit(&modbus_rx_function, ucData, (unsigned short)(length + 2)) > 0) { // transmit the queued frame
                                return APP_SENT_DATA;
                            }
                        }
                    }
    #endif
                }
                return APP_ACCEPT;

            case TCP_EVENT_REGENERATE:                                   // no ack was received for previous data
                if (modbus_session->ucState < MODBUS_STATE_ACTIVE) {     // ignore if the server has closed in the meantime
                    return APP_ACCEPT;
                }
    #if defined TCP_SLAVE_ROUTER
                fnMODBUS_event_report(MODBUS_TCP_REPETITION, (unsigned char)(modbus_session->ucPort));
    #else
                fnMODBUS_event_report(MODBUS_TCP_REPETITION, (unsigned char)(modbus_session->ucPort - MODBUS_TCP_SERVERS));
    #endif
                return (fnSendTCP(modbus_session->OwnerTCPSocket, (unsigned char *)&modbus_session->tx_modbus_frame->tTCP_Header, modbus_session->usOutstandingData, TCP_FLAG_PUSH) > 0);

            case TCP_EVENT_DATA:
                {
                    MODBUS_RX_FUNCTION modbus_rx_function;               // build a temporary function block for subroutine use
                    MODBUS_TCP_FRAME *ptrModbusFrame = (MODBUS_TCP_FRAME *)ucIp_Data;
                    unsigned short usLength = (ptrModbusFrame->modbus_header.ucLength[0] << 8);
                    usLength |= (ptrModbusFrame->modbus_header.ucLength[1]);
                    modbus_rx_function.ucMODBUSport = modbus_session->ucPort;
                    modbus_rx_function.ucSourceAddress = ptrModbusFrame->modbus_header.ucUnitIdentifier;
                    modbus_rx_function.ucMappedAddress = modbus_rx_function.ucSourceAddress;
                    modbus_rx_function.ucFunctionCode = ptrModbusFrame->ucData[0];
                    modbus_rx_function.ucSourceType = TCP_BINARY_INPUT;
                    modbus_rx_function.data_content.user_data = (ucIp_Data + sizeof(MODBUS_TCP_HEADER) + 1);
                    modbus_rx_function.data_content.usUserDataLength = (unsigned short)(usLength - 2);
                    modbus_rx_function.modbus_session = modbus_session;
                    fnHandleMODBUS_input(&modbus_rx_function);           // perform generic MODBUS frame handling
                    if (modbus_session->usOutstandingData != 0) {
                        return APP_SENT_DATA;
                    }
                }
                return APP_ACCEPT;

            case TCP_EVENT_CLOSE:
                break;
            }
            return APP_ACCEPT;
        }
        modbus_session = ptrMODBUS_master_TCP[++iSessionNumber];         // move to next TCP master session
    }
    return APP_REJECT;                                                   // unexpected occurance - signal rejection...
}

// Transmit data to a TCP slave server. If there is no TCP connection first establish this.
//
static int fnTCP_Master_Send(MODBUS_RX_FUNCTION *modbus_rx_function)
{
    unsigned char ucServer = modbus_rx_function->ucMODBUSport - (MODBUS_SERIAL_INTERFACES + MODBUS_TCP_SERVERS);
    if (modbus_rx_function->modbus_session->OwnerTCPSocket < 0) {        // we have no socket - or called before initialisation complete
        modbus_rx_function->modbus_session->OwnerTCPSocket = fnGetTCP_Socket(TOS_MINIMISE_DELAY, ptrMODBUS_pars->usIdleTimeoutMaster[ucServer], fnMODBUSClient);
        if (fnTCP_Connect(modbus_rx_function->modbus_session->OwnerTCPSocket, ptrMODBUS_pars->ucSlaveIP[ucServer], ptrMODBUS_pars->usSlavePort[ucServer], 0, 0) < 0) {
            return NO_MODBUS_SOCKET_TO_SEND;
        }
        else {
            return MODBUS_TCP_ESTABLISHING_CONNECTION;
        }
    }
    else if (modbus_rx_function->modbus_session->usOutstandingData != 0) {
        return (fnSendTCP(modbus_rx_function->modbus_session->OwnerTCPSocket, (unsigned char *)&modbus_rx_function->modbus_session->tx_modbus_frame->tTCP_Header, modbus_rx_function->modbus_session->usOutstandingData, TCP_FLAG_PUSH) > 0);
    }
    return 0;
}
#endif




#if defined MODBUS_GATE_WAY_QUEUE
static int fnQueueMODBUS_Message(unsigned char ucMODBUSport, unsigned char *ptrData, unsigned short usDataLength, unsigned char ucType)
{
    #if defined USE_MODBUS_MASTER && MODBUS_SERIAL_INTERFACES > 0
    if ((ucMODBUSport < MODBUS_SERIAL_INTERFACES) && (!(ptrMODBUS_pars->ucModbusSerialPortMode[ucMODBUSport] & MODBUS_SERIAL_MASTER))) {
        return 0;                                                        // serial port with no master functionality can always send
    }
    #endif
    #if (defined MODBUS_SERIAL_INTERFACES && defined USE_MODBUS_MASTER) || defined MODBUS_TCP
    if (modbus_queue[ucMODBUSport].queue_handle == 0) {
        return 0;                                                        // no queue defined so always send
    }
    if (modbus_queue[ucMODBUSport].ucOutstanding == 0) {
        modbus_queue[ucMODBUSport].ucOutstanding = 1;                    // mark that the master is expecting a slave response
        return 0;                                                        // OK to send
    }
    else {
        usDataLength -= 2;                                               // remove CRC
        if (fnWrite(modbus_queue[ucMODBUSport].queue_handle, 0, (QUEUE_TRANSFER)(usDataLength + sizeof(usDataLength) + sizeof(ucType))) != 0) {
            fnWrite(modbus_queue[ucMODBUSport].queue_handle, &ucType, sizeof(ucType));
            fnWrite(modbus_queue[ucMODBUSport].queue_handle, (unsigned char *)&usDataLength, sizeof(usDataLength));
            fnWrite(modbus_queue[ucMODBUSport].queue_handle, ptrData, usDataLength);
            modbus_queue[ucMODBUSport].ucOutstanding++;                  // another message waiting
            return MODBUS_TX_MESSAGE_QUEUED;                             // transmission not presently possible, queued
        }
        else {
            return MODBUS_TX_MESSAGE_LOST_NO_QUEUE_SPACE;                // no more space to queue
        }
    }
    #endif
    return 0;
}
#endif


// This routine is used to transmit data (beginning with address and function). It converts its format if required (ASCII) and adds and needed start, termination and check sum information as appropriate.
// Serial data is transmitted over the serial interface and TCP data is transmitted over the TCP interface.
//
extern int fnMODBUS_transmit(MODBUS_RX_FUNCTION *modbus_rx_function, unsigned char *ptrData, unsigned short usDataLength)
{
#if MODBUS_SERIAL_INTERFACES > 0 && defined MODBUS_RTU
    if (modbus_rx_function->ucSourceType == RTU_SERIAL_INPUT) {
        QUEUE_TRANSFER transmitted;
        unsigned short usCRC;
    #if defined MODBUS_RS485_SUPPORT                                     // {V1.10}
        if (ptrMODBUS_pars->ucModbusSerialPortMode[modbus_rx_function->ucMODBUSport] & (MODBUS_RS485_NEGATIVE | MODBUS_RS485_POSITIVE)) {
        #if defined _HW_SAM7X
            if (fnGetUARTChannel(modbus_rx_function->ucMODBUSport) >= 2) { // only control RTS line manually when DBGU UART is used, with no automatic control
                fnDriver(SerialHandle[modbus_rx_function->ucMODBUSport], ucAssertRTS[modbus_rx_function->ucMODBUSport], 0); // assert RTS line ready for transmission
            }
        #elif defined _HW_AVR32
            if (fnGetUARTChannel(modbus_rx_function->ucMODBUSport) != 1) { // only control RTS line manually USART1 is not used (which has automatic control)
                fnDriver(SerialHandle[modbus_rx_function->ucMODBUSport], ucAssertRTS[modbus_rx_function->ucMODBUSport], 0); // assert RTS line ready for transmission
            }
        #elif !defined _KINETIS                                          // {V1.21}
            #if NUMBER_EXTERNAL_SERIAL > 0                               // {V1.16}
            if (fnGetUARTChannel(modbus_rx_function->ucMODBUSport) < NUMBER_SERIAL) {              // not required when using external UART (assumes that this supports automatic control)
                fnDriver(SerialHandle[modbus_rx_function->ucMODBUSport], ucAssertRTS[modbus_rx_function->ucMODBUSport], 0); // assert RTS line ready for transmission
            }
            #else
            fnDriver(SerialHandle[modbus_rx_function->ucMODBUSport], ucAssertRTS[modbus_rx_function->ucMODBUSport], 0); // assert RTS line ready for transmission
            #endif
        #endif
        }
    #endif
        usCRC = fnCRCRTUFrame(ptrData, (unsigned short)(usDataLength - 2));// calculate the CRC of the frame to be transmitted
        ptrData[usDataLength - 2] = (unsigned char)usCRC;
        ptrData[usDataLength - 1] = (unsigned char)(usCRC >> 8);
        transmitted = fnWrite(SerialHandle[modbus_rx_function->ucMODBUSport], ptrData, (QUEUE_TRANSFER)usDataLength); // transmit
        if (*ptrData == BROADCAST_MODBUS_ADDRESS) {
            return MODBUS_BROADCAST_TRANSMITTED;                         // broadcasts require different timeouts
        }
        return transmitted;
    }
#endif
#if MODBUS_SERIAL_INTERFACES > 0 && defined MODBUS_ASCII
    if (modbus_rx_function->ucSourceType == ASCII_SERIAL_INPUT) {        // in ASCII mode the binary content has to be converted to ASCII - also a start, LRC and terminator needs to be added
        int iAsciiLength = 1;
        QUEUE_TRANSFER transmitted = 0;
        unsigned char ucTxBuf[10];                                       // short buffer to allow blocks of data to be sent - avoiding any possibility of inter-character spaces
        unsigned char ucLRC = 0;
        unsigned char ucSlaveAddress;
    #if defined MODBUS_SUPPORT_SERIAL_LINE_FUNCTIONS && defined MODBUS_SUPPORT_SERIAL_LINE_DIAGNOSTICS
        unsigned char ucNewDelimiter = END_OF_MESSAGE_DELIMITER(modbus_rx_function->ucMODBUSport);
    #endif
    #if defined MODBUS_RS485_SUPPORT                                     // {V1.10}
        if (ptrMODBUS_pars->ucModbusSerialPortMode[modbus_rx_function->ucMODBUSport] & (MODBUS_RS485_NEGATIVE | MODBUS_RS485_POSITIVE)) {
        #if defined _HW_SAM7X
            if (fnGetUARTChannel(modbus_rx_function->ucMODBUSport) == 2) { // only control RTS line manually when DBGU UART is used, with no automatic control
                fnDriver(SerialHandle[modbus_rx_function->ucMODBUSport], ucAssertRTS[modbus_rx_function->ucMODBUSport], 0); // assert RTS line ready for transmission
            }
        #elif defined _HW_AVR32
            if (fnGetUARTChannel(modbus_rx_function->ucMODBUSport) != 1) { // only control RTS line manually USART1 is not used (which has automatic control)
                fnDriver(SerialHandle[modbus_rx_function->ucMODBUSport], ucAssertRTS[modbus_rx_function->ucMODBUSport], 0); // assert RTS line ready for transmission
            }
        #elif !defined _KINETIS || (defined KINETIS_KE || defined KINETIS_KL) // {V1.21}{V1.27}
            #if NUMBER_EXTERNAL_SERIAL > 0                               // {V1.16}
            if (fnGetUARTChannel(modbus_rx_function->ucMODBUSport) < NUMBER_SERIAL) { // not required when using external UART (assumes that this supports automatic control)
                fnDriver(SerialHandle[modbus_rx_function->ucMODBUSport], ucAssertRTS[modbus_rx_function->ucMODBUSport], 0); // assert RTS line ready for transmission
            }
            #else
            fnDriver(SerialHandle[modbus_rx_function->ucMODBUSport], ucAssertRTS[modbus_rx_function->ucMODBUSport], 0); // assert RTS line ready for transmission
            #endif
        #endif
        }
    #endif
        ucTxBuf[0] = MODBUS_START_BYTE;                                  // set the start byte
        ucSlaveAddress = *ptrData;
    #if defined MODBUS_SUPPORT_SERIAL_LINE_FUNCTIONS && defined MODBUS_SUPPORT_SERIAL_LINE_DIAGNOSTICS
        if (*(ptrData + 1) == MODBUS_DIAGNOSTICS) {
            if (*(ptrData + 3) == MODBUS_DIAG_SUB_CHANGE_ASCII_DELIM) {  // ASCII delimiter is being changed for subsequent messages
                ucNewDelimiter = *(ptrData + 4);                         // prepare the new delimiter for use after sending the request
            }
        }
    #endif
        while (usDataLength-- > 2) {
            ucLRC += *ptrData;
            fnConvertToASCII(*ptrData++, &ucTxBuf[iAsciiLength]);        // add ASCII  data content
            iAsciiLength += 2;
            if (iAsciiLength >= (sizeof(ucTxBuf) - 1)) {
                transmitted += fnWrite(SerialHandle[modbus_rx_function->ucMODBUSport], ucTxBuf, (QUEUE_TRANSFER)iAsciiLength); // start a block of transmission
                iAsciiLength = 0;
            }
        }
        if (iAsciiLength >= (sizeof(ucTxBuf) - 3)) {
            transmitted += fnWrite(SerialHandle[modbus_rx_function->ucMODBUSport], ucTxBuf, (QUEUE_TRANSFER)iAsciiLength); // start a block of transmission
            iAsciiLength = 0;
        }
        ucLRC = ~ucLRC;
        fnConvertToASCII((unsigned char)(ucLRC + 1), &ucTxBuf[iAsciiLength]); // add LRC
        iAsciiLength += 2;
        ucTxBuf[iAsciiLength++] = MODBUS_CARRIAGE_RETURN;
        ucTxBuf[iAsciiLength++] = END_OF_MESSAGE_DELIMITER(modbus_rx_function->ucMODBUSport); // add terminator
        transmitted += fnWrite(SerialHandle[modbus_rx_function->ucMODBUSport], ucTxBuf, (QUEUE_TRANSFER)iAsciiLength); // complete transmission
    #if defined MODBUS_SUPPORT_SERIAL_LINE_FUNCTIONS && defined MODBUS_SUPPORT_SERIAL_LINE_DIAGNOSTICS
        END_OF_MESSAGE_DELIMITER(modbus_rx_function->ucMODBUSport) = ucNewDelimiter;
    #endif
        if (ucSlaveAddress == BROADCAST_MODBUS_ADDRESS) {
            return MODBUS_BROADCAST_TRANSMITTED;                         // broadcasts require different timeouts
        }
        return transmitted;
    }
#endif
#if defined MODBUS_TCP                                                   // TCP - no CRC and no address in the data
    if (modbus_rx_function->modbus_session->usOutstandingData != 0) {    // previous data has not yet been acknowledged
        return MODBUS_TX_MESSAGE_LOST_QUEUE_REQUIRED;
    }
    uMemcpy(modbus_rx_function->modbus_session->tx_modbus_frame->ucData, (ptrData + 1), (usDataLength - 3)); // copy, skipping the address
    usDataLength -= 2;
    modbus_rx_function->modbus_session->tx_modbus_frame->modbus_header.ucLength[0] = (unsigned char)(usDataLength >> 8);
    modbus_rx_function->modbus_session->tx_modbus_frame->modbus_header.ucLength[1] = (unsigned char)(usDataLength);
    modbus_rx_function->modbus_session->usOutstandingData = (usDataLength + (sizeof(MODBUS_TCP_HEADER) - 1)); // mark that this amount of data should be sent
    modbus_rx_function->modbus_session->tx_modbus_frame->modbus_header.ucUnitIdentifier = *ptrData; // add the slave address
    #if defined MODBUS_TCP_SERVERS
    if (modbus_rx_function->ucMODBUSport >= (MODBUS_TCP_SERVERS + MODBUS_SERIAL_INTERFACES)) { // MODBUS TCP master transmitting
        #if MODBUS_TCP_MASTERS
        unsigned char ucTransactionIdentifier = modbus_rx_function->modbus_session->tx_modbus_frame->modbus_header.ucTransactionIdentifier[1];
        ucTransactionIdentifier++;
        modbus_rx_function->modbus_session->tx_modbus_frame->modbus_header.ucTransactionIdentifier[1] = ucTransactionIdentifier;
        if (ucTransactionIdentifier == 0) {
            modbus_rx_function->modbus_session->tx_modbus_frame->modbus_header.ucTransactionIdentifier[0]++;
        }
        return (fnTCP_Master_Send(modbus_rx_function));
        #endif
    }
    else if (modbus_rx_function->ucMODBUSport >= MODBUS_SERIAL_INTERFACES) {
        if (modbus_rx_function->modbus_session->usOutstandingData != 0) {
            return (fnSendTCP(modbus_rx_function->modbus_session->OwnerTCPSocket, (unsigned char *)&modbus_rx_function->modbus_session->tx_modbus_frame->tTCP_Header, modbus_rx_function->modbus_session->usOutstandingData, TCP_FLAG_PUSH) > 0);
        }
    }
    #endif
    return (usDataLength);
#else
    return 0;
#endif
}

#if defined USE_MODBUS_SLAVE
// This routine use used to construct a MODBUS response frame with address and function entries. It automatically handles bit based data (eg. coils) and short word data (eg. registers)
// After frame construction it is passed to the transmit routine.
//
static int fnSendMODBUS_response(MODBUS_RX_FUNCTION *modbus_rx_function, void *ptrData, unsigned short usLength, unsigned char ucShift)
{
    unsigned char ucData;
    int iTxLength = 3;
    unsigned char ucTxBuffer[MODBUS_TX_BUFFER_SIZE];
    if (modbus_rx_function->ucSourceAddress == BROADCAST_MODBUS_ADDRESS) {
        return 0;                                                        // never send an answer to a broadcast message
    }

    ucTxBuffer[0] = modbus_rx_function->ucSourceAddress;                 // enter the address
    ucTxBuffer[1] = modbus_rx_function->ucFunctionCode;                  // enter the function code
    if (ucShift & (NO_SHIFT_REGISTER_VALUE | NO_SHIFT_FIFO_VALUE)) {     // register values rather than bit values
        unsigned short *ptrShort = (unsigned short *)ptrData;            // we are dealing with short words
        if (ucShift & NO_SHIFT_FIFO_VALUE) {                             // 16 bit byte count plus 16 bit count
            ucTxBuffer[2] = (unsigned char)(usLength >> 8);              // special case for MODBUS_READ_FIFO_QUEUE response
            ucTxBuffer[3] = (unsigned char)(usLength);
            usLength /= 2;
            usLength--;
            ucTxBuffer[4] = (unsigned char)(usLength >> 8);
            ucTxBuffer[5] = (unsigned char)(usLength);
            iTxLength = 6;
        }
        else {
            ucTxBuffer[2] = (unsigned char)(usLength * 2);               // followed by the byte count
        }
        while (usLength != 0) {                                          // the number of short words
            ucTxBuffer[iTxLength++] = (unsigned char)(*ptrShort >> 8);   // add the register values in big endian from little endian format
            ucTxBuffer[iTxLength++] = (unsigned char)(*ptrShort++);
            usLength--;
        }
    }
    else {
        if (ucShift == NO_SHIFT_LENGTH_MUL_8) {                          // caller signalling that there are no bit shifts but the length is to be multiplied by 8
            iTxLength = 2;
            ucShift = 0;
            usLength *= 8;
        }
        else {
            ucTxBuffer[2] = (unsigned char)_MODBUS_BITS_BYTE_SIZE(usLength); // followed by the byte count
        }
        if (ucShift & SHIFT_LONG_WORD_ALIGNED) {                         // insert long words
    #if MODBUS_BITS_ELEMENT_WIDTH > 16
            unsigned long *ptrLong = (unsigned long *)ptrData;
            unsigned char ucData;
            CAST_POINTER_ARITHMETIC Offset = (CAST_POINTER_ARITHMETIC)ptrLong & 0x03;
            ptrLong = (unsigned long *)(((CAST_POINTER_ARITHMETIC)ptrLong) & ~0x03);
            ucShift &= 0x07;
            while (usLength != 0) {
                if (Offset == 0) {                                       // long word aligned address
                    ucData = (unsigned char)(*ptrLong);
                    if (ucShift != 0) {
                        ucData >>= ucShift;
                        ucData |= ((unsigned char)(*ptrLong >> 8) << (8 - ucShift));
                    }
                    if (usLength < 8) {
                        ucTxBuffer[iTxLength++] = (ucData & (0xff >> (8 - (unsigned char)usLength)));
                        break;
                    }
                    ucTxBuffer[iTxLength++] = ucData;
                    usLength -= 8;
                }
                if (Offset <= 1) {
                    ucData = (unsigned char)(*ptrLong >> 8);
                    if (ucShift != 0) {
                        ucData >>= ucShift;
                        ucData |= ((unsigned char)(*ptrLong >> 16) << (8 - ucShift));
                    }
                    if (usLength < 8) {
                        ucTxBuffer[iTxLength++] = (ucData & (0xff >> (8 - (unsigned char)usLength)));
                        break;
                    }
                    ucTxBuffer[iTxLength++] = ucData;
                    usLength -= 8;
                }
                if (Offset <= 2) {
                    ucData = (unsigned char)(*ptrLong >> 16);
                    if (ucShift != 0) {
                        ucData >>= ucShift;
                        ucData |= ((unsigned char)(*ptrLong >> 24) << (8 - ucShift));
                    }
                    if (usLength < 8) {
                        ucTxBuffer[iTxLength++] = (ucData & (0xff >> (8 - (unsigned char)usLength)));
                        break;
                    }
                    ucTxBuffer[iTxLength++] = ucData;
                    usLength -= 8;
                }
                ucData = (unsigned char)(*ptrLong++ >> 24);
                if (ucShift != 0) {
                    ucData >>= ucShift;
                    ucData |= ((unsigned char)(*ptrLong) << (8 - ucShift));
                }
                if (usLength < 8) {
                    ucTxBuffer[iTxLength++] = (ucData & (0xff >> (8 - (unsigned char)usLength)));
                    break;
                }
                ucTxBuffer[iTxLength++] = ucData;
                usLength -= 8;
            }
            Offset = 0;
    #endif
        }
        else if (ucShift & SHIFT_SHORT_WORD_ALIGNED) {                   // insert short words
    #if MODBUS_BITS_ELEMENT_WIDTH > 8
            unsigned short *ptrShort = (unsigned short *)ptrData;
            unsigned char ucData;
            ucShift &= 0x07;
            while (usLength != 0) {
                if ((CAST_POINTER_ARITHMETIC)ptrShort & 0x01) {          // uneven address
                    ptrShort = (unsigned short *)(((CAST_POINTER_ARITHMETIC)ptrShort) - 1); // set data pointer to boundary and ignore first byte
                }
                else {
                    ucData = (unsigned char)(*ptrShort);
                    if (ucShift != 0) {
                        ucData >>= ucShift;
                        ucData |= ((unsigned char)(*ptrShort >> 8) << (8 - ucShift));
                    }
                    if (usLength < 8) {
                        ucTxBuffer[iTxLength++] = (ucData & (0xff >> (8 - (unsigned char)usLength)));
                        break;
                    }
                    ucTxBuffer[iTxLength++] = ucData;
                    usLength -= 8;
                }
                ucData = (unsigned char)(*ptrShort++ >> 8);
                if (ucShift != 0) {
                    ucData >>= ucShift;
                    ucData |= ((unsigned char)(*ptrShort) << (8 - ucShift));
                }
                if (usLength < 8) {
                    ucTxBuffer[iTxLength++] = (ucData & (0xff >> (8 - (unsigned char)usLength)));
                    break;
                }
                ucTxBuffer[iTxLength++] = ucData;
                usLength -= 8;
            }
    #endif
        }
        else {                                                           // byte aligned
            unsigned char *ptrByte = (unsigned char *)ptrData;
            while (usLength != 0) {
                ucData = *ptrByte++;
                if (ucShift != 0) {
                    ucData >>= ucShift;
                    ucData |= (*ptrByte << (8 - ucShift));
                }
                if (usLength < 8) {
                    ucTxBuffer[iTxLength++] = (ucData & (0xff >> (8 - (unsigned char)usLength)));
                    break;
                }
                ucTxBuffer[iTxLength++] = ucData;
                usLength -= 8;
            }
        }
    }
    return (fnMODBUS_transmit(modbus_rx_function, ucTxBuffer, (unsigned short)(iTxLength + 2)));
}
#endif

#if defined MODBUS_SUPPORT_SERIAL_LINE_FUNCTIONS && defined USE_MODBUS_SLAVE
// Transmit using a delivered buffer with space for the address and function at the start and a CRC at the end
//
static void fnSendMODBUS_transparent_response(MODBUS_RX_FUNCTION *modbus_rx_function, unsigned char *ptrData, unsigned short usLength)
{
    *ptrData = modbus_rx_function->ucSourceAddress;                      // enter the address
    *(ptrData + 1) = modbus_rx_function->ucFunctionCode;                 // enter the function code
    fnMODBUS_transmit(modbus_rx_function, ptrData, usLength);
}
#endif

#if defined USE_MODBUS_SLAVE
// This routine constructs and transmits MODBUS exception frame
//
static int fnSendMODBUS_exception(MODBUS_RX_FUNCTION *modbus_rx_function, unsigned char ucExceptionCode)
{
    unsigned char ucTxBuff[5];
    if (modbus_rx_function->ucSourceAddress == BROADCAST_MODBUS_ADDRESS) {
        return 0;                                                        // never return a response to a broadcast
    }
    ucTxBuff[0] = modbus_rx_function->ucSourceAddress;
    ucTxBuff[1] = (modbus_rx_function->ucFunctionCode | 0x80);
    ucTxBuff[2] = ucExceptionCode;
    return (fnMODBUS_transmit(modbus_rx_function, ucTxBuff, sizeof(ucTxBuff)));
}
#endif


#if defined MODBUS_SUPPORT_SERIAL_LINE_FUNCTIONS && defined USE_MODBUS_SLAVE && (!defined NO_SLAVE_MODBUS_GET_COMM_EVENT_LOG || !defined NO_SLAVE_MODBUS_GET_COMM_EVENT_COUNTER) // {V1.02}

// Copy the last recorded events (up to 64) to the buffer and return the copied amount, which can also be 0
//
static unsigned char fnGetEvents(unsigned char *ptrBuf)
{
    return 0;
}
#endif


#if defined USE_MODBUS_MASTER
extern void fnMODBUS_event_report(int iEvent, unsigned char ucInterface)
{
    if (fnMasterCallback[ucInterface] != 0) {
        MODBUS_RX_FUNCTION modbus_rx_function; // = {0};
        uMemset(&modbus_rx_function, 0, sizeof(MODBUS_RX_FUNCTION));      // ensure struct zeroed
        modbus_rx_function.ucMODBUSport = ucInterface;
        fnMasterCallback[ucInterface](iEvent, &modbus_rx_function);
    }
}
#endif



#if defined MODBUS_TCP

extern MODBUS_TCP_CTL *fnGet_MODBUS_TCP_port_status(unsigned char ucMODBUSport) // {V1.07}
{
    #if MODBUS_TCP_MASTERS > 0
    if (ucMODBUSport >= MODBUS_SERIAL_INTERFACES) {
        return ptrMODBUS_master_TCP[ucMODBUSport - (MODBUS_SERIAL_INTERFACES + MODBUS_TCP_SERVERS)];
    }
    #endif
    return 0;
}

#if MODBUS_TCP_SERVERS > 0 && defined USE_MODBUS_SLAVE
// This is called to initialise the MODBUS slave tcp interface and set the user interface
//
static int fnInitialiseMODBUStcp(unsigned char ucModbusPort, unsigned char ucSessions)
{
    int i;
    if (ptrMODBUS_TCP == 0) {
        ptrMODBUS_TCP = uMalloc(MODBUS_TCP_SOCKETS * sizeof(MODBUS_TCP_CTL)); // get socket administration memory
    }
    else {
        ptrMODBUS_TCP += usMODBUS_sessions_allocated;
    }

    for (i = 0; i < ucSessions; i++)    {                                // open a socket for each possible session
        ptrMODBUS_TCP->tx_modbus_frame = uMalloc(sizeof(MODBUS_TCP_TX_FRAME)); // each socket has its own tx frame buffer in case it needs to repeat
        if (ptrMODBUS_TCP->tx_modbus_frame == 0) {
            return MODBUS_TCP_NO_HEAP_MEMORY;                            // not adequate heap memory
        }
        ptrMODBUS_TCP->OwnerTCPSocket = fnGetTCP_Socket(TOS_MAXIMISE_THROUGHPUT, ptrMODBUS_pars->usIdleTimeoutSlave[ucModbusPort - MODBUS_SERIAL_INTERFACES], fnMODBUSListener);
        if (fnTCP_Listen(ptrMODBUS_TCP->OwnerTCPSocket, ptrMODBUS_pars->usMODBUS_TCPport[ucModbusPort - MODBUS_SERIAL_INTERFACES], 0) != ptrMODBUS_TCP->OwnerTCPSocket) {
            return MODBUS_TCP_LISTENER_INSTALL_FAILED;                   // listener couldn't be started
        }
        ptrMODBUS_TCP->ucPort = ucModbusPort;                            // the MODBUS port the TCP session belongs to
        if (ptrMODBUS_pars->ucMODBUS_TCP_server_mode[ucModbusPort - MODBUS_SERIAL_INTERFACES] & MODBUS_TCP_SERIAL_GATEWAY) {
            ptrMODBUS_TCP->ucSerialPort = (ucModbusPort - MODBUS_SERIAL_INTERFACES); // gateway serial port
        }
        else {
            ptrMODBUS_TCP->ucSerialPort = NO_MODBUS_GATEWAY;             // mark that this port receives only local slave messages
        }
        ptrMODBUS_TCP++;
        usMODBUS_sessions_allocated++;
    }
    ptrMODBUS_TCP -= usMODBUS_sessions_allocated;                        // set our pointer back to first session
    return 0;                                                            // successful
}
#endif                                                                   // end if MODBUS TCP slave support

#if defined USE_MODBUS_SLAVE

// Handle a MODBUS TCP frame which has just been received
//
static int fnHandleModbusTcpData(MODBUS_TCP_FRAME *ptrMODBUS_frame, unsigned short usDataLength, MODBUS_TCP_CTL *modbus_session)
{
    unsigned short usMODBUSlength;
    #if defined USE_MODBUS_SLAVE && MODBUS_SHARED_INTERFACES > 0
    unsigned char ucSharedSlave = 0;
    #endif
    MODBUS_RX_FUNCTION modbus_rx_function;                               // build a temporary function block for subroutine use
    modbus_rx_function.ucSourceAddress = ptrMODBUS_frame->modbus_header.ucUnitIdentifier;
    modbus_rx_function.ucMappedAddress = modbus_rx_function.ucSourceAddress;
    modbus_rx_function.ucFunctionCode  = ptrMODBUS_frame->ucData[0];
    modbus_rx_function.ucSourceType    = TCP_BINARY_INPUT;
    modbus_rx_function.modbus_session  = modbus_session;
    modbus_rx_function.modbus_session->ptrMODBUS_input_frame = ptrMODBUS_frame;

    uMemcpy(&modbus_session->tx_modbus_frame->modbus_header, &ptrMODBUS_frame->modbus_header, sizeof(ptrMODBUS_frame->modbus_header)); // {V1.17}
    modbus_rx_function.ucMODBUSport = modbus_session->ucPort;            // {V1.17}

    if ((ptrMODBUS_frame->modbus_header.ucProtocolIdentifier[0] != 0) || (ptrMODBUS_frame->modbus_header.ucProtocolIdentifier[1] != 0)) { // ignore any invalid protocol identifiers
        return (fnSendMODBUS_exception(&modbus_rx_function, MODBUS_EXCEPTION_GATEWAY_PATH_UNAV) > 0);
    }
    usMODBUSlength = ptrMODBUS_frame->modbus_header.ucLength[0];         // the length of the content
    usMODBUSlength <<= 8;
    usMODBUSlength |= ptrMODBUS_frame->modbus_header.ucLength[1];
    if (usMODBUSlength != (usDataLength - (sizeof(MODBUS_TCP_HEADER) - 1))) {
        return (fnSendMODBUS_exception(&modbus_rx_function, MODBUS_EXCEPTION_CODE_3) > 0); // invalid content length
    }
  //uMemcpy(&modbus_session->tx_modbus_frame->modbus_header, &ptrMODBUS_frame->modbus_header, sizeof(ptrMODBUS_frame->modbus_header)); // {V1.17} set before exceptions can be called
  //modbus_session->usOutstandingData = 0;                               // {V1.11} only reset when ACK received
    #if defined MODBUS_TCP && MODBUS_SHARED_TCP_INTERFACES > 0
    if (ptrMODBUS_pars->ucMODBUS_TCP_server_mode[modbus_session->ucPort - MODBUS_SERIAL_INTERFACES] & MODBUS_TCP_SLAVE_PORT) { // check that slave function is active
        if ((ucSharedSlave = fnCheckSlaveAddresses(modbus_session->ucPort, ptrMODBUS_frame->modbus_header.ucUnitIdentifier)) != 0) {
            goto _shared_tcp_slave;
        }
        else {
            ucSharedSlave = modbus_session->ucPort;                      // message for main address
        }
    }
    #endif
    #if defined MODBUS_TCP_GATEWAY && MODBUS_SERIAL_INTERFACES > 0
    if (((ptrMODBUS_frame->modbus_header.ucUnitIdentifier != MODBUS_TCP_NON_SIGNIFICANT_UNIT_IDENTIFIER) && (ptrMODBUS_pars->ucMODBUS_TCP_server_mode[modbus_session->ucPort - MODBUS_SERIAL_INTERFACES] & (MODBUS_TCP_SLAVE_PORT | MODBUS_TCP_SERIAL_GATEWAY))) && (modbus_session->ucSerialPort < MODBUS_SERIAL_INTERFACES)) { // {V1.08} gateway function required
        int iRtn = APP_ACCEPT;
        #if defined TCP_SLAVE_ROUTER
        if (fnMasterCallback[modbus_session->ucPort] != 0) {
            iRtn = fnMasterCallback[modbus_session->ucPort](TCP_ROUTE_FROM_SLAVE, &modbus_rx_function);
            if (iRtn <= MODBUS_APP_GATEWAY_FUNCTION) {                   // {V1.05} the user wants this received message to be passed to a gateway
                return (fnWaitException(iRtn, &modbus_rx_function, 0));  // {V1.07} route and return whether successful so that TCP knows whether to return an ACK or not
            }
        }
        else
        #endif
        if (SerialHandle[modbus_session->ucSerialPort] == 0) {           // check that the serial port is available
            iRtn = (fnSendMODBUS_exception(&modbus_rx_function, MODBUS_EXCEPTION_GATEWAY_PATH_UNAV) > 0);
        }
        else {
            MODBUS_ROUTE routing_table;
            routing_table.ucNextRange = 0xff;
            routing_table.ucMODBUSPort = modbus_session->ucSerialPort;
            fnMODBUS_route(TCP_ROUTE_FROM_SLAVE, &modbus_rx_function, &routing_table);
        }
        if (ptrMODBUS_frame->modbus_header.ucUnitIdentifier != BROADCAST_MODBUS_ADDRESS) {
            return iRtn;
        }
    }
    #endif
    #if defined MODBUS_TCP && MODBUS_SHARED_TCP_INTERFACES > 0
_shared_tcp_slave:
    #endif
  //modbus_rx_function.ucMODBUSport = modbus_session->ucPort;            // {V1.17} set before exceptions can be called
    #if defined USE_MODBUS_SLAVE && MODBUS_SHARED_INTERFACES > 0
    modbus_rx_function.ucMODBUS_Slaveport = ucSharedSlave;
    #endif
    #if defined MODBUS_DELAYED_RESPONSE
    modbus_rx_function.ucDelayed = 0;
    #endif
    modbus_rx_function.data_content.usUserDataLength = (usMODBUSlength - 2);
    modbus_rx_function.data_content.user_data = &ptrMODBUS_frame->ucData[1];
    if (fnHandleMODBUS_input(&modbus_rx_function) != 0) {                // handle the received MODBUS query
        return APP_SENT_DATA;                                            // note: it is known that the return value MODBUS_TX_MESSAGE_LOST_QUEUE_REQUIRED occurs when a previous TCP ACK is still outstanding - resulting in the reception not being acked although data was not sent. This provokes a TCP repetition from the MODBUS TCP master. This would normally never occur but its behavior is documented here in case it should be encountered as a consequence of another error
    }
    #if defined MODBUS_TCP && MODBUS_SHARED_TCP_INTERFACES > 0
    if (ptrMODBUS_frame->modbus_header.ucUnitIdentifier == BROADCAST_MODBUS_ADDRESS) { // allow all shared slaves to process a broadcast
        if (ptrSharedPort != 0) {
            SHARED_MODBUS_PORT *ptrPort = ptrSharedPort;
            unsigned char ucSharedSlave = 0;
            while (ptrPort->ucMODBUSPort != 0xff) {
                if (ptrPort->ucMODBUSPort == modbus_session->ucPort) {
                    modbus_rx_function.ucMODBUS_Slaveport = (ucSharedSlave + MODBUS_SERIAL_INTERFACES + MODBUS_TCP_SERVERS);
                    fnHandleMODBUS_input(&modbus_rx_function);           // perform generic MODBUS frame handling
                }
                if (++ucSharedSlave >= MODBUS_SHARED_INTERFACES) {
                    break;
                }
                ptrPort++;
            }
        }
    }
    #endif
    return APP_ACCEPT;
}


// MODBUS TCP slave listener
//
static int fnMODBUSListener(USOCKET Socket, unsigned char ucEvent, unsigned char *ucIp_Data, unsigned short usPortLen)
{
    MODBUS_TCP_CTL *modbus_session = ptrMODBUS_TCP;
    int iSessionNumber;

    for (iSessionNumber = 0; iSessionNumber < MODBUS_TCP_SOCKETS; iSessionNumber++) {
        if (_TCP_SOCKET_MASK(modbus_session->OwnerTCPSocket) == _TCP_SOCKET_MASK(Socket)) { // {V1.23} search for the session to handle this event
            switch (ucEvent) {
            case TCP_EVENT_CONREQ:                                       // session requested
                if (modbus_session->ucState == MODBUS_STATE_FREE) {
                    modbus_session->ucState = MODBUS_STATE_RESERVED;     // reserve the session if the socket is available
                    modbus_session->OwnerTCPSocket = Socket;             // {V1.23} add network, VLAN and interface details
                }
                break;

            case TCP_EVENT_ABORT:
                modbus_session->usOutstandingData = 0;                   // {V1.13}
            case TCP_EVENT_CLOSED:
                modbus_session->ucState = MODBUS_STATE_FREE;
                fnTCP_Listen(Socket, ptrMODBUS_pars->usMODBUS_TCPport[modbus_session->ucPort - MODBUS_SERIAL_INTERFACES], 0); // go back to listening state
                break;

            case TCP_EVENT_CONNECTED:
                modbus_session->ucState = MODBUS_STATE_ACTIVE;
                break;

            case TCP_EVENT_ACK:                                          // the last transmission was successful - we can continue sending next data
                modbus_session->usOutstandingData = 0;                   // data acknowledged
                return APP_ACCEPT;

            case TCP_EVENT_REGENERATE:                                   // no ack was received for previous data
                if (modbus_session->ucState < MODBUS_STATE_ACTIVE) {     // ignore if the server has closed in the meantime
                    return APP_ACCEPT;
                }
                return (fnSendTCP(modbus_session->OwnerTCPSocket, (unsigned char *)&modbus_session->tx_modbus_frame->tTCP_Header, modbus_session->usOutstandingData, TCP_FLAG_PUSH) > 0);

            case TCP_EVENT_DATA:
                return (fnHandleModbusTcpData((MODBUS_TCP_FRAME *)ucIp_Data, (unsigned short)(usPortLen), modbus_session));

            case TCP_EVENT_CLOSE:
                break;
            }
            return APP_ACCEPT;
        }
        modbus_session++;
    }
    return APP_REJECT;                                                   // unexpected occurance - signal rejection...
}
#endif                                                                   // end if USE_MODBUS_SLAVE

#endif                                                                   // end if MODBUS_TCP


#if defined USE_MODBUS_MASTER

// The user can control master transmissions via this function
// When the slave responds or a timeout occurs the application will be informed
//
extern int fnMODBUS_Master_send(unsigned char ucModbusPort, unsigned char ucSlave, unsigned short usFunction, void *details)
{
    int iRtn;
    unsigned char ucBuff[MODBUS_TX_BUFFER_SIZE];
    unsigned short usLength = 8;
    MODBUS_RX_FUNCTION modbus_rx_function;                               // build a temporary function block for subroutine use
    unsigned char *ptrBuf = ucBuff;
    modbus_rx_function.ucMODBUSport = ucModbusPort;
    modbus_rx_function.ucSourceAddress = 0;
    modbus_rx_function.ucFunctionCode = 0;
    if (ucModbusPort < MODBUS_SERIAL_INTERFACES) {
    #if MODBUS_SERIAL_INTERFACES > 0
        if (ptrMODBUS_pars->ucModbusSerialPortMode[ucModbusPort] & MODBUS_MODE_ASCII) {
            modbus_rx_function.ucSourceType = ASCII_SERIAL_INPUT;
        }
        else {
            modbus_rx_function.ucSourceType = RTU_SERIAL_INPUT;
        }
    #endif
    }
    #if MODBUS_TCP_MASTERS > 0
    else {                                                               // TCP port
        modbus_rx_function.ucSourceType = TCP_BINARY_INPUT;
        modbus_rx_function.modbus_session = ptrMODBUS_master_TCP[ucModbusPort - (MODBUS_SERIAL_INTERFACES + MODBUS_TCP_SERVERS)];
    }
    #endif
    ucBuff[0] = ucSlave;
    ucBuff[1] = (unsigned char)usFunction;
    switch (usFunction) {
    #if defined TCP_SLAVE_ROUTER
    case MODBUS_TRANSFER:                                                // transparent transfer of data
    case MODBUS_TRANSFER_TCP_UART:                                       // transparent from TCP to UART
        ptrBuf = ((MODBUS_TCP_FRAME *)details)->ucData;
        ptrBuf--;
        usLength = ((MODBUS_TCP_FRAME *)details)->modbus_header.ucLength[0];
        usLength <<= 8;
        usLength |= ((MODBUS_TCP_FRAME *)details)->modbus_header.ucLength[1];
      //if (MODBUS_TRANSFER_TCP_UART == usFunction) {                       {V1.06}
            usLength += 2;                                               // respect that a CRC will be added when transmitting UART data
      //}
        break;
    #endif
    #if !defined NO_MASTER_MODBUS_READ_COILS
    case MODBUS_READ_COILS:                                              // the master wants to read coils
    #endif
    #if !defined NO_MASTER_MODBUS_READ_DISCRETE_INPUTS
    case MODBUS_READ_DISCRETE_INPUTS:                                    // the master wants to read discrete inputs
    #endif
    #if !defined NO_MASTER_MODBUS_READ_HOLDING_REGISTERS
    case MODBUS_READ_HOLDING_REGISTERS:                                  // the master wants to read holding registers
    #endif
    #if !defined NO_MASTER_MODBUS_READ_INPUT_REGISTERS
    case MODBUS_READ_INPUT_REGISTERS:                                    // the master wants to read input registers
    #endif
    #if !defined NO_MASTER_MODBUS_READ_COILS || !defined NO_MASTER_MODBUS_READ_DISCRETE_INPUTS || !defined NO_MASTER_MODBUS_READ_HOLDING_REGISTERS || !defined NO_MASTER_MODBUS_READ_INPUT_REGISTERS
        {
            MODBUS_READ_QUANTITY *coilDetails = (MODBUS_READ_QUANTITY *)details;
            ucBuff[2] = (unsigned char)(coilDetails->usStartAddress >> 8);
            ucBuff[3] = (unsigned char)(coilDetails->usStartAddress);
            ucBuff[4] = (unsigned char)(coilDetails->usQuantity >> 8);
            ucBuff[5] = (unsigned char)(coilDetails->usQuantity);
        }
        break;
    #endif
    #if !defined NO_MASTER_MODBUS_WRITE_SINGLE_COIL
    case MODBUS_WRITE_SINGLE_COIL:                                       // the master wants to set the state of a single coil
        {
            MODBUS_COIL_STATE *coil_state = (MODBUS_COIL_STATE *)details;
            ucBuff[2] = (unsigned char)(coil_state->usAddress >> 8);
            ucBuff[3] = (unsigned char)(coil_state->usAddress);
            if (coil_state->ucState != 0) {
                ucBuff[4] = 0xff;                                        // set state '1'
            }
            else {
                ucBuff[4] = 0;                                           // set state '0'
            }
            ucBuff[5] = 0;
        }
        break;
    #endif
    #if !defined NO_MASTER_MODBUS_WRITE_SINGLE_REGISTER
    case MODBUS_WRITE_SINGLE_REGISTER:                                   // the master wants to write a single register value
        {
            MODBUS_SINGLE_REGISTER *register_value = (MODBUS_SINGLE_REGISTER *)details;
            ucBuff[2] = (unsigned char)(register_value->usAddress >> 8);
            ucBuff[3] = (unsigned char)(register_value->usAddress);
            ucBuff[4] = (unsigned char)(register_value->usValue >> 8);
            ucBuff[5] = (unsigned char)(register_value->usValue);
        }
        break;
    #endif
    #if !defined NO_MASTER_MODBUS_WRITE_MULTIPLE_COILS
    case MODBUS_WRITE_MULTIPLE_COILS:                                    // the master wants to write multiple coils
        {
            MODBUS_BITS *coil_values = (MODBUS_BITS *)details;
            unsigned short usRangeLength = ((coil_values->address_range.usEndAddress - coil_values->address_range.usStartAddress) + 1);
            unsigned char ucBytes;
            ucBuff[2] = (unsigned char)(coil_values->address_range.usStartAddress >> 8);
            ucBuff[3] = (unsigned char)(coil_values->address_range.usStartAddress);
            ucBuff[4] = (unsigned char)(usRangeLength >> 8);
            ucBuff[5] = (unsigned char)(usRangeLength);
            ucBytes   = _MODBUS_BITS_BYTE_SIZE((unsigned char)usRangeLength); // the number of bytes
            ucBuff[6] = ucBytes;
            uMemcpy(&ucBuff[7], coil_values->ptrBits, ucBytes);          // the coil values
            ucBuff[6 + ucBytes] &= (0xff >> ((ucBytes * 8) - usRangeLength)); // clear any unused bits in the last byte
            usLength = (ucBytes + 9);
        }
        break;
    #endif
    #if !defined NO_MASTER_MODBUS_WRITE_MULTIPLE_REGISTERS
    case MODBUS_WRITE_MULTIPLE_REGISTERS:                                // the master wants to write multiple registers
        {
            MODBUS_REGISTERS *register_values = (MODBUS_REGISTERS *)details;
            unsigned short *ptrValues = register_values->ptrRegisters;
            unsigned short usRangeLength = ((register_values->address_range.usEndAddress - register_values->address_range.usStartAddress) + 1);
            int iOffset = 7;
            unsigned char ucBytes;
            ucBuff[2] = (unsigned char)(register_values->address_range.usStartAddress >> 8);
            ucBuff[3] = (unsigned char)(register_values->address_range.usStartAddress);
            ucBuff[4] = (unsigned char)(usRangeLength >> 8);
            ucBuff[5] = (unsigned char)(usRangeLength);
            ucBytes   = (unsigned char)(usRangeLength * 2);              // the number of bytes
            ucBuff[6] = ucBytes;
            usLength = (ucBytes + 9);
            while (ucBytes) {
                ucBuff[iOffset++] = (unsigned char)(*ptrValues >> 8);    // the register values
                ucBuff[iOffset++] = (unsigned char)(*ptrValues++);
                ucBytes -= 2;
            }
        }
        break;
    #endif
    #if !defined NO_MASTER_MODBUS_MASK_WRITE_REGISTER
    case MODBUS_MASK_WRITE_REGISTER:
        {
            MODBUS_SINGLE_REGISTER_MASK *maskWrite = (MODBUS_SINGLE_REGISTER_MASK *)details;
            ucBuff[2] = (unsigned char)(maskWrite->usAddress >> 8);
            ucBuff[3] = (unsigned char)(maskWrite->usAddress);
            ucBuff[4] = (unsigned char)(maskWrite->usValueAND >> 8);
            ucBuff[5] = (unsigned char)(maskWrite->usValueAND);
            ucBuff[6] = (unsigned char)(maskWrite->usValueOR >> 8);
            ucBuff[7] = (unsigned char)(maskWrite->usValueOR);
            usLength = 10;
        }
        break;
    #endif
    #if !defined NO_MASTER_MODBUS_READ_WRITE_MULTIPLE_REGISTER
    case MODBUS_READ_WRITE_MULTIPLE_REGISTER:
        {
            MODBUS_READ_WRITE_REGISTERS *multi_regs = (MODBUS_READ_WRITE_REGISTERS *)details;
            unsigned short *ptrReg = multi_regs->reg_write.ptrRegisters;
            unsigned short usQuantity;
            int x = 11;
            ucBuff[2] = (unsigned char)(multi_regs->reg_read.usStartAddress >> 8);
            ucBuff[3] = (unsigned char)(multi_regs->reg_read.usStartAddress);
            ucBuff[4] = 0;                                               // range 1..0x7d doesn't need MSB
            ucBuff[5] = (unsigned char)(multi_regs->reg_read.usQuantity);
            ucBuff[6] = (unsigned char)(multi_regs->reg_write.address_range.usStartAddress >> 8);
            ucBuff[7] = (unsigned char)(multi_regs->reg_write.address_range.usStartAddress);
            usQuantity = ((multi_regs->reg_write.address_range.usEndAddress - multi_regs->reg_write.address_range.usStartAddress) + 1);
            ucBuff[8] = 0;                                               // range 1..0x79 doesn't need MSB
            ucBuff[9] = (unsigned char)(usQuantity);
            ucBuff[10] = (unsigned char)(usQuantity * 2);                // byte count
            while (usQuantity--) {
                ucBuff[x++] = (unsigned char)(*ptrReg >> 8);             // add the write register content
                ucBuff[x++] = (unsigned char)(*ptrReg++);
            }
            usLength = (unsigned short)(x + 2);
        }
        break;
    #endif
    #if !defined NO_MASTER_MODBUS_READ_FIFO_QUEUE
    case MODBUS_READ_FIFO_QUEUE:
        {
            FIFO_ADDRESS *fifo_add = (FIFO_ADDRESS *)details;
            ucBuff[2] = (unsigned char)(fifo_add->usFIFO_address >> 8);
            ucBuff[3] = (unsigned char)(fifo_add->usFIFO_address);
            usLength = 6;
        }
        break;
    #endif
    #if defined MODBUS_SUPPORT_SERIAL_LINE_FUNCTIONS
        #if !defined NO_MASTER_MODBUS_READ_EXCEPTION_STATUS
    case MODBUS_READ_EXCEPTION_STATUS:                                   // serial line only
        #endif
        #if !defined NO_MASTER_MODBUS_GET_COMM_EVENT_COUNTER
    case MODBUS_GET_COMM_EVENT_COUNTER:
        #endif
        #if !defined NO_MASTER_MODBUS_GET_COMM_EVENT_LOG
    case MODBUS_GET_COMM_EVENT_LOG:
        #endif
        #if !defined NO_MASTER_MODBUS_REPORT_SLAVE_ID
    case MODBUS_REPORT_SLAVE_ID:
        #endif
        #if !defined NO_MASTER_MODBUS_READ_EXCEPTION_STATUS || !defined NO_MASTER_MODBUS_GET_COMM_EVENT_COUNTER || !defined NO_MASTER_MODBUS_GET_COMM_EVENT_LOG || !defined NO_MASTER_MODBUS_REPORT_SLAVE_ID
        usLength = 4;
        break;
        #endif
        #if defined MODBUS_SUPPORT_SERIAL_LINE_DIAGNOSTICS & !defined NO_MASTER_MODBUS_DIAGNOSTICS
    case MODBUS_DIAGNOSTICS:                                             // serial line only
        {
            MODBUS_DIAGNOSTICS_REQUEST *diagnostics = (MODBUS_DIAGNOSTICS_REQUEST *)details;
            ucBuff[2] = (unsigned char)(diagnostics->usSubFunction >> 8);
            ucBuff[3] = (unsigned char)(diagnostics->usSubFunction);
            ucBuff[4] = (unsigned char)(diagnostics->usData >> 8);
            ucBuff[5] = (unsigned char)(diagnostics->usData);
        }
        break;
        #endif
    #endif
  //case MODBUS_READ_FILE_RECORD:                                        // not supported
  //case MODBUS_WRITE_FILE_RECORD:                                       // not supported
  //case MODBUS_ENCAPSUL_INTERFACE_TRANSPORT:                            // not supported
    default:                                                             // unrecognised functions are treated as transparent user content
        {
            MODBUS_DATA_CONTENT *transparentContent = (MODBUS_DATA_CONTENT *)details;
            uMemcpy(&ucBuff[2], transparentContent->user_data, transparentContent->usUserDataLength);
            usLength = (transparentContent->usUserDataLength + 4);
        }
        break;
    }

    #if defined MODBUS_GATE_WAY_QUEUE
    if ((iRtn = fnQueueMODBUS_Message(ucModbusPort, ptrBuf, usLength, NO_ROUTE_INFO)) != 0) {
        return iRtn;                                                     // MODBUS serial master is presently busy so the message has been queued
    }
    #endif
    #if MODBUS_SERIAL_INTERFACES > 0
        #if defined _V1_18B1
            return (fnMODBUS_transmit_master_timer(&modbus_rx_function, ptrBuf, (unsigned char)usLength));
        #else
    if ((iRtn = fnMODBUS_transmit(&modbus_rx_function, ptrBuf, (unsigned char)usLength)) >= usLength) {
            #if defined MULTIPLE_SW_TIMERS
        uTaskerGlobalMonoTimer(OWN_TASK, ptrMODBUS_pars->serial_master_timeout[ucModbusPort], (unsigned char)(T_TIMER_SLAVE - ucModbusPort)); // monitor the transaction
            #else
        uTaskerMonoTimer(OWN_TASK, ptrMODBUS_pars->serial_master_timeout[ucModbusPort], T_TIMER_SLAVE); // monitor the transaction
            #endif
        return 0;                                                        // OK
    }
    if (MODBUS_BROADCAST_TRANSMITTED == iRtn) {                          // the broadcast transmission was sent to a MODBUS serial port, the next can be sent either after transmission (ASCII) or after transmission plus 3.5T inter-character space (RTU)
            #if defined MULTIPLE_SW_TIMERS
        uTaskerGlobalMonoTimer(OWN_TASK, ptrMODBUS_pars->serial_master_broadcast_timeout[ucModbusPort], (unsigned char)(T_TIMER_BROADCAST - ucModbusPort)); // give the broadcast a timeout period
            #else
        uTaskerMonoTimer(OWN_TASK, ptrMODBUS_pars->serial_master_broadcast_timeout[ucModbusPort], T_TIMER_BROADCAST); // give the broadcast a timeout period
            #endif
        return 0;
    }
    return iRtn;                                                         // TCP masters don't start a timer here, but instead when the TCP ack has been received
        #endif
    #else
    return (fnMODBUS_transmit(&modbus_rx_function, ptrBuf, (unsigned char)usLength));
    #endif
}

extern int fnClose_MODBUS_port(unsigned char ucMODBUSport)               // {V1.07}
{
    #if defined MODBUS_TCP && MODBUS_TCP_MASTERS > 0                     // {V1.08}
    if (ucMODBUSport >= (MODBUS_SERIAL_INTERFACES + MODBUS_TCP_SERVERS)) {
        MODBUS_TCP_CTL *modbus_session = ptrMODBUS_master_TCP[ucMODBUSport - (MODBUS_SERIAL_INTERFACES + MODBUS_TCP_SERVERS)];
        if (modbus_session != 0) {
            return fnTCP_close(modbus_session->OwnerTCPSocket);
        }
    }
    #endif
    return -1;                                                           // not a TCP master
}
#endif                                                                   // end if USE_MODBUS_MASTER

#if defined MODBUS_GATE_WAY_ROUTING

// MODBUS routing function. This can be called by application callback functions to route from gateways to destination ports
//
extern int fnMODBUS_route(int iType, MODBUS_RX_FUNCTION *modbus_rx_function, MODBUS_ROUTE *routing_table)
{
    switch (iType) {
    #if defined MODBUS_TCP
    case TCP_ROUTE_FROM_SLAVE:
        {
            MODBUS_TCP_FRAME *ptrMODBUS_frame = modbus_rx_function->modbus_session->ptrMODBUS_input_frame;
            while (1) {
                if (routing_table->ucNextRange >= ptrMODBUS_frame->modbus_header.ucUnitIdentifier) {
                    unsigned short usTransfer;
                    if (routing_table->ucMODBUSPort == 0xff) {           // ignore range
                        break;
                    }
                    if ((open_routes[routing_table->ucMODBUSPort].Valid == 0)
        #if defined MODBUS_GATE_WAY_QUEUE && defined _V1_07B3
                        && (modbus_queue[routing_table->ucMODBUSPort].ucOutstanding == 0)
        #endif
                    ) {
                        if (ptrMODBUS_frame->modbus_header.ucUnitIdentifier != BROADCAST_MODBUS_ADDRESS) {
                            open_routes[routing_table->ucMODBUSPort].ucID[0] = ptrMODBUS_frame->modbus_header.ucTransactionIdentifier[0]; // save the transaction identifier
                            open_routes[routing_table->ucMODBUSPort].ucID[1] = ptrMODBUS_frame->modbus_header.ucTransactionIdentifier[1];
                            open_routes[routing_table->ucMODBUSPort].modbus_session = modbus_rx_function->modbus_session; // the session that the answer should be returned to
                            open_routes[routing_table->ucMODBUSPort].ucMappedAddress = ptrMODBUS_frame->modbus_header.ucUnitIdentifier;
                            ptrMODBUS_frame->modbus_header.ucUnitIdentifier = modbus_rx_function->ucMappedAddress;
                            open_routes[routing_table->ucMODBUSPort].Valid = 1; // we are expecting the response from this port
                        }
                    }
                    else {
        #if defined MODBUS_GATE_WAY_QUEUE
                        unsigned short usLength = ptrMODBUS_frame->modbus_header.ucLength[0];
                        unsigned char *ptrHeader = (unsigned char *)&ptrMODBUS_frame->modbus_header;
                        unsigned char ucType;
                        ptrHeader -= sizeof(MODBUS_TCP_CTL *);           // make use of the IP frame part of received TCP frame
                        uMemcpy(ptrHeader, &modbus_rx_function->modbus_session, sizeof(MODBUS_TCP_CTL *));
                        usLength <<= 8;
                        usLength |= ptrMODBUS_frame->modbus_header.ucLength[1];
                        usLength += (sizeof(MODBUS_TCP_HEADER) + 1 + sizeof(MODBUS_TCP_CTL *));
                        if (ptrMODBUS_frame->modbus_header.ucUnitIdentifier != modbus_rx_function->ucMappedAddress) { // if the slave address is to be mapped, add this information to the queue
                            ptrHeader--;
                            *ptrHeader = ptrMODBUS_frame->modbus_header.ucUnitIdentifier;
                            ptrMODBUS_frame->modbus_header.ucUnitIdentifier = modbus_rx_function->ucMappedAddress;
                            usLength++;
                            ucType = TCP_ROUTE_INFO_WITH_MAPPED_ADDRESS;
                        }
                        else {
                            ucType = TCP_ROUTE_INFO;
                        }
                        if (MODBUS_TX_MESSAGE_LOST_NO_QUEUE_SPACE == fnQueueMODBUS_Message(routing_table->ucMODBUSPort, ptrHeader, usLength, ucType)) {
                            return APP_REJECT_DATA;                      // no room to accept the data so reject the TCP frame so that it will be repeated later
                        }
                        if ((ptrMODBUS_frame->modbus_header.ucUnitIdentifier != BROADCAST_MODBUS_ADDRESS) || (routing_table->ucNextRange == 0xff)) {
                            return APP_ACCEPT;
                        }
        #else
                        return APP_REJECT_DATA;                          // no room to accept the data so reject the TCP frame so that it will be repeated later
        #endif
                    }
        #if MODBUS_SERIAL_INTERFACES > 0
                    if (routing_table->ucMODBUSPort < MODBUS_SERIAL_INTERFACES) {
                        usTransfer = MODBUS_TRANSFER_TCP_UART;
                    }
                    else {
                        usTransfer = MODBUS_TRANSFER;
                    }
        #else
                    usTransfer = MODBUS_TRANSFER;
        #endif
                    if (MODBUS_TX_MESSAGE_LOST_NO_QUEUE_SPACE == fnMODBUS_Master_send(routing_table->ucMODBUSPort, 0, usTransfer, ptrMODBUS_frame)) { // pass on to MODBUS port
                        return APP_REJECT_DATA;                          // no room to accept the data so reject the TCP frame so that it will be repeated later
                    }
                    if ((ptrMODBUS_frame->modbus_header.ucUnitIdentifier != BROADCAST_MODBUS_ADDRESS) || (routing_table->ucNextRange == 0xff)) {
                        break;
                    }
                }
                routing_table++;
            }
            return APP_ACCEPT;
        }
        break;
    #endif
    #if defined MODBUS_SERIAL_INTERFACES
    case SERIAL_ROUTE_FROM_SLAVE:
        {
            while (1) {
                if (routing_table->ucNextRange >= modbus_rx_function->ucSourceAddress) {
                    if (routing_table->ucMODBUSPort == 0xff) {           // ignore range
                        break;
                    }
                    if ((open_routes[routing_table->ucMODBUSPort].Valid == 0)
        #if defined MODBUS_GATE_WAY_QUEUE && defined _V1_07B3
                        && (modbus_queue[routing_table->ucMODBUSPort].ucOutstanding == 0)
        #endif
                        ) {
        #if defined MODBUS_TCP
                        open_routes[routing_table->ucMODBUSPort].modbus_session = 0; // not TCP
        #endif
                        open_routes[routing_table->ucMODBUSPort].ucDestinationMODBUSport = modbus_rx_function->ucMODBUSport;
                        open_routes[routing_table->ucMODBUSPort].ucMappedAddress = modbus_rx_function->ucSourceAddress;
                        open_routes[routing_table->ucMODBUSPort].Valid = 1;  // we are expecting the response from this port
                        fnMODBUS_Master_send(routing_table->ucMODBUSPort, modbus_rx_function->ucMappedAddress, (unsigned short)(MODBUS_TRANSFER_SERIAL | modbus_rx_function->ucFunctionCode), &modbus_rx_function->data_content);// pass on to MODBUS port
                        return 1;
                    }
                    else {
        #if defined MODBUS_GATE_WAY_QUEUE
            #if defined _V1_07B4_1
                        unsigned short usLength = (unsigned short)(modbus_rx_function->data_content.usUserDataLength + 6);
                        unsigned char *ptrContent = (modbus_rx_function->data_content.user_data - 2);
                        *(ptrContent + usLength - 4) = modbus_rx_function->ucMODBUSport; // the routing port
                        *(ptrContent + usLength - 3) = *ptrContent;  // the original slave address
                        *ptrContent = modbus_rx_function->ucMappedAddress; // modify the remote slave address
                        return (fnQueueMODBUS_Message(routing_table->ucMODBUSPort, ptrContent, usLength, SERIAL_ROUTE_INFO));
            #else
                        return (fnQueueMODBUS_Message(routing_table->ucMODBUSPort, (modbus_rx_function->data_content.user_data - 2), (unsigned short)(modbus_rx_function->data_content.usUserDataLength + 4), NO_ROUTE_INFO));
            #endif
        #else
                        return MODBUS_TX_MESSAGE_LOST_NO_QUEUE_SPACE;
        #endif
                    }
                }
                routing_table++;
            }
        }
        break;
    #endif
    }
    return 0;
}

// Check whether this frame needs to be routed back via a gateway rather than handled locally. If it is to be routed, perform the routing here.
//
static int fnReverseSerialRoute(MODBUS_RX_FUNCTION *modbus_rx_function)
{
    unsigned char ucMODBUSport = modbus_rx_function->ucMODBUSport;
    if (open_routes[ucMODBUSport].Valid != 0) {                          // are we expecting a response on this MODBUS port?
        MODBUS_RX_FUNCTION copy_modbus_rx_function;
        uMemcpy(&copy_modbus_rx_function, modbus_rx_function, sizeof(MODBUS_RX_FUNCTION)); // make a copy to avoid corrupting content which could still be used by caller
        open_routes[ucMODBUSport].Valid = 0;                             // {V1.22} the entry will always be deleted
        #if defined MODBUS_TCP
        if (open_routes[ucMODBUSport].modbus_session != 0) {             // tcp return path rather than serial
            if (open_routes[ucMODBUSport].modbus_session->ucState != MODBUS_STATE_ACTIVE) { // {V1.22} check that the TCP connection is still open and silently ignore if not
                return 1;                                                // reverse route performed
            }
            copy_modbus_rx_function.ucSourceType = TCP_BINARY_INPUT;
            copy_modbus_rx_function.modbus_session = open_routes[ucMODBUSport].modbus_session;
            copy_modbus_rx_function.ucMODBUSport = open_routes[ucMODBUSport].modbus_session->ucPort;
            copy_modbus_rx_function.modbus_session->tx_modbus_frame->modbus_header.ucTransactionIdentifier[0] = open_routes[ucMODBUSport].ucID[0];
            copy_modbus_rx_function.modbus_session->tx_modbus_frame->modbus_header.ucTransactionIdentifier[1] = open_routes[ucMODBUSport].ucID[1];
            *(copy_modbus_rx_function.data_content.user_data - 2) = open_routes[ucMODBUSport].ucMappedAddress; // overwrite the unit identifier with the return value
        }
            #if MODBUS_SERIAL_INTERFACES > 0
        else {
            copy_modbus_rx_function.ucMODBUSport = open_routes[ucMODBUSport].ucDestinationMODBUSport;
            if (ptrMODBUS_pars->ucModbusSerialPortMode[copy_modbus_rx_function.ucMODBUSport] & MODBUS_MODE_ASCII) { // {V1.03}{V1.05}
                copy_modbus_rx_function.ucSourceType = ASCII_SERIAL_INPUT; 
            } 
            else { 
                copy_modbus_rx_function.ucSourceType = RTU_SERIAL_INPUT; 
            } 
        }
            #endif
        #else
        copy_modbus_rx_function.ucMODBUSport = open_routes[ucMODBUSport].ucDestinationMODBUSport;
        #endif
        fnMODBUS_transmit(&copy_modbus_rx_function, (copy_modbus_rx_function.data_content.user_data - 2), (unsigned short)(copy_modbus_rx_function.data_content.usUserDataLength + 4));
      //open_routes[ucMODBUSport].Valid = 0;                             // {V1.22} - moved to start of routine
        return 1;                                                        // reverse route performed
    }
    return 0;                                                            // no routing necessary
}
#endif

#endif                                                                   // end if defined USE_MODBUS
