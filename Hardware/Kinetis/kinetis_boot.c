/**********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher

    ---------------------------------------------------------------------
    File:      kinetis_boot.c
    Project:   Single Chip Embedded Internet - boot loader
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2016
    *********************************************************************
    17.04.2012 Add start_application() and flash support for KINETIS_K_FPU types {1}
    25.08.2013 Allow user defined start-up code immediately after the watchdog configuration and before clock configuration to be defined {2}
    18.06.2014 Implement fnFlashRoutine() as assemble code to avoid possibility of compilers in-lining it {3}
    27.01.2015 Add Green Hills project support (_COMPILE_GHS)            {4}

    */

#ifdef _KINETIS

#ifdef _WINDOWS
    #include "config.h"
    #define INITHW  extern
    extern void fnOpenDefaultHostAdapter(void);
    extern void fec_txf_isr(void);
    extern void fnSimulateDMA(int channel);
    #define START_CODE 0
#else
    #define OPSYS_CONFIG                                                 // this module owns the operating system configuration
    #define INITHW  static
    #include "config.h"
    #define  fnSimulateDMA(x)
    #if defined ROWLEY || defined _KDS
        #define asm(x) __asm__(x)
    #endif
    #if defined _COMPILE_KEIL
        #define START_CODE main
    #elif defined _COMPILE_IAR
        #define START_CODE disable_watchdog
    #elif defined _COMPILE_GHS                                           // {4}
        extern void _start_T(void);                                      // GHS library initialisation routine
        #define START_CODE disable_watchdog
    #else
        #define START_CODE main
    #endif
#endif

#ifdef SPI_SW_UPLOAD
    #define SPI_FLASH_DEVICE_COUNT 1
    static int SPI_FLASH_Danger[SPI_FLASH_DEVICE_COUNT] = {0};           // signal that the FLASH status should be checked before using since there is a danger that it is still busy
    static unsigned char ucSPI_FLASH_Type[SPI_FLASH_DEVICE_COUNT];
    #define _EXTENDED_CS
#endif
static unsigned long ulFlashRow[FLASH_ROW_SIZE/sizeof(unsigned long)];   // FLASH row backup buffer (on word boundary)


#if defined _COMPILE_IAR || defined _COMPILE_GHS                         // {4}
extern void __iar_program_start(void);                                   // IAR library initialisation routine
// This is the first function called so that it can immediately disable the watchdog so that it doesn't fire during variable initialisation
//
static void disable_watchdog(void)
{
    UNLOCK_WDOG();                                                       // enable watchdog modification
    CONFIGURE_WATCHDOG();                                                // allow user configuration of internal watch dog timer
    #if defined USER_STARTUP_CODE                                        // {2} allow user defined start-up code immediately after the watchdog configuration and before clock configuration to be defined
    USER_STARTUP_CODE;
    #endif
    #if defined _COMPILE_GHS                                             // {4}
    _start_T();                                                          // now call the GHS initialisation code which initialises variables and then calls main() 
    #else
    __iar_program_start();                                               // now call the IAR initialisation code which initialises variables and then calls main() 
    #endif
}
#endif


// CRC-16 routine
//
extern unsigned short fnCRC16(unsigned short usCRC, unsigned char *ptrInput, unsigned short usBlockSize)
{
    while (usBlockSize--) {
        usCRC = (unsigned char)(usCRC >> 8) | (usCRC << 8);
        usCRC ^= *ptrInput++;
        usCRC ^= (unsigned char)(usCRC & 0xff) >> 4;
        usCRC ^= (usCRC << 8) << 4;
        usCRC ^= ((usCRC & 0xff) << 4) << 1;
    }
    return usCRC;
}

extern void fnDelayLoop(unsigned long ulDelay_us)
{
    volatile int i_us;
    while (ulDelay_us--) {                                               // for each us required
        i_us = (CORE_CLOCK/8300000);
        while (i_us--) {}                                                // simple loop tuned to perform us timing
    }
}

// memset implementation
//
extern void *uMemset(void *ptrTo, unsigned char ucValue, size_t Size)
{
    void *buffer = ptrTo;
    unsigned char *ptr = (unsigned char *)ptrTo;

    while (Size--) {
        *ptr++ = ucValue;
    }

    return buffer;
}

// memcpy implementation
//
extern void *uMemcpy(void *ptrTo, const void *ptrFrom, size_t Size)
{
    void *buffer = ptrTo;
    unsigned char *ptr1 = (unsigned char *)ptrTo;
    unsigned char *ptr2 = (unsigned char *)ptrFrom;

    while (Size--) {
        *ptr1++ = *ptr2++;
    }

    return buffer;
}

// This routine runs from SRAM - the reason why the pointer is passed is to avoid the routine taking it from a const value in FLASH, which is then not code location independent
//
    #if defined _WINDOWS
static void fnFlashRoutine(volatile unsigned char *ptrFTFL_BLOCK)
{
    *ptrFTFL_BLOCK = FTFL_STAT_CCIF;                                     // launch the command - this clears the FTFL_STAT_CCIF flag (register is FTFL_FSTAT)
    while (!(*ptrFTFL_BLOCK & FTFL_STAT_CCIF)) {}                        // wait for the command to terminate
}
    #else                                                                // {3}
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
    if (!fnRAM_code) {                                                   // the first time this is used it will load the program to SRAM
        #define PROG_WORD_SIZE 30                                        // adequate space for the small program
        int i = 0;
        unsigned char *ptrThumb2 = (unsigned char *)fnFlashRoutine;
        static unsigned short usProgSpace[PROG_WORD_SIZE];               // make space for the routine on stack (this will have an even boundary)

        ptrThumb2 =  (unsigned char *)(((CAST_POINTER_ARITHMETIC)ptrThumb2) & ~0x1); // thumb 2 address
        while (i < PROG_WORD_SIZE) {                                     // copy program to SRAM
            usProgSpace[i++] = *(unsigned short *)ptrThumb2;
            ptrThumb2 += sizeof (unsigned short);
        }
        ptrThumb2 = (unsigned char *)usProgSpace;
        ptrThumb2++;                                                     // create a thumb 2 call
        fnRAM_code = (void(*)(volatile unsigned char *))(ptrThumb2);
    }
    while (!(FTFL_FSTAT & FTFL_STAT_CCIF)) {}                            // wait for previous commands to complete

    if (FTFL_FSTAT & (FTFL_STAT_ACCERR | FTFL_STAT_FPVIOL)) {            // check for errors in previous command 
        FTFL_FSTAT = (FTFL_STAT_ACCERR | FTFL_STAT_FPVIOL);              // clear old errors
    }
     
    switch (FTFL_FCCOB0 = ucCommand) {                                   // enter the command sequence
    case FCMD_ERASE_FLASH_SECTOR:
    case FCMD_PROGRAM:
        FTFL_FCCOB1 = (unsigned char)(((CAST_POINTER_ARITHMETIC)ptrWord) >> 16); // set address in flash
        FTFL_FCCOB2 = (unsigned char)(((CAST_POINTER_ARITHMETIC)ptrWord) >> 8);
        FTFL_FCCOB3 = (unsigned char)((CAST_POINTER_ARITHMETIC)ptrWord);
        if (ucCommand == FCMD_PROGRAM) {                                 // program long-word aligned value
            FTFL_FCCOB4 = (unsigned char)(*ptr_ulWord >> 24);            // enter the long word to be programmed
            FTFL_FCCOB5 = (unsigned char)(*ptr_ulWord >> 16);
            FTFL_FCCOB6 = (unsigned char)(*ptr_ulWord >> 8);
            FTFL_FCCOB7 = (unsigned char)(*ptr_ulWord);
    #ifdef _WINDOWS
            *(unsigned long *)fnGetFlashAdd((unsigned char *)ptrWord) = *ptr_ulWord;
    #endif
    #if defined KINETIS_K_FPU                                            // write second long word {1}
            ptr_ulWord++;
            FTFL_FCCOB8 = (unsigned char)(*ptr_ulWord >> 24);            // enter the second long word to be programmed
            FTFL_FCCOB9 = (unsigned char)(*ptr_ulWord >> 16);
            FTFL_FCCOBA = (unsigned char)(*ptr_ulWord >> 8);
            FTFL_FCCOBB = (unsigned char)(*ptr_ulWord);
    #endif
    #ifdef _WINDOWS
            *(unsigned long *)fnGetFlashAdd((unsigned char *)(ptrWord + 1)) = *ptr_ulWord;;
    #endif
        }
        else {
    #ifdef _WINDOWS
            fnDeleteFlashSector(fnGetFlashAdd((unsigned char *)ptrWord));// the sector erase must be phrase aligned ([2:0] = 0)
    #endif
        }
        break;
    }

#if defined _WINDOWS
    fnFlashRoutine((volatile unsigned char *)FTFL_BLOCK);
#else
    fnRAM_code((volatile unsigned char *)FTFL_BLOCK);                    // execute the command from SRAM
#endif

    return (FTFL_FSTAT & (FTFL_STAT_ACCERR | FTFL_STAT_FPVIOL | FTFL_STAT_MGSTAT0)); // if there was an error this will be non-zero
}

#ifdef SPI_SW_UPLOAD
    #ifdef SPI_FLASH_SST25
static unsigned char *fnGetSPI_FLASH_address(unsigned char *ucDestination)
{
    ucDestination -= (SPI_FLASH_START);                                  // location relative to the start of the SPI FLASH chip address
    return ucDestination;
}
    #else
// Return the page number and optionally the address offset in the page
//
static unsigned short fnGetSPI_FLASH_location(unsigned char *ptrSector, unsigned short *usPageOffset)
{
    unsigned short usPageNumber;
    ptrSector -= (SPI_FLASH_START);                                      // location relative to the start of the SPI FLASH chip address

    usPageNumber = (unsigned short)(((CAST_POINTER_ARITHMETIC)ptrSector)/SPI_FLASH_PAGE_LENGTH); // the page the address is in
    if (usPageOffset != 0) {
        *usPageOffset = (unsigned short)((CAST_POINTER_ARITHMETIC)ptrSector - (usPageNumber * SPI_FLASH_PAGE_LENGTH)); // offset in the page
    }
    return usPageNumber;
}
    #endif

    #define _SPI_FLASH_INTERFACE                                         // insert manufacturer dependent code
        #include "spi_flash_kinetis_atmel.h"
        #include "spi_flash_kinetis_stmicro.h"
        #include "spi_flash_kinetis_sst25.h"
    #undef _SPI_FLASH_INTERFACE

// Power up the SPI interface, configure the pins used and select the mode and speed
//
extern int fnConfigSPIFileSystem(void)
{
    POWER_UP_SPI_FLASH_INTERFACE();
    CONFIGURE_SPI_FLASH_INTERFACE();
    #define _CHECK_SPI_CHIPS                                             // insert manufacturer dependent code
        #include "spi_flash_kinetis_atmel.h"
        #include "spi_flash_kinetis_stmicro.h"
        #include "spi_flash_kinetis_sst25.h"
    #undef _CHECK_SPI_CHIPS
    if (NO_SPI_FLASH_AVAILABLE == ucSPI_FLASH_Type[0]) {                 // if no SPI Flash detected
        return 1;
    }
    return 0;
}
#endif

// Erase FLASH sector(s). The pointer can be anywhere in the sector to be erased.
// If the length signifies multiple sectors, each one necessary is erased.
//
extern int fnEraseFlashSector(unsigned char *ptrSector, MAX_FILE_LENGTH Length)
{
    #ifdef PROTECTED_FILE
    int iUnprotected = 0;                                                // device presently protected
    #endif
    #ifdef MANAGED_FILES
    MAX_FILE_LENGTH OriginalLength = Length;
    #endif
    do {
        #if defined SPI_SW_UPLOAD
        if (ptrSector >= ((unsigned char *)(FLASH_START_ADDRESS + SIZE_OF_FLASH))) { // we are working from external SPI FLASH memory
            #ifdef SPI_FLASH_MULTIPLE_CHIPS                              // delete in SPI FLASH
            int iChipSelect;
            #endif
            #if !defined SPI_FLASH_ST
            MAX_FILE_LENGTH BlockLength;
            unsigned char  ucCommand;
            #endif
            #ifdef SPI_FLASH_SST25
            unsigned long ulSectorAlign;
            unsigned long ulLocation = (CAST_POINTER_ARITHMETIC)fnGetSPI_FLASH_address(ptrSector); // convert to virtual SPI Flash memory location
            ulSectorAlign = (ulLocation & ~(SPI_FLASH_SUB_SECTOR_LENGTH - 1));
            Length += (ulLocation - ulSectorAlign);
            ptrSector -= (ulLocation - ulSectorAlign);
            #else
            unsigned short usPageNumber, usPageOffset;
            usPageNumber = fnGetSPI_FLASH_location(ptrSector, &usPageOffset);
            Length += usPageOffset;                                      // length with respect to start of present page
            ptrSector -= usPageOffset;                                   // set pointer to start of present page
            #endif
            #if defined SPI_FLASH_ST
            fnSPI_command(WRITE_ENABLE, 0, 0, 0);                        // enable the write
                #ifdef SPI_DATA_FLASH
            fnSPI_command(SUB_SECTOR_ERASE, ((unsigned long)usPageNumber<<8), 0, 0); // delete appropriate sub-sector
                #else
            fnSPI_command(SECTOR_ERASE, ((unsigned long)usPageNumber<<8), 0, 0); // delete appropriate sector
                #endif
            if (Length <= SPI_FLASH_SECTOR_LENGTH) {
                break;                                                   // delete complete
            }
            ptrSector += SPI_FLASH_SECTOR_LENGTH;
            Length -= SPI_FLASH_SECTOR_LENGTH;
            #elif defined SPI_FLASH_SST25
            fnSPI_command(WRITE_ENABLE, 0, 0, 0);                        // command write enable to allow byte programming
                #ifndef SST25_A_VERSION
            if ((Length >= SPI_FLASH_SECTOR_LENGTH) && (ulSectorAlign == (ulLocation & (SPI_FLASH_SECTOR_LENGTH - 1)))) { // if a complete 64k sector can be deleted
                ucCommand = SECTOR_ERASE;                                // delete block of 64k
                BlockLength = SPI_FLASH_SECTOR_LENGTH;
            }
            else 
                #endif
            if ((Length >= SPI_FLASH_HALF_SECTOR_LENGTH) && ((ulSectorAlign & (SPI_FLASH_HALF_SECTOR_LENGTH - 1)) == 0)) {
                ucCommand = HALF_SECTOR_ERASE;                           // delete block of 32k
                BlockLength = SPI_FLASH_HALF_SECTOR_LENGTH;
            }
            else {
                ucCommand = SUB_SECTOR_ERASE;                            // delete smallest sector of 4k
                BlockLength = SPI_FLASH_SUB_SECTOR_LENGTH;
            }
            fnSPI_command(ucCommand, ulLocation, 0, 0);
            if (Length <= BlockLength) {
                break;                                                   // delete complete
            }
            ptrSector += BlockLength;
            Length -= BlockLength;
            #else
            if ((Length >= SPI_FLASH_BLOCK_LENGTH) && (usPageNumber%8 == 0)) { // if delete range corresponds to a block, use faster block delete
                BlockLength = SPI_FLASH_BLOCK_LENGTH;
                ucCommand = BLOCK_ERASE;
            }
            else {
                BlockLength = SPI_FLASH_PAGE_LENGTH;
                ucCommand = PAGE_ERASE;
            }
            fnSPI_command(ucCommand, usPageNumber, 0, 0);                // delete appropriate page/block
            if (Length <= BlockLength) {
                break;                                                   // delete complete
            }
            ptrSector += BlockLength;
            Length -= BlockLength;
            #endif
            continue;
        }
        #endif
                                                                         // delete in internal FLASH
        Length += (((CAST_POINTER_ARITHMETIC)ptrSector) - ((CAST_POINTER_ARITHMETIC)ptrSector & ~(FLASH_GRANULARITY - 1)));
        ptrSector = (unsigned char *)((CAST_POINTER_ARITHMETIC)ptrSector & ~(FLASH_GRANULARITY - 1)); // set to sector boundary
        #if defined NVRAM                                                // FlexBus
        if (ptrSector >= (unsigned char *)(FLASH_START_ADDRESS + SIZE_OF_FLASH)) {
            uMemset(fnGetFlashAdd(ptrSector), 0xff, Length);
            break;
        }
        #endif
        if ((fnFlashNow(FCMD_ERASE_FLASH_SECTOR, (unsigned long *)ptrSector, 0)) != 0) {
            return 1;                                                    // error
        }
        ptrSector += FLASH_GRANULARITY;
        if (Length <= FLASH_GRANULARITY) {
            break;
        }
        Length -= FLASH_GRANULARITY;
    } while (Length);

    #ifdef PROTECTED_FILE
    if (iUnprotected != 0) {                                             // protect device again on exit
    }
    #endif
    #ifdef MANAGED_FILES
    if (OriginalLength == 0) {                                           // if a single page erase was called, return the page size
	    return FLASH_GRANULARITY;
	}
    #endif
    return 0;
}

// The routine assumes that all writes are long word aligned and don't write to already programmed addresses when writing to program FLASH
//
extern int fnWriteBytesFlash(unsigned char *ucDestination, unsigned char *ucData, MAX_FILE_LENGTH Length)
{
    static unsigned char *ptrOpenBuffer = 0;
    #if defined FLASH_FILE_SYSTEM && defined FLASH_ROW_SIZE && FLASH_ROW_SIZE > 0
    unsigned char *ptrFlashBuffer;
    unsigned long ulBufferOffset;
    MAX_FILE_LENGTH BufferCopyLength;
    #endif

        #if defined SPI_SW_UPLOAD
    if (ucDestination >= ((unsigned char *)(FLASH_START_ADDRESS + SIZE_OF_FLASH))) { // we are working from external SPI FLASH memory 
            #ifdef SPI_FLASH_MULTIPLE_CHIPS
        int iChipSelect;
            #endif
            #if defined SPI_FLASH_SST25
        int iMultipleWrites = 0;
            #endif 
        unsigned short usDataLength;
            #if defined SPI_FLASH_SST25
        if (Length == 0) {
            return 0;                                                    // ignore if length is zero
        }
        ucDestination = fnGetSPI_FLASH_address(ucDestination);           // convert to virtual SPI Flash memory location
        if ((CAST_POINTER_ARITHMETIC)ucDestination & 0x1) {              // start at odd SPI address, requires an initial byte write
            fnSPI_command(WRITE_ENABLE, 0, 0, 0);                        // command write enable to allow byte programming
            fnSPI_command(BYTE_PROG, (unsigned long)ucDestination, ucData, 1); // program last byte 
            if (--Length == 0) {                                         // single byte write so complete
                return 0;
            }
            ucDestination++;
            ucData++;
        }
        fnSPI_command(WRITE_ENABLE, 0, 0, 0);                            // command write enable to allow programming
            #else
        unsigned short usPageNumber;
        unsigned short usPageOffset;
        usPageNumber = fnGetSPI_FLASH_location(ucDestination, &usPageOffset);
            #endif
        while (Length != 0) {
            usDataLength = (unsigned short)Length;
            #if defined SPI_FLASH_ST
            if (usDataLength > (SPI_FLASH_PAGE_LENGTH - usPageOffset)) {
                usDataLength = (SPI_FLASH_PAGE_LENGTH - usPageOffset);
            }
            fnSPI_command(WRITE_ENABLE, 0, 0, 0);                        // write enable
            fnSPI_command(PAGE_PROG, (usPageNumber<<8) | usPageOffset, ucData, usDataLength);// copy new content
            Length -= usDataLength;
            ucData += usDataLength;
            usPageNumber++;
            usPageOffset = 0;
            #elif defined SPI_FLASH_SST25
                #ifdef SST25_A_VERSION
                    #define AAI_LENGTH 1
                #else
                    #define AAI_LENGTH 2
                #endif
            if (usDataLength > 1) {
                if (iMultipleWrites == 0) {
                    fnSPI_command(AAI_WORD_PROG, (unsigned long)ucDestination, ucData, AAI_LENGTH); // program 2 bytes (1 byte for A type)
                    iMultipleWrites = 1;                                 // mark that we are in a AAI sequence
                }
                else {
                    fnSPI_command(AAI_WORD_PROG, (unsigned long)ucDestination, ucData, 0); // continue in AAI sequence - note that the data length is zero but this is used to differentiate - always pairs are written
                }
                ucDestination += AAI_LENGTH;
                Length -= AAI_LENGTH;
                ucData += AAI_LENGTH;
            }
            else {
                if (iMultipleWrites != 0) {
                    fnSPI_command(WRITE_DISABLE, 0, 0, 0);               // first close AAI sequence
                    fnSPI_command(WRITE_ENABLE, 0, 0, 0);                // command write enable to allow byte programming
                }
                fnSPI_command(BYTE_PROG, (unsigned long)ucDestination, ucData, 1); // program last byte 
                break;               
            }
            #else
            if ((usPageOffset != 0) || (Length < SPI_FLASH_PAGE_LENGTH)) { // are we writing a partial page?
                fnSPI_command(MAIN_TO_BUFFER_1, usPageNumber, 0, 0);     // copy main memory to buffer
            }
            if (usDataLength > (SPI_FLASH_PAGE_LENGTH - usPageOffset)) {
                usDataLength = (SPI_FLASH_PAGE_LENGTH - usPageOffset);
            }
            fnSPI_command(WRITE_BUFFER_1, usPageOffset, ucData, usDataLength);// copy new content
            fnSPI_command(PROG_FROM_BUFFER_1, usPageNumber, 0, 0);       // program to main memory
            Length -= usDataLength;
            ucData += usDataLength;
            usPageNumber++;
            usPageOffset = 0;
            #endif
        }
            #if defined SPI_FLASH_SST25
        fnSPI_command(WRITE_DISABLE, 0, 0, 0);                           // disable writes
            #endif
        return 0;
    }
        #endif
        #if defined NVRAM                                                // FlexBus
    if (ucDestination >= (unsigned char *)(FLASH_START_ADDRESS + SIZE_OF_FLASH)) {
        uMemcpy(fnGetFlashAdd(ucDestination), ucData, Length);
        return 0;
    }
        #endif
    #if defined FLASH_ROW_SIZE && FLASH_ROW_SIZE > 0
        #ifdef FLASH_FILE_SYSTEM
    if (ucData == 0) {                                                   // close an open buffer
        ulBufferOffset = ((CAST_POINTER_ARITHMETIC)ptrOpenBuffer & (FLASH_ROW_SIZE - 1));
        if (ulBufferOffset == 0) {
            return 0;                                                    // no open buffer so nothing to do
        }
        ulBufferOffset = FLASH_ROW_SIZE;                                 // cause the open buffer to be saved without copying any input data
        ptrOpenBuffer = (unsigned char *)((CAST_POINTER_ARITHMETIC)ptrOpenBuffer & ~(FLASH_ROW_SIZE-1));
    }
    else {
        ptrOpenBuffer = (unsigned char *)((CAST_POINTER_ARITHMETIC)ucDestination & ~(FLASH_ROW_SIZE-1));
        ulBufferOffset = ((CAST_POINTER_ARITHMETIC)ucDestination & (FLASH_ROW_SIZE-1));
    }
    do {
        BufferCopyLength = (FLASH_ROW_SIZE - ulBufferOffset);            // remaining buffer space to end of present backup buffer
        if (BufferCopyLength > Length) {
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
            if ((fnFlashNow(FCMD_PROGRAM, (unsigned long *)ptrOpenBuffer, &ulFlashRow[0])) != 0) {
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
#endif
}

extern int uFileErase(unsigned char *ptrFile, MAX_FILE_LENGTH FileLength)
{
    return fnEraseFlashSector(ptrFile, FileLength);
}

extern void fnGetPars(unsigned char *ParLocation, unsigned char *ptrValue, MAX_FILE_LENGTH Size)
{
#ifdef SPI_SW_UPLOAD
    if (ParLocation >= uFILE_SYSTEM_END) {                               // get from SPI FLASH
    #ifdef SPI_FLASH_SST25
        ParLocation = fnGetSPI_FLASH_address(ParLocation);
    #else
        unsigned short usPageNumber;
        unsigned short usPageOffset;
        usPageNumber = fnGetSPI_FLASH_location(ParLocation, &usPageOffset);
    #endif

            #ifdef SPI_FLASH_ST
        fnSPI_command(READ_DATA_BYTES, (unsigned long)((unsigned long)(usPageNumber << 8) | (usPageOffset)), ptrValue, Size);
            #elif defined SPI_FLASH_SST25
        fnSPI_command(READ_DATA_BYTES, (unsigned long)ParLocation, ptrValue, Size);
            #else
                #if SPI_FLASH_PAGE_LENGTH >= 1024
        fnSPI_command(CONTINUOUS_ARRAY_READ, (unsigned long)((unsigned long)(usPageNumber << 11) | (usPageOffset)), ptrValue, Size);
                #elif SPI_FLASH_PAGE_LENGTH >= 512
        fnSPI_command(CONTINUOUS_ARRAY_READ, (unsigned long)((unsigned long)(usPageNumber << 10) | (usPageOffset)), ptrValue, Size);
                #else
        fnSPI_command(CONTINUOUS_ARRAY_READ, (unsigned long)((unsigned long)(usPageNumber << 9) | (usPageOffset)), ptrValue, Size);
                #endif
            #endif
        return;
    }
#endif
    uMemcpy(ptrValue, fnGetFlashAdd(ParLocation), Size);                 // the Coldfire uses a file system in FLASH with no access restrictions so we can simply copy the data
}



// This routine is called to reset the card
//
extern void fnResetBoard(void)
{
    APPLICATION_INT_RESET_CTR_REG = (VECTKEY | SYSRESETREQ);             // request Cortex core reset, which will cause the software reset bit to be set in the mode controller for recognistion after restart
#ifndef _WINDOWS
    while (1) {}
#endif
}

#ifndef _COMPILE_KEIL                                                    // Keil doesn't support in-line assembler in Thumb mode so an assembler file is required
// Allow the jump to a foreign application as if it were a reset (load SP and PC)
//
extern void start_application(unsigned long app_link_location)
{
    #ifndef _WINDOWS
    asm(" ldr sp, [r0,#0]");
    asm(" ldr pc, [r0,#4]");
    #endif
}
#endif


#ifdef _COMPILE_KEIL

typedef struct stREGION_TABLE
{
    unsigned char *ptrConst;                                             // const variables belonging to block
    unsigned char *ptrDestination;                                       // destination in RAM
    unsigned long  ulLength;                                             // length of the block in SRAM
    unsigned long  ulUnknown;                                            // entry unknown
} REGION_TABLE;

// Calculate the end of used SRAM from the Keil linker information and optionally initialise variables
//
static unsigned char *_keil_ram_size(int iInit)
{
    extern REGION_TABLE Region$$Table$$Base;                            // table location supplied by linker
    extern REGION_TABLE Region$$Table$$Limit;                           // end of table list
    REGION_TABLE *ptrEntries = &Region$$Table$$Base;                    // first block
    unsigned char *ptrRam = ptrEntries->ptrDestination;                 // RAM address
    do {
        if (iInit != 0) {
            if (ptrEntries->ulUnknown == 0x60) {
                uMemset(ptrRam, 0, ptrEntries->ulLength);               // zero data
            }
            else {
                uMemcpy(ptrRam, ptrEntries->ptrConst, ptrEntries->ulLength); // intialise data
            }
        }
        ptrRam += ptrEntries->ulLength;                                 // add length
        ptrEntries++;                                                   // move to next block
    } while (ptrEntries != &Region$$Table$$Limit);
    return ptrRam;
}
#endif

#if defined (_GNU) || defined _CODE_WARRIOR
extern unsigned char __data_start__, __data_end__;
extern const unsigned char __data_load_start__;
extern unsigned char __text_start__, __text_end__;
extern const unsigned char __text_load_start__;
extern unsigned char __bss_start__, __bss_end__;

extern void __init_gnu_data(void)
{
    unsigned char *ptrData;
    unsigned long ulInitDataLength;
    #ifndef _RAM_DEBUG
    const unsigned char *ptrFlash = &__data_load_start__;
    ulInitDataLength = (&__data_end__ - &__data_start__);
    ptrData = &__data_start__;
    while (ulInitDataLength--) {                                         // initialise data
        *ptrData++ = *ptrFlash++;
    }

    ptrData = &__text_start__;
    ptrFlash = &__text_load_start__;
    if (ptrFlash != ptrData) {                                           // if a move is required
        ulInitDataLength = (&__text_end__ - &__text_start__);
        while (ulInitDataLength--) {                                     // initialise text
            *ptrData++ = *ptrFlash++;
        }
    }
    #endif
    ptrData = &__bss_start__;
    ulInitDataLength = (&__bss_end__ - &__bss_start__);
    while (ulInitDataLength--) {                                         // initialise bss
        *ptrData++ = 0;
    }
}
#endif

// Perform very low level initialisation - called by the start up code
//
#if defined _COMPILE_KEIL || defined _GNU || defined _CODE_WARRIOR
extern int
#else
extern void
#endif
#ifdef _WINDOWS
            _LowLevelInit(void)
#else
            main(void)
#endif
{
#ifndef _COMPILE_IAR
    UNLOCK_WDOG();                                                       // enable watchdog modification
    CONFIGURE_WATCHDOG();                                                // allow user configuration of internal watch dog timer
    #if defined USER_STARTUP_CODE                                        // {2} allow user defined start-up code immediately after the watchdog configuration and before clock configuration to be defined
    USER_STARTUP_CODE;
    #endif
#endif
#ifdef EXTERNAL_CLOCK                                                    // first move from state FEI to state FBE (presently running from about 25MHz internal clock)
    MCG_C2 = (MCG_C2_RANGE_8M_32M);                                      // don't use oscillator
    MCG_C1 = (MCG_C1_CLKS_EXTERN_CLK | MCG_C1_FRDIV_1024);               // switch to external input clock (the FLL input clock is set to as close to its input range as possible, although this is not necessary since the FLL will not be used)
#else
    #if CRYSTAL_FREQUENCY > 8000000
    MCG_C2 = (MCG_C2_RANGE_8M_32M | MCG_C2_HGO | MCG_C2_EREFS);          // select crystal oscillator
    #elif CRYSTAL_FREQUENCY >= 1000000
    MCG_C2 = (MCG_C2_RANGE_1M_8M | MCG_C2_HGO | MCG_C2_EREFS);           // select crystal oscillator
    #else                                                                // assumed to be 32kHz crystal
    MCG_C2 = (MCG_C2_RANGE_32K_40K | MCG_C2_HGO | MCG_C2_EREFS);         // select crystal oscillator
    #endif
    MCG_C1 = (MCG_C1_CLKS_EXTERN_CLK | MCG_C1_FRDIV_256);                // switch to external source (the FLL input clock is set to as close to its input range as possible, although this is not absolutely necessary since the FLL will not be used) this is accurate for 8MHz clock but hasn't been tested for other values
    while (!(MCG_S & MCG_S_OSCINIT)) {                                   // loop until the crystal source has been selected
    #ifdef _WINDOWS
        MCG_S |= MCG_S_OSCINIT;
    #endif
    }
#endif
    while (MCG_S & MCG_S_IREFST) {                                       // loop until the FLL source is no longer the internal reference clock
#ifdef _WINDOWS
        MCG_S &= ~MCG_S_IREFST;
#endif
    }
    while ((MCG_S & MCG_S_CLKST_MASK) != MCG_S_CLKST_EXTERN_CLK) {       // loop until the external reference clock source is valid
#ifdef _WINDOWS
        MCG_S &= ~MCG_S_CLKST_MASK;
        MCG_S |= MCG_S_CLKST_EXTERN_CLK;
#endif
    }    
    MCG_C5 = (CLOCK_DIV - 1);                                            // now move from state FBE to state PBE
    MCG_C6 = ((CLOCK_MUL - 24) | MCG_C6_PLLS);
    while ((MCG_S & MCG_S_PLLST) == 0) {                                 // loop until the PLLS clock source becomes valid
#ifdef _WINDOWS
        MCG_S |= MCG_S_PLLST;
#endif
    }
    while ((MCG_S & MCG_S_LOCK) == 0) {                                  // loop until PLL locks
#ifdef _WINDOWS
        MCG_S |= MCG_S_LOCK;
#endif
    }
    SIM_CLKDIV1 = (((SYSTEM_CLOCK_DIVIDE - 1) << 28) | ((BUS_CLOCK_DIVIDE - 1) << 24) | ((FLEX_CLOCK_DIVIDE - 1) << 20) | ((FLASH_CLOCK_DIVIDE - 1) << 16)); // prepare bus clock divides
    MCG_C1 = (MCG_C1_CLKS_PLL_FLL | MCG_C1_FRDIV_1024);                  // finally move from PBE to PEE mode - switch to PLL clock
    while ((MCG_S & MCG_S_CLKST_MASK) != MCG_S_CLKST_PLL) {              // loop until the PLL clock is selected
#ifdef _WINDOWS
        MCG_S &= ~MCG_S_CLKST_MASK;
        MCG_S |= MCG_S_CLKST_PLL;
#endif
    }   

#if defined (_GNU) || defined _CODE_WARRIOR
    __init_gnu_data();                                                   // initialise variables
#elif defined _COMPILE_KEIL
    _keil_ram_size(1);                                                   // initialise variables
#endif
#ifdef _WINDOWS                                                          // check that the size of the interrupt vectors has not grown beyond that what is expected (increase its space in the script file if necessary!!)
    if (VECTOR_SIZE > CHECK_VECTOR_SIZE) {
        _EXCEPTION("Check the vector table size setting!!");
    }
#endif
#ifndef _WINDOWS
    uTaskerBoot();                                                       // call the boot loader
#endif
#if defined _COMPILE_KEIL || defined _GNU || defined _CODE_WARRIOR
    return 0;
#endif
}


// The initial stack pointer and PC value - this is linked at address 0x00000000
//
#if defined _COMPILE_IAR
__root const RESET_VECTOR __vector_table @ ".intvec"                     // __root forces the function to be linked in IAR project
#elif defined _GNU
const RESET_VECTOR __attribute__((section(".vectors"))) reset_vect
#elif defined _CODE_WARRIOR
#pragma define_section vectortable ".RESET" ".RESET" ".RESET" far_abs R
static __declspec(vectortable) RESET_VECTOR __vect_table
#elif defined _COMPILE_KEIL
__attribute__((section("VECT"))) const RESET_VECTOR reset_vect
#else
    #if defined _COMPILE_GHS                                             // {4}
        #pragma ghs section rodata=".vectors"
    #endif
const RESET_VECTOR __vector_table
#endif
= {
    (void *)(RAM_START_ADDRESS + SIZE_OF_RAM - 4),                       // stack pointer to top of RAM
    (void (*)(void))START_CODE,                                          // start address
};


// Flash configuration - this is linked at address 0x00000400
//
#if defined _COMPILE_IAR
__root const KINETIS_FLASH_CONFIGURATION __flash_config @ ".f_config"    // __root forces the function to be linked in IAR project
#elif defined _GNU
const KINETIS_FLASH_CONFIGURATION __attribute__((section(".f_config"))) __flash_config
#elif defined _CODE_WARRIOR
#pragma define_section flash_cfg ".FCONFIG" ".FCONFIG" ".FCONFIG" far_abs R
static __declspec(flash_cfg) KINETIS_FLASH_CONFIGURATION __flash_config
#elif defined _COMPILE_KEIL
__attribute__((section("F_INIT"))) const KINETIS_FLASH_CONFIGURATION __flash_config
#else
    #if defined _COMPILE_GHS                                             // {4}
        #pragma ghs section rodata=".f_config"
    #endif
const KINETIS_FLASH_CONFIGURATION __flash_config
#endif
= {
    KINETIS_FLASH_CONFIGURATION_BACKDOOR_KEY,
    KINETIS_FLASH_CONFIGURATION_PROGRAM_PROTECTION,
    KINETIS_FLASH_CONFIGURATION_SECURITY,
    KINETIS_FLASH_CONFIGURATION_NONVOL_OPTION,
    KINETIS_FLASH_CONFIGURATION_EEPROM_PROT,
    KINETIS_FLASH_CONFIGURATION_DATAFLASH_PROT
};













#ifdef _WINDOWS
// The following routines are only for simulator compatibility

extern void *fnGetHeapStart(void) { return 0; }

// Convert a MAC address to a string
//
signed char *fnMACStr(unsigned char *ptrMAC, signed char *cStr)
{
    signed char cDummyMac[] = "--:--:--:--:--:--";
    int i = 0;
    while (cDummyMac[i] != 0) {
        *cStr++ = cDummyMac[i++];
    }
    *cStr = 0;
    return (cStr);
}

// Convert an IP address to a string
//
signed char *fnIPStr(unsigned char *ptrIP, signed char *cStr)
{
    signed char cDummyIP[] = "---.---.---.---";
    int i = 0;
    while (cDummyIP[i] != 0) {
        *cStr++ = cDummyIP[i++];
    }
    *cStr = 0;
    return (cStr);
}

extern CHAR *fnBufferDec(signed long slNumberToConvert, unsigned char ucStyle, CHAR *ptrBuf)
{
    return ptrBuf;
}

extern CHAR *uStrcpy(CHAR *ptrTo, const CHAR *ptrFrom)
{
    return ptrTo;
}


extern unsigned char *fnGetTxBufferAdd(int iBufNr) { return 0;}

extern int fnCheckEthernetMode(unsigned char *ucData, unsigned short usLen) {return 0;}
TASK_LIMIT uTaskerStart(const UTASKTABLEINIT *ptATaskTable, const signed char *a_node_descriptions, const PHYSICAL_Q_LIMIT nr_physicalQueues) {return 0;}
void fnInitialiseHeap(const HEAP_NEEDS *ctOurHeap, void *start_heap ){}

static void fnDummyTick(void)
{
}


// Basic hardware initialisation of specific hardware
//
INITHW void fnInitHW(void)                                               //perform hardware initialisation
{
#ifdef _WINDOWS
    #define PORTA_DEFAULT_INPUT        0xffffffff
    #define PORTB_DEFAULT_INPUT        0xffffffff

    unsigned long ulPortPullups[] = {
        PORTA_DEFAULT_INPUT,                                             // set the port states out of reset in the project file app_hw_sam7x.h
        PORTB_DEFAULT_INPUT
    };

    fnInitialiseDevice((void *)ulPortPullups);
    _LowLevelInit();
#endif
#ifdef _WINDOWS
    fnSimPorts();                                                        // ensure port states are recognised
#endif
}


extern void uTaskerBoot(void);
extern void uTaskerSchedule( void )
{
    static int iDone = 0;

    if (!iDone) {
        iDone = 1;
        uTaskerBoot();
    }
}
 #endif
#endif
