/***********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher

    ---------------------------------------------------------------------
    File:      disk_loader.c
    Project:   uTasker disk loader
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2017
    *********************************************************************
    03.03.2012 Add TWR_K53N512                                           {1}
    19.09.2012 Close Flash buffer when device has flash with row writing {2}
    25.11.2013 Limit wait when no SD card detected                       {3}
    18.12.2013 Allow another loader method to stop application from starting {4}
    18.12.2013 Add peripheral resets when jumping to the application     {5}
    18.12.2013 Add file system entry after SD card loading if used together with USB-MSD {6}
    17.01.2014 Pass directory pointer to utFAT2.0 version of utOpenFile(){7}
    15.05.2014 Add optional SD card wildcard file name matching          {8}
    15.05.2014 Add decryption of SD card content                         {9}
    11.12.2014 Add check of oversize SD card file                        {10}
    21.01.2015 Add file object whenever USB-MSD is enabled               {11}
    02.07.2015 Allow existing application to start if no matching file is found and using wildcard matching {12}
    21.07.2015 Add option to start the application by pressing a button (to restart update check and to jump to the [new] application) {13}
    23.10.2015 Renamed from SDLoader.c to disk_loader.c
    07.01.2016 Use fnJumpToValidApplication() to start application       {14}
    20.09.2017 Correct decryption when offset is larger than the file length {15}

*/

/* =================================================================== */
/*                           include files                             */
/* =================================================================== */

#include "config.h"

#if defined SDCARD_SUPPORT || defined SPI_FLASH_FAT || defined USB_MSD_HOST_LOADER

/* =================================================================== */
/*                          local definitions                          */
/* =================================================================== */

#define OWN_TASK                   TASK_SD_LOADER

#define STATE_INIT                 0                                     // task states
#define STATE_ACTIVE               1
#define STATE_START_CHECKING       2
#define STATE_CHECKING             3
#define STATE_CHECK_SECRET_KEY     4
#define STATE_DELETING_FLASH       5
#define STATE_PROGRAMMING          6
#define STATE_VERIFYING            7


#define T_JUMP_TO_APP              1
#define T_CHECK_CARD               2

#define E_DO_NEXT                  1
#define E_DO_SEARCH                2
#define E_RECHECK_SW               3

#define CRC_BLOCK_SIZE             1024                                  // CRC check in internal flash made in blocks of this size
#define COPY_BUFFER_SIZE           256                                   // file processed in blocks of this size from SD card

#if defined USB_MSD_HOST_LOADER
    #define LOADING_DISK           DISK_E
#else
    #define LOADING_DISK           DISK_D
#endif

#if defined ENCRYPTED_CARD_CONTENT                                       // ensure length is compiler independent
    #define SIZE_OF_UPLOAD_HEADER (sizeof(unsigned long) + (3 * sizeof(unsigned short)))
#else
    #define SIZE_OF_UPLOAD_HEADER (sizeof(unsigned long) + (2 * sizeof(unsigned short)))
#endif

/* =================================================================== */
/*                      local structure definitions                    */
/* =================================================================== */

typedef struct _PACK stUPLOAD_HEADER
{
    unsigned long  ulCodeLength;
    unsigned short usMagicNumber;
    unsigned short usCRC;
#if defined ENCRYPTED_CARD_CONTENT                                       // {9}
    unsigned short usRAWCRC;
#endif
} UPLOAD_HEADER;

/* =================================================================== */
/*                 local function prototype declarations               */
/* =================================================================== */

#if defined ENCRYPTED_CARD_CONTENT                                       // {9}
    static unsigned short fnDecrypt(unsigned char *ptrData, unsigned short usBlockSize);
#endif
static int fnUpdateSoftware(int iAppState, UTFILE *ptr_utFile, UPLOAD_HEADER *ptrFile_header);
#if defined (_WINDOWS) || defined (_LITTLE_ENDIAN)
    static void fnHeaderToLittleEndian(UPLOAD_HEADER *file_header);
#endif
#if defined WILDCARD_FILES                                               // {8}
    static int fnWildcardMatch(CHAR *cString, CHAR *cPattern);
#endif
#if defined START_ON_INTERRUPT
    static void fnEnableRepeatInterrupt(void);                           // {13}
#endif
static void fnJumpToApplication(int iGo);                                // {3}

/* =================================================================== */
/*                             constants                               */
/* =================================================================== */

static const unsigned char ucSecretKey[] = _SECRET_KEY;

/* =================================================================== */
/*                     global variable definitions                     */
/* =================================================================== */

/* =================================================================== */
/*                      local variable definitions                     */
/* =================================================================== */

#if defined FLASH_ROW_SIZE && FLASH_ROW_SIZE > 0
    static unsigned char ucCodeStart[FLASH_ROW_SIZE];
#else
    static unsigned char ucCodeStart[4] = {0xff, 0xff, 0xff, 0xff};
#endif
#if defined USB_INTERFACE && defined USB_MSD_DEVICE_LOADER
    static FILE_OBJECT_INFO fileObjInfo = {0};                           // {6}
#endif
#if defined WILDCARD_FILES && defined DELETE_SDCARD_FILE_AFTER_UPDATE
    static CHAR cSWfile[COPY_BUFFER_SIZE];
#endif
#if defined START_ON_INTERRUPT
    static int iStartRequested = 0;                                      // {13}
#endif
static UTDIRECTORY  *ptr_utDirectory = 0;                                // pointer to a directory object


// Application task
//
extern void fnSD_loader(TTASKTABLE *ptrTaskTable)
{
    #if defined WILDCARD_FILES                                           // {8}
    static int iResult = 0;
    static UTLISTDIRECTORY utListDirectory = {0};                        // list directory object for directory search
    static FILE_LISTING fileList = {0};
    static CHAR cBuffer[MAX_UTFAT_FILE_NAME + DOS_STYLE_LIST_ENTRY_LENGTH]; // buffer for listing string
    #endif
    static int iAppState = STATE_INIT;                                   // task state
    static unsigned char ucInputMessage[HEADER_LENGTH];                  // reserve space for receiving messages
    static UPLOAD_HEADER file_header;
    static UTFILE        utFile;                                         // local file object
    QUEUE_HANDLE         PortIDInternal = ptrTaskTable->TaskID;          // queue ID for task input

    if (STATE_INIT == iAppState) {
        iAppState = STATE_ACTIVE;
    #if defined FLASH_ROW_SIZE && FLASH_ROW_SIZE > 0
        uMemset(ucCodeStart, 0xff, sizeof(ucCodeStart));
    #endif
    #if (defined KWIKSTIK || defined TWR_K40X256 || defined TWR_K53N512 || defined TWR_KL43Z48M || defined FRDM_KL43Z || defined TWR_K40D100M) && !defined SERIAL_INTERFACE && !defined USB_INTERFACE // {1}
        CONFIGURE_SLCD();
    #endif
    #if defined USB_MSD_HOST_LOADER
        uTaskerStateChange(TASK_USB_HOST, UTASKER_ACTIVATE);             // allow the USB host task to start
        #if defined MEMORY_STICK_TASK
            uTaskerMonoTimer(OWN_TASK, (DELAY_LIMIT)(SEC * 5), T_JUMP_TO_APP); // maximum wait for memory stick to mount
        #else
            uTaskerMonoTimer(OWN_TASK, (DELAY_LIMIT)(SEC * 1), T_CHECK_CARD); // delay to allow memory stick to be mounted
        #endif
    #else
        uTaskerMonoTimer(OWN_TASK, (DELAY_LIMIT)(SEC * 1), T_CHECK_CARD);// delay to allow SD card to be mounted
    #endif
        ptr_utDirectory = utAllocateDirectory(LOADING_DISK, 0);          // allocate a directory for use by this module associated with D: without path name string length
    }
    else {
        while (fnRead(PortIDInternal, ucInputMessage, HEADER_LENGTH) != 0) { // check input queue
            if (ucInputMessage[MSG_SOURCE_TASK] == TIMER_EVENT) {        // timer event
                if (T_CHECK_CARD == ucInputMessage[MSG_TIMER_EVENT]) {   // check whether the disk is ready
                    const UTDISK *ptrDiskInfo = fnGetDiskInfo(LOADING_DISK);
    #if defined IGNORE_SD_CARD
                    if ((IGNORE_SD_CARD()) || (ptrDiskInfo->usDiskFlags == 0) || (ptrDiskInfo->usDiskFlags & DISK_NOT_PRESENT))
    #else
                    if ((ptrDiskInfo->usDiskFlags == 0) || (ptrDiskInfo->usDiskFlags & DISK_NOT_PRESENT)) // no card detected
    #endif
                    {
                        _DISPLAY_SD_CARD_NOT_PRESENT();                  // optionally display that the card is not present
                        fnJumpToApplication(0);                          // {3} jump to application if retries expire (restart timer when not yet true)
                    }
                    else if (ptrDiskInfo->usDiskFlags & DISK_UNFORMATTED) {
                        _DISPLAY_SD_CARD_NOT_FORMATTED();                // optionally display that SD card is not formatted
                        fnJumpToApplication(0);                          // {3} jump to application if retries expire (restart timer when not yet true)
                    }
                    else {                                               // formatted disk detected
                        utFile.ownerTask = OWN_TASK;
                        _DISPLAY_SD_CARD_PRESENT();                      // optionally display that card is ready
    #if defined WILDCARD_FILES                                           // {8}
                        if (utOpenDirectory("/", ptr_utDirectory) >= UTFAT_SUCCESS) { // open the root directory where we expect to find software
                            fileList.usMaxItems = 1;                     // one item at a time
                            fileList.ptrBuffer = cBuffer;                // the string buffer to create the listing in
                            fileList.usBufferLength = sizeof(cBuffer);   // buffer size
                            fileList.ucStyle = (DOS_TYPE_LISTING | NO_CR_LF_LISTING | NULL_TERMINATE_LISTING);

                            utListDirectory.ptr_utDirObject = ptr_utDirectory; // reference the list directory to the main directory object
                            utLocateDirectory("/", &utListDirectory);
                            fnInterruptMessage(OWN_TASK, E_DO_SEARCH);   // schedule scan through directory items
                            return;
                        }
    #else
                        if (utOpenFile(NEW_SOFTWARE_FILE, &utFile, ptr_utDirectory, UTFAT_OPEN_FOR_READ) == UTFAT_PATH_IS_FILE) { // {7}
                            if (utFile.ulFileSize > (MAX_UTASKER_APP_SIZE + SIZE_OF_UPLOAD_HEADER)) { // {10} if the file is too large we ignore it
                                _DISPLAY_OVERSIZE_CONTENT();
                            }
                            else {
                                utReadFile(&utFile, (unsigned char *)&file_header, SIZE_OF_UPLOAD_HEADER); // read the file header
        #if defined (_WINDOWS) || defined (_LITTLE_ENDIAN)
                                fnHeaderToLittleEndian(&file_header);
        #endif
                                if (utFile.ulFileSize == (file_header.ulCodeLength + SIZE_OF_UPLOAD_HEADER)) { // content length matches
                                    if ((file_header.usMagicNumber == VALID_VERSION_MAGIC_NUMBER)) { // check test that the header version (magic number) is correct
        #if defined USB_INTERFACE && defined USB_MSD_DEVICE_LOADER
                                        fileObjInfo.usCreationDate = utFile.usCreationDate;
                                        fileObjInfo.usCreationTime = utFile.usCreationTime;
        #endif
                                        iAppState = STATE_START_CHECKING;
                                        iAppState = fnUpdateSoftware(iAppState, &utFile, &file_header);
                                        return;
                                    }
                                    else {
                                        _DISPLAY_INVALID_CONTENT();          // optionally display that the software file has been found but has invalid content
                                    }
                                }
                                else {
                                    _DISPLAY_INVALID_CONTENT();              // optionally display that the software file has been found but has invalid content
                                }
                            }
                        }
                        else {
                            _DISPLAY_NO_FILE();                          // optionally display that the software file is not present on the SD card
                        }
    #endif
                        uTaskerMonoTimer(OWN_TASK, (DELAY_LIMIT)(SEC * 1), T_JUMP_TO_APP); // {3} allow an existing application to start after short delay to display the state
                    }
                }
                else if (T_JUMP_TO_APP == ucInputMessage[MSG_TIMER_EVENT]) { // go to application
                    fnJumpToApplication(1);                              // {3} unconditional jump to application code
                }
            }
            else {                                                       // interrupt event assumed
    #if defined WILDCARD_FILES                                           // {8}
                if (E_DO_SEARCH == ucInputMessage[MSG_INTERRUPT_EVENT]) {
                     if (utListDir(&utListDirectory, &fileList) != UTFAT_NO_MORE_LISTING_ITEMS_FOUND) { // search through all files in the root directory
                        if (fileList.ucFileAttributes & DIR_ATTR_ARCHIVE) { // if a file
                            CHAR *ptrFileName = &cBuffer[fileList.usStringLength - fileList.ucNameLength]; // set pointer to null terminated file name
                            if (fnWildcardMatch(ptrFileName, NEW_SOFTWARE_FILE) == 0) { // if the name matches
                                iResult = 1;                             // note that a file has been found with a valid name
                                if (utOpenFile(ptrFileName, &utFile, ptr_utDirectory, UTFAT_OPEN_FOR_READ) == UTFAT_PATH_IS_FILE) { // open the file
                                    if (utFile.ulFileSize > (MAX_UTASKER_APP_SIZE + SIZE_OF_UPLOAD_HEADER)) { // {10} if the file is too large we ignore it
                                        _DISPLAY_OVERSIZE_CONTENT();
                                    }
                                    else {
                                        utReadFile(&utFile, (unsigned char *)&file_header, SIZE_OF_UPLOAD_HEADER); // read the file header
        #if defined (_WINDOWS) || defined (_LITTLE_ENDIAN)
                                        fnHeaderToLittleEndian(&file_header);
        #endif
                                        if (utFile.ulFileSize == (file_header.ulCodeLength + SIZE_OF_UPLOAD_HEADER)) { // content length matches
                                            if ((file_header.usMagicNumber == VALID_VERSION_MAGIC_NUMBER)) { // check test that the header version (magic number) is correct
        #if defined USB_INTERFACE && defined USB_MSD_DEVICE_LOADER
                                                fileObjInfo.usCreationDate = utFile.usCreationDate;
                                                fileObjInfo.usCreationTime = utFile.usCreationTime;
        #endif
                                                iAppState = STATE_START_CHECKING;
        #if defined DELETE_SDCARD_FILE_AFTER_UPDATE
                                                uStrcpy(cSWfile, ptrFileName); // make a copy of the file name so that it can be deleted later
        #endif
                                                iAppState = fnUpdateSoftware(iAppState, &utFile, &file_header);
                                                return;
                                            }
                                        }
                                    }
                                }
                            }
                        }
                        fnInterruptMessage(OWN_TASK, E_DO_SEARCH);       // schedule next item
                        return;
                    }
                    if (iResult != 0) {
                        _DISPLAY_INVALID_CONTENT();                      // optionally display that a software file was found but has invalid content
                    }
                    else {
                        _DISPLAY_NO_FILE();                              // optionally display that no matching software file is not present on the SD card
                    }
                    uTaskerMonoTimer(OWN_TASK, (DELAY_LIMIT)(SEC * 1), T_JUMP_TO_APP); // {12} allow an existing application to start after short delay to display the state
                    return;
                }
    #endif
    #if defined MEMORY_STICK_TASK
                if (MEM_STICK_MOUNTED == ucInputMessage[MSG_INTERRUPT_EVENT]) { // the mass storage task is informing that the memory stick has been mounted
                    uTaskerMonoTimer(OWN_TASK, (DELAY_LIMIT)(0), T_CHECK_CARD);
                }
    #endif
    #if defined START_ON_INTERRUPT
                if (E_RECHECK_SW == ucInputMessage[MSG_INTERRUPT_EVENT]) { // {13}
                    if (iAppState == STATE_ACTIVE) {                     // only accept when idle
                        iStartRequested = 1;                             // always start application after the next check
                        uTaskerMonoTimer(OWN_TASK, (DELAY_LIMIT)(0.1 * SEC), T_CHECK_CARD); // start the check for new SW and start the application
                    }
                    break;
                }
    #endif
                iAppState = fnUpdateSoftware(iAppState, &utFile, &file_header);
                return;
            }
        }
    }
}

#if defined WILDCARD_FILES                                               // {8}
// Perform a single character match, allowing small and capitals to be equal
//
static int fnMatchCharacters(CHAR cChar1, CHAR cChar2)
{
    if (cChar1 != cChar2) {                                              // check for character match
        if ((cChar1 >= 'A') && (cChar1 <= 'Z')) {                        // capital letter
            if (cChar1 != (cChar2 - ('a' - 'A'))) {                      // allow capital letters and small letters to be matched
                return -1;                                               // failed
            }
        }
        else if ((cChar1 >= 'a') && (cChar1 <= 'z')) {                   // capital letter
            if (cChar1 != (cChar2 + ('a' - 'A'))) {                      // allow capital letters and small letters to be matched
                return -1;                                               // failed
            }
        }
        else {
            return -1;                                                   // failed
        }
    }
    return 0;                                                            // match
}

// Perform a wildcard string match
//
static int fnWildcardMatch(CHAR *cString, CHAR *cPattern)
{
    while (*cPattern != 0) {                                             // until the end of the pattern is reached
        switch (*cPattern) {                                             // letter of fixed string to be matched with
        case '?':                                                        // single wildcard - always accept and move over single wildcard
            if (*cString == 0) {                                         // don't match if the string is shorter
                return -1;
            }
            break;
        case '*':                                                        // wildcard (zero, one or multiple characters could be replaced)
            do {
                if (fnWildcardMatch(cString, (cPattern + 1)) == 0) {
                    return 0;                                            // match after wildcard is successful
                }
            } while (*cString++ != 0);
            return -1;                                                   // failed
        default:
            if (fnMatchCharacters(*cPattern, *cString) != 0) {           // case insensitive match
                return -1;                                               // difference found and so failed
            }
            break;
        }
        cString++;
        cPattern++;                                                      // move to next letter
    }
    if (*cString != 0) {                                                 // string too long
        return -1;                                                       // failed
    }
    return 0;                                                            // successfully matched
}
#endif

static void fnJumpToApplication(int iGo)                                 // {3}
{
    static int iMaxWait = 0;

    if ((iGo != 0) || (++iMaxWait >= MAX_WAIT_SD_CARD)) {                // if unconditional or if maximum attempts has been reached
#if defined SERIAL_INTERFACE || defined USB_INTERFACE                    // {4} if the SD card loader is operating in parallel with UART or USB-MSD loader, which wants to retain the mode
        if (RETAIN_LOADER_MODE()) {                                      // if the retain loader input is true (this will usually be unconditionally true when working with START_ON_INTERRUPT) 
    #if defined START_ON_INTERRUPT                                       // {13}
            if (iStartRequested == 0) {
                fnEnableRepeatInterrupt();                               // the user can now press the button to repeat the loading process and unconditionally restart the board
                return;                                                  // remain in the loader mode
            }
    #else
            return;                                                      // remain in loader mode if another loader method is forcing it
    #endif
        }
#endif
        fnJumpToValidApplication(1);                                     // {14}
        return;
    }
    uTaskerMonoTimer(OWN_TASK, (DELAY_LIMIT)(SEC * 1), T_CHECK_CARD);    // try again later
}

// SD card update state-event machine
//
static int fnUpdateSoftware(int iAppState, UTFILE *ptr_utFile, UPLOAD_HEADER *ptrFile_header)
{
    #if defined ENCRYPTED_CARD_CONTENT                                   // {9}
    static unsigned long  ulCodeOffset = 0;
    static unsigned long  ulProgrammed = 0;
    static UTFILE         utFile_decrypt = {0};
    #endif
    static unsigned short usCRC = 0;
    static int            iFlashMismatch = 0;
    static unsigned char  *ptrInternalFlash;
    static unsigned long  ulFileLength;
    int                   iNextState = iAppState;
    #if defined ENCRYPTED_CARD_CONTENT
    MAX_FILE_LENGTH       toProgram;
    #endif
    unsigned char         ucBuffer[COPY_BUFFER_SIZE];

    switch (iAppState) {
    case STATE_START_CHECKING:
    #if defined USB_INTERFACE && defined USB_MSD_DEVICE_LOADER
        fileObjInfo.ptrLastAddress = 0;                                  // {6}
    #endif
        iFlashMismatch = 0;
        usCRC = 0;
        ptrInternalFlash = (unsigned char *)_UTASKER_APP_START_;
    #if defined ENCRYPTED_CARD_CONTENT                                   // {9}
        uMemcpy(&utFile_decrypt, ptr_utFile, sizeof(utFile_decrypt));    // copy the file object for decryption use
        ulCodeOffset = CODE_OFFSET;
        while (ulCodeOffset >= ptrFile_header->ulCodeLength) {
            ulCodeOffset -= ptrFile_header->ulCodeLength;
        }
        utSeek(&utFile_decrypt, (SIZE_OF_UPLOAD_HEADER + ulCodeOffset), UTFAT_SEEK_SET); // {15} move to the start of the encrypted code
        fnDecrypt(0, 0);                                                 // reset decryptor
    #endif
        iNextState = STATE_CHECKING;
        // Fall-through intentional
        //
    case STATE_CHECKING:                                                 // check the CRC of the file on the SD card
        utReadFile(ptr_utFile, ucBuffer, sizeof(ucBuffer));              // read a single buffer from the file on the SD card
        usCRC = fnCRC16(usCRC, ucBuffer, ptr_utFile->usLastReadWriteLength); // calculate the CRC of complete file content
        if (iFlashMismatch == 0) {                                       // if the code still matches
    #if defined ENCRYPTED_CARD_CONTENT                                   // {9}
            utReadFile(&utFile_decrypt, ucBuffer, sizeof(ucBuffer));     // read a single buffer from the file on the SD card
            if (utFile_decrypt.ulFilePosition >= utFile_decrypt.ulFileSize) { // end of file reached
                unsigned short usRemaining = (ptr_utFile->usLastReadWriteLength - utFile_decrypt.usLastReadWriteLength);
                utSeek(&utFile_decrypt, SIZE_OF_UPLOAD_HEADER, UTFAT_SEEK_SET); // move back to start of code in file
                utReadFile(&utFile_decrypt, &ucBuffer[utFile_decrypt.usLastReadWriteLength], usRemaining); // read a single buffer from the file on the SD card
            }
            fnDecrypt(ucBuffer, ptr_utFile->usLastReadWriteLength);      // decrypt the buffer before comparing
    #endif
            if (uMemcmp(fnGetFlashAdd(ptrInternalFlash), ucBuffer, ptr_utFile->usLastReadWriteLength) != 0) { // check whether the code is different
                iFlashMismatch = 1;                                      // the code is different so needs to be updated
            }
            else {
                ptrInternalFlash += ptr_utFile->usLastReadWriteLength;
            }
        }
        if (ptr_utFile->usLastReadWriteLength != sizeof(ucBuffer)) {     // end of file reached
            iNextState = STATE_CHECK_SECRET_KEY;
        }
        fnInterruptMessage(OWN_TASK, E_DO_NEXT);                         // schedule next
        break;

    case STATE_CHECK_SECRET_KEY:
        usCRC = fnCRC16(usCRC, (unsigned char *)ucSecretKey, sizeof(ucSecretKey)); // add the secret key
        if (usCRC == ptrFile_header->usCRC) {                            // content is valid
            _DISPLAY_VALID_CONTENT();                                    // optionally display that the content is valid
            if (iFlashMismatch != 0) {                                   // valid new code which needs to be programmed
                fnEraseFlashSector((unsigned char *)UTASKER_APP_START, (UTASKER_APP_END - (unsigned char *)UTASKER_APP_START)); // delete application space
                iNextState = STATE_DELETING_FLASH;
                fnInterruptMessage(OWN_TASK, E_DO_NEXT);                 // schedule next
            }
            else {
                _DISPLAY_SW_OK();                                        // optionally display that the programmed application is already up-to-date
    #if defined DELETE_SDCARD_FILE_AFTER_UPDATE
        #if defined WILDCARD_FILES
                utDeleteFile((const CHAR *)cSWfile, ptr_utDirectory);    // delete the file from the disk since it is the same as the loaded software
        #else
                utDeleteFile(NEW_SOFTWARE_FILE, ptr_utDirectory);        // delete the file from the disk since it is the same as the loaded software
        #endif
    #endif
                uTaskerMonoTimer(OWN_TASK, (DELAY_LIMIT)(SEC * 1.5), T_JUMP_TO_APP); // start the application after a delay
                iNextState = STATE_ACTIVE;
            }
        }
        else {
            _DISPLAY_INVALID_CONTENT();                                  // optionally display that the content is not valid
            uTaskerMonoTimer(OWN_TASK, (DELAY_LIMIT)(SEC * 1), T_JUMP_TO_APP); // {3} start the existing application after a delay
            iNextState = STATE_ACTIVE;
        }
        break;

    case STATE_DELETING_FLASH:                                           // flash deleted
        ptrInternalFlash = (unsigned char *)_UTASKER_APP_START_;
    #if defined ENCRYPTED_CARD_CONTENT                                   // {9}
        ulProgrammed = sizeof(ucCodeStart);
        utSeek(ptr_utFile, (SIZE_OF_UPLOAD_HEADER + ulCodeOffset), UTFAT_SEEK_SET); // {15} return to the start of the encrypted code
        utReadFile(ptr_utFile, ucCodeStart, sizeof(ucCodeStart));        // store code start for programming as final step
        fnDecrypt(0, 0);                                                 // reset decryptor
        fnDecrypt(ucCodeStart, sizeof(ucCodeStart));
    #else
        utSeek(ptr_utFile, SIZE_OF_UPLOAD_HEADER, UTFAT_SEEK_SET);       // return to the start of the file after the header
        utReadFile(ptr_utFile, ucCodeStart, sizeof(ucCodeStart));        // store code start for programming as final step
    #endif
        ptrInternalFlash += sizeof(ucCodeStart);
        iNextState = STATE_PROGRAMMING;
        // Fall-through intentional
        //
    case STATE_PROGRAMMING:                                              // programming from SD card file to internal application space
        utReadFile(ptr_utFile, ucBuffer, sizeof(ucBuffer));              // read buffer from file
    #if defined ENCRYPTED_CARD_CONTENT                                   // {9}
        fnDecrypt(ucBuffer, ptr_utFile->usLastReadWriteLength);          // decrypt the buffer before programming
        ulProgrammed += ptr_utFile->usLastReadWriteLength;
        if (ulProgrammed > ptrFile_header->ulCodeLength) {
            toProgram = (ptr_utFile->usLastReadWriteLength - (ulProgrammed - ptrFile_header->ulCodeLength)); // final content length
        }
        else {
            toProgram = ptr_utFile->usLastReadWriteLength;
        }
        fnWriteBytesFlash(ptrInternalFlash, ucBuffer, toProgram);        // program to Flash
        ptrInternalFlash += ptr_utFile->usLastReadWriteLength;
        if (ptr_utFile->ulFilePosition >= ptr_utFile->ulFileSize) {      // end of file reached
            utSeek(ptr_utFile, SIZE_OF_UPLOAD_HEADER, UTFAT_SEEK_SET);   // move back to start of code in file
            ptr_utFile->usLastReadWriteLength = sizeof(ucBuffer);        // avoid terminating
        }
        if (ulProgrammed >= ptrFile_header->ulCodeLength) {
            ptr_utFile->usLastReadWriteLength = 0;
        }
    #else
        fnWriteBytesFlash(ptrInternalFlash, ucBuffer, ptr_utFile->usLastReadWriteLength); // program to Flash
        ptrInternalFlash += ptr_utFile->usLastReadWriteLength;
    #endif
        if (ptr_utFile->usLastReadWriteLength != sizeof(ucBuffer)) {     // end of file reached
    #if defined FLASH_ROW_SIZE && FLASH_ROW_SIZE > 0                     // {2}
            fnWriteBytesFlash(ptrInternalFlash, 0, 0);                   // close any outstanding FLASH buffer from end of the file
    #endif
    #if defined USB_INTERFACE && defined USB_MSD_DEVICE_LOADER
            fileObjInfo.ptrLastAddress = ptrInternalFlash;               // save information about the length of data written
    #endif
            ptrInternalFlash = (unsigned char *)_UTASKER_APP_START_;
            fnWriteBytesFlash(ptrInternalFlash, ucCodeStart, sizeof(ucCodeStart)); // program start of code to Flash
    #if defined FLASH_ROW_SIZE && FLASH_ROW_SIZE > 0                     // {2}
            fnWriteBytesFlash(ptrInternalFlash, 0, 0);                   // close any outstanding FLASH buffer from write to start of code
    #endif
    #ifdef USB_INTERFACE__                                               // the USB loader expects that the first long word in application flash to be non-blank so ensure it is the case
            fnWriteBytesFlash((unsigned char *)UTASKER_APP_START, ucCodeStart, sizeof(ucCodeStart));
        #if defined FLASH_ROW_SIZE && FLASH_ROW_SIZE > 0                 // {2}
            fnWriteBytesFlash((unsigned char *)UTASKER_APP_START, 0, 0); // close any outstanding FLASH buffer from previous write
        #endif
    #endif
            usCRC = 0;
            iNextState = STATE_VERIFYING;
            ulFileLength = (ptr_utFile->ulFileSize - SIZE_OF_UPLOAD_HEADER);
        }
        fnInterruptMessage(OWN_TASK, E_DO_NEXT);                         // schedule next
        break;

    case STATE_VERIFYING:                                                // verify the CRC of the new application in flash
        if (ulFileLength >= CRC_BLOCK_SIZE) {
            usCRC = fnCRC16(usCRC, fnGetFlashAdd(ptrInternalFlash), CRC_BLOCK_SIZE);
            ulFileLength -= CRC_BLOCK_SIZE;
            ptrInternalFlash += CRC_BLOCK_SIZE;
            fnInterruptMessage(OWN_TASK, E_DO_NEXT);                     // schedule next
        }
        else {
            usCRC = fnCRC16(usCRC, fnGetFlashAdd(ptrInternalFlash), ulFileLength); // last block
            usCRC = fnCRC16(usCRC, (unsigned char *)ucSecretKey, sizeof(ucSecretKey)); // add the secret key
    #if defined ENCRYPTED_CARD_CONTENT                                   // {9}
            if (usCRC == ptrFile_header->usRAWCRC)
    #else
            if (usCRC == ptrFile_header->usCRC)
    #endif
            {                                                            // new code has been verified as being correct
    #if defined USB_INTERFACE && defined USB_MSD_DEVICE_LOADER && !defined USE_USB_MSD // {6}{11}
                fnAddSREC_file(&fileObjInfo);                            // set file entry compatible with USB-MSD
    #endif
    #if defined DELETE_SDCARD_FILE_AFTER_UPDATE
        #if defined WILDCARD_FILES
                utDeleteFile((const CHAR *)cSWfile, ptr_utDirectory);    // delete the file from the disk since it has been successfully loaded
        #else
                utDeleteFile(NEW_SOFTWARE_FILE, ptr_utDirectory);        // delete the file from the disk since it has been successfully loaded
        #endif
    #endif
    #if defined START_ON_INTERRUPT
                iStartRequested = 1;                                     // {13} allow the application to start after an update
    #endif
                _DISPLAY_SW_UPDATED();                                   // optionally display that the code was successfully programmed
                uTaskerMonoTimer(OWN_TASK, (DELAY_LIMIT)(SEC * 1.5), T_JUMP_TO_APP); // start the application after a short delay
            }
            else {
                _DISPLAY_ERROR();                                        // optionally display that there was an error with the image loaded to flash
                uTaskerMonoTimer(OWN_TASK, (DELAY_LIMIT)(SEC * 4), T_CHECK_CARD); // try again or allow application to start after a further check
            }
            iNextState = STATE_ACTIVE;
        }
        break;
    }
    return iNextState;
}

#if defined (_WINDOWS) || defined (_LITTLE_ENDIAN)
static void fnHeaderToLittleEndian(UPLOAD_HEADER *file_header)
{
    unsigned short usShort;
    unsigned long  ulLong;

    #if defined ENCRYPTED_CARD_CONTENT                                   // {9}
    usShort = (file_header->usRAWCRC >> 8);
    file_header->usRAWCRC <<= 8;
    file_header->usRAWCRC |= usShort;
    #endif

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
}
#endif

#if defined ENCRYPTED_CARD_CONTENT                                       // {9}
// Decode a block of data
//
static unsigned short fnDecrypt(unsigned char *ptrData, unsigned short usBlockSize)
{
    static int iCycleLength = 0;
    static unsigned short usPRNG = 0;
    if (ptrData == 0) {
        iCycleLength = 0;
        usPRNG = KEY_PRIME;
        return 0;
    }

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

#if defined START_ON_INTERRUPT                                           // {13}
// Interrupt handler
//
static void fnRepeatCheck(void)                                          // button has been pressed
{
    fnInterruptMessage(OWN_TASK, E_RECHECK_SW);                          // cause the SD card checking process to be repeated
}

// Configure an interrupt on an input to detect a button being pressed
//
static void fnEnableRepeatInterrupt(void)
{
    INTERRUPT_SETUP interrupt_setup;                                     // interrupt configuration parameters
    interrupt_setup.int_type       = PORT_INTERRUPT;                     // identifier to configure port interrupt
    interrupt_setup.int_handler    = fnRepeatCheck;                      // handling function
    interrupt_setup.int_priority   = 1;                                  // interrupt priority level (low priority since not critical)
    interrupt_setup.int_port       = BUTTON_PORT;                        // the port that the interrupt input is on
    interrupt_setup.int_port_bits  = BUTTON_INPUT;                       // the interrupt input
    interrupt_setup.int_port_sense = BUTTON_INTERRUPT_EDGE;              // interrupt edge sensitivity
    fnConfigureInterrupt((void *)&interrupt_setup);                      // configure interrupt
}
#endif
#endif

