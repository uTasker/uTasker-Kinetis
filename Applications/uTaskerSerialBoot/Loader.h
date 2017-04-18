/***********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher

    ---------------------------------------------------------------------
    File:      Loader.h
    Project:   uTasker Demonstration project
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2016
    *********************************************************************
    15.09.2009 Add STR91XF setup
    03.01.2010 Add SAM7X setup
    26.03.2010 LPC2XXX application start set to 8k and add intermediate buffer {1}
    05.10.2010 Add optional SHOW_APP_DETAILS                             {2}
    01.12.2010 Add RX6XX                                                 {3}
    02.04.0211 Add Kinetis                                               {4}
    14.05.2011 Add USB for Coldfire                                      {5}
    31.05.2011 Add USB for SAM7X                                         {6}
    03.06.2011 Activate USB_MSD password protection for M522xx and Kinetis {7}
    22.01.2012 Add Kinetis SD card loader configuration                  {8}
    11.02.2012 Add M522xx SD card loader configuration                   {9}
    12.02.2012 Add STM32                                                 {10}
    18.02.2012 Add LPC2xxx SD card configuration                         {11}
    20.04.2012 Add TWR_K20D50M                                           {12}
    03.03.2013 Add fnAddSREC_file()                                      {13}
    25.11.2013 Add MAX_WAIT_SD_CARD                                      {14}
    16.01.2014 Add FRDM_K20D50M, FRDM_KL46Z and TWR_K70F120M             {15}
    28.01.2014 Add TWR_KL46Z48M                                          {16}
    29.01.2014 Add FRDM_KL25Z, FRDM_KL26Z and TWR_KL25Z48M               {17}
    01.02.2014 Add FRDM_KL02Z, FRDM_KL05Z and FRDM_KE02Z                 {18}
    15.04.2014 Add FRDM_K64F and TWR_K64F120M                            {19}
    15.04.2014 Add web server based software upload support              {20}
    15.05.2014 Add optional SD card wildcard file name matching          {21}
    15.05.2014 Add optional decryption of SD card content                {22}
    12.07.2014 Add FRDM_KE02Z40M, FRDM_KE04Z and FRDM_KE06Z              {23}
    12.07.2014 Add kboot global defines, which are shared by HID and UART modes {24}
    21.01.2015 Modify fnAddSREC_file() to pass additional information    {25}
    20.10.2015 Add fnJumpToValidApplication();                           {26}

*/

#define SOFTWARE_VERSION              "V1.3"

#define MY_PROJECT_NAME               "uTasker loader project"

#if defined USB_MSD_HOST_LOADER
    #define MAX_WAIT_SD_CARD          3
    #define MEMORY_STICK_TASK         TASK_SD_LOADER                     // if the memory stick sucessfully mounts, this task will be informed by an event
#else
    #define MAX_WAIT_SD_CARD          3                                  // {14} if no SD card is not detected after 4s the existing application is started
#endif

#define WILDCARD_FILES                                                   // {21} allow wildcard name matching to be used
#if defined WILDCARD_FILES
    #undef _REMOVE_FORMATTED_OUTPUT
#endif
//#define ENCRYPTED_CARD_CONTENT                                         // {22}


#if !defined FRDM_KE04Z
    #define SHOW_APP_DETAILS                                             // {2} display application start and size in menu
#endif

#if defined _M5223X
    #define SERIAL_SPEED              SERIAL_BAUD_115200                 // the Baud rate of the UART
    #ifdef _M5225X
        #ifdef USB_INTERFACE
            #define UTASKER_APP_START (24 * 1024)                        // application starts at this address        
            #define UTASKER_APP_END   (unsigned char *)(UTASKER_APP_START + (78  *1024)) // end of application space - after maximum application size                        
        #else
            #define UTASKER_APP_START (12 * 1024)                        // application starts at this address
            #define UTASKER_APP_END   (unsigned char *)(UTASKER_APP_START + (90 * 1024)) // end of application space - after maximum application size            
        #endif
    #else
        #define UTASKER_APP_START     (10 * 1024)                        // application starts at this address
        #define UTASKER_APP_END       (unsigned char *)(UTASKER_APP_START + (246 * 1024)) // end of application space - after maximum application size
    #endif
    #define ROOT_FILE_ENTRIES         4                                  // {5} when USB MSD loader this many directory entries are set to the start of FLASH - the application start is shifted by this amount x 32 bytes
    #define ENABLE_READBACK                                              // allow USB to transfer present application to PC
    #define MASS_ERASE                                                   // support a mass-erase command. This is used together with a protected FLASH configuration.
                                                                         // when the FLASH is protected, downloads are still possible but the debug interface is blocked.
                                                                         // this allows a commanded delete of the complete FLASH content (including serial loader) to unblock the
                                                                         // debug interface
    // Before software can be read from the disk a password file must have been copied {7}
    //
    #define READ_PASSWORD             "enable file read from the M522xx device by dragging this file to the disk" // password with maximum length of 512 bytes

    // SD card loading - file to be loaded, magic number and secret key for authenticating the file's content {9}
    //
    #define NEW_SOFTWARE_FILE            "software.bin"
    #define VALID_VERSION_MAGIC_NUMBER   0x1234
    #define _SECRET_KEY                  {0xa7, 0x48, 0xb6, 0x53, 0x11, 0x24}
#elif defined _KINETIS                                                   // {4}
    #if defined FRDM_KL02Z || defined FRDM_KL05Z
        #define SERIAL_SPEED          SERIAL_BAUD_57600                  // the Baud rate of the UART
    #elif defined FRDM_KL03Z || defined FRDM_KE04Z
        #define SERIAL_SPEED          SERIAL_BAUD_19200                  // the Baud rate of the UART (there is a 100nF capacitor on the Rx input on this board so a slow Baud rate is needed)
    #elif defined FRDM_KE02Z || defined FRDM_KE02Z40M || defined TWR_KW21D256
        #define SERIAL_SPEED          SERIAL_BAUD_38400                  // the Baud rate of the UART
    #else
        #define SERIAL_SPEED          SERIAL_BAUD_115200                 // the Baud rate of the UART
    #endif
    #define ROOT_FILE_ENTRIES         4                                  // when USB MSD loader this many directory entries are set to the start of FLASH - the application start is shifted by this amount x 32 bytes
    #define ENABLE_READBACK                                              // allow USB to transfer present application to PC
    #if defined TEENSY_LC || defined FRDM_KL27Z
        #if defined SPECIAL_VERSION
            #define UTASKER_APP_START (0x4000)                        // 14k application starts at this address
            #define UTASKER_APP_END   (unsigned char *)(UTASKER_APP_START + (48 * 1024) - (ROOT_FILE_ENTRIES * 32)) // end of application space - after maximum application size
        #else
            #define UTASKER_APP_START (24 * 1024)                        // application starts at this address
            #define UTASKER_APP_END   (unsigned char *)(UTASKER_APP_START + (38 * 1024)) // end of application space - after maximum application size
        #endif
        #define INTERMEDIATE_PROG_BUFFER  (1 * 1024)                     // when UART speed greater than 57600 Baud is used an intermediate buffer is recommended
    #elif defined FRDM_KL25Z || defined FRDM_KL26Z || defined tinyK20 || defined TWR_KL25Z48M || defined FRDM_KE06Z || defined TRK_KEA128 || defined FRDM_K20D50M // {17}
        #define UTASKER_APP_START     (32 * 1024)                        // application starts at this address
        #define UTASKER_APP_END       (unsigned char *)(UTASKER_APP_START + (64 * 1024)) // end of application space - after maximum application size
        #define INTERMEDIATE_PROG_BUFFER  (2 * 1024)                     // when UART speed greater than 57600 Baud is used an intermediate buffer is recommended
    #elif defined TWR_K20D50M ||  defined FRDM_KL46Z || defined FRDM_KL43Z || defined TWR_KL43Z48M || defined TWR_KL46Z48M || defined TEENSY_3_1 || defined TWR_K24F120M // {16}
        #if defined FLEXFLASH_DATA
            #define DISK_D_LOCATION   (SIZE_OF_FLASH - SIZE_OF_FLEXFLASH)// locate a second hard drive in flex flash memory at a virtual location just after internal flash
            #define DISK_D_END        (unsigned char *)(SIZE_OF_FLASH);  // the second hard drive ends after the flex memory
        #endif
        #define UTASKER_APP_START     (32 * 1024)                        // application starts at this address
        #define INTERMEDIATE_PROG_BUFFER  (8 * 1024)                     // when UART speed greater than 57600 Baud is used an intermediate buffer is recommended
        #define UTASKER_APP_END       (unsigned char *)(UTASKER_APP_START + (100 * 1024)) // end of application space - after maximum application size
    #elif defined FRDM_KL02Z || defined FRDM_KL03Z || defined FRDM_KL05Z || defined FRDM_KE02Z || defined FRDM_KE02Z40M || defined TWR_KV10Z32 || defined TWR_KV31F120M || defined TRK_KEA64 // {18}
        #define UTASKER_APP_START     (10 * 1024)                        // application starts at this address
        #if defined TWR_KV31F120M
            #define UTASKER_APP_END   (unsigned char *)(UTASKER_APP_START + (48 * 1024)) // end of application space - after maximum application size
        #else
            #define UTASKER_APP_END   (unsigned char *)(UTASKER_APP_START + (22 * 1024)) // end of application space - after maximum application size
        #endif
    #else
        #if defined FRDM_K64F && defined MEMORY_SWAP
            #define UTASKER_APP_START     (SIZE_OF_FLASH/2)              // second half of flash memory is used by the next application
        #else
            #define UTASKER_APP_START     (32 * 1024)                    // application starts at this address
        #endif
      //#define INTERMEDIATE_PROG_BUFFER  (8 * 1024)                     // when UART speed greater than 57600 Baud is used an intermediate buffer is recommended
        #define UTASKER_APP_END           (unsigned char *)(UTASKER_APP_START + (100 * 1024)) // end of application space - after maximum application size
    #endif
    #if !defined TEENSY_3_1 && !defined TEENSY_LC                        // warning: do not use mass erase with Teensy devices since their loader doesn't support the completely erased state and requires an external loader to recoved to the unsecured flash state
        #define MASS_ERASE                                               // support a mass-erase command. This is used together with a protected FLASH configuration.
                                                                         // when the FLASH is protected, downloads are still possible but the debug interface is blocked.
                                                                         // this allows a commanded delete of the complete FLASH content (including serial loader) to unblock the
                                                                         // debug interface
    #endif
    // Before software can be read from the disk a password file must have been copied {7}
    //
  //#define READ_PASSWORD             "enable file read from the Kinetis device by dragging this file to the disk" // password with maximum length of 512 bytes

    // SD card loading - file to be loaded, magic number and secret key for authenticating the file's content {8}
    //
    #if defined ENCRYPTED_CARD_CONTENT
        #define NEW_SOFTWARE_FILE     "sd_card_enc.bin"
        #define VALID_VERSION_MAGIC_NUMBER   0x1235
        #define _SECRET_KEY           {0xb7, 0x48, 0xb6, 0x53, 0x11, 0x24}
        static const unsigned char ucDecrypt[] = {0xff, 0x25, 0xa7, 0x88, 0xf2, 0xe6, 0x81, 0x33, 0x87, 0x77}; // must be even in length (dividable by unsigned short)
        #define KEY_PRIME             0xafe1                             // never set to 0
        #define CODE_OFFSET           0xc298                             // ensure that this value is a multiple of the smallest flash programming entity size (divisible by 8 is suitable for all Kinetis parts)
    #else
        #define NEW_SOFTWARE_FILE     "XC_Tracer_*.bin"//"software.bin"
        #define VALID_VERSION_MAGIC_NUMBER   0x1234
        #define _SECRET_KEY           {0xa7, 0x48, 0xb6, 0x53, 0x11, 0x24}
    #endif
#elif defined _LPC23XX
    #define SERIAL_SPEED              SERIAL_BAUD_115200                 // the Baud rate of the UART
    #if defined USB_INTERFACE || defined SDCARD_SUPPORT || defined SPI_FLASH_FAT
        #define UTASKER_APP_START     (FLASH_START_ADDRESS + (20 * 1024))// application starts at this address
    #else
        #define UTASKER_APP_START     (FLASH_START_ADDRESS + (8 * 1024)) // {1} application starts at this address
    #endif
    #define UTASKER_APP_END           (unsigned char *)(UTASKER_APP_START + (45 * 1024)) // end of application space - after maximum application size
    #define INTERMEDIATE_PROG_BUFFER  (16 * 1024)                        // {1} use an intermediate buffer and flow control to avoid character loss during FLASH writes
    #ifdef _GNU
        #define _GNU_TEMP_WORKAROUND                                     // activate a GCC workaround to allow this to operate with intermediate buffer
    #endif
    #define ROOT_FILE_ENTRIES         4                                  // when USB MSD loader this many directory entries are set to the start of FLASH - the application start is shifted by this amount x 32 bytes
    #define ENABLE_READBACK                                              // allow USB to transfer present application to PC
    // Before software can be read from the disk a password file must have been copied {7}
    //
    #define READ_PASSWORD             "enable file read from the LPC2xxx device by dragging this file to the disk" // password with maximum length of 512 bytes

    // SD card loading - file to be loaded, magic number and secret key for authenticating the file's content {11}
    //
    #define NEW_SOFTWARE_FILE            "software.bin"
    #define VALID_VERSION_MAGIC_NUMBER   0x22ba
    #define _SECRET_KEY                  {0xb2, 0x67, 0xa8, 0x20, 0x09, 0x26}
#elif defined _LPC17XX
    #define SERIAL_SPEED              SERIAL_BAUD_115200                 // the Baud rate of the UART
    #define UTASKER_APP_START         (FLASH_START_ADDRESS + (8 * 1024)) // application starts at this address
    #define UTASKER_APP_END           (unsigned char *)(UTASKER_APP_START + (45 * 1024)) // end of application space - after maximum application size
    #define INTERMEDIATE_PROG_BUFFER  (16 * 1024)                        // use an intermediate buffer and flow control to avoid character loss during FLASH writes
    #ifdef _GNU
        #define _GNU_TEMP_WORKAROUND                                     // activate a GCC workaround to allow this to operate with intermediate buffer
    #endif
#elif defined _HW_AVR32
    #define SERIAL_SPEED              SERIAL_BAUD_115200                 // the Baud rate of the UART
    #define UTASKER_APP_START         (FLASH_START_ADDRESS + (10 * 1024))// application starts at this address
    #define UTASKER_APP_END           (unsigned char *)(UTASKER_APP_START + (80 * 1024)) // end of application space - after maximum application size
    #define INTERMEDIATE_PROG_BUFFER  (16 * 1024)                        // use an intermediate buffer and flow control to avoid character loss during FLASH writes
    #ifdef _GNU
        #define _GNU_TEMP_WORKAROUND                                     // activate a GCC workaround to allow this to operate with intermediate buffer
    #endif
#elif defined _RX6XX                                                     // {3}
    #undef LOADER_UART
    #ifdef RX62N_EVB
        #define LOADER_UART           2
    #else
        #define LOADER_UART           2
        #define UART2_B
    #endif
    #define SERIAL_SPEED              SERIAL_BAUD_115200                 // the Baud rate of the UART
    #define UTASKER_APP_START         (FLASH_START_ADDRESS + (16 * 1024))// application starts at this address
    #define UTASKER_APP_END           (unsigned char *)(UTASKER_APP_START + (80 * 1024)) // end of application space - after maximum application size
    #define INTERMEDIATE_PROG_BUFFER  (45 * 1024)                        // use an intermediate buffer and flow control to avoid character loss during FLASH writes
    #ifdef _GNU
        #define _GNU_TEMP_WORKAROUND                                     // activate a GCC workaround to allow this to operate with intermediate buffer
    #endif
#elif defined _STR91XF
    #define SERIAL_SPEED              SERIAL_BAUD_57600                  // the Baud rate of the UART
    #define UTASKER_APP_START         (0x80000)                          // application starts at this address
    #define UTASKER_APP_END           (unsigned char *)(UTASKER_APP_START + (64 * 1024)) // end of application space - after maximum application size
#elif defined _HW_SAM7X
    #define SERIAL_SPEED              SERIAL_BAUD_115200                 // the Baud rate of the UART
    #ifdef USB_INTERFACE
        #define UTASKER_APP_START     (FLASH_START_ADDRESS + (20 * 1024))// application starts at this address
    #else
        #define UTASKER_APP_START     (FLASH_START_ADDRESS + (10 * 1024))// application starts at this address
    #endif
    #define UTASKER_APP_END           (unsigned char *)(UTASKER_APP_START + (48 * 1024)) // end of application space - after maximum application size
    #define ROOT_FILE_ENTRIES         4                                  // {6} when USB MSD loader this many directory entries are set to the start of FLASH - the application start is shifted by this amount x 32 bytes
    #define ENABLE_READBACK                                              // allow USB to transfer present application to PC
    // Before software can be read from the disk a password file must have been copied
    //
    #define READ_PASSWORD             "enable file read from the SAM7 device by dragging this file to the disk" // password with maximum length of 512 bytes
  //#define DELETE_BLOCK              (12 * 1024)                        // perform program delete in maximum blocks, winth watchdog retrigger in-between
#elif defined _STM32                                                     // {10}
    #define SERIAL_SPEED              SERIAL_BAUD_115200                 // the Baud rate of the UART
    #define ROOT_FILE_ENTRIES         4                                  // when USB MSD loader this many directory entries are set to the start of FLASH - the application start is shifted by this amount x 32 bytes
    #define ENABLE_READBACK                                              // allow USB to transfer present application to PC
    #define UTASKER_APP_START         (FLASH_START_ADDRESS + (16 * 1024))// application starts at this address
    #define UTASKER_APP_END           (unsigned char *)(UTASKER_APP_START + (130 * 1024)) // end of application space - after maximum application size

    // Before software can be read from the disk a password file must have been copied
    //
    #define READ_PASSWORD             "enable file read from the Kinetis device by dragging this file to the disk" // password with maximum length of 512 bytes

    // SD card loading - file to be loaded, magic number and secret key for authenticating the file's content
    //
    #define NEW_SOFTWARE_FILE            "software.bin"
    #define VALID_VERSION_MAGIC_NUMBER   0x651c
    #define _SECRET_KEY                  {0x9c, 0x66, 0x9a, 0x00, 0x8f, 0x71}
#else                                                                    // Luminary
    #define SERIAL_SPEED              SERIAL_BAUD_115200                 // the Baud rate of the UART
    #define UTASKER_APP_START         (12 * 1024)                        // application starts at this address
    #define UTASKER_APP_END           (unsigned char *)(UTASKER_APP_START + (116 * 1024)) // end of application space - after maximum application size
    #define UPLOAD_OFFSET             (1952 * SPI_FLASH_PAGE_LENGTH)     // offset to image area in SPI Flash (when used)
#endif

#if defined MEMORY_SWAP
    #define _UTASKER_APP_START_       (FLASH_START_ADDRESS)
#elif defined USB_INTERFACE && defined USB_MSD_DEVICE_LOADER
    #define _UTASKER_APP_START_       (UTASKER_APP_START + (ROOT_FILE_ENTRIES * 32)) // when USB is used the start of application space is used for FAT entries
#else
    #define _UTASKER_APP_START_       (UTASKER_APP_START)
#endif

#if !defined UPLOAD_OFFSET
    #define UPLOAD_OFFSET 0
#endif

#define POINTER_USER_NAME "ADMIN"                                        // {20} optional web server authentication
#define POINTER_USER_PASS "admin"

#define MAX_UTASKER_APP_SIZE      (MAX_FILE_LENGTH)(UTASKER_APP_END - (unsigned char *)_UTASKER_APP_START_)

#if defined USE_TFTP
    #define TFTP_ERROR_MESSAGE     "uTasker TFTP Error"                  // fixed TFTP error test
#endif

extern void fnConfigureAndStartWebServer(void);
extern void fnTransferTFTP(void);

#define T_RESET                   1                                      // application timer events
#define T_GO_TO_APP               2
#define T_COMMIT_BUFFER           3
#define T_MESSAGE_TIMEOUT         4
#define T_HOOKUP_TIMEOUT          4

// USB to mass storage
//
#define MOUNT_USB_MSD             1
#define USB_MSD_REMOVED           2

// Mass storage to its application task
//
#define MEM_STICK_MOUNTED         0xff

typedef struct stFILE_OBJECT_INFO
{
    unsigned char  *ptrLastAddress;
    unsigned short usCreationDate;
    unsigned short usCreationTime;
    const CHAR     *ptrShortFileName;
} FILE_OBJECT_INFO;

extern int fnAddSREC_file(FILE_OBJECT_INFO *ptrFileObjectInfo);          // {13}{25}
extern unsigned short fnCRC16(unsigned short usCRC, unsigned char *ptrInput, unsigned long ulBlockSize);
extern void fnJumpToValidApplication(int iResetPeripherals);             // {26}
#if defined MEMORY_SWAP
    extern void fnHandleSwap(int iCheck);
#endif

#if defined USE_USB_CDC
    extern QUEUE_HANDLE USBPortID_comms;                                 // USB CDC handle
#endif

// Global KBOOT defines, shared by HID and UART modes                    {24}
//
typedef struct stKBOOT_PACKET
{
    unsigned char ucCommandType;
    unsigned char ucDummy;
    unsigned char ucLength[2];
    unsigned char ucData[32];
} KBOOT_PACKET;

extern int fnHandleKboot(QUEUE_HANDLE USBPortID_coms, int iInterfaceType, KBOOT_PACKET *ptrKBOOT_packet);
  #define KBOOT_UART 0
  #define KBOOT_HID  1

