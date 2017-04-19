/**********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher

    ---------------------------------------------------------------------
    File:      usb_keyboard_descriptors.h
    Project:   uTasker project
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2017
    *********************************************************************

*/

#if defined INCLUDE_USB_DEFINES
    #define USB_PRODUCT_RELEASE_NUMBER          0x0100                   // V1.0 (binary coded decimal)

    #define NUMBER_OF_ENDPOINTS                 1                        // uses 1 interrupt IN endpoint in addition to the default control endpoint 0
    #define USB_VENDOR_ID                       0x15a2                   // Freescale vendor ID
    #define USB_PRODUCT_ID                      0xff00                   // unofficial test value for PID

    #define USB_KEYBOARD_INTERFACE_NUMBER       0

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

#if defined INCLUDE_USB_CONFIG_DESCRIPTOR
typedef struct _PACK stUSB_CONFIGURATION_DESCRIPTOR_COLLECTION
{
    USB_CONFIGURATION_DESCRIPTOR               config_desc_hid_keyboard; // compulsory configuration descriptor
    USB_INTERFACE_DESCRIPTOR                   interface_desc_0;         // first interface descriptor
    USB_HID_DESCRIPTOR                         hid_desc_0;               // HID descriptor
    USB_ENDPOINT_DESCRIPTOR                    endpoint_1;               // single endpoint
} USB_CONFIGURATION_DESCRIPTOR_COLLECTION;
#endif


#if defined INCLUDE_USB_CLASS_CONSTS

/*
    /--------------------------\
    |     Device Descriptor    |
    /--------------------------\
                  |
    /--------------------------\
    | Configuration Descriptor |
    /--------------------------\
                  |
    /--------------------------\
    |    Interface Descriptor  |
    /--------------------------\
                  |   |
                  |   ----------------------------
                  |                              |
    /--------------------------\     /--------------------------\
    |    Endpoint Descriptor   |     |       HID Descriptor     |
    /--------------------------\     /--------------------------\
                                                 |
                                  -------------------------------
                                  |                             |
                   /--------------------------\     /--------------------------\
                   |      Report Descriptor   |     |   Physical Descriptor    |
                   /--------------------------\     /--------------------------\
*/

    static const unsigned char ucKeyboardReport[] = {
    0x05,                                                                // usage page
    0x01,                                                                // generic desktop controls

    0x09,                                                                // usage
    0x06,                                                                // keyboard

    0xa1,                                                                // collection 
    0x01,                                                                // application

    // To classify
    //
    0x05, 0x07, 0x19, 0xe0, 0x29, 0xe7, 0x15, 0x00, 0x25, 0x01, 0x75, 0x01, 0x95, 0x08,
    0x81, 0x02, 0x95, 0x01, 0x75, 0x08, 0x81, 0x01,
    0x95, 0x03,                                                          // report count
    0x75, 0x01,                                                          // report size
    0x05, 0x08,                                                          // usage page - page number for LEDs
    0x19, 0x01,                                                          // usage minimum
    0x29, 0x03,                                                          // usage maximum
    0x91, 0x02, 0x95, 0x05, 0x75, 0x01, 0x91, 0x01, 0x95, 0x06,
    0x75, 0x08, 0x15, 0x00, 0x26, 0xff, 0x00, 0x05, 0x07, 0x19, 0x00, 0x2a, 0xff, 0x00,
    0x81, 0x00, 

    0xc0                                                                 // end collection
};
#endif

#if defined INCLUDE_USB_DEVICE_DESCRIPTOR
static const USB_DEVICE_DESCRIPTOR device_descriptor = {                 // constant device descriptor
    STANDARD_DEVICE_DESCRIPTOR_LENGTH,                                   // standard device descriptor length (0x12)
    DESCRIPTOR_TYPE_DEVICE,                                              // 0x01
    {LITTLE_SHORT_WORD_BYTES(USB_SPEC_VERSION)},                         // USB1.1 or USB2
    DEVICE_CLASS_AT_INTERFACE,                                           // device class, sub-class and protocol (class defined at interface level) - HID should never specifiy the device class at this level
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
    0x01,                                                                // configuration number - HID keyboard has only one configuration
    0x01,                                                                // configuration value
    #if defined USB_STRING_OPTION
    CONFIGURATION_STRING_INDEX,                                          // string index to configuration
    #else
    0,                                                                   // zero when strings are not supported
    #endif
    (BUS_POWERED | ATTRIBUTE_DEFAULT),                                   // attributes for configuration,
    (100/2)                                                              // consumption in 2mA steps (eg. 100/2 for 100mA)
    },                                                                   // end of compulsory config descriptor

    {                                                                    // interface descriptor
    DESCRIPTOR_TYPE_INTERFACE_LENGTH,                                    // length (0x09)
    DESCRIPTOR_TYPE_INTERFACE,                                           // 0x04
    USB_KEYBOARD_INTERFACE_NUMBER,                                       // interface number 0
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
    #if defined _LPC23XX || defined _LPC17XX
    (IN_ENDPOINT | 0x05),                                                // direction and address of endpoint
    #else
    (IN_ENDPOINT | 0x01),                                                // direction and address of endpoint
    #endif
    ENDPOINT_INTERRUPT,                                                  // endpoint attributes
    {LITTLE_SHORT_WORD_BYTES(8)},                                        // endpoint FIFO size (little-endian - 8 bytes)
    #if defined USB_KEYBOARD_DELAY
    1                                                                    // polling interval in ms
    #else
    10                                                                   // polling interval in ms
    #endif
    }
};
#endif
