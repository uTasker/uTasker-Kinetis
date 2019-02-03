/**********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher

    ---------------------------------------------------------------------
    File:      usb_application.c - MSD
    Project:   uTasker project
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2016
    *********************************************************************
    This application is the same as the USB application in the uTaskerV1.4 project - state 20.7.2015,
    whereby the USB-MSD model is used to allow USB-MSD (on SD card) to be operated whilst in the boot loader

*/


/* =================================================================== */
/*                           include files                             */
/* =================================================================== */

#include "config.h"

#if (defined USB_INTERFACE && defined USE_USB_MSD && defined SDCARD_SUPPORT)

/* =================================================================== */
/*                          local definitions                          */
/* =================================================================== */

#define OWN_TASK                            TASK_USB

#if !defined USB_CDC_COUNT
    #define USB_CDC_COUNT                   1
#endif
#if !defined MODBUS_USB_CDC_COUNT
    #define MODBUS_USB_CDC_COUNT            0
#endif

#if !defined USB_SPEC_VERSION                                            // {28}
    #define USB_SPEC_VERSION                USB_SPEC_VERSION_1_1         // default is to report USB1.1 since it is equivalent to USB2.0 but requires one less descriptor exchange
#endif

#if defined USB_HOST_SUPPORT
    #define NUMBER_OF_ENDPOINTS             2                            // reserve 2 bulk endpoints

    #define STATUS_TRANSPORT                0x100
#endif

#define INCLUDE_USB_DEFINES
    #if defined USE_USB_CDC                                              // one or more CDC interfaces (with optional MSD, mouse and/or keyboard)
        #include "usb_cdc_descriptors.h"
    #elif defined USE_USB_MSD                                            // MSD (with optional mouse and/or keyboard)
        #include "usb_msd_descriptors.h"
    #elif defined USE_USB_HID_MOUSE                                      // HID mouse (with optional keyboard)
        #include "usb_mouse_descriptors.h"
    #elif defined USE_USB_HID_KEYBOARD                                   // HID keyboard
        #include "usb_keyboard_descriptors.h"
    #elif defined USE_USB_HID_RAW                                        // HID raw
        #include "usb_hid_raw_descriptors.h"
    #endif
#undef INCLUDE_USB_DEFINES

// Interrupt events
//
#define EVENT_LUN_READY                     1
#define EVENT_RETURN_PRESENT_UART_SETTING   10                           // 11, 12, etc. for additional USB-CDC interfaces
#define EVENT_COMMIT_NEW_UART_SETTING       20                           // 21, 22, etc. for additional USB-CDC interfaces

// Timer events
//
#define T_MOUSE_ACTION                      1
#define T_KEYBOARD_ACTION                   2
#define T_CHECK_DISK                        3
#define T_GET_STATUS                        4
#define T_GET_STATUS_TEST                   5
#define T_REPEAT_COMMAND                    6
#define T_REQUEST_LUN                       7

#if defined USE_USB_MSD
    #define NUMBER_OF_PARTITIONS            DISK_COUNT
#endif

#if defined _LITTLE_ENDIAN || defined _WINDOWS
    #define USBC_SIGNATURE   (0x43425355)                                // 'U' 'S' 'B' 'C'
    #define USBS_SIGNATURE   (0x53425355)                                // 'U' 'S' 'B' 'S'
#else
    #define USBC_SIGNATURE   (0x55534243)                                // 'U' 'S' 'B' 'C'
    #define USBS_SIGNATURE   (0x55534253)                                // 'U' 'S' 'B' 'S'
#endif


#define KEYBOARD_INTERVAL                   (DELAY_LIMIT)(0.02 * SEC)
#define MOUSE_INTERVAL                      (DELAY_LIMIT)(0.05 * SEC)

#define KEYBOARD_ENUMERATED                 1
#define KEYBOARD_IN_ACTIVE                  2

/* =================================================================== */
/*                       local structure definitions                   */
/* =================================================================== */

__PACK_ON                                                                // compilers using pragmas to control packing will start struct packing from here
#define INCLUDE_USB_CONFIG_DESCRIPTOR
// We define the contents of the configuration descriptor used for our specific device and then set its contents
//
    #if defined USE_USB_CDC                                              // one or more CDC interfaces (with optional MSD, mouse and/or keyboard)
        #include "usb_cdc_descriptors.h"
    #elif defined USE_USB_MSD                                            // MSD (with optional mouse and/or keyboard)
        #include "usb_msd_descriptors.h"
    #elif defined USE_USB_HID_MOUSE                                      // HID mouse (with optional keyboard)
        #include "usb_mouse_descriptors.h"
    #elif defined USE_USB_HID_KEYBOARD                                   // HID keyboard
        #include "usb_keyboard_descriptors.h"
    #elif defined USE_USB_HID_RAW                                        // HID raw
        #include "usb_hid_raw_descriptors.h"
    #endif
#undef INCLUDE_USB_CONFIG_DESCRIPTOR


/* =================================================================== */
/*                             constants                               */
/* =================================================================== */

#if defined USB_HOST_SUPPORT
    static const USB_HOST_DESCRIPTOR get_max_lum = {
        (STANDARD_DEVICE_TO_HOST | REQUEST_INTERFACE_CLASS),             // 0xa1 request class specific interface
        GET_MAX_LUN,                                                     // 0xfe
        {LITTLE_SHORT_WORD_BYTES(0)},
        {LITTLE_SHORT_WORD_BYTES(0)},
        {LITTLE_SHORT_WORD_BYTES(1)},
    };
#endif

#define INCLUDE_USB_CLASS_CONSTS
    #if defined USE_USB_MSD
        #include "usb_msd_descriptors.h"
    #endif
    #if defined USE_USB_HID_MOUSE
        #include "usb_mouse_descriptors.h"
    #endif
    #if defined USE_USB_HID_KEYBOARD
        #include "usb_keyboard_descriptors.h"
    #endif
    #if defined USE_USB_HID_RAW
        #include "usb_hid_raw_descriptors.h"
    #endif
#undef INCLUDE_USB_CLASS_CONSTS

#define INCLUDE_USB_DEVICE_DESCRIPTOR
    #if defined USE_USB_CDC                                              // one or more CDC interfaces (with optional MSD, mouse and/or keyboard)
        #include "usb_cdc_descriptors.h"
    #elif defined USE_USB_MSD                                            // MSD (with optional mouse and/or keyboard)
        #include "usb_msd_descriptors.h"
    #elif defined USE_USB_HID_MOUSE                                      // HID mouse (with optional keyboard)
        #include "usb_mouse_descriptors.h"
    #elif defined USE_USB_HID_KEYBOARD                                   // HID keyboard
        #include "usb_keyboard_descriptors.h"
    #elif defined USE_USB_HID_RAW                                        // HID raw
        #include "usb_hid_raw_descriptors.h"
    #endif
#undef INCLUDE_USB_DEVICE_DESCRIPTOR

/* =================================================================== */
/*                     global variable definitions                     */
/* =================================================================== */

#if defined USE_USB_HID_KEYBOARD
    QUEUE_HANDLE keyboardQueue = NO_ID_ALLOCATED;
#endif

/* =================================================================== */
/*                      local variable definitions                     */
/* =================================================================== */

static QUEUE_HANDLE USB_control = NO_ID_ALLOCATED;                       // USB default control endpoint handle
static unsigned short usExpectedData = 0;                                // used by the control endpoint to collect data
static unsigned char  ucCollectingMode = 0;

#if defined USB_HOST_SUPPORT
    static QUEUE_HANDLE USBPortID_host = NO_ID_ALLOCATED;
    static unsigned char ucMSDBulkInEndpoint = 0;                        // the endpoint that is to be used as MSD bulk IN endpoint
    static unsigned short usMSDBulkInEndpointSize = 0;
    static unsigned char ucMSDBulkOutEndpoint = 0;                       // the endpoint that is to be used as MSD bulk OUT endpoint
    static unsigned short usMSDBulkOutEndpointSize = 0;
#endif
#if defined USE_USB_CDC                                                  // local variables required by USB-CDC
    static CDC_PSTN_LINE_CODING uart_setting[USB_CDC_COUNT];             // use a static struct to ensure that non-buffered transmission remains stable. Use also to receive new settings to
    static QUEUE_HANDLE USBPortID_interrupt[USB_CDC_COUNT] = {NO_ID_ALLOCATED}; // interrupt endpoint (not actually used)
    #if !defined MODBUS_USB_SLAVE                                        // {11}
    static QUEUE_HANDLE USBPortID_comms[USB_CDC_COUNT] = {NO_ID_ALLOCATED}; // USB port endpoint handle(s)
    #endif
    #if USB_CDC_COUNT > 1
        static QUEUE_HANDLE CDCSerialPortID[USB_CDC_COUNT] = {NO_ID_ALLOCATED};
        static UART_MODE_CONFIG CDC_UART_SerialMode[USB_CDC_COUNT - 1];
        static unsigned char ucCDC_UART_SerialSpeed[USB_CDC_COUNT - 1];
    #endif
#endif
#if defined USE_USB_HID_MOUSE
    static QUEUE_HANDLE USBPortID_HID_mouse = NO_ID_ALLOCATED;           // USB handle for HID mouse communication
#endif
#if defined USE_USB_HID_RAW
    static QUEUE_HANDLE USBPortID_HID_raw = NO_ID_ALLOCATED;            // USB handle for HID raw communication
#endif
#if defined USE_USB_HID_KEYBOARD                                         // {34}
    static QUEUE_HANDLE USBPortID_HID_kb = NO_ID_ALLOCATED;              // USB handle for HID keyboard communication
    static CHAR cLastInput = 0;
    static int iUSB_keyboard_state = 0;
#endif
#if defined USE_USB_MSD                                                  // local variables required by USB-MSD
    static QUEUE_HANDLE USBPortID_MSD = NO_ID_ALLOCATED;                 // USB handle for MSD communication
    #if !defined SDCARD_SUPPORT && !defined SPI_FLASH_FAT && !defined FLASH_FAT
        #error USBMSD needs utFAT enabled !!
    #endif
    static unsigned long ulLogicalBlockAdr = 0;                          // present logical block address (shared between read and write)
    static unsigned long ulReadBlock = 0;                                // the outstanding blocks to be read from the media
    static unsigned long ulWriteBlock = 0;                               // the outstanding blocks to be written to the media
    static int iContent = 0;
    static const UTDISK *ptrDiskInfo[DISK_COUNT] = {0};
    static USB_MASS_STORAGE_CSW csw = {{'U', 'S', 'B', 'S'}, {0}, {0}, CSW_STATUS_COMMAND_PASSED };
    static unsigned char ucActiveLUN = DISK_D;
#endif

#if defined USB_STRING_OPTION && defined USB_RUN_TIME_DEFINABLE_STRINGS
    static USB_STRING_DESCRIPTOR *SerialNumberDescriptor;                // string built up to contain a variable serial number
#endif

#if defined USB_HOST_SUPPORT
    static unsigned char ucDeviceLUN = 0;
    static int iUSB_MSD_OpCode = 0;
    static unsigned long ulTag =0 ;
    static unsigned long ulLBA = 0;
    static unsigned long ulBlockByteCount = 0;
    static unsigned short usBlockCount = 0;
#endif


/* =================================================================== */
/*                      local function definitions                     */
/* =================================================================== */

static void fnConfigureUSB(void);                                        // routine to open and configure USB interface
#if defined USB_STRING_OPTION && defined USB_RUN_TIME_DEFINABLE_STRINGS
    static void fnSetSerialNumberString(CHAR *ptrSerialNumber);
#endif
#if defined USE_USB_CDC && defined SERIAL_INTERFACE && defined USB_SERIAL_CONNECTIONS && (USB_CDC_COUNT > 1)
    static void fnOpenUSB_UARTs(int iInterface);                         // configure and open UARTs to be used with USB CDC connection
#endif
#if defined USE_USB_MSD
    static void fnContinueMedia(void);
    static unsigned char *fnGetPartition(unsigned char ucLUN, QUEUE_TRANSFER *length);
#endif
#if defined USE_USB_CDC
    static void fnReturnUART_settings(int iInterface);
    static void fnNewUART_settings(unsigned char *ptrData, unsigned short usLength, unsigned short usExpected, int iInterface);
    static void fnComitt_UART(int iInterface);
#endif
#if defined USB_HOST_SUPPORT
    static void fnConfigureApplicationEndpoints(unsigned char ucConfiguration);
    static void fnRequestLUN(void);
    static int fnSendMSD_host(unsigned char ucOpcode);
    static unsigned char fnDisplayDeviceInfo(void);
    static void fnDeviceLun(unsigned char *ptrData, unsigned short usLength, unsigned short usExpected);
#endif
#if defined USE_USB_HID_KEYBOARD                                         // {34}
    static void fnSetKeyboardOutput(unsigned char ucOutputs);
    static void fnKeyboardInput(void);
#endif


/* =================================================================== */
/*                                task                                 */
/* =================================================================== */


// USB task
//
extern void fnTaskUSB(TTASKTABLE *ptrTaskTable)
{
    QUEUE_HANDLE PortIDInternal = ptrTaskTable->TaskID;                  // queue ID for task input
    unsigned char ucInputMessage[LARGE_MESSAGE];                         // reserve space for receiving messages
#if defined USE_USB_HID_MOUSE                                            // {23}
    static int iUSB_mouse_state = 0;
#endif
#if defined USB_HOST_SUPPORT || defined USE_USB_MSD || defined USE_USB_CDC
    QUEUE_TRANSFER Length;
#endif
#if defined USE_USB_CDC                                                  // {11}
    #if (USB_CDC_COUNT > 1) && ((USB_CDC_COUNT > (MODBUS_USB_CDC_COUNT + 1)) || !defined USE_MAINTENANCE)
    int iCDC_input;
    #endif
#endif

    if (USB_control == NO_ID_ALLOCATED) {                                // initialisation
#if defined USE_USB_HID_KEYBOARD
        keyboardQueue = fnOpen(TYPE_FIFO, FOR_I_O, (FIFO_LENGTH)64);     // open a FIFO queue (of 64 bytes length) for receiving input to be sent from the keyboard
#endif
#if (defined KWIKSTIK || defined TWR_K40X256 || defined TWR_K40D100M || defined TWR_K53N512 || defined FRDM_KL46Z || defined FRDM_KL43Z || defined TWR_KL43Z48M || defined TWR_KL46Z48M) // {3}{6}
        CONFIGURE_SLCD();
#endif
#if defined USB_STRING_OPTION && defined USB_RUN_TIME_DEFINABLE_STRINGS  // if dynamic strings are supported, prepare a specific serial number ready for enumeration
        fnSetSerialNumberString(temp_pars->temp_parameters.cDeviceIDName); // construct a serial number string for USB use
#endif
#if defined USE_USB_CDC && defined SERIAL_INTERFACE && defined USB_SERIAL_CONNECTIONS && (USB_CDC_COUNT > 1)
        fnOpenUSB_UARTs(0);                                              // configure and open additional UARTs to be used with USB CDC connection
#endif
        fnConfigureUSB();                                                // configure the USB interface
#if defined MODBUS_USB_SLAVE                                             // {11}
        fnInitModbus();                                                  // initialise MODBUS since the USB handle is now ready
#endif
#if defined USE_USB_HID_MOUSE                                            // {23}
        CONFIGURE_MOUSE_INPUTS();
#endif
#if defined USE_USB_MSD
        do {
            ptrDiskInfo[ucActiveLUN] = fnGetDiskInfo(ucActiveLUN);       // get a pointer to each disk information for local use
        } while (++ucActiveLUN < DISK_COUNT);
#endif
    }

    while (fnRead(PortIDInternal, ucInputMessage, HEADER_LENGTH) != 0) { // check task input queue
        switch (ucInputMessage[MSG_SOURCE_TASK]) {                       // switch depending on source
        case TIMER_EVENT:
#if defined USB_HOST_SUPPORT
            if (T_GET_STATUS == ucInputMessage[MSG_TIMER_EVENT]) {
                iUSB_MSD_OpCode |= STATUS_TRANSPORT;
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
//ucDeviceLUN = 1; //WARNING - if the LUM is not requested the host state machine stays in an unexpected state (to improve in host state-machine)
//fnInterruptMessage(OWN_TASK, EVENT_LUN_READY);
            }
#endif
#if defined USE_USB_HID_MOUSE                                            // {23}
            if (iUSB_mouse_state != 0) {                                 // T_MOUSE_ACTION assumed
                static unsigned char ucMouseLastState[4] = {0, 0, 0, 0}; // {24}
                static unsigned char ucMouseState[4] = {0, 0, 0, 0};     // this must be static since the interrupt IN endpoint uses non-buffered transmission
                if (MOUSE_LEFT_CLICK()) {
                    ucMouseState[0] |= 0x01;                             // mouse left key held down
                }
                else {
                    ucMouseState[0] &= ~0x01;                            // mouse left key releases
                }
                if (MOUSE_RIGHT()) {
                    ucMouseState[1]++;                                   // increase x coordinate
                }
                else if (MOUSE_LEFT()) {
                    ucMouseState[1]--;                                   // decrease x coordinate
                }
                else {
                    ucMouseState[1] = 0;
                }
                if (MOUSE_UP()) {
                    ucMouseState[2]++;                                   // increase y coordinate
                }
                else if (MOUSE_DOWN()) {
                    ucMouseState[2]--;                                   // decrease y coordinate
                }
                else {
                    ucMouseState[2] = 0;
                }
                if (uMemcmp(ucMouseLastState, ucMouseState, sizeof(ucMouseState)) != 0) { // {24} if change to the mouse state
                    fnWrite(USBPortID_HID_mouse, (unsigned char *)&ucMouseState, sizeof(ucMouseState)); // prepare interrupt IN (non-buffered)
                    uMemcpy(ucMouseLastState, ucMouseState, sizeof(ucMouseState));
                }
                uTaskerMonoTimer(OWN_TASK, MOUSE_INTERVAL, T_MOUSE_ACTION); // next mouse state poll
            }
#endif
#if defined USE_USB_HID_KEYBOARD  && !defined IN_COMPLETE_CALLBACK       // {34}
            fnKeyboardInput();                                           // T_KEYBOARD_ACTION assumed - handle keyboard input
#endif
            break;
        case INTERRUPT_EVENT:                                            // interrupt event without data
            switch (ucInputMessage[MSG_INTERRUPT_EVENT]) {               // specific interrupt event type
            case EVENT_USB_RESET:                                        // active USB connection has been reset
#if defined USE_USB_HID_MOUSE                                            // {23}
                iUSB_mouse_state = 0;                                    // mark that the mouse function is not active
                uTaskerStopTimer(OWN_TASK);
#endif
#if defined USE_USB_HID_KEYBOARD                                         // {34}
                iUSB_keyboard_state = 0;                                 // mark that the keyboard function is not active
                cLastInput = 0;
    #if !defined IN_COMPLETE_CALLBACK
                uTaskerStopTimer(OWN_TASK);
    #endif
    #if defined USE_MAINTENANCE
                usUSB_state &= ~(ES_USB_KEYBOARD_MODE);                  // keyboard no longer connected
    #endif
                if (keyboardQueue != NO_ID_ALLOCATED) {                  // flush the keyboard fifo
    #if !defined SUPPORT_FLUSH
                    unsigned char ucFlush;
                    do {
                    } while (fnRead(keyboardQueue, &ucFlush, 1) != 0);
    #else
                    fnFlush(keyboardQueue, FLUSH_RX);
    #endif
                }
#endif
#if defined USE_USB_MSD
                ulWriteBlock = 0;                                        // reset local variables on each reset
                ulReadBlock = 0;
                iContent = 0;
#endif
#if defined USE_USB_CDC && defined USE_MAINTENANCE
                if (usUSB_state != ES_NO_CONNECTION) {                   // if the USB connection was being used for debug menu, restore previous interface
    #if defined SERIAL_INTERFACE && defined DEMO_UART                    // {8}
                    DebugHandle = SerialPortID;
    #else
                    DebugHandle = NETWORK_HANDLE;
    #endif
                    fnGotoNextState(ES_NO_CONNECTION);
                    usUSB_state = ES_NO_CONNECTION;
                }
#endif
                fnDebugMsg("USB Reset\n\r");                             // display that the USB bus has been reset
                DEL_USB_SYMBOL();                                        // {21} optionally display the new USB state
                break;

            case EVENT_USB_SUSPEND:                                      // a suspend condition has been detected. A bus powered device should reduce consumption to <= 500uA or <= 2.5mA (high power device)
#if defined USE_USB_CDC && defined USE_MAINTENANCE
                if (usUSB_state != ES_NO_CONNECTION) {                   // if the USB connection was being used for debug menu, restore previous interface
    #if defined SERIAL_INTERFACE && defined DEMO_UART                    // {8}
                    DebugHandle = SerialPortID;
    #else
                    DebugHandle = NETWORK_HANDLE;
    #endif
                    fnGotoNextState(ES_NO_CONNECTION);
                    usUSB_state = ES_NO_CONNECTION;
                }
#endif
#if defined USE_USB_HID_MOUSE
                iUSB_mouse_state = 0;                                    // mark that the mouse function is not active
                uTaskerStopTimer(OWN_TASK);                              // stop mouse polling timer
#endif
#if defined USE_USB_HID_KEYBOARD                                         // {34}
                iUSB_keyboard_state = 0;                                 // mark that the keyboard function is not active
                cLastInput = 0;
    #if defined IN_COMPLETE_CALLBACK
                uTaskerStopTimer(OWN_TASK);                              // stop keyboard polling timer
    #endif
    #if defined USE_MAINTENANCE
                usUSB_state &= ~(ES_USB_KEYBOARD_MODE);                  // keyboard no longer connected
    #endif
#endif
                fnSetUSBConfigState(USB_DEVICE_SUSPEND, 0);              // set all endpoint states to suspended
                fnDebugMsg("USB Suspended\n\r");
                DEL_USB_SYMBOL();                                        // {21} optionally display the new USB state
                break;

            case EVENT_USB_RESUME:                                       // a resume sequence has been detected so full power consumption can be resumed
                fnSetUSBConfigState(USB_DEVICE_RESUME, 0);               // remove suspended state from all endpoints
                fnDebugMsg("USB Resume\n\r");
                SET_USB_SYMBOL();                                        // {21} optionally display the new USB state
#if defined USE_USB_HID_MOUSE                                            // {23}
                iUSB_mouse_state = 1;                                    // mark that the mouse function is active
                uTaskerMonoTimer(OWN_TASK, MOUSE_INTERVAL, T_MOUSE_ACTION); // start polling the USB state since the USB connection has resumed
#endif
#if defined USE_USB_HID_KEYBOARD                                         // {34}
                iUSB_keyboard_state = KEYBOARD_ENUMERATED;               // mark that the keyboard function is active
    #if !defined IN_COMPLETE_CALLBACK
                uTaskerMonoTimer(OWN_TASK, KEYBOARD_INTERVAL, T_KEYBOARD_ACTION); // start polling the USB state since the USB connection has resumed
    #endif
#endif
                break;
#if defined USE_USB_HID_KEYBOARD && defined IN_COMPLETE_CALLBACK
            case EVENT_USB_KEYBOARD_INPUT:                               // the user has entered new keyboard input
                fnKeyboardInput();                                       // handle the keyboard input
                break;
#endif
#if defined USE_USB_MSD
            case TX_FREE:                                                // the USB tx buffer has enough space to accept more data
                fnContinueMedia();                                       // the output buffer has space so continue with data transfer
                break;
#endif
#if defined USB_HOST_SUPPORT                                             // {32}
            case EVENT_USB_DETECT_HS:
            case EVENT_USB_DETECT_LS:
            case EVENT_USB_DETECT_FS:
                fnDebugMsg("USB ");
                if (EVENT_USB_DETECT_HS == ucInputMessage[MSG_INTERRUPT_EVENT]) {
                    fnDebugMsg("H");
                }
                else if (EVENT_USB_DETECT_FS == ucInputMessage[MSG_INTERRUPT_EVENT]) {
                    fnDebugMsg("F");
                }
                else {
                    fnDebugMsg("L");
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
                fnSendMSD_host(UFI_INQUIRY);                             // continue by requesting information
                break;
#endif
#if defined USE_USB_CDC
    #if (USB_CDC_COUNT > 1)
            default:
                if (ucInputMessage[MSG_INTERRUPT_EVENT] >= EVENT_COMMIT_NEW_UART_SETTING) {
                    fnComitt_UART(ucInputMessage[MSG_INTERRUPT_EVENT] - EVENT_COMMIT_NEW_UART_SETTING); // set the serial interface characteristics
                }
                else if (ucInputMessage[MSG_INTERRUPT_EVENT] >= EVENT_RETURN_PRESENT_UART_SETTING) {
                    fnReturnUART_settings(ucInputMessage[MSG_INTERRUPT_EVENT] - EVENT_RETURN_PRESENT_UART_SETTING); // return the present serial interface settings
                }
                break;
    #else
            case EVENT_COMMIT_NEW_UART_SETTING:                          // the CDC host has sent new UART settings - we commit them here
                fnComitt_UART(0);                                        // set the serial interface characteristics
                break;
            case EVENT_RETURN_PRESENT_UART_SETTING:                      // the CDC class host is requesting our serial settings
                fnReturnUART_settings(0);                                // return the present serial interface settings
                break;
    #endif
#endif
            }                                                            // end interrupt event switch
            break;

        case TASK_USB:                                                   // USB interrupt handler is requesting us to perform work offline
            {
                unsigned char ucEndpointConfiguration;
                fnRead(PortIDInternal, &ucInputMessage[MSG_CONTENT_COMMAND], ucInputMessage[MSG_CONTENT_LENGTH]); // get the content
                ucEndpointConfiguration = ucInputMessage[MSG_CONTENT_COMMAND + 1];
                switch (ucInputMessage[MSG_CONTENT_COMMAND]) {           // switch on the event type
#if defined USB_HOST_SUPPORT                                             // {32}
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
                    uTaskerMonoTimer(OWN_TASK, (DELAY_LIMIT)(0.05 * SEC), T_GET_STATUS); // collect the status after a short delay
                    break;
                case E_USB_HOST_ERROR_REPORT:
                    fnDebugMsg("USB Host error: EP-");
                    fnDebugDec((ucEndpointConfiguration >> 4), 0);
                    fnDebugMsg(" Code: ");
                    fnDebugDec((ucEndpointConfiguration & 0x0f), WITH_CR_LF);
                    break;
#endif
                case E_USB_ACTIVATE_CONFIGURATION:
                    fnDebugMsg("Enumerated (");                          // the interface has been activated and enumeration has completed
                    fnDebugDec(ucEndpointConfiguration, 0);              // {10} the configuration
                    fnDebugMsg(")\n\r");
                    SET_USB_SYMBOL();                                    // {21} display connection in LCD, on LED etc.
#if defined USB_HOST_SUPPORT                                             // {32}
                    fnConfigureApplicationEndpoints(ucEndpointConfiguration); // configure endpoints according to configuration
                    iUSB_MSD_OpCode = 0;
    /* WARNING - < 1s doesn't allow Cruzer 2 to work yet - since it requires multiple stalls to be cleared *///uTaskerMonoTimer(OWN_TASK, (DELAY_LIMIT)(0.20 * SEC), T_REQUEST_LUN); // request LUN after a delay to ensure that the stick is ready
                    fnRequestLUN();                                      // the first thing that the MSD host does is request the number of logical units that the disk drive has
#endif
#if defined USE_USB_HID_MOUSE
                    iUSB_mouse_state = 1;                                // mark that the mouse function is active
                    uTaskerMonoTimer(OWN_TASK, MOUSE_INTERVAL, T_MOUSE_ACTION); // start polling the mouse state
#endif
#if defined USE_USB_HID_KEYBOARD                                         // {34}
                    iUSB_keyboard_state = KEYBOARD_ENUMERATED;           // mark that the keyboard function is enumerated and active
    #if !defined IN_COMPLETE_CALLBACK
                    uTaskerMonoTimer(OWN_TASK, KEYBOARD_INTERVAL, T_KEYBOARD_ACTION); // start polling the USB state since the USB connection has resumed
    #endif
#endif
                }
                break;
            }
            break;

        default:
            break;
        }
    }

#if defined USB_HOST_SUPPORT
    if (USBPortID_host == NO_ID_ALLOCATED) {                             // don't check bulk input queue until host is configured
        return;
    }
    while (fnMsgs(USBPortID_host) != 0) {                                // reception from host IN endpoint
#if defined _WINDOWS
//utWriteMSD(ucInputMessage, 123); // quick test
#endif
        Length = fnRead(USBPortID_host, ucInputMessage, 64);             // read the content (up to 64 bytes content each time)
        switch (iUSB_MSD_OpCode) {
        case UFI_READ_10:
            {   // Simplified for first test - no overflow checking
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
              //CBW_CAPACITY_LIST *ptrCapacityList = (CBW_CAPACITY_LIST *)ucInputMessage;
                fnDriver(USBPortID_host, (RX_ON), 0);                    // enable IN polling so that we can receive the status transport
                iUSB_MSD_OpCode |= STATUS_TRANSPORT;
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
              //CBW_READ_CAPACITY_DATA *ptrCapacity = (CBW_READ_CAPACITY_DATA *)ucInputMessage;
                fnDriver(USBPortID_host, (RX_ON), 0);                    // enable IN polling so that we can receive the status transport
                iUSB_MSD_OpCode |= STATUS_TRANSPORT;
            }
            break;
        case (UFI_INQUIRY | STATUS_TRANSPORT):                           // expecting status transport after inquiry data
        case (UFI_READ_FORMAT_CAPACITY | STATUS_TRANSPORT):              // expecting status transport after reading format capacities
        case (UFI_REQUEST_SENSE | STATUS_TRANSPORT):                     // expecting status transport after reading requesting sense
        case (UFI_READ_CAPACITY | STATUS_TRANSPORT):                     // expecting status transport after reading capacity
        case (UFI_READ_10 | STATUS_TRANSPORT):                           // expecting status transport after reading data
            {
                USB_MASS_STORAGE_CBW_LW *ptrStatus = (USB_MASS_STORAGE_CBW_LW *)ucInputMessage;
                fnDebugMsg("Status transport - ");
                if ((ptrStatus->dCBWSignatureL == USBS_SIGNATURE) && (ptrStatus->dCBWTagL == ulTag)) { // check the status transport's signature and tag
                    if (ptrStatus->dmCBWFlags == CSW_STATUS_COMMAND_PASSED) { // check that there are no errors reported
                        fnDebugMsg("Passed\r\n");
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
                        case (UFI_READ_10 | STATUS_TRANSPORT):
                            iUSB_MSD_OpCode = 0;
                            fnDebugMsg("READ:");
                            break;
                        }
                    }
                    else {
                        fnDebugMsg("Failed\r\n");
                        iUSB_MSD_OpCode &= ~(STATUS_TRANSPORT);
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
#endif

#if defined USE_USB_MSD        
    while (fnMsgs(USBPortID_MSD) != 0) {                                 // handle USB-MSD host receptions
        if (ulWriteBlock != 0) {                                         // write data expected
            static unsigned char ucBuffer[512];                          // intermediate buffer to collect each sector content
            Length = fnRead(USBPortID_MSD, &ucBuffer[iContent], (QUEUE_TRANSFER)(512 - iContent)); // read the content directly to the intermediate buffer
            ulWriteBlock -= Length;
            iContent += Length;
            if (iContent >= 512) {                                       // input buffer is complete
                fnWriteSector(ucActiveLUN, ucBuffer, ulLogicalBlockAdr++); // commit the buffer content to the media
                iContent = 0;                                            // reset intermediate buffer
                if (ulWriteBlock != 0) {                                 // block not complete
                    uTaskerStateChange(OWN_TASK, UTASKER_ACTIVATE);      // {17} yield after a write when further data is expected but schedule again immediately to read any remaining queue content
                    return;
                }
            }
            if (ulWriteBlock != 0) {                                     // more data expected
                continue;
            }                                                            // allow CSW to be sent after a complete transfer has completed
        }
        else {
          //unsigned long ulTransferLength;
            USB_MASS_STORAGE_CBW *ptrCBW;
            Length = fnRead(USBPortID_MSD, ucInputMessage, LARGE_MESSAGE); // read the content
            ptrCBW = (USB_MASS_STORAGE_CBW *)ucInputMessage;
          //ulTransferLength = (ptrCBW->dCBWDataTransferLength[0] | (ptrCBW->dCBWDataTransferLength[1] << 8) | (ptrCBW->dCBWDataTransferLength[2] << 16) | (ptrCBW->dCBWDataTransferLength[3] << 24));
            uMemcpy(&csw.dCBWTag, ptrCBW->dCBWTag, sizeof(csw.dCBWTag)); // save the tag
            csw.dCSWDataResidue[0] = 0;
            csw.dCSWDataResidue[1] = 0;
            csw.dCSWDataResidue[2] = 0;
            csw.dCSWDataResidue[3] = 0;
            csw.bCSWStatus = CSW_STATUS_COMMAND_PASSED;                  // set for success
            fnFlush(USBPortID_MSD, FLUSH_TX);                            // always flush the tx buffer to ensure message alignment in buffer before responding
            ucActiveLUN = ptrCBW->dCBWLUN;                               // the logical unit number to use
            if (ptrCBW->dmCBWFlags & CBW_IN_FLAG) {
                switch (ptrCBW->CBWCB[CBW_OperationCode]) {
                case UFI_FORMAT_UNIT:
                    break;
                case UFI_INQUIRY:
                    {
                        CBW_INQUIRY_COMMAND *ptrInquiry = (CBW_INQUIRY_COMMAND *)ptrCBW->CBWCB;
                        unsigned char ucLogicalUnitNumber = (ptrInquiry->ucLogicalUnitNumber_EVPD >> 5); // the logical unit number being requested
                        QUEUE_TRANSFER inquiry_length;
                        unsigned char *ptrInquiryData = fnGetPartition(ucLogicalUnitNumber, &inquiry_length);
                        if (ptrInquiryData == 0) {                       // partition doesn't exist
                            csw.bCSWStatus = CSW_STATUS_COMMAND_FAILED;
                        }
                        else {
                            fnWrite(USBPortID_MSD, ptrInquiryData, inquiry_length); // respond to the request and then return 
                        }
                    }
                    break;
                case UFI_MODE_SENSE_6:                                   // {15}
                    {
                        MODE_PARAMETER_6 SelectDataWP;
                        uMemcpy(&SelectDataWP, &SelectData, sizeof(SelectData));
                        if (ptrDiskInfo[ucActiveLUN]->usDiskFlags & WRITE_PROTECTED_SD_CARD) {
                            SelectDataWP.ucWP_DPOFUA = PAR6_WRITE_PROTECTED; // the medium is write protected
                        }
                        fnWrite(USBPortID_MSD, (unsigned char *)&SelectDataWP, sizeof(SelectData)); // respond to the request and then return 
                    }
                    break;
                case UFI_START_STOP:
                case UFI_MODE_SELECT:
              //case UFI_MODE_SENSE:
                case UFI_PRE_ALLOW_REMOVAL:
                    break;
                case UFI_READ_10:
                case UFI_READ_12:
                    {                                
                        CBW_READ_10 *ptrRead = (CBW_READ_10 *)ptrCBW->CBWCB;
                      //ulLogicalBlockAdr = ((ptrRead->ucLogicalBlockAddress[0] << 24) | (ptrRead->ucLogicalBlockAddress[1] << 16) | (ptrRead->ucLogicalBlockAddress[2] << 8) | ptrRead->ucLogicalBlockAddress[3]);
                        if (ptrRead->ucOperationCode == UFI_READ_12) {
                            CBW_READ_12 *ptrRead = (CBW_READ_12 *)ptrCBW->CBWCB;
                            ulReadBlock = ((ptrRead->ucTransferLength[0] << 24) | (ptrRead->ucTransferLength[1] << 16) | (ptrRead->ucTransferLength[2] << 8) | ptrRead->ucTransferLength[3]);
                        }
                        else {
                            ulReadBlock = ((ptrRead->ucTransferLength[0] << 8) | ptrRead->ucTransferLength[1]); // the total number of blocks to be returned
                        }
                        fnContinueMedia();
                        continue;                                        // the transfer has not completed so don't send termination stage yet
                    }
                    break;
                case UFI_READ_CAPACITY:
                    {
                        CBW_READ_CAPACITY_DATA formatData;
                        unsigned long ulLastSector = (ptrDiskInfo[ucActiveLUN]->ulSD_sectors - 1);
                        formatData.ucBlockLengthInBytes[0] = 0;
                        formatData.ucBlockLengthInBytes[1] = 0;
                        formatData.ucBlockLengthInBytes[2] = (unsigned char)(ptrDiskInfo[ucActiveLUN]->utFAT.usBytesPerSector >> 8);
                        formatData.ucBlockLengthInBytes[3] = (unsigned char)(ptrDiskInfo[ucActiveLUN]->utFAT.usBytesPerSector);
                        formatData.ucLastLogicalBlockAddress[0] = (unsigned char)(ulLastSector >> 24);
                        formatData.ucLastLogicalBlockAddress[1] = (unsigned char)(ulLastSector >> 16);
                        formatData.ucLastLogicalBlockAddress[2] = (unsigned char)(ulLastSector >> 8);
                        formatData.ucLastLogicalBlockAddress[3] = (unsigned char)(ulLastSector);
                        fnWrite(USBPortID_MSD, (unsigned char *)&formatData, sizeof(formatData));
                    }
                    break;
                case UFI_READ_FORMAT_CAPACITY:
                    {
                        CBW_FORMAT_CAPACITIES *ptrCapacities = (CBW_FORMAT_CAPACITIES *)ptrCBW->CBWCB;
                        unsigned short usLengthAccepted = ((ptrCapacities->ucAllocationLength[0] << 8) | (ptrCapacities->ucAllocationLength[1]));
                        CBW_CAPACITY_LIST mediaCapacity;
                        uMemcpy(&mediaCapacity, &formatCapacityNoMedia, sizeof(CBW_CAPACITY_LIST)); // assume no disk
                        if (ptrDiskInfo[ucActiveLUN]->usDiskFlags & (DISK_MOUNTED | DISK_UNFORMATTED)) { // {16}
                            if (ptrDiskInfo[ucActiveLUN]->usDiskFlags & DISK_FORMATTED) {
                                mediaCapacity.capacityDescriptor.ucDescriptorCode = DESC_CODE_FORMATTED_MEDIA;
                            }
                            else {
                                mediaCapacity.capacityDescriptor.ucDescriptorCode = DESC_CODE_UNFORMATTED_MEDIA;
                            }
                            mediaCapacity.capacityDescriptor.ucNumberOfBlocks[3] = (unsigned char)ptrDiskInfo[ucActiveLUN]->ulSD_sectors;
                            mediaCapacity.capacityDescriptor.ucNumberOfBlocks[2] = (unsigned char)(ptrDiskInfo[ucActiveLUN]->ulSD_sectors >> 8);
                            mediaCapacity.capacityDescriptor.ucNumberOfBlocks[1] = (unsigned char)(ptrDiskInfo[ucActiveLUN]->ulSD_sectors >> 16);
                            mediaCapacity.capacityDescriptor.ucNumberOfBlocks[0] = (unsigned char)(ptrDiskInfo[ucActiveLUN]->ulSD_sectors >> 24);
                        }                                
                        if (usLengthAccepted > sizeof(CBW_CAPACITY_LIST)) {
                            usLengthAccepted = sizeof(CBW_CAPACITY_LIST);
                        }
                        fnWrite(USBPortID_MSD, (unsigned char *)&mediaCapacity, usLengthAccepted);
                    }
                    break;
                case UFI_REQUEST_SENSE:
                    {
                        CBW_RETURN_SENSE_DATA present_sense_data;
                        uMemcpy(&present_sense_data, &sense_data_OK, sizeof(sense_data_OK));
                        if ((ptrDiskInfo[ucActiveLUN]->usDiskFlags & (DISK_MOUNTED | DISK_UNFORMATTED)) == 0) {
                            present_sense_data.ucValid_ErrorCode = (CURRENT_ERRORS); // set that the disk is presently in a non-useable state
                            present_sense_data.ucSenseKey = SENSE_NOT_READY;
                            present_sense_data.ucAdditionalSenseCode = DESC_MEDIUM_NOT_PRESENT;
                        }
                        fnWrite(USBPortID_MSD, (unsigned char *)&present_sense_data, sizeof(present_sense_data));
                    }
                    break;
                case UFI_REZERO_UNIT:
                case UFI_SEEK_10:
                case UFI_SEND_DIAGNOSTIC:
                case UFI_VERIFY:
                case UFI_WRITE_AND_VERIFY:
                default:
                    break;
                }
            }
            else {                                                       // OUT types
                switch (ptrCBW->CBWCB[CBW_OperationCode]) {
                case UFI_TEST_UNIT_READY:
                    if ((ptrDiskInfo[ucActiveLUN]->usDiskFlags & (DISK_MOUNTED | DISK_UNFORMATTED)) == 0) { // if the partition is not ready
                        csw.bCSWStatus = CSW_STATUS_COMMAND_FAILED;
                    }
                    break;
                case UFI_WRITE_10:
                case UFI_WRITE_12:
                    {
                        CBW_WRITE_10 *ptrWrite = (CBW_WRITE_10 *)ptrCBW->CBWCB;
                      //ulLogicalBlockAdr = ((ptrWrite->ucLogicalBlockAddress[0] << 24) | (ptrWrite->ucLogicalBlockAddress[1] << 16) | (ptrWrite->ucLogicalBlockAddress[2] << 8) | ptrWrite->ucLogicalBlockAddress[3]);
                        if (ptrWrite->ucOperationCode == UFI_WRITE_12) {
                            CBW_WRITE_12 *ptrWrite = (CBW_WRITE_12 *)ptrCBW->CBWCB;
                            ulWriteBlock = ((ptrWrite->ucTransferLength[0] << 24) | (ptrWrite->ucTransferLength[1] << 16) | (ptrWrite->ucTransferLength[2] << 8) | ptrWrite->ucTransferLength[3]);
                        }
                        else {
                            ulWriteBlock = ((ptrWrite->ucTransferLength[0] << 8) | ptrWrite->ucTransferLength[1]); // the total number of blocks to be returned
                        }
    #if defined UTFAT_MULTIPLE_BLOCK_WRITE
                        fnPrepareBlockWrite(ucActiveLUN, ulWriteBlock, 1); // {31} since we know that there will be a write to one or more blocks we can prepare the disk so that it can write faster
    #endif
                        ulWriteBlock *= 512;                             // convert to byte count
                    }
                    continue;                                            // data will follow so don't reply with CSW stage yet
                }
            }
        }
        fnWrite(USBPortID_MSD, (unsigned char *)&csw, sizeof(csw));      // close with CSW stage
    }
#endif
#if defined FREEMASTER_CDC && (USB_CDC_COUNT == 1)                       // {35} FreeMaster run-time debugging on single USB-CDC connection rather than command-line interface
    if ((Length = fnRead(USBPortID_comms[0], ucInputMessage, MEDIUM_MESSAGE)) != 0) { // read available data
        fnHandleFreeMaster(USBPortID_comms[0], ucInputMessage, Length);  // handle the received data
    }
#elif defined USE_USB_CDC && defined USE_MAINTENANCE                     // USB-CDC with command line interface
    while (fnMsgs(USBPortID_comms[0]) != 0) {                            // reception from OUT endpoint on first CDC interface
        if (usUSB_state & (ES_USB_DOWNLOAD_MODE | ES_USB_RS232_MODE)) {
            if (usUSB_state & ES_USB_RS232_MODE) {
    #if defined SERIAL_INTERFACE && defined DEMO_UART                    // {8}
                if (fnWrite(SerialPortID, 0, MEDIUM_MESSAGE) != 0) {     // check that there is space for a block of data
                    Length = fnRead(USBPortID_comms[0], ucInputMessage, MEDIUM_MESSAGE); // read the content
                    fnWrite(SerialPortID, ucInputMessage, Length);       // send input to serial port
                }
                else {
                    fnDriver(SerialPortID, MODIFY_WAKEUP, (MODIFY_TX | OWN_TASK)); // we want to be woken when the queue is free again
                    break;                                               // leave present USB data in the input buffer until we have enough serial output buffer space
                                                                         // the TX_FREE event is not explicitly handled since it is used to wake a next check of the buffer progress
                }
    #else
            fnRead(USBPortID_comms[0], ucInputMessage, LARGE_MESSAGE);   // read the content to empty the queue
    #endif
            }
            else {
                Length = fnRead(USBPortID_comms[0], ucInputMessage, LARGE_MESSAGE); // read the content
    #if defined ACTIVE_FILE_SYSTEM
                fnDownload(ucInputMessage, Length);                      // pass the input to the downloader
    #endif
            }
            continue;
        }
        else {
            Length = fnRead(USBPortID_comms[0], ucInputMessage, LARGE_MESSAGE); // read the content
        }
        fnWrite(USBPortID_comms[0], ucInputMessage, Length);             // echo input
        if (usUSB_state == ES_NO_CONNECTION) {
            if (fnCommandInput(ucInputMessage, Length, SOURCE_USB) != 0) {
                if (fnInitiateLogin(ES_USB_LOGIN) == TELNET_ON_LINE) {
                    static const CHAR ucCOMMAND_MODE_BLOCKED[] = "Command line blocked\r\n";
                    fnWrite(USBPortID_comms[0], (unsigned char *)ucCOMMAND_MODE_BLOCKED, sizeof(ucCOMMAND_MODE_BLOCKED));
                }
            }
        }
        else {
            fnCommandInput(ucInputMessage, Length, SOURCE_USB);
        }
    }
    #if (USB_CDC_COUNT > 1) && (USB_CDC_COUNT > (MODBUS_USB_CDC_COUNT + 1))
    for (iCDC_input = (1 + MODBUS_USB_CDC_COUNT); iCDC_input < USB_CDC_COUNT; iCDC_input++) { // for each CDC interface
        #if defined FREEMASTER_CDC && (USB_CDC_COUNT > 1)                // {35} FreeMaster run-time debugging on final USB-CDC connection
        if (iCDC_input == (USB_CDC_COUNT - 1)) {                         // final CDC instance if used for FreeMaster run-time debugging
            if ((Length = fnRead(USBPortID_comms[iCDC_input], ucInputMessage, MEDIUM_MESSAGE)) != 0) { // read available data
                fnHandleFreeMaster(USBPortID_comms[iCDC_input], ucInputMessage, Length); // handle the received data
            }
            break;
        }
        #endif
        #if !defined FREEMASTER_CDC || (USB_CDC_COUNT > 2)
            #if defined SERIAL_INTERFACE && defined USB_SERIAL_CONNECTIONS
        if (fnMsgs(USBPortID_comms[iCDC_input]) != 0) {                  // if there is data available in the USB input
            if (CDCSerialPortID[iCDC_input] == NO_ID_ALLOCATED) {        // no attached UART
                fnRead(USBPortID_comms[iCDC_input], ucInputMessage, LARGE_MESSAGE); // clear input
            }
            else {
                if (fnWrite(CDCSerialPortID[iCDC_input], 0, MEDIUM_MESSAGE) != 0) { // check that there is space for a block of data to be copied to the corresponding UART outout
                    Length = fnRead(USBPortID_comms[iCDC_input], ucInputMessage, MEDIUM_MESSAGE); // read the content from the USB input
                    fnWrite(CDCSerialPortID[iCDC_input], ucInputMessage, Length); // send read content to serial port
                }
                else {
                    fnDriver(CDCSerialPortID[iCDC_input], MODIFY_WAKEUP, (MODIFY_TX | OWN_TASK)); // we want to be woken when the UART's queue is free again
                                                                         // leave present USB data in the input buffer until we have enough serial output buffer space
                                                                         // the TX_FREE event is not explicitly handled since it is used to wake a next check of the buffer progress
                }
            }
        }
        if (CDCSerialPortID[iCDC_input] != NO_ID_ALLOCATED) {            // if there is a UART ready
            if ((Length = fnRead(CDCSerialPortID[iCDC_input], ucInputMessage, LARGE_MESSAGE)) != 0) { // if there is serial reception
                fnWrite(USBPortID_comms[iCDC_input], ucInputMessage, Length); // send to USB-CDC interface
            }
        }
            #else
        Length = fnRead(USBPortID_comms[iCDC_input], ucInputMessage, LARGE_MESSAGE); // read any content
        if (Length != 0) {
            fnWrite(USBPortID_comms[iCDC_input], ucInputMessage, Length); // echo the reception back
        }
            #endif
        #endif
    }
    #endif
#elif defined USE_USB_CDC                                                // USB-CDC without command line menu (quick test case without full flow control handling)
    #if USB_CDC_COUNT > 1                                                // multiple CDC interfacs
        #if defined SERIAL_INTERFACE && defined DEMO_UART
    CDCSerialPortID[0] = SerialPortID;                                   // automatically enter the debug serial port in the CDC UART list
        #endif
        #if defined USE_MAINTENANCE
    iCDC_input = 1;                                                      // first interface is handled in application.c
        #else
    iCDC_input = MODBUS_USB_CDC_COUNT;                                   // CDC-UART bridge on all subsequent interfaces
        #endif
    for ( ; iCDC_input < USB_CDC_COUNT; iCDC_input++) {                  // for each CDC interface
    #elif !defined USE_MAINTENANCE                                       // single CDC interface without command-line shell
        #define iCDC_input 0
    #endif
        Length = fnRead(USBPortID_comms[iCDC_input], ucInputMessage, LARGE_MESSAGE); // read any CDC input content
    #if defined SERIAL_INTERFACE && defined USB_SERIAL_CONNECTIONS       // {8} CDC-UART bridge
        {
        #if USB_CDC_COUNT > 1
            QUEUE_HANDLE uartID = CDCSerialPortID[iCDC_input];           // the associated UART to bridge to/from
        #else
            QUEUE_HANDLE uartID = SerialPortID;                          // the associated UART to bridge to/from
        #endif
            if (uartID != NO_ID_ALLOCATED) {                             // if the UART interface has been opened
                if (Length != 0) {                                       // CDC reception available
                    fnWrite(uartID, ucInputMessage, Length);             // send input to serial port (data loss not respected if the serial port has no more output buffer space)
                }
                if ((Length = fnRead(uartID, ucInputMessage, LARGE_MESSAGE)) != 0) { // read any UART input data
                    fnWrite(USBPortID_comms[iCDC_input], ucInputMessage, Length); // pass it on the the USB-CDC connection
                }
            }
        }
    #else
        fnWrite(USBPortID_comms[iCDC_input], ucInputMessage, Length);    // echo the reception back
    #endif
    #if USB_CDC_COUNT > 1
    }
    #endif
#endif
#if defined USE_USB_HID_RAW
    if (USBPortID_HID_raw != NO_ID_ALLOCATED) {
    #if HID_RAW_TX_SIZE > HID_RAW_RX_SIZE
        static unsigned char ucRawData[HID_RAW_TX_SIZE];                 // static reception buffer
    #else
        static unsigned char ucRawData[HID_RAW_RX_SIZE];                 // static reception buffer
    #endif
        if (fnRead(USBPortID_HID_raw, ucRawData, HID_RAW_RX_SIZE) == HID_RAW_RX_SIZE) {
            fnWrite(USBPortID_HID_raw, ucRawData, HID_RAW_TX_SIZE);      // send back
        }
    }
#endif
}

#if defined USE_USB_HID_KEYBOARD                                         // {34}
static void fnSetKeyboardOutput(unsigned char ucOutputs)                 // assumed to be set report with a single byte content
{
    // Directly change the state of outputs
    //
}

// ucKeyboardState[8] is assumed to be zeroed when called
//
static void fnInsertUSBKeyCode(CHAR cInputCharacter, unsigned char ucKeyboardState[8])
{
    if ((cInputCharacter >= 'A') && (cInputCharacter <= 'Z')) {          // if capital
        ucKeyboardState[0] = 0x02;                                       // shift
        cInputCharacter += ('a' - 'A');                                  // convert to miniscule equivalent
    }
    if ((cInputCharacter >= 'a') && (cInputCharacter <= 'z')) {          // if a character key
        cInputCharacter -= ('a' - 4);                                    // convert 'a' to the code 4
    }
    else if ((cInputCharacter >= '1') && (cInputCharacter <= '9')) {     // number (not including '0')
        cInputCharacter -= ('1' - 30);                                   // convert '1' to code 30
    }
    else if (cInputCharacter == '0') {
        cInputCharacter = 39;                                            // code of '0' is 39
    }
    else if (cInputCharacter == ' ') {
        cInputCharacter = 44;                                            // space bar
    }
    else if (TAB_KEY == cInputCharacter) {
        cInputCharacter = 43;                                            // tab key
    }
    else if (CARRIAGE_RETURN == cInputCharacter) {
        cInputCharacter = 40;                                            // return key
    }
    else if (DELETE_KEY == cInputCharacter) {
        cInputCharacter = 42;                                            // backspace
    }
    else if ('!' == cInputCharacter) {
        ucKeyboardState[0] = 0x02;                                       // shift
        cInputCharacter = 30;
    }
    else if ('@' == cInputCharacter) {
        ucKeyboardState[0] = 0x02;                                       // shift
        cInputCharacter = 31;
    }
    else if ('#' == cInputCharacter) {
        ucKeyboardState[0] = 0x02;                                       // shift
        cInputCharacter = 32;
    }
    else if ('$' == cInputCharacter) {
        ucKeyboardState[0] = 0x02;                                       // shift
        cInputCharacter = 33;
    }
    else if ('%' == cInputCharacter) {
        ucKeyboardState[0] = 0x02;                                       // shift
        cInputCharacter = 34;
    }
    else if ('^' == cInputCharacter) {
        ucKeyboardState[0] = 0x02;                                       // shift
        cInputCharacter = 35;
    }
    else if ('&' == cInputCharacter) {
        ucKeyboardState[0] = 0x02;                                       // shift
        cInputCharacter = 36;
    }
    else if ('*' == cInputCharacter) {
        ucKeyboardState[0] = 0x02;                                       // shift
        cInputCharacter = 37;
    }
    else if ('(' == cInputCharacter) {
        ucKeyboardState[0] = 0x02;                                       // shift
        cInputCharacter = 38;
    }
    else if (')' == cInputCharacter) {
        ucKeyboardState[0] = 0x02;                                       // shift
        cInputCharacter = 39;
    }
    else if ('=' == cInputCharacter) {
        cInputCharacter = 46;
    }
    else if ('+' == cInputCharacter) {
        ucKeyboardState[0] = 0x02;                                       // shift
        cInputCharacter = 46;
    }
    else if ('[' == cInputCharacter) {
        cInputCharacter = 47;
    }
    else if ('{' == cInputCharacter) {
        ucKeyboardState[0] = 0x02;                                       // shift
        cInputCharacter = 47;
    }
    else if (']' == cInputCharacter) {
        cInputCharacter = 48;
    }
    else if ('}' == cInputCharacter) {
        ucKeyboardState[0] = 0x02;                                       // shift
        cInputCharacter = 48;
    }
    else if (0x5c == cInputCharacter) {                                  // '\'
        cInputCharacter = 49;
    }
    else if ('|' == cInputCharacter) {
        ucKeyboardState[0] = 0x02;                                       // shift
        cInputCharacter = 49;
    }
    else if (',' == cInputCharacter) {
        cInputCharacter = 54;
    }
    else if ('<' == cInputCharacter) {
        ucKeyboardState[0] = 0x02;                                       // shift
        cInputCharacter = 54;
    }
    else if ('.' == cInputCharacter) {
        cInputCharacter = 55;
    }
    else if ('>' == cInputCharacter) {
        ucKeyboardState[0] = 0x02;                                       // shift
        cInputCharacter = 55;
    }
    else if ('/' == cInputCharacter) {
        cInputCharacter = 56;
    }
    else if ('?' == cInputCharacter) {
        ucKeyboardState[0] = 0x02;                                       // shift
        cInputCharacter = 56;
    }
    else {
        return;                                                          // others are not yet supported
    }
    ucKeyboardState[2] = (unsigned char)cInputCharacter;                 // key code
}

static void fnKeyboardInput(void)
{
    if (iUSB_keyboard_state == KEYBOARD_ENUMERATED) {                    // if enumerated and not waiting for the previous IN to complete
        static unsigned char ucKeyboardState[8] = {0};                   // this must be static since the interrupt IN endpoint uses non-buffered transmission
        static CHAR cDoubleKey = 0;
        CHAR cSingleInput = 0;
        if (cDoubleKey != 0) {                                           // if a key is being queued (the second in a row with the same value) we insert it now
            cSingleInput = cDoubleKey;
            cDoubleKey = 0;
        }
        else if ((keyboardQueue != NO_ID_ALLOCATED) && (fnRead(keyboardQueue, (unsigned char *)&cSingleInput, 1) != 0)) { // extract a single input from the FIFO queue and send a key code if one was found
            if (cSingleInput == cLastInput) {                            // if the same value is received twice in a row we need to send a key release before sending the second so that the host recognises it
                cDoubleKey = cSingleInput;                               // save the key that still needs to be sent
                cSingleInput = 0;                                        // cause a the key relase to be sent
            }
        }
#if !defined IN_COMPLETE_CALLBACK
        else {                                                           // handle mechanical keys if required
    #if defined TEENSY_3_1 || defined TEENSY_LC
            _CONFIG_PORT_INPUT_FAST_LOW(D, (SWITCH_1 | SWITCH_2), PORT_PS_UP_ENABLE);
            #define SHIFTKEY()  (_READ_PORT_MASK(D, SWITCH_2) == 0)      // pin 21 [A6] acts as shift key
            #define KEY_T()     (_READ_PORT_MASK(D, SWITCH_1) == 0)      // pin 20 [A7] acts as 't' key
    #elif defined FRDM_KL25Z
            _CONFIG_PORT_INPUT_FAST_LOW(D, (SWITCH_1), PORT_PS_UP_ENABLE);
            #define SHIFTKEY()  0
            #define KEY_T()     (_READ_PORT_MASK(D, SWITCH_1) == 0)      // switch 1 acts as 't' key
    #elif defined TWR_K24F120M || defined FRDM_K20D50M
            _CONFIG_PORT_INPUT_FAST_LOW(C, (SWITCH_1), PORT_PS_UP_ENABLE);
            #define SHIFTKEY()  0
            #define KEY_T()     (_READ_PORT_MASK(C, SWITCH_1) == 0)      // switch 1 acts as 't' key
    #elif defined TWR_KL25Z48M
            _CONFIG_PORT_INPUT_FAST_LOW(C, (SWITCH_4), PORT_PS_UP_ENABLE);
            #define SHIFTKEY()  0
            #define KEY_T()     (_READ_PORT_MASK(C, SWITCH_4) == 0)      // switch 4 acts as 't' key
    #elif defined FRDM_K64F
            _CONFIG_PORT_INPUT_FAST_LOW(C, SWITCH_2, PORT_PS_UP_ENABLE);
            _CONFIG_PORT_INPUT_FAST_LOW(A, SWITCH_3, PORT_PS_UP_ENABLE);
            #define SHIFTKEY()  (_READ_PORT_MASK(C, SWITCH_2) == 0)      // FRDM-K64F SW2 acts as shift key
            #define KEY_T()     (_READ_PORT_MASK(A, SWITCH_3) == 0)      // FRDM-K64F SW3 acts as 't' key
    #endif
    #if defined KEY_T
            if (KEY_T() != 0) {                                          // simple key press
                if (SHIFTKEY() != 0) {                                   // shift also pressed
                    cSingleInput = 'T';
                    fnInsertUSBKeyCode('T', ucKeyboardState);
                }
                else {
                    cSingleInput = 't';
                    fnInsertUSBKeyCode('t', ucKeyboardState);
                }
            }
    #endif
        }
#endif
        // cSingleInput will be 0 if there was no key being pressed or will have an ASCII value representing a key that has just been pressed or is being held down
        //
        if (cLastInput != cSingleInput) {                                // if there has been a change the new state is sent
            ucKeyboardState[0] = 0;                                      // reset the relevant bytes in the frame
            ucKeyboardState[2] = 0;
            fnInsertUSBKeyCode(cSingleInput, ucKeyboardState);           // convert the ASCII input to a key code
#if defined IN_COMPLETE_CALLBACK
            iUSB_keyboard_state = (KEYBOARD_ENUMERATED | KEYBOARD_IN_ACTIVE); // mark that there will be a new IN frame prepared
#endif
            fnWrite(USBPortID_HID_kb, ucKeyboardState, sizeof(ucKeyboardState)); // place the message into the output buffer so that it is sent on next IN poll
            cLastInput = cSingleInput;
        }
#if !defined IN_COMPLETE_CALLBACK
        uTaskerMonoTimer(OWN_TASK, KEYBOARD_INTERVAL, T_KEYBOARD_ACTION);// next keyboard state poll (slower than the USB IN polling rate to ensure that the frame has been sent before next input check)
#endif
    }
}

    #if defined IN_COMPLETE_CALLBACK
// The previous IN data was successfully transferred (this is an interrupt callback)
//
static void fnKeyboardPolled(unsigned char ucEndpoint)
{
    iUSB_keyboard_state = KEYBOARD_ENUMERATED;                           // keyboard is no longer waiting for the previous IN to complete
    fnInterruptMessage(TASK_USB, EVENT_USB_KEYBOARD_INPUT);              // check to see whether there are more keyboard strokes to be sent
}
    #endif
#endif

#if defined USE_USB_HID_RAW && defined IN_COMPLETE_CALLBACK
static void fnRawHIDPolled(unsigned char ucEndpoint)
{
}
#endif

#if defined USE_USB_MSD
// The output buffer has space so continue with data transfer or terminate
//
static void fnContinueMedia(void)
{
    QUEUE_TRANSFER buffer_space = fnWrite(USBPortID_MSD, 0, 0);          // check whether there is space available in the USB output buffer to queue
    while ((ulReadBlock != 0) && (buffer_space >= 512)) {                // send as many blocks as possible as long as the output buffer has space
        if (UTFAT_SUCCESS == fnReadSector(ucActiveLUN, 0, ulLogicalBlockAdr)) { // read a sector from the disk
            fnWrite(USBPortID_MSD, ptrDiskInfo[ucActiveLUN]->ptrSectorData, 512); // send it to the USB output queue
            buffer_space -= 512;
            ulLogicalBlockAdr++;
        }
        else {
            // Error reading media
            //
        }
        ulReadBlock--;                                                   // one less block to be sent
    }
    if (ulReadBlock == 0) {                                              // all blocks have been read and put to the output queue
        if (buffer_space >= sizeof(csw)) {
            fnWrite(USBPortID_MSD, (unsigned char *)&csw, sizeof(csw));  // close with CSW stage
            return;
        }
    }
    if (buffer_space < 512) {                                            // if there is too little buffer space for a complete sector
        fnDriver(USBPortID_MSD, MODIFY_WAKEUP, (MODIFY_TX | TASK_USB));  // when there is room in the output buffer the task will be woken
    }
}
#endif

#if defined USE_USB_CDC
// Called to transmit data from defined USB endpoint (serial input uses this)
//
extern QUEUE_TRANSFER fnSendToUSB(unsigned char *ptrData, QUEUE_TRANSFER Length)
{
    return (fnWrite(USBPortID_comms[0], ptrData, Length));               // send to USB-CDC interface
}

// Set the global debug interface handle to the USB handle
//
extern void fnSetUSB_debug(void)
{
    DebugHandle = USBPortID_comms[0];
}

    #if defined USE_USB_CDC && defined SERIAL_INTERFACE && defined USB_SERIAL_CONNECTIONS && (USB_CDC_COUNT > 1)
static QUEUE_HANDLE fnOpenUART(TTYTABLE *PtrInterfaceParameters, unsigned char ucDriverMode)
{
    QUEUE_HANDLE serialHandle;
        #if defined FRDM_KL27Z || defined CAPUCCINO_KL27                 // DMA transmission doesn't work on UART2 of KL27 - to be investigated
    if (PtrInterfaceParameters->Channel == 2) {
        PtrInterfaceParameters->ucDMAConfig = 0;
    }
        #endif
    if ((serialHandle = fnOpen(TYPE_TTY, ucDriverMode, PtrInterfaceParameters)) != 0) { // open the channel with defined configurations (initially inactive)
        fnDriver(serialHandle, (TX_ON | RX_ON), 0);                      // enable rx and tx
        if (PtrInterfaceParameters->Config & RTS_CTS) {
            fnDriver(serialHandle, (MODIFY_INTERRUPT | ENABLE_CTS_CHANGE), 0); // activate CTS interrupt when working with HW flow control (this returns also the present control line states)
            fnDriver(serialHandle, (MODIFY_CONTROL | SET_RTS), 0 );      // activate RTS line when working with HW flow control
        }
    }
    return serialHandle;
}

static void fnOpenUSB_UARTs(int iInterface)
{
    static const QUEUE_HANDLE UART_channels[USB_CDC_COUNT - 1] = {       // the UARTs mapped to each USB-CDC interface
        CDC_UART_1,
        #if USB_CDC_COUNT > 2
        CDC_UART_2,
        #endif
        #if USB_CDC_COUNT > 3
        CDC_UART_3,
        #endif
        #if USB_CDC_COUNT > 4
        CDC_UART_4,
        #endif
        #if USB_CDC_COUNT > 5
        CDC_UART_5,
        #endif
    };
    TTYTABLE tInterfaceParameters;                                       // table for passing information to driver
    int iUART;
    tInterfaceParameters.ucSpeed = SERIAL_BAUD_115200;                   // baud rate
    tInterfaceParameters.Rx_tx_sizes.RxQueueSize = RX_BUFFER_SIZE;       // input buffer size
    tInterfaceParameters.Rx_tx_sizes.TxQueueSize = TX_BUFFER_SIZE;       // output buffer size
    tInterfaceParameters.Task_to_wake = OWN_TASK;                        // wake self when messages have been received
        #if defined SUPPORT_FLOW_HIGH_LOW
    tInterfaceParameters.ucFlowHighWater = 80;                           // set the flow control high and low water levels in %
    tInterfaceParameters.ucFlowLowWater = 20;
        #endif
    tInterfaceParameters.Config = (CHAR_8 + NO_PARITY + ONE_STOP + CHAR_MODE);
        #if defined SERIAL_SUPPORT_DMA
    tInterfaceParameters.ucDMAConfig = 0;                                // disable DMA
    tInterfaceParameters.ucDMAConfig = UART_TX_DMA;                      // activate DMA on transmission
        #endif
        #if defined SUPPORT_HW_FLOW
  //tInterfaceParameters.Config |= RTS_CTS;                              // enable RTS/CTS operation
        #endif
    if (iInterface == 0) {
        for (iUART = 0; iUART < (USB_CDC_COUNT - 1); iUART++) {          // configure and open each UART attached to a USB-CDC connection
            tInterfaceParameters.Channel = UART_channels[iUART];         // set UART channel for serial use
            CDCSerialPortID[iUART + 1] = fnOpenUART(&tInterfaceParameters, FOR_I_O);
            ucCDC_UART_SerialSpeed[iUART] = tInterfaceParameters.ucSpeed; // set additional interface parameters to the same as the main one
            CDC_UART_SerialMode[iUART] = tInterfaceParameters.Config;
        }
    }
    else {                                                               // modify single interface
        iInterface--;
        tInterfaceParameters.ucSpeed = ucCDC_UART_SerialSpeed[iInterface];
        tInterfaceParameters.Config = CDC_UART_SerialMode[iInterface];
        tInterfaceParameters.Channel = UART_channels[iInterface];        // set UART channel for serial use
        CDCSerialPortID[iInterface + 1] = fnOpenUART(&tInterfaceParameters, MODIFY_CONFIG);
    }
}
    #endif
#endif

// The application must always supply this routine and return its device descriptor when requested
// This example shows a single fixed device configuration but multiple configurations could be selected (eg. for experimental use)
//
extern void *fnGetUSB_device_descriptor(unsigned short *usLength)
{
#if defined USB_HOST_SUPPORT
    return 0;
#else
    *usLength = sizeof(device_descriptor);
    return (void *)&device_descriptor;
#endif
}

// The application must always supply this routine and return its configuration descriptor when requested
// This example shows a single fixed configuration but multiple configurations could be selected (eg. for programmable device types)
//
extern void *fnGetUSB_config_descriptor(unsigned short *usLength)
{
#if defined USB_HOST_SUPPORT
    return 0;                                                            // in host mode we return a zero so that the generic driver uses its own information
#else
    *usLength = sizeof(config_descriptor);
    return (void *)&config_descriptor;
#endif
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
extern unsigned char *fnGetUSB_string_entry(unsigned char ucStringRef, unsigned short *usLength)
{
    #if defined USB_HOST_SUPPORT
    return 0;
    #else
    if (ucStringRef > LAST_STRING_INDEX) {
        return 0;                                                        // invalid string index
    }
        #if defined USB_RUN_TIME_DEFINABLE_STRINGS                       // if variable strings are supported
    if (ucStringTable[ucStringRef][0] == 0) {                            // no length defined, meaning it is a run-time definabled string
        switch (ucStringRef) {
        case SERIAL_NUMBER_STRING_INDEX:                                 // USB driver needs to know what string is used as serial number
            *usLength = (unsigned short)SerialNumberDescriptor->bLength; // return length and location of the user defined serial number
            return (unsigned char *)SerialNumberDescriptor;
        default:
            return 0;                                                    // invalid string index
        }
    }
    else {
        *usLength = ucStringTable[ucStringRef][0];                       // the length of the string
    }
        #else
    *usLength = ucStringTable[ucStringRef][0];                           // the length of the string
        #endif
    return ((unsigned char *)ucStringTable[ucStringRef]);                // return a pointer to the string
    #endif
}
#endif

#if defined USE_USB_CDC
// Convert UART interface settings format to CDC setting format and report to CDC host
//
static void fnReturnUART_settings(int iInterface)
{
    unsigned long ulSpeed;
    UART_MODE_CONFIG SerialMode = 0;
    unsigned char ucSerialSpeed = SERIAL_BAUD_115200;

    if (iInterface == 0) {
        ucSerialSpeed = temp_pars->temp_parameters.ucSerialSpeed;
        SerialMode = temp_pars->temp_parameters.SerialMode;
    }
    #if USB_CDC_COUNT > 1
    else {                                                               // supplementary CDC interfaces
        ucSerialSpeed = ucCDC_UART_SerialSpeed[iInterface - 1];
        SerialMode = CDC_UART_SerialMode[iInterface - 1];
    }
    #endif

    switch (ucSerialSpeed) {                                             // the present baud rate
    case SERIAL_BAUD_300:
        ulSpeed = 300;
        break;
    case SERIAL_BAUD_600:
        ulSpeed = 600;
        break;
    case SERIAL_BAUD_1200:
        ulSpeed = 1200;
        break;
    case SERIAL_BAUD_2400:
        ulSpeed = 2400;
        break;
    case SERIAL_BAUD_4800:
        ulSpeed = 4800;
        break;
    case SERIAL_BAUD_9600:
        ulSpeed = 9600;
        break;
    case SERIAL_BAUD_14400:
        ulSpeed = 14400;
        break;
    case SERIAL_BAUD_19200:
    default:
        ulSpeed = 19200;
        break;
    case SERIAL_BAUD_38400:
        ulSpeed = 38400;
        break;
    case SERIAL_BAUD_57600:
        ulSpeed = 57600;
        break;
    case SERIAL_BAUD_115200:
        ulSpeed = 115200;
        break;
    case SERIAL_BAUD_230400:
        ulSpeed = 230400;
        break;
    case SERIAL_BAUD_250K:
        ulSpeed = 250000;
        break;
    }
    uart_setting[iInterface].dwDTERate[0] = (unsigned char)ulSpeed;
    ulSpeed >>= 8;
    uart_setting[iInterface].dwDTERate[1] = (unsigned char)ulSpeed;
    ulSpeed >>= 8;
    uart_setting[iInterface].dwDTERate[2] = (unsigned char)ulSpeed;
    ulSpeed >>= 8;
    uart_setting[iInterface].dwDTERate[3] = (unsigned char)ulSpeed;

    if (SerialMode & CHAR_7) {                                           // set present character length
        uart_setting[iInterface].bDataBits = CDC_PSTN_7_DATA_BITS;
    }
    else {
        uart_setting[iInterface].bDataBits = CDC_PSTN_8_DATA_BITS;
    }

    if (SerialMode & RS232_EVEN_PARITY) {                                // set present parity
        uart_setting[iInterface].bParityType = CDC_PSTN_EVEN_PARITY;
    }
    else if (SerialMode & RS232_ODD_PARITY) {
        uart_setting[iInterface].bParityType = CDC_PSTN_ODD_PARITY;
    }
    else {
        uart_setting[iInterface].bParityType = CDC_PSTN_NO_PARITY;
    }

    if (SerialMode & ONE_HALF_STOPS) {                                   // set present format
        uart_setting[iInterface].bCharFormat = CDC_PSTN_1_5_STOP_BIT;
    }
    else if (SerialMode & TWO_STOPS) {
        uart_setting[iInterface].bCharFormat = CDC_PSTN_2_STOP_BITS;
    }
    else {
        uart_setting[iInterface].bCharFormat = CDC_PSTN_1_STOP_BIT;
    }
    fnWrite(USB_control, (unsigned char *)&uart_setting[iInterface], sizeof(uart_setting[iInterface])); // return directly (non-buffered)
}

// Convert new UART settings from the CDC host
//
static void fnNewUART_settings(unsigned char *ptrData, unsigned short usLength, unsigned short usExpected, int iInterface)
{
    unsigned char *ptrSet = (unsigned char *)&uart_setting[iInterface];

    if (usExpected > sizeof(uart_setting[iInterface])) {                 // some protection against corrupted reception
        usExpected = sizeof(uart_setting[iInterface]);
    }
    if (usLength > usExpected) {
        usLength = usExpected;
    }

    ptrSet += (sizeof(uart_setting[iInterface]) - usExpected);
    uMemcpy(ptrSet, ptrData, usLength);

    if (usLength >= usExpected) {                                        // all data has been copied
        fnInterruptMessage(OWN_TASK, (unsigned char)(EVENT_COMMIT_NEW_UART_SETTING + iInterface)); // perform the UART modification in the task
    }
}

// Convert the CDC setting format to the UART interface settings format and modify the UART setting
//
static void fnComitt_UART(int iInterface)
{
    unsigned long ulSpeed = uart_setting[iInterface].dwDTERate[0];
    unsigned short usNewSerialMode = 0;
    unsigned char ucNewSerialSpeed;
    ulSpeed |= (uart_setting[iInterface].dwDTERate[1] << 8);
    ulSpeed |= (uart_setting[iInterface].dwDTERate[2] << 16);
    ulSpeed |= (uart_setting[iInterface].dwDTERate[3] << 24);

    if (ulSpeed > 230400) {
        ucNewSerialSpeed = SERIAL_BAUD_250K;
    }
    else if (ulSpeed > 115200) {
        ucNewSerialSpeed = SERIAL_BAUD_230400;
    }
    else if (ulSpeed > 57600) {
        ucNewSerialSpeed = SERIAL_BAUD_115200;
    }
    else if (ulSpeed > 38400) {
        ucNewSerialSpeed = SERIAL_BAUD_57600;
    }
    else if (ulSpeed > 19200) {
        ucNewSerialSpeed = SERIAL_BAUD_38400;
    }
    else if (ulSpeed > 14400) {
        ucNewSerialSpeed = SERIAL_BAUD_19200;
    }
    else if (ulSpeed > 9600) {
        ucNewSerialSpeed = SERIAL_BAUD_14400;
    }
    else if (ulSpeed > 4800) {
        ucNewSerialSpeed = SERIAL_BAUD_9600;
    }
    else if (ulSpeed > 2400) {
        ucNewSerialSpeed = SERIAL_BAUD_4800;
    }
    else if (ulSpeed > 1200) {
        ucNewSerialSpeed = SERIAL_BAUD_2400;
    }
    else if (ulSpeed > 600) {
        ucNewSerialSpeed = SERIAL_BAUD_1200;
    }
    else if (ulSpeed > 300) {
        ucNewSerialSpeed = SERIAL_BAUD_600;
    }
    else {
        ucNewSerialSpeed = SERIAL_BAUD_300;
    }

    if (uart_setting[iInterface].bDataBits == CDC_PSTN_7_DATA_BITS) {
        usNewSerialMode |= CHAR_7;
    }

    switch (uart_setting[iInterface].bParityType) {
    case CDC_PSTN_EVEN_PARITY:
        usNewSerialMode |= RS232_EVEN_PARITY;
        break;
    case CDC_PSTN_ODD_PARITY:
        usNewSerialMode |= RS232_ODD_PARITY;
        break;
    }

    switch (uart_setting[iInterface].bCharFormat) {
    case CDC_PSTN_1_5_STOP_BIT:
        usNewSerialMode |= ONE_HALF_STOPS;
        break;
    case CDC_PSTN_2_STOP_BITS:
        usNewSerialMode |= TWO_STOPS;
        break;
    }

    if (iInterface == 0) {                                               // first interface controls the debug UART interface
        temp_pars->temp_parameters.SerialMode &= ~(CHAR_7 | RS232_EVEN_PARITY | RS232_ODD_PARITY | ONE_HALF_STOPS | TWO_STOPS);
        temp_pars->temp_parameters.SerialMode |= usNewSerialMode;
        temp_pars->temp_parameters.ucSerialSpeed = ucNewSerialSpeed;
    #if defined SERIAL_INTERFACE && defined DEMO_UART                    // {8}
        fnSetNewSerialMode(MODIFY_CONFIG);                               // modify the debug interface setting accordingly
        return;
    #endif
    }
    #if USB_CDC_COUNT > 1
    else {                                                               // supplimentary CDC interfaces
        iInterface--;
        CDC_UART_SerialMode[iInterface] &= ~(CHAR_7 | RS232_EVEN_PARITY | RS232_ODD_PARITY | ONE_HALF_STOPS | TWO_STOPS);
        CDC_UART_SerialMode[iInterface] |= usNewSerialMode;
        ucCDC_UART_SerialSpeed[iInterface] = ucNewSerialSpeed;
        #if defined USE_USB_CDC && defined SERIAL_INTERFACE && defined USB_SERIAL_CONNECTIONS && (USB_CDC_COUNT > 1)
        fnOpenUSB_UARTs(iInterface + 1);                                 // modify the single interface
        #endif
    }
    #endif
}
#endif

#if defined USE_USB_MSD
static int mass_storage_callback(unsigned char *ptrData, unsigned short length, int iType)
{
    if (ulWriteBlock != 0) {                                             // data expected
        return TRANSPARENT_CALLBACK;                                     // handle data in task
    }
    if (uMemcmp(cCBWSignature, ptrData, sizeof(cCBWSignature)) == 0) {   // check that the signature matches
        USB_MASS_STORAGE_CBW *ptrCBW = (USB_MASS_STORAGE_CBW *)ptrData;
        ucActiveLUN = ptrCBW->dCBWLUN;                                   // the logical unit number to use
        if (fnGetPartition(ucActiveLUN, 0) != 0) {                       // if valid storage device
            if ((ptrCBW->dCBWCBLength > 0) && (ptrCBW->dCBWCBLength <= 16)) { // check for valid length
                if (ptrCBW->dmCBWFlags & CBW_IN_FLAG) {
                    switch (ptrCBW->CBWCB[CBW_OperationCode]) {
                    case UFI_MODE_SENSE_6:
                        {                                                // {15}
                            CBW_MODE_SENSE_6 *ptrSense = (CBW_MODE_SENSE_6 *)ptrCBW->CBWCB;
                            unsigned char ucLogicalUnitNumber = (ptrSense->ucLogicalUnitNumber_DBD >> 5);
                            if (fnGetPartition(ucLogicalUnitNumber, 0) != 0) {
                                if ((ptrSense->ucPC_PageCode & MODE_SENSE_6_PAGE_CODE_ALL_PAGES) == MODE_SENSE_6_PAGE_CODE_ALL_PAGES) { // only accept all page request
                                    return TRANSPARENT_CALLBACK;         // accept to be handled by task
                                }
                            }
                        }
                        break;                                           // stall
                    case UFI_REQUEST_SENSE:
                    case UFI_INQUIRY:
                        return TRANSPARENT_CALLBACK;                     // the call-back has done its work and the input buffer can now be used

                    case UFI_READ_CAPACITY:                              // {36}
                    case UFI_READ_FORMAT_CAPACITY:
                        if (ptrDiskInfo[ucActiveLUN]->usDiskFlags & (DISK_MOUNTED | DISK_UNFORMATTED)) { // {16} only respond when there is media inserted, else stall
                            return TRANSPARENT_CALLBACK;             // the call-back has done its work and the input buffer can now be used
                        }
                        break;                                           // stall
                    case UFI_READ_10:
                    case UFI_READ_12:
                        if (ptrDiskInfo[ucActiveLUN]->usDiskFlags & (DISK_MOUNTED | DISK_UNFORMATTED)) { // {16} only respond when there is media inserted, else stall
                            CBW_READ_10 *ptrRead = (CBW_READ_10 *)ptrCBW->CBWCB;
                            ulLogicalBlockAdr = ((ptrRead->ucLogicalBlockAddress[0] << 24) | (ptrRead->ucLogicalBlockAddress[1] << 16) | (ptrRead->ucLogicalBlockAddress[2] << 8) | ptrRead->ucLogicalBlockAddress[3]);
                            if (ulLogicalBlockAdr < ptrDiskInfo[ucActiveLUN]->ulSD_sectors) { // check that the sector is valid
                                return TRANSPARENT_CALLBACK;             // the call-back has done its work and the input buffer can now be used
                            }
                        }
                        break;                                           // stall
                    default:
                        break;                                           // stall
                    }
                }
                else {
                    switch (ptrCBW->CBWCB[CBW_OperationCode]) {
                    case UFI_PRE_ALLOW_REMOVAL:
                    case UFI_TEST_UNIT_READY:
                        return TRANSPARENT_CALLBACK;                     // the call-back has done its work and the input buffer can now be used
                    case UFI_WRITE_10:
                    case UFI_WRITE_12:
                        if (ptrDiskInfo[ucActiveLUN]->usDiskFlags & (DISK_MOUNTED | DISK_UNFORMATTED)) { // {16} only respond when there is media inserted, else stall
                            CBW_WRITE_10 *ptrWrite = (CBW_WRITE_10 *)ptrCBW->CBWCB;
                            ulLogicalBlockAdr = ((ptrWrite->ucLogicalBlockAddress[0] << 24) | (ptrWrite->ucLogicalBlockAddress[1] << 16) | (ptrWrite->ucLogicalBlockAddress[2] << 8) | ptrWrite->ucLogicalBlockAddress[3]);
                            if (ulLogicalBlockAdr < ptrDiskInfo[ucActiveLUN]->ulSD_sectors) { // check that the sector is valid
                                return TRANSPARENT_CALLBACK;             // the call-back has done its work and the input buffer can now be used
                            }                        }
                        break;                                           // stall
                    default:
                        break;
                    }
                }
                uMemcpy(&csw.dCBWTag, ptrCBW->dCBWTag, sizeof(csw.dCBWTag)); // save the tag since we will halt
                uMemcpy(csw.dCSWDataResidue, &ptrCBW->dCBWDataTransferLength, sizeof(csw.dCSWDataResidue)); // since no data will be processes the residue is equal to the requested transfer length
                csw.bCSWStatus = CSW_STATUS_COMMAND_FAILED;              // set fail
            }
        }
    }
    return STALL_ENDPOINT;                                               // stall on any invalid formats
}

// Get partition details - used to ease support of multiple LUNs (Logical Units)
//
static unsigned char *fnGetPartition(unsigned char ucLUN, QUEUE_TRANSFER *length)
{
    if (ucLUN >= NUMBER_OF_PARTITIONS) {
        return 0;                                                        // invalid LUN
    }
    if (length != 0) {
        *length = sizeof(inquiryData);                                   // the inquiry data length
    }
    return (unsigned char *)&inquiryData;                                // the fixed inquiry data
}
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
#if defined USE_USB_CDC
    static int iInterface = 0;
#endif
    int iRtn = BUFFER_CONSUMED;
    switch (iType) {
#if defined USE_USB_MSD
    case ENDPOINT_CLEARED:                                               // halted endpoint has been freed
    #if defined _LPC23XX || defined _LPC17XX                             // {20}
        if (*ptrData == (IN_ENDPOINT | 0x05))                            // BULK IN 5 is the only endpoint that is expect to be halted
    #else
        if (*ptrData == (IN_ENDPOINT | USB_MSB_IN_ENDPOINT_NUMBER))      // this BULK IN is the only endpoint that is expect to be halted
    #endif
        {
            fnWrite(USBPortID_MSD, (unsigned char *)&csw, sizeof(csw));  // answer with the failed CSW on next endpoint 1 OUT
        }
        return TERMINATE_ZERO_DATA;                                      // send zero data to terminate the halt phase
#endif
    case STATUS_STAGE_RECEPTION:                                         // this is the status stage of a control transfer - it confirms that the exchange has completed and can be ignored if not of interest to us
        return BUFFER_CONSUMED;
    case SETUP_DATA_RECEPTION:
        {
            USB_SETUP_HEADER *ptrSetup = (USB_SETUP_HEADER *)ptrData;    // interpret the received data as a setup header
#if !defined USE_USB_HID_MOUSE && !defined USE_USB_HID_KEYBOARD && !defined USE_USB_HID_RAW // {23}{34}
            if ((ptrSetup->bmRequestType & ~STANDARD_DEVICE_TO_HOST) != REQUEST_INTERFACE_CLASS) { // 0x21
                return STALL_ENDPOINT;                                   // stall on any unsupported request types
            }
#endif
            usExpectedData = ptrSetup->wLength[0];                       // the amount of additional data which is expected to arrive from the host belonging to this request
            usExpectedData |= (ptrSetup->wLength[1] << 8);
            if (ptrSetup->bmRequestType & STANDARD_DEVICE_TO_HOST) {     // request for information
                usExpectedData = 0;                                      // no data expected to be received by us
                switch (ptrSetup->bRequest) {
#if defined USE_USB_HID_MOUSE || defined USE_USB_HID_KEYBOARD || defined USE_USB_HID_RAW // {23}
                case USB_REQUEST_GET_DESCRIPTOR:                         // standard request
                    if (ptrSetup->wValue[1] == DESCRIPTOR_TYPE_REPORT) {
    #if defined USE_USB_HID_MOUSE && defined USE_USB_HID_KEYBOARD
                        int iHID_Interface = (ptrSetup->wIndex[0] | (ptrSetup->wIndex[1] << 8));
                        if (iHID_Interface == USB_MOUSE_INTERFACE_NUMBER) { // mouse interface number
                            fnWrite(USB_control, (unsigned char *)&ucMouseReport, sizeof(ucMouseReport)); // return directly (non-buffered)
                        }
                        else if (iHID_Interface == USB_KEYBOARD_INTERFACE_NUMBER) { // keyboard interface number
                            fnWrite(USB_control, (unsigned char *)&ucKeyboardReport, sizeof(ucKeyboardReport)); // return directly (non-buffered)
                        }
                        else {
                            return STALL_ENDPOINT;                       // not supported on other interfaces
                        }
    #elif defined USE_USB_HID_MOUSE
                        fnWrite(USB_control, (unsigned char *)&ucMouseReport, sizeof(ucMouseReport)); // return directly (non-buffered)
    #elif defined USE_USB_HID_KEYBOARD                                     // {34}
                        fnWrite(USB_control, (unsigned char *)&ucKeyboardReport, sizeof(ucKeyboardReport)); // return directly (non-buffered)
    #elif defined USE_USB_HID_RAW
                        fnWrite(USB_control, (unsigned char *)&ucRawReport, sizeof(ucRawReport)); // return directly (non-buffered)
    #endif
                    }
                    else {
                        return STALL_ENDPOINT;                           // not supported
                    }
                    break;
#endif
#if defined USE_USB_MSD
                case BULK_ONLY_MASS_STORAGE_RESET:
                    break;
                case GET_MAX_LUN:
                    {
                        static const unsigned char ucPartitions = (NUMBER_OF_PARTITIONS - 1);
                        if ((ptrSetup->wValue[0] != 0) || (ptrSetup->wValue[1] != 0) || (ptrSetup->wLength[0] != 1) || (ptrSetup->wLength[1] != 0)) {
                            return STALL_ENDPOINT;                       // stall if bad request format
                        }
                        else {
                            fnWrite(USB_control, (unsigned char *)&ucPartitions, sizeof(ucPartitions)); // return directly (non-buffered)
                        }
                    }
                    break;
#endif
#if defined USE_USB_CDC
                case GET_LINE_CODING:                                    // 0x21
                    iInterface = (ptrSetup->wIndex[0] | (ptrSetup->wIndex[1] << 8)); // remember the interface that this command belongs to
                    fnInterruptMessage(OWN_TASK, (unsigned char)(EVENT_RETURN_PRESENT_UART_SETTING + ((iInterface - FIRST_CDC_INTERFACE)/2)));
                    break;
#endif
                default:
                    return STALL_ENDPOINT;                               // stall on any unsupported requests
                }
            }
            else {                                                       // command
                iRtn = TERMINATE_ZERO_DATA;                              // {5} acknowledge receipt of the request if we have no data to return (default)
                switch (ptrSetup->bRequest) {
#if defined USE_USB_CDC                                                  // {23}
                case SET_LINE_CODING:                                    // 0x20 - the host is informing us of parameters to use
                    ucCollectingMode = ptrSetup->bRequest;               // the next OUT frame will contain the settings
                    iInterface = (ptrSetup->wIndex[0] | (ptrSetup->wIndex[1] << 8)); // remember the interface that this command belongs to
                    iRtn = BUFFER_CONSUMED_EXPECT_MORE;                  // {5} the present buffer has been consumed but extra data is subsequently expected
                    break;

                case SET_CONTROL_LINE_STATE:                             // OUT - 0x22 (controls RTS and DTR)
                    iInterface = (ptrSetup->wIndex[0] | (ptrSetup->wIndex[1] << 8)); // remember the interface that this command belongs to
    #if defined SERIAL_INTERFACE && defined SUPPORT_HW_FLOW && defined DEMO_UART // {8}
                    {
                        QUEUE_HANDLE serialHandle = SerialPortID;
        #if USB_CDC_COUNT > 1
                        iInterface -= FIRST_CDC_INTERFACE;
                        iInterface /= 2;
                        if (iInterface > 0) {
                            serialHandle = CDCSerialPortID[iInterface];
                        }
        #endif
                        if (serialHandle != NO_ID_ALLOCATED) {           // only set the control lines when the UART is available
                            if (ptrSetup->wValue[0] & CDC_RTS) {
                                fnDriver(serialHandle, (MODIFY_CONTROL | SET_RTS), 0);
                            }
                            else {
                                fnDriver(serialHandle, (MODIFY_CONTROL | CLEAR_RTS), 0);
                            }
                            if (ptrSetup->wValue[0] & CDC_DTR) {
                                fnDriver(serialHandle, (MODIFY_CONTROL | SET_DTR), 0);
                            }
                            else {
                                fnDriver(serialHandle, (MODIFY_CONTROL | CLEAR_DTR), 0);
                            }
                        }
                    }
    #endif
                    break;
#endif
#if defined USE_USB_HID_MOUSE || defined USE_USB_HID_KEYBOARD || defined USE_USB_HID_RAW // {23}{34}
                case HID_SET_IDLE:                                       // 0x0a - this can silence a report
                    break;                                               // answer with zero data
#endif
#if defined USE_USB_HID_KEYBOARD                                         // {34}
                case HID_SET_REPORT:                                     // 0x09 - set report
                    return BUFFER_CONSUMED_EXPECT_MORE;                  // the present buffer has been consumed but extra data is subsequently expected
#endif
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
#if defined USB_HOST_SUPPORT
            case GET_MAX_LUN:
                fnDeviceLun(ptrData, length, usExpectedData);            // handle the response
                break;
#endif
#if defined USE_USB_CDC
            case SET_LINE_CODING:
                fnNewUART_settings(ptrData, length, usExpectedData, ((iInterface - FIRST_CDC_INTERFACE)/2)); // set the new UART mode (the complete data will always be received here so we can always terminate now, otherwise BUFFER_CONSUMED_EXPECT_MORE would be returned until complete)
              //iRtn = TERMINATE_ZERO_DATA;                              // {5}
                break;
#endif
            default:
#if defined USE_USB_HID_KEYBOARD                                         // {34}
                fnSetKeyboardOutput(*ptrData);                           // assumed to be set report with a single byte content
#endif
                break;
            }
            ucCollectingMode = 0;                                        // {9} reset to avoid repeat of command when subsequent, invalid commands are received
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

#if defined USB_HOST_SUPPORT

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

// Receiving details of device logical unit count
//
static void fnDeviceLun(unsigned char *ptrData, unsigned short usLength, unsigned short usExpected)
{
    ucDeviceLUN = (*ptrData + 1);                                        // increment the value by 1 to get the number of partitions
    fnInterruptMessage(OWN_TASK, EVENT_LUN_READY);
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
        command_transport.dCBWDataTransferLengthL = LITTLE_LONG_WORD(252); // data transfer length
        command_transport.dmCBWFlags = CBW_IN_FLAG;
        command_transport.dCBWCBLength = 10;
        command_transport.CBWCB[8] = 252;
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
#if defined UTFAT_WRITE
    case UFI_WRITE_10:
#endif
    case UFI_READ_10:
        command_transport.dCBWDataTransferLengthL = LITTLE_LONG_WORD(512); // data transfer length
        command_transport.dmCBWFlags = CBW_IN_FLAG;
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
     fnFlush(USBPortID_host, FLUSH_TX);                                  // always flush the tx buffer to ensure message alignment in buffer before sending
    if (fnWrite(USBPortID_host, (unsigned char *)&command_transport, command_length) == command_length) { // write data (buffered on this bulk endpoint)
        if ((ucOpcode != UFI_WRITE_10) && (ucOpcode != UFI_READ_10)) {
            uTaskerMonoTimer(OWN_TASK, (DELAY_LIMIT)(0 * SEC), T_GET_STATUS_TEST); // collect the status after a short delay (we should do it after the transmission has been acked!!!)
          //fnDriver(USBPortID_host, (RX_ON), 0);                        // enable IN polling so that we can receive data/status response
        }
        iUSB_MSD_OpCode = ucOpcode;                                      // the operation in progress
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
        tInterfaceParameters.usConfig = USB_TERMINATING_ENDPOINT;        // configure the IN endpoint to terminate messages with a zero length frame
        tInterfaceParameters.queue_sizes.RxQueueSize = 1024;             // optional input queue (used only when no call-back defined)
        tInterfaceParameters.queue_sizes.TxQueueSize = 512;              // additional tx buffer
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
    fnDebugHex(ulSectorNumber, (sizeof(ulSectorNumber) | WITH_SPACE | WITH_LEADIN));
    fnFlush(USBPortID_host, FLUSH_TX);                                   // always flush the tx buffer to ensure message alignment in buffer before commanding
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
                            fnDebugMsg(" OK\r\n");
                            break;
                        }
                        else {
                            fnDebugMsg(" Staus error\r\n");
                            return UTFAT_DISK_READ_ERROR;
                        }
                    }
                    else if (++iWait > MAX_USB_MSD_READ_WAIT) {          // limit the maximum wait in case the memory stick fails
                        fnDebugMsg(" TOS\r\n");
                        return UTFAT_DISK_READ_ERROR;
                    }
                }
                break;
            }
        }
        else if (++iWait > MAX_USB_MSD_READ_WAIT) {                      // limit the maximum wait in case the memory stick fails
            fnDebugMsg(" TOD\r\n");
            return UTFAT_DISK_READ_ERROR;
        }
    }
    return UTFAT_SUCCESS;
}

// Read a single sector from the disk (usb-msd drive) - the sector number is specified by ulSectorNumber
//
extern int utReadMSDsector(UTDISK *ptr_utDisk, unsigned long ulSectorNumber, void *ptrBuf)
{
    fnDebugMsg("Sec");
    return (utReadMSD((unsigned char *)ptrBuf, ulSectorNumber));         // this blocks until the read completes (or fails)
}

// Read a single sector from the disk (usb-msd drive) - the sector number is specified by ulSectorNumber but return only demanded quantity
//
extern int utReadPartialMSDsector(UTDISK *ptr_utDisk, unsigned long ulSectorNumber, void *ptrBuf, unsigned short usOffset, unsigned short usLength)
{
    unsigned long ulTemp[512/sizeof(unsigned long)];
    int iRtn;
    fnDebugMsg("Part");
    fnDebugHex(usOffset, (sizeof(usOffset) | WITH_SPACE | WITH_LEADIN));
    fnDebugMsg("/");
    fnDebugDec(usLength, 0);
    iRtn = utReadMSD((unsigned char *)ulTemp, ulSectorNumber);           // read sector content to temporary buffer
    if (UTFAT_SUCCESS == iRtn) {
        uMemcpy(ptrBuf, (((unsigned char *)ulTemp) + usOffset), usLength); // return the requested content
    }
    return iRtn;
}

    #if defined UTFAT_WRITE
static int utWriteMSD(unsigned char *ptrBuffer, unsigned long ulSectorNumber)
{   // !!!!! TO BE TESTED !!!!
    #define MAX_USB_MSD_WRITE_WAIT (50000)
    volatile int iWait;
    USB_MASS_STORAGE_CBW_LW status_stage;
    ulLBA = ulSectorNumber;                                              // write a sector from this logical block address
    usBlockCount = 1;                                                    // write a single block
    fnDebugHex(ulSectorNumber, (sizeof(ulSectorNumber) | WITH_SPACE | WITH_LEADIN));
    fnFlush(USBPortID_host, FLUSH_TX);                                   // always flush the tx buffer to ensure message alignment in buffer before commanding
    fnSendMSD_host(UFI_WRITE_10);                                        // request the write
    fnWrite(USBPortID_host, ptrBuffer, 512);                             // write data (buffered on this bulk endpoint)
    iWait = 0;
//    fnDriver(USBPortID_host, (RX_ON), 0);                                // enable IN polling so that we can receive data/status response
    while (1) {                                                          // wait until the status stage has been received
        if (fnRead(USBPortID_host, (unsigned char *)&status_stage, sizeof(status_stage)) != 0) { // read status stage
            if ((status_stage.dCBWSignatureL == USBS_SIGNATURE) && (status_stage.dCBWTagL == ulTag)) {
                fnDebugMsg(" OK\r\n");
                break;
            }
            else {
                fnDebugMsg(" Staus error\r\n");
                return UTFAT_DISK_READ_ERROR;
            }
        }
        else if (++iWait > MAX_USB_MSD_WRITE_WAIT) {                     // limit the maximum wait in case the memory stick fails
            fnDebugMsg(" TO\r\n");
            return UTFAT_DISK_READ_ERROR;
        }
    }
    return UTFAT_SUCCESS;
}

extern int utCommitMSDSectorData(UTDISK *ptr_utDisk, void *ptrBuffer, unsigned long ulSectorNumber)
{
    fnDebugMsg("W");
    return (utWriteMSD((unsigned char *)ptrBuffer, ulSectorNumber));
}

extern int utDeleteMSDSector(UTDISK *ptr_utDisk, unsigned long ulSectorNumber)
{
    unsigned long ulTemp[512/sizeof(unsigned long)];
    fnDebugMsg("Del");
    uMemset(ulTemp, 0, sizeof(ulTemp));                                  // prepare a zeroed buffer to be written
    return (utWriteMSD((unsigned char *)ulTemp, ulSectorNumber));
}
    #endif
#else
static void fnConfigureApplicationEndpoints(void)
{
    USBTABLE tInterfaceParameters;                                       // table for passing information to driver
    QUEUE_HANDLE endpointNumber = 1;
    #if defined USE_USB_CDC
    QUEUE_HANDLE endpointNumberInt;
    #endif
    #if USB_CDC_COUNT > 1
    int iCDC_interface = 1;                                              // additional CDC interfaces
    #endif

    tInterfaceParameters.owner_task = OWN_TASK;                          // USB task is woken on receptions
    tInterfaceParameters.usConfig = 0;
    #if defined IN_COMPLETE_CALLBACK
    tInterfaceParameters.INcallback = 0;
    #endif
    tInterfaceParameters.usEndpointSize = 64;                            // endpoint queue size (2 buffers of this size will be created for reception)
    #if defined USE_USB_CDC
        #if defined _LPC23XX || defined _LPC17XX                         // {14}
    tInterfaceParameters.Endpoint = 5;                                   // set USB endpoints to act as an input/output pair - transmitter (IN)
    tInterfaceParameters.Paired_RxEndpoint = 2;                          // receiver (OUT)
        #else
    tInterfaceParameters.Paired_RxEndpoint = endpointNumber;             // receiver (OUT)
            #if !defined USB_SIMPLEX_ENDPOINTS
    endpointNumber++;
            #endif
    tInterfaceParameters.Endpoint = endpointNumber;                      // set USB endpoints to act as an input/output pair - transmitter (IN)
    endpointNumberInt = (endpointNumber + 1);
            #if defined USB_SIMPLEX_ENDPOINTS
    endpointNumber++;
            #else
    endpointNumber += 2;
            #endif
        #endif    
    tInterfaceParameters.usb_callback = 0;                               // no call-back since we use rx buffer - the same task is owner
    tInterfaceParameters.usConfig = USB_TERMINATING_ENDPOINT;            // {13} configure the IN endpoint to terminate messages with a zero length frame is a block transmission equals the endpoint size
        #if SIZE_OF_RAM < (16 * 1024)                                    // use smaller buffers when there is limited RAM available
    tInterfaceParameters.queue_sizes.RxQueueSize = 64;                   // optional input queue (used only when no call-back defined)
    tInterfaceParameters.queue_sizes.TxQueueSize = 256;                  // additional tx buffer
        #else
    tInterfaceParameters.queue_sizes.RxQueueSize = 256;                  // optional input queue (used only when no call-back defined)
    tInterfaceParameters.queue_sizes.TxQueueSize = 1024;                 // additional tx buffer
        #endif
        #if defined WAKE_BLOCKED_USB_TX
    tInterfaceParameters.low_water_level = (tInterfaceParameters.queue_sizes.TxQueueSize/2); // TX_FREE event on half buffer empty
        #endif
        #if defined MODBUS_USB_SLAVE && defined USE_USB_CDC && (MODBUS_USB_INTERFACE_BASE == 0) // {11}{23}
    tInterfaceParameters.owner_task = TASK_MODBUS;                       // receptions on this endpoint are to be handled by the MODBUS task
        #endif
    USBPortID_comms[0] = fnOpen(TYPE_USB, 0, &tInterfaceParameters);     // open the endpoints with defined configurations (initially inactive)
        #if defined MODBUS_USB_SLAVE && defined USE_USB_CDC && (MODBUS_USB_INTERFACE_BASE == 0)
    tInterfaceParameters.owner_task = OWN_TASK;
        #endif
        #if USB_CDC_COUNT > 1                                            // {33}
            #if SIZE_OF_RAM < (32 * 1024) && (USB_CDC_COUNT > 1)
    tInterfaceParameters.queue_sizes.RxQueueSize = 64;                   // reduce the queue sizes used for the extended CDC interfaces when there is little memory
    tInterfaceParameters.queue_sizes.TxQueueSize = 128;
            #endif
    while (iCDC_interface < USB_CDC_COUNT) {
            #if defined USB_SIMPLEX_ENDPOINTS
        endpointNumber++;
        tInterfaceParameters.Paired_RxEndpoint = tInterfaceParameters.Endpoint = endpointNumber++; 
            #else
        tInterfaceParameters.Paired_RxEndpoint = endpointNumber++;       // receiver (OUT)
        tInterfaceParameters.Endpoint = endpointNumber;                  // set USB endpoints to act as an input/output pair - transmitter (IN)
        endpointNumber += 2;
            #endif
        #if defined MODBUS_USB_SLAVE && defined USE_USB_CDC
        if ((iCDC_interface >= MODBUS_USB_INTERFACE_BASE) && (iCDC_interface < (MODBUS_USB_INTERFACE_BASE + MODBUS_USB_CDC_COUNT))) {
            tInterfaceParameters.owner_task = TASK_MODBUS;               // receptions on this endpoint are to be handled by the MODBUS task
        }
        #endif
        USBPortID_comms[iCDC_interface++] = fnOpen(TYPE_USB, 0, &tInterfaceParameters); // open the endpoints with defined configurations (initially inactive)
        #if defined MODBUS_USB_SLAVE && defined USE_USB_CDC
        tInterfaceParameters.owner_task = OWN_TASK;                      // USB task to handle next interfaces if not overwritten
        #endif
    }
        #endif
    tInterfaceParameters.usConfig = 0;                                   // {13}
        #if defined _LPC23XX || defined _LPC17XX                         // {14}
    tInterfaceParameters.Endpoint = 4;                                   // set interrupt endpoint
        #else
    tInterfaceParameters.Endpoint = endpointNumberInt;                   // set interrupt endpoint
        #endif
    tInterfaceParameters.Paired_RxEndpoint = 0;                          // no pairing
    tInterfaceParameters.owner_task = 0;                                 // don't wake task on reception
    tInterfaceParameters.usb_callback = 0;                               // no call back function
    tInterfaceParameters.queue_sizes.TxQueueSize = tInterfaceParameters.queue_sizes.RxQueueSize = 0; // no buffering
    USBPortID_interrupt[0] = fnOpen(TYPE_USB, 0, &tInterfaceParameters); // open the endpoint with defined configurations (initially inactive)
        #if USB_CDC_COUNT > 1                                            // {33}
    iCDC_interface = 1;
    while (iCDC_interface < USB_CDC_COUNT) {                             // configure endpoints for each CDC interface
            #if defined USB_SIMPLEX_ENDPOINTS
        endpointNumberInt += 2;                                          // 2 endpoints for each additional CDC interface (1 interrupt, 1 bulk IN/OUT)
            #else
        endpointNumberInt += 3;                                          // 3 endpoints for each additional CDC interface (1 interrupt, 1 bulk IN and 1 bulk OUT)
            #endif
        tInterfaceParameters.Endpoint = endpointNumberInt;
        USBPortID_interrupt[iCDC_interface++] = fnOpen(TYPE_USB, 0, &tInterfaceParameters); // open the endpoint with defined configurations (initially inactive)
    }
        #endif
    endpointNumber++;
    #endif
    #if defined USE_USB_MSD
        #if defined _LPC23XX || defined _LPC17XX                         // {14}
    tInterfaceParameters.Endpoint = 5;                                   // set USB endpoints to act as an input/output pair - transmitter (IN)
    tInterfaceParameters.Paired_RxEndpoint = 2;                          // receiver (OUT)
        #else
    tInterfaceParameters.Paired_RxEndpoint = endpointNumber;             // receiver (OUT)
            #if !defined USB_SIMPLEX_ENDPOINTS
    endpointNumber++;
            #endif
    tInterfaceParameters.Endpoint = endpointNumber++;                    // set USB endpoints to act as an input/output pair - transmitter (IN)
        #endif
    tInterfaceParameters.usb_callback = mass_storage_callback;           // allow receptions to be 'peeked' by call-back
    tInterfaceParameters.queue_sizes.RxQueueSize = 512;                  // optional input queue (used only when no call-back defined) and large enough to hold a full sector
    tInterfaceParameters.queue_sizes.TxQueueSize = 1024;                 // additional tx buffer
        #if defined WAKE_BLOCKED_USB_TX
    tInterfaceParameters.low_water_level = (tInterfaceParameters.queue_sizes.TxQueueSize/2); // TX_FREE event on half buffer empty
        #endif
    tInterfaceParameters.owner_task = OWN_TASK;                          // wake the USB task on receptions
    USBPortID_MSD = fnOpen(TYPE_USB, 0, &tInterfaceParameters);          // open the endpoints with defined configurations (initially inactive)
    #endif
    #if defined USE_USB_HID_MOUSE || defined USE_USB_HID_KEYBOARD        // {34}
    tInterfaceParameters.usEndpointSize = 8;                             // endpoint queue size (2 buffers of this size will be created for reception)
        #if defined _LPC23XX || defined _LPC17XX
    tInterfaceParameters.Endpoint = 5;
        #else
    tInterfaceParameters.Endpoint = endpointNumber++;                    // the endpoint used by HID device
        #endif
    tInterfaceParameters.usb_callback = 0;
    tInterfaceParameters.queue_sizes.TxQueueSize = 0;
    tInterfaceParameters.queue_sizes.RxQueueSize = 0;
    tInterfaceParameters.Paired_RxEndpoint = 0;                          // no pairing
        #if defined USE_USB_HID_MOUSE
    USBPortID_HID_mouse = fnOpen(TYPE_USB, 0, &tInterfaceParameters);    // open the endpoints with defined configurations (initially inactive)
        #endif
        #if defined USE_USB_HID_KEYBOARD
            #if defined USE_USB_HID_MOUSE
    tInterfaceParameters.Endpoint = endpointNumber++;                    // the endpoint used by HID keyboard
            #endif
            #if defined IN_COMPLETE_CALLBACK
    tInterfaceParameters.INcallback = fnKeyboardPolled;
            #endif
    USBPortID_HID_kb = fnOpen(TYPE_USB, 0, &tInterfaceParameters);       // open the endpoints with defined configurations (initially inactive)
        #endif
    #endif
    #if defined USE_USB_HID_RAW
    tInterfaceParameters.usEndpointSize = HID_RAW_RX_SIZE;               // endpoint queue size (2 buffers of this size will be created for reception)
    tInterfaceParameters.Endpoint = endpointNumber++;                    // the endpoint used by HID raw device for reception
    tInterfaceParameters.usb_callback = 0;
    tInterfaceParameters.queue_sizes.TxQueueSize = 0;
    tInterfaceParameters.queue_sizes.RxQueueSize = (2 * HID_RAW_RX_SIZE);// reception buffer with space for two complete HID raw messages
    tInterfaceParameters.Paired_RxEndpoint = endpointNumber++;
        #if defined IN_COMPLETE_CALLBACK
    tInterfaceParameters.INcallback = fnRawHIDPolled;
        #endif
    USBPortID_HID_raw = fnOpen(TYPE_USB, 0, &tInterfaceParameters);      // open the endpoints with defined configurations (initially inactive)
    #endif
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
    #if defined USB_HOST_SUPPORT                                         // {32} host mode rather than device mode
    tInterfaceParameters.usConfig = USB_HOST_MODE;                       // configure host mode of operation
    #else
        #if defined OLIMEX_LPC2378_STK                                   // {20} this board requires alternative USB pin mapping
    tInterfaceParameters.usConfig = (USB_FULL_SPEED | USB_ALT_PORT_MAPPING); // full-speed, rather than low-speed - use alternative pins
        #else
            #if defined USB_HS_INTERFACE                                 // {27}
    tInterfaceParameters.usConfig = USB_HIGH_SPEED;                      // user high speed interface
            #else
    tInterfaceParameters.usConfig = USB_FULL_SPEED;                      // full-speed, rather than low-speed
            #endif
        #endif
    #endif
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
    tInterfaceParameters.ucEndPoints = NUMBER_OF_ENDPOINTS;              // number of endpoints, in addition to EP0, required by the configuratzion
    #endif
    tInterfaceParameters.owner_task = OWN_TASK;                          // local task receives USB state change events
    #if defined USE_USB_OTG_CHARGE_PUMP
    tInterfaceParameters.OTG_I2C_Channel = IICPortID;                    // let the driver open its own IIC interface, if not yet open
    #endif
    USB_control = fnOpen(TYPE_USB, 0, &tInterfaceParameters);            // open the default control endpoint with defined configurations (reserves resources but only control is active)
    #if !defined USB_HOST_SUPPORT
    fnConfigureApplicationEndpoints();                                   // configure the endpoints that will be used by the class/application
    #endif
}
__PACK_OFF
#endif

