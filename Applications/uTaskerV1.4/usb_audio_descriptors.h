/**********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher

    ---------------------------------------------------------------------
    File:      usb_audio_descriptors.h
    Project:   uTasker project
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2016
    *********************************************************************

*/

#if defined INCLUDE_USB_DEFINES
    #define USB_PRODUCT_RELEASE_NUMBER          0x0100                   // V1.0 (binary coded decimal)

    #define USB_VENDOR_ID                       0x251c                   // Freescale reference
    #if defined USB_SIMPLEX_ENDPOINTS
        #define NUMBER_OF_ENDPOINTS             1                        // uses 1 shared interrupt IN and interrupt OUT endpoint in addition to the default control endpoint 0
    #else
        #define NUMBER_OF_ENDPOINTS             2                        // uses 1 interrupt IN and 1 interrupt OUT endpoint in addition to the default control endpoint 0
    #endif
    #define NUMBER_OF_INTERFACES                3
    #define USB_PRODUCT_ID                      0x0140                   // Freescale reference

    #define USB_AUDIO_OUT_ENDPOINT_NUMBER       1
    #define USB_AUDIO_IN_ENDPOINT_NUMBER        2

    #define AUDIO_CONTROL_INTERFACE             0
    #define AUDIO_SINK_INTERFACE                1
    #define AUDIO_SOURCE_INTERFACE              2

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
    USB_CONFIGURATION_DESCRIPTOR               config_desc_audio;        // compulsory configuration descriptor
    USB_INTERFACE_DESCRIPTOR                   interface_desc_audio_0;   // first interface descriptor
    USB_AUDIO_CONTROL_INTERFACE_HEADER_DESCRIPTOR_2 audio_control_header1; // audio control interface header descriptor
    USB_AUDIO_INPUT_TERMINAL_DESCRIPTOR        in_terminal_0;            // input terminal descriptor - loud-speaker
    USB_AUDIO_FEATURE_UNIT_DESCRIPTOR          feature_0;                // feature unit decriptor
    USB_AUDIO_OUTPUT_TERMINAL_DESCRIPTOR       out_terminal_0;           // output terminal descriptor - loud-speaker
    USB_AUDIO_INPUT_TERMINAL_DESCRIPTOR        in_terminal_1;            // input terminal descriptor - microphone
    USB_AUDIO_OUTPUT_TERMINAL_DESCRIPTOR       out_terminal_1;           // output terminal descriptor - microphone
    USB_INTERFACE_DESCRIPTOR                   interface_desc_audio_1;   // second interface descriptor
    USB_INTERFACE_DESCRIPTOR                   interface_desc_2;         // third interface descriptor
    USB_AUDIO_STREAMING_GENERAL_INTERFACE_DESCRIPTOR streaming_desc1;    // streaming descriptor
    USB_AUDIO_TYPE_I_FORMAT_TYPE_DESCRIPTOR    format_1;                 // format type I descriptor
    USB_AUDIO_CONTROL_ENDPOINT_DESCRIPTOR      endpoint_audio_0;         // first audio endpoint
    USB_ISO_AUDIO_DATA_ENDPOINT_DESCRIPTOR     iso_audio_desc1;          // isochronous audio data endpoint descriptor
    USB_INTERFACE_DESCRIPTOR                   interface_desc_3;         // fourth interface descriptor
    USB_INTERFACE_DESCRIPTOR                   interface_desc_4;         // fifth interface descriptor
    USB_AUDIO_STREAMING_GENERAL_INTERFACE_DESCRIPTOR streaming_desc2;    // streaming descriptor
    USB_AUDIO_TYPE_I_FORMAT_TYPE_DESCRIPTOR    format_2;                 // format type I descriptor
    USB_AUDIO_CONTROL_ENDPOINT_DESCRIPTOR      endpoint_audio_1;         // second audio endpoint
    USB_ISO_AUDIO_DATA_ENDPOINT_DESCRIPTOR     iso_audio_desc2;          // isochronous audio data endpoint descriptor
} USB_CONFIGURATION_DESCRIPTOR_COLLECTION;
#endif

#if defined INCLUDE_USB_CLASS_CONSTS

static const unsigned char ucReport1[] = {0x9e, 0x00};
static const unsigned char ucReport2[] = {0x00, 0x80};
static const unsigned char ucReport3[] = {0xff, 0x7f};
static const unsigned char ucReport4[] = {0x01, 0x00};
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
    0, 0, 0,                                                               // used when no strings are supported
    #endif
    NUMBER_OF_POSSIBLE_CONFIGURATIONS                                    // number of configurations possible
};

static const USB_CONFIGURATION_DESCRIPTOR_COLLECTION config_descriptor = {
    {                                                                    // config descriptor
    DESCRIPTOR_TYPE_CONFIGURATION_LENGTH,                                // length (0x09)
    DESCRIPTOR_TYPE_CONFIGURATION,                                       // 0x02
    {LITTLE_SHORT_WORD_BYTES(sizeof(USB_CONFIGURATION_DESCRIPTOR_COLLECTION))}, // total length (little-endian)
    NUMBER_OF_INTERFACES,                                                // number of interfaces
    1,                                                                   // configuration value
    #if defined USB_STRING_OPTION
    CONFIGURATION_STRING_INDEX,                                          // string index to configuration
    #else
    0,                                                                   // zero when strings are not supported
    #endif
    (BUS_POWERED | ATTRIBUTE_DEFAULT),                                   // attributes for configuration,
    100/2                                                                // consumption in 2mA steps (eg. 100/2 for 100mA)
    },                                                                   // end of compulsory config descriptor

    // Interface 0, alternative setting 0, audio control
    {                                                                    // interface descriptor
    DESCRIPTOR_TYPE_INTERFACE_LENGTH,                                    // length (0x09)
    DESCRIPTOR_TYPE_INTERFACE,                                           // 0x04
    AUDIO_CONTROL_INTERFACE,                                             // interface number 0
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
};
#endif
