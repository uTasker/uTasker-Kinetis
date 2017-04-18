/************************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher

    ---------------------------------------------------------------------
    File:      mass_storage.c
    Project:   uTasker project
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2016
    *********************************************************************
    11.07.2014 utFATV2.00
    05.08.2014 Correct long file rename end of directory save            {1}
    15.08.2014 Corrected =! to !=                                        {2}
    29.08.2014 Correct creating additional directory clusters            {3}
    03.09.2014 Remove fnCreateFile(), fnSetFileLocation() and fnInsertLFN_name() parameter {4}
    03.09.2014 Reset any deleted location markers when moving to next paragraph {5}
    06.10.2014 Correct brackets in fnExtractLongFileName()               {6} [uFATV2.01]
    30.11.2014 Add SPI_FLASH_FAT (run utFAT in external SPI based flash)
    03.12.2014 Don't display hidden files unless the HIDDEN_TYPE_LISTING flag is set and expert functions enabled {7}
    04.12.2014 Add FLASH_FAT (run utFAT in internal flash)
    11.12.2014 Add fnResetDirectories() for use when a disk is re-mounted and delete valid sector after re-formatting {8}
    13.12.2014 Ensure that the sector buffer is synchronised when writes are made using fnWriteSector() {9}
    22.01.2015 Add option to return a file's creation time and date in its file object {10}
    06.10.2015 Only when LFN is disabled: Corrected _utOpenDirectory() directory location returned when opening to write new files, plus reuse deleted directory space when possible {11}
    30.10.2015 Added emulated FAT support (FAT_EMULATION)                {12} [uFATV2.02]
    16.11.2015 Ensure that EMULATED_FAT_LUMS is available                {13}
    17.01.2016 Add utFileAttribute() - allows changing file attributes (not directories) {14}
    17.01.2016 Reset log file name counter when skipping hidden files    {15} [uFATV2.03]

*/


/* =================================================================== */
/*                           include files                             */
/* =================================================================== */

#include "config.h"

#define FAT12_DEVELOPMENT                                                // temporary for migration to FAT12 support

/* =================================================================== */
/*                          local definitions                          */
/* =================================================================== */


#if !defined UTFAT_DISABLE_DEBUG_OUT                                     // allow all debug messages to be disabled
    #define fnMemoryDebugMsg(x) fnDebugMsg(x)                            // enable debug output
    #define fnMemoryDebugHex(x, y) fnDebugHex((x), (y))                  // enable debug output
    #define fnMemoryDebugDec(x, y) fnDebugDec((x), (y))                  // enable debug output
#else
    #define fnMemoryDebugMsg(x)                                          // disable debug output
    #define fnMemoryDebugHex(x, y)                                       // disable debug output
    #define fnMemoryDebugDec(x, y)                                       // disable debug output
#endif

#if !defined SDCARD_MALLOC
    #define SDCARD_MALLOC(x)             uMalloc((MAX_MALLOC)(x))
#endif

#define OWN_TASK                         TASK_MASS_STORAGE

#define T_POWER_STABILISE                (DELAY_LIMIT)(SEC * 0.05)
#define T_NEXT_CHECK                     (DELAY_LIMIT)(SEC * SD_CARD_RETRY_INTERVAL)
#define T_CLEAN_SPARE                    (DELAY_LIMIT)(SEC * 0.10)

// Timer events
//
#define E_POWER_STABILISED               1
#define E_POLL_SD_CARD                   2
#define E_CLEAN_SPARE                    3
#define E_CHECK_CARD_REMOVAL             4                               // used when trying to detect removal by reading register

// Interrupt events
//
#define E_SDCARD_DETECTION_CHANGE        1

#define SD_STATE_STARTING                0
#define SD_STATE_WAIT_CARD               1
#define SD_STATE_STABILISING             2
#define SD_STATE_GO_IDLE                 3
#define SD_STATE_IF_COND                 4
#define SD_STATE_APP_CMD55_CMD41         5
#define SD_STATE_APP_CMD55_CMD41_2       6
#define SD_STATE_OCR                     7
#define SD_STATE_GET_INFO                8
#define SD_STATE_SET_ADDRESS             9
#define SD_STATE_GET_SECTORS             10
#define SD_SET_INTERFACE_PREPARE         11
#define SD_SET_INTERFACE                 12
#define SD_SELECT_CARD                   13
#define SD_SET_BLOCK_LENGTH              14
#define DISK_MOUNTING_1                  15
#define DISK_MOUNTING_2                  16
#define DISK_MOUNTING_3                  17
#define DISK_MOUNTING_4                  18
#define DISK_STATE_READY                 19

#define STATE_FORMATTING_DISK_1          20
#define STATE_FORMATTING_DISK_2          21
#define STATE_FORMATTING_DISK_3          22
#define STATE_FORMATTING_DISK_4          23
#define STATE_FORMATTING_DISK_5          24
#define STATE_FORMATTING_DISK_6          25
#define STATE_FORMATTING_DISK_7          26
#define STATE_FORMATTING_DISK_8          31
#define STATE_FORMATTING_DISK_8A         32
#define STATE_FORMATTING_DISK_9          33
#define STATE_FORMATTING_DISK_10         34
#define STATE_FORMATTING_DISK_11         35
#define STATE_CHECKING_DISK_REMOVAL      36

#define DISK_NOT_FORMATTED               50

#define DISK_HW_NOT_RESPONDING           60


#define _IDLE_MEMORY                     0x00
#define _INITIALISED_MEMORY              0x01
#define _READING_MEMORY                  0x02
#define _WRITING_MEMORY                  0x04
#define _FORMATTING_DISK                 0x08
#define _COUNTING_CLUSTERS               0x10

#define ENTRY_VOLUME_ID                 -5
#define ENTRY_DELETED                   -4
#define MATCH_NOT_LFN                   -3
#define END_DIRECTORY_ENTRIES           -2
#define MATCH_FALSE                     -1
#define MATCH_CONTINUE                   0
#define MATCH_SUCCESSFUL                 1
#define DELETED_LFN_MATCH_SUCCESSFUL     2


// File search results
//
#define FULLY_QUALIFIED_LONG_NAME        4
#define FULLY_QUALIFIED_LONG_NAME_SFNM   3
#define LONG_NAME_PARAGRAPH              2
#define FULLY_QUALIFIED_SHORT_NAME       1
#define SHORT_NAME_PARAGRAPH             0
#define INVALID_PARAGRAPH                -1


// FAT32 BPB_ExtFlags flags
//
#define BPB_ExtFlags_0_ZERO_BASED_NUMBER 0x0f                            // only valid if mirroring is disabled
#define BPB_ExtFlags_0_MIRRORED_FAT      0x00
#define BPB_ExtFlags_0_ONE_FAT           0x80                            // mirroring disabled

#define DIR_NAME_FREE                    0xe5

#define NT_FLAG                          0x18

#define NEW_ABSOLUTE_CLUSTER             0x00
#define INITIALISE_DIR_CLUSTER           0x01
#define INITIALISE_DIR_EXTENSION         0x02
#define UPDATE_FAT_END                   0x04
#define UPDATE_FAT_END_IN_DIFF_SECT      0x08
#define NEW_RELATIVE_CLUSTER             (UPDATE_FAT_END | UPDATE_FAT_END_IN_DIFF_SECT)


#define _CHAR_REJECT                     0x01
#define _CHAR_TERMINATE                  0x02
#define _CHAR_CAPITAL                    0x04
#define _CHAR_SMALL                      0x08
#define _CHAR_NUMBER                     0x10
#define _CHAR_REJECT_NON_JAP             0x20
#if defined UTFAT_LFN_WRITE
    #define _CHAR_ACCEPT_LFN             0x40
#else
    #define _CHAR_ACCEPT_LFN             0x00
#endif

#if defined SD_CONTROLLER_AVAILABLE
    #define SD_CONTROLLER_SHIFT          2
#else
    #define SD_CONTROLLER_SHIFT          0
#endif

#if !defined SFN_ENTRY_CACHE_SIZE
    #define SFN_ENTRY_CACHE_SIZE         1
#endif

#define SIMPLE_DELETE                    0
#define SAFE_DELETE                      1
#define REUSE_CLUSTERS                   2

#define ROOT_DIRECTORY_REFERENCE         0x01
#define ROOT_DIRECTORY_RELOCATE          0x02
#define ROOT_DIRECTORY_SET               0x04
#define ROOT_DIRECTORY_SETTING           0x08

#if !defined EMULATED_FAT_LUMS                                           // {13}
    #define EMULATED_FAT_LUMS            DISK_COUNT
#endif

/* =================================================================== */
/*                      local structure definitions                    */
/* =================================================================== */

#if defined SDCARD_SUPPORT || defined SPI_FLASH_FAT || defined FLASH_FAT || defined USB_MSD_HOST || defined FAT_EMULATION
#define DELETED_ENTRY_COUNT 21                                           // 0 is a reference to a single space, 1 to a double deleted hole in the directory objects, 1 to a tripple,.. 20 to a row of 21 deleted objects
typedef struct stOPEN_FILE_BLOCK
{
    const CHAR *ptrLocalDirPath;
    DISK_LOCATION *ptrDiskLocation;
    UTDISK *ptr_utDisk;
    unsigned long ulCluster;
    int iContinue;
    int iRootDirectory;
    int iQualifiedPathType;
    unsigned short usDirFlags;
    CHAR cShortFileName[8 + 3 + 1];                                      // space for 8:3 short file name format plus an NT specific byte

    const CHAR   *ptrFileNameStart;
    CHAR         *ptrFileNameMatch;
    const CHAR   *ptrFileNameEnd;
    DISK_LOCATION present_location;                                      // present location being treated
    #if defined UTFAT_LFN_READ && ((defined UTFAT_LFN_DELETE || defined UTFAT_LFN_WRITE) || defined UTFAT_EXPERT_FUNCTIONS)
    DISK_LOCATION lfn_file_location;                                     // the location where the LFN file name begins
    #endif
    unsigned long ulSFN_found;                                           // count of short file names found in the present directory
    #if defined UTFAT_WRITE
    DISK_LOCATION DirectoryEndLocation;                                  // location of the end of the present directory (after a complete scan)
    #endif
    #if defined UTFAT_LFN_WRITE
    DISK_LOCATION DeleteLocationRef;                                     // temporary sector reference to start of contiguous deleted entries
    DISK_LOCATION DeleteLocation[DELETED_ENTRY_COUNT];                   // location of double, tripple entry deletes found to ease reuse by new LFN entries (21 is maximum possible length of entries in LFN including a final SFN entry)
    #endif
    CHAR          cSFN_entry[SFN_ENTRY_CACHE_SIZE][13];                  // cache of short file names found in a directory
    unsigned char ucSFN_alias_checksum;
    #if defined UTFAT_LFN_WRITE
    unsigned char ucDeleteCount;                                         // temporary counter of contiguous deleted entries
    #endif
    #if defined UTFAT_LFN_READ && ((defined UTFAT_LFN_DELETE || defined UTFAT_LFN_WRITE) || defined UTFAT_EXPERT_FUNCTIONS)
    unsigned char ucLFN_entries;                                         // directory entries occupied by this LFN entry
    #endif
} OPEN_FILE_BLOCK;
#endif

typedef struct stFAT12_FAT
{
    unsigned long ulFat12SectorNumber;
    int iFat12Offset;
    int iFat12Tripplet;
    int iNoEntryIncrement;
} FAT12_FAT;

#if defined UTMANAGED_FILE_COUNT && UTMANAGED_FILE_COUNT > 0
    typedef struct stUTMANAGED_FILE
    {
        unsigned char   managed_mode;
        unsigned char   managed_owner;
        UTFILE         *utManagedFile;
    }   UTMANAGED_FILE;
#endif

#if defined FAT_EMULATION                                                // {12}
    typedef struct _PACK stDATA_FILE_INFO
    {
        int           iFileExists;
        unsigned long ulFormattedDatalength;
        unsigned long ulStartSector;
        unsigned long ulEndSector;
        unsigned long ulStartFATSector;
        unsigned long ulEndFATSector;
        int           iEndOffset;
    } DATA_FILE_INFO;
#endif


/* =================================================================== */
/*                 local function prototype declarations               */
/* =================================================================== */

#if defined SDCARD_SUPPORT || defined SPI_FLASH_FAT || defined FLASH_FAT || defined USB_MSD_HOST
    #if defined SDCARD_SUPPORT
        #if defined SD_CONTROLLER_AVAILABLE                              // routines supplied by HW specific module
            extern void fnInitSDCardInterface(void);
            extern int  fnSendSD_command(const unsigned char ucCommand[6], unsigned char *ucResult, unsigned char *ptrReturnData);
            extern int  fnGetSector(unsigned char *ptrBuf);       
            extern int  fnReadPartialSector(unsigned char *ptrBuf, unsigned short usStart, unsigned short usStop);
            extern int  fnPutSector(unsigned char *ptrBuf, int iMultiBlock);
        #else
            static int  fnWaitSD_ready(int iMaxWait);
            static int  fnSendSD_command(const unsigned char ucCommand[6], unsigned char *ucResult, unsigned char *ptrReturnData);
            static int  fnGetSector(unsigned char *ptrBuf);
            static int  fnReadPartialSector(unsigned char *ptrBuf, unsigned short usStart, unsigned short usStop);
        #endif
        static const unsigned char *fnCreateCommand(unsigned char ucCommand, unsigned long ulValue);
    #endif
    #if defined NAND_FLASH_FAT
        #include "NAND_driver.h"                                         // include NAND driver code
    #endif
    #if defined USB_MSD_HOST
        extern int utReadMSDsector(UTDISK *ptr_utDisk, unsigned long ulSectorNumber, void *ptrBuf);
        extern int utReadPartialMSDsector(UTDISK *ptr_utDisk, unsigned long ulSectorNumber, void *ptrBuf, unsigned short usOffset, unsigned short usLength);
        #if defined UTFAT_WRITE
            extern int utCommitMSDSectorData(UTDISK *ptr_utDisk, void *ptrBuffer, unsigned long ulSectorNumber);
            extern int utDeleteMSDSector(UTDISK *ptr_utDisk, unsigned long ulSectorNumber);
        #endif
    #endif
        
    static int  ut_read_disk(UTDISK *ptr_utDisk);
    static void fnCardNotFormatted(int iDisk);
    static void fnInitialisationError(int iDisk, int iNotSupported);
    static void fnResetDirectories(unsigned char ucDisk);
    #if defined UTFAT_WRITE        
        static unsigned long fnAllocateCluster(UTDISK *ptr_utDisk, unsigned long ulPresentCluster, unsigned char ucClusterType);
        static int  fnDeleteFileContent(UTFILE *ptr_utFile, UTDISK *ptr_utDisk, int iDestroyClusters);
        static int  fnDeleteClusterChain(unsigned long ulClusterStart, unsigned char ucDrive, int iDestroyClusters);
        static void fnAddInfoSect(INFO_SECTOR_FAT32 *ptrInfoSector, unsigned long ulFreeCount, unsigned long ulNextFree);
        static void fnSetTimeDate(DIR_ENTRY_STRUCTURE_FAT32 *ptrEntry, int iCreation);
    #endif
    #if defined UTFAT_LFN_READ && (defined UTFAT_LFN_DELETE || defined UTFAT_LFN_WRITE)
        static int fnDeleteLFN_entry(UTFILE *ptr_utFile);
        static int fnDirectorySectorCreate(UTDISK *ptr_utDisk, FILE_LOCATION *ptr_location);
    #endif
    #if defined UTMANAGED_FILE_COUNT && UTMANAGED_FILE_COUNT > 0
        static int fnFileLocked(UTFILE *ptr_utFile);
    #endif
    #if defined SDCARD_DETECT_INPUT_INTERRUPT 
        static void fnPrepareDetectInterrupt(void);
    #endif
    #if defined UTFAT_FILE_CACHE_POOL && (UTFAT_FILE_CACHE_POOL > 0) && (UTMANAGED_FILE_COUNT > 0)
        static int fnGetManagedFileCache(unsigned long ulSector, unsigned char *ptrBuffer, unsigned short usAccessOffset, unsigned short usAccessLength);
    #endif
#endif
#if ((defined SDCARD_SUPPORT || defined SPI_FLASH_FAT || defined FLASH_FAT || defined USB_MSD_HOST) && (defined UTFAT_WRITE)) || defined FAT_EMULATION
    static void fnAddInfoSect(INFO_SECTOR_FAT32 *ptrInfoSector, unsigned long ulFreeCount, unsigned long ulNextFree);
#endif
#if defined SDCARD_SUPPORT || defined SPI_FLASH_FAT || defined FLASH_FAT || defined USB_MSD_HOST || (defined FAT_EMULATION && defined EMULATED_FAT_FILE_NAME_CONTROL)
    static int fnCreateNameParagraph(const CHAR **pptrDirectoryPath, CHAR cDirectoryName[12]);
#endif
#if defined UTFAT_LFN_READ || (defined FAT_EMULATION && defined FAT_EMULATION_LFN  && defined EMULATED_FAT_FILE_NAME_CONTROL)
    static unsigned char fnLFN_checksum(CHAR *cPtrSFN_alias);
#endif
#if defined UTFAT12
    static int fnGetFat12_tripplet(unsigned long ulClusterNumber);
    static int fnGetFat12_cluster_entry(unsigned long ulPresentCluster, FAT12_FAT *ptr_fat12_fat);
    static unsigned long fnExtractFat12_cluster_value(FAT12_FAT *ptr_fat12_fat, unsigned long *ptrSource, int iAdditionalInput);
#endif

/* =================================================================== */
/*                             constants                               */
/* =================================================================== */


#if defined SDCARD_SUPPORT
    static const unsigned char ucGO_IDLE_STATE_CMD0[6]      = {GO_IDLE_STATE_CMD0, 0x00, 0x00, 0x00, 0x00, CS_GO_IDLE_STATE_CMD0};
    static const unsigned char ucIF_COND_CMD8[6]            = {SEND_IF_COND_CMD8, 0x00, 0x00, VOLTAGE_2_7__3_6, CHECK_PATTERN, CS_SEND_IF_COND_CMD8};
    #if defined SD_CONTROLLER_AVAILABLE                                  // no CRC is set to buffer since the SD controller appends this automatically {5}
        static const unsigned char ucSEND_OP_COND_ACMD_CMD41[5] = {SEND_OP_COND_ACMD_CMD41, HIGH_CAPACITY_SD_CARD_MEMORY, 0xff, 0x80, 0x00};
        static const unsigned char ucSET_REL_ADD_CMD3[5]    = {SET_REL_ADD_CMD3, 0x00, 0x00, 0x00, 0x00};
        static const unsigned char ucSEND_CID_CMD2[5]       = {SEND_CID_CMD2, 0x00, 0x00, 0x00, 0x00};
        static const unsigned char ucSET_BLOCK_LENGTH_CMD16[5]  = {SET_BLOCKLEN_CMD16, 0x00, 0x02, 0x00, 0x02}; // 512 byte block length
        static unsigned char ucSELECT_CARD_CMD7[5]          = {SELECT_CARD_CMD7, 0x00, 0x00, 0x00, 0x00};
        static unsigned char ucSEND_CSD_CMD9[5]             = {SEND_CSD_CMD9, 0x00, 0x00, 0x00, 0x00};
        static unsigned char ucAPP_CMD_CMD55[5]             = {APP_CMD_CMD55, 0x00, 0x00, 0x00, 0x00};
        static unsigned char ucSET_BUS_WIDTH_CMD6[5]        = {SET_BUS_WIDTH_CMD6, 0x00, 0x00, 0x00, 0x02};
    #else
        static const unsigned char ucSEND_OP_COND_ACMD_CMD41[6] = {SEND_OP_COND_ACMD_CMD41, HIGH_CAPACITY_SD_CARD_MEMORY, 0x00, 0x00, 0x00, CS_SEND_OP_COND_ACMD_CMD41};
        static const unsigned char ucSEND_CSD_CMD9[6]       = {SEND_CSD_CMD9, 0x00, 0x00, 0x00, 0x00, CS_SEND_CSD_CMD9};
        static const unsigned char ucAPP_CMD_CMD55[6]       = {APP_CMD_CMD55, 0x00, 0x00, 0x00, 0x00, CS_APP_CMD_CMD55};
        static const unsigned char ucREAD_OCR_CMD58[6]      = {READ_OCR_CMD58, 0x00, 0x00, 0x00, 0x00, CS_READ_OCR_CMD58};
    #endif
#endif
#if defined SDCARD_SUPPORT || defined SPI_FLASH_FAT || defined FLASH_FAT || defined USB_MSD_HOST
    #if defined UTFAT_WRITE && defined UTFAT_FORMATTING
static const unsigned char ucEmptyFAT32[12] = {
    LITTLE_LONG_WORD_BYTES(MEDIA_VALUE_FIXED),
    LITTLE_LONG_WORD_BYTES(0xffffffff),
    LITTLE_LONG_WORD_BYTES(CLUSTER_MASK)
};
        #if defined UTFAT16
static const unsigned char ucEmptyFAT16[4] = {
    LITTLE_LONG_WORD_BYTES(0xfffffff8)
};
        #endif
        #if defined UTFAT12
static const unsigned char ucEmptyFAT12[4] = {
    LITTLE_LONG_WORD_BYTES(0x00fffff8)
};
        #endif
    #endif
#endif
#if defined SDCARD_SUPPORT || defined SPI_FLASH_FAT || defined FLASH_FAT || defined USB_MSD_HOST || (defined FAT_EMULATION && defined EMULATED_FAT_FILE_NAME_CONTROL)
static const unsigned char ucCharacterTable[] = {
    (0),                                                                 // !
    (_CHAR_REJECT),                                                      // "
    (0),                                                                 // #
    (0),                                                                 // $
    (0),                                                                 // %
    (0),                                                                 // &
    (0),                                                                 // ´
    (0),                                                                 // (
    (0),                                                                 // )
    (_CHAR_REJECT),                                                      // *
    (_CHAR_REJECT),                                                      // +
    (0),                                                                 // ,
    (0),                                                                 // -
    (0),                                                                 // .
    (_CHAR_TERMINATE),                                                   // /
    (_CHAR_NUMBER),                                                      // 0
    (_CHAR_NUMBER),                                                      // 1
    (_CHAR_NUMBER),                                                      // 2
    (_CHAR_NUMBER),                                                      // 3
    (_CHAR_NUMBER),                                                      // 4
    (_CHAR_NUMBER),                                                      // 5
    (_CHAR_NUMBER),                                                      // 6
    (_CHAR_NUMBER),                                                      // 7
    (_CHAR_NUMBER),                                                      // 8
    (_CHAR_NUMBER),                                                      // 9
    (_CHAR_REJECT),                                                      // :
    (_CHAR_REJECT),                                                      // ;
    (_CHAR_REJECT),                                                      // <
    (_CHAR_REJECT),                                                      // =
    (_CHAR_REJECT),                                                      // >
    (0),                                                                 // ?
    (0),                                                                 // @
    (_CHAR_CAPITAL),                                                     // A
    (_CHAR_CAPITAL),                                                     // B
    (_CHAR_CAPITAL),                                                     // C
    (_CHAR_CAPITAL),                                                     // D
    (_CHAR_CAPITAL),                                                     // E
    (_CHAR_CAPITAL),                                                     // F
    (_CHAR_CAPITAL),                                                     // G
    (_CHAR_CAPITAL),                                                     // H
    (_CHAR_CAPITAL),                                                     // I
    (_CHAR_CAPITAL),                                                     // J
    (_CHAR_CAPITAL),                                                     // K
    (_CHAR_CAPITAL),                                                     // L
    (_CHAR_CAPITAL),                                                     // M
    (_CHAR_CAPITAL),                                                     // N
    (_CHAR_CAPITAL),                                                     // O
    (_CHAR_CAPITAL),                                                     // P
    (_CHAR_CAPITAL),                                                     // Q
    (_CHAR_CAPITAL),                                                     // R
    (_CHAR_CAPITAL),                                                     // S
    (_CHAR_CAPITAL),                                                     // T
    (_CHAR_CAPITAL),                                                     // U
    (_CHAR_CAPITAL),                                                     // V
    (_CHAR_CAPITAL),                                                     // W
    (_CHAR_CAPITAL),                                                     // X
    (_CHAR_CAPITAL),                                                     // Y
    (_CHAR_CAPITAL),                                                     // Z
    (_CHAR_REJECT_NON_JAP),                                              // [
    (_CHAR_TERMINATE),                                                   // back slash
    (_CHAR_REJECT_NON_JAP),                                              // ]
    (0),                                                                 // ^
    (0),                                                                 // _
    (0),                                                                 // '
    (_CHAR_SMALL),                                                       // a
    (_CHAR_SMALL),                                                       // b
    (_CHAR_SMALL),                                                       // c
    (_CHAR_SMALL),                                                       // d
    (_CHAR_SMALL),                                                       // e
    (_CHAR_SMALL),                                                       // f
    (_CHAR_SMALL),                                                       // g
    (_CHAR_SMALL),                                                       // h
    (_CHAR_SMALL),                                                       // i
    (_CHAR_SMALL),                                                       // j
    (_CHAR_SMALL),                                                       // k
    (_CHAR_SMALL),                                                       // l
    (_CHAR_SMALL),                                                       // m
    (_CHAR_SMALL),                                                       // n
    (_CHAR_SMALL),                                                       // o
    (_CHAR_SMALL),                                                       // p
    (_CHAR_SMALL),                                                       // q
    (_CHAR_SMALL),                                                       // r
    (_CHAR_SMALL),                                                       // s
    (_CHAR_SMALL),                                                       // t
    (_CHAR_SMALL),                                                       // u
    (_CHAR_SMALL),                                                       // v
    (_CHAR_SMALL),                                                       // w
    (_CHAR_SMALL),                                                       // x
    (_CHAR_SMALL),                                                       // y
    (_CHAR_SMALL),                                                       // z
    (0),                                                                 // {
    (_CHAR_REJECT_NON_JAP),                                              // |
    (0),                                                                 // }
    (0),                                                                 // ~
};
#endif

/* =================================================================== */
/*                     global variable definitions                     */
/* =================================================================== */


/* =================================================================== */
/*                      local variable definitions                     */
/* =================================================================== */

#if defined SDCARD_SUPPORT || defined SPI_FLASH_FAT || defined FLASH_FAT || defined USB_MSD_HOST
static UTDISK utDisks[DISK_COUNT] = {{0}};                               // disk objects

#if defined SDCARD_SUPPORT && !defined SD_CONTROLLER_AVAILABLE && !defined NAND_FLASH_FAT
    #if defined _WINDOWS
        static int CommandTimeout = 10;
    #else
        static int CommandTimeout = 20000;                               // change depending on SPI speed
    #endif
#endif

static int iMemoryOperation[DISK_COUNT] = {0};
static int iMemoryState[DISK_COUNT] = {0};

#if defined UTFAT_MULTIPLE_BLOCK_WRITE
    static unsigned long ulBlockWriteLength = 0;                         // multiple block writing to speed up write operations
    static unsigned long ulMultiBlockAddress = 0;
#endif

#if defined UTFAT12
    static unsigned long ulLastFAT12_value[DISK_COUNT] = {0};
    static unsigned long ulMaximumClusterCount[DISK_COUNT] = {0};
#endif
static unsigned long  ulClusterSectorCheck[DISK_COUNT] = {0};
static unsigned long  ulActiveFreeClusterCount[DISK_COUNT] = {0};
static UTASK_TASK     cluster_task[DISK_COUNT] = {0};
#if defined HTTP_ROOT || defined FTP_ROOT
    static unsigned short usServerStates = 0;
    static unsigned short usServerResets = 0;
#endif

#if defined UTFAT_WRITE && (defined UTFAT_FILE_CACHE_POOL && UTFAT_FILE_CACHE_POOL > 0)
    #define FILE_BUFFER_FREE      0x00
    #define FILE_BUFFER_IN_USE    0x01
    #define FILE_BUFFER_VALID     0x02
    #define FILE_BUFFER_MODIFIED  0x04
    static FILE_DATA_CACHE FileDataCache[UTFAT_FILE_CACHE_POOL] = {{0}}; // initially all area free to be allocated
#endif
#endif

#if defined FAT_EMULATION                                                // {12}
    static DATA_FILE_INFO dataFileList[EMULATED_FAT_LUMS][MAXIMUM_DATA_FILES] = {{{0}}};
    #if defined ROOT_DIR_SECTORS
        static DIR_ENTRY_STRUCTURE_FAT32 root_file[EMULATED_FAT_LUMS][(ROOT_DIR_SECTORS * (BYTES_PER_SECTOR/sizeof(DIR_ENTRY_STRUCTURE_FAT32)))] = {{{{0}}}}; // copy of present root directory (a number of sectors are maintained, depending on root directory size to hold maximum file objects [LFN can use multiple objects per file])
    #else
        static unsigned char ucRootSectorCount[EMULATED_FAT_LUMS] = {0};
    #endif
#endif


#if defined UTMANAGED_FILE_COUNT && UTMANAGED_FILE_COUNT > 0
    static UTMANAGED_FILE utManagedFiles[UTMANAGED_FILE_COUNT] = {{0}};
#endif

#if defined MANAGED_FILES
    #if defined SUB_FILE_SIZE
        #define SUBFILE_WRITE  , ucSubFileInProgress
        #define SUB_FILE_ON    ,SUB_FILE_TYPE
    #else
        #define SUBFILE_WRITE
        #define SUB_FILE_ON
    #endif

    static int iManagedMedia = 0;

    static MANAGED_FILE managed_files[MANAGED_FILE_COUNT] = {{0}};


extern int uFileManagedDelete(int fileHandle)
{
	int iEraseStatus = 0;
    if (managed_files[fileHandle].managed_size != 0) {
        iEraseStatus = fnEraseFlashSector(managed_files[fileHandle].managed_write, 0); // start single page erase
    }
    #if defined TIME_SLICED_FILE_OPERATION
    else {                                                               // final sector delete has already started
        managed_files[fileHandle].period = 0;
    }
    #endif
	if (iEraseStatus != MEDIA_BUSY) {
        if (managed_files[fileHandle].managed_size == 0) {               // delete has completed (after one polly)
			managed_files[fileHandle].managed_mode = 0;
          //managed_files[fileHandle].managed_mode &= ~WAITING_DELETE;
			if (managed_files[fileHandle].ucParameters & AUTO_CLOSE) {
				managed_files[fileHandle].managed_owner = 0;
			}
			if (managed_files[fileHandle].fileOperationCallback) {
				managed_files[fileHandle].fileOperationCallback(fileHandle, 0);
			}
			return 0;                                                    // complete
		}
		else {
            managed_files[fileHandle].managed_write += iEraseStatus;     // erase of block started so increment the erase pointer
            if ((unsigned int)iEraseStatus >= managed_files[fileHandle].managed_size) {
                managed_files[fileHandle].managed_size = 0;              // final sector delete started
            }
            else {
			    managed_files[fileHandle].managed_size -= iEraseStatus;
            }
		}
    #if defined TIME_SLICED_FILE_OPERATION
		if (managed_files[fileHandle].period != 0) {                     // if a page delete period rate is defined wait until this expires before continuing
			uTaskerGlobalMonoTimer(OWN_TASK, managed_files[fileHandle].period, (unsigned char)(fileHandle | _DELAYED_DELETE));
            return 1;
		}
    #endif
	}
	managed_files[fileHandle].managed_mode |= WAITING_DELETE;            // either waiting to delete a busy page or waiting for a page to complete deletion
    uTaskerStateChange(OWN_TASK, UTASKER_ACTIVATE);                      // set to run again
    iManagedMedia = 1;                                                   // mark that this media type needs monitoring
    return 1;                                                            // delete not yet complete
}

    #if defined MANAGED_FILE_WRITE
extern int uFileManagedWrite(int fileHandle)
{
    int iWriteStatus;
    MAX_FILE_LENGTH write_chunk_length = managed_files[fileHandle].managed_chunk_size; // the maximum write size that is specified
    if ((write_chunk_length == 0) || (write_chunk_length > managed_files[fileHandle].managed_size)) {
        write_chunk_length = managed_files[fileHandle].managed_size;     // maximum remaining size possible
    }
	iWriteStatus = fnWriteBytesFlashNonBlocking(managed_files[fileHandle].managed_write, managed_files[fileHandle].managed_buffer, write_chunk_length); // start a single block write
    if (iWriteStatus != MEDIA_BUSY) {                                    // if the storage media was not busy the write has been started
        if (managed_files[fileHandle].managed_size <= write_chunk_length) { // final write completed
            if (managed_files[fileHandle].managed_size == 0) {           // we never terminate immediately after the final write has started but always perform a dummy poll to avoid recursive calls (when writes always terminate immediately)
                if (managed_files[fileHandle].ucParameters & AUTO_CLOSE) {
                    managed_files[fileHandle].managed_owner = 0;         // automatically close on completion
                }
                if (managed_files[fileHandle].fileOperationCallback) {   // if there is a user callback defined
                    managed_files[fileHandle].fileOperationCallback(fileHandle, 0);
                }
                return 0;                                                // complete
            }
        #if defined TIME_SLICED_FILE_OPERATION
            else {
                managed_files[fileHandle].period = 0;
            }
        #endif
		}
        else {
            managed_files[fileHandle].managed_write += write_chunk_length; // write of block started so increment the write pointer
            managed_files[fileHandle].managed_buffer += write_chunk_length; // and increment the buffer pointer
        }
        managed_files[fileHandle].managed_size -= write_chunk_length;
        #if defined TIME_SLICED_FILE_OPERATION
		if (managed_files[fileHandle].period != 0) {                     // if a block write period rate is defined wait until this expires before continuing
			uTaskerGlobalMonoTimer(OWN_TASK, managed_files[fileHandle].period, (unsigned char)(fileHandle | _DELAYED_WRITE));
            return 1;
		}
        #endif
	}
	managed_files[fileHandle].managed_mode |= WAITING_WRITE;             // waiting to write a busy page
    uTaskerStateChange(OWN_TASK, UTASKER_ACTIVATE);                      // set to run again
    iManagedMedia = 1;                                                   // mark that this media type needs monitoring
    return 1;                                                            // write not yet complete
}
    #endif

    #if defined MANAGED_FILE_READ
extern int uFileManagedRead(int fileHandle)
{
    int iReadStatus;
    MAX_FILE_LENGTH read_chunk_length = managed_files[fileHandle].managed_chunk_size; // the maximum read size that is specified
    if (read_chunk_length > managed_files[fileHandle].managed_size) {
        read_chunk_length = managed_files[fileHandle].managed_size;      // remaining size
    }
	iReadStatus = fnReadBytesFlashNonBlocking(managed_files[fileHandle].managed_write, managed_files[fileHandle].managed_buffer, read_chunk_length); // start a single block read
    if (iReadStatus != MEDIA_BUSY) {                                     // if the storage media was not busy the read has completed
        if (managed_files[fileHandle].managed_size <= read_chunk_length) { // final read completed
            if (managed_files[fileHandle].managed_size == 0) {           // we never terminate immediately after the final read has started but always perform a dummy poll to avoid recursive calls (when writes always terminate immediately)
                if (managed_files[fileHandle].ucParameters & AUTO_CLOSE) {
                    managed_files[fileHandle].managed_owner = 0;         // automatically close on completion
                }
                if (managed_files[fileHandle].fileOperationCallback) {   // if there is a user callback defined
                    managed_files[fileHandle].fileOperationCallback(fileHandle, 0);
                }
                return 0;                                                // complete
            }
        #if defined TIME_SLICED_FILE_OPERATION
            else {
                managed_files[fileHandle].period = 0;
            }
        #endif
		}
        else {
            managed_files[fileHandle].managed_write += read_chunk_length; // read from block completed so increment the read pointer
            managed_files[fileHandle].managed_buffer += read_chunk_length; // and increment the buffer pointer
        }
        managed_files[fileHandle].managed_size -= read_chunk_length;
        #if defined TIME_SLICED_FILE_OPERATION
		if (managed_files[fileHandle].period != 0) {                     // if a block write period rate is defined wait until this expires before continuing
			uTaskerGlobalMonoTimer(OWN_TASK, managed_files[fileHandle].period, (unsigned char)(fileHandle | _DELAYED_READ));
            return 1;
		}
        #endif
	}
	managed_files[fileHandle].managed_mode |= WAITING_READ;              // waiting to read a busy page
    uTaskerStateChange(OWN_TASK, UTASKER_ACTIVATE);                      // set to run again
    iManagedMedia = 1;                                                   // mark that this media type needs monitoring
    return 1;                                                            // write not yet complete
}
    #endif

extern int uOpenManagedFile(void *ptrFileName, UTASK_TASK owner_task, unsigned char ucMode)
{
    int i = 0;
    int iFree = 0;
    unsigned char *ptrFile;
	MAX_FILE_LENGTH file_length;
	if (ucMode & MANAGED_MEMORY_AREA) {
		MANAGED_MEMORY_AREA_BLOCK *ptrMemoryArea = (MANAGED_MEMORY_AREA_BLOCK *)ptrFileName;
		ptrFile = ptrMemoryArea->ptrStart;
		file_length = ptrMemoryArea->size;                               // size of the area
	}
	else {
        ptrFile = uOpenFile((CHAR *)ptrFileName);                        // get the file to be opened from the uFileSystem
		file_length = uGetFileLength(ptrFile);                           // file length of existing file
	}
    while (i < MANAGED_FILE_COUNT) {                                     // check whether this file is presently being protected
        if (managed_files[i].managed_owner != 0) {
            if (managed_files[i].managed_mode & (MANAGED_WRITE | MANAGED_LOCK | MANAGED_DELETE)) {
                if ((ptrFile >= managed_files[i].managed_start) && (ptrFile < (managed_files[i].managed_start + managed_files[i].managed_size))) {
                    return MANAGED_FILE_NO_ACCESS;                       // this file can presently not be accessed
                }
            }
        }
        else {
            iFree = (i + 1);                                             // free space found
        }
        i++;
    }
    if (iFree-- == 0) {
        return MANAGED_FILE_NO_FILE_HANDLE;                              // all managed file spaces are presently occupied
    }
    managed_files[iFree].managed_owner = owner_task;
    managed_files[iFree].managed_write = managed_files[iFree].managed_start = ptrFile;
    managed_files[iFree].managed_mode = ucMode;
    managed_files[iFree].managed_size = file_length;                     // file length of existing file
    if (ucMode & MANAGED_MEMORY_AREA) {
		MANAGED_MEMORY_AREA_BLOCK *ptrMemoryArea = (MANAGED_MEMORY_AREA_BLOCK *)ptrFileName;
		managed_files[iFree].ucParameters = ptrMemoryArea->ucParameters;
		managed_files[iFree].fileOperationCallback = ptrMemoryArea->fileOperationCallback;
		managed_files[iFree].period = ptrMemoryArea->period;
    #if defined MANAGED_FILE_WRITE || defined MANAGED_FILE_READ          // parameters only used by writes
        managed_files[iFree].managed_buffer = ptrMemoryArea->ptrBuffer;
        managed_files[iFree].managed_chunk_size = ptrMemoryArea->chunk_size;
    #endif
		if (ptrMemoryArea->ucParameters & AUTO_DELETE) {                 // if a delete is to be performed immediately call the function
			uFileManagedDelete(iFree);                                   // start deletion
		}
    #if defined MANAGED_FILE_WRITE
        else if (ptrMemoryArea->ucParameters & AUTO_WRITE) {             // if a write is to be performed immediately call the function
			uFileManagedWrite(iFree);                                    // start write
        }
    #endif
    #if defined MANAGED_FILE_READ
        else if (ptrMemoryArea->ucParameters & AUTO_READ) {              // if a write is to be performed immediately call the function
			uFileManagedRead(iFree);                                     // start read
        }
    #endif
	}
    return iFree;                                                        // return file handle
}

static void fnManagedMediaCheck(void)
{
    int iManagedFileEntry = MANAGED_FILE_COUNT;
    while (iManagedFileEntry > 0) {                                      // check all managed files
        iManagedFileEntry--;
        if (managed_files[iManagedFileEntry].managed_owner != 0) {       // for each one that is owned
            if (managed_files[iManagedFileEntry].managed_mode & WAITING_DELETE) { // waiting to start or continue delete operation
                uFileManagedDelete(iManagedFileEntry);
			}
    #if defined MANAGED_FILE_WRITE
            else if (managed_files[iManagedFileEntry].managed_mode & WAITING_WRITE) { // waiting to start or continue write operation
                uFileManagedWrite(iManagedFileEntry);
            }
    #endif
    #if defined MANAGED_FILE_READ
            else if (managed_files[iManagedFileEntry].managed_mode & WAITING_READ) { // waiting to start or continue write operation
                uFileManagedRead(iManagedFileEntry);
            }
    #endif
        }
    }
}
#endif



#if defined SDCARD_SUPPORT
// Read a part of the specified sector to the buffer (avoiding overwriting all buffer content)
//
static int utReadPartialSector(UTDISK *ptr_utDisk, unsigned long ulSectorNumber, void *ptrBuf, unsigned short usOffset, unsigned short usLength)
{
    static unsigned long ulSector;
    static int iActionResult;
    switch (iMemoryOperation[DISK_SDCARD] & _READING_MEMORY) {
    case _IDLE_MEMORY:
        if ((ptr_utDisk->usDiskFlags & HIGH_CAPACITY_SD_CARD) == 0) {
            ulSectorNumber *= 512;                                       // convert the sector number to byte address
        }
        SET_SD_CS_LOW();
        iMemoryOperation[DISK_SDCARD] |= _READING_MEMORY;
        ulSector = ulSectorNumber;
    case _READING_MEMORY:
        {
            unsigned char ucResult;
            while ((iActionResult = fnSendSD_command(fnCreateCommand(READ_SINGLE_BLOCK_CMD17, ulSector), &ucResult, 0)) == CARD_BUSY_WAIT) {}
            if (iActionResult < 0) {
                SET_SD_CS_HIGH();
                iMemoryOperation[DISK_SDCARD] &= ~_READING_MEMORY;       // read operation has completed
                return iActionResult;
            }
            if (ucResult == 0) {
                fnReadPartialSector(ptrBuf, usOffset, (unsigned short)(usOffset + usLength)); // start reading a sector direct to buffer
            }
            SET_SD_CS_HIGH();
            iMemoryOperation[DISK_SDCARD] &= ~_READING_MEMORY;           // read operation has completed
        }
        break;
    }
    return UTFAT_SUCCESS;
}

// Read a single, complete sector from the disk to the specified buffer
//
static int utReadDiskSector(UTDISK *ptr_utDisk, unsigned long ulSectorNumber, void *ptrBuf)
{
    static int iActionResult = UTFAT_SUCCESS;
    static unsigned long ulSector;
    switch (iMemoryOperation[DISK_SDCARD] & _READING_MEMORY) {
    case _IDLE_MEMORY:
        if ((ptr_utDisk->usDiskFlags & HIGH_CAPACITY_SD_CARD) == 0) {
            ulSectorNumber *= 512;                                       // convert the sector number to byte address
        }
        SET_SD_CS_LOW();
        iMemoryOperation[DISK_SDCARD] |= _READING_MEMORY;
        ulSector = ulSectorNumber;
    case _READING_MEMORY:
        {
            unsigned char ucResult;
            while ((iActionResult = fnSendSD_command(fnCreateCommand(READ_SINGLE_BLOCK_CMD17, ulSector), &ucResult, 0)) == CARD_BUSY_WAIT) {}
            if (iActionResult < 0) {
                SET_SD_CS_HIGH();
                iMemoryOperation[DISK_SDCARD] &= ~_READING_MEMORY;       // read operation has completed
                return iActionResult;
            }
            if (ucResult == 0) {
                iActionResult = fnGetSector(ptrBuf);                     // read a single sector to the buffer
            }
            SET_SD_CS_HIGH();
            iMemoryOperation[DISK_SDCARD] &= ~_READING_MEMORY;           // read operation has completed
        }
        break;
    }
    return iActionResult;
}
#endif

#if defined SDCARD_SUPPORT && defined UTFAT_WRITE
    #if !defined SD_CONTROLLER_AVAILABLE && !defined NAND_FLASH_FAT       // SPI interface is local - SD card interface uses HW specific external routines
// Write present sector with buffer data
//
static int fnPutSector(unsigned char *ptrBuf, int iMultiBlock)
{
    #define DATA_TOKEN 0xfe
    unsigned char ucResponse;
    int iLength = 512;
    do {
        WRITE_SPI_CMD(0xff);
        WAIT_TRANSMISSON_END();                                          // wait until transmission complete
    } while ((ucResponse = READ_SPI_DATA()) != 0xff);
    WRITE_SPI_CMD(DATA_TOKEN);                                           // transmit data byte
    WAIT_TRANSMISSON_END();                                              // wait until transmission complete
    (void)READ_SPI_DATA();                                               // dummy read to clear the receiver
    do {
        WRITE_SPI_CMD(*ptrBuf);                                          // transmit data byte
        ptrBuf++;
        WAIT_TRANSMISSON_END();                                          // wait until transmission complete
        (void)READ_SPI_DATA();                                           // clear the receiver with a dummy read
    } while (--iLength);
    WRITE_SPI_CMD(0xff);                                                 // send two dummy CRC bytes
    WAIT_TRANSMISSON_END();                                              // wait until transmission complete
    (void)READ_SPI_DATA();                                               // dummy read to clear the receiver
    WRITE_SPI_CMD(0xff);
    WAIT_TRANSMISSON_END();                                              // wait until transmission complete
    (void)READ_SPI_DATA();                                               // dummy read to clear the receiver
    WRITE_SPI_CMD(0xff);                                                 // send two dummy CRC bytes
    WAIT_TRANSMISSON_END();                                              // wait until transmission complete
    ucResponse = READ_SPI_DATA();
    if ((ucResponse & 0x1f) != 0x05) {
        return UTFAT_DISK_WRITE_ERROR;
    }
    return UTFAT_SUCCESS;                                                // write successfully completed
}
    #endif

static int utCommitSectorData(UTDISK *ptr_utDisk, void *ptrBuffer, unsigned long ulSectorNumber)
{
    static unsigned long ulSector;
    static int iActionResult;
    switch (iMemoryOperation[DISK_SDCARD] & _WRITING_MEMORY) {
    case _IDLE_MEMORY:
        if ((ptr_utDisk->usDiskFlags & HIGH_CAPACITY_SD_CARD) == 0) {
            ulSectorNumber *= 512;                                       // convert the sector number to byte address
        }
        SET_SD_CS_LOW();
        iMemoryOperation[DISK_SDCARD] |= _WRITING_MEMORY;
        ulSector = ulSectorNumber;
    case _WRITING_MEMORY:
        {
            unsigned char ucResult;
    #if defined UTFAT_MULTIPLE_BLOCK_WRITE
            if (ulBlockWriteLength != 0) {                               // if multiple block writes are to be performed
                if (!(ulBlockWriteLength & 0x80000000)) {                // if block write command has not yet been sent
                    if ((iActionResult = fnSendSD_command(fnCreateCommand(WRITE_MULTIPLE_BLOCK_CMD25, ulSector), &ucResult, (unsigned char *)&ulBlockWriteLength)) != 0) { // start multiple block write
                        return iActionResult;
                    }
                    ulMultiBlockAddress = ulSector;
                    ulBlockWriteLength |= 0x80000000;                    // mark that the command has been sent
                }
                else {
                /*  if (!(ulBlockWriteLength & ~0x80000000)) {           // if multiple write is being aborted
                        ulBlockWriteLength = 0;
                        while (fnSendSD_command(fnCreateCommand(STOP_TRANSMISSION_CMD12, 0), &ucResult, 0) == CARD_BUSY_WAIT) {}; // terminate present multiple block mode
                        if ((iActionResult = fnSendSD_command(fnCreateCommand(WRITE_BLOCK_CMD24, ulSector), &ucResult, 0)) != 0) { // single block write command
                            return iActionResult;
                        }
                    }*/
                    ucResult = 0;                                        // since we are in a multiple write state we can continue writing data
                }
            }
            else {
                if ((iActionResult = fnSendSD_command(fnCreateCommand(WRITE_BLOCK_CMD24, ulSector), &ucResult, 0)) != 0) { // single block write command
                    return iActionResult;
                }
            }
            if (ucResult == 0) {
                if (fnPutSector((unsigned char *)ptrBuffer, (ulBlockWriteLength != 0)) != UTFAT_SUCCESS) { // start writing buffer to single sector
                    fnMemoryDebugMsg("Write error\r\n");
                    return UTFAT_DISK_WRITE_ERROR;
                }
                if (!(ptr_utDisk->usDiskFlags & HIGH_CAPACITY_SD_CARD)) {
                    ulMultiBlockAddress += 512;                         // monitor the multiple write sector
                }
                else {
                    ulMultiBlockAddress++;
                }
            }
    #else
            if ((iActionResult = fnSendSD_command(fnCreateCommand(WRITE_BLOCK_CMD24, ulSector), &ucResult, 0)) != 0) { // single block write command
                return iActionResult;
            }
            if (ucResult == 0) {
                if (fnPutSector((unsigned char *)ptrBuffer, 0) != UTFAT_SUCCESS) { // start writing buffer to single sector
                    fnMemoryDebugMsg("Write error\r\n");
                    return UTFAT_DISK_WRITE_ERROR;
                }
            }
    #endif
    #if defined UTFAT_MULTIPLE_BLOCK_WRITE
            if (ulBlockWriteLength != 0) {                               // in multiple block write
                ulBlockWriteLength--;                                    // block has been written
                if ((ulBlockWriteLength & ~(0x80000000)) == 0) {         // final block has been written
                    ulBlockWriteLength = 0;                              // planned multiple block write has completed
                    fnSendSD_command(fnCreateCommand(STOP_TRANSMISSION_CMD12, 0), &ucResult, 0); // terminate multiple block mode
                }
            }
    #endif
            SET_SD_CS_HIGH();
            iMemoryOperation[DISK_SDCARD] &= ~_WRITING_MEMORY;           // write operation has completed
        }
        break;
    }
    return UTFAT_SUCCESS;
}

// Delete the specified sector by writing data content of 0x00
//
static int utDeleteSector(UTDISK *ptr_utDisk, unsigned long ulSectorNumber)
{
    static unsigned long ulSector;
    static int iActionResult;
    switch (iMemoryOperation[DISK_SDCARD] & _WRITING_MEMORY) {
    case _IDLE_MEMORY:
        if ((ptr_utDisk->usDiskFlags & HIGH_CAPACITY_SD_CARD) == 0) {
            ulSectorNumber *= 512;                                       // convert the sector number to byte address
        }
        SET_SD_CS_LOW();
        iMemoryOperation[DISK_SDCARD] |= _WRITING_MEMORY;
        ulSector = ulSectorNumber;
    case _WRITING_MEMORY:
        {
            unsigned char ucResult;
            if ((iActionResult = fnSendSD_command(fnCreateCommand(WRITE_BLOCK_CMD24, ulSector), &ucResult, 0)) != 0) {
                return iActionResult;
            }
            if (ucResult == 0) {
                unsigned long ulTemp[512/sizeof(unsigned long)];         // temporary long-word aligned buffer
    #if defined LONG_UMEMSET
                uMemset_long(ulTemp, 0x00, sizeof(ulTemp));              // zero buffer content for delete
    #else
                uMemset(ulTemp, 0x00, sizeof(ulTemp));
    #endif
                if (fnPutSector((unsigned char *)ulTemp, 0) != UTFAT_SUCCESS) { // write sector content to 0x00
                    fnMemoryDebugMsg("Write error\r\n");
                    return UTFAT_DISK_WRITE_ERROR;
                }
            }
            SET_SD_CS_HIGH();
            iMemoryOperation[DISK_SDCARD] &= ~_WRITING_MEMORY;           // write operation has completed
        }
        break;
    }
    return UTFAT_SUCCESS;
}
#endif



// Temporary development
//
#if defined FLASH_FAT || defined SPI_FLASH_FAT
#if !defined FLASH_FAT_MALLOC
    #define FLASH_FAT_MALLOC(x) uMalloc((MAX_MALLOC)(x))
#endif

#if defined SIMPLE_FLASH
    #define FLASH_FAT_START_ADDRESS     FLASH_FAT_MANAGEMENT_ADDRESS
    #define FLASH_FAT_SECTORS           (FLASH_FAT_SIZE/512)             // number of sectors that are available from the FAT's point of view
    #define FLASH_FAT_SIZE              (INTERNAL_FLASH_FAT_SIZE)
#else
#define NO_REMAP_TABLE_FOUND            -1
#define SWAP_AREA_NOT_VALID             -2

#define FLASH_MANAGEMENT_BLOCK_SIZE     (8 * 512)                        // 8 x 512 = 4k there are two swap blocks making 8k in total
#define FLASH_SPARE_BLOCK_SIZE          (16 * 512)                       // 16 x 512 = 8k
#define FLASH_FAT_SIZE                  (INTERNAL_FLASH_FAT_SIZE - (FLASH_MANAGEMENT_BLOCK_SIZE * 2) - FLASH_SPARE_BLOCK_SIZE) // the memory area used by the FTAT
#define FLASH_FAT_SECTORS               (FLASH_FAT_SIZE/512)             // number of sectors that are available from the FAT's point of view
#define FLASH_FAT_PHYSICAL_SECTORS      ((FLASH_FAT_SIZE + FLASH_SPARE_BLOCK_SIZE)/512) // number of sectors physically available for mapping FAT sectors to
#define FLASH_PHYSICAL_SECTORS          ((FLASH_FAT_SIZE + FLASH_SPARE_BLOCK_SIZE)/FLASH_GRANULARITY) // number of flash sectors (blocks that can be erased independently) making up the physical sector area
#define FLASH_FAT_START_ADDRESS         (FLASH_FAT_MANAGEMENT_ADDRESS + (2 * FLASH_MANAGEMENT_BLOCK_SIZE))

#define SPI_FLASH_FAT_AREA_SIZE         (SPI_FLASH_SIZE/8)               // part of the SPI flash used as FAT (1MBytes)
#define SPI_FLASH_MANAGEMENT_BLOCK_SIZE (16 * 1024)
#define SPI_FLASH_SPARE_BLOCK_SIZE      (16 * 512)                       // 16 x 512 = 8k
#define SPI_FLASH_FAT_SIZE              (SPI_FLASH_FAT_AREA_SIZE - (SPI_FLASH_MANAGEMENT_BLOCK_SIZE * 2) - SPI_FLASH_SPARE_BLOCK_SIZE) // the memory area used by the FTAT
#define SPI_FLASH_FAT_SECTORS           (SPI_FLASH_FAT_SIZE/512)         // number of sectors that are available from the FAT's point of view
#define SPI_FLASH_FAT_PHYSICAL_SECTORS  ((SPI_FLASH_FAT_SIZE + SPI_FLASH_SPARE_BLOCK_SIZE)/512) // number of sectors physically available for mapping FAT sectors to
#define SPI_FLASH_PHYSICAL_SECTORS      ((SPI_FLASH_FAT_SIZE + SPI_FLASH_SPARE_BLOCK_SIZE)/SPI_FLASH_SECTOR_LENGTH) // number of flash sectors (blocks that can be erased independently) making up the physical sector area
#define SPI_FLASH_FAT_START_ADDRESS     (SPI_FLASH_FAT_MANAGEMENT_ADDRESS + (2 * SPI_FLASH_MANAGEMENT_BLOCK_SIZE))

#define MANAGEMENT_AREA_VALID       0xa5

#define PAGE_ADDRESS  unsigned short                                     // up to 64k

#define CHANGE_TYPE_NEW_MAP_ENTRY   0x01
#define CHANGE_TYPE_NEW_ERASE_COUNT 0x02

#if FLASH_ROW_SIZE != 4 && FLASH_ROW_SIZE != 8
    #error FLASH_ROW_SIZE is expected to be 4 or 8
#endif

#define FILL_SIZE (8 - (2 * sizeof(PAGE_ADDRESS)) - 1)

__PACK_ON                                                                // compilers using pragmas to control packing will start struct packing from here

// This element must be aligned and of a suitable size for cumulative flash writes to be tagged onto each other
//
typedef struct _PACK stPAGE_BLOCK_CHANGE_NOTICE
{
    PAGE_ADDRESS   changedSector;                                        // the FAT sector that has undergone change
    PAGE_ADDRESS   newMapping;                                           // the new physical location of this FAT sector
    unsigned char  ucChangeType;                                         // the type of change involved
    unsigned char  ucFill[FILL_SIZE];
} PAGE_BLOCK_CHANGE_NOTICE;

#define CHANGE_NOTICE_COUNT_MAX ((FLASH_MANAGEMENT_BLOCK_SIZE - (2 * sizeof(PAGE_ADDRESS)) - (FILL_SIZE + 1))/sizeof(PAGE_BLOCK_CHANGE_NOTICE))

typedef struct _PACK stBLOCK_MANAGEMENT_AREA                             // this struct is valid only for internal flash but is used as reference to decsribe the general content
{                                                                        // the ordering is fixed and so must never be changed here
    unsigned char ucBlockStatus[FLASH_ROW_SIZE];                         // first element used to recognise the status of the management area (it must be possible to write this element in a signle Flash write)
    unsigned long ulEraseBaseCount;                                      // base erase program/cycle counter for all flash sectors
    unsigned char blockEraseCount[FLASH_PHYSICAL_SECTORS];               // each physical flash sector has an erase count that is used for wear leveling
    PAGE_ADDRESS  sectorRemap[FLASH_FAT_SECTORS];                        // each FAT sector has a remap sector that it references
    PAGE_BLOCK_CHANGE_NOTICE pageChange[CHANGE_NOTICE_COUNT_MAX];        // a number change notices that are tagged on during operation (this must be on a flash line boundary)
} BLOCK_MANAGEMENT_AREA;
__PACK_OFF


typedef struct stFLASH_MEDIUM
{
    MAX_MALLOC RemapArraySize;
    MAX_MALLOC WearLevelArraySize;
    MAX_MALLOC PhysicalSectorFlagsSize;
    BLOCK_MANAGEMENT_AREA *ptrManagementAddress;
    unsigned char *ptrPhysicalFatArea;
    unsigned long ulFatSectors;
    unsigned long ulManagementBlockSize;
    unsigned long ulMaxPhysicalFatSectors;
    unsigned char ucBlockStatusSize;
    unsigned char ucChangeNoteSize;
} FLASH_MEDIUM;


const FLASH_MEDIUM flashMedium[DISK_COUNT]= {
#if defined FLASH_FAT
    {                                                                    // internal flash
        ((FLASH_FAT_SECTORS) * sizeof(PAGE_ADDRESS)),                    // RemapArraySize to hold the FAT sectors remaping information
        (FLASH_PHYSICAL_SECTORS),                                        // WearLevelArraySize to hold the physical sector erase counts
        ((FLASH_FAT_PHYSICAL_SECTORS + 7)/8),                            // PhysicalSectorFlagsSize to hold physical sector flags
        (BLOCK_MANAGEMENT_AREA *)(FLASH_FAT_MANAGEMENT_ADDRESS),         // location in internal flash
        (unsigned char *)(FLASH_FAT_START_ADDRESS),
        (FLASH_FAT_SECTORS),                                             // the number of sectors available to the FAT
        (FLASH_MANAGEMENT_BLOCK_SIZE),                                   // management block size (there are two used as swap blocks)
        (FLASH_FAT_PHYSICAL_SECTORS),
        (FLASH_ROW_SIZE),                                                // smallest write size that the flash supports
        (sizeof(PAGE_BLOCK_CHANGE_NOTICE)),
    },
#endif
#if defined SDCARD_SUPPORT
    {                                                                    // SD card has no properties that are used
        {0},
    },
#endif
#if defined SPI_FLASH_FAT
    {                                                                    // SPI flash
        ((SPI_FLASH_FAT_SECTORS) * sizeof(PAGE_ADDRESS)),                // RemapArraySize to hold the FAT sectors remaping information
        (SPI_FLASH_PHYSICAL_SECTORS),                                    // WearLevelArraySize to hold the physical sector erase counts
        ((SPI_FLASH_FAT_PHYSICAL_SECTORS + 7)/8),                        // PhysicalSectorFlagsSize to hold physical sector flags
        (BLOCK_MANAGEMENT_AREA *)((FLASH_START_ADDRESS + SIZE_OF_FLASH)),// start of SPI flash (virtual address)
        (unsigned char *)((FLASH_START_ADDRESS + SIZE_OF_FLASH) + (2 * SPI_FLASH_MANAGEMENT_BLOCK_SIZE)),
        (SPI_FLASH_FAT_SECTORS),                                         // the number of sectors available to the FAT
        (SPI_FLASH_MANAGEMENT_BLOCK_SIZE),                               // management block size (there are two used as swap blocks)
        (SPI_FLASH_FAT_PHYSICAL_SECTORS),
        (1),                                                             // smallest write size that the flash supports - byte writes possible
        (sizeof(PAGE_BLOCK_CHANGE_NOTICE)),
    },
#endif
};


static PAGE_ADDRESS  *ptrRemapArray[DISK_COUNT] = {0};
static unsigned char *ptrWearLevelArray[DISK_COUNT] = {0};
static unsigned long ulWearLevelBaseCount[DISK_COUNT] = {0};
static unsigned char *ptrDirtySectors[DISK_COUNT] = {0};
static unsigned char *ptrUsedSectors[DISK_COUNT] = {0};
static int           iSwapBufferValid[DISK_COUNT] = {0};
static PAGE_BLOCK_CHANGE_NOTICE *ptrNextChange[DISK_COUNT] = {0};

static void fnCleanManagement(BLOCK_MANAGEMENT_AREA *ptrManagementArea, unsigned long ulManagementBlockSize)
{
    unsigned long ulTemp[512/sizeof(unsigned long)];
    unsigned long ulBufferLength = sizeof(ulTemp);
    unsigned char *ptrTest = (unsigned char *)ptrManagementArea;
    int i;
    while (ulManagementBlockSize != 0) {
        if (ulBufferLength > ulManagementBlockSize) {
            ulBufferLength = ulManagementBlockSize;
        }
        fnGetParsFile(ptrTest, (unsigned char *)ulTemp, ulBufferLength);
        for (i = 0; i < (sizeof(ulTemp)/sizeof(unsigned long)); i++) {
            if (ulTemp[i] != 0xffffffff) {
                fnEraseFlashSector((unsigned char *)ptrManagementArea, ulManagementBlockSize); // if any content is not erased the complete managemenet block is deleted
                return;                                                  // cleaning completed after an erasure
            }
        }
        ptrTest += ulBufferLength;
        ulManagementBlockSize -= ulBufferLength;
    }
}

// If the swap area is valid it takes presidence over the remapping area. The remapping area is deleted, if not already blank.
// The swap area will be used until the next block exchange is required, after which it will be erased.
//
static int fnCheckSwapArea(int iDiskNumber)
{
    int iPageIndex;
    unsigned long i;
    unsigned long ulPhysicalSectorNumber;
    PAGE_BLOCK_CHANGE_NOTICE changeNotice;
    BLOCK_MANAGEMENT_AREA *ptrManagementArea = flashMedium[iDiskNumber].ptrManagementAddress; // first management area
    unsigned char *ptrManagementContent;
    unsigned char *ptrManagementContentEnd;
    unsigned char ucStatus;
    unsigned char ucPageMask;
    fnGetParsFile(ptrManagementArea->ucBlockStatus, &ucStatus, 1);       // read the status of the first management area block (always first byte in the management area)
    if (ucStatus != MANAGEMENT_AREA_VALID) {                             // if the first swap block is not valid we check the second
        fnCleanManagement(ptrManagementArea, flashMedium[iDiskNumber].ulManagementBlockSize); // ensure that the content of this non-valid management block is deleted to avoid possible problems when it is used in the future
        ptrManagementArea = (BLOCK_MANAGEMENT_AREA *)((unsigned char *)ptrManagementArea + flashMedium[iDiskNumber].ulManagementBlockSize); // try the next
        fnGetParsFile(ptrManagementArea->ucBlockStatus, &ucStatus, 1);
        if (ucStatus != MANAGEMENT_AREA_VALID) {
            fnCleanManagement(ptrManagementArea, flashMedium[iDiskNumber].ulManagementBlockSize); // ensure that the content of this non-valid management block is deleted to avoid possible problems when it is used in the future
            return SWAP_AREA_NOT_VALID;                                  // no valid management area exists
        }
        iSwapBufferValid[iDiskNumber] = 1;                               // flag that the swap buffer is the present location of the management block
    }
    else {
        fnCleanManagement((BLOCK_MANAGEMENT_AREA *)((unsigned char *)ptrManagementArea + flashMedium[iDiskNumber].ulManagementBlockSize), flashMedium[iDiskNumber].ulManagementBlockSize); // ensure that the content of the alternative management block is deleted to avoid possible problems when it is used in the future
    }
    ptrManagementContent = (unsigned char *)ptrManagementArea;           // start of the valid management block
    ptrManagementContentEnd = ptrManagementContent + flashMedium[iDiskNumber].ulManagementBlockSize; // end of the valid management block
    // There is valid management information available so load the basic settings and then update them with any available change notices
    //
    ptrManagementContent += flashMedium[iDiskNumber].ucBlockStatusSize;  // move pointer to base erase count
    fnGetParsFile(ptrManagementContent, (unsigned char *)&ulWearLevelBaseCount[iDiskNumber], sizeof(unsigned long));
    ptrManagementContent += sizeof(unsigned long);                       // move pointer to block erase count area
    fnGetParsFile(ptrManagementContent, (unsigned char *)ptrWearLevelArray[iDiskNumber], flashMedium[iDiskNumber].WearLevelArraySize); // load the initial wear-level couters
    ptrManagementContent += flashMedium[iDiskNumber].WearLevelArraySize; // move the pointer to the remap area
    fnGetParsFile(ptrManagementContent, (unsigned char *)ptrRemapArray[iDiskNumber], flashMedium[iDiskNumber].RemapArraySize); // load the initial remapping entries
    ptrManagementContent += flashMedium[iDiskNumber].RemapArraySize;     // move to the change list area
    for (i = 0; i < flashMedium[iDiskNumber].ulFatSectors; i++) {        // for each sector from the point of view of the FAT
        ulPhysicalSectorNumber = ptrRemapArray[iDiskNumber][i];          // its physical remapped location
        if (ulPhysicalSectorNumber != (PAGE_ADDRESS)(0 - 1)) {           // if the FAT sector is mapped
            iPageIndex = (ulPhysicalSectorNumber/8);                     // calculate its page index
            ucPageMask = (unsigned char)(1 << (ulPhysicalSectorNumber%8)); // calculate its page mask
            ptrUsedSectors[iDiskNumber][iPageIndex] |= ucPageMask;       // mark that the physical FAT sector is in use
        }
    }
    while (1) {                                                          // now scan through the list of change notices
        fnGetParsFile(ptrManagementContent, (unsigned char *)&changeNotice, sizeof(changeNotice)); // read the next change notice
        if (changeNotice.ucChangeType == CHANGE_TYPE_NEW_MAP_ENTRY) {
            ulPhysicalSectorNumber = ptrRemapArray[iDiskNumber][changeNotice.changedSector];
            if (ulPhysicalSectorNumber != (PAGE_ADDRESS)(0 - 1)) {
                iPageIndex = (ulPhysicalSectorNumber/8);
                ucPageMask = (unsigned char)(1 << (ulPhysicalSectorNumber%8));
                ptrUsedSectors[iDiskNumber][iPageIndex] &= ~ucPageMask;
            }
            ptrRemapArray[iDiskNumber][changeNotice.changedSector] = changeNotice.newMapping;
            ulPhysicalSectorNumber = ptrRemapArray[iDiskNumber][changeNotice.changedSector];
            if (ulPhysicalSectorNumber != (PAGE_ADDRESS)(0 - 1)) {
                iPageIndex = (ulPhysicalSectorNumber/8);
                ucPageMask = (unsigned char)(1 << (ulPhysicalSectorNumber%8));
                ptrUsedSectors[iDiskNumber][iPageIndex] |= ucPageMask;
            }
        }
        else if (changeNotice.ucChangeType == CHANGE_TYPE_NEW_ERASE_COUNT) {
        }
        else {                                                           // end of change notices reached
            break;
        }
        ptrManagementContent += flashMedium[iDiskNumber].ucChangeNoteSize;
        // TO DO.. when close to the end of the block do a consolidation
        if (ptrManagementContent >= ptrManagementContentEnd) {           // end of the management area has been reached (it is completely full)
            break;
        }
    }
    ptrNextChange[iDiskNumber] = (PAGE_BLOCK_CHANGE_NOTICE *)ptrManagementContent; // the location for posting further changes notices to
    return UTFAT_SUCCESS;
}

// Check all sectors in the physical FAT area and mark the ones that are blank so that they can be found quickly when needed
//
static void fnGetBlankBlocks(int iDiskNumber)
{
    unsigned long *ptrFlash = (unsigned long *)(flashMedium[iDiskNumber].ptrPhysicalFatArea);
    unsigned long ulBuffer[512/sizeof(unsigned long)];
    unsigned long ulDirtyFATsectorsCount = 0;
    PAGE_ADDRESS page;
    int i;
    unsigned char ucEmptyPageBit = 0x01;
    for (page = 0; page < flashMedium[iDiskNumber].ulMaxPhysicalFatSectors; page++) { // for each physical FAT sector
        i = 0; 
        if ((ptrUsedSectors[iDiskNumber][page/8] & ucEmptyPageBit) == 0) { // used physical FAT sectors are considered to be dirty and not checked further
            fnGetParsFile((unsigned char *)ptrFlash, (unsigned char *)ulBuffer, 512);
            for ( ; i < (512/sizeof(unsigned long)); i++) {              // check each long word in the page
                if (0xffffffff != ulBuffer[i]) {                         // if a location is not blank
                    break;
                }
            }
        }
        if (i != (512/sizeof(unsigned long))) {
            ptrDirtySectors[iDiskNumber][page/8] |= ucEmptyPageBit;      // mark that this is a dirty physical FAT sector
            ulDirtyFATsectorsCount++;
        }
        ptrFlash += (512/sizeof(unsigned long));                         // next sector to be checked
        ucEmptyPageBit <<= 1;
        if (ucEmptyPageBit == 0) {
            ucEmptyPageBit = 0x01;
        }
    }
}


// This is used when the remap information is stored in the swap block.
// It builds a new initial map in the main remap area and deletes the swap block - the main remap area will be clean
//
static int fnActivateRemapArea(int iDiskNumber, int iSwap)
{
    int i = flashMedium[iDiskNumber].ucBlockStatusSize;
    unsigned char ucBlockStatus = MANAGEMENT_AREA_VALID;
    BLOCK_MANAGEMENT_AREA *ptrManagementArea = flashMedium[iDiskNumber].ptrManagementAddress; // first management area address
    unsigned char *ptrManagementContent = (unsigned char *)ptrManagementArea;
    if (iSwap != 0) {
        iSwapBufferValid[iDiskNumber] ^= 1;                              // swap to alternative management block
        if (iSwapBufferValid[iDiskNumber] != 0) {
            ptrManagementContent += flashMedium[iDiskNumber].ulManagementBlockSize; // set the address to the second management block
            ptrManagementArea = (BLOCK_MANAGEMENT_AREA *)ptrManagementContent;
        }
    }
    ptrManagementContent += flashMedium[iDiskNumber].ucBlockStatusSize;  // move pointer to block erase base count value
    fnWriteBytesFlash(ptrManagementContent, (unsigned char *)&ulWearLevelBaseCount[iDiskNumber], sizeof(unsigned long)); // add the base count value
    ptrManagementContent += sizeof(unsigned long);                       // move pointer to block erase count area
    fnWriteBytesFlash(ptrManagementContent, (unsigned char *)ptrWearLevelArray[iDiskNumber], flashMedium[iDiskNumber].WearLevelArraySize); // reset the wear level counters
    ptrManagementContent += flashMedium[iDiskNumber].WearLevelArraySize; // move the pointer to the remap area
    fnWriteBytesFlash(ptrManagementContent, (unsigned char *)ptrRemapArray[iDiskNumber], flashMedium[iDiskNumber].RemapArraySize); // write the initial mapping table to flash
    ptrManagementContent = (unsigned char *)&ptrManagementArea->ucBlockStatus; // set the pointer back to the status
    while (i--) {                                                        // flash types which don't support single byte writes are written with the defined status size (the minimum write size required)
        fnWriteBytesFlash(ptrManagementContent++, &ucBlockStatus, 1);    // finally validate the management area
    }
    if (iSwap != 0) {                                                    // when swapping we delete the original management block
        ptrManagementContent = (unsigned char *)flashMedium[iDiskNumber].ptrManagementAddress;
        if (iSwapBufferValid[iDiskNumber] == 0) {                        // the second management block is now valid
            ptrManagementContent += flashMedium[iDiskNumber].ulManagementBlockSize; // set the address accordingly
        }
        fnEraseFlashSector(ptrManagementContent, flashMedium[iDiskNumber].ulManagementBlockSize); // erase the original management block area
        ptrManagementContent += flashMedium[iDiskNumber].ucBlockStatusSize; // move pointer to block erase count area
    }
    ptrManagementContent += sizeof(unsigned long);                       // move pointer to block erase count area
    ptrManagementContent += flashMedium[iDiskNumber].WearLevelArraySize; // move the pointer to the remap area
    ptrManagementContent += flashMedium[iDiskNumber].RemapArraySize;     // move to the change list area
    ptrNextChange[iDiskNumber] = (PAGE_BLOCK_CHANGE_NOTICE *)ptrManagementContent; // the location for posting further changes notices to
    return UTFAT_SUCCESS;
}

// This routine is called at reset in order to construct page mapping/wear-level and other management tables
//
static int fnInitialiseBlockMap(int iDiskNumber, int iCreate)
{
    if (iCreate != 0) {                                                  // create a default address map table
      //fnEraseFlashSector((unsigned char *)flashMedium[iDiskNumber].ptrManagementAddress, (flashMedium[iDiskNumber].ulManagementBlockSize * 2)); // clean out the remapping area ready for first use
        uMemset(ptrRemapArray[iDiskNumber], 0xff, flashMedium[iDiskNumber].RemapArraySize); // initially no pages are mapped nor do they belong to the user
        return (fnActivateRemapArea(iDiskNumber, 0));                    // create initial entry
    }

    if (ptrRemapArray[iDiskNumber] == 0) {                               // on first call
        ptrRemapArray[iDiskNumber] = FLASH_FAT_MALLOC(flashMedium[iDiskNumber].RemapArraySize); // get memory to perform mapping between user and physical pages
        ptrWearLevelArray[iDiskNumber] = FLASH_FAT_MALLOC(flashMedium[iDiskNumber].WearLevelArraySize); // get zeroed memory for RAM copy of wear-level table
        ptrDirtySectors[iDiskNumber] = FLASH_FAT_MALLOC(flashMedium[iDiskNumber].PhysicalSectorFlagsSize); // get zeroed memory for a table of presently dirty FAT sectors in physical flash
        ptrUsedSectors[iDiskNumber] = FLASH_FAT_MALLOC(flashMedium[iDiskNumber].PhysicalSectorFlagsSize); // get zeroed memory for a table of FAT sectors in physical flash mapped to the user
    }
    if (fnCheckSwapArea(iDiskNumber) == UTFAT_SUCCESS) {
        return UTFAT_SUCCESS;                                            // OK
    }
    else {
        return NO_REMAP_TABLE_FOUND;
    }
}
#endif

static int utReadPartialFlash(UTDISK *ptr_utDisk, unsigned long ulSectorNumber, void *ptrBuf, unsigned short usOffset, unsigned short usLength)
{
#if !defined SIMPLE_FLASH
    int iDiskNumber = ptr_utDisk->ucDriveNumber;
    ulSectorNumber = ptrRemapArray[iDiskNumber][ulSectorNumber];         // get the mapped physical FAT sector
    if (ulSectorNumber == (PAGE_ADDRESS)(0 - 1)) {                       // physical sectors that are not allocated to the FAT return all zeros
        uMemset(ptrBuf, 0, usLength);
    }
    else {
        ulSectorNumber *= 512;                                           // convert the sector number to byte address
        ulSectorNumber += (unsigned long)(flashMedium[iDiskNumber].ptrPhysicalFatArea); // map into physical FAT area in flash
        ulSectorNumber += usOffset;                                      // offset into the sector
        fnGetParsFile((unsigned char *)ulSectorNumber, ptrBuf, usLength);// read a partial sector from the flash
    }
#else
    ulSectorNumber *= 512;                                               // convert the sector number to byte address
    ulSectorNumber += (FLASH_FAT_START_ADDRESS);                         // map into FAT area in internal flash
    ulSectorNumber += usOffset;
    fnGetParsFile((unsigned char *)ulSectorNumber, ptrBuf, usLength);    // read a partial sector from the flash
#endif
    return UTFAT_SUCCESS;
}

// Read a single, complete sector from the disk to the specified buffer
//
static int utReadFlashSector(UTDISK *ptr_utDisk, unsigned long ulSectorNumber, void *ptrBuf)
{
#if !defined SIMPLE_FLASH
    int iDiskNumber = ptr_utDisk->ucDriveNumber;
    ulSectorNumber = ptrRemapArray[iDiskNumber][ulSectorNumber];         // get the mapped physical FAT sector
    if (ulSectorNumber == (PAGE_ADDRESS)(0 - 1)) {                       // physical sectors that are not allocated to the FAT return all zeros
        uMemset(ptrBuf, 0, 512);
    }
    else {
        ulSectorNumber *= 512;                                           // convert the sector number to byte address
        ulSectorNumber += (unsigned long)(flashMedium[iDiskNumber].ptrPhysicalFatArea); // map into physical FAT area in flash
        fnGetParsFile((unsigned char *)ulSectorNumber, ptrBuf, 512);     // read a sector from the flash
    }
#else
    ulSectorNumber *= 512;                                               // convert the sector number to byte address
    ulSectorNumber += (FLASH_FAT_START_ADDRESS);                         // map into FAT area in internal flash
    fnGetParsFile((unsigned char *)ulSectorNumber, ptrBuf, 512);         // read a sector from the flash
#endif
    return UTFAT_SUCCESS;
}

    #if defined UTFAT_WRITE
        #if !defined SIMPLE_FLASH
static unsigned long fnRemap(int iDiskNumber, unsigned long ulSectorNumber, int iPhysicalPageIndex, unsigned char ucPhysicalPageMask)
{
    static unsigned char preferenceWear = 80;
    unsigned long ulNewPage = 0;
    unsigned long ulSector;
    int iPageIndex = 0;
    int iPreferredPageIndex = 0;
    unsigned char lowestWear = (unsigned char)(0 - 1);
    unsigned char ucPageMask = 0x01;
    unsigned char ucPreferredPageMask = 0;
    for (ulSector = 0; ulSector < flashMedium[iDiskNumber].ulMaxPhysicalFatSectors; ulSector++) { // get the oldest blank page
        if (((ptrDirtySectors[iDiskNumber][iPageIndex] | ptrUsedSectors[iDiskNumber][iPageIndex]) & ucPageMask) == 0) { // find a physical sector that is not already mapped to the FAT
            if (ptrWearLevelArray[iDiskNumber][iPageIndex] <= preferenceWear) { // fresh page can be used with priority
                lowestWear = preferenceWear;
                ulNewPage = ulSector;
                iPreferredPageIndex = iPageIndex;
                ucPreferredPageMask = ucPageMask;
                break;
            }
            else if (ptrWearLevelArray[iDiskNumber][iPageIndex] < lowestWear) {
                ulNewPage = ulSector;
                lowestWear = ptrWearLevelArray[iDiskNumber][iPageIndex];
                iPreferredPageIndex = iPageIndex;
                ucPreferredPageMask = ucPageMask;
            }
        }
        ucPageMask <<= 1;
        if (ucPageMask == 0) {
            ucPageMask = 1;
            iPageIndex++;
        }
    }
    if (lowestWear == (unsigned char)(0 - 1)) {
        _EXCEPTION("TO DO - no page found!!!");                          // this should never occur because we will clear up beforehand and when getting critical
    }
    preferenceWear = lowestWear;
    ptrRemapArray[iDiskNumber][ulSectorNumber] = (PAGE_ADDRESS)ulNewPage;
    ptrUsedSectors[iDiskNumber][iPhysicalPageIndex] &= ~(ucPhysicalPageMask); // the original physical sector is no longer locked to the FAT
    ptrDirtySectors[iDiskNumber][iPreferredPageIndex] |= ucPreferredPageMask; // most probably it is now dirty
    ptrUsedSectors[iDiskNumber][iPreferredPageIndex] |= ucPreferredPageMask; // this physical sector now belongs to the FAT (no longer free for allocating user pages to)
    return ulNewPage;
}

static void fnReportPageChange(int iDiskNumber, unsigned long ulSectorNumber, unsigned long ulNewSectorNumber)
{
    PAGE_BLOCK_CHANGE_NOTICE pageChange;
    if (iSwapBufferValid[iDiskNumber] != 0) {
        if ((ptrNextChange[iDiskNumber] + 1) > (PAGE_BLOCK_CHANGE_NOTICE *)((unsigned char *)(flashMedium[iDiskNumber].ptrManagementAddress) + (2 * flashMedium[iDiskNumber].ulManagementBlockSize))) { // presently the swap block is being used
            fnActivateRemapArea(iDiskNumber, 1);                         // there is not enough space to add the change notice so we perform a swap
        }
    }
    else {                                                               // presently the first management block is being used
        if ((ptrNextChange[iDiskNumber] + 1) > (PAGE_BLOCK_CHANGE_NOTICE *)((unsigned char *)(flashMedium[iDiskNumber].ptrManagementAddress) + (flashMedium[iDiskNumber].ulManagementBlockSize))) {
            fnActivateRemapArea(iDiskNumber, 1);                         // there is not enough space to add the change notice so we perform a swap
        }
    }
    pageChange.ucChangeType = CHANGE_TYPE_NEW_MAP_ENTRY;                 // the reason for the change notice
    pageChange.changedSector = (PAGE_ADDRESS)ulSectorNumber;             // the sector number in the FAT
    pageChange.newMapping = (PAGE_ADDRESS)ulNewSectorNumber;             // the physical sector number that it is mapped to
    fnWriteBytesFlash((unsigned char *)ptrNextChange[iDiskNumber], (unsigned char *)&pageChange, sizeof(PAGE_BLOCK_CHANGE_NOTICE));
    ptrNextChange[iDiskNumber]++;                                        // move to next change notification ready for next use
}
        #endif
static int utCommitFlashSectorData(UTDISK *ptr_utDisk, void *ptrBuffer, unsigned long ulSectorNumber)
{
    unsigned long ulPhysicalSectorNumber;
    #if !defined SIMPLE_FLASH
    int iDiskNumber = ptr_utDisk->ucDriveNumber;
    unsigned long ulNewSectorNumber = ulSectorNumber;
    int iPageIndex;
    unsigned char ucPageMask;
    ulPhysicalSectorNumber = ptrRemapArray[iDiskNumber][ulSectorNumber]; // get the mapped physical sector
    if (ulPhysicalSectorNumber == (unsigned char)(0 - 1)) {              // if this sector doesn't yet belong to the user we can't write to it directly
        ulPhysicalSectorNumber = ulNewSectorNumber = fnRemap(iDiskNumber, ulSectorNumber, 0, 0); // get a first sector
    }
    else {
        iPageIndex = (ulPhysicalSectorNumber/8);
        ucPageMask = (unsigned char)(1 << (ulPhysicalSectorNumber%8));
        ulPhysicalSectorNumber = ulNewSectorNumber = fnRemap(iDiskNumber, ulSectorNumber, iPageIndex, ucPageMask); // get a new page
    }
    ulPhysicalSectorNumber *= 512;                                       // convert the sector number to byte address
    ulPhysicalSectorNumber += (unsigned long)(flashMedium[iDiskNumber].ptrPhysicalFatArea); // map into FAT area in internal flash
    fnWriteBytesFlash((unsigned char *)ulPhysicalSectorNumber, (unsigned char *)ptrBuffer, 512); // write a sector to the flash
    fnReportPageChange(ptr_utDisk->ucDriveNumber, ulSectorNumber, ulNewSectorNumber); // now write a change notice due to the fact that this written sectors belongs to the FAT
    #else
    int iOffset;
    unsigned char ucTempBuffer[FLASH_GRANULARITY];
    unsigned char *ptrFlashSector;
    ulPhysicalSectorNumber = (ulSectorNumber * 512);                     // convert the sector number to byte address
    ulPhysicalSectorNumber += (FLASH_FAT_START_ADDRESS);                 // map into FAT area in internal flash
    ptrFlashSector = (unsigned char *)((CAST_POINTER_ARITHMETIC)ulPhysicalSectorNumber & ~(FLASH_GRANULARITY - 1)); // the flash sector that this sector belongs to
    iOffset = (ulPhysicalSectorNumber - (unsigned long)ptrFlashSector);
    fnGetParsFile(ptrFlashSector, ucTempBuffer, FLASH_GRANULARITY);      // read a complete flash sector from the flash
    if (uMemcmp(&ucTempBuffer[iOffset], ptrBuffer, 512) != 0) {          // if there is a change
        uMemcpy(&ucTempBuffer[iOffset], ptrBuffer, 512);                 // overwrite the relevant disk sector
        fnEraseFlashSector(ptrFlashSector, 0);                           // delete the flash sector before writing the complete new content
        fnWriteBytesFlash((unsigned char *)ptrFlashSector, ucTempBuffer, FLASH_GRANULARITY); // write a sector to the flash
    }
    #endif
    return UTFAT_SUCCESS;
}

static int utDeleteFlashSector(UTDISK *ptr_utDisk, unsigned long ulSectorNumber)
{
    unsigned long ulZeroBuffer[512/sizeof(unsigned long)];
    uMemset(ulZeroBuffer, 0, sizeof(ulZeroBuffer));
    return (utCommitFlashSectorData(ptr_utDisk, ulZeroBuffer, ulSectorNumber));
}
    #endif
#endif


#if SPI_FLASH_PAGE_LENGTH == 256
    #define SPI_FLASH_FAT_SECTOR_SIZE     512
#else
    #define SPI_FLASH_FAT_SECTOR_SIZE     SPI_FLASH_PAGE_LENGTH          // 512 or 528
#endif

#if defined SPI_FLASH_FAT
static int utReadPartialSPI(UTDISK *ptr_utDisk, unsigned long ulSectorNumber, void *ptrBuf, unsigned short usOffset, unsigned short usLength)
{
    if (ulSectorNumber >= ptr_utDisk->ulSD_sectors) {
        return UTFAT_DISK_READ_ERROR;
    }
    ulSectorNumber *= SPI_FLASH_FAT_SECTOR_SIZE;                         // convert the sector number to byte address
    ulSectorNumber += (FLASH_START_ADDRESS + SIZE_OF_FLASH);             // map into SPI flash
    ulSectorNumber += usOffset;
    fnGetParsFile((unsigned char *)ulSectorNumber, ptrBuf, usLength);    // read a sector from the SPI flash
    return UTFAT_SUCCESS;
}

// Read a single, complete sector from the disk to the specified buffer
//
static int utReadSPISector(UTDISK *ptr_utDisk, unsigned long ulSectorNumber, void *ptrBuf)
{
    if (ulSectorNumber >= ptr_utDisk->ulSD_sectors) {
        return UTFAT_DISK_READ_ERROR;
    }
    ulSectorNumber *= SPI_FLASH_FAT_SECTOR_SIZE;                         // convert the sector number to byte address
    ulSectorNumber += (FLASH_START_ADDRESS + SIZE_OF_FLASH);             // map into SPI flash
    fnGetParsFile((unsigned char *)ulSectorNumber, ptrBuf, 512);         // read a sector from the SPI flash
    return UTFAT_SUCCESS;
}

    #if defined UTFAT_WRITE
static int utCommitSPISectorData(UTDISK *ptr_utDisk, void *ptrBuffer, unsigned long ulSectorNumber)
{
    if (ulSectorNumber >= ptr_utDisk->ulSD_sectors) {
        return UTFAT_DISK_WRITE_ERROR;
    }
    ulSectorNumber *= SPI_FLASH_FAT_SECTOR_SIZE;                         // convert the sector number to byte address
    ulSectorNumber += (FLASH_START_ADDRESS + SIZE_OF_FLASH);             // map into SPI flash
    fnEraseFlashSector((unsigned char *)ulSectorNumber, 512);            // pre-erase (could be done with integrated command!)
    fnWriteBytesFlash((unsigned char *)ulSectorNumber, ptrBuffer, 512);  // write a sector to the SPI flash
    return UTFAT_SUCCESS;
}

static int utDeleteSPISector(UTDISK *ptr_utDisk, unsigned long ulSectorNumber)
{
    unsigned long ulZeroBuffer[512/sizeof(unsigned long)];
    uMemset(ulZeroBuffer, 0, sizeof(ulZeroBuffer));
    return (utCommitSPISectorData(ptr_utDisk, ulZeroBuffer, ulSectorNumber));
}
    #endif
#endif

#if defined SDCARD_SUPPORT || defined SPI_FLASH_FAT || defined FLASH_FAT || defined USB_MSD_HOST
    #if defined UTFAT_WRITE
        static int (*_utCommitSectorData[DISK_COUNT])(UTDISK *ptr_utDisk, void *ptrBuffer, unsigned long ulSectorNumber) = {0};
        static int (*_utDeleteSector[DISK_COUNT])(UTDISK *ptr_utDisk, unsigned long ulSectorNumber) = {0};
    #endif
    static int (*_utReadDiskSector[DISK_COUNT])(UTDISK *ptr_utDisk, unsigned long ulSectorNumber, void *ptrBuf) = {0};
    static int (*_utReadPartialDiskData[DISK_COUNT])(UTDISK *ptr_utDisk, unsigned long ulSector, void *ptrBuf, unsigned short usOffset, unsigned short usLength) = {0};
    #if defined FAT_EMULATION                                            // {12}
        static int fnReadEmulatedSector(UTDISK *ptr_utDisk, unsigned long ulSectorNumber, void *ptrBuf);
        static int fnReadPartialEmulatedSector(UTDISK *ptr_utDisk, unsigned long ulSector, void *ptrBuf, unsigned short usOffset, unsigned short usLength);
    #endif
#endif

#if defined SDCARD_SUPPORT && !defined SD_CONTROLLER_AVAILABLE
static int fnCheckCSD(unsigned char ucData[18])
{
  //unsigned char test[] = {0xff, 0xfe, 0x40, 0x0e, 0x00, 0x32, 0x5b, 0x59, 0x00, 0x00, 0x1d, 0x8a, 0x7f, 0x80, 0x0a, 0x40, 0x40, 0xb9}; // reference reception for test purposes
    unsigned char ucCRC = 0;
    int iOffset;
    int i, j;
    unsigned char ucByte;

  //uMemcpy(ucData, test, 18);                                           // these can be used to test a reference case
  //uMemcpy(ucData, &test[1], 17);

    if (ucData[0] == 0xfe) {                                             // if a synchronisation is found at the start of the buffer we assume that the SD card returned the CSD content one byte earlier than normal
        iOffset = 1;
    }
    else {
        iOffset = 2;                                                     // normal location for the content start
    }
    for (i = 0; i < 15; i++) {                                           // check the CRC-7 of the content
        ucByte = ucData[iOffset++];
        for (j = 0; j < 8; j++) {                                        // for each bit of the byte
            ucCRC <<= 1;
            if ((ucByte & 0x80) ^ (ucCRC & 0x80)) {
                ucCRC ^=0x09;
            }
            ucByte <<= 1;
        }
    }
    if ((ucCRC & 0x7f) == (ucData[iOffset] >> 1)) {                      // check that the received CRC-7 matches with the value calculated for the contnet
        if (iOffset == 16) {
            uReverseMemcpy(&ucData[2], &ucData[1], 16);                  // shift the content to the expected location in case it was originally shifted
        }
        return 0;                                                        // CRC-7 is correct and the data is in the correct location
    }
    else {
        return -1;                                                       // invalid CRC-7
    }
}
#endif

#if ((defined SDCARD_SUPPORT || defined SPI_FLASH_FAT || defined FLASH_FAT || defined USB_MSD_HOST) && (defined UTFAT_WRITE && defined UTFAT_FORMATTING)) || defined FAT_EMULATION
#if !defined SDCARD_SUPPORT && !defined SPI_FLASH_FAT && !defined FLASH_FAT && !defined USB_MSD_HOST
    static UTDISK utDisks[EMULATED_FAT_LUMS] = {{0}};
#endif
static unsigned long ulFAT32size[EMULATED_FAT_LUMS] = {0};

static void fnCreateExtendedBootRecord(int iDiskNumber)
{
    EXTENDED_BOOT_RECORD *ptrExtendedBootRecord = (EXTENDED_BOOT_RECORD *)utDisks[iDiskNumber].ptrSectorData;
    unsigned long ulPartitionSize = (utDisks[iDiskNumber].ulSD_sectors - BOOT_SECTOR_LOCATION);
    uMemset(ptrExtendedBootRecord, 0, 512);                              // ensure content is blank
    ptrExtendedBootRecord->EBR_partition_table[0].starting_cylinder = 2; // fixed values
    ptrExtendedBootRecord->EBR_partition_table[0].starting_head     = 0x0c;
    ptrExtendedBootRecord->EBR_partition_table[0].partition_type    = 0x0b;
    ptrExtendedBootRecord->EBR_partition_table[0].ending_cylinder   = 0x38;
    ptrExtendedBootRecord->EBR_partition_table[0].ending_head       = 0xf8;
    ptrExtendedBootRecord->EBR_partition_table[0].ending_sector     = 0xb8;
    ptrExtendedBootRecord->EBR_partition_table[0].start_sector[0]   = BOOT_SECTOR_LOCATION;
    ptrExtendedBootRecord->EBR_partition_table[0].partition_size[0] = (unsigned char)(ulPartitionSize);
    ptrExtendedBootRecord->EBR_partition_table[0].partition_size[1] = (unsigned char)(ulPartitionSize >> 8);
    ptrExtendedBootRecord->EBR_partition_table[0].partition_size[2] = (unsigned char)(ulPartitionSize >> 16);
    ptrExtendedBootRecord->EBR_partition_table[0].partition_size[3] = (unsigned char)(ulPartitionSize >> 24);
    ptrExtendedBootRecord->ucCheck55 = 0x55;
    ptrExtendedBootRecord->ucCheckAA = 0xaa;
}

static void fnCreateBootSector(int iDiskNumber)
{
    BOOT_SECTOR_FAT32 *ptrBootSector = (BOOT_SECTOR_FAT32 *)utDisks[iDiskNumber].ptrSectorData;
    #if defined UTFAT16 || defined UTFAT12
    BOOT_SECTOR_FAT12_FAT16 *ptrBootSector_16 = (BOOT_SECTOR_FAT12_FAT16 *)utDisks[iDiskNumber].ptrSectorData;
    #endif
    BOOT_SECT_COM *ptr_common;
    unsigned long ulPartitionSize;
    unsigned char ucFATs = NUMBER_OF_FATS;
    #if defined FAT_EMULATION                                            // {12}
    if ((utDisks[iDiskNumber].usDiskFlags & DISK_FAT_EMULATION) != 0) {
        ucFATs = 1;                                                      // emulated FAT uses only one FAT
    }
    #endif
    uMemset(ptrBootSector, 0, 508);                                      // ensure content is blank (without overwriting 0xaa550000)
    ptrBootSector->boot_sector_bpb.BS_jmpBoot[0]     = 0xeb;             // fixed values
    ptrBootSector->boot_sector_bpb.BS_jmpBoot[1]     = 0x58;
    ptrBootSector->boot_sector_bpb.BS_jmpBoot[2]     = 0x90;
    uStrcpy(ptrBootSector->boot_sector_bpb.BS_OEMName, "MSDOS5.0");
    utDisks[iDiskNumber].utFAT.usBytesPerSector      = BYTES_PER_SECTOR;
    ptrBootSector->boot_sector_bpb.BPB_BytesPerSec[0]= (unsigned char)(BYTES_PER_SECTOR);
    ptrBootSector->boot_sector_bpb.BPB_BytesPerSec[1]= (unsigned char)(BYTES_PER_SECTOR >> 8);
    ptrBootSector->boot_sector_bpb.BPB_SecPerTrk[0]  = 63;
    ptrBootSector->boot_sector_bpb.BPB_NumHeads[0]   = 0xff;                
    ptrBootSector->boot_sector_bpb.BPB_Media         = FIXED_MEDIA;                
    #if defined UTFAT16 || defined UTFAT12
    if (utDisks[iDiskNumber].usDiskFlags & (DISK_FORMAT_FAT12 | DISK_FORMAT_FAT16)) {
        unsigned long ulSectorCnt = (unsigned long)(utDisks[iDiskNumber].ulSD_sectors); // total count of sectors in volume
        ptrBootSector->boot_sector_bpb.BS_jmpBoot[1] = 0x3c;
        ptrBootSector->boot_sector_bpb.BPB_NumFATs   = 1;
        ptrBootSector->boot_sector_bpb.BPB_RootEntCnt[1] = (unsigned char)(512 >> 8);
        ptrBootSector->boot_sector_bpb.BPB_TotSec16[0]   = (unsigned char)(ulSectorCnt);
        ptrBootSector->boot_sector_bpb.BPB_TotSec16[1]   = (unsigned char)(ulSectorCnt >> 8);
        if (utDisks[iDiskNumber].usDiskFlags & (DISK_FORMAT_FAT12)) {
        #if defined UTFAT12
            ulSectorCnt = ((utDisks[iDiskNumber].ulSD_sectors - 8)/341); // size of FAT12 in sectors
            ptrBootSector->boot_sector_bpb.BPB_RsvdSecCnt[0] = 6;
        #endif
        }
        #if defined UTFAT12
        else {
            ulSectorCnt = ((utDisks[iDiskNumber].ulSD_sectors - 8)/257); // size of FAT16 in sectors
            ptrBootSector->boot_sector_bpb.BPB_RsvdSecCnt[0] = 8;
        }
        #endif
        if (ulSectorCnt > 0xfe) {
            ulSectorCnt = (0xfe + 8 + ((0xfe * 512)/sizeof(unsigned short)));
            ptrBootSector->boot_sector_bpb.BPB_TotSec16[0]   = (unsigned char)(ulSectorCnt);
            ptrBootSector->boot_sector_bpb.BPB_TotSec16[1]   = (unsigned char)(ulSectorCnt >> 8);
            ulSectorCnt = 0xfe;                                          // limit size of FAT16
        }
        #if defined UTFAT12
        if ((utDisks[iDiskNumber].usDiskFlags & (DISK_FORMAT_FAT12)) && (ulSectorCnt > 12)) {
            ulSectorCnt = (12 + 8 + ((12 * 512)/3));
            ptrBootSector->boot_sector_bpb.BPB_TotSec16[0]   = (unsigned char)(ulSectorCnt);
            ptrBootSector->boot_sector_bpb.BPB_TotSec16[1]   = (unsigned char)(ulSectorCnt >> 8);
            ulSectorCnt = 12;
        }
        #endif
        if (ulSectorCnt == 0) {
            ulSectorCnt = 1;
        }
        ptrBootSector->boot_sector_bpb.BPB_FATSz16[0]    = (unsigned char)(ulSectorCnt);
        ptrBootSector->boot_sector_bpb.BPB_FATSz16[1]    = (unsigned char)(ulSectorCnt >> 8);
        utDisks[iDiskNumber].utFAT.ucSectorsPerCluster = 1;              // one sector per cluster assumed since only small systems expected with FAT16
        ptrBootSector->ucCheck55 = 0x55;                                 // mark that the sector is valid
        ptrBootSector->ucCheckAA = 0xaa;
        ptr_common = &ptrBootSector_16->bs_common;
        uMemcpy(ptr_common->BS_FilSysType, "FAT16   ", 8);
        #if defined UTFAT12 || defined UTFAT16
        if (utDisks[iDiskNumber].usDiskFlags & DISK_FORMAT_FAT12) {
            #if defined UTFAT12
            ptr_common->BS_FilSysType[4] = '2';
            utDisks[iDiskNumber].ulLogicalBaseAddress = (6 + ulSectorCnt);
            #endif
        }
            #if defined UTFAT16
        else {
            utDisks[iDiskNumber].ulLogicalBaseAddress = (8 + ulSectorCnt);
        }
            #endif
        #endif
        utDisks[iDiskNumber].ulVirtualBaseAddress = (utDisks[iDiskNumber].ulLogicalBaseAddress + (32 - 1));
        ulFAT32size[iDiskNumber] = ulSectorCnt;
    }
    else {
    #endif
        ptrBootSector->boot_sector_bpb.BPB_HiddSec[0]    = BOOT_SECTOR_LOCATION;
        ulPartitionSize = (utDisks[iDiskNumber].ulSD_sectors - BOOT_SECTOR_LOCATION);
        ptrBootSector->boot_sector_bpb.BPB_NumFATs       = ucFATs;
        ptrBootSector->boot_sector_bpb.BPB_RsvdSecCnt[0] = RESERVED_SECTION_COUNT;
        ptrBootSector->boot_sector_bpb.BPB_TotSec32[0]   = (unsigned char)(ulPartitionSize);
        ptrBootSector->boot_sector_bpb.BPB_TotSec32[1]   = (unsigned char)(ulPartitionSize >> 8);
        ptrBootSector->boot_sector_bpb.BPB_TotSec32[2]   = (unsigned char)(ulPartitionSize >> 16);
        ptrBootSector->boot_sector_bpb.BPB_TotSec32[3]   = (unsigned char)(ulPartitionSize >> 24);
        if (ulPartitionSize <= 532480) {                                 // disks up to 260MB
            utDisks[iDiskNumber].utFAT.ucSectorsPerCluster = 1;
        }
        else if (ulPartitionSize <= 16777216) {                          // disks up to 8GB
            utDisks[iDiskNumber].utFAT.ucSectorsPerCluster = 8;
        }
        else if (ulPartitionSize <= 33554432) {                          // disks up to 16GB
            utDisks[iDiskNumber].utFAT.ucSectorsPerCluster = 16;
        }
        else if (ulPartitionSize <= 67108864) {                          // disks up to 32GB
            utDisks[iDiskNumber].utFAT.ucSectorsPerCluster = 32;
        }
        else {                                                           // greater than 32GB
            utDisks[iDiskNumber].utFAT.ucSectorsPerCluster = 64;
        }                    
        ulFAT32size[iDiskNumber] = (((256 * utDisks[iDiskNumber].utFAT.ucSectorsPerCluster) + ucFATs)/2);
        ulFAT32size[iDiskNumber] = (((ulPartitionSize - RESERVED_SECTION_COUNT) + (ulFAT32size[iDiskNumber] - 1)) / ulFAT32size[iDiskNumber]);
        ptrBootSector->BPB_FATSz32[0]   = (unsigned char)(ulFAT32size[iDiskNumber]);
        ptrBootSector->BPB_FATSz32[1]   = (unsigned char)(ulFAT32size[iDiskNumber] >> 8);
        ptrBootSector->BPB_FATSz32[2]   = (unsigned char)(ulFAT32size[iDiskNumber] >> 16);
        ptrBootSector->BPB_FATSz32[3]   = (unsigned char)(ulFAT32size[iDiskNumber] >> 24);
        ptrBootSector->BPB_RootClus[0]  = 2;
        ptrBootSector->BPB_FSInfo[0]    = 1;
        ptrBootSector->BPB_BkBootSec[0] = BACKUP_ROOT_SECTOR;
        utDisks[iDiskNumber].ulLogicalBaseAddress = (BOOT_SECTOR_LOCATION + RESERVED_SECTION_COUNT + (ulFAT32size[iDiskNumber] * ucFATs));
        ptr_common = &ptrBootSector->bs_common;
        uMemcpy(ptr_common->BS_FilSysType, "FAT32   ", 8);
        utDisks[iDiskNumber].ulVirtualBaseAddress = utDisks[iDiskNumber].ulLogicalBaseAddress - (2 * utDisks[iDiskNumber].utFAT.ucSectorsPerCluster);
    #if defined UTFAT12 || defined UTFAT16
    }
    #endif
    ptrBootSector->boot_sector_bpb.BPB_SecPerClus = utDisks[iDiskNumber].utFAT.ucSectorsPerCluster;
    ptr_common->BS_DrvNum  = 0x80;
    ptr_common->BS_BootSig = 0x29;
    #if defined FAT_EMULATION                                            // {12}
    if ((utDisks[iDiskNumber].usDiskFlags & DISK_FAT_EMULATION) != 0) {
        #if !defined SDCARD_SUPPORT
        utDisks[iDiskNumber].utFAT.ulFAT_start = (utDisks[iDiskNumber].ulLogicalBaseAddress - ulFAT32size[iDiskNumber]);
        utDisks[iDiskNumber].utFAT.ulFatSize = ulFAT32size[iDiskNumber];
        #endif
        uMemcpy(ptr_common->BS_VolLab, "DATA_DISK  ", 11);
        #if DISK_COUNT > 1
        ptr_common->BS_VolLab[10] = (iDiskNumber + '1');
        #endif
        ptr_common->BS_VolID[0] = 1;
        ptr_common->BS_VolID[1] = 2;
        ptr_common->BS_VolID[2] = 3;
        ptr_common->BS_VolID[3] = 4;
        ptrBootSector->ucCheck55 = 0x55;
        ptrBootSector->ucCheckAA = 0xaa;
        return;
    }
    #endif
    #if ((defined SDCARD_SUPPORT || defined SPI_FLASH_FAT || defined FLASH_FAT || defined USB_MSD_HOST) && (defined UTFAT_WRITE && defined UTFAT_FORMATTING))
        #if defined RANDOM_NUMBER_GENERATOR
    ptr_common->BS_VolID[0] = (unsigned char)fnRandom();                 // could also be generated by combining present data and time
    ptr_common->BS_VolID[1] = (unsigned char)fnRandom();
    ptr_common->BS_VolID[2] = (unsigned char)fnRandom();
    ptr_common->BS_VolID[3] = (unsigned char)fnRandom();
        #else
    ptr_common->BS_VolID[0] = 1;
    ptr_common->BS_VolID[1] = 2;
    ptr_common->BS_VolID[2] = 3;
    ptr_common->BS_VolID[3] = 4;
        #endif
    if (utDisks[iDiskNumber].cVolumeLabel[0] == 0) {
        uMemcpy(ptr_common->BS_VolLab, "NO NAME    ", 11);
    }
    else {
        int i = 0;
        while (i < 11) {
            if ((ptr_common->BS_VolLab[i] = utDisks[iDiskNumber].cVolumeLabel[i]) == 0) {
                while (i < 11) {
                    ptr_common->BS_VolLab[i++] = ' ';
                }
            }
            i++;
        }
    }
  //ptrBootSector->ucCheck55 = 0x55;                                     // already present in buffer
  //ptrBootSector->ucCheckAA = 0xaa;
    #endif
}

static void fnCreateInfoSector(int iDiskNumber)
{
    INFO_SECTOR_FAT32 *ptrInfoSector;
    unsigned long ulFreeCount;
    UTDISK *ptr_utDisk = &utDisks[iDiskNumber];
    unsigned char ucFATs = NUMBER_OF_FATS;
    #if defined FAT_EMULATION                                            // {12}
    if ((utDisks[iDiskNumber].usDiskFlags & DISK_FAT_EMULATION) != 0) {
        ucFATs = 1;                                                      // emulated FAT uses only one FAT
    }
    #endif
    ptrInfoSector = (INFO_SECTOR_FAT32 *)ptr_utDisk->ptrSectorData;
    ulFreeCount = ((ptr_utDisk->ulSD_sectors - RESERVED_SECTION_COUNT - BOOT_SECTOR_LOCATION - (ucFATs * ulFAT32size[iDiskNumber]))/ptr_utDisk->utFAT.ucSectorsPerCluster);
    ptrInfoSector->FSI_LeadSig[3] = 0x41;
    ptrInfoSector->FSI_LeadSig[2] = 0x61;
    ptrInfoSector->FSI_LeadSig[1] = 0x52;
    ptrInfoSector->FSI_LeadSig[0] = 0x52;
    ptrInfoSector->FSI_StrucSig[3] = 0x61;
    ptrInfoSector->FSI_StrucSig[2] = 0x41;
    ptrInfoSector->FSI_StrucSig[1] = 0x72;
    ptrInfoSector->FSI_StrucSig[0] = 0x72;
    #if defined FAT_EMULATION                                            // {12}
    if ((ptr_utDisk->usDiskFlags & DISK_FAT_EMULATION) != 0) {
        fnAddInfoSect(ptrInfoSector, (ulFreeCount - 1 - ptr_utDisk->utFileInfo.ulNextFreeCluster), ptr_utDisk->utFileInfo.ulNextFreeCluster);
    }
    else {
    #endif
        fnAddInfoSect(ptrInfoSector, (ulFreeCount - 1), 3);              // one cluster occupied by root directory by default and first useable cluster number is 3
    #if defined FAT_EMULATION                                            // {12}
    }
    #endif
    ptrInfoSector->FSI_StrucSig[3] = 0x61;
    ptrInfoSector->FSI_StrucSig[2] = 0x41;
    ptrInfoSector->FSI_StrucSig[1] = 0x72;
    ptrInfoSector->FSI_StrucSig[0] = 0x72;
    ptrInfoSector->FSI_TrailSig[3] = 0xaa;
    ptrInfoSector->FSI_TrailSig[2] = 0x55;
}
#endif

#if ((defined SDCARD_SUPPORT || defined SPI_FLASH_FAT || defined FLASH_FAT || defined USB_MSD_HOST) && (defined UTFAT_WRITE)) || defined FAT_EMULATION
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

#if defined SDCARD_SUPPORT || defined SPI_FLASH_FAT || defined FLASH_FAT || defined USB_MSD_HOST || defined MANAGED_FILES
    #if !defined SIMPLE_FLASH && (defined SPI_FLASH_FAT || defined FLASH_FAT)
static void fnInitBlockManagement(int iDiskNumber)
{
    if (fnInitialiseBlockMap(iDiskNumber, 0) == NO_REMAP_TABLE_FOUND) {  // no table available
        fnInitialiseBlockMap(iDiskNumber, 1);                            // create the table on first use
    }
    fnGetBlankBlocks(iDiskNumber);                                       // create a list of blank physical sectors that can be used for new FAT sector remapping
}
    #endif

// Mass storage task
//
extern void fnMassStorage(TTASKTABLE *ptrTaskTable)
{
    #if defined _WINDOWS
    static int iFormatCount;
    #endif
    #if DISK_COUNT > 1
        int iDiskNumber = -1;
        #define _return continue
    #else
        #if !defined DISK_INTERNAL_FLASH
            #if defined DISK_D
                #define iDiskNumber DISK_D                               // if the single disk is not internal flash use disk D
            #else
                #define iDiskNumber DISK_E                               // if the single disk is not internal flash use disk E (when disk D is not defined)
            #endif
        #else
            #define iDiskNumber DISK_INTERNAL_FLASH
        #endif
        #define _return return
    #endif
    unsigned char ucInputMessage[HEADER_LENGTH];                         // reserve space for receiving messages
    #if defined SDCARD_SUPPORT || defined SPI_FLASH_FAT || defined FLASH_FAT || defined USB_MSD_HOST
    int iActionResult = 0;
        #if defined SDCARD_SUPPORT
    unsigned char ucData[18];
    unsigned char ucResult = 0;
        #endif
    #endif
    #if defined MANAGED_FILES
    if (iManagedMedia != 0) {
        iManagedMedia = 0;
        fnManagedMediaCheck();
    }
    #endif
    #if DISK_COUNT > 1
    while (++iDiskNumber < DISK_COUNT) {                                 // for each disk
    #endif
    #if defined SDCARD_SUPPORT || defined SPI_FLASH_FAT || defined FLASH_FAT || defined USB_MSD_HOST
    if (iMemoryOperation[iDiskNumber] & _READING_MEMORY) {               // reading
        if ((iActionResult = _utReadDiskSector[iDiskNumber](&utDisks[iDiskNumber], 0, utDisks[iDiskNumber].ptrSectorData)) == CARD_BUSY_WAIT) {
            return;                                                      // still reading so keep waiting
        }
    }
    if ((iMemoryOperation[iDiskNumber] & _INITIALISED_MEMORY) == 0) {    // if initialisation in progress
        #if defined UTFAT_WRITE && defined UTFAT_FORMATTING
        static unsigned long ulFatSectors[DISK_COUNT] = {0};             // static variables used when formatting a disk
        static unsigned char ucFatCopies[DISK_COUNT] = {0};
        #endif
        switch (iMemoryState[iDiskNumber]) {                             // perform the initialisation state-event process
        case SD_STATE_STARTING:
        case SD_STATE_WAIT_CARD:
        #if defined FLASH_FAT                                            // FAT in internal flash
            #if DISK_COUNT > 1
            if (iDiskNumber == DISK_INTERNAL_FLASH) {
            #endif
                if (utDisks[iDiskNumber].ptrSectorData == 0) {           // local sector copy not yet allocated
                    utDisks[iDiskNumber].ptrSectorData = SDCARD_MALLOC(512); // allocate a buffer which will contain data read from the present sector (this should be long word aligned and possibly in a specific memory in case DMA operation is to be used)
                    utDisks[iDiskNumber].ulSD_sectors = (FLASH_FAT_SECTORS); // number of 512 byte sectors available for FAT usage
                    utDisks[iDiskNumber].utFAT.usBytesPerSector = BYTES_PER_SECTOR;
                    utDisks[iDiskNumber].ucDriveNumber = (unsigned char)iDiskNumber;
                    _utReadPartialDiskData[iDiskNumber] = utReadPartialFlash;// internal flash function to read a partial sector
                    _utReadDiskSector[iDiskNumber] = utReadFlashSector;  // internal flash function to read a sector to a defined buffer
            #if defined UTFAT_WRITE
                    _utCommitSectorData[iDiskNumber] = utCommitFlashSectorData; // internal flash function to write a sector from a buffer
                    _utDeleteSector[iDiskNumber] = utDeleteFlashSector;  // internal flash function to delete a sector
            #endif
            #if !defined SIMPLE_FLASH
                    fnInitBlockManagement(iDiskNumber);                  // initiate block management and wear leveling
            #endif
                }
            #if DISK_COUNT > 1
                iMemoryState[iDiskNumber] = DISK_MOUNTING_1;
                iDiskNumber--;
                _return;
            }
            #endif
        #endif
        #if defined NAND_FLASH_FAT
            if (utDisks[iDiskNumber].ptrSectorData == 0) {               // local sector copy not yet allocated
                fnInitNAND();                                            // initialise the NAND interface
                utDisks[iDiskNumber].ptrSectorData = SDCARD_MALLOC(512); // allocate a buffer which will contain data read from the present sector (this should be long word aligned and possibly in a specific memory in case DMA operation is to be used)
                utDisks[iDiskNumber].ulSD_sectors = (USER_AREA_BLOCKS * NAND_PAGES_IN_BLOCK);
                utDisks[iDiskNumber].utFAT.usBytesPerSector = BYTES_PER_SECTOR;
                uTaskerStateChange(OWN_TASK, UTASKER_GO);                // switch to polling mode of operation
            }
            if (fnGetBlankBlocks() != 0) {                               // start the process of reading all blocks to get their present state
                _return;                                                 // repeat until the complete NAND flash has been checked for blank blocks
            }
            uTaskerStateChange(OWN_TASK, UTASKER_STOP);
        #endif
        #if defined USB_MSD_HOST
            #if DISK_COUNT > 1
            if (iDiskNumber == DISK_MEM_STICK) {
                _utReadDiskSector[iDiskNumber] = utReadMSDsector;        // USB function to read a sector to a defined buffer
                _utReadPartialDiskData[iDiskNumber] = utReadPartialMSDsector; // internal flash function to read a partial sector
                #if defined UTFAT_WRITE
                _utCommitSectorData[iDiskNumber] = utCommitMSDSectorData;
                _utDeleteSector[iDiskNumber] = utDeleteMSDSector;
                #endif
                continue;
            }
            #else
            _utReadDiskSector[iDiskNumber] = utReadMSDsector;            // USB function to read a sector to a defined buffer
            _utReadPartialDiskData[iDiskNumber] = utReadPartialMSDsector;// USB function to read a partial sector
                #if defined UTFAT_WRITE
            _utCommitSectorData[iDiskNumber] = utCommitMSDSectorData;
            _utDeleteSector[iDiskNumber] = utDeleteMSDSector;
                #endif
            break;
            #endif
        #endif
        #if defined SPI_FLASH_FAT
            #if DISK_COUNT > 1
            if (iDiskNumber == DISK_SPI_FLASH) {
            #endif
                // It is assumed that SPI interface has been previously initialised in the hardware module
                //
                #if (SPI_FLASH_PAGE_LENGTH != 256) && (SPI_FLASH_PAGE_LENGTH != 512) && (SPI_FLASH_PAGE_LENGTH != 528) // only 256/512/528 byte pages are supported
                    #error utFAT expects the external SPI flash to have 512 byte page size (eg. AT45DB321)
                #endif
                if (utDisks[iDiskNumber].ptrSectorData == 0) {           // local sector copy not yet allocated
                #if defined SPI_FLASH_MULTIPLE_CHIPS
                    if (fnSPI_FlashExt_available(0) == NO_SPI_FLASH_AVAILABLE)
                #else
                    if (fnSPI_Flash_available() == NO_SPI_FLASH_AVAILABLE)
                #endif
                    {
                        iMemoryState[iDiskNumber] = DISK_HW_NOT_RESPONDING;
                        utDisks[iDiskNumber].usDiskFlags = DISK_NOT_PRESENT;
                        fnMemoryDebugMsg("No SPI Flash!!\r\n");
                        _return;
                    }
                    utDisks[iDiskNumber].ucDriveNumber = (unsigned char)iDiskNumber;
                #if !defined SIMPLE_FLASH
                    fnInitBlockManagement(iDiskNumber);                  // initiate block management and wear leveling
                #endif
                    utDisks[iDiskNumber].ptrSectorData = SDCARD_MALLOC(512); // allocate a buffer which will contain data read from the present sector (this should be long word aligned and possibly in a specific memory in case DMA operation is to be used)
                    utDisks[iDiskNumber].utFAT.usBytesPerSector = BYTES_PER_SECTOR;
                #if !defined SIMPLE_FLASH
                    utDisks[iDiskNumber].ulSD_sectors = (SPI_FLASH_FAT_SECTORS); // 512 byte sectors available for FAT usage
                    _utReadPartialDiskData[iDiskNumber] = utReadPartialFlash;
                    _utReadDiskSector[iDiskNumber] = utReadFlashSector;
                #else
                    utDisks[iDiskNumber].ulSD_sectors = (SPI_FLASH_PAGES); // 512 byte sectors (pages) contained in the device
                    _utReadPartialDiskData[iDiskNumber] = utReadPartialSPI; // SPI flash function to read a partial sector
                    _utReadDiskSector[iDiskNumber] = utReadSPISector;    // SPI flash function to read a sector to a defined buffer
                #endif
                #if defined UTFAT_WRITE
                    #if !defined SIMPLE_FLASH
                    _utCommitSectorData[iDiskNumber] = utCommitFlashSectorData;
                    _utDeleteSector[iDiskNumber] = utDeleteFlashSector;
                    #else
                    _utCommitSectorData[iDiskNumber] = utCommitSPISectorData; // SPI flash function to write a sector from a buffer
                    _utDeleteSector[iDiskNumber] = utDeleteSPISector;    // SPI flash function to delete a sector
                    #endif
                #endif
                }
            #if DISK_COUNT > 1
                iMemoryState[iDiskNumber] = DISK_MOUNTING_1;
                iDiskNumber--;
                _return;
            }
            #endif
        #endif
        #if defined FAT_EMULATION                                        // {12}
            if (iDiskNumber == DISK_EM_FAT) {
                _utReadDiskSector[iDiskNumber] = fnReadEmulatedSector;   // emulated FAT function to read a sector to a defined buffer
                _utReadPartialDiskData[iDiskNumber] = fnReadPartialEmulatedSector;
                utDisks[iDiskNumber].ucDriveNumber = iDiskNumber;
                utDisks[iDiskNumber].ulSD_sectors = (EMULATED_FAT_DISK_SIZE/512); // number of 512 byte sectors available for FAT usage
                utDisks[iDiskNumber].usDiskFlags = DISK_FAT_EMULATION;
                utDisks[iDiskNumber].ptrSectorData = (unsigned char *)SDCARD_MALLOC(512);
                iMemoryState[iDiskNumber] = DISK_MOUNTING_1;
            #if !defined SDCARD_SUPPORT
                uTaskerMonoTimer(OWN_TASK, 0, E_POWER_STABILISED);       // schedule again to mount the virtual disk
            #endif
                continue;
            }
        #endif
        #if defined SDCARD_SUPPORT                                       // initialisation only required by SD card
            _utReadPartialDiskData[iDiskNumber] = utReadPartialSector;   // SD card function to read a partial sector
            _utReadDiskSector[iDiskNumber] = utReadDiskSector;           // SD card function to read a sector to a defined buffer
            #if defined UTFAT_WRITE
            _utCommitSectorData[iDiskNumber] = utCommitSectorData;       // SD card function to write a sector from a buffer
            _utDeleteSector[iDiskNumber] = utDeleteSector;               // SD card function to delete a sector
            #endif
            #if defined SD_CONTROLLER_AVAILABLE
            fnInitSDCardInterface();                                     // HW interface initialisation
            ucSEND_CSD_CMD9[1] = ucSELECT_CARD_CMD7[1] = ucAPP_CMD_CMD55[1] = ucSET_BUS_WIDTH_CMD6[1] = 0; // start with zeroed RCA address
            ucSEND_CSD_CMD9[2] = ucSELECT_CARD_CMD7[2] = ucAPP_CMD_CMD55[2] = ucSET_BUS_WIDTH_CMD6[2] = 0;
            iMemoryState[DISK_SDCARD] = SD_STATE_STABILISING;            // move to stabilisation delay state
            uTaskerMonoTimer(OWN_TASK, T_POWER_STABILISE, E_POWER_STABILISED); // wait until SD card power stabilised
            #else
            INITIALISE_SPI_SD_INTERFACE();                               // initialise the SPI interface to the card
                #if defined SDCARD_DETECT_INPUT_POLL || defined SDCARD_DETECT_INPUT_INTERRUPT
                    #if defined SDCARD_DETECT_INPUT_INTERRUPT 
            fnPrepareDetectInterrupt();                                  // prepare interrupt detection of SD card presence
                    #endif
            if (SDCARD_DETECTION() == 0) {                               // if card is not detected immediately abort mounting process
                fnInitialisationError(DISK_SDCARD, 0);                   // try to remount the card
                break;
            }
                    #if defined _WINDOWS
            else {
                SD_card_state(SDCARD_INSERTED, SDCARD_REMOVED);
            }
                    #endif
                #endif
            SET_SD_DI_CS_HIGH();                                         // prepare chip select and DI ready for the initialisation
            POWER_UP_SD_CARD();                                          // power up if necessary
            iMemoryState[DISK_SDCARD] = SD_STATE_STABILISING;            // move to stabilisation delay state
            ENABLE_SPI_SD_OPERATION();                                   // dummy to solve an AVR32 GCC optimising problem
            uTaskerMonoTimer(OWN_TASK, T_POWER_STABILISE, E_POWER_STABILISED); // wait until SD card power stabilised
            #endif
            break;

        case SD_STATE_STABILISING:                                       // delay after applying power to the SD-card
            #if defined SD_CONTROLLER_AVAILABLE
            POWER_UP_SD_CARD();                                          // apply power
                #if defined SDCARD_DETECT_INPUT_POLL || defined SDCARD_DETECT_INPUT_INTERRUPT
                    #if defined SDCARD_DETECT_INPUT_INTERRUPT 
            fnPrepareDetectInterrupt();                                  // prepare interrupt detection of SD card presence
                    #endif
            if (SDCARD_DETECTION() == 0) {                               // if card is not detected immediately abort mounting process
                fnInitialisationError(DISK_SDCARD, 0);                   // try to remount the card
                break;
            }
                    #if defined _WINDOWS
            else {
                SD_card_state(SDCARD_INSERTED, SDCARD_REMOVED);
            }
                    #endif
                #endif
            iMemoryState[DISK_SDCARD] = SD_STATE_GO_IDLE;                // prepare to communicate
            uTaskerMonoTimer(OWN_TASK, T_POWER_STABILISE, E_POWER_STABILISED); // wait until SD card power stabilised
            break;
            #else
            {                                                            // send at least 74 clocks to the SD-card
                int i = 10;
                while (i--) {                                            // set the SD card to native command mode by sending 80 clocks
                    WRITE_SPI_CMD(0xff);                                 // write dummy tx
                    WAIT_TRANSMISSON_END();                              // wait until transmission complete
                    (void)READ_SPI_DATA();                               // read 10 dummy bytes from the interface in order to generate 80 clock pulses on the interface (at least 74 needed)
                }
                SET_SD_CS_LOW();                                         // assert the chip select line to the SD-card ready to start the initialisation sequence
                iMemoryState[DISK_SDCARD] = SD_STATE_GO_IDLE;            // move to next state
                SET_SD_CARD_MODE();                                      // allow final mode in case the DIN line had to be pulled up during native mode sequence
            }                                                            // fall through
            #endif
        case SD_STATE_GO_IDLE:
            if ((iActionResult = fnSendSD_command(ucGO_IDLE_STATE_CMD0, &ucResult, 0)) != UTFAT_SUCCESS) {
                if (iActionResult == CARD_BUSY_WAIT) {
                    _return;                                             // read is taking time to complete so quit for the moment
                }
                ucResult = 0;                                            // set error since the result is not as expected
            }
            if (R1_IN_IDLE_STATE != ucResult) {                          // no valid card detected so disable power and try again after a delay
                fnInitialisationError(DISK_SDCARD, 0);                   // the card is no present or is behaving incorrectly - stop and try again later
                break;
            }                                                            // SD card must return the idle state to be able to continue
                                                                         // in the idle state the card accepts only commands 0, 1, ACMD41 and 58
            iMemoryState[DISK_SDCARD] = SD_STATE_IF_COND;                // fall through
        case SD_STATE_IF_COND:                                           // it is mandatory for a host compliant to Physical Spec. Version 2.00 to send the CMD8 to retrieve non-supported voltage range
            if ((iActionResult = fnSendSD_command(ucIF_COND_CMD8, &ucResult, ucData)) != UTFAT_SUCCESS) {
                if (iActionResult == CARD_BUSY_WAIT) {
                    _return;                                             // read is taking time to complete so quit for the moment
                }
                fnInitialisationError(DISK_SDCARD, 0);                   // the card is behaving incorrectly - stop and try again later
                break;
            }
            if (ucResult == SDC_CARD_VERSION_2PLUS) {                    // version 2 or higher
                if (ucData[3] == CHECK_PATTERN) {
                    if (ucData[2] == VOLTAGE_2_7__3_6) {                 // check whether the card can operate between 2.7V and 3.6V
                        iMemoryState[DISK_SDCARD] = SD_STATE_APP_CMD55_CMD41; // now poll the SD card until it accept the application command 42
                        uTaskerStateChange(OWN_TASK, UTASKER_ACTIVATE);  // run again
                        break;
                    }
                    fnMemoryDebugMsg("SD-card voltage error\r\n");
                }
                else {
                    fnInitialisationError(DISK_SDCARD, 0);               // no valid response
                    break;
                }
            }
            else {                                                       // version 1 or MMC type
                fnMemoryDebugMsg("SD-card V1 or MMC - not supported!\r\n");
            }
            fnInitialisationError(DISK_SDCARD, 1);                       // not supported
            break;
        case SD_STATE_APP_CMD55_CMD41:
            if ((iActionResult = fnSendSD_command(ucAPP_CMD_CMD55, &ucResult, 0)) != UTFAT_SUCCESS) {
                if (iActionResult == CARD_BUSY_WAIT) {
                    _return;                                             // read is taking time to complete so quit for the moment
                }
                fnInitialisationError(DISK_SDCARD, 0);                   // the card is behaving incorrectly - stop and try again later
                break;
            }
            if (ucResult > R1_IN_IDLE_STATE) {
                uTaskerStateChange(OWN_TASK, UTASKER_ACTIVATE);          // try again
                break;
            }
            iMemoryState[DISK_SDCARD] = SD_STATE_APP_CMD55_CMD41_2;      // fall through to send the application command
        case SD_STATE_APP_CMD55_CMD41_2:            
            #if defined SD_CONTROLLER_AVAILABLE                          // this command returns OCR result in SD card mode
            if ((iActionResult = fnSendSD_command(ucSEND_OP_COND_ACMD_CMD41, &ucResult, ucData)) != UTFAT_SUCCESS)
            #else
            if ((iActionResult = fnSendSD_command(ucSEND_OP_COND_ACMD_CMD41, &ucResult, 0)) != UTFAT_SUCCESS) 
            #endif
            {
                if (iActionResult == CARD_BUSY_WAIT) {
                    _return;                                             // read is taking time to complete so quit for the moment
                }
                fnInitialisationError(DISK_SDCARD, 0);                   // the card is behaving incorrectly - stop and try again later
                break;
            }
            #if defined SD_CONTROLLER_AVAILABLE
            if ((ucData[0] & 0x80) == 0)                                 // check card busy bit
            #else
            if (ucResult != 0) 
            #endif
            {
                iMemoryState[DISK_SDCARD] = SD_STATE_APP_CMD55_CMD41;    // loop back
                uTaskerStateChange(OWN_TASK, UTASKER_ACTIVATE);          // try again
                break;
            }
            iMemoryState[DISK_SDCARD] = SD_STATE_OCR;                    // fall through to issue CMD58
        case SD_STATE_OCR:
            #if !defined SD_CONTROLLER_AVAILABLE                         // OCR value is returned to the CMD41 on SD card interface {5}
            if ((iActionResult = fnSendSD_command(ucREAD_OCR_CMD58, &ucResult, ucData)) != UTFAT_SUCCESS) { // get card capacity information
                if (iActionResult == CARD_BUSY_WAIT) {
                    _return;                                             // read is taking time to complete so quit for the moment
                }
                fnInitialisationError(DISK_SDCARD, 0);                   // the card is behaving incorrectly - stop and try again later
                break;
            }
            #endif
            fnMemoryDebugMsg("SD-card V2 - ");
            if (ucData[0] & HIGH_CAPACITY_SD_CARD_MEMORY) {              // check the CCS bit
                utDisks[DISK_SDCARD].usDiskFlags = HIGH_CAPACITY_SD_CARD;
                fnMemoryDebugMsg("High Capacity\r\n");
            }
            else {
                utDisks[DISK_SDCARD].usDiskFlags = 0;
                fnMemoryDebugMsg("standard\r\n");
            }
            #if defined SD_CONTROLLER_AVAILABLE                          // SC card mode sequency requires the CID to be read and then an RCA address to be set
            iMemoryState[DISK_SDCARD] = SD_STATE_GET_INFO;               // fall through to issue CMD2 and read card information
        case SD_STATE_GET_INFO:
            if ((iActionResult = fnSendSD_command(ucSEND_CID_CMD2, &ucResult, ucData)) != UTFAT_SUCCESS) { // get card information
                if (iActionResult == CARD_BUSY_WAIT) {
                    _return;                                             // read is taking time to complete so quit for the moment
                }
                fnInitialisationError(DISK_SDCARD, 0);                   // the card is behaving incorrectly - stop and try again later
                break;
            }
            iMemoryState[DISK_SDCARD] = SD_STATE_SET_ADDRESS;
        case SD_STATE_SET_ADDRESS:
            if ((iActionResult = fnSendSD_command(ucSET_REL_ADD_CMD3, &ucResult, ucData)) != UTFAT_SUCCESS) { // set relative address
                if (iActionResult == CARD_BUSY_WAIT) {
                    _return;                                             // read is taking time to complete so quit for the moment
                }
                fnInitialisationError(DISK_SDCARD, 0);                   // the card is behaving incorrectly - stop and try again later
                break;
            }
            if ((ucData[2] & (CURRENT_CARD_STATUS_MASK | SD_CARD_READY_FOR_DATA)) == (CURRENT_STATE_IDENT | SD_CARD_READY_FOR_DATA)) {
                ucSEND_CSD_CMD9[1] = ucSELECT_CARD_CMD7[1] = ucAPP_CMD_CMD55[1] = ucSET_BUS_WIDTH_CMD6[1] = ucData[0];  // save the published RCA address
                ucSEND_CSD_CMD9[2] = ucSELECT_CARD_CMD7[2] = ucAPP_CMD_CMD55[2] = ucSET_BUS_WIDTH_CMD6[2] = ucData[1];
            }
            #endif
            iMemoryState[DISK_SDCARD] = SD_STATE_GET_SECTORS;            // fall through to issue CMD9 and read card specific data
        case SD_STATE_GET_SECTORS:
            if ((iActionResult = fnSendSD_command(ucSEND_CSD_CMD9, &ucResult, ucData)) != UTFAT_SUCCESS) { // get card capacity information
                if (iActionResult == CARD_BUSY_WAIT) {
                    _return;                                             // read is taking time to complete so quit for the moment
                }
                fnInitialisationError(DISK_SDCARD, 0);                   // the card is behaving incorrectly - stop and try again later
                break;
            }
            #if !defined SD_CONTROLLER_AVAILABLE
            if (fnCheckCSD(ucData) != 0) {
                uTaskerStateChange(OWN_TASK, UTASKER_ACTIVATE);          // schedule the task to retry
                _return;                                                 // not recognised or there was a CRC error
            }
            #endif
            uMemcpy(utDisks[DISK_SDCARD].utFileInfo.ucCardSpecificData, &ucData[2 - SD_CONTROLLER_SHIFT], sizeof(utDisks[DISK_SDCARD].utFileInfo.ucCardSpecificData)); // back up the card specific data since it can be of interest later
            if (ucData[2 - SD_CONTROLLER_SHIFT] & HIGH_CAPACITY_SD_CARD_MEMORY) { // high capacity
                utDisks[DISK_SDCARD].ulSD_sectors = (((ucData[9 - SD_CONTROLLER_SHIFT] << 16) + (ucData[10 - SD_CONTROLLER_SHIFT] << 8)  + ucData[11 - SD_CONTROLLER_SHIFT]) + 1);// SD version 2 assumed
                utDisks[DISK_SDCARD].ulSD_sectors *= 1024;               // the number of sectors on the SD card
            }
            else {                                                       // standard capacity
                utDisks[DISK_SDCARD].ulSD_sectors = ((((ucData[8 - SD_CONTROLLER_SHIFT] & 0x03) << 10) + (ucData[9 - SD_CONTROLLER_SHIFT] << 2) + (ucData[10 - SD_CONTROLLER_SHIFT] >> 6)) + 1);// SD version 2 assumed
                utDisks[DISK_SDCARD].ulSD_sectors *= (1 << ((((ucData[11 - SD_CONTROLLER_SHIFT] & 0x03) << 1) + (ucData[12 - SD_CONTROLLER_SHIFT] >> 7)) + 2)); // the number of 512 byte sectors on the SD card
                if ((ucData[7 - SD_CONTROLLER_SHIFT] & 0x0f) == 0x0a) {  // 1024 byte block length indicates 2G card
                    utDisks[DISK_SDCARD].ulSD_sectors *= 2;
                }
            }
            fnMemoryDebugMsg("\r\n");
            if (utDisks[DISK_SDCARD].ptrSectorData == 0) {
                utDisks[DISK_SDCARD].ptrSectorData = SDCARD_MALLOC(512); // allocate a buffer which will contain data read from the present sector (this should be long word aligned and possibly in a specific memory in case DMA operation is to be used)
            }
            #if defined SD_CONTROLLER_AVAILABLE
            iMemoryState[DISK_SDCARD] = SD_SELECT_CARD;
        case SD_SELECT_CARD:                                             // select the card before reading/writing
            if ((iActionResult = fnSendSD_command(ucSELECT_CARD_CMD7, &ucResult, ucData)) != UTFAT_SUCCESS) {
                if (iActionResult == CARD_BUSY_WAIT) {
                    _return;                                             // read is taking time to complete so quit for the moment
                }
                fnInitialisationError(DISK_SDCARD, 0);                   // the card is behaving incorrectly - stop and try again later
                break;
            }
            iMemoryState[DISK_SDCARD] = SD_SET_INTERFACE_PREPARE;
        case SD_SET_INTERFACE_PREPARE:                                   // set the interface so that 4 bit data mode is used - first set command mode
            if ((iActionResult = fnSendSD_command(ucAPP_CMD_CMD55, &ucResult, 0)) != UTFAT_SUCCESS) {
                if (iActionResult == CARD_BUSY_WAIT) {
                    _return;                                             // read is taking time to complete so quit for the moment
                }
                fnInitialisationError(DISK_SDCARD, 0);                   // the card is behaving incorrectly - stop and try again later
                break;
            }
            if (ucResult > R1_IN_IDLE_STATE) {
                uTaskerStateChange(OWN_TASK, UTASKER_ACTIVATE);          // try again
                break;
            }
            iMemoryState[DISK_SDCARD] = SD_SET_INTERFACE;                // fall through to send the application command
        case SD_SET_INTERFACE:
            if ((iActionResult = fnSendSD_command(ucSET_BUS_WIDTH_CMD6, &ucResult, ucData)) != UTFAT_SUCCESS) { // set bus width
                if (iActionResult == CARD_BUSY_WAIT) {
                    _return;                                             // read is taking time to complete so quit for the moment
                }
                fnInitialisationError(DISK_SDCARD, 0);                   // the card is behaving incorrectly - stop and try again later
                break;
            }
            iMemoryState[DISK_SDCARD] = SD_SET_BLOCK_LENGTH;
            SET_SPI_SD_INTERFACE_FULL_SPEED();                           // speed up the SPI interface since initialisation is complete (as well as data bus width)
        case SD_SET_BLOCK_LENGTH:
            if ((iActionResult = fnSendSD_command(ucSET_BLOCK_LENGTH_CMD16, &ucResult, ucData)) != UTFAT_SUCCESS) { // set relative address
                if (iActionResult == CARD_BUSY_WAIT) {
                    _return;                                             // read is taking time to complete so quit for the moment
                }
                fnInitialisationError(DISK_SDCARD, 0);                   // the card is behaving incorrectly - stop and try again later
                break;
            }
            iMemoryState[DISK_SDCARD] = DISK_MOUNTING_1;
            uTaskerStateChange(OWN_TASK, UTASKER_ACTIVATE);              // schedule again to attempt mounting
            utDisks[DISK_SDCARD].ucDriveNumber = (unsigned char)iDiskNumber;
            #else
            iMemoryState[DISK_SDCARD] = DISK_MOUNTING_1;
            uTaskerStateChange(OWN_TASK, UTASKER_ACTIVATE);              // schedule again to attempt mounting
            utDisks[DISK_SDCARD].ucDriveNumber = (unsigned char)iDiskNumber;
            SET_SD_CS_HIGH();
            SET_SPI_SD_INTERFACE_FULL_SPEED();                           // speed up the SPI interface since initialisation is complete
            #endif
            break;
        #endif

            // Disk mounting begins here
            //
        case DISK_MOUNTING_1:                                            // start mounting SD card or other medium
            utDisks[iDiskNumber].ulPresentSector = 0;                    // the first sector that will be accessed
            if ((iActionResult = ut_read_disk(&utDisks[iDiskNumber])) == CARD_BUSY_WAIT) { // read boot sector from disk
                _return;                                                 // read is taking time to complete so quit for the moment
            }
            iMemoryState[iDiskNumber] = DISK_MOUNTING_2;
        case DISK_MOUNTING_2:
            if (ERROR_SECTOR_INVALID == iActionResult) {
                fnCardNotFormatted(iDiskNumber);
                break;
            }
            else {
                iMemoryState[iDiskNumber] = DISK_MOUNTING_3;
                if (iActionResult != 0) {
                    fnInitialisationError(iDiskNumber, 0);               // the card is behaving incorrectly - stop and try again later
                    break;
                }
                else {
                    EXTENDED_BOOT_RECORD *ptrExtendedBootSector = (EXTENDED_BOOT_RECORD *)utDisks[iDiskNumber].ptrSectorData; // check to see whether partitions are defined rather than it being a boot record
                    BOOT_SECTOR_FAT32 *ptrBootSector = (BOOT_SECTOR_FAT32 *)utDisks[iDiskNumber].ptrSectorData;
                    BOOT_SECTOR_FAT12_FAT16 *ptrBootSectorFat16_12 = (BOOT_SECTOR_FAT12_FAT16 *)utDisks[iDiskNumber].ptrSectorData;
                    if ((ptrExtendedBootSector->EBR_partition_table[0].partition_type != 0) && (ptrBootSector->bs_common.BS_FilSysType[0] != 'F') && (ptrBootSectorFat16_12->bs_common.BS_FilSysType[0] != 'F')) { // identify whether this is a partition table or the boot sector (if the file system type begins with 'F' we assume FAT12/16 or 32)
                        utDisks[iDiskNumber].ulPresentSector = ((ptrExtendedBootSector->EBR_partition_table[0].start_sector[3] << 24) + (ptrExtendedBootSector->EBR_partition_table[0].start_sector[2] << 16) + (ptrExtendedBootSector->EBR_partition_table[0].start_sector[1] << 8) + ptrExtendedBootSector->EBR_partition_table[0].start_sector[0]);
                        if ((iActionResult = ut_read_disk(&utDisks[iDiskNumber])) ==  CARD_BUSY_WAIT) { // read boot sector from disk
                            _return;                                     // read is taking time to complete so quit for the moment
                        }
                    }
                    else {
                        utDisks[iDiskNumber].ulPresentSector = 0;        // the boot sector is 0
                    }
                }
            }                                                            // fall-through
        case DISK_MOUNTING_3:
            if (ERROR_SECTOR_INVALID == iActionResult) {
                fnCardNotFormatted(iDiskNumber);
                break;
            }
            else {
                int iFAT32 = 0;
                BOOT_SECTOR_FAT32 *ptrBootSector;
                unsigned long  ulTotalSections;
                unsigned long  ulFatSize;
                unsigned long  ulFirstDataSector;
                unsigned long  ulCountofClusters;
                unsigned short BPB_RsvdSecCnt;
                unsigned short BPB_FSInfo;
                unsigned short BPB_BytesPerSec;
                unsigned char  BPB_SecPerClus;
                BOOT_SECT_COM  *ptr_common;

                ptrBootSector = (BOOT_SECTOR_FAT32 *)utDisks[iDiskNumber].ptrSectorData; // the buffer content is a boot sector
                BPB_BytesPerSec = (ptrBootSector->boot_sector_bpb.BPB_BytesPerSec[1] << 8);
                ulFatSize = ((ptrBootSector->boot_sector_bpb.BPB_FATSz16[1] << 8) + ptrBootSector->boot_sector_bpb.BPB_FATSz16[0]);
                if (ulFatSize == 0) {                                    // FAT32 will indicate the size in the BPB_FATSz32 field instead
                    ulFatSize = ((ptrBootSector->BPB_FATSz32[3] << 24) + (ptrBootSector->BPB_FATSz32[2] << 16) + (ptrBootSector->BPB_FATSz32[1] << 8) + ptrBootSector->BPB_FATSz32[0]);
                    iFAT32 = 1;                                          // FAT32, irrespective of the disk's size
                }
                ulTotalSections = ((ptrBootSector->boot_sector_bpb.BPB_TotSec16[1] << 8) + ptrBootSector->boot_sector_bpb.BPB_TotSec16[0]);
                if (ulTotalSections == 0) {                              // FAT32 will indicate the size in the BPB_FATSz32 field instead
                    ulTotalSections = ((ptrBootSector->boot_sector_bpb.BPB_TotSec32[3] << 24) + (ptrBootSector->boot_sector_bpb.BPB_TotSec32[2] << 16) + (ptrBootSector->boot_sector_bpb.BPB_TotSec32[1] << 8) + ptrBootSector->boot_sector_bpb.BPB_TotSec32[0]);
                }
                utDisks[iDiskNumber].utFAT.ulFatSize = ulFatSize;         // the sectors in a single FAT
                utDisks[iDiskNumber].utFAT.ucNumberOfFATs = ptrBootSector->boot_sector_bpb.BPB_NumFATs; // the number of FAT copies
                ulFatSize *= ptrBootSector->boot_sector_bpb.BPB_NumFATs; // the complete number of sections occupied by all FATs
                BPB_SecPerClus = ptrBootSector->boot_sector_bpb.BPB_SecPerClus;
                utDisks[iDiskNumber].utFAT.ucSectorsPerCluster = BPB_SecPerClus;
                BPB_RsvdSecCnt = ((ptrBootSector->boot_sector_bpb.BPB_RsvdSecCnt[1] << 8) + ptrBootSector->boot_sector_bpb.BPB_RsvdSecCnt[0]);
                utDisks[iDiskNumber].utFAT.ulFAT_start = (utDisks[iDiskNumber].ulPresentSector + BPB_RsvdSecCnt); // the boot sector plus reserved sectors
            #if defined USB_MSD_HOST
                if ((BPB_BytesPerSec < 512) || (BPB_SecPerClus == 0) || ((BPB_SecPerClus * BPB_BytesPerSec) > (64 * 1024))) // memory stick uses up to 128 BPB_SecPerClus
            #else
                if ((BPB_BytesPerSec < 512) || (BPB_SecPerClus == 0) || ((BPB_SecPerClus * BPB_BytesPerSec) > (32 * 1024)))
            #endif
                {
                    fnMemoryDebugMsg("Malformed - ");
                    fnCardNotFormatted(iDiskNumber);
                    break;
                }
                utDisks[iDiskNumber].usDiskFlags &= ~DISK_UNFORMATTED;
                utDisks[iDiskNumber].usDiskFlags |= DISK_FORMATTED;
                utDisks[iDiskNumber].utFAT.usBytesPerSector = BPB_BytesPerSec;
                ulFirstDataSector = (BPB_RsvdSecCnt + ulFatSize);
                ulCountofClusters = ((ulTotalSections - ulFirstDataSector)/ptrBootSector->boot_sector_bpb.BPB_SecPerClus);
                if ((ulCountofClusters < 65525) && (iFAT32 == 0)) {      // not FAT32
            #if defined UTFAT16 || defined UTFAT12
                    utDisks[iDiskNumber].ulDirectoryBase = 1;
                    ptr_common = &((BOOT_SECTOR_FAT12_FAT16 *)utDisks[iDiskNumber].ptrSectorData)->bs_common;
                    if (ptr_common->BS_FilSysType[4] == '2') {           // FAT12
                #if defined UTFAT12
                        utDisks[iDiskNumber].usDiskFlags |= DISK_FORMAT_FAT12;
                #else
                        fnMemoryDebugMsg("NOT FAT16- ");
                        fnCardNotFormatted(iDiskNumber);
                        break;
                #endif
                    }
                    else {
                #if defined UTFAT16
                        utDisks[iDiskNumber].usDiskFlags |= DISK_FORMAT_FAT16;
                #else
                        fnMemoryDebugMsg("NOT FAT12- ");
                        fnCardNotFormatted(iDiskNumber);
                        break;
                #endif
                    }
            #else
                    fnMemoryDebugMsg("NOT FAT32- ");
                    fnCardNotFormatted(iDiskNumber);
                    break;
            #endif
                }
                else {
                    BPB_FSInfo = ((ptrBootSector->BPB_FSInfo[1] << 8) + ptrBootSector->BPB_FSInfo[0]);
                    utDisks[iDiskNumber].ulDirectoryBase = ((ptrBootSector->BPB_RootClus[3] << 24) + (ptrBootSector->BPB_RootClus[2] << 16) + (ptrBootSector->BPB_RootClus[1] << 8) + ptrBootSector->BPB_RootClus[0]); // root directory start cluster
                    utDisks[iDiskNumber].ulPresentSector = utDisks[iDiskNumber].utFileInfo.ulInfoSector = (utDisks[iDiskNumber].ulPresentSector + BPB_FSInfo); // sector location of structure
                    ptr_common = &ptrBootSector->bs_common;
                }
                utDisks[iDiskNumber].utFAT.ulClusterCount = ((ulTotalSections - BPB_RsvdSecCnt - ulFatSize)/BPB_SecPerClus); // total cluster count
                utDisks[iDiskNumber].ulLogicalBaseAddress = (utDisks[iDiskNumber].utFAT.ulFAT_start + ulFatSize); // data start sector (logical block address)
                utDisks[iDiskNumber].ulVirtualBaseAddress = (utDisks[iDiskNumber].ulLogicalBaseAddress - (utDisks[iDiskNumber].ulDirectoryBase * utDisks[iDiskNumber].utFAT.ucSectorsPerCluster));
            #if defined UTFAT16 || defined UTFAT12
                if (utDisks[iDiskNumber].usDiskFlags & (DISK_FORMAT_FAT16 | DISK_FORMAT_FAT12)) {
                    utDisks[iDiskNumber].ulVirtualBaseAddress += (32 - 1); // fixed 16 kbyte root folder
                }
            #endif
                uMemcpy(utDisks[iDiskNumber].cVolumeLabel, ptr_common->BS_VolLab, sizeof(utDisks[iDiskNumber].cVolumeLabel));
                iMemoryState[iDiskNumber] = DISK_MOUNTING_4;
                if ((iActionResult = ut_read_disk(&utDisks[iDiskNumber])) ==  CARD_BUSY_WAIT) { // read boot sector from disk
                    _return;                                             // read is taking time to complete so quit for the moment
                }
            }                                                            // fall through
        case DISK_MOUNTING_4:                                            // optional step for FAT32 disks
            #if defined SDCARD_SUPPORT
            if (DISK_SDCARD == iDiskNumber) {
                if ((iActionResult == 0) && ((utDisks[iDiskNumber].usDiskFlags & DISK_FORMAT_FAT16) == 0)) { // if the information sector was valid
                    INFO_SECTOR_FAT32 *ptrInfo = (INFO_SECTOR_FAT32 *)utDisks[iDiskNumber].ptrSectorData;
                    if ((ptrInfo->FSI_TrailSig[3] == 0xaa) && (ptrInfo->FSI_TrailSig[2] == 0x55) &&  // check that the FSInfo sector is valid
                      (ptrInfo->FSI_LeadSig[3] == 0x41) && (ptrInfo->FSI_LeadSig[2] == 0x61) && (ptrInfo->FSI_LeadSig[1] == 0x52) && (ptrInfo->FSI_LeadSig[0] == 0x52) &&
                      (ptrInfo->FSI_StrucSig[3] == 0x61) && (ptrInfo->FSI_StrucSig[2] == 0x41) && (ptrInfo->FSI_StrucSig[1] == 0x72) && (ptrInfo->FSI_StrucSig[0] == 0x72)) {
                        utDisks[iDiskNumber].utFileInfo.ulFreeClusterCount = ((ptrInfo->FSI_Free_Count[3] << 24) + (ptrInfo->FSI_Free_Count[2] << 16) + (ptrInfo->FSI_Free_Count[1] << 8) + ptrInfo->FSI_Free_Count[0]);
                        utDisks[iDiskNumber].utFileInfo.ulNextFreeCluster = ((ptrInfo->FSI_Nxt_Free[3] << 24) + (ptrInfo->FSI_Nxt_Free[2] << 16) + (ptrInfo->FSI_Nxt_Free[1] << 8) + ptrInfo->FSI_Nxt_Free[0]);
                        if (utDisks[iDiskNumber].utFileInfo.ulNextFreeCluster <= (utDisks[iDiskNumber].utFileInfo.ulFreeClusterCount - 2)) { // check that the information is valid
                            utDisks[iDiskNumber].usDiskFlags |= FSINFO_VALID;
                        }
                    }
                }
            }
            #endif
            #if defined DISK_C
            if (iDiskNumber == DISK_C) {
                fnMemoryDebugMsg("Disk C mounted");
            }
            #endif
            #if defined DISK_D
            if (iDiskNumber == DISK_D) {
                fnMemoryDebugMsg("Disk D mounted");
            }
            #endif
            #if defined DISK_E
            if (iDiskNumber == DISK_E) {
                fnMemoryDebugMsg("Disk E mounted");
            }
            #endif
            #if defined SDCARD_SUPPORT
            if ((DISK_SDCARD == iDiskNumber) && (GET_SDCARD_WP_STATE())) {
                utDisks[iDiskNumber].usDiskFlags |= (DISK_MOUNTED | WRITE_PROTECTED_SD_CARD); // the write protected disk is now ready for use
                fnMemoryDebugMsg(" (WP)");
            }
                #if defined FAT_EMULATION                                // {12} emulated FAT is always write protected
            else if ((utDisks[iDiskNumber].usDiskFlags & DISK_FAT_EMULATION) != 0) {
                utDisks[iDiskNumber].usDiskFlags = (DISK_MOUNTED | DISK_FORMATTED | DISK_FAT_EMULATION | WRITE_PROTECTED_SD_CARD);
                fnPrepareRootDirectory(&utDisks[iDiskNumber], -1);
            }
                #endif
            else {
                utDisks[iDiskNumber].usDiskFlags |= DISK_MOUNTED;        // the disk is now ready for use (not write protected)
            }
            #else
            utDisks[iDiskNumber].usDiskFlags |= DISK_MOUNTED;            // the disk is now ready for use (not write protected)
            #endif
            fnMemoryDebugMsg("\r\n");
            fnResetDirectories((unsigned char)iDiskNumber);              // {8} reset directories using this disk
            iMemoryOperation[iDiskNumber] |= _INITIALISED_MEMORY;
            iMemoryState[iDiskNumber] = DISK_STATE_READY;
        #if defined MEMORY_STICK_TASK
            if (DISK_MEM_STICK == iDiskNumber) {                         // if the memory stick has just been mounted
                fnInterruptMessage(MEMORY_STICK_TASK, MEM_STICK_MOUNTED);// inform that application that the memory stick can be used
            }
        #endif
        #if defined T_CHECK_CARD_REMOVAL
            if (DISK_SDCARD == iDiskNumber) {
                uTaskerMonoTimer(OWN_TASK, T_CHECK_CARD_REMOVAL, E_CHECK_CARD_REMOVAL); // poll the SD card to detect removal
            }
        #endif
        #if defined _WINDOWS && defined SDCARD_SUPPORT
            if (DISK_SDCARD == iDiskNumber) {
                if (utDisks[iDiskNumber].usDiskFlags & WRITE_PROTECTED_SD_CARD) {
                    SD_card_state((SDCARD_MOUNTED | SDCARD_WR_PROTECTED | SDCARD_INSERTED), SDCARD_REMOVED);
                }
                else {
                    SD_card_state((SDCARD_MOUNTED | SDCARD_INSERTED), SDCARD_REMOVED);
                }
                if (utDisks[iDiskNumber].usDiskFlags & DISK_FORMAT_FAT16) {
                    SD_card_state((SDCARD_FORMATTED_16), (SDCARD_FORMATTED_32));
                }
                else {
                    SD_card_state((SDCARD_FORMATTED_32), (SDCARD_FORMATTED_16));
                }
            }
        #endif
            break;
        #if defined T_CHECK_CARD_REMOVAL
        case STATE_CHECKING_DISK_REMOVAL:
            #if defined SDCARD_DETECT_INPUT_POLL
            if ((SDCARD_DETECTION()) == 0) {                             // if card has been removed start remounting process
                fnInitialisationError(iDiskNumber, 0);                   // try to remount the card
            }
            else {
                iMemoryState[iDiskNumber] = DISK_STATE_READY;
                iMemoryOperation[iDiskNumber] |= (_INITIALISED_MEMORY);
                uTaskerMonoTimer(OWN_TASK, T_CHECK_CARD_REMOVAL, E_CHECK_CARD_REMOVAL); // poll the SD card to detect removal
            }
            #else
            if (UTFAT_SUCCESS != _utReadDiskSector[iDiskNumber](&utDisks[iDiskNumber], utDisks[iDiskNumber].ulPresentSector, utDisks[iDiskNumber].ptrSectorData)) { // {53} read presently selected buffer to verify that the card is still responding
                fnInitialisationError(iDiskNumber, 0);                   // try to remount the card
            }
            else {                                                       // card polling was successful
                iMemoryState[iDiskNumber] = DISK_STATE_READY;
                iMemoryOperation[iDiskNumber] |= (_INITIALISED_MEMORY);
                uTaskerMonoTimer(OWN_TASK, T_CHECK_CARD_REMOVAL, E_CHECK_CARD_REMOVAL); // poll the SD card to detect removal
            }
            #endif
            break;
        #endif
        #if defined UTFAT_WRITE && defined UTFAT_FORMATTING
        case STATE_FORMATTING_DISK_1:                                    // start by creating a partition
            fnCreateExtendedBootRecord(iDiskNumber);
            if ((iActionResult = _utCommitSectorData[iDiskNumber](&utDisks[iDiskNumber], utDisks[iDiskNumber].ptrSectorData, 0)) != 0) {
                if (iActionResult == CARD_BUSY_WAIT) {
                    _return;                                             // write is taking time to complete so quit for the moment
                }
                iMemoryState[iDiskNumber] = DISK_NOT_FORMATTED;
                break;
            }
            #if defined _WINDOWS
            iFormatCount = 0;
            #endif
            iMemoryState[iDiskNumber] = STATE_FORMATTING_DISK_2;     // fall through
            // Fall through intentionally
            //
        case STATE_FORMATTING_DISK_2:                                    // add the boot sector
            fnCreateBootSector(iDiskNumber);
            iMemoryState[iDiskNumber] = STATE_FORMATTING_DISK_3;
            // Fall through intentional
            //
        case STATE_FORMATTING_DISK_3:
            #if defined UTFAT12 || defined UTFAT16
            if ((utDisks[iDiskNumber].usDiskFlags & (DISK_FORMAT_FAT12 | DISK_FORMAT_FAT16)) == 0) { // only valid for FAT32
            #endif
                if ((iActionResult = _utCommitSectorData[iDiskNumber](&utDisks[iDiskNumber], utDisks[iDiskNumber].ptrSectorData, BOOT_SECTOR_LOCATION)) != 0) {
                    if (iActionResult == CARD_BUSY_WAIT) {
                        _return;                                         // write is taking time to complete so quit for the moment
                    }
                    iMemoryState[iDiskNumber] = DISK_NOT_FORMATTED;
                    break;
                }
            #if defined UTFAT12 || defined UTFAT16
            }
            #endif
            iMemoryState[iDiskNumber] = STATE_FORMATTING_DISK_4;
            // Fall through intentional
            //
        case STATE_FORMATTING_DISK_4:
            {
                unsigned long ulLocation = (BOOT_SECTOR_LOCATION + BACKUP_ROOT_SECTOR);
            #if defined UTFAT12 || defined UTFAT16
                if (utDisks[iDiskNumber].usDiskFlags & (DISK_FORMAT_FAT12 | DISK_FORMAT_FAT16)) {
                    ulLocation = 0;                                      // FAT12/FAT16 have a single boot sector at the start of the memory area
                }
            #endif
                if ((iActionResult = _utCommitSectorData[iDiskNumber](&utDisks[iDiskNumber], utDisks[iDiskNumber].ptrSectorData, ulLocation)) != 0) {
                    if (iActionResult == CARD_BUSY_WAIT) {
                        _return;                                         // write is taking time to complete so quit for the moment
                    }
                    iMemoryState[iDiskNumber] = DISK_NOT_FORMATTED;
                    break;
                }
                else {
                    DIR_ENTRY_STRUCTURE_FAT32 *ptrVolumeEntry = (DIR_ENTRY_STRUCTURE_FAT32 *)utDisks[iDiskNumber].ptrSectorData;
                    uMemset(utDisks[iDiskNumber].ptrSectorData, 0, 512);
                    uMemcpy(ptrVolumeEntry->DIR_Name, utDisks[iDiskNumber].cVolumeLabel, 11); //add the volume ID
                    ptrVolumeEntry->DIR_Attr = DIR_ATTR_VOLUME_ID;
                    fnSetTimeDate(ptrVolumeEntry, 1);                    // add creation time and date
                    ulFatSectors[iDiskNumber] = 0;
                    iMemoryState[iDiskNumber] = STATE_FORMATTING_DISK_5;
                }
            }
            // Fall through intentional
            //
        case STATE_FORMATTING_DISK_5:                                    // ensure that the first boot cluster is blank (with volume ID)
            if (ulFatSectors[iDiskNumber] < utDisks[iDiskNumber].utFAT.ucSectorsPerCluster) {
                if ((iActionResult = _utCommitSectorData[iDiskNumber](&utDisks[iDiskNumber], utDisks[iDiskNumber].ptrSectorData, (ulFatSectors[iDiskNumber] + utDisks[iDiskNumber].ulLogicalBaseAddress))) != 0) {
                    if (iActionResult == CARD_BUSY_WAIT) {
                        _return;                                         // write is taking time to complete so quit for the moment
                    }
                    iMemoryState[iDiskNumber] = DISK_NOT_FORMATTED;
                    break;
                }
                ulFatSectors[iDiskNumber]++;
                uMemset(utDisks[iDiskNumber].ptrSectorData, 0, 512);
                uTaskerStateChange(OWN_TASK, UTASKER_ACTIVATE);          // schedule again to continue with next sector
                break;
            }
            iMemoryState[iDiskNumber] = STATE_FORMATTING_DISK_6;
            ulFatSectors[iDiskNumber] = 0;
            ucFatCopies[iDiskNumber] = 1;
            // Fall through intentional
            //
        case STATE_FORMATTING_DISK_6:
            #if defined UTFAT12 || defined UTFAT16
            if ((utDisks[iDiskNumber].usDiskFlags & DISK_FORMAT_FAT16) != 0) {
                #if defined UTFAT16
                uMemcpy(utDisks[iDiskNumber].ptrSectorData, ucEmptyFAT16, sizeof(ucEmptyFAT16));
                #endif
            }
                #if defined UTFAT12
            else if ((utDisks[iDiskNumber].usDiskFlags & DISK_FORMAT_FAT12) != 0) {
                uMemcpy(utDisks[iDiskNumber].ptrSectorData, ucEmptyFAT12, sizeof(ucEmptyFAT12));
            }
                #endif
            else {
                uMemcpy(utDisks[iDiskNumber].ptrSectorData, ucEmptyFAT32, sizeof(ucEmptyFAT32));
            }
            #else
            uMemcpy(utDisks[iDiskNumber].ptrSectorData, ucEmptyFAT32, sizeof(ucEmptyFAT32));
            #endif
            iMemoryState[iDiskNumber] = STATE_FORMATTING_DISK_7;
            // Fall through intentional
            //
        case STATE_FORMATTING_DISK_7:
            {
                unsigned long ulLocation = (ulFatSectors[iDiskNumber] + (BOOT_SECTOR_LOCATION + RESERVED_SECTION_COUNT));
            #if defined UTFAT12
                if (utDisks[iDiskNumber].usDiskFlags & (DISK_FORMAT_FAT12)) {
                    ulLocation = 6;
                }
            #endif
            #if defined UTFAT16
                if (utDisks[iDiskNumber].usDiskFlags & (DISK_FORMAT_FAT16)) {
                    ulLocation = 8;
                }
            #endif
                if ((iActionResult = _utCommitSectorData[iDiskNumber](&utDisks[iDiskNumber], utDisks[iDiskNumber].ptrSectorData, ulLocation)) != 0) {
                    if (iActionResult == CARD_BUSY_WAIT) {
                        _return;                                         // write is taking time to complete so quit for the moment
                    }
                    iMemoryState[iDiskNumber] = DISK_NOT_FORMATTED;
                    break;
                }
                uMemset(utDisks[iDiskNumber].ptrSectorData, 0, sizeof(ucEmptyFAT32));
                ulFatSectors[iDiskNumber]++;
                iMemoryState[iDiskNumber] = STATE_FORMATTING_DISK_8;
            }
            // Fall through intentional
            //
        case STATE_FORMATTING_DISK_8:
            if (ulFatSectors[iDiskNumber] >= (ulFAT32size[iDiskNumber] * ucFatCopies[iDiskNumber])) {
                if (ucFatCopies[iDiskNumber] < NUMBER_OF_FATS) {
                    ucFatCopies[iDiskNumber]++;
                    iMemoryState[iDiskNumber] = STATE_FORMATTING_DISK_6; // next FAT copy
                    uTaskerStateChange(OWN_TASK, UTASKER_ACTIVATE);      // schedule again to continue with next sector
                    break;
                }
            #if defined UTFAT_FULL_FORMATTING
                if (utDisks[iDiskNumber].usDiskFlags & DISK_FORMAT_FULL) {
                    utDisks[iDiskNumber].usDiskFlags &= ~DISK_FORMAT_FULL;
                    ulFatSectors[iDiskNumber] += (BOOT_SECTOR_LOCATION + RESERVED_SECTION_COUNT);
                #if defined UTFAT16 || defined UTFAT12
                    if (utDisks[iDiskNumber].usDiskFlags & (DISK_FORMAT_FAT12 | DISK_FORMAT_FAT16)) {
                        ulFatSectors[iDiskNumber] -= (BOOT_SECTOR_LOCATION + RESERVED_SECTION_COUNT - 8);
                    }
                #endif
                    iMemoryState[iDiskNumber] = STATE_FORMATTING_DISK_8A;// the FAT has been cleared but now clear out all data sectors too
                }
                else {
                    iMemoryState[iDiskNumber] = STATE_FORMATTING_DISK_9; // all FAT copies have been initialised
                }
            #else
                iMemoryState[iDiskNumber] = STATE_FORMATTING_DISK_9;     // all FAT copies have been initialised
            #endif
            }
            else {
                unsigned long ulLocation = (ulFatSectors[iDiskNumber] + (BOOT_SECTOR_LOCATION + RESERVED_SECTION_COUNT));
            #if defined UTFAT16 || defined UTFAT12
                if (utDisks[iDiskNumber].usDiskFlags & (DISK_FORMAT_FAT12 | DISK_FORMAT_FAT16)) {
                    ulLocation -= (BOOT_SECTOR_LOCATION + RESERVED_SECTION_COUNT - 8);
                }
            #endif
                if ((iActionResult = _utCommitSectorData[iDiskNumber](&utDisks[iDiskNumber], utDisks[iDiskNumber].ptrSectorData, ulLocation)) != 0) {
                    if (iActionResult == CARD_BUSY_WAIT) {
                        _return;                                         // write is taking time to complete so quit for the moment
                    }
                    iMemoryState[iDiskNumber] = DISK_NOT_FORMATTED;
                    break;
                }
                ulFatSectors[iDiskNumber]++;
                if ((ulFatSectors[iDiskNumber] % 256) == 0) {
                    fnMemoryDebugMsg("*");
                }
            #if defined _WINDOWS
                if (++iFormatCount == 0x100) {                           // accelerate simulation
                    ulFatSectors[iDiskNumber] = (ulFAT32size[iDiskNumber] * ucFatCopies[iDiskNumber]);
                    iFormatCount = 0;
                }
            #endif
            }
            uTaskerStateChange(OWN_TASK, UTASKER_ACTIVATE);              // schedule again to continue with next sector
            break;
            #if defined UTFAT_FULL_FORMATTING
        case STATE_FORMATTING_DISK_8A:                                   // delete disk data
            if (ulFatSectors[iDiskNumber] >= utDisks[iDiskNumber].ulSD_sectors) {
                iMemoryState[iDiskNumber] = STATE_FORMATTING_DISK_9;     // all disk data has been cleaned
            }
            else {
                if ((iActionResult = _utCommitSectorData[iDiskNumber](&utDisks[iDiskNumber], utDisks[iDiskNumber].ptrSectorData, ulFatSectors[iDiskNumber])) != 0) {
                    if (iActionResult == CARD_BUSY_WAIT) {
                        _return;                                         // write is taking time to complete so quit for the moment
                    }
                    iMemoryState[iDiskNumber] = DISK_NOT_FORMATTED;
                    break;
                }
                ulFatSectors[iDiskNumber]++;
                if ((ulFatSectors[iDiskNumber] % 256) == 0) {
                    fnMemoryDebugMsg("X");
                }
                #if defined _WINDOWS
                if (++iFormatCount == 0x200) {                           // accelerate simulation
                    ulFatSectors[iDiskNumber] = (utDisks[iDiskNumber].utFAT.ulFAT_start + utDisks[iDiskNumber].utFAT.ulFatSize + (utDisks[iDiskNumber].utFAT.ulClusterCount * (utDisks[iDiskNumber].utFAT.usBytesPerSector/512)));
                    iFormatCount = 0;
                }
                #endif
                uTaskerStateChange(OWN_TASK, UTASKER_ACTIVATE);          // schedule again to continue with next sector
                break;
            }
            #endif
        case STATE_FORMATTING_DISK_9:                                    // finally create the FAT32 info record
            {
            #if defined UTFAT16 || defined UTFAT12
                if (utDisks[iDiskNumber].usDiskFlags & (DISK_FORMAT_FAT12 | DISK_FORMAT_FAT16)) {
                    fnMemoryDebugMsg("Disk formatted FAT1");
                    if (utDisks[iDiskNumber].usDiskFlags & DISK_FORMAT_FAT12) {
                #if defined UTFAT12
                        fnMemoryDebugMsg("2\r\n");
                #endif
                    }
                #if defined UTFAT16
                    else {
                        fnMemoryDebugMsg("6\r\n");
                    }
                #endif
                    iMemoryState[iDiskNumber] = DISK_MOUNTING_1;
                    uTaskerStateChange(OWN_TASK, UTASKER_ACTIVATE);      // schedule again to mount the disk which has just been formatted
                    break;
                }
            #endif
            #if defined SDCARD_SUPPORT || defined USB_MSD_HOST
                if
                #if defined SDCARD_SUPPORT
                    (DISK_SDCARD == iDiskNumber)
                #else
                    (DISK_MEM_STICK == iDiskNumber)
                #endif
                {
                    fnCreateInfoSector(iDiskNumber);
                    if ((iActionResult = _utCommitSectorData[iDiskNumber](&utDisks[iDiskNumber], utDisks[iDiskNumber].ptrSectorData, (BOOT_SECTOR_LOCATION + 1))) != 0) { // write info sector
                        if (iActionResult == CARD_BUSY_WAIT) {
                            _return;                                     // write is taking time to complete so quit for the moment
                        }
                        iMemoryState[iDiskNumber] = DISK_NOT_FORMATTED;
                        break;
                    }
                    utDisks[iDiskNumber].usDiskFlags |= FSINFO_VALID;
                }
            #endif
                iMemoryState[iDiskNumber] = STATE_FORMATTING_DISK_10;
        case STATE_FORMATTING_DISK_10:                                   // backup
                if ((iActionResult = _utCommitSectorData[iDiskNumber](&utDisks[iDiskNumber], utDisks[iDiskNumber].ptrSectorData, (BOOT_SECTOR_LOCATION + 1 + BACKUP_ROOT_SECTOR))) != 0) { // write info sector backup
                    if (iActionResult == CARD_BUSY_WAIT) {
                        _return;                                         // write is taking time to complete so quit for the moment
                    }
                    iMemoryState[iDiskNumber] = DISK_NOT_FORMATTED;
                    break;
                }
                fnMemoryDebugMsg("Disk formatted\r\n");
                iMemoryState[iDiskNumber] = DISK_MOUNTING_1;
                uTaskerStateChange(OWN_TASK, UTASKER_ACTIVATE);          // schedule again to mount the disk which has just been formatted
            }
            break;
        #endif                                                           // end #if defined UTFAT_WRITE && defined UTFAT_FORMATTING
        }
    }
    else if (iMemoryOperation[iDiskNumber] & _COUNTING_CLUSTERS) {       // free cluster counting in progress
#if defined FAT12_DEVELOPMENT
        if (ulClusterSectorCheck[iDiskNumber] < (utDisks[iDiskNumber].utFAT.ulFAT_start + utDisks[iDiskNumber].utFAT.ulFatSize))
#else
        if (ulClusterSectorCheck[iDiskNumber] >= utDisks[iDiskNumber].utFAT.ulFAT_start)
#endif
        {
#if defined FAT12_DEVELOPMENT
            unsigned long ulRepetitions;
#else
            int ulRepetitions = 10;                                      // read maximum 10 sectors at a time
#endif
            unsigned long ulSectorContent[512/sizeof(signed long)];      // long word aligned FAT sector memory
#if defined FAT12_DEVELOPMENT
            ulRepetitions = ((utDisks[iDiskNumber].utFAT.ulFAT_start + utDisks[iDiskNumber].utFAT.ulFatSize) - ulClusterSectorCheck[iDiskNumber]); // the remaining number of clusters to be checked
            if (ulRepetitions > 10) {                                    // limit to a maximum 10 sectors at a time
                ulRepetitions = 10;
            }
#else
            if (ulClusterSectorCheck[iDiskNumber] < (iRepetitions + utDisks[iDiskNumber].utFAT.ulFAT_start)) {
                ulRepetitions = (ulClusterSectorCheck[iDiskNumber] - utDisks[iDiskNumber].utFAT.ulFAT_start);
            }
#endif
            while (ulRepetitions-- > 0) {
#if defined FAT12_DEVELOPMENT
                if ((_utReadDiskSector[iDiskNumber](&utDisks[iDiskNumber], ulClusterSectorCheck[iDiskNumber]++, ulSectorContent)) == UTFAT_SUCCESS) // read a FAT sector containing the cluster information
#else
                if ((_utReadDiskSector[iDiskNumber](&utDisks[iDiskNumber], ulClusterSectorCheck[iDiskNumber]--, ulSectorContent)) == UTFAT_SUCCESS) // read a FAT sector containing the cluster information
#endif
                {
        #if defined UTFAT16 || defined UTFAT12
                    if (utDisks[iDiskNumber].usDiskFlags & (DISK_FORMAT_FAT16)) {
            #if defined UTFAT16
                        int i = 0;                                       // count free FAT16 clusters
                        while (i < (512/sizeof(signed long))) {          // search through the FAT sector
                            if (ulSectorContent[i] == 0) {               // a value of 0x00000000 represents two free clusters
                                ulActiveFreeClusterCount[iDiskNumber] += 2;
                            }
                            else if (((unsigned short)(ulSectorContent[i]) == 0) || ((unsigned short)(ulSectorContent[i] >> 16) == 0)) {
                                ulActiveFreeClusterCount[iDiskNumber]++;
                            }
                            i++;
                        }
            #endif
                    }
            #if defined UTFAT12
                    else if (utDisks[iDiskNumber].usDiskFlags & (DISK_FORMAT_FAT12)) {
                        unsigned long ulNextValue;
                        int i = 0;                                       // count free FAT12 clusters
                        int iFat12_tripplet = fnGetFat12_tripplet(ulClusterSectorCheck[iDiskNumber] - utDisks[iDiskNumber].utFAT.ulFAT_start - 1);
                        while (i < (512/sizeof(signed long))) {          // search through the FAT sector
                            ulNextValue = LITTLE_LONG_WORD(ulSectorContent[i]); // processing is performed as little-endian
                            switch (iFat12_tripplet) {
                            case 0:
                                if ((ulNextValue & 0x00000fff) == 0) {
                                    ulActiveFreeClusterCount[iDiskNumber]++;
                                }
                                if (--ulMaximumClusterCount[iDiskNumber] == 0) {
                                    i = 512;
                                    break;
                                }
                                if ((ulNextValue & 0x00fff000) == 0) {
                                    ulActiveFreeClusterCount[iDiskNumber]++;
                                }
                                if (--ulMaximumClusterCount[iDiskNumber] == 0) {
                                    i = 512;
                                    break;
                                }
                                ulLastFAT12_value[iDiskNumber] = ulNextValue;
                                iFat12_tripplet = 1;
                                break;
                            case 1:
                                if (((ulNextValue & 0x0000000f) | (ulLastFAT12_value[iDiskNumber] & 0xff000000)) == 0) {
                                    ulActiveFreeClusterCount[iDiskNumber]++;
                                }
                                if (--ulMaximumClusterCount[iDiskNumber] == 0) {
                                    i = 512;
                                    break;
                                }
                                if ((ulNextValue & 0x0000fff0) == 0) {
                                    ulActiveFreeClusterCount[iDiskNumber]++;
                                }
                                if (--ulMaximumClusterCount[iDiskNumber] == 0) {
                                    i = 512;
                                    break;
                                }
                                if ((ulNextValue & 0x0fff0000) == 0) {
                                    ulActiveFreeClusterCount[iDiskNumber]++;
                                }
                                if (--ulMaximumClusterCount[iDiskNumber] == 0) {
                                    i = 512;
                                    break;
                                }
                                ulLastFAT12_value[iDiskNumber] = ulNextValue;
                                iFat12_tripplet = 2;
                                break;
                            case 2:
                                if (((ulNextValue & 0x000000ff) | (ulLastFAT12_value[iDiskNumber] & 0xf0000000)) == 0) {
                                    ulActiveFreeClusterCount[iDiskNumber]++;
                                }
                                if (--ulMaximumClusterCount[iDiskNumber] == 0) {
                                    i = 512;
                                    break;
                                }
                                if ((ulNextValue & 0x000fff00) == 0) {
                                    ulActiveFreeClusterCount[iDiskNumber]++;
                                }
                                if (--ulMaximumClusterCount[iDiskNumber] == 0) {
                                    i = 512;
                                    break;
                                }
                                if ((ulNextValue & 0xfff00000) == 0) {
                                    ulActiveFreeClusterCount[iDiskNumber]++;
                                }
                                if (--ulMaximumClusterCount[iDiskNumber] == 0) {
                                    i = 512;
                                    break;
                                }
                                iFat12_tripplet = 0;
                                break;
                            }
                            i++;
                        }
                    }
            #endif
                    else {
         #endif
                        int i = 0;                                       // count free FAT32 clusters
                        while (i < (512/sizeof(signed long))) {          // search through the FAT sector
                            if (ulSectorContent[i++] == 0) {             // a value of 0x00000000 represents a free cluster
                                ulActiveFreeClusterCount[iDiskNumber]++; // increment the total count of free clusters found
                            }
                        }
        #if defined UTFAT16 || defined UTFAT12
                    }
        #endif
                }
            }
            uTaskerStateChange(OWN_TASK, UTASKER_ACTIVATE);              // schedule to run again
        }
        else {
            iMemoryOperation[iDiskNumber] &= ~_COUNTING_CLUSTERS;        // cluster counting has completed
            utDisks[iDiskNumber].utFileInfo.ulFreeClusterCount = ulActiveFreeClusterCount[iDiskNumber];
            fnInterruptMessage(cluster_task[iDiskNumber], UTFAT_OPERATION_COMPLETED); // inform that the cluster count has completed
            cluster_task[iDiskNumber] = 0;
        }
    }
        #if DISK_COUNT > 1
    }                                                                    // end of while for each disk
        #endif
    #endif                                                               // end #if defined SDCARD_SUPPORT || defined USB_MSD_HOST

    // Interrupt and timer events are onyl valid for the SD card interface
    //
    while (fnRead(ptrTaskTable->TaskID, ucInputMessage, HEADER_LENGTH)) {// check task input queue
        switch (ucInputMessage[MSG_SOURCE_TASK]) {
        case TIMER_EVENT:
    #if defined NAND_FLASH_FAT
            if (E_CLEAN_SPARE == ucInputMessage[MSG_TIMER_EVENT]) {
                fnCleanNAND();                                           // if there are area of the NAND Flash marked to be cleaned do the work as a background operation
                break;
            }
    #endif
    #if defined T_CHECK_CARD_REMOVAL
            if (E_CHECK_CARD_REMOVAL == ucInputMessage[MSG_TIMER_EVENT]) { // time to check whether the card is responding
                if (DISK_STATE_READY == iMemoryState[DISK_SDCARD]) {     // only perform removal check when the card is in the ready state - don't disturb operation when formatting or mounting
                    iMemoryState[DISK_SDCARD] = STATE_CHECKING_DISK_REMOVAL; // set state to cause next check
                    iMemoryOperation[DISK_SDCARD] &= ~(_INITIALISED_MEMORY);
                }
                else {
                    uTaskerMonoTimer(OWN_TASK, T_CHECK_CARD_REMOVAL, E_CHECK_CARD_REMOVAL); // poll the SD card to detect removal
                }
                uTaskerStateChange(OWN_TASK, UTASKER_ACTIVATE);          // schedule the task to perform a check to see whether the card is responding (or has been removed)
            }
    #endif
    #if defined MANAGED_FILES && defined TIME_SLICED_FILE_OPERATION
			if (ucInputMessage[MSG_TIMER_EVENT] & _DELAYED_DELETE) {
                uFileManagedDelete(ucInputMessage[MSG_TIMER_EVENT] & ~(_DELAYED_DELETE));
                break;
			}
        #if defined MANAGED_FILE_WRITE
            else if (ucInputMessage[MSG_TIMER_EVENT] & _DELAYED_WRITE) {
                uFileManagedWrite(ucInputMessage[MSG_TIMER_EVENT] & ~(_DELAYED_WRITE));
            }
        #endif
        #if defined MANAGED_FILE_READ
            else if (ucInputMessage[MSG_TIMER_EVENT] & _DELAYED_READ) {
                uFileManagedRead(ucInputMessage[MSG_TIMER_EVENT] & ~(_DELAYED_READ));
            }
        #endif
    #endif
            break;

    #if defined SDCARD_DETECT_INPUT_INTERRUPT
        case INTERRUPT_EVENT:                                            // new state of SD card detection
            if (E_SDCARD_DETECTION_CHANGE == ucInputMessage[MSG_INTERRUPT_EVENT]) {
                if (SDCARD_DETECTION() != 0) {                           // card has been inserted so try to mount it
                                                                         // do nothing in this case since the interrupt event will have already started mounting attempt
        #if defined _WINDOWS
                    SD_card_state(SDCARD_INSERTED, SDCARD_REMOVED);
        #endif
                }
                else {                                                   // card has been removed so stop operation
                    fnInitialisationError(DISK_SDCARD, 0);
                }
        #if defined SDCARD_SINGLE_EDGE_INTERRUPT
                fnPrepareDetectInterrupt();                              // devices that support interrupt one just one edge need to change the interrupt polarity now
        #endif
            }
            break;
    #endif

    #if defined USB_MSD_HOST
        case TASK_USB_HOST:
            fnRead(ptrTaskTable->TaskID, ucInputMessage, ucInputMessage[MSG_CONTENT_LENGTH]); // read the message content
            if (ucInputMessage[0] == MOUNT_USB_MSD) {                    // we should start mounting a memory stick that is ready to be worked with (DISK_E)
                if (iMemoryState[DISK_E] <= DISK_MOUNTING_1) {           // don't disturb if already mounting or being mounted
                    if (utDisks[DISK_E].ptrSectorData == 0) {            // if sector memory is not yet allocated
                        utDisks[DISK_E].ptrSectorData = SDCARD_MALLOC(512); // allocate a buffer which will contain data read from the present sector (this should be long word aligned and possibly in a specific memory in case DMA operation is to be used)
                    }
                    utDisks[DISK_E].ucDriveNumber = DISK_E;
                    iMemoryState[DISK_E] = DISK_MOUNTING_1;              // set to state initiating the mounting sequence
                    uTaskerStateChange(OWN_TASK, UTASKER_ACTIVATE);      // schedule the task to start mounting
                    fnDebugMsg("Mem-Stick mounting...\r\n");
                }
            }
            break;
    #endif
        }
    }
}
#endif                                                                   // end #if defined SDCARD_SUPPORT || defined MANAGED_FILES


#if defined SDCARD_SUPPORT || defined SPI_FLASH_FAT || defined FLASH_FAT || defined USB_MSD_HOST
static void fnCardNotFormatted(int iDisk)
{
    utDisks[iDisk].usDiskFlags |= DISK_UNFORMATTED;
    iMemoryState[iDisk] = DISK_NOT_FORMATTED;
    fnMemoryDebugMsg("Non-formatted ");
    #if defined DISK_SPI_FLASH
    if (iDisk == DISK_SPI_FLASH) {
        fnMemoryDebugMsg("SPI flash\r\n");
    }
    #endif
    #if defined DISK_SDCARD
    if (iDisk == DISK_SDCARD) {
        fnMemoryDebugMsg("SD-card\r\n");
        #if defined _WINDOWS && defined SDCARD_SUPPORT
        SD_card_state(SDCARD_MOUNTED, (SDCARD_FORMATTED_32 | SDCARD_FORMATTED_16));
        #endif
    }
    #endif
    #if defined DISK_INTERNAL_FLASH
    if (iDisk == DISK_INTERNAL_FLASH) {
        fnMemoryDebugMsg("internal flash\r\n");
    }
    #endif
    #if defined DISK_MEM_STICK
    if (iDisk == DISK_MEM_STICK) {
        fnMemoryDebugMsg("Memory stick\r\n");
    }
    #endif
}

// Interrupt initisalisation sequence on error, remove power and start next try after a delay
//
static void fnInitialisationError(int iDisk, int iNotSupported)
{
    #if defined SDCARD_SUPPORT
    SET_SD_CS_HIGH();
    POWER_DOWN_SD_CARD();
    iMemoryState[iDisk] = SD_STATE_WAIT_CARD;                            // wait for next check of valid card
        #if !defined SDCARD_DETECT_INPUT_INTERRUPT
    uTaskerMonoTimer(OWN_TASK, T_NEXT_CHECK, E_POLL_SD_CARD);            // try again later
        #endif
    if (iNotSupported != 0) {
        utDisks[iDisk].usDiskFlags = DISK_TYPE_NOT_SUPPORTED;
        #if defined _WINDOWS
        SD_card_state(0, (SDCARD_FORMATTED_32 | SDCARD_FORMATTED_16));
        #endif
    }
    else {
        fnMemoryDebugMsg("SD-card not responding\r\n");
        utDisks[iDisk].usDiskFlags = DISK_NOT_PRESENT;
        iMemoryOperation[iDisk] = _IDLE_MEMORY;
        #if defined _WINDOWS
        SD_card_state(0, (SDCARD_INSERTED | SDCARD_MOUNTED | SDCARD_FORMATTED_32 | SDCARD_FORMATTED_16 | SDCARD_WR_PROTECTED));
        #endif
    }
    #endif
}

#if defined SDCARD_SUPPORT
static const unsigned char *fnCreateCommand(unsigned char ucCommand, unsigned long ulValue)
{
    static unsigned char ucCommandBuffer[6];                             // space for a permanent command
    ucCommandBuffer[0] = ucCommand;
    ucCommandBuffer[1] = (unsigned char)(ulValue >> 24);
    ucCommandBuffer[2] = (unsigned char)(ulValue >> 16);
    ucCommandBuffer[3] = (unsigned char)(ulValue >> 8);
    ucCommandBuffer[4] = (unsigned char)(ulValue);
    ucCommandBuffer[5] = 0;                                              // dummy CRC
    return (const unsigned char *)ucCommandBuffer;
}

    #if !defined SD_CONTROLLER_AVAILABLE                                // SPI interface is local - SD card interface uses HW specific external routines
// Read a sector from SD card into the specified data buffer
//
static int fnGetSector(unsigned char *ptrBuf)
{
    unsigned char ucResponse;
    int iLength = 512;
    do {
        WRITE_SPI_CMD(0xff);
        WAIT_TRANSMISSON_END();                                          // wait until transmission complete
    } while ((ucResponse = READ_SPI_DATA()) == 0xff);
    if (ucResponse != 0xfe) {
        return UTFAT_DISK_READ_ERROR;                                    // error
    }
    do {
        WRITE_SPI_CMD(0xff);
        WAIT_TRANSMISSON_END();                                          // wait until transmission complete
        *ptrBuf++ = READ_SPI_DATA();
    } while (--iLength);
    WRITE_SPI_CMD(0xff);                                                 // read and discard two CRC bytes
    WAIT_TRANSMISSON_END();                                              // wait until transmission complete
    ucResponse = READ_SPI_DATA();
    WRITE_SPI_CMD(0xff);
    WAIT_TRANSMISSON_END();                                              // wait until transmission complete
    ucResponse = READ_SPI_DATA();
    return UTFAT_SUCCESS;                                                // read successfully terminated
}

// Read a specified amount of data from present SD card sector into the specified data buffer (usStart and usStop are offset from start of sector and avoid other data outside of this range being overwritted)
//
static int fnReadPartialSector(unsigned char *ptrBuf, unsigned short usStart, unsigned short usStop)
{
    unsigned char ucResponse;
    unsigned short usLoopCounter = 0;
    // this may block so a timer may be required to protect it (100ms?)
    do {
        WRITE_SPI_CMD(0xff);
        WAIT_TRANSMISSON_END();                                          // wait until transmission complete
    } while ((ucResponse = READ_SPI_DATA()) == 0xff);
    if (ucResponse != 0xfe) {
        fnMemoryDebugMsg("ERR-1!!!\r\n");
        return 0;                                                        // error
    }
    while (usLoopCounter < usStart) {
        WRITE_SPI_CMD(0xff);
        usLoopCounter++;
        WAIT_TRANSMISSON_END();                                          // wait until transmission complete
        (void)READ_SPI_DATA();                                           // discard unwanted data at the start of the sector
    }
    while (usLoopCounter < usStop) {
        WRITE_SPI_CMD(0xff);
        usLoopCounter++;
        WAIT_TRANSMISSON_END();                                          // wait until transmission complete
        *ptrBuf++ = READ_SPI_DATA();
    }
    while (usLoopCounter < 512) {
        WRITE_SPI_CMD(0xff);
        usLoopCounter++;
        WAIT_TRANSMISSON_END();                                          // wait until transmission complete
        (void)READ_SPI_DATA();                                           // discard any additional data, but always read a full 512 byte block
    }
    WRITE_SPI_CMD(0xff);                                                 // read and discard two CRC bytes
    WAIT_TRANSMISSON_END();                                              // wait until transmission complete
    ucResponse = READ_SPI_DATA();
    WRITE_SPI_CMD(0xff);
    WAIT_TRANSMISSON_END();                                              // wait until transmission complete
    ucResponse = READ_SPI_DATA();
    return UTFAT_SUCCESS;                                                // read terminated
}

// Send a command to the SD-card and return a result, plus optional returned arguments
//
static int fnSendSD_command(const unsigned char ucCommand[6], unsigned char *ucResult, unsigned char *ptrReturnData)
{
    static int iCommandYieldCount = 0;
    static int iCommandState = 0;
    int iRtn = UTFAT_SUCCESS;
    if (iCommandYieldCount > CommandTimeout) {                           // the present access is taking too long - quit with SD card error
        fnMemoryDebugMsg("TIMEOUT!!!\r\n");
        iRtn = ERROR_CARD_TIMEOUT;
    }
    else {
        switch (iCommandState) {
        case 0:
            if (fnWaitSD_ready(20) != 0) {                               // poll up to 20 times before yielding
                iCommandYieldCount++;                                    // monitor the maximum delay
                uTaskerStateChange(OWN_TASK, UTASKER_GO);                // switch to polling mode of operation
                return CARD_BUSY_WAIT;
            }  
            for ( ; iCommandState < 6; iCommandState++) {                // command content is always 6 bytes in length
                WRITE_SPI_CMD(ucCommand[iCommandState]);                 // send the command and arguments
                WAIT_TRANSMISSON_END();                                  // wait until transmission complete
                (void)READ_SPI_DATA();                                   // read to clear the flag
            }                                                            // fall through intentional after sending the command
            if (iCommandYieldCount != 0) {
                uTaskerStateChange(OWN_TASK, UTASKER_STOP);              // switch to event mode if operation is continuing
                iCommandYieldCount = 0;
            }
        case 6:
            do {
                if (iCommandState++ > 26) {
                    iCommandYieldCount++;                                // monitor the maximum delay
                    iCommandState = 6;
                    uTaskerStateChange(OWN_TASK, UTASKER_GO);            // switch to polling mode of operation
                    return CARD_BUSY_WAIT;                               // poll up to 20 times before yielding
                }
                WRITE_SPI_CMD(0xff);                                     // send idle line
                WAIT_TRANSMISSON_END();                                  // wait until transmission complete
                *ucResult = READ_SPI_DATA();                             // read result byte
            } while (*ucResult & SD_CARD_BUSY);                          // poll the card until it is no longer indicating busy and returns the value
            if (ptrReturnData != 0) {                                    // if the caller requests data, read it here
                int iReturnLength;
                if (ucCommand[0] == SEND_CSD_CMD9) {                     // exception requiring 18 bytes
                    iReturnLength = 18;
                }
                else {
                    iReturnLength = 4;
                }
                for (iCommandState = 0; iCommandState < iReturnLength; iCommandState++) {
                    WRITE_SPI_CMD(0xff);                                 // send the command and arguments
                    WAIT_TRANSMISSON_END();                              // wait until transmission complete
                    *ptrReturnData++ = READ_SPI_DATA();
                }
            }
            break;
        }
    }
    if (iCommandYieldCount != 0) {
        uTaskerStateChange(OWN_TASK, UTASKER_STOP);                      // switch to event mode of operation since write has completed
        iCommandYieldCount = 0;
    }
    iCommandState = 0;
    return iRtn;
}

// Wait until the SD card is ready by reading until 0xff is returned
//
static int fnWaitSD_ready(int iMaxWait)
{
    do {
        if (iMaxWait-- == 0) {
            return 1;                                                    // maximum wait attempts executed
        }
        WRITE_SPI_CMD(0xff);
        WAIT_TRANSMISSON_END();                                          // wait until transmission complete
    } while (READ_SPI_DATA() != 0xff);                                   // 0xff expected when the device is ready
    return 0;                                                            // the device is now ready
}
    #endif
#endif

#if defined NAND_FLASH_FAT

// Read a part of the specified sector to the buffer (avoiding overwriting all buffer content)
//
static int utReadPartialNAND(UTDISK *ptr_utDisk, unsigned long ulSectorNumber, void *ptrBuf, unsigned short usOffset, unsigned short usLength)
{
    static unsigned long ulSector;
    switch (iMemoryOperation[DISK_D] & _READING_MEMORY) {
    case _IDLE_MEMORY:
        iMemoryOperation[DISK_D] |= _READING_MEMORY;
        ulSector = ulSectorNumber;
    case _READING_MEMORY:
        {
            unsigned char ucTemp[512];
            if (fnReadNANDsector(ulSectorNumber, 0, ucTemp, 512) != 0) {
                fnMemoryDebugMsg(" - ECC check failed\r\n");             // this message can be ignored if the page is blank since it will not have a valid ECC
            }
            uMemcpy(ptrBuf, &ucTemp[usOffset], usLength);
            iMemoryOperation[DISK_D] &= ~_READING_MEMORY;                // read operation has completed
        }
        break;
    }
    return UTFAT_SUCCESS;
}

// Read a single, complete sector from the disk to the specified buffer
//
static int utReadDiskSector(UTDISK *ptr_utDisk, unsigned long ulSectorNumber, void *ptrBuf)
{
    static unsigned long ulSector;
    switch (iMemoryOperation[DISK_D] & _READING_MEMORY) {
    case _IDLE_MEMORY:
        iMemoryOperation[DISK_D] |= _READING_MEMORY;
        ulSector = ulSectorNumber;
    case _READING_MEMORY:
        {
            if (ptr_utDisk->usDiskFlags & DISK_TEST_MODE) {
                _fnReadNANDdata((unsigned short)(ulSectorNumber/NAND_PAGES_IN_BLOCK), (unsigned char)(ulSectorNumber%NAND_PAGES_IN_BLOCK), 0, ptrBuf, 528);
            }
            else {
                if (fnReadNANDsector(ulSectorNumber, 0, ptrBuf, 512) != 0) {
                    fnMemoryDebugMsg(" - ECC check failed\r\n");         // this message can be ignored if the page is blank since it will not have a valid ECC
                }
            }
            iMemoryOperation[DISK_D] &= ~_READING_MEMORY;                // read operation has completed
        }
        break;
    }
    return UTFAT_SUCCESS;
}
#endif

// Read one sector from the disk specified by the drive number
//
static int ut_read_disk(UTDISK *ptr_utDisk)
{
    int iRtn = _utReadDiskSector[ptr_utDisk->ucDriveNumber](ptr_utDisk, ptr_utDisk->ulPresentSector, ptr_utDisk->ptrSectorData); // read specified sector to buffer
    if (iRtn == UTFAT_SUCCESS) {                                         // read from SD media was successful and we expect to have the boot sector in the buffer
        BOOT_SECTOR_FAT32 *ptrBootSector = (BOOT_SECTOR_FAT32 *)ptr_utDisk->ptrSectorData;
        if ((ptrBootSector->ucCheck55 != 0x55) || (ptrBootSector->ucCheckAA != 0xaa)) {
            return ERROR_SECTOR_INVALID;                                 // the sector data is not valid
        }
    }
    return iRtn;
}

#if defined UTFAT_LFN_READ
// This routine checks through a chain of long file name entries to see whether there is a match with the reference file or directory name
// It is called multiple times, once for each possible entry
//
static int fnMatchLongName(OPEN_FILE_BLOCK *ptrOpenBlock, LFN_ENTRY_STRUCTURE_FAT32 *ptrLFN_entry, int iDeletedEntry, int *ptrSkip)
{
    int iReturn = MATCH_CONTINUE;
    if ((iDeletedEntry == 0) && (ptrLFN_entry->LFN_EntryNumber == DIR_NAME_FREE)) { // deleted entry when searching for valid entry
        iReturn = ENTRY_DELETED;
    }
    else {
        if (iDeletedEntry != 0) {                                        // searching for deleted entry
            if (ptrLFN_entry->LFN_EntryNumber == 0) {                    // ensure end of directory is detected
                iDeletedEntry = 0;
            }
            else if ((ptrLFN_entry->LFN_Zero0 != 0) || (ptrLFN_entry->LFN_Zero1 != 0) || (ptrLFN_entry->LFN_Zero2 != 0)) { // check whether it is clearly not a LFN
                iDeletedEntry = 0;
            }
        }
        if ((iDeletedEntry == 0) && ((ptrLFN_entry->LFN_Attribute & DIR_ATTR_MASK) != DIR_ATTR_LONG_NAME)) { // check whether this is a long file name entry
            if (ptrLFN_entry->LFN_EntryNumber == 0) {                    // end of directory entries reached
                if (DIR_ATTR_VOLUME_ID == (ptrLFN_entry->LFN_Attribute & DIR_ATTR_MASK)) {
                    iReturn = ENTRY_VOLUME_ID;
                }
                else {
                    iReturn = END_DIRECTORY_ENTRIES;
                }
            }
            else {
    #if defined UTFAT_EXPERT_FUNCTIONS
                if (ptrLFN_entry->LFN_EntryNumber == DIR_NAME_FREE) {    // possibly the deleted SFN alias after a deleted LFN
                    // It is not possible to check the short frame alias checksum since it has been deleted but if the name strings match we accept it
                    //
                    if (ptrOpenBlock->ptrFileNameMatch == ptrOpenBlock->ptrFileNameStart) {
                        ptrOpenBlock->ptrFileNameMatch = (CHAR *)ptrOpenBlock->ptrFileNameEnd; // reset in case it needs to be used again
                        return DELETED_LFN_MATCH_SUCCESSFUL;             // complete match has been verified
                    }
                }
    #endif
                iReturn = MATCH_NOT_LFN;                                 // this is not a long file name entry so ignore it
            }
            if (ptrLFN_entry->LFN_Attribute != DIR_ATTR_VOLUME_ID) {     // as long as the short file name is for a directory or file we add it to the SFN cache
                if (ptrOpenBlock->ulSFN_found < SFN_ENTRY_CACHE_SIZE) {  // as long as the cache has not been filled
                    uMemcpy(ptrOpenBlock->cSFN_entry[ptrOpenBlock->ulSFN_found++], ptrLFN_entry, 11); // copy SFN to cache and increment cache entry count
                }
            }
        }
        else {                                                           // LFN entry found
            int iLFN_start;
            int i = 13;                                                  // there are maximum 13 characters in an entry
            unsigned char ucCharacter = 0;
            unsigned char ucMatchCharacter;
    #if defined UTFAT_EXPERT_FUNCTIONS
            if ((ptrLFN_entry->LFN_EntryNumber & 0x40) && ((ptrLFN_entry->LFN_EntryNumber != DIR_NAME_FREE) || (ptrOpenBlock->ptrFileNameMatch == (CHAR *)ptrOpenBlock->ptrFileNameEnd)))
    #else
            if (ptrLFN_entry->LFN_EntryNumber & 0x40)                    // start of long file name (in case detected in the middle of a long file name)
    #endif
            {
                iLFN_start = 1;
                ptrOpenBlock->ptrFileNameMatch = (CHAR *)ptrOpenBlock->ptrFileNameEnd; // reset the search string
            }
            else {
                iLFN_start = 0;
            }

            while (--i >= 0) {                                           // this assumes only English character set
                switch (i) {
                case 12:
                    ucCharacter = ptrLFN_entry->LFN_Name_12;
                    break;
                case 11:
                    ucCharacter = ptrLFN_entry->LFN_Name_11;
                    break;
                case 10:
                    ucCharacter = ptrLFN_entry->LFN_Name_10;
                    break;
                case 9:
                    ucCharacter = ptrLFN_entry->LFN_Name_9;
                    break;
                case 8:
                    ucCharacter = ptrLFN_entry->LFN_Name_8;
                    break;
                case 7:
                    ucCharacter = ptrLFN_entry->LFN_Name_7;
                    break;
                case 6:
                    ucCharacter = ptrLFN_entry->LFN_Name_6;
                    break;
                case 5:
                    ucCharacter = ptrLFN_entry->LFN_Name_5;
                    break;
                case 4:
                    ucCharacter = ptrLFN_entry->LFN_Name_4;
                    break;
                case 3:
                    ucCharacter = ptrLFN_entry->LFN_Name_3;
                    break;
                case 2:
                    ucCharacter = ptrLFN_entry->LFN_Name_2;
                    break;
                case 1:
                    ucCharacter = ptrLFN_entry->LFN_Name_1;
                    break;
                case 0:
                    ucCharacter = ptrLFN_entry->LFN_Name_0;
                    break;
                }
                if (ucCharacter == 0xff) {                               // past end of entry
                    continue;
                }
                if (ucCharacter == 0x00) {                               // null-terminator
                    continue;
                }
                ucMatchCharacter = *(--(ptrOpenBlock->ptrFileNameMatch));// the character that is to be matched
                if (ucCharacter != ucMatchCharacter) {                   // check the match, working backward, with pre-decrement
                    if ((ucCharacter >= 'A') && (ucCharacter <= 'Z')) {
                        ucCharacter += ('a' - 'A');
                    }
                    else if ((ucCharacter >= 'a') && (ucCharacter <= 'z')) {
                        ucMatchCharacter += ('a' - 'A');
                    }
                    else {
                        iReturn = MATCH_FALSE;
                        break;
                    }
                    if (ucCharacter != ucMatchCharacter) {               // last chance for case-insensitive match
                        iReturn = MATCH_FALSE;
                        break;
                    }
                }
            }

            if (iReturn != MATCH_FALSE) {                                // if no errors (yet)
                if (iLFN_start != 0) {                                   // start of long file name
    #if defined UTFAT_LFN_READ && ((defined UTFAT_LFN_DELETE || defined UTFAT_LFN_WRITE) || defined UTFAT_EXPERT_FUNCTIONS)
                    uMemcpy(&ptrOpenBlock->lfn_file_location, &ptrOpenBlock->present_location, sizeof(ptrOpenBlock->lfn_file_location)); // the directory location where the long file name entry chain begins
                    ptrOpenBlock->ucLFN_entries = 0;                     // reset LFN entry counter
    #endif
                    ptrOpenBlock->ucSFN_alias_checksum = ptrLFN_entry->LFN_Checksum; // the same checksum is expected in each entry in the long file name part
                }
                else if (ptrOpenBlock->ucSFN_alias_checksum != ptrLFN_entry->LFN_Checksum) { // if the checksum is not correct in the long file name chain reject it
                    iReturn = MATCH_FALSE;
                }
                if (iReturn != MATCH_FALSE) {
    #if defined UTFAT_LFN_READ && ((defined UTFAT_LFN_DELETE || defined UTFAT_LFN_WRITE) || defined UTFAT_EXPERT_FUNCTIONS)
                    ptrOpenBlock->ucLFN_entries++;                       // one more LFN entry counted belonging to this file name
    #endif
                    if ((ptrLFN_entry->LFN_EntryNumber & 0x3f) == 1) {   // the end of the long file name reached (this is never true when matching a deleted LFN))
                        if (ptrOpenBlock->ptrFileNameMatch == ptrOpenBlock->ptrFileNameStart) { // check whether complete name has been verified
                            ptrOpenBlock->ptrFileNameMatch = (CHAR *)ptrOpenBlock->ptrFileNameEnd; // reset in case it needs to be used again
                            if ((iDeletedEntry != 0) && (ptrLFN_entry->LFN_EntryNumber != DIR_NAME_FREE)) { // matching deleted files
                                return MATCH_FALSE;
                            }
                            return MATCH_SUCCESSFUL;                     // complete match has been verified
                        }
                        iReturn =  MATCH_FALSE;
                    }
                    else if (ptrOpenBlock->ptrFileNameMatch < ptrOpenBlock->ptrFileNameStart) {
                        iReturn =  MATCH_FALSE;
                    }
                }
            }
        }
    }

    if (iReturn < MATCH_CONTINUE) {                                      // if an error was detected
        ptrOpenBlock->ptrFileNameMatch = (CHAR *)ptrOpenBlock->ptrFileNameEnd; // reset on mis-matches or errors
    #if defined UTFAT_LFN_READ && ((defined UTFAT_LFN_DELETE || defined UTFAT_LFN_WRITE) || defined UTFAT_EXPERT_FUNCTIONS)
        ptrOpenBlock->lfn_file_location.directory_location.ulSector = 0; // mark as not LFN
    #endif
        if ((iDeletedEntry == 0) && (MATCH_FALSE == iReturn)) {          // non-matching LFN when matching valid entres
            *ptrSkip = (ptrLFN_entry->LFN_EntryNumber & 0x3f);           // the number of LFN entries that can be skipped
        }
    }
    return iReturn;
}
#endif


// Load a sector if the present sector content is not alread valid
//
static int fnLoadSector(UTDISK *ptr_utDisk, unsigned long ulSector)
{
    if (ptr_utDisk->ulPresentSector != ulSector) {                       // if the requested sector is not the already loaded one
#if defined UTFAT_WRITE
        if (ptr_utDisk->usDiskFlags & WRITEBACK_BUFFER_FLAG) {           // if changes have been made to the sector since its read it must be written back
            while (_utCommitSectorData[ptr_utDisk->ucDriveNumber](ptr_utDisk, ptr_utDisk->ptrSectorData, ptr_utDisk->ulPresentSector) == CARD_BUSY_WAIT) {} // force writeback to finalise the operation
            ptr_utDisk->usDiskFlags &= ~WRITEBACK_BUFFER_FLAG;
        }
#endif
        ptr_utDisk->ulPresentSector = ulSector;
        return (_utReadDiskSector[ptr_utDisk->ucDriveNumber](ptr_utDisk, ulSector, ptr_utDisk->ptrSectorData)); // read the new sector
    }
    return UTFAT_SUCCESS;
}

// Read a part of a sector directly to a buffer (length is maximum 512 bytes)
//
static int fnLoadPartialData(UTDISK *ptr_utDisk, unsigned long ulSector, void *ptrBuf, unsigned short usOffset, unsigned short usLength)
{
    if ((usOffset + usLength) > 512) {
        usLength = (512 - usOffset);                                     // limit the length if the offset plus requested length would be beyond a sector
    }
    if (ptr_utDisk->ulPresentSector == ulSector) {                       // if the requested sector is already loaded (also if it has been modified)
        uMemcpy(ptrBuf, &ptr_utDisk->ptrSectorData[usOffset], usLength); // the data is already in the local sector copy so copy it directly to the caller's buffer
    }
    else {                                                               // the normal case is to load directly
        return (_utReadPartialDiskData[ptr_utDisk->ucDriveNumber](ptr_utDisk, ulSector, ptrBuf, usOffset, usLength)); // read data from disk without changing the present sector
    }
    return UTFAT_SUCCESS;
}

// Increment the sector within a cluster - this is a simple increment of ulSector (for FAT32) unless the end of a cluster is encountered, in which case it causes a move to the first sector in the next cluster
//
static int fnNextSector(UTDISK *ptr_utDisk, FILE_LOCATION *ptr_location)
{
    ptr_location->ulSector++;                                            // next sector
    if (((ptr_location->ulSector - ptr_utDisk->ulLogicalBaseAddress) % ptr_utDisk->utFAT.ucSectorsPerCluster) == 0) { // check whether the present cluster end has been reached
        unsigned long ulClusterSector;
        unsigned long ulCluster;
#if defined UTFAT16 || defined UTFAT12
        unsigned short usCluster;
        if (ptr_utDisk->usDiskFlags & (DISK_FORMAT_FAT12)) {
    #if defined UTFAT12
            if (ptr_location->ulSector == (ptr_utDisk->ulVirtualBaseAddress + 2)) { // root folder end reached
                return UTFAT_FAT12_ROOT_FOLDER_EXHAUSTED;                // FAT12 root folder exhausted
            }
            else if (ptr_location->ulSector < (ptr_utDisk->ulVirtualBaseAddress + 2)) { // in the FAT12 root folder
                return UTFAT_SUCCESS;                                    // this never grows so its FAT12 entry always remains the same
            }
            else {
                FAT12_FAT fat12_fat;
                unsigned long ulClusters[2];
                unsigned short usClusterEntry = (unsigned short)fnGetFat12_cluster_entry(ptr_location->ulCluster, &fat12_fat); // get information about where the FAT12 entry is located
                ulClusterSector = (ptr_utDisk->utFAT.ulFAT_start + fat12_fat.ulFat12SectorNumber); // section where the FAT12 responsible for this cluster resides
                if (fnLoadPartialData(ptr_utDisk, ulClusterSector, (unsigned char *)ulClusters, (usClusterEntry * sizeof(unsigned long)), sizeof(ulClusters)) != UTFAT_SUCCESS) { // read directly to cluster value
                    return UTFAT_DISK_READ_ERROR;
                }
                ulCluster = fnExtractFat12_cluster_value(&fat12_fat, ulClusters, (usClusterEntry < (512/(sizeof(unsigned long) - 1))));
            }
    #endif
        }
    #if defined UTFAT16
        else if (ptr_utDisk->usDiskFlags & (DISK_FORMAT_FAT16)) {
            if (ptr_location->ulSector == (ptr_utDisk->ulVirtualBaseAddress + 2)) { // root folder end reached
                return UTFAT_FAT16_ROOT_FOLDER_EXHAUSTED;                // FAT16 root folder exhausted
            }
            else if (ptr_location->ulSector < (ptr_utDisk->ulVirtualBaseAddress + 2)) { // in the FAT16 root folder
                return UTFAT_SUCCESS;                                    // this never grows so its FAT16 entry always remains the same
            }
            ulClusterSector = (ptr_utDisk->utFAT.ulFAT_start + (ptr_location->ulCluster >> 8)); // section where the FAT16 responsible for this cluster resides
            if (fnLoadPartialData(ptr_utDisk, ulClusterSector, (unsigned char *)&usCluster, (unsigned short)((ptr_location->ulCluster & 0xff) * sizeof(unsigned short)), sizeof(unsigned short)) != UTFAT_SUCCESS) { // read directly to cluster value
                return UTFAT_DISK_READ_ERROR;
            }
            ulCluster = LITTLE_SHORT_WORD(usCluster);                    // ensure endien is correct
        }
    #endif
        else {
            ulClusterSector = (ptr_utDisk->utFAT.ulFAT_start + (ptr_location->ulCluster >> 7)); // section where the FAT32 responsible for this cluster resides
            if (fnLoadPartialData(ptr_utDisk, ulClusterSector, (unsigned char *)&ulCluster, (unsigned short)((ptr_location->ulCluster & 0x7f) * sizeof(unsigned long)), sizeof(unsigned long)) != UTFAT_SUCCESS) { // read directly to cluster value
                return UTFAT_DISK_READ_ERROR;
            }
            ulCluster = LITTLE_LONG_WORD(ulCluster);                     // ensure endien is correct
        }
#else
        ulClusterSector = (ptr_utDisk->utFAT.ulFAT_start + (ptr_location->ulCluster >> 7)); // section where the FAT responsible for this cluster resides
        if (fnLoadPartialData(ptr_utDisk, ulClusterSector, (unsigned char *)&ulCluster, (unsigned short)((ptr_location->ulCluster & 0x7f) * sizeof(unsigned long)), sizeof(unsigned long)) != UTFAT_SUCCESS) { // read directly to cluster value
            return UTFAT_DISK_READ_ERROR;
        }
        ulCluster = LITTLE_LONG_WORD(ulCluster);                         // ensure endien is correct
#endif
        if ((ulCluster <= ptr_utDisk->ulDirectoryBase) || (ulCluster > ptr_utDisk->utFAT.ulClusterCount)) {
            return UTFAT_DIRECTORY_AREA_EXHAUSTED;
        }
        ptr_location->ulCluster = ulCluster;                             // set next cluster
        ptr_location->ulSector = ((ptr_location->ulCluster * ptr_utDisk->utFAT.ucSectorsPerCluster) + ptr_utDisk->ulVirtualBaseAddress);
    }
    return UTFAT_SUCCESS;
}

static int fnNextSectorCreate(UTDISK *ptr_utDisk, FILE_LOCATION *ptr_location, int iSeek) // added iSeek parameter
{
    ptr_location->ulSector++;                                            // next sector
    if (((ptr_location->ulSector - ptr_utDisk->ulLogicalBaseAddress) % ptr_utDisk->utFAT.ucSectorsPerCluster) == 0) { // check whether the present cluster end has been reached
        unsigned long ulClusterSector;
        unsigned long ulCluster;
        unsigned long ulClusterMask;
    #if defined UTFAT16 || defined UTFAT12
        if (ptr_utDisk->usDiskFlags & (DISK_FORMAT_FAT12)) {
        #if defined UTFAT12
            unsigned long ulClusters[2];
            FAT12_FAT fat12_fat;
            unsigned short usClusterEntry = (unsigned short)fnGetFat12_cluster_entry(ptr_location->ulCluster, &fat12_fat); // get information about where the FAT12 entry is located
            ulClusterSector = (ptr_utDisk->utFAT.ulFAT_start + fat12_fat.ulFat12SectorNumber); // section where the FAT12 responsible for this cluster resides
            if (iSeek != 0) {
                if (fnLoadSector(ptr_utDisk, ulClusterSector) != UTFAT_SUCCESS) { // when seeking we prefer to load a sector locally to avoid multiple reads when seeking through them
                    return UTFAT_DISK_READ_ERROR;
                }
            }
            if (fnLoadPartialData(ptr_utDisk, ulClusterSector, (unsigned char *)ulClusters, (usClusterEntry * sizeof(unsigned long)), sizeof(ulClusters)) != UTFAT_SUCCESS) { // read directly to cluster value
                return UTFAT_DISK_READ_ERROR;
            }
            ulCluster = fnExtractFat12_cluster_value(&fat12_fat, ulClusters, (usClusterEntry < (512/(sizeof(unsigned long) - 1))));
            ulClusterMask = FAT12_CLUSTER_MASK;
        #endif
        }
        #if defined UTFAT16
        else if (ptr_utDisk->usDiskFlags & (DISK_FORMAT_FAT16)) {
            unsigned short usCluster;
            ulClusterSector = (ptr_utDisk->utFAT.ulFAT_start + (ptr_location->ulCluster >> 8)); // section where the FAT16 responsible for this cluster resides
            if (iSeek != 0) {
                if (fnLoadSector(ptr_utDisk, ulClusterSector) != UTFAT_SUCCESS) { // when seeking we prefer to load a sector locally to avoid multiple reads when seeking through them
                    return UTFAT_DISK_READ_ERROR;
                }
            }
            if (fnLoadPartialData(ptr_utDisk, ulClusterSector, (unsigned char *)&usCluster, (unsigned short)((ptr_location->ulCluster & 0xff) * sizeof(unsigned short)), sizeof(unsigned short)) != UTFAT_SUCCESS) { // read directly to cluster value
                return UTFAT_DISK_READ_ERROR;
            }
            ulCluster = LITTLE_SHORT_WORD(usCluster);                    // ensure endien is correct
            ulClusterMask = FAT16_CLUSTER_MASK;
        }
        #endif
        else {
            ulClusterSector = (ptr_utDisk->utFAT.ulFAT_start + (ptr_location->ulCluster >> 7)); // section where the FAT32 responsible for this cluster resides
            if (iSeek != 0) {
                if (fnLoadSector(ptr_utDisk, ulClusterSector) != UTFAT_SUCCESS) { // when seeking we prefer to load a sector locally to avoid multiple reads when seeking through them
                    return UTFAT_DISK_READ_ERROR;
                }
            }
            if (fnLoadPartialData(ptr_utDisk, ulClusterSector, (unsigned char *)&ulCluster, (unsigned short)((ptr_location->ulCluster & 0x7f) * sizeof(unsigned long)), sizeof(unsigned long)) != UTFAT_SUCCESS) { // read directly to cluster value
                return UTFAT_DISK_READ_ERROR;
            }
            ulCluster = LITTLE_LONG_WORD(ulCluster);                     // ensure endien is correct
            ulClusterMask = CLUSTER_MASK;
        }
    #else
        ulClusterSector = (ptr_utDisk->utFAT.ulFAT_start + (ptr_location->ulCluster >> 7)); // section where the FAT32 responsible for this cluster resides
        if (iSeek != 0) {
            if (fnLoadSector(ptr_utDisk, ulClusterSector) != UTFAT_SUCCESS) { // when seeking we prefer to load a sector locally to avoid multiple reads when seeking through them
                return UTFAT_DISK_READ_ERROR;
            }
        }
        if (fnLoadPartialData(ptr_utDisk, ulClusterSector, (unsigned char *)&ulCluster, (unsigned short)((ptr_location->ulCluster & 0x7f) * sizeof(unsigned long)), sizeof(unsigned long)) != UTFAT_SUCCESS) { // read directly to cluster value
            return UTFAT_DISK_READ_ERROR;
        }
        ulCluster = LITTLE_LONG_WORD(ulCluster);                         // ensure endien is correct
        ulClusterMask = CLUSTER_MASK;
    #endif
        if ((ulCluster <= ptr_utDisk->ulDirectoryBase) || (ulCluster >= ulClusterMask)) { // invalid or end of current cluster - add a cluster so that the file can grow
    #if defined UTFAT_WRITE                                              // allow operation without write support
            ulCluster = fnAllocateCluster(ptr_utDisk, ptr_location->ulCluster, NEW_RELATIVE_CLUSTER); // create a new cluster so that it can be used as next cluster
    #endif
        }
        if (ptr_location->ulCluster > ptr_utDisk->utFAT.ulClusterCount) {
            return UTFAT_DIRECTORY_AREA_EXHAUSTED;
        }
        ptr_location->ulCluster = ulCluster;
        ptr_location->ulSector = ((ulCluster * ptr_utDisk->utFAT.ucSectorsPerCluster) + ptr_utDisk->ulVirtualBaseAddress);
    }
    return UTFAT_SUCCESS;
}

#if defined UTFAT_LFN_WRITE
static int fnDirectorySectorCreate(UTDISK *ptr_utDisk, FILE_LOCATION *ptr_location)
{
    unsigned long ulClusterSector;
    unsigned long ulCluster;
    unsigned long ulClusterMask;
    #if defined UTFAT16 || defined UTFAT12
    unsigned short usCluster;
    if (ptr_utDisk->usDiskFlags & (DISK_FORMAT_FAT12 | DISK_FORMAT_FAT16)) {
        ulClusterSector = (ptr_utDisk->utFAT.ulFAT_start + (ptr_location->ulCluster >> 8)); // section where the FAT16 responsible for this cluster resides
        if (fnLoadPartialData(ptr_utDisk, ulClusterSector, (unsigned char *)&usCluster, (unsigned short)((ptr_location->ulCluster & 0xff) * sizeof(unsigned short)), sizeof(unsigned short)) != UTFAT_SUCCESS) { // read directly to cluster value
            return UTFAT_DISK_READ_ERROR;
        }
        ulCluster = LITTLE_SHORT_WORD(usCluster);                        // ensure endien is correct
        ulClusterMask = FAT16_CLUSTER_MASK;
    }
    else {
        ulClusterSector = (ptr_utDisk->utFAT.ulFAT_start + (ptr_location->ulCluster >> 7)); // section where the FAT32 responsible for this cluster resides
        if (fnLoadPartialData(ptr_utDisk, ulClusterSector, (unsigned char *)&ulCluster, (unsigned short)((ptr_location->ulCluster & 0x7f) * sizeof(unsigned long)), sizeof(unsigned long)) != UTFAT_SUCCESS) { // read directly to cluster value
            return UTFAT_DISK_READ_ERROR;
        }
        ulCluster = LITTLE_LONG_WORD(ulCluster);                         // ensure endien is correct
        ulClusterMask = CLUSTER_MASK;
    }
    #else
    ulClusterSector = (ptr_utDisk->utFAT.ulFAT_start + (ptr_location->ulCluster >> 7)); // section where the FAT responsible for this cluster resides
    if (fnLoadPartialData(ptr_utDisk, ulClusterSector, (unsigned char *)&ulCluster, (unsigned short)((ptr_location->ulCluster & 0x7f) * sizeof(unsigned long)), sizeof(unsigned long)) != UTFAT_SUCCESS) { // read directly to cluster value
        return UTFAT_DISK_READ_ERROR;
    }
    ulCluster = LITTLE_LONG_WORD(ulCluster);                             // ensure endien is correct
    ulClusterMask = CLUSTER_MASK;
    #endif
    if ((ulCluster <= ptr_utDisk->ulDirectoryBase) || (ulCluster >= ulClusterMask)) { // invalid or end of current cluster - add a cluster so that the file can grow
        ulCluster = fnAllocateCluster(ptr_utDisk, ptr_location->ulCluster, (INITIALISE_DIR_EXTENSION | NEW_RELATIVE_CLUSTER)); // next cluster
    }
    if (ptr_location->ulCluster > ptr_utDisk->utFAT.ulClusterCount) {
        return UTFAT_DIRECTORY_AREA_EXHAUSTED;
    }
    ptr_location->ulCluster = ulCluster;
    ptr_location->ulSector = ((ulCluster * ptr_utDisk->utFAT.ucSectorsPerCluster) + ptr_utDisk->ulVirtualBaseAddress);
    return UTFAT_SUCCESS;
}
#endif

#if defined UTFAT_WRITE && (defined UTFAT_UNDELETE || defined UTFAT_EXPERT_FUNCTIONS)
// This function moves to a deleted short file name location and changes its name from undeleted to a fixed short file name which can then be renamed if needed
//
extern int utUndeleteFile(UTLISTDIRECTORY *ptrListDirectory)
{
    unsigned char ucDrive = ptrListDirectory->ptr_utDirObject->ucDrive;
    UTDISK *ptr_utDisk = &utDisks[ucDrive];
    DIR_ENTRY_STRUCTURE_FAT32 *ptrDirectoryEntry;

    if (fnLoadSector(ptr_utDisk, ptrListDirectory->undelete_disk_location.directory_location.ulSector) != UTFAT_SUCCESS) { // move to the disk sector containing the directory and read its content
        return UTFAT_DISK_READ_ERROR;
    }
    ptrDirectoryEntry = (DIR_ENTRY_STRUCTURE_FAT32 *)ptr_utDisk->ptrSectorData;
    ptrDirectoryEntry += ptrListDirectory->undelete_disk_location.ucDirectoryEntry; // move to the present entry
    if (ptrDirectoryEntry->DIR_Name[0] != DIR_NAME_FREE) {
        return UTFAT_FILE_NOT_WRITEABLE;                                 // if we have a reference to a non-deleted file we ignore the request
    }
    uMemcpy(ptrDirectoryEntry->DIR_Name, "UNDELETETXT", 11);             // give a valid short file name to the file so that it can be accessed again
    while (_utCommitSectorData[ucDrive](ptr_utDisk, ptr_utDisk->ptrSectorData, ptrListDirectory->undelete_disk_location.directory_location.ulSector) == CARD_BUSY_WAIT) {} // force writeback to finalise the operation
    ptr_utDisk->usDiskFlags &= ~WRITEBACK_BUFFER_FLAG;                   // the disk is up to date with the buffer
    return UTFAT_SUCCESS;
}
#endif


// Move to the next entry in a directory - this is a simple increment of ucDirectoryEntry as long as the end of the sector is not reached, in which case
// the next sector will need to be loaded 
//
static int fnNextDirectoryEntry(UTDISK *ptr_utDisk, DISK_LOCATION *ptr_location)
{
    if (++(ptr_location->ucDirectoryEntry) >= (ptr_utDisk->utFAT.usBytesPerSector/sizeof(DIR_ENTRY_STRUCTURE_FAT32))) { // end of present sector reached
        ptr_location->ucDirectoryEntry = 0;                              // start at first entry in next sector
        return (fnNextSector(ptr_utDisk, &ptr_location->directory_location)); // move to next sector associated with the directory
    }
    return UTFAT_SUCCESS;
}


static void fnLoadShortFileInfo(UTFILEINFO *ptr_utFileInfo, const DIR_ENTRY_STRUCTURE_FAT32 *ptrDirectoryEntry)
{
    int i;
    unsigned char c;
    CHAR *ptrShortName = ptr_utFileInfo->cFileName;
    unsigned char ucNT_info = ptrDirectoryEntry->DIR_NTRes;

    for (i = 0; i < 8; i++) {                                            // name
        c = (unsigned char)ptrDirectoryEntry->DIR_Name[i];
        if (c == ' ') {
            break;
        }
        if (c == 0x05) {
            c = DIR_NAME_FREE;
        }
        if ((ucNT_info & 0x08) && (c >= 'A') && (c <= 'Z')) {
            c += ('a' - 'A');                                            // convert to small letters
        }
        *ptrShortName++ = c;
    }
    if (ptrDirectoryEntry->DIR_Name[8] != ' ') {                         // extension
        *ptrShortName++ = '.';
        for (i = 8; i < 11; i++) {
            c = ptrDirectoryEntry->DIR_Name[i];
            if (c == ' ') {
                break;
            }
            if ((ucNT_info & 0x10) && (c >= 'A') && (c <= 'Z')) {
                c += ('a' - 'A');                                        // convert to small letters
            }
            *ptrShortName++ = c;
        }
    }
    *ptrShortName = 0;                                                   // terminate

    ptr_utFileInfo->ucFileAttributes = ptrDirectoryEntry->DIR_Attr;
    ptr_utFileInfo->ulFileSize = ((ptrDirectoryEntry->DIR_FileSize[3] << 24) + (ptrDirectoryEntry->DIR_FileSize[2] << 16) + (ptrDirectoryEntry->DIR_FileSize[1] << 8) + ptrDirectoryEntry->DIR_FileSize[0]);
    ptr_utFileInfo->usFileData = ((ptrDirectoryEntry->DIR_WrtDate[1] << 8) + ptrDirectoryEntry->DIR_WrtDate[0]);
    ptr_utFileInfo->usFileTime = ((ptrDirectoryEntry->DIR_WrtTime[1] << 8) + ptrDirectoryEntry->DIR_WrtTime[0]);
}

#if defined UTFAT_LFN_READ

static int fnExtractLongFileName(CHAR cLongFileName[MAX_UTFAT_FILE_NAME], LFN_ENTRY_STRUCTURE_FAT32 *ptrLFN_Entry, unsigned char *ucLongFileNameLength, unsigned char *ucLFN_checksum)
{
    int i = *ucLongFileNameLength;
    if ((ptrLFN_Entry->LFN_Attribute & DIR_ATTR_MASK) != (DIR_ATTR_LONG_NAME)) {
        return -1;                                                       // not long file name attribute so don't test further
    }
    if (i == 0) {
        return -1;                                                       // protect against invalid length
    }
    #if defined UTFAT_UNDELETE || defined UTFAT_EXPERT_FUNCTIONS
    if (((ptrLFN_Entry->LFN_EntryNumber & 0x40)) && ((ptrLFN_Entry->LFN_EntryNumber != DIR_NAME_FREE) || (i == MAX_UTFAT_FILE_NAME))) // {6}
    #else
    if (ptrLFN_Entry->LFN_EntryNumber & 0x40)                            // first LFN entry
    #endif
    {
        if ((ptrLFN_Entry->LFN_Name_12 != 0xff) && (ptrLFN_Entry->LFN_Name_12 != 0x00)) { // string ending without correct termination (padding or null terminator)
            cLongFileName[--i] = 0;                                      // force a null terminator
        }
        *ucLFN_checksum = ptrLFN_Entry->LFN_Checksum;                    // the checksum to be used to verify that the SFN alias is valid
    }
    else if (*ucLFN_checksum != ptrLFN_Entry->LFN_Checksum) {            // it is expected that the SFN alias in all individual LFN entries are the same
        return -1;
    }
    if (i < 14) {                                                        // protect against over-long names
        uMemset(cLongFileName, 0, i);                                    // pad with zeros
        i = 0;
    }
    else {
        cLongFileName[--i] = ptrLFN_Entry->LFN_Name_12;                  // enter the characters backwards in the LFN buffer
        cLongFileName[--i] = ptrLFN_Entry->LFN_Name_11;
        cLongFileName[--i] = ptrLFN_Entry->LFN_Name_10;
        cLongFileName[--i] = ptrLFN_Entry->LFN_Name_9;
        cLongFileName[--i] = ptrLFN_Entry->LFN_Name_8;
        cLongFileName[--i] = ptrLFN_Entry->LFN_Name_7;
        cLongFileName[--i] = ptrLFN_Entry->LFN_Name_6;
        cLongFileName[--i] = ptrLFN_Entry->LFN_Name_5;
        cLongFileName[--i] = ptrLFN_Entry->LFN_Name_4;
        cLongFileName[--i] = ptrLFN_Entry->LFN_Name_3;
        cLongFileName[--i] = ptrLFN_Entry->LFN_Name_2;
        cLongFileName[--i] = ptrLFN_Entry->LFN_Name_1;
        cLongFileName[--i] = ptrLFN_Entry->LFN_Name_0;
    }
    *ucLongFileNameLength = i;                                           // update the length position
    return UTFAT_SUCCESS;                                                // long file name has been extracted
}
#endif

#if defined UTFAT_UNDELETE || defined UTFAT_EXPERT_FUNCTIONS
    static int fnExtractFileDetails(UTDISK *ptr_utDisk, UTFILEINFO *ptr_ut_fileInfo, DISK_LOCATION *ptr_disk_location, DISK_LOCATION *ptr_undelete_disk_location, unsigned char ucListFlags)
#else
    static int fnExtractFileDetails(UTDISK *ptr_utDisk, UTFILEINFO *ptr_ut_fileInfo, DISK_LOCATION *ptr_disk_location)
#endif
{
#if defined UTFAT_LFN_READ
    CHAR cLongFileName[MAX_UTFAT_FILE_NAME];                             // temporary buffer to hold LFN
    unsigned char ucLongFileNameLength = MAX_UTFAT_FILE_NAME;            // maximum LFN length supported
    unsigned char ucLFN_checksum = 0;
#endif
    DIR_ENTRY_STRUCTURE_FAT32 *ptrDirectoryEntry;
    int iFound = 0;                                                      // entry not yet found

    while (iFound == 0) {                                                // repeat until the complete entry has been retrieved
        if (fnLoadSector(ptr_utDisk, ptr_disk_location->directory_location.ulSector) != UTFAT_SUCCESS) { // move to the disk sector containing the directory and read its content
            return UTFAT_DISK_READ_ERROR;
        }
        ptrDirectoryEntry = (DIR_ENTRY_STRUCTURE_FAT32 *)ptr_utDisk->ptrSectorData;
        ptrDirectoryEntry += ptr_disk_location->ucDirectoryEntry;        // move to the present entry
        switch (ptrDirectoryEntry->DIR_Name[0]) {
        case 0:                                                          // end of directory has been reached
            if (ptrDirectoryEntry->DIR_Attr & DIR_ATTR_VOLUME_ID) {      // skip volume ID
                break;
            }
            return UTFAT_END_OF_DIRECTORY;
        case DIR_NAME_FREE:                                              // 0xe5 means that a deleted entry has been found
#if defined UTFAT_LFN_READ
    #if defined UTFAT_UNDELETE || defined UTFAT_EXPERT_FUNCTIONS
            if ((ucListFlags & DELETED_TYPE_LISTING) == 0) {
                ucLongFileNameLength = MAX_UTFAT_FILE_NAME;              // reset the LFN index
            }
    #else
            ucLongFileNameLength = MAX_UTFAT_FILE_NAME;                  // reset the LFN index
    #endif
#endif
#if defined UTFAT_UNDELETE || defined UTFAT_EXPERT_FUNCTIONS
            if (ucListFlags & DELETED_TYPE_LISTING) {                    // don't skip non-deleted entries
                goto _entry_found;                                       // list a deleted entry
            }
#endif
            break;                                                       // skip the entry
        default:                                                         // non-deleted entry
#if defined UTFAT_UNDELETE || defined UTFAT_EXPERT_FUNCTIONS
            if ((ucListFlags & DELETED_TYPE_LISTING) != 0) {             // skip non-deleted entries if deleted items are to be listed
//              if (ucLongFileNameLength == MAX_UTFAT_FILE_NAME) {       // not in the process of collecting a possible long file name
                    break;
//              }
            }
#endif
#if defined UTFAT_UNDELETE || defined UTFAT_EXPERT_FUNCTIONS
_entry_found:                                                            // entry is to be listed
#endif
#if defined UTFAT_LFN_READ                                               // if long file name read support is enabled
            if (fnExtractLongFileName(cLongFileName, (LFN_ENTRY_STRUCTURE_FAT32 *)ptrDirectoryEntry, &ucLongFileNameLength, &ucLFN_checksum) != UTFAT_SUCCESS) {
                if ((ptrDirectoryEntry->DIR_Attr & DIR_ATTR_HIDDEN) != 0) { // {7} if hidden we don't display it
    #if defined UTFAT_EXPERT_FUNCTIONS
                    if ((ucListFlags & HIDDEN_TYPE_LISTING) == 0) {      // allow listing hidden files
                        ucLongFileNameLength = MAX_UTFAT_FILE_NAME;      // {15} reset long file name length counter
                        break;
                    }
    #else
                    ucLongFileNameLength = MAX_UTFAT_FILE_NAME;          // {15} reset long file name length counter
                    break;
    #endif
                }
                if ((ptrDirectoryEntry->DIR_Attr & DIR_ATTR_VOLUME_ID) == 0) { // if not a volume entry it is a short file name or short file name alias
                    fnLoadShortFileInfo(ptr_ut_fileInfo, ptrDirectoryEntry); // extract the short file information
                    if (ucLongFileNameLength != MAX_UTFAT_FILE_NAME) {   // long file name has been collected
    #if defined UTFAT_UNDELETE || defined UTFAT_EXPERT_FUNCTIONS
                        if (((ucListFlags & DELETED_TYPE_LISTING) != 0) || (fnLFN_checksum((CHAR *)(ptrDirectoryEntry->DIR_Name)) == ucLFN_checksum)) 
    #else
                        if (fnLFN_checksum((CHAR *)(ptrDirectoryEntry->DIR_Name)) == ucLFN_checksum) 
    #endif
                        {                                                // as long as the short file name alias checksum matches
                            uStrcpy(ptr_ut_fileInfo->cFileName, &cLongFileName[ucLongFileNameLength]); // copy the collected long file name to the file name string
                            iFound = 1;
                        }
                        else {
                            // If the checksum of the SFN alias doesn't match with the LFN entry it may mean that the SFN location has been deleted
                            // and reused by a system that doesn't understand LFN - it is therefore ignored
                            //
                            _EXCEPTION("Debug if interested");
                        }
                    }
                    else {
                        iFound = 1;                                      // mark that a file has been found and we can terminate after incrementing the directory entry
    #if defined UTFAT_UNDELETE || defined UTFAT_EXPERT_FUNCTIONS
                        if (ucListFlags & DELETED_TYPE_LISTING) {        // if listing deleted entries
                            int i;
                            ptr_ut_fileInfo->cFileName[0] = '~';         // deleted files/directories start with ~
                            for (i = 1; i < 10; i++) {                   // display short file name
                                if (ptr_ut_fileInfo->cFileName[i] < 0) {
                                    ptr_ut_fileInfo->cFileName[i] = (ptr_ut_fileInfo->cFileName[i] - 0x80);
                                }
                                if (ptr_ut_fileInfo->cFileName[i] == 0) {
                                    break;
                                }
                                if (ptr_ut_fileInfo->cFileName[i] <= ' ') {
                                    ptr_ut_fileInfo->cFileName[i] += 0x21; // replace deleted character for display purposes
                                }
                            }
        #if defined UTFAT_UNDELETE
                          uMemcpy(ptr_undelete_disk_location, ptr_disk_location, sizeof(DISK_LOCATION)); // copy the present location in case it is to be used to undelete this entry
        #endif
                        }
    #endif
                    }
                    ucLongFileNameLength = MAX_UTFAT_FILE_NAME;          // reset long file name counter
                }
            }
#else                                                                    // no LFN read supported
            if ((ptrDirectoryEntry->DIR_Attr & DIR_ATTR_VOLUME_ID) == 0) {  // if not a volume entry
                if (ptrDirectoryEntry->DIR_Attr & DIR_ATTR_HIDDEN) {     // {7} if hidden we don't display it
    #if defined UTFAT_EXPERT_FUNCTIONS
                    if ((ucListFlags & HIDDEN_TYPE_LISTING) == 0) {      // allow listing hidden files
                        break;
                    }
    #else
                    break;
    #endif
                }
                fnLoadShortFileInfo(ptr_ut_fileInfo, ptrDirectoryEntry); // extract the file information
                iFound = 1;                                              // mark that a file has been found and we can terminate after incrementing the directory entry
            }
#endif
            break;
        }
        if (fnNextDirectoryEntry(ptr_utDisk, ptr_disk_location) == UTFAT_DIRECTORY_AREA_EXHAUSTED) { // move to the next entry
            if (iFound == 1) {
                return UTFAT_FINAL_LISTING_ITEM_FOUND;
            }
            return UTFAT_DIRECTORY_AREA_EXHAUSTED;                       // quit since the complete directory space has been exhausted
        }
    }
    return UTFAT_SUCCESS;
}

// Read directory or file entry and fill out details for display purposes
//
extern int utReadDirectory(UTLISTDIRECTORY *ptr_utListDirectory, UTFILEINFO *ptr_ut_fileInfo)
{
    UTDISK *ptr_utDisk = &utDisks[ptr_utListDirectory->ptr_utDirObject->ucDrive]; // the disk that the directory is associated with
#if defined UTFAT_UNDELETE || defined UTFAT_EXPERT_FUNCTIONS
    return (fnExtractFileDetails(ptr_utDisk, ptr_ut_fileInfo, &ptr_utListDirectory->private_disk_location, &ptr_utListDirectory->undelete_disk_location, ptr_utListDirectory->ucListFlags));
#else
    return (fnExtractFileDetails(ptr_utDisk, ptr_ut_fileInfo, &ptr_utListDirectory->private_disk_location));
#endif
}


#if !defined _REMOVE_FORMATTED_OUTPUT                                    // no directory listing possible without formatted output
// Perform a directory listing to a buffer (with options for DOS and FTP styles)
//
extern int utListDir(UTLISTDIRECTORY *ptr_utListDirectory, FILE_LISTING *ptrFileListingInfo)
{
    unsigned char ucLineLength;
    int iResult;
    CHAR *ptrBuf = ptrFileListingInfo->ptrBuffer;
    CHAR *ptrName;
    UTFILEINFO utFileInfo;                                               // temporary file info object
    DISK_LOCATION previous_disk_loc;
    unsigned short usYear;
    unsigned char  ucDay, ucMonth, ucHour, ucMinutes;
    ptrFileListingInfo->usStringLength = 0;
    if ((ptrFileListingInfo->ucStyle & FTP_TYPE_LISTING) != 0) {
        ucLineLength = FTP_STYLE_LIST_ENTRY_LENGTH;                      // the maximum length of a single entry line - the user's buffer must be adequate to accept this
    }
    else {
        ucLineLength = DOS_STYLE_LIST_ENTRY_LENGTH;                      // the maximum length of a single entry line - the user's buffer must be adequate to accept this
    }
    #if defined UTFAT_UNDELETE || defined UTFAT_EXPERT_FUNCTIONS
    ptr_utListDirectory->ucListFlags = ptrFileListingInfo->ucStyle;
    #endif
    ptrFileListingInfo->usItemsReturned = 0;
    while (ptrFileListingInfo->usItemsReturned < ptrFileListingInfo->usMaxItems) { // for maximum amount of items
        uMemcpy(&previous_disk_loc, &ptr_utListDirectory->private_disk_location, sizeof (ptr_utListDirectory->private_disk_location)); // backup in case we need to restore due to lack of buffer space
        iResult = utReadDirectory(ptr_utListDirectory, &utFileInfo);
        if ((iResult != UTFAT_SUCCESS) && (iResult != UTFAT_FINAL_LISTING_ITEM_FOUND)) { // read next directory or file in the present directory
            return UTFAT_NO_MORE_LISTING_ITEMS_FOUND;                    // no more items found
        }
        if ((ptrFileListingInfo->usStringLength + ucLineLength + uStrlen(utFileInfo.cFileName)) > ptrFileListingInfo->usBufferLength) {
            uMemcpy(&ptr_utListDirectory->private_disk_location, &previous_disk_loc, sizeof (ptr_utListDirectory->private_disk_location)); // restore location
            return UTFAT_NO_MORE_LISING_SPACE;                           // no more file listings fit in the present buffer
        }
        ucDay = (utFileInfo.usFileData & 0x1f);
        ucMonth = ((utFileInfo.usFileData >> 5) & 0xf);
        usYear = ((utFileInfo.usFileData >> 9) + 1980);
        ucMinutes = ((utFileInfo.usFileTime >> 5) & 0x3f);
        ucHour = (utFileInfo.usFileTime >> 11);
        ptrFileListingInfo->ucFileAttributes = utFileInfo.ucFileAttributes;
        if (ptrFileListingInfo->ucStyle & FTP_TYPE_LISTING) {
            int i = 10;
            unsigned short usRights;
            CHAR cAccess = 'd';
            if (utFileInfo.ucFileAttributes & DIR_ATTR_DIRECTORY) {
                usRights = 0x3ed;                                        // directory
            }
            else {                                                       // file
                if (utFileInfo.ucFileAttributes & DIR_ATTR_READ_ONLY) {
                    usRights = 0x124;
                }
                else {
                    usRights = 0x1a4;
                }
            }
            while (i--) {
                if (usRights & 0x200) {
                    *ptrBuf++ = cAccess;                                 // rights
                }
                else {
                    *ptrBuf++ = '-';                                     // no rights
                }
                switch (cAccess) {                                       // set next flag
                case 'r':
                    cAccess = 'w';
                    break;
                case 'w':
                    cAccess = 'x';
                    break;
                default:
                    cAccess = 'r';
                    break;
                }
                usRights <<= 1;
            }
            ptrBuf = uStrcpy(ptrBuf, " 1 502 502 ");
            if (utFileInfo.ucFileAttributes & DIR_ATTR_DIRECTORY) {      // when listing a directory display its minimum cluster size rather than a size of 0
                UTFAT *ptrFAT = &utDisks[ptr_utListDirectory->ptr_utDirObject->ucDrive].utFAT; // the FAT that the directory is associated with
                ptrBuf = fnBufferDec((ptrFAT->ucSectorsPerCluster * ptrFAT->usBytesPerSector), 0, ptrBuf); // file size
            }
            else {
                ptrBuf = fnBufferDec(utFileInfo.ulFileSize, 0, ptrBuf); // file size
            }
            *ptrBuf++ = ' ';
            switch (ucMonth) {
            default:
                ptrBuf = uStrcpy(ptrBuf, "Jan");
                break;
            case 2:
                ptrBuf = uStrcpy(ptrBuf, "Feb");
                break;
            case 3:
                ptrBuf = uStrcpy(ptrBuf, "Mar");
                break;
            case 4:
                ptrBuf = uStrcpy(ptrBuf, "Apr");
                break;
            case 5:
                ptrBuf = uStrcpy(ptrBuf, "May");
                break;
            case 6:
                ptrBuf = uStrcpy(ptrBuf, "Jun");
                break;
            case 7:
                ptrBuf = uStrcpy(ptrBuf, "Jul");
                break;
            case 8:
                ptrBuf = uStrcpy(ptrBuf, "Aug");
                break;
            case 9:
                ptrBuf = uStrcpy(ptrBuf, "Sep");
                break;
            case 10:
                ptrBuf = uStrcpy(ptrBuf, "Oct");
                break;
            case 11:
                ptrBuf = uStrcpy(ptrBuf, "Nov");
                break;
            case 12:
                ptrBuf = uStrcpy(ptrBuf, "Dec");
                break;
            }
            ptrBuf = fnBufferDec(ucDay, (LEADING_ZERO | WITH_SPACE), ptrBuf); // day of month
            ptrBuf = fnBufferDec(usYear, WITH_SPACE, ptrBuf);
        }
        else {                                                           // DOS style listing
            if (utFileInfo.ucFileAttributes & DIR_ATTR_READ_ONLY) {
                *ptrBuf++ = 'R';
            }
            else {
                *ptrBuf++ = '-';
            }
            if (utFileInfo.ucFileAttributes & DIR_ATTR_HIDDEN) {
                *ptrBuf++ = 'H';
            }
            else {
                *ptrBuf++ = '-';
            }
            if (utFileInfo.ucFileAttributes & DIR_ATTR_SYSTEM) {
                *ptrBuf++ = 'S';
            }
            else {
                *ptrBuf++ = '-';
            }
            if (utFileInfo.ucFileAttributes & DIR_ATTR_ARCHIVE) {
                *ptrBuf++ = 'A';
            }
            else {
                *ptrBuf++ = '-';
            }
            ptrBuf = fnBufferDec(ucDay, (LEADING_ZERO | WITH_SPACE), ptrBuf);
            *ptrBuf++ = '.';
            ptrBuf = fnBufferDec(ucMonth, (LEADING_ZERO), ptrBuf);
            *ptrBuf++ = '.';
            ptrBuf = fnBufferDec(usYear, 0, ptrBuf);
            *ptrBuf++ = ' ';
            ptrBuf = fnBufferDec(ucHour, (LEADING_ZERO | WITH_SPACE), ptrBuf);
            *ptrBuf++ = ':';
            ptrBuf = fnBufferDec(ucMinutes, (LEADING_ZERO), ptrBuf);
            if (utFileInfo.ucFileAttributes & DIR_ATTR_DIRECTORY) {
                if ((ptrFileListingInfo->ucStyle & FTP_TYPE_LISTING) == DOS_TYPE_LISTING) {
                    ptrBuf = uStrcpy(ptrBuf, " <DIR>           ");
                }
                ptrFileListingInfo->usDirectoryCount++;                  // count the directories in this listing
            }
            else {
                int iLen;
                CHAR cLenBuf[18];                                        // maximum decimal file length plus null terminator plus " <DIR> "fill
                CHAR *ptrLen = fnBufferDec(utFileInfo.ulFileSize, WITH_SPACE, cLenBuf);
                iLen = (17 - (ptrLen - cLenBuf));
                while (iLen--) {
                    *ptrBuf++ = ' ';
                }
                ptrBuf = uStrcpy(ptrBuf, cLenBuf);
                ptrFileListingInfo->usFileCount++;                       // count the files in this listing
                ptrFileListingInfo->ulFileSizes += utFileInfo.ulFileSize;// sum of the total file sizes in this listing
            }
        }
        *ptrBuf++ = ' ';
        ptrName = ptrBuf;                                                // pointer to start of the file name in the output string
        ptrBuf = uStrcpy(ptrBuf, utFileInfo.cFileName);                  // add the file name
        ptrFileListingInfo->ucNameLength = (unsigned char)(ptrBuf - ptrName); // length of file name for easy recognition in the output string
        if ((ptrFileListingInfo->ucStyle & NO_CR_LF_LISTING) == 0) {     // don't add CR and LF if not desired
            *ptrBuf++ = '\r';
            *ptrBuf++ = '\n';
        }
        ptrFileListingInfo->usStringLength = (ptrBuf - ptrFileListingInfo->ptrBuffer); // the complete string length in output buffer
        ptrFileListingInfo->usItemsReturned++;                           // additional item counted
        if (ptrFileListingInfo->ucStyle & NO_CR_LF_LISTING) {
            *ptrBuf = 0;
        }
        if (iResult == UTFAT_FINAL_LISTING_ITEM_FOUND) {
            return UTFAT_NO_MORE_LISTING_ITEMS_FOUND;                    // the content is the final content
        }
    }
    return UTFAT_SUCCESS;
}
#endif


static UTDIRECTORY utDirectoryObjects[UT_DIRECTORIES_AVAILABLE] = {{0}};

extern UTDIRECTORY *utAllocateDirectory(unsigned char ucDisk, unsigned short usPathLength)
{
    int i = 0;
    while (i < UT_DIRECTORIES_AVAILABLE) {
        if (utDirectoryObjects[i].usDirectoryFlags == 0) {               // directory not allocated
            utDirectoryObjects[i].usDirectoryFlags = UTDIR_ALLOCATED;
            utDirectoryObjects[i].ucDrive = ucDisk;
            if (usPathLength != 0) {
                utDirectoryObjects[i].ptrDirectoryPath = SDCARD_MALLOC((MAX_MALLOC)(usPathLength + 1)); // reserve space for holding the directory path string (plus space for null terminator)
            }
            else {
                utDirectoryObjects[i].ptrDirectoryPath = 0;
            }
            utDirectoryObjects[i].usDirectoryPathLength = usPathLength;  // enter the maximum length that can be stored
            return &utDirectoryObjects[i];
        }
        i++;
    }
    return 0;                                                            // no directory object available for allocation
}

static void fnResetDirectories(unsigned char ucDisk)
{
    int i = 0;
    while (i < UT_DIRECTORIES_AVAILABLE) {
        if ((utDirectoryObjects[i].usDirectoryFlags & UTDIR_ALLOCATED) && (utDirectoryObjects[i].ucDrive == ucDisk)) { // directory allocated to this drive
            CHAR *ptrPath = utDirectoryObjects[i].ptrDirectoryPath;
            uMemset(&utDirectoryObjects[i], 0, sizeof(utDirectoryObjects[i]));
            utDirectoryObjects[i].usDirectoryFlags = UTDIR_ALLOCATED;
            utDirectoryObjects[i].ucDrive = ucDisk;
            if (ptrPath != 0) {
                utDirectoryObjects[i].ptrDirectoryPath = ptrPath;
#if defined DISK_C
                uStrcpy(ptrPath, "C:\\");                                // set root path
#elif defined DISK_D
                uStrcpy(ptrPath, "D:\\");                                // set root path
#elif defined DISK_E
                uStrcpy(ptrPath, "E:\\");                                // set root path
#endif
#if DISK_COUNT > 1
                *ptrPath += ucDisk;                                      // set the drive name
#endif
                utDirectoryObjects[i].usRelativePathLocation = 3;        // the relative path is equal to the root path   
            }
        }
        i++;
    }
}

extern UTDIRECTORY *utFreeDirectory(UTDIRECTORY *ptrDirectory)
{
#if defined _WINDOWS
    if (ptrDirectory->ptrDirectoryPath != 0) {                           // if memory was allocated for use by a path string warn that it is not freed
        _EXCEPTION("Path string not freed!!!");
    }
#endif
    uMemset(ptrDirectory, 0, sizeof(UTDIRECTORY));                       // release the directory object so that it could be reused
    return 0;
}

static int _utOpenDirectory(OPEN_FILE_BLOCK *ptrOpenBlock, UTDIRECTORY *ptrDirObject, unsigned long ulAccessMode/*iDeletedEntry*/) // {3} pass full access mode
{
#if defined UTFAT_EXPERT_FUNCTIONS || defined UTFAT_LFN_READ
    int iLFN_status = 0;
#endif
#if defined UTFAT_LFN_READ
    int iSkip = 0;
    int iMatchStatus;
#else
    DISK_LOCATION DeletedFile;                                           // {11} disk location of first found deleted object (for possible reuse)
#endif
    int iResult;
    int iDeletedEntry = ((ulAccessMode & UTFAT_OPEN_DELETED) != 0);
    DISK_LOCATION *ptrDiskLocation = ptrOpenBlock->ptrDiskLocation;      // the start of the present directory
    const CHAR *ptrLocalDirPath = ptrOpenBlock->ptrLocalDirPath;
    UTDISK *ptr_utDisk = &utDisks[ptrDirObject->ucDrive];
    DIR_ENTRY_STRUCTURE_FAT32 *ptrFoundEntry = 0;
    int iMatchedParagraph;
    DISK_LOCATION DirStart;                                              // backup of original disk location

    uMemcpy(&DirStart, ptrDiskLocation, sizeof(DirStart));               // backup the original starting directory location in case we need to return it

#if defined UTFAT_LFN_READ
    if ((*ptrLocalDirPath == BACK_SLASH) || (*ptrLocalDirPath == FORWARD_SLASH)) {
        ptrLocalDirPath++;                                               // ignore leading slashes
    }
#else
    DeletedFile.directory_location.ulSector = 0;                         // {11} initially invalid
#endif

    while (1) {                                                          // search through the directory to find the file/directory entry
#if defined UTFAT_LFN_READ
        ptrOpenBlock->ptrFileNameStart = ptrLocalDirPath;                // set a pointer to the name being searched for
#endif
        if ((ptrOpenBlock->iQualifiedPathType = fnCreateNameParagraph(&ptrLocalDirPath, ptrOpenBlock->cShortFileName)) < 0) { // fill the file name with a single paragraph from the file path
            return UTFAT_PATH_NOT_FOUND;                                 // error in the requested path
        }
#if defined UTFAT_LFN_READ
        ptrOpenBlock->ptrFileNameEnd = ptrLocalDirPath;                  // set to last character of name to match
        ptrOpenBlock->ptrFileNameMatch = (CHAR *)ptrOpenBlock->ptrFileNameEnd;
        if ((*ptrLocalDirPath == BACK_SLASH) || (*ptrLocalDirPath == FORWARD_SLASH)) {
            ptrLocalDirPath++;                                           // ignore leading slashes
        }
#endif
        iMatchedParagraph = 0;
        do {                                                             // handle entry, which could consist of multiple objects if LFN
            if (fnLoadSector(ptr_utDisk, ptrDiskLocation->directory_location.ulSector) != UTFAT_SUCCESS) { // load new sector (this will only read from physical disk when the sector is not already in local cache)
                return UTFAT_DISK_READ_ERROR;
            }
            ptrFoundEntry = (DIR_ENTRY_STRUCTURE_FAT32 *)ptr_utDisk->ptrSectorData; // the directory entry in the sector buffer
            ptrFoundEntry += ptrDiskLocation->ucDirectoryEntry;          // move to the present entry
#if defined UTFAT_LFN_READ                                               // when LFN support is enabled
            if (iLFN_status == 0) {                                      // if a potential long file name match has been found
                if (iSkip == 0) {                                        // if not skipping non-matching LFN entries
                    uMemcpy(&ptrOpenBlock->present_location, ptrDiskLocation, sizeof(ptrOpenBlock->present_location)); // note the present location being checked
                    iMatchStatus = fnMatchLongName(ptrOpenBlock, (LFN_ENTRY_STRUCTURE_FAT32 *)ptrFoundEntry, iDeletedEntry, &iSkip); // try to match the file/directory with LFN entry
    #if defined UTFAT_LFN_WRITE
                    if (ENTRY_DELETED == iMatchStatus) {                 // if a deleted entry is found
                        if (ptrOpenBlock->ucDeleteCount == 0) {          // first deleted entry in possible contiguous deleted entries
                            uMemcpy(&ptrOpenBlock->DeleteLocationRef, ptrDiskLocation, sizeof(ptrOpenBlock->DeleteLocationRef)); // mark the location of first deleted entry
                        }
                        if (ptrOpenBlock->ucDeleteCount < DELETED_ENTRY_COUNT) { // limit the deleted entry count to the entries available
                            ptrOpenBlock->ucDeleteCount++;               // count contiguous deleted entries
                        }
                    }
                    else {                                               // not a deleted entry
                        if (ptrOpenBlock->ucDeleteCount != 0) {          // end of deleted space
                                                                         // convert ptrOpenBlock->ucDeleteCount to an index (index 0 is a single-hole location, 1 a double-hold-location, up to 20 for a 21-hole-location)
                            uMemcpy(&ptrOpenBlock->DeleteLocation[ptrOpenBlock->ucDeleteCount - 1], &ptrOpenBlock->DeleteLocationRef, sizeof(ptrOpenBlock->DeleteLocationRef)); // save details about the location of this hole type
                            ptrOpenBlock->DeleteLocationRef.directory_location.ulSector = 0; // reset for next use
                            ptrOpenBlock->ucDeleteCount = 0;             // reset for next use
                        }
                    }
    #endif
                }
                else {
                    iMatchStatus = ENTRY_DELETED;                        // ignore as if deleted
                    if (--iSkip == 0) {                                  // SFN alias being skipped
                        if (ptrOpenBlock->ulSFN_found < SFN_ENTRY_CACHE_SIZE) { // as long as the cache has not been filled
                            uMemcpy(ptrOpenBlock->cSFN_entry[ptrOpenBlock->ulSFN_found++], &ptrFoundEntry->DIR_Name, 11); // copy SFN alias to cache and increment cach entry count
                        }
                    }
                }
            }
            else {                                                       // match has been found in LFN part of name
                if (fnLFN_checksum((CHAR *)ptrFoundEntry->DIR_Name) == ptrOpenBlock->ucSFN_alias_checksum) { // check the checksum of the short file name alias associated with the matched long file name [note that compilers may suggest that lfn_match_block.ucSFN_alias_checksum could be used uninitialised but this is not the case because we only arrive here after it has been set on previous loop]
                    ptrOpenBlock->iQualifiedPathType = FULLY_QUALIFIED_SHORT_NAME;
                }
                else {
                    iLFN_status = 0;                                     // consider the match as failed
                }
                iMatchStatus = MATCH_NOT_LFN;
            }
            switch (iMatchStatus) {
            case MATCH_SUCCESSFUL:                                       // complete match has been found in the long file name part
                iLFN_status = 1;                                         // for an unconditional match on next directory entry
                break;
            case MATCH_CONTINUE:                                         // match is still correct but not completed
                break;
            case MATCH_FALSE:                                            // LFN entry found but not matching
              //iSkip = (ptrFoundEntry->DIR_Name[0] & 0x3f);             // the entries to skip (LFN parts plus one SFN alias)
                break;
            case END_DIRECTORY_ENTRIES:                                  // end if end found
                if (*ptrLocalDirPath == 0) {
                    return UTFAT_FILE_NOT_FOUND;                         // the directory path was OK but the file was not found
                }
                return UTFAT_PATH_NOT_FOUND;                             // the directory path was not OK
    #if defined UTFAT_EXPERT_FUNCTIONS
            case DELETED_LFN_MATCH_SUCCESSFUL:
                ptrOpenBlock->iQualifiedPathType = FULLY_QUALIFIED_LONG_NAME_SFNM;
                iLFN_status = 1;
                // Fall through intentionally
                //
    #endif
            case MATCH_NOT_LFN:
                if ((FULLY_QUALIFIED_SHORT_NAME == ptrOpenBlock->iQualifiedPathType) || (FULLY_QUALIFIED_LONG_NAME_SFNM == ptrOpenBlock->iQualifiedPathType)) { // possibly short file name
#endif
                    switch (ptrFoundEntry->DIR_Name[0]) {                // {11} use when not in LFN mode
                    case 0:                                              // end of directory reached
                        if (ROOT_DIRECTORY_SETTING & ptrOpenBlock->iRootDirectory) { // if a virtual root path is being set it has failed so reset
                            ptrDirObject->usDirectoryFlags &= (UTDIR_ALLOCATED);
                            ptrDirObject->usRelativePathLocation = 0;
                            if (ptrDirObject->ptrDirectoryPath != 0) {
                                *ptrDirObject->ptrDirectoryPath = 0;
                            }
                        }
                        if (ptrOpenBlock->iQualifiedPathType != 0) {
                            if (ptrOpenBlock->usDirFlags & UTDIR_SET_START) {
#if !defined UTFAT_LFN_READ                                              // {11}
                                if (ulAccessMode & UTFAT_CREATE) {       // if a new file or directory is to be created
                                    if (DeletedFile.directory_location.ulSector != 0) { // if  delete entry was found in the present directory
                                        uMemcpy(ptrDiskLocation, &DeletedFile, sizeof(DeletedFile)); // return deleted entry if found so that it can be reused
                                    }
                                    return UTFAT_FILE_NOT_FOUND;         // searched file/directory not found (don't return the original location but instead the deleted or present empty one)
                                }
#endif
                                uMemcpy(ptrDiskLocation, &DirStart, sizeof(DirStart)); // set the location to the start of the lowest directory
                            }
                            return UTFAT_FILE_NOT_FOUND;                 // searched file/directory not found
                        }
                        else {
                            return UTFAT_PATH_NOT_FOUND;                 // searched path failed
                        }

                    case DIR_NAME_FREE:                                  // deleted entry (free)
#if defined UTFAT_EXPERT_FUNCTIONS
    #if defined UTFAT_LFN_READ
                        if ((iDeletedEntry != 0) && (iMatchStatus == MATCH_NOT_LFN)) {
                            iLFN_status = 1;                             // force match
                            // Fall through intentionally
                            //
                        }
                        else if (iMatchStatus != DELETED_LFN_MATCH_SUCCESSFUL) {
                            break;
                        }
    #else                                                                // {11}
                        if (iDeletedEntry != 0) {
                            iLFN_status = 1;                             // force match
                            // Fall through intentionally
                            //
                        }
                        else {
                            if (DeletedFile.directory_location.ulSector == 0) { // if no deleted entry already recorded
                                uMemcpy(&DeletedFile, ptrDiskLocation, sizeof(DeletedFile)); // save the deleted location in this directory
                            }
                            break;                                       // continue by jumping the deleted entry
                        }
    #endif
#else
    #if !defined UTFAT_LFN_READ
                        if (DeletedFile.directory_location.ulSector == 0) { // {11} if no deleted entry already recorded
                            uMemcpy(&DeletedFile, ptrDiskLocation, sizeof(DeletedFile)); // save the deleted file object location in the present directory
                        }
    #endif
                        break;                                       // continue by jumping the deleted entry
#endif
                    default:
                        if (
#if defined UTFAT_LFN_READ || defined UTFAT_EXPERT_FUNCTIONS
                            (iLFN_status != 0) ||                        // this short file name entry unconditionally belongs to a previous long file name block
#endif
                            (((ptrFoundEntry->DIR_Attr & DIR_ATTR_VOLUME_ID) == 0) && (uMemcmp(ptrFoundEntry->DIR_Name, ptrOpenBlock->cShortFileName, sizeof(ptrFoundEntry->DIR_Name)) == 0))) { // if entry has been found
                            ptrOpenBlock->ulCluster = ((ptrFoundEntry->DIR_FstClusHI[1] << 24) + (ptrFoundEntry->DIR_FstClusHI[0] << 16) + (ptrFoundEntry->DIR_FstClusLO[1] << 8) + ptrFoundEntry->DIR_FstClusLO[0]);
                            ptrDirObject->ptrEntryStructure = ptrFoundEntry; // set pointer to the entry so that the caller can extract additional information if required
                            if ((ptrFoundEntry->DIR_Attr & DIR_ATTR_DIRECTORY) == 0) {
                                ptrDirObject->public_file_location.ulCluster = ptrOpenBlock->ulCluster; // file's start cluster
                                ptrDirObject->public_file_location.ulSector = ((ptrOpenBlock->ulCluster * ptr_utDisk->utFAT.ucSectorsPerCluster) + ptr_utDisk->ulVirtualBaseAddress);// section referenced to logical base address
                                return UTFAT_PATH_IS_FILE;               // not a directory so can not be traced further
                            }
                            if ((ptrOpenBlock->iQualifiedPathType != 0) && (ptrOpenBlock->usDirFlags & UTDIR_DIR_AS_FILE)) { // if a directory is to be treated as a file, its location is preserved rather than moving to its content
                                return UTFAT_SUCCESS;                    // file matched
                            }
                            ptrDiskLocation->directory_location.ulCluster = ptrOpenBlock->ulCluster; // move to the new directory
                            ptrDiskLocation->directory_location.ulSector = ((ptrOpenBlock->ulCluster * ptr_utDisk->utFAT.ucSectorsPerCluster) + ptr_utDisk->ulVirtualBaseAddress);// section referenced to logical base address
                            ptrDiskLocation->ucDirectoryEntry = 2;       // skip "." and ".." entries
                            if (*ptrLocalDirPath == 0) {                 // check whether the end of the path has been reached
                                if (ROOT_DIRECTORY_RELOCATE == ptrOpenBlock->iRootDirectory) {
                                    return UTFAT_PATH_IS_ROOT_REF;       // directory path successfully found but relative to root
                                }
                                if (ROOT_DIRECTORY_SETTING & ptrOpenBlock->iRootDirectory) { // successfully found (virtual) root set set the location
                                    uMemcpy(&ptrDirObject->root_disk_location, &ptrDirObject->private_disk_location, sizeof(ptrDirObject->root_disk_location)); // this is the first open, which is setting the virtual root as seen by the user
                                    uMemcpy(&ptrDirObject->public_disk_location, &ptrDirObject->private_disk_location, sizeof(ptrDirObject->public_disk_location));
                                }
                                return UTFAT_SUCCESS;                    // file matched
                            }
                            uMemcpy(&DirStart, ptrDiskLocation, sizeof(DirStart)); // set the directory location to the start of this directory
                            ptrOpenBlock->ptrLocalDirPath = ptrLocalDirPath;
#if defined UTFAT_LFN_READ
                            if (iLFN_status != 0) {                      // if forced match due to LFN
                                ptrOpenBlock->iQualifiedPathType = FULLY_QUALIFIED_LONG_NAME; // correct qualifier
                                iLFN_status = 0;                         // reset the flag
                            }
#else
                            DeletedFile.directory_location.ulSector = 0; // {11} reset deleted file monitoring from previous directory
#endif
                            iMatchedParagraph = 1;                       // this paragraph matched, but not last one
#if defined UTFAT_LFN_WRITE
                            uMemset(ptrOpenBlock->DeleteLocation, 0, sizeof(ptrOpenBlock->DeleteLocation)); // {5} reset any deleted location markers since they are not valid for next paragraph
#endif
                        }
#if defined UTFAT_LFN_READ
                        break;
#endif
                    }
#if defined UTFAT_LFN_READ
                }
                break;
            }
#endif
            if (iMatchedParagraph != 0) {
                break;
            }
            if ((iResult = fnNextDirectoryEntry(ptr_utDisk, ptrDiskLocation)) != UTFAT_SUCCESS) { // move to next directory entry {3} remember the return value
                if (iMatchedParagraph != 0) {
                    return UTFAT_SUCCESS;
                }
                if (ptrOpenBlock->iQualifiedPathType != SHORT_NAME_PARAGRAPH) {
                    if (ptrOpenBlock->usDirFlags & UTDIR_SET_START) {
#if defined UTFAT_LFN_WRITE
                        if ((iResult == UTFAT_DIRECTORY_AREA_EXHAUSTED) && (ulAccessMode & UTFAT_CREATE)) { // {3} the end of the directory is at the end of the present cluster chain and a new file/directoy is to be created
                            // Extend the cluster chain so that the new file can be created in the next one
                            //
                            ptrDiskLocation->directory_location.ulSector--; // set back to previous sector
                            iResult = fnDirectorySectorCreate(ptr_utDisk, &ptrDiskLocation->directory_location); // create additional directory cluster
                            if (iResult != UTFAT_SUCCESS) {
                                return iResult;                          // return error
                            }
                        }
                        else {
                            uMemcpy(ptrDiskLocation, &DirStart, sizeof(DirStart)); // set the location to the start of the lowest directory
                        }
#else
                        uMemcpy(ptrDiskLocation, &DirStart, sizeof(DirStart)); // set the location to the start of the lowest directory
#endif
                    }
                    return UTFAT_FILE_NOT_FOUND;
                }
                else {
                    return UTFAT_PATH_NOT_FOUND;
                }
            }
        } while (1);
    }
}

static int _fnHandlePath(OPEN_FILE_BLOCK *ptrOpenBlock, const CHAR *ptrDirPath, UTDIRECTORY *ptrDirObject)
{
    UTDISK *ptr_utDisk = &utDisks[ptrDirObject->ucDrive];

    if ((ptr_utDisk->usDiskFlags & DISK_MOUNTED) == 0) {                 // if the disk is not ready for use quit immediately
        return UTFAT_DISK_NOT_READY;
    }

    ptrOpenBlock->iRootDirectory = 0;

    if (ptrDirPath != 0) {                                               // if there is a path string
        int iMoveUp = 0;
        unsigned short usPathTerminator = ptrDirObject->usRelativePathLocation; // present location path terminator
        while ((*ptrDirPath == '.') && (*(ptrDirPath + 1) == '.')) {     // if input ".."
            if (ptrDirObject->ptrDirectoryPath == 0) {                   // if no directory string path in use only referenced paths possible so always move to root
                ptrDirObject->usDirectoryFlags |= UTDIR_TEST_FULL_PATH;
                ptrDirPath = 0;
                break;
            }

            if (usPathTerminator <= 3) {                                 // can't move up any further
                return UTFAT_PATH_NOT_FOUND;
            }
            if (iMoveUp != 0) {
                usPathTerminator--;
            }
            while ((ptrDirObject->ptrDirectoryPath[usPathTerminator] != BACK_SLASH) && (ptrDirObject->ptrDirectoryPath[usPathTerminator] != FORWARD_SLASH)) {
                usPathTerminator--;
            }
            ptrDirPath += 2;
            iMoveUp++;
            if (*ptrDirPath == 0) {
                break;
            }
            ptrDirPath++;
        }
        if (iMoveUp != 0) {                                              // if we are to move up in the path ("../..")
            ptrDirObject->usDirectoryFlags |= UTDIR_TEST_FULL_PATH;      // use full path to set directory but don't move original (this flag is automatically reset)
            ptrOpenBlock->usDirFlags = ptrDirObject->usDirectoryFlags;   // backup original flags
            if (usPathTerminator > 3) {                                  // not root directory
                int iReturn;
                CHAR cTemp = ptrDirObject->ptrDirectoryPath[usPathTerminator]; // backup the location
                ptrDirObject->ptrDirectoryPath[usPathTerminator] = 0;    // temporary termination
                ptrDirObject->usDirectoryFlags &= ~UTDIR_DIR_AS_FILE;    // this may not be set when moving to next directory location
                if ((iReturn = utOpenDirectory(&ptrDirObject->ptrDirectoryPath[3], ptrDirObject)) != UTFAT_SUCCESS) { // try to locate the new directory
                    ptrDirObject->ptrDirectoryPath[usPathTerminator] = cTemp;// correct the original path
                    return UTFAT_PATH_NOT_FOUND;
                }
                if (*ptrDirPath != 0) {                                   // if the path should go downwards after locating the upward path location
                    ptrDirObject->usDirectoryFlags = ((ptrOpenBlock->usDirFlags & ~UTDIR_TEST_FULL_PATH) | UTDIR_TEST_FULL_PATH_TEMP);
                    if ((iReturn = utOpenDirectory(ptrDirPath, ptrDirObject)) != UTFAT_SUCCESS) { // continue downward search from the new location
                        ptrOpenBlock->usDirFlags = 0;
                    }
                    else if (ptrOpenBlock->usDirFlags & UTDIR_ALLOW_MODIFY_PATH) {
                        CHAR *ptrStart = &ptrDirObject->ptrDirectoryPath[usPathTerminator];                        
                        usPathTerminator += (uStrcpy(ptrStart, (ptrDirPath - 1)) - ptrStart);
                        return UTFAT_SUCCESS_PATH_MODIFIED;
                    }
                }
                if (ptrOpenBlock->usDirFlags & UTDIR_ALLOW_MODIFY_PATH) {// if path modification allowed
                    ptrDirObject->usRelativePathLocation = usPathTerminator;
                    return UTFAT_SUCCESS_PATH_MODIFIED;
                }
                ptrDirObject->ptrDirectoryPath[usPathTerminator] = cTemp;// correct the original path
                return iReturn;
            }
            else {
                if (ptrOpenBlock->usDirFlags & UTDIR_ALLOW_MODIFY_PATH) {// if path modification allowed
                    ptrDirObject->ptrDirectoryPath[3] = 0;
                    ptrDirObject->usRelativePathLocation = 3;
                    uMemcpy(&ptrDirObject->public_disk_location, &ptrDirObject->root_disk_location, sizeof(ptrDirObject->private_disk_location)); // synchronise to root location
                    return UTFAT_SUCCESS_PATH_MODIFIED;
                }
            }
        }
    }
    ptrOpenBlock->ptrLocalDirPath = ptrDirPath;
    ptrOpenBlock->usDirFlags = ptrDirObject->usDirectoryFlags;           // flags on entry

    ptrDirObject->usDirectoryFlags &= ~(UTDIR_TEST_FULL_PATH | UTDIR_TEST_FULL_PATH_TEMP | UTDIR_TEST_REL_PATH | UTDIR_DIR_AS_FILE | UTDIR_SET_START | UTDIR_ALLOW_MODIFY_PATH); // these flags are automatically cleared

    if ((ptrOpenBlock->ptrLocalDirPath == 0) || (*ptrOpenBlock->ptrLocalDirPath == 0) || ((*ptrOpenBlock->ptrLocalDirPath == LINE_FEED) || (*ptrOpenBlock->ptrLocalDirPath == CARRIAGE_RETURN))) { // if root directory
        ptrOpenBlock->iRootDirectory = ROOT_DIRECTORY_REFERENCE;         // ready to work with root directory or referenced directory
    }
    else if (((*ptrOpenBlock->ptrLocalDirPath == BACK_SLASH) || (*ptrOpenBlock->ptrLocalDirPath == FORWARD_SLASH))) {
        ptrOpenBlock->usDirFlags |= UTDIR_TEST_REL_PATH;
        if (*(ptrOpenBlock->ptrLocalDirPath + 1) != 0) {                 // root directory referenced
            if ((*(ptrOpenBlock->ptrLocalDirPath + 1) == BACK_SLASH) || (*(ptrOpenBlock->ptrLocalDirPath + 1) == FORWARD_SLASH)) { // this allows "/\" to be recognised as root
                ptrOpenBlock->iRootDirectory = ROOT_DIRECTORY_SET;       // ready to work with root directory explicitly
            }
            else {
                ptrOpenBlock->iRootDirectory = ROOT_DIRECTORY_RELOCATE;  // ready to work with root directory or referenced directory
            }
        }
        else {
            ptrOpenBlock->iRootDirectory = ROOT_DIRECTORY_SET;           // ready to work with root directory explicitly
        }
    }

    if ((ptrOpenBlock->usDirFlags & UTDIR_VALID) == 0) {
        ptrOpenBlock->ulCluster = ptr_utDisk->ulDirectoryBase;           // the first cluster in the root directory
        ptrDirObject->private_disk_location.directory_location.ulCluster = ptrOpenBlock->ulCluster;
#if defined UTFAT16 || defined UTFAT12
        if ((ptrOpenBlock->ulCluster <= 1) && (ptr_utDisk->usDiskFlags & (DISK_FORMAT_FAT12 | DISK_FORMAT_FAT16))) { // if FAT12/FAT16 root folder
            ptrDirObject->private_disk_location.directory_location.ulSector = (ptr_utDisk->utFAT.ucSectorsPerCluster + ptr_utDisk->ulVirtualBaseAddress - (32 - 1));// the sector in which the directory entries begin
        }
        else {
            ptrDirObject->private_disk_location.directory_location.ulSector = (ptrOpenBlock->ulCluster * ptr_utDisk->utFAT.ucSectorsPerCluster);// section referenced to logical base address
            ptrDirObject->private_disk_location.directory_location.ulSector += ptr_utDisk->ulVirtualBaseAddress;// the sector in which the directory entries begin
        }
#else
        ptrDirObject->private_disk_location.directory_location.ulSector = (ptrOpenBlock->ulCluster * ptr_utDisk->utFAT.ucSectorsPerCluster);// section referenced to logical base address
        ptrDirObject->private_disk_location.directory_location.ulSector += ptr_utDisk->ulVirtualBaseAddress;// the sector in which the directory entries begin
#endif
        ptrDirObject->private_disk_location.ucDirectoryEntry = 0;        // reset the entry index
        uMemcpy(&ptrDirObject->root_disk_location, &ptrDirObject->private_disk_location, sizeof(ptrDirObject->root_disk_location)); // enter the fixed root location
        uMemcpy(&ptrDirObject->public_disk_location, &ptrDirObject->private_disk_location, sizeof(ptrDirObject->root_disk_location)); // ensure that the public entry can not be used with invalid value
        ptrDirObject->usDirectoryFlags = (UTDIR_ALLOCATED | UTDIR_VALID | UTDIR_REFERENCED);// the entry is now valid
        ptrOpenBlock->iRootDirectory |= ROOT_DIRECTORY_SETTING;          // mark that the (virtual) root has been set
        if (ptrDirObject->ptrDirectoryPath != 0) {                       // if a root path is being used and root is to be set, set also for virtual root location 
#if defined DISK_C
            uStrcpy(ptrDirObject->ptrDirectoryPath, "C:\\");             // set root path
#elif defined DISK_D
            uStrcpy(ptrDirObject->ptrDirectoryPath, "D:\\");             // set root path
#elif defined DISK_E
            uStrcpy(ptrDirObject->ptrDirectoryPath, "E:\\");             // set root path
#endif
#if DISK_COUNT > 1
            ptrDirObject->ptrDirectoryPath[0] += ptrDirObject->ucDrive;  // adjust the drive letter
#endif
            ptrDirObject->usRelativePathLocation = 3;                    // the relative path is equal to the root path
        }
    }

    if (ptrOpenBlock->usDirFlags & (UTDIR_TEST_FULL_PATH | UTDIR_TEST_FULL_PATH_TEMP | UTDIR_TEST_REL_PATH | UTDIR_REFERENCED)) {
        if (ptrOpenBlock->usDirFlags & UTDIR_TEST_FULL_PATH) {           // set temporary root directory
            uMemcpy(&ptrDirObject->public_disk_location, &ptrDirObject->root_disk_location, sizeof(ptrDirObject->private_disk_location)); // {102a} synchronise to root location
        }
        else if ((ptrOpenBlock->usDirFlags & UTDIR_TEST_FULL_PATH_TEMP) == 0) {
            if (ptrOpenBlock->iRootDirectory & (ROOT_DIRECTORY_RELOCATE | ROOT_DIRECTORY_SET)) {
                uMemcpy(&ptrDirObject->public_disk_location, &ptrDirObject->root_disk_location, sizeof(ptrDirObject->public_disk_location)); // synchronise to root
                if (ptrOpenBlock->iRootDirectory == ROOT_DIRECTORY_SET) {
                    return UTFAT_PATH_IS_ROOT;                           // inform that the location is root
                }
            }
            else {
                uMemcpy(&ptrDirObject->public_disk_location, &ptrDirObject->private_disk_location, sizeof(ptrDirObject->public_disk_location)); // synchronise to present location
            }
        }                                                                // UTDIR_TEST_FULL_PATH_TEMP continues using present public values to continue a search
        ptrOpenBlock->ptrDiskLocation = &ptrDirObject->public_disk_location;        
    }
    else {
        ptrOpenBlock->ptrDiskLocation = &ptrDirObject->private_disk_location; // work with the private disk location pointer so that its adsolute base location is set
        uMemcpy(&ptrDirObject->public_disk_location, &ptrDirObject->private_disk_location, sizeof(ptrDirObject->private_disk_location)); // synchronise to present location
    }
    if (ptrOpenBlock->iRootDirectory & (ROOT_DIRECTORY_REFERENCE | ROOT_DIRECTORY_SET)) { // if root directory reference
        return UTFAT_SUCCESS;                                            // ready to work with root directory or referenced directory
    }
    ptrOpenBlock->iContinue = 1;                                         // the function has done its work and requires that the caller function continues to complete
    return UTFAT_SUCCESS;
}

// Open or modify a directory object
//
extern int utOpenDirectory(const CHAR *ptrDirPath, UTDIRECTORY *ptrDirObject)
{
    int iReturn;
    OPEN_FILE_BLOCK openBlock;
    uMemset(&openBlock, 0, sizeof(openBlock));                           // initialise open file block

    iReturn = _fnHandlePath(&openBlock, ptrDirPath, ptrDirObject);       // handle the input path string
    if (openBlock.iContinue == 0) {                                      // if the path handling had an error or completed all work
        return iReturn;                                                  // return with code
    }

    return _utOpenDirectory(&openBlock, ptrDirObject, 0);
}

    #if defined UTFAT_FILE_CACHE_POOL && UTFAT_FILE_CACHE_POOL > 0
static FILE_DATA_CACHE *fnGetDataCache(void)
{
    int i;
    for (i = 0; i < UTFAT_FILE_CACHE_POOL; i++) {                        // attempt to find a free buffer in the file data cache pool
        if (FileDataCache[i].ucFileCacheState == FILE_BUFFER_FREE) {     // if a free cache is found
            FileDataCache[i].ucFileCacheState = (FILE_BUFFER_IN_USE);    // initial cache state
            return &FileDataCache[i];                                    // allocate a data cache for use by the file
        }
    }
    return 0;                                                            // presently no free cache available in the pool
}

static int fnHandleFileDataCache(UTDISK *ptr_utDisk, UTFILE *ptr_utFile, FILE_DATA_CACHE *ptrDataCache, int iNoLoad)
{
    unsigned char ucDriveNumber = ptr_utDisk->ucDriveNumber;
    if (ptrDataCache->ucFileCacheState & FILE_BUFFER_VALID) {            // the cache has valid data in it
        if (ptrDataCache->ulFileCacheSector != ptr_utFile->public_file_location.ulSector) { // the sector doesn't match
    #if defined UTFAT_WRITE
            if (ptrDataCache->ucFileCacheState & FILE_BUFFER_MODIFIED) { // if modified data waiting to be saved
                while (_utCommitSectorData[ucDriveNumber](ptr_utDisk, ptrDataCache->ucFileDataCache, ptrDataCache->ulFileCacheSector) == CARD_BUSY_WAIT) {} // commit the data to the disk
                ptrDataCache->ucFileCacheState &= ~(FILE_BUFFER_MODIFIED); // no longer needs to be saved
            }
    #endif
            if (iNoLoad == 0) {                                          // if the load operation is not blocked
                if (_utReadDiskSector[ucDriveNumber](ptr_utDisk, ptr_utFile->public_file_location.ulSector, ptrDataCache->ucFileDataCache) != UTFAT_SUCCESS) { // read complete sector directly to buffer
                    return UTFAT_DISK_READ_ERROR;
                }
                ptrDataCache->ulFileCacheSector = ptr_utFile->public_file_location.ulSector; // new valid cached sector
            }
            else {
                ptrDataCache->ucFileCacheState &= ~(FILE_BUFFER_VALID);
            }
        }
    }
    else if (iNoLoad == 0) {                                             // if we have no valid data in the cache we load it now if the load operation is not blocked
        if (_utReadDiskSector[ucDriveNumber](ptr_utDisk, ptr_utFile->public_file_location.ulSector, ptrDataCache->ucFileDataCache) != UTFAT_SUCCESS) { // read complete sector directly to buffer
            return UTFAT_DISK_READ_ERROR;
        }
        ptrDataCache->ulFileCacheSector = ptr_utFile->public_file_location.ulSector; // save the sector that the cached data reflects
        ptrDataCache->ucFileCacheState |= FILE_BUFFER_VALID;             // mark that the cache now holds valid data from this sector
    }
    return UTFAT_SUCCESS;
}
#endif

// Read linear file data content directly to an external buffer
//
extern int utReadFile(UTFILE *ptr_utFile, void *ptrBuffer, unsigned short usReadLength)
{
    UTDISK *ptr_utDisk = &utDisks[ptr_utFile->ucDrive];
    unsigned long ulContentRemaining;
    #if defined UTFAT_FILE_CACHE_POOL && UTFAT_FILE_CACHE_POOL > 0
    FILE_DATA_CACHE *ptrDataCache;
    #endif
    unsigned short usAccessLength;
    unsigned short usAccessOffset;

    ptr_utFile->usLastReadWriteLength = 0;
    if ((ptr_utFile->ulFileMode & UTFAT_OPEN_FOR_READ) == 0) {
        return UTFAT_FILE_NOT_READABLE;                                  // don't allow read of files not opened for read
    }
    ulContentRemaining = (ptr_utFile->ulFileSize - ptr_utFile->ulFilePosition); // the amount of data until the end of the file
    if (ulContentRemaining == 0) {                                       // can't read more data form the end of the file
        return UTFAT_SUCCESS;
    }
    if (usReadLength > ulContentRemaining) {                             // if the user is attempting to read more data than is in the file
        usReadLength = (unsigned short)ulContentRemaining;               // limit to remaining size in file
    }

    #if defined UTFAT_FILE_CACHE_POOL && UTFAT_FILE_CACHE_POOL > 0
    if ((ptr_utFile->ulFileMode & UTFAT_WITH_DATA_CACHE) && (ptr_utFile->ptrFileDataCache == 0)) { // use a file data buffer if one is available in the pool
        ptr_utFile->ptrFileDataCache = fnGetDataCache();                 // try to get data cache to work with
    }
    ptrDataCache = ptr_utFile->ptrFileDataCache;
    #endif

    while (usReadLength != 0) {                                          // while requested length of data has not yet been returned
        usAccessOffset = (unsigned short)(ptr_utFile->ulFilePosition & 0x1ff);
        usAccessLength = (512 - usAccessOffset);
        if (usAccessLength > usReadLength) {
            usAccessLength = usReadLength;
        }
    #if defined UTFAT_FILE_CACHE_POOL && UTFAT_FILE_CACHE_POOL > 0
        if (ptrDataCache != 0) {                                         // if a data cache is being used by this file
            int iReturnValue = fnHandleFileDataCache(ptr_utDisk, ptr_utFile, ptrDataCache, 0); // handle cache - load data to cache if necessary and save any cached data that may require saving
            if (iReturnValue != UTFAT_SUCCESS) {
                return iReturnValue;                                     // access error
            }
            uMemcpy(ptrBuffer, &ptrDataCache->ucFileDataCache[usAccessOffset], usAccessLength); // return the cached data
        }
        else {                                                           // the caller doesn't use a data cache
        #if defined UTFAT_FILE_CACHE_POOL && (UTFAT_FILE_CACHE_POOL > 0) && (UTMANAGED_FILE_COUNT > 0) // check whether another user has written file changes to its data cache which need to be used instead of the data stored on the disk
            if (fnGetManagedFileCache(ptr_utFile->public_file_location.ulSector, ptrBuffer, usAccessOffset, usAccessLength) == 0) { // attempt to get the data form a managed file's data cache
        #endif
    #endif
                if ((usAccessOffset != 0) || (usAccessLength != 512)) {  // only load partical data if a complete sector can not be read directly to the user's buffer
                    if (fnLoadPartialData(ptr_utDisk, ptr_utFile->public_file_location.ulSector, (unsigned char *)ptrBuffer, usAccessOffset, usAccessLength) != UTFAT_SUCCESS) { // read directly to buffer
                        return UTFAT_DISK_READ_ERROR;
                    }
                }
                else {                                                   // load a complete sector directly to the user's buffer
                    if (_utReadDiskSector[ptr_utFile->ucDrive](ptr_utDisk, ptr_utFile->public_file_location.ulSector, (unsigned char *)ptrBuffer) != UTFAT_SUCCESS) { // read complete sector directly to buffer
                        return UTFAT_DISK_READ_ERROR;
                    }
                }
    #if defined UTFAT_FILE_CACHE_POOL && (UTFAT_FILE_CACHE_POOL > 0) && (UTMANAGED_FILE_COUNT > 0)
            }
    #endif
    #if defined UTFAT_FILE_CACHE_POOL && (UTFAT_FILE_CACHE_POOL > 0)
        }
    #endif
        usReadLength -= usAccessLength;
        ptr_utFile->ulFilePosition += usAccessLength;
        ptr_utFile->usLastReadWriteLength += usAccessLength;
        ptrBuffer = (void *)((CAST_POINTER_ARITHMETIC)ptrBuffer + usAccessLength);
        if ((ptr_utFile->ulFilePosition % ptr_utDisk->utFAT.usBytesPerSector) == 0) { // {13} on a sector boundary
            int iResult = fnNextSector(ptr_utDisk, &ptr_utFile->public_file_location);
            if (iResult != UTFAT_SUCCESS) {
                return iResult;
            }
        }
    }
    return UTFAT_SUCCESS;
}


// Open a list directory referenced to its main directory object, which must already have been configured
//
extern int utLocateDirectory(const CHAR *ptrDirPath, UTLISTDIRECTORY *ptrListDirectory)
{
    int iReturn = UTFAT_SUCCESS;
    if (ptrListDirectory->ptr_utDirObject == 0) {
        return UTFAT_DIRECTORY_OBJECT_MISSING;
    }
    if ((iReturn = utOpenDirectory(ptrDirPath, ptrListDirectory->ptr_utDirObject)) >= UTFAT_SUCCESS) {
        uMemcpy(&ptrListDirectory->private_disk_location, &ptrListDirectory->ptr_utDirObject->public_disk_location, sizeof(ptrListDirectory->private_disk_location)); // copy the referenced directory details to the list directory object
    }
    return iReturn;
}

#if defined UTFAT_WRITE
// This routine sets a time and data to a file object - it uses if fixed value if no local date/time is present
//
static void fnSetTimeDate(DIR_ENTRY_STRUCTURE_FAT32 *ptrEntry, int iCreation)
{
    #define CREATION_HOURS         12                                    // fixed date/time stamp used if no other information is available
    #define CREATION_MINUTES       00
    #define CREATION_SECONDS       00

    #define CREATION_DAY_OF_MONTH  26
    #define CREATION_MONTH_OF_YEAR 10
    #define CREATION_YEAR          (2015 - 1980)

    unsigned short usCreationTime;
    unsigned short usCreationDate;
    #if defined SUPPORT_FILE_TIME_STAMP                                  // retrieve the date/time from the time manager
    if (fnGetLocalFileTime(&usCreationTime, &usCreationDate) != 0) {
        usCreationTime = (CREATION_SECONDS | (CREATION_MINUTES << 5) | (CREATION_HOURS << 11));
        usCreationDate = (CREATION_DAY_OF_MONTH | (CREATION_MONTH_OF_YEAR << 5) | (CREATION_YEAR << 9));
    }
    #else
    usCreationTime = (CREATION_SECONDS | (CREATION_MINUTES << 5) | (CREATION_HOURS << 11));
    usCreationDate = (CREATION_DAY_OF_MONTH | (CREATION_MONTH_OF_YEAR << 5) | (CREATION_YEAR << 9));
    #endif
    ptrEntry->DIR_WrtTime[0] = (unsigned char)(usCreationTime);
    ptrEntry->DIR_WrtTime[1] = (unsigned char)(usCreationTime >> 8);
    ptrEntry->DIR_LstAccDate[0] = ptrEntry->DIR_WrtDate[0] = (unsigned char)(usCreationDate);
    ptrEntry->DIR_LstAccDate[1] = ptrEntry->DIR_WrtDate[1] = (unsigned char)(usCreationDate >> 8);
    if (iCreation != 0) {                                                // add creation time and not just modify write time
        ptrEntry->DIR_CrtTime[0] = ptrEntry->DIR_WrtTime[0];
        ptrEntry->DIR_CrtTime[1] = ptrEntry->DIR_WrtTime[1];
        ptrEntry->DIR_CrtDate[0] = ptrEntry->DIR_LstAccDate[0];
        ptrEntry->DIR_CrtDate[1] = ptrEntry->DIR_LstAccDate[1];
    }
}

// Add a new file entry with zero file length and creation date/time
//
static void fnAddEntry(DIR_ENTRY_STRUCTURE_FAT32 *ptrEntry, unsigned long ulPresentCluster, unsigned char ucAttributes)
{
    ptrEntry->DIR_FstClusLO[0] = (unsigned char)ulPresentCluster;
    ptrEntry->DIR_FstClusLO[1] = (unsigned char)(ulPresentCluster >> 8);
    ptrEntry->DIR_FstClusHI[0] = (unsigned char)(ulPresentCluster >> 16);
    ptrEntry->DIR_FstClusHI[1] = (unsigned char)(ulPresentCluster >> 24);
    ptrEntry->DIR_Attr = ucAttributes;
    uMemset(ptrEntry->DIR_FileSize, 0, sizeof(ptrEntry->DIR_FileSize));  // set the file size to zero
    fnSetTimeDate(ptrEntry, 1);                                          // add creation time
}

// Update file size and write date/time
//
static void fnSetFileInformation(DIR_ENTRY_STRUCTURE_FAT32 *ptrFileEntry, unsigned long ulFileSize)
{
    ptrFileEntry->DIR_FileSize[0] = (unsigned char)(ulFileSize);         // update the file size
    ptrFileEntry->DIR_FileSize[1] = (unsigned char)(ulFileSize >> 8);
    ptrFileEntry->DIR_FileSize[2] = (unsigned char)(ulFileSize >> 16);
    ptrFileEntry->DIR_FileSize[3] = (unsigned char)(ulFileSize >> 24);
    fnSetTimeDate(ptrFileEntry, 0);                                      // set the modification time
}

static int fnCommitFileInfo(UTFILE *ptr_utFile, UTDISK *ptr_utDisk)
{
    DIR_ENTRY_STRUCTURE_FAT32 *ptrFileEntry = (DIR_ENTRY_STRUCTURE_FAT32 *)ptr_utDisk->ptrSectorData; // the directory entry in the sector buffer
    ptrFileEntry += ptr_utFile->private_disk_location.ucDirectoryEntry;  // move to the file entry
    if (fnLoadSector(ptr_utDisk, ptr_utFile->private_disk_location.directory_location.ulSector) != UTFAT_SUCCESS) { // ensure that the directory sector is loaded
        return UTFAT_DISK_READ_ERROR;
    }
    fnSetFileInformation(ptrFileEntry, ptr_utFile->ulFileSize);
    while (_utCommitSectorData[ptr_utDisk->ucDriveNumber](ptr_utDisk, ptr_utDisk->ptrSectorData, ptr_utFile->private_disk_location.directory_location.ulSector) == CARD_BUSY_WAIT) {} // force writeback to finalise the operation
    ptr_utDisk->usDiskFlags &= ~WRITEBACK_BUFFER_FLAG;                   // the disk is up to date with the buffer
    return UTFAT_SUCCESS;
}
#endif

#if defined UTFAT12
static unsigned long fnGetFat12_sector(unsigned long ulClusterNumber)
{
     return (((ulClusterNumber + ulClusterNumber + ulClusterNumber)/(512/sizeof(signed long)))/3); // the fat sector where the start of the fat12 cluster entry is located
}

static int fnGetFat12_tripplet(unsigned long ulClusterNumber)
{
    int iFat12_tripplet = (ulClusterNumber%3);
    if (iFat12_tripplet != 0) {
        if (iFat12_tripplet == 1) {
            iFat12_tripplet = 2;
        }
        else {
            iFat12_tripplet = 1;
        }
    }
    return iFat12_tripplet;
}

static int fnGetFat12_cluster_entry(unsigned long ulPresentCluster, FAT12_FAT *ptr_fat12_fat)
{
    int iClusterEntry;
    ptr_fat12_fat->ulFat12SectorNumber = fnGetFat12_sector(ulPresentCluster);  // the fat sector where the start of the fat12 cluster entry is located
    ptr_fat12_fat->iFat12Tripplet = fnGetFat12_tripplet(ptr_fat12_fat->ulFat12SectorNumber); // get the tripplet reference
    iClusterEntry = (ulPresentCluster - ((ptr_fat12_fat->ulFat12SectorNumber/3) * 1024)); // offset into a tripplet
    iClusterEntry *= 3;
    iClusterEntry -= (1024 * ptr_fat12_fat->iFat12Tripplet);
    ptr_fat12_fat->iFat12Offset = (ptr_fat12_fat->iFat12Tripplet + (iClusterEntry/3));
    iClusterEntry /= 8;                                                  // the long word entry offset
    return iClusterEntry;
}

static unsigned long fnExtractFat12_cluster_value(FAT12_FAT *ptr_fat12_fat, unsigned long *ptrSource, int iAdditionalInput)
{
    unsigned long ulClusterEntryContent = LITTLE_LONG_WORD(*ptrSource);
    switch ((ptr_fat12_fat->iFat12Offset + (ptr_fat12_fat->iFat12Tripplet * 8))%8) {
    case 0:
        ulClusterEntryContent &= 0x00000fff;
        break;
    case 1:
        ulClusterEntryContent >>= 12;
        ulClusterEntryContent &= 0x00000fff;
        break;
    case 2:
        ulClusterEntryContent >>= 24;
        if (iAdditionalInput != 0) {
            ulClusterEntryContent |= ((LITTLE_LONG_WORD(*(ptrSource + 1)) & 0x0000000f) << 8);
            ptr_fat12_fat->iNoEntryIncrement = 0;
        }
        else {
            _EXCEPTION("GET first word from next sector");
        }
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
        if (iAdditionalInput != 0) {
            ulClusterEntryContent |= ((LITTLE_LONG_WORD(*(ptrSource + 1)) & 0x000000ff) << 4);
            ptr_fat12_fat->iNoEntryIncrement = 0;
        }
        else {
            _EXCEPTION("GET first word from next sector");
        }
        break;
    case 6:
        ulClusterEntryContent >>= 8;
        ulClusterEntryContent &= 0x00000fff;
        break;
    case 7:
        ulClusterEntryContent >>= 20;
        ptr_fat12_fat->iNoEntryIncrement = 0;
        break;
    }
    return ulClusterEntryContent;
}

static int fnInsertFat12_cluster(FAT12_FAT *ptr_fat12_fat, unsigned long ulClusterValue, unsigned long *ptrDestination, int iAdditionalInput)
{
    switch ((ptr_fat12_fat->iFat12Offset + (ptr_fat12_fat->iFat12Tripplet * 8))%8) {
    case 0:
        *ptrDestination &= ~(LITTLE_LONG_WORD(FAT12_CLUSTER_MASK));
        *ptrDestination |= (LITTLE_LONG_WORD(ulClusterValue));
        break;
    case 1:
        *ptrDestination &= ~(LITTLE_LONG_WORD(FAT12_CLUSTER_MASK << 12));
        *ptrDestination |= (LITTLE_LONG_WORD(ulClusterValue << 12));
        break;
    case 2:
        *ptrDestination &= ~(LITTLE_LONG_WORD(FAT12_CLUSTER_MASK << 24));
        *ptrDestination |= (LITTLE_LONG_WORD(ulClusterValue << 24));
        if (iAdditionalInput != 0) {
            *(ptrDestination + 1) &= ~(LITTLE_LONG_WORD(FAT12_CLUSTER_MASK >> 8));
            *(ptrDestination + 1) |= (LITTLE_LONG_WORD(ulClusterValue >> 8));
        }
        else {
            _EXCEPTION("SET first word in next sector");
        }
        break;
    case 3:
        *ptrDestination &= ~(LITTLE_LONG_WORD(FAT12_CLUSTER_MASK << 4));
        *ptrDestination |= (LITTLE_LONG_WORD(ulClusterValue << 4));
        break;
    case 4:
        *ptrDestination &= ~(LITTLE_LONG_WORD(FAT12_CLUSTER_MASK << 16));
        *ptrDestination |= (LITTLE_LONG_WORD(ulClusterValue << 16));
        break;
    case 5:
        *ptrDestination &= ~(LITTLE_LONG_WORD(FAT12_CLUSTER_MASK << 28));
        *ptrDestination |= (LITTLE_LONG_WORD(ulClusterValue << 28));
        if (iAdditionalInput != 0) {
            *(ptrDestination + 1) &= ~(LITTLE_LONG_WORD(FAT12_CLUSTER_MASK >> 4));
            *(ptrDestination + 1) |= (LITTLE_LONG_WORD(ulClusterValue >> 4));
        }
        else {
            _EXCEPTION("SET first word in next sector");
        }
        break;
    case 6:
        *ptrDestination &= ~(LITTLE_LONG_WORD(FAT12_CLUSTER_MASK << 8));
        *ptrDestination |= (LITTLE_LONG_WORD(ulClusterValue << 8));
        break;
    case 7:
        *ptrDestination &= ~(LITTLE_LONG_WORD(FAT12_CLUSTER_MASK << 20));
        *ptrDestination |= (LITTLE_LONG_WORD(ulClusterValue << 20));
        break;
    }
    return 0;
}
#endif

#if defined UTFAT_WRITE
// Allocate a single new cluster
//
static unsigned long fnAllocateCluster(UTDISK *ptr_utDisk, unsigned long ulPresentCluster, unsigned char ucClusterType)
{
    int iClusterEntry;
    int iOriginalCluster;
    int iClusterMax;
    unsigned long ulClusterEntryContent = 0;
    unsigned long ulFAT = ptr_utDisk->utFAT.ulFAT_start;                 // sector in which the file's first cluster is located in
    unsigned long ulFatOriginal;
    unsigned long ulSectorContent[512/sizeof(signed long)];              // sector to read FAT32 sectors to
    unsigned long ulAbsoluteCluster;
    unsigned char ucDriveNumber = ptr_utDisk->ucDriveNumber;
    #if defined UTFAT12
    FAT12_FAT fat12_fat = {0};
    FAT12_FAT original_fat12_fat = {0};
    #endif
    if (ucClusterType & NEW_RELATIVE_CLUSTER) {
        ulAbsoluteCluster = ulPresentCluster;
    }
    else {
        if ((ptr_utDisk->usDiskFlags & FSINFO_VALID) && (ptr_utDisk->utFileInfo.ulNextFreeCluster <= ptr_utDisk->utFileInfo.ulFreeClusterCount)) { // the info block is valid so reference to next free cluster
            ulAbsoluteCluster = ptr_utDisk->utFileInfo.ulNextFreeCluster;
        }
        else {
            ulAbsoluteCluster = ptr_utDisk->ulDirectoryBase;
        }
        ulPresentCluster = ulAbsoluteCluster;
    }
    #if defined UTFAT16 || defined UTFAT12
    if (ptr_utDisk->usDiskFlags & (DISK_FORMAT_FAT12)) {
        #if defined UTFAT12
        iClusterEntry = fnGetFat12_cluster_entry(ulPresentCluster, &fat12_fat); // get information about where the FAT12 entry is located
        ulFAT += fat12_fat.ulFat12SectorNumber;                          // the fat sector that the cluster information starts in
        iClusterMax = ((512/sizeof(unsigned long)) - 1);                 // the number of clusters controlled in this fat sector
        uMemcpy(&original_fat12_fat, &fat12_fat, sizeof(fat12_fat));     // backup the starting location
        #endif
    }
        #if defined UTFAT16
    else if (ptr_utDisk->usDiskFlags & (DISK_FORMAT_FAT16)) {
        iClusterEntry = (unsigned char)ulPresentCluster;                 // the long word entry offset
        ulFAT += (ulPresentCluster/(512/sizeof(signed short)));          // the fat sector that the cluster information starts in
        iClusterMax = ((512/sizeof(unsigned short)) - 1);                // the number of clusters controlled in this fat sector
    }
        #endif
    else {
        ulFAT += (ulPresentCluster/(512/sizeof(signed long)));           // the fat sector that the cluster information starts in
        iClusterEntry = (ulPresentCluster & ((512/sizeof(signed long)) - 1)); // the offset in the fat sector
        iClusterMax = ((512/sizeof(unsigned long)) - 1);                 // the number of clusters controlled in this fat sector
    }
    #else
    ulFAT += (ulPresentCluster/(512/sizeof(signed long)));               // the fat sector that the cluster information starts in
    iClusterEntry = (ulPresentCluster & ((512/sizeof(signed long)) - 1));// the offset in the fat sector
    iClusterMax = ((512/sizeof(unsigned long)) - 1);                     // the number of clusters controlled in this fat sector
    #endif
    ulFatOriginal = ulFAT;                                               // remember the starting locations
    iOriginalCluster = iClusterEntry;
    while (1) {
        if ((_utReadDiskSector[ucDriveNumber](ptr_utDisk, ulFAT, ulSectorContent)) != UTFAT_SUCCESS) { // read a FAT sector containing the (start of) cluster information
            return (unsigned long)UTFAT_DISK_READ_ERROR;
        }
        while (1) {
    #if defined UTFAT16 || defined UTFAT12
            if (ptr_utDisk->usDiskFlags & (DISK_FORMAT_FAT12)) {
        #if defined UTFAT12
                fat12_fat.iNoEntryIncrement = 1;
                ulClusterEntryContent = fnExtractFat12_cluster_value(&fat12_fat, &ulSectorContent[iClusterEntry], (iClusterEntry < (512/(sizeof(unsigned long) - 1))));
        #endif
            }
        #if defined UTFAT16
            else if (ptr_utDisk->usDiskFlags & (DISK_FORMAT_FAT16)) {
                ulClusterEntryContent = LITTLE_LONG_WORD(ulSectorContent[iClusterEntry/2]);
                if (iClusterEntry & 1) {
                    ulClusterEntryContent >>= 16;
                }
                else {
                    ulClusterEntryContent &= 0x0000ffff;
                }
            }
        #endif
            else {
                ulClusterEntryContent = ulSectorContent[iClusterEntry];  // directly read the cluster location
            }
    #else
            ulClusterEntryContent = ulSectorContent[iClusterEntry];      // directly read the cluster location
    #endif
            if (ulClusterEntryContent == 0) {                            // next free cluster location found
                unsigned char ucFatCopyCount = 0;
                if (ucClusterType & (INITIALISE_DIR_CLUSTER | INITIALISE_DIR_EXTENSION)) { // the new cluster must be configured as empty directory
                    unsigned char ucSectors = ptr_utDisk->utFAT.ucSectorsPerCluster;
                    unsigned long ulSectorToDelete = ((ulAbsoluteCluster * ptr_utDisk->utFAT.ucSectorsPerCluster) + ptr_utDisk->ulVirtualBaseAddress);
                    while (ucSectors != 0) {
                        if ((ucSectors-- == ptr_utDisk->utFAT.ucSectorsPerCluster) && (ucClusterType & INITIALISE_DIR_CLUSTER)) { // first sector - add "." and ".." entries
                            unsigned char ucDirectoryDefault[512];
                            DIR_ENTRY_STRUCTURE_FAT32 *ptrEntry = (DIR_ENTRY_STRUCTURE_FAT32 *)ucDirectoryDefault;
                            uMemset(ucDirectoryDefault, 0x00, sizeof(ucDirectoryDefault));
                            uMemset(ptrEntry->DIR_Name, ' ', sizeof(ptrEntry->DIR_Name));
                            ptrEntry->DIR_Name[0] = '.';
                            fnAddEntry(ptrEntry, ulPresentCluster, DIR_ATTR_DIRECTORY);
                            ptrEntry++;
                            uMemset(ptrEntry->DIR_Name, ' ', sizeof(ptrEntry->DIR_Name));
                            ptrEntry->DIR_Name[0] = '.';
                            ptrEntry->DIR_Name[1] = '.';
                            fnAddEntry(ptrEntry, 2, DIR_ATTR_DIRECTORY);
                            while (_utCommitSectorData[ucDriveNumber](ptr_utDisk, ucDirectoryDefault, ulSectorToDelete) == CARD_BUSY_WAIT) {}
                        }
                        else {
                            while (_utDeleteSector[ucDriveNumber](ptr_utDisk, ulSectorToDelete) == CARD_BUSY_WAIT) {} // delete sector
                        }
                        ulSectorToDelete++;
                    }
                }
    #if defined UTFAT16 || defined UTFAT12
                if (ptr_utDisk->usDiskFlags & (DISK_FORMAT_FAT12)) {
        #if defined UTFAT12
                    fnInsertFat12_cluster(&fat12_fat, FAT12_CLUSTER_MASK, &ulSectorContent[iClusterEntry], (iClusterEntry < (512/(sizeof(unsigned long) - 1))));
        #endif
                }
        #if defined UTFAT16
                else if (ptr_utDisk->usDiskFlags & (DISK_FORMAT_FAT16)) {
                    if (iClusterEntry & 0x01) {
                        ulSectorContent[iClusterEntry/2] |= (LITTLE_LONG_WORD(FAT16_CLUSTER_MASK << 16)); // mark last cluster in extension
                    }
                    else {
                        ulSectorContent[iClusterEntry/2] |= (LITTLE_LONG_WORD(FAT16_CLUSTER_MASK)); // mark last cluster in extension
                    }
                }
        #endif
                else {                                                   // else FAT32
                    ulSectorContent[iClusterEntry] = LITTLE_LONG_WORD(CLUSTER_MASK); // mark last cluster in extension
                }
    #else
                ulSectorContent[iClusterEntry] = LITTLE_LONG_WORD(CLUSTER_MASK); // mark last cluster in extension
    #endif
                if (ucClusterType & UPDATE_FAT_END) {
                    ucClusterType = 0;
    #if defined UTFAT16 || defined UTFAT12
                    if (ptr_utDisk->usDiskFlags & (DISK_FORMAT_FAT12)) {
        #if defined UTFAT12
                        fnInsertFat12_cluster(&original_fat12_fat, ulAbsoluteCluster, &ulSectorContent[iOriginalCluster], (iOriginalCluster < (512/(sizeof(unsigned long) - 1))));
        #endif
                    }
        #if defined UTFAT16
                    else if (ptr_utDisk->usDiskFlags & (DISK_FORMAT_FAT16)) {
                        if (iClusterEntry & 0x01) {
                            ulSectorContent[iOriginalCluster/2] &= ~LITTLE_LONG_WORD(0x0000ffff);
                            ulSectorContent[iOriginalCluster/2] |= LITTLE_LONG_WORD(ulAbsoluteCluster); // mark last cluster in extension
                        }
                        else {
                            iOriginalCluster -= 2;                          // {38}
                            ulSectorContent[iOriginalCluster/2] &= ~LITTLE_LONG_WORD(0xffff0000);
                            ulSectorContent[iOriginalCluster/2] |= LITTLE_LONG_WORD(ulAbsoluteCluster << 16); // mark last cluster in extension
                        }
                    }
        #endif
                    else {
                        ulSectorContent[iOriginalCluster] = LITTLE_LONG_WORD(ulAbsoluteCluster); // modify the original end marker to point to the additional cluster
                    }
    #else
                    ulSectorContent[iOriginalCluster] = LITTLE_LONG_WORD(ulAbsoluteCluster); // modify the original end marker to point to the additional cluster
    #endif
                }
                while (ucFatCopyCount < ptr_utDisk->utFAT.ucNumberOfFATs) {
                    while (_utCommitSectorData[ucDriveNumber](ptr_utDisk, ulSectorContent, (ulFAT + (ucFatCopyCount * ptr_utDisk->utFAT.ulFatSize))) == CARD_BUSY_WAIT) {} // write the new FAT entry
                    ucFatCopyCount++;
                }
                if (ucClusterType & UPDATE_FAT_END_IN_DIFF_SECT) {       // the new cluster end has been marked but the original end must be modified to point to it (it is in a different sector so needs to be modified seperately)
                    ucFatCopyCount = 0;
                    if ((_utReadDiskSector[ucDriveNumber](ptr_utDisk, ulFatOriginal, ulSectorContent)) != UTFAT_SUCCESS) { // read a FAT sector containing the cluster information
                        return (unsigned long)UTFAT_DISK_READ_ERROR;
                    }
    #if defined UTFAT16 || defined UTFAT12
                    if (ptr_utDisk->usDiskFlags & (DISK_FORMAT_FAT12 | DISK_FORMAT_FAT16)) {
                        if (iClusterEntry & 0x01) {
                            ulSectorContent[((unsigned char)ulPresentCluster/2)] &= ~LITTLE_LONG_WORD(0x0000ffff); // unsigned char casting before divide by 2!!
                            ulSectorContent[((unsigned char)ulPresentCluster/2)] |= LITTLE_LONG_WORD(ulAbsoluteCluster); // mark last cluster in extension
                        }
                        else {
                            ulSectorContent[((unsigned char)ulPresentCluster/2)] &= ~LITTLE_LONG_WORD(0xffff0000);
                            ulSectorContent[((unsigned char)ulPresentCluster/2)] |= LITTLE_LONG_WORD(ulAbsoluteCluster << 16); // mark last cluster in extension
                        }
                    }
                    else {
                        ulSectorContent[ulPresentCluster & ((512/sizeof(signed long)) - 1)] = LITTLE_LONG_WORD(ulAbsoluteCluster);
                    }
    #else
                    ulSectorContent[ulPresentCluster & ((512/sizeof(signed long)) - 1)] = LITTLE_LONG_WORD(ulAbsoluteCluster);
    #endif
                    while (ucFatCopyCount < ptr_utDisk->utFAT.ucNumberOfFATs) {
                        while (_utCommitSectorData[ucDriveNumber](ptr_utDisk, ulSectorContent, (ulFatOriginal + (ucFatCopyCount * ptr_utDisk->utFAT.ulFatSize))) == CARD_BUSY_WAIT) {} // write the new FAT entry
                        ucFatCopyCount++;
                    }
                }
                if (ptr_utDisk->usDiskFlags & FSINFO_VALID) {
                    if (ulAbsoluteCluster >= ptr_utDisk->utFileInfo.ulNextFreeCluster) {                        
                        ptr_utDisk->utFileInfo.ulNextFreeCluster = (ulAbsoluteCluster + 1);
                    }
                    ptr_utDisk->usDiskFlags |= WRITEBACK_INFO_FLAG;      // mark that the info block information has changed and should be committed at some point
                    ptr_utDisk->utFileInfo.ulFreeClusterCount--;         // since we have allocated a new cluster there is one less free
                }
                return (ulAbsoluteCluster);
            }
            ulAbsoluteCluster++;
            if (iClusterEntry >= iClusterMax) {
                break;
            }
#if defined UTFAT12
            fat12_fat.iFat12Offset++;
            if (fat12_fat.iNoEntryIncrement == 0) {
                iClusterEntry++;
            }
#else
            iClusterEntry++;
#endif
        }
        ucClusterType &= ~UPDATE_FAT_END;                                // the new FAT entry is in a different FAT sector so needs to be modified later
        iClusterEntry = 0;
        ulFAT++;
    }
}

static int fnCommitInfoChanges(UTDISK *ptr_utDisk)
{
    if (ptr_utDisk->usDiskFlags & WRITEBACK_INFO_FLAG) {                 // info sector content has changed
        INFO_SECTOR_FAT32 info_Sector;
        unsigned long ulInfoSector_Location = ptr_utDisk->utFileInfo.ulInfoSector;
        unsigned char ucDriveNumber = ptr_utDisk->ucDriveNumber;
        if ((_utReadDiskSector[ucDriveNumber](ptr_utDisk, ulInfoSector_Location, &info_Sector)) != UTFAT_SUCCESS) { // read the information sector
            return UTFAT_DISK_READ_ERROR;
        }
        fnAddInfoSect(&info_Sector, ptr_utDisk->utFileInfo.ulFreeClusterCount, ptr_utDisk->utFileInfo.ulNextFreeCluster);
        while (_utCommitSectorData[ucDriveNumber](ptr_utDisk, &info_Sector, ulInfoSector_Location) == CARD_BUSY_WAIT) {}
        ptr_utDisk->usDiskFlags &= ~WRITEBACK_INFO_FLAG;
    }
    return UTFAT_SUCCESS;
}
#endif
#endif

#if defined SDCARD_SUPPORT || defined SPI_FLASH_FAT || defined FLASH_FAT || defined USB_MSD_HOST || (defined FAT_EMULATION && defined EMULATED_FAT_FILE_NAME_CONTROL)
static int fnCreateNameParagraph(const CHAR **pptrDirectoryPath, CHAR cDirectoryName[12])
{
    #if defined UTFAT_LFN_WRITE || (defined FAT_EMULATION && defined FAT_EMULATION_LFN)
    int iLFN_force = FULLY_QUALIFIED_SHORT_NAME;
    #endif
    int iLength = 0;
    int iEvenByte = 0;
    int iSeparatorFound = 0;
    CHAR cInputCharacter;
    unsigned char ucCharacterCharacteristics;

    uMemset(cDirectoryName, ' ', 11);                                    // start with blank short file name
    cDirectoryName[11] = NT_FLAG;
    while (1) {
        cInputCharacter = *(*pptrDirectoryPath)++;
        switch (cInputCharacter) {
        case '.':
            if ((iLength != 0) && (iLength <= 8)) {                      // separator found
                iSeparatorFound = 1;
                iLength = 8;
                continue;
            }
            break;
        case 0x0a:                                                       // for FTP compatibility
        case 0x0d:
            if (iLength == 0) {
                return INVALID_PARAGRAPH;
            }
        case 0:                                                          // end of path string
        case FORWARD_SLASH:                                              // directory level change or terminator
        case BACK_SLASH:
            if (iLength != 0) {                                          // if characters have been collected
                (*pptrDirectoryPath)--;                                  // set back to terminator of file name
    #if defined UTFAT_LFN_WRITE || (defined FAT_EMULATION && defined FAT_EMULATION_LFN)
                return iLFN_force;
    #else
                return ((cInputCharacter == 0) || (**pptrDirectoryPath == 0)); // return true if the end of the path has been found
    #endif
            }                                                            // fall through to ignore
            continue;                                                    // ignore at start of string
        default:
            if ((unsigned char)cInputCharacter <= ' ') {                 // reject all invisible characters
    #if defined UTFAT_LFN_WRITE || (defined FAT_EMULATION && defined FAT_EMULATION_LFN)
                if ((iLength != 0) && (cInputCharacter == ' ')) {        // spaces in the name force LFNs
                    iLFN_force = FULLY_QUALIFIED_LONG_NAME;
                    cInputCharacter = '_';                               // replace long file name accepted characters with '_';
                    break;
                }
    #endif
                continue;
            }
            else {
                if ((unsigned char)cInputCharacter <= '~') {
                    ucCharacterCharacteristics = ucCharacterTable[cInputCharacter - '!'];
                    if (ucCharacterCharacteristics & (_CHAR_REJECT | _CHAR_REJECT_NON_JAP)) {
                        if ((iEvenByte == 0) || (!(ucCharacterCharacteristics & _CHAR_REJECT_NON_JAP))) { // don't apply these to second byte of Japanese characters
    #if defined UTFAT_LFN_WRITE || (defined FAT_EMULATION && defined FAT_EMULATION_LFN)
                            if (ucCharacterCharacteristics & _CHAR_ACCEPT_LFN) {
                                cInputCharacter = '_';                   // replace long file name accepted characters with '_';
                                iLFN_force = FULLY_QUALIFIED_LONG_NAME;  // if this name is to be created it must be a long file name type due to this character
                            }
                            else {
                                continue;                                // reject
                            }
    #else
                            continue;                                    // reject
    #endif
                        }
                    }
                    else if (iEvenByte == 0) {                           // don't apply to second byte of Japanese characters
                        if (ucCharacterCharacteristics & _CHAR_CAPITAL) {
                            if (iSeparatorFound != 1) {
                                cDirectoryName[11] &= ~0x08;
                            }
                            else {
                                cDirectoryName[11] &= ~0x10;
                            }
                        }
                        else if (ucCharacterCharacteristics & _CHAR_SMALL) {
    #if defined UTFAT_LFN_WRITE || (defined FAT_EMULATION && defined FAT_EMULATION_LFN)
                            iLFN_force = FULLY_QUALIFIED_LONG_NAME_SFNM; // if this name is to be created it must be a long file name type due the fact that it has small letters in the name (if it is to be searched for it may still mathc with a SFN)
    #endif
                            cInputCharacter -= ('a' - 'A');              // convert to upper case
                            if (iSeparatorFound != 1) {
                                cDirectoryName[11] |= 0x08;
                            }
                            else {
                                cDirectoryName[11] |= 0x10;
                            }
                        }
                    }
                }
                else if ((((unsigned char)cInputCharacter >= 0x81) && ((unsigned char)cInputCharacter <= 0x9f)) || (((unsigned char)cInputCharacter >= 0xe0) && ((unsigned char)cInputCharacter <= 0xfc))) { // Japanese
                    iEvenByte = 1;
                    if ((iLength == 0) && (cInputCharacter == (CHAR)DIR_NAME_FREE)) {
                        cDirectoryName[iLength++] = 0x05;                // substitute
                        continue;                                        // continue with even byte
                    }
                }
                else {
                    continue;                                            // reject
                }
            }
            break;                                                       // accept
        }
        if (iLength >= 8) {
            if ((iSeparatorFound == 0) || (iLength >= 11)) {             // name part or extension is too long
    #if defined UTFAT_LFN_READ || (defined FAT_EMULATION && defined FAT_EMULATION_LFN)
                while (1) {
                    switch (*(*pptrDirectoryPath)++) {                   // search the end of the long file name paragraph
                    case 0x0d:                                           // end of line considered fully qualified terminator (for FTP compatibility)
                    case 0x0a:
                    case 0:
                        (*pptrDirectoryPath)--;                          // set back to null-terminator of long file name
                        return FULLY_QUALIFIED_LONG_NAME;
                    case FORWARD_SLASH:
                    case BACK_SLASH:
                        *pptrDirectoryPath = (*pptrDirectoryPath - 1);   // set back to end of long file name
                        return LONG_NAME_PARAGRAPH;
                    }
                }
    #else
                break;
    #endif
            }
        }
        iEvenByte = 0;
        cDirectoryName[iLength++] = cInputCharacter;                     // collect short file name
    }
    return INVALID_PARAGRAPH;                                            // invalid
}
#endif

#if defined UTFAT_LFN_READ || (defined FAT_EMULATION && defined FAT_EMULATION_LFN  && defined EMULATED_FAT_FILE_NAME_CONTROL)
// Calculate the checkum of a short file name alias
//
static unsigned char fnLFN_checksum(CHAR *cPtrSFN_alias)
{
   int i;
   unsigned char ucCheckSum = 0;
 
   for (i = 11; i; i--) {
      ucCheckSum = ((ucCheckSum & 1) << 7) + (ucCheckSum >> 1) + *cPtrSFN_alias++;
   }
   return ucCheckSum;
}
#endif

#if defined UTFAT_LFN_WRITE || (defined FAT_EMULATION && defined FAT_EMULATION_LFN  && defined EMULATED_FAT_FILE_NAME_CONTROL)
    #if defined UTFAT_LFN_WRITE_PATCH
// Create a short file name alisas that does not represent a valid file name and minimises the risk
// of causing problems with older systems supporting long file names and causing collision in a directory
// This implementation is based on the original algorithm  used by VFAT which can be found at https://lkml.org/lkml/2009/6/26/313.
// All credits go to the original author, whos comments are retained
//
static void fnCreateInvalidSFN_alias(CHAR cInvalidSFN_alias[12])         // the length includes the NT byte
{       
    unsigned long ulRandom;
    uMemset(cInvalidSFN_alias, 0, 12);                                   // set content to zero including NT byte

   /* we start with a space followed by nul as spaces at the
    * start of an entry are trimmed in FAT, which means that
    * starting the 11 bytes with 0x20 0x00 gives us a value which
    * cannot be used to access the file. It also means that the
    * value as seen from all Windows and Linux APIs is a constant
    */
    cInvalidSFN_alias[0] = ' ';

   /* we use / and 2 nul bytes for the extension. These are
    * invalid in FAT and mean that utilities that show the
    * directory show no extension, but still work via the long
    * name for old Linux kernels
    */
    cInvalidSFN_alias[8] = '/';

   /*
    * fill the remaining 6 bytes with random invalid values
    * This gives us a low collision rate, which means a low
    * chance of problems with chkdsk.exe and WindowsXP
    */
    do {
        ulRandom = (fnRandom() | (fnRandom() << 16));
        ulRandom &= 0x1f1f1f1f;
    } while (ulRandom == 0);                                             // avoid 0 as explained below
    uMemcpy(&cInvalidSFN_alias[2], &ulRandom, 4);                        // fill out [2..5] with random values
    ulRandom = (fnRandom() & 0x1f1f);
    uMemcpy(&cInvalidSFN_alias[6], &ulRandom, 2);                        // fill out [6..7] with random values

   /* a value of zero would leave us with only nul and spaces,
    * which would not work with older linux systems
    */
}
    #else
// Create short file name according to true FAT32 LFN specification
//
static void fnCreateSFN_alias(CHAR cFileName[12])
{
    _EXCEPTION("TO DO!!");                                               // due to various advantages of the workaround in modern embedded systems it is possible that this will not be added
}
    #endif
#endif

#if (defined UTFAT_LFN_READ && defined UTFAT_LFN_WRITE) || (defined FAT_EMULATION && defined FAT_EMULATION_LFN && defined EMULATED_FAT_FILE_NAME_CONTROL)
// Verify that a long file name entry is possible or else move to a location that is possible, which may be the end of the present directory
//
static int fnInsertLFN_name(OPEN_FILE_BLOCK *ptr_openBlock, UTFILE *ptr_utFile, int iRename) // {4}
{
    DIR_ENTRY_STRUCTURE_FAT32 original_file_object;
    DIR_ENTRY_STRUCTURE_FAT32 *ptrDirectoryEntry;
    LFN_ENTRY_STRUCTURE_FAT32 *ptrLongFileEntry = 0;
    UTDISK *ptr_utDisk = ptr_openBlock->ptr_utDisk;
    DISK_LOCATION *ptrDiskLocation = ptr_openBlock->ptrDiskLocation;
    const CHAR *ptrLongFileName = ptr_openBlock->ptrLocalDirPath;        // {4}
    int i;
    int iFileObjectMoved = 0;
    int iNameLength = uStrlen(ptrLongFileName);                          // length of long file name that must be saved
    unsigned char ucEntryLength = 1;                                     // smallest size that can be required by a LFN (SFN will be required after it too)
    const CHAR *ptrReverseLFN = (ptrLongFileName + iNameLength);         // set the LFN name pointer to the end of the string (terminator) since it is going to be copied in reverse order
    unsigned char ucNextCharacter = 0xff;                                // default is pad character
    unsigned char ucNextCharacterExtension = 0xff;
    unsigned char ucSFN_alias_checksum;
    #if defined UTFAT_LFN_WRITE_PATCH
        #if defined FAT_EMULATION && !defined ROOT_DIR_SECTORS           // {12}
    static CHAR cEmulationShortFileName[8 + 3 + 1] = {0};
    if (ptr_utFile == 0) {                                               // only the first long name file of emulated FAT uses a new short file name
        if (cEmulationShortFileName[0] == 0) {                           // this to keep the short file name part stable when the LFN straddles sectors
            fnCreateInvalidSFN_alias(cEmulationShortFileName);           // generate single short file name alias
        }
        uMemcpy(ptr_openBlock->cShortFileName, cEmulationShortFileName, sizeof(ptr_openBlock->cShortFileName)); // share the short file name alias
    }
    else {
        fnCreateInvalidSFN_alias(ptr_openBlock->cShortFileName);
    }
        #else
    fnCreateInvalidSFN_alias(ptr_openBlock->cShortFileName);             // create invalid SFN entry that avoids FAT32 LFN patent issues, doesn't allow compatibility with non-LFN systems and has lowest probability to cause Windows XP or chkdsk problems
        #endif
    #else
    fnCreateSFN_alias(ptr_openBlock->cShortFileName);                    // create short file name according to true FAT32 LFN specification
    #endif
    ucSFN_alias_checksum = fnLFN_checksum(ptr_openBlock->cShortFileName);// calculate the short file name alias's checksum

    iNameLength++;                                                       // include the string terminator in the length
    while (iNameLength > 13) {                                           // calculate the number of entries required to store the long file name in
        ucEntryLength++;
        iNameLength -= 13;                                               // subtract the part of the name that can fit in each single entry
    }
    i = ucEntryLength;                                                   // we check whether there is a known hole in the directory for reuse

    #if defined FAT_EMULATION_LFN                                        // {12}
    if (ptr_utFile != 0) {
    #endif
    #if defined UTFAT_LFN_READ && defined UTFAT_LFN_WRITE
        if ((iRename != 0) && (ptr_utFile->lfn_file_location.directory_location.ulSector != 0) && (ptr_utFile->ucLFN_entries >= ucEntryLength)) { // renamed file is LFN and so its space can be used if large enough
            uMemcpy(ptrDiskLocation, &ptr_utFile->lfn_file_location, sizeof(DISK_LOCATION)); // set the start of the original long file name
            if (ptr_utFile->ucLFN_entries != ucEntryLength) {            // if a location past the original start is to be used move to it
                ptr_utFile->ucLFN_entries -= ucEntryLength;
                while (ptr_utFile->ucLFN_entries != 0) {
                    fnNextDirectoryEntry(ptr_utDisk, ptrDiskLocation);
                    ptr_utFile->ucLFN_entries--;
                }
                ptr_utFile->ucLFN_entries = ucEntryLength;
                uMemcpy(&ptr_utFile->lfn_file_location, ptrDiskLocation, sizeof(DISK_LOCATION));
            }
        }
        else {
            while (i < DELETED_ENTRY_COUNT) {                            // make use of existing deleted area in the directory (starting with a hole of exactly the size required)
                if (ptr_openBlock->DeleteLocation[i].directory_location.ulSector != 0) { // deleted space of adequate size found
                    break;                                               // we have a new reference to the location of the deleted entries
                }
                i++;                                                     // try larger holes
            }
            if (iRename != 0) {                                          // the renamed file must be relocated (original details transferred then destroyed)
                if (fnLoadSector(ptr_utDisk, ptrDiskLocation->directory_location.ulSector) != UTFAT_SUCCESS) {
                    return UTFAT_DISK_READ_ERROR;
                }
                ptrDirectoryEntry = (DIR_ENTRY_STRUCTURE_FAT32 *)ptr_utDisk->ptrSectorData; // the directory entry in the sector buffer
                ptrDirectoryEntry += ptrDiskLocation->ucDirectoryEntry;  // move to the present entry
                uMemcpy(&original_file_object, ptrDirectoryEntry, sizeof(original_file_object)); // backup the origional file object
                ptrDirectoryEntry->DIR_Name[0] = DIR_NAME_FREE;          // free the original object
                ptr_utDisk->usDiskFlags |= WRITEBACK_BUFFER_FLAG;        // mark that the original sector content must be committed
                iFileObjectMoved = 1;
            }
            if (i >= DELETED_ENTRY_COUNT) {                              // no reuse is possible in the existing directory area so the renamed file must be moved to the end of the present directory
                uMemcpy(ptrDiskLocation, &ptr_openBlock->DirectoryEndLocation, sizeof(DISK_LOCATION)); // move to the new LFN location which is extending the directory
            }
            else {
                uMemcpy(ptrDiskLocation, &ptr_openBlock->DeleteLocation[i], sizeof(DISK_LOCATION)); // move to the new LFN location which is reusing a deleted area
            }
        }
        if (fnLoadSector(ptr_utDisk, ptrDiskLocation->directory_location.ulSector) != UTFAT_SUCCESS) { // this is either reusing deleted entry space or creating a new name at the end of the directory
            return UTFAT_DISK_READ_ERROR;
        }
        ptrDirectoryEntry = (DIR_ENTRY_STRUCTURE_FAT32 *)ptr_utDisk->ptrSectorData; // the directory entry in the sector buffer
        ptrDirectoryEntry += ptrDiskLocation->ucDirectoryEntry;          // move to the present entry
    #endif
    #if defined FAT_EMULATION_LFN                                        // {12}
    }
    else {
        ptrDirectoryEntry = (DIR_ENTRY_STRUCTURE_FAT32 *)ptrDiskLocation;
    }
    #endif
    ucEntryLength |= 0x40;                                               // mark first entry
    i = -1;
    while (1) {                                                          // for each character in the LFN
        if (iNameLength >= 13) {                                         // pad end when the LFN doesn't fill an entry
            ucNextCharacter = *ptrReverseLFN;
            ucNextCharacterExtension = 0;                                // only english character set supported
        }
        switch (i--) {
        case 0:                                                          // move to next entry
            {
    #if defined FAT_EMULATION_LFN                                        // {12}
                if (ptr_utFile != 0) {
    #endif
    #if defined UTFAT_LFN_READ && defined UTFAT_LFN_WRITE
                    int iResult = fnNextDirectoryEntry(ptr_utDisk, ptrDiskLocation);
                    if (UTFAT_DIRECTORY_AREA_EXHAUSTED == iResult) {     // the present directory cluster end has been reached so a new one must be started
                        ptrDiskLocation->directory_location.ulSector--;  // set back to previous sector
                        iResult = fnDirectorySectorCreate(ptr_utDisk, &ptrDiskLocation->directory_location); // create additional directory cluster
                    }
                    if (iResult != UTFAT_SUCCESS) {
                        return iResult;                                  // return error
                    }
                    if (fnLoadSector(ptr_utDisk, ptrDiskLocation->directory_location.ulSector) != UTFAT_SUCCESS) {
                        return UTFAT_DISK_READ_ERROR;
                    }
                    ptrDirectoryEntry = (DIR_ENTRY_STRUCTURE_FAT32 *)ptr_utDisk->ptrSectorData; // the directory entry in the sector buffer
                    ptrDirectoryEntry += ptrDiskLocation->ucDirectoryEntry;  // move to the present entry
    #endif
    #if defined FAT_EMULATION_LFN                                        // {12}
                }
                else {
                    int iFileObjectsInSector = ptr_utDisk->utFAT.usBytesPerSector/sizeof(DIR_ENTRY_STRUCTURE_FAT32);
                    ptrDirectoryEntry++;
                    ptr_openBlock->iRootDirectory++;
                    if (ptr_openBlock->iRootDirectory >= iFileObjectsInSector) { // if the end of the sector has been reached
                        ptr_openBlock->iRootDirectory = 0;
                        if (ptr_openBlock->iContinue == 0) {             // if we should't continue filling the name
                            return UTFAT_SUCCESS;
                        }
                        ptrDirectoryEntry -= iFileObjectsInSector;       // loop back to the beginning of the buffer
                    }
                }
    #endif
                if (ptrReverseLFN < ptrLongFileName) {                   // the complete LFN has been added
                    if (iFileObjectMoved != 0) {
                        uMemcpy(ptrDirectoryEntry, &original_file_object, sizeof(original_file_object)); // restore the original file object
                    }
                    return UTFAT_SUCCESS;
                }
            }
            // Fall through intentional
            //
        case -1:
            ptrLongFileEntry = (LFN_ENTRY_STRUCTURE_FAT32 *)ptrDirectoryEntry;
            uMemset(ptrLongFileEntry, 0x00, sizeof(LFN_ENTRY_STRUCTURE_FAT32)); // ensure initially zeroed
            ptrLongFileEntry->LFN_EntryNumber = ucEntryLength;           // entry number
            ucEntryLength &= ~0x40;
            ucEntryLength--;
            ptrLongFileEntry->LFN_Attribute = 0x0f;                      // mark as hidden system file so that SFN systems will ignore the entry
            ptrLongFileEntry->LFN_Checksum = ucSFN_alias_checksum;       // the SFN alias checksum is inserted in each entry
            i = 31;
            continue;
        case 1:
            ptrLongFileEntry->LFN_Name_0 = ucNextCharacter;
            ptr_utDisk->usDiskFlags |= WRITEBACK_BUFFER_FLAG;            // mark that the original sector content must be committed
            break;
        case 2:
            ptrLongFileEntry->LFN_Name_0_extension = ucNextCharacterExtension;
            continue;
        case 3:
            ptrLongFileEntry->LFN_Name_1 = ucNextCharacter;
            break;
        case 4:
            ptrLongFileEntry->LFN_Name_1_extension = ucNextCharacterExtension;
            continue;
        case 5:
            ptrLongFileEntry->LFN_Name_2 = ucNextCharacter;
            break;
        case 6:
            ptrLongFileEntry->LFN_Name_2_extension = ucNextCharacterExtension;
            continue;
        case 7:
            ptrLongFileEntry->LFN_Name_3 = ucNextCharacter;
            break;
        case 8:
            ptrLongFileEntry->LFN_Name_3_extension = ucNextCharacterExtension;
            continue;
        case 9:
            ptrLongFileEntry->LFN_Name_4 = ucNextCharacter;
            break;
        case 10:
            ptrLongFileEntry->LFN_Name_4_extension = ucNextCharacterExtension;
            continue;
      //case 11:                                                         // attribute (added during initialisation)
      //case 12:                                                         // zero (zeroed during initialisation)
      //case 13:                                                         // checksum (added during initialisation)
     //     continue;
        case 14:
            ptrLongFileEntry->LFN_Name_5 = ucNextCharacter;
            i -= 3;                                                      // skip 11, 12, and 13
            break;
        case 15:
            ptrLongFileEntry->LFN_Name_5_extension = ucNextCharacterExtension;
            continue;
        case 16:
            ptrLongFileEntry->LFN_Name_6 = ucNextCharacter;
            break;
        case 17:
            ptrLongFileEntry->LFN_Name_6_extension = ucNextCharacterExtension;
            continue;
        case 18:
            ptrLongFileEntry->LFN_Name_7 = ucNextCharacter;
            break;
        case 19:
            ptrLongFileEntry->LFN_Name_7_extension = ucNextCharacterExtension;
            continue;
        case 20:
            ptrLongFileEntry->LFN_Name_8 = ucNextCharacter;
            break;
        case 21:
            ptrLongFileEntry->LFN_Name_8_extension = ucNextCharacterExtension;
            continue;
        case 22:
            ptrLongFileEntry->LFN_Name_9 = ucNextCharacter;
            break;
        case 23:
            ptrLongFileEntry->LFN_Name_9_extension = ucNextCharacterExtension;
            continue;
        case 24:
            ptrLongFileEntry->LFN_Name_10 = ucNextCharacter;
            break;
        case 25:
            ptrLongFileEntry->LFN_Name_10_extension = ucNextCharacterExtension;
            continue;
      //case 26:                                                         // zeros (zeroed during initialisation)
      //case 27:
      //    continue;
        case 28:
            ptrLongFileEntry->LFN_Name_11 = ucNextCharacter;
            i -= 2;                                                      // skip 26 and 27
            break;
        case 29:
            ptrLongFileEntry->LFN_Name_11_extension = ucNextCharacterExtension;
            continue;
        case 30:
            ptrLongFileEntry->LFN_Name_12 = ucNextCharacter;
            break;
        case 31:
            ptrLongFileEntry->LFN_Name_12_extension = ucNextCharacterExtension;
            continue;
        }
        if (iNameLength >= 13) {                                         // name character was added
            ptrReverseLFN--;
        }
        else {
            iNameLength++;                                               // pad was added
        }
    }
    return UTFAT_SUCCESS;
}
#endif

#if defined UTFAT_WRITE
// The new file or its new name will be located either at the end of the present directory or else reuse deleted areas of adequate size
//
static int fnSetFileLocation(UTFILE *ptr_utFile, OPEN_FILE_BLOCK *ptr_openBlock, /*const CHAR *ptrFilePath,*/ int iRename) // {4}
{
    int iReturn = UTFAT_SUCCESS;
    #if defined UTFAT_LFN_READ && (defined UTFAT_LFN_DELETE || defined UTFAT_LFN_WRITE)
    if (iRename != 0) {                                                  // if renaming
        iReturn = fnDeleteLFN_entry(ptr_utFile);                         // delete any original LFN directory entry (short file names can be simply overwritten)
        if (iReturn != UTFAT_SUCCESS) {
            return iReturn;
        }
    }
        #if defined UTFAT_LFN_WRITE
    if (ptr_openBlock->iQualifiedPathType >= FULLY_QUALIFIED_LONG_NAME_SFNM) { // the new file requires a long file name and cannot use a short one
        return fnInsertLFN_name(ptr_openBlock, ptr_utFile, /*ptrFilePath,*/ iRename); // {4}
    }
        #endif
    #endif
    // Creating a short file name - this is set at the end of the present directory and doesn't reuse deleted space
    //
    return iReturn;
}

// Create or rename a file
//
static int fnCreateFile(OPEN_FILE_BLOCK *ptr_openBlock, UTFILE *ptr_utFile, /*const CHAR *ptrFilePath, */unsigned long ulAccessMode) // {4}
{
    int iReturn;
    int iRename = ((ulAccessMode & _RENAME_EXISTING) != 0);              // check whether renaming existing file
    unsigned long ulFreeCluster = 0;
    DISK_LOCATION *ptrDiskLocation;                                      // this is the start of the directory in which the new file is to be placed - this allows reuse of deleted file locations in the directory
    DIR_ENTRY_STRUCTURE_FAT32 *ptrFoundEntry;
    UTDISK *ptr_utDisk = ptr_openBlock->ptr_utDisk;
    unsigned char ucDrive = ptr_utDisk->ucDriveNumber;
    iReturn = fnSetFileLocation(ptr_utFile, ptr_openBlock, /*ptrFilePath,*/ iRename); // {4} get the location in the present directory where the (SFN or SFN alias of) new/renamed file will be located
    if (iReturn != UTFAT_SUCCESS) {
        return iReturn;                                                  // presumed error
    }
    ptrDiskLocation = ptr_openBlock->ptrDiskLocation;                    // this is location in the directory where the new/renamed file (SFN or SFN alias) is to be placed
    if (fnLoadSector(ptr_utDisk, ptrDiskLocation->directory_location.ulSector) != UTFAT_SUCCESS) {
        return UTFAT_DISK_READ_ERROR;
    }
    ptrFoundEntry = (DIR_ENTRY_STRUCTURE_FAT32 *)ptr_utDisk->ptrSectorData; // the directory entry in the sector buffer
    ptrFoundEntry += ptrDiskLocation->ucDirectoryEntry;                  // move to the present directory entry

    uMemcpy(ptrFoundEntry->DIR_Name, ptr_openBlock->cShortFileName, 11); // add the short file name
    ptrFoundEntry->DIR_NTRes = ptr_openBlock->cShortFileName[11];
    if (iRename == 0) {                                                  // if creating and not renaming
        if (ptr_utFile == 0) {                                           // directory and not file
            ulFreeCluster = fnAllocateCluster(ptr_utDisk, ptrDiskLocation->directory_location.ulCluster, INITIALISE_DIR_CLUSTER);
            fnAddEntry(ptrFoundEntry, ulFreeCluster, DIR_ATTR_DIRECTORY);
        }
        else {
            ulFreeCluster = fnAllocateCluster(ptr_utDisk, 0, 0);         // allocate a cluster for the new file
            fnAddEntry(ptrFoundEntry, ulFreeCluster, DIR_ATTR_ARCHIVE);
        }
    }
    while (_utCommitSectorData[ucDrive](ptr_utDisk, ptr_utDisk->ptrSectorData, ptrDiskLocation->directory_location.ulSector) == CARD_BUSY_WAIT) {} // force writeback to finalise the operation
    ptr_utDisk->usDiskFlags &= ~WRITEBACK_BUFFER_FLAG;                   // the disk is up to date with the buffer
    if (iRename == 0) {                                                  // if creating and not renaming
        if (ptr_utFile != 0) {                                           // file and not directory
            uMemcpy(&ptr_utFile->private_disk_location, &ptr_utFile->ptr_utDirObject->public_disk_location, sizeof(ptr_utFile->private_disk_location)); // copy the referenced directory details
            ptr_utFile->private_file_location.ulCluster = ulFreeCluster;
            ptr_utFile->private_file_location.ulSector = (ulFreeCluster * ptr_utDisk->utFAT.ucSectorsPerCluster); // section referenced to logical base address
            ptr_utFile->private_file_location.ulSector += ptr_utDisk->ulVirtualBaseAddress; // the sector in which the file content begins
            uMemcpy(&ptr_utFile->public_file_location, &ptr_utFile->private_file_location, sizeof(ptr_utFile->private_file_location)); // copy the referenced file start details
            ptr_utFile->ulFileMode = ulAccessMode;
            ptr_utFile->ulFileSize = 0;
            ptr_utFile->ulFilePosition = 0;                              // set position to start of file on open
            ptr_utFile->ucDrive = ucDrive;
            iReturn = UTFAT_PATH_IS_FILE;
        }
    }
    fnCommitInfoChanges(ptr_utDisk);
    return iReturn;
}
#endif

#if defined UTFAT_EXPERT_FUNCTIONS
static int fnDisplayLFN(DISK_LOCATION *ptrFileLocation, UTDISK *ptr_utDisk)
{
    DISK_LOCATION dirLocation;
    LFN_ENTRY_STRUCTURE_FAT32 *ptrLFN_entry = 0;
    unsigned char *ptrData;
    int j;
    int iCheckedStart = 0;
    unsigned char ucEntry;
    unsigned char ucEntryCheck = 0;
    unsigned char ucSFN_alias_CS = 0;

    uMemcpy(&dirLocation, ptrFileLocation, sizeof(dirLocation));         // backup the location
    
    do {                                                                 // work through the long file name entries
        if (fnLoadSector(ptr_utDisk, dirLocation.directory_location.ulSector) != UTFAT_SUCCESS) { // load new sector (this will only read from physical disk when the sector is not already in local cache)
            return UTFAT_DISK_READ_ERROR;
        }
        ptrLFN_entry = (LFN_ENTRY_STRUCTURE_FAT32 *)ptr_utDisk->ptrSectorData; // the directory entry in the sector buffer
        ptrLFN_entry += dirLocation.ucDirectoryEntry;                    // move to the present entry
        ucEntry = ptrLFN_entry->LFN_EntryNumber;
        if (iCheckedStart == 0) {                                        // if the integrity of the first entry has not yet been confirmed
            if (ucEntry == DIR_NAME_FREE) {                              // analysing a deleted LFN
                fnDebugMsg("Deleted LFN\r\n");
                if ((ptrLFN_entry->LFN_Attribute & DIR_ATTR_MASK) != DIR_ATTR_LONG_NAME) {
                    fnDebugMsg("End\r\n");
                    break;
                }
                if (ucEntryCheck != 0) {
                    goto _show_data;
                }
            }
            else if (ucEntry & 0x40) {                                   // the first LFN entry must always start with this bit set
                ucEntry &= ~0x40;
                fnDebugMsg("First object from ");
                fnDebugDec(ucEntry, WITH_CR_LF);
                iCheckedStart = 1;
            }
            else {
                fnDebugMsg("First object error\r\n");                    // unexpected start - check the data content that follows!
                ucEntryCheck = 1;                                        // quit after displaying content
                goto _show_data;
            }
            ucEntryCheck = ucEntry;                                      // the number of LFN entries (will be 0xe5 for deleted entry)
            ucSFN_alias_CS = ptrLFN_entry->LFN_Checksum;                 // save the checksum for checking
        }
        else {
            if (--ucEntryCheck != ucEntry) {
                fnDebugMsg("LFN object error\r\n");                      // unexpected LFN entry counter - check the data content that follows!
                ucEntryCheck = 0;
            }
        }
_show_data:
        ptrData = (unsigned char *)ptrLFN_entry;
        fnDebugMsg("Data =");
        for (j = 0; j < 32; j++) {                                       // display the entries raw data
            fnDebugHex(*ptrData++, (WITH_LEADIN | WITH_SPACE | sizeof(unsigned char)));
        }
        fnDebugMsg("\r\n");
        if ((ptrLFN_entry->LFN_Attribute & DIR_ATTR_MASK) != DIR_ATTR_LONG_NAME) { // verify that the entry has LFN attribute
            fnDebugMsg("Invalid LFN attribute!!");
        }
        else {
            if ((ptrLFN_entry->LFN_Zero0 != 0) || (ptrLFN_entry->LFN_Zero1 != 0) || (ptrLFN_entry->LFN_Zero2 != 0)) { // perform some sanity checking of content
                fnDebugMsg("LFN content zero error - ");
                if (ptrLFN_entry->LFN_Zero0 != 0) {
                    fnDebugMsg("0");
                }   
                if (ptrLFN_entry->LFN_Zero1 != 0) {
                    fnDebugMsg("1");
                }
                if (ptrLFN_entry->LFN_Zero2 != 0) {
                    fnDebugMsg("2");
                }
                fnDebugMsg(" not zero!!\r\n");
            }
            if (ucSFN_alias_CS != ptrLFN_entry->LFN_Checksum) {          // {2}
                fnDebugMsg("SFN alias CS mis-match!!\r\n");
            }
        }
        if (ucEntryCheck == 1) {                                         // last LFN entry displayed
            break;
        }        
        if (fnNextDirectoryEntry(ptr_utDisk, &dirLocation) == UTFAT_DIRECTORY_AREA_EXHAUSTED) { // move to the next entry
            return UTFAT_DIRECTORY_AREA_EXHAUSTED;
        }
    } while (1);
    return UTFAT_SUCCESS;
}

static int fnDisplaySFN(int iFile, UTFILE *ptr_utFile, OPEN_FILE_BLOCK *ptr_openBlock)
{
    unsigned long ulCluster;
    unsigned long ulSector;
    unsigned long ulFatSector;
    DISK_LOCATION *ptrFileLocation = &ptr_utFile->private_disk_location;
    UTDISK *ptr_utDisk = &utDisks[ptr_utFile->ptr_utDirObject->ucDrive];
    DIR_ENTRY_STRUCTURE_FAT32 *ptrEntry = 0;
    unsigned char *ptrData;
    int j;
    CHAR cBuf[12];

    if (fnLoadSector(ptr_utDisk, ptrFileLocation->directory_location.ulSector) != UTFAT_SUCCESS) { // load new sector (this will only read from physical disk when the sector is not already in local cache)
        return UTFAT_DISK_READ_ERROR;
    }
    ptrEntry = (DIR_ENTRY_STRUCTURE_FAT32 *)ptr_utDisk->ptrSectorData; // the directory entry in the sector buffer
    ptrEntry += ptrFileLocation->ucDirectoryEntry;                   // move to the present entry

    ptrData = (unsigned char *)ptrEntry;
    fnDebugMsg("Data =");
    for (j = 0; j < 32; j++) {                                       // display an entry
        fnDebugHex(*ptrData++, (WITH_LEADIN | WITH_SPACE | sizeof(unsigned char)));
    }
    fnDebugMsg("\r\nSFN name (");
    switch (ptrEntry->DIR_Attr) {
    case DIR_ATTR_READ_ONLY:
        fnDebugMsg("read-only");
        break;
    case DIR_ATTR_HIDDEN:
        fnDebugMsg("hidden");
        break;
    case DIR_ATTR_SYSTEM:
        fnDebugMsg("system");
        break;
    case DIR_ATTR_VOLUME_ID:
        fnDebugMsg("volume ID");
        break;
    case DIR_ATTR_DIRECTORY:
        fnDebugMsg("directory");
        break;
    case DIR_ATTR_ARCHIVE:
        fnDebugMsg("archive");
        break;
    default:
        fnDebugMsg("???");
        break;
    }
    fnDebugMsg(") = ");
    ptrData = (unsigned char *)ptrEntry;
    for (j = 0; j < 11; j++) {                                       // display an entry
        cBuf[j] = *ptrData++;
        if ((cBuf[j] < ' ') || (cBuf[j] >= 0x7f)) {                  // characters that can't be displayed
            cBuf[j] = '.';
        }
    }
    cBuf[11] = 0;
    fnDebugMsg(cBuf);
    #if defined UTFAT_LFN_READ
    if (ptr_utFile->lfn_file_location.directory_location.ulSector != 0) { // if a long file name
        fnDebugMsg(" Alias CS = ");
        fnDebugHex(ptr_openBlock->ucSFN_alias_checksum, (sizeof(ptr_openBlock->ucSFN_alias_checksum) | WITH_LEADIN));
    }
    #endif
    if (iFile != 0) {
        fnDebugMsg("\r\nFile length = ");
        fnDebugDec(ptr_utFile->ulFileSize, 0);
        ulCluster = ptr_utFile->private_file_location.ulCluster;
        ulSector = ptr_utFile->private_file_location.ulSector;
    }
    else {
        ulCluster = ptr_openBlock->ulCluster;
        ulSector = ptr_utDisk->ulVirtualBaseAddress + (ptr_openBlock->ulCluster * ptr_utDisk->utFAT.ucSectorsPerCluster);
    }
    ulFatSector = (ptr_utDisk->utFAT.ulFAT_start + (ulCluster >> 7));
    fnDebugMsg(" starting in sector ");
    fnDebugHex(ulSector, (WITH_LEADIN | sizeof(ulSector)));
    fnDebugMsg(" (cluster ");
    fnDebugHex(ulCluster, (WITH_LEADIN | sizeof(ulCluster)));
    fnDebugMsg(") FAT sector ");
    fnDebugHex(ulFatSector, (WITH_LEADIN | sizeof(ulFatSector)));
    fnDebugMsg(" offset ");
    fnDebugHex((ulCluster & 0x7f), (WITH_LEADIN | WITH_CR_LF | sizeof(unsigned char)));
    return UTFAT_SUCCESS;
}
#endif

#if defined UTFAT_EXPERT_FUNCTIONS
static void fnDisplayFileInfo(int iFile, const CHAR *ptrFilePath, UTFILE *ptr_utFile, OPEN_FILE_BLOCK *ptr_openBlock)
{
    if (iFile != 0) {
        fnDebugMsg("File: ");
    }
    else {
        fnDebugMsg("Directory: ");
    }
    fnDebugMsg((CHAR *)ptrFilePath);
    #if defined UTFAT_LFN_READ
    if (ptr_utFile->lfn_file_location.directory_location.ulSector == 0) { // if a short file name
        fnDebugMsg(" is SFN\r\n");
    }
    else {
        fnDebugMsg(" is LFN ");
        fnDebugMsg("\r\nStarting at entry ");
        fnDebugHex(ptr_utFile->lfn_file_location.ucDirectoryEntry, (WITH_LEADIN | sizeof(ptr_utFile->lfn_file_location.ucDirectoryEntry)));
        fnDebugMsg(" in sector ");
        fnDebugHex(ptr_utFile->lfn_file_location.directory_location.ulSector, (WITH_LEADIN | sizeof(ptr_utFile->lfn_file_location.directory_location.ulSector)));
        fnDebugMsg(" (cluster ");
        fnDebugHex(ptr_utFile->lfn_file_location.directory_location.ulCluster, (WITH_LEADIN | sizeof(ptr_utFile->lfn_file_location.directory_location.ulCluster)));
        fnDebugMsg(")\r\n");
        fnDisplayLFN(&ptr_utFile->lfn_file_location, &utDisks[ptr_utFile->ptr_utDirObject->ucDrive]); // display long file name information
    }
    #else
    fnDebugMsg(" is SFN\r\n");
    #endif
    fnDebugMsg("SFN File located at entry ");
    fnDebugHex(ptr_utFile->private_disk_location.ucDirectoryEntry, (WITH_LEADIN | sizeof(ptr_utFile->private_disk_location.ucDirectoryEntry)));
    fnDebugMsg(" in sector ");
    fnDebugHex(ptr_utFile->private_disk_location.directory_location.ulSector, (WITH_LEADIN | sizeof(ptr_utFile->private_disk_location.directory_location.ulSector)));
    fnDebugMsg(" (cluster ");
    fnDebugHex(ptr_utFile->private_disk_location.directory_location.ulCluster, (WITH_LEADIN | sizeof(ptr_utFile->private_disk_location.directory_location.ulCluster)));
    fnDebugMsg(")\r\n");
    fnDisplaySFN(iFile, ptr_utFile, ptr_openBlock);                      // display short file name information
}
#endif

#if defined SDCARD_SUPPORT || defined SPI_FLASH_FAT || defined FLASH_FAT || defined USB_MSD_HOST
// Internal function to open a file or directory object
//
static int _utOpenFile(const CHAR *ptrFilePath, UTFILE *ptr_utFile, unsigned long ulAccessMode)
{
    int iReturn = UTFAT_SUCCESS;
    OPEN_FILE_BLOCK openBlock;
    uMemset(&openBlock, 0, sizeof(openBlock));                           // initialise open file block

    if (ulAccessMode & UTFAT_OPEN_FOR_RENAME) {
        ptr_utFile->ptr_utDirObject->usDirectoryFlags |= (UTDIR_DIR_AS_FILE | UTDIR_REFERENCED | UTDIR_SET_START); // opens for renames are set to allow directories to be handled as files 
    }
    else {
        ptr_utFile->ptr_utDirObject->usDirectoryFlags |= (UTDIR_REFERENCED | UTDIR_SET_START); // the next open is a referenced open; set to start of lowest directory when file not found
    }

    iReturn = _fnHandlePath(&openBlock, ptrFilePath, ptr_utFile->ptr_utDirObject); // handle the input path string
    if (openBlock.iContinue == 0) {                                      // if the path handling had an error or completed all work
        return iReturn;                                                  // return with code
    }
#if defined UTFAT_EXPERT_FUNCTIONS
    if (ulAccessMode & UTFAT_DISPLAY_INFO) {
        openBlock.usDirFlags |= UTDIR_DIR_AS_FILE;                       // handle directories as files
    }
#endif

    iReturn = _utOpenDirectory(&openBlock, ptr_utFile->ptr_utDirObject, ulAccessMode /*((ulAccessMode & UTFAT_OPEN_DELETED) != UTFAT_SUCCESS)*/); // {3} pass full access mode
    if (UTFAT_PATH_IS_FILE == iReturn) {
        uMemcpy(&ptr_utFile->private_disk_location, &ptr_utFile->ptr_utDirObject->public_disk_location, sizeof(ptr_utFile->private_disk_location)); // copy the referenced directory details
        uMemcpy(&ptr_utFile->private_file_location, &ptr_utFile->ptr_utDirObject->public_file_location, sizeof(ptr_utFile->private_file_location)); // copy the referenced file start details
        uMemcpy(&ptr_utFile->public_file_location, &ptr_utFile->ptr_utDirObject->public_file_location, sizeof(ptr_utFile->public_file_location)); // copy the referenced file start details
        if (ptr_utFile->ptr_utDirObject->ptrEntryStructure->DIR_Attr & DIR_ATTR_READ_ONLY) { // {10}
            ulAccessMode &= ~(UTFAT_OPEN_FOR_DELETE | UTFAT_OPEN_FOR_WRITE); // the file is read-only so remove possible delete and write modes
        }
        ptr_utFile->ulFileMode = ulAccessMode;
        ptr_utFile->ulFileSize = ((ptr_utFile->ptr_utDirObject->ptrEntryStructure->DIR_FileSize[3] << 24) + (ptr_utFile->ptr_utDirObject->ptrEntryStructure->DIR_FileSize[2] << 16) + (ptr_utFile->ptr_utDirObject->ptrEntryStructure->DIR_FileSize[1] << 8) + ptr_utFile->ptr_utDirObject->ptrEntryStructure->DIR_FileSize[0]);
        ptr_utFile->ulFilePosition = 0;                                  // set position to start of file on open
        ptr_utFile->ucDrive = ptr_utFile->ptr_utDirObject->ucDrive;
#if defined UTFAT_RETURN_FILE_CREATION_TIME                              // {10}
        ptr_utFile->usCreationDate = ((ptr_utFile->ptr_utDirObject->ptrEntryStructure->DIR_CrtDate[1] << 8) | ptr_utFile->ptr_utDirObject->ptrEntryStructure->DIR_CrtDate[0]);
        ptr_utFile->usCreationTime = ((ptr_utFile->ptr_utDirObject->ptrEntryStructure->DIR_CrtTime[1] << 8) | ptr_utFile->ptr_utDirObject->ptrEntryStructure->DIR_CrtTime[0]);
#endif
#if defined UTFAT_LFN_READ && ((defined UTFAT_LFN_DELETE || defined UTFAT_LFN_WRITE) || defined UTFAT_EXPERT_FUNCTIONS)
        uMemcpy(&ptr_utFile->lfn_file_location, &openBlock.lfn_file_location, sizeof(ptr_utFile->lfn_file_location)); // save the start of a LFN entry so that its length and location is known
        ptr_utFile->ucLFN_entries = openBlock.ucLFN_entries;
#endif
#if defined UTFAT_WRITE
        if (ulAccessMode & UTFAT_TRUNCATE) {                             // the file is to be overwritten so delete its content
            fnDeleteFileContent(ptr_utFile, &utDisks[ptr_utFile->ucDrive], REUSE_CLUSTERS); // delete the content and set the file length to zero
            ptr_utFile->ulFileSize = 0;
        }
        else if (UTFAT_APPEND & ulAccessMode) {
            if ((iReturn = utSeek(ptr_utFile, 0, UTFAT_SEEK_END)) != UTFAT_SUCCESS) { // seek to the end of the file so that writes cause append
                return iReturn;
            }
        }
#endif
#if defined UTFAT_EXPERT_FUNCTIONS
        if (ulAccessMode & UTFAT_DISPLAY_INFO) {
            fnDisplayFileInfo(1, ptrFilePath, ptr_utFile, &openBlock);
        }
#endif
        return (UTFAT_PATH_IS_FILE);
    }
#if defined UTFAT_WRITE
    else if ((ulAccessMode & UTFAT_CREATE) != 0) {                       // file doesn't exist so we should create it
        if (iReturn != UTFAT_FILE_NOT_FOUND) {
            return iReturn;                                              // invalid path has been entered so return error code
        }
        openBlock.ptr_utDisk = &utDisks[ptr_utFile->ptr_utDirObject->ucDrive];
        uMemcpy(&openBlock.DirectoryEndLocation, &ptr_utFile->ptr_utDirObject->public_disk_location, sizeof(openBlock.DirectoryEndLocation)); // {1}
        return (fnCreateFile(&openBlock, ptr_utFile, /*openBlock.ptrLocalDirPath,*/ (ulAccessMode & ~_RENAME_EXISTING))); // {4} create the file
    }
#endif
    else if (iReturn == UTFAT_SUCCESS) {                                 // a directory was matched 
        uMemcpy(&ptr_utFile->private_disk_location, &ptr_utFile->ptr_utDirObject->public_disk_location, sizeof(ptr_utFile->ptr_utDirObject->public_disk_location)); // copy the referenced directory details
        ptr_utFile->ulFileMode = (UTFAT_FILE_IS_DIR | ulAccessMode);     // mark that the entry is not a file but a directory
        ptr_utFile->ucDrive = ptr_utFile->ptr_utDirObject->ucDrive;
#if defined UTFAT_LFN_READ && ((defined UTFAT_LFN_DELETE || defined UTFAT_LFN_WRITE) || defined UTFAT_EXPERT_FUNCTIONS)
        uMemcpy(&ptr_utFile->lfn_file_location, &openBlock.lfn_file_location, sizeof(ptr_utFile->lfn_file_location)); // save the start of a LFN entry so that its length and location is known
        ptr_utFile->ucLFN_entries = openBlock.ucLFN_entries;
#endif
#if defined UTFAT_EXPERT_FUNCTIONS
        if (ulAccessMode & UTFAT_DISPLAY_INFO) {
            fnDisplayFileInfo(0, ptrFilePath, ptr_utFile, &openBlock);
        }
#endif
    }
    return iReturn;
}

// Open a file or directory object
//
extern int utOpenFile(const CHAR *ptrFilePath, UTFILE *ptr_utFile, UTDIRECTORY *ptr_utDirectory, unsigned long ulAccessMode)
{
    if ((ptr_utFile->ptr_utDirObject = ptr_utDirectory) == 0) {
        return UTFAT_SEARCH_INVALID;
    }    
    if ((utDisks[ptr_utFile->ptr_utDirObject->ucDrive].usDiskFlags & WRITE_PROTECTED_SD_CARD) && (ulAccessMode & (UTFAT_OPEN_FOR_RENAME | UTFAT_OPEN_FOR_WRITE | UTFAT_OPEN_FOR_DELETE))) { // {34}
        return UTFAT_DISK_WRITE_PROTECTED;
    }
#if defined UTMANAGED_FILE_COUNT && UTMANAGED_FILE_COUNT > 0             // allow SD card interface to be used without UTMANAGED_FILE_COUNT
    if ((ulAccessMode & UTFAT_MANAGED_MODE) != 0) {
        int iFileHandle = 0;
        UTMANAGED_FILE *ptrManagedFile = utManagedFiles;
        while (iFileHandle < UTMANAGED_FILE_COUNT) {
            if (ptrManagedFile->managed_owner == 0) {                
                if ((utOpenFile(ptrFilePath, ptr_utFile, ptr_utDirectory, (ulAccessMode & ~UTFAT_MANAGED_MODE)) != UTFAT_PATH_IS_FILE)) { // recursive call with adjusted access mode
                    return UTFAT_FILE_NOT_FOUND;                         // file doesn't exist
                }
                if (fnFileLocked(ptr_utFile) != 0) {
                    return UTFAT_FILE_LOCKED;                            // the file is locked by another user so may not be opened
                }
                ptrManagedFile->utManagedFile = ptr_utFile;
                ptrManagedFile->managed_owner = ptr_utFile->ownerTask;   // validate the entry with the owner task
                ptr_utFile->ulFileMode = ulAccessMode;
                ptrManagedFile->managed_mode = (unsigned char)ulAccessMode;
                ptr_utFile->iFileHandle = iFileHandle;
                return UTFAT_PATH_IS_FILE;                               // open in managed mode was successful
            }
            iFileHandle++;
            ptrManagedFile++;
        }
        return MANAGED_FILE_NO_FILE_HANDLE;                              // all managed file spaces are presently occupied
    }
#endif
#if defined UTFAT_FILE_CACHE_POOL && UTFAT_FILE_CACHE_POOL > 0
    ptr_utFile->ptrFileDataCache = 0;                                    // initially no cache is allocated
#endif
    return _utOpenFile(ptrFilePath, ptr_utFile, ulAccessMode);
}

#if defined UTFAT_WRITE                                                  // allow operation without write support
    #if defined UTFAT_LFN_READ && (defined UTFAT_LFN_DELETE || defined UTFAT_LFN_WRITE)
// If the file name being deleted this routine deletes the complete long file name part before the short file name alias (it doesn't delete file content)
//
static int fnDeleteLFN_entry(UTFILE *ptr_utFile)
{
    DISK_LOCATION FileLocation;
    UTDISK *ptr_utDisk = &utDisks[ptr_utFile->ucDrive];
  //unsigned long ulSectorToWrite = ptr_utDisk->ulPresentSector;
    LFN_ENTRY_STRUCTURE_FAT32 *ptrFileEntry;
    unsigned char ucEntryNumber;

    if (ptr_utFile->lfn_file_location.directory_location.ulSector == 0) {// the file is using a short file name and so no long file name part needs to be deleted
        return UTFAT_SUCCESS;
    }
    uMemcpy(&FileLocation, &(ptr_utFile->lfn_file_location), sizeof(DISK_LOCATION)); // make a copy of the long file name entry details
    while (1) {
        ptr_utDisk->usDiskFlags |= WRITEBACK_BUFFER_FLAG;                // mark that the modified sector content must be committed each time the sector is changed
        if (fnLoadSector(ptr_utDisk, FileLocation.directory_location.ulSector) != UTFAT_SUCCESS) { // load sector where the LFN begins
            return UTFAT_DISK_READ_ERROR;
        }
        ptrFileEntry = (LFN_ENTRY_STRUCTURE_FAT32 *)ptr_utDisk->ptrSectorData;
        ptrFileEntry += FileLocation.ucDirectoryEntry;
        ucEntryNumber = ptrFileEntry->LFN_EntryNumber;
        ptrFileEntry->LFN_EntryNumber = DIR_NAME_FREE;                   // delete the entry
        if ((ucEntryNumber & ~0x40) <= 1) {
            break;                                                       // final LFN entry has been deleted (following SFN alias still exists)
        }
        fnNextDirectoryEntry(ptr_utDisk, &FileLocation);                 // move to next entry
    }
    // Since the renames file object will be added there is no need to commit the changes yet
    //
    return UTFAT_SUCCESS;
}
    #endif

// Rename a referenced file or directory (can result in SFN->LFN or LFN->SFN conversions or LFN with different entry lengths)
//
extern int utRenameFile(const CHAR *ptrFilePath, UTFILE *ptr_utFile)
{
    int iReturn;
    UTDISK *ptr_utDisk = &utDisks[ptr_utFile->ucDrive];
    DISK_LOCATION file_location;
    OPEN_FILE_BLOCK openBlock;

    if (ptr_utDisk->usDiskFlags & WRITE_PROTECTED_SD_CARD) {
        return UTFAT_DISK_WRITE_PROTECTED;                               // can't rename anything on a write protected disk
    }
    if ((ptr_utFile->ulFileMode & (UTFAT_OPEN_FOR_RENAME | UTFAT_OPEN_FOR_DELETE)) == 0) { // if the file is neither open for rename or write don't allow a rename
        return UTFAT_FILE_NOT_WRITEABLE;                                 // file access doesn't allow renaming
    }
    uMemset(&openBlock, 0, sizeof(openBlock));                           // initialise open file block

    uMemcpy(&file_location, &ptr_utFile->ptr_utDirObject->public_disk_location, sizeof(file_location)); // backup the present file location details

    iReturn = _fnHandlePath(&openBlock, ptrFilePath, ptr_utFile->ptr_utDirObject); // handle the input path string
    if (openBlock.iContinue == 0) {                                      // if the path handling had an error or completed all work
        return iReturn;                                                  // return with code
    }
    if ((iReturn = _utOpenDirectory(&openBlock, ptr_utFile->ptr_utDirObject, 0)) != UTFAT_FILE_NOT_FOUND) { // check that the new file or directory name doesn't already exist
        return iReturn;                                                  // if the file or directory is already present return code
    }
    #if !defined UTFAT_LFN_WRITE                                         // don't allow renaming to LFN when LFN writing is not supported
    if (FULLY_QUALIFIED_LONG_NAME == openBlock.iQualifiedPathType) {
        return LFN_RENAME_NOT_POSSIBLE;
    }
    #endif
    openBlock.ptr_utDisk = ptr_utDisk;
    uMemcpy(&openBlock.DirectoryEndLocation, &ptr_utFile->ptr_utDirObject->public_disk_location, sizeof(openBlock.DirectoryEndLocation));
    uMemcpy(&ptr_utFile->ptr_utDirObject->public_disk_location, &file_location, sizeof(file_location)); // return the file location details
    return (fnCreateFile(&openBlock, ptr_utFile, /*openBlock.ptrLocalDirPath,*/ _RENAME_EXISTING)); // {4} delete the original file/directory name and create the new one without changing file properties or content
}
#endif

// A close only has real significance when the file was opened as a managed file, or when data caching or delayed directory object write is used
//
extern int utCloseFile(UTFILE *ptr_utFile)
{
    int iReturn = UTFAT_SUCCESS;
#if defined UTMANAGED_FILE_COUNT && UTMANAGED_FILE_COUNT > 0             // allow SD card interface to be used without UTMANAGED_FILE_COUNT
    if (ptr_utFile->ulFileMode & UTFAT_MANAGED_MODE) {
        utManagedFiles[ptr_utFile->iFileHandle].managed_owner = 0;       // free the managed file entry
    }
#endif
#if defined UTFAT_FILE_CACHE_POOL && UTFAT_FILE_CACHE_POOL > 0
    if (ptr_utFile->ptrFileDataCache != 0) {                             // if we are working with a data cache check whether data has to be saved to disk
        FILE_DATA_CACHE *ptrDataCache = ptr_utFile->ptrFileDataCache;
    #if defined UTFAT_WRITE
        if (ptrDataCache->ucFileCacheState & FILE_BUFFER_MODIFIED) {     // if modified data waiting to be saved
            int iDrive = ptr_utFile->ucDrive;
            while (_utCommitSectorData[iDrive](&utDisks[iDrive], ptrDataCache->ucFileDataCache, ptrDataCache->ulFileCacheSector) == CARD_BUSY_WAIT) {} // commit the data to the disk
        }
    #endif
        ptrDataCache->ucFileCacheState &= ~(FILE_BUFFER_MODIFIED);       // free the cache for use by other files
    }
#endif
#if defined UTFAT_WRITE
    if (ptr_utFile->ulFileMode & _FILE_CHANGED) {                        // the file's content was changed while the file was open so we write the information on close
        iReturn = fnCommitFileInfo(ptr_utFile, &utDisks[ptr_utFile->ucDrive]);
    }
#endif
    uMemset(ptr_utFile, 0, sizeof(UTFILE));                              // delete the file object
    return iReturn;
}


#if defined UTMANAGED_FILE_COUNT && UTMANAGED_FILE_COUNT > 0             // allow SD card interface to be used without UTMANAGED_FILE_COUNT
    #if defined UTFAT_WRITE
// The file has changed so update all user information
//
static void fnSynchroniseManagedFile(UTFILE *ptr_utFile)
{
    int iFileHandle = 0;
    while (iFileHandle < UTMANAGED_FILE_COUNT) {                         // check through the managed file list
        if (utManagedFiles[iFileHandle].managed_owner != 0) {            // if the entry is valid
            if (uMemcmp(&ptr_utFile->private_file_location, &utManagedFiles[iFileHandle].utManagedFile->private_file_location, sizeof(FILE_LOCATION)) == 0) { // if a managed file matches this one
                utManagedFiles[iFileHandle].utManagedFile->ulFileSize = ptr_utFile->ulFileSize; // update the file size
                if (utManagedFiles[iFileHandle].utManagedFile->ulFileSize < utManagedFiles[iFileHandle].utManagedFile->ulFilePosition) { // if the file size has shrunk to before the present file location
                    utManagedFiles[iFileHandle].utManagedFile->ulFilePosition = utManagedFiles[iFileHandle].utManagedFile->ulFileSize; // set the new file location to the end of the file
                }
            }
        }
        iFileHandle++;
    }
}
        #if defined UTFAT_FILE_CACHE_POOL && UTFAT_FILE_CACHE_POOL > 0
static void fnSynchroniseManagedFileCache(FILE_DATA_CACHE *ptrDataCache)
{
    int iFileHandle = 0;
    while (iFileHandle < UTMANAGED_FILE_COUNT) {                         // check through the managed file list
        if (utManagedFiles[iFileHandle].managed_owner != 0) {            // if the entry is valid
            if (utManagedFiles[iFileHandle].utManagedFile->ptrFileDataCache) { // and a managed file has its own cache
                if (ptrDataCache->ulFileCacheSector == utManagedFiles[iFileHandle].utManagedFile->ptrFileDataCache->ulFileCacheSector) { // and is caching the same sector
                    if (utManagedFiles[iFileHandle].utManagedFile->ptrFileDataCache->ucFileDataCache != ptrDataCache->ucFileDataCache) {
                        uMemcpy(utManagedFiles[iFileHandle].utManagedFile->ptrFileDataCache->ucFileDataCache, ptrDataCache->ucFileDataCache, sizeof(utManagedFiles[iFileHandle].utManagedFile->ptrFileDataCache->ucFileDataCache)); // synchronise the other cache content
                    }
                }
            }
        }
        iFileHandle++;
    }
}
        #endif
    #endif
    #if defined UTFAT_FILE_CACHE_POOL && UTFAT_FILE_CACHE_POOL > 0
static int fnGetManagedFileCache(unsigned long ulSector, unsigned char *ptrBuffer, unsigned short usAccessOffset, unsigned short usAccessLength)
{
    int iFileHandle = 0;
    while (iFileHandle < UTMANAGED_FILE_COUNT) {                         // check through the managed file list
        if (utManagedFiles[iFileHandle].managed_owner != 0) {            // if the entry is valid
            if (utManagedFiles[iFileHandle].utManagedFile->ptrFileDataCache) { // and a managed file has its own cache
                if (ulSector == utManagedFiles[iFileHandle].utManagedFile->ptrFileDataCache->ulFileCacheSector) { // and is caching the same sector being read
                    uMemcpy(ptrBuffer, &utManagedFiles[iFileHandle].utManagedFile->ptrFileDataCache->ucFileDataCache[usAccessOffset], usAccessLength); // read data form the cache
                    return 1;                                            // copy has been taken from cache so no need to read form the disk
                }
            }
        }
        iFileHandle++;
    }
    return 0;                                                            // no cache entries found
}
    #endif

// Check whether the file is locked by another user
//
static int fnFileLocked(UTFILE *ptr_utFile)
{
    int iFileHandle = 0;
    while (iFileHandle < UTMANAGED_FILE_COUNT) {
        if (utManagedFiles[iFileHandle].managed_owner != 0) {
            if (uMemcmp(&ptr_utFile->private_file_location, &utManagedFiles[iFileHandle].utManagedFile->private_file_location, sizeof(FILE_LOCATION)) == 0) {
                if (utManagedFiles[iFileHandle].managed_mode & UTFAT_PROTECTED) {
                    return 1;                                            // another user is locking this file
                }
            }
        }
        iFileHandle++;
    }
    return 0;                                                            // this file is not being locked by another user
}
#endif

#if defined UTFAT_WRITE
extern int utWriteFile(UTFILE *ptr_utFile, unsigned char *ptrBuffer, unsigned short usLength)
{
    unsigned short usWriteLength;
    unsigned short usRemainingBuffer;
    unsigned char  ucDriveNumber = ptr_utFile->ucDrive;
    unsigned char  ucDataContent[512];                                   // temporary buffer for reading disk data to and manipulating data in
    unsigned char *ptrFileDateBuffer = ucDataContent;                    // use the temporary buffer by default
    UTDISK *ptr_utDisk = &utDisks[ucDriveNumber];
    #if defined UTFAT_FILE_CACHE_POOL && UTFAT_FILE_CACHE_POOL > 0
    FILE_DATA_CACHE *ptrDataCache;
    #endif
    ptr_utFile->usLastReadWriteLength = 0;
    if (ptr_utDisk->usDiskFlags & WRITE_PROTECTED_SD_CARD) {
        return UTFAT_DISK_WRITE_PROTECTED;
    }
    if (!(ptr_utFile->ulFileMode & UTFAT_OPEN_FOR_WRITE)) {              // only allow writes if the file is open for writing
        return UTFAT_FILE_NOT_WRITEABLE;
    }
    #if defined UTFAT_FILE_CACHE_POOL && UTFAT_FILE_CACHE_POOL > 0
    if ((ptr_utFile->ulFileMode & UTFAT_WITH_DATA_CACHE) && (ptr_utFile->ptrFileDataCache == 0)) { // use a file data buffer if one is available in the pool
        ptr_utFile->ptrFileDataCache = fnGetDataCache();                 // try to get data cache to work with
    }
    ptrDataCache = ptr_utFile->ptrFileDataCache;
    #endif
    while (usLength != 0) {
        usRemainingBuffer = (unsigned short)(ptr_utFile->ulFilePosition % 512); // the start offset in the sector buffer to be written to
        if ((usRemainingBuffer != 0) || ((usLength < 512) && (usLength < (ptr_utFile->ulFileSize - ptr_utFile->ulFilePosition)))) { // if the complete content is not being overwritten
    #if defined UTFAT_FILE_CACHE_POOL && UTFAT_FILE_CACHE_POOL > 0
            if (ptrDataCache != 0) {                                     // if a data cache is being used by this file
                int iReturnValue = fnHandleFileDataCache(ptr_utDisk, ptr_utFile, ptrDataCache, 0); // handle cache - load data to cache if necessary and save any cached data that may require saving
                if (iReturnValue != UTFAT_SUCCESS) {
                    return iReturnValue;                                 // access error
                }
                ptrFileDateBuffer = ptrDataCache->ucFileDataCache;       // use the data cache
            }
            else {
    #endif
                if (_utReadDiskSector[ptr_utFile->ucDrive](ptr_utDisk, ptr_utFile->public_file_location.ulSector, ucDataContent) != UTFAT_SUCCESS) { // read data sector to the temporary buffer
                    return UTFAT_DISK_READ_ERROR;
                }
    #if defined UTFAT_FILE_CACHE_POOL && UTFAT_FILE_CACHE_POOL > 0
            }
    #endif
        }
        else {                                                           // the complete sector content will be overwritten so there is no need to load any data from disk
    #if defined UTFAT_FILE_CACHE_POOL && UTFAT_FILE_CACHE_POOL > 0
            if (ptrDataCache != 0) {                                     // if a data cache is being used by this file
                int iReturnValue = fnHandleFileDataCache(ptr_utDisk, ptr_utFile, ptrDataCache, 1); // handle cache - save any cached data that may require saving
                if (iReturnValue != UTFAT_SUCCESS) {
                    return iReturnValue;                                 // access error
                }
                ptrFileDateBuffer = ptrDataCache->ucFileDataCache;       // use the data cache
            }
    #endif
            uMemset(ptrFileDateBuffer, 0, sizeof(ucDataContent));        // zero the temporary data buffer so that any unmodified content at the end will be 0x00
        }
        usWriteLength = usLength;
        if (usWriteLength > (512 - usRemainingBuffer)) {                 // check whether the data will fit into the content buffer
            usWriteLength = (512 - usRemainingBuffer);                   // if it won't fit copy as much as possible
        }
        uMemcpy(&ptrFileDateBuffer[usRemainingBuffer], ptrBuffer, usWriteLength); // copy the data that is to be saved to the correct location in the data content buffer
    #if defined UTFAT_FILE_CACHE_POOL && UTFAT_FILE_CACHE_POOL > 0
        if (ptrDataCache != 0) {                                         // if working with data cache
            ptrDataCache->ucFileCacheState |= (FILE_BUFFER_MODIFIED | FILE_BUFFER_VALID); // content has been modified and so needs to be saved
            ptrDataCache->ulFileCacheSector = ptr_utFile->public_file_location.ulSector; // the sector that the cache belongs to
        #if defined UTMANAGED_FILE_COUNT && UTMANAGED_FILE_COUNT > 0
            fnSynchroniseManagedFileCache(ptrDataCache);                 // synchronise any managed files caching the data content
        #endif
        }
        else {
            while (_utCommitSectorData[ucDriveNumber](ptr_utDisk, ptrFileDateBuffer, ptr_utFile->public_file_location.ulSector) == CARD_BUSY_WAIT) {} // commit the data to the disk
        }
    #else
        while (_utCommitSectorData[ucDriveNumber](ptr_utDisk, ptrFileDateBuffer, ptr_utFile->public_file_location.ulSector) == CARD_BUSY_WAIT) {} // commit the data to the disk
    #endif
        ptrBuffer += usWriteLength;                                      // increment the data pointer to beyond the data that has been saved
        ptr_utFile->ulFilePosition += usWriteLength;                     // also move the present file pointer accordingly
        if (ptr_utFile->ulFilePosition > ptr_utFile->ulFileSize) {       // if the write has made the file grow in size
            ptr_utFile->ulFileSize = ptr_utFile->ulFilePosition;         // increase the file length accordingly
        }
        ptr_utFile->usLastReadWriteLength += usWriteLength;              // the number of bytes written
        usLength -= usWriteLength;                                       // the number of bytes that still need to be written
        if ((usRemainingBuffer + usWriteLength) >= 512) {                // if the write has just filled a sector
            int iResult = fnNextSectorCreate(ptr_utDisk, &(ptr_utFile->public_file_location), 0); // move to next sector and create a new cluster if needed
            if (iResult != UTFAT_SUCCESS) {                              // move to next sector/cluster and create additional cluster if necessary
                return iResult;
            }
        }
    }
    if (ptr_utFile->usLastReadWriteLength != 0) {                        // updata the file entry if there was a change
        if (ptr_utFile->ulFileMode & UTFAT_COMMIT_FILE_ON_CLOSE) {       // if the user wishes to make file size/time stamp changes only on file close
            ptr_utFile->ulFileMode |= _FILE_CHANGED;                     // mark that a change is still required - it takes place on file close
        }
        else {
            int iReturn = fnCommitFileInfo(ptr_utFile, ptr_utDisk);
            if (UTFAT_SUCCESS != iReturn) {
                return iReturn;
            }
        }
    }
    #if defined UTMANAGED_FILE_COUNT && UTMANAGED_FILE_COUNT > 0         // allow SD card interface to be used without UTMANAGED_FILE_COUNT
    fnSynchroniseManagedFile(ptr_utFile);
    #endif
    return UTFAT_SUCCESS;
}
#endif

// Change the directory location with reference to its present position
//
extern int utChangeDirectory(const CHAR *ptrDirPath, UTDIRECTORY *ptrDirObject)
{
    unsigned short usPathTerminator = ptrDirObject->usRelativePathLocation; // original path location
    ptrDirObject->usDirectoryFlags |= UTDIR_ALLOW_MODIFY_PATH;           // allow the open to modify the path if necessary
    switch (utOpenDirectory(ptrDirPath, ptrDirObject)) {                 // try to locate the new directory
    case UTFAT_PATH_IS_ROOT_REF:
        ptrDirObject->usRelativePathLocation = usPathTerminator = 3;     // reset the root string
        ptrDirPath++;                                                    // jump the root reference and fall through to set new path
    case UTFAT_SUCCESS:
        if (ptrDirObject->ptrDirectoryPath != 0) {
            if (*ptrDirPath != 0) {
                if (usPathTerminator > 3) {
                    ptrDirObject->ptrDirectoryPath[usPathTerminator] = '\\';
                }
                if (usPathTerminator != 3) {
                    usPathTerminator++;
                }
                ptrDirObject->usRelativePathLocation = (uStrcpy(&ptrDirObject->ptrDirectoryPath[usPathTerminator], ptrDirPath) - ptrDirObject->ptrDirectoryPath);
            }
            else {
                ptrDirObject->usRelativePathLocation = usPathTerminator;
                if (usPathTerminator == 2) {
                    usPathTerminator = 3;
                }
                ptrDirObject->ptrDirectoryPath[usPathTerminator] = 0;
            }
        }
        break;
    case UTFAT_PATH_IS_ROOT:                                             // change was to the root
        ptrDirObject->usRelativePathLocation = 3;                        // reset the root string
        ptrDirObject->ptrDirectoryPath[3] = 0;
        break;
    case UTFAT_DISK_NOT_READY:
        return UTFAT_DISK_NOT_READY;
    case UTFAT_DISK_READ_ERROR:
        return UTFAT_DISK_READ_ERROR;
    case UTFAT_SUCCESS_PATH_MODIFIED:                                    // successful and no further action required since it has been performed
        break;
    default:
         return UTFAT_PATH_NOT_FOUND;
    }
    uMemcpy(&ptrDirObject->private_disk_location, &ptrDirObject->public_disk_location, sizeof(ptrDirObject->private_disk_location)); // commit the new directory location
    return UTFAT_SUCCESS;
}

// Move through sectors and cluster chain until the end sector is reached
//
static int fnRunThroughClusters(UTDISK *ptr_utDisk, FILE_LOCATION *prtFileLocation, unsigned long ulStartPosition, unsigned long ulEndPosition)
{
    int iResult = UTFAT_SUCCESS;
    unsigned short usBytesPerSector = ptr_utDisk->utFAT.usBytesPerSector;
    ulStartPosition &= ~(usBytesPerSector - 1);                          // round down to start of present sector
    ulEndPosition &= ~(usBytesPerSector - 1);                            // round down to start of end sector
    while (ulStartPosition < ulEndPosition) {
        ulStartPosition += usBytesPerSector;
        if ((iResult = fnNextSectorCreate(ptr_utDisk, prtFileLocation, 1)) != UTFAT_SUCCESS) { // if the end of the file is reached, which is at the end of a cluster, a new cluster will be created if write is supported
            break;
        }
    }
    return iResult;
}

extern int utSeek(UTFILE *ptr_utFile, unsigned long ulPosition, int iSeekType)
{
    int iResult = UTFAT_SUCCESS;
    unsigned long ulNewPosition = 0;                                     // default to start of file
    UTDISK *ptr_utDisk = &utDisks[ptr_utFile->ucDrive];
    switch (iSeekType) {
    case UTFAT_SEEK_SET:                                                 // relative to the start of the file - always forwards
        ulNewPosition = ulPosition;
        break;
    case UTFAT_SEEK_CUR:                                                 // relative to current position
        if ((signed long)ulPosition < 0) {                               // relative backwards
            if ((unsigned long)(-(signed long)ulPosition) > ptr_utFile->ulFilePosition) {
                break;                                                   // move to beginning of file
            }
        }
        else {                                                           // relative forwards
            if (ulPosition > (ptr_utFile->ulFileSize - ptr_utFile->ulFilePosition)) {
                ulPosition = (ptr_utFile->ulFileSize - ptr_utFile->ulFilePosition);
            }
        }
        ulNewPosition = ptr_utFile->ulFilePosition + ulPosition;
        break;
    case UTFAT_SEEK_END:                                                 // relative to the end of the file (always backwards)
        if (ulPosition < ptr_utFile->ulFileSize) {
            ulNewPosition = (ptr_utFile->ulFileSize - ulPosition);
        }
        break;
    }
    if (ulNewPosition != ptr_utFile->ulFilePosition) {                   // update the cluster pointer
        if (ulNewPosition < ptr_utFile->ulFilePosition) {                // move back
            uMemcpy(&ptr_utFile->public_file_location, &ptr_utFile->private_file_location, sizeof(ptr_utFile->private_file_location)); // rewind to start and search from there
            ptr_utFile->ulFilePosition = 0;
        }
        else if (ulNewPosition > ptr_utFile->ulFileSize) {               // limit seek to end of file
            ulNewPosition = ptr_utFile->ulFileSize;                      
        }
        iResult = fnRunThroughClusters(ptr_utDisk, &ptr_utFile->public_file_location, ptr_utFile->ulFilePosition, ulNewPosition);
        if (iResult == UTFAT_SUCCESS) {                                  // if the cluster location could be successfully set
            ptr_utFile->ulFilePosition = ulNewPosition;                  // set the file position to the new position
        }
    }
    return iResult;
}

#if defined UTFAT_WRITE                                                  // support writing as well as reading
    #if defined NAND_FLASH_FAT

static int utCommitSectorData(UTDISK *ptr_utDisk, void *ptrBuffer, unsigned long ulSectorNumber)
{
    static unsigned long ulSector;
    switch (iMemoryOperation[DISK_D] & _WRITING_MEMORY) {
    case _IDLE_MEMORY:
        iMemoryOperation[DISK_D] |= _WRITING_MEMORY;
        ulSector = ulSectorNumber;
    case _WRITING_MEMORY:
        {
            if (fnCheckBlankPage(ulSectorNumber) != 0) {
                if (fnOverWriteSector(ulSectorNumber, ptrBuffer) != UTFAT_SUCCESS) {
                    return UTFAT_DISK_WRITE_ERROR;
                }
            }
            else {
                if (fnWriteNANDsector(ulSector, 0, ptrBuffer, 512) != UTFAT_SUCCESS) {
                    fnMemoryDebugMsg("Write error\r\n");
                    return UTFAT_DISK_WRITE_ERROR;
                }
            }
            iMemoryOperation[DISK_D] &= ~_WRITING_MEMORY;                // write operation has completed
        }
        break;
    }
    return UTFAT_SUCCESS;
}

// Delete the specified sector by writing data content of 0x00
//
static int utDeleteSector(UTDISK *ptr_utDisk, unsigned long ulSectorNumber)
{
    static unsigned long ulSector;
    switch (iMemoryOperation[DISK_D] & _WRITING_MEMORY) {
    case _IDLE_MEMORY:
        iMemoryOperation[DISK_D] |= _WRITING_MEMORY;
        ulSector = ulSectorNumber;
    case _WRITING_MEMORY:
        {
            unsigned long ulTemp[512/sizeof(unsigned long)];             // temporary long-word aligned buffer
    #if defined LONG_UMEMSET
            uMemset_long(ulTemp, 0x00, sizeof(ulTemp));                  // zero buffer content for delete
    #else
            uMemset(ulTemp, 0x00, sizeof(ulTemp));
    #endif
            if (fnWriteNANDsector(ulSector, 0, (unsigned char *)ulTemp, 512) != 0) {
                fnMemoryDebugMsg("Write error\r\n");
                return UTFAT_DISK_WRITE_ERROR;
            } 
            iMemoryOperation[DISK_D] &= ~_WRITING_MEMORY;                // write operation has completed
        }
        break;
    }
    return UTFAT_SUCCESS;
}
    #endif

static int fnDeleteClusterChain(unsigned long ulClusterStart, unsigned char ucDrive, int iDestroyClusters)
{
    UTDISK *ptr_utDisk = &utDisks[ucDrive];
    unsigned long ulCluster = (((ulClusterStart - ptr_utDisk->ulLogicalBaseAddress)/ptr_utDisk->utFAT.ucSectorsPerCluster) + ptr_utDisk->ulDirectoryBase);
    unsigned long ulClusterSector;
    unsigned long ulNextCluster, ulNextClusterSector;
    unsigned long ulRemovedClusters = 0;
    unsigned char ucClusterEntry;
    unsigned long ulSectorContent[512/sizeof(signed long)];              // temporary long-word aligned buffer
    #if defined UTFAT16 || defined UTFAT12
    unsigned long ulClusterMask;
    if (ptr_utDisk->usDiskFlags & (DISK_FORMAT_FAT12 | DISK_FORMAT_FAT16)) {
        ulClusterSector = (ptr_utDisk->utFAT.ulFAT_start + (ulCluster >> 8)); // section where the FAT responsible for this cluster resides
        ulCluster -= (32 - 1);                                           // compensate for fixed 16k boot sector
        ucClusterEntry = (unsigned char)ulCluster;
        ulClusterMask = FAT16_CLUSTER_MASK;
    }
    else {
        ulClusterSector = (ptr_utDisk->utFAT.ulFAT_start + (ulCluster >> 7)); // section where the FAT responsible for this cluster resides
        ucClusterEntry = (unsigned char)(ulCluster & 0x7f);
        ulClusterMask = CLUSTER_MASK;
    }
    #else
    ulClusterSector = (ptr_utDisk->utFAT.ulFAT_start + (ulCluster >> 7)); // section where the FAT responsible for this cluster resides
    ucClusterEntry = (unsigned char)(ulCluster & 0x7f);
    #endif
    if ((_utReadDiskSector[ucDrive](ptr_utDisk, ulClusterSector, ulSectorContent)) != UTFAT_SUCCESS) { // read a FAT sector containing the cluster information
        return UTFAT_DISK_READ_ERROR;
    }
    while (1) {
    #if defined UTFAT16 || defined UTFAT12
        if (ptr_utDisk->usDiskFlags & (DISK_FORMAT_FAT12 | DISK_FORMAT_FAT16)) {
            ulNextCluster = LITTLE_LONG_WORD(ulSectorContent[ucClusterEntry/2]);
            if (ucClusterEntry & 1) {
                ulNextCluster &= LITTLE_LONG_WORD(0xffff0000);
                ulNextCluster >>= 16;
            }
            else {
                ulNextCluster &= LITTLE_LONG_WORD(0x0000ffff);
            }
        }
        else {
            ulNextCluster = LITTLE_LONG_WORD(ulSectorContent[ucClusterEntry]); // read the next entry
        }
        if (((ulNextCluster & ulClusterMask) == ulClusterMask) || (ulNextCluster <= ptr_utDisk->ulDirectoryBase)) // check whether the end of the cluster chain has been reached (if the next cluster is 0, 1 or 2 it is ignored since it must be corrupt - 0, 1 are not used and 2 is always the start of the root directory! [FAT16 is 0..1])
    #else
        ulNextCluster = LITTLE_LONG_WORD(ulSectorContent[ucClusterEntry]); // read the next entry
        if (((ulNextCluster & CLUSTER_MASK) == CLUSTER_MASK) || (ulNextCluster <= ptr_utDisk->ulDirectoryBase)) // check whether the end of the cluster chain has been reached (if the next cluster is 0, 1 or 2 it is ignored since it must be corrupt - 0, 1 are not used and 2 is always the start of the root directory!)
    #endif
        {
            int iResult = UTFAT_SUCCESS;
            unsigned char ucFatCopyCount = 0;
            if (iDestroyClusters == REUSE_CLUSTERS) {                    // if the file's cluster is to be reused and only occupied one cluster, don't delete it
                break;
            }
            else {
    #if defined UTFAT16 || defined UTFAT12
                if (ptr_utDisk->usDiskFlags & (DISK_FORMAT_FAT12 | DISK_FORMAT_FAT16)) {
                    if (ucClusterEntry & 1) {
                        ulSectorContent[ucClusterEntry/2] &= ~LITTLE_LONG_WORD(0xffff0000);
                    }
                    else {
                        ulSectorContent[ucClusterEntry/2] &= ~LITTLE_LONG_WORD(0x0000ffff);
                    }
                }
                else {
                    ulSectorContent[ucClusterEntry] = 0;                 // delete the previous entry
                }
    #else
                ulSectorContent[ucClusterEntry] = 0;                     // delete the previous entry
    #endif
                if (ptr_utDisk->usDiskFlags & FSINFO_VALID) {
                    ptr_utDisk->utFileInfo.ulFreeClusterCount += ++ulRemovedClusters;
                    ptr_utDisk->usDiskFlags |= WRITEBACK_INFO_FLAG;      // mark that the info block information has changed
                }
            }
            while (ucFatCopyCount < ptr_utDisk->utFAT.ucNumberOfFATs) {
                while ((iResult = _utCommitSectorData[ucDrive](ptr_utDisk, ulSectorContent, (ulClusterSector + (ucFatCopyCount * ptr_utDisk->utFAT.ulFatSize)))) == CARD_BUSY_WAIT) {}
                ucFatCopyCount++;
            }
            return iResult;
        }
        if (iDestroyClusters == REUSE_CLUSTERS) {                        // if the file's cluster is to be reused
    #if defined UTFAT16 || defined UTFAT12
            if (ptr_utDisk->usDiskFlags & (DISK_FORMAT_FAT12 | DISK_FORMAT_FAT16)) {
                if (ucClusterEntry & 1) {
                    ulSectorContent[ucClusterEntry/2] = ulSectorContent[ucClusterEntry/2] & ~LITTLE_LONG_WORD(0xffff0000);
                    ulSectorContent[ucClusterEntry/2] = (ulSectorContent[ucClusterEntry/2] | LITTLE_LONG_WORD(FAT16_CLUSTER_MASK << 16)); // mark last cluster in extension
                }
                else {
                    ulSectorContent[ucClusterEntry/2] = ulSectorContent[ucClusterEntry/2] & ~LITTLE_LONG_WORD(0x0000ffff);
                    ulSectorContent[ucClusterEntry/2] = (ulSectorContent[ucClusterEntry/2] | LITTLE_LONG_WORD(FAT16_CLUSTER_MASK)); // mark last cluster in extension
                }
            }
            else {
                ulSectorContent[ucClusterEntry] = CLUSTER_MASK;          // mark the end of the cluster chain
            }
    #else
            ulSectorContent[ucClusterEntry] = CLUSTER_MASK;              // mark the end of the cluster chain
    #endif
            iDestroyClusters = SIMPLE_DELETE;                            // from here on, destroy rest of chain
        }
        else {
    #if defined UTFAT16 || defined UTFAT12
            if (ptr_utDisk->usDiskFlags & (DISK_FORMAT_FAT12 | DISK_FORMAT_FAT16)) {
                if (ucClusterEntry & 1) {
                    ulSectorContent[ucClusterEntry/2] &= ~LITTLE_LONG_WORD(0xffff0000);
                }
                else {
                    ulSectorContent[ucClusterEntry/2] &= ~LITTLE_LONG_WORD(0x0000ffff);
                }
            }
            else {
                ulSectorContent[ucClusterEntry] = 0;                     // delete the previous entry
            }
    #else
            ulSectorContent[ucClusterEntry] = 0;                         // delete the previous entry
    #endif
            ulRemovedClusters++;
        }
    #if defined UTFAT16 || defined UTFAT12
        if (ptr_utDisk->usDiskFlags & (DISK_FORMAT_FAT12 | DISK_FORMAT_FAT16)) {
            ulNextClusterSector = (ptr_utDisk->utFAT.ulFAT_start + (ulNextCluster >> 8));
            ucClusterEntry = (unsigned char)(ulNextCluster);
        }
        else {
            ulNextClusterSector = (ptr_utDisk->utFAT.ulFAT_start + (ulNextCluster >> 7));
            ucClusterEntry = (unsigned char)(ulNextCluster & 0x7f);
        }
    #else
        ulNextClusterSector = (ptr_utDisk->utFAT.ulFAT_start + (ulNextCluster >> 7));
        ucClusterEntry = (unsigned char)(ulNextCluster & 0x7f);
    #endif
        if (ulNextClusterSector != ulClusterSector) {                    // moving to a new sector so update the FAT
            unsigned char ucFatCopyCount = 0;
            while (ucFatCopyCount < ptr_utDisk->utFAT.ucNumberOfFATs) {
                while (_utCommitSectorData[ucDrive](ptr_utDisk, ulSectorContent, (ulClusterSector + (ucFatCopyCount * ptr_utDisk->utFAT.ulFatSize))) == CARD_BUSY_WAIT) {}
                ucFatCopyCount++;
            }
            while (_utCommitSectorData[ucDrive](ptr_utDisk, ulSectorContent, ulClusterSector) == CARD_BUSY_WAIT) {}
            ulClusterSector = ulNextClusterSector;
            if ((_utReadDiskSector[ucDrive](ptr_utDisk, ulClusterSector, ulSectorContent)) != UTFAT_SUCCESS) { // read a FAT sector containing the cluster information
                return UTFAT_DISK_READ_ERROR;
            }
        }
    }
    return UTFAT_SUCCESS;
}

static int fnDeleteFileContent(UTFILE *ptr_utFile, UTDISK *ptr_utDisk, int iDestroyClusters)
{
    int iResult;
    if (iDestroyClusters != SIMPLE_DELETE) {                             // the file is to be re-used so set length to zero
        fnSetFileInformation(ptr_utFile->ptr_utDirObject->ptrEntryStructure, 0); // set the file size to zero
    }
    #if defined UTFAT_LFN_READ && (defined UTFAT_LFN_DELETE || defined UTFAT_LFN_WRITE) // if long file names are in use, delete also possible long file name    
    if (iDestroyClusters != REUSE_CLUSTERS) {
        if ((iResult = fnDeleteLFN_entry(ptr_utFile)) != UTFAT_SUCCESS) {// delete the long file name as long as the file is not being truncated for reuse
            return iResult;                                              // return error code
        }
    }
    #endif
    #if defined UTFAT_SAFE_DELETE
    if (iDestroyClusters == SAFE_DELETE) {
        uMemset(ptr_utFile->ptr_utDirObject->ptrEntryStructure, 0, sizeof(DIR_ENTRY_STRUCTURE_FAT32)); // completely remove the directory entry content
    }
    #endif
    while (_utCommitSectorData[ptr_utFile->ucDrive](ptr_utDisk, ptr_utDisk->ptrSectorData, ptr_utDisk->ulPresentSector) == CARD_BUSY_WAIT) {} // commit the file entry change
    ptr_utDisk->usDiskFlags &= ~WRITEBACK_BUFFER_FLAG;
    if (ptr_utFile->private_file_location.ulSector >= ptr_utDisk->ulLogicalBaseAddress) { // don't delete cluster chain if there is none allocated
        iResult = fnDeleteClusterChain(ptr_utFile->private_file_location.ulSector, ptr_utDisk->ucDriveNumber, iDestroyClusters); // free up all clusters belonging to the file content
        if (iResult < 0) {
            return iResult;                                              // return error cause
        }
    }
    return fnCommitInfoChanges(ptr_utDisk);                              // update the info sector accordingly if it has changed
}

static int _utDeleteFile(const CHAR *ptrFilePath, UTDIRECTORY *ptrDirObject, int iSafeDelete)
{
    int iResult;
    UTFILE utFile;
    UTDISK *ptr_utDisk = &utDisks[ptrDirObject->ucDrive];
    utFile.ptr_utDirObject = ptrDirObject;
    ptrDirObject->usDirectoryFlags |= (UTDIR_TEST_REL_PATH);
    iResult = _utOpenFile(ptrFilePath, &utFile, UTFAT_OPEN_FOR_DELETE);
    if (iResult == UTFAT_SUCCESS) {                                      // directory has been located
        DISK_LOCATION *ptrDirContent = &utFile.private_disk_location;
        DIR_ENTRY_STRUCTURE_FAT32 *ptrDirEntry;
        uMemcpy(&utFile.private_file_location, &utFile.private_disk_location.directory_location, sizeof(utFile.private_disk_location.directory_location));
        do {
            if (fnLoadSector(ptr_utDisk, utFile.private_disk_location.directory_location.ulSector) != UTFAT_SUCCESS) {
                return UTFAT_DISK_READ_ERROR;
            }
            ptrDirEntry = (DIR_ENTRY_STRUCTURE_FAT32 *)ptr_utDisk->ptrSectorData; // the directory entry in the sector buffer
            ptrDirEntry += ptrDirContent->ucDirectoryEntry;              // move to the present entry
            if (ptrDirEntry->DIR_Name[0] == 0) {
                break;                                                   // directory is empty so allow delete
            }
            else if (ptrDirEntry->DIR_Name[0] != DIR_NAME_FREE) {
                return UTFAT_DIR_NOT_EMPTY;
            }
            if (fnNextDirectoryEntry(ptr_utDisk, ptrDirContent) == UTFAT_DIRECTORY_AREA_EXHAUSTED) { // move to the next entry
                return UTFAT_DIRECTORY_AREA_EXHAUSTED;
            }
        } while (1);
        ptrDirObject->usDirectoryFlags |= (UTDIR_TEST_REL_PATH | UTDIR_DIR_AS_FILE); // handle a directory as a file so that it can also be deleted if found
        iResult = _utOpenFile(ptrFilePath, &utFile, UTFAT_OPEN_FOR_DELETE);
        if (iResult == UTFAT_SUCCESS) {
            utFile.ptr_utDirObject->ptrEntryStructure->DIR_Name[0] = DIR_NAME_FREE; // delete the (short) file name
            return (fnDeleteFileContent(&utFile, ptr_utDisk, iSafeDelete));
        }
    }
    else if (iResult == UTFAT_PATH_IS_FILE) {                            // if file has been located
        if (utFile.ptr_utDirObject->ptrEntryStructure->DIR_Attr & DIR_ATTR_READ_ONLY) {
            return UTFAT_FILE_NOT_WRITEABLE;                             // the file is read-only so cannot be deleted
        }
    #if defined UTFAT_SAFE_DELETE
        if (iSafeDelete == SAFE_DELETE) {                                // destroy the file content
            unsigned long ulLength = utFile.ulFileSize;                  // the content size
            unsigned short usWriteLength = 512;
            unsigned long ulEmptyBuffer[512/sizeof(unsigned long)];
            uMemset(ulEmptyBuffer, 0, sizeof(ulEmptyBuffer));            // empty buffer content
            utSeek(&utFile, 0, UTFAT_SEEK_SET);                          // ensure that the file pointer is set to the start fo the file
            while (ulLength != 0) {
                if (ulLength < 512) {
                    usWriteLength = (unsigned short)ulLength;
                }
                utWriteFile(&utFile, (unsigned char *)ulEmptyBuffer, usWriteLength);
                ulLength -= usWriteLength;
            }
        }
    #endif
        utFile.ptr_utDirObject->ptrEntryStructure->DIR_Name[0] = DIR_NAME_FREE; // delete the (short) file name
        return (fnDeleteFileContent(&utFile, ptr_utDisk, iSafeDelete));
    }
    return iResult;
}

extern int utDeleteFile(const CHAR *ptrFilePath, UTDIRECTORY *ptrDirObject)
{
    return _utDeleteFile(ptrFilePath, ptrDirObject, SIMPLE_DELETE);
}

    #if defined UTFAT_SAFE_DELETE
extern int utSafeDeleteFile(const CHAR *ptrFilePath, UTDIRECTORY *ptrDirObject)
{
    return _utDeleteFile(ptrFilePath, ptrDirObject, SAFE_DELETE);
}
    #endif

// Make a new directory - don't allow if a directory of the same name already exists
//
extern int utMakeDirectory(const CHAR *ptrDirPath, UTDIRECTORY *ptrDirObject)
{
    UTDISK *ptr_utDisk = &utDisks[ptrDirObject->ucDrive];
    OPEN_FILE_BLOCK openBlock;
    int iReturn;

    uMemset(&openBlock, 0, sizeof(openBlock));                           // initialise open file block

    if (ptr_utDisk->usDiskFlags & WRITE_PROTECTED_SD_CARD) {
        return UTFAT_DISK_WRITE_PROTECTED;
    }
    ptrDirObject->usDirectoryFlags |= UTDIR_SET_START;                   // set to start of lowest directory when file not found

    openBlock.iContinue = 0;
    iReturn = _fnHandlePath(&openBlock, ptrDirPath, ptrDirObject);       // handle the input path string
    if (openBlock.iContinue == 0) {                                      // if the path handling had an error or completed all work
        return iReturn;                                                  // return with code
    }

    if ((iReturn = _utOpenDirectory(&openBlock, ptrDirObject, UTFAT_CREATE)) != UTFAT_FILE_NOT_FOUND) { // check that the directory doesn't already exist {3} pass create flag
        if (UTFAT_SUCCESS == iReturn) {
            return UTFAT_DIRECTORY_EXISTS_ALREADY;                       // directory already exists
        }
        return iReturn;                                                  // another error
    }
    openBlock.ptr_utDisk = ptr_utDisk;
    uMemcpy(&openBlock.DirectoryEndLocation, &ptrDirObject->public_disk_location, sizeof(openBlock.DirectoryEndLocation));
    return (fnCreateFile(&openBlock, 0, /*openBlock.ptrLocalDirPath,*/ 0)); // {4} create a new directory
}

/// Reformat a disk that is presently formatted
//
static int utReFormat(const unsigned char ucDrive, const CHAR *cVolumeLabel, unsigned char ucFlags) // static and pass flags
{
    if (utDisks[ucDrive].usDiskFlags & WRITE_PROTECTED_SD_CARD) {
        return UTFAT_DISK_WRITE_PROTECTED;
    }
    iMemoryOperation[ucDrive] = 0;
    utDisks[ucDrive].usDiskFlags &= (HIGH_CAPACITY_SD_CARD);             // remove all flags apart from high capacity information
    utDisks[ucDrive].usDiskFlags |= DISK_UNFORMATTED;                    // consider unformatted from this point on
    #if defined UTFAT16 || defined UTFAT12
    if (ucFlags & (UTFAT_FORMAT_16)) {
        #if defined UTFAT16
        utDisks[ucDrive].usDiskFlags |= DISK_FORMAT_FAT16;               // FAT16 to be formatted rather than FAT32
        iMemoryState[ucDrive] = STATE_FORMATTING_DISK_2;                 // no extended record added to FAT16
        #endif
    }
        #if defined UTFAT12
    else if (ucFlags & (UTFAT_FORMAT_12)) {
        utDisks[ucDrive].usDiskFlags |= DISK_FORMAT_FAT12;               // FAT12 to be formatted rather than FAT32
        iMemoryState[ucDrive] = STATE_FORMATTING_DISK_2;                 // no extended record added to FAT12
    }
        #endif
    else {
        iMemoryState[ucDrive] = STATE_FORMATTING_DISK_1;                 // start with an extended record when formatting to FAT32
    }
    #else
    iMemoryState[ucDrive] = STATE_FORMATTING_DISK_1;
    #endif
    if (ucFlags & UTFAT_FULL_FORMAT) {
        utDisks[ucDrive].usDiskFlags |= DISK_FORMAT_FULL;                // delete all data content as well as just FAT content
    }
    uMemcpy(utDisks[ucDrive].cVolumeLabel, cVolumeLabel, 11);
    uTaskerStateChange(OWN_TASK, UTASKER_ACTIVATE);                      // schedule the task to start formatting
    return UTFAT_SUCCESS;
}

// Format a disk that is presently not unformatted
//
extern int utFormat(const unsigned char ucDrive, const CHAR *cVolumeLabel, unsigned char ucFlags)
{
    if (((ucFlags & UTFAT_REFORMAT) == 0) && (iMemoryState[ucDrive] != DISK_NOT_FORMATTED)) {
        return UTFAT_DISK_NOT_READY;
    }
    return utReFormat(ucDrive, cVolumeLabel, ucFlags);
}

// Truncate the file to the present file pointer location
// - this usually makes the file smaller and cleans up no longer used clusters
//
extern int utTruncateFile(UTFILE *ptr_utFile)
{
    if (ptr_utFile->ulFileSize > ptr_utFile->ulFilePosition) {           // if the file pointer is at the end of the present file there is nothing to do
        if ((ptr_utFile->ulFileMode & UTFAT_OPEN_FOR_WRITE) == 0) {      // only allow truncation if the file is open for writing
            return UTFAT_FILE_NOT_WRITEABLE;
        }
        else {
            UTDISK *ptr_utDisk = &utDisks[ptr_utFile->ucDrive];
            DIR_ENTRY_STRUCTURE_FAT32 *ptrFileEntry = (DIR_ENTRY_STRUCTURE_FAT32 *)ptr_utDisk->ptrSectorData; // the directory entry in the sector buffer
            ptrFileEntry += ptr_utFile->private_disk_location.ucDirectoryEntry; // move to the file entry
            if (fnLoadSector(ptr_utDisk, ptr_utFile->private_disk_location.directory_location.ulSector) != UTFAT_SUCCESS) { // ensure that the directory sector is loaded
                return UTFAT_DISK_READ_ERROR;
            }
            // We set the new file size to be equal to the present file pointer location
            //
            ptr_utFile->ulFileSize = ptr_utFile->ulFilePosition;
            fnSetFileInformation(ptrFileEntry, ptr_utFile->ulFileSize);

            while (_utCommitSectorData[ptr_utFile->ucDrive](ptr_utDisk, ptr_utDisk->ptrSectorData, ptr_utFile->private_disk_location.directory_location.ulSector) == CARD_BUSY_WAIT) {} // force writeback to finalise the operation
            ptr_utDisk->usDiskFlags &= ~WRITEBACK_BUFFER_FLAG;           // the disk is up to date with the buffer


            // We need to free all clusters no longer needed by content after the new end of the file
            //
            if (ptr_utFile->public_file_location.ulSector >= ptr_utDisk->ulLogicalBaseAddress) { // don't delete cluster chain if there is none allocated
                return (fnDeleteClusterChain(ptr_utFile->public_file_location.ulSector, ptr_utDisk->ucDriveNumber, REUSE_CLUSTERS)); // free up all clusters belonging to the file content
            }
        }
    }
    return UTFAT_SUCCESS;
}

    #if defined UTFAT_EXPERT_FUNCTIONS
// Change a file or directory attribute
//
extern int utFileAttribute(UTFILE *ptr_utFile, int iNewAttributes)
{
    if ((ptr_utFile->ulFileMode & UTFAT_OPEN_FOR_WRITE) == 0) {          // only allow changing the attribute if the file is open for writing
        return UTFAT_FILE_NOT_WRITEABLE;
    }
    else {
        UTDISK *ptr_utDisk = &utDisks[ptr_utFile->ucDrive];
        DIR_ENTRY_STRUCTURE_FAT32 *ptrFileEntry = (DIR_ENTRY_STRUCTURE_FAT32 *)ptr_utDisk->ptrSectorData; // the directory entry in the sector buffer
        unsigned long ulSector;
        unsigned char ucEntry;
        if ((iNewAttributes & FILE_ATTRIBUTE_OF_DIRECTORY) != 0) {
            ucEntry = ptr_utFile->ptr_utDirObject->private_disk_location.ucDirectoryEntry;
            ulSector = ptr_utFile->ptr_utDirObject->private_disk_location.directory_location.ulSector;
        }
        else {
            ucEntry = ptr_utFile->private_disk_location.ucDirectoryEntry;
            ulSector = ptr_utFile->private_disk_location.directory_location.ulSector;
        }
        if (fnLoadSector(ptr_utDisk, ulSector) != UTFAT_SUCCESS) {       // ensure that the directory sector is loaded
            return UTFAT_DISK_READ_ERROR;
        }
        ptrFileEntry += ucEntry;                                         // move to the file entry
        if ((iNewAttributes & FILE_ATTRIBUTE_SET_HIDE) != 0) {
            ptrFileEntry->DIR_Attr |= DIR_ATTR_HIDDEN;
        }
        else if ((iNewAttributes & FILE_ATTRIBUTE_REMOVE_HIDE) != 0) {
            ptrFileEntry->DIR_Attr &= ~(DIR_ATTR_HIDDEN);
        }
        if ((iNewAttributes & FILE_ATTRIBUTE_SET_WRITE_PROTECT) != 0) {
            ptrFileEntry->DIR_Attr |= (DIR_ATTR_READ_ONLY);
        }
        else if ((iNewAttributes & FILE_ATTRIBUTE_REMOVE_WRITE_PROTECT) != 0) {
            ptrFileEntry->DIR_Attr &= ~(DIR_ATTR_READ_ONLY);
        }

        while (_utCommitSectorData[ptr_utFile->ucDrive](ptr_utDisk, ptr_utDisk->ptrSectorData, ulSector) == CARD_BUSY_WAIT) {} // force writeback to finalise the operation
        ptr_utDisk->usDiskFlags &= ~WRITEBACK_BUFFER_FLAG;               // the disk is up to date with the buffer
    }
    return UTFAT_SUCCESS;
}
    #endif
#endif


// This routine is called to count the free clusters and update the disk information
//
extern int utFreeClusters(unsigned char ucDisk, UTASK_TASK owner_task)
{
    if (cluster_task[ucDisk] != 0) {                                     // cluster count already in progress
        return MISSING_USER_TASK_REFERENCE;                              // the user must pass a task reference
    }
    ulActiveFreeClusterCount[ucDisk] = 0;                                // reset the free-cluster counter
#if defined UTFAT12
    ulMaximumClusterCount[ucDisk] = utDisks[ucDisk].utFAT.ulClusterCount;// maximum clusters to check
#endif
#if defined FAT12_DEVELOPMENT                                            // FAT12 requires scanning in the forward direction
    ulClusterSectorCheck[ucDisk] = utDisks[ucDisk].utFAT.ulFAT_start;    // set to first FAT sector
#else
    ulClusterSectorCheck[ucDisk] = (utDisks[ucDisk].utFAT.ulFAT_start + (utDisks[ucDisk].utFAT.ulFatSize - 1)); // set to final FAT sector
#endif
    iMemoryOperation[ucDisk] |= _COUNTING_CLUSTERS;
    cluster_task[ucDisk] = owner_task;
    uTaskerStateChange(OWN_TASK, UTASKER_ACTIVATE);                      // start cluster counting operation
    return UTFAT_SUCCESS;
}

// Routine to check for a certain file extension (type of file)
//
extern int uMatchFileExtension(UTFILEINFO *ptrFileInfo, const CHAR *ptrExtension)
{
    int i = 0;
    CHAR *ptrName = ptrFileInfo->cFileName;
    while (i++ < MAX_UTFAT_FILE_NAME) {
        if (*ptrName++ == '.') {
            int iMatch = uStrEquiv(ptrName, ptrExtension);
            if (iMatch > 0) {
                if ((*(ptrName + iMatch) == 0) && (*(ptrExtension + iMatch) == 0)) {
                    return 0;                                            // successful match
                }
            }
            break;                                                       // match failed
        }
    }
    return -1;                                                           // file type doesn't match
}

// Low level access routines
//
extern int fnReadSector(unsigned char ucDisk, unsigned char *ptrBuffer, unsigned long ulSectorNumber)
{
    if (ptrBuffer == 0) {                                                // if a zero pointer is given force a load but don't copy to a buffer
        return (fnLoadSector(&utDisks[ucDisk], ulSectorNumber));
    }
    return (_utReadDiskSector[ucDisk](&utDisks[ucDisk], ulSectorNumber, ptrBuffer));
}

#if defined UTFAT_WRITE                                                  // allow operation without write support

#if defined UTFAT_MULTIPLE_BLOCK_WRITE
// Prepare the write to sectors if there are capabilities to improve the following write speed
//
extern int fnPrepareBlockWrite(unsigned char ucDisk, unsigned long ulWriteBlocks, int iPreErase)
{
    if (ulBlockWriteLength == 0) {                                       // if a multi block write is not already in operations
    #if defined UTFAT_PRE_ERASE
        if ((iPreErase != 0) && (ulWriteBlocks > 1)) {                   // ignore if single block is to written or no pre-erase is requested
            unsigned char ucResult;
            int iActionResult;
            SET_SD_CS_LOW();
            while ((iActionResult = fnSendSD_command(ucAPP_CMD_CMD55, &ucResult, 0)) == CARD_BUSY_WAIT) {}
            while ((iActionResult = fnSendSD_command(fnCreateCommand(PRE_ERASE_BLOCKS_CMD23, ulWriteBlocks), &ucResult, 0)) == CARD_BUSY_WAIT) {}
            SET_SD_CS_HIGH();
        }
    #endif
        ulBlockWriteLength = ulWriteBlocks;                              // set the number of blocks to be written
    }
    else {                                                               // test abort
        //ulBlockWriteLength = 0x80000000;
    }
    return UTFAT_SUCCESS;
}
#endif

// Write a sector with data from a passed buffer
//
extern int fnWriteSector(unsigned char ucDisk, unsigned char *ptrBuffer, unsigned long ulSectorNumber)
{
    int iResult;
    UTDISK *ptr_utDisk = &utDisks[ucDisk];
    while ((iResult = _utCommitSectorData[ucDisk](ptr_utDisk, ptrBuffer, ulSectorNumber)) == CARD_BUSY_WAIT) {}
    if (ulSectorNumber == ptr_utDisk->ulPresentSector) {                 // {9} if the write was to the present sector the sector buffer cache is also updated
        uMemcpy(ptr_utDisk->ptrSectorData, ptrBuffer, ptr_utDisk->utFAT.usBytesPerSector);
    }
    return iResult;
}
#endif

extern const UTDISK *fnGetDiskInfo(unsigned char ucDisk)
{
    return &utDisks[ucDisk];
}

#if defined HTTP_ROOT || defined FTP_ROOT
extern int utServer(UTDIRECTORY *ptr_utDirectory, unsigned long ulServerType)
{
    if (ptr_utDirectory == 0) {                                          // change setting
        usServerStates &= ~(unsigned short)(ulServerType >> 16);         // disable
        usServerResets |= ((unsigned short)ulServerType & ~usServerStates); // the servers that have just been enabled can be reset once
        usServerStates |= (unsigned short)(ulServerType);                // enable
    }
    else {
        if (utDisks[ptr_utDirectory->ucDrive].usDiskFlags & (DISK_NOT_PRESENT | DISK_TYPE_NOT_SUPPORTED | DISK_UNFORMATTED)) { // unusable disk states
            usServerResets = 0;
            ptr_utDirectory->usDirectoryFlags &= (UTDIR_ALLOCATED);
            return UTFAT_DISK_NOT_READY;
        }
        if (!(usServerStates & ulServerType)) {                          // the server type has no access rights so return an error
            return UTFAT_DISK_NOT_READY;
        }
        if (usServerResets & ulServerType) {                             // if a server has been reset its root has to be confirmed
            ptr_utDirectory->usDirectoryFlags &= (UTDIR_ALLOCATED);
            usServerResets &= ~ulServerType;                             // only once
        }
        if (!(ptr_utDirectory->usDirectoryFlags & UTDIR_VALID)) {        // if the disk is not known to be valid try to locate the root directory
            const CHAR *ptrRoot;
            if (ulServerType & UTFAT_HTTP_SERVER) {
    #if defined HTTP_ROOT
                ptrRoot = HTTP_ROOT;
    #endif
            }
            else if (ulServerType & UTFAT_FTP_SERVER) {
    #if defined FTP_ROOT
                ptrRoot = FTP_ROOT;
    #endif
            }
            else {
                return UTFAT_DISK_NOT_READY;                             // unknown server
            }
            if (utOpenDirectory(ptrRoot, ptr_utDirectory) < 0) {         // open the root directory of disk D on connection (if no disk or no directory this fails and the HTTP server falls back to other file systems)
                return UTFAT_DISK_NOT_READY;
            }
        }
    }
    return UTFAT_SUCCESS;
}
#endif

#if defined SDCARD_DETECT_INPUT_INTERRUPT
// Interrupt call-back on change in SD-card presence
//
static void sdcard_detection_change(void)
{
    fnInterruptMessage(OWN_TASK, E_SDCARD_DETECTION_CHANGE);             // send interrupt event to task so that it can respond accordingly
}

// SD-card detection interrupt configuration
//
static void fnPrepareDetectInterrupt(void)
{
    INTERRUPT_SETUP interrupt_setup;                                     // interrupt configuration parameters
    interrupt_setup.int_type = PORT_INTERRUPT;                           // identifier when configuring
    interrupt_setup.int_handler = sdcard_detection_change;               // handling function
    #if defined _HW_AVR32                                                // AVR32 specific
    interrupt_setup.int_port = SDCARD_DETECT_PORT;                       // the port used
    interrupt_setup.int_port_bits = SDCARD_DETECT_PIN;                   // the input connected
    interrupt_setup.int_priority = PRIORITY_GPIO;                        // port interrupt priority
    interrupt_setup.int_port_sense = (IRQ_BOTH_EDGES | IRQ_ENABLE_GLITCH_FILER); // interrupt on both edges with active glitch filter
    #elif defined _KINETIS || defined _LPC23XX || defined _LPC17XX       // {54} KINETIS {57} LPC17xx and LPC2xxx
    interrupt_setup.int_port = SDCARD_DETECT_PORT;                       // the port used
    interrupt_setup.int_port_bits = SDCARD_DETECT_PIN;                   // the input connected
    interrupt_setup.int_priority = PRIORITY_SDCARD_DETECT_PORT_INT;      // port interrupt priority
    interrupt_setup.int_port_sense = (IRQ_BOTH_EDGES | PULLUP_ON);       // interrupt on both edges
    #elif defined _M5223X                                                // {55} Coldfire V2
    interrupt_setup.int_port_bit = SDCARD_DETECT_PIN;                    // the IRQ input connected
    interrupt_setup.int_priority = PRIORITY_SDCARD_DETECT_PORT_INT;      // port interrupt priority
    interrupt_setup.int_port_sense = (IRQ_BOTH_EDGES);                   // interrupt on both edges
    #elif defined _STM32                                                 // {56} SMT32
    interrupt_setup.int_port = SDCARD_DETECT_PORT;                       // the port used
    interrupt_setup.int_port_bit = SDCARD_DETECT_PIN;                    // the IRQ input connected
    interrupt_setup.int_priority = PRIORITY_SDCARD_DETECT_PORT_INT;      // port interrupt priority
    if (SDCARD_DETECTION() != 0) {
        interrupt_setup.int_port_sense = (IRQ_RISING_EDGE);              // interrupt on rising edge to detect card being removed
    }
    else {
        interrupt_setup.int_port_sense = (IRQ_FALLING_EDGE);             // interrupt on falling edge to detect card being inserted
    }
    #endif
    fnConfigureInterrupt((void *)&interrupt_setup);                      // configure test interrupt
}
#endif
#endif


#if defined FAT_EMULATION                                                // {12}

static const unsigned char *fnGetDataPointer(int iDisk, unsigned long ulSector, int *ptr_iFileReference)
{
    EMULATED_FILE_DETAILS fileDetails;
    int iFile = 0;
    iDisk -= DISK_EM_FAT;
    while (iFile < MAXIMUM_DATA_FILES) {
        while ((dataFileList[iDisk][iFile].iFileExists <= 0) || (dataFileList[iDisk][iFile].ulFormattedDatalength == 0)) { // jump non-existent files or those without length (and thus without clusters)
            if (++iFile >= MAXIMUM_DATA_FILES) {                         // end of files reached
                *ptr_iFileReference = -1;
                return 0;                                                // invalid file
            }
        }
        if ((ulSector >= dataFileList[iDisk][iFile].ulStartSector) && (ulSector <= dataFileList[iDisk][iFile].ulEndSector)) {
            *ptr_iFileReference = iFile;                                 // valid file
            return (fnGetDataFile(iDisk, iFile, &fileDetails) + ((ulSector - dataFileList[iDisk][iFile].ulStartSector) * 512)); // get a pointer to the location of the content in data flash (assuming raw data)
        }
        iFile++;
    }
    *ptr_iFileReference = -1;
    return 0;                                                            // invalid file
}


// Generate FAT sector content
//
static void fnGenerateFAT(const UTDISK *ptr_utDisk, unsigned char *ptrFAT, unsigned long ulSectorNumber)
{
    int iDisk = (ptr_utDisk->ucDriveNumber - DISK_EM_FAT);
    if ((ptr_utDisk->usDiskFlags & (DISK_FORMAT_FAT12 | DISK_FORMAT_FAT16)) == 0) { // FAT32 format
        int iFile = 0;
        unsigned long ulEntries = 0;
        unsigned long ulLimit = 0;
        int iStart = 1;
        int iLastFile = 0;
        unsigned long ulFAT_cluster = (ROOT_DIR_SECTORS + 2);
        unsigned long ulFAT_sector = (ulSectorNumber - ptr_utDisk->utFAT.ulFAT_start);
        unsigned long *ptr_ulFAT_content = (unsigned long *)ptrFAT;
        if (ulFAT_sector == 0) {                                         // first FAT sector
            *ptr_ulFAT_content++ = LITTLE_LONG_WORD(MEDIA_VALUE_FIXED);  // FAT32
            *ptr_ulFAT_content++ = LITTLE_LONG_WORD(0xffffffff);
            ulEntries = 2;
            while (++ulEntries < ulFAT_cluster) {
                *ptr_ulFAT_content++ = LITTLE_LONG_WORD(ulEntries);      // fixed root directory clusters
            }
            *ptr_ulFAT_content++ = LITTLE_LONG_WORD(CLUSTER_MASK);       // end of cluster chain
        }
        while (ulEntries < (512/sizeof(unsigned long))) {                // fill to the end of the FAT sector
            if ((ulEntries > ulLimit) || (iStart != 0)) {
                iStart = 0;
                if (iFile >= MAXIMUM_DATA_FILES) {                       // last file has been handled
                    uMemset(ptr_ulFAT_content, 0 , (((512/sizeof(unsigned long)) - ulEntries) * sizeof(unsigned long))); // fill zeroes to the end of the FAT sector
                    return;
                }
                while (iFile < MAXIMUM_DATA_FILES) {                     // find details of next file's data cluster use
                    while (dataFileList[iDisk][iFile].iFileExists <= 0) {// jump non-existent files
                        if (++iFile >= MAXIMUM_DATA_FILES) {             // end of files reached
                            uMemset(ptr_ulFAT_content, 0 , (((512/sizeof(unsigned long)) - ulEntries) * sizeof(unsigned long))); // fill zeroes to the end of the FAT sector
                            return;
                        }
                    }
                    if (dataFileList[iDisk][iFile].ulFormattedDatalength != 0) {
                        if ((ulFAT_sector >= dataFileList[iDisk][iFile].ulStartFATSector) && (ulFAT_sector <= dataFileList[iDisk][iFile].ulEndFATSector)) { // data's cluster is in this FAT sector
                            int iNextLastFile = iLastFile;
                            if (ulFAT_sector == dataFileList[iDisk][iFile].ulEndFATSector) { // ends in this sector
                                ulLimit = dataFileList[iDisk][iFile].iEndOffset;
                                iNextLastFile = iFile;
                            }
                            else {
                                ulLimit = ((512/sizeof(unsigned long)) + 1); // fill to the end of the sector
                            }
                            if (ulFAT_sector > dataFileList[iDisk][iFile].ulStartFATSector) { // if the data started in a previous sector
                                ulFAT_cluster += ((ulFAT_sector - (dataFileList[iDisk][iFile].ulStartFATSector)) * 128); // the next cluster value if the complete previous sectors were occupied
                                if ((dataFileList[iDisk][iLastFile].iFileExists > 0) && (iLastFile != iFile)) {
                                    ulFAT_cluster -= (dataFileList[iDisk][iLastFile].iEndOffset + 1); // minus the clusters in the previous sector not belonging to this file
                                }
                                else {                                   // no previous file so compensate for the root sector
                                    ulFAT_cluster -= (ROOT_DIR_SECTORS + 2);
                                }
                            }
                            iLastFile = iNextLastFile;
                            iFile++;                                     // move to the next file
                            break;
                        }
                        else {
                            ulFAT_cluster += ((dataFileList[iDisk][iFile].ulFormattedDatalength)/(ptr_utDisk->utFAT.ucSectorsPerCluster * ptr_utDisk->utFAT.usBytesPerSector));
                            ulFAT_cluster++;
                            iLastFile = iFile;
                        }
                    }
                    if (++iFile >= MAXIMUM_DATA_FILES) {                 // no more files exist
                        uMemset(ptr_ulFAT_content, 0 , (((512/sizeof(unsigned long)) - ulEntries) * sizeof(unsigned long))); // fill zeroes to the end of the FAT sector
                        return;
                    }
                }
            }
            ulFAT_cluster++;
            if (ulLimit <= ulEntries) {
                *ptr_ulFAT_content++ = LITTLE_LONG_WORD(CLUSTER_MASK);   // end of cluster chain
            }
            else {
                *ptr_ulFAT_content++ = LITTLE_LONG_WORD(ulFAT_cluster);
            }
            ulEntries++;
        }
    }
    #if defined UTFAT12
    else if ((ptr_utDisk->usDiskFlags & DISK_FORMAT_FAT12) != 0) {       // FAT12 format
        unsigned short *ptrCluster = (unsigned short *)ptrFAT;
        unsigned short *ptrClusterEnd = (unsigned short *)(ptrFAT + BYTES_PER_SECTOR);
        static unsigned long  ulFileSize = 0;
        static unsigned short uslastEntry = 0;                           // maintain values between FAT sections
        static unsigned short usCluster = 0;
        static unsigned char  ucOffset = 0;
        static int iAnswer = 0;

        unsigned long ulFAT_sector = (ulSectorNumber - ptr_utDisk->utFAT.ulFAT_start);

        if (ulFAT_sector == 0) {                                         // first FAT sector
        #if DISK_COUNT > 1
            fnGetParsFile((unsigned char *)ptr_disk_location[ucDisk], ptrFAT, (ROOT_FILE_ENTRIES * sizeof(DIR_ENTRY_STRUCTURE_FAT32))); // get the software length according to software file content
        #else
            ulFileSize = fnGetFileSize((LFN_ENTRY_STRUCTURE_FAT32 *)fnGetFlashAdd((unsigned char *)(UTASKER_APP_START))); // get the software length according to software file content
        #endif
            ptrCluster += 2;                                             // jump initial FAT info
            ucOffset = 2;
            usCluster = 4;                                               // first cluster
            iAnswer = 1;
        }
        while (ulFileSize >= (unsigned long)(BYTES_PER_SECTOR * ptr_utDisk->utFAT.ucSectorsPerCluster)) { // add single file cluster information
            if (ptrCluster >= ptrClusterEnd) {
                return UTFAT_SUCCESS;                                    // complete section filled with cluster information
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
        if (ptrCluster >= ptrClusterEnd) {
            return UTFAT_SUCCESS;                                        // no space for the final entry this time
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
        iAnswer = 0;                                                     // only report end of file cluster once
        uslastEntry = 0;                                                 // reset in case of need to repeat
        usCluster = 0;
        ucOffset = 0;
    }
    #endif
}

static int fnReadEmulatedSector(UTDISK *ptr_utDisk, unsigned long ulSectorNumber, void *ptrBuf)
{
    int iDisk = ptr_utDisk->ucDriveNumber;
    uMemset(ptrBuf, 0, BYTES_PER_SECTOR);                                // always start with blank sector content
    switch (ulSectorNumber) {
    case 0:                                                              // extended boot record
        fnCreateExtendedBootRecord(iDisk);
        break;
    case BOOT_SECTOR_LOCATION:                                           // boot sector
    case (BOOT_SECTOR_LOCATION + BACKUP_ROOT_SECTOR):                    // or backup boot sector
        fnCreateBootSector(iDisk);
        break;

    case (BOOT_SECTOR_LOCATION + 1):                                     // used by FAT32 only (info sector)
        fnCreateInfoSector(iDisk);
        break;

    default:
        iDisk -= DISK_EM_FAT;
    #if defined ROOT_DIR_SECTORS
        if ((ulSectorNumber >= ptr_utDisk->ulLogicalBaseAddress) && (ulSectorNumber < ptr_utDisk->ulLogicalBaseAddress + ROOT_DIR_SECTORS))
    #else
        if ((ulSectorNumber >= ptr_utDisk->ulLogicalBaseAddress) && (ulSectorNumber < ptr_utDisk->ulLogicalBaseAddress + ucRootSectorCount[iDisk]))
    #endif
        {                                                                // root directory sector
            int iRootSection = (ulSectorNumber - ptr_utDisk->ulLogicalBaseAddress);
    #if defined ROOT_DIR_SECTORS
            uMemcpy(ptrBuf, (((unsigned char *)&root_file[iDisk]) + (iRootSection * BYTES_PER_SECTOR)), (BYTES_PER_SECTOR)); // add root directory content
    #else
            fnPrepareRootDirectory(ptr_utDisk, iRootSection);
    #endif
        }
        else if ((ulSectorNumber >= (ptr_utDisk->utFAT.ulFAT_start)) && (ulSectorNumber < (ptr_utDisk->utFAT.ulFAT_start + ptr_utDisk->utFAT.ulFatSize))) { // FAT area
            fnGenerateFAT(ptr_utDisk, (unsigned char *)ptrBuf, ulSectorNumber);
        }
        else {                                                           // clusters
            const unsigned char *ptrSW_source;
            unsigned long ulCluster;
            unsigned long ulSector;
            int iFileReference = 0;
            int iDataPrepared = 0;
            if ((ptr_utDisk->usDiskFlags & DISK_FORMAT_FAT12) != 0) {    // FAT12
                ulCluster = (ulSectorNumber - ptr_utDisk->ulVirtualBaseAddress);
            }
            else {                                                       // FAT32
                ulCluster = (ulSectorNumber - (ptr_utDisk->ulLogicalBaseAddress - 2));
            }
    #if defined ROOT_DIR_SECTORS
            ulSector = (ulCluster * ptr_utDisk->utFAT.ucSectorsPerCluster);
    #else
            ulSector = ((ulCluster + ucRootSectorCount[iDisk]) * ptr_utDisk->utFAT.ucSectorsPerCluster);
    #endif
            ptrSW_source = fnGetDataPointer(ptr_utDisk->ucDriveNumber, ulSector, &iFileReference); // request a pointer to the data
            if (iFileReference >= 0) {                                   // if a valid file
                iDataPrepared = uDatacopy(iDisk, iFileReference, ptr_utDisk->ptrSectorData, ptrSW_source, BYTES_PER_SECTOR); // request sector data to be prepared
            }
            if (iDataPrepared < BYTES_PER_SECTOR) {                      // if the buffer was not completely filled
                uMemset((ptr_utDisk->ptrSectorData + iDataPrepared), 0, (BYTES_PER_SECTOR - iDataPrepared)); // pad with zeroes
            }
        }
        break;
    }
    return UTFAT_SUCCESS;
}

    #if !defined SDCARD_SUPPORT
extern int fnReadSector(unsigned char ucDisk, unsigned char *ptrBuffer, unsigned long ulSectorNumber)
{
    if (ptrBuffer == 0) {                                                // if a zero pointer is given read to the sector buffer
        ptrBuffer = utDisks[ucDisk].ptrSectorData;
    }
    return (fnReadEmulatedSector(&utDisks[ucDisk], ulSectorNumber, ptrBuffer));
}

extern int fnWriteSector(unsigned char ucDisk, unsigned char *ptrBuffer, unsigned long ulSectorNumber)
{
    return 0;                                                            // dummy
}
    #else
static int fnReadPartialEmulatedSector(UTDISK *ptr_utDisk, unsigned long ulSector, void *ptrBuf, unsigned short usOffset, unsigned short usLength)
{
    int iReturn;
    unsigned long ulTempSector[512/4];
    unsigned char *ptrTempBuf = (unsigned char *)&ulTempSector;
    iReturn = fnReadEmulatedSector(ptr_utDisk, ulSector, ptrTempBuf);
    uMemcpy(ptrBuf, (ptrTempBuf + usOffset), usLength);
    return iReturn;
}
    #endif

// This routine sets a time and data to a data file object - it uses a fixed time stamp if no date/time information is present
//
static void fnSetEmulatedDetails(DIR_ENTRY_STRUCTURE_FAT32 *ptrEntry, EMULATED_FILE_DETAILS *ptrFileDetails)
{
    #define CREATION_HOURS         12                                    // fixed date/time stamp used if no other information is available
    #define CREATION_MINUTES       00
    #define CREATION_SECONDS       00

    #define CREATION_DAY_OF_MONTH  26
    #define CREATION_MONTH_OF_YEAR 10
    #define CREATION_YEAR          (2015 - 1980)

    unsigned short usCreationTime;
    unsigned short usCreationDate;
    #if defined EMULATED_FAT_FILE_DATE_CONTROL
    if (ptrFileDetails->usCreationDate == 0) {                           // if no details have been set we use default values
    #endif
        usCreationTime = (CREATION_SECONDS | (CREATION_MINUTES << 5) | (CREATION_HOURS << 11)); // fixed time stamp
        usCreationDate = (CREATION_DAY_OF_MONTH | (CREATION_MONTH_OF_YEAR << 5) | (CREATION_YEAR << 9));
    #if defined EMULATED_FAT_FILE_DATE_CONTROL
    }
    else {
        usCreationTime = ptrFileDetails->usCreationTime;
        usCreationDate = ptrFileDetails->usCreationDate;
    }
    #endif
    ptrEntry->DIR_WrtTime[0] = (unsigned char)(usCreationTime);
    ptrEntry->DIR_WrtTime[1] = (unsigned char)(usCreationTime >> 8);
    ptrEntry->DIR_LstAccDate[0] = ptrEntry->DIR_WrtDate[0] = (unsigned char)(usCreationDate);
    ptrEntry->DIR_LstAccDate[1] = ptrEntry->DIR_WrtDate[1] = (unsigned char)(usCreationDate >> 8);
    ptrEntry->DIR_CrtTime[0] = ptrEntry->DIR_WrtTime[0];
    ptrEntry->DIR_CrtTime[1] = ptrEntry->DIR_WrtTime[1];
    ptrEntry->DIR_CrtDate[0] = ptrEntry->DIR_LstAccDate[0];
    ptrEntry->DIR_CrtDate[1] = ptrEntry->DIR_LstAccDate[1];
    ptrEntry->DIR_FileSize[0] = (unsigned char)(ptrFileDetails->ulFileLength); // enter the file size
    ptrEntry->DIR_FileSize[1] = (unsigned char)(ptrFileDetails->ulFileLength >> 8);
    ptrEntry->DIR_FileSize[2] = (unsigned char)(ptrFileDetails->ulFileLength >> 16);
    ptrEntry->DIR_FileSize[3] = (unsigned char)(ptrFileDetails->ulFileLength >> 24);
}

extern void fnPrepareRootDirectory(const UTDISK *ptr_utDisk, int iRootSection)
{
    if ((ptr_utDisk->usDiskFlags & DISK_FAT_EMULATION) != 0) {           // ignore if the disk is not emulating FAT
        // Prime the root directory with as many files as required
        //
        int iDisk = (ptr_utDisk->ucDriveNumber - DISK_EM_FAT);
        EMULATED_FILE_DETAILS fileDetails;
        int iFileRef = 0;
        int iRootFile = 0;
        CHAR fileName[11] = {'D', 'A', 'T', 'A', '_', '0', '0', '0', 'B', 'I', 'N'}; // default short file name if no other is supplied
    #if defined ROOT_DIR_SECTORS
        unsigned long ulCluster = (2 + ROOT_DIR_SECTORS);                // first cluster used for first file content
        DIR_ENTRY_STRUCTURE_FAT32 *ptrRoot = root_file[iDisk];
    #else
        int iThisRootSection = 0;
        unsigned long ulCluster = (2 + ucRootSectorCount[iDisk]);        // first cluster used for first file content - temporary when ucRootSectorCount is 0
        DIR_ENTRY_STRUCTURE_FAT32 *ptrRoot = (DIR_ENTRY_STRUCTURE_FAT32 *)ptr_utDisk->ptrSectorData;
        if (ucRootSectorCount[iDisk] == 0) {                             // if the root sector size is not yet know we assume that it is 1
            ulCluster++;                                                 // add single root sector (it will be corrected using a recursive call if not correct)
        }
    #endif
        uMemset(&fileDetails, 0, sizeof(fileDetails));                   // reset the structure before first use
        while (iFileRef <= MAXIMUM_DATA_FILES) {
            if (iRootFile == 0) {                                        // first entry is a volume name
                ptrRoot->DIR_Attr = DIR_ATTR_VOLUME_ID;                  // volume
                uMemcpy(ptrRoot->DIR_Name, "DATADISK  ", 11);            // volume name
                ptrRoot->DIR_Name[9] = (ptr_utDisk->ucDriveNumber + '1');
            }
            else {                                                       // user file
    #if defined EMULATED_FAT_FILE_NAME_CONTROL
                if (fileDetails.ptrFileName != 0) {                      // if the user has specified a name
                    CHAR cDirectoryName[12];
                    OPEN_FILE_BLOCK openFileBlock;
                    if (fileDetails.ucValid & 0x02) {                    // the string is already fat formatted
                        DIR_ENTRY_STRUCTURE_FAT32 *ptrObject = (DIR_ENTRY_STRUCTURE_FAT32 *)fileDetails.ptrFileName;
                        while (ptrObject->DIR_Attr == DIR_ATTR_LONG_NAME) { // add any leading long file names
                            uMemcpy(ptrRoot++, ptrObject++, sizeof(DIR_ENTRY_STRUCTURE_FAT32)); // use the complete LFN object
                            iRootFile++;
                        }
                        uMemcpy(ptrRoot->DIR_Name, ptrObject->DIR_Name, sizeof(ptrRoot->DIR_Name)); // add the short file name
                      //iRootFile++;
                        goto _add_length;                                // skip the string input handling
                    }
                    openFileBlock.ptrLocalDirPath = fileDetails.ptrFileName;
                    openFileBlock.ptr_utDisk = (UTDISK *)ptr_utDisk;
                    if ((openFileBlock.iQualifiedPathType = fnCreateNameParagraph(&fileDetails.ptrFileName, cDirectoryName)) < 0) {
                        // Invalid file name
                        //
                        _EXCEPTION("File name is invalid!!");
                        ptrRoot->DIR_NTRes = 0x18;
                        ptrRoot->DIR_Attr = DIR_ATTR_ARCHIVE;            // file
                        uMemcpy(ptrRoot->DIR_Name, fileName, sizeof(fileName)); // fixed short default file name
                    }
        #if defined FAT_EMULATION_LFN
                    else if (openFileBlock.iQualifiedPathType >= FULLY_QUALIFIED_LONG_NAME_SFNM) { // the new file requires a long file name and cannot use a short one
            #if !defined ROOT_DIR_SECTORS
                        int iOriginalOffset = (iRootFile % (BYTES_PER_SECTOR/sizeof(DIR_ENTRY_STRUCTURE_FAT32)));
                        openFileBlock.iRootDirectory = iOriginalOffset;
                        openFileBlock.iContinue = (iRootSection != iThisRootSection); // we are in the required root sector
            #else
                        openFileBlock.iRootDirectory = 0;
            #endif
                        openFileBlock.ptrDiskLocation = (DISK_LOCATION *)ptrRoot;
                        fnInsertLFN_name(&openFileBlock, 0, 0);          // insert long file name
                        ((UTDISK *)ptr_utDisk)->usDiskFlags &= ~(WRITEBACK_BUFFER_FLAG); // we don't write back to emulated disk after inserting file name
            #if !defined ROOT_DIR_SECTORS
                        if (openFileBlock.iRootDirectory >= iOriginalOffset) {
                            openFileBlock.iRootDirectory -= iOriginalOffset;
                        }
                        else {                                           // the insertion of the long file name caused the present sector to be filled
                            openFileBlock.iRootDirectory += (((BYTES_PER_SECTOR/sizeof(DIR_ENTRY_STRUCTURE_FAT32)) - 1) - iOriginalOffset);
                            if (iRootSection == iThisRootSection) {      // if this is the sector to be filled
                                return;                                  // complete
                            }
                            ptrRoot = (DIR_ENTRY_STRUCTURE_FAT32 *)ptr_utDisk->ptrSectorData;
                            iThisRootSection++;
                        }
            #endif
                        iRootFile += openFileBlock.iRootDirectory;       // the number of root entries that were required
                        ptrRoot += openFileBlock.iRootDirectory;         // the number of root entries that were required
                        uMemcpy(ptrRoot->DIR_Name, openFileBlock.cShortFileName, sizeof(openFileBlock.cShortFileName)); // add the short file name
                    }
        #endif
                    else {                                               // short file name
                        uMemcpy(ptrRoot->DIR_Name, cDirectoryName, sizeof(ptrRoot->DIR_Name)); // short file name as defined by the user
                    }
                }
                else {
    #endif
                    uMemcpy(ptrRoot->DIR_Name, fileName, sizeof(fileName)); // fixed short default file name
    #if defined EMULATED_FAT_FILE_NAME_CONTROL
                }
    #endif
_add_length:
                ptrRoot->DIR_NTRes = 0x18;
                ptrRoot->DIR_Attr = DIR_ATTR_ARCHIVE;                    // file
                if (fileDetails.ulFileLength != 0) {
                    ptrRoot->DIR_FstClusHI[1] = (unsigned char)(ulCluster >> 24);
                    ptrRoot->DIR_FstClusHI[0] = (unsigned char)(ulCluster >> 16);
                    ptrRoot->DIR_FstClusLO[1] = (unsigned char)(ulCluster >> 8);
                    ptrRoot->DIR_FstClusLO[0] = (unsigned char)ulCluster;
                    ulCluster += (fileDetails.ulFileLength/(ptr_utDisk->utFAT.ucSectorsPerCluster * ptr_utDisk->utFAT.usBytesPerSector)); // reserve clusters required by this file's length
                    ulCluster++;
                }
            }
            fnSetEmulatedDetails(ptrRoot, &fileDetails);                 // add file's creation date/time details
            do {
                if (iFileRef >= MAXIMUM_DATA_FILES) {                    // last file was entered
                    iFileRef = (MAXIMUM_DATA_FILES + 1);
                    break;
                }
                if (fileName[7] == '9') {                                // increment file name (supports up to 99 files)
                    fileName[6]++;
                    fileName[7] = '0';
                }
                else {
                    fileName[7]++;
                }
                uMemset(&fileDetails, 0, sizeof(fileDetails));           // reset the structure before each use
                fnGetDataFile(iDisk, iFileRef, &fileDetails);            // get next file's details (after any formatting)
                if (fileDetails.ucValid != 0) {                          // if the file is to be displayed
                    ptrRoot++;
    #if !defined ROOT_DIR_SECTORS
                    if ((unsigned char *)ptrRoot == (ptr_utDisk->ptrSectorData + 512)) { // if a complete sector has just been filled
                        if (iThisRootSection == iRootSection) {          // the sector that was required
                            return;                                      // complete
                        }
                    }
    #endif
                    dataFileList[iDisk][iFileRef].iFileExists = 1;
                    dataFileList[iDisk][iFileRef].ulFormattedDatalength = fileDetails.ulFileLength; // content length (formatted)
                    dataFileList[iDisk][iFileRef].ulStartSector = ulCluster; // sector that the files's cluster begins in
                    dataFileList[iDisk][iFileRef].ulStartFATSector = (dataFileList[iDisk][iFileRef].ulStartSector/128);
                    dataFileList[iDisk][iFileRef].ulEndSector = ((UTDISK *)ptr_utDisk)->utFileInfo.ulNextFreeCluster = (ulCluster + (fileDetails.ulFileLength/(ptr_utDisk->utFAT.ucSectorsPerCluster * ptr_utDisk->utFAT.usBytesPerSector))); // sector that the file's cluster ends in
                    dataFileList[iDisk][iFileRef].ulEndFATSector = (dataFileList[iDisk][iFileRef].ulEndSector/128);
                    if (fileDetails.ulFileLength == 0) {                 // a file without any data doesn't occupy a cluster so compensate
                        dataFileList[iDisk][iFileRef].iEndOffset = ((dataFileList[iDisk][iFileRef].ulEndSector - 1) % 128);
                    }
                    else {
                        dataFileList[iDisk][iFileRef].iEndOffset = (dataFileList[iDisk][iFileRef].ulEndSector % 128);
                    }
                    iRootFile++;
                    iFileRef++;
                    break;
                }
            } while (iFileRef++ < MAXIMUM_DATA_FILES);
        }
    #if !defined ROOT_DIR_SECTORS
        if (ucRootSectorCount[iDisk] == 0) {                             // first call
            ucRootSectorCount[iDisk] = (iThisRootSection + 1);
            if (ucRootSectorCount[iDisk] > 1) {
                fnPrepareRootDirectory(ptr_utDisk, -1);                   // recursive call to use the correct cluster values now that the size of the root sector is known
            }
        }
        else if (iRootSection != -1) {
            int iRemaining = (512 - ((unsigned char *)(++ptrRoot) - ptr_utDisk->ptrSectorData)); // the remaining space in the sector that needs to be filled with zeros
            if (iRemaining != 0) {
                uMemset(ptrRoot, 0, iRemaining);
            }
        }
    #endif
    #if defined _WINDOWS                                                 // test FAT emulation algorithm
        {
            unsigned long ulSectorNumber;
            for (ulSectorNumber = 0; ulSectorNumber < 100; ulSectorNumber++) {
                fnGenerateFAT(ptr_utDisk, ptr_utDisk->ptrSectorData, (ulSectorNumber + ptr_utDisk->utFAT.ulFAT_start));
            }
        }
    #endif
    }
}

// Emulated disk
//
    #if !defined SDCARD_SUPPORT
extern const UTDISK *fnGetDiskInfo(unsigned char ucDisk)
{
    static unsigned long ulSectorMemory[2 + (BYTES_PER_SECTOR / sizeof(unsigned long))] = {0}; // long-word aligned buffer shared by all disks (used by only one at a time)
    UTDISK *ptr_utDisk = &utDisks[ucDisk];
    ptr_utDisk->ucDriveNumber = ucDisk;
    ptr_utDisk->utFAT.usBytesPerSector = BYTES_PER_SECTOR;               // prime some information
    ptr_utDisk->ulSD_sectors = (EMULATED_FAT_DISK_SIZE/BYTES_PER_SECTOR);// the number of sector that the disk has
    ptr_utDisk->usDiskFlags = (DISK_MOUNTED | DISK_FORMATTED | DISK_FAT_EMULATION | WRITE_PROTECTED_SD_CARD); // the disk is assumed to be mounted and formatted - with write protection set
    ptr_utDisk->ptrSectorData = (unsigned char *)ulSectorMemory;         // define sector memory for working with (it is long word aligned and shared by disks since its content doesn't not need to be preserved)
    fnReadSector(ucDisk, 0, BOOT_SECTOR_LOCATION);                       // ensure FAT information is known
    #if defined FAT_EMULATION && defined ROOT_DIR_SECTORS
    ptr_utDisk->rootBuffer = (DIR_ENTRY_STRUCTURE_FAT32 *)&root_file[ucDisk];
    #endif
    return (ptr_utDisk);
}
    #endif
#endif                                                                   // endif FAT_EMULATION
