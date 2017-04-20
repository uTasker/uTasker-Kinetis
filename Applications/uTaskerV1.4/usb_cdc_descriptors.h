/**********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher

    ---------------------------------------------------------------------
    File:      usb_cdc_descriptors.h
    Project:   uTasker project
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2017
    *********************************************************************
    01.12.2015 Add RNDIS support
    23.12.2015 Add RAW HID and Audio support
    20.12.2016 Correct spelling of USB_MSB_OUT_ENDPOINT_NUMBER to USB_MSD_OUT_ENDPOINT_NUMBER {1}

*/

#if defined INCLUDE_USB_DEFINES
    #define USB_PRODUCT_RELEASE_NUMBER      0x0100                       // V1.0 (binary coded decimal)
    #if defined USB_HS_INTERFACE
        #define CDC_DATA_ENDPOINT_SIZE      512                          // maximum high speed bulk endpoint size
        #define RNDIS_DATA_ENDPOINT_SIZE    512                          // maximum high speed bulk endpoint size
    #else
        #define CDC_DATA_ENDPOINT_SIZE      64                           // maximum full speed bulk endpoint size
        #define RNDIS_DATA_ENDPOINT_SIZE    64                           // maximum full speed bulk endpoint size
    #endif

    #define CDC_SPEC_VERSION                USB_SPEC_VERSION_1_1         // CDC version
    #if !defined USB_CDC_VCOM_COUNT
        #define USB_CDC_VCOM_COUNT          USB_CDC_COUNT
    #endif

    #if defined USB_SIMPLEX_ENDPOINTS                                    // share endpoints for IN/OUT
        #define NUMBER_OF_CDC_ENDPOINTS     (2 * USB_CDC_COUNT)          // each CDC interface uses 2 endpoints (1 interrupt IN and 1 bulk IN/OUT) in addition to the default control endpoint 0
    #else
        #define NUMBER_OF_CDC_ENDPOINTS     (3 * USB_CDC_COUNT)          // each CDC interface uses 3 endpoints (2 IN and 1 OUT) in addition to the default control endpoint 0
    #endif
    #if defined USE_USB_MSD                                              // CDC with MSD composite
        #define USB_MSD_INTERFACE_COUNT     1
        #define USB_MSD_ENDPOINT_COUNT      2
        #define USB_MSD_OUT_ENDPOINT_NUMBER (NUMBER_OF_CDC_ENDPOINTS + 1)

        #if defined USB_SIMPLEX_ENDPOINTS
            #define USB_MSD_IN_ENDPOINT_NUMBER  USB_MSD_OUT_ENDPOINT_NUMBER
        #else
            #define USB_MSD_IN_ENDPOINT_NUMBER  (USB_MSD_OUT_ENDPOINT_NUMBER + 1)
        #endif
    #else
        #define USB_MSD_INTERFACE_COUNT     0
        #define USB_MSD_ENDPOINT_COUNT      0
        #define USB_MSD_IN_ENDPOINT_NUMBER  (NUMBER_OF_CDC_ENDPOINTS + 1)
    #endif
    #define USB_MOUSE_IN_ENDPOINT_NUMBER (USB_MSD_IN_ENDPOINT_NUMBER + USB_MSD_ENDPOINT_COUNT)
    #if defined USE_USB_HID_MOUSE                                        // CDC with HID mouse
        #define USB_HID_MOUSE_INTERFACE_COUNT 1
        #define USB_HID_MOUSE_ENDPOINT_COUNT  1
        #define USB_MOUSE_INTERFACE_NUMBER    ((USB_CDC_COUNT * 2) + 1  + USB_MSD_INTERFACE_COUNT)
    #else
        #define USB_HID_MOUSE_INTERFACE_COUNT 0
        #define USB_HID_MOUSE_ENDPOINT_COUNT  0
    #endif
    #define USB_KEYBOARD_IN_ENDPOINT_NUMBER (USB_MOUSE_IN_ENDPOINT_NUMBER + USB_HID_MOUSE_ENDPOINT_COUNT)
    #if defined USE_USB_HID_KEYBOARD                                     // CDC with HID keyboard
        #define USB_HID_KB_INTERFACE_COUNT  1
        #define USB_HID_KB_ENDPOINT_COUNT   1
        #define USB_KEYBOARD_INTERFACE_NUMBER  ((USB_CDC_COUNT * 2) + 1 + USB_MSD_INTERFACE_COUNT + USB_HID_MOUSE_INTERFACE_COUNT)
    #else
        #define USB_HID_KB_INTERFACE_COUNT  0
        #define USB_HID_KB_ENDPOINT_COUNT   0
    #endif
    #define USB_HID_RAW_IN_ENDPOINT_NUMBER (USB_KEYBOARD_IN_ENDPOINT_NUMBER + USB_HID_KB_ENDPOINT_COUNT)
    #if defined USE_USB_HID_RAW                                          // CDC with HID RAW device
        #define USB_HID_RAW_INTERFACE_COUNT 1
        #define USB_HID_RAW_ENDPOINT_COUNT  2
    #else
        #define USB_HID_RAW_INTERFACE_COUNT 0
        #define USB_HID_RAW_ENDPOINT_COUNT  0
    #endif
    #define USB_AUDIO_OUT_ENDPOINT_NUMBER (USB_HID_RAW_IN_ENDPOINT_NUMBER + USB_HID_RAW_INTERFACE_COUNT)
    #if defined USB_SIMPLEX_ENDPOINTS
        #define USB_AUDIO_IN_ENDPOINT_NUMBER USB_AUDIO_OUT_ENDPOINT_NUMBER
    #else
        #define USB_AUDIO_IN_ENDPOINT_NUMBER (USB_AUDIO_OUT_ENDPOINT_NUMBER + 1)
    #endif
    #if defined USE_USB_AUDIO                                            // CDC with audio class
        #define USB_AUDIO_INTERFACE_COUNT   3
        #if defined USB_SIMPLEX_ENDPOINTS
            #define USB_AUDIO_ENDPOINT_COUNT    1
        #else
            #define USB_AUDIO_ENDPOINT_COUNT    2
        #endif
    #else
        #define USB_AUDIO_INTERFACE_COUNT   0
        #define USB_AUDIO_ENDPOINT_COUNT    0
    #endif
    #define AUDIO_CONTROL_INTERFACE  ((USB_CDC_COUNT * 2) + USB_MSD_INTERFACE_COUNT + USB_HID_MOUSE_INTERFACE_COUNT + USB_HID_KB_INTERFACE_COUNT + USB_HID_RAW_INTERFACE_COUNT)
    #define AUDIO_SINK_INTERFACE     (AUDIO_CONTROL_INTERFACE + 1)
    #define AUDIO_SOURCE_INTERFACE   (AUDIO_SINK_INTERFACE + 1)


    #define USB_INTERFACE_COUNT  ((2 * USB_CDC_COUNT) + USB_MSD_INTERFACE_COUNT + USB_HID_MOUSE_INTERFACE_COUNT + USB_HID_KB_INTERFACE_COUNT + USB_HID_RAW_INTERFACE_COUNT + USB_AUDIO_INTERFACE_COUNT) // configuration number (2 interfaces for each CDC plus 1 each for available MSD, mouse and keyboard, and 3 for audio)
    #define NUMBER_OF_ENDPOINTS  (NUMBER_OF_CDC_ENDPOINTS + USB_MSD_ENDPOINT_COUNT + USB_HID_MOUSE_ENDPOINT_COUNT + USB_HID_KB_ENDPOINT_COUNT + USB_HID_RAW_ENDPOINT_COUNT + USB_AUDIO_ENDPOINT_COUNT)

    #if defined _M5223X || defined _KINETIS
        #define USB_VENDOR_ID               0x15a2                       // Freescale vendor ID
        #if defined FREEMASTER_CDC
            #define USB_PRODUCT_ID          0x0072
        #elif defined USE_USB_MSD || defined USE_USB_HID_MOUSE || defined USE_USB_HID_KEYBOARD || defined USE_USB_AUDIO // composite
            #define USB_PRODUCT_ID          (0xff42 + USB_CDC_COUNT)     // unofficial test value
        #elif defined USB_CDC_RNDIS
            #define USB_PRODUCT_ID          (0xf045 - 0x1235)            // unofficial test value
        #else
            #define USB_PRODUCT_ID          0x0044                       // uTasker Freescale development CDC product ID
        #endif
    #elif defined _LM3SXXXX
        #define USB_VENDOR_ID               0x1cbe                       // {3} Luminary Micro, Inc. vendor ID
        #define USB_PRODUCT_ID              0x0101                       // {6} uTasker Luminary development CDC product ID
    #elif defined _HW_SAM7X || defined _HW_AVR32
        #define USB_VENDOR_ID               0x03eb                       // {7} ATMEL Corp. vendor ID
        #define USB_PRODUCT_ID              0x21fd                       // {7} uTasker development CDC product ID
    #else
        #define USB_VENDOR_ID               0x4321                       // non-official test VID
        #define USB_PRODUCT_ID              0x1221                       // non-official test CDC PID
    #endif

    #if defined USB_STRING_OPTION                                        // if our project supports strings
        #define MANUFACTURER_STRING_INDEX       1                        // index must match with order in the string list
        #define PRODUCT_STRING_INDEX            2                        // to remove a particular string from the list set to zero
        #define SERIAL_NUMBER_STRING_INDEX      3
        #define CONFIGURATION_STRING_INDEX      4
        #if defined USB_CDC_RNDIS
            #define RNDIS_INTERFACE_STRING_INDEX  5
            #if defined USB_CDC_VCOM_COUNT
                #define INTERFACE_STRING_INDEX  6
            #else
                #define INTERFACE_STRING_INDEX  (RNDIS_INTERFACE_STRING_INDEX)
            #endif
            #define MSD_INTERFACE_STRING_INDEX  7
        #else
            #define INTERFACE_STRING_INDEX      5
            #define MSD_INTERFACE_STRING_INDEX  6
        #endif
        #if defined USE_USB_MSD
            #define LAST_STRING_INDEX           MSD_INTERFACE_STRING_INDEX // last string entry - used for protection against invalid string index requests
        #else
            #define LAST_STRING_INDEX           INTERFACE_STRING_INDEX // last string entry - used for protection against invalid string index requests
        #endif

        #define UNICODE_LANGUAGE_INDEX          UNICODE_ENGLISH_LANGUAGE // English language used by strings

                                                                         // the characters in the string must be entered as 16 bit unicode in little-endian order!!
                                                                         // the first entry is the length of the content (including the length and descriptor type string entries)
        static const unsigned char usb_language_string[]   = {4,  DESCRIPTOR_TYPE_STRING, LITTLE_SHORT_WORD_BYTES(UNICODE_LANGUAGE_INDEX)}; // this is compulsory first string
        static const unsigned char manufacturer_str[]      = {10, DESCRIPTOR_TYPE_STRING, 'M',0, 'a',0, 'n',0, 'u',0};
        static const unsigned char product_str[]           = {16, DESCRIPTOR_TYPE_STRING, 'M',0, 'y',0, ' ',0, 'P',0, 'r',0, 'o',0, 'd',0};
        #if defined USB_RUN_TIME_DEFINABLE_STRINGS
            static const unsigned char serial_number_str[] = {0};        // the application delivers this string (generated at run time)
        #else
            static const unsigned char serial_number_str[] = {10, DESCRIPTOR_TYPE_STRING, '0',0, '0',0, '0',0, '1',0};
        #endif
        static const unsigned char config_str[]            = {10, DESCRIPTOR_TYPE_STRING, 'C',0, 'o',0, 'n',0, 'f',0};
        #if defined USB_CDC_RNDIS
            static const unsigned char interface_str_rndis[] = {12,  DESCRIPTOR_TYPE_STRING, 'R',0, 'N',0, 'D',0, 'I',0, 'S',0};
        #endif
        #if (USB_CDC_VCOM_COUNT > 0)
            static const unsigned char interface_str_cdc[] = {10,  DESCRIPTOR_TYPE_STRING, 'V',0, 'C',0, 'o',0, 'm',0};
        #endif
        #if defined USE_USB_MSD
            static const unsigned char usb_msd_str[]       = {8,  DESCRIPTOR_TYPE_STRING, 'M',0, 'S',0, 'D',0};
        #endif
        #if defined USE_USB_AUDIO
            static const unsigned char interface_str_audio[] = {12,  DESCRIPTOR_TYPE_STRING, 'A',0, 'u',0, 'd',0, 'i',0, 'o',0};
        #endif

        static const unsigned char *ucStringTable[]        = {usb_language_string, manufacturer_str, product_str, serial_number_str, config_str,
        #if defined USB_CDC_RNDIS
                                                              interface_str_rndis,
        #endif
        #if (USB_CDC_VCOM_COUNT > 0)
                                                              interface_str_cdc,
        #endif
        #if defined USE_USB_MSD
                                                              usb_msd_str,
        #endif
        #if defined USE_USB_AUDIO
                                                              interface_str_audio,
        #endif
                                                             };
    #endif
#endif


#if defined INCLUDE_USB_CLASS_CONSTS
    #if defined USB_CDC_RNDIS
static const REMOTE_NDIS_RESPONSE_AVAILABLE ResponseAvailable = {
    TYPE_NOTIFICATION_REQUEST,
    RESPONSE_NOTIFICATION_AVAILABLE,
    {LITTLE_SHORT_WORD_BYTES(0)},
    {LITTLE_SHORT_WORD_BYTES(0)},
    {LITTLE_SHORT_WORD_BYTES(0)}
};
    #endif
#endif


#if defined INCLUDE_USB_CONFIG_DESCRIPTOR
    #if !defined USB_CDC_RNDIS_COUNT
        #define USB_CDC_RNDIS_COUNT      0
    #endif
#define FIRST_CDC_RNDIS_INTERFACE        0                               // RNDIS virtual network adapter interfaces are always at the start
#define FIRST_CDC_INTERFACE              USB_CDC_RNDIS_COUNT             // CDC virtual COM interfaces follow
typedef struct _PACK stUSB_CONFIGURATION_DESCRIPTOR_COLLECTION
{
    USB_CONFIGURATION_DESCRIPTOR               config_desc_cdc;          // compulsory configuration descriptor

  //#if USB_CDC_COUNT > 1                                                // the interface association descriptor is use as standard so that it achieves the same endpoint usage whether multiple CDC interfaces are used or not
    USB_INTERFACE_ASSOCIATION_DESCRIPTOR       cdc_interface_0;          // if there are multiple CDC interfaces an interface association descriptor is required
  //#endif

    USB_INTERFACE_DESCRIPTOR                   interface_desc_0;         // first interface descriptor
        USB_CDC_FUNCTIONAL_DESCRIPTOR_HEADER   CDC_func_header_0;        // CDC function descriptors due to class used
        USB_CDC_FUNCTIONAL_DESCRIPTOR_CALL_MAN CDC_call_management_0;
        USB_CDC_FUNCTIONAL_DESCRIPTOR_ABSTRACT_CONTROL  CDC_abstract_control_0;
        USB_CDC_FUNCTIONAL_DESCRIPTOR_UNION    CDC_union_0;
    USB_ENDPOINT_DESCRIPTOR                    endpoint_3;               // endpoint of first interface

    USB_INTERFACE_DESCRIPTOR                   interface_desc_1;         // second interface descriptor
    USB_ENDPOINT_DESCRIPTOR                    endpoint_1;               // endpoints of second interface
    USB_ENDPOINT_DESCRIPTOR                    endpoint_2;

    #if USB_CDC_COUNT > 1
    USB_INTERFACE_ASSOCIATION_DESCRIPTOR       cdc_interface_1;
    USB_INTERFACE_DESCRIPTOR                   interface_desc_2;         // first interface descriptor for second connection
        USB_CDC_FUNCTIONAL_DESCRIPTOR_HEADER   CDC_func_header_1;        // CDC function descriptors due to class used
        USB_CDC_FUNCTIONAL_DESCRIPTOR_CALL_MAN CDC_call_management_1;
        USB_CDC_FUNCTIONAL_DESCRIPTOR_ABSTRACT_CONTROL  CDC_abstract_control_1;
        USB_CDC_FUNCTIONAL_DESCRIPTOR_UNION    CDC_union_1;
    USB_ENDPOINT_DESCRIPTOR                    endpoint_6;               // endpoint of this interface

    USB_INTERFACE_DESCRIPTOR                   interface_desc_3;         // second interface descriptor for second connection
    USB_ENDPOINT_DESCRIPTOR                    endpoint_4;               // endpoints of second interface
    USB_ENDPOINT_DESCRIPTOR                    endpoint_5;
    #endif

    #if USB_CDC_COUNT > 2
    USB_INTERFACE_ASSOCIATION_DESCRIPTOR       cdc_interface_2;
    USB_INTERFACE_DESCRIPTOR                   interface_desc_4;         // first interface descriptor for third connection
        USB_CDC_FUNCTIONAL_DESCRIPTOR_HEADER   CDC_func_header_2;        // CDC function descriptors due to class used
        USB_CDC_FUNCTIONAL_DESCRIPTOR_CALL_MAN CDC_call_management_2;
        USB_CDC_FUNCTIONAL_DESCRIPTOR_ABSTRACT_CONTROL  CDC_abstract_control_2;
        USB_CDC_FUNCTIONAL_DESCRIPTOR_UNION    CDC_union_2;
    USB_ENDPOINT_DESCRIPTOR                    endpoint_9;               // endpoint of this interface

    USB_INTERFACE_DESCRIPTOR                   interface_desc_5;         // second interface descriptor for third connection
    USB_ENDPOINT_DESCRIPTOR                    endpoint_7;               // endpoints of third interface
    USB_ENDPOINT_DESCRIPTOR                    endpoint_8;
    #endif

    #if USB_CDC_COUNT > 3
    USB_INTERFACE_ASSOCIATION_DESCRIPTOR       cdc_interface_3;
    USB_INTERFACE_DESCRIPTOR                   interface_desc_6;         // first interface descriptor for fourth connection
        USB_CDC_FUNCTIONAL_DESCRIPTOR_HEADER   CDC_func_header_3;        // CDC function descriptors due to class used
        USB_CDC_FUNCTIONAL_DESCRIPTOR_CALL_MAN CDC_call_management_3;
        USB_CDC_FUNCTIONAL_DESCRIPTOR_ABSTRACT_CONTROL  CDC_abstract_control_3;
        USB_CDC_FUNCTIONAL_DESCRIPTOR_UNION    CDC_union_3;
    USB_ENDPOINT_DESCRIPTOR                    endpoint_12;              // endpoint of this interface

    USB_INTERFACE_DESCRIPTOR                   interface_desc_7;         // second interface descriptor for fourth connection
    USB_ENDPOINT_DESCRIPTOR                    endpoint_10;              // endpoints of fourth interface
    USB_ENDPOINT_DESCRIPTOR                    endpoint_11;
    #endif

    #if USB_CDC_COUNT > 4
    USB_INTERFACE_ASSOCIATION_DESCRIPTOR       cdc_interface_4;
    USB_INTERFACE_DESCRIPTOR                   interface_desc_8;         // first interface descriptor for fifth connection
        USB_CDC_FUNCTIONAL_DESCRIPTOR_HEADER   CDC_func_header_5;        // CDC function descriptors due to class used
        USB_CDC_FUNCTIONAL_DESCRIPTOR_CALL_MAN CDC_call_management_4;
        USB_CDC_FUNCTIONAL_DESCRIPTOR_ABSTRACT_CONTROL  CDC_abstract_control_4;
        USB_CDC_FUNCTIONAL_DESCRIPTOR_UNION    CDC_union_4;
    USB_ENDPOINT_DESCRIPTOR                    endpoint_15;              // endpoint of this interface

    USB_INTERFACE_DESCRIPTOR                   interface_desc_9;         // second interface descriptor for fifth connection
    USB_ENDPOINT_DESCRIPTOR                    endpoint_13;              // endpoints of fifth interface
    USB_ENDPOINT_DESCRIPTOR                    endpoint_14;
    #endif

    #if USB_CDC_COUNT > 5
    USB_INTERFACE_ASSOCIATION_DESCRIPTOR       cdc_interface_5;
    USB_INTERFACE_DESCRIPTOR                   interface_desc_10;        // first interface descriptor for sixth connection
        USB_CDC_FUNCTIONAL_DESCRIPTOR_HEADER   CDC_func_header_6;        // CDC function descriptors due to class used
        USB_CDC_FUNCTIONAL_DESCRIPTOR_CALL_MAN CDC_call_management_5;
        USB_CDC_FUNCTIONAL_DESCRIPTOR_ABSTRACT_CONTROL  CDC_abstract_control_5;
        USB_CDC_FUNCTIONAL_DESCRIPTOR_UNION    CDC_union_5;
    USB_ENDPOINT_DESCRIPTOR                    endpoint_18;              // endpoint of this interface

    USB_INTERFACE_DESCRIPTOR                   interface_desc_11;        // second interface descriptor for sixth connection
    USB_ENDPOINT_DESCRIPTOR                    endpoint_16;              // endpoints of sixth interface
    USB_ENDPOINT_DESCRIPTOR                    endpoint_17;
    #endif
    #if defined USE_USB_MSD
    USB_INTERFACE_DESCRIPTOR                   interface_desc_msd;       // USB-MSD interface descriptor
    USB_ENDPOINT_DESCRIPTOR                    msd_endpoint_1;           // endpoints of USB-MSD interface
    USB_ENDPOINT_DESCRIPTOR                    msd_endpoint_2;
    #endif
    #if defined USE_USB_HID_MOUSE
    USB_INTERFACE_DESCRIPTOR                   interface_desc_mouse;     // first interface descriptor
    USB_HID_DESCRIPTOR                         hid_desc_mouse;           // HID descriptor
    USB_ENDPOINT_DESCRIPTOR                    endpoint_mouse;           // single endpoint
    #endif
    #if defined USE_USB_HID_KEYBOARD
    USB_INTERFACE_DESCRIPTOR                   interface_desc_keyboard;  // first interface descriptor
    USB_HID_DESCRIPTOR                         hid_desc_keyboard;        // HID descriptor
    USB_ENDPOINT_DESCRIPTOR                    endpoint_keyboard;        // single endpoint
    #endif
    #if defined USE_USB_HID_RAW
    USB_INTERFACE_DESCRIPTOR                   interface_desc_raw;       // first interface descriptor
    USB_HID_DESCRIPTOR                         hid_desc_raw;             // HID descriptor
    USB_ENDPOINT_DESCRIPTOR                    endpoint_raw_1;           // endpoint
    USB_ENDPOINT_DESCRIPTOR                    endpoint_raw_2;           // endpoint
    #endif
    #if defined USE_USB_AUDIO
    USB_INTERFACE_ASSOCIATION_DESCRIPTOR       audio_interface_0;        // interface association descriptor

    USB_INTERFACE_DESCRIPTOR                   interface_desc_audio_0;   // first interface descriptor
    USB_AUDIO_CONTROL_INTERFACE_HEADER_DESCRIPTOR_2 audio_control_header1; // audio control interface header descriptor
    USB_AUDIO_INPUT_TERMINAL_DESCRIPTOR        in_terminal_0;            // input terminal descriptor - loud-speaker
    USB_AUDIO_FEATURE_UNIT_DESCRIPTOR          feature_0;                // feature unit descriptor
    USB_AUDIO_OUTPUT_TERMINAL_DESCRIPTOR       out_terminal_0;           // output terminal descriptor - loud-speaker
    USB_AUDIO_INPUT_TERMINAL_DESCRIPTOR        in_terminal_1;            // input terminal descriptor - microphone
    USB_AUDIO_OUTPUT_TERMINAL_DESCRIPTOR       out_terminal_1;           // output terminal descriptor - microphone
    USB_INTERFACE_DESCRIPTOR                   interface_desc_audio_1;   // second interface descriptor (zero bandwidth)
    USB_INTERFACE_DESCRIPTOR                   interface_desc_audio_2;   // second interface descriptor (alternative - streaming)
    USB_AUDIO_STREAMING_GENERAL_INTERFACE_DESCRIPTOR streaming_desc1;    // streaming descriptor
    USB_AUDIO_TYPE_I_FORMAT_TYPE_DESCRIPTOR    format_1;                 // format type I descriptor
    USB_AUDIO_CONTROL_ENDPOINT_DESCRIPTOR      endpoint_audio_0;         // first audio endpoint
    USB_ISO_AUDIO_DATA_ENDPOINT_DESCRIPTOR     iso_audio_desc1;          // isochronous audio data endpoint descriptor
    USB_INTERFACE_DESCRIPTOR                   interface_desc_audio_3;   // third interface  (zero bandwidth)
    USB_INTERFACE_DESCRIPTOR                   interface_desc_audio_4;   // third interface descriptor (aternative - streaming)
    USB_AUDIO_STREAMING_GENERAL_INTERFACE_DESCRIPTOR streaming_desc2;    // streaming descriptor
    USB_AUDIO_TYPE_I_FORMAT_TYPE_DESCRIPTOR    format_2;                 // format type I descriptor
    USB_AUDIO_CONTROL_ENDPOINT_DESCRIPTOR      endpoint_audio_1;         // second audio endpoint
    USB_ISO_AUDIO_DATA_ENDPOINT_DESCRIPTOR     iso_audio_desc2;          // isochronous audio data endpoint descriptor
    #endif
} USB_CONFIGURATION_DESCRIPTOR_COLLECTION;
#endif

#if defined INCLUDE_USB_DEVICE_DESCRIPTOR
static const USB_DEVICE_DESCRIPTOR device_descriptor = {                 // constant device descriptor
    STANDARD_DEVICE_DESCRIPTOR_LENGTH,                                   // standard device descriptor length (0x12)
    DESCRIPTOR_TYPE_DEVICE,                                              // 0x01
    {LITTLE_SHORT_WORD_BYTES(USB_SPEC_VERSION)},                         // USB1.1 or USB2
    DEVICE_CLASS_MISC_INTERFACE_ASSOCIATION_DESC,                        // uses interface association descriptors (we always use an interface association descriptor since in simplifies the driver installation)
    ENDPOINT_0_SIZE,                                                     // size of endpoint reception buffer
    {LITTLE_SHORT_WORD_BYTES(USB_VENDOR_ID)},                            // our vendor ID
    {LITTLE_SHORT_WORD_BYTES(USB_PRODUCT_ID)},                           // our product ID
    {LITTLE_SHORT_WORD_BYTES(USB_PRODUCT_RELEASE_NUMBER)},               // product release number
    #if defined USB_STRING_OPTION                                        // if we support strings add the data here
    MANUFACTURER_STRING_INDEX, PRODUCT_STRING_INDEX, SERIAL_NUMBER_STRING_INDEX, // fixed string table indexes
    #else
    0, 0, 0,                                                             // used when no strings are supported
    #endif
    NUMBER_OF_POSSIBLE_CONFIGURATIONS                                    // number of configurations possible
};

static const USB_CONFIGURATION_DESCRIPTOR_COLLECTION config_descriptor = {
    {                                                                    // config descriptor
    DESCRIPTOR_TYPE_CONFIGURATION_LENGTH,                                // length (0x09)
    DESCRIPTOR_TYPE_CONFIGURATION,                                       // 0x02
    {LITTLE_SHORT_WORD_BYTES(sizeof(USB_CONFIGURATION_DESCRIPTOR_COLLECTION))}, // total length (little-endian)
    USB_INTERFACE_COUNT,                                                 // the number of interfaces
    1,                                                                   // configuration value
    #if defined USB_STRING_OPTION
    CONFIGURATION_STRING_INDEX,                                          // string index to configuration
    #else
    0,                                                                   // zero when strings are not supported
    #endif
    (SELF_POWERED | ATTRIBUTE_DEFAULT),                                  // attributes for configuration,
    0                                                                    // consumption in 2mA steps (eg. 100/2 for 100mA)
    },                                                                   // end of compulsory config descriptor

    // Interface association descriptor for first CDC interface
    // - we always use an interface association descriptor since it simplifies the driver installation
    //
    {                                                                    // interface association descriptor required when there is more than one CDC channel
    DESCRIPTOR_TYPE_INTERFACE_ASSOCIATION_LENGTH,                        // 0x08
    DESCRIPTOR_TYPE_INTERFACE_ASSOCIATION,                               // 0x0b
    0,                                                                   // first interface number
    2,                                                                   // interface count
    #if defined USB_CDC_RNDIS
    DEVICE_CLASS_COMMUNICATION_AND_CONTROL_MANUAL,                       // 0x02, 0x00, 0x00
    #else
    DEVICE_CLASS_COMMUNICATION_AND_CONTROL,                              // 0x02, 0x02, 0x00
    #endif
    0                                                                    // string reference
    },

    // Interface descriptor for first CDC - control interface
    //
    {                                                                    // interface descriptor
    DESCRIPTOR_TYPE_INTERFACE_LENGTH,                                    // length (0x09)
    DESCRIPTOR_TYPE_INTERFACE,                                           // 0x04
    0,                                                                   // interface number 0
    0,                                                                   // alternative setting 0
    1,                                                                   // number of endpoints in addition to EP0
    USB_CLASS_COMMUNICATION_CONTROL,                                     // interface class (0x02)
    USB_ABSTRACT_LINE_CONTROL_MODEL,                                     // interface sub-class (0x02)
    #if defined USB_CDC_RNDIS
    0xff,                                                                // vendor specific
    #else
    0,                                                                   // interface protocol
    #endif
    #if defined USB_STRING_OPTION
        #if defined USB_CDC_RNDIS
    RNDIS_INTERFACE_STRING_INDEX,                                        // string index for interface
        #else
    INTERFACE_STRING_INDEX,                                              // string index for interface
        #endif
    #else
    0,                                                                   // zero when strings are not supported
    #endif
    },                                                                   // end of interface descriptor

    // Function descriptors for first CDC - control interface
    //
    {                                                                    // function descriptors
    USB_CDC_FUNCTIONAL_DESCRIPTOR_HEADER_LENGTH,                         // descriptor size in bytes (0x05)
    CS_INTERFACE,                                                        // type field (0x24)
    HEADER_FUNCTION_DESCRIPTOR,                                          // header descriptor (0x00)
    {LITTLE_SHORT_WORD_BYTES(CDC_SPEC_VERSION)}                          // {4} specification version
    },
    {
    USB_CDC_FUNCTIONAL_DESCRIPTOR_CALL_MAN_LENGTH,                       // descriptor size in bytes (0x05)
    CS_INTERFACE,                                                        // type field (0x24)
    CALL_MAN_FUNCTIONAL_DESCRIPTOR,                                      // call management function descriptor (0x01)
    #if defined USB_CDC_RNDIS
    CALL_MAN_FUNCTIONAL_CAPABILITY_NO_CALL_MANAGEMENT,                   // capabilities
    0                                                                    // data interface
    #else
    CALL_MAN_FUNCTIONAL_CAPABILITY_HANDLES_CALL_MANAGEMENT,              // capabilities
    0                                                                    // data interface
    #endif
    },                                                                   // end of function descriptors
    {
    USB_CDC_FUNCTIONAL_DESCRIPTOR_ABSTRACT_CONTROL_LENGTH,               // descriptor size in bytes (0x04)
    CS_INTERFACE,                                                        // type field (0x24)
    ABSTRACT_CONTROL_FUNCTION_DESCRIPTOR,                                // abstract control descriptor (0x02)
    #if defined USB_CDC_RNDIS
    (ABSTRACT_CONTROL_FUNCTION_CAPABILITY_LINE_STATE_CODING | ABSTRACT_CONTROL_FUNCTION_CAPABILITY_SEND_BREAK) // capabilities
    #else
    ABSTRACT_CONTROL_FUNCTION_CAPABILITY_LINE_STATE_CODING               // capabilities
    #endif
    },
    {
    USB_CDC_FUNCTIONAL_DESCRIPTOR_UNION_LENGTH,                          // descriptor size in bytes (0x05)
    CS_INTERFACE,                                                        // type field (0x24)
    UNION_FUNCTIONAL_DESCRIPTOR,                                         // union function descriptor (0x06)
    0,                                                                   // control interface
    1                                                                    // subordinate interface
    },                                                                   // end of function descriptors

    // Interrupt endpoint descriptor for first CDC - control interface
    //
    {                                                                    // interrupt endpoint descriptor for first interface
    DESCRIPTOR_TYPE_ENDPOINT_LENGTH,                                     // descriptor size in bytes (0x07)
    DESCRIPTOR_TYPE_ENDPOINT,                                            // endpoint descriptor (0x05)
    #if defined _LPC23XX || defined _LPC17XX                             // {14}
    (IN_ENDPOINT | 0x04),                                                // direction and address of endpoint (endpoint 4 is interrupt)
    #else
        #if defined USB_SIMPLEX_ENDPOINTS
    (IN_ENDPOINT | 0x02),                                                // direction and address of endpoint
        #else
    (IN_ENDPOINT | 0x03),                                                // direction and address of endpoint
        #endif
    #endif
    ENDPOINT_INTERRUPT,                                                  // endpoint attributes
    #if defined USB_CDC_RNDIS
    {LITTLE_SHORT_WORD_BYTES(16)},                                       // endpoint FIFO size (little-endian)
    #else
    {LITTLE_SHORT_WORD_BYTES(64)},                                       // endpoint FIFO size (little-endian)
    #endif
    10                                                                   // polling interval in ms
    },                                                                   // end of endpoint descriptor

    // Interface descriptor for first CDC - data interface
    //
    {                                                                    // the first CDC data interface
    DESCRIPTOR_TYPE_INTERFACE_LENGTH,                                    // descriptor size in bytes (0x09)
    DESCRIPTOR_TYPE_INTERFACE,                                           // interface descriptor (0x04)
    1,                                                                   // interface number 1
    0,                                                                   // no alternative setting
    2,                                                                   // 2 endpoints
    INTERFACE_CLASS_COMMUNICATION_DATA,                                  // 0x0a
    0,                                                                   // sub-class
    0,                                                                   // interface protocol
    #if defined USB_STRING_OPTION
    INTERFACE_STRING_INDEX,                                              // string index for interface
    #else
    0                                                                    // zero when strings are not supported
    #endif
    },

    // Bulk endpoint descriptors for first CDC - data interface
    //
    {                                                                    // bulk out endpoint descriptor for the first CDC data interface
    DESCRIPTOR_TYPE_ENDPOINT_LENGTH,                                     // descriptor size in bytes (0x07)
    DESCRIPTOR_TYPE_ENDPOINT,                                            // endpoint descriptor (0x05)
    #if defined _LPC23XX || defined _LPC17XX                             // {14}
    (OUT_ENDPOINT | 0x02),                                               // direction and address of endpoint (endpoint 2 is bulk)
    #else
    (OUT_ENDPOINT | 0x01),                                               // direction and address of endpoint
    #endif
    ENDPOINT_BULK,                                                       // endpoint attributes
    #if defined USB_CDC_RNDIS
    {LITTLE_SHORT_WORD_BYTES(RNDIS_DATA_ENDPOINT_SIZE)},                 // endpoint FIFO size (little-endian)
    #else
    {LITTLE_SHORT_WORD_BYTES(CDC_DATA_ENDPOINT_SIZE)},                   // endpoint FIFO size (little-endian)
    #endif
    0                                                                    // polling interval in ms - ignored for bulk
    },

    {                                                                    // bulk in endpoint descriptor for the first CDC data interface
    DESCRIPTOR_TYPE_ENDPOINT_LENGTH,                                     // descriptor size in bytes (0x07)
    DESCRIPTOR_TYPE_ENDPOINT,                                            // endpoint descriptor (0x05)
    #if defined _LPC23XX || defined _LPC17XX                             // {14}
    (IN_ENDPOINT | 0x05),                                                // direction and address of endpoint (endpoint 5 is bulk)
    #else
        #if defined USB_SIMPLEX_ENDPOINTS
    (IN_ENDPOINT | 0x01),                                                // direction and address of endpoint
        #else
    (IN_ENDPOINT | 0x02),                                                // direction and address of endpoint
        #endif
    #endif
    ENDPOINT_BULK,                                                       // endpoint attributes
    #if defined USB_CDC_RNDIS
    {LITTLE_SHORT_WORD_BYTES(RNDIS_DATA_ENDPOINT_SIZE)},                 // endpoint FIFO size (little-endian)
    #else
    {LITTLE_SHORT_WORD_BYTES(CDC_DATA_ENDPOINT_SIZE)},                   // endpoint FIFO size (little-endian)
    #endif
    0                                                                    // polling interval in ms - ignored for bulk
    },

    #if USB_CDC_COUNT > 1                                                // {33} at least 2 CDC interfaces
    // Interface association descriptor for second CDC interface
    //
    {                                                                    // interface association descriptor
    DESCRIPTOR_TYPE_INTERFACE_ASSOCIATION_LENGTH,                        // 0x08
    DESCRIPTOR_TYPE_INTERFACE_ASSOCIATION,                               // 0x0b
    2,                                                                   // second interface number
    2,                                                                   // interface count
    DEVICE_CLASS_COMMUNICATION_AND_CONTROL,                              // 0x02, 0x02, 0x00
    0                                                                    // string reference
    },

    // Interface descriptor for second CDC - control interface
    //
    {                                                                    // interface descriptor
    DESCRIPTOR_TYPE_INTERFACE_LENGTH,                                    // length (0x09)
    DESCRIPTOR_TYPE_INTERFACE,                                           // 0x04
    2,                                                                   // interface number 2
    0,                                                                   // alternative setting 0
    1,                                                                   // number of endpoints in addition to EP0
    USB_CLASS_COMMUNICATION_CONTROL,                                     // interface class (0x02)
    USB_ABSTRACT_LINE_CONTROL_MODEL,                                     // interface sub-class (0x02)
    0,                                                                   // interface protocol
        #if defined USB_STRING_OPTION
    INTERFACE_STRING_INDEX,                                              // string index for interface
        #else
    0,                                                                   // zero when strings are not supported
        #endif
    },                                                                   // end of interface descriptor

    // Function descriptors for second CDC - control interface
    //
    {                                                                    // function descriptors
    USB_CDC_FUNCTIONAL_DESCRIPTOR_HEADER_LENGTH,                         // descriptor size in bytes (0x05)
    CS_INTERFACE,                                                        // type field (0x24)
    HEADER_FUNCTION_DESCRIPTOR,                                          // header descriptor (0x00)
    {LITTLE_SHORT_WORD_BYTES(CDC_SPEC_VERSION)}                          // {4} specification version
    },
    {
    USB_CDC_FUNCTIONAL_DESCRIPTOR_CALL_MAN_LENGTH,                       // descriptor size in bytes (0x05)
    CS_INTERFACE,                                                        // type field (0x24)
    CALL_MAN_FUNCTIONAL_DESCRIPTOR,                                      // call management function descriptor (0x01)
    CALL_MAN_FUNCTIONAL_CAPABILITY_HANDLES_CALL_MANAGEMENT,              // capabilities
    0                                                                    // data interface
    },                                                                   // end of function descriptors
    {
    USB_CDC_FUNCTIONAL_DESCRIPTOR_ABSTRACT_CONTROL_LENGTH,               // descriptor size in bytes (0x04)
    CS_INTERFACE,                                                        // type field (0x24)
    ABSTRACT_CONTROL_FUNCTION_DESCRIPTOR,                                // abstract control descriptor (0x02)
    ABSTRACT_CONTROL_FUNCTION_CAPABILITY_LINE_STATE_CODING               // capabilities
    },
    {
    USB_CDC_FUNCTIONAL_DESCRIPTOR_UNION_LENGTH,                          // descriptor size in bytes (0x05)
    CS_INTERFACE,                                                        // type field (0x24)
    UNION_FUNCTIONAL_DESCRIPTOR,                                         // union function descriptor (0x06)
    0,                                                                   // control interface
    1                                                                    // subordinate interface
    },                                                                   // end of function descriptors

    // Interrupt endpoint descriptor for second CDC - control interface
    //
    {                                                                    // interrupt endpoint descriptor for first interface
    DESCRIPTOR_TYPE_ENDPOINT_LENGTH,                                     // descriptor size in bytes (0x07)
    DESCRIPTOR_TYPE_ENDPOINT,                                            // endpoint descriptor (0x05)
        #if defined USB_SIMPLEX_ENDPOINTS
    (IN_ENDPOINT | 0x04),                                                // direction and address of endpoint
        #else
    (IN_ENDPOINT | 0x06),                                                // direction and address of endpoint
        #endif
    ENDPOINT_INTERRUPT,                                                  // endpoint attributes
    {LITTLE_SHORT_WORD_BYTES(64)},                                       // endpoint FIFO size (little-endian)
    10                                                                   // polling interval in ms
    },                                                                   // end of endpoint descriptor

    // Interface descriptor for second CDC - data interface
    //
    {                                                                    // the second CDC data interface
    DESCRIPTOR_TYPE_INTERFACE_LENGTH,                                    // descriptor size in bytes (0x09)
    DESCRIPTOR_TYPE_INTERFACE,                                           // interface descriptor (0x04)
    3,                                                                   // interface number 3
    0,                                                                   // no alternative setting
    2,                                                                   // 2 endpoints
    INTERFACE_CLASS_COMMUNICATION_DATA,                                  // 0x0a
    0,                                                                   // sub-class
    0,                                                                   // interface protocol
        #if defined USB_STRING_OPTION
    INTERFACE_STRING_INDEX,                                              // string index for interface
        #else
    0                                                                    // zero when strings are not supported
        #endif
    },

    // Bulk endpoint descriptors for second CDC - data interface
    //
    {                                                                    // bulk out endpoint descriptor for the second CDC data interface
    DESCRIPTOR_TYPE_ENDPOINT_LENGTH,                                     // descriptor size in bytes (0x07)
    DESCRIPTOR_TYPE_ENDPOINT,                                            // endpoint descriptor (0x05)
        #if defined USB_SIMPLEX_ENDPOINTS
    (OUT_ENDPOINT | 0x03),                                               // direction and address of endpoint
        #else
    (OUT_ENDPOINT | 0x04),                                               // direction and address of endpoint
        #endif
    ENDPOINT_BULK,                                                       // endpoint attributes
    {LITTLE_SHORT_WORD_BYTES(CDC_DATA_ENDPOINT_SIZE)},                   // endpoint FIFO size (little-endian)
    0                                                                    // polling interval in ms - ignored for bulk
    },

    {                                                                    // bulk in endpoint descriptor for the second CDC data interface
    DESCRIPTOR_TYPE_ENDPOINT_LENGTH,                                     // descriptor size in bytes (0x07)
    DESCRIPTOR_TYPE_ENDPOINT,                                            // endpoint descriptor (0x05)
        #if defined USB_SIMPLEX_ENDPOINTS
    (IN_ENDPOINT | 0x03),                                                // direction and address of endpoint
        #else
    (IN_ENDPOINT | 0x05),                                                // direction and address of endpoint
        #endif
    ENDPOINT_BULK,                                                       // endpoint attributes
    {LITTLE_SHORT_WORD_BYTES(CDC_DATA_ENDPOINT_SIZE)},                   // endpoint FIFO size (little-endian)
    0                                                                    // polling interval in ms - ignored for bulk
    },
    #endif
    #if USB_CDC_COUNT > 2                                                // {33}
    {                                                                    // interface association descriptor
    DESCRIPTOR_TYPE_INTERFACE_ASSOCIATION_LENGTH,                        // 0x08
    DESCRIPTOR_TYPE_INTERFACE_ASSOCIATION,                               // 0x0b
    4,                                                                   // first interface number
    2,                                                                   // interface count
    DEVICE_CLASS_COMMUNICATION_AND_CONTROL,
    0                                                                    // string reference
    },

    {                                                                    // interface descriptor
    DESCRIPTOR_TYPE_INTERFACE_LENGTH,                                    // length (0x09)
    DESCRIPTOR_TYPE_INTERFACE,                                           // 0x04
    4,                                                                   // interface number 4
    0,                                                                   // alternative setting 0
    1,                                                                   // number of endpoints in addition to EP0
    USB_CLASS_COMMUNICATION_CONTROL,                                     // interface class (0x02)
    USB_ABSTRACT_LINE_CONTROL_MODEL,                                     // interface sub-class (0x02)
    0,                                                                   // interface protocol
        #if defined USB_STRING_OPTION
    INTERFACE_STRING_INDEX,                                              // string index for interface
        #else
    0,                                                                   // zero when strings are not supported
        #endif
    },                                                                   // end of interface descriptor

    {                                                                    // function descriptors
    USB_CDC_FUNCTIONAL_DESCRIPTOR_HEADER_LENGTH,                         // descriptor size in bytes (0x05)
    CS_INTERFACE,                                                        // type field (0x24)
    HEADER_FUNCTION_DESCRIPTOR,                                          // header descriptor (0x00)
    {LITTLE_SHORT_WORD_BYTES(CDC_SPEC_VERSION)}                          // {4} specification version
    },

    {
    USB_CDC_FUNCTIONAL_DESCRIPTOR_CALL_MAN_LENGTH,                       // descriptor size in bytes (0x05)
    CS_INTERFACE,                                                        // type field (0x24)
    CALL_MAN_FUNCTIONAL_DESCRIPTOR,                                      // call management function descriptor (0x01)
    CALL_MAN_FUNCTIONAL_CAPABILITY_HANDLES_CALL_MANAGEMENT,              // capabilities
    0                                                                    // data interface
    },                                                                   // end of function descriptors

    {
    USB_CDC_FUNCTIONAL_DESCRIPTOR_ABSTRACT_CONTROL_LENGTH,               // descriptor size in bytes (0x04)
    CS_INTERFACE,                                                        // type field (0x24)
    ABSTRACT_CONTROL_FUNCTION_DESCRIPTOR,                                // abstract control descriptor (0x02)
    ABSTRACT_CONTROL_FUNCTION_CAPABILITY_LINE_STATE_CODING               // capabilities
    },

    {
    USB_CDC_FUNCTIONAL_DESCRIPTOR_UNION_LENGTH,                          // descriptor size in bytes (0x05)
    CS_INTERFACE,                                                        // type field (0x24)
    UNION_FUNCTIONAL_DESCRIPTOR,                                         // union function descriptor (0x06)
    0,                                                                   // control interface
    1                                                                    // subordinate interface
    },                                                                   // end of function descriptors

    {                                                                    // interrupt endpoint descriptor for first interface
    DESCRIPTOR_TYPE_ENDPOINT_LENGTH,                                     // descriptor size in bytes (0x07)
    DESCRIPTOR_TYPE_ENDPOINT,                                            // endpoint descriptor (0x05)
        #if defined USB_SIMPLEX_ENDPOINTS
    (IN_ENDPOINT | 0x06),                                                // direction and address of endpoint
        #else
    (IN_ENDPOINT | 0x09),                                                // direction and address of endpoint
        #endif
    ENDPOINT_INTERRUPT,                                                  // endpoint attributes
    {LITTLE_SHORT_WORD_BYTES(64)},                                       // endpoint FIFO size (little-endian)
    10                                                                   // polling interval in ms
    },                                                                   // end of endpoint descriptor

    {                                                                    // the second interface
    DESCRIPTOR_TYPE_INTERFACE_LENGTH,                                    // descriptor size in bytes (0x09)
    DESCRIPTOR_TYPE_INTERFACE,                                           // interface descriptor (0x04)
    5,                                                                   // interface number 5
    0,                                                                   // no alternative setting
    2,                                                                   // 2 endpoints
    INTERFACE_CLASS_COMMUNICATION_DATA,                                  // 0x0a
    0,                                                                   // sub-class
    0,                                                                   // interface protocol
        #if defined USB_STRING_OPTION
    INTERFACE_STRING_INDEX,                                              // string index for interface
        #else
    0                                                                    // zero when strings are not supported
        #endif
    },

    {                                                                    // bulk out endpoint descriptor for the second interface
    DESCRIPTOR_TYPE_ENDPOINT_LENGTH,                                     // descriptor size in bytes (0x07)
    DESCRIPTOR_TYPE_ENDPOINT,                                            // endpoint descriptor (0x05)
        #if defined USB_SIMPLEX_ENDPOINTS
    (OUT_ENDPOINT | 0x05),                                               // direction and address of endpoint
        #else
    (OUT_ENDPOINT | 0x07),                                               // direction and address of endpoint
        #endif
    ENDPOINT_BULK,                                                       // endpoint attributes
    {LITTLE_SHORT_WORD_BYTES(CDC_DATA_ENDPOINT_SIZE)},                   // endpoint FIFO size (little-endian)
    0                                                                    // polling interval in ms - ignored for bulk
    },

    {                                                                    // bulk in endpoint descriptor for the second interface
    DESCRIPTOR_TYPE_ENDPOINT_LENGTH,                                     // descriptor size in bytes (0x07)
    DESCRIPTOR_TYPE_ENDPOINT,                                            // endpoint descriptor (0x05)
        #if defined USB_SIMPLEX_ENDPOINTS
    (IN_ENDPOINT | 0x05),                                                // direction and address of endpoint
        #else
    (IN_ENDPOINT | 0x08),                                                // direction and address of endpoint
        #endif
    ENDPOINT_BULK,                                                       // endpoint attributes
    {LITTLE_SHORT_WORD_BYTES(CDC_DATA_ENDPOINT_SIZE)},                   // endpoint FIFO size (little-endian)
    0                                                                    // polling interval in ms - ignored for bulk
    },
    #endif
    #if USB_CDC_COUNT > 3                                                // {33}
    {                                                                    // interface association descriptor
    DESCRIPTOR_TYPE_INTERFACE_ASSOCIATION_LENGTH,                        // 0x08
    DESCRIPTOR_TYPE_INTERFACE_ASSOCIATION,                               // 0x0b
    6,                                                                   // first interface number
    2,                                                                   // interface count
    DEVICE_CLASS_COMMUNICATION_AND_CONTROL,
    0                                                                    // string reference
    },

    {                                                                    // interface descriptor
    DESCRIPTOR_TYPE_INTERFACE_LENGTH,                                    // length (0x09)
    DESCRIPTOR_TYPE_INTERFACE,                                           // 0x04
    6,                                                                   // interface number 6
    0,                                                                   // alternative setting 0
    1,                                                                   // number of endpoints in addition to EP0
    USB_CLASS_COMMUNICATION_CONTROL,                                     // interface class (0x02)
    USB_ABSTRACT_LINE_CONTROL_MODEL,                                     // interface sub-class (0x02)
    0,                                                                   // interface protocol
        #if defined USB_STRING_OPTION
    INTERFACE_STRING_INDEX,                                              // string index for interface
        #else
    0,                                                                   // zero when strings are not supported
        #endif
    },                                                                   // end of interface descriptor

    {                                                                    // function descriptors
    USB_CDC_FUNCTIONAL_DESCRIPTOR_HEADER_LENGTH,                         // descriptor size in bytes (0x05)
    CS_INTERFACE,                                                        // type field (0x24)
    HEADER_FUNCTION_DESCRIPTOR,                                          // header descriptor (0x00)
    {LITTLE_SHORT_WORD_BYTES(CDC_SPEC_VERSION)}                          // {4} specification version
    },

    {
    USB_CDC_FUNCTIONAL_DESCRIPTOR_CALL_MAN_LENGTH,                       // descriptor size in bytes (0x05)
    CS_INTERFACE,                                                        // type field (0x24)
    CALL_MAN_FUNCTIONAL_DESCRIPTOR,                                      // call management function descriptor (0x01)
    CALL_MAN_FUNCTIONAL_CAPABILITY_HANDLES_CALL_MANAGEMENT,              // capabilities
    0                                                                    // data interface
    },                                                                   // end of function descriptors

    {
    USB_CDC_FUNCTIONAL_DESCRIPTOR_ABSTRACT_CONTROL_LENGTH,               // descriptor size in bytes (0x04)
    CS_INTERFACE,                                                        // type field (0x24)
    ABSTRACT_CONTROL_FUNCTION_DESCRIPTOR,                                // abstract control descriptor (0x02)
    ABSTRACT_CONTROL_FUNCTION_CAPABILITY_LINE_STATE_CODING               // capabilities
    },

    {
    USB_CDC_FUNCTIONAL_DESCRIPTOR_UNION_LENGTH,                          // descriptor size in bytes (0x05)
    CS_INTERFACE,                                                        // type field (0x24)
    UNION_FUNCTIONAL_DESCRIPTOR,                                         // union function descriptor (0x06)
    0,                                                                   // control interface
    1                                                                    // subordinate interface
    },                                                                   // end of function descriptors

    {                                                                    // interrupt endpoint descriptor for first interface
    DESCRIPTOR_TYPE_ENDPOINT_LENGTH,                                     // descriptor size in bytes (0x07)
    DESCRIPTOR_TYPE_ENDPOINT,                                            // endpoint descriptor (0x05)
        #if defined USB_SIMPLEX_ENDPOINTS
    (IN_ENDPOINT | 0x08),                                                // direction and address of endpoint
        #else
    (IN_ENDPOINT | 0x0c),                                                // direction and address of endpoint
        #endif
    ENDPOINT_INTERRUPT,                                                  // endpoint attributes
    {LITTLE_SHORT_WORD_BYTES(64)},                                       // endpoint FIFO size (little-endian)
    10                                                                   // polling interval in ms
    },                                                                   // end of endpoint descriptor

    {                                                                    // the second interface
    DESCRIPTOR_TYPE_INTERFACE_LENGTH,                                    // descriptor size in bytes (0x09)
    DESCRIPTOR_TYPE_INTERFACE,                                           // interface descriptor (0x04)
    7,                                                                   // interface number 7
    0,                                                                   // no alternative setting
    2,                                                                   // 2 endpoints
    INTERFACE_CLASS_COMMUNICATION_DATA,                                  // 0x0a
    0,                                                                   // sub-class
    0,                                                                   // interface protocol
        #if defined USB_STRING_OPTION
    INTERFACE_STRING_INDEX,                                              // string index for interface
        #else
    0                                                                    // zero when strings are not supported
        #endif
    },

    {                                                                    // bulk out endpoint descriptor for the second interface
    DESCRIPTOR_TYPE_ENDPOINT_LENGTH,                                     // descriptor size in bytes (0x07)
    DESCRIPTOR_TYPE_ENDPOINT,                                            // endpoint descriptor (0x05)
        #if defined USB_SIMPLEX_ENDPOINTS
    (OUT_ENDPOINT | 0x07),                                               // direction and address of endpoint
        #else
    (OUT_ENDPOINT | 0x0a),                                               // direction and address of endpoint
        #endif
    ENDPOINT_BULK,                                                       // endpoint attributes
    {LITTLE_SHORT_WORD_BYTES(CDC_DATA_ENDPOINT_SIZE)},                   // endpoint FIFO size (little-endian)
    0                                                                    // polling interval in ms - ignored for bulk
    },

    {                                                                    // bulk in endpoint descriptor for the second interface
    DESCRIPTOR_TYPE_ENDPOINT_LENGTH,                                     // descriptor size in bytes (0x07)
    DESCRIPTOR_TYPE_ENDPOINT,                                            // endpoint descriptor (0x05)
        #if defined USB_SIMPLEX_ENDPOINTS
    (IN_ENDPOINT | 0x07),                                                // direction and address of endpoint
        #else
    (IN_ENDPOINT | 0x0b),                                                // direction and address of endpoint
        #endif
    ENDPOINT_BULK,                                                       // endpoint attributes
    {LITTLE_SHORT_WORD_BYTES(CDC_DATA_ENDPOINT_SIZE)},                   // endpoint FIFO size (little-endian)
    0                                                                    // polling interval in ms - ignored for bulk
    },
    #endif
    #if USB_CDC_COUNT > 4
    {                                                                    // interface association descriptor
    DESCRIPTOR_TYPE_INTERFACE_ASSOCIATION_LENGTH,                        // 0x08
    DESCRIPTOR_TYPE_INTERFACE_ASSOCIATION,                               // 0x0b
    8,                                                                   // first interface number
    2,                                                                   // interface count
    DEVICE_CLASS_COMMUNICATION_AND_CONTROL,
    0                                                                    // string reference
    },

    {                                                                    // interface descriptor
    DESCRIPTOR_TYPE_INTERFACE_LENGTH,                                    // length (0x09)
    DESCRIPTOR_TYPE_INTERFACE,                                           // 0x04
    8,                                                                   // interface number 8
    0,                                                                   // alternative setting 0
    1,                                                                   // number of endpoints in addition to EP0
    USB_CLASS_COMMUNICATION_CONTROL,                                     // interface class (0x02)
    USB_ABSTRACT_LINE_CONTROL_MODEL,                                     // interface sub-class (0x02)
    0,                                                                   // interface protocol
        #if defined USB_STRING_OPTION
    INTERFACE_STRING_INDEX,                                              // string index for interface
        #else
    0,                                                                   // zero when strings are not supported
        #endif
    },                                                                   // end of interface descriptor

    {                                                                    // function descriptors
    USB_CDC_FUNCTIONAL_DESCRIPTOR_HEADER_LENGTH,                         // descriptor size in bytes (0x05)
    CS_INTERFACE,                                                        // type field (0x24)
    HEADER_FUNCTION_DESCRIPTOR,                                          // header descriptor (0x00)
    {LITTLE_SHORT_WORD_BYTES(CDC_SPEC_VERSION)}                          // {4} specification version
    },

    {
    USB_CDC_FUNCTIONAL_DESCRIPTOR_CALL_MAN_LENGTH,                       // descriptor size in bytes (0x05)
    CS_INTERFACE,                                                        // type field (0x24)
    CALL_MAN_FUNCTIONAL_DESCRIPTOR,                                      // call management function descriptor (0x01)
    CALL_MAN_FUNCTIONAL_CAPABILITY_HANDLES_CALL_MANAGEMENT,              // capabilities
    0                                                                    // data interface
    },                                                                   // end of function descriptors

    {
    USB_CDC_FUNCTIONAL_DESCRIPTOR_ABSTRACT_CONTROL_LENGTH,               // descriptor size in bytes (0x04)
    CS_INTERFACE,                                                        // type field (0x24)
    ABSTRACT_CONTROL_FUNCTION_DESCRIPTOR,                                // abstract control descriptor (0x02)
    ABSTRACT_CONTROL_FUNCTION_CAPABILITY_LINE_STATE_CODING               // capabilities
    },

    {
    USB_CDC_FUNCTIONAL_DESCRIPTOR_UNION_LENGTH,                          // descriptor size in bytes (0x05)
    CS_INTERFACE,                                                        // type field (0x24)
    UNION_FUNCTIONAL_DESCRIPTOR,                                         // union function descriptor (0x06)
    0,                                                                   // control interface
    1                                                                    // subordinate interface
    },                                                                   // end of function descriptors

    {                                                                    // interrupt endpoint descriptor for first interface
    DESCRIPTOR_TYPE_ENDPOINT_LENGTH,                                     // descriptor size in bytes (0x07)
    DESCRIPTOR_TYPE_ENDPOINT,                                            // endpoint descriptor (0x05)
        #if defined USB_SIMPLEX_ENDPOINTS
    (IN_ENDPOINT | 0x0a),                                                // direction and address of endpoint
        #else
    (IN_ENDPOINT | 0x0f),                                                // direction and address of endpoint
        #endif
    ENDPOINT_INTERRUPT,                                                  // endpoint attributes
    {LITTLE_SHORT_WORD_BYTES(64)},                                       // endpoint FIFO size (little-endian)
    10                                                                   // polling interval in ms
    },                                                                   // end of endpoint descriptor

    {                                                                    // the second interface
    DESCRIPTOR_TYPE_INTERFACE_LENGTH,                                    // descriptor size in bytes (0x09)
    DESCRIPTOR_TYPE_INTERFACE,                                           // interface descriptor (0x04)
    9,                                                                   // interface number 9
    0,                                                                   // no alternative setting
    2,                                                                   // 2 endpoints
    INTERFACE_CLASS_COMMUNICATION_DATA,                                  // 0x0a
    0,                                                                   // sub-class
    0,                                                                   // interface protocol
        #if defined USB_STRING_OPTION
    INTERFACE_STRING_INDEX,                                              // string index for interface
        #else
    0                                                                    // zero when strings are not supported
        #endif
    },

    {                                                                    // bulk out endpoint descriptor for the second interface
    DESCRIPTOR_TYPE_ENDPOINT_LENGTH,                                     // descriptor size in bytes (0x07)
    DESCRIPTOR_TYPE_ENDPOINT,                                            // endpoint descriptor (0x05)
        #if defined USB_SIMPLEX_ENDPOINTS
    (OUT_ENDPOINT | 0x09),                                               // direction and address of endpoint
        #else
    (OUT_ENDPOINT | 0x0d),                                               // direction and address of endpoint
        #endif
    ENDPOINT_BULK,                                                       // endpoint attributes
    {LITTLE_SHORT_WORD_BYTES(CDC_DATA_ENDPOINT_SIZE)},                   // endpoint FIFO size (little-endian)
    0                                                                    // polling interval in ms - ignored for bulk
    },

    {                                                                    // bulk in endpoint descriptor for the second interface
    DESCRIPTOR_TYPE_ENDPOINT_LENGTH,                                     // descriptor size in bytes (0x07)
    DESCRIPTOR_TYPE_ENDPOINT,                                            // endpoint descriptor (0x05)
        #if defined USB_SIMPLEX_ENDPOINTS
    (IN_ENDPOINT | 0x09),                                                // direction and address of endpoint
        #else
    (IN_ENDPOINT | 0x0e),                                                // direction and address of endpoint
        #endif
    ENDPOINT_BULK,                                                       // endpoint attributes
    {LITTLE_SHORT_WORD_BYTES(CDC_DATA_ENDPOINT_SIZE)},                   // endpoint FIFO size (little-endian)
    0                                                                    // polling interval in ms - ignored for bulk
    },
    #endif
    #if (USB_CDC_COUNT > 5) && defined USB_SIMPLEX_ENDPOINTS             // 6 CDC interfaces are only possible when using the bulk IN/OUT on a single endpoint
    {                                                                    // interface association descriptor
    DESCRIPTOR_TYPE_INTERFACE_ASSOCIATION_LENGTH,                        // 0x08
    DESCRIPTOR_TYPE_INTERFACE_ASSOCIATION,                               // 0x0b
    10,                                                                  // first interface number
    2,                                                                   // interface count
    DEVICE_CLASS_COMMUNICATION_AND_CONTROL,
    0                                                                    // string reference
    },

    {                                                                    // interface descriptor
    DESCRIPTOR_TYPE_INTERFACE_LENGTH,                                    // length (0x09)
    DESCRIPTOR_TYPE_INTERFACE,                                           // 0x04
    10,                                                                  // interface number 10
    0,                                                                   // alternative setting 0
    1,                                                                   // number of endpoints in addition to EP0
    USB_CLASS_COMMUNICATION_CONTROL,                                     // interface class (0x02)
    USB_ABSTRACT_LINE_CONTROL_MODEL,                                     // interface sub-class (0x02)
    0,                                                                   // interface protocol
        #if defined USB_STRING_OPTION
    INTERFACE_STRING_INDEX,                                              // string index for interface
        #else
    0,                                                                   // zero when strings are not supported
        #endif
    },                                                                   // end of interface descriptor

    {                                                                    // function descriptors
    USB_CDC_FUNCTIONAL_DESCRIPTOR_HEADER_LENGTH,                         // descriptor size in bytes (0x05)
    CS_INTERFACE,                                                        // type field (0x24)
    HEADER_FUNCTION_DESCRIPTOR,                                          // header descriptor (0x00)
    {LITTLE_SHORT_WORD_BYTES(CDC_SPEC_VERSION)}                          // {4} specification version
    },

    {
    USB_CDC_FUNCTIONAL_DESCRIPTOR_CALL_MAN_LENGTH,                       // descriptor size in bytes (0x05)
    CS_INTERFACE,                                                        // type field (0x24)
    CALL_MAN_FUNCTIONAL_DESCRIPTOR,                                      // call management function descriptor (0x01)
    CALL_MAN_FUNCTIONAL_CAPABILITY_HANDLES_CALL_MANAGEMENT,              // capabilities
    0                                                                    // data interface
    },                                                                   // end of function descriptors

    {
    USB_CDC_FUNCTIONAL_DESCRIPTOR_ABSTRACT_CONTROL_LENGTH,               // descriptor size in bytes (0x04)
    CS_INTERFACE,                                                        // type field (0x24)
    ABSTRACT_CONTROL_FUNCTION_DESCRIPTOR,                                // abstract control descriptor (0x02)
    ABSTRACT_CONTROL_FUNCTION_CAPABILITY_LINE_STATE_CODING               // capabilities
    },

    {
    USB_CDC_FUNCTIONAL_DESCRIPTOR_UNION_LENGTH,                          // descriptor size in bytes (0x05)
    CS_INTERFACE,                                                        // type field (0x24)
    UNION_FUNCTIONAL_DESCRIPTOR,                                         // union function descriptor (0x06)
    0,                                                                   // control interface
    1                                                                    // subordinate interface
    },                                                                   // end of function descriptors

    {                                                                    // interrupt endpoint descriptor for first interface
    DESCRIPTOR_TYPE_ENDPOINT_LENGTH,                                     // descriptor size in bytes (0x07)
    DESCRIPTOR_TYPE_ENDPOINT,                                            // endpoint descriptor (0x05)
    (IN_ENDPOINT | 0x0c),                                                // direction and address of endpoint
    ENDPOINT_INTERRUPT,                                                  // endpoint attributes
    {LITTLE_SHORT_WORD_BYTES(64)},                                       // endpoint FIFO size (little-endian)
    10                                                                   // polling interval in ms
    },                                                                   // end of endpoint descriptor

    {                                                                    // the second interface
    DESCRIPTOR_TYPE_INTERFACE_LENGTH,                                    // descriptor size in bytes (0x09)
    DESCRIPTOR_TYPE_INTERFACE,                                           // interface descriptor (0x04)
    11,                                                                  // interface number 11
    0,                                                                   // no alternative setting
    2,                                                                   // 2 endpoints
    INTERFACE_CLASS_COMMUNICATION_DATA,                                  // 0x0a
    0,                                                                   // sub-class
    0,                                                                   // interface protocol
        #if defined USB_STRING_OPTION
    INTERFACE_STRING_INDEX,                                              // string index for interface
        #else
    0                                                                    // zero when strings are not supported
        #endif
    },

    {                                                                    // bulk out endpoint descriptor for the second interface
    DESCRIPTOR_TYPE_ENDPOINT_LENGTH,                                     // descriptor size in bytes (0x07)
    DESCRIPTOR_TYPE_ENDPOINT,                                            // endpoint descriptor (0x05)
    (OUT_ENDPOINT | 0x0b),                                               // direction and address of endpoint
    ENDPOINT_BULK,                                                       // endpoint attributes
    {LITTLE_SHORT_WORD_BYTES(CDC_DATA_ENDPOINT_SIZE)},                   // endpoint FIFO size (little-endian)
    0                                                                    // polling interval in ms - ignored for bulk
    },

    {                                                                    // bulk in endpoint descriptor for the second interface
    DESCRIPTOR_TYPE_ENDPOINT_LENGTH,                                     // descriptor size in bytes (0x07)
    DESCRIPTOR_TYPE_ENDPOINT,                                            // endpoint descriptor (0x05)
    (IN_ENDPOINT | 0x0b),                                                // direction and address of endpoint
    ENDPOINT_BULK,                                                       // endpoint attributes
    {LITTLE_SHORT_WORD_BYTES(CDC_DATA_ENDPOINT_SIZE)},                   // endpoint FIFO size (little-endian)
    0                                                                    // polling interval in ms - ignored for bulk
    },
    #endif
    #if defined USE_USB_MSD
    {                                                                    // interface descriptor
    DESCRIPTOR_TYPE_INTERFACE_LENGTH,                                    // length (0x09)
    DESCRIPTOR_TYPE_INTERFACE,                                           // 0x04
    (USB_CDC_COUNT * 2),                                                 // interface number
    0,                                                                   // alternative setting 0
    2,                                                                   // number of endpoints in addition to EP0
    INTERFACE_CLASS_MASS_STORAGE,                                        // interface class (0x08)
    GENERIC_SCSI_MEDIA,                                                  // interface sub-class (0x06)
    BULK_ONLY_TRANSPORT,                                                 // interface protocol (0x50)
    #if defined USB_STRING_OPTION
    MSD_INTERFACE_STRING_INDEX,                                          // string index for interface
    #else
    0,                                                                   // zero when strings are not supported
    #endif
    },                                                                   // end of interface descriptor

    {                                                                    // bulk out endpoint descriptor for the second interface
    DESCRIPTOR_TYPE_ENDPOINT_LENGTH,                                     // descriptor size in bytes (0x07)
    DESCRIPTOR_TYPE_ENDPOINT,                                            // endpoint descriptor (0x05)
    (OUT_ENDPOINT | USB_MSD_OUT_ENDPOINT_NUMBER),                        // {} direction and address of endpoint
    ENDPOINT_BULK,                                                       // endpoint attributes
    {LITTLE_SHORT_WORD_BYTES(64)},                                       // endpoint FIFO size (little-endian - 64 bytes)
    0                                                                    // polling interval in ms - ignored for bulk
    },

    {                                                                    // bulk in endpoint descriptor for the second interface
    DESCRIPTOR_TYPE_ENDPOINT_LENGTH,                                     // descriptor size in bytes (0x07)
    DESCRIPTOR_TYPE_ENDPOINT,                                            // endpoint descriptor (0x05)
    (IN_ENDPOINT | USB_MSD_IN_ENDPOINT_NUMBER),                          // direction and address of endpoint
    ENDPOINT_BULK,                                                       // endpoint attributes
    {LITTLE_SHORT_WORD_BYTES(64)},                                       // endpoint FIFO size (little-endian - 64 bytes)
    0                                                                    // polling interval in ms - ignored for bulk
    },
    #endif
    #if defined USE_USB_HID_MOUSE
    {                                                                    // interface descriptor
    DESCRIPTOR_TYPE_INTERFACE_LENGTH,                                    // length (0x09)
    DESCRIPTOR_TYPE_INTERFACE,                                           // 0x04
    (USB_MOUSE_INTERFACE_NUMBER),                                        // interface number
    0,                                                                   // alternative setting 0
    1,                                                                   // number of endpoints in addition to EP0
    USB_CLASS_HID,                                                       // interface class (0x03)
    USB_SUBCLASS_BOOT_INTERFACE,                                         // interface sub-class (0x01)
    USB_INTERFACE_PROTOCOL_MOUSE,                                        // interface protocol (0x02)
        #if defined USB_STRING_OPTION
    INTERFACE_STRING_INDEX,                                              // string index for interface
        #else
    0,                                                                   // zero when strings are not supported
        #endif
    },                                                                   // end of interface descriptor

    {                                                                    // HID descriptor
    DESCRIPTOR_TYPE_HID_LENGTH,                                          // descriptor size in bytes
    DESCRIPTOR_TYPE_HID,                                                 // device descriptor type (0x21)
    {LITTLE_SHORT_WORD_BYTES(1)},                                        // HID class specific release number
    0,                                                                   // hardware target country
    1,                                                                   // number of HID class descriptors to follow
    DESCRIPTOR_TYPE_REPORT,                                              // descriptor type (0x22)
    {LITTLE_SHORT_WORD_BYTES(sizeof(ucMouseReport))},                    // total length of report descriptor
    },

    {                                                                    // interrupt in endpoint descriptor for the interface
    DESCRIPTOR_TYPE_ENDPOINT_LENGTH,                                     // descriptor size in bytes (0x07)
    DESCRIPTOR_TYPE_ENDPOINT,                                            // endpoint descriptor (0x05)
    (IN_ENDPOINT | USB_MOUSE_IN_ENDPOINT_NUMBER),                        // direction and address of endpoint
    ENDPOINT_INTERRUPT,                                                  // endpoint attributes
    {LITTLE_SHORT_WORD_BYTES(8)},                                        // endpoint FIFO size (little-endian - 8 bytes)
    10                                                                   // polling interval in ms
    },
    #endif
    #if defined USE_USB_HID_KEYBOARD
    {                                                                    // interface descriptor
    DESCRIPTOR_TYPE_INTERFACE_LENGTH,                                    // length (0x09)
    DESCRIPTOR_TYPE_INTERFACE,                                           // 0x04
    USB_KEYBOARD_INTERFACE_NUMBER,                                       // interface number
    0,                                                                   // alternative setting 0
    1,                                                                   // number of endpoints in addition to EP0
    USB_CLASS_HID,                                                       // interface class (0x03)
    USB_SUBCLASS_BOOT_INTERFACE,                                         // interface sub-class (0x01)
    USB_INTERFACE_PROTOCOL_KEYBOARD,                                     // interface protocol (0x02)
        #if defined USB_STRING_OPTION
    INTERFACE_STRING_INDEX,                                              // string index for interface
        #else
    0,                                                                   // zero when strings are not supported
        #endif
    },                                                                   // end of interface descriptor

    {                                                                    // HID descriptor
    DESCRIPTOR_TYPE_HID_LENGTH,                                          // descriptor size in bytes
    DESCRIPTOR_TYPE_HID,                                                 // device descriptor type (0x21)
    {LITTLE_SHORT_WORD_BYTES(1)},                                        // HID class specific release number
    0,                                                                   // hardware target country
    1,                                                                   // number of HID class descriptors to follow
    DESCRIPTOR_TYPE_REPORT,                                              // descriptor type (0x22)
    {LITTLE_SHORT_WORD_BYTES(sizeof(ucKeyboardReport))},                 // total length of report descriptor
    },

    {                                                                    // interrupt in endpoint descriptor for the interface
    DESCRIPTOR_TYPE_ENDPOINT_LENGTH,                                     // descriptor size in bytes (0x07)
    DESCRIPTOR_TYPE_ENDPOINT,                                            // endpoint descriptor (0x05)
    (IN_ENDPOINT | USB_KEYBOARD_IN_ENDPOINT_NUMBER),                     // direction and address of endpoint
    ENDPOINT_INTERRUPT,                                                  // endpoint attributes
    {LITTLE_SHORT_WORD_BYTES(8)},                                        // endpoint FIFO size (little-endian - 8 bytes)
    10                                                                   // polling interval in ms
    }
    #endif

    #if defined USE_USB_HID_RAW
    {                                                                    // interface descriptor
    DESCRIPTOR_TYPE_INTERFACE_LENGTH,                                    // length (0x09)
    DESCRIPTOR_TYPE_INTERFACE,                                           // 0x04
    ((USB_CDC_COUNT * 2) + 1 + USB_MSD_INTERFACE_COUNT + USB_HID_MOUSE_INTERFACE_COUNT + USB_HID_KB_INTERFACE_COUNT), // interface number
    0,                                                                   // alternative setting 0
    2,                                                                   // number of endpoints in addition to EP0
    USB_CLASS_HID,                                                       // interface class (0x03)
    0,                                                                   // interface sub-class (0x00)
    0,                                                                   // interface protocol (0x00)
    #if defined USB_STRING_OPTION
    INTERFACE_STRING_INDEX,                                              // string index for interface
    #else
    0,                                                                   // zero when strings are not supported
    #endif
    },                                                                   // end of interface descriptor

    {                                                                    // HID descriptor
    DESCRIPTOR_TYPE_HID_LENGTH,                                          // descriptor size in bytes (0x9)
    DESCRIPTOR_TYPE_HID,                                                 // device descriptor type (0x21)
    {LITTLE_SHORT_WORD_BYTES(0x111)},                                    // HID class specific release number
    0,                                                                   // hardware target country
    1,                                                                   // number of HID class descriptors to follow
    DESCRIPTOR_TYPE_REPORT,                                              // descriptor type (0x22)
    {LITTLE_SHORT_WORD_BYTES(sizeof(ucRawReport))},                      // total length of report descriptor
    },

    {                                                                    // interrupt in endpoint descriptor for the interface
    DESCRIPTOR_TYPE_ENDPOINT_LENGTH,                                     // descriptor size in bytes (0x07)
    DESCRIPTOR_TYPE_ENDPOINT,                                            // endpoint descriptor (0x05)
    (IN_ENDPOINT | USB_HID_RAW_IN_ENDPOINT_NUMBER),                      // direction and address of endpoint
    ENDPOINT_INTERRUPT,                                                  // endpoint attributes (0x03)
    {LITTLE_SHORT_WORD_BYTES(HID_RAW_TX_SIZE)},                          // endpoint FIFO size (little-endian - byte count)
    HID_RAW_TX_RATE                                                      // polling interval in ms
    },

    {                                                                    // interrupt in endpoint descriptor for the interface
    DESCRIPTOR_TYPE_ENDPOINT_LENGTH,                                     // descriptor size in bytes (0x07)
    DESCRIPTOR_TYPE_ENDPOINT,                                            // endpoint descriptor (0x05)
    (OUT_ENDPOINT | USB_HID_RAW_OUT_ENDPOINT_NUMBER),                    // direction and address of endpoint
    ENDPOINT_INTERRUPT,                                                  // endpoint attributes (0x03)
    {LITTLE_SHORT_WORD_BYTES(HID_RAW_RX_SIZE)},                          // endpoint FIFO size (little-endian - byte count)
    HID_RAW_RX_RATE                                                      // polling interval in ms
    },
    #endif
    #if defined USE_USB_AUDIO
    {                                                                    // interface association descriptor
    DESCRIPTOR_TYPE_INTERFACE_ASSOCIATION_LENGTH,                        // 0x08
    DESCRIPTOR_TYPE_INTERFACE_ASSOCIATION,                               // 0x0b
    2,                                                                   // first interface number
    3,                                                                   // interface count
    DEVICE_CLASS_AUDIO,                                                  // 0x01, 0x00, 0x00
    0                                                                    // string reference
    },

    // Audio class interface 0, alternative setting 0, audio control
    {                                                                    // interface descriptor
    DESCRIPTOR_TYPE_INTERFACE_LENGTH,                                    // length (0x09)
    DESCRIPTOR_TYPE_INTERFACE,                                           // 0x04
    AUDIO_CONTROL_INTERFACE,                                             // interface number
    0,                                                                   // alternative setting 0
    0,                                                                   // number of endpoints in addition to EP0
    AUDIO_CLASS,                                                         // interface class (0x01)
    AUDIO_SUBCLASS_CONTROL,                                              // interface sub-class (0x01)
    0,                                                                   // interface protocol (0x00)
    #if defined USB_STRING_OPTION
    INTERFACE_STRING_INDEX,                                              // string index for interface
    #else
    0,                                                                   // zero when strings are not supported
    #endif
    },                                                                   // end of interface descriptor

    // Audio control interface header descriptor
    //
    {
    (sizeof(USB_AUDIO_CONTROL_INTERFACE_HEADER_DESCRIPTOR_2)),           // bLength (0x0a)
    CS_INTERFACE,                                                        // bDescriptorType (0x24)
    AUDIO_CONTROL_HEADER,                                                // bDescriptorSubtype (0x01)
    {LITTLE_SHORT_WORD_BYTES(0x0100)},                                   // bcdADC (specification release 1.0)
    {LITTLE_SHORT_WORD_BYTES(61)},                                       // wTotalLength (combined length of this header and all unit and terminal descriptors)
    2,                                                                   // bInCollection (number of interfaces in the audio interface collection)
    AUDIO_SINK_INTERFACE,                                                // baInterfaceNr1 - first interface number
    AUDIO_SOURCE_INTERFACE,                                              // baInterfaceNr2 - last interface number
    },

    // Audio input terminal descriptor (loud-speaker)
    //
    {
    (sizeof(USB_AUDIO_INPUT_TERMINAL_DESCRIPTOR)),                       // bLength (0x0c)
    CS_INTERFACE,                                                        // bDescriptorType (0x24)
    AUDIO_CONTROL_INPUT_TERMINAL,                                        // bDescriptorSubtype (0x02)
    1,                                                                   // bTerminalID
    {LITTLE_SHORT_WORD_BYTES(USB_AUDIO_TERMINAL_STREAMING)},             // wTerminalType (USB Streaming - 0x0101)
    0,                                                                   // bAssocTerminal - associated output terminal
    1,                                                                   // bNrChannels
    {LITTLE_SHORT_WORD_BYTES(0x0000)},                                   // wChannelConfig - spacial location of logical channels
    0,                                                                   // iChannelNames - string index
    0,                                                                   // iTerminal - string index
    },

    // Audio feature unit descriptor (loud-speaker)
    //
    {
    (sizeof(USB_AUDIO_FEATURE_UNIT_DESCRIPTOR)),                         // bLength (0x09)
    CS_INTERFACE,                                                        // bDescriptorType (0x24)
    AUDIO_CONTROL_FEATURE_UNIT,                                          // bDescriptorSubtype (0x06)
    2,                                                                   // bUnitID
    1,                                                                   // bSourceID
    1,                                                                   // bControlSize
    {LITTLE_SHORT_WORD_BYTES(USB_AUDIO_UNIT_CONTROL_MUTE | USB_AUDIO_UNIT_CONTROL_VOLUME)}, // bmaControls (0x0003)
    0,                                                                   // iFeature - string index
    },

    // Audio output terminal descriptor (loud-speaker)
    //
    {
    (sizeof(USB_AUDIO_OUTPUT_TERMINAL_DESCRIPTOR)),                      // bLength (0x09)
    CS_INTERFACE,                                                        // bDescriptorType (0x24)
    AUDIO_CONTROL_OUTPUT_TERMINAL,                                       // bDescriptorSubtype (0x03)
    3,                                                                   // bTerminalID
    {LITTLE_SHORT_WORD_BYTES(USB_AUDIO_OUTPUT_TERMINAL_SPEAKER)},        // wTerminalType (0x0301)
    0,                                                                   // bAssocTerminal - input terminal to which this output terminal is associated
    2,                                                                   // bSourceID - ID of the unit or terminal to which this terminal is connected
    0,                                                                   // iTerminal - string index
    },

    // Audio input terminal descriptor (microphone)
    //
    {
    (sizeof(USB_AUDIO_INPUT_TERMINAL_DESCRIPTOR)),                       // bLength (0x0c)
    CS_INTERFACE,                                                        // bDescriptorType (0x24)
    AUDIO_CONTROL_INPUT_TERMINAL,                                        // bDescriptorSubtype (0x02)
    4,                                                                   // bTerminalID
    {LITTLE_SHORT_WORD_BYTES(USB_AUDIO_INPUT_TERMINAL_MICROPHONE)},      // wTerminalType
    0,                                                                   // bAssocTerminal
    1,                                                                   // bNrChannels
    {LITTLE_SHORT_WORD_BYTES(0x0000)},                                   // wChannelConfig - spacial location of logical channels
    0,                                                                   // iChannelNames - string index
    0,                                                                   // iTerminal - string index
    },

    // Audio output terminal descriptor (microphone)
    //
    {
    (sizeof(USB_AUDIO_OUTPUT_TERMINAL_DESCRIPTOR)),                      // bLength (0x09)
    CS_INTERFACE,                                                        // bDescriptorType (0x24)
    AUDIO_CONTROL_OUTPUT_TERMINAL,                                       // bDescriptorSubtype (0x03)
    5,                                                                   // bTerminalID
    {LITTLE_SHORT_WORD_BYTES(USB_AUDIO_TERMINAL_STREAMING)},             // wTerminalType (USB Streaming - 0x0101)
    0,                                                                   // bAssocTerminal
    4,                                                                   // bSourceID
    0                                                                    // iTerminal - string index
    },

    // Interface 1 - alternative setting 0, audio streaming - zero bandwidth
    //
    {                                                                    // interface descriptor
    DESCRIPTOR_TYPE_INTERFACE_LENGTH,                                    // length (0x09)
    DESCRIPTOR_TYPE_INTERFACE,                                           // 0x04
    AUDIO_SINK_INTERFACE,                                                // interface number
    0,                                                                   // alternative setting 0
    0,                                                                   // number of endpoints in addition to EP0
    AUDIO_CLASS,                                                         // interface class (0x01)
    AUDIO_SUBCLASS_STREAMING,                                            // interface sub-class (0x02)
    0,                                                                   // interface protocol (0x00)
    #if defined USB_STRING_OPTION
    INTERFACE_STRING_INDEX,                                              // string index for interface
    #else
    0,                                                                   // zero when strings are not supported
    #endif
    },                                                                   // end of interface descriptor

    // Interface 1 - alternative setting 1, audio streaming - operational
    //
    {                                                                    // interface descriptor
    DESCRIPTOR_TYPE_INTERFACE_LENGTH,                                    // length (0x09)
    DESCRIPTOR_TYPE_INTERFACE,                                           // 0x04
    AUDIO_SINK_INTERFACE,                                                // interface number
    1,                                                                   // alternative setting 0
    1,                                                                   // number of endpoints in addition to EP0
    AUDIO_CLASS,                                                         // interface class (0x01)
    AUDIO_SUBCLASS_STREAMING,                                            // interface sub-class (0x02)
    0,                                                                   // interface protocol - unused (0x00)
    #if defined USB_STRING_OPTION
    INTERFACE_STRING_INDEX,                                              // string index for interface
    #else
    0,                                                                   // zero when strings are not supported
    #endif
    },                                                                   // end of interface descriptor

    // Class-specific audio-streaming general interface descriptor
    //
    {
    (sizeof(USB_AUDIO_STREAMING_GENERAL_INTERFACE_DESCRIPTOR)),          // bLength (0x07)
    CS_INTERFACE,                                                        // bDescriptorType (0x24)
    AUDIO_STREAMING_GENERAL,                                             // bDescriptorSubtype (0x01)
    1,                                                                   // bTerminalLink - terminal to which the endpoint of this interface is connected
    0,                                                                   // bDelay - number of frames delay introduced by the data path
    {LITTLE_SHORT_WORD_BYTES(USB_AUDIO_FORMAT_TYPE_I_PCM8)},             // wFormatTag (0x0002)
    },

    // Type I format type descriptor
    //
    {
    (sizeof(USB_AUDIO_TYPE_I_FORMAT_TYPE_DESCRIPTOR)),                   // bLength (0x0b)
    CS_INTERFACE,                                                        // bDescriptorType (0x24)
    AUDIO_STREAMING_FORMAT_TYPE,                                         // bDescriptorSubtype (0x02)
    USB_AUDIO_FORMAT_TYPE_I,                                             // bFormatType (0x01)
    1,                                                                   // bNrChannels - 1 channel
    2,                                                                   // bSubFrameSize - 2 bytes per audio subframe
    16,                                                                  // bBitResolution - 16 Bits
    1,                                                                   // bSamFreqType - single frequency supported
    {LITTLE_SHORT_24BIT_BYTES(48000)}                                    // tSamFreq (48 kHz)
    },

    // Interrupt OUT audio class endpoint descriptor
    //
    {                                                                    // interrupt in endpoint descriptor for the interface
    DESCRIPTOR_TYPE_AUDIO_CLASS_ENDPOINT_LENGTH,                         // descriptor size in bytes (0x09)
    DESCRIPTOR_TYPE_ENDPOINT,                                            // endpoint descriptor (0x05)
    (OUT_ENDPOINT | USB_AUDIO_OUT_ENDPOINT_NUMBER),                      // direction and address of endpoint
    (ENDPOINT_ISOCHRONOUS | NO_SYNCHRONISATION | DATA_ENDPOINT_USE),     // endpoint attributes (0x01)
    {LITTLE_SHORT_WORD_BYTES(96)},                                       // endpoint FIFO size (little-endian - byte count)
    1,                                                                   // polling interval in ms
    0,                                                                   // refresh
    0                                                                    // synch endpoint address
    },

    // Isochronous audio data endpoint descriptor
    //
    {
    (sizeof(USB_ISO_AUDIO_DATA_ENDPOINT_DESCRIPTOR)),                    // bLength (0x07)
    CS_ENDPOINT,                                                         // bDescriptorType (0x25)
    AUDIO_STREAMING_GENERAL,                                             // bDescriptor (0x01)
    AUDIO_ENDPOINT_CONTROL_MAX_PACKET_SIZE_ONLY,                         // bmAttributes - no sampling frequency or pitch cntrol
    AUDIO_ENDPOINT_LOCK_DELAY_UNITS_UNDEFINED,                           // bLockDelayUnits
    {LITTLE_SHORT_WORD_BYTES(0)},                                        // wLockDelay
    },

    // Interface 2, alternative setting 0, audio streaming - zero bandwidth
    //
    {                                                                    // interface descriptor
    DESCRIPTOR_TYPE_INTERFACE_LENGTH,                                    // length (0x09)
    DESCRIPTOR_TYPE_INTERFACE,                                           // 0x04
    AUDIO_SOURCE_INTERFACE,                                              // interface number
    0,                                                                   // alternative setting 0
    0,                                                                   // number of endpoints in addition to EP0
    AUDIO_CLASS,                                                         // interface class (0x01)
    AUDIO_SUBCLASS_STREAMING,                                            // interface sub-class (0x02)
    0,                                                                   // interface protocol (0x00)
    #if defined USB_STRING_OPTION
    INTERFACE_STRING_INDEX,                                              // string index for interface
    #else
    0,                                                                   // zero when strings are not supported
    #endif
    },                                                                   // end of interface descriptor

    // Interface 2, alternative setting 1, audio streaming - operational
    //
    {                                                                    // interface descriptor
    DESCRIPTOR_TYPE_INTERFACE_LENGTH,                                    // length (0x09)
    DESCRIPTOR_TYPE_INTERFACE,                                           // 0x04
    AUDIO_SOURCE_INTERFACE,                                              // interface number
    1,                                                                   // alternative setting 0
    1,                                                                   // number of endpoints in addition to EP0
    AUDIO_CLASS,                                                         // interface class (0x01)
    AUDIO_SUBCLASS_STREAMING,                                            // interface sub-class (0x02)
    0,                                                                   // interface protocol (0x00)
    #if defined USB_STRING_OPTION
    INTERFACE_STRING_INDEX,                                              // string index for interface
    #else
    0,                                                                   // zero when strings are not supported
    #endif
    },                                                                   // end of interface descriptor

    // Class-specific audio-streaming general interface descriptor
    //
    {
    0x07,                                                                // bLength (0x07)
    CS_INTERFACE,                                                        // bDescriptorType (0x24)
    AUDIO_STREAMING_GENERAL,                                             // bDescriptorSubtype (0x01)
    5,                                                                   // bTerminalLink (output terminal microphone)
    1,                                                                   // bDelay - number of frames delay introduced by the data path
    {LITTLE_SHORT_WORD_BYTES(USB_AUDIO_FORMAT_TYPE_I_PCM)},              // wFormatTag (0x0001)
    },

    // Audio type I format
    //
    {
    (sizeof(USB_AUDIO_TYPE_I_FORMAT_TYPE_DESCRIPTOR)),                   // bLength (0x0b)
    CS_INTERFACE,                                                        // bDescriptorType (0x24)
    AUDIO_STREAMING_FORMAT_TYPE,                                         // bDescriptorSubtype (0x02)
    USB_AUDIO_FORMAT_TYPE_I,                                             // bFormatType (0x01)
    1,                                                                   // bNrChannels - 1 channel
    2,                                                                   // bSubFrameSize - 2 bytes per audio subframe
    16,                                                                  // bBitResolution - 16 Bits
    1,                                                                   // bSamFreqType - single frequency supported
    {LITTLE_SHORT_24BIT_BYTES(48000)}                                    // tSamFreq (48 kHz)
    },

    // Isochronous IN audio class endpoint descriptor
    //
    {                                                                    // interrupt IN endpoint descriptor for the interface
    DESCRIPTOR_TYPE_AUDIO_CLASS_ENDPOINT_LENGTH,                         // descriptor size in bytes (0x09)
    DESCRIPTOR_TYPE_ENDPOINT,                                            // endpoint descriptor (0x05)
    #if defined USB_SIMPLEX_ENDPOINTS
    (IN_ENDPOINT | 0x01),                                                // direction and address of endpoint
    #else
    (IN_ENDPOINT | USB_AUDIO_IN_ENDPOINT_NUMBER),                        // direction and address of endpoint
    #endif
    (ENDPOINT_ISOCHRONOUS | NO_SYNCHRONISATION | DATA_ENDPOINT_USE),     // endpoint attributes (0x01)
    {LITTLE_SHORT_WORD_BYTES(96)},                                       // endpoint FIFO size (little-endian - byte count)
    1,                                                                   // polling interval in ms
    0,                                                                   // bRefresh
    0                                                                    // bSynchAddress
    },

    // Isochronous audio data endpoint descriptor
    //
    {
    (sizeof(USB_ISO_AUDIO_DATA_ENDPOINT_DESCRIPTOR)),                    // bLength (0x07)
    CS_ENDPOINT,                                                         // bDescriptorType (0x25)
    AUDIO_STREAMING_GENERAL,                                             // bDescriptor - general (0x01)
    AUDIO_ENDPOINT_CONTROL_MAX_PACKET_SIZE_ONLY,                         // bmAttributes - no sampling frequency or pitch cntrol
    AUDIO_ENDPOINT_LOCK_DELAY_UNITS_UNDEFINED,                           // bLockDelayUnits
    {LITTLE_SHORT_WORD_BYTES(0)},                                        // wLockDelay
    },
    #endif
};
#endif
