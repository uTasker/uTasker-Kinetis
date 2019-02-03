/**********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher

    ---------------------------------------------------------------------
    File:      spi_flash_STM32_stmicro.h
    Project:   Single Chip Embedded Internet 
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2017
    *********************************************************************
    This file contains SPI FLASH specific code for all chips that are supported.
    It is declared as a header so that projects do not need to specify that it is not to be compiled.
    Its goal is to improve overall readability of the hardware interface.

*/ 

#if defined SPI_FLASH_ST

#if defined _SPI_DEFINES
    #if defined _WINDOWS
        #define WAIT_TRANSFER_END()   while ((SSPSR_X & SPISR_TXE) == 0) { SSPSR_X |= SPISR_TXE;} \
                                      while (SSPSR_X & SPISR_BSY) {SSPSR_X &= ~SPISR_BSY;}
    #else
        #define WAIT_TRANSFER_END()   while ((SSPSR_X & SPISR_TXE) == 0) {} \
                                      while (SSPSR_X & SPISR_BSY) {}
    #endif

    #if defined SPI_FLASH_MULTIPLE_CHIPS
        static unsigned char fnCheckSTM25Pxxx(int iChipSelect);
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
        static unsigned char fnCheckSTM25Pxxx(void);
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
#ifdef _CHECK_SPI_CHIPS
    #ifdef SPI_FLASH_MULTIPLE_CHIPS 
    ucSPI_FLASH_Type[0] = fnCheckSTM25Pxxx(0);                           // flag whether the first SPI FLASH device is connected
    #else
    ucSPI_FLASH_Type[0] = fnCheckSTM25Pxxx();                            // flag whether the SPI FLASH device is connected
    #endif
    if (ucSPI_FLASH_Type[0] == NO_SPI_FLASH_AVAILABLE) {                 // we expect at least this part to be available                                                            
        POWER_DOWN_SPI_FLASH_INTERFACE();                                // power down SPI
    }
    else {
    #ifdef SPI_FLASH_MULTIPLE_CHIPS                                      // check for further devices
        int i = 0;
        while (++i < SPI_FLASH_DEVICE_COUNT) {
            ucSPI_FLASH_Type[i] = fnCheckSTM25Pxxx(i);
        }
    #endif
    #if !defined BOOT_LOADER                                         // the boot loader doesn't use storage lists
        UserStorageListPtr = (STORAGE_AREA_ENTRY *)&spi_flash_storage;   // insert spi flash as storage medium
    #endif
    }
#endif




#ifdef _SPI_FLASH_INTERFACE
// This is the main interface code to the STMicroelectronics SPI FLASH device

/* =================================================================== */
/*                           STM25PXX driver                           */
/* =================================================================== */

#define CHECK_SPI_FLASH_BUSY     0x00                                    // pseudo request to see whether device is ready
#define READ_STATUS_REGISTER     0x05
  #define STATUS_BUSY            0x01
  #define STATUS_WR_ENABLED      0x02
  #define STATUS_BL_PROTECT1     0x04
  #define STATUS_BL_PROTECT2     0x08
  #define STATUS_BL_PROTECT3     0x10
  #define STATUS_WRITE_PROTECT   0x80
#define WRITE_ENABLE             0x06
#define WRITE_DISABLE            0x04
#define WRITE_STATUS_REGISTER    0x01
#define READ_DATA_BYTES          0x03
#define FAST_READ                0x0B
#define PAGE_PROG                0x02
#define SECTOR_ERASE             0xd8
#define BULK_ERASE               0xc7
#define SUB_SECTOR_ERASE         0x20                                    // only available on the M25PExxx devices
#define READ_MANUFACTURER_ID     0x9f

#define MANUFACTURER_ID_ST       0x20                                    // ST manufacturer's ID

#define DEVICE_ID_1_DATA_ST_FLASH_512K 0x10                              // 512kBit / 64kByte
#define DEVICE_ID_1_DATA_ST_FLASH_1M   0x11                              // 1MBit / 128kByte
#define DEVICE_ID_1_DATA_ST_FLASH_2M   0x12                              // 2MBit / 256kByte
#define DEVICE_ID_1_DATA_ST_FLASH_4M   0x13                              // 4MBit / 0.5MegByte
#define DEVICE_ID_1_DATA_ST_FLASH_8M   0x14                              // 8MBit / 1MegByte
#define DEVICE_ID_1_DATA_ST_FLASH_16M  0x15                              // 16MBit / 2MegByte
#define DEVICE_ID_1_DATA_ST_FLASH_32M  0x16                              // FLASH size - 32MBit / 4MegByte
#define DEVICE_ID_1_DATA_ST_FLASH_64M  0x17                              // FLASH size - 64MBit / 8 MegByte
#define DEVICE_ID_1_DATA_ST_FLASH_128M 0x18                              // FLASH size - 128MBit / 16 MegByte



// SPI FLASH hardware interface
//
    #ifdef SPI_FLASH_MULTIPLE_CHIPS
static void fnSPI_command(unsigned char ucCommand, unsigned long ulPageNumberOffset, int iChipSelect, volatile unsigned char *ucData, MAX_FILE_LENGTH DataLength)
    #else
static void fnSPI_command(unsigned char ucCommand, unsigned long ulPageNumberOffset, volatile unsigned char *ucData, MAX_FILE_LENGTH DataLength)
    #endif
{
    #define CMD_WRITE 0x01
    #ifdef SPI_FLASH_MULTIPLE_CHIPS
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
    #ifdef MANAGED_FILES
            if (ucCommand == CHECK_SPI_FLASH_BUSY) {                     // pseudo request to see whether device is ready
                if (!(ucStatus & STATUS_BUSY)) {
                    return;                                              // the device is no longer busy
                }
                else if (--(*ucData) == 0) {
                    SPI_FLASH_Danger[iChipSelect] = 1;                   // put the busy bit back
                    return;                                              // the maximum number of status requests has expired so quit
                }
            }
    #endif
        } while ((ucStatus & STATUS_BUSY) != 0);                         // until no longer busy
    }
    #ifdef MANAGED_FILES
    else if (ucCommand == CHECK_SPI_FLASH_BUSY) {                        // pseudo command used to check device's status
        return;                                                          // the device is not busy so return immediately
    }
    #endif

    __ASSERT_CS(ulChipSelectLine);                                       // assert chip select low before starting

    ulDummy = SSPDR_X;                                                   // reset receive data flag with dummy read - the rx data is not interesting here
    SSPDR_X = ucCommand;                                                 // send command

    #ifdef _WINDOWS
    fnSimSTM25Pxxx(STM25PXXX_WRITE, (unsigned char)SSPDR_X);             // simulate the SPI FLASH device
    #endif
    WAIT_TRANSFER_END();                                                 // wait until tx byte has been sent and rx byte has been completely received
    ulDummy = SSPDR_X;                                                   // reset receive data flag with dummy read - the rx data is not interesting here

    switch (ucCommand) {
    case WRITE_ENABLE:                                                   // enable subsequent writes
        ucTxCount = sizeof(ucCommandBuffer);                             // no more data
        DataLength = 0;
        break;

    case SUB_SECTOR_ERASE:                                               // erase one sub-sector
    case SECTOR_ERASE:                                                   // erase one sector
    case PAGE_PROG:                                                      // program data to a page
		SPI_FLASH_Danger[iChipSelect] = 1;                               // mark that the chip will be busy for some time
    case READ_DATA_BYTES:
        ucCommandBuffer[0] = (unsigned char)(ulPageNumberOffset >> 16);
        ucCommandBuffer[1] = (unsigned char)(ulPageNumberOffset >> 8);
        ucCommandBuffer[2] = (unsigned char)ulPageNumberOffset;
        break;

    case READ_MANUFACTURER_ID:                                           // this only works on D-device
    case READ_STATUS_REGISTER:                                           // read single byte from status register
        while (DataLength--) {
            SSPDR_X = 0xff;                                              // empty transmission byte
            WAIT_TRANSFER_END();                                         // wait until dummy tx byte has been sent and rx byte has been completely received
    #ifdef _WINDOWS
            SSPDR_X = fnSimSTM25Pxxx(STM25PXXX_READ, 0);                 // simulate the SPI FLASH device
    #endif
            *ucData++ = (unsigned char)SSPDR_X;                          // read received byte and clear rx interrupt
        }
        __NEGATE_CS(ulChipSelectLine);                                   // negate chip select when complete
    #ifdef _WINDOWS
        fnSimSTM25Pxxx(STM25PXXX_CHECK_SS, 0);                           // simulate the SPI FLASH device
    #endif
        return;
    }

    while (ucTxCount < sizeof(ucCommandBuffer)) {                        // complete the command sequence
        SSPDR_X = ucCommandBuffer[ucTxCount++];                          // send data
    #ifdef _WINDOWS
        fnSimSTM25Pxxx(STM25PXXX_WRITE, (unsigned char)SSPDR_X);         // simulate the SPI FLASH device
    #endif
        WAIT_TRANSFER_END();                                             // wait until tx byte has been sent
    }

    if (READ_DATA_BYTES == ucCommand) {
        ulDummy = SSPDR_X;                                               // reset receive data flag with dummy read - the rx data is not interesting here
        while (DataLength-- != 0) {                                      // while data bytes to be read
            SSPDR_X = 0xff;
    #ifdef _WINDOWS
            SSPDR_X = fnSimSTM25Pxxx(STM25PXXX_READ, 0);                 // simulate the SPI FLASH device
    #endif
            WAIT_TRANSFER_END();                                         // wait until tx byte has been sent and rx byte has been completely received
            *ucData++ = (unsigned char)SSPDR_X;
        }
    }
    else {
        while (DataLength-- != 0) {                                      // while data bytes to be written
            SSPDR_X = *ucData++;                                         // send data
    #ifdef _WINDOWS
            fnSimSTM25Pxxx(STM25PXXX_WRITE, (unsigned char)SSPDR_X);     // simulate the SPI FLASH device
    #endif
            WAIT_TRANSFER_END();                                         // wait until tx byte has been sent
        }
        WAIT_TRANSFER_END();                                             // wait until last byte has been completely received before negating the CS line
    }

    __NEGATE_CS(ulChipSelectLine);                                       // negate chip select when complete
    #ifdef _WINDOWS
    fnSimSTM25Pxxx(STM25PXXX_CHECK_SS, 0);                               // simulate the SPI FLASH device
    #endif
}


// Check whether a known SPI FLASH device can be detected - called only once on start up
//
		#ifdef SPI_FLASH_MULTIPLE_CHIPS
static unsigned char fnCheckSTM25Pxxx(int iChipSelect)
		#else
static unsigned char fnCheckSTM25Pxxx(void)
		#endif
{
    volatile unsigned char ucID[3];

		#ifdef SPI_FLASH_MULTIPLE_CHIPS
    if (iChipSelect == 0) {
        fnDelayLoop(15000);                                              // 15ms start up delay to ensure SPI FLASH ready after power up
    }
		#else
    fnDelayLoop(15000);                                                  // 15ms start up delay to ensure SPI FLASH ready after power up
		#endif

    fnSPI_command(READ_MANUFACTURER_ID, 0, _EXTENDED_CS ucID, sizeof(ucID));
    if (ucID[0] == MANUFACTURER_ID_ST) {
        switch (ucID[2]) {
        case (DEVICE_ID_1_DATA_ST_FLASH_512K):                           // 512kBit / 64kByte
            return STM25P05;
        case (DEVICE_ID_1_DATA_ST_FLASH_1M):                             // 1MBit / 128kByte
            return STM25P10;
        case (DEVICE_ID_1_DATA_ST_FLASH_2M):                             // 2MBit / 256kByte
            return STM25P20;
        case (DEVICE_ID_1_DATA_ST_FLASH_4M):                             // 4MBit / 0.5MegByte
            return STM25P40;
        case (DEVICE_ID_1_DATA_ST_FLASH_8M):                             // 8MBit / 1MegByte
            return STM25P80;
        case (DEVICE_ID_1_DATA_ST_FLASH_16M):                            // 16MBit / 2MegByte
            return STM25P16;
        case (DEVICE_ID_1_DATA_ST_FLASH_32M):                            // 32MBit / 4MegByte
            return STM25P32;
        case (DEVICE_ID_1_DATA_ST_FLASH_64M):                            // 64MBit / 8MegByte
            return STM25P64;
        case (DEVICE_ID_1_DATA_ST_FLASH_128M):                           // 128MBit / 16MegByte
            return STM25P128;
        }
    }
    return NO_SPI_FLASH_AVAILABLE;                                       // device not found
}

#endif

#endif
