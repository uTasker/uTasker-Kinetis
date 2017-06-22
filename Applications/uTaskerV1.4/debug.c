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
    Copyright (C) M.J.Butcher Consulting 2004..2016
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
//#define DEVELOP_PHY_CONTROL                                            // {33} activate to enable PHY register dump and writes to individual register addresses
                                                                         // note that STOP_MII_CLOCK should not be enabled when using this (kinetis)
//#define _DEBUG_CAN                                                     // support dumping CAN register details for debugging purpose
#if !defined KINETIS_KE && !defined KINETIS_KL
    #define EZPORT_CLONER                                                // {55}
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

    #define IIC_WRITE_ADDRESS       0xd0
    #define IIC_READ_ADDRESS        0xd1

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
    #define DO_MENU_HELP_FTP_TELNET 16                                   // {37}
    #define DO_MENU_HELP_CAN        17                                   // {38}
    #define DO_HELP_UP              18                                   // go up menu tree

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




#define DO_FTP_TELNET             12
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
    static void fnSetPortBit(unsigned short usBit, int iSetClr);
    static int  fnConfigOutputPort(CHAR cPortBit);
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
    {"9",                 "FTP/TELNET client commands",            DO_HELP,          DO_MENU_HELP_FTP_TELNET }, // {37}
    {"a",                 "CAN commands",                          DO_HELP,          DO_MENU_HELP_CAN }, // {38}
    {"help",              "Display menu specific help",            DO_HELP,          DO_MAIN_HELP },
    {"quit",              "Leave command mode",                    DO_TELNET,        DO_TELNET_QUIT },
};

static const DEBUG_COMMAND tLANCommand[] = {
    {"up",                "go to main menu",                       DO_HELP,          DO_HELP_UP },
#if defined ETH_INTERFACE || defined USB_CDC_RNDIS
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
    {"set_dhcp",          "<enable/disable> DHCP",                 DO_SERVER,        DO_ENABLE_DHCP },
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
//  {"set_user",          "Set output mode [0..3] [<d>|<u>]",      DO_HARDWARE,      DO_SET_USER_OUTPUT },
//  {"get_user",          "Get output mode [0..3]",                DO_HARDWARE,      DO_GET_USER_OUTPUT },
    {"set_ddr",           "Set port type [1..4] [<i>|<o>",         DO_HARDWARE,      DO_DDR },
    {"get_ddr",           "Get data direction [1..4]",             DO_HARDWARE,      DO_GET_DDR },
    {"read_port",         "Read port input [1..4]",                DO_HARDWARE,      DO_INPUT },
    {"write_port",        "Set port output [1..4] [0/1]",          DO_HARDWARE,      DO_OUTPUT },
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
#if defined USE_PARAMETER_BLOCK
    {"save",              "Save port setting as default",          DO_HARDWARE,      DO_SAVE_PORT },
#endif
    {"help",              "Display menu specific help",            DO_HELP,          DO_MAIN_HELP },
    {"quit",              "Leave command mode",                    DO_TELNET,        DO_TELNET_QUIT },
};

static const DEBUG_COMMAND tADMINCommand[] = {
    {"up",                "go to main menu",                       DO_HELP,          DO_HELP_UP },
#if defined ETH_INTERFACE || defined USB_CDC_RNDIS
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
#endif
    {"reset",             "Reset device",                          DO_HARDWARE,      DO_RESET },
    {"last_rst",          "Reset cause",                           DO_HARDWARE,      DO_LAST_RESET }, // {63}
    {"help",              "Display menu specific help",            DO_HELP,          DO_MAIN_HELP },
    {"quit",              "Leave command mode",                    DO_TELNET,        DO_TELNET_QUIT },
};

static const DEBUG_COMMAND tStatCommand[] = {
    {"up",                "go to main menu",                       DO_HELP,          DO_HELP_UP },
#if defined ETH_INTERFACE || defined USB_CDC_RNDIS
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
    #endif
    #if defined USE_USB_HID_KEYBOARD
    {"usb-kb",            "Keyboard input (disconnect to quit)",   DO_USB,           DO_USB_KEYBOARD }, // {77}
    #endif
    #if defined USE_USB_AUDIO
    {"delta",             "USB audio drift",                       DO_USB,           DO_USB_DELTA },
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
    #if defined TEST_I2C_INTERFACE
    {"wr",                "write [add] [value] {rep}",             DO_I2C,           DO_I2C_WRITE },
    {"rd",                "read  [add] [no. of bytes]",            DO_I2C,           DO_I2C_READ },
    {"srd",               "simple read [no. of bytes]",            DO_I2C,           DO_I2C_READ_NO_ADDRESS },
    {"rdq",               "read  [add] [no. of bytes] + wr",       DO_I2C,           DO_I2C_READ_PLUS_WRITE },
    #endif
    {"help",              "Display menu specific help",            DO_HELP,          DO_MAIN_HELP },
#endif
    {"quit",              "Leave command mode",                    DO_TELNET,        DO_TELNET_QUIT },
};

static const DEBUG_COMMAND tDiskCommand[] = {                            // {17}
    {"up",                "go to main menu",                       DO_HELP,          DO_HELP_UP },
#if defined SDCARD_SUPPORT || defined SPI_FLASH_FAT || defined FLASH_FAT || defined USB_MSD_HOST // {81}
    #if DISK_COUNT > 1
    {"disk",              "select disk [C/D/E]",                   DO_DISK,          DO_DISK_NUMBER },
    #endif
    {"info",              "utFAT/card info",                       DO_DISK,          DO_INFO },
    {"dir",               "[path] show directory content",         DO_DISK,          DO_DIR },
    #if defined UTFAT_UNDELETE || defined UTFAT_EXPERT_FUNCTIONS         // {60}
    {"dird",              "[path] show deleted directory content", DO_DISK,          DO_DIR_DELETED },
    #endif
    #if defined UTFAT_EXPERT_FUNCTIONS                                   // {60}
    {"dirh",              "[path] show hidden content",            DO_DISK,          DO_DIR_HIDDEN },
  //{"dirc",              "[path] show corrupted directory content", DO_DISK,        DO_DIR_CORRUPTED },
    {"infof",             "[path] show file info",                 DO_DISK,          DO_INFO_FILE },
    {"infod",             "[path] show deleted info",              DO_DISK,          DO_INFO_DELETED },
    #endif
    {"cd",                "[path] change dir. (.. for up)",        DO_DISK,          DO_CHANGE_DIR },
    #if defined UTFAT_WRITE                                              // {45}
    {"file" ,             "[path] new empty file",                 DO_DISK,          DO_NEW_FILE },
    {"write",             "[path] test write to file",             DO_DISK,          DO_WRITE_FILE },
    {"mkdir" ,            "new empty dir",                         DO_DISK,          DO_NEW_DIR },
    {"rename" ,           "[from] [to] rename",                    DO_DISK,          DO_RENAME },
    {"trunc" ,            "truncate to [length] [path]",           DO_DISK,          DO_TEST_TRUNCATE }, // {59}
        #if defined UTFAT_EXPERT_FUNCTIONS                               // {83}
    {"hide",              "[path] file/dir to hide",               DO_DISK,          DO_WRITE_HIDE },
    {"unhide",            "[path] file/dir to un-hide",            DO_DISK,          DO_WRITE_UNHIDE },
    {"prot",              "[path] file/dir to write-protect",      DO_DISK,          DO_SET_PROTECT },
    {"unprot",            "[path] file/dir to un-hide",            DO_DISK,          DO_REMOVE_PROTECT },
        #endif
    #endif
    #if !defined LOW_MEMORY
    {"print",             "[path] print file content",             DO_DISK,          DO_PRINT_FILE },
    #endif
  //{"root",              "set root dir",                          DO_DISK,          DO_ROOT }, {19}
    #if defined UTFAT_WRITE                                              // {45}
    {"del",               "[path] delete file or dir.",            DO_DISK,          DO_DELETE },
        #if defined UTFAT_SAFE_DELETE                                    // {60}
    {"dels",              "[path] safe delete file or dir.",       DO_DISK,          DO_DELETE_SAFE },
        #endif
        #if defined UTFAT_UNDELETE && defined UTFAT_WRITE                // {60}
    {"undel",             "undelete [name]",                       DO_DISK,          DO_UNDELETE },
        #endif
        #if defined UTFAT_FORMATTING
    {"format",            "[-16/12] [label] format (unformatted) disk",DO_DISK,      DO_FORMAT }, // {26}
        #endif
        #if defined UTFAT_FULL_FORMATTING
    {"fformat",           "[-16/12] [label] full format (unformatted) disk",DO_DISK, DO_FORMAT_FULL }, // {26}   
        #endif
        #if defined UTFAT_FORMATTING
    {"re-format",         "[-16/12] [label] reformat disk!!!!!",      DO_DISK,       DO_REFORMAT }, // {26} 
        #endif
        #if defined UTFAT_FULL_FORMATTING
    {"re-fformat",        "[-16/12] [label] full reformat disk!!!!!", DO_DISK,       DO_REFORMAT_FULL },// {26}
        #endif
    #endif
        #if !defined LOW_MEMORY
    {"sect",              "[hex no.] display sector",              DO_DISK,          DO_DISPLAY_SECTOR },
        #endif
    #if defined UTFAT_WRITE
        #if defined NAND_FLASH_FAT
    {"secti",             "[hex number] display sector info",      DO_DISK,          DO_DISPLAY_SECTOR_INFO },
    {"del-remap",         "delete remapping table!!",              DO_DISK,          DO_DELETE_REMAP_INFO },
    {"del-FAT",           "delete FAT",                            DO_DISK,          DO_DELETE_FAT },
    {"page",              "[hex number] display physical page",    DO_DISK,          DO_DISPLAY_PAGE },
            #if defined VERIFY_NAND
    {"tnand",             "write test patterns to NAND",           DO_DISK,          DO_TEST_NAND },
    {"vnand",             "verify test patterns in NAND",          DO_DISK,          DO_VERIFY_NAND },
            #endif
        #elif defined TEST_SDCARD_SECTOR_WRITE
    {"sectw",             "[hex no.] [patt.] [cnt]",               DO_DISK,          DO_WRITE_SECTOR }, // {44}
            #if defined UTFAT_MULTIPLE_BLOCK_WRITE
    {"sectmw",            "multi-block [dito]",                    DO_DISK,          DO_WRITE_MULTI_SECTOR },
                #if defined UTFAT_PRE_ERASE
    {"sectmwp",           "multi-block with pre-erase [dito]",     DO_DISK,          DO_WRITE_MULTI_SECTOR_PRE },
                #endif
            #endif
        #endif
    #endif
    {"help",              "Display menu specific help",            DO_HELP,          DO_MAIN_HELP },
#endif
    {"quit",              "Leave command mode",                    DO_TELNET,        DO_TELNET_QUIT },
};

static const DEBUG_COMMAND tFTP_TELNET_Command[] = {                     // {37}
    {"up",                "go to main menu",                       DO_HELP,          DO_HELP_UP },
#if defined USE_FTP_CLIENT
    {"show_config",       "Show FTP client settings",              DO_FTP_TELNET,    DO_SHOW_FTP_CONFIG },
    {"ftp_port",          "Set default FTP command port",          DO_FTP_TELNET,    DO_FTP_SET_PORT },
    {"ftp_ip",            "Set default FTP server IP",             DO_FTP_TELNET,    DO_FTP_SERVER_IP },
    {"ftp_user",          "Set default FTP user name",             DO_FTP_TELNET,    DO_FTP_USER_NAME },    
    {"ftp_pass",          "Set default FTP user password",         DO_FTP_TELNET,    DO_FTP_USER_PASS }, 
    {"ftp_psv",           "<enable/disable> passive mode",         DO_FTP_TELNET,    DO_FTP_PASSIVE },
    {"ftp_tout",          "Set FTP connection idle timeout",       DO_FTP_TELNET,    DO_FTP_SET_IDLE_TIMEOUT },

    {"ftp_con",           "Connect to FTP server",                 DO_FTP_TELNET,    DO_FTP_CONNECT },
    #if defined USE_IPV6                                                 // {48}
    {"ftp_con6",          "Connect to FTP server over IPv6",       DO_FTP_TELNET,    DO_FTP_CONNECT_IPV6 },
    #endif
    {"ftp_path",          "Set directory location",                DO_FTP_TELNET,    DO_FTP_PATH },
    {"ftp_dir",           "Directory listing [path]",              DO_FTP_TELNET,    DO_FTP_DIR },
    {"ftp_mkd",           "Make directory <path/dir>",             DO_FTP_TELNET,    DO_FTP_MKDIR },
    {"ftp_get",           "Get binary file <path/file>",           DO_FTP_TELNET,    DO_FTP_GET },
    {"ftp_get_a",         "Get ASCII file <path/file>",            DO_FTP_TELNET,    DO_FTP_GETA },
    {"ftp_put",           "Put binary file <path/file>",           DO_FTP_TELNET,    DO_FTP_PUT },
    {"ftp_put_a",         "Put ASCII file <path/file>",            DO_FTP_TELNET,    DO_FTP_PUTA },
    {"ftp_app",           "Append to binary file <path/file>",     DO_FTP_TELNET,    DO_FTP_APP },
    {"ftp_app_a",         "Append to ASCII file <path/file>",      DO_FTP_TELNET,    DO_FTP_APPA },
    {"ftp_ren",           "Rename file or dir. <path/dir> <path/dir>", DO_FTP_TELNET,DO_FTP_RENAME },
    {"ftp_del",           "Delete file <path/file>",               DO_FTP_TELNET,    DO_FTP_DEL },
    {"ftp_remove",        "Delete an empty dir. <path/dir>",       DO_FTP_TELNET,    DO_FTP_REMOVE_DIR },
    {"ftp_dis",           "Disconnect from FTP server",            DO_FTP_TELNET,    DO_FTP_DISCONNECT },
    #if defined USE_PARAMETER_BLOCK    
    {"save",              "Save configuration to FLASH",           DO_FLASH,         DO_SAVE_PARS },
    #endif
    #if !defined USE_TELNET_CLIENT
    {"help",              "Display menu specific help",            DO_HELP,          DO_MAIN_HELP },
    #endif
#endif
#if defined USE_TELNET_CLIENT                                            // {72}
    #if defined TELNET_CLIENT_COUNT && (TELNET_CLIENT_COUNT > 1)
    {"tel_int",           "Set TELNET interface [num]",            DO_FTP_TELNET,    DO_TELNET_SET_INTERFACE },
    #endif
    {"tel_port",          "Set TELNET [port]",                     DO_FTP_TELNET,    DO_TELNET_SET_PORT },
    {"tel_con",           "Connect to TELNET server [ip]",         DO_FTP_TELNET,    DO_TELNET_CONNECT },
    {"tel_echo",          "Set echo mode [1/0]",                   DO_FTP_TELNET,    DO_TELNET_SET_ECHO },
    {"tel_neg",           "Disable negotiation [1/0]",             DO_FTP_TELNET,    DO_TELNET_SET_NEGOTIATION },
    {"show_tel",          "Show TELNET config",                    DO_FTP_TELNET,    DO_TELNET_SHOW },
    #if !defined USE_FTP_CLIENT
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


// Special secret menu - not displayed as menu...
//
static const DEBUG_COMMAND tSecretCommands[] = {
    {"MAC",               0,                 DO_IP,            DO_SET_MAC },
};

static const MENUS ucMenus[] = {
    { tMainCommand,        15, (sizeof(tMainCommand) / sizeof(DEBUG_COMMAND)),        "     Main menu"},
    { tLANCommand,         20, (sizeof(tLANCommand) / sizeof(DEBUG_COMMAND)),         " LAN configuration"},
    { tSERIALCommand,      22, (sizeof(tSERIALCommand) / sizeof(DEBUG_COMMAND)),      "   Serial config."},
    { tIOCommand,          15, (sizeof(tIOCommand) / sizeof(DEBUG_COMMAND)),          " Input/Output menu"},
    { tADMINCommand,       17, (sizeof(tADMINCommand) / sizeof(DEBUG_COMMAND)),       "   Admin. menu"},
    { tStatCommand,        17, (sizeof(tStatCommand) / sizeof(DEBUG_COMMAND)),        "   Stats. menu"},
    { tUSBCommand,         13, (sizeof(tUSBCommand) / sizeof(DEBUG_COMMAND)),         "    USB menu"},
    { tI2CCommand,         13, (sizeof(tI2CCommand) / sizeof(DEBUG_COMMAND)),         "    I2C menu"},
    { tDiskCommand,        13, (sizeof(tDiskCommand) / sizeof(DEBUG_COMMAND)),        "  Disk interface"}, // {17}
    { tFTP_TELNET_Command, 15, (sizeof(tFTP_TELNET_Command) / sizeof(DEBUG_COMMAND)), "FTP/TELNET commands"}, // {37}
    { tCANCommand,         15, (sizeof(tCANCommand) / sizeof(DEBUG_COMMAND)),         "   CAN commands"}, // {38}
};

#if defined EZPORT_CLONER_SKIP_REGIONS
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
#if defined SERIAL_INTERFACE
    static unsigned char ucPasswordState = PASSWORD_IDLE;
#endif
#if defined SDCARD_SUPPORT || defined SPI_FLASH_FAT || defined FLASH_FAT || defined USB_MSD_HOST // {17}{81}
    static UTDIRECTORY *ptr_utDirectory[DISK_COUNT] = {0};               // pointer to a directory object
    static int iFATstalled = 0;                                          // stall flags when listing large directories and printing content
#endif
#if defined I2C_INTERFACE && defined TEST_I2C_INTERFACE
    extern QUEUE_HANDLE IICPortID;
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
        for (i = 0; i < TELNET_CLIENT_COUNT; i++) {
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
                if (iCloningActive & CLONING_ERASING_BULK) {
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

    while (fnRead(PortIDInternal, ucInputMessage, HEADER_LENGTH)) {      // check input queue
        switch (ucInputMessage[MSG_SOURCE_TASK]) {                       // switch depending on message source
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
                if (iMenuLocation) {
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
#if defined ETH_INTERFACE || defined USB_CDC_RNDIS
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
#if defined I2C_INTERFACE && defined TEST_I2C_INTERFACE
    if (fnMsgs(IICPortID) != 0) {                                        // if IIC message waiting
        QUEUE_TRANSFER iic_length = fnMsgs(IICPortID);
        int x = 0;
        fnDebugMsg("I2C Input = ");
        fnRead( IICPortID, ucInputMessage, iic_length);
        while (iic_length--) {
            fnDebugHex(ucInputMessage[x++], (WITH_LEADIN | WITH_SPACE | 1)); // display received bytes
        }
        fnDebugMsg("\r\n");
    }
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

    if (iStart) {
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

    while (iEntries--) {
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

    while (*ptrInput == ' ') {                                           // jump over white space
       ++(*ptrIn);
       ptrInput++;
    }

    while ((i--) && (*ptrInput >= '0')) {
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
    #if defined ETH_INTERFACE || defined USB_CDC_RNDIS
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
    #if defined ENC424J600_INTERFACE && (IP_INTERFACE_COUNT > 1)
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
    #if defined ENC424J600_INTERFACE && (IP_INTERFACE_COUNT > 1)
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
#define NUMBER_OF_SPEEDS 12
static const CHAR cSpeeds[NUMBER_OF_SPEEDS][7] = {
    {"300"},
    {"600"},
    {"1200"},
    {"2400"},
    {"4800"},
    {"9600"},
    {"14400"},
    {"19200"},
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
        if (!(uStrcmp((CHAR *)cSpeeds[i], ptr_input))) {
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
    if (temp_pars->temp_parameters.SerialMode & TWO_STOPS) {
        i = 2;
    }
    else if (temp_pars->temp_parameters.SerialMode & ONE_HALF_STOPS) {
        i = 1;
    }
    fnDebugMsg((CHAR *)cStops[i]);
    fnDebugMsg(" stop bit(s)\r\n");
}

static int fnGetStops(CHAR *ptr_input, UART_MODE_CONFIG *Mode)
{
    int i = 0;
    while (i < NUMBER_OF_STOPS) {
        if (!(uStrcmp((CHAR *)cStops[i], ptr_input))) {
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
    if (temp_pars->temp_parameters.SerialMode & RS232_EVEN_PARITY) {
        i = 1;
    }
    else if (temp_pars->temp_parameters.SerialMode & RS232_ODD_PARITY) {
        i = 0;
    }
    fnDebugMsg((CHAR *)cParity[i]);
    fnDebugMsg(" parity\r\n");
}

static int fnGetParity(CHAR *ptr_input, UART_MODE_CONFIG *Mode)
{
    int i = 0;
    while (i < NUMBER_OF_PARITY) {
        if (!(uStrcmp(cParity[i], ptr_input))) {
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
    if (temp_pars->temp_parameters.SerialMode & RTS_CTS) {
        i = 1;
    }
    else if (temp_pars->temp_parameters.SerialMode & USE_XON_OFF) {
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
        if (!(uStrcmp(cFlow[i], ptr_input))) {
            *Mode &= ~(USE_XON_OFF | RTS_CTS);                           // default no flow control
            switch (i) {
            case 1:                                                      // RTC/CTS
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

#if defined ETH_INTERFACE || defined USB_CDC_RNDIS
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
        if (!(fnGetSpeed(ptr_input, &temp_pars->temp_parameters.ucSerialSpeed))) {
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
        if (!(fnGetParity(ptr_input, &temp_pars->temp_parameters.SerialMode))) {
           fnDebugMsg("Incorrect parity\r\n");
           return;
        }
        iChangeSerial = 1;
        break;

    case SERIAL_SET_STOP:
        if (!(fnGetStops(ptr_input, &temp_pars->temp_parameters.SerialMode))) {
           fnDebugMsg("Incorrect stop bits\r\n");
           return;
        }
        iChangeSerial = 1;
        break;

    case SERIAL_SET_FLOW:
        if (!(fnSetFlowControl(ptr_input, &temp_pars->temp_parameters.SerialMode))) {
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

#if defined ETH_INTERFACE || defined USB_CDC_RNDIS
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
    fnShowServerEnabled("DHCP_SERVER", ACTIVE_DHCP);
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
#if defined ETH_INTERFACE || defined USB_CDC_RNDIS
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

#if defined ETH_INTERFACE || defined USB_CDC_RNDIS
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
#if defined ETH_INTERFACE || defined USB_CDC_RNDIS
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
#if defined ETH_INTERFACE || defined USB_CDC_RNDIS
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
#endif
    case DO_DISPLAY_MEMORY_USE:                                          // memory use display
        {
            STACK_REQUIREMENTS stackUsed;                                // {79}
            fnDebugMsg("\n\rSystem memory use:\n\r");
            fnDebugMsg(    "==================\n\r");
            fnDebugMsg("Free heap = ");
            fnDebugHex(fnHeapFree(), (2 | WITH_LEADIN));
            fnDebugMsg(" from ");
            fnDebugHex(fnHeapAvailable(), (2 | WITH_LEADIN));
            fnDebugMsg("\n\rUnused stack = ");
            fnDebugHex(fnStackFree(&stackUsed), (2 | WITH_LEADIN));      // {79}
            fnDebugMsg(" (");
            fnDebugHex(stackUsed, (2 | WITH_LEADIN));                    // {79}
            fnDebugMsg(")\n\r");
        }
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

//    case DO_GET_USER_OUTPUT:                                           // display whether default or user - removed {2}
//        if ((*ptrInput < '0') || (*ptrInput > '3')) {
//            fnDebugMsg("Bad port number\r\n");
//            break;
//        }
//        fnDebugMsg("\r\nPort use: ");
//        if (!fnPortInputConfig(*ptrInput)) {
//            fnDebugMsg("USER\r\n");
//        }
//        else {
//            fnDebugMsg("DEFAULT\r\n");
//        }
//        break;

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
              while (cPortBit < '5') {
                  if (fnPortState(cPortBit++)) {
                      ucPresentPort |= ucBit;
                  }
                  ucBit <<= 1;
              }
              ucBit = (unsigned char)fnHexStrHex(ptrInput);
              ucBit = (1 << (ucBit-1));
              if (fnHexStrHex(++ptrInput) == 0x01) {
                  ucPresentPort |= ucBit;
              }
              else {
                  ucPresentPort &= ~ucBit;
              }
              fnSetPortOut(ucPresentPort, 0);                            // set new port state
          }
          break;

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
          fnDebugMsg("Backligh intensity = ");
          fnDebugDec(temp_pars->temp_parameters.ucGLCDBacklightPWM, 0);
          fnDebugMsg("%\r\n");
          break;
#endif

//    case DO_SET_USER_OUTPUT:                                           // set either default or user - removed {2}
//        if (fnConfigPort(*ptrInput, *(ptrInput+2)) < 0) {
//            fnDebugMsg("Error in input\r\n");
//            return;
//        }
//        break;
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
              fnDebugMsg("\r\n??\r\n#");                                 // signal input not recognised and send prompt in command mode
              return;
          }
          break;
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
        #if !defined NO_FLASH_SUPPORT
            int iStorageAccess;
        #endif
            CAST_POINTER_ARITHMETIC ptrMemory = (CAST_POINTER_ARITHMETIC)fnHexStrHex(ptrInput); // get the address
            int iFillLength = 1;                                         // default
            if ((DO_MEM_FILL == ucType) || (DO_STORAGE_FILL == ucType)) {
                fnDebugMsg("Fill\r\n");
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
                fnWriteBytesFlash((unsigned char *)ptrMemory, ucPattern, SPI_FLASH_PAGE_LENGTH); // write the page to flash (this is intended for testing SPI flash onl)
                break;
            }
        #endif
            else {
                fnDebugMsg("Write\r\n");
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
                            fnWriteBytesFlash((unsigned char *)ptrMemory, &ucValue, BYTE_ACCESS);
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
                            fnWriteBytesFlash((unsigned char *)ptrMemory, (unsigned char *)&usValue, SHORT_WORD_ACCESS);
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
                            fnWriteBytesFlash((unsigned char *)ptrMemory, (unsigned char *)&ulValue, LONG_WORD_ACCESS);
                        }
                        else {
                            *(unsigned long *)ptrMemory = ulValue;
                        }
    #endif
                        break;
                    default:
                        break;
                    }
                    ptrMemory += iType;
                }
            }
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
    #endif
    #if defined USE_USB_HID_KEYBOARD
    case DO_USB_KEYBOARD:                                                // {77}
        usUSB_state |= ES_USB_KEYBOARD_MODE;                             // pass input to USB keyboard
        break;
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

#if defined I2C_INTERFACE
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
    #if defined TEST_I2C_INTERFACE
    case DO_I2C_WRITE:                                                   // write a value to a specified I2C address
        {
            QUEUE_TRANSFER length;
            unsigned char ucTest[257]; // = {IIC_WRITE_ADDRESS, 0, 0};
            ucTest[0] = IIC_WRITE_ADDRESS;
            ucTest[1] = (unsigned char)fnHexStrHex(ptrInput);            // the address
            if (fnJumpWhiteSpace(&ptrInput) != 0) {
                fnDebugMsg("Data missing\r\n");
                return;
            }
            ucTest[2] = (unsigned char)fnHexStrHex(ptrInput);            // the data
            if (fnJumpWhiteSpace(&ptrInput) == 0) {
                int iRepeat = 1;
                length = (unsigned char)fnDecStrHex(ptrInput);           // repetition length for data
                if (length == 0) {
                    length = 1;
                }
                while (iRepeat++ < length) {
                    ucTest[iRepeat + 1] = (ucTest[iRepeat] + 1);
                }
                length += 2;
            }
            else {
                length = 3;
            }
            fnWrite(IICPortID, ucTest, length);
            fnDebugMsg("Written value ");
            fnDebugHex(ucTest[2], (WITH_LEADIN | sizeof(ucTest[2])));
            fnDebugMsg(" to address ");
            fnDebugHex(ucTest[1], (WITH_LEADIN | WITH_CR_LF | sizeof(ucTest[1])));
        }
        break;

    case DO_I2C_READ_PLUS_WRITE:
    case DO_I2C_READ:                                                    // read values from an I2C address
        {
            unsigned char ucSetAddress[2];// = {IIC_WRITE_ADDRESS, 0};
            unsigned char ucTest[3];// = {1, IIC_READ_ADDRESS, OWN_TASK};
            ucSetAddress[0] = IIC_WRITE_ADDRESS;
            ucSetAddress[1] = (unsigned char)fnHexStrHex(ptrInput);      // the address
            fnWrite(IICPortID, (unsigned char *)ucSetAddress, sizeof(ucSetAddress));
            if (fnJumpWhiteSpace(&ptrInput) == 0) {
                ucTest[0] = (unsigned char)fnDecStrHex(ptrInput);        // the length               
            }
            else {
                ucTest[0] = 1;
            }
            ucTest[1] = IIC_READ_ADDRESS;
            ucTest[2] = OWN_TASK;
            fnRead(IICPortID, (unsigned char *)ucTest, 0);               // start the read
            if (DO_I2C_READ_PLUS_WRITE == ucType) {
                fnWrite(IICPortID, (unsigned char *)ucSetAddress, sizeof(ucSetAddress));
                fnDebugMsg("Reading (+ wr) ");
            }
            else {
                fnDebugMsg("Reading ");
            }
            fnDebugDec(ucTest[0], sizeof(ucTest[1]));
            fnDebugMsg(" bytes from address ");
            fnDebugHex(ucSetAddress[1], (WITH_LEADIN | WITH_CR_LF | sizeof(ucSetAddress[1])));
        }
        break;

    case DO_I2C_READ_NO_ADDRESS:
        {
            unsigned char ucTest[3];// = {1, IIC_READ_ADDRESS, OWN_TASK};
            ucTest[0] = (unsigned char)fnDecStrHex(ptrInput);            // the length
            if (ucTest[0] == 0) {
                fnDebugMsg("Invalid length\n\r");
                break;
            }
            ucTest[1] = IIC_READ_ADDRESS;
            ucTest[2] = OWN_TASK;
            fnRead(IICPortID, (unsigned char *)ucTest, 0);               // start the read
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
            while (1) {
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
                } while (1);
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
            unsigned long ulBuffer[512/sizeof(unsigned long)];
            fnDebugMsg("Reading sector ");
    #endif
            fnDebugHex(ulSectorNumber, (WITH_LEADIN | sizeof(ulSectorNumber)));
    #if (defined _WINDOWS || defined _LITTLE_ENDIAN) && defined UTFAT_SECT_BIG_ENDIAN // {61}
            fnDebugMsg(" (big-endian view)");
    #elif (!defined _LITTLE_ENDIAN) && defined UTFAT_SECT_LITTLE_ENDIAN
            fnDebugMsg(" (little-endian view)");
    #endif
            if (fnReadSector(ucPresentDisk, (unsigned char *)ulBuffer, ulSectorNumber) != 0) {
                fnDebugMsg(" FAILED!!\r\n");
                break;
            }
            fnDebugMsg("\r\n");
            for (i = 0; i < 8; i++) {
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
    case DO_WRITE_SECTOR:
        {
            unsigned long ulSectorNumber = fnHexStrHex(ptrInput);        // the sector number
            int i;
            unsigned char ucPattern = 0x00;
            unsigned char ucTestPattern[512];
            unsigned char ucSectorCount = 1;
            if (fnJumpWhiteSpace(&ptrInput) == 0) {
                ucPattern = (unsigned char)fnHexStrHex(ptrInput);
            }
            for (i = 0; i < sizeof(ucTestPattern); i++) {                // generate the test pattern starting with the defined value
                ucTestPattern[i] = ucPattern++;
            }
            fnDebugMsg("Writing sector(s) ");
            fnDebugHex(ulSectorNumber, (WITH_LEADIN | sizeof(ulSectorNumber)));
            TOGGLE_TEST_OUTPUT();                                        // enable measurement of the write time
            if (fnJumpWhiteSpace(&ptrInput) == 0) {
                ucSectorCount = (unsigned char)fnHexStrHex(ptrInput);
                if (ucSectorCount == 0) {
                    ucSectorCount = 1;
                }
            }
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
else {
//fnPrepareBlockWrite(ucPresentDisk, 0, 0); // test abort
}
            do {
                if (fnWriteSector(ucPresentDisk, ucTestPattern, ulSectorNumber) != UTFAT_SUCCESS) {
                    fnDebugMsg(" Sector write error!\n\r");
                }
                else {
                    fnDebugMsg(" - OK\n\r");                             // sector write successful
                }
                ulSectorNumber++;
            } while (--ucSectorCount != 0);
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
            if (ptrDiskInfo->usDiskFlags & DISK_NOT_PRESENT) {           // {24}{49} no card detected
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
    if (iFTP_data_state & FTP_DATA_STATE_PAUSE) {
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
    if (ptrClientMessageBox->iCallbackEvent & FTP_CLIENT_ERROR_FLAG) {   // error event code
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

#if defined USE_FTP_CLIENT || defined USE_TELNET_CLIENT                  // {72}
static void fnDoFTP_TELNET(unsigned char ucType, CHAR *ptrInput)
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
    while (1) {                                                          // extract first an optional ID from 0 -- 7ff or extended from 0 -- 1fffffff, followed by data
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
        else if (DO_MENU_HELP_FTP_TELNET == ucType) {                    // {37}
            ucMenu = MENU_HELP_FTP;                                      // set FTP/TELNET client menu
            return (fnDisplayHelp(0));                                   // large menu may require special handling
        }
        else if (DO_MENU_HELP_CAN == ucType) {                           // {38}
            ucMenu = MENU_HELP_CAN;                                      // set CAN menu
            return (fnDisplayHelp(0));                                   // large menu may require special handling
        }
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
#if defined USE_FTP_CLIENT || defined USE_TELNET_CLIENT                  // {37}{72}
    case DO_FTP_TELNET:                                                  // FTP/TELNET client group
        fnDoFTP_TELNET(ucType, ptrInput);
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
            return 0;                                                    // we can not treat commands
        }
    }
#if defined USE_USB_CDC                                                  // {8}
    else if (iSource == SOURCE_USB) {
        if (usUSB_state == ES_NO_CONNECTION) {
            return 0;                                                    // we can not treat commands
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
    switch (ucTestCase) {
    case 0x01:                                                           // we should send data blocks as fast as possible
        {
            int i;
            int iSent = 0;
            unsigned char test_buffer[26];
            for (i = 0; i < 26; i++) {
                test_buffer[i] = 'a' + i;
            }
            while (fnSendBufTCP(Socket, 0, 26, TCP_BUF_CHECK)) {         // while space in output buffer              
                iSent += fnSendBufTCP(Socket, test_buffer, 26, (TCP_BUF_SEND | TCP_BUF_SEND_REPORT_COPY));
            }
            return iSent;
        }
        break;
    }
    return 0;
}

static int fnServerTestListener(USOCKET Socket, unsigned char ucEvent, unsigned char *ucIp_Data, unsigned short usPortLen)
{
    static unsigned char ucTestMode = 0;
    switch (ucEvent) {
    case TCP_EVENT_CONREQ:                                               // session request received on the TCP port
        fnDebugMsg("Connection request\n\r");
        break;
    case TCP_EVENT_CONNECTED:
        fnDebugMsg("Connection established\n\r");
        break;
    case TCP_EVENT_DATA:
        if (ucTestMode == 0) {                                           // the first data indicates the test mode
            ucTestMode = *ucIp_Data;
            if (fnTestTCP(Socket, ucTestMode) > 0) {
                return APP_SENT_DATA;
            }
        }
        break;
    case TCP_EVENT_ACK:
        if (fnTestTCP(Socket, ucTestMode) > 0) {
            return APP_SENT_DATA;
        }
        break;
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
#if defined USE_USB_HID_KEYBOARD
    if (usUSB_state & ES_USB_KEYBOARD_MODE) {                            // {77} if the input is connected to the USB keyboard connection
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
    if (iFTP_data_state & (FTP_DATA_STATE_GETTING | FTP_DATA_STATE_PUTTING)) { // if getting or putting a file
    #if defined FTP_CLIENT_BUFFERED_SOCKET_MODE
        unsigned short usLengthToSend = usLen;                           // backup the buffer length
    #endif
        while (usLen--) {
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
                if (iFTP_data_state & FTP_DATA_STATE_PUTTING) {          // input is sent to FTP server when putting
                    if (iFTP_data_state & FTP_DATA_STATE_SENDING) {
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

    while (usLen--) {
        if (*ptrData == DELETE_KEY) {
            if (ucDebugCnt) {
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
                while (iOriginalLength--) {
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
        break;
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
#if defined USE_USB_HID_KEYBOARD
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
static void fnSetPortBit(unsigned short usBit, int iSetClr);
static int  fnConfigOutputPort(CHAR cPortBit);
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
        test_server_socket = fnStartTelnet(0x1234, (2 * 60), 0, 0, fnServerTestListener);
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
    if (iActions & CHANGE_SERIAL_SETTINGS) {
        fnSetNewSerialMode(MODIFY_CONFIG);                               // return settings to interface
    }
    #endif
    if (CHANGE_WEB_SERVER & iActions) {
    #if defined USE_HTTP
        fnConfigureAndStartWebServer();
    #endif
    }
    if (CHANGE_FTP_SERVER & iActions) {
        fnConfigureFtpServer(FTP_TIMEOUT);                               // {3}
    }
    #if defined USE_TELNET
    if (CHANGE_TELNET_SERVER & iActions) {
        fnConfigureTelnetServer();
    }
    #endif
}
#endif

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
#if defined KL43Z_256_32_CL
                if (file_header->ulCodeLength >= (SIZE_OF_FLASH))
#else
                if (file_header->ulCodeLength >= (SIZE_OF_FLASH/2))
#endif
                {                                                        // simple check that the length is valid
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
        uTaskerMonoTimer(TASK_APPLICATION, (DELAY_LIMIT)(2*SEC), E_TIMER_SW_DELAYED_RESET);
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

#if defined USE_PARAMETER_BLOCK
// When this routine is called, the parameter block is set to a new valid block and the old block is deleted
//
extern int fnSaveNewPars(int iTemp)
{
    #if defined ETH_INTERFACE || defined USB_CDC_RNDIS
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
    #if defined ETH_INTERFACE || defined USB_CDC_RNDIS
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
    if (temp_pars->temp_parameters.usServers[DEFAULT_NETWORK] & ACTIVE_FTP_SERVER) { // should the FTP server be started?
        unsigned char ucFTP_mode = 0;
        if (temp_pars->temp_parameters.usServers[DEFAULT_NETWORK] & ACTIVE_FTP_LOGIN) {
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


#include "debug_hal.h"                                                   // {53}
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
