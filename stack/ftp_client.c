/***********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher

    ---------------------------------------------------------------------
    File:      ftp_client.c
    Project:   Single Chip Embedded Internet
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2016
    *********************************************************************
    03.06.2012 Return to state FTP_CLIENT_STATE_LOGGED_IN when response 200 is not received when expected {1}
    03.06.2012 Listing and Getting requires both the server's success response and the data connection to be closed before terminating and informing of success {2}
    03.06.2012 Remember the ASCII/binary state of server and only send type when a change is required {3}
    03.06.2012 Add flags to fnFTP_client_connect() and support of IPv6 operation {4}
    05.06.2012 Accept "125" as well as "150" when getting and listing    {5}
    05.06.2012 Close open data connection on errors                      {6}

*/

#include "config.h"

#ifdef USE_FTP_CLIENT

/* =================================================================== */
/*                          local definitions                          */
/* =================================================================== */

// FTP client states
//
#define FTP_CLIENT_STATE_CLOSED                 0                        // FTP control connection is not active
#define FTP_CLIENT_STATE_CONNECTING             1                        // FTP control connection presently in connection process
#define FTP_CLIENT_STATE_CONNECTED              2                        // FTP control connection is connected but idle
#define FTP_CLIENT_STATE_LOGIN_USER             3                        // FTP control connection is sending the user name during login
#define FTP_CLIENT_STATE_LOGIN_PASS             4                        // FTP control connection is sending the user password during login
#define FTP_CLIENT_STATE_LOGGED_IN              5                        // FTP client is logged in to the FTP server and the connection is idle

#define FTP_CLIENT_STATE_LIST_AFTER_ASCII       20                       // keep strict order of these state groups!!
#define FTP_CLIENT_STATE_RENAME_AFTER_ASCII     21                       // the exact number range of the groups is not important but the defines in a group must count upward with no spaces
#define FTP_CLIENT_STATE_DELETE_AFTER_ASCII     22
#define FTP_CLIENT_STATE_REMOVE_AFTER_ASCII     23
#define FTP_CLIENT_STATE_MKDIR_AFTER_ASCII      24
#define FTP_CLIENT_STATE_PATH_AFTER_ASCII       25
#define FTP_CLIENT_STATE_APP_AFTER_ASCII        26
#define FTP_CLIENT_STATE_PUT_AFTER_ASCII        27
#define FTP_CLIENT_STATE_GET_AFTER_ASCII        28
#define FTP_CLIENT_STATE_APP_AFTER_BINARY       29
#define FTP_CLIENT_STATE_PUT_AFTER_BINARY       30
#define FTP_CLIENT_STATE_GET_AFTER_BINARY       31

#define FTP_CLIENT_STATE_SET_PASSIVE_LST        50
#define FTP_CLIENT_STATE_SET_PASSIVE_APP        51
#define FTP_CLIENT_STATE_SET_PASSIVE_PUT        52
#define FTP_CLIENT_STATE_SET_PASSIVE_GET        53

#define FTP_CLIENT_STATE_PREPARE_LST            70
#define FTP_CLIENT_STATE_PREP_RENAMING          71                       // dummy
#define FTP_CLIENT_STATE_PREP_DELETING          72                       // dummy
#define FTP_CLIENT_STATE_PREP_REMOVING          73                       // dummy
#define FTP_CLIENT_STATE_PREP_CREATING          74                       // dummy
#define FTP_CLIENT_STATE_PREP_SETTING           75                       // dummy
#define FTP_CLIENT_STATE_PREPARE_APP_ASCII      76
#define FTP_CLIENT_STATE_PREPARE_PUT_ASCII      77
#define FTP_CLIENT_STATE_PREPARE_GET_ASCII      78
#define FTP_CLIENT_STATE_PREPARE_APP_BIN        79
#define FTP_CLIENT_STATE_PREPARE_PUT_BIN        80
#define FTP_CLIENT_STATE_PREPARE_GET_BIN        81

#define FTP_CLIENT_STATE_LISTING                100
#define FTP_CLIENT_STATE_RENAMING               101
#define FTP_CLIENT_STATE_DELETING               102
#define FTP_CLIENT_STATE_REMOVING               103
#define FTP_CLIENT_STATE_CREATING               104
#define FTP_CLIENT_STATE_SETTING                105
#define FTP_CLIENT_STATE_APPENDING              106
#define FTP_CLIENT_STATE_PUTTING                107
#define FTP_CLIENT_STATE_GETTING                108
#define FTP_CLIENT_STATE_RENAMING_2             109

#define FTP_CLIENT_STATE_PREPARE_ACT_LST        130
#define FTP_CLIENT_STATE_PREPARE_ACT_RENAME     131
#define FTP_CLIENT_STATE_PREPARE_ACT_DELETE     132
#define FTP_CLIENT_STATE_PREPARE_ACT_REMOVE     133
#define FTP_CLIENT_STATE_PREPARE_ACT_MKDIR      134
#define FTP_CLIENT_STATE_PREPARE_ACT_PATH       135
#define FTP_CLIENT_STATE_PREPARE_ACT_APP        136
#define FTP_CLIENT_STATE_PREPARE_ACT_PUT        137
#define FTP_CLIENT_STATE_PREPARE_ACT_GET        138

#define FTP_CLIENT_STATE_TERMINATING            150



// FTP client message references
//
#define FTP_CLIENT_MSG_DO_NOTHING               0                        // message numbers
#define FTP_CLIENT_MSG_REPEAT_DATA              1
#define FTP_CLIENT_MSG_USER                     2
#define FTP_CLIENT_MSG_USER_PASS                3

#define FTP_CLIENT_MSG_SET_ASCII_LST            20                       // keep strict order of these message groups!!
#define FTP_CLIENT_MSG_SET_ASCII_RENAME         21                       // the exact number range of the groups is not important but the defines in a group must count upward with no spaces
#define FTP_CLIENT_MSG_SET_ASCII_DELETE_FILE    22
#define FTP_CLIENT_MSG_SET_ASCII_REMOVE_DIR     23
#define FTP_CLIENT_MSG_SET_ASCII_MAKE_DIR       24
#define FTP_CLIENT_MSG_SET_ASCII_PATH           25
#define FTP_CLIENT_MSG_SET_ASCII_APP            26
#define FTP_CLIENT_MSG_SET_ASCII_PUT            27
#define FTP_CLIENT_MSG_SET_ASCII_GET            28

#define FTP_CLIENT_MSG_SET_BINARY_APP           40
#define FTP_CLIENT_MSG_SET_BINARY_PUT           41
#define FTP_CLIENT_MSG_SET_BINARY_GET           42

#define FTP_CLIENT_MSG_LIST                     60
#define FTP_CLIENT_MSG_RENAME                   61
#define FTP_CLIENT_MSG_DELETE                   62
#define FTP_CLIENT_MSG_REMOVE                   63
#define FTP_CLIENT_MSG_MAKE                     64
#define FTP_CLIENT_MSG_PATH                     65
#define FTP_CLIENT_MSG_APP                      66
#define FTP_CLIENT_MSG_PUT                      67
#define FTP_CLIENT_MSG_GET                      68
#define FTP_CLIENT_MSG_RENAME_2                 69

#define FTP_CLIENT_MSG_SET_PSV_LST              90
#define FTP_CLIENT_MSG_SET_PSV_RENAME           91
#define FTP_CLIENT_MSG_SET_PSV_DELETE_FILE      92
#define FTP_CLIENT_MSG_SET_PSV_REMOVE_DIR       93
#define FTP_CLIENT_MSG_SET_PSV_MAKE_DIR         94
#define FTP_CLIENT_MSG_SET_PSV_PATH             95
#define FTP_CLIENT_MSG_SET_PSV_APP_ASCII        96
#define FTP_CLIENT_MSG_SET_PSV_PUT_ASCII        97
#define FTP_CLIENT_MSG_SET_PSV_GET_ASCII        98
#define FTP_CLIENT_MSG_SET_PSV_APP_BIN          99
#define FTP_CLIENT_MSG_SET_PSV_PUT_BIN          100
#define FTP_CLIENT_MSG_SET_PSV_GET_BIN          101

#define FTP_CLIENT_MSG_ACTIVE_LIST              130
#define FTP_CLIENT_MSG_ACTIVE_RENAME            131
#define FTP_CLIENT_MSG_ACTIVE_DELETE_FILE       132
#define FTP_CLIENT_MSG_ACTIVE_REMOVE_DIR        133
#define FTP_CLIENT_MSG_ACTIVE_MAKE_DIR          134
#define FTP_CLIENT_MSG_ACTIVE_PATH              135
#define FTP_CLIENT_MSG_ACTIVE_APP               136
#define FTP_CLIENT_MSG_ACTIVE_PUT               137
#define FTP_CLIENT_MSG_ACTIVE_GET               138

#define FTP_CLIENT_MSG_CLOSE_SESSION            160

#define FTP_CLIENT_MSG_REPEAT_CONTROL           180



#define DATA_CONNECTION_CLOSED                  0x0                      // {2}
#define DATA_CONNECTION_CONNECTED               0x1
#define WAITING_LISTING_CLOSE                   0x2
#define WAITING_GETTING_CLOSE                   0x4

#define SERVER_TYPE_UNKNOWN                     0                        // {3}
#define SERVER_TYPE_BINARY                      1
#define SERVER_TYPE_ASCII                       2


#define FTP_CLIENT_IPV4                         0x0                      // flag that the connection is over IPv4
#define FTP_CLIENT_IPV6                         0x1                      // flag that the connection is over IPv6
#define FTP_CLIENT_IPV6_LINK_LOCAL              0x2                      // flag that the connection is over IPv6 using the link local address (rather than global)


typedef struct stTCP_FTP_CLIENT_MESSAGE                                  // definition of a data frame structure
{
    TCP_HEADER     tTCP_Header;                                          // reserve header space
    unsigned char  ucTCP_Message[MAX_FTP_CLIENT_PATH_LENGTH + 10];       // space for content
} TCP_FTP_CLIENT_MESSAGE;


/* =================================================================== */
/*                      local variable definitions                     */
/* =================================================================== */

    #if !defined FTP_CLIENT_EXTERN_DATA_SOCKET
static USOCKET          FTP_TCP_client_Data_socket = -1;
    #endif
static unsigned short   FTP_TCP_data_port;
static USOCKET          FTP_Client_TCP_socket = -1;
static unsigned char    ucFTP_client_state = FTP_CLIENT_STATE_CLOSED;
static unsigned char    ucLastControl = FTP_CLIENT_MSG_DO_NOTHING;
static CHAR             dir_location[MAX_FTP_CLIENT_PATH_LENGTH + 8];
static CHAR            *ptrRenameTo = 0;
#if !defined FTP_CLIENT_EXTERN_DATA_SOCKET
    static int          iDataConnection = DATA_CONNECTION_CLOSED;        // {2}
#endif
static int              iServerType = SERVER_TYPE_UNKNOWN;               // {3}
#if defined USE_IPV6
    static int          iServerIPv6 = 0;                                 // {4}
    static unsigned char ucServerIPv6_address[IPV6_LENGTH];              // backup of IPv6 server address
#endif


/* =================================================================== */
/*                 local function prototype declarations               */
/* =================================================================== */

#if !defined FTP_CLIENT_EXTERN_DATA_SOCKET
    static int      fnFTP_Data_Listener(USOCKET cSocket, unsigned char ucEvent, unsigned char *ucIp_Data, unsigned short usPortLen);
#endif
static int          fnFTP_client_ControlListener(USOCKET cSocket, unsigned char ucEvent, unsigned char *ucIp_Data, unsigned short usPortLen);
static signed short fnSendClientFTP(unsigned char ucMsg);
static int         (*fnFTP_client_user_callback)(TCP_CLIENT_MESSAGE_BOX *) = 0;
static void         fnEnterPath(CHAR *ptrPath, const CHAR *ptrCommand);


/* =================================================================== */
/*                             constants                               */
/* =================================================================== */

static const CHAR   cFTP_TYPE_A[]  = "TYPE A\r\n";                       // request ascii type
static const CHAR   cFTP_TYPE_I[]  = "TYPE I\r\n";                       // request binary typ
static const CHAR   cFTP_PASV[]    = "PASV\r\n";                         // request passive mode
#if defined USE_IPV6                                                     // {4}
    static const CHAR cFTP_EPSV[]  = "EPSV\r\n";                         // request extended passive mode
#endif
static const CHAR   cFTP_QUIT[]    = "QUIT\r\n";                         // quit FTP session


// Connect to an FTP server located at a defined IP address
//
extern USOCKET fnFTP_client_connect(unsigned char *ptr_ucIP_address, unsigned short ucPortNumber, unsigned short usFTPTimeout, int (*user_callback)(TCP_CLIENT_MESSAGE_BOX *), int iFlags) // {4}
{
#if defined USE_IPV6
    unsigned long ulMaxWindow_flags = 0;
    if (iFlags & FTP_CLIENT_IPv6) {
        ulMaxWindow_flags = TCP_CONNECT_IPV6;
        iServerIPv6 = FTP_CLIENT_IPV6;
    }
    else {
        iServerIPv6 = 0;
    }
#endif
    if (user_callback == 0) {
        return NO_TCP_LISTENER_INSTALLED;                                // a user call back must be installed!!
    }
    if (FTP_Client_TCP_socket == -1) {                                   // socket doesn't exist yet
        FTP_Client_TCP_socket = fnGetTCP_Socket(TOS_MINIMISE_DELAY, usFTPTimeout, fnFTP_client_ControlListener); // get TCP Socket and listen on FTP control port
    #if !defined FTP_CLIENT_EXTERN_DATA_SOCKET
        FTP_TCP_client_Data_socket = fnGetTCP_Socket(TOS_MAXIMISE_THROUGHPUT, usFTPTimeout, fnFTP_Data_Listener); // reserve FTP data socket with same timeout value as the control socket
    #endif
    #if defined FTP_CLIENT_BUFFERED_SOCKET_MODE && defined INDIVIDUAL_BUFFERED_TCP_BUFFER_SIZE
        fnDefineTCPBufferSize(FTP_TCP_client_Data_socket, FTP_CLIENT_TCP_BUFFER_LENGTH); // dimension the buffered socket's output buffer
    #endif
    }
#if defined USE_IPV6
    if ((iServerIPv6 != 0) && ((uMemcmp(ptr_ucIP_address, ucLinkLocalIPv6Address[DEFAULT_NETWORK], IPV6_SUBNET_LENGTH) == 0))) {
        iServerIPv6 = (FTP_CLIENT_IPV6_LINK_LOCAL | FTP_CLIENT_IPV6);
    }
    uMemcpy(ucServerIPv6_address, ptr_ucIP_address, IPV6_LENGTH);        // make a copy of the IPv6 server address in case we need to make a passiev data connection to it later
    if (fnTCP_Connect(FTP_Client_TCP_socket, ptr_ucIP_address, ucPortNumber, 0, ulMaxWindow_flags) < 0) {
        return SOCKET_STATE_INVALID;                                     // connection couldn't be started
    }
#else
    if (fnTCP_Connect(FTP_Client_TCP_socket, ptr_ucIP_address, ucPortNumber, 0, 0) < 0) {
        return SOCKET_STATE_INVALID;                                     // connection couldn't be started
    }
#endif
    ucFTP_client_state = FTP_CLIENT_STATE_CONNECTING;                    // we are now in the process of connecting
    fnFTP_client_user_callback = user_callback;
    return FTP_Client_TCP_socket;                                        // return the control socket handle when successful
}

// Disconnect from FTP server
//
extern int fnFTP_client_disconnect(void)
{
    if (ucFTP_client_state != FTP_CLIENT_STATE_LOGGED_IN) {              // the control connection must be in idle state to be able to quit
        return SOCKET_STATE_INVALID;
    }
    return (fnSendClientFTP(FTP_CLIENT_MSG_CLOSE_SESSION));
}


// Command a listing of the directory content
// - if a zero path is gived it will request from the present directory location in ascii mode
// - if a directory path is given it will first move to that directory and then request the contents in ascii mode
//
extern int fnFTP_client_dir(CHAR *ptrPath, int iAction)
{
    if (ucFTP_client_state == FTP_CLIENT_STATE_LOGGED_IN) {              // the control connection must be in idle state to be able to start
        unsigned char ucCommand;
        switch (iAction) {
        case FTP_DIR_SET_PATH:
            fnEnterPath(ptrPath, "CWD");
            ucCommand = FTP_CLIENT_MSG_SET_ASCII_PATH;                   // always set to ASCII type before setting the path
            break;
        case FTP_DIR_LIST:
            fnEnterPath(ptrPath, "LIST");
            ucCommand = FTP_CLIENT_MSG_SET_ASCII_LST;                    // always set to ASCII type before listing
            break;
        case FTP_DIR_RENAME:
            fnEnterPath(ptrPath, "RNFR");
            ucCommand = FTP_CLIENT_MSG_SET_ASCII_RENAME;                 // always set to ASCII type before renaming
            break;
        case FTP_DIR_DELETE:
            fnEnterPath(ptrPath, "DELE");
            ucCommand = FTP_CLIENT_MSG_SET_ASCII_DELETE_FILE;            // always set to ASCII type before deleting file
            break;
        case FTP_DIR_REMOVE_DIR:
            fnEnterPath(ptrPath, "RMD");
            ucCommand = FTP_CLIENT_MSG_SET_ASCII_REMOVE_DIR;             // always set to ASCII type before attempting to remove empty directory
            break;
        case FTP_DIR_MAKE_DIR:
            fnEnterPath(ptrPath, "XMKD");
            ucCommand = FTP_CLIENT_MSG_SET_ASCII_MAKE_DIR;               // always set to ASCII type before creating new directory
            break;
        default:
            return SOCKET_STATE_INVALID;
        }
        return (fnSendClientFTP(ucCommand));                             // send first command in sequence
    }
    return SOCKET_STATE_INVALID;
}

// Command a data transfer, which can be a get, put or append type
// - a valid <path name and> file name must be given and the mode can be defined to be either ascii or binary
//
extern int fnFTP_client_transfer(CHAR *ptrFilePath, int iMode)
{
    unsigned char ucMessage = FTP_CLIENT_MSG_DO_NOTHING;
    if (ucFTP_client_state != FTP_CLIENT_STATE_LOGGED_IN) {              // the control connection must be in idle state to start
        return SOCKET_STATE_INVALID;
    }
    if (iMode & (FTP_DO_PUT | FTP_DO_APPEND)) {                          // not get
        if (iMode & FTP_DO_APPEND) {                                     // append
            fnEnterPath(ptrFilePath, "APPE");
            if (iMode & FTP_TRANSFER_ASCII) {                            // append in ascii mode
                ucMessage = FTP_CLIENT_MSG_SET_ASCII_APP;                // set to ASCII type before appening data
            }
            else {
                ucMessage = FTP_CLIENT_MSG_SET_BINARY_APP;               // set to binary mode before appending data
            }
        }
        else {                                                           // put
            fnEnterPath(ptrFilePath, "STOR");
            if (iMode & FTP_TRANSFER_ASCII) {                            // put in ascii mode
                ucMessage = FTP_CLIENT_MSG_SET_ASCII_PUT;                // set to ASCII type before sending data
            }
            else {                                                       // send in binary mode
                ucMessage = FTP_CLIENT_MSG_SET_BINARY_PUT;               // set to binary type before sending data
            }
        }
    }
    else {                                                               // get
        fnEnterPath(ptrFilePath, "RETR");
        if (iMode & FTP_TRANSFER_ASCII) {                                // get in ascii mode
            ucMessage = FTP_CLIENT_MSG_SET_ASCII_GET;                    // set to ASCII type before retrieving data
        }
        else {                                                           // get in binary mode
            ucMessage = FTP_CLIENT_MSG_SET_BINARY_GET;                   // set to binary type before retrieving data
        }
    }
    return (fnSendClientFTP(ucMessage));
}


// This routine constructs a CWD, LIST, APP, PUT or GET with a path name which remains valid until the command has been successfully acknowledged
//
static void fnEnterPath(CHAR *ptrPath, const CHAR *ptrCommand)
{
    CHAR *ptrEnd;
    int iPathLength;
    if (ptrPath != 0) {
        iPathLength = uStrlen(ptrPath);
        if (iPathLength > MAX_FTP_CLIENT_PATH_LENGTH) {
            iPathLength = MAX_FTP_CLIENT_PATH_LENGTH;
        }
    }
    else {
        iPathLength = 0;
    }
    ptrEnd = uStrcpy(dir_location, ptrCommand);
    *ptrEnd++ = ' ';
    uMemcpy(ptrEnd, ptrPath, iPathLength);
    ptrEnd += iPathLength;
    *ptrEnd++ = CARRIAGE_RETURN;
    *ptrEnd++ = LINE_FEED;
    *ptrEnd = 0;
}


// Extract the IP address and port number out of the reply from the FTP server
//
// Note that ftp.c uses a routine called fnGetDataPort to effectively do the same. These could be shared.
//
static unsigned short fnExtractPassivePort(CHAR *ptrPassiveString, unsigned char ucIP_data_address[IPV4_LENGTH], unsigned short usLength)
{
    register unsigned char ucDigit;
    int iSearchState = 0;
    unsigned short usPort = 0;
    unsigned char ucIP_digit = 0;
    // Search for the details in the format (xx,yy,zz,aa,p1,p2)
    //
    while (usLength--) {
        ucDigit = *ptrPassiveString;
        switch (iSearchState) {
        case 0:                                                          // searching
            if (ucDigit == '(') {
                iSearchState = 1;                                        // start recognised
                ucIP_digit = 0;
            }
            break;
        case 1:                                                          // IP address
        case 2:
        case 3:
        case 4:
            if ((ucDigit < '0') || (ucDigit > '9')) {                    // end of IP digit
                *ucIP_data_address++ = ucIP_digit;
                ucIP_digit = 0;
                iSearchState++;
            }
            else {                                                       // assumed to be 0..9
                ucIP_digit = ((ucIP_digit * 8) + (ucIP_digit * 2));
                ucIP_digit += (ucDigit - '0');
            }
            break;
        case 5:                                                          // port
        case 6:
            if ((ucDigit < '0') || (ucDigit > '9')) {                    // end of port number digit
                usPort <<= 8;
                usPort += ucIP_digit;
                ucIP_digit = 0;
                iSearchState++;              
            }
            else {                                                       // assumed to be 0..9
                ucIP_digit = ((ucIP_digit * 8) + (ucIP_digit * 2));
                ucIP_digit += (ucDigit - '0');
            }
            break;
        default:
            return usPort;
        }
        ptrPassiveString++;
    }
    return usPort;
}

#if defined FTP_CLIENT_EXTERN_DATA_SOCKET
// Generate an event number containing details about the data connection to be established
//
static int fnGenerateEventNumber(unsigned char ucFTP_client_state)
{
    int iEventNumber;
    if (ucFTP_client_state > FTP_CLIENT_STATE_GET_AFTER_BINARY) {
        iEventNumber = FTP_CLIENT_EVENT_PASSIVE_CONNECT_DATA;            // passive connection required (we must connect to FTP server's data port)
    }
    else {
        iEventNumber = FTP_CLIENT_EVENT_ACTIVE_LISTEN_DATA;              // active connection required (we must listen on the FTP server's data port)
    }
    if ((iEventNumber < FTP_CLIENT_STATE_APP_AFTER_BINARY) || ((iEventNumber >= FTP_CLIENT_STATE_PREPARE_LST) && (iEventNumber < FTP_CLIENT_STATE_PREPARE_APP_BIN))) {
        iEventNumber |= FTP_CLIENT_EVENT_FLAG_ASCII_MODE;                // flag that we are in ASCII mode rather than binary
    }
    switch (ucFTP_client_state) {
    case FTP_CLIENT_STATE_LIST_AFTER_ASCII:
    case FTP_CLIENT_STATE_PREPARE_LST:
        iEventNumber |= FTP_CLIENT_EVENT_FLAG_LISTING;                   // mark that a listing will be received rather than file content
        break;
    case FTP_CLIENT_STATE_GET_AFTER_ASCII:                               // the data direction is from the FTP server to the client (we receive)
    case FTP_CLIENT_STATE_GET_AFTER_BINARY:
    case FTP_CLIENT_STATE_PREPARE_GET_BIN:
    case FTP_CLIENT_STATE_PREPARE_GET_ASCII:
        break;
    default:
        iEventNumber |= FTP_CLIENT_EVENT_FLAG_PUT_DIRECTION;             // the data direction is from FTP client to server (we send)
        break;
    }
    return iEventNumber;                                                 // the event uses flag to indicate what type fo transfer is to be made
}
#endif


static int fnHandleType(TCP_CLIENT_MESSAGE_BOX *ptrCallback_message_box) // {3}
{
    unsigned char ucMessage = (unsigned char)(FTP_CLIENT_MSG_SET_PSV_LST + (ucFTP_client_state - FTP_CLIENT_STATE_LIST_AFTER_ASCII));
    if ((ucFTP_client_state >= FTP_CLIENT_STATE_RENAME_AFTER_ASCII) && (ucFTP_client_state <= FTP_CLIENT_STATE_PATH_AFTER_ASCII)) { // exceptions that don't use a data connection
        return (fnSendClientFTP((unsigned char)(FTP_CLIENT_MSG_RENAME + (ucFTP_client_state - FTP_CLIENT_STATE_RENAME_AFTER_ASCII)))); // request the action
    }
    ptrCallback_message_box->iCallbackEvent = FTP_CLIENT_EVENT_ACTIVE_PASSIVE_LIST; // ask the application whether is wants to transfer data in active or passive mode
    if (0 == fnFTP_client_user_callback(ptrCallback_message_box)) { // request the user whether we should execute the action in active or passive mode
        return (fnSendClientFTP(ucMessage));                 // request passive mode before performing the appropriate action
    }
    else {                                                   // perform action in active mode
        FTP_TCP_data_port = fnGetFreeTCP_Port();             // get a tcp port for use by the data connection - the FTP server will contact us in this
#if defined FTP_CLIENT_EXTERN_DATA_SOCKET
        ptrCallback_message_box->iCallbackEvent = fnGenerateEventNumber(ucFTP_client_state); // inform that the user should listen on the data port number
        ptrCallback_message_box->usDataPort = FTP_TCP_data_port;
        ptrCallback_message_box->usDataLength = ucFTP_client_state; // the reason for the transfer that will take place
        ptrCallback_message_box->uControlSocket = FTP_Client_TCP_socket; // inform the external data socket application of the control socket number
        if (fnFTP_client_user_callback(ptrCallback_message_box) < 0) { // pass details of data port to application who should start listening on it using its own socket
            ucFTP_client_state = FTP_CLIENT_STATE_LOGGED_IN;
            return APP_ACCEPT;
        }
#else
        fnTCP_Listen(FTP_TCP_client_Data_socket, FTP_TCP_data_port, 0); // start listening on the port
#endif
        if (ucMessage >= FTP_CLIENT_MSG_SET_PSV_APP_BIN) {   // special case for binary data transfers
            ucMessage -= (FTP_CLIENT_STATE_PREPARE_APP_BIN - FTP_CLIENT_STATE_PREPARE_APP_ASCII);
        }
        ucMessage += (unsigned char)(FTP_CLIENT_MSG_ACTIVE_LIST - FTP_CLIENT_MSG_SET_PSV_LST);
        return (fnSendClientFTP(ucMessage));                 // request the action
    }
}

static int fnPassiveModeAccepted(unsigned long ulFlags, unsigned char *ucIp_Data, unsigned short usPortLen, TCP_CLIENT_MESSAGE_BOX *ptrCallback_message_box) // {4}
{
#if !defined FTP_CLIENT_EXTERN_DATA_SOCKET
    #if defined USE_IPV6
    register unsigned long Flags = ulFlags;
    #else
    register unsigned short Flags = 0;
    #endif
#endif
    unsigned char *ptrIP_address;
    ucIp_Data += 3;
    usPortLen -= 3;
#if defined USE_IPV6
    if (Flags & TCP_CONNECT_IPV6) {                                      // response is from EPSV - Expected (|||decimal_port_number|)
        int iPortFound = 0;
        while (usPortLen--) {
            switch (iPortFound) {
            case 0:
                if (*ucIp_Data == '(') {
                    iPortFound = 1;
                }
                break;
            case 1:
            case 2:
            case 3:
                if (*ucIp_Data == '|') {
                    iPortFound++;
                }
                break;
            case 4:
                ptrCallback_message_box->usDataPort = (unsigned short)fnDecStrHex((CHAR *)ucIp_Data);
                iPortFound++;
                break;
            default:
                break;
            }
            ucIp_Data++;
        }
        ptrIP_address = ucServerIPv6_address;                            // the data connection is always made to the same address as the command connection
#if defined FTP_CLIENT_EXTERN_DATA_SOCKET
        ptrCallback_message_box->ptrIPv6Address = ucServerIPv6_address;
#endif
    }
    else {
        ptrCallback_message_box->usDataPort = fnExtractPassivePort((CHAR *)ucIp_Data, ptrCallback_message_box->ucIP_data_address, usPortLen); // extract the IP address and port number to be used for the data connection
        ptrIP_address = ptrCallback_message_box->ucIP_data_address;
#if defined FTP_CLIENT_EXTERN_DATA_SOCKET
        ptrCallback_message_box->ptrIPv6Address = 0;                     // IPv6 address pointer is zero when the connection is over IPv4
#endif
    }
#else
    ptrCallback_message_box->usDataPort = fnExtractPassivePort((CHAR *)ucIp_Data, ptrCallback_message_box->ucIP_data_address, usPortLen); // extract the IP address and port number to be used for the data connection
    ptrIP_address = ptrCallback_message_box->ucIP_data_address;
#endif
#if defined FTP_CLIENT_EXTERN_DATA_SOCKET
    ptrCallback_message_box->iCallbackEvent = fnGenerateEventNumber(ucFTP_client_state); // inform that the user should make a connection to the server's data channel
    ptrCallback_message_box->uControlSocket = FTP_Client_TCP_socket;     // inform the external data socket application of the control socket number
    if (fnFTP_client_user_callback(ptrCallback_message_box) < 0) {       // pass details of data port to application who should connect to it using its own socket
        ucFTP_client_state = FTP_CLIENT_STATE_LOGGED_IN;
        return APP_ACCEPT;
    }
#else
    if (fnTCP_Connect(FTP_TCP_client_Data_socket, ptrIP_address, ptrCallback_message_box->usDataPort, 0, Flags) < 0) { // start open of data connection
        ptrCallback_message_box->iCallbackEvent = FTP_CLIENT_EVENT_DATA_CONNECTION_FAILED;
        ucFTP_client_state = FTP_CLIENT_STATE_LOGGED_IN;
        fnFTP_client_user_callback(ptrCallback_message_box);
        return APP_ACCEPT;
    }
#endif
    if (ucFTP_client_state >= FTP_CLIENT_STATE_PREPARE_APP_BIN) {        // special case for binary transfers
        ucFTP_client_state -= (FTP_CLIENT_STATE_PREPARE_APP_BIN - FTP_CLIENT_STATE_PREPARE_APP_ASCII); // from here on the state is the same for GET, PUT and APPEND whether ASCII or BINARY 
    }
    return (fnSendClientFTP((unsigned char)(FTP_CLIENT_MSG_LIST + (ucFTP_client_state - FTP_CLIENT_STATE_PREPARE_LST)))); // request the action
}

#if !defined FTP_CLIENT_EXTERN_DATA_SOCKET                               // {6}
static int fnCloseDataConnection(void)
{
    if (iDataConnection == DATA_CONNECTION_CONNECTED) {                  // if the data connection is open
        fnTCP_close(FTP_TCP_client_Data_socket);                         // command close of data connection
        return APP_REQUEST_CLOSE;
    }
    return APP_ACCEPT;
}
#endif

// Local listener on TCP FTP command port
//
static int fnFTP_client_ControlListener(USOCKET Socket, unsigned char ucEvent, unsigned char *ucIp_Data, unsigned short usPortLen)
{
    TCP_CLIENT_MESSAGE_BOX callback_message_box;
    if (Socket != FTP_Client_TCP_socket) {
        return APP_REJECT;                                               // ignore if not our socket handle
    }

    callback_message_box.ptrData = ucIp_Data;
    callback_message_box.usDataLength = usPortLen;

    switch (ucEvent) {
    case TCP_EVENT_CONNECTED:
        ucFTP_client_state = FTP_CLIENT_STATE_CONNECTED;                 // we are now connected but not yet logged in
        iServerType = SERVER_TYPE_UNKNOWN;                               // {3}
        break;
    case TCP_EVENT_ACK:
        break;

    case TCP_EVENT_ABORT:
    case TCP_EVENT_CLOSED:
        ucFTP_client_state = FTP_CLIENT_STATE_CLOSED;
        callback_message_box.iCallbackEvent = FTP_CLIENT_EVENT_CONNECTION_CLOSED;
        fnFTP_client_user_callback(&callback_message_box);               // inform that the connection has been closed
        break;

    case TCP_EVENT_DATA:                                                 // we have new receive data
        /*fnDebugMsg("STATE = ");
        fnDebugDec(ucFTP_client_state, 0);
        fnDebugMsg("\r\n");*/
        switch (ucFTP_client_state) {
        case FTP_CLIENT_STATE_CONNECTED:                                 // just connected
            if (uMemcmp(ucIp_Data, "220", 3) == 0) {                     // 220 is the OK response from the FTP server
                return (fnSendClientFTP(FTP_CLIENT_MSG_USER));           // we send our user name
            }
            break;
        case FTP_CLIENT_STATE_LOGIN_USER:                            
            if (uMemcmp(ucIp_Data, "331", 3) == 0) {                     // 331 is the password request from the FTP server
                return (fnSendClientFTP(FTP_CLIENT_MSG_USER_PASS));      // we send our password
            }
            else {                                                       // error or rejected
                callback_message_box.iCallbackEvent = FTP_CLIENT_USER_NAME_ERROR;
                fnFTP_client_user_callback(&callback_message_box);
            }
            break;
        case FTP_CLIENT_STATE_LOGIN_PASS:
            if (uMemcmp(ucIp_Data, "230", 3) == 0) {                     // 230 is confirmation that our login was successful
                ucFTP_client_state = FTP_CLIENT_STATE_LOGGED_IN;
                callback_message_box.iCallbackEvent = FTP_CLIENT_EVENT_LOGGED_IN;
                return (fnFTP_client_user_callback(&callback_message_box)); // inform that we have been successfully logged in
            }
            else {                                                       // error or rejected
                callback_message_box.iCallbackEvent = FTP_CLIENT_USER_PASS_ERROR;
                fnFTP_client_user_callback(&callback_message_box);                
            }
            break;
        case FTP_CLIENT_STATE_RENAMING:
            if (uMemcmp(ucIp_Data, "350", 3) == 0) {                     // 350 is indicating that the FTP server has found the file and now needs the new name of it
                fnEnterPath(ptrRenameTo, "RNTO");                        // create the rename to message from the original with from and to
                return (fnSendClientFTP(FTP_CLIENT_MSG_RENAME_2));       // inform of the new name to be set
            }
            else {                                                       // error
                ucFTP_client_state = FTP_CLIENT_STATE_LOGGED_IN;
                callback_message_box.iCallbackEvent = FTP_CLIENT_ERROR_RENAMING_FILE;
                fnFTP_client_user_callback(&callback_message_box);                
            }
            break;
        case FTP_CLIENT_STATE_CREATING:
            if (uMemcmp(ucIp_Data, "257", 3) == 0) {                     // 257 is confirmation that the FTP server has successfully created the directory
                ucFTP_client_state = FTP_CLIENT_STATE_LOGGED_IN;
                callback_message_box.iCallbackEvent = FTP_CLIENT_EVENT_DIR_CREATED;
                return (fnFTP_client_user_callback(&callback_message_box)); // inform that the new directory has been successfully created
            }
            else {                                                       // error
                ucFTP_client_state = FTP_CLIENT_STATE_LOGGED_IN;
                callback_message_box.iCallbackEvent = FTP_CLIENT_ERROR_CREATING_DIRECTORY;
                fnFTP_client_user_callback(&callback_message_box);                
            }
            break;
        case FTP_CLIENT_STATE_RENAMING_2:
        case FTP_CLIENT_STATE_DELETING:
        case FTP_CLIENT_STATE_REMOVING:
        case FTP_CLIENT_STATE_SETTING:
            if (uMemcmp(ucIp_Data, "250", 3) == 0) {                     // 250 is confirmation that the FTP server has successfully relocated, renamed or deleted                
                switch (ucFTP_client_state) {
                case FTP_CLIENT_STATE_REMOVING:
                    callback_message_box.iCallbackEvent = FTP_CLIENT_EVENT_DIR_DELETED;
                    break;
                case FTP_CLIENT_STATE_DELETING:
                    callback_message_box.iCallbackEvent = FTP_CLIENT_EVENT_FILE_DELETED;
                    break;
                case FTP_CLIENT_STATE_RENAMING_2:
                    callback_message_box.iCallbackEvent = FTP_CLIENT_EVENT_RENAMED;
                    break;
                case FTP_CLIENT_STATE_SETTING:
                    callback_message_box.iCallbackEvent = FTP_CLIENT_EVENT_LOCATION_SET;
                    break;
                }
            }
            else {                                                       // error  
                switch (ucFTP_client_state) {                
                case FTP_CLIENT_STATE_REMOVING:
                    callback_message_box.iCallbackEvent = FTP_CLIENT_ERROR_DELETING_DIR;
                    break;
                case FTP_CLIENT_STATE_DELETING:
                    callback_message_box.iCallbackEvent = FTP_CLIENT_ERROR_DELETING_FILE;
                    break;
                case FTP_CLIENT_STATE_RENAMING_2:
                    callback_message_box.iCallbackEvent = FTP_CLIENT_ERROR_RENAMING_FILE;
                    break;
                case FTP_CLIENT_STATE_SETTING:
                    callback_message_box.iCallbackEvent = FTP_CLIENT_ERROR_SETTING_LOCATION;
                    break;
                }
            }
            ucFTP_client_state = FTP_CLIENT_STATE_LOGGED_IN;
            return (fnFTP_client_user_callback(&callback_message_box));  // inform of the result

        case FTP_CLIENT_STATE_LIST_AFTER_ASCII:                          // trying to set the FTP server to ascii mode - with data connection
        case FTP_CLIENT_STATE_RENAME_AFTER_ASCII:                        // no data connection
        case FTP_CLIENT_STATE_DELETE_AFTER_ASCII:                        // no data connection
        case FTP_CLIENT_STATE_REMOVE_AFTER_ASCII:                        // no data connection
        case FTP_CLIENT_STATE_MKDIR_AFTER_ASCII:                         // no data connection
        case FTP_CLIENT_STATE_PATH_AFTER_ASCII:                          // no data connection
        case FTP_CLIENT_STATE_APP_AFTER_ASCII:                           // with data connection
        case FTP_CLIENT_STATE_PUT_AFTER_ASCII:                           // with data connection
        case FTP_CLIENT_STATE_GET_AFTER_ASCII:                           // with data connection
        case FTP_CLIENT_STATE_APP_AFTER_BINARY:                          // trying to set the FTP server to binary mode - with data connection
        case FTP_CLIENT_STATE_PUT_AFTER_BINARY:                          // with data connection
        case FTP_CLIENT_STATE_GET_AFTER_BINARY:                          // with data connection
            if (uMemcmp(ucIp_Data, "200", 3) == 0) {                     // 200 is confirmation that the FTP server has moved to ascii/binary mode
                if (ucFTP_client_state < FTP_CLIENT_STATE_APP_AFTER_BINARY) { // {3}
                    iServerType = SERVER_TYPE_ASCII;                     // is is known that we are in ASCII mode
                }
                else {
                    iServerType = SERVER_TYPE_BINARY;                    // is is known that we are in binary mode
                }
                return (fnHandleType(&callback_message_box));            // {3}
            }
            else {                                                       // error
                callback_message_box.iCallbackEvent = FTP_CLIENT_ERROR_SETTING_MODE;
                ucFTP_client_state = FTP_CLIENT_STATE_LOGGED_IN;         // {1}
                fnFTP_client_user_callback(&callback_message_box);
            }
            break;
        case FTP_CLIENT_STATE_PREPARE_ACT_LST:
        case FTP_CLIENT_STATE_PREPARE_ACT_APP:
        case FTP_CLIENT_STATE_PREPARE_ACT_PUT:
        case FTP_CLIENT_STATE_PREPARE_ACT_GET:
            if (uMemcmp(ucIp_Data, "200", 3) == 0) {                     // 200 is confirmation that the FTP server has accepted the port command
                return (fnSendClientFTP((unsigned char)(FTP_CLIENT_MSG_LIST + (ucFTP_client_state - FTP_CLIENT_STATE_PREPARE_ACT_LST)))); // request the action
            }
            else {                                                       // error
                callback_message_box.iCallbackEvent = FTP_CLIENT_ERROR_SETTING_PORT;
                ucFTP_client_state = FTP_CLIENT_STATE_LOGGED_IN;
                fnFTP_client_user_callback(&callback_message_box);
            }
            break;
        case FTP_CLIENT_STATE_PREPARE_LST:
        case FTP_CLIENT_STATE_PREPARE_APP_ASCII:
        case FTP_CLIENT_STATE_PREPARE_PUT_ASCII:
        case FTP_CLIENT_STATE_PREPARE_GET_ASCII:
        case FTP_CLIENT_STATE_PREPARE_APP_BIN:
        case FTP_CLIENT_STATE_PREPARE_PUT_BIN:
        case FTP_CLIENT_STATE_PREPARE_GET_BIN:
            if (uMemcmp(ucIp_Data, "227", 3) == 0) {                     // 227 is confirmation that the FTP server has accepted passive mode and the port number is in the message
                return (fnPassiveModeAccepted(0, ucIp_Data, usPortLen, &callback_message_box));// {4}
            }
    #if defined USE_IPV6                                                 // {4}
            else if (uMemcmp(ucIp_Data, "229", 3) == 0) {                // passive mode accepted over IPv6 so the response contains IPv6 information
                return (fnPassiveModeAccepted(TCP_CONNECT_IPV6, ucIp_Data, usPortLen, &callback_message_box));
            }
    #endif
            else {                                                       // error
                callback_message_box.iCallbackEvent = FTP_CLIENT_ERROR_PASSIVE_MODE;
                ucFTP_client_state = FTP_CLIENT_STATE_LOGGED_IN;
                fnFTP_client_user_callback(&callback_message_box);
            }
            break;
        case FTP_CLIENT_STATE_LISTING:
        case FTP_CLIENT_STATE_GETTING:
            if (uMemcmp(ucIp_Data, "125", 3) == 0) {                     // {5} "125" indicates that the data connection is already open and the server will now send data
                break;
            }
        case FTP_CLIENT_STATE_APPENDING:
        case FTP_CLIENT_STATE_PUTTING:                                  
            if (uMemcmp(ucIp_Data, "150", 3) == 0) { // FTP server informing that the data transfer will start
                if (ucFTP_client_state == FTP_CLIENT_STATE_PUTTING) {
                    callback_message_box.iCallbackEvent = FTP_CLIENT_EVENT_PUT_CAN_START;
                }
                else if (ucFTP_client_state == FTP_CLIENT_STATE_APPENDING) {
                    callback_message_box.iCallbackEvent = FTP_CLIENT_EVENT_APPEND_CAN_START;
                }
                else {
                    break;
                }
    #if !defined FTP_CLIENT_EXTERN_DATA_SOCKET
                callback_message_box.uDataSocket = FTP_TCP_client_Data_socket;
    #endif
                return (fnFTP_client_user_callback(&callback_message_box)); // inform the user of result
            }
            else if (uMemcmp(ucIp_Data, "226", 3) == 0) {                // FTP server informing that the data transfer has successfully terminated
                switch (ucFTP_client_state) {
                    case FTP_CLIENT_STATE_LISTING:
    #if !defined FTP_CLIENT_EXTERN_DATA_SOCKET
                        if (iDataConnection != DATA_CONNECTION_CLOSED) { // {2} if the data connection has not terminated we delay completion
                            iDataConnection = WAITING_LISTING_CLOSE;
                            return APP_ACCEPT;
                        }
    #endif
                        callback_message_box.iCallbackEvent = FTP_CLIENT_EVENT_LISTING_DATA_COMPLETE;
                        break;
                    case FTP_CLIENT_STATE_APPENDING:
                        callback_message_box.iCallbackEvent = FTP_CLIENT_EVENT_APPEND_DATA_COMPLETE;
                        break;
                    case FTP_CLIENT_STATE_PUTTING:
                        callback_message_box.iCallbackEvent = FTP_CLIENT_EVENT_PUT_DATA_COMPLETE;
                        break;
                    case FTP_CLIENT_STATE_GETTING:
    #if !defined FTP_CLIENT_EXTERN_DATA_SOCKET
                        if (iDataConnection != DATA_CONNECTION_CLOSED) { // {2}  if the data connection has not terminated we delay completion
                            iDataConnection = WAITING_GETTING_CLOSE;
                            return APP_ACCEPT;
                        }
    #endif
                        callback_message_box.iCallbackEvent = FTP_CLIENT_EVENT_GET_DATA_COMPLETE;
                        break;
                }
            }
            else {                                                       // error
                switch (ucFTP_client_state) {
                    case FTP_CLIENT_STATE_LISTING:
                        callback_message_box.iCallbackEvent = FTP_CLIENT_ERROR_LISTING;
                        break;
                    case FTP_CLIENT_STATE_APPENDING:
                        callback_message_box.iCallbackEvent = FTP_CLIENT_ERROR_APPENDING;
                        break;
                    case FTP_CLIENT_STATE_PUTTING:
                        callback_message_box.iCallbackEvent = FTP_CLIENT_ERROR_PUTTING;
                        break;
                    case FTP_CLIENT_STATE_GETTING: 
                        callback_message_box.iCallbackEvent = FTP_CLIENT_ERROR_GETTING;
                        break;
                }
    #if !defined FTP_CLIENT_EXTERN_DATA_SOCKET                           // {6}
                fnCloseDataConnection();
    #endif
            }
            ucFTP_client_state = FTP_CLIENT_STATE_LOGGED_IN;             // allow further operations
            return (fnFTP_client_user_callback(&callback_message_box));  // inform the user of result

        case FTP_CLIENT_STATE_TERMINATING:
            if (uMemcmp(ucIp_Data, "221", 3) == 0) {                     // FTP server informing that it will close the connection (we ignore it)
            }
            break;
        }
        break;

    case TCP_EVENT_REGENERATE:                                           // we must repeat the last control buffer we sent
        return (fnSendClientFTP(FTP_CLIENT_MSG_REPEAT_CONTROL));         // repeat the lost control request that was sent

    case TCP_EVENT_CONREQ:                                               // client never accepts connection requests
        return APP_REJECT;

    case TCP_EVENT_CLOSE:                                                // the FTP server requests that the connection be closed
        break;

    default:
        break;
    }
    return APP_ACCEPT;
}


#if !defined FTP_CLIENT_EXTERN_DATA_SOCKET                               // if the user supplies its own data connection this is not used
// Local listener on TCP FTP data port
//
static int fnFTP_Data_Listener(USOCKET Socket, unsigned char ucEvent, unsigned char *ucIp_Data, unsigned short usPortLen)
{
    TCP_CLIENT_MESSAGE_BOX callback_message_box;
    if (Socket != FTP_TCP_client_Data_socket) {
        return APP_REJECT;                                               // ignore if not our socket handle
    }
    fnTCP_Activity(FTP_Client_TCP_socket);                               // retrigger control socket on data socket activity

    callback_message_box.uDataSocket  = Socket;
    callback_message_box.usDataPort   = usPortLen;
    callback_message_box.ptrData      = ucIp_Data;
    callback_message_box.usDataLength = usPortLen;

    switch (ucEvent) {
    case TCP_EVENT_CONNECTED:
        if ((ucFTP_client_state == FTP_CLIENT_STATE_LISTING) || (ucFTP_client_state == FTP_CLIENT_STATE_GETTING)) {
            iDataConnection = DATA_CONNECTION_CONNECTED;                 // {2} remember that the data connection is connected
            break;                                                       // quietly accept connection when listing or getting
        }
        callback_message_box.iCallbackEvent = FTP_CLIENT_EVENT_DATA_CONNECTED;
        return (fnFTP_client_user_callback(&callback_message_box));      // inform the user that data can now be sent (append or put)

    #if defined FTP_CLIENT_BUFFERED_SOCKET_MODE && defined SUPPORT_PEER_WINDOW
    case TCP_EVENT_PARTIAL_ACK:                                          // possible ack to a part of a transmission received
        if (fnSendBufTCP(Socket, 0, usPortLen, TCP_BUF_NEXT)) {          // send next buffered (if waiting)
            return APP_SENT_DATA;
        }
        break;
    #endif

    case TCP_EVENT_ACK:
        {
            int iReturn = 0;
    #ifdef FTP_CLIENT_BUFFERED_SOCKET_MODE
            if (fnSendBufTCP(Socket, 0, 0, TCP_BUF_NEXT)) {              // send next buffered (if waiting)
                iReturn = APP_SENT_DATA;                                 // mark that data has been transmitted
            }
    #endif
            if ((ucFTP_client_state == FTP_CLIENT_STATE_LISTING) || (ucFTP_client_state == FTP_CLIENT_STATE_GETTING)) {
                break;                                                   // quietly accept acks when listing or getting
            }
            if (iReturn == 0) {
                callback_message_box.iCallbackEvent = FTP_CLIENT_EVENT_DATA_SENT;
                iReturn |= fnFTP_client_user_callback(&callback_message_box);// inform the user that all transmitted data has been acknowledged
                if (iReturn & APP_REQUEST_CLOSE) {                       // if application request a close
                    fnTCP_close(Socket);                                 // terminate the connection
                    return APP_REQUEST_CLOSE;
                }
            }
            return (iReturn);
        }

    case TCP_EVENT_DATA:                                                 // we have new receive data
        switch (ucFTP_client_state) { 
        case FTP_CLIENT_STATE_LISTING:
            callback_message_box.iCallbackEvent = FTP_CLIENT_EVENT_LISTING_DATA;
            break;
        case FTP_CLIENT_STATE_GETTING:
            callback_message_box.iCallbackEvent = FTP_CLIENT_EVENT_GET_DATA;
            break;
        default:
            return APP_ACCEPT;
        }
        return (fnFTP_client_user_callback(&callback_message_box));      // pass on the received data to the user

    case TCP_EVENT_REGENERATE:                                           // we must repeat the last data buffer we sent
    #ifdef FTP_CLIENT_BUFFERED_SOCKET_MODE
        if (fnSendBufTCP(Socket, 0, 0, TCP_BUF_REP) != 0) {              // repeat send buffered
        #ifdef SUPPORT_PEER_WINDOW 
            fnSendBufTCP(Socket, 0, 0, (TCP_BUF_NEXT | TCP_BUF_KICK_NEXT)); // kick off any following data as long as windowing allows it
        #endif
            return APP_SENT_DATA;
        }
        break;
    #else
        callback_message_box.iCallbackEvent = FTP_CLIENT_EVENT_DATA_LOST;
        return (fnFTP_client_user_callback(&callback_message_box));
    #endif

    case TCP_EVENT_CLOSE:                                                // remote client wants to close
        break;
    case TCP_EVENT_ABORT:
    case TCP_EVENT_CLOSED:
        if ((ucFTP_client_state == FTP_CLIENT_STATE_LISTING) || (ucFTP_client_state == FTP_CLIENT_STATE_GETTING)) {
            if (iDataConnection == WAITING_GETTING_CLOSE) {              // {2}
                callback_message_box.iCallbackEvent = FTP_CLIENT_EVENT_GET_DATA_COMPLETE;
                ucFTP_client_state = FTP_CLIENT_STATE_LOGGED_IN;         // allow further operations
                fnFTP_client_user_callback(&callback_message_box);
            }
            else if (iDataConnection == WAITING_LISTING_CLOSE) {          // {2}
                callback_message_box.iCallbackEvent = FTP_CLIENT_EVENT_LISTING_DATA_COMPLETE;
                ucFTP_client_state = FTP_CLIENT_STATE_LOGGED_IN;         // allow further operations
                fnFTP_client_user_callback(&callback_message_box);
            }
            iDataConnection = DATA_CONNECTION_CLOSED;                    // flag that the data connection has been closed
            break;                                                       // quietly accept disconnection when listing or getting
        }
        iDataConnection = DATA_CONNECTION_CLOSED;
        callback_message_box.iCallbackEvent = FTP_CLIENT_EVENT_DATA_DISCONNECTED;
        return (fnFTP_client_user_callback(&callback_message_box));

    case TCP_EVENT_CONREQ:
        break;

    default:
        break;
    }
    return APP_ACCEPT;
}
#endif


// Send a control message or regenerate a control message
//
static signed short fnSendClientFTP(unsigned char ucMsg)
{
    unsigned short         usSize;
    const CHAR             *ptrMsg = 0;
    TCP_CLIENT_MESSAGE_BOX callback_message_box;
    TCP_FTP_CLIENT_MESSAGE FTP_Data_Tx;                                  // space for tcp data

    if (FTP_CLIENT_MSG_DO_NOTHING == ucMsg) {
        return 0;
    }

    if (ucMsg == FTP_CLIENT_MSG_REPEAT_CONTROL) {                        // handle repetitions
        ucMsg = ucLastControl;                                           // automatically regenerate last control message
    }
    else {
        ucLastControl = ucMsg;
    }

    switch (ucMsg) {                                                     // the message to be constructed / regenerated
    case FTP_CLIENT_MSG_USER:                                            // send the user name
    case FTP_CLIENT_MSG_USER_PASS:                                       // send the user password
        {
            CHAR *ptrUser;
            CHAR *ptrEnd = 0;
            if (ucMsg == FTP_CLIENT_MSG_USER) {
                uMemcpy(FTP_Data_Tx.ucTCP_Message, "USER ", 5);
                ucFTP_client_state = FTP_CLIENT_STATE_LOGIN_USER;        // logging in (sending user name)
                callback_message_box.iCallbackEvent = FTP_CLIENT_EVENT_REQUEST_FTP_USER_NAME;
            }
            else {
                uMemcpy(FTP_Data_Tx.ucTCP_Message, "PASS ", 5);
                ucFTP_client_state = FTP_CLIENT_STATE_LOGIN_PASS;        // logging in (sending user password)
                callback_message_box.iCallbackEvent = FTP_CLIENT_EVENT_REQUEST_FTP_USER_PASSWORD;
            }
            fnFTP_client_user_callback(&callback_message_box);           // get user name or password
            ptrUser = (CHAR *)callback_message_box.ptrData;
            if (ptrUser != 0) {
                ptrEnd = uStrcpy((CHAR *)&FTP_Data_Tx.ucTCP_Message[5], ptrUser);
                if (ptrEnd == (CHAR *)&FTP_Data_Tx.ucTCP_Message[5]) {   // user name has no length
                    ptrEnd = 0;
                }
            }
            if (ptrEnd == 0) {                                           // no user name or password so attempt to log in anonymously
                ptrEnd = uStrcpy((CHAR *)&FTP_Data_Tx.ucTCP_Message[5], "ANONYMOUS");
            }
            *ptrEnd++ = CARRIAGE_RETURN;
            *ptrEnd++ = LINE_FEED;
            usSize = (ptrEnd - (CHAR *)FTP_Data_Tx.ucTCP_Message);
        }
        break;

    case FTP_CLIENT_MSG_SET_PSV_LST:                                     // command that the action be performed in passive mode (this is always in ascii mode)
    case FTP_CLIENT_MSG_SET_PSV_RENAME:
    case FTP_CLIENT_MSG_SET_PSV_DELETE_FILE:
    case FTP_CLIENT_MSG_SET_PSV_REMOVE_DIR:
    case FTP_CLIENT_MSG_SET_PSV_MAKE_DIR:
  //case FTP_CLIENT_MSG_SET_PSV_PATH:
    case FTP_CLIENT_MSG_SET_PSV_APP_ASCII:
    case FTP_CLIENT_MSG_SET_PSV_PUT_ASCII:
    case FTP_CLIENT_MSG_SET_PSV_GET_ASCII:
    case FTP_CLIENT_MSG_SET_PSV_APP_BIN:
    case FTP_CLIENT_MSG_SET_PSV_PUT_BIN:
    case FTP_CLIENT_MSG_SET_PSV_GET_BIN:
        ucFTP_client_state = (FTP_CLIENT_STATE_PREPARE_LST + (ucMsg - FTP_CLIENT_MSG_SET_PSV_LST)); // move to next state
    #if defined USE_IPV6                                                 // {4}
        if (iServerIPv6 != 0) {                                          // if not IPv4 connection
            usSize = (sizeof(cFTP_EPSV) - 1);                            // extended passive mode when over IPv6
            ptrMsg = cFTP_EPSV;
            break;
        }
    #endif
        usSize = (sizeof(cFTP_PASV) - 1);
        ptrMsg = cFTP_PASV;
        break;

    case FTP_CLIENT_MSG_ACTIVE_LIST:                                     // send the PORT command 
    case FTP_CLIENT_MSG_ACTIVE_RENAME:
    case FTP_CLIENT_MSG_ACTIVE_DELETE_FILE:
    case FTP_CLIENT_MSG_ACTIVE_REMOVE_DIR:
    case FTP_CLIENT_MSG_ACTIVE_MAKE_DIR:
  //case FTP_CLIENT_MSG_ACTIVE_PATH:
    case FTP_CLIENT_MSG_ACTIVE_APP:
    case FTP_CLIENT_MSG_ACTIVE_PUT:
    case FTP_CLIENT_MSG_ACTIVE_GET:
        {
            CHAR *ptrEnd;
    #if defined USE_IPV6                                                 // {4}
            if (iServerIPv6 != 0) {
                uMemcpy(FTP_Data_Tx.ucTCP_Message, "EPRT |2|", 8);       // extended port when used over IPv6 (format is |2|2001:470:21:105::10|4234|)
                if (iServerIPv6 & FTP_CLIENT_IPV6_LINK_LOCAL) {          // if we are dealing with a connection on the link local we use the link local IPv6 address
                    ptrEnd = fnIPV6Str(ucLinkLocalIPv6Address[DEFAULT_NETWORK], (CHAR *)&FTP_Data_Tx.ucTCP_Message[8]);
                }
                else {                                                   // else we use the global IPv6 address
                    ptrEnd = fnIPV6Str(network[DEFAULT_NETWORK].ucOurIPV6, (CHAR *)&FTP_Data_Tx.ucTCP_Message[8]);
                }
                *ptrEnd++ = '|';
                ptrEnd = fnBufferDec(FTP_TCP_data_port, 0, ptrEnd);
                *ptrEnd++ = '|';
            }
            else {
    #endif
                uMemcpy(FTP_Data_Tx.ucTCP_Message, "PORT ", 5);
                ptrEnd = fnBufferDec(network[DEFAULT_NETWORK].ucOurIP[0], 0, (CHAR *)&FTP_Data_Tx.ucTCP_Message[5]);
                *ptrEnd++ = ',';
                ptrEnd = fnBufferDec(network[DEFAULT_NETWORK].ucOurIP[1], 0, ptrEnd);
                *ptrEnd++ = ',';
                ptrEnd = fnBufferDec(network[DEFAULT_NETWORK].ucOurIP[2], 0, ptrEnd);
                *ptrEnd++ = ',';
                ptrEnd = fnBufferDec(network[DEFAULT_NETWORK].ucOurIP[3], 0, ptrEnd);
                *ptrEnd++ = ',';
                ptrEnd = fnBufferDec((unsigned char)(FTP_TCP_data_port >> 8), 0, ptrEnd);
                *ptrEnd++ = ',';
                ptrEnd = fnBufferDec((unsigned char)(FTP_TCP_data_port), 0, ptrEnd);
    #if defined USE_IPV6
            }
    #endif
            *ptrEnd++ = CARRIAGE_RETURN;
            *ptrEnd++ = LINE_FEED;
            usSize = (ptrEnd - (CHAR *)FTP_Data_Tx.ucTCP_Message);
            ucFTP_client_state = (FTP_CLIENT_STATE_PREPARE_ACT_LST + (ucMsg - FTP_CLIENT_MSG_ACTIVE_LIST)); // move to next state
        }
        break;

    case FTP_CLIENT_MSG_LIST:                                            // request that the server sends a directory listing (always in ascii mode)
    case FTP_CLIENT_MSG_RENAME:
    case FTP_CLIENT_MSG_DELETE:
    case FTP_CLIENT_MSG_REMOVE:
    case FTP_CLIENT_MSG_MAKE:
    case FTP_CLIENT_MSG_PATH:
    case FTP_CLIENT_MSG_APP:                                             // request that the action starts, whereby the command has been prepared together with any additional directory information (ascii or binary mode)
    case FTP_CLIENT_MSG_PUT:
    case FTP_CLIENT_MSG_GET:
    case FTP_CLIENT_MSG_RENAME_2:
        {
            CHAR *ptrEnd;
            if (FTP_CLIENT_MSG_RENAME == ucMsg) {                        // this is a special case with old and new name in the string - we don't copy the new name yet
                CHAR *ptrPath = dir_location;
                ptrEnd = (CHAR *)FTP_Data_Tx.ucTCP_Message;
                while (*ptrPath != 0) {
                    if (ptrPath > &dir_location[5]) {                    // ignore command
                        if (*ptrPath == ' ') {                           // the next space found is assumed to be the end of the first name
                            ptrRenameTo = (ptrPath + 1);                 // remember the location of the rename-to string
                            break;
                        }
                    }
                    *ptrEnd++ = *ptrPath++;
                }
                *ptrEnd++ = CARRIAGE_RETURN;
                *ptrEnd++ = LINE_FEED;
                *ptrEnd = 0;                                             // terminate
                ptrPath = ptrRenameTo;
                while (*ptrPath != 0) {                                  // now strip off carriage return and line feed from end of original message since it will be added again later
                    if (*ptrPath == CARRIAGE_RETURN) {
                        *ptrPath = 0;
                        break;
                    }
                    ptrPath++;
                }
            }
            else {
                ptrEnd = uStrcpy((CHAR *)FTP_Data_Tx.ucTCP_Message, dir_location); // copy the prepared command with path
            }
            usSize = (ptrEnd - (CHAR *)FTP_Data_Tx.ucTCP_Message);
            ucFTP_client_state = (FTP_CLIENT_STATE_LISTING + (ucMsg - FTP_CLIENT_MSG_LIST)); // move to next state
        }
        break;

    case FTP_CLIENT_MSG_SET_ASCII_LST:                                   // send TYPE A to set ascii mode
    case FTP_CLIENT_MSG_SET_ASCII_RENAME:
    case FTP_CLIENT_MSG_SET_ASCII_DELETE_FILE:
    case FTP_CLIENT_MSG_SET_ASCII_REMOVE_DIR:
    case FTP_CLIENT_MSG_SET_ASCII_MAKE_DIR:
    case FTP_CLIENT_MSG_SET_ASCII_PATH:
    case FTP_CLIENT_MSG_SET_ASCII_APP:
    case FTP_CLIENT_MSG_SET_ASCII_PUT:
    case FTP_CLIENT_MSG_SET_ASCII_GET:
        ucFTP_client_state = (FTP_CLIENT_STATE_LIST_AFTER_ASCII + (ucMsg - FTP_CLIENT_MSG_SET_ASCII_LST)); // set next state
        if (iServerType == SERVER_TYPE_ASCII) {                          // {3} the server is already operating in ASCII mode so we can jump the first step
            return (fnHandleType(&callback_message_box));
        }
        usSize = (sizeof(cFTP_TYPE_A) - 1);
        ptrMsg = cFTP_TYPE_A;
        break;

    case FTP_CLIENT_MSG_SET_BINARY_APP:
    case FTP_CLIENT_MSG_SET_BINARY_PUT:
    case FTP_CLIENT_MSG_SET_BINARY_GET:
        usSize = (sizeof(cFTP_TYPE_I) - 1);
        ptrMsg = cFTP_TYPE_I;
        ucFTP_client_state = (FTP_CLIENT_STATE_APP_AFTER_BINARY + (ucMsg - FTP_CLIENT_MSG_SET_BINARY_APP));
        break;

    case FTP_CLIENT_MSG_CLOSE_SESSION:
        usSize = (sizeof(cFTP_QUIT) - 1);
        ptrMsg = cFTP_QUIT;
        ucFTP_client_state = FTP_CLIENT_STATE_TERMINATING; 
        break;

    default:                                                             // unexpected call value - ignore
        return 0;
    }

    if (ptrMsg != 0) {
        uMemcpy(FTP_Data_Tx.ucTCP_Message, ptrMsg, usSize);
    }
    return (fnSendTCP(FTP_Client_TCP_socket, (unsigned char *)&FTP_Data_Tx.tTCP_Header, usSize, TCP_FLAG_PUSH) > 0);
}
#endif




