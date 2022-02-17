/**********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher

    ---------------------------------------------------------------------
    File:      spi_flash_w25q.h - Winbond
    Project:   Single Chip Embedded Internet 
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2016
    *********************************************************************
    This file contains SPI FLASH specific code for all chips that are supported.
    It is declared as a header so that projects do not need to specify that it is not to be compiled.
    Its goal is to improve overall readability of the hardware interface.

    15.01.2016 Added SPI_FLASH_W25Q128 and control of final SPI byte for Kinetis parts with queued SPI CS

    **********************************************************************/


#if defined SPI_FLASH_W25Q

#if defined _SPI_DEFINES
    #if defined SPI_FLASH_MULTIPLE_CHIPS
        static unsigned char fnCheckW25Qxx(int iChipSelect);
        static const STORAGE_AREA_ENTRY spi_flash_storage = {
            (void *)&default_flash,                                      // link to internal flash
            (unsigned char *)(FLASH_START_ADDRESS + SIZE_OF_FLASH),      // spi flash area starts after internal flash
            (unsigned char *)(FLASH_START_ADDRESS + SIZE_OF_FLASH + (SPI_DATA_FLASH_SIZE - 1)),
            _STORAGE_SPI_FLASH,                                          // type
            SPI_FLASH_DEVICE_COUNT                                       // multiple devices
        };
    #else
        static unsigned char fnCheckW25Qxx(void);
        static const STORAGE_AREA_ENTRY spi_flash_storage = {
            (void *)&default_flash,                                      // link to internal flash
            (unsigned char *)(FLASH_START_ADDRESS + SIZE_OF_FLASH),      // spi flash area starts after internal flash
            (unsigned char *)(FLASH_START_ADDRESS + SIZE_OF_FLASH + (SPI_DATA_FLASH_SIZE - 1)),
            _STORAGE_SPI_FLASH,                                          // type
            0                                                            // not multiple devices
        };
    #endif
#endif


    // This code is inserted to detect the presence of the SPI FLASH device(s). If the first device is not detected the SPI interface is disabled.
    // - if there are multiple devices, each will be recorded
    //
#if defined _CHECK_SPI_CHIPS
        #if defined SPI_FLASH_MULTIPLE_CHIPS
    ucSPI_FLASH_Type[0] = fnCheckW25Qxx(0);                              // flag whether the first SPI FLASH device is connected
        #else
    ucSPI_FLASH_Type[0] = fnCheckW25Qxx();                               // flag whether the SPI FLASH device is connected
        #endif
    if (ucSPI_FLASH_Type[0] < W25Q10) {                             // we expect at least this part to be available
        POWER_DOWN_SPI_FLASH_INTERFACE();                                // power down SPI 
    }
    else {
        #if defined SPI_FLASH_MULTIPLE_CHIPS                             // check for further devices
        int i = 0;
        while (++i < SPI_FLASH_DEVICE_COUNT) {
            ucSPI_FLASH_Type[i] = fnCheckW25Qxx(i);
        }
        #endif
        #if !defined BOOT_LOADER                                         // the boot loader doesn't use storage lists
        UserStorageListPtr = (STORAGE_AREA_ENTRY *)&spi_flash_storage;   // insert spi flash as storage medium
        #endif
    }
#endif



#if defined _SPI_FLASH_INTERFACE
// This is the main interface code to the Winbond SPI FLASH device
//
/* =================================================================== */
/*                             W25Qxx driver                           */
/* =================================================================== */

#define CHECK_SPI_FLASH_BUSY     0x00                                    // pseudo request to see whether device is ready

#define WRITE_ENABLE             0x06
#define WRITE_DISABLE            0x04
#define READ_STATUS_REGISTER_1   0x05
  #define STATUS_BUSY            0x01
  #define STATUS_WEL             0x02
  #define STATUS_BP0             0x04
  #define STATUS_BP1             0x08
  #define STATUS_BP2             0x10
  #define STATUS_TB              0x20
  #define STATUS_SEC             0x40
  #define STATUS_SRP0            0x80
#define READ_STATUS_REGISTER_2   0x35
  #define STATUS_SRP1            0x01
  #define STATUS_QE              0x02
  #define STATUS_SUS             0x80
#define WRITE_STATUS_REGISTER    0x01
#define PAGE_PROG                0x02
#define QUAD_PROGRAM_PAGE        0x32
#define SUB_SECTOR_ERASE         0x20                                    // sector erase - 4k (name for compatibility)
#define HALF_SECTOR_ERASE        0x52                                    // half block erase - 32k (name for compatibility)
#define SECTOR_ERASE             0xd8                                    // block erase - 64k (name for compatibility)
#define CHIP_ERASE               0x60
#define ERASE_SUSPEND            0x75
#define ERASE_RESUME             0x7a
#define _POWER_DOWN              0xb9
#define _RELEASE_POWER_DOWN      0xab
#define CONT_READ_RESET          0xff
#define MANUFACTURER_ID          0x90
#define MANUFACTURER_ID_DUAL_I_O 0x92
#define MANUFACTURER_ID_QUAD_I_O 0x94
#define READ_JEDEC_ID            0x9f
#define READ_UNIQUE_ID           0x4b
#define READ_DATA_BYTES          0x03
#define FAST_READ                0x0b
#define FAST_READ_DUAL_OUTPUT    0x3b
#define FAST_READ_DUAL_I_O       0xbb
#define FAST_READ_QUAD_OUTPUT    0x6b
#define FAST_READ_QUAD_I_O       0xeb
#define WORD_READ_QUAD_I_O       0xe7
#define OCTAL_WORD_READ_QUAD_I_O 0xe3

#define MANUFACTURER_ID_WB       0xef                                    // Winbond manufacturer's ID

// Unique ID's
//
#define DEVICE_ID_1_DATA_WB_FLASH_1M    0x??                             // 1MBit / 128kByte - W25X10
#define DEVICE_ID_1_DATA_WB_FLASH_2M    0x??                             // 2MBit / 256kByte - W25X20
#define DEVICE_ID_1_DATA_WB_FLASH_Q2M   0x??                             // 2MBit / 256kByte - W25Q20
#define DEVICE_ID_1_DATA_WB_FLASH_4M    0x??                             // 4MBit / 512kByte - W25X40
#define DEVICE_ID_1_DATA_WB_FLASH_Q4M   0x??                             // 4MBit / 512kByte - W25Q40
#define DEVICE_ID_1_DATA_WB_FLASH_Q8M   0x??                             // 8MBit / 1MegByte - W25Q80
#define DEVICE_ID_1_DATA_WB_FLASH_Q16M  0x15                             // 16MBit / 2MegByte - W25Q16
#define DEVICE_TYPE_Q16                 0x40
#define DEVICE_ID_1_DATA_WB_FLASH_Q32M  0x??                             // 32MBit / 4MegByte - W25Q32
#define DEVICE_ID_1_DATA_WB_FLASH_Q64M  0x??                             // 64MBit / 8MegByte - W25Q64
#define DEVICE_ID_1_DATA_WB_FLASH_Q128M 0x18                             // 128MBit / 16MegByte - W25Q128
#define DEVICE_TYPE_Q128                0x60
#define DEVICE_ID_1_DATA_WB_FLASH_Q256M 0x??                             // 256MBit / 32MegByte - W25Q256


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
    int iRead = 0;
    int iErase = 0;
    unsigned char ucTxCount = 0;
    unsigned char ucCommandBuffer[3];

    FLUSH_SPI_FIFO_AND_FLAGS();                                          // ensure that the FIFOs are empty and the status flags are reset before starting

    if (SPI_FLASH_Danger[iChipSelect] != 0) {                            // check whether the chip is ready to work, if not wait
        volatile unsigned char ucStatus;
        SPI_FLASH_Danger[iChipSelect] = 0;                               // device will no longer be busy after continuing
        do {
            fnSPI_command(READ_STATUS_REGISTER_1, 0, _EXTENDED_CS &ucStatus, 1); // read busy status register
    #if defined MANAGED_FILES
            if (ucCommand == CHECK_SPI_FLASH_BUSY) {                     // pseudo request to see whether device is ready
                if ((ucStatus & STATUS_BUSY) == 0) {
                    return;                                              // the device is no longer busy
                }
                else if (--(*ucData) == 0) {
                    SPI_FLASH_Danger[iChipSelect] = 1;                   // put the busy bit back
                    return;                                              // the maximum number of status requests has expired so quit
                }
            }
    #endif
        } while (ucStatus & STATUS_BUSY);                                // until no longer busy
    }
    #if defined MANAGED_FILES
    else if (ucCommand == CHECK_SPI_FLASH_BUSY) {                        // pseudo command used to check device's status
        return;                                                          // the device is not busy so return immediately
    }
    #endif

    SET_SPI_FLASH_MODE();

    #if defined KINETIS_KL || defined MANUAL_FLASH_CS_CONTROL
    ASSERT_CS_LINE(ulChipSelectLine);                                    // assert the chip select line
    #endif

    switch (ucCommand) {
    case HALF_SECTOR_ERASE:                                              // 32k half-block
    case SECTOR_ERASE:                                                   // 64k block erase
    case SUB_SECTOR_ERASE:                                               // 4k sector
        iErase = 1;
    case PAGE_PROG:
        SPI_FLASH_Danger[iChipSelect] = 1;                               // a write/erase will be started so we need to poll the status before next command
    case READ_DATA_BYTES:                                                // 25MHz read - first setting the address and then reading the defined amount of data bytes
        WRITE_SPI_CMD0(ucCommand);                                       // send command
    #if defined _WINDOWS
        fnSimW25Qxx(W25Q_WRITE, (unsigned char)SPI_TX_BYTE);             // simulate the SPI FLASH device
    #endif
        ucCommandBuffer[0] = (unsigned char)(ulPageNumberOffset >> 16);  // define the address to be read, written or erased
        ucCommandBuffer[1] = (unsigned char)(ulPageNumberOffset >> 8);
        ucCommandBuffer[2] = (unsigned char)(ulPageNumberOffset);
        while (ucTxCount < sizeof(ucCommandBuffer)) {                    // complete the command sequence
            WAIT_SPI_RECEPTION_END();                                    // wait until at least one byte is in the receive FIFO
            CLEAR_RECEPTION_FLAG();                                      // clear the receive flag
            if ((ucTxCount == 2) && (iErase != 0)) {                     // erase doesn't have further data after the address
                WRITE_SPI_CMD0_LAST(ucCommandBuffer[ucTxCount++]);       // send address with no further data to follow
            }
            else {
                WRITE_SPI_CMD0(ucCommandBuffer[ucTxCount++]);            // send address
            }
        #if defined _WINDOWS
            fnSimW25Qxx(W25Q_WRITE, (unsigned char)SPI_TX_BYTE);         // simulate the SPI FLASH device
        #endif
        }
        break;

    case WRITE_DISABLE:
    case WRITE_ENABLE:
        WRITE_SPI_CMD0_LAST(ucCommand);                                  // send command
    #if defined _WINDOWS
        fnSimW25Qxx(W25Q_WRITE, (unsigned char)SPI_TX_BYTE);             // simulate the SPI FLASH device
    #endif
    #if defined KINETIS_KL
        WAIT_SPI_RECEPTION_END();                                        // wait until the command has been sent
        (void)READ_SPI_FLASH_DATA();                                     // discard the received byte
    #endif
    #if defined KINETIS_KL || defined MANUAL_FLASH_CS_CONTROL
        NEGATE_CS_LINE(ulChipSelectLine);                                  negate the chip select line
    #endif
    #if defined _WINDOWS
        #if !defined KINETIS_KL
        if ((SPI_TX_BYTE & SPI_PUSHR_EOQ) != 0) {                        // check that the CS has been negated
            SPI_TX_BYTE &= ~(ulChipSelectLine);
        }
        #endif
        fnSimW25Qxx(W25Q_CHECK_SS, 0);                                   // simulate the SPI FLASH device
    #endif
        REMOVE_SPI_FLASH_MODE();
        return;

    case READ_JEDEC_ID:
    case READ_STATUS_REGISTER_1:                                         // read single byte from status register
        WRITE_SPI_CMD0(ucCommand);                                       // send command
        iRead = 1;
    #if defined _WINDOWS
        fnSimW25Qxx(W25Q_WRITE, (unsigned char)SPI_TX_BYTE);             // simulate the SPI FLASH device
    #endif
        ucTxCount = sizeof(ucCommandBuffer);                             // no additional address to be written
        break;

    case WRITE_STATUS_REGISTER:
        WRITE_SPI_CMD0(ucCommand);                                       // send command
    #if defined _WINDOWS
        fnSimW25Qxx(W25Q_WRITE, (unsigned char)SPI_TX_BYTE);             // simulate the SPI FLASH device
    #endif
    #if defined KINETIS_KL
        WAIT_SPI_RECEPTION_END();                                        // wait until the command has been sent
        (void)READ_SPI_FLASH_DATA();                                     // discard the received byte
    #endif
    default:
        break;
    }

    if ((iRead != 0) || (READ_DATA_BYTES == ucCommand)) {
        WAIT_SPI_RECEPTION_END();                                        // wait until tx byte has been sent and rx byte has been completely received
        (void)READ_SPI_FLASH_DATA();                                     // the rx data is not interesting here
        CLEAR_RECEPTION_FLAG();                                          // clear the receive flag
        while (DataLength-- != 0) {                                      // while data bytes to be read
            if (DataLength == 0) {                                       // final byte
                WRITE_SPI_CMD0_LAST(0xff);                               // send idle line (final byte)
            }
            else {
                WRITE_SPI_CMD0(0xff);                                    // send idle line
            }
    #if defined _WINDOWS
            SPI_RX_BYTE = fnSimW25Qxx(W25Q_READ, 0);                     // simulate the SPI FLASH device
    #endif
            WAIT_SPI_RECEPTION_END();                                    // wait until tx byte has been sent and rx byte has been completely received
            *ucData++ = READ_SPI_FLASH_DATA();
            CLEAR_RECEPTION_FLAG();                                      // clear the receive flag
        }
    }
    else {
        while (DataLength-- != 0) {                                      // while data bytes to be written
            if (DataLength == 0) {                                       // final byte
                WRITE_SPI_CMD0_LAST(*ucData++);                          // send data (final byte)
            }
            else {
                WRITE_SPI_CMD0(*ucData++);                               // send data
            }
    #if defined _WINDOWS
            fnSimW25Qxx(W25Q_WRITE, (unsigned char)SPI_TX_BYTE);         // simulate the SPI FLASH device
    #endif
            WAIT_SPI_RECEPTION_END();                                    // wait until tx byte has been sent
        }
        WAIT_SPI_RECEPTION_END();                                        // wait until last byte has been completely received before negating the CS line
    }

    #if defined KINETIS_KL || defined MANUAL_FLASH_CS_CONTROL
    NEGATE_CS_LINE(ulChipSelectLine);                                    // negate the chip select line
    #endif
    #if defined _WINDOWS
        #if !defined KINETIS_KL
    if ((SPI_TX_BYTE & SPI_PUSHR_EOQ) != 0) {                            // check that the CS has been negated
        SPI_TX_BYTE &= ~(ulChipSelectLine);
    }
        #endif
    fnSimW25Qxx(W25Q_CHECK_SS, 0);                                       // simulate the SPI FLASH device
    #endif
    REMOVE_SPI_FLASH_MODE();
}

// Check whether a known SPI FLASH device can be detected - called only once on start up
//
#if defined SPI_FLASH_MULTIPLE_CHIPS
static unsigned char fnCheckW25Qxx(int iChipSelect)
#else
static unsigned char fnCheckW25Qxx(void)
#endif
{
    volatile unsigned char ucID[3];
    unsigned char ucReturnType = NO_SPI_FLASH_AVAILABLE;
    fnDelayLoop(10000);                                                  // the SPI Flash requires maximum 10ms after power has been applied until it can be read
    fnSPI_command(READ_JEDEC_ID, 0, _EXTENDED_CS ucID, sizeof(ucID));
    if (ucID[0] == MANUFACTURER_ID_WB) {                                 // Winbond memory part recognised
        switch (ucID[2]) {
        case DEVICE_ID_1_DATA_WB_FLASH_Q16M:
            ucReturnType = W25Q16;
            break;
        case DEVICE_ID_1_DATA_WB_FLASH_Q128M:
            ucReturnType = W25Q128;
            break;
        default:                                                         // possibly a larger part but we don't accept it
            return NO_SPI_FLASH_AVAILABLE;
        }
    }
    return ucReturnType;
}
#endif

#endif
