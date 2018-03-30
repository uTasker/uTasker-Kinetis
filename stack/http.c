/**********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher

    ---------------------------------------------------------------------
    File:      http.c
    Project:   Single Chip Embedded Internet
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2018
    *********************************************************************
    16.05.2007 Rename variable from i to iSessionNumber and remove unnecessary zeroing {1}
    16.05.2007 Add iWebHandlerCommand variable for clarity.                            {2}
    16.05.2007 Protect a loop from not exiting when corrupted data found               {3}
    22.05.2007 Sub-file write support added for devices with large flash granularity
    01.06.2007 Modified user name and password call                                    {4}
    04.06.2007 Corrected web parsing filter on none-html files                         {5}
    22.06.2007 Correct message regeneration in delayed serving state                   {6}
    21.08.2007 Add HTTP windowing tx support (HTTP_WINDOWING_BUFFERS)                  {7}
    23.08.2007 allow displaying short strings (less than 4 bytes) without inserting spaces (improves formular field inputs) (INSERT_SHORT_STRINGS) {8}
    04.10.2007 Introduce WEB_ESCAPE_LEN for variable escaping size                     {9}
    17.11.2007 Enable accurate distinction between simulated and Window's memory       {10}
    14.12.2007 Various improvements and increased efficiency of windowing transmission by avoiding short final frames {11}
    16.12.2007 Cover function for fnWebParGenerator() ->fnWebParGen() which increases efficiency by avoiding short final frames {12}
    06.01.2008 Pass end of buffer space to fnWebParGenerator                           {13}
    06.01.2008 Pass buffer space end to fnInsertHTMLString()                           {14}
    07.01.2008 Simulator flag iFetchingInternalMemory reset on completion (no longer in driver) {15}
    08.01.2008 Rework dynamic insertion parameter solution                             {16}
    09.01.2008 Allow frame length increase when inserting strings                      {17}
    11.01.2008 Improvement when generated frames cannot grow more                      {18}
    12.01.2008 Additional management information to aid dynamic generation             {19}
    12.01.2008 Optionally pass HTTP session information to fnInsertValue()             {20}
    12.01.2008 Apply INSERT_SHORT_STRINGS support always (define removed)
    03.03.2008 Correct transmission length when receiver's window closes               {21}
    03.03.2008 Don't send empty frame when no space for more data in window            {22}
    03.03.2008 Correct dynamic frame references when sending single or non-frames (due to receiver's windows closing) {23}
    13.03.2008 fnStartHTTP's fnInsertRoutine() parameter now uses typedef LENGTH_CHUNK_COUNT as does fnInsertValue. ChunkReference correspondingly defined. {24}
    17.03.2008 Correct web insert cut off                                              {25}
    16.05.2008 Allow return value on GET connection                                    {26}
    07.09.2008 Add plain text post support to enable posting parameters to application {27}
    22.09.2008 Replace simulator flag iFetchingInternalMemory by a HTTP session flag to improve accuracy when frames are repeated {28}
    23.09.2008 Correct remaining length in parameter posting routine                   {29}
    13.11.2008 Correct variable name ucDymamicFlags->ucDynamicFlags at various locations {30}
    13.11.2008 Avoid padding dynamically generated binary content using LAST_DYNAMIC_CONTENT_DATA flag {31}
    13.11.2008 Avoid HTTP probing when sending binary contents                         {32}
    13.11.2008 Add DYNAMIC_DATA_FIXED_LENGTH support to optimise some data transfers   {33}
    29.11.2008 Add support for operation with CHROME                                   {34}
    29.11.2008 Modify plain text post to allow its content to be in first or second TCP buffer (also required for Chrome) {35}
    29.11.2008 Optionally decode ASCII plain text post reception                       {36}
    17.01.2009 Ignore string compares when frame shorter than string                   {37}
    14.02.2009 ucFlag passed variable used rather than a new stack variable            {38}
    14.02.2009 Add SUPPORT_HTTP_CONTINUE to respond to Expect: 100-continue during parameter posting {39}
    14.02.2009 Activate reworked handling of post support using match string table and allowing parameter post over multiple frames {40}
    07.04.2009 Support working with user files from code space                         {41}
    19.06.2009 Add BMP post                                                            {42}
    22.06.2009 Reset open count when regenerating                                      {43}
    01.07.2009 Reduce length of final dynamically generated binary content to avoid zero at end {44}
    15.10.2009 Add plain text HTTP header information - ensures that Internet Explorer correctly displays txt files {45}
    16.10.2009 Allow other file types than HTML to be parsed if desired                {46}
    11.12.2009 Move HTTP_HEADER_CONTENT_INFO as an option to config.h
    04.12.2009 Add optional utFAT support                                              {47}
    10.11.2010 Modify the AVR32 GCC optimisation workaround to be effective with newest version {48}
    28.11.2010 Add HTTP_DEFAULT_UFILE to control default uFileSystem file on web server contact {49}
    08.12.2010 Add optional HTTP session pointer to generator call-back function       {50}
    08.12.2010 Pass full file name to call-back on first GET                           {51}
    08.12.2010 Add WEB_BLACKLIST_IP option flag (originally 0x02 but could clash)      {52}
    16.04.2011 Add variable HTTP port number                                           {53}
    16.04.2011 Modify padding removal length from {44}                                 {54}
    24.10.2011 Change fnServeDelayed() to optionally pass file string rather than just character {55}
    09.12.2011 Correct handling maximum authentication name and password length (8 each) {56}
    15.01.2012 Terminate authentication name and password                              {57}
    02.04.2012 Allow utFAT index directory to be found when it is added during operation and control access {58}
    19.04.2012 Add HTTP_POST_DEFINES_PAGE, HTTP_SET_USERFILE_START_PAGE, HTTP_SERVE_ONLY_USER_FILES {59}
    19.04.2012 Don't allow more than 1 dynamic insertion in a single frame (break if second is detected) {60}
    03.05.2012 Add dependency HTTP_DYNAMIC_CONTENT                                     {61}
    29.05.2012 Add _FIXED_WEB_PAGE_404                                                 {62}
    07.09.2012 Don't reset http session state until after user call back at end of post{63}
    10.11.2012 Don't clear usDirectoryFlags when a file is not found on SD card because it can cause parallel loads to fail {64}
    17.12.2012 Allow HTTP server to work on multiple networks                          {65}
    30.01.2013 Add extended content type support                                       {66}
    04.06.2013 Added HTTP_MALLOC() default                                             {67}
    12.07.2013 Respect peer's MSS                                                      {68}
    09.09.2013 Add option HTTP_REPEAT_DOUBLE_ACKS                                      {69}
    24.09.2013 Correct single frame serving when not working with SUPPORT_MIME_IDENTIFIER {70}
    17.01.2014 Pass directory pointer to utFAT2.0 version of utOpenFile()              {71}
    22.01.2014 Reset post state when a connection terminates prematurely               {72}
    28.02.2014 Pass zero port number when setting HTTP socket back to listening mode   {73}
    21.03.2014 Add HTTP_FILE_REFERENCE                                                 {74}
    28.04.2014 Correct file length when SUPPORT_CHROME is used without HTTP_HEADER_CONTENT_INFO option {75}
    04.06.2014 Modify HTTP_FILE_REFERENCE to use a local string of definable length    {76}
    28.03.2015 Enable content caching                                                  {77}
    07.02.2016 Add web socket support                                                  {78}
    11.02 2016 Parameters for fnStartHTTP() modified                                   {79}

*/

#include "config.h"

#if defined USE_HTTP


/* =================================================================== */
/*                          local definitions                          */
/* =================================================================== */

#define OWN_TASK        TASK_HTTP

#if !defined HTTP_MALLOC                                                 // {67}
    #define HTTP_MALLOC(x) uMalloc((MAX_MALLOC)(x))
#endif
#if !defined WEB_ESCAPE_LEN                                              // {9}
    #define WEB_ESCAPE_LEN 4                                             // standard escape length, eg. "£xyz"
#endif
#if defined SUB_FILE_SIZE                                                // used by some devices with large flash granularity
    #define SUBFILE_WRITE  ,http_session->ucSubFileWrite
#else
    #define SUBFILE_WRITE
#endif
#if defined _WINDOWS && defined FILE404_IN_PROG
    #define fnGetParsFile(x, y, z)   iFetchingInternalMemory = http_session->iFetchingInternalMemory; fnGetParsFile(x, y, z); iFetchingInternalMemory = 0 // {28} synchronise the external flag with the session flag during memory fetch
#endif
#if !defined HTTP_DEFAULT_UFILE
    #define HTTP_DEFAULT_UFILE '0'
#endif

#if defined SUPPORT_PEER_MSS                                             // {68}
    #define HTTP_BUFFER_LENGTH_ http_session->usPeerMSS
#else
    #define HTTP_BUFFER_LENGTH_ HTTP_BUFFER_LENGTH
#endif

#define BOUNDARY_FOUND                1
#define CONTENT_LENGTH_FOUND          2
#define FIRST_BOUNDARY_FOUND          3
#define CONTENT_TYPE_FOUND            4
#define EXPECT_CONTINUE_FOUND         5
#define BINARY_CONTENT_RECOGNISED     6
#define PLAIN_TEXT_CONTENT_RECOGNISED 7
#define WWW_FORM_CONTENT_RECOGNISED   8
#define IMAGE_GIF_CONTENT_RECOGNISED  9
#define IMAGE_BMP_CONTENT_RECOGNISED  10
#define PDF_CONTENT_RECOGNISED        11


/* =================================================================== */
/*                      local structure definitions                    */
/* =================================================================== */

typedef struct stTCP_MESSAGE
{
    TCP_HEADER     tTCP_Header;                                          // reserve header space
    unsigned char  ucTCP_Message[HTTP_BUFFER_LENGTH];
} TCP_MESSAGE;

#if defined HTTP_WINDOWING_BUFFERS
    extern TCP_CONTROL *present_tcp;                                     // {7} global pointer to present tcp socket's control structure
#endif

typedef struct stPOST_HEADER_SEARCH
{
    const CHAR   *ptrSearchStr;
    unsigned char ucLength;
    unsigned char ucEventID;
    unsigned char ucPad[2];                                              // pad each entry to long word boundary
} POST_HEADER_SEARCH;



/* =================================================================== */
/*                 local function prototype declarations               */
/* =================================================================== */

static int fnHTTPListener(USOCKET Socket, unsigned char ucEvent, unsigned char *ucIp_Data, unsigned short usPortLen);
static unsigned short fnWebParGenerator(unsigned char *ptrBuffer, HTTP *http_session, unsigned char *ptrBufferEnd); // {13}
static unsigned short fnWebParGen(unsigned char ptrBuffer[], HTTP *http_session, unsigned short usFrameLength, unsigned short usTx_window); // {12}
#if defined FNGENERATOR_PASS_HTTP                                        // {50}
    static unsigned char (*fnIsSelected)(unsigned char *, HTTP *);
    #define PASS_HTTP_SESSION_POINTER , http_session
#else
    static unsigned char (*fnIsSelected)(unsigned char *);
    #define PASS_HTTP_SESSION_POINTER
#endif
#if defined HTTP_DYNAMIC_CONTENT
    static CHAR *(*fnInsertValue)(unsigned char *, LENGTH_CHUNK_COUNT, unsigned short *, HTTP *); // {24}
    #define PASS_SESSION_INFO , http_session
    #define PASS_SESSION_INFO_TYPE , HTTP *
#else
    static CHAR *(*fnInsertValue)(unsigned char *, LENGTH_CHUNK_COUNT, unsigned short *); // {24}
    #define PASS_SESSION_INFO
    #define PASS_SESSION_INFO_TYPE
#endif
#if defined SUPPORT_WEBSOCKET                                            // {78}
    static int fnWebSocketRx(HTTP *http_session, unsigned char *ptrData, unsigned short usDataLength);
    static int (*fnWebSocketHandler)(unsigned char *, unsigned long, unsigned char, HTTP *);
#endif
static int fnDoWebPage(CHAR *cFileName, HTTP *http_session);
static int (*fnHandleWeb)(unsigned char, CHAR *, HTTP *);
static int fnWebParHandler(unsigned char *ptrptrFile, unsigned short usDataLength, HTTP *http_session);


/* =================================================================== */
/*                             constants                               */
/* =================================================================== */

#if defined HTTP_AUTHENTICATION
    static const CHAR cCredentialsRequired[] = PROJECT_CREDENTIALS;
#endif
static const CHAR cGet[]  = {'G', 'E', 'T'};
#if defined SUPPORT_HTTP_POST
    static const CHAR cPost[] = {'P', 'O', 'S', 'T'};
#endif
#if defined SUPPORT_CHROME                                               // {34} Chrome requires that the web server always sends a HTTP header otherwise it refuses to operate
    static const CHAR cucHTTP_header[] = "HTTP/1.0 200 OK\r\n\r\n";
    #define SIMPLE_HTTP_HEADER_LENGTH    (sizeof(cucHTTP_header) - 1)
    #if !defined _EXTENDED_HTTP_MIME_CONTENT_SUPPORT                     // {66}
        static const CHAR cucHTTP_plain_text[] = "Content-Type: text/plain\r\n\r\n"; // {45}
        #define TEXT_PLAIN_HTTP_HEADER_LENGTH    (sizeof(cucHTTP_plain_text) - 1)
    #endif
#endif
#if defined SUPPORT_HTTP_CONTINUE                                        // {39}
    static const CHAR ccHTTP_continue[] = "*HTTP/1.0 100 cont\r\n\r\n";
    #define HTTP_CONTINUE_HEADER_LENGTH   (sizeof(ccHTTP_continue) - 1)
#endif
#if defined SUPPORT_HTTP_POST
    static const CHAR cBoundary[]         = "boundary=";
    static const CHAR cContLen[]          = "Content-Length:";
    static const CHAR cFirstBoundary[]    = {'\r', '\n', '\r', '\n'};    // 0x0d, 0x0a, 0x0d, 0x0a
    static const CHAR cContentType[]      = "Content-Type: ";
    #if defined SUPPORT_HTTP_CONTINUE
    static const CHAR cExpectContinue[]   = "Expect: 100-continue";
    #endif
    #if defined RESTRICT_POST_CONTENT_TYPE
        #if defined SUPPORT_POST_BINARY
    static const CHAR cContentTypeBinary[]= "application/octet-stream";
        #endif
        #if defined SUPPORT_POST_TEXT || defined PLAIN_TEXT_POST
    static const CHAR cContentTypeText[]  = "text/plain";
        #endif
        #if defined PLAIN_TEXT_POST && defined X_WWW_FORM_URLENCODED
    static const CHAR cContentTypeX_WWW[] = "application/x-www-form-urlencoded";
        #endif
        #if defined SUPPORT_POST_GIF
    static const CHAR cContentTypeImageGIF[] = "image/gif";
        #endif
        #if defined SUPPORT_POST_BMP
    static const CHAR cContentTypeImageBMP[] = "image/bmp";              // {42}
        #endif
        #if defined SUPPORT_POST_PDF
    static const CHAR cContentTypePDF[] = "application/pdf";
        #endif
    #endif

    static const POST_HEADER_SEARCH post_header_analysis[] = {           // list of recognised header strings, with an event ID number for each
        {cBoundary,          (sizeof(cBoundary) - 1),          BOUNDARY_FOUND},
        {cContLen,           (sizeof(cContLen) - 1),           CONTENT_LENGTH_FOUND},
        {cFirstBoundary,      sizeof(cFirstBoundary),          FIRST_BOUNDARY_FOUND},
        {cContentType,       (sizeof(cContentType) - 1),       CONTENT_TYPE_FOUND},
    #if defined SUPPORT_HTTP_CONTINUE
        {cExpectContinue,    (sizeof(cExpectContinue) - 1),    EXPECT_CONTINUE_FOUND},
    #endif
    #if defined RESTRICT_POST_CONTENT_TYPE
        #if defined SUPPORT_POST_BINARY
        {cContentTypeBinary, (sizeof(cContentTypeBinary) - 1), BINARY_CONTENT_RECOGNISED},
        #endif
        #if defined SUPPORT_POST_TEXT || defined PLAIN_TEXT_POST
        {cContentTypeText,   (sizeof(cContentTypeText) - 1),   PLAIN_TEXT_CONTENT_RECOGNISED},
        #endif
        #if defined PLAIN_TEXT_POST && defined X_WWW_FORM_URLENCODED
        {cContentTypeX_WWW,  (sizeof(cContentTypeX_WWW) - 1),  WWW_FORM_CONTENT_RECOGNISED},
        #endif
        #if defined SUPPORT_POST_GIF
        {cContentTypeImageGIF,  (sizeof(cContentTypeImageGIF) - 1), IMAGE_GIF_CONTENT_RECOGNISED},
        #endif
        #if defined SUPPORT_POST_BMP
        {cContentTypeImageBMP,  (sizeof(cContentTypeImageBMP) - 1), IMAGE_BMP_CONTENT_RECOGNISED}, // {42}
        #endif
        #if defined SUPPORT_POST_PDF
        {cContentTypePDF,    (sizeof(cContentTypePDF) - 1),    PDF_CONTENT_RECOGNISED},
        #endif
    #endif
    };
#endif

#define HEADER_MATCH_ENTRIES (sizeof(post_header_analysis) / sizeof(POST_HEADER_SEARCH))

typedef struct stPOST_HEADER_SEARCH_PROGRESS
{
    HTTP *ptrHTTP_session;                                               // {72}
    unsigned char ucMatch[HEADER_MATCH_ENTRIES];
    unsigned char ucState;
} POST_HEADER_SEARCH_PROGRESS;

/* =================================================================== */
/*                     global variable definitions                     */
/* =================================================================== */

/* =================================================================== */
/*                      local variable definitions                     */
/* =================================================================== */

static HTTP        *ptrHTTP = 0;
static TCP_MESSAGE *HTTP_Tx = 0;
static unsigned char ucWebServerMode = 0;                                // the mode of operation, valid for all sockets
#if defined HTTP_UTFAT                                                   // {47}
    static UTDIRECTORY *ptr_utDirectory = 0;                             // single directory object for all users
#endif
#if defined SUPPORT_HTTP_POST
    static POST_HEADER_SEARCH_PROGRESS header_search_state = {0};
#endif


// The user calls this to start the HTTP server
// The user specifies call back functions for optionally handle HTTP parameters and generating its own web site information
//
extern void fnStartHTTP(HTTP_FUNCTION_SET *ptrFunctionSet)               // {79}
{
    int i;
#if defined _VARIABLE_HTTP_PORT                                          // {53}
    unsigned short usServerPort = ptrFunctionSet->usPort;
#else
    unsigned short usServerPort = HTTP_SERVERPORT;
#endif
    if (ptrHTTP == 0) {                                                  // when called, the user defined number of HTTP sockets will be created
        ptrHTTP = (HTTP *)HTTP_MALLOC((NO_OF_HTTP_SESSIONS + NO_OF_HTTPS_SESSIONS) * sizeof(HTTP)); // get memory for socket administration
        HTTP_Tx = (TCP_MESSAGE *)HTTP_MALLOC(sizeof(TCP_MESSAGE));       // get space for Tx message buffer

        for (i = 0; i < (NO_OF_HTTP_SESSIONS + NO_OF_HTTPS_SESSIONS); i++) { // open a socket for each possible session
            ptrHTTP->OwnerTCPSocket = fnGetTCP_Socket(TOS_MAXIMISE_THROUGHPUT, TCP_DEFAULT_TIMEOUT, fnHTTPListener);
#if NO_OF_HTTPS_SESSIONS > 0
            if (i >= NO_OF_HTTP_SESSIONS) {
                usServerPort = HTTPS_SERVERPORT;                         // listen on secure HTTP port number (443)
                ptrHTTP->OwnerTCPSocket |= SECURE_SOCKET_MODE;
            }
#endif
            fnTCP_Listen(ptrHTTP->OwnerTCPSocket, usServerPort, 0);
            ptrHTTP++;
        }
        ptrHTTP -= (NO_OF_HTTP_SESSIONS + NO_OF_HTTPS_SESSIONS);         // set our pointer to first session
#if defined HTTP_UTFAT                                                   // {47}
        if (ptr_utDirectory == 0) {
            ptr_utDirectory = utAllocateDirectory(DISK_D, 0);            // allocate a directory for use by this module associated with D: - no path string
            if ((ptrFunctionSet->ucParameters & WEB_UTFAT_OFF) == 0) {   // {58}
                utServer(0, UTFAT_HTTP_SERVER_ON);                       // enable http server operation with utFAT
            }
        }
#endif
    }
    else {
        HTTP *http_session = ptrHTTP;
        for (i = 0; i < (NO_OF_HTTP_SESSIONS + NO_OF_HTTP_SESSIONS); i++) { // open a socket for each possible session
            if (http_session->OwnerTCPSocket < 0) {                      // if the socket is free
#if NO_OF_HTTPS_SESSIONS > 0
                if (i >= NO_OF_HTTP_SESSIONS) {
                    usServerPort = HTTPS_SERVERPORT;
                    ptrHTTP->OwnerTCPSocket |= SECURE_SOCKET_MODE;
                }
#endif
                http_session->OwnerTCPSocket = fnGetTCP_Socket(TOS_MAXIMISE_THROUGHPUT, TCP_DEFAULT_TIMEOUT, fnHTTPListener);
                fnTCP_Listen(http_session->OwnerTCPSocket, usServerPort, 0);
            }
            http_session++;
        }
    }
    ucWebServerMode    = ptrFunctionSet->ucParameters;                   // parameters to be used when operating a web site

    fnHandleWeb        = ptrFunctionSet->fnWebHandler;                   // set optional call back function to handle web parameter changes
    fnIsSelected       = ptrFunctionSet->fnGenerator;
    fnInsertValue      = ptrFunctionSet->fnInsertRoutine;
#if defined SUPPORT_WEBSOCKET
    fnWebSocketHandler = ptrFunctionSet->fnWebSocketHandler;
#endif
}

// Call to stop a Web server
//
extern void fnStopHTTP(void)
{
    int i;
    HTTP *http_session = ptrHTTP;
    if (http_session == 0) {
        return;                                                          // http use not yet defined so don't try to stop
    }

    for (i = 0; i < (NO_OF_HTTP_SESSIONS + NO_OF_HTTPS_SESSIONS); i++) { // close all sockets
        fnReleaseTCP_Socket(http_session->OwnerTCPSocket);
        http_session->OwnerTCPSocket = -1;
        http_session++;
    }
}

#if defined SUPPORT_DELAY_WEB_SERVING
// Serve a delayed web page (useful when data has to be collected before displaying)
//
    #if defined PASS_SERVER_DELAYED_STRING
extern unsigned char fnServeDelayed(CHAR *cFile, unsigned char ucOption) // {55}
    #else
extern unsigned char fnServeDelayed(CHAR cFile, unsigned char ucOption)
    #endif
{
    HTTP *http_session = ptrHTTP;
    int i;
    unsigned char ucServed = 0;
    #if !defined PASS_SERVER_DELAYED_STRING
    CHAR cFileToServe = cFile;
    #endif

    for (i = 0; i < (NO_OF_HTTP_SESSIONS + NO_OF_HTTPS_SESSIONS); i++) {
        if (http_session->ucState == HTTP_STATE_DELAYED_SERVING) {
            ucServed++;
            http_session->ucState = HTTP_STATE_ACTIVE;
    #if defined PASS_SERVER_DELAYED_STRING
            fnDoWebPage(cFile, http_session);                            // {55} display the page defined by the application
    #else
            fnDoWebPage(&cFileToServe, http_session);                    // display the page defined by the application
    #endif
        }
        http_session++;
    }
    return ucServed;
}
#endif


#if defined HTTP_AUTHENTICATION
// Send a TCP frame requesting credentials to be entered
//
static int fnRequestCreds(HTTP *http_session)
{
    uMemcpy(HTTP_Tx->ucTCP_Message, cCredentialsRequired, (sizeof(cCredentialsRequired) - 1));
    http_session->usUnacked = (sizeof(cCredentialsRequired) - 1);
    http_session->ucState = HTTP_STATE_REQUEST_CREDS;                    // we close connection after receiving an ack to our request
    return (fnSendTCP(http_session->OwnerTCPSocket, (unsigned char *)&HTTP_Tx->tTCP_Header, (sizeof(cCredentialsRequired) - 1), TCP_FLAG_PUSH) > 0);
}
#endif

#if defined FILE404_IN_PROG
    static const CHAR cFile404[] = FILE_404_CONTENT;
#endif

#if defined HTTP_WINDOWING_BUFFERS                                       // {7}
// Send HTTP TCP frame followed by further if windowing allows this. This greatly speeds up web page serving since it
// overcomes pauses caused by delayed acks at the client side.
//
static int fnSendHTTP_windows(HTTP *http_session, unsigned short usLen, unsigned char ucPush)
{
    int iSent;

    if ((iSent = (fnSendTCP(http_session->OwnerTCPSocket, (unsigned char *)&HTTP_Tx->tTCP_Header, usLen, ucPush) > 0)) != 0) {
        http_session->ucOpenWindows++;                                   // we have served one frame, see whether we can already serve a second
        if (http_session->ucOpenWindows < 2) {
            unsigned short tx_window = present_tcp->usTxWindowSize - usLen;
            http_session->DynamicCnt[HTTP_WINDOWING_BUFFERS] = http_session->DynamicCnt[HTTP_WINDOWING_BUFFERS - 1];// {23} fill with previous frame value in case of no data being added
            if ((http_session->FileLength > (http_session->FilePoint + http_session->usUnacked)) && (tx_window != 0)) {
                MAX_FILE_LENGTH Len = (http_session->FileLength - http_session->usUnacked - http_session->FilePoint);
                unsigned short usFrameLength;
                ucPush = 0;                                              // {38} passed variable used rather than a new stack variable
                present_tcp->usOpenCnt = usLen;                          // raw TCP data length of first frame
                usFrameLength = (unsigned short)Len;
                if (usFrameLength > HTTP_BUFFER_LENGTH_) {
                    usFrameLength = HTTP_BUFFER_LENGTH_;                 // limit length to maximum buffer
                }
                if (tx_window < usFrameLength) {                         // if transmission size is limited by receiver's buffer
                    usFrameLength = tx_window;                           // limit length to receiver's buffer size
                }
                else {
                    if (tx_window > HTTP_BUFFER_LENGTH_) {               // ensure that the receiver's window can never be overwritten {11}
                        tx_window = HTTP_BUFFER_LENGTH_;
                    }
                }
    #if defined HTTP_UTFAT                                               // {47}
                if ((ptr_utDirectory->usDirectoryFlags & UTDIR_VALID) && (http_session->utFile.ulFileSize != 0)) {
                    utReadFile(&http_session->utFile, HTTP_Tx->ucTCP_Message, usFrameLength); // read from the file on the SD card
                }
        #if defined ACTIVE_FILE_SYSTEM
                else {
                    fnGetParsFile((http_session->ptrFileStart + http_session->FilePoint + http_session->usUnacked), HTTP_Tx->ucTCP_Message, usFrameLength); // when not working with SD-card use standard uFileSystem call
                }
        #endif
    #else
                fnGetParsFile((http_session->ptrFileStart + http_session->FilePoint + http_session->usUnacked), HTTP_Tx->ucTCP_Message, usFrameLength); // retrieve the data to send
    #endif
                http_session->usUnackedPreviousContent = usFrameLength;  // raw content length
                if (ucWebServerMode & WEB_SUPPORT_PARAM_GEN) {           // {11} (this section has been reworked)
                    usFrameLength = fnWebParGen(HTTP_Tx->ucTCP_Message, http_session, usFrameLength, tx_window);
                }
                else {
                    http_session->usUnacked += usFrameLength;
                }
                if (http_session->usUnackedPreviousContent >= Len) {
                    ucPush = TCP_FLAG_PUSH;                              // set PUSH flag since this is the last frame
                }
                if (usFrameLength != 0) {                                // {22}
                    http_session->usUnackedPrevious = usFrameLength;     // {11} new position for RAW frame content and not only dynamic modified content
                    fnSendTCP(http_session->OwnerTCPSocket, (unsigned char *)&HTTP_Tx->tTCP_Header, usFrameLength, ucPush);
                }
            }
        }
    }
    #if defined _WINDOWS
  //iFetchingInternalMemory = _ACCESS_NOT_IN_CODE;                       // {15}{28} removed to session flag
    #endif
    return iSent;
}
#endif

#if defined _EXTENDED_HTTP_MIME_CONTENT_SUPPORT                          // {66}
static unsigned short fnAddContentType(CHAR *ptrBuffer, const CHAR *ptrType, int iCache)
{
    CHAR *ptrContent = ptrBuffer;
    ptrContent = uStrcpy(ptrContent, "Content-Type: ");
    ptrContent = uStrcpy(ptrContent, ptrType);
    ptrContent = uStrcpy(ptrContent, "\r\n\r\n");
    if (iCache != 0) {                                                   // {77}
        ptrContent = uStrcpy((ptrContent - 2), "Cache-Control: max-age=1800, private\r\n\r\n"); // cache for 30 minutes
    }
    return ((ptrContent - (CHAR *)ptrBuffer) - 2);
}

static unsigned short fnAddMimeContent(unsigned char *ptrBuffer, unsigned char ucMimeType)
{
    switch (ucMimeType) {
    #if defined MIME_TXT
    case MIME_TXT:
        return (fnAddContentType((CHAR *)ptrBuffer, "text/plain", 1));
    #endif
    #if defined MIME_CSS
    case MIME_CSS:
        return (fnAddContentType((CHAR *)ptrBuffer, "text/css", 1));
    #endif
    #if defined MIME_JAVA_SCRIPT
    case MIME_JAVA_SCRIPT:
        return (fnAddContentType((CHAR *)ptrBuffer, "application/javascript", 1));
    #endif
    #if defined MIME_PNG
    case MIME_PNG:
        return (fnAddContentType((CHAR *)ptrBuffer, "image/png", 1));
    #endif
    #if defined MIME_JPG
    case MIME_JPG:
        return (fnAddContentType((CHAR *)ptrBuffer, "image/jpg", 1));
    #endif
    #if defined MIME_BMP
    case MIME_BMP:
        return (fnAddContentType((CHAR *)ptrBuffer, "image/bmp", 1));
    #endif
    #if defined MIME_GIF
    case MIME_GIF:
        return (fnAddContentType((CHAR *)ptrBuffer, "image/gif", 1));
    #endif
    default:
        return 0;
    }
}
#endif

#if defined SUPPORT_CHROME && defined HTTP_HEADER_CONTENT_INFO && defined SUPPORT_MIME_IDENTIFIER // {45}
static unsigned short fnGenerateHTTP_header(unsigned char *pucTCP_Message, HTTP *http_session, unsigned char *ptrPush, int iRepeat)
{
    unsigned short usHTTP_HeaderLength = SIMPLE_HTTP_HEADER_LENGTH;
    unsigned short usLen;
    #if defined SUPPORT_WEBSOCKET
    if (http_session->ucState == HTTP_STATE_WEB_SOCKET_CONNECTION) {
        usHTTP_HeaderLength = 0;
    }
    else {
    #endif
        uMemcpy(pucTCP_Message, cucHTTP_header, SIMPLE_HTTP_HEADER_LENGTH);  // copy the HTTP header to start of each file to be transferred
    #if defined _EXTENDED_HTTP_MIME_CONTENT_SUPPORT                      // {66}
        usHTTP_HeaderLength += fnAddMimeContent((pucTCP_Message + (SIMPLE_HTTP_HEADER_LENGTH - 2)), http_session->ucMimeType);
    #else
        if (PLAIN_TEXTCONTENT(http_session->ucMimeType)) {                   // check whether this type should be declared as plain text content
            uMemcpy((pucTCP_Message + (SIMPLE_HTTP_HEADER_LENGTH - 2)), cucHTTP_plain_text, TEXT_PLAIN_HTTP_HEADER_LENGTH);
            usHTTP_HeaderLength += (TEXT_PLAIN_HTTP_HEADER_LENGTH - 2);
        }
        #endif
        if (iRepeat != 0) {                                                  // repetition so correct the file information with header compensation
            http_session->ptrFileStart += usHTTP_HeaderLength;
            http_session->FileLength -= usHTTP_HeaderLength;
        }
    #if defined SUPPORT_WEBSOCKET
    }
    #endif
    if (http_session->FileLength <= (unsigned short)(HTTP_BUFFER_LENGTH_ - usHTTP_HeaderLength)) {
        usLen = (unsigned short)(http_session->FileLength);              // the transmission is complete after one frame
        *ptrPush = TCP_FLAG_PUSH;
    }
    else {
        usLen = (HTTP_BUFFER_LENGTH_ - usHTTP_HeaderLength);             // full size frame
        *ptrPush = 0;
    }
    #if defined HTTP_UTFAT                                               // {47}
    if ((ptr_utDirectory->usDirectoryFlags & UTDIR_VALID) && (http_session->utFile.ulFileSize != 0)) {
        utSeek(&http_session->utFile, 0, UTFAT_SEEK_SET);                // ensure at start of file
        utReadFile(&http_session->utFile, (pucTCP_Message + usHTTP_HeaderLength), usLen); // read from the file on the SD card
    }
        #if defined ACTIVE_FILE_SYSTEM
    else {
        fnGetParsFile(http_session->ptrFileStart, (pucTCP_Message + usHTTP_HeaderLength), usLen); // when not working with SD-card use standard uFileSystem call
    }
        #endif
    #else
    fnGetParsFile(http_session->ptrFileStart, (pucTCP_Message + usHTTP_HeaderLength), usLen); // get a frame to send
    #endif
    http_session->ptrFileStart -= usHTTP_HeaderLength;                   // compensate transmitted header
    http_session->FileLength += usHTTP_HeaderLength;
    return (usLen + usHTTP_HeaderLength);                                // full frame content size
}
#endif

// Display a specific web page
//
static int fnDoWebPage(CHAR *cFileName, HTTP *http_session)
{
    unsigned short usLen = HTTP_BUFFER_LENGTH_;
    unsigned char ucPush = TCP_FLAG_PUSH;
    MEMORY_RANGE_POINTER ucFile;

#if defined SUPPORT_INTERNAL_HTML_FILES
    if (cFileName != 0) {                                                // predefined internal file is to be displayed
#endif
#if defined HTTP_FILE_REFERENCE                                          // {74}
        uMemcpy(http_session->FileNameReference, cFileName, HTTP_FILE_REFERENCE_LENGTH); // {76}
#endif
#if defined HTTP_UTFAT                                                   // {47}
        if (utServer(ptr_utDirectory, UTFAT_HTTP_SERVER) == UTFAT_SUCCESS) { // {58} if the http root exists and http server access is allowed
          //http_session->utFile.ptr_utDirObject = ptr_utDirectory;
            if ((http_session->ucState > HTTP_STATE_ACTIVE) || (*cFileName < '0')) { // if we have just authenticated, always do start page
                cFileName = DEFAULT_HTTP_FILE;                           // default start side
            }
            else {
                fnWebStrcpy(cFileName, cFileName);                       // convert the file name to ascii
            }
            if (utOpenFile((const CHAR *)cFileName, &http_session->utFile, ptr_utDirectory, UTFAT_OPEN_FOR_READ) != UTFAT_PATH_IS_FILE) { // {71} open a file referenced to the directory object
                http_session->FileLength = 0;                            // file not found
              //ptr_utDirectory->usDirectoryFlags = 0;                   // {64}{58} cause the http root directory to be searched again to be sure that it hasn't been deleted (removed since parallel files being loaded can fail due to this)
            }
            else {
                http_session->FileLength = http_session->utFile.ulFileSize; // the size of the file to be served
    #if defined SUPPORT_MIME_IDENTIFIER
                http_session->ucMimeType = fnGetMimeType(cFileName);     // get the type of file to be served
    #endif
            }
            ucFile = 0;                                                  // not using a uFile
        }
        else {
          //utServer(0, (UTFAT_HTTP_SERVER_ON | UTFAT_HTTP_SERVER_ROOT_RESET)); // prepare reset on next open
#endif
#if defined HTTP_POST_DEFINES_PAGE                                       // {59}
            if (http_session->ucState == HTTP_STATE_POSTING_PLAIN) {
                ucFile = http_session->ptrFile;
                http_session->FileLength = uGetFileLength(ucFile);       // get the length of the file to be displayed
    #if defined SUPPORT_MIME_IDENTIFIER
                http_session->ucMimeType = UNKNOWN_MIME;
    #endif
            }
            else {
#endif
#if defined _FIXED_WEB_PAGE_404                                          // {62}
            http_session->FileLength = 0;                                // force display of 404 error page
#else
            if ((http_session->ucState > HTTP_STATE_ACTIVE) || (*cFileName < '0')) { // if we have just authenticated, always do start page
    #if defined HTTP_SET_USERFILE_START_PAGE                             // {59}
                ucFile = uOpenUserFile(HTTP_START_SIDE);                 // default start side
                if (ucFile == 0) {                                       // if no such file in the user file system
                    *cFileName = HTTP_DEFAULT_UFILE;                     // default start side in uFileSystem {49}
                    ucFile = uOpenFile(cFileName);                       // open the file to be displayed
                }
    #else
                *cFileName = HTTP_DEFAULT_UFILE;                         // default start side {49}
    #endif
            }
    #if defined HTTP_SET_USERFILE_START_PAGE                             // {59}
			else {
    #endif
    #if defined HTTP_SERVE_ONLY_USER_FILES                               // {59}
            ucFile = uOpenUserFile(cFileName);                           // get a user file only
    #elif defined ACTIVE_FILE_SYSTEM
            ucFile = uOpenFile(cFileName);                               // open the file to be displayed
    #endif
    #if defined HTTP_SET_USERFILE_START_PAGE                             // {59}
            }
    #endif
    #if defined SUPPORT_MIME_IDENTIFIER
            http_session->ucMimeType = UNKNOWN_MIME;
    #endif
    #if defined HTTP_SERVE_ONLY_USER_FILES                               // {59}
            if (ucFile != 0) {
                http_session->FileLength = uGetFileLength(ucFile);       // get the length of the file to be displayed
            }
    #elif defined ACTIVE_FILE_SYSTEM
            http_session->FileLength = uGetFileLength(ucFile);           // get the length of the file to be displayed
    #endif
#endif
#if defined HTTP_POST_DEFINES_PAGE                                       // {59}
            }
#endif
            http_session->ucState = HTTP_STATE_ACTIVE;
#if defined HTTP_UTFAT                                                   // {47}
        }
#endif
        if (http_session->FileLength == 0) {                             // file not found
#if defined FILE404_IN_PROG                                              // processors with linear memory can choose to use a 404 file  in code space
            ucFile = ((MEMORY_RANGE_POINTER)cFile404 - FILE_HEADER);
            http_session->FileLength = sizeof(cFile404);
    #if defined SUPPORT_MIME_IDENTIFIER
            http_session->ucMimeType = MIME_HTML;                        // force HTML type
    #endif
    #if defined _WINDOWS                                                 // {10}{28} removed to session flag
            http_session->iFetchingInternalMemory = _ACCESS_IN_CODE;     // inform the simulator that this access is definitively from internal FLASH
    #endif
#else
            ucFile = (MEMORY_RANGE_POINTER)FILE404;                      // no file exists so get the error page
            if ((http_session->FileLength = uGetFileLength(ucFile)) == 0) { // get length of 404 error file from the file system
                fnTCP_close(http_session->OwnerTCPSocket);               // no error file exists so just close connection
                return APP_REQUEST_CLOSE;
            }
#endif
        }
#if defined _WINDOWS
        else {
            http_session->iFetchingInternalMemory = _ACCESS_NOT_IN_CODE; // ensure that the simulator gets page from file system memory {28}
        }
#endif
#if defined SUPPORT_INTERNAL_HTML_FILES                                  // this allows specifying fixed pages in program code rather than in the file system
    }
    else {
#if defined HTTP_FILE_REFERENCE
        uMemset(http_session->FileNameReference, 0, HTTP_FILE_REFERENCE_LENGTH); // {76}
#endif
        ucFile = (http_session->ptrFileStart - FILE_HEADER);
    #if defined _WINDOWS                                                 // {28} removed to session flag
        if (http_session->iFetchingInternalMemory == _ACCESS_FROM_EXT_FLASH) { // allow supressing this
            http_session->iFetchingInternalMemory = _ACCESS_NOT_IN_CODE;
        }
        else {
            http_session->iFetchingInternalMemory = _ACCESS_IN_CODE;     // ensure that the simulator gets page from program memory {10}
        }
    #endif
    }
#endif

#if defined SUPPORT_CHROME && !defined HTTP_HEADER_CONTENT_INFO          // {45}
  //http_session->FileLength += SIMPLE_HTTP_HEADER_LENGTH;               // {34}{75} account for HTTP header
#endif
#if (defined SUPPORT_CHROME && !(defined HTTP_HEADER_CONTENT_INFO && defined SUPPORT_MIME_IDENTIFIER)) // {70}{75}
    if (http_session->FileLength <= (HTTP_BUFFER_LENGTH - SIMPLE_HTTP_HEADER_LENGTH)) {
        usLen = (unsigned short)http_session->FileLength;                // the transmission is complete after one frame
    }
    else {
        usLen -= SIMPLE_HTTP_HEADER_LENGTH;
        ucPush = 0;
    }
#elif !(defined SUPPORT_CHROME && defined HTTP_HEADER_CONTENT_INFO)      // {45}
    if (http_session->FileLength <= HTTP_BUFFER_LENGTH_) {
        usLen = (unsigned short)http_session->FileLength;                // the transmission is complete after one frame
    }
    else {
        ucPush = 0;
    }
#endif
#if defined SUPPORT_MIME_IDENTIFIER
    if ((http_session->ucMimeType == UNKNOWN_MIME)
    #if defined HTTP_UTFAT
        && (ucFile != 0)
    #endif
        ) {
    #if defined INTERNAL_USER_FILES                                      // {41} it is assumed that this is always used together with SUPPORT_MIME_IDENTIFIER
        if (fnGetUserMimeType(ucFile, &http_session->ucMimeType) == 0) {
        #if defined _WINDOWS
            if ((fnUserFileProperties(ucFile) & FILE_NOT_CODE) == 0) {
                http_session->iFetchingInternalMemory = _ACCESS_IN_CODE; // ensure that the simulator gets page from program memory {10}
            }
        #endif
            ucFile -= FILE_HEADER;
        }
    #elif defined ACTIVE_FILE_SYSTEM
        fnGetParsFile((ucFile + sizeof(MAX_FILE_LENGTH)), &http_session->ucMimeType, 1); // get file type
    #endif
    }
#endif
    http_session->ptrFileStart = (ucFile + FILE_HEADER);                 // save start of file address
#if defined SUPPORT_CHROME                                               // {34}
    #if defined HTTP_HEADER_CONTENT_INFO && defined SUPPORT_MIME_IDENTIFIER // {45}
    usLen = fnGenerateHTTP_header(HTTP_Tx->ucTCP_Message, http_session, &ucPush, 0);
    #else
    uMemcpy(HTTP_Tx->ucTCP_Message, cucHTTP_header, SIMPLE_HTTP_HEADER_LENGTH); // copy the HTTP header to start of each file to be transferred
    #if defined HTTP_UTFAT                                               // {47}
    if ((ptr_utDirectory->usDirectoryFlags & UTDIR_VALID) && (http_session->utFile.ulFileSize != 0)) {
        utReadFile(&http_session->utFile, (pucTCP_Message + usHTTP_HeaderLength), usLen); // read from the file on the SD card
    }
    else {
        fnGetParsFile(http_session->ptrFileStart, (HTTP_Tx->ucTCP_Message + SIMPLE_HTTP_HEADER_LENGTH), (usLen/* - SIMPLE_HTTP_HEADER_LENGTH*/)); // when not working with SD-card use standard uFileSystem call {70}
        usLen += SIMPLE_HTTP_HEADER_LENGTH;                              // {70}
    }
    #else
    fnGetParsFile(http_session->ptrFileStart, (HTTP_Tx->ucTCP_Message + SIMPLE_HTTP_HEADER_LENGTH), (usLen/* - SIMPLE_HTTP_HEADER_LENGTH*/)); // get a frame to send {70}
    usLen += SIMPLE_HTTP_HEADER_LENGTH;                                  // {70}
    #endif
    http_session->ptrFileStart -= SIMPLE_HTTP_HEADER_LENGTH;              // offset for the added header
    #endif
#else
    #if defined HTTP_UTFAT                                               // {47}
    if ((ptr_utDirectory->usDirectoryFlags & UTDIR_VALID) && (http_session->utFile.ulFileSize != 0)) {
        utReadFile(&http_session->utFile, (pucTCP_Message + usHTTP_HeaderLength), usLen);
    }
    else {
        fnGetParsFile(http_session->ptrFileStart, (pucTCP_Message + usHTTP_HeaderLength), usLen); // when not working with SD-card use standard uFileSystem call
    }
    #else
    fnGetParsFile(http_session->ptrFileStart, HTTP_Tx->ucTCP_Message, usLen); // get a frame to send
    #endif
#endif
    http_session->usUnacked = usLen;                                     // save the length of the frame
    if ((ucWebServerMode & WEB_SUPPORT_PARAM_GEN) != 0) {                // if the web server is enabled with generator support
#if defined HTTP_WINDOWING_BUFFERS                                       // {21}
        unsigned short usWindow = HTTP_BUFFER_LENGTH_;
        if (present_tcp->usTxWindowSize < HTTP_BUFFER_LENGTH_) {
            usWindow = present_tcp->usTxWindowSize;
        }
        usLen = fnWebParGen(HTTP_Tx->ucTCP_Message, http_session, 0, usWindow); // allow the user to modify or insert data if required {12}
#else
        usLen = fnWebParGen(HTTP_Tx->ucTCP_Message, http_session, 0, HTTP_BUFFER_LENGTH_); // allow the user to modify or insert data if required {12}
#endif
    }
#if defined HTTP_WINDOWING_BUFFERS                                       // {7}
    present_tcp->usOpenCnt = 0;
    return (fnSendHTTP_windows(http_session, usLen, ucPush));
#else
    #if defined _WINDOWS
  //iFetchingInternalMemory = _ACCESS_IN_CODE;                           // {15} leave the mode (only simulator) {28} removed to session flag
    #endif
    if (fnSendTCP(http_session->OwnerTCPSocket, (unsigned char *)&HTTP_Tx->tTCP_Header, usLen, ucPush) > 0) {
        return APP_SENT_DATA;
    }
    else {
        return APP_ACCEPT;
    }
#endif
}

#if defined SUPPORT_HTTP_POST
    #if defined AUTO_ASCII_PLAIN_TEXT_DECODE
static void fnAutoASCIIdecode(CHAR *cInput, HTTP *http_session)          // {36}
{
    CHAR *ptrEnd;
    *(cInput + http_session->FileLength) = 0;                            // terminate the input so that it can be handled as a string
    ptrEnd = fnWebStrcpy(0, cInput);
    http_session->FileLength = ((ptrEnd - cInput) - 1);                  // new length
}
    #endif

#define HTTP_HEADER_IDLE              0x00
#define HTTP_HEADER_SEARCHING         0x01
#define EXPECTING_CONTENT_TYPE        0x02
#define CONTENT_LENGTH_KNOWN          0x04
#define COUNTING_BOUNDARY             0x08
#define EXPECTING_DATA                0x10
#define EXPECTING_BOUNDARY            0x20
#define BOUNDARY_EXISTS               0x40

static void fnResetPost(HTTP *http_session)                              // {72}
{
    if (header_search_state.ucState != 0) {                              // posting is active
        if (http_session == header_search_state.ptrHTTP_session) {       // posting active on closed connection
            header_search_state.ucState = 0;                             // reset post state so that other sockets can use it
        }
    }
}

    #if defined PLAIN_TEXT_POST
static int fnParameterPost(HTTP *http_session, CHAR *cParameters, unsigned short usParameterLength, unsigned char *ucSearchState)
{
    MAX_FILE_LENGTH remaining_length = 0;
    unsigned char ucPostType;
    if (usParameterLength < http_session->FileLength) {
        http_session->ucState = HTTP_STATE_DOING_PARAMETER_POST;         // parameter post over multiple frames
        if (usParameterLength == 0) {                                    // no parameters actually contained in this frame
            return APP_ACCEPT;
        }
        remaining_length = http_session->FileLength - usParameterLength;
        http_session->FileLength = usParameterLength;                    // the amount of parameter data in this frame
        ucPostType = POSTING_PARTIAL_PARAMETER_DATA_TO_APP;              // flag that this is a partial post
    }
    else {
        ucPostType = POSTING_PARAMETER_DATA_TO_APP;                      // the contents close a post
        http_session->ucState = HTTP_STATE_ACTIVE;
        *ucSearchState = HTTP_HEADER_IDLE;                               // post has completed and new ones can be accepted
    }
        #if defined AUTO_ASCII_PLAIN_TEXT_DECODE
    fnAutoASCIIdecode(cParameters, http_session); 
        #endif
    fnHandleWeb(ucPostType, cParameters, http_session);                  // post content to application
    if (ucPostType == POSTING_PARTIAL_PARAMETER_DATA_TO_APP) {
        http_session->FileLength = remaining_length;                     // remaining data to be received
        return APP_ACCEPT;
    }
        #if defined HTTP_POST_DEFINES_PAGE                               // {59}
    http_session->ucState = HTTP_STATE_POSTING_PLAIN;
        #endif
    return (fnDoWebPage(&http_session->cDisplayFile, http_session));     // serve page on completion
}
    #endif

// This routine assumes sequence is boundary, length, file start
//
static int fnPostFrame(HTTP *http_session, unsigned char *ucIp_Data, unsigned short usPortLen) // {40}
{ 
    int iPostRtn = APP_ACCEPT;

    if (http_session->ucState == HTTP_STATE_START_POST) {                // on start, check to see whether a post is possible
        if (header_search_state.ucState != 0) {
            return APP_REJECT_DATA;                                      // a post is already in progress so this one must wait
        }
        header_search_state.ptrHTTP_session = http_session;              // {72} remember which HTTP session is posting
        http_session->ucState = HTTP_STATE_POSTING;                      // post begins by interpreting the header
        header_search_state.ucState = HTTP_HEADER_SEARCHING;             // mark initial state
        http_session->ucBoundaryLength = 0;
    }
    if (http_session->ucState <= HTTP_STATE_POSTING) {                   // handling header
        while (usPortLen) {                                              // handle posted data content
            if (header_search_state.ucState & EXPECTING_BOUNDARY) {      // a mime content is now expected
                http_session->FileLength--;                              // reduce the length by boundary and type details
            }
            if (header_search_state.ucState & COUNTING_BOUNDARY) {
                if (*ucIp_Data == '\r') {
                    header_search_state.ucState &= ~COUNTING_BOUNDARY;   // end of boundary has been found
                    if (header_search_state.ucState & CONTENT_LENGTH_KNOWN) {
                        continue;
                    }
                }
                else {
                    http_session->ucBoundaryLength++;                    // count boundary length
                }
            }
            else {
                int iHeaderParser = 0;
                while (iHeaderParser < HEADER_MATCH_ENTRIES) {           // check input byte with header parser list
                    if (*ucIp_Data == post_header_analysis[iHeaderParser].ptrSearchStr[header_search_state.ucMatch[iHeaderParser]]) {
                        if (++header_search_state.ucMatch[iHeaderParser] == post_header_analysis[iHeaderParser].ucLength) { // increment match counter
                            switch (post_header_analysis[iHeaderParser].ucEventID) {
                            case BOUNDARY_FOUND:
                                header_search_state.ucState |= (COUNTING_BOUNDARY | BOUNDARY_EXISTS);
                                header_search_state.ucState &= ~EXPECTING_CONTENT_TYPE;
                                break;
                            case CONTENT_LENGTH_FOUND:
                                {
                                    CHAR *ptrLength = (CHAR *)(ucIp_Data + 2);
                                    header_search_state.ucState |= CONTENT_LENGTH_KNOWN;
                                    http_session->FileLength = (MAX_FILE_LENGTH)fnDecStrHex(ptrLength); // size of data content
                                }
                                break;
                            case FIRST_BOUNDARY_FOUND:
                                if ((header_search_state.ucState & CONTENT_LENGTH_KNOWN) != 0) {
        #if defined PLAIN_TEXT_POST                                      //  allow post of parameters
                                    if (http_session->ucBoundaryLength == 0) {
                                        return (fnParameterPost(http_session, (CHAR *)(ucIp_Data + 1), (unsigned short)(usPortLen - 1), &header_search_state.ucState));
                                    }
        #endif
                                    header_search_state.ucState |= EXPECTING_BOUNDARY;
                                }
                                if ((header_search_state.ucState & (EXPECTING_DATA | EXPECTING_CONTENT_TYPE)) != 0) {
                                    http_session->FileLength -= (2 * (sizeof(cFirstBoundary))); // subtract the opening and closing boundary lengths

                                    http_session->FileLength -= http_session->ucBoundaryLength; // remove closing boundary length
        #if defined WEB_PARAMETER_HANDLING
                                    if (fnHandleWeb(CAN_POST_BEGIN, 0, http_session) != 0) {
                                        http_session->ucState = HTTP_STATE_DUMPING_DATA; // application rejects posting
                                        break;
                                    }
        #endif
        #if defined RESTRICT_POST_CONTENT_TYPE
                                    if ((EXPECTING_DATA & header_search_state.ucState) != 0) {
                                        http_session->ucState = HTTP_STATE_POSTING_DATA;
                                    }
                                    else {
                                        http_session->ucState = HTTP_STATE_DUMPING_DATA;
                                    }
        #else
                                    http_session->ucState = HTTP_STATE_POSTING_DATA;
        #endif
                                }
                                break;
                            case CONTENT_TYPE_FOUND:
                                header_search_state.ucState |= EXPECTING_CONTENT_TYPE;
                                break;
        #if defined RESTRICT_POST_CONTENT_TYPE
            #if defined SUPPORT_POST_TEXT || defined PLAIN_TEXT_POST
                            case PLAIN_TEXT_CONTENT_RECOGNISED:
            #endif
            #if defined PLAIN_TEXT_POST && defined X_WWW_FORM_URLENCODED
                            case WWW_FORM_CONTENT_RECOGNISED:
            #endif
            #if defined SUPPORT_POST_PDF
                            case PDF_CONTENT_RECOGNISED:
            #endif
            #if defined SUPPORT_POST_GIF
                            case IMAGE_GIF_CONTENT_RECOGNISED:
            #endif
            #if defined SUPPORT_POST_BMP
                            case IMAGE_BMP_CONTENT_RECOGNISED:
            #endif
                            case BINARY_CONTENT_RECOGNISED:
                                if ((EXPECTING_CONTENT_TYPE & header_search_state.ucState) != 0) {
                                    header_search_state.ucState &= ~EXPECTING_CONTENT_TYPE;
                                    header_search_state.ucState |= EXPECTING_DATA;
                                }
                                break;
        #endif
        #if defined SUPPORT_HTTP_CONTINUE                                // {39}
                            case EXPECT_CONTINUE_FOUND:
                                #if defined _WINDOWS
                                http_session->iFetchingInternalMemory = _ACCESS_IN_CODE; // inform the simulator that access to this is definitively from internal FLASH
                                #endif
                                uMemcpy(HTTP_Tx->ucTCP_Message, &ccHTTP_continue[1], (HTTP_CONTINUE_HEADER_LENGTH - 1));
                                http_session->usUnacked = (HTTP_CONTINUE_HEADER_LENGTH - 1);
                                http_session->FileLength = HTTP_CONTINUE_HEADER_LENGTH;
                                http_session->FilePoint = 1;             // avoid standard header on regeneration
                                #if defined SUPPORT_MIME_IDENTIFIER
                                http_session->ucMimeType = UNKNOWN_MIME; // ensure repetitions are not parsed
                                #endif
                                http_session->ptrFileStart = (unsigned char *)ccHTTP_continue; // save start of file address
                                if (fnSendTCP(http_session->OwnerTCPSocket, (unsigned char *)&HTTP_Tx->tTCP_Header, (HTTP_CONTINUE_HEADER_LENGTH - 1), TCP_FLAG_PUSH) > 0) {
                                    iPostRtn = APP_SENT_DATA;
                                }
                                break;
        #endif
                            }
                            uMemset(header_search_state.ucMatch, 0, HEADER_MATCH_ENTRIES); // clear match counters                        
                            if (http_session->ucState >= HTTP_STATE_POSTING_DATA) { // header has completed so continue with data content
                                ucIp_Data++;
                                usPortLen--;
                                goto _handle_post_data_content;
                            }
                            break;
                        }
                    }
                    else {
                        header_search_state.ucMatch[iHeaderParser] = 0;
                    }
                    iHeaderParser++;
                }
            }
            ucIp_Data++;
            usPortLen--;
        }
    }
_handle_post_data_content:
#if defined PLAIN_TEXT_POST
    if (http_session->ucState == HTTP_STATE_DOING_PARAMETER_POST) {      // performing parameter post over multiple frames
        return (fnParameterPost(http_session, (CHAR *)ucIp_Data, usPortLen, &header_search_state.ucState));
    }
#endif
    while (usPortLen) {                                                  // handle posted data content
        if (http_session->FileLength < usPortLen) {
            usPortLen = (unsigned short)http_session->FileLength;        // last part of post is in this frame
        }
        if (http_session->ucState != HTTP_STATE_DUMPING_DATA) {
    #if defined SUPPORT_HTTP_POST_TO_APPLICATION
            if (!(http_session->ptrFile)) {
                MAX_FILE_LENGTH FileLength = http_session->FileLength;
                http_session->FileLength = usPortLen;
                fnHandleWeb(POSTING_DATA_TO_APP, (CHAR *)ucIp_Data, http_session);
                http_session->FileLength = FileLength;
            }
            else {
        #if defined ACTIVE_FILE_SYSTEM
                uFileWrite(http_session->ptrFileStart, ucIp_Data, usPortLen SUBFILE_WRITE); // write valid data to file
        #endif
            }
    #else
        #if defined ACTIVE_FILE_SYSTEM
            uFileWrite(http_session->ptrFileStart, ucIp_Data, usPortLen SUBFILE_WRITE);// write valid data to file
        #endif
    #endif
            if ((usPortLen != 0) && (http_session->ptrFileStart == http_session->ptrFile)) {
                http_session->ptrFileStart += FILE_HEADER;
            }
        }
        http_session->FileLength -= usPortLen;
        http_session->ptrFileStart += usPortLen;
        usPortLen = 0;
        if ((http_session->FileLength) == 0) {                           // last data content frame received
            MAX_FILE_LENGTH SavedLength;
    #if defined ACTIVE_FILE_SYSTEM
        #if defined SUPPORT_MIME_IDENTIFIER
            SavedLength = uFileCloseMime(http_session->ptrFile, &http_session->ucMimeType); // this will cause the file length and type to be written in the file
        #else
            SavedLength = uFileClose(http_session->ptrFile);             // this will cause the file length to be written in the file
        #endif
    #endif
            header_search_state.ucState = HTTP_HEADER_IDLE;              // post has completed and new ones can be accepted
            http_session->ptrFileStart = 0;
          //http_session->ucState = HTTP_STATE_ACTIVE;                   // {63} post completed
    #if defined WEB_PARAMETER_HANDLING
        #if defined SUPPORT_HTTP_POST_TO_APPLICATION
            if (http_session->ptrFile == 0) {
                SavedLength = 1;                                         // when post was destined for the application it is always declared as successful
            }
        #endif
            if (SavedLength != 0) {
                SavedLength = fnHandleWeb(INFORM_POST_SUCCESS, 0, http_session);
            }
            else {
                SavedLength = fnHandleWeb(INFORM_POST_FAILED, 0, http_session);
            }
            http_session->ucState = HTTP_STATE_ACTIVE;                   // {63} post completed (don't change state until after the user callback)
            if (SavedLength != 0) {
                return (fnDoWebPage(0, http_session));
            }
    #else
            http_session->ucState = HTTP_STATE_ACTIVE;                   // {63} post completed
    #endif
            return (fnDoWebPage(&http_session->cDisplayFile, http_session));
        }
    }
    return iPostRtn;
}
#endif                                                                   // end SUPPORT_HTTP_POST


#if defined SUPPORT_WEBSOCKET
static int fnWebSocketReception(HTTP *http_session, WEB_SOCKET_FRAME_16_MASKED *WebSocket_data, unsigned short usPortLen)
{
    unsigned long ulPayloadLength;
    unsigned long ulPayloadLengthCountdown;
    unsigned char *ptrPayloadData;
    unsigned char *ptrPayloadDataStart;
    unsigned char *ptrMaskKey;
    if ((WebSocket_data->ucPayloadLength[0] & WEB_SOCKET_MASK) == 0) {   // the client must mask all frames - if an unmasked one is detection it is an error
        return APP_ACCEPT;
    }
    ulPayloadLength = (WebSocket_data->ucPayloadLength[0] & WEB_SOCKET_PAYLOAD_LEN_MASK);
    if (ulPayloadLength == 126) {                                        // extended 16 bit length
        ulPayloadLength = ((WebSocket_data->ucPayloadLength[1] << 8) | (WebSocket_data->ucPayloadLength[2]));
        ptrPayloadData = WebSocket_data->ucPayload;
        ptrMaskKey = WebSocket_data->ucMaskingKey;
    }
    else if (ulPayloadLength == 127) {                                   // extended 64 bit length
      //ullPayloadLength = ((((WEB_SOCKET_FRAME_64_MASKED *)WebSocket_data)->ucPayloadLength[1] << 24) | (((WEB_SOCKET_FRAME_64_MASKED *)WebSocket_data)->ucPayloadLength[2] << 16) | (((WEB_SOCKET_FRAME_64_MASKED *)WebSocket_data)->ucPayloadLength[3] << 8) | (((WEB_SOCKET_FRAME_64_MASKED *)WebSocket_data)->ucPayloadLength[4]));
      //ullPayloadLength <<= 32;
      //ullPayloadLength |= ((((WEB_SOCKET_FRAME_64_MASKED *)WebSocket_data)->ucPayloadLength[5] << 24) | (((WEB_SOCKET_FRAME_64_MASKED *)WebSocket_data)->ucPayloadLength[6] << 16) | (((WEB_SOCKET_FRAME_64_MASKED *)WebSocket_data)->ucPayloadLength[7] << 8) | (((WEB_SOCKET_FRAME_64_MASKED *)WebSocket_data)->ucPayloadLength[8]));
        return APP_ACCEPT;                                               // this length is not supported
    }
    else {
        ptrPayloadData = ((WEB_SOCKET_FRAME_MASKED *)WebSocket_data)->ucPayload;
        ptrMaskKey = ((WEB_SOCKET_FRAME_MASKED *)WebSocket_data)->ucMaskingKey;
    }
    ptrPayloadDataStart = ptrPayloadData;
    ulPayloadLengthCountdown = ulPayloadLength;
    while (ulPayloadLengthCountdown >= 4) {                              // unmask the content
        *ptrPayloadData++ ^= *ptrMaskKey++;
        *ptrPayloadData++ ^= *ptrMaskKey++;
        *ptrPayloadData++ ^= *ptrMaskKey++;
        *ptrPayloadData++ ^= *ptrMaskKey++;
        ptrMaskKey -= 4;
        ulPayloadLengthCountdown -= 4;
    }
    switch (ulPayloadLengthCountdown) {
    case 3:
        *ptrPayloadData++ ^= *ptrMaskKey++;
    case 2:
        *ptrPayloadData++ ^= *ptrMaskKey++;
    case 1:
        *ptrPayloadData++ ^= *ptrMaskKey++;
        break;
    }
    if (fnWebSocketHandler != 0) {
        return (fnWebSocketHandler(ptrPayloadDataStart, ulPayloadLength, WebSocket_data->ucFinOpcode, http_session));
    }
    return APP_ACCEPT;
}

extern int fnWebSocketSend(unsigned char *ptrPayload, unsigned long ulPayloadLength, unsigned char ucOpCode, HTTP *http_session)
{
    WEB_SOCKET_FRAME_UNMASKED *ptrTxMessage = (WEB_SOCKET_FRAME_UNMASKED *)HTTP_Tx->ucTCP_Message;
    unsigned char *ptrDataCopy;
    int iHeaderLength;
    ptrTxMessage->ucFinOpcode = ucOpCode;
    if (ulPayloadLength < 126) {
        ptrTxMessage->ucPayloadLength = (unsigned char)ulPayloadLength;
        iHeaderLength = (sizeof(WEB_SOCKET_FRAME_UNMASKED) - 1);
        ptrDataCopy = ptrTxMessage->ucPayload;
    }
    else {
        ptrTxMessage->ucPayloadLength = 126;                             // use 16 bit length
        ((WEB_SOCKET_FRAME_16_UNMASKED *)ptrTxMessage)->ucPayloadLength[1] = (unsigned char)(ulPayloadLength >> 8);
        ((WEB_SOCKET_FRAME_16_UNMASKED *)ptrTxMessage)->ucPayloadLength[2] = (unsigned char)(ulPayloadLength);
        iHeaderLength = (sizeof(WEB_SOCKET_FRAME_16_UNMASKED) - 1);
        ptrDataCopy = ((WEB_SOCKET_FRAME_16_UNMASKED *)ptrTxMessage)->ucPayload;
    }
    uMemcpy(ptrDataCopy, ptrPayload, ulPayloadLength);
    ulPayloadLength += iHeaderLength;        
    if (fnSendTCP(http_session->OwnerTCPSocket, (unsigned char *)&HTTP_Tx->tTCP_Header, (unsigned short)ulPayloadLength, TCP_FLAG_PUSH) > 0) {
        return APP_SENT_DATA;
    }
    return APP_ACCEPT;
}
#endif


// The HTTP listener, supporting a user defined number of sessions
//
static int fnHTTPListener(USOCKET Socket, unsigned char ucEvent, unsigned char *ucIp_Data, unsigned short usPortLen)
{
    HTTP *http_session = ptrHTTP;
    int iSessionNumber;                                                  //  {1}

    for (iSessionNumber = 0; iSessionNumber < (NO_OF_HTTP_SESSIONS + NO_OF_HTTPS_SESSIONS); iSessionNumber++) {
        if (_TCP_SOCKET_MASK(http_session->OwnerTCPSocket) == _TCP_SOCKET_MASK(Socket)) { // {65} search for the session to handle this event
            switch (ucEvent) {
            case TCP_EVENT_CONREQ:                                       // session requested
                if (http_session->ucState == HTTP_STATE_FREE) {
                    if ((ucWebServerMode & WEB_TRUSTED_IP) != 0) {       // we only allow connection(s) from a trusted address
                        if (fnIsSelected != 0) {
                            *(--ucIp_Data) = 'a';                        // request whether trusted address
                            if (fnIsSelected(ucIp_Data PASS_HTTP_SESSION_POINTER) != IS_SELECTED) {
                                return APP_REJECT;                       // signal TCP to send a RST back to the connection request
                            }
                        }
                    }
                    else if ((ucWebServerMode & WEB_BLACKLIST_IP) != 0) {// {52}
                        if (fnIsSelected != 0) {
                            *(--ucIp_Data) = 'b';                        // request whether blacklisted address
                            if (fnIsSelected(ucIp_Data PASS_HTTP_SESSION_POINTER) == IS_SELECTED) {
                                return APP_REJECT;                       // signal TCP to send a RST back to the connection request
                            }
                        }
                    }
                    http_session->ucState = HTTP_STATE_RESERVED;         // reserve the session if the socket is available
                    http_session->OwnerTCPSocket = Socket;               // {65} add interface, network and VLAN details
                }
                break;

            case TCP_EVENT_ABORT:
            case TCP_EVENT_CLOSED:
                {
#if defined HTTP_DYNAMIC_CONTENT                                         // {19}
                    void *prtBackup = http_session->ptrUserData;
                    if ((prtBackup != 0) && (fnInsertValue != 0)) {
                        fnInsertValue(0, 1, 0, http_session);            // inform user of session termination
                    }
                    uMemset(http_session, 0, sizeof(HTTP));              // terminate present session
                    http_session->ptrUserData = prtBackup;               // don't destroy any user data on heap
#else
                    uMemset(http_session, 0, sizeof(HTTP));              // terminate present session
#endif
                    http_session->OwnerTCPSocket = Socket;
#if defined SUPPORT_HTTP_POST
                    fnResetPost(http_session);                           // {72} terminate post if active but not completed
#endif
                    fnTCP_Listen(Socket, 0, 0);                          // {73} go back to listening state
#if defined _VARIABLE_HTTP_PORT                                          // {53}
//                  fnTCP_Listen(Socket, present_tcp->usLocport, 0);     // go back to listening state (risked setting wrong port number)
#else
//                  fnTCP_Listen(Socket, HTTP_SERVERPORT, 0);            // go back to listening state
#endif
                }
                break;

            case TCP_EVENT_CONNECTED:
                http_session->ucState = HTTP_STATE_ACTIVE;
#if defined SUPPORT_PEER_MSS                                             // {68}
                if (present_tcp->usPeerMSS > HTTP_BUFFER_LENGTH) {
                    http_session->usPeerMSS = HTTP_BUFFER_LENGTH;        // limit to the size of our buffer
                }
                else {
                    http_session->usPeerMSS = present_tcp->usPeerMSS;    // set the peer's MSS for this connection
                }
#endif
#if NO_OF_HTTPS_SESSIONS > 0
                if (Socket >= NO_OF_HTTP_SESSIONS) {
                    return APP_SECURITY_CONNECTED;                       // signal to the TCP level that it should now insert the secure socket layer for further operation
                }
#endif
                break;

#if defined HTTP_WINDOWING_BUFFERS                                       // {7}
    #if !defined HTTP_IGNORE_PARTIAL_ACKS
            case TCP_EVENT_PARTIAL_ACK:                                  // partial window has been acked
                if (http_session->usUnackedPrevious == usPortLen) {      // {11} (section reworked to correct and simplify)                  
                    http_session->usUnacked -= http_session->usUnackedPreviousContent;
                    present_tcp->ulSendUnackedNumber += present_tcp->usOpenCnt;
                    present_tcp->ulNextTransmissionNumber -= usPortLen;
                    http_session->ucOpenWindows--;
        #if defined HTTP_DYNAMIC_CONTENT                                 // {19}
                    uMemcpy(&http_session->DynamicCnt[0], &http_session->DynamicCnt[1], sizeof(http_session->DynamicCnt));
                    http_session->DynamicCnt[HTTP_WINDOWING_BUFFERS] = 0;
        #endif
                    goto _continue_ack;                                  // in some circumstances this leads to unnecessary regeneration of (delayed) frame but is otherwise reliable and non-disturbing
                }
                break;
    #elif defined HTTP_REPEAT_DOUBLE_ACKS                                // {69} with this option partial ACKs can be used cause complete retransmission
            case TCP_EVENT_PARTIAL_ACK:
                if (usPortLen == http_session->usUnacked) {              // the peer has sent an ack back but it is not acknowledging any data that is on the fly - we use this to optionally retransmit all data to avoid timeout delays when the peer seems to have an issue
                    goto _regenerate;                                    // regenerate all non-acked data
                }
                break;
    #endif

            case TCP_WINDOW_UPDATE:                                      // handle TCP windows update as ACK
                if (http_session->FilePoint == 0) {                      // this arrives here after a connection establishment so ignore in this case
                    break;
                }
                // Fall through to ack
                //
    #if !defined HTTP_IGNORE_PARTIAL_ACKS
_continue_ack:
    #endif
#endif    
            case TCP_EVENT_ACK:                                          // the last transmission was successful - we can continue sending next data
                if (http_session->ucState == HTTP_STATE_REQUEST_CREDS) {
                    fnTCP_close(http_session->OwnerTCPSocket);
                    return APP_REQUEST_CLOSE;                            // inform TCP that we have requested a close
                }
                http_session->FilePoint += http_session->usUnacked;      // increment the offset to next part of message
                http_session->usUnacked = 0;                             // reset the unacked value
#if defined HTTP_WINDOWING_BUFFERS                                       // {7}
    #if defined HTTP_DYNAMIC_CONTENT
                if (TCP_EVENT_ACK == ucEvent) {                          // all outstanding data has been acknowledged
                    http_session->DynamicCnt[0] = http_session->DynamicCnt[HTTP_WINDOWING_BUFFERS]; // {19}
                    uMemset(&http_session->DynamicCnt[1], 0, (HTTP_WINDOWING_BUFFERS * sizeof(http_session->DynamicCnt[1])));
                }
    #endif
    #if !defined HTTP_IGNORE_PARTIAL_ACKS                                // {43}
                http_session->ucOpenWindows = 0;
                present_tcp->usOpenCnt = 0;
    #endif
#else
    #if defined HTTP_DYNAMIC_CONTENT
                http_session->DynamicCnt[0] = http_session->DynamicCnt[1];
                http_session->DynamicCnt[1] = 0;
    #endif
#endif
                // Fall through - after an ack we send next part of a block - a regenerate repeats the block
                //
            case TCP_EVENT_REGENERATE:
#if defined HTTP_WINDOWING_BUFFERS && defined HTTP_IGNORE_PARTIAL_ACKS   // {43}
    #if defined HTTP_REPEAT_DOUBLE_ACKS                                  // {69}
_regenerate:
    #endif
                http_session->ucOpenWindows = 0;
                present_tcp->usOpenCnt = 0;
#endif
#if defined HTTP_AUTHENTICATION
                if (http_session->ucState == HTTP_STATE_REQUEST_CREDS) {
                    return (fnRequestCreds(http_session));
                }
#endif
                if (http_session->ucState < HTTP_STATE_ACTIVE) {         // ignore if the server has closed in the meantime {6}
                    return APP_ACCEPT; 
                }
                if (http_session->FileLength > http_session->FilePoint) { // if the complete content has been served and acknowledged
                    MAX_FILE_LENGTH Len = (http_session->FileLength - http_session->FilePoint);
                    unsigned char ucPush = 0;
                    if (Len > HTTP_BUFFER_LENGTH_) {
                        Len = HTTP_BUFFER_LENGTH_;
                    }
                    else {
                       ucPush = TCP_FLAG_PUSH;                           // set PUSH flag so that the block goes to Rx application
                    }
#if defined HTTP_WINDOWING_BUFFERS                                       // {7}
                    if (Len > present_tcp->usTxWindowSize) {                             
                        Len = present_tcp->usTxWindowSize;               // restrict transmission to respect destination window capability
                        if (Len == 0) {                                  // destination's rx window has closed. We probe until there is space again
                            break;                                       // do not send, but wait for a window update
                        }
                    }
                    goto _probe_jumped;                                  // continue by jumping the probed case

            case TCP_WINDOW_PROBE:
                    Len = 1;                                             // persist timer causing windows probe of one byte
                    http_session->ucOpenWindows = 1;                     // ensure only one frame sent
                    ucPush = 0;
_probe_jumped:
#endif
                    http_session->usUnacked = (unsigned short)Len;
#if defined SUPPORT_CHROME                                               // {34}
                    if (http_session->FilePoint == 0) {                  // regeneration of first frame needs to be handled differently to insert the HTTP header
    #if defined HTTP_HEADER_CONTENT_INFO && defined SUPPORT_MIME_IDENTIFIER // {45}
                        Len = fnGenerateHTTP_header(HTTP_Tx->ucTCP_Message, http_session, &ucPush, 1);
    #else
                        uMemcpy(HTTP_Tx->ucTCP_Message, cucHTTP_header, SIMPLE_HTTP_HEADER_LENGTH); // copy the HTTP header to start of each file to be transferred
        #if defined HTTP_UTFAT                                           // {47}
                        if ((ptr_utDirectory->usDirectoryFlags & UTDIR_VALID) && (http_session->utFile.ulFileSize != 0)) {
                            utSeek(&utFile, 0, UTFAT_SEEK_SET);          // set back to start of file
                            utReadFile(&http_session->utFile, (HTTP_Tx->ucTCP_Message + SIMPLE_HTTP_HEADER_LENGTH), (Len - SIMPLE_HTTP_HEADER_LENGTH)); // read from the file on the SD card
                        }
                        else {
                            fnGetParsFile((http_session->ptrFileStart + SIMPLE_HTTP_HEADER_LENGTH), (HTTP_Tx->ucTCP_Message + SIMPLE_HTTP_HEADER_LENGTH), (Len - SIMPLE_HTTP_HEADER_LENGTH)); // when not working with SD-card use standard uFileSystem call
                        }
        #else
                        fnGetParsFile((http_session->ptrFileStart + SIMPLE_HTTP_HEADER_LENGTH), (HTTP_Tx->ucTCP_Message + SIMPLE_HTTP_HEADER_LENGTH), (Len - SIMPLE_HTTP_HEADER_LENGTH)); // get a frame to send
        #endif
    #endif
                    }
                    else {
    #if defined HTTP_UTFAT                                               // {47}
                        if ((ptr_utDirectory->usDirectoryFlags & UTDIR_VALID) && (http_session->utFile.ulFileSize != 0)) {
                            utSeek(&http_session->utFile, (unsigned long)(http_session->ptrFileStart + http_session->FilePoint - FILE_HEADER), UTFAT_SEEK_SET); // set back to start of frame to be repeated
                            utReadFile(&http_session->utFile, HTTP_Tx->ucTCP_Message, (unsigned short)Len); // read from the file on the SD card
                        }
        #if defined ACTIVE_FILE_SYSTEM
                        else {
                            fnGetParsFile((http_session->ptrFileStart + http_session->FilePoint), HTTP_Tx->ucTCP_Message, Len); // when not working with SD-card use standard uFileSystem call
                        }
        #endif
    #else
                        fnGetParsFile((http_session->ptrFileStart + http_session->FilePoint), HTTP_Tx->ucTCP_Message, Len);
    #endif
                    }
#else
    #if defined HTTP_UTFAT                                               // {47}
                    if ((ptr_utDirectory->usDirectoryFlags & UTDIR_VALID) && (http_session->utFile.ulFileSize != 0)) {
                        utReadFile(&http_session->utFile, HTTP_Tx->ucTCP_Message, Len); // read from the file on the SD card
                    }
                    else {
                        fnGetParsFile(http_session->ptrFileStart + http_session->FilePoint), HTTP_Tx->ucTCP_Message, Len); // when not working with SD-card use standard uFileSystem call
                    }
    #else
                    fnGetParsFile((http_session->ptrFileStart + http_session->FilePoint), HTTP_Tx->ucTCP_Message, Len);
    #endif
#endif
                    if ((ucWebServerMode & WEB_SUPPORT_PARAM_GEN) != 0) {
#if defined HTTP_WINDOWING_BUFFERS
                        unsigned short usWindow = HTTP_BUFFER_LENGTH_;
                        if (present_tcp->usTxWindowSize < HTTP_BUFFER_LENGTH_) {
                            usWindow = present_tcp->usTxWindowSize;
                        }
                        Len = fnWebParGen(HTTP_Tx->ucTCP_Message, http_session, 0, usWindow); // {12}{21}
                        if (Len == 0) {                                  // couldn't insert generated string since window too small
    #if defined HTTP_DYNAMIC_CONTENT
                            if ((http_session->ucDynamicFlags & GENERATING_DYNAMIC_BINARY) == 0) { // {32}
                                *HTTP_Tx->ucTCP_Message = ' ';           // send a space as probe because this doesn't influence HTML
                                Len = 1;
                            }
    #else
                            *HTTP_Tx->ucTCP_Message = ' ';               // send a space because this doesn't influence HTML
                            Len = 1;
    #endif
                            http_session->usUnacked = 0;
                            http_session->DynamicCnt[HTTP_WINDOWING_BUFFERS - 1] = http_session->DynamicCnt[0];// {23} fill with previous frame values to maintain dynamic reference
                            http_session->DynamicCnt[HTTP_WINDOWING_BUFFERS] = http_session->DynamicCnt[0];
                        }
#else
                        Len = fnWebParGen(HTTP_Tx->ucTCP_Message, http_session, 0, HTTP_BUFFER_LENGTH_); // {12}{21}
#endif
                    }
#if defined HTTP_WINDOWING_BUFFERS                                       // {7}
                    return (fnSendHTTP_windows(http_session, (unsigned short)Len, ucPush));
#else
                    return (fnSendTCP(http_session->OwnerTCPSocket, (unsigned char *)&HTTP_Tx->tTCP_Header, (unsigned short)Len, ucPush) > 0);
#endif
                }
                else {
#if defined SUPPORT_WEBSOCKET
                    if (http_session->ucState == HTTP_STATE_WEB_SOCKET_CONNECTION) {
                        return APP_ACCEPT;                               // don't close the connection in web socket mode (persistent connection)
                    }
#endif
                    fnTCP_close(http_session->OwnerTCPSocket);
                }
                return APP_REQUEST_CLOSE;

            case TCP_EVENT_DATA:
                {
                int iWebHandlerCommand;                                  // {2}
                if (http_session->ptrFileStart == 0) {
                    if (http_session->ucState < HTTP_STATE_PROCESSING) { // first command after TCP connection
#if defined SUPPORT_HTTP_POST
                        if (uMemcmp(ucIp_Data, (unsigned char *)cGet, sizeof(cGet)) != 0) {
                            if (uMemcmp(ucIp_Data, (unsigned char *)cPost, sizeof(cPost)) != 0) {
                                return APP_REJECT_DATA;                  // we expect the GET command or the POST if not already in a http block [any thing else is ignored]
                            }
                            http_session->ucState = HTTP_STATE_START_POST; // POST command
                        }
#else
                        if (uMemcmp(ucIp_Data, (unsigned char *)cGet, sizeof(cGet))) {
                            return APP_REJECT_DATA;                      // we expect the GET command if not already in a http block [any thing else is ignored]
                        }
#endif
                      //i = 0;                                           // {1}
                        do {
                            if (usPortLen == 0) {
                                return APP_REJECT_DATA;                  // {3}
                            }
                            usPortLen--;                                 
                        }  while (*ucIp_Data++ != '/');                  // search for '/'
                    }

#if defined SUPPORT_HTTP_POST
                    if (http_session->ucState >= HTTP_STATE_START_POST) {// POST handling
                        if (HTTP_STATE_START_POST == http_session->ucState) {
                            MEMORY_RANGE_POINTER ucFile;
    #if defined SUPPORT_HTTP_POST_TO_APPLICATION
        #if defined ACTIVE_FILE_SYSTEM
                            if (*ucIp_Data == '*') {                     // post to application
                                ucFile = 0;
                            }
                            else {
                                ucFile = uOpenFile((CHAR *)ucIp_Data);   // open the file to be written or displayed
                            }
        #else
                            ucFile = 0;                                  // only allow post to the application
        #endif
    #else
                            ucFile = uOpenFile((CHAR *)ucIp_Data);       // open the file to be written or displayed
    #endif
    #if defined EXTENDED_UFILESYSTEM
                            http_session->cDisplayFile = *(CHAR *)(ucIp_Data + 1 + EXTENDED_UFILESYSTEM); // special case, where following letter specifies the file to be displayed after the post
    #else
                            http_session->cDisplayFile = *(CHAR *)(ucIp_Data + 1); // special case, where second letter specifies the file to be displayed after the post
    #endif
    #if defined SUPPORT_MIME_IDENTIFIER
                            http_session->ucMimeType = fnGetMimeType((CHAR *)(ucIp_Data)); // get the type of file being saved
    #endif
    #if defined SUB_FILE_SIZE
                            http_session->ucSubFileWrite = fnGetFileType((CHAR*)(ucIp_Data)); // get file characteristics so that it is later handled correctly
    #endif
                            http_session->ptrFile = ucFile;
    #if defined SUPPORT_HTTP_POST_TO_APPLICATION
                            if ((http_session->ptrFileStart = ucFile) == 0) { // save start of file address
                                http_session->ptrFileStart++;
                            }
    #else
                            http_session->ptrFileStart = ucFile;         // save start of file address
    #endif
    #if defined SUPPORT_CHROME || defined SUPPORT_HTTP_CONTINUE          // {35}
                            http_session->FileLength = 0;                // reset the file length when starting post
    #endif
                        }
                        return (fnPostFrame(http_session, ucIp_Data, usPortLen));
                    }
#endif
#if defined HTTP_DYNAMIC_CONTENT            
                    uMemset(&http_session->DynamicCnt[0], 0, sizeof(http_session->DynamicCnt));// {19}
#endif
                    // Let our parameter handler look at the data. If it wants, it can choose the page to be displayed, request credentials
                    // or let the standard side be displayed.
                    //
                    if ((iWebHandlerCommand = fnWebParHandler(ucIp_Data, (unsigned short)(usPortLen), http_session)) != 0) { // note that the handler has the possibility to change the file name..
                        switch (iWebHandlerCommand) {
                        case PROCESSING_INPUT:                           // if processing input, do nothing here
                            break;
#if defined HTTP_AUTHENTICATION
                        case CREDENTIALS_REQUIRED:
                            return (fnRequestCreds(http_session));       // the handler wants first to see credentials
#endif
#if defined SUPPORT_INTERNAL_HTML_FILES
                        case DISPLAY_INTERNAL:
                            return (fnDoWebPage(0, http_session));       // display internal page defined by application
#endif
#if defined SUPPORT_DELAY_WEB_SERVING
                        case DELAY_SERVING:
                            http_session->ucState = HTTP_STATE_DELAYED_SERVING;
                            return APP_ACCEPT;
#endif
#if defined SUPPORT_WEBSOCKET                                            // {78}
                        case WEB_SOCKET_HANDSHAKE:                       // a web socket handshake has been detected
                            return (fnDoWebPage(0, http_session));       // display internal page defined by application
#endif
                        default:
                            {
                                CHAR cAppPage = (CHAR)iWebHandlerCommand;// convert to file name reference in file system
                                return (fnDoWebPage(&cAppPage, http_session)); // display the page defined by the application
                            }
                        }
                    }
                    else {
                        return (fnDoWebPage((CHAR *)ucIp_Data, http_session)); // the standard file from file system should be displayed according to the file name
                    }
                }
#if defined SUPPORT_WEBSOCKET
                else if (http_session->ucState == HTTP_STATE_WEB_SOCKET_CONNECTION) {
                    return (fnWebSocketReception(http_session, (WEB_SOCKET_FRAME_16_MASKED *)ucIp_Data, usPortLen)); // handle content received
                }
#endif
#if defined SUPPORT_HTTP_POST
                else if (http_session->ucState >= HTTP_STATE_START_POST) { // POSTING data
                    return (fnPostFrame(http_session, ucIp_Data, usPortLen));
                }
#endif
                }
                break;

            case TCP_EVENT_CLOSE:
                break;
            }
            return APP_ACCEPT;
        }
        http_session++;
    }
    return APP_REJECT;                                                   // unexpected occurance - signal rejection... {4}
}


// Add information to the web page being sent
//
static unsigned short fnWebParGenerator(unsigned char *ptrBuffer, HTTP *http_session, unsigned char *ptrBufferEnd) // {13}
{
#if defined WEB_PARAMETER_GENERATION
    unsigned short *usMaxLen;
    unsigned short  usToSend;
    unsigned short  usLen = http_session->usUnacked;
    unsigned short  usInsertLength;
    unsigned char   ucAction;
    const CHAR     *cInsertString;

    #if defined SUPPORT_MIME_IDENTIFIER
    if (http_session->ucMimeType > MIME_HTML) {                          // only parse certain files {5}{46}
        return usLen;                                                    // return original length with no modifications
    }
    #endif

    usMaxLen = &http_session->usUnacked;
    usToSend = usLen;
    ucAction = NOT_SELECTED;

    while (usLen) {                                                      // we scan the HTTP frame to be sent, looking for a position to fill out
        if (*ptrBuffer++ == (unsigned char)WEB_PARSER_START) {           // a field has been found which needs to be filled out
                                                                         // example £sAB - insert select, parameter A / B
            if (usLen < WEB_ESCAPE_LEN) {                                // check whether long enough to ensure correct interpretation in all cases {9}{25}(change <= to <)
                *usMaxLen -= usLen;
                return (usToSend - usLen);                               // parameter list cut off, handle it next time around
            }
            cInsertString = 0;                                           // set null pointer

            switch (*ptrBuffer++) {                                      // check which of our fields this is
            case WEB_DISABLE_FIELD:                                      // disable field
            case WEB_NOT_DISABLE_FIELD:                                  // not disable field
            case WEB_SELECTED_FIELD:                                     // select field
                if (fnIsSelected) {
                    if (*(ptrBuffer - 1) == 's') {
                        ucAction = fnIsSelected(ptrBuffer PASS_HTTP_SESSION_POINTER); // allow application to define its setting
                    }
                    else {
                        ucAction = fnIsSelected((ptrBuffer - 1) PASS_HTTP_SESSION_POINTER); // allow application to define its setting
                    }
                }
                if (ucAction != NOT_SELECTED) {
                    static const CHAR cSelected[] = {'s', 'e', 'l', 'e', 'c',  't', 'e', 'd'};
                    static const CHAR cChecked[]  = {'c', 'h', 'e', 'c', 'k',  'e', 'd'};
                    static const CHAR cDisabled[] = {'d', 'i', 's', 'a', 'b',  'l', 'e', 'd'};
                    if (IS_CHECKED == ucAction) {
                        cInsertString = cChecked;
                        usInsertLength = sizeof(cChecked);
                    }
                    else if (IS_DISABLED == ucAction) {
                        cInsertString = cDisabled;
                        usInsertLength = sizeof(cDisabled);
                    }
                    else {
                        cInsertString = cSelected;
                        usInsertLength = sizeof(cSelected);
                    }
                }
                // If the element is not to be selected, we leave the parameter field unmodified since it will be ignored
                // by the browser displaying the information
                //
                break;

            case WEB_INSERT_STRING:                                      // insert value string field
                if (fnInsertValue) {
                    cInsertString = fnInsertValue(ptrBuffer, 0, &usInsertLength PASS_SESSION_INFO); // allow application to insert its own string
                }
                break;
    #if defined HTTP_DYNAMIC_CONTENT
            case WEB_INSERT_DYNAMIC:                                     // insert dynamic HTML {16}
                if (fnInsertValue) {
                    CHAR cInput[WEB_ESCAPE_LEN - 2];                     // backup of the control tag
                    LENGTH_CHUNK_COUNT ChunkReference;                   // {24}
                    if (http_session->ucDynamicFlags & MAXIMUM_DYNAMIC_INSERTS) { // {60} do not allow a second insertion in the same frame!!
                        http_session->ucDynamicFlags |= QUIT_FRAME_DURING_GENERATION;
                        *usMaxLen -= usLen;
                        return (usToSend - usLen);
                    }
        #if defined HTTP_WINDOWING_BUFFERS
                    ChunkReference = (http_session->DynamicCnt[http_session->ucPresentFrame] + 1); // inform the user about which chunk of content must be inserted
        #else
                    ChunkReference = (http_session->DynamicCnt[0] + 1);
        #endif
                    uMemcpy(cInput, ptrBuffer, sizeof(cInput));          // backup the control tag
                    ptrBuffer += (WEB_ESCAPE_LEN - 2);
                    usLen -= (WEB_ESCAPE_LEN - 1);
                    while ((cInsertString = fnInsertValue((unsigned char *)cInput, ChunkReference, &usInsertLength, http_session)) != 0) {
                        signed short sLost;
                        if (http_session->ucDynamicFlags & LAST_DYNAMIC_CONTENT_DATA) { // {31} if the user signals that this is the last chunk of binary data
                          //usInsertLength -= (WEB_ESCAPE_LEN + 1);      // {44} avoid any padding at the end of data
                            usInsertLength -= (WEB_ESCAPE_LEN);          // {54} avoid any padding at the end of data
                        }
                        else {
                            uMemset((unsigned char *)(cInsertString + usInsertLength), ' ', WEB_ESCAPE_LEN); // pad
                        }
                        sLost = (signed short)fnInsertHTMLString((CHAR *)cInsertString, (unsigned short)(usInsertLength + WEB_ESCAPE_LEN), &ptrBuffer, usMaxLen, &usLen, ptrBufferEnd);
                        if (http_session->ucDynamicFlags & NO_DYNAMIC_CONTENT_TO_ADD) {
                            http_session->ucDynamicFlags &= GENERATING_DYNAMIC_BINARY;
                            return usToSend;
                        }
                        if (ptrBuffer != 0) {                            // file length reduction
                            usToSend -= sLost;
        #if defined DYNAMIC_DATA_FIXED_LENGTH                            // {33}
                            if (usToSend > (HTTP_BUFFER_LENGTH_ - DYNAMIC_DATA_FIXED_LENGTH)) { // special case where frame buffer has been completely filled - so  quit
            #if defined HTTP_WINDOWING_BUFFERS
                                http_session->DynamicCnt[http_session->ucPresentFrame + 1] = ChunkReference; // mark the continuation point next time round
            #else
                                http_session->DynamicCnt[1] = ChunkReference; // mark the continuation point next time round
            #endif
                                http_session->ucDynamicFlags |= QUIT_FRAME_DURING_GENERATION; // ensure no more attempts are made to increase the content
                                http_session->usUnacked -= WEB_ESCAPE_LEN;
                                return (usToSend - WEB_ESCAPE_LEN);
                            }
        #endif
                        }
                        else {
        #if defined HTTP_WINDOWING_BUFFERS
                            http_session->DynamicCnt[http_session->ucPresentFrame + 1] = (ChunkReference - 1); // mark the continuation point next time round
        #else
                            http_session->DynamicCnt[1] = (ChunkReference - 1); // mark the continuation point next time round
        #endif
                            http_session->ucDynamicFlags |= QUIT_FRAME_DURING_GENERATION; // ensure no more attempts are made to increase the content
                            return (usToSend - sLost);                   // parameter list cut off, handle it next time around
                        }
                        ChunkReference++;
                    }
                    http_session->ucDynamicFlags |= MAXIMUM_DYNAMIC_INSERTS; // {60} insertion has taken place and further must be blocked from taking place a second time in this TCP frame
					http_session->DynamicCnt[HTTP_WINDOWING_BUFFERS] = 0;// since all content is now underway remove the follow-on value
                    ptrBuffer -= (WEB_ESCAPE_LEN - 2);
                    usLen += (WEB_ESCAPE_LEN - 1);
                }
                break;
    #endif
            }
            ptrBuffer += (WEB_ESCAPE_LEN - 2);                           // jump over parameters {9}
            usLen -= (WEB_ESCAPE_LEN - 1);                               // {9}
            if (cInsertString) {
                signed short sLost;                                      // {17} accept positive and negative values
                if ((sLost = (signed short)fnInsertHTMLString((CHAR *)cInsertString, usInsertLength, &ptrBuffer, usMaxLen, &usLen, ptrBufferEnd)) != 0) {  // {14} 
                    if (ptrBuffer != 0) {                                // file length reduction
                        usToSend -= sLost;
                    }
                    else {
                        return (usToSend - sLost);                       // parameter list cut off, handle it next time around
                    }
                }
            }
        }
        usLen--;
    }
    return usToSend;                                                     // length of data frame to send
#else
    return http_session->usUnacked;                                      // raw file content not changed
#endif
}

// Cover function which ensures that full TCP frame buffer is utilised {12}
//
static unsigned short fnWebParGen(unsigned char ptrBuffer[], HTTP *http_session, unsigned short usFrameLength, unsigned short usTx_window)
{
    unsigned short usAdditionalFrameLength = 0;
    unsigned short usContentShrink;

    if (usFrameLength == 0) {                                            // not a follow on frame
        usContentShrink = usFrameLength = http_session->usUnacked;
#if defined HTTP_WINDOWING_BUFFERS
        http_session->usUnackedPreviousContent = usFrameLength;
    #if defined HTTP_DYNAMIC_CONTENT
        http_session->ucPresentFrame = 0;
    #endif
#endif
#if defined _HW_AVR32 && defined _GNU                                    // workaround for AVR32 GCC compiler bug (usUnacked is otherwise not set to zero)
        while (http_session->usUnacked != 0) {                           // {48} this forces the compiler to keep the code
            http_session->usUnacked -= usFrameLength;                    // no content acknowledged yet
        }
#else
        http_session->usUnacked = 0;                                     // no content acknowledged yet
#endif
    }
    else {                                                               // a follow on frame
        usContentShrink = usFrameLength;
#if defined HTTP_DYNAMIC_CONTENT && defined HTTP_WINDOWING_BUFFERS
        http_session->ucPresentFrame++;
#endif
    }
#if defined HTTP_DYNAMIC_CONTENT                                         // {61}
    http_session->ucDynamicFlags &= ~(MAXIMUM_DYNAMIC_INSERTS | QUIT_FRAME_DURING_GENERATION); // {60}
#endif
    FOREVER_LOOP() {
        unsigned short usUnacked = (http_session->usUnacked + usFrameLength); // backup the original total content length
        unsigned short usOriginalLength = usFrameLength;                 // backup the original frame length
        unsigned short usThisLength;                                     // {18} allow loop quit when buffer cannot grow any more 
        usFrameLength -= usContentShrink;
        http_session->usUnacked = usContentShrink;                       // for compatibility with fnWebParGenerator (the length to be processed) - it will be changed accoring to inserts made
        usFrameLength += (usThisLength = fnWebParGenerator(&ptrBuffer[usFrameLength], http_session, &ptrBuffer[usTx_window])); // parse the raw data contents {13}
        http_session->usUnacked += usAdditionalFrameLength;
        usContentShrink = usOriginalLength - http_session->usUnacked;
        http_session->usUnacked = usUnacked - usContentShrink;           // correct for new total open content
    #if defined HTTP_UTFAT                                               // {47}
        if ((usContentShrink != 0) && (ptr_utDirectory->usDirectoryFlags & UTDIR_VALID) && (http_session->utFile.ulFileSize != 0)) {
            utSeek(&http_session->utFile, -usContentShrink, UTFAT_SEEK_CUR);
        }
    #endif
        if ((usContentShrink == 0) || (usThisLength == 0) || ((usFrameLength + usContentShrink) >= usTx_window)
    #if defined HTTP_DYNAMIC_CONTENT
            || (http_session->ucDynamicFlags & QUIT_FRAME_DURING_GENERATION) // when a frame buffer cannot accept more dynamic chunks quit immediately
    #endif
            ) {                                                          // frame buffer completely full (or shrunk content will have no room)
    #if defined HTTP_DYNAMIC_CONTENT
            http_session->ucDynamicFlags &= GENERATING_DYNAMIC_BINARY;
    #endif
    #if defined HTTP_WINDOWING_BUFFERS
            http_session->usUnackedPreviousContent -= usContentShrink;   // correct for new content payload
    #endif
            break;
        }
    #if defined HTTP_UTFAT                                               // {47}
        if ((ptr_utDirectory->usDirectoryFlags & UTDIR_VALID) && (http_session->utFile.ulFileSize != 0)) {
            utReadFile(&http_session->utFile, &HTTP_Tx->ucTCP_Message[usFrameLength], usContentShrink); // read from the file on the SD card
        }
        #if defined ACTIVE_FILE_SYSTEM
        else {
            fnGetParsFile((http_session->ptrFileStart + http_session->FilePoint + http_session->usUnacked), &HTTP_Tx->ucTCP_Message[usFrameLength], usContentShrink); // when not working with SD-card use standard uFileSystem call
        }
        #endif
    #else
        fnGetParsFile((http_session->ptrFileStart + http_session->FilePoint + http_session->usUnacked), &HTTP_Tx->ucTCP_Message[usFrameLength], usContentShrink); // retrieve additional data to send
    #endif
        http_session->usUnacked -= usFrameLength;
        usAdditionalFrameLength = usFrameLength;                         // total additional length
        usFrameLength += usContentShrink;                                // new total frame length
    }
    return usFrameLength;
}


#if defined HTTP_AUTHENTICATION
static int fnAuthenticate(HTTP *http_session, unsigned char *ptrFile, unsigned short usDataLength)
{
    static const unsigned char ucAuthText[] = {'A', 'u', 't', 'h', 'o', 'r', 'i', 'z', 'a', 't', 'i', 'o', 'n', ':', ' ', 'B', 'a', 's', 'i', 'c', ' ' };
    unsigned char ucState = http_session->ucState;                       // our present synchronisation/authentication state
    unsigned char ucAuth  = http_session->ucAuth;                        // the present number of bytes collected

    if (!(ucWebServerMode & WEB_AUTHENTICATE)) {
        return APP_ACCEPT;                                               // if authentication is disabled, accept
    }

    if (ucState < HTTP_STATE_PROCESSING) {
        ucState = HTTP_STATE_PROCESSING;
    }

     while (usDataLength--) {
         switch (ucState) {
         case HTTP_STATE_PROCESSING:
             if (*ptrFile == '\r') {                                     // initially we are searching for '\r' '\n'
                 ucState = (HTTP_STATE_PROCESSING + 1);                  // is it followed by '\n' ?
             }
             break;

         case (HTTP_STATE_PROCESSING + 1):
             if (*ptrFile == '\n') {
                 ucState = (HTTP_STATE_PROCESSING +2 );                  // '\r' '\n' has been found
             }
             else {
                 ucState = HTTP_STATE_PROCESSING;                        // not '\n' so start again
             }
             break;

         case (HTTP_STATE_PROCESSING + 2):
             if (*ptrFile == '\r') {                                     // another '\r' following '\r' '\n'
                 ucState = (HTTP_STATE_PROCESSING + 3);
             }
             else {
                 if (*ptrFile == ucAuthText[ucAuth]) {                   // check authentication header
                     if (++ucAuth < (sizeof(ucAuthText))) {
                         break;                                          // we are presently matching the authentication text we are searching for - not yet complete
                     }
                     ucState = (HTTP_STATE_PROCESSING + 4);              // complete match found, we can now handle the user name and password
                 }
                 else {
                     ucState = HTTP_STATE_PROCESSING;                    // header not found so start again
                 }
                 ucAuth = 0;                                             // no characters collected
             }
             break;

         case (HTTP_STATE_PROCESSING + 3):
             if (*ptrFile == '\n') {                                     // '\r' '\n' + '\r' '\n' sequence means no authentication info in frame - reject
                 http_session->ucState = HTTP_STATE_ACTIVE;              // reset state
                 return CREDENTIALS_REQUIRED;                            // inform that credentials must be entered
             }
             else {
                 ucState = HTTP_STATE_PROCESSING;                        // invalid sequence, start again
             }
             break;

         case (HTTP_STATE_PROCESSING + 4):
             if (*ptrFile == '\r') {                                     // {56} collect user name and password until end reached
                 http_session->cUserPass[ucAuth] = '\r';                 // {57} fnDecode64() requires the terminator to be present
                 fnDecode64(http_session->cUserPass, http_session->cUserPass); // decode the user and password content
                 return (fnVerifyUser(http_session->cUserPass, (DO_CHECK_USER_NAME | DO_CHECK_PASSWORD | HTML_PASS_CHECK)));
             }
             else if (ucAuth >= USER_PASS_LENGTH) {                      // check that maximum length has not been reached
                return CREDENTIALS_REQUIRED;                             // too long so cannot be correct
             }
             http_session->cUserPass[ucAuth++] = *ptrFile;               // {56}
             break;

         default:
             break;
         }
         ptrFile++;
     }
     http_session->ucAuth  = ucAuth;                                    // update session info since we have not completed
     http_session->ucState = ucState;
     return PROCESSING_INPUT;                                           // inform that we havn't completed and wait for part in next frame
}
#endif

// The web parameter handler is optional. Note that the handler can decide which web page is displayed by modifying the file pointer to the desired file name
//
static int fnWebParHandler(unsigned char *ptrptrFile, unsigned short usDataLength, HTTP *http_session)
{
#if defined WEB_PARAMETER_HANDLING
    int iReturn;                                                         // {26} new return value
    #if defined HTTP_AUTHENTICATION                                      // {51}
    unsigned char ucFile = *ptrptrFile;                                  // this is valid if not already processing a block
    #endif
    unsigned char *ptrFile = ptrptrFile;

    #if defined HTTP_AUTHENTICATION
    if (http_session->ucState >= HTTP_STATE_PROCESSING) {
        ucFile = ' ';                                                    // we do authentication only on first contact...
    }
        #if defined VARIABLE_PAGE_AUTHENTICATION                         // protect only certain pages, defined by the application
    if (fnHandleWeb(CHECK_AUTHENTICATION_ON_PAGE, (CHAR *)ptrptrFile, http_session) != APP_ACCEPT) { // check whether this page is protected
        if ((iReturn = fnAuthenticate(http_session, ptrFile, usDataLength)) != APP_ACCEPT) { // {26} only return when rejected
            return iReturn;
        }
    }
        #else                                                            // default if to authenticate only on first contact
    if (ucFile == ' ') {                                                 // is it the initial GET
        if ((iReturn = fnAuthenticate(http_session, ptrFile, usDataLength)) != APP_ACCEPT) { // {26} only return when rejected
            return iReturn;
        }
    }
        #endif
    #endif

    #if defined SUPPORT_WEBSOCKET                                        // {78}
    if ((iReturn = fnWebSocketRx(http_session, ptrFile, usDataLength)) < 0) { // allow the web socket protocol to handle the reception
        return iReturn;                                                  // web socket content handled
    }
    #endif

    if ((ucWebServerMode & WEB_SUPPORT_HANDLER) == 0) {                  // if we are not configured to handle web parameters just accept the page demanded
        return APP_ACCEPT; 
    }

    // Warning - we expect all parameters to be in one TCP frame. This should always be true since frames are never so short. However, if a frame were to be cut, we may fail drastically...so be careful...
    //
    if ((iReturn = fnHandleWeb(GET_STARTING, (CHAR *)/*&ucFile*/ptrFile, http_session)) != APP_ACCEPT) { // inform that a GET is starting {26}{51}
        return iReturn;                                                  // if the initial connection is not accepted
    }
    while (*(++ptrFile) != ' ') {
        if ((--usDataLength) == 0) {
            break;                                                       // end of data - terminate
        }

        if ((*ptrFile == '&') || (*ptrFile == '?')) {                    // search for parameters
            unsigned char ucType = *(++ptrFile);
            if ((iReturn = fnHandleWeb(ucType, (CHAR *)++ptrFile, http_session)) != APP_ACCEPT) { // allow the user to handle this frame and return a special page if desired
                return iReturn;                                          // application is not accepting but rather modifying something
            }
        }
    }
#endif
    return APP_ACCEPT;                                                   // APP_ACCEPT (zero) means show requested page
}

#if defined SUPPORT_WEBSOCKET                                            // {78}


#define SHA1_SIZE 20

#if _MSC_VER
# define _sha1_restrict __restrict
#else
# define _sha1_restrict __restrict__
#endif

static /*inline*/ void sha1mix(unsigned *_sha1_restrict r, unsigned *_sha1_restrict w) {
	unsigned a = r[0];
	unsigned b = r[1];
	unsigned c = r[2];
	unsigned d = r[3];
	unsigned e = r[4];
	unsigned t, i = 0;

#define rol(x,s) ((x) << (s) | (unsigned) (x) >> (32 - (s)))
#define mix(f,v) do { \
		t = (f) + (v) + rol(a, 5) + e + w[i & 0xf]; \
		e = d; \
		d = c; \
		c = rol(b, 30); \
		b = a; \
		a = t; \
	} while (0)

	for (; i < 16; ++i)
		mix(d ^ (b & (c ^ d)), 0x5a827999);

	for (; i < 20; ++i) {
		w[i & 0xf] = rol(w[i + 13 & 0xf] ^ w[i + 8 & 0xf] ^ w[i + 2 & 0xf] ^ w[i & 0xf], 1);
		mix(d ^ (b & (c ^ d)), 0x5a827999);
	}

	for (; i < 40; ++i) {
		w[i & 0xf] = rol(w[i + 13 & 0xf] ^ w[i + 8 & 0xf] ^ w[i + 2 & 0xf] ^ w[i & 0xf], 1);
		mix(b ^ c ^ d, 0x6ed9eba1);
	}

	for (; i < 60; ++i) {
		w[i & 0xf] = rol(w[i + 13 & 0xf] ^ w[i + 8 & 0xf] ^ w[i + 2 & 0xf] ^ w[i & 0xf], 1);
		mix((b & c) | (d & (b | c)), 0x8f1bbcdc);
	}

	for (; i < 80; ++i) {
		w[i & 0xf] = rol(w[i + 13 & 0xf] ^ w[i + 8 & 0xf] ^ w[i + 2 & 0xf] ^ w[i & 0xf], 1);
		mix(b ^ c ^ d, 0xca62c1d6);
	}

#undef mix
#undef rol

	r[0] += a;
	r[1] += b;
	r[2] += c;
	r[3] += d;
	r[4] += e;
}

static void sha1(unsigned char h[SHA1_SIZE], const void *_sha1_restrict p, size_t n) {
	size_t i = 0;
	unsigned w[16], r[5] = {0x67452301, 0xefcdab89, 0x98badcfe, 0x10325476, 0xc3d2e1f0};

	for (; i < (n & ~0x3f);) {
		do w[i >> 2 & 0xf] =
			((const unsigned char *) p)[i + 3] << 0x00 |
			((const unsigned char *) p)[i + 2] << 0x08 |
			((const unsigned char *) p)[i + 1] << 0x10 |
			((const unsigned char *) p)[i + 0] << 0x18;
		while ((i += 4) & 0x3f);
		sha1mix(r, w);
	}

	uMemset(w, 0, sizeof w);

	for (; i < n; ++i)
		w[i >> 2 & 0xf] |= ((const unsigned char *) p)[i] << ((3 ^ i & 3) << 3);

	w[i >> 2 & 0xf] |= 0x80 << ((3 ^ i & 3) << 3);

	if ((n & 0x3f) > 56) {
		sha1mix(r, w);
		uMemset(w, 0, sizeof w);
	}

	w[15] = n << 3;
	sha1mix(r, w);

	for (i = 0; i < 5; ++i)
		h[(i << 2) + 0] = (unsigned char) (r[i] >> 0x18),
		h[(i << 2) + 1] = (unsigned char) (r[i] >> 0x10),
		h[(i << 2) + 2] = (unsigned char) (r[i] >> 0x08),
		h[(i << 2) + 3] = (unsigned char) (r[i] >> 0x00);
}


static int fnWebSocketRx(HTTP *http_session, unsigned char *ptrData, unsigned short usDataLength)
{
    static CHAR cWebSocketTest1[] = "HTTP/1.1 101 Switching Protocols\r\nUpgrade: websocket\r\nConnection: Upgrade\r\nSec-WebSocket-Accept: "; // this needs to be constructed and remain stable for each web socket until the connection has been confirmed
  //static CHAR cWebSocketTest2[] = "\r\nSec-WebSocket-Protocol: chat\r\n\r\n";                                   // this needs to be constructed and remain stable for each web socket until the connection has been confirmed
    static CHAR cWebSocketTest2[] = "\r\nSec-WebSocket-Protocol: ";                                   // this needs to be constructed and remain stable for each web socket until the connection has been confirmed
    static CHAR cWebSocketTest3[] = "\r\n\r\n";
    static CHAR cWebResponseTest[512];
    static MAX_FILE_LENGTH responseLength;
    static CHAR subProtocol[64];
    int iReturn = 0;
    unsigned char *ptrStart = ptrData;
    if (http_session->ucState < HTTP_STATE_WEB_SOCKET_CONNECTION) {      // depending on the connection state
        while (usDataLength-- >= 4) {                                    // limit to the received frame length
            if (uMemcmp("\r\n\r\n", ptrData, 4) == 0) {                  // locate the end of the header
                while (ptrStart < ptrData) {                             // scan the header for web socket details
                    if (uMemcmp(ptrStart, "Sec-WebSocket-", 14) == 0) {
                        ptrStart += 14;
                        if (uMemcmp(ptrStart, "Version:", 8) == 0) {
                            ptrStart += 8;
                        }
                        else if (uMemcmp(ptrStart, "Protocol:", 9) == 0) {
                            int iSubProtLen = 0;
                            ptrStart += 10;                              // jump the protocol and a single space                
                            while (*ptrStart != '\r') {
                                subProtocol[iSubProtLen++] = *ptrStart++;
                            }
                            subProtocol[iSubProtLen] = 0;
                        }
                        else if (uMemcmp(ptrStart, "Extensions:", 11) == 0) {
                            ptrStart += 11;
                        }
                        else if (uMemcmp(ptrStart, "Key:", 4) == 0) {
#if defined _WINDOWS_
                            CHAR test[] = "dGhlIHNhbXBsZSBub25jZQ== "; // RFC 6455 reference input
#endif
                            static const CHAR cGUID[] = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11"; // globally unique identfier in string form
                            int iStrLen = 0;
                            unsigned char shaHash[20];
                            CHAR tempKey[64];
                            CHAR *ptrBuf;
                            ptrStart += 5;                               // jump the key string and initial space
#if defined _WINDOWS_
                            ptrStart = test;
#endif
                            while ((*ptrStart != '\r') && (*ptrStart != ' ')) {
                                tempKey[iStrLen++] = *ptrStart++;        // copy the key value (base64 coded), ignoring any trailing space
                            }
                            uMemcpy(&tempKey[iStrLen], cGUID, sizeof(cGUID)); // concatenate the decoded string with the GUID
                            // - a SHA-1 hash of this string is returned (encoded as base64) in the server's handshake
                            uMemset(shaHash, 0, sizeof(shaHash));
                            sha1(shaHash, tempKey, (iStrLen + sizeof(cGUID) - 1));
                            // expected value 0xb3 0x7a 0x4f 0x2c 0xc0 0x62 0x4f 0x16 0x90 0xf6 0x46 0x06 0xcf 0x38 0x59 0x45 0xb2 0xbe 0xc4 0xea
                            iStrLen = (int)fnEncode64(shaHash, tempKey, sizeof(shaHash));  // encode the hash
                            tempKey[iStrLen] = 0;                        // terminate string
                            // expected value s3pPLMBiTxaQ9kYGzzhZRbK+xOo=
    ptrBuf = uStrcpy(cWebResponseTest, cWebSocketTest1);
    ptrBuf = uStrcpy(ptrBuf, tempKey);
    ptrBuf = uStrcpy(ptrBuf, cWebSocketTest2);
    ptrBuf = uStrcpy(ptrBuf, subProtocol);
    ptrBuf = uStrcpy(ptrBuf, cWebSocketTest3);
    responseLength = (ptrBuf - cWebResponseTest);
                        }
                    }
                    else if (uMemcmp(ptrStart, "Upgrade: websocket", 18) == 0) {
                        ptrStart += 18;
                        http_session->ucState = HTTP_STATE_WEB_SOCKET_CONNECTION; // connected state
                        iReturn = WEB_SOCKET_HANDSHAKE;                  // this is a web socket connection handshake which is to be accepted in case of no other errors
                    }
                    ptrStart++;
                }
            }
            ptrData++;
        }
        if (iReturn == WEB_SOCKET_HANDSHAKE) {
            http_session->ptrFileStart = (unsigned char*)cWebResponseTest;
            http_session->FileLength   = responseLength;
            #if defined SUPPORT_MIME_IDENTIFIER
            http_session->ucMimeType   = MIME_HTML;                      // force HTML type
            #endif
        }
    }
    else {                                                               // when connected handle the content as messages
    }
    return iReturn;
}
#endif

#endif                                                                   // end USE_HTTP
