/**********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher

    ---------------------------------------------------------------------
    File:      spi_flash_STM32_sst25.h
    Project:   Single Chip Embedded Internet
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2017
    *********************************************************************
    This file contains SPI FLASH specific code for all chips that are supported.
    It is declared as a header so that projects do not need to specify that it is not to be compiled.
    Its goal is to improve overall readability of the hardware interface.

*/

#if defined SPI_FLASH_SST25

#if defined _SPI_DEFINES
    #if defined _WINDOWS
        #define WAIT_TRANSFER_END()   while (!(SSPSR_X & SPISR_TXE)) { SSPSR_X |= SPISR_TXE;} \
                                      while (SSPSR_X & SPISR_BSY) {SSPSR_X &= ~SPISR_BSY;}
    #else
        #define WAIT_TRANSFER_END()   while (!(SSPSR_X & SPISR_TXE)) {} \
                                      while (SSPSR_X & SPISR_BSY) {}
    #endif
    #if defined SPI_FLASH_MULTIPLE_CHIPS
        static unsigned char fnCheckSST25xxx(int iChipSelect);
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
        static unsigned char fnCheckSST25xxx(void);
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
    ucSPI_FLASH_Type[0] = fnCheckSST25xxx(0);                            // flag whether the first SPI FLASH device is connected
    #else
    ucSPI_FLASH_Type[0] = fnCheckSST25xxx();                             // flag whether the SPI FLASH device is connected
    #endif
    if (ucSPI_FLASH_Type[0] < SST25VF010A) {                             // we expect at least this part to be available
        POWER_DOWN_SPI_FLASH_INTERFACE();                                // power down SPI
    }
    else {
    #if defined SPI_FLASH_MULTIPLE_CHIPS                                 // check for further devices
        int i = 0;
        while (++i < SPI_FLASH_DEVICE_COUNT) {
            ucSPI_FLASH_Type[i] = fnCheckSST25xxx(i);
        }
    #endif
    #if !defined BOOT_LOADER                                             // the boot loader doesn't use storage lists
        UserStorageListPtr = (STORAGE_AREA_ENTRY *)&spi_flash_storage;   // insert spi flash as storage medium
    #endif
    }
#endif




#if defined _SPI_FLASH_INTERFACE
// This is the main interface code to the SST SPI FLASH device

/* =================================================================== */
/*                             SST25 driver                            */
/* =================================================================== */

#define READ_STATUS_REGISTER     0x05
  #define STATUS_BUSY            0x01
  #define STATUS_WEL             0x02
  #define STATUS_BP0             0x04
  #define STATUS_BP1             0x08
  #define STATUS_BP2             0x10
  #define STATUS_BP3             0x20
  #define STATUS_AAI             0x40                                    // Auto Address Increment Programming Status
  #define STATUS_BPL             0x80
#define READ_DATA_BYTES          0x03
#define FAST_READ                0x0B
#define SUB_SECTOR_ERASE         0x20
#define HALF_SECTOR_ERASE        0x52
#define SECTOR_ERASE             0xd8
#define BULK_ERASE               0xc7
#define BYTE_PROG                0x02
#if defined SST25_A_VERSION
    #define AAI_WORD_PROG        0xaf
#else
    #define AAI_WORD_PROG        0xad
#endif
#define ENABLE_WRITE_STATUS_REG  0x50
#define WRITE_STATUS_REGISTER    0x01
#define WRITE_ENABLE             0x06
#define WRITE_DISABLE            0x04
#define READ_ID                  0x90
#define READ_JEDEC_ID            0x9f
#define READ_MANUFACTURER_ID     READ_JEDEC_ID                           // for compatibility
#define ENABLE_SO_TO_RY          0x70
#define DISABLE_SO_TO_RY         0x80



#define MANUFACTURER_ID_SST      0xbf                                    // SST manufacturer's ID
#define DEVICE_TYPE              0x25

#define DEVICE_ID_DATA_SST_FLASH_A_1M  0x49                              // 1MBit / 128MegByte - A type
#define DEVICE_ID_DATA_SST_FLASH_A_2M  0x43                              // 2MBit / 256MegByte - A type
#define DEVICE_ID_DATA_SST_FLASH_A_4M  0x44                              // 4MBit / 0.5MegByte - A type
#define DEVICE_ID_1_DATA_SST_FLASH_4M  0x8d                              // 4MBit / 0.5MegByte
#define DEVICE_ID_1_DATA_SST_FLASH_8M  0x8e                              // 8MBit / 1MegByte
#define DEVICE_ID_1_DATA_SST_FLASH_16M 0x41                              // 16MBit / 2MegByte
#define DEVICE_ID_1_DATA_SST_FLASH_32M 0x4a                              // FLASH size - 32MBit / 4MegByte



// SPI FLASH hardware interface
//
    #if defined SPI_FLASH_MULTIPLE_CHIPS
static void fnSPI_command(unsigned char ucCommand, unsigned long ulPageNumberOffset, int iChipSelect, volatile unsigned char *ucData, MAX_FILE_LENGTH DataLength)
    #else
static void fnSPI_command(unsigned char ucCommand, unsigned long ulPageNumberOffset, volatile unsigned char *ucData, MAX_FILE_LENGTH DataLength)
    #endif
{
    #define CMD_WRITE 0x01
    #if defined SPI_FLASH_MULTIPLE_CHIPS
    unsigned long ulChipSelectLine = ulChipSelect[iChipSelect];
    #define ulChipSelectLineSim 0xffffffff
    #else
    #define ulChipSelectLine    CS0_LINE
    #define ulChipSelectLineSim CS0_LINE
    #define iChipSelect 0
    #endif
    int iEraseCommand = 0;
    unsigned char ucTxCount = 0;
    volatile unsigned long ulDummy;
    unsigned char ucCommandBuffer[3];

    if (SPI_FLASH_Danger[iChipSelect] != 0) {                            // check whether the chip is ready to work, if not wait
        volatile unsigned char ucStatus;
        SPI_FLASH_Danger[iChipSelect] = 0;                               // device will no longer be busy after continuing
        do {
            fnSPI_command(READ_STATUS_REGISTER, 0, _EXTENDED_CS &ucStatus, 1); // read busy status register
        } while (ucStatus & STATUS_BUSY);                                // until no longer busy
    }

    SET_SPI_FLASH_MODE();

    __ASSERT_CS(ulChipSelectLine);                                       // assert chip select low before starting

    ulDummy = SSPDR_X;                                                   // reset receive data flag with dummy read - the rx data is not interesting here
    SSPDR_X = ucCommand;                                                 // send command

    #if defined _WINDOWS
    fnSimSST25(SST25_WRITE, (unsigned char)SSPDR_X);                     // simulate the SPI FLASH device
    #endif
    WAIT_TRANSFER_END();                                                 // wait until tx byte has been sent and rx byte has been completely received
    ulDummy = SSPDR_X;                                                   // reset receive data flag with dummy read - the rx data is not interesting here

    switch (ucCommand) {
    case SUB_SECTOR_ERASE:
    case HALF_SECTOR_ERASE:
    case SECTOR_ERASE:
        iEraseCommand = 1;
    case BYTE_PROG:
    case AAI_WORD_PROG:
        SPI_FLASH_Danger[iChipSelect] = 1;                               // a write/erase will be started so we need to poll the status before next command
    case READ_ID:
    case READ_DATA_BYTES:                                                // 25MHz read - first setting the address and then reading the defined amount of data bytes
        if ((DataLength != 0) || (iEraseCommand != 0)) {
            ucCommandBuffer[0] = (unsigned char)(ulPageNumberOffset >> 16);
            ucCommandBuffer[1] = (unsigned char)(ulPageNumberOffset >> 8);
            ucCommandBuffer[2] = (unsigned char)(ulPageNumberOffset);
        }
        else {
            ucTxCount = sizeof(ucCommandBuffer);                         // no address is required in this case since we are in an AAI sequence
    #if defined SST25_A_VERSION
            DataLength = 1;                                              // always a single byte for A types
    #else
            DataLength = 2;                                              // always a write pair performed
    #endif
        }
        break;

    case BULK_ERASE:
    case ENABLE_WRITE_STATUS_REG:
    case WRITE_DISABLE:
    case WRITE_ENABLE:
    case READ_MANUFACTURER_ID:
    case READ_STATUS_REGISTER:                                           // read single byte from status register
        while (DataLength--) {
            SSPDR_X = 0xff;                                              // empty transmission byte
            WAIT_TRANSFER_END();                                         // wait until dummy tx byte has been sent and rx byte has been completely received
    #if defined _WINDOWS
            SSPDR_X = fnSimSST25(SST25_READ, 0);                         // simulate the SPI FLASH device
    #endif
            *ucData++ = (unsigned char)SSPDR_X;                          // read received byte and clear rx interrupt
        }
        __NEGATE_CS(ulChipSelectLine);                                   // negate chip select when complete
    #if defined _WINDOWS
        fnSimSST25(SST25_CHECK_SS, 0);                                   // simulate the SPI FLASH device
    #endif
        REMOVE_SPI_FLASH_MODE();
        return;

    case WRITE_STATUS_REGISTER:
    default:
        ucTxCount = sizeof(ucCommandBuffer);                             // no command buffer to be sent
        break;
    }

    while (ucTxCount < sizeof(ucCommandBuffer)) {                        // complete the command sequence
        SSPDR_X = ucCommandBuffer[ucTxCount++];                          // send data
    #if defined _WINDOWS
        fnSimSST25(SST25_WRITE, (unsigned char)SSPDR_X);                 // simulate the SPI FLASH device
    #endif
        WAIT_TRANSFER_END();                                             // wait until tx byte has been sent
    }

    if ((READ_DATA_BYTES == ucCommand) || (READ_ID == ucCommand)) {
        ulDummy = SSPDR_X;                                               // reset receive data flag with dummy read - the rx data is not interesting here
        while (DataLength-- != 0) {                                      // while data bytes to be read
            SSPDR_X = 0xff;
    #if defined _WINDOWS
            SSPDR_X = fnSimSST25(SST25_READ, 0);                         // simulate the SPI FLASH device
    #endif
            WAIT_TRANSFER_END();                                         // wait until tx byte has been sent and rx byte has been completely received
            *ucData++ = (unsigned char)SSPDR_X;
        }
    }
    else {
        while (DataLength-- != 0) {                                      // while data bytes to be written
            SSPDR_X = *ucData++;                                         // send data
    #if defined _WINDOWS
            fnSimSST25(SST25_WRITE, (unsigned char)SSPDR_X);             // simulate the SPI FLASH device
    #endif
            WAIT_TRANSFER_END();                                         // wait until tx byte has been sent
        }
        WAIT_TRANSFER_END();                                             // wait until last byte has been completely received before negating the CS line
    }

    __NEGATE_CS(ulChipSelectLine);                                       // negate chip select when complete
    #if defined _WINDOWS
    fnSimSST25(SST25_CHECK_SS, 0);                                       // simulate the SPI FLASH device
    #endif
    REMOVE_SPI_FLASH_MODE();
}



// Check whether a known SPI FLASH device can be detected - called only once on start up
//
#if defined SPI_FLASH_MULTIPLE_CHIPS
    static unsigned char fnCheckSST25xxx(int iChipSelect)
#else
    static unsigned char fnCheckSST25xxx(void)
#endif
{
    volatile unsigned char ucID[3];
    unsigned char ucReturnType = NO_SPI_FLASH_AVAILABLE;
    fnSPI_command(READ_MANUFACTURER_ID, 0, _EXTENDED_CS ucID, sizeof(ucID));
    if ((ucID[0] == MANUFACTURER_ID_SST) && (ucID[1] == DEVICE_TYPE)) {  // SST memory part recognised
        switch (ucID[2]) {
        case DEVICE_ID_1_DATA_SST_FLASH_4M:
            ucReturnType = SST25VF040B;
            break;
        case DEVICE_ID_1_DATA_SST_FLASH_8M:
            ucReturnType = SST25VF080B;
            break;
        case DEVICE_ID_1_DATA_SST_FLASH_16M:
            ucReturnType = SST25VF016B;
            break;
        case DEVICE_ID_1_DATA_SST_FLASH_32M:
            ucReturnType = SST25VF032B;
            break;
        default:                                                         // possibly a larger part but we don't accept it
            return NO_SPI_FLASH_AVAILABLE;
        }
    }
    else {                                                               // try requesting the ID since it may be an A-version
        fnSPI_command(READ_ID, 0, _EXTENDED_CS ucID, 2);
        if (ucID[0] == MANUFACTURER_ID_SST) {
            switch (ucID[1]) {
            case DEVICE_ID_DATA_SST_FLASH_A_1M:
                ucReturnType = SST25VF010A;
                break;
            case DEVICE_ID_DATA_SST_FLASH_A_2M:
                ucReturnType = SST25LF020A;
                break;
            case DEVICE_ID_DATA_SST_FLASH_A_4M:
                ucReturnType = SST25LF040A;
                break;
            default:                                                     // possibly a larger part but we don't accept it
                return NO_SPI_FLASH_AVAILABLE;
            }
        }
        else {
            return NO_SPI_FLASH_AVAILABLE;
        }
    }
    ucID[0] = 0;
    fnSPI_command(ENABLE_WRITE_STATUS_REG, 0, _EXTENDED_CS 0, 0);        // sequence to write status register to remove protection - first enable status register write
    fnSPI_command(WRITE_STATUS_REGISTER, 0, _EXTENDED_CS ucID, 1);       // now write to status register
    return ucReturnType;
}
#endif

#endif
