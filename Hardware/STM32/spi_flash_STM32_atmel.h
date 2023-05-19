/**********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher

    ---------------------------------------------------------------------
    File:      spi_flash_STM32_atmel.h
    Project:   Single Chip Embedded Internet 
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2017
    *********************************************************************
    This file contains SPI FLASH specific code for all chips that are supported.
    It is declared as a header so that projects do not need to specify that it is not to be compiled.
    Its goal is to improve overall readability of the hardware interface.

*/ 

#if defined SPI_FLASH_ATMEL

#if defined _SPI_DEFINES
    #if defined _WINDOWS
        #define WAIT_TRANSFER_END()   while ((SSPSR_X & SPISR_TXE) == 0) { SSPSR_X |= SPISR_TXE;} \
                                      while (SSPSR_X & SPISR_BSY) {SSPSR_X &= ~SPISR_BSY;}
    #else
        #define WAIT_TRANSFER_END()   while ((SSPSR_X & SPISR_TXE) == 0) {} \
                                      while (SSPSR_X & SPISR_BSY) {}
    #endif

    #if defined SPI_FLASH_MULTIPLE_CHIPS
        static unsigned char fnCheckAT45dbxxx(int iChipSelect);
        #if !defined BOOT_LOADER                                         // the boot loader doesn't use storage lists
        static const STORAGE_AREA_ENTRY spi_flash_storage = {
            (void *)&default_flash,                                      // link to internal flash
            (unsigned char *)(FLASH_START_ADDRESS + SIZE_OF_FLASH),      // spi flash area starts after internal flash
            (unsigned char *)(FLASH_START_ADDRESS + SIZE_OF_FLASH + (SPI_DATA_FLASH_SIZE - 1)),
            _STORAGE_SPI_FLASH,                                          // type
            SPI_FLASH_DEVICE_COUNT                                       // multiple devices
        };
        #endif
    #else
        static unsigned char fnCheckAT45dbxxx(void);
        #if !defined BOOT_LOADER                                         // the boot loader doesn't use storage lists
        static const STORAGE_AREA_ENTRY spi_flash_storage = {
            (void *)&default_flash,                                      // link to internal flash
            (unsigned char *)(FLASH_START_ADDRESS + SIZE_OF_FLASH),      // spi flash area starts after internal flash
            (unsigned char *)(FLASH_START_ADDRESS + SIZE_OF_FLASH + (SPI_DATA_FLASH_SIZE - 1)),
            _STORAGE_SPI_FLASH,                                          // type
            0                                                            // not multiple devices
        };
        #endif
    #endif
#endif


    // This code is inserted to detect the presence of the SPI FLASH device(s). If the first device is not detected the SPI interface is disabled.
    // If there are multiple devices, each will be recorded.
    //
#if defined _CHECK_SPI_CHIPS
    #if defined SPI_FLASH_MULTIPLE_CHIPS
    ucSPI_FLASH_Type[0] = fnCheckAT45dbxxx(0);                           // flag whether the first SPI FLASH device is connected
    #else
    ucSPI_FLASH_Type[0] = fnCheckAT45dbxxx();                            // flag whether the SPI FLASH device is connected
    #endif
    #if SPI_FLASH_PAGES == 512
    if (ucSPI_FLASH_Type[0] < AT45DB011B)                                // we expect at least this part to be available
    #else
    if (ucSPI_FLASH_Type[0] < AT45DB021B)                                // we expect at least this part to be available
    #endif
    {                                                                    
        POWER_DOWN_SPI_FLASH_INTERFACE();                                // power down SPI
    }
    else {
    #if defined SPI_FLASH_MULTIPLE_CHIPS                                 // check for further devices
        int i = 0;
        while (++i < SPI_FLASH_DEVICE_COUNT) {
            ucSPI_FLASH_Type[i] = fnCheckAT45dbxxx(i);
        }
    #endif
    #if !defined BOOT_LOADER                                             // the boot loader doesn't use storage lists
        UserStorageListPtr = (STORAGE_AREA_ENTRY *)&spi_flash_storage;   // insert spi flash as storage medium
    #endif
    }
#endif



#if defined _SPI_FLASH_INTERFACE
    // This is the main interface code to the ATMEL SPI FLASH device

/* =================================================================== */
/*                           AT45DB041 driver                          */
/* =================================================================== */

#define MANUFACTURER_ID_ATMEL      0x1F                                  // D-version manufacturer's ID
#define DEVICE_ID_1_DATA_FLASH_1M  0x22                                  // D-version FLASH size - 1MBit / 128kByte {4}
#define DEVICE_ID_1_DATA_FLASH_2M  0x23                                  // D-version FLASH size - 2MBit / 256kByte {4}
#define DEVICE_ID_1_DATA_FLASH_4M  0x24                                  // D-version FLASH size - 4MBit / 0.5MegByte
#define DEVICE_ID_1_DATA_FLASH_8M  0x25                                  // D-version FLASH size - 8MBit / 1MegByte
#define DEVICE_ID_1_DATA_FLASH_16M 0x26                                  // D-version FLASH size - 16MBit / 2MegByte
#define DEVICE_ID_1_DATA_FLASH_32M 0x27                                  // C/D-version FLASH size - 32MBit / 4MegByte
#define DEVICE_ID_1_DATA_FLASH_64M 0x28                                  // D-version FLASH size - 64MBit / 8MegByte

#define CHECK_SPI_FLASH_BUSY     0x00                                    // pseudo request to see whether device is ready
#define READ_STATUS_REGISTER     0xd7
  #define STATUS_READY           0x80
  #define STATUS_MISMATCH        0x40
  #define STATUS_1MEG            0x0c                                    // expected in 1Meg bit device
  #define STATUS_2MEG            0x14                                    // expected in 2Meg bit device
  #define STATUS_4MEG            0x1c                                    // expected in 4Meg bit device
  #define STATUS_8MEG            0x24                                    // expected in 8Meg bit device
  #define STATUS_16MEG           0x2c                                    // expected in 16Meg bit device
  #define STATUS_32MEG           0x34                                    // expected in 32Meg bit device
  #define STATUS_64MEG           0x3c                                    // expected in 64Meg bit device
  #define STATUS_PROTECTED_SECT  0x02                                    // sector is protected (D-device)
  #define STATUS_PAGE_SIZE       0x01                                    // 256 byte pages size (rather than 264)
#define WRITE_BUFFER_1           0x84
#define WRITE_BUFFER_2           0x87
#define ERASE_PROG_FROM_BUFFER_1 0x83
#define ERASE_PROG_FROM_BUFFER_2 0x86
#define PROG_FROM_BUFFER_1       0x88
#define PROG_FROM_BUFFER_2       0x89
#define PAGE_ERASE               0x81
#define BLOCK_ERASE              0x50
#define PROG_THROUGH_BUFFER_1    0x82                                    // combines WRITE_BUFFER and ERASE_PROG_FROM_BUFFER
#define PROG_THROUGH_BUFFER_2    0x85
#define MAIN_TO_BUFFER_1         0x53                                    // copy page of main memory to buffer
#define MAIN_TO_BUFFER_2         0x55
#define COMPARE_WITH_BUFFER_1    0x60                                    // compare page of main memory with buffer
#define COMPARE_WITH_BUFFER_2    0x61
#define AUTO_PAGE_REWRITE_1      0x58                                    // combines MAIN_TO_BUFFER and ERASE_PROG_FROM_BUFFER and can be used to refresh pages (it is recommeded to refresh each page in a sector at least every 10'000 page erase/program operations of the sector)
#define AUTO_PAGE_REWRITE_2      0x59
#define CONTINUOUS_ARRAY_READ    0xe8


// Command supported only by D-device
    //
#define SECTOR_ERASE             0x7c
#define READ_ARRAY_LOW_FREQ      0x03
#define READ_MANUFACTURER_ID     0x9f
// Chip erase is not spported due to bugs in some chips - sector erase is used instead
//

// SPI FLASH hardware interface
//
    #if defined SPI_FLASH_MULTIPLE_CHIPS
static void fnSPI_command(unsigned char ucCommand, unsigned long ulPageNumberOffset, int iChipSelect, volatile unsigned char *ucData, MAX_FILE_LENGTH DataLength)
    #else
static void fnSPI_command(unsigned char ucCommand, unsigned long ulPageNumberOffset, volatile unsigned char *ucData, MAX_FILE_LENGTH DataLength)
    #endif
{
    #if defined COUNT_SPI_TX
    int iSPI_transmissions = (DataLength + 4);                           // counter used only in special cases to record the number of bytes sent using the command
    #endif
    #define CMD_WRITE 0x01
    #if defined SPI_FLASH_MULTIPLE_CHIPS
    unsigned long ulChipSelectLine = ulChipSelect[iChipSelect];
    #define ulChipSelectLineSim 0xffffffff
    #else
    #define ulChipSelectLine    CS0_LINE
    #define ulChipSelectLineSim CS0_LINE
    #define iChipSelect 0
    #endif
    unsigned char ucTxCount = 0;
    volatile unsigned long ulDummy;
    unsigned char ucCommandBuffer[3];

    if (SPI_FLASH_Danger[iChipSelect] != 0) {                            // check whether the chip is ready to work, if not wait
        volatile unsigned char ucStatus;
        SPI_FLASH_Danger[iChipSelect] = 0;                               // device will no longer be busy after continuing
        do {
            fnSPI_command(READ_STATUS_REGISTER, 0, _EXTENDED_CS &ucStatus, 1); // read busy status register
    #if defined MANAGED_FILES
            if (ucCommand == CHECK_SPI_FLASH_BUSY) {                     // pseudo request to see whether device is ready
                if (ucStatus & STATUS_READY) {
                    return;                                              // the device is no longer busy
                }
                else if (--(*ucData) == 0) {
                    SPI_FLASH_Danger[iChipSelect] = 1;                   // put the busy bit back
                    return;                                              // the maximum number of status requests has expired so quit
                }
            }
    #endif
        } while (!(ucStatus & STATUS_READY));                            // until no longer busy
    }
    #if defined MANAGED_FILES
    else if (ucCommand == CHECK_SPI_FLASH_BUSY) {                        // pseudo command used to check device's status
        return;                                                          // the device is not busy so return immediately
    }
    #endif

    SET_SPI_FLASH_MODE();

    __ASSERT_CS(ulChipSelectLine);                                       // assert chip select low before starting

    ulDummy = SSPDR_X;                                                   // reset receive data flag with dummy read - the rx data is not interesting here
    SSPDR_X = ucCommand;                                                 // send command

    #if defined _WINDOWS
    fnSimAT45DBXXX(AT45DBXXX_WRITE, (unsigned char)SSPDR_X);             // simulate the SPI FLASH device
    #endif
    WAIT_TRANSFER_END();                                                 // wait until tx byte has been sent and rx byte has been completely received
    ulDummy = SSPDR_X;                                                   // reset receive data flag with dummy read - the rx data is not interesting here

    switch (ucCommand) {
    case WRITE_BUFFER_1:                                                 // write data to the buffer
  //case WRITE_BUFFER_2:
        ucCommandBuffer[0] = 0;
        ucCommandBuffer[1] = (unsigned char)(ulPageNumberOffset >> 8);
        ucCommandBuffer[2] = (unsigned char)ulPageNumberOffset;
        break;

    case MAIN_TO_BUFFER_1:                                               // command chip to copy a page from main memory to the buffer (400us transfer time)
  //case MAIN_TO_BUFFER_2:
    case PROG_FROM_BUFFER_1:                                             // command programming buffer to FLASH (no erase)
  //case PROG_FROM_BUFFER_2:
    case ERASE_PROG_FROM_BUFFER_1:                                       // erase before programming
  //case ERASE_PROG_FROM_BUFFER_2:
    case PAGE_ERASE:
    case BLOCK_ERASE:
    #if SPI_FLASH_PAGE_LENGTH >= 1024
        ucCommandBuffer[0] = (unsigned char)(ulPageNumberOffset >> 5);
        ucCommandBuffer[1] = (unsigned char)(ulPageNumberOffset << 3);
    #elif SPI_FLASH_PAGE_LENGTH >= 512
        ucCommandBuffer[0] = (unsigned char)(ulPageNumberOffset >> 6);
        ucCommandBuffer[1] = (unsigned char)(ulPageNumberOffset << 2);
    #else
        ucCommandBuffer[0] = (unsigned char)(ulPageNumberOffset >> 7);
        ucCommandBuffer[1] = (unsigned char)(ulPageNumberOffset << 1);
    #endif
        ucCommandBuffer[2] = 0;
        SPI_FLASH_Danger[iChipSelect] = 1;                               // mark that the device will be busy for some time
        break;

    case CONTINUOUS_ARRAY_READ:                                          // this is a legacy command for compatibility between B,C and D-devices
    #if defined COUNT_SPI_TX
        iSPI_transmissions += 4;
    #endif
        ucCommandBuffer[0] = (unsigned char)(ulPageNumberOffset >> 16);
        ucCommandBuffer[1] = (unsigned char)(ulPageNumberOffset >> 8);
        ucCommandBuffer[2] = (unsigned char)(ulPageNumberOffset);
        break;

    case READ_MANUFACTURER_ID:                                           // this only works on D-device
    case READ_STATUS_REGISTER:                                           // read single byte from status register
        while (DataLength--) {
            SSPDR_X = 0xff;                                              // empty transmission byte
            WAIT_TRANSFER_END();                                         // wait until dummy tx byte has been sent and rx byte has been completely received
    #if defined _WINDOWS
            SSPDR_X = fnSimAT45DBXXX(AT45DBXXX_READ, 0);                 // simulate the SPI FLASH device
    #endif
            *ucData++ = (unsigned char)SSPDR_X;                          // read received byte and clear rx interrupt
        }
        __NEGATE_CS(ulChipSelectLine);                                   // negate chip select when complete
    #if defined _WINDOWS
        fnSimAT45DBXXX(AT45DBXXX_CHECK_SS, 0);                           // simulate the SPI FLASH device
    #endif
    #if defined COUNT_SPI_TX
        iSPI_transmissions = 5;                                          // counter used only in special cases to record the number of bytes sent using the command
    #endif
        REMOVE_SPI_FLASH_MODE();
        return;

        // Note - no default here. This must never be called with non-supported command since it will otherwise hang.
        //
    }

    while (ucTxCount < sizeof(ucCommandBuffer)) {                        // complete the command sequence
        SSPDR_X = ucCommandBuffer[ucTxCount++];                          // send data
    #if defined _WINDOWS
        fnSimAT45DBXXX(AT45DBXXX_WRITE, (unsigned char)SSPDR_X);         // simulate the SPI FLASH device
    #endif
        WAIT_TRANSFER_END();                                             // wait until tx byte has been sent
    }

    if (CONTINUOUS_ARRAY_READ == ucCommand) {
        ucTxCount = 4;
        while (ucTxCount--) {                                            // 4 dummy bytes required before device returns data
            SSPDR_X = 0xff;                                              // send data
            WAIT_TRANSFER_END();                                         // wait until tx byte has been sent
        }
        ulDummy = SSPDR_X;                                               // reset receive data flag with dummy read - the rx data is not interesting here
        while (DataLength-- != 0) {                                      // while data bytes to be read
            SSPDR_X = 0xff;
    #if defined _WINDOWS
            SSPDR_X = fnSimAT45DBXXX(AT45DBXXX_READ, 0);                 // simulate the SPI FLASH device
    #endif
            WAIT_TRANSFER_END();                                         // wait until tx byte has been sent and rx byte has been completely received
            *ucData++ = (unsigned char)SSPDR_X;
        }
    }
    else {
        while (DataLength-- != 0) {                                      // while data bytes to be written
            SSPDR_X = *ucData++;                                         // send data
    #if defined _WINDOWS
            fnSimAT45DBXXX(AT45DBXXX_WRITE, (unsigned char)SSPDR_X);     // simulate the SPI FLASH device
    #endif
            WAIT_TRANSFER_END();                                         // wait until tx byte has been sent
        }
        WAIT_TRANSFER_END();                                             // wait until last byte has been completely received before negating the CS line
    }

    __NEGATE_CS(ulChipSelectLine);                                       // negate chip select when complete
    #if defined _WINDOWS
    fnSimAT45DBXXX(AT45DBXXX_CHECK_SS, 0);                               // simulate the SPI FLASH device
    #endif
    REMOVE_SPI_FLASH_MODE();
}



// Check whether a known SPI FLASH device can be detected - called only once on start up
//
#if defined SPI_FLASH_MULTIPLE_CHIPS
    static unsigned char fnCheckAT45dbxxx(int iChipSelect)
#else
    static unsigned char fnCheckAT45dbxxx(void)
#endif
{
    volatile unsigned char ucID[4];

#if defined SPI_FLASH_MULTIPLE_CHIPS
    if (iChipSelect == 0) {
        fnDelayLoop(25000);                                              // 25ms start up delay to ensure SPI FLASH ready after power up
    }
#else
    fnDelayLoop(25000);                                                  // 25ms start up delay to ensure SPI FLASH ready after power up
#endif

    fnSPI_command(READ_MANUFACTURER_ID, 0, _EXTENDED_CS ucID, sizeof(ucID));
    if (ucID[0] == MANUFACTURER_ID_ATMEL) {                              // ATMEL D-part recognised
        switch (ucID[1]) {
        case DEVICE_ID_1_DATA_FLASH_1M:
            return AT45DB011D;
        case DEVICE_ID_1_DATA_FLASH_2M:
            return AT45DB021D;
        case DEVICE_ID_1_DATA_FLASH_4M:
            return AT45DB041D;
        case DEVICE_ID_1_DATA_FLASH_8M:
            return AT45DB081D;
        case DEVICE_ID_1_DATA_FLASH_16M:
            return AT45DB161D;
        case DEVICE_ID_1_DATA_FLASH_32M:
            return AT45DB321D;
        case DEVICE_ID_1_DATA_FLASH_64M:
            return AT45DB641D;
        default:                                                         // possibly a larger part but we don't accept it
            break;
        }
    }

    fnSPI_command(READ_STATUS_REGISTER, 0, _EXTENDED_CS ucID, 1);        // it is possibly a B-device so check the status register
    switch (ucID[0] & 0x3c) {                                            // check part size field
    case STATUS_1MEG:
        return AT45DB011B;
    case STATUS_2MEG:
        return AT45DB021B;
    case STATUS_4MEG:
        return AT45DB041B;
    case STATUS_8MEG:
        return AT45DB081B;
    case STATUS_16MEG:
        return AT45DB321C;
    default:
        break;
    }
    return NO_SPI_FLASH_AVAILABLE;                                       // device not found
}
#endif

#endif
