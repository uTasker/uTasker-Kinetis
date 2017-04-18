/***********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher
    
    ---------------------------------------------------------------------
    File:      kinetis_FLASH.h
    Project:   Single Chip Embedded Internet
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2016
    *********************************************************************
    12.01.2015 Perform write enable for SPI_FLASH_S25FL1_K (for KL/KE driver compatibility) {1}

*/


/* =================================================================== */
/*                          local definitions                          */
/* =================================================================== */

#if !defined SIZE_OF_NVRAM
    #define SIZE_OF_NVRAM 0
#endif

#if defined SPI_SW_UPLOAD || defined SPI_FLASH_FAT || (defined SPI_FILE_SYSTEM && defined FLASH_FILE_SYSTEM)
    #define SPI_FLASH_ENABLED 
#endif

/* =================================================================== */
/*                             constants                               */
/* =================================================================== */

/* =================================================================== */
/*                     global variable definitions                     */
/* =================================================================== */

#if (defined FLASH_ROUTINES || defined ACTIVE_FILE_SYSTEM  || defined USE_PARAMETER_BLOCK || defined INTERNAL_USER_FILES) && defined _WINDOWS
    extern int iFetchingInternalMemory = _ACCESS_NOT_IN_CODE;
#endif



#if defined FLASH_ROUTINES || defined FLASH_FILE_SYSTEM || defined USE_PARAMETER_BLOCK || defined SUPPORT_PROGRAM_ONCE
// This routine runs from SRAM - the reason why the pointer is passed is to avoid the routine taking it from a const value in FLASH, which is then not code location independent
//
    #if defined _WINDOWS
        #if defined USE_SECTION_PROGRAMMING                              // {105}
            static unsigned char ucFlexRam[FLEXRAM_SIZE] = {0};
            #undef FLEXRAM_START_ADDRESS
            #define FLEXRAM_START_ADDRESS  ucFlexRam
        #endif
static void fnFlashRoutine(volatile unsigned char *ptrFTFL_BLOCK)
{
    *ptrFTFL_BLOCK = FTFL_STAT_CCIF;                                     // launch the command - this clears the FTFL_STAT_CCIF flag (register is FTFL_FSTAT)
    while ((*ptrFTFL_BLOCK & FTFL_STAT_CCIF) == 0) {}                    // wait for the command to terminate
}
    #else                                                                // {46}
static unsigned short fnFlashRoutine[] = {                               // to avoid potential compiler in-lining of the routine (removing position independency) the machine code is used directly
    0x2180,    // MOVS   r1,#0x80                                           load the value 0x80 (command complete interrupt flag) to register r1
    0x7001,    // STRB   r1,[r0,#0x00]                                      write r1 (0x80) to the passed pointer location (r0)
    0x7801,    // LDRB   r1,[r0,#0x00]                                      read back from the same location to r1
    0x0609,    // LSLS   r1,r1,#24                                          shift the register content by 24 bits to the left so that the command complete interrupt flag is at bit 31
    0xd5fc,    // BPL    -4                                                 if the command complete interrupt flag bit is '0' (register content is not negative value) branch back to read its value again
    0x4770     // BX     lr                                                 return from sub-routine
};
    #endif


static int fnFlashNow(unsigned char ucCommand, unsigned long *ptrWord, unsigned long *ptr_ulWord)
{
    static void (*fnRAM_code)(volatile unsigned char *) = 0;
    #if defined FLEXFLASH_DATA || (defined KINETIS_KE && (defined SIZE_OF_EEPROM && SIZE_OF_EEPROM > 0)) // {82}{109}
    int iNoInterruptDisable = 0;                                         // the default is to protect programming from interrupts
    #endif
    if (fnRAM_code == 0) {                                               // the first time this is used it will load the program to SRAM
        #define PROG_WORD_SIZE 30                                        // adequate space for the small program
        int i = 0;
        unsigned char *ptrThumb2 = (unsigned char *)fnFlashRoutine;
        static unsigned short usProgSpace[PROG_WORD_SIZE] = {0};         // make space for the routine on stack (this will have an even boundary)

        ptrThumb2 =  (unsigned char *)(((CAST_POINTER_ARITHMETIC)ptrThumb2) & ~0x1); // thumb 2 address
        while (i < PROG_WORD_SIZE) {                                     // copy program to SRAM
            usProgSpace[i++] = *(unsigned short *)ptrThumb2;
            ptrThumb2 += sizeof (unsigned short);
        }
        ptrThumb2 = (unsigned char *)usProgSpace;
        ptrThumb2++;                                                     // create a thumb 2 call
        fnRAM_code = (void(*)(volatile unsigned char *))(ptrThumb2);
    #if defined KINETIS_KE                                               // set the flash programming clock to 1MHz once after a reset
        #if BUS_CLOCK < 8000000
        _EXCEPTION("Bus clock must be at least 800kHz to program flash!");
        #elif BUS_CLOCK <= 1600000                                       // recommended values depending on bus clock
        FTMRH_FCLKDIV = 0x00;
        #elif BUS_CLOCK <= 2600000
        FTMRH_FCLKDIV = 0x01
        #elif BUS_CLOCK <= 3600000
        FTMRH_FCLKDIV = 0x02;
        #elif BUS_CLOCK <= 4600000
        FTMRH_FCLKDIV = 0x03;
        #elif BUS_CLOCK <= 5600000
        FTMRH_FCLKDIV = 0x04;
        #elif BUS_CLOCK <= 6600000
        FTMRH_FCLKDIV = 0x05;
        #elif BUS_CLOCK <= 7600000
        FTMRH_FCLKDIV = 0x06;
        #elif BUS_CLOCK <= 8600000
        FTMRH_FCLKDIV = 0x07;
        #elif BUS_CLOCK <= 9600000
        FTMRH_FCLKDIV = 0x08;
        #elif BUS_CLOCK <= 10600000
        FTMRH_FCLKDIV = 0x09;
        #elif BUS_CLOCK <= 11600000
        FTMRH_FCLKDIV = 0x0a;
        #elif BUS_CLOCK <= 12600000
        FTMRH_FCLKDIV = 0x0b;
        #elif BUS_CLOCK <= 13600000
        FTMRH_FCLKDIV = 0x0c;
        #elif BUS_CLOCK <= 14600000
        FTMRH_FCLKDIV = 0x0d;
        #elif BUS_CLOCK <= 15600000
        FTMRH_FCLKDIV = 0x0e;
        #elif BUS_CLOCK <= 16600000
        FTMRH_FCLKDIV = 0x0f;
        #elif BUS_CLOCK <= 17600000
        FTMRH_FCLKDIV = 0x10;
        #elif BUS_CLOCK <= 18600000
        FTMRH_FCLKDIV = 0x11;
        #elif BUS_CLOCK <= 19600000
        FTMRH_FCLKDIV = 0x12;
        #elif BUS_CLOCK <= 20600000
        FTMRH_FCLKDIV = 0x13;
        #elif BUS_CLOCK <= 21600000
        FTMRH_FCLKDIV = 0x14;
        #elif BUS_CLOCK <= 22600000
        FTMRH_FCLKDIV = 0x15;
        #elif BUS_CLOCK <= 23600000
        FTMRH_FCLKDIV = 0x16;
        #elif BUS_CLOCK <= 24600000
        FTMRH_FCLKDIV = 0x17;
        #else
        FTMRH_FCLKDIV = 0x18;
        #endif
    #endif
    }
    while ((FTFL_FSTAT & FTFL_STAT_CCIF) == 0) {}                        // wait for previous commands to complete

    #if defined FTFL_STAT_RDCOLERR
    if ((FTFL_FSTAT & (FTFL_STAT_ACCERR | FTFL_STAT_FPVIOL | FTFL_STAT_RDCOLERR)) != 0) { // check for errors in previous command 
        FTFL_FSTAT = (FTFL_STAT_ACCERR | FTFL_STAT_FPVIOL | FTFL_STAT_RDCOLERR); // clear old errors
    }
    #else
    if ((FTFL_FSTAT & (FTFL_STAT_ACCERR | FTFL_STAT_FPVIOL)) != 0) {     // check for errors in previous command 
        FTFL_FSTAT = (FTFL_STAT_ACCERR | FTFL_STAT_FPVIOL);              // clear old errors
    }
    #endif

    #if defined KINETIS_KE
    FTMRH_FCCOBIX = 0;                                                   // reset the command index
        #if (defined SIZE_OF_EEPROM && SIZE_OF_EEPROM > 0)               // {109}
    if (ptrWord >= (unsigned long *)(SIZE_OF_FLASH - SIZE_OF_EEPROM)) {  // is the access a write/erase in EEPROM
        ptrWord += ((KE_EEPROM_START_ADDRESS - (SIZE_OF_FLASH - SIZE_OF_EEPROM))/sizeof(unsigned long)); // convert to EEPROM physical address
        if (FCMD_ERASE_FLASH_SECTOR == ucCommand) {
            ucCommand = FCMD_ERASE_EEPROM_SECTOR;                        // modify the command to the EEPROM sector erase command
        }
        iNoInterruptDisable = 1;                                         // this command may execute without interrupts being disabled
    }
        #endif
    #endif

    switch (FTFL_FCCOB0 = ucCommand) {                                   // enter the command sequence
    case FCMD_ERASE_ALL_BLOCKS:                                          // single command word required with the erase all blocks command
    #if defined _WINDOWS
        {
            unsigned char *ptrFlash = 0;
            do {
                fnDeleteFlashSector(fnGetFlashAdd(ptrFlash));            // erase all sectors
                ptrFlash += FLASH_GRANULARITY;
            } while (ptrFlash < (unsigned char *)SIZE_OF_FLASH);
        }
    #endif
        break;
    #if defined USE_SECTION_PROGRAMMING                                  // {105}
    case FCMD_PROGRAM_SECTOR:
    #endif
    #if (defined KINETIS_KE && (defined SIZE_OF_EEPROM && SIZE_OF_EEPROM > 0)) // {109}
    case FCMD_PROGRAM_EEPROM:
    case FCMD_ERASE_EEPROM_SECTOR:
    #endif
    case FCMD_ERASE_FLASH_SECTOR:
    case FCMD_PROGRAM:
    #if defined FLEXFLASH_DATA                                           // {82} if working with FlashNMV in data flash mode
        if (ptrWord >= (unsigned long *)(SIZE_OF_FLASH - SIZE_OF_FLEXFLASH)) { // is the access a write/erase in FlexNVM
            ptrWord += ((FLEXNVM_START_ADDRESS - (SIZE_OF_FLASH - SIZE_OF_FLEXFLASH))/sizeof(unsigned long)); // convert to FlexNVM physical address
        }
    #endif
    #if defined SUPPORT_SWAP_BLOCK
    case FCMD_SWAP:
    #endif
        FTFL_FCCOB1 = (unsigned char)(((CAST_POINTER_ARITHMETIC)ptrWord) >> 16); // set address in flash
    #if defined FLEXFLASH_DATA                                           // {82} if working with FlashNMV in data flash mode
        if ((CAST_POINTER_ARITHMETIC)ptrWord & FLEXNVM_START_ADDRESS) {
            FTFL_FCCOB1 |= 0x80;                                         // set address A23 so that the write/erase is in FlexNVM
            iNoInterruptDisable = 1;                                     // this command may execute without interrupts being disabled
        }
    #endif
    #if defined KINETIS_KE
        FTMRH_FCCOBIX = 1;                                               // set the command index
    #endif
        FTFL_FCCOB2 = (unsigned char)(((CAST_POINTER_ARITHMETIC)ptrWord) >> 8);
        FTFL_FCCOB3 = (unsigned char)((CAST_POINTER_ARITHMETIC)ptrWord);
        if (FCMD_PROGRAM == ucCommand) {                                 // program long-word aligned value
    #if defined KINETIS_KE
            FTMRH_FCCOBIX = 2;                                           // set the command index
            FTMRH_FCCOBHI = (unsigned char)(*ptr_ulWord >> 8);
            FTMRH_FCCOBLO = (unsigned char)(*ptr_ulWord);
            FTMRH_FCCOBIX = 3;                                           // set the command index
            FTMRH_FCCOBHI = (unsigned char)(*ptr_ulWord >> 24);
            FTMRH_FCCOBLO = (unsigned char)(*ptr_ulWord >> 16);
    #else
            FTFL_FCCOB7_4 = *ptr_ulWord;                                 // enter the long word to be programmed
    #endif
    #if defined _WINDOWS
          if (*(unsigned long *)fnGetFlashAdd((unsigned char *)ptrWord) != 0xffffffff) { // {36}
                _EXCEPTION("Attempting to write to non-blank flash unit!!");
            }
            *(unsigned long *)fnGetFlashAdd((unsigned char *)ptrWord) = *ptr_ulWord;
    #endif
    #if (FLASH_ROW_SIZE == 8)                                            // {10} write second long word
            ptr_ulWord++;
        #if defined KINETIS_KE
            FTMRH_FCCOBIX = 4;                                           // set the command index
            FTMRH_FCCOBHI = (unsigned char)(*ptr_ulWord >> 8);
            FTMRH_FCCOBLO = (unsigned char)(*ptr_ulWord);
            FTMRH_FCCOBIX = 5;                                           // set the command index
            FTMRH_FCCOBHI = (unsigned char)(*ptr_ulWord >> 24);
            FTMRH_FCCOBLO = (unsigned char)(*ptr_ulWord >> 16);
        #else
            FTFL_FCCOBB_8 = *ptr_ulWord;                                 // enter the second long word to be programmed
        #endif
        #if defined _WINDOWS                                             // {31}
            if (*(unsigned long *)fnGetFlashAdd((unsigned char *)(ptrWord + 1)) != 0xffffffff) { // {36}
                _EXCEPTION("Attempting to write to non-blank flash unit!!");
            }
            *(unsigned long *)fnGetFlashAdd((unsigned char *)(ptrWord + 1)) = *ptr_ulWord;;
        #endif
    #endif
        }
    #if (defined KINETIS_KE && (defined SIZE_OF_EEPROM && SIZE_OF_EEPROM > 0)) // {109}
        else if (FCMD_PROGRAM_EEPROM == ucCommand) {                     // if programming to EEPROM write a single byte
            FTMRH_FCCOBIX = 2;                                           // set the command index
            FTMRH_FCCOBLO = *(unsigned char *)ptr_ulWord;
        #if defined _WINDOWS
            if (*(unsigned char *)fnGetFlashAdd((unsigned char *)ptrWord) != 0xff) {
                _EXCEPTION("Attempting to write to non-blank EEPROM byte!!");
            }
            *(unsigned char *)fnGetFlashAdd((unsigned char *)ptrWord) = *(unsigned char *)ptr_ulWord;
        #endif
            break;
        }
    #endif
    #if defined USE_SECTION_PROGRAMMING                                  // {105}
        else if (ucCommand == FCMD_PROGRAM_SECTOR) {
            FTFL_FCCOB4 = (unsigned char)(*ptr_ulWord >> 8);             // enter the number of long words/phrases to be programmed
            FTFL_FCCOB5 = (unsigned char)(*ptr_ulWord);
        #if defined _WINDOWS
            {
                unsigned long ulProgrammingCycles = ((FTFL_FCCOB4 << 8) | FTFL_FCCOB5);
                unsigned long ulStartAddress = (FTFL_FCCOB1 << 16);
                ulStartAddress |= (FTFL_FCCOB2 << 8);
                ulStartAddress |= (FTFL_FCCOB3);
                ulProgrammingCycles *= FLASH_ROW_SIZE;                   // number of bytes to be programmed
                if (ulProgrammingCycles > FLEXRAM_MAX_SECTION_COPY_SIZE) {
                    _EXCEPTION("Sector write size is too long");
                }
                memcpy(fnGetFlashAdd((unsigned char *)ulStartAddress), (void *)FLEXRAM_START_ADDRESS, ulProgrammingCycles);
            }
        #endif
        }
    #endif
    #if defined SUPPORT_SWAP_BLOCK
        else if (FCMD_SWAP == ucCommand) {
            FTFL_FCCOB7_4 = *ptr_ulWord;
        }
    #endif
        else {
    #if defined _WINDOWS
            fnDeleteFlashSector(fnGetFlashAdd((unsigned char *)ptrWord));// the sector erase must be phrase aligned ([2:0] = 0)
    #endif
        }
        break;
    #if defined SUPPORT_PROGRAM_ONCE                                     // {20}
    case FCMD_READ_ONCE:
        FTFL_FCCOB1 = (unsigned char)(*ptrWord);                         // record index (0..7) for FPU types or (0..15)
        #if defined _WINDOWS
        {
            unsigned char *ptrFlash = fnGetFileSystemStart(0);
            ptrFlash += SIZE_OF_FLASH + ((unsigned char)FTFL_FCCOB1 * FLASH_ROW_SIZE); // move to the location containg this value in simulated flash
            FTFL_FCCOB4 = *ptrFlash++;                                   // load the result registers with the content
            FTFL_FCCOB5 = *ptrFlash++;
            FTFL_FCCOB6 = *ptrFlash++;
            FTFL_FCCOB7 = *ptrFlash++;
            #if (FLASH_ROW_SIZE == 8)
            FTFL_FCCOB8 = *ptrFlash++;
            FTFL_FCCOB9 = *ptrFlash++;
            FTFL_FCCOBA = *ptrFlash++;
            FTFL_FCCOBB = *ptrFlash;
            #endif
        }
        #endif
        break;
    case FCMD_PROGRAM_ONCE:
        FTFL_FCCOB1 = (unsigned char)(*ptrWord);                         // record index (0..7) for FPU types or (0..15)
        FTFL_FCCOB7_4 = *ptr_ulWord;
        #if (FLASH_ROW_SIZE == 8)
        ptr_ulWord++;
        FTFL_FCCOBB_8 = *ptr_ulWord;
        #endif
        #if defined _WINDOWS
        {
            unsigned char cBlank[8] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
            unsigned char *ptrFlash = fnGetFileSystemStart(0);
            ptrFlash += SIZE_OF_FLASH + ((unsigned char)FTFL_FCCOB1 * FLASH_ROW_SIZE);// move to the location containing this value in simulated flash
            if (uMemcmp(ptrFlash, cBlank, FLASH_ROW_SIZE) == 0) {        // if not yet programmed
                *ptrFlash++ = FTFL_FCCOB4;                               // program the content - this can only be done once
                *ptrFlash++ = FTFL_FCCOB5;
                *ptrFlash++ = FTFL_FCCOB6;
                *ptrFlash++ = FTFL_FCCOB7;
            #if (FLASH_ROW_SIZE == 8)
                *ptrFlash++ = FTFL_FCCOB8;
                *ptrFlash++ = FTFL_FCCOB9;
                *ptrFlash++ = FTFL_FCCOBA;
                *ptrFlash   = FTFL_FCCOBB;
            #endif
            }
            else {
                _EXCEPTION("OTP location already programmed and can not be modified!!!");
                return FTFL_STAT_FPVIOL;                                 // signal programming error
            }
        }
        #endif
        break;
    #endif
    }
    #if defined FLEXFLASH_DATA || (defined KINETIS_KE && (defined SIZE_OF_EEPROM && SIZE_OF_EEPROM > 0)) // {82}{109} if working with FlashNMV in data flash mode or EEPROM
    if (iNoInterruptDisable == 0) {
        uDisable_Interrupt();                                            // protect this region from interrupts
    }
    #else
    uDisable_Interrupt();                                                // protect this region from interrupts
    #endif
    #if defined _WINDOWS
    fnFlashRoutine((volatile unsigned char *)FLASH_STATUS_REGISTER);
    #else
    fnRAM_code((volatile unsigned char *)FLASH_STATUS_REGISTER);         // execute the command from SRAM
    #endif
    #if defined FLEXFLASH_DATA || (defined KINETIS_KE && (defined SIZE_OF_EEPROM && SIZE_OF_EEPROM > 0)) // {82}{109} if working with FlashNMV in data flash mode or EEPROM
    if (iNoInterruptDisable == 0) {
        uEnable_Interrupt();                                             // safe to accept interrupts again
    }
    #else
    uEnable_Interrupt();                                                 // safe to accept interrupts again
    #endif

    #if defined SUPPORT_PROGRAM_ONCE                                     // {20}
    if (FCMD_READ_ONCE == ucCommand) {                                   // the result (8 bytes) is in FTFL_FCCOB2/FTFL_FCCOB3
        *ptr_ulWord++ = ((FTFL_FCCOB4 << 24) | (FTFL_FCCOB5 << 16) | (FTFL_FCCOB6 << 8) | FTFL_FCCOB7);
        #if (FLASH_ROW_SIZE == 8)
        *ptr_ulWord   = ((FTFL_FCCOB8 << 24) | (FTFL_FCCOB9 << 16) | (FTFL_FCCOBA << 8) | FTFL_FCCOBB);
        #endif
    }
    #endif
    #if defined SUPPORT_SWAP_BLOCK
    if (FCMD_SWAP == ucCommand) {
        #if defined _WINDOWS
        switch (FTFL_FCCOB4) {
        case SWAP_CONTROL_CODE_REPORT_SWAP_STATUS:
            FTFL_FCCOB5 = CURRENT_SWAP_MODE_UNINITIALISED;               // initial state is usually uninitialised (except when previous operation was interrupted)
            break;
        case SWAP_CONTROL_CODE_INITIALISE_SWAP_SYSTEM:
            {
                unsigned char *ptrFlash = fnGetFileSystemStart(0);
                unsigned long ulOffset = ((FTFL_FCCOB1 << 16) | (FTFL_FCCOB2 << 8) | FTFL_FCCOB3);
                ptrFlash += ulOffset;
                if (*ptrFlash != 0xff) {
                    _EXCEPTION("Swap indicator not in erased state!!");
                }
                *ptrFlash = 0;
                FTFL_FCCOB5 = CURRENT_SWAP_MODE_READY;
            }
            break;
        }
        #endif
        *ptr_ulWord = FTFL_FCCOB7_4;                                     // update with the result
    }
    #endif
    return (FTFL_FSTAT & (FTFL_STAT_ACCERR | FTFL_STAT_FPVIOL | FTFL_STAT_MGSTAT0)); // if there was an error this will be non-zero
}
#endif

#if defined SUPPORT_PROGRAM_ONCE                                         // {20}
// Read and write data from the program once area (64 bytes available which can only be programmed once in 4/8 byte blocks)
// The length is the number of long words to be copied to/from the buffer - FPU devices write 8 bytes at a time other 4 bytes at a time
//
extern int fnProgramOnce(int iCommand, unsigned long *ptrBuffer, unsigned char ucBlockNumber, unsigned char ucLength)
{
    #if (FLASH_ROW_SIZE == 8)
    unsigned long ulBuffer[2];                                           // the devices with phrase programming always read/write 8 bytes at a time
    unsigned char ucIndex8;
    #endif
    unsigned char ucMax = (ucLength + ucBlockNumber);                    // finishing block
    unsigned char ucIndex = ucBlockNumber;                               // starting block
    unsigned char ucCommandType;
    if (iCommand == PROGRAM_ONCE_READ) {                                 // read data from the program once area
        ucCommandType = FCMD_READ_ONCE;
    }
    else {                                                               // program (one time programmable)
        ucCommandType = FCMD_PROGRAM_ONCE;
    }
    for (; ucIndex < ucMax; ) {
    #if (FLASH_ROW_SIZE == 8)
        ucIndex8 = (ucIndex/2);
        if (ucCommandType == FCMD_PROGRAM_ONCE) {                        // writing
            if (ucIndex & 1) {                                           // uneven
                ulBuffer[0] = 0xffffffff;
                ulBuffer[1] = *ptrBuffer++;
                ucIndex++;
            }
            else {
                ulBuffer[0] = *ptrBuffer++;
                if (++ucIndex >= ucMax) {
                    ulBuffer[1] = 0xffffffff;
                }
                else {
                    ulBuffer[1] = *ptrBuffer++;
                }
                ucIndex++;
            }
        }
        if (fnFlashNow(ucCommandType, (unsigned long *)&ucIndex8, ulBuffer) != 0) { // read/write 2 long words at a time
            return -1;                                                   // error
        }
        if (ucCommandType == FCMD_READ_ONCE) {                           // reading
            if (ucIndex & 1) {                                           // uneven
                *ptrBuffer++ = ulBuffer[1];
            }
            else {
                *ptrBuffer++ = ulBuffer[0];
                ucIndex++;
                if (ucIndex >= ucMax) {
                    break;
                }
                *ptrBuffer++ = ulBuffer[1];
            }
            ucIndex++;
        }
    #else
        if (fnFlashNow(ucCommandType, (unsigned long *)&ucIndex, ptrBuffer) != 0) { // read/write 1 long word at a time
            return -1;                                                   // error
        }
        ucIndex++;
        ptrBuffer++;
    #endif
    }
    return 0;                                                            // success
}
#endif

#if defined FLASH_ROUTINES || defined FLASH_FILE_SYSTEM || defined USE_PARAMETER_BLOCK
// This command is used to erase the complete FLASH contents, including the security area
// Since the complete FLASH will be empty the code will no longer run afterwards
//
extern int fnMassEraseFlash(void)
{
    return (fnFlashNow(FCMD_ERASE_ALL_BLOCKS, 0, 0));
}
#endif


#if defined FLASH_ROUTINES || defined ACTIVE_FILE_SYSTEM || defined USE_PARAMETER_BLOCK
static int fnWriteInternalFlash(ACCESS_DETAILS *ptrAccessDetails, unsigned char *ucData)
{
    #if (defined FLASH_ROUTINES || defined FLASH_FILE_SYSTEM) && (defined FLASH_ROW_SIZE && FLASH_ROW_SIZE > 0)
    static unsigned char *ptrOpenBuffer = 0;
    unsigned char *ptrFlashBuffer;
    unsigned long ulBufferOffset;
    MAX_FILE_LENGTH BufferCopyLength;
    MAX_FILE_LENGTH Length = ptrAccessDetails->BlockLength;
    if (ucData == 0) {                                                   // close an open buffer
        ulBufferOffset = ((CAST_POINTER_ARITHMETIC)ptrOpenBuffer & (FLASH_ROW_SIZE - 1));
        if (ulBufferOffset == 0) {
            return 0;                                                    // no open buffer so nothing to do
        }
        ulBufferOffset = FLASH_ROW_SIZE;                                 // cause the open buffer to be saved without copying any input data
        ptrOpenBuffer = (unsigned char *)((CAST_POINTER_ARITHMETIC)ptrOpenBuffer & ~(FLASH_ROW_SIZE - 1));
    }
    else {
        #if (defined KINETIS_KE && (defined SIZE_OF_EEPROM && SIZE_OF_EEPROM > 0)) // {109}
        if ((unsigned char *)ptrAccessDetails->ulOffset >= (unsigned char *)(SIZE_OF_FLASH - SIZE_OF_EEPROM)) { // is the address in EEPROM
            ulBufferOffset = ptrAccessDetails->ulOffset;
        }
        else {
            ptrOpenBuffer = (unsigned char *)(ptrAccessDetails->ulOffset & ~(FLASH_ROW_SIZE - 1)); // set to start of long word or phrase that the address is in
            ulBufferOffset = (ptrAccessDetails->ulOffset & (FLASH_ROW_SIZE - 1)); // offset in the long word or phrase
        }
        #else
        ptrOpenBuffer = (unsigned char *)(ptrAccessDetails->ulOffset & ~(FLASH_ROW_SIZE - 1)); // set to start of long word or phrase that the address is in
        ulBufferOffset = (ptrAccessDetails->ulOffset & (FLASH_ROW_SIZE - 1)); // offset in the long word or phrase
        #endif
    }
    do {                                                                 // handle each byte to be programmed
        #if (defined KINETIS_KE && (defined SIZE_OF_EEPROM && SIZE_OF_EEPROM > 0)) // {109}
        if ((unsigned char *)ulBufferOffset >= (unsigned char *)(SIZE_OF_FLASH - SIZE_OF_EEPROM)) { // is the address in EEPROM
            if ((fnFlashNow(FCMD_PROGRAM_EEPROM, (unsigned long *)ulBufferOffset, (unsigned long *)ucData)) != 0) { // program single bytes
                return 1;                                                // error
            }
            ucData++;
            ulBufferOffset++;
            Length--;
            continue;
        }
        #endif
        #if defined USE_SECTION_PROGRAMMING                              // {105}
        if (ulBufferOffset == 0) {                                       // if the data start is aligned there is a possibility of using accelerated section programming
            MAX_FILE_LENGTH SectionLength = (Length & ~(FLASH_ROW_SIZE - 1)); // round down to full long words/phrases
            if (SectionLength > (FLASH_ROW_SIZE * 2)) {                  // from 2 long words or phrases
                unsigned char *ptrEnd = (unsigned char *)((CAST_POINTER_ARITHMETIC)ptrOpenBuffer & ~(FLASH_GRANULARITY - 1));
                ptrEnd += FLASH_GRANULARITY;                             // pointer to the next flash sector
                if ((ptrOpenBuffer + SectionLength) >= ptrEnd) {         // end of write is past a sector boundary
                    SectionLength = (ptrEnd - ptrOpenBuffer);            // limit
                }
                if (SectionLength > (FLASH_ROW_SIZE * 2)) {              // if still at least 2 long words or phrases to make section write worthwhile
                    if (SectionLength > FLEXRAM_MAX_SECTION_COPY_SIZE) {
                        SectionLength = FLEXRAM_MAX_SECTION_COPY_SIZE;
                    }
                    uMemcpy((void *)FLEXRAM_START_ADDRESS, ucData, SectionLength); // copy the data to the accelerator RAM
                    ulFlashRow[0] = (SectionLength/FLASH_ROW_SIZE);      // the number of long words/phrases to be written to the section
                    if ((fnFlashNow(FCMD_PROGRAM_SECTOR, (unsigned long *)ptrOpenBuffer, &ulFlashRow[0])) != 0) { // write section
                        return 1;                                        // error
                    }
                    ptrOpenBuffer += SectionLength;
                    Length -= SectionLength;
                    ucData += SectionLength;
                    if (Length == 0) {
                        return 0;
                    }
                    continue;
                }
            }
        }
        #endif
        BufferCopyLength = (FLASH_ROW_SIZE - ulBufferOffset);            // remaining buffer space to end of present backup buffer
        if (BufferCopyLength > Length) {                                 // limit in case the amount of bytes to be programmed is less than the long word or phrase involved
            BufferCopyLength = Length;
        }
        ptrFlashBuffer = (unsigned char *)ulFlashRow + ulBufferOffset;   // pointer set in FLASH row backup buffer
        uMemcpy(ptrFlashBuffer, ucData, BufferCopyLength);               // copy the input data to the FLASH row backup buffer
        ucData += BufferCopyLength;
        Length -= BufferCopyLength;                                      // remaining data length
        ptrFlashBuffer += BufferCopyLength;                              // next copy location
        if (ptrFlashBuffer >= ((unsigned char *)ulFlashRow + FLASH_ROW_SIZE)) { // a complete backup buffer is ready to be copied to FLASH
            ptrFlashBuffer = (unsigned char *)ulFlashRow;                // set pointer to start of FLASH row backup buffer
            ulBufferOffset = 0;
            if ((fnFlashNow(FCMD_PROGRAM, (unsigned long *)ptrOpenBuffer, &ulFlashRow[0])) != 0) { // {10} write long word/phrase
                return 1;                                                // error
            }
            ptrOpenBuffer += FLASH_ROW_SIZE;
            uMemset(ulFlashRow, 0xff, FLASH_ROW_SIZE);                   // flush the intermediate buffer
        }
        else {                                                           // incomplete buffer collected
            ptrOpenBuffer += BufferCopyLength;
        }
    } while (Length != 0);
    #endif
    return 0;
}


    #if defined SPI_FLASH_ENABLED
// This routine reads data from the defined device into a buffer. The access details inform of the length to be read (already limited to maximum possible length for the device)
// as well as the address in the specific device
//
static void fnReadSPI(ACCESS_DETAILS *ptrAccessDetails, unsigned char *ptrBuffer)
{
    #if !defined SPI_FLASH_SST25 && !defined SPI_FLASH_W25Q && !defined SPI_FLASH_S25FL1_K
    unsigned short usPageNumber = (unsigned short)(ptrAccessDetails->ulOffset/SPI_FLASH_PAGE_LENGTH); // the page the address is in
    unsigned short usPageOffset = (unsigned short)(ptrAccessDetails->ulOffset - (usPageNumber * SPI_FLASH_PAGE_LENGTH)); // offset in the page
    #endif

    #if defined SPI_FLASH_ST
    fnSPI_command(READ_DATA_BYTES, (unsigned long)((unsigned long)(usPageNumber << 8) | (usPageOffset)), _EXTENDED_CS ptrBuffer, ptrAccessDetails->BlockLength);
    #elif defined SPI_FLASH_SST25 || defined SPI_FLASH_W25Q || defined SPI_FLASH_S25FL1_K
    fnSPI_command(READ_DATA_BYTES, ptrAccessDetails->ulOffset, _EXTENDED_CS ptrBuffer, ptrAccessDetails->BlockLength);
    #else                                                                // ATMEL
        #if SPI_FLASH_PAGE_LENGTH >= 1024
            #if SPI_FLASH_PAGE_LENGTH == 1024                            // {108}
    fnSPI_command(CONTINUOUS_ARRAY_READ, (unsigned long)((unsigned long)(usPageNumber << 10) | (usPageOffset)), _EXTENDED_CS ptrBuffer, ptrAccessDetails->BlockLength);
            #else
    fnSPI_command(CONTINUOUS_ARRAY_READ, (unsigned long)((unsigned long)(usPageNumber << 11) | (usPageOffset)), _EXTENDED_CS ptrBuffer, ptrAccessDetails->BlockLength);
            #endif
        #elif SPI_FLASH_PAGE_LENGTH >= 512
            #if SPI_FLASH_PAGE_LENGTH == 512                            // {108}
    fnSPI_command(CONTINUOUS_ARRAY_READ, (unsigned long)((unsigned long)(usPageNumber << 9) | (usPageOffset)), _EXTENDED_CS ptrBuffer, ptrAccessDetails->BlockLength);
            #else
    fnSPI_command(CONTINUOUS_ARRAY_READ, (unsigned long)((unsigned long)(usPageNumber << 10) | (usPageOffset)), _EXTENDED_CS ptrBuffer, ptrAccessDetails->BlockLength);
            #endif
        #else
            #if SPI_FLASH_PAGE_LENGTH == 256                            // {108}
    fnSPI_command(CONTINUOUS_ARRAY_READ, (unsigned long)((unsigned long)(usPageNumber << 8) | (usPageOffset)), _EXTENDED_CS ptrBuffer, ptrAccessDetails->BlockLength);
            #else
    fnSPI_command(CONTINUOUS_ARRAY_READ, (unsigned long)((unsigned long)(usPageNumber << 9) | (usPageOffset)), _EXTENDED_CS ptrBuffer, ptrAccessDetails->BlockLength);
            #endif
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
    #if defined SPI_FLASH_ST || defined SPI_FLASH_W25Q || defined SPI_FLASH_S25FL1_K
        if (usDataLength > (SPI_FLASH_PAGE_LENGTH - usPageOffset)) {
            usDataLength = (SPI_FLASH_PAGE_LENGTH - usPageOffset);
        }
      //#if !defined SPI_FLASH_S25FL1_K                                  // {1}
        fnSPI_command(WRITE_ENABLE, 0, _EXTENDED_CS 0, 0);               // write enable
      //#endif
        #if defined SPI_FLASH_W25Q || defined SPI_FLASH_S25FL1_K
        fnSPI_command(PAGE_PROG, ((usPageNumber * SPI_FLASH_PAGE_LENGTH)) | usPageOffset, _EXTENDED_CS ptrBuffer, usDataLength); // copy new content
        #else
        fnSPI_command(PAGE_PROG, ((usPageNumber << 8) | usPageOffset), _EXTENDED_CS ptrBuffer, usDataLength); // copy new content
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
        fnSPI_command(WRITE_BUFFER_1, usPageOffset, _EXTENDED_CS ptrBuffer, usDataLength); // copy new content
        fnSPI_command(PROG_FROM_BUFFER_1, usPageNumber, _EXTENDED_CS 0, 0); // program to main memory
        Length -= usDataLength;
        ptrBuffer += usDataLength;
        usPageNumber++;
        usPageOffset = 0;
    #endif
    }
    #if defined SPI_FLASH_SST25 || defined SPI_FLASH_W25Q
    fnSPI_command(WRITE_DISABLE, 0, _EXTENDED_CS 0, 0);                  // disable writes on exit
    #endif
}

// The routine is used to delete an area in SPI Flash, whereby the caller has set the address to the start of a page and limited the erase to a single storage area and device
//
static MAX_FILE_LENGTH fnDeleteSPI(ACCESS_DETAILS *ptrAccessDetails)
{
    MAX_FILE_LENGTH BlockLength = SPI_FLASH_PAGE_LENGTH;
    #if !defined SPI_FLASH_ST
    unsigned char  ucCommand;
    #endif
    #if !defined SPI_FLASH_SST25 && !defined SPI_FLASH_W25Q && !defined SPI_FLASH_S25FL1_K
    unsigned short usPageNumber = (unsigned short)(ptrAccessDetails->ulOffset/SPI_FLASH_PAGE_LENGTH); // the page the address is in
    #endif
    #if defined SPI_FLASH_ST
    fnSPI_command(WRITE_ENABLE, 0, _EXTENDED_CS 0, 0);                   // enable the write/erase
        #if defined SPI_DATA_FLASH
    fnSPI_command(SUB_SECTOR_ERASE, ((unsigned long)usPageNumber << 8), _EXTENDED_CS 0, 0); // delete appropriate sub-sector
    BlockLength = SPI_FLASH_SUB_SECTOR_LENGTH;
        #else
    fnSPI_command(SECTOR_ERASE, ((unsigned long)usPageNumber << 8), _EXTENDED_CS 0, 0); // delete appropriate sector
    BlockLength = SPI_FLASH_SECTOR_LENGTH;
        #endif
    #elif defined SPI_FLASH_SST25 || defined SPI_FLASH_W25Q || defined SPI_FLASH_S25FL1_K
      //#if !defined SPI_FLASH_S25FL1_K                                  // {1}
    fnSPI_command(WRITE_ENABLE, 0, _EXTENDED_CS 0, 0);                   // command write enable to allow byte programming
      //#endif
        #if !defined SST25_A_VERSION
    if ((ptrAccessDetails->BlockLength >= (64 * 1024)) && ((ptrAccessDetails->ulOffset & ((64 * 1024) - 1)) == 0)) { // if a complete 64k block can be deleted
            #if defined SPI_FLASH_S25FL1_K
        ucCommand = BLOCK_ERASE;                                         // delete block of 64k
            #else
        ucCommand = SECTOR_ERASE;                                        // delete block of 64k
            #endif
        BlockLength = (64 * 1024);
    }
    else 
        #endif
        #if defined SPI_FLASH_S25FL1_K
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

    #if !defined ONLY_INTERNAL_FLASH_STORAGE

// Search for the memory type that the starting address is in, return the type and restrict the largest length that can be read,written, erased from that location
//
extern unsigned char fnGetStorageType(unsigned char *memory_pointer, ACCESS_DETAILS *ptrAccessDetails)
{
    unsigned long ulMaximumLength;
    STORAGE_AREA_ENTRY *ptrStorageList = UserStorageListPtr;
    #if defined _WINDOWS                                                 // this is only used when reading with the simulator
    if (iFetchingInternalMemory != _ACCESS_NOT_IN_CODE) {                // if the access is to program memory when simulating
        iFetchingInternalMemory = _ACCESS_NOT_IN_CODE;                   // reset single-shot program access
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
#if (defined KINETIS_KE && (defined SIZE_OF_EEPROM && SIZE_OF_EEPROM > 0)) // {109}
    unsigned short _FLASH_GRANULARITY = FLASH_GRANULARITY;               // assume program flash granularity
#else
    #define _FLASH_GRANULARITY FLASH_GRANULARITY
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
            Length += (((CAST_POINTER_ARITHMETIC)ptrSector) - ((CAST_POINTER_ARITHMETIC)ptrSector & ~(_FLASH_GRANULARITY - 1)));
            ptrSector = (unsigned char *)((CAST_POINTER_ARITHMETIC)ptrSector & ~(_FLASH_GRANULARITY - 1)); // set to sector boundary
            if ((fnFlashNow(FCMD_ERASE_FLASH_SECTOR, (unsigned long *)(unsigned long)ptrSector, (unsigned long)0)) != 0) { // {53}
                return -1;                                               // error
            }
            AccessDetails.BlockLength = _FLASH_GRANULARITY;
            break;
        #if defined SPI_FLASH_ENABLED
        case _STORAGE_SPI_FLASH:
            {
                MAX_FILE_LENGTH PageBoundaryOffset = (MAX_FILE_LENGTH)(((CAST_POINTER_ARITHMETIC)(AccessDetails.ulOffset))%SPI_FLASH_PAGE_LENGTH); // {50}
                Length += PageBoundaryOffset;                            // include length back to start of page
                ptrSector -= PageBoundaryOffset;                         // set to page boundary
                AccessDetails.BlockLength = fnDeleteSPI(&AccessDetails); // delete page/block in SPI flash 
            }
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
    } while (1);
    #else                                                                // case when only internal Flash is available
        #if (defined KINETIS_KE && (defined SIZE_OF_EEPROM && SIZE_OF_EEPROM > 0)) // {109}
    if (ptrSector >= (unsigned char *)(SIZE_OF_FLASH - SIZE_OF_EEPROM)) {// is the sector in EEPROM
        _FLASH_GRANULARITY = KE_EEPROM_GRANULARITY;                      // set EEPROM granularity
    }
        #endif
    Length += (((CAST_POINTER_ARITHMETIC)ptrSector) - ((CAST_POINTER_ARITHMETIC)ptrSector & ~(_FLASH_GRANULARITY - 1)));
    ptrSector = (unsigned char *)((CAST_POINTER_ARITHMETIC)ptrSector & ~(_FLASH_GRANULARITY - 1)); // set to sector boundary
    do {
        if ((fnFlashNow(FCMD_ERASE_FLASH_SECTOR, (unsigned long *)ptrSector, (unsigned long)0)) != 0) { // erase a single sector
            return 1;                                                    // error
        }
        if (Length <= _FLASH_GRANULARITY) {                              // check whether entire deletion has completed
        #if defined MANAGED_FILES
            if (OriginalLength == 0) {                                   // if a single page erase was called, return the page size of the storage area
	            return _FLASH_GRANULARITY;
	        }
        #endif
            break;
        }
        ptrSector += _FLASH_GRANULARITY;                                 // advance sector point to next internal flash sector
        Length -= _FLASH_GRANULARITY;
    } while (1);
    #endif
    return 0;
}


extern int fnWriteBytesFlash(unsigned char *ucDestination, unsigned char *ucData, MAX_FILE_LENGTH Length)
{
    ACCESS_DETAILS AccessDetails;
    AccessDetails.BlockLength = Length;
    #if !defined ONLY_INTERNAL_FLASH_STORAGE
    while (1/*Length != 0*/) {                                           // {24} allow zero length write to ensure that open flash buffer can be closed
        switch (fnGetStorageType(ucDestination, &AccessDetails)) {       // get the storage type based on the memory location and also return the largest amount of data that can be read from a single device
        case _STORAGE_INTERNAL_FLASH:
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
        AccessDetails.BlockLength = Length;                              // synchronise individual accesses to remaining length
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
    if (iFetchingInternalMemory != _ACCESS_NOT_IN_CODE) {
        uMemcpy(ptrValue, ParLocation, Size);
        return;
    }
    #endif
    #if !defined NO_FLASH_SUPPORT
        #if !defined ONLY_INTERNAL_FLASH_STORAGE
    {
        ACCESS_DETAILS AccessDetails;
        while (Size != 0) {
            AccessDetails.BlockLength = Size;                            // synchronise individual accesses to remaining length
            switch (fnGetStorageType(ParLocation, &AccessDetails)) {     // get the storage type based on the memory location and also return the largest amount of data that can be read from a single device
        #if !defined _WINDOWS
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
            #if defined FLEXFLASH_DATA                                   // {82} if working with FlashNMV in data flash mode
    if (ParLocation >= (unsigned char *)(SIZE_OF_FLASH - SIZE_OF_FLEXFLASH)) { // is the access a read from FlexNVM
        ParLocation += (FLEXNVM_START_ADDRESS - (SIZE_OF_FLASH - SIZE_OF_FLEXFLASH)); // convert to FlexNVM physical address
    }
            #elif (defined KINETIS_KE && (defined SIZE_OF_EEPROM && SIZE_OF_EEPROM > 0)) // {109}
    if (ParLocation >= (unsigned char *)(SIZE_OF_FLASH - SIZE_OF_EEPROM)) { // is the access a read from EEPROM
        ParLocation += (KE_EEPROM_START_ADDRESS - (SIZE_OF_FLASH - SIZE_OF_EEPROM)); // convert to EEPROM physical address
    }
            #endif
    uMemcpy(ptrValue, fnGetFlashAdd(ParLocation), Size);                 // directly copy memory since this must be a pointer to code (embedded file)
        #endif
    #endif
}
#endif

#if defined SUPPORT_SWAP_BLOCK
extern int fnSwapMemory(int iCheck)
{
    #define FLASH_SWAP_INDICATOR_ADDRESS    ((FLASH_START_ADDRESS + (SIZE_OF_FLASH/2)) - 32) // final sector in the first half of flash memory used as flash swap indicator
    unsigned long ulCommand;
    while (1) {
        ulCommand = ((SWAP_CONTROL_CODE_REPORT_SWAP_STATUS << 24) | 0x00ffffff); // note that the unused bytes in the command are set to 0xff so that it is clear whether they are changed by the operation (failed operatios may otherwise not be detectable)
        if (fnFlashNow(FCMD_SWAP, (unsigned long *)FLASH_SWAP_INDICATOR_ADDRESS, &ulCommand) != 0) { // get the swap state
            return SWAP_COMMAND_FAILURE;                                 // error
        }
      //fnDebugHex(ulCommand, (WITH_LEADIN | WITH_CR_LF | sizeof(ulCommand)));
        switch ((unsigned char)(ulCommand >> 16)) {                      // switch on current swap mode
        case CURRENT_SWAP_MODE_UNINITIALISED:
            {
                int iPhrases = 0;
                unsigned long *ptrSwapIndicator = (unsigned long *)FLASH_SWAP_INDICATOR_ADDRESS;
                if (iCheck != 0) {                                       // if checking present state
                    return SWAP_STATE_UNINITIALISED;                     // uninitialised
                }
                while (iPhrases++ < ((FLASH_ROW_SIZE/sizeof(unsigned long)) * 2)) { // for each of the two phrases of each block containing the swap indicator
                    if (*(unsigned long *)fnGetFlashAdd((unsigned char *)ptrSwapIndicator) != 0xffffffff) {
                        fnDebugMsg("Cleaning swap indicator sector\r\n");
                        if ((fnFlashNow(FCMD_ERASE_FLASH_SECTOR, ptrSwapIndicator, (unsigned long)0)) != 0) { // erase the sector
                            return SWAP_ERASE_FAILURE;
                        }
                    }
                    if (iPhrases == (FLASH_ROW_SIZE/sizeof(unsigned long))) { // after checking the two phrases in the first block move to the second block
                        ptrSwapIndicator = (unsigned long *)(FLASH_SWAP_INDICATOR_ADDRESS + (SIZE_OF_FLASH/2)); // location in the second block
                    }
                    else {
                        ptrSwapIndicator++;                              // move to the next long word
                    }
                }
                ulCommand = ((SWAP_CONTROL_CODE_INITIALISE_SWAP_SYSTEM << 24) | 0x00ffffff); // set the flash swap indicator address to initialise the swap mechanism (this address is programmed to IFR swap memory and 0xff00 programmed to the swap indicator address in the first block)
                if (fnFlashNow(FCMD_SWAP, (unsigned long *)FLASH_SWAP_INDICATOR_ADDRESS, &ulCommand) != 0) { // start by initialising the swap process
                    return SWAP_COMMAND_FAILURE;
                }
                while (FTFL_FCCOB5 == CURRENT_SWAP_MODE_UNINITIALISED) {}// the current swap mode may take a little time to complete so wait until the new state is indicated
            }
            break;
        case CURRENT_SWAP_MODE_READY:
            ulCommand = ((SWAP_CONTROL_CODE_SET_SWAP_IN_UPDATE_STATE << 24) | 0x00ffffff);
            if (fnFlashNow(FCMD_SWAP, (unsigned long *)FLASH_SWAP_INDICATOR_ADDRESS, &ulCommand) != 0) {
                return SWAP_COMMAND_FAILURE;                             // error
            }
            while (FTFL_FCCOB5 == CURRENT_SWAP_MODE_READY) {}            // the current swap mode may take a little time to complete so wait until the new state is indicated
            break;
        case CURRENT_SWAP_MODE_UPDATE:
            if (iCheck != 0) {                                           // checking and not swapping
                if ((unsigned char)(ulCommand >> 8) != 0) {              // check the active block
                    return SWAP_STATE_USING_1;                           // block 1 is being used
                }
                else {
                    return SWAP_STATE_USING_0;                           // block 0 is being used
                }
            }
            else {
                // We are performing a swap from one active block to another
                // - this requires erasing the swap indicator of the non-active block
                //
                if ((fnFlashNow(FCMD_ERASE_FLASH_SECTOR, (unsigned long *)(FLASH_SWAP_INDICATOR_ADDRESS + (SIZE_OF_FLASH/2)), (unsigned long)0)) != 0) { // erase the sector
                    return SWAP_ERASE_FAILURE;
                }
            }
            // Fall through intentionally
            //
        case CURRENT_SWAP_MODE_UPDATE_ERASED:
            ulCommand = ((SWAP_CONTROL_CODE_SET_SWAP_IN_COMPLETE_STATE << 24) | 0x00ffffff);
            if (fnFlashNow(FCMD_SWAP, (unsigned long *)FLASH_SWAP_INDICATOR_ADDRESS, &ulCommand) != 0) {
                    return SWAP_COMMAND_FAILURE;
            }
            while (FTFL_FCCOB5 != CURRENT_SWAP_MODE_COMPLETE) {}         // the current swap mode may take a little time to complete so wait until the new state is indicated
            // Fall through intentionally
            //
        case CURRENT_SWAP_MODE_COMPLETE:
            return SWAP_STATE_SWAPPED;                                   // swap has completed - a reset is required to effect the new operation
        default:
            break;                                                       // unexpected - continue until a valid state is returned
        }
    }
    return 0;
}
#endif

#if defined USE_PARAMETER_BLOCK

#define PARAMETER_BLOCK_1 (unsigned long *)(PARAMETER_BLOCK_START)
#define PARAMETER_BLOCK_2 (unsigned long *)(PARAMETER_BLOCK_START + PARAMETER_BLOCK_SIZE)

#if (FLASH_ROW_SIZE == 8)                                                // {10}
    #define PARAMETER_STATUS_SIZE   4                                    // status is 8 bytes long to match the  phrase programming requirement of these parts
#else
    #define PARAMETER_STATUS_SIZE   2                                    // status is 4 bytes long to match the long word programming requirement of these parts
#endif

// Kinetis has 2k (4k for FPU parts) byte blocks in program Flash which can be individually modified so we use one of these, and a backup if desired
// Kinetis KL has 1k byte blocks
//
extern unsigned char fnGetValidPars(unsigned char ucValid)
{
    unsigned long ulValidUse[PARAMETER_STATUS_SIZE];
    unsigned long ulCompare;

    fnGetParsFile((unsigned char *)PARAMETER_BLOCK_1, (unsigned char *)ulValidUse, sizeof(ulValidUse)); // the first 2 long word locations (4 with fast devices) define the validity of the block. 0x55555555 0x55555555 means it is fully valid. 0x55555555 0xffffffff means it contains temporary unvalidated data

    if (PRESENT_TEMP_BLOCK != ucValid) {
        ulCompare = 0x55555555;                                          // we are looking for validated pattern
    }
    else {
        ulCompare = 0xffffffff;                                          // we are looking for temporary pattern
    }

    if ((ulValidUse[0] == 0x55555555) && (ulValidUse[PARAMETER_STATUS_SIZE/2] == ulCompare)) {
        return 1;                                                        // first block matched request
    }
    #if defined USE_PAR_SWAP_BLOCK
    else {
        fnGetParsFile((unsigned char *)PARAMETER_BLOCK_2, (unsigned char *)ulValidUse, sizeof(ulValidUse));
        if ((ulValidUse[0] == 0x55555555) && (ulValidUse[PARAMETER_STATUS_SIZE/2] == ulCompare)) {
            return 2;                                                    // second block matched request
        }
    }
    #endif
    return 0;                                                            // all other cases indicate the searched block doesn't exist
}

// Retrieve parameter from the defined block
//
extern int fnGetParameters(unsigned char ucValidBlock, unsigned short usParameterReference, unsigned char *ucValue, unsigned short usLength)
{
    unsigned long *ptrPar = PARAMETER_BLOCK_1;
    #if defined USE_PAR_SWAP_BLOCK
    if (ucValidBlock == 2) {
        ptrPar += (PARAMETER_BLOCK_2 - PARAMETER_BLOCK_1);
    }
    #endif

    ptrPar += PARAMETER_STATUS_SIZE;                                     // first parameter starts after validation information
    #if defined PARAMETER_NO_ALIGNMENT                                   // {35}
    ptrPar = (unsigned long *)((unsigned char *)ptrPar + usParameterReference); // move to the location of the parameter using a pointer to bytes
    fnGetParsFile((unsigned char *)ptrPar, ucValue, usLength);
    #else
    ptrPar += usParameterReference;                                      // move to the long word location where the parameter value is stored
        #if (FLASH_ROW_SIZE == 8)
    ptrPar += usParameterReference;                                      // {10} move to double-long word location
        #endif

    while (usLength--) {
        fnGetParsFile((unsigned char *)ptrPar, ucValue++, 1);
        ptrPar += (PARAMETER_STATUS_SIZE/2);
    }
    #endif
    return 0;
}


// Delete all sectors contained in a parameter block
//
static int fnDeleteParBlock(unsigned char *ptrAddInPar)
{
    return (fnEraseFlashSector(ptrAddInPar, PARAMETER_BLOCK_SIZE));
}


// Write parameters to the defined block
//
extern int fnSetParameters(unsigned char ucValidBlock, unsigned short usParameterReference, unsigned char *ucValue, unsigned short usLength)
{
    unsigned long *ptrPar = PARAMETER_BLOCK_1, *ptrStart;
    unsigned long ulValid = 0x55555555;
    int iBlockUse = 0;
    unsigned long ulValidCheck[PARAMETER_STATUS_SIZE/2];

    if (TEMP_PARS & ucValidBlock) {
        ucValidBlock &= ~TEMP_PARS;
        iBlockUse = 1;
    }

    if (ucValidBlock & BLOCK_INVALID) {                                  // no valid parameter blocks have been found so we can use the first for saving the data.
        fnDeleteParBlock((unsigned char *)PARAMETER_BLOCK_1);            // we delete it to be sure it is fresh
    #if defined USE_PAR_SWAP_BLOCK
        fnDeleteParBlock((unsigned char *)PARAMETER_BLOCK_2);
    #endif
        ucValidBlock = 1;
    }

    #if defined USE_PAR_SWAP_BLOCK
    if (ucValidBlock == 2) {
        ptrPar = PARAMETER_BLOCK_2;
    }
    #endif
    ptrStart = ptrPar;
    ptrPar += PARAMETER_STATUS_SIZE;                                     // first parameter starts after validation information
    #if defined PARAMETER_NO_ALIGNMENT                                   // {35}
    ptrPar = (unsigned long *)((unsigned char *)ptrPar + usParameterReference); // move to the location of the parameter using a pointer to bytes
    #else
    ptrPar += usParameterReference;                                      // move to the long word location where the parameter value is stored
        #if (FLASH_ROW_SIZE == 8)
    ptrPar += usParameterReference;                                      // {10} move to double-long word (phrase) location
        #endif
    #endif

    if (usLength == 0) {                                                 // command rather than normal write
        if (iBlockUse != 0) {                                            // if we have a new set waiting, set it to use state
            iBlockUse = 0;
    #if defined USE_PAR_SWAP_BLOCK
            if (ptrStart == PARAMETER_BLOCK_1) {
                ptrPar = PARAMETER_BLOCK_2;
            }
            else {
                ptrPar = PARAMETER_BLOCK_1;
            }
            fnDeleteParBlock((unsigned char *)ptrPar);
    #endif
        }
        else {                                                           // we must delete the block
            fnDeleteParBlock((unsigned char *)ptrStart);
            if (usParameterReference != 0) {
                return 0;                                                // we don't validate it again
            }
        }
    }
    else {
    #if defined PARAMETER_NO_ALIGNMENT                                   // {35}
        #if defined _WINDOWS                                             // when simulating the write size to internal flash is checked since each write is expected to be a multiple of the flash write size
        if (ptrPar < (unsigned long *)SIZE_OF_FLASH) {                   // parameter memory is in the internal flash
            if ((usLength % FLASH_ROW_SIZE) != 0) {                      // check write block size which must always be a multiple of the flash write size
                _EXCEPTION("When using the option PARAMETER_NO_ALIGNMENT all parameter block writes must be of the correct modularity size");
            }
            if (((CAST_POINTER_ARITHMETIC)ptrPar & (FLASH_ROW_SIZE - 1)) != 0) { // check the alignment of the write
                _EXCEPTION("When using the option PARAMETER_NO_ALIGNMENT all parameter block writes must be aligned to a flash long word/phrase");
            }
        }
        #endif
        fnWriteBytesFlash((unsigned char *)ptrPar, ucValue, usLength);   // it is expected that the user writes blocks of parameter data that is aligned and packed to respect the characteristics of the flash memory being used
    #else
        #if (FLASH_ROW_SIZE == 8)                                        // {10}
        ulValidCheck[1] = 0xffffffff;
        #endif
        while (usLength--) {
        #if defined _WINDOWS || defined _LITTLE_ENDIAN
            ulValidCheck[0] = (0xffffff00 | *ucValue++);
        #else
            ulValidCheck[0] = (0x00ffffff | (*ucValue++ << 24));
        #endif
            fnWriteBytesFlash((unsigned char *)ptrPar, (unsigned char *)&ulValidCheck[0], sizeof(ulValidCheck)); // write each parameter to FLASH (each byte occupies a long word - 2 long words for fast parts)
            ptrPar += (PARAMETER_STATUS_SIZE/2);
        }
    #endif
    }
    fnGetParsFile((unsigned char *)ptrStart, (unsigned char *)&ulValidCheck, sizeof(ulValidCheck));
    if (ulValidCheck[0] != ulValid) {
        ulValidCheck[0] = ulValid;
        fnWriteBytesFlash((unsigned char *)ptrStart, (unsigned char *)&ulValidCheck[0], sizeof(ulValidCheck)); // validate data
    }
    if (iBlockUse == 0) {
        fnGetParsFile((unsigned char *)(ptrStart + (PARAMETER_STATUS_SIZE/2)), (unsigned char *)&ulValidCheck, sizeof(ulValidCheck));
        if (ulValidCheck[0] != ulValid) {
            ulValidCheck[0] = ulValid;
            fnWriteBytesFlash((unsigned char *)(ptrStart + (PARAMETER_STATUS_SIZE/2)), (unsigned char *)&ulValidCheck[0], sizeof(ulValidCheck)); // use data
        }
    }
    return 0;
}
#endif
