/***********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher

    ---------------------------------------------------------------------
    File:      modbus_app.c
    Project:   uTasker project
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2016
    *********************************************************************
    02.09.2009 Adjust gateway parameter to respect present use           // {1}
    07.11.2011 Don't use fnMODBUS_Master_send() when there is no master functionality {2}

    This example of a MODBUS application is based on a configuration with
    two UARTs (one master and one slave) plus two TCP slaves and two TCP
    masters.

    Various public functions are tested by either the serial or a TCP master
    by sending request and then waiting of a response. On a response, exception
    or timeout the next function will be sent until all test functions have
    been completed and the test restarts.

    By activating various test defines special conditions can be tested.

    The serial test runs in the uTasker simulator when the serial master
    and slave are connected (eg. using a com0com loop-back).

    TCP -> serial gateway tests are possible to include modbus TCP in the
    process, whereby the simulator can also be used to achieve full tests
    (activate PSEUDO_LOOPBACK in config.h to allow a TCP master to transmit
    a TCP slave on its own IP address). The test path is then -
    Request: TCP master -> TCP slave gateway -> serial master -> serial slave
    Response: serial slave -> serial master ->TCP slave gateway -> TCP master

    The MODBUS traffic can simply be monitored using wireshark, where the
    MODBUS content is conveniently interpreted.

*/


/* =================================================================== */
/*                           include files                             */
/* =================================================================== */

#include "config.h"


#if defined USE_MODBUS
/* =================================================================== */
/*                          local definitions                          */
/* =================================================================== */

//#define TEST_QUEUE                                                     // activate to test master queues
//#define TEST_DELAYED_RESPONSE                                          // activate to test delayed responses

/* =================================================================== */
/*                      local structure definitions                    */
/* =================================================================== */

/* =================================================================== */
/*                 local function prototype declarations               */
/* =================================================================== */

static void          fnGetMODBUS_parameters(void);
static int           fnMODBUSPreFunction (int iType, MODBUS_RX_FUNCTION *modbus_rx_function);
static int           fnMODBUSPostFunction(int iType, MODBUS_RX_FUNCTION *modbus_rx_function);
static int           fnMODBUSuserFunction(int iType, MODBUS_RX_FUNCTION *modbus_rx_function);
#if defined USE_MODBUS_MASTER
    static void      fnModbusTest(unsigned char);
    static int       fnMODBUSmaster(int iType, MODBUS_RX_FUNCTION *modbus_rx_function);
#else
    #define fnMODBUSmaster 0
#endif

/* =================================================================== */
/*                             constants                               */
/* =================================================================== */


static const MODBUS_PARS cMODBUS_default = {
    MODBUS_PAR_BLOCK_VERSION,
#if defined MODBUS_SERIAL_INTERFACES && defined SERIAL_INTERFACE
    {
    #if MODBUS_SERIAL_INTERFACES > 1
        (0),                                                             // master address not relevant - serial port 0
    #endif
        (33),                                                            // slave address - serial port 1
    },
    {
    #if MODBUS_SERIAL_INTERFACES > 1
        (CHAR_8 + NO_PARITY + ONE_STOP + CHAR_MODE),                     // serial interface settings (ASCII mode uses 7 bits) - serial port 0
    #endif
        (CHAR_8 + NO_PARITY + ONE_STOP + CHAR_MODE),                     // serial interface settings (ASCII mode uses 7 bits) - serial port 1
    },
    {
    #if MODBUS_SERIAL_INTERFACES > 1
        SERIAL_BAUD_19200,                                               // baud rate of serial interface - serial port 0
    #endif
        SERIAL_BAUD_19200,                                               // baud rate of serial interface - serial port 1
    },
    {
    #if MODBUS_SERIAL_INTERFACES > 1
        (MODBUS_MODE_ASCII | MODBUS_SERIAL_MASTER | MODBUS_RS485_POSITIVE /*MODBUS_RS485_NEGATIVE*/),// default to RTU mode as master - serial port 0
    #endif
        (MODBUS_MODE_ASCII | MODBUS_SERIAL_SLAVE | /*MODBUS_SERIAL_GATEWAY | */MODBUS_RS485_POSITIVE), // default to RTU mode as slave - serial port 1
    },
    #if defined MODBUS_GATE_WAY_QUEUE
    {
        #if MODBUS_SERIAL_INTERFACES > 1
        1024,                                                            // buffer size for queuing waiting messages on MODBUS serial port 0
        #endif
        0,                                                               // slave doesn't need queue MODBUS serial port 1
    },
    #endif
    #if defined MODBUS_ASCII
    {
        #if MODBUS_SERIAL_INTERFACES > 1
        (DELAY_LIMIT)(1 * SEC),                                          // inter-character delays greater than 1s are considered errors in ASCII mode - serial port 0
        #endif
        (DELAY_LIMIT)(2 * SEC),                                          // inter-character delays greater than 2s are considered errors in ASCII mode - serial port 1
    },
    {
        #if MODBUS_SERIAL_INTERFACES > 1
        0x0a,                                                            // ASCII mode line feed character - serial port 0
        #endif
        0x0a,                                                            // ASCII mode line feed character - serial port 1
    },
    #endif
    #if defined USE_MODBUS_MASTER
    {
        #if MODBUS_SERIAL_INTERFACES > 1
        (DELAY_LIMIT)(2 * SEC),                                          // MODBUS master maximum wait for a response from a slave - serial port 0
        #endif
        0,                                                               // no timeout for a slave - serial port 1
    },
    {
        #if MODBUS_SERIAL_INTERFACES > 1
        (DELAY_LIMIT)(0.2 * SEC),                                        // MODBUS master broadcast timeout - serial port 0
        #endif
        0,                                                               // no timeout for a slave - serial port 1
    },

    #endif
    #if defined MODBUS_SUPPORT_SERIAL_LINE_FUNCTIONS && !defined NO_SLAVE_MODBUS_REPORT_SLAVE_ID
    { 'u', 'T', 'a', 's', 'k', 'e', 'r', '-', 'M', 'O', 'D', 'B', 'U', 'S', '-', 's', 'l','a','v','e' },
    #endif
#endif
#if defined MODBUS_TCP
    #if defined MODBUS_TCP_SERVERS                                       // slaves
    {
        (/*MODBUS_TCP_SERIAL_GATEWAY | */MODBUS_TCP_SLAVE_PORT),         // MODBUS listener mode - TCP slave port 2
        MODBUS_TCP_SLAVE_PORT,                                           // MODBUS listener mode - TCP slave port 3
    },
    {
        MODBUS_TCP_PORT,                                                 // MODBUS listener port number - standard - TCP slave port 2
        4321,                                                            // MODBUS listener port number - user defined - TCP slave port 3
    },
    {
        (2 * 60),                                                        // MODBUS listener port number - 2 minute TCP idle timeout - TCP slave port 2
        INFINITE_TIMEOUT,                                                // MODBUS listener port number - no TCP connection timeout - TCP slave port 3
    },
    #endif
    #if MODBUS_TCP_MASTERS > 0
    {
        #if defined _WINDOWS && defined PSEUDO_LOOPBACK                  // test environment
        {192, 168, 0, 3},                                                // IP address of TCP slave - TCP master port 4
        {192, 168, 0, 4},                                                // IP address of TCP slave - TCP master port 5
        #else
        {192, 168, 0, 4},                                                // IP address of TCP slave - TCP master port 4
        {192, 168, 0, 5},                                                // IP address of TCP slave - TCP master port 5
        #endif
    },
    {
        MODBUS_TCP_PORT,                                                 // port number of TCP slave - TCP master port 4
        4321,                                                            // port number of TCP slave - TCP master port 5
    },
    {
        (2 * 60),                                                        // MODBUS master - 2 minute TCP idle timeout - TCP master port 4
        INFINITE_TIMEOUT,                                                // MODBUS master - no TCP connection timeout - TCP master port 5
    },
    {
        (DELAY_LIMIT)(2 * SEC),                                          // MODBUS master maximum wait for a response from a slave - TCP master port 4
        (DELAY_LIMIT)(3 * SEC),                                          // MODBUS master maximum wait for a response from a slave - TCP master port 5
    },
    {
        (DELAY_LIMIT)(0.2 * SEC),                                        // MODBUS master broadcast timeout - TCP master port 4
        (DELAY_LIMIT)(0.2 * SEC),                                        // MODBUS master broadcast timeout - TCP master port 5
    },
    #endif
    #if defined MODBUS_GATE_WAY_QUEUE
    {
        512,                                                             // buffer size for queuing waiting messages on MODBUS tcp port 2
        256,                                                             // buffer size for queuing waiting messages on MODBUS tcp port 3
        128,                                                             // buffer size for queuing waiting messages on MODBUS tcp port 4
        64,                                                              // buffer size for queuing waiting messages on MODBUS tcp port 5
    },
    #endif
#endif
};

#if defined USE_MODBUS_MASTER
    #define TEST_SLAVE_ADDRESS    33                                     // used by demo when testing master functionality
  //#define TEST_SLAVE_ADDRESS MODBUS_TCP_NON_SIGNIFICANT_UNIT_IDENTIFIER // TCP slave address
  //#define TEST_SLAVE_ADDRESS BROADCAST_MODBUS_ADDRESS                  // broadcast
#endif

#define DISCRETES_START       15                                         // start address
#define DISCRETES_END         43                                         // end address
#define DISCRETES_QUANTITY    ((DISCRETES_END - DISCRETES_START) + 1)    // quantity
static MODBUS_BITS_ELEMENT discretes[_MODBUS_BITS_ELEMENT_SIZE(DISCRETES_QUANTITY)] = {0};
static MODBUS_BITS test_discretes = { discretes, {DISCRETES_START, DISCRETES_END}};

#define COILS_START           10                                         // start address
#define COILS_END             39                                         // end address
#define COILS_QUANTITY        ((COILS_END - COILS_START) + 1)            // quantity
static MODBUS_BITS_ELEMENT coils[_MODBUS_BITS_ELEMENT_SIZE(COILS_QUANTITY)] = {0};
static MODBUS_BITS test_coils = { coils, {COILS_START, COILS_END}};

#define INPUT_REGS_START      24                                         // start address
#define INPUT_REGS_END        29                                         // end address
#define INPUT_REGS_QUANTITY   ((INPUT_REGS_END - INPUT_REGS_START) + 1)  // quantity
static unsigned short input_regs[INPUT_REGS_QUANTITY] = {0};
static MODBUS_REGISTERS test_input_regs = { input_regs, {INPUT_REGS_START, INPUT_REGS_END}};

#define HOLDING_REGS_START    2                                          // start address
#define HOLDING_REGS_END      6                                          // end address
#define HOLDING_REGS_QUANTITY ((HOLDING_REGS_END - HOLDING_REGS_START) + 1) // quantity
static unsigned short holding_regs[HOLDING_REGS_QUANTITY] = {0};
static MODBUS_REGISTERS test_holding_regs = { holding_regs, {HOLDING_REGS_START, HOLDING_REGS_END}};

static const MODBUS_CONFIG modbus_configuration = {
    &test_discretes,                                                     // read-only discrete input configuration
    &test_coils,                                                         // read/write coil configuration
    &test_input_regs,                                                    // read-only input registers
    &test_holding_regs,                                                  // read/write input registers
};

static const MODBUS_CONFIG modbus_partial_configuration = {
    &test_discretes,                                                     // read-only discrete input configuration
    &test_coils,                                                         // read/write coil configuration
    &test_input_regs,                                                    // read-only input registers
    &test_holding_regs,                                                  // read/write input registers
};

static const MODBUS_CALLBACKS modbus_slave_callbacks = {
    fnMODBUSPreFunction,
    fnMODBUSPostFunction,
    fnMODBUSuserFunction,
};

#if defined USE_MODBUS_MASTER && defined MODBUS_GATE_WAY_ROUTING
static const MODBUS_ROUTE routing_table_1[] = {
    {0, 0x80},                                                           // all slave addresses up to 0x80 are routed to MODBUS port 0
    {5, 0xff}                                                            // any others are routes to MODBUS port 5
};
#endif



/* =================================================================== */
/*                     global variable definitions                     */
/* =================================================================== */

MODBUS_PARS *ptrMODBUS_pars = 0;                                         // these parameters need to be supplied by the MODBUS application
#if defined MODBUS_DELAYED_RESPONSE
    unsigned short usDelayedReference = 0;
#endif

/* =================================================================== */
/*                      local variable definitions                     */
/* =================================================================== */




// Load either a temporary configuration, or committed configuration. If none found, load default settings
//
static void fnGetMODBUS_parameters(void)
{
    if (ptrMODBUS_pars == 0) {
        ptrMODBUS_pars = uMalloc(sizeof(MODBUS_PARS));
    }
    if (fnGetPar((PAR_MODBUS | TEMPORARY_PARAM_SET), (unsigned char *)ptrMODBUS_pars, sizeof(MODBUS_PARS)) < 0) {
        if ((fnGetPar(PAR_MODBUS, (unsigned char *)ptrMODBUS_pars, sizeof(MODBUS_PARS)) < 0) || (MODBUS_PAR_BLOCK_VERSION != ptrMODBUS_pars->ucModbusParVersion)) {
            uMemcpy(ptrMODBUS_pars, (unsigned char *)&cMODBUS_default, sizeof(MODBUS_PARS)); // no valid parameters available - set defaults
        }
    }
    uMemcpy(&temp_pars->modbus_parameters, ptrMODBUS_pars, sizeof(MODBUS_PARS)); // copy the working parameters to a temporary set
}

// This routine is called by the MODBUS interface prior to a read
// It can be used to update the MODBUS tables if required
//
static int fnMODBUSPreFunction(int iType, MODBUS_RX_FUNCTION *modbus_rx_function)
{
    unsigned short usAddress = modbus_rx_function->usElementAddress;
    switch (iType) {
    case PREPARE_COILS:                                                  // coils are being read - update table values if necessary
        usAddress -= COILS_START;                                        // reference to start of coil region
        coils[0] ^= 0xf0;                                                // toggle some bits on each request
#if defined TEST_DELAYED_RESPONSE
        usDelayedReference = modbus_rx_function->usReference;            // save the request reference for delayed handling
        uTaskerMonoTimer(TASK_APPLICATION, (DELAY_LIMIT)(1 * SEC), E_TEST_MODBUS_DELAY);
        return MODBUS_APP_DELAYED_RESPONSE;                              // test delayed response
#else
        if (modbus_rx_function->ucSourceAddress != 33) {
            modbus_rx_function->ucMappedAddress = 33;                    // map the receive address to a new destination address
            return (MODBUS_APP_GATEWAY_FUNCTION - 0);                    // decide to gateway this to a different port - MODBUS port 0 in this case
        }
#endif
        break;
    case PREPARE_DISCRETE_INPUTS:                                        // discrete inputs are being read - update table values if necessary
        discretes[0] ^= 0xa3;                                            // toggle test discretes
        break;
    case PREPARE_HOLDING_REGISTERS:                                      // holding registers are being read - update table values if necessary
        holding_regs[0] += 0x0010;
        holding_regs[1] -= 0x0010;
        holding_regs[2] += 0x0100;
        holding_regs[3] -= 0x0100;
        break;
    case PREPARE_INPUT_REGISTERS:                                        // input registers are being read - update table values if necessary
        input_regs[0] += 1;
        input_regs[1] -= 1;
        input_regs[2] ^= 0xffff;
        input_regs[3] ^= 0xff00;
        input_regs[4] ^= 0x00ff;
        break;

    case PREPARE_FIFO_READ:                                              // return the FIFO queue length (maximum 31)
        return 2;                                                        // test 2 byte FIFO queue (return 0 if not supported)
    case DO_FIFO_READ:                                                   // return FIFO content - incrementing FIFO read position on each call
        {
            static unsigned short usFifoContent = 0;
            return usFifoContent++;
        }
#if defined MODBUS_SUPPORT_SERIAL_LINE_FUNCTIONS                         // serial line function support
    case GET_OPERATING_STATE:
        return MODBUS_DEVICE_RUNNING;                                    // always signal running state
    case GET_EXCEPTION_STATUS:                                           // return 8 bits of status information if desired
        return 0x53;
    case GET_STATUS_WORD:                                                // if a previous program command is still being processed this can be returned, for example, as 0xffff
        return 0x1234;                                                   // return a status short word
#endif
    }
    return 0;
}

// Handle special coil controls
//
static void fnSetCoil(unsigned short usAddress, unsigned char ucState)
{
    switch (usAddress) {
    case 14:                                                             // map this coil to a relay output
        if (ucState != 0) {
          //RELAY1ON;
        }
        else {
          //RELAY1OFF;
        }
        break;
    }
}

// This routine is called by the MODBUS interface after a write to the MODBUS table
// It can be used to respond to table changes
//
static int fnMODBUSPostFunction(int iType, MODBUS_RX_FUNCTION *modbus_rx_function)
{
    unsigned short usAddress = modbus_rx_function->usElementAddress;
    unsigned short usLength = modbus_rx_function->usElementLength;
    switch (iType) {
    case UPDATE_COILS:                                                   // coils have been altered - react to changes if necessary
        {
    #if defined NOTIFY_ONLY_COIL_CHANGES
            fnSetCoil(usAddress, (unsigned char)usLength);
    #else
            MODBUS_BITS_ELEMENT coil_bit;
            MODBUS_BITS_ELEMENT coil_element;
            unsigned short usTableAddress = (usAddress - COILS_START);   // referenced to the coil table
            coil_bit = (0x01 << usTableAddress%MODBUS_BITS_ELEMENT_WIDTH); // the first coil bit
            coil_element = usTableAddress/MODBUS_BITS_ELEMENT_WIDTH;     // the first coil element location
            while (usLength != 0) {
                fnSetCoil(usAddress, (unsigned char)((coils[coil_element] & coil_bit) != 0));
                if (coil_bit & (0x1 << (MODBUS_BITS_ELEMENT_WIDTH - 1))) {
                    coil_bit = 0x01;
                    coil_element++;
                }
                else {
                    coil_bit <<= 1;
                }
                usAddress++;
                usLength--;
            }
    #endif
        }
        break;
    case UPDATE__HOLDING_REGISTERS:                                      // holding registers have been altered - react to changes if necessary
        break;
    }
    return 0;
}

// The user will receive functions which are defined in the range START_USER_CODE_BLOCK to END_USER_CODE_BLOCK to handled completely here
// Return 0 if no error, or else the appropriate exception value
// Any responses must be sent here using the call fnMODBUS_transmit(ucBuff, usLength, MODBUS_MODE_RTU, 0);
//
static int fnMODBUSuserFunction(int iType, MODBUS_RX_FUNCTION *modbus_rx_function)
{
    switch (iType) {
    case USER_RECEIVING_ALL_MODBUS_DATA:
        if (modbus_rx_function->ucSourceAddress != 33) {
            modbus_rx_function->ucMappedAddress = 33;                    // map the receive address to a new destination address
            return (MODBUS_APP_GATEWAY_FUNCTION - 0);                    // decide to gateway this to a different port - MODBUS port 0 in this case
        }
        break;
    case USER_RECEIVING_ALL_MODBUS_TYPE:
        break;
    case USER_RECEIVING_MISSED_RANGE:
        break;
    case USER_RECEIVING_MODBUS_UNDEFINED_FUNCTION:
        break;
    }
    if (modbus_rx_function->ucFunctionCode == MODBUS_READ_COILS) {
        unsigned char ucTest[6];                                         // the message to be sent must have the address at the start and 2 additional bytes space at the end for a check sum to be added. Do not use const data!
        ucTest[0] = modbus_rx_function->ucSourceAddress;                 // our address
        ucTest[1] = MODBUS_READ_COILS;
        ucTest[2] = 1;                                                   // byte count
        ucTest[3] = 3;                                                   // value
        // Two byte required for CRC but are not set here - transmitted length includes CRC
        //
        fnMODBUS_transmit(modbus_rx_function, ucTest, (sizeof(ucTest))); // answer with pre-defined response
        return 0;
    }
    return MODBUS_EXCEPTION_ILLEGAL_FUNCTION;                            // unsupported function
}

#if defined MODBUS_TCP
// Gateway routing call-back
//
static int fnGateway(int iType, MODBUS_RX_FUNCTION *modbus_rx_function)
{
    if (iType == TCP_ROUTE_FROM_SLAVE) {                                 // TCP frame from MODBUS TCP slave has been received. We should route it to a TCP master
        #if defined MODBUS_GATE_WAY_ROUTING
      //fnMODBUS_Master_send(5, 0, MODBUS_TRANSFER, modbus_rx_function->modbus_session->ptrMODBUS_input_frame); // {1} pass on to MODBUS port 0 - method not to be used for routing!!
//      return(MODBUS_APP_GATEWAY_FUNCTION - 5);                         // send to MODBUS TCP master
//      uTaskerMonoTimer( TASK_APPLICATION, (DELAY_LIMIT)(0), E_TEST_MODBUS_DELAY ); // test asap
        return(MODBUS_APP_GATEWAY_FUNCTION - 0);                         // send to MODBUS serial master
      //return 0;
        switch (modbus_rx_function->ucFunctionCode) {
        case MODBUS_WRITE_SINGLE_COIL:
            return (MODBUS_APP_GATEWAY_FUNCTION - 0);
        case MODBUS_WRITE_MULTIPLE_COILS:
            return(MODBUS_APP_GATEWAY_FUNCTION - 1);
        }
        return fnMODBUS_route(iType, modbus_rx_function, (MODBUS_ROUTE *)routing_table_1); // route using routing table
    #elif defined USE_MODBUS_MASTER                                      // {2}
        fnMODBUS_Master_send(0, 0, MODBUS_TRANSFER, modbus_rx_function->modbus_session->ptrMODBUS_input_frame); // {1} pass on to MODBUS port 0
        #endif
    }
    return 0;
}
#endif

static unsigned char ucNexttestPort = 0;;

#if defined USE_MODBUS_MASTER
// This routine is called by the MODBUS interface after a master transaction to a slave has completed
// It can be used to handle the response or error
//
static int fnMODBUSmaster(int iType, MODBUS_RX_FUNCTION *modbus_rx_function)
{
    switch (iType) {
    case SERIAL_ROUTE_FROM_SLAVE:
        {
        unsigned char ucTest[6];                                         // the message to be sent must have the address at the start and 2 additional bytes space at the end for a check sum to be added. Do not use const data!
        ucTest[1] = modbus_rx_function->ucFunctionCode;
        ucTest[2] = 1;
        ucTest[3] = 3;
        ucTest[0] = modbus_rx_function->ucSourceAddress;                 // our address
        return(MODBUS_APP_GATEWAY_FUNCTION - 2);
      //return (fnMODBUS_transmit(modbus_rx_function, ucTest, (sizeof(ucTest)))); // answer with pre-defined response
        }


    case MODBUS_CONNECTION_ESTABLISHED:
        fnDebugMsg("Connection established\r\n");
        return 0;
    case MODBUS_BROADCAST_TIMEOUT:
        fnDebugMsg("Broadcast timeout\r\n");
        break;
    case MODBUS_NO_SLAVE_RESPONSE:
        fnDebugMsg("MODBUS slave timeout\r\n");
        break;
    case MODBUS_CONNECTION_ABORTED:
        fnDebugMsg("Connection aborted\r\n");
        return 0;
    case MODBUS_CONNECTION_CLOSED:
        fnDebugMsg("Connection closed\r\n");
        return 0;
    case MODBUS_TCP_REPETITION:
        fnDebugMsg("TCP repetition\r\n");
        return 0;
    case MODBUS_SLAVE_RESPONSE:
        {
            if (modbus_rx_function->ucFunctionCode & 0x80) {
                fnDebugMsg("Exception: ");
            }
            switch (modbus_rx_function->ucFunctionCode & 0x7f) {
            case MODBUS_READ_COILS:                                      // function codes
                fnDebugMsg("Coils\r\n");
                break;
            case MODBUS_READ_DISCRETE_INPUTS:
                fnDebugMsg("Inputs\r\n");
                break;
            case MODBUS_READ_HOLDING_REGISTERS:
                fnDebugMsg("Hol Regs\r\n");
                break;
            case MODBUS_READ_INPUT_REGISTERS:
                fnDebugMsg("Input Regs\r\n");
                break;
            case MODBUS_WRITE_SINGLE_COIL:
                fnDebugMsg("Write Coil\r\n");
                break;
            case MODBUS_WRITE_SINGLE_REGISTER:
                fnDebugMsg("Write Reg\r\n");
                break;
            case MODBUS_WRITE_MULTIPLE_COILS:
                fnDebugMsg("Write Multiple Coils\r\n");
                break;
            case MODBUS_WRITE_MULTIPLE_REGISTERS:
                fnDebugMsg("Write Multiple Regs\r\n");
                break;
            case MODBUS_MASK_WRITE_REGISTER:
                fnDebugMsg("Mask Write Reg\r\n");
                break;
            case MODBUS_READ_WRITE_MULTIPLE_REGISTER:
                fnDebugMsg("Read Write Mult. Regs\r\n");
                break;
            case MODBUS_READ_FIFO_QUEUE:
                fnDebugMsg("FIFO read\r\n");
                break;
    #if defined MODBUS_SUPPORT_SERIAL_LINE_FUNCTIONS
            case MODBUS_READ_EXCEPTION_STATUS:
                fnDebugMsg("Exception Status =");
                fnDebugHex(modbus_rx_function->data_content.user_data[0], (WITH_LEADIN | WITH_CR_LF | WITH_SPACE | 1));
                break;
            case MODBUS_GET_COMM_EVENT_COUNTER:
                fnDebugMsg("Comm event cnt =");
                fnDebugDec(((modbus_rx_function->data_content.user_data[2] << 8) | modbus_rx_function->data_content.user_data[3]), (WITH_CR_LF | WITH_SPACE));
                break;
            case MODBUS_GET_COMM_EVENT_LOG:
                fnDebugMsg("Comm event log\r\n");
                break;
            case MODBUS_REPORT_SLAVE_ID:
                fnDebugMsg("Slave ID: ");
                modbus_rx_function->data_content.user_data[modbus_rx_function->data_content.usUserDataLength] = 0; // overwrite the running information in order to terminate ID string
                fnDebugMsg((CHAR *)&modbus_rx_function->data_content.user_data[1]);
                fnDebugMsg("\r\n");
                break;
        #if defined MODBUS_SUPPORT_SERIAL_LINE_DIAGNOSTICS
            case MODBUS_DIAGNOSTICS:
                {
                    unsigned short usSubFunction = ((modbus_rx_function->data_content.user_data[0] << 8) | modbus_rx_function->data_content.user_data[1]);
                    unsigned short usMsg = ((modbus_rx_function->data_content.user_data[2] << 8) | modbus_rx_function->data_content.user_data[3]);
                    fnDebugMsg("Diagnostics Result ");
                    switch (usSubFunction) {
                    case MODBUS_DIAG_SUB_RETURN_QUERY_DATA:
                        if ((modbus_rx_function->data_content.user_data[2] != 0x12) || (modbus_rx_function->data_content.user_data[3] != 0x34)) {
                            fnDebugMsg("Echo incorrect!!!\r\n");
                        }
                        else {
                            fnDebugMsg("Echo OK\r\n");
                        }
                        break;
                    case MODBUS_DIAG_SUB_RESTART_COMS:
                        fnDebugMsg("Resetting after restart-coms\r\n");
                        break;
                    case MODBUS_DIAG_SUB_RETURN_DIAG_REG:
                        fnDebugMsg("Diagnostics Reg.\r\n");
                        break;
                    case MODBUS_DIAG_SUB_CHANGE_ASCII_DELIM:
                        fnDebugMsg("Delimiter changed\r\n");
                        break;
                    case MODBUS_DIAG_SUB_CLEAR_DISGNOSTICS:
                        fnDebugMsg("Diag cleared\r\n");
                        break;
                    case MODBUS_DIAG_SUB_RTN_BUS_MSG_CNT:
                        fnDebugMsg("Bus msg cnt =");
                        fnDebugDec(usMsg, (WITH_CR_LF | WITH_SPACE));
                        break;
                    case MODBUS_DIAG_SUB_RTN_BUS_COM_ERR_CNT:
                        fnDebugMsg("Bus com err cnt =");
                        fnDebugDec(usMsg, (WITH_CR_LF | WITH_SPACE));
                        break;
                    case MODBUS_DIAG_SUB_RTN_BUS_EXC_ERR_CNT:
                        fnDebugMsg("Bus exc err cnt =");
                        fnDebugDec(usMsg, (WITH_CR_LF | WITH_SPACE));
                        break;
                    case MODBUS_DIAG_SUB_RTN_SLAVE_MSG_CNT:
                        fnDebugMsg("slave msg cnt =");
                        fnDebugDec(usMsg, (WITH_CR_LF | WITH_SPACE));
                        break;
                    case MODBUS_DIAG_SUB_RTN_SLAVE_NO_RSP_CNT:
                        fnDebugMsg("no slave rsp cnt =");
                        fnDebugDec(usMsg, (WITH_CR_LF | WITH_SPACE));
                        break;
                    case MODBUS_DIAG_SUB_RTN_SLAVE_NAK_CNT:
                        fnDebugMsg("slave nak cnt =");
                        fnDebugDec(usMsg, (WITH_CR_LF | WITH_SPACE));
                        break;
                    case MODBUS_DIAG_SUB_RTN_SLAVE_BSY_CNT:
                        fnDebugMsg("slave bsy cnt =");
                        fnDebugDec(usMsg, (WITH_CR_LF | WITH_SPACE));
                        break;
                    case MODBUS_DIAG_SUB_RTN_BUS_CHR_ORUN_CNT:
                        fnDebugMsg("bus orun cnt =");
                        fnDebugDec(usMsg, (WITH_CR_LF | WITH_SPACE));
                        break;
                    case MODBUS_DIAG_SUB_CLEAR_ORUN_AND_FLG:
                        fnDebugMsg("orun cleared\r\n");
                        break;
                    }
                }
                break;
        #endif
    #endif
            default:
                fnDebugMsg("Unknown\r\n");
                break;
            }
        }
        break;
    }
    if (modbus_rx_function != 0) {
      //fnModbusTest(modbus_rx_function->ucMODBUSport);                  // next test
        ucNexttestPort = modbus_rx_function->ucMODBUSport;
        uTaskerMonoTimer(TASK_APPLICATION, (DELAY_LIMIT)(1 * SEC), E_TEST_MODBUS_DELAY); // next test in 1 sec
    }
    return 0;
}

extern void fnNextTest(void)
{
    fnModbusTest(ucNexttestPort);                                        // next test    
}

// MODBUS master test
//
static void fnModbusTest(unsigned char ucTestPort)
{
    static int iTestNumber = 0;
    static unsigned char ucMasterTestPort;
    ucMasterTestPort = ucTestPort;

    switch (iTestNumber) {
    default:
        iTestNumber = 0;                                                 // repeat
    case 0:
        {
            static MODBUS_READ_QUANTITY read_coils = { 10, 11};          // starting at coil address 57 read 11 coils
            fnMODBUS_Master_send(ucMasterTestPort, TEST_SLAVE_ADDRESS, MODBUS_READ_COILS, (void *)&read_coils);
#if defined TEST_QUEUE
            read_coils.usStartAddress--;
            fnMODBUS_Master_send(ucMasterTestPort, TEST_SLAVE_ADDRESS, MODBUS_READ_COILS, (void *)&read_coils); // queue a second message
            read_coils.usStartAddress--;
            fnMODBUS_Master_send(ucMasterTestPort, TEST_SLAVE_ADDRESS, MODBUS_READ_COILS, (void *)&read_coils); // queue a third message
#endif
            read_coils.usStartAddress++;
            return;
        }
        break;
    case 1:
        {
            static const MODBUS_READ_QUANTITY read_discretes = {15, 7};  // starting at input address 15 read 7 inputs
            fnMODBUS_Master_send(ucMasterTestPort, TEST_SLAVE_ADDRESS, MODBUS_READ_DISCRETE_INPUTS, (void *)&read_discretes);
        }
        break;
    case 2:
        {
            static const MODBUS_READ_QUANTITY read_holding_regs = {2, 1}; // starting at register address 2 read 1 inputs
            fnMODBUS_Master_send(ucMasterTestPort, TEST_SLAVE_ADDRESS, MODBUS_READ_HOLDING_REGISTERS, (void *)&read_holding_regs);
        }
        break;
    case 3:
        {
            static const MODBUS_READ_QUANTITY read_input_regs = {24, 3}; // starting at register address 24 read 3 registers
            fnMODBUS_Master_send(ucMasterTestPort, TEST_SLAVE_ADDRESS, MODBUS_READ_INPUT_REGISTERS, (void *)&read_input_regs);
        }
        break;
    case 4:
        {
            static const MODBUS_COIL_STATE write_coil = {50, 1};         // set coil 50 to state 1
            fnMODBUS_Master_send(ucMasterTestPort, TEST_SLAVE_ADDRESS, MODBUS_WRITE_SINGLE_COIL, (void *)&write_coil);
        }
        break;
    case 5:
        {
            static const MODBUS_SINGLE_REGISTER write_register = {2, 0x1234}; // write register at address 2 with 0x1234
            fnMODBUS_Master_send(ucMasterTestPort, TEST_SLAVE_ADDRESS, MODBUS_WRITE_SINGLE_REGISTER, (void *)&write_register);
        }
        break;
    case 6:
        {
            static const unsigned char coil_values[] = {0x55, 0xaa, 0x07};
            static const  MODBUS_BITS multi_coils = {(MODBUS_BITS_ELEMENT *)&coil_values[0], {55, (55 + 8 -1)}}; // write 8 coils starting at coil 55
            fnMODBUS_Master_send(ucMasterTestPort, TEST_SLAVE_ADDRESS, MODBUS_WRITE_MULTIPLE_COILS, (void *)&multi_coils);
        }
        break;
    case 7:
        {
            static const unsigned short reg_values[] = {0x55aa, 0xcc33, 9876, 0xabcd};
            const MODBUS_REGISTERS multi_regs = { (unsigned short *)&reg_values[0], {2, 5}};  // write 4 registers starting at address 2 and finishing at address 5
            fnMODBUS_Master_send(ucMasterTestPort, TEST_SLAVE_ADDRESS, MODBUS_WRITE_MULTIPLE_REGISTERS, (void *)&multi_regs);
        }
        break;
    case 8:
        {
            static const MODBUS_SINGLE_REGISTER_MASK reg_mod = {2, 0xff00, 0x0055}; // AND register at address 2 with 0xff00 and then OR with 0x0055
            fnMODBUS_Master_send(ucMasterTestPort, TEST_SLAVE_ADDRESS, MODBUS_MASK_WRITE_REGISTER, (void *)&reg_mod);
        }
        break;
    case 9:
        {
            static const unsigned short reg_values[] = {0x1234, 0x5678, 0x9abc, 0xdef0, 0x55aa};
            const MODBUS_READ_WRITE_REGISTERS multi_regs = {{2, 5}, {(unsigned short *)&reg_values[0], {2, 6}}}; // write 5 holding registers starting at address 2 and finishing at address 6, then read 5 registers starting at address 2
            fnMODBUS_Master_send(ucMasterTestPort, TEST_SLAVE_ADDRESS, MODBUS_READ_WRITE_MULTIPLE_REGISTER, (void *)&multi_regs);
        }
        break;
    case 10:
        {
            static const FIFO_ADDRESS FIFO_Add = {0x0003};               // read FIFO queue from FIFO pointer register 3
            fnMODBUS_Master_send(ucMasterTestPort, TEST_SLAVE_ADDRESS, MODBUS_READ_FIFO_QUEUE, (void *)&FIFO_Add);
        }
        break;

    #if defined MODBUS_SUPPORT_SERIAL_LINE_FUNCTIONS
    case 11:
        fnMODBUS_Master_send(ucMasterTestPort, TEST_SLAVE_ADDRESS, MODBUS_READ_EXCEPTION_STATUS, 0);
        break;
        #if defined MODBUS_SUPPORT_SERIAL_LINE_DIAGNOSTICS
    case 12:                                                              // echo test
        {
            static const MODBUS_DIAGNOSTICS_REQUEST diag = {MODBUS_DIAG_SUB_RETURN_QUERY_DATA, 0x1234};
            fnMODBUS_Master_send(ucMasterTestPort, TEST_SLAVE_ADDRESS, MODBUS_DIAGNOSTICS, (void *)&diag);
        }
        break;
    case 13:                                                             // read diagnostic register
        {
            static const MODBUS_DIAGNOSTICS_REQUEST diag = {MODBUS_DIAG_SUB_RETURN_DIAG_REG, 0};
            fnMODBUS_Master_send(ucMasterTestPort, TEST_SLAVE_ADDRESS, MODBUS_DIAGNOSTICS, (void *)&diag);
        }
        break;
    case 14:                                                             // change ASCII input delimiter
        {
            static const MODBUS_DIAGNOSTICS_REQUEST diag = {MODBUS_DIAG_SUB_CHANGE_ASCII_DELIM, 0x1000}; // new value is 0x10
            fnMODBUS_Master_send(ucMasterTestPort, TEST_SLAVE_ADDRESS, MODBUS_DIAGNOSTICS, (void *)&diag);
        }
        break;
    case 15:                                                             // get bus message count
        {
            static const MODBUS_DIAGNOSTICS_REQUEST diag = {MODBUS_DIAG_SUB_RTN_BUS_MSG_CNT, 0x0000};
            fnMODBUS_Master_send(ucMasterTestPort, TEST_SLAVE_ADDRESS, MODBUS_DIAGNOSTICS, (void *)&diag);
        }
        break;
    case 16:                                                             // get bus communication error count
        {
            static const MODBUS_DIAGNOSTICS_REQUEST diag = {MODBUS_DIAG_SUB_RTN_BUS_COM_ERR_CNT, 0x0000};
            fnMODBUS_Master_send(ucMasterTestPort, TEST_SLAVE_ADDRESS, MODBUS_DIAGNOSTICS, (void *)&diag);
        }
        break;
    case 17:                                                             // get bus exception error count
        {
            static const MODBUS_DIAGNOSTICS_REQUEST diag = {MODBUS_DIAG_SUB_RTN_BUS_EXC_ERR_CNT, 0x0000};
            fnMODBUS_Master_send(ucMasterTestPort, TEST_SLAVE_ADDRESS, MODBUS_DIAGNOSTICS, (void *)&diag);
        }
        break;
    case 18:                                                             // get slave message count
        {
            static const MODBUS_DIAGNOSTICS_REQUEST diag = {MODBUS_DIAG_SUB_RTN_SLAVE_MSG_CNT, 0x0000};
            fnMODBUS_Master_send(ucMasterTestPort, TEST_SLAVE_ADDRESS, MODBUS_DIAGNOSTICS, (void *)&diag);
        }
        break;
    case 19:                                                             // get slave no response count
        {
            static const MODBUS_DIAGNOSTICS_REQUEST diag = {MODBUS_DIAG_SUB_RTN_SLAVE_NO_RSP_CNT, 0x0000};
            fnMODBUS_Master_send(ucMasterTestPort, TEST_SLAVE_ADDRESS, MODBUS_DIAGNOSTICS, (void *)&diag);
        }
        break;
    case 20:                                                             // get slave nack count
        {
            static const MODBUS_DIAGNOSTICS_REQUEST diag = {MODBUS_DIAG_SUB_RTN_SLAVE_NAK_CNT, 0x0000};
            fnMODBUS_Master_send(ucMasterTestPort, TEST_SLAVE_ADDRESS, MODBUS_DIAGNOSTICS, (void *)&diag);
        }
        break;
    case 21:                                                             // get slave busy count
        {
            static const MODBUS_DIAGNOSTICS_REQUEST diag = {MODBUS_DIAG_SUB_RTN_SLAVE_BSY_CNT, 0x0000};
            fnMODBUS_Master_send(ucMasterTestPort, TEST_SLAVE_ADDRESS, MODBUS_DIAGNOSTICS, (void *)&diag);
        }
        break;
    case 22:                                                             // get bus character overrun count
        {
            static const MODBUS_DIAGNOSTICS_REQUEST diag = {MODBUS_DIAG_SUB_RTN_BUS_CHR_ORUN_CNT, 0x0000};
            fnMODBUS_Master_send(ucMasterTestPort, TEST_SLAVE_ADDRESS, MODBUS_DIAGNOSTICS, (void *)&diag);
        }
        break;
    case 23:                                                             // clear overrun counter and flag
        {
            static const MODBUS_DIAGNOSTICS_REQUEST diag = {MODBUS_DIAG_SUB_CLEAR_ORUN_AND_FLG, 0x0000};
            fnMODBUS_Master_send(ucMasterTestPort, TEST_SLAVE_ADDRESS, MODBUS_DIAGNOSTICS, (void *)&diag);
        }
        break;
    case 24:                                                             // get comm event counter
        fnMODBUS_Master_send(ucMasterTestPort, TEST_SLAVE_ADDRESS, MODBUS_GET_COMM_EVENT_COUNTER, 0);
        break;
    case 25:                                                             // get comm event log
        fnMODBUS_Master_send(ucMasterTestPort, TEST_SLAVE_ADDRESS, MODBUS_GET_COMM_EVENT_LOG, 0);
        break;
    case 26:                                                             // clear all counters and the diagnostic register
        {
            static const MODBUS_DIAGNOSTICS_REQUEST diag = {MODBUS_DIAG_SUB_CLEAR_DISGNOSTICS, 0x0000};
            fnMODBUS_Master_send(ucMasterTestPort, TEST_SLAVE_ADDRESS, MODBUS_DIAGNOSTICS, (void *)&diag);
        }
        break;
    case 27:
        fnMODBUS_Master_send(ucMasterTestPort, TEST_SLAVE_ADDRESS, MODBUS_REPORT_SLAVE_ID, 0);
        break;
        #endif
/*                                                                       // this test renders the slave inactive so is bypassed for normal tests
    case 28:                                                             // force listen only mode
        {
            static const MODBUS_DIAGNOSTICS_REQUEST diag = {MODBUS_DIAG_SUB_FORCE_LISTEN_ONLY, 0x0000};
            fnMODBUS_Master_send(ucMasterTestPort, TEST_SLAVE_ADDRESS, MODBUS_DIAGNOSTICS, (void *)&diag);
        }
        break;*/
/*                                                                       // this test resets the board and so is commented out for normal tests
    case 29:                                                             // reset and clear log
        {
            static const MODBUS_DIAGNOSTICS_REQUEST diag = {MODBUS_DIAG_SUB_RESTART_COMS, RESTART_CLEAR_LOG};
            fnMODBUS_Master_send(ucMasterTestPort, TEST_SLAVE_ADDRESS, MODBUS_DIAGNOSTICS, (void *)&diag);
        }
        break;*/
    #endif
    }
    iTestNumber++;                                                       // prepare for next test
}
#endif

extern void fnInitModbus(void)
{
    unsigned char usModbusPort = 0;
    ptrMODBUS_pars = uMalloc(sizeof(MODBUS_PARS));                       // get memory for MODBUS parameters
    fnGetMODBUS_parameters();                                            // fill the working parameters from configuration settings

#if MODBUS_SERIAL_INTERFACES > 1
    fnInitialiseMODBUS_port(usModbusPort++, 0, 0, fnMODBUSmaster);       // initialise MODBUS serial interface - port 0 (master)
#endif
    fnInitialiseMODBUS_port(usModbusPort, &modbus_configuration, &modbus_slave_callbacks, fnMODBUSmaster); // port 1 - both serial interfaces use the same application tables, but could use separate ones if required
#if MODBUS_SHARED_SERIAL_INTERFACES > 0
    fnShareMODBUS_port(usModbusPort, &modbus_configuration, &modbus_slave_callbacks, (ptrMODBUS_pars->ucModbus_slave_address[1] + 1)); // additional MODBUS serial slave sharing port 0 UART - bus address slave + 1
    fnShareMODBUS_port(usModbusPort, &modbus_configuration, &modbus_slave_callbacks, (ptrMODBUS_pars->ucModbus_slave_address[1] + 2)); // additional MODBUS serial slave sharing port 0 UART - bus address slave + 2
#endif
#if defined MODBUS_TCP
    fnInitialiseMODBUS_port(++usModbusPort, &modbus_configuration, &modbus_slave_callbacks, fnGateway); // initialise MODBUS tcp interface on first TCP port
    fnInitialiseMODBUS_port(++usModbusPort, 0, &modbus_slave_callbacks, 0);           // initialise MODBUS tcp interface on second TCP port
    #if MODBUS_SHARED_TCP_INTERFACES > 0
    fnShareMODBUS_port((usModbusPort - 1), &modbus_configuration, &modbus_slave_callbacks, 2); // additional MODBUS slave sharing port 2 TCP - bus address 2
    fnShareMODBUS_port((usModbusPort - 1), &modbus_configuration, &modbus_slave_callbacks, 3); // additional MODBUS slave sharing port 2 TCP - bus address 3
    #endif
#endif
#if MODBUS_TCP_MASTERS > 0 && defined MODBUS_TCP
    #if defined _WINDOWS && defined PSEUDO_LOOPBACK
    fnAddARP(&network.ucOurIP[0], (unsigned char *)cucBroadcast, ARP_FIXED_IP); // prime our own IP address in the ARP table to allow internal loop back testing - only for simulation!
    #endif
    fnInitialiseMODBUS_port(++usModbusPort, 0, 0, fnMODBUSmaster);       // initialise MODBUS TCP MASTER
    fnInitialiseMODBUS_port(++usModbusPort, 0, 0, fnMODBUSmaster);       // initialise MODBUS TCP MASTER
    #if defined USE_MODBUS_MASTER
    fnModbusTest(0);                                                     // start a MODBUS master test on TCP master
    #endif
#elif defined USE_MODBUS_MASTER
    fnModbusTest(0);                                                     // start a MODBUS master test on the serial port
#endif
}
#endif
