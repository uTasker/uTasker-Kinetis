/***********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher

    ---------------------------------------------------------------------
    File:      uFile.c
    Project:   Single Chip Embedded Internet
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2016
    *********************************************************************
    01.03.2007 Optional uCompareFile() added
    19.05.2007 Add FLASH unprotect/protect support for devices which require or support it {1}
    22.05.2007 Add Sub-file support for devices with only large FLASH granularity {2}
    11.08.2007 SPI FLASH support                                         {3}
    23.09.2007 Modify uFileErase() to work with new version of fnEraseFlashSector() {4}
    17.11.2007 Add address rounding macro/function to support blocks of non-power of 2 size {5}
    09.12.2007 Allow sub-file systems with differing FLASH granularity and FLASH_LINE_SIZE  {6}
    16.01.2008 Corrections for file systems where SINGLE_FILE_SIZE is a multiple of FILE_GRANULARITY {7}
    17.01.2008 Add dependency on parameter file system for calls fnSetPar(), fnGetPar and fnDelPar {8}
    29.03.2008 Pass file pointer when closing an open line buffer        {9}
    09.06.2008 Allow external SPI FLASH to operate with sub-files        {10}
    08.09.2008 Correct limitation of writes too large for the file system and optimise the data size written in this case {11}
    14.09.2008 Enable parameter and flash routines when parameter system is enabled without file system support {12}
    21.01.2009 Allow user to specify internal files                      {13}
    08.05.2009 Add _NO_FILE_INTERFACE to aid code size optimisation      {14}
    08.10.2009 Add simulation support for embedded user files in file system {15}
    15.10.2009 Export simulation flag for use by FTP                     {16}
    14.11.2009 Add USE_PARAMETER_AREA support (as simple alternative to USE_PARAMETER_BLOCK) {17}
    06.12.2009 Close line buffer always when small row length is used    {18}
    21.01.2010 Extract file content from a user file without uFileSystem header {19}
    11.10.2010 Only set user_files_not_in_code when the user file table is not in RAM {20}
    29.10.2010 Correct write to intermediate line buffer when writing multiple short blocks {21}
    30.10.2010 Rename fnFilterUserFile() to uOpenUserFile() and make external {22}
    30.10.2010 Increase maximum user file name string from 32 to 64 (allow path extensions to fit) {23}
    10.11.2010 Add MANAGED_FILES compatibility                           {24}
    24.08.2011 Add extended uFileSystem file name handling               {25}
    07.10.2011 Add UFILESYSTEM_APPEND support                            {26}
    16.10.2011 Add support for appending with flash line buffer          {27}
    28.10.2011 FILE_GRANULARITY is used exclusively (SINGLE_FILE_SIZE is no longer used)
    21.12.2011 Adjust file system dimensions when using extended files without EXTENSION_FILE_COUNT_VARIABLE {28}
    22.12.2011 Correct extended file name buffer length                  {29}
    08.01.2012 Call writes to close line buffers with the managed files compatible routine [_fnWriteBytesFlash() instead of fnWriteBytesFlash()] {30}
    10.04.2012 Allow writing to end of sub-file file system when the file system ends with small sectors {31}
    04.06.2013 Added UFILE_MALLOC() default                              {32}
    04.10.2014 Added error when the extended file system is used but the standard uFileSystem file block quantity is larger than 61 (0,1..A,..Z, a..y) so that the extended area starts at 'z' {33}

*/

/* =================================================================== */
/*                           include files                             */
/* =================================================================== */

#include "config.h"


/* =================================================================== */
/*                          local definitions                          */
/* =================================================================== */

#if !defined UFILE_MALLOC                                                // {32}
    #define UFILE_MALLOC(x)      uMalloc((MAX_MALLOC)(x))
#endif

#if defined ACTIVE_FILE_SYSTEM
    #define _FILE_GRANULARITY    FILE_GRANULARITY                        // {7}

    #if defined SUB_FILE_SIZE                                            // {2}
        #define SUB_FILE  , unsigned char ucSubfile
        #define SUB_FILE_OFF  ,0
    #else
        #define SUB_FILE
        #define SUB_FILE_OFF
    #endif

    #ifndef CAST_EXTENDED_POINTER_ARITHMETIC
        #define CAST_EXTENDED_POINTER_ARITHMETIC CAST_POINTER_ARITHMETIC
    #endif
#else
  //#define SUB_FILE
  //#define SUB_FILE_OFF
#endif

#if defined EXTENSION_FILE_COUNT_VARIABLE
    #define _EXTENSION_FILE_COUNT     iExtensionFileCount
    #define _LAST_FILE_BLOCK          (LAST_FILE_BLOCK + iExtensionFileCount)
#else
    #if defined EXTENDED_UFILESYSTEM
        #define _EXTENSION_FILE_COUNT (EXTENDED_FILE_SYSTEM_SIZE/EXTENSION_FILE_SIZE) // {28} the number of file blocks in the extended area
        #define _LAST_FILE_BLOCK      (LAST_FILE_BLOCK + _EXTENSION_FILE_COUNT) // {28} total file block count in the uFileSystem and extended areas together
        #if LAST_FILE_BLOCK > 61                                         // {33} note - if this line generates an error remove any casting used in the definition of LAST_FILE_BLOCK 
            #error "When using the extended uFileSystem the standard file system should be dimensioned to be no larger than 61 blocks in size otherwise the extended part cannot be  addressed (please redimension accordingly)"
        #endif
    #else
        #define _EXTENSION_FILE_COUNT  EXTENSION_FILE_COUNT
        #define _LAST_FILE_BLOCK       LAST_FILE_BLOCK
    #endif
#endif

#if defined EXTENSION_FILE_COUNT_VARIABLE                                // end of the file system can be modified
    static unsigned char *_uFILE_SYSTEM_END = uFILE_SYSTEM_END;          // set initially to the end of the internal Flash area
    extern void fnExtendFileSystem(unsigned long ulExtendedSpace)
    {
        _uFILE_SYSTEM_END += ulExtendedSpace;                            // add extra length
        iExtensionFileCount += (ulExtendedSpace / EXTENSION_FILE_SIZE);  // add extra file spaces
    }
#else
    #if defined EXTENDED_UFILESYSTEM                                     // {28}
        #define _uFILE_SYSTEM_END (uFILE_SYSTEM_END + EXTENDED_FILE_SYSTEM_SIZE)
    #else
        #define _uFILE_SYSTEM_END uFILE_SYSTEM_END
    #endif
#endif

/* =================================================================== */
/*                      local structure definitions                    */
/* =================================================================== */

/* =================================================================== */
/*                 local function prototype declarations               */
/* =================================================================== */

/* =================================================================== */
/*                             constants                               */
/* =================================================================== */

/* =================================================================== */
/*                      local variable definitions                     */
/* =================================================================== */

#if defined INTERNAL_USER_FILES                                          // {13}
    #if defined USE_FTP
    USER_FILE *ptrUserFiles = 0;                                         // global when FTP is active
    #else
    static USER_FILE *ptrUserFiles = 0;
    #endif
#endif
#if defined ACTIVE_FILE_SYSTEM && !defined _NO_FILE_INTERFACE            // {14}
    static MEMORY_RANGE_POINTER FileInProgress;
    static MAX_FILE_LENGTH LengthInProgress = 0;
#endif
#if defined UFILESYSTEM_APPEND                                           // {26}
    static unsigned char *ptrAppendFlash = 0;
    static MAX_FILE_LENGTH append_length = 0;
    #if defined FLASH_LINE_SIZE && FLASH_LINE_SIZE > 0                   // {27}
        static unsigned char *ptrAppendLine = 0;
        static unsigned char *ptrAppendSector = 0;
    #endif
#endif
#if defined EXTENSION_FILE_COUNT_VARIABLE
    static int iExtensionFileCount = 0;                                  // extension file size - initially zero until known
#endif

#if defined MANAGED_FILES                                                // {24}
// When managed files are in operation the FLASH driver will return MEDIA_BUSY if writes can not yet be performed.
// This module waits for the write to complete by recalling if necessary
//
static int _fnWriteBytesFlash(unsigned char *ucDestination, unsigned char *ucData, MAX_FILE_LENGTH Length)
{
    int iRtn;

    do {
        iRtn = fnWriteBytesFlash(ucDestination, ucData, Length);
    } while (iRtn == MEDIA_BUSY);
    return iRtn;
}
#else
    #define _fnWriteBytesFlash  fnWriteBytesFlash
#endif

#if defined ACTIVE_FILE_SYSTEM
    #if (SPI_FLASH_PAGE_LENGTH != 256 && SPI_FLASH_PAGE_LENGTH != 512) && (defined SPI_FILE_SYSTEM && defined FLASH_FILE_SYSTEM)
        static void fnGetLowerBoundary(MAX_FILE_LENGTH *length, MAX_FILE_LENGTH granularity)
        {
            MAX_FILE_LENGTH lower_boundary = granularity;
            while (lower_boundary <= *length) {
                lower_boundary += granularity;
            }
            *length = lower_boundary - granularity;
        }

        #define ROUNDDOWN_BOUNDARY(length, GRANULARITY) fnGetLowerBoundary(&length, GRANULARITY);
        #define ROUNDUP_BOUNDARY(length, GRANULARITY)   fnGetLowerBoundary(&length, GRANULARITY); length += GRANULARITY  // {5}
    #else
        #define ROUNDUP_BOUNDARY(length, GRANULARITY)  length &= ~(GRANULARITY-1); length += GRANULARITY  // {5}
        #define ROUNDDOWN_BOUNDARY(length, GRANULARITY) length &= ~(GRANULARITY-1);                       // {5}
    #endif
#endif                                                                   // {12}

#if defined USE_PARAMETER_BLOCK                                          // {8}
// Called to set a byte or block of bytes to a parameter block
//
extern int fnSetPar(unsigned short usParameterReference, unsigned char *ucValue, unsigned short usLength)
{
    #if defined USE_PAR_SWAP_BLOCK
    unsigned char ucValidPars = fnGetValidPars(PRESENT_VALID_BLOCK);     // get the valid parameter block

    if (ucValidPars == 0) {                                              // is there a valid block ?
        if ((ucValidPars = fnGetValidPars(PRESENT_TEMP_BLOCK)) == 0) {   // see whether we have a temporary block
            ucValidPars = (1 | BLOCK_INVALID);                           // if we have no valid blocks we set a first block but also mark it as invalid (this causes all parameters to be deleted to ensure fresh)
        }
        ucValidPars ^= 0x3;                                              // swap blocks
    }

    if (usParameterReference & TEMPORARY_PARAM_SET) {
        ucValidPars |= TEMP_PARS;
        ucValidPars ^= 0x3;                                              // swap blocks
    }
    #else
    unsigned char ucValidPars = (1 | TEMP_PARS);                         // when no swap block we must delete and validate new ones before and after saving
    #endif
    usParameterReference &= ~TEMPORARY_PARAM_SET;

    return (fnSetParameters(ucValidPars, usParameterReference, ucValue, usLength)); // save data in the hardware
}

// Called to retrieve a byte or a block of bytes from the present parameter block
//
extern int fnGetPar(unsigned short usParameterReference, unsigned char *ucValue, unsigned short usLength)
{
    unsigned char ucValidPars;
    unsigned char ucValidSet = PRESENT_VALID_BLOCK;

    if (usParameterReference & TEMPORARY_PARAM_SET) {
        usParameterReference &= ~TEMPORARY_PARAM_SET;
        ucValidSet = PRESENT_TEMP_BLOCK;
    }

    if ((ucValidPars = fnGetValidPars(ucValidSet)) == 0) {
        return -1;                                                       // return if we have no valid parameter block
    }

    return (fnGetParameters(ucValidPars, usParameterReference, ucValue, usLength)); // get data from the hardware
}

// Called to delete a block in the parameter block or to manipulate parameter block itself
//
extern int fnDelPar(unsigned char ucDeleteType)
{
    unsigned char ucValidPars;

    switch (ucDeleteType) {
    case INVALIDATE_PARAMETER_BLOCK:
        ucValidPars = PRESENT_VALID_BLOCK;                               // work with valid block
        break;                                                           // delete and invalidate parameter block

    case INVALIDATE_TEST_PARAMETER_BLOCK:
        ucValidPars = PRESENT_TEMP_BLOCK;                                // work with temp. block
        break;                                                           // delete and invalidate temporary parameter block

    case SWAP_PARAMETER_BLOCK:
        if ((ucValidPars = fnGetValidPars(PRESENT_TEMP_BLOCK)) == 0) {
            return -1;                                                   // return if we have no temp valid parameter block
        }
        return (fnSetParameters((unsigned char)(ucValidPars | TEMP_PARS), 0, 0, 0)); // delete and invalidate parameter block and swap to new parameter block

    default:                                                             // unknow command
        return -1;
    }

    if ((ucValidPars = fnGetValidPars(ucValidPars)) == 0) {
        return -1;                                                       // return if we have no (temp) valid parameter block
    }
    return (fnSetParameters(ucValidPars, 1, 0, 0));                      // delete and invalidate (temp) parameter block
}
#elif defined USE_PARAMETER_AREA                                         // {17}

static unsigned char *fnGetValidParArea(unsigned short usLength, unsigned char **ppNext)
{
    unsigned char *pucArea = (unsigned char *)PARAMETER_BLOCK_START;     // the start of the parameter area
    unsigned short usRoundedLength = usLength;
    unsigned char *pucNextBlock = pucArea;                               // the start of the possible following parameter block
    #if defined FLASH_LINE_SIZE && FLASH_LINE_SIZE > 0
    usRoundedLength = (((usLength + (FLASH_LINE_SIZE - 1))/FLASH_LINE_SIZE) * FLASH_LINE_SIZE); // round the length up if necessary
    #endif
    pucNextBlock += usRoundedLength;
    while (pucNextBlock < (unsigned char *)(PARAMETER_BLOCK_START + PAR_BLOCK_SIZE - usRoundedLength)) { // while within the parameter area and enough room for a further block of parameters
        if (*(fnGetFlashAdd(pucNextBlock)) == 0xff) {                    // blank area found
            break;
        }
        pucArea += usRoundedLength;                                      // jump old parameters
        pucNextBlock += usRoundedLength;
    }
    if (ppNext != 0) {
        if ((*fnGetFlashAdd(pucArea) == 0xff) || (pucNextBlock >= (unsigned char *)(PARAMETER_BLOCK_START + PAR_BLOCK_SIZE - usRoundedLength))) {
            pucNextBlock = pucArea;                                      // empty or no further room so next block is same a present block
        }
        *ppNext = pucNextBlock;
    }
    return pucArea;                                                      // most up-to-data parameters
}

// Called to delete a block in the parameter block or to manipulate parameter block itself
//
extern int fnDelPar(unsigned char ucDeleteType)
{
    return (fnEraseFlashSector((unsigned char *)PARAMETER_BLOCK_START, PAR_BLOCK_SIZE));
}

// Called to retrieve the valid block of parameters from the parameter area
//
extern int fnGetPar(unsigned short usParameterReference, unsigned char *ucValue, unsigned short usLength)
{
    unsigned char *ucArea = (unsigned char *)PARAMETER_BLOCK_START;      // the start of the parameter area
    if (*(fnGetFlashAdd(ucArea)) == 0xff) {                              // empty
        return 1;
    }
    else if (*(fnGetFlashAdd(ucArea)) != (unsigned char)usParameterReference) { // incorrect version
        fnEraseFlashSector(ucArea, usLength);                            // delete invalid area
        return 1;
    }
    ucArea = fnGetValidParArea(usLength, 0);                             // the most up-to-data parameters available
    uMemcpy(ucValue, fnGetFlashAdd(ucArea), usLength);                   // retrieve the valid parameters
    return 0;
}

// Called to set a byte or block of bytes to a parameter block
//
extern int fnSetPar(unsigned short usParameterReference, unsigned char *ucValue, unsigned short usLength)
{
    unsigned char *ptrNextArea;
    unsigned char *ptrPresentArea;
    ptrPresentArea = fnGetValidParArea(usLength, &ptrNextArea);
    if (ptrPresentArea == ptrNextArea) {                                 // present block and last block are the same
        if (ptrPresentArea != (unsigned char *)PARAMETER_BLOCK_START) {  // block full - we have no choice but to delete it and set new content to the beginning
            fnDelPar(0);
            ptrNextArea = (unsigned char *)PARAMETER_BLOCK_START;
        }
    }
    _fnWriteBytesFlash(ptrNextArea, ucValue, usLength);
    #if defined FLASH_LINE_SIZE && FLASH_LINE_SIZE > 0
    _fnWriteBytesFlash(ptrNextArea, 0, 0);                               // {30} close any outstanding FLASH buffer
    #endif
    return 0;
}
#endif                                                                   // end USE_PARAMETER_BLOCK

#if defined ACTIVE_FILE_SYSTEM                                           // {12}
    #if defined NO_ACCUMULATIVE_WORDS
    static unsigned char ucStartByte;
    static unsigned char ucEndByte;
    static unsigned char ucEndByteValid;
    #elif !defined _NO_FILE_INTERFACE && (defined FLASH_LINE_SIZE && FLASH_LINE_SIZE > 0) // {6}
    static unsigned char ucFirstLine[FLASH_LINE_SIZE];                   // space for collecting first FLASH line of a file
    #endif

    #if !defined _NO_FILE_INTERFACE                                      // {14}
// Converts a file name to its file offset in the uFileSystem
//
extern unsigned char fnConvertName(CHAR cName)
{
    if (cName < 'A') {                                                   // 0...9
       cName -= '0';
    }
    else if (cName < 'a') {                                              // A..Z
        cName -= ('A' - 10);
    }
    else {
       cName -= ('a' - 36);                                              // a..z
    }
    return (unsigned char)cName;
}

// Converts a file offset in the uFileSystem to a file name
//
static CHAR fnConvertToName(unsigned char ucNumber)
{
    if (ucNumber <= 9) {
      ucNumber += '0';
    }
    else if (ucNumber < 36) {
      ucNumber += ('A' - 10);
    }
    else {
      ucNumber += ('a' - 36);
    }
    return (CHAR)ucNumber;
}

#if defined EXTENDED_UFILESYSTEM || defined _WINDOWS
extern unsigned char *fnGetEndOf_uFileSystem(void)
{
    return _uFILE_SYSTEM_END;
}
#endif

#if defined EXTENDED_UFILESYSTEM                                         // {25}
// Convert the extension to a pointer in the extended uFileSystem
//
static MAX_FILE_SYSTEM_OFFSET fnGetExtendedFilePointer(CHAR *ucIp_Data)
{
    int iNameExtension = EXTENDED_UFILESYSTEM;
    int iExtension = 0;
    MAX_FILE_SYSTEM_OFFSET file_location = (LAST_FILE_BLOCK * FILE_GRANULARITY);
    while (iNameExtension--) {
        if ((*ucIp_Data >= '0') && (*ucIp_Data <= '9')) {
            iExtension = ((iExtension * 8) + (iExtension * 2));          // multiply by 10
            iExtension += (*ucIp_Data - '0');
        }
        else {
            break;
        }
        ucIp_Data++;
    }
    if (iExtension >= _EXTENSION_FILE_COUNT) {                           // limit to size of extension memory
        iExtension = (_EXTENSION_FILE_COUNT - 1);
    }
    file_location += (iExtension * EXTENSION_FILE_SIZE);
    return (file_location);
}

static void fnConvertToExtendedName(MEMORY_RANGE_POINTER ExternalFile, CHAR *file_name)
{
    unsigned long ulExtension;
    int iExtLen;
    CHAR *ptrFileName = (file_name + (EXTENDED_UFILESYSTEM + 1));
    CHAR cOriginalName[6];                                               // {29}
    *file_name++ = 'z';                                                  // {33} set extended file name
    uMemcpy(cOriginalName, file_name, sizeof(cOriginalName));            // backup the original name (minus the first letter which is set to 'Z')
    ExternalFile -= (CAST_POINTER_ARITHMETIC)((CAST_POINTER_ARITHMETIC)uFILE_SYSTEM_START + (LAST_FILE_BLOCK * FILE_GRANULARITY));
    ulExtension = ((CAST_POINTER_ARITHMETIC)ExternalFile / EXTENSION_FILE_SIZE);
    if (ulExtension >= (unsigned long)_EXTENSION_FILE_COUNT) {
        ulExtension = (unsigned long)(_EXTENSION_FILE_COUNT - 1);
    }
  //fnBufferDec(ulExtension, 0, file_name);
    iExtLen = (fnBufferDec(ulExtension, 0, file_name) - file_name);
    if (iExtLen < EXTENDED_UFILESYSTEM) {
        while (iExtLen < EXTENDED_UFILESYSTEM) {
            *file_name++ = '0';
            iExtLen++;
        }
        fnBufferDec(ulExtension, 0, file_name);
    }
    uMemcpy(ptrFileName, cOriginalName, sizeof(cOriginalName));
}
#endif

// Simple one number / letter file names
//
static MAX_FILE_SYSTEM_OFFSET fnGetFilePointer(CHAR *ucIp_Data)
{
    MAX_FILE_SYSTEM_OFFSET FileLocation;
    #if defined SPI_SW_UPLOAD                                            // {3}
    if (SW_UPLOAD_FILE()) {
        return (MAX_FILE_SYSTEM_OFFSET)(FILE_SYSTEM_SIZE/* - PAR_BLOCK_SIZE*/); // special location for external SPI FLASH {10} [remove PAR_BLOCK_SIZE]
    }
    #endif
    #if defined USER_SPECIFIED_FILE_HANDLING
    USER_SPECIFIED_FILE_HANDLING;
    #endif
    FileLocation = fnConvertName(*ucIp_Data);                            // first letter/number signals which block it should be put in
        #if defined SUB_FILE_SIZE                                        // {2}
        #define LAST_FULL_SECTOR   ((FILE_SYSTEM_SIZE + FILE_GRANULARITY - SUB_FILE_SIZE)/FILE_GRANULARITY) // {6}{10} [use FILE_GRANULARITY for flexibility]
    if (FileLocation >= LAST_FULL_SECTOR) {
        FileLocation -= LAST_FULL_SECTOR;
        if (FileLocation >= LAST_SUB_FILE_BLOCK) {                       // limit to physical range - if outside we set to last...
            FileLocation = (LAST_SUB_FILE_BLOCK - 1);
        }
        return (FileLocation * SUB_FILE_SIZE);                           // return an offset from start of file
    }
        #endif

    if (FileLocation >= LAST_FILE_BLOCK) {                               // limit to physical range - if outside we set to last...
        #if defined EXTENDED_UFILESYSTEM                                 // {25}
        return fnGetExtendedFilePointer(ucIp_Data + 1);                  // extended uFileSystem follows the standard uFileSystem
        #else
        FileLocation = (LAST_FILE_BLOCK - 1);
        #endif
    }
    return (FileLocation * FILE_GRANULARITY);                            // return an offset from start of file
}
    #endif                                                               // end #if !defined _NO_FILE_INTERFACE 
#endif                                                                   // end #if defined ACTIVE_FILE_SYSTEM

#if defined INTERNAL_USER_FILES                                          // {13}

    #if defined _WINDOWS
    extern int user_files_not_in_code = 0;                               // {15}{16}
    #endif

// The user files must be entered once. They can be removed by calling with a zero pointer
// Alternatively they can be changed by calling a different set of files.
//
extern void fnEnterUserFiles(USER_FILE *ptrUserFileList)
{
    ptrUserFiles = ptrUserFileList;                                      // enter the reference to the user files
    #if defined _WINDOWS
    user_files_not_in_code = 0;                                          // {15}
    #endif
}

// Search through user files for the file name
//
extern MEMORY_RANGE_POINTER uOpenUserFile(CHAR *file_name)
{
    USER_FILE *ptrFiles = ptrUserFiles;
    if (ptrFiles == 0) {
        return 0;                                                        // no user files registered
    }
    #if defined _WINDOWS
    if (user_files_not_in_code != 0) {                                   // {15}
        ptrFiles = (USER_FILE *)fnGetFlashAdd((unsigned char *)ptrFiles);
    }
    #endif
    while (ptrFiles->fileName != 0) {
        const CHAR *ptrName = ptrFiles->fileName;
    #if defined _WINDOWS
        if (user_files_not_in_code != 0) {                               // {15}
            ptrName = (const CHAR *)fnGetFlashAdd((unsigned char *)ptrFiles->fileName);
        }
    #endif
        if (uStrEquiv(file_name, ptrName) != 0) {
            return (MEMORY_RANGE_POINTER)ptrFiles->file_content;         // return the start of the file
        }
        ptrFiles++;
    }
    return 0;                                                            // no file found with that name
}

// Search through the user files to match a pointer to the content start
//
static USER_FILE *fnFilterUserFile(unsigned char *ptrfile)
{
    USER_FILE *ptrFiles = (USER_FILE *)ptrUserFiles;
    if (ptrFiles == 0) {
        return 0;                                                        // no user files registered
    }
    #if defined _WINDOWS
    if (user_files_not_in_code != 0) {                                   // {15}
        ptrFiles = (USER_FILE *)fnGetFlashAdd((unsigned char *)ptrFiles);
    }
    #endif
    while (ptrFiles->fileName != 0) {
        if (ptrfile == ptrFiles->file_content) {
            return ptrFiles;                                             // return the user file pointer
        }
        ptrFiles++;
    }
    return 0;
}

// Get the mime type from the user file, or else from the file system file
//
extern int fnGetUserMimeType(unsigned char *ptrfile, unsigned char *ptrMimeType)
{
    USER_FILE *ptrFiles = (USER_FILE *)ptrUserFiles;
    #if defined _WINDOWS
    if (user_files_not_in_code != 0) {                                   // {15}
        ptrFiles = (USER_FILE *)fnGetFlashAdd((unsigned char *)ptrFiles);
    }
    #endif
    if (ptrFiles != 0) {
        while (ptrFiles->fileName != 0) {
            if (ptrfile == ptrFiles->file_content) {
                *ptrMimeType = ptrFiles->ucMimeType;
                return 0;                                                // the file is from user space
            }
            ptrFiles++;
        }
    }
    fnGetParsFile(ptrfile + sizeof(MAX_FILE_LENGTH), ptrMimeType, 1);
    return 1;
}

    #if defined EMBEDDED_USER_FILES

        #if defined _WINDOWS && !defined _LITTLE_ENDIAN
static void fnSwapTable(USER_FILE *ptrRAM_table)
{
    while (ptrRAM_table->file_content != 0) {
        ptrRAM_table->file_content = (unsigned char *)((((CAST_POINTER_ARITHMETIC)ptrRAM_table->file_content << 24) & 0xff000000) | (((CAST_POINTER_ARITHMETIC)ptrRAM_table->file_content << 8) & 0x00ff0000) | (((CAST_POINTER_ARITHMETIC)ptrRAM_table->file_content >> 8) & 0x0000ff00) | ((CAST_POINTER_ARITHMETIC)ptrRAM_table->file_content >> 24));
        ptrRAM_table->fileName = (unsigned char *)((((CAST_POINTER_ARITHMETIC)ptrRAM_table->fileName << 24) & 0xff000000) | (((CAST_POINTER_ARITHMETIC)ptrRAM_table->fileName << 8) & 0x00ff0000) | (((CAST_POINTER_ARITHMETIC)ptrRAM_table->fileName >> 8) & 0x0000ff00) | ((CAST_POINTER_ARITHMETIC)ptrRAM_table->fileName >> 24));
        ptrRAM_table->file_length = (unsigned long)((((CAST_POINTER_ARITHMETIC)ptrRAM_table->file_length << 24) & 0xff000000) | (((CAST_POINTER_ARITHMETIC)ptrRAM_table->file_length << 8) & 0x00ff0000) | (((CAST_POINTER_ARITHMETIC)ptrRAM_table->file_length >> 8) & 0x0000ff00) | ((CAST_POINTER_ARITHMETIC)ptrRAM_table->file_length >> 24));
        ptrRAM_table++;
    }
}
        #endif

static void fnInsertStringNames(USER_FILE *ptrRAM_table)
{
    CHAR temp[64];                                                       // {23}
    CHAR *new_string;
    int iLength;
    while (ptrRAM_table->file_content != 0) {
        fnGetParsFile((unsigned char *)ptrRAM_table->fileName, (unsigned char *)temp, sizeof(temp)); // get a string
        iLength = uStrlen(temp);
        new_string = UFILE_MALLOC(iLength + 1);
        uStrcpy(new_string, temp);
        ptrRAM_table->fileName = new_string;
        ptrRAM_table++;
    }
}

extern USER_FILE *fnActivateEmbeddedUserFiles(CHAR *cFile, int iType)
{
    USER_FILE *ptrUserTable;
    USER_FILE *ptrSearch;
    MAX_FILE_LENGTH table_length;
    MEMORY_RANGE_POINTER ptrThisFile = uOpenFile(cFile);
    MEMORY_RANGE_POINTER ptrPossibleEmbeddedFiles = ptrThisFile;
    MAX_FILE_LENGTH file_length = uGetFileLength(ptrThisFile);
    USER_FILE test_file;
    CHAR *last_string;
    unsigned char *last_content;
    if (file_length == 0) {
        return 0;                                                        // no file found
    }
    ptrPossibleEmbeddedFiles += (file_length + FILE_HEADER - sizeof(unsigned long)); // move to the location containing a pointer to the file table
    fnGetParsFile(ptrPossibleEmbeddedFiles, (unsigned char *)&ptrUserTable, sizeof(USER_FILE *));
        #if defined _WINDOWS && !defined _LITTLE_ENDIAN
    ptrUserTable = (USER_FILE *)((((CAST_POINTER_ARITHMETIC)ptrUserTable << 24) & 0xff000000) | (((CAST_POINTER_ARITHMETIC)ptrUserTable << 8) & 0x00ff0000) | (((CAST_POINTER_ARITHMETIC)ptrUserTable >> 8) & 0x0000ff00) | ((CAST_POINTER_ARITHMETIC)ptrUserTable >> 24));
        #endif
    table_length = ((unsigned char *)ptrPossibleEmbeddedFiles - (unsigned char *)ptrUserTable);
    if (table_length >= file_length) {
        return 0;                                                        // invalid for an embedded table
    }
    ptrSearch = ptrUserTable;
    last_string = (CHAR *)ptrThisFile;
    last_content = (unsigned char *)ptrThisFile;
    fnGetParsFile((unsigned char *)ptrSearch, (unsigned char *)&test_file, sizeof(USER_FILE));
        #if defined _WINDOWS && !defined _LITTLE_ENDIAN
    test_file.file_content = (unsigned char *)((((CAST_POINTER_ARITHMETIC)test_file.file_content << 24) & 0xff000000) | (((CAST_POINTER_ARITHMETIC)test_file.file_content << 8) & 0x00ff0000) | (((CAST_POINTER_ARITHMETIC)test_file.file_content >> 8) & 0x0000ff00) | ((CAST_POINTER_ARITHMETIC)test_file.file_content >> 24));
    test_file.fileName = (unsigned char *)((((CAST_POINTER_ARITHMETIC)test_file.fileName << 24) & 0xff000000) | (((CAST_POINTER_ARITHMETIC)test_file.fileName << 8) & 0x00ff0000) | (((CAST_POINTER_ARITHMETIC)test_file.fileName >> 8) & 0x0000ff00) | ((CAST_POINTER_ARITHMETIC)test_file.fileName >> 24));
    test_file.file_length = (unsigned long)((((CAST_POINTER_ARITHMETIC)test_file.file_length << 24) & 0xff000000) | (((CAST_POINTER_ARITHMETIC)test_file.file_length << 8) & 0x00ff0000) | (((CAST_POINTER_ARITHMETIC)test_file.file_length >> 8) & 0x0000ff00) | ((CAST_POINTER_ARITHMETIC)test_file.file_length >> 24));
        #endif
    while (1) {
        if (((CHAR *)test_file.fileName > last_string) && ((unsigned char *)test_file.fileName < (unsigned char *)ptrUserTable)) {
            if (((unsigned char *)test_file.file_content > last_content) && ((unsigned char *)test_file.file_content < (unsigned char *)ptrUserTable)) {
                if (test_file.file_length < file_length) {
                    last_string = test_file.fileName;
                    last_content = test_file.file_content;
                    ptrSearch++;
                    fnGetParsFile((unsigned char *)ptrSearch, (unsigned char *)&test_file, sizeof(USER_FILE));
        #if defined _WINDOWS && !defined _LITTLE_ENDIAN
                    test_file.file_content = (unsigned char *)((((CAST_POINTER_ARITHMETIC)test_file.file_content << 24) & 0xff000000) | (((CAST_POINTER_ARITHMETIC)test_file.file_content << 8) & 0x00ff0000) | (((CAST_POINTER_ARITHMETIC)test_file.file_content >> 8) & 0x0000ff00) | ((CAST_POINTER_ARITHMETIC)test_file.file_content >> 24));
                    test_file.fileName = (unsigned char *)((((CAST_POINTER_ARITHMETIC)test_file.fileName << 24) & 0xff000000) | (((CAST_POINTER_ARITHMETIC)test_file.fileName << 8) & 0x00ff0000) | (((CAST_POINTER_ARITHMETIC)test_file.fileName >> 8) & 0x0000ff00) | ((CAST_POINTER_ARITHMETIC)test_file.fileName >> 24));
                    test_file.file_length = (unsigned long)((((CAST_POINTER_ARITHMETIC)test_file.file_length << 24) & 0xff000000) | (((CAST_POINTER_ARITHMETIC)test_file.file_length << 8) & 0x00ff0000) | (((CAST_POINTER_ARITHMETIC)test_file.file_length >> 8) & 0x0000ff00) | ((CAST_POINTER_ARITHMETIC)test_file.file_length >> 24));
        #endif
                    if ((test_file.fileName == 0) && (test_file.file_content == 0)) { // possibly end of list
                        if ((ptrSearch + 1) == (USER_FILE *)ptrPossibleEmbeddedFiles) {
        #if defined _WINDOWS && !defined _LITTLE_ENDIAN
                            iType = USER_FILE_IN_EXTERNAL_SPACE;         // when simulating use SRAM due to endian compatibility in this case
        #endif
                            if (iType == USER_FILE_IN_EXTERNAL_SPACE) {
                                USER_FILE *ptrRAM_table = UFILE_MALLOC(table_length); // get memory to copy the embedded table to
                                fnGetParsFile((unsigned char *)ptrUserTable, (unsigned char *)ptrRAM_table, table_length); // copy the table to local SRAM
        #if defined _WINDOWS && !defined _LITTLE_ENDIAN
                                fnSwapTable(ptrRAM_table);
        #endif
                                fnInsertStringNames(ptrRAM_table);       // copy file strings to SRAM for direct access support
                                ptrUserTable = ptrRAM_table;
                            }
                            fnEnterUserFiles(ptrUserTable);              // use embedded user file - which can be in internal FLASH or external SPI FLASH
        #if defined _WINDOWS
                            if (iType != USER_FILE_IN_EXTERNAL_SPACE) {  // {20}
                                user_files_not_in_code = 1;              // {15}
                            }
        #endif
                            return ptrUserTable;
                        }
                        return 0;
                    }
                    continue;
                }
            }
        }
        return 0;                                                        // invalid content
    }
}
    #endif                                                               // end #if defined EMBEDDED_USER_FILES

    #if defined _WINDOWS
extern unsigned char fnUserFileProperties(unsigned char *ptrfile)
{
    USER_FILE *ptrFiles = (USER_FILE *)ptrUserFiles;
    if (user_files_not_in_code != 0) {
        ptrFiles = (USER_FILE *)fnGetFlashAdd((unsigned char *)ptrFiles);
    }
    if (ptrFiles != 0) {
        while (ptrFiles->fileName != 0) {
            if (ptrfile == ptrFiles->file_content) {
                return ptrFiles->ucProperties;                           // return the file properties for the simulator
            }
            ptrFiles++;
        }
    }
    return 0;
}
    #endif
#endif                                                                   // end #if defined INTERNAL_USER_FILES

#if (defined ACTIVE_FILE_SYSTEM || defined INTERNAL_USER_FILES) && !defined _NO_FILE_INTERFACE  // {14}
// This routine returns a pointer to a file (which can either be empty, exits or overlap - this is not checked here)
//
extern MEMORY_RANGE_POINTER uOpenFile(CHAR *ptrfileName)
{
    MEMORY_RANGE_POINTER ptrFlash;
    #if defined INTERNAL_USER_FILES                                      // {13}
    ptrFlash = uOpenUserFile(ptrfileName);
    if (ptrFlash != 0) {
        return ptrFlash;                                                 // matching user file has been found
    }
    #endif
    #if defined ACTIVE_FILE_SYSTEM
    ptrFlash = uFILE_SYSTEM_START + fnGetFilePointer(ptrfileName);
    #endif
    return (ptrFlash);                                                   // return file pointer
}

    #if defined SUB_FILE_SIZE
// Return file type - whether it is a sub-file or not                    {2}
//
extern unsigned char fnGetFileType(CHAR *ptrFileName)
{
    if (*ptrFileName >= ('0' + LAST_FULL_SECTOR /*(FILE_SYSTEM_SIZE/FILE_GRANULARITY)*/)) {
        return SUB_FILE_TYPE;
    }
    return 0;
}
    #endif
#endif

#if (defined ACTIVE_FILE_SYSTEM || defined INTERNAL_USER_FILES) && !defined _NO_FILE_INTERFACE
// This routine returns the length of the file, a zero if empty or invalid
//
extern MAX_FILE_LENGTH uGetFileLength(MEMORY_RANGE_POINTER ptrfile)
{
    MAX_FILE_LENGTH FileLength;
    #if defined INTERNAL_USER_FILES                                      // {13}
    USER_FILE *ptrUserFile = fnFilterUserFile(ptrfile);
    if (ptrUserFile != 0) {
        return ptrUserFile->file_length;                                 // return user file length
    }
    #endif
    #if defined ACTIVE_FILE_SYSTEM
    fnGetParsFile(ptrfile, (unsigned char *)&FileLength, sizeof(MAX_FILE_LENGTH));

    if (FileLength == 0) {                                               // protect against corrupted file so that it can be deleted
        return _FILE_GRANULARITY;                                        // the size of a file block
    }

    if (FileLength == (MAX_FILE_LENGTH)(-1)) {                           // check for blank FLASH memory
        return 0;                                                        // no length so empty
    }
                                                                         // if a (corrupted) length should be too large, we limit it to end of the file system
        #if defined EXTENSION_FILE_COUNT_VARIABLE                        // {25}
    if (((CAST_EXTENDED_POINTER_ARITHMETIC)((MEMORY_RANGE_POINTER)(_uFILE_SYSTEM_END - FILE_HEADER)) - (CAST_EXTENDED_POINTER_ARITHMETIC)ptrfile) < FileLength) {
        FileLength = (MAX_FILE_LENGTH)((CAST_EXTENDED_POINTER_ARITHMETIC)(MEMORY_RANGE_POINTER)(_uFILE_SYSTEM_END - FILE_HEADER) - (CAST_EXTENDED_POINTER_ARITHMETIC)ptrfile); // the space remaining
    }
        #else
    if (((CAST_EXTENDED_POINTER_ARITHMETIC)((MEMORY_RANGE_POINTER)(uFILE_START + FILE_SYSTEM_SIZE - FILE_HEADER)) - (CAST_EXTENDED_POINTER_ARITHMETIC)ptrfile) < FileLength) {
        FileLength = (MAX_FILE_LENGTH)((CAST_EXTENDED_POINTER_ARITHMETIC)(MEMORY_RANGE_POINTER)(uFILE_START + FILE_SYSTEM_SIZE - FILE_HEADER) - (CAST_EXTENDED_POINTER_ARITHMETIC)ptrfile); // the space remaining
    }
        #endif
    #endif
    return FileLength;
}

    #if defined INTERNAL_USER_FILES && defined ACTIVE_FILE_SYSTEM  && !defined _NO_FILE_INTERFACE // {19}
// Local copy of the routine uGetFileLength which adjusts the file header if the file is not a user file
//
static MAX_FILE_LENGTH uGetFileLength_variable_header(MEMORY_RANGE_POINTER ptrfile, unsigned char *ptrHeaderLength)
{
    MAX_FILE_LENGTH FileLength;
    USER_FILE *ptrUserFile = fnFilterUserFile(ptrfile);                  // check for a user file
    if (ptrUserFile != 0) {
        #if defined _WINDOWS
        if (ptrUserFile->ucProperties & FILE_NOT_CODE) {
            *ptrHeaderLength = 0x80;                                     // signal that the content is not in code
        }
        #endif
        return ptrUserFile->file_length;                                 // valid user file found
    }
        #if defined ACTIVE_FILE_SYSTEM && !defined _NO_FILE_INTERFACE
    fnGetParsFile(ptrfile, (unsigned char *)&FileLength, sizeof(MAX_FILE_LENGTH));

    *ptrHeaderLength = FILE_HEADER;                                      // adjust the length since it is a uFileSystem file
    if (FileLength == 0) {                                               // protect against corrupted file so that it can be deleted
        return _FILE_GRANULARITY;                                        // the size of a file block
    }

    if (FileLength == (MAX_FILE_LENGTH)(-1)) {                           // check for blank FLASH memory
        return 0;                                                        // no length so empty
    }
                                                                         // if a (corrupted) length should be too large, we limit it to end of the file system
    if (((CAST_EXTENDED_POINTER_ARITHMETIC)((MEMORY_RANGE_POINTER)(uFILE_START + FILE_SYSTEM_SIZE - FILE_HEADER)) - (CAST_EXTENDED_POINTER_ARITHMETIC)ptrfile) < FileLength) {
        FileLength = (MAX_FILE_LENGTH)((CAST_EXTENDED_POINTER_ARITHMETIC)(MEMORY_RANGE_POINTER)(uFILE_START + FILE_SYSTEM_SIZE - FILE_HEADER) - (CAST_EXTENDED_POINTER_ARITHMETIC)ptrfile); // the space remaining
    }
        #endif
    return FileLength;
}
    #endif
#endif

#if defined ACTIVE_FILE_SYSTEM && !defined _NO_FILE_INTERFACE
// This routine returns a pointer to the next file in the system, or zero when no more
//
#if defined SUPPORT_MIME_IDENTIFIER
  extern MEMORY_RANGE_POINTER uOpenNextMimeFile(MEMORY_RANGE_POINTER ptrfileLocation, MAX_FILE_LENGTH *FileLength, unsigned char *ucMimeType  SUB_FILE)
#else
  extern MEMORY_RANGE_POINTER uOpenNextFile(MEMORY_RANGE_POINTER ptrfileLocation, MAX_FILE_LENGTH *FileLength  SUB_FILE)
#endif
{
    if (ptrfileLocation == 0) {
        ptrfileLocation = uFILE_SYSTEM_START;                            // get first file found when zero passed
    }
    else {
        MAX_FILE_LENGTH ThisLength = uGetFileLength(ptrfileLocation);    // set the pointer to next possible block
        ThisLength += (FILE_HEADER - 1);
    #if defined SUB_FILE_SIZE
        if (ucSubfile != 0) {
            ThisLength &= ~(SUB_FILE_SIZE - 1);                          // round the length up to next file sub-block size
            ThisLength += SUB_FILE_SIZE;
        }
        else {
            ROUNDUP_BOUNDARY(ThisLength, _FILE_GRANULARITY);             // round the length up to next file block size {5}
        }
    #else
        ROUNDUP_BOUNDARY(ThisLength, _FILE_GRANULARITY);                 // round the length up to next file block size {5}
    #endif
        ptrfileLocation += ThisLength;                                   // address of next possible block
    }

    while (1) {
        if (ptrfileLocation >= (_uFILE_SYSTEM_END - 1)) {                // end of file system reached
            *FileLength = 0;                                             // file length is zero
            return 0;                                                    // nothing found
        }
        if ((*FileLength = uGetFileLength(ptrfileLocation)) != 0) {
    #if defined SUPPORT_MIME_IDENTIFIER
            fnGetParsFile((ptrfileLocation + sizeof(MAX_FILE_LENGTH)), ucMimeType, 1);
    #endif
            return ptrfileLocation;                                      // next file found here
        }
    #if defined SUB_FILE_SIZE
        if (ucSubfile != 0) {
            ptrfileLocation += SUB_FILE_SIZE;                            // check next sub-block
        }
        else {
            ptrfileLocation += _FILE_GRANULARITY;                        // check next block
        }
    #else
        ptrfileLocation += _FILE_GRANULARITY;                            // check next block
    #endif
    }
}

// Erase all sectors occupied by file
//
extern int uFileErase(MEMORY_RANGE_POINTER ptrFile, MAX_FILE_LENGTH FileLength) // {4}
{
    if (fnEraseFlashSector(ptrFile, FileLength)) {                       // erase a file block
        return  (-1);                                                    // return error if one found
    }
    return 0;
}


#if defined PROTECTED_FILE
static int iFlashUnprotected = 0;

// Protect all file sections after writing and erasing has terminated
//
extern void fnProtectFile(void)
{
 /* ?? when this is called - the interrupts are disabled 5 times???
    unsigned char *ptrFileSegment = uFILE_SYSTEM_START;
    if ((iFlashUnprotected != 0) && (--iFlashUnprotected == 0)) {
        while (ptrFileSegment < (uFILE_SYSTEM_START + FILE_SYSTEM_SIZE)) {
            fnProtectFlash(ptrFileSegment, PROTECT_SECTOR);
            ptrFileSegment += FILE_GRANULARITY;
        }
    }
    */
}

// Unprotect all file sections ready for writing and erasing
//
extern void fnUnprotectFile(void)
{
    unsigned char *ptrFileSegment = uFILE_SYSTEM_START;
    if (iFlashUnprotected == 0) {
        while (ptrFileSegment < (uFILE_SYSTEM_START + FILE_SYSTEM_SIZE)) {
            fnProtectFlash(ptrFileSegment, UNPROTECT_SECTOR);
            ptrFileSegment += FILE_GRANULARITY;
        }
    }
    iFlashUnprotected++;
}
#endif                                                                   // end if PROTECTED_FILE

#if !defined _NO_FILE_INTERFACE                                          // {14}

#if defined UFILESYSTEM_APPEND                                           // {26}

// Open a file for appending to
//
extern unsigned char *uOpenFileAppend(CHAR *ptrfileName)
{
    ptrAppendFlash = uOpenFile(ptrfileName);
    append_length = LengthInProgress = uGetFileLength(ptrAppendFlash);
    if (append_length != 0) {
        FileInProgress = (ptrAppendFlash + FILE_HEADER);
    #if defined FLASH_LINE_SIZE && FLASH_LINE_SIZE > 0                   // {27}
        #if !defined ONLY_INTERNAL_FLASH_STORAGE
        if (_STORAGE_INTERNAL_FLASH != fnGetStorageType(FileInProgress, 0)) { // only use flash line buffer when internal flash is being accessed
            return (ptrAppendFlash);                                     // return file pointer
        }
        #endif
        ptrAppendLine = (unsigned char *)((CAST_POINTER_ARITHMETIC)(FileInProgress + append_length) & ~(FLASH_LINE_SIZE - 1));        
        if ((FileInProgress + append_length) != ptrAppendLine) {
            uMemcpy(ucFirstLine, fnGetFlashAdd(ptrAppendLine), FLASH_LINE_SIZE); // fill the flash line buffer with original data
            ptrAppendSector = (unsigned char *)((CAST_POINTER_ARITHMETIC)(FileInProgress + append_length) & ~(FLASH_GRANULARITY - 1));
        }
        else {
            ptrAppendLine = 0;                                           // since the start of the append is on a flash line boundary nothing special needs to be done
        }
    #endif
    }
    return (ptrAppendFlash);                                             // return file pointer
}
#endif


// This routine deletes present files which reside at the position of the new file and then writes the new data
//
    #if defined SUB_FILE_SIZE
    extern MAX_FILE_LENGTH uFileWrite(MEMORY_RANGE_POINTER ptrFile, unsigned char *ptrData, MAX_FILE_LENGTH DataLength, unsigned char ucSubFile)
    #else
    extern MAX_FILE_LENGTH uFileWrite(MEMORY_RANGE_POINTER ptrFile, unsigned char *ptrData, MAX_FILE_LENGTH DataLength)
    #endif
{
    #if defined SUPPORT_MIME_IDENTIFIER
    #define MIME_TYPE , &ucMimeType
    #define UOPENNEXTFILE uOpenNextMimeFile
    unsigned char ucMimeType;
    #else
    #define UOPENNEXTFILE uOpenNextFile
    #define MIME_TYPE
    #endif

    if (DataLength == 0) {
        return LengthInProgress;
    }

    if (LengthInProgress == 0) {                                         // first write
        MAX_FILE_LENGTH FileLength;
        MEMORY_RANGE_POINTER prtPresentFile;
        MEMORY_RANGE_POINTER ptrNewFile = (ptrFile + FILE_HEADER);       // leave space for file header
    #if defined SPI_SW_UPLOAD                                            // {3}
        MEMORY_RANGE_POINTER ptrFileSysEnd = (MEMORY_RANGE_POINTER)(uFILE_START + FILE_SYSTEM_SIZE);
        MEMORY_RANGE_POINTER ptrSPI_FlashEnd = (ptrFileSysEnd + SPI_DATA_FLASH_SIZE);
    #endif
        FileInProgress = ptrNewFile;
    #if defined SPI_SW_UPLOAD                                            // {3}
        if ((ptrFile >= ptrFileSysEnd) && (ptrFile < ptrSPI_FlashEnd)) { // we are working from external SPI FLASH memory
            if ((FileLength = uGetFileLength(ptrFile)) != 0) {           // if there is already data in the SPI FLASH
                uFileErase(ptrFile, FileLength);                         // erase the file to make space for new upload
            }
        }
    #endif
    #if defined NO_ACCUMULATIVE_WORDS
        ucEndByteValid = 0;
        ucEndByte = 0;
    #elif defined FLASH_LINE_SIZE && FLASH_LINE_SIZE > 0
        uMemset(ucFirstLine, 0xff, sizeof(ucFirstLine));                 // flush FLASH line backup buffer
    #endif
    #if defined PROTECTED_FILE
        fnUnprotectFile();                                               // {1}
    #endif
    #if defined SUB_FILE_SIZE
        if (ucSubFile == 0) {
    #endif
        if ((prtPresentFile = UOPENNEXTFILE(0, &FileLength  MIME_TYPE  SUB_FILE_OFF)) != 0) { // get details of first file in the system
            do {
                FileLength += (_FILE_GRANULARITY + (FILE_HEADER - 1));   // round the length up to next file block size
                ROUNDDOWN_BOUNDARY(FileLength, _FILE_GRANULARITY);       // {5}
                if ((ptrNewFile >= prtPresentFile) && (ptrNewFile < (prtPresentFile + FileLength))) {
                    uFileErase(prtPresentFile, FileLength);              // erase present file to make space for new one
                    if (FileLength >= DataLength) {
                        break;                                           // we now have enough space to save the new data
                    }
                    ptrNewFile += FileLength;                            // we may still have to make more space
                }
                prtPresentFile = UOPENNEXTFILE(prtPresentFile, &FileLength  MIME_TYPE  SUB_FILE_OFF); // try next present file
            }
            while ((prtPresentFile != 0) && (prtPresentFile <= ptrNewFile));
        }
    #if defined SUB_FILE_SIZE
        }
    #endif
    #if defined NO_ACCUMULATIVE_WORDS
        if ((unsigned char)((CAST_EXTENDED_POINTER_ARITHMETIC)FileInProgress + LengthInProgress) & 0x1) { // we have to handle odd addresses carefully
            ucStartByte = *ptrData++;                                    // save the first byte and move to even boundary
            LengthInProgress++;
            DataLength--;
        }
    #endif
    }
    else {                                                               // this is a follow on write so check that we have enough space to write - or else make space by deleting next file
    #if defined SUB_FILE_SIZE                                            // {31}
        unsigned short usPresentBlockCheck = (unsigned short)(((FileInProgress - uFILE_SYSTEM_START) + LengthInProgress)/SUB_FILE_SIZE);
        unsigned short usNextBlockCheck    = (unsigned short)((((FileInProgress - uFILE_SYSTEM_START) + LengthInProgress + DataLength) - 1)/SUB_FILE_SIZE);
    #endif
        unsigned short usPresentBlock = (unsigned short)(((FileInProgress - uFILE_SYSTEM_START) + LengthInProgress)/FILE_GRANULARITY);
        unsigned short usNextBlock    = (unsigned short)((((FileInProgress - uFILE_SYSTEM_START) + LengthInProgress + DataLength) - 1)/FILE_GRANULARITY);


    #if defined SPI_SW_UPLOAD                                            // {3}
        MEMORY_RANGE_POINTER ptrFileSysEnd = (MEMORY_RANGE_POINTER)(uFILE_START + FILE_SYSTEM_SIZE);
        MEMORY_RANGE_POINTER ptrSPI_FlashEnd = ptrFileSysEnd + SPI_DATA_FLASH_SIZE;

        if (((FileInProgress + LengthInProgress) >= ptrFileSysEnd) && ((FileInProgress + LengthInProgress) < ptrSPI_FlashEnd)) { // we are working from external SPI FLASH memeory
            if ((LengthInProgress + DataLength) < (SPI_DATA_FLASH_SIZE - FILE_HEADER)) {  // destination is SPI data flash
                _fnWriteBytesFlash((FileInProgress + LengthInProgress), ptrData, DataLength);
                LengthInProgress += DataLength;
            }

            return LengthInProgress;
        }
    #endif
    #if defined SUB_FILE_SIZE                                            // {31}
        if (usNextBlockCheck >= LAST_SUB_FILE_BLOCK) {                   // limit writes to within valid FLASH range (anything more is cut)
            if (usPresentBlockCheck >= LAST_SUB_FILE_BLOCK) {
                return LengthInProgress;                                 // don't allow any (more) writes to take place
            }
            DataLength = (_uFILE_SYSTEM_END - (FileInProgress + LengthInProgress)); // allow filling to end of available memory
            usNextBlock = (LAST_SUB_FILE_BLOCK - 1);
        }
    #else
        if (usNextBlock >= _LAST_FILE_BLOCK) {                           // {11} limit writes to within valid FLASH range (anything more is cut)
            if (usPresentBlock >= _LAST_FILE_BLOCK) {
                return LengthInProgress;                                 // don't allow any (more) writes to take place
            }
            DataLength = (_uFILE_SYSTEM_END - (FileInProgress + LengthInProgress)); // {11} allow filling to end of available memory
            usNextBlock = (_LAST_FILE_BLOCK - 1);
        }
    #endif
    #if defined SUB_FILE_SIZE
        if (ucSubFile == 0) {
    #endif
            while (usPresentBlock != usNextBlock) {
                MEMORY_RANGE_POINTER ptrNextBlock = uFILE_SYSTEM_START + (usPresentBlock + 1)*_FILE_GRANULARITY;
                MAX_FILE_LENGTH NextLength;
                if (ptrNextBlock >= uFILE_SYSTEM_END) {                  // {31} not within file system
                    break;
                }
                NextLength = uGetFileLength(ptrNextBlock);               // get length of following file
                if (NextLength) {                                        // if not empty
                    if ((ptrNextBlock + NextLength) >= uFILE_SYSTEM_END) { // verify that it is not a corrupted length
                        NextLength = (uFILE_SYSTEM_END - ptrNextBlock);  // {31} limit deletes to within file system space
                    }
                    uFileErase(ptrNextBlock, (MAX_FILE_LENGTH)(NextLength + (FILE_HEADER - 1)));// since we want to write to location occupied already, we delete the existing file
                }
                usPresentBlock++;
            }
    #if defined SUB_FILE_SIZE
        }
    #endif
    }

    #if defined NO_ACCUMULATIVE_WORDS
    if (ucEndByteValid) {
        *(--ptrData) = ucEndByte;                                        // we assume the input buffer can accept this - FTP is OK
        DataLength++;
        ucEndByteValid = 0;
    }
    if (DataLength & 0x01) {                                             // we can't write odd length so save end byte
        DataLength--;
        ucEndByte = *(ptrData + DataLength);
        ucEndByteValid = 1;
    }
    #endif
    #if defined FLASH_LINE_SIZE && FLASH_LINE_SIZE > 0                   // {6}
    if (LengthInProgress < (FLASH_LINE_SIZE - FILE_HEADER)) {            // save first FLASH line of file in backup buffer
        size_t CopyLength = ((FLASH_LINE_SIZE - FILE_HEADER) - LengthInProgress); // {21} the length of the data that can be copied to the line buffer
        if (DataLength < CopyLength) {
            CopyLength = DataLength;
        }
        uMemcpy(&ucFirstLine[FILE_HEADER + LengthInProgress], ptrData, CopyLength);
        LengthInProgress += CopyLength;
        ptrData += CopyLength;
        DataLength -= CopyLength;
    }
        #if defined UFILESYSTEM_APPEND                                   // {27}
    else if (ptrAppendLine != 0) {
        if (((FileInProgress + LengthInProgress) > ptrAppendLine) && ((FileInProgress + LengthInProgress) < (ptrAppendLine + FLASH_LINE_SIZE))) {
            do {
                ucFirstLine[(FileInProgress + LengthInProgress) - ptrAppendLine] = *ptrData++;
                LengthInProgress++;
                DataLength--;
            } while ((DataLength != 0) && (FileInProgress + LengthInProgress) < (ptrAppendLine + FLASH_LINE_SIZE));
        }
    }
        #endif
    #endif
    if ((DataLength != 0) || (ptrData == 0)) {
        _fnWriteBytesFlash((FileInProgress + LengthInProgress), ptrData, DataLength); // we are sure that we have enough space to save the present data so save it
        LengthInProgress += DataLength;
    }
    return LengthInProgress;
}

extern MAX_FILE_LENGTH uGetFileData(MEMORY_RANGE_POINTER ptrFile, MAX_FILE_SYSTEM_OFFSET FileOffset, unsigned char *ucData, MAX_FILE_LENGTH DataLength)
{
    #if defined INTERNAL_USER_FILES                                      // {19}
    unsigned char ucFileHeaderLength = 0;
    MAX_FILE_SYSTEM_OFFSET RemainingLength = uGetFileLength_variable_header(ptrFile, &ucFileHeaderLength);
    #else
    MAX_FILE_SYSTEM_OFFSET RemainingLength = uGetFileLength(ptrFile);
    #endif
    if (RemainingLength) {
        RemainingLength -= FileOffset;                                   // this is remaining length in file from this position
        if (RemainingLength != 0) {
            if (RemainingLength >= DataLength) {
                 RemainingLength = DataLength;
            }
    #if defined INTERNAL_USER_FILES                                      // {19}
        #if defined _WINDOWS
            if ((ucFileHeaderLength & 0x7f) == 0) {
                if (ucFileHeaderLength & 0x80) {
                    ucFileHeaderLength = 0;
                }
                else {
                    iFetchingInternalMemory = _ACCESS_IN_CODE;           // mark that the file in in code memory for simulation use
                }
            }
        #endif
            fnGetParsFile((ptrFile + FileOffset + ucFileHeaderLength), ucData, (MAX_FILE_LENGTH)RemainingLength);
    #else
            fnGetParsFile((ptrFile + FileOffset + FILE_HEADER), ucData, (MAX_FILE_LENGTH)RemainingLength);
    #endif
        }
    }
    return ((MAX_FILE_LENGTH)RemainingLength);                           // return the length copied
}

#if defined UFILESYSTEM_APPEND                                           // {26}
// Update the new file length by copying the initial sector to ram, deleting the sector, updating the length and saving the sector again
//
static MAX_FILE_LENGTH fnAppendFileLength(unsigned char *ptrFile)
{
    MAX_FILE_LENGTH *ptrLength;
    MAX_FILE_LENGTH Len = LengthInProgress;
    MAX_FILE_LENGTH SectorLength;
    unsigned long  ulBuffer[FLASH_GRANULARITY/sizeof(unsigned long)];    // sector backup space on stack (assumes internal granularity is valid) - buffer long word aligned
    #if defined FLASH_LINE_SIZE && FLASH_LINE_SIZE > 0
    _fnWriteBytesFlash(ptrFile, 0, 0);                                   // {30} close any outstanding FLASH buffer
    if (ptrAppendLine != 0) {
        if (ptrFile != ptrAppendSector) {                                // insert the start flash line buffer into the initial sector
            SectorLength = ((LengthInProgress + FILE_HEADER) - (ptrAppendSector - ptrFile));
            if (SectorLength > FLASH_GRANULARITY) {
                SectorLength = FLASH_GRANULARITY;
            }
            fnGetParsFile(ptrAppendSector, (unsigned char *)ulBuffer, SectorLength); // read the first append sector's content (or less if there is not a complete sector length)
            uMemcpy(&ulBuffer[((ptrAppendLine - ptrAppendSector)/sizeof(unsigned long))], ucFirstLine, FLASH_LINE_SIZE); // fill the first line content
            fnEraseFlashSector(ptrAppendSector, 0);                      // erase the flash sector
            if (_fnWriteBytesFlash(ptrAppendSector, (unsigned char *)ulBuffer, SectorLength)) { // {30} save the flash sector back
                return 0;
            }
            _fnWriteBytesFlash(ptrFile, 0, 0);                            // {30} close any outstanding FLASH buffer
            ptrAppendLine = 0;
        }
    }
    #endif
    SectorLength = (LengthInProgress + FILE_HEADER);
    if (SectorLength > FLASH_GRANULARITY) {
        SectorLength = FLASH_GRANULARITY;
    }
    fnGetParsFile(ptrFile, (unsigned char *)ulBuffer, SectorLength);     // read the first sector's content (or less if there is not a complete sector length)
    #if defined FLASH_LINE_SIZE && FLASH_LINE_SIZE > 0                   // {27}
    if (ptrAppendLine != 0) {                                            // if the first flash row append content is to be saved in the first sector       
        uMemcpy(&ulBuffer[((ptrAppendLine - ptrAppendSector)/sizeof(unsigned long))], ucFirstLine, FLASH_LINE_SIZE); // fill the first line content
        ptrAppendLine = 0;
    }
    #endif
    fnEraseFlashSector(ptrFile, 0);                                      // erase the flash sector
    ptrLength = (MAX_FILE_LENGTH *)ulBuffer;
    *ptrLength = LengthInProgress;                                       // modify the file length
    LengthInProgress = 0;
    if (_fnWriteBytesFlash(ptrFile, (unsigned char *)ulBuffer, SectorLength)) { // {30} save the first flash sector back (with modified length)
        return 0;
    }
    #if defined FLASH_LINE_SIZE && FLASH_LINE_SIZE > 0                   // {27}
    _fnWriteBytesFlash(ptrFile, 0, 0);                                   // {30} close any outstanding FLASH buffer
    #endif
    return (Len);
}
#endif

// Write the file length and close write process
//
#if defined SUPPORT_MIME_IDENTIFIER
    extern MAX_FILE_LENGTH uFileCloseMime(MEMORY_RANGE_POINTER ptrFile, unsigned char *ucMimeType)
#else
    extern MAX_FILE_LENGTH uFileClose(MEMORY_RANGE_POINTER ptrFile)
#endif
{
    MAX_FILE_LENGTH Len;

    #if defined UFILESYSTEM_APPEND                                       // {26}
    if (append_length != 0) {                                            // if the file is being appended to
        append_length = 0;                                               // terminate append mode
        #if defined ONLY_INTERNAL_FLASH_STORAGE
            return fnAppendFileLength(ptrFile);                          // close with update of file length
        #else
        if (!(fnGetStorageType((unsigned char *)ptrFile, 0) & EEPROM_CHARACTERISTICS)) { // if the initial sector of memory is not EEPROM type
            return fnAppendFileLength(ptrFile);                          // close with update of file length
        }
            #if defined SUPPORT_MIME_IDENTIFIER
        ucMimeType = 0;                                                  // don't change existing mime type when appending data to a file
            #endif
        #endif
    }
    #endif

    #if defined NO_ACCUMULATIVE_WORDS
        #if defined SUPPORT_MIME_IDENTIFIER
    unsigned char ucMimeTypeWord[2];
        #endif

    if (ucEndByteValid) {
        _fnWriteBytesFlash((FileInProgress + LengthInProgress++), &ucEndByte, 1); // add last byte to end of file
        ucEndByteValid = 0;
    }
    #endif
    if ((Len = LengthInProgress) == 0) {
    #if defined PROTECTED_FILE
        fnProtectFile();                                                 // {1}
    #endif
        return 0;
    }

    #if defined SUPPORT_MIME_IDENTIFIER
        #if defined NO_ACCUMULATIVE_WORDS
    ucMimeTypeWord[0] = *ucMimeType;
    ucMimeTypeWord[1] = ucStartByte;
    _fnWriteBytesFlash((ptrFile + sizeof(MAX_FILE_LENGTH)), ucMimeTypeWord, 2);
        #elif defined FLASH_LINE_SIZE && FLASH_LINE_SIZE > 0             // {6}
    ucFirstLine[sizeof(MAX_FILE_LENGTH)] = *ucMimeType;                  // add mime type to the FLASH line buffer
        #else
            #if defined UFILESYSTEM_APPEND
    if (ucMimeType != 0) {
        _fnWriteBytesFlash((ptrFile + sizeof(MAX_FILE_LENGTH)), ucMimeType, 1);
    }
            #else
    _fnWriteBytesFlash((ptrFile + sizeof(MAX_FILE_LENGTH)), ucMimeType, 1);
            #endif
        #endif
    #endif

    LengthInProgress = 0;
    #if defined FLASH_LINE_SIZE && FLASH_LINE_SIZE > 0                   // {6}
        #if FLASH_ROW_SIZE > 4
    if (Len >= (FLASH_ROW_SIZE - FILE_HEADER)) {
        _fnWriteBytesFlash(ptrFile, 0, 0);                               // {30} close any outstanding FLASH buffer {9}
    }
        #else
    _fnWriteBytesFlash(ptrFile, 0, 0);                                   // {30} close any outstanding FLASH buffer {18}
        #endif
    uMemcpy(ucFirstLine, &Len, sizeof(MAX_FILE_LENGTH));                 // add length to first line buffer
    _fnWriteBytesFlash(ptrFile, ucFirstLine, sizeof(ucFirstLine));       // add the first line
    if ((_fnWriteBytesFlash(ptrFile, 0, 0)) != 0) {                      // {30} and then write it to complete the work {9}
        Len = 0;
    }
    #else
    if ((_fnWriteBytesFlash(ptrFile, (unsigned char *)&Len, sizeof(MAX_FILE_LENGTH))) != 0) { // write the length to the file
        Len = 0;
    }
    #endif
    #if defined PROTECTED_FILE
    fnProtectFile();                                                     // {1}
    #endif
    return (Len);
}


// This routine returns the name of the file
//
#if defined EXTENDED_UFILESYSTEM
    extern int uGetFileName(MEMORY_RANGE_POINTER ptrFile, CHAR *ptrFileName)
#else
    extern CHAR uGetFileName(MEMORY_RANGE_POINTER ptrFile)
#endif
{                                                                        // we are using a simple one number / letter scheme
    MEMORY_RANGE_POINTER ptrBlock = (uFILE_SYSTEM_START + FILE_GRANULARITY);
    unsigned char ucBlock = 0;

    while (ptrBlock <= ptrFile) {
        ucBlock++;
        ptrBlock += FILE_GRANULARITY;
#if defined EXTENDED_UFILESYSTEM                                         // {25}
        if (ucBlock >= LAST_FILE_BLOCK) {                                // if beyond the standard uFileSystem area
            fnConvertToExtendedName(ptrFile, ptrFileName);
            return 1;
        }
#endif
    }
#if defined EXTENDED_UFILESYSTEM
    *ptrFileName = fnConvertToName(ucBlock);
    return 0;
#else
    return (fnConvertToName(ucBlock));
#endif
}

#if defined SUB_FILE_SIZE
// This routine returns the name of the file when viewed as a sub-file
//
extern CHAR uGetSubFileName(unsigned char *ptrFile)
{                                                                        // we are using a simple one number / letter scheme
    unsigned char *ptrBlock = uFILE_SYSTEM_START + SUB_FILE_SIZE;
    unsigned char ucBlock = LAST_FULL_SECTOR;

    while (ptrBlock <= ptrFile) {
        ucBlock++;
        ptrBlock += SUB_FILE_SIZE;
    }
    return (fnConvertToName(ucBlock));
}
#endif
#endif

#if defined SUPPORT_FILE_COMPARE
// This routine compares the content of a file without modifying it. If the file has to be retrieved, it is performed in user definable blocks.
// If the file can be directly addressed, it uses uMemcmp - defined by FILES_ADDRESSABLE
//
extern int uCompareFile(unsigned char *ptrFile, unsigned char *ptrData, MAX_FILE_LENGTH DataLength)
{
    #if defined FILES_ADDRESSABLE
    return (uMemcmp(fnGetFlashAdd(ptrFile), ptrData, DataLength));
    #else
    unsigned char ucCompareBuffer[COMPARE_BLOCK_SIZE];
    MAX_FILE_LENGTH compare_block_length;
    while (DataLength) {
        if (DataLength >= COMPARE_BLOCK_SIZE) {
            compare_block_length = COMPARE_BLOCK_SIZE;
        }
        else {
            compare_block_length = DataLength;
        }
        fnGetParsFile(ptrFile, ucCompareBuffer, compare_block_length);
        if (uMemcmp(ucCompareBuffer, ptrData, compare_block_length)) {
            return 1;                                                    // contents are different
        }
        ptrFile += compare_block_length;
        DataLength -= compare_block_length;
    }
    return 0;                                                            // contents are identical
    #endif
}
#endif

#endif
