/**********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher

    ---------------------------------------------------------------------
    File:      stm32_FLASH.h
    Project:   Single Chip Embedded Internet
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2019
    *********************************************************************
    20.01.2017 Add 2MByte Flash support                                  {1}
    28.11.2018 Add fnSetFlashOption()                                    {2}

*/

/* =================================================================== */
/*                           include files                             */
/* =================================================================== */

/* =================================================================== */
/*                          local definitions                          */
/* =================================================================== */

#if defined _STM32F103X
    #define _DISABLE_INTERRUPTS()   uEnable_Interrupt()
    #define _ENABLE_INTERRUPTS()    uDisable_Interrupt()
#else
    #define _DISABLE_INTERRUPTS()
    #define _ENABLE_INTERRUPTS()
#endif

/* =================================================================== */
/*                       local structure definitions                   */
/* =================================================================== */

/* =================================================================== */
/*                global function prototype declarations               */
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

#if defined _WINDOWS
    static unsigned long ulFlashLockState = 0;
#endif

/* =================================================================== */
/*                     global variable definitions                     */
/* =================================================================== */

/* =================================================================== */
/*                          FLASH Interface                            */
/* =================================================================== */

#if (defined SPI_FILE_SYSTEM || defined SPI_SW_UPLOAD)
static void fnConfigSPIFileSystem(void)
{
    POWER_UP_SPI_FLASH_INTERFACE();
    CONFIGURE_SPI_FLASH_INTERFACE();
}
#endif

#if defined FLASH_OPTCR
// This routine operates in RAM
//
static void fnExecuteCommand(_STM32_FMI *ptrFMI, _STM32_IWDG *ptrIWDG, unsigned long ulOption)
{
    ptrFMI->_FLASH_OPTCR = ulOption;                                     // command the change
    while ((ptrFMI->_FLASH_SR & FLASH_SR_BSY) != 0) {                    // wait until the change has been committed
      //TOGGLE_WATCHDOG_LED();                                           // indicate that it is waiting for the flash to be erased and the command to complete
        ptrIWDG->_IWDG_KR = IWDG_KR_RETRIGGER;                           // ensure that the watchdog doesn't fire while waiting
    #if defined _WINDOWS
        ptrFMI->_FLASH_SR &= ~(FLASH_SR_BSY);
    #endif
    }
}

// Routine to set new flash options (write protections, brown-out reset level, etc.)
//
extern void fnSetFlashOption(unsigned long ulOption, unsigned long ulOption1, unsigned long ulMask) // {2}
{
    ulOption &= ~(ulMask);                                               // mask out the flags that are not to be modified
    ulOption |= (FLASH_OPTCR & ulMask);
#if !defined ALLOW_SECURING_DEBUG_ACCESS
    if ((ulOption & FLASH_OPTCR_RDP_MASK) == FLASH_OPTCR_RDP_LEVEL_2) {  // if the option would set read protection level 2 (cannot be lowered again and loses debug control forever)
        ulOption &= ~(FLASH_OPTCR_RDP_MASK);
        ulOption |= (FLASH_OPTCR & FLASH_OPTCR_RDP_MASK);                // keep present read protection level setting
    }
#endif
    ulOption &= FLASH_OPTCR_SETTING_MASK;
#if defined FLASH_OPTION_SETTING_1 && defined FLASH_OPTCR1
    ulOption1 &= FLASH_OPTCR1_SETTING_MASK;
#endif
    if (
        ((FLASH_OPTCR & FLASH_OPTCR_SETTING_MASK) != ulOption)
    #if defined FLASH_OPTCR1
        || ((FLASH_OPTCR1 & FLASH_OPTCR1_SETTING_MASK) != ulOption1)
    #endif
        )
    {                                                                    // only save new setting when it is not already valid
        FLASH_OPTKEYR = FLASH_OPTKEYR_KEY1;                              // unlock option register(s)
        FLASH_OPTKEYR = FLASH_OPTKEYR_KEY2;
    #if defined FLASH_OPTION_SETTING_1 && defined FLASH_OPTCR1
        FLASH_OPTCR1 = ulOption1;
    #endif
        if (((FLASH_OPTCR & FLASH_OPTCR_RDP_MASK) != FLASH_OPTCR_RDP_LEVEL_2) && ((FLASH_OPTCR & FLASH_OPTCR_RDP_MASK) != FLASH_OPTCR_RDP_LEVEL_0)) {
            if ((ulOption & FLASH_OPTCR_RDP_MASK) == FLASH_OPTCR_RDP_LEVEL_0) {
                // Special case where we are in read protection level 1 and want to move back to level 0
                // This triggers a mass erase of the flash before the command is accepted so we must run in SRAM and retrigger the watchdog so that we don't reset before it completes
                //
                #define PROG_WORD_SIZE 30                                // adequate space for the small program
                unsigned short usProgSpace[PROG_WORD_SIZE];              // make space for the routine in static memory (this will have an even boundary)
                int i = 0;
                unsigned char *ptrThumb2 = (unsigned char *)fnExecuteCommand;
                void (*fnRAM_code)(_STM32_FMI *, _STM32_IWDG *, unsigned long);
                FLASH_OPTCR = ulOption;                                  // set the desired options
                ptrThumb2 = (unsigned char *)(((CAST_POINTER_ARITHMETIC)ptrThumb2) & ~0x1); // thumb 2 address
                while (i < PROG_WORD_SIZE) {                             // copy program to SRAM
                    usProgSpace[i++] = *(unsigned short *)ptrThumb2;
                    ptrThumb2 += sizeof(unsigned short);
                }
                ptrThumb2 = (unsigned char *)usProgSpace;
                ptrThumb2++;                                             // create a thumb 2 call
                fnRAM_code = (void(*)(_STM32_FMI *, _STM32_IWDG *, unsigned long))(ptrThumb2);
                uDisable_Interrupt();
    #if defined _WINDOWS
                fnExecuteCommand((_STM32_FMI *)FMI_BLOCK, (_STM32_IWDG *)IWDG_BLOCK, (ulOption | FLASH_OPTCR_OPTSTRT));
    #else
                fnRAM_code((_STM32_FMI *)FMI_BLOCK, (_STM32_IWDG *)IWDG_BLOCK, (ulOption | FLASH_OPTCR_OPTSTRT)); // this will never return
    #endif
            }
        }
        FLASH_OPTCR = ulOption;                                          // set the desired options
        FLASH_OPTCR = (ulOption | FLASH_OPTCR_OPTSTRT);                  // command the change
        while ((FLASH_SR & FLASH_SR_BSY) != 0) {                         // wait until the change has been committed
    #if defined _WINDOWS
            FLASH_SR &= ~(FLASH_SR_BSY);
    #endif
        }
        FLASH_OPTCR = (ulOption | FLASH_OPTCR_OPTLOCK);                  // lock the options register
    }
}
#endif

#if defined ACTIVE_FILE_SYSTEM || defined USE_PARAMETER_BLOCK
    #if defined SPI_FLASH_ENABLED
// This routine reads data from the defined device into a buffer. The access details inform of the length to be read (already limited to maximum possible length for the device)
// as well as the address in the specific device
//
static void fnReadSPI(ACCESS_DETAILS *ptrAccessDetails, unsigned char *ptrBuffer)
{
    #if !defined SPI_FLASH_SST25 && !defined SPI_FLASH_MX66L
    unsigned short usPageNumber = (unsigned short)(ptrAccessDetails->ulOffset/SPI_FLASH_PAGE_LENGTH); // the page the address is in
    unsigned short usPageOffset = (unsigned short)(ptrAccessDetails->ulOffset - (usPageNumber * SPI_FLASH_PAGE_LENGTH)); // offset in the page
    #endif

    #if defined SPI_FLASH_ST
    fnSPI_command(READ_DATA_BYTES, (unsigned long)((unsigned long)(usPageNumber << 8) | (usPageOffset)), _EXTENDED_CS ptrBuffer, ptrAccessDetails->BlockLength);
    #elif defined SPI_FLASH_SST25 || defined SPI_FLASH_MX66L
    fnSPI_command(READ_DATA_BYTES, ptrAccessDetails->ulOffset, _EXTENDED_CS ptrBuffer, ptrAccessDetails->BlockLength);
    #else                                                                // ATMEL
        #if SPI_FLASH_PAGE_LENGTH >= 1024
    fnSPI_command(CONTINUOUS_ARRAY_READ, (unsigned long)((unsigned long)(usPageNumber << 11) | (usPageOffset)), _EXTENDED_CS ptrBuffer, ptrAccessDetails->BlockLength);
        #elif SPI_FLASH_PAGE_LENGTH >= 512
    fnSPI_command(CONTINUOUS_ARRAY_READ, (unsigned long)((unsigned long)(usPageNumber << 10) | (usPageOffset)), _EXTENDED_CS ptrBuffer, ptrAccessDetails->BlockLength);
        #else
    fnSPI_command(CONTINUOUS_ARRAY_READ, (unsigned long)((unsigned long)(usPageNumber << 9) | (usPageOffset)), _EXTENDED_CS ptrBuffer, ptrAccessDetails->BlockLength);
        #endif
    #endif
}

// This routine writes data from a buffer to an area in SPI Flash (the caller has already defined the data to a particular area and device)
//
static void fnWriteSPI(ACCESS_DETAILS *ptrAccessDetails, unsigned char *ptrBuffer)
{
    MAX_FILE_LENGTH Length = ptrAccessDetails->BlockLength;
    unsigned long Destination = ptrAccessDetails->ulOffset;
    unsigned short usDataLength;
    #if defined SPI_FLASH_SST25
    int iMultipleWrites = 0;
    if (Length == 0) {
        return;                                                          // ignore if length is zero
    }
    if (Destination & 0x1) {                                             // start at odd SPI address, requires an initial byte write
        fnSPI_command(WRITE_ENABLE, 0, _EXTENDED_CS 0, 0);               // command write enable to allow byte programming
        fnSPI_command(BYTE_PROG, Destination++, _EXTENDED_CS ptrBuffer++, 1);// program last byte 
        if (--Length == 0) {                                             // single byte write so complete
            return ;
        }
    }
    fnSPI_command(WRITE_ENABLE, 0, _EXTENDED_CS 0, 0);                   // command write enable to allow programming
    #else
    unsigned short usPageNumber = (unsigned short)(Destination/SPI_FLASH_PAGE_LENGTH); // the page the address is in
    unsigned short usPageOffset = (unsigned short)(Destination - (usPageNumber * SPI_FLASH_PAGE_LENGTH)); // offset in the page
    #endif
    while (Length != 0) {
        usDataLength = (unsigned short)Length;
    #if defined SPI_FLASH_ST || defined SPI_FLASH_MX66L
        if (usDataLength > (SPI_FLASH_PAGE_LENGTH - usPageOffset)) {
            usDataLength = (SPI_FLASH_PAGE_LENGTH - usPageOffset);
        }
        fnSPI_command(WRITE_ENABLE, 0, _EXTENDED_CS 0, 0);               // write enable
        #if defined SPI_FLASH_W25Q || defined SPI_FLASH_S25FL1_K
        fnSPI_command(PAGE_PROG, ((usPageNumber * SPI_FLASH_PAGE_LENGTH)) | usPageOffset, _EXTENDED_CS ptrBuffer, usDataLength); // copy new content
        #else
        fnSPI_command(PAGE_PROG, (usPageNumber << 8) | usPageOffset, _EXTENDED_CS ptrBuffer, usDataLength);// copy new content
        #endif
        Length -= usDataLength;
        ptrBuffer += usDataLength;
        usPageNumber++;
        usPageOffset = 0;
    #elif defined SPI_FLASH_SST25
        #if defined SST25_A_VERSION
            #define AAI_LENGTH 1
        #else
            #define AAI_LENGTH 2
        #endif
        if (usDataLength > 1) {
            if (iMultipleWrites == 0) {
                fnSPI_command(AAI_WORD_PROG, Destination, _EXTENDED_CS ptrBuffer, AAI_LENGTH); // program 2 bytes (1 byte for A type)
                iMultipleWrites = 1;                                     // mark that we are in a AAI sequence
            }
            else {
                fnSPI_command(AAI_WORD_PROG, Destination, _EXTENDED_CS ptrBuffer, 0); // continue in AAI sequence - note that the data length is zero but this is used to differentiate - always pairs are written
            }
            Destination += AAI_LENGTH;
            Length -= AAI_LENGTH;
            ptrBuffer += AAI_LENGTH;
        }
        else {
            if (iMultipleWrites != 0) {
                fnSPI_command(WRITE_DISABLE, 0, _EXTENDED_CS 0, 0);      // first close AAI sequence
                fnSPI_command(WRITE_ENABLE, 0, _EXTENDED_CS 0, 0);       // command write enable to allow byte programming
            }
            fnSPI_command(BYTE_PROG, Destination, _EXTENDED_CS ptrBuffer, 1); // program last byte 
            break;               
        }
    #else                                                                // ATMEL
        if ((usPageOffset != 0) || (Length < SPI_FLASH_PAGE_LENGTH)) {   // are we writing a partial page?
            fnSPI_command(MAIN_TO_BUFFER_1, usPageNumber, _EXTENDED_CS 0, 0); // copy main memory to buffer
        }
        if (usDataLength > (SPI_FLASH_PAGE_LENGTH - usPageOffset)) {
            usDataLength = (SPI_FLASH_PAGE_LENGTH - usPageOffset);
        }
        fnSPI_command(WRITE_BUFFER_1, usPageOffset, _EXTENDED_CS ptrBuffer, usDataLength);// copy new content
        fnSPI_command(PROG_FROM_BUFFER_1, usPageNumber, _EXTENDED_CS 0, 0); // program to main memory
        Length -= usDataLength;
        ptrBuffer += usDataLength;
        usPageNumber++;
        usPageOffset = 0;
    #endif
    }
    #if defined SPI_FLASH_SST25
    fnSPI_command(WRITE_DISABLE, 0, _EXTENDED_CS 0, 0);                  // disable writes on exist
    #endif
}

// The routine is used to delete an area in SPI Flash, whereby the caller has set the address to the start of a page and limited the erase to a single storage area and device
//
static MAX_FILE_LENGTH fnDeleteSPI(ACCESS_DETAILS *ptrAccessDetails)
{
    MAX_FILE_LENGTH BlockLength = SPI_FLASH_PAGE_LENGTH;
    #if !defined SPI_FLASH_ST
    unsigned char   ucCommand;
    #endif
    #if !defined SPI_FLASH_SST25 && !defined SPI_FLASH_MX66L
    unsigned short usPageNumber = (unsigned short)(ptrAccessDetails->ulOffset/SPI_FLASH_PAGE_LENGTH); // the page the address is in
    #endif
    #if defined SPI_FLASH_ST
    fnSPI_command(WRITE_ENABLE, 0, _EXTENDED_CS 0, 0);                   // enable the write
        #if defined SPI_DATA_FLASH
    fnSPI_command(SUB_SECTOR_ERASE, ((unsigned long)usPageNumber << 8), _EXTENDED_CS 0, 0); // delete appropriate sub-sector
    BlockLength = SPI_FLASH_SUB_SECTOR_LENGTH;
        #else
    fnSPI_command(SECTOR_ERASE, ((unsigned long)usPageNumber << 8), _EXTENDED_CS 0, 0); // delete appropriate sector
    BlockLength = SPI_FLASH_SECTOR_LENGTH;
        #endif
    #elif defined SPI_FLASH_SST25 || defined SPI_FLASH_MX66L
    fnSPI_command(WRITE_ENABLE, 0, _EXTENDED_CS 0, 0);                   // command write enable to allow byte programming
        #if !defined SST25_A_VERSION
    if ((ptrAccessDetails->BlockLength >= SPI_FLASH_SECTOR_LENGTH) && ((ptrAccessDetails->ulOffset & (SPI_FLASH_SECTOR_LENGTH - 1)) == 0)) { // if a complete 64k sector can be deleted
        ucCommand = SECTOR_ERASE;                                        // delete block of 64k
        BlockLength = SPI_FLASH_SECTOR_LENGTH;
    }
    else 
        #endif
        #if defined SPI_FLASH_MX66L
    {
        ucCommand = SECTOR_ERASE;                                        // delete smallest sector of 4k
        BlockLength = SPI_FLASH_SECTOR_LENGTH;
    }
        #else
    if ((ptrAccessDetails->BlockLength >= SPI_FLASH_HALF_SECTOR_LENGTH) && ((ptrAccessDetails->ulOffset & (SPI_FLASH_HALF_SECTOR_LENGTH - 1)) == 0)) {
        ucCommand = HALF_SECTOR_ERASE;                                   // delete block of 32k
        BlockLength = SPI_FLASH_HALF_SECTOR_LENGTH;
    }
    else {
        ucCommand = SUB_SECTOR_ERASE;                                    // delete smallest sector of 4k
        BlockLength = SPI_FLASH_SUB_SECTOR_LENGTH;
    }
        #endif
    fnSPI_command(ucCommand, ptrAccessDetails->ulOffset, _EXTENDED_CS 0, 0);    
    #else                                                                // ATMEL
    if ((ptrAccessDetails->BlockLength >= SPI_FLASH_BLOCK_LENGTH) && (usPageNumber % 8 == 0)) { // if delete range corresponds to a block, use faster block delete
        BlockLength = SPI_FLASH_BLOCK_LENGTH;
        ucCommand = BLOCK_ERASE;
    }
    else {
        BlockLength = SPI_FLASH_PAGE_LENGTH;
        ucCommand = PAGE_ERASE;
    }
    fnSPI_command(ucCommand, usPageNumber, _EXTENDED_CS 0, 0);           // delete appropriate page/block
    #endif
    return (BlockLength);
}
    #endif  
#endif

    #if !defined _ST_FLASH_UNIFORM_GRANULARITY || defined _WINDOWS
// The STM32F2xx and STM32F4xx have variable flash granularity - this routine determines the size of the flash sector that the access is in as well as the sector's number
//
static unsigned long fnGetFlashSectorSize(unsigned char *ptrSector, unsigned long *ulSectorNumber, int *iProtected)
{
    unsigned long ulSectorSize;
    #if SIZE_OF_FLASH >= (2 * 1024 * 1024)                               // {1}
    if (ptrSector < (unsigned char *)(FLASH_START_ADDRESS + (1 * 1024 * 1024))) {
        *ulSectorNumber = 0;                                             // access in first bank
    }
    else {
        ptrSector -= (1 * 1024 * 1024);
        *ulSectorNumber = 16;                                            // access in second bank (note that the first sector in the second bank is named as sector 1 but it has to be addressed as sector 16!)
    }
    #else
    *ulSectorNumber = 0;
    #endif
    *iProtected = FLASH_SECTOR_NOT_PROTECTED;
    #if defined _ST_FLASH_UNIFORM_GRANULARITY
    ptrSector -= (FLASH_START_ADDRESS);
    *ulSectorNumber += (((CAST_POINTER_ARITHMETIC)ptrSector) / FLASH_GRANULARITY);
    ulSectorSize = FLASH_GRANULARITY;
    if ((FLASH_WRPR & (FLASH_WRPR_WRP0 << *ulSectorNumber)) == 0) {
        *iProtected = FLASH_SECTOR_PROTECTED;                            // sector is write protected
    }
    #else
    if (ptrSector >= (unsigned char *)(FLASH_START_ADDRESS + (NUMBER_OF_BOOT_SECTORS * FLASH_GRANULARITY_BOOT) + (NUMBER_OF_PARAMETER_SECTORS * FLASH_GRANULARITY_PARAMETER))) { // {22}
        ptrSector -= (FLASH_START_ADDRESS + (NUMBER_OF_BOOT_SECTORS * FLASH_GRANULARITY_BOOT) + (NUMBER_OF_PARAMETER_SECTORS * FLASH_GRANULARITY_PARAMETER));
        *ulSectorNumber += ((NUMBER_OF_BOOT_SECTORS + NUMBER_OF_PARAMETER_SECTORS) + (((CAST_POINTER_ARITHMETIC)ptrSector)/FLASH_GRANULARITY));
        ulSectorSize = FLASH_GRANULARITY;                                // access in code area
    }
    else if (ptrSector >= (unsigned char *)(FLASH_START_ADDRESS + (NUMBER_OF_BOOT_SECTORS * FLASH_GRANULARITY_BOOT))) {
        #if NUMBER_OF_PARAMETER_SECTORS > 1
        ptrSector -= (FLASH_START_ADDRESS + (NUMBER_OF_BOOT_SECTORS * FLASH_GRANULARITY_BOOT));
        *ulSectorNumber += ((NUMBER_OF_BOOT_SECTORS) + (((CAST_POINTER_ARITHMETIC)ptrSector)/FLASH_GRANULARITY_PARAMETER));
        #else
        *ulSectorNumber += (NUMBER_OF_BOOT_SECTORS);
        #endif
        ulSectorSize = FLASH_GRANULARITY_PARAMETER;                      // access in parameter area
    }
    else {
        ptrSector -= (FLASH_START_ADDRESS);
        *ulSectorNumber += (((CAST_POINTER_ARITHMETIC)ptrSector)/FLASH_GRANULARITY_BOOT);
        ulSectorSize = FLASH_GRANULARITY_BOOT;                           // access in boot area
    }
    if (*ulSectorNumber < 16) {
        if ((FLASH_OPTCR & (FLASH_OPTCR_nWRP0 << *ulSectorNumber)) == 0) {
            *iProtected = FLASH_SECTOR_PROTECTED;                        // sector is write protected
        }
    }
        #if defined FLASH_OPTCR1
    else {
        if ((FLASH_OPTCR1 & (FLASH_OPTCR1_nWRP0 << (*ulSectorNumber - 16))) == 0) {
            *iProtected = FLASH_SECTOR_PROTECTED;                        // sector is write protected
        }
    }
        #endif
    #endif
    return ulSectorSize;
}
    #endif

#if !defined _ST_FLASH_UNIFORM_GRANULARITY
static int fnSingleByteFlashWrite(unsigned char *ucDestination, unsigned char ucData)
{
    #if defined _WINDOWS
    unsigned long ulSectorNumber;
    int iProtectedState;
    #endif
    if (*(unsigned char *)fnGetFlashAdd((unsigned char *)ucDestination) == ucData) {
        return 0;                                                        // if the value is already programmed in flash there is no need to write
    }
    FLASH_CR = FLASH_CR_PG;                                              // select byte programming
    #if defined _WINDOWS
    fnGetFlashSectorSize(ucDestination, &ulSectorNumber, &iProtectedState);
    FLASH_CR |= ulFlashLockState;
    if (((FLASH_CR & FLASH_CR_LOCK) != 0) || (iProtectedState != 0)) {   // if lock bit is set or the sector is write protected don't program
        FLASH_SR |= FLASH_ERROR_FLAGS;
    }
    else {
    #endif
        *((unsigned char *)fnGetFlashAdd((unsigned char *)ucDestination)) = ucData; // program the byte
    #if defined _WINDOWS
    }
    #endif
    while ((FLASH_SR & FLASH_SR_BSY) != 0) {}                            // wait until write operation completes
    if ((FLASH_SR & (FLASH_SR_WRPERR | FLASH_SR_PGAERR | FLASH_SR_PGPERR | FLASH_SR_PGSERR)) != 0) { // check for errors
        return -1;                                                       // write error
    }
    return 0;
}

    #if SUPPLY_VOLTAGE > SUPPLY_1_8__2_1                                 // short word writes are only possible when the supply voltage is greater than 2.1V
static int fnSingleWordFlashWrite(unsigned short *usDestination, unsigned short usData)
{
    #if defined _WINDOWS
    unsigned long ulSectorNumber;
    int iProtectedState;
    #endif
    if (*(unsigned short *)fnGetFlashAdd((unsigned char *)usDestination) == usData) {
        return 0;                                                        // if the value is already programmed in flash there is no need to write
    }
    FLASH_CR = (FLASH_CR_PG | FLASH_CR_PSIZE_16);                        // select short word programming
    #if defined _WINDOWS
    fnGetFlashSectorSize((unsigned char *)usDestination, &ulSectorNumber, &iProtectedState);
    FLASH_CR |= ulFlashLockState;
    if (((FLASH_CR & FLASH_CR_LOCK) != 0) || (iProtectedState != 0)) {   // if lock bit is set or the sector is write protected don't program
        FLASH_SR |= FLASH_ERROR_FLAGS;
    }
    else {
    #endif
        *((unsigned short *)fnGetFlashAdd((unsigned char *)usDestination)) = usData; // program the byte
    #if defined _WINDOWS
    }
    #endif
    while ((FLASH_SR & FLASH_SR_BSY) != 0) {}                            // wait until write operation completes
    if ((FLASH_SR & (FLASH_SR_WRPERR | FLASH_SR_PGAERR | FLASH_SR_PGPERR | FLASH_SR_PGSERR)) != 0) { // check for errors
        return -1;                                                       // write error
    }
    return 0;
}
    #endif

    #if SUPPLY_VOLTAGE >= SUPPLY_2_7__3_6                                 // long word writes are only possible when the supply voltage is greater than 2.7V
static int fnSingleLongWordFlashWrite(unsigned long *ulDestination, unsigned long ulData)
{
    #if defined _WINDOWS
    unsigned long ulSectorNumber;
    int iProtectedState;
    #endif
    if (*(unsigned long *)fnGetFlashAdd((unsigned char *)ulDestination) == ulData) {
        return 0;                                                        // if the value is already programmed in flash there is no need to write
    }
    FLASH_CR = (FLASH_CR_PG | FLASH_CR_PSIZE_32);                        // select long short word programming
    #if defined _WINDOWS
    fnGetFlashSectorSize((unsigned char *)ulDestination, &ulSectorNumber, &iProtectedState);
    FLASH_CR |= ulFlashLockState;
    if (((FLASH_CR & FLASH_CR_LOCK) != 0) || (iProtectedState != 0)) {   // if lock bit is set or the sector is write protected don't program
        FLASH_SR |= FLASH_ERROR_FLAGS;                                   // set the error flag
    }
    else {
    #endif
        *((unsigned long *)fnGetFlashAdd((unsigned char *)ulDestination)) = ulData; // program the byte
    #if defined _WINDOWS
    }
    #endif
    while ((FLASH_SR & FLASH_SR_BSY) != 0) {}                            // wait until write operation completes
    if ((FLASH_SR & (FLASH_SR_WRPERR | FLASH_SR_PGAERR | FLASH_SR_PGPERR | FLASH_SR_PGSERR)) != 0) { // check for errors
        return -1;                                                       // write error
    }
    return 0;                                                            // programming was successul
}
    #endif
#endif


static int fnWriteInternalFlash(ACCESS_DETAILS *ptrAccessDetails, unsigned char *ucData)
{
    int iError = 0;
    MAX_FILE_LENGTH Length = ptrAccessDetails->BlockLength;
    unsigned char *ucDestination = (unsigned char *)ptrAccessDetails->ulOffset;
#if defined FLASH_USES_WRITE_PROTECTION && (defined _STM32F2XX || defined _STM32F4XX || defined _STM32F7XX)
    unsigned char *ptrSector = ucDestination;
    unsigned long _ulSectorSize;                                         // F2/F4/F7 have variable flash granularity
    unsigned long ulSectorNumber;
    int iProtectedSector;
    unsigned long ulProtectedSectors = 0;
#endif
#if !defined _STM32L0x1                                                  // temporary
    if ((FLASH_CR & FLASH_CR_LOCK) != 0) {                               // if the flash has not been unlocked, unlock it before programming
        FLASH_KEYR = FLASH_KEYR_KEY1;
        FLASH_KEYR = FLASH_KEYR_KEY2;
    #if defined _WINDOWS
        FLASH_CR &= ~FLASH_CR_LOCK;
        ulFlashLockState = 0;
    #endif
    }
    #if defined FLASH_USES_WRITE_PROTECTION && !defined _ST_FLASH_UNIFORM_GRANULARITY
    do {
        _ulSectorSize = fnGetFlashSectorSize(ptrSector, &ulSectorNumber, &iProtectedSector);
        ptrSector = (unsigned char *)((CAST_POINTER_ARITHMETIC)ptrSector & ~(_ulSectorSize - 1)); // align to sector start boundary
        ptrSector += _ulSectorSize;                                      // following flash sector
        if (iProtectedSector != 0) {                                     // if the sector is write protected
            fnProtectFlash(ucDestination, UNPROTECT_SECTOR);             // unprotect the sectors we will be writing to
            ulProtectedSectors |= (1 << ulSectorNumber);                 // mark the sectors that have been unprotected
        }
    } while ((ucDestination + Length) >= ptrSector);                     // repeat for all sectors that will be written to
    #endif
    #if defined _STM32F2XX || defined _STM32F4XX || defined _STM32F7XX   // depending on the power supply range it is possible to write bytes, short- and long words (with external Vpp 64 bits but this is not supported in this implementation)
    FLASH_SR = (FLASH_STATUS_FLAGS);                                     // reset status flags
        #if defined _WINDOWS
    FLASH_SR = 0;
        #endif
        #if SUPPLY_VOLTAGE == SUPPLY_1_8__2_1                            // only byte wise programming is possible
    while (Length-- != 0) {        
        if (fnSingleByteFlashWrite(ucDestination, *ucData) != 0) {
            iError = 1;                                                  // write error
            break;
        }
        ucDestination++;
        ucData++;
    }
        #elif SUPPLY_VOLTAGE < SUPPLY_2_7__3_6                           // byte or half-word programming possible
    {
        unsigned short usWord;
        int iCollected = 0;
        while (Length != 0) {
            if (iCollected == 0) {
                if ((Length == 1) || (((CAST_POINTER_ARITHMETIC)ucDestination) & 0x1)) { // single byte write (or final byte write) or misaligned half-word
                    if (fnSingleByteFlashWrite(ucDestination, *ucData) != 0) { // perform single byte programming
                        iError = 1;                                      // write error
                        break;
                    }
                    ucDestination++;
                }
                else {
                    usWord = *ucData;                                    // collect in little endian format
                    iCollected++;
                }
            }
            else {
                usWord |= (*ucData << 8);
                if (fnSingleWordFlashWrite((unsigned short *)ucDestination, usWord) != 0) { // perform a short word write
                    iError = 1;                                          // write error
                    break;
                }
                ucDestination += 2;
                iCollected = 0;
            }
            ucData++;
            Length--;
        }
    }
        #else                                                            // byte, half-word and word programming possible when aligned
    {
        unsigned long  ulLongWord = 0;
        unsigned short usWord = 0;
        int iCollected = 0;
        while (Length != 0) {
            if (iCollected == 0) {
                if ((Length == 1) || (((CAST_POINTER_ARITHMETIC)ucDestination) & 0x1)) { // single byte write (or final byte write) or misaligned half-word
                    if (fnSingleByteFlashWrite(ucDestination, *ucData) != 0) { // perform single byte programming
                        iError = 1;                                      // write error
                        break;
                    }
                    ucDestination++;
                }
                else {
                    usWord = *ucData;                                    // collect in little endian format
                    iCollected++;
                }
            }
            else if (iCollected == 1) {
                usWord |= (*ucData << 8);
                iCollected++;
                if ((Length <= 2) || ((((CAST_POINTER_ARITHMETIC)(ucDestination + 2)) & 0x3) == 0)) { // last short word or ending on a 128 bit line boundary
                    if (fnSingleWordFlashWrite((unsigned short *)ucDestination, usWord) != 0) { // perform a short word write
                        iError = 1;                                      // write error
                        break;
                    }
                    ucDestination += 2;
                    iCollected = 0;
                }
            }
            else if (iCollected == 2) {
                ulLongWord = (usWord | (*ucData << 16));
                iCollected++;
            }
            else {
                ulLongWord |= (*ucData << 24);
                if (fnSingleLongWordFlashWrite((unsigned long *)ucDestination, ulLongWord) != 0) { // perform a long word write
                    iError = 1;                                          // write error
                    break;
                }
                ucDestination += 4;
                iCollected = 0;
            }
            ucData++;
            Length--;
        }
    }
        #endif
    #else
    {
        unsigned short usValue;
        // The STM32F1xx writes always in short words so the start is expected to be aligned and the length to be a multiple of half words
        //
        while (Length != 0) {
            usValue = *ucData++;                                         // little endian format
            usValue |= (*ucData++ << 8);
            if (*(unsigned short *)fnGetFlashAdd((unsigned char *)ucDestination) != usValue) { // if the value is already programmed skip the write
                FLASH_CR = FLASH_CR_PG;                                  // select half-word programming
                FLASH_SR = (FLASH_STATUS_FLAGS);                         // reset status flags
                #if defined _WINDOWS
                FLASH_SR = 0;
                #endif
                _DISABLE_INTERRUPTS();
                *(volatile unsigned short *)fnGetFlashAdd((unsigned char *)ucDestination) = usValue; // write the value to the flash location
                while ((FLASH_SR & FLASH_SR_BSY) != 0) {}                // wait until write operation completes
                _ENABLE_INTERRUPTS();
                if (FLASH_SR & (FLASH_SR_WRPRTERR | FLASH_SR_PGERR)) {   // check for errors
                    iError = 1;                                          // write error
                    break;
                }
            }
            if (Length <= 2) {
                break;
            }
            ucDestination += 2;
            Length -= 2;
        }
    }
    #endif
    #if defined FLASH_USES_WRITE_PROTECTION && !defined _ST_FLASH_UNIFORM_GRANULARITY
    ptrSector = (unsigned char *)FLASH_START_ADDRESS;
    do {
        _ulSectorSize = fnGetFlashSectorSize(ptrSector, &ulSectorNumber, &iProtectedSector);
        if ((ulProtectedSectors & 1) != 0) {
            fnProtectFlash(ptrSector, PROTECT_SECTOR);                   // protect the un-protected sectors after completing the write
        }
        ptrSector += _ulSectorSize;                                      // following flash sector
        ulProtectedSectors >>= 1;
    } while (ulProtectedSectors != 0);                                   // repeat for all sectors that need to be re-protected
    #endif
    #if !defined _STM32F7XX                                              // workaround: re-locking the Flash causes the previous write to be cancelled (to be investigated)
    FLASH_CR = FLASH_CR_LOCK;                                            // lock flash when complete
        #if defined _WINDOWS
    ulFlashLockState = FLASH_CR_LOCK;
        #endif
    #endif
#endif
    return iError;
}

extern void fnProtectFlash(unsigned char *ptrSector, unsigned char ucProtection)
{
#if 0
    unsigned long ulSectorNumber;
    int iProtected;
    unsigned long ulRegister = FLASH_OPTCR;
    #if defined FLASH_OPTCR1
    unsigned long ulRegister1 = FLASH_OPTCR1;
    #endif
    fnGetFlashSectorSize(ptrSector, &ulSectorNumber, &iProtected);
    if (UNPROTECT_SECTOR == ucProtection) {                              // unprotect flash
        if (iProtected == 0) {
            return;
        }
        if (ulSectorNumber < 16) {
            ulRegister |= (FLASH_OPTCR_nWRP0 << ulSectorNumber);
        }
        else {
            ulRegister1 |= (FLASH_OPTCR1_nWRP0 << (ulSectorNumber - 16));
        }
    }
    else {                                                               // protect flash
        if (iProtected != 0) {
            return;
        }
        if (ulSectorNumber < 16) {
            ulRegister &= ~(FLASH_OPTCR_nWRP0 << ulSectorNumber);
        }
        else {
            ulRegister1 &= ~(FLASH_OPTCR1_nWRP0 << (ulSectorNumber - 16));
        }
    }
    ulRegister &= ~(FLASH_OPTCR_OPTLOCK | FLASH_OPTCR_OPTSTRT);
    FLASH_OPTKEYR = FLASH_OPTKEYR_KEY1;                                  // unlock option register(s)
    FLASH_OPTKEYR = FLASH_OPTKEYR_KEY2;
    #if defined _WINDOWS
    FLASH_OPTCR &= ~(FLASH_OPTCR_OPTLOCK);                               // register is unlocked
    #endif
    #if defined FLASH_OPTCR1
    FLASH_OPTCR1 = ulRegister1;
    #endif
    FLASH_OPTCR = ulRegister;                                            // write new setting and relock the register
    FLASH_OPTCR |= (FLASH_OPTCR_OPTSTRT);
    while ((FLASH_SR & FLASH_SR_BSY) != 0) {
    }
    FLASH_OPTCR |= (FLASH_OPTCR_OPTLOCK);
    /*
    ulRegister |= FLASH_OPTCR_OPTLOCK;
    FLASH_OPTKEYR = FLASH_OPTKEYR_KEY1;                                  // unlock option register(s)
    FLASH_OPTKEYR = FLASH_OPTKEYR_KEY2;
    #if defined _WINDOWS
    FLASH_OPTCR &= ~(FLASH_OPTCR_OPTLOCK);                               // register is unlocked
    #endif
    #if defined FLASH_OPTCR1
    FLASH_OPTCR1 = ulRegister1;
    #endif
    FLASH_OPTCR = ulRegister;                                            // write new setting and relock the register
    */
#endif
}

        #if !defined ONLY_INTERNAL_FLASH_STORAGE

// Search for the memory type that the starting address is in, return the type and restrict the largest length that can be read,written, erased from that location
//
extern unsigned char fnGetStorageType(unsigned char *memory_pointer, ACCESS_DETAILS *ptrAccessDetails)
{
    unsigned long ulMaximumLength;
    STORAGE_AREA_ENTRY *ptrStorageList = UserStorageListPtr;
    #if defined _WINDOWS                                                 // this is only used when reading with the simulator
    if (iFetchingInternalMemory != 0) {                                  // if the access is to program memory when simulating
        iFetchingInternalMemory = 0;                                     // reset single-shot program access
        return _STORAGE_PROGRAM;
    }
    #endif
    while (ptrStorageList != 0) {                                        // while there are storage areas defined
        if ((memory_pointer >= ptrStorageList->ptrMemoryStart) && (memory_pointer <= ptrStorageList->ptrMemoryEnd)) { // determine in which memory the access starts in
            if (ptrAccessDetails == 0) {
                return ptrStorageList->ucStorageType;
            }
            ptrAccessDetails->ucDeviceNumber = 0;
            if (ptrStorageList->ucDeviceCount > 1) {                     // if storage area is made up of multiple devices (assumed to be all the same size)
                unsigned long ulDeviceRangeLength = ((unsigned long)((ptrStorageList->ptrMemoryEnd - ptrStorageList->ptrMemoryStart) + 1)/ptrStorageList->ucDeviceCount);
                unsigned char *ptrStart = ptrStorageList->ptrMemoryStart;
                unsigned char *ptrEnd = (ptrStorageList->ptrMemoryStart + ulDeviceRangeLength);                
                while (ptrEnd < memory_pointer) {
                    ptrStart += ulDeviceRangeLength;
                    ptrEnd += ulDeviceRangeLength;
                    ptrAccessDetails->ucDeviceNumber++;                  // the device number that the access is in
                }
                ulMaximumLength = (unsigned long)(ptrEnd - memory_pointer);
                ptrAccessDetails->ulOffset = (unsigned long)(memory_pointer - ptrStart); // offset from start of memory
            }
            else {
                ulMaximumLength = (unsigned long)(ptrStorageList->ptrMemoryEnd - memory_pointer);
                ulMaximumLength++;                                       // compensate the fact that the end pointer is to the end of the last used address (see reason in STORAGE_AREA_ENTRY definition)
                ptrAccessDetails->ulOffset = (unsigned long)(memory_pointer - ptrStorageList->ptrMemoryStart); // offset from start of memory
            }
            if (ulMaximumLength < ptrAccessDetails->BlockLength) {
                ptrAccessDetails->BlockLength = (MAX_FILE_LENGTH)ulMaximumLength; // limit the length to the present storage device
            }
            return ptrStorageList->ucStorageType;                        // the storage area type
        }
        ptrStorageList = (STORAGE_AREA_ENTRY *)(ptrStorageList->ptrNext);// move to next storage area
    };
    _EXCEPTION("Attempted access to unsupported storage area!!!!");
    return _STORAGE_INVALID;                                             // attempted access outside of the defined storage areas
}
    #endif


// Erase FLASH sector(s). The pointer can be anywhere in the sector to be erased.
// If the length signifies multiple sectors, each one necessary is erased.
//
extern int fnEraseFlashSector(unsigned char *ptrSector, MAX_FILE_LENGTH Length)
{
#if !defined _STM32L0x1                                                  // temporary
    #if defined _ST_FLASH_UNIFORM_GRANULARITY                            // F1 has uniform flash granularity
        #define _ulSectorSize FLASH_GRANULARITY
    #else
    unsigned long _ulSectorSize;                                         // F2/F4/F7 have variable flash granularity
    unsigned long ulSectorNumber;
    int iProtectedSector;
    #endif
    #if defined MANAGED_FILES
    MAX_FILE_LENGTH OriginalLength = Length;
    #endif
    #if !defined ONLY_INTERNAL_FLASH_STORAGE
    ACCESS_DETAILS AccessDetails;
    do {
        AccessDetails.BlockLength = Length;
        switch (fnGetStorageType(ptrSector, &AccessDetails)) {           // get the storage type based on the memory location and also return the largest amount of data that can be read from a single device
        case _STORAGE_INTERNAL_FLASH:
            #if !defined _ST_FLASH_UNIFORM_GRANULARITY
            _ulSectorSize = fnGetFlashSectorSize(ptrSector, &ulSectorNumber, &iProtectedSector);
            #endif
            Length += (((CAST_POINTER_ARITHMETIC)ptrSector) - ((CAST_POINTER_ARITHMETIC)ptrSector & ~(_ulSectorSize - 1)));
            ptrSector = (unsigned char *)((CAST_POINTER_ARITHMETIC)ptrSector & ~(_ulSectorSize - 1)); // set to sector boundary
            if ((FLASH_CR & FLASH_CR_LOCK) != 0) {                       // if the flash has not been unlocked, unlock it before erasing
                FLASH_KEYR = FLASH_KEYR_KEY1;
                FLASH_KEYR = FLASH_KEYR_KEY2;
            #if defined _WINDOWS
                FLASH_CR &= ~FLASH_CR_LOCK;
                ulFlashLockState = 0;
            #endif
            }
            #if defined _STM32F2XX || defined _STM32F4XX || defined _STM32F7XX
            FLASH_CR = (FLASH_CR_SER | MAXIMUM_PARALLELISM | (ulSectorNumber << FLASH_CR_SNB_SHIFT)); // prepare the section to be deleted
            FLASH_CR |= (FLASH_CR_STRT);                                 // start the erase operation
            #else
            FLASH_CR = FLASH_CR_PER;                                     // {15} select page erase
            FLASH_SR = (FLASH_SR_PGERR | FLASH_SR_WRPRTERR | FLASH_SR_EOP); // {15} reset status flags
                #if defined _WINDOWS                                     // {15}
            FLASH_SR = 0;                                                // reset self-clearing status bits
                #endif
            FLASH_AR = (CAST_POINTER_ARITHMETIC)ptrSector;               // set pointer to first location in the page to be erased
            FLASH_CR = (FLASH_CR_PER | FLASH_CR_STRT);                   // {15} start page erase operation
          //FLASH_CR |= FLASH_CR_STRT;                                   // start page erase operation
            #endif
            #if defined _WINDOWS
            FLASH_CR |= ulFlashLockState;
            if ((FLASH_CR & FLASH_CR_LOCK) != 0) {                       // if lock bit set don't erase
                FLASH_SR |= FLASH_ERROR_FLAGS;
            }
            else {
                uMemset(fnGetFlashAdd(ptrSector), 0xff, _ulSectorSize);  // delete the page content
            }
            #endif
            while ((FLASH_SR & FLASH_SR_BSY) != 0) {}                    // wait until delete operation completes
            if ((FLASH_SR & FLASH_ERROR_FLAGS) != 0) {
                return -1;                                               // erase error
            }
            AccessDetails.BlockLength = _ulSectorSize;                   // the length of the sector that was erased
            break;
        #if defined SPI_FLASH_ENABLED
        case _STORAGE_SPI_FLASH:
            Length += (((CAST_POINTER_ARITHMETIC)ptrSector) - ((CAST_POINTER_ARITHMETIC)ptrSector & ~(SPI_FLASH_PAGE_LENGTH - 1)));
            ptrSector = (unsigned char *)((CAST_POINTER_ARITHMETIC)ptrSector & ~(SPI_FLASH_PAGE_LENGTH - 1)); // set to sector boundary
            AccessDetails.BlockLength = fnDeleteSPI(&AccessDetails);     // delete page/block in SPI flash 
            break;
        #endif
        #if defined SPI_EEPROM_FILE_SYSTEM
        case _STORAGE_SPI_EEPROM:
            break;
        #endif
        #if defined I2C_EEPROM_FILE_SYSTEM
        case _STORAGE_I2C_EEPROM:
            {
                CAST_POINTER_ARITHMETIC align_length = (((CAST_POINTER_ARITHMETIC)ptrSector) - ((CAST_POINTER_ARITHMETIC)ptrSector & ~(EXTENSION_FILE_SIZE - 1)));
                Length += align_length;
                AccessDetails.BlockLength += align_length;
                ptrSector = (unsigned char *)((CAST_POINTER_ARITHMETIC)ptrSector & ~(EXTENSION_FILE_SIZE - 1)); // set to sector boundary
                if (fnDeleteI2C_eeprom(&AccessDetails) < 0) {            // delete data in I2C EEPROM
                    return -1;
                }
            }
            break;
        #endif
        #if defined EXT_FLASH_FILE_SYSTEM
        case _STORAGE_PARALLEL_FLASH:
            Length += (((CAST_POINTER_ARITHMETIC)ptrSector) - ((CAST_POINTER_ARITHMETIC)ptrSector & ~(EXT_FLASH_PAGE_LENGTH - 1)));
            ptrSector = (unsigned char *)((CAST_POINTER_ARITHMETIC)ptrSector & ~(EXT_FLASH_PAGE_LENGTH - 1)); // set to sector boundary
            AccessDetails.BlockLength = fnDeleteParallelFlash(&AccessDetails); // delete page/block in parallel flash 
            if (AccessDetails.BlockLength == 0) {
                return -1;                                               // error
            }
            break;
        #endif
        default:
            _EXCEPTION("Invalid storage type");
            return -1;                                                   // invalid
        }
        ptrSector += AccessDetails.BlockLength;
        if (Length <= AccessDetails.BlockLength) {                       // if completed
        #if defined MANAGED_FILES
            if (OriginalLength == 0) {                                   // if a single page erase was called, return the page size of the storage area
	            return AccessDetails.BlockLength;
	        }
        #endif
            break;
        }
        Length -= AccessDetails.BlockLength;
    } FOREVER_LOOP();
    #else                                                                // case when only internal Flash is available
    do {
        #if !defined _ST_FLASH_UNIFORM_GRANULARITY
        _ulSectorSize = fnGetFlashSectorSize(ptrSector, &ulSectorNumber, &iProtectedSector);
        #endif
        Length += (MAX_FILE_LENGTH)(((CAST_POINTER_ARITHMETIC)ptrSector) - ((CAST_POINTER_ARITHMETIC)ptrSector & ~(_ulSectorSize - 1)));
        ptrSector = (unsigned char *)((CAST_POINTER_ARITHMETIC)ptrSector & ~(_ulSectorSize - 1)); // set to sector boundary
        if ((FLASH_CR & FLASH_CR_LOCK) != 0) {                           // if the flash has not been unlocked, unlock it before erasing
            FLASH_KEYR = FLASH_KEYR_KEY1;
            FLASH_KEYR = FLASH_KEYR_KEY2;
        #if defined _WINDOWS
            FLASH_CR &= ~FLASH_CR_LOCK;
            ulFlashLockState = 0;
        #endif
        }
        #if defined FLASH_USES_WRITE_PROTECTION && (defined _STM32F2XX || defined _STM32F4XX || defined _STM32F7XX)
        if (iProtectedSector != 0) {
            fnProtectFlash(ptrSector, UNPROTECT_SECTOR);                 // unprotect the write-protected sector so that we can erase it
        }
        #endif
        #if defined _STM32F2XX || defined _STM32F4XX || defined _STM32F7XX
        FLASH_CR = (FLASH_CR_SER | MAXIMUM_PARALLELISM | (ulSectorNumber << FLASH_CR_SNB_SHIFT)); // prepare the section to be deleted
        FLASH_CR |= (FLASH_CR_STRT);                                     // start the erase operation
        #else
        FLASH_CR = FLASH_CR_PER;                                         // {15} select page erase
        FLASH_SR = (FLASH_SR_PGERR | FLASH_SR_WRPRTERR | FLASH_SR_EOP);  // {15} reset status flags
            #if defined _WINDOWS                                         // {15}
        FLASH_SR = 0;                                                    // reset self-clearing status bits
            #endif
        FLASH_AR = (CAST_POINTER_ARITHMETIC)ptrSector;                   // set pointer to first location in the page to be erased
        _DISABLE_INTERRUPTS();
        FLASH_CR = (FLASH_CR_PER | FLASH_CR_STRT);                       // {15} start page erase operation
      //FLASH_CR |= FLASH_CR_STRT;                                       // start page erase operation
        #endif
        #if defined _WINDOWS                                             // only used when simulating
        FLASH_CR |= ulFlashLockState;
        {
            int iProtectedState;
            unsigned long ulSectorNumber;
            fnGetFlashSectorSize(ptrSector, &ulSectorNumber, &iProtectedState);
            if (((FLASH_CR & FLASH_CR_LOCK) == 0) && (iProtectedState == 0)) {
                uMemset(fnGetFlashAdd(ptrSector), 0xff, _ulSectorSize);  // delete the sector content
            }
            else {
                FLASH_SR |= FLASH_ERROR_FLAGS;
            }
        }
        #endif
        while ((FLASH_SR & FLASH_SR_BSY) != 0) {}                        // wait until delete operation completes
        _ENABLE_INTERRUPTS();
        #if defined FLASH_USES_WRITE_PROTECTION && (defined _STM32F2XX || defined _STM32F4XX || defined _STM32F7XX)
        if (iProtectedSector != 0) {
            fnProtectFlash(ptrSector, PROTECT_SECTOR);                   // set protection again
        }
        #endif
        if ((FLASH_SR & FLASH_ERROR_FLAGS) != 0) {
            FLASH_CR = FLASH_CR_LOCK;                                    // lock flash when complete
            return -1;                                                   // erase error
        }
        if (Length <= _ulSectorSize) {                                   // last sector has been deleted
            FLASH_CR = FLASH_CR_LOCK;                                    // lock flash when complete
        #if defined MANAGED_FILES
            if (OriginalLength == 0) {                                   // if a single page erase was called, return the page size
	            return (int)_ulSectorSize;
	        }
        #endif
            break;
        }
        ptrSector += _ulSectorSize;                                      // advance sector point to next internal flash sector
        Length -= (MAX_FILE_LENGTH)_ulSectorSize;
    } FOREVER_LOOP();
    #endif
#endif
    return 0;
}

extern int fnWriteBytesFlash(unsigned char *ucDestination, unsigned char *ucData, MAX_FILE_LENGTH Length)
{
    ACCESS_DETAILS AccessDetails;
    AccessDetails.BlockLength = Length;
    #if !defined ONLY_INTERNAL_FLASH_STORAGE
    while (Length != 0) {
        switch (fnGetStorageType(ucDestination, &AccessDetails)) {       // get the storage type based on the memory location and also return the largest amount of data that can be read from a single device
        case _STORAGE_INTERNAL_FLASH:
            AccessDetails.ulOffset += FLASH_START_ADDRESS;               // set physical address from flash start and the offset
            if (fnWriteInternalFlash(&AccessDetails, ucData) != 0) {
                return 1;
            }
            break;
        #if defined SPI_FLASH_ENABLED
        case _STORAGE_SPI_FLASH:
            fnWriteSPI(&AccessDetails, ucData);
            break;
        #endif
        #if defined SPI_EEPROM_FILE_SYSTEM
        case _STORAGE_SPI_EEPROM:
            break;
        #endif
        #if defined I2C_EEPROM_FILE_SYSTEM
        case _STORAGE_I2C_EEPROM:
            fnWriteI2C_eeprom(&AccessDetails, ucData);
            break;
        #endif
        #if defined EXT_FLASH_FILE_SYSTEM
        case _STORAGE_PARALLEL_FLASH:
            if (fnWriteParallelFlash(&AccessDetails, ucData) != 0) {
                return 1;                                                // error
            }
            break;
        #endif
        default:
            _EXCEPTION("Invalid storage type");
            return 1;                                                    // invalid
        }
        if (Length == AccessDetails.BlockLength) {                       // if write is complete
            return 0;
        }
        Length -= AccessDetails.BlockLength;                             // decrease the amount of data remaining
        ucDestination += AccessDetails.BlockLength;                      // increase the destination address accordingly
        ucData += AccessDetails.BlockLength;                             // increase the buffer address accordingly
    }
    return 0;
    #else
    AccessDetails.ulOffset = (unsigned long)ucDestination;
    return (fnWriteInternalFlash(&AccessDetails, ucData));
    #endif
}

extern void fnGetParsFile(unsigned char *ParLocation, unsigned char *ptrValue, MAX_FILE_LENGTH Size)
{
    #if defined _WINDOWS
    if (iFetchingInternalMemory != 0) {
        uMemcpy(ptrValue, ParLocation, Size);
        return;
    }
    #endif
    #if !defined NO_FLASH_SUPPORT
        #if !defined ONLY_INTERNAL_FLASH_STORAGE
    {
        ACCESS_DETAILS AccessDetails;
        AccessDetails.BlockLength = Size;
        while (Size != 0) {
            switch (fnGetStorageType(ParLocation, &AccessDetails)) {     // get the storage type based on the memory location and also return the largest amount of data that can be read from a single device
        #ifndef _WINDOWS
            case _STORAGE_INVALID:                                       // read from unknown area is allowed on the HW so that SRAM (for example) could be read without explicity declaring it
        #endif
            case _STORAGE_INTERNAL_FLASH:
                uMemcpy(ptrValue, fnGetFlashAdd(ParLocation), AccessDetails.BlockLength); // read from internal FLASH with no access restrictions so we can simply copy the data
                break;
                #if defined SPI_FLASH_ENABLED
            case _STORAGE_SPI_FLASH:
                fnReadSPI(&AccessDetails, ptrValue);                     // read from the SPI device
                break;
                #endif
                #if defined SPI_EEPROM_FILE_SYSTEM
            case _STORAGE_SPI_EEPROM:
                break;
                #endif
                #if defined I2C_EEPROM_FILE_SYSTEM
            case _STORAGE_I2C_EEPROM:
                fnReadI2C_eeprom(&AccessDetails, ptrValue);              // read the data from the external device
                break;
                #endif
                #if defined EXT_FLASH_FILE_SYSTEM
            case _STORAGE_PARALLEL_FLASH:
                uMemcpy(ptrValue, fnGetExtFlashAdd(ParLocation), AccessDetails.BlockLength); // read from external parallel FLASH with no access restrictions so we can simply copy the data
                break;
                #endif
                #if defined _WINDOWS                                     // only when simulating
            case _STORAGE_PROGRAM:
                uMemcpy(ptrValue, ParLocation, Size);                            
                return;
                #endif
            default:
                _EXCEPTION("Invalid storage type");
                return;                                                  // invalid
            }
            if (Size == AccessDetails.BlockLength) {                     // if read is complete
                return;
            }
            Size -= AccessDetails.BlockLength;                           // decrease the amount of data remaining
            ParLocation += AccessDetails.BlockLength;                    // increase the destination address accordingly
            ptrValue += AccessDetails.BlockLength;                       // increase the buffer address accordingly
        }
    }
        #else
    uMemcpy(ptrValue, fnGetFlashAdd(ParLocation), Size);                 // directly copy memory since this must be a pointer to code (embedded file)
        #endif
    #endif
}

#if defined USE_PARAMETER_BLOCK
    #define PARAMETER_BLOCK_1     (unsigned char *)(PARAMETER_BLOCK_START)
    #if defined USE_PAR_SWAP_BLOCK
        #define PARAMETER_BLOCK_2 (unsigned char *)(PARAMETER_BLOCK_1 + PARAMETER_BLOCK_SIZE)
    #endif

// The STM32F1 has 1k or 2k byte blocks which can be individually modified so we use one of these, and a backup if desired (F2/F4 has 16k boot sectors which can be used)
// STM32F2/4 flash can write as individual bytes to flash and so uses a narrower parameter validity marking since byte writes are always persissible (also at low operating supply voltage)
//
extern unsigned char fnGetValidPars(unsigned char ucValid)
{
    #if defined _STM32F2XX || defined _STM32F4XX || defined _STM32F7XX
    unsigned char ucValidUse[2];
    unsigned char ucCompare;

    // The first 2 byte locations define the validity of the block. 0x55 0x55 means it is fully valid. 0xff 0x55 means it contains temporary unvalidated data
    //
    fnGetParsFile(PARAMETER_BLOCK_1, ucValidUse, sizeof(ucValidUse));

    if (PRESENT_TEMP_BLOCK != ucValid) {
        ucCompare = 0x55;                                                // we are looking for validated pattern
    }
    else {
        ucCompare = 0xff;                                                // we are looking for temporary pattern
    }

    if ((ucValidUse[0] == 0x55) && (ucValidUse[1] == ucCompare)) {
        return 1;                                                        // first block matches request
    }
        #if defined USE_PAR_SWAP_BLOCK
    else {
        fnGetParsFile(PARAMETER_BLOCK_2, ucValidUse, sizeof(ucValidUse));
        if ((ucValidUse[0] == 0x55) && (ucValidUse[1] == ucCompare)) {
            return 2;                                                    // second block matches request
        }
    }
        #endif
    #else
    unsigned short usValidUse[2];
    unsigned short usCompare;

    // The first 2 word locations define the validity of the block. 0x5555 0x5555 means it is fully valid. 0xffff 0x5555 means it contains temporary unvalidated data
    //
    fnGetParsFile(PARAMETER_BLOCK_1, (unsigned char*)usValidUse, sizeof(usValidUse));

    if (PRESENT_TEMP_BLOCK != ucValid) {
        usCompare = 0x5555;                                              // we are looking for validated pattern
    }
    else {
        usCompare = 0xffff;                                              // we are looking for temporary pattern
    }

    if ((usValidUse[0] == 0x5555) && (usValidUse[1] == usCompare)) {
        return 1;                                                        // first block matches request
    }
        #if defined USE_PAR_SWAP_BLOCK
    else {
        fnGetParsFile(PARAMETER_BLOCK_2, (unsigned char*)usValidUse, sizeof(usValidUse));
        if ((usValidUse[0] == 0x5555) && (usValidUse[1] == usCompare)) {
            return 2;                                                    // second block matches request
        }
    }
        #endif
    #endif
    return 0;                                                            // all other cases indicate the searched block doesn't exist
}


static int fnDeleteParBlock(unsigned char *ptrAddInPar)
{
    return (fnEraseFlashSector(ptrAddInPar, PARAMETER_BLOCK_SIZE));
}

extern int fnGetParameters(unsigned char ucValidBlock, unsigned short usParameterReference, unsigned char *ucValue, unsigned short usLength)
{
    unsigned char *ptrPar = PARAMETER_BLOCK_1;

    #if defined USE_PAR_SWAP_BLOCK
    if (ucValidBlock == 2) {
        ptrPar += (PARAMETER_BLOCK_2 - PARAMETER_BLOCK_1);
    }
    #endif
    #if defined PARAMETER_NO_ALIGNMENT                                   // {19}
    ptrPar += usParameterReference;
    ptrPar += 2;                                                         // first parameter starts after validation information
    fnGetParsFile((unsigned char *)ptrPar, ucValue, usLength);
    #else
        #if defined _STM32F2XX || defined _STM32F4XX || defined _STM32F7XX // byte storage used 
    ptrPar += usParameterReference;
    ptrPar += 2;                                                         // first parameter starts after validation information

    while (usLength--) {
        fnGetParsFile(ptrPar, ucValue++, 1);
        ptrPar++;
    }
        #else                                                            // short word storage used
    ptrPar += (usParameterReference * 2);
    ptrPar += 4;                                                         // first parameter starts after validation information

    while (usLength--) {
        fnGetParsFile(ptrPar, ucValue++, 1);
        ptrPar += sizeof(unsigned short);
    }
        #endif
    #endif
    return 0;
}

extern int fnSetParameters(unsigned char ucValidBlock, unsigned short usParameterReference, unsigned char *ucValue, unsigned short usLength)
{
    #if defined _STM32F2XX || defined _STM32F4XX || defined _STM32F7XX
    unsigned char *ptrPar = PARAMETER_BLOCK_1, *ptrStart;
    unsigned char ucValid = 0x55;
    int iBlockUse = 0;

    if (TEMP_PARS & ucValidBlock) {
        ucValidBlock &= ~TEMP_PARS;
        iBlockUse = 1;
    }

    if (ucValidBlock & BLOCK_INVALID) {
        // No valid parameter blocks have been found so we can use the first for saving the data.
        // We delete it to be sure it is fresh
        //
        fnDeleteParBlock(PARAMETER_BLOCK_1);
        #if defined USE_PAR_SWAP_BLOCK
        fnDeleteParBlock(PARAMETER_BLOCK_2);
        #endif
        ucValidBlock = 1;
    }

        #if defined USE_PAR_SWAP_BLOCK
    if (ucValidBlock == 2) {
        ptrPar += (PARAMETER_BLOCK_2 - PARAMETER_BLOCK_1);
    }
        #endif
    ptrStart = ptrPar;
    ptrPar += 2;                                                         // first parameter starts after validation information
    ptrPar += usParameterReference;

    if (usLength == 0) {
        if (iBlockUse) {                                                 // if we have a new set waiting, set it to use state
            iBlockUse = 0;
        #if defined USE_PAR_SWAP_BLOCK
            if (ptrStart == PARAMETER_BLOCK_1) {
                ptrPar = PARAMETER_BLOCK_2;
            }
            else {
                ptrPar = PARAMETER_BLOCK_1;
            }
            fnDeleteParBlock(ptrPar);
        #endif
        }
        else {                                                           // we must delete the block
            fnDeleteParBlock(ptrStart);
            if (usParameterReference) {
                return 0;                                                // we don't validate it again
            }
        }
    }
    else {
        while (usLength--) {
            if (fnWriteBytesFlash(ptrPar++, ucValue++, 1) != 0) {        // write parameters to flash on a byte basis
                return 1;                                                // error
            }
        }
    }
    // Now we must validate the parameter block which we have just written to
    //
    if (fnWriteBytesFlash(ptrStart, &ucValid, 1) != 0) {                 // validate data
        return 1;                                                        // error
    }
    if (iBlockUse == 0) {
        if (fnWriteBytesFlash((ptrStart + 1), &ucValid, 1) != 0) {       // use data
            return 1;                                                    // error
        }
    }
    #else
    unsigned char *ptrPar = PARAMETER_BLOCK_1, *ptrStart;
    unsigned short usValid = 0x5555;
    int iBlockUse = 0;

    if (TEMP_PARS & ucValidBlock) {
        ucValidBlock &= ~TEMP_PARS;
        iBlockUse = 1;
    }

    if (ucValidBlock & BLOCK_INVALID) {
        // No valid parameter blocks have been found so we can use the first for saving the data.
        // We delete it to be sure it is fresh
        //
        fnDeleteParBlock(PARAMETER_BLOCK_1);
        #if defined USE_PAR_SWAP_BLOCK
        fnDeleteParBlock(PARAMETER_BLOCK_2);
        #endif
        ucValidBlock = 1;
    }

        #if defined USE_PAR_SWAP_BLOCK
    if (ucValidBlock == 2) {
        ptrPar += (PARAMETER_BLOCK_2 - PARAMETER_BLOCK_1);
    }
        #endif
    ptrStart = ptrPar;
    ptrPar += 4;                                                         // first parameter starts after validation information
        #if defined PARAMETER_NO_ALIGNMENT                               // {19}
    ptrPar += usParameterReference;
        #else
    ptrPar += (usParameterReference * 2);
        #endif

    if (!usLength) {
        if (iBlockUse) {                                                 // if we have a new set waiting, set it to use state
            iBlockUse = 0;
        #if defined USE_PAR_SWAP_BLOCK
            if (ptrStart == PARAMETER_BLOCK_1) {
                ptrPar = PARAMETER_BLOCK_2;
            }
            else {
                ptrPar = PARAMETER_BLOCK_1;
            }
            fnDeleteParBlock(ptrPar);
        #endif
        }
        else {                                                           // we must delete the block
            fnDeleteParBlock(ptrStart);
            if (usParameterReference) {
                return 0;                                                // we don't validate it again
            }
        }
    }
    else {
        #if defined PARAMETER_NO_ALIGNMENT                               // {19}
            #if defined _WINDOWS                                         // when simulating the write size to internal flash is checked since each write is expected to be a multiple of the flash write size
        if (ptrPar < (unsigned long *)SIZE_OF_FLASH) {                   // parameter memory is in the internal flash
            if ((usLength % 2) != 0) {                                   // check write block size which must always be a multiple of the flash write size (2)
                _EXCEPTION("When using the option PARAMETER_NO_ALIGNMENT all parameter block writes must be of the correct modularity size");
            }
            if (((CAST_POINTER_ARITHMETIC)ptrPar & 0x1) != 0) {          // check the alignment of the write
                _EXCEPTION("When using the option PARAMETER_NO_ALIGNMENT all parameter block writes must be aligned to a flash long word/phrase");
            }
        }
            #endif
        fnWriteBytesFlash((unsigned char *)ptrPar, ucValue, usLength);   // it is expected that the user writes blocks of parameter data that is aligned and packed to respect the characteristics of the flash memory being used
        #else
        unsigned char ucNewValue[2];
        ucNewValue[1] = 0xff;
        while (usLength--) {
            ucNewValue[0] = *ucValue++;
            if (fnWriteBytesFlash(ptrPar, ucNewValue, 2) != 0) {         // write a half-word to FLASH
                return 1;                                                // error
            }
            ptrPar += sizeof(unsigned short);
        }
        #endif
    }
    // Now we must validate the parameter block which we have just written to
    //
    if (fnWriteBytesFlash(ptrStart, (unsigned char *)&usValid, sizeof(usValid)) != 0) { // validate data
        return 1;                                                        // error
    }
    if (iBlockUse == 0) {
        if (fnWriteBytesFlash((ptrStart + sizeof(usValid)), (unsigned char *)&usValid, sizeof(usValid)) != 0) { // use data
            return 1;                                                    // error
        }
    }
    #endif
    return 0;
}
#endif