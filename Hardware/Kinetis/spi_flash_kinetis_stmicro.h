/**********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher

    ---------------------------------------------------------------------
    File:      spi_flash_kinetis_stmicro.h
    Project:   Single Chip Embedded Internet 
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2016
    *********************************************************************
    14.08.2014 Adapt for KL family compatibility (KL has neither FIFO nor automatic chip select control)
    This file contains SPI FLASH specific code for all chips that are supported.
    It is declared as a header so that projects do not need to specify that it is not to be compiled.
    Its goal is to improve overall readability of the hardware interface.

*/

#if defined SPI_FLASH_ST

#if defined _SPI_DEFINES
    #if defined SPI_FLASH_MULTIPLE_CHIPS
        static unsigned char fnCheckSTM25Pxxx(int iChipSelect);
        static const STORAGE_AREA_ENTRY spi_flash_storage = {
            (void *)&default_flash,                                      // link to internal flash
            (unsigned char *)(FLASH_START_ADDRESS + SIZE_OF_FLASH),      // spi flash area starts after internal flash
            (unsigned char *)(FLASH_START_ADDRESS + SIZE_OF_FLASH + (SPI_DATA_FLASH_SIZE - 1)),
            _STORAGE_SPI_FLASH,                                          // type
            SPI_FLASH_DEVICE_COUNT                                       // multiple devices
        };
    #else
        static unsigned char fnCheckSTM25Pxxx(void);
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
    // If there are multiple devices, each will be recorded.
#if defined _CHECK_SPI_CHIPS
        #if defined SPI_FLASH_MULTIPLE_CHIPS 
    ucSPI_FLASH_Type[0] = fnCheckSTM25Pxxx(0);                           // flag whether the first SPI FLASH device is connected
        #else
    ucSPI_FLASH_Type[0] = fnCheckSTM25Pxxx();                            // flag whether the SPI FLASH device is connected
        #endif
    if (ucSPI_FLASH_Type[0] == NO_SPI_FLASH_AVAILABLE) {                 // we expect at least this part to be available  
        POWER_DOWN_SPI_FLASH_INTERFACE();                                // power down SPI         
    }
    else {
        #if defined SPI_FLASH_MULTIPLE_CHIPS                             // check for further devices
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



#if defined _SPI_FLASH_INTERFACE
// This is the main interface code to the STMicroelectronics SPI FLASH device

/* =================================================================== */
/*                           STM25PXX driver                           */
/* =================================================================== */

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
#define SUB_SECTOR_ERASE         0x20                                    // only available of the M25PExxx devices
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
#if defined SPI_FLASH_MULTIPLE_CHIPS
static void fnSPI_command(unsigned char ucCommand, unsigned long ulPageNumberOffset, int iChipSelect, volatile unsigned char *ucData, MAX_FILE_LENGTH DataLength)
#else
static void fnSPI_command(unsigned char ucCommand, unsigned long ulPageNumberOffset, volatile unsigned char *ucData, MAX_FILE_LENGTH DataLength)
#endif
{
    #if defined SPI_FLASH_MULTIPLE_CHIPS
    unsigned long ulChipSelectLine = ulChipSelect[iChipSelect];
    #else
    #define ulChipSelectLine    CS0_LINE
    #define iChipSelect 0
    #endif
    int iNoWriteAddress = 0;
    MAX_FILE_LENGTH dummyWrites  = 0;
    MAX_FILE_LENGTH dataWrites   = 0;
    MAX_FILE_LENGTH discardCount = 0;

    FLUSH_SPI_FIFO_AND_FLAGS();                                          // ensure that the FIFOs are empty and the status flags are reset before starting

    if (SPI_FLASH_Danger[iChipSelect] != 0) {                            // check whether the chip is ready to work, if not wait
        volatile unsigned char ucStatus;
        SPI_FLASH_Danger[iChipSelect] = 0;                               // device will no longer be busy after continuing
        do {
            fnSPI_command(READ_STATUS_REGISTER, 0, _EXTENDED_CS &ucStatus, 1); // read busy status register 
        } while ((ucStatus & STATUS_BUSY) != 0);                         // until no longer busy
    }

    SET_SPI_FLASH_MODE();

    #if defined KINETIS_KL
    ASSERT_CS_LINE(ulChipSelectLine);                                    // assert the chip select line
    #endif

    switch (ucCommand) {                                                 // decision on the command type
    case BULK_ERASE:
        SPI_FLASH_Danger[iChipSelect] = 1;                               // mark that the chip will be busy for some time
    case WRITE_ENABLE:                                                   // enable subsequent writes
        WRITE_SPI_CMD0_LAST(ucCommand);                                  // single byte write
#if defined _WINDOWS
        fnSimSTM25Pxxx(STM25PXXX_WRITE, (unsigned char)SPI_TX_BYTE);     // simulate the SPI FLASH device
#endif
        WAIT_SPI_RECEPTION_END();
#if defined KINETIS_KL
        NEGATE_CS_LINE(ulChipSelectLine);                                // negate the chip select line
#endif
#if defined _WINDOWS
    #if !defined KINETIS_KL
        if (SPI_TX_BYTE & SPI_PUSHR_EOQ) {                               // check that the CS has been negated
            SPI_TX_BYTE &= ~(ulChipSelectLine);
        }
    #endif
        fnSimSTM25Pxxx(STM25PXXX_CHECK_SS, 0);                           // simulate the SPI FLASH device
#endif
        REMOVE_SPI_FLASH_MODE();
        return;

    case PAGE_PROG:                                                      // program data to a page (programming started when CS line negated)
        dataWrites = DataLength;                                         // data bytes to be written
    case SUB_SECTOR_ERASE:                                               // erase one sub-sector
    case SECTOR_ERASE:                                                   // erase one sector
        discardCount = (DataLength + 4);                                 // discard four byte
        SPI_FLASH_Danger[iChipSelect] = 1;                               // mark that the device will be busy for some time
        break;

    case READ_DATA_BYTES:                                                // read data from the chip
        discardCount = 4;
        dummyWrites = DataLength;
        break;

    case READ_MANUFACTURER_ID:                                           // get then manufacturer and chip size information
        WRITE_SPI_CMD0(ucCommand);                                       // write command byte
#if defined _WINDOWS
        fnSimSTM25Pxxx(STM25PXXX_WRITE, (unsigned char)SPI_TX_BYTE);     // simulate the SPI FLASH device
#endif
#if defined KINETIS_KL
        WAIT_SPI_RECEPTION_END();                                        // wait until the command has been sent
        (void)READ_SPI_FLASH_DATA();                                     // discard the received byte
#endif
        WRITE_SPI_CMD0(0xff);                                            // write second dummy
#if defined KINETIS_KL
        WAIT_SPI_RECEPTION_END();                                        // wait until the command has been sent
        (void)READ_SPI_FLASH_DATA();                                     // discard the received byte
#endif
        WRITE_SPI_CMD0(0xff);                                            // write third dummy
        discardCount = 1;                                                // discard one byte
        iNoWriteAddress = 1;                                             // command already written and no address to be sent
        dummyWrites = 1;
        break;

    case READ_STATUS_REGISTER:                                           // read single byte from status register
        WRITE_SPI_CMD0(ucCommand);                                       // write command byte
#if defined _WINDOWS
        fnSimSTM25Pxxx(STM25PXXX_WRITE, (unsigned char)SPI_TX_BYTE);     // simulate the SPI FLASH device
#endif
#if defined KINETIS_KL
        WAIT_SPI_RECEPTION_END();                                        // wait until the command has been sent
        (void)READ_SPI_FLASH_DATA();                                     // discard the received byte
#endif
        WRITE_SPI_CMD0_LAST(0xff);                                       // write final dummy
        discardCount = 1;                                                // discard one byte
        iNoWriteAddress = 1;
        break;

        // Note - no default here. This must never be called with non-supported command since it will hang
        //
    }

    if (iNoWriteAddress == 0) {                                          // if a command and three address bytes are to be sent
        WRITE_SPI_CMD0(ucCommand);                                       // write command byte
#if defined _WINDOWS
        fnSimSTM25Pxxx(STM25PXXX_WRITE, (unsigned char)SPI_TX_BYTE);     // simulate the SPI FLASH device
#endif
#if defined KINETIS_KL
        WAIT_SPI_RECEPTION_END();                                        // wait until the command has been sent
        (void)READ_SPI_FLASH_DATA();                                     // discard the received byte
#endif
        WRITE_SPI_CMD0((unsigned char)(ulPageNumberOffset >> 16));
#if defined _WINDOWS
        fnSimSTM25Pxxx(STM25PXXX_WRITE, (unsigned char)SPI_TX_BYTE);     // simulate the SPI FLASH device
#endif
#if defined KINETIS_KL
        WAIT_SPI_RECEPTION_END();                                        // wait until the command has been sent
        (void)READ_SPI_FLASH_DATA();                                     // discard the received byte
#endif
        WRITE_SPI_CMD0((unsigned char)(ulPageNumberOffset >> 8));
#if defined _WINDOWS
        fnSimSTM25Pxxx(STM25PXXX_WRITE, (unsigned char)SPI_TX_BYTE);     // simulate the SPI FLASH device
#endif
#if defined KINETIS_KL
        WAIT_SPI_RECEPTION_END();                                        // wait until the command has been sent
        (void)READ_SPI_FLASH_DATA();                                     // discard the received byte
#endif
        if (DataLength != 0) {
            WRITE_SPI_CMD0((unsigned char)ulPageNumberOffset);
        }
        else {
            WRITE_SPI_CMD0_LAST((unsigned char)ulPageNumberOffset);      // if no data length this is the final byte
        }
#if defined _WINDOWS
        fnSimSTM25Pxxx(STM25PXXX_WRITE, (unsigned char)SPI_TX_BYTE);     // simulate the SPI FLASH device
#endif
    }

    while (DataLength != 0) {
        WAIT_SPI_RECEPTION_END();                                        // wait until at least one byte is in the receive FIFO
        if (discardCount != 0) {
            (void)READ_SPI_FLASH_DATA();                                 // discard one byte
            discardCount--;
        }
        else {
#if defined _WINDOWS
            SPI_RX_BYTE = fnSimSTM25Pxxx(STM25PXXX_READ, (unsigned char)SPI_TX_BYTE); // simulate the SPI FLASH device
#endif
            *ucData++ = READ_SPI_FLASH_DATA();                           // read the byte from the receive FIFO and save to the application buffer
            DataLength--;
        }
        CLEAR_RECEPTION_FLAG();                                          // clear the receive flag
        if (dummyWrites != 0) {                                          // dummy writes
            if (dummyWrites > 1) {
                WRITE_SPI_CMD0(0xff);                                    // write dummy
            }
            else {
                WRITE_SPI_CMD0_LAST(0xff);                               // write final dummy
            }
            dummyWrites--;
        }
        if (dataWrites != 0) {                                           // data writes
            if (dataWrites > 1) {
                WRITE_SPI_CMD0(*ucData++);                               // write data byte
            }
            else {
                WRITE_SPI_CMD0_LAST(*ucData++);                          // write final data byte
            }
#if defined _WINDOWS
            fnSimSTM25Pxxx(STM25PXXX_WRITE, (unsigned char)SPI_TX_BYTE); // simulate the SPI FLASH device
#endif
            dataWrites--;
            DataLength--;
        }
    }
    while (discardCount != 0) {
        WAIT_SPI_RECEPTION_END();                                        // wait until at least one byte is in the receive FIFO
        (void)READ_SPI_FLASH_DATA();                                     // discard
        CLEAR_RECEPTION_FLAG();                                          // clear the receive flag
        discardCount--;
    }
#if defined KINETIS_KL
    NEGATE_CS_LINE(ulChipSelectLine);                                    // negate the chip select line
#endif
#if defined _WINDOWS
    #if !defined KINETIS_KL
    if (SPI_TX_BYTE & SPI_PUSHR_EOQ) {                                   // check that the CS has been negated
        SPI_TX_BYTE &= ~(ulChipSelectLine);
    }
    #endif
    fnSimSTM25Pxxx(STM25PXXX_CHECK_SS, 0);                               // simulate the SPI FLASH device
#endif
    REMOVE_SPI_FLASH_MODE();
}



// Check whether a known SPI FLASH device can be detected - called only once on start up
//
		#if defined SPI_FLASH_MULTIPLE_CHIPS
static unsigned char fnCheckSTM25Pxxx(int iChipSelect)
		#else
static unsigned char fnCheckSTM25Pxxx(void)
		#endif
{
    volatile unsigned char ucID[3];

		#if defined SPI_FLASH_MULTIPLE_CHIPS
    if (iChipSelect == 0) {
        fnDelayLoop(15000);                                              // start up delay to ensure SPI FLASH ready
    }
		#else
    fnDelayLoop(15000);                                                  // start up delay to ensure SPI FLASH ready
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
