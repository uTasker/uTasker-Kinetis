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


/*=========================================================================
| INCLUDES
 ========================================================================*/
/* This #include can be customized to conform to the user's build paths. */
#include "komodo.h"


/*=========================================================================
| VERSION CHECK
 ========================================================================*/
#define KM_CFILE_VERSION   0x010a   /* v1.10 */
#define KM_REQ_SW_VERSION  0x010a   /* v1.10 */

/*
 * Make sure that the header file was included and that
 * the version numbers match.
 */
#ifndef KM_HEADER_VERSION
#  error Unable to include header file. Please check include path.

#elif KM_HEADER_VERSION != KM_CFILE_VERSION
#  error Version mismatch between source and header files.

#endif


/*=========================================================================
| DEFINES
 ========================================================================*/
#define API_NAME                     "..\\..\\..\\WinSim\\Komodo\\komodo"
#define API_DEBUG                    KM_DEBUG
#define API_OK                       KM_OK
#define API_UNABLE_TO_LOAD_LIBRARY   KM_UNABLE_TO_LOAD_LIBRARY
#define API_INCOMPATIBLE_LIBRARY     KM_INCOMPATIBLE_LIBRARY
#define API_UNABLE_TO_LOAD_FUNCTION  KM_UNABLE_TO_LOAD_FUNCTION
#define API_HEADER_VERSION           KM_HEADER_VERSION
#define API_REQ_SW_VERSION           KM_REQ_SW_VERSION


/*=========================================================================
| LINUX AND DARWIN SUPPORT
 ========================================================================*/
#if defined(__APPLE_CC__) && !defined(DARWIN)
#define DARWIN
#endif

#if defined(linux) || defined(DARWIN)

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>

#ifdef DARWIN
#define DLOPEN_NO_WARN
extern int _NSGetExecutablePath (char *buf, unsigned long *bufsize);
#endif

#include <dlfcn.h>

#define DLL_HANDLE  void *
#define MAX_SO_PATH 256

static char SO_NAME[MAX_SO_PATH+1] = API_NAME ".so";

/*
 * These functions allow the Linux behavior to emulate
 * the Windows behavior as specified below in the Windows
 * support section.
 * 
 * First, search for the shared object in the application
 * binary path, then in the current working directory.
 * 
 * Searching the application binary path requires /proc
 * filesystem support, which is standard in 2.4.x kernels.
 * 
 * If the /proc filesystem is not present, the shared object
 * will not be loaded from the execution path unless that path
 * is either the current working directory or explicitly
 * specified in LD_LIBRARY_PATH.
 */
static int _checkPath (const char *path) {
    char *filename = (char *)malloc(strlen(path) +1 + strlen(SO_NAME) +1);
    int   fd;

    // Check if the file is readable
    sprintf(filename, "%s/%s", path, SO_NAME);
    fd = open(filename, O_RDONLY);
    if (fd >= 0) {
        strncpy(SO_NAME, filename, MAX_SO_PATH);
        close(fd);
    }

    // Clean up and exit
    free(filename);
    return (fd >= 0);
}

static int _getExecPath (char *path, unsigned long maxlen) {
#ifdef linux
    return readlink("/proc/self/exe", path, maxlen);
#endif

#ifdef DARWIN
    _NSGetExecutablePath(path, &maxlen);
    return maxlen;
#endif
}

static void _setSearchPath () {
    char  path[MAX_SO_PATH+1];
    int   count;
    char *p;

    /* Make sure that SO_NAME is not an absolute path. */
    if (SO_NAME[0] == '/')  return;

    /* Check the execution directory name. */
    memset(path, 0, sizeof(path));
    count = _getExecPath(path, MAX_SO_PATH);

    if (count > 0) {
        char *p = strrchr(path, '/');
        if (p == path)  ++p;
        if (p != 0)     *p = '\0';

        /* If there is a match, return immediately. */
        if (_checkPath(path))  return;
    }

    /* Check the current working directory. */
    p = getcwd(path, MAX_SO_PATH);
    if (p != 0)  _checkPath(path);
}

#endif


/*=========================================================================
| WINDOWS SUPPORT
 ========================================================================*/
#if defined(WIN32) || defined(_WIN32)

#include <stdio.h>
#include <windows.h>

#define DLL_HANDLE           HINSTANCE
#define dlopen(name, flags)  LoadLibraryA(name)
#define dlsym(handle, name)  GetProcAddress(handle, name)
#define dlerror()            "Exiting program"
#define SO_NAME              API_NAME ".dll"

/*
 * Use the default Windows DLL loading rules:
 *   1.  The directory from which the application binary was loaded.
 *   2.  The application's current directory.
 *   3a. [Windows NT/2000/XP only] 32-bit system directory
 *       (default: c:\winnt\System32)
 *   3b. 16-bit system directory
 *       (default: c:\winnt\System or c:\windows\system)
 *   4.  The windows directory
 *       (default: c:\winnt or c:\windows)
 *   5.  The directories listed in the PATH environment variable
 */
static void _setSearchPath () {
    /* Do nothing */
}

#endif


/*=========================================================================
| SHARED LIBRARY LOADER
 ========================================================================*/
/* The error conditions can be customized depending on the application. */
static void *_loadFunction (const char *name, int *result) {
    static DLL_HANDLE handle = 0;
    void * function = 0;

    /* Load the shared library if necessary */
    if (handle == 0) {
        u32 (*version) (void);
        u16 sw_version;
        u16 api_version_req;

        _setSearchPath();
        handle = dlopen(SO_NAME, RTLD_LAZY);
        if (handle == 0) {
#if API_DEBUG
            fprintf(stderr, "Unable to load %s\n", SO_NAME);
            fprintf(stderr, "%s\n", dlerror());
#endif
            *result = API_UNABLE_TO_LOAD_LIBRARY;
            return 0;
        }

        version = (void *)dlsym(handle, "km_c_version");
        if (version == 0) {
#if API_DEBUG
            fprintf(stderr, "Unable to bind km_c_version() in %s\n",
                    SO_NAME);
            fprintf(stderr, "%s\n", dlerror());
#endif
            handle  = 0;
            *result = API_INCOMPATIBLE_LIBRARY;
            return 0;
        }

        sw_version      = (u16)((version() >>  0) & 0xffff);
        api_version_req = (u16)((version() >> 16) & 0xffff);
        if (sw_version  < API_REQ_SW_VERSION ||
            API_HEADER_VERSION < api_version_req)
        {
#if API_DEBUG
            fprintf(stderr, "\nIncompatible versions:\n");

            fprintf(stderr, "  Header version  = v%d.%02d  ",
                    (API_HEADER_VERSION >> 8) & 0xff, API_HEADER_VERSION & 0xff);

            if (sw_version < API_REQ_SW_VERSION)
                fprintf(stderr, "(requires library >= %d.%02d)\n",
                        (API_REQ_SW_VERSION >> 8) & 0xff,
                        API_REQ_SW_VERSION & 0xff);
            else
                fprintf(stderr, "(library version OK)\n");
                        
                   
            fprintf(stderr, "  Library version = v%d.%02d  ",
                    (sw_version >> 8) & 0xff,
                    (sw_version >> 0) & 0xff);

            if (API_HEADER_VERSION < api_version_req)
                fprintf(stderr, "(requires header >= %d.%02d)\n",
                        (api_version_req >> 8) & 0xff,
                        (api_version_req >> 0) & 0xff);
            else
                fprintf(stderr, "(header version OK)\n");
#endif
            handle  = 0;
            *result = API_INCOMPATIBLE_LIBRARY;
            return 0;
        }
    }

    /* Bind the requested function in the shared library */
    function = (void *)dlsym(handle, name);
    *result  = function ? API_OK : API_UNABLE_TO_LOAD_FUNCTION;
    return function;
}


/*=========================================================================
| FUNCTIONS
 ========================================================================*/
static int (*c_km_find_devices) (int, u16 *) = 0;
int km_find_devices (
    int   num_ports,
    u16 * ports
)
{
    if (c_km_find_devices == 0) {
        int res = 0;
        if (!(c_km_find_devices = _loadFunction("c_km_find_devices", &res)))
            return res;
    }
    return c_km_find_devices(num_ports, ports);
}


static int (*c_km_find_devices_ext) (int, u16 *, int, u32 *) = 0;
int km_find_devices_ext (
    int   num_ports,
    u16 * ports,
    int   num_ids,
    u32 * unique_ids
)
{
    if (c_km_find_devices_ext == 0) {
        int res = 0;
        if (!(c_km_find_devices_ext = _loadFunction("c_km_find_devices_ext", &res)))
            return res;
    }
    return c_km_find_devices_ext(num_ports, ports, num_ids, unique_ids);
}


static Komodo (*c_km_open) (int) = 0;
Komodo km_open (
    int port_number
)
{
    if (c_km_open == 0) {
        int res = 0;
        if (!(c_km_open = _loadFunction("c_km_open", &res)))
            return res;
    }
    return c_km_open(port_number);
}


static Komodo (*c_km_open_ext) (int, KomodoExt *) = 0;
Komodo km_open_ext (
    int         port_number,
    KomodoExt * km_ext
)
{
    if (c_km_open_ext == 0) {
        int res = 0;
        if (!(c_km_open_ext = _loadFunction("c_km_open_ext", &res)))
            return res;
    }
    return c_km_open_ext(port_number, km_ext);
}


static int (*c_km_close) (Komodo) = 0;
int km_close (
    Komodo komodo
)
{
    if (c_km_close == 0) {
        int res = 0;
        if (!(c_km_close = _loadFunction("c_km_close", &res)))
            return res;
    }
    return c_km_close(komodo);
}


static int (*c_km_port) (Komodo) = 0;
int km_port (
    Komodo komodo
)
{
    if (c_km_port == 0) {
        int res = 0;
        if (!(c_km_port = _loadFunction("c_km_port", &res)))
            return res;
    }
    return c_km_port(komodo);
}


static int (*c_km_features) (Komodo) = 0;
int km_features (
    Komodo komodo
)
{
    if (c_km_features == 0) {
        int res = 0;
        if (!(c_km_features = _loadFunction("c_km_features", &res)))
            return res;
    }
    return c_km_features(komodo);
}


static u32 (*c_km_unique_id) (Komodo) = 0;
u32 km_unique_id (
    Komodo komodo
)
{
    if (c_km_unique_id == 0) {
        int res = 0;
        if (!(c_km_unique_id = _loadFunction("c_km_unique_id", &res)))
            return res;
    }
    return c_km_unique_id(komodo);
}


static const char * (*c_km_status_string) (int) = 0;
const char * km_status_string (
    int status
)
{
    if (c_km_status_string == 0) {
        int res = 0;
        if (!(c_km_status_string = _loadFunction("c_km_status_string", &res)))
            return 0;
    }
    return c_km_status_string(status);
}


static int (*c_km_version) (Komodo, KomodoVersion *) = 0;
int km_version (
    Komodo          komodo,
    KomodoVersion * version
)
{
    if (c_km_version == 0) {
        int res = 0;
        if (!(c_km_version = _loadFunction("c_km_version", &res)))
            return res;
    }
    return c_km_version(komodo, version);
}


static u32 (*c_km_sleep_ms) (u32) = 0;
u32 km_sleep_ms (
    u32 milliseconds
)
{
    if (c_km_sleep_ms == 0) {
        int res = 0;
        if (!(c_km_sleep_ms = _loadFunction("c_km_sleep_ms", &res)))
            return res;
    }
    return c_km_sleep_ms(milliseconds);
}


static int (*c_km_acquire) (Komodo, u32) = 0;
int km_acquire (
    Komodo komodo,
    u32    features
)
{
    if (c_km_acquire == 0) {
        int res = 0;
        if (!(c_km_acquire = _loadFunction("c_km_acquire", &res)))
            return res;
    }
    return c_km_acquire(komodo, features);
}


static int (*c_km_release) (Komodo, u32) = 0;
int km_release (
    Komodo komodo,
    u32    features
)
{
    if (c_km_release == 0) {
        int res = 0;
        if (!(c_km_release = _loadFunction("c_km_release", &res)))
            return res;
    }
    return c_km_release(komodo, features);
}


static int (*c_km_timeout) (Komodo, u32) = 0;
int km_timeout (
    Komodo komodo,
    u32    timeout_ms
)
{
    if (c_km_timeout == 0) {
        int res = 0;
        if (!(c_km_timeout = _loadFunction("c_km_timeout", &res)))
            return res;
    }
    return c_km_timeout(komodo, timeout_ms);
}


static int (*c_km_can_query_bus_state) (Komodo, km_can_ch_t, u08 *, u08 *, u08 *) = 0;
int km_can_query_bus_state (
    Komodo      komodo,
    km_can_ch_t channel,
    u08 *       bus_state,
    u08 *       rx_error,
    u08 *       tx_error
)
{
    if (c_km_can_query_bus_state == 0) {
        int res = 0;
        if (!(c_km_can_query_bus_state = _loadFunction("c_km_can_query_bus_state", &res)))
            return res;
    }
    return c_km_can_query_bus_state(komodo, channel, bus_state, rx_error, tx_error);
}


static int (*c_km_latency) (Komodo, u32) = 0;
int km_latency (
    Komodo komodo,
    u32    latency_ms
)
{
    if (c_km_latency == 0) {
        int res = 0;
        if (!(c_km_latency = _loadFunction("c_km_latency", &res)))
            return res;
    }
    return c_km_latency(komodo, latency_ms);
}


static int (*c_km_can_configure) (Komodo, u32) = 0;
int km_can_configure (
    Komodo komodo,
    u32    config
)
{
    if (c_km_can_configure == 0) {
        int res = 0;
        if (!(c_km_can_configure = _loadFunction("c_km_can_configure", &res)))
            return res;
    }
    return c_km_can_configure(komodo, config);
}


static int (*c_km_can_bus_timeout) (Komodo, km_can_ch_t, u16) = 0;
int km_can_bus_timeout (
    Komodo      komodo,
    km_can_ch_t channel,
    u16         timeout_ms
)
{
    if (c_km_can_bus_timeout == 0) {
        int res = 0;
        if (!(c_km_can_bus_timeout = _loadFunction("c_km_can_bus_timeout", &res)))
            return res;
    }
    return c_km_can_bus_timeout(komodo, channel, timeout_ms);
}


static int (*c_km_can_bitrate) (Komodo, km_can_ch_t, u32) = 0;
int km_can_bitrate (
    Komodo      komodo,
    km_can_ch_t channel,
    u32         bitrate_hz
)
{
    if (c_km_can_bitrate == 0) {
        int res = 0;
        if (!(c_km_can_bitrate = _loadFunction("c_km_can_bitrate", &res)))
            return res;
    }
    return c_km_can_bitrate(komodo, channel, bitrate_hz);
}


static int (*c_km_can_auto_bitrate) (Komodo, km_can_ch_t) = 0;
int km_can_auto_bitrate (
    Komodo      komodo,
    km_can_ch_t channel
)
{
    if (c_km_can_auto_bitrate == 0) {
        int res = 0;
        if (!(c_km_can_auto_bitrate = _loadFunction("c_km_can_auto_bitrate", &res)))
            return res;
    }
    return c_km_can_auto_bitrate(komodo, channel);
}


static int (*c_km_can_auto_bitrate_ext) (Komodo, km_can_ch_t, u32, const u32 *) = 0;
int km_can_auto_bitrate_ext (
    Komodo      komodo,
    km_can_ch_t channel,
    u32         num_bitrates_hz,
    const u32 * bitrates_hz
)
{
    if (c_km_can_auto_bitrate_ext == 0) {
        int res = 0;
        if (!(c_km_can_auto_bitrate_ext = _loadFunction("c_km_can_auto_bitrate_ext", &res)))
            return res;
    }
    return c_km_can_auto_bitrate_ext(komodo, channel, num_bitrates_hz, bitrates_hz);
}


static int (*c_km_get_samplerate) (Komodo) = 0;
int km_get_samplerate (
    Komodo komodo
)
{
    if (c_km_get_samplerate == 0) {
        int res = 0;
        if (!(c_km_get_samplerate = _loadFunction("c_km_get_samplerate", &res)))
            return res;
    }
    return c_km_get_samplerate(komodo);
}


static int (*c_km_can_target_power) (Komodo, km_can_ch_t, km_power_t) = 0;
int km_can_target_power (
    Komodo      komodo,
    km_can_ch_t channel,
    km_power_t  power
)
{
    if (c_km_can_target_power == 0) {
        int res = 0;
        if (!(c_km_can_target_power = _loadFunction("c_km_can_target_power", &res)))
            return res;
    }
    return c_km_can_target_power(komodo, channel, power);
}


static int (*c_km_enable) (Komodo) = 0;
int km_enable (
    Komodo komodo
)
{
    if (c_km_enable == 0) {
        int res = 0;
        if (!(c_km_enable = _loadFunction("c_km_enable", &res)))
            return res;
    }
    return c_km_enable(komodo);
}


static int (*c_km_disable) (Komodo) = 0;
int km_disable (
    Komodo komodo
)
{
    if (c_km_disable == 0) {
        int res = 0;
        if (!(c_km_disable = _loadFunction("c_km_disable", &res)))
            return res;
    }
    return c_km_disable(komodo);
}


static int (*c_km_can_read) (Komodo, km_can_info_t *, km_can_packet_t *, int, u08 *) = 0;
int km_can_read (
    Komodo            komodo,
    km_can_info_t *   info,
    km_can_packet_t * pkt,
    int               num_bytes,
    u08 *             data
)
{
    if (c_km_can_read == 0) {
        int res = 0;
        if (!(c_km_can_read = _loadFunction("c_km_can_read", &res)))
            return res;
    }
    return c_km_can_read(komodo, info, pkt, num_bytes, data);
}


static int (*c_km_can_async_submit) (Komodo, km_can_ch_t, u08, const km_can_packet_t *, int, const u08 *) = 0;
int km_can_async_submit (
    Komodo                  komodo,
    km_can_ch_t             channel,
    u08                     flags,
    const km_can_packet_t * pkt,
    int                     num_bytes,
    const u08 *             data
)
{
    if (c_km_can_async_submit == 0) {
        int res = 0;
        if (!(c_km_can_async_submit = _loadFunction("c_km_can_async_submit", &res)))
            return res;
    }
    return c_km_can_async_submit(komodo, channel, flags, pkt, num_bytes, data);
}


static int (*c_km_can_async_collect) (Komodo, u32, u32 *) = 0;
int km_can_async_collect (
    Komodo komodo,
    u32    timeout_ms,
    u32 *  arbitration_count
)
{
    if (c_km_can_async_collect == 0) {
        int res = 0;
        if (!(c_km_can_async_collect = _loadFunction("c_km_can_async_collect", &res)))
            return res;
    }
    return c_km_can_async_collect(komodo, timeout_ms, arbitration_count);
}


static int (*c_km_can_write) (Komodo, km_can_ch_t, u08, const km_can_packet_t *, int, const u08 *, u32 *) = 0;
int km_can_write (
    Komodo                  komodo,
    km_can_ch_t             channel,
    u08                     flags,
    const km_can_packet_t * pkt,
    int                     num_bytes,
    const u08 *             data,
    u32 *                   arbitration_count
)
{
    if (c_km_can_write == 0) {
        int res = 0;
        if (!(c_km_can_write = _loadFunction("c_km_can_write", &res)))
            return res;
    }
    return c_km_can_write(komodo, channel, flags, pkt, num_bytes, data, arbitration_count);
}


static int (*c_km_gpio_config_in) (Komodo, u08, u08, u08) = 0;
int km_gpio_config_in (
    Komodo komodo,
    u08    pin_number,
    u08    bias,
    u08    trigger
)
{
    if (c_km_gpio_config_in == 0) {
        int res = 0;
        if (!(c_km_gpio_config_in = _loadFunction("c_km_gpio_config_in", &res)))
            return res;
    }
    return c_km_gpio_config_in(komodo, pin_number, bias, trigger);
}


static int (*c_km_gpio_config_out) (Komodo, u08, u08, u08) = 0;
int km_gpio_config_out (
    Komodo komodo,
    u08    pin_number,
    u08    drive,
    u08    source
)
{
    if (c_km_gpio_config_out == 0) {
        int res = 0;
        if (!(c_km_gpio_config_out = _loadFunction("c_km_gpio_config_out", &res)))
            return res;
    }
    return c_km_gpio_config_out(komodo, pin_number, drive, source);
}


static int (*c_km_gpio_set) (Komodo, u08, u08) = 0;
int km_gpio_set (
    Komodo komodo,
    u08    value,
    u08    mask
)
{
    if (c_km_gpio_set == 0) {
        int res = 0;
        if (!(c_km_gpio_set = _loadFunction("c_km_gpio_set", &res)))
            return res;
    }
    return c_km_gpio_set(komodo, value, mask);
}


static int (*c_km_gpio_get) (Komodo) = 0;
int km_gpio_get (
    Komodo komodo
)
{
    if (c_km_gpio_get == 0) {
        int res = 0;
        if (!(c_km_gpio_get = _loadFunction("c_km_gpio_get", &res)))
            return res;
    }
    return c_km_gpio_get(komodo);
}


