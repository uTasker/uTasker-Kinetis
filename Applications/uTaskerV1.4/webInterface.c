/**********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher

    ---------------------------------------------------------------------
    File:      webInterface.c
    Project:   uTasker project
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2018
    *********************************************************************
    26.02.2007 Added SMTP parameter support
    17.03.2007 Corrected SMTP login flag so that it is not cleared when Ethernet settings are changed {1}
    17.03.2007 Decoded received SMTP string settings to support Firefox  {2} 
    03.03.2007 Add configurable timeout when configuring FTP server      {3}
    10.12.2007 Avoid displaying local password                           {4}
    08.01.2008 Add dynamic insertion demo                                {5}
    12.01.2008 Adjust fnInsertValue() to accept session information      {6}
    17.01.2008 Ensure post defined file is returned when not SW upload   {7}
    13.01.2008 fnInsertString TxLength type change (LENGTH_CHUNK_COUNT) for dynamic content generation {8}
    22.03.2008 Correct length of MAC address string when displaying ARP entries {9}
    11.04.2008 Add missing break                                         {10}
    01.05.2008 Add blacklist to reject some HTTP connections             {11}
    04.05.2008 Allow simplified project without USE_MAINTENANCE          {12}
    29.06.2008 Change download FLASH size from 160k to 156k (due to new organisation of M5223X FLASH where parameter system not counted) {13}
    16.07.2008 Correct length of MAC address string when displaying MAC address {14}
    07.09.2008 Add demo reception of POST parameter data                 {15}
    01.11.2008 Conditional call to enable project without parameters     {16}
    07.11.2008 cOnColor[] conditional on USE_MAINTENANCE                 {17}
    30.01.2009 Add display of reset cause for certain processors         {18}
    30.06.2009 Add GLCD display support in browser                       {19}
    01.07.2009 Adapt for compatibility with STRING_OPTIMISATION (fnDebugDec() replaced by fnBufferDec()), pointers used from uStrcpy no longer decremented
    06.08.2009 Add OLED graphical demo                                   {20}
    20.11.2009 Increase size of ucTable to include WEB_ESCAPE_LEN space for padding by HTTP dynamic content inserter {21}
    16.01.2010 Remove OLED_X/Y and use only GLCD_X/Y                     {22}
    05.02.2010 Add IPV6 interface                                        {23}
    05.02.2010 temp_pars->temp_parameters.ucServers changed to .usServers
    05.02.2010 Return value from fnGetARPentry() checked against 0 rather than against cucNullMACIP since the return value changed {24}
    28.02.2010 Control LCD web interface with define LCD_WEB_INTERFACE   {25}
    09.09.2010 Ajax demonstration display TICK rather than a local counter {26}
    16.04.2011 Add variable HTTP port number                             {27}
    29.08.2011 Add display of connection IPv4 address                    {28}
    20.09.2011 Added display of web server connection counter            {29}
    20.09.2011 Optionally stop display demo operation when posting to TFT{30}
    19.10.2011 Add reset cause to kinetis                                {31}
    19.10.2011 Add display of time from RTC                              {32}
    27.10.2011 Add reset cause to SAM7X                                  {33}
    17.11.2011 Add reset cause to LPC17XX                                {34}
    18.12.2011 Add STM32 reset cause display                             {35}
    08.01.2012 Add AVR32 reset cause display                             {36}
    27.02.2012 Modify some consts to avoid memcpy() use by GNU compiler  {37}
    12.03.2012 Change display of IP address to support IPv4 and IPv6     {38}
    26.03.2012 Handle display and modification of global IPv6 address    {39}
    27.05.2012 Adapt use of fnGetARPentry() and optionally display interface that an ARP entry belongs to {40}
    27.05.2012 Adapt network parameters for compatibility with multiple networks
    07.10.2012 Display unknown fnInsertString() parameters as dash rather than leave as escape sequence {41}
    07.10.2012 Allow SPECIAL_LCD_DEMO to be used together with TFT_GLCD_MODE {42}
    12.08.2013 Add ST7565S_GLCD_MODE                                     {43}
    06.11.2015 Adjust fnStackFree() parameters                           {44}
    11.02 2016 Parameters for fnStartHTTP() modified                     {45}

*/


/* =================================================================== */
/*                           include files                             */
/* =================================================================== */

#include "config.h"

/* =================================================================== */
/*                             constants                               */
/* =================================================================== */

#if defined USE_MAINTENANCE || defined USE_HTTP
    const CHAR  cSoftwareVersion[] = SOFTWARE_VERSION;                   // software version for general purpose display use
#endif

#if defined USE_HTTP

extern TCP_CONTROL *present_tcp;                                         // {28}

/* =================================================================== */
/*                      local structure definitions                    */
/* =================================================================== */



/* =================================================================== */
/*                          local definitions                          */
/* =================================================================== */

//#define REFRESH_DEMO                                                   // activate this for a demo of a delayed refresh

#if (defined SPECIAL_LCD_DEMO || defined SUPPORT_GLCD || defined SUPPORT_OLED || defined MB785_GLCD_MODE) // {25}
    #define LCD_WEB_INTERFACE
#endif

#define W_BMP_HEADER_LENGTH  0x36
#define W_BMP_CONTENT_SIZE   (DISPLAY_WIDTH * DISPLAY_HEIGHT)
#define W_BMP_TOTAL_SIZE     (W_BMP_CONTENT_SIZE + W_BMP_HEADER_LENGTH)
#define DISPLAY_WIDTH        GLCD_X
#define DISPLAY_HEIGHT       GLCD_Y


/* =================================================================== */
/*                 local function prototype declarations               */
/* =================================================================== */

/* =================================================================== */
/*                     global variable definitions                     */
/* =================================================================== */

/* =================================================================== */
/*                             constants                               */
/* =================================================================== */

#if defined LCD_WEB_INTERFACE                                            // {37}
    static const W_BITMAPHEADER bm_header = {{0x42, 0x4d}, {(unsigned char)W_BMP_TOTAL_SIZE, (unsigned char)(W_BMP_TOTAL_SIZE >> 8), (unsigned char)(W_BMP_TOTAL_SIZE >> 16), (unsigned char)(W_BMP_TOTAL_SIZE >> 24)}, {0,0,0,0}, {W_BMP_HEADER_LENGTH, 0x00, 0x00, 0x00}};
    static const W_BITMAPINFO   bm_info = {{0x28, 0x00, 0x00, 0x00}, {(unsigned char)DISPLAY_WIDTH, (unsigned char)(DISPLAY_WIDTH >> 8), 0x00, 0x00}, {(unsigned char)DISPLAY_HEIGHT, (unsigned char)(DISPLAY_HEIGHT >> 8), 0x00, 0x00}, {0x01, 0x00}, {0x18, 0x00}, {0,0,0,0}, {(unsigned char)W_BMP_CONTENT_SIZE, (unsigned char)(W_BMP_CONTENT_SIZE >> 8), (unsigned char)(W_BMP_CONTENT_SIZE >> 16), (unsigned char)(W_BMP_CONTENT_SIZE >> 24)}, {0x61, 0x0f, 0x00, 0x00}, {0x61, 0x0f, 0x00, 0x00}, {0,0,0,0}, {0,0,0,0}};  
#endif

/* =================================================================== */
/*                      local variable definitions                     */
/* =================================================================== */

#if defined HTTP_DYNAMIC_CONTENT                                         // {5}
typedef struct stMULTIPLICATION_TABLE                                    // structure used by demo
{
    unsigned short usRows;
    unsigned char  ucColumns;
} MULTIPLICATION_TABLE;
#endif

static unsigned char  fnIsSelected(unsigned char *ptrBuffer);
#if defined HTTP_DYNAMIC_CONTENT                                         // {6}{8}
    static CHAR      *fnInsertString(unsigned char *ptrBuffer, LENGTH_CHUNK_COUNT TxLength, unsigned short *usLengthToSend, HTTP *http_session);
#else
    static CHAR      *fnInsertString(unsigned char *ptrBuffer, LENGTH_CHUNK_COUNT TxLength, unsigned short *usLengthToSend);
#endif
static int            fnHandleWeb(unsigned char ucType, CHAR *ptrData, HTTP *http_session);
#if defined SUPPORT_WEBSOCKET
    static int        fnWebSocketHandler(unsigned char *ptrPayload, unsigned long ulPayloadLength, unsigned char ucOpCode, HTTP *http_session);
#endif

static unsigned short fnGetSerialMode(unsigned char ucMode, unsigned char *ucForceCheck);
static unsigned short fnGetNetworkMode(unsigned char ucMode, unsigned char *ucForceCheck);
static unsigned short fnGetServerType(unsigned char ucType);
static int            fnModified(CHAR cPar);
static void           fnDelayResetBoard(void);

    #if defined USE_SMTP
    static int fnCheckMailAddress(void);
    static unsigned short usSentEmails = 0;
    #endif

static const CHAR cBackgroundColor[] = "background-color:";
static const CHAR cOffColor[] = "gray";
#if defined USE_MAINTENANCE                                              // {17}
    static const CHAR cOnColor[]  = "red ";                              // note length made same with space...
#endif

    #if defined REFRESH_DEMO
    static const CHAR cRefresh[] = "Refresh";
    #endif

    #if defined SUPPORT_HTTP_POST && !defined _HW_NE64
        #if FILE_SYSTEM_SIZE >= (156 * 1024) || defined MB785_GLCD_MODE  // {13} support posting new firmware only with large file system
        static const CHAR cWarningHTML[]       = UPLOAD_FAILED;
        static const CHAR cSuccessSW_HTML[]    = SW_UPLOAD_COMPLETED;
        #endif
    #endif

extern void fnConfigureAndStartWebServer(void)
{
    if ((ACTIVE_WEB_SERVER & temp_pars->temp_parameters.usServers[DEFAULT_NETWORK]) != 0) {
        HTTP_FUNCTION_SET FunctionSet;                                   // {45}
        FunctionSet.ucParameters = (WEB_SUPPORT_PARAM_GEN | WEB_SUPPORT_HANDLER);
        if ((AUTHENTICATE_WEB_SERVER & temp_pars->temp_parameters.usServers[DEFAULT_NETWORK]) != 0) {
            FunctionSet.ucParameters |= WEB_AUTHENTICATE;                // activate authentication
        }
        if (uMemcmp(temp_pars->temp_parameters.ucTrustedIP, cucNullMACIP, IPV4_LENGTH) != 0) {
            FunctionSet.ucParameters |= WEB_TRUSTED_IP;                  // activate trusted IP address checking
        }
        FunctionSet.fnWebHandler = fnHandleWeb;
        FunctionSet.fnInsertRoutine = fnInsertString;
        FunctionSet.fnGenerator = fnIsSelected;
    #if defined SUPPORT_WEBSOCKET
        FunctionSet.fnWebSocketHandler = fnWebSocketHandler;
    #endif
    #if defined _VARIABLE_HTTP_PORT
        FunctionSet.usPort = HTTP_SERVERPORT;                            // {27} pass the port number to be used, which can be variable of required
    #endif
        fnStartHTTP(&FunctionSet);                                       // {45}
    }
    else {
        fnStopHTTP();
    }
}

static int fnOnMyBlackList(unsigned char ip_add[IPV4_LENGTH])
{
    static const unsigned char ucBlacklisted[] = {212, 224, 127, 14};
    if (!uMemcmp(ip_add, ucBlacklisted, IPV4_LENGTH)) {
        return 1;
    }
    return 0;
}

// We can define whether a selection is set or not when a web page is displayed
//
static unsigned char fnIsSelected(unsigned char *ptrBuffer)
{
    // Format £sAB (length assumed to be 4 bytes, even when both parameters are not used)
    //
    unsigned char ucCheck = NOT_SELECTED;

    switch (*ptrBuffer++) {
    case 'a':                                                            // special case - do we accept this address?
        if ((!(uMemcmp(temp_pars->temp_parameters.ucTrustedIP, cucNullMACIP, IPV4_LENGTH))) || (!(uMemcmp(temp_pars->temp_parameters.ucTrustedIP, ptrBuffer, IPV4_LENGTH)))) {
            return IS_SELECTED;                                          //
        }
        break;                                                           // {10}

    case 'b':                                                            // {11}
        if (fnOnMyBlackList(ptrBuffer) != 0) {
            return IS_SELECTED;
        }
        break;

    case 'd':
        if (fnIsSelected (ptrBuffer) != NOT_SELECTED) {
            return IS_DISABLED;                                          // we should disable the entry field
        }
        break;

    case 'D':                                                            // disabled switches                                                      // dial-out - if we want, we can disable dialout entry
        if (fnIsSelected (ptrBuffer) == NOT_SELECTED) {
            return IS_DISABLED;                                          // we should disable the entry field
        }
        break;

    case 'B':                                                            // select appropriate UART Baud rate
        if ((*ptrBuffer - 'A') == temp_pars->temp_parameters.ucSerialSpeed) { // the baud rates are defined from A (slowest) through B, C, D, E, etc..
            return IS_SELECTED;
        }
        break;

#if !defined _HW_NE64
    #if FILE_SYSTEM_SIZE >= (156 * 1024) || defined MB785_GLCD_MODE      // {13}
    case 'N':
        return IS_SELECTED;                                              // new software upload supported
    #endif
#endif

    case 'U':                                                            // set various UART selections
        if (temp_pars->temp_parameters.SerialMode & fnGetSerialMode(*ptrBuffer, &ucCheck)) { // check transfer mode
            return IS_CHECKED;
        }
        return ucCheck;

    case 'E':                                                            // set various Ethernet settings
        if (temp_pars->temp_network[DEFAULT_NETWORK].usNetworkOptions & fnGetNetworkMode(*ptrBuffer, &ucCheck)) { // check network mode
            return IS_CHECKED;
        }
        return ucCheck;

    case 'S':                                                            // set various Server enabled states
        if ((temp_pars->temp_parameters.usServers[DEFAULT_NETWORK] & fnGetServerType(*ptrBuffer)) != 0) {
            return IS_CHECKED;                                           // check server state
        }
        break;
#if defined USE_MAINTENANCE && !defined REMOVE_PORT_INITIALISATIONS      // {12}
    case 'I':                                                            // port input configuration
        if (fnPortInputConfig(*ptrBuffer) != 0) {
            return IS_CHECKED;
        }
        break;
    case 'O':                                                            // port output configuration
        if ((fnPortInputConfig(*ptrBuffer)) == 0) {
            return IS_CHECKED;
        }
        break;
    case 'P':                                                            // port value
        if (fnPortState(*ptrBuffer) != 0) {
            return IS_CHECKED;
        }
        break;
#endif
    case 'M':                                                            // modified
        if (fnModified(*ptrBuffer) != 0) {
            return IS_CHECKED;
        }
        break;

#if defined USE_SMTP
    case 'm':
        return IS_SELECTED;
#endif
#if defined USE_DNS
    case 'n':
        return IS_SELECTED;
#endif

    default:
        break;

    }
    return NOT_SELECTED;
}

// Here we handle incoming web parameters - we can also change the file to be displayed if we want to
//
static int fnHandleWeb(unsigned char ucType, CHAR *ptrData, HTTP *http_session)
{
#if defined SERIAL_INTERFACE && defined DEMO_UART
    static UART_MODE_CONFIG SerialMode;
#endif
    static unsigned short usServers;
    static unsigned char  usNewNetworkOptions;
#if defined USE_MAINTENANCE                                              // {12}
    static CHAR           cUserName_and_pass[18];
    static CHAR           cNewpass[9];
#endif
    unsigned char        *ucPtr;

    switch (ucType) {
#if defined PLAIN_TEXT_POST                                              // {15}
    case POSTING_PARTIAL_PARAMETER_DATA_TO_APP:
    case POSTING_PARAMETER_DATA_TO_APP:                                  // the buffer contains POST parameter data, whose length is defined by http_session->FileLength
        fnWrite(DebugHandle, (unsigned char *)ptrData, (QUEUE_TRANSFER)http_session->FileLength); // send to debug output
        break;
#endif
#if defined SUPPORT_HTTP_POST && !defined _HW_NE64
    #if FILE_SYSTEM_SIZE >= (156 * 1024) || defined MB785_GLCD_MODE      // {13} support posting new firmware only with large file system
    case CAN_POST_BEGIN:
      //return (1);                                                      // block upload!!
        #if defined LCD_WEB_INTERFACE                                    // {19}{20}{25}
            #if defined GLCD_COLOR
        if (http_session->cDisplayFile == 'B')
            #else
                #if defined _KINETIS                                     // the file name used depends on the file system layout
        if (http_session->cDisplayFile == 'A')
                #else
        if (http_session->cDisplayFile == 'C')
                #endif
            #endif
        {                                                                // GLCD display content upload
            fnDisplayBitmap(0, 0);                                       // reset GLCD upload sequence
        }
        #endif
        return (0);                                                      // allow post at all times

    case INFORM_POST_FAILED:
        http_session->ptrFileStart = (unsigned char*)cWarningHTML;
        http_session->FileLength   = (sizeof(cWarningHTML) - 1);
        #if defined SUPPORT_MIME_IDENTIFIER
        http_session->ucMimeType   = MIME_HTML;                          // force HTML type
        #endif
        return DISPLAY_INTERNAL;                                         // inform that the entered page should be displayed
    #endif

    case INFORM_POST_SUCCESS:
    #if defined SUPPORT_MIME_IDENTIFIER
        http_session->ucMimeType = MIME_HTML;                            // force HTML type
    #endif
    #if FILE_SYSTEM_SIZE >= (156 * 1024) 
        if (http_session->cDisplayFile == 'S') {                         // {13} software upload
            http_session->ptrFileStart = (unsigned char*)cSuccessSW_HTML;
            http_session->FileLength = (sizeof(cSuccessSW_HTML) - 1);
            fnDelayResetBoard();                                         // after 2 seconds the application will reset and then the new software will be copied
            return DISPLAY_INTERNAL;                                     // {7}
        }
    #endif
        break;

    #if defined LCD_WEB_INTERFACE                                        // {19}{20}{25}
    case POSTING_DATA_TO_APP:                                            // a new bit map for the GLCD is being received
        fnDisplayBitmap((unsigned char *)ptrData, (unsigned short)(http_session->FileLength));
        #if defined SPECIAL_LCD_DEMO && (defined CGLCD_GLCD_MODE || defined TFT_GLCD_MODE || defined ST7789S_GLCD_MODE) // {30}{} when a bit map is posted to the display, stop the GLCD compatibility mode demo so that the image remains
        uTaskerStopTimer(TASK_APPLICATION);
        #endif
        break;
    #endif
#endif

    case 0:                                                              // this is called on each new GET - we can reset variables which collect data here if required
        usServers = 0;
#if defined SERIAL_INTERFACE && defined DEMO_UART
        SerialMode = 0;
#endif
        usNewNetworkOptions = 0;
        break;
#if defined USE_MAINTENANCE                                              // {12}
    case 'A':                                                            //  do administration page action (if one)
/*      if (!fnVerifyUser(cUserName_and_pass, DO_CHECK_PASSWORD)) {          no password checking in this project
            fnResetChanges();
            usServers = 0;
            return BAD_PASSWORD_PAGE;
        } */
        switch (*++ptrData) {
        case '2':                                                        // modify and save security settings
            temp_pars->temp_parameters.usServers[DEFAULT_NETWORK] = (usServers | ACTIVE_WEB_SERVER | (temp_pars->temp_parameters.usServers[DEFAULT_NETWORK] & (ACTIVE_DHCP))); // modify server state according to on bits
    #if defined USE_PARAMETER_BLOCK
            if (fnSaveNewPars(SAVE_NEW_PARAMETERS_CHECK_CRITICAL)) {     // save new parameters
                fnSaveNewPars(SAVE_NEW_PARAMETERS_VALIDATE);             // temporarily save new parameters
                fnDelayResetBoard();                                     // reset to test the new parameters
                //return WARN_BEFORE_SAVE;                               // the user wants to change a critical parameter so we first show a side warning of this
            }
    #endif
            fnConfigureAndStartWebServer();
            fnConfigureFtpServer(FTP_TIMEOUT);                           // {3}
    #if defined USE_TELNET
            fnConfigureTelnetServer();
    #endif
            return 0;

        case '1':                                                        // modify and save user name and password
            {
                int i = 0;
                do {                                                     // copy the new user name and password
                    temp_pars->temp_parameters.cUserName[i] = cUserName_and_pass[i];
                } while (cUserName_and_pass[i++] != '&');
                i = 0;
                do {
                    temp_pars->temp_parameters.cUserPass[i] = cNewpass[i];
                } while (cNewpass[i++] != '&');
            }
            break;

    #if defined USE_PARAMETER_BLOCK
        case '3':                                                        // restore factory settings
            fnRestoreFactory();                                          // temporarily restore factory settings                                                   // Modify and save security settings
            if (fnSaveNewPars(SAVE_NEW_PARAMETERS_CHECK_CRITICAL)) {     // save new parameters
                fnSaveNewPars(SAVE_NEW_PARAMETERS_VALIDATE);             // temporarily save new parameters
                fnDelayResetBoard();                                     // reset to test the new parameters
              //return WARN_BEFORE_SAVE;                                 // the user wants to change a critical parameter so we first show a side warning of this
            }
            break;
    #endif

        case '4':                                                        // RESET device
            fnDelayResetBoard();
            break;

        default:
            break;                                                       // do no others
        }
    #if defined USE_PARAMETER_BLOCK
        fnResetChanges();
    #endif
        usServers = 0;
        break;
#endif

    /*********************Server enable/disables*************************/
    case 'S':                                                            // collect active servers
        usServers |= fnGetServerType(*ptrData);                          // set desired server state to active
        break;

#if defined USE_SMTP_AUTHENTICATION
    case 's':                                                            // toggle a server state
        temp_pars->temp_parameters.usServers[DEFAULT_NETWORK] ^= fnGetServerType(*ptrData); // set desired server state to active (immediately update)
        break;
#endif

    /*************************Serial Baud*******************************/
    case 'B':                                                            // set serial speed (B=C) - we jump the =
        temp_pars->temp_parameters.ucSerialSpeed = (*(++ptrData) - 'A'); // set new speed
        break;

    /*************************UART Settings******************************/
#if defined SERIAL_INTERFACE && defined DEMO_UART
    case 'U':                                                            // collect active servers (Ux=y) - we jump the x and =
        ptrData += 2;
        SerialMode |= fnGetSerialMode(*ptrData, 0);                      // set desired transfer mode to active
        break;

    case 'u':                                                            // end of serial mode change
        temp_pars->temp_parameters.SerialMode = SerialMode;              // modify serial mode according to on bits
        fnSetNewSerialMode(MODIFY_CONFIG);                               // affect any changes
        break;
#endif

    /*************************ETHERNET Settings**************************/
    case 'E':                                                            // collect network details (E=y) - we jump the =
        usNewNetworkOptions |= fnGetNetworkMode(*(++ptrData), 0);        // set desired transfer mode to active
        break;

    case 'e':                                                            // command from ethernet page
        switch (*ptrData) {
        case 'm':                                                        // end of network mode change
            if (fnAreWeValidating() == 0) {
                temp_pars->temp_network[DEFAULT_NETWORK].usNetworkOptions &= ~(AUTO_NEGOTIATE | LAN_10M | LAN_100M | FULL_DUPLEX);
                temp_pars->temp_network[DEFAULT_NETWORK].usNetworkOptions |= usNewNetworkOptions; // modify network options according to on bits
                temp_pars->temp_parameters.usServers[DEFAULT_NETWORK] = (usServers | ACTIVE_WEB_SERVER | (temp_pars->temp_parameters.usServers[DEFAULT_NETWORK] & (ACTIVE_FTP_SERVER | ACTIVE_LOGIN | ACTIVE_TELNET_SERVER | SMTP_LOGIN))); // modify server state according to on bits {1}
            }
            else {
#if defined USE_PARAMETER_BLOCK                                          // {16}
                fnDelPar(SWAP_PARAMETER_BLOCK);                          // validate the present parameters
#endif
                fnWeHaveBeenValidated();
            }
            break;
#if defined USE_MAINTENANCE && defined USE_PARAMETER_BLOCK               // {12}
    #if !defined REMOVE_PORT_INITIALISATIONS
        case 'q':                                                        // save the port setup as default
            fnSavePorts();
            break;
    #endif
        case 'r':                                                        // reject changes
            fnResetChanges();
            break;
        case 's':                                                        // save changes temporarily and reset awaiting validation
            if (fnSaveNewPars(SAVE_NEW_PARAMETERS_CHECK_CRITICAL) != 0) {// save new parameters
                fnSaveNewPars(SAVE_NEW_PARAMETERS_VALIDATE);             // temporarily save new parameters
                fnDelayResetBoard();                                     // reset to test the new parameters
//              return WARN_BEFORE_SAVE;                                 // the user wants to change a critical parameter so we first show a side warning of this
            }
            break;
        case 'S':                                                        // save changes temporarily and reset awaiting validation
            fnSaveNewPars(SAVE_NEW_PARAMETERS_VALIDATE);                 // temporarily save new parameters
            fnDelayResetBoard();                                         // reset to test the new parameters
            break;
#endif
        }
        break;
    /********************************************************************/
#if defined USE_IP_STATS
    case 'R':                                                            // reset Ethernet statistics
        fnDeleteEthernetStats(DEFAULT_NETWORK);
        break;
#endif

    case 'D':                                                            // delete the ARP table
#if defined USE_IPV6
        if (*ptrData == 'N') {
            fnDeleteNeighbors();
            break;
        }
#endif
        fnDeleteArp();
        break;


    /********************************************************************/
    case 'I':                                                            // set an IP address
        switch (*ptrData) {
        case 'i':                                                        // our IP address
            ucPtr = &temp_pars->temp_network[DEFAULT_NETWORK].ucOurIP[0];
            break;
        case 's':                                                        // subnet mask
            ucPtr = &temp_pars->temp_network[DEFAULT_NETWORK].ucNetMask[0];
            break;
        case 'g':                                                        // default gateway
            ucPtr = &temp_pars->temp_network[DEFAULT_NETWORK].ucDefGW[0];
            break;
#if defined (SMTP_PARAMETERS) && !defined (USE_DNS)
        case 'S':
            ucPtr = &temp_pars->temp_parameters.ucSMTP_server_ip[0];     // SMTP IP address when not using DNS
            break;
#endif
#if defined USE_IPV6                                                     // {23}
        case '7':                                                        // {39}
            {
                CHAR cIPV6_str[MAX_IPV6_STRING];                         // since the IPV6 delimiters are generally coded we first extract the input string to this buffer
                unsigned char ucTestIPV6[IPV6_LENGTH];                   // extract the address locally in case it has syntax errors
                fnWebStrcpy(cIPV6_str, (ptrData + 2));
                if (fnStrIPV6(cIPV6_str, ucTestIPV6) != 0) {
                    uMemcpy(&temp_pars->temp_network[DEFAULT_NETWORK].ucOurIPV6[0], ucTestIPV6, IPV6_LENGTH); // set the new temporary IPV6 global address
                }
            }
            return 0;
    #if defined USE_IPV6INV4
        case 't':                                                        // IPv6 in IPv4 tunnel address
            ucPtr = &temp_pars->temp_network[DEFAULT_NETWORK].ucTunnelIPV4[0];
            break;
    #endif
#endif
        default:                                                         // ignore non-supported parameters
            return 0;
        }
        ptrData += 2;
        fnStrIP(ptrData, ucPtr);                                         // set the input IP address to the internal location
#if defined (SMTP_PARAMETERS) && !defined (USE_DNS)
        if (*(ptrData - 2) == 'S') {                                     // if we have just updated the SMTP IP address, set it to the working value
            uMemcpy(ucSMTP_server, temp_pars->temp_parameters.ucSMTP_server_ip, IPV4_LENGTH);
        }
#endif
        break;

    /********************************************************************/
#if defined ETH_INTERFACE || defined USB_CDC_RNDIS || defined USE_PPP
    case 'c':                                                            // set MAC (only possible when zero)
        if (uMemcmp(&temp_pars->temp_network[DEFAULT_NETWORK].ucOurMAC[0], cucNullMACIP, MAC_LENGTH) == 0) {
            fnSetMAC((ptrData + 1), &temp_pars->temp_network[DEFAULT_NETWORK].ucOurMAC[0]);
        }
        break;
#endif
#if defined USE_MAINTENANCE                                              // {12}
    case 'N':                                                            // change Device ID/Name
        fnWebStrcpy(temp_pars->temp_parameters.cDeviceIDName, (++ptrData));
    #if defined USE_PARAMETER_BLOCK
        if (fnSaveNewPars(SAVE_NEW_PARAMETERS_CHECK_CRITICAL)) {         // save the new name
            fnSaveNewPars(SAVE_NEW_PARAMETERS_VALIDATE);                 // temporarily save new parameters
            fnDelayResetBoard();                                         // reset to test the new parameters
            //return WARN_BEFORE_SAVE;                                   // the user wants to change a critical parameter so we first show a side warning of this
        }
    #endif
    #if defined SUPPORT_DELAY_WEB_SERVING && defined REFRESH_DEMO        // test code
        uTaskerMonoTimer( TASK_APPLICATION, (DELAY_LIMIT)(2*SEC), E_SERVE_PAGE );
        return DELAY_SERVING;
    #else
        break;
    #endif
#endif
    /********************************************************************/
        case 'n':                                                        // change a port number
            if (*ptrData == 'T') {                                       // Telnet port
                fnSetNewValue(TELNET_PORT, (ptrData+2));
            }
            break;
#if defined USE_MAINTENANCE && !defined REMOVE_PORT_INITIALISATIONS      // {12}
        case 'P':                                                        // port settings (configure as input, output or ADC)
            fnConfigPort(*ptrData, *(ptrData+2));
            break;
        case 'o':                                                        // toggle out bit states
            fnTogglePortOut(*ptrData);
            break;
        case 'O':                                                        // collect output bit states
            usServers |= (1 << (*ptrData - '0'));                        // this bit is to be set
            break;
        case 'p':                                                        // set port output values after collecting bit states
            fnSetPortOut((unsigned char)usServers, 0);                   // set new port output states
            usServers = 0;                                               // reset for next time around
            break;
#endif
        case 'Z':
            fnDelayResetBoard();                                         // reset the board now
            break;

        case 'F':                                                        // flow control
            if (*ptrData == 'H') {                                       // set new high level
                fnSetNewValue(FLOW_CONTROL_HIGH_WATER, (ptrData+2));
            }
            else {                                                       // assume set new low level
                fnSetNewValue(FLOW_CONTROL_LOW_WATER, (ptrData+2));
            }
            break;
    #if defined USE_SMTP
        case 'M':                                                        // modify Email address
            ptrData++;
            ucPtr = (unsigned char *)cEmailAdd;
            while ((*ptrData != '&') && (*ptrData != ' ')) {
                *ucPtr++ = *ptrData++;
            }
            *ucPtr = 0;                                                  // terminate string
            if (!fnCheckMailAddress()) {                                 // modify any coded strings and verify address looks valid
                fnSendEmail(0);
                usSentEmails++;
            }
            break;
        #if defined SMTP_PARAMETERS
        case 'X':                                                        // modify SMTP details
            switch (*ptrData++) {
            case '0':                                                    // User name
                ucPtr = temp_pars->temp_parameters.ucSMTP_user_name;
                break;
            case '1':                                                    // User password
                ucPtr = temp_pars->temp_parameters.ucSMTP_password;
                break;
            case '2':                                                    // User's email address
                ucPtr = temp_pars->temp_parameters.ucSMTP_user_email;
                break;
            case '3':                                                    // SMTP server
                ucPtr = temp_pars->temp_parameters.ucSMTP_server;
                break;
            default:
                return 0;
            }
            fnWebStrcpy((CHAR *)ucPtr, (++ptrData));                     // {2}
            break;
        #endif
    #endif

    #if defined HTTP_DYNAMIC_CONTENT                                     // {5}
            case 'H':
                {
                    MULTIPLICATION_TABLE *ptrMulTable;
                    if (http_session->ptrUserData == 0) {                // if no user data space belonging to this session create it
                        http_session->ptrUserData = uMalloc(sizeof(MULTIPLICATION_TABLE));
                    }
                    ptrMulTable = (MULTIPLICATION_TABLE *)http_session->ptrUserData;
                    if (*ptrData == '1') {
                        ptrMulTable->ucColumns = (unsigned char)fnDecStrHex(ptrData + 2);
                        if (ptrMulTable->ucColumns < 1) {
                            ptrMulTable->ucColumns = 1;
                        }
                        else if (ptrMulTable->ucColumns > 12) {
                            ptrMulTable->ucColumns = 12;
                        }
                    }
                    else if (*ptrData == '2') {
                        ptrMulTable->usRows = (unsigned short)fnDecStrHex(ptrData + 2);
                        if (ptrMulTable->usRows < 1) {
                            ptrMulTable->usRows = 1;
                        }
                        else if (ptrMulTable->usRows > 1000) {
                            ptrMulTable->usRows = 1000;
                        }
                    }
                }
                break;
    #endif

        default:                                                         // page refreshes should be performed using 'r', which is then not processed
            break;
    }
    return 0;                                                            // OK to display the requested file
}

// We can add strings to a web page being transmitted. If the value usTxLength is not zero we are expected to generate dynamic HTML.
//
#if defined HTTP_DYNAMIC_CONTENT                                         // {6}{8}
static CHAR *fnInsertString(unsigned char *ptrBuffer, LENGTH_CHUNK_COUNT TxLength, unsigned short *usLengthToSend, HTTP *http_session)
#else
static CHAR *fnInsertString(unsigned char *ptrBuffer, LENGTH_CHUNK_COUNT TxLength, unsigned short *usLengthToSend)
#endif
{
    // £vXY commands - to display a value, string etc.
    //
#if !defined WEB_ESCAPE_LEN
    #define WEB_ESCAPE_LEN 4
#endif
#if defined LCD_WEB_INTERFACE                                            // {22}{19}{25} larger buffer for full line of display content
    #define MAX_SIMPLE_INSERT ((GLCD_X * 3) + WEB_ESCAPE_LEN)            // longest message
#else
    #define MAX_SIMPLE_INSERT 60                                         // longest message
#endif
#if defined REFRESH_DEMO
    static int iTest = 0;
#endif
    static unsigned long ulConnectionCount = 0;                          // {29}
    static const CHAR cNoEntry[] = {'-', ' ', '-', ' ', '-'};
    static       CHAR cValue[MAX_SIMPLE_INSERT];                         // space to hold statistic strings (longest is device ID)
    CHAR *cPtr = 0;

    if (TxLength == 0) {                                                 // this means that we have a simple string field to be filled
        switch (*ptrBuffer++) {
#if defined USE_IP_STATS
        case 's':                                                        // LAN statistics
            *usLengthToSend = (fnBufferDec(fnGetEthernetStats((unsigned char)(*ptrBuffer - 'a'), DEFAULT_NETWORK), 0, cValue) - cValue);
            break;
#endif

        case 'K':                                                        // we insert the color a key is to be displayed, depending on output setting
            uStrcpy(cValue, cBackgroundColor);
#if defined USE_MAINTENANCE && !defined REMOVE_PORT_INITIALISATIONS      // {12}
            if (fnUserPortState(*ptrBuffer)) {
                uStrcpy(&cValue[(sizeof(cBackgroundColor) - 1)], cOnColor);
            }
            else {
                uStrcpy(&cValue[(sizeof(cBackgroundColor) - 1)], cOffColor);
            }
#else
            uStrcpy(&cValue[(sizeof(cBackgroundColor) - 1)], cOffColor);
#endif
            *usLengthToSend = (sizeof(cOffColor) - 1) + (sizeof(cBackgroundColor) - 1); // {17} sizeof(cOffColor) rather than sizeof(cOffColor)
            break;

#if defined REFRESH_DEMO
        case 'r':
            if (!iTest) {
                uStrcpy(cValue, cRefresh);
                *usLengthToSend = (sizeof(cRefresh) - 1);
            }
            else {
                return 0;
            }
            break;
#endif
        case 'C':                                                        // {29} display counter
            *usLengthToSend = (fnBufferDec(++ulConnectionCount, 0, cValue) - cValue); // display the amount fo times this variable has been requested. This is approximately the same as the number of times the web page has been served (altough repetitions will be counted)
            break;

        case 'm':                                                        // memory use
            {
                unsigned long ulHeap;
                switch (*ptrBuffer) {
                case 'f':                                                // free heap
                    ulHeap = fnHeapFree();
                    break;
                case 'h':                                                // total heap
                    ulHeap = fnHeapAvailable();
                    break;
                case 's':                                                // unused stack
                    {
                        STACK_REQUIREMENTS freeStack;
                        STACK_REQUIREMENTS stackUsed;                    // {44}
                        CHAR *ptrBuf = cValue;
                        freeStack = fnStackFree(&stackUsed);
                        ptrBuf = fnBufferHex(freeStack, (sizeof(freeStack) | WITH_LEADIN), ptrBuf);
                        *ptrBuf++ = ' ';
                        *ptrBuf++ = '[';
                        ptrBuf = fnBufferHex(stackUsed, (sizeof(stackUsed) | WITH_LEADIN), ptrBuf);
                        *ptrBuf++ = ']';
                        *usLengthToSend = (ptrBuf - cValue);             // 0x0000 [0x0000]
                    }
                    return cValue;
                default:
                    return 0;
                }
                fnBufferHex(ulHeap, (sizeof(ulHeap) | WITH_LEADIN), cValue);
                *usLengthToSend = (sizeof(ulHeap) + 2);                  // 0x0000
            }
            break;

        case 'a':                                                        // show an ARP IP entry
            {
                ARP_TAB *PtrEntry = fnGetARPentry((unsigned char)(*ptrBuffer - 'a')); // get pointer to ARP entry
                if (PtrEntry != 0) {                                     // {24} check whether valid entry
                    cPtr = (CHAR *)PtrEntry->ucIP;                       // get pointer to IP address
                    *usLengthToSend = (fnIPStr((unsigned char *)cPtr, cValue) - cValue);
                }
                else {
                    *usLengthToSend = (sizeof(cNoEntry) - 1);
                    return (CHAR *)cNoEntry;
                }
            }
            break;

        case 'A':                                                        // show an ARP MAC entry
            {
                ARP_TAB *PtrEntry = fnGetARPentry((unsigned char)(*ptrBuffer - 'a')); // get pointer to ARP entry
                if (PtrEntry != 0) {                                     // {24} check whether valid entry
                    cPtr = (CHAR *)PtrEntry->ucMac;                      // get pointer to IP address
                    *usLengthToSend = (fnMACStr((unsigned char *)cPtr, cValue) - cValue);
                }
                else {
                    *usLengthToSend = (sizeof(cNoEntry) - 1);
                    return (CHAR *)cNoEntry;
                }
            }
            break;
                       
#if IP_INTERFACE_COUNT > 1                                               // {40}
        case 'i':
            {
                ARP_TAB *PtrEntry = fnGetARPentry((unsigned char)(*ptrBuffer - 'a')); // get pointer to ARP entry
                if (PtrEntry != 0) {                                     // the interface number
                    cPtr = cValue;
                    *usLengthToSend = (fnBufferHex(PtrEntry->ucInterface, (sizeof(PtrEntry->ucInterface) | WITH_LEADIN), cPtr) - cValue);
                }
                else {
                    *usLengthToSend = (sizeof(cNoEntry) - 1);
                    return (CHAR *)cNoEntry;
                }
            }
            break;
#endif
#if defined USE_IPV6
        case 'n':                                                        // show an IPV6 neighbor entry
            cPtr = (CHAR *)fnGetNeighborEntry((unsigned char)(*ptrBuffer - 'a'), GET_IP); // get pointer to IP address
            if (cPtr != 0) {                                             // check whether valid entry
                *usLengthToSend = (fnIPV6Str((unsigned char *)cPtr, cValue) - cValue);
            }
            else {
                *usLengthToSend = sizeof(cNoEntry) - 1;
                return (CHAR *)cNoEntry;
            }
            break;

        case 'h':                                                        // show an IPV6 neighbor MAC entry
            cPtr = (CHAR *)fnGetNeighborEntry((unsigned char)(*ptrBuffer - 'a'), GET_MAC); // get pointer to HW address
            if (cPtr != 0) {                                             // check whether valid entry
                *usLengthToSend = (fnMACStr((unsigned char *)cPtr, cValue) - cValue); // {9}  - not required with STRING_OPTIMISATION
            }
            else {
                *usLengthToSend = sizeof(cNoEntry) - 1;
                return (CHAR *)cNoEntry;
            }
            break;
#endif
        case 'U':                                                        // display the time the device has been running
            if (*ptrBuffer == '0') {
                cPtr = fnUpTime(cValue);
                *usLengthToSend = ((CHAR *)cPtr - cValue);
            }
#if defined _M5223X || defined _LPC23XX || defined _KINETIS || defined _HW_SAM7X || defined _LPC17XX || defined _STM32 || defined _HW_AVR32 // {31}{33}{34}{35}{36}
            else if (*ptrBuffer == '2') {                                // {18} display last reset cause
                *usLengthToSend = fnAddResetCause(cValue);
            }
#endif
#if defined SUPPORT_RTC || defined USE_TIME_SERVER || defined SUPPORT_SW_RTC // {32} RTC has priority over time server
            else {
                *usLengthToSend = fnSetShowTime(DISPLAY_RTC_TIME_DATE, cValue);
            }
#else
            else {
                *usLengthToSend = sizeof(cNoEntry) - 1;
                return (CHAR *)cNoEntry;
            }
#endif
            break;

        case 'u':                                                        // user name
            cPtr = &temp_pars->temp_parameters.cUserName[0];
            while (*cPtr > '&') {                                        // get the length of user name (uses & as terminator)
                cPtr++;
            }
            *usLengthToSend = (cPtr - temp_pars->temp_parameters.cUserName);
            return (&temp_pars->temp_parameters.cUserName[0]);

        case 'I':                                                        // show an IP address
            switch (*ptrBuffer) {
            case 'i':                                                    // our IP address
                cPtr = (CHAR *)&temp_pars->temp_network[DEFAULT_NETWORK].ucOurIP[0];
                break;
            case 's':                                                    // our sub-net mask
                cPtr = (CHAR *)&temp_pars->temp_network[DEFAULT_NETWORK].ucNetMask[0];
                break;
            case 'g':                                                    // default gateway address
                cPtr = (CHAR *)&temp_pars->temp_network[DEFAULT_NETWORK].ucDefGW[0];
                break;
            case 't':                                                    // IPv6 in IPv4 tunnel address
#if defined USE_IPV6 && defined USE_IPV6INV4
                cPtr = (CHAR *)&temp_pars->temp_network[DEFAULT_NETWORK].ucTunnelIPV4[0];
                break;
#endif
            case 'V':                                                    // IP protocol version
                cValue[0] = 'I';
                cValue[1] = 'P';
                cValue[2] = 'v';
#if defined USE_IPV6                                                     // {38}
                if (present_tcp->ucTCPInternalFlags & TCP_OVER_IPV6) {   // present connection using IPv6 rather than IPv4
                    cValue[3] = '6';
                }
                else {
                    cValue[3] = '4';
                }
#else
                cValue[3] = '4';
#endif
                *usLengthToSend = 4;
                return cValue;

            case 'v':                                                    // {28}
#if defined USE_IPV6                                                     // {38}
                if (present_tcp->ucTCPInternalFlags & TCP_OVER_IPV6) {   // present connection using IPv6 rather than IPv4
                    *usLengthToSend = (fnIPV6Str(present_tcp->ucRemoteIP, cValue) - cValue); // IPV6 address
                    return cValue;
                }
                else {
                    cPtr = (CHAR *)present_tcp->ucRemoteIP;              // remote IPv4 address of connection
                }
#else
                cPtr = (CHAR *)present_tcp->ucRemoteIP;                  // remote IP address of connection
#endif
                break;

            case 'd':                                                    // show value set by dhcp, if active
#if defined USE_PARAMETER_BLOCK
                if (parameters->usServers[DEFAULT_NETWORK] & ACTIVE_DHCP)
#else
                if (temp_pars->temp_parameters.usServers[DEFAULT_NETWORK] & ACTIVE_DHCP)
#endif
                {
                    cPtr = (CHAR *)&network[DEFAULT_NETWORK].ucOurIP[0]; // display working address
                }
                else {
                    *usLengthToSend = 4;
                    return (CHAR *)cucNullMACIP;                         // display nothing
                }
                break;

            case 'm':                                                    // show value set by dhcp, if active
#if defined USE_PARAMETER_BLOCK
                if (parameters->usServers[DEFAULT_NETWORK] & ACTIVE_DHCP)
#else
                if (temp_pars->temp_parameters.usServers[DEFAULT_NETWORK] & ACTIVE_DHCP)
#endif
                {
                    cPtr = (CHAR *)&network[DEFAULT_NETWORK].ucNetMask[0]; // display working sub-net mask
                }
                else {
                    *usLengthToSend = 4;
                    return (CHAR *)cucNullMACIP;                         // display nothing
                }
                break;

            case 'G':                                                    // show value set by dhcp, if active
#if defined USE_PARAMETER_BLOCK
                if (parameters->usServers[DEFAULT_NETWORK] & ACTIVE_DHCP)
#else
                if (temp_pars->temp_parameters.usServers[DEFAULT_NETWORK] & ACTIVE_DHCP)
#endif
                {
                    cPtr = (CHAR *)&network[DEFAULT_NETWORK].ucDefGW[0]; // display working gateway
                }
                else {
                    *usLengthToSend = 4;
                    return (CHAR *)cucNullMACIP;                         // display nothing
                }
                break;
#if defined SMTP_PARAMETERS
            case 'S':                                                    // SMTP IP address
                cPtr = (CHAR *)&ucSMTP_server[0];
                break;
#endif
            case '6':
            case '7':                                                    // {39}
#if defined USE_IPV6                                                     // {23}
                if (*ptrBuffer == '7') {
                    *usLengthToSend = (fnIPV6Str(&temp_pars->temp_network[DEFAULT_NETWORK].ucOurIPV6[0], cValue) - cValue); // IPV6 global address
                }
                else {
                    *usLengthToSend = (fnIPV6Str(ucLinkLocalIPv6Address[DEFAULT_NETWORK], cValue) - cValue); // display link-local IPv6 address
                }
                return cValue;
#else
                *usLengthToSend = sizeof(cNoEntry) - 1;
                return (CHAR *)cNoEntry;
#endif
            default:
                *usLengthToSend = (sizeof(cNoEntry) - 1);
                return (CHAR *)cNoEntry;
            }
            *usLengthToSend = (fnIPStr((unsigned char *)cPtr, cValue) - cValue);
            break;

        case 'M':                                                        // show MAC address
#if defined NO_INTERNAL_ETHERNET || defined DEVICE_WITHOUT_ETHERNET
            if (network[DEFAULT_NETWORK].usNetworkOptions & NETWORK_VALUES_FIXED) {
                *usLengthToSend = (fnMACStr(&network[DEFAULT_NETWORK].ucOurMAC[0], cValue) - cValue); // display factory programmed MAC address
                break;
            }
#endif
            *usLengthToSend = (fnMACStr(&temp_pars->temp_network[DEFAULT_NETWORK].ucOurMAC[0], cValue) - cValue); // {14} - not required with STRING_OPTIMISATION
            break;

        case 'N':                                                        // show Device ID/Name
            {
                int i = 0;
#if defined REFRESH_DEMO
                if (iTest) {
#endif
                    while ((cValue[i] = temp_pars->temp_parameters.cDeviceIDName[i]) != 0) {
                        i++;
                    }
                    *usLengthToSend = i;
#if defined REFRESH_DEMO
                    iTest = 0;
                    }
                else {
                    uMemset(cValue, ' ', 4);
                    *usLengthToSend = 4;
                    iTest = 1;
                }
#endif
            }
            break;

        case 'S':                                                        // show serial number
            cPtr = fnShowSN(cValue);
            *usLengthToSend = (cPtr - cValue);
            break;

        case 'V':                                                        // show software version
            *usLengthToSend = (sizeof(cSoftwareVersion) - 1);
            return (CHAR *)cSoftwareVersion;

        case 'F':
            if (*ptrBuffer == 'H') {                                     // show flow control high water level (%)
                cPtr = fnBufferDec(temp_pars->temp_parameters.ucFlowHigh, 0, cValue);
            }
            else {                                                       // assume show flow control low water level (%)
                cPtr = fnBufferDec(temp_pars->temp_parameters.ucFlowLow, 0, cValue);
            }
            *usLengthToSend = (cPtr - cValue);
            break;

        case 'p':                                                        // show a port
            if (*ptrBuffer == 'T') {                                     // Telnet port
                cPtr = fnBufferDec(temp_pars->temp_parameters.usTelnetPort, 0, cValue);
            }
            *usLengthToSend = (cPtr - cValue);
            break;
#if defined USE_SMTP
    #if defined SMTP_PARAMETERS
        case 'X':                                                        // display SMTP details
            switch (*ptrBuffer) {
            case '0':                                                    // user name
            case '1':                                                    // user password {4}
              //cPtr = (CHAR *)temp_pars->temp_parameters.ucSMTP_password;
                cPtr = (CHAR *)temp_pars->temp_parameters.ucSMTP_user_name;
                break;

            case '2':                                                    // user's email address
                cPtr = (CHAR *)temp_pars->temp_parameters.ucSMTP_user_email;
                break;
            case '3':                                                    // SMTP server
                cPtr = (CHAR *)temp_pars->temp_parameters.ucSMTP_server;
                break;
            case '4':
                fnBufferDec(uTaskerSystemTick, 0, cValue);               // {26} display the present TICK value
                cPtr = cValue;
                break;
            default:
                *usLengthToSend = 0;
                return 0;
            }
            *usLengthToSend = uStrlen(cPtr);
            return cPtr;
    #endif
#endif
        case 'E':                                                        // display Email address
#if defined USE_SMTP
            *usLengthToSend = uStrlen(cEmailAdd);
            return cEmailAdd;
#else
            cValue[0] = ' ';
            *usLengthToSend = 1;
            break;
#endif
        case 'e':
#if defined USE_SMTP
            cPtr = fnBufferDec(usSentEmails, 0, cValue);
            *usLengthToSend = (cPtr - cValue);
#else
            cValue[0] = '0';
            *usLengthToSend = 1;
#endif
            break;

#if defined HTTP_DYNAMIC_CONTENT                                         // {5}
        case 'H':
            {
                MULTIPLICATION_TABLE *ptrMulTable;
                unsigned short usRows = 10;
                unsigned char  ucCols = 10;                              // default start values
                int iSessionValid = 0;

                if (http_session->ptrUserData != 0) {
                    ptrMulTable = (MULTIPLICATION_TABLE *)http_session->ptrUserData;
                    if (ptrMulTable->ucColumns != 0) {                   // if values have just been set valid
                        ucCols = ptrMulTable->ucColumns;
                        usRows = ptrMulTable->usRows;
                        iSessionValid = 1;
                    }
                }
                switch (*ptrBuffer) {
                case '0':
                    if (iSessionValid != 0) {
                        cPtr = uStrcpy(cValue, "<br><br>Here is the requested multiplication table:<br><br>");
                    }
                    else {
                        cPtr = uStrcpy(cValue, "<br><br>Change values and click generate!!<br><br>");
                    }
                    break;

                case '1':                                                // columns
                    cPtr = fnBufferDec(ucCols, 0, cValue);
                    break;

                case '2':                                                // rows
                    cPtr = fnBufferDec(usRows, 0, cValue);
                    break;

                default:
                    return 0;
                }
                *usLengthToSend = (cPtr - cValue);
            }
            break;
#endif
        default:
            *usLengthToSend = (sizeof(cNoEntry) - 1);                    // {41}
            return (CHAR *)cNoEntry;
        }
    }
#if defined HTTP_DYNAMIC_CONTENT                                         // {5}
    else {                                                               // usTxLength contains insertion chunk reference (1...64k)
        if (ptrBuffer == 0) {                                            // special case when session is terminating. This is used to clear any session specific data and only occurs when there is a valid user data pointer
            uMemset(http_session->ptrUserData, 0, sizeof(MULTIPLICATION_TABLE)); // clear content
            return 0;
        }
        if (*ptrBuffer == '0') {
            MULTIPLICATION_TABLE *ptrMulTable;
            LENGTH_CHUNK_COUNT Chunk = (TxLength - 1);                   // {8}
            LENGTH_CHUNK_COUNT X, Y;

            if (http_session->ptrUserData == 0) {
                http_session->ucDynamicFlags = NO_DYNAMIC_CONTENT_TO_ADD;// inform that we don't want to generate anything this time
                *usLengthToSend = 0;
                return cValue;                                           // user session unknown so don't generate anything. This occurs when the window is first opened
            }
            ptrMulTable = (MULTIPLICATION_TABLE *)http_session->ptrUserData;

            if (TxLength > (LENGTH_CHUNK_COUNT)(ptrMulTable->ucColumns * ptrMulTable->usRows)) {
                if (ptrMulTable->ucColumns == 0) {                       // no valid input data
                    http_session->ucDynamicFlags = NO_DYNAMIC_CONTENT_TO_ADD;// inform that we don't want to generate anything this time
                    *usLengthToSend = 0;
                    return cValue;
                }
                return 0;                                                // complete table content generated
            }

            X = (Chunk%ptrMulTable->ucColumns);
            Y = ((Chunk/ptrMulTable->ucColumns) + 1);
            if (X == 0) {
                if (Y == 1) {
                    cPtr = uStrcpy(cValue, "<tr style=""background-color:white""><th width=""60"">"); // start a new table row
                }
                else {
                    cPtr = uStrcpy(cValue, "<tr><th style=""background-color:white"" width=""60"">"); // start a new table row
                }
            }
            else {
                cPtr = uStrcpy(cValue, "<th width=""60"">");             // start a new column
                Y *= (X + 1);                                            // the multiplication result
            }
            cPtr = fnBufferDec(Y, (WITH_TERMINATOR), cPtr);              // insert the result

            if (X == ptrMulTable->ucColumns) {                           // end of this row
                cPtr = uStrcpy(cPtr, "</tr></th>");                      // close table row
            }
            else {
                cPtr = uStrcpy(cPtr, "</th>");                           // close a single column
            }
            *usLengthToSend = (cPtr - cValue);                           // length of code to insert
        }
        else if (*ptrBuffer == '1') {                                    // generate large file content
            static unsigned char ucTable[256 + WEB_ESCAPE_LEN];          // {21} table to be sent in each chunk
            int x = 0;
            http_session->ucDynamicFlags |= GENERATING_DYNAMIC_BINARY;   // ensure the HTTP server knows that we are generating binary content
            if (TxLength > ((8 * 1024 * 1024)/256)) {                    // after generating 8 Meg stop
                return 0;
            }
            while (x < 256) {                                            // {21} 
                ucTable[x] = x;
                x++;
            }
            *usLengthToSend = 256;                                       //  {21}
            if (TxLength == ((8 * 1024 * 1024)/256)) {                   // signal last chunk to ensure that no padding is added
                http_session->ucDynamicFlags |= LAST_DYNAMIC_CONTENT_DATA;
            }
            return (CHAR *)ucTable;
        }
    #if defined LCD_WEB_INTERFACE                                        // {19}{20}{25} generate a BMP from the present display content
        else if (*ptrBuffer == '2') {                                    // generate a BMP containing the present LCD display content
            LENGTH_CHUNK_COUNT Chunk = (TxLength - 1);                   // the present chunk reference
            http_session->ucDynamicFlags = GENERATING_DYNAMIC_BINARY;    // ensure the HTTP server knows that we are generating binary content
            if (Chunk == 0) {                                            // first frame - construct a bit map header 24 bit color
                /* mono-chrome format
                #define W_BMP_HEADER_LENGTH  0x36
                #define W_BMP_CONTENT_SIZE   ((DISPLAY_WIDTH/8) * DISPLAY_HEIGHT)
                #define W_BMP_TOTAL_SIZE     (W_BMP_CONTENT_SIZE + W_BMP_HEADER_LENGTH)
                const W_BITMAPHEADER bm_header = {{0x42, 0x4d}, {(unsigned char)W_BMP_TOTAL_SIZE, (unsigned char)(W_BMP_TOTAL_SIZE >> 8), (unsigned char)(W_BMP_TOTAL_SIZE >> 16), (unsigned char)(W_BMP_TOTAL_SIZE >> 24)}, {0,0,0,0}, {W_BMP_HEADER_LENGTH, 0x00, 0x00, 0x00}};
                const W_BITMAPINFO   bm_info = {{0x28, 0x00, 0x00, 0x00}, {(unsigned char)GLCD_X, (unsigned char)(GLCD_X >> 8), 0x00, 0x00}, {(unsigned char)DISPLAY_HEIGHT, (unsigned char)(DISPLAY_HEIGHT >> 8), 0x00, 0x00}, {0x01, 0x00}, {0x01, 0x00}, {0,0,0,0}, {(unsigned char)W_BMP_CONTENT_SIZE, (unsigned char)(W_BMP_CONTENT_SIZE >> 8), (unsigned char)(W_BMP_CONTENT_SIZE >> 16), (unsigned char)(W_BMP_CONTENT_SIZE >> 24)}, {0x61, 0x0f, 0x00, 0x00}, {0x61, 0x0f, 0x00, 0x00}, {0,0,0,0}, {0,0,0,0}};  
                */
                uMemcpy(cValue, &bm_header, sizeof(bm_header));
                uMemcpy(&cValue[sizeof(bm_header)], &bm_info, sizeof(bm_info));
                *usLengthToSend = (sizeof(bm_header) + sizeof(bm_info));
            }
    #if (3 * DISPLAY_WIDTH) > HTTP_BUFFER_LENGTH                         // reduce the chunk size by factor 3 if a line would occupy more that the HTTP buffer space available
            else if (Chunk > (3 * DISPLAY_HEIGHT))                       // complete bit map returned so quit
    #else
            else if (Chunk > DISPLAY_HEIGHT)                             // complete bit map returned so quit
    #endif
            {
                return 0;
            }
            else {                                                       // bit map content - the complete display is always sent
    #if (defined GLCD_COLOR || defined CGLCD_GLCD_MODE) && !defined SUPPORT_TFT
                extern void et024006_SetLimits(unsigned short x1, unsigned short y1, unsigned short x2, unsigned short y2 );
    #endif
    #if defined _WINDOWS
                extern unsigned char *fnGetSDRAM(unsigned char *ptrSDRAM);
    #else
                #define fnGetSDRAM(x) x
                #define AVR32_EBI_CS0_ADDRESS      0xC0000000
                #define ET024006_CMD_ADDR          ((volatile unsigned short *) AVR32_EBI_CS0_ADDRESS)
                #define ET024006_PARAM_ADDR        ((volatile unsigned short *) (AVR32_EBI_CS0_ADDRESS + 0x00200000))
                #define WRITE_REG(add, value)      *ET024006_CMD_ADDR = add; *ET024006_PARAM_ADDR = value
                #define SELECT_REG(add)            *ET024006_CMD_ADDR = add
                #define WRITE_DATA(value)          *ET024006_PARAM_ADDR = value
    #endif
                register unsigned long ulPixel;
                int x;
    #if defined SUPPORT_TFT || defined TFT_GLCD_MODE
                unsigned long *ptDst = (unsigned long *)fnGetSDRAM((unsigned char *)SDRAM_ADDR); // the start of SDRAM where the image is located
                ptDst += ((GLCD_Y - 1) * GLCD_X);                        // inverted bit maps, so move to bottom of image ready to scan upwards
        #if (3 * DISPLAY_WIDTH) > HTTP_BUFFER_LENGTH                     // reduce the chunk size by factor 3 if a line would occupy more that the HTTP buffer space available
                ptDst -= ((Chunk - 1)/3 * GLCD_X);                       // move up to the line that is to be displayed (this assumes that the width can be divided exactly by 3 - valid for 480 byte width)
                ptDst += (((Chunk - 1)%3) * (GLCD_X/3));                 // move to the location in the line to be displayed
        #else
                ptDst -= ((Chunk - 1) * GLCD_X);                         // move up to the line that is to be displayed
        #endif
    #elif defined GLCD_COLOR || defined CGLCD_GLCD_MODE
                #define HIMAX_SRAMREAD 0x22
                et024006_SetLimits( 0, (unsigned short)(GLCD_Y - Chunk), GLCD_X, (unsigned short)(GLCD_Y - Chunk)); // set the address reading range
        #if defined _WINDOWS
                CollectCommand(1, HIMAX_SRAMREAD);
        #else
                SELECT_REG(HIMAX_SRAMREAD);                              // prepare read
                ulPixel = *ET024006_PARAM_ADDR;                          // dummy read
        #endif
    #endif
    #if (3 * DISPLAY_WIDTH) > HTTP_BUFFER_LENGTH                         // reduce the chunk size by factor 3 if a line would occupy more that the HTTP buffer space available
                for (x = 0; x < DISPLAY_WIDTH; x += 3)
    #else
                for (x = 0; x < (3 * DISPLAY_WIDTH); x += 3)             // 24 bit pixels require 3 bytes of data for each pixel
    #endif
                {
    #if defined SUPPORT_TFT || defined TFT_GLCD_MODE                     // generate the BMP content directly from SDRAM memory
                    ulPixel = *ptDst++;
                    cValue[x]     = (unsigned char)(ulPixel >> 16);
                    cValue[x + 1] = (unsigned char)(ulPixel >> 8);
                    cValue[x + 2] = (unsigned char)(ulPixel);
    #elif defined GLCD_COLOR || defined CGLCD_GLCD_MODE                  // generate the BMP content by reading it from the display
        #if defined _WINDOWS
                    ulPixel = ReadDisplay(1);                            // read the red content
                    cValue[x + 2] = (unsigned char)(ulPixel);
                    ulPixel = ReadDisplay(1);                            // read the green content
                    cValue[x + 1] = (unsigned char)(ulPixel);
                    ulPixel = ReadDisplay(1);                            // read the blue content
                    cValue[x]     = (unsigned char)(ulPixel);
        #else
                    ulPixel = *ET024006_PARAM_ADDR;                      // read the red content
                    cValue[x + 2] = (unsigned char)(ulPixel);
                    ulPixel = *ET024006_PARAM_ADDR;                      // read the green content
                    cValue[x + 1] = (unsigned char)(ulPixel);
                    ulPixel = *ET024006_PARAM_ADDR;                      // read the blue content
                    cValue[x]     = (unsigned char)(ulPixel);
        #endif
    #else                                                                // mono-GLCD
                    extern int iGetPixelState(unsigned long ulPixelNumber);
                    ulPixel = iGetPixelState(((DISPLAY_HEIGHT - Chunk) * DISPLAY_WIDTH) + (x/3)); // generate the BMP content by reading the backup buffer - this method is also used when it is not possible to read from the display
                    if (ulPixel != 0) {
        #if defined MB785_GLCD_MODE || defined _HX8347 || defined TFT2N0369_GLCD_MODE || defined ST7789S_GLCD_MODE
                        cValue[x + 2] = (unsigned char)((LCD_PIXEL_COLOUR & RED)  >> 8);
                        cValue[x + 1] = (unsigned char)((LCD_PIXEL_COLOUR & GREEN) >> 3);
                        cValue[x]     = (unsigned char)((LCD_PIXEL_COLOUR & BLUE) << 3);
        #elif defined ST7565S_GLCD_MODE                                  // {43}
                        cValue[x + 2] = (unsigned char)((LCD_PIXEL_COLOUR & 0x00f) << 4);
                        cValue[x + 1] = (unsigned char) (LCD_PIXEL_COLOUR & 0x0f0);
                        cValue[x]     = (unsigned char)((LCD_PIXEL_COLOUR & 0xf00) >> 4);
        #else
                        cValue[x + 2] = (unsigned char)(LCD_PIXEL_COLOUR);
                        cValue[x + 1] = (unsigned char)(LCD_PIXEL_COLOUR >> 8);
                        cValue[x]     = (unsigned char)(LCD_PIXEL_COLOUR >> 16);
        #endif
                    }
                    else {
        #if defined MB785_GLCD_MODE || defined _HX8347 || defined TFT2N0369_GLCD_MODE || defined ST7789S_GLCD_MODE
                        cValue[x + 2] = (unsigned char)((LCD_ON_COLOUR & RED)  >> 8);
                        cValue[x + 1] = (unsigned char)((LCD_ON_COLOUR & GREEN) >> 3);
                        cValue[x]     = (unsigned char)((LCD_ON_COLOUR & BLUE) << 3);
        #elif defined ST7565S_GLCD_MODE                                  // {43}
                        cValue[x + 2] = (unsigned char)((LCD_ON_COLOUR & 0x00f) << 4);
                        cValue[x + 1] = (unsigned char) (LCD_ON_COLOUR & 0x0f0);
                        cValue[x]     = (unsigned char)((LCD_ON_COLOUR & 0xf00) >> 4);
        #else
                        cValue[x + 2] = (unsigned char)(LCD_ON_COLOUR);
                        cValue[x + 1] = (unsigned char)(LCD_ON_COLOUR >> 8);
                        cValue[x]     = (unsigned char)(LCD_ON_COLOUR >> 16);
        #endif
                    }
    #endif
                }
    #if (3 * DISPLAY_WIDTH) > HTTP_BUFFER_LENGTH                         // reduce the chunk size by factor 3 if a line would occupy more that the HTTP buffer space available
                *usLengthToSend = (DISPLAY_WIDTH);                       // one third of an LCD line per chunk
    #else
                *usLengthToSend = (DISPLAY_WIDTH * 3);                   // one LCD line per chunk
    #endif
    #if (3 * DISPLAY_WIDTH) > HTTP_BUFFER_LENGTH                         // reduce the chunk size by factor 3 if a line would occupy more that the HTTP buffer space available
                if (Chunk == (DISPLAY_HEIGHT * 3))                       // last chunk sent so flag this
    #else
                if (Chunk == DISPLAY_HEIGHT)                             // last chunk sent so flag this
    #endif
                {
                    http_session->ucDynamicFlags = (GENERATING_DYNAMIC_BINARY | LAST_DYNAMIC_CONTENT_DATA);
                }
            }
        }
    #endif
    }
#endif
#if defined _WINDOWS                                                     // this will usually also be caught by newer VisualStudio versions when it performs stack usage checking
    if (*usLengthToSend > MAX_SIMPLE_INSERT) {
        _EXCEPTION("WARNING: the buffer cValue[] is too small - increase its size to avoid possible corruption!!!!");
    }
#endif
    return cValue;
}


/************************************************* Detail work here ****************************************************/


// Reset board after a delay to allow present web page to complete serving
//
static void fnDelayResetBoard(void)
{
    uTaskerMonoTimer( TASK_APPLICATION, (DELAY_LIMIT)(2*SEC), E_TIMER_SW_DELAYED_RESET );
}


// Get server types
//
static unsigned short fnGetServerType(unsigned char ucType)
{
    switch (ucType) {
    case 'D':                                                            // DHCP server state
        return ACTIVE_DHCP;
#if defined USE_IPV6                                                     // {23}
    case 'd':                                                            // DHCPV6 server state
        return ACTIVE_DHCPV6;
#endif
#if defined USE_SMTP_AUTHENTICATION
    case 's':                                                            // SMTP authentication
        return SMTP_LOGIN;
#endif

    case 'S':                                                            // terminal menu login
        return ACTIVE_LOGIN;

    case 'T':                                                            // TELNET server state
        return ACTIVE_TELNET_SERVER;

    case 'F':                                                            // FTP server state
        return ACTIVE_FTP_SERVER;

    case 'A':
        return AUTHENTICATE_WEB_SERVER;                                  // authentication setting

    default:
        return 0;                                                        // unknown server type
    }
}

// Get serial mode settings
//
static unsigned short fnGetSerialMode(unsigned char ucMode, unsigned char *ucForceCheck)
{
    unsigned char ucForceChk = NOT_SELECTED;

    switch (ucMode) {
    case 'E':                                                            // even parity
        return RS232_EVEN_PARITY;
    case 'O':                                                            // odd parity
        return RS232_ODD_PARITY;
    case 'N':                                                            // no parity
        if (!(temp_pars->temp_parameters.SerialMode & (RS232_EVEN_PARITY | RS232_ODD_PARITY))) {
            ucForceChk = IS_CHECKED;
        }
        break;
    case '1':                                                            // one stop bit
        if (!(temp_pars->temp_parameters.SerialMode & (TWO_STOPS | ONE_HALF_STOPS))) {
            ucForceChk = IS_CHECKED;
        }
        break;
    case '2':                                                            // 2 stop bits
        return TWO_STOPS;

    case 'H':                                                            // 1.5 stop bits
        return ONE_HALF_STOPS;

    case '8':                                                            // 8 bit characters
        if (!(temp_pars->temp_parameters.SerialMode & CHAR_7)) {
            ucForceChk = IS_CHECKED;
        }
        break;

    case '7':                                                            // 7 bit characters
        return CHAR_7;

    case 'X':                                                            // XON/XOFF protcoll
        return USE_XON_OFF;

    case 'R':                                                            // RTS/CTS protocol
        return RTS_CTS;

    case 'n':                                                            // no flow control
        if (!(temp_pars->temp_parameters.SerialMode & (USE_XON_OFF | RTS_CTS))) {
            ucForceChk = IS_CHECKED;
        }
        break;

    default:
        break;
    }

    if ((ucForceChk != NOT_SELECTED) && (ucForceCheck)) {
        *ucForceCheck = ucForceChk;
    }
    return 0;
}



// Get present network modes
//
static unsigned short fnGetNetworkMode(unsigned char ucMode, unsigned char *ucForceCheck)
{
    unsigned char ucForceChk = NOT_SELECTED;

    switch (ucMode) {
    case 'F':                                                            // fast - 100M LAN
        return LAN_100M;
    case 'S':                                                            // slow - 10M LAN
        return LAN_10M;
    case 'D':
        return FULL_DUPLEX;
    case 'A':                                                            // auto-negotiation
        if (!(temp_pars->temp_network[DEFAULT_NETWORK].usNetworkOptions & (LAN_10M | LAN_100M))) {
            ucForceChk = IS_CHECKED;
        }
        break;
    default:
        break;
    }

    if ((ucForceChk != NOT_SELECTED) && (ucForceCheck)) {
        *ucForceCheck = ucForceChk;
    }

    return 0;
}


extern int fnModified(CHAR cPar)
{
    switch (cPar) {
    case '0':                                                            // special case to check whether MAC is non zero
        return (!uMemcmp(temp_pars->temp_network[DEFAULT_NETWORK].ucOurMAC, cucNullMACIP, MAC_LENGTH));

    case '1':                                                            // IP address changed?
        return (uMemcmp(&temp_pars->temp_network[DEFAULT_NETWORK].ucOurIP[0], &network_flash[DEFAULT_NETWORK].ucOurIP[0], IPV4_LENGTH));

    case '2':                                                            // subnet mask address changed?
        return (uMemcmp(&temp_pars->temp_network[DEFAULT_NETWORK].ucNetMask[0], &network_flash[DEFAULT_NETWORK].ucNetMask[0], IPV4_LENGTH));

    case '3':                                                            // gateway address changed?
        return (uMemcmp(&temp_pars->temp_network[DEFAULT_NETWORK].ucDefGW[0], &network_flash[DEFAULT_NETWORK].ucDefGW[0], IPV4_LENGTH));

    case '4':                                                            // network options changed?
        return (temp_pars->temp_network[DEFAULT_NETWORK].usNetworkOptions != network_flash[DEFAULT_NETWORK].usNetworkOptions);

    case '5':                                                            // DHCP setting change?
#if defined USE_PARAMETER_BLOCK
        return ((temp_pars->temp_parameters.usServers[DEFAULT_NETWORK] & ACTIVE_DHCP) != (parameters->usServers[DEFAULT_NETWORK] & ACTIVE_DHCP));
#else
        return 0;
#endif

    case '6':                                                            // validated Ethernet settings?
        return (fnAreWeValidating() == 0);
#if defined USE_IPV6                                                     // {23}
    case '7':                                                            // IPV6 global address changed?
    #if defined USE_PARAMETER_BLOCK
        return (uMemcmp(&temp_pars->temp_network[DEFAULT_NETWORK].ucOurIPV6[0], &network_flash[DEFAULT_NETWORK].ucOurIPV6[0], IPV6_LENGTH));
    #else
        return 0;
    #endif

    case '8':                                                            // DHCPV6 setting change?
    #if defined USE_PARAMETER_BLOCK
        return ((temp_pars->temp_parameters.usServers[DEFAULT_NETWORK] & ACTIVE_DHCPV6) != (parameters->usServers[DEFAULT_NETWORK] & ACTIVE_DHCPV6));
    #else
        return 0;
    #endif

    #if defined USE_IPV6INV4
    case '9':                                                            // IPv6 in IPv4 tunnel address
        #if defined USE_PARAMETER_BLOCK
        return (uMemcmp(&temp_pars->temp_network[DEFAULT_NETWORK].ucTunnelIPV4[0], &network_flash[DEFAULT_NETWORK].ucTunnelIPV4[0], IPV4_LENGTH));
        #else
        return 0;
        #endif
    #endif
#endif
    default:
        return 0;
    }

}

#if defined USE_SMTP
// The main reason for this function is to convert the coded @ sign sent by Firefix and co.
//
static int fnCheckMailAddress(void)
{
    CHAR *cPtrIn = cEmailAdd;
    CHAR *cPtrOut = cPtrIn;
    int iaT_Notfound = 1;

    while (*cPtrIn != 0) {
        if (*cPtrIn == '%') {
            unsigned char ucValue1 = *(++cPtrIn) - '0';
            unsigned char ucValue2 = *(++cPtrIn) - '0';
            if (ucValue1 > 9) ucValue1 -= 'A' - '9'- 1;                  // we assume capitals
            if (ucValue2 > 9) ucValue2 -= 'A' - '9'- 1;
            ucValue1 <<= 4;
            ucValue1 |= ucValue2;
            *cPtrOut = ucValue1;                                         // we have to replace the HEX ASCII value following with its hex value
        }
        else {
            *cPtrOut = *cPtrIn;
        }
        if (*cPtrOut == '@') {
            iaT_Notfound = 0;
        }
        ++cPtrOut;
        ++cPtrIn;
    }
    *cPtrOut = 0;
    return iaT_Notfound;
}
#endif


#if defined SUPPORT_WEBSOCKET
static int fnWebSocketHandler(unsigned char *ptrPayload, unsigned long ulPayloadLength, unsigned char ucOpCode, HTTP *http_session)
{
    if ((ucOpCode & WEBSOCKET_CONTROL_FRAME) != 0) {                     // control frame
        switch (ucOpCode & WEB_SOCKET_OPCODE_MASK) {
        case WEB_SOCKET_OPCODE_CONNECTION_CLOSE:                         // when a close handshake is receive (which can also have a message body) a close it returned in response and the server closes the connection (possibly after completing a fragmented message that is in progress)
            break;
        case WEB_SOCKET_OPCODE_PING:
            break;
        case WEB_SOCKET_OPCODE_PONG:
            break;
        }
    }
    return (fnWebSocketSend(ptrPayload, ulPayloadLength, ucOpCode, http_session)); // echo message back
}
#endif
#endif
