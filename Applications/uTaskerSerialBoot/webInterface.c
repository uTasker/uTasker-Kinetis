/************************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher

    ---------------------------------------------------------------------
    File:      webInterface.c
    Project:   Ethernet Boot Loader
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2016
    *********************************************************************
    03.06.2014 Add file object when working together with USB-MSD        {1}
    04.06.2014 Add nework indicator task to allow PHY polling when necessary {2}
    21.01.2015 Add file object whenever USB-MSD is enabled               {3}
    12.06.2015 Close outstanding flash buffer before committing start of code {4}
    07.07 2020 Parameters for fnStartHTTP() modified                     {5}

*/


/* =================================================================== */
/*                           include files                             */
/* =================================================================== */

#include "config.h"

/* =================================================================== */
/*                             constants                               */
/* =================================================================== */

#if defined USE_HTTP
const CHAR  cSoftwareVersion[] = SOFTWARE_VERSION;                       // software version for general purpose display use

/* =================================================================== */
/*                      local structure definitions                    */
/* =================================================================== */

/* =================================================================== */
/*                          local definitions                          */
/* =================================================================== */

/* =================================================================== */
/*                 local function prototype declarations               */
/* =================================================================== */

static void fnDelayResetBoard(void);
static int  fnCheckErasePassword(int iType, CHAR *ptrData);

#endif

/* =================================================================== */
/*                     global variable definitions                     */
/* =================================================================== */

#if defined USE_HTTP || defined USE_TFTP
NETWORK_PARAMETERS network[IP_NETWORK_COUNT] = {                         // fixed network values
    {
    (AUTO_NEGOTIATE /*| FULL_DUPLEX*/ | RX_FLOW_CONTROL),                // usNetworkOptions - see driver.h for other possibilities
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x05},                                // ucOurMAC - when no other value can be read from parameters this will be used
    { 192, 168, 0, 125 },                                                // ucOurIP - our default IP address
    { 255, 255, 255, 0 },                                                // ucNetMask - Our default network mask
    { 192, 168, 0, 1 },                                                  // ucDefGW - Our default gateway
    { 192, 168, 0, 1 },                                                  // ucDNS_server - Our default DNS server
    #if defined USE_IPV6
    { _IP6_ADD_DIGIT(0x2001), _IP6_ADD_DIGIT(0x0470), _IP6_ADD_DIGIT(0x0026), _IP6_ADD_DIGIT(0x0105), _IP6_ADD_DIGIT(0x0000), _IP6_ADD_DIGIT(0x0000), _IP6_ADD_DIGIT(0x0000), _IP6_ADD_DIGIT(0x0030) }, // default global IPV6 address
    #endif
    },
};
#endif

/* =================================================================== */
/*                             constants                               */
/* =================================================================== */

#if defined USE_HTTP
const CHAR  cNoSW[] = "No Application SW present";
const CHAR  cSW_detected[] = "Software Detected";
static const CHAR cProcessorName[] = TARGET_HW;                          // processor/board name for reporting/display use

// Erase password
// p12X-k3ve2B1O2Ba
//
const CHAR  cErasePass[] = {'p', '1', '2', 'X', '-', 'k', '3', 'v', 'e', '2', 'B', '1', 'O', '2', 'B', 'a'};

// Mass erase password
// mMm122-aHHHQq1x8
//
const CHAR  cMassErasePass[] = {'m', 'M', 'm', '1', '2', '2', '-', 'a', 'H', 'H', 'H', 'Q', 'q', '1', 'x', '8'};
const CHAR  cEmptyPass[]     = {'*', '*', '*', '*', '*', '*', '*', '*'};
#endif


/* =================================================================== */
/*                      local variable definitions                     */
/* =================================================================== */


#if defined USE_TFTP
    static unsigned char ucTFTP_server_ip[IPV4_LENGTH] = {192, 168, 0, 102};
#elif defined USE_HTTP
static unsigned char  fnIsSelected(unsigned char *ptrBuffer);
#ifdef HTTP_DYNAMIC_CONTENT
    static CHAR      *fnInsertString(unsigned char *ptrBuffer, LENGTH_CHUNK_COUNT TxLength, unsigned short *usLengthToSend, HTTP *http_session);
#else
    static CHAR      *fnInsertString(unsigned char *ptrBuffer, LENGTH_CHUNK_COUNT TxLength, unsigned short *usLengthToSend);
#endif
static int            fnHandleWeb(unsigned char ucType, CHAR *ptrData, HTTP *http_session);

#define UPLOAD_FAILED         "<html><head><title>SW Upload failed</title></head><body bgcolor=#ff9000 text=#000000 topmargin=3 marginheight=3><center><td valign=top class=h><font color=#ff0000 style=font-size:30px><b style='mso-bidi-font-weight:normal'>&micro;Tasker</font></i></b></td><br></td><td align=left><br><br>Sorry but upload has failed.</font><br><br><a href=""javascript:history.back()"">Return</a></body></html>"
#define SW_UPLOAD_COMPLETED   "<html><head><meta http-equiv=""refresh"" content=""5;URL=0Menu.htm""><title>KINETIS SW Uploaded</title></head><body bgcolor=#ffffff text=#000000 topmargin=3 marginheight=3><center><td valign=top class=h><font color=#ff0000 style=font-size:30px><b style='mso-bidi-font-weight:normal'>&micro;Tasker</font> - SW Update</i></b></td><br></td><td align=left><br><br>SW Upload successful. The KINETIS target will now reset and start the new program. Please wait 5s...</body></html>"


static const CHAR cWarningHTML[]       = UPLOAD_FAILED;
static const CHAR cSuccessSW_HTML[]    = SW_UPLOAD_COMPLETED;


extern void fnConfigureAndStartWebServer(void)
{
    HTTP_FUNCTION_SET FunctionSet;                                       // {5}
    FunctionSet.ucParameters = (WEB_SUPPORT_PARAM_GEN | WEB_SUPPORT_HANDLER);
    #if defined AUTHENTICATE_WEB_ACCESS
    FunctionSet.ucParameters = (WEB_SUPPORT_PARAM_GEN | WEB_SUPPORT_HANDLER | WEB_AUTHENTICATE);
    #else
    FunctionSet.ucParameters = (WEB_SUPPORT_PARAM_GEN | WEB_SUPPORT_HANDLER);
    #endif
    FunctionSet.fnWebHandler = fnHandleWeb;
    FunctionSet.fnInsertRoutine = fnInsertString;
    FunctionSet.fnGenerator = fnIsSelected;
    #ifdef _VARIABLE_HTTP_PORT
    FunctionSet.usPort = HTTP_SERVERPORT;
    #endif
    fnStartHTTP(&FunctionSet);
}


// We can define whether a selection is set or not when a web page is displayed
//
static unsigned char fnIsSelected(unsigned char *ptrBuffer)
{
    int iDisabled = 0;
    // Format \xA3sAB (length assumed to be 4 bytes, even when both parameters are not used)
    //
    switch (*(ptrBuffer + 1)) {
    case 's':
        if (*(unsigned long *)fnGetFlashAdd((unsigned char *)_UTASKER_APP_START_) == 0xffffffff) { // check whether there is code at the start address
            iDisabled = 1;
        }
        if (*ptrBuffer == WEB_NOT_DISABLE_FIELD) {                       // if the condition is to be inverted
            iDisabled = !iDisabled;
        }
        if (iDisabled != 0) {
            return IS_DISABLED;
        }
        break;

    default:
        break;

    }
    return NOT_SELECTED;
}

// Here we handle incoming web parameters - we can also change the file to be displayed if we want to
//
static int fnHandleWeb(unsigned char ucType, CHAR *ptrData, HTTP *http_session)
{
    static unsigned char *ptrCode = 0;
    #if defined FLASH_ROW_SIZE && FLASH_ROW_SIZE > 0
        static unsigned char ucCodeStart[FLASH_ROW_SIZE];
    #else
        static unsigned char ucCodeStart[4] = {0xff, 0xff, 0xff, 0xff};
    #endif
    switch (ucType) {
    case 0:                                                              // this is called on each new GET - we can reset variable which collect data here if required
        break;

    case CAN_POST_BEGIN:
        if (http_session->FileLength < MAX_UTASKER_APP_SIZE) { 
            ptrCode = (unsigned char *)_UTASKER_APP_START_;              // note that an upload is starting
        }
        return (0);                                                      // allow post at all times

    case POSTING_DATA_TO_APP:
        if (ptrCode != 0) {
            unsigned short usBuff_length = (unsigned short)http_session->FileLength;
            if (ptrCode == (unsigned char *)_UTASKER_APP_START_) {       // first frame being received
                if (http_session->FileLength < sizeof(ucCodeStart)) {
                    ptrCode = 0;
                    break;
                }
                uMemcpy(ucCodeStart, ptrData, sizeof(ucCodeStart));
                ptrData += sizeof(ucCodeStart);
                ptrCode += sizeof(ucCodeStart);
                usBuff_length -= sizeof(ucCodeStart);
            }
            fnWriteBytesFlash(ptrCode, (unsigned char *)ptrData, usBuff_length); // program the frame to flash
            ptrCode += usBuff_length;
        }
        break;

    case INFORM_POST_SUCCESS:
#if defined SUPPORT_MIME_IDENTIFIER
        http_session->ucMimeType = MIME_HTML;                            // force HTML type
#endif
        if (http_session->cDisplayFile == 'S') {                         // software upload
            if (ptrCode != 0) {
#if defined USB_INTERFACE && defined USB_MSD_DEVICE_LOADER               // {3}
                FILE_OBJECT_INFO fileObjInfo;
                fileObjInfo.ptrLastAddress = ptrCode;
                fileObjInfo.ptrShortFileName = "WEB_LOADBIN";
                fnAddSREC_file(&fileObjInfo);
#endif
#if defined FLASH_ROW_SIZE && FLASH_ROW_SIZE > 0
                fnWriteBytesFlash(0, 0, 0);                              // {4} close any outstanding FLASH buffer
#endif
                fnWriteBytesFlash((unsigned char *)_UTASKER_APP_START_, ucCodeStart, sizeof(ucCodeStart));
                http_session->ptrFileStart = (unsigned char*)cSuccessSW_HTML;
                http_session->FileLength = (sizeof(cSuccessSW_HTML) - 1);
                fnDelayResetBoard();                                     // after 2 seconds the application will reset and then the new software will be copied
            }
            else {
                http_session->ptrFileStart = (unsigned char*)cWarningHTML;
                http_session->FileLength = (sizeof(cWarningHTML) - 1);
            }
            return DISPLAY_INTERNAL;
        }
        break;

    case 'c':                                                            // erase the complete application space
        if (*ptrData++ == '1') {                                         // if erase software
            if (fnCheckErasePassword(1, ++ptrData) != 0) {
                fnEraseFlashSector((unsigned char *)UTASKER_APP_START, (unsigned long)(UTASKER_APP_END - UTASKER_APP_START));
            }
        }
        else {
            if (fnCheckErasePassword(0, ++ptrData) != 0) {
#if defined MASS_ERASE
                fnMassEraseFlash();
#endif
            }
        }
        break;

    default:                                                             // page refreshes should be performed using 'r', which is then not processed
        break;
    }
    return 0;                                                            // OK to display the requested file
}

// We can add strings to a web page being transmitted. If the value usTxLength is not zero we are expected to generate dynamic HTML.
//
#ifdef HTTP_DYNAMIC_CONTENT                                              // {6}{8}
static CHAR *fnInsertString(unsigned char *ptrBuffer, LENGTH_CHUNK_COUNT TxLength, unsigned short *usLengthToSend, HTTP *http_session)
#else
static CHAR *fnInsertString(unsigned char *ptrBuffer, LENGTH_CHUNK_COUNT TxLength, unsigned short *usLengthToSend)
#endif
{
    // \xA3vXY commands - to display a value, string etc.
    //
#if !defined WEB_ESCAPE_LEN
    #define WEB_ESCAPE_LEN 4
#endif

    if (!TxLength) {                                                     // this means that we have a simple string field to be filled
        switch (*ptrBuffer++) {
        case 'V':                                                        // display version
            if (*ptrBuffer == '1') {                                     // display application version
                if (*(unsigned long *)fnGetFlashAdd((unsigned char *)_UTASKER_APP_START_) != 0xffffffff) {
                    *usLengthToSend = (sizeof(cSW_detected) - 1);
                    return (CHAR *)cSW_detected;                         // display that SW is present
                }
                else {
                    *usLengthToSend = (sizeof(cNoSW) - 1);
                    return (CHAR *)cNoSW;                                // display that there is no SW loaded
                }
            }
            else {
                *usLengthToSend = (sizeof(cSoftwareVersion) - 1);
                return (CHAR *)cSoftwareVersion;                         // display loader version
            }
            break;

        case 'e':                                                        // erase password - display as empty string
            *usLengthToSend = (sizeof(cEmptyPass));
            return (CHAR *)cEmptyPass;

        case 'N':                                                        // insert the name of the board/processor
            *usLengthToSend = (sizeof(cProcessorName) - 1);
            return (CHAR *)cProcessorName;

        default:
            break;
        }
    }
    *usLengthToSend = 0;
    return 0;
}


static int fnCheckErasePassword(int iType, CHAR *ptrData)
{
    if (iType != 0) {
        if (!uMemcmp(ptrData, cErasePass, sizeof(cErasePass))) {
            return 1;
        }
    }
    else {
        if (!uMemcmp(ptrData, cMassErasePass, sizeof(cMassErasePass))) {
            return 1;
        }
    }
    return 0;
}
#endif

#if defined USE_TFTP
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
extern void fnTransferTFTP(void)
{
    fnStartTFTP_client(tftp_listener, ucTFTP_server_ip, TFTP_GET, "test.txt", '0'); // get a file (text.txt) from TFTP server and save it locally (to file '0')
    //fnStartTFTP_client(tftp_listener, ucTFTP_server_ip, TFTP_GET_COMPARE, "test.txt", '0'); // get a file (text.txt) from TFTP server and compare it to local file ('0')
    //fnStartTFTP_client(tftp_listener, ucTFTP_server_ip, TFTP_PUT, "test1.txt", '0'); // transfer local file ('0') to TFTP server and save it there as (test1.txt)
}
#endif


#if defined USE_HTTP || defined USE_TFTP
// Reset board after a delay to allow present web page to complete serving
//
static void fnDelayResetBoard(void)
{
    uTaskerMonoTimer(TASK_APPLICATION, (DELAY_LIMIT)(2 * SEC), T_RESET);
}

// Dummy function
//
extern QUEUE_TRANSFER fnNetworkTx(unsigned char *output_buffer, QUEUE_TRANSFER nr_of_bytes)
{
    return 0;
}

// Dummy function
//
extern void fnGetEthernetPars(void)
{
}

#if defined PHY_POLL_LINK
// Task to poll PHY to ensure that link states are synchronised
//
extern void fnNetworkIndicator(TTASKTABLE *ptrTaskTable)                 // {2}
{
    fnCheckEthLinkState();                                               // check the PHY link state (synchronising the EMAC if required)
}
#endif
#endif
