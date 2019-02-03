/***********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher

    ---------------------------------------------------------------------
    File:      debug.c
    Project:   uTasker project
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2018
    *********************************************************************
    18.02.2007 Corrected port support for M52233 DEMO board              {1}
    26.02.2007 Improve port control via serial/Telnet                    {2}
    03.06.2007 Add FTP timeout and mode when configuring FTP server      {3}
    02.07.2007 Add LPC23XX GPIOs
    26.09.2007 Add LM3SXXXX GPIOs
    31.12.2007 Improve NE64 user port update                             {4}
    01.01.2008 ping test made conditional on ICMP_SEND_PING
    26.01.2008 parameter tests made conditional on parameter system      {5}
    02.05.2008 Add dummy code to allow the project to use serial interface without USE_MAINTENANCE {6}
    25.05.2008 Move fnAddTime() outside of USE_MAINTENANCE dependency    {7}
    09.08.2008 Add USB_INTERFACE debug interface support                 {8}
    08.10.2008 User POWER_UP macro for SAM7X code                        {9}
    05.02.2009 Remove some serial interface code when DEMO_UART not defined {10}
    26.04.2009 Add AVR32 support                                         {11}
    09.05.2009 Mask user ports when setting output                       {12}
    01.07.2009 Adapt for STRING_OPTIMISATION configuration (fnDebugDec() replaced by fnBufferDec() when used with string buffer), pointers used from uStrcpy no longer decremented
    24.07.2009 Add DO_RESET_ETHERNET_STATS support, which was previously missing {13}
    13.08.2009 Add simulator port call to ensure Luminary accuracy       {14}
    09.10.2009 Don't initialise port bit 1 when working with CGLCD (Luminary) {15}
    18.11.2009 Add AVR32_EVK1100 and AVR32_EVK1101 support               {16}
    28.11.2009 Add SD-card disk interface                                {17}
    19.12.2009 Automatic check of TELNET output buffer when the TELNET connection is in use {18}
    31.01.2010 Remove "root" command since this can be achieved with "cd \" {19}
    31.01.2010 File open for rename uses attribute UTFAT_OPEN_FOR_RENAME rather than UTFAT_OPEN_FOR_WRITE {20}
    05.02.2010 temp_pars->temp_parameters.ucServers changed to .usServers
    05.02.2010 Add IPV6                                                  {21}
    05.02.2010 Return value from fnGetARPentry() checked against 0 rather than against cucNullMACIP since the return value changed {22}
    21.05.2010 Add M5208 support                                         {23}
    13.06.2010 Change SD card state recognition to show unformatted information {24}
    24.09.2010 Change usDiskFlags to usDiskFlags
    09.02.2011 Add MONITOR_PERFORMANCE support                           {25}
    24.02.2011 Add additional formatting commands                        {26}
    27.02.2011 Unstall disk interface after cluster count has completed  {27}
    27.02.2011 Quit file rename when no new name has been entered        {28}
    22.03.2011 Return APP_SENT_DATA on TELNET login to avoid unnecessary TCP ack from being sent {29}
    26.03.2011 Add TEST_TCP_SERVER support                               {30}
    28.07.2011 Add time and date setting routines                        {31}
    14.08.2011 Add DNS server setting (when it doesn't follow the gateway address - DNS_SERVER_OWN_ADDRESS) {32}
    19.10.2011 Add optional PHY register control via menu                {33}
    19.10.2011 Modify the RTC time setting and displaying code to subroutine fnSetShowTime {34}
    05.12.2011 Return last command with up-arrow key and allow old command editing {35}
    18.12.2011 Add reset cause to STM32                                  {36}
    27.12.2011 Add ftp client interface                                  {37}
    27.12.2011 Add CAN menu                                              {38}
    01.02.2012 Use utFAT UTFAT_APPEND rather than seeking to file end    {39}
    18.01.2012 Add reset cause to AVR32                                  {40}
    05.02.2012 Replace condition USB_INTERFACE with USE_USB_CDC          {41}
    18.02.2012 Display FAT16/32 formatting when FAT16 enabled            {42}
    20.02.2012 Display large SD card sizes in kBytes to avoid 32 bit overflow {43}
    03.03.2012 SD card sector write debugging option added               {44}
    03.03.2012 Allow utFAT operation without UTFAT_WRITE                 {45}
    02.04.2012 Add link-local and global IPv6 addresses                  {46}
    15.04.2012 Add TWR_K20D50M support and GPIO output control reworked
    27.04.2012 Reset causes adapted for Kinetis devices with FPU
    27.05.2012 Adapt network parameters for compatibility with multiple networks {47}
    03.06.2012 Add FTP client IPv6 support                               {48}
    04.07.2012 Display removed SD card info state correctly              {49}
    09.08.2012 ping test modified to be compatible with sockets wider than a byte (used with multiple networks and interfaces) {50}
    13.08.2012 Optionally save MAC to program-once memory (Kinetis)      {K1}
    15.08.2012 Add ICMP_PING_IP_RESULT to display ping source(s)         {51}
    26.09.2012 Allow write test to non-formatted SD card                 {52}
    03.10.2012 Include HW specific parts of the file in a new include file {53}
    30.01.2013 Add memory modify/display interface                       {54}
    16.05.2013 Add optional ezport interface                             {55}
    23.06.2013 Add multi-port PHY and Micrel SMI modes                   {56}
    06.08.2013 Add IMMEDIATE_MEMORY_ALLOCATION compatibility of fnStartTelnet() use {57}
    17.08.2013 Modify PHY debugging interface for _DP83849I              {58}
    02.10.2013 Add utFAT truncation command                              {59}
    03.11.2013 Add utFAT expert functions, undelete and safe delete      {60}
    07.01.2014 Add option to display SD card sector content in big-endian style on little-endian processors (or simulator) or reverse {61}
    17.01.2014 Pass directory pointer to utFAT2.0 version of utOpenFile(){62}
    16.02.2014 Add last reset request command                            {63}
    05.03.2014 Add USB error counter display and reset                   {64}
    26.03.2014 Improve memory display interface                          {65}
    12.04.2014 Add IGMP/multicast interface                              {66}
    16.04.2014 Extend IP counters to multiple networks                   {67}
    03.05.2014 Add accelerometer debug out enable                        {68}
    11.05.2014 Memory modify corrections and storage interface added     {69}
    22.05.2014 Allow Telnet to operate on multiple networks              {70}
    15.07.2014 Add low power mode selection                              {71}
    31.10.2014 Add TELNET client interface                               {72}
    03.12.2014 Modify fnSendUDP_multicast() parameter use                {73}
    13.12.2014 Change storage erase length input from decimal to hex     {74}
    27.12.2014 Add LCD backlight intensity control                       {75}
    08.02.2015 Time keeping functions moved to uTasker\time_keeper.c
    08.02.2015 Add alarm time/date                                       {76}
    01.03.2015 Add USB-keyboard input operation                          {77}
    15.09.2014 Add dusk and dawn support                                 {78}
    06.11.2015 Add worst-case used stack display                         {79}
    11.11.2015 Accept more than 9 power mode inputs                      {80}
    18.11.2015 Enable disk interface when USB-MSD host is enabled        {81}
    23.11.2015 Add USB host CDC virtual COM connection                   {82}
    17.01.2015 Add file hide and write-protect commands                  {83}
    12.12.2016 Add CMSIS CFFT and AES                                    {84}
    02.02.2017 Add low power cycling control                             {85} - see video https://youtu.be/v4UnfcDiaE4
    05.07.2017 Modify SD card sector write interface                     {86}
    10.11.2017 Add MQTT client commands                                  {87}
    04.12.2017 Add memory-mapped divide and square root demonstration    {88} - Kinetis processors with MMDVSQ
    17.01.2018 Add I2C master firmware loader                            {89}
    16.03.2018 Accept control-? (0x7f) as back space (used by putty by default) {90}
    17.03.2018 Add FlexIO menu                                           {91}

*/


/* =================================================================== */
/*                           include files                             */
/* =================================================================== */

#include "config.h"


// Each project includes a number of functions that allow different processors of configurations to share the same main file
//

/* =================================================================== */
/*                          local definitions                          */
/* =================================================================== */

//#define TEST_SDCARD_SECTOR_WRITE                                       // {44} activate to allow sector writes to be tested
//#define TEST_I2C_INTERFACE                                             // activate to enable special I2C tests via menu
//#define DEVELOP_PHY_CONTROL                                            // {33} activate to enable PHY register dump and writes to individual register addresses (warning: disabled STOP_MII_CLOCK option when using this)
                                                                         // note that STOP_MII_CLOCK should not be enabled when using this (kinetis)
//#define _DEBUG_CAN                                                     // support dumping CAN register details for debugging purpose
//#define I2C_MASTER_LOADER                                              // {89} load firmware to a connected I2C slave (requires I2C_INTERFACE - enable TEST_I2C in i2c_tests.h for interface open)
#if defined CHIP_HAS_FLEXIO
  //#define TEST_FLEXIO                                                  // {91} allow testing of flexio operations
#endif

#if defined CMSIS_DSP_CFFT
    #define TEST_CMSIS_CFFT                                              // {84} enable test of CMSIS CFFT
#endif
#if !defined KINETIS_KE && !defined KINETIS_KL
  //#define EZPORT_CLONER                                                // {55}
        #define EZPORT_CLONER_SKIP_REGIONS                               // cloner leaves defined areas blank
#endif
//#define LOW_MEMORY                                                     // remove utFAT print and sector display to save stack space when calling other utFAT interface routines

#if defined EZPORT_CLONER                                                // {55}
    #define EZCOMMAND_WREN      0x06                                     // write enable
    #define EZCOMMAND_WRDI      0x04                                     // write disabled
    #define EZCOMMAND_RDSR      0x05                                     // read status register (reads one byte of data)
    #define EZCOMMAND_WRCR      0x01                                     // write configuration register (writes one byte of data)
    #define EZCOMMAND_READ      0x03                                     // read data (writes 3 address bytes and reads one or more data bytes)
    #define EZCOMMAND_FAST_READ 0x0b                                     // read data (writes 3 address bytes plus dummy byte and reads one or more data bytes)
    #define EZCOMMAND_PP        0x02                                     // page program (writes 3 address bytes plus 4 to 256 data bytes)
    #define EZCOMMAND_SE        0xd8                                     // sector erase (writes 3 address bytes)
    #define EZCOMMAND_BE        0xc7                                     // bulk erase
    #define EZCOMMAND_RESET     0xb9                                     // reset chip

    // Stats register flags
    //
    #define EZCOMMAND_SR_WIP    0x01                                     // write in progress
    #define EZCOMMAND_SR_WEN    0x02                                     // write enable
    #define EZCOMMAND_SR_CRL    0x20                                     // configuration register has been loaded; erase and program commands are accepted
    #define EZCOMMAND_SR_WEF    0x40                                     // write error flag
    #define EZCOMMAND_SR_FS     0x80                                     // flash security status

    #define CLONING_VERIFYING_CLONED_CODE  (0x1000)
    #define CLONING_VERIFYING_CODE         (0x2000)
    #define CLONING_VERIFYING_BLANK        (0x4000)
    #define CLONING_ERASING_BULK           (0x8000)

    #define FLASH_PAGE_SIZE (1024)                                       // value suitable for Kinetis EzPort

    #define EZ_RESET_CS_DELAY_US           (10)                          // delay in us between the reset line being negated and the CS line being taken high (when entering EzPort mode)
#endif

#if defined USE_TELNET || defined SERIAL_INTERFACE || defined USE_USB_CDC// {8}
    #define KEEP_DEBUG                                                   // remove the debug interface if none of these interfaces are used
#endif

#if defined USE_MAINTENANCE
    #define OWN_TASK                TASK_DEBUG

    #define TCP_SERVER_TEST         5

    #define STALL_CRITICAL          (TX_BUFFER_SIZE/2)                   // when the Windows TCP implementation sees that the window is less than half the maximum windows size we have advertised it will perform a delay of about 5s.

    #define I2C_READ_ADDRESS        0xa5
    #define I2C_WRITE_ADDRESS       0xa4
  //#define I2C_READ_ADDRESS        0xaf                                 // the I2C device address to test
  //#define I2C_WRITE_ADDRESS       0xae
      //#define I2C_TWO_BYTE_ADDRESS                                     // when addressing I2C device internal registers use two bytes (16 bit addressing)

    #define UT_PATH_LENGTH          256                                  // length of maximum directory string

    #define BYTE_ACCESS             1
    #define SHORT_WORD_ACCESS       2
    #define LONG_WORD_ACCESS        4

    #define STALL_DIR_LISTING       1
    #define STALL_PRINTING_FILE     2
    #define STALL_COUNTING_CLUSTERS 3
    #define STALL_DIRD_LISTING      4
    #define STALL_DIRH_LISTING      5

    #define IPv4_DUMMY_SOCKET       SOCKET_NUMBER_MASK                   // {50} dummy IPv4 socket number (assumed no socket of this high number in the system)
    #define IPv6_DUMMY_SOCKET      (SOCKET_NUMBER_MASK - 1)              // {50} dummy IPv6 socket number (assumed no socket of this high number in the system)
                                                                         // we want to avoid this and so ensure that our TCP informs when the Winodw has opened again.

#if defined KEEP_DEBUG                                                   // if neither Telnet nor serial, we save space by removing debug task code

#define MAX_TAB_MARK              27                                     // this defines the maximum tab length supported
#define MAX_MENU_LINE             66                                     // a line of a menu should not be longer than this

#define DO_HELP                   0                                      // reference to help handler
    #define DO_MAIN_HELP            0                                    // specific help command to display main help menu
    #define DO_HELP_AT              1                                    // specific help command to display AT help menu
    #define DO_HELP_TELNET          2                                    // specific help command to display TELNET help menu

    #define DO_MENU_HELP_LAN        4                                    // go to LAN menu
    #define DO_MENU_HELP_SERIAL     5                                    // go to serial menu
    #define DO_MENU_HELP_IO         6                                    // go to I/O menu
    #define DO_MENU_HELP_ADMIN      7                                    // go to Administration menu
  //#define DO_MENU_HELP_SERVICES   8                                    // go to services menu
    #define DO_MENU_HELP_STATS      9                                    // go to statistics menu
    #define DO_MENU_HELP_USB        10                                   // go to USB menu
    #define DO_MENU_HELP_TCP        11                                   // go to TCP menu
  //#define DO_MENU_HELP_TCP_SER    12                                   // TCP/serial configs
    #define DO_MENU_HELP_PHY        12
    #define DO_MENU_HELP_OLED       13
    #define DO_MENU_HELP_I2C        14
    #define DO_MENU_HELP_DISK       15                                   // {17}
    #define DO_MENU_HELP_FTP_TELNET_MQTT 16                              // {37}
    #define DO_MENU_HELP_CAN        17                                   // {38}
    #define DO_MENU_HELP_ADVANCED   18                                   // {84}
    #define DO_MENU_HELP_FLEXIO     19
    #define DO_HELP_UP              20                                   // go up menu tree

#define DO_HARDWARE               1                                      // reference to hardware group of commands
    #define DO_RESET                0                                    // specific hardware command to reset target
    #define DO_DISPLAY_MEMORY_USE   1                                    // specific hardware command to display memory use
    #define DO_DDR                  2                                    // specific hardware command to set port direction
    #define DO_INPUT                4                                    // specific hardware command to read input port values
    #define DO_OUTPUT               5                                    // specific hardware command to set output port values
    #define DO_READ_ADC             6                                    // specific hardware command to read ADC inputs
    #define DO_READY_DOWNLOAD       7                                    // specific hardware command to prepare for a download
    #define DO_SET_USER_OUTPUT      8                                    // specific hardware command to configure user port outputs
    #define DO_GET_USER_OUTPUT      9                                    // specific hardware command to get user port outputs configuration
    #define DO_DISPLAY_UP_TIME     10                                    // specific hardware command to display up time
    #define DO_GET_DDR             11                                    // specific hardware command to get ddr
    #define DO_SAVE_PORT           12                                    // specific hardware command to save port settings
    #define DO_DISPLAY_TASK_USE    13                                    // specific hardware command to display the duration of tasks
    #define DO_TASK_USE_RESET      14                                    // specific hardware command to reset task duration monitoring
    #define DO_MEM_DISPLAY         15                                    // specific hardware command to display memory contents
    #define DO_MEM_WRITE           16                                    // specific hardware command to write memory contents
    #define DO_MEM_FILL            17                                    // specific hardware command to fill memory contents
    #define DO_EZINIT              18                                    // specific hardware command to configure ports for ezport use
    #define DO_EZRESET             19                                    // specific hardware command to reset the processor input ezport mode
    #define DO_EZSTATUS            20                                    // specific hardware command to read and display the ezport status register
    #define DO_EZGETREADY          21                                    // specific hardware command to prepare the ezport for programming
    #define DO_EZBULK              22                                    // specific hardware command to command a buld erase of the attached chip
    #define DO_EZCLONE             23                                    // specific hardware command to clone local software to the attached chip
    #define DO_EZSCLONE            24                                    // specific hardware command to securely clone local software to the attached chip
    #define DO_EZCOMMAND           25                                    // specific hardware command to write individual commands
    #define DO_EZSTART             26                                    // specific hardware command to allow the ezport slave to start running
    #define DO_EZREAD              27                                    // specific hardware command to read data from the ezport slave
    #define DO_EZSECT              28                                    // specific hardware command to erase a sector/page in ezport slave's memory
    #define DO_EZPROG              29                                    // specific hardware command to unsecure the ezport slave
    #define DO_EZVERIF             30                                    // specific hardware command to verify that the ezport is blank or correctly cloned
    #define DO_EZUNSECURE          31                                    // specific hardware command to read data from the ezport slave
    #define DO_LAST_RESET          32                                    // specific hardware command to display the last reset cause
    #define DO_STORAGE_DISPLAY     33                                    // specific hardware command to display storage memory contents
    #define DO_STORAGE_WRITE       34                                    // specific hardware command to write storage memory contents
    #define DO_STORAGE_FILL        35                                    // specific hardware command to fill storage memory contents
    #define DO_STORAGE_PAGE        36                                    // specific hardware command to write a page buffer to storage memory
    #define DO_STORAGE_ERASE       37                                    // specific hardware command to erase memory contents
    #define DO_LP_GET              38                                    // specific hardware command to show present low power mode, and available options
    #define DO_LP_SET              39                                    // specific hardware command to set new low power mode
    #define DO_GET_BACKLIGHT       40                                    // specific hardware command to show backlight intensity
    #define DO_SET_BACKLIGHT       41                                    // specific hardware command to set backlight intensity
    #define DO_SUSPEND_PAGE        42                                    // specific hardware command to test suspend/resume operation during erasure
    #define DO_FFT                 43                                    // specific hardware command to test CMSIS CFFT operation
    #define DO_AES128              44                                    // specific hardware command to test AES128 operation
    #define DO_AES192              45                                    // specific hardware command to test AES192 operation
    #define DO_AES256              46                                    // specific hardware command to test AES256 operation
    #define DO_SHA256              47                                    // specific hardware command to test SHA256 operation
    #define DO_LP_CYCLE            48                                    // specific hardware command to enable/disable low power cycle mode
    #define DO_GET_CONTRAST        49                                    // specific hardware command to show the LCD contrast setting (%)
    #define DO_SET_CONTRAST        50                                    // specific hardware command to set the LCD contrast setting (%)
    #define DO_SQRT                51                                    // specific hardware command to calculate the square root of an integer entered as dec or hex
    #define DO_DIV                 52                                    // specific hardware command to calculate the remainder and quotient of an integer division
    #define DO_FLEXIO_ON           53                                    // specific flexio command to power up the flexio module
    #define DO_FLEXIO_PIN          54                                    // specific flexio command to configure pin(s) to flexio mode
    #define DO_FLEXIO_PIN_STATE    55                                    // specific flexio command to display the pin states
    #define DO_FLEXIO_STATUS       56                                    // specific flexio command to display shifter and timer status
    #define DO_SHOW_TRIM           57
    #define DO_CHANGE_TRIM_COARSE  58
    #define DO_CHANGE_TRIM_FINE    59

#define DO_TELNET                 2                                      // reference to Telnet group
    #define DO_TELNET_QUIT              0                                // specific Telnet comand to quit the session
    #define DO_TELNET_INACTIVITY_TIME   1                                // specific Telnet comand to set inactivity timeout
    #define DO_SET_TELNET_PORT          3                                // specific Telnet comand to set telnet port number
    #define DO_ENABLE_TELNET_DIALOUT    4                                // specific Telnet comand to enable/disable telnet dialout
    #define DO_ENABLE_TELNET_RAW_DIAL   5                                // specific Telnet comand to enable/disable telnet dialout in raw mode
    #define DO_ENABLE_TELNET_SCANNING   6                                // specific Telnet comand to enable/disable telnet escape sequence scanning
    #define DO_SET_TELNET_ESCAPE        7                                // specific Telnet comand to set escape sequence for command mode
    #define DO_RESUME_SESSION           8                                // specific Telnet comand to resume dial out data session
    #define DO_SERIAL_TCP_CONFIG        9                                // specific Telnet comand to show serial/TCP config

#define DO_FLASH                  3                                      // reference to FLASH group
    #define DO_SET_VALUE            0                                    // specific FLASH command to set a value
    #define DO_GET_VALUE            1                                    // specific FLASH command to get a value
    #define DO_SET_DEFAULTS         2                                    // specific FLASH command to return default parameters
    #define DO_INVALID_PARS         3                                    // specific FLASH command to invalidate all parameters
    #define DO_SET_TEMP_VALUE       4                                    // specific FLASH command to set a temporary value
    #define DO_VALIDATE_NEW_PARS    5                                    // specific FLASH command to validate new parameters
    #define DO_SAVE_PARS            6                                    // specific FLASH command to save and validate parameters
    #define DO_REJECT_CHANGES       7                                    // specific FLASH command to reset temporary modification

#define DO_IP                     4                                      // reference to IP group
    #define PING_TEST               0                                    // specific IP command to send ping test
    #define DO_SET_IP_ADDRESS       1                                    // specific IP command to set device's IPV4 address
    #define DO_SET_IP_SUBNET_MASK   2                                    // specific IP command to set device's subnet mask
    #define DO_SET_DEFAULT_GATEWAY  3                                    // specific IP command to set default gateway
    #define DO_SHOW_ETHERNET_STATS  4                                    // specific IP command to show ethernet statistics
    #define DO_SHOW_IP_CONFIG       5                                    // specific IP command to show IP configuration
    #define DO_SET_LAN_SPEED        6                                    // specific IP command to set speed of LAN
    #define SHOW_ARP                7                                    // specific IP command to show ARP table contents
    #define DELETE_ARP              8                                    // specific IP command to delete ARP table contents
    #define DO_SET_MAC              9                                    // specific IP command to set a new MAC address
    #define DO_SHOW_IP_CONFIG_TEMP  10                                   // specific IP command to show the temporary IP configuration
    #define DO_RESET_ETHERNET_STATS 11                                   // specific IP command to reset the ethernet statistics
    #define DO_SET_IPV6_ADDRESS     12                                   // specific IP command to set device's IPV6 address
    #define SHOW_NN                 13                                   // specific IP command to show IPV6 neighbor table contents
    #define DELETE_NN               14                                   // specific IP command to delete IPV6 neighbor table contents
    #define PING_TEST_V6            15                                   // specific IP command to send IPV6 ping test
    #define DO_SET_DEFAULT_DNS      16                                   // specific IP command to set DNS server's IP address
    #define DO_DUMP_PHY             17                                   // specific IP command to dump PHY register values
    #define DO_SET_PHY              18                                   // specific IP command to set PHY register value
    #define DO_PHY_ADDRESS          19                                   // specific IP command to set PHY address
    #define DO_READ_PHY_SMI         20                                   // specific IP command to read PHY register via SMI interface
    #define DO_WRITE_PHY_SMI        21                                   // specific IP command to write PHY register via SMI interface
    #define SEND_MULTICAST          22                                   // specific IP command to send a multicast frame
    #define LIST_IGMP_HOSTS         23                                   // specific IP command to list multicast host group
    #define JOIN_HOST_GROUP         24                                   // specific IP command to join a multicast group
    #define LEAVE_HOST_GROUP        25                                   // specific IP command to leave a multicast group
    #define DO_SET_NETWORK          26                                   // specific IP command to set the network being referenced
    #define DO_SET_INTERFACE        27                                   // specific IP command to set the interface being referenced
    
#define DO_SERIAL                 5                                      // reference to serial group
    #define SERIAL_SET_BAUD               0                              // specific serial command to set baud rate
    #define SERIAL_SET_STOP               1                              // specific serial command to set stop bits
    #define SERIAL_SET_DATA_LENGTH        2                              // specific serial command to set data character length
    #define SERIAL_SET_PARITY             3                              // specific serial command to set parity
    #define SERIAL_SET_FLOW               4                              // specific serial command to set baud flow control
    #define DO_ENABLE_SERIAL_ETHERNET     5                              // specific serial command to enable/disable automatic serial->ethernet telnet connection
    #define DO_ENABLE_SERIAL_ESCAPE_SCAN  6                              // specific serial command to enable/disable escape sequence scanning
    #define DO_SET_SERIAL_ESCAPE          7                              // specific serial command to set escape sequence to be scanned
    #define SERIAL_SET_WAIT_BUFFER_SIZE   8                              // specific serial command to set wait buffer size
    #define SERIAL_SET_WAIT_BUFFER_TIME   9                              // specific serial command to set wait buffer wait time
    #define DO_ENABLE_ETHERNET_SERIAL     10                             // specific serial command to enable/disable automatic ethernet->serial connection
    #define RESERVED_COMMAND              11                             // specific serial command - reserved for the moment
    #define SERIAL_SET_DIALOUT_PORT       12                             // specific serial command to set dialout port number
    #define SERIAL_ENABLE_RAW_DIALOUT     13                             // specific serial command to enable/disable dial out in raw mode
    #define SERIAL_ENABLE_SERIAL_ETHERNET 14                             // specific serial command to enable/disable serial->ethernet dialin
    #define SERIAL_DIALIN_PORT            15                             // specific serial command to set serial->ethernet dialin port number
    #define SERIAL_DIALIN_IP_ADDRESS      16                             // specific serial command to set serial->ethernet dialin IP address
    #define SERIAL_SHOW_CONFIG            17                             // specific serial command to show serial configuration
    #define SERIAL_SHOW_STATS             18                             // specific serial command to show serial statistics
    #define SERIAL_TRUSTED_IP             19                             // specific serial command to set a trusted IP address
    #define DO_ENABLE_BINARY_CONNECTION   20                             // specific serial command to set binary / Telnet data connection
    #define SERIAL_SET_HIGH_WATER         21                             // specific serial command to set flow control high water level
    #define SERIAL_SET_LOW_WATER          22                             // specific serial command to set flow control low water level
    #define SERIAL_ENABLE_AUTO_SER_ETH    23                             // specific serial command to enable/disable auto dial-in
    #define SERIAL_ENABLE_AUTO_SER_ETH_HW 24                             // specific serial command to enable/disable DTR rather than data dial-in (when auto dial-in enabled)
    #define RESET_SERIAL_STATS            25                             // specific serial command to reset serial statistics
    #define DO_ENABLE_IAC_TX              26                             // specific serial command to enable/disable IAC stuffing when in RAW mode
    #define DO_ENABLE_IAC_RX              27                             // specific serial command to enable/disable IAC filtering when in RAW mode
    #define SERIAL_SHOW_USB_ERRORS        28                             // specific serial command to show USB error counters
    #define SERIAL_RESET_USB_ERRORS       29                             // specific serial command to reset USB error counters

#define DO_ADMIN                  6                                      // reference to administration group
    #define DO_SETNAME_PASS         0                                    // specific administration command to set user and password
    #define DO_SHOW_STATS           1                                    // specific administration command to show statistics
    #define DO_SHOW_CONFIG          2                                    // specific administration command to show configuration
    #define DO_SHOW_INFO            3                                    // specific administration command to show information
    #define DO_SET_DEVICE_NAME      4                                    // specific administration command to set a device identification name
    #define DO_SET_SERIAL_NUMBER    5                                    // specific administration command to set new serial number
    #define DO_CONNECT              6                                    // specific administration command to connect to test server
    #define DO_START                7                                    // specific administration command to start server test
    #define SET_DATE                8                                    // specific administration command to set data to RTC
    #define SET_TIME                9                                    // specific administration command to set time of day to RTC
    #define SHOW_TIME               10                                   // specific administration command to show time and date
    #define SET_ALARM               11                                   // specific administration command to set an alarm at a specific time and (optional) date
    #define SHOW_ALARM              12                                   // specific administration command to show alarm time and date
    #define CLEAR_ALARM             13                                   // specific administration command to clear an alarm
    #define SET_LOCATION            14
    #define SHOW_LOCATION           15
    #define DARK                    16
    #define DUSK                    17
    #define DAWN                    18

#define DO_SERVER                 7                                      // reference to server group
    #define DO_ENABLE_TELNET        0                                    // specific server command to enable/disable telnet
    #define DO_ENABLE_DHCP          1                                    // specific server command to enable/disable DHCP client
    #define DO_ENABLE_FTP           2                                    // specific server command to enable/disable FTP server
    #define DO_ENABLE_WEB_SERVER    3                                    // specific server command to enable/disable HTTP server
    #define SHOW_SECURITY           4                                    // specific server command to show security settings
    #define DO_HTTP_AUTH            5                                    // specific server command to enable/disable HTTP authentication
    #define DO_ENABLE_LOGIN         6                                    // specific server command to enable/disable terminal menu login

#define DO_USB                    8                                      // reference to USB group
    #define DO_USB_RS232_MODE       0                                    // specific USB command to go to RS232-USB mode
    #define DO_USB_DOWNLOAD         1                                    // specific USB command to download firmware
    #define DO_USB_UPLOAD           2                                    // specific USB command to upload data
    #define DO_USB_KEYBOARD         3                                    // specific USB command to connect the debug input to a USB-keyboard input
    #define DO_VIRTUAL_COM          4                                    // specific USB command to open a connection to a connected CDC device
    #define DO_USB_DELTA            5                                    // specific USB command to display present audio drift value
    #define DO_USB_KEYBOARD_DELAY   6                                    // specific USB command to display and change keystroke delay
    #define DO_USB_SPEED            7                                    // specific USB command to send 10MBytes of data to test the transmit speed

#define DO_OLED                   9                                      // reference to OLED group
    #define DO_OLED_GET_GRAY_SCALES 0                                    // specific OLED command to get gray levels
    #define DO_OLED_SET_GRAY_SCALE  1                                    // specific OLED command to set gray level value

#define DO_I2C                    10                                     // reference to I2C group
    #define DO_I2C_WRITE            0                                    // specific I2C command to write to I2C bus
    #define DO_I2C_READ             1                                    // specific I2C command to read from I2C bus
    #define DO_I2C_READ_NO_ADDRESS  2                                    // specific I2C command to read from I2C bus without first sending an address
    #define DO_I2C_READ_PLUS_WRITE  3                                    // specific I2C command to read from I2C bus from present address, followed by a queued write to set address zero
    #define DO_ACC_ON               4                                    // specific I2C command to enable accelerator debugout output
    #define DO_ACC_OFF              5                                    // specific I2C command to disable accelerator debugout output
    #define DO_I2C_LOAD_FIRMWARE    6                                    // specific I2C command to load firmware to an I2C slave

#define DO_DISK                   11                                     // reference to SD-card, disk group
    #define DO_DIR                  0                                    // specific command to display directory content
    #define DO_CHANGE_DIR           1                                    // specific command to change directory
    #define DO_NEW_FILE             2                                    // specific command to create a file
    #define DO_NEW_DIR              3                                    // specific command to create a directory
    #define DO_WRITE_FILE           4                                    // specific command to add file content
    #define DO_PRINT_FILE           5                                    // specific command to print the content of a file
    #define DO_ROOT                 6                                    // specific command to set root directory
    #define DO_DELETE               7                                    // specific command to delete a file or directory
    #define DO_DISPLAY_SECTOR       8                                    // specific command to display a sector of the SD card
    #define DO_FORMAT               9                                    // specific command to format a disk
    #define DO_REFORMAT             10                                   // specific command to reformat a disk
    #define DO_INFO                 11                                   // specific command to display information about SD card and utFAT
    #define DO_RENAME               12                                   // specific command to rename file or directory
    #define DO_DISPLAY_SECTOR_INFO  13                                   // specific command to display sector information (NAND flash)
    #define DO_DELETE_REMAP_INFO    14                                   // specific command to delete remap info (NAND flash - only development since all data is lost, including level-wear data)
    #define DO_DISPLAY_PAGE         15                                   // specific command to delete physical page content (including spare area)
    #define DO_DELETE_FAT           16                                   // specific command to delete all sectors in FAT
    #define DO_FORMAT_FULL          17                                   // specific command to format a disk completely
    #define DO_REFORMAT_FULL        18                                   // specific command to re-format a disk completely
    #define DO_TEST_NAND            19                                   // specific command to write test patterns to NAND
    #define DO_VERIFY_NAND          20                                   // specific command to verify test patterns in NAND
    #define DO_WRITE_SECTOR         21                                   // specific command to write a test pattern to SD card sector
    #define DO_TEST_TRUNCATE        22                                   // specific command to truncate an existing file to a smaller length
    #define DO_DIR_DELETED          23                                   // specific command to list deleted content in a directory
    #define DO_DIR_CORRUPTED        24                                   // specific command to list corrupted content in a directory
    #define DO_INFO_FILE            25                                   // specific command to display details of a file or directory on the disk
    #define DO_DELETE_SAFE          26                                   // specific command to delete content safely (complete content removal without undelete possibilities)
    #define DO_UNDELETE             27                                   // specific command to undelete a file or directory
    #define DO_INFO_DELETED         28                                   // specific command to display details of a deleted file or directory on the disk
    #define DO_WRITE_MULTI_SECTOR   29                                   // specific command to write a test pattern to multiple contiguous SD card sectors
    #define DO_WRITE_MULTI_SECTOR_PRE 30                                 // specific command to write a test pattern to multiple contiguous SD card sectors with pre-delete
    #define DO_DISK_NUMBER          31                                   // specific command to select the disk
    #define DO_DIR_HIDDEN           32                                   // specific command to list of all directory content, including invisible items
    #define DO_WRITE_HIDE           33                                   // specific command to set a file/directory as hidden
    #define DO_WRITE_UNHIDE         34                                   // specific command to remove hidden file/director property
    #define DO_SET_PROTECT          35                                   // specific command to set file/directory write-protection
    #define DO_REMOVE_PROTECT       36                                   // specific command to remove file/directory write-protection
    #define DO_DISPLAY_MULTI_SECTOR 37                                   // specific command to display multiple sectors of the SD card

#define DO_FTP_TELNET_MQTT        12
    #define DO_SHOW_FTP_CONFIG      0                                    // specific ftp client command to show settings
    #define DO_FTP_SET_PORT         1                                    // specific ftp client command to set command port number
    #define DO_FTP_SERVER_IP        2                                    // specific ftp client command to set default server IP address
    #define DO_FTP_USER_NAME        3                                    // specific ftp client command to set default FTP user name
    #define DO_FTP_USER_PASS        4                                    // specific ftp client command to set default FTP password
    #define DO_FTP_SET_IDLE_TIMEOUT 5                                    // specific ftp client command to set default FTP idle timeout
    #define DO_FTP_PASSIVE          10                                   // specific ftp client command to set default FTP mode (active/passive)

    #define DO_FTP_CONNECT          20                                   // specific ftp client command to connect to FTP server
    #define DO_FTP_DISCONNECT       21                                   // specific ftp client command to disconnect from FTP server
    #define DO_FTP_DIR              22                                   // specific ftp client command to perform a directory listing
    #define DO_FTP_PATH             23                                   // specific ftp client command to move to a new location on the FTP server
    #define DO_FTP_MKDIR            24                                   // specific ftp client command to make a new directory on the FTP server
    #define DO_FTP_GET              25                                   // specific ftp client command to perform a binary file get from the FTP server
    #define DO_FTP_GETA             26                                   // specific ftp client command to perform an ascii file get from the FTP server
    #define DO_FTP_PUT              27                                   // specific ftp client command to perform a binary file put to the FTP server
    #define DO_FTP_PUTA             28                                   // specific ftp client command to perform an ascii file put to the FTP server
    #define DO_FTP_APP              29                                   // specific ftp client command to perform a binary file append at the FTP server
    #define DO_FTP_APPA             30                                   // specific ftp client command to perform an ascii file append at the FTP server
    #define DO_FTP_RENAME           31                                   // specific ftp client command to rename a file or directors at the FTP server
    #define DO_FTP_DEL              32                                   // specific ftp client command to delete a file from the FTP server
    #define DO_FTP_REMOVE_DIR       33                                   // specific ftp client command to remove an empty directory from on the FTP server
    #define DO_FTP_CONNECT_IPV6     34                                   // specific ftp client command to connect to FTP server over IPv6

    #define DO_TELNET_SET_INTERFACE 59
    #define DO_TELNET_SHOW          60                                   // specific TELNET client command to show its configuration
    #define DO_TELNET_SET_ECHO      61
    #define DO_TELNET_SET_TERM_TYPE 62
    #define DO_TELNET_SET_TERM_SPEED 63
    #define DO_TELNET_SET_X_LOCATION 64
    #define DO_TELNET_SET_ENVIRONMENT 65
    #define DO_TELNET_SET_PORT      66
    #define DO_TELNET_CONNECT       67
    #define DO_TELNET_SET_NEGOTIATION 68

    #define DO_MQTT_CONNECT         69
    #define DO_MQTTS_CONNECT        70
    #define DO_MQTT_SUBSCRIBE       71
    #define DO_MQTT_UNSUBSCRIBE     72
    #define DO_MQTT_TOPICS          73
    #define DO_MQTT_PUB             74
    #define DO_MQTT_PUB_LONG        75
    #define DO_MQTT_DISCONNECT      76

#define DO_CAN                    13
    #define DO_SEND_CAN_DEFAULT     0                                    // specific CAN command to send a message to the default destination
    #define DO_SEND_CAN_STANDARD    1                                    // specific CAN command to send a message to a standard (ID) destination
    #define DO_SEND_CAN_EXTENDED    2                                    // specific CAN command to send a message to an extended (ID) destination
    #define DO_REQUEST_CAN_DEFAULT  3                                    // specific CAN command to collect a remote message from the default destination
    #define DO_REQUEST_CAN_STANDARD 4                                    // specific CAN command to collect a remote message from a standard (ID) destination
    #define DO_REQUEST_CAN_EXTENDED 5                                    // specific CAN command to collect a remote message from an extended (ID) destination
    #define DO_SET_CAN_REMOTE       6                                    // specific CAN command to deposit a remote message to be collected by other nodes
    #define DO_CLEAR_CAN_REMOTE     7                                    // specific CAN command to cancel a remote message
    #define DO_DEBUG_CAN            8                                    // specific CAN command to dumy CAN register details
    #define DO_CLEAR_REMOTE_BUF     9                                    // specific CAN command to free message buffer used to receive a remote request that didn't return

#define MENU_HELP_LAN               1
#define MENU_HELP_SERIAL            2
#define MENU_HELP_IO                3
#define MENU_HELP_ADMIN             4
#define MENU_HELP_STATS             5
#define MENU_HELP_USB               6
#define MENU_HELP_I2C               7
#define MENU_HELP_DISK              8
#define MENU_HELP_FTP               9
#define MENU_HELP_CAN               10
#define MENU_HELP_ADVANCED          11
#define MENU_HELP_FLEXIO            11                                   // in place of MENU_HELP_ADVANCED

#if defined USE_FTP_CLIENT && !defined FTP_CLIENT_BUFFERED_SOCKET_MODE && !defined FTP_CLIENT_EXTERN_DATA_SOCKET // {37}
    #define FTP_SIMPLE_DATA_SOCKET
    #define FTP_TX_BUFFER_MAX       8                                    // small buffer for collecting data to be sent (typing type input expected)
#endif

/* =================================================================== */
/*                      local structure definitions                    */
/* =================================================================== */

typedef struct stDEBUG_COMMAND
{
    CHAR    *strCommand;                                                 // the command string to accept
    CHAR    *strText;                                                    // the help text related to this command
    unsigned char  ucDoFunction;                                         // reference to the function handling the command
    unsigned char  ucType;                                               // the command type
} DEBUG_COMMAND;

typedef struct stMENUS
{
    const DEBUG_COMMAND  *pMenu;                                         // pointer to the menu
    unsigned char  ucTabSize;                                            // the tab width for this menu
    unsigned char  ucMenuLength;                                         // the length of this menu
    CHAR           *cTitle;                                              // menu caption
} MENUS;

#if defined USE_FTP_CLIENT && !defined FTP_CLIENT_BUFFERED_SOCKET_MODE && !defined FTP_CLIENT_EXTERN_DATA_SOCKET // {37}
    typedef struct stFTP_MESSAGE
    {
        TCP_HEADER     tTCP_Header;                                      // reserve header space
        unsigned char  ucTCP_Message[FTP_TX_BUFFER_MAX];                 // message space
    } FTP_MESSAGE;
#endif

typedef struct stCLONER_SKIP_REGION
{
    unsigned long ulStartAddress;
    unsigned long ulEndAddress;
} CLONER_SKIP_REGION;

/* =================================================================== */
/*                 local function prototype declarations               */
/* =================================================================== */

#if defined EZPORT_CLONER                                                // {55}
    static void fnConfigEz(void);
    static void fnNegateResetEz(int iEzMode);
    static unsigned char fnGetStatusEz(int iDisplay);
    #if defined _M5223X
        static void fnPrepareEz(CHAR *ptrInput);
    #endif
    static void fnEraseEz(CHAR *ptrInput);
    static void fnCommandEz(CHAR *ptrInput);
    static void fnReadEz(CHAR *ptrInput);
    static void fnProgEz(CHAR *ptrInput);
    static void fnCopyEz(unsigned long ulWriteAddress, unsigned char *ptrSource, int iLength);
    static void fnGetEz(unsigned char *ptrBuffer, unsigned long ulReadAddress, int iLength);
#endif
#if defined USE_MAINTENANCE
    #if defined TEST_CLIENT_SERVER
        static int fnTestTCPServer(int iAction);
            #define TCP_TEST_SERVER_START    0
            #define START_SERVER_TEST        1
            #define TCP_SERVER_TX            2
            #define RESET_SERVER_MESSAGE     3
            #define INCREMENT_SERVER_MESSAGE 4
            #define TCP_TEST_SERVER_LISTEN   5
            #define CONNECT_REMOTE           6
            #define TCP_SERVER_REPEAT        7
            #define ALLOW_NEXT_MESSAGE       8
    #endif
    #if defined KEEP_DEBUG
        #if defined USE_TELNET
            static int  fnTELNETListener(USOCKET Socket, unsigned char ucEvent, unsigned char *ucIp_Data, unsigned short usPortLen);
            #if defined TEST_TCP_SERVER                                  // {30}
                static int fnServerTestListener(USOCKET Socket, unsigned char ucEvent, unsigned char *ucIp_Data, unsigned short usPortLen);
            #endif
        #endif
        static int  fnDisplayHelp(int iStart);
        #if defined USE_ICMP && defined ICMP_SEND_PING
            static void fnPingTest(USOCKET dummy_socket);
            #if defined ICMP_PING_IP_RESULT                              // {51}
                static void fnPingSuccess(USOCKET dummy_socket, unsigned char *ucIPv4);
            #else
                static void fnPingSuccess(USOCKET dummy_socket);
            #endif
        #endif
        #if defined USE_IPV6 && defined ICMP_SEND_PING
            #if defined ICMP_PING_IP_RESULT                              // {51}
                static void fnPingIPV6Success(USOCKET dummy_socket, unsigned char *ucIPv6);
            #else
                static void fnPingIPV6Success(USOCKET dummy_socket);
            #endif
            static void fnPingV6Test(USOCKET dummy_socket);
        #endif
        static void fnDoTelnet(unsigned char ucType, CHAR *ptrInput);
        #if defined SDCARD_SUPPORT || defined SPI_FLASH_FAT || defined FLASH_FAT || defined USB_MSD_HOST // {17}{81}
            static int  fnDoDisk(unsigned char ucType, CHAR *ptrInput);
            static void fnSendPrompt(void);
            static void fnDisplayDiskSize(unsigned long ulSectors, unsigned short usBytesPerSector); // {43}
        #endif
    #endif
    #if defined USE_TELNET_CLIENT
        static int fnTELNETClientListener(USOCKET Socket, unsigned char ucEvent, unsigned char *ucIp_Data, unsigned short usPortLen);
    #endif
    #if !defined REMOVE_PORT_INITIALISATIONS
        static void fnSetPortBit(unsigned short usBit, int iSetClr);
        static int  fnConfigOutputPort(CHAR cPortBit);
    #endif
    #if defined I2C_INTERFACE && defined I2C_MASTER_LOADER                   // {89}
        static void fnProgramI2CSlave(unsigned char ucSlaveAddress, int iCommand);
    #endif
#endif

/* =================================================================== */
/*                             constants                               */
/* =================================================================== */

#if defined USE_TELNET
    static const UTASK_TASK OurTask = OWN_TASK;
#endif

static const CHAR cDeleteInput[] = {DELETE_KEY, ' ', DELETE_KEY, 0};

static const DEBUG_COMMAND tMainCommand[] = {
    {"1",                 "Configure LAN interface",               DO_HELP,          DO_MENU_HELP_LAN },
    {"2",                 "Configure serial interface",            DO_HELP,          DO_MENU_HELP_SERIAL },
    {"3",                 "Go to I/O menu",                        DO_HELP,          DO_MENU_HELP_IO },
    {"4",                 "Go to administration menu",             DO_HELP,          DO_MENU_HELP_ADMIN },
    {"5",                 "Go to overview/statistics menu",        DO_HELP,          DO_MENU_HELP_STATS },
    {"6",                 "Go to USB menu",                        DO_HELP,          DO_MENU_HELP_USB },
    {"7",                 "Go to I2C menu",                        DO_HELP,          DO_MENU_HELP_I2C },
    {"8",                 "Go to utFAT disk interface",            DO_HELP,          DO_MENU_HELP_DISK }, // {17}
#if defined USE_MQTT_CLIENT
    {"9",                 "FTP/TELNET/MQTT client commands",       DO_HELP,          DO_MENU_HELP_FTP_TELNET_MQTT }, // {37}
#else
    {"9",                 "FTP/TELNET commands",                   DO_HELP,          DO_MENU_HELP_FTP_TELNET_MQTT },
#endif
    {"a",                 "CAN commands",                          DO_HELP,          DO_MENU_HELP_CAN }, // {38}
#if defined TEST_FLEXIO                                                  // {91}
    {"b",                 "FLEXIO",                                DO_HELP,          DO_MENU_HELP_FLEXIO },
#elif defined CMSIS_DSP_CFFT || defined CRYPTOGRAPHY || defined MMDVSQ_AVAILABLE // {84}{88}
    {"b",                 "Advanced commands",                     DO_HELP,          DO_MENU_HELP_ADVANCED },
#endif
    {"help",              "Display menu specific help",            DO_HELP,          DO_MAIN_HELP },
    {"quit",              "Leave command mode",                    DO_TELNET,        DO_TELNET_QUIT },
};

static const DEBUG_COMMAND tLANCommand[] = {
    {"up",                "go to main menu",                       DO_HELP,          DO_HELP_UP },
#if defined ETH_INTERFACE || defined USB_CDC_RNDIS || defined USE_PPP
    #if IP_NETWORK_COUNT > 1
    {"net",               "Set network reference (0 default)",     DO_IP,            DO_SET_NETWORK },
    #endif
    #if IP_NETWORK_COUNT > 1
    {"int",               "Set interface reference (0 default)",   DO_IP,            DO_SET_INTERFACE },
    #endif
    #if defined DEVELOP_PHY_CONTROL && defined ETHERNET_AVAILABLE        // {33}
        #if defined PHY_MULTI_PORT                                       // {56}
    {"phy_add",           "Set PHY address",                       DO_IP,            DO_PHY_ADDRESS },
        #endif
    {"phy",               "Dump PHY registers",                    DO_IP,            DO_DUMP_PHY },
    {"set_phy",           "<reg> <val>",                           DO_IP,            DO_SET_PHY },    
        #if defined PHY_MICREL_SMI                                       // {56}
    {"smi_r",             "Read SMI register <reg>",               DO_IP,            DO_READ_PHY_SMI },
    {"smi_w",             "Write SMI register <reg> <val>",        DO_IP,            DO_WRITE_PHY_SMI },
        #endif   
    #endif
    #if defined USE_DHCP_CLIENT
    {"set_dhcp",          "<enable/disable> DHCP",                 DO_SERVER,        DO_ENABLE_DHCP },
    #endif
    {"set_ip_add",        "Set IPV4 address",                      DO_IP,            DO_SET_IP_ADDRESS },
    #if defined USE_IPV6                                                 // {21}
    {"set_ipv6_add",      "Set IPV6 address",                      DO_IP,            DO_SET_IPV6_ADDRESS },
    #endif
    {"set_ip_mask",       "Set IP subnet mask",                    DO_IP,            DO_SET_IP_SUBNET_MASK },
    {"set_ip_gway",       "Set default gateway",                   DO_IP,            DO_SET_DEFAULT_GATEWAY },
    #if defined DNS_SERVER_OWN_ADDRESS                                   // {32}
    {"set_ip_dns",        "Set default DNS",                       DO_IP,            DO_SET_DEFAULT_DNS },
    #endif
    {"set_eth_speed",     "Set LAN speed (10/100/AUTO)",           DO_IP,            DO_SET_LAN_SPEED },
    {"show_config",       "Show network configuration",            DO_IP,            DO_SHOW_IP_CONFIG_TEMP },
    {"show_config_o",     "Show original network configuration",   DO_IP,            DO_SHOW_IP_CONFIG },
    #if defined USE_PARAMETER_BLOCK                                      // {5}
    {"save",              "Save configuration to FLASH",           DO_FLASH,         DO_SAVE_PARS },
    {"validate",          "Validate temporary config. in FLASH",   DO_FLASH,         DO_VALIDATE_NEW_PARS },
    #endif
    #if defined USE_ICMP && defined ICMP_SEND_PING
    {"ping",              "Ping test IP address",                  DO_IP,            PING_TEST },
    #endif
    #if defined USE_IPV6 && defined ICMP_SEND_PING                       // {21}
    {"pingv6",            "Ping test IPV6 address",                DO_IP,            PING_TEST_V6 },
    #endif
    {"arp -a",            "Request ARP table",                     DO_IP,            SHOW_ARP },
    {"arp -d",            "Delete ARP table",                      DO_IP,            DELETE_ARP },
    #if defined USE_IPV6                                                 // {21}
    {"nn -a",             "Request IPV6 neighbor table",           DO_IP,            SHOW_NN },
    {"nn -d",             "Delete IPV6 neighbor table",            DO_IP,            DELETE_NN },
    #endif
    #if defined USE_IGMP                                                 // {66}
    {"hosts",             "List multicast host groups",            DO_IP,            LIST_IGMP_HOSTS },
    {"join",              "[multicast] join group",                DO_IP,            JOIN_HOST_GROUP },
    {"leave",             "[host ID] leave group",                 DO_IP,            LEAVE_HOST_GROUP },
    #endif
    #if defined USE_IGMP || defined SUPPORT_MULTICAST_TX                 // {66}
    {"multi",             "<ip> send test frame",                  DO_IP,            SEND_MULTICAST },
    #endif
    {"help",              "Display menu specific help",            DO_HELP,          DO_MAIN_HELP },
#endif
    {"quit",              "Leave command mode",                    DO_TELNET,        DO_TELNET_QUIT },
};

#if defined SERIAL_INTERFACE || defined USB_INTERFACE
static const DEBUG_COMMAND tSERIALCommand[] = {
    {"up",                "go to main menu",                       DO_HELP,          DO_HELP_UP },
    #if defined SERIAL_INTERFACE
    {"set_baud",          "Set serial baud rate",                  DO_SERIAL,        SERIAL_SET_BAUD },
    {"set_stop",          "Set stop bits (1/1.5/2)",               DO_SERIAL,        SERIAL_SET_STOP },
    {"set_bits",          "Set data bits (7/8)",                   DO_SERIAL,        SERIAL_SET_DATA_LENGTH },
    {"set_par",           "Set parity (EVEN/ODD/NONE)",            DO_SERIAL,        SERIAL_SET_PARITY },
    {"set_flow",          "Set flow control (XON/RTS/NONE)",       DO_SERIAL,        SERIAL_SET_FLOW },
    {"set_high_water",    "Set flow stall (%) [1..99]",            DO_SERIAL,        SERIAL_SET_HIGH_WATER },
    {"set_low_water",     "Set flow restart (%) [1..99]",          DO_SERIAL,        SERIAL_SET_LOW_WATER },
    {"show_config",       "Show serial configuration",             DO_SERIAL,        SERIAL_SHOW_CONFIG },
        #if defined USE_PARAMETER_BLOCK                                       // {5}
    {"save",              "Save configuration to FLASH",           DO_FLASH,         DO_SAVE_PARS },
        #endif
    {"help",              "Display menu specific help",            DO_HELP,          DO_MAIN_HELP },
    #endif
    {"quit",              "Leave command mode",                    DO_TELNET,        DO_TELNET_QUIT },
};
#else
static const DEBUG_COMMAND tSERIALCommand[] = {
    {"up",                "go to main menu",                       DO_HELP,          DO_HELP_UP },
    {"quit",              "Leave command mode",                    DO_TELNET,        DO_TELNET_QUIT },
};
#endif

static const DEBUG_COMMAND tIOCommand[] = {
    {"up",                "go to main menu",                       DO_HELP,          DO_HELP_UP },
#if defined MEMORY_DEBUGGER                                              // {54}
    {"md",                "Memory Display [address] [<l>|<w>|<b>] [num]", DO_HARDWARE, DO_MEM_DISPLAY },
    {"mm",                "Memory Modify [address] [<l>|<w>|<b>] [val]", DO_HARDWARE, DO_MEM_WRITE },
    {"mf",                "Memory Fill [address] [<l>|<w>|<b>] [val] [num]", DO_HARDWARE, DO_MEM_FILL },
    #if !defined NO_FLASH_SUPPORT
    {"sd",                "Storage Display {as md}",               DO_HARDWARE,      DO_STORAGE_DISPLAY }, // {69}
    {"sm",                "Storage Modify {as mm}",                DO_HARDWARE,      DO_STORAGE_WRITE },
    {"sf",                "Storage Fill {as mf}",                  DO_HARDWARE,      DO_STORAGE_FILL },
        #if defined SPI_FILE_SYSTEM && defined SPI_FLASH_S25FL1_K
    {"sp",                "Storage Page (write page)",             DO_HARDWARE,      DO_STORAGE_PAGE }, // this command tests a single page buffer write with a predefined pattern
    {"ss",                "Suspend (delete page)",                 DO_HARDWARE,      DO_SUSPEND_PAGE }, // this command tests the suspend and resume operation
        #endif
    {"se",                "Storage Erase [address] [len-hex]",     DO_HARDWARE,      DO_STORAGE_ERASE }, // {74}
    #endif
#endif
#if !defined REMOVE_PORT_INITIALISATIONS
    {"set_ddr",           "Set port type [1..4] [<i>|<o>",         DO_HARDWARE,      DO_DDR },
    {"get_ddr",           "Get data direction [1..4]",             DO_HARDWARE,      DO_GET_DDR },
    {"read_port",         "Read port input [1..4]",                DO_HARDWARE,      DO_INPUT },
    {"write_port",        "Set port output [1..4] [0/1]",          DO_HARDWARE,      DO_OUTPUT },
#endif
#if defined SUPPORT_LCD && defined LCD_CONTRAST_CONTROL                  // {88}
    { "lcd_c",            "Show LCD contrast [0..100]%",           DO_HARDWARE,      DO_GET_CONTRAST },
    { "slcd_c",           "Set LCD contrast [0..100]%",            DO_HARDWARE,      DO_SET_CONTRAST },
#endif
#if defined GLCD_BACKLIGHT_CONTROL                                       // {75}
    {"sbl",               "Show backlight",                        DO_HARDWARE,      DO_GET_BACKLIGHT },
    {"blight",            "Set backlight [0..100]%",               DO_HARDWARE,      DO_SET_BACKLIGHT },
#endif
#if defined EZPORT_CLONER                                                // {55}
    {"ez_config",         "Prepare EzPort interface",              DO_HARDWARE,      DO_EZINIT },
    {"ez_reset",          "Reset target and enter EzPort mode",    DO_HARDWARE,      DO_EZRESET },
    {"ez_status",         "Read status",                           DO_HARDWARE,      DO_EZSTATUS }, 
    {"ez_start",          "Allow slave to start",                  DO_HARDWARE,      DO_EZSTART },
    {"ez_cmd",            "Send command [val]",                    DO_HARDWARE,      DO_EZCOMMAND },
    {"ez_rd",             "Write [add] <len>",                     DO_HARDWARE,      DO_EZREAD },
    #if defined _M5223X
    {"ez_prepare",        "Prepare for programming <speed>",       DO_HARDWARE,      DO_EZGETREADY },
    #else                                                                // kinetis
    {"ez_unsec",          "Unsecure",                              DO_HARDWARE,      DO_EZUNSECURE },
    #endif
    {"ez_sect",           "Erase sector [add]",                    DO_HARDWARE,      DO_EZSECT },
    {"ez_bulk",           "Perform Bulk erase",                    DO_HARDWARE,      DO_EZBULK },
    {"ez_prg",            "Program value [add] [val]",             DO_HARDWARE,      DO_EZPROG },
    {"ez_verif",          "Verify [<b(lank)>|<c(loned)>]",         DO_HARDWARE,      DO_EZVERIF },
    {"ez_clone",          "Clone software",                        DO_HARDWARE,      DO_EZCLONE },
    {"ez_s_clone",        "Securely Clone software",               DO_HARDWARE,      DO_EZSCLONE },
#endif
#if defined PWM_MEASUREMENT_DEVELOPMENT
    { "test",             "Temp test",                             DO_HARDWARE,      75 },
#endif
#if defined SUPPORT_LPTMR
    { "lp_cnt",           "Read LPTMR CNT",                        DO_HARDWARE,      76 },
#endif
#if defined DEV1
    { "set_an",           "Set anode [hex]",                       DO_HARDWARE,      77 },
    { "set_ca",           "Set cathode [hex]",                     DO_HARDWARE,      78 },
    { "relay",            "Relay <1..3> <0..1>",                   DO_HARDWARE,      79 },
#endif
#if defined USE_PARAMETER_BLOCK
    {"save",              "Save port setting as default",          DO_HARDWARE,      DO_SAVE_PORT },
#endif
    {"help",              "Display menu specific help",            DO_HELP,          DO_MAIN_HELP },
    {"quit",              "Leave command mode",                    DO_TELNET,        DO_TELNET_QUIT },
};

static const DEBUG_COMMAND tADMINCommand[] = {
    {"up",                "go to main menu",                       DO_HELP,          DO_HELP_UP },
#if defined ETH_INTERFACE || defined USB_CDC_RNDIS || defined USE_PPP
    {"set_ftp",           "[enable/disable] FTP server",           DO_SERVER,        DO_ENABLE_FTP },
    {"set_telnet",        "[enable/disable] Telnet service",       DO_SERVER,        DO_ENABLE_TELNET },
    {"set_web",           "[enable/disable] WEB server",           DO_SERVER,        DO_ENABLE_WEB_SERVER },
    {"set_web_auth",      "[enable/disable] WEB server authentication", DO_SERVER,   DO_HTTP_AUTH },
#endif
    {"show_config",       "Show configuration",                    DO_ADMIN,         DO_SHOW_CONFIG },
#if defined USE_PARAMETER_BLOCK                                          // {5}
    {"save",              "Save configuration to FLASH",           DO_FLASH,         DO_SAVE_PARS },
    {"reject",            "Reset non-saved changes",               DO_FLASH,         DO_REJECT_CHANGES },
    {"restore",           "Restore factory settings",              DO_FLASH,         DO_SET_DEFAULTS },
#endif
#if defined SUPPORT_RTC || defined SUPPORT_SW_RTC                        // {31}
    {"show_time",         "Display date/time",                     DO_ADMIN,         SHOW_TIME },
    {"set_time",          "Set time hh:mm:ss",                     DO_ADMIN,         SET_TIME },
    {"set_date",          "Set Date dd:mm:yyyy",                   DO_ADMIN,         SET_DATE },
    {"show_alarm",        "Display alarm d/t",                     DO_ADMIN,         SHOW_ALARM }, // {76}
    {"set_alarm",         "Set alarm (date)(+)[time]",             DO_ADMIN,         SET_ALARM }, // {76}
    {"del_alarm",         "Delete alarm",                          DO_ADMIN,         CLEAR_ALARM }, // {76}
#endif
#if defined DUSK_AND_DAWN                                                // {78}
	{"set_loc",           "Set locatin 50D46M30S, -6D5M3S",        DO_ADMIN,         SET_LOCATION },
	{"show_loc",          "Show location",                         DO_ADMIN,         SHOW_LOCATION },
	{"dark",              "Display darkness O,C,N,A",              DO_ADMIN,         DARK },
    {"dusk",              "Display dusk time O,C,N,A",             DO_ADMIN,         DUSK },
    {"dawn",              "Display dawn time O,C,N,A",             DO_ADMIN,         DAWN },
#endif
#if defined TEST_CLIENT_SERVER
    {"connect",           "Connect to server",                     DO_ADMIN,         DO_CONNECT },
    {"test",              "Start server test",                     DO_ADMIN,         DO_START },
#endif
#if defined SUPPORT_LOW_POWER
    {"show_lp",           "Show low power mode and options",       DO_HARDWARE,      DO_LP_GET }, // {71}
    {"set_lp",            "[option] Set low power mode",           DO_HARDWARE,      DO_LP_SET }, // {71}
    #if defined LOW_POWER_CYCLING_MODE
    { "lpc",              "low power cycling [1/0]",               DO_HARDWARE,      DO_LP_CYCLE }, // {85}
    #endif
#endif
    {"reset",             "Reset device",                          DO_HARDWARE,      DO_RESET },
    {"last_rst",          "Reset cause",                           DO_HARDWARE,      DO_LAST_RESET }, // {63}
    {"help",              "Display menu specific help",            DO_HELP,          DO_MAIN_HELP },
    {"quit",              "Leave command mode",                    DO_TELNET,        DO_TELNET_QUIT },
};

static const DEBUG_COMMAND tStatCommand[] = {
    {"up",                "go to main menu",                       DO_HELP,          DO_HELP_UP },
#if defined ETH_INTERFACE || defined USB_CDC_RNDIS || defined USE_PPP
    {"ipstat",            "Show Ethernet statistics",              DO_IP,            DO_SHOW_ETHERNET_STATS },
    {"r_ipstat",          "Reset Ethernet statistics",             DO_IP,            DO_RESET_ETHERNET_STATS },
#endif
    {"up_time",           "Show operating time",                   DO_HARDWARE,      DO_DISPLAY_UP_TIME },
    {"memory",            "Show memory use",                       DO_HARDWARE,      DO_DISPLAY_MEMORY_USE },
#if defined MONITOR_PERFORMANCE                                               // {25}
    {"tasks",             "Show task use",                         DO_HARDWARE,      DO_DISPLAY_TASK_USE },    
    {"rst_tasks",         "Reset task use measurement",            DO_HARDWARE,      DO_TASK_USE_RESET },
#endif
    {"help",              "Display menu specific help",            DO_HELP,          DO_MAIN_HELP },
    {"quit",              "Leave command mode",                    DO_TELNET,        DO_TELNET_QUIT },
};

static const DEBUG_COMMAND tUSBCommand[] = {
    {"up",                "go to main menu",                       DO_HELP,          DO_HELP_UP },
#if defined USB_INTERFACE                                                // {64}
    {"usb_errors",        "Show USB errors",                       DO_SERIAL,        SERIAL_SHOW_USB_ERRORS },
    {"usb_cnt_rst",       "Reset USB counters",                    DO_SERIAL,        SERIAL_RESET_USB_ERRORS },
    #if defined USB_CDC_HOST                                             // {82}
    {"virtcom",           "Open virtual COM connection",           DO_USB,           DO_VIRTUAL_COM },
    #endif
    #if defined USE_USB_CDC
        #if defined SERIAL_INTERFACE
    {"usb-serial",        "RS232<->USB mode (disconnect to quit)", DO_USB,           DO_USB_RS232_MODE },
        #endif
    {"usb-load",          "USB-SW download",                       DO_USB,           DO_USB_DOWNLOAD },
    {"usb-tx",            "Test tx speed (send 10MByte data)",     DO_USB,           DO_USB_SPEED },
    #endif
    #if defined USE_USB_HID_KEYBOARD && defined SUPPORT_FIFO_QUEUES
    {"usb-kb",            "Keyboard input (disconnect to quit)",   DO_USB,           DO_USB_KEYBOARD }, // {77}
        #if defined USB_KEYBOARD_DELAY
    {"kb-space",          "Keystroke delay (ms)",                  DO_USB,           DO_USB_KEYBOARD_DELAY },
        #endif
    #endif
    #if defined USE_USB_AUDIO
    {"delta",             "USB audio drift",                       DO_USB,           DO_USB_DELTA },
    #endif
    #if defined USE_USB_HID_KEYBOARD && defined SUPPORT_FIFO_QUEUES && defined USB_KEYBOARD_DELAY
    {"save",              "Save configuration to FLASH",           DO_FLASH,         DO_SAVE_PARS },
    #endif
#endif
    {"help",              "Display menu specific help",            DO_HELP,          DO_MAIN_HELP },
    {"quit",              "Leave command mode",                    DO_TELNET,        DO_TELNET_QUIT },
};

static const DEBUG_COMMAND tI2CCommand[] = {
    {"up",                "go to main menu",                       DO_HELP,          DO_HELP_UP },
#if defined I2C_INTERFACE
    {"acc_on",            "enable accelerometer output",           DO_I2C,           DO_ACC_ON }, // {68}
    {"acc_off",           "disable output",                        DO_I2C,           DO_ACC_OFF },
    #if LPI2C_AVAILABLE > 0 && defined TEMP_LPI2C_TEST
    {"tpause",            "test potential bug [1/0]",              DO_I2C,           122 },
    {"rpause",            "test potential bug [1/0]",              DO_I2C,           123 },
    {"dozen",             "DOZEN [1/0]",                           DO_I2C,           124 },
    {"dbgen",             "DBGEN [1/0]",                           DO_I2C,           125 },
    {"auto",              "AUTOSTOP [1/0]",                        DO_I2C,           126 },
    #endif
    #if defined TEST_I2C_INTERFACE
    {"wr",                "write [add] [value] {rep}",             DO_I2C,           DO_I2C_WRITE },
    {"rd",                "read  [add] [no. of bytes]",            DO_I2C,           DO_I2C_READ },
    {"srd",               "simple read [no. of bytes]",            DO_I2C,           DO_I2C_READ_NO_ADDRESS },
    {"rdq",               "read  [add] [no. of bytes] + wr",       DO_I2C,           DO_I2C_READ_PLUS_WRITE },
    #endif
    #if defined I2C_INTERFACE && defined I2C_MASTER_LOADER                // {89}
    {"load",              "Load firmware [add]",                   DO_I2C,           DO_I2C_LOAD_FIRMWARE },
    #endif
    {"help",              "Display menu specific help",            DO_HELP,          DO_MAIN_HELP },
#endif
    {"quit",              "Leave command mode",                    DO_TELNET,        DO_TELNET_QUIT },
};

static const DEBUG_COMMAND tDiskCommand[] = {                            // {17}
    {"up",                "go to main menu",                       DO_HELP,          DO_HELP_UP},
#if defined SDCARD_SUPPORT || defined SPI_FLASH_FAT || defined FLASH_FAT || defined USB_MSD_HOST // {81}
    #if DISK_COUNT > 1
    {"disk",              "select disk [C/D/E]",                   DO_DISK,          DO_DISK_NUMBER},
    #endif
    {"info",              "utFAT/card info",                       DO_DISK,          DO_INFO},
    {"dir",               "[path] show directory content",         DO_DISK,          DO_DIR},
    #if defined UTFAT_UNDELETE || defined UTFAT_EXPERT_FUNCTIONS         // {60}
    {"dird",              "[path] show deleted directory content", DO_DISK,          DO_DIR_DELETED},
    #endif
    #if defined UTFAT_EXPERT_FUNCTIONS                                   // {60}
    {"dirh",              "[path] show hidden content",            DO_DISK,          DO_DIR_HIDDEN},
  //{"dirc",              "[path] show corrupted directory content", DO_DISK,        DO_DIR_CORRUPTED},
    {"infof",             "[path] show file info",                 DO_DISK,          DO_INFO_FILE},
    {"infod",             "[path] show deleted info",              DO_DISK,          DO_INFO_DELETED},
    #endif
    {"cd",                "[path] change dir. (.. for up)",        DO_DISK,          DO_CHANGE_DIR},
    #if defined UTFAT_WRITE                                              // {45}
    {"file" ,             "[path] new empty file",                 DO_DISK,          DO_NEW_FILE},
    {"write",             "[path] test write to file",             DO_DISK,          DO_WRITE_FILE},
    {"mkdir" ,            "new empty dir",                         DO_DISK,          DO_NEW_DIR},
    {"rename" ,           "[from] [to] rename",                    DO_DISK,          DO_RENAME},
    {"trunc" ,            "truncate to [length] [path]",           DO_DISK,          DO_TEST_TRUNCATE}, // {59}
        #if defined UTFAT_EXPERT_FUNCTIONS                               // {83}
    {"hide",              "[path] file/dir to hide",               DO_DISK,          DO_WRITE_HIDE},
    {"unhide",            "[path] file/dir to un-hide",            DO_DISK,          DO_WRITE_UNHIDE},
    {"prot",              "[path] file/dir to write-protect",      DO_DISK,          DO_SET_PROTECT},
    {"unprot",            "[path] file/dir to un-protet",          DO_DISK,          DO_REMOVE_PROTECT},
        #endif
    #endif
    #if !defined LOW_MEMORY
    {"print",             "[path] print file content",             DO_DISK,          DO_PRINT_FILE },
    #endif
  //{"root",              "set root dir",                          DO_DISK,          DO_ROOT}, {19}
    #if defined UTFAT_WRITE                                              // {45}
    {"del",               "[path] delete file or dir.",            DO_DISK,          DO_DELETE},
        #if defined UTFAT_SAFE_DELETE                                    // {60}
    {"dels",              "[path] safe delete file or dir.",       DO_DISK,          DO_DELETE_SAFE},
        #endif
        #if defined UTFAT_UNDELETE && defined UTFAT_WRITE                // {60}
    {"undel",             "undelete [name]",                       DO_DISK,          DO_UNDELETE},
        #endif
        #if defined UTFAT_FORMATTING
    {"format",            "[-16/12] [label] format (unformatted) disk",DO_DISK,      DO_FORMAT}, // {26}
        #endif
        #if defined UTFAT_FULL_FORMATTING
    {"fformat",           "[-16/12] [label] full format (unformatted) disk",DO_DISK, DO_FORMAT_FULL}, // {26}   
        #endif
        #if defined UTFAT_FORMATTING
    {"re-format",         "[-16/12] [label] reformat disk!!!!!",      DO_DISK,       DO_REFORMAT}, // {26} 
        #endif
        #if defined UTFAT_FULL_FORMATTING
    {"re-fformat",        "[-16/12] [label] full reformat disk!!!!!", DO_DISK,       DO_REFORMAT_FULL },// {26}
        #endif
    #endif
        #if !defined LOW_MEMORY
    {"sect",              "[hex no.] display sector",              DO_DISK,          DO_DISPLAY_SECTOR},
            #if defined UTFAT_MULTIPLE_BLOCK_READ
    { "msect",            "[hex no.] display multi sector",        DO_DISK,          DO_DISPLAY_MULTI_SECTOR },
            #endif
        #endif
    #if defined UTFAT_WRITE
        #if defined NAND_FLASH_FAT
    {"secti",             "[hex number] display sector info",      DO_DISK,          DO_DISPLAY_SECTOR_INFO},
    {"del-remap",         "delete remapping table!!",              DO_DISK,          DO_DELETE_REMAP_INFO},
    {"del-FAT",           "delete FAT",                            DO_DISK,          DO_DELETE_FAT},
    {"page",              "[hex number] display physical page",    DO_DISK,          DO_DISPLAY_PAGE},
            #if defined VERIFY_NAND
    {"tnand",             "write test patterns to NAND",           DO_DISK,          DO_TEST_NAND},
    {"vnand",             "verify test patterns in NAND",          DO_DISK,          DO_VERIFY_NAND},
            #endif
        #elif defined TEST_SDCARD_SECTOR_WRITE
    {"sectw",             "[hex no.] [offset] [val] [cnt]",        DO_DISK,          DO_WRITE_SECTOR}, // {44}{86}
            #if defined UTFAT_MULTIPLE_BLOCK_WRITE
    {"sectmw",            "multi-block [dito]",                    DO_DISK,          DO_WRITE_MULTI_SECTOR},
                #if defined UTFAT_PRE_ERASE
    {"sectmwp",           "multi-block with pre-erase [dito]",     DO_DISK,          DO_WRITE_MULTI_SECTOR_PRE},
                #endif
            #endif
        #endif
    #endif
    {"help",              "Display menu specific help",            DO_HELP,          DO_MAIN_HELP},
#endif
    {"quit",              "Leave command mode",                    DO_TELNET,        DO_TELNET_QUIT},
};

static const DEBUG_COMMAND tFTP_TELNET_Command[] = {                     // {37}
    {"up",                "go to main menu",                       DO_HELP,          DO_HELP_UP },
#if defined USE_FTP_CLIENT
    {"show_config",       "Show FTP client settings",              DO_FTP_TELNET_MQTT, DO_SHOW_FTP_CONFIG },
    {"ftp_port",          "Set default FTP command port",          DO_FTP_TELNET_MQTT, DO_FTP_SET_PORT },
    {"ftp_ip",            "Set default FTP server IP",             DO_FTP_TELNET_MQTT, DO_FTP_SERVER_IP },
    {"ftp_user",          "Set default FTP user name",             DO_FTP_TELNET_MQTT, DO_FTP_USER_NAME },
    {"ftp_pass",          "Set default FTP user password",         DO_FTP_TELNET_MQTT, DO_FTP_USER_PASS },
    {"ftp_psv",           "<enable/disable> passive mode",         DO_FTP_TELNET_MQTT, DO_FTP_PASSIVE },
    {"ftp_tout",          "Set FTP connection idle timeout",       DO_FTP_TELNET_MQTT, DO_FTP_SET_IDLE_TIMEOUT },

    {"ftp_con",           "Connect to FTP server",                 DO_FTP_TELNET_MQTT, DO_FTP_CONNECT },
    #if defined USE_IPV6                                                 // {48}
    {"ftp_con6",          "Connect to FTP server over IPv6",       DO_FTP_TELNET_MQTT, DO_FTP_CONNECT_IPV6 },
    #endif
    {"ftp_path",          "Set directory location",                DO_FTP_TELNET_MQTT, DO_FTP_PATH },
    {"ftp_dir",           "Directory listing [path]",              DO_FTP_TELNET_MQTT, DO_FTP_DIR },
    {"ftp_mkd",           "Make directory <path/dir>",             DO_FTP_TELNET_MQTT, DO_FTP_MKDIR },
    {"ftp_get",           "Get binary file <path/file>",           DO_FTP_TELNET_MQTT, DO_FTP_GET },
    {"ftp_get_a",         "Get ASCII file <path/file>",            DO_FTP_TELNET_MQTT, DO_FTP_GETA },
    {"ftp_put",           "Put binary file <path/file>",           DO_FTP_TELNET_MQTT, DO_FTP_PUT },
    {"ftp_put_a",         "Put ASCII file <path/file>",            DO_FTP_TELNET_MQTT, DO_FTP_PUTA },
    {"ftp_app",           "Append to binary file <path/file>",     DO_FTP_TELNET_MQTT, DO_FTP_APP },
    {"ftp_app_a",         "Append to ASCII file <path/file>",      DO_FTP_TELNET_MQTT, DO_FTP_APPA },
    {"ftp_ren",           "Rename file or dir. <path/dir> <path/dir>", DO_FTP_TELNET_MQTT,DO_FTP_RENAME },
    {"ftp_del",           "Delete file <path/file>",               DO_FTP_TELNET_MQTT, DO_FTP_DEL },
    {"ftp_remove",        "Delete an empty dir. <path/dir>",       DO_FTP_TELNET_MQTT, DO_FTP_REMOVE_DIR },
    {"ftp_dis",           "Disconnect from FTP server",            DO_FTP_TELNET_MQTT, DO_FTP_DISCONNECT },
    #if defined USE_PARAMETER_BLOCK    
    {"save",              "Save configuration to FLASH",           DO_FLASH,         DO_SAVE_PARS },
    #endif
    #if !defined USE_TELNET_CLIENT && !defined USE_MQTT_CLIENT
    {"help",              "Display menu specific help",            DO_HELP,          DO_MAIN_HELP },
    #endif
#endif
#if defined USE_TELNET_CLIENT                                            // {72}
    #if defined TELNET_CLIENT_COUNT && (TELNET_CLIENT_COUNT > 1)
    {"tel_int",           "Set TELNET interface [num]",            DO_FTP_TELNET_MQTT, DO_TELNET_SET_INTERFACE },
    #endif
    {"tel_port",          "Set TELNET [port]",                     DO_FTP_TELNET_MQTT, DO_TELNET_SET_PORT },
    {"tel_con",           "Connect to TELNET server [ip]",         DO_FTP_TELNET_MQTT, DO_TELNET_CONNECT },
    {"tel_echo",          "Set echo mode [1/0]",                   DO_FTP_TELNET_MQTT, DO_TELNET_SET_ECHO },
    {"tel_neg",           "Disable negotiation [1/0]",             DO_FTP_TELNET_MQTT, DO_TELNET_SET_NEGOTIATION },
    {"show_tel",          "Show TELNET config",                    DO_FTP_TELNET_MQTT, DO_TELNET_SHOW },
    #if !defined USE_FTP_CLIENT && ! defined USE_MQTT_CLIENT
    {"help",              "Display menu specific help",            DO_HELP,          DO_MAIN_HELP },
    #endif
#endif
#if defined USE_MQTT_CLIENT                                              // {87}
    #if defined SECURE_MQTT
    { "mqtts_con",        "Secure con. to MQTT broker [ip]",       DO_FTP_TELNET_MQTT, DO_MQTTS_CONNECT },
    #endif
    { "mqtt_con",         "Connect to MQTT broker [ip]",           DO_FTP_TELNET_MQTT, DO_MQTT_CONNECT },
    { "mqtt_sub",         "Subscribe [topic] <QoS>",               DO_FTP_TELNET_MQTT, DO_MQTT_SUBSCRIBE },
    { "mqtt_top",         "List sub. topics",                      DO_FTP_TELNET_MQTT, DO_MQTT_TOPICS },
    { "mqtt_un",          "Unsubscribe [ref]",                     DO_FTP_TELNET_MQTT, DO_MQTT_UNSUBSCRIBE },
    { "mqtt_pub",         "Publish <\x22topic\x22/ref> <QoS>",    DO_FTP_TELNET_MQTT, DO_MQTT_PUB },
    { "mqtt_pub_l",       "Publish (long) <\x22topic\x22/ref> <QoS>", DO_FTP_TELNET_MQTT, DO_MQTT_PUB_LONG },
    { "mqtt_dis",         "Disconnect from MQTT broker",           DO_FTP_TELNET_MQTT, DO_MQTT_DISCONNECT },
    #if !defined USE_FTP_CLIENT && ! defined USE_TELNET_CLIENT
    {"help",              "Display menu specific help",            DO_HELP,          DO_MAIN_HELP },
    #endif
#endif
    {"quit",              "Leave command mode",                    DO_TELNET,        DO_TELNET_QUIT },
};

static const DEBUG_COMMAND tCANCommand[] = {                             // {38}
    {"up",                "go to main menu",                       DO_HELP,          DO_HELP_UP },
#if defined CAN_INTERFACE
    {"can",               "Send to default [ch] <data hex>",       DO_CAN,           DO_SEND_CAN_DEFAULT },
    {"can_s",             "Send to id [ch] <id> <data hex>",       DO_CAN,           DO_SEND_CAN_STANDARD },
    {"can_e",             "Send to ext. id [ch] <id> <data hex>",  DO_CAN,           DO_SEND_CAN_EXTENDED },
    {"can_r",             "Request remote default [ch]",           DO_CAN,           DO_REQUEST_CAN_DEFAULT },
    {"can_rs",            "Request remote id [ch] <id>",           DO_CAN,           DO_REQUEST_CAN_STANDARD },
    {"can_re",            "Request remote ext. id [ch] <id>",      DO_CAN,           DO_REQUEST_CAN_EXTENDED },
    {"can_f",             "Free remote rx when no response",       DO_CAN,           DO_CLEAR_REMOTE_BUF },
    {"can_d",             "Deposit remote message [ch] <data hex>", DO_CAN,          DO_SET_CAN_REMOTE },
    {"can_c",             "Clear remote message [ch]",             DO_CAN,           DO_CLEAR_CAN_REMOTE },
    #if defined _DEBUG_CAN
    {"can_dump",          "Dump CAN debug info [ch]",              DO_CAN,           DO_DEBUG_CAN },
    #endif
    {"help",              "Display menu specific help",            DO_HELP,          DO_MAIN_HELP },
#endif
    {"quit",              "Leave command mode",                    DO_TELNET,        DO_TELNET_QUIT },
};

#if defined TEST_FLEXIO                                                  // {91}
static const DEBUG_COMMAND tFlexioCommand[] = {
    {"up",                "go to main menu",                       DO_HELP,          DO_HELP_UP},
    {"flex_on",           "power up module",                       DO_HARDWARE,      DO_FLEXIO_ON},
    #if defined KINETIS_K80
    {"flex_pin",          "config pin [0..31]",                    DO_HARDWARE,      DO_FLEXIO_PIN},
    #else
    {"flex_pin",          "config pin [0..23]",                    DO_HARDWARE,      DO_FLEXIO_PIN},
    #endif
    #if defined KINETIS_KL28 || defined KINETIS_K80
    {"flex_in",           "display the pin state",                 DO_HARDWARE,      DO_FLEXIO_PIN_STATE},
    #endif
    {"flex_stat",         "display status",                        DO_HARDWARE,      DO_FLEXIO_STATUS},
    #if defined KINETIS_KL28
    {"show_trim",         "display trim setting",                  DO_HARDWARE,      DO_SHOW_TRIM},
    {"trim_c",            "coarse trim [0..0x3f]",                 DO_HARDWARE,      DO_CHANGE_TRIM_COARSE},
    {"trim_f",            "fine trim [0..0x7f]",                   DO_HARDWARE,      DO_CHANGE_TRIM_FINE },
    #endif
    {"quit",              "Leave command mode",                    DO_TELNET,        DO_TELNET_QUIT},
};

#elif defined CMSIS_DSP_CFFT || defined CRYPTOGRAPHY || defined MMDVSQ_AVAILABLE
static const DEBUG_COMMAND tAdvancedCommand[] = {                        // {84}
    {"up",                "go to main menu",                       DO_HELP,          DO_HELP_UP },
#if defined TEST_CMSIS_CFFT
    {"fft",               "Test CFFT [len (16|32|...|2048|4096)]", DO_HARDWARE,      DO_FFT},
#endif
#if defined CRYPTOGRAPHY
    #if defined CRYPTO_AES
    { "aes128",           "Test aes128",                           DO_HARDWARE,      DO_AES128 },
    { "aes192",           "Test aes192",                           DO_HARDWARE,      DO_AES192 },
    { "aes256",           "Test aes256",                           DO_HARDWARE,      DO_AES256 },
    #endif
    #if defined CRYPTO_SHA
    { "sha256",           "Test sha256",                           DO_HARDWARE,      DO_SHA256 },
    #endif
#endif
#if defined MMDVSQ_AVAILABLE                                             // {88}
    { "sqrt",             "Square root [<0xHEX><dec>]",            DO_HARDWARE,      DO_SQRT },
    { "div",              "Divide (signed) [<0xHEX><dec>] / [<0xHEX><dec>]", DO_HARDWARE, DO_DIV },
#endif
    {"quit",              "Leave command mode",                    DO_TELNET,        DO_TELNET_QUIT },
};
#endif


// Special secret menu - not displayed as menu...
//
static const DEBUG_COMMAND tSecretCommands[] = {
    {"MAC",               0,                 DO_IP,            DO_SET_MAC },
};

static const MENUS ucMenus[] = {
    { tMainCommand,        15, (sizeof(tMainCommand)/sizeof(DEBUG_COMMAND)),        "     Main menu"},
    { tLANCommand,         20, (sizeof(tLANCommand)/sizeof(DEBUG_COMMAND)),         " LAN configuration"},
    { tSERIALCommand,      22, (sizeof(tSERIALCommand)/sizeof(DEBUG_COMMAND)),      "   Serial config."},
    { tIOCommand,          15, (sizeof(tIOCommand)/sizeof(DEBUG_COMMAND)),          " Input/Output menu"},
    { tADMINCommand,       17, (sizeof(tADMINCommand)/sizeof(DEBUG_COMMAND)),       "   Admin. menu"},
    { tStatCommand,        17, (sizeof(tStatCommand)/sizeof(DEBUG_COMMAND)),        "   Stats. menu"},
    { tUSBCommand,         13, (sizeof(tUSBCommand)/sizeof(DEBUG_COMMAND)),         "    USB menu"},
    { tI2CCommand,         13, (sizeof(tI2CCommand)/sizeof(DEBUG_COMMAND)),         "    I2C menu"},
    { tDiskCommand,        13, (sizeof(tDiskCommand)/sizeof(DEBUG_COMMAND)),        "  Disk interface"}, // {17}
#if defined USE_MQTT_CLIENT
    { tFTP_TELNET_Command, 15, (sizeof(tFTP_TELNET_Command)/sizeof(DEBUG_COMMAND)), "FTP/TELNET/MQTT" }, // {37}
#else
    { tFTP_TELNET_Command, 15, (sizeof(tFTP_TELNET_Command)/sizeof(DEBUG_COMMAND)), "FTP/TELNET commands"}, // {37}
#endif
    { tCANCommand,         15, (sizeof(tCANCommand)/sizeof(DEBUG_COMMAND)),         "   CAN commands"}, // {38}
#if defined TEST_FLEXIO                                                  // {91}
    { tFlexioCommand,      15, (sizeof(tFlexioCommand)/sizeof(DEBUG_COMMAND)),      "   FLEXIO" },
#elif defined CMSIS_DSP_CFFT || defined CRYPTOGRAPHY || defined MMDVSQ_AVAILABLE
    { tAdvancedCommand,    15, (sizeof(tAdvancedCommand)/sizeof(DEBUG_COMMAND)),    "   Advanced" }, // {84}{88}
#endif
};

#if defined EZPORT_CLONER && defined EZPORT_CLONER_SKIP_REGIONS
    static const CLONER_SKIP_REGION ulSkipRegion[] = {
        {PARAMETER_BLOCK_START, (FLASH_START_ADDRESS + SIZE_OF_FLASH)},
        {0}                                                              // end of list
    };
#endif

/* =================================================================== */
/*                     global variable definitions                     */
/* =================================================================== */

#if defined I2C_INTERFACE                                                // {68}
    int iAccelOutput = 0;                                                // flag to control whether accelerometer output should be printed to the debug output or not
#endif
#if defined USE_MAINTENANCE || defined SERIAL_INTERFACE
    unsigned short usData_state = 0;
#endif

/* =================================================================== */
/*                      local variable definitions                     */
/* =================================================================== */

static int             iMenuLocation = 0;
static unsigned short  usTelnet_state = 0;
static unsigned char   ucMenu = 0;
static unsigned char   ucDebugCnt = 0;

#if IP_NETWORK_COUNT > 1
    static unsigned char ucPresentNetwork = DEFAULT_NETWORK;
#else
    #define ucPresentNetwork DEFAULT_NETWORK
#endif
#if IP_INTERFACE_COUNT > 1
    static unsigned char ucPresentInterface = DEFAULT_IP_INTERFACE;
#else
    #define ucPresentInterface DEFAULT_IP_INTERFACE
#endif
#if defined USE_MAINTENANCE || defined USE_USB_CDC                       // {8}
    #if defined USE_MAINTENANCE
        unsigned short usUSB_state = 0;
    #else
        unsigned short usUSB_state = ES_USB_RS232_MODE;
    #endif
#endif
#if defined USE_MAINTENANCE && defined KEEP_DEBUG
    #if defined USE_ICMP && defined ICMP_SEND_PING
        static unsigned char ucTempIP[IPV4_LENGTH];
    #endif
    #if defined USE_IPV6 && defined ICMP_SEND_PING
        static unsigned char ucPingTestV6[IPV6_LENGTH];
    #endif
#endif
#if defined USE_MAINTENANCE && defined TEST_TCP_SERVER                   // {30}
    static USOCKET     test_server_socket = -1;
#endif
#if defined USE_FTP_CLIENT                                               // {37}
    static USOCKET uFtpClientDataSocket = -1;
    int iFTP_data_state = 0;

    #if !defined FTP_CLIENT_BUFFERED_SOCKET_MODE && !defined FTP_CLIENT_EXTERN_DATA_SOCKET
        #define FTP_SIMPLE_DATA_SOCKET
        #define FTP_TX_BUFFER_MAX 8                                      // small buffer for collecting data to be sent (typing type input expected)

    FTP_MESSAGE FTP_tx[2];

    static unsigned char ucFTP_buffer_content[2] = {0};
    #endif
#endif
#if defined USE_TELNET
    static USOCKET     Telnet_socket = -1;
#endif
#if defined USE_TELNET_CLIENT                                            // {72}
    static unsigned char ucTelnetClientInterface = 0;                    // interface being controlled
    static TELNET_CLIENT_DETAILS telnet_client_details[TELNET_CLIENT_COUNT] = {{0}};
    static int iTELNET_clientActive = 0;
#endif
#if defined SERIAL_INTERFACE || (defined USE_TELNET && defined USE_TELNET_LOGIN)
    static unsigned char ucPasswordState = PASSWORD_IDLE;
#endif
#if defined SDCARD_SUPPORT || defined SPI_FLASH_FAT || defined FLASH_FAT || defined USB_MSD_HOST // {17}{81}
    static UTDIRECTORY *ptr_utDirectory[DISK_COUNT] = {0};               // pointer to a directory object
    static int iFATstalled = 0;                                          // stall flags when listing large directories and printing content
#endif
#if defined I2C_INTERFACE && (defined TEST_I2C_INTERFACE || defined I2C_MASTER_LOADER)
    extern QUEUE_HANDLE I2CPortID;
#endif
#if DISK_COUNT > 1
    static unsigned char ucPresentDisk = 0;
#else
    #define ucPresentDisk                0
#endif
#if defined EZPORT_CLONER
    static int iCloningActive = 0;
    static unsigned long ulCloningAddress = 0;
    #if defined EZPORT_CLONER_SKIP_REGIONS
        static int iCloningRegion = 0;
    #endif
#endif
#if defined USE_MQTT_CLIENT
    static unsigned short usPubLength = 0;
#endif



// Debug task
//
extern void fnDebug(TTASKTABLE *ptrTaskTable)
{
    QUEUE_HANDLE        PortIDInternal = ptrTaskTable->TaskID;           // queue ID for task input
    unsigned char       ucInputMessage[SMALL_MESSAGE];                   // reserve space for receiving messages

#if defined USE_TELNET
    if (Telnet_socket < 0) {                                             // if the TELNET server socket has not yet been obtained
        fnConfigureTelnetServer();
    }
#endif
#if defined USE_TELNET_CLIENT
    if (telnet_client_details[0].listener == 0) {                        // if the TELNET clients have not yet been configured with default settings
        int i;
        for (i = 0; i < TELNET_CLIENT_COUNT; i++) {                      // for each telnet client socket
            telnet_client_details[i].usPortNumber = TELNET_SERVERPORT;   // set default TELNET port number
            telnet_client_details[i].listener = fnTELNETClientListener;  // configure fixed listener
            telnet_client_details[i].usIdleTimeout = 120;                // set idle timeout to 120s
            telnet_client_details[i].usTelnetMode = TELNET_CLIENT_MODE_ECHO; // set echo mode as default
        }
    }
#endif
#if defined EZPORT_CLONER
    if (iCloningActive != 0) {
        unsigned char ucStatus = fnGetStatusEz(0);                       // poll the write in progress flag
        if ((ucStatus & EZCOMMAND_SR_WIP) == 0) {                        // no write in progress (not busy)
            // EzPort programming is not busy
            //
            if ((ucStatus & EZCOMMAND_SR_WEF) != 0) {                    // write error
                fnDebugMsg("Write error: ");
                fnDebugHex(ulCloningAddress, (sizeof(ulCloningAddress) | WITH_LEADIN));
                fnDebugHex(ucStatus, (sizeof(ucStatus) | WITH_LEADIN | WITH_SPACE | WITH_CR_LF));
                ulCloningAddress = (FLASH_START_ADDRESS + SIZE_OF_FLASH); // abort
            }
            else if (ulCloningAddress >= (FLASH_START_ADDRESS + SIZE_OF_FLASH)) { // final sector programmed/checked
                fnDebugMsg("\r\nCompleted");
                if (((CLONING_VERIFYING_BLANK | CLONING_VERIFYING_CODE | CLONING_VERIFYING_CLONED_CODE) & iCloningActive) == 0) {
                    iCloningActive |= (CLONING_VERIFYING_CODE | CLONING_VERIFYING_CLONED_CODE);
                    ulCloningAddress = (FLASH_START_ADDRESS);            // start verifying at the start of flash
        #if defined EZPORT_CLONER_SKIP_REGIONS
                    iCloningRegion = 0;                                  // reset region skipping
        #endif
                    fnDebugMsg(" - verifying\r\n");
                    return;
                }
                else if (((CLONING_VERIFYING_CLONED_CODE) & iCloningActive) != 0) {
                    fnNegateResetEz(0);                                  // allow the target to start running
                    fnDebugMsg(" - target starting");
                }
                uTaskerStateChange(OWN_TASK, UTASKER_STOP);              // remove polling mode
                fnDebugMsg("\r\n");
                iCloningActive = 0;
            }
            else {
                if ((iCloningActive & CLONING_ERASING_BULK) != 0) {
                    fnEraseEz("0");                                      // erase the first sector so that the security byte is erased
                    iCloningActive &= ~(CLONING_ERASING_BULK);
                    return;
                }
                if ((ulCloningAddress == 0x400) && (((CLONING_VERIFYING_BLANK | CLONING_VERIFYING_CLONED_CODE | CLONING_VERIFYING_CODE) & iCloningActive) == 0)) { // sector with flash configuration
                    unsigned char ucTemp[FLASH_PAGE_SIZE];
                    uMemcpy(ucTemp, fnGetFlashAdd((unsigned char *)0x400), FLASH_PAGE_SIZE); // the master's sector content
                    if (iCloningActive == DO_EZSCLONE) {                 // the target device will be secured
                        ucTemp[0x0c] = (0xff);                           // set security but allow mass erase
                        fnDebugMsg("Securing\r\n");
                    }
                    else {                                               // the target device will not be secured
                        ucTemp[0x0c] &= ~(0x03);
                        ucTemp[0x0c] |= FTFL_FSEC_SEC_UNSECURE;          // unsecure
                        fnDebugMsg("Unsecured\r\n");
                    }
                    ucTemp[0x0d] |= FTFL_FOPT_EZPORT_ENABLED;            // don't allow the EzPort to be disabled
                    fnCopyEz(0x400, ucTemp, FLASH_PAGE_SIZE);
                }
                else {
    #if defined EZPORT_CLONER_SKIP_REGIONS
                    if ((ulCloningAddress == ulSkipRegion[iCloningRegion].ulStartAddress) && ((CLONING_VERIFYING_BLANK & iCloningActive) == 0)) {
                        fnDebugMsg("\r\nRegion: ");
                        fnDebugHex(ulSkipRegion[iCloningRegion].ulStartAddress, (sizeof(ulSkipRegion[iCloningRegion].ulStartAddress) | WITH_LEADIN));
                        fnDebugMsg(" - ");
                        if (ulSkipRegion[iCloningRegion].ulEndAddress <= ulSkipRegion[iCloningRegion].ulStartAddress) {
                            ulCloningAddress = (FLASH_START_ADDRESS + SIZE_OF_FLASH);
                        }
                        else {
                            ulCloningAddress = ulSkipRegion[iCloningRegion].ulEndAddress;
                        }
                        fnDebugHex((ulCloningAddress - 1), (sizeof(ulCloningAddress) | WITH_LEADIN));
                        iCloningRegion++;
                        fnDebugMsg(" skipped\r\n");
                        return;
                    }
    #endif
                    if (((CLONING_VERIFYING_BLANK | CLONING_VERIFYING_CODE) & iCloningActive) != 0) {
                        unsigned char ucTemp[FLASH_PAGE_SIZE];
                        fnGetEz(ucTemp, ulCloningAddress, FLASH_PAGE_SIZE); // read a page of data from the device
                        if (iCloningActive & CLONING_VERIFYING_BLANK) {
                            int i = 0;
                            while (i < FLASH_PAGE_SIZE) {
                                if (ucTemp[i] != 0xff) {
                                    if (((ulCloningAddress + i) != 0x40c) && (ucTemp[0x0c] != 0xfe)) { // allow unsecure value in configuration flash
                                        fnDebugMsg("Not blank - address ");
                                        fnDebugHex((ulCloningAddress + i), (sizeof(ulCloningAddress) | WITH_LEADIN | WITH_CR_LF));
                                        ulCloningAddress = (FLASH_START_ADDRESS + SIZE_OF_FLASH); // cause termination
                                        return;
                                    }
                                }
                                i++;
                            }
                        }
                        else {
                            int iOffset = 0;
                            int iChecklength = FLASH_PAGE_SIZE;
                            unsigned long ulCheckAddress = ulCloningAddress;
                            if (ulCloningAddress == 0x400) {             // flash configuration
                                iOffset = 0x10;
                                iChecklength -= 0x10;
                                ulCheckAddress += 0x10;                  // skip checking the flash configuration area
                                fnDebugMsg("\r\nTarget ");
                                if ((ucTemp[0x0c] & 0x01) == 0) {
                                    fnDebugMsg("Not ");
                                }
                                fnDebugMsg("Secured\r\n");
                            }
                            if (uMemcmp(&ucTemp[iOffset], (unsigned char *)fnGetFlashAdd((unsigned char *)ulCheckAddress), iChecklength) != 0) {
                                fnDebugMsg("Mismatch - page ");
                                fnDebugHex(ulCloningAddress, (sizeof(ulCloningAddress) | WITH_LEADIN | WITH_CR_LF));
                                ulCloningAddress = (FLASH_START_ADDRESS + SIZE_OF_FLASH); // cause termination
                                return;
                            }
                        }
                    }
                    else {
                        fnCopyEz(ulCloningAddress, (unsigned char *)fnGetFlashAdd((unsigned char *)ulCloningAddress), FLASH_PAGE_SIZE);
                    }
                }
                fnDebugMsg(".");
                ulCloningAddress += FLASH_PAGE_SIZE;
            }
        }
    }
#endif

    while (fnRead(PortIDInternal, ucInputMessage, HEADER_LENGTH) != 0) { // check input queue
        switch (ucInputMessage[MSG_SOURCE_TASK]) {                       // switch depending on message source
#if defined USE_MAINTENANCE && defined USB_INTERFACE && defined USE_USB_CDC
        case TIMER_EVENT:
            if (E_TIMER_START_USB_TX == ucInputMessage[MSG_TIMER_EVENT]) {
                fnUSB_CDC_TX(1);                                         // start transmission of data to USB-CDC interface(s)
            }
            break;
#endif
        case INTERRUPT_EVENT:
            if (TX_FREE == ucInputMessage[MSG_INTERRUPT_EVENT]) {
#if defined SDCARD_SUPPORT || defined SPI_FLASH_FAT || defined FLASH_FAT || defined USB_MSD_HOST // {17}{81}
                if (iFATstalled != 0) {
                    if (STALL_DIR_LISTING == iFATstalled) {
                        fnDoDisk(DO_DIR, 0);                             // continue listing a directory content
                    }
    #if defined UTFAT_UNDELETE || defined UTFAT_EXPERT_FUNCTIONS
                    else if (STALL_DIRD_LISTING == iFATstalled) {
                        fnDoDisk(DO_DIR_DELETED, 0);                     // continue listing a directory's deleted content
                    }
    #endif
    #if defined UTFAT_EXPERT_FUNCTIONS
                    else if (STALL_DIRH_LISTING == iFATstalled) {
                        fnDoDisk(DO_DIR_HIDDEN, 0);                      // continue listing a directory's hidden content
                    }
    #endif
                    else if (STALL_PRINTING_FILE == iFATstalled) {
                        fnDoDisk(DO_PRINT_FILE, 0);
                    }
                }
#endif
                if (iMenuLocation != 0) {
                    fnDisplayHelp(iMenuLocation);
                }
#if defined USE_FTP_CLIENT                                               // {37}
                else if (iFTP_data_state & FTP_DATA_STATE_CRITICAL) {
    #if defined CONTROL_WINDOW_SIZE
                    fnReportTCPWindow(uFtpClientDataSocket, TX_BUFFER_SIZE); // report that the output buffer has space for more data
    #endif
                    iFTP_data_state &= ~FTP_DATA_STATE_CRITICAL;         // single-shot reporting
                }
#endif
            }
#if defined USE_MAINTENANCE && defined USB_INTERFACE && defined USE_USB_CDC
            else if (E_USB_TX_CONTINUE == ucInputMessage[MSG_INTERRUPT_EVENT]) {
                if (fnUSB_CDC_TX(0) != 0) {                              // continue transmission of data to USB-CDC interface(s)
                    return;
                }
            }
#endif
#if defined SDCARD_SUPPORT || defined SPI_FLASH_FAT || defined FLASH_FAT || defined USB_MSD_HOST // {81}
            else if (UTFAT_OPERATION_COMPLETED == ucInputMessage[MSG_INTERRUPT_EVENT]) {
                if (iFATstalled == STALL_COUNTING_CLUSTERS) {
                    const UTDISK *ptrDiskInfo = fnGetDiskInfo(ucPresentDisk);
                    if (ptrDiskInfo->utFileInfo.ulFreeClusterCount != 0xffffffff) {
                        fnDisplayDiskSize((ptrDiskInfo->utFileInfo.ulFreeClusterCount * ptrDiskInfo->utFAT.ucSectorsPerCluster), ptrDiskInfo->utFAT.usBytesPerSector); // {43}
                        fnDebugMsg(" free\r\n");
                        fnSendPrompt();
                    }
                    iFATstalled = 0;                                     // {27}
                }
            }
#endif
#if defined TEST_CLIENT_SERVER
            else if (TCP_SERVER_TEST == ucInputMessage[MSG_INTERRUPT_EVENT]) {
                fnTestTCPServer(TCP_SERVER_TX);                          // send next test frame
            }
#endif
            break;
#if defined ETH_INTERFACE || defined USB_CDC_RNDIS || defined USE_PPP
        case TASK_ARP:
            fnRead(PortIDInternal, ucInputMessage, ucInputMessage[MSG_CONTENT_LENGTH]);  // read the contents
            if (ucInputMessage[0] == ARP_RESOLUTION_SUCCESS) {
                // ARP has just resolved the address of our maintenance partner so we can resend the last message
                // This will not generally happen for server applications since we have just received a frame from the destination before sending ours
                //
                USOCKET socket;
                uMemcpy(&socket, &ucInputMessage[1], sizeof(socket));    // {50} extract the socket number from the received message
                fnDebugMsg("\n\rARP resolved\n\r");

                if ((socket & SOCKET_NUMBER_MASK) >= IPv6_DUMMY_SOCKET) {// {50}
    #if defined USE_IP && defined USE_IPV6
                    if ((socket & SOCKET_NUMBER_MASK) == IPv6_DUMMY_SOCKET) { // single IPV6 ping test being performed to a destination outside of the local subnet - the IPV4 gatewas has been resolved
        #if defined ICMP_SEND_PING
                        fnPingV6Test(socket);                            // repeat IPV6 ping test
        #endif
                    }
        #if defined USE_ICMP && defined ICMP_SEND_PING
                    else {
                        fnPingTest(socket);                              // it must be for our IPv4 ping test
                    }
        #endif
    #else
        #if defined USE_ICMP && defined ICMP_SEND_PING
                    fnPingTest(socket);                                  // it must be for our IPv4 ping test
        #endif
    #endif
                }            
    #if defined USE_TELNET
                else {
                    fnSendBufTCP(Telnet_socket, 0, 0, TCP_BUF_REP);      // must be for telnet
                }
    #endif
            }
            else if (ucInputMessage[0] == ARP_RESOLUTION_FAILED) {
                fnDebugMsg("\n\rARP resolution failed\n\r");             // we have learned that the partner MAC address could not be resolved...we ignore it in this case
            }
    #if defined USE_IPV6
            else if (ucInputMessage[0] == NN_RESOLUTION_SUCCESS) {
                USOCKET socket;
                uMemcpy(&socket, &ucInputMessage[1], sizeof(socket));    // {50} extract the socket number from the received message
                fnDebugMsg("\n\rNN resolved\n\r");
                if ((socket & SOCKET_NUMBER_MASK) == IPv6_DUMMY_SOCKET) {
        #if defined ICMP_SEND_PING
                    fnPingV6Test(socket);                                // it must be for our IPV6 ping test
        #endif
                }
            }
            else if (ucInputMessage[0] == NN_RESOLUTION_FAILED) {
                fnDebugMsg("\n\rNN resolution failed\n\r");              // we have learned that the partner MAC address could not be resolved...we ignore it in this case
            }
    #endif
            break;

    #if (defined USE_ICMP || defined USE_IPV6) && defined ICMP_SEND_PING
        case TASK_ICMP:
            {
                USOCKET socket;
                fnRead( PortIDInternal, ucInputMessage, ucInputMessage[MSG_CONTENT_LENGTH]); // read the contents
                uMemcpy(&socket, &ucInputMessage[1], sizeof(socket));    // {50} extract the socket number from the received message
        #if defined USE_ICMP && defined ICMP_SEND_PING
                if (ucInputMessage[0] == PING_RESULT) {
            #if defined ICMP_PING_IP_RESULT                              // {51}
                    fnPingSuccess(socket, &ucInputMessage[1 + sizeof(socket)]); // pass IPv4 address of answering node
            #else
                    fnPingSuccess(socket);
            #endif
                    break;
                }
        #endif
        #if defined USE_IPV6
                if (ucInputMessage[0] == PING_IPV6_RESULT) {
            #if defined ICMP_PING_IP_RESULT                              // {51}
                    fnPingIPV6Success(socket, &ucInputMessage[1 + sizeof(socket)]); // pass IPv4 address of answering node
            #else
                    fnPingIPV6Success(socket);
            #endif
                }
        #endif
            }
            break;
    #endif
#endif
        default:
            fnRead(PortIDInternal, ucInputMessage, ucInputMessage[MSG_CONTENT_LENGTH]); // flush any unexpected messages (assuming they arrived from another task)
            break;
        }
    }
#if defined I2C_INTERFACE
    #if defined TEST_I2C_INTERFACE
    if (fnMsgs(I2CPortID) != 0) {                                        // if I2C message waiting
        QUEUE_TRANSFER i2c_length = fnMsgs(I2CPortID);
        int x = 0;
        fnDebugMsg("I2C Input = ");
        fnRead(I2CPortID, ucInputMessage, i2c_length);
        while (i2c_length-- != 0) {
            fnDebugHex(ucInputMessage[x++], (WITH_LEADIN | WITH_SPACE | 1)); // display received bytes
        }
        fnDebugMsg("\r\n");
    }
    #elif defined I2C_MASTER_LOADER
    if (fnMsgs(I2CPortID) != 0) {                                        // if I2C message waiting
        QUEUE_TRANSFER i2c_length = fnMsgs(I2CPortID);                   // the waiting length
        fnRead(I2CPortID, ucInputMessage, i2c_length);
        #if defined _WINDOWS
        ucInputMessage[2] = 0x01;
        #endif
        if (3 == i2c_length) {                                           // delete status
            if (ucInputMessage[2] == 0x01) {                             // flass is empty
                fnDebugMsg("Slave flash deleted\r\n");
                fnProgramI2CSlave(0, 2);                                 // program the firmware
                fnDebugMsg("Programming");
            }
            else if (ucInputMessage[2] == 0x00) {                        // flash is not empty
                fnDebugMsg("*");
                fnProgramI2CSlave(0, 1);                                 // poll the delete status until the slave responds that it has completed
            }
            else {
                fnDebugMsg("Slave not responding!\r\n");
            }
        }
        else if (5 == i2c_length) {                                      // programming status
            fnDebugMsg(".");
            if (ucInputMessage[0] == 0x01) {
                fnProgramI2CSlave(0, 2);                                 // program next block
            }
            else {
                fnDebugMsg("Programming error!\r\n");
            }
        }
    }
    #endif
#endif
}

static void fnDisplaySerialNumber(void)
{
    CHAR cSN[19];                                                        // space for largest possible serial number with line feeds and null-terminator
    fnDebugMsg("\n\rSerial number: ");
    fnShowSN(cSN);
    fnDebugMsg(cSN);
}

static void fnDisplaySoftwareVersion(void)
{
    fnDebugMsg("Software version ");
    fnDebugMsg((CHAR *)cSoftwareVersion);
    fnDebugMsg("\n\r");
}

static void fnDisplayDeviceName(void)
{
    fnDebugMsg("Device identification: ");
    fnDebugMsg(temp_pars->temp_parameters.cDeviceIDName);
}



// Check whether the command input is correct - if it is, return pointer to rest of input (skipping white space before possible parameters)
//
static CHAR *fnCheckInput(CHAR *str1, CHAR *str2)
{
    while (*str2 != 0) {
        if (*str1++ != *str2++) return 0;                                // no match
    }
    if (*str1 > ' ') {                                                   // we don't accept the match because too short
        return 0;
    }
    while (*str1 == ' ') {                                               // jump over trailing white space
        str1++;
    }
    return str1;                                                         // return pointer to possible parameter
}

static int fnDisplayHelp(int iStart)
{
    const DEBUG_COMMAND *ptrCom = ucMenus[ucMenu].pMenu;
    unsigned char ucTab = ucMenus[ucMenu].ucTabSize;
    int iEntries = ucMenus[ucMenu].ucMenuLength;
    int iDisplayLen;
    unsigned char ucTabs[MAX_TAB_MARK];

    uMemset(ucTabs, ' ', MAX_TAB_MARK);

    if (iStart != 0) {
        while (iEntries > iStart) {
            ptrCom++;
            iEntries--;
        }
    }
    else {
        fnDebugMsg("\n\n\r");
        fnDebugMsg(ucMenus[ucMenu].cTitle);
        fnDebugMsg("\n\r===================\n\r");
    }

    while (iEntries-- != 0) {
        if (fnWrite(DebugHandle, 0, MAX_MENU_LINE) == 0) {               // the transmit buffer cannot accept more data so we will pause until it can
            fnDriver(DebugHandle, MODIFY_WAKEUP, (MODIFY_TX | OWN_TASK));// we want to be woken when the queue is free again
            iMenuLocation = (iEntries + 1);                              // save the position we stopped at
            return 1;
        }
        iDisplayLen = fnDebugMsg(ptrCom->strCommand);
        if (iDisplayLen >= ucTab) {
            fnWrite(DebugHandle, ucTabs, 1);
        }
        else {
            fnWrite(DebugHandle, ucTabs, (QUEUE_TRANSFER)(ucTab - iDisplayLen));
        }
        iDisplayLen = fnDebugMsg(ptrCom->strText);
        fnDebugMsg("\n\r");
        ptrCom++;
    }
    iMenuLocation = 0;
    return 0;
}

#if defined USE_PARAMETER_BLOCK                                          // {5}
static unsigned short fnGetWordShift(CHAR **ptrIn)
{
    unsigned short usResult = 0;
    unsigned char ucValue;
    CHAR *ptrInput = *ptrIn;
    int i = 4;

    ptrInput = fnSkipWhiteSpace(ptrInput);                               // jump over white space                  

    while ((i-- != 0) && (*ptrInput >= '0')) {
        usResult <<= 4;
        ucValue = *ptrInput++;
        ucValue -= '0';
        if (ucValue > 9) ucValue -= ('A' - '9' - 1);
        ucValue &= 0x0f;                                                 // makes previous line insensitive to case
        usResult |= ucValue;
        ++(*ptrIn);
    }

    return usResult;
}

static void fnDoFlash(unsigned char ucType, CHAR *ptrInput)
{
    unsigned short usParameter;
    unsigned char  ucValue;

    switch (ucType) {
    case DO_SET_VALUE:                                                   // set a parameter value
        usParameter = fnGetWordShift(&ptrInput);
        ucValue = (unsigned char)fnGetWordShift(&ptrInput);
        fnSetPar(usParameter, &ucValue, sizeof(ucValue));                // set parameter value to FLASH
        fnDebugMsg("\n\rParameter set\n\r");
        break;
    #if defined USE_PAR_SWAP_BLOCK
    case DO_SET_TEMP_VALUE:
        usParameter = fnGetWordShift(&ptrInput);
        ucValue = (unsigned char)fnGetWordShift(&ptrInput);
        fnSetPar((unsigned short)(usParameter | TEMPORARY_PARAM_SET), (unsigned char *)&ucValue, sizeof(ucValue)); // set temporary parameter value to FLASH
        fnDebugMsg("\n\rTemp. Parameter set\n\r");
        break;
    #endif
    case DO_GET_VALUE:                                                   // get a parameter value from FLASH
        usParameter = fnGetWordShift(&ptrInput);
        if ((fnGetPar(usParameter, &ucValue, sizeof(ucValue)) >= 0)) {   // get parameter value from FLASH
            fnDebugMsg("\n\rParameter value = ");
            fnDebugHex(ucValue, (WITH_CR_LF | sizeof(ucValue)));
        }
        else {
            fnDebugMsg("\n\rNo valid parameters available\n\r");
        }
        break;
    case DO_REJECT_CHANGES:
    #if defined ETH_INTERFACE || defined USB_CDC_RNDIS || defined USE_PPP
        uMemcpy(&temp_pars->temp_network[0], &network[0], sizeof(network)); // reverse all changes in the network settings
    #endif
        uMemcpy(&temp_pars->temp_parameters, parameters, sizeof(PARS));
        fnDebugMsg("\n\rOriginal parameters restored\n\r");
        break;

    case DO_SET_DEFAULTS:                                                // delete all parameter values (reset to 0xff)
        fnRestoreFactory();
        fnSaveNewPars(SAVE_NEW_PARAMETERS);
        fnDebugMsg("\n\rFactory parameters restored\n\r");
        break;

    case DO_INVALID_PARS:                                                // delete and invalidate all parameter values
    #if defined ACTIVE_FILE_SYSTEM
        fnDelPar(INVALIDATE_PARAMETER_BLOCK);
    #endif
        fnDebugMsg("\n\rParameters invalidated\n\r");
        break;
    #if defined USE_PAR_SWAP_BLOCK
    case DO_VALIDATE_NEW_PARS:                                           // delete and invalidate present invalidate and switch to new parameter block
        #if defined ACTIVE_FILE_SYSTEM
        fnDelPar(SWAP_PARAMETER_BLOCK);
        #endif
        fnDebugMsg("\n\rNew Parameters validated\n\r");
        break;
    #endif
    case DO_SAVE_PARS:                                                   // copy present parameters to FLASH and validate them
        fnSaveNewPars(SAVE_NEW_PARAMETERS);
        fnDebugMsg("\n\rConfiguration saved\n\r");
        break;
    }
}
#endif

#if defined USE_IP
static void fnDisplayMAC(unsigned char *ucMac)
{
    CHAR cBuf[18];

    fnMACStr(ucMac, cBuf);
    fnDebugMsg(cBuf);
}

extern void fnDisplayIP(unsigned char *ptrIP)
{
    CHAR cBuf[16];

    fnIPStr(ptrIP, cBuf);
    fnDebugMsg(cBuf);
}
#endif
#if defined USE_IPV6                                                     // {21}
static void fnDisplayIPV6(unsigned char *ptrIP)
{
    CHAR cBuf[MAX_IPV6_STRING];

    fnIPV6Str(ptrIP, cBuf);
    fnDebugMsg(cBuf);
}
#endif

#if defined USE_ICMP && defined ICMP_SEND_PING
    #if defined ICMP_PING_IP_RESULT                                      // {51}
static void fnPingSuccess(USOCKET dummy_socket, unsigned char *ucIPv4)
    #else
static void fnPingSuccess(USOCKET dummy_socket)
    #endif
{
    unsigned char *ping_address;

    if ((dummy_socket & SOCKET_NUMBER_MASK) == IPv4_DUMMY_SOCKET) {      // {50}
    #if defined ICMP_PING_IP_RESULT                                      // {51}
        ping_address = ucIPv4;                                           // display answering address
    #else
        ping_address = ucTempIP;                                         // assume response is from the pinged address
    #endif
    }
    else {
        fnDebugMsg("Unknown IPv4 PING response\r\n");
        return;
    }
    fnDebugMsg("PING response from ");
    fnDisplayIP(ping_address);
    fnDebugMsg("\r\n");
}

static void fnPingTest(USOCKET dummy_socket)
{
    unsigned char *ping_address;
    #if (defined ENC424J600_INTERFACE || defined PHY_MULTI_PORT) && (IP_INTERFACE_COUNT > 1)
    if (dummy_socket >= IPv4_DUMMY_SOCKET)
    #else
    if ((dummy_socket & SOCKET_NUMBER_MASK) == IPv4_DUMMY_SOCKET)        // {50}
    #endif
    {
        ping_address = ucTempIP;
    }
    else {
        return;                                                          // ignore if unexpected socket
    }
    if (NO_ARP_ENTRY == fnSendPing(ping_address, MAX_TTL, OWN_TASK, dummy_socket)) {
        fnDebugMsg("ARP resolution started\n\r");
    }
}
#endif

#if defined USE_IPV6 && defined ICMP_SEND_PING
    #if defined ICMP_PING_IP_RESULT                                      // {51}
static void fnPingIPV6Success(USOCKET dummy_socket, unsigned char *ucIPv6)
    #else
static void fnPingIPV6Success(USOCKET dummy_socket)
    #endif
{
    unsigned char *ping_address;

    if ((dummy_socket & SOCKET_NUMBER_MASK) == IPv6_DUMMY_SOCKET) {      // {50}
    #if defined ICMP_PING_IP_RESULT                                      // {51}
        ping_address = ucIPv6;                                           // display answering address
    #else
        ping_address = ucPingTestV6;                                     // assume response is from the pinged address
    #endif
    }
    else {
        fnDebugMsg("Unknown IPv6 PING response\r\n");
        return;
    }
    fnDebugMsg("IPV6 PING response from ");
    fnDisplayIPV6(ping_address);
    fnDebugMsg("\r\n");
}

static void fnPingV6Test(USOCKET dummy_socket)
{
    unsigned char *ping_IPV6_address;
    #if (defined ENC424J600_INTERFACE || defined PHY_MULTI_PORT) && (IP_INTERFACE_COUNT > 1)
    if (dummy_socket >= IPv6_DUMMY_SOCKET)
    #else
    if ((dummy_socket & SOCKET_NUMBER_MASK) == IPv6_DUMMY_SOCKET)        // {50}
    #endif
    {
        ping_IPV6_address = ucPingTestV6;
    }
    else {
        return;                                                          // ignore if unexpected socket
    }
    if (NO_ARP_ENTRY == fnSendV6Ping(ping_IPV6_address, MAX_TTL, OWN_TASK, dummy_socket)) {
        fnDebugMsg("NN resolution started\n\r");
    }
}
#endif

#if defined SERIAL_INTERFACE && defined DEMO_UART
#if defined SUPPORT_MIDI_BAUD_RATE
    #define NUMBER_OF_SPEEDS 13
#else
    #define NUMBER_OF_SPEEDS 12
#endif
static const CHAR cSpeeds[NUMBER_OF_SPEEDS][7] = {
    {"300"},
    {"600"},
    {"1200"},
    {"2400"},
    {"4800"},
    {"9600"},
    {"14400"},
    {"19200"},
    #if defined SUPPORT_MIDI_BAUD_RATE
    {"31250"},
    #endif
    {"38400"},
    {"57600"},
    {"115200"},
    {"230400"},
};

static void fnShowSpeed(void)
{
    if (temp_pars->temp_parameters.ucSerialSpeed < NUMBER_OF_SPEEDS) {
        fnDebugMsg((CHAR *)cSpeeds[temp_pars->temp_parameters.ucSerialSpeed]);
    }
    else {
        fnDebugMsg("invalid!");
    }
}
#endif

#if defined SERIAL_INTERFACE && defined DEMO_UART
static int fnGetSpeed(CHAR *ptr_input, unsigned char *ucNew_speed)
{
    int i = 0;
    while (i < NUMBER_OF_SPEEDS) {
        if ((uStrcmp((CHAR *)cSpeeds[i], ptr_input)) == 0) {
            *ucNew_speed = i;                                            // set corresponding speed index
            return 1;                                                    // return found
        }
        i++;
    }
    return 0;
}


#define NUMBER_OF_STOPS 3
static const CHAR cStops[NUMBER_OF_STOPS][4] = {
    {"1"},
    {"1.5"},
    {"2"},
};

static void fnShowStops(void)
{
    int i = 0;
    if ((temp_pars->temp_parameters.SerialMode & TWO_STOPS) != 0) {
        i = 2;
    }
    else if ((temp_pars->temp_parameters.SerialMode & ONE_HALF_STOPS) != 0) {
        i = 1;
    }
    fnDebugMsg((CHAR *)cStops[i]);
    fnDebugMsg(" stop bit(s)\r\n");
}

static int fnGetStops(CHAR *ptr_input, UART_MODE_CONFIG *Mode)
{
    int i = 0;
    while (i < NUMBER_OF_STOPS) {
        if ((uStrcmp((CHAR *)cStops[i], ptr_input)) == 0) {
            *Mode &= ~(ONE_HALF_STOPS | TWO_STOPS);                      // default 1 stop bit
            switch (i) {
            case 1:                                                      // 1,5 stop bits
                *Mode |= ONE_HALF_STOPS;
                break;

            case 2:
                *Mode |= TWO_STOPS;                                      // 2 stop bits
                break;
            }
            return 1;                                                    // return found
        }
        i++;
    }
    return 0;
}


#define NUMBER_OF_PARITY 3
static const CHAR cParity[NUMBER_OF_PARITY][5] = {
    {"ODD"},
    {"EVEN"},
    {"NONE"},
};

static void fnShowParity(void)
{
    int i = 2;
    if ((temp_pars->temp_parameters.SerialMode & RS232_EVEN_PARITY) != 0) {
        i = 1;
    }
    else if ((temp_pars->temp_parameters.SerialMode & RS232_ODD_PARITY) != 0) {
        i = 0;
    }
    fnDebugMsg((CHAR *)cParity[i]);
    fnDebugMsg(" parity\r\n");
}

static int fnGetParity(CHAR *ptr_input, UART_MODE_CONFIG *Mode)
{
    int i = 0;
    while (i < NUMBER_OF_PARITY) {
        if ((uStrcmp(cParity[i], ptr_input)) == 0) {
            *Mode &= ~(RS232_EVEN_PARITY | RS232_ODD_PARITY);            // default no parity
            switch (i) {
            case 0:                                                      // odd parity
                *Mode |= RS232_ODD_PARITY;
                break;

            case 1:                                                      // odd parity
                *Mode |= RS232_EVEN_PARITY;
                break;
            }
            return 1;                                                    // return found
        }
        i++;
    }
    return 0;
}

#define NUMBER_OF_FLOW 3
static const CHAR cFlow[NUMBER_OF_FLOW][8] = {
    {"NONE"},
    {"RTS"},
    {"XON"},
};

static void fnShowFlow(void)
{
    int i = 0;
    if ((temp_pars->temp_parameters.SerialMode & RTS_CTS) != 0) {
        i = 1;
    }
    else if ((temp_pars->temp_parameters.SerialMode & USE_XON_OFF) != 0) {
        i = 2;
    }
    fnDebugMsg("Flow control ");
    fnDebugMsg((CHAR *)cFlow[i]);
    fnDebugMsg("\r\n");
    if (i != 0) {
        fnDebugMsg("High water ");
        fnDebugDec(temp_pars->temp_parameters.ucFlowHigh, 0);
        fnDebugMsg("%\r\n");
        fnDebugMsg("Low water ");
        fnDebugDec(temp_pars->temp_parameters.ucFlowLow, 0);
        fnDebugMsg("%\r\n");
    }
}

static int fnSetFlowControl(CHAR *ptr_input, UART_MODE_CONFIG *Mode)
{

    int i = 0;
    while (i < NUMBER_OF_FLOW) {
        if ((uStrcmp(cFlow[i], ptr_input)) != 0) {
            *Mode &= ~(USE_XON_OFF | RTS_CTS);                           // default no flow control
            switch (i) {
            case 1:                                                      // RTS/CTS
                *Mode |= RTS_CTS;
                break;

            case 2:                                                      // XON/XOFF
                *Mode |= USE_XON_OFF;
                break;
            }
            return 1;                                                    // return found
        }
        i++;
    }
    return 0;
}
#endif

#define NO_ENABLE_MATCH 0
#define ENABLE_OPTION   1
#define DISABLE_OPTION  2
static int fnEnable(CHAR *ptrInput)
{
    if ((uStrcmp("enable", ptrInput)) == 0) {
        return ENABLE_OPTION;
    }
    if ((uStrcmp("disable", ptrInput)) == 0) {
        return DISABLE_OPTION;
    }
    fnDebugMsg("\r\nInvalid switch\r\n");
    return NO_ENABLE_MATCH;
}

static int fnEnableDisableServers(CHAR *ptr_input, unsigned short usOption) // {37} extend option to unsigned short
{
    int iEnable = fnEnable(ptr_input);

    if (iEnable == ENABLE_OPTION) {
        temp_pars->temp_parameters.usServers[ucPresentNetwork] |= usOption;
    }
    else if (iEnable == DISABLE_OPTION) {
        temp_pars->temp_parameters.usServers[ucPresentNetwork] &= ~usOption;
    }
    else {
        return 1;
    }
    return 0;
}

#if defined ETH_INTERFACE || defined USB_CDC_RNDIS || defined USE_PPP
static int fnShowServerEnabled(const CHAR *cText, unsigned char ucOption)
{
    fnDebugMsg((CHAR *)cText);
    fnDebugMsg(" - ");
    if (temp_pars->temp_parameters.usServers[ucPresentNetwork] & ucOption) {
        fnDebugMsg("enabled\r\n");
        return 1;
    }
    else {
        fnDebugMsg("disabled\r\n");
        return 0;
    }
}
#endif

#if defined SERIAL_INTERFACE && defined DEMO_UART
static void fnShowSerial(void)
{
    fnDebugMsg("\r\nSerial configuration\r\n");
    fnDebugMsg("Baud = ");
    fnShowSpeed();
    fnDebugMsg("\r\n");
    if (temp_pars->temp_parameters.SerialMode & CHAR_7) {
       fnDebugMsg("7");
    }
    else {
       fnDebugMsg("8");
    }
    fnDebugMsg(" Bits\r\n");
    fnShowParity();
    fnShowStops();
    fnShowFlow();
}
#endif

#if (defined SERIAL_INTERFACE && defined DEMO_UART) || defined USB_INTERFACE // {10}
static void fnDoSerial(unsigned char ucType, CHAR *ptr_input)
{
    #if (defined SERIAL_INTERFACE && defined DEMO_UART)
    int iChangeSerial = 0;
    #endif

    switch (ucType) {
    #if (defined SERIAL_INTERFACE && defined DEMO_UART)
    case SERIAL_SET_BAUD:
        if ((fnGetSpeed(ptr_input, &temp_pars->temp_parameters.ucSerialSpeed)) == 0) {
           fnDebugMsg("Incorrect serial speed\r\n");
           return;
        }
        iChangeSerial = 1;
        break;

    case SERIAL_SET_DATA_LENGTH:
        if (*ptr_input == '7') {
            temp_pars->temp_parameters.SerialMode |= CHAR_7;
        }
        else if (*ptr_input == '8') {
           temp_pars->temp_parameters.SerialMode &= ~CHAR_7;
        }
        else {
           fnDebugMsg("Incorrect data length\r\n");
           return;
        }
        iChangeSerial = 1;
        break;

    case SERIAL_SET_PARITY:
        if ((fnGetParity(ptr_input, &temp_pars->temp_parameters.SerialMode)) == 0) {
           fnDebugMsg("Incorrect parity\r\n");
           return;
        }
        iChangeSerial = 1;
        break;

    case SERIAL_SET_STOP:
        if ((fnGetStops(ptr_input, &temp_pars->temp_parameters.SerialMode)) == 0) {
           fnDebugMsg("Incorrect stop bits\r\n");
           return;
        }
        iChangeSerial = 1;
        break;

    case SERIAL_SET_FLOW:
        if ((fnSetFlowControl(ptr_input, &temp_pars->temp_parameters.SerialMode)) == 0) {
           fnDebugMsg("Incorrect flow control\r\n");
           return;
        }
        iChangeSerial = 1;
        break;

    case SERIAL_SHOW_CONFIG:
        fnShowSerial();
        return;

    case SERIAL_SET_HIGH_WATER:
    case SERIAL_SET_LOW_WATER:
        {
            unsigned char ucValue = (unsigned char)fnDecStrHex(ptr_input);
            if (ucValue > 99) {
                ucValue = 99;
            }
            else if (ucValue == 0) {
                ucValue = 1;
            }
            if (ucType == SERIAL_SET_HIGH_WATER) {
                temp_pars->temp_parameters.ucFlowHigh = ucValue;
            }
            else {
                temp_pars->temp_parameters.ucFlowLow  = ucValue;
            }
            iChangeSerial = 1;
        }
        break;
    #endif
    #if defined USB_INTERFACE                                            // {64}
    case SERIAL_SHOW_USB_ERRORS:
        fnDebugMsg("USB Error Counters\r\n");
        fnDebugMsg("- CRC-16 errors:");
        fnDebugDec(fnUSB_error_counters(USB_CRC_16_ERROR_COUNTER), (WITH_SPACE | WITH_CR_LF));
        fnDebugMsg("- CRC-5 errors :");
        fnDebugDec(fnUSB_error_counters(USB_CRC_5_ERROR_COUNTER), (WITH_SPACE | WITH_CR_LF));
        break;
    case SERIAL_RESET_USB_ERRORS:
        fnUSB_error_counters(USB_ERRORS_RESET);
        fnDebugMsg("USB Error Counters reset\r\n");
        break;
    #endif

    default:
        return;
    }
    #if (defined SERIAL_INTERFACE && defined DEMO_UART)
    if (iChangeSerial != 0) {
        fnSetNewSerialMode(MODIFY_CONFIG);                               // if there were changes, modify interface
    }
    #endif
}
#endif

#if defined ETH_INTERFACE || defined USB_CDC_RNDIS || defined USE_PPP
    #if defined USE_IP_STATS
static const CHAR cIPStatTypes[TOTAL_OTHER_EVENTS + 1][19] = {
    {"Total Rx frames"},
    {"Rx overruns"},
    {"Rx ARP"},
    #if defined USE_IGMP
    {"Rx IGMP"},
    #endif
    {"Rx ICMP"},
    {"Rx UDP"},
    {"Rx TCP"},
    {"Rx checksum errors"},
    {"Rx other protocols"},
    {"Foreign Rx ARP"},
    {"Foreign Rx ICMP"},
    {"Foreign Rx UDP"},
    {"Foreign Rx TCP"},
    {"Total Tx frames"},
    {"ARP sent"},
    #if defined USE_IGMP
    {"IGMP sent"},
    #endif
    {"ICMP sent"},
    {"UDP sent"},
    {"TCP sent"},
    {"Other events"}
};
#endif

#define NUMBER_OF_LAN 3
static const CHAR cLAN[NUMBER_OF_LAN][5] = {
    {"10"},
    {"100"},
    {"AUTO"},
};

    #if defined USE_IP
static void fnShowLAN(NETWORK_PARAMETERS *ptrNetwork)
{
    int i = 2;
    if (ptrNetwork->usNetworkOptions & LAN_10M) {
        i = 0;
    }
    else if (ptrNetwork->usNetworkOptions & LAN_100M) {
        i = 1;
    }
    fnDebugMsg("LAN speed ");
    fnDebugMsg((CHAR *)cLAN[i]);
    fnDebugMsg("\r\n");
        #if defined USE_DHCP_CLIENT
    fnShowServerEnabled("DHCP_SERVER", ACTIVE_DHCP);
        #endif
}
    #endif
    #if defined ETH_INTERFACE
static int fnSetLAN(CHAR *ptr_input, unsigned short *usMode)
{
    int i = 0;
    while (i < NUMBER_OF_LAN) {
        if (uStrcmp(cLAN[i], ptr_input) == 0) {
            *usMode &= ~(LAN_10M | LAN_100M);                            // default auto-negotiation
            switch (i) {
            case 0:                                                      // 10M fixed
                *usMode |= LAN_10M;
                break;

            case 1:                                                      // 100M fixed
                *usMode |= LAN_100M;
                break;
            }
            return 1;                                                    // return found
        }
        i++;
    }
    return 0;
}
    #endif
#endif

#if defined USE_IP
static void fnDisplayArpEntries(void)                                    // {47}
{
    unsigned char ucEntry;
    ARP_TAB *ptrEntry;

    fnDebugMsg("\r\nARP Table contents:");

    for (ucEntry = 0; ucEntry < ARP_TABLE_ENTRIES; ucEntry++) {
        ptrEntry = fnGetARPentry(ucEntry);
        if (ptrEntry != 0) {
            fnDebugMsg("\r\nIP address: ");
            fnDisplayIP(ptrEntry->ucIP);
            fnDebugMsg(" MAC address: ");
            fnDisplayMAC(ptrEntry->ucMac);
        }
    }
    fnDebugMsg("\r\nEnd\r\n");
}

static void fnIpConfig(NETWORK_PARAMETERS *ptrNetwork)
{
    fnDebugMsg("\r\nIPV4 address = ");
    fnDisplayIP(&ptrNetwork->ucOurIP[0]);
    #if defined USE_IPV6                                                 // {21}
    fnDebugMsg("\r\nIPV6 link-local address = ");                        // {46}
    fnDisplayIPV6(ucLinkLocalIPv6Address[ucPresentNetwork]);
    fnDebugMsg("\r\nIPV6 global address = ");
    fnDisplayIPV6(&ptrNetwork->ucOurIPV6[0]);
    #endif
    fnDebugMsg("\r\nMAC address = ");
    fnDisplayMAC(&ptrNetwork->ucOurMAC[0]);
    fnDebugMsg("\r\nSubnet mask = ");
    fnDisplayIP(&ptrNetwork->ucNetMask[0]);
    fnDebugMsg("\r\nDefault gateway = ");
    fnDisplayIP(&ptrNetwork->ucDefGW[0]);
    #if defined DNS_SERVER_OWN_ADDRESS                                   // {32}
    fnDebugMsg("\r\nDNS server = ");
    fnDisplayIP(&ptrNetwork->ucDNS_server[0]);
    #endif
    fnDebugMsg("\r\n");
}
#endif

#if defined USE_IPV6
static void fnDisplayNeighborEntries(void)
{
    unsigned char ucEntry;
    unsigned char *ptrEntry;

    fnDebugMsg("\r\nIPV6 neighbor Table contents:");

    for (ucEntry = 0; ucEntry < NEIGHBOR_TABLE_ENTRIES; ucEntry++) {
        ptrEntry = fnGetNeighborEntry(ucEntry, GET_IP);
        if (ptrEntry != 0) {
            fnDebugMsg("\r\nIPV6 address: ");
            fnDisplayIPV6(ptrEntry);
            fnDebugMsg(" MAC address: ");
            ptrEntry = fnGetNeighborEntry(ucEntry, GET_MAC);
            fnDisplayMAC(ptrEntry);
        }
    }
    fnDebugMsg("\r\nEnd\r\n");
}
#endif

#if defined USE_IGMP                                                     // {66}
static void fnMulticastProcess(int iHostID, unsigned short usSourcePort, unsigned short usRemotePort, unsigned char *ptrBuf, unsigned short usDataLen)
{
    fnDebugMsg("Mult-Rx:");
    fnDebugHex((unsigned short)iHostID, (WITH_SPACE | WITH_LEADIN | sizeof(unsigned short))); // display the host group/process ID
    fnDebugHex(*ptrBuf, (WITH_SPACE | WITH_LEADIN | WITH_CR_LF | sizeof(*ptrBuf))); // display first byte of content
}
#endif

static void fnDoIP(unsigned char ucType, CHAR *ptr_input)
{
#if defined ETH_INTERFACE && defined DEVELOP_PHY_CONTROL && defined PHY_MULTI_PORT // {56}
    #if !defined PHY_ADDRESS
        #define PHY_ADDRESS 0x01                                         // default if not defined by user
    #endif
    static unsigned char phy_add = PHY_ADDRESS;
#endif
    switch (ucType) {
#if defined USE_ICMP && defined ICMP_SEND_PING
    case PING_TEST:
        fnStrIP(ptr_input, ucTempIP);                                    // ping entered address
    #if defined ENC424J600_INTERFACE && (IP_INTERFACE_COUNT > 1)
        fnPingTest(ETHERNET_INTERFACE | ENC424J00_INTERFACE | IPv4_DUMMY_SOCKET); // ping on both Ethernet interfaces
    #elif defined PHY_MULTI_PORT && (IP_INTERFACE_COUNT > 1)
        fnPingTest(ETHERNET_INTERFACE | PHY1_INTERFACE | PHY2_INTERFACE | PHY12_INTERFACE | IPv4_DUMMY_SOCKET); // ping on all Ethernet interfaces
    #elif defined USE_PPP && defined ETH_INTERFACE
        fnPingTest(ETHERNET_INTERFACE | PPP_INTERFACE | IPv4_DUMMY_SOCKET); // ping on both Ethernet and PPP interfaces
    #else
        fnPingTest(IPv4_DUMMY_SOCKET);                                   // {50} execute using dummy IPv4 socket
    #endif
        return;
#endif
#if defined USE_IP
    case DO_SET_IP_ADDRESS:
        fnStrIP(ptr_input, &temp_pars->temp_network[ucPresentNetwork].ucOurIP[0]); // set device's IP address to the temporary set
        break;
#endif
#if defined ETH_INTERFACE && defined ETHERNET_AVAILABLE && defined DEVELOP_PHY_CONTROL // {33}
    #if defined PHY_MULTI_PORT                                           // {56}
        #if defined PHY_MICREL_SMI
	case DO_READ_PHY_SMI:
		{
            unsigned char ucReg = (unsigned char)fnHexStrHex(&ptr_input[0]); // register to be read (0x00..0xc6)
            fnDebugMsg("reg-value = ");
            fnDebugHex(_fnMIIread(0, ucReg), (1 | WITH_LEADIN | WITH_CR_LF));
		}
		break;
	case DO_WRITE_PHY_SMI:
		{
            unsigned char reg_add = (unsigned char)fnHexStrHex(&ptr_input[0]); // register address to write to (0x00..0xc6)
            unsigned char ucValue = (unsigned char)fnHexStrHex(&ptr_input[2]); // value to write
            _fnMIIwrite(0, reg_add, ucValue);
		}
		break;
        #endif
    case DO_PHY_ADDRESS:
        {           
            phy_add = (unsigned char)fnHexStrHex(&ptr_input[0]);         // address of PHY
            if ((phy_add < 0) || (phy_add > 4)) {
                phy_add = 1;
            }
            fnDebugMsg("PHY add = ");
            fnDebugHex(phy_add, (1 | WITH_LEADIN | WITH_CR_LF));
        }
        break;
    #endif
    case DO_DUMP_PHY:
        {
    #if defined _DP83849I                                                // {58}
        #define PHY_REGISTER_COUNT 12
    #else
        #define PHY_REGISTER_COUNT 13
    #endif
            int i = 0;
            unsigned char ucReg;
            for (i = 0; i < PHY_REGISTER_COUNT; i++) {
                fnDebugMsg("Reg: ");
                ucReg = (unsigned char)i;
    #if defined _DP83849I                                                // {58}
                switch (ucReg) {
                case 0x08:
                    ucReg = 0x10;
                    break;
                case 0x09:
                    ucReg = 0x11;
                    break;
                case 0x0a:
                    ucReg = 0x12;
                    break;
                case 0x0b:
                    ucReg = 0x13;
                    break;
                }
    #else
                switch (ucReg) {
                case 0x09:
                    ucReg = 0x15;
                    break;
                case 0x0a:
                    ucReg = 0x1b;
                    break;
                case 0x0b:
                    ucReg = 0x1e;
                    break;
                case 0x0c:
                    ucReg = 0x1f;
                    break;
                }
    #endif
                fnDebugHex(ucReg, (1 | WITH_LEADIN));
                fnDebugMsg(" = ");
    #if defined PHY_MULTI_PORT
                fnDebugHex(_fnMIIread(phy_add, ucReg), (2 | WITH_LEADIN | WITH_CR_LF));
    #else
                fnDebugHex(_fnMIIread(ucReg), (2 | WITH_LEADIN | WITH_CR_LF));
    #endif
            }
        }
        break;
    case DO_SET_PHY:
        {
            unsigned char reg_add = (unsigned char)fnHexStrHex(&ptr_input[0]); // register address to write to
            unsigned short usValue = (unsigned short)fnHexStrHex(&ptr_input[2]); // value to write
    #if defined _DP83849I                                                // {58}
            switch (reg_add) {
            case 0x08:
                reg_add = 0x10;
                break;
            case 0x09:
                reg_add = 0x11;
                break;
            case 0x0a:
                reg_add = 0x12;
                break;
            case 0x0b:
                reg_add = 0x13;
                break;
            }
    #else
            switch (reg_add) {
            case 0x09:
                reg_add = 0x15;
                break;
            case 0x0a:
                reg_add = 0x1b;
                break;
            case 0x0b:
                reg_add = 0x1e;
                break;
            case 0x0c:
                reg_add = 0x1f;
                break;
            }
    #endif
    #if defined PHY_MULTI_PORT
            _fnMIIwrite(phy_add, reg_add, usValue);
    #else
            _fnMIIwrite(reg_add, usValue);
    #endif
        }
        break;
#endif
#if defined USE_IPV6
    case DO_SET_IPV6_ADDRESS:
    case PING_TEST_V6:
        {
            unsigned char ucTestIPV6[IPV6_LENGTH];                       // extract the address locally in case it has syntax errors
            if (fnStrIPV6(ptr_input, ucTestIPV6) != 0) {
                if (ucType == PING_TEST_V6) {
    #if defined ICMP_SEND_PING
                    uMemcpy(&ucPingTestV6, ucTestIPV6, IPV6_LENGTH);     // save the IP address to be tested
        #if defined ENC424J600_INTERFACE && (IP_INTERFACE_COUNT > 1)
                    fnPingV6Test(ETHERNET_INTERFACE | ENC424J00_INTERFACE | IPv6_DUMMY_SOCKET); // ping on both Ethernet interfaces
        #else
                    fnPingV6Test(IPv6_DUMMY_SOCKET);                     // {50} start an IPV6 ping test
        #endif
    #endif
                }
                else {
                    uMemcpy(&temp_pars->temp_network[ucPresentNetwork].ucOurIPV6[0], ucTestIPV6, IPV6_LENGTH); // set the new temporary IPV6 address
                }
            }
            else {
                fnDebugMsg("Invalid IPV6 entry!\r\n");
            }
        }
        break; 
#endif
#if defined ETH_INTERFACE || defined USB_CDC_RNDIS || defined USE_PPP
    case DO_SET_MAC:                                                     // set device's MAC address
        if (((uMemcmp(&network[ucPresentNetwork].ucOurMAC[0], cucNullMACIP, MAC_LENGTH))) || ((fnSetMAC(ptr_input, &temp_pars->temp_network[ucPresentNetwork].ucOurMAC[0])) == 0)) {                                       // interpret MAC address input and save this
            fnDebugMsg("MAC may not be modified!!\r\n");                 // we are only allowed to save the new address once
            return;
        }
    #if defined USE_PARAMETER_BLOCK                                      // {5}
        else {
    #if (defined _KINETIS && defined SUPPORT_PROGRAM_ONCE && defined MAC_FROM_USER_REG) // {K1}
            unsigned long ulTestBuffer[2];
            ulTestBuffer[1] = 0xffffffff;
            uMemcpy(ulTestBuffer, temp_pars->temp_network[DEFAULT_NETWORK].ucOurMAC, MAC_LENGTH); // retrieve first MAC address from storage
            fnProgramOnce(PROGRAM_ONCE_WRITE, ulTestBuffer, 0, 2);       // save to the first 2 long words in the program once area
    #else
            fnDoFlash(DO_SAVE_PARS, 0);                                  // save the new MAC address
    #endif
            fnResetBoard();                                              // reset so that it starts with the new values
        }
    #endif
        break;
#endif
#if defined USE_IP
    case DO_SET_IP_SUBNET_MASK:                                          // set device's subnet mask
        fnStrIP(ptr_input, &temp_pars->temp_network[ucPresentNetwork].ucNetMask[0]);
        break;

    case DO_SET_DEFAULT_GATEWAY:                                         // set device's default gateway
        fnStrIP(ptr_input, &temp_pars->temp_network[ucPresentNetwork].ucDefGW[0]);
    #ifndef DNS_SERVER_OWN_ADDRESS                                       // {32}
        fnStrIP(ptr_input, &temp_pars->temp_network[ucPresentNetwork].ucDNS_server[0]);    // DNS server address follows default gateway address
    #endif
        break;

    #if defined DNS_SERVER_OWN_ADDRESS                                   // {32}
    case DO_SET_DEFAULT_DNS:                                             // set device's DNS server address
        fnStrIP(ptr_input, &temp_pars->temp_network[ucPresentNetwork].ucDNS_server[0]);
        break;
    #endif

    case DO_SHOW_IP_CONFIG_TEMP:                                         // show the present working IP configuration
        fnIpConfig(&temp_pars->temp_network[ucPresentNetwork]);
        fnShowLAN(&temp_pars->temp_network[ucPresentNetwork]);
        break;

    case DO_SHOW_IP_CONFIG:                                              // show the temporary working IP configuration which may be saved
        fnIpConfig(&network[ucPresentNetwork]);
        fnShowLAN(&network[ucPresentNetwork]);
        break;
#endif
#if defined USE_IP_STATS
    case DO_RESET_ETHERNET_STATS:                                        // {13}
        fnDeleteEthernetStats(ucPresentNetwork);
        break;

    case DO_SHOW_ETHERNET_STATS:                                         // show Ethernet statistic counters
        fnDebugMsg("\r\nEthernet Statistics\r\n");
        for (ucType = 0; ucType <= TOTAL_OTHER_EVENTS; ucType++) {
            fnDebugMsg("\r\n");
            fnDebugMsg((CHAR *)cIPStatTypes[ucType]);
            fnDebugMsg(" = ");
            fnDebugDec(fnGetEthernetStats(ucType, ucPresentNetwork), 0); // {67}
        }
        fnDebugMsg("\r\n");
        return;
#endif
#if defined ETH_INTERFACE
    case DO_SET_LAN_SPEED:
        if ((fnSetLAN(ptr_input, &temp_pars->temp_network[ucPresentNetwork].usNetworkOptions)) == 0) {
           fnDebugMsg("Incorrect LAN speed\r\n");
           return;
        }
        break;
#endif
#if defined USE_IP
    case SHOW_ARP:
        fnDisplayArpEntries();                                           // display present ARP table contents
        return;

    case DELETE_ARP:                                                     // delete the ARP table contents
        fnDeleteArp();
        break;
#endif
#if defined USE_IPV6
    case SHOW_NN:
        fnDisplayNeighborEntries();                                      // display present IPV6 neighbor table contents
        return;

    case DELETE_NN:                                                      // delete the IPV6 neighbor table contents
        fnDeleteNeighbors();
        break;
#endif
#if defined USE_IGMP                                                     // {66}
    case LIST_IGMP_HOSTS:
        fnReportIGMP();
        break;
    case JOIN_HOST_GROUP:
        {
            int iHostGroupID;
            unsigned char ucMulticastIP[IPV4_LENGTH];
            fnStrIP(ptr_input, ucMulticastIP);                           // extract multicast address
            iHostGroupID = fnJoinMulticastGroup(ucMulticastIP, (USOCKET)(defineNetwork(ucPresentNetwork) | defineInterface(ucPresentInterface)), fnMulticastProcess);  // join a multicast group
            if (iHostGroupID < 0) {
                fnDebugMsg("Error: ");
                switch (iHostGroupID) {
                case ERROR_IGMP_HOST_PROCESSES_EXHAUSED:
                    fnDebugMsg("No process space!\r\n");
                    break;
                case ERROR_IGMP_HOSTS_EXHAUSED:
                    fnDebugMsg("No host space!\r\n");
                    break;
                case ERROR_IGMP_NETWORK_NOT_AVAILABLE:
                    fnDebugMsg("Network not (yet) available!\r\n");
                    break;
                case ERROR_IGMP_INVALID_MULT_ADDRESS:
                    fnDebugMsg("Invalid address!\r\n");
                    break;
                }
            }
            else {
                fnDebugMsg("Joined as host/processor ID: ");
                fnDebugHex((unsigned short)iHostGroupID, (WITH_SPACE | WITH_LEADIN | WITH_CR_LF | sizeof(unsigned short)));
            }
        }
        break;
    case LEAVE_HOST_GROUP:
        {
            int iHostGroupID = (int)fnHexStrHex(&ptr_input[0]);
            switch (fnLeaveMulticastGroup(iHostGroupID)) {               // leave the group
            case ERROR_IGMP_ILLEGAL_HOST_ID:
                fnDebugMsg("Illegal host ID!\r\n");
                break;
            case ERROR_IGMP_NON_EXISTANT_HOST_ID:
                fnDebugMsg("Unknown host ID!\r\n");
                break;
            case IGMP_GROUP_FREED:
                fnDebugMsg("Group left!\r\n");
                break;
            case IGMP_PROCESS_REMOVED:
                fnDebugMsg("Process freed\r\n");
                break;
            }
        }
        break;
#endif
#if defined USE_IGMP || defined SUPPORT_MULTICAST_TX                     // {66}
    case SEND_MULTICAST:
        {
            typedef struct stMULTICAST_MESSAGE
            {
                UDP_HEADER     tUDP_Header;                              // reserve header space
                unsigned char  data_content[10];                         // reserve message space
            } MULTICAST_MESSAGE;
            MULTICAST_MESSAGE msg;                                       // temporary UDP message
            UDP_TX_PROPERTIES udp_properties;                            // {73} temporary UDP properties object
            unsigned char ucMulticastIP[IPV4_LENGTH];
            uMemset(msg.data_content, 0x55, sizeof(msg.data_content));   // test message content
            fnStrIP(ptr_input, ucMulticastIP);                           // extract multicast address
            udp_properties.ucIPAddress = ucMulticastIP;
            udp_properties.SocketHandle = (USOCKET)(defineNetwork(ucPresentNetwork) | defineInterface(ucPresentInterface));
            udp_properties.ucOptions = (UDP_OPT_SEND_CS/* | UDP_OPT_NO_LOOPBACK*/);
            udp_properties.ucTOS = TOS_MINIMISE_DELAY;
            udp_properties.ucTTL = 1;
            udp_properties.usDestinationPort = udp_properties.usSourcePort = 0x1234;
            if (fnSendUDP_multicast(0, &udp_properties, (unsigned char *)&msg, sizeof(msg.data_content)) == BAD_MULTICAST_ADDRESS) { // {73} send to multicast address
                fnDebugMsg("IP address not multicast\n\r!");
            }
        }
        break;
#endif
#if IP_NETWORK_COUNT > 1
    case DO_SET_NETWORK:                                                 // allow the referenced network to be changed
        ucPresentNetwork = (unsigned char)fnDecStrHex(ptr_input);
        if (ucPresentNetwork >= IP_NETWORK_COUNT) {
            ucPresentNetwork = (IP_NETWORK_COUNT - 1);
        }
        break;
#endif
#if IP_INTERFACE_COUNT > 1
    case DO_SET_INTERFACE:                                               // allow the referenced interface to be changed
        ucPresentInterface = (unsigned char)fnDecStrHex(ptr_input);
        if (ucPresentInterface >= IP_INTERFACE_COUNT) {
            ucPresentInterface = (IP_INTERFACE_COUNT - 1);
        }
        break;
#endif
    }
}


#if defined DUSK_AND_DAWN
static CHAR * fStrchr(const char *s, int c)
{
	do {
		if (*s == (char)c) return ((char *)s);
	} while (*s++);
	return (0);
}

static void fnShowTimeDouble(double t)
{
	CHAR cTimeBuf[10];
	CHAR *cTime;

	cTime = cTimeBuf;
	cTime = fnBufferDec((long)t, LEADING_ZERO, cTime);
	*cTime++ = ':';

	t = (t - (long)t) * 60.0;
	if ((t - (long)t) > 0.5)
		t += 1.0;
	cTime = fnBufferDec((long)t, LEADING_ZERO, cTime);
	*cTime++ = '\0';
	fnDebugMsg(cTimeBuf);
	fnDebugMsg("\r\n");
}

static void fnShowTimeIntSecs(int t)
{
	CHAR cTimeBuf[10];
	CHAR *cTime;

	cTime = cTimeBuf;
	cTime = fnBufferDec((t/3600), LEADING_ZERO, cTime);
	*cTime++ = ':';
	cTime = fnBufferDec((t % 3600)/60, LEADING_ZERO, cTime);
	*cTime++ = '\0';
	fnDebugMsg(cTimeBuf);
	fnDebugMsg("\r\n");
}

static void fnShowCoordinates(COORDINATE *coord)
{
	CHAR cBuf[100];
	CHAR *c;

	c = cBuf;
	c = fnBufferDec(coord->scDegree, DISPLAY_NEGATIVE, c);
	*c++ = 'D';
	c = fnBufferDec(coord->ucMinute, 0, c);
	*c++ = 'M';
	c = fnBufferDec(coord->ucSecond, 0, c);
	*c++ = 'S';
	*c++ = '\0';
	fnDebugMsg(cBuf);
}

static int fnGetCoordinates(COORDINATE *coord, CHAR *ptrInput)
{
	// Input: 50D46M30S or 50° 46' 30''
	CHAR *pInput = ptrInput;

	pInput = fStrchr(ptrInput, 'D');
	if (pInput)
	{
		*pInput++ = '\0';
		coord->scDegree = (int)fnDecStrHex(ptrInput);  // Degree
		ptrInput = pInput;
		pInput = fStrchr(ptrInput, 'M');
		if (pInput)
		{
			*pInput++ = '\0';
			coord->ucMinute = (int)fnDecStrHex(ptrInput);  // Minute
			ptrInput = pInput;
			pInput = fStrchr(ptrInput, 'S');
			if (pInput)
			{
				*pInput++ = '\0';
				coord->ucSecond = (int)fnDecStrHex(ptrInput);  // Second
			}
			else
			{
				coord->ucSecond = 0;
				pInput = ptrInput;
			}
			while (*pInput == ' ') *pInput++;
            return 1;
		}
	}
	return 0;
}

static const CHAR* str_dawn[] = {"Official Sunrise ", "Civil twilight starts ", "Nautical twilight starts ", "Astronomical twilight starts "};
static const CHAR* str_dusk[] = {"Official Sunset ", "Civil twilight ends ", "Nautical twilight ends ", "Astronomical twilight ends "};
#endif // defined USE_TWILIGHT

static void fnDoAdmin(unsigned char ucType, CHAR *ptrInput)
{
    switch (ucType) {

    case DO_SHOW_CONFIG:
        fnDisplaySerialNumber();
        fnDisplaySoftwareVersion();
        fnDisplayDeviceName();
        break;

    case DO_SET_DEVICE_NAME:
        uMemcpy(temp_pars->temp_parameters.cDeviceIDName, ptrInput, (sizeof(temp_pars->temp_parameters.cDeviceIDName)-1));
        break;

#if defined TEST_CLIENT_SERVER
    case DO_CONNECT:                                                     // connect to remote server and send test frames
        fnTestTCPServer(CONNECT_REMOTE);                                 // connect to a remote client
        break;

    case DO_START:
        fnTestTCPServer(TCP_SERVER_TX);                                  // start server test
        break;
#endif
#if defined SUPPORT_RTC || defined SUPPORT_SW_RTC                        // {31}
    case SHOW_ALARM:
    case SHOW_TIME:
        {
            int iType;
            CHAR cTimeDateBuf[20];
            if (ucType == SHOW_ALARM) {
                iType = DISPLAY_RTC_ALARM;
            }
            else {
                iType = DISPLAY_RTC_TIME_DATE;
            }
            fnSetShowTime(iType, cTimeDateBuf);                          // {34}
            fnDebugMsg(cTimeDateBuf);
            fnDebugMsg("\r\n");
        }
        break;
    case SET_ALARM:                                                      // {76}
        fnSetShowTime(SET_RTC_ALARM_TIME, ptrInput);
        fnDebugMsg("New alarm set\r\n");
        fnDoAdmin(SHOW_ALARM, 0);
        break;
    case CLEAR_ALARM:
        fnSetShowTime(SET_RTC_ALARM_TIME, 0);                            // delete alarm before it fires
        fnDebugMsg("Alarm cleared\r\n");
        break;
    case SET_TIME:
        fnSetShowTime(SET_RTC_TIME, ptrInput);                           // {34}
        fnDebugMsg("New time set\r\n");
        fnDoAdmin(SHOW_TIME, 0);
        break;
    case SET_DATE:
        fnSetShowTime(SET_RTC_DATE, ptrInput);
        fnDebugMsg("New date set\r\n");
        fnDoAdmin(SHOW_TIME, 0);
        break;
#endif
#if defined DUSK_AND_DAWN
    case SET_LOCATION:
        {
			// Input: 50D46M30S N, 6D5M3S E
			CHAR *pInput = ptrInput;
			LOCATION location;

			pInput = fStrchr(ptrInput, ',');
			if (pInput)
			{
				*pInput++ = '\0';
				// latitude
				if (fnGetCoordinates(&location.latitude, ptrInput))
				{
					// longitude
					if (fnGetCoordinates(&location.longitude, pInput))
					{
						uMemcpy(&temp_pars->temp_parameters.ourLocation, &location, sizeof(LOCATION));
						uMemcpy(&parameters->ourLocation, &location, sizeof(LOCATION));
						break;
					}
				}
			}
			fnDebugMsg("Error setting position\r\n");
        }
        break;
    case SHOW_LOCATION:
        {

			fnDebugMsg("Coordinates ");
			fnShowCoordinates(&temp_pars->temp_parameters.ourLocation.latitude);
			fnDebugMsg(", ");
			fnShowCoordinates(&temp_pars->temp_parameters.ourLocation.longitude);
			fnDebugMsg("\r\n");
        }
        break;
    case DARK:
        {
			enum twilight tw = c2tw(ptrInput[0]);
			double t_until_next;

			if (tw != UNKNOWN)
			{
				t_until_next = dark(tw);
				if (t_until_next > 0)
				{
		            fnDebugMsg("It is dark now. ");
					fnDebugMsg((CHAR*)str_dawn[tw-1]);
					fnDebugMsg("in: ");
					fnShowTimeDouble(t_until_next);
				}
				else if (t_until_next < 0)
				{
		            fnDebugMsg("It is day now. ");
					fnDebugMsg((CHAR*)str_dusk[tw-1]);
					fnDebugMsg("in: ");
					fnShowTimeDouble(-1 * t_until_next);
				}
				else
				{
		            fnDebugMsg("Error.\r\n");
				}
			}
			else
			{
				fnDebugMsg("Error in parameter!");
			}
        }
        break;
    case DUSK:
        {
			enum twilight tw = c2tw(ptrInput[0]);
			if (tw != UNKNOWN)
			{
				fnDebugMsg((CHAR*)str_dusk[tw-1]);
				fnDebugMsg("at: ");
				fnShowTimeDouble(dusk(tw));
			}
			else
			{
				fnDebugMsg("Error in parameter!");
			}
        }
        break;
    case DAWN:
        {
			enum twilight tw = c2tw(ptrInput[0]);
			if (tw != UNKNOWN)
			{
				fnDebugMsg((CHAR*)str_dawn[tw-1]);
				fnDebugMsg("at: ");
				fnShowTimeDouble(dawn(tw));
			}
			else
			{
				fnDebugMsg("Error in parameter!");
			}
        }
        break;
#endif
    }
}

#if defined ETH_INTERFACE || defined USB_CDC_RNDIS || defined USE_PPP
static void fnShowSec(void)
{
    fnDebugMsg("\r\nSecurity settings\r\n");
    fnShowServerEnabled("Terminal menu login", ACTIVE_LOGIN);
    fnShowServerEnabled("FTP server", ACTIVE_FTP_SERVER);
    if (fnShowServerEnabled("WEB server", ACTIVE_WEB_SERVER)) {
        fnShowServerEnabled("WEB server authentication", AUTHENTICATE_WEB_SERVER);
    }
    if (fnShowServerEnabled("TELNET server", ACTIVE_TELNET_SERVER)) {
        fnDebugMsg("   Telnet port number = ");
        fnDebugDec(temp_pars->temp_parameters.usTelnetPort, WITH_CR_LF);
    }
}
#endif

static void fnDoServer(unsigned char ucType, CHAR *ptrInput)
{
    switch (ucType) {
#if defined ETH_INTERFACE || defined USB_CDC_RNDIS || defined USE_PPP
    #if defined USE_TELNET
    case DO_ENABLE_TELNET:
        if (fnEnableDisableServers(ptrInput, ACTIVE_TELNET_SERVER)) {
            return;
        }
        fnConfigureTelnetServer();
        break;
    #endif
    case DO_ENABLE_DHCP:
        if (fnEnableDisableServers(ptrInput, ACTIVE_DHCP)) {
            return;
        }
        break;

    case DO_ENABLE_FTP:
        if (fnEnableDisableServers(ptrInput, ACTIVE_FTP_SERVER)) {
            return;
        }
        fnConfigureFtpServer(FTP_TIMEOUT);                               // activate / deactivate server {3}
        break;
#endif

    case DO_ENABLE_LOGIN:
        if (fnEnableDisableServers(ptrInput, ACTIVE_LOGIN)) {
            return;
        }
        break;
#if defined ETH_INTERFACE || defined USB_CDC_RNDIS || defined USE_PPP
    case DO_ENABLE_WEB_SERVER:
        if (fnEnableDisableServers(ptrInput, ACTIVE_WEB_SERVER)) {
            return;
        }
    #if defined USE_HTTP
        fnConfigureAndStartWebServer();
    #endif
        break;

    case DO_FTP_PASSIVE:
        if (fnEnableDisableServers(ptrInput, PASSIVE_MODE_FTP_CLIENT)) {
            return;
        }
        break;

    case DO_HTTP_AUTH:
        if (fnEnableDisableServers(ptrInput, AUTHENTICATE_WEB_SERVER)) {
            return;
        }
    #if defined USE_HTTP
        fnConfigureAndStartWebServer();
    #endif
        break;

    case SHOW_SECURITY:
        fnShowSec();
        break;
#endif
    default:
        return;
    }
}

static void fnDoTelnet(unsigned char ucType, CHAR *ptrInput)
{
    switch (ucType) {
      case DO_SET_TELNET_PORT:
          temp_pars->temp_parameters.usTelnetPort = (unsigned short)fnDecStrHex(ptrInput);
          break;

      case DO_TELNET_QUIT:
          if (usTelnet_state == ES_NETWORK_COMMAND_MODE){
              fnDebugMsg("\n\rBye Bye\n\r");
              usTelnet_state = ES_TERMINATE_CONNECTION;                  // now quit the Telnet session
          }
          else {
#if defined USE_MAINTENANCE || defined USE_USB_CDC                       // {8}
              usUSB_state = 0;
#endif
              fnGotoNextState(ES_NO_CONNECTION);                         // log out of serial session and kill any monitoring timers
          }
          return;
    }
}


// This routine is used when we know that we have to jump some white space
//
static int fnJumpWhiteSpace(CHAR **ptrptrInput)
{
    CHAR *ptrInput = *ptrptrInput;

    if (*ptrInput != ' ') {
        while (*ptrInput != 0) {
            if (*ptrInput != ' ') {
                ptrInput++;
            }
            else {
                break;
            }
        }
    }

    while (*ptrInput != 0) {
        if (*ptrInput == ' ') {
            ptrInput++;
        }
        else {
            break;
        }
    }
    *ptrptrInput = ptrInput;
    return (*ptrInput == 0);
}

#if !defined REMOVE_PORT_INITIALISATIONS
static int fnSetPort(unsigned char ucType, CHAR *ptrInput)               // modify the port type
{
    unsigned char ucBit;

    switch (ucType) {
    case DO_DDR:
        ucBit = (unsigned char )fnHexStrHex(ptrInput);                   // extract the bit to be changed (1..4)
        if (fnJumpWhiteSpace(&ptrInput)) {
            return -1;
        }
        return (fnConfigPort((CHAR)(ucBit + '0' - 1), (*ptrInput)));
    }
    return 0;
}
#endif

#if defined MONITOR_PERFORMANCE                                          // {25}
static void fnDisplayTaskUse(CHAR cTask)
{
    TTASKTABLE *ptrTask;
    UTASK_PERFORMANCE Details;
    int iTaskNumber = 0;
    unsigned char ucPercentage;

    while ((ptrTask = fnGetTaskPerformance(iTaskNumber++, &Details)) != 0) { // get each task's performance details
        fnDebugMsg("Task \"");
        fnDebugMsg(ptrTask->pcTaskName);
        fnDebugMsg("\" executed ");
        fnDebugDec(Details.ulExecutions, 0);
        fnDebugMsg(" times - ");
        fnDebugMsg("Task duration [min/av/max] = ");
        fnDebugDec(Details.ulMinimumExecution, 0);
        fnDebugMsg("/");
        fnDebugDec(Details.ulAverageExecution, 0);
        fnDebugMsg("/");
        fnDebugDec(Details.ulMaximumExecution, 0);
        fnDebugMsg(" [Max. at ");
        fnDebugDec(Details.ulTimeStamp, 0);
        fnDebugMsg(" TICKs] - CPU = ");
        if ((Details.ulTotalTasksDuration != 0) || (Details.ulTotalIdle != 0)) {
            ucPercentage = (unsigned char)((100 * Details.ulTotalExecution) / (Details.ulTotalTasksDuration + Details.ulTotalIdle));
        }
        else {
            ucPercentage = 0;
        }
        fnDebugDec(ucPercentage, 0);
        fnDebugMsg("%\r\n");
    }
    fnDebugMsg("Total CPU task load = ");
    if ((Details.ulTotalTasksDuration != 0) || (Details.ulTotalIdle != 0)) {
        ucPercentage = (unsigned char)((100 * Details.ulTotalTasksDuration) / (Details.ulTotalTasksDuration + Details.ulTotalIdle));
    }
    else {
        ucPercentage = 0;
    }
    fnDebugDec(ucPercentage, 0);
    fnDebugMsg("% (");
    fnDebugDec((Details.ulTotalTasksDuration), 0);
    fnDebugMsg("/");
    fnDebugDec((Details.ulTotalIdle), 0);
    fnDebugMsg("/");
    fnDebugDec((Details.ulTotalTasksDuration + Details.ulTotalIdle), 0);
    fnDebugMsg(")\r\n");
}
#endif

#if defined MEMORY_DEBUGGER                                              // {54}
static CHAR *fnDisplayAscii(unsigned long ulValue, CHAR *ptrAscii, int iLength)
{
    register CHAR cValue;
    int iShift;
    switch (iLength) {
    default:
  //case BYTE_ACCESS:
        iShift = 0;
        break;
    case SHORT_WORD_ACCESS:
        iShift = 8;
        break;
    case LONG_WORD_ACCESS:
        iShift = 24;
        break;
    }
    while (iLength--) {                                                  // the number of bytes in each entity
        cValue = (CHAR)(ulValue >> iShift);
        if ((cValue < ' ') || (cValue > 'z')) {
            cValue = '.';                                                // if the value can't be displayed as simple ASCII display it as a dot
        }
        *ptrAscii++ = cValue;
        iShift -= 8;
    }
    return ptrAscii;
}

/************************************************************************/
#define NUMBER_OF_ANALOGUE_INPUTS 8
signed short diInput1[NUMBER_OF_ANALOGUE_INPUTS];

static const int ADC0channel[8] = {0,1,2,3,4,5,6,7}; // in reality the numbers may be mixed around
static const int ADC1channel[8] = {0,1,2,3,4,5,6,7};

void function(void) // actually an interrupt routine
{
	static int iDinIndex = 0;                                            // multiplexed input index (counts 0,1,2,3,4,5,6,7,8,9,10,11)

    // Read the sampled values so that the next conversion can be performed (this clears the interrupt flag)
    //
	diInput1[ADC0channel[iDinIndex]] = (signed short)0x0080;              // save multiplexed inputs (odd samples are generally discarded - exception monitoring inputs)
	diInput1[ADC1channel[iDinIndex]] = (signed short)0x0080;

    if (iDinIndex & 0x01) {                                              // every even sample
        register signed short usSample = diInput1[ADC0channel[iDinIndex]]; // ADC input sample
        register int iADC;

        for (iADC = 0; iADC < 2; iADC++) {                           // for each ADC (2 digital channels)
          //if (iADC == 0) {
          //    usSample = diInput1[ADC0channel[iDinIndex]];
          //}
            fnDebugHex(usSample, (sizeof(usSample) | WITH_LEADIN | WITH_CR_LF));
            usSample = diInput1[ADC1channel[iDinIndex]];             // next ADC input sample (ADC1)
        }

        if (++iDinIndex >= 8) {
		    iDinIndex = 0;
	    }
	    
    }
    else {                                                               // odd samples
        ++iDinIndex;
    }
}
/************************************************************************/
#endif

#if defined PWM_MEASUREMENT_DEVELOPMENT                                  // temporary code for developing PWM measurement based on K pin change triggered DMA
#define PWM_PIT_CHANNEL                  2
#define PWM_MEASUREMENT_DMA_CHANNEL      9                               // use DMA channel 9
#define PWM_EDGES                        10
volatile unsigned long PWM_sample_buffer[PWM_EDGES] = {0};
static int iInitialPortState = 0;
static int iLastPortRef = 0;
static unsigned long ulLastPortBit = 0;
static void _PWM_Interrupt(void)
{
    volatile unsigned long *ptrPCR = (volatile unsigned long *)(PORT0_BLOCK + (iLastPortRef * 0x1000));
    ATOMIC_PERIPHERAL_BIT_REF_CLEAR(DMA_ERQ, PWM_MEASUREMENT_DMA_CHANNEL); // disable further DMA operation
  //DMA_ERQ &= ~(DMA_ERQ_ERQ0 << PWM_MEASUREMENT_DMA_CHANNEL);
    while (ulLastPortBit != 0) {
        if ((ulLastPortBit & 0x1) != 0) {
            *ptrPCR &= ~PORT_IRQC_DMA_BOTH;                              // disable DMA on last input
        }
        ptrPCR++;
        ulLastPortBit >>= 1;
    }
    fnInterruptMessage(TASK_APPLICATION, (unsigned char)(198));
}

extern void fnShowPWM(void)
{
    int i;
    unsigned long ulHigh = 0;
    unsigned long ulLow = 0;
    int iIntegration = 0;
    unsigned long ulMSR;
    fnDebugMsg("PWM\r\n");
    for (i = 0; i < PWM_EDGES;) {
        fnDebugHex(PWM_sample_buffer[i], (sizeof(PWM_sample_buffer[i]) | WITH_LEADIN | WITH_CR_LF));
        fnDebugHex(PWM_sample_buffer[i + 1], (sizeof(PWM_sample_buffer[i + 1]) | WITH_LEADIN | WITH_CR_LF));
        if (i < (PWM_EDGES - 2)) {
            if ((PWM_sample_buffer[i] != 0) && (PWM_sample_buffer[i + 1]) && (PWM_sample_buffer[i + 2])) {
                ulHigh += (PWM_sample_buffer[i] - PWM_sample_buffer[i + 1]);
                ulLow += (PWM_sample_buffer[i + 1] - PWM_sample_buffer[i + 2]);
                iIntegration++;
            }
        }
        i += 2;
    }
    if (iIntegration == 0) {
        if (iInitialPortState != 0) {
            ulMSR = 10000;
        }
        else {
            ulMSR = 0;
        }
    }
    else {
        if (iInitialPortState != 0) {
            ulMSR = ((ulLow * 10000) / (ulHigh + ulLow));                // first edge was a high to low one
        }
        else {
            ulMSR = ((ulHigh * 10000) / (ulHigh + ulLow));               // first edge was a low to high one
        }
    }
    fnDebugMsg("MSR = ");
    fnDebugDec(ulMSR, 0);
    fnDebugMsg("%%%\r\n");
    uMemset((void *)PWM_sample_buffer, 0, sizeof(PWM_sample_buffer));
}

static void fnMeasurePWM(int iPortRef, unsigned long ulPortBit)
{
    KINETIS_PIT_CTL *ptrCtl = (KINETIS_PIT_CTL *)PIT_CTL_ADD;
    PIT_SETUP pit_setup;                                                 // interrupt configuration parameters
    KINETIS_DMA_TDC *ptrDMA_TCD = (KINETIS_DMA_TDC *)eDMA_DESCRIPTORS;
    unsigned char *ptrDMAMUX = DMAMUX0_CHCFG_ADD;
    INTERRUPT_SETUP interrupt_setup;                                     // interrupt configuration parameters
    interrupt_setup.int_type = PORT_INTERRUPT;                           // identifier to configure port interrupt
    interrupt_setup.int_port = iPortRef;                                 // the port that the interrupt input is on
    interrupt_setup.int_port_bits = ulPortBit;
    interrupt_setup.int_port_sense = (IRQ_BOTH_EDGES | PULLUP_ON | PORT_DMA_MODE); // DMA on both edges and keep peripheral function
    interrupt_setup.int_handler = 0;                                    // no interrupt handler when using DMA
    fnConfigureInterrupt((void *)&interrupt_setup);                     // configure interrupt (port triggered DMA)

    iLastPortRef = iPortRef;
    ulLastPortBit = ulPortBit;

    ptrDMA_TCD += PWM_MEASUREMENT_DMA_CHANNEL;
    ptrDMAMUX += PWM_MEASUREMENT_DMA_CHANNEL;
    ptrDMA_TCD->DMA_TCD_SOFF = 0;                                        // source not incremented
    ptrDMA_TCD->DMA_TCD_DOFF = sizeof(unsigned long);                    // destination incremented by one long word after each transfer
    ptrDMA_TCD->DMA_TCD_BITER_ELINK = ptrDMA_TCD->DMA_TCD_CITER_ELINK = PWM_EDGES;
    ptrDMA_TCD->DMA_TCD_ATTR = (DMA_TCD_ATTR_DSIZE_32 | DMA_TCD_ATTR_SSIZE_32); // transfer sizes always single bytes
    ptrCtl += PWM_PIT_CHANNEL;
    ptrDMA_TCD->DMA_TCD_SADDR = (unsigned long)&(ptrCtl->PIT_CVAL);
    ptrDMA_TCD->DMA_TCD_DADDR = (unsigned long)PWM_sample_buffer;        // destination is the sample buffer
    ptrDMA_TCD->DMA_TCD_SLAST = ptrDMA_TCD->DMA_TCD_DLASTSGA = 0;        // no change to address when the buffer has filled
    ptrDMA_TCD->DMA_TCD_NBYTES_ML = sizeof(unsigned long);               // each request starts a single long word transfer
    ptrDMA_TCD->DMA_TCD_CSR = (DMA_TCD_CSR_ACTIVE);                      // set active
    POWER_UP(6, SIM_SCGC6_DMAMUX0);                                      // enable DMA multiplexer 0
    *ptrDMAMUX = ((DMAMUX0_CHCFG_SOURCE_PORTA + iPortRef) | DMAMUX_CHCFG_ENBL); // trigger DMA channel on port changes

    // Set measurement time (at least 3 edges must be collected)
    //
    pit_setup.int_type = PIT_INTERRUPT;
    pit_setup.int_priority = PIT2_INTERRUPT_PRIORITY;
    pit_setup.count_delay = PIT_MS_DELAY(2);                             // 2ms (suitable from about 1kHz)
    pit_setup.mode = (PIT_SINGLE_SHOT);
    pit_setup.int_handler = _PWM_Interrupt;
#if defined SUPPORT_PITS                                                 // multiple PITs
    pit_setup.ucPIT = PWM_PIT_CHANNEL;                                   // use PIT channel
#endif
    fnConfigureInterrupt((void *)&pit_setup);                            // configure PIT

    {                                                                    // start the measurement and determine the initial input state
        int iInputState1;
        int iInputState2;
        uDisable_Interrupt();
        switch (iPortRef) {
        case PORTA:
            iInputState1 = (_READ_PORT_MASK(A, ulPortBit) != 0);             // read initial input state
            ATOMIC_PERIPHERAL_BIT_REF_SET(DMA_ERQ, PWM_MEASUREMENT_DMA_CHANNEL); // enable request source
          //DMA_ERQ |= (DMA_ERQ_ERQ0 << PWM_MEASUREMENT_DMA_CHANNEL);        
            iInputState2 = (_READ_PORT_MASK(A, ulPortBit) != 0);             // read new input state
            break;
        case PORTB:
            iInputState1 = (_READ_PORT_MASK(B, ulPortBit) != 0);             // read initial input state
            ATOMIC_PERIPHERAL_BIT_REF_SET(DMA_ERQ, PWM_MEASUREMENT_DMA_CHANNEL); // enable request source
          //DMA_ERQ |= (DMA_ERQ_ERQ0 << PWM_MEASUREMENT_DMA_CHANNEL);
            iInputState2 = (_READ_PORT_MASK(B, ulPortBit) != 0);             // read new input state
            break;
        case PORTC:
            iInputState1 = (_READ_PORT_MASK(C, ulPortBit) != 0);             // read initial input state
            ATOMIC_PERIPHERAL_BIT_REF_SET(DMA_ERQ, PWM_MEASUREMENT_DMA_CHANNEL); // enable request source
          //DMA_ERQ |= (DMA_ERQ_ERQ0 << PWM_MEASUREMENT_DMA_CHANNEL);
            iInputState2 = (_READ_PORT_MASK(C, ulPortBit) != 0);             // read new input state
            break;
        case PORTD:
            iInputState1 = (_READ_PORT_MASK(D, ulPortBit) != 0);             // read initial input state
            ATOMIC_PERIPHERAL_BIT_REF_SET(DMA_ERQ, PWM_MEASUREMENT_DMA_CHANNEL); // enable request source
          //DMA_ERQ |= (DMA_ERQ_ERQ0 << PWM_MEASUREMENT_DMA_CHANNEL);        // enable request source
            iInputState2 = (_READ_PORT_MASK(D, ulPortBit) != 0);             // read new input state
            break;
        case PORTE:
            iInputState1 = (_READ_PORT_MASK(E, ulPortBit) != 0);             // read initial input state
            ATOMIC_PERIPHERAL_BIT_REF_SET(DMA_ERQ, PWM_MEASUREMENT_DMA_CHANNEL); // enable request source
          //DMA_ERQ |= (DMA_ERQ_ERQ0 << PWM_MEASUREMENT_DMA_CHANNEL);        // enable request source
            iInputState2 = (_READ_PORT_MASK(E, ulPortBit) != 0);             // read new input state
            break;
        }

        if (iInputState1 == iInputState2) {
            iInitialPortState = iInputState1;
        }
        else if (ptrDMA_TCD->DMA_TCD_CITER_ELINK != PWM_EDGES) {         // if a DMA transfer took place due to the input change
            iInitialPortState = iInputState1;                            // the state change was recognised so we take the initial input as start state
        }
        else {
            iInitialPortState = iInputState2;                            // an initial input change during DMA enabe was not recognised so the second state is the state state
        }
        uEnable_Interrupt();
    }
}
#endif


#if defined TEST_CMSIS_CFFT                                              // {84}
// ARM's standard test input, defined as "Test Input signal contains 10KHz signal + Uniformly distributed white noise" but as samples rather than complex values
//
static const float _testInput_f32_10khz[1024] =
{
   -0.865129623056441,     	-2.655020678073846,    	0.600664612949661,     	0.080378093886515,    
   -2.899160484012034,     	2.563004262857762,     	3.078328403304206,     	0.105906778385130,    
   0.048366940168201,      	-0.145696461188734,    	-0.023417155362879,    	2.127729174988954,    
   -1.176633086028377,     	3.690223557991855,     	-0.622791766173194,    	0.722837373872203,    
   2.739754205367484,      	-0.062610410524552,    	-0.891296810967338,    	-1.845872258871811,   
   1.195039415434387,      	-2.177388969045026,    	1.078649103637905,     	2.570976050490193,    
   -1.383551403404574,     	2.392141424058873,     	2.858002843205065,     	-3.682433899725536,   
   -3.488146646451150,     	1.323468578888120,     	-0.099771155430726,    	1.561168082500454,    
   1.025026795103179,      	0.928841900171200,     	2.930499509864950,     	2.013349089766430,    
   2.381676148486737,      	-3.081062307950236,    	-0.389579115537544,    	0.181540149166620,    
   -2.601953341353208,     	0.333435137783218,     	-2.812945856162965,    	2.649109640172910,    
   -1.003963025744654,     	1.552460768755035,     	0.088641345335247,     	-2.519951327113426,   
   -4.341348988610527,     	0.557772429359965,     	-1.671267412948494,    	0.733951350960387,    
   0.409263788034864,      	3.566033071952806,     	1.882565173848352,     	-1.106017073793287,   
   0.154456720778718,      	-2.513205795512153,    	0.310978660939421,     	0.579706500111723,    
   0.000086383683251,      	-1.311866980897721,    	1.840007477574986,     	-3.253005768451345,   
   1.462584328739432,      	1.610103610851738,     	0.761914676858907,     	0.974541361089834,    
   0.686845845885983,      	1.849153122025191,     	0.787800410401453,     	-1.187438909666279,   
   -0.754937911044720,     	0.084373858395232,     	-2.600269011710521,    	-0.962982842142644,   
   -0.369328108540868,     	0.810791418361879,     	3.587016488699641,     	-0.520776145083723,   
   0.640249919627884,      	1.103122489464969,     	2.231779881455556,     	-1.308035392685241,   
   0.424070304330106,      	-0.200383932651189,    	-2.365526783356541,    	-0.989114757436628,   
   2.770807688959777,      	-0.444172737462307,    	0.079760979374078,     	-0.005199118412183,   
   -0.664712668309527,     	-0.624171857561896,    	0.537306979007338,     	-2.575955675497642,   
   1.562363235756780,      	1.814069369848895,     	-1.293428583392509,    	-1.026188449495686,   
   -2.981771815588717,     	-4.223468103075124,    	2.672674782004045,     	-0.856096801117735,   
   0.048517345512563,      	-0.026860721136222,    	0.392932277758187,     	-1.331740855093099,   
   -1.894292129477081,     	-1.425006468460681,    	-2.721772427617057,    	-1.616831100216806,   
   3.551177651488947,      	-0.069685667896087,    	-3.134634907409102,    	-0.263627598944639,   
   -1.650469945991350,     	-2.203580339374399,    	-0.872203246123242,    	1.230782812607287,    
   0.257288860093291,      	1.989083106173137,     	-1.985638729453261,    	-1.416185105842892,   
   -1.131097688325772,     	-2.245130805416057,    	-1.938873996219074,    	2.043608361562645,    
   -0.583727989880841,     	-1.785266378212929,    	1.961457586224753,     	1.139400099963223,    
   -1.979519343363991,     	2.003023322818429,     	0.229004069076829,     	3.452808862193135,    
   2.882273808365857,      	-1.549450501844438,    	-3.283872089931876,    	-0.327025884099064,   
   -0.054979977136430,     	-1.192280531479012,    	0.645539328365578,     	2.300832863404618,    
   -1.092951789535240,     	-1.017368249363773,    	-0.142673056169787,    	0.831073544881250,    
   -2.314612531587064,     	-2.221456299106321,    	0.460261143885226,     	0.050585301888595,    
   0.364373329183988,      	-1.685956552069538,    	0.050664512351055,     	-0.193355783902718,   
   -0.158660446046828,     	2.394156453841953,     	-1.562965718554525,    	-2.199750600869900,   
   1.544984022381773,      	-1.988307216807315,    	-0.628240722541046,    	-1.436235771505429,   
   1.677013691147313,      	1.600741781678228,     	-0.757380959134706,    	-4.784797439515566,   
   0.265121462834569,      	3.862029485934378,     	2.386823577249430,     	-3.655779745436893,   
   -0.763541621368016,     	-1.182140388432962,    	-1.349106114858063,    	-2.287533624396759,   
   -0.028603745188423,     	-1.353580755934427,    	0.461602380352937,     	-0.059599055078928,   
   -0.929946734342228,     	0.065773089295561,     	1.106565863102982,     	4.719295086373593,    
   -2.108377703544395,     	-2.226393620240159,    	1.375668397437521,     	-0.960772428525443,   
   -2.156313465390571,     	1.126060012375311,     	2.756485137030720,     	0.739639690862600,    
   3.914769510295006,      	1.685232785586675,     	4.079058040970612,     	-1.174598301660513,   
   -2.885776587275580,     	-0.241073635188767,    	3.080489872502403,     	-2.051244183999421,   
   0.664330486845139,      	-1.697798999370016,    	1.452369423649782,     	-1.523532831019280,   
   0.171981186587481,      	-4.685274721583927,    	-1.336175835319380,    	1.419070770428945,    
   -0.035791601713475,     	2.291937971632081,     	-1.962559313450293,    	-4.831595589339301,   
   -1.857055284000925,     	2.606271522635512,     	-0.576447978738030,    	0.082299166967720,    
   1.888399453494614,      	-3.564705298046079,    	-0.939357831083889,    	-1.903578203697778,   
   -2.642492215447250,     	-0.182990405251017,    	3.742026478011174,     	0.104295803798333,    
   1.848678195370347,      	-1.887384346896369,    	0.365048973046045,     	-0.889638010354219,   
   1.173877118428863,      	-1.178562827540109,    	0.610271645685184,     	1.831284815697871,    
   0.449575390102283,      	1.597171905253443,     	3.918574971904773,     	0.868104027970404,    
   0.582643134746494,      	2.321256382353331,     	-0.238118642223180,    	-2.890287868054370,   
   0.970995414625622,      	0.666137930891283,     	-0.202435718709502,    	2.057930200518194,    
   3.120583443719949,      	-0.863945271701041,    	0.906848893874630,     	-1.434124930222570,   
   0.754659384848783,      	-5.224154442713778,    	2.330229744098967,     	1.113946320164698,    
   0.523324920322840,      	1.750740911548348,     	-0.899333972913577,    	0.228705845203506,    
   -1.934782624767648,     	-3.508386237231303,    	-2.107108523073510,    	0.380587645474815,    
   -0.476200877183279,     	-2.172086712642198,    	1.795372535780299,     	-2.100318983391055,   
   -0.022571122461405,     	0.674514020010955,     	-0.148872569390857,    	0.298175890592737,    
   -1.134244492493590,     	-3.146848422289455,    	-1.357950199087602,    	0.667362732020878,    
   -3.119397998316724,     	-1.189341126297637,    	-1.532744386856668,    	-1.672972484202534,   
   -2.042283373871558,     	-1.479481547595924,    	-0.002668662875396,    	0.262737760129546,    
   2.734456080621830,      	-0.671945925075102,    	-3.735078262179111,    	-0.161705013319883,   
   0.748963512361001,      	1.128046374367600,     	0.649651335592966,     	1.880020215025867,    
   -1.095632293842306,     	1.197764876160487,     	0.323646656252985,     	-1.655502751114502,   
   3.666399062961496,      	-0.334060899735197,    	-2.119056978738397,    	3.721375117275012,    
   0.044874186872307,      	-2.733053897593234,    	1.590700278891042,     	3.215711772781902,    
   -1.792085012843801,     	-0.405797188885475,    	-0.628080020080892,    	-1.831815840843960,   
   2.973656862522834,      	-0.212032655138417,    	0.372437389437234,     	-1.614030579023492,   
   -0.704900996358698,     	1.123700273452105,     	-0.136371848130819,    	3.020284357635585,    
   -0.550211350877649,     	5.101256236381711,     	3.367051512192333,     	-4.385131946669234,   
   -3.967303337694391,     	-0.965894936640022,    	0.328366945264681,     	0.199041562924914,    
   1.067681999025495,      	-1.939516091697170,    	-1.092980954328824,    	0.273786079368066,    
   -0.040928322190265,     	-0.118368078577437,    	1.766589628899997,     	1.738321311635393,    
   -2.895012794321649,     	1.213521771395142,     	0.922971726633985,     	1.091516563636489,    
   3.226378465469620,      	1.149169778666974,     	-1.695986327709386,    	-0.974803077355813,   
   -4.898035507513607,     	1.622719302889447,     	0.583891313586579,     	-1.677182424094957,   
   -1.915633132814685,     	-1.980150370851616,    	0.604538269404190,     	0.939862406149365,    
   -1.266939874246416,     	-1.494771249200063,    	0.278042784093988,     	0.326627416008916,    
   -1.914530157643303,     	1.908947721862196,     	0.531819285694044,     	3.056856632319658,    
   -0.389241827774643,     	-2.418606606780420,    	0.915299238878703,     	-0.098774174295283,   
   -0.906199428444304,     	0.316716451217743,     	-4.367700643578311,    	1.491687997515293,    
   -1.962381126288365,     	-0.700829196527045,    	3.028958963615630,     	-2.313461067462598,   
   -1.431933239886712,     	-0.831153039725342,    	3.939495598250743,     	0.342974753984771,    
   -2.768330763002974,     	-2.744010370019008,    	3.821352685212561,     	4.551065271455856,    
   3.270136437041298,      	-3.188028411950982,    	-0.777075012417436,    	0.097110650265216,    
   1.221216137608812,      	-1.325824244541822,    	-2.655296734084113,    	-1.074792144885704,   
   2.770401584439407,      	5.240270645610543,     	0.108576672208892,     	-1.209394350650142,   
   1.403344353838785,      	-0.299032904177277,    	4.074959450638227,     	1.718727473952107,    
   -3.061349227080806,     	-1.158596888541269,    	3.381858904662625,     	0.957339964054052,    
   0.179900074904899,      	-3.909641902506081,    	0.805717289408649,     	2.047413793928261,    
   -1.273580225826614,     	-2.681359186869971,    	-0.721241345822093,    	-1.613090681569475,   
   0.463138804815955,      	0.377223507800954,     	2.046550684968141,     	0.178508732797712,    
   -0.477815330358845,     	3.763355908332053,     	1.300430303035163,     	-0.214625793857725,   
   1.343267891864081,      	-0.340007682433245,    	2.062703194680005,     	0.042032160234235,    
   0.643732569732250,      	-1.913502543857589,    	3.771340762937158,     	1.050024807363386,    
   -4.440489488592649,     	0.444904302066643,     	2.898702265650048,     	1.953232980548558,    
   2.761564952735079,      	1.963537633260397,     	-2.168858472916215,    	-4.116235357699841,   
   4.183678271896528,      	0.600422284944681,     	-0.659352647255126,    	-0.993127338218109,   
   -2.463571314945747,     	0.937720951545881,     	-3.098957308429730,    	-2.354719140045463,   
   -0.417285119323949,     	2.187974075975947,     	1.101468905172585,     	-3.185800678152109,   
   2.357534709345083,      	0.246645606729407,     	4.440905650784504,     	-2.236807716637866,   
   -2.171481518317550,     	-2.029571795072690,    	0.135599790431348,     	-1.277965265520191,   
   -1.927976233157507,     	-5.434492783745394,    	-2.026375829312657,    	1.009666016819321,    
   0.238549782367247,      	-0.516403923971309,    	-0.933977817429352,    	0.155803015935614,    
   -0.396194809997929,     	-0.915178100253214,    	0.666329367985015,     	-1.517991149945785,   
   0.458266744144822,      	-1.242845974381418,    	0.057914823556477,     	0.994101307476875,    
   -2.387209849199325,     	0.459297048883826,     	0.227711405683905,     	0.030255073506117,    
   -1.323361608181337,     	-4.650244457426706,    	0.062908579526021,     	3.462831028244432,    
   1.303608183314856,      	-1.430415193881612,    	-1.672886118942142,    	0.992890699210099,    
   -0.160814531784247,     	-1.238132939350430,    	-0.589223271459376,    	2.326363810561534,    
   -4.433789496230785,     	1.664686987538929,     	-2.366128834617921,    	1.212421570743837,    
   -4.847914267690055,     	0.228485221404712,     	0.466139765470957,     	-1.344202776943546,   
   -1.012053673330574,     	-2.844980626424742,    	-1.552703722026340,    	-1.448830983885038,   
   0.127010756753980,      	-1.667188263752299,    	3.424818052085100,     	0.956291135453840,    
   -3.725533331754662,     	-1.584534272368832,    	-1.654148210472472,    	0.701610500675698,    
   0.164954538683927,      	-0.739260064712987,    	-2.167324026090101,    	-0.310240491909496,   
   -2.281790349106906,     	1.719655331305361,     	-2.997005923606441,    	-1.999301431556852,   
   -0.292229010068828,     	1.172317994855851,     	0.196734885241533,     	2.981365193477068,    
   2.637726016926352,      	1.434045125217982,     	0.883627180451827,     	-1.434040761445747,   
   -1.528891971086553,     	-3.306913135367542,    	-0.399059265470646,    	-0.265674394285178,   
   3.502591252855384,      	0.830301156604454,     	-0.220021317046083,    	-0.090553770476646,   
   0.771863477047951,      	1.351209629105760,     	3.773699756201963,     	0.472600118752329,    
   2.332825668012222,      	1.853747950314528,     	0.759515251766178,     	1.327112776215496,    
   2.518730296237868,      	0.764450208786353,     	-0.278275349491296,    	-0.041559465082020,   
   1.387166083167787,      	2.612996769598122,     	-0.385404831721799,    	2.005630016170309,    
   -0.950500047307998,     	-1.166884021392492,    	1.432973552928162,     	2.540370505384567,    
   -1.140505295054501,     	-3.673358835201185,    	-0.450691288038056,    	1.601024294408014,    
   0.773213556014045,      	2.973873693246168,     	-1.361548406382279,    	1.409136332424815,    
   -0.963382518314713,     	-2.031268227368161,    	0.983309972085586,     	-3.461412488471631,   
   -2.601124929406039,     	-0.533896239766343,    	-2.627129008866350,    	0.622111169161305,    
   -1.160926365580422,     	-2.406196188132628,    	-1.076870362758737,    	-1.791866820937175,   
   -0.749453071522325,     	-5.324156615990973,    	-1.038698022238289,    	-2.106629944730630,   
   0.659295598564773,      	0.520940881580988,     	-0.055649203928700,    	0.292096765423137,    
   -4.663743901790872,     	-0.125066503391666,    	-2.452620252445380,    	-0.712128227397468,   
   -0.048938037970968,     	-1.821520226003361,    	0.810106421304257,     	-0.196636623956257,   
   -0.701769836763804,     	2.460345045649201,     	3.506597671641116,     	-2.711322611972225,   
   -0.658079876600542,     	-2.040082099646173,    	2.201668355395807,     	1.181507395879711,    
   -1.640739552179682,     	-1.613393726467190,    	-1.156741241731352,    	2.527773464519963,    
   -0.497040638009502,     	-0.975817112895589,    	-2.866830755546166,    	1.120214498507878,    
   5.986771654661698,      	0.398219252656757,     	-3.545606013198135,    	0.312398099396191,    
   -2.265327979531788,     	0.792121001107366,     	-3.736145137670100,    	0.762228883650802,    
   2.283545661214646,      	3.780020629583529,     	3.117260228608810,     	-2.011159255609613,   
   0.279107700476072,      	2.003369134246936,     	-1.448171234480257,    	0.584697150310140,    
   0.919508663636197,      	-3.071349141675388,    	-1.555923649263667,    	2.232497079438850,    
   -0.012662139119883,     	0.372825540734715,     	2.378543590847629,     	1.459053407813062,    
   -0.967913907390927,     	1.322825200678212,     	-1.033775820061824,    	-1.813629552693142,   
   4.794348161661486,      	0.655279811518676,     	-2.224590138589720,    	0.595329481295766,    
   3.364055988866225,      	1.863416422998127,     	1.930305751828105,     	-0.284467053432545,   
   -0.923374905878938,     	1.922988234041399,     	0.310482143432719,     	0.332122302397134,    
   -1.659487472408966,     	-1.865943507877961,    	-0.186775297569864,    	-1.700543850628361,   
   0.497157959366735,      	-0.471244843957418,    	-0.432013753969948,    	-4.000189880113231,   
   -0.415335170016467,     	0.317311950972859,     	0.038393428927595,     	0.177219909465206,    
   0.531650958095143,      	-2.711644985175806,    	0.328744077805156,     	-0.938417707547928,   
   0.970379584897379,      	1.873649473917137,     	0.177938226987023,     	0.155609346302393,    
   -1.276504241867208,     	-0.463725075928807,    	-0.064748250389500,    	-1.725568534062385,   
   -0.139066584804067,     	1.975514554117767,     	-0.807063199499478,    	-0.326926659682788,   
   1.445727032487938,      	-0.597151107739100,    	2.732557531709386,     	-2.907130934109188,   
   -1.461264832679981,     	-1.708588604968163,    	3.652851925431363,     	0.682050868282879,    
   -0.281312579963294,     	0.554966483307825,     	-0.981341739340932,    	1.279543331141603,    
   0.036589747826856,      	2.312073745896073,     	1.754682200732425,     	-0.957515875428627,   
   -0.833596942819695,     	0.437054368791033,     	-0.898819399360279,    	-0.296050580896839,   
   -0.785144257649601,     	-2.541503089003311,    	2.225075846758761,     	-1.587290487902002,   
   -1.421404172056462,     	-3.015149802293631,    	1.780874288867949,     	-0.865812740882613,   
   -2.845327531197112,     	1.445225867774367,     	2.183733236584647,     	1.163371072749080,    
   0.883547693520409,      	-1.224093106684675,    	-1.854501116331044,    	1.783082089255796,    
   2.301508706196191,      	-0.539901944139077,    	1.962315832319967,     	-0.060709041870503,   
   -1.353139923300238,     	-1.482887537805234,    	1.273732601967176,     	-3.456609915556321,   
   -3.752320586540873,     	3.536356614978951,     	0.206035952043233,     	5.933966913773842,    
   -0.486633898075490,     	-0.329595089863342,    	1.496414153905337,     	0.137868749388880,    
   -0.437192030996792,     	2.682750615210656,     	-2.440234892848570,    	1.433910252426186,    
   -0.415051506104074,     	1.982003013708649,     	1.345796609972435,     	-2.335949513404370,   
   1.065988867433025,      	2.741844905000464,     	-1.754047930934362,    	0.229252730015575,    
   -0.679791016408669,     	-2.274097820043743,    	0.149802252231876,     	-0.139697151364830,   
   -2.773367420505435,     	-4.403400246165611,    	-1.468974515184135,    	0.664990623095844,    
   -3.446979775557143,     	1.850006428987618,     	-1.550866747921936,    	-3.632874882935257,   
   0.828039662992464,      	2.794055182632816,     	-0.593995716682633,    	0.142788156054200,    
   0.552461945119668,      	0.842127129738758,     	1.414335509600077,     	-0.311559241382430,   
   1.510590844695250,      	1.692217183824300,     	0.613760285711957,     	0.065233463207770,    
   -2.571912893711505,     	-1.707001531141341,    	0.673884968382041,     	0.889863883420103,    
   -2.395635435233346,     	1.129247296359819,     	0.569074704779735,     	6.139436017480722,    
   0.822158309259017,      	-3.289872016222589,    	0.417612988384414,     	1.493982103868165,    
   -0.415353391377005,     	0.288670764933155,     	-1.895650228872272,    	-0.139631694475020,   
   1.445103299005436,      	2.877182243683429,     	1.192428490172580,     	-5.964591921763842,   
   0.570859795882959,      	2.328333316356666,     	0.333755014930026,     	1.221901577771909,    
   0.943358697415568,      	2.793063983613067,     	3.163005066073616,     	2.098300664513867,    
   -3.915313164333447,     	-2.475766769064539,    	1.720472044894277,     	-1.273591949275665,   
   -1.213451272938616,     	0.697439404325690,     	-0.309902287574293,    	2.622575852162781,    
   -2.075881936219060,     	0.777847545691770,     	-3.967947986440650,    	-3.066503371806472,   
   1.193780625937845,      	0.214246579281311,     	-2.610681491162162,    	-1.261224183972745,   
   -1.165071748544285,     	-1.116548474834374,    	0.847202164846982,     	-3.474301529532390,   
   0.020799541946476,      	-3.868995473288166,    	1.757979409638067,     	0.868115130183109,    
   0.910167436737958,      	-1.878855115563720,    	1.710357104174161,     	-1.468933980990902,   
   1.799544171601169,      	-4.922332880027887,    	0.219424548939720,     	-0.971671113451924,   
   -0.940533475616266,     	0.122510114412152,     	-1.373686254916911,    	1.760348103896323,    
   0.391745067829643,      	2.521958505327354,     	-1.300693516405092,    	-0.538251788309178,   
   0.797184135810173,      	2.908800548982588,     	1.590902251655215,     	-1.070323714487264,   
   -3.349764443340999,     	-1.190563529731447,    	1.363369471291963,     	-1.814270299924576,   
   -0.023381588315711,     	1.719182048679569,     	0.839917213252626,     	1.006099633839122,    
   0.812462674381527,      	1.755814336346739,     	2.546848681206319,     	-1.555300208869455,   
   1.017053811631167,      	0.996591039170903,     	-1.228047247924881,    	4.809462271463009,    
   2.318113116151685,      	-1.206932520679733,    	1.273757685623312,     	0.724335352481802,    
   1.519876652073198,      	-2.749670314714158,    	3.424042481847581,     	-3.714668360421517,   
   1.612834197004014,      	-2.038234723985566,    	1.470938786562152,     	2.111634918450302,    
   1.030376670151787,      	-0.420877189003829,    	-1.502024800532894,    	0.452310749163804,    
   -1.606059382300987,     	-4.006159967834147,    	-2.152801208196508,    	1.671674089372579,    
   1.714536333564101,      	-1.011518543005344,    	-0.576410282180584,    	0.733689809480836,    
   1.004245602717974,      	1.010090391888449,     	3.811459513385621,     	-5.230621089271954,   
   0.678044861034399,      	1.255935859598107,     	1.674521701615288,     	-1.656695216761705,   
   1.169286028869693,      	0.524915416191998,     	2.397642885039520,     	2.108711400616072,    
   2.037618211018084,      	-0.623664553406925,    	2.984106170984409,     	1.132182737400932,    
   -2.859274340352130,     	-0.975550071398723,    	-1.359935119997407,    	-2.963308211050121,   
   -0.228726662781163,     	-1.411110379682043,    	0.741553355734225,     	0.497554254758309,    
   2.371907950598855,      	1.063465168988748,     	-0.641082692081488,    	-0.855439878540726,   
   0.578321738578726,      	3.005809768796194,     	1.961458699064065,     	-3.206261663772745,   
   -0.364431989095434,     	-0.263182496622273,    	1.843464680631139,     	-0.419107530229249,   
   1.662335873298487,      	-0.853687563304005,    	-2.584133404357169,    	3.466839568922895,    
   0.881671345091973,      	0.454620014206908,     	-1.737245187402739,    	2.162713238369243,    
   -3.868539002714486,     	2.014114855933826,     	-0.703233831811006,    	-3.410319935997574,   
   -1.851235811006584,     	0.909783907894036,     	0.091884002136728,     	-2.688294201131650,   
   -0.906134178460955,     	3.475054609035133,     	-0.573927964170323,    	-0.429542937515399,   
   0.991348618739939,      	1.974804904926325,     	0.975783450796698,     	-3.057119549071503,   
   -3.899429237481194,     	0.362439009175350,     	-1.124461670265618,    	1.806000360163583,    
   -2.768333362600288,     	0.244387897900379,     	0.908767296720926,     	1.254669374391882,    
   -1.420441929463686,     	-0.875658895966293,    	0.183824603376167,     	-3.361653917011686,   
   -0.796615630227952,     	-1.660226542658673,    	1.654439358307226,     	2.782812946709771,    
   1.418064412811531,      	-0.819645647243761,    	0.807724772592699,     	-0.941967976379298,   
   -2.312768306047469,     	0.872426936477443,     	0.919528961530845,     	-2.084904575264847,   
   -1.972464868459322,     	-1.050687203338466,    	1.659579707007902,     	-1.820640014705855,   
   -1.195078061671045,     	-1.639773173762048,    	1.616744338157063,     	4.019216096811563,    
   3.461021102549681,      	1.642352734361484,     	-0.046354693720813,    	-0.041936252359677,   
   -2.393307519480551,     	-0.341471634615121,    	-0.392073595257017,    	-0.219299018372730,   
   -2.016391579662071,     	-0.653096251969787,    	1.466353155666821,     	-2.872058864320412,   
   -2.157180779503830,     	0.723257479841560,     	3.769951308104384,     	-1.923392042420024,   
   0.644899359942840,      	-2.090226891621437,    	-0.277043982890403,    	-0.528271428321112,   
   2.518120645960652,      	1.040820431111488,     	-4.560583754742486,    	-0.226899614918836,   
   1.713331231108959,      	-3.293941019163642,    	-1.113331444648290,    	-1.032308423149906,   
   1.593774272982443,      	-1.246840475090529,    	-0.190344684920137,    	-1.719386356896355,   
   -2.827721754659679,     	-0.092438285279020,    	-0.565844430675246,    	-1.077916121691716,   
   -1.208665809504693,     	-2.996014266381254,    	2.888573323402423,     	2.829507048720695,    
   -0.859177034120755,     	-1.969302377743254,    	0.777437674525362,     	-0.124910190157646,   
   0.129875493115290,      	-4.192139262163992,    	3.023496047962126,     	1.149775163736637,    
   2.038151304801731,      	3.016122489841263,     	-4.829481812137012,    	-1.668436615909279,   
   0.958586784636918,      	1.550652410058678,     	-1.456305257976716,    	-0.079588392344731,   
   -2.453213599392345,     	0.296795909127105,     	-0.253426616607643,    	1.418937160028195,    
   -1.672949529066915,     	-1.620990298572947,    	-1.085103073196045,    	0.738606361195386,    
   -2.097831202853255,     	2.711952282071310,     	1.498539238246888,     	1.317457282535915,    
   -0.302765938349717,     	-0.044623707947201,    	2.337405215062395,     	-3.980689173859100,   
};

#if defined CMSIS_DSP_FFT_4096                                           // define the required dimension of the FFT input buffer based on the FFT size to be tested
    #if SIZE_OF_RAM <= (16 * 1024)                                       // restrict maximum FFT size when RAM is limited
        #define MAX_FFT_TEST_LENGTH     1024
    #elif SIZE_OF_RAM <= (32 * 1024)
        #define MAX_FFT_TEST_LENGTH     2048
    #else
        #define MAX_FFT_TEST_LENGTH     4096
    #endif
#elif defined CMSIS_DSP_FFT_2048
    #if SIZE_OF_RAM <= (16 * 1024)
        #define MAX_FFT_TEST_LENGTH     1024
    #else
        #define MAX_FFT_TEST_LENGTH     2048
    #endif
#elif defined CMSIS_DSP_FFT_1024
    #define MAX_FFT_TEST_LENGTH         1024
#elif defined CMSIS_DSP_FFT_512
    #define MAX_FFT_TEST_LENGTH         512
#elif defined CMSIS_DSP_FFT_256
    #define MAX_FFT_TEST_LENGTH         256
#else
    #define MAX_FFT_TEST_LENGTH         128
#endif

static void fnTestFFT(int iLength)
{
    float fft_result_buffer[MAX_FFT_TEST_LENGTH/2];                      // temporary result buffer
    if (iLength > MAX_FFT_TEST_LENGTH) {
        iLength = MAX_FFT_TEST_LENGTH;                                   // limit the size
        fnDebugMsg("FFT length reduced to max. of ");
        fnDebugDec(iLength, WITH_CR_LF);
    }
    
    if (fnFFT((void *)_testInput_f32_10khz, (void *)fft_result_buffer, iLength, 0, iLength, 0, 0, (FFT_INPUT_FLOATS | FFT_OUTPUT_FLOATS | FFT_MAGNITUDE_RESULT)) < 0) {
        fnDebugMsg("Invalid FFT length\r\n");
        return;
    }
    if (iLength == 1024) {                                               // check the expected result, which is maximum energy in bin 213
        if ((fft_result_buffer[213] > 300.0) && (fft_result_buffer[212] < 300.0) && (fft_result_buffer[214] < 300.00)) {
            fnDebugMsg("FFT result passed\r\n");
        }
        else {
            fnDebugMsg("FFT FAILED!!\r\n");
        }
    }
}
#endif



static void fnDoHardware(unsigned char ucType, CHAR *ptrInput)
{
#if defined MEMORY_DEBUGGER && defined _WINDOWS
    static unsigned char ucTestBuffer[] = {'3','2','1',4,5,6,7,8,9,'A','b','C','d',14,15,16}; // test buffer {54}
#endif
    switch (ucType) {
    case DO_RESET:                                                       // hardware - reset
        fnResetBoard();
        break;
    case DO_LAST_RESET:
        fnDebugMsg("Last reset cause = ");
        fnAddResetCause(0);
        break;
#if defined SUPPORT_LOW_POWER                                            // {71}
    case DO_LP_GET:
        fnShowLowPowerMode();
        break;
    case DO_LP_SET:
        {
            unsigned char ucPowerMode = (unsigned char)fnDecStrHex(ptrInput); // {80}
            fnSetLowPowerMode(ucPowerMode);
        }
        break;
    #if defined LOW_POWER_CYCLING_MODE                                   // {85}
    case DO_LP_CYCLE:
        fnDebugMsg("Low Power Cycling ");
        if (*ptrInput == '1') {
            iLowPowerLoopMode = LOW_POWER_CYCLING_ENABLED;
            fnDebugMsg("ON\r\n");
        }
        else {
            fnDebugMsg("OFF\r\n");
            iLowPowerLoopMode = LOW_POWER_CYCLING_DISABLED;
        }
        break;
    #endif
#endif
    case DO_DISPLAY_MEMORY_USE:                                          // memory use display
        fnDisplayMemoryUsage();
        break;

#if defined MONITOR_PERFORMANCE                                          // {25}
    case DO_DISPLAY_TASK_USE:
        fnDisplayTaskUse(*ptrInput);
        break;

    case DO_TASK_USE_RESET:
        fnDebugMsg("Task monitoring reset\r\n");
        ulMaximumIdle = 0xffffffff;                                      // flag that the monitoring should be reset after this task completes
        break;
#endif

    case DO_DISPLAY_UP_TIME:
        {
            CHAR cUpTime[20];
            *(fnUpTime(cUpTime)) = 0;
            fnDebugMsg("\r\nThe device has been operating for ");
            fnDebugMsg(cUpTime);
        }
        return;
#if !defined REMOVE_PORT_INITIALISATIONS
      case DO_GET_DDR:                                                   // get present ddr setting {2}
          if ((*ptrInput < '1') || (*ptrInput > '4')) {
              fnDebugMsg("Bad port number\r\n");
              break;
          }
          fnDebugMsg("\r\nPort direction: ");
          if (fnPortInputConfig(*ptrInput) == 0) {
              fnDebugMsg("OUTPUT\r\n");
          }
          else {
              fnDebugMsg("INPUT\r\n");
          }
          break;

      case DO_OUTPUT:                                                    // set output port value {2}
          if ((*ptrInput < '1') || (*ptrInput > '4')) {
              fnDebugMsg("Invalid port\r\n");
              return;
          }
          if (fnPortInputConfig(*ptrInput)) {
              fnDebugMsg("Port is input\r\n");
              return;
          }
          else {
              unsigned char ucPresentPort = 0;
              unsigned char ucBit = 0x01;
              CHAR cPortBit = '1';
              while (cPortBit < '5') {                                   // get the present port states
                  if (fnPortState(cPortBit++) != 0) {
                      ucPresentPort |= ucBit;
                  }
                  ucBit <<= 1;
              }
              ucBit = (unsigned char)fnHexStrHex(ptrInput);              // the input to be modified
              ucBit = (1 << (ucBit - 1));                                // the bit represented by this input
              if (fnHexStrHex(++ptrInput) == 0x01) {                     // if a '1' is to be set
                  ucPresentPort |= ucBit;
              }
              else {                                                     // else set 0
                  ucPresentPort &= ~ucBit;
              }
              fnSetPortOut(ucPresentPort, 0);                            // set new port state
          }
          break;
#endif
#if defined TEST_CMSIS_CFFT                                              // {84}
      case DO_FFT:
          fnTestFFT((int)fnDecStrHex(ptrInput));
          break;
#endif
#if defined CRYPTOGRAPHY                                                 // {84}
    #if defined CRYPTO_AES
      case DO_AES128:
      case DO_AES192:
      case DO_AES256:
    #endif
    #if defined CRYPTO_SHA
      case DO_SHA256:
    #endif
          {
              typedef struct stALIGNED_BUFFER
              {
                  unsigned long  ulAlignedLongWord;                      // unused long word to ensure following data alignment
                  unsigned char  ucData[256/8];
              } ALIGNED_BUFFER;
              static const ALIGNED_BUFFER encryption_key = { 0, { 0x60,0x3d,0xeb,0x10,0x15,0xca,0x71,0xbe,0x2b,0x73,0xae,0xf0,0x85,0x7d,0x77,0x81,0x1f,0x35,0x2c,0x07,0x3b,0x61,0x08,0xd7,0x2d,0x98,0x10,0xa3,0x09,0x14,0xdf,0xf4 } };
              static const ALIGNED_BUFFER plaintext = { 0, { 0x60,0x3d,0xeb,0x10,0x15,0xca,0x71,0xbe,0x2b,0x73,0xae,0xf0,0x85,0x7d,0x77,0x81,0x1f,0x35,0x2c,0x07,0x3b,0x61,0x08,0xd7,0x2d,0x98,0x10,0xa3,0x09,0x14,0xdf,0xf4 } };
              ALIGNED_BUFFER ciphertext = {0};
              ALIGNED_BUFFER recovered  = {0};
              int i;
              int iKeyLength;
              switch (ucType) {
    #if defined CRYPTO_AES
              case DO_AES128:
                  iKeyLength = 128;
                  break;
              case DO_AES192:
                  iKeyLength = 192;
                  break;
              case DO_AES256:
                  iKeyLength = 256;
                  break;
    #endif
    #if defined CRYPTO_SHA
              case DO_SHA256:
                  extern int fnSHA256(const unsigned char *ptrInput, unsigned char *ptrOutput, unsigned long ulLength, int iMode);
                  TOGGLE_TEST_OUTPUT();
                  fnSHA256(plaintext.ucData, recovered.ucData, sizeof(plaintext.ucData), 0);
                  TOGGLE_TEST_OUTPUT();
                  for (i = 0; i < 32; i++) {
                      fnDebugHex(recovered.ucData[i], (WITH_SPACE | sizeof(unsigned char) | WITH_LEADIN));
                  }
                  return;
    #endif
              }

              TOGGLE_TEST_OUTPUT();
              fnAES_Init(AES_COMMAND_AES_SET_KEY_ENCRYPT, encryption_key.ucData, iKeyLength); // register the encryption key
              TOGGLE_TEST_OUTPUT();
              fnAES_Cipher((AES_COMMAND_AES_ENCRYPT | AES_COMMAND_AES_RESET_IV), plaintext.ucData, ciphertext.ucData, sizeof(plaintext.ucData)); // encrypt the data content
              TOGGLE_TEST_OUTPUT();
              for (i = 0; i < 32; i++) {
                  fnDebugHex(ciphertext.ucData[i], (WITH_SPACE | sizeof(unsigned char) | WITH_LEADIN));
              }
              fnDebugMsg("\r\n");
              TOGGLE_TEST_OUTPUT();
              fnAES_Init(AES_COMMAND_AES_SET_KEY_DECRYPT, encryption_key.ucData, iKeyLength); // register the decryption key
              TOGGLE_TEST_OUTPUT();
              fnAES_Cipher((AES_COMMAND_AES_DECRYPT | AES_COMMAND_AES_RESET_IV), (const unsigned char *)ciphertext.ucData, recovered.ucData, sizeof(ciphertext.ucData)); // decrypt the data content
              TOGGLE_TEST_OUTPUT();
              fnDebugMsg("AES");
              fnDebugDec(iKeyLength, 0);
              if (uMemcmp(plaintext.ucData, recovered.ucData, sizeof(recovered.ucData)) == 0) {
                  TOGGLE_TEST_OUTPUT();
                  fnDebugMsg(" passed\n\r");
                  TOGGLE_TEST_OUTPUT();
              }
              else {
                  TOGGLE_TEST_OUTPUT();
                  fnDebugMsg(" failed\n\r");
              }
          }
          break;
#endif
#if defined TEST_FLEXIO                                                  // {91}
      case DO_FLEXIO_ON:
    #if defined KINETIS_WITH_PCC
          SELECT_PCC_PERIPHERAL_SOURCE(FLEXIO0, FLEXIO_PCC_SOURCE);      // select the PCC clock used by LPI2C0
    #endif
          POWER_UP(2, SIM_SCGC2_FLEXIO0);
          FLEXIO0_CTRL = (FLEXIO_CTRL_SWRST | FLEXIO_CTRL_FLEXEN | FLEXIO_CTRL_DBGE); // reset and enable
          FLEXIO0_CTRL = (FLEXIO_CTRL_FLEXEN | FLEXIO_CTRL_DBGE);
          fnDebugMsg("Flex io powered\r\n");
          fnDebugMsg("Verion ID = ");
          while ((FLEXIO0_CTRL & FLEXIO_CTRL_SWRST) != 0) {              // wait until module has completed its reset
          }
          fnDebugHex(FLEXIO0_VERID, (sizeof(unsigned long) | WITH_LEADIN | WITH_CR_LF));
          fnDebugMsg("Parameter = ");
          fnDebugHex(FLEXIO0_PARAM, (sizeof(unsigned long) | WITH_LEADIN));
          break;
      case DO_FLEXIO_PIN:
          {
              unsigned char ucPinRef = (unsigned char)fnDecStrHex(ptrInput);
              switch (ucPinRef) {
    #if defined KINETIS_K80
              case 0:
                  fnDebugMsg("PTB0");
                  _CONFIG_PERIPHERAL(B, 0, (PB_0_FXIO0_D0 | PORT_SRE_FAST | PORT_DSE_HIGH | PORT_PS_UP_ENABLE));
                  break;
              case 1:
                  fnDebugMsg("PTB1");
                  _CONFIG_PERIPHERAL(B, 1, (PB_1_FXIO0_D1 | PORT_SRE_FAST | PORT_DSE_HIGH | PORT_PS_UP_ENABLE));
                  break;
              case 2:
                  fnDebugMsg("PTB2");
                  _CONFIG_PERIPHERAL(B, 2, (PB_2_FXIO0_D2 | PORT_SRE_FAST | PORT_DSE_HIGH | PORT_PS_UP_ENABLE));
                  break;
              case 3:
                  fnDebugMsg("PTB3");
                  _CONFIG_PERIPHERAL(B, 3, (PB_3_FXIO0_D3 | PORT_SRE_FAST | PORT_DSE_HIGH | PORT_PS_UP_ENABLE));
                  break;
              case 4:
                  fnDebugMsg("PTB10");
                  _CONFIG_PERIPHERAL(B, 10, (PB_10_FXIO0_D4 | PORT_SRE_FAST | PORT_DSE_HIGH | PORT_PS_UP_ENABLE));
                  break;
              case 5:
                  fnDebugMsg("PTB11");
                  _CONFIG_PERIPHERAL(B, 11, (PB_11_FXIO0_D5 | PORT_SRE_FAST | PORT_DSE_HIGH | PORT_PS_UP_ENABLE));
                  break;
              case 6:
                  fnDebugMsg("PTB18");
                  _CONFIG_PERIPHERAL(B, 18, (PB_18_FXIO0_D6 | PORT_SRE_FAST | PORT_DSE_HIGH | PORT_PS_UP_ENABLE));
                  break;
              case 7:
                  fnDebugMsg("PTB19");
                  _CONFIG_PERIPHERAL(B, 19, (PB_19_FXIO0_D7 | PORT_SRE_FAST | PORT_DSE_HIGH | PORT_PS_UP_ENABLE));
                  break;
              case 8:
                  fnDebugMsg("PTB20");
                  _CONFIG_PERIPHERAL(B, 20, (PB_20_FXIO0_D8 | PORT_SRE_FAST | PORT_DSE_HIGH | PORT_PS_UP_ENABLE));
                  break;
              case 9:
                  fnDebugMsg("PTB21");
                  _CONFIG_PERIPHERAL(B, 21, (PB_21_FXIO0_D9 | PORT_SRE_FAST | PORT_DSE_HIGH | PORT_PS_UP_ENABLE));
                  break;
              case 10:
                  fnDebugMsg("PTB22");
                  _CONFIG_PERIPHERAL(B, 22, (PB_22_FXIO0_D10 | PORT_SRE_FAST | PORT_DSE_HIGH | PORT_PS_UP_ENABLE));
                  break;
              case 11:
                  fnDebugMsg("PTB23");
                  _CONFIG_PERIPHERAL(B, 23, (PB_23_FXIO0_D11 | PORT_SRE_FAST | PORT_DSE_HIGH | PORT_PS_UP_ENABLE));
                  break;
              case 12:
                  fnDebugMsg("PTC0");
                  _CONFIG_PERIPHERAL(C, 0, (PC_0_FXIO0_D12 | PORT_SRE_FAST | PORT_DSE_HIGH | PORT_PS_UP_ENABLE));
                  break;
              case 13:
                  fnDebugMsg("PTC1");
                  _CONFIG_PERIPHERAL(C, 1, (PC_1_FXIO0_D13 | PORT_SRE_FAST | PORT_DSE_HIGH | PORT_PS_UP_ENABLE));
                  break;
              case 14:
                  fnDebugMsg("PTC6");
                  _CONFIG_PERIPHERAL(C, 6, (PC_6_FXIO0_D14 | PORT_SRE_FAST | PORT_DSE_HIGH | PORT_PS_UP_ENABLE));
                  break;
              case 15:
                  fnDebugMsg("PTC7");
                  _CONFIG_PERIPHERAL(C, 7, (PC_7_FXIO0_D15 | PORT_SRE_FAST | PORT_DSE_HIGH | PORT_PS_UP_ENABLE));
                  break;
              case 16:
                  fnDebugMsg("PTC8");
                  _CONFIG_PERIPHERAL(C, 8, (PC_8_FXIO0_D16 | PORT_SRE_FAST | PORT_DSE_HIGH | PORT_PS_UP_ENABLE));
                  break;
              case 17:
                  fnDebugMsg("PTC9");
                  _CONFIG_PERIPHERAL(C, 9, (PC_9_FXIO0_D17 | PORT_SRE_FAST | PORT_DSE_HIGH | PORT_PS_UP_ENABLE));
                  break;
              case 18:
                  fnDebugMsg("PTC10");
                  _CONFIG_PERIPHERAL(C, 10, (PC_10_FXIO0_D18 | PORT_SRE_FAST | PORT_DSE_HIGH | PORT_PS_UP_ENABLE));
                  break;
              case 19:
                  fnDebugMsg("PTC11");
                  _CONFIG_PERIPHERAL(C, 11, (PC_11_FXIO0_D19 | PORT_SRE_FAST | PORT_DSE_HIGH | PORT_PS_UP_ENABLE));
                  break;
              case 20:
                  fnDebugMsg("PTC14");
                  _CONFIG_PERIPHERAL(C, 14, (PC_14_FXIO0_D20 | PORT_SRE_FAST | PORT_DSE_HIGH | PORT_PS_UP_ENABLE));
                  break;
              case 21:
                  fnDebugMsg("PTC15");
                  _CONFIG_PERIPHERAL(C, 15, (PC_15_FXIO0_D21 | PORT_SRE_FAST | PORT_DSE_HIGH | PORT_PS_UP_ENABLE));
                  break;
              case 22:
                  fnDebugMsg("PTD0");
                  _CONFIG_PERIPHERAL(D, 0, (PD_0_FXIO0_D22 | PORT_SRE_FAST | PORT_DSE_HIGH | PORT_PS_UP_ENABLE));
                  break;
              case 23:
                  fnDebugMsg("PTD1");
                  _CONFIG_PERIPHERAL(D, 1, (PD_1_FXIO0_D23 | PORT_SRE_FAST | PORT_DSE_HIGH | PORT_PS_UP_ENABLE));
                  break;
              case 24:
                  fnDebugMsg("PTD8");
                  _CONFIG_PERIPHERAL(D, 8, (PD_8_FXIO0_D24 | PORT_SRE_FAST | PORT_DSE_HIGH | PORT_PS_UP_ENABLE));
                  break;
              case 25:
                  fnDebugMsg("PTD9");
                  _CONFIG_PERIPHERAL(D, 9, (PD_9_FXIO0_D25 | PORT_SRE_FAST | PORT_DSE_HIGH | PORT_PS_UP_ENABLE));
                  break;
              case 26:
                  fnDebugMsg("PTD10");
                  _CONFIG_PERIPHERAL(D, 10, (PD_10_FXIO0_D26 | PORT_SRE_FAST | PORT_DSE_HIGH | PORT_PS_UP_ENABLE));
                  break;
              case 27:
                  fnDebugMsg("PTD11");
                  _CONFIG_PERIPHERAL(D, 11, (PD_11_FXIO0_D27 | PORT_SRE_FAST | PORT_DSE_HIGH | PORT_PS_UP_ENABLE));
                  break;
              case 28:
                  fnDebugMsg("PTD12");
                  _CONFIG_PERIPHERAL(D, 12, (PD_12_FXIO0_D28 | PORT_SRE_FAST | PORT_DSE_HIGH | PORT_PS_UP_ENABLE));
                  break;
              case 29:
                  fnDebugMsg("PTD13");
                  _CONFIG_PERIPHERAL(D, 13, (PD_13_FXIO0_D29 | PORT_SRE_FAST | PORT_DSE_HIGH | PORT_PS_UP_ENABLE));
                  break;
              case 30:
                  fnDebugMsg("PTD14");
                  _CONFIG_PERIPHERAL(D, 14, (PD_14_FXIO0_D30 | PORT_SRE_FAST | PORT_DSE_HIGH | PORT_PS_UP_ENABLE));
                  break;
              case 31:
                  fnDebugMsg("PTD15");
                  _CONFIG_PERIPHERAL(D, 15, (PD_15_FXIO0_D31 | PORT_SRE_FAST | PORT_DSE_HIGH | PORT_PS_UP_ENABLE));
                  break;
    #elif defined KINETIS_KL28
              case 0:
                  fnDebugMsg("PTE16");
                  _CONFIG_PERIPHERAL(E, 16, (PE_16_FXIO0_D0 | PORT_SRE_FAST | PORT_DSE_HIGH | PORT_PS_UP_ENABLE));
                  break;
              case 1:
                  fnDebugMsg("PTE17");
                  _CONFIG_PERIPHERAL(E, 17, (PE_17_FXIO0_D1 | PORT_SRE_FAST | PORT_DSE_HIGH | PORT_PS_UP_ENABLE));
                  break;
              case 2:
                  fnDebugMsg("PTE18");
                  _CONFIG_PERIPHERAL(E, 18, (PE_18_FXIO0_D2 | PORT_SRE_FAST | PORT_DSE_HIGH | PORT_PS_UP_ENABLE));
                  break;
              case 3:
                  fnDebugMsg("PTE19");
                  _CONFIG_PERIPHERAL(E, 19, (PE_19_FXIO0_D3 | PORT_SRE_FAST | PORT_DSE_HIGH | PORT_PS_UP_ENABLE));
                  break;
              case 4:
                  fnDebugMsg("PTE20");
                  _CONFIG_PERIPHERAL(E, 20, (PE_20_FXIO0_D4 | PORT_SRE_FAST | PORT_DSE_HIGH | PORT_PS_UP_ENABLE));
                  break;
              case 5:
                  fnDebugMsg("PTE21");
                  _CONFIG_PERIPHERAL(E, 21, (PE_21_FXIO0_D5 | PORT_SRE_FAST | PORT_DSE_HIGH | PORT_PS_UP_ENABLE));
                  break;
              case 6:
                  fnDebugMsg("PTE22");
                  _CONFIG_PERIPHERAL(E, 22, (PE_22_FXIO0_D6 | PORT_SRE_FAST | PORT_DSE_HIGH | PORT_PS_UP_ENABLE));
                  break;
              case 7:
                  fnDebugMsg("PTE23");
                  _CONFIG_PERIPHERAL(E, 23, (PE_23_FXIO0_D7 | PORT_SRE_FAST | PORT_DSE_HIGH | PORT_PS_UP_ENABLE));
                  break;
              case 8:
                  fnDebugMsg("PTB0");
                  _CONFIG_PERIPHERAL(B, 0, (PB_0_FXIO0_D8 | PORT_SRE_FAST | PORT_DSE_HIGH | PORT_PS_UP_ENABLE));
                  break;
              case 9:
                  fnDebugMsg("PTB1");
                  _CONFIG_PERIPHERAL(B, 1, (PB_1_FXIO0_D9 | PORT_SRE_FAST | PORT_DSE_HIGH | PORT_PS_UP_ENABLE));
                  break;
              case 10:
                  fnDebugMsg("PTB2");
                  _CONFIG_PERIPHERAL(B, 2, (PB_2_FXIO0_D10 | PORT_SRE_FAST | PORT_DSE_HIGH | PORT_PS_UP_ENABLE));
                  break;
              case 11:
                  fnDebugMsg("PTB3");
                  _CONFIG_PERIPHERAL(B, 3, (PB_3_FXIO0_D11 | PORT_SRE_FAST | PORT_DSE_HIGH | PORT_PS_UP_ENABLE));
                  break;
              case 12:
                  fnDebugMsg("PTB8");
                  _CONFIG_PERIPHERAL(B, 8, (PB_8_FXIO0_D12 | PORT_SRE_FAST | PORT_DSE_HIGH | PORT_PS_UP_ENABLE));
                  break;
              case 13:
                  fnDebugMsg("PTB9");
                  _CONFIG_PERIPHERAL(B, 9, (PB_9_FXIO0_D13 | PORT_SRE_FAST | PORT_DSE_HIGH | PORT_PS_UP_ENABLE));
                  break;
              case 14:
                  fnDebugMsg("PTB10");
                  _CONFIG_PERIPHERAL(B, 10, (PB_10_FXIO0_D14 | PORT_SRE_FAST | PORT_DSE_HIGH | PORT_PS_UP_ENABLE));
                  break;
              case 15:
                  fnDebugMsg("PTB11");
                  _CONFIG_PERIPHERAL(B, 11, (PB_11_FXIO0_D15 | PORT_SRE_FAST | PORT_DSE_HIGH | PORT_PS_UP_ENABLE));
                  break;
              case 16:
                  fnDebugMsg("PTB16");
                  _CONFIG_PERIPHERAL(B, 16, (PB_16_FXIO0_D16 | PORT_SRE_FAST | PORT_DSE_HIGH | PORT_PS_UP_ENABLE));
                  break;
              case 17:
                  fnDebugMsg("PTB17");
                  _CONFIG_PERIPHERAL(B, 17, (PB_17_FXIO0_D17 | PORT_SRE_FAST | PORT_DSE_HIGH | PORT_PS_UP_ENABLE));
                  break;
              case 18:
                  fnDebugMsg("PTB18");
                  _CONFIG_PERIPHERAL(B, 18, (PB_18_FXIO0_D18 | PORT_SRE_FAST | PORT_DSE_HIGH | PORT_PS_UP_ENABLE));
                  break;
              case 19:
                  fnDebugMsg("PTB19");
                  _CONFIG_PERIPHERAL(B, 19, (PB_19_FXIO0_D19 | PORT_SRE_FAST | PORT_DSE_HIGH | PORT_PS_UP_ENABLE));
                  break;
              case 20:
                  fnDebugMsg("PTC7");
                  _CONFIG_PERIPHERAL(C, 7, (PC_7_FXIO0_D20 | PORT_SRE_FAST | PORT_DSE_HIGH | PORT_PS_UP_ENABLE));
                  break;
              case 21:
                  fnDebugMsg("PTC8");
                  _CONFIG_PERIPHERAL(C, 8, (PC_8_FXIO0_D21 | PORT_SRE_FAST | PORT_DSE_HIGH | PORT_PS_UP_ENABLE));
                  break;
              case 22:
                  fnDebugMsg("PTC9");
                  _CONFIG_PERIPHERAL(C, 9, (PC_9_FXIO0_D22 | PORT_SRE_FAST | PORT_DSE_HIGH | PORT_PS_UP_ENABLE));
                  break;
              case 23:
                  fnDebugMsg("PTC10");
                  _CONFIG_PERIPHERAL(C, 10, (PC_10_FXIO0_D23 | PORT_SRE_FAST | PORT_DSE_HIGH | PORT_PS_UP_ENABLE));
                  break;
    #endif
              default:
                  fnDebugMsg("Invalid pin");
                  return;
              }
              fnDebugMsg(" configured");
          }
          break;
    #if defined KINETIS_KL28 || defined KINETIS_K80
        case DO_FLEXIO_PIN_STATE:
            fnDebugMsg("Flexio pin state ");
            fnDebugHex(FLEXIO0_PIN, (sizeof(unsigned long) | WITH_LEADIN));
            break;
        #if 0                                                            // test code for trimming fast IRC on KL28 -however not possible due to chip errata
        case DO_SHOW_TRIM:
            fnDebugMsg("Fast IRC trim ");
            fnDebugHex(SCG_FIRCTCFG, (sizeof(unsigned long) | WITH_LEADIN));
            fnDebugHex(SCG_FIRCSTAT, (WITH_SPACE | sizeof(unsigned long) | WITH_LEADIN | WITH_CR_LF));
            break;
        case DO_CHANGE_TRIM_COARSE:
        case DO_CHANGE_TRIM_FINE:
            {
                unsigned char ucTrim = (unsigned char)fnHexStrHex(ptrInput);
                SCG_FIRCTCFG = SCG_FIRCTCFG_TRIMSRC_OSC;
                SCG_FIRCCSR |= SCG_FIRCCSR_FIRCTREN;
                SCG_FIRCCSR &= ~(SCG_FIRCCSR_FIRCTRUP);                  // allow writing the register
                if (DO_CHANGE_TRIM_FINE == ucType) {
                    SCG_FIRCSTAT = ((SCG_FIRCSTAT & ~(SCG_FIRCSTAT_TRIMFINE_MASK)) | (ucTrim & SCG_FIRCSTAT_TRIMFINE_MASK));
                }
                else {
                    SCG_FIRCSTAT = ((SCG_FIRCSTAT & ~(SCG_FIRCSTAT_TRIMCOAR_MASK)) | ((ucTrim & SCG_FIRCSTAT_TRIMCOAR_MASK) << SCG_FIRCSTAT_TRIMCOAR_SHIFT));
                }
            }
            break;
        #endif
    #endif
        case DO_FLEXIO_STATUS:
            fnDebugMsg("Shifter status ");
            fnDebugHex(FLEXIO0_SHIFTSTAT, (sizeof(unsigned long) | WITH_LEADIN));
            fnDebugHex(FLEXIO0_SHIFTSIEN, (WITH_SPACE | sizeof(unsigned long) | WITH_LEADIN | WITH_CR_LF));
            fnDebugMsg("Shifter error ");
            fnDebugHex(FLEXIO0_SHIFTERR, (sizeof(unsigned long) | WITH_LEADIN));
            fnDebugHex(FLEXIO0_SHIFTEIEN, (WITH_SPACE | sizeof(unsigned long) | WITH_LEADIN | WITH_CR_LF));
            fnDebugMsg("Timer status ");
            fnDebugHex(FLEXIO0_TIMSTAT, (sizeof(unsigned long) | WITH_LEADIN));
            fnDebugHex(FLEXIO0_TIMIEN, (WITH_SPACE | sizeof(unsigned long) | WITH_LEADIN));
            break;
#elif defined MMDVSQ_AVAILABLE                                           // {88}
      case DO_SQRT:
      case DO_DIV:
          {
              unsigned long ulInput[2] = { 0, 0 };
              int iInput = 0;
              unsigned long ulDividend = 0;
              int iHex;
              FOREVER_LOOP() {
                  iHex = 0;
                  if (*ptrInput == '0') {
                      ptrInput++;
                      if ((*ptrInput == 'x') || (*ptrInput == 'X')) {    // recognise hex inputs
                          ptrInput++;
                          iHex = 1;
                      }
                      else {
                          ptrInput--;
                      }
                  }
                  if (iHex != 0) {
                      ulInput[iInput] = fnHexStrHex(ptrInput);           // hex input
                  }
                  else {
                      ulInput[iInput] = fnDecStrHex(ptrInput);           // decimal input
                  }
                  iInput++;
                  if (DO_SQRT == ucType) {
                      fnDebugMsg("SQRT = ");
                      TOGGLE_TEST_OUTPUT();
                      ulInput[0] = fnIntegerSQRT(ulInput[0]);            // perform integer square root calculation
                      TOGGLE_TEST_OUTPUT();
                      iInput = 1;
                      break;
                  }
                  if ((iInput >= 2)) {
                      break;
                  }
                  fnJumpWhiteSpace(&ptrInput);
              }
              while (iInput-- != 0) {
                  if (DO_DIV == ucType) {
                      // Division
                      //
                      if (ulInput[1] == 0) {
                          fnDebugMsg("Divide by zero!");
                          break;
                      }
                      if (iInput == 1) {
                          fnDebugMsg("Quotient = ");
                          ulDividend = ulInput[0];                       // save for second use later
                          TOGGLE_TEST_OUTPUT();
                          ulInput[0] = fnFastSignedIntegerDivide(ulDividend, ulInput[1]); // perform fast signed division
                          TOGGLE_TEST_OUTPUT();
                      }
                      else {
                          fnDebugMsg("\r\nRemainder = ");
                          TOGGLE_TEST_OUTPUT();
                          ulInput[0] = fnFastSignedModulo(ulDividend, ulInput[1]); // perform fast unsigned modulo calculation
                          TOGGLE_TEST_OUTPUT();
                      }
                  }
                  fnDebugDec(ulInput[0], DISPLAY_NEGATIVE);
                  fnDebugMsg(" [");
                  fnDebugHex(ulInput[0], (WITH_LEADIN | sizeof(ulInput[0])));
                  fnDebugMsg("]");
              }
          }
          break;
#endif
#if defined SUPPORT_LCD && defined LCD_CONTRAST_CONTROL                  // {88}
      case DO_SET_CONTRAST:
          temp_pars->temp_parameters.ucGLCDContrastPWM = (unsigned char)fnDecStrHex(ptrInput);
          if (temp_pars->temp_parameters.ucGLCDContrastPWM > 100) {      // limit to 0..100% range
              temp_pars->temp_parameters.ucGLCDContrastPWM = 100;
          }
          fnSetLCDContrast(temp_pars->temp_parameters.ucGLCDContrastPWM);
          // Fall though intentionally to show the setting
          //
      case DO_GET_CONTRAST:
          fnDebugMsg("LCD contrast = ");
          fnDebugDec(temp_pars->temp_parameters.ucGLCDContrastPWM, 0);
          fnDebugMsg("%\r\n");
          break;
#endif
#if defined GLCD_BACKLIGHT_CONTROL                                       // {75}
      case DO_SET_BACKLIGHT:
          temp_pars->temp_parameters.ucGLCDBacklightPWM = (unsigned char)fnDecStrHex(ptrInput);
          if (temp_pars->temp_parameters.ucGLCDBacklightPWM > 100) {
              temp_pars->temp_parameters.ucGLCDBacklightPWM = 100;
          }
          fnSetBacklight();
          // Fall though intentionally to show the setting
          //
      case DO_GET_BACKLIGHT:
          fnDebugMsg("Backlight intensity = ");
          fnDebugDec(temp_pars->temp_parameters.ucGLCDBacklightPWM, 0);
          fnDebugMsg("%\r\n");
          break;
#endif

#if !defined REMOVE_PORT_INITIALISATIONS
    #if defined USE_PARAMETER_BLOCK
      case DO_SAVE_PORT:
          fnSavePorts();
          break;
    #endif

      case DO_INPUT:                                                     // read an input bit {2}
            if ((*ptrInput < '1') || (*ptrInput > '4')) {
                fnDebugMsg("Invalid port\r\n");
                return;
            }
            fnDebugMsg("Port state: ");
            if (fnPortState(*ptrInput)) {
              fnDebugMsg("1\r\n");
            }
            else {
              fnDebugMsg("0\r\n");
            }
            break;

      case DO_DDR:                                                       // set port direction (and use)
          if (fnSetPort(ucType, ptrInput)) {                             // modify the port
              fnDebugMsg("\r\n??");                                      // signal input not recognised
              return;
          }
          break;
#endif
#if defined MEMORY_DEBUGGER
    case DO_MEM_DISPLAY:
    #if !defined NO_FLASH_SUPPORT
    case DO_STORAGE_DISPLAY:                                             // {69}
    #endif
        {
            static int iType = BYTE_ACCESS;                              // default is to use bytes
            static CAST_POINTER_ARITHMETIC ptrMemory = 0;
            CHAR cAscii[(16 * 2) + 5];                                   // output line buffer
            CHAR *ptrAscii = cAscii;                                     // set pointer to start of output line buffer
            unsigned long ulValue;
            int iLoopLength;
            int iDisplayCount = 16;                                      // default is to display a single line
            if (*ptrInput != 0) {                                        // if there is an address given
                ptrMemory = (CAST_POINTER_ARITHMETIC)fnHexStrHex(ptrInput); // get the address
                fnJumpWhiteSpace(&ptrInput);
                if (*ptrInput != 0) {
                    if ((*ptrInput == 'L') || (*ptrInput == 'l')) {
                        iType = LONG_WORD_ACCESS;
                        ptrInput++;
                    }
                    else if ((*ptrInput == 'W') || (*ptrInput == 'w')) {
                        iType = SHORT_WORD_ACCESS;
                        ptrInput++;
                    }
                    else if ((*ptrInput == 'B') || (*ptrInput == 'b')) { // default
                        iType = BYTE_ACCESS;
                        ptrInput++;
                    }
                    if (*ptrInput != 0) {
                        iDisplayCount = (int)fnDecStrHex(ptrInput);
                    }
                }
            }
            fnDebugMsg("Memory Display\r\n");
            cAscii[0] = ' ';
            cAscii[1] = ' ';
            while (iDisplayCount > 0) {
    #if defined _WINDOWS
                if (ucType != DO_STORAGE_DISPLAY) {
                    ptrMemory = (CAST_POINTER_ARITHMETIC)ucTestBuffer;
                }
    #endif
                fnDebugHex((unsigned long)ptrMemory, (sizeof(ptrMemory) | WITH_LEADIN));
                fnDebugMsg("    ");
                iLoopLength = 16;
                ptrAscii = &cAscii[2];
                while (iLoopLength) {
                    iDisplayCount--;                                     // {65} display the requested amount of the type and not bytes
                    switch (iType) {
                    case BYTE_ACCESS:
    #if defined NO_FLASH_SUPPORT
                        ulValue = *(unsigned char *)ptrMemory;
    #else
                        if (ucType == DO_STORAGE_DISPLAY) {
                            unsigned char ucValue;
                            fnGetParsFile((unsigned char *)ptrMemory, &ucValue, BYTE_ACCESS);
                            ulValue = ucValue;
                        }
                        else {
                            ulValue = *(unsigned char *)ptrMemory;
                        }
    #endif
                        fnDebugHex((unsigned char)ulValue, (unsigned char)(WITH_SPACE | iType));
                        break;
                    case SHORT_WORD_ACCESS:
    #if defined NO_FLASH_SUPPORT
                        ulValue = *(unsigned short *)ptrMemory;
    #else
                        if (ucType == DO_STORAGE_DISPLAY) {
                            unsigned short usValue;
                            fnGetParsFile((unsigned char *)ptrMemory, (unsigned char *)&usValue, SHORT_WORD_ACCESS);
                            ulValue = usValue;
                        }
                        else {
                            ulValue = *(unsigned short *)ptrMemory;
                        }
    #endif
                        fnDebugHex((unsigned short)ulValue, (unsigned char)(WITH_SPACE | iType));
                        break;
                    case LONG_WORD_ACCESS:
    #if defined NO_FLASH_SUPPORT
                        ulValue = *(unsigned long *)ptrMemory;
    #else
                        if (ucType == DO_STORAGE_DISPLAY) {
                            fnGetParsFile((unsigned char *)ptrMemory, (unsigned char *)&ulValue, LONG_WORD_ACCESS);
                        }
                        else {
                            ulValue = *(unsigned long *)ptrMemory;
                        }
    #endif
                        fnDebugHex(ulValue, (unsigned char)(WITH_SPACE | iType));
                        break;
                    default:
                        return;                                          // never should happen
                    }
                    ptrMemory += iType;
                    iLoopLength -= iType;
                    ptrAscii = fnDisplayAscii(ulValue, ptrAscii, iType);
                    if (iDisplayCount <= 0) {
                        break;
                    }
                }
                *ptrAscii++ = '\r';
                *ptrAscii++ = '\n';
                *ptrAscii   = 0;
                fnDebugMsg(cAscii);                                      // display the content as ASCII
            }
        }
        break;
        #if defined SPI_FILE_SYSTEM && defined SPI_FLASH_S25FL1_K
    case DO_STORAGE_PAGE:
        #endif
    case DO_MEM_FILL:
    case DO_MEM_WRITE:
        #if !defined NO_FLASH_SUPPORT
    case DO_STORAGE_WRITE:                                               // {69}
    case DO_STORAGE_FILL:                                                // {69}
        #endif
        if (*ptrInput != 0) {                                            // if there is an address given
            static int iType = BYTE_ACCESS;                              // default is byte
            int iResult = 0;
        #if !defined NO_FLASH_SUPPORT
            int iStorageAccess;
        #endif
            CAST_POINTER_ARITHMETIC ptrMemory = (CAST_POINTER_ARITHMETIC)fnHexStrHex(ptrInput); // get the address
            int iFillLength = 1;                                         // default
            if ((DO_MEM_FILL == ucType) || (DO_STORAGE_FILL == ucType)) {
                fnDebugMsg("Fill");
            }
        #if defined SPI_FILE_SYSTEM && defined SPI_FLASH_S25FL1_K
            else if (DO_STORAGE_PAGE == ucType) {
                int i = 0;
                unsigned char ucPattern[SPI_FLASH_PAGE_LENGTH];          // buffer
                fnDebugMsg("Page\r\n");
                while (i < SPI_FLASH_PAGE_LENGTH) {
                    ucPattern[i] = (unsigned char)i;                     // set pattern to buffer
                    i++;
                }
                iResult = fnWriteBytesFlash((unsigned char *)ptrMemory, ucPattern, SPI_FLASH_PAGE_LENGTH); // write the page to flash (this is intended for testing SPI flash onl)
                break;
            }
        #endif
            else {
                fnDebugMsg("Write");
            }
            fnJumpWhiteSpace(&ptrInput);
            if (*ptrInput != 0) {
                unsigned long ulValue;
                if (*ptrInput != 0) {
                    if ((*ptrInput == 'L') || (*ptrInput == 'l')) {
                        iType = LONG_WORD_ACCESS;
                        ptrInput++;
                    }
                    else if ((*ptrInput == 'W') || (*ptrInput == 'w')) {
                        iType = SHORT_WORD_ACCESS;
                        ptrInput++;
                    }
                    else if ((*ptrInput == 'B') || (*ptrInput == 'b')) {
                        iType = BYTE_ACCESS;
                        ptrInput++;
                    }
                    fnJumpWhiteSpace(&ptrInput);
                    ulValue = fnHexStrHex(ptrInput);                     // {69} the value to write
                    fnJumpWhiteSpace(&ptrInput);
                    if ((DO_MEM_FILL == ucType) || (DO_STORAGE_FILL == ucType)) { // number of repetitions follows
                        if (*ptrInput != 0) {
                            iFillLength = (int)fnDecStrHex(ptrInput);
                        }
                    }
                }
    #if !defined NO_FLASH_SUPPORT
                if ((DO_STORAGE_WRITE == ucType) || (DO_STORAGE_FILL == ucType)) {
                    iStorageAccess = 1;
                }
                else {
                    iStorageAccess = 0;
                }
    #endif
    #if defined _WINDOWS && !defined NO_FLASH_SUPPORT
                if (iStorageAccess == 0) {
                    ptrMemory = (CAST_POINTER_ARITHMETIC)ucTestBuffer;
                }
    #endif
                while (iFillLength-- > 0) {
                    switch (iType) {
                    case BYTE_ACCESS:
    #if defined NO_FLASH_SUPPORT
                        *(unsigned char *)ptrMemory = (unsigned char)ulValue;
    #else
                        if (iStorageAccess != 0) {
                            unsigned char ucValue = (unsigned char)ulValue;
                            iResult = fnWriteBytesFlash((unsigned char *)ptrMemory, &ucValue, BYTE_ACCESS);
                        }
                        else {
                            *(unsigned char *)ptrMemory = (unsigned char)ulValue;
                        }
    #endif
                        break;
                    case SHORT_WORD_ACCESS:
    #if defined NO_FLASH_SUPPORT
                        *(unsigned short *)ptrMemory = (unsigned short)ulValue;
    #else
                        if (iStorageAccess != 0) {
                            unsigned short usValue = (unsigned short)ulValue;
                            iResult = fnWriteBytesFlash((unsigned char *)ptrMemory, (unsigned char *)&usValue, SHORT_WORD_ACCESS);
                        }
                        else {
                            *(unsigned short *)ptrMemory = (unsigned short)ulValue;
                        }
    #endif
                        break;
                    case LONG_WORD_ACCESS:
    #if defined NO_FLASH_SUPPORT
                        *(unsigned long *)ptrMemory = ulValue;
    #else
                        if (iStorageAccess != 0) {
                            iResult = fnWriteBytesFlash((unsigned char *)ptrMemory, (unsigned char *)&ulValue, LONG_WORD_ACCESS);
                        }
                        else {
                            *(unsigned long *)ptrMemory = ulValue;
                        }
    #endif
                        break;
                    default:
                        break;
                    }
                    if (iResult != 0) {
                        fnDebugMsg(" - Failed\r\n");
                    }
                    ptrMemory += iType;
                }
            }
            fnDebugMsg(" - OK\r\n");
        }
        break;
    #if !defined NO_FLASH_SUPPORT
        #if defined SPI_FILE_SYSTEM && defined SPI_FLASH_S25FL1_K
    case DO_SUSPEND_PAGE:
        #endif
    case DO_STORAGE_ERASE:                                               // {69}
        if (*ptrInput != 0) {                                            // if there is an address given
            CAST_POINTER_ARITHMETIC ptrMemory = (CAST_POINTER_ARITHMETIC)fnHexStrHex(ptrInput); // get the address
            MAX_FILE_LENGTH Length = 0;                                  // default is a single sector
            fnJumpWhiteSpace(&ptrInput);                                 // move to length
            if (*ptrInput != 0) {
                Length = (MAX_FILE_LENGTH)fnHexStrHex(ptrInput);         // {74}
            }
            if (fnEraseFlashSector((unsigned char *)ptrMemory, Length) == 0) { // erase the defined length
        #if defined SPI_FILE_SYSTEM && defined SPI_FLASH_S25FL1_K
                if (DO_SUSPEND_PAGE == ucType) {                         // test suspending the erase with an immediate read
                    fnDoHardware(DO_STORAGE_DISPLAY, "81000");           // read from another sector
                    fnDoHardware(DO_STORAGE_PAGE, "81000");              // write to another sector
                }
        #endif
                fnDebugMsg("Erased\r\n");
            }
            else {
                fnDebugMsg("Erase failed!\r\n");
            }
        }
        break;
    #endif
#endif
#if defined EZPORT_CLONER                                                // {55}
      case DO_EZINIT:                                                    // configure ports for use by ezport interface - holding the device in reset
          fnConfigEz();
          break;
      case DO_EZRESET:
          fnNegateResetEz(1);                                            // remove reset with ezport chip select asserted
          break;
      case DO_EZSTART:
          fnNegateResetEz(0);                                            // remove reset without ezport chip select asserted
          break;
      case DO_EZSTATUS:
          fnGetStatusEz(1);
          break;
    #if defined _M5223X
      case DO_EZGETREADY:
          fnPrepareEz(ptrInput);
          break;
    #else
      case DO_EZUNSECURE:
          if ((fnGetStatusEz(0) & EZCOMMAND_SR_FS) == 0) {
              fnDebugMsg("Device not secured - doing nothing\r\n"); // fnProgEz("U"); can be used to set the unsecure bit in flash in case the first sector was erased
              break;
          }
          // Fall-through intentionally to bulk-erase the device and unsecure it
          //
    #endif
      case DO_EZBULK:
          fnEraseEz(0);
          break;
      case DO_EZREAD:
          fnReadEz(ptrInput);
          break;
      case DO_EZCOMMAND:
          fnCommandEz(ptrInput);
          break;
      case DO_EZSECT:                                                    // erase a single sector
          fnEraseEz(ptrInput);
          break;
      case DO_EZPROG:                                                    // test programming
          fnProgEz(ptrInput);
          break;
      case DO_EZVERIF:
      case DO_EZCLONE:                                                   // non-secure clone
      case DO_EZSCLONE:                                                  // secure clone
          if ((FTFL_FSEC & FTFL_FSEC_SEC_MASK) == FTFL_FSEC_SEC_UNSECURE) {
              #define MAX_EZ_MODE_ATTEMPTS (50)
              int iAttempts = 0;
              do {
                  if (iAttempts >= MAX_EZ_MODE_ATTEMPTS) {               // allow maximum atempts before erroring
                      fnDebugMsg("EzPort slave not responding!!\r\n");
                      return;
                  }
                  if (iAttempts%8 == 0) {
                      fnConfigEz();                                      // ensure configured and target in reset
                      fnNegateResetEz(1);                                // remove reset with ezport chip select asserted
                  }
                  iAttempts++;
              } while (fnGetStatusEz(0) == 0xff);
              fnDebugMsg("\r\nCloner V1.0 ");
              iCloningActive = ucType;
              if (DO_EZVERIF == ucType) {
                  if ((fnGetStatusEz(0) & EZCOMMAND_SR_FS) != 0) {
                      iCloningActive = 0;
                      fnDebugMsg("Target is secured - aborting\r\n");
                      return;
                  }
                  if ((*ptrInput == 'b') || (*ptrInput == 'B')) {        // blank check rather than cloning check
                      iCloningActive |= CLONING_VERIFYING_BLANK;
                      fnDebugMsg("blank check\r\n");
                  }
                  else {
                      iCloningActive |= CLONING_VERIFYING_CODE;
                      fnDebugMsg("verifying\r\n");
                  }
              }
              else {
                  fnDebugMsg("cloning\r\n");
                  fnEraseEz(0);                                          // start with a bulk erase to ensure unsecured and fully blank flash
                  iCloningActive |= CLONING_ERASING_BULK;
              }
              ulCloningAddress = (FLASH_START_ADDRESS);                  // start cloning at the start of flash
    #if defined EZPORT_CLONER_SKIP_REGIONS
              iCloningRegion = 0;
    #endif
              uTaskerStateChange(OWN_TASK, UTASKER_POLLING);             // go to polling mode
          }
          else {
              fnDebugMsg("Cloning not allowed (master is secured)!!\r\n");
          }
          break;
#endif
#if defined PWM_MEASUREMENT_DEVELOPMENT
        case 75:                                                         // temp
            fnMeasurePWM(PORTC, PORTC_BIT4);                             // test measuring a PWM input on this input
            break;
#endif
#if defined SUPPORT_LPTMR
        case 76:                                                         // temp test to check technique for reading value of counter
            {
                unsigned long ulCnt;
                LPTMR0_CNR = 0;                                          // write any value to the counter register so that it puts its present counter value into a temporay register
                ulCnt = LPTMR0_CNR;                                      // read the value from the temporary register
                fnDebugHex(ulCnt, (WITH_LEADIN | WITH_CR_LF | sizeof(ulCnt)));
            }
            break;
#endif
#if defined DEV1
        case 77:
        {
            unsigned char ucAnode = (unsigned char)fnHexStrHex(ptrInput); // get the anode value
            ucAnode &= 0x7f;
            iBlockLed = 1;
            fnDebugMsg("Anode set to ");
            fnDebugHex(ucAnode, (WITH_LEADIN | sizeof(ucAnode)));
            if (ucAnode & 0x01) {
                _SETBITS(D, PORTD_BIT6);
            }
            else {
                _CLEARBITS(D, PORTD_BIT6);
            }
            if (ucAnode & 0x02) {
                _SETBITS(D, PORTD_BIT5);
            }
            else {
                _CLEARBITS(D, PORTD_BIT5);
            }
            if (ucAnode & 0x04) {
                _SETBITS(D, PORTD_BIT4);
            }
            else {
                _CLEARBITS(D, PORTD_BIT4);
            }
            if (ucAnode & 0x08) {
                _SETBITS(D, PORTD_BIT3);
            }
            else {
                _CLEARBITS(D, PORTD_BIT3);
            }
            if (ucAnode & 0x10) {
                _SETBITS(D, PORTD_BIT2);
            }
            else {
                _CLEARBITS(D, PORTD_BIT2);
            }
            if (ucAnode & 0x20) {
                _SETBITS(D, PORTD_BIT1);
            }
            else {
                _CLEARBITS(D, PORTD_BIT1);
            }
            if (ucAnode & 0x40) {
                _SETBITS(D, PORTD_BIT0);
            }
            else {
                _CLEARBITS(D, PORTD_BIT0);
            }
        }
        break;
        case 78:
        {
            unsigned short usCathode = (unsigned short)fnHexStrHex(ptrInput); // get the cathode value
            usCathode &= 0x7fff;
            iBlockLed = 1;
            fnDebugMsg("Cathod set to ");
            fnDebugHex(usCathode, (WITH_LEADIN | sizeof(usCathode)));
            if (usCathode & 0x0001) {
                _SETBITS(C, PORTC_BIT3);
            }
            else {
                _CLEARBITS(C, PORTC_BIT3);
            }
            if (usCathode & 0x0002) {
                _SETBITS(C, PORTC_BIT2);
            }
            else {
                _CLEARBITS(C, PORTC_BIT2);
            }
            if (usCathode & 0x0004) {
                _SETBITS(C, PORTC_BIT1);
            }
            else {
                _CLEARBITS(C, PORTC_BIT1);
            }
            if (usCathode & 0x0008) {
                _SETBITS(C, PORTC_BIT0);
            }
            else {
                _CLEARBITS(C, PORTC_BIT0);
            }
            if (usCathode & 0x0010) {
                _SETBITS(B, PORTB_BIT19);
            }
            else {
                _CLEARBITS(B, PORTB_BIT19);
            }
            if (usCathode & 0x0020) {
                _SETBITS(B, PORTB_BIT18);
            }
            else {
                _CLEARBITS(B, PORTB_BIT18);
            }
            if (usCathode & 0x0040) {
                _SETBITS(B, PORTB_BIT17);
            }
            else {
                _CLEARBITS(B, PORTB_BIT17);
            }
            if (usCathode & 0x0080) {
                _SETBITS(B, PORTB_BIT16);
            }
            else {
                _CLEARBITS(B, PORTB_BIT16);
            }
            if (usCathode & 0x0100) {
                _SETBITS(B, PORTB_BIT3);
            }
            else {
                _CLEARBITS(B, PORTB_BIT3);
            }
            if (usCathode & 0x0200) {
                _SETBITS(B, PORTB_BIT2);
            }
            else {
                _CLEARBITS(B, PORTB_BIT2);
            }
            if (usCathode & 0x0400) {
                _SETBITS(B, PORTB_BIT1);
            }
            else {
                _CLEARBITS(B, PORTB_BIT1);
            }
            if (usCathode & 0x0800) {
                _SETBITS(C, PORTC_BIT8);
            }
            else {
                _CLEARBITS(C, PORTC_BIT8);
            }
            if (usCathode & 0x1000) {
                _SETBITS(C, PORTC_BIT9);
            }
            else {
                _CLEARBITS(C, PORTC_BIT9);
            }
            if (usCathode & 0x2000) {
                _SETBITS(C, PORTC_BIT10);
            }
            else {
                _CLEARBITS(C, PORTC_BIT10);
            }
            if (usCathode & 0x4000) {
                _SETBITS(C, PORTC_BIT11);
            }
            else {
                _CLEARBITS(C, PORTC_BIT11);
            }
        }
        break;
        case 79:
        {
            unsigned char ucRelay = (unsigned char)fnDecStrHex(ptrInput); // get the relay value
            if ((ucRelay < 1) || (ucRelay > 3)) {
                fnDebugMsg("Invalid relay - 1, 2 or 3");
                break;
            }
            fnJumpWhiteSpace(&ptrInput);
            if (*ptrInput == '1') {
                switch (ucRelay) {
                case 1:
                    _SETBITS(C, PORTC_BIT5);
                    break;
                case 2:
                    _SETBITS(C, PORTC_BIT4);
                    break;
                case 3:
#if defined CAN_INTERFACE
                    _SETBITS(A, PORTA_BIT5);
#else
                    _SETBITS(A, PORTA_BIT13);
#endif
                    break;
                }
            }
            else {
                switch (ucRelay) {
                case 1:
                    _CLEARBITS(C, PORTC_BIT5);
                    break;
                case 2:
                    _CLEARBITS(C, PORTC_BIT4);
                    break;
                case 3:
#if defined CAN_INTERFACE
                    _CLEARBITS(A, PORTA_BIT5);
#else
                    _CLEARBITS(A, PORTA_BIT13);
#endif
                    break;
                }
            }
        }
        break;
#endif
    }
}

#if defined USB_INTERFACE
static void fnDoUSB(unsigned char ucType, CHAR *ptrInput)                // USB group
{
    switch (ucType) {
    #if defined USE_USB_CDC                                              // {8}
        #if defined SERIAL_INTERFACE
    case DO_USB_RS232_MODE:                                              // set RS232<->USB link - exit is via USB disconnect
        fnDebugMsg("Connecting USB with RS232 - disconnect USB to quit...\r\n");
        usUSB_state |= ES_USB_RS232_MODE;
        break;
        #endif
    case DO_USB_DOWNLOAD:                                                // start a download via USB
        if (usUSB_state != ES_USB_COMMAND_MODE) {
            fnDebugMsg("Please use USB connection!!\r\n");
        }
        else {
        #if defined M52210DEMO
            fnDebugMsg("Download not supported on M52210DEMO!!\r\n");
        #else
            fnDebugMsg("Please start download:");
            usUSB_state |= ES_USB_DOWNLOAD_MODE;
        #endif
        }
        break;
    case DO_USB_SPEED:
        fnDebugMsg("Test starts in 1s - get ready to receive 10 MBytes of data....");
        uTaskerMonoTimer(OWN_TASK, (DELAY_LIMIT)(1 * SEC), E_TIMER_START_USB_TX);
        break;
    #endif
    #if defined USE_USB_HID_KEYBOARD && defined SUPPORT_FIFO_QUEUES
    case DO_USB_KEYBOARD:                                                // {77}
        usUSB_state |= ES_USB_KEYBOARD_MODE;                             // pass input to USB keyboard
        break;
        #if defined USB_KEYBOARD_DELAY
    case DO_USB_KEYBOARD_DELAY:
        if (*ptrInput != 0) {                                            // if further input is given
            temp_pars->temp_parameters.ucKeyboardInterCharacterDelay = (unsigned char)(fnDecStrHex(ptrInput) - 1);
        }
        fnDebugMsg("Keystroke delay = ");
        fnDebugDec((temp_pars->temp_parameters.ucKeyboardInterCharacterDelay + 1), 0);
        fnDebugMsg("ms");
        break;
        #endif
    #endif
    #if defined USB_CDC_HOST
    case DO_VIRTUAL_COM:                                                 // {82}
        fnDebugMsg("Debug input is passed to remote device - disconnect to terminate");
        usUSB_state |= ES_USB_RS232_MODE;
        break;
    #endif
    #if defined USE_USB_AUDIO
    case DO_USB_DELTA:
        fnDebugMsg("Delta = ");
        fnDebugDec((slDelta/(BUS_CLOCK/1000000)), DISPLAY_NEGATIVE);     // deviation from ideal (+/-) us
        fnDebugMsg("us\r\n");
        break;
    #endif
    }
}
#endif

#if defined SUPPORT_OLED
extern void fnSaveGrayScales(void);
extern unsigned char ucGrayScales[15 + 1];
static void fnDoOLED(unsigned char ucType, CHAR *ptrInput)               // OLED group
{
    switch (ucType) {
    case DO_OLED_GET_GRAY_SCALES:                                        // get gray levels
        {
            int x;
            fnDebugMsg("Present settings:\r\n");
            for (x = 1; x < 16; x++) {
                fnDebugMsg("GS");
                fnDebugDec(x, 0);
                fnDebugMsg(" = ");
                fnDebugDec(ucGrayScales[x], WITH_CR_LF);
            }
        }
        break;
    case DO_OLED_SET_GRAY_SCALE:                                         // set gray level value
        {
            unsigned char ucGrayScaleValue;
            unsigned char ucMin = 0;
            unsigned char ucMax = 64;
            unsigned char ucGrayScaleIndex = (unsigned char)fnDecStrHex(ptrInput);
            if ((ucGrayScaleIndex < 1) || (ucGrayScaleIndex > 15) || (fnJumpWhiteSpace(&ptrInput))) {
                fnDebugMsg("Invalid GS!\r\n");
                return;
            }
            ucGrayScaleValue = (unsigned char)fnDecStrHex(ptrInput);
            if (ucGrayScaleIndex != 1) {
                ucMin = ucGrayScales[ucGrayScaleIndex - 1];
            }
            if (ucGrayScaleIndex != 15) {
                ucMax = ucGrayScales[ucGrayScaleIndex + 1];
            }
            if ((ucGrayScaleValue <= ucMin) || (ucGrayScaleValue >= ucMax)) {
                fnDebugMsg("Invalid value!\r\n");
            }
            else {
                ucGrayScales[ucGrayScaleIndex] = ucGrayScaleValue;
                fnSaveGrayScales();
                fnDebugMsg("OK\r\n");
            }
        }
        break;
    }
}
#endif

#if defined I2C_INTERFACE && defined I2C_MASTER_LOADER                   // {89}
static void fnProgramI2CSlave(unsigned char ucSlaveAddress, int iCommand)
{
    static unsigned char ucAddressToProgram = 0;
    static MEMORY_RANGE_POINTER slave_firmware = 0;
    static MAX_FILE_LENGTH slave_firmwareLength = 0;
    unsigned char ucDelete[4];
    if (iCommand == 0) {
        if (0 == ucSlaveAddress) {
            fnDebugMsg("Invalid slave address (0x00)!");
            return;
        }
        slave_firmware = uOpenFile("0.bin");                             // location of firmware to send to the slave (uFileSystem "0.bin")
        slave_firmwareLength = uGetFileLength(slave_firmware);           // the length of the data to program
        if (slave_firmwareLength == 0) {
            fnDebugMsg("No program to send!");
            return;
        }
        slave_firmware += FILE_HEADER;
        fnDebugMsg("Commanding slave device at ");
        ucAddressToProgram = (ucSlaveAddress & 0xfe);                    // ensure the write address is used
        fnDebugHex(ucAddressToProgram, (WITH_LEADIN | sizeof(ucAddressToProgram)));
        fnDebugMsg(" to delete its memory...");
        ucDelete[0] = ucAddressToProgram;                                // slave write address
        ucDelete[1] = 0x01;                                              // write to 0x01 to command an erase
        ucDelete[2] = 0x52;                                              // magic number must match for the erase to be executed
        ucDelete[3] = 0x84;
        fnWrite(I2CPortID, ucDelete, sizeof(ucDelete));                  // send delete command
    }
    if (iCommand < 2) {                                                  // delete or request delete status
        ucDelete[0] = 3;                                                 // read three bytes of data (versin plus flash state)
        ucDelete[1] = (ucAddressToProgram | 0x01);                       // slave read address
        ucDelete[2] = OWN_TASK;                                          // wake our task when the read has completed
        fnRead(I2CPortID, ucDelete, 0);                                  // get the status
    }
    else {                                                               // programming
        if (slave_firmwareLength != 0) {                                 // if more data to program
            unsigned char ucProgram[60];                                 // small blocks are sent since the I2C ty buffer may be no larger than 64 bytes (see setting in i2c_tests.h)
            unsigned char ucLength = (sizeof(ucProgram) - 2);
            if (ucLength > slave_firmwareLength) {                       // if there is not enough remaining program data to fill the buffer
                ucLength = (unsigned char)slave_firmwareLength;          // send the remaining amount of data
            }
            ucProgram[0] = ucAddressToProgram;                           // slave write address
            ucProgram[1] = 0x00;                                         // write to 0 to program slave
            fnGetParsFile(slave_firmware, &ucProgram[2], ucLength);      // get next block of data to send
            fnWrite(I2CPortID, ucProgram, (ucLength + 2));               // send delete command
            slave_firmwareLength -= ucLength;
            slave_firmware += ucLength;
            ucProgram[0] = 5;                                            // read five bytes of data
            ucProgram[1] = (ucAddressToProgram | 0x01);                  // slave read address
            ucProgram[2] = OWN_TASK;                                     // wake our task when the read has completed
            fnRead(I2CPortID, ucProgram, 0);                             // get the programming status
        }
        else {                                                           // all data programmed
            unsigned char ucReset[4];
            ucReset[0] = ucAddressToProgram;                             // slave write address
            ucReset[1] = 0x02;                                           // write to 0x02 to command a reset
            ucReset[2] = 0x55;                                           // magic number must match for the erase to be executed
            ucReset[3] = 0xaa;
            fnWrite(I2CPortID, ucReset, sizeof(ucReset));                // send reset command
            fnDebugMsg("\r\nProgramming complete\r\n");
        }
    }
}
#endif

#if defined I2C_INTERFACE
    #if LPI2C_AVAILABLE > 0 && defined TEMP_LPI2C_TEST
    extern unsigned long ulRxLPI2Cpause;
    extern unsigned long ulTxLPI2Cpause;
    extern unsigned long ulChange;
    #endif
static void fnDoI2C(unsigned char ucType, CHAR *ptrInput)                // I2C group
{
    switch (ucType) {
    case DO_ACC_ON:                                                      // {68}
        iAccelOutput = 1;
        fnDebugMsg("Enabled\r\n");
        break;
    case DO_ACC_OFF:
        iAccelOutput = 0;
        fnDebugMsg("Disabled\r\n");
        break;
    #if defined I2C_INTERFACE && defined I2C_MASTER_LOADER               // {89}
    case DO_I2C_LOAD_FIRMWARE:
        {
            unsigned char ucI2C_slave_address = (unsigned char)fnHexStrHex(ptrInput);
            fnProgramI2CSlave(ucI2C_slave_address, 0);
        }
        break;
    #endif
    #if LPI2C_AVAILABLE > 0 && defined TEMP_LPI2C_TEST
    case 122:
        if (*ptrInput == '1') {
            fnDebugMsg("Tx pause 0.5ms");
            ulTxLPI2Cpause = 500;
        }
        else {
            fnDebugMsg("Tx pause removed");
            ulTxLPI2Cpause = 0;
        }
        break;
    case 123:
        if (*ptrInput == '1') {
            fnDebugMsg("Rx pause 0.5ms");
            ulRxLPI2Cpause = 500;
        }
        else {
            fnDebugMsg("Rx pause removed");
            ulRxLPI2Cpause = 0;
        }
        break;
    case 124:
        if (*ptrInput == '1') {
            fnDebugMsg("DOZEN = '1'");
            ulChange = 0x04;
        }
        else {
            fnDebugMsg("DOZEN = '0'");
            ulChange = 0x08;
        }
        break;
    case 125:
        if (*ptrInput == '1') {
            fnDebugMsg("DBGEN = '1'");
            ulChange = 0x10;
        }
        else {
            fnDebugMsg("DBGEN = '0'");
            ulChange = 0x20;
        }
        break;
    case 126:
        if (*ptrInput == '1') {
            fnDebugMsg("AUTOSTOP = '1'");
            ulChange = 0x01;
        }
        else {
            fnDebugMsg("AUTOSTOP = '0'");
            ulChange = 0x02;
        }
        break;
    #endif
    #if defined TEST_I2C_INTERFACE
    case DO_I2C_WRITE:                                                   // write a value to a specified I2C address
        {
        #if defined I2C_TWO_BYTE_ADDRESS
            #define I2C_DATA_POSITION 3
            unsigned short usAddress;
        #else
            #define I2C_DATA_POSITION 2
        #endif
            QUEUE_TRANSFER length;
            unsigned char ucTest[257];                                   // = {I2C_WRITE_ADDRESS, 0, 0};
            ucTest[0] = I2C_WRITE_ADDRESS;
        #if defined I2C_TWO_BYTE_ADDRESS
            usAddress = (unsigned short)fnHexStrHex(ptrInput);           // the address
            ucTest[1] = (unsigned char)(usAddress >> 8);
            ucTest[2] = (unsigned char)(usAddress);
        #else
            ucTest[1] = (unsigned char)fnHexStrHex(ptrInput);            // the address
        #endif
            if (fnJumpWhiteSpace(&ptrInput) != 0) {
                fnDebugMsg("Data missing\r\n");
                return;
            }
            ucTest[I2C_DATA_POSITION] = (unsigned char)fnHexStrHex(ptrInput); // the data value
            if (fnJumpWhiteSpace(&ptrInput) == 0) {
                int iRepeat = (I2C_DATA_POSITION - 1);
                length = (unsigned char)fnDecStrHex(ptrInput);           // repetition length for data
                if (length == 0) {
                    length = (I2C_DATA_POSITION - 1);
                }
        #if defined I2C_TWO_BYTE_ADDRESS
                else {
                    length++;
                }
        #endif
                while (iRepeat++ < length) {
                    ucTest[iRepeat + 1] = (ucTest[iRepeat] + 1);
                }
                length += 2;
            }
            else {
                length = 3;
            }
            fnWrite(I2CPortID, ucTest, length);
            fnDebugMsg("Written value ");
            fnDebugHex(ucTest[I2C_DATA_POSITION], (WITH_LEADIN | sizeof(ucTest[2])));
            fnDebugMsg(" to address ");
        #if defined I2C_TWO_BYTE_ADDRESS
            fnDebugHex(usAddress, (WITH_LEADIN | WITH_CR_LF | sizeof(usAddress)));
        #else
            fnDebugHex(ucTest[1], (WITH_LEADIN | WITH_CR_LF | sizeof(ucTest[1])));
        #endif
        }
        break;

    case DO_I2C_READ_PLUS_WRITE:
    case DO_I2C_READ:                                                    // read values from an I2C address
        {
        #if defined I2C_TWO_BYTE_ADDRESS
            unsigned short usAddress;
        #endif
            unsigned char ucSetAddress[I2C_DATA_POSITION];               // = {I2C_WRITE_ADDRESS, 0, (0)};
            unsigned char ucTest[3];                                     // = {1, I2C_READ_ADDRESS, OWN_TASK};
            ucSetAddress[0] = I2C_WRITE_ADDRESS;
        #if defined I2C_TWO_BYTE_ADDRESS
            usAddress = (unsigned short)fnHexStrHex(ptrInput);           // the address
            ucSetAddress[1] = (unsigned char)(usAddress >> 8);
            ucSetAddress[2] = (unsigned char)(usAddress);
        #else
            ucSetAddress[1] = (unsigned char)fnHexStrHex(ptrInput);      // the address
        #endif
            fnWrite(I2CPortID, (unsigned char *)ucSetAddress, sizeof(ucSetAddress)); // write the address to be read from
            if (fnJumpWhiteSpace(&ptrInput) == 0) {
                ucTest[0] = (unsigned char)fnDecStrHex(ptrInput);        // the length to be read           
            }
            else {
                ucTest[0] = 1;
            }
            ucTest[1] = I2C_READ_ADDRESS;
            ucTest[2] = OWN_TASK;
            fnRead(I2CPortID, (unsigned char *)ucTest, 0);               // start the read
            if (DO_I2C_READ_PLUS_WRITE == ucType) {
                fnWrite(I2CPortID, (unsigned char *)ucSetAddress, sizeof(ucSetAddress));
                fnDebugMsg("Reading (+ wr) ");
            }
            else {
                fnDebugMsg("Reading ");
            }
            fnDebugDec(ucTest[0], sizeof(ucTest[1]));
            fnDebugMsg(" bytes from address ");
        #if defined I2C_TWO_BYTE_ADDRESS
            fnDebugHex(usAddress, (WITH_LEADIN | WITH_CR_LF | sizeof(usAddress)));
        #else
            fnDebugHex(ucSetAddress[1], (WITH_LEADIN | WITH_CR_LF | sizeof(ucSetAddress[1])));
        #endif
        }
        break;

    case DO_I2C_READ_NO_ADDRESS:
        {
            unsigned char ucTest[3];                                     // = {1, I2C_READ_ADDRESS, OWN_TASK};
            ucTest[0] = (unsigned char)fnDecStrHex(ptrInput);            // the length
            if (ucTest[0] == 0) {
                fnDebugMsg("Invalid length\n\r");
                break;
            }
            ucTest[1] = I2C_READ_ADDRESS;
            ucTest[2] = OWN_TASK;
            fnRead(I2CPortID, (unsigned char *)ucTest, 0);               // start the read
            fnDebugMsg("Reading ");
            fnDebugDec(ucTest[0], sizeof(ucTest[1]));
            fnDebugMsg(" bytes from present address\n\r");
        }
        break;
    #endif
    }
}
#endif

#if defined SDCARD_SUPPORT || defined SPI_FLASH_FAT || defined FLASH_FAT || defined USB_MSD_HOST // {17}{81}

static void fnDisplayDiskSize(unsigned long ulSectors, unsigned short usBytesPerSector) // {43}
{
    if (ulSectors >= 0x800000) {                                         // note that 512 byte sectors are assumed
        fnDebugDec((ulSectors/2), 0);                                    // the number of bytes cannot be held in a 32 bit number so display it in kBytes
        fnDebugMsg(" kBytes");
    }
    else {
        fnDebugDec((ulSectors * 512), 0);                                // the value can be displayed in bytes
        fnDebugMsg(" bytes");
    }
}


// Handle SD-card disk commands
//
static int fnDoDisk(unsigned char ucType, CHAR *ptrInput)
{
    static UTLISTDIRECTORY utListDirectory[DISK_COUNT];                  // list directory object for a single user
#if defined UTFAT_UNDELETE                                               // {60}
    CHAR *ptrUndelInput = 0;
#endif
#if defined VERIFY_NAND && defined UTFAT_WRITE                           // {45}
    if (DO_TEST_NAND == ucType) {                                        // write pattern to NAND Flash for low-level test purposes
        unsigned long ulBuffer[512/4];
        int iUserBlock;
        int iError;
        uMemset(ulBuffer, 0, sizeof(ulBuffer));                          // blank sector content
        for (iUserBlock = 0; iUserBlock < 1807; iUserBlock++) {          // write a value to the first sector of each user block (valid for 32M NAND with 90% block utilisation)
            ulBuffer[0] = iUserBlock;                                    // mark the sector with the block number
            if ((iError = fnWriteSector(ucPresentDisk, (unsigned char *)ulBuffer, (iUserBlock * 32))) != UTFAT_SUCCESS) { // 32 sectors in a block assumed - write the pattern
                fnDebugMsg("Write error: ");
                fnDebugHex(iUserBlock, sizeof(iUserBlock));
                fnDebugDec(iError, (WITH_SPACE | DISPLAY_NEGATIVE | WITH_CR_LF));
                return 0;
            }
        }
        fnDebugMsg("Pattern set\r\n");
        return 0;
    }
    else if (DO_VERIFY_NAND == ucType) {                                 // verify previous low level test patterns in NAND and report any errors detected
        unsigned long ulBuffer[512/4];
        unsigned long ulUserBlock;
        int iError;
        for (ulUserBlock = 0; ulUserBlock < 1807; ulUserBlock++) {       // read back the first sector of each user block and check that the content is correct - report first error found
            if ((iError = fnReadSector(ucPresentDisk, (unsigned char *)ulBuffer, (ulUserBlock * 32)))) { // 32 sectors in a block assumed - write the pattern
                fnDebugMsg("Read error: ");
                fnDebugHex(ulUserBlock, sizeof(ulUserBlock));
                fnDebugDec(iError, (WITH_SPACE | DISPLAY_NEGATIVE | WITH_CR_LF));
                return 0;
            }
            if (ulBuffer[0] != ulUserBlock) {                             // check that each user block has the expected content
                fnDebugMsg("Error in user block ");
                fnDebugHex(ulUserBlock, (WITH_LEADIN | WITH_CR_LF | sizeof(ulUserBlock)));
                return 0;
            }
        }
        fnDebugMsg("Check successful\r\n");
        return 0;
    }
#endif
#if DISK_COUNT > 1
    if (DO_DISK_NUMBER == ucType) {
        fnDebugMsg("Disk ");
    #if defined DISK_C
        if ((*ptrInput == 'C') || (*ptrInput == 'c')) {
            fnDebugMsg("C\r\n");
            ucPresentDisk = DISK_C;
            return 0;
        }
    #endif
    #if defined DISK_D
        if ((*ptrInput == 'D') || (*ptrInput == 'd')) {
            fnDebugMsg("D\r\n");
            ucPresentDisk = DISK_D;
            return 0;
        }
    #endif
    #if defined DISK_E
        if ((*ptrInput == 'E') || (*ptrInput == 'e')) {
            fnDebugMsg("E\r\n");
            ucPresentDisk = DISK_E;
            return 0;
        }
    #endif
        fnDebugMsg("not avaiable\r\n");
        return 0;
    }
#endif
    if ((DO_FORMAT != ucType) && (DO_FORMAT_FULL != ucType) && (DO_DISPLAY_SECTOR != ucType)
#if defined TEST_SDCARD_SECTOR_WRITE                                     // {52}
        && (DO_WRITE_SECTOR != ucType) && (DO_WRITE_MULTI_SECTOR != ucType) && (DO_WRITE_MULTI_SECTOR_PRE != ucType)
#endif
#if defined NAND_FLASH_FAT
        && (DO_DISPLAY_SECTOR_INFO != ucType) && (DO_DELETE_REMAP_INFO != ucType) && (DO_DISPLAY_PAGE != ucType) && (DO_DELETE_FAT != ucType)
#endif
        && (DO_INFO != ucType)) {
        if (ptr_utDirectory[ucPresentDisk] == 0) {
            ptr_utDirectory[ucPresentDisk] = utAllocateDirectory(ucPresentDisk, UT_PATH_LENGTH); // allocate a directory for use by this module associated with D: and reserve its path name string length
        }
        if ((ptr_utDirectory[ucPresentDisk]->usDirectoryFlags & UTDIR_VALID) == 0) { // directory not valid
            if (utOpenDirectory(0, ptr_utDirectory[ucPresentDisk]) != UTFAT_SUCCESS) { // open the root directory
#if DISK_COUNT > 1 || !defined SDCARD_SUPPORT
                fnDebugMsg("No Disk ready\r\n");
#else
                fnDebugMsg("No SD-Card ready\r\n");
#endif
                return 0;
            }
            utListDirectory[ucPresentDisk].ptr_utDirObject = ptr_utDirectory[ucPresentDisk]; // the list directory is always referenced to the main directory object
        }
    }
    switch (ucType) {
#if defined UTFAT_UNDELETE && defined UTFAT_WRITE                        // {60}
    case DO_UNDELETE:
        ptrUndelInput = ptrInput;                                        // save pointer to the file to be modified and its new name
        ptrInput = 0;                                                    // search only in present directory
#endif
#if defined UTFAT_UNDELETE || defined UTFAT_EXPERT_FUNCTIONS             // {60}
    case DO_DIR_DELETED:                                                 // display just deleted entries in a directory
#endif
#if defined UTFAT_EXPERT_FUNCTIONS
    case DO_DIR_HIDDEN:                                                  // display just hidden entries in a directory
#endif
    case DO_DIR:                                                         // display present directory
        if ((iFATstalled == 0) && (utLocateDirectory(ptrInput, &utListDirectory[ucPresentDisk]) < UTFAT_SUCCESS)) { // open a list referenced to the main directory
            fnDebugMsg("Invalid directory\r\n");
        }
        else {                                                           // list the directory content
            static FILE_LISTING fileList = {0};
            const UTDISK *ptrDiskInfo;
            int iResult;
            CHAR cBuffer[MAX_UTFAT_FILE_NAME + DOS_STYLE_LIST_ENTRY_LENGTH]; // temporary string buffer for listing
            fileList.usMaxItems = 1;
            fileList.ptrBuffer = cBuffer;
            fileList.usBufferLength = sizeof(cBuffer);
            fileList.ucStyle = DOS_TYPE_LISTING;                         // DOS style listing requested
#if defined UTFAT_UNDELETE || defined UTFAT_EXPERT_FUNCTIONS             // {60}
            if (DO_DIR_DELETED == ucType) {
                fileList.ucStyle |= DELETED_TYPE_LISTING;                // list deleted entries and not normal entries
            }
    #if defined UTFAT_UNDELETE
            else if (DO_UNDELETE == ucType) {
                fileList.ucStyle |= (DELETED_TYPE_LISTING | INVISIBLE_TYPE_LISTING);
            }
    #endif
    #if defined UTFAT_EXPERT_FUNCTIONS
            else if (DO_DIR_HIDDEN == ucType) {
                fileList.ucStyle |= (HIDDEN_TYPE_LISTING);
            }
    #endif
#endif
            if (iFATstalled == 0) {
                fnDebugMsg("Directory ");
                fnDebugMsg(ptr_utDirectory[ucPresentDisk]->ptrDirectoryPath);
                if (ptr_utDirectory[ucPresentDisk]->usRelativePathLocation > 3) { // if the relative path is not the root directory
                    fnDebugMsg("\\");
                }
                if ((ptrInput != 0) &&  (*ptrInput != 0)) {
                    fnDebugMsg(ptrInput);
                }
                fnDebugMsg("\r\n\n");
            }
            else {
                iFATstalled = 0;
            }
            FOREVER_LOOP() {
#if defined UTFAT_UNDELETE || defined UTFAT_EXPERT_FUNCTIONS
                if (fnWrite(DebugHandle, 0, (sizeof(cBuffer) + 3)) == 0) // check whether there is enough space in the output buffer to accept the next entry 
#else
                if (fnWrite(DebugHandle, 0, sizeof(cBuffer)) == 0)       // check whether there is enough space in the output buffer to accept the next entry 
#endif
                {
                    fnDriver(DebugHandle, MODIFY_WAKEUP, (MODIFY_TX | OWN_TASK)); // we want to be woken when the queue is free again
#if defined UTFAT_UNDELETE || defined UTFAT_EXPERT_FUNCTIONS
                    if (DO_DIR_DELETED == ucType) {
                        iFATstalled = STALL_DIRD_LISTING;
                    }
    #if defined UTFAT_EXPERT_FUNCTIONS
                    else if (DO_DIR_HIDDEN == ucType) {
                        iFATstalled = STALL_DIRH_LISTING;
                    }
    #endif
                    else {
                        iFATstalled = STALL_DIR_LISTING;
                    }
#else
                    iFATstalled = STALL_DIR_LISTING;
#endif
                    return 1;
                }
                iResult = utListDir(&utListDirectory[ucPresentDisk], &fileList);
                if (fileList.usStringLength != 0) {
#if defined UTFAT_UNDELETE || defined UTFAT_EXPERT_FUNCTIONS
                    if (DO_DIR_DELETED == ucType) {
                        fnDebugMsg("[D]");
                    }
#endif
#if defined UTFAT_UNDELETE && defined UTFAT_WRITE                        // {60}
                    if (DO_UNDELETE == ucType) {
                        int iTestName = (fileList.usStringLength - 2);
                        int iTestLength = 0;
                        CHAR *ptrInputName = ptrUndelInput;
                        while ((*ptrInputName != ' ') && (*ptrInputName != 0)) {
                            ptrInputName++;
                            iTestName--;
                            iTestLength++;
                        }
                        if ((iTestLength != 0) && (uMemcmp(&cBuffer[iTestName], ptrUndelInput, iTestLength) == 0)) { // if deleted entry matched
                            CHAR *ptrBuf = &cBuffer[fileList.usStringLength - 2]; // before carriage return/line feed                                  
                            if (utUndeleteFile(&utListDirectory[ucPresentDisk]) != UTFAT_SUCCESS) {
                                ptrBuf = uStrcpy(ptrBuf, " {could not be undeleted!!}");
                            }
                            else {
                                ptrBuf = uStrcpy(ptrBuf, " {undeleted}");
                            }
                            *ptrBuf++ = '\r';
                            *ptrBuf++ = '\n';
                            fileList.usStringLength = (ptrBuf - cBuffer);
                            ptrUndelInput = 0;                           // only first file that matches is undeleted
                        }
                    }
#endif
                    fnWrite(DebugHandle, (unsigned char *)cBuffer, (QUEUE_TRANSFER)fileList.usStringLength);
                }
                if (iResult == UTFAT_NO_MORE_LISTING_ITEMS_FOUND) {
                    break;                                               // end of the directory content
                }
            }
            fnDebugDec(fileList.usFileCount, 0);
            fnDebugMsg(" files with ");
            fnDebugDec(fileList.ulFileSizes, 0);
            fnDebugMsg(" bytes\r\n");
            fnDebugDec(fileList.usDirectoryCount, 0);
            fnDebugMsg(" directories, ");
            uMemset(&fileList, 0, sizeof(fileList));                     // reset for next use
            ptrDiskInfo = fnGetDiskInfo(ucPresentDisk);
            if ((ptrDiskInfo->usDiskFlags & FSINFO_VALID) && (ptrDiskInfo->utFileInfo.ulFreeClusterCount != 0xffffffff)) {
                fnDisplayDiskSize((ptrDiskInfo->utFileInfo.ulFreeClusterCount * ptrDiskInfo->utFAT.ucSectorsPerCluster), ptrDiskInfo->utFAT.usBytesPerSector); // {43}
                fnDebugMsg(" free\r\n");
            }
            else {
                iFATstalled = STALL_COUNTING_CLUSTERS;
                utFreeClusters(ucPresentDisk, OWN_TASK);                 // start count of free clusters - the result will be displayed on completion
                return 1;
            }
        }
        break;
#if defined UTFAT_EXPERT_FUNCTIONS                                       // {60}
    case DO_INFO_DELETED:
    case DO_INFO_FILE:
        {
            unsigned long ulAccessMode;
            UTFILE utTempFile;                                           // temporary file object
          //utTempFile.ptr_utDirObject = ptr_utDirectory[ucPresentDisk]; // set in utOpenFile() from utFAT2.0
            if (DO_INFO_DELETED == ucType) {
                ulAccessMode = (UTFAT_OPEN_FOR_READ | UTFAT_DISPLAY_INFO | UTFAT_OPEN_DELETED);
            }
            else {
                ulAccessMode = (UTFAT_OPEN_FOR_READ | UTFAT_DISPLAY_INFO);
            }
            if (utOpenFile(ptrInput, &utTempFile, ptr_utDirectory[ucPresentDisk], ulAccessMode) < 0) { // {62} open the file or directory and display its details to debug interface
                fnDebugMsg("File/directory not found\r\n");
            }
            utCloseFile(&utTempFile);
        }
        break;
#endif

#if !defined LOW_MEMORY
    #if defined UTFAT_WRITE                                              // {45}
    case DO_WRITE_FILE:                                                  // write extra content to file
    #endif
    case DO_PRINT_FILE:                                                  // print the content of a file (ASCII)
        {
            static UTFILE utFile;                                        // local file object
            unsigned short usOpenAttributes = (UTFAT_OPEN_FOR_READ | UTFAT_MANAGED_MODE);
            unsigned char ucTemp[256];                                   // temp buffer to retrieve a block of data from the file
          //utFile.ptr_utDirObject = ptr_utDirectory[ucPresentDisk];
            utFile.ownerTask = OWN_TASK;
    #if defined UTFAT_WRITE                                              // {45}
            if (DO_WRITE_FILE == ucType) {
                usOpenAttributes = (UTFAT_OPEN_FOR_WRITE | UTFAT_APPEND | UTFAT_MANAGED_MODE); // {39}
            }
    #endif
            if ((iFATstalled == STALL_PRINTING_FILE) || (utOpenFile(ptrInput, &utFile, ptr_utDirectory[ucPresentDisk], usOpenAttributes) == UTFAT_PATH_IS_FILE)) { // {62} open a file referenced to the directory object
                int iLoop;
    #if defined UTFAT_WRITE                                              // {45}
                if (DO_WRITE_FILE == ucType) {                           // add input to file
                    static unsigned char ucContent = 0x55;
                    uMemset(ucTemp, ucContent, sizeof(ucTemp));          // fill with a pattern
                    ucContent++;                                         // change for next write
                  //utSeek(&utFile, 0, UTFAT_SEEK_END);                  // move to the end of the file {39} use UTFAT_APPEND to automatically move to end of file
                    if (utWriteFile(&utFile, ucTemp, sizeof(ucTemp)) != UTFAT_SUCCESS) {
                        fnDebugMsg("write failed");
                    }
                    else {
                        fnDebugMsg("New file length = ");
                        fnDebugDec(utFile.ulFileSize, 0);
                    }
                    fnDebugMsg("\r\n");
                    utCloseFile(&utFile);
                    break;
                }
    #endif
                do {
                    if (fnWrite(DebugHandle, 0, sizeof(ucTemp)) == 0) {  // check whether there is enough space in the output buffer to accept the next block
                        fnDriver(DebugHandle, MODIFY_WAKEUP, (MODIFY_TX | OWN_TASK)); // we want to be woken when the queue is free again
                        iFATstalled = STALL_PRINTING_FILE;
                        return 1;
                    }
                    iFATstalled = 0;
                    if (utReadFile(&utFile, ucTemp, sizeof(ucTemp)) != UTFAT_SUCCESS) {
                        fnDebugMsg("\r\nREAD ERROR occurred");
                        break;
                    }
                    for (iLoop = 0; iLoop < utFile.usLastReadWriteLength; iLoop++) {
                        if ((ucTemp[iLoop] < 0x0a) || (ucTemp[iLoop] > 'z')) {
                            ucTemp[iLoop] = '.';                         // non-printable characters replaced
                        }
                    }
                    fnWrite(DebugHandle, ucTemp, utFile.usLastReadWriteLength); // print the read data buffer
                    if (utFile.usLastReadWriteLength < sizeof(ucTemp)) { // complete file content has been read
                        break;
                    }
                } FOREVER_LOOP();
                fnDebugMsg("\r\n");
                utCloseFile(&utFile);
            }
            else {
                fnDebugMsg("File not found\r\n");
            }
        }
        break;
#endif
#if defined NAND_FLASH_FAT
    case DO_DISPLAY_SECTOR_INFO:
        {
            extern void fnPrintSectorDetails(unsigned long ulUserSector);
            fnPrintSectorDetails(fnHexStrHex(ptrInput));
        }
        break;
    case DO_DELETE_REMAP_INFO:
        {
            extern void fnDeleteRemapTable(void);
            fnDeleteRemapTable();                                        // only use during development due to loss of all data
        }
        break;
    case DO_DELETE_FAT:
        {
            extern void fnDeleteFAT(void);
            fnDeleteFAT();                                               // only use during development due to loss of all data
        }
        break;
    case DO_DISPLAY_PAGE:
#endif
#if !defined LOW_MEMORY
    #if defined UTFAT_MULTIPLE_BLOCK_READ
    case DO_DISPLAY_MULTI_SECTOR:                                        // multiple sector (to test faster reading)
    #endif
    case DO_DISPLAY_SECTOR:
        {
            int i, j;
            unsigned long ulSectorNumber = fnHexStrHex(ptrInput);        // the sector number
    #if defined NAND_FLASH_FAT
            unsigned long ulBuffer[528/sizeof(unsigned long)];
            UTDISK *ptrDiskInfo = (UTDISK *)fnGetDiskInfo(ucPresentDisk);
            if (DO_DISPLAY_PAGE == ucType) {
                fnDebugMsg("Reading page ");
                ptrDiskInfo->usDiskFlags |= DISK_TEST_MODE;              // set test mode which returns physical page instead of sector, plus the spare area data
            }
            else {
                fnDebugMsg("Reading sector ");
            }
    #else
            unsigned long ulBuffer[512/sizeof(unsigned long)];           // long word aligned buffer for efficiency
            fnDebugMsg("Reading sector ");
    #endif
            fnDebugHex(ulSectorNumber, (WITH_LEADIN | sizeof(ulSectorNumber)));
    #if (defined _WINDOWS || defined _LITTLE_ENDIAN) && defined UTFAT_SECT_BIG_ENDIAN // {61}
            fnDebugMsg(" (big-endian view)");
    #elif (!defined _LITTLE_ENDIAN) && defined UTFAT_SECT_LITTLE_ENDIAN
            fnDebugMsg(" (little-endian view)");
    #endif
    #if defined UTFAT_MULTIPLE_BLOCK_READ
            if (DO_DISPLAY_MULTI_SECTOR == ucType) {
                fnPrepareBlockRead(ucPresentDisk, 20);                   // prepare for a block read of 20 sectors
            }
            for (i = 0; i < 20; i++) {                                   // read 20 consecutive sectors (block read)
                TOGGLE_TEST_OUTPUT();
                if (fnReadSector(ucPresentDisk, (unsigned char *)ulBuffer, ulSectorNumber++) != 0) { // the the sector content to a buffer
                    fnDebugMsg(" FAILED!!\r\n");
                    return 0;
                }
                TOGGLE_TEST_OUTPUT();
                if (DO_DISPLAY_SECTOR == ucType) {
                    break;
                }
            }
    #else
            if (fnReadSector(ucPresentDisk, (unsigned char *)ulBuffer, ulSectorNumber) != 0) { // the the sector content to a buffer
                fnDebugMsg(" FAILED!!\r\n");
                break;
            }
    #endif
            fnDebugMsg("\r\n");
            for (i = 0; i < 8; i++) {                                    // display the read data
                for (j = 0; j < 16; j++) {
    #if (defined _WINDOWS || defined _LITTLE_ENDIAN) && defined UTFAT_SECT_BIG_ENDIAN // {61}
                    fnDebugHex(BIG_LONG_WORD(ulBuffer[(i * 16) + j]), (WITH_LEADIN | WITH_SPACE | sizeof(ulBuffer[0])));
    #elif (!defined _LITTLE_ENDIAN) && defined UTFAT_SECT_LITTLE_ENDIAN
                    fnDebugHex(LITTLE_LONG_WORD(ulBuffer[(i * 16) + j]), (WITH_LEADIN | WITH_SPACE | sizeof(ulBuffer[0])));
    #else
                    fnDebugHex(ulBuffer[(i * 16) + j], (WITH_LEADIN | WITH_SPACE | sizeof(ulBuffer[0])));
    #endif
                }
                fnDebugMsg("\r\n");
            }
    #if defined NAND_FLASH_FAT
            if (DO_DISPLAY_PAGE == ucType) {
                for (j = 0; j < 4; j++) {
                    fnDebugHex(ulBuffer[(8 * 16) + j], (WITH_LEADIN | WITH_SPACE | sizeof(ulBuffer[0])));
                }
                fnDebugMsg("\r\n");
                ptrDiskInfo->usDiskFlags &= ~DISK_TEST_MODE;             // remove test mode
            }
    #endif
        }
        break;
#endif
#if defined TEST_SDCARD_SECTOR_WRITE && defined UTFAT_WRITE              // {44}{45}
    #if defined UTFAT_MULTIPLE_BLOCK_WRITE
    case DO_WRITE_MULTI_SECTOR:
        #if defined UTFAT_PRE_ERASE
    case DO_WRITE_MULTI_SECTOR_PRE:
        #endif
    #endif
    case DO_WRITE_SECTOR:                                                // {86} write a number of bytes to a sector at a specified offset in the sector
        {
            unsigned long ulSectorNumber = fnHexStrHex(ptrInput);        // the sector number
            unsigned short usOffset = 0;
            unsigned char  ucPattern = 0x00;
            unsigned long  ulBuffer[512/sizeof(unsigned long)];
            unsigned char *ptrStart = (unsigned char *)ulBuffer;
            unsigned short usByteCount = 1;
            unsigned short usThisWriteLength;
            if (fnJumpWhiteSpace(&ptrInput) == 0) {                      // move over the input to the next parameter
                usOffset = (unsigned short)fnDecStrHex(ptrInput);
                if (fnJumpWhiteSpace(&ptrInput) == 0) {                  // move over the input to the next parameter
                    ucPattern = (unsigned char)fnHexStrHex(ptrInput);
                    if (fnJumpWhiteSpace(&ptrInput) == 0) {
                        usByteCount = (unsigned short)fnDecStrHex(ptrInput);
                        if (usByteCount == 0) {
                            usByteCount = 1;
                        }
                    }
                }
                usOffset %= 512;
            }
            if (fnReadSector(ucPresentDisk, (unsigned char *)ulBuffer, ulSectorNumber) != 0) { // read the present sector content
                fnDebugMsg(" FAILED to read!!\r\n");
                break;
            }
            ptrStart += usOffset;
            usThisWriteLength = usByteCount;
            if ((usOffset + usByteCount) > 512) {                        // limit to a single sector
                usThisWriteLength = (512 - usOffset);
            }
            uMemset(ptrStart, ucPattern, usThisWriteLength);             // modify the sector content in the buffer
            fnDebugMsg("Writing sector ");
            fnDebugHex(ulSectorNumber, (WITH_LEADIN | sizeof(ulSectorNumber)));
            TOGGLE_TEST_OUTPUT();                                        // enable measurement of the write time
    #if defined UTFAT_MULTIPLE_BLOCK_WRITE
            if (ucType == DO_WRITE_MULTI_SECTOR) {
                fnPrepareBlockWrite(ucPresentDisk, ucSectorCount, 0);    // multiple sector write but without pre-delete
              //fnPrepareBlockWrite(ucPresentDisk, (ucSectorCount + 1), 0); // multiple sector write but without pre-delete
            }
        #if defined UTFAT_PRE_ERASE
            else if (ucType == DO_WRITE_MULTI_SECTOR_PRE) {              // multiple sector write with pre-delete
                fnPrepareBlockWrite(ucPresentDisk, ucSectorCount, 1);
            }
        #endif
    #endif
          //fnPrepareBlockWrite(ucPresentDisk, 0, 0); // test abort
            if (fnWriteSector(ucPresentDisk, (unsigned char *)ulBuffer, ulSectorNumber) != UTFAT_SUCCESS) { // write the modified sector back
                fnDebugMsg(" Sector write error!\n\r");
            }
            else {
                fnDebugMsg(" - OK\n\r");                                 // sector write successful
            }
            TOGGLE_TEST_OUTPUT();
        }
        break;
#endif
#if defined UTFAT_FORMATTING && defined UTFAT_WRITE                      // {45}
    #if defined UTFAT_FULL_FORMATTING
    case DO_FORMAT_FULL:                                                 // {26}
    case DO_REFORMAT_FULL:
    #endif
    case DO_REFORMAT:
    case DO_FORMAT:
        {
            unsigned char ucFlags = UTFAT_FORMAT_32;                     // default
            fnDebugMsg("Formatting FAT");
    #if defined UTFAT16 || defined UTFAT12
            if ((*ptrInput == '-') && (*(ptrInput + 1) == '1') && ((*(ptrInput + 2) == '2') || (*(ptrInput + 2) == '6'))) {
                if (*(ptrInput + 2) == '2') {
        #if defined UTFAT12
                    fnDebugMsg("12 ");
                    ucFlags = UTFAT_FORMAT_12;
        #endif
                }
        #if defined UTFAT16 
                else {
                    fnDebugMsg("16 ");
                    ucFlags = UTFAT_FORMAT_16;
                }
        #endif
                ptrInput += 4;
            }
            else {
                fnDebugMsg("32 ");
            }
    #endif
            if ((DO_REFORMAT == ucType) || (DO_REFORMAT_FULL == ucType)) {
              //utReFormat(ucPresentDisk, ptrInput, ucFlags);              // {26} utReFormat() no longer used - controlled by utFormat() flags instead
                ucFlags |= UTFAT_REFORMAT;
            }
    #if defined UTFAT_FULL_FORMATTING
            if ((DO_FORMAT_FULL == ucType) || (DO_REFORMAT_FULL == ucType)) {
                ucFlags |= UTFAT_FULL_FORMAT;
            }
    #endif
            if (utFormat(ucPresentDisk, ptrInput, ucFlags) != UTFAT_SUCCESS) {
                fnDebugMsg("not possible\r\n");
                break;
            }
            fnDebugMsg("in progress - please wait...\r\n");
        }
        break;
#endif
#if defined UTFAT_WRITE                                                  // {45}
    case DO_NEW_DIR:
        if (utMakeDirectory(ptrInput, ptr_utDirectory[ucPresentDisk]) != UTFAT_SUCCESS) {
            fnDebugMsg("Make dir failed\r\n");
        }
        break;

    case DO_NEW_FILE:
        {
            UTFILE utFile;                                               // temporary file object for creation
          //utFile.ptr_utDirObject = ptr_utDirectory[ucPresentDisk];
            if (utOpenFile(ptrInput, &utFile, ptr_utDirectory[ucPresentDisk], (UTFAT_OPEN_FOR_WRITE | UTFAT_CREATE | UTFAT_TRUNCATE)) != UTFAT_PATH_IS_FILE) { // {62} open and truncate file or create it if not existing
                fnDebugMsg("Create file failed\r\n");
            }
        }
        break;
    case DO_RENAME:
        {
            UTFILE utFile;
            CHAR *ptrNewName = ptrInput;
            while (*ptrNewName != ' ') {
                if (*ptrNewName++ == 0) {
                    fnDebugMsg("New name missing\r\n");
                    return 0;                                            // {28}
                }
            }
            *ptrNewName++ = 0;                                           // intermediate terminator
          //utFile.ptr_utDirObject = ptr_utDirectory[ucPresentDisk];
            if (utOpenFile(ptrInput, &utFile, ptr_utDirectory[ucPresentDisk], UTFAT_OPEN_FOR_RENAME) < UTFAT_SUCCESS) { // {20}{62}
                fnDebugMsg("Not found\r\n");
            }
            else {
                if (utRenameFile(ptrNewName, &utFile) != UTFAT_SUCCESS) {
                    fnDebugMsg("Rename failed\r\n");
                }
            }
        }
        break;

    #if defined UTFAT_SAFE_DELETE                                        // {60}
    case DO_DELETE_SAFE:
    #endif
    case DO_DELETE:
        {
            int iResult;
    #if defined UTFAT_SAFE_DELETE                                        // {60}
            if (DO_DELETE_SAFE == ucType) {
                iResult = utSafeDeleteFile(ptrInput, ptr_utDirectory[ucPresentDisk]);
            }
            else {
                iResult = utDeleteFile(ptrInput, ptr_utDirectory[ucPresentDisk]);
            }
    #else
            iResult = utDeleteFile(ptrInput, ptr_utDirectory[ucPresentDisk]);
    #endif
            if (iResult == UTFAT_DIR_NOT_EMPTY) {
                fnDebugMsg("Dir NOT empty\r\n");
            }
            else if (iResult != UTFAT_SUCCESS) {
                fnDebugMsg("Delete failed\r\n");
            }
        }
        break;

    #if defined UTFAT_EXPERT_FUNCTIONS
    case DO_WRITE_HIDE:                                                  // {83}
    case DO_WRITE_UNHIDE:
    case DO_SET_PROTECT:
    case DO_REMOVE_PROTECT:
        {
            int iResult;
            UTFILE utFile;                                               // temporary file object
            if ((iResult = utOpenFile(ptrInput, &utFile, ptr_utDirectory[ucPresentDisk], UTFAT_OPEN_FOR_WRITE)) < UTFAT_SUCCESS) { // open an existing file/directory
                fnDebugMsg("File not found\r\n");
            }
            else {
                int iAttributes = 0;
                fnDebugMsg("Attribute ");
                switch (ucType) {
                case DO_WRITE_HIDE:
                    iAttributes = FILE_ATTRIBUTE_SET_HIDE;
                    break;
                case DO_WRITE_UNHIDE:
                    iAttributes = FILE_ATTRIBUTE_REMOVE_HIDE;
                    break;
                case DO_SET_PROTECT:
                    iAttributes = FILE_ATTRIBUTE_SET_WRITE_PROTECT;
                    break;
                case DO_REMOVE_PROTECT:
                    iAttributes = FILE_ATTRIBUTE_REMOVE_WRITE_PROTECT;
                    break;
                }
                if (iResult != UTFAT_PATH_IS_FILE) {                     // if directory
                    iAttributes &= ~(FILE_ATTRIBUTE_SET_WRITE_PROTECT | FILE_ATTRIBUTE_REMOVE_WRITE_PROTECT);
                    iAttributes |= FILE_ATTRIBUTE_OF_DIRECTORY;
                }
                if (utFileAttribute(&utFile, iAttributes) == UTFAT_SUCCESS) {
                    fnDebugMsg("modified\r\n");
                }
                else {
                    fnDebugMsg("could not be changed\r\n");
                }
            }
        }
        break;
    #endif
    case DO_TEST_TRUNCATE:                                               // {59}
        {
            UTFILE utFile;                                               // temporary file object
            unsigned long ulFileLength = fnDecStrHex(ptrInput);          // length to truncate to
            while (*ptrInput++ != ' ') {}                                // move to file name
            if (utOpenFile(ptrInput, &utFile, ptr_utDirectory[ucPresentDisk], UTFAT_OPEN_FOR_WRITE) != UTFAT_PATH_IS_FILE) { // {62} open an existing file
                fnDebugMsg("File not found\r\n");
            }
            else {
                const UTDISK *ptrDiskInfo = fnGetDiskInfo(ucPresentDisk);
                unsigned long ulOriginalFreeClusters = ptrDiskInfo->utFileInfo.ulFreeClusterCount;
                utSeek(&utFile, ulFileLength, UTFAT_SEEK_SET);           // move the file pointer to the end of the file size that is required
                if (utTruncateFile(&utFile) == UTFAT_SUCCESS) {          // truncate to new length
                    fnDebugMsg("File successfully truncated");
                    ulOriginalFreeClusters = (ptrDiskInfo->utFileInfo.ulFreeClusterCount - ulOriginalFreeClusters);
                    if (ulOriginalFreeClusters != 0) {
                        fnDebugDec(ulOriginalFreeClusters, WITH_SPACE);
                        fnDebugMsg(" clusters freed");
                    }
                    fnDebugMsg("\r\n");
                }
                else {
                    fnDebugMsg("Truncation failed\r\n");
                }
            }
        }
        break;
#endif
    case DO_CHANGE_DIR:
        if (utChangeDirectory(ptrInput, ptr_utDirectory[ucPresentDisk]) != UTFAT_SUCCESS) { // change the directory location
            fnDebugMsg("Invalid path\r\n");
        }
        break;
    case DO_INFO:
        {
            const UTDISK *ptrDiskInfo = fnGetDiskInfo(ucPresentDisk);
            fnDebugMsg("\r\nDisk ");
            if ((ptrDiskInfo->usDiskFlags & DISK_NOT_PRESENT) != 0) {    // {24}{49} no card detected
                fnDebugMsg("not detected\r\n");
            }
            else {
                int i = 0;
                if ((ptrDiskInfo->usDiskFlags & DISK_FORMATTED) == 0) {
                    fnDebugMsg("not formatted");
                }
                else {
                    int iLastSpace = 0;
                    CHAR cVolumeLabel[12];
                    do {
                        cVolumeLabel[i] = ptrDiskInfo->cVolumeLabel[i];
                        if (cVolumeLabel[i++] != ' ') {                  // monitor location of final space
                            iLastSpace = i;
                        }
                    } while (i < 11);
                    cVolumeLabel[iLastSpace] = 0;
                    fnDebugMsg(cVolumeLabel);
                }
                fnDebugMsg(" (");
                fnDisplayDiskSize(ptrDiskInfo->ulSD_sectors, ptrDiskInfo->utFAT.usBytesPerSector); // {43}
                fnDebugMsg(")");
                if (ptrDiskInfo->usDiskFlags & HIGH_CAPACITY_SD_CARD) {
                    fnDebugMsg(" SDHC");
                }
#if defined FAT_EMULATION
                if (ptrDiskInfo->usDiskFlags & DISK_FAT_EMULATION) {
                    fnDebugMsg(" EM");                                   // emulated disk
                }
#endif
#if defined UTFAT16 || defined UTFAT12                                   // {42}
                fnDebugMsg(" FAT");
                if (ptrDiskInfo->usDiskFlags & DISK_FORMATTED) {
                    if (ptrDiskInfo->usDiskFlags & (DISK_FORMAT_FAT16 | DISK_FORMAT_FAT12)) {
    #if defined UTFAT16
                        if (ptrDiskInfo->usDiskFlags & DISK_FORMAT_FAT16) {
                            fnDebugMsg("16");
                        }
    #endif
    #if defined UTFAT12
                        if (ptrDiskInfo->usDiskFlags & DISK_FORMAT_FAT12) {
                            fnDebugMsg("12");
                        }
    #endif
                    }
                    else {
                        fnDebugMsg("32");
                    }
                }
#endif
                if (ptrDiskInfo->usDiskFlags & WRITE_PROTECTED_SD_CARD) {
                    fnDebugMsg(" write-protected");
                }
                fnDebugMsg("\r\n");
                if (ptrDiskInfo->usDiskFlags & DISK_FORMATTED) {
                    fnDebugMsg("Bytes per sector:");
                    fnDebugDec(ptrDiskInfo->utFAT.usBytesPerSector, (WITH_SPACE | WITH_CR_LF));
                    fnDebugMsg("Cluster size:");
                    fnDebugDec((ptrDiskInfo->utFAT.ucSectorsPerCluster * ptrDiskInfo->utFAT.usBytesPerSector), (WITH_SPACE | WITH_CR_LF));
                    fnDebugMsg("Directory base:");
                    fnDebugHex(ptrDiskInfo->ulDirectoryBase, (WITH_SPACE | WITH_CR_LF | WITH_LEADIN | sizeof(ptrDiskInfo->ulDirectoryBase)));
                    fnDebugMsg("FAT start:");
                    fnDebugHex(ptrDiskInfo->utFAT.ulFAT_start, (WITH_SPACE | WITH_CR_LF | WITH_LEADIN | sizeof(ptrDiskInfo->utFAT.ulFAT_start)));
                    fnDebugMsg("FAT size:");
                    fnDebugHex(ptrDiskInfo->utFAT.ulFatSize, (WITH_SPACE | WITH_CR_LF | WITH_LEADIN | sizeof(ptrDiskInfo->utFAT.ulFatSize)));
                    fnDebugMsg("Number of FATs:");
                    fnDebugDec(ptrDiskInfo->utFAT.ucNumberOfFATs, (WITH_SPACE | WITH_CR_LF));
                    fnDebugMsg("LBA:");
                    fnDebugHex(ptrDiskInfo->ulLogicalBaseAddress, (WITH_SPACE | WITH_CR_LF | WITH_LEADIN | sizeof(ptrDiskInfo->ulLogicalBaseAddress)));
                    fnDebugMsg("Total clusters:");
                    fnDebugHex(ptrDiskInfo->utFAT.ulClusterCount, (WITH_SPACE | WITH_CR_LF | WITH_LEADIN | sizeof(ptrDiskInfo->utFAT.ulClusterCount)));
                    fnDebugMsg("Info sect:");
                    fnDebugHex(ptrDiskInfo->utFileInfo.ulInfoSector, (WITH_SPACE | WITH_CR_LF | WITH_LEADIN | sizeof(ptrDiskInfo->utFileInfo.ulInfoSector)));
                    fnDebugMsg("Free clusters:");
                    fnDebugHex(ptrDiskInfo->utFileInfo.ulFreeClusterCount, (WITH_SPACE | WITH_CR_LF | WITH_LEADIN | sizeof(ptrDiskInfo->utFileInfo.ulFreeClusterCount)));
                    fnDebugMsg("Next free:");
                    fnDebugHex(ptrDiskInfo->utFileInfo.ulNextFreeCluster, (WITH_SPACE | WITH_CR_LF | WITH_LEADIN | sizeof(ptrDiskInfo->utFileInfo.ulNextFreeCluster)));
                }
    #if defined DISK_SDCARD
                if (ucPresentDisk == DISK_SDCARD) {
                    fnDebugMsg("CSD:");
                    for (i = 0; i < sizeof(ptrDiskInfo->utFileInfo.ucCardSpecificData); i++) {
                        fnDebugHex(ptrDiskInfo->utFileInfo.ucCardSpecificData[i], (WITH_SPACE | WITH_LEADIN | sizeof(ptrDiskInfo->utFileInfo.ucCardSpecificData[0])));
                    }
                    fnDebugMsg("\r\n");
                }
    #endif
            }
        }
        break;
    }
    return 0;
}
#endif

#if defined USE_FTP_CLIENT                                               // {37}

static void fnDoFTP_flow_control(USOCKET uDataSocket, int iForceUpdate)
{
    unsigned short usBufferSpace;
    if ((iFTP_data_state & FTP_DATA_STATE_PAUSE) != 0) {
        usBufferSpace = 0;                                               // signal no space to receive in order to pause reception
        uFtpClientDataSocket = uDataSocket;                              // save the data socket number
    }
    else {
        usBufferSpace = fnWrite(DebugHandle, 0, 0);                      // check the remaining space in the output buffer
        if (usBufferSpace < 128) {                                       // if getting critical set a wakeup when the buffer empties
            uFtpClientDataSocket = uDataSocket;                          // save the data socket number
            fnDriver(DebugHandle, MODIFY_WAKEUP, (MODIFY_TX | OWN_TASK));// wake when the output buffer has adequate space so that we can open the receive window
            iFTP_data_state |= FTP_DATA_STATE_CRITICAL;
        }
    }
    if (iForceUpdate != 0) {
        fnReportTCPWindow(uDataSocket, usBufferSpace);                   // report the new size
    }
    else {
        fnModifyTCPWindow(uDataSocket, usBufferSpace);                   // modify the TCP reception window according to the space left in the output buffer
    }
}

// This is the call-back interface to the FTP client
//
static int fnFTP_client_user_callback_handler(TCP_CLIENT_MESSAGE_BOX *ptrClientMessageBox)
{
    if ((ptrClientMessageBox->iCallbackEvent & FTP_CLIENT_ERROR_FLAG) != 0) { // error event code
        fnDebugMsg("FTP ERROR:[");
        fnDebugHex(ptrClientMessageBox->iCallbackEvent, 2);
        fnDebugMsg("] ");
        fnWrite(DebugHandle, ptrClientMessageBox->ptrData, ptrClientMessageBox->usDataLength); // display exact error sent by FTP server as text
        return 0;
    }

    switch (ptrClientMessageBox->iCallbackEvent) {                       // FTP client events
    case FTP_CLIENT_EVENT_LOGGED_IN:                                     // FTP connection now established
        fnDebugMsg("FTP connection established\r\n");
        iFTP_data_state = FTP_DATA_STATE_CONNECTED;
        break;
    case FTP_CLIENT_EVENT_REQUEST_FTP_USER_NAME:                         // return a pointer to the FTP server user name
        ptrClientMessageBox->ptrData = (unsigned char *)temp_pars->temp_parameters.cFTPUserName; // return user name string (if zero or zero length anonymous login will be attempted)        
        break;
    case FTP_CLIENT_EVENT_REQUEST_FTP_USER_PASSWORD:                     // return a pointer to the FTP server user password
        ptrClientMessageBox->ptrData = (unsigned char *)temp_pars->temp_parameters.cFTPUserPass; // set user password
        break;
    case FTP_CLIENT_USER_NAME_ERROR:
        fnDebugMsg("FTP User failed\r\n");
        break;
    case FTP_CLIENT_USER_PASS_ERROR:
        fnDebugMsg("FTP Pass failed\r\n");
        break;
    case FTP_CLIENT_EVENT_LOGGED_FAILED:                                 // login was rejected - the server will close the connection
        fnDebugMsg("FTP-bad_login\r\n");
        break;
    case FTP_CLIENT_EVENT_CONNECTION_CLOSED:                             // connection closed or aborted
        fnDebugMsg("FTP connection terminated\r\n");
        iFTP_data_state = 0;
        break;

    case FTP_CLIENT_EVENT_ACTIVE_PASSIVE_LIST:                           // the FTP client is asking whether we want to transfer data in active or passive mode
        return ((temp_pars->temp_parameters.usServers[ucPresentNetwork] & PASSIVE_MODE_FTP_CLIENT) == 0); // return the FTP mode setting

    case FTP_CLIENT_EVENT_GET_DATA:
        iFTP_data_state |= FTP_DATA_STATE_GETTING;
    case FTP_CLIENT_EVENT_LISTING_DATA:                                  // receiving listing data
        fnWrite(DebugHandle, ptrClientMessageBox->ptrData, ptrClientMessageBox->usDataLength); // write to UART output queue - it is assumed that there is place for complete content on write
        fnDoFTP_flow_control(ptrClientMessageBox->uDataSocket, 0);
        break;
    case FTP_CLIENT_EVENT_GET_DATA_COMPLETE:
        iFTP_data_state &= ~FTP_DATA_STATE_GETTING;
    case FTP_CLIENT_EVENT_LISTING_DATA_COMPLETE:                         // listing terminated and the data connection was terminated by the FTP server
        fnDebugMsg("\r\nFTP reception complete\r\n");
        break;

    case FTP_CLIENT_EVENT_DATA_CONNECTION_FAILED:                        // data connection failed
        fnDebugMsg("Data connection failed\r\n");
        break;

    case FTP_CLIENT_EVENT_LOCATION_SET:                                  // requested path successfully set
        fnDebugMsg("New path set\r\n");
        break;

    case FTP_CLIENT_EVENT_DIR_CREATED:
        fnDebugMsg("Dir. created\r\n");
        break;

    case FTP_CLIENT_EVENT_DIR_DELETED:
    case FTP_CLIENT_EVENT_FILE_DELETED:
        fnDebugMsg("Delete successful\r\n");
        break;

    case FTP_CLIENT_EVENT_RENAMED:
        fnDebugMsg("Rename successful\r\n");
        break;

#if defined FTP_SIMPLE_DATA_SOCKET || defined FTP_CLIENT_BUFFERED_SOCKET_MODE
    case FTP_CLIENT_EVENT_DATA_CONNECTED:                                // data connection has been established and PUT/APPEND data can now be sent
        fnDebugMsg("Data connected\r\n");
        break;
    case FTP_CLIENT_EVENT_PUT_CAN_START:
    case FTP_CLIENT_EVENT_APPEND_CAN_START:
        iFTP_data_state |= (FTP_DATA_STATE_PUTTING);
        uFtpClientDataSocket = ptrClientMessageBox->uDataSocket;
        fnDebugMsg("Send data now..\r\n");
    #if defined FTP_CLIENT_BUFFERED_SOCKET_MODE
        break;
    case FTP_CLIENT_EVENT_DATA_SENT:                                     // all outstanding data was sent
        break;
    #else
        ucFTP_buffer_content[0] = ucFTP_buffer_content[1] = 0;
        break;
    case FTP_CLIENT_EVENT_DATA_SENT:                                     // last data frame was successfully sent
        if (ucFTP_buffer_content[1] != 0) {                              // if more data waiting
            uMemcpy(FTP_tx[0].ucTCP_Message, FTP_tx[1].ucTCP_Message, ucFTP_buffer_content[1]);
            ucFTP_buffer_content[0] = ucFTP_buffer_content[1];
            ucFTP_buffer_content[1] = 0;
            iFTP_data_state |= FTP_DATA_STATE_SENDING;
            if (fnSendTCP(uFtpClientDataSocket, (unsigned char *)&FTP_tx[0].tTCP_Header, ucFTP_buffer_content[0], TCP_FLAG_PUSH) > 0) { // send the buffer to FTP server (to file)
                return APP_SENT_DATA;
            }
        }
        else {
            ucFTP_buffer_content[0] = 0;                                     // no data waiting
            iFTP_data_state &= ~FTP_DATA_STATE_SENDING;
        }
        break;
    case FTP_CLIENT_EVENT_DATA_LOST:                                     // last data frame was lost - we need to repeat
        if (fnSendTCP(uFtpClientDataSocket, (unsigned char *)&FTP_tx[0].tTCP_Header, ucFTP_buffer_content[0], TCP_FLAG_PUSH) > 0) { // resend the previous buffer to FTP server (to file)
            return APP_SENT_DATA;
        }
        break;
    #endif
    case FTP_CLIENT_EVENT_DATA_DISCONNECTED:
        iFTP_data_state &= ~(FTP_DATA_STATE_CLOSING);
        fnDebugMsg("\r\nData terminated\r\n");
        break;
    case FTP_CLIENT_EVENT_APPEND_DATA_COMPLETE:
    case FTP_CLIENT_EVENT_PUT_DATA_COMPLETE:
        fnDebugMsg("Success\r\n");
        iFTP_data_state &= ~(FTP_DATA_STATE_SENDING | FTP_DATA_STATE_GETTING | FTP_DATA_STATE_PUTTING);
        break;
#endif
       
    default:
        _EXCEPTION("TO DO!!");
        break;
    }

    return 0;
}

static void fnShowFTPconfig(void)
{
    fnDebugMsg("FTP client configuration\r\n");
    fnDebugMsg("========================\r\nFTP server address: ");
    fnDisplayIP(temp_pars->temp_parameters.ucFTP_server_IP);             // server IP address
    fnDebugMsg("\r\nPort number: ");
    fnDebugDec(temp_pars->temp_parameters.usFTPcommandPort, WITH_CR_LF);
    fnDebugMsg("Idle timeout: ");
    fnDebugDec(temp_pars->temp_parameters.ftp_idle_seconds, 0);
    fnDebugMsg("s\r\nConnection type: ");
    if (temp_pars->temp_parameters.usServers[ucPresentNetwork] & PASSIVE_MODE_FTP_CLIENT) {
        fnDebugMsg("PASSIVE mode");
    }
    else {
        fnDebugMsg("ACTIVE mode");
    }
    fnDebugMsg("\r\nUser name: ");
    fnDebugMsg(temp_pars->temp_parameters.cFTPUserName);
    fnDebugMsg("\r\n");
}
#endif

#if defined USE_TELNET_CLIENT
// Application call back to handle TELNET client events
//
static int fnTELNETClientListener(USOCKET Socket, unsigned char ucEvent, unsigned char *ucIp_Data, unsigned short usPortLen)
{
    int iReference;
    for (iReference = 0; iReference < TELNET_CLIENT_COUNT; iReference++) {
        if (_TCP_SOCKET_MASK(telnet_client_details[iReference].Socket) == _TCP_SOCKET_MASK(Socket)) { // match the socket to the interface
            break;
        }
    }
    if (iReference >= TELNET_CLIENT_COUNT) {
        return 0;
    }
    switch (ucEvent) {
    case TCP_EVENT_CONREQ:                                               // session request received on the TCP port
        return APP_REJECT;                                               // reject since we are a clinet only

    case TCP_EVENT_CONNECTED:                                            // TCP connection has been established
        iTELNET_clientActive = (iReference + 1);                         // mark that this Telnet client connection is active
        telnet_client_details[ucTelnetClientInterface].Socket = Socket;
        fnDebugMsg("TCP Connection established\r\n");
        break;

    case TCP_EVENT_ABORT:
        iTELNET_clientActive = 0;
        fnDebugMsg("\r\nTELNET Connection aborted\r\n");
        break;

    case TCP_EVENT_CLOSE:
      //fnDebugMsg("TCP close request received\r\n");
        break;

    case TCP_EVENT_CLOSED:
        iTELNET_clientActive = 0;
        fnDebugMsg("\r\nTELNET Connection closed\r\n");
        break;

  //case TCP_EVENT_ACK:
  //default:
  //    break;

    case TCP_EVENT_DATA:                                                 // application level data received
        fnWrite(DebugHandle, ucIp_Data, usPortLen);                      // send received data to the debug interface
        break;
    }
    return APP_ACCEPT;                                                   // default is to accept a connection
}
#endif

#if defined USE_TELNET_CLIENT
static void fnDisplayTelnetMode(unsigned short usMode, unsigned short usFlags)
{
    if (usFlags & TELNET_CLIENT_MODE_ECHO) {
        fnDebugMsg("Echo ");
        if (usMode & TELNET_CLIENT_MODE_ECHO) {
            fnDebugMsg("On\r\n");
        }
        else {
            fnDebugMsg("Off\r\n");
        }
    }
    if (usFlags & TELNET_CLIENT_NO_NEGOTIATION) {
        fnDebugMsg("Negotiation ");
        if (usMode & TELNET_CLIENT_NO_NEGOTIATION) {
            fnDebugMsg("Disabled\r\n");
        }
        else {
            fnDebugMsg("Enabled\r\n");
        }
    }
}
#endif

#if defined USE_MQTT_CLIENT
static unsigned short fnMQTT_callback(signed char scEvent, unsigned char *ptrData, unsigned long ulLength, unsigned char ucSubscriptionRef)
{
    CHAR *ptrBuf = (CHAR *)ptrData;
    int iAddRef = 0;
    switch (scEvent) {
    case MQTT_CLIENT_IDENTIFIER:
        ptrBuf = uStrcpy(ptrBuf, temp_pars->temp_parameters.cDeviceIDName); // supply a string to be used as MQTT device identifier - this should be unique and normally contain only characters 0..9, a..z, A..Z (normally up to 23 bytes)
        break;
    case MQTT_WILL_TOPIC:                                                // optional connection string fields
    case MQTT_WILL_MESSAGE:
    case MQTT_USER_NAME:
    case MQTT_USER_PASSWORD:
        ptrBuf = uStrcpy(ptrBuf, "string");
        break;
    case MQTT_CONNACK_RECEIVED:
        fnDebugMsg("MQTT connected\r\n");
        break;
    case MQTT_SUBACK_RECEIVED:
        fnDebugMsg("MQTT subscribed");
        iAddRef = 1;
        break;
    case MQTT_UNSUBACK_RECEIVED:
        fnDebugMsg("MQTT subscribed");
        iAddRef = 1;
        break;
    case MQTT_PUBLISH_ACKNOWLEDGED:
        fnDebugMsg("MQTT published - QoS");
        fnDebugDec(ulLength, 0);
        iAddRef = 1;
        break;
    case MQTT_PUBLISH_TOPIC:                                             // add a default publish topic
        ptrBuf = uStrcpy(ptrBuf, "xyz/abc");
        break;
    case MQTT_PUBLISH_DATA:
        {
            static unsigned char ucDataCnt = 0;
            int i = 0;
    #if defined nRF24L01_INTERFACE
            if (0 == ucSubscriptionRef) {
                ptrBuf = (CHAR *)fnSetLast_nRF24201_data(ptrData);       // insert the last received data from nRF24201 receiver
                if (ptrBuf != (CHAR *)ptrData) {                         // if there was data to insert
                    break;                                               // complete
                }
            }
    #endif
            ptrBuf = uStrcpy(ptrBuf, "abcd");                            // add string content
            while (i++ < usPubLength) {                                  // plus some binary content
                *ptrBuf++ = ucDataCnt++;
            }
        }
        break;
    case MQTT_HOST_CLOSED:
    case MQTT_CONNECTION_CLOSED:
        fnDebugMsg("MQTT closed\r\n");
        break;
    case MQTT_TOPIC_MESSAGE:
        fnDebugMsg("Message (");
        fnDebugDec(ulLength, 0);
        fnDebugMsg(")");
        iAddRef = 1;
        break;
    }
    if (iAddRef != 0) {
        fnDebugMsg(" [");
        fnDebugDec(ucSubscriptionRef, 0);
        fnDebugMsg("]\r\n");
    }
    return (unsigned short)((unsigned char *)ptrBuf - ptrData);
}
#endif

#if defined USE_FTP_CLIENT || defined USE_TELNET_CLIENT || defined USE_MQTT_CLIENT // {72}{87}
static void fnDoFTP_TELNET_MQTT(unsigned char ucType, CHAR *ptrInput)
{
    switch (ucType) {
    #if defined USE_FTP_CLIENT
    case DO_SHOW_FTP_CONFIG:                                             // display FTP client settings
        fnShowFTPconfig();
        break;
    case DO_FTP_SET_PORT:                                                // modify the FTP server's command TCP port number
        temp_pars->temp_parameters.usFTPcommandPort = (unsigned short)fnDecStrHex(ptrInput);
        break;
    case DO_FTP_SET_IDLE_TIMEOUT:                                        // modify the FTP client connection timeout (also valid for data connection)
        temp_pars->temp_parameters.ftp_idle_seconds = (unsigned short)fnDecStrHex(ptrInput);
        break;
    case DO_FTP_SERVER_IP:                                               // modify the IP address of the FTP server
        fnStrIP(ptrInput, temp_pars->temp_parameters.ucFTP_server_IP);
        break;
    case DO_FTP_USER_NAME:                                               // modify the user account name to be used with the FTP server
        {
            int iStringLength = uStrlen(ptrInput);
            if (iStringLength > (sizeof(temp_pars->temp_parameters.cFTPUserName) - 1)) {
              iStringLength = (sizeof(temp_pars->temp_parameters.cFTPUserName) - 1);
            }
            uMemcpy(temp_pars->temp_parameters.cFTPUserName, ptrInput, (iStringLength + 1));
        }
        break;
    case DO_FTP_USER_PASS:                                             // modify the user account password to be used with the FTP server
      {
          int iStringLength = uStrlen(ptrInput);
          if (iStringLength > (sizeof(temp_pars->temp_parameters.cFTPUserPass) - 1)) {
              iStringLength = (sizeof(temp_pars->temp_parameters.cFTPUserPass) - 1);
          }
          uMemcpy(temp_pars->temp_parameters.cFTPUserPass, ptrInput, (iStringLength + 1)); 
      }
        break;
    case DO_FTP_PASSIVE:                                                 // set active or passive FTP mode
        fnDoServer(ucType, ptrInput);
        break;
    #if defined USE_IPV6                                                 // {48}
    case DO_FTP_CONNECT_IPV6:
        fnFTP_client_connect(temp_pars->temp_parameters.ucFTP_server_IPv6, temp_pars->temp_parameters.usFTPcommandPort, temp_pars->temp_parameters.ftp_idle_seconds, fnFTP_client_user_callback_handler, FTP_CLIENT_IPv6);
        break;
    #endif
    case DO_FTP_CONNECT:                                                 // start connection process at a remote FTP server
        fnFTP_client_connect(temp_pars->temp_parameters.ucFTP_server_IP, temp_pars->temp_parameters.usFTPcommandPort, temp_pars->temp_parameters.ftp_idle_seconds, fnFTP_client_user_callback_handler, FTP_CLIENT_IPv4);
        break;
    case DO_FTP_DISCONNECT:
        fnFTP_client_disconnect();                                       // disconnect from FTP server
        break;

    case DO_FTP_DIR:                                                     // request a directory listing
        if (*ptrInput == 0) {
            ptrInput = 0;                                                // command a directory listing at the present location
        }
        if (fnFTP_client_dir(ptrInput, FTP_DIR_LIST) < 0) {
            fnDebugMsg("No FTP connection!\r\n");
            break;
        }
        break;

    case DO_FTP_PATH:                                                    // set a location at the FTP server
        if (*ptrInput == 0) {
            fnDebugMsg("Path missing!\r\n");
            return;
        }
        fnFTP_client_dir(ptrInput, FTP_DIR_SET_PATH);
        break;

    case DO_FTP_MKDIR:                                                   // make a directory at the FTP server
        fnFTP_client_dir(ptrInput, (FTP_DIR_MAKE_DIR));
        break;
    case DO_FTP_GETA:                                                    // start get of ascii file from FTP server
        fnFTP_client_transfer(ptrInput, (FTP_DO_GET | FTP_TRANSFER_ASCII));
        break;
    case DO_FTP_PUTA:                                                    // start put of ascii file to FTP server
       fnFTP_client_transfer(ptrInput, (FTP_DO_PUT | FTP_TRANSFER_ASCII));
       break;
    case DO_FTP_APPA:                                                    // start append to ascii file at FTP server
        fnFTP_client_transfer(ptrInput, (FTP_DO_APPEND | FTP_TRANSFER_ASCII));
        break;
    case DO_FTP_GET:                                                     // start get of binary file from FTP server
        fnFTP_client_transfer(ptrInput, (FTP_DO_GET | FTP_TRANSFER_BINARY));
        break;
    case DO_FTP_PUT:                                                     // start put of binary file to FTP server
        fnFTP_client_transfer(ptrInput, (FTP_DO_PUT | FTP_TRANSFER_BINARY));
        break;
    case DO_FTP_APP:                                                     // start append to binary file at FTP server
        fnFTP_client_transfer(ptrInput, (FTP_DO_APPEND | FTP_TRANSFER_BINARY));
        break;
    case DO_FTP_RENAME:                                                  // rename file or directory at the FTP server
        fnFTP_client_dir(ptrInput, (FTP_DIR_RENAME));
        break;
    case DO_FTP_DEL:
        fnFTP_client_dir(ptrInput, (FTP_DIR_DELETE));
        break;
    case DO_FTP_REMOVE_DIR:
        fnFTP_client_dir(ptrInput, (FTP_DIR_REMOVE_DIR));
        break;
    #endif
    #if defined USE_TELNET_CLIENT                                        // {72}
        #if defined TELNET_CLIENT_COUNT && (TELNET_CLIENT_COUNT > 1)
    case DO_TELNET_SET_INTERFACE:                                        // select the interface to communicate on
        ucTelnetClientInterface = (unsigned char)fnDecStrHex(ptrInput);
        if (ucTelnetClientInterface >= TELNET_CLIENT_COUNT) {
            ucTelnetClientInterface = 0;
            fnDebugMsg("Invalid interface - 0 set");
        }
        else {
            fnDebugMsg("Interface set");
        }
        break;
        #endif
    case DO_TELNET_SHOW:                                                 // show the TELNET client's present configuration
        fnDebugMsg("Telnet client ");
        fnDebugDec(ucTelnetClientInterface, (WITH_CR_LF));
        fnDebugMsg("Port number: ");
        fnDebugDec(telnet_client_details[ucTelnetClientInterface].usPortNumber, (WITH_CR_LF));
        fnDisplayTelnetMode(telnet_client_details[ucTelnetClientInterface].usTelnetMode, (TELNET_CLIENT_MODE_ECHO | TELNET_CLIENT_NO_NEGOTIATION));
        break;
    case DO_TELNET_SET_ECHO:
        if (*ptrInput == '1') {
            telnet_client_details[ucTelnetClientInterface].usTelnetMode |= TELNET_CLIENT_MODE_ECHO;
        }
        else {
            telnet_client_details[ucTelnetClientInterface].usTelnetMode &= ~TELNET_CLIENT_MODE_ECHO;
        }
        fnDisplayTelnetMode(telnet_client_details[ucTelnetClientInterface].usTelnetMode, TELNET_CLIENT_MODE_ECHO);
        break;
    case DO_TELNET_SET_NEGOTIATION:
        if (*ptrInput == '1') {
            telnet_client_details[ucTelnetClientInterface].usTelnetMode &= ~TELNET_CLIENT_NO_NEGOTIATION;
        }
        else {
            telnet_client_details[ucTelnetClientInterface].usTelnetMode |= TELNET_CLIENT_NO_NEGOTIATION;
        }
        fnDisplayTelnetMode(telnet_client_details[ucTelnetClientInterface].usTelnetMode, TELNET_CLIENT_NO_NEGOTIATION);
        break;
        
    case DO_TELNET_SET_PORT:                                             // set the TELNET client's port number
        telnet_client_details[ucTelnetClientInterface].usPortNumber = (unsigned short)fnDecStrHex(ptrInput);
        break;
    case DO_TELNET_CONNECT:                                              // connect to a specified IP address
        {
            unsigned char ucDestinationIP[IPV4_LENGTH] = {0};
            if (fnStrIP(ptrInput, ucDestinationIP) != 0) {
                fnDebugMsg("TELNET client ");
                uMemcpy(telnet_client_details[ucTelnetClientInterface].ucDestinationIPv4, ucDestinationIP, sizeof(telnet_client_details[ucTelnetClientInterface].ucDestinationIPv4));
                telnet_client_details[ucTelnetClientInterface].ucInterfaceReference = ucTelnetClientInterface;
                telnet_client_details[ucTelnetClientInterface].Socket = fnConnectTELNET(&telnet_client_details[ucTelnetClientInterface]); // establish a connection
                if (telnet_client_details[ucTelnetClientInterface].Socket > 0) {
                    fnDebugMsg("connecting...");
                }
                else {
                    fnDebugMsg("can't connect");
                }
            }
            else {
                fnDebugMsg("Invalid IP");
            }
        }
        break;
    #endif
    #if defined USE_MQTT_CLIENT
        #if defined SECURE_MQTT
    case DO_MQTTS_CONNECT:
        #endif
    case DO_MQTT_CONNECT:
        {
            unsigned char ucDestinationIP[IPV4_LENGTH] = {0};
            if (fnStrIP(ptrInput, ucDestinationIP) != 0) {
                unsigned long ulModeFlags = (UNSECURE_MQTT_CONNECTION | MQTT_CONNECT_FLAG_CLEAN_SESSION);
        #if defined SECURE_MQTT
                if (DO_MQTTS_CONNECT == ucType) {
                    ulModeFlags = (SECURE_MQTT_CONNECTION | MQTT_CONNECT_FLAG_CLEAN_SESSION);
                }
        #endif
                fnDebugMsg("MQTT client ");
                if (fnConnectMQTT(ucDestinationIP, fnMQTT_callback, ulModeFlags) == 0) {
                    fnDebugMsg("Connecting...");
                }
                else {
                    fnDebugMsg("Can't connect!");
                }
            }
            else {
                fnDebugMsg("Invalid IP");
            }
        }
        break;
    case DO_MQTT_SUBSCRIBE:
        {
            CHAR *ptrQoS = ptrInput;
            int iSubscriptionReference;
            unsigned char ucQoS;
            if (*ptrInput == 0) {
                fnDebugMsg("Missing topic!!");
                break;
            }
            fnJumpWhiteSpace(&ptrQoS);
            if (*ptrQoS == 0) {                                          // if no QoS specified
                ucQoS = 2;                                               // default QoS if not supplied
            }
            else {
                *(ptrQoS - 1) = 0;                                       // terminate the topic string
                ucQoS = (unsigned char)fnDecStrHex(ptrQoS);              // QoS to use
            }
            iSubscriptionReference = fnSubscribeMQTT(ptrInput, ucQoS);
            if (iSubscriptionReference >= 0) {
                fnDebugMsg("Subscribing (ref=");
                fnDebugDec(iSubscriptionReference, 0);
                fnDebugMsg(")...");
            }
            else {
                fnDebugMsg("MQTT error!");
            }
        }
        break;
    case DO_MQTT_TOPICS:                                                 // list the topics that we are presently subscribed to
        {
           int iRef = 1;
           int iResult;
           int iNotEmpty = 0;
           while ((iResult = fnShowMQTT_subscription(iRef++)) != ERROR_MQTT_INVALID_SUBSCRIPTION) { // print out details of each mqtt subscription
               if (MQTT_RESULT_OK == iResult) {
                   iNotEmpty = 1;
               }
           }
           if (iNotEmpty == 0) {
               fnDebugMsg("No MQTT substriptions");
           }
        }
        break;
    case DO_MQTT_UNSUBSCRIBE:
        {
            unsigned char ucSubscriptionRef = (unsigned char)fnDecStrHex(ptrInput); // the subscription reference to be unsubscribed
            if (fnUnsubscribeMQTT(ucSubscriptionRef) >= 0) {
                fnDebugMsg("Unsubscribing...");
            }
            else {
                fnDebugMsg("MQTT error!");
            }
        }
        break;
    case DO_MQTT_PUB_LONG:
    case DO_MQTT_PUB:
        {
            CHAR *ptrTopic = 0;
            unsigned char ucSubscriptionRef = 0;                         // default will call-back to insert the publish topic
            signed char cQoS;
            if (*ptrInput == '"') {                                      // we want to insert a one-off publish topic instead of using a subscribed reference
                ptrTopic = (ptrInput + 1);
                fnJumpWhiteSpace(&ptrInput);
                if (*ptrInput == 0) {
                    *(ptrInput - 1) = 0;                                 // terminate the topic string in the input buffer
                }
                else {
                    *(ptrInput - 2) = 0;                                 // terminate the topic string in the input buffer
                }
            }
            else {
                if (*ptrInput != 0) {
                    ucSubscriptionRef = (unsigned char)fnDecStrHex(ptrInput); // use the publish topic defined
                    fnJumpWhiteSpace(&ptrInput);
                }
            }
            if (*ptrInput == 0) {                                        // if no QoS is specified
                cQoS = -1;                                               // a QoS value fo -1 causes the subscription to be used or else 2 for one-off-topics
            }
            else {
                cQoS = (signed char)fnDecStrHex(ptrInput);               // QoS to use
            }
            if (ucType == DO_MQTT_PUB_LONG) {
                usPubLength = 1024;
            }
            else {
                usPubLength = 10;
            }
            if (fnPublishMQTT(ucSubscriptionRef, ptrTopic, cQoS) == 0) {
                fnDebugMsg("Publishing...");
            }
            else {
                fnDebugMsg("MQTT error!");
            }
        }
        break;
    case DO_MQTT_DISCONNECT:
        if (fnDisconnectMQTT() == 0) {
            fnDebugMsg("Disconnecting...");
        }
        else {
            fnDebugMsg("Not connected!");
        }
        break;
    #endif
    }
}
#endif


#if defined CAN_INTERFACE                                                // {38}

#define EXTRACT_CAN_ID          0x01
#define EXTRACT_CAN_EXTENDED_ID 0x02
#define EXTRACT_CAN_DATA        0x04

static unsigned char fnGetID_Data(int iActions, CHAR *ptrInput, unsigned char *ucMessage)
{
    unsigned long ulID = 0;
    unsigned char ucLength = 0;
    unsigned char ucBlockLength = 0;
    unsigned char ucTemp;
    FOREVER_LOOP() {                                                     // extract first an optional ID from 0 -- 7ff or extended from 0 -- 1fffffff, followed by data
        while ((*ptrInput != ' ') && (*ptrInput != 0)) {
            ulID <<= 4;
            ucTemp = (*ptrInput - '0');
            if (ucTemp > 9) {
                ucTemp -= ('A' - '9' - 1);
                if (ucTemp > 0x0f) {
                    ucTemp -= ('a' - 'A');
                }
            }
            ulID |= ucTemp;
            ptrInput++;
            if (++ucBlockLength >= 8) {                                  // 4 bytes of data collected
                break;
            }
        }
        if (ucBlockLength == 0) {
            break;
        }
        if (iActions & EXTRACT_CAN_EXTENDED_ID) {                       // extract first an ID 
            ulID |= CAN_EXTENDED_ID;                                    // mark that the id is an extended id rather than a standard id
        }
        if ((iActions & (EXTRACT_CAN_EXTENDED_ID | EXTRACT_CAN_ID)) || (ucBlockLength > 6)) {
            *ucMessage++ = (unsigned char)(ulID >> 24);
            ucLength++;
        }
        if ((iActions & (EXTRACT_CAN_EXTENDED_ID | EXTRACT_CAN_ID)) || (ucBlockLength > 4)) {
            *ucMessage++ = (unsigned char)(ulID >> 16);
            ucLength++;
        }
        if ((iActions & (EXTRACT_CAN_EXTENDED_ID | EXTRACT_CAN_ID)) || (ucBlockLength > 2)) {
            *ucMessage++ = (unsigned char)(ulID >> 8);
            ucLength++;
        }
        if ((iActions & (EXTRACT_CAN_EXTENDED_ID | EXTRACT_CAN_ID)) || (ucBlockLength > 0)) {
            *ucMessage++ = (unsigned char)(ulID);
            ucLength++;
        }
        if (iActions & (EXTRACT_CAN_EXTENDED_ID | EXTRACT_CAN_ID)) {
            ptrInput = fnSkipWhiteSpace(ptrInput);
            iActions &= ~(EXTRACT_CAN_EXTENDED_ID | EXTRACT_CAN_ID);     // id flags only valid as first parameter
        }
        if (ucLength >= 12) {                                            // protect buffer
            break;
        }
        ucBlockLength = 0;
    }
    return ucLength;
}

// The input has the general format: [optional CAN port number] <standard or extended address when the command expects it - in hex> <data in ASCII HEX 0011223344556677>
//
static void fnDoCAN(unsigned char ucType, CHAR *ptrInput)
{
    int iChannel = 0;                                                    // default CAN controller channel 0
    unsigned char ucMessage[12];                                         // CAN message content (4 bytes for ID plus 8 for maximum data)
    unsigned char ucMessageLength;
    if ((*ptrInput >= '0') && (*ptrInput < ('0' + NUMBER_OF_CAN_INTERFACES))) { // possible port number
        if ((*(ptrInput + 1) == ' ') || (*(ptrInput + 1) == 0)) {
            iChannel = (*ptrInput - '0');                                // CAN channel
            ptrInput += 2;
        }
    }
    switch (ucType) {
    case DO_SEND_CAN_DEFAULT:                                            // send a message to the default destination
        ucMessageLength = fnGetID_Data(EXTRACT_CAN_DATA, ptrInput, ucMessage);
        fnSendCAN_message(iChannel, (CAN_TX_ACK_ON), ucMessage, ucMessageLength);
        break;
    case DO_SEND_CAN_STANDARD:                                           // send a message to a standard (ID) destination
        ucMessageLength = fnGetID_Data((EXTRACT_CAN_ID | EXTRACT_CAN_DATA), ptrInput, ucMessage);
        fnSendCAN_message(iChannel, (CAN_TX_ACK_ON | SPECIFIED_ID), ucMessage, ucMessageLength);
        break;
    case DO_SEND_CAN_EXTENDED:                                           // send a message to an extended (ID) destination
        ucMessageLength = fnGetID_Data((EXTRACT_CAN_EXTENDED_ID | EXTRACT_CAN_DATA), ptrInput, ucMessage);
        fnSendCAN_message(iChannel, (CAN_TX_ACK_ON | SPECIFIED_ID), ucMessage, ucMessageLength);
        break;
    case DO_REQUEST_CAN_DEFAULT:                                         // collect a remote message from the default destination
        fnSendCAN_message(iChannel, 0, 0, 0);
        break;
    case DO_REQUEST_CAN_STANDARD:                                        // collect a remote message from a standard (ID) destination
        ucMessageLength = fnGetID_Data(EXTRACT_CAN_ID, ptrInput, ucMessage);
        fnSendCAN_message(iChannel, SPECIFIED_ID, ucMessage, 4);
        break;
    case DO_REQUEST_CAN_EXTENDED:                                        // collect a remote message from an extended (ID) destination
        ucMessageLength = fnGetID_Data(EXTRACT_CAN_EXTENDED_ID, ptrInput, ucMessage);
        fnSendCAN_message(iChannel, SPECIFIED_ID, ucMessage, 4);
        break;
    case DO_SET_CAN_REMOTE:                                              // deposit a remote message to be collected by other nodes (or overwrite an existing one)
        ucMessageLength = fnGetID_Data(EXTRACT_CAN_DATA, ptrInput, ucMessage);
        fnSendCAN_message(iChannel, (TX_REMOTE_FRAME | CAN_TX_ACK_ON), ucMessage, ucMessageLength);
        break;
    case DO_CLEAR_CAN_REMOTE:                                            // clear (single) deposited remote message
        fnSendCAN_message(iChannel, (TX_REMOTE_STOP), 0, 0);        
        break;
    case DO_CLEAR_REMOTE_BUF:                                            // free a buffer expecting a remote response that doesn't arrive
        fnSendCAN_message(iChannel, (FREE_CAN_RX_REMOTE), 0, 0);
        break;
    #if defined _DEBUG_CAN
    case DO_DEBUG_CAN:
        {
            KINETIS_CAN_BUF *ptrMessageBuffer;
            int i = 0;
            if (iChannel != 0) {
                ptrMessageBuffer = MBUFF0_ADD_1;
                fnDebugMsg("[1]");
            }
            else {
                ptrMessageBuffer = MBUFF0_ADD_0;
                fnDebugMsg("[0]");
            }
            fnDebugMsg(" FlexCAN message buffers:\r\n");
            while (i < NUMBER_CAN_MESSAGE_BUFFERS) {
                fnDebugDec(i, 0);
                fnDebugMsg(" ID = ");
                fnDebugHex(ptrMessageBuffer->ulID, (sizeof(ptrMessageBuffer->ulID) | WITH_LEADIN));
                fnDebugMsg(" Code = ");
                fnDebugHex(ptrMessageBuffer->ulCode_Len_TimeStamp, (sizeof(ptrMessageBuffer->ulCode_Len_TimeStamp) | WITH_LEADIN));
                fnDebugMsg(" Data = ");
                fnDebugHex(ptrMessageBuffer->ulData[0], (sizeof(ptrMessageBuffer->ulData[0]) | WITH_LEADIN)); 
                fnDebugHex(ptrMessageBuffer->ulData[1], (sizeof(ptrMessageBuffer->ulData[1]) | WITH_LEADIN | WITH_SPACE | WITH_CR_LF));
                i++;
                ptrMessageBuffer++;
            }
        }
        break;
    #endif
    }
}
#endif

static int fnDoCommand(unsigned char ucFunction, unsigned char ucType, CHAR *ptrInput)
{
    switch (ucFunction) {                                                // decide which function type to call
    case DO_HELP:                                                        // help group
        if (DO_MAIN_HELP == ucType) {
            return (fnDisplayHelp(0));                                   // show present menu
        }
        else if (DO_MENU_HELP_LAN == ucType) {
            ucMenu = MENU_HELP_LAN;                                      // set LAN menu
            return (fnDisplayHelp(0));                                   // large menu may require special handling
        }
        else if (DO_MENU_HELP_SERIAL == ucType) {
            ucMenu = MENU_HELP_SERIAL;                                   // set SERIAL menu
            return (fnDisplayHelp(0));                                   // large menu may require special handling
        }
        else if (DO_MENU_HELP_IO == ucType) {
            ucMenu = MENU_HELP_IO;                                       // set I/O menu
            return (fnDisplayHelp(0));                                   // large menu may require special handling
        }
        else if (DO_MENU_HELP_ADMIN == ucType) {
            ucMenu = MENU_HELP_ADMIN;                                    // set ADMIN menu
            return (fnDisplayHelp(0));                                   // large menu may require special handling
        }
        else if (DO_MENU_HELP_STATS == ucType) {
            ucMenu = MENU_HELP_STATS;                                    // set statistics menu
            return (fnDisplayHelp(0));                                   // large menu may require special handling
        }
        else if (DO_MENU_HELP_USB == ucType) {
            ucMenu = MENU_HELP_USB;                                      // set USB menu
            return (fnDisplayHelp(0));                                   // large menu may require special handling
        }
        else if (DO_MENU_HELP_I2C == ucType) {
            ucMenu = MENU_HELP_I2C;                                      // set I2C menu
            return (fnDisplayHelp(0));                                   // large menu may require special handling
        }
        else if (DO_MENU_HELP_DISK == ucType) {                          // {17}
            ucMenu = MENU_HELP_DISK;                                     // set SD-card disk menu
            return (fnDisplayHelp(0));                                   // large menu may require special handling
        }
        else if (DO_MENU_HELP_FTP_TELNET_MQTT == ucType) {               // {37}
            ucMenu = MENU_HELP_FTP;                                      // set FTP/TELNET client menu
            return (fnDisplayHelp(0));                                   // large menu may require special handling
        }
        else if (DO_MENU_HELP_CAN == ucType) {                           // {38}
            ucMenu = MENU_HELP_CAN;                                      // set CAN menu
            return (fnDisplayHelp(0));                                   // large menu may require special handling
        }
    #if defined TEST_FLEXIO                                              // {91}
        else if (DO_MENU_HELP_FLEXIO == ucType) {
            ucMenu = MENU_HELP_FLEXIO;                                   // set flexio menu
            return (fnDisplayHelp(0));
        }
    #elif defined CMSIS_DSP_CFFT || defined CRYPTOGRAPHY || defined MMDVSQ_AVAILABLE // {84}
        else if (DO_MENU_HELP_ADVANCED == ucType) {
            ucMenu = MENU_HELP_ADVANCED;                                 // set advanced menu
            return (fnDisplayHelp(0));
        }
    #endif
        else if (DO_HELP_UP == ucType) {
            ucMenu = 0;                                                  // set top menu level
            return (fnDisplayHelp(0));                                   // large menu may require special handling
        }
        break;
    case DO_HARDWARE:                                                    // hardware group
        fnDoHardware(ucType, ptrInput);
        break;
#if defined USB_INTERFACE
    case DO_USB:
        fnDoUSB(ucType, ptrInput);                                       // USB group
        break;
#endif

#if defined I2C_INTERFACE
    case DO_I2C:
        fnDoI2C(ucType, ptrInput);                                       // I2C group
        break;
#endif

#if defined SDCARD_SUPPORT || defined SPI_FLASH_FAT || defined FLASH_FAT || defined USB_MSD_HOST // {17}{81}
    case DO_DISK:
        return (fnDoDisk(ucType, ptrInput));                             // SD-card disk group
#endif

    case DO_TELNET:
        fnDoTelnet(ucType, ptrInput);
        break;
#if defined USE_FTP_CLIENT || defined USE_TELNET_CLIENT || defined USE_MQTT_CLIENT // {37}{72}
    case DO_FTP_TELNET_MQTT:                                             // FTP/TELNET/MQTT client group
        fnDoFTP_TELNET_MQTT(ucType, ptrInput);
        break;
#endif
#if defined CAN_INTERFACE                                                // {38}
    case DO_CAN:                                                         // CAN group
        fnDoCAN(ucType, ptrInput);
        break;
#endif

#if defined USE_PARAMETER_BLOCK                                          // {5}
    case DO_FLASH:
        fnDoFlash(ucType, ptrInput);
        break;
#endif

    case DO_IP:
        fnDoIP(ucType, ptrInput);
        break;

#if (defined SERIAL_INTERFACE && defined DEMO_UART) || defined USB_INTERFACE // {10}
    case DO_SERIAL:
        fnDoSerial(ucType, ptrInput);
        break;
#endif

    case DO_ADMIN:
        fnDoAdmin(ucType, ptrInput);
        break;

    case DO_SERVER:
        fnDoServer(ucType, ptrInput);
        break;
    }
    return 0;
}

#if defined SDCARD_SUPPORT || defined SPI_FLASH_FAT || defined FLASH_FAT || defined USB_MSD_HOST // {17}{81}
static void fnSendPrompt(void)
{
    if (ucMenu == MENU_HELP_DISK) {
        if ((ptr_utDirectory[ucPresentDisk] != 0) && (ptr_utDirectory[ucPresentDisk]->ptrDirectoryPath != 0)) {
            fnDebugMsg(ptr_utDirectory[ucPresentDisk]->ptrDirectoryPath);
        }
        fnDebugMsg(">");
    }
    else {
        fnDebugMsg("\r\n#");                                             // send prompt in command mode
    }
}
#endif


static int fnDoDebug(CHAR *ptr_input, unsigned short usInputLen, int iSource)
{
    const DEBUG_COMMAND *ptrCom = ucMenus[ucMenu].pMenu;
    int iEntries = ucMenus[ucMenu].ucMenuLength;
    CHAR *ptrNext;

    ptr_input[usInputLen - 1] = 0;                                       // terminate input string

    if (iSource == SOURCE_SERIAL) {
        if ((usData_state == ES_NO_CONNECTION) || (usData_state & ES_SERIAL_SUSPENDED)) {
            return 0;                                                    // we cannot treat commands
        }
    }
#if defined USE_USB_CDC                                                  // {8}
    else if (iSource == SOURCE_USB) {
        if (usUSB_state == ES_NO_CONNECTION) {
            return 0;                                                    // we cannot treat commands
        }
    }
#endif

    if (usInputLen != 0) {
        while (iEntries-- != 0) {
            if ((ptrNext = fnCheckInput(ptr_input, ptrCom->strCommand)) != 0) {
                fnDebugMsg("\r\n");
                if (fnDoCommand(ptrCom->ucDoFunction, ptrCom->ucType, ptrNext) == 0) {
#if defined SDCARD_SUPPORT || defined SPI_FLASH_FAT || defined FLASH_FAT || defined USB_MSD_HOST // {17}{81}
                    fnSendPrompt();
#else
                    fnDebugMsg("\r\n#");                                 // send prompt in command mode
#endif

                }
                if (iSource == SOURCE_SERIAL) {
                    fnGotoNextState(usData_state);                       // reinforce present state (to handle connection timers)
                }
                return 1;
            }
            ptrCom++;
        }
        iEntries = (sizeof(tSecretCommands) / sizeof(DEBUG_COMMAND));    // check whether we are to perform a secret command
        ptrCom = tSecretCommands;
        while (iEntries--) {
            if ((ptrNext = fnCheckInput(ptr_input, ptrCom->strCommand)) != 0) {
                if (fnDoCommand(ptrCom->ucDoFunction, ptrCom->ucType, ptrNext) == 0) {
                    fnDebugMsg("\r\n#");                                 // send prompt in command mode
                }
                if (iSource == SOURCE_SERIAL) {
                    fnGotoNextState(usData_state);                       // reinforce present state (to handle connection timers)
                }
                return 1;
            }
            ptrCom++;
        }
    }

    fnDebugMsg("\r\n??\r\n#");                                           // signal input non recognised and send prompt in command mode
    return 0;
}

static void fnLoginSuccess(void)
{
    if (ES_SERIAL_LOGIN == usData_state) {
        fnGotoNextState(ES_SERIAL_COMMAND_MODE);
    }
#if defined USE_USB_CDC                                                  // {8}
    else if (ES_USB_LOGIN == usUSB_state) {
        usUSB_state = ES_USB_COMMAND_MODE;
        fnSetUSB_debug();                                                // enter USB channel as debug interface
    }
#endif
#if defined USE_TELNET
    else {
        usTelnet_state = ES_NETWORK_COMMAND_MODE;
        fnTelnet(Telnet_socket, CLEAR_TEXT_ENTRY);                       // make sure telnet echos in clear text mode again
    }
#endif
    fnDoAdmin(DO_SHOW_CONFIG, 0);
    ucMenu = 0;                                                          // set main menu
    fnDisplayHelp(0);                                                    // display main menu
}


#if defined USE_TELNET
// Telnet server
//
static int fnTELNETListener(USOCKET Socket, unsigned char ucEvent, unsigned char *ucIp_Data, unsigned short usPortLen)
{
    static QUEUE_HANDLE  BackupDebugPort;

    switch (ucEvent) {
    case TCP_EVENT_CONREQ:                                               // session request received on the TCP port
        if ((uMemcmp(temp_pars->temp_parameters.ucTrustedIP, cucNullMACIP, IPV4_LENGTH)) && (uMemcmp(temp_pars->temp_parameters.ucTrustedIP, ucIp_Data, IPV4_LENGTH))) {
            return APP_REJECT;                                           // we refuse the connection since we don't know the source
        }
        if (usData_state & (ES_CONNECTING_CLIENT | ES_SERIAL_COMMAND_MODE | ES_SERIAL_LOGIN)) { // if we have a serial command connection in progress, block a telnet connection
            return APP_REJECT;
        }
    #if defined USE_USB_CDC                                              // {8}
        if (usUSB_state & (ES_USB_COMMAND_MODE | ES_USB_LOGIN)) {        // if we have a USB command connection in progress, block a telnet connection
            return APP_REJECT;
        }
    #endif
        usTelnet_state = ES_CONNECTING_SERVER;
        Telnet_socket = Socket;                                          // {70}
        break;

    case TCP_EVENT_CONNECTED:                                            // TCP connection has been established
                                                                         // we start negotiation of the telnet session and say that we will perform echo (only when not a binary TCP connection)
        usTelnet_state = ES_STARTING_COMMAND_MODE;                       // connection starts in command mode
        return fnTelnet(Telnet_socket, GOTO_ECHO_MODE);                  // try to set echo mode

    case TCP_EVENT_ABORT:
        break;

    case TCP_EVENT_CLOSE:
        usTelnet_state = ES_NO_CONNECTION;
    #if defined SERIAL_INTERFACE
        ucPasswordState = PASSWORD_IDLE;
    #endif
        ucDebugCnt = 0;                                                  // invalidate any input in the debug input buffer
        break;

    case TCP_EVENT_CLOSED:
        if (DebugHandle == NETWORK_HANDLE) {
            DebugHandle = BackupDebugPort;                               // return original debug interface
        }
        usTelnet_state = ES_NO_CONNECTION;
    #if defined SERIAL_INTERFACE
        ucPasswordState = PASSWORD_IDLE;
    #endif
        ucDebugCnt = 0;                                                  // invalidate any input in the debug input buffer
        break;

    case TCP_EVENT_ACK:
        if (usTelnet_state == ES_STARTING_COMMAND_MODE) {                // we have received an ack from our negotiation transmission(s) so we can welcome the user
            BackupDebugPort = DebugHandle;                               // save debug handle before telnet connection
            DebugHandle = NETWORK_HANDLE;                                // set the debug port to the network - we will send it over Telnet
            if (DO_PASSWORD_ENTRY == fnInitiateLogin(ES_NETWORK_LOGIN)) {// request password details before continuing
                fnTelnet(Telnet_socket, PASSWORD_ENTRY);                 // make sure telnet doesn't echo in clear text
            }
            return APP_SENT_DATA;                                        // {29} default menu was sent
        }

        if (ES_TERMINATE_CONNECTION == usTelnet_state) {
            return APP_REQUEST_CLOSE;
        }
        break;

    case TCP_EVENT_DATA:
        if (fnCommandInput(ucIp_Data, usPortLen, SOURCE_NETWORK)) {      // collect data input and try to interpret if a LF is found
            return APP_SENT_DATA;                                        // we will always have sent an answer
        }
        break;
    }
    return APP_ACCEPT;                                                   // default is to accept a connection
}
#endif


#if defined TEST_TCP_SERVER                                              // {30}
static int fnTestTCP(USOCKET Socket, unsigned char ucTestCase)
{
    int iSent = 0;
    switch (ucTestCase) {
    case 0x01:                                                           // we should send data blocks as fast as possible
        {
            int i;
            unsigned char test_buffer[26];
            for (i = 0; i < 26; i++) {
                test_buffer[i] = 'a' + i;
            }
            while (fnSendBufTCP(Socket, 0, 26, TCP_BUF_CHECK) != 0) {    // while space in output buffer              
                iSent += fnSendBufTCP(Socket, test_buffer, 26, (TCP_BUF_SEND | TCP_BUF_SEND_REPORT_COPY));
            }
        }
        break;
    }
    return iSent;
}

//#define FAST_PEER_DROPED_FRAME_TEST                                    // temp test
#if defined FAST_PEER_DROPED_FRAME_TEST
static unsigned char test_buffer[1500] = {0};

static int fnFastRxTest(USOCKET Socket)
{
    static unsigned char ucRtn[] = {
        32,0, 76,0,  24,0,  52,52, 20,0, 64,0, 52,0, 64,0, 36,52, 52,52, 52,0, 64,52, 52,0, 64,0, 52,0, 64,0, 52,52, 52,64, 52,0, 64,0, 
        52,0, 48,52, 52,88, 48,0, 48,0, 88,0, 48,0, 36,52, 48,0, 48,0, 48,52, 48,0, 20,0, 20,0, 20,0, 20,52, 20,0, 20,0, 20,24, 20,0, 
        52,52, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 20,20, 0,0, 
        0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 36,48, 
    };
    static int iCnt = 0;
    int iRtn = 0;

    if (ucRtn[iCnt] != 0) {
        fnSendBufTCP(Socket, test_buffer, ucRtn[iCnt], (TCP_BUF_SEND | TCP_BUF_SEND_REPORT_COPY));
        iRtn = APP_SENT_DATA;
    }
    iCnt++;
    if (ucRtn[iCnt] != 0) {
        fnSendBufTCP(Socket, test_buffer, ucRtn[iCnt], (TCP_BUF_SEND | TCP_BUF_SEND_REPORT_COPY));
        iRtn = APP_SENT_DATA;
    }
    iCnt++;
    return iRtn;
}

static int fnFastRxTestAck(USOCKET Socket)
{
    static unsigned short ucRtn[] = {
        0,0,0,36,0,0,0,0,0,0,64,0,0,52,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,52+88,0,0,0,52,0,0,0,0,0,52,0,0,0,0,0,36+52,52+52,52+52,52+52,52+36,
        52+52,52+52,52+52,52+52,36+52,52+24,52+52,16+16,16+16,16+1400+120,52,52+52,52+36,52+52,52+52,52+52,52+52,36+52,52+52,52,0,20+20,20+60,
        0, 88+48, 48+272, 
    };
    static int iCnt = 0;
    int iRtn = 0;

    if (ucRtn[iCnt] != 0) {
        if (ucRtn[iCnt] == (16 + 1400 + 120)) {
            fnSendBufTCP(Socket, test_buffer, (16 + 1400), (TCP_BUF_SEND | TCP_BUF_SEND_REPORT_COPY));
            fnSendBufTCP(Socket, test_buffer, (120 + 52), (TCP_BUF_SEND | TCP_BUF_SEND_REPORT_COPY));
        }
        else {
            fnSendBufTCP(Socket, test_buffer, ucRtn[iCnt], (TCP_BUF_SEND | TCP_BUF_SEND_REPORT_COPY));
        }
        iRtn = APP_SENT_DATA;
    }
    iCnt++;
    return iRtn;
}
#endif

static int fnServerTestListener(USOCKET Socket, unsigned char ucEvent, unsigned char *ucIp_Data, unsigned short usPortLen)
{
    #if !defined FAST_PEER_DROPED_FRAME_TEST
    static unsigned char ucTestMode = 0;
    #endif
    switch (ucEvent) {
    case TCP_EVENT_CONREQ:                                               // session request received on the TCP port
        fnDebugMsg("Connection request\n\r");
        break;
    case TCP_EVENT_CONNECTED:
        fnDebugMsg("Connection established\n\r");
        break;
    case TCP_EVENT_DATA:
    #if defined FAST_PEER_DROPED_FRAME_TEST
        return fnFastRxTest(Socket);
    #else
        if (ucTestMode == 0) {                                           // the first data indicates the test mode
            ucTestMode = *ucIp_Data;
            if (fnTestTCP(Socket, ucTestMode) > 0) {
                return APP_SENT_DATA;
            }
        }
        break;
    #endif
    case TCP_EVENT_ACK:
    #if defined FAST_PEER_DROPED_FRAME_TEST
        return fnFastRxTestAck(Socket);
    #else
        if (fnTestTCP(Socket, ucTestMode) > 0) {
            return APP_SENT_DATA;
        }
        break;
    #endif
    }
    return APP_ACCEPT;                                                   // default is to accept a connection
}
#endif

extern CHAR *fnSkipWhiteSpace(CHAR *ptr_input)
{
    while ((*ptr_input == ' ') || (*ptr_input == TAB_KEY)) {
        ptr_input++;                                                     // remove white space
    }
    return (ptr_input);
}

#if (defined USE_TELNET && defined USE_TELNET_LOGIN)
static unsigned char fnCheckUserPass(CHAR *ptrData, unsigned char ucInputLength)
{
    switch (ucPasswordState) {
    case TELNET_LOGIN:
    case START_PASSWORD:
        // The user has just entered the present user name and password in the form "user:pass"
        *(ptrData + ucInputLength) = '&';                                // terminate input in the form we require
        if (fnVerifyUser(ptrData, (DO_CHECK_USER_NAME | DO_CHECK_PASSWORD | HTML_PASS_CHECK)) != 0) {
            fnDebugMsg("\n\rError - false user details!\n\r");
            if (usTelnet_state == ES_NETWORK_LOGIN){
                fnTelnet(Telnet_socket, LEAVE_ECHO_MODE);
                usTelnet_state = ES_TERMINATE_CONNECTION;                // quit an active Telnet session
            }
            else {
                fnGotoNextState(ES_NO_CONNECTION);
            }
            return PASSWORD_IDLE;
        }
        if (ucPasswordState == TELNET_LOGIN) {                           // the user has successfully logged in
            fnLoginSuccess();
            return PASSWORD_IDLE;
        }
        fnDebugMsg("\n\rPlease enter new user name (4..8 characters): ");
        return ENTER_USER_NAME;

    case ENTER_USER_NAME:
        if ((ucInputLength < 4) || (ucInputLength > 8)) {
            fnDebugMsg("\n\rError - invalid length\n\r");
            return PASSWORD_IDLE;
        }
        uMemcpy(temp_pars->temp_parameters.cUserName, ptrData, ucInputLength);
        if (ucInputLength < 8) {
            temp_pars->temp_parameters.cUserName[ucInputLength] = '&';
        }
        fnDebugMsg("\n\rPlease enter new password (4..8 characters): ");
        return ENTER_NEW_PASSWORD;

    case ENTER_NEW_PASSWORD:
        if ((ucInputLength < 4) || (ucInputLength > 8)) {
            fnDebugMsg("\n\rError - invalid length\n\r");
            return PASSWORD_IDLE;
        }
        uMemcpy(temp_pars->temp_parameters.cUserPass, ptrData, ucInputLength);
        if (ucInputLength < 8) {
            temp_pars->temp_parameters.cUserPass[ucInputLength] = '&';
        }
        fnDebugMsg("\n\rPlease confirm the new password: ");
        return CONFIRM_NEW_PASS;

    case CONFIRM_NEW_PASS:
        *(ptrData + ucInputLength) = '&';                                // Terminate input in the form we require
        if (!fnCheckPass(temp_pars->temp_parameters.cUserPass, ptrData)) {
            fnDebugMsg("\n\rNew user data set\n\r");
            fnSaveNewPars(SAVE_NEW_PARAMETERS);
        }
        else {
            fnDebugMsg("\n\rError - false password!\n\r");
        }
        break;
    }
    return PASSWORD_IDLE;
}
#endif


extern int fnCommandInput(unsigned char *ptrData, unsigned short usLen, int iSource)
{
    #define MAX_DEBUG_IN 40
    int iReturn = 0;
#if defined PREVIOUS_COMMAND_BUFFERS                                     // {35}
    static CHAR cDebugIn[PREVIOUS_COMMAND_BUFFERS][MAX_DEBUG_IN];        // list of previous commands
    static int iDebugBufferIndex = 0;
    static unsigned char ucEscapeSequence = 0;
#else
    static CHAR cDebugIn[1][MAX_DEBUG_IN];
    #define iDebugBufferIndex      0
#endif

#if defined USE_TELNET_CLIENT                                            // {72}
    if (iTELNET_clientActive != 0) {                                     // if Telnet client connection is being controlled
        int iTELNET_interface = (iTELNET_clientActive - 1);              // the active interface index
        fnSendBufTCP(telnet_client_details[iTELNET_interface].Socket, ptrData, usLen, TCP_BUF_SEND); // send to Telnet server
        return 0;
    }
#endif
#if defined USE_USB_HID_KEYBOARD && defined SUPPORT_FIFO_QUEUES
    if ((usUSB_state & ES_USB_KEYBOARD_MODE) != 0) {                     // {77} if the input is connected to the USB keyboard connection
        if (keyboardQueue != NO_ID_ALLOCATED) {                          // if there is a FIFO queue to put the input ino
            fnWrite(keyboardQueue, ptrData, usLen);                      // put the received input to the USB keyboard FIFO
    #if defined IN_COMPLETE_CALLBACK
            fnInterruptMessage(TASK_USB, EVENT_USB_KEYBOARD_INPUT);      // wake the USB task so that it can convert the queued data to keyboard strokes
    #endif
            return 0;
        }
    }
#endif
#if defined USE_FTP_CLIENT                                               // {37}
    if ((iFTP_data_state & (FTP_DATA_STATE_GETTING | FTP_DATA_STATE_PUTTING)) != 0) { // if getting or putting a file
    #if defined FTP_CLIENT_BUFFERED_SOCKET_MODE
        unsigned short usLengthToSend = usLen;                           // backup the buffer length
    #endif
        while (usLen-- != 0) {
            switch (*ptrData) {
            case ASCII_ETX:                                              // (ctrl + C) abort a get or terminate a put
                fnTCP_close(uFtpClientDataSocket);
                iFTP_data_state |= FTP_DATA_STATE_CLOSING;               // mark that we are expecting the connection to terminate
                iFTP_data_state &= ~(FTP_DATA_STATE_GETTING | FTP_DATA_STATE_PUTTING);
                return 0;
            case 'p':                                                    // pause or unpause get
            case 'P':
                if (iFTP_data_state & FTP_DATA_STATE_GETTING) {          // only valid when getting and not when putting
                    iFTP_data_state ^= FTP_DATA_STATE_PAUSE;
                    if ((iFTP_data_state & FTP_DATA_STATE_PAUSE) == 0) { // just left pause state
                        fnDoFTP_flow_control(uFtpClientDataSocket, 1);   // update the window to allow further reception
                    }
                }
                // Fall through intentional
                //
            default:
    #if defined FTP_SIMPLE_DATA_SOCKET
                if ((iFTP_data_state & FTP_DATA_STATE_PUTTING) != 0) {   // input is sent to FTP server when putting
                    if ((iFTP_data_state & FTP_DATA_STATE_SENDING) != 0) {
                        if (ucFTP_buffer_content[1] < FTP_TX_BUFFER_MAX) {
                            FTP_tx[1].ucTCP_Message[ucFTP_buffer_content[1]++] = *ptrData; // collect data in next buffer
                        }
                    }
                    else {
                        if (ucFTP_buffer_content[0] < FTP_TX_BUFFER_MAX) {
                            FTP_tx[0].ucTCP_Message[ucFTP_buffer_content[0]++] = *ptrData; // collect data in active buffer
                        }
                    }
                }
    #endif
                break;
            }
            ptrData++;
        }
    #if defined FTP_SIMPLE_DATA_SOCKET
        if ((ucFTP_buffer_content[0] != 0) && (!(iFTP_data_state & FTP_DATA_STATE_SENDING))) { // only filled in put mode
            iFTP_data_state |= FTP_DATA_STATE_SENDING;
            fnSendTCP(uFtpClientDataSocket, (unsigned char *)&FTP_tx[0].tTCP_Header, ucFTP_buffer_content[0], TCP_FLAG_PUSH); // send the buffer to FTP server (to file)
        }
    #elif defined FTP_CLIENT_BUFFERED_SOCKET_MODE
        fnSendBufTCP(uFtpClientDataSocket, (ptrData - usLengthToSend), usLengthToSend, (TCP_BUF_SEND | TCP_BUF_SEND_REPORT_COPY)); // send using buffered TCP (in some cases the output buffer should be checked to ensure that there is adequate space but this is not performed here)
    #endif
        return 0;
    }
#endif

    while (usLen-- != 0) {
        if ((DELETE_KEY == *ptrData) || (CONTROL_QUESTION_MARK == *ptrData)) { // {90} putty defaults to using control-? instead of control-H for back space
            if (ucDebugCnt != 0) {
                ucDebugCnt--;                                            // we delete it from our local buffer but not from display
                fnDebugMsg((CHAR *)&cDeleteInput[1]);
            }
#if defined PREVIOUS_COMMAND_BUFFERS                                     // {35}
            ucEscapeSequence = 0;
#endif
            continue;
        }
#if defined PREVIOUS_COMMAND_BUFFERS                                     // {35}
        else if (*ptrData == ESCAPE_SEQUENCE_START) {                    // start of escape sequence detected
            ucEscapeSequence = ESCAPE_SEQUENCE_START;
            ptrData++;
            continue;
        }
        else if ((*ptrData == ESCAPE_ARROWS) && (ucEscapeSequence == ESCAPE_SEQUENCE_START)) {
            ucEscapeSequence = ESCAPE_ARROWS;
            ptrData++;
            continue;
        }
        else if (ucEscapeSequence == ESCAPE_ARROWS) {                    // arrow direction expected
            int iOriginalLength;
            switch (*ptrData) {
            case ARROW_LEFT_SEQUENCE:                                    // move left in buffer to edit
                if (ucDebugCnt != 0) {
                    ucDebugCnt--;
                }
                continue;
            case ARROW_RIGHT_SEQUENCE:                                   // move right in buffer to edit
                if (ucDebugCnt < MAX_DEBUG_IN) {
                    ucDebugCnt++;
                }
                continue;
            case ARROW_UP_SEQUENCE:                                      // scroll up through previous commands
            case ARROW_DOWN_SEQUENCE:                                    // scroll down through previous commands
                iOriginalLength = uStrlen(cDebugIn[iDebugBufferIndex]);
                if (ARROW_UP_SEQUENCE == *ptrData) {
                    if (iDebugBufferIndex == 0) {
                        iDebugBufferIndex = (PREVIOUS_COMMAND_BUFFERS - 1);
                    }
                    else {
                        iDebugBufferIndex--;
                    }
                    fnDebugMsg("\n\r\r");
                }
                else {
                    if (++iDebugBufferIndex >= PREVIOUS_COMMAND_BUFFERS) { // move to the next command buffer
                        iDebugBufferIndex = 0;                           // overflow back to first buffer
                    }
                    fnDebugMsg("\r");
                }
                while (iOriginalLength-- != 0) {
                    fnDebugMsg(" ");
                }
                fnDebugMsg("\r");
                fnDebugMsg(cDebugIn[iDebugBufferIndex]);
                ucDebugCnt = uStrlen(cDebugIn[iDebugBufferIndex]);
                continue;
            default:
                break;
            }
        }
#endif
        if ((*ptrData == '\n') || (*ptrData == 0)) {                     // some telnet clients send \r\n and some \r\0 - others only \r
            continue;
        }
        cDebugIn[iDebugBufferIndex][ucDebugCnt] = *ptrData;
        if (*ptrData == '\r') {
            cDebugIn[iDebugBufferIndex][ucDebugCnt] = '\n';
#if (defined USE_TELNET && defined USE_TELNET_LOGIN)
            if ((ucPasswordState != PASSWORD_IDLE) && ((usTelnet_state != ES_NETWORK_LOGIN) || (iSource == SOURCE_NETWORK))) {
                ucPasswordState = fnCheckUserPass(cDebugIn[iDebugBufferIndex], ucDebugCnt);
                ucDebugCnt = 0;
                return 1;                                                // we will always send something so report that a transmission has been started
            }
#endif
            fnSaveDebugHandle(iSource);                                  // save present debug handle and set appropriate
            fnDoDebug(cDebugIn[iDebugBufferIndex], (unsigned char)(ucDebugCnt + 1), iSource); // treat debug messages
            fnRestoreDebugHandle();                                      // return the present debug output
            iReturn = 1;                                                 // we will always send something so report that a transmission has been started
            ucDebugCnt = 0;
#if defined PREVIOUS_COMMAND_BUFFERS                                     // {35}
            if (++iDebugBufferIndex >= PREVIOUS_COMMAND_BUFFERS) {       // move to the next command buffer
                iDebugBufferIndex = 0;                                   // overflow back to first buffer
            }
#endif
        }
        else {
            ucDebugCnt++;
        }
#if defined PREVIOUS_COMMAND_BUFFERS                                     // {35}
        ucEscapeSequence = 0;
#endif
        ptrData++;
        if (ucDebugCnt >= MAX_DEBUG_IN) {
            ucDebugCnt = (MAX_DEBUG_IN - 1);
        }
    }
    return (iReturn);
}


extern int fnInitiateLogin(unsigned short usNextState)
{
    switch (usNextState) {                                               // {8}
#if defined USE_USB_CDC
    case ES_USB_LOGIN:
        if ((usTelnet_state != ES_NO_CONNECTION) || (usData_state != ES_NO_CONNECTION)) { // can't login because already connected
            return TELNET_ON_LINE;
        }
        usUSB_state = usNextState;
        break;
#endif
#if defined SERIAL_INTERFACE
    case ES_SERIAL_LOGIN:
        if ((usUSB_state != ES_NO_CONNECTION) || (usTelnet_state != ES_NO_CONNECTION)) { // can't login because already connected
            return TELNET_ON_LINE;
        }
        fnGotoNextState(usNextState);
        break;
#endif
    default:                                                             // network login
        usTelnet_state = ES_NETWORK_LOGIN;
#if (defined USE_TELNET && defined USE_TELNET_LOGIN)
        ucPasswordState = TELNET_LOGIN;                                  // we request login before continuing
        fnDebugMsg("\n\rWelcome to the Telnet server.\n\rPlease enter user name and password (user:pass): ");
        return DO_PASSWORD_ENTRY;
#else
        break;
#endif
    }
    fnLoginSuccess();                                                    // no login - go directly to connected state
    return DIRECT_LOGIN;
}

    #if defined SERIAL_INTERFACE
// Echo input or send start on password entry
//
extern void fnEchoInput(unsigned char *ucInputMessage, QUEUE_TRANSFER Length)
{
#if defined USE_TELNET_CLIENT                                            // {72}
    if (iTELNET_clientActive != 0) {                                     // don't echo since the TELNET server will do
        return;
    }
#endif
    if ((usTelnet_state != ES_NETWORK_LOGIN) && (ucPasswordState == TELNET_LOGIN)) {
        if (*ucInputMessage == DELETE_KEY) {
            fnWrite(SerialPortID, (unsigned char *)cDeleteInput, 3);
        }
        else if (*ucInputMessage == ':') {
            fnWrite(SerialPortID, ucInputMessage, 1);
        }
        else {                                                           // we want to hide password entry when logging in
            fnWrite(SerialPortID, (unsigned char *)"*", 1);              // send one star back (we assume human entry with one byte frames)
        }
    }
    else {
#if defined USE_USB_HID_KEYBOARD && defined SUPPORT_FIFO_QUEUES
        if (usUSB_state & ES_USB_KEYBOARD_MODE) {                        // {77} if the input is connected to the USB keyboard connection we don't echo
            return;
        }
#endif
        fnWrite(SerialPortID, ucInputMessage, Length);                   // echo received character(s)
    }
}
    #endif
#else                                                                    // endif KEEP_DEBUG
void fnDebug(TTASKTABLE *ptrTaskTable)                                   // dummy task
{
}
    #if !defined REMOVE_PORT_INITIALISATIONS
static void fnSetPortBit(unsigned short usBit, int iSetClr);
static int  fnConfigOutputPort(CHAR cPortBit);
    #endif
#endif                                                                   // endif not KEEP_DEBUG


#if defined USE_TELNET
extern void fnConfigureTelnetServer(void)
{
    if (temp_pars->temp_parameters.usServers[DEFAULT_NETWORK] & ACTIVE_TELNET_SERVER) {
    #if defined IMMEDIATE_MEMORY_ALLOCATION                              // {57} extra parameter for tx buffer size to immediately allocate (uses default size)
        Telnet_socket = fnStartTelnet(temp_pars->temp_parameters.usTelnetPort, (2 * 60), 0, 0, 0, fnTELNETListener); // 2 minute inactivity timeout
    #else
        Telnet_socket = fnStartTelnet(temp_pars->temp_parameters.usTelnetPort, (2 * 60), 0, 0, fnTELNETListener); // 2 minute inactivity timeout
    #endif
    }
    else {
        fnStopTelnet(Telnet_socket);
        usTelnet_state = ES_NO_CONNECTION;
    #if defined SERIAL_INTERFACE
        ucPasswordState = PASSWORD_IDLE;
    #endif
    }
    #if defined TEST_CLIENT_SERVER
    fnTestTCPServer(TCP_TEST_SERVER_START);                              // start the test TCP server
    #endif
    #if defined TEST_TCP_SERVER                                          // {30}
    if (test_server_socket < 0) {
        test_server_socket = fnStartTelnet(3000, (2 * 60), 0, 0, fnServerTestListener);
    }
    #endif
}
#endif

#if defined USE_PARAMETER_BLOCK
extern void fnResetChanges(void)
{
    int iActions = 0;
    #define CHANGE_SERIAL_SETTINGS 0x01
    #define CHANGE_WEB_SERVER      0x02
    #define CHANGE_FTP_SERVER      0x04
    #define CHANGE_TELNET_SERVER   0x08
    if ((temp_pars->temp_parameters.SerialMode != parameters->SerialMode) ||  // check whether serial setting are being set back
        (temp_pars->temp_parameters.ucFlowHigh != parameters->ucFlowHigh) ||
        (temp_pars->temp_parameters.ucFlowLow != parameters->ucFlowLow) ||
        (temp_pars->temp_parameters.ucSerialSpeed != parameters->ucSerialSpeed))
    {
        iActions |= CHANGE_SERIAL_SETTINGS;
    }
    if ((temp_pars->temp_parameters.usServers[DEFAULT_NETWORK] & (AUTHENTICATE_WEB_SERVER | ACTIVE_WEB_SERVER)) != (parameters->usServers[DEFAULT_NETWORK] & (AUTHENTICATE_WEB_SERVER | ACTIVE_WEB_SERVER))) {
        iActions |= CHANGE_WEB_SERVER;
    }
    if ((temp_pars->temp_parameters.usServers[DEFAULT_NETWORK] & (ACTIVE_FTP_SERVER)) != (parameters->usServers[DEFAULT_NETWORK] & (ACTIVE_FTP_SERVER))) {
        iActions |= CHANGE_FTP_SERVER;
    }
    if ((temp_pars->temp_parameters.usServers[DEFAULT_NETWORK] & (ACTIVE_TELNET_SERVER)) != (parameters->usServers[DEFAULT_NETWORK] & (ACTIVE_TELNET_SERVER))) {
        iActions |= CHANGE_TELNET_SERVER;
    }
    fnGetOurParameters(1);                                               // get original parameters from FLASH, but preserve DHCP defined values
    #if defined SERIAL_INTERFACE && defined DEMO_UART                    // {10}
    if ((iActions & CHANGE_SERIAL_SETTINGS) != 0) {
        fnSetNewSerialMode(MODIFY_CONFIG);                               // return settings to interface
    }
    #endif
    if ((CHANGE_WEB_SERVER & iActions) != 0) {
    #if defined USE_HTTP
        fnConfigureAndStartWebServer();
    #endif
    }
    if ((CHANGE_FTP_SERVER & iActions) != 0) {
        fnConfigureFtpServer(FTP_TIMEOUT);                               // {3}
    }
    #if defined USE_TELNET
    if ((CHANGE_TELNET_SERVER & iActions) != 0) {
        fnConfigureTelnetServer();
    }
    #endif
}
#endif

#if !defined REMOVE_PORT_INITIALISATIONS
    #if defined USE_PARAMETER_BLOCK
// Save port setup to flash
//
extern void fnSavePorts(void)
{
    unsigned char ucTempUserOutputs = temp_pars->temp_parameters.ucUserOutputs;
    unsigned char ucTempUserOutputValues = 0;
    unsigned short usTempUserDefinedOutputs = temp_pars->temp_parameters.usUserDefinedOutputs;
    CHAR cPort = '1';
    unsigned char ucBit = 1;
        #if defined SUPPORT_LCD && defined LCD_CONTRAST_CONTROL          // {88}
    unsigned char ucContrast = temp_pars->temp_parameters.ucGLCDContrastPWM;
        #endif

    while (cPort < '9') {
        if (fnPortState(cPort++)) {
            ucTempUserOutputValues |= ucBit;
        }
        ucBit <<= 1;
    }

    fnResetChanges();                                                    // ensure we don't save any unwanted changes

    temp_pars->temp_parameters.ucUserOutputs = ucTempUserOutputs;
    temp_pars->temp_parameters.ucUserOutputValues = ucTempUserOutputValues;
    temp_pars->temp_parameters.usUserDefinedOutputs = usTempUserDefinedOutputs;
        #if defined SUPPORT_LCD && defined LCD_CONTRAST_CONTROL          // {88}
    temp_pars->temp_parameters.ucGLCDContrastPWM = ucContrast;
        #endif
    fnSaveNewPars(SAVE_NEW_PARAMETERS);                                  // save these settings as default
}
    #endif

// Set all port bits to new state
// The ARM processors tend to use set and clear registers to perform the job
//
static void fnSetUserPortOut(unsigned short usPortOutputs)
{
    unsigned short usBit = 0x0001;

    while (usBit != 0) {
        fnSetPortBit(usBit, ((usPortOutputs & usBit) != 0));
        usBit <<= 1;
    }
    temp_pars->temp_parameters.usUserDefinedOutputs = usPortOutputs;     // backup the new setting
}

// Initialise ports to input/output
//
extern void fnInitialisePorts(void)
{
    CHAR sPort = '0';
    CHAR cType;
    unsigned char ucBit = 1;
    unsigned char ucUserOutputs = temp_pars->temp_parameters.ucUserOutputs;

    temp_pars->temp_parameters.ucUserOutputValues &= ucUserOutputs;      // clear inputs
    fnSetPortOut(temp_pars->temp_parameters.ucUserOutputValues, 1);      // configure all user outputs to default states

    while (sPort < '8') {                                                // up to 8 demo port bits
        cType = 'o';
        if ((ucUserOutputs & ucBit) == 0) {
            cType = 'i';
        }
        fnConfigPort(sPort++, cType);                                    // configure port as input or output
        ucBit <<= 1;
    }
                                                                         // initialise user ports as outputs with default values
    sPort = 'a';                                                         // first user output port
    while (sPort < 'q') {                                                // until last port
        fnConfigOutputPort(sPort++);                                     // configure port bits as outputs
    }
    fnSetUserPortOut(temp_pars->temp_parameters.usUserDefinedOutputs);   // set all user outputs to default states
}
#endif

#if defined USE_USB_CDC && defined ACTIVE_FILE_SYSTEM                    // {8}
    typedef struct stUPLOAD_HEADER                                       // the start of a uTasker boot binary file
    {
        unsigned long  ulCodeLength;
        unsigned short usMagicNumber;
        unsigned short usCRC;
    } UPLOAD_HEADER;

    #if defined SUB_FILE_SIZE
        #define SUBFILE_WRITE  , ucSubFileInProgress
        #define SUB_FILE_ON    ,SUB_FILE_TYPE
    #else
        #define SUBFILE_WRITE
        #define SUB_FILE_ON
    #endif

    #if defined SUB_FILE_SIZE
        static unsigned char    ucSubFileInProgress = 0;
    #endif

    #if defined (_WINDOWS) || defined (_LITTLE_ENDIAN)
static void fnHeaderToLittleEndian(UPLOAD_HEADER *file_header)
{
    unsigned short usShort;
    unsigned long  ulLong;

    usShort = (file_header->usMagicNumber >> 8);
    file_header->usMagicNumber <<= 8;
    file_header->usMagicNumber |= usShort;

    ulLong = (file_header->ulCodeLength >> 24);
    ulLong |= (((file_header->ulCodeLength >> 16) & 0xff) << 8);
    ulLong |= (((file_header->ulCodeLength >> 8) & 0xff) << 16);
    ulLong |= (((file_header->ulCodeLength) & 0xff) << 24);
    file_header->ulCodeLength = ulLong;
}
    #endif

// Binary download support for USB
// This routine coordinates the task of opening the file on first contact, collecting buffers for saving and terminating when all data has been received.
// The format must be uTasker boot loader binary (which can also be encrypted)
//
extern void fnDownload(unsigned char *ptrData, QUEUE_TRANSFER Length)
{
    #define DOWNLOAD_STATE_IDLE       0
    #define DOWNLOAD_STATE_COLLECTING 1
    #define DOWNLOAD_STATE_SAVING     2
    #define DOWNLOAD_ERROR_STATE      3
    #define DOWNLOAD_BUFFER_SIZE      512

    static int iDownloadState = DOWNLOAD_STATE_IDLE;
    static unsigned long ulBufferData;
    static unsigned long ulExpectedData;
    static unsigned long ulSavedLength;
    static unsigned char *ptrWorkSpace = 0;
    static unsigned char *ptrBufferPointer;
    static unsigned char *ptrFile;
    switch (iDownloadState) {
    case 0:
        if (ptrWorkSpace == 0) {
            ptrWorkSpace = ptrBufferPointer = uMallocAlign(DOWNLOAD_BUFFER_SIZE, sizeof(unsigned long)); // get working memory on first contact (aligned to long word)
        }
        ulBufferData = 0;
        ulSavedLength = 0;
        ptrBufferPointer = ptrWorkSpace;
        iDownloadState = DOWNLOAD_STATE_COLLECTING;
        // Fall through intentionally
        //
    case DOWNLOAD_STATE_COLLECTING:
        while (ulBufferData < DOWNLOAD_BUFFER_SIZE) {
            if (Length == 0) {
                return;
            }
            *ptrBufferPointer++ = *ptrData++;
            Length--;
            ulBufferData++;
            if (ulBufferData == sizeof(UPLOAD_HEADER)) {                 // the boot loader header has been collected
                UPLOAD_HEADER *file_header = (UPLOAD_HEADER *)ptrWorkSpace;
#if defined (_WINDOWS) || defined (_LITTLE_ENDIAN)
                fnHeaderToLittleEndian(file_header);                     // convert header content to little endian format
#endif
                if (file_header->usMagicNumber != VALID_VERSION_MAGIC_NUMBER) {
                    fnDebugMsg("\r\nBAD FORMAT - quitting\r\n");
                    iDownloadState = DOWNLOAD_ERROR_STATE;
                    return;
                }
                if (file_header->ulCodeLength >= (SIZE_OF_FLASH/2)) {    // simple check that the length is valid
                    fnDebugMsg("\r\nBAD LENGTH - quitting\r\n");
                    iDownloadState = DOWNLOAD_ERROR_STATE;
                    return;
                }
                ulExpectedData = (file_header->ulCodeLength + sizeof(UPLOAD_HEADER)); // the length we will receive before terminating
                ptrFile = uOpenFile(USB_DOWNLOAD_FILE);                  // open a file to save the data to
        #if defined SUB_FILE_SIZE
                ucSubFileInProgress = fnGetFileType(USB_DOWNLOAD_FILE);  // get file characteristics so that it is later handled correctly
        #endif
                iDownloadState = DOWNLOAD_STATE_SAVING;
                fnDebugMsg("Downloading..");
#if defined (_WINDOWS) || defined (_LITTLE_ENDIAN)
                fnHeaderToLittleEndian(file_header);                     // convert header back to original format
#endif
                break;
            }
        }
        // Fall through intentional
        //
    case DOWNLOAD_STATE_SAVING:
        while (ulBufferData < DOWNLOAD_BUFFER_SIZE) {
            if (Length == 0) {
                break;
            }
            *ptrBufferPointer++ = *ptrData++;
            Length--;
            ulBufferData++;
            if (ulBufferData == DOWNLOAD_BUFFER_SIZE) {                  // a buffer has been collected so we save it
                fnDebugMsg("*");                                         // display progress
                uFileWrite(ptrFile, ptrWorkSpace, DOWNLOAD_BUFFER_SIZE SUBFILE_WRITE); // write the block to the file
                ulSavedLength += DOWNLOAD_BUFFER_SIZE;
                ulBufferData = 0;
                ptrBufferPointer = ptrWorkSpace;
            }
        }
        break;

    case DOWNLOAD_ERROR_STATE:
        if ((ptrData == 0) && (Length == 0)) {
            iDownloadState = 0;
        }
        return;
    }

    if ((ulSavedLength + ulBufferData) >= ulExpectedData) {              // if all data has been collected
        #if defined SUPPORT_MIME_IDENTIFIER
        unsigned char ucMimeType = MIME_BINARY;
        #endif
        if ((ulExpectedData > ulSavedLength) != 0) {
            uFileWrite(ptrFile, ptrWorkSpace, (ulExpectedData - ulSavedLength) SUBFILE_WRITE); // write the remainder to the file
        }
        #if defined SUPPORT_MIME_IDENTIFIER                              // close the file
        uFileCloseMime(ptrFile, &ucMimeType);                            // this will cause the file length and type to be written in the file
        #else
        uFileClose(ptrFile);                                             // this will cause the file length to be written in the file
        #endif
        fnDebugMsg("\r\nTerminated, resetting...");
        uTaskerMonoTimer(TASK_APPLICATION, (DELAY_LIMIT)(2 * SEC), E_TIMER_SW_DELAYED_RESET);
    }
}

    #endif

    #if defined TEST_CLIENT_SERVER

static int fnTestListener(USOCKET Socket, unsigned char ucEvent, unsigned char *ucIp_Data, unsigned short usPortLen)
{
  //static int iDelay = 0;
	switch (ucEvent) {
    case TCP_EVENT_CONNECTED:
      //fnTestTCPServer(START_SERVER_TEST);
      //return (fnTestTCPServer(TCP_SERVER_TX));
        fnDebugMsg("Connected\r\n");
        break;
    case TCP_EVENT_ACK:
        fnTestTCPServer(ALLOW_NEXT_MESSAGE); 
      //if (++iDelay > 5) {
            fnTestTCPServer(INCREMENT_SERVER_MESSAGE);        
            return (fnTestTCPServer(TCP_SERVER_TX));
      //}
        break;
    case TCP_EVENT_REGENERATE:
        return (fnTestTCPServer(TCP_SERVER_REPEAT));
    case TCP_EVENT_ABORT:
        fnDebugMsg("Connection reset\n\r");
    case TCP_EVENT_CLOSED:
        fnTestTCPServer(TCP_TEST_SERVER_LISTEN);
        break;
    case TCP_EVENT_DATA:
        fnTestTCPServer(INCREMENT_SERVER_MESSAGE);        
        return (fnTestTCPServer(TCP_SERVER_TX));
    case TCP_EVENT_CONREQ:
    case TCP_EVENT_PARTIAL_ACK:
    case TCP_EVENT_CLOSE:
        break;
    case TCP_EVENT_ARP_RESOLUTION_FAILED:
        fnDebugMsg("ARP failed\n\r");
        break;
    }
    return APP_ACCEPT;
}

#if defined _LPC23XX
static void server_int(void)
{
    fnInterruptMessage(OWN_TASK, TCP_SERVER_TEST);
}
#endif

static int fnTestTCPServer(int iAction)
{
    typedef struct stTCP_MESSAGE
    {
        TCP_HEADER     tTCP_Header;                                      // reserve header space
        unsigned char  ucTCP_Message[10];
    } TCP_MESSAGE;
    #define TEST_SERVERPORT    1234
    static const unsigned char ucRemoteIP[] = {192, 168, 0, 4};
    static int iTCPState = 0;
    static USOCKET test_socket = -1;
    static TCP_MESSAGE TCP_server_data;
    switch (iAction) {
    case TCP_TEST_SERVER_START:
        test_socket = fnGetTCP_Socket(TOS_MAXIMISE_THROUGHPUT, TCP_DEFAULT_TIMEOUT, fnTestListener);
    case TCP_TEST_SERVER_LISTEN:
        iTCPState = 0;
        fnTCP_Listen(test_socket, TEST_SERVERPORT, 0);
        break;
    case CONNECT_REMOTE:
        fnTCP_Connect(test_socket, (unsigned char *)ucRemoteIP, TEST_SERVERPORT, 0, 0);
        break;
    case START_SERVER_TEST:                                              // on connection from remote client
        {
        #if defined _LPC23XX
            static TIMER_INTERRUPT_SETUP timer_setup = {0};              // interrupt configuration parameters
            fnTestTCPServer(RESET_SERVER_MESSAGE);
            timer_setup.int_type = TIMER_INTERRUPT;
            timer_setup.int_priority = PRIORITY_TIMERS;
            timer_setup.int_handler = server_int;
            timer_setup.timer_reference = 2;                             // timer channel 2
            #if defined _HW_TIMER_MODE
            timer_setup.timer_mode = TIMER_PERIODIC;                     // period timer interrupt        
            timer_setup.timer_value = TIMER_US_DELAY(9500);             // periodic timer us value
            #else
            timer_setup.timer_value = 0;
            timer_setup.timer_us_value = 9500;
            #endif
            fnConfigureInterrupt((void *)&timer_setup);                  // enter interrupt for server timer
        #endif
        }
        break;
    case ALLOW_NEXT_MESSAGE:
        iTCPState = 0;
        break;
    case INCREMENT_SERVER_MESSAGE:
        {
            int iIndex = (sizeof(TCP_server_data.ucTCP_Message) - 1);
            if (iTCPState != 0) {
                break;
            }
            while (iIndex >= 0) {
                if (++TCP_server_data.ucTCP_Message[iIndex] == 0) {
                    iIndex--;
                }
                break;
            }
        }
        break;
    case RESET_SERVER_MESSAGE:
        uMemset(&TCP_server_data.ucTCP_Message, 0, sizeof(TCP_server_data.ucTCP_Message));
        break;
    case TCP_SERVER_TX:
        if (iTCPState != 0) {
            break;
        }
    case TCP_SERVER_REPEAT:
        {
            static int iSent = 0;
            int iIndex = (sizeof(TCP_server_data.ucTCP_Message) - 1);
            if (TCP_server_data.ucTCP_Message[iIndex] == 1) {
                TCP_server_data.ucTCP_Message[iIndex] = 1;
            }
            if (iAction == TCP_SERVER_REPEAT) {
                iTCPState = 0;
                iSent--;
            }
            if (iSent > 1) {
                break;
            }
            if (fnSendTCP(test_socket, (unsigned char *)&TCP_server_data.tTCP_Header, sizeof(TCP_server_data.ucTCP_Message), TCP_FLAG_PUSH) > 0) {
                iTCPState = 1;                                               // we are expecting an ack
                iSent++;
                return APP_SENT_DATA;
            }
        }
        break;
    }
    return APP_ACCEPT;
}
    #endif
#else                                                                    // else not USE_MAINTENANCE {6}
    #if defined USE_USB_CDC
    unsigned short usUSB_state = ES_USB_RS232_MODE;
    #endif
    #if defined SERIAL_INTERFACE
// Echo input or send start on password entry
//
extern void fnEchoInput(unsigned char *ucInputMessage, QUEUE_TRANSFER Length)
{
    fnWrite(SerialPortID, ucInputMessage, Length);                       // echo received character(s)
}
extern int fnInitiateLogin(unsigned short usNextState)
{
    return DIRECT_LOGIN;
}
extern int fnCommandInput(unsigned char *ptrData, unsigned short usLen, int iSource)
{
    return 0;
}
    #endif
#endif

extern void fnDisplayMemoryUsage(void)
{
    STACK_REQUIREMENTS stackUsed;                                        // {79}
    fnDebugMsg("\n\rSystem memory use:\n\r");
    fnDebugMsg("==================\n\r");
    fnDebugMsg("Free heap = ");
    fnDebugHex(fnHeapFree(), (sizeof(HEAP_REQUIREMENTS) | WITH_LEADIN));
    fnDebugMsg(" from ");
    fnDebugHex(fnHeapAvailable(), (sizeof(HEAP_REQUIREMENTS) | WITH_LEADIN));
    fnDebugMsg("\n\rUnused stack = ");
    fnDebugHex(fnStackFree(&stackUsed), (sizeof(unsigned long) | WITH_LEADIN)); // {79}
    fnDebugMsg(" (");
    fnDebugHex(stackUsed, (sizeof(unsigned long) | WITH_LEADIN));        // {79}
    fnDebugMsg(")\n\r");
}

#if defined USE_PARAMETER_BLOCK
// When this routine is called, the parameter block is set to a new valid block and the old block is deleted
//
extern int fnSaveNewPars(int iTemp)
{
    #if defined ETH_INTERFACE || defined USB_CDC_RNDIS || defined USE_PPP
    // Network variables
    //
    if (iTemp == SAVE_NEW_PARAMETERS_CHECK_CRITICAL) {                   // we check to see whether network parameters have been changed
       if (((temp_pars->temp_parameters.usServers[DEFAULT_NETWORK] & ACTIVE_DHCP) != (parameters->usServers[DEFAULT_NETWORK] & ACTIVE_DHCP)) || (uMemcmp((unsigned char *)&temp_pars->temp_network[DEFAULT_NETWORK], (unsigned char *)&network_flash, sizeof(NETWORK_PARAMETERS)))) {
           return 1;                                                     // since a network parameter has been change, we warn the user that he/she will have to validate afterwards
       }
    }
    #endif
    #if !defined USE_PAR_SWAP_BLOCK
    fnDelPar(INVALIDATE_PARAMETER_BLOCK);                                // delete parameter block before continuing
    iTemp = SAVE_NEW_PARAMETERS;
    #endif
    #if defined ETH_INTERFACE || defined USB_CDC_RNDIS || defined USE_PPP
    fnSetPar((unsigned short)(PAR_NETWORK | TEMPORARY_PARAM_SET), (unsigned char *)&temp_pars->temp_network[DEFAULT_NETWORK], sizeof(temp_pars->temp_network)); // network parameters
    #endif

    // Device parameters
    //
    fnSetPar((PAR_DEVICE | TEMPORARY_PARAM_SET), (unsigned char *)&temp_pars->temp_parameters, sizeof(PARS)); // copy the new set to the swap buffer (temp)

    if (iTemp != SAVE_NEW_PARAMETERS_VALIDATE) {
    #if defined ACTIVE_FILE_SYSTEM
       fnDelPar(SWAP_PARAMETER_BLOCK);                                   // delete old block and validate new one.
    #endif
       uMemcpy(parameters, &temp_pars->temp_parameters, sizeof(PARS));
    }
    return 0;
}
#endif


extern CHAR *fnShowSN(CHAR *cValue)
{
    #if defined ETH_INTERFACE
    unsigned short usStart = temp_pars->temp_network[DEFAULT_NETWORK].ucOurMAC[1];
    unsigned long  ulEnd   = temp_pars->temp_network[DEFAULT_NETWORK].ucOurMAC[5];

    usStart += ((unsigned short)(temp_pars->temp_network[DEFAULT_NETWORK].ucOurMAC[0]) << 8);

    ulEnd += ((unsigned long)(temp_pars->temp_network[DEFAULT_NETWORK].ucOurMAC[4]) << 8);
    ulEnd += ((unsigned long)(temp_pars->temp_network[DEFAULT_NETWORK].ucOurMAC[3]) << 16);
    ulEnd += ((unsigned long)(temp_pars->temp_network[DEFAULT_NETWORK].ucOurMAC[2]) << 24);

    cValue = fnBufferDec(ulEnd,  LEADING_ZERO, cValue);
    *cValue++ = '-';
    cValue = fnBufferDec(usStart, (LEADING_ZERO | WITH_CR_LF), cValue);
    #else
    cValue = fnBufferDec(temp_pars->temp_parameters.ulSerialNumber, (LEADING_ZERO | WITH_CR_LF), cValue);
    #endif
    return (cValue);
}

extern void fnSetNewValue(int iType, CHAR *ptr_input)                    // {6}
{
    while ('+' == *ptr_input) {
        ptr_input++;
    }
    switch (iType) {
    case FLOW_CONTROL_HIGH_WATER:
        temp_pars->temp_parameters.ucFlowHigh = (unsigned char)fnDecStrHex(ptr_input);
        if (temp_pars->temp_parameters.ucFlowHigh > 99) {
            temp_pars->temp_parameters.ucFlowHigh = 99;
        }
        else if (temp_pars->temp_parameters.ucFlowHigh < 10) {
            temp_pars->temp_parameters.ucFlowHigh = 10;
        }
        break;

    case FLOW_CONTROL_LOW_WATER:
        temp_pars->temp_parameters.ucFlowLow = (unsigned char)fnDecStrHex(ptr_input);
        if (temp_pars->temp_parameters.ucFlowLow > 99) {
            temp_pars->temp_parameters.ucFlowLow = 99;
        }
        else if (temp_pars->temp_parameters.ucFlowLow < 10) {
            temp_pars->temp_parameters.ucFlowLow = 10;
        }
        break;

    case TELNET_PORT:
        temp_pars->temp_parameters.usTelnetPort = (unsigned short)fnDecStrHex(ptr_input);
        break;
    }
}

extern void fnConfigureFtpServer(unsigned short usTimeout)               // {3}{6}
{
#if defined USE_FTP
    if ((temp_pars->temp_parameters.usServers[DEFAULT_NETWORK] & ACTIVE_FTP_SERVER) != 0) { // should the FTP server be started?
        unsigned char ucFTP_mode = 0;
        if ((temp_pars->temp_parameters.usServers[DEFAULT_NETWORK] & ACTIVE_FTP_LOGIN) != 0) {
            ucFTP_mode = FTP_AUTHENTICATE;
        }
        fnStartFtp(usTimeout, ucFTP_mode);                               // start the FTP server
    }
    else {
        fnStopFtp();
    }
#endif
}


// We support the network transmit routine here
//
extern QUEUE_TRANSFER fnNetworkTx(unsigned char *output_buffer, QUEUE_TRANSFER nr_of_bytes)
{
#if defined USE_TELNET && defined USE_MAINTENANCE                        // {6} no Telnet without USE_MAINTENANCE
    if (output_buffer == 0) {                                            // {18} if a check of output buffer size call the correct function
        return (fnSendBufTCP(Telnet_socket, (unsigned char *)&OurTask, nr_of_bytes, TCP_BUF_CHECK));
    }
    return (fnSendBufTCP(Telnet_socket, output_buffer, nr_of_bytes, (TCP_BUF_SEND | TCP_BUF_SEND_REPORT_COPY)));
#else
    return 0;
#endif
}


#if defined _KINETIS
    #include "debug_hal_kinetis.h"                                       // {53}
#elif defined _iMX
    #include "debug_hal_iMX.h"
#elif defined _M5223X
    #include "debug_hal_m522xx.h"
#elif defined _STM32
    #include "debug_hal_stm32.h"
#elif defined _LM3SXXXX
    #include "debug_hal_luminary.h"
#elif defined _HW_AVR32
    #include "debug_hal_avr32.h"
#endif
//extern int fnPortState(CHAR cPortBit); - check the present state of a particular port
//extern int fnPortInputConfig(CHAR cPortBit); - check whether the port is configured as an input or as output
//extern int fnConfigPort(CHAR cPortBit, CHAR cType); - configure a processor port pin as input/output or analogue
//static void fnSetPortBit(unsigned short usBit, int iSetClr); - set a single output to logical '1' or '0' state
//extern int fnTogglePortOut(CHAR cPortBit); - toggle the state of an output port
//static int fnConfigOutputPort(CHAR cPortBit); - configure port bit as output (a..p are port bit references)
//extern int fnUserPortState(CHAR cPortBit); - request present logic level of an output
//extern void fnSetPortOut(unsigned char ucPortOutputs, int iInitialisation); - write a value to a group of output bits grouped as a byte
//extern unsigned char fnAddResetCause(CHAR *ptrBuffer); - determine the cause of the last reset
//optionally ezport interface routines
