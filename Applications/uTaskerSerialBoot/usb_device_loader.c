/**********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher

    ---------------------------------------------------------------------
    File:      usb_device_loader.c - USB-based firmware loading
    Project:   uTasker Demonstration project
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2019
    *********************************************************************
    17.05.2011 Official USB VID/PID for Freescale MSD                    {1}
    31.05.2011 Add optional password protection on SW uploads            {2}
    18.07.2011 Use SLCD together with Kinetis K40                        {3}
    06.01.2012 Allow USB powered configuration                           {4}
    22.01.2012 Rename some local functions to avoid conflict with SD card module when SD card loader is used {5}
    03.03.2012 Add TWR_K53N512 support                                   {6}
    28.02.2013 Add HS USB                                                {7}
    03.03.2013 Correct FAT12 cluster chain termination                   {8}
    03.03.2013 Add fnAddSREC_file() when both USB-MSD and SREC loading enabled together {9}
    06.06.2013 Add USB_SPEC_VERSION default                              {10}
    17.01.2014 Set formatted disk flag                                   {11}
    17.01.2014 Allow uploads to overwrite existing file as long as the name is the same {12}
    18.01.2014 Partly revert change {8} since it limited file length     {13}
    18.01.2014 Reset FAT12 content variables to allow multiple use       {14}
    07.04.2014 Add Windows 8.1 workaround                                {15}
    24.04.2014 Add HID_LOADER                                            {16}
    25.05.2014 Correct FAT12 setting                                     {17}
    03.06.2014 Use fnAddSREC_file() together with web loader             {18}
    11.06.2014 Add KBOOT_HID_LOADER mode                                 {19}
    12.07.2014 Move KBOOT handler to Loader.c so that it is shared by HID and UART modes {20}
    29.11.2014 Adapted Windows 8.1 workaround to keep upload file information {21}
    29.11.2014 Improve recognition of a file object delete               {22}
    08.01.2015 Correct FAT12 empty FAT value so that it doesn't disturb MAC PCs {23}
    29.01.2015 Protect against over-sized long file names                {24}
    06.02.2015 Include workaround for MAC OS X compatibility             {25}
    17.02.2015 Extend with optional second LUM                           {26}
    23.03.2015 Adjust handling of final flash sector (when writing and reading) to allow it to be used to the end of the application space {27}
    27.09.2015 Correct backup of root sectors                            {28}
    23.10.2015 Added optional operation together with host mode loading [renamed to usb_device_loader.c] {29}
    05.11.2015 Add volume entry in root directory to ensure that the content is displayed by all host OSs {30}
    20.11.2015 Add USB-CDC option to allow SREC loading via virtual COM  {31}
    28.03.2019 Add option to start the application in KBOOT mode when there is no enumeration within time KBOOT_HID_ENUMERATION_LIMIT
    28.03.2019 Add option to start the application in KBOOT mode when there is no command received within time KBOOT_COMMAND_LIMIT

*/


/* =================================================================== */
/*                           include files                             */
/* =================================================================== */

#include "config.h"

#if defined USB_INTERFACE && !defined USE_USB_MSD && (!defined USB_MSD_HOST_LOADER || defined USB_MSD_DEVICE_LOADER || defined USE_USB_CDC)

/* =================================================================== */
/*                          local definitions                          */
/* =================================================================== */

//#define DEBUG_CODE                                                     // acivate some debug ouput to monitor operation
//#define RESET_ON_SUSPEND                                               // reset when the USB cable is pulled
//#define RESET_ON_EJECT                                                 // reset when the drive is ejected

#if !defined DISK_COUNT
    #define DISK_COUNT 1                                                 // {26}
#endif

#define NO_BLOCKING_PERIOD                                               // test removal of blocking period due to improved filtering
#define WINDOWS_8_1_WORKAROUND                                           // {15} from Windows 8.1 the PC writes some hidden file information to newly attached USB drives, which is uses to detect whether it has been attached before (enabling fast search, for example)
                                                                         // since this looks like a software upload starting the boot loader ignores writes for a period of 5s after enumeration - after this period has expired an upload can be started and will be recognised
#define MAC_OS_X_WORKAROUND                                              // {25} filter hidden content write that can occur together with file copies
#if defined NO_BLOCKING_PERIOD
    #define AUTO_DELETE_ON_ANY_FIRMWARE                                  // allow dropping any file name (with software content) to delete existing file and ensure that MAC overwrites operate in all cases
#endif

#define OWN_TASK                           TASK_USB

#if !defined USB_SPEC_VERSION                                            // {10}
    #define USB_SPEC_VERSION               USB_SPEC_VERSION_1_1          // default is to report USB1.1 since it is equivalent to USB2.0 but requires one less descriptor exchange
#endif

#if defined HID_LOADER && defined USB_MSD_DEVICE_LOADER                  // USB HID and MSD composite
    #if defined KBOOT_HID_LOADER                                         // {19}
        #if defined USB_SIMPLEX_ENDPOINTS
            #define NUMBER_OF_ENDPOINTS    3                             // uses two endpoints for KBOOT and one for MSD
        #else
            #define NUMBER_OF_ENDPOINTS    4                             // uses two endpoints for KBOOT and two for MSD
        #endif
    #else
        #if defined USB_SIMPLEX_ENDPOINTS
            #define NUMBER_OF_ENDPOINTS    2                             // uses one endpoints for HID and one for MSD
        #else
            #define NUMBER_OF_ENDPOINTS    3                             // uses one endpoint for HID and two for MSD
        #endif
    #endif
#elif defined HID_LOADER
    #if defined KBOOT_HID_LOADER                                         // {19}
        #define NUMBER_OF_ENDPOINTS        2                             // uses 1 IN and 2 OUT interrupt endpoints
    #else
        #define NUMBER_OF_ENDPOINTS        1                             // uses just 1 IN interrupt endpoint
    #endif
#elif defined USE_USB_CDC
    #define NUMBER_OF_ENDPOINTS            3                             // uses 1 IN and 2 OUT bulk enpoints endpoints as well as 3 interrupt endpoint
#else
    #define NUMBER_OF_ENDPOINTS            2                             // USB-MSD uses 2 endpoints (1 IN and 1 OUT) in addition to the default control endpoint 0
#endif

#define NUMBER_OF_POSSIBLE_CONFIGURATIONS  1

#if defined _M5223X || defined _KINETIS
    #if defined HID_LOADER
        #if defined KBOOT_HID_LOADER                                     // {19}
            #define USB_VENDOR_ID          0x15a2                        // Freescale vendor ID
            #define USB_PRODUCT_ID         0x0073                        // product ID
            #define USB_MSD_OUT_ENDPOINT_NUMBER 3
            #if defined USB_SIMPLEX_ENDPOINTS
                #define USB_MSD_IN_ENDPOINT_NUMBER  3                    // IN shared OUT endpoint
            #else
                #define USB_MSD_IN_ENDPOINT_NUMBER  4
            #endif
        #else
            #define USB_VENDOR_ID          0x0425                        // Motorola vendor ID
            #define USB_PRODUCT_ID         0x0400                        // product ID
            #define USB_MSD_OUT_ENDPOINT_NUMBER 2
            #define USB_MSD_IN_ENDPOINT_NUMBER  3
        #endif
    #elif defined USE_USB_CDC                                            // {31}
        #define USB_VENDOR_ID              0x15a2                        // Freescale vendor ID
        #define USB_PRODUCT_ID             0x0044                        // uTasker Freescale development CDC product ID
    #else
        #define USB_VENDOR_ID              0x0425                        // MOTOROLA vendor ID {1}
        #define USB_PRODUCT_ID             0x03fc                        // uTasker Motorola MSD development product ID {1}
        #define USB_MSD_OUT_ENDPOINT_NUMBER     1
        #define USB_MSD_IN_ENDPOINT_NUMBER      2
    #endif
#elif defined _LM3SXXXX
    #define USB_VENDOR_ID                  0x1cbe                        // Luminary Micro, Inc. vendor ID
    #define USB_PRODUCT_ID                 0x1234                        // non-official test MSD PID
    #define USB_MSD_OUT_ENDPOINT_NUMBER         1
    #define USB_MSD_IN_ENDPOINT_NUMBER          2
#elif defined _HW_SAM7X || defined _HW_AVR32
    #define USB_VENDOR_ID                  0x03eb                        // ATMEL Corp. vendor ID
    #define USB_PRODUCT_ID                 0x1234                        // non-official test MSD PID
    #define USB_MSD_OUT_ENDPOINT_NUMBER         1
    #define USB_MSD_IN_ENDPOINT_NUMBER          2
#else
    #define USB_VENDOR_ID                  0x4321                        // non-official test VID
    #define USB_PRODUCT_ID                 0x1234                        // non-official test MSD PID
    #define USB_MSD_OUT_ENDPOINT_NUMBER         1
    #define USB_MSD_IN_ENDPOINT_NUMBER          2
#endif


#define USB_PRODUCT_RELEASE_NUMBER         0x0100                        // V1.0 (binary coded decimal)

#if defined USB_STRING_OPTION                                            // if our project supports strings
    #define MANUFACTURER_STRING_INDEX      1                             // index must match with order in the string list
    #define PRODUCT_STRING_INDEX           2                             // to remove a particular string from the list set to zero
    #define SERIAL_NUMBER_STRING_INDEX     3
    #define CONFIGURATION_STRING_INDEX     4
    #define INTERFACE_STRING_INDEX         5

    #define UNICODE_LANGUAGE_INDEX         UNICODE_ENGLISH_LANGUAGE      // English language used by strings
    #define LAST_STRING_INDEX              INTERFACE_STRING_INDEX        // last string entry - used for protection against invalid string index requests
#endif

// USB-MSD defines
//
#if !defined EMULATED_FAT_LUMS
    #define EMULATED_FAT_LUMS              1
#endif
#if !defined ROOT_DIR_SECTORS
    #define ROOT_DIR_SECTORS               1
#endif
#define NUMBER_OF_PARTITIONS               EMULATED_FAT_LUMS             // {26}

#if defined SPECIAL_VERSION
    #if EMULATED_FAT_LUMS > 1
        #define DISK_NAME                  "LPT_DISK1"
        #define DISK_NAME_INC              7
    #else
        #define DISK_NAME                  "LPT_DISK"
    #endif
#else
    #if EMULATED_FAT_LUMS > 1
        #define DISK_NAME                  "UPLOAD_DSK1"
        #define DISK_NAME_INC              10
    #else
        #define DISK_NAME                  "UPLOAD_DISK"
    #endif
#endif

#define DIR_NAME_FREE                      0xe5

#if !defined SDCARD_SUPPORT && !defined SPI_FLASH_FAT && !defined USB_MSD_HOST_LOADER && !defined FAT_EMULATION
    #define FIXED_MEDIA                    0xf8
    #define REMOVABLE_MEDIA                0xf0

    #define DIR_ATTR_READ_ONLY             0x01
    #define DIR_ATTR_HIDDEN                0x02
    #define DIR_ATTR_SYSTEM                0x04
    #define DIR_ATTR_VOLUME_ID             0x08
    #define DIR_ATTR_DIRECTORY             0x10
    #define DIR_ATTR_ARCHIVE               0x20
    #define DIR_ATTR_LONG_NAME             (DIR_ATTR_READ_ONLY | DIR_ATTR_HIDDEN | DIR_ATTR_SYSTEM | DIR_ATTR_VOLUME_ID)
    #define DIR_ATTR_MASK                  (DIR_ATTR_LONG_NAME | DIR_ATTR_DIRECTORY | DIR_ATTR_ARCHIVE)

    #define WRITEBACK_BUFFER_FLAG          0x0001                        // flag that the present sector buffer has been changed and so needs to be physically written back to the card
    #define WRITEBACK_INFO_FLAG            0x0002                        // flag that there has been a change made to the card which need its info block updated too
    #define FSINFO_VALID                   0x0004                        // the disk has a valid info block which can be used to accelerate some calculations
    #define DISK_UNFORMATTED               0x0008                        // disk detected but its content is not formatted
    #define WRITE_PROTECTED_SD_CARD        0x0010                        // the SD card has write protection active and so no write operations are allowed
    #define DISK_FORMATTED                 0x0020                        // the disk has been detected and is formatted
    #define HIGH_CAPACITY_SD_CARD          0x0040                        // the disk is of high capacity type
    #define DISK_MOUNTED                   0x0080                        // the disk has been mounted and so is ready for use
    #define DISK_NOT_PRESENT               0x0100                        // a check of the disk failed to identify its presence
    #define DISK_TYPE_NOT_SUPPORTED        0x0200                        // unsupported disk type detected
    #define DISK_FORMAT_FULL               0x2000                        // set all content to 0x00 rather than just all FAT
    #define DISK_FORMAT_FAT16              0x4000                        // FAT16 format rather than FAT32
    #define DISK_TEST_MODE                 0x8000                        // special flag to control testing (development tests)
#endif

#if defined SDCARD_SUPPORT || defined SPI_FLASH_FAT || defined USB_MSD_HOST_LOADER && !defined FAT_EMULATION // {5}
    #undef BOOT_SECTOR_LOCATION
    #undef BACKUP_ROOT_SECTOR
    #undef RESERVED_SECTION_COUNT
    #undef NUMBER_OF_FATS
#elif !defined FAT_EMULATION
    #define UTFAT_SUCCESS_PATH_MODIFIED        5
    #define UTFAT_PATH_IS_ROOT                 4
    #define UTFAT_PATH_IS_ROOT_REF             3
    #define UTFAT_PATH_IS_FILE                 2                         // the referenced object is a file and not a directory
    #define UTFAT_END_OF_DIRECTORY             1
    #define UTFAT_SUCCESS                      0
    #define UTFAT_DIR_NOT_EMPTY                -1
    #define UTFAT_DISK_NOT_READY               -2                        // disk not ready for use - not formatted or not mounted
    #define UTFAT_PATH_NOT_FOUND               -3                        // the referenced directory path could not be found
    #define UTFAT_DISK_READ_ERROR              -4                        // error occurred while trying to read a sector from the disk
    #define UTFAT_DISK_WRITE_ERROR             -5
    #define UTFAT_FILE_NOT_FOUND               -6                        // the referenced file was could not be found
    #define UTFAT_DIRECTORY_AREA_EXHAUSTED     -7                        // the end of the FAT space was reached and no valid clusters found
    #define UTFAT_FAT16_ROOT_FOLDER_EXHAUSTED  -8                        // the FAT16 root folder can hold up to 512 items and not more - once this has been use up no more can be located in the root folder (note that LFNs can count as multiple items and restrict further then number of real items)
    #define UTFAT_FILE_NOT_READABLE            -9                        // the file is not opened in read mode
    #define UTFAT_FILE_NOT_WRITEABLE           -10                       // the file cannot be written because it is either not opened in write mode, is marked as a read-only file on the disk or writes are being blocked by another user
    #define UTFAT_FILE_LOCKED                  -11                       // the file could not be opened since it is locked for exclusive use by another user
    #define UTFAT_SEARCH_INVALID               -12                       // a file search was invalid since the file object is not associated with a directory object
    #define UTFAT_DISK_ALREADY_FORMATTED       -13
    #define UTFAT_NO_MORE_LISING_SPACE         -14
    #define UTFAT_NO_MORE_LISTING_ITEMS_FOUND  -15
    #define UTFAT_FINAL_LISTING_ITEM_FOUND     -16
    #define UTFAT_DIRECTORY_OBJECT_MISSING     -17
    #define UTFAT_DIRECTORY_EXISTS_ALREADY     -18
    #define UTFAT_DISK_WRITE_PROTECTED         -19
    #define MANAGED_FILE_NO_ACCESS             -20
    #define MANAGED_FILE_NO_FILE_HANDLE        -21                       // no space is available for a managed file
#endif

#if !defined FAT_EMULATION
    #define UTFAT12                                                      // since small disk sizes are involved FAT12 is used - this also ensures that Windows XP can work with it

    #if defined UTFAT12
        #define BOOT_SECTOR_LOCATION           1
        #define BACKUP_ROOT_SECTOR             1
        #define BYTES_PER_SECTOR               512
        #define RESERVED_SECTION_COUNT         2
        #define NUMBER_OF_FATS                 1
    #else
        #define BOOT_SECTOR_LOCATION           63
        #define BACKUP_ROOT_SECTOR             6
        #define BYTES_PER_SECTOR               512
        #define RESERVED_SECTION_COUNT         32
        #define NUMBER_OF_FATS                 1
    #endif
#endif

#define BAD_CLUSTER_VALUE                  0x0ffffff7
#define CLUSTER_MASK                       0x0fffffff
#define MEDIA_VALUE_FIXED                  0x0ffffff8
#define MEDIA_VALUE_REMOVABLE              0x0ffffff0

#define FAT12_CLUSTER_MASK                 0x00000fff

#undef DISK_C
#undef DISK_D
#define DISK_C                             0
#define DISK_D                             1

#define DISK_SIZE                          (1024 * 1024)                 // 1Meg - up to 2Meg with FAT12

#define PARTITION_SIZE                     ((DISK_SIZE/BYTES_PER_SECTOR) - BOOT_SECTOR_LOCATION)

#define SW_EMPTY                           0
#define SW_PROGRAMMING                     1
#define SW_AVAILABLE                       2

#define TIMEOUT_RESET_NOW                  1
#define TIMEOUT_ACCEPT_UPLOAD              2
#define TIMEOUT_USB_ENUMERATION            3
#define TIMEOUT_USB_LOADING_COMPLETE_C     4
#define TIMEOUT_USB_LOADING_COMPLETE_D     (TIMEOUT_USB_LOADING_COMPLETE_C + 1)

#define UNKNOWN_CONTENT                    0
#define FIRMWARE_START_CONTENT             1
#define HIDDEN_FILE_CLUSTER                2
#define WINDOWS_HIDDEN_DATA_CONTENT        3
#define MAC_HIDDEN_DATA_CONTENT            4

#if defined FAT_EMULATION
    #define APPLICATION_DATA_FILES           MAXIMUM_DATA_FILES
    #define FORMAT_TYPE_INVALID_FILE         0
    #define FORMAT_TYPE_RAW_BINARY           1
    #define FORMAT_TYPE_RAW_BINARY_PROTECTED 2
    #define FORMAT_TYPE_RAW_STRING           3
#endif


// HID loader defines
//
#define HID_IDENTIFIER                     0xa5
#define HID_REPORT_IN                      0x5a
#define HID_REPORT_OK                      0x00

#define HID_BOOT_ERASE_SECTOR              0x01
#define HID_BOOT_WRITE_BUFFER_TO_FLASH     0x12
#define HID_BOOT_WRITE_BUFFER              0x13

// Interrupt events
//
#define HID_LOADER_ERASE                   1                             // local events
#define HID_LOADER_PROGRAM                 2
#define EVENT_RETURN_PRESENT_UART_SETTING  10


/* =================================================================== */
/*                       local structure definitions                   */
/* =================================================================== */

#if defined _CODE_WARRIOR_CF                                             // ensure no padding in structs used in this file
    #pragma pack(1)
#endif


// We define the contents of the configuration descriptor used for our specific device and then set its contents
//
typedef struct _PACK stUSB_CONFIGURATION_DESCRIPTOR_COLLECTION
{
#if defined USB_MSD_DEVICE_LOADER && defined HID_LOADER                  // composite USB-MSD and Kboot compatible HID loader
    USB_CONFIGURATION_DESCRIPTOR               config_desc;              // compulsory configuration descriptor
    USB_INTERFACE_DESCRIPTOR                   interface_desc_0;         // first interface descriptor
    USB_HID_DESCRIPTOR                         hid_desc_1;               // HID descriptor
    USB_ENDPOINT_DESCRIPTOR                    endpoint_1;               // interrupt IN endpoint
    #if defined KBOOT_HID_LOADER
    USB_ENDPOINT_DESCRIPTOR                    endpoint_2;               // interrupt OUT endpoint
    USB_INTERFACE_DESCRIPTOR                   interface_desc_1;         // second interface descriptor
    USB_ENDPOINT_DESCRIPTOR                    endpoint_3;               // end points of second interface
    USB_ENDPOINT_DESCRIPTOR                    endpoint_4;
    #else
    USB_INTERFACE_DESCRIPTOR                   interface_desc_1;         // first interface descriptor
    USB_ENDPOINT_DESCRIPTOR                    endpoint_2;               // end points of second interface
    USB_ENDPOINT_DESCRIPTOR                    endpoint_3;
    #endif
#elif defined HID_LOADER                                                 // {16}
    USB_CONFIGURATION_DESCRIPTOR               config_desc;              // compulsory configuration descriptor
    USB_INTERFACE_DESCRIPTOR                   interface_desc_0;         // first interface descriptor
    USB_HID_DESCRIPTOR                         hid_desc_1;               // HID descriptor
    USB_ENDPOINT_DESCRIPTOR                    endpoint_1;               // interrupt IN endpoint
    #if defined KBOOT_HID_LOADER                                         // {19}
    USB_ENDPOINT_DESCRIPTOR                    endpoint_2;               // interrupt OUT endpoint
    #endif
#elif defined USE_USB_CDC                                                // {31}
    USB_CONFIGURATION_DESCRIPTOR               config_desc_cdc;          // compulsory configuration descriptor
                                                                         // the interface association descriptor is use as standard so that it achieves the same endpoint usage whether multiple CDC interfaces are used or not
    USB_INTERFACE_ASSOCIATION_DESCRIPTOR       cdc_interface_0;          // if there are multiple CDC interfaces and interface association descriptor is required

    USB_INTERFACE_DESCRIPTOR                   interface_desc_0;         // interface descriptor
        USB_CDC_FUNCTIONAL_DESCRIPTOR_HEADER   CDC_func_header_0;        // CDC function descriptors due to class used
        USB_CDC_FUNCTIONAL_DESCRIPTOR_CALL_MAN CDC_call_management_0;
        USB_CDC_FUNCTIONAL_DESCRIPTOR_ABSTRACT_CONTROL  CDC_abstract_control_0;
        USB_CDC_FUNCTIONAL_DESCRIPTOR_UNION    CDC_union_0;
    USB_ENDPOINT_DESCRIPTOR                    endpoint_3;               // endpoint of first interface

    USB_INTERFACE_DESCRIPTOR                   interface_desc_1;         // second interface descriptor
    USB_ENDPOINT_DESCRIPTOR                    endpoint_1;               // endpoints of second interface
    USB_ENDPOINT_DESCRIPTOR                    endpoint_2;
#else                                                                    // USB-MSD
    USB_CONFIGURATION_DESCRIPTOR               config_desc;              // compulsory configuration descriptor
    USB_INTERFACE_DESCRIPTOR                   interface_desc_0;         // first interface descriptor
    USB_ENDPOINT_DESCRIPTOR                    endpoint_1;               // end points of second interface
    USB_ENDPOINT_DESCRIPTOR                    endpoint_2;
#endif
} USB_CONFIGURATION_DESCRIPTOR_COLLECTION;


#if defined FAT_EMULATION
    #define APPLICATION_DATA_FILES           MAXIMUM_DATA_FILES
    #define FORMAT_TYPE_INVALID_FILE         0
    #define FORMAT_TYPE_RAW_BINARY           1
    #define FORMAT_TYPE_RAW_BINARY_PROTECTED 2
    #define FORMAT_TYPE_RAW_STRING           3

    typedef struct stDATA_FILE_INFORMATION
    {
        unsigned long ulDataFileLength;
        const unsigned char *ptrFileLocation;
        #if defined EMULATED_FAT_FILE_NAME_CONTROL
            const CHAR *ptrFileName;                                         // short file name (8:3 format) or LFN (when FAT_EMULATION_LFN is enabled) [leave at 0 for default name]
        #endif
        #if defined EMULATED_FAT_FILE_DATE_CONTROL
            unsigned short usCreationTime;
            unsigned short usCreationDate;                                   // [leave at 0 for fixed date/time stamp]
        #endif
        unsigned char ucFormatType;
        #if defined EMULATED_FAT_FILE_NAME_CONTROL
            unsigned char ucNameFormat;
        #endif
    } DATA_FILE_INFORMATION;
#elif !defined SDCARD_SUPPORT && !defined SPI_FLASH_FAT && !defined USB_MSD_HOST_LOADER // {5} if SD card is used as well these local struct definitions are not required
// Use part of utFAT code to behave as a virtual disk
//
typedef struct stUTFAT
{
    unsigned long  ulFatSize;                                            // sectors occupied by ONE FAT
    unsigned long  ulFAT_start;                                          // the sector in which the first FAT starts (further FAT copies start at this sector + FAT size)
    unsigned long  ulClusterCount;                                       // total number of clusters in the cluster area
    unsigned short usBytesPerSector;                                     // the number of bytes in a single sector (512, 1024, 2048 or 4096)
    unsigned char  ucNumberOfFATs;                                       // the number of FATs (more that 1 are copies)
    unsigned char  ucSectorsPerCluster;                                  // the number of sectors in a single cluster - cluster size is sector size x sectors per cluster
} UTFAT;

typedef struct stFILEINFO
{
    unsigned long ulInfoSector;                                          // sector where the information is located
    unsigned long ulFreeClusterCount;
    unsigned long ulNextFreeCluster;
    unsigned char ucCardSpecificData[16];                                // CSD register content
} FILEINFO;

typedef struct stUTDISK
{
    unsigned long  ulPresentSector;                                      // the present sector being used by the disk
    unsigned long  ulDirectoryBase;                                      // the first cluster in the root directory (usually 2)
    unsigned long  ulLogicalBaseAddress;                                 // first cluster containing data
    unsigned long  ulVirtualBaseAddress;                                 // virtual cluster starting address, compensating unused clusters
    unsigned long  ulSD_sectors;                                         // physical sectors on the device
    unsigned char  *ptrSectorData;                                       // pointer to a buffer containing a copy of the sector data
    unsigned short usDiskFlags;                                          // flags indicating the status of the disk
    UTFAT          utFAT;                                                // FAT information concerning the data content
    FILEINFO       utFileInfo;                                           // file information used by FAT32
    unsigned char  ucDriveNumber;                                        // the drive number of this disk
    CHAR           cVolumeLabel[11];                                     // the volume's label
} UTDISK;

typedef struct stPARTITION_TABLE_ENTRY
{
    unsigned char boot_indicator;                                        // 0x80 indicates bootable
    unsigned char starting_cylinder;                                     // cylinder start value
    unsigned char starting_head;                                         // head start value
    unsigned char starting_sector;                                       // sector start value
    unsigned char partition_type;                                        // partition type descriptor
    unsigned char ending_cylinder;                                       // cylinder start value
    unsigned char ending_head;                                           // head start value
    unsigned char ending_sector;                                         // sector start value
    unsigned char start_sector[4];                                       // start sector
    unsigned char partition_size[4];                                     // partition size in sectors
} PARTITION_TABLE_ENTRY;

typedef struct _PACK stEXTENDED_BOOT_RECORD
{
    unsigned char         EBR_unused1[394];                              // generally 0
    unsigned char         EBR_IBM_menu[9];                               // possible IBM boot manager menu entry
    unsigned char         EBR_unused2[43];                               // generally 0
    PARTITION_TABLE_ENTRY EBR_partition_table[2];                        // two partition tables
    unsigned char         EBR_unused3[32];                               // generally 0
    unsigned char         ucCheck55;                                     // this location must be 0x55 - offset 510
    unsigned char         ucCheckAA;                                     // this location must be 0xaa - offset 511
} EXTENDED_BOOT_RECORD;

typedef struct stBOOT_SECTOR_BPB                                         // boot sector and bios parameter block
{
    unsigned char BS_jmpBoot[3];                                         // jump instruction to boot code
    CHAR          BS_OEMName[8];                                         // string usually indicating the system that formatted the volume - "MSWIN4.1" is recommended although MSDOS5.0 is typical
    unsigned char BPB_BytesPerSec[2];                                    // count of bytes per sector. This value may take on only the following values: 512, 1024, 2048 or 4096
    unsigned char BPB_SecPerClus;                                        // number of sectors per allocation unit. The legal values are 1, 2, 4, 8, 16, 32, 64, and 128 - however never cause a "bytes per cluster" value (BPB_BytesPerSec * BPB_SecPerClus) greater than 32K!!
    unsigned char BPB_RsvdSecCnt[2];                                     // number of reserved sectors in the reserved region of the volume starting at the first sector of the volume. Never 0. FAT12/16 always 1. FAT32 uses typically 32
    unsigned char BPB_NumFATs;                                           // the count of FAT data structures on the volume. Recommended to be always 2 (although FLASH could use 1)
    unsigned char BPB_RootEntCnt[2];                                     // FAT12 and FAT16 volumes count of 32-byte directory entries in the root directory. FAT32 must always be 0. FAT16 should use 512. When multiplied by 32 it should result in an even multiple of BPB_BytesPerSec (for FAT12 and FAT16)
    unsigned char BPB_TotSec16[2];                                       // old 16-bit total count of sectors on the volume (in all four regions of the volume). May be zero if BPB_TotSec32 is non-zero. Must be 0 for FAT32
    unsigned char BPB_Media;                                             // 0xF0, 0xF8, 0xF9, 0xFA, 0xFB, 0xFC, 0xFD, 0xFE, or 0xFF. 0xF8 usually used for fixed and 0xF0 for removable media. Should match with FAT[0] entry but is otherwise obsolete
    unsigned char BPB_FATSz16[2];                                        // FAT12/FAT16 16-bit count of sectors occupied by ONE FAT. Must be 0 for FAT32
    unsigned char BPB_SecPerTrk[2];                                      // sectors per track for interrupt 0x13. Only valid for media whose volume is broken down into tracks by multiple heads and cylinders
    unsigned char BPB_NumHeads[2];                                       // number of heads for interrupt 0x13.
    unsigned char BPB_HiddSec[4];                                        // count of hidden sectors preceding the partition that contains this FAT volume. Should always be zero on media that are not partitioned but otherwise operating system dependent
    unsigned char BPB_TotSec32[4];                                       // 32-bit total count of sectors on the volume (all sectors in all four regions). Can be zero if BPB_TotSec16 is non-zero. Must be non-zero for FAT32
} BOOT_SECTOR_BPB;

typedef struct _PACK stBOOT_SECT_COM
{
    unsigned char BS_DrvNum;                                             // int 0x13 drive number - operating system specific
    unsigned char BS_Reserved1;                                          // should always be set to zero when formatting (is in fact used by Windows NT)
    unsigned char BS_BootSig;                                            // extended boot signature (0x29). Signature indicating that the following three fields are present
    unsigned char BS_VolID[4];                                           // volume serial number. Usually generated by simply combining the current date and time into a 32-bit value
    CHAR          BS_VolLab[11];                                         // label matching the 11-byte volume label recorded in the root directory. "NO NAME    " when no specific label
    CHAR          BS_FilSysType[8];                                      // "FAT12   ", "FAT16   ", or "FAT     ". Not actually used to determine type (more informational) and not used at all by Microsoft FAT
} BOOT_SECT_COM;

typedef struct _PACK stBOOT_SECTOR_FAT32
{
    BOOT_SECTOR_BPB boot_sector_bpb;                                     // standard boot sector and bios parameter block up to offset 36
    unsigned char BPB_FATSz32[4];                                        // FAT32 32-bit count of sectors occupied by ONE FAT - BPB_FATSz16 must be zero!
    unsigned char BPB_ExtFlags[2];
    unsigned char BPB_FSVer[2];                                          // version number of the FAT32 volume. major:minor - 0:0 expected at the time of writing but could change in the future indicating changes
    unsigned char BPB_RootClus[4];                                       // cluster number of the first cluster of the root directory. Usually 2
    unsigned char BPB_FSInfo[2];                                         // sector number of FSINFO structure in the reserved area of the FAT32 volume. Usually 1
    unsigned char BPB_BkBootSec[2];                                      // sector number in the reserved area of the volume of a copy of the boot record (if non-zero). 6 is recommended
    unsigned char BPB_Reserved[12];                                      // should be 0
    BOOT_SECT_COM bs_common;                                             // common info in FAT16 and FAT32 but shifted in location   
/*  unsigned char BS_DrvNum;                                             // int 0x13 drive number - operating system specific
    unsigned char BS_Reserved1;                                          // should always be set to zero when formatting (is in fact used by Windows NT)
    unsigned char BS_BootSig;                                            // extended boot signature (0x29). Signature indicating that the following three fields are present
    unsigned char BS_VolID[4];                                           // volume serial number. Usually generated by simply combining the current date and time into a 32-bit value
    CHAR          BS_VolLab[11];                                         // label matching the 11-byte volume label recorded in the root directory. "NO NAME    " when no specific label
    CHAR          BS_FilSysType[8];                                      // always "FAT32   ". Not actually used to determine type (more informational) and not used at all by Microsoft FAT */
    unsigned char ucSpace[420];
    unsigned char ucCheck55;                                             // this location must be 0x55 - offset 510
    unsigned char ucCheckAA;                                             // this location must be 0xaa - offset 511
} BOOT_SECTOR_FAT32;


typedef struct _PACK stBOOT_SECTOR_FAT12_FAT16
{
    BOOT_SECTOR_BPB boot_sector_bpb;                                     // standard boot sector and bios parameter block up to offset 36
    BOOT_SECT_COM bs_common;                                             // common info in FAT16 and FAT32 but shifted in location
/*  unsigned char BS_DrvNum;                                             // int 0x13 drive number - operating system specific
    unsigned char BS_Reserved1;                                          // should always be set to zero when formatting (is in fact used by Windows NT)
    unsigned char BS_BootSig;                                            // extended boot signature (0x29). Signature indicating that the following three fields are present
    unsigned char BS_VolID[4];                                           // volume serial number. Usually generated by simply combining the current date and time into a 32-bit value
    CHAR          BS_VolLab[11];                                         // label matching the 11-byte volume label recorded in the root directory. "NO NAME    " when no specific label
    CHAR          BS_FilSysType[8];                                      // "FAT12   ", "FAT16   ", or "FAT     ". Not actually used to determine type (more informational) and not used at all by Microsoft FAT */
    unsigned char ucSpace[448];
    unsigned char ucCheck55;                                             // this location must be 0x55 - offset 510
    unsigned char ucCheckAA;                                             // this location must be 0xaa - offset 511
} BOOT_SECTOR_FAT12_FAT16;


typedef struct stDIR_ENTRY_STRUCTURE_FAT32
{
    unsigned char DIR_Name[11];                                          // directory short name. If the first byte is 0xe5 the directory entry is free. If it is 0x00 this and all following are free. If it is 0x05 it means that the actual file name begins with 0xe5 (makes Japanese character set possible). May not start with ' ' or lower (apart from special case for 0x05) and lower case characters are not allowed. The following characters are not allowed: "	0x22, 0x2A, 0x2B, 0x2C, 0x2E, 0x2F, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F, 0x5B, 0x5C, 0x5D, and 0x7C
    unsigned char DIR_Attr;                                              // file attributes
    unsigned char DIR_NTRes;                                             // reserved for Windows NT - should be 0
    unsigned char DIR_CrtTimeTenth;                                      // millisecond stamp at file creation time. Actually contains a count of tenths of a second 0..199
    unsigned char DIR_CrtTime[2];                                        // time file was created
    unsigned char DIR_CrtDate[2];                                        // data file was created
    unsigned char DIR_LstAccDate[2];                                     // last access date (read or write), set to same as DIR_WrtDate on write
    unsigned char DIR_FstClusHI[2];                                      // high word of this entry's first cluster number (always 0 for a FAT12 or FAT16 volume)
    unsigned char DIR_WrtTime[2];                                        // time of last write, whereby a file creation is considered as a write
    unsigned char DIR_WrtDate[2];                                        // date of last write, whereby a file creation is considered as a write
    unsigned char DIR_FstClusLO[2];                                      // low word of this entry's first cluster number
    unsigned char DIR_FileSize[4];                                       // file's size in bytes
} DIR_ENTRY_STRUCTURE_FAT32;

typedef struct stINFO_SECTOR_FAT32
{
    unsigned char FSI_LeadSig[4];                                        // value 0x41615252. Used to validate that this is in fact an FSInfo sector
    unsigned char FSI_Reserved1[480];                                    // content should be zero
    unsigned char FSI_StrucSig[4];                                       // value 0x61417272. More localized signature
    unsigned char FSI_Free_Count[4];                                     // last known free cluster count on the volume - 0xffffffff means unknown
    unsigned char FSI_Nxt_Free[4];                                       // indicates the cluster number at which the driver should start looking for free clusters - 0xffffffff means unknown
    unsigned char FSI_Reserved2[12];                                     // content should be zero
    unsigned char FSI_TrailSig[4];                                       // value 0xaa550000
} INFO_SECTOR_FAT32;

typedef struct stLFN_ENTRY_STRUCTURE_FAT32
{
    unsigned char LFN_EntryNumber;                                       // entry number starting from last - 0x40 is always set and the value decrements until 1
    unsigned char LFN_Name_0;                                            // first letter
    unsigned char LFN_Name_0_extension;                                  // first letter extension - is always 0 in english character set
    unsigned char LFN_Name_1;                                            // second letter
    unsigned char LFN_Name_1_extension;                                  // second letter extension - is always 0 in english character set
    unsigned char LFN_Name_2;                                            // third letter
    unsigned char LFN_Name_2_extension;                                  // third letter extension - is always 0 in english character set
    unsigned char LFN_Name_3;                                            // fourth letter
    unsigned char LFN_Name_3_extension;                                  // fourth letter extension - is always 0 in english character set
    unsigned char LFN_Name_4;                                            // fifth letter
    unsigned char LFN_Name_4_extension;                                  // fifth letter extension - is always 0 in english character set
    unsigned char LFN_Attribute;                                         // always 0x0f
    unsigned char LFN_Zero0;                                             // always zero
    unsigned char LFN_Checksum;                                          // check sum
    unsigned char LFN_Name_5;                                            // sixth letter
    unsigned char LFN_Name_5_extension;                                  // sixth letter extension - is always 0 in english character set
    unsigned char LFN_Name_6;                                            // seventh letter
    unsigned char LFN_Name_6_extension;                                  // seventh letter extension - is always 0 in english character set
    unsigned char LFN_Name_7;                                            // eighth letter
    unsigned char LFN_Name_7_extension;                                  // eighth letter extension - is always 0 in english character set
    unsigned char LFN_Name_8;                                            // ninth letter
    unsigned char LFN_Name_8_extension;                                  // ninth letter extension - is always 0 in english character set
    unsigned char LFN_Name_9;                                            // tenth letter
    unsigned char LFN_Name_9_extension;                                  // tenth letter extension - is always 0 in english character set
    unsigned char LFN_Name_10;                                           // eleventh letter
    unsigned char LFN_Name_10_extension;                                 // eleventh letter extension - is always 0 in english character set
    unsigned char LFN_Zero1;                                             // always zero
    unsigned char LFN_Zero2;                                             // always zero
    unsigned char LFN_Name_11;                                           // twelfth letter
    unsigned char LFN_Name_11_extension;                                 // twelfth letter extension - is always 0 in english character set
    unsigned char LFN_Name_12;                                           // thirteenth letter
    unsigned char LFN_Name_12_extension;                                 // thirteenth letter extension - is always 0 in english character set
} LFN_ENTRY_STRUCTURE_FAT32;
#endif

/* =================================================================== */
/*                      global function definitions                    */
/* =================================================================== */


/* =================================================================== */
/*                             constants                               */
/* =================================================================== */

#if defined HID_LOADER                                                   // {16}
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

    static const unsigned char ucHID_Report[] = {                        // fixed HID report
    0x06,                                                                // usage page
    0x00,
    0xff,

    0x09,                                                                // usage
    0x01,

    0xa1,                                                                // collection 
    0x01,                                                                // application
    #if defined KBOOT_HID_LOADER                                         // {19}
        0x85, 0x01, 0x19, 0x01, 0x29, 0x01, 0x15,
        0x00, 0x26, 0xff, 0x00, 0x75, 0x08, 0x95, 0x23, 0x91, 0x02, 0x85, 0x02, 0x19, 0x01,
        0x29, 0x01, 0x15, 0x00, 0x26, 0xff, 0x00, 0x75, 0x08, 0x95, 0x23, 0x91, 0x02, 0x85,
        0x03, 0x19, 0x01, 0x29, 0x01, 0x15, 0x00, 0x26, 0xff, 0x00, 0x75, 0x08, 0x95, 0x23,
        0x81, 0x02, 0x85, 0x04, 0x19, 0x01, 0x29, 0x01, 0x15, 0x00, 0x26, 0xff, 0x00, 0x75,
        0x08, 0x95, 0x23, 0x81, 0x02,
    #else
        0x15,                                                            // logical minimum
        0x00,                                                            // 0

        0x26,                                                            // logical maximum
        0xff,                                                            // 255
        0x00,

        0x75,                                                            // report size
        0x08,                                                            // 8

        0x95,                                                            // report count 
        0x04,

            0x19,                                                        // usage minimum
            0x01,                                                        // 

            0x29,                                                        // usage maximum
            0x40,                                                        // 

            0x81,                                                        // input
            0x02,                                                        // data

            0x09,                                                        // usage
            0x01,                                                        // byte count

        0x75,                                                            // report size
        0x08,                                                            // 8

        0x95,                                                            // report count 
        0x40,

            0x19,                                                        // usage minimum
            0x01,                                                        // 

            0x29,                                                        // usage maximum
            0x40,                                                        // 

            0x91,                                                        // output
            0x02,                                                        // data variable

            0x09,                                                        // usage
            0x01,                                                        // byte count
    #endif
    0xc0                                                                 // end collection
    };

    #if !defined KBOOT_HID_LOADER
        static const unsigned char ucHID_Ack[] = {HID_REPORT_IN, HID_REPORT_OK, 0x44, 0xf4};
    #endif
#endif
#if defined USB_MSD_DEVICE_LOADER                                        // USB-MSD
    static int iDeviceMode = 0;                                          // device mode initially not active
    static const unsigned char  cCBWSignature[4] = {'U', 'S', 'B', 'C'};

    static const CBW_INQUIRY_DATA inquiryData = {
        DEVICE_TYPE_FLOPPY,
        RMB_REMOVABLE,
        0,
        RESPONSE_FORMAT_UFI,
        31,                                                              // additional length should be 31
        {0},                                                             // reserved field
        {'u', 'T', 'a', 's', 'k', 'e', 'r', ' '},                        // vendor information (8 bytes)
        {'U', 'S', 'B', ' ', 'M', 'S', 'D', ' ', 'L', 'o', 'a', 'd', 'e', 'r', ' ', ' '}, // product identification (16 bytes)
        {'1', '.', '0', '0' }                                            // product revision level
    };

    static const CBW_RETURN_SENSE_DATA sense_data = {VALID_SENSE_DATA, 0, SENSE_NO_SENSE, {0,0,0,0}, SENSE_LENGTH_10, {0,0,0,0}, 0, 0, {0,0,0,0}};

    static const MODE_PARAMETER_6 SelectData = {                         // standard response without descriptors and no write protection
        3,                                                               // content length
        DEVICE_TYPE_FLOPPY,                                              // medium type
        0,                                                               // no write protection
        0                                                                // no descriptors
    };

    static const CBW_CAPACITY_LIST formatCapacityNoMedia = {
        {0},
        8,                                                               // capacity list length with just one entry
        {
            {0x00, 0x01, 0xf8, 0x00}, DESC_CODE_NO_CARTRIDGE_IN_DRIVE, {0x00, 0x02, 0x00}, // 512 bytes block length
        }
    };

    #if !defined FAT_EMULATION
        #if defined UTFAT12
    static const unsigned char ucEmptyFAT12[4] = {
        LITTLE_LONG_WORD_BYTES(0x00fffff8)
    };
        #else
    static const unsigned char ucEmptyFAT32[12] = {
        LITTLE_LONG_WORD_BYTES(MEDIA_VALUE_FIXED),
        LITTLE_LONG_WORD_BYTES(0xffffffff),
        LITTLE_LONG_WORD_BYTES(CLUSTER_MASK)
    };
        #endif
    #endif

    #if defined READ_PASSWORD                                            // {2}
        static const CHAR ucReadPassword[] = READ_PASSWORD;              // access password
    #endif
#endif

// Device descriptor
//
static const USB_DEVICE_DESCRIPTOR device_descriptor = {                 // constant device descriptor
    STANDARD_DEVICE_DESCRIPTOR_LENGTH,                                   // standard device descriptor length (0x12)
    DESCRIPTOR_TYPE_DEVICE,                                              // 0x01
    #if defined USB_HS_INTERFACE                                         // {7} when using high speed set USB2.0
    {LITTLE_SHORT_WORD_BYTES(USB_SPEC_VERSION_2_0)},                     // USB2
    #else
    {LITTLE_SHORT_WORD_BYTES(USB_SPEC_VERSION)},                         // USB1.1 or USB2
    #endif
    #if defined USE_USB_CDC                                              // {31}
    DEVICE_CLASS_MISC_INTERFACE_ASSOCIATION_DESC,                        // uses interface association descriptors
    #else
    DEVICE_CLASS_AT_INTERFACE,                                           // device class, sub-class and protocol (class defined at interface level)
    #endif
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

// Configuration descriptor
//
static const USB_CONFIGURATION_DESCRIPTOR_COLLECTION config_descriptor = {
    {                                                                    // config descriptor
    DESCRIPTOR_TYPE_CONFIGURATION_LENGTH,                                // length (0x09)
    DESCRIPTOR_TYPE_CONFIGURATION,                                       // 0x02
    {LITTLE_SHORT_WORD_BYTES(sizeof(USB_CONFIGURATION_DESCRIPTOR_COLLECTION))}, // total length (little-endian)
    #if (defined USB_MSD_DEVICE_LOADER && defined HID_LOADER) || defined USE_USB_CDC
    2,                                                                   // HID and MSD together have two interfaces, as does USB-CDC
    #else
    1,                                                                   // configuration number - mass storage and HID have only one configuration
    #endif
    1,                                                                   // configuration value
    #if defined USB_STRING_OPTION && !defined HID_LOADER
    CONFIGURATION_STRING_INDEX,                                          // string index to configuration
    #else
    0,                                                                   // zero when strings are not supported
    #endif
    #if defined USB_POWER_CONSUMPTION_MA                                 // {4}
    (ATTRIBUTE_DEFAULT),                                                 // attributes for configuration,
    (USB_POWER_CONSUMPTION_MA/2)                                         // consumption in 2mA steps (eg. 100/2 for 100mA)
    #else
    (SELF_POWERED | ATTRIBUTE_DEFAULT),                                  // attributes for configuration,
    0                                                                    // consumption in 2mA steps (eg. 100/2 for 100mA)
    #endif
    },                                                                   // end of compulsory config descriptor
    #if defined USE_USB_CDC
    {                                                                    // interface association descriptor required when there is more than a CDC channel
    DESCRIPTOR_TYPE_INTERFACE_ASSOCIATION_LENGTH,                        // 0x08
    DESCRIPTOR_TYPE_INTERFACE_ASSOCIATION,                               // 0x0b
    0,                                                                   // first interface number
    2,                                                                   // interface count
    DEVICE_CLASS_COMMUNICATION_AND_CONTROL,
    0                                                                    // string reference
    },
    #endif

    {                                                                    // interface descriptor
    DESCRIPTOR_TYPE_INTERFACE_LENGTH,                                    // length (0x09)
    DESCRIPTOR_TYPE_INTERFACE,                                           // 0x04
    0,                                                                   // interface number 0
    0,                                                                   // alternative setting 0
    #if defined HID_LOADER                                               // {16}
        #if defined KBOOT_HID_LOADER                                     // {19}
    2,                                                                   // number of endpoints in addition to EP0 
        #else
    1,                                                                   // number of endpoints in addition to EP0
        #endif
    USB_CLASS_HID,                                                       // interface class (0x03)
    0,                                                                   // interface sub-class (0x00)
    0,                                                                   // interface protocol (0x00)
    #elif defined USE_USB_CDC
    1,                                                                   // number of endpoints in addition to EP0
    USB_CLASS_COMMUNICATION_CONTROL,                                     // interface class (0x02)
    USB_ABSTRACT_LINE_CONTROL_MODEL,                                     // interface sub-class (0x02)
    0,                                                                   // interface protocol
    #else                                                                // USB-MSD
    2,
    INTERFACE_CLASS_MASS_STORAGE,                                        // interface class (0x08)
    GENERIC_SCSI_MEDIA,                                                  // interface sub-class (0x06)
    BULK_ONLY_TRANSPORT,                                                 // interface protocol (0x50)    // number of endpoints in addition to EP0
    #endif
    #if defined USB_STRING_OPTION && !defined HID_LOADER
    INTERFACE_STRING_INDEX,                                              // string index for interface
    #else
    0,                                                                   // zero when strings are not supported
    #endif
    },                                                                   // end of interface descriptor

    #if defined HID_LOADER                                               // {16}
    {                                                                    // HID descriptor
        DESCRIPTOR_TYPE_HID_LENGTH,                                      // descriptor size in bytes (0x09)
        DESCRIPTOR_TYPE_HID,                                             // device descriptor type (0x21)
        #if defined KBOOT_HID_LOADER                                     // {19}
        {LITTLE_SHORT_WORD_BYTES(0x0100)},                               // HID class specific release number
        #else
        {LITTLE_SHORT_WORD_BYTES(0x0110)},                               // HID class specific release number
        #endif
        0,                                                               // hardware target country
        1,                                                               // number of HID class descriptors to follow
        DESCRIPTOR_TYPE_REPORT,                                          // descriptor type (0x22)
        {LITTLE_SHORT_WORD_BYTES(sizeof(ucHID_Report))},                 // total length of report descriptor
    },
    {                                                                    // interrupt IN endpoint descriptor
        DESCRIPTOR_TYPE_ENDPOINT_LENGTH,                                 // descriptor size in bytes (0x07)
        DESCRIPTOR_TYPE_ENDPOINT,                                        // end point descriptor (0x05)
        (IN_ENDPOINT | 0x01),                                            // direction and address of end point
        ENDPOINT_INTERRUPT,                                              // endpoint attributes
        {LITTLE_SHORT_WORD_BYTES(64)},                                   // endpoint FIFO size (little-endian - 64 bytes)
        #if defined KBOOT_HID_LOADER                                     // {19}
        10                                                               // polling interval in ms (10ms)
        #else
        1                                                                // polling interval in ms (1ms)
        #endif
    },
        #if defined KBOOT_HID_LOADER                                     // {19}
    {                                                                    // interrupt OUT endpoint descriptor
        DESCRIPTOR_TYPE_ENDPOINT_LENGTH,                                 // descriptor size in bytes (0x07)
        DESCRIPTOR_TYPE_ENDPOINT,                                        // end point descriptor (0x05)
        (OUT_ENDPOINT | 0x02),                                           // direction and address of end point
        ENDPOINT_INTERRUPT,                                              // endpoint attributes
        {LITTLE_SHORT_WORD_BYTES(64)},                                   // endpoint FIFO size (little-endian - 64 bytes)
        1                                                                // polling interval in ms (1ms)
    },
        #endif
        #if defined USB_MSD_DEVICE_LOADER && defined HID_LOADER          // HID/MSD composite
    {                                                                    // interface descriptor
        DESCRIPTOR_TYPE_INTERFACE_LENGTH,                                // length (0x09)
        DESCRIPTOR_TYPE_INTERFACE,                                       // 0x04
        1,                                                               // interface number 1
        0,                                                               // alternative setting 0
        2,                                                               // number of endpoints
        INTERFACE_CLASS_MASS_STORAGE,                                    // interface class (0x08)
        GENERIC_SCSI_MEDIA,                                              // interface sub-class (0x06)
        BULK_ONLY_TRANSPORT,                                             // interface protocol (0x50)    // number of endpoints in addition to EP0
        0,                                                               // zero when strings are not supported
    },                                                                   // end of interface descriptor
    {                                                                    // bulk out endpoint descriptor for the second interface
        DESCRIPTOR_TYPE_ENDPOINT_LENGTH,                                 // descriptor size in bytes (0x07)
        DESCRIPTOR_TYPE_ENDPOINT,                                        // end point descriptor (0x05)
        (OUT_ENDPOINT | USB_MSD_OUT_ENDPOINT_NUMBER),                    // direction and address of end point
        ENDPOINT_BULK,                                                   // endpoint attributes
            #if defined USB_HS_INTERFACE
        {LITTLE_SHORT_WORD_BYTES(512)},                                  // endpoint FIFO size (little-endian - 512 bytes)
            #else
        {LITTLE_SHORT_WORD_BYTES(64)},                                   // endpoint FIFO size (little-endian - 64 bytes)
            #endif
        0                                                                // polling interval in ms - ignored for bulk
    },
    {                                                                    // bulk in endpoint descriptor for the second interface
        DESCRIPTOR_TYPE_ENDPOINT_LENGTH,                                 // descriptor size in bytes (0x07)
        DESCRIPTOR_TYPE_ENDPOINT,                                        // end point descriptor (0x05)
        (IN_ENDPOINT | USB_MSD_IN_ENDPOINT_NUMBER),                      // direction and address of end point
        ENDPOINT_BULK,                                                   // endpoint attributes
        #if defined USB_HS_INTERFACE
        {LITTLE_SHORT_WORD_BYTES(512)},                                  // endpoint FIFO size (little-endian - 512 bytes)
        #else
        {LITTLE_SHORT_WORD_BYTES(64)},                                   // endpoint FIFO size (little-endian - 64 bytes)
        #endif
        0                                                                // polling interval in ms - ignored for bulk
    }
        #endif
    #elif defined USE_USB_CDC                                            // {31}
    {                                                                    // function descriptors
    USB_CDC_FUNCTIONAL_DESCRIPTOR_HEADER_LENGTH,                         // descriptor size in bytes (0x05)
    CS_INTERFACE,                                                        // type field (0x24)
    HEADER_FUNCTION_DESCRIPTOR,                                          // header descriptor (0x00)
    {LITTLE_SHORT_WORD_BYTES(USB_SPEC_VERSION)}                          // specification version
    },

    {
    USB_CDC_FUNCTIONAL_DESCRIPTOR_CALL_MAN_LENGTH,                       // descriptor size in bytes (0x05)
    CS_INTERFACE,                                                        // type field (0x24)
    CALL_MAN_FUNCTIONAL_DESCRIPTOR,                                      // call management function descriptor (0x01)
    CALL_MAN_FUNCTIONAL_CAPABILITY_HANDLES_CALL_MANAGEMENT,              // capabilities
    0                                                                    // data interface
    },                                                                   // end of function descriptors

    {
    USB_CDC_FUNCTIONAL_DESCRIPTOR_ABSTRACT_CONTROL_LENGTH,               // descriptor size in bytes (0x04)
    CS_INTERFACE,                                                        // type field (0x24)
    ABSTRACT_CONTROL_FUNCTION_DESCRIPTOR,                                // abstract control descriptor (0x02)
    ABSTRACT_CONTROL_FUNCTION_CAPABILITY_LINE_STATE_CODING               // capabilities
    },

    {
    USB_CDC_FUNCTIONAL_DESCRIPTOR_UNION_LENGTH,                          // descriptor size in bytes (0x05)
    CS_INTERFACE,                                                        // type field (0x24)
    UNION_FUNCTIONAL_DESCRIPTOR,                                         // union function descriptor (0x06)
    0,                                                                   // control interface
    1                                                                    // subordinate interface
    },                                                                   // end of function descriptors

    {                                                                    // interrupt endpoint descriptor for first interface
    DESCRIPTOR_TYPE_ENDPOINT_LENGTH,                                     // descriptor size in bytes (0x07)
    DESCRIPTOR_TYPE_ENDPOINT,                                            // endpoint descriptor (0x05)
    #if defined _LPC23XX || defined _LPC17XX
    (IN_ENDPOINT | 0x04),                                                // direction and address of endpoint (endpoint 4 is interrupt)
    #else
        #if defined USB_SIMPLEX_ENDPOINTS
    (IN_ENDPOINT | 0x02),                                                // direction and address of endpoint
        #else
    (IN_ENDPOINT | 0x03),                                                // direction and address of endpoint
        #endif
    #endif
    ENDPOINT_INTERRUPT,                                                  // endpoint attributes
    {LITTLE_SHORT_WORD_BYTES(64)},                                       // endpoint FIFO size (little-endian)
    10                                                                   // polling interval in ms
    },                                                                   // end of endpoint descriptor

    {                                                                    // the second interface
    DESCRIPTOR_TYPE_INTERFACE_LENGTH,                                    // descriptor size in bytes (0x09)
    DESCRIPTOR_TYPE_INTERFACE,                                           // interface descriptor (0x04)
    1,                                                                   // interface number 1
    0,                                                                   // no alternative setting
    2,                                                                   // 2 endpoints
    INTERFACE_CLASS_COMMUNICATION_DATA,                                  //
    0,                                                                   // sub-class
    0,                                                                   // interface protocol
    #if defined USB_STRING_OPTION
    INTERFACE_STRING_INDEX,                                              // string index for interface
    #else
    0                                                                    // zero when strings are not supported
    #endif
    },

    {                                                                    // bulk out endpoint descriptor for the second interface
    DESCRIPTOR_TYPE_ENDPOINT_LENGTH,                                     // descriptor size in bytes (0x07)
    DESCRIPTOR_TYPE_ENDPOINT,                                            // endpoint descriptor (0x05)
    #if defined _LPC23XX || defined _LPC17XX
    (OUT_ENDPOINT | 0x02),                                               // direction and address of endpoint (endpoint 2 is bulk)
    #else
    (OUT_ENDPOINT | 0x01),                                               // direction and address of endpoint
    #endif
    ENDPOINT_BULK,                                                       // endpoint attributes
    {LITTLE_SHORT_WORD_BYTES(64)},                                       // endpoint FIFO size (little-endian - 64 bytes)
    0                                                                    // polling interval in ms - ignored for bulk
    },

    {                                                                    // bulk in endpoint descriptor for the second interface
    DESCRIPTOR_TYPE_ENDPOINT_LENGTH,                                     // descriptor size in bytes (0x07)
    DESCRIPTOR_TYPE_ENDPOINT,                                            // endpoint descriptor (0x05)
    #if defined _LPC23XX || defined _LPC17XX
    (IN_ENDPOINT | 0x05),                                                // direction and address of endpoint (endpoint 5 is bulk)
    #else
        #if defined USB_SIMPLEX_ENDPOINTS
    (IN_ENDPOINT | 0x01),                                                // direction and address of endpoint
        #else
    (IN_ENDPOINT | 0x02),                                                // direction and address of endpoint
        #endif
    #endif
    ENDPOINT_BULK,                                                       // endpoint attributes
    {LITTLE_SHORT_WORD_BYTES(64)},                                       // endpoint FIFO size (little-endian - 64 bytes)
    0                                                                    // polling interval in ms - ignored for bulk
    },
    #else                                                                // USB-MSD alone
        {                                                                // bulk out endpoint descriptor for the second interface
        DESCRIPTOR_TYPE_ENDPOINT_LENGTH,                                 // descriptor size in bytes (0x07)
        DESCRIPTOR_TYPE_ENDPOINT,                                        // end point descriptor (0x05)
        (OUT_ENDPOINT | USB_MSD_OUT_ENDPOINT_NUMBER),                    // direction and address of end point
        ENDPOINT_BULK,                                                   // endpoint attributes
        #if defined USB_HS_INTERFACE                                     // {7}
        {LITTLE_SHORT_WORD_BYTES(512)},                                  // endpoint FIFO size (little-endian - 512 bytes)
        #else
        {LITTLE_SHORT_WORD_BYTES(64)},                                   // endpoint FIFO size (little-endian - 64 bytes)
        #endif
        0                                                                // polling interval in ms - ignored for bulk
        },
        {                                                                // bulk in endpoint descriptor for the second interface
        DESCRIPTOR_TYPE_ENDPOINT_LENGTH,                                 // descriptor size in bytes (0x07)
        DESCRIPTOR_TYPE_ENDPOINT,                                        // end point descriptor (0x05)
        (IN_ENDPOINT | USB_MSD_IN_ENDPOINT_NUMBER),                      // direction and address of end point
        ENDPOINT_BULK,                                                   // endpoint attributes
        #if defined USB_HS_INTERFACE                                     // {7}
        {LITTLE_SHORT_WORD_BYTES(512)},                                  // endpoint FIFO size (little-endian - 512 bytes)
        #else
        {LITTLE_SHORT_WORD_BYTES(64)},                                   // endpoint FIFO size (little-endian - 64 bytes)
        #endif
        0                                                                // polling interval in ms - ignored for bulk
        }
    #endif
};


#if defined USB_STRING_OPTION                                            // if our project supports strings
                                                                         // the characters in the string must be entered as 16 bit unicode in little-endian order!!
                                                                         // the first entry is the length of the content (including the length and descriptor type string entries)
    static const unsigned char usb_language_string[] = {4,  DESCRIPTOR_TYPE_STRING, LITTLE_SHORT_WORD_BYTES(UNICODE_LANGUAGE_INDEX)}; // this is compulsory first string
    #if defined HID_LOADER                                               // {16}
        static const unsigned char manufacturer_str[]    = {16, DESCRIPTOR_TYPE_STRING, 'u',0, 'T',0, 'a',0, 's',0, 'k',0, 'e',0, 'r',0};
        #if defined KBOOT_HID_LOADER                                     // {19}
            static const unsigned char product_str[]         = {38, DESCRIPTOR_TYPE_STRING, 'K',0, 'i',0, 'n',0, 'e',0, 't',0, 'i',0, 's',0, ' ',0, 'B',0, 'o',0, 'o',0, 't',0, 'l',0, 'o',0, 'a',0, 'd',0, 'e',0, 'r',0};
            static const unsigned char serial_number_str[]   = {8,  DESCRIPTOR_TYPE_STRING, '0',0, '0',0, '0',0};
        #else
            static const unsigned char product_str[]         = {8,  DESCRIPTOR_TYPE_STRING, 'H',0, 'I',0, 'D',0};
            static const unsigned char serial_number_str[]   = {8,  DESCRIPTOR_TYPE_STRING, 'I',0, 'C',0, 'P',0};
        #endif
        static const unsigned char *ucStringTable[]      = {usb_language_string, manufacturer_str, product_str, serial_number_str};
    #elif defined USE_USB_CDC                                            // {31}
        static const unsigned char manufacturer_str[]      = {10, DESCRIPTOR_TYPE_STRING, 'M',0, 'a',0, 'n',0, 'u',0};
        static const unsigned char product_str[]           = {16, DESCRIPTOR_TYPE_STRING, 'M',0, 'y',0, ' ',0, 'P',0, 'r',0, 'o',0, 'd',0};
        static const unsigned char serial_number_str[]     = {10, DESCRIPTOR_TYPE_STRING, '0',0, '0',0, '0',0, '1',0};
        static const unsigned char config_str[]            = {10, DESCRIPTOR_TYPE_STRING, 'C',0, 'o',0, 'n',0, 'f',0};
        static const unsigned char interface_str[]         = {8,  DESCRIPTOR_TYPE_STRING, 'I',0, 'n',0, 't',0};
        static const unsigned char *ucStringTable[]        = {usb_language_string, manufacturer_str, product_str, serial_number_str, config_str, interface_str};
    #else
        static const unsigned char manufacturer_str[]    = {10, DESCRIPTOR_TYPE_STRING, 'M',0, 'a',0, 'n',0, 'u',0};
        static const unsigned char product_str[]         = {16, DESCRIPTOR_TYPE_STRING, 'M',0, 'y',0, ' ',0, 'P',0, 'r',0, 'o',0, 'd',0};
        #if defined USB_RUN_TIME_DEFINABLE_STRINGS
        static const unsigned char serial_number_str[]   = {0};          // the application delivers this string (generated at run time)
        #else
        static const unsigned char serial_number_str[]   = {10, DESCRIPTOR_TYPE_STRING, '0',0, '0',0, '0',0, '1',0};
        #endif
        static const unsigned char config_str[]          = {10, DESCRIPTOR_TYPE_STRING, 'C',0, 'o',0, 'n',0, 'f',0};
        static const unsigned char interface_str[]       = {8,  DESCRIPTOR_TYPE_STRING, 'I',0, 'n',0, 't',0};

        static const unsigned char *ucStringTable[]      = {usb_language_string, manufacturer_str, product_str, serial_number_str, config_str, interface_str};
    #endif
#endif

/* =================================================================== */
/*                      local variable definitions                     */
/* =================================================================== */

static QUEUE_HANDLE USB_control = NO_ID_ALLOCATED;                       // USB default control endpoint handle
#if defined HID_LOADER
    static QUEUE_HANDLE USBPortID_HID = NO_ID_ALLOCATED;                 // USB port endpoint handle for HID
#endif
#if defined USE_USB_CDC                                                  // {31}
    static CDC_PSTN_LINE_CODING uart_setting = {{0}};                    // use a static struct to ensure that non-buffered transmission remains stable. Use also to receive new settings to
    static QUEUE_HANDLE USBPortID_interrupt = NO_ID_ALLOCATED;           // interrupt endpoint (not actually used)
#endif
#if defined USB_MSD_DEVICE_LOADER
    static QUEUE_HANDLE USBPortID_MSD = NO_ID_ALLOCATED;                 // USB port endpoint handle for MSD
#endif

#if defined USB_STRING_OPTION && defined USB_RUN_TIME_DEFINABLE_STRINGS
    static USB_STRING_DESCRIPTOR *SerialNumberDescriptor = 0;            // string built up to contain a variable serial number
#endif

#if defined HID_LOADER && !defined KBOOT_HID_LOADER                      // {16}
    static unsigned char *ptrFlashAddress = 0;
    static unsigned char ucSectorBuffer[FLASH_GRANULARITY] = {0};
    static unsigned short usBuff_length = 0;
#endif
#if defined USB_MSD_DEVICE_LOADER                                        // USB-MSD
    #if defined READ_PASSWORD && !defined FAT_EMULATION                  // {2}
        static int iReadEnabled = 0;
    #endif
    static unsigned long ulLogicalBlockAdr = 0;                          // present logical block address (shared between read and write)
    static unsigned long ulReadBlock = 0;                                // the outstanding blocks to be read from the media
    static unsigned long ulWriteBlock = 0;                               // the outstanding blocks to be written to the media
    static int iContent = 0;
    static USB_MASS_STORAGE_CSW csw = {{'U', 'S', 'B', 'S'}, {0}, {0}, CSW_STATUS_COMMAND_PASSED};
    static CBW_RETURN_SENSE_DATA present_sense_data = {0};
    #if defined MAC_OS_X_WORKAROUND
        static unsigned long ulNewWriteBlock[EMULATED_FAT_LUMS] = {0};
    #endif
    static const UTDISK *ptrDiskInfo[EMULATED_FAT_LUMS] = {0};
    static const unsigned char *ptr_disk_location[EMULATED_FAT_LUMS] = {0};
    static const unsigned char *ptr_fileobject_location[EMULATED_FAT_LUMS] = {0};
    static const unsigned char *ptr_disk_end[EMULATED_FAT_LUMS] = {0};
    #if defined FAT_EMULATION
        static DATA_FILE_INFORMATION dataFile[EMULATED_FAT_LUMS][APPLICATION_DATA_FILES] = {{{0}}};
    #else
        static DIR_ENTRY_STRUCTURE_FAT32 root_file[EMULATED_FAT_LUMS][(2 * (BYTES_PER_SECTOR/sizeof(DIR_ENTRY_STRUCTURE_FAT32)))] = {{{{0}}}}; // copy of present root directory (2 sectors are maintained due to some hidden files that may be written and long file names that could be used)
        static unsigned long ulSectorMemory[BYTES_PER_SECTOR / sizeof(unsigned long)] = {0};
        static unsigned long ulFAT32size = 0;
        static UTDISK utDisks[EMULATED_FAT_LUMS] = {{0}};
    #endif
    static int iSoftwareState[EMULATED_FAT_LUMS] = {SW_EMPTY};
    #if defined WINDOWS_8_1_WORKAROUND || defined MAC_OS_X_WORKAROUND    // {15}
        static unsigned char ucAcceptUploads[EMULATED_FAT_LUMS] = {0};
    #endif
    #if defined MAC_OS_X_WORKAROUND
        static unsigned long ulFatSector[EMULATED_FAT_LUMS][BYTES_PER_SECTOR/sizeof(unsigned long)] = {{0}}; // we maintain a backup of a single fat sector for monitoring hidden OS cluster content
        #if !defined AUTO_DELETE_ON_ANY_FIRMWARE
        static unsigned long ulLastIgnoredClusterWrite[EMULATED_FAT_LUMS] = {0};
        #endif				
    #endif
    static unsigned char ucActiveLUN = DISK_C;
#endif


/* =================================================================== */
/*                      local function definitions                     */
/* =================================================================== */

static void fnConfigureUSB(void);                                        // routine to open and configure USB interface

#if defined USB_MSD_DEVICE_LOADER
    #if defined USB_STRING_OPTION && defined USB_RUN_TIME_DEFINABLE_STRINGS
        static void fnSetSerialNumberString(CHAR *ptrSerialNumber);
    #endif
    static void fnContinueMedia(void);
    static unsigned char *fnGetPartition(unsigned char ucLUN, QUEUE_TRANSFER *length);
    #if !defined FAT_EMULATION
        static const UTDISK *_fnGetDiskInfo(unsigned char ucDisk);
        static int _fnReadSector(unsigned char ucDisk, unsigned char *ptrBuffer, unsigned long ulSectorNumber);
    #endif
    static int _fnWriteSector(unsigned char ucDisk, unsigned char *ptrBuffer, unsigned long ulSectorNumber);
    static unsigned long fnGetFileSize(const LFN_ENTRY_STRUCTURE_FAT32 *ptrLFN_entry);
    static int mass_storage_callback(unsigned char *ptrData, unsigned short length, int iType);
    #if defined DEBUG_MAC
        static void fnDebugWrite(int iDisk, unsigned char *ptr_ucBuffer, unsigned long ulLogicalBlockAdr);
    #endif
    #if defined FAT_EMULATION
        static void fnPrepareEmulatedFAT(int iDisk);
    #endif
    #if !defined EMULATED_FAT_LUMS
        #define EMULATED_FAT_LUMS    DISK_COUNT
    #endif
#endif
#if defined USE_USB_CDC                                                  // {31}
    static void fnReturnUART_settings(void);
    static void fnNewUART_settings(unsigned char *ptrData, unsigned short usLength, unsigned short usExpected);
#endif

/* =================================================================== */
/*                           global variables                          */
/* =================================================================== */

#if defined USE_USB_CDC                                                  // {31}
    QUEUE_HANDLE USBPortID_comms = NO_ID_ALLOCATED;                      // USB port endpoint handle
#endif

/* =================================================================== */
/*                                task                                 */
/* =================================================================== */


// USB device task
//
extern void fnTaskUSB(TTASKTABLE *ptrTaskTable)
{
    #if defined USB_MSD_DEVICE_LOADER
    QUEUE_TRANSFER Length;
    #endif
    QUEUE_HANDLE PortIDInternal = ptrTaskTable->TaskID;                  // queue ID for task input
    unsigned char ucInputMessage[LARGE_MESSAGE];                         // reserve space for receiving messages

    if (USB_control == NO_ID_ALLOCATED) {                                // initialisation
    #if defined USB_STRING_OPTION && defined USB_RUN_TIME_DEFINABLE_STRINGS // if dynamic strings are supported, prepare a specific serial number ready for enumeration
        fnSetSerialNumberString(parameters->cDeviceIDName);              // construct a serial number string for USB use
    #endif
    #if (defined KWIKSTIK || defined TWR_K40X256 || defined TWR_K40D100M || defined TWR_K53N512 || defined FRDM_KL46Z || defined FRDM_KL43Z || defined TWR_KL43Z48M || defined TWR_KL46Z48M) // {3}{6}
        CONFIGURE_SLCD();
    #endif
    #if defined USB_MSD_DEVICE_LOADER
        #if defined MEMORY_SWAP
            #if !defined SERIAL_INTERFACE
        fnHandleSwap(1);                                                 // check that no previous swap had aborted before completion
            #endif
        ptr_disk_location[DISK_C] = (const unsigned char *)(FLASH_START_ADDRESS + (SIZE_OF_FLASH/2));
        ptr_fileobject_location[DISK_C] = (const unsigned char *)(SIZE_OF_FLASH - (2 * FLASH_GRANULARITY));
        ptr_disk_end[DISK_C] = (const unsigned char *)(FLASH_START_ADDRESS + (SIZE_OF_FLASH/2) + (UTASKER_APP_END - UTASKER_APP_START));
        #else
        ptr_disk_location[DISK_C] = (const unsigned char *)UTASKER_APP_START;
        ptr_fileobject_location[DISK_C] = (const unsigned char *)UTASKER_APP_START;
        ptr_disk_end[DISK_C] = (const unsigned char *)UTASKER_APP_END;
        #endif
        #if EMULATED_FAT_LUMS > 1
            #if defined MEMORY_SWAP
        ptr_disk_location[DISK_D] = (const unsigned char *)(FLASH_START_ADDRESS);
        ptr_fileobject_location[DISK_D] = (const unsigned char *)(((SIZE_OF_FLASH/2) - (2 * FLASH_GRANULARITY)));
        ptr_disk_end[DISK_D] = (const unsigned char *)(FLASH_START_ADDRESS + (UTASKER_APP_END - UTASKER_APP_START));
            #else
        ptr_disk_location[DISK_D] = (const unsigned char *)(UTASKER_APP_START);
        ptr_fileobject_location[DISK_D] = (const unsigned char *)(UTASKER_APP_START);
        ptr_disk_end[DISK_D] = (const unsigned char *)UTASKER_APP_END;
            #endif
        #endif
        iDeviceMode = 1;                                                 // device mode is active (not host mode)
        do {
            unsigned long ulData;
            fnGetParsFile((unsigned char *)ptr_disk_location[ucActiveLUN], (unsigned char *)&ulData, sizeof(unsigned long));
            if (ulData != 0xffffffff) {                                  // check whether data is present at the start of the memory area
            #if !defined FAT_EMULATION
                fnGetParsFile((unsigned char *)ptr_fileobject_location[ucActiveLUN], (unsigned char *)&root_file[ucActiveLUN], (ROOT_FILE_ENTRIES * sizeof(DIR_ENTRY_STRUCTURE_FAT32))); // load initial root directory entries from FLASH
            #endif
                iSoftwareState[ucActiveLUN] = SW_AVAILABLE;              // existing software needs to be deleted before new code can be loaded
            }
            #if defined FAT_EMULATION
            fnPrepareEmulatedFAT(ucActiveLUN);                           // prepare the files that we want to show on the USB-MSD disk
            ptrDiskInfo[ucActiveLUN] = fnGetDiskInfo(ucActiveLUN);       // get a pointer to the disk information for local use
                #if defined MEMORY_SWAP
            if (ucActiveLUN == DISK_C) {                                 // only the swap memory disk can be written to
                #endif
                ((UTDISK *)ptrDiskInfo[ucActiveLUN])->usDiskFlags &= ~(WRITE_PROTECTED_SD_CARD); // remove write protection since we want to write new firmware to the disk
                #if defined MEMORY_SWAP
            }
                #endif
            fnPrepareRootDirectory(ptrDiskInfo[ucActiveLUN], -1);
                #if defined MEMORY_SWAP && (EMULATED_FAT_LUMS > 1)
            if (ucActiveLUN == 0) {
                uMemcpy(((UTDISK *)ptrDiskInfo[ucActiveLUN])->rootBuffer->DIR_Name, "UPLOAD_DISK", 11); // set volume name
            }
            else {
                uMemcpy(((UTDISK *)ptrDiskInfo[ucActiveLUN])->rootBuffer->DIR_Name, "ACTIVE_SW", 11); // set volume name
            }
                #else
            uMemcpy(((UTDISK *)ptrDiskInfo[ucActiveLUN])->rootBuffer->DIR_Name, DISK_NAME, 11); // set volume name
                    #if EMULATED_FAT_LUMS > 1
            ((UTDISK *)ptrDiskInfo[ucActiveLUN])->rootBuffer->DIR_Name[DISK_NAME_INC] += ucActiveLUN;
                    #endif
                #endif
            #else
            ptrDiskInfo[ucActiveLUN] = _fnGetDiskInfo(ucActiveLUN);      // get a pointer to each disk information for local use
            #endif
        } while (++ucActiveLUN < EMULATED_FAT_LUMS);

        fnConfigureUSB();                                                // configure the USB interface for device mode operation
        #if defined _WINDOWS && defined UTFAT12 && defined ENABLE_READBACK && !defined FAT_EMULATION // development test to verify the FAT12 cluster construction
        {
            static unsigned short *fnFillClusterEntry(unsigned short *ptrCluster, unsigned char *ucOffset, unsigned short *uslastEntry, unsigned short usCluster);
            unsigned short usTestCluster[8 * 1024];
            unsigned long ulFileSize = (378608 + (3 * 512));
            unsigned char ucOffset = 2;
            unsigned short uslastEntry = 0;
            unsigned short usCluster = 4;
            unsigned short *ptrCluster = usTestCluster;
            while (ulFileSize >= BYTES_PER_SECTOR) {                     // add single file cluster information
                ptrCluster = fnFillClusterEntry(ptrCluster, &ucOffset, &uslastEntry, usCluster);
                usCluster++;
                if ((usCluster & 0xff) == 0x00) {
                    usCluster = usCluster;                               // break point at end of each sector in FAT
                }
                ulFileSize -= BYTES_PER_SECTOR;
            }
            switch (ucOffset) {
            case 0:
                ptrCluster = fnFillClusterEntry(ptrCluster, &ucOffset, &uslastEntry, (unsigned short)(usCluster | 0x000f));
                *ptrCluster = LITTLE_SHORT_WORD(0x00ff);
                break;
            case 1:
                ptrCluster = fnFillClusterEntry(ptrCluster, &ucOffset, &uslastEntry, (unsigned short)(usCluster | 0x00ff));
                *ptrCluster = LITTLE_SHORT_WORD(0x000f);
                break;
            case 2:
                ptrCluster = fnFillClusterEntry(ptrCluster, &ucOffset, &uslastEntry, 0x0fff);
                break;
            case 3:
                *ptrCluster = LITTLE_SHORT_WORD(0x0fff);
                break;
            }
        }
        #endif
    #else
        fnConfigureUSB();                                                // configure the USB interface for device mode operation
    #endif
    #if defined USB_MSD_HOST_LOADER || defined USB_MSD_TIMEOUT           // {29}
        #if defined _WINDOWS
        uTaskerMonoTimer(OWN_TASK, (DELAY_LIMIT)(3 * SEC), TIMEOUT_USB_ENUMERATION); // longer time when simulating to allow enumeration to be commanded in the simulator
        #else
        uTaskerMonoTimer(OWN_TASK, (DELAY_LIMIT)(0.5 * SEC), TIMEOUT_USB_ENUMERATION); // 500ms in which the device must enumerate otherwise the host mode will be switched to instead
        #endif
    #endif
    #if defined HID_LOADER && defined KBOOT_HID_LOADER && defined KBOOT_HID_ENUMERATION_LIMIT
        uTaskerMonoTimer(OWN_TASK, KBOOT_HID_ENUMERATION_LIMIT, TIMEOUT_USB_ENUMERATION);
    #endif
    }

    while (fnRead(PortIDInternal, ucInputMessage, HEADER_LENGTH)) {      // check input queue
        switch (ucInputMessage[MSG_SOURCE_TASK]) {                       // switch depending on source
        case INTERRUPT_EVENT:                                            // interrupt event without data
            switch (ucInputMessage[MSG_INTERRUPT_EVENT]) {               // specific interrupt event type
            case EVENT_USB_RESET:                                        // active USB connection has been reset
    #if defined USB_MSD_DEVICE_LOADER
                ulWriteBlock = 0;                                        // reset local variables on each reset
        #if defined MAC_OS_X_WORKAROUND
                uMemset(ulNewWriteBlock, 0, sizeof(ulNewWriteBlock));
        #endif
                ulReadBlock = 0;
                iContent = 0;
        #if defined WINDOWS_8_1_WORKAROUND || defined MAC_OS_X_WORKAROUND// {15}
                uMemset(ucAcceptUploads, 0, sizeof(ucAcceptUploads));
        #endif
    #endif
                fnDebugMsg("USB Reset\n\r");                             // display that the USB bus has been reset
    #if defined DEL_USB_SYMBOL
                DEL_USB_SYMBOL();
    #endif
                break;
            case EVENT_USB_SUSPEND:                                      // a suspend condition has been detected. A bus powered device should reduce consumption to <= 500uA or <= 2.5mA (high power device)
                fnSetUSBConfigState(USB_DEVICE_SUSPEND, 0);              // set all endpoint states to suspended
                fnDebugMsg("USB Suspended\n\r");
    #if defined RESET_ON_SUSPEND
                fnResetBoard();                                          // command hardware reset when the USB cable is pulled
    #endif
    #if defined DEL_USB_SYMBOL
                DEL_USB_SYMBOL();
    #endif
                break;
            case EVENT_USB_RESUME:                                       // a resume sequence has been detected so full power consumption can be resumed
                fnSetUSBConfigState(USB_DEVICE_RESUME, 0);               // remove suspended state from all endpoints
                fnDebugMsg("USB Resume\n\r");
    #if defined SET_USB_SYMBOL
                SET_USB_SYMBOL();
    #endif
                break;
    #if defined USE_USB_CDC
            case EVENT_RETURN_PRESENT_UART_SETTING:                      // the CDC class host is requesting our serial settings
                fnReturnUART_settings();                                 // return the present serial interface settings
                break;
    #endif
    #if defined HID_LOADER                                               // {16}
        #if !defined KBOOT_HID_LOADER
            case HID_LOADER_ERASE:                                       // erase a flash sector
                if ((ptrFlashAddress >= (unsigned char *)UTASKER_APP_START) && (ptrFlashAddress < UTASKER_APP_END)) { // if the sector belongs to the application space
                    fnEraseFlashSector(ptrFlashAddress, 0);              // erase the single sector
                }
                fnWrite(USBPortID_HID, (unsigned char *)&ucHID_Ack, sizeof(ucHID_Ack)); // acknowledge that the sector erase has completed
                break;

            case HID_LOADER_PROGRAM:
                if ((ptrFlashAddress >= (unsigned char *)UTASKER_APP_START) && (ptrFlashAddress < UTASKER_APP_END)) { // if the sector belongs to the application space
                    if ((ptrFlashAddress + usBuff_length) >= UTASKER_APP_END) { // if the write would be past the end of the application space
                        usBuff_length = (unsigned short)(ptrFlashAddress - UTASKER_APP_END);
                    }
                    fnWriteBytesFlash(ptrFlashAddress, ucSectorBuffer, usBuff_length); // program flash
                    if (ptrFlashAddress == (unsigned char *)UTASKER_APP_START) { // if the first sector was just programmed it means that the final sector has been written
                        uTaskerMonoTimer(OWN_TASK, (DELAY_LIMIT)(1 * SEC), TIMEOUT_RESET_NOW); // automatic reset after a short delay to start new code
                    }
                }
                fnWrite(USBPortID_HID, (unsigned char *)&ucHID_Ack, sizeof(ucHID_Ack)); // acknowledge that the buffer programming has completed
                break;
        #endif
    #endif
    #if defined USB_MSD_DEVICE_LOADER
            case TX_FREE:
                fnContinueMedia();                                       // the output buffer has space so continue with data transfer
                break;
    #endif
            }
            break;

        case TIMER_EVENT:
            switch (ucInputMessage[MSG_TIMER_EVENT]) {
    #if defined USB_MSD_HOST_LOADER || defined USB_MSD_TIMEOUT           // {29}
            case TIMEOUT_USB_ENUMERATION:                                // there has been no enumeration after an initial delay
                // We disable device mode and move to host to see whether we can load from a memory stick instead
                //
        #if defined USB_MSD_HOST_LOADER
                uTaskerStateChange(TASK_SD_LOADER, UTASKER_ACTIVATE);    // allow the SD card (memory stick) loader task to start (which also starts the USB host task)
                iDeviceMode = 0;                                         // no longer device mode of operation
                fnDebugMsg("Switching to host mode\r\n");
        #else
                fnJumpToValidApplication(1);
        #endif
                break;
    #elif defined HID_LOADER && defined KBOOT_HID_LOADER && defined KBOOT_HID_ENUMERATION_LIMIT
            case TIMEOUT_USB_ENUMERATION:                                // there has been no enumeration after an initial delay
                fnJumpToValidApplication(1);
                break;
    #endif
    #if defined HID_LOADER
            case TIMEOUT_RESET_NOW:
                fnResetBoard();                                          // reset to start the new application
                break;
    #endif
    #if defined USB_MSD_DEVICE_LOADER
        #if (defined WINDOWS_8_1_WORKAROUND || defined MAC_OS_X_WORKAROUND) && !defined NO_BLOCKING_PERIOD // {15}
            case TIMEOUT_ACCEPT_UPLOAD:
            #if defined MAC_OS_X_WORKAROUND
                uMemset(ucAcceptUploads, 1, sizeof(ucAcceptUploads));    // potentially accept software uploads from now (valid for all disks)
            #else
                uMemset(ucAcceptUploads, 2, sizeof(ucAcceptUploads));    // accept software uploads from now (valid for all disks)
            #endif
            #if defined DEBUG_MAC
                fnDebugMsg("Blocking period expired\r\n");
            #endif
            break;
        #endif

            case TIMEOUT_USB_LOADING_COMPLETE_C:                         // idle timer fired, indicating the end of a file copy
        #if EMULATED_FAT_LUMS > 1
            case TIMEOUT_USB_LOADING_COMPLETE_D:
        #endif
        #if defined RESET_ON_EJECT
                if (iSoftwareState[ucInputMessage[MSG_TIMER_EVENT] - TIMEOUT_USB_LOADING_COMPLETE_C] == SW_PROGRAMMING) // timeout after software upload, indicating that we should complete and reset
        #endif
                {
                    int iDisk = (ucInputMessage[MSG_TIMER_EVENT] - TIMEOUT_USB_LOADING_COMPLETE_C);
        #if defined FAT_EMULATION
                    DIR_ENTRY_STRUCTURE_FAT32 *ptrVolumeEntry = ptrDiskInfo[iDisk]->rootBuffer; // pointer to a copy of the root directory
        #else
                    DIR_ENTRY_STRUCTURE_FAT32 *ptrVolumeEntry = (DIR_ENTRY_STRUCTURE_FAT32 *)&root_file[iDisk];  // pointer to a copy of the root directory
        #endif
        #if defined WINDOWS_8_1_WORKAROUND || defined MAC_OS_X_WORKAROUND
                    DIR_ENTRY_STRUCTURE_FAT32 *ptrVolumeEntryBackup = 0;
                    MAX_FILE_LENGTH FileObjectSize = 0;
        #endif
        #if defined FLASH_ROW_SIZE && FLASH_ROW_SIZE > 0
                    fnWriteBytesFlash(0, 0, 0);                          // close any outstanding FLASH buffer from end of the file
        #endif
        #if defined WINDOWS_8_1_WORKAROUND || defined MAC_OS_X_WORKAROUND// {21} jump possible hidden files (added by Windows 8.1 or MAC OS X) and deleted entries
                    while (1) {
                        if ((ptrVolumeEntry->DIR_Name[0] == DIR_NAME_FREE) || ((ptrVolumeEntry->DIR_Attr != DIR_ATTR_LONG_NAME) && (ptrVolumeEntry->DIR_Attr & (DIR_ATTR_HIDDEN | DIR_ATTR_VOLUME_ID)))) { // volume ID, deleted or hidden system file/directory {25} and hidden type is ignored
                            ptrVolumeEntryBackup = 0;                    // invalid as upload file so reset
                        }
                        else {
                            if (ptrVolumeEntryBackup == 0) {
                                ptrVolumeEntryBackup = ptrVolumeEntry;   // remember the location of the start of the file object
                                FileObjectSize = sizeof(DIR_ENTRY_STRUCTURE_FAT32); // reset the object size
                            }
                            else {
                                FileObjectSize += sizeof(DIR_ENTRY_STRUCTURE_FAT32); // count the file object's length
                            }
                            if (ptrVolumeEntry->DIR_Attr == DIR_ATTR_ARCHIVE) { // file name found - this is assumed to be the one that was uploaded (always first)
                                ptrVolumeEntry->DIR_FstClusLO[0] = (RESERVED_SECTION_COUNT + 1); // ensure that the cluster location is the first (since it may have been located elsewhere after inserting a hidden system directory)
                                ptrVolumeEntry = ptrVolumeEntryBackup;   // set to the beginning of this file object
                                break;
                            }
                        }
                        ptrVolumeEntry++;                                // move to next entry
                    }
                    if (FileObjectSize > ((ROOT_FILE_ENTRIES - 1) * sizeof(DIR_ENTRY_STRUCTURE_FAT32))) { // {24} if the software name is too long for the file object we use its short file name name instead
                        while (FileObjectSize > sizeof(DIR_ENTRY_STRUCTURE_FAT32)) {
                            ptrVolumeEntry++;                            // move to the final directory struct entry which will be the short file name
                            FileObjectSize -= (sizeof(DIR_ENTRY_STRUCTURE_FAT32));
                        }
                    }
            #if defined FAT_EMULATION
                    fnWriteBytesFlash((unsigned char *)(ptr_fileobject_location[iDisk]), (unsigned char *)ptrDiskInfo[iDisk]->rootBuffer, sizeof(DIR_ENTRY_STRUCTURE_FAT32)); // {30} save volume object
            #else
                    fnWriteBytesFlash((unsigned char *)(ptr_fileobject_location[iDisk]), (unsigned char *)&root_file[iDisk], sizeof(DIR_ENTRY_STRUCTURE_FAT32)); // {30} save volume object
            #endif
                    fnWriteBytesFlash((unsigned char *)(ptr_fileobject_location[iDisk] + sizeof(DIR_ENTRY_STRUCTURE_FAT32)), (unsigned char *)ptrVolumeEntry, FileObjectSize); // write the file object next
                    FileObjectSize += sizeof(DIR_ENTRY_STRUCTURE_FAT32); // {30}
                    while (FileObjectSize < (ROOT_FILE_ENTRIES * sizeof(DIR_ENTRY_STRUCTURE_FAT32))) {
                        DIR_ENTRY_STRUCTURE_FAT32 emptyObject;           // ensure remainder of the file object area is blank
                        uMemset(&emptyObject, 0, sizeof(emptyObject));
                        fnWriteBytesFlash((unsigned char *)(ptr_disk_location[iDisk] + FileObjectSize), (unsigned char *)&emptyObject, sizeof(emptyObject));
                        FileObjectSize += sizeof(DIR_ENTRY_STRUCTURE_FAT32);
                    }
        #else
                    while (ptrVolumeEntry->DIR_Name[0] == DIR_NAME_FREE) { // jump any deleted entries (from delete before programming)
                        ptrVolumeEntry++;
                    }
                    fnWriteBytesFlash((unsigned char *)(ptr_fileobject_location[iDisk]), (unsigned char *)ptrVolumeEntry, (ROOT_FILE_ENTRIES * sizeof(DIR_ENTRY_STRUCTURE_FAT32)));
        #endif
        #if defined FLASH_ROW_SIZE && FLASH_ROW_SIZE > 0
                    fnWriteBytesFlash(0, 0, 0);                          // close any outstanding FLASH buffer from end of the file
        #endif
                    iSoftwareState[iDisk] = SW_AVAILABLE;                // software upload completed
        #if !defined RESET_ON_EJECT && !defined MEMORY_SWAP
                    fnResetBoard();                                      // reset to start the new application
        #endif
                }
        #if defined RESET_ON_EJECT && !defined MEMORY_SWAP
                fnResetBoard();                                          // reset to start the new application
        #endif
                break;
    #endif
            }
            break;

        case TASK_USB:                                                   // USB interrupt handler is requesting us to perform work offline
            fnRead(PortIDInternal, &ucInputMessage[MSG_CONTENT_COMMAND], ucInputMessage[MSG_CONTENT_LENGTH]); // get the content
            switch (ucInputMessage[MSG_CONTENT_COMMAND]) {
            case E_USB_ACTIVATE_CONFIGURATION:
                fnDebugMsg("Enumerated\r\n");                            // the interface has been activated and enumeration completed
    #if defined SET_USB_SYMBOL
                SET_USB_SYMBOL();
    #endif
    #if defined USB_MSD_HOST_LOADER || defined USB_MSD_TIMEOUT           // {29}
                uTaskerStopTimer(OWN_TASK);                              // stop the enumeration monitor timer so that we stay in USB-MSD device mode
    #endif
    #if defined USB_MSD_DEVICE_LOADER && (defined WINDOWS_8_1_WORKAROUND || defined MAC_OS_X_WORKAROUND) // {15}
        #if defined NO_BLOCKING_PERIOD
            #if defined MAC_OS_X_WORKAROUND
                uMemset(ucAcceptUploads, 1, sizeof(ucAcceptUploads));    // potentially accept software uploads from now (valid for all disks)
            #else
                uMemset(ucAcceptUploads, 2, sizeof(ucAcceptUploads));    // accept software uploads from now (valid for all disks)
            #endif
        #else
                uTaskerMonoTimer(OWN_TASK, (DELAY_LIMIT)(2 * SEC), TIMEOUT_ACCEPT_UPLOAD); // start a period where writes will be ignored - when the timer has expired uploads are accepted
        #endif
    #endif
    #if defined HID_LOADER && defined KBOOT_HID_LOADER && defined KBOOT_COMMAND_LIMIT
                uTaskerMonoTimer(OWN_TASK, KBOOT_COMMAND_LIMIT, TIMEOUT_USB_ENUMERATION);
    #elif defined HID_LOADER && defined KBOOT_HID_LOADER && defined KBOOT_HID_ENUMERATION_LIMIT
                uTaskerStopTimer(OWN_TASK);
    #endif
                break;
            }
            break;

        default:
            break;
        }
    }
    #if defined HID_LOADER && defined KBOOT_HID_LOADER                   // {19}
    while (fnMsgs(USBPortID_HID) != 0) {                                 // reception from HID OUT endpoint
        KBOOT_PACKET KBOOT_packet;
        fnRead(USBPortID_HID, (unsigned char *)&KBOOT_packet, sizeof(KBOOT_packet)); // read the complete packet (there is always one complete packet in each USB reception)
        if (fnHandleKboot(USBPortID_HID, KBOOT_HID, &KBOOT_packet) != 0) { // {20} perform generic kboot handling
            uTaskerMonoTimer(OWN_TASK, (DELAY_LIMIT)(1 * SEC), TIMEOUT_RESET_NOW); // the command requires a (delayed) reset
        }
    }
    #endif
    #if defined USB_MSD_DEVICE_LOADER                                    // MSD mode of operation
    while (fnMsgs(USBPortID_MSD) != 0) {                                 // reception from MSD OUT endpoint
        if (ulWriteBlock != 0) {                                         // write data expected
            static unsigned char ucBuffer[BYTES_PER_SECTOR] = {0};       // intermediate buffer to collect each sector content
            Length = fnRead(USBPortID_MSD, &ucBuffer[iContent], (QUEUE_TRANSFER)(BYTES_PER_SECTOR - iContent)); // read the content directly to the intermediate buffer
            ulWriteBlock -= Length;
            iContent += Length;
            if (iContent >= BYTES_PER_SECTOR) {                          // input buffer is complete
        #if defined WINDOWS_8_1_WORKAROUND && !defined MAC_OS_X_WORKAROUND // {15}
                if (ucAcceptUploads[ucActiveLUN] != 0) {                 // ignore writes until the upload delay has expired
                    _fnWriteSector(ucActiveLUN, ucBuffer, ulLogicalBlockAdr++); // commit the buffer content to the media
                }
        #else
            #if defined DEBUG_MAC
                fnDebugWrite(ucActiveLUN, ucBuffer, ulLogicalBlockAdr);
            #endif
                _fnWriteSector(ucActiveLUN, ucBuffer, ulLogicalBlockAdr++); // commit the buffer content to the media
        #endif
                iContent = 0;                                            // reset intermediate buffer
                if (ulWriteBlock != 0) {
                    uTaskerStateChange(OWN_TASK, UTASKER_ACTIVATE);      // yield after a write when further data is expected but schedule again immediately to read any remaining queue content
                    return;
                }
            }
            if (ulWriteBlock != 0) {                                     // more data expected
                continue;
            }                                                            // allow CSW to be sent after a complete transfer has completed
        }
        else {
            const UTDISK *_ptrDiskInfo;
            USB_MASS_STORAGE_CBW *ptrCBW;
            Length = fnRead(USBPortID_MSD, ucInputMessage, LARGE_MESSAGE); // read the content
            ptrCBW = (USB_MASS_STORAGE_CBW *)ucInputMessage;
            uMemcpy(&csw.dCBWTag, ptrCBW->dCBWTag, sizeof(csw.dCBWTag)); // save the tag
            csw.dCSWDataResidue[0] = 0;
            csw.dCSWDataResidue[1] = 0;
            csw.dCSWDataResidue[2] = 0;
            csw.dCSWDataResidue[3] = 0;
            csw.bCSWStatus = CSW_STATUS_COMMAND_PASSED;                  // set for success
            fnFlush(USBPortID_MSD, FLUSH_TX);                            // always flush the tx buffer to ensure message alignment in buffer before responding
            ucActiveLUN = ptrCBW->dCBWLUN;                               // the logical unit number to use
            _ptrDiskInfo = ptrDiskInfo[ucActiveLUN];
            if (ptrCBW->CBWCB[CBW_OperationCode] != UFI_REQUEST_SENSE) { // if not a sense request reset the sense information
                uMemcpy(&present_sense_data, &sense_data, sizeof(sense_data));
            }
            if (ptrCBW->dmCBWFlags & CBW_IN_FLAG) {
                switch (ptrCBW->CBWCB[CBW_OperationCode]) {
                case UFI_FORMAT_UNIT:
                    break;
                case UFI_INQUIRY:
                    {
                        CBW_INQUIRY_COMMAND *ptrInquiry = (CBW_INQUIRY_COMMAND *)ptrCBW->CBWCB;
                        unsigned char ucLogicalUnitNumber = (ptrInquiry->ucLogicalUnitNumber_EVPD >> 5);
                        QUEUE_TRANSFER inquiry_length;
                        unsigned char *ptrInquiryData = fnGetPartition(ucLogicalUnitNumber, &inquiry_length);
                        if (ptrInquiryData == 0) {                       // partition doesn't exist
                            csw.bCSWStatus = CSW_STATUS_COMMAND_FAILED;
                        }
                        else {
                            fnWrite(USBPortID_MSD, ptrInquiryData, inquiry_length); // respond to the request and then return 
                        }
                    }
                    break;
                case UFI_MODE_SENSE_6:
                    {
                        MODE_PARAMETER_6 SelectDataWP;
                        uMemcpy(&SelectDataWP, &SelectData, sizeof(SelectData));
                        if (_ptrDiskInfo->usDiskFlags & WRITE_PROTECTED_SD_CARD) {
                            SelectDataWP.ucWP_DPOFUA = PAR6_WRITE_PROTECTED; // the medium is write protected
                        }
                        fnWrite(USBPortID_MSD, (unsigned char *)&SelectDataWP, sizeof(SelectData)); // respond to the request and then return 
                    }
                    break;
                case UFI_START_STOP:
                case UFI_MODE_SELECT:
              //case UFI_MODE_SENSE:
                case UFI_PRE_ALLOW_REMOVAL:
                    break;
                case UFI_READ_10:
                case UFI_READ_12:
                    {                                
                        CBW_READ_10 *ptrRead = (CBW_READ_10 *)ptrCBW->CBWCB;
                        ulLogicalBlockAdr = ((ptrRead->ucLogicalBlockAddress[0] << 24) | (ptrRead->ucLogicalBlockAddress[1] << 16) | (ptrRead->ucLogicalBlockAddress[2] << 8) | ptrRead->ucLogicalBlockAddress[3]);
                        if (ptrRead->ucOperationCode == UFI_READ_12) {
                            CBW_READ_12 *ptrRead = (CBW_READ_12 *)ptrCBW->CBWCB;
                            ulReadBlock = ((ptrRead->ucTransferLength[0] << 24) | (ptrRead->ucTransferLength[1] << 16) | (ptrRead->ucTransferLength[2] << 8) | ptrRead->ucTransferLength[3]);
                        }
                        else {
                            ulReadBlock = ((ptrRead->ucTransferLength[0] << 8) | ptrRead->ucTransferLength[1]); // the total number of blocks to be returned
                        }
                        fnContinueMedia();
                    }
                    continue;                                            // the transfer has not completed so don't send termination stage yet
                case UFI_READ_CAPACITY:
                    {
                        CBW_READ_CAPACITY_DATA formatData;
                        unsigned long ulLastSector = (_ptrDiskInfo->ulSD_sectors - 1);
                        formatData.ucBlockLengthInBytes[0] = 0;
                        formatData.ucBlockLengthInBytes[1] = 0;
                        formatData.ucBlockLengthInBytes[2] = (unsigned char)(_ptrDiskInfo->utFAT.usBytesPerSector >> 8);
                        formatData.ucBlockLengthInBytes[3] = (unsigned char)(_ptrDiskInfo->utFAT.usBytesPerSector);
                        formatData.ucLastLogicalBlockAddress[0] = (unsigned char)(ulLastSector >> 24);
                        formatData.ucLastLogicalBlockAddress[1] = (unsigned char)(ulLastSector >> 16);
                        formatData.ucLastLogicalBlockAddress[2] = (unsigned char)(ulLastSector >> 8);
                        formatData.ucLastLogicalBlockAddress[3] = (unsigned char)(ulLastSector);
                        fnWrite(USBPortID_MSD, (unsigned char *)&formatData, sizeof(formatData));
                    }
                    break;
                case UFI_READ_FORMAT_CAPACITY:
                    {
                        CBW_FORMAT_CAPACITIES *ptrCapacities = (CBW_FORMAT_CAPACITIES *)ptrCBW->CBWCB;
                        unsigned short usLengthAccepted = ((ptrCapacities->ucAllocationLength[0] << 8) | (ptrCapacities->ucAllocationLength[1]));
                        CBW_CAPACITY_LIST mediaCapacity;
                        uMemcpy(&mediaCapacity, &formatCapacityNoMedia, sizeof(CBW_CAPACITY_LIST)); // assume no disk
                        if (_ptrDiskInfo->usDiskFlags & (DISK_MOUNTED | DISK_UNFORMATTED)) {
                            if (_ptrDiskInfo->usDiskFlags & DISK_FORMATTED) {
                                mediaCapacity.capacityDescriptor.ucDescriptorCode = DESC_CODE_FORMATTED_MEDIA;
                            }
                            else {
                                mediaCapacity.capacityDescriptor.ucDescriptorCode = DESC_CODE_UNFORMATTED_MEDIA;
                            }
                            mediaCapacity.capacityDescriptor.ucNumberOfBlocks[3] = (unsigned char)_ptrDiskInfo->ulSD_sectors;
                            mediaCapacity.capacityDescriptor.ucNumberOfBlocks[2] = (unsigned char)(_ptrDiskInfo->ulSD_sectors >> 8);
                            mediaCapacity.capacityDescriptor.ucNumberOfBlocks[1] = (unsigned char)(_ptrDiskInfo->ulSD_sectors >> 16);
                            mediaCapacity.capacityDescriptor.ucNumberOfBlocks[0] = (unsigned char)(_ptrDiskInfo->ulSD_sectors >> 24);
                        }                                
                        if (usLengthAccepted > sizeof(CBW_CAPACITY_LIST)) {
                            usLengthAccepted = sizeof(CBW_CAPACITY_LIST);
                        }
                        fnWrite(USBPortID_MSD, (unsigned char *)&mediaCapacity, usLengthAccepted);
                    }
                    break;
                case UFI_REQUEST_SENSE:
                    fnWrite(USBPortID_MSD, (unsigned char *)&present_sense_data, sizeof(present_sense_data));                
                    break;
                case UFI_REZERO_UNIT:
                case UFI_SEEK_10:
                case UFI_SEND_DIAGNOSTIC:
                case UFI_VERIFY:
                case UFI_WRITE_AND_VERIFY:
                default:
                    break;
                }
            }
            else {                                                       // OUT types
                switch (ptrCBW->CBWCB[CBW_OperationCode]) {
    #if defined MEMORY_SWAP
                case UFI_START_STOP:
                    {
                        CBW_START_STOP_UNIT *ptrStartStopUnit = (CBW_START_STOP_UNIT *)ptrCBW->CBWCB;
                        if ((ptrStartStopUnit->ucPowerCondition & START_STOP_UNIT_POWER_CONDITION) == START_STOP_UNIT_POWER_CONDITION_START_VALID) { // process the start and LOEJ bits
                            if ((ptrStartStopUnit->ucPowerCondition & START_STOP_UNIT_POWER_CONDITION_START_BIT) != 0) {
                                // power up
                            }
                            else {
                                // power down
                                //
                                fnHandleSwap(0);
                            }
                            if ((ptrStartStopUnit->ucPowerCondition & START_STOP_UNIT_POWER_CONDITION_LOEJ) != 0) {
                                // eject
                            }
                        }
                    }
                    break;
    #endif
                case UFI_TEST_UNIT_READY:
                    {
                        if ((_ptrDiskInfo->usDiskFlags & (DISK_MOUNTED | DISK_UNFORMATTED)) == 0) { // if the partition is not ready
                            csw.bCSWStatus = CSW_STATUS_COMMAND_FAILED;
                        }
        #if defined RESET_ON_EJECT
                        else {
                            if (iSoftwareState[DISK_C] != SW_PROGRAMMING) { // ignore while programming
                                uTaskerMonoTimer(OWN_TASK, (DELAY_LIMIT)(3 * SEC), (TIMEOUT_USB_LOADING_COMPLETE_C)); // retrigger the MSD connection timer (the Test Unit Ready is sent event 1s from the host) - if it is missing for > 3s the unit will reset
                            }
                        }
        #endif
                    }
                    break;
                case UFI_WRITE_10:
                case UFI_WRITE_12:
                    {
                        CBW_WRITE_10 *ptrWrite = (CBW_WRITE_10 *)ptrCBW->CBWCB;
                        ulLogicalBlockAdr = ((ptrWrite->ucLogicalBlockAddress[0] << 24) | (ptrWrite->ucLogicalBlockAddress[1] << 16) | (ptrWrite->ucLogicalBlockAddress[2] << 8) | ptrWrite->ucLogicalBlockAddress[3]);
                        if (ptrWrite->ucOperationCode == UFI_WRITE_12) {
                            CBW_WRITE_12 *ptrWrite = (CBW_WRITE_12 *)ptrCBW->CBWCB;
                            ulWriteBlock = ((ptrWrite->ucTransferLength[0] << 24) | (ptrWrite->ucTransferLength[1] << 16) | (ptrWrite->ucTransferLength[2] << 8) | ptrWrite->ucTransferLength[3]);
                        }
                        else {
                            ulWriteBlock = ((ptrWrite->ucTransferLength[0] << 8) | ptrWrite->ucTransferLength[1]); // the total number of blocks to be written
                        }
                        ulWriteBlock *= BYTES_PER_SECTOR;                // convert to byte count
        #if defined MAC_OS_X_WORKAROUND
                        ulNewWriteBlock[ucActiveLUN] = ulWriteBlock;     // mark that a new block write is starting, together with its size
        #endif
                    }
                    continue;                                            // data will follow so don't reply with CSW stage yet
                }
            }
        }
        fnWrite(USBPortID_MSD, (unsigned char *)&csw, sizeof(csw));      // close with CSW stage
    }
    #endif
}

// The application must always supply this routine and return its device descriptor when requested
// This example shows a single fixed device configuration but multiple configurations could be selected (eg. for experimental use)
//
extern void *fnGetUSB_device_descriptor(unsigned short *usLength)
{
    *usLength = sizeof(device_descriptor);
    return (void *)&device_descriptor;
}

// The application must always supply this routine and return its configuration descriptor when requested
// This example shows a single fixed configuration but multiple configurations could be selected (eg. for programmable device types)
//
extern void *fnGetUSB_config_descriptor(unsigned short *usLength)
{
#if defined USB_MSD_DEVICE_LOADER
    if (iDeviceMode == 0) {
        return 0;                                                        // device mode is not active so return 0 for host mode
    }
#endif
    *usLength = sizeof(config_descriptor);
    return (void *)&config_descriptor;
}

#if defined USB_STRING_OPTION
    #if defined USB_RUN_TIME_DEFINABLE_STRINGS
// This routine constructs a USB string descriptor for use by the USB interface during emumeration
// The new string has to respect the descriptor format (using UNICODE) and is built in preparation so that it can be passed in an interrupt
//
static void fnSetSerialNumberString(CHAR *ptrSerialNumber) {
    unsigned char ucDescriptorLength = (sizeof(USB_STRING_DESCRIPTOR) - 2);
    unsigned char *ptrString;
    int iStringLength = (uStrlen(ptrSerialNumber)*2);
    if (iStringLength == 0) {
        ucDescriptorLength += 2;                                         // space for a null-terminator
    }
    else {
        ucDescriptorLength += iStringLength;
    }
    if (SerialNumberDescriptor == 0) {
        SerialNumberDescriptor = uMalloc(ucDescriptorLength);            // get memory to store the string descriptor
        SerialNumberDescriptor->bLength = ucDescriptorLength;
        SerialNumberDescriptor->bDescriptorType = DESCRIPTOR_TYPE_STRING;
    }
    ptrString = &SerialNumberDescriptor->unicode_string_space[0];
    if (iStringLength == 0) {
        *ptrString++ = 0;                                                // when no string add a null-terminator
        *ptrString++ = 0;
    }
    else {
        while (*ptrSerialNumber != 0) {
            *ptrString++ = *ptrSerialNumber++;                           // unicode - english string (requiring just zeros to be added)
            *ptrString++ = 0;
        }
    }
}
    #endif


// This routine must always be supplied by the user if usb strings are supported
//
extern unsigned char *fnGetUSB_string_entry(unsigned char ucStringRef, unsigned short *usLength)
{
    if (ucStringRef > LAST_STRING_INDEX) {
        return 0;                                                        // invalid string index
    }
    #if defined USB_RUN_TIME_DEFINABLE_STRINGS                           // if variable strings are supported
    if (ucStringTable[ucStringRef][0] == 0) {                            // no length defined, meaning it is a run-time definabled string
        switch (ucStringRef) {
        case SERIAL_NUMBER_STRING_INDEX:                                 // USB driver needs to know what string is used as serial number
            *usLength = (unsigned short)SerialNumberDescriptor->bLength; // return length and location of the user defined serial number
            return (unsigned char *)SerialNumberDescriptor;
        default:
            return 0;                                                    // invalid string index
        }
    }
    else {
        *usLength = ucStringTable[ucStringRef][0];                       // the length of the string
    }
    #else
    *usLength = ucStringTable[ucStringRef][0];                           // the length of the string
    #endif
    return ((unsigned char *)ucStringTable[ucStringRef]);                // return a pointer to the string
}
#endif



// Endpoint 0 call-back for any non-supported control transfers.
// This can be called with either setup frame content (iType != 0) or with data belonging to following OUT frames.
// TERMINATE_ZERO_DATA must be returned to setup tokens with NO further data, when there is no response sent.
// BUFFER_CONSUMED_EXPECT_MORE is returned when extra data is to be received.
// STALL_ENDPOINT should be returned if the request in the setup frame is not expected.
// Return BUFFER_CONSUMED in all other cases.
//
// If further data is to be received, this may arrive in multiple frames and the callback needs to manage this to be able to know when the data is complete
//
static int control_callback(unsigned char *ptrData, unsigned short length, int iType)
{
    static unsigned short usExpectedData = 0;
    static unsigned char ucCollectingMode = 0;
    int iRtn = BUFFER_CONSUMED;
    switch (iType) {
    #if defined USB_MSD_DEVICE_LOADER                                    // {16}
    case ENDPOINT_CLEARED:                                               // halted endpoint has been freed
        if (*ptrData == (IN_ENDPOINT | USB_MSD_IN_ENDPOINT_NUMBER)) {    // this BULK IN is the only endpoint that is expect to be halted
            fnWrite(USBPortID_MSD, (unsigned char *)&csw, sizeof(csw));  // answer with the failed CSW on next endpoint OUT
        }
        return TERMINATE_ZERO_DATA;                                      // send zero data to terminate the halt phase
    #endif
    case SETUP_DATA_RECEPTION:
        {
            USB_SETUP_HEADER *ptrSetup = (USB_SETUP_HEADER *)ptrData;    // interpret the received data as a setup header
    #if !defined HID_LOADER                                              // {16}
            if ((ptrSetup->bmRequestType & ~STANDARD_DEVICE_TO_HOST) != REQUEST_INTERFACE_CLASS) { // 0x21
                return STALL_ENDPOINT;                                   // stall on any unsupported request types
            }
    #endif
            usExpectedData = ptrSetup->wLength[0];                       // the amount of additional data which is expected to arrive from the host belonging to this request
            usExpectedData |= (ptrSetup->wLength[1] << 8);
            if (ptrSetup->bmRequestType & STANDARD_DEVICE_TO_HOST) {     // request for information
                switch (ptrSetup->bRequest) {
    #if defined HID_LOADER                                               // {16}
                case USB_REQUEST_GET_DESCRIPTOR:                         // standard request
                    if (ptrSetup->wValue[1] == DESCRIPTOR_TYPE_REPORT) { // (0x22)
                        fnWrite(USB_control, (unsigned char *)&ucHID_Report, sizeof(ucHID_Report)); // return directly (non-buffered)
                    }
                    else {
                        return STALL_ENDPOINT;                           // not supported
                    }
                    break;
    #endif
    #if defined USB_MSD_DEVICE_LOADER
                case BULK_ONLY_MASS_STORAGE_RESET:
                    break;
                case GET_MAX_LUN:
                    {
                        static const unsigned char ucPartitions = (NUMBER_OF_PARTITIONS - 1);
                        if ((ptrSetup->wValue[0] != 0) || (ptrSetup->wValue[1] != 0) || (ptrSetup->wLength[0] != 1) || (ptrSetup->wLength[1] != 0)) {
                            return STALL_ENDPOINT;                       // stall if bad request format
                        }
                        else {
                            fnWrite(USB_control, (unsigned char *)&ucPartitions, sizeof(ucPartitions)); // return directly (non-buffered)
                        }
                    }
                    break;
    #endif
    #if defined USE_USB_CDC                                              // {31}
                case GET_LINE_CODING:                                    // 0x21
                    fnInterruptMessage(OWN_TASK, (unsigned char)(EVENT_RETURN_PRESENT_UART_SETTING));
                    break;
    #endif
                default:
                    return STALL_ENDPOINT;                               // stall on any unsupported requests
                }
            }
            else {                                                       // command
                iRtn = TERMINATE_ZERO_DATA;                              // acknowledge receipt of the request if we have no data to return (default)
                switch (ptrSetup->bRequest) {
    #if defined HID_LOADER                                               // {16}
                case HID_SET_IDLE:                                       // set idle request (0x0a)
                    break;                                               // accept the request and terminate
                case HID_SET_REPORT:                                     // set report (0x09) (data will follow)
                    ucCollectingMode = ptrSetup->bRequest;
                    return BUFFER_CONSUMED;
    #endif
    #if defined USE_USB_CDC                                              // {31}
                case SET_LINE_CODING:                                    // 0x20 - the host is informing us of parameters to use
                    ucCollectingMode = ptrSetup->bRequest;               // the next OUT frame will contain the settings
                    iRtn = BUFFER_CONSUMED_EXPECT_MORE;                  // the present buffer has been consumed but extra data is subsequently expected
                    break;

                case SET_CONTROL_LINE_STATE:                             // OUT - 0x22 (controls RTS and DTR)
                    break;
    #endif
                default:
                    return STALL_ENDPOINT;                               // stall on any unsupported requests
                }
            }

            if (length <= sizeof(USB_SETUP_HEADER)) {
                return iRtn;                                             // no extra data in this frame
            }
            length -= sizeof(USB_SETUP_HEADER);                          // header handled
            ptrData += sizeof(USB_SETUP_HEADER);
        }
        break;
    case STATUS_STAGE_RECEPTION:                                         // this is the status stage of a control transfer - it confirms that the exchange has completed and can be ignored if not of interest to us
        return BUFFER_CONSUMED;
    default:                                                             // OUT_DATA_RECEPTION
        break;
    }

    if (usExpectedData != 0) {                                           // if we are expecting OUT data
        switch (ucCollectingMode) {                                      // the type of data we are expecting
    #if defined HID_LOADER && !defined KBOOT_HID_LOADER                  // {16}{19}
        case HID_SET_REPORT:
            if (*ptrData++ == HID_IDENTIFIER) {                          // all OUT data packets start with this
                switch (*ptrData++) {                                    // command type
                case HID_BOOT_ERASE_SECTOR:                              // erase (0x01)
                    ptrFlashAddress = (unsigned char *)(*ptrData | (*(ptrData + 1) << 8) | (*(ptrData + 2) << 16) | (*(ptrData + 3) << 24)); // the address of the sector to be erased
                    fnInterruptMessage(OWN_TASK, HID_LOADER_ERASE);      // send event to start sector erase
                    iRtn = TERMINATE_ZERO_DATA;
                    break;
                case HID_BOOT_WRITE_BUFFER_TO_FLASH:                     // program the buffer to flash (0x12)
                    ptrFlashAddress = (unsigned char *)(*ptrData | (*(ptrData + 1) << 8) | (*(ptrData + 2) << 16) | (*(ptrData + 3) << 24)); // the address of the sector to be erased
                    usBuff_length = (unsigned short)(*(ptrData + 4) | (*(ptrData + 5) << 8)); // the buffer size to program
                    fnInterruptMessage(OWN_TASK, HID_LOADER_PROGRAM);    // send event to start buffer to flash programming
                    iRtn = TERMINATE_ZERO_DATA;
                    break;
                case HID_BOOT_WRITE_BUFFER:                              // write to buffer (0x13)
                    {
                        unsigned long ulThisOffset = (unsigned long)(*ptrData | (*(ptrData + 1) << 8) | (*(ptrData + 2) << 16) | (*(ptrData + 3) << 24));
                        unsigned short usBuff_length = (unsigned short)(*(ptrData + 4) | (*(ptrData + 5) << 8)); // the length to copy
                        if (ulThisOffset < sizeof(ucSectorBuffer)) {     // check that the offset is within the buffer
                            // Collect data to buffer
                            //
                            if ((ulThisOffset + usBuff_length) >= sizeof(ucSectorBuffer)) {
                                usBuff_length = (unsigned short)(sizeof(ucSectorBuffer) - ulThisOffset); // protect buffer
                            }
                            uMemcpy(&ucSectorBuffer[ulThisOffset], (ptrData + 6), usBuff_length); // copy OUT buffer to the programming buffer
                        }
                        iRtn = TERMINATE_ZERO_DATA;
                    }
                    break;
                }
            }
            break;
    #endif
    #if defined USE_USB_CDC                                              // {31}
            case SET_LINE_CODING:
                fnNewUART_settings(ptrData, length, usExpectedData);     // set the new UART mode (the complete data will always be received here so we can always terminate now, otherwise BUFFER_CONSUMED_EXPECT_MORE would be returned until complete)
                break;
    #endif
        default:
            break;
        }
        ucCollectingMode = 0;                                            // reset to avoid repeat of command when subsequent, invalid commands are received
        if (length >= usExpectedData) {
            usExpectedData = 0;                                          // all of the expected data belonging to this transfer has been received
            return TERMINATE_ZERO_DATA;
        }
        else {
            usExpectedData -= length;                                    // remaining length to be received before transaction has completed
        }
        return BUFFER_CONSUMED_EXPECT_MORE;
    }
    return iRtn;
}


// The USB interface is configured by opening the USB interface once for the default control endpoint 0,
// followed by an open of each endpoint to be used (each endpoint has its own handle). Each endpoint can use an optional call-back
// or can define a task to be woken on OUT frames. Transmission can use direct memory method or else an output buffer (size defined by open),
// and receptions can use an optional input buffer (size defined by open).
//
static void fnConfigureUSB(void)
{
    USBTABLE tInterfaceParameters;                                       // table for passing information to driver
    #if !defined USE_USB_CDC
    QUEUE_HANDLE endpointNumber = 1;
    #endif

    tInterfaceParameters.Endpoint = 0;                                   // set USB default control endpoint for configuration
    #if defined USB_HS_INTERFACE                                         // {7}
    tInterfaceParameters.usConfig = USB_HIGH_SPEED;                      // user high speed interface
    #else
    tInterfaceParameters.usConfig = USB_FULL_SPEED;                      // full-speed, rather than low-speed
    #endif
    tInterfaceParameters.usb_callback = control_callback;                // callback for control endpoint to enable class exchanges to be handled
    tInterfaceParameters.queue_sizes.TxQueueSize = 0;                    // no tx buffering
    tInterfaceParameters.queue_sizes.RxQueueSize = 0;                    // no rx buffering
    #if defined _KINETIS
    tInterfaceParameters.ucClockSource = INTERNAL_USB_CLOCK;             // use system clock and dividers
    #else
    tInterfaceParameters.ucClockSource = EXTERNAL_USB_CLOCK;             // use 48MHz crystal directly as USB clock (recommended for lowest jitter)
    #endif
    #if defined _LPC23XX || defined _LPC17XX
    tInterfaceParameters.ucEndPoints = 5;                                // due to fixed endpoint ordering in the LPC endpoints up to 5 are used
    #else
    tInterfaceParameters.ucEndPoints = NUMBER_OF_ENDPOINTS;              // number of endpoints, in addition to EP0
    #endif
    tInterfaceParameters.owner_task = OWN_TASK;                          // local task receives USB state change events
    #if defined USE_USB_OTG_CHARGE_PUMP
    tInterfaceParameters.OTG_I2C_Channel = IICPortID;                    // let the driver open its own IIC interface, if not yet open
    #endif
    USB_control = fnOpen(TYPE_USB, 0, &tInterfaceParameters);            // open the default control endpoint with defined configurations (reserves resources but only control is active)
    #if defined USB_HS_INTERFACE
    tInterfaceParameters.usEndpointSize = 512;                           // endpoint queue size
    #else
    tInterfaceParameters.usEndpointSize = 64;                            // endpoint queue size (2 buffers of this size will be created for reception)
    #endif
    #if defined HID_LOADER
    tInterfaceParameters.usb_callback = 0;                               // no call-back used
        #if defined KBOOT_HID_LOADER
    tInterfaceParameters.Endpoint = endpointNumber++;                    // set USB endpoints to act as an input/output pair - transmitter (IN)
    tInterfaceParameters.Paired_RxEndpoint = endpointNumber++;           // receiver (OUT)
        #else
    tInterfaceParameters.Endpoint = endpointNumber++;                    // IN endpoint
    tInterfaceParameters.Paired_RxEndpoint = 0;                          // new receiver and so not paired
    tInterfaceParameters.queue_sizes.TxQueueSize = tInterfaceParameters.queue_sizes.RxQueueSize = 0;
        #endif
        #if defined WAKE_BLOCKED_USB_TX
    tInterfaceParameters.low_water_level = 0;
        #endif
        #if defined USB_HS_INTERFACE                                     // {7}
    tInterfaceParameters.queue_sizes.RxQueueSize = (USBHS_RX_QUEUE_SIZE);// input queue capable of holding larger blocks of data to be saved
        #else
    tInterfaceParameters.queue_sizes.RxQueueSize = BYTES_PER_SECTOR;     // optional input queue (used only when no callback defined) and large enough to hold a full sector
        #endif
    tInterfaceParameters.queue_sizes.TxQueueSize = 1024;                 // additional tx buffer
    USBPortID_HID = fnOpen(TYPE_USB, 0, &tInterfaceParameters);          // open the endpoints with defined configurations (initially inactive)
    #endif
    #if defined USB_MSD_DEVICE_LOADER
    tInterfaceParameters.Paired_RxEndpoint = endpointNumber;             // receiver (OUT)
        #if !defined USB_SIMPLEX_ENDPOINTS
    endpointNumber++;
        #endif
    tInterfaceParameters.Endpoint = endpointNumber++;                    // set USB endpoints to act as an input/output pair - transmitter (IN)
    tInterfaceParameters.usb_callback = mass_storage_callback;           // allow receptions to be 'peeked' by callback
        #if !defined HID_LOADER
            #if defined USB_HS_INTERFACE
    tInterfaceParameters.queue_sizes.RxQueueSize = (USBHS_RX_QUEUE_SIZE);// input queue capable of holding larger blocks of data to be saved
            #else
    tInterfaceParameters.queue_sizes.RxQueueSize = 512;                  // optional input queue (used only when no callback defined) and large enough to hold a full sector
            #endif
    tInterfaceParameters.queue_sizes.TxQueueSize = 1024;                 // additional tx buffer
            #if defined WAKE_BLOCKED_USB_TX
    tInterfaceParameters.low_water_level = (tInterfaceParameters.queue_sizes.TxQueueSize/2); // TX_FREE event on half buffer empty
            #endif
        #endif
    USBPortID_MSD = fnOpen(TYPE_USB, 0, &tInterfaceParameters);          // open the endpoints with defined configurations (initially inactive)
    #endif
    #if defined USE_USB_CDC                                              // {31}
    tInterfaceParameters.owner_task = TASK_APPLICATION;                  // loader task is woken on receptions
    tInterfaceParameters.usConfig = USB_TERMINATING_ENDPOINT;            // configure the IN endpoint to terminate messages with a zero length frame is a block transmission equals the endpoint size
    tInterfaceParameters.usEndpointSize = 64;
    tInterfaceParameters.Paired_RxEndpoint = 1;                          // receiver (OUT)
    #if defined USB_SIMPLEX_ENDPOINTS
    tInterfaceParameters.Endpoint = 1;                                   // set USB endpoints to act as an input/output pair - transmitter (IN)
    #else
    tInterfaceParameters.Endpoint = 2;                                   // set USB endpoints to act as an input/output pair - transmitter (IN)
    #endif
    tInterfaceParameters.usb_callback = 0;                               // no call-back since we use rx buffer
    tInterfaceParameters.queue_sizes.RxQueueSize = 256;                  // optional input queue (used only when no call-back defined)
    tInterfaceParameters.queue_sizes.TxQueueSize = 256;                  // additional tx buffer
        #if defined WAKE_BLOCKED_USB_TX
    tInterfaceParameters.low_water_level = (tInterfaceParameters.queue_sizes.TxQueueSize/2); // TX_FREE event on half buffer empty
        #endif
    USBPortID_comms = fnOpen(TYPE_USB, 0, &tInterfaceParameters);        // open the endpoints with defined configurations (initially inactive)
    #if defined USB_SIMPLEX_ENDPOINTS
    tInterfaceParameters.Endpoint = 2;                                   // set interrupt endpoint
    #else
    tInterfaceParameters.Endpoint = 3;                                   // set interrupt endpoint
    #endif
    tInterfaceParameters.Paired_RxEndpoint = 0;                          // no pairing
    tInterfaceParameters.owner_task = 0;                                 // don't wake task on reception
    tInterfaceParameters.usb_callback = 0;                               // no call back function
    tInterfaceParameters.queue_sizes.TxQueueSize = tInterfaceParameters.queue_sizes.RxQueueSize = 0; // no buffering
    USBPortID_interrupt = fnOpen(TYPE_USB, 0, &tInterfaceParameters);    // open the endpoint with defined configurations (initially inactive)
    #endif
}

#if defined USE_USB_CDC                                                  // {31}
// Convert UART interface settings format to CDC setting format and report to CDC host
//
static void fnReturnUART_settings(void)
{
    fnWrite(USB_control, (unsigned char *)&uart_setting, sizeof(uart_setting)); // return directly (non-buffered)
}

// Save new UART settings from the CDC host
//
static void fnNewUART_settings(unsigned char *ptrData, unsigned short usLength, unsigned short usExpected)
{
    unsigned char *ptrSet = (unsigned char *)&uart_setting;

    if (usExpected > sizeof(uart_setting)) {                             // some protection against corrupted reception
        usExpected = sizeof(uart_setting);
    }
    if (usLength > usExpected) {
        usLength = usExpected;
    }

    ptrSet += (sizeof(uart_setting) - usExpected);
    uMemcpy(ptrSet, ptrData, usLength);                                  // save to a local copy (no UART is actually modified)
}
#endif

// USB-MSD device
//
#if defined USB_MSD_DEVICE_LOADER
// The output buffer has space so continue with data transfer or terminate
//
static void fnContinueMedia(void)
{
    QUEUE_TRANSFER buffer_space = fnWrite(USBPortID_MSD, 0, 0);          // check whether there is space available in the USB output buffer to queue
    while ((ulReadBlock != 0) && (buffer_space >= BYTES_PER_SECTOR)) {   // send as many blocks as possible as long as the output buffer has space
    #if defined FAT_EMULATION
        if (UTFAT_SUCCESS == fnReadSector(ucActiveLUN, 0, ulLogicalBlockAdr))
    #else
        if (UTFAT_SUCCESS == _fnReadSector(ucActiveLUN, 0, ulLogicalBlockAdr))
    #endif
        {
            fnWrite(USBPortID_MSD, ptrDiskInfo[ucActiveLUN]->ptrSectorData, BYTES_PER_SECTOR);
            buffer_space -= BYTES_PER_SECTOR;
            ulLogicalBlockAdr++;
        }
        else {
                                                                         // error reading media
        }
        ulReadBlock--;
    }
    if (ulReadBlock == 0) {                                              // all blocks have been read and put to the output queue
        if (buffer_space >= sizeof(csw)) {
            fnWrite(USBPortID_MSD, (unsigned char *)&csw, sizeof(csw));  // close with CSW stage
            return;
        }
    }
    if (buffer_space < BYTES_PER_SECTOR) {
        fnDriver(USBPortID_MSD, MODIFY_WAKEUP, (MODIFY_TX | TASK_USB));  // when there is room in the output buffer the task will be woken
    }
}

static int mass_storage_callback(unsigned char *ptrData, unsigned short length, int iType)
{
    if (ulWriteBlock != 0) {                                             // data expected
        return TRANSPARENT_CALLBACK;                                     // handle data in task
    }
    if (uMemcmp(cCBWSignature, ptrData, sizeof(cCBWSignature)) == 0) {
        const UTDISK *_ptrDiskInfo;
        USB_MASS_STORAGE_CBW *ptrCBW = (USB_MASS_STORAGE_CBW *)ptrData;
        ucActiveLUN = ptrCBW->dCBWLUN;                                   // the logical unit number to use
        _ptrDiskInfo = ptrDiskInfo[ucActiveLUN];
        if (fnGetPartition(ptrCBW->dCBWLUN, 0) != 0) {                   // if valid storage device
            if ((ptrCBW->dCBWCBLength > 0) && (ptrCBW->dCBWCBLength <= 16)) { // check for valid length
                if (ptrCBW->dmCBWFlags & CBW_IN_FLAG) {
                    switch (ptrCBW->CBWCB[CBW_OperationCode]) {
                    case UFI_MODE_SENSE_6:
                        {
                            CBW_MODE_SENSE_6 *ptrSense = (CBW_MODE_SENSE_6 *)ptrCBW->CBWCB;
                            unsigned char ucLogicalUnitNumber = (ptrSense->ucLogicalUnitNumber_DBD >> 5);
                            if (fnGetPartition(ucLogicalUnitNumber, 0) != 0) {
                                if ((ptrSense->ucPC_PageCode & MODE_SENSE_6_PAGE_CODE_ALL_PAGES) == MODE_SENSE_6_PAGE_CODE_ALL_PAGES) { // only accept all page request
                                    return TRANSPARENT_CALLBACK;         // accept to be handled by task
                                }
                            }
                        }
                        break;                                           // stall
                    case UFI_REQUEST_SENSE:
                    case UFI_INQUIRY:
                    case UFI_READ_CAPACITY:
                        return TRANSPARENT_CALLBACK;                     // the callback has done its work and the input buffer can now be used
                    case UFI_READ_10:
                    case UFI_READ_12:
                    case UFI_READ_FORMAT_CAPACITY:
                        if (_ptrDiskInfo->usDiskFlags & (DISK_MOUNTED | DISK_UNFORMATTED)) { // only respond when there is media inserted, else stall
                            return TRANSPARENT_CALLBACK;                 // the callback has done its work and the input buffer can now be used
                        }
                        present_sense_data.ucValid_ErrorCode = (VALID_SENSE_DATA | CURRENT_ERRORS);
                        present_sense_data.ucSenseKey = SENSE_NOT_READY;
                        present_sense_data.ucAdditionalSenseCode = DESC_MEDIUM_NOT_PRESENT;
                        break;                                           // stall
                    default:
                        break;                                           // stall
                    }
                }
                else {
                    switch (ptrCBW->CBWCB[CBW_OperationCode]) {
                    case UFI_PRE_ALLOW_REMOVAL:
                    case UFI_TEST_UNIT_READY:
                    case UFI_START_STOP:
                        return TRANSPARENT_CALLBACK;                     // the callback has done its work and the input buffer can now be used
                    case UFI_WRITE_10:
                    case UFI_WRITE_12:
                        if (_ptrDiskInfo->usDiskFlags & (DISK_MOUNTED | DISK_UNFORMATTED)) { // only respond when there media inserted, else stall
                            return TRANSPARENT_CALLBACK;                 // the callback has done its work and the input buffer can now be used
                        }
                        present_sense_data.ucValid_ErrorCode = (VALID_SENSE_DATA | CURRENT_ERRORS);
                        present_sense_data.ucSenseKey = SENSE_NOT_READY;
                        present_sense_data.ucAdditionalSenseCode = DESC_MEDIUM_NOT_PRESENT;
                        break;                                           // stall
                    default:
                        break;
                    }
                }
                uMemcpy(&csw.dCBWTag, ptrCBW->dCBWTag, sizeof(csw.dCBWTag)); // save the tag since we will halt
                uMemcpy(csw.dCSWDataResidue, &ptrCBW->dCBWDataTransferLength, sizeof(csw.dCSWDataResidue)); // since no data will be processed the residue is equal to the requested transfer length
                csw.bCSWStatus = CSW_STATUS_COMMAND_FAILED;              // set fail
            }
        }
    }
    return STALL_ENDPOINT;                                               // stall on any invalid formats
}

// Get partition details - used to ease support of multiple LUNs (Logical Units)
//
static unsigned char *fnGetPartition(unsigned char ucLUN, QUEUE_TRANSFER *length)
{
    if (ucLUN >= NUMBER_OF_PARTITIONS) {
        return 0;                                                        // invalid LUN
    }
    if (length != 0) {
        *length = sizeof(inquiryData);                                   // the inquiry data length
    }
    return (unsigned char *)&inquiryData;                                // the fixed inquiry data
}

// Virtual FAT interface
//
    #if !defined UTFAT12 && !defined FAT_EMULATION
static void fnAddInfoSect(INFO_SECTOR_FAT32 *ptrInfoSector, unsigned long ulFreeCount, unsigned long ulNextFree)
{
    ptrInfoSector->FSI_Free_Count[0] = (unsigned char)(ulFreeCount);
    ptrInfoSector->FSI_Free_Count[1] = (unsigned char)(ulFreeCount >> 8);
    ptrInfoSector->FSI_Free_Count[2] = (unsigned char)(ulFreeCount >> 16);
    ptrInfoSector->FSI_Free_Count[3] = (unsigned char)(ulFreeCount >> 24);
    ptrInfoSector->FSI_Nxt_Free[0]   = (unsigned char)(ulNextFree);
    ptrInfoSector->FSI_Nxt_Free[1]   = (unsigned char)(ulNextFree >> 8);
    ptrInfoSector->FSI_Nxt_Free[2]   = (unsigned char)(ulNextFree >> 16);
    ptrInfoSector->FSI_Nxt_Free[3]   = (unsigned char)(ulNextFree >> 24);
}
    #endif

    #if defined UTFAT12 && defined ENABLE_READBACK && !defined FAT_EMULATION
static unsigned short *fnFillClusterEntry(unsigned short *ptrCluster, unsigned char *ucOffset, unsigned short *uslastEntry, unsigned short usCluster)
{
    unsigned short usNextEntry;
    switch ((*ucOffset)++) {
    case 0:
        usNextEntry = (*uslastEntry | ((usCluster << 12) & 0xf000) | (usCluster & 0x0f00)); // {8}{13}
        *uslastEntry = ((usCluster >> 4) & 0x000f);
        *ptrCluster++ = LITTLE_SHORT_WORD(usNextEntry);
        break;
    case 1:
        usNextEntry = (((usCluster << 8) & 0xff00) | ((usCluster >> 4) & 0x00f0)); // {8}{13}
        usNextEntry |= *uslastEntry;
        *uslastEntry = ((usCluster >> 8) & 0x000f);
        *ptrCluster++ = LITTLE_SHORT_WORD(usNextEntry);
        break;
    case 2:
        usNextEntry = ((usCluster << 4) & 0xfff0);
        usNextEntry |= *uslastEntry;
        *uslastEntry = 0;
        *ptrCluster++ = LITTLE_SHORT_WORD(usNextEntry);
        break;
    case 3:
        *ucOffset = 0;
        usNextEntry = (usCluster & 0x00ff);
        *uslastEntry = usNextEntry;
        *ptrCluster = LITTLE_SHORT_WORD(usNextEntry);
        break;
    }
    return ptrCluster;
}
    #endif

#if !defined FAT_EMULATION
static int _fnReadSector(unsigned char ucDisk, unsigned char *ptrBuffer, unsigned long ulSectorNumber)
{
    UTDISK *ptr_utDisk = &utDisks[ucDisk];
    uMemset(ptr_utDisk->ptrSectorData, 0, BYTES_PER_SECTOR);             // always start with blank sector content
    switch (ulSectorNumber) {
    case 0:                                                              // extended boot record
        {
            EXTENDED_BOOT_RECORD *ptrExtendedBootRecord = (EXTENDED_BOOT_RECORD *)ptr_utDisk->ptrSectorData;
            ptrExtendedBootRecord->EBR_partition_table[0].starting_cylinder = 2; // fixed values
            ptrExtendedBootRecord->EBR_partition_table[0].starting_head     = 0x0c;
            ptrExtendedBootRecord->EBR_partition_table[0].partition_type    = 0x0b;
            ptrExtendedBootRecord->EBR_partition_table[0].ending_cylinder   = 0x38;
            ptrExtendedBootRecord->EBR_partition_table[0].ending_head       = 0xf8;
            ptrExtendedBootRecord->EBR_partition_table[0].ending_sector     = 0xb8;
            ptrExtendedBootRecord->EBR_partition_table[0].start_sector[0]   = BOOT_SECTOR_LOCATION;
            ptrExtendedBootRecord->EBR_partition_table[0].partition_size[0] = (unsigned char)(PARTITION_SIZE);
            ptrExtendedBootRecord->EBR_partition_table[0].partition_size[1] = (unsigned char)(PARTITION_SIZE >> 8);
            ptrExtendedBootRecord->EBR_partition_table[0].partition_size[2] = (unsigned char)(PARTITION_SIZE >> 16);
            ptrExtendedBootRecord->EBR_partition_table[0].partition_size[3] = (unsigned char)(PARTITION_SIZE >> 24);
            ptrExtendedBootRecord->ucCheck55 = 0x55;
            ptrExtendedBootRecord->ucCheckAA = 0xaa;
        }
        break;
    case BOOT_SECTOR_LOCATION:                                           // boot sector
    case (BOOT_SECTOR_LOCATION + BACKUP_ROOT_SECTOR):                    // or backup boot sector
        {
    #if defined UTFAT12
            unsigned long ulSectorCnt = (DISK_SIZE/BYTES_PER_SECTOR);    // total count of sectors on volume
    #endif
            BOOT_SECTOR_FAT32 *ptrBootSector = (BOOT_SECTOR_FAT32 *)ptr_utDisk->ptrSectorData; // the sector memory will also be viewed as fat32 boot sector content
            BOOT_SECTOR_FAT12_FAT16 *ptrBootSector_16 = (BOOT_SECTOR_FAT12_FAT16 *)ptr_utDisk->ptrSectorData; // the sector memeory will also be viewed as fat12/16 boot sector content
            BOOT_SECT_COM *ptr_common;                                   // the sector memory will be viewed as common boot sector content
            ptrBootSector->boot_sector_bpb.BS_jmpBoot[0]     = 0xeb;     // fixed values
    #if !defined UTFAT12
            ptrBootSector->boot_sector_bpb.BS_jmpBoot[1]     = 0x58;
    #endif
            ptrBootSector->boot_sector_bpb.BS_jmpBoot[2]     = 0x90;
            uStrcpy(ptrBootSector->boot_sector_bpb.BS_OEMName, "MSDOS5.0");
            ptr_utDisk->utFAT.usBytesPerSector               = BYTES_PER_SECTOR;
            ptrBootSector->boot_sector_bpb.BPB_BytesPerSec[0]= (unsigned char)(BYTES_PER_SECTOR);
            ptrBootSector->boot_sector_bpb.BPB_BytesPerSec[1]= (unsigned char)(BYTES_PER_SECTOR >> 8);
            ptrBootSector->boot_sector_bpb.BPB_SecPerTrk[0]  = 63;
            ptrBootSector->boot_sector_bpb.BPB_NumHeads[0]   = 0xff;                
            ptrBootSector->boot_sector_bpb.BPB_Media         = FIXED_MEDIA;                
    #if defined UTFAT12
            ptrBootSector->boot_sector_bpb.BS_jmpBoot[1]     = 0x3c;
            ptrBootSector->boot_sector_bpb.BPB_RsvdSecCnt[0] = RESERVED_SECTION_COUNT;
            ptrBootSector->boot_sector_bpb.BPB_NumFATs       = 1;        // define a single fat for simplicity
            ptrBootSector->boot_sector_bpb.BPB_RootEntCnt[1] = (unsigned char)(BYTES_PER_SECTOR >> 8);
            ptrBootSector->boot_sector_bpb.BPB_TotSec16[0]   = (unsigned char)(ulSectorCnt);
            ptrBootSector->boot_sector_bpb.BPB_TotSec16[1]   = (unsigned char)(ulSectorCnt >> 8);
        #if defined MAC_OS_X_WORKAROUND
            ptr_utDisk->utFAT.ucSectorsPerCluster = 8;                   // use 4k clusters to allow complete 1MByte memory to be controlled in a single fat sector
            ulSectorCnt = ((((DISK_SIZE/BYTES_PER_SECTOR) - RESERVED_SECTION_COUNT + 1)/341)/(ptr_utDisk->utFAT.ucSectorsPerCluster * 2)); // size of FAT12 in sectors
            if (ulSectorCnt == 0) {                                      // round up to 1 if the size rounds down to zero
                ulSectorCnt = 1;
            }
        #else
            ptr_utDisk->utFAT.ucSectorsPerCluster = 1;
            ulSectorCnt = (((DISK_SIZE/BYTES_PER_SECTOR) - RESERVED_SECTION_COUNT + 1)/257); // size of FAT12 in sectors
        #endif
          //if (ulSectorCnt > 0xfe) {
          //    ulSectorCnt = (0xfe + 8 + ((0xfe * BYTES_PER_SECTOR)/sizeof(unsigned short)));
          //    ptrBootSector->boot_sector_bpb.BPB_TotSec16[0]   = (unsigned char)(ulSectorCnt);
          //    ptrBootSector->boot_sector_bpb.BPB_TotSec16[1]   = (unsigned char)(ulSectorCnt >> 8);
          //    ulSectorCnt = 0xfe;                                      // limit size of FAT12
          //}
            ptrBootSector->boot_sector_bpb.BPB_FATSz16[0] = (unsigned char)(ulSectorCnt);
            ptrBootSector->boot_sector_bpb.BPB_FATSz16[1] = (unsigned char)(ulSectorCnt >> 8);
            ptr_utDisk->ulLogicalBaseAddress = (RESERVED_SECTION_COUNT + 1 + ulSectorCnt);
            ptr_common = &ptrBootSector_16->bs_common;
            uMemcpy(ptr_common->BS_FilSysType, "FAT12   ", 8);           // note that FAT12 is used
            ulFAT32size = ulSectorCnt;
            ptr_utDisk->ulVirtualBaseAddress = (ptr_utDisk->ulLogicalBaseAddress + 32); // fat12/16 has a fixed 32 sector root
            ptr_utDisk->utFAT.ulFAT_start = (ptr_utDisk->ulLogicalBaseAddress - ulFAT32size);
            ptr_utDisk->utFAT.ulFatSize = ulFAT32size;
            ptrBootSector_16->boot_sector_bpb.BPB_SecPerClus = ptr_utDisk->utFAT.ucSectorsPerCluster;
    #else
            ptrBootSector->boot_sector_bpb.BPB_HiddSec[0]    = BOOT_SECTOR_LOCATION;
            ptrBootSector->boot_sector_bpb.BPB_NumFATs       = NUMBER_OF_FATS;
            ptrBootSector->boot_sector_bpb.BPB_RsvdSecCnt[0] = RESERVED_SECTION_COUNT;
            ptrBootSector->boot_sector_bpb.BPB_TotSec32[0]   = (unsigned char)(PARTITION_SIZE);
            ptrBootSector->boot_sector_bpb.BPB_TotSec32[1]   = (unsigned char)(PARTITION_SIZE >> 8);
            ptrBootSector->boot_sector_bpb.BPB_TotSec32[2]   = (unsigned char)(PARTITION_SIZE >> 16);
            ptrBootSector->boot_sector_bpb.BPB_TotSec32[3]   = (unsigned char)(PARTITION_SIZE >> 24);
        #if PARTITION_SIZE <= 532480                                     // disks up to 260MB
            ptr_utDisk->utFAT.ucSectorsPerCluster = 1;
        #elif PARTITION_SIZE <= 16777216                                 // disks up to 8GB
            ptr_utDisk->utFAT.ucSectorsPerCluster = 8;
        #elif PARTITION_SIZE <= 33554432                                 // disks up to 16GB
            ptr_utDisk->utFAT.ucSectorsPerCluster = 16;
        #elif PARTITION_SIZE <= 67108864                                 // disks up to 32GB
            ptr_utDisk->utFAT.ucSectorsPerCluster = 32;
        #else                                                            // greater than 32GB
            ptr_utDisk->utFAT.ucSectorsPerCluster = 64;
        #endif
            ulFAT32size = (((256 * ptr_utDisk->utFAT.ucSectorsPerCluster) + NUMBER_OF_FATS)/2);
            ulFAT32size = (((PARTITION_SIZE - RESERVED_SECTION_COUNT) + (ulFAT32size - 1)) / ulFAT32size);
            ptrBootSector->BPB_FATSz32[0]   = (unsigned char)(ulFAT32size);
            ptrBootSector->BPB_FATSz32[1]   = (unsigned char)(ulFAT32size >> 8);
            ptrBootSector->BPB_FATSz32[2]   = (unsigned char)(ulFAT32size >> 16);
            ptrBootSector->BPB_FATSz32[3]   = (unsigned char)(ulFAT32size >> 24);
            ptrBootSector->BPB_RootClus[0]  = 2;
            ptrBootSector->BPB_FSInfo[0]    = 1;
            ptrBootSector->BPB_BkBootSec[0] = BACKUP_ROOT_SECTOR;
            ptr_utDisk->ulLogicalBaseAddress = (BOOT_SECTOR_LOCATION + RESERVED_SECTION_COUNT + (ulFAT32size * NUMBER_OF_FATS));
            ptr_common = &ptrBootSector->bs_common;
            uMemcpy(ptr_common->BS_FilSysType, "FAT32   ", 8);
            ptr_utDisk->ulVirtualBaseAddress = ptr_utDisk->ulLogicalBaseAddress - (2 * ptr_utDisk->utFAT.ucSectorsPerCluster);
            ptr_utDisk->utFAT.ulFAT_start = (ptr_utDisk->ulLogicalBaseAddress - ulFAT32size);
            ptr_utDisk->utFAT.ulFatSize = ulFAT32size;
            ptrBootSector->boot_sector_bpb.BPB_SecPerClus = ptr_utDisk->utFAT.ucSectorsPerCluster;
    #endif
            ptr_common->BS_DrvNum  = 0x80;
            ptr_common->BS_BootSig = 0x29;
            ptr_common->BS_VolID[0] = 1;
            ptr_common->BS_VolID[1] = 2;
            ptr_common->BS_VolID[2] = 3;
            ptr_common->BS_VolID[3] = 4;
            uMemcpy(ptr_common->BS_VolLab, DISK_NAME, 11);
    #if EMULATED_FAT_LUMS > 1
            ptr_common->BS_VolLab[DISK_NAME_INC] += ucDisk;
    #endif
            uMemcpy(ptr_utDisk->cVolumeLabel, ptr_common->BS_VolLab, 11);
            ptrBootSector->ucCheck55 = 0x55;                             // mark that the sector is valid
            ptrBootSector->ucCheckAA = 0xaa;
        }
        break;

    #if !defined UTFAT12
    case (BOOT_SECTOR_LOCATION + 1):
        {
            INFO_SECTOR_FAT32 *ptrInfoSector;
            unsigned long ulFreeCount;
            ptrInfoSector = (INFO_SECTOR_FAT32 *)ptr_utDisk->ptrSectorData;
            ulFreeCount = (((DISK_SIZE/BYTES_PER_SECTOR) - RESERVED_SECTION_COUNT - BOOT_SECTOR_LOCATION - (NUMBER_OF_FATS * ulFAT32size))/ptr_utDisk->utFAT.ucSectorsPerCluster);
            ptrInfoSector->FSI_LeadSig[3] = 0x41;
            ptrInfoSector->FSI_LeadSig[2] = 0x61;
            ptrInfoSector->FSI_LeadSig[1] = 0x52;
            ptrInfoSector->FSI_LeadSig[0] = 0x52;
            ptrInfoSector->FSI_StrucSig[3] = 0x61;
            ptrInfoSector->FSI_StrucSig[2] = 0x41;
            ptrInfoSector->FSI_StrucSig[1] = 0x72;
            ptrInfoSector->FSI_StrucSig[0] = 0x72;                                 
            fnAddInfoSect(ptrInfoSector, (ulFreeCount - 1), 3);          // one cluster occupied by root directory by default and first useable cluster number is 3
            ptrInfoSector->FSI_StrucSig[3] = 0x61;
            ptrInfoSector->FSI_StrucSig[2] = 0x41;
            ptrInfoSector->FSI_StrucSig[1] = 0x72;
            ptrInfoSector->FSI_StrucSig[0] = 0x72;
            ptrInfoSector->FSI_TrailSig[3] = 0xaa;
            ptrInfoSector->FSI_TrailSig[2] = 0x55;
        }
        break;
    #endif

    default:
        if (ptr_utDisk->ulLogicalBaseAddress == ulSectorNumber) {        // root directory (first sector)
            DIR_ENTRY_STRUCTURE_FAT32 *ptrVolumeEntry = (DIR_ENTRY_STRUCTURE_FAT32 *)ptr_utDisk->ptrSectorData;
            uMemcpy(ptrVolumeEntry, &root_file[ucDisk], BYTES_PER_SECTOR); // {28} add the software information
            uMemcpy(ptrVolumeEntry->DIR_Name, ptr_utDisk->cVolumeLabel, 11); //add the volume ID
            ptrVolumeEntry->DIR_Attr = DIR_ATTR_VOLUME_ID;
          //ptrVolumeEntry++;                                            // {28} move to first (and only) file
          //uMemcpy(ptrVolumeEntry, &root_file[ucDisk], (BYTES_PER_SECTOR - sizeof(DIR_ENTRY_STRUCTURE_FAT32))); // {28} add the software information
        }
        else if ((ptr_utDisk->ulLogicalBaseAddress + 1) == ulSectorNumber) { // root directory (second sector needs to be supported to allow long file names and MAC OS X hidden objects)
            DIR_ENTRY_STRUCTURE_FAT32 *ptrVolumeEntry = (DIR_ENTRY_STRUCTURE_FAT32 *)ptr_utDisk->ptrSectorData;
            uMemcpy(ptrVolumeEntry, (((unsigned char *)&root_file[ucDisk]) + BYTES_PER_SECTOR), (BYTES_PER_SECTOR)); // add and further entries that may have been written by the PC
        }
        else if ((ulSectorNumber >= (ptr_utDisk->utFAT.ulFAT_start)) && (ulSectorNumber < (ptr_utDisk->utFAT.ulFAT_start + ptr_utDisk->utFAT.ulFatSize))) { // FAT area
            if (ulSectorNumber == (ptr_utDisk->utFAT.ulFAT_start)) {     // only first fat sector is filled with an initial empty fat pattern
    #if defined UTFAT12
                uMemcpy(ptr_utDisk->ptrSectorData, ucEmptyFAT12, sizeof(ucEmptyFAT12)); // set pattern for an empty fat with no clusters allocated
    #else
                uMemcpy(ptr_utDisk->ptrSectorData, ucEmptyFAT32, sizeof(ucEmptyFAT32));
    #endif
            }
    #if !defined ENABLE_READBACK
            else {
                break;
            }
    #endif
            if (iSoftwareState[ucDisk] == SW_AVAILABLE) {                // if there is software loaded
    #if defined UTFAT12                                                  // {17} this setting became inverted and stopped read-back from operating correctly in FAT12 formatted mode
                unsigned short *ptrCluster = (unsigned short *)ptr_utDisk->ptrSectorData;
        #if defined ENABLE_READBACK
                static unsigned long  ulFileSize = 0;
                static unsigned short uslastEntry = 0;                   // maintain values between FAT sections
                static unsigned short usCluster = 0;
                static unsigned char  ucOffset = 0;
                static int iAnswer = 0;

                unsigned long ulFAT_sector = (ulSectorNumber - ptr_utDisk->utFAT.ulFAT_start);

                if (ulFAT_sector == 0) {                                 // first fat sector
            #if EMULATED_FAT_LUMS > 1
                    fnGetParsFile((unsigned char *)ptr_disk_location[ucDisk], ptr_utDisk->ptrSectorData, (ROOT_FILE_ENTRIES * sizeof(DIR_ENTRY_STRUCTURE_FAT32))); // get the software length according to software file content
            #else
                    ulFileSize = fnGetFileSize((const LFN_ENTRY_STRUCTURE_FAT32 *)fnGetFlashAdd((unsigned char *)(UTASKER_APP_START))); // get the software length according to software file content
            #endif
                    ptrCluster += 2;                                     // jump initial FAT info
                    ucOffset = 2;
                    usCluster = 4;                                       // first cluster
                    iAnswer = 1;
                }
                while (ulFileSize >= (unsigned long)(BYTES_PER_SECTOR * ptr_utDisk->utFAT.ucSectorsPerCluster)) { // add single file cluster information
                    if (ptrCluster >= (unsigned short *)&ptr_utDisk->ptrSectorData[BYTES_PER_SECTOR]) {
                        return UTFAT_SUCCESS;                            // complete section filled with cluster information
                    }
                    ptrCluster = fnFillClusterEntry(ptrCluster, &ucOffset, &uslastEntry, usCluster);
                    usCluster++;
                    ulFileSize -= (BYTES_PER_SECTOR * ptr_utDisk->utFAT.ucSectorsPerCluster);
                }
                // End of the cluster chain has been reached
                //
                if (iAnswer == 0) {
                    return UTFAT_SUCCESS;
                }
                if (ptrCluster >= (unsigned short *)&ptr_utDisk->ptrSectorData[BYTES_PER_SECTOR]) {
                    return UTFAT_SUCCESS;                                // no space for the final entry this time
                }
                switch (ucOffset) {                                      // {13}
                case 0:
                    ptrCluster = fnFillClusterEntry(ptrCluster, &ucOffset, &uslastEntry, (unsigned short)(usCluster | 0x000f));
                    *ptrCluster = LITTLE_SHORT_WORD(0x00ff);
                    break;
                case 1:
                    ptrCluster = fnFillClusterEntry(ptrCluster, &ucOffset, &uslastEntry, (unsigned short)(usCluster | 0x00ff));
                    *ptrCluster = LITTLE_SHORT_WORD(0x000f);
                    break;
                case 2:
                    ptrCluster = fnFillClusterEntry(ptrCluster, &ucOffset, &uslastEntry, 0x0fff);
                    break;
                case 3:
                    *ptrCluster = LITTLE_SHORT_WORD(0x0fff);
                    break;
                }
                iAnswer = 0;                                             // only report end of file cluster once
                uslastEntry = 0;                                         // {14} reset in case of need to repeat
                usCluster = 0;
                ucOffset = 0;
        #else
                if (ulSectorNumber == ptr_utDisk->utFAT.ulFAT_start) {
                    ptrCluster += 2;
                    *ptrCluster = LITTLE_SHORT_WORD(0xfff0);             // end of cluster chain
                }
        #endif
    #else
                unsigned long *ptrCluster = (unsigned long *)ptr_utDisk->ptrSectorData;
        #if defined ENABLE_READBACK
                unsigned long ulFileSize = fnGetFileSize((const LFN_ENTRY_STRUCTURE_FAT32 *)fnGetFlashAdd((unsigned char *)(UTASKER_APP_START))); // get the software length according to software file content
                unsigned long ulCluster = 0x00000004;
                unsigned long ulFAT_sector = (ulSectorNumber - ptr_utDisk->utFAT.ulFAT_start);
                if (ulFAT_sector == 0) {
                    ptrCluster += 3;                                     // jump initial FAT info
                }
                else {
                    unsigned long ulSkip = (ulFAT_sector * (BYTES_PER_SECTOR * 128));
                    ulFileSize += (4 * 128);                             // compensate 4 cluster entries for standard FAT start and single file
                    if (ulFileSize >= ulSkip) {
                        ulFileSize -= ulSkip;
                    }
                    else {
                        return UTFAT_SUCCESS;
                    }
                }
                ulCluster += (ulFAT_sector * 128);                       // initial cluster reference
                while (ulFileSize >= BYTES_PER_SECTOR) {                 // add single file cluster information
                    if (ptrCluster >= (unsigned long *)&ptr_utDisk->ptrSectorData[BYTES_PER_SECTOR]) {
                        return UTFAT_SUCCESS;
                    }
                    *ptrCluster++ = LITTLE_LONG_WORD(ulCluster);
                    ulCluster++;
                    ulFileSize -= BYTES_PER_SECTOR;
                }
                if (ptrCluster >= (unsigned long *)&ptr_utDisk->ptrSectorData[BYTES_PER_SECTOR]) {
                    return UTFAT_SUCCESS;
                }
        #else
                ptrCluster += 3;
        #endif
                *ptrCluster = LITTLE_LONG_WORD(CLUSTER_MASK);            // end of cluster chain
    #endif
            }
        }
    #if defined ENABLE_READBACK
        #if defined UTFAT12
        else if (ulSectorNumber >= ptr_utDisk->ulVirtualBaseAddress) 
        #else
        else if (ulSectorNumber >= ptr_utDisk->ulLogicalBaseAddress)
        #endif
        {                                                                // clusters
        #if defined UTFAT12
            unsigned long ulCluster = ((ulSectorNumber - ptr_utDisk->ulVirtualBaseAddress) - 1);
        #else
            unsigned long ulCluster = ((ulSectorNumber - ptr_utDisk->ulLogicalBaseAddress) - 1);
        #endif
            unsigned char *ptrSW_source = (unsigned char *)(ptr_disk_location[ucDisk] + (ROOT_FILE_ENTRIES * sizeof(DIR_ENTRY_STRUCTURE_FAT32)));
        #if defined MAC_OS_X_WORKAROUND
            ptrSW_source += ((ulCluster - 7) * BYTES_PER_SECTOR);
        #else
            ptrSW_source += (ulCluster * BYTES_PER_SECTOR);
        #endif
            if (ptrSW_source < ptr_disk_end[ucDisk]) {                   // {27} protect to valid area
        #if defined READ_PASSWORD                                        // {2}
                if (iReadEnabled != 0) {                                 // only upload data when the password has been entered
                    MAX_FILE_LENGTH read_length = (MAX_FILE_LENGTH)(UTASKER_APP_END - ptrSW_source); // {27} space remaining in the application area
                    if (read_length > BYTES_PER_SECTOR) {                // limit each read size to a single sector
                        read_length = BYTES_PER_SECTOR;                  // usually complete sectors are written
                    }
                    uMemcpy(ptr_utDisk->ptrSectorData, ptrSW_source, read_length);
                  /*if (read_length < BYTES_PER_SECTOR) {                // if only a fragment could be read we fill the remainder of the sector with zeros
                        uMemset((ptr_utDisk->ptrSectorData + read_length), 0x00, (BYTES_PER_SECTOR - read_length)); // read zero content when password not entered
                    }*/
                }
                else {
                    uMemset(ptr_utDisk->ptrSectorData, 0x00, BYTES_PER_SECTOR); // read zero content when password not entered
                }
        #else
                uMemcpy(ptr_utDisk->ptrSectorData, ptrSW_source, BYTES_PER_SECTOR);
        #endif
            }
        }
    #endif
        break;
    }
    return UTFAT_SUCCESS;
}

static const UTDISK *_fnGetDiskInfo(unsigned char ucDisk)
{
    UTDISK *ptr_utDisk = &utDisks[ucDisk];
    ptr_utDisk->utFAT.usBytesPerSector = BYTES_PER_SECTOR;               // prime some information
    ptr_utDisk->ulSD_sectors = (DISK_SIZE/BYTES_PER_SECTOR);             // the number of sector that the disk has
    ptr_utDisk->usDiskFlags = (DISK_MOUNTED | DISK_FORMATTED);           // {11} the disk is assumed to be mounted and formatted
    ptr_utDisk->ptrSectorData = (unsigned char *)ulSectorMemory;         // define sector memory for working with (it is long word aligned and shared by disks since its content doesn't not need to be preserved)
    _fnReadSector(ucDisk, 0, BOOT_SECTOR_LOCATION);                      // ensure FAT information is known
    return (ptr_utDisk);
}
#endif

// Get the software length details from flash header - the file name is not of interest, just its length
//
static unsigned long fnGetFileSize(const LFN_ENTRY_STRUCTURE_FAT32 *ptrLFN_entry)
{
    DIR_ENTRY_STRUCTURE_FAT32 *ptrDirectoryEntry;
    int iEntries = (ROOT_FILE_ENTRIES + 1);                              // the largest number of entries accepted (when long file name) - ignoring the volume entry
    while (ptrLFN_entry->LFN_Attribute != DIR_ATTR_ARCHIVE) {            // search for first visible file
        if (--iEntries == 0) {
            return 1;                                                    // file not found - set non-zero length to avoid deleting
        }
        ptrLFN_entry++;                                                  // move to next
    }
    ptrDirectoryEntry = (DIR_ENTRY_STRUCTURE_FAT32 *)ptrLFN_entry;
    return ((ptrDirectoryEntry->DIR_FileSize[3] << 24) + (ptrDirectoryEntry->DIR_FileSize[2] << 16) + (ptrDirectoryEntry->DIR_FileSize[1] << 8) + ptrDirectoryEntry->DIR_FileSize[0]); // return the file length
}

#if defined FAT_EMULATION && defined EMULATED_FAT_FILE_DATE_CONTROL
static void fnGetFileDate(const LFN_ENTRY_STRUCTURE_FAT32 *ptrLFN_entry, unsigned short *ptr_usCreationTime, unsigned short *ptr_usCreationDate)
{
    DIR_ENTRY_STRUCTURE_FAT32 *ptrDirectoryEntry;
    int iEntries = (ROOT_FILE_ENTRIES + 1);                              // the largest number of entries accepted (when long file name) - ignoring the volume entry
    while (ptrLFN_entry->LFN_Attribute != DIR_ATTR_ARCHIVE) {            // search for first visible file
        if (--iEntries == 0) {
            return;                                                      // file not found
        }
        ptrLFN_entry++;                                                  // move to next
    }
    ptrDirectoryEntry = (DIR_ENTRY_STRUCTURE_FAT32 *)ptrLFN_entry;
    *ptr_usCreationDate = ((ptrDirectoryEntry->DIR_CrtDate[1] << 8) | ptrDirectoryEntry->DIR_CrtDate[0]);
    *ptr_usCreationTime = ((ptrDirectoryEntry->DIR_CrtTime[1] << 8) | ptrDirectoryEntry->DIR_CrtTime[0]);
}
#endif



static void fnDeleteApplication(unsigned char ucDisk)
{
    #if defined DELETE_BLOCK                                             // devices with long delete times can divide the application erase into smaller blocks with watchdog triggers inbetween
    unsigned long ulApplicationLength = (UTASKER_APP_END - (unsigned char *)UTASKER_APP_START);
    unsigned long ulBlockLength = DELETE_BLOCK;
    unsigned char *ptrFlash = (unsigned char *)UTASKER_APP_START;
    do {
        if (ulApplicationLength < ulBlockLength) {
            ulBlockLength = ulApplicationLength;
        }
        fnEraseFlashSector(ptrFlash, ulBlockLength);                     // delete application space
        ulApplicationLength -= ulBlockLength;
        ptrFlash += ulBlockLength;
        fnRetriggerWatchdog();                                           // retrigger watchdog after each block delete
    } while (ulApplicationLength != 0);
    #else
        #if defined MEMORY_SWAP
    fnEraseFlashSector((unsigned char *)ptr_fileobject_location[ucDisk], 0); // erase the file object
        #endif
    fnEraseFlashSector((unsigned char *)ptr_disk_location[ucDisk], (ptr_disk_end[ucDisk] - (unsigned char *)ptr_disk_location[ucDisk])); // delete application space
    #endif
    iSoftwareState[ucDisk] = SW_EMPTY;                                   // mark that no software is present so that new software can be loaded
}

#if defined MAC_OS_X_WORKAROUND                                          // {25}
// This routine supports only the first fat sector (no partial entry with overflow into next sector) which is adequate for files up to the size that a single sector of clusters can hold
//
static unsigned long fnGetNextCluster(unsigned char ucDisk, unsigned long ulClusterRef)
{
    unsigned long ulThisOffset = ((ulClusterRef * 3)/8);
    unsigned long ulClusterEntryContent = LITTLE_LONG_WORD(ulFatSector[ucDisk][ulThisOffset]);   
    switch (ulClusterRef%8) {
    case 0:
        ulClusterEntryContent &= 0x00000fff;
        break;
    case 1:
        ulClusterEntryContent >>= 12;
        ulClusterEntryContent &= 0x00000fff;
        break;
    case 2:
        ulClusterEntryContent >>= 24;
        ulClusterEntryContent |= ((LITTLE_LONG_WORD(ulFatSector[ucDisk][ulThisOffset + 1]) & 0x0000000f) << 8);
        break;
    case 3:
        ulClusterEntryContent >>= 4;
        ulClusterEntryContent &= 0x00000fff;
        break;
    case 4:
        ulClusterEntryContent >>= 16;
        ulClusterEntryContent &= 0x00000fff;
        break;
    case 5:
        ulClusterEntryContent >>= 28;
        ulClusterEntryContent |= ((ulFatSector[ucDisk][ulThisOffset + 1] & 0x000000ff) << 4);
        break;
    case 6:
        ulClusterEntryContent >>= 8;
        ulClusterEntryContent &= 0x00000fff;
        break;
    case 7:
        ulClusterEntryContent >>= 20;
        break;
    }
    return ulClusterEntryContent;
}

static int fnIsInClusterChain(unsigned char ucDisk, unsigned long ulSectorNumber, unsigned long ulInvisibleClusterChain)
{
    unsigned long ulFirstSectorInCluster;
    do {
    #if defined FAT_EMULATION
        ulFirstSectorInCluster = (((ulInvisibleClusterChain - 2) * 8) + (ptrDiskInfo[ucDisk]->ulVirtualBaseAddress/* + 2*/)); // a cluster consists of 8 sectors
    #else
        ulFirstSectorInCluster = (((ulInvisibleClusterChain - 2) * 8) + (utDisks[ucDisk].ulVirtualBaseAddress/* + 2*/)); // a cluster consists of 8 sectors
    #endif
        if ((ulSectorNumber >= ulFirstSectorInCluster) && (ulSectorNumber < (ulFirstSectorInCluster + 8))) { // if the cluster sector matches the sector number
            return 0;                                                    // match
        }
        ulInvisibleClusterChain = fnGetNextCluster(ucDisk, ulInvisibleClusterChain); // get the next cluster in the present chain
        if (ulInvisibleClusterChain == FAT12_CLUSTER_MASK) {             // end of chain reached
            return 1;
        }
    } while (1);
}

static int fnIsWindows(unsigned char *ptrBuffer)
{
    if (ptrBuffer[0] != '{') {                                           // start of {.1.4.2.2.7.F.1.6.-.7.9.E.6.-.4.4.2.A.-.A.9.6.9.-.B.D.9.8.A.5.F.6.E.C.1.B.} unicode string where . is 0x00
        return 1;
    }
    if (ptrBuffer[0x4a] != '}') {
        return 1;
    }
    if (ptrBuffer[1] != 0x00) {
        return 1;
    }
    if (ptrBuffer[3] != 0x00) {
        return 1;
    }
    if (ptrBuffer[5] != 0x00) {
        return 1;
    }
    return 0;                                                            // very high chance that this is Windows hidden file data that is being written to an as yet unassigned cluster
}

static int fnIsMAC(unsigned char *ptrBuffer)
{
    /*
    if ((ptrBuffer[8] == '-') && (ptrBuffer[13] == '-') && (ptrBuffer[18] == '-') && (ptrBuffer[23] == '-') && (ptrBuffer[36] == 0)) { // serial number
        return 0;                                                        // high change that this is serial number content
    }*/
    if (ptrBuffer[8] != 'M') {                                           // "Mac OS X"
        return 1;
    }
    if (ptrBuffer[9] != 'a') {
        return 1;
    }
    if (ptrBuffer[10] != 'c') {
        return 1;
    }
    if (ptrBuffer[84] != 'A') {                                          // "ATTR"
        return 1;
    }
    if (ptrBuffer[85] != 'T') {
        return 1;
    }
    if (ptrBuffer[86] != 'T') {
        return 1;
    }
    if (ptrBuffer[87] != 'R') {
        return 1;
    }
    return 0;                                                            // very high chance that this is MAC hidden file data that is being written to an as yet unassigned cluster
}

    #if defined AUTO_DELETE_ON_ANY_FIRMWARE
static unsigned long fnGetContentAddress(unsigned char *ptrBuffer)
{
    unsigned long ulAddress = *ptrBuffer++;
        #if defined _LITTLE_ENDIAN
    ulAddress |= (*ptrBuffer++ << 8);
    ulAddress |= (*ptrBuffer++ << 16);
    ulAddress |= (*ptrBuffer << 24);
        #else
    ulAddress <<= 8;
    ulAddress |= (*ptrBuffer++);
    ulAddress <<= 8;
    ulAddress |= (*ptrBuffer++);
    ulAddress <<= 8;
    ulAddress |= (*ptrBuffer);
        #endif
    return ulAddress;
}

// Firmware content can be recognised by the fact that the first long word is the initial stack pointer, which will be in SRAM (normally near the top)
// In addition, the second long word will be the entry address (initial PC value)
// - valid for ARM and Coldfire
//
static int fnIsFirmware(unsigned char *ptrBuffer)
{
    unsigned long ulAddress = fnGetContentAddress(ptrBuffer);
    if ((ulAddress > RAM_START_ADDRESS) && (ulAddress <= (RAM_START_ADDRESS + SIZE_OF_RAM))) {
        ulAddress = fnGetContentAddress(ptrBuffer + 4);
    #if defined MEMORY_SWAP
        if ((ulAddress > FLASH_START_ADDRESS) && (ulAddress <= (FLASH_START_ADDRESS + (SIZE_OF_FLASH/2)))) {
            return 0;                                                    // probably firmware content
        }
    #else
        if ((ulAddress > _UTASKER_APP_START_) && (ulAddress <= (FLASH_START_ADDRESS + SIZE_OF_FLASH))) {
            return 0;                                                    // probably firmware content
        }
    #endif
    }
    return -1;                                                           // not firmware content
}
    #endif

// This function is called to check the content of the first sector written to cluster space in a block write
// - if the write is to a cluster that is known to belong to a hidden file it is rejected
// - if the content correlates to MAC OS X hidden file content it is rejected so that the complete block write is ignored
// - if the content correlates to Windows hidden file content it is rejected so that the complete block write is ignored
//
static int fnCorrolateData(unsigned char ucDisk, unsigned char *ptrBuffer, unsigned long ulSectorNumber)
{
    unsigned long ulInvisibleClusterChain;
    #if defined FAT_EMULATION
    DIR_ENTRY_STRUCTURE_FAT32 *file_object = ptrDiskInfo[ucDisk]->rootBuffer;
    #else
    DIR_ENTRY_STRUCTURE_FAT32 *file_object = (DIR_ENTRY_STRUCTURE_FAT32 *)&root_file[ucDisk];
    #endif
    while (file_object->DIR_Attr != 0) {                                 // search through the root directory to see whether the data is destined for a hidden file
        switch (file_object->DIR_Attr) {
        case DIR_ATTR_LONG_NAME:
        case DIR_ATTR_ARCHIVE:
            break;
        default:
            if ((file_object->DIR_Attr & DIR_ATTR_HIDDEN) && (file_object->DIR_Name[0] != DIR_NAME_FREE)) { // valid hidden file
                ulInvisibleClusterChain = (file_object->DIR_FstClusHI[1] << 24);
                ulInvisibleClusterChain |= (file_object->DIR_FstClusHI[0] << 16);
                ulInvisibleClusterChain |= (file_object->DIR_FstClusLO[1] << 8);
                ulInvisibleClusterChain |= file_object->DIR_FstClusLO[0];
                if (fnIsInClusterChain(ucDisk, ulSectorNumber, ulInvisibleClusterChain) == 0) { // check to see whether the cluster block start is in this hidden files' cluster chanin
                    return HIDDEN_FILE_CLUSTER;                          // this is destined for a hidden file so ignore
                }
            }
            break;
        }
        file_object++;
    }
    if (fnIsWindows(ptrBuffer) == 0) {
        return WINDOWS_HIDDEN_DATA_CONTENT;                              // probably hidden windows data
    }
    if (fnIsMAC(ptrBuffer) == 0) {
        return MAC_HIDDEN_DATA_CONTENT;                                  // probably hidden MAC data
    }
    #if defined AUTO_DELETE_ON_ANY_FIRMWARE
    if (fnIsFirmware(ptrBuffer) == 0) {
        return FIRMWARE_START_CONTENT;                                   // probably the start of firmware copy
    }
    #endif
    return UNKNOWN_CONTENT;                                              // this content is probably firmware data so it can be accepted if required
}
#endif

static int _fnWriteSector(unsigned char ucDisk, unsigned char *ptrBuffer, unsigned long ulSectorNumber)
{
    static int iSynchronise[EMULATED_FAT_LUMS] = {0};                    // {12} single-shot offset synchronisation flag
    static unsigned long ulOffset[EMULATED_FAT_LUMS] = {0};              // {13} file address offset when the host writes code to different area than empty disk start

    #if defined FAT_EMULATION
    UTDISK *ptr_utDisk = (UTDISK *)ptrDiskInfo[ucDisk];
    #else
    UTDISK *ptr_utDisk = &utDisks[ucDisk];
    #endif
    #if defined UTFAT12
    if (ulSectorNumber >= ptr_utDisk->ulVirtualBaseAddress)              // write to cluster space
    #else
        #if defined FAT_EMULATION
    if (ulSectorNumber >= (ptr_utDisk->ulLogicalBaseAddress + ROOT_DIR_SECTORS))
        #else
    if (ulSectorNumber >= ptr_utDisk->ulLogicalBaseAddress)
        #endif
    #endif
    {                                                                    // data being written to file - since the PC believes that all clusters are free it will increment the sectors as the file is saved
        if (iSoftwareState[ucDisk] <= SW_PROGRAMMING) {                  // when no software available, program to FLASH starting at the application start address
    #if defined UTFAT12
            unsigned char *ptrProgAdd = (unsigned char *)(ptr_disk_location[ucDisk] + ((ulSectorNumber - ptr_utDisk->ulVirtualBaseAddress) - ROOT_DIR_SECTORS) * BYTES_PER_SECTOR);
    #else
            unsigned char *ptrProgAdd = (unsigned char *)(ptr_disk_location[ucDisk] + ((ulSectorNumber - ptr_utDisk->ulLogicalBaseAddress) - ROOT_DIR_SECTORS) * BYTES_PER_SECTOR);
    #endif
    #if !defined MEMORY_SWAP
            ptrProgAdd += (ROOT_FILE_ENTRIES * sizeof(DIR_ENTRY_STRUCTURE_FAT32));
    #endif
    #if defined MAC_OS_X_WORKAROUND
            if (ucAcceptUploads[ucDisk] == 0) {                          // data received immediately after connection can only be hidden OS content so ignore
                ulNewWriteBlock[ucDisk] = 0;
                return UTFAT_SUCCESS;                                    // ignore all writes
            }
            else if (ucAcceptUploads[ucDisk] == 1) {                     // we need to filter data content before we accept data
                if (ulNewWriteBlock[ucDisk] != 0) {                      // first sector write in a cluster
                    ulNewWriteBlock[ucDisk] = 0;                         // check is only made on first sector of a cluster block write
        #if defined AUTO_DELETE_ON_ANY_FIRMWARE
                    if (fnCorrolateData(ucDisk, ptrBuffer, ulSectorNumber) != FIRMWARE_START_CONTENT) { // check whether the first sector of this data corresponds to a hidden MAC/Windows file write
                        return UTFAT_SUCCESS;                            // ignore all writes in this block
                    }
        #else
                    if (fnCorrolateData(ucDisk, ptrBuffer, ulSectorNumber) > FIRMWARE_START_CONTENT) { // check whether the first sector of this data corresponds to a hidden MAC/Windows file write
                        return UTFAT_SUCCESS;                            // ignore all writes in this block
                    }
                    if (ulLastIgnoredClusterWrite[ucDisk] != 0) {        // this is the first write after a delete was performed
                        if (ulSectorNumber == ulLastIgnoredClusterWrite[ucDisk]) { // filter a single post-delete write to the same cluster
                          //ulLastIgnoredClusterWrite[ucDisk] = 0;       // only ignore single block write
                            return UTFAT_SUCCESS;                        // ignore all writes in this block
                        }
                    }
        #endif
                    ucAcceptUploads[ucDisk] = 2;                         // from this point on all cluster writes are accepted
        #if defined DEBUG_MAC
                    fnDebugMsg("Prog. accepted\r\n");
        #endif
                }
                else {
                    return UTFAT_SUCCESS;                                // ignore all writes
                }
            }
    #endif
            if (iSynchronise[ucDisk] == 0) {                             // on first write to cluster area
    #if defined MEMORY_SWAP
                ulOffset[ucDisk] = (unsigned long)(ptrProgAdd - (ptr_disk_location[ucDisk])); // check whether the data is starting at an offset or not
    #else
                ulOffset[ucDisk] = (unsigned long)(ptrProgAdd - (ptr_disk_location[ucDisk] + (ROOT_FILE_ENTRIES * sizeof(DIR_ENTRY_STRUCTURE_FAT32)))); // check whether the data is starting at an offset or not
    #endif
                iSynchronise[ucDisk] = 1;                                // mark that the offset is synchronised
    #if defined DEBUG_CODE
                fnDebugMsg("O:");
                fnDebugHex(ulOffset[ucDisk], (WITH_LEADIN | WITH_CR_LF | sizeof(ulOffset[ucDisk])));
    #endif
            }
            ptrProgAdd -= ulOffset[ucDisk];                              // adjust offset to align flash address to start of application if needed
    #if defined MEMORY_SWAP
            if ((ptrProgAdd < ptr_disk_end[ucDisk]) && (ptrProgAdd >= (ptr_disk_location[ucDisk])))
    #else
            if ((ptrProgAdd < ptr_disk_end[ucDisk]) && (ptrProgAdd >= (ptr_disk_location[ucDisk] + (ROOT_FILE_ENTRIES * sizeof(DIR_ENTRY_STRUCTURE_FAT32)))))
    #endif
            {                                                            // limit location and length of file
                MAX_FILE_LENGTH write_length = (MAX_FILE_LENGTH)(UTASKER_APP_END - ptrProgAdd); // {27} space remaining in the application area
                if (write_length > BYTES_PER_SECTOR) {                   // limit each write size to a single sector
                    write_length = BYTES_PER_SECTOR;                     // usually complete sectors are written
                }
    #if defined DEBUG_CODE
                fnDebugMsg("W:");
                fnDebugHex((unsigned long)ptrProgAdd, (WITH_LEADIN | WITH_CR_LF | sizeof(ptrProgAdd)));
    #endif
                fnWriteBytesFlash(ptrProgAdd, ptrBuffer, write_length);  // program the sector (or part of final sector)
            }
    #if defined DEBUG_CODE
            else {
                fnDebugMsg("N");                                         // write outside of code area - ignored
            }
    #endif
    #if defined _WINDOWS
            uTaskerMonoTimer(OWN_TASK, (DELAY_LIMIT)(3.0 * SEC), (TIMEOUT_USB_LOADING_COMPLETE_C + ucDisk)); // a file is being written - it is not always possible to detect when the write has completed so we use an idle timer. After the idle period the file entry will be written and the board reset
    #else
            uTaskerMonoTimer(OWN_TASK, (DELAY_LIMIT)(1.5 * SEC), (TIMEOUT_USB_LOADING_COMPLETE_C + ucDisk)); // a file is being written - it is not always possible to detect when the write has completed so we use an idle timer. After the idle period the file entry will be written and the board reset
    #endif
            iSoftwareState[ucDisk] = SW_PROGRAMMING;
        }
    #if defined READ_PASSWORD                                            // {2}
        else {                                                           // when SW is already present monitor upload of a password file to unlock software uploade
        #if defined MAC_OS_X_WORKAROUND
            if ((ucAcceptUploads[ucDisk] != 0) && (ulNewWriteBlock[ucDisk] != 0)) {
                int iContentType = fnCorrolateData(ucDisk, ptrBuffer, ulSectorNumber);
            #if defined AUTO_DELETE_ON_ANY_FIRMWARE
                if (FIRMWARE_START_CONTENT == iContentType) {            // date is being received that has content recognised as firmware
                    // This can happen when a MAC sends new data content before deleting the original file
                    //
                    fnDeleteApplication(ucDisk);                         // deleted existing firmware (changing the state to be without software)
                #if defined DEBUG_CODE || defined DEBUG_MAC
                    fnDebugMsg("Deleting (3)\r\n");
                #endif
                    _fnWriteSector(ucDisk, ptrBuffer, ulSectorNumber);   // recall the write handler so that the software content can be written
                    return UTFAT_SUCCESS;
                }
            #endif
            #if !defined AUTO_DELETE_ON_ANY_FIRMWARE
                if (iContentType <= FIRMWARE_START_CONTENT) {            // check whether this is to an unallocated cluster and not recognisable as host hidden data
                    ulLastIgnoredClusterWrite[ucDisk] = ulSectorNumber;  // remember this cluster so that it can be ignored after a delete
                }
            #endif
        #endif
                if ((uMemcmp(ucReadPassword, ptrBuffer, sizeof(ucReadPassword))) == 0) {
        #if defined FAT_EMULATION
                    dataFile[ucDisk][0].ucFormatType = FORMAT_TYPE_RAW_BINARY; // on password match enable reading (valid until next reset)
        #else
                    iReadEnabled = 1;                                    // on password match enable reading (valid until next reset)
        #endif
                    fnDebugMsg("USB MSD Read enabled\n\r");
                }
        #if defined MAC_OS_X_WORKAROUND
            }
        #endif
        }
    #endif
    }
    #if defined FAT_EMULATION
    else if ((ulSectorNumber >= ptr_utDisk->ulLogicalBaseAddress) && (ulSectorNumber < ptr_utDisk->ulLogicalBaseAddress + ROOT_DIR_SECTORS)) { // root directory sector
        int iRootSection = (ulSectorNumber - ptr_utDisk->ulLogicalBaseAddress);
        uMemcpy(((unsigned char *)(ptrDiskInfo[ucDisk]->rootBuffer) + (iRootSection * BYTES_PER_SECTOR)), ptrBuffer, (BYTES_PER_SECTOR)); // update the root sector
    #else
    else if ((ulSectorNumber == ptr_utDisk->ulLogicalBaseAddress) || (ulSectorNumber == (ptr_utDisk->ulLogicalBaseAddress + 1))) { // write to the root directory (first two sectors are maintained)
        if (ulSectorNumber == ptr_utDisk->ulLogicalBaseAddress) {
            uMemcpy(&root_file[ucDisk], ptrBuffer, BYTES_PER_SECTOR);    // update the first root sector
        }
        else {
            uMemcpy((((unsigned char *)&root_file[ucDisk]) + BYTES_PER_SECTOR), ptrBuffer, BYTES_PER_SECTOR); // update the second root sector
        }
    #endif
        if (iSoftwareState[ucDisk] == SW_AVAILABLE) {                    // check whether a delete has just been performed since software is already present
    #if defined FAT_EMULATION
            DIR_ENTRY_STRUCTURE_FAT32 *file_object = ptrDiskInfo[ucDisk]->rootBuffer;
    #else
            DIR_ENTRY_STRUCTURE_FAT32 *file_object = (DIR_ENTRY_STRUCTURE_FAT32 *)&root_file[ucDisk];
    #endif
            int i = 0;

            while (i < (BYTES_PER_SECTOR/sizeof(DIR_ENTRY_STRUCTURE_FAT32))) { // search the first root directory sector (the file of interest is alwas at the start)
                if ((file_object->DIR_Name[0] == DIR_NAME_FREE) && (file_object->DIR_Attr == DIR_ATTR_ARCHIVE)) { // {22} deleted file entry found indicates that a deletion has taken place
    #if defined DEBUG_CODE || defined DEBUG_MAC
                    fnDebugMsg("Deleting (1)\r\n");
    #endif
                    fnDeleteApplication(ucDisk);                         // deleted (visible) file found so we delete the original firmware
    #if defined MAC_OS_X_WORKAROUND
                    ulNewWriteBlock[ucDisk] = 0;
    #endif
                    return UTFAT_SUCCESS;                                // the original software has been deleted and now a new one can be accepted
                }
                if (file_object->DIR_Attr == DIR_ATTR_ARCHIVE) {         // {22} we only check the first (visible) file entry
                    break;
                }
                file_object++;
                i++;
            }
    #if defined FAT_EMULATION
            if (fnGetFileSize((const LFN_ENTRY_STRUCTURE_FAT32 *)ptrDiskInfo[ucDisk]->rootBuffer) == 0)
    #else
            if (fnGetFileSize((const LFN_ENTRY_STRUCTURE_FAT32 *)&root_file[ucDisk]) == 0)
    #endif
            {                                                            // some times the file length is set to zero but the file name is not deleted - this works around this by checking the file size as well
    #if defined DEBUG_CODE || defined DEBUG_MAC
                fnDebugMsg("Deleting (2)\r\n");
    #endif
                fnDeleteApplication(ucDisk);
            }
        }
        else {
    #if defined DEBUG_CODE
            fnDebugMsg("R");                                             // root write - ignored
    #endif
        }
    }
    #if defined DEBUG_CODE
    else {
        fnDebugMsg("F");                                                 // FAT write - ignored
    }
    #endif
    #if defined MAC_OS_X_WORKAROUND
    if (ulSectorNumber == ptr_utDisk->utFAT.ulFAT_start) {               // write to first fat sector
        uMemcpy(&ulFatSector[ucDisk], ptrBuffer, BYTES_PER_SECTOR);      // update the first fat sector
    }
    ulNewWriteBlock[ucDisk] = 0;
    #endif
    return UTFAT_SUCCESS;
}

    #if defined DEBUG_MAC
static void fnDebugWrite(int iDisk, unsigned char *ptr_ucBuffer, unsigned long ulLogicalBlockAdr)
{
    int i, j;
    UTDISK *ptr_utDisk = &utDisks[iDisk];
    if (ulNewWriteBlock[iDisk] == 0) {                                   // only first sector or a block
        return;
    }
    fnDebugMsg("// [Time:");
    fnDebugDec((uTaskerSystemTick / SEC), 0);
    fnDebugMsg("s] LBA = ");
    fnDebugDec(ulLogicalBlockAdr, 0);
    #if defined UTFAT12
    if (ulLogicalBlockAdr >= ptr_utDisk->ulVirtualBaseAddress)           // write to cluster space
    #else
    if (ulLogicalBlockAdr >= ptr_utDisk->ulLogicalBaseAddress)
    #endif
    {
        fnDebugMsg(" - cluster");
    }
    else if ((ulLogicalBlockAdr == ptr_utDisk->ulLogicalBaseAddress) || (ulLogicalBlockAdr == (ptr_utDisk->ulLogicalBaseAddress + 1))) {
        fnDebugMsg(" - root");
    }
    else if (ulLogicalBlockAdr == ptr_utDisk->utFAT.ulFAT_start) {
        fnDebugMsg(" - FAT");
    }
    else {
        fnDebugMsg(" - ??");
    }
    switch (iSoftwareState[iDisk]) {
    case SW_PROGRAMMING:
        fnDebugMsg(" - programming");
        break;
    case SW_AVAILABLE:
        fnDebugMsg(" - SW exists");
        break;
    case SW_EMPTY:
        fnDebugMsg(" - no SW");
        break;
    }
    switch (ucAcceptUploads[iDisk]) {
    case 0:
        fnDebugMsg(" - blocking\r\n");
        break;
    case 1:
        fnDebugMsg(" - filtering\r\n");
        break;
    case 2:
        fnDebugMsg(" - accepting\r\n");
        break;
    }
    fnDebugMsg("+50  USB-1 = 55 53 42 43 50 00 00 00 00 0E 00 00 00 00 0A 2A 00 00 00 00 ");
    fnDebugHex(ulLogicalBlockAdr, 1);
    fnDebugMsg(" 00 00 01 00 00 00 00 00 00 00\r\n\r\n");

    for (i = 0; i < 16; i++) {
        fnDebugMsg("+50  USB-1 =");
        for (j = 0; j < 32; j++) {
            fnDebugHex(*ptr_ucBuffer++, (1 | WITH_SPACE));
        }
        fnDebugMsg("\r\n");
    }
    fnDebugMsg("\r\n");
}
    #endif

    #if defined SERIAL_INTERFACE || defined SDCARD_SUPPORT || defined SPI_FLASH_FAT || defined USE_HTTP || (defined USB_INTERFACE && defined HID_LOADER && defined KBOOT_HID_LOADER) // {18}

#define CREATION_HOURS          12                                       // fixed data and time stamp for files loaded via other interfaces (when nothing else is specified)
#define CREATION_MINUTES        0
#define CREATION_SECONDS        0

#define CREATION_DAY_OF_MONTH   5
#define CREATION_MONTH_OF_YEAR  11
#define CREATION_YEAR           (2015 - 1980)

// This routine sets a time and data to a data file object - it uses a fixed time stamp if no date/time information is present
//
static void fnSetObjectDetails(DIR_ENTRY_STRUCTURE_FAT32 *ptrEntry, FILE_OBJECT_INFO *ptrFileObjectInfo)
{
    unsigned long ulFileLength = (unsigned long)(ptrFileObjectInfo->ptrLastAddress - (unsigned char *)_UTASKER_APP_START_); // the length of file that has been saved
    unsigned short usCreationTime = ptrFileObjectInfo->usCreationTime;
    unsigned short usCreationDate = ptrFileObjectInfo->usCreationDate;

    if (usCreationTime == 0) {
        usCreationTime = (CREATION_SECONDS | (CREATION_MINUTES << 5) | (CREATION_HOURS << 11));
    }
    if (usCreationDate == 0) {
        usCreationDate = (CREATION_DAY_OF_MONTH | (CREATION_MONTH_OF_YEAR << 5) | (CREATION_YEAR << 9));
    }

    ptrEntry->DIR_WrtTime[0] = (unsigned char)(usCreationTime);
    ptrEntry->DIR_WrtTime[1] = (unsigned char)(usCreationTime >> 8);
    ptrEntry->DIR_LstAccDate[0] = ptrEntry->DIR_WrtDate[0] = (unsigned char)(usCreationDate);
    ptrEntry->DIR_LstAccDate[1] = ptrEntry->DIR_WrtDate[1] = (unsigned char)(usCreationDate >> 8);
    ptrEntry->DIR_CrtTime[0] = ptrEntry->DIR_WrtTime[0];
    ptrEntry->DIR_CrtTime[1] = ptrEntry->DIR_WrtTime[1];
    ptrEntry->DIR_CrtDate[0] = ptrEntry->DIR_LstAccDate[0];
    ptrEntry->DIR_CrtDate[1] = ptrEntry->DIR_LstAccDate[1];
    ptrEntry->DIR_FileSize[0] = (unsigned char)(ulFileLength);           // enter the file size
    ptrEntry->DIR_FileSize[1] = (unsigned char)(ulFileLength >> 8);
    ptrEntry->DIR_FileSize[2] = (unsigned char)(ulFileLength >> 16);
    ptrEntry->DIR_FileSize[3] = (unsigned char)(ulFileLength >> 24);
}

// After an SREC download has terminated this routine programs a file entry for the new software with a default name and date but with physical size
//
extern int fnAddSREC_file(FILE_OBJECT_INFO *ptrFileObjectInfo)           // {9}
{
    DIR_ENTRY_STRUCTURE_FAT32 *ptrFileEntry;
    FILE_OBJECT_INFO volumeEntry;                                        // {30}
    unsigned char ucScratchPad[ROOT_FILE_ENTRIES * 32];
    uMemset(ucScratchPad, 0, sizeof(ucScratchPad));                      // zero the content
    ptrFileEntry = (DIR_ENTRY_STRUCTURE_FAT32 *)ucScratchPad;
    ptrFileEntry->DIR_Attr = DIR_ATTR_VOLUME_ID;                         // {30} volume
    uMemcpy(ptrFileEntry->DIR_Name, DISK_NAME, 11);                      // volume name
    volumeEntry.ptrLastAddress = (unsigned char *)_UTASKER_APP_START_;   // cause length zero to be set
    volumeEntry.usCreationDate = volumeEntry.usCreationTime = 0;         // use default time/date
    fnSetObjectDetails(ptrFileEntry, &volumeEntry);                      // add volume's creation date/time details
    ptrFileEntry++;
    if (ptrFileObjectInfo->ptrShortFileName != 0) {
        uMemcpy(ptrFileEntry->DIR_Name, ptrFileObjectInfo->ptrShortFileName, 11); // this is expected to be a short file name of exactly 11 characters in 8:3 format
    }
    else {
        uMemcpy(ptrFileEntry->DIR_Name, "SOFTWAREBIN", 11);              // fixed short file name when loaded via SREC
    }
    ptrFileEntry->DIR_NTRes = 0x18;
    ptrFileEntry->DIR_Attr = DIR_ATTR_ARCHIVE;                           // file
    ptrFileEntry->DIR_FstClusLO[0] = 3;                                  // first cluster used for file content
    fnSetObjectDetails(ptrFileEntry, ptrFileObjectInfo);
        #if defined MEMORY_SWAP
    if (fnWriteBytesFlash((unsigned char *)(UTASKER_APP_START + (SIZE_OF_FLASH/2) - (2 * FLASH_GRANULARITY)), ucScratchPad, sizeof(ucScratchPad))) {
        return -1;
    }
        #else
    if (fnWriteBytesFlash((unsigned char *)UTASKER_APP_START, ucScratchPad, sizeof(ucScratchPad))) {
        return -1;
    }
        #endif
        #if defined FLASH_ROW_SIZE && FLASH_ROW_SIZE > 0
    fnWriteBytesFlash(0, 0, 0);                                          // close any outstanding FLASH buffer
        #endif
    return 0;
}
    #endif

#if defined FAT_EMULATION

// The application must supply this function when using FAT_EMULATION
// - the FAT emulator calls it to obtain referenced file information (formatted length, location in memory, creation date/time and name)
//
extern const unsigned char *fnGetDataFile(int iDisk, int iDataRef, EMULATED_FILE_DETAILS *ptrFileDetails)
{
    if ((iDataRef < APPLICATION_DATA_FILES) && (dataFile[iDisk][iDataRef].ucFormatType != FORMAT_TYPE_INVALID_FILE)) {
        ptrFileDetails->ucValid = 1;                                     // file is valid
        ptrFileDetails->ulFileLength = dataFile[iDisk][iDataRef].ulDataFileLength; // raw content length of file
    #if defined EMULATED_FAT_FILE_DATE_CONTROL
        ptrFileDetails->usCreationTime = dataFile[iDisk][iDataRef].usCreationTime;
        ptrFileDetails->usCreationDate = dataFile[iDisk][iDataRef].usCreationDate; // (leave at 0 for fixed date/time stamp)
    #endif
    #if defined EMULATED_FAT_FILE_NAME_CONTROL
        ptrFileDetails->ptrFileName = dataFile[iDisk][iDataRef].ptrFileName;
        if (dataFile[iDisk][iDataRef].ucNameFormat != 0) {
            ptrFileDetails->ucValid = 3;                                 // file is valid and its name is already in fat format
        }
    #endif
        return (dataFile[iDisk][iDataRef].ptrFileLocation);              // memory mapped address of file's raw data
    }
    else {
        ptrFileDetails->ucValid = 0;                                     // file is not valid
        return 0;
    }
}

// The application must supply this function when using FAT_EMULATION
// - the FAT emulator calls it to obtain referenced file content, which can be returned as raw data or formatted
// - this example formats raw binary data to CSV format so that it can be easily opened in various PC programs for viewing and processing
//
extern int uDatacopy(int iDisk, int iDataRef, unsigned char *ptrSectorData, const unsigned char *ptrSourceData, int iLength)
{
    int iAdded = 0;

    if ((iDataRef >= APPLICATION_DATA_FILES) || (dataFile[iDisk][iDataRef].ucFormatType == FORMAT_TYPE_INVALID_FILE)) {                                                    // data belongs to first disk only
        return 0;                                                        // no data added
    }
    iAdded = (dataFile[iDisk][iDataRef].ulDataFileLength - (dataFile[iDisk][iDataRef].ptrFileLocation - ptrSourceData)); // remaining raw content length
    if (iAdded > iLength) {
        iAdded = iLength;
    }
    switch (dataFile[iDisk][iDataRef].ucFormatType) {
    #if defined READ_PASSWORD
    case FORMAT_TYPE_RAW_BINARY_PROTECTED:
        uMemset(ptrSectorData, 0, iAdded);                               // return zeroes since the content is to be protected
        break;
    #endif
    case FORMAT_TYPE_RAW_BINARY:
        fnGetParsFile((unsigned char *)ptrSourceData, ptrSectorData, iAdded); // prepare the raw data
        break;
    case FORMAT_TYPE_RAW_STRING:
        uMemcpy(ptrSectorData, ptrSourceData, iAdded);                   // strings are in code so copy directly from memory
        break;

    default:
        break;
    }
    return iAdded;                                                       // the length added to the buffer
}

// Prepare data files in linear flash (prime them if not already present)
//
static void fnPrepareEmulatedFAT(int iDisk)
{
    if (iSoftwareState[iDisk] == SW_AVAILABLE) {                         // only display a software file when firmware is available
        const LFN_ENTRY_STRUCTURE_FAT32 *ptrDirObject = (const LFN_ENTRY_STRUCTURE_FAT32 *)fnGetFlashAdd((unsigned char *)(ptr_fileobject_location[iDisk]));
    #if defined READ_PASSWORD
        dataFile[iDisk][0].ucFormatType = FORMAT_TYPE_RAW_BINARY_PROTECTED; // reads return zero content unless a password has been copied
    #else
        dataFile[iDisk][0].ucFormatType = FORMAT_TYPE_RAW_BINARY;        // softare can be read back from the device
    #endif
        dataFile[iDisk][0].ptrFileLocation = (const unsigned char *)ptr_disk_location[iDisk]; // memory mapped address of file's raw data
    #if !defined MEMORY_SWAP
        dataFile[iDisk][0].ptrFileLocation += (ROOT_FILE_ENTRIES * 32);
    #endif
        if (ptrDirObject->LFN_Attribute == 0xff) {                       // if the file object is erased/not available
            dataFile[iDisk][0].ulDataFileLength = (UTASKER_APP_END - (unsigned char * )UTASKER_APP_START); // maximum application size
            dataFile[iDisk][0].usCreationTime = dataFile[iDisk][0].usCreationDate = 0;
            dataFile[iDisk][0].ptrFileName = "firmware.bin";             // give the firmware a name
            return;
        }
        else {
            dataFile[iDisk][0].ulDataFileLength = fnGetFileSize(ptrDirObject); // the length of the file according to its file object
    #if defined EMULATED_FAT_FILE_DATE_CONTROL
            fnGetFileDate(ptrDirObject, &dataFile[iDisk][0].usCreationTime, &dataFile[iDisk][0].usCreationDate);
    #endif
        }
    #if defined EMULATED_FAT_FILE_NAME_CONTROL
        if (dataFile[iDisk][0].ulDataFileLength > (UTASKER_APP_END - (unsigned char *)_UTASKER_APP_START_)) { // if the length is too large
            dataFile[iDisk][0].ptrFileLocation = (const unsigned char *)ptr_disk_location[iDisk]; // allow the complete content to be read to see what is there
            dataFile[iDisk][0].ulDataFileLength = (UTASKER_APP_END - (unsigned char *)UTASKER_APP_START); // complete application area, including file object
            dataFile[iDisk][0].ucFormatType = FORMAT_TYPE_RAW_BINARY;    // allow the content to be viewed
            dataFile[iDisk][0].ptrFileName = "trash.bin";                       // if the file object is not valid we assume that it is old random data so show it as trash so that it can be deleted
        }
        else {
            dataFile[iDisk][0].ptrFileName = (const CHAR *)(ptrDirObject + 1);  // raw file content
            dataFile[iDisk][0].ucNameFormat = 1;                                // mark that the string is already fat-formatted
        }
    #endif
    }
}
#endif
#endif
#endif
