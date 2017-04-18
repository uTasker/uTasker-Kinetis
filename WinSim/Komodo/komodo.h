/*=========================================================================
| Komodo Interface Library
|--------------------------------------------------------------------------
| Copyright (c) 2011 Total Phase, Inc.
| All rights reserved.
| www.totalphase.com
|
| Redistribution and use in source and binary forms, with or without
| modification, are permitted provided that the following conditions
| are met:
|
| - Redistributions of source code must retain the above copyright
|   notice, this list of conditions and the following disclaimer.
|
| - Redistributions in binary form must reproduce the above copyright
|   notice, this list of conditions and the following disclaimer in the
|   documentation and/or other materials provided with the distribution.
|
| - Neither the name of Total Phase, Inc. nor the names of its
|   contributors may be used to endorse or promote products derived from
|   this software without specific prior written permission.
|
| THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
| "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
| LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
| FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE
| COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
| INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
| BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
| LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
| CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
| LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
| ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
| POSSIBILITY OF SUCH DAMAGE.
|--------------------------------------------------------------------------
| To access Komodo devices through the API:
|
| 1) Use one of the following shared objects:
|      komodo.so      --  Linux shared object
|      komodo.dll     --  Windows dynamic link library
|
| 2) Along with one of the following language modules:
|      komodo.c/h     --  C/C++ API header file and interface module
|      komodo_py.py   --  Python API
|      komodo.bas     --  Visual Basic 6 API
|      komodo.cs      --  C# .NET source
|      komodo_net.dll --  Compiled .NET binding
 ========================================================================*/


#ifndef __komodo_h__
#define __komodo_h__

#ifdef __cplusplus
extern "C" {
#endif


/*=========================================================================
| TYPEDEFS
 ========================================================================*/
#ifndef TOTALPHASE_DATA_TYPES
#define TOTALPHASE_DATA_TYPES

#ifndef _MSC_VER
/* C99-compliant compilers (GCC) */
#include <stdint.h>
typedef uint8_t   u08;
typedef uint16_t  u16;
typedef uint32_t  u32;
typedef uint64_t  u64;
typedef int8_t    s08;
typedef int16_t   s16;
typedef int32_t   s32;
typedef int64_t   s64;

#else
/* Microsoft compilers (Visual C++) */
typedef unsigned __int8   u08;
typedef unsigned __int16  u16;
typedef unsigned __int32  u32;
typedef unsigned __int64  u64;
typedef signed   __int8   s08;
typedef signed   __int16  s16;
typedef signed   __int32  s32;
typedef signed   __int64  s64;

#endif /* __MSC_VER */

#endif /* TOTALPHASE_DATA_TYPES */


/*=========================================================================
| DEBUG
 ========================================================================*/
/* Set the following macro to '1' for debugging */
#define KM_DEBUG 0


/*=========================================================================
| VERSION
 ========================================================================*/
#define KM_HEADER_VERSION  0x010a   /* v1.10 */


/*=========================================================================
| STATUS CODES
 ========================================================================*/
/*
 * All API functions return an integer which is the result of the
 * transaction, or a status code if negative.  The status codes are
 * defined as follows:
 */
enum km_status_t {
    /* General codes (0 to -99) */
    KM_OK                      =    0,
    KM_UNABLE_TO_LOAD_LIBRARY  =   -1,
    KM_UNABLE_TO_LOAD_DRIVER   =   -2,
    KM_UNABLE_TO_LOAD_FUNCTION =   -3,
    KM_INCOMPATIBLE_LIBRARY    =   -4,
    KM_INCOMPATIBLE_DEVICE     =   -5,
    KM_COMMUNICATION_ERROR     =   -6,
    KM_UNABLE_TO_OPEN          =   -7,
    KM_UNABLE_TO_CLOSE         =   -8,
    KM_INVALID_HANDLE          =   -9,
    KM_CONFIG_ERROR            =  -10,
    KM_PARAM_ERROR             =  -11,
    KM_FUNCTION_NOT_AVAILABLE  =  -12,
    KM_FEATURE_NOT_ACQUIRED    =  -13,
    KM_NOT_DISABLED            =  -14,
    KM_NOT_ENABLED             =  -15,

    /* CAN codes (-100 to -199) */
    KM_CAN_READ_EMPTY          = -101,
    KM_CAN_SEND_TIMEOUT        = -102,
    KM_CAN_SEND_FAIL           = -103,
    KM_CAN_ASYNC_EMPTY         = -104,
    KM_CAN_ASYNC_MAX_REACHED   = -105,
    KM_CAN_ASYNC_PENDING       = -106,
    KM_CAN_ASYNC_TIMEOUT       = -107,
    KM_CAN_AUTO_BITRATE_FAIL   = -108
};
#ifndef __cplusplus
typedef enum km_status_t km_status_t;
#endif


/*=========================================================================
| GENERAL TYPE DEFINITIONS
 ========================================================================*/
/* Komodo handle type definition */
typedef int Komodo;

/*
 * Komodo version matrix.
 *
 * This matrix describes the various version dependencies
 * of Komodo components.  It can be used to determine
 * which component caused an incompatibility error.
 *
 * All version numbers are of the format:
 *   (major << 8) | minor
 *
 * ex. v1.20 would be encoded as:  0x0114
 */
struct KomodoVersion {
    /* Software, firmware, and hardware versions. */
    u16 software;
    u16 firmware;
    u16 hardware;

    /*
     * Firmware revisions that are compatible with this software version.
     * The top 16 bits gives the maximum accepted fw revision.
     * The lower 16 bits gives the minimum accepted fw revision.
     */
    u32 fw_revs_for_sw;

    /*
     * Hardware revisions that are compatible with this software version.
     * The top 16 bits gives the maximum accepted hw revision.
     * The lower 16 bits gives the minimum accepted hw revision.
     */
    u32 hw_revs_for_sw;

    /* Software requires that the API interface must be >= this version. */
    u16 api_req_by_sw;
};
#ifndef __cplusplus
typedef struct KomodoVersion KomodoVersion;
#endif

/*
 * Komodo feature set
 *
 * This bitmask field describes the features available on this device.
 *
 * When returned by km_features() or km_open_ext(), it refers to the
 * potential features of the device.
 * When used as a parameter by km_enable() or km_disable(), it refers
 * to the features that the user wants to use.
 * And when returned by km_disable(), it refers to the features currently
 * in use by the user.
 */
#define KM_FEATURE_GPIO_LISTEN 0x00000001
#define KM_FEATURE_GPIO_CONTROL 0x00000002
#define KM_FEATURE_GPIO_CONFIG 0x00000004
#define KM_FEATURE_CAN_A_LISTEN 0x00000008
#define KM_FEATURE_CAN_A_CONTROL 0x00000010
#define KM_FEATURE_CAN_A_CONFIG 0x00000020
#define KM_FEATURE_CAN_B_LISTEN 0x00000040
#define KM_FEATURE_CAN_B_CONTROL 0x00000080
#define KM_FEATURE_CAN_B_CONFIG 0x00000100

/*=========================================================================
| GENERAL API
 ========================================================================*/
/*
 * Get a list of ports to which Komodo devices are attached.
 *
 * nelem   = maximum number of elements to return
 * devices = array into which the port numbers are returned
 *
 * Each element of the array is written with the port number.
 * Devices that are in-use are ORed with KM_PORT_NOT_FREE (0x8000).
 *
 * ex.  devices are attached to ports 0, 1, 2
 *      ports 0 and 2 are available, and port 1 is in-use.
 *      array => 0x0000, 0x8001, 0x0002
 *
 * If the array is NULL, it is not filled with any values.
 * If there are more devices than the array size, only the
 * first nmemb port numbers will be written into the array.
 *
 * Returns the number of devices found, regardless of the
 * array size.
 */
#define KM_PORT_NOT_FREE 0x8000
#define KM_PORT_NUM_MASK 0x00ff
int km_find_devices (
    int   num_ports,
    u16 * ports
);


/*
 * Get a list of ports to which Komodo devices are attached.
 *
 * This function is the same as km_find_devices() except that
 * it returns the unique IDs of each Komodo device.  The IDs
 * are guaranteed to be non-zero if valid.
 *
 * The IDs are the unsigned integer representation of the 10-digit
 * serial numbers.
 */
int km_find_devices_ext (
    int   num_ports,
    u16 * ports,
    int   num_ids,
    u32 * unique_ids
);


/*
 * Open the Komodo port.
 *
 * The port number is a zero-indexed integer.
 *
 * The port number is the same as that obtained from the
 * km_find_devices() function above.
 *
 * Returns an Komodo handle, which is guaranteed to be
 * greater than zero if it is valid.
 *
 * This function is recommended for use in simple applications
 * where extended information is not required.  For more complex
 * applications, the use of km_open_ext() is recommended.
 */
Komodo km_open (
    int port_number
);


/*
 * Open the Komodo port, returning extended information
 * in the supplied structure.  Behavior is otherwise identical
 * to km_open() above.  If 0 is passed as the pointer to the
 * structure, this function is exactly equivalent to km_open().
 *
 * The structure is zeroed before the open is attempted.
 * It is filled with whatever information is available.
 *
 * For example, if the firmware version is not filled, then
 * the device could not be queried for its version number.
 *
 * The feature list is a bitmap of Komodo resources, with the same
 * mapping as obtained from the km_features() function below.
 * Details on the bitmask are found above.
 *
 * This function is recommended for use in complex applications
 * where extended information is required.  For more simple
 * applications, the use of km_open() is recommended.
 */
struct KomodoExt {
    /* Version matrix */
    KomodoVersion   version;

    /* Features of this device. */
    u32             features;
};
#ifndef __cplusplus
typedef struct KomodoExt KomodoExt;
#endif

Komodo km_open_ext (
    int         port_number,
    KomodoExt * km_ext
);


/* Close the Komodo port. */
int km_close (
    Komodo komodo
);


/*
 * Return the port for this Komodo handle.
 *
 * The port number is a zero-indexed integer identical to those
 * returned by km_find_devices() above.  This includes the count of
 * interfaces in use in the upper byte.
 */
int km_port (
    Komodo komodo
);


/*
 * Return the device features as a bit-mask of values, or an error code
 * if the handle is not valid.  Details on the bitmask are found above.
 */
int km_features (
    Komodo komodo
);


/*
 * Return the unique ID for this Komodo adapter.
 * IDs are guaranteed to be non-zero if valid.
 * The ID is the unsigned integer representation of the
 * 10-digit serial number.
 */
u32 km_unique_id (
    Komodo komodo
);


/*
 * Return the status string for the given status code.
 * If the code is not valid or the library function cannot
 * be loaded, return a NULL string.
 */
const char * km_status_string (
    int status
);


/*
 * Return the version matrix for the device attached to the
 * given handle.  If the handle is 0 or invalid, only the
 * software and required api versions are set.
 */
int km_version (
    Komodo          komodo,
    KomodoVersion * version
);


/*
 * Sleep for the specified number of milliseconds.
 * Accuracy depends on the operating system scheduler.
 * Returns the number of milliseconds slept.
 */
u32 km_sleep_ms (
    u32 milliseconds
);


/*
 * Acquire device features.
 * Returns the features that are currently acquired.
 */
int km_acquire (
    Komodo komodo,
    u32    features
);


/*
 * Release device features.
 * Returns the features that are still acquired.
 */
int km_release (
    Komodo komodo,
    u32    features
);



/*=========================================================================
| CAN API
 ==========================================================================
| These special timeout constants can be used with the functions
| km_timeout and km_can_async_collec*/
#define KM_TIMEOUT_IMMEDIATE 0
#define KM_TIMEOUT_INFINITE -1
/*
 * Set the timeout of the km_can_read function to the specified
 * number of milliseconds.
 */
int km_timeout (
    Komodo komodo,
    u32    timeout_ms
);


/* Standard enumeration for the CAN channels available on the Komodo. */
enum km_can_ch_t {
    KM_CAN_CH_A = 0,
    KM_CAN_CH_B = 1
};
#ifndef __cplusplus
typedef enum km_can_ch_t km_can_ch_t;
#endif

/* CAN Bus state constants. */
#define KM_CAN_BUS_STATE_LISTEN_ONLY 0x00000001
#define KM_CAN_BUS_STATE_CONTROL 0x00000002
#define KM_CAN_BUS_STATE_WARNING 0x00000004
#define KM_CAN_BUS_STATE_ACTIVE 0x00000008
#define KM_CAN_BUS_STATE_PASSIVE 0x00000010
#define KM_CAN_BUS_STATE_OFF 0x00000020
/* Retreive the current bus state of the supplied CAN channel */
int km_can_query_bus_state (
    Komodo      komodo,
    km_can_ch_t channel,
    u08 *       bus_state,
    u08 *       rx_error,
    u08 *       tx_error
);


/*
 * Set the capture latency to the specified number of milliseconds.
 * This number determines the minimum time that a read call will
 * block if there is no available data.  Lower times result in
 * faster turnaround at the expense of reduced buffering.  Setting
 * this parameter too low can cause packets to be dropped.
 */
int km_latency (
    Komodo komodo,
    u32    latency_ms
);


/* Config mask for km_can_configure */
#define KM_CAN_CONFIG_NONE 0x00000000
#define KM_CAN_CONFIG_LISTEN_SELF 0x00000001
int km_can_configure (
    Komodo komodo,
    u32    config
);


/*
 * Set the bus timeout.  If a zero is passed as the timeout,
 * the timeout is unchanged and the current timeout is returned.
 */
int km_can_bus_timeout (
    Komodo      komodo,
    km_can_ch_t channel,
    u16         timeout_ms
);


/*
 * Set the CAN bit rate in hertz on the given channel.  If a zero is
 * passed as the bitrate, the bitrate is unchanged.  In all cases, the
 * call will return the bitrate that will be in effect.
 */
#define KM_KHZ 1000
#define KM_MHZ 1000000
int km_can_bitrate (
    Komodo      komodo,
    km_can_ch_t channel,
    u32         bitrate_hz
);


int km_can_auto_bitrate (
    Komodo      komodo,
    km_can_ch_t channel
);


int km_can_auto_bitrate_ext (
    Komodo      komodo,
    km_can_ch_t channel,
    u32         num_bitrates_hz,
    const u32 * bitrates_hz
);


/* Get the sample rate in kilohertz. */
int km_get_samplerate (
    Komodo komodo
);


/* Configure the target power.  Returns power status or error code. */
enum km_power_t {
    KM_TARGET_POWER_QUERY = 0x02,
    KM_TARGET_POWER_OFF   = 0x00,
    KM_TARGET_POWER_ON    = 0x01
};
#ifndef __cplusplus
typedef enum km_power_t km_power_t;
#endif

/* Set the target power for the specified CAN channel. */
int km_can_target_power (
    Komodo      komodo,
    km_can_ch_t channel,
    km_power_t  power
);


/* Enable the Komodo. */
int km_enable (
    Komodo komodo
);


/* Disable the Komodo. */
int km_disable (
    Komodo komodo
);


/* Status mask for km_can_read */
#define KM_READ_TIMEOUT 0x80000000
#define KM_READ_ERR_OVERFLOW 0x40000000
#define KM_READ_END_OF_CAPTURE 0x20000000
#define KM_READ_CAN_ERR 0x00000100
#define KM_READ_CAN_ERR_FULL_MASK 0x000000ff
#define KM_READ_CAN_ERR_POS_MASK 0x0000001f
#define KM_READ_CAN_ERR_POS_SOF 0x00000003
#define KM_READ_CAN_ERR_POS_ID28_21 0x00000002
#define KM_READ_CAN_ERR_POS_ID20_18 0x00000006
#define KM_READ_CAN_ERR_POS_SRTR 0x00000004
#define KM_READ_CAN_ERR_POS_IDE 0x00000005
#define KM_READ_CAN_ERR_POS_ID17_13 0x00000007
#define KM_READ_CAN_ERR_POS_ID12_5 0x0000000f
#define KM_READ_CAN_ERR_POS_ID4_0 0x0000000e
#define KM_READ_CAN_ERR_POS_RTR 0x0000000c
#define KM_READ_CAN_ERR_POS_RSVD_1 0x0000000d
#define KM_READ_CAN_ERR_POS_RSVD_0 0x00000009
#define KM_READ_CAN_ERR_POS_DLC 0x0000000b
#define KM_READ_CAN_ERR_POS_DF 0x0000000a
#define KM_READ_CAN_ERR_POS_CRC_SEQ 0x00000008
#define KM_READ_CAN_ERR_POS_CRC_DEL 0x00000018
#define KM_READ_CAN_ERR_POS_ACK_SLOT 0x00000019
#define KM_READ_CAN_ERR_POS_ACK_DEL 0x0000001b
#define KM_READ_CAN_ERR_POS_EOF 0x0000001a
#define KM_READ_CAN_ERR_POS_INTRMSN 0x00000012
#define KM_READ_CAN_ERR_POS_AEF 0x00000011
#define KM_READ_CAN_ERR_POS_PEF 0x00000016
#define KM_READ_CAN_ERR_POS_TDB 0x00000013
#define KM_READ_CAN_ERR_POS_ERR_DEL 0x00000017
#define KM_READ_CAN_ERR_POS_OVRFLG 0x0000001c
#define KM_READ_CAN_ERR_DIR_MASK 0x00000020
#define KM_READ_CAN_ERR_DIR_TX 0x00000000
#define KM_READ_CAN_ERR_DIR_RX 0x00000020
#define KM_READ_CAN_ERR_TYPE_MASK 0x000000c0
#define KM_READ_CAN_ERR_TYPE_BIT 0x00000000
#define KM_READ_CAN_ERR_TYPE_FORM 0x00000040
#define KM_READ_CAN_ERR_TYPE_STUFF 0x00000080
#define KM_READ_CAN_ERR_TYPE_OTHER 0x000000c0
#define KM_READ_CAN_ARB_LOST 0x00000200
#define KM_READ_CAN_ARB_LOST_POS_MASK 0x000000ff
/* GPIO Configuration */
#define KM_GPIO_PIN_1_CONFIG 0x00
#define KM_GPIO_PIN_2_CONFIG 0x01
#define KM_GPIO_PIN_3_CONFIG 0x02
#define KM_GPIO_PIN_4_CONFIG 0x03
#define KM_GPIO_PIN_5_CONFIG 0x04
#define KM_GPIO_PIN_6_CONFIG 0x05
#define KM_GPIO_PIN_7_CONFIG 0x06
#define KM_GPIO_PIN_8_CONFIG 0x07
/* GPIO Mask */
#define KM_GPIO_PIN_1_MASK 0x01
#define KM_GPIO_PIN_2_MASK 0x02
#define KM_GPIO_PIN_3_MASK 0x04
#define KM_GPIO_PIN_4_MASK 0x08
#define KM_GPIO_PIN_5_MASK 0x10
#define KM_GPIO_PIN_6_MASK 0x20
#define KM_GPIO_PIN_7_MASK 0x40
#define KM_GPIO_PIN_8_MASK 0x80
/* Event mask for km_can_read */
#define KM_EVENT_DIGITAL_INPUT 0x00000100
#define KM_EVENT_DIGITAL_INPUT_MASK 0x000000ff
#define KM_EVENT_DIGITAL_INPUT_1 0x00000001
#define KM_EVENT_DIGITAL_INPUT_2 0x00000002
#define KM_EVENT_DIGITAL_INPUT_3 0x00000004
#define KM_EVENT_DIGITAL_INPUT_4 0x00000008
#define KM_EVENT_DIGITAL_INPUT_5 0x00000010
#define KM_EVENT_DIGITAL_INPUT_6 0x00000020
#define KM_EVENT_DIGITAL_INPUT_7 0x00000040
#define KM_EVENT_DIGITAL_INPUT_8 0x00000080
#define KM_EVENT_CAN_BUS_STATE_LISTEN_ONLY 0x00001000
#define KM_EVENT_CAN_BUS_STATE_CONTROL 0x00002000
#define KM_EVENT_CAN_BUS_STATE_WARNING 0x00004000
#define KM_EVENT_CAN_BUS_STATE_ACTIVE 0x00008000
#define KM_EVENT_CAN_BUS_STATE_PASSIVE 0x00010000
#define KM_EVENT_CAN_BUS_STATE_OFF 0x00020000
#define KM_EVENT_CAN_BUS_BITRATE 0x00040000
struct km_can_info_t {
    u64         timestamp;
    u32         status;
    u32         events;
    km_can_ch_t channel;
    u32         bitrate_hz;
    u08         host_gen;
    u08         rx_error_count;
    u08         tx_error_count;
    u32         overflow_count;
};
#ifndef __cplusplus
typedef struct km_can_info_t km_can_info_t;
#endif

struct km_can_packet_t {
    u08 remote_req;
    u08 extend_addr;
    u08 dlc;
    u32 id;
};
#ifndef __cplusplus
typedef struct km_can_packet_t km_can_packet_t;
#endif

/*
 * Read a single CAN packet from the Komodo data stream.
 * This will block for timeout_ms milliseconds; 0 will return
 * immediately, and MAXINT will block indefinitely.
 * timestamp is in units of nanoseconds.
 */
int km_can_read (
    Komodo            komodo,
    km_can_info_t *   info,
    km_can_packet_t * pkt,
    int               num_bytes,
    u08 *             data
);


/* Flags mask */
#define KM_CAN_ONE_SHOT 0x01
/* Submit a CAN packet to the Komodo data stream, asynchronously. */
int km_can_async_submit (
    Komodo                  komodo,
    km_can_ch_t             channel,
    u08                     flags,
    const km_can_packet_t * pkt,
    int                     num_bytes,
    const u08 *             data
);


/*
 * Collect a response to a CAN packet submitted to the Komodo data
 * stream, asynchronously.
 */
int km_can_async_collect (
    Komodo komodo,
    u32    timeout_ms,
    u32 *  arbitration_count
);


/*
 * Write a stream of bytes to the CAN slave device.  The return
 * value of the function is a status code.
 */
int km_can_write (
    Komodo                  komodo,
    km_can_ch_t             channel,
    u08                     flags,
    const km_can_packet_t * pkt,
    int                     num_bytes,
    const u08 *             data,
    u32 *                   arbitration_count
);



/*=========================================================================
| GPIO API
 ==========================================================================
| Enumeration of input GPIO pin bias configuration*/
enum km_pin_bias_t {
    KM_PIN_BIAS_TRISTATE = 0x00,
    KM_PIN_BIAS_PULLUP   = 0x01,
    KM_PIN_BIAS_PULLDOWN = 0x02
};
#ifndef __cplusplus
typedef enum km_pin_bias_t km_pin_bias_t;
#endif

/* Enumeration of input GPIO pin trigger edge condition. */
enum km_pin_trigger_t {
    KM_PIN_TRIGGER_NONE         = 0x00,
    KM_PIN_TRIGGER_RISING_EDGE  = 0x01,
    KM_PIN_TRIGGER_FALLING_EDGE = 0x02,
    KM_PIN_TRIGGER_BOTH_EDGES   = 0x03
};
#ifndef __cplusplus
typedef enum km_pin_trigger_t km_pin_trigger_t;
#endif

/*
 * Configure a GPIO pin to act as an input.  The return value
 * of the function is a status code
 */
int km_gpio_config_in (
    Komodo komodo,
    u08    pin_number,
    u08    bias,
    u08    trigger
);


/* Enumeration of output GPIO pin driver configurations. */
enum km_pin_drive_t {
    KM_PIN_DRIVE_NORMAL            = 0x00,
    KM_PIN_DRIVE_INVERTED          = 0x01,
    KM_PIN_DRIVE_OPEN_DRAIN        = 0x02,
    KM_PIN_DRIVE_OPEN_DRAIN_PULLUP = 0x03
};
#ifndef __cplusplus
typedef enum km_pin_drive_t km_pin_drive_t;
#endif

/* Enumeration of output GPIO pin sources. */
enum km_pin_source_t {
    KM_PIN_SRC_SOFTWARE_CTL       = 0x00,
    KM_PIN_SRC_ALL_ERR_CAN_A      = 0x11,
    KM_PIN_SRC_BIT_ERR_CAN_A      = 0x12,
    KM_PIN_SRC_FORM_ERR_CAN_A     = 0x13,
    KM_PIN_SRC_STUFF_ERR_CAN_A    = 0x14,
    KM_PIN_SRC_OTHER_ERR_CAN_A    = 0x15,
    KM_PIN_SRC_ALL_ERR_CAN_B      = 0x21,
    KM_PIN_SRC_BIT_ERR_CAN_B      = 0x22,
    KM_PIN_SRC_FORM_ERR_CAN_B     = 0x23,
    KM_PIN_SRC_STUFF_ERR_CAN_B    = 0x24,
    KM_PIN_SRC_OTHER_ERR_CAN_B    = 0x25,
    KM_PIN_SRC_ALL_ERR_CAN_BOTH   = 0x31,
    KM_PIN_SRC_BIT_ERR_CAN_BOTH   = 0x32,
    KM_PIN_SRC_FORM_ERR_CAN_BOTH  = 0x33,
    KM_PIN_SRC_STUFF_ERR_CAN_BOTH = 0x34,
    KM_PIN_SRC_OTHER_ERR_CAN_BOTH = 0x35
};
#ifndef __cplusplus
typedef enum km_pin_source_t km_pin_source_t;
#endif

/*
 * Configure a GPIO pin to act as an output. The return value
 * of the function is a status code.
 */
int km_gpio_config_out (
    Komodo komodo,
    u08    pin_number,
    u08    drive,
    u08    source
);


/*
 * Set the value of any GPIO pins configured as software controlled
 * outputs. The return value of the function is a status code.
 */
int km_gpio_set (
    Komodo komodo,
    u08    value,
    u08    mask
);


/* Returns the current values of all GPIO pins. */
int km_gpio_get (
    Komodo komodo
);




#ifdef __cplusplus
}
#endif

#endif  /* __komodo_h__ */
