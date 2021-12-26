/**********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher

    ---------------------------------------------------------------------
    File:      uTaskerBootLoader.c [FREESCALE Coldfire, SAM7X, LPC23XX, LM3Sxxxx, STR91XF, AVR32, RX6XX, Kinetis]
    Project:   Single Chip Embedded Internet
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2021
    *********************************************************************
    27.02.2007 Changed header interpretation so that it is endian-independent {1}
    29.03.2007 Added decryption support                                  {2}
    11.08.2007 Added SPI FLASH support (M5223X)                          {3}
    16.12.2007 Added SPI FLASH support (SAM7X)                           {4}
    02.04.2008 Add LPC23XX support                                       {5}
    02.04.2008 Add LM3SXXXX support                                      {6}
    26.04.2008 Modify SPI FLASH support to include ST FLASH              {7}
    29.04.2008 Wait until SPI FLASH delete has terminated before starting application {8}
    19.08.2008 Add M5222X and M5221X support (USB boot loader)           {9}
    29.08.2008 Add MAIN_FUNCTION_TYPE for compilers which only accept main with int type {10}
    07.01.2009 Change setting to distinguish between M5221x and M5222x   {11}
    19.01.2009 Configuration for M5225x                                  {12}
    07.06.2009 Configuration for AVR32                                   {13}
    25.07.2009 Use start_application() for M5223X and GCC                {14}
    25.07.2009 LM3Sxxxx IAR5 used uTaskerBoot as main()                  {15}
    04.04.2010 Add LPC17XX configuration                                 {16}
    29.11.2010 Add RX6XX configuration                                   {17}
    29.03.2011 Add Kinetis configuration                                 {18}
    16.11.2011 Adjust call to application for Luminary and LPC17xx using start_application() {19}
    12.02.2012 Add Coldfire IAR support                                  {20}
    17.04.2012 Start Kinetis application                                 {21}
    05.05.2012 Add adaptable parameters based on magic number            {22}
    05.05.2012 Correction for encrypted data with large offset           {23}
    24.08.2013 Add option for multiple intermediate locations (MULTIPLE_INTERMEDIATE_CODE_LOCATIONS) {24}
    25.08.2013 Add NET_KBED and NET_K60 configuration                    {25}
    26.05.2013 Option added to operate without uFileSystem header (working directly with upload header) {26}
    27.11.2021 Add STM32 targets

*/

/* =================================================================== */
/*                           include files                             */
/* =================================================================== */

#include "config.h"


/* =================================================================== */
/*                          local definitions                          */
/* =================================================================== */

//#define NO_UFILE_HEADER                                                // {26} the uploaded file has no uFileSystem header

//#define ADAPTABLE_PARAMETERS                                           // support secondary bootloader uploads as well as application uploads
#define BARE_ESSENTIAL
//#define SUPPORT_FORCE_BOOT
//#define SUPPORT_VERIFY_APP_BEFORE_START


/********************* Set up for the boot loader **********************/
/*************** This must match the application and project!! *********/


#if defined _COMPILE_KEIL || ((defined _LPC23XX || defined _LPC17XX || defined _HW_AVR32 || defined _HW_SAM7X) && defined _GNU) // {10}
    #define MAIN_FUNCTION_TYPE int
#else
    #define MAIN_FUNCTION_TYPE void
#endif

#if !defined UTASKER_CODE_START
    #define UTASKER_CODE_START UTASKER_APP_START
#endif


#if defined (_M5223X)
    #if defined SPI_SW_UPLOAD
        #define UPLOAD_FILE_LOCATION   (uFILE_SYSTEM_END + 0)            // start of SPI FLASH {3}
        #define UTASKER_APP_START      0x1000                            // external SPI FLASH solution requires two FLASH block for the boot code because it needs both FLASH and SPI drivers
        #define UTASK_APP_LENGTH       (MAX_FILE_LENGTH)(0x18000 - UTASKER_APP_START)// 92k
    #else
        #if defined _M5225X
            #define UTASKER_APP_START      0x1000                        // internal FLASH solution requires one FLASH block for the boot code (larger sector size)
        #else
            #define UTASKER_APP_START      0x800                         // internal FLASH solution requires one FLASH block for the boot code
        #endif

        #if defined _M5221X
            #define UPLOAD_FILE_LOCATION   (unsigned char *)uFILE_START  // location in internal FLASH (64k)
            #define UTASK_APP_LENGTH       (MAX_FILE_LENGTH)((uFILE_START - 0x1000) - UTASKER_APP_START)// 58k
        #elif defined _M5222X                                            // {11}
            #define UPLOAD_FILE_LOCATION   (unsigned char *)uFILE_START      // location in internal FLASH (100k)
            #define UTASK_APP_LENGTH       (MAX_FILE_LENGTH)((uFILE_START - 0x1000) - UTASKER_APP_START)// 54k
        #else
            #if defined _M5225X
                #define UPLOAD_FILE_LOCATION   (unsigned char *)uFILE_START // location in internal FLASH
                #define UTASK_APP_LENGTH       FILE_SYSTEM_SIZE          // 128k
            #else
                #define UPLOAD_FILE_LOCATION   (unsigned char *)0x21800  // location in internal FLASH
                #define UTASK_APP_LENGTH       (MAX_FILE_LENGTH)(0x18000 - UTASKER_APP_START)// 94k
            #endif
        #endif
    #endif

    static const unsigned char ucKey[] = {0xa7, 0x48, 0xb6, 0x53, 0x11, 0x24};
  //#define _ENCRYPTED                                                   // {2}
    #if defined _ENCRYPTED
        static const unsigned char ucDecrypt[] = {0xff, 0x25, 0xa7, 0x88, 0xf2, 0xe6, 0x81, 0x33, 0x87, 0x77};   // {2} - must be even in length (dividable by unsigned short)
        #define KEY_PRIME               0xafe1                           // never set to 0
        #define CODE_OFFSET             0x226a
        #define VALID_VERSION_MAGIC_NUMBER  0x1235
    #else
        #define VALID_VERSION_MAGIC_NUMBER  0x1234                       // ensure encrypted version has different magic number
    #endif
    #if defined _COMPILE_IAR                                             // {20}
        #define uTaskerBoot            main                              // main calls this routine
    #endif
#elif defined _KINETIS                                                   // {18}
    #if defined NET_KBED || defined NET_K60                              // {25}
        #if defined SPI_SW_UPLOAD
            #define UPLOAD_FILE_LOCATION   (uFILE_SYSTEM_END + 0)        // start of SPI FLASH
            #define UTASKER_APP_START      (0x1000)                      // external SPI FLASH solution requires two FLASH block for the boot code because it needs both FLASH and SPI drivers
          //#define UTASKER_APP2_START     (32 * 1024)                   // this is possible without SREC loader
            #define UTASKER_APP2_START     (48 * 1024)                   // including SREC loader
            #define BOOT_LOADER_LENGTH     UTASKER_APP_START             // this boot loader must fit within this size
            #define UTASK_APP_LENGTH       (MAX_FILE_LENGTH)(UTASKER_APP2_START - BOOT_LOADER_LENGTH) // the maximum length of the secondary loader
        #else
            #define UTASKER_APP_START      0x1000                        // internal FLASH solution requires two FLASH block for the boot code (slightly larger than 2k)
            #define UPLOAD_FILE_LOCATION   (unsigned char *)uFILE_START  // location in internal FLASH
            #define UTASK_APP_LENGTH       FILE_SYSTEM_SIZE              // 128k
        #endif

        static const unsigned char ucKey[] = {'E', 'L', 'Z', 'E', 'T', 'B'};  // boot loader key

        #if defined ADAPTABLE_PARAMETERS                                 // used for secondary loader and application
            #if defined NET_KBED
                static const unsigned char ucKey2[] = {'E', 'L', 'Z', 'E', 'T', 'A'}; // new application key
                #define VALID_VERSION_MAGIC_NUMBER_2  0xA035             // new application magic number
            #elif defined NET_K60
                static const unsigned char ucKey2[] = {'E', 'L', 'Z', 'E', 'T', 'A'}; // new application key
                #define VALID_VERSION_MAGIC_NUMBER_2  0xA040             // new application magic number
            #endif	  	
        #endif
      //#define _ENCRYPTED
        #if defined _ENCRYPTED
            static const unsigned char ucDecrypt[] = {0xff, 0x25, 0xa7, 0x88, 0xf2, 0xe6, 0x81, 0x33, 0x87, 0x77}; // must be even in length (dividable by unsigned short)
            #define KEY_PRIME              0xafe1                        // never set to 0
            #define CODE_OFFSET            0x226a
            #define VALID_VERSION_MAGIC_NUMBER  0x1235
        #else
            #if defined NET_KBED
                #define VALID_VERSION_MAGIC_NUMBER  0xB035               // magic number
            #elif defined NET_K60
                #define VALID_VERSION_MAGIC_NUMBER  0xB040               // magic number
            #endif	  	
        #endif
    #else
        #if defined SPI_SW_UPLOAD
            #define UPLOAD_FILE_LOCATION   (uFILE_SYSTEM_END + 0)        // start of SPI FLASH
            #define UTASKER_APP_START      0x1000                        // external SPI FLASH solution requires one FLASH block for the boot code because it needs both FLASH and SPI drivers
            #define UTASK_APP_LENGTH       (MAX_FILE_LENGTH)(0x18000 - UTASKER_APP_START) // 92k
        #else
            #define UTASKER_APP_START      0x1000                        // internal FLASH solution requires one FLASH block for the boot code (slightly larger than 2k)
            #define UPLOAD_FILE_LOCATION   (unsigned char *)uFILE_START  // location in internal FLASH
            #define UTASK_APP_LENGTH       FILE_SYSTEM_SIZE              // 128k
        #endif

        static const unsigned char ucKey[] = {0xa7, 0x48, 0xb6, 0x53, 0x11, 0x24};
      //#define _ENCRYPTED
        #if defined _ENCRYPTED
            static const unsigned char ucDecrypt[] = {0xff, 0x25, 0xa7, 0x88, 0xf2, 0xe6, 0x81, 0x33, 0x87, 0x77}; // must be even in length (dividable by unsigned short)
            #define KEY_PRIME               0xafe1                       // never set to 0
            #define CODE_OFFSET             0x226a
            #define VALID_VERSION_MAGIC_NUMBER  0x1235
        #else
            #define VALID_VERSION_MAGIC_NUMBER  0x1234                   // ensure encrypted version has different magic number
        #endif
    #endif
#elif defined (_HW_SAM7X)
    #if defined MJB_BUILD_BC2
        #if defined SPI_SW_UPLOAD
            #define UPLOAD_FILE_LOCATION   (unsigned char *)(uFILE_SYSTEM_END + (SPI_FLASH_BLOCK_LENGTH*4*56))
            #define UTASKER_APP_START      0x100b00                      // 2.75k boot loader space
            #define UTASK_APP_LENGTH       (MAX_FILE_LENGTH)(73.25*1024) // 73.25k
            #define VALID_VERSION_MAGIC_NUMBER  0x5634
            static const unsigned char ucKey[] = {0x99, 0x28, 0x37, 0x67, 0x6b, 0xcd}; // 992837676bcd
          //#define _ENCRYPTED
            static const unsigned char ucDecrypt[] = {0x36, 0x77, 0x2b, 0xe9, 0x2b, 0xca, 0x92, 0x83, 0x6f, 0xee, 0xca, 0xa1, 0x10, 0x29};   // {2} - must be even in length (dividable by unsigned short)
            #define KEY_PRIME               0x229a
            #define CODE_OFFSET             0x2003
        #else
            #define UPLOAD_FILE_LOCATION   (unsigned char *)0x130000
            #define UTASKER_APP_START      0x100700
            #define UTASK_APP_LENGTH       (MAX_FILE_LENGTH)(0xf100)     // 60,25k
            #define VALID_VERSION_MAGIC_NUMBER  0x1234
            static const unsigned char ucKey[] = {0xa7, 0x48, 0xb6, 0x53, 0x11, 0x24};
        #endif
    #elif defined SPI_SW_UPLOAD
        #define UPLOAD_FILE_LOCATION   (unsigned char *)0x337400
        #define UTASKER_APP_START      0x100700
        #define UTASK_APP_LENGTH       (MAX_FILE_LENGTH)(254*1024)       // 254k
        #define VALID_VERSION_MAGIC_NUMBER  0x1234
        static const unsigned char ucKey[] = {0xa7, 0x48, 0xb6, 0x53, 0x11, 0x24};
    #else
        #define UPLOAD_FILE_LOCATION   (unsigned char *)0x127800
        #define UTASKER_APP_START      0x100700
        #define UTASK_APP_LENGTH       (MAX_FILE_LENGTH)(0x18000 - 0x700)// 94.25k
        #define VALID_VERSION_MAGIC_NUMBER  0x1234
        static const unsigned char ucKey[] = {0xa7, 0x48, 0xb6, 0x53, 0x11, 0x24};
    #endif
    #if !defined _WINDOWS
        #define uTaskerBoot        main                                  // main calls this routine
    #endif
#elif defined (_STR91XF)
    #define UPLOAD_FILE_LOCATION   (unsigned char *)0xe0000
    #define UTASKER_APP_START      0x80000
    #define UTASK_APP_LENGTH       (MAX_FILE_LENGTH)(128 * 1024)         // 128k
    #define VALID_VERSION_MAGIC_NUMBER  0x5678
    #if !defined _WINDOWS
        #define uTaskerBoot        main                                  // main calls this routine
    #endif
    static const unsigned char ucKey[] = {0xa7, 0x48, 0xb6, 0x53, 0x11, 0x24};
#elif defined (_LPC23XX)
    #if defined _NPT
        #define UPLOAD_FILE_LOCATION   (unsigned char *)0x40000          // location in internal FLASH
        #define UTASKER_APP_START      0x1000                            // internal FLASH solution requires one small FLASH block
        #define UTASK_APP_LENGTH       (MAX_FILE_LENGTH)(231 * 1024)     // 231k maxim upload (to avoid the small FLASH blocks at the end from being used)
	#else
        #define UPLOAD_FILE_LOCATION   (unsigned char *)0x5a000          // location in internal FLASH
        #define UTASKER_APP_START      0x1000                            // internal FLASH solution requires one 4k FLASH block for the boot code
        #define UTASK_APP_LENGTH       (MAX_FILE_LENGTH)(135*1024)       // 135k
	#endif
    #define VALID_VERSION_MAGIC_NUMBER  0x22ba
    static const unsigned char ucKey[] = {0xb2, 0x67, 0xa8, 0x20, 0x09, 0x26};
  //#define _ENCRYPTED
    #if defined _ENCRYPTED
        static const unsigned char ucDecrypt[] = {0x36, 0x77, 0x2b, 0xe9, 0x2b, 0xca, 0x92, 0x83, 0x6f, 0xee, 0xca, 0xa1, 0x10, 0x29};   // {2} - must be even in length (dividable by unsigned short)
        #define KEY_PRIME               0x229a
        #define CODE_OFFSET             0x2003
    #endif

    #if !defined _WINDOWS
        #define uTaskerBoot        main                                  // main calls this routine
    #endif
    extern void fnSetNormalMode(void);
#elif defined _LPC17XX                                                   // {16}
    #if defined SPI_SW_UPLOAD
        #define UPLOAD_FILE_LOCATION   (unsigned char *)0xbdd00          // location in SPI Flash
        #define UTASKER_APP_START      0x3000                            // internal FLASH solution requires one 4k FLASH block for the boot code but then reserves 2 x 4k parameter blocks for use by the application
        #define UTASK_APP_LENGTH       (MAX_FILE_LENGTH)(128*1024 - UTASKER_APP_START) 
    #else
        #define UPLOAD_FILE_LOCATION   (unsigned char *)0x20000          // location in internal FLASH - 128k
        #define UTASKER_APP_START      0x3000                            // internal FLASH solution requires one 4k FLASH block for the boot code but then reserves 2 x 4k parameter blocks for use by the application
        #define UTASK_APP_LENGTH       (MAX_FILE_LENGTH)(128*1024 - UTASKER_APP_START) 
    #endif
    #define VALID_VERSION_MAGIC_NUMBER  0x22ba
    static const unsigned char ucKey[] = {0xb2, 0x67, 0xa8, 0x20, 0x09, 0x26};
  //#define _ENCRYPTED
    #if defined _ENCRYPTED
        static const unsigned char ucDecrypt[] = {0x37, 0x79, 0x33, 0xd3, 0x11, 0xa3, 0x10, 0x19, 0x55, 0xff, 0x22, 0xd1, 0x09, 0x2f};   // must be even in length (dividable by unsigned short)
        #define KEY_PRIME               0x1982
        #define CODE_OFFSET             0x26dc
    #endif
    #if !defined _WINDOWS
        #define uTaskerBoot         _main                                // main calls this routine
    #endif
#elif defined (_LM3SXXXX)
    #if defined SPI_SW_UPLOAD
        #define UPLOAD_FILE_LOCATION   (unsigned char *)(SIZE_OF_FLASH + UPLOAD_OFFSET) // start of firmware in SPI FLASH
        #define UTASKER_CODE_START     0x0c00                            // external SPI FLASH solution requires two FLASH blocks for the boot code because it needs both FLASH and SPI drivers
        #define UTASKER_APP_START      0x3000                            // location of application
        #define UTASK_APP_LENGTH       (MAX_FILE_LENGTH)(116 * 1024)     // 119k
    #else
        #define UPLOAD_FILE_LOCATION   (unsigned char *)0x28400          // location in internal FLASH
        #define UTASKER_APP_START      0x800                             // internal FLASH solution requires two 1k FLASH blocks for the boot code
        #define UTASK_APP_LENGTH       (MAX_FILE_LENGTH)(94 * 1024)      // 94k
    #endif

    static const unsigned char ucKey[] = {0x88, 0x26, 0x3b, 0x62, 0x90, 0xa1};
    #define _ENCRYPTED
    #if defined _ENCRYPTED
        static const unsigned char ucDecrypt[] = {0xff, 0x25, 0xa7, 0x88, 0xf2, 0xe6, 0x81, 0x33, 0x87, 0x77};   // {2} - must be even in length (dividable by unsigned short)
        #define KEY_PRIME               0xafe1                           // never set to 0
        #define CODE_OFFSET             0x226a
        #define VALID_VERSION_MAGIC_NUMBER  0x1235
    #else
        #define VALID_VERSION_MAGIC_NUMBER  0x72ca                       // ensure encrypted version has different magic number
    #endif
    #if defined COMPILE_IAR5
        #define uTaskerBoot        main                                  // {15} main calls this routine
    #endif
#elif defined (_HW_AVR32)                                                // {13}
    static const unsigned char ucKey[] = {0xa7, 0x48, 0xb6, 0x53, 0x11, 0x24};
    #if defined ADAPTABLE_PARAMETERS                                     // support upload of secondary loader and application
        #define UTASKER_APP_START      (START_OF_FLASH + 0x1400)         // internal FLASH solution requires one FLASH block for the boot code (larger sector size)
        #define UPLOAD_FILE_LOCATION   (unsigned char *)(0x80214400)     // location in internal FLASH (500k short of the end of 2Meg SPi Flash (528 byte mode)
        #define UTASK_APP_LENGTH       (32 * 1024)                       // secondary boot loader size - this is deleted when a new secondary loader code is uploaded
        #define UTASKER_APP2_START     (START_OF_FLASH + 0x9400)         // application start location
        #define UTASK_APP2_LENGTH      ((512 - 37) * 1024)               // maximum application size - this is deleted when space if made for the new clode to be copied to

        static const unsigned char ucKey2[] = {0x22, 0xa1, 0xb6, 0x9a, 0x01, 0x5f};
    #else
        #define UTASKER_APP_START      (START_OF_FLASH + 0x800)          // internal FLASH solution requires one FLASH block for the boot code (larger sector size)
        #define UPLOAD_FILE_LOCATION   (unsigned char *)uFILE_START      // location in internal FLASH
        #define UTASK_APP_LENGTH       FILE_SYSTEM_SIZE                  // 128k
    #endif
    #define _ENCRYPTED
    #if defined _ENCRYPTED
        static const unsigned char ucDecrypt[] = {0xff, 0x25, 0xa7, 0x88, 0xf2, 0xe6, 0x81, 0x33, 0x87, 0x77};   // {2} - must be even in length (dividable by unsigned short)
        #define KEY_PRIME               0xafe1                           // never set to 0
        #define CODE_OFFSET             0x226a
        #define VALID_VERSION_MAGIC_NUMBER   0x1235
        #define VALID_VERSION_MAGIC_NUMBER_2 0x1245
    #else
        #define VALID_VERSION_MAGIC_NUMBER   0x1234                      // ensure encrypted version has different magic number
        #define VALID_VERSION_MAGIC_NUMBER_2 0x1244
    #endif
    #if !defined _WINDOWS
        #define uTaskerBoot        main                                  // main calls this routine
    #endif
#elif defined (_RX6XX)                                                   // {17}
    #define UTASKER_APP_START      (START_OF_FLASH)                      // at bottom of FLASH since the loader is either in boot loader FLASH or else in the boot vector FLASH sector
    #define UPLOAD_FILE_LOCATION   (unsigned char *)uFILE_START          // location in internal FLASH where the new code is located
    #define UTASK_APP_LENGTH       FILE_SYSTEM_SIZE                      // 16k
    static const unsigned char ucKey[] = {0xa7, 0x48, 0xb6, 0x53, 0x11, 0x24};
  //#define _ENCRYPTED
    #if defined _ENCRYPTED
        static const unsigned char ucDecrypt[] = {0xff, 0x25, 0xa7, 0x88, 0xf2, 0xe6, 0x81, 0x33, 0x87, 0x77};   // {2} - must be even in length (dividable by unsigned short)
        #define KEY_PRIME               0xafe1                           // never set to 0
        #define CODE_OFFSET             0x226a
        #define VALID_VERSION_MAGIC_NUMBER  0x1235
    #else
        #define VALID_VERSION_MAGIC_NUMBER  0x1234                       // ensure encrypted version has different magic number
    #endif
    #if !defined _WINDOWS
        #define uTaskerBoot        uTaskerBoot                           // main calls this routine
    #endif
#elif defined _STM32
    #if defined FLASH_GRANULARITY_BOOT                                   // F2/F4 reserve first boot sector for boot loader and start the application after it
        #define UTASKER_APP_START  (FLASH_START_ADDRESS + (3 * FLASH_GRANULARITY_BOOT)) // 48k - 0xc000 from start of flash (after boot loader plus 2 swap block sectors)
    #else
        #define UTASKER_APP_START  (FLASH_START_ADDRESS + FLASH_GRANULARITY) // F1 devices reserve first sector in flash for boot loader and start the application after it
    #endif

    #if defined SPI_SW_UPLOAD  
        #define uFILE_START    (unsigned char *)(FLASH_START_ADDRESS + SIZE_OF_FLASH) // the address where the new application is located when present
    #else
        #define uFILE_START    (unsigned char *)(FLASH_START_ADDRESS + 0x60000) // the address where the new application is located when present
    #endif
    #define FILE_SYSTEM_SIZE   (128 * 1024)                              // maximum size of new application
    static const unsigned char ucKey[] = { 0x62, 0x23, 0x19, 0xde, 0x22, 0xb1 };
    //#define _ENCRYPTED
    #if defined _ENCRYPTED
        static const unsigned char ucDecrypt[] = {0xee, 0x23, 0xa9, 0xa1, 0x98, 0xa9, 0x00, 0x21, 0xba, 0x2a};   // must be even in length (dividable by unsigned short)
        #define KEY_PRIME               0x8862                           // never set to 0
        #define CODE_OFFSET             0x3cde
        #define VALID_VERSION_MAGIC_NUMBER  0x9876
    #else
        #define VALID_VERSION_MAGIC_NUMBER  0x5432                       // ensure encrypted version has different magic number
    #endif

    #define FILE_GRANULARITY       (1 * FLASH_GRANULARITY)
    #define UPLOAD_FILE_LOCATION   uFILE_START
    #define UTASK_APP_LENGTH       FILE_SYSTEM_SIZE
    #if !defined _WINDOWS
        #define uTaskerBoot        uTaskerBoot                           // main calls this routine
    #endif
#endif
#if !defined UTASKER_CODE_START
    #define UTASKER_CODE_START UTASKER_APP_START
#endif


#define BLOCK_SIZE 1024                                                  // the code is etrached and programmed in blocks of this size

/***********************************************************************/

typedef struct stCODE_LOCATIONS                                          // {24} intermediate code location entry
{
    unsigned char *ptrCodeAddress;
    unsigned long  ulMaxApplicationSize;
} CODE_LOCATIONS;

typedef struct stUPLOAD_HEADER
{
    unsigned long  ulCodeLength;
    unsigned short usMagicNumber;
    unsigned short usCRC;
#if defined _ENCRYPTED
    unsigned short usRAWCRC;
#endif
#if defined ADAPTABLE_PARAMETERS                                         // {22}
    unsigned short usDestinationType;
#endif
#if defined MULTIPLE_INTERMEDIATE_CODE_LOCATIONS                         // {24}
    CODE_LOCATIONS *ptrCodeLocation;
#endif
} UPLOAD_HEADER;

#if defined _ENCRYPTED                                                   // ensure length is compiler independent
    #define SIZE_OF_UPLOAD_HEADER (sizeof(unsigned long) + (3 * sizeof(unsigned short)))
#else
    #define SIZE_OF_UPLOAD_HEADER (sizeof(unsigned long) + (2 * sizeof(unsigned short)))
#endif

#if defined MULTIPLE_INTERMEDIATE_CODE_LOCATIONS                         // {24}
    #define _UTASK_APP2_LENGTH    file_header.ulCodeLength
    #define _UPLOAD_FILE_LOCATION file_header->ptrCodeLocation->ptrCodeAddress
    #define _LOCATION             file_header.ptrCodeLocation
#else
    #define _UTASK_APP2_LENGTH    UTASK_APP2_LENGTH
    #define _UPLOAD_FILE_LOCATION UPLOAD_FILE_LOCATION
    #define _LOCATION
#endif

/* =================================================================== */
/*                             constants                               */
/* =================================================================== */

#if defined MULTIPLE_INTERMEDIATE_CODE_LOCATIONS                         // {24}
    #if defined SPI_SW_UPLOAD
        #define UPLOAD_START_IN_SPI_FLASH 0x113fc0                       // 464k from end of SPI Flash
        #define SPI_FLASH_LOCATIONS       8                              // number of possible code locations in SPI flash
    #else
        #define SPI_FLASH_LOCATIONS       0
    #endif
    #define INTERNAL_FLASH_LOCATIONS      6                              // number of possible code locations in internal flash
    #define FIRST_INTERNAL_FLASH_ENTRY    SPI_FLASH_LOCATIONS            // index to the first location in internal flash

    CODE_LOCATIONS cCodeLocations[SPI_FLASH_LOCATIONS + INTERNAL_FLASH_LOCATIONS + 1] = {
    #if defined SPI_SW_UPLOAD
        {(unsigned char *)(UPLOAD_START_IN_SPI_FLASH),                     (464 * 1024)}, // SPI flash locations
        {(unsigned char *)((UPLOAD_START_IN_SPI_FLASH) + (1 * 64 * 1056)), (400 * 1024)},
        {(unsigned char *)((UPLOAD_START_IN_SPI_FLASH) + (2 * 64 * 1056)), (336 * 1024)},
        {(unsigned char *)((UPLOAD_START_IN_SPI_FLASH) + (3 * 64 * 1056)), (272 * 1024)},
        {(unsigned char *)((UPLOAD_START_IN_SPI_FLASH) + (4 * 64 * 1056)), (208 * 1024)},
        {(unsigned char *)((UPLOAD_START_IN_SPI_FLASH) + (5 * 64 * 1056)), (144 * 1024)},
        {(unsigned char *)((UPLOAD_START_IN_SPI_FLASH) + (6 * 64 * 1056)), (80  * 1024)},
        {(unsigned char *)((UPLOAD_START_IN_SPI_FLASH) + (7 * 64 * 1056)), (16  * 1024)},
    #endif
        {(unsigned char *)(288 * 1024), (224 * 1024)},                   // internal flash locations
        {(unsigned char *)(320 * 1024), (192 * 1024)},
        {(unsigned char *)(352 * 1024), (160 * 1024)},
        {(unsigned char *)(384 * 1024), (128 * 1024)},
        {(unsigned char *)(416 * 1024), (96  * 1024)},
        {(unsigned char *)(448 * 1024), (64  * 1024)},
        {0, 0}                                                           // end of the list
    };
#endif

/* =================================================================== */
/*                 local function prototype declarations               */
/* =================================================================== */

#if defined SUPPORT_VERIFY_APP_BEFORE_START
    static int  fnCheckValidCode(void);
#endif
#if defined SUPPORT_FORCE_BOOT
    static int  fnCheckForceBoot(void);
#endif

static int fnCheckNewCode(UPLOAD_HEADER *file_header);
static void fnCopyNewCode(UPLOAD_HEADER *file_header);
static int fnCheckCopiedCode(UPLOAD_HEADER *file_header);
#if defined MULTIPLE_INTERMEDIATE_CODE_LOCATIONS                         // {24}
    static void fnDeleteCodeCopy(CODE_LOCATIONS *ptrCodeLocation);
#else
    static void fnDeleteCodeCopy(void);
#endif
#if defined _ENCRYPTED
    static unsigned short fnDecrypt(unsigned short usPRNG, unsigned char *ptrData, unsigned short usBlockSize);
#endif


// The uTasker Boot-loader
//
extern MAIN_FUNCTION_TYPE uTaskerBoot(void)
{
    UPLOAD_HEADER file_header;
#if defined (_STR91XF) || defined (_HW_SAM7X) || defined (_LPC23XX)      // {5}{6}{13}{19}
    void (*main_call)(void);
    #if defined _LM3SXXXX || defined _LPC17XX                            // {16}
    unsigned long ulStart = *(unsigned long *)fnGetFlashAdd((unsigned char *)(UTASKER_CODE_START + 4)); // {6} address of code start routine
    #endif
#endif
#if defined MULTIPLE_INTERMEDIATE_CODE_LOCATIONS                         // {24}
    #if defined SPI_SW_UPLOAD
    if (fnConfigSPIFileSystem() == 0) {                                  // if the flash was successfully detected
        file_header.ptrCodeLocation = cCodeLocations;                    // start in SPI flash
    }
    else {                                                               // else there is no SPI flash
        file_header.ptrCodeLocation = &cCodeLocations[FIRST_INTERNAL_FLASH_ENTRY]; // jump the SPI locations and search only in internal flash
    }
    #else
    file_header.ptrCodeLocation = cCodeLocations;                        // start in internal flash
    #endif
    do {                                                                 // for each possible code location in the table
#endif
#if defined SPI_SW_UPLOAD && !defined MULTIPLE_INTERMEDIATE_CODE_LOCATIONS
        if (!(fnConfigSPIFileSystem()) && (fnCheckNewCode(&file_header)))// configure SPI interface for maximum speed {3} and see whether waiting code is valid
#else
        if (fnCheckNewCode(&file_header))                                // see whether waiting code is valid
#endif
        {
#if defined ADAPTABLE_PARAMETERS                                         // {22}
            if (file_header.usDestinationType == 2) {
                uFileErase((unsigned char *)UTASKER_APP2_START, _UTASK_APP2_LENGTH); // valid code is waiting to be loaded so first delete the old code
            }
            else {
                uFileErase((unsigned char *)UTASKER_APP_START, UTASK_APP_LENGTH); // valid code is waiting to be loaded so first delete the old code
            }
#else
            uFileErase((unsigned char *)UTASKER_APP_START, UTASK_APP_LENGTH); // valid code is waiting to be loaded so first delete the old code
#endif
            fnCopyNewCode(&file_header);                                 // now copy the new code to the uTasker application position
            if (fnCheckCopiedCode(&file_header) != 0) {                  // now verify that the new code is correct and delete its copy
                fnDeleteCodeCopy(_LOCATION);
            }
            fnResetBoard();
        }
#if defined MULTIPLE_INTERMEDIATE_CODE_LOCATIONS                         // {24}
    } while ((++(file_header.ptrCodeLocation))->ptrCodeAddress != 0);    // check through all possible intermediate code locations
#endif
#if !defined BARE_ESSENTIAL
    else if (
    #if defined SUPPORT_FORCE_BOOT
    fnCheckForceBoot()
    #endif
    #if defined SUPPORT_VERIFY_APP_BEFORE_START
    || fnCheckValidCode()
    #endif
    ) {
        // The boot loader has detected that there is no program loaded. If there is serial loader, offer this now
    }
#endif
    // The boot loader has detected the uTasker application code so it will be started now
    //
#if defined (_STR91XF) || defined (_HW_SAM7X) || defined (_LPC23XX)      // {5}
    #if defined _LPC23XX
    fnSetNormalMode();                                                   // ensure settings normal when starting application
    #endif
    main_call = (void (*)(void))UTASKER_CODE_START;
    #if !defined _WINDOWS
    main_call();                                                         // call code
    #endif
#elif defined _STM32 || defined _HW_AVR32 || defined _RX6XX || defined _KINETIS || defined _LM3SXXXX || defined _LPC17XX || (defined _M5223X && (defined _GNU || defined _COMPILE_IAR)) // {14}{17}{19}{20}{21}
    #if !defined _WINDOWS
    start_application(UTASKER_CODE_START);                               // jump to the application
    #endif
#endif
#if defined _COMPILE_KEIL || ((defined _LPC23XX || defined _LPC17XX || defined _HW_AVR32 || defined _HW_SAM7X) && defined _GNU)
    return 0;
#endif
}


// Copy code from backup location to the application area
//
static void fnCopyNewCode(UPLOAD_HEADER *file_header)
{
#if defined _ENCRYPTED
    #if defined NO_UFILE_HEADER                                          // {26}
    unsigned char *ptrFile = (_UPLOAD_FILE_LOCATION + CODE_OFFSET + SIZE_OF_UPLOAD_HEADER);
    #else
    unsigned char *ptrFile = (_UPLOAD_FILE_LOCATION + CODE_OFFSET + SIZE_OF_UPLOAD_HEADER + FILE_HEADER);
    #endif
#else
    unsigned char *ptrFile = _UPLOAD_FILE_LOCATION;
#endif
    unsigned char *ptrNewFile;
    unsigned char ucCodeBlock[BLOCK_SIZE];
    unsigned short usBlockSize = BLOCK_SIZE;
    MAX_FILE_LENGTH file_length = (MAX_FILE_LENGTH)file_header->ulCodeLength;
#if defined _ENCRYPTED                                                   // {2}
    unsigned short usPRNG = KEY_PRIME;                                   // prime the random generator from the key pattern
    #if defined NO_UFILE_HEADER                                          // {26}
    unsigned char *ptrEnd = ((UPLOAD_FILE_LOCATION + SIZE_OF_UPLOAD_HEADER) + file_length);
    #else
    unsigned char *ptrEnd = ((UPLOAD_FILE_LOCATION + SIZE_OF_UPLOAD_HEADER + FILE_HEADER) + file_length);
    #endif
    while (ptrFile >= ptrEnd) {                                          // {23}
        ptrFile -= file_length;
    }
#else
    #if defined NO_UFILE_HEADER                                          // {26}
    ptrFile += SIZE_OF_UPLOAD_HEADER;
    #else
    ptrFile += SIZE_OF_UPLOAD_HEADER + FILE_HEADER;
    #endif
#endif
#if defined ADAPTABLE_PARAMETERS                                         // {22}
    if (file_header->usDestinationType == 2) {
        ptrNewFile = (unsigned char *)UTASKER_APP2_START;
    }
    else {
        ptrNewFile = (unsigned char *)UTASKER_APP_START;
    }
#else
    ptrNewFile = (unsigned char *)UTASKER_APP_START;
#endif

    while (file_length) {
        if (file_length < BLOCK_SIZE) {
            usBlockSize = (unsigned short)file_length;                   // last block smaller than full block size
        }
#if defined _ENCRYPTED
        if ((ptrFile + usBlockSize) >= ptrEnd) {
            unsigned short usSubBlockSize = (ptrEnd - ptrFile);
            fnGetPars(ptrFile, ucCodeBlock, usSubBlockSize);
            fnGetPars((UPLOAD_FILE_LOCATION + SIZE_OF_UPLOAD_HEADER + FILE_HEADER), &ucCodeBlock[usSubBlockSize], (usBlockSize - usSubBlockSize));
            ptrFile = (UPLOAD_FILE_LOCATION + SIZE_OF_UPLOAD_HEADER + FILE_HEADER) - usSubBlockSize;
        }
        else {
            fnGetPars(ptrFile, ucCodeBlock, usBlockSize);
        }
#else
        fnGetPars(ptrFile, ucCodeBlock, usBlockSize);
#endif
#if defined _ENCRYPTED                                                   // {2}
        usPRNG = fnDecrypt(usPRNG, ucCodeBlock, usBlockSize);            // decode the block
#endif
        fnWriteBytesFlash(ptrNewFile, ucCodeBlock, usBlockSize);
        ptrFile += usBlockSize;
        ptrNewFile += usBlockSize;
        file_length -= usBlockSize;
    }
#if defined _LPC23XX || defined _LPC17XX || defined _HW_AVR32 || defined _RX6XX || defined _KINETIS // {16}{17}{18}
    fnWriteBytesFlash(0, 0, 0);                                          // close last write
#endif
}

// Delete the backup of the new application code
//
#if defined MULTIPLE_INTERMEDIATE_CODE_LOCATIONS                         // {24}
static void fnDeleteCodeCopy(CODE_LOCATIONS *ptrCodeLocation)
#else
static void fnDeleteCodeCopy(void)
#endif
{
#if defined MULTIPLE_INTERMEDIATE_CODE_LOCATIONS                         // {24}
    unsigned char *ptrFile = ptrCodeLocation->ptrCodeAddress;
#else
    unsigned char *ptrFile = UPLOAD_FILE_LOCATION;
#endif
    MAX_FILE_LENGTH file_length = uGetFileLength(ptrFile);
#if defined NO_UFILE_HEADER
    uFileErase(ptrFile, (MAX_FILE_LENGTH)(file_length + SIZE_OF_UPLOAD_HEADER));
#else
    uFileErase(ptrFile, (MAX_FILE_LENGTH)(file_length + FILE_HEADER));
#endif
#if defined SPI_SW_UPLOAD                                                 // {8} the erase of SPI FLASH can take some time. Wait until complete before starting the application.
    uGetFileLength(ptrFile);                                             // this automatically waits for the SPI FLASH to be ready again
#endif
}

// Verify that the copied code is intact
//
static int fnCheckCopiedCode(UPLOAD_HEADER *file_header)
{
    unsigned char *ptrFile;
    unsigned char ucCodeBlock[BLOCK_SIZE];
    unsigned short usBlockSize = BLOCK_SIZE;
    unsigned short usCRC = 0;
    MAX_FILE_LENGTH file_length = (MAX_FILE_LENGTH)file_header->ulCodeLength;

#if defined ADAPTABLE_PARAMETERS                                         // {22}
    if (file_header->usDestinationType == 2) {
        ptrFile = (unsigned char *)UTASKER_APP2_START;
    }
    else {
        ptrFile = (unsigned char *)UTASKER_APP_START;
    }
#else
    ptrFile = (unsigned char *)UTASKER_APP_START;
#endif
    while (file_length) {
        if (file_length < BLOCK_SIZE) {
            usBlockSize = (unsigned short)file_length;
        }
        fnGetPars(ptrFile, ucCodeBlock, usBlockSize);
        usCRC = fnCRC16(usCRC, ucCodeBlock, usBlockSize);
        ptrFile += usBlockSize;
        file_length -= usBlockSize;
    }
#if !defined NO_SECRET_KEY
    #if defined ADAPTABLE_PARAMETERS                                     // {22}
    if (file_header->usDestinationType == 2) {
        usCRC = fnCRC16(usCRC, (unsigned char *)ucKey2, sizeof(ucKey2)); // add the secret key belonging to the second destination type
    }
    else {
        usCRC = fnCRC16(usCRC, (unsigned char *)ucKey, sizeof(ucKey));   // add the secret key
    }
    #else
    usCRC = fnCRC16(usCRC, (unsigned char *)ucKey, sizeof(ucKey));       // add the secret key
    #endif
#endif
#if defined _ENCRYPTED
    return (usCRC == file_header->usRAWCRC);
#else
    return (usCRC == file_header->usCRC);
#endif
}

#if defined _ENCRYPTED
// Decode a block of data
//
static unsigned short fnDecrypt(unsigned short usPRNG, unsigned char *ptrData, unsigned short usBlockSize)                          // {2}
{
    static int iCycleLength = 0;

    while (usBlockSize != 0) {
        while (iCycleLength < sizeof(ucDecrypt)) {                       // cycle through second key
            usPRNG = fnCRC16(usPRNG, (unsigned char *)&ucDecrypt[iCycleLength++], 1); // generate next pseudo random pattern from decrypt pattern
            *ptrData = *ptrData ^ usPRNG;                                // decrypt each byte
            ptrData++;
            usBlockSize--;
            if (!usBlockSize) {
                break;
            }
        }
        if (iCycleLength >= sizeof(ucDecrypt)) {
            iCycleLength = 0;
        }
    }
    return usPRNG;
}
#endif


#if defined _STR91XF                                                     // {4}
extern int uFileErase(unsigned char *ptrFile, MAX_FILE_LENGTH FileLength)
{
    while (FileLength) {
        if (fnEraseFlash(ptrFile)) {                                     // erase a file block
            return 1;                                                    // return error if one found
        }
        ptrFile += FILE_GRANULARITY;
        if (FileLength <= FILE_GRANULARITY) {
            return 0;
        }
        else {
            FileLength -= FILE_GRANULARITY;
        }
    }
    return 0;
}
#endif

#if defined _HW_SAM7X || defined _LPC23XX || defined _LPC17XX || defined _LM3SXXXX || defined _HW_AVR32 || defined _RX6XX // {5}{6}{13}{16}{17}
// Erase all sectors occupied by file
//
extern int uFileErase(MEMORY_RANGE_POINTER ptrFile, MAX_FILE_LENGTH FileLength) // {4}
{
    if (fnEraseFlashSector(ptrFile, FileLength)) {                       // erase a file block
        return  (-1);                                                    // return error if one found
    }
    return 0;
}
#endif

#if !defined _M5223X
// This routine returns the length of the file, a zero if empty or invalid
//
extern MAX_FILE_LENGTH uGetFileLength(unsigned char *ptrfile)
{
    MAX_FILE_LENGTH FileLength;

    fnGetPars(ptrfile, (unsigned char *)&FileLength, sizeof(MAX_FILE_LENGTH));

    if (FileLength == 0) {                                               // protect against corrupted file so that it can be deleted
        return FILE_GRANULARITY;                                         // the size of a flash block
    }

    if (FileLength == (MAX_FILE_LENGTH)(-1)) {                           // check for blank FLASH memory
        return 0;                                                        // no length so empty
    }

    // If a (corrupted) length should be too large, we limit it to end of the file system
    //
    if (((CAST_POINTER_ARITHMETIC)((unsigned char *)(uFILE_START + FILE_SYSTEM_SIZE - FILE_HEADER)) - (CAST_POINTER_ARITHMETIC)ptrfile) < FileLength) {
        FileLength = (MAX_FILE_LENGTH)((CAST_POINTER_ARITHMETIC)(unsigned char *)(uFILE_START + FILE_SYSTEM_SIZE - FILE_HEADER) - (CAST_POINTER_ARITHMETIC)ptrfile); // the space remaining
    }

    return FileLength;
}
#endif


#if defined (_WINDOWS) || defined (_LITTLE_ENDIAN)
static void fnHeaderToLittleEndian(UPLOAD_HEADER *file_header)
{
    unsigned short usShort;
    unsigned long  ulLong;

    usShort = (file_header->usCRC >> 8);
    file_header->usCRC <<= 8;
    file_header->usCRC |= usShort;

    usShort = (file_header->usMagicNumber >> 8);
    file_header->usMagicNumber <<= 8;
    file_header->usMagicNumber |= usShort;

    ulLong = (file_header->ulCodeLength >> 24);
    ulLong |= (((file_header->ulCodeLength >> 16) & 0xff) << 8);
    ulLong |= (((file_header->ulCodeLength >> 8) & 0xff) << 16);
    ulLong |= (((file_header->ulCodeLength) & 0xff) << 24);
    file_header->ulCodeLength = ulLong;

    #if defined _ENCRYPTED
    usShort = (file_header->usRAWCRC >> 8);
    file_header->usRAWCRC <<= 8;
    file_header->usRAWCRC |= usShort;
    #endif
}
#endif

// Check whether there is new code waiting and that it has a valid header and check sum
//
static int fnCheckNewCode(UPLOAD_HEADER *file_header)
{
#if defined NO_UFILE_HEADER                                              // {26}
    #if defined MULTIPLE_INTERMEDIATE_CODE_LOCATIONS                     // {24}
    unsigned char *ptrFile = (file_header->ptrCodeLocation->ptrCodeAddress + SIZE_OF_UPLOAD_HEADER);
    #else
    unsigned char *ptrFile = (UPLOAD_FILE_LOCATION + SIZE_OF_UPLOAD_HEADER);
    #endif
#else
    #if defined MULTIPLE_INTERMEDIATE_CODE_LOCATIONS                     // {24}
    unsigned char *ptrFile = (file_header->ptrCodeLocation->ptrCodeAddress + FILE_HEADER + SIZE_OF_UPLOAD_HEADER);
    #else
    unsigned char *ptrFile = (UPLOAD_FILE_LOCATION + FILE_HEADER + SIZE_OF_UPLOAD_HEADER);
    #endif
#endif
    MAX_FILE_LENGTH file_length = uGetFileLength(UPLOAD_FILE_LOCATION);
    unsigned short usCRC;
    unsigned char ucCodeBlock[BLOCK_SIZE];
    unsigned short usBlockSize;
#if defined NO_UFILE_HEADER                                              // {26}
    fnGetPars(UPLOAD_FILE_LOCATION, (unsigned char*)file_header, SIZE_OF_UPLOAD_HEADER);
#else
    fnGetPars((ptrFile - SIZE_OF_UPLOAD_HEADER), (unsigned char*)file_header, SIZE_OF_UPLOAD_HEADER);
#endif
#if defined (_WINDOWS) || defined (_LITTLE_ENDIAN)                       // {1}
    fnHeaderToLittleEndian(file_header);
#endif

    if ((file_header->usMagicNumber != VALID_VERSION_MAGIC_NUMBER)) {    // first test that the header version (magic number) is correct
#if defined ADAPTABLE_PARAMETERS                                         // {22}
        if ((file_header->usMagicNumber != VALID_VERSION_MAGIC_NUMBER_2)) { // check whether second code destination
            return 0;                                                    // no match found
        }
        file_header->usDestinationType = 2;                              // second destination
#else
        return 0;                                                        // magic number doesn't match so ignore
#endif
    }
#if defined ADAPTABLE_PARAMETERS                                         // {22}
    else {
        file_header->usDestinationType = 1;                              // first destination
    }
#endif
#if !defined NO_UFILE_HEADER                                             // {26}
    if (file_header->ulCodeLength != (file_length - SIZE_OF_UPLOAD_HEADER)) { // now test that the file length and data length correspond
        return 0;
    }
#endif
#if defined ADAPTABLE_PARAMETERS                                         // {22}
    if (!file_header->ulCodeLength) {                                    // ignore zero contents or if too large
        return 0;
    }
    if (file_header->usDestinationType == 2) {
    #if defined MULTIPLE_INTERMEDIATE_CODE_LOCATIONS                     // {24}
        if (file_header->ulCodeLength > (file_header->ptrCodeLocation->ulMaxApplicationSize)) { // ignore if the application length is unrealistically large
            return 0; 
        }
    #else
        if (file_header->ulCodeLength > UTASK_APP2_LENGTH) {             // ignore if the application length is unrealistically large
            return 0; 
        }
    #endif
    }
    else {
        if (file_header->ulCodeLength > UTASK_APP_LENGTH) {             // ignore if the application length is unrealistically large
            return 0; 
        }
    }
#else
    if (!file_header->ulCodeLength || (file_header->ulCodeLength > UTASK_APP_LENGTH)) { // ignore zero contents or if too large
        return 0; 
    }
#endif
    file_length = (MAX_FILE_LENGTH)file_header->ulCodeLength;            // finally check the check sum of the code
    usBlockSize = BLOCK_SIZE;                                            // check in blocks of this size
    usCRC = 0;
    while (file_length) {
        if (file_length < BLOCK_SIZE) {
            usBlockSize = (unsigned short)file_length;
        }
        fnGetPars(ptrFile, ucCodeBlock, usBlockSize);                    // retrieve a block from the intermediate storage area
        usCRC = fnCRC16(usCRC, ucCodeBlock, usBlockSize);                // perform CRC-16 over the block
        ptrFile += usBlockSize;                                          // advance the source pointer
        file_length -= usBlockSize;
    }
#if !defined NO_SECRET_KEY
    #if defined ADAPTABLE_PARAMETERS                                     // {22}
    if (file_header->usDestinationType == 2) {
        usCRC = fnCRC16(usCRC, (unsigned char *)ucKey2, sizeof(ucKey2)); // add the secret key belonging to the second destination type
    }
    else {
        usCRC = fnCRC16(usCRC, (unsigned char *)ucKey, sizeof(ucKey));   // add the secret key
    }
    #else
    usCRC = fnCRC16(usCRC, (unsigned char *)ucKey, sizeof(ucKey));       // add the secret key
    #endif
#endif
    return (usCRC == file_header->usCRC);                                // the code is valid if the CRC is correct
}


#if defined SUPPORT_VERIFY_APP_BEFORE_START
// Check that the code at the application location is present and valid
//
static int fnCheckValidCode(void)
{
    return 0;
}
#endif

#if defined SUPPORT_FORCE_BOOT
// Check possible methods to force boot mode even when the application is ready to run
//
static int fnCheckForceBoot(void)
{
    return 0;
}
#endif
