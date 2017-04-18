/************************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher

    ---------------------------------------------------------------------
    File:        snmp.c (SNMPv1, SNMPv2c)
    Project:     Single Chip Embedded Internet
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2016
    *********************************************************************
    30.03.2014 Modify handling single variable binding items in the request since some SNMP managers count ASN1_NULL_CODE as part of the sequence length and some don't {1}
    30.03.2014 Correct ASN1_TIME_STAMP_CODE insersion to use the same format as ucANS1_type {2}
    30.03.2014 Modify trap generation to send to each specific manager rather than collect first a list of managers {3}
    24.09.2014 Extend value that can be handled by fnAddASN1_identifier_string() and fnExtractVariableBinding() {4}
    25.09.2014 Added community for read and write                        {5}

*/

/* =================================================================== */
/*                           include files                             */
/* =================================================================== */

#include "config.h"

#if defined USE_SNMP

/* =================================================================== */
/*                          local definitions                          */
/* =================================================================== */

#define AVOID_SHIFTS                                                     // pack length fields to 2 bytes to avoid needing to insert additional space and shifting the buffer

#if defined SUPPORT_SNMPV3                                               // determine the highest version that we support
    #define MAX_SNMP_VERSION     SNMP_MANAGER_V3
#elif defined SUPPORT_SNMPV2C
    #define MAX_SNMP_VERSION     SNMP_MANAGER_V2
#else
    #define MAX_SNMP_VERSION     SNMP_MANAGER_V1
#endif


#define OWN_TASK                 TASK_SNMP

#define SNMP_RESEND_PERIOD      (DELAY_LIMIT)(2 * SEC)

#define E_SNMP_RESEND            1

#define SNMP_NUM_RETRIES         5

#define SNMP_HEADER_LENGTH_MAX   12                                      // fixed length header

/* =================================================================== */
/*                      local structure definitions                    */
/* =================================================================== */

typedef struct stUDP_SNMP_MESSAGE
{ 
    unsigned short usDataLength;
    UDP_HEADER     tUDP_Header;                                          // reserve header space
    unsigned char  ucUDP_Message[SNMP_MAX_BUFFER];
} UDP_SNMP_MESSAGE;

typedef struct stTrapList {
    unsigned char ucTrapType;
    unsigned char ucTrapSpecificCode;
}
TRAP_LIST;


/* =================================================================== */
/*                 local function prototype declarations               */
/* =================================================================== */

static int fnFormatFrame(UDP_SNMP_MESSAGE *UDP_Message, unsigned short usLength, unsigned char ucPDU_type);
static int fnSNMPListner(USOCKET SNMP_socket, unsigned char ucEvent, unsigned char *ucIP, unsigned short usPort, unsigned char *ucData, unsigned short usDataLen);
static int fnRetry(USOCKET socket, int iResolved);

/* =================================================================== */
/*                             constants                               */
/* =================================================================== */

static const unsigned char     ucSNMPV1[]  = {ASN1_INTEGER_CODE, 0x01, SNMPV1};
#if defined SUPPORT_SNMPV2C
    static const unsigned char ucSNMPV2C[] = {ASN1_INTEGER_CODE, 0x01, SNMPV2};
#endif
#if defined SUPPORT_SNMPV3
    static const unsigned char ucSNMPV3[]  = {ASN1_INTEGER_CODE, 0x01, SNMPV3};
#endif


/* =================================================================== */
/*                     global variable definitions                     */
/* =================================================================== */


/* =================================================================== */
/*                      local variable definitions                     */
/* =================================================================== */

static USOCKET SNMPSocketNr = -1;                                        // UDP socket
static unsigned char ucSNMPRetries[SNMP_MANAGER_COUNT] = {0};
#if defined SUPPORT_SNMPV2C
    static unsigned char ucSNMPV2_RequestID[SNMP_MANAGER_COUNT] = {0};
#endif
static int (*fnSNMPClientListener)(unsigned char ucEvent, unsigned char *data, unsigned short usLength, int iManagerReference);
static const SNMP_MANAGER *ptrSNMP_manager_details = 0;                  // pointer to a list of SNMP manager IP addresses and details
static TRAP_LIST trap_list[SNMP_MANAGER_COUNT][SNMP_TRAP_QUEUE_LENGTH] = {{{0}}};
static unsigned char ucTrapCnt[SNMP_MANAGER_COUNT] = {0};                // the number of traps waiting in the queue
static const MIB_TABLE *mib_table_start = 0;

// SNMP task
//
extern void fnSNMP(TTASKTABLE *ptrTaskTable)
{
    QUEUE_HANDLE PortIDInternal = ptrTaskTable->TaskID;                  // queue ID for task input
    unsigned char ucInputMessage[SMALL_QUEUE];                           // reserve space for receiving messages

    while (fnRead(PortIDInternal, ucInputMessage, HEADER_LENGTH)) {      // check input queue
        if (ucInputMessage[MSG_SOURCE_TASK] == TASK_ARP) {
            USOCKET socket;
            fnRead(PortIDInternal, ucInputMessage, ucInputMessage[MSG_CONTENT_LENGTH]); // read the contents
            uMemcpy(&socket, &ucInputMessage[1], sizeof(socket));        // extract socket reference
            if (ARP_RESOLUTION_FAILED == ucInputMessage[0]) {
                if (fnRetry(socket, 0) != 0) {                           // SNMP manager could not be resolved - try a few times before informing listener 
                    fnSNMPClientListener(SNMP_ERROR_NO_ARP_RES, 0, 0, 0);
                }
            }
            else if (ARP_RESOLUTION_SUCCESS == ucInputMessage[0]) {
                fnRetry(socket, 1);                                      // the ARP resolution was successful - now start fresh
            }
        }
    }
}



extern int fnStartSNMP(int (*fnAppCallback)(unsigned char ucEvent, unsigned char *data, unsigned short usLength, int iManagerReference), const SNMP_MANAGER *ptrManagerDetails, const MIB_TABLE *ptr_MIB_table)
{
    if (SNMPSocketNr < 0) {                                              // socker doesn't exist
        if (((SNMPSocketNr = fnGetUDP_socket(TOS_MINIMISE_DELAY, fnSNMPListner, (UDP_OPT_SEND_CS | UDP_OPT_CHECK_CS))) >= 0)) {
            fnBindSocket(SNMPSocketNr, SNMP_AGENT_PORT);
        }    
    } 
    fnSNMPClientListener = fnAppCallback;
    ptrSNMP_manager_details = ptrManagerDetails;
    mib_table_start = ptr_MIB_table;
    return SNMPSocketNr;
}

static unsigned short fnAddTimeStamp(unsigned char *ptrBuffer)
{
    unsigned long ulHundredths = uTaskerSystemTick;
    *ptrBuffer++ = ASN1_TIME_STAMP_CODE;
    *ptrBuffer++ = sizeof(unsigned long);                                // content length
    ulHundredths *= TICK_RESOLUTION;
    ulHundredths /= 100;                                                 // up time in 100th of second
    *ptrBuffer++ = (unsigned char)(ulHundredths >> 24);
    *ptrBuffer++ = (unsigned char)(ulHundredths >> 16);
    *ptrBuffer++ = (unsigned char)(ulHundredths >> 8);
    *ptrBuffer   = (unsigned char)(ulHundredths);
    return (sizeof(unsigned long) + 2);
}

// Convert a value so that it respects the rule: 0x80 bit of final byte is 0, and previous bytes have 0x80 bit set
//
extern unsigned char *fnInsertOID_value(unsigned char *ucData, unsigned long ulValue, int *iBufferSpace) // {4}
{
    int iBufferSpaceAvailable;
    if (iBufferSpace == 0) {
        iBufferSpaceAvailable = 256;
    }
    else {
        iBufferSpaceAvailable = *iBufferSpace;
        if (iBufferSpaceAvailable == 0) {
            return 0;
        }
    }
    if (ulValue <= 0x7f) {                                               // simply single digit case
        *ucData++ = (unsigned char)ulValue;
    }
    else {
        if (ulValue & 0x80) {                                            // the final byte must have this bit set to 0
            ulValue = ((ulValue & 0x7f) | ((ulValue << 1) & ~0xff));
        }
        if (ulValue < 0x10000) {                                         // two bytes needed
            ulValue |= 0x8000;                                           // msb must be set
            if (iBufferSpaceAvailable < 2) {
                return 0;
            }
            *ucData++ = (unsigned char)(ulValue >> 8);
            *ucData++ = (unsigned char)ulValue;
        }
        else {
            if ((ulValue & 0x8000) == 0) {                               // this bit must be set in non-final bytes
                ulValue = ((ulValue & 0xffff) | ((ulValue << 1) & ~0xffff) | 0x8000);
            }
            if (ulValue < 0x1000000) {                                   // three bytes needed
                ulValue |= 0x800000;                                     // msb must be set
                if (iBufferSpaceAvailable < 3) {
                    return 0;
                }
                *ucData++ = (unsigned char)(ulValue >> 16);
                *ucData++ = (unsigned char)(ulValue >> 8);
                *ucData++ = (unsigned char)ulValue;
            }
            else {
                if ((ulValue & 0x800000) == 0) {                         // this bit must be set in non-final bytes
                    ulValue = ((ulValue & 0xffffff) | ((ulValue << 1) & ~0xffffff) | 0x800000);
                }
                ulValue |= 0x80000000;                                   // msb must be set
                if (iBufferSpaceAvailable < 4) {
                    return 0;
                }
                *ucData++ = (unsigned char)(ulValue >> 24);              // four bytes needed
                *ucData++ = (unsigned char)(ulValue >> 16);
                *ucData++ = (unsigned char)(ulValue >> 8);
                *ucData++ = (unsigned char)ulValue;
            }
        }
    }
    return ucData;
}

// Extract single node in variable binding binary input
// - 0 returns 1 since both are handled the same by the caller
//
extern unsigned long fnExtractOID_value(unsigned char *ucPtrVariableBinding)
{
    unsigned long ulValue = 0;
    while (*ucPtrVariableBinding & 0x80) {                               // leading bytes
        ulValue |= (*ucPtrVariableBinding++ & ~0x80);
        ulValue <<= 7;
    }
    ulValue |= (*ucPtrVariableBinding);
    if (ulValue == 0) {                                                  // handle zero as 1
        return 1;
    }
    return ulValue;
}

// Covert dot-separated string input to binary representation of a variable binding object
//
static unsigned char fnExtractVariableBinding(CHAR *ptrVariableBinding)
{
    unsigned char *ptrOutput = (unsigned char *)ptrVariableBinding;
    unsigned char *ucDataPtr;                                            // {4}
    unsigned long ulValue;                                               // {4}
    unsigned char ucLength = 0;
    while (*ptrVariableBinding != 0) {
        ulValue = fnDecStrHex(ptrVariableBinding);                       // extract the node value
        ucDataPtr = fnInsertOID_value(ptrOutput, ulValue, 0);            // {4}
        ucLength += (ucDataPtr - ptrOutput);
        ptrOutput = ucDataPtr;
        while (*ptrVariableBinding++ != '.') {
            if (*ptrVariableBinding == 0) {
                break;                                                   // end of input string
            }
        }
    }
    return ucLength;
}


// Extract a length and move over it in the input data
//
static unsigned char *fnGetASN1_length(unsigned char *ucData, unsigned short *usStructureLength, unsigned short *usInputReduceLength)
{
    int iInputReduction = 2;                                             // reduced by type and length field
    unsigned short usLength = *ucData++;
    if (usLength & 0x80) {                                               // is length spread over several bytes?
        usLength &= ~0x80;                                               // the number of bytes
        switch (usLength) {
        case 1:
            iInputReduction++;
            usLength = *ucData++;
            break;
        case 2:
            usLength = *ucData++;
            usLength <<= 8;
            usLength |= *ucData++;
            iInputReduction += 2;
            break;
        default:                                                         // assume structure length of greater than 64k are invalid
            usLength = 0;
            _EXCEPTION("Unexpected!!");
            break;
        }
    }
    *usStructureLength = usLength;
    if (usInputReduceLength != 0) {
        *usInputReduceLength = (*usInputReduceLength - iInputReduction);
    }
    return ucData;
}

static unsigned char *fnGetANS1_value(unsigned char *ptr_ucData, unsigned long *ulValue)
{
    register unsigned long ulResult = 0;
    if (*ptr_ucData == ASN1_INTEGER_CODE) {                              // 0x02
        register unsigned char ucBytes = *(++ptr_ucData);                // length
        if ((ucBytes >= 1) && (ucBytes <= 4)) {                          // we expect 1,2,3 or 4 only for integer values
            ulResult = *(++ptr_ucData);
            while (--ucBytes) {
                ulResult <<= 8;
                ulResult |= *(++ptr_ucData);
            }
            ptr_ucData++;
        }
        else {
            return (ptr_ucData - 1);                                     // unexpected for an integer
        }
    }
    *ulValue = ulResult;
    return ptr_ucData;
}


static void fnExecuteNextHandler(MIB_CONTROL *ptrMIB_control, const MIB_TABLE *ptr_mib_table)
{
    do {
        if (ptr_mib_table->cLevel < 0) {
            ptr_mib_table = mib_table_start;                             // if the end of the table was reached we wrap around to the beginning
        }
        else {
            ptr_mib_table++;                                             // move to following entry (get-next)
        }
#if defined SUPPORT_SNMPV2C
        if (ptrMIB_control->ucBulkLevel >= ptr_mib_table->cLevel) {      // recognise the end of of a bulk block
            return;                                                      // no further objects
        }
#endif
        ptrMIB_control->ucVariableBinding[ptr_mib_table->cLevel] = ptr_mib_table->ucNode; // modify the object identifier accordingly
    } while (ptr_mib_table->snmp_get_set_handler == 0);
    ptrMIB_control->ucVariableBindingLength = (ptr_mib_table->cLevel + 1); // new variable binding identifier length
    ptrMIB_control->ucVariableBinding[ptrMIB_control->ucVariableBindingLength] = 0; // set simple variable appendix
    ptrMIB_control->ucRequestType = ASN1_SNMP_GET_REQUEST;               // execute a get and not a get-next
    ptr_mib_table->snmp_get_set_handler(ptrMIB_control);                 // call the next get handler in the list
    ptrMIB_control->ucVariableBindingLength++;
}

// Get and handle identifier or next identifier
//
static unsigned char fnHandleObjectIdentifier(unsigned char *ucData, unsigned short usLength, MIB_CONTROL *ptrMIB_control)
{
    const MIB_TABLE *ptr_mib_table = mib_table_start;                    // start at the top of the table
   
    if (ucData != 0) {                                                   // if not performing bulk-itteration (SNMPv2c)
        if (*ucData++ != 0x2b) {                                         // 1.3. (iso.org.)
            return SNMP_ERROR_STATUS_NO_SUCH_NAME;
        }

        if (usLength >= (sizeof(ptrMIB_control->ucVariableBinding) - 10)) { // protect against unexpectedly long entries (allowing growth of 10 when extending with table entries [growth of 6 is maximum known])
            return SNMP_ERROR_STATUS_TOO_BIG;
        }
        uMemcpy(ptrMIB_control->ucVariableBinding, ucData, --usLength);  // copy variable to local buffer (1.3.xxxx - 1.3. is assumed and not saved)
        ptrMIB_control->ptrContent = (ucData + usLength);                // data content pointer
        ptrMIB_control->ucVariableBindingLength = (unsigned char)usLength; // original length
    }
    ptrMIB_control->ucObjectType = ASN1_UNKNOWN_CODE;                    // initially unknown
    ptrMIB_control->ucLevel = 0;                                         // start at highest level (after 1.3.)

    while (ptr_mib_table->cLevel >= 0) {                                 // for each level until the end of the table has been found
        if (ptrMIB_control->ucLevel == ptr_mib_table->cLevel) {          // check that the entry matches the level being checked
            if (ptrMIB_control->ucVariableBinding[ptrMIB_control->ucLevel] == ptr_mib_table->ucNode) { // check whether the node at this level matches
                if (ptr_mib_table->snmp_get_set_handler != 0) {          // if this node level has a handler (and is thus accessible and supported in this implementation)
                    if (ptrMIB_control->ucRequestType == ASN1_SNMP_SET_REQUEST) { // check access rights when setting
                        if (ptr_mib_table->ucAccessType & MIB_R_ONLY) {
                            fnSNMPClientListener(SNMP_ERROR_INVALID_WRITE_ATTEMPT, ptrMIB_control->ucVariableBinding, ptrMIB_control->ucVariableBindingLength, 0);
                            return SNMP_ERROR_STATUS_READ_ONLY;          // attempting to set a read-only value
                        }
                        else if (ptrMIB_control->iCommunityAccessRights == SNMP_COMMUNITY_READ_ONLY) {
                            fnSNMPClientListener(SNMP_ERROR_UNPERMITTED_WRITE_ATTEMPT, ptrMIB_control->ucVariableBinding, ptrMIB_control->ucVariableBindingLength, 0);
                            return SNMP_ERROR_STATUS_READ_ONLY;          // attempting to set a read-only value without corresponding community access rights
                        }
                    }
                    if (((ptrMIB_control->ucRequestType == ASN1_SNMP_GET_NEXT_REQUEST) && ((ptr_mib_table->ucAccessType & MIB_TAB) == 0)) || (ptr_mib_table->snmp_get_set_handler(ptrMIB_control) < 0)) { // get variable or next variable from this point
                        fnExecuteNextHandler(ptrMIB_control, ptr_mib_table);
                    }
                    break;
                }
                if (++(ptrMIB_control->ucLevel) >= usLength) {           // if path has been matched before a handler has been found
#if defined SUPPORT_SNMPV2C
                    if (ptrMIB_control->ucRequestType == ASN1_SNMP_GET_BULK_REQUEST) {// get full variable path from this point since a get-bulk is being handled
                        ptrMIB_control->ucBulkLevel = (ptrMIB_control->ucLevel - 1); // remember the next lower level belonging to the get-bulk (allowing the first of the block to be accepted)
                        fnExecuteNextHandler(ptrMIB_control, ptr_mib_table);
                        ptrMIB_control->ucBulkLevel++;                   // remember the level belonging to the get-bulk
                        break;
                    }
                    else if (ptrMIB_control->ucRequestType == ASN1_SNMP_GET_NEXT_REQUEST) {// get full variable path from this point since a get-next is being handled
                        fnExecuteNextHandler(ptrMIB_control, ptr_mib_table);
                        break;
                    }
#else
                    if (ptrMIB_control->ucRequestType == ASN1_SNMP_GET_NEXT_REQUEST) { // get full variable path from this point since a get-next is being handled
                        fnExecuteNextHandler(ptrMIB_control, ptr_mib_table);
                        break;
                    }
#endif
                }
            }
        }
        else {
            if (ptrMIB_control->ucLevel > ptr_mib_table->cLevel) {
                break;                                                   // no match
            }
        }
        ptr_mib_table++;                                                 // move to following mib entry in the table
    }

    if (ASN1_UNKNOWN_CODE == ptrMIB_control->ucObjectType) {             // if no object could be found
#if defined SUPPORT_SNMPV2C
        if (ptrMIB_control->ucSNMP_version > SNMPV1) {                   // if using SNMPv2c
            ptrMIB_control->ucObjectType = SNMP_OBJECT_ERROR_NO_SUCH_OBJECT; // change error to noSuchObject
            return SNMP_ERROR_STATUS_NO_ERROR;                           // no error
        }
#endif
        return SNMP_ERROR_STATUS_NO_SUCH_NAME;                           // return error
    }
    return SNMP_ERROR_STATUS_NO_ERROR;                                   // no error
}


// Format a frame based buffer to ensure that field lengths are all correct
//
static int fnFormatFrame(UDP_SNMP_MESSAGE *UDP_Message, unsigned short usLength, unsigned char ucPDU_type)
{
    unsigned char *ucData = UDP_Message->ucUDP_Message;                  // the start of SNMP UDP data content
    unsigned short usFrameLength = usLength;                             // original length
    unsigned short usObjectLength;
    unsigned char ucType;
    while (usLength--) {                                                 // for the frame content
        ucType = *ucData++;
        if ((ucType == ASN1_SEQUENCE) || (ucType == ucPDU_type)) {
            usLength--;
            if (*ucData & 0x80) {                                        // two byte length - we maintain this format (all such length fields are prepared with this format to avoid buffer shifts and other buffer checking complications)
                ucData++;
                usLength -= 2;
                *ucData++ = (unsigned char)(usLength >> 8);
                *ucData++ = (unsigned char)(usLength);
            }
            else {                                                       // single byte length (we expect the value to have already been inserted at this location)
#if !defined AVOID_SHIFTS
                if (*ucData == 0) {
                    // This code is avoided in practice since length fields are prepared to use a two byte format to avoid the buffer needing to be shifted when sizes require more space
                    // - as well as avoiding buffer shifts it simplified checking that buffer size is adequate when entering the data since the overall space requirement of previous objects can't increase as further objects are added
                    //
                    if (usLength > 0x7f) {                               // if the length is larger that 127 it needs to use two byte length field
                        usFrameLength += 2;                              // new frame length after insertion
                        if (usFrameLength > SNMP_MAX_BUFFER) {           // the buffer doesn't have adequate space to grow
                            _EXCEPTION("Rework to allow single objects to be removed");
                            return -1;
                        }
                        *ucData++ = 0x82;                                // length as two bytes
                        uReverseMemcpy((ucData + 2), ucData, usLength);  // shift contents
                        *ucData++ = 0;                                   // add two byte length (will be filled out in the re-itteration
                        *ucData++ = 0;
                        return (fnFormatFrame(UDP_Message, usFrameLength, ucPDU_type)); // re-itterate with new frame and increased length
                    }
                    else {
                        *ucData = (unsigned char)usLength;               // maintain format and insert the length
                    }
                }
#endif
                ucData++;
            }
        }
        else {
            fnGetASN1_length(ucData, &usObjectLength, 0);
            if (usObjectLength > 0x7f) {
                ucData += (3 + usObjectLength);
                usLength -= 3;
            }
            else {
                ucData += (1 + usObjectLength);
                usLength--;
            }
            if (usObjectLength > usLength) {
                _EXCEPTION("Formatting Error!!!");
                return -1;                                               // serious formatting error; quit to avoid fatal consequences.
            }
            usLength -= usObjectLength;                                  // length remaining
        }
    }
    return usFrameLength;                                                // new frame length
}


// Insert an integer value into a buffer - the resulting value can occupy from 3 bytes to 6 bytes space
//
static unsigned char *fnAddASN1_integer(unsigned long ulInteger, unsigned char *ptrBuffer, unsigned char ucANS1_type, int *iBufferSpace)
{
    *ptrBuffer++ = ucANS1_type;
    if (ulInteger < 0x100) {                                             // if the integer can fit in a single byte
        *ptrBuffer++ = 0x01;                                             // length is 1 byte
        *ptrBuffer++ = (unsigned char)ulInteger;                         // add the value
    }
    else  if (ulInteger < 0x10000) {                                     // if the integer can fit in to two bytes
        *ptrBuffer++ = 0x02;                                             // length is 2 bytes
        *ptrBuffer++ = (unsigned char)(ulInteger >> 8);                  // add the value
        *ptrBuffer++ = (unsigned char)(ulInteger);                       // add the value
    }
    else  if (ulInteger < 0x1000000) {                                   // if the integer can fit in to three bytes
        *ptrBuffer++ = 0x03;                                             // length is 3 bytes
        *ptrBuffer++ = (unsigned char)(ulInteger >> 16);                 // add the value
        *ptrBuffer++ = (unsigned char)(ulInteger >> 8);                  // add the value
        *ptrBuffer++ = (unsigned char)(ulInteger);                       // add the value
    }
    else {
        *ptrBuffer++ = 0x04;                                             // length is 4 bytes
        *ptrBuffer++ = (unsigned char)(ulInteger >> 24);                 // add the value
        *ptrBuffer++ = (unsigned char)(ulInteger >> 16);                 // add the value
        *ptrBuffer++ = (unsigned char)(ulInteger >> 8);                  // add the value
        *ptrBuffer++ = (unsigned char)(ulInteger);                       // add the value
    }
    return ptrBuffer;
}

static void fnAddGetSetHeader(MIB_CONTROL *ptrMIB_control, UDP_SNMP_MESSAGE *udp_frame, unsigned short usOriginalDataLength)
{
    register unsigned char *ptrBuffer = &udp_frame->ucUDP_Message[usOriginalDataLength];;

    // The header is generated in a manner to be of fixed length so that it can be inserted before the content without needing to shift into position
    // - 12 bytes (SNMP_HEADER_LENGTH_MAX)
    //
    *ptrBuffer++ = ASN1_INTEGER_CODE;                                    // request id can occupy a fixed 4 bype integer
    *ptrBuffer++ = 4;
    *ptrBuffer++ = (unsigned char)(ptrMIB_control->ulRequestID >> 24);
    *ptrBuffer++ = (unsigned char)(ptrMIB_control->ulRequestID >> 16);
    *ptrBuffer++ = (unsigned char)(ptrMIB_control->ulRequestID >> 8);
    *ptrBuffer++ = (unsigned char)(ptrMIB_control->ulRequestID);

    *ptrBuffer++ = ASN1_INTEGER_CODE;                                    // the error status can have a value between 0..5 and so can be a fixed since byte length
    *ptrBuffer++ = 1;
    *ptrBuffer++ = (unsigned char)(ptrMIB_control->ulErrorStatus);

    *ptrBuffer++ = ASN1_INTEGER_CODE;                                    // the error index will never be greater than one byte so can be a fixed sincle byte length
    *ptrBuffer++ = 1;
    *ptrBuffer++ = (unsigned char)(ptrMIB_control->ulErrorIndex);
}

extern CHAR *fnCopyASN1_string(CHAR *ptrBuffer, unsigned char *ptrData, unsigned short usBufferLength)
{
    unsigned short usLength;
    if (*ptrData++ != ASN1_OCTET_STRING_CODE) {
        return 0;                                                        // input is not an ASN1 string type
    }
    ptrData = fnGetASN1_length(ptrData, &usLength, 0);
    if (usLength >= usBufferLength) {
        usLength = (usBufferLength - 1);                                 // limit to buffer length
    }
    uMemcpy(ptrBuffer, ptrData, usLength);
    ptrBuffer += usLength;
    *ptrBuffer = 0;                                                      // zero terminate the string
    return ptrBuffer;                                                    // return a pointer to the end of the string
}


// Insert a null terminated string
//
static unsigned char *fnAddASN1_string(const CHAR *cPtrString, unsigned char *ucData, int *iBufferSpace)
{
    unsigned short usStringLength;
    if (0 == cPtrString) {
        usStringLength = 0;
    }
    else {
        usStringLength = uStrlen(cPtrString);
    }

    if (iBufferSpace != 0) {
        int iCheckLength = (usStringLength + 2);
        if (usStringLength > 0x7f) {
            iCheckLength += 2;
        }
        if (iCheckLength > *iBufferSpace) {
            return 0;                                                    // space is not adequate to add the string
        }
        *iBufferSpace -= iCheckLength;
    }

    *ucData++ = ASN1_OCTET_STRING_CODE;                                  // 0x04
    if (usStringLength <= 0x7f) {
        *ucData++ = (unsigned char)usStringLength;
    }
    else {
        *ucData++ = 0x82;                                                // length represented by 2 bytes
        *ucData++ = (unsigned char)(usStringLength >> 8);
        *ucData++ = (unsigned char)(usStringLength);
    }
    if (usStringLength != 0) {
        uMemcpy(ucData, (unsigned char *)cPtrString, usStringLength);
    }
    return (ucData + usStringLength);
}


static unsigned char *fnAddASN1_identifier_string(const CHAR *cPtrString, unsigned char *ucData, int *iBufferSpace)
{
    unsigned char *ptrStart;
    unsigned char *ptrLength;
    unsigned long ulValue = 0;                                           // {4}

    if (iBufferSpace != 0) {
        if (*iBufferSpace < 2) {                                         // check adequate space in buffer
            return 0;
        }
        *iBufferSpace -= 2;
    }
    *ucData++ = ASN1_OBJECT_IDENT_CODE;
    ptrLength = ucData;                                                  // remember the location of the length to be inserted (only 1 byte will be required)
    ptrStart = ++ucData;                                                 // start of the object identifier
    *ucData++ = 0x2b;                                                    // 1.3. (iso.org.)

    while (1) {                                                          // the object identifier string assume leading 1.3. (which is not present)
        if (*cPtrString < '0') {                                         // 0..9 and separating dots are assumed < '0' assumes a dot has been encountered
            ucData = fnInsertOID_value(ucData, ulValue, iBufferSpace);   // {4}
            if (ucData == 0) {
                return 0;                                                // no buffer space to add
            }
            if (*cPtrString == 0) {
                break;
            }
            cPtrString++;
        }
        else {
            ulValue = fnDecStrHex((CHAR *)cPtrString);                   // decimal input assumed (terminated with '.' or null terminator)
            while (*cPtrString >= '0') {
                cPtrString++;                                            // move over the decimal value
            }
        }
    }
    *ptrLength = (ucData - ptrStart);                                    // insert length
    return ucData;
}

// Insert buffer content
//
static unsigned char *fnAddASN1_buffer(const CHAR *cPtrString, unsigned char ucLength, unsigned char *ucData, unsigned char ucType, int *iBufferSpace)
{
    if (iBufferSpace != 0) {
        if ((ucLength + 2) > *iBufferSpace) {
            return 0;                                                    // space is not adequate to add the octet string
        }
        *iBufferSpace -= (ucLength + 2);
    }
    *ucData++ = ucType;
    *ucData++ = ucLength;
    uMemcpy(ucData, cPtrString, ucLength);
    return (ucData + ucLength);
}


// Add an object identifier to the end of the UDP data buffer
//
static int fnAddVariableBinding(MIB_CONTROL *ptrMIB_control, UDP_SNMP_MESSAGE *udp_frame)
{
    register int iOriginalLength = udp_frame->usDataLength;
    int iBufferSpace = (SNMP_MAX_BUFFER - SNMP_HEADER_LENGTH_MAX - iOriginalLength); // initial buffer space available (using fixed length header size)
    unsigned char *ptrBuffer = &udp_frame->ucUDP_Message[iOriginalLength]; // start location in the UDP data buffer
    unsigned char *ptrBufferStart = ptrBuffer;
    unsigned char *ptrLength;

    if (ptrMIB_control->iObjectsCount++ == 0) {
        if ((ptrMIB_control->ucVariableBindingLength + 9) > iBufferSpace) { // check whether the object identifer can fit
            return -1;                                                   // not adequate space
        }
        else {
            iBufferSpace -= (ptrMIB_control->ucVariableBindingLength + 9);
        }
        *ptrBuffer++ = ASN1_SEQUENCE;                                    // 0x30
#if defined AVOID_SHIFTS
        *ptrBuffer = 0x82;                                               // dummy length (will be filled out when formatted)
        ptrBuffer += 3;
#else
        *ptrBuffer++ = 0x00;                                             // dummy length (will be filled out when formatted)
#endif
    }
    else if ((ptrMIB_control->ucVariableBindingLength + 5) > iBufferSpace) { // check whether the object identifier can fit
        return -1;                                                       // not adequate space
    }
    else {
        iBufferSpace -= (ptrMIB_control->ucVariableBindingLength + 5);
    }
    *ptrBuffer++ = ASN1_SEQUENCE;                                        // 0x30
    ptrLength = ptrBuffer++;                                             // the location where the entry length will be put (assumed to never be greater than 0x7f)
    *ptrBuffer++ = ASN1_OBJECT_IDENT_CODE;
    *ptrBuffer++ = (ptrMIB_control->ucVariableBindingLength + 1);        // length of the variable binding (assumed to never be greater than 0xff)
    *ptrBuffer++ = 0x2b;                                                 // iso.3.
    uMemcpy(ptrBuffer, ptrMIB_control->ucVariableBinding, ptrMIB_control->ucVariableBindingLength);
    ptrBuffer += ptrMIB_control->ucVariableBindingLength;                // the buffer is known to have adequate space up to this point
    switch (ptrMIB_control->ucObjectType) {
    case ASN1_GAUGE_CODE:                                                // gauge plus integer
        ptrBuffer = fnAddASN1_integer(ptrMIB_control->ulInteger, ptrBuffer, ASN1_GAUGE_CODE, &iBufferSpace);
        break;
    case ASN1_TIME_STAMP_CODE:                                           // 0x43 time ticks plus integer
        if (iBufferSpace == 0) {
            return -1;                                                   // no more buffer space
        }
      //*ptrBuffer++ = ASN1_TIME_STAMP_CODE;
        ptrBuffer = fnAddASN1_integer(ptrMIB_control->ulInteger, ptrBuffer, ASN1_TIME_STAMP_CODE, &iBufferSpace); // {2}
        // Fall through intentional to add integer
        //
        break;
    case ASN1_INTEGER_CODE:                                              // 0x02
        ptrBuffer = fnAddASN1_integer(ptrMIB_control->ulInteger, ptrBuffer, ASN1_INTEGER_CODE, &iBufferSpace);
        break;
    case ASN1_PHY_ADDRESS_CODE:                                          // pseudo type inserting an octet string (0x04)
        ptrBuffer = fnAddASN1_buffer(ptrMIB_control->ptrString, ptrMIB_control->ucObjectLength, ptrBuffer, ASN1_OCTET_STRING_CODE, &iBufferSpace);
        break;
    case ASN1_IP_ADDRESS_CODE:                                           // 0x40
        ptrBuffer = fnAddASN1_buffer(ptrMIB_control->ptrString, ptrMIB_control->ucObjectLength, ptrBuffer, ASN1_IP_ADDRESS_CODE, &iBufferSpace);
        break;
    case ASN1_OCTET_STRING_CODE:                                         // 0x04
        ptrBuffer = fnAddASN1_string(ptrMIB_control->ptrString, ptrBuffer, &iBufferSpace); // insert a null terminated string
        break;
    case ASN1_OBJECT_IDENT_CODE:                                         // 0x06
        ptrBuffer = fnAddASN1_identifier_string(ptrMIB_control->ptrString, ptrBuffer, &iBufferSpace); // insert an object identifier
        break;
#if defined SUPPORT_SNMPV2C                                              // SNMPv2 errors
    case SNMP_OBJECT_ERROR_END_OF_MIB_VIEW:
    case SNMP_OBJECT_ERROR_NO_SUCH_INSTANCE:
    case SNMP_OBJECT_ERROR_NO_SUCH_OBJECT:
        if (iBufferSpace < 2) {
            return -1;                                                   // no more buffer space
        }
        *ptrBuffer++ = ptrMIB_control->ucObjectType;
        *ptrBuffer++ = 0;
        break;
#endif
    }
    if (ptrBuffer == 0) {                                                // an object couldn't be added to the buffer due to lack of buffer space
        return -1;                                                       // no more buffer space
    }
    *ptrLength = (ptrBuffer - ptrLength - 1);
    udp_frame->usDataLength += (ptrBuffer - ptrBufferStart);             // new length
    return 0;                                                            // successful
}

// Handle a SET-REQUEST, GET-REQUEST, GET-NEXT-REQUEST (or GET_BULK_REQUEST SNMPV2)
//
static int fnHandleRequest(unsigned char *ucData, UDP_SNMP_MESSAGE *udp_frame, MIB_CONTROL *ptr_mib_control, unsigned short usStructureLength)
{
    unsigned char *ptrNextData;                                          // {1}
    unsigned char *ptrDataIn = ucData;
    unsigned short usOriginalDataLength = udp_frame->usDataLength;
    unsigned short usSubSequenceLength;
    unsigned char ucError = 0;
    unsigned char ucType = ptr_mib_control->ucRequestType;               // the original request type

    udp_frame->usDataLength += SNMP_HEADER_LENGTH_MAX;                   // leave space for fixed length 12 byte header

    ucData = fnGetANS1_value(ucData, &ptr_mib_control->ulRequestID);     // extract ID, error status and error index which preceeds the variable bindings
    ucData = fnGetANS1_value(ucData, &ptr_mib_control->ulErrorStatus);   // in the case of V2c get-bulk this is the non-repeaters field
#if defined SUPPORT_SNMPV2C
    if (ucType == ASN1_SNMP_GET_BULK_REQUEST) {
        ucData = fnGetANS1_value(ucData, &ptr_mib_control->ulMaxRepetitions); // in the case of V2c get-bulk this is the max-repetitions field
    }
    else {
        ptr_mib_control->ulMaxRepetitions = 0;
        ucData = fnGetANS1_value(ucData, &ptr_mib_control->ulErrorIndex);
    }
#else
    ucData = fnGetANS1_value(ucData, &ptr_mib_control->ulErrorIndex);
#endif

    // We now expect a sub-sequence
    //
    if (*ucData++ != ASN1_SEQUENCE) {                                    // 0x30
        return -1 ;
    }
    ucData = fnGetASN1_length(ucData, &usSubSequenceLength, 0);          // get the length and move over it

    usStructureLength -= (ucData - ptrDataIn);                           // calculate the structure length
    if (usSubSequenceLength != usStructureLength) {                      // check that the structure sizes make sense
        return - 1;                                                      // ignore if invalid
    }
#if defined SUPPORT_SNMPV2C
    ptr_mib_control->ucBulkLevel = 0;
#endif
    ptr_mib_control->ulErrorIndex = 0;                                   // default to no error index
    ptr_mib_control->iObjectsCount = 0;                                  // no object identifiers added yet
    while (usStructureLength != 0) {
        // We now expect a number of possible sub-sequence (items)
        //
        switch (*ucData++) {
        case ASN1_SEQUENCE:                                              // 0x30
            ucData = fnGetASN1_length(ucData, &usSubSequenceLength, &usStructureLength); // get length of sequence and move over it
            switch (*ucData++) {
            case ASN1_OBJECT_IDENT_CODE:                                 // 0x06
                ptrNextData = (ucData + usSubSequenceLength - 1);        // {1} prepare the pointer to the next entry based on the reported item size
                usStructureLength -= usSubSequenceLength;
                ucData = fnGetASN1_length(ucData, &usSubSequenceLength, 0); // get length of sequence and move over it
                ucError = fnHandleObjectIdentifier(ucData, usSubSequenceLength, ptr_mib_control);
                switch (ucError) {
                case SNMP_ERROR_STATUS_BAD_VALUE:
                case SNMP_ERROR_STATUS_NO_SUCH_NAME:
              //case SNMP_ERROR_STATUS_READ_ONLY:
                    ptr_mib_control->ulErrorIndex = 0;                   // set the error index only for these types of error
                case SNMP_ERROR_STATUS_TOO_BIG:
                case SNMP_ERROR_STATUS_GENERAL_ERROR:
                    ptr_mib_control->ulErrorStatus = ucError;            // prepare error status
                    break;
                case SNMP_ERROR_STATUS_NO_ERROR:
                    if (fnAddVariableBinding(ptr_mib_control, udp_frame) < 0) { // add the variable binding to the response
                        ptr_mib_control->ulErrorStatus = SNMP_ERROR_STATUS_TOO_BIG; // response cannot be added due to lack of buffer size
                        break;
                    }
#if defined SUPPORT_SNMPV2C
                    // If this is a bulk request we pack in as many variable binding objects as required/possible
                    //
                    if (ptr_mib_control->ulMaxRepetitions > 1) {         // as long as the max-repetitions field is set to more than 1 (non-get-bulk will be zero)
                        ptr_mib_control->ucRequestType = ASN1_SNMP_GET_NEXT_REQUEST; // handle itterative calls as get-next types
                        while (fnHandleObjectIdentifier(0, ptr_mib_control->ucVariableBindingLength, ptr_mib_control) == SNMP_ERROR_STATUS_NO_ERROR) {
                            if (fnAddVariableBinding(ptr_mib_control, udp_frame) != 0) {
                                break;                                   // no more possible due to buffer space limits
                            }
                            if (--ptr_mib_control->ulMaxRepetitions < 2) { // stop if the meximum number of requested repetitions has been reached
                                break;
                            }
                            ptr_mib_control->ucRequestType = ASN1_SNMP_GET_NEXT_REQUEST; // handle itterative calls as get-next types
                        }
                    }
#endif
                    break;
                }
              //ucData += usSubSequenceLength;
                ucData = ptrNextData;                                    // {1} set known next entry start  in case the declared length was including following ANS1_NULL
                if (ucType == ASN1_SNMP_SET_REQUEST) {                   // a get requests needs to jump also the data
                    goto _get_terminate;                                 // there is only one object in a set so we can quit
                }
                break;
            case ASN1_NULL_CODE:                                         // 0x05
                ucData++;
                usStructureLength -= 2;
                break;
            default:
                return -1;                                               // invalid content
            }
            break;
        case ASN1_NULL_CODE:                                             // 0x05
            ucData++;
            usStructureLength -= 2;
            break;
        default:
            return -1;                                                   // invalid content
        }
    }
_get_terminate:
    fnAddGetSetHeader(ptr_mib_control, udp_frame, usOriginalDataLength); // insert a get/set header
    return 0;                                                            // OK
}


// Create and send a trap in the specified version's format (specific code is only relevant to V1 traps)
//
static int fnSendTrap(unsigned char ucTrap, unsigned char ucSpecificCode, int iManagerRef, unsigned char ucTrapVersion)
{
    MIB_CONTROL mib_control;
    UDP_SNMP_MESSAGE UDP_Message;
    int iNewLength;
    unsigned char ucVariableBindingRef;
    unsigned short usLength;
    unsigned char ucPDU;
    CHAR cVariableBinding[64];                                
    unsigned char ucVariableBindingLength;

    cVariableBinding[0] = 0x2b;                                          // 1.3. (iso.org.)

    UDP_Message.ucUDP_Message[0] = ASN1_SEQUENCE;                        // 0x30
#if defined AVOID_SHIFTS
    UDP_Message.ucUDP_Message[1] = 0x82;                                 // fixed length field with two places
    UDP_Message.ucUDP_Message[4] = ASN1_INTEGER_CODE;                    // 0x02
    UDP_Message.ucUDP_Message[5] = 0x01;
    UDP_Message.ucUDP_Message[6] = ucTrapVersion;                        // length is 7 bytes to this point
    usLength = 7;
#else
    UDP_Message.ucUDP_Message[1] = 0x00;                                 // fixed length field with two places
    UDP_Message.ucUDP_Message[2] = ASN1_INTEGER_CODE;                    // 0x02
    UDP_Message.ucUDP_Message[3] = 0x01;
    UDP_Message.ucUDP_Message[4] = ucTrapVersion;                        // length is 5 bytes to this point
    usLength = 5;
#endif
    UDP_Message.ucUDP_Message[usLength++] = ASN1_OCTET_STRING_CODE;      // insert the community, which is always a string (0x04)
    UDP_Message.ucUDP_Message[usLength] = fnSNMPClientListener(SNMP_GET_COMMUNITY, &UDP_Message.ucUDP_Message[usLength + 1], 0, iManagerRef); // request the community being used by this manager to be inserted
    usLength += (UDP_Message.ucUDP_Message[usLength] + 1);               // new length
    switch (ucTrapVersion) {
    case SNMPV1:
        UDP_Message.ucUDP_Message[usLength++] = ucPDU = ASN1_SNMP_TRAP;  // version 1 trap
#if defined AVOID_SHIFTS
        UDP_Message.ucUDP_Message[usLength] = 0x82;                      // place holder for length to be added
        usLength += 3;
#else
        UDP_Message.ucUDP_Message[usLength++] = 0;                       // place holder for length to be added
#endif
        UDP_Message.ucUDP_Message[usLength++] = ASN1_OBJECT_IDENT_CODE;  // 0x06
        UDP_Message.ucUDP_Message[usLength + 1] = 0x2b;                  // 1.3. (iso.org.)
        UDP_Message.ucUDP_Message[usLength] = (fnSNMPClientListener(SNMP_GET_ENTERPRISE, &UDP_Message.ucUDP_Message[usLength + 2], 0, iManagerRef) + 1);
        usLength += (UDP_Message.ucUDP_Message[usLength] + 1);           // new length
        UDP_Message.ucUDP_Message[usLength++] = ASN1_IP_ADDRESS_CODE;    // 0x40
        UDP_Message.ucUDP_Message[usLength++] = IPV4_LENGTH;    
        uMemcpy(&UDP_Message.ucUDP_Message[usLength], network[DEFAULT_NETWORK].ucOurIP, IPV4_LENGTH);
        usLength += IPV4_LENGTH;
        UDP_Message.ucUDP_Message[usLength++] = ASN1_INTEGER_CODE;       // 0x02
        UDP_Message.ucUDP_Message[usLength++] = 1;                       // one byte
        UDP_Message.ucUDP_Message[usLength++] = ucTrap;                  // add the trap type
        UDP_Message.ucUDP_Message[usLength++] = ASN1_INTEGER_CODE;       // 0x02
        UDP_Message.ucUDP_Message[usLength++] = 1;                       // one byte
        UDP_Message.ucUDP_Message[usLength++] = ucSpecificCode;          // add specific trap code
        usLength += fnAddTimeStamp(&UDP_Message.ucUDP_Message[usLength]);// add time stamp (100ms units since the device started)
        break;
#if defined SUPPORT_SNMPV2C
    case SNMPV2:
        UDP_Message.ucUDP_Message[usLength++] = ucPDU = ASN1_SNMP_TRAP2; // version 2c trap 0xa7

#if defined AVOID_SHIFTS
        UDP_Message.ucUDP_Message[usLength] = 0x82;                      // place holder for length to be added
        usLength += 3;
#else
        UDP_Message.ucUDP_Message[usLength++] = 0;                       // place holder for length to be added
#endif
        UDP_Message.ucUDP_Message[usLength++] = ASN1_INTEGER_CODE;       // 0x02
        UDP_Message.ucUDP_Message[usLength++] = 1;                       // one integer byte
        if (ucSNMPV2_RequestID[iManagerRef] == 0) {                      // avoid ID of 0 (1..255)
            ucSNMPV2_RequestID[iManagerRef] = 1;
        }
        UDP_Message.ucUDP_Message[usLength++] = ucSNMPV2_RequestID[iManagerRef]; // request ID
        UDP_Message.ucUDP_Message[usLength++] = ASN1_INTEGER_CODE;       // 0x02
        UDP_Message.ucUDP_Message[usLength++] = 1;                       // one integer byte
        UDP_Message.ucUDP_Message[usLength++] = fnSNMPClientListener(SNMP_GET_ERROR_STATUS, 0, 0, iManagerRef); // error status
        UDP_Message.ucUDP_Message[usLength++] = ASN1_INTEGER_CODE;       // 0x02
        UDP_Message.ucUDP_Message[usLength++] = 1;                       // one integer byte
        UDP_Message.ucUDP_Message[usLength++] = fnSNMPClientListener(SNMP_GET_ERROR_INDEX, 0, 0, iManagerRef); // error index
        break;
#endif
    default:                                                             // non-supported snmp version
        return SNMP_TRAP_TX_NOT_SUPPORTED_VERSION;
    }
    UDP_Message.ucUDP_Message[usLength++] = ASN1_SEQUENCE;               // 0x30
#if defined AVOID_SHIFTS
    UDP_Message.ucUDP_Message[usLength] = 0x82;                          // place holder for length to be added
    UDP_Message.usDataLength = (usLength + 3);
#else
    UDP_Message.ucUDP_Message[usLength] = 0x00;                          // place holder for length to be added
    UDP_Message.usDataLength = (usLength + 1);
#endif

    mib_control.ucRequestType = ASN1_SNMP_GET_REQUEST;                   // handle as a get
    mib_control.iObjectsCount = 1;                                       // avoid adding the initial ASN1_SEQUENCE
#if defined SUPPORT_SNMPV2C
    mib_control.ucTrapReference = ucTrap;
#endif

    ucVariableBindingRef = 0;

    while (1) {
#if defined SUPPORT_SNMPV2C
        if (ucTrapVersion == SNMPV2) {                                   // always return variable bindings sysUpTime and snmpTrapOID in SNMPv2-Trap-PDU
            if (ucVariableBindingRef == 0) {
                uStrcpy(&cVariableBinding[1], "6.1.2.1.1.3.0");          // [sysUpTime]
            }
            else if (ucVariableBindingRef == 1) {
                uStrcpy(&cVariableBinding[1], "6.1.6.3.1.1.4.1.0");      // [snmpTrapOID] 
            }
            else if (fnSNMPClientListener(SNMP_GET_VARIABLE_BINDING, (unsigned char *)&cVariableBinding[1], (unsigned short)(ucTrap | ((ucVariableBindingRef - 2) << 8)), iManagerRef) == 0) { // allow the user to add variable bindings if desired
                break;
            }
        }
        else {
#endif
            if (fnSNMPClientListener(SNMP_GET_VARIABLE_BINDING, (unsigned char *)&cVariableBinding[1], (unsigned short)(ucTrap | (ucVariableBindingRef << 8)), iManagerRef) == 0) { // allow the user to add variable bindings if desired
                break;
            }
#if defined SUPPORT_SNMPV2C
        }
#endif
        ucVariableBindingLength = fnExtractVariableBinding(&cVariableBinding[1]); // convert the variable binding from string input to binary representation and determine its length
        if (ucVariableBindingLength != 0) {                              // ignore if invalid variable binding
            if (fnHandleObjectIdentifier((unsigned char *)cVariableBinding, (unsigned char)(ucVariableBindingLength + 1), &mib_control) == SNMP_ERROR_STATUS_NO_ERROR) { // ignore if the variable binding reference can't be handled
                if (fnAddVariableBinding(&mib_control, &UDP_Message) < 0) { // add the variable binding to the response
                    break;                                               // response cannot be added due to lack of buffer size
                }
            }
        }
        ucVariableBindingRef++;                                          // move to next variable binding
    }

    if ((iNewLength = fnFormatFrame(&UDP_Message, UDP_Message.usDataLength, ucPDU)) <= 0) {
        _EXCEPTION("Formatting error detected!!!");
        return SNMP_TRAP_TX_FORMATTING_ERROR;
    }
    if (fnSendUDP((USOCKET)(SNMPSocketNr | ptrSNMP_manager_details[iManagerRef].snmp_manager_details | ((iManagerRef & USER_INFO_MASK) << USER_INFO_SHIFT)), (unsigned char *)ptrSNMP_manager_details[iManagerRef].snmp_manager_ip_address, SNMP_MANAGER_PORT, (unsigned char *)&UDP_Message.tUDP_Header, (unsigned short)iNewLength, OWN_TASK) == NO_ARP_ENTRY) {
        return SNMP_TRAP_TX_RESOLVING;                                   // not sent because the destination first needs to be resolved
    }
#if defined SUPPORT_SNMPV2C
    if (ucTrapVersion == SNMPV2) {
        ucSNMPV2_RequestID[iManagerRef]++;                               // since a v2c trap was successfully sent we increment the request ID
    }
#endif
    return 0;                                                            // successful
}

// The SNMP Agent listener function
//
static int fnSNMPListner(USOCKET SNMP_socket, unsigned char ucEvent, unsigned char *ucIP, unsigned short usPort, unsigned char *ucData, unsigned short usDataLen)
{
    if (ucEvent == UDP_EVENT_RXDATA) {
        UDP_SNMP_MESSAGE UDP_Message;                                    // temporary buffer for constructing a response in
        MIB_CONTROL mib_control;
        CHAR *cCommunity;
        unsigned char *frame_data;
        unsigned long ulVersion;
        unsigned short usCommunityLength;
        unsigned short usStructureLength;

        if (*ucData++ != ASN1_SEQUENCE) {                                // 0x30
            return 0;                                                    // not ANS.1 BER conform - silently ignore
        }
        ucData = fnGetASN1_length(ucData, &usStructureLength, 0);        // extract the length of the structure and move over it
        if ((usStructureLength == 0) || (ASN1_INTEGER_CODE != *ucData)) {// 0x02
            return 0;                                                    // silently ignore invalid structure length or invalid version type
        }
        frame_data = ucData;
        ucData = fnGetANS1_value(ucData, &ulVersion);                    // get the SNMP version number
        mib_control.ucSNMP_version = (unsigned char)ulVersion;
        switch (ulVersion) {
        case SNMPV3:                                                     // 0x02
#if defined SUPPORT_SNMPV3
            break;                                                       // accepted
#else
            fnSNMPClientListener(SNMP_ERROR_V3_NOT_SUPPORTED, ucIP, 0, 0);
#endif
            return 0;                                                    // silently ignore
        case SNMPV2:                                                     // 0x01
#if defined SUPPORT_SNMPV2C
            break;                                                       // accepted
#else
            fnSNMPClientListener(SNMP_ERROR_V2_NOT_SUPPORTED, ucIP, 0, 0);
            return 0;                                                    // silently ignore
#endif
        case SNMPV1:                                                     // 0x00
            break;                                                       // accept

        default:
            return 0;                                                    // silently ignore bad SNMP versions
        }
        if (*ucData++ != ASN1_OCTET_STRING_CODE) {                       // community string (0x04) is expected 
            return 0;                                                    // silently ignore if string type is not found
        }
        ucData = fnGetASN1_length(ucData, &usCommunityLength, 0);        // extract the length and move over it
        cCommunity = (CHAR *)ucData;                                     // set pointer to string (often "public") in the input data buffer
        ucData += usCommunityLength;                                     // jump the community string
        *UDP_Message.ucUDP_Message = ASN1_SEQUENCE;
#if defined AVOID_SHIFTS
        *(UDP_Message.ucUDP_Message + 1) = 0x82;                         // prepare fixed 2 byte length with space (this avoids needing to shift the buffer content later whatever its size)
        UDP_Message.usDataLength = (ucData - frame_data);                // the start of the response includes the same data as the reception has
        uMemcpy((UDP_Message.ucUDP_Message + 4), frame_data, UDP_Message.usDataLength); // copy the start of the frame, including version and community
        UDP_Message.usDataLength += 4;
#else
        *(UDP_Message.ucUDP_Message + 1) = 0x00;                         // space holder for length
        UDP_Message.usDataLength = (ucData - frame_data);                // the start of the response includes the same data as the reception has
        uMemcpy((UDP_Message.ucUDP_Message + 2), frame_data, UDP_Message.usDataLength); // copy the start of the frame, including version and community
        UDP_Message.usDataLength += 2;
#endif
        switch (mib_control.ucRequestType = *ucData++) {                 // decision on the PDU type
        case ASN1_SNMP_SET_REQUEST:                                      // 0xa3 request to set a value
            if ((mib_control.iCommunityAccessRights = fnSNMPClientListener(SNMP_COMMUNITY_CHECK_WRITE, (unsigned char *)cCommunity, usCommunityLength, 0)) == SNMP_COMMUNITY_MISMATCH) { // allow the application to check the community for writes
                return 0;                                                    // silently discard when not for our community
            }
#if defined SUPPORT_SNMPV2C
        case ASN1_SNMP_GET_BULK_REQUEST:                                 // 0xa5
#endif
        case ASN1_SNMP_GET_NEXT_REQUEST:                                 // 0xa1 request for next value
        case ASN1_SNMP_GET_REQUEST:                                      // 0xa0 request for specific value
            {
                if ((mib_control.ucRequestType != ASN1_SNMP_SET_REQUEST) && (mib_control.iCommunityAccessRights = fnSNMPClientListener(SNMP_COMMUNITY_CHECK_READ, (unsigned char *)cCommunity, usCommunityLength, 0)) == SNMP_COMMUNITY_MISMATCH) { // allow the application to check the community for reads
                    return 0;                                                    // silently discard when not for our community
                }
                UDP_Message.ucUDP_Message[UDP_Message.usDataLength++] = ASN1_SNMP_GET_RESPONSE; // all requests return a get response
#if defined AVOID_SHIFTS
                UDP_Message.ucUDP_Message[UDP_Message.usDataLength] = 0x82; // dummy length (for maximum possible to avoid shifts later)
                UDP_Message.usDataLength += 3;                           // leave space for length to be inserted
#else
                UDP_Message.ucUDP_Message[UDP_Message.usDataLength++] = 0; // dummy length as place holder
#endif
                ucData = fnGetASN1_length(ucData, &usStructureLength, 0);// get the length and move over it
                if (fnHandleRequest(ucData, &UDP_Message, &mib_control, usStructureLength) == 0) { // handle one or more get requests (or single get-next request)
                    int iNewLength = fnFormatFrame(&UDP_Message, UDP_Message.usDataLength, ASN1_SNMP_GET_RESPONSE); // ensure formated correctly
                    if (iNewLength > 0) {                                // as long as no formatting error
                        fnSendUDP(SNMP_socket, ucIP, usPort, (unsigned char *)&UDP_Message.tUDP_Header, (unsigned short)iNewLength, 0); // send response back (this will never fail due to the ARP cache not containg the address)
                    }
                }
            }
            break;
#if defined SUPPORT_SNMPV2C_                                             // not used
        case ASN1_SNMP_INFORM_REQUEST:                                   // 0xa6 (initially manager to manager but also allowed from agent to manager - not received)
            break;
        case ASN1_SNMP_TRAP2:                                            // 0xa7 (traps are not received by agents)
            break;
        case ASN1_SNMP_REPORT:                                           // 0xa8
            break;
#endif
        default:
            break;                                                       // unknown type - silently discard
        }
    }
    return 0;
}


// Send an SNMP trap to the specified manager (specific code is only relevant to V1 traps)
//
static int fnGenerateTrap(unsigned char ucTrap, unsigned char ucSpecificCode, int iManagerRef) // {3} specific manager passed
{
    unsigned char ucManagerVersion = SNMP_MANAGER_V1;
    unsigned char ucVersionRef = SNMPV1;

    // Send V1, V2c and V3 traps
    //
    while (ucVersionRef < MAX_SNMP_VERSION) {                            // work through SNMP versions
        if (ptrSNMP_manager_details[iManagerRef].ucSNMP_versions & ucManagerVersion) { // if this version of traps are to be sent to this manager
            if (SNMP_TRAP_TX_RESOLVING == fnSendTrap(ucTrap, ucSpecificCode, iManagerRef, ucVersionRef)) { // send the trap for this SNMP version
                return SNMP_TRAP_TX_RESOLVING;                           // if the address has to be resolved there is no need in trying other versions to this address
            }
        }
        ucManagerVersion <<= 1;
        ucVersionRef++;                                                  // increase the snmp version
    }
    return 0;                                                            // success
}

// Build and send a trap message to the SNMP manager (specific code is only relevant to V1 traps)
//
extern void fnSendSNMPTrap(unsigned char ucTrap, unsigned char ucSpecificCode, unsigned long ulManagers) // {101a}
{
    unsigned long ulValidManager = 0x00000001;
    int iManagerNumber = 0;
    while (iManagerNumber < SNMP_MANAGER_COUNT) {                        // enter the trap into the queue
        if (ulManagers & ulValidManager) {                               // if this manager is to be informed
            if ((ucTrapCnt[iManagerNumber] < SNMP_TRAP_QUEUE_LENGTH) && (temp_pars->temp_parameters.usServers & (ACTIVE_SNMP_0 << iManagerNumber)) && (uMemcmp(ptrSNMP_manager_details[iManagerNumber].snmp_manager_ip_address, cucNullMACIP, IPV4_LENGTH) != 0)) { // if there is space in its trap queue and the manager's IP address is valid
                trap_list[iManagerNumber][ucTrapCnt[iManagerNumber]].ucTrapType = ucTrap; // enter the trap details
                trap_list[iManagerNumber][ucTrapCnt[iManagerNumber]].ucTrapSpecificCode = ucSpecificCode;
                if (ucTrapCnt[iManagerNumber]++ == 0) {                  // first in queue
                    ucSNMPRetries[iManagerNumber] = 0;                   // reset the retry counter
                    if (fnGenerateTrap(ucTrap, ucSpecificCode, iManagerNumber) != SNMP_TRAP_TX_RESOLVING) { // try to send the new trap to the specified manager {3}
                        ucTrapCnt[iManagerNumber] = 0;                   // assume that the trap was transmitted so it no longer needs to be queued
                    }
                }
            }
            else {
                ulManagers &= ~ulValidManager;
            }
        }
        ulValidManager <<= 1;
        iManagerNumber++;
    }
}

// ARP failed to resolve the destination address
//
static void fnResendSNMPTrap(unsigned char ucManager)
{
    if (fnGenerateTrap(trap_list[ucManager][0].ucTrapType, trap_list[ucManager][0].ucTrapSpecificCode, ucManager) != SNMP_TRAP_TX_RESOLVING) {
        int iTrapQueue = 1;                                              // frame could be sent this time so can be deleted form the queue
        while (--ucTrapCnt[ucManager] != 0) {                            // any further in manager queue are also sent now since the destination is known
            fnGenerateTrap(trap_list[ucManager][iTrapQueue].ucTrapType, trap_list[ucManager][iTrapQueue].ucTrapSpecificCode, ucManager);
            iTrapQueue++;
        }
    }
}

// Resend a queued trap frame - usually due to ARP having been resolved
//
static int fnRetry(USOCKET socket, int iResolved)
{
    unsigned char ucSNMP_manager = ((socket >> USER_INFO_SHIFT) & USER_INFO_MASK); // extract the snmp manager information
    if (iResolved != 0)  {                                               // if called due to successful ARP resolution
        ucSNMPRetries[ucSNMP_manager] = 0;                               // reset the repetition counter
    }

    if (ucSNMPRetries[ucSNMP_manager] < SNMP_NUM_RETRIES) {              // if the maximum retries have not been used up
        ucSNMPRetries[ucSNMP_manager]++;
        fnResendSNMPTrap(ucSNMP_manager);                                // try again
    }
    else {                                                               // last timeout - resolution failed
        ucSNMPRetries[ucSNMP_manager] = 0;
        ucTrapCnt[ucSNMP_manager] = 0;                                   // cancel any traps in the queue because there is no manager available
        return 1;                                                        // we give up
    }
    return 0;                                                            // OK - repetition attempted
}
#endif
