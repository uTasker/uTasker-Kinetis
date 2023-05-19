/***********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher

    ---------------------------------------------------------------------
    File:      FTP.c
    Project:   Single Chip Embedded Internet
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2016
    *********************************************************************
    08.05.2007 Change a routine array initialisation to avoid library memcpy {1}
    22.05.2007 Add sub-file write support for devices with large FLASH segments {2}
    01.06.2007 Improved FTP login when not anonymous                     {3}
    03.06.2007 Add optional checking of data port in active mode         {4}
    03.06.2007 Activate passive mode (see define FTP_PASV_SUPPORT)
    03.06.2007 Pass FTP timeout when starting FTP server plus operating mode {5}
    03.06.2007 Added clean QUIT handling                                 {6}
    06.06.2007 Changed empty directory display file to include -r at beginning - without this FireFTP can not work correctly (it doesn't display anthing though) {7}
    03.09.2007 Add optional retrigger of control socket idle timer on data port activity (DATA_PORT_TRIGGERS_CONTROL) {8}
    17.11.2007 Correct dependency FTP_SUPPORTS_DELETE rather than FTP_SUPPORTS_NAME_DISPLAY {9}
    17.11.2007 Add define FILE_NAMES_PER_FTP_FRAME to correct operation without file names and adapt for compatibility {10}
    23.04.2008 Add optional DEL *.* support                              {11}
    23.02.2009 Add fast download support with FTP_DATA_WINDOWS           {12}
    18.05.2009 Add user files to DIR and return deny when delete attempted {13}
    01.07.2009 Adapt for compatibility with STRING_OPTIMISATION (fnDebugDec() replaced by fnBufferDec()) {14}
    14.10.2009 Improve simulation support for embedded user files in file system {15}
    14.10.2009 Close connection on password failure                      {16}
    04.12.2009 Add optional utFAT support                                {17}
    09.12.2009 Close the data connection immediately if no directory content is returned {18}
    31.01.2010 FTP SD card file open for rename uses attribute UTFAT_OPEN_FOR_RENAME rather than UTFAT_OPEN_FOR_WRITE {19}
    10.05.2010 Add optional MANAGED_FILES control of deletes             {20}
    27.08.2011 Add EXTENDED_UFILESYSTEM support                          {21}
    07.09.2011 Correction for EXTENDED_UFILESYSTEM support               {22}
    22.12.2011 Correct display listing over multiple frames and extend to 64k files when working with extended file option {23}
    02.04.2012 Add EPRT and EPSV for IPv6 use                            {24}
    02.04.2012 Add control of utFAT access                               {25}
    03.06.2012 Add return value when sending MSG_FTP_QUITTING to avoid an unnecessary TCP ACK {26}
    03.06.2012 Cause the FTP server to always close the connection after responding to the quit command {27}
    27.11.2012 Correct MKD directory name                                {28}
    17.12.2012 Allow FTP server to work on multiple networks             {29}
    10.07.2013 Support PWD with FAT sub-directories                      {30}
    13.10.2013 Terminate file names as strings when passing to utFAT     {31}
    13.10.2013 Ensure FTP root is reset on each connection               {32}
    30.10.2013 Handle FTP data connections for not yet known purpose     {33}
    17.01.2014 Pass directory pointer to utFAT2.0 version of utOpenFile(){34}
    04.10.2014 Correct DIR entry length when EXTENDED_UFILESYSTEM is used (and generally simplify method) {35}
    07.11.2014 Optimise internal file name DIR display to allow variable length names and sizes without buffer overflow risk {36}
    21.11.2014 Introduce fnSendNextDataBlock() to centralise the handling of file data transmission and avoid problem with transfer or zero length file from utFAT {37}
    21.11.2014 Correct passive mode from utFAT which could try to handle file transfer actively {38}
    22.11.2014 Allow multiple control connections                        {39}
    23.11.2014 Correct data connection's use of the data tcp control object on first frame transmission when using passive mode {40}
    23.01.2015 Return 500 if an unknown commnd is received before or during authentication {41}

*/

// FTP server with support for only one user - we want to use it for file uploads

#include "config.h"



#if defined USE_FTP

/* =================================================================== */
/*                          local definitions                          */
/* =================================================================== */


#if defined STRING_OPTIMISATION                                          // {14}
    #define _fnBufferDec(x, y, z) fnBufferDec(x, y, z)                   // new decimal string routine
#else
    #define _fnBufferDec(x, y, z) fnDebugDec(x, y, z)                    // original decimal string routine
#endif
#if !defined FTP_DISPLAY_USER_FILES
    #undef INTERNAL_USER_FILES
#endif
#if !defined FTP_CONNECTIONS
    #define FTP_CONNECTIONS 1
#endif

#define OWN_TASK                    TASK_FTP

#define FTP_STATE_CLOSED            0                                    // FTP command states
#define FTP_STATE_CONNECTED         1
#define FTP_STATE_LOGIN             2
#define FTP_STATE_ACTIVE_USER       3
#define FTP_STATE_PREPARE_CLOSE     4

#define MSG_DO_NOTHING              0                                    // Message states
#define MSG_REPEAT_DATA             1
#define MSG_REPEAT_CONTROL          2

#define MSG_SERVER_READY            10                                   // Command messages
#define MSG_DIR_OK                  11
#define MSG_ENTER_PASS              12
#define MSG_LOG_SUCCESS             13
#define MSG_LOG_FAILED              14
#define MSG_FTP_DATA                15
#define MSG_FTP_TYPE                16
#define MSG_FTP_DENIED              17
#define MSG_FTP_DIR                 18
#define MSG_FTP_OK                  19
#define MSG_DIR_CHANGED             20
#define MSG_FTP_UNKNOWN             21
#define MSG_DEL_OK                  22
#define MSG_FILE_LENGTH             23
#define MSG_PASV_OK                 24
#define MSG_BAD_DATA_CONNECTION     25
#define MSG_NOT_SUPPORTED           26
#define MSG_FTP_QUITTING            27
#define MSG_FTP_READY_FOR_RENAME    28
#define MSG_FTP_RENAME_SUCCESSFUL   29
#define MSG_EPSV_OK                 30

#define MSG_DIR                     40                                   // Data messages
#define MSG_UPLOAD                  41

#define FIRST_DATA_MESSAGE          MSG_DIR


#define FTP_MODE_ASCII  0
#define FTP_MODE_BINARY 1

#define DATA_INACTIVE   0
#define DO_LIST         1
#define DO_SAVE         2
#define DOING_SAVE      3
#define SAVE_COMPLETE   4
#define WAIT_COMPLETE   5
#define DO_UPLOAD       6
#define DOING_UPLOAD    7

#define MODE_ACTIVE_FTP   0x00
#define MODE_PASV         0x01
#define DATA_CONNECTED    0x02
#define DATA_UNKNOWN      0x04                                           // {33}

#if defined SUB_FILE_SIZE
    #define SUBFILE_WRITE  , ptrFtp->ucSubFileInProgress
    #define SUB_FILE_ON    ,SUB_FILE_TYPE
#else
    #define SUBFILE_WRITE
    #define SUB_FILE_ON
#endif

#if !defined UT_FTP_PATH_LENGTH                                          // {30}
    #define UT_FTP_PATH_LENGTH    0                                      // if the user has not specified a path buffer length the PWD command will always return "/" - therefore this string length should be allocated to allow the deepest directory to be displayed
#endif


#if defined FTP_SUPPORTS_NAME_DISPLAY
    static const CHAR cFileType[]    = {' ', '1', ' ', '5', '0', '2', ' ', '5', '0', '2', ' '};
    static const CHAR cFileDate[]    = {' ', 'A', 'p', 'r', ' ', '5', ' ', '2', '0', '1', '5', ' '}; // {13} fixed date time stamp
    static const CHAR cFileRights[]  = {'-', 'r', 'w', 'x', 'r', 'w', 'x', 'r', 'w', 'x'};
    #define LENGTH_OF_FILE_TYPE       sizeof(cFileType)
    #define LENGTH_OF_FILE_LENGTH     8                                  // {36} 99 Meg is max possible file size (uFileSystem or internal file)
    #define LENGTH_OF_FILE_DATE       sizeof(cFileDate)
    #define LENGTH_OF_FILE_NAME       7                                  // {13} reduced by 1
    #if defined EXTENDED_UFILESYSTEM                                     // {22}
        #define LENGTH_OF_FILE_INFO   (EXTENDED_UFILESYSTEM + sizeof(cFileRights) + LENGTH_OF_FILE_TYPE + LENGTH_OF_FILE_LENGTH + LENGTH_OF_FILE_DATE + LENGTH_OF_FILE_NAME)
    #else
        #define LENGTH_OF_FILE_INFO   (sizeof(cFileRights) + LENGTH_OF_FILE_TYPE + LENGTH_OF_FILE_LENGTH + LENGTH_OF_FILE_DATE + LENGTH_OF_FILE_NAME) // standard file length assuming maximum file length
    #endif
    #if !defined FTP_DATA_BUFFER_LENGTH                                  // use buffer the same size as the maximum TCP content if no other length is specified
        #define FTP_DATA_BUFFER_LENGTH    TCP_DEF_MTU                    // {36} 
    #endif
#else
    #define FTP_DATA_BUFFER_LENGTH    100
    #define FILE_NAMES_PER_FTP_FRAME  FTP_DATA_BUFFER_LENGTH             // {10}
#endif
#if defined USE_IPV6
    #define IP_ADDRESS_LENGTH         IPV6_LENGTH
#else
    #define IP_ADDRESS_LENGTH         IPV4_LENGTH
    #define _ulIPv6_flags             0
#endif

/* =================================================================== */
/*                      local structure definitions                    */
/* =================================================================== */


typedef struct stTCP_FTP_MESSAGE                                         // definition of a data frame structure
{
    TCP_HEADER     tTCP_Header;                                          // reserve header space
    unsigned char  ucTCP_Message[FTP_DATA_BUFFER_LENGTH];                // space for content
} TCP_FTP_MESSAGE;


typedef struct stFTP_INSTANCE                                            // definition of a data frame structure
{
    USOCKET              FTP_TCP_socket;                                 // control connection socket - this must be first in the struct
    USOCKET              FTP_TCP_Data_socket;                            // data connection socket
    unsigned long        FileOffset;
#if defined USE_IPV6                                                     // {24}
    unsigned long        ulIPv6_flags;                                   // flags used onyl by IPv6
#endif
    unsigned char        ipFTP_data[IP_ADDRESS_LENGTH];                  // data port IP address
#if defined MANAGED_FILES && defined MANAGED_FTP_DELETES
    int                  iDeleteFileHandle;
#endif
    MEMORY_RANGE_POINTER ptrFile;
#if defined FTP_SUPPORTS_DOWNLOAD
    unsigned short       usLastSentSize;
#endif
    unsigned short       usLastFrameFileCount;
    unsigned short       usFilesSent;
    unsigned short       usFTP_DataPort;                                 // data port used
    unsigned char        ucLastData;
#if defined FTP_USER_LOGIN
    unsigned char        ucPasswordAccess;                               // {3}
#endif
#if defined FTP_PASV_SUPPORT
    unsigned char        ucMode;
#endif
#if defined SUPPORT_MIME_IDENTIFIER
    unsigned char        ucMimeType;
#endif
    unsigned char        ucFTP_state;
    unsigned char        ucFTP_action;
    unsigned char        ucLastControl;
    unsigned char        ucControlQueue;
#if defined SUB_FILE_SIZE
    unsigned char        ucSubFileInProgress;
#endif
} FTP_INSTANCE;



/* =================================================================== */
/*                 local function prototype declarations               */
/* =================================================================== */

static int  fnFTPListener(USOCKET Socket, unsigned char ucEvent, unsigned char *ucIp_Data, unsigned short usPortLen);
static int  fnFTP_Data_Listener(USOCKET Socket, unsigned char ucEvent, unsigned char *ucIp_Data, unsigned short usPortLen);
static int  fnGetDataPort(CHAR *ucIP_Port, int iIPv6, FTP_INSTANCE *ptrFtp);
static signed short fnSendFTP(unsigned char ucMsg, FTP_INSTANCE *ptrFtp);


/* =================================================================== */
/*                             constants                               */
/* =================================================================== */


static const CHAR   cFTPServerReady[]     = FTP_WELCOME_RESPONSE;
static const CHAR   cFTPBadPass[]         = "530 Log BAD.\r\n";
static const CHAR   cFTPEnterPass[]       = "331 Enter pass.\r\n";
static const CHAR   cFTPloginSuccessful[] = "230 Log OK.\r\n";
static const CHAR   cFTPUnknownCommand[]  = "500 What?.\r\n";
static const CHAR   cFTPType[]            = "215 UNIX Type: L8\r\n";     // pretend to be UNIX...
static const CHAR   cFTPDenied[]          = "550 Denied.\r\n";
static const CHAR   cFTPDir[]             = "257 \x22/\x22\r\n";         // path name
static const CHAR   cFTPOK[]              = "200 OK.\r\n";
static const CHAR   cFTPDirChanged[]      = "250 Dir changed.\r\n";
static const CHAR   cFTP_Data[]           = "150 Data.\r\n";
static const CHAR   cFTP_DIR_OK[]         = "226 OK.\r\n";
static const CHAR   cFTPTerminating[]     = "221 Bye\r\n";

#if defined FTP_SUPPORTS_DOWNLOAD
    static const unsigned char ucSizeResponse[] = {'2', '1', '3', ' '};
#endif
#if defined FTP_PASV_SUPPORT
    static const unsigned char ucPasvResponse[] = {'2', '2', '7', ' ', 'P', 'A', 'S', 'V', ' ', 'M', 'O', 'D', 'E', ' ', '('};
#else
    static const CHAR cFTP_Not_Supported[]= "502 No do.\r\n";
#endif
#if defined FTP_VERIFY_DATA_PORT
    static const CHAR cFTP_Bad_Port[]     = "425 Bad data port\r\n";     // {4}
#endif
#if defined FTP_SUPPORTS_DELETE                                          // {9}
    static const CHAR cFTPDelOK[]         = "250 Del OK.\r\n";
#endif
#if defined FTP_UTFAT                                                    // {17}
    static const CHAR cFTP_Rename_OK[]    = "350 Rdy\r\n";
    static const CHAR cFTP_Rename_Success[] = "250 OK\r\n";
#endif
static const CHAR cFTP_DIR[]              = "-r Empty\r\n";              // we display this when a directory is empty {7}

/* =================================================================== */
/*                     global variable definitions                     */
/* =================================================================== */

#if defined FTP_DATA_WINDOWS                                             // {12}
    extern TCP_CONTROL *present_tcp;                                     // global pointer to present tcp socket's control structure
#endif
#if defined _WINDOWS
    extern int user_files_not_in_code;                                   // {15}
#endif
#if defined INTERNAL_USER_FILES                                          // {13}
    extern USER_FILE *ptrUserFiles;                                      // global pointer to user file set
#endif

/* =================================================================== */
/*                      local variable definitions                     */
/* =================================================================== */

#if defined EXTENDED_UFILESYSTEM                                         // {23}
    typedef unsigned short  MAX_FILES;                                   // up to 64k files possible
#else
    typedef unsigned char   MAX_FILES;                                   // up to 256 files possible
#endif

static FTP_INSTANCE         ftp_instance[FTP_CONNECTIONS] = {{-1}};

#if defined FTP_USER_LOGIN
    static unsigned char    ucFTP_mode;                                  // mode vallid for all sessions
#endif

#if defined FTP_UTFAT                                                    // {17}
    static UTDIRECTORY *ptr_utDirectory = 0;                             // directory object for a single user (shared by multiple connection)
    static UTLISTDIRECTORY utListDirectory;                              // list directory object for a single user
    static UTLISTDIRECTORY utListLastDirectory;                          // backup in case of need to repeat
    static FILE_LISTING fileList = {0};
    static UTFILE utFile = {0};
#endif




// Initialise and start the FTP server with the defined timeout value and in the defined operating mode
//
extern void fnStartFtp(unsigned short usFTPTimeout, unsigned char ucFTP_operating_mode) // {5}
{
    FTP_INSTANCE *ptrFtp = ftp_instance;
    if (ptrFtp->FTP_TCP_socket != -1) {                                  //if the socket has already been configured
        return;
    }
#if defined FTP_UTFAT                                                    // {17}
    if (ptr_utDirectory == 0) {                                          // if the utFAT directory object has not yet been defined
        ptr_utDirectory = utAllocateDirectory(DISK_D, UT_FTP_PATH_LENGTH); // {30} allocate a directory for use by this module associated with D: - no path string
        if ((ucFTP_operating_mode & FTP_UTFAT_OFF) == 0) {               // {25} as long as the utFAT mode hasn't been disabled
            utServer(0, UTFAT_FTP_SERVER_ON);                            // enable ftp server operation with utFAT
        }
    }
#endif
#if defined FTP_USER_LOGIN
    ucFTP_mode = ucFTP_operating_mode;                                   // save operating mode
#endif
    do {
        ptrFtp->FTP_TCP_socket = fnGetTCP_Socket(TOS_MINIMISE_DELAY, usFTPTimeout, fnFTPListener); // get TCP Socket and listen on FTP control port
        fnTCP_Listen(ptrFtp->FTP_TCP_socket, FTP_CONTROL_PORT, 0);
        ptrFtp->ucFTP_state = FTP_STATE_CLOSED;                          // ensure we start in closed state
        ptrFtp->FTP_TCP_Data_socket = fnGetTCP_Socket(TOS_MAXIMISE_THROUGHPUT, TCP_DEFAULT_TIMEOUT, fnFTP_Data_Listener); // reserve FTP data socket
    } while (++ptrFtp < &ftp_instance[FTP_CONNECTIONS]);
}

// Stop and de-initisliase the FTP server
//
extern void fnStopFtp(void)
{
    FTP_INSTANCE *ptrFtp = ftp_instance;
    do {
        fnReleaseTCP_Socket(ptrFtp->FTP_TCP_socket);                     // we close the sockets so that FTP is effectively dead.
        fnReleaseTCP_Socket(ptrFtp->FTP_TCP_Data_socket);
        ptrFtp->FTP_TCP_socket = -1;                                     // invalidate the socket
    } while (++ptrFtp < &ftp_instance[FTP_CONNECTIONS]);
}



// The data socket generates some messages for the control socket. Since the data socket has no information
// about the state of the control socket it uses a small queue to send messages (only one place in queue)
//
static void fnQueueSendFTP(unsigned char ucControlMsg, FTP_INSTANCE *ptrFtp)
{
    if (ptrFtp->ucLastControl == MSG_DO_NOTHING) {                       // if the control socket is not waiting for an ack it can send the message immediately
        fnSendFTP(ucControlMsg, ptrFtp);                                 // immediately send control message
        ptrFtp->ucControlQueue = MSG_DO_NOTHING;                         // nothing else in the queue
    }
    else {
        ptrFtp->ucControlQueue = ucControlMsg;                           // we queue the message to be sent once the control socket can
    }
}

static unsigned char fnGetQueue(FTP_INSTANCE *ptrFtp)
{
    unsigned char ucMsg;
    if (ptrFtp->ucLastControl != MSG_DO_NOTHING) {                       // if FTP is still waiting for an ACK don't release anything from queue
        return MSG_DO_NOTHING;
    }
    ucMsg = ptrFtp->ucControlQueue;
    ptrFtp->ucControlQueue = MSG_DO_NOTHING;
    return ucMsg;
}

#if defined MANAGED_FILES && defined MANAGED_FTP_DELETES                 // {20}
static void fnDelete_terminated(int iHandle, int iResult)
{
    FTP_INSTANCE *ptrFtp = ftp_instance;
    while (ptrFtp < &ftp_instance[FTP_CONNECTIONS]) {
        if (ptrFtp->iDeleteFileHandle == iHandle) {                      // match the terminated delete handle to the connection
            fnSendFTP(MSG_DEL_OK, ptrFtp);                               // delete terminated
            return;
        }
        ptrFtp++;
    }
}
#endif

// Set passive mode if this is supported
//
static int fnPreparePassive(int iIPv6, FTP_INSTANCE *ptrFtp)
{
#if defined FTP_PASV_SUPPORT
    if (MODE_PASV & ptrFtp->ucMode) {
        fnTCP_close(ptrFtp->FTP_TCP_socket);
        if (DATA_CONNECTED & ptrFtp->ucMode) {
            fnTCP_close(ptrFtp->FTP_TCP_Data_socket);
        }
        ptrFtp->ucMode = 0;
        return APP_REJECT_DATA;                                          // we don't allow multiple data ports
    }
    ptrFtp->usFTP_DataPort = fnGetFreeTCP_Port();                        // get a port to listen on
    ptrFtp->ucMode = MODE_PASV;                                          // move to passive mode
    fnTCP_Listen(ptrFtp->FTP_TCP_Data_socket, ptrFtp->usFTP_DataPort, 0);// start listening
    #if defined USE_IPV6
    if (iIPv6 != 0) {
        return (fnSendFTP(MSG_EPSV_OK, ptrFtp));                         // IPv6 response
    }
    else {
        return (fnSendFTP(MSG_PASV_OK, ptrFtp));                         // IPv4 response
    }
    #else
    return (fnSendFTP(MSG_PASV_OK, ptrFtp));
    #endif
#else
     return (fnSendFTP(MSG_NOT_SUPPORTED, ptrFtp));
#endif
}

#if defined FTP_UTFAT                                                    // {31}
static const CHAR *fnStringTerminate(unsigned char *ptrInput)
{
    CHAR *ptrInputString = (CHAR *)ptrInput;

    while (*ptrInputString >= ' ') {                                     // ftp inputs are terminated with 0x0d 0x0a so we search for this
        ptrInputString++;
    }
    *ptrInputString = 0;                                                 // the input is terminated so that it can be handled as a string by utFAT
    return (const CHAR *)ptrInput;
}
#endif

#if defined FTP_SUPPORTS_DOWNLOAD
static int fnSendNextDataBlock(FTP_INSTANCE *ptrFtp)                     // {37}
{
    if (fnSendFTP(MSG_UPLOAD, ptrFtp) != 0) {                            // now return the data [FTP-Data]
        ptrFtp->ucFTP_action = DOING_UPLOAD;
        return APP_ACCEPT;
    }
    else {
        fnQueueSendFTP(MSG_DIR_OK, ptrFtp);                              // completely transmitted
        ptrFtp->ucFTP_action = DATA_INACTIVE;
        fnTCP_close(ptrFtp->FTP_TCP_Data_socket);
        return APP_REQUEST_CLOSE;
    }
}
#endif

// Local listener to TCP FTP command port
//
static int fnFTPListener(USOCKET Socket, unsigned char ucEvent, unsigned char *ucIp_Data, unsigned short usPortLen)
{
    FTP_INSTANCE *ptrFtp = ftp_instance;
    unsigned short usNextData;
    while (1) {
        if (_TCP_SOCKET_MASK(Socket) == _TCP_SOCKET_MASK(ptrFtp->FTP_TCP_socket)) { // {29}
            break;                                                       // handling connection matched
        }
        if (++ptrFtp >= &ftp_instance[FTP_CONNECTIONS]) {
            return APP_REJECT;                                           // ignore if socket handle not found
        }
    }

    switch (ucEvent) {
    case TCP_EVENT_CONNECTED:
#if defined FTP_UTFAT                                                    // {17}
      //ptr_utDirectory->usDirectoryFlags &= ~UTDIR_VALID;               // ensure reset on each new connection
      //utOpenDirectory(FTP_ROOT, ptr_utDirectory);                      // open the FTP root directory of disk D on connection (if no disk or no directory this fails and the FTP server falls back to other file systems)
        utServer(0, (UTFAT_FTP_SERVER_ON | UTFAT_FTP_SERVER_ROOT_RESET));// {32} prepare reset on next open
        utServer(ptr_utDirectory, (UTFAT_FTP_SERVER_ON));                // {25} if the ftp root exists and ftp server access is allowed
#endif
        ptrFtp->ucFTP_state = FTP_STATE_CONNECTED;                       // we send standard response 220 followed by a message
        ptrFtp->ucLastControl = MSG_DO_NOTHING;
        return (fnSendFTP(MSG_SERVER_READY, ptrFtp));

    case TCP_EVENT_ACK:
        ptrFtp->ucLastControl = MSG_DO_NOTHING;
        if (ptrFtp->ucFTP_action != DO_LIST) {
//#if defined _CLOSE_ERROR && defined FTP_USER_LOGIN                     // {27}
            if (FTP_STATE_PREPARE_CLOSE == ptrFtp->ucFTP_state) {        // {16}
                fnTCP_close(Socket);
                ptrFtp->ucFTP_state = FTP_STATE_CONNECTED;
                return APP_REQUEST_CLOSE;
            }
//#endif
            fnSendFTP(fnGetQueue(ptrFtp), ptrFtp);                       // if there are queued command, send next one
        }
        break;

    case TCP_EVENT_ABORT:
        // Fall through intentional
        //
    case TCP_EVENT_CLOSED:
        ptrFtp->ucFTP_state = FTP_STATE_CLOSED;
        ptrFtp->ucFTP_action = DATA_INACTIVE;
        fnTCP_Listen(ptrFtp->FTP_TCP_socket, FTP_CONTROL_PORT, 0);       // go back to listening state
        break;

    case TCP_EVENT_DATA:                                                 // we have new receive data
        if ((FTP_STATE_CONNECTED == ptrFtp->ucFTP_state) && ((usNextData = uStrEquiv((const CHAR *)ucIp_Data, "USER ")) != 0)) {
            usPortLen -= usNextData;
            ucIp_Data += usNextData;
#if defined FTP_USER_LOGIN
            if (ucFTP_mode & FTP_AUTHENTICATE) {                         // if authentication is required
                ptrFtp->ucPasswordAccess = (unsigned char)fnVerifyUser((CHAR *)ucIp_Data, (DO_CHECK_USER_NAME | FTP_PASS_CHECK)); // {3}
            }
#endif
            ptrFtp->ucFTP_state = FTP_STATE_LOGIN;                       // we always accept the user name but may reject after password check
            return (fnSendFTP(MSG_ENTER_PASS, ptrFtp));                  // request password
        }
        else if ((FTP_STATE_LOGIN == ptrFtp->ucFTP_state) && ((usNextData = uStrEquiv((const CHAR *)ucIp_Data, "PASS ")) != 0)) {
#if defined FTP_USER_LOGIN                                               // {3}
            if (ucFTP_mode & FTP_AUTHENTICATE) {                         // if authentication is required
                usPortLen -= usNextData;                                 // we verify that the password matches that what we are looking for
                ucIp_Data += usNextData;
                ptrFtp->ucPasswordAccess |= (unsigned char)fnVerifyUser((CHAR *)ucIp_Data, (DO_CHECK_PASSWORD | FTP_PASS_CHECK)); // {3}
                if (ptrFtp->ucPasswordAccess != 0) {                     // if there was a user name or password error
                    ptrFtp->ucFTP_state = FTP_STATE_PREPARE_CLOSE;       // {16}
                    return (fnSendFTP(MSG_LOG_FAILED, ptrFtp));          // send bad password which causes the client to close
                }
            }
#endif
            ptrFtp->ucFTP_state = FTP_STATE_ACTIVE_USER;                 // we accept any old password...
            return (fnSendFTP(MSG_LOG_SUCCESS, ptrFtp));
        }
        else if (FTP_STATE_ACTIVE_USER == ptrFtp->ucFTP_state) {
#if defined FTP_UTFAT
            int iCmpLen;                                                 // {28}
#endif
            if (uStrEquiv((const CHAR *)ucIp_Data, "syst")) {
                return (fnSendFTP(MSG_FTP_TYPE, ptrFtp));
            }
            else if (uStrEquiv((const CHAR *)ucIp_Data, "site")) {
                return (fnSendFTP(MSG_FTP_DENIED, ptrFtp));
            }
            else if (uStrEquiv((const CHAR *)ucIp_Data, "PWD")) {
                return (fnSendFTP(MSG_FTP_DIR, ptrFtp));
            }
            else if (uStrEquiv((const CHAR *)ucIp_Data, "PASV")) {       // client requests passive mode of operation
                return (fnPreparePassive(0, ptrFtp));                    // prepare a listening port for the data connection
            }
#if defined USE_IPV6                                                     // {24}
            else if (uStrEquiv((const CHAR *)ucIp_Data, "EPSV")) {       // client requests passive IPv6 operation
                return (fnPreparePassive(1, ptrFtp));
            }
#endif
            else if (uStrEquiv((const CHAR *)ucIp_Data, "noop")) {
                return (fnSendFTP(MSG_FTP_OK, ptrFtp));
            }
            else if (uStrEquiv((const CHAR *)ucIp_Data, "CWD")) {
#if defined FTP_UTFAT                                                    // {17}
                if (ptr_utDirectory->usDirectoryFlags & UTDIR_VALID) {
                    utListDirectory.ptr_utDirObject = ptr_utDirectory;   // the list directory is always referenced to the main directory object
                    if (utChangeDirectory(fnStringTerminate(ucIp_Data + 4), ptr_utDirectory) != UTFAT_SUCCESS) { // {31} change the directory location
                        return (fnSendFTP(MSG_FTP_DENIED, ptrFtp));      // invalid path
                    }
                }
#endif
                return (fnSendFTP(MSG_DIR_CHANGED, ptrFtp));
            }
            else if ((usNextData = uStrEquiv((const CHAR *)ucIp_Data, "TYPE ")) != 0) { // type command can be ASCII (A) or Image (I)
                return (fnSendFTP(MSG_FTP_OK, ptrFtp));
            }
            else if ((usNextData = uStrEquiv((const CHAR *)ucIp_Data, "PORT ")) != 0) { // client defines port to be used for active data transfer
                return (fnGetDataPort((CHAR *)(ucIp_Data + usNextData), 0, ptrFtp));
            }
#if defined USE_IPV6                                                     // {24}
            else if ((usNextData = uStrEquiv((const CHAR *)ucIp_Data, "EPRT ")) != 0) { // client defines port to be used for active data transfer
                return (fnGetDataPort((CHAR *)(ucIp_Data + usNextData), 1, ptrFtp));
            }
#endif
            else if (uStrEquiv((const CHAR *)ucIp_Data, "LIST")) {
#if defined USE_IPV6
                unsigned long _ulIPv6_flags = ptrFtp->ulIPv6_flags;
#endif
#if defined FTP_UTFAT                                                    // {17}
                if (ptr_utDirectory->usDirectoryFlags & UTDIR_VALID) {
                    utListDirectory.ptr_utDirObject = ptr_utDirectory;   // the list directory is always referenced to the main directory object
                    if (utLocateDirectory(fnStringTerminate(ucIp_Data + 5), &utListDirectory) != UTFAT_SUCCESS) { // {31} set to present directory
                        return (fnSendFTP(MSG_FTP_DENIED, ptrFtp));      // invalid path
                    }
                    fileList.usMaxItems = 0xffff;                        // the maximum number of entries will be limited by the TCP buffer space
                }
#endif
#if defined FTP_PASV_SUPPORT
                if (MODE_PASV & ptrFtp->ucMode) {
                    int iRtn = (fnSendFTP(MSG_FTP_DATA, ptrFtp) > 0);
                    ptrFtp->usLastFrameFileCount = 0;                    // {23}
                    ptrFtp->usFilesSent = 0;
                    if (DATA_CONNECTED & ptrFtp->ucMode) {
                        fnSendFTP(MSG_DIR, ptrFtp);                      // return the data response [FTP-Data] 
                    }
                    ptrFtp->ucFTP_action = DO_LIST;
                    return iRtn;
                }
#endif
                _TCP_SOCKET_MASK_ASSIGN(ptrFtp->FTP_TCP_Data_socket);    // {29}
                ptrFtp->FTP_TCP_Data_socket |= (ptrFtp->FTP_TCP_socket & ~(SOCKET_NUMBER_MASK)); // allow the data socket to inherit the network, interface and VLAN properties if the command socket
                if (fnTCP_Connect(ptrFtp->FTP_TCP_Data_socket, ptrFtp->ipFTP_data, ptrFtp->usFTP_DataPort, FTP_DATA_PORT, _ulIPv6_flags) >= 0) {
                   ptrFtp->ucFTP_action = DO_LIST;
                }
                else {
                    return (fnSendFTP(MSG_FTP_DENIED, ptrFtp));          // presently not possible to open data connection...
                }
                                                                         // we don't send an ACK here but wait for the data link establishment to do it
            }
            else if (uStrEquiv((const CHAR *)ucIp_Data, "STOR ")) {
#if defined USE_IPV6
                unsigned long _ulIPv6_flags = ptrFtp->ulIPv6_flags;
#endif
                _TCP_SOCKET_MASK_ASSIGN(ptrFtp->FTP_TCP_Data_socket);    // {29}
                ptrFtp->FTP_TCP_Data_socket |= (ptrFtp->FTP_TCP_socket & ~(SOCKET_NUMBER_MASK)); // allow the data socket to inherit the network, interface and VLAN properties if the command socket
#if defined FTP_PASV_SUPPORT
                if ((MODE_PASV & ptrFtp->ucMode) || (fnTCP_Connect(ptrFtp->FTP_TCP_Data_socket, ptrFtp->ipFTP_data, ptrFtp->usFTP_DataPort, FTP_DATA_PORT, _ulIPv6_flags) >= 0))
#else
                if (fnTCP_Connect(ptrFtp->FTP_TCP_Data_socket, ptrFtp->ipFTP_data, ptrFtp->usFTP_DataPort, FTP_DATA_PORT, _ulIPv6_flags) >= 0) 
#endif
                {
#if defined FTP_UTFAT                                                    // {17}
                    if (ptr_utDirectory->usDirectoryFlags & UTDIR_VALID) {
                      //utFile.ptr_utDirObject = ptr_utDirectory;
                        if (utOpenFile(fnStringTerminate(ucIp_Data + 5), &utFile, ptr_utDirectory, (UTFAT_OPEN_FOR_WRITE | UTFAT_CREATE | UTFAT_TRUNCATE)) != UTFAT_PATH_IS_FILE) { // {31}{34} open a file referenced to the directory object
                            return (fnSendFTP(MSG_FTP_DENIED, ptrFtp));  // file can not be created, overwritten
                        }
                    }
                    else {
#endif
#if defined ACTIVE_FILE_SYSTEM
                        ptrFtp->ptrFile = uOpenFile((CHAR *)(ucIp_Data + 5)); // get file pointer (to new file or file to overwrite)
    #if defined SUPPORT_MIME_IDENTIFIER
                        ptrFtp->ucMimeType = fnGetMimeType((CHAR *)(ucIp_Data + 5)); // get the type of file being saved
    #endif
    #if defined SUB_FILE_SIZE
                        ptrFtp->ucSubFileInProgress = fnGetFileType((CHAR *)(ucIp_Data + 5)); // get file characteristics so that it is later handled correctly
    #endif
#endif
#if defined FTP_UTFAT                                                    // {17}
                    }
#endif
#if defined FTP_PASV_SUPPORT
                    if (MODE_PASV & ptrFtp->ucMode)  { 
                        ptrFtp->ucFTP_action = DOING_SAVE;
                        return (fnSendFTP(MSG_FTP_DATA, ptrFtp));
                    }
#endif
                    ptrFtp->ucFTP_action = DO_SAVE;                      // mark that we are doing save
                }
                else {
                    return (fnSendFTP(MSG_FTP_DENIED, ptrFtp));          // presently not possible to open data connection...
                }
                                                                         // we don't send an ACK here but instead wait for the data link establishment to do it
            }
#if defined FTP_SUPPORTS_DELETE
            else if (uStrEquiv((const CHAR *)ucIp_Data, "DELE ")) {
    #if defined FTP_UTFAT                                                // {17}
                if (ptr_utDirectory->usDirectoryFlags & UTDIR_VALID) {
                    if (utDeleteFile(fnStringTerminate(ucIp_Data + 5), ptr_utDirectory) != UTFAT_SUCCESS) { // {31}
                        return (fnSendFTP(MSG_FTP_DENIED, ptrFtp));
                    }
                    else {
                        return (fnSendFTP(MSG_DEL_OK, ptrFtp));
                    }
                }
    #endif
    #if defined FTP_WILDCARD_DEL                                         // {11}
                if (uStrEquiv((const CHAR *)(ucIp_Data + 5), "*.*")) {   // wildcard delete
        #if defined MANAGED_FILES && defined MANAGED_FTP_DELETES         // {20}
                    MANAGED_MEMORY_AREA_BLOCK memory_area;
                    memory_area.ptrStart = uFILE_SYSTEM_START;           // start address of the area to be deleted
                    memory_area.size = FILE_SYSTEM_SIZE;                 // size of area to be deleted
                    memory_area.period = 0;                              // page delete once every scheduling pass
                    memory_area.ucParameters = (AUTO_CLOSE | AUTO_DELETE); // on termination automatically close the file
                    memory_area.fileOperationCallback = fnDelete_terminated; // callback on completion
                    if ((ptrFtp->iDeleteFileHandle = uOpenManagedFile(&memory_area, OWN_TASK, (MANAGED_MEMORY_AREA | MANAGED_DELETE))) < 0) { // open the file which will be automatically deleted and closed
                        return (fnSendFTP(MSG_FTP_DENIED, ptrFtp));
                    }
                    return APP_ACCEPT;                                   // return OK after the delete has been successful
        #else
            #if defined ACTIVE_FILE_SYSTEM
                #if defined EXTENDED_UFILESYSTEM                         // {21}
                    uFileErase(uFILE_SYSTEM_START, (MAX_FILE_LENGTH)(fnGetEndOf_uFileSystem() - uFILE_SYSTEM_START));
                #else
                    uFileErase(uFILE_SYSTEM_START, (MAX_FILE_LENGTH)FILE_SYSTEM_SIZE);
                #endif
            #endif
                    ptrFtp->FileOffset = 0;
        #endif
                }
                else {
        #if defined MANAGED_FILES && defined MANAGED_FTP_DELETES         // {20}
                    MANAGED_MEMORY_AREA_BLOCK memory_area;
                    memory_area.ptrStart = uOpenFile((CHAR *)(ucIp_Data + 5)); // start address of the area to be deleted
                    memory_area.size = uGetFileLength(memory_area.ptrStart); // size of area to be deleted
                    memory_area.period = 0;                              // page delete once every scheduling pass
                    memory_area.ucParameters = (AUTO_CLOSE | AUTO_DELETE); // on termination automatically close the file
                    memory_area.fileOperationCallback = fnDelete_terminated; // callback on completion
                    if ((ptrFtp->iDeleteFileHandle = uOpenManagedFile(&memory_area, OWN_TASK, (MANAGED_MEMORY_AREA | MANAGED_DELETE))) < 0) { // open the file which will be automatically deleted and closed
                        return (fnSendFTP(MSG_FTP_DENIED, ptrFtp));
                    }
                    return APP_ACCEPT;                                   // return OK after the delete has been successful
        #else
            #if defined ACTIVE_FILE_SYSTEM
                    MEMORY_RANGE_POINTER ptrThisFile = uOpenFile((CHAR *)(ucIp_Data + 5));
                    MAX_FILE_LENGTH Length = uGetFileLength(ptrThisFile);
            #endif
            #if defined INTERNAL_USER_FILES                              // {13}
                #if !defined ACTIVE_FILE_SYSTEM
                    return (fnSendFTP(MSG_FTP_DENIED, ptrFtp));          // deny delete of internal files
                #else
                    if ((Length == 0) || (ptrThisFile < uFILE_SYSTEM_START)
                    #if defined _WINDOWS
                        || (ptrThisFile >= fnGetEndOf_uFileSystem())
                    #endif
                        ) {
                        return (fnSendFTP(MSG_FTP_DENIED, ptrFtp));      // deny delete of internal files
                    }
                #endif
            #endif
            #if defined ACTIVE_FILE_SYSTEM
                    uFileErase(ptrThisFile, (MAX_FILE_LENGTH)(Length + FILE_HEADER));
                    ptrFtp->FileOffset = (ptrThisFile - uFILE_SYSTEM_START);
            #endif
        #endif
                }
                return (fnSendFTP(MSG_DEL_OK, ptrFtp));                  // we always respond with OK
    #else
                MEMORY_RANGE_POINTER ptrThisFile = uOpenFile((CHAR *)(ucIp_Data + 5));
                MAX_FILE_LENGTH Length = uGetFileLength(ptrThisFile);
        #if defined ACTIVE_FILE_SYSTEM
                uFileErase(ptrThisFile, (MAX_FILE_LENGTH)(Length + FILE_HEADER));
        #endif
                ptrFtp->FileOffset = (ptrThisFile - uFILE_SYSTEM_START);
                return (fnSendFTP(MSG_DEL_OK, ptrFtp));                  // we always respond with OK
    #endif
            }
#endif

#if defined FTP_SUPPORTS_DOWNLOAD 
            else if (uStrEquiv((const CHAR *)ucIp_Data, "SIZE ")) {
    #if defined FTP_UTFAT                                                // {17}
                if (ptr_utDirectory->usDirectoryFlags & UTDIR_VALID) {
                    if (utOpenFile(fnStringTerminate(ucIp_Data + 5), &utFile, ptr_utDirectory, UTFAT_OPEN_FOR_READ) != UTFAT_PATH_IS_FILE) { // {31}{34} open a file referenced to the directory object
                        return (fnSendFTP(MSG_FTP_DENIED, ptrFtp));      // file not found
                    }
                    ptrFtp->FileOffset = utFile.ulFileSize;
                }
                else {
    #endif
    #if defined FTP_PASV_SUPPORT
                    if (*(ucIp_Data + 5) == '/') {                       // if no file given, return that it is not valid
                          return (fnSendFTP(MSG_FTP_DENIED, ptrFtp));
                    }
    #endif
    #if defined ACTIVE_FILE_SYSTEM
                    ptrFtp->FileOffset = (uOpenFile((CHAR *)(ucIp_Data + 5)) - uFILE_SYSTEM_START);
    #endif
    #if defined FTP_UTFAT                                                // {17}
                }
    #endif
                return (fnSendFTP(MSG_FILE_LENGTH, ptrFtp));             // return the size of the requested data
            }
            else if (uStrEquiv((const CHAR *)ucIp_Data, "RETR ")) {
    #if defined USE_IPV6
                unsigned long _ulIPv6_flags = ptrFtp->ulIPv6_flags;
    #endif
                _TCP_SOCKET_MASK_ASSIGN(ptrFtp->FTP_TCP_Data_socket);    // {29}
                ptrFtp->FTP_TCP_Data_socket |= (ptrFtp->FTP_TCP_socket & ~(SOCKET_NUMBER_MASK)); // allow the data socket to inherit the network, interface and VLAN properties if the command socket
    #if defined FTP_UTFAT                                                // {17}
                if (ptr_utDirectory->usDirectoryFlags & UTDIR_VALID) {
                  //utFile.ptr_utDirObject = ptr_utDirectory;
                    if (utOpenFile(fnStringTerminate(ucIp_Data + 5), &utFile, ptr_utDirectory, UTFAT_OPEN_FOR_READ) != UTFAT_PATH_IS_FILE) { // {31}{34} open a file referenced to the directory opbject
        #if defined FTP_PASV_SUPPORT
                        if (DATA_CONNECTED & ptrFtp->ucMode) {           // if we have a data connection waiting to transfer, close it
                            fnTCP_close(ptrFtp->FTP_TCP_Data_socket);
                        }
                        ptrFtp->ucMode = 0;
        #endif
                        return (fnSendFTP(MSG_FTP_DENIED, ptrFtp));      // file not found
                    }
                    ptrFtp->FileOffset = 0;                              // start at beginning of file
        #if defined FTP_PASV_SUPPORT                                     // {38}
                    if (MODE_PASV & ptrFtp->ucMode) {                    // if in passive mode (rather than active)
                        if (DATA_CONNECTED & ptrFtp->ucMode) {           // if the data connection already exists
                            fnSendNextDataBlock(ptrFtp);                 // {37} immediately return the data [FTP-Data] - data connection already established
                        }
                        else {
                            ptrFtp->ucFTP_action = DO_UPLOAD;            // start upload on next data connection
                        }
                        break;
                    }
        #endif
                    if (fnTCP_Connect(ptrFtp->FTP_TCP_Data_socket, ptrFtp->ipFTP_data, ptrFtp->usFTP_DataPort, FTP_DATA_PORT, _ulIPv6_flags) >= 0) { // in active mode we innitiate a data connection
                        ptrFtp->ucFTP_action = DO_UPLOAD;
                    }
                    else {
                        return (fnSendFTP(MSG_FTP_DENIED, ptrFtp));      // presently not possible to open data connection...
                    }
                    break;
                }
    #endif
    #if defined FTP_PASV_SUPPORT
                if (*(ucIp_Data + 5) == '/') {                           // if no file given, return length of first file
                    if (DATA_CONNECTED & ptrFtp->ucMode) {               // if we have a data connection waiting to transfer, close it
                        fnTCP_close(ptrFtp->FTP_TCP_Data_socket);
                    }
                    ptrFtp->ucMode = 0;
                    return (fnSendFTP(MSG_FTP_DENIED, ptrFtp));
                }
    #endif
    #if defined ACTIVE_FILE_SYSTEM
                ptrFtp->ptrFile = uOpenFile((CHAR *)(ucIp_Data + 5));    // get file pointer
                if ((uGetFileLength(ptrFtp->ptrFile) != 0) && (fnTCP_Connect(ptrFtp->FTP_TCP_Data_socket, ptrFtp->ipFTP_data, ptrFtp->usFTP_DataPort, FTP_DATA_PORT, _ulIPv6_flags) >= 0)) {
                    ptrFtp->ucFTP_action = DO_UPLOAD;
                    ptrFtp->FileOffset = 0;                              // start at beginning of file
                }
                else {
    #endif
    #if defined FTP_PASV_SUPPORT
                    if (MODE_PASV & ptrFtp->ucMode) { 
                        ptrFtp->FileOffset = 0;                          // start at beginning of file
                        fnSendFTP(MSG_UPLOAD, ptrFtp);                   // immediately return the data [FTP-Data] - data connection alread established
                        ptrFtp->ucFTP_action = DOING_UPLOAD;
                        break;
                    }
    #endif
                   return (fnSendFTP(MSG_FTP_DENIED, ptrFtp));           // either no file or presently not possible to open data connection...
    #if defined ACTIVE_FILE_SYSTEM
                }
    #endif
            }
#endif
#if defined FTP_UTFAT                                                    // {17}
            else if ((ptr_utDirectory->usDirectoryFlags & UTDIR_VALID) && (uStrEquiv((const CHAR *)ucIp_Data, "RNFR "))) {
              //utFile.ptr_utDirObject = ptr_utDirectory;
                if (utOpenFile(fnStringTerminate(ucIp_Data + 5), &utFile, ptr_utDirectory, UTFAT_OPEN_FOR_RENAME) < UTFAT_SUCCESS) { // {19}{31}{34}
                   return (fnSendFTP(MSG_FTP_DENIED, ptrFtp));
                }
                else {
                   return (fnSendFTP(MSG_FTP_READY_FOR_RENAME, ptrFtp));
                }
            }
            else if ((ptr_utDirectory->usDirectoryFlags & UTDIR_VALID) && (uStrEquiv((const CHAR *)ucIp_Data, "RNTO "))) {
                if (utRenameFile(fnStringTerminate(ucIp_Data + 5), &utFile) != UTFAT_SUCCESS) { // {31}
                   return (fnSendFTP(MSG_FTP_DENIED, ptrFtp));
                }
                else {
                   return (fnSendFTP(MSG_FTP_RENAME_SUCCESSFUL, ptrFtp));
                }
            }
            else if ((ptr_utDirectory->usDirectoryFlags & UTDIR_VALID) && (((iCmpLen = uStrEquiv((const CHAR *)ucIp_Data, "XMKD ")) != 0) || ((iCmpLen = uStrEquiv((const CHAR *)ucIp_Data, "MKD ")) != 0))) { // {28}
                if (utMakeDirectory(fnStringTerminate(ucIp_Data + iCmpLen), ptr_utDirectory) == UTFAT_SUCCESS) { // {28}{31}
                    return (fnSendFTP(MSG_FTP_DIR, ptrFtp));
                }
                return (fnSendFTP(MSG_FTP_DENIED, ptrFtp));
            }
#endif
            else if (uStrEquiv((const CHAR *)ucIp_Data, "QUIT")) {
                return (fnSendFTP(MSG_FTP_QUITTING, ptrFtp));            // signal that we are terminating {26} add return
            }
            else {
                return (fnSendFTP(MSG_FTP_UNKNOWN, ptrFtp));             // we don't recognise this command so just say we don't know it
            }
        }
        else {
            return (fnSendFTP(MSG_FTP_UNKNOWN, ptrFtp));                 // {41} we don't recognise this command so just say we don't know it
        }
        break;

    case TCP_EVENT_REGENERATE:                                           // we must repeat the last control buffer we sent
        return (fnSendFTP(MSG_REPEAT_CONTROL, ptrFtp));

    case TCP_EVENT_CONREQ:
        if (ptrFtp->ucFTP_state != FTP_STATE_CLOSED) {
             return APP_REJECT;
        }
        ptrFtp->FTP_TCP_socket = Socket;                                 // {29}
        break;

    case TCP_EVENT_CLOSE:
        break;

    default:
        break;                                                           // {29} return -1;
    }
    return APP_ACCEPT;
}


// Local listener to TCP FTP data port
//
static int fnFTP_Data_Listener(USOCKET Socket, unsigned char ucEvent, unsigned char *ucIp_Data, unsigned short usPortLen)
{
    FTP_INSTANCE *ptrFtp = ftp_instance;
    while (1) {
        if (_TCP_SOCKET_MASK(Socket) == _TCP_SOCKET_MASK(ptrFtp->FTP_TCP_Data_socket)) {
            break;                                                       // handling connection matched
        }
        if (++ptrFtp >= &ftp_instance[FTP_CONNECTIONS]) {
            return APP_REJECT;                                           // ignore if socket handle not found
        }
    }

#if defined DATA_PORT_TRIGGERS_CONTROL                                   // {8}
    fnTCP_Activity(ptrFtp->FTP_TCP_socket);                              // retrigger control socket on data socket activity
#endif

    switch (ucEvent) {
    case TCP_EVENT_CONNECTED:
        ptrFtp->ucMode &= ~(DATA_UNKNOWN);                               // {33} reset DATA_UNKNOWN flag on a new data connection
#if defined FTP_PASV_SUPPORT
        if (MODE_PASV & ptrFtp->ucMode) {
            ptrFtp->ucMode |= DATA_CONNECTED;                            // mark that we are connected (passive mode)
        }
        else {
            fnQueueSendFTP(MSG_FTP_DATA, ptrFtp);                        // acknowledge command since data connection has been established
        }
#else
        fnQueueSendFTP(MSG_FTP_DATA, ptrFtp);                            // acknowledge command since data connection has been established
#endif
        if (DO_LIST == ptrFtp->ucFTP_action) {                           // we must send a directory listing
            ptrFtp->usLastFrameFileCount = 0;                            // {23}
            ptrFtp->usFilesSent = 0;
            if (fnSendFTP(MSG_DIR, ptrFtp) == 0) {                       // now return the data [FTP-Data] {18}
                fnQueueSendFTP(MSG_DIR_OK, ptrFtp);
                ptrFtp->ucFTP_action = DATA_INACTIVE;
                fnTCP_close(ptrFtp->FTP_TCP_Data_socket);
                return APP_REQUEST_CLOSE;
            }
        }
        else if (DO_SAVE == ptrFtp->ucFTP_action) {                      // we must receive a file and save it
            ptrFtp->ucFTP_action = DOING_SAVE;
        }
#if defined FTP_SUPPORTS_DOWNLOAD
        else if (DO_UPLOAD == ptrFtp->ucFTP_action) {                    // waiting for connection in order to start the data transfer
            int iRtn = (fnSendFTP(MSG_FTP_DATA, ptrFtp) > 0);
            fnSendNextDataBlock(ptrFtp);                                 // {37} send the first block of file data (or terminate if the file is empty)
            return iRtn;
        }
#endif
        else {                                                           // {33}
            ptrFtp->ucMode |= DATA_UNKNOWN;                              // we don't know what the data link will be used for yet
        }
        break;

    case TCP_EVENT_ACK:                                                  // all outstanding data transmission acknowledged
        if (DO_LIST == ptrFtp->ucFTP_action) {
            if (fnSendFTP(MSG_DIR, ptrFtp) == 0) {                       // continue sending file names or terminate
                fnQueueSendFTP(MSG_DIR_OK, ptrFtp);
                ptrFtp->ucFTP_action = DATA_INACTIVE;
                fnTCP_close(ptrFtp->FTP_TCP_Data_socket);
                return APP_REQUEST_CLOSE;
            }
        }
#if defined FTP_SUPPORTS_DOWNLOAD
        else if (DOING_UPLOAD == ptrFtp->ucFTP_action) {
    #if defined FTP_DATA_WINDOWS                                         // {12}
            present_tcp->usOpenCnt = 0;                                  // no outstanding data to be acked
    #endif
            ptrFtp->FileOffset += ptrFtp->usLastSentSize;                // block was successfully received so we can send subsequent ones
            return (fnSendNextDataBlock(ptrFtp));                        // {37} send next block (or close when completed)
        }
#endif
        break;

    case TCP_EVENT_CLOSED:
        if (DOING_SAVE == ptrFtp->ucFTP_action) {
#if defined FTP_UTFAT                                                    // {17}
            if (ptr_utDirectory->usDirectoryFlags & UTDIR_VALID) {
                utCloseFile(&utFile);
            }
            else {
#endif
#if defined ACTIVE_FILE_SYSTEM
    #if defined SUPPORT_MIME_IDENTIFIER
                uFileCloseMime(ptrFtp->ptrFile, &ptrFtp->ucMimeType);    // this will cause the file length and type to be written in the file
    #else
                uFileClose(ptrFtp->ptrFile);                             // this will cause the file length to be written in the file
    #endif
#endif
#if defined FTP_UTFAT                                                    // {17}
            }
#endif
            fnQueueSendFTP(MSG_DIR_OK, ptrFtp);
            ptrFtp->ucFTP_action = DATA_INACTIVE;
        }
        else {
            fnSendFTP(fnGetQueue(ptrFtp), ptrFtp);                       // data socket has closed so kick any waiting commands
        }
#if defined FTP_PASV_SUPPORT
        ptrFtp->ucMode = MODE_ACTIVE_FTP;                                // go to active mode after data connection has close
#endif
        break;

    case TCP_EVENT_DATA:                                                 // we have new receive data
        if (DOING_SAVE == ptrFtp->ucFTP_action) {                        // if the data is to be saved to disk
            ptrFtp->ucMode &= ~(DATA_UNKNOWN);                           // {33}
#if defined FTP_UTFAT                                                    // {17}
            if (ptr_utDirectory->usDirectoryFlags & UTDIR_VALID) {
                utWriteFile(&utFile, ucIp_Data, usPortLen);              // it is assumed that this will not fail
                break;
            }
#endif
#if defined ACTIVE_FILE_SYSTEM
            uFileWrite(ptrFtp->ptrFile, ucIp_Data, usPortLen SUBFILE_WRITE); // save the received data to file. Existing files will automatically be deleted
#endif
        }
        else if (ptrFtp->ucMode & DATA_UNKNOWN) {                        // {33} if we receive data when not saving it may be because the data is arriving before the reason has been established - we reject rather than saving this locally since it will be repeated
            return APP_REJECT_DATA;
        }
        break;

    case TCP_EVENT_REGENERATE:                                           // we must repeat the last data buffer we sent
#if defined FTP_UTFAT                                                    // {17}
        if (ptr_utDirectory->usDirectoryFlags & UTDIR_VALID) {           // if using SD-card
            if (ptrFtp->usFilesSent != 0) {
                uMemcpy(&utListDirectory, &utListLastDirectory, sizeof(utListDirectory)); // return to last list position in order to repeat
                fileList.usMaxItems = 0xffff;                            // the maximum number of entries will be limited by the TCP buffer space
            }
            else {
                utSeek(&utFile, -(ptrFtp->usLastSentSize), UTFAT_SEEK_CUR);                
            }
        }
        else {
            ptrFtp->usFilesSent = ptrFtp->usLastFrameFileCount;          // return listing position to last frame start
        }
#else
        ptrFtp->usFilesSent = ptrFtp->usLastFrameFileCount;              // return listing position to last frame start
#endif
#if defined FTP_DATA_WINDOWS
        present_tcp->usOpenCnt = 0;
#endif
        return (fnSendFTP(MSG_REPEAT_DATA, ptrFtp));

    case TCP_EVENT_CLOSE:                                                // remote client wants to close
        if (ptrFtp->ucMode & DATA_UNKNOWN) {                             // {33} if we haven't established the reason for the connection we ignore the close to avoid missing any data that will be repeated
            return APP_REJECT_DATA;                                      // - this happens when the FTP server sends short data content before sending the STOR command, whereby we have rejected the data (will be repeated) since we don't want to have to store it locally untill it is known where to save it to
        }
    case TCP_EVENT_ABORT:
        break;

    case TCP_EVENT_CONREQ:
#if defined FTP_PASV_SUPPORT
        if (MODE_PASV & ptrFtp->ucMode) {
            ptrFtp->FTP_TCP_Data_socket = Socket;                        // {29}
            break;                                                       // allow PASV connection
        }
#endif
        return APP_REJECT;                                               // we never accept requests

    default:
        break;                                                           // {29} return -1;
    }
    return APP_ACCEPT;
}

// Extract IP address and data port from the input message
// format 192,168,0,33,4,46 when IPv4 is in operation
// format is |2|2001:470:21:105::10|4234| when IPv6 is in operation
//
static int fnGetDataPort(CHAR *cIP_Port, int iIPv6, FTP_INSTANCE *ptrFtp)// {24}
{
    int iCnt = 0;
#if defined USE_IPV6
    CHAR cDelimiter = 0;
    if (iIPv6 != 0) {
        CHAR *cPort = cIP_Port;
        CHAR cDelimiter = *cPort;                                        // delimiter used (range 33..126 but typically '|' [124])
        cPort += 3;
        while (*cPort != cDelimiter) {
            cPort++;
        }
        *cPort = 0;                                                      // replace IPv6 end delimiter with null teminator
        ptrFtp->ulIPv6_flags = TCP_CONNECT_IPV6;                         // flag which type of connection the data will be transferred over
        cIP_Port = (CHAR *)fnStrIPV6((cIP_Port + 3), ptrFtp->ipFTP_data);// extract the IPv6 address (assume |2| preceeds)
        ptrFtp->usFTP_DataPort = 0;
        goto _IPv6_Port;
    }
    else {
        ptrFtp->ulIPv6_flags = 0;
    }
#endif
    cIP_Port = (CHAR *)fnStrIP(cIP_Port, ptrFtp->ipFTP_data);            // extract the IP address

    while (*cIP_Port >= '0') {
        iCnt++;
        cIP_Port++;
    }
    *cIP_Port = '+';                                                     // terminate (we use this due to internet use)
    ptrFtp->usFTP_DataPort = (unsigned short)fnDecStrHex(cIP_Port - iCnt);
    ptrFtp->usFTP_DataPort <<= 8;
    cIP_Port++;

    iCnt = 0;
#if defined USE_IPV6
_IPv6_Port:
    while ((*cIP_Port >= '0') && (*cIP_Port != cDelimiter)) {
        iCnt++;
        cIP_Port++;
    }
#else
    while (*cIP_Port >= '0') {
        iCnt++;
        cIP_Port++;
    }
#endif
    *cIP_Port = 0;                                                       // terminate
    ptrFtp->usFTP_DataPort += (unsigned short)fnDecStrHex(cIP_Port - iCnt);
#if defined FTP_VERIFY_DATA_PORT                                         // {4}
    if ((iIPv6 == 0) && (!(uMemcmp(cucNullMACIP, ptrFtp->ipFTP_data, IPV4_LENGTH)))) { // check for IPv4 address 0.0.0.0
        return (fnSendFTP(MSG_BAD_DATA_CONNECTION, ptrFtp));             // bad data port - this has been seen to happen with FTP from DOS command line
    }
#endif
    return (fnSendFTP(MSG_FTP_OK, ptrFtp));
}


#if defined FTP_PASV_SUPPORT
static unsigned short fnDoGetPasvPort(CHAR *cMessage, int iIPv6, FTP_INSTANCE *ptrFtp)
{
    CHAR *ptr = cMessage;
    int i;

    uMemcpy(cMessage, ucPasvResponse, sizeof(ucPasvResponse));
    #if defined USE_IPV6
    if (iIPv6 != 0) {
        cMessage[2] = '9';                                               // modify response code to 229 (extended passive mode entered)
        cMessage[4] = 'E';
        cMessage[5] = 'P';                                               // modify text to EPSV MODE
        cMessage += sizeof(ucPasvResponse);
        *cMessage++ = '|';
        *cMessage++ = '|';
        *cMessage++ = '|';
        cMessage = _fnBufferDec((unsigned long)(ptrFtp->usFTP_DataPort), 0, cMessage); // add port
        *cMessage++ = '|';
        *cMessage++ = ')';
    }
    else {
        cMessage += sizeof(ucPasvResponse);
        for (i = 0; i < sizeof(network[DEFAULT_NETWORK].ucOurIP); i++) {
            cMessage = _fnBufferDec((unsigned long)network[DEFAULT_NETWORK].ucOurIP[i], 0, cMessage); // add IP address
            *cMessage++ = ',';
        }
        cMessage = _fnBufferDec((unsigned long)(ptrFtp->usFTP_DataPort/256), 0, cMessage); // add port
        *cMessage++ = ',';
        cMessage = _fnBufferDec((unsigned long)((unsigned char)(ptrFtp->usFTP_DataPort)), 0, cMessage); // add port
        *cMessage++ = ')';
        *cMessage++ = '.';
    }
    #else
    cMessage += sizeof(ucPasvResponse);
    for (i = 0; i < sizeof(network[DEFAULT_NETWORK].ucOurIP); i++) {
        cMessage = _fnBufferDec((unsigned long)network[DEFAULT_NETWORK].ucOurIP[i], 0, cMessage); // add IP address
        *cMessage++ = ',';
    }
    cMessage = _fnBufferDec((unsigned long)(ptrFtp->usFTP_DataPort/256), 0, cMessage); // add port
    *cMessage++ = ',';
    cMessage = _fnBufferDec((unsigned long)((unsigned char)(ptrFtp->usFTP_DataPort)), 0, cMessage); // add port
    *cMessage++ = ')';
    *cMessage++ = '.';
    #endif
    *cMessage++ = '\r';
    *cMessage++ = '\n';
    return (unsigned short)(cMessage - ptr);
}
#endif

#if defined FTP_SUPPORTS_DOWNLOAD
static unsigned short fnDoGetFileLen(CHAR *cMessage, FTP_INSTANCE *ptrFtp)
{
    CHAR *ptr = cMessage;
    uMemcpy(cMessage, ucSizeResponse, sizeof(ucSizeResponse));
    cMessage += sizeof(ucSizeResponse);
    cMessage = _fnBufferDec(ptrFtp->FileOffset, 0, cMessage);
    *cMessage++ = '\r';
    *cMessage++ = '\n';
    return (unsigned short)(cMessage - ptr);
}

    #if defined FTP_UTFAT                                                // {17}
static unsigned short fnGetFileData(MEMORY_RANGE_POINTER ptrThisFile, MAX_FILE_SYSTEM_OFFSET ThisFileOffset, unsigned char *ptrBuffer, unsigned short usTxLength)
{
    if ((ptr_utDirectory->usDirectoryFlags & UTDIR_VALID) && (utFile.ulFileSize != 0)) {
        utReadFile(&utFile, ptrBuffer, usTxLength);
        return utFile.usLastReadWriteLength;
    }
        #if defined ACTIVE_FILE_SYSTEM
    return ((unsigned short)uGetFileData(ptrThisFile, ThisFileOffset, ptrBuffer, usTxLength)); // when not working with SD-card use standard uFileSystem call
        #else
    return 0;
        #endif
}
    #endif
#endif                                                                   // end FTP_SUPPORTS_DOWNLOAD

#if defined INTERNAL_USER_FILES                                          // {13}
static USER_FILE *fnNextUserFile(USER_FILE *ptrFiles, MAX_FILE_LENGTH *FileLength, unsigned char *ucMimeType)
{
    while ((ptrFiles != 0) && (ptrFiles->fileName != 0)) {
        if (ptrFiles->ucProperties & FILE_INVISIBLE) {
            ptrFiles++;                                                  // jump the invisible file
            continue;
        }
        *FileLength = ptrFiles->file_length;
        *ucMimeType = ptrFiles->ucMimeType;
        return ptrFiles;
    }
    return 0;                                                            // no more user files
}
#endif

#if defined ACTIVE_FILE_SYSTEM
// Create a file listing message - putting the contents directly to buffer
//
static unsigned short fnDoDir(CHAR *cMessage, FTP_INSTANCE *ptrFtp)
{
#if defined FTP_SUPPORTS_NAME_DISPLAY
    CHAR *ptrRef;
    int iFileNameLength;
    signed short sCompensation = 0;                                      // {36} used to monitor when a name can't fit and to optimise buffer usage
    #if defined INTERNAL_USER_FILES                                      // {13}
    USER_FILE *ptrFiles = ptrUserFiles;
    #endif
    #if defined EXTENDED_UFILESYSTEM                                     // {21}
    CHAR cFileName[7 + EXTENDED_UFILESYSTEM]; // = {'0', 'x', 'x', 'x', '.', 'H', 'T', 'M', '\r', '\n'}; // variable length for 'x', 'x', ..
    #else
    CHAR cFileName[7]; // = {'0', '.', 'H', 'T', 'M', '\r', '\n'}; {1}
    #endif
    MEMORY_RANGE_POINTER ucFileAddress;
    unsigned short usTotalLen = 0;                                       // content length prepared
    CHAR *ptr;
    MAX_FILE_LENGTH FileLength;
    MAX_FILES FileCnt = 0;                                               // count of number of files added in this frame
    #if defined SUPPORT_MIME_IDENTIFIER
        #define MIME_TYPE , &ucMimeType
        #define UOPENNEXTFILE uOpenNextMimeFile
        unsigned char ucMimeType;
    #else
        #define UOPENNEXTFILE uOpenNextFile
        #define MIME_TYPE
    #endif
    #if !defined EXTENDED_UFILESYSTEM
    uMemcpy(cFileName, "0.HTM\r\n", 7);                                  // {1}{13} removed leading space
    #endif
    #if defined INTERNAL_USER_FILES                                      // {13}
        #if defined _WINDOWS
    if (user_files_not_in_code != 0) {                                   // {15}
        ptrFiles = (USER_FILE *)fnGetFlashAdd((unsigned char *)ptrFiles);
    }
        #endif
    if ((ptrFiles = fnNextUserFile(ptrFiles, &FileLength, &ucMimeType)) != 0) {
        ucFileAddress = 0;                                               // first process the user files
    }
    else {
        ucFileAddress = UOPENNEXTFILE(0, &FileLength MIME_TYPE  SUB_FILE_ON); // open the first file in the directory and get its length
    }
    #elif defined ACTIVE_FILE_SYSTEM
    ucFileAddress = UOPENNEXTFILE(0, &FileLength MIME_TYPE  SUB_FILE_ON);// open the first file in the directory and get its length
    #endif
    while (FileLength != 0) {                                            // for each file present
        if (FileCnt++ >= ptrFtp->usFilesSent) {                          // if this file is valid for this frame
    #if defined EXTENDED_UFILESYSTEM                                     // {21}
            uMemcpy(cFileName, "0.HTM\r\n", 7);                          // ensure that the default file name is reset before each new listing
    #endif
            ptr = cMessage;                                              // {35} set reference to start of the entry in the buffer
    #if defined INTERNAL_USER_FILES                                      // {13}
            if (ptrFiles != 0) {                                         // {36} internal file - this has a variable file length so we much be sure that we protect the buffer and quit if there will be no room to accept it
        #if defined _WINDOWS
                if (user_files_not_in_code != 0) {
                    sCompensation -= uStrlen(fnGetFlashAdd((unsigned char *)ptrFiles->fileName));
                }
                else {
                    sCompensation -= uStrlen(ptrFiles->fileName);
                }
        #else
                sCompensation -= uStrlen(ptrFiles->fileName);
        #endif
                if (ptrFiles->ucProperties & FILE_ADD_EXT) {             // if an extension will be added
                    sCompensation -= 6;                                  // 4 extra characters (eg. ".BIN") and CR/LF
                }
                else {
                    sCompensation -= 2;                                  // CR/LF
                }
                sCompensation += sizeof(cFileName);                      // adjust the compensation value to offset the real name length
            }
    #endif
            if (usTotalLen > ((FTP_DATA_BUFFER_LENGTH - LENGTH_OF_FILE_INFO) + sCompensation)) { // {36} if there is no room to add the next file (assuming standard length)
                ptrFtp->usLastFrameFileCount = ptrFtp->usFilesSent;      // {23} amount of files listed before this frame, in case we need to repeat
                ptrFtp->usFilesSent += ((FileCnt - ptrFtp->usFilesSent) - 1); // the total number of files listed
                return (usTotalLen);                                     // ensure we don't overwrite available space (return the buffer content length to be sent)
            }
            uMemcpy(cMessage, cFileRights, sizeof(cFileRights));         // first add the file rights
    #if defined INTERNAL_USER_FILES                                      // {13}
            if (ptrFiles != 0) {                                         // internal file - set read-only
                *(cMessage + 2) = '-';                                   // overwrite default attributes in the string
                *(cMessage + 5) = '-';
                *(cMessage + 8) = '-';
            }
    #endif
            cMessage += sizeof(cFileRights);
            uMemcpy(cMessage, cFileType, sizeof(cFileType));             // add file type to the string buffer
            cMessage += sizeof(cFileType);
            ptrRef = cMessage;                                           // {36} mark the start of the length
            cMessage = _fnBufferDec(FileLength, 0, cMessage);            // add file length
            sCompensation += (LENGTH_OF_FILE_LENGTH - (cMessage - ptrRef)); // {36} adjust the compensated size
            uMemcpy(cMessage, cFileDate, sizeof(cFileDate));             // add the fixed file date
            cMessage += sizeof(cFileDate);
    #if defined SUPPORT_MIME_IDENTIFIER
            if (ucMimeType > UNKNOWN_MIME) {      
                ucMimeType = UNKNOWN_MIME;
            }
    #endif
    #if defined INTERNAL_USER_FILES                                      // {13}
            if (ptrFiles != 0) {                                         // this entry is an internal file
        #if defined _WINDOWS
                if (user_files_not_in_code != 0) {                       // {15}
                    cMessage = uStrcpy(cMessage, fnGetFlashAdd((unsigned char *)ptrFiles->fileName)); // add the file name (variable length)
                }
                else {
                    cMessage = uStrcpy(cMessage, ptrFiles->fileName);    // add the file name (variable length)
                }
        #else
                cMessage = uStrcpy(cMessage, ptrFiles->fileName);        // add the file name (variable length)
        #endif
        #if defined STRING_OPTIMISATION                                  // {14}
                if (ptrFiles->ucProperties & FILE_ADD_EXT) {
                    *cMessage++ = '.';
                    uMemcpy(cMessage, cMimeTable[ucMimeType], 3);
                    cMessage += 3;
                }
                *cMessage++ = '\r';                                      // add line termination
                *cMessage++ = '\n';
        #else
                if (ptrFiles->ucProperties & FILE_ADD_EXT) {
                    *(cMessage - 1) = '.';
                    uMemcpy(cMessage, cMimeTable[ucMimeType], 3);
                    cMessage += 4;
                }
                *(cMessage - 1) = '\r';
                *cMessage++ = '\n';
        #endif
            }
            else {
    #endif
                iFileNameLength = sizeof(cFileName);
    #if defined SUPPORT_MIME_IDENTIFIER
                uMemcpy(&cFileName[2], cMimeTable[ucMimeType], 3);       // set file extension {13} change location of extension
    #endif
    #if defined SUB_FILE_SIZE
                cFileName[0] = uGetSubFileName(ucFileAddress);           // copy the file name to the local buffer {13} change location of file name
    #else
        #if defined EXTENDED_UFILESYSTEM                                 // {21}
                if (uGetFileName(ucFileAddress, cFileName) == 0) {       // copy the file name to the local buffer
                    iFileNameLength -= EXTENDED_UFILESYSTEM;             // not extended so remove additional extended length
                    sCompensation += LENGTH_OF_FILE_LENGTH;              // {36} allow additional usage if possible
                }
        #else
                cFileName[0] = uGetFileName(ucFileAddress);              // copy the file name to the local buffer {13} change location of file name
        #endif
    #endif
                uMemcpy(cMessage, cFileName, iFileNameLength);           // add file name
                cMessage += iFileNameLength;
    #if defined INTERNAL_USER_FILES                                      // {13}
            }
    #endif
            usTotalLen += (cMessage - ptr);                              // {35} new length of the complete frame content
        }
    #if defined INTERNAL_USER_FILES                                      // {13}
        if (ptrFiles != 0) {                                             // this entry is an internal file
            if ((ptrFiles = fnNextUserFile(++ptrFiles, &FileLength, &ucMimeType)) != 0) {
                continue;                                                // next user file to be displayed
            }
        }
    #endif
        ucFileAddress = UOPENNEXTFILE(ucFileAddress, &FileLength MIME_TYPE SUB_FILE_ON); // open next file in the uFileSystem
    }
    if (FileCnt != 0) {                                                  // if file system is not empty
        ptrFtp->usLastFrameFileCount = ptrFtp->usFilesSent;              // {23} amount of files listed before this frame, in case we need to repeat
        ptrFtp->usFilesSent += FileCnt;                                  // total number of files listed till now (last frame)
    }
    return usTotalLen;                                                   // return the buffer content length to be sent
#else                                                                    // not FTP_SUPPORTS_NAME_DISPLAY {10}
    static const CHAR cFileNr[] = {'\r', '\n'};                          // no support - we simply count the number of files
    MEMORY_RANGE_POINTER ucFileAddress;
    unsigned char   ucFiles = 0;
    MAX_FILE_LENGTH FileLength;
    CHAR           *ptrMsg;
    #if defined SUPPORT_MIME_IDENTIFIER
        #define MIME_TYPE , &ucMimeType
        #define UOPENNEXTFILE uOpenNextMimeFile
        unsigned char ucMimeType;
    #else
        #define UOPENNEXTFILE uOpenNextFile
        #define MIME_TYPE
    #endif

    if (ptrFtp->usFilesSent != 0) {
        return 0;
    }

    ucFileAddress = UOPENNEXTFILE(0, &FileLength MIME_TYPE  SUB_FILE_ON);// open the first file in the directory and get its length
    while (FileLength) {                                                 // for each file present
        ucFiles++;                                                       // another file found
        ucFileAddress = UOPENNEXTFILE(ucFileAddress, &FileLength MIME_TYPE SUB_FILE_ON);// open next file in the system
    }

    if (!ucFiles) return 0;

    ptrMsg = cMessage;
    cMessage = _fnBufferDec(ucFiles, 0, cMessage);                       // add the number of files found
    uMemcpy(cMessage, cFileNr, sizeof(cFileNr));                         // add terminator
    ptrFtp->usFilesSent += FILE_NAMES_PER_FTP_FRAME;
    return (sizeof(cFileNr) + (cMessage - ptrMsg));                      // return the buffer contnet length to be sent
#endif                                                                   // end not FTP_SUPPORTS_NAME_DISPLAY
}
#endif

// Send a message or regenerate a message
//
static signed short fnSendFTP(unsigned char ucMsg, FTP_INSTANCE *ptrFtp)
{
    USOCKET              Socket = ptrFtp->FTP_TCP_socket;                // assume control socket
    unsigned short       usSize;
    const CHAR           *ptrMsg;
    TCP_FTP_MESSAGE      FTP_Data_Tx;                                    // temporary TCP message

    if (MSG_DO_NOTHING == ucMsg) {
        return 0;
    }

    if (ucMsg <= MSG_REPEAT_CONTROL) {                                   // handle repetitions
        if (ucMsg == MSG_REPEAT_CONTROL) {                               // repeat the previous control command sent
            ucMsg = ptrFtp->ucLastControl;                               // automatically regenerate last data or control message
        }
        else {                                                           // repeat the previous data block sent
            ucMsg = ptrFtp->ucLastData;
            Socket = ptrFtp->FTP_TCP_Data_socket;
        }
    }
    else if (ucMsg >= FIRST_DATA_MESSAGE) {
        ptrFtp->ucLastData = ucMsg;                                      // remember the data message in case it needs to be repeated
        Socket = ptrFtp->FTP_TCP_Data_socket;                            // set data socket instead of default control socket
    }
    else {
        ptrFtp->ucLastControl = ucMsg;                                   // remember the control command in case it needs to be repeated
    }

    switch (ucMsg) {
    case MSG_SERVER_READY:
        usSize = (sizeof(cFTPServerReady) - 1);
        ptrMsg = cFTPServerReady;
        break;

    case MSG_DIR_OK:
        usSize = (sizeof(cFTP_DIR_OK) - 1);
        ptrMsg = cFTP_DIR_OK;
        break;

    case MSG_ENTER_PASS:
        usSize = (sizeof(cFTPEnterPass) - 1);
        ptrMsg = cFTPEnterPass;
        break;

    case MSG_LOG_SUCCESS:
        usSize = (sizeof(cFTPloginSuccessful) - 1);
        ptrMsg = cFTPloginSuccessful;
        break;

    case MSG_LOG_FAILED:
        usSize = (sizeof(cFTPBadPass) - 1);
        ptrMsg = cFTPBadPass;
        break;

    case MSG_FTP_DATA:
        usSize = (sizeof(cFTP_Data) - 1);
        ptrMsg = cFTP_Data;
        break;

    case MSG_FTP_TYPE:
        usSize = (sizeof(cFTPType) - 1);
        ptrMsg = cFTPType;
        break;

    case MSG_FTP_DENIED:
        usSize = (sizeof(cFTPDenied) - 1);
        ptrMsg = cFTPDenied;
        break;

    case MSG_FTP_DIR:
#if defined FTP_UTFAT && UT_FTP_PATH_LENGTH > 0                          // {30}
        if (ptr_utDirectory->usDirectoryFlags & UTDIR_VALID) {
            uMemcpy(FTP_Data_Tx.ucTCP_Message, cFTPDir, 5);              // 257 "
            usSize = uStrlen(ptr_utDirectory->ptrDirectoryPath);
            if (usSize > 2) {
                usSize -= 2;                                             // remove "D:" from front
            }
            if (usSize > (FTP_DATA_BUFFER_LENGTH - 8)) {                 // if the string is too long for the buffer it is cut short
                usSize = (FTP_DATA_BUFFER_LENGTH - 8);
            }
            ptrMsg = uMemcpy(&FTP_Data_Tx.ucTCP_Message[5], &ptr_utDirectory->ptrDirectoryPath[2], usSize);
            ptrMsg += usSize;
            uMemcpy((CHAR *)ptrMsg, &cFTPDir[6], 3);
            usSize = ((unsigned char *)ptrMsg + 3 - FTP_Data_Tx.ucTCP_Message);
            return (fnSendTCP(Socket, (unsigned char *)&FTP_Data_Tx.tTCP_Header, usSize, TCP_FLAG_PUSH) > 0);
        }
#endif
        usSize = (sizeof(cFTPDir) - 1);                                  // return "257 "/" as root directory location
        ptrMsg = cFTPDir;
        break;

    case MSG_FTP_OK:
        usSize = (sizeof(cFTPOK) - 1);
        ptrMsg = cFTPOK;
        break;

    case MSG_DIR_CHANGED:
        usSize = (sizeof(cFTPDirChanged) - 1);
        ptrMsg = cFTPDirChanged;
        break;

#if defined FTP_VERIFY_DATA_PORT                                          // {4}
    case MSG_BAD_DATA_CONNECTION:
        usSize = (sizeof(cFTP_Bad_Port) - 1);
        ptrMsg = cFTP_Bad_Port;
        break;
#endif

#if defined FTP_PASV_SUPPORT
    #if defined USE_IPV6
    case MSG_EPSV_OK:
    #endif
    case MSG_PASV_OK:
        usSize = fnDoGetPasvPort((CHAR *)FTP_Data_Tx.ucTCP_Message, (ucMsg == MSG_EPSV_OK), ptrFtp); // build response with IP and listening port 
        return (fnSendTCP(Socket, (unsigned char *)&FTP_Data_Tx.tTCP_Header, usSize, TCP_FLAG_PUSH) > 0);
#else
    case MSG_NOT_SUPPORTED:
        usSize = (sizeof(cFTP_Not_Supported) - 1);
        ptrMsg = cFTP_Not_Supported;
        break;
#endif

#if defined FTP_SUPPORTS_DELETE
    case MSG_DEL_OK:
        usSize = (sizeof(cFTPDelOK) - 1);
        ptrMsg = cFTPDelOK;
        break;
#endif

#if defined FTP_UTFAT                                                    // {17}
    case MSG_FTP_READY_FOR_RENAME:
        usSize = (sizeof(cFTP_Rename_OK) - 1);
        ptrMsg = cFTP_Rename_OK;
        break;

    case MSG_FTP_RENAME_SUCCESSFUL:
        usSize = (sizeof(cFTP_Rename_Success) - 1);
        ptrMsg = cFTP_Rename_Success;
        break;        
#endif

#if defined FTP_SUPPORTS_DOWNLOAD
    #if defined FTP_UTFAT                                                // {17}
        #define _uGetFileData fnGetFileData
    #else
        #define _uGetFileData uGetFileData
    #endif
    case MSG_FILE_LENGTH:
        usSize = fnDoGetFileLen((CHAR *)FTP_Data_Tx.ucTCP_Message, ptrFtp);
        return (fnSendTCP(Socket, (unsigned char *)&FTP_Data_Tx.tTCP_Header, usSize, TCP_FLAG_PUSH) > 0);

    case MSG_UPLOAD:
    #if defined FTP_DATA_WINDOWS                                         // {12}
        {
            TCP_CONTROL *data_tcp =  fnGetSocketControl(Socket);         // {40} get the tcp control object belonging to the data connection
            unsigned short usTxLength = data_tcp->usTxWindowSize;
            if (usTxLength > FTP_DATA_BUFFER_LENGTH) {                   // if the destination rx buffer can accept a full TCP frame send a full frame
                usTxLength = FTP_DATA_BUFFER_LENGTH;                     // full frame size
            }
            else if (usTxLength < (FTP_DATA_BUFFER_LENGTH/2)) {
                return -1;                                                // destination's receive window is very small so don't send yet
            }
            if ((ptrFtp->usLastSentSize = (unsigned short)_uGetFileData(ptrFtp->ptrFile, ptrFtp->FileOffset, FTP_Data_Tx.ucTCP_Message, usTxLength)) != 0) {
                unsigned short usNextSent;
                signed short sSend = fnSendTCP(Socket, (unsigned char *)&FTP_Data_Tx.tTCP_Header, ptrFtp->usLastSentSize, TCP_FLAG_PUSH);
                if (sSend <= 0) {                                        // if transmission error
                    return sSend;                                        // return transmission status
                }
                data_tcp->usOpenCnt += ptrFtp->usLastSentSize;           // the destination receiver's input buffer size after receiving the previous frame
                usTxLength = (data_tcp->usTxWindowSize - ptrFtp->usLastSentSize);
                if (usTxLength > FTP_DATA_BUFFER_LENGTH) {               // if the destination rx buffer can still accept a full TCP frame send a full frame
                    usTxLength = FTP_DATA_BUFFER_LENGTH;                 // full frame size
                }
                else if (usTxLength == 0) {                              // we can't send further data since the destination's receive window is closed
                    return sSend;
                }
                usNextSent = (unsigned short)_uGetFileData(ptrFtp->ptrFile, (ptrFtp->FileOffset + ptrFtp->usLastSentSize), FTP_Data_Tx.ucTCP_Message, usTxLength );
                if (usNextSent == 0) {                                   // end of file reached
                    return sSend;
                }
                ptrFtp->usLastSentSize += usNextSent;                    // complete outstanding data to be acked
                return (fnSendTCP(Socket, (unsigned char *)&FTP_Data_Tx.tTCP_Header, usNextSent, TCP_FLAG_PUSH)); // send second buffer
            }
            else {
                return 0;                                                // end of file reached
            }
        }
    #else
        if ((ptrFtp->usLastSentSize = (unsigned short)_uGetFileData(ptrFtp->ptrFile, ptrFtp->FileOffset, FTP_Data_Tx.ucTCP_Message, FTP_DATA_BUFFER_LENGTH)) != 0) {
            return (fnSendTCP(Socket, (unsigned char *)&FTP_Data_Tx.tTCP_Header, ptrFtp->usLastSentSize, TCP_FLAG_PUSH));
        }
        else {
            return 0;                                                    // end of file reached
        }
    #endif
#endif

    case MSG_FTP_UNKNOWN:
        usSize = (sizeof(cFTPUnknownCommand) - 1);
        ptrMsg = cFTPUnknownCommand;
        break;

    case MSG_FTP_QUITTING:
        usSize = (sizeof(cFTPTerminating) - 1);
        ptrMsg = cFTPTerminating;
        ptrFtp->ucFTP_state = FTP_STATE_PREPARE_CLOSE;                   // {27} close the connection when the quit has been acknowledged
        break;

                                                                         // data commands
    case MSG_DIR:                                                        // list files
#if defined FTP_UTFAT                                                    // {17} utFAT has priority over other file system types
        if (ptr_utDirectory->usDirectoryFlags & UTDIR_VALID) {
            fileList.ptrBuffer = (CHAR *)FTP_Data_Tx.ucTCP_Message;
            fileList.ucStyle = FTP_TYPE_LISTING;
            fileList.usBufferLength = sizeof(FTP_Data_Tx.ucTCP_Message); // the maximum space in a single TCP frame
            uMemcpy(&utListLastDirectory, &utListDirectory, sizeof(utListDirectory)); // backup the original listing location in case of necessity to repeat
            if (utListDir(&utListDirectory, &fileList) == UTFAT_NO_MORE_LISTING_ITEMS_FOUND) { // generate the directory listing content
                fileList.usMaxItems = 0;                                 // mark that this content is the final content
            }
            if (fileList.usItemsReturned != 0) {
                ptrFtp->usFilesSent = 1;                                 // mark that listing is taking place in case of repetitions
                return (fnSendTCP(Socket, (unsigned char *)&FTP_Data_Tx.tTCP_Header, fileList.usStringLength, TCP_FLAG_PUSH) > 0); // send the frame
            }
            else {
                ptrFtp->usFilesSent = 0;
                return 0;                                                // end of directory listing
            }
        }
#endif
#if defined ACTIVE_FILE_SYSTEM
        if ((usSize = fnDoDir((CHAR *)FTP_Data_Tx.ucTCP_Message, ptrFtp)) != 0) { // create a file listing message
            return (fnSendTCP(Socket, (unsigned char *)&FTP_Data_Tx.tTCP_Header, usSize, TCP_FLAG_PUSH) > 0); // send the frame
        }
        else if (ptrFtp->usFilesSent != 0) {                             // end of list
            return 0;                                                    // nothing more to send
        }
    #if defined FTP_SUPPORTS_NAME_DISPLAY
        else {                                                           // empty directory
            ptrFtp->usFilesSent = 1;                                     // count the display of the entry directory as a file so that the listing terminates after it has been acknowledged
        }
    #else
        else {
            ptrFtp->usFilesSent += FILE_NAMES_PER_FTP_FRAME;
        }
    #endif
        usSize = (sizeof(cFTP_DIR) - 1);
        ptrMsg = cFTP_DIR;
        break;
#else
        ptrFtp->usFilesSent = 0;
        return 0;                                                // end of directory listing
#endif

    default:                                                             // unexpected call value - ignore
        return 0;
    }

    uMemcpy(FTP_Data_Tx.ucTCP_Message, ptrMsg, usSize);
    return (fnSendTCP(Socket, (unsigned char *)&FTP_Data_Tx.tTCP_Header, usSize, TCP_FLAG_PUSH) > 0);
}
#endif

