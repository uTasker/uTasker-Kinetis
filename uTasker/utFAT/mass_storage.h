/**********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher

    ---------------------------------------------------------------------
    File:      mass_storage.h
    Project:   uTasker project
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2016
    *********************************************************************
    11.07.2014 utFAT2.0
    22.01.2015 Add option to return a file's creation time and date in its file object {1}
    30.10.2015 Add emulated FAT support (FAT_EMULATION)                  {2}
    17.01.2016 Add utFileAttribute()                                     {3}

*/

#if defined USB_MSD_HOST && defined SDCARD_SUPPORT
    #define DISK_COUNT                     2                             // this implementation supports two disks (SD-card and USB-MSD host) with up to one partition - disks D and E
    #define DISK_D                         0
    #define DISK_E                         1
    #define DISK_SDCARD                    DISK_D
    #define DISK_MEM_STICK                 DISK_E
#elif defined SDCARD_SUPPORT && defined SPI_FLASH_FAT && defined FLASH_FAT
    #define DISK_COUNT                     3                             // this implementation supports three disks (SD-card, internal flash and SPI flash) with up to one partition - disks C, D and E
    #define DISK_C                         0
    #define DISK_D                         1
    #define DISK_E                         2
    #define DISK_INTERNAL_FLASH            DISK_C
    #define DISK_SDCARD                    DISK_D
    #define DISK_SPI_FLASH                 DISK_E
#elif defined SDCARD_SUPPORT && defined SPI_FLASH_FAT
    #define DISK_COUNT                     2                             // this implementation supports two disks (SD-card and SPI flash) with up to one partition - disks D and E
    #define DISK_D                         0
    #define DISK_E                         1
    #define DISK_SDCARD                    DISK_D
    #define DISK_SPI_FLASH                 DISK_E
#elif defined SDCARD_SUPPORT && defined FLASH_FAT
    #define DISK_COUNT                     2                             // this implementation supports two disks (SD-card and internal flash) with up to one partition - disks C and D
    #define DISK_C                         0
    #define DISK_D                         1
    #define DISK_INTERNAL_FLASH            DISK_C
    #define DISK_SDCARD                    DISK_D
#elif defined SPI_FLASH_FAT && defined FLASH_FAT
    #define DISK_COUNT                     2                             // this implementation supports two disks (internal and SPI flash) with up to one partition - disks C and D
    #define DISK_C                         0
    #define DISK_D                         1
    #define DISK_INTERNAL_FLASH            DISK_C
    #define DISK_SPI_FLASH                 DISK_D
#elif defined SDCARD_SUPPORT && defined FAT_EMULATION                    // {2} SD card and FAT emulation
    #define DISK_COUNT                     2                             // this implementation supports two disk (SD card and emulated FAT) with up to one partition - disks D and E
    #define DISK_D                         0
    #define DISK_E                         1
    #define DISK_SDCARD                    DISK_D
    #define DISK_EM_FAT                    DISK_E
#elif defined SDCARD_SUPPORT
    #define DISK_COUNT                     1                             // this implementation supports one disk (SD-card) with up to one partition - disk D
    #define DISK_D                         0
    #define DISK_SDCARD                    DISK_D
#elif defined USB_MSD_HOST
    #define DISK_COUNT                     1                             // this implementation supports one disk (memory stick) with up to one partition - disk E
    #define DISK_E                         0
    #define DISK_MEM_STICK                 DISK_E
#elif defined SPI_FLASH_FAT                                              // SPI flash alone
    #define DISK_D                         0
    #define DISK_COUNT                     1
    #define DISK_SPI_FLASH                 DISK_D
#elif defined FLASH_FAT                                                  // internal flash alone
    #define DISK_C                         0
    #define DISK_COUNT                     1
    #define DISK_INTERNAL_FLASH            DISK_C
#elif defined FAT_EMULATION                                              // {2} FAT emulation alone
    #define DISK_D                         0
    #define DISK_COUNT                     1
    #define DISK_EM_FAT                    DISK_D
#else                                                                    // default
    #define DISK_D                         0
    #define DISK_COUNT                     1
    #define DISK_SDCARD                    DISK_D
#endif


#define WRITEBACK_BUFFER_FLAG              0x0001                        // flag that the present sector buffer has been changed and so needs to be physically written back to the card
#define WRITEBACK_INFO_FLAG                0x0002                        // flag that there has been a change made to the card which need its info block updated too
#define FSINFO_VALID                       0x0004                        // the disk has a valid info block which can be used to accelerate some calculations
#define DISK_UNFORMATTED                   0x0008                        // disk detected but its content is not formatted
#define WRITE_PROTECTED_SD_CARD            0x0010                        // the SD card has write protection active and so no write operations are allowed
#define DISK_FORMATTED                     0x0020                        // the disk has been detected and is formatted
#define HIGH_CAPACITY_SD_CARD              0x0040                        // the disk is of high capacity type
#define DISK_MOUNTED                       0x0080                        // the disk has been mounted and so is ready for use
#define DISK_NOT_PRESENT                   0x0100                        // a check of the disk failed to identify its presence
#define DISK_TYPE_NOT_SUPPORTED            0x0200                        // unsupported disk type detected
#define DISK_FAT_EMULATION                 0x0400                        // {2} this disk uses FAT emulation
#define DISK_FAT_EMULATION_ROOT_CONTROL    0x0800                        // {2} the root directory content is controlled by the application and not by the emulator
#define DISK_FORMAT_FULL                   0x1000                        // set all content to 0x00 rather than just all FAT
#if defined UTFAT12
    #define DISK_FORMAT_FAT12              0x2000                        // FAT12 format rather than FAT32
#else
    #define DISK_FORMAT_FAT12              0x0000                        // not used
#endif
#if defined UTFAT16
    #define DISK_FORMAT_FAT16              0x4000                        // FAT16 format rather than FAT32
#else
    #define DISK_FORMAT_FAT16              0x0000                        // not used
#endif
#define DISK_TEST_MODE                     0x8000                        // special flag to control testing (development tests)

#define UTFAT_SUCCESS_PATH_MODIFIED        5
#define UTFAT_PATH_IS_ROOT                 4
#define UTFAT_PATH_IS_ROOT_REF             3
#define UTFAT_PATH_IS_FILE                 2                             // the referenced object is a file and not a directory
#define UTFAT_END_OF_DIRECTORY             1
#define UTFAT_SUCCESS                      0
#define UTFAT_DIR_NOT_EMPTY                -1
#define UTFAT_DISK_NOT_READY               -2                            // disk not ready for use - not formatted or not mounted
#define UTFAT_PATH_NOT_FOUND               -3                            // the referenced directory path could not be found
#define UTFAT_DISK_READ_ERROR              -4                            // error occurred while trying to read a sector from the disk
#define UTFAT_DISK_WRITE_ERROR             -5
#define UTFAT_FILE_NOT_FOUND               -6                            // the referenced file was could not be found
#define UTFAT_DIRECTORY_AREA_EXHAUSTED     -7                            // the end of the FAT space was reached and no valid clusters found
#define UTFAT_FAT12_ROOT_FOLDER_EXHAUSTED  -8                            // the FAT12 root folder is limited to 14 clusters in length - once this is used up (224 entries) no more can be loated there
#define UTFAT_FAT16_ROOT_FOLDER_EXHAUSTED  -9                            // the FAT16 root folder can hold up to 512 items and not more - once this has been use up no more can be located in the root folder (note that LFNs can count as multiple items and restrict further then number of real items)
#define UTFAT_FILE_NOT_READABLE            -9                            // the file is not opened in read mode
#define UTFAT_FILE_NOT_WRITEABLE           -10                           // the file cannot be written because it is either not opened in write mode, is marked as a read-only file on the disk or writes are being blocked by another user
#define UTFAT_FILE_LOCKED                  -11                           // the file could not be opened since it is locked for exclusive use by another user
#define UTFAT_SEARCH_INVALID               -12                           // a file search was invalid since the file object is not associated with a directory object
#define UTFAT_DISK_ALREADY_FORMATTED       -13
#define UTFAT_NO_MORE_LISING_SPACE         -14
#define UTFAT_NO_MORE_LISTING_ITEMS_FOUND  -15
#define UTFAT_FINAL_LISTING_ITEM_FOUND     -16
#define UTFAT_DIRECTORY_OBJECT_MISSING     -17
#define UTFAT_DIRECTORY_EXISTS_ALREADY     -18
#define UTFAT_DISK_WRITE_PROTECTED         -19
#define UTFAT_INVALID_NAME                 -20                           // file or directory name is invalid
#define MANAGED_FILE_NO_ACCESS             -21
#define MANAGED_FILE_NO_FILE_HANDLE        -22                           // no space is available for a managed file
#define MISSING_USER_TASK_REFERENCE        -23
#define LFN_RENAME_NOT_POSSIBLE            -24


#if defined SDCARD_SUPPORT || defined USB_MSD_HOST || defined SPI_FLASH_FAT || defined FLASH_FAT || defined FAT_EMULATION

#define UTDIR_ALLOCATED                    0x0001                        // directory object is allocated to an application
#define UTDIR_VALID                        0x0002                        // the directory object is valid and can be used
#define UTDIR_REFERENCED                   0x0004                        // the access is referenced to the present directory path location
#define UTDIR_SET_START                    0x0008                        // if the file is not found the directory is set to the lower directory in the path so that new files can be added there
#define UTDIR_DIR_AS_FILE                  0x0010                        // handle directories and files equivalently - used when renaming and deleting
#define UTDIR_TEST_REL_PATH                0x0020                        // test a path relative to the present directory path location but don't move to it
#define UTDIR_TEST_FULL_PATH               0x0040                        // temporary reference from the root directory
#define UTDIR_TEST_FULL_PATH_TEMP          0x0080                        // temporary reference from the present directory path reference
#define UTDIR_ALLOW_MODIFY_PATH            0x0100                        // allow the directory search to modify the directory path string if it exists

#define BAD_CLUSTER_VALUE                  0x0ffffff7
#define CLUSTER_MASK                       0x0fffffff
#define MEDIA_VALUE_FIXED                  0x0ffffff8
#define MEDIA_VALUE_REMOVABLE              0x0ffffff0

#define FAT16_CLUSTER_MASK                 0x0000ffff
#define FAT12_CLUSTER_MASK                 0x00000fff

#define FIXED_MEDIA                        0xf8
#define REMOVABLE_MEDIA                    0xf0


typedef struct stUTFAT
{
    unsigned long  ulFatSize;                                            // sectors occupied by ONE FAT
    unsigned long  ulFAT_start;                                          // the sector in which the first FAT starts (further FAT copies start at this sector + FAt size)
    unsigned long  ulClusterCount;                                       // total number of clusters in the cluster area
    unsigned short usBytesPerSector;                                     // the number of bytes in a single sector (512, 1024, 2048 or 4096)
    unsigned char  ucNumberOfFATs;                                       // the number of FATs (more that 1 are copies)
    unsigned char  ucSectorsPerCluster;                                  // the number of sectors in a singel cluster - cluster size is sector size x sectors per cluster
} UTFAT;

typedef struct stFILEINFO
{
    unsigned long ulInfoSector;                                          // sector where the information is located
    unsigned long ulFreeClusterCount;
    unsigned long ulNextFreeCluster;
    unsigned char ucCardSpecificData[16];                                // CSD register content (128 bits)
} FILEINFO;

typedef struct _PACK stDIR_ENTRY_STRUCTURE_FAT32
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
    #if defined FAT_EMULATION && defined ROOT_DIR_SECTORS
    DIR_ENTRY_STRUCTURE_FAT32 *rootBuffer;                               // pointer to a root buffer of ROOT_DIR_SECTORS sector size
    #endif
} UTDISK;

__PACK_ON                                                                // compilers using pragmas to control packing will start struct packing from here
typedef struct _PACK stBOOT_SECTOR_BPB                                   // boot sector and bios parameter block
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


// FAT32 BPB_ExtFlags flags
//
#define BPB_ExtFlags_0_ZERO_BASED_NUMBER 0x0f                            // only valid if mirroring is disabled
#define BPB_ExtFlags_0_MIRRORED_FAT      0x00
#define BPB_ExtFlags_0_ONE_FAT           0x80                            // mirroring disabled


typedef struct _PACK stINFO_SECTOR_FAT32
{
    unsigned char FSI_LeadSig[4];                                        // value 0x41615252. Used to validate that this is in fact an FSInfo sector
    unsigned char FSI_Reserved1[480];                                    // content should be zero
    unsigned char FSI_StrucSig[4];                                       // value 0x61417272. More localized signature
    unsigned char FSI_Free_Count[4];                                     // last known free cluster count on the volume - 0xffffffff means unknown
    unsigned char FSI_Nxt_Free[4];                                       // indicates the cluster number at which the driver should start looking for free clusters - 0xffffffff means unknown
    unsigned char FSI_Reserved2[12];                                     // content should be zero
    unsigned char FSI_TrailSig[4];                                       // value 0xaa550000
} INFO_SECTOR_FAT32;


typedef struct _PACK stPARTITION_TABLE_ENTRY
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




typedef struct _PACK stLFN_ENTRY_STRUCTURE_FAT32
{
    unsigned char LFN_EntryNumber;                                       // entry number starting from last - 0x40 is always set in the first entry and the value decrements until 1
    unsigned char LFN_Name_0;                                            // first letter
    unsigned char LFN_Name_0_extension;                                  // first letter extension - is always 0 in English character set
    unsigned char LFN_Name_1;                                            // second letter
    unsigned char LFN_Name_1_extension;                                  // second letter extension - is always 0 in English character set
    unsigned char LFN_Name_2;                                            // third letter
    unsigned char LFN_Name_2_extension;                                  // third letter extension - is always 0 in English character set
    unsigned char LFN_Name_3;                                            // fourth letter
    unsigned char LFN_Name_3_extension;                                  // fourth letter extension - is always 0 in English character set
    unsigned char LFN_Name_4;                                            // fifth letter
    unsigned char LFN_Name_4_extension;                                  // fifth letter extension - is always 0 in English character set
    unsigned char LFN_Attribute;                                         // always 0x0f
    unsigned char LFN_Zero0;                                             // always zero
    unsigned char LFN_Checksum;                                          // check sum
    unsigned char LFN_Name_5;                                            // sixth letter
    unsigned char LFN_Name_5_extension;                                  // sixth letter extension - is always 0 in English character set
    unsigned char LFN_Name_6;                                            // seventh letter
    unsigned char LFN_Name_6_extension;                                  // seventh letter extension - is always 0 in English character set
    unsigned char LFN_Name_7;                                            // eighth letter
    unsigned char LFN_Name_7_extension;                                  // eighth letter extension - is always 0 in English character set
    unsigned char LFN_Name_8;                                            // ninth letter
    unsigned char LFN_Name_8_extension;                                  // ninth letter extension - is always 0 in English character set
    unsigned char LFN_Name_9;                                            // tenth letter
    unsigned char LFN_Name_9_extension;                                  // tenth letter extension - is always 0 in English character set
    unsigned char LFN_Name_10;                                           // eleventh letter
    unsigned char LFN_Name_10_extension;                                 // eleventh letter extension - is always 0 in English character set
    unsigned char LFN_Zero1;                                             // always zero
    unsigned char LFN_Zero2;                                             // always zero
    unsigned char LFN_Name_11;                                           // twelfth letter
    unsigned char LFN_Name_11_extension;                                 // twelfth letter extension - is always 0 in English character set
    unsigned char LFN_Name_12;                                           // thirteenth letter
    unsigned char LFN_Name_12_extension;                                 // thirteenth letter extension - is always 0 in English character set
} LFN_ENTRY_STRUCTURE_FAT32;
__PACK_OFF

#define DIR_ATTR_READ_ONLY               0x01
#define DIR_ATTR_HIDDEN                  0x02
#define DIR_ATTR_SYSTEM                  0x04
#define DIR_ATTR_VOLUME_ID               0x08
#define DIR_ATTR_DIRECTORY               0x10
#define DIR_ATTR_ARCHIVE                 0x20
#define DIR_ATTR_LONG_NAME               (DIR_ATTR_READ_ONLY | DIR_ATTR_HIDDEN | DIR_ATTR_SYSTEM | DIR_ATTR_VOLUME_ID)
#define DIR_ATTR_MASK                    (DIR_ATTR_LONG_NAME | DIR_ATTR_DIRECTORY | DIR_ATTR_ARCHIVE)

#if !defined UTFAT_LFN_READ
    #undef MAX_UTFAT_FILE_NAME
    #define MAX_UTFAT_FILE_NAME (8 + 1 + 3)                              // 8.3 format
#endif

#define DOS_STYLE_LIST_ENTRY_LENGTH (44)
#define FTP_STYLE_LIST_ENTRY_LENGTH (47)


typedef struct stUTFILEINFO {
    unsigned long  ulFileSize;
    unsigned short usFileData;
    unsigned short usFileTime;
    unsigned char  ucFileAttributes;
    CHAR           cFileName[MAX_UTFAT_FILE_NAME + 1];
} UTFILEINFO;


typedef struct stFILE_LOCATION
{
    unsigned long ulCluster;
    unsigned long ulSector;
} FILE_LOCATION;

typedef struct stDISK_LOCATION
{
    FILE_LOCATION directory_location;
    unsigned char ucDirectoryEntry;
} DISK_LOCATION;


// Main directory object
//
typedef struct stUTDIRECTORY
{
    CHAR *                     ptrDirectoryPath;                         // pointer to the location where the directory path string is stored
    DIR_ENTRY_STRUCTURE_FAT32 *ptrEntryStructure;                        // pointer to a loaded entry
    DISK_LOCATION              root_disk_location;                       // reference to the root directory
    DISK_LOCATION              private_disk_location;                    // details of where the directory is physically located on the disk
    DISK_LOCATION              public_disk_location;                     // working information when searching for and manipulating directory contents
    FILE_LOCATION              public_file_location;                     // reference to a file location
    unsigned short             usDirectoryPathLength;                    // the length of string reserved for use by the directory path
    unsigned short             usRelativePathLocation;                   // the present relative path location
    unsigned short             usDirectoryFlags;                         // flags reflecting the directory's state and for defining specific operation
    unsigned char              ucDrive;                                  // the physical drive associated with the directory
} UTDIRECTORY;


// Directory list object, which works together with a main directory object
//
typedef struct stUTLISTDIRECTORY
{
    UTDIRECTORY   *ptr_utDirObject;                                      // pointer to the main directory object
    DISK_LOCATION private_disk_location;                                 // disk location used when searching
    #if defined UTFAT_UNDELETE || defined UTFAT_EXPERT_FUNCTIONS
    DISK_LOCATION undelete_disk_location;                                // disk location set each time deleted object is found
    #endif
    unsigned long ulDirectorySector;                                     // the sector in which the directory entries are contained
    unsigned long ulDirectorySectorIncrement;                            // incremental sector value
    DIR_ENTRY_STRUCTURE_FAT32 *ptrEntryStructure;                        // pointer to a loaded entry
    unsigned char ucDirectoryEntry;                                      // directory entry in present sector (0..31) for 512 sized sectors, up to 255 for maximum 4k sectors
    unsigned char ucDirectoryEntryIncrement;                             // incremental directory entry in present sector
    #if defined UTFAT_UNDELETE || defined UTFAT_EXPERT_FUNCTIONS
    unsigned char ucListFlags;                                           // flag to control whether normal or deleted entries are to be listed or whether output is generated
    #endif
} UTLISTDIRECTORY;

typedef struct stFILE_DATA_CACHE
{
    unsigned long ulFileCacheSector;                                     // the sector being cached
    unsigned char ucFileDataCache[512];                                  // cached data
    unsigned char ucFileCacheState;                                      // state of the cache and cached data
} FILE_DATA_CACHE;

// File access object, which works together with a main directory object
//
typedef struct stUTFILE 
{
    UTDIRECTORY    *ptr_utDirObject;                                     // pointer to the main directory object
#if defined UTFAT_FILE_CACHE_POOL && UTFAT_FILE_CACHE_POOL > 0
    FILE_DATA_CACHE *ptrFileDataCache;                                   // pointer to optional file data cache
#endif
    unsigned long  ulFileMode;                                           // mode that the file was opened for
    unsigned long  ulFileSize;                                           // file's total length
    unsigned long  ulFilePosition;                                       // present linear file position
    int            iFileHandle;                                          // file handle for use in managed mode
    DISK_LOCATION  private_disk_location;                                // location of the file information
#if defined UTFAT_LFN_READ && ((defined UTFAT_LFN_DELETE || defined UTFAT_LFN_WRITE) || defined UTFAT_EXPERT_FUNCTIONS)
    DISK_LOCATION  lfn_file_location;                                    // reference to the long file name entry start (zeroed when the file uses short file name)
#endif
    FILE_LOCATION  private_file_location;                                // location of the start of file data
    FILE_LOCATION  public_file_location;                                 // present file data location
    unsigned short usLastReadWriteLength;                                // read or write length last performed
#if defined UTFAT_RETURN_FILE_CREATION_TIME                              // {1}
    unsigned short usCreationDate;
    unsigned short usCreationTime;
#endif
    unsigned char  ucDrive;                                              // disk drive that the file is on
    UTASK_TASK     ownerTask;                                            // owner task for use by managed mode
#if defined UTFAT_LFN_READ && ((defined UTFAT_LFN_DELETE || defined UTFAT_LFN_WRITE) || defined UTFAT_EXPERT_FUNCTIONS)
    unsigned char  ucLFN_entries;                                        // the number of long file name entries in the long file name
#endif
} UTFILE;


typedef struct stFILE_LISTING
{
    CHAR *ptrBuffer;                                                     // pointer to character buffer
    unsigned long  ulFileSizes;                                          // sum of the total file sizes in this listing
    unsigned short usBufferLength;                                       // length available in character buffer
    unsigned short usStringLength;                                       // length added to character buffer
    unsigned short usMaxItems;                                           // maximum items to be treated in this pass
    unsigned short usItemsReturned;                                      // the number of items treated in this pass
    unsigned short usDirectoryCount;                                     // the number of directories treated in this pass
    unsigned short usFileCount;                                          // the number of files treated in this pass
    unsigned char  ucStyle;                                              // the formatting style to be used
    unsigned char  ucFileAttributes;                                     // file attributes of last entry lister
    unsigned char  ucNameLength;                                         // file name length in present listing
} FILE_LISTING;


#define DOS_TYPE_LISTING       0x00                                      // file listing string in DOS style
#define FTP_TYPE_LISTING       0x01                                      // file listing string in FTP style
#define DELETED_TYPE_LISTING   0x02                                      // file listing of deleted entries only
#define INVISIBLE_TYPE_LISTING 0x04                                      // don't generate output
#define NO_CR_LF_LISTING       0x08                                      // don't end output with CR/LF
#define NULL_TERMINATE_LISTING 0x10                                      // null-terminate the listing
#define HIDDEN_TYPE_LISTING    0x20                                      // file listing of invisible entries


// Parameters used when formatting a disk (values recommended by Microsoft and suiting the Microsoft dimensioning algorithm)
//
#if !defined USER_DEFINED_FAT_PARAMETERS
    #define BOOT_SECTOR_LOCATION   63
    #define BACKUP_ROOT_SECTOR     6
    #define BYTES_PER_SECTOR       512
    #define RESERVED_SECTION_COUNT 32
    #define NUMBER_OF_FATS         2
#endif



extern UTDIRECTORY *utAllocateDirectory(unsigned char ucDisk, unsigned short usPathLength);
extern UTDIRECTORY *utFreeDirectory(UTDIRECTORY *ptrDirectory);
extern const UTDISK *fnGetDiskInfo(unsigned char ucDisk);
extern int  utOpenDirectory(const CHAR *ptrDirPath, UTDIRECTORY *ptrDirObject);
extern int  utLocateDirectory(const CHAR *ptrDirPath, UTLISTDIRECTORY *ptrListDirectory);
extern int  utMakeDirectory(const CHAR *ptrDirPath, UTDIRECTORY *ptrDirObject);
extern int  utChangeDirectory(const CHAR *ptrDirPath, UTDIRECTORY *ptrDirObject);
extern int  utDeleteFile(const CHAR *ptrFilePath, UTDIRECTORY *ptrDirObject);
extern int  utSafeDeleteFile(const CHAR *ptrFilePath, UTDIRECTORY *ptrDirObject);
extern int  utListDir(UTLISTDIRECTORY *ptr_utDirectory, FILE_LISTING *ptrFileLists);
extern int  utReadFile(UTFILE *ptr_utFile, void *ptrBuffer, unsigned short usReadLength);
extern int  utOpenFile(const CHAR *ptrFilePath, UTFILE *ptr_utFile, UTDIRECTORY *ptr_utDirectory, unsigned long ulAccessMode);
extern int  utRenameFile(const CHAR *ptrFilePath, UTFILE *ptr_utFile);
extern int  utCloseFile(UTFILE *ptr_utFile);
extern int  utWriteFile(UTFILE *ptr_utFile, unsigned char *ptrBuffer, unsigned short usLength);
extern int  utFormat(const unsigned char ucDrive, const CHAR *cVolumeLabel, unsigned char ucFlags); // new flags
    #define UTFAT_FORMAT      0x00                                       // format only non-formatted disk (default)
    #define UTFAT_REFORMAT    0x01                                       // reformat already formatted disk
    #define UTFAT_FORMAT_12   0x02                                       // format as FAT16 rather than FAT32
    #define UTFAT_FORMAT_16   0x04                                       // format as FAT16 rather than FAT32
    #define UTFAT_FORMAT_32   0x00                                       // format as FAT32 (default)
    #define UTFAT_FULL_FORMAT 0x08                                       // perform full format - including deleting existing cluster content
extern int  utSeek(UTFILE *ptr_utFile, unsigned long ulPosition, int iSeekType);
extern int  utTruncateFile(UTFILE *ptr_utFile);
extern int  utUndeleteFile(UTLISTDIRECTORY *ptrListDirectory);
extern int  utFileAttribute(UTFILE *ptr_utFile, int iNewAttributes);
    #define FILE_ATTRIBUTE_SET_HIDE             0x01
    #define FILE_ATTRIBUTE_REMOVE_HIDE          0x02
    #define FILE_ATTRIBUTE_SET_WRITE_PROTECT    0x04
    #define FILE_ATTRIBUTE_REMOVE_WRITE_PROTECT 0x08
    #define FILE_ATTRIBUTE_OF_DIRECTORY         0x10
extern int  fnReadSector(unsigned char ucDisk, unsigned char *ptrBuffer, unsigned long ulSectorNumber);
extern int  fnWriteSector(unsigned char ucDisk, unsigned char *ptrBuffer, unsigned long ulSectorNumber);
extern int  fnPrepareBlockWrite(unsigned char ucDisk, unsigned long ulWriteBlocks, int iPreErase);
extern int  utFreeClusters(unsigned char ucDisk, UTASK_TASK owner_task);
extern int  utReadDirectory(UTLISTDIRECTORY *ptr_utListDirectory, UTFILEINFO *ptr_ut_fileInfo);
extern int  uMatchFileExtension(UTFILEINFO *ptrFileInfo, const CHAR *ptrExtension);

extern int utServer(UTDIRECTORY *ptr_utDirectory, unsigned long ulServerType);
    #define UTFAT_HTTP_SERVER                0x01
    #define UTFAT_HTTP_SERVER_ON             UTFAT_HTTP_SERVER
    #define UTFAT_HTTP_SERVER_OFF            (UTFAT_HTTP_SERVER << 16)
    #define UTFAT_HTTP_SERVER_ROOT_RESET     (UTFAT_HTTP_SERVER_OFF | UTFAT_HTTP_SERVER_ON)
    #define UTFAT_FTP_SERVER                 0x02
    #define UTFAT_FTP_SERVER_ON              UTFAT_FTP_SERVER
    #define UTFAT_FTP_SERVER_OFF             (UTFAT_FTP_SERVER << 16)
    #define UTFAT_FTP_SERVER_ROOT_RESET      (UTFAT_FTP_SERVER_OFF | UTFAT_FTP_SERVER_ON)

extern int  fnGetLocalFileTime(unsigned short *ptr_usCreationTime, unsigned short *ptr_usCreationDate); // this routine needs to be delivered

// Emulated FAT
//
typedef struct stEMULATED_FILE_DETAILS                                   // {2}
{
    unsigned long  ulFileLength;                                         // formatted length of the file (can be larger than its raw data size)
    #if defined EMULATED_FAT_FILE_DATE_CONTROL
        unsigned short usCreationTime;                                   // file creation time (not used if date is 0)
        unsigned short usCreationDate;                                   // file creation date (set 0 to use default)
    #endif
    #if defined EMULATED_FAT_FILE_NAME_CONTROL
        const CHAR    *ptrFileName;                                      // optional pointer to string to be used as file name
    #endif
    unsigned char ucValid;
} EMULATED_FILE_DETAILS;

extern void fnPrepareRootDirectory(const UTDISK *ptr_utDisk, int iRootSection);
extern const unsigned char *fnGetDataFile(int iDisk, int iDataRef, EMULATED_FILE_DETAILS *ptrFileDetails); // routine to be supplied by the application using emulated FAT drives
extern int uDatacopy(int iDisk, int iDataRef, unsigned char *ptrSectorData, const unsigned char *ptrSourceData, int iLength); // routine to be supplied by the application using emulated FAT drives

// Open modes - only the first byte is saved to the file object; the others are attributes for the controlling function
//
#define UTFAT_OPEN_FOR_READ        0x00000001                            // file to be opened for reading
#define UTFAT_OPEN_FOR_WRITE       0x00000002                            // file to be opened for writing to
#define UTFAT_OPEN_FOR_DELETE      0x00000004                            // file to be opened so that it can be deleted
#define UTFAT_PROTECTED            0x00000008                            // the file is to be opened and protected - no access by other users
#define UTFAT_MANAGED_MODE         0x00000010                            // open the file in managed mode so that any changes to it by other users are automatically updated
#define UTFAT_OPEN_FOR_RENAME      0x00000020                            // file to be opened so that it can be renamed
#define UTFAT_FILE_IS_DIR          0x00000080                            // the file is a directory type
#define UTFAT_TRUNCATE             0x00000200                            // if the file already exists truncate it so that its length is zero
#define UTFAT_CREATE               0x00000400                            // if the file doesn't exist create it
#define UTFAT_APPEND               0x00000800                            // an existing file opened for writing has its file pointer automatically set to the end
#define UTFAT_DISPLAY_INFO         0x00004000                            // display information to debug interface
#define UTFAT_OPEN_DELETED         0x00008000                            // deleted file referenced rather than valid file
#define UTFAT_COMMIT_FILE_ON_CLOSE 0x00010000                            // don't write file information on each write but delay until file is closed
#define UTFAT_WITH_DATA_CACHE      0x00020000                            // use a data cache (if available) between the file and the SD card's sector

// Internal file flags (not to be changed externally)
//
#define _FILE_CHANGED              0x20000000                            // file content has been changed but the file details are to be updated only on file close
#define _BUFFER_CHANGED            0x40000000                            // file buffer data needs to be saved (latest on a close)
#define _RENAME_EXISTING           0x80000000                            // internal attribute

#define UTFAT_SEEK_SET             0                                     // set the file position relative to the start of the file
#define UTFAT_SEEK_CUR             1                                     // set the file position relative to the current position
#define UTFAT_SEEK_END             2                                     // set the file position relative to the end fo the file

#endif

#if defined MANAGED_FILES
// Managed file object
//
typedef struct stMANAGED_FILE
{
    unsigned char  *managed_start;
    unsigned char  *managed_write;
    MAX_FILE_LENGTH managed_write_length;
    MAX_FILE_LENGTH managed_size;
    UTASK_TASK      managed_owner;
	DELAY_LIMIT     period;
	void (*fileOperationCallback)(int iHandle, int iResult);             // optional callback on completion
    unsigned char   managed_mode;
	unsigned char   ucParameters;
} MANAGED_FILE;

typedef struct stMANAGED_MEMORY_AREA_BLOCK
{
    unsigned char  *ptrStart;                                            // pointer to start of memory area
    MAX_FILE_LENGTH size;                                                // area size in bytes
	DELAY_LIMIT     period;                                              // maximum periodicity for the operation
    void (*fileOperationCallback)(int iHandle, int iResult);             // optional callback on completion
	unsigned char   ucParameters;                                        // specific operation parameters
} MANAGED_MEMORY_AREA_BLOCK;

#define AUTO_CLOSE                       0x01                            // automatically close the file on operation completion
#define AUTO_DELETE                      0x02                            // automatically delete the file on successful open
#define _DELAYED_DELETE                  0x80

#define MANAGED_READ                     0x00
#define MANAGED_WRITE                    0x01
#define MANAGED_LOCK                     0x02
#define MANAGED_DELETE                   0x04
#define MANAGED_MEMORY_AREA              0x08
#define MANAGED_FLUSH_FILE_SYSTEM        0x10
#define WAITING_DELETE                   0x20
#define WAITING_READ                     0x40
#define WAITING_WRITE                    0x80

#define MEDIA_BUSY                       -1

extern int uOpenManagedFile(void *ptrFileName, UTASK_TASK owner_task, unsigned char ucMode);
extern int uFileManagedDelete(int fileHandle);

#endif


#define CARD_BUSY_WAIT                   -1
#define ERROR_CARD_TIMEOUT               -2
#define ERROR_SECTOR_INVALID             -3


// Definitions for MMC/SDC commands (0x40 + command number)
//
#if defined SD_CONTROLLER_AVAILABLE
    #define COMMAND_OFFSET               0x00                            // no software offset since this is performed by the controller
#else
    #define COMMAND_OFFSET               0x40
#endif
#define GO_IDLE_STATE_CMD0               (COMMAND_OFFSET + 0)
#define SEND_OP_COND_CMD1                (COMMAND_OFFSET + 1)
#define SEND_CID_CMD2                    (COMMAND_OFFSET + 2)            // SD card mode
#define SET_REL_ADD_CMD3                 (COMMAND_OFFSET + 3)            // SD card mode
#define SET_BUS_WIDTH_CMD6               (COMMAND_OFFSET + 6)            // SD card mode
#define SELECT_CARD_CMD7                 (COMMAND_OFFSET + 7)            // SD card mode
#define SEND_IF_COND_CMD8                (COMMAND_OFFSET + 8)
#define SEND_CSD_CMD9                    (COMMAND_OFFSET + 9)
#define STOP_TRANSMISSION_CMD12          (COMMAND_OFFSET + 12)
#define SET_BLOCKLEN_CMD16               (COMMAND_OFFSET + 16)
#define READ_SINGLE_BLOCK_CMD17          (COMMAND_OFFSET + 17)
#define PRE_ERASE_BLOCKS_CMD23           (COMMAND_OFFSET + 23)
#define WRITE_BLOCK_CMD24                (COMMAND_OFFSET + 24)
#define WRITE_MULTIPLE_BLOCK_CMD25       (COMMAND_OFFSET + 25)
#define SEND_OP_COND_ACMD_CMD41          (COMMAND_OFFSET + 41)
#define APP_CMD_CMD55                    (COMMAND_OFFSET + 55)           // defines to the card that the following command is an application specific command rather than a standard command
#define READ_OCR_CMD58                   (COMMAND_OFFSET + 58)


#define VOLTAGE_2_7__3_6                 0x01
#define CHECK_PATTERN                    0xaa
#define CS_SEND_IF_COND_CMD8             0x87

#define HIGH_CAPACITY_SD_CARD_MEMORY     0x40

#define CS_GO_IDLE_STATE_CMD0            0x95
#define CS_SEND_OP_COND_ACMD_CMD41       0x00
#define CS_APP_CMD_CMD55                 0x00
#define CS_READ_OCR_CMD58                0x00
#define CS_SEND_OP_COND_CMD1             0x00
#define CS_SEND_CSD_CMD9                 0x00

#define R1_IN_IDLE_STATE                 0x01
#define R1_ERASE_RESET                   0x02
#define R1_ILLEGAL_COMMAND               0x04
#define R1_COMMAND_CRC_ERROR             0x08
#define R1_ERASE_SEQUENCE_ERROR          0x10
#define R1_ADDRESS_ERROR                 0x20
#define R1_PARAMETER_ERROR               0x40

#define CURRENT_CARD_STATUS_MASK         0x1e
#define CURRENT_STATE_IDENT              0x04
#define CURRENT_STATE_STBY               0x06
#define SD_CARD_READY_FOR_DATA           0x01


#define SDC_CARD_VERSION_2PLUS           0x01

#define SD_CARD_BUSY                     0x80


