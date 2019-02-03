/**********************************************************************
   Mark Butcher    Bsc (Hons) MPhil MIET

   M.J.Butcher Consulting
   Birchstrasse 20f,    CH-5406, Rütihof
   Switzerland

   www.uTasker.com    Skype: M_J_Butcher

   --------------------------------------------------------------------
   File:        modbus.h
   Project:     uTasker Demonstration project
   --------------------------------------------------------------------
   Copyright (C) M.J.Butcher Consulting 2004..2017
   ********************************************************************
   22.08.2009 Dimensions of tcp_master_timeout and tcp_master_broadcast_timeout corrected to MODBUS_TCP_MASTERS {V1.04}
   29.09.2009 Add SERIAL_ROUTE_INFO                                      {V1.07}
   02.09.2009 Add fnClose_MODBUS_port() and fnGet_MODBUS_TCP_port_status() {V1.07}
   23.11.2009 Struct packing control removed to driver.h
   20.01.2010 Add ADJUST_READ_WRITE_MULTIPLE_REGISTER                    {V1.09}
   03.05.2010 Allow MODBUS_TCP_SERVERS 0 value to disable                {V1.10}
   25.08.2010 Extra configuration definitions for more verified configurations {V1.14}

*/


// MODBUS parameters
//
#define SLAVE_ID_LENGTH        20
typedef struct stMODBUS_PARS
{
    unsigned char  ucModbusParVersion;                                   // version of MODBUS parameter block
#if defined MODBUS_SERIAL_INTERFACES && defined SERIAL_INTERFACE
    unsigned char  ucModbus_slave_address[MODBUS_SERIAL_INTERFACES];     // slave address on MODBUS
    unsigned short usSerialMode[MODBUS_SERIAL_INTERFACES];               // serial settings
    unsigned char  ucSerialSpeed[MODBUS_SERIAL_INTERFACES];              // Baud rate of serial interface
    unsigned char  ucModbusSerialPortMode[MODBUS_SERIAL_INTERFACES];     // RTC or ASCII - master or slave
    #if defined MODBUS_GATE_WAY_QUEUE
    QUEUE_TRANSFER serial_master_queue_size[MODBUS_SERIAL_INTERFACES];   // queue size for buffering waiting MODBUS messages
    #endif
    #if defined MODBUS_ASCII
    DELAY_LIMIT    inter_character_limit[MODBUS_SERIAL_INTERFACES];      // longest period between character in ASCII mode
    unsigned char  ucLineFeedCharacter[MODBUS_SERIAL_INTERFACES];        // commandable line feed character
    #endif
    #if defined USE_MODBUS_MASTER
    DELAY_LIMIT    serial_master_timeout[MODBUS_SERIAL_INTERFACES];      // longest wait for answer by serial master
    DELAY_LIMIT    serial_master_broadcast_timeout[MODBUS_SERIAL_INTERFACES];      
    #endif
    #if defined MODBUS_SUPPORT_SERIAL_LINE_FUNCTIONS && !defined NO_SLAVE_MODBUS_REPORT_SLAVE_ID
    CHAR           cSlaveID[SLAVE_ID_LENGTH];                            // slave ID (name)
    #endif
#endif
#if defined MODBUS_TCP                                                   // MODBUS slaves/gateways
    #if MODBUS_TCP_SERVERS > 0                                           // {V1.10}
    unsigned char  ucMODBUS_TCP_server_mode[MODBUS_TCP_SERVERS];         // MODBUS TCP server mode
    unsigned short usMODBUS_TCPport[MODBUS_TCP_SERVERS];                 // MODBUS TCP ports
    unsigned short usIdleTimeoutSlave[MODBUS_TCP_SERVERS];               // TCP connection idle timeout
    #endif
    #if MODBUS_TCP_MASTERS > 0
    unsigned char  ucSlaveIP[MODBUS_TCP_MASTERS][IPV4_LENGTH];           // IP address of TCP slave
    unsigned short usSlavePort[MODBUS_TCP_MASTERS];                      // port number of TCP slave
    unsigned short usIdleTimeoutMaster[MODBUS_TCP_MASTERS];              // TCP connection idle timeout
    DELAY_LIMIT    tcp_master_timeout[MODBUS_TCP_MASTERS];               // longest wait for answer by TCP master {V1.04}
    DELAY_LIMIT    tcp_master_broadcast_timeout[MODBUS_TCP_MASTERS];     // {V1.04}
    #endif
    #if defined MODBUS_GATE_WAY_QUEUE
    QUEUE_TRANSFER tcp_queue_size[MODBUS_TCP_SERVERS + MODBUS_TCP_MASTERS];
    #endif
#endif
} MODBUS_PARS;

typedef struct stMODBUS_RANGE
{
    unsigned short usStartAddress;
    unsigned short usEndAddress;
} MODBUS_RANGE;

typedef struct stMODBUS_READ_QUANTITY
{
    unsigned short usStartAddress;
    unsigned short usQuantity;
} MODBUS_READ_QUANTITY;


typedef struct stMODBUS_COIL_STATE
{
    unsigned short usAddress;
    unsigned char  ucState;
} MODBUS_COIL_STATE;

typedef struct stMODBUS_SINGLE_REGISTER
{
    unsigned short usAddress;
    unsigned short usValue;
} MODBUS_SINGLE_REGISTER;

typedef struct stMODBUS_SINGLE_REGISTER_MASK
{
    unsigned short usAddress;
    unsigned short usValueAND;
    unsigned short usValueOR;
} MODBUS_SINGLE_REGISTER_MASK;

typedef struct stMODBUS_DATA_CONTENT
{
    unsigned char  *user_data;
    unsigned short  usUserDataLength;
} MODBUS_DATA_CONTENT;

typedef struct stMODBUS_BITS
{
    MODBUS_BITS_ELEMENT *ptrBits;
    MODBUS_RANGE         address_range;
} MODBUS_BITS;

typedef struct stMODBUS_REGISTERS
{
    unsigned short *ptrRegisters;
    MODBUS_RANGE  address_range;
} MODBUS_REGISTERS;

typedef struct stMODBUS_READ_WRITE_REGISTERS
{
    MODBUS_READ_QUANTITY reg_read;
    MODBUS_REGISTERS     reg_write;
} MODBUS_READ_WRITE_REGISTERS;

typedef struct stFIFO_ADDRESS
{
    unsigned short usFIFO_address;
} FIFO_ADDRESS;

// MODBUS configuration
//
typedef struct stMODBUS_CONFIG
{
    MODBUS_BITS      *modbus_discrete_inputs;                            // read-only discrete input configuration
    MODBUS_BITS      *modbus_coils;                                      // read/write coil configuration
    MODBUS_REGISTERS *modbus_input_regs;                                 // read-only input registers
    MODBUS_REGISTERS *modbus_holding_regs;                               // read/write input registers
} MODBUS_CONFIG;

typedef struct stMODBUS_QUEUE
{
    QUEUE_HANDLE   queue_handle;                                         // queue handle for this MODBUS queue
    unsigned char  ucOutstanding;                                        // the number of outstanding messages
} MODBUS_QUEUE;


#define _MODBUS_BITS_BYTE_SIZE(quantity)    ((quantity + 7)/8)           // the amount of elements needed to hold the defined bit quantity
#define _MODBUS_BITS_ELEMENT_SIZE(quantity) ((quantity + (MODBUS_BITS_ELEMENT_WIDTH - 1))/MODBUS_BITS_ELEMENT_WIDTH)

// Global MODBUS variables
//
extern MODBUS_PARS *ptrMODBUS_pars;                                      // the MODBUS application needs to supply this pointer




#define BROADCAST_MODBUS_ADDRESS             0
#define MIN_SLAVE_MODBUS_ADDRESS             1
#define MAX_SLAVE_MODBUS_ADDRESS             247

#define ADD_AND_FUNCTION                     2
#define MODBUS_CRC_LENGTH                    2

#define MODBUS_RX_ASCII_SIZE                 513
#define MODBUS_RX_BUFFER_SIZE                256
#define MODBUS_TX_ASCII_SIZE                 513
#define MODBUS_TX_BUFFER_SIZE                256 

#define MODBUS_SERIAL_PORT                   0x00
#define MODBUS_MODE_RTU                      0x00
#define MODBUS_MODE_ASCII                    0x01
#define MODBUS_SERIAL_SLAVE                  0x02
#define MODBUS_SERIAL_GATEWAY                0x04
#define MODBUS_SERIAL_MASTER                 0x08
#define MODBUS_RS485_NEGATIVE                0x10
#define MODBUS_RS485_POSITIVE                0x20
#define MODBUS_TCP_SLAVE_PORT                0x20
#define MODBUS_TCP_MASTER_PORT               0x40
#define MODBUS_TCP_SERIAL_GATEWAY            0x80


#define MODBUS_START_BYTE                    ':'                         // 0x3a
#define MODBUS_LINE_FEED                     0x0a
#define MODBUS_CARRIAGE_RETURN               0x0d

// Function code descriptions
//
#define MODBUS_TRANSFER                      0x8000                      // fictive extended functional code to instruct a transfer
#define MODBUS_TRANSFER_TCP_UART             0x9000                      // fictive extended functional code to instruct a transfer from TCP to UART
#define MODBUS_TRANSFER_SERIAL               0xa000                      // fictive extended functional code to instruct a transfer from UART to UART
#define MODBUS_READ_COILS                    0x01
#define MODBUS_READ_DISCRETE_INPUTS          0x02
#define MODBUS_READ_HOLDING_REGISTERS        0x03
#define MODBUS_READ_INPUT_REGISTERS          0x04
#define MODBUS_WRITE_SINGLE_COIL             0x05
#define MODBUS_WRITE_SINGLE_REGISTER         0x06
#define MODBUS_READ_EXCEPTION_STATUS         0x07                        // serial line only
#define MODBUS_DIAGNOSTICS                   0x08                        // serial line only
    #define MODBUS_DIAG_SUB_RETURN_QUERY_DATA    0x0000                  // return query data
    #define MODBUS_DIAG_SUB_RESTART_COMS         0x0001                  // restart communications option
        #define RESTART_LEAVE_LOG                0x0000
        #define RESTART_CLEAR_LOG                0xff00
    #define MODBUS_DIAG_SUB_RETURN_DIAG_REG      0x0002                  // return diagnostics register
    #define MODBUS_DIAG_SUB_CHANGE_ASCII_DELIM   0x0003                  // change ASCII input delimiter
    #define MODBUS_DIAG_SUB_FORCE_LISTEN_ONLY    0x0004                  // force listen only mode
    #define MODBUS_DIAG_SUB_CLEAR_DISGNOSTICS    0x000a                  // clear counters and disgnostics register
    #define MODBUS_DIAG_SUB_RTN_BUS_MSG_CNT      0x000b                  // return bus message count
    #define MODBUS_DIAG_SUB_RTN_BUS_COM_ERR_CNT  0x000c                  // return bus communication error count
    #define MODBUS_DIAG_SUB_RTN_BUS_EXC_ERR_CNT  0x000d                  // return bus exception error count
    #define MODBUS_DIAG_SUB_RTN_SLAVE_MSG_CNT    0x000e                  // return slave message count
    #define MODBUS_DIAG_SUB_RTN_SLAVE_NO_RSP_CNT 0x000f                  // return slave no response count
    #define MODBUS_DIAG_SUB_RTN_SLAVE_NAK_CNT    0x0010                  // return slave NAK count
    #define MODBUS_DIAG_SUB_RTN_SLAVE_BSY_CNT    0x0011                  // return slave busy count
    #define MODBUS_DIAG_SUB_RTN_BUS_CHR_ORUN_CNT 0x0012                  // return bus character overrun count
    #define MODBUS_DIAG_SUB_CLEAR_ORUN_AND_FLG   0x0014                  // clear overrun counter and flag
#define MODBUS_GET_COMM_EVENT_COUNTER        0x0b                        // serial line only
#define MODBUS_GET_COMM_EVENT_LOG            0x0c                        // serial line only
#define MODBUS_WRITE_MULTIPLE_COILS          0x0f
#define MODBUS_WRITE_MULTIPLE_REGISTERS      0x10
#define MODBUS_REPORT_SLAVE_ID               0x11                        // serial line only
#define MODBUS_READ_FILE_RECORD              0x14
#define MODBUS_WRITE_FILE_RECORD             0x15
#define MODBUS_MASK_WRITE_REGISTER           0x16
#define MODBUS_READ_WRITE_MULTIPLE_REGISTER  0x17
#define MODBUS_READ_FIFO_QUEUE               0x18
#define MODBUS_ENCAPSUL_INTERFACE_TRANSPORT  0x2b
    #define MODBUS_ENCAP_CANOPEN_PDU             0x0d
    #define MODBUS_ENCAP_READ_DEVICE_IDENTIFIER  0x0e
#define USER_DEFINED_FUNCTION_CODES_START_1  0x41
#define USER_DEFINED_FUNCTION_CODES_END_1    0x48
#define USER_DEFINED_FUNCTION_CODES_START_2  0x64
#define USER_DEFINED_FUNCTION_CODES_END_2    0x6e


typedef struct stMODBUS_DIAGNOSTICS_REQUEST
{
    unsigned short usSubFunction;                                        // sub-function code
    unsigned short usData;
} MODBUS_DIAGNOSTICS_REQUEST;

typedef struct stDIAGNOSTIC_COUNTERS
{
    unsigned short usMessageCounter;                                     // total number of messages detected on the bus
    unsigned short usCRC_errorCounter;                                   // total number of CRC errors counted
    unsigned short usException_errorCounter;                             // total number of exceptions sent
    unsigned short usSlaveMessageCounter;                                // total number of messages addressing the slave
    unsigned short usNoResponseCounter;                                  // total number of messages which were not responded to
    unsigned short usNAKCounter;                                         // total number of NAKs sent
    unsigned short usSlaveBusyCounter;                                   // total number of Slave Busy Exception responses sent
    unsigned short usOverrunCounter;                                     // total number of receiver overruns experienced
} DIAGNOSTIC_COUNTERS;

// MODBUS exception codes
//
#define MODBUS_EXCEPTION_ILLEGAL_FUNCTION    0x01
#define MODBUS_EXCEPTION_ILLEGAL_DATA_ADD    0x02
#define MODBUS_EXCEPTION_ILLEGAL_DATA_VALUE  0x03
#define MODBUS_EXCEPTION_SLAVE_DEV_FAILURE   0x04
#define MODBUS_EXCEPTION_ACKNOWLEDGE         0x05
#define MODBUS_EXCEPTION_SLAVE_DEVICE_BUSY   0x06
#define MODBUS_EXCEPTION_MEMORY_PARITY_ERROR 0x07
#define MODBUS_EXCEPTION_GATEWAY_PATH_UNAV   0x0a
#define MODBUS_EXCEPTION_GATEWAY_DEV_NO_RESP 0x0b

#define MODBUS_EXCEPTION_CODE_1              MODBUS_EXCEPTION_ILLEGAL_FUNCTION
#define MODBUS_EXCEPTION_CODE_2              MODBUS_EXCEPTION_ILLEGAL_DATA_ADD
#define MODBUS_EXCEPTION_CODE_3              MODBUS_EXCEPTION_ILLEGAL_DATA_VALUE
#define MODBUS_EXCEPTION_CODE_4              MODBUS_EXCEPTION_SLAVE_DEV_FAILURE


#define MINIMUM_COILS                        1
#define MAXIMUM_COILS                        2000
#define MINIMUM_REGISTERS                    1
#define MAXIMUM_REGISTERS                    125
#define MAXIMUM_MULTI_REGISTERS              123
#define MAXIMUM_MULTI_REGISTERS_WITH_READ    121
#define MAX_FIFO_COUNT                       31




// MODBUS TCP
//
#define MODBUS_TCP_PORT                       502

#define MODBUS_TCP_DATA_CONTENT               253

__PACK_ON                                                                // compilers using pragmas to control packing will start struct packing from here
// MODBUS configuration
//
typedef struct _PACK stMODBUS_TCP_HEADER
{
    unsigned char ucTransactionIdentifier[2];                            // identification of a MODBUS request / response transaction
    unsigned char ucProtocolIdentifier[2];                               // MODBUS protocol = 0
    unsigned char ucLength[2];                                           // number of following bytes
    unsigned char ucUnitIdentifier;                                      // identification of a remote slave connected on a serial line or on other buses
} MODBUS_TCP_HEADER;

typedef struct _PACK stMODBUS_TCP_FRAME
{
    MODBUS_TCP_HEADER modbus_header;
    unsigned char ucData[MODBUS_TCP_DATA_CONTENT];
} MODBUS_TCP_FRAME;

typedef struct _PACK stMODBUS_TCP_TX_FRAME
{
    TCP_HEADER     tTCP_Header;
    MODBUS_TCP_HEADER modbus_header;
    unsigned char ucData[MODBUS_TCP_DATA_CONTENT];
} MODBUS_TCP_TX_FRAME;
__PACK_OFF

#define MODBUS_PROTOCOL_ID                          0x0000

#define MODBUS_TCP_NON_SIGNIFICANT_UNIT_IDENTIFIER  0xff


#define MODBUS_TCP_MODE_AUTOMATIC                   0x00


typedef struct stMODBUS_TCP_CTL
{
    MODBUS_TCP_TX_FRAME *tx_modbus_frame;
    MODBUS_TCP_FRAME *ptrMODBUS_input_frame;
    unsigned short usOutstandingData;
    USOCKET OwnerTCPSocket; 
    unsigned char ucState;
    unsigned char ucMode;
    unsigned char ucPort;
    unsigned char ucSerialPort;
} MODBUS_TCP_CTL;

#define NO_MODBUS_GATEWAY                           0xff

typedef struct stMODBUS_RX_FUNCTION
{
#if defined MODBUS_TCP
    MODBUS_TCP_CTL *modbus_session;                                      // TCP session control information
#endif
    void *ptrElementContent;
	MODBUS_DATA_CONTENT data_content; 
    unsigned short usElementAddress;                                     // function element address
    unsigned short usElementLength;                                      // function element length
    unsigned short usReference;                                          // unique reception frame reference number
    unsigned char ucMODBUSport;                                          // MODBUS serial port number
#if defined USE_MODBUS_SLAVE && (MODBUS_SHARED_SERIAL_INTERFACES > 0 || MODBUS_SHARED_TCP_INTERFACES > 0)
    unsigned char ucMODBUS_Slaveport;                                    // slave reference for shared port
#endif
#if defined USE_MODBUS_SLAVE && defined MODBUS_SUPPORT_SERIAL_LINE_DIAGNOSTICS && MODBUS_SHARED_SERIAL_INTERFACES > 0
    unsigned char ucMODBUS_Diagnostics_SlavePort;
#endif
    unsigned char ucFunctionCode;                                        // function code being handled
    unsigned char ucSourceAddress;                                       // function code being handled
    unsigned char ucSourceType;                                          // source reference type
    unsigned char ucMappedAddress;                                       // address to map gatewayed message to
#if defined MODBUS_DELAYED_RESPONSE
    unsigned char ucDelayed;                                             // mark that a response has been delayed
#endif
} MODBUS_RX_FUNCTION;

#define MODBUS_STATE_FREE                                    0
#define MODBUS_STATE_RESERVED                                1
#define MODBUS_STATE_ACTIVE                                  2

#define USER_RECEIVING_ALL_MODBUS_DATA                       0
#define USER_RECEIVING_ALL_MODBUS_TYPE                       1
#define USER_RECEIVING_MISSED_RANGE                          2
#define USER_RECEIVING_MODBUS_UNDEFINED_FUNCTION             3

// Gateway routing
//
typedef struct stMODBUS_ROUTE
{
    unsigned char ucMODBUSPort;                                          // default route
    unsigned char ucNextRange;                                           // 0 ends list, otherwise start of next slave address range
} MODBUS_ROUTE;

typedef struct stROUTE_ENTRY
{
#if defined MODBUS_TCP
    MODBUS_TCP_CTL *modbus_session;
    unsigned char ucID[2];
#endif
    unsigned char Valid;                                                 // entry valid
    unsigned char ucSourceMODBUSport;                                    // source MODBUS port
    unsigned char ucDestinationMODBUSport;                               // destination MODBUS port
    unsigned char ucMappedAddress;                                       // mapped slave address
} ROUTE_ENTRY;


#define NO_ROUTE_INFO                         0x00
#define TCP_ROUTE_INFO                        0x01
#define TCP_ROUTE_INFO_WITH_MAPPED_ADDRESS    0x02
#define SERIAL_ROUTE_INFO                     0x04                       // {V1.07}

// Delayed Response list
//
typedef struct stMODBUS_DELAY_LIST
{
#if defined MODBUS_TCP
    MODBUS_TCP_CTL *modbus_session;                                      // the TCP session that the request belongs to
    unsigned char  ucID[2];                                              // the MODBUS TCP transaction identifier of the request
#endif
    unsigned short usValidLength;                                        // length of request to be delayed. Zero represents an empty entry
    unsigned short usDelayedReference;                                   // a unique message reference number for matching delayed request continuations
    unsigned char  ucMODBUSport;                                         // the MODBUS port on which the request belongs to
    unsigned char  ucAddress;                                            // slave address
    unsigned char  ucFunction;                                           // function code
#if defined USE_MODBUS_MASTER && defined MODBUS_DELAYED_RESPONSE         // {V1.14}
    unsigned char  ucRequest[MAX_QUEUED_REQUEST_LENGTH];                 // request content to be repeated once the application has prepared content
#endif
} MODBUS_DELAY_LIST;

// Optional shared MODBUS slave ports
//
typedef struct stSHARED_MODBUS_PORT
{
    unsigned char ucMODBUSPort;                                          // the port that the serial interface is shared on
    unsigned char ucSlaveAdress;                                         // the slave address
} SHARED_MODBUS_PORT;


// MODBUS interface callbacks
//
typedef struct stMODBUS_CALLBACKS
{
    int ( *pre_function_call  )( int iType, MODBUS_RX_FUNCTION * );      // called to prepare for read
    int ( *post_function_call )( int iType, MODBUS_RX_FUNCTION * );      // called to process after a write
    int ( *user_function_call )( int iType, MODBUS_RX_FUNCTION *);       // called to process a user defined function
} MODBUS_CALLBACKS;

#define MODBUS_APP_FUNCTION_ERROR   -1                                   // function can not be handled
#define MODBUS_APP_DELAYED_RESPONSE -2                                   // application needs to to prepare response and will inform later
#define MODBUS_APP_GATEWAY_FUNCTION -3

#define TCP_ROUTE_FROM_SLAVE         1
#define MODBUS_SLAVE_RESPONSE        2
#define SERIAL_ROUTE_FROM_SLAVE      3

extern void fnRetrigger_T1_5_monitor(QUEUE_HANDLE Channel);

extern int fnInitialiseMODBUS_port(unsigned char ucMODBUSport, const MODBUS_CONFIG *ptrMODBUS_config, const MODBUS_CALLBACKS *ptrModbus_callbacks, int ( *master_callback )( int iType, MODBUS_RX_FUNCTION *));
  #define MODBUS_TCP_NO_HEAP_MEMORY           -20                        // return values
  #define MODBUS_TCP_LISTENER_INSTALL_FAILED  -21
  #define MODBUS_TCP_PORTS_USED               -22
extern int fnShareMODBUS_port(unsigned char ucMODBUSport, const MODBUS_CONFIG *ptrMODBUS_config, const MODBUS_CALLBACKS *ptrModbus_callbacks, unsigned char ucSlaveAddress);
  #define MODBUS_MAXIMUM_SHARES_EXCEEDED        -1
extern int fnMODBUS_Master_send(unsigned char ucInterface, unsigned char ucSlave, unsigned short usFunction, void *details);
  #define MODBUS_TX_MESSAGE_QUEUED               1
  #define MODBUS_TX_MESSAGE_LOST_NO_QUEUE_SPACE -1
  #define MODBUS_TX_MESSAGE_LOST_QUEUE_REQUIRED -2
extern int fnMODBUS_transmit(MODBUS_RX_FUNCTION *modbus_rx_function, unsigned char *ptrData, unsigned short usDataLength);
  #define RTU_SERIAL_INPUT                     0x00                      // source types
  #define ASCII_SERIAL_INPUT                   0x01
  #define TCP_BINARY_INPUT                     0x02
  #define INTERMEDIATE_STEP_WITHOUT_RESPONSE   0x04
  #define ADJUST_READ_WRITE_MULTIPLE_REGISTER  0x08                      // {V1.09} special case where MODBUS_READ_HOLDING_REGISTERS is being used for MODBUS_READ_WRITE_MULTIPLE_REGISTER

  #define NO_MODBUS_SOCKET_TO_SEND             -1                        // return values
  #define MODBUS_TCP_ESTABLISHING_CONNECTION   -2                        // no data sent yet but connection being established
  #define MODBUS_BROADCAST_TRANSMITTED         -3                        // broadcast being sent so don't start a monitoring timer

extern void fnMODBUS_event_report(int iEvent, unsigned char ucInterface);
  #define MODBUS_CONNECTION_ESTABLISHED        0
  #define MODBUS_NO_SLAVE_RESPONSE             -1
  #define MODBUS_BROADCAST_TIMEOUT             -2
  #define MODBUS_CONNECTION_ABORTED            -3
  #define MODBUS_CONNECTION_CLOSED             -4
  #define MODBUS_TCP_REPETITION                -5

extern int fnClose_MODBUS_port(unsigned char ucMODBUSport);              // {V1.07}
extern MODBUS_TCP_CTL *fnGet_MODBUS_TCP_port_status(unsigned char ucMODBUSport); // {V1.07}

extern int fnMODBUS_route(int iType, MODBUS_RX_FUNCTION *modbus_rx_function, MODBUS_ROUTE *routing_table);

#define PREPARE_COILS                          0
#define PREPARE_DISCRETE_INPUTS                1
#define PREPARE_HOLDING_REGISTERS              2
#define PREPARE_INPUT_REGISTERS                3
#define PREPARE_FIFO_READ                      4
#define DO_FIFO_READ                           5
#define GET_OPERATING_STATE                    6
#define GET_EXCEPTION_STATUS                   7 
#define GET_STATUS_WORD                        8
#define PREPARE_FIFO_READ_DELAYED              9                         // same as PREPARE_FIFO_READ but as a result of a delayed response


#define UPDATE_COILS                           0
#define UPDATE__HOLDING_REGISTERS              1

#define MODBUS_DEVICE_RUNNING                  0xff
#define MODBUS_DEVICE_IDLE                     0x00

extern void fnMODBUS_delayed_response(unsigned short usDelayedReference);



