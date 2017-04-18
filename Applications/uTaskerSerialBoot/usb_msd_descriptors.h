/**********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher

    ---------------------------------------------------------------------
    File:      usb_msd_descriptors.h
    Project:   uTasker project
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2016
    *********************************************************************

*/


#if defined INCLUDE_USB_DEFINES
    #define USB_PRODUCT_RELEASE_NUMBER      0x0100                       // V1.0 (binary coded decimal)

    #if defined USB_SIMPLEX_ENDPOINTS
        #define NUMBER_OF__SM_ENDPOINTS     (1)                          // uses 1 endpoint (1 bulk IN/OUT) in addition to the default control endpoint 0
        #define USB_MSB_IN_ENDPOINT_NUMBER  0x01
    #else
        #define NUMBER_OF_MSD_ENDPOINTS     (2)                          // uses 2 endpoints (1 IN and 1 OUT) in addition to the default control endpoint 0
        #define USB_MSB_IN_ENDPOINT_NUMBER  0x02
    #endif

    #define USB_MSD_INTERFACE_COUNT           1
    #define USB_MOUSE_IN_ENDPOINT_NUMBER (USB_MSB_IN_ENDPOINT_NUMBER + 1)
    #if defined USE_USB_HID_MOUSE                                        // MSD with HID mouse
        #define USB_MOUSE_INTERFACE_NUMBER    1
        #define USB_HID_MOUSE_INTERFACE_COUNT 1
        #define USB_HID_MOUSE_ENDPOINT_COUNT  1
    #else
        #define USB_HID_MOUSE_INTERFACE_COUNT 0
        #define USB_HID_MOUSE_ENDPOINT_COUNT  0
    #endif
    #define USB_KEYBOARD_IN_ENDPOINT_NUMBER (USB_MOUSE_IN_ENDPOINT_NUMBER + USB_HID_MOUSE_ENDPOINT_COUNT)
    #if defined USE_USB_HID_KEYBOARD                                     // CDC with HID keyboard
        #define USB_KEYBOARD_INTERFACE_NUMBER 2
        #define USB_HID_KB_INTERFACE_COUNT    1
        #define USB_HID_KB_ENDPOINT_COUNT     1
    #else
        #define USB_HID_KB_INTERFACE_COUNT    0
        #define USB_HID_KB_ENDPOINT_COUNT     0
    #endif

    #define USB_INTERFACE_COUNT              (USB_MSD_INTERFACE_COUNT + USB_HID_MOUSE_INTERFACE_COUNT + USB_HID_KB_INTERFACE_COUNT) // configuration number (1 interfaces for each available MSD, mouse and keyboard)
    #define NUMBER_OF_ENDPOINTS              (NUMBER_OF_MSD_ENDPOINTS + USB_HID_MOUSE_ENDPOINT_COUNT + USB_HID_KB_ENDPOINT_COUNT)

    #if defined _M5223X || defined _KINETIS
        #define USB_VENDOR_ID               0x0425                       // MOTOROLA vendor ID {19}
        #define USB_PRODUCT_ID              (0xff4f + USB_HID_MOUSE_INTERFACE_COUNT + (USB_HID_KB_INTERFACE_COUNT * 2)) // unofficial test value
    #elif defined _LM3SXXXX
        #define USB_VENDOR_ID               0x1cbe                       // Luminary Micro, Inc. vendor ID
        #define USB_PRODUCT_ID              0x1234                       // non-official test MSD PID
    #elif defined _HW_SAM7X || defined _HW_AVR32
        #define USB_VENDOR_ID               0x03eb                       // ATMEL Corp. vendor ID
        #define USB_PRODUCT_ID              0x1234                       // non-official test MSD PID
    #else
        #define USB_VENDOR_ID               0x4321                       // non-official test VID
        #define USB_PRODUCT_ID              0x1234                       // non-official test MSD PID
    #endif

    #if defined USB_STRING_OPTION                                        // if our project supports strings
        #define MANUFACTURER_STRING_INDEX       1                        // index must match with order in the string list
        #define PRODUCT_STRING_INDEX            2                        // to remove a particular string from the list set to zero
        #define SERIAL_NUMBER_STRING_INDEX      3
        #define CONFIGURATION_STRING_INDEX      4
        #define INTERFACE_STRING_INDEX          5

        #define UNICODE_LANGUAGE_INDEX          UNICODE_ENGLISH_LANGUAGE // English language used by strings
        #define LAST_STRING_INDEX               INTERFACE_STRING_INDEX   // last string entry - used for protection against invalid string index requests

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
        static const unsigned char interface_str[]         = {8,  DESCRIPTOR_TYPE_STRING, 'I',0, 'n',0, 't',0};

        static const unsigned char *ucStringTable[]        = {usb_language_string, manufacturer_str, product_str, serial_number_str, config_str, interface_str};
    #endif
#endif

#if defined INCLUDE_USB_CLASS_CONSTS                                     // consts required by USB-MSD
static const unsigned char  cCBWSignature[4] = {'U', 'S', 'B', 'C'};

static const CBW_INQUIRY_DATA inquiryData = {
    DEVICE_TYPE_FLOPPY,
    RMB_REMOVABLE,
    0,
    RESPONSE_FORMAT_UFI,
    31,                                                                  // additional length should be 31
    {0},                                                                 // reserved field
    {'u', 'T', 'a', 's', 'k', 'e', 'r', ' '},                            // vendor information (8 bytes)
    {'U', 'S', 'B', 't', 'o', 'S', 'D', '-', 'C', 'a', 'r', 'd', ' ', ' ', ' ', ' '}, // product identification (16 bytes)
    {'1', '.', '0', '0' }                                                // product revision level
};

static const CBW_RETURN_SENSE_DATA sense_data_OK = {VALID_SENSE_DATA, 0, SENSE_NO_SENSE, {0,0,0,0}, SENSE_LENGTH_10, {0,0,0,0}, 0, 0, {0,0,0,0}};

static const MODE_PARAMETER_6 SelectData = {                             // {15} standard response without descriptors and no write protection
    3,                                                                   // content length
    DEVICE_TYPE_FLOPPY,                                                  // medium type
    0,                                                                   // no write protection
    0                                                                    // no descriptors
};

static const CBW_CAPACITY_LIST formatCapacityNoMedia = {
    {0},
    8,                                                                   // capacity list length with just one entry
    {
        {0x00, 0x01, 0xf8, 0x00}, DESC_CODE_NO_CARTRIDGE_IN_DRIVE, {0x00, 0x02, 0x00}, // 512 bytes block length
    }
};
#endif

#if defined INCLUDE_USB_CONFIG_DESCRIPTOR
typedef struct _PACK stUSB_CONFIGURATION_DESCRIPTOR_COLLECTION
{
    USB_CONFIGURATION_DESCRIPTOR               config_desc_msd;          // compulsory configuration descriptor
    USB_INTERFACE_DESCRIPTOR                   interface_desc_0;         // first interface descriptor
    USB_ENDPOINT_DESCRIPTOR                    endpoint_1;               // endpoints of second interface
    USB_ENDPOINT_DESCRIPTOR                    endpoint_2;
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
} USB_CONFIGURATION_DESCRIPTOR_COLLECTION;
#endif

#if defined INCLUDE_USB_DEVICE_DESCRIPTOR
static const USB_DEVICE_DESCRIPTOR device_descriptor = {                 // constant device descriptor
    STANDARD_DEVICE_DESCRIPTOR_LENGTH,                                   // standard device descriptor length (0x12)
    DESCRIPTOR_TYPE_DEVICE,                                              // 0x01
    {LITTLE_SHORT_WORD_BYTES(USB_SPEC_VERSION)},                         // USB1.1 or USB2
    DEVICE_CLASS_AT_INTERFACE,                                           // device class, sub-class and protocol (class defined at interface level)
    ENDPOINT_0_SIZE,                                                     // size of endpoint reception buffer
    {LITTLE_SHORT_WORD_BYTES(USB_VENDOR_ID)},                            // our vendor ID
    {LITTLE_SHORT_WORD_BYTES(USB_PRODUCT_ID)},                           // our product ID
    {LITTLE_SHORT_WORD_BYTES(USB_PRODUCT_RELEASE_NUMBER)},               // product release number
    #if defined USB_STRING_OPTION                                        // if we support strings add the data here
    MANUFACTURER_STRING_INDEX, PRODUCT_STRING_INDEX, SERIAL_NUMBER_STRING_INDEX, // fixed string table indexes - note that mass storage class demainds that each device has a unique serial number of at least 12 digits length!
    #else
    0,0,0,                                                               // used when no strings are supported
    #endif
    NUMBER_OF_POSSIBLE_CONFIGURATIONS                                    // number of configurations possible
};

static const USB_CONFIGURATION_DESCRIPTOR_COLLECTION config_descriptor = {
    {                                                                    // config descriptor
    DESCRIPTOR_TYPE_CONFIGURATION_LENGTH,                                // length (0x09)
    DESCRIPTOR_TYPE_CONFIGURATION,                                       // 0x02
    {LITTLE_SHORT_WORD_BYTES(sizeof(USB_CONFIGURATION_DESCRIPTOR_COLLECTION))}, // total length (little-endian)
    USB_INTERFACE_COUNT,                                                 // configuration number - total number of interfaces
    0x01,                                                                // configuration value
    #if defined USB_STRING_OPTION
    CONFIGURATION_STRING_INDEX,                                          // string index to configuration
    #else
    0,                                                                   // zero when strings are not supported
    #endif
    (SELF_POWERED | ATTRIBUTE_DEFAULT),                                  // attributes for configuration,
    0                                                                    // consumption in 2mA steps (eg. 100/2 for 100mA)
    },                                                                   // end of compulsory config descriptor

    {                                                                    // interface descriptor
    DESCRIPTOR_TYPE_INTERFACE_LENGTH,                                    // length (0x09)
    DESCRIPTOR_TYPE_INTERFACE,                                           // 0x04
    0,                                                                   // interface number 0
    0,                                                                   // alternative setting 0
    2,                                                                   // number of endpoints in addition to EP0
    INTERFACE_CLASS_MASS_STORAGE,                                        // interface class (0x08)
    GENERIC_SCSI_MEDIA,                                                  // interface sub-class (0x06)
    BULK_ONLY_TRANSPORT,                                                 // interface protocol (0x50)
    #if defined USB_STRING_OPTION
    INTERFACE_STRING_INDEX,                                              // string index for interface
    #else
    0,                                                                   // zero when strings are not supported
    #endif
    },                                                                   // end of interface descriptor

    {                                                                    // bulk out endpoint descriptor for the second interface
    DESCRIPTOR_TYPE_ENDPOINT_LENGTH,                                     // descriptor size in bytes (0x07)
    DESCRIPTOR_TYPE_ENDPOINT,                                            // endpoint descriptor (0x05)
    #if defined _LPC23XX || defined _LPC17XX
    (OUT_ENDPOINT | 0x02),                                               // direction and address of endpoint
    #else
    (OUT_ENDPOINT | 0x01),                                               // direction and address of endpoint
    #endif
    ENDPOINT_BULK,                                                       // endpoint attributes
    {LITTLE_SHORT_WORD_BYTES(64)},                                       // endpoint FIFO size (little-endian - 64 bytes)
    0                                                                    // polling interval in ms - ignored for bulk
    },

    {                                                                    // bulk in endpoint descriptor for the second interface
    DESCRIPTOR_TYPE_ENDPOINT_LENGTH,                                     // descriptor size in bytes (0x07)
    DESCRIPTOR_TYPE_ENDPOINT,                                            // endpoint descriptor (0x05)
    #if defined _LPC23XX || defined _LPC17XX
    (IN_ENDPOINT | 0x05),                                                // direction and address of endpoint
    #else
    (IN_ENDPOINT | USB_MSB_IN_ENDPOINT_NUMBER),                          // direction and address of endpoint
    #endif
    ENDPOINT_BULK,                                                       // endpoint attributes
    {LITTLE_SHORT_WORD_BYTES(64)},                                       // endpoint FIFO size (little-endian - 64 bytes)
    0                                                                    // polling interval in ms - ignored for bulk
    },
    #if defined USE_USB_HID_MOUSE
    {                                                                    // interface descriptor
    DESCRIPTOR_TYPE_INTERFACE_LENGTH,                                    // length (0x09)
    DESCRIPTOR_TYPE_INTERFACE,                                           // 0x04
    (USB_MSD_INTERFACE_COUNT),                                           // interface number
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
    (USB_MSD_INTERFACE_COUNT + USB_HID_MOUSE_INTERFACE_COUNT),           // number of interfaces
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
};
#endif