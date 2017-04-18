/**********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher

    ---------------------------------------------------------------------
    File:      usb_host_loader.c - MSD
    Project:   uTasker project
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2016
    *********************************************************************
    USB-MSD host loader application
    19.11.2015 Limit UFI_READ_FORMAT_CAPACITY transfer length to a single 64 byte frame {1}

*/


/* =================================================================== */
/*                           include files                             */
/* =================================================================== */

#include "config.h"

#if  (defined USB_INTERFACE && defined USB_MSD_HOST_LOADER && defined USB_HOST_SUPPORT)

extern void fnUSB_ResetCycle(void); // temp

/* =================================================================== */
/*                          local definitions                          */
/* =================================================================== */

#define OWN_TASK                            TASK_USB_HOST

#if !defined USB_CDC_COUNT
    #define USB_CDC_COUNT                   1
#endif
#if !defined MODBUS_USB_CDC_COUNT
    #define MODBUS_USB_CDC_COUNT            0
#endif

#if !defined USB_SPEC_VERSION                                            // {28}
    #define USB_SPEC_VERSION                USB_SPEC_VERSION_1_1         // default is to report USB1.1 since it is equivalent to USB2.0 but requires one less descriptor exchange
#endif

#define NUMBER_OF_ENDPOINTS                 3                            // reserve 2 bulk endpoints for USB-MSD host as well as an interrupt endpoint (although not used, some memory sticks request this)

#define STATUS_TRANSPORT                    0x100


// Interrupt events
//
#define EVENT_LUN_READY                     1


// Timer events
//
#define T_CHECK_DISK                        1
#define T_GET_STATUS_AFTER_STALL            2
#define T_GET_STATUS_TEST                   3
#define T_REPEAT_COMMAND                    4
#define T_REQUEST_LUN                       5


#if defined _LITTLE_ENDIAN || defined _WINDOWS
    #define USBC_SIGNATURE   (0x43425355)                                // 'U' 'S' 'B' 'C'
    #define USBS_SIGNATURE   (0x53425355)                                // 'U' 'S' 'B' 'S'
#else
    #define USBC_SIGNATURE   (0x55534243)                                // 'U' 'S' 'B' 'C'
    #define USBS_SIGNATURE   (0x55534253)                                // 'U' 'S' 'B' 'S'
#endif


/* =================================================================== */
/*                       local structure definitions                   */
/* =================================================================== */


/* =================================================================== */
/*                             constants                               */
/* =================================================================== */

static const USB_HOST_DESCRIPTOR get_max_lum = {
    (STANDARD_DEVICE_TO_HOST | REQUEST_INTERFACE_CLASS),                 // 0xa1 request class specific interface
    GET_MAX_LUN,                                                         // 0xfe
    {LITTLE_SHORT_WORD_BYTES(0)},
    {LITTLE_SHORT_WORD_BYTES(0)},
    {LITTLE_SHORT_WORD_BYTES(1)},
};


/* =================================================================== */
/*                     global variable definitions                     */
/* =================================================================== */


/* =================================================================== */
/*                      local variable definitions                     */
/* =================================================================== */

static QUEUE_HANDLE USB_control = NO_ID_ALLOCATED;                       // USB default control endpoint handle
static unsigned short usExpectedData = 0;                                // used by the control endpoint to collect data
static unsigned char  ucCollectingMode = 0;

static QUEUE_HANDLE USBPortID_host = NO_ID_ALLOCATED;
static unsigned char ucMSDBulkInEndpoint = 0;                            // the endpoint that is to be used as MSD bulk IN endpoint
static unsigned short usMSDBulkInEndpointSize = 0;
static unsigned char ucMSDBulkOutEndpoint = 0;                           // the endpoint that is to be used as MSD bulk OUT endpoint
static unsigned short usMSDBulkOutEndpointSize = 0;
static unsigned char ucRequestCount = 0;

#if defined USB_STRING_OPTION && defined USB_RUN_TIME_DEFINABLE_STRINGS
    static USB_STRING_DESCRIPTOR *SerialNumberDescriptor;                // string built up to contain a variable serial number
#endif

static unsigned char ucDeviceLUN = 0;
static int iUSB_MSD_OpCode = 0;
static unsigned long ulTag =0 ;
static unsigned long ulLBA = 0;
static unsigned long ulBlockByteCount = 0;
static unsigned short usBlockCount = 0;


/* =================================================================== */
/*                      local function definitions                     */
/* =================================================================== */

static void fnConfigureUSB(void);                                        // routine to open and configure USB interface
#if defined USB_STRING_OPTION && defined USB_RUN_TIME_DEFINABLE_STRINGS
    static void fnSetSerialNumberString(CHAR *ptrSerialNumber);
#endif
static void fnConfigureApplicationEndpoints(unsigned char ucConfiguration);
static void fnRequestLUN(void);
static int fnSendMSD_host(unsigned char ucOpcode);
static unsigned char fnDisplayDeviceInfo(void);


/* =================================================================== */
/*                                task                                 */
/* =================================================================== */


// USB host task
//
extern void fnTaskUSB_host(TTASKTABLE *ptrTaskTable)
{
    QUEUE_HANDLE PortIDInternal = ptrTaskTable->TaskID;                  // queue ID for task input
    unsigned char ucInputMessage[LARGE_MESSAGE];                         // reserve space for receiving messages
    QUEUE_TRANSFER Length;

    if (USB_control == NO_ID_ALLOCATED) {                                // initialisation
#if (defined KWIKSTIK || defined TWR_K40X256 || defined TWR_K40D100M || defined TWR_K53N512 || defined FRDM_KL46Z || defined FRDM_KL43Z || defined TWR_KL43Z48M || defined TWR_KL46Z48M) // {3}{6}
        CONFIGURE_SLCD();
#endif
#if defined USB_STRING_OPTION && defined USB_RUN_TIME_DEFINABLE_STRINGS  // if dynamic strings are supported, prepare a specific serial number ready for enumeration
        fnSetSerialNumberString(temp_pars->temp_parameters.cDeviceIDName); // construct a serial number string for USB use
#endif
        fnConfigureUSB();                                                // configure the USB host interface
    }

    while (fnRead(PortIDInternal, ucInputMessage, HEADER_LENGTH) != 0) { // check task input queue
        switch (ucInputMessage[MSG_SOURCE_TASK]) {                       // switch depending on source
        case TIMER_EVENT:
            if (T_GET_STATUS_AFTER_STALL == ucInputMessage[MSG_TIMER_EVENT]) {
                ucRequestCount++;                                        // count the number of stall that are cleared for this request
                iUSB_MSD_OpCode |= STATUS_TRANSPORT;                     // mark that the status transport is being read
                fnDriver(USBPortID_host, (RX_ON), 0);                    // enable IN polling so that we can receive the status transport after the endpoint was cleared
                continue;
            }
            else if (T_GET_STATUS_TEST == ucInputMessage[MSG_TIMER_EVENT]) {
                fnDriver(USBPortID_host, (RX_ON), 0);                    // enable IN polling so that we can receive the status transport after the endpoint was cleared
                continue;
            }
            else if (T_REPEAT_COMMAND == ucInputMessage[MSG_TIMER_EVENT]) { // repeat command since the stick reported an error when last sent (assume the stick is not yet ready)
                fnSendMSD_host(iUSB_MSD_OpCode);
                continue;
            }
            else if (T_REQUEST_LUN == ucInputMessage[MSG_TIMER_EVENT]) {
                fnRequestLUN();
              //ucDeviceLUN = 1;                                         //WARNING - if the LUM is not requested the host state machine stays in an unexpected state (to improve in host state-machine)
              //fnInterruptMessage(OWN_TASK, EVENT_LUN_READY);
            }
            break;

        case INTERRUPT_EVENT:                                            // interrupt event without data
            switch (ucInputMessage[MSG_INTERRUPT_EVENT]) {               // specific interrupt event type
            case EVENT_USB_RESET:                                        // active USB connection has been reset
                fnDebugMsg("USB Reset\n\r");                             // display that the USB bus has been reset
#if defined DEL_USB_SYMBOL
                DEL_USB_SYMBOL();                                        // optionally display the new USB state
#endif
                break;

            case EVENT_USB_SUSPEND:                                      // a suspend condition has been detected. A bus powered device should reduce consumption to <= 500uA or <= 2.5mA (high power device)
                fnSetUSBConfigState(USB_DEVICE_SUSPEND, 0);              // set all endpoint states to suspended
                fnDebugMsg("USB Suspended\n\r");
#if defined DEL_USB_SYMBOL
                DEL_USB_SYMBOL();                                        // optionally display the new USB state
#endif
                break;

            case EVENT_USB_RESUME:                                       // a resume sequence has been detected so full power consumption can be resumed
                fnSetUSBConfigState(USB_DEVICE_RESUME, 0);               // remove suspended state from all endpoints
                fnDebugMsg("USB Resume\n\r");
#if defined SET_USB_SYMBOL
                SET_USB_SYMBOL();                                        // optionally display the new USB state
#endif
                break;

            case EVENT_USB_DETECT_HS:
            case EVENT_USB_DETECT_LS:
            case EVENT_USB_DETECT_FS:
                fnDebugMsg("USB ");
                if (EVENT_USB_DETECT_HS == ucInputMessage[MSG_INTERRUPT_EVENT]) {
                    fnDebugMsg("H");                                     // high speed device attached
                }
                else if (EVENT_USB_DETECT_FS == ucInputMessage[MSG_INTERRUPT_EVENT]) {
                    fnDebugMsg("F");                                     // full speed device attached
                }
                else {
                    fnDebugMsg("L");                                     // low speed device attached
                }
                fnDebugMsg("S device detected\r\n");
                break;

            case EVENT_USB_REMOVAL:
                fnDebugMsg("USB device removed\r\n");
                break;
            case EVENT_LUN_READY:
                fnDebugMsg("LUN =");
                fnDebugDec(ucDeviceLUN, (WITH_CR_LF | WITH_SPACE));      // max LUN
                ucDeviceLUN = 1;                                         // communicate with first LUN
                fnSendMSD_host(UFI_INQUIRY);                             // continue by requesting information from the connected MSD
                break;
            }                                                            // end interrupt event switch
            break;

        case TASK_USB:                                                   // USB interrupt handler is requesting us to perform work offline
            {
                unsigned char ucEndpointConfiguration;
                fnRead(PortIDInternal, &ucInputMessage[MSG_CONTENT_COMMAND], ucInputMessage[MSG_CONTENT_LENGTH]); // get the content
                ucEndpointConfiguration = ucInputMessage[MSG_CONTENT_COMMAND + 1];
                switch (ucInputMessage[MSG_CONTENT_COMMAND]) {           // switch on the event type
                case E_USB_DEVICE_INFO:                                  // the USB host has collected information ready to decide how to work with a device
                    fnDebugMsg("USB device information ready:\r\n");
                    ucEndpointConfiguration = fnDisplayDeviceInfo();     // analyse and display information
                    if (ucEndpointConfiguration != 0) {                  // if there is a supported configuration to be enabled
                        ulTag = fnRandom();                              // start with random tag number
                        ulTag <<= 16;
                        ulTag |= fnRandom();
                        fnDriver(USB_control, (TX_ON), ucEndpointConfiguration); // enable configuration - this causes the generic USB layer to send SetConfiguration
                    }
                    break;
                case E_USB_DEVICE_STALLED:
                    fnDebugMsg("Stall on EP-");
                    fnDebugDec(ucEndpointConfiguration, 0);              // display the endpoint that stalled
                    fnDebugMsg("\n\r");
                    fnClearFeature(USB_control, ucEndpointConfiguration);// clear the stalled endpoint
                    break;
                case E_USB_DEVICE_CLEARED:
                    fnDebugMsg("EP-");
                    fnDebugDec(ucEndpointConfiguration, 0);              // display the endpoint that was cleared
                    fnDebugMsg(" cleared\n\r");
                    uTaskerMonoTimer(OWN_TASK, (DELAY_LIMIT)(0.05 * SEC), T_GET_STATUS_AFTER_STALL); // collect the status after a short delay
                    break;
                case E_USB_HOST_ERROR_REPORT:
                    fnDebugMsg("USB Host error: EP-");
                    fnDebugDec((ucEndpointConfiguration >> 4), 0);
                    fnDebugMsg(" Code: ");
                    fnDebugDec((ucEndpointConfiguration & 0x0f), WITH_CR_LF);
                    // Attempt a device reset in case it was a spurious error
                    //
                    fnUSB_ResetCycle();
                    break;
                case E_USB_ACTIVATE_CONFIGURATION:
                    fnDebugMsg("Enumerated (");                          // the interface has been activated and enumeration has completed
                    fnDebugDec(ucEndpointConfiguration, 0);              // the configuration
                    fnDebugMsg(")\n\r");
#if defined SET_USB_SYMBOL
                    SET_USB_SYMBOL();                                    // display connection in LCD, on LED etc.
#endif
                    fnConfigureApplicationEndpoints(ucEndpointConfiguration); // configure endpoints according to configuration
                    iUSB_MSD_OpCode = 0;
                    fnRequestLUN();                                      // the first thing that the MSD host does is request the number of logical units that the disk drive has
                }
                break;
            }
            break;

        default:
            break;
        }
    }

    if (USBPortID_host == NO_ID_ALLOCATED) {                             // don't check bulk input queue until host is configured
        return;
    }

    while (fnMsgs(USBPortID_host) != 0) {                                // reception from host IN endpoint
        Length = fnRead(USBPortID_host, ucInputMessage, 64);             // read the content (up to 64 bytes content each time)
        switch (iUSB_MSD_OpCode) {
        case UFI_READ_10:
            {
                static unsigned char ucBuffer[512];
                static int iOffset = 0;
                fnDriver(USBPortID_host, (RX_ON), 0);                    // enable IN polling so that we can receive next block or the status transport
                if (Length >= ulBlockByteCount) {                        // final data packet
                    uMemcpy(&ucBuffer[iOffset], ucInputMessage, ulBlockByteCount);
                    ulBlockByteCount = 0;
                    iOffset = 0;
                    iUSB_MSD_OpCode |= STATUS_TRANSPORT;
                }
                else {                                                   // more data expected
                    uMemcpy(&ucBuffer[iOffset], ucInputMessage, Length);
                    iOffset += Length;
                    ulBlockByteCount -= Length;
                }
            }
            break;
        case UFI_INQUIRY:                                                // expecting data transport from inquiry
            {
              //CBW_INQUIRY_DATA *ptrInquiry = (CBW_INQUIRY_DATA *)ucInputMessage;
                fnDriver(USBPortID_host, (RX_ON), 0);                    // enable IN polling so that we can receive the status transport
                iUSB_MSD_OpCode |= STATUS_TRANSPORT;
            }
            break;
        case UFI_READ_FORMAT_CAPACITY:
            {
                CBW_CAPACITY_LIST *ptrCapacityList = (CBW_CAPACITY_LIST *)ucInputMessage;
                unsigned char ucListLength = ptrCapacityList->ucCapacityListLength;
                CAPACITY_DESCRIPTOR *ptrCapacityDescriptor = &ptrCapacityList->capacityDescriptor; 
                fnDriver(USBPortID_host, (RX_ON), 0);                    // enable IN polling so that we can receive the status transport
                iUSB_MSD_OpCode |= STATUS_TRANSPORT;
                while (ucListLength >= 8) {                              // for each capacity descriptor
                    unsigned long ulBlockLength = ((ptrCapacityDescriptor->ucBlockLength[0] << 16) | (ptrCapacityDescriptor->ucBlockLength[1] << 8) | ptrCapacityDescriptor->ucBlockLength[2]);
                    unsigned long ulNoOfBlocks  = ((ptrCapacityDescriptor->ucNumberOfBlocks[0] << 24) | (ptrCapacityDescriptor->ucNumberOfBlocks[1] << 16) | (ptrCapacityDescriptor->ucNumberOfBlocks[2] << 8) | ptrCapacityDescriptor->ucNumberOfBlocks[3]);
                    fnDebugMsg("(");
                    fnDebugDec(ptrCapacityDescriptor->ucDescriptorCode, 0);
                    fnDebugMsg(":");
                    fnDebugDec(ulBlockLength, 0);
                    fnDebugMsg(":");
                    fnDebugDec(ulNoOfBlocks, 0);
                    fnDebugMsg(") ");
                    ptrCapacityDescriptor++;
                    if (ucListLength >= 16) {
                        ucListLength -= 8;
                    }
                    else {
                        break;
                    }
                }
            }
            break;
        case UFI_REQUEST_SENSE:
            {
              //CBW_RETURN_SENSE_DATA *ptrSenseData = (CBW_RETURN_SENSE_DATA *)ucInputMessage;
                fnDriver(USBPortID_host, (RX_ON), 0);                    // enable IN polling so that we can receive the status transport
                iUSB_MSD_OpCode |= STATUS_TRANSPORT;
            }
            break;
        case UFI_READ_CAPACITY:
            {
                CBW_READ_CAPACITY_DATA *ptrCapacity = (CBW_READ_CAPACITY_DATA *)ucInputMessage;
                unsigned long ulLastLogicalBlockAddress = ((ptrCapacity->ucLastLogicalBlockAddress[0] << 24) | (ptrCapacity->ucLastLogicalBlockAddress[1] << 16) | (ptrCapacity->ucLastLogicalBlockAddress[2] << 8) | ptrCapacity->ucLastLogicalBlockAddress[3]);
                unsigned long ulBlockLengthInBytes =  ((ptrCapacity->ucBlockLengthInBytes[0] << 24) | (ptrCapacity->ucBlockLengthInBytes[1] << 16) | (ptrCapacity->ucBlockLengthInBytes[2] << 8) | ptrCapacity->ucBlockLengthInBytes[3]);
                fnDriver(USBPortID_host, (RX_ON), 0);                    // enable IN polling so that we can receive the status transport
                iUSB_MSD_OpCode |= STATUS_TRANSPORT;
                fnDebugMsg("(");
                fnDebugDec(ulBlockLengthInBytes, 0);                     // block length
                fnDebugMsg(":");
                fnDebugDec(ulLastLogicalBlockAddress, 0);                // last logical block address
                fnDebugMsg(") ");
            }
            break;
        case (UFI_INQUIRY | STATUS_TRANSPORT):                           // expecting status transport after inquiry data
        case (UFI_READ_FORMAT_CAPACITY | STATUS_TRANSPORT):              // expecting status transport after reading format capacities
        case (UFI_REQUEST_SENSE | STATUS_TRANSPORT):                     // expecting status transport after reading requesting sense
        case (UFI_READ_CAPACITY | STATUS_TRANSPORT):                     // expecting status transport after reading capacity
        case (UFI_TEST_UNIT_READY | STATUS_TRANSPORT):                   // expecting status transport after sending unit ready
        case (UFI_READ_10 | STATUS_TRANSPORT):                           // expecting status transport after reading data
            {
                USB_MASS_STORAGE_CBW_LW *ptrStatus = (USB_MASS_STORAGE_CBW_LW *)ucInputMessage;
                fnDebugMsg("Status transport - ");
                if ((ptrStatus->dCBWSignatureL == USBS_SIGNATURE) && (ptrStatus->dCBWTagL == ulTag)) { // check the status transport's signature and tag
                    if (ptrStatus->dmCBWFlags == CSW_STATUS_COMMAND_PASSED) { // check that there are no errors reported
                        fnDebugMsg("Passed\r\n");
                        ucRequestCount = 0;
                        switch (iUSB_MSD_OpCode) {
                        case (UFI_INQUIRY | STATUS_TRANSPORT):           // inquiry was successful
                            fnSendMSD_host(UFI_REQUEST_SENSE);           // now request sense
                            break;
                        case (UFI_REQUEST_SENSE | STATUS_TRANSPORT):
                            fnSendMSD_host(UFI_READ_FORMAT_CAPACITY);    // now request format capacity
                            break;
                        case (UFI_READ_FORMAT_CAPACITY | STATUS_TRANSPORT):
                            fnSendMSD_host(UFI_READ_CAPACITY);           // now request capacity
                            break;
                        case (UFI_READ_CAPACITY | STATUS_TRANSPORT):
                            // At this point the memory stick details are known and it can be mounted
                            //
                            fnEventMessage(TASK_MASS_STORAGE, OWN_TASK, MOUNT_USB_MSD); // initiate mounting the memory stick at the mass storage task
                            break;
                        case (UFI_TEST_UNIT_READY | STATUS_TRANSPORT):
                            fnSendMSD_host(UFI_READ_FORMAT_CAPACITY);    // now request format capacity
                            break;
                        case (UFI_READ_10 | STATUS_TRANSPORT):
                            iUSB_MSD_OpCode = 0;
                            fnDebugMsg("READ:");
                            break;
                        }
                    }
                    else {                                               // failed
                        iUSB_MSD_OpCode &= ~(STATUS_TRANSPORT);          // the origial request
                        if (UFI_TEST_UNIT_READY == iUSB_MSD_OpCode) {    // unit of not ready
                            fnDebugMsg("Not ready\r\n");                 // alway repeat when not ready
                            iUSB_MSD_OpCode = UFI_REQUEST_SENSE;
                        }
                        else if (ucRequestCount != 0) {                  // if the original request stalled
                            if (UFI_READ_FORMAT_CAPACITY == iUSB_MSD_OpCode) { // some sticks may not support this so we repeat just three times and then accept that there will be no ansswer
                                if (ucRequestCount >= 3) {
                                    iUSB_MSD_OpCode = UFI_READ_CAPACITY; // continue with next request
                                    ucRequestCount = 0;
                                }
                            }
                            else if (UFI_READ_CAPACITY == iUSB_MSD_OpCode) { // some sticks may be slow and stall on UFI_READ_FORMAT_CAPACITY and then never accept UFI_READ_CAPACITY
                                if (ucRequestCount >= 3) {
                                    iUSB_MSD_OpCode = UFI_TEST_UNIT_READY;
                                    ucRequestCount = 0;
                                }
                            }
                        }
                        uTaskerMonoTimer(OWN_TASK, (DELAY_LIMIT)(0.05 * SEC), T_REPEAT_COMMAND); // repeat command after a short delay
                    }
                }
                else {
                    fnDebugMsg("ERROR STATUS\r\n");
                    iUSB_MSD_OpCode = 0;
                }
            }
        }
    }
}


#if !defined USB_MSD_DEVICE_LOADER
// The application must always supply this routine and return its device descriptor when requested
// This example shows a single fixed device configuration but multiple configurations could be selected (eg. for experimental use)
//
extern void *fnGetUSB_device_descriptor(unsigned short *usLength)
{
    return 0;                                                            // dummy for host
}

// The application must always supply this routine and return its configuration descriptor when requested
// This example shows a single fixed configuration but multiple configurations could be selected (eg. for programmable device types)
//
extern void *fnGetUSB_config_descriptor(unsigned short *usLength)
{
    return 0;                                                            // in host mode we return a zero so that the generic driver uses its own information
}

#if defined USB_STRING_OPTION
    #if defined USB_RUN_TIME_DEFINABLE_STRINGS
// This routine constructs a USB string descriptor for use by the USB interface during emumeration
// - the new string has to respect the descriptor format (using UNICODE) and is built in preparation so that it can be passed in an interrupt
//
static void fnSetSerialNumberString(CHAR *ptrSerialNumber) {             // {12}
    unsigned char ucDescriptorLength = (sizeof(USB_STRING_DESCRIPTOR) - 2);
    unsigned char *ptrString;
    int iStringLength = (uStrlen(ptrSerialNumber) * 2);
    if (iStringLength == 0) {
        ucDescriptorLength += 2;                                         // space for a null-terminator
    }
    else {
        ucDescriptorLength += iStringLength;
    }
    if (SerialNumberDescriptor == 0) {
        SerialNumberDescriptor = uMalloc(ucDescriptorLength);            // get memory to store the string descriptor
        SerialNumberDescriptor->bLength = ucDescriptorLength;
        SerialNumberDescriptor->bDescriptorType = DESCRIPTOR_TYPE_STRING;
    }
    ptrString = &SerialNumberDescriptor->unicode_string_space[0];
    if (iStringLength == 0) {
        *ptrString++ = 0;                                                // when no string add a null-terminator
        *ptrString++ = 0;
    }
    else {
        while (*ptrSerialNumber != 0) {
            *ptrString++ = *ptrSerialNumber++;                           // unicode - english string (requiring just zeros to be added)
            *ptrString++ = 0;
        }
    }
}
    #endif


// This routine must always be supplied by the user if usb strings are supported
//
extern unsigned char *fnGetUSB_string_entry(unsigned short usStringRef, unsigned short *usLength)
{
    return 0;                                                            // dummy for host
}
#endif
#endif




// Endpoint 0 call-back for any non-supported control transfers.
// This can be called with either setup frame content (iType != 0) or with data belonging to following OUT frames.
// TERMINATE_ZERO_DATA must be returned to setup tokens with NO further data, when there is no response sent.
// BUFFER_CONSUMED_EXPECT_MORE is returned when extra data is to be received.
// STALL_ENDPOINT should be returned if the request in the setup frame is not expected.
// Return BUFFER_CONSUMED in all other cases.
//
// If further data is to be received, this may arrive in multiple frames and the call-back needs to manage this to be able to know when the data is complete
//
static int control_callback(unsigned char *ptrData, unsigned short length, int iType)
{
    static unsigned char ucLocalEvent = 0;
    int iRtn = BUFFER_CONSUMED;
    switch (iType) {
    case STATUS_STAGE_RECEPTION:                                         // this is the status stage of a control transfer - it confirms that the exchange has completed and can be ignored if not of interest to us
        if (ucLocalEvent != 0) {
            fnInterruptMessage(OWN_TASK, ucLocalEvent);
            ucLocalEvent = 0;
        }
        return BUFFER_CONSUMED;
    case SETUP_DATA_RECEPTION:
        {
            USB_SETUP_HEADER *ptrSetup = (USB_SETUP_HEADER *)ptrData;    // interpret the received data as a setup header
            if ((ptrSetup->bmRequestType & ~STANDARD_DEVICE_TO_HOST) != REQUEST_INTERFACE_CLASS) { // 0x21
                return STALL_ENDPOINT;                                   // stall on any unsupported request types
            }
            usExpectedData = ptrSetup->wLength[0];                       // the amount of additional data which is expected to arrive from the host belonging to this request
            usExpectedData |= (ptrSetup->wLength[1] << 8);
            if (ptrSetup->bmRequestType & STANDARD_DEVICE_TO_HOST) {     // request for information
                usExpectedData = 0;                                      // no data expected to be received by us
                switch (ptrSetup->bRequest) {
                default:
                    return STALL_ENDPOINT;                               // stall on any unsupported requests
                }
            }
            else {                                                       // command
                iRtn = TERMINATE_ZERO_DATA;                              // acknowledge receipt of the request if we have no data to return (default)
                switch (ptrSetup->bRequest) {
                default:
                    return STALL_ENDPOINT;                               // stall on any unsupported requests
                }
            }
            if (length <= sizeof(USB_SETUP_HEADER)) {
                return iRtn;                                             // no extra data in this frame
            }
            length -= sizeof(USB_SETUP_HEADER);                          // header handled
            ptrData += sizeof(USB_SETUP_HEADER);
        }
        // Fall through intentionally
        //
    default:                                                             // OUT_DATA_RECEPTION
        if (usExpectedData != 0) {
            // Handle and control commands here
            //
            switch (ucCollectingMode) {
            case GET_MAX_LUN:
                ucDeviceLUN = (*ptrData + 1);                            // increment the value by 1 to get the number of partitions
                ucLocalEvent = EVENT_LUN_READY;                          // when the returned zero data has been acknowledged we continue with this event
                break;
            default:
                break;
            }
            ucCollectingMode = 0;                                        // reset to avoid repeat of command when subsequent, invalid commands are received
            if (length >= usExpectedData) {
                usExpectedData = 0;                                      // all of the expected data belonging to this transfer has been received
                return TERMINATE_ZERO_DATA;
            }
            else {
                usExpectedData -= length;                                // remaining length to be received before transaction has completed
            }
            return BUFFER_CONSUMED_EXPECT_MORE;
        }
        break;
    }
    return iRtn;
}


// Display a device string - assuming english character set
//
static void fnDisplayUSB_string(unsigned char *ptr_string)
{
    unsigned char ucLength = *ptr_string;                                // reported length of unicode string
    CHAR cSingleCharacter[2];
    if (ucLength > (USB_MAX_STRING_LENGTH * 2)) {                        // in case the unicode string is reported longer than the part that we requested we cut the end
        ucLength = (USB_MAX_STRING_LENGTH * 2);
    }
    cSingleCharacter[1] = 0;                                             // terminator
    while (ucLength >= 4) {                                              // for each unicode character (ignore final 0x0000)
        ptr_string += 2;                                                 // set to second byte of unicode string
        cSingleCharacter[0] = *ptr_string;
        fnDebugMsg(cSingleCharacter);                                    // display
        ucLength -= 2;
    }
}

static unsigned char fnDisplayDeviceInfo(void)
{
    USB_DEVICE_DESCRIPTOR *ptr_device_descriptor;
    USB_CONFIGURATION_DESCRIPTOR *ptr_config_desc;
    USB_INTERFACE_DESCRIPTOR *ptr_interface_desc;
    unsigned char *ptr_string;
    unsigned short usVendor_product;
    ptr_device_descriptor = (USB_DEVICE_DESCRIPTOR *)fnGetDeviceInfo(REQUEST_USB_DEVICE_DESCRIPTOR);
    ptr_config_desc = (USB_CONFIGURATION_DESCRIPTOR *)fnGetDeviceInfo(REQUEST_USB_CONFIG_DESCRIPTOR);
    fnDebugMsg("USB");
    if (ptr_device_descriptor->bcdUSB[1] == 2) {
        fnDebugMsg("2.0");
    }
    else {
        fnDebugMsg("1.1");
    }
    fnDebugMsg(" device with ");
    fnDebugDec(ptr_device_descriptor->bMaxPacketSize0, 0);
    fnDebugMsg(" byte pipe\r\nVendor/Product = ");
    usVendor_product = ((ptr_device_descriptor->idVendor[1] << 8) | ptr_device_descriptor->idVendor[0]);
    fnDebugHex(usVendor_product, (WITH_LEADIN | sizeof(usVendor_product)));
    fnDebugMsg("/");
    usVendor_product = ((ptr_device_descriptor->idProduct[1] << 8) | ptr_device_descriptor->idProduct[0]);
    fnDebugHex(usVendor_product, (WITH_LEADIN | WITH_CR_LF | sizeof(usVendor_product)));
    #if defined USB_STRING_OPTION
    ptr_string = (unsigned char *)fnGetDeviceInfo(REQUEST_USB_STRING_DESCRIPTOR + ptr_device_descriptor->iManufacturer);
    if (ptr_string != 0) {
        fnDebugMsg("Manufacturer = \x22");
        fnDisplayUSB_string(ptr_string);
        fnDebugMsg("\x22\r\n");
    }
    ptr_string = (unsigned char *)fnGetDeviceInfo(REQUEST_USB_STRING_DESCRIPTOR + ptr_device_descriptor->iProduct);
    if (ptr_string != 0) {
        fnDebugMsg("Product = \x22");
        fnDisplayUSB_string(ptr_string);
        fnDebugMsg("\x22\r\n");
    }
    ptr_string = (unsigned char *)fnGetDeviceInfo(REQUEST_USB_STRING_DESCRIPTOR + ptr_device_descriptor->iSerialNumber);
    if (ptr_string != 0) {
        fnDebugMsg("Serial Number = \x22");
        fnDisplayUSB_string(ptr_string);
        fnDebugMsg("\x22\r\n");
    }
    #endif

    if (ptr_config_desc->bmAttributes & SELF_POWERED) {
        fnDebugMsg("\r\nSelf-powered device");
    }
    else {
        fnDebugMsg("\r\nBus-powered device (max. ");
        fnDebugDec((ptr_config_desc->bMaxPower * 2), 0);
        fnDebugMsg("mA)");
    }
    fnDebugMsg(" with ");
    fnDebugDec(ptr_config_desc->bNumInterface, 1);
    fnDebugMsg(" interface(s)\r\n");
    ptr_interface_desc = (USB_INTERFACE_DESCRIPTOR *)(ptr_config_desc + 1);
    if (ptr_interface_desc->bInterfaceClass == INTERFACE_CLASS_MASS_STORAGE) { // we expect a mass storage device class
        USB_ENDPOINT_DESCRIPTOR *ptr_endpoint_desc;
        int iEndpoints = ptr_interface_desc->bNumEndpoints;
        fnDebugMsg("Mass Storage Class : Sub-class =");
        fnDebugHex(ptr_interface_desc->bInterfaceSubClass, (WITH_SPACE | WITH_LEADIN | sizeof(ptr_interface_desc->bInterfaceSubClass)));
        fnDebugMsg(" interface protocol =");
        fnDebugHex(ptr_interface_desc->bInterfaceProtocol, (WITH_SPACE | WITH_LEADIN | WITH_CR_LF | sizeof(ptr_interface_desc->bInterfaceProtocol)));
        fnDebugMsg("Endpoints:\r\n");
        ptr_endpoint_desc = (USB_ENDPOINT_DESCRIPTOR *)(ptr_interface_desc + 1);
        while (iEndpoints--) {
            unsigned short usEndpointLength = (ptr_endpoint_desc->wMaxPacketSize[0] | (ptr_endpoint_desc->wMaxPacketSize[1] << 8));
            unsigned char ucEndpointNumber = (ptr_endpoint_desc->bEndpointAddress & ~(IN_ENDPOINT));
            fnDebugDec(ucEndpointNumber, 0);
            fnDebugMsg(" = ");
            switch (ptr_endpoint_desc->bmAttributes) {
            case ENDPOINT_BULK:
                fnDebugMsg("BULK");
                break;
            case ENDPOINT_INTERRUPT:
                fnDebugMsg("INTERRUPT");
                break;
            case ENDPOINT_ISOCHRONOUS:
                fnDebugMsg("ISOCHRONOUS");
                break;
            case ENDPOINT_CONTROL:
                fnDebugMsg("CONTROL");
                break;
            default:
                fnDebugMsg("??");
                break;
            }
            if (ptr_endpoint_desc->bEndpointAddress & IN_ENDPOINT) {
                fnDebugMsg(" IN");
                if (ptr_endpoint_desc->bmAttributes == ENDPOINT_BULK) {  // it is expected that MSD has only one IN endpoint
                    ucMSDBulkInEndpoint = ucEndpointNumber;              // the endpoint that is to be used as IN endpoint
                    usMSDBulkInEndpointSize = usEndpointLength;
                }
            }
            else {
                fnDebugMsg(" OUT");
                if (ptr_endpoint_desc->bmAttributes == ENDPOINT_BULK) {  // it is expected that MSD has only one OUT endpoint
                    ucMSDBulkOutEndpoint = ucEndpointNumber;             // the endpoint that is to be used as OUT endpoint
                    usMSDBulkOutEndpointSize = usEndpointLength;
                }
            }
            fnDebugMsg(" with size");
            fnDebugDec(usEndpointLength, WITH_SPACE);
            if (ptr_endpoint_desc->bmAttributes == ENDPOINT_INTERRUPT) {
                fnDebugMsg(" (polling interval =");
                fnDebugDec(ptr_endpoint_desc->bInterval, WITH_SPACE);
                fnDebugMsg("ms)");
            }
            fnDebugMsg("\r\n");
            ptr_endpoint_desc++;
        }
        return (ptr_config_desc->bConfigurationValue);                   // the valid configuration to be enabled
    }
    else {
        fnDebugMsg("NON-SUPPORTED CLASS -");
        fnDebugHex(ptr_interface_desc->bInterfaceClass, (WITH_LEADIN | WITH_SPACE | WITH_CR_LF | sizeof(ptr_interface_desc->bInterfaceClass)));
    }
    return 0;                                                            // not supported device
}

static void fnRequestLUN(void)
{
    if (fnWrite(USB_control, (unsigned char *)&get_max_lum, sizeof(get_max_lum)) != 0) { // return directly (non-buffered) - the data must remain stable
        ucCollectingMode = GET_MAX_LUN;                                  // expect data of this type
        usExpectedData = 1;                                              // the length of the expected response
    }
}

static int fnSendMSD_host(unsigned char ucOpcode)
{
    USB_MASS_STORAGE_CBW_LW command_transport;
    QUEUE_TRANSFER command_length;
    uMemset(&command_transport, 0x00, sizeof(command_transport));
    ulTag++;                                                             // increment the transaction ID for each transfer
    command_transport.dCBWSignatureL = USBC_SIGNATURE;
    command_transport.dCBWTagL = ulTag;                                  // transaction identifier
    command_transport.dCBWLUN = (ucDeviceLUN - 1);
    command_transport.CBWCB[0] = ucOpcode;
    command_length = 31;                                                 // physical size of USB_MASS_STORAGE_CBW_LW

    switch (ucOpcode)  {
    case UFI_INQUIRY:
        command_transport.dCBWDataTransferLengthL = LITTLE_LONG_WORD(36);// data transfer length
        command_transport.dmCBWFlags = CBW_IN_FLAG;
        command_transport.dCBWCBLength = 0x06;
        command_transport.CBWCB[4] = 36;
        fnDebugMsg("UFI INQUIRY -> ");
        break;
    case UFI_READ_FORMAT_CAPACITY:
        command_transport.dCBWDataTransferLengthL = LITTLE_LONG_WORD(64);// {1} data transfer length (reduced from 252 to 64 in order to limit the amount of fill data that some memory sticks send back to a single frame)
        command_transport.dmCBWFlags = CBW_IN_FLAG;
        command_transport.dCBWCBLength = 10;
        command_transport.CBWCB[8] = 64;                                 // {1}
        fnDebugMsg("UFI FORMAT CAP. -> ");
        break;
    case UFI_REQUEST_SENSE:
        command_transport.dCBWDataTransferLengthL = LITTLE_LONG_WORD(18);// data transfer length
        command_transport.dmCBWFlags = CBW_IN_FLAG;
        command_transport.dCBWCBLength = 12;
        command_transport.CBWCB[4] = 18;
        fnDebugMsg("UFI REQUEST SENSE -> ");
        break;
    case UFI_READ_CAPACITY:
        command_transport.dCBWDataTransferLengthL = LITTLE_LONG_WORD(8); // data transfer length
        command_transport.dmCBWFlags = CBW_IN_FLAG;
        command_transport.dCBWCBLength = 10;
        fnDebugMsg("UFI READ CAP. -> ");
        break;
    case UFI_TEST_UNIT_READY:
      //command_transport.dCBWDataTransferLengthL = LITTLE_LONG_WORD(0); // data transfer length
      //command_transport.dmCBWFlags = 0;
        command_transport.dCBWCBLength = 6;
        fnDebugMsg("UFI RDY -> ");
        break;
    #if defined UTFAT_WRITE
    case UFI_WRITE_10:
    #endif
    case UFI_READ_10:
        command_transport.dCBWDataTransferLengthL = LITTLE_LONG_WORD(512); // data transfer length
    #if defined UTFAT_WRITE
        if (UFI_WRITE_10 != ucOpcode) {
            command_transport.dmCBWFlags = CBW_IN_FLAG;
        }
    #else
        command_transport.dmCBWFlags = CBW_IN_FLAG;
    #endif
        command_transport.dCBWCBLength = 10;
        command_transport.CBWCB[2] = (unsigned char)(ulLBA >> 24);
        command_transport.CBWCB[3] = (unsigned char)(ulLBA >> 16);
        command_transport.CBWCB[4] = (unsigned char)(ulLBA >> 8);
        command_transport.CBWCB[5] = (unsigned char)ulLBA;
        command_transport.CBWCB[7] = (unsigned char)(usBlockCount >> 8);
        command_transport.CBWCB[8] = (unsigned char)(usBlockCount);
        ulBlockByteCount = (usBlockCount * 512);                         // the amount of data in bytes
        break;
    default:
        return -1;
    }
    fnFlush(USBPortID_host, FLUSH_TX);                                   // always flush the tx buffer to ensure message alignment in buffer before sending
    if (fnWrite(USBPortID_host, (unsigned char *)&command_transport, command_length) == command_length) { // write data (buffered on this bulk endpoint)
        if ((ucOpcode != UFI_WRITE_10) && (ucOpcode != UFI_READ_10)) {
            uTaskerMonoTimer(OWN_TASK, (DELAY_LIMIT)(0.05 * SEC), T_GET_STATUS_TEST); // collect the status after a short delay (we should do it after the transmission has been acked!!!)
          //fnDriver(USBPortID_host, (RX_ON), 0);                        // enable IN polling so that we can receive data/status response
        }
        if (UFI_TEST_UNIT_READY == ucOpcode) {
            iUSB_MSD_OpCode = (ucOpcode | STATUS_TRANSPORT);             // this message has no data reception so we immediately request the status transport
        }
        else {
            iUSB_MSD_OpCode = ucOpcode;                                  // the operation in progress
        }
    }
    else {
        fnDebugMsg("Host write failure!\r\n");                           // the host could not send, due to device removal or buffer error
    }
    return 0;
}

static void fnConfigureApplicationEndpoints(unsigned char ucActiveConfiguration)
{
    USBTABLE tInterfaceParameters;                                       // table for passing information to driver

    if (NO_ID_ALLOCATED == USBPortID_host) {
        tInterfaceParameters.owner_task = OWN_TASK;                      // wake usb task on receptions
        tInterfaceParameters.Endpoint = ucMSDBulkOutEndpoint;            // set USB endpoints to act as an input/output pair - transmitter (OUT)
        tInterfaceParameters.Paired_RxEndpoint = ucMSDBulkInEndpoint;    // receiver (IN)
        tInterfaceParameters.usEndpointSize = usMSDBulkOutEndpointSize;  // endpoint queue size (2 buffers of this size will be created for reception)
        tInterfaceParameters.usb_callback = 0;                           // no call-back since we use rx buffer - the same task is owner
        tInterfaceParameters.usConfig = 0;
        tInterfaceParameters.queue_sizes.RxQueueSize = 1024;             // optional input queue (used only when no call-back defined)
        tInterfaceParameters.queue_sizes.TxQueueSize = (512 + 32);       // additional tx buffer - size for a sector plus a command
        #if defined WAKE_BLOCKED_USB_TX
        tInterfaceParameters.low_water_level = (tInterfaceParameters.queue_sizes.TxQueueSize/2); // TX_FREE event on half buffer empty
        #endif
        USBPortID_host = fnOpen(TYPE_USB, 0, &tInterfaceParameters);     // open the endpoints with defined configurations (initially inactive)
    }
    fnSetUSBConfigState(USB_CONFIG_ACTIVATE, ucActiveConfiguration);     // now activate the configuration
}

static int utReadMSD(unsigned char *ptrBuffer, unsigned long ulSectorNumber)
{
    #define MAX_USB_MSD_READ_WAIT (50000)
    volatile int iWait;
    USB_MASS_STORAGE_CBW_LW status_stage;
    QUEUE_TRANSFER byte_length = 512;
    QUEUE_TRANSFER Length;
    ulLBA = ulSectorNumber;                                              // read a sector from this logical block address
    usBlockCount = 1;                                                    // read a single block
    fnDebugMsg("*");
    fnSendMSD_host(UFI_READ_10);                                         // request the read (and enable IN polling for the read data)
    fnDriver(USBPortID_host, (RX_ON), 0);                                // enable IN polling so that we can receive data response
    iWait = 0;
    while (1) {                                                          // wait until the data has been received
        Length = fnRead(USBPortID_host, (ptrBuffer + (512 - byte_length)), byte_length); // read received data
        if (Length != 0) {                                               // all or part of the requested data is ready
            byte_length -= Length;                                       // the remaining length
            fnDriver(USBPortID_host, (RX_ON), 0);                        // enable IN polling so that we can receive further data, or the status stage
            iWait = 0;
            if (byte_length == 0) {                                      // data has been completely received so we are waiting for the status stage
                while (1) {                                              // wait until the status stage has been received
                    if (fnRead(USBPortID_host, (unsigned char *)&status_stage, sizeof(status_stage)) != 0) { // read status stage
                        if ((status_stage.dCBWSignatureL == USBS_SIGNATURE) && (status_stage.dCBWTagL == ulTag)) {
                          //fnDebugMsg(" OK\r\n");
                            break;
                        }
                        else {
                            fnDebugMsg(" Staus error\r\n");
                            return UTFAT_DISK_READ_ERROR;
                        }
                    }
                    else {
    #if defined _WINDOWS
                        fnSimInts(0);                                    // allow the USB interrupts to be simulated
    #endif
                        if (++iWait > MAX_USB_MSD_READ_WAIT) {          // limit the maximum wait in case the memory stick fails
                            fnDebugMsg(" TOS\r\n");
                            return UTFAT_DISK_READ_ERROR;
                        }
                    }
                }
                break;
            }
        }
        else {
    #if defined _WINDOWS
            fnSimInts(0);                                                // allow the USB interrupts to be simulated
    #endif
            if (++iWait > MAX_USB_MSD_READ_WAIT) {                       // limit the maximum wait in case the memory stick fails
                fnDebugMsg(" TOD\r\n");                                  // timeout
                return UTFAT_DISK_READ_ERROR;
            }
        }
    }
    return UTFAT_SUCCESS;
}

// Read a single sector from the disk (usb-msd drive) - the sector number is specified by ulSectorNumber
//
extern int utReadMSDsector(UTDISK *ptr_utDisk, unsigned long ulSectorNumber, void *ptrBuf)
{
    return (utReadMSD((unsigned char *)ptrBuf, ulSectorNumber));         // this blocks until the read completes (or fails)
}

// Read a single sector from the disk (usb-msd drive) - the sector number is specified by ulSectorNumber but return only demanded quantity
//
extern int utReadPartialMSDsector(UTDISK *ptr_utDisk, unsigned long ulSectorNumber, void *ptrBuf, unsigned short usOffset, unsigned short usLength)
{
    unsigned long ulTemp[512/sizeof(unsigned long)];
    int iRtn;
    iRtn = utReadMSD((unsigned char *)ulTemp, ulSectorNumber);           // read sector content to temporary buffer
    if (UTFAT_SUCCESS == iRtn) {
        uMemcpy(ptrBuf, (((unsigned char *)ulTemp) + usOffset), usLength); // return the requested content
    }
    return iRtn;
}

#if defined UTFAT_WRITE
static int utWriteMSD(unsigned char *ptrBuffer, unsigned long ulSectorNumber)
{
    #define MAX_USB_MSD_WRITE_WAIT (50000)
    volatile int iWait;
    USB_MASS_STORAGE_CBW_LW status_stage;
    ulLBA = ulSectorNumber;                                              // write a sector from this logical block address
    usBlockCount = 1;                                                    // write a single block
  //fnDebugHex(ulSectorNumber, (sizeof(ulSectorNumber) | WITH_SPACE | WITH_LEADIN));
    fnSendMSD_host(UFI_WRITE_10);                                        // request the write
    fnWrite(USBPortID_host, ptrBuffer, 512);                             // write data (buffered on this bulk endpoint)
    iWait = 0;
    while (fnWrite(USBPortID_host, 0, 512) < 512) {                      // wait until the buffer has been transmitted before enabling IN tokens
    #if defined _WINDOWS
            fnSimInts(0);                                                // allow the USB interrupts to be simulated
    #endif
    }
    fnDriver(USBPortID_host, (RX_ON), 0);                                // enable IN polling so that we can receive data/status response (this will block until the trnsmission has completed)
    while (1) {                                                          // wait until the status stage has been received
        if (fnRead(USBPortID_host, (unsigned char *)&status_stage, sizeof(status_stage)) != 0) { // read status stage
            if ((status_stage.dCBWSignatureL == USBS_SIGNATURE) && (status_stage.dCBWTagL == ulTag)) {
              //fnDebugMsg(" OK\r\n");
                break;
            }
            else {
                fnDebugMsg(" Staus error\r\n");
                return UTFAT_DISK_READ_ERROR;
            }
        }
        else {
    #if defined _WINDOWS
            fnSimInts(0);                                                // allow the USB interrupts to be simulated
    #endif
            if (++iWait > MAX_USB_MSD_WRITE_WAIT) {                      // limit the maximum wait in case the memory stick fails
                fnDebugMsg(" TO\r\n");
                return UTFAT_DISK_READ_ERROR;
            }
        }
    }
    return UTFAT_SUCCESS;
}

extern int utCommitMSDSectorData(UTDISK *ptr_utDisk, void *ptrBuffer, unsigned long ulSectorNumber)
{
  //fnDebugMsg("W");
    return (utWriteMSD((unsigned char *)ptrBuffer, ulSectorNumber));
}

extern int utDeleteMSDSector(UTDISK *ptr_utDisk, unsigned long ulSectorNumber)
{
    unsigned long ulTemp[512/sizeof(unsigned long)];
  //fnDebugMsg("Del");
    uMemset(ulTemp, 0, sizeof(ulTemp));                                  // prepare a zeroed buffer to be written
    return (utWriteMSD((unsigned char *)ulTemp, ulSectorNumber));
}
#endif

// The USB interface is configured by opening the USB interface once for the default control endpoint 0,
// followed by an open of each endpoint to be used (each endpoint has its own handle). Each endpoint can use an optional call-back
// or can define a task to be woken on OUT frames. Transmission can use direct memory method or else an output buffer (size defined by open),
// and receptions can use an optional input buffer (size defined by open).
//
static void fnConfigureUSB(void)
{
    USBTABLE tInterfaceParameters;                                       // table for passing information to driver

    // Configure the control endpoint
    //
    tInterfaceParameters.Endpoint = 0;                                   // set USB default control endpoint for configuration
    tInterfaceParameters.usConfig = USB_HOST_MODE;                       // configure host mode of operation
    tInterfaceParameters.usb_callback = control_callback;                // call-back for control endpoint to enable class exchanges to be handled
    tInterfaceParameters.queue_sizes.TxQueueSize = 0;                    // no tx buffering on control endpoint
    tInterfaceParameters.queue_sizes.RxQueueSize = 0;                    // no rx buffering on control endpoint
    #if defined _KINETIS                                                 // {18}{25}{26}{29}{30}
    tInterfaceParameters.ucClockSource = INTERNAL_USB_CLOCK;             // use system clock and dividers
    #else
    tInterfaceParameters.ucClockSource = EXTERNAL_USB_CLOCK;             // use 48MHz crystal directly as USB clock (recommended for lowest jitter)
    #endif
    #if defined _LPC23XX || defined _LPC17XX                             // {14}
    tInterfaceParameters.ucEndPoints = 5;                                // due to fixed endpoint ordering in the LPC endpoints up to 5 are used
    #else
    tInterfaceParameters.ucEndPoints = NUMBER_OF_ENDPOINTS;              // number of endpoints, in addition to EP0, required by the configuration
    #endif
    tInterfaceParameters.owner_task = OWN_TASK;                          // local task receives USB state change events
    #if defined USE_USB_OTG_CHARGE_PUMP
    tInterfaceParameters.OTG_I2C_Channel = IICPortID;                    // let the driver open its own IIC interface, if not yet open
    #endif
    USB_control = fnOpen(TYPE_USB, 0, &tInterfaceParameters);            // open the default control endpoint with defined configurations (reserves resources but only control is active)
}
__PACK_OFF
#endif

