/***********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher

    ---------------------------------------------------------------------
    File:      usb.h
    Project:   Single Chip Embedded Internet
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2019
    *********************************************************************
    20.09.2008 Add USB host defines                                      {1}
    23.11.2009 Remove little-endian macros and padding defined to driver.h since the same is required for SD cards and other general use
    13.03.2010 Add mass storage                                          {2}
    21.03.2010 Add ENDPOINT_TERMINATE_TRANSMISSION                       {3}
    21.03.2010 Correct REQUEST_ENDPOINT_STANDARD value                   {4}
    22.02.2011 Add CBW_MODE_SENSE_6                                      {5}
    25.01.2012 Add HID mouse defines                                     {6}
    15.08.2014 Add interface association descriptor                      {7}
    01.12.2015 Add RNDIS definitions                                     {8}
    13.04.2016 Change parameter of fnGetUSB_string_entry() to unsigned char {9}
    15.08.2018 Move USB_SPEC_VERSION default to this header              {10}

*/

#if !defined _USB_
#define _USB_

__PACK_ON                                                                // ensure no padding in structs in this file

#define ENDPOINT_PAIR(x, y)          ()

#define USB_SPEC_VERSION_2_0     0x0200                                  // V2.0
#define USB_SPEC_VERSION_1_1     0x0110                                  // V1.1
#define USB_SPEC_VERSION_1_0     0x0100                                  // V1.0

// Packet Identifiers (PIDs)
//
#define SOF_PID   0x05                                                   // token packets
#define SETUP_PID 0x0d
#define IN_PID    0x09
#define OUT_PID   0x01

#define DATA0_PID 0x03                                                   // data packets
#define DATA1_PID 0x0b

#define ACK_PID   0x02                                                   // handshake packets
#define NAK_PID   0x0a
#define STALL_PID 0x0e
                                                                         // special packets
#define PRE_PID   0x0c                                                   // note PRE is the only packet which doesn't have an EOP at the end!

#if !defined USB_SPEC_VERSION                                            // {10}
    #define USB_SPEC_VERSION               USB_SPEC_VERSION_1_1          // default is to report USB1.1 since it is equivalent to USB2.0 but requires one less descriptor exchange
#endif

#define USB_DEVICE_SPEED_LOW               0
#define USB_DEVICE_SPEED_FULL              1
#define USB_DEVICE_SPEED_HIGH              2

#define UNICODE_ENGLISH_LANGUAGE           0x0409

// Standard USB descriptor type values
//
#define STANDARD_DEVICE_DESCRIPTOR         0x0100
#define STANDARD_CONFIG_DESCRIPTOR         0x0200
#define STANDARD_STRING_DESCRIPTOR         0x0300                        // optional
#define STANDARD_INTERFACE_DESCRIPTOR      0x0400
#define STANDARD_ENDPOINT_DESCRIPTOR       0x0500

#define DEVICE_QUALIFIER_DESCRIPTOR        0x0600                        // USB2.0


#define TYPE_AND_DIRECTION_MASK            0xe0
#define STANDARD_HOST_TO_DEVICE            0x00
#define STANDARD_DEVICE_TO_HOST            0x80

__PACK_ON                                                                // compilers using pragmas to control packing will start struct packing from here
typedef struct _PACK stUSB_DEVICE_DESCRIPTOR
{
    unsigned char          bLength;                                      // descriptor size in bytes
    unsigned char          bDescriptorType;                              // device descriptor
    unsigned char          bcdUSB[2];                                    // version of USB specification
    unsigned char          bDeviceClass;                                 // class code
    unsigned char          bDeviceSubClass;                              // sub-class code
    unsigned char          bDeviceProtocol;                              // protocol code
    unsigned char          bMaxPacketSize0;                              // EP0 FIFO size
    unsigned char          idVendor[2];                                  // vendor ID
    unsigned char          idProduct[2];                                 // product ID
    unsigned char          bcdDevice[2];                                 // release number
    unsigned char          iManufacturer;                                // string index for manufacturer
    unsigned char          iProduct;                                     // string index for product
    unsigned char          iSerialNumber;                                // string index for serial number
    unsigned char          bNumConfigurations;                           // number of possible configurations
} USB_DEVICE_DESCRIPTOR;

// USB Class Codes
//
#define DEVICE_CLASS_AT_INTERFACE                        0,0,0           // class defined at interface level
#define DEVICE_CLASS_AUDIO                               1,0,0           // Audio Device Class
#define DEVICE_CLASS_COMMUNICATION_AND_CONTROL_MANUAL    2,0,0           // Communication and Communication Device Class Control
#define DEVICE_CLASS_COMMUNICATION_AND_CONTROL           2,2,0           // Communication and Communication Device Class Control - recommended by Microsoft for Windows 10 (no installation required)
#define DEVICE_CLASS_COMMUNICATION_ABSTRACT_VENDOR       2,2,0xff        // Communication and Communication Device Class Control - abstract control mode, vendor specific
#define DEVICE_CLASS_HID                                 3,0,0           // Human Interface Device Class
#define DEVICE_CLASS_PHYSICAL                            5,0,0           // Physical Device Class
#define DEVICE_CLASS_STILL_IMAGING                       6,1,1           // Still Imaging Device Class
#define DEVICE_CLASS_PRINTER                             7,0,0           // Printer Device Class
#define DEVICE_CLASS_MASS_STORAGE                        8,0,0           // Mass Storage Device Class
#define DEVICE_CLASS_FULL_SPEED_HUB                      9,0,0           // Hub Device Class
#define DEVICE_CLASS_HIGHSPEED_HUB_SINGLE_TT             9,0,1           // Hub Device Class 
#define DEVICE_CLASS_HIGHSPEED_HUB_MULTIPLE_TTS          9,0,2           // Hub Device Class
#define DEVICE_CLASS_COMMUNICATION                       0xa,0,0         // Communication Device Class
#define DEVICE_CLASS_SMART_CARD                          0xb,0,0         // Smart Card Device Class
#define DEVICE_CLASS_CONTENT_SECURITY                    0xd,0,0         // Content Security Device Class
#define DEVICE_CLASS_VIDEA                               0xe,0,0         // Video Device Class
#define DEVICE_CLASS_PERSONAL_HEALTHCARE                 0xf,0,0         // Personal Healthcare Device Class
#define DEVICE_CLASS_DIAGNOSTICS_DEVICE                  0xdc,0,0        // USB2 Compliance Device
#define DEVICE_CLASS_WIRELESS_CONTROLLER_BLUETOOTH_PROG  0xe0,1,1        // Bluetooth Programming Interface
#define DEVICE_CLASS_WIRELESS_CONTROLLER_UWB_RADIO_CONT  0xe0,1,2        // UWB Radio Control Interface
#define DEVICE_CLASS_WIRELESS_CONTROLLER_REMOTE_NDIS     0xe0,1,3        // Remote NDIS
#define DEVICE_CLASS_WIRELESS_CONTROLLER_HOST_WIRE       0xe0,2,1        // Host Wire Adapter Control/Data Interface
#define DEVICE_CLASS_WIRELESS_CONTROLLER_DEVICE_WIRE     0xe0,2,2        // Device Wire Adapter Control/Data Interface
#define DEVICE_CLASS_WIRELESS_CONTROLLER_DEVICE_WIRE_ISO 0xe0,2,3        // Device Wire Adapter Isochronous Interface
#define DEVICE_CLASS_MISC_ACTIVE_SYNC_DEVICE             0xef,1,1        // Active Sync Device
#define DEVICE_CLASS_MISC_PALM_SYNC_DEVICE               0xef,1,2        // Palm Sync Device
#define DEVICE_CLASS_MISC_INTERFACE_ASSOCIATION_DESC     0xef,2,1        // Interface Association Descriptor
#define DEVICE_CLASS_MISC_WIRE_ADAPTER_MULTIFUNC         0xef,2,2        // Wire Adapter Multifunctional Peripheral Programming Interface
#define DEVICE_CLASS_MISC_CABLE_BASED_ASSOCIATION_FWORK  0xef,3,1        // Cable Based Association Framework
#define DEVICE_CLASS_APP_DEVICE_FIRMWARE_UPGRADE         0xfe,1,1        // Device Firmware Upgrade
#define DEVICE_CLASS_APP_IRDA_BRIDGE                     0xfe,2,0        // IRDA Bridge Device
#define DEVICE_CLASS_APP_USB_TEST_MEASUREMENT            0xfe,3,0        // USB Test and Measurement Device
#define DEVICE_CLASS_APP_USB_TEST_MEASUREMENT_USB488     0xfe,3,1        // USB Test and Measurement Device conforming to USBTMC USB488
#define DEVICE_CLASS_VENDOR_SPECIFIC                     0xff,0,0        // Vendor Specific Device Class

#define VENDOR_SPECIFIC_ENTRY                            0xff

typedef struct _PACK stUSB_DEVICE_QUALIFIER_DESCRIPTOR
{
    unsigned char          bLength;                                      // descriptor size in bytes
    unsigned char          bDescriptorType;                              // device descriptor
    unsigned char          bcdUSB[2];                                    // version of USB specification
    unsigned char          bDeviceClass;                                 // class code
    unsigned char          bDeviceSubClass;                              // sub-class code
    unsigned char          bDeviceProtocol;                              // protocol code
    unsigned char          bMaxPacketSize0;                              // EP0 FIFO size
    unsigned char          bNumConfigurations;                           // number of possible configurations
    unsigned char          bReserved;                                    // reserved for future extensions
} USB_DEVICE_QUALIFIER_DESCRIPTOR;

typedef struct _PACK stUSB_CONFIGURATION_DESCRIPTOR
{
    unsigned char          bLength;                                      // descriptor size in bytes
    unsigned char          bDescriptorType;                              // device descriptor
    unsigned char          wTotalLength[2];                              // 
    unsigned char          bNumInterface;                                // 
    unsigned char          bConfigurationValue;                          // 
    unsigned char          iConfiguration;                               // string index for configuration
    unsigned char          bmAttributes;                                 // 
    unsigned char          bMaxPower;                                    // maximum current consumption in 2mA units
} USB_CONFIGURATION_DESCRIPTOR;

typedef struct _PACK stUSB_INTERFACE_ASSOCIATION_DESCRIPTOR              // {7}
{
    unsigned char          bLength;                                      // descriptor size in bytes
    unsigned char          bDescriptorType;                              // device descriptor
    unsigned char          bFirstInterface;                              // first interface reference
    unsigned char          bInterfaceCount;                              // number of interfaces entrie belonging to this interface
    unsigned char          bFunctionClass;                               
    unsigned char          bFunctionSubClass;
    unsigned char          bFunctionProtocol;
    unsigned char          iFunction;                                    // string reference
} USB_INTERFACE_ASSOCIATION_DESCRIPTOR;

typedef struct _PACK stUSB_INTERFACE_DESCRIPTOR
{
    unsigned char          bLength;                                      // descriptor size in bytes
    unsigned char          bDescriptorType;                              // device descriptor
    unsigned char          bInterfaceNumber;                             // 
    unsigned char          bAlternateSetting;                            // 
    unsigned char          bNumEndpoints;                                // number of endpoints in addition to EP0
    unsigned char          bInterfaceClass;                              // 
    unsigned char          bInterfaceSubClass;                           // 
    unsigned char          bInterfaceProtocol;                           // 
    unsigned char          iInterface;                                   // string index for interface
} USB_INTERFACE_DESCRIPTOR;

typedef struct _PACK stUSB_HID_DESCRIPTOR                                // HID descriptor
{
    unsigned char          bLength;                                      // descriptor size in bytes
    unsigned char          bDescriptorType;                              // device descriptor
    unsigned char          bcdHID[2];                                    // HID class specific release number
    unsigned char          bCountryCode;                                 // Hardware target country
    unsigned char          bNumDescriptors;                              // Number of HID class descriptors to follow
    unsigned char          bReportDescriptorType;                        // Report Descriptor Type
    unsigned char          wItemLength[2];                               // Total Length of Report Descriptor
} USB_HID_DESCRIPTOR;

typedef struct _PACK stUSB_CDC_FUNCTIONAL_DESCRIPTOR_HEADER              // CDC function descriptor - header
{
    unsigned char          bFunctionLength;                              // descriptor size in bytes
    unsigned char          bDescriptorType;                              // device descriptor
    unsigned char          bDescriptorSubtype;                           // HEADER_FUNCTION_DESCRIPTOR
    unsigned char          bcdCDC[2];                                    // CDC specification release number
} USB_CDC_FUNCTIONAL_DESCRIPTOR_HEADER;

typedef struct _PACK stUSB_CDC_FUNCTIONAL_DESCRIPTOR_ABSTRACT_CONTROL    // CDC function descriptor - adstract control
{
    unsigned char          bLength;                                      // descriptor size in bytes
    unsigned char          bDescriptorType;                              // device descriptor
    unsigned char          bDescriptorSubtype;                           // ABSTRACT_CONTROL_FUNCTION_DESCRIPTOR
    unsigned char          bmCapabilities;
} USB_CDC_FUNCTIONAL_DESCRIPTOR_ABSTRACT_CONTROL;

typedef struct _PACK stUSB_CDC_FUNCTIONAL_DESCRIPTOR_UNION               // CDC function descriptor - union
{
    unsigned char          bLength;                                      // descriptor size in bytes
    unsigned char          bDescriptorType;                              // device descriptor
    unsigned char          bDescriptorSubtype;                           // USB_CDC_FUNCTIONAL_DESCRIPTOR_UNION
    unsigned char          bControlInterface;
    unsigned char          bSubordinateInterface;
} USB_CDC_FUNCTIONAL_DESCRIPTOR_UNION;

typedef struct _PACK stUSB_CDC_FUNCTIONAL_DESCRIPTOR_CALL_MAN            // CDC function descriptor - union
{
    unsigned char          bLength;                                      // descriptor size in bytes
    unsigned char          bDescriptorType;                              // device descriptor
    unsigned char          bDescriptorSubtype;                           // USB_CDC_FUNCTIONAL_DESCRIPTOR_CALL_MAN
    unsigned char          bmCapabilities;
    unsigned char          bDataInterface;
} USB_CDC_FUNCTIONAL_DESCRIPTOR_CALL_MAN;

#define CS_INTERFACE       0x24
#define CS_ENDPOINT        0x25

// Descriptor SubTypes
//
#define HEADER_FUNCTION_DESCRIPTOR                              0

#define CALL_MAN_FUNCTIONAL_DESCRIPTOR                          1
#define ABSTRACT_CONTROL_FUNCTION_DESCRIPTOR                    2
#define UNION_FUNCTIONAL_DESCRIPTOR                             6

#define USB_CDC_FUNCTIONAL_DESCRIPTOR_HEADER_LENGTH             5
#define USB_CDC_FUNCTIONAL_DESCRIPTOR_CALL_MAN_LENGTH           5
#define USB_CDC_FUNCTIONAL_DESCRIPTOR_ABSTRACT_CONTROL_LENGTH   4
#define USB_CDC_FUNCTIONAL_DESCRIPTOR_UNION_LENGTH              5

#define CALL_MAN_FUNCTIONAL_CAPABILITY_NO_CALL_MANAGEMENT       0x00
#define CALL_MAN_FUNCTIONAL_CAPABILITY_HANDLES_CALL_MANAGEMENT  0x01
#define CALL_MAN_FUNCTIONAL_CAPABILITY_CALL_MAN_OVER_DATA_CLASS 0x02

#define ABSTRACT_CONTROL_FUNCTION_CAPABILITY_NONE               0x00
#define ABSTRACT_CONTROL_FUNCTION_CAPABILITY_COMM_FEATURE       0x01
#define ABSTRACT_CONTROL_FUNCTION_CAPABILITY_LINE_STATE_CODING  0x02
#define ABSTRACT_CONTROL_FUNCTION_CAPABILITY_SEND_BREAK         0x04
#define ABSTRACT_CONTROL_FUNCTION_CAPABILITY_NETWORK_CONNECTION 0x08

#define AUDIO_CONTROL_HEADER                                    1
#define AUDIO_CONTROL_INPUT_TERMINAL                            2
#define AUDIO_CONTROL_OUTPUT_TERMINAL                           3
#define AUDIO_CONTROL_MIXER_UIT                                 4
#define AUDIO_CONTROL_SELECTOR_UNIT                             5
#define AUDIO_CONTROL_FEATURE_UNIT                              6
#define AUDIO_CONTROL_PROCESSING_UNIT                           7
#define AUDIO_CONTROL_EXTENSION_UNIT                            8

#define AUDIO_STREAMING_GENERAL                                 1
#define AUDIO_STREAMING_FORMAT_TYPE                             2
#define AUDIO_STREAMING_FORMAT_SPECIFIC                         3

// USB audio endpoint attributes
//
#define AUDIO_ENDPOINT_CONTROL_SAMPLING_FREQUENCY               0x01
#define AUDIO_ENDPOINT_CONTROL_PITCH                            0x02
#define AUDIO_ENDPOINT_CONTROL_MAX_PACKET_SIZE_ONLY             0x80

// USB audio endpoint lock delay units
//
#define AUDIO_ENDPOINT_LOCK_DELAY_UNITS_UNDEFINED               0x00
#define AUDIO_ENDPOINT_LOCK_DELAY_UNITS_MS                      0x01
#define AUDIO_ENDPOINT_LOCK_DELAY_UNITS_PCM_SAMPLES             0x02


// USB audio terminal types
//
#define USB_AUDIO_TERMINAL_UNDEFINED                            0x0100
#define USB_AUDIO_TERMINAL_STREAMING                            0x0101
#define USB_AUDIO_TERMINAL_VENDOR_SPECIFIC                      0x01ff

// USB audio input terminal types
//
#define USB_AUDIO_INPUT_TERMINAL_UNDEFINED                      0x0200
#define USB_AUDIO_INPUT_TERMINAL_MICROPHONE                     0x0201
#define USB_AUDIO_INPUT_TERMINAL_DESKTOP_MICROPHONE             0x0202
#define USB_AUDIO_INPUT_TERMINAL_PERSONAL_MICROPHONE            0x0203
#define USB_AUDIO_INPUT_TERMINAL_OMNI_DIRECT_MICROPHONE         0x0204
#define USB_AUDIO_INPUT_TERMINAL_MICROPHONE_ARRAY               0x0205
#define USB_AUDIO_INPUT_TERMINAL_PROC_MICROPHONE_ARRAY          0x0206

// USB audio output terminal types
//
#define USB_AUDIO_OUTPUT_TERMINAL_UNDEFINED                     0x0300
#define USB_AUDIO_OUTPUT_TERMINAL_SPEAKER                       0x0301
#define USB_AUDIO_OUTPUT_TERMINAL_HEADPHONES                    0x0302
#define USB_AUDIO_OUTPUT_TERMINAL_HEAD_MOUNTED_DISPLAY_AUDIO    0x0303
#define USB_AUDIO_OUTPUT_TERMINAL_DESKTOP_SPEAKER               0x0304
#define USB_AUDIO_OUTPUT_TERMINAL_ROOM_SPEAKER                  0x0305
#define USB_AUDIO_OUTPUT_TERMINAL_COMMUNICATION_SPEAKER         0x0306
#define USB_AUDIO_OUTPUT_TERMINAL_LF_EFFECTS_SPEAKER            0x0307
// USB audio channel cluster format
//
#define USB_AUDIO_CHANNEL_LEFT_FRONT                            0x0001
#define USB_AUDIO_CHANNEL_RIGHT_FRONT                           0x0002
#define USB_AUDIO_CHANNEL_CENTER_FRONT                          0x0004
#define USB_AUDIO_CHANNEL_LF_ENHANCEMENT                        0x0008
#define USB_AUDIO_CHANNEL_LEFT_SURROUND                         0x0010
#define USB_AUDIO_CHANNEL_RIGHT_SURROUND                        0x0020
#define USB_AUDIO_CHANNEL_LEFT_OF_CENTER                        0x0040
#define USB_AUDIO_CHANNEL_RIGHT_OF_CENTER                       0x0080
#define USB_AUDIO_CHANNEL_SURROUND                              0x0100
#define USB_AUDIO_CHANNEL_SIDE_LEFT                             0x0200
#define USB_AUDIO_CHANNEL_SIDE_RIGHT                            0x0400
#define USB_AUDIO_CHANNEL_TOP                                   0x0800

// USB audio feature unit controls
//
#define USB_AUDIO_UNIT_CONTROL_MUTE                             0x0001
#define USB_AUDIO_UNIT_CONTROL_VOLUME                           0x0002
#define USB_AUDIO_UNIT_CONTROL_BASS                             0x0004
#define USB_AUDIO_UNIT_CONTROL_MID                              0x0008
#define USB_AUDIO_UNIT_CONTROL_TREBLE                           0x0010
#define USB_AUDIO_UNIT_CONTROL_GRAPHIC_EQUALISER                0x0020
#define USB_AUDIO_UNIT_CONTROL_AGC                              0x0040
#define USB_AUDIO_UNIT_CONTROL_DELAY                            0x0080
#define USB_AUDIO_UNIT_CONTROL_BASS_BOOST                       0x0100
#define USB_AUDIO_UNIT_CONTROL_LOUDNESS                         0x0200

// USB audio format type codes
//
#define USB_AUDIO_FORMAT_TYPE_UNDEFINED                         0x00
#define USB_AUDIO_FORMAT_TYPE_I                                 0x01
#define USB_AUDIO_FORMAT_TYPE_II                                0x02
#define USB_AUDIO_FORMAT_TYPE_III                               0x03

// USB audio format type I codes
//
#define USB_AUDIO_FORMAT_TYPE_I_UNDEFINED                       0x0000
#define USB_AUDIO_FORMAT_TYPE_I_PCM                             0x0001
#define USB_AUDIO_FORMAT_TYPE_I_PCM8                            0x0002
#define USB_AUDIO_FORMAT_TYPE_I_IEEE_FLOAT                      0x0003
#define USB_AUDIO_FORMAT_TYPE_I_ALAW                            0x0004
#define USB_AUDIO_FORMAT_TYPE_I_MULAW                           0x0005

// CDC management element notifications
//
#define CDC_NETWORK_CONNECTION          0x00
#define CDC_RESPONSE_AVAILABLE          0x01
#define CDC_AUX_JACK_HOOK_STATE         0x08                             // described in USBPSTN1.2
#define CDC_RING_DETECT                 0x09                             // described in USBPSTN1.2
#define CDC_SERIAL_STATE                0x20                             // described in USBPSTN1.2
#define CDC_CALL_STATE_CHANGE           0x28                             // described in USBPSTN1.2
#define CDC_LINE_STATE_CHANGE           0x29                             // described in USBPSTN1.2
#define CDC_CONNECTION_SPEED_CHANGE     0x2a

#define CDC_REQUEST_TYPE_NOTIFICATION   0xa1

typedef struct _PACK CDC_NOTIFICATION_HEADER
{
    unsigned char          bmRequestType;                                // CDC_REQUEST_TYPE_NOTIFICATION
    unsigned char          bNotificationCode;                            // code (eg. CDC_NETWORK_CONNECTION)
    unsigned char          wValue[2];                                    // state value for CDC_NETWORK_CONNECTION and CDC_AUX_JACK_HOOK_STATE (zero for others)
    unsigned char          wIndex[2];                                    // interface for CDC_NETWORK_CONNECTION, CDC_RESPONSE_AVAILABLE and CDC_CONNECTION_SPEED_CHANGE
    unsigned char          wLength[2];                                   // 8 for CDC_CONNECTION_SPEED_CHANGE (zero for others)
    // Optional data
    // - 8 bytes for CDC_CONNECTION_SPEED_CHANGE (connection speed change data structure)
    // - 2 for CDC_SERIAL_STATE (UART state bitmap)
    // - no data for others
    //
} CDC_NOTIFICATION_HEADER;

#define CDC_NETWORK_CONNECTION_DISCONNECTED     LITTLE_SHORT_WORD(0x0000)
#define CDC_NETWORK_CONNECTION_CONNECTED        LITTLE_SHORT_WORD(0x0001)
#define CDC_AUX_JACK_HOOK_STATE_ON_HOOK         LITTLE_SHORT_WORD(0x0000)
#define CDC_AUX_JACK_HOOK_STATE_OFF_HOOK        LITTLE_SHORT_WORD(0x0001)

// Bitmap values
//
#define CDC_SERIAL_STATE_bRxCarrier             LITTLE_SHORT_WORD(0x0001) // DCD equivalent
#define CDC_SERIAL_STATE_bTxCarrier             LITTLE_SHORT_WORD(0x0002) // DSR equivalent
#define CDC_SERIAL_STATE_bBreak                 LITTLE_SHORT_WORD(0x0004)
#define CDC_SERIAL_STATE_bRingSignal            LITTLE_SHORT_WORD(0x0008)
#define CDC_SERIAL_STATE_bFraming               LITTLE_SHORT_WORD(0x0010)
#define CDC_SERIAL_STATE_bParity                LITTLE_SHORT_WORD(0x0020)
#define CDC_SERIAL_STATE_bOverRun               LITTLE_SHORT_WORD(0x0040)


#if defined USB_CDC_RNDIS                                                // {8}
// General objects
//
#define OID_GEN_SUPPORTED_LIST                  0x00010101               // required
#define OID_GEN_HARDWARE_STATUS                 0x00010102               // required
#define OID_GEN_MEDIA_SUPPORTED                 0x00010103               // required
#define OID_GEN_MEDIA_IN_USE                    0x00010104               // required
#define OID_GEN_MAXIMUM_LOOKAHEAD               0x00010105
#define OID_GEN_MAXIMUM_FRAME_SIZE              0x00010106               // required
#define OID_GEN_LINK_SPEED                      0x00010107               // required
#define OID_GEN_TRANSMIT_BUFFER_SPACE           0x00010108
#define OID_GEN_RECEIVE_BUFFER_SPACE            0x00010109
#define OID_GEN_TRANSMIT_BLOCK_SIZE             0x0001010a               // required
#define OID_GEN_RECEIVE_BLOCK_SIZE              0x0001010b               // required
#define OID_GEN_VENDOR_ID                       0x0001010c               // required
#define OID_GEN_VENDOR_DESCRIPTION              0x0001010d               // required
#define OID_GEN_CURRENT_PACKET_FILTER           0x0001010e               // required
#define OID_GEN_CURRENT_LOOKAHEAD               0x0001010f
#define OID_GEN_DRIVER_VERSION                  0x00010110
#define OID_GEN_MAXIMUM_TOTAL_SIZE              0x00010111               // required
#define OID_GEN_PROTOCOL_OPTIONS                0x00010112
#define OID_GEN_MAC_OPTIONS                     0x00010113               // optional
#define OID_GEN_MEDIA_CONNECT_STATUS            0x00010114               // required
#define OID_GEN_MAXIMUM_SEND_PACKETS            0x00010115
#define OID_GEN_VENDOR_DRIVER_VERSION           0x00010116               // required
#define OID_GEN_XMIT_OK                         0x00020101               // required
#define OID_GEN_RCV_OK                          0x00020102               // required
#define OID_GEN_XMIT_ERROR                      0x00020103               // required
#define OID_GEN_RCV_ERROR                       0x00020104               // required
#define OID_GEN_RCV_NO_BUFFER                   0x00020105               // required
#define OID_GEN_DIRECTED_BYTES_XMIT             0x00020201               // optional
#define OID_GEN_DIRECTED_FRAMES_XMIT            0x00020202               // optional
#define OID_GEN_MULTICAST_BYTES_XMIT            0x00020203               // optional
#define OID_GEN_MULTICAST_FRAMES_XMIT           0x00020204               // optional
#define OID_GEN_BROADCAST_BYTES_XMIT            0x00020205               // optional
#define OID_GEN_BROADCAST_FRAMES_XMIT           0x00020206               // optional
#define OID_GEN_DIRECTED_BYTES_RCV              0x00020207               // optional
#define OID_GEN_DIRECTED_FRAMES_RCV             0x00020208               // optional
#define OID_GEN_MULTICAST_BYTES_RCV             0x00020209               // optional
#define OID_GEN_MULTICAST_FRAMES_RCV            0x0002020a               // optional
#define OID_GEN_BROADCAST_BYTES_RCV             0x0002020b               // optional
#define OID_GEN_BROADCAST_FRAMES_RCV            0x0002020c               // optional
#define OID_GEN_RCV_CRC_ERROR                   0x0002020d               // optional
#define OID_GEN_TRANSMIT_QUEUE_LENGTH           0x0002020e               // optional
#define OID_GEN_GET_TIME_CAPS                   0x0002020f
#define OID_GEN_GET_NETCARD_TIME                0x00020210

// 802.3 Ethernet operational characteristics
//
#define OID_802_3_PERMANENT_ADDRESS             0x01010101               // required
#define OID_802_3_CURRENT_ADDRESS               0x01010102               // required
#define OID_802_3_MULTICAST_LIST                0x01010103               // required
#define OID_802_3_MAXIMUM_LIST_SIZE             0x01010104               // required
#define OID_802_3_MAC_OPTIONS                   0x01010105               // optional

// 802.3 Ethernet statistics
//
#define OID_802_3_RCV_ERROR_ALIGNMENT           0x01020101
#define OID_802_3_XMIT_ONE_COLLISION            0x01020102
#define OID_802_3_XMIT_MORE_COLLISIONS          0x01020103
#define OID_802_3_XMIT_DEFERRED                 0x01020201
#define OID_802_3_XMIT_MAX_COLLISIONS           0x01020202
#define OID_802_3_RCV_OVERRUN                   0x01020203
#define OID_802_3_XMIT_UNDERRUN                 0x01020204
#define OID_802_3_XMIT_HEARTBEAT_FAILURE        0x01020205
#define OID_802_3_XMIT_TIMES_CRS_LOST           0x01020206
#define OID_802_3_XMIT_LATE_COLLISIONS          0x01020207

#define OID_GENERAL_OBJECT                      0x00                     // object addressing levels
#define OID_GENERAL_OPERATIONAL_CHAR                0x01
#define OID_GENERAL_STATISTICS                      0x02
#define OID_GENERAL_STATISTICS_1                        0x01
#define OID_GENERAL_STATISTICS_2                        0x02
#define OID_ETHERNET_OBJECT                     0x01
#define OID_ETHERNET_OPERATIONAL_CHAR               0x01
#define OID_ETHERNET_STATISTICS                     0x02
#define OID_ETHERNET_STATISTICS_1                       0x01
#define OID_ETHERNET_STATISTICS_2                       0x02

#define NON_REGISTERED_VENDOR                   0xffffff00               // value to use when no IEEE OUI is available

// Ndis Packet Filter Bits (OID_GEN_CURRENT_PACKET_FILTER).
//
#define NDIS_PACKET_TYPE_DIRECTED               0x0001
#define NDIS_PACKET_TYPE_MULTICAST              0x0002
#define NDIS_PACKET_TYPE_ALL_MULTICAST          0x0004
#define NDIS_PACKET_TYPE_BROADCAST              0x0008
#define NDIS_PACKET_TYPE_SOURCE_ROUTING         0x0010
#define NDIS_PACKET_TYPE_PROMISCUOUS            0x0020
#define NDIS_PACKET_TYPE_SMT                    0x0040
#define NDIS_PACKET_TYPE_ALL_LOCAL              0x0080
#define NDIS_PACKET_TYPE_MAC_FRAME              0x8000
#define NDIS_PACKET_TYPE_FUNCTIONAL             0x4000
#define NDIS_PACKET_TYPE_ALL_FUNCTIONAL         0x2000
#define NDIS_PACKET_TYPE_GROUP                  0x1000

// RNDIS encapsulated messages
//
typedef struct _PACK stREMOTE_NDIS_INITIALIZE_MESSAGE
{
    unsigned char          ucMessageType[4];                             // type of message being sent
    unsigned char          ucMessageLength[4];                           // total length of this message
    unsigned char          ucRequestID[4];                               // RNDIS message ID value
    unsigned char          ucMajorVersion[4];                            // RNDIS protocol version implemented by the host - major
    unsigned char          ucMinorVersion[4];                            // RNDIS protocol version implemented by the host - minor
    unsigned char          ucMaxTransferSize[4];                         // maximum size that can be received
} REMOTE_NDIS_INITIALIZE_MESSAGE;

typedef struct _PACK stREMOTE_NDIS_INITIALIZE_HALT_MESSAGE
{
    unsigned char          ucMessageType[4];                             // type of message being sent
    unsigned char          ucMessageLength[4];                           // total length of this message
    unsigned char          ucRequestID[4];                               // RNDIS message ID value
} REMOTE_NDIS_HALT_MESSAGE;

typedef struct _PACK stREMOTE_NDIS_QUERY_MESSAGE
{
    unsigned char          ucMessageType[4];                             // type of message being sent
    unsigned char          ucMessageLength[4];                           // total length of this message
    unsigned char          ucRequestID[4];                               // RNDIS message ID value
    unsigned char          ucOid[4];                                     // RNDIS object identifier that identifies the parameter being queried
    unsigned char          ucInformationBufferLength[4];                 // length of the input data for the query
    unsigned char          ucInformationBufferOffset[4];
    unsigned char          ucDeviceVcHandle[4];                          // reserved for connection oriented devices
} REMOTE_NDIS_QUERY_MESSAGE;

typedef struct _PACK stREMOTE_NDIS_INIT_COMPLETE_MESSAGE
{
    unsigned char          ucMessageType[4];                             // type of message being sent
    unsigned char          ucMessageLength[4];                           // total length of this message
    unsigned char          ucRequestID[4];                               // RNDIS message ID value
    unsigned char          ucStatus[4];
    unsigned char          ucMajorVersion[4];                            // RNDIS protocol version implemented by the host - major
    unsigned char          ucMinorVersion[4];                            // RNDIS protocol version implemented by the host - minor
    unsigned char          ucDeviceFlags[4];
    unsigned char          ucMedium[4];
    unsigned char          ucMaxPacketsPerTransfer[4];
    unsigned char          ucMaxTransferSize[4];
    unsigned char          ucPacketAlignmentFactor[4];
    unsigned char          ucAfListOffset[4];
    unsigned char          ucAfListSize[4];
} REMOTE_NDIS_INIT_COMPLETE_MESSAGE;

typedef struct _PACK stREMOTE_NDIS_QUERY_COMPLETE_MESSAGE
{
    unsigned char          ucMessageType[4];                             // type of message being sent
    unsigned char          ucMessageLength[4];                           // total length of this message
    unsigned char          ucRequestID[4];                               // RNDIS message ID value
    unsigned char          ucStatus[4];
    unsigned char          ucInformationBufferLength[4];
    unsigned char          ucInformationBufferOffset[4];
} REMOTE_NDIS_QUERY_COMPLETE_MESSAGE;

typedef struct _PACK stREMOTE_NDIS_SET_COMPLETE_MESSAGE
{
    unsigned char          ucMessageType[4];                             // type of message being sent
    unsigned char          ucMessageLength[4];                           // total length of this message
    unsigned char          ucRequestID[4];                               // RNDIS message ID value
    unsigned char          ucStatus[4];
} REMOTE_NDIS_SET_COMPLETE_MESSAGE;

typedef struct _PACK stREMOTE_NDIS_INDICATE_STATUS_MESSAGE
{
    unsigned char          ucMessageType[4];                             // type of message being sent
    unsigned char          ucMessageLength[4];                           // total length of this message
    unsigned char          ucStatus[4];
    unsigned char          ucStatusBufferLength[4];
    unsigned char          ucStatusBufferOffset[4];
} REMOTE_NDIS_INDICATE_STATUS_MESSAGE;

typedef struct _PACK stREMOTE_NDIS_RESPONSE_AVAILABLE
{
    unsigned char          ucType;                                       // type of message being sent
    unsigned char          ucComand;                                     // specific command
    unsigned char          ucValue[2];
    unsigned char          ucInterface[2];
    unsigned char          ucLength[2];
} REMOTE_NDIS_RESPONSE_AVAILABLE;

// RNDIS commands and responses
//
#define REMOTE_NDIS_INITIALIZE_MSG        0x00000002
#define REMOTE_NDIS_INITIALIZE_COMPLETE   0x80000002
#define REMOTE_NDIS_HALT_MSG              0x00000003
#define REMOTE_NDIS_QUERY_MSG             0x00000004
#define REMOTE_NDIS_QUERY_COMPLETE        0x80000004
#define REMOTE_NDIS_SET_MSG               0x00000005
#define REMOTE_NDIS_SET_COMPLETE          0x80000005
#define REMOTE_NDIS_RESET_MSG             0x00000006
#define REMOTE_NDIS_RESET_COMPLETE        0x80000006
#define REMOTE_NDIS_INDICATE_STATUS_MSG   0x00000007
#define REMOTE_NDIS_KEEPALIVE_MSG         0x00000008
#define REMOTE_NDIS_KEEPALIVE_COMPLETE    0x80000008


#define TYPE_NOTIFICATION_REQUEST         0xa1
#define RESPONSE_NOTIFICATION_AVAILABLE   0x01

#define RNDIS_MAJOR_VERSION               1
#define RNDIS_MINOR_VERSION               0

// RNDIS status values 
//
#define RNDIS_STATUS_SUCCESS              0x00000000
#define RNDIS_STATUS_NOT_RECOGNIZED       0x00010001
#define RNDIS_STATUS_NOT_SUPPORTED        0xc00000bb
#define RNDIS_STATUS_NOT_ACCEPTED         0x00010003
#define RNDIS_STATUS_FAILURE              0xc0000001
#define RNDIS_STATUS_RESOURCES            0xc000009a
#define RNDIS_STATUS_CLOSING              0xc0010002
#define RNDIS_STATUS_CLOSING_INDICATING   0xc001000e
#define RNDIS_STATUS_RESET_IN_PROGRESS    0xc001000d
#define RNDIS_STATUS_INVALID_LENGTH       0xc0010014
#define RNDIS_STATUS_BUFFER_TOO_SHORT     0xc0010016
#define RNDIS_STATUS_INVALID_DATA         0xc0010015
#define RNDIS_STATUS_INVALID_OID          0xc0010017
#define RNDIS_STATUS_MEDIA_CONNECT        0x4001000b
#define RNDIS_STATUS_MEDIA_DISCONNECT     0x4001000c

#define NDIS_MEDIA_STATE_CONNECTED       (0x00000000)
#define NDIS_MEDIA_STATE_DISCONNECTED    (0x00000001) 
#define NDIS_MEDIA_STATE_UNKNOWN         (0xffffffff)

// Descriptor defines
//
#define RNDIS_CONNECTIONLESS_FLAGS        0x00000000
#define RNDIS_CONNECTION_ORIENTED_FLAGS   0x00000001

#define RNDIS_MEDIUM_802_3                0x00000000
#define RNDIS_MEDIUM_802_3                0x00000000
#define RNDIS_MEDIUM_802_5                0x00000001
#define RNDIS_MEDIUM_FDDI                 0x00000002
#define RNDIS_MEDIUM_WAN                  0x00000003
#define RNDIS_MEDIUM_LOCAL_TALK           0x00000004
#define RNDIS_MEDIUM_DIX                  0x00000005
#define RNDIS_MEDIUM_ARCENT_RAW           0x00000006
#define RNDIS_MEDIUM_ARCENT_878_2         0x00000007
#define RNDIS_MEDIUM_ATM                  0x00000008
#define RNDIS_MEDIUM_WIRELESS_LAN         0x00000009
#define RNDIS_MEDIUM_IRDA                 0x0000000a
#define RNDIS_MEDIUM_BPC                  0x0000000b
#define RNDIS_MEDIUM_CO_WAN               0x0000000c
#define NDIS_MEDIUM_1394                  0x0000000d

#define USB_ETH_MTU                       (ETH_MTU + 58)

#define REMOTE_NDIS_PACKET_MSG 0x00000001

typedef struct _PACK stREMOTE_NDIS_PACKET_MESSAGE {
    unsigned char          ucMessageType[4];                             // type of message being sent
    unsigned char          ucMessageLength[4];                           // total length of this message
    unsigned char          ucDataOffset[4];
    unsigned char          ucDataLength[4];
    unsigned char          ucOutOfBandDataOffset[4];
    unsigned char          ucOutOfBandDataLength[4];
    unsigned char          ucPerPacketInfoOffset[4];
    unsigned char          ucPerPacketInfoLength[4];
    unsigned char          ucVcHandle[4];                                // reserved for connection-oriented devices
    unsigned char          ucReserved[8];                                // must be zero
} REMOTE_NDIS_PACKET_MESSAGE;

typedef struct stREMOTE_NDIS_CONTROL
{
    unsigned long ulMaxRNDISmessageLength;
    unsigned short usTotalExpectControlData;
    QUEUE_TRANSFER encapsulatedResponseLength;
    unsigned char remoteNDISresponse[sizeof(REMOTE_NDIS_QUERY_COMPLETE) + 128]; // buffer to construct remote NDIS message responses in
    unsigned char remoteNDISstatus[sizeof(REMOTE_NDIS_INDICATE_STATUS_MESSAGE) + 8];
    unsigned char statusResponseLength;
    unsigned char ucRNDIS_state;
} REMOTE_NDIS_CONTROL;

typedef struct _PACK stREMOTE_NDIS_ETHERNET_PACKET_MESSAGE {
    REMOTE_NDIS_PACKET_MESSAGE rndis_message;
    unsigned char          ucPayload[USB_ETH_MTU];                       // space for maximum Ethernet data length
} REMOTE_NDIS_ETHERNET_PACKET_MESSAGE;
#endif

typedef struct _PACK stUSB_ENDPOINT_DESCRIPTOR
{
    unsigned char          bLength;                                      // descriptor size in bytes
    unsigned char          bDescriptorType;                              // device descriptor
    unsigned char          bEndpointAddress;                             // direction and address of endpoint
    unsigned char          bmAttributes;                                 // endpoint attributes
    unsigned char          wMaxPacketSize[2];                            // endpoint FIFO size
    unsigned char          bInterval;                                    // polling interval in ms
} USB_ENDPOINT_DESCRIPTOR;

typedef struct _PACK stUSB_AUDIO_CONTROL_ENDPOINT_DESCRIPTOR
{
    unsigned char          bLength;                                      // descriptor size in bytes
    unsigned char          bDescriptorType;                              // device descriptor
    unsigned char          bEndpointAddress;                             // direction and address of endpoint
    unsigned char          bmAttributes;                                 // endpoint attributes
    unsigned char          wMaxPacketSize[2];                            // endpoint FIFO size
    unsigned char          bInterval;                                    // polling interval in ms
    unsigned char          bRefresh;                                     // reset to 0
    unsigned char          bSynchAddress;                                // synch endpoint address - reset to 0
} USB_AUDIO_CONTROL_ENDPOINT_DESCRIPTOR;

typedef struct _PACK stUSB_AUDIO_CONTROL_INTERFACE_HEADER_DESCRIPTOR_2
{
    unsigned char          bLength;                                      // descriptor size in bytes
    unsigned char          bDescriptorType;                              // device descriptor
    unsigned char          bDescriptorSubtype;                           // descriptor subtype
    unsigned char          bcdADC[2];                                    // audio device class specification release number
    unsigned char          wTotalLength[2];
    unsigned char          bInCollection;                                // number of audio-streaming and MIDI-streaming interfaces in the audio interface collection
    unsigned char          baInterfaceNr1;                               // interface number of the first audio-streaming or MIDI-streaming interface in the collection
    unsigned char          baInterfaceNr2;                               // interface number of the last audio-streaming or MIDI-streaming interface in the collection
} USB_AUDIO_CONTROL_INTERFACE_HEADER_DESCRIPTOR_2;

typedef struct _PACK stUSB_AUDIO_INPUT_TERMINAL_DESCRIPTOR
{
    unsigned char          bLength;                                      // descriptor size in bytes
    unsigned char          bDescriptorType;                              // device descriptor
    unsigned char          bDescriptorSubtype;                           // descriptor subtype
    unsigned char          bTerminalID;                                  // identifier of the terminal within the audio function
    unsigned char          wTerminalType[2];                             // terminal type
    unsigned char          bAssocTerminal;                               // identifier of the output terminal to which this input terminal is associated
    unsigned char          bNrChannels;                                  // number of logical output channels in the terminal's output audio channel cluster
    unsigned char          wChannelConfig[2];                            // spacial location of the logical channels
    unsigned char          iChannelNames;                                // index of string for the first logical channel
    unsigned char          iTerminal;                                    // index of string for the input terminal
} USB_AUDIO_INPUT_TERMINAL_DESCRIPTOR;

typedef struct _PACK stUSB_AUDIO_OUTPUT_TERMINAL_DESCRIPTOR
{
    unsigned char          bLength;                                      // descriptor size in bytes
    unsigned char          bDescriptorType;                              // device descriptor
    unsigned char          bDescriptorSubtype;                           // descriptor subtype
    unsigned char          bTerminalID;                                  // identifier of the terminal within the audio function
    unsigned char          wTerminalType[2];                             // terminal type
    unsigned char          bAssocTerminal;                               // identifier of the input terminal to which this output terminal is associated
    unsigned char          bSourceID;                                    // identifier of the unit or terminal to which this terminal is connected
    unsigned char          iTerminal;                                    // index of string for the output terminal
} USB_AUDIO_OUTPUT_TERMINAL_DESCRIPTOR;

typedef struct _PACK stUSB_AUDIO_FEATURE_UNIT_DESCRIPTOR
{
    unsigned char          bLength;                                      // descriptor size in bytes
    unsigned char          bDescriptorType;                              // device descriptor
    unsigned char          bDescriptorSubtype;                           // descriptor subtype
    unsigned char          bUnitID;                                      // identifier of the unit within the audio function
    unsigned char          bSourceID;                                    // identifier of the unit or terminal to which the feature unit is connected
    unsigned char          bControlSize;                                 // size in bytes of an element of the bmsControls() array
    unsigned char          bmaControls[2];                               // bitmap
    unsigned char          iFeature;                                     // index of string for the feature unit
} USB_AUDIO_FEATURE_UNIT_DESCRIPTOR;

typedef struct _PACK stUSB_ISO_AUDIO_DATA_ENDPOINT_DESCRIPTOR
{
    unsigned char          bLength;                                      // descriptor size in bytes
    unsigned char          bDescriptorType;                              // device descriptor
    unsigned char          bDescriptorSubtype;                           // descriptor subtype
    unsigned char          bmAttributes;                                 // attributes
    unsigned char          bLockDelayUnits;
    unsigned char          wLockDelay[2];
} USB_ISO_AUDIO_DATA_ENDPOINT_DESCRIPTOR;

typedef struct _PACK stUSB_AUDIO_STREAMING_GENERAL_INTERFACE_DESCRIPTOR
{
    unsigned char          bLength;                                      // descriptor size in bytes
    unsigned char          bDescriptorType;                              // device descriptor
    unsigned char          bDescriptorSubtype;                           // descriptor subtype
    unsigned char          bTerminalLink;                                // unit ID or the output terminal
    unsigned char          bDelay;                                       // interface delay
    unsigned char          wFormatTag[2];                                // PCM format
} USB_AUDIO_STREAMING_GENERAL_INTERFACE_DESCRIPTOR;

typedef struct _PACK stUSB_AUDIO_TYPE_I_FORMAT_TYPE_DESCRIPTOR
{
    unsigned char          bLength;                                      // descriptor size in bytes
    unsigned char          bDescriptorType;                              // device descriptor
    unsigned char          bDescriptorSubtype;                           // descriptor subtype
    unsigned char          bFormatType;                                  // format type
    unsigned char          bNrChannels;                                  // number of channels
    unsigned char          bSubFrameSize;                                // number of bytes per audio subframe
    unsigned char          bBitResolution;                               // bits per sample
    unsigned char          bSamFreqType;                                 // number of frequencies supported
    unsigned char          tSamFreq[3];                                  // sample frequency
} USB_AUDIO_TYPE_I_FORMAT_TYPE_DESCRIPTOR;
__PACK_OFF

typedef struct stUSB_ENDPOINT_DESCRIPTOR_ENTRIES
{
    const USB_ENDPOINT_DESCRIPTOR *ptrActiveEndpoint;
    unsigned short usMaxEndpointLength;
} USB_ENDPOINT_DESCRIPTOR_ENTRIES;

#define IN_ENDPOINT                     0x80
#define OUT_ENDPOINT                    0x00

#define ENDPOINT_CONTROL                   0
#define ENDPOINT_ISOCHRONOUS               1
#define ENDPOINT_BULK                      2
#define ENDPOINT_INTERRUPT                 3
#define ENDPOINT_TERMINATE_TRANSMISSION 0x40                             // pseudo attribute so that this endpoint sends a zero frame on buffer termination
#define ENDPOINT_DISABLE                0xff

// USB2.0 extended attributes for isochronous endpoints
//
#define NO_SYNCHRONISATION                 0
#define ASYNCHRONOUS_SYNCHRONISATION    0x04
#define ADAPTIVE_SYNCHRONISATION        0x08
#define SYNCHRONOUS_SYNCHRONISATION     0x0c

// USB2.0 extended attributes for use
//
#define DATA_ENDPOINT_USE                      0
#define EXPLICIT_FEEDBACK_ENDPOINT_USE      0x10                         // isochronous feedback endpoints must use NO_SYNCHRONISATION
#define IMPLICIT_FEEDBACK_DATA_ENDPOINT_USE 0x20                         // isochronous feedback endpoints must use NO_SYNCHRONISATION

#define AUDIO_CLASS                        1
#define USB_CLASS_COMMUNICATION_CONTROL    2
#define USB_CLASS_HID                      3
#define INTERFACE_CLASS_MASS_STORAGE       8                             // {2}
#define INTERFACE_CLASS_HUB                9
#define INTERFACE_CLASS_COMMUNICATION_DATA 0x0a

#define AUDIO_SUBCLASS_CONTROL             1
#define AUDIO_SUBCLASS_STREAMING           2

#define USB_SUBCLASS_BOOT_INTERFACE        1

#define USB_INTERFACE_PROTOCOL_NONE        0
#define USB_INTERFACE_PROTOCOL_KEYBOARD    1                             // only meaningful for boot interface
#define USB_INTERFACE_PROTOCOL_MOUSE       2                             // only meaningful for boot interface
#define USB_ABSTRACT_LINE_CONTROL_MODEL    2

#define ATAPI_CD_DVD_DEVICE                2                             // {2}
#define QIC157_TAPE_DEVICE                 3
#define USB_FLOPPY_INTERFACE               4                             // UFI
#define ATAPI_REMOVABLE_MEDIA              5
#define GENERIC_SCSI_MEDIA                 6

#define CBI_WITH_COMMAND_COMPLETION_INTERRUPT_TRANSFER    0x00           // {2}
#define CBI_WITHOUT_COMMAND_COMPLETION_INTERRUPT_TRANSFER 0x01
#define BULK_ONLY_TRANSPORT                               0x50

__PACK_ON                                                                // compilers using pragmas to control packing will start struct packing from here
typedef struct _PACK stUSB_SETUP_HEADER
{
    unsigned char bmRequestType;
    unsigned char bRequest;
    unsigned char wValue[2];
    unsigned char wIndex[2];
    unsigned char wLength[2];
} USB_SETUP_HEADER;

typedef struct _PACK stUSB_STRING_DESCRIPTOR
{
    unsigned char          bLength;                                      // descriptor size in bytes
    unsigned char          bDescriptorType;                              // device descriptor
    unsigned char          unicode_string_space[2];                      // first unicode location
    // Variable length
    //
} USB_STRING_DESCRIPTOR;

#define REMOTE_WAKEUP_SUPPORTED        0x20
#define BUS_POWERED                    0x00
#define SELF_POWERED                   0x40
#define ATTRIBUTE_DEFAULT              0x80                              // must always be set

#define STANDARD_DEVICE_DESCRIPTOR_LENGTH            0x12
#define DEVICE_QUALIFIER_DESCRIPTOR_LENGTH           0x0a
#define DESCRIPTOR_TYPE_ENDPOINT_LENGTH              0x07
#define DESCRIPTOR_TYPE_AUDIO_CLASS_ENDPOINT_LENGTH  0x09
#define DESCRIPTOR_TYPE_CONFIGURATION_LENGTH         0x09
#define DESCRIPTOR_TYPE_INTERFACE_LENGTH             0x09
#define DESCRIPTOR_TYPE_INTERFACE_ASSOCIATION_LENGTH 0x08
#define DESCRIPTOR_TYPE_HID_LENGTH                   0x09 

#define DESCRIPTOR_TYPE_DEVICE                       0x01
#define DESCRIPTOR_TYPE_CONFIGURATION                0x02
#define DESCRIPTOR_TYPE_STRING                       0x03
#define DESCRIPTOR_TYPE_INTERFACE                    0x04
#define DESCRIPTOR_TYPE_ENDPOINT                     0x05
#define DESCRIPTOR_DEVICE_QUALIFIER                  0x06
#define DESCRIPTOR_TYPE_INTERFACE_ASSOCIATION        0x0b

#define MAJOR_USB_VERSION              1                                 // index to major version
#define MINOR_USB_VERSION              0                                 // index to minor version

#define REQUEST_DEVICE_STANDARD        0x00                              // bmRequestType.Recipient == Device, bmRequestType.Type == Standard
#define REQUEST_DEVICE_VENDOR          0x40                              // bmRequestType.Recipient == Device, bmRequestType.Type == Vendor
#define REQUEST_INTERFACE_STANDARD     0x01                              // bmRequestType.Recipient == Interface, bmRequestType.Type == Standard
#define REQUEST_INTERFACE_CLASS        0x21                              // bmRequestType.Recipient == Interface, bmRequestType.Type == Class
#define REQUEST_ENDPOINT_STANDARD      0x02                              // {4}
#define REQUEST_ENDPOINT_OTHER         0x03

#define USB_REQUEST_GET_STATUS         0                                 // request field
#define USB_REQUEST_CLEAR_FEATURE      1
#define USB_REQUEST_SET_FEATURE        3
#define USB_REQUEST_SET_ADDRESS        5
#define USB_REQUEST_GET_DESCRIPTOR     6
#define USB_REQUEST_SET_DESCRIPTOR     7
#define USB_REQUEST_GET_CONFIGURATION  8
#define USB_REQUEST_SET_CONFIGURATION  9
#define USB_REQUEST_GET_INTERFACE      10
#define USB_REQUEST_SET_INTERFACE      11
#define USB_REQUEST_SYNCH_FRAME        12

#define ENDPOINT_STATUS_STALLED        0x0001

// Communication Device Class
//
#define SEND_ENCAPSULATED_COMMAND      0x00
#define GET_ENCAPSULATED_RESPONSE      0x01
#define SET_LINE_CODING                0x20
#define GET_LINE_CODING                0x21
#define SET_CONTROL_LINE_STATE         0x22
  #define CDC_DTR                      0x01                              // bitmap values in wValue
  #define CDC_RTS                      0x02

typedef struct _PACK stCDC_PSTN_LINE_CODING
{
    unsigned char          dwDTERate[4];                                 // data terminal rate in bits per second
    unsigned char          bCharFormat;                                  // stop bits
    unsigned char          bParityType;                                  // parity
    unsigned char          bDataBits;                                    // data bits
} CDC_PSTN_LINE_CODING;

#define CDC_PSTN_1_STOP_BIT   0
#define CDC_PSTN_1_5_STOP_BIT 1
#define CDC_PSTN_2_STOP_BITS  2

#define CDC_PSTN_NO_PARITY    0
#define CDC_PSTN_ODD_PARITY   1
#define CDC_PSTN_EVEN_PARITY  2
#define CDC_PSTN_MARK_PARITY  3
#define CDC_PSTN_SPACE_PARITY 4

#define CDC_PSTN_5_DATA_BITS  5
#define CDC_PSTN_6_DATA_BITS  6
#define CDC_PSTN_7_DATA_BITS  7
#define CDC_PSTN_8_DATA_BITS  8
#define CDC_PSTN_16_DATA_BITS 16

// HID
//
#define DESCRIPTOR_TYPE_HID      0x21
#define DESCRIPTOR_TYPE_REPORT   0x22
#define DESCRIPTOR_TYPE_PHYSICAL 0x23

#define HID_GET_REPORT        0x01                                       // mandatory
#define HID_GET_IDLE          0x02
#define HID_GET_PROTOCOL      0x03                                       // mandatory only for boot devices
#define HID_SET_REPORT        0x09
#define HID_SET_IDLE          0x0a                                       // silence a particular report on the interrupt IN pipe, with optional expiration time 
#define HID_SET_PROTOCOL      0x0b                                       // mandatory only for boot devices

// Interface for application to access device information
//
extern void *fnGetUSB_config_descriptor(unsigned short *usLength);
extern void *fnGetUSB_device_descriptor(unsigned short *usLength);
extern unsigned char *fnGetUSB_string_entry(unsigned char ucStringRef, unsigned short *usLength); // {9}

extern void *fnGetDeviceInfo(int iInfoRef);                              // used by host application to access device information
    #define REQUEST_USB_DEVICE_DESCRIPTOR 0
    #define REQUEST_USB_CONFIG_DESCRIPTOR 1
    #define REQUEST_USB_STRING_DESCRIPTOR 2

extern QUEUE_TRANSFER fnClearFeature(QUEUE_LIMIT control_handle, unsigned char ucEndpoint);


// Mass Storage
//
#define BULK_ONLY_MASS_STORAGE_RESET       0xff                          // {2}
#define GET_MAX_LUN                        0xfe

typedef struct _PACK stUSB_MASS_STORAGE_CBW                              // command block wrapper
{
    unsigned char          dCBWSignature[4];                             // signature is 0x43425355 "USBC"
    unsigned char          dCBWTag[4];                                   // tag associating this CBW with the CSW response
    unsigned char          dCBWDataTransferLength[4];                    // the number of bytes the host expect to transfer in the data-transfer stage
    unsigned char          dmCBWFlags;                                   // specifies the direction of the data-transfer stage
    unsigned char          dCBWLUN;                                      // LUN that the command block is directed to (4 bits valid)
    unsigned char          dCBWCBLength;                                 // the length of the command block in bytes (1..16)
    unsigned char          CBWCB[16];                                    // the command block for the device to execute
} USB_MASS_STORAGE_CBW;

typedef struct _PACK stUSB_MASS_STORAGE_CBW_LW                           // command block wrapper with long word content
{
    unsigned long          dCBWSignatureL;                               // signature is 0x43425355 "USBC"
    unsigned long          dCBWTagL;                                     // tag associating this CBW with the CSW response
    unsigned long          dCBWDataTransferLengthL;                      // the number of bytes the host expect to transfer in the data-transfer stage
    unsigned char          dmCBWFlags;                                   // specifies the direction of the data-transfer stage
    unsigned char          dCBWLUN;                                      // LUN that the command block is directed to (4 bits valid)
    unsigned char          dCBWCBLength;                                 // the length of the command block in bytes (1..16)
    unsigned char          CBWCB[16];                                    // the command block for the device to execute
} USB_MASS_STORAGE_CBW_LW;

#define CBW_IN_FLAG              0x80

#define CBW_OperationCode        0

#define UFI_FORMAT_UNIT          0x04
#define UFI_INQUIRY              0x12
#define UFI_START_STOP           0x1b
#define UFI_MODE_SENSE_6         0x1a
#define UFI_MODE_SELECT          0x55
#define UFI_MODE_SENSE           0x5a
#define UFI_PRE_ALLOW_REMOVAL    0x1e
#define UFI_READ_10              0x28
#define UFI_READ_12              0xa8
#define UFI_READ_CAPACITY        0x25
#define UFI_READ_FORMAT_CAPACITY 0x23
#define UFI_REQUEST_SENSE        0x03
#define UFI_REZERO_UNIT          0x01
#define UFI_SEEK_10              0x2b
#define UFI_SEND_DIAGNOSTIC      0x1d
#define UFI_TEST_UNIT_READY      0x00
#define UFI_VERIFY               0x2f
#define UFI_WRITE_10             0x2a
#define UFI_WRITE_12             0xaa
#define UFI_WRITE_AND_VERIFY     0x2e

typedef struct _PACK stCBW_TEST_UNIT_READY                               // 0x00
{
    unsigned char          ucOperationCode;
    unsigned char          ucLogicalUnitNumber_EVPD;
} CBW_TEST_UNIT_READY;

typedef struct _PACK stCBW_REQUEST_SENSE_DATA                            // 0x03
{
    unsigned char          ucOperationCode;
    unsigned char          ucLogicalUnitNumber;
    unsigned char          ucRes0[2];
    unsigned char          ucAllocationLength;
} CBW_REQUEST_SENSE_DATA;

typedef struct _PACK stCBW_RETURN_SENSE_DATA
{
    unsigned char          ucValid_ErrorCode;
    unsigned char          ucRes0;
    unsigned char          ucSenseKey;
    unsigned char          ucInformation[4];
    unsigned char          ucAdditionalSenseLength_10;
    unsigned char          ucRes1[4];
    unsigned char          ucAdditionalSenseCode;
    unsigned char          ucAdditionalSenseCodeQualifier;
    unsigned char          ucRes2[4];
} CBW_RETURN_SENSE_DATA;

#define VALID_SENSE_DATA         0x80
#define CURRENT_ERRORS           0x70
#define SENSE_LENGTH_10          10

// Sense keys
//
#define SENSE_NO_SENSE           0x00
#define SENSE_RECOVERED_ERROR    0x01
#define SENSE_NOT_READY          0x02
    #define DESC_MEDIUM_NOT_PRESENT  0x3a
#define SENSE_MEDIUM_ERROR       0x03
#define SENSE_HARDWARE_ERROR     0x04
#define SENSE_ILLEGAL_REQUEST    0x05
#define SENSE_UNIT_ATTENTION     0x06
#define SENSE_DATA_PROTECTION    0x07
#define SENSE_BLANK_CHECK        0x08
#define SENSE_VENDOR_SPECIFIC    0x09
#define SENSE_ABORTED_COMMAND    0x0b
#define SENSE_VOLUME_OVERFLOW    0x0d
#define SENSE_MISCOMPARE         0x0e

typedef struct _PACK stCBW_INQUIRY_COMMAND                               // 0x12
{
    unsigned char          ucOperationCode;
    unsigned char          ucLogicalUnitNumber_EVPD;
    unsigned char          ucPageCode;
    unsigned char          ucReserved0;
    unsigned char          ucAllocationLength;
} CBW_INQUIRY_COMMAND;

typedef struct _PACK stCBW_INQUIRY_DATA
{
    unsigned char          ucPeripheralDeviceType;
    unsigned char          ucRMB;
    unsigned char          ucISOversion_ECMAversion_ANSIversion;
    unsigned char          ucResponseDataFormat;
    unsigned char          ucAdditionalLength;
    unsigned char          ucReserved0[3];
    unsigned char          ucVendorInformation[8];
    unsigned char          ucProductIdentification[16];
    unsigned char          ucProductRevisionLevel[4];
} CBW_INQUIRY_DATA;

#define DEVICE_TYPE_FLOPPY           0x00
#define DEVICE_TYPE_NONE             0x1f
#define RMB_REMOVABLE                0x80
#define RMB_FIXED                    0x00
#define RESPONSE_FORMAT_UFI          0x01

typedef struct _PACK stCBW_MODE_SENSE_6                                  // 0x1a {5}
{
    unsigned char          ucOperationCode;
    unsigned char          ucLogicalUnitNumber_DBD;
    unsigned char          ucPC_PageCode;
    unsigned char          ucSubpageCode;
    unsigned char          ucAllocationLength;
    unsigned char          ucControl;
} CBW_MODE_SENSE_6;

#define MODE_SENSE_6_DBD                     0x08                        // don't return block descriptors when set
#define MODE_SENSE_6_PAGE_CONTROL_CURRENT    0x00
#define MODE_SENSE_6_PAGE_CONTROL_CHANGEABLE 0x40
#define MODE_SENSE_6_PAGE_CONTROL_DEFAULT    0x80
#define MODE_SENSE_6_PAGE_CONTROL_SAVED      0xc0
#define MODE_SENSE_6_PAGE_CODE_CACHING       0x08
#define MODE_SENSE_6_PAGE_CODE_ALL_PAGES     0x3f



typedef struct _PACK stMODE_PARAMETER_6
{
    unsigned char          ucModeDataLength;
    unsigned char          ucMediumType;
    unsigned char          ucWP_DPOFUA;
    unsigned char          ucBlockDescriptorLength;
  //BLOCK_DESCRIPTOR       blockDescriptor;                              // there can be a list of descriptors
} MODE_PARAMETER_6;

#define PAR6_WRITE_PROTECTED 0x80
#define PAR6_DPO_FUA         0x10


typedef struct _PACK stCBW_FORMAT_CAPACITIES                             // 0x23
{
    unsigned char          ucOperationCode;
    unsigned char          ucLogicalUnitNumber;
    unsigned char          ucReserved0[5];
    unsigned char          ucAllocationLength[2];
} CBW_FORMAT_CAPACITIES;

typedef struct _PACK stCAPACITY_DESCRIPTOR
{
    unsigned char          ucNumberOfBlocks[4];                          // total number of addressable blocks for the descriptor's media type
    unsigned char          ucDescriptorCode;
    unsigned char          ucBlockLength[3];                             // length in bytes for each logical block
} CAPACITY_DESCRIPTOR;

#define DESC_CODE_UNFORMATTED_MEDIA      0x01
#define DESC_CODE_FORMATTED_MEDIA        0x02
#define DESC_CODE_NO_CARTRIDGE_IN_DRIVE  0x03

typedef struct _PACK stCBW_READ_CAPACITY                                 // 0x25
{
    unsigned char          ucOperationCode;
    unsigned char          ucLogicalUnitNumberRelAdr;
    unsigned char          ucLogicalBlockAddress[4];
    unsigned char          ucReserved0[2];
    unsigned char          ucPMI;
} CBW_READ_CAPACITY;

typedef struct _PACK stCBW_READ_CAPACITY_DATA
{
    unsigned char          ucLastLogicalBlockAddress[4];
    unsigned char          ucBlockLengthInBytes[4];
} CBW_READ_CAPACITY_DATA;

typedef struct _PACK stCBW_READ_10                                       // 0x28
{
    unsigned char          ucOperationCode;
    unsigned char          ucLogicalUnitNumberRelAdr;
    unsigned char          ucLogicalBlockAddress[4];
    unsigned char          ucReserved0;
    unsigned char          ucTransferLength[2];
} CBW_READ_10;

typedef struct _PACK stCBW_READ_12                                       // 0xa8
{
    unsigned char          ucOperationCode;
    unsigned char          ucLogicalUnitNumberRelAdr;
    unsigned char          ucLogicalBlockAddress[4];
    unsigned char          ucTransferLength[4];
} CBW_READ_12;

typedef struct _PACK stCBW_WRITE_10                                      // 0x2a
{
    unsigned char          ucOperationCode;
    unsigned char          ucLogicalUnitNumberRelAdr;
    unsigned char          ucLogicalBlockAddress[4];
    unsigned char          ucReserved0;
    unsigned char          ucTransferLength[2];
} CBW_WRITE_10;

typedef struct _PACK stCBW_WRITE_12                                      // 0xaa
{
    unsigned char          ucOperationCode;
    unsigned char          ucLogicalUnitNumberRelAdr;
    unsigned char          ucLogicalBlockAddress[4];
    unsigned char          ucTransferLength[4];
} CBW_WRITE_12;

typedef struct _PACK stCBW_START_STOP_UNIT                               // 0x1b
{
    unsigned char          ucOperationCode;
    unsigned char          ucImmediate;
    unsigned char          ucReserved0;
    unsigned char          ucPowerConditionModifier;
    unsigned char          ucPowerCondition;
    unsigned char          ucControl;
} CBW_START_STOP_UNIT;

#define START_STOP_UNIT_POWER_CONDITION             0xf0
#define START_STOP_UNIT_POWER_CONDITION_START_BIT   0x01
#define START_STOP_UNIT_POWER_CONDITION_LOEJ        0x02
#define START_STOP_UNIT_POWER_CONDITION_START_VALID 0x00                 // process the start and LOEJ bits

typedef struct _PACK stCBW_CAPACITY_LIST
{
    unsigned char          ucReserved0[3];
    unsigned char          ucCapacityListLength;
    CAPACITY_DESCRIPTOR    capacityDescriptor;                           // there can be a list of descriptors
} CBW_CAPACITY_LIST;



typedef struct _PACK stUSB_MASS_STORAGE_CSW                              // command status wrapper
{
    unsigned char          dCBWSignature[4];                             // signature is 0x53425355 "USBS"
    unsigned char          dCBWTag[4];                                   // tag originally received from the host
    unsigned char          dCSWDataResidue[4];                           // the difference in the number of bytes processed or the difference between the requested and number of bytes sent
    unsigned char          bCSWStatus;                                   // status
} USB_MASS_STORAGE_CSW;

#define CSW_STATUS_COMMAND_PASSED    0x00
#define CSW_STATUS_COMMAND_FAILED    0x01
#define CSW_STATUS_PHASE_ERROR       0x02


// Host                                                                  {1}
//
typedef struct _PACK stUSB_HOST_DESCRIPTOR
{
    unsigned char          bmRecipient_device_direction;                 // descriptor size in bytes
    unsigned char          bRequest;                                     // request type
    unsigned char          wValue[2];                                    // 
    unsigned char          wIndex[2];                                    // 
    unsigned char          wLength[2];                                   // length that can be accepted
} USB_HOST_DESCRIPTOR;
__PACK_OFF
#if defined _CODE_WARRIOR_CF                                             // ensure alignment continues outside of this file
    #pragma pack(0)
#endif

// USB messages
//
#define E_USB_ACTIVATE_CONFIGURATION      0xfe                           // device mode
#define E_USB_SET_INTERFACE               0xfd
#define E_USB_DEVICE_INFO                 0xfc                           // host mode
#define E_USB_DEVICE_STALLED              0xfb
#define E_USB_DEVICE_CLEARED              0xfa
#define E_USB_HOST_ERROR_REPORT           0xf9
#endif
