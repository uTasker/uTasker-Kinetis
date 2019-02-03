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
    Copyright (C) M.J.Butcher Consulting 2004..2019
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
    03.08.2017 Add USB-MSD iHex/SREC content support                     {27}
    05.10.2017 Add modbus loading                                        {28}
    17.01.2018 Add I2C slave loading                                     {29}

*/

#define SOFTWARE_VERSION              "V1.5"

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

#define OUR_SLAVE_ADDRESS             0x50                               // {29} I2C slave address

#if defined _M5223X
    #define SERIAL_SPEED              SERIAL_BAUD_115200                 // the Baud rate of the UART
    #if defined _M5225X
        #if defined USB_INTERFACE
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
        #define SERIAL_SPEED          SERIAL_BAUD_38400                  // the Baud rate of the UART
    #elif defined FRDM_KL03Z || defined FRDM_KE04Z
        #define SERIAL_SPEED          SERIAL_BAUD_19200                  // the Baud rate of the UART (there is a 100nF capacitor on the Rx input on this board so a slow Baud rate is needed)
    #elif defined FRDM_KE02Z || defined FRDM_KE02Z40M || defined TWR_KW21D256 || defined FRDM_KEAZ128Q80 || defined FRDM_KEAZ64Q64 || defined FRDM_KEAZN32Q64
        #define SERIAL_SPEED          SERIAL_BAUD_38400                  // the Baud rate of the UART
    #else
        #define SERIAL_SPEED          SERIAL_BAUD_115200                 // the Baud rate of the UART
    #endif
    #define ROOT_FILE_ENTRIES         4                                  // when USB MSD loader this many directory entries are set to the start of FLASH - the application start is shifted by this amount x 32 bytes
    #define ENABLE_READBACK                                              // allow USB to transfer present application to PC
    #if defined TEENSY_LC || defined FRDM_KL27Z
        #if defined _DEV2
            #undef ROOT_FILE_ENTRIES
            #define ROOT_FILE_ENTRIES (FLASH_GRANULARITY/32)
            #define UTASKER_APP_START (16 * 1024)                        // 16k application starts at this address
            #define UTASKER_APP_END   (unsigned char *)(UTASKER_APP_START + (48 * 1024) - (ROOT_FILE_ENTRIES * 32)) // end of application space - after maximum application size
            #define UTASKER_PARAMETER_FILE_START (SIZE_OF_FLASH - FLASH_GRANULARITY) // final sector in flash
            #define UTASKER_PARAMETER_FILE_SIZE  (FLASH_GRANULARITY)     // 1k in size (maximum)
            #define PARAMETER_DELETE_PASSWORD  "Enable parameter file erase by dragging this file to the disk"
        #elif defined SPECIAL_VERSION
            #define UTASKER_APP_START (0x4000)                           // 14k application starts at this address
            #define UTASKER_APP_END   (unsigned char *)(UTASKER_APP_START + (48 * 1024) - (ROOT_FILE_ENTRIES * 32)) // end of application space - after maximum application size
        #else
            #define UTASKER_APP_START (24 * 1024)                        // application starts at this address
            #define UTASKER_APP_END   (unsigned char *)(UTASKER_APP_START + (38 * 1024)) // end of application space - after maximum application size
        #endif
        #define INTERMEDIATE_PROG_BUFFER  (1 * 1024)                     // when UART speed greater than 57600 Baud is used an intermediate buffer is recommended
    #elif defined FRDM_KL25Z || defined FRDM_KL26Z || defined tinyK20 || defined TWR_KL25Z48M || defined FRDM_KE06Z || defined TRK_KEA128 || defined FRDM_K20D50M || defined TWR_KM34Z50M || defined TWR_KM34Z75M // {17}
        #define UTASKER_APP_START     (32 * 1024)                        // application starts at this address
        #define UTASKER_APP_END       (unsigned char *)(UTASKER_APP_START + (64 * 1024)) // end of application space - after maximum application size
        #define INTERMEDIATE_PROG_BUFFER  (2 * 1024)                     // when UART speed greater than 57600 Baud is used an intermediate buffer is recommended
    #elif defined TEENSY_3_1 && defined SPECIAL_VERSION
        #if defined SPECIAL_VERSION_SDCARD
            #define UTASKER_APP_START  (32 * 1024)                       // application starts at this address
        #else
            #define UTASKER_APP_START  (16 * 1024)                       // application starts at this address
        #endif
        #if defined SPECIAL_VERSION_2
            #define UTASKER_APP_END   (unsigned char *)(UTASKER_APP_START + (112 * 1024)) // end of application space - after maximum application size
        #else
            #define UTASKER_APP_END   (unsigned char *)(SIZE_OF_FLASH)   // end of application space - after maximum application size
        #endif
    #elif defined TWR_K20D50M ||  defined FRDM_KL46Z || defined FRDM_KL43Z || defined TWR_KL43Z48M || defined TWR_KL46Z48M || defined TEENSY_3_1 || defined TWR_K24F120M // {16}
        #if defined FLEXFLASH_DATA
            #define DISK_D_LOCATION   (SIZE_OF_FLASH - SIZE_OF_FLEXFLASH)// locate a second hard drive in flex flash memory at a virtual location just after internal flash
            #define DISK_D_END        (unsigned char *)(SIZE_OF_FLASH);  // the second hard drive ends after the flex memory
        #endif
        #define UTASKER_APP_START     (32 * 1024)                        // application starts at this address
        #define INTERMEDIATE_PROG_BUFFER  (8 * 1024)                     // when UART speed greater than 57600 Baud is used an intermediate buffer is recommended
        #define UTASKER_APP_END       (unsigned char *)(UTASKER_APP_START + (100 * 1024)) // end of application space - after maximum application size
    #elif defined FRDM_KL02Z || defined FRDM_KL03Z || defined FRDM_KL05Z || defined FRDM_KE02Z || defined FRDM_KE02Z40M || defined TWR_KV10Z32 || defined TWR_KV31F120M || defined TRK_KEA64 || defined FRDM_KEAZN32Q64 // {18}
        #define UTASKER_APP_START     (11 * 1024)                        // application starts at this address
        #if defined TWR_KV31F120M
            #define UTASKER_APP_END   (unsigned char *)(UTASKER_APP_START + (48 * 1024)) // end of application space - after maximum application size
        #else
            #define UTASKER_APP_END   (unsigned char *)(UTASKER_APP_START + (19 * 1024)) // end of application space - after maximum application size
        #endif
    #elif defined FRDM_KEAZ64Q64
        #define UTASKER_APP_START     (32 * 1024)                        // application starts at this address
        #define UTASKER_APP_END       (unsigned char *)(UTASKER_APP_START + (28 * 1024)) // end of application space - after maximum application size
    #elif defined FRDM_KEAZ128Q80 || defined FRDM_KL82Z
        #define UTASKER_APP_START     (32 * 1024)                        // application starts at this address
        #define UTASKER_APP_END       (unsigned char *)(UTASKER_APP_START + (60 * 1024)) // end of application space - after maximum application size
    #elif defined DEV1
        #define UTASKER_APP_START     (16 * 1024)                        // application starts at this address
        #define UTASKER_APP_END       (unsigned char *)(UTASKER_APP_START + (44 * 1024)) // end of application space - after maximum application size
    #else
        #if (defined FRDM_K64F || defined FRDM_K66F) && defined MEMORY_SWAP
            #define UTASKER_APP_START     (SIZE_OF_FLASH/2)              // second half of flash memory is used by the next application
        #elif defined NXP_MSD_HOST                                       // if using NXP host stack the loader is larger in size
            #define UTASKER_APP_START     (64 * 1024)                    // application starts at this address
        #elif defined DWGB_SDCARD
            #define UTASKER_APP_START     (24 * 1024)                    // application starts at this address
        #elif ((defined K02F100M || defined K12D50M) && defined DEV5) || (defined TWR_K60D100M && defined DEV6)
            #define UTASKER_APP_START     (20 * 1024)                    // application starts at 0x5000
        #else
            #define UTASKER_APP_START     (32 * 1024)                    // application starts at this address
        #endif
        #define INTERMEDIATE_PROG_BUFFER  (8 * 1024)                     // when UART speed greater than 57600 Baud is used an intermediate buffer is recommended
        #if defined DWGB_SDCARD
            #define UTASKER_APP_END           (unsigned char *)(SIZE_OF_FLASH) // end of application space - after maximum application size
            #define MAX_FLASH_ERASE_SIZE      (64 * 1024)                // limit flash erasure to blocks of thie size to avoid blocking watchdog task when large flash size is to be erased
            #define ERASE_NEEDED_FLASH_ONLY                              // erase only the flash size needed by the new program code
        #elif (defined TWR_K60D100M && defined DEV6)
            #define UTASKER_APP_END           (unsigned char *)(UTASKER_APP_START + (256 * 1024)) // end of application space - after maximum application size
        #elif (defined K02F100M || defined K12D50M) && defined DEV5
            #define UTASKER_APP_END           (unsigned char *)(UTASKER_APP_START + (108 * 1024)) // end of application space - after maximum application size
        #else
            #define UTASKER_APP_END           (unsigned char *)(UTASKER_APP_START + (128 * 1024)) // end of application space - after maximum application size
        #endif
    #endif
    #if !defined TEENSY_3_1 && !defined TEENSY_LC                        // warning: do not use mass erase with Teensy devices since their loader doesn't support the completely erased state and requires an external loader to recoved to the unsecured flash state
        #define MASS_ERASE                                               // support a mass-erase command. This is used together with a protected FLASH configuration.
                                                                         // when the FLASH is protected, downloads are still possible but the debug interface is blocked.
                                                                         // this allows a commanded delete of the complete FLASH content (including serial loader) to unblock the
                                                                         // debug interface
    #endif
    // Before software can be read from the disk a password file must have been copied {7}
    //
    #if !defined _DEV2
      //#define READ_PASSWORD             "enable file read from the Kinetis device by dragging this file to the disk" // password with maximum length of 512 bytes
    #endif

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
        #if defined SPECIAL_VERSION_SDCARD
          //#define NEW_SOFTWARE_FILE "BCgun*.bin"
            #define NEW_SOFTWARE_FILE "BCvest*.bin"
            #define VALID_VERSION_MAGIC_NUMBER   0x1234
            #define _SECRET_KEY       {0xa7, 0x48, 0xb6, 0x53, 0x11, 0x24}
        #elif (defined TWR_K60D100M && defined DEV6)
            #define NEW_SOFTWARE_FILE "display.bin"
            #define VALID_VERSION_MAGIC_NUMBER   0x0001
            #define _SECRET_KEY       {0xa7, 0x48, 0xb6, 0x53, 0x11, 0x25}
        #elif (defined K02F100M || defined K12D50M) && defined DEV5
            #define NEW_SOFTWARE_FILE "hoist.bin"
            #define VALID_VERSION_MAGIC_NUMBER   0x0002
            #define _SECRET_KEY       {0xa7, 0x48, 0xb6, 0x53, 0x11, 0x26}
        #else
            #if defined SDCARD_SECURE_LOADER
                #define NEW_SOFTWARE_FILE "sd_card_aes256_coded.bin"
              //#define NEW_SOFTWARE_FILE "encrypted.bin"
            #else
                #define NEW_SOFTWARE_FILE "software.bin"
            #endif
            #define VALID_VERSION_MAGIC_NUMBER   0x1234
            #define _SECRET_KEY       {0xa7, 0x48, 0xb6, 0x53, 0x11, 0x24}
        #endif
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
    #if defined _GNU
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
    #if defined _GNU
        #define _GNU_TEMP_WORKAROUND                                     // activate a GCC workaround to allow this to operate with intermediate buffer
    #endif
#elif defined _HW_AVR32
    #define SERIAL_SPEED              SERIAL_BAUD_115200                 // the Baud rate of the UART
    #define UTASKER_APP_START         (FLASH_START_ADDRESS + (10 * 1024))// application starts at this address
    #define UTASKER_APP_END           (unsigned char *)(UTASKER_APP_START + (80 * 1024)) // end of application space - after maximum application size
    #define INTERMEDIATE_PROG_BUFFER  (16 * 1024)                        // use an intermediate buffer and flow control to avoid character loss during FLASH writes
    #if defined _GNU
        #define _GNU_TEMP_WORKAROUND                                     // activate a GCC workaround to allow this to operate with intermediate buffer
    #endif
#elif defined _RX6XX                                                     // {3}
    #undef LOADER_UART
    #if defined RX62N_EVB
        #define LOADER_UART           2
    #else
        #define LOADER_UART           2
        #define UART2_B
    #endif
    #define SERIAL_SPEED              SERIAL_BAUD_115200                 // the Baud rate of the UART
    #define UTASKER_APP_START         (FLASH_START_ADDRESS + (16 * 1024))// application starts at this address
    #define UTASKER_APP_END           (unsigned char *)(UTASKER_APP_START + (80 * 1024)) // end of application space - after maximum application size
    #define INTERMEDIATE_PROG_BUFFER  (45 * 1024)                        // use an intermediate buffer and flow control to avoid character loss during FLASH writes
    #if defined _GNU
        #define _GNU_TEMP_WORKAROUND                                     // activate a GCC workaround to allow this to operate with intermediate buffer
    #endif
#elif defined _STR91XF
    #define SERIAL_SPEED              SERIAL_BAUD_57600                  // the Baud rate of the UART
    #define UTASKER_APP_START         (0x80000)                          // application starts at this address
    #define UTASKER_APP_END           (unsigned char *)(UTASKER_APP_START + (64 * 1024)) // end of application space - after maximum application size
#elif defined _HW_SAM7X
    #define SERIAL_SPEED              SERIAL_BAUD_115200                 // the Baud rate of the UART
    #if defined USB_INTERFACE
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
    #if defined ARDUINO_BLUE_PILL
        #define UTASKER_APP_START     (FLASH_START_ADDRESS + (12 * 1024))// application starts at this address
        #define UTASKER_APP_END       (unsigned char *)(UTASKER_APP_START + (32 * 1024)) // end of application space - after maximum application size
    #else
        #define UTASKER_APP_START     (FLASH_START_ADDRESS + (16 * 1024))// application starts at this address
        #define UTASKER_APP_END       (unsigned char *)(UTASKER_APP_START + (130 * 1024)) // end of application space - after maximum application size
    #endif

    // Before software can be read from the disk a password file must have been copied
    //
  //#define READ_PASSWORD             "enable file read from the STM32 device by dragging this file to the disk" // password with maximum length of 512 bytes

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
#elif defined USB_INTERFACE && (defined USB_MSD_DEVICE_LOADER || defined SPECIAL_VERSION_SDCARD)
    #if defined _DEV2
        #define _UTASKER_APP_START_   (UTASKER_APP_START + (ROOT_FILE_ENTRIES * 32)) // when USB is used the start of application space is used for FAT entries
    #else
        #define _UTASKER_APP_START_   (UTASKER_APP_START + (ROOT_FILE_ENTRIES * 32)) // when USB is used the start of application space is used for FAT entries
    #endif
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
extern void fnSetBacklight(void);
#if defined USB_MSD_ACCEPTS_SREC_FILES || defined USB_MSD_ACCEPTS_HEX_FILES  // {27}
    extern int fnHandleRecord(unsigned char *ptrLine, unsigned char *ptrEnd, int Type);
    // iType
    //
    #define TEST_SERIAL_CONTENT          0
    #define SERIAL_LOADING_IN_OPERATION  1
    #define USB_LOADING_IN_OPERATION     2
#else
    extern int fnHandleRecord(unsigned char *ptrLine, unsigned char *ptrEnd);
#endif
    // Return values
    //
    #define LINE_ACCEPTED                0
    #define PROGRAMMING_ERROR            1
    #define CORRUPTED_SREC               2
    #define INVALID_SREC_HOLE            3
    #define SREC_CS_ERROR                4
    #define INVALID_APPLICATION_LOCATION 5
    #define STOP_FLOW_CONTROL            6
    #define PROGRAMMING_COMPLETE         7

#define T_RESET                   1                                      // application timer events
#define T_GO_TO_APP               2
#define T_COMMIT_BUFFER           3
#define T_MESSAGE_TIMEOUT         4
#define T_HOOKUP_TIMEOUT          4
#define T_MASS_ERASE              5
#define T_RECHECK_CARD            100

// LDC to application
//
#define E_LCD_INITIALISED         1
#define E_LCD_READY               2
#define E_LCD_READ                3
#define E_LCD_ERROR               4
#define E_TOUCH_MOUSE_EVENT       5

// Application to LCD
//
#define E_LCD_COMMAND             1
#define E_LCD_TEXT                2
#define E_LCD_PATTERN             3
#define E_LCD_READ_ADDRESS        4
#define E_LCD_READ_RAM            5
#define E_LCD_COMMAND_TEXT        6
#define E_LCD_PIC                 7
#define E_LCD_LINE                8
#define E_LCD_RECT                9
#define E_LCD_SCROLL              10
#define E_LCD_STYLE               11

// Application interrupt events
//
#define USER_FORCE_LOADER         1
#define DELETE_APPLICATION_FLASH  2
#define TERMINATE_PROGRAMMING     3

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
#if defined USE_MODBUS                                                   // {28}
    extern void fnInitModbus(void);
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
extern void fnPrepareDecrypt(int iEncrypt);

