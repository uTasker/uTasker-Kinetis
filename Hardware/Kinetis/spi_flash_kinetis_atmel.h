/**********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher

    ---------------------------------------------------------------------
    File:      spi_flash_kinetis_atmel.h
    Project:   Single Chip Embedded Internet 
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2016
    *********************************************************************
    This file contains SPI FLASH specific code for all chips that are supported.
    It is declared as a header so that projects do not need to specify that it is not to be compiled.
    Its goal is to improve overall readability of the hardware interface.
    08.03.2014 Adapt for KL family compatibility (KL has neither FIFO nor automatic chip select control)
    22.11.2014 Add automatic power of 2s mode setting if the page size is defined for this {1}

*/ 

#if defined SPI_FLASH_ATMEL

#if defined _SPI_DEFINES
    #if defined SPI_FLASH_MULTIPLE_CHIPS
        static unsigned char fnCheckAT45dbxxx(int iChipSelect);
        static const STORAGE_AREA_ENTRY spi_flash_storage = {
            (void *)&default_flash,                                      // link to internal flash
            (unsigned char *)(FLASH_START_ADDRESS + SIZE_OF_FLASH),      // spi flash area starts after internal flash
            (unsigned char *)(FLASH_START_ADDRESS + SIZE_OF_FLASH + (SPI_DATA_FLASH_SIZE - 1)),
            _STORAGE_SPI_FLASH,                                          // type
            SPI_FLASH_DEVICE_COUNT                                       // multiple devices
        };
    #else
        static unsigned char fnCheckAT45dbxxx(void);
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
    // - if there are multiple devices, each will be recorded.
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
        #if defined SPI_FLASH_MULTIPLE_CHIPS                             // check for further devices
        int i = 0;
        while (++i < SPI_FLASH_DEVICE_COUNT) {
            ucSPI_FLASH_Type[i] = fnCheckAT45dbxxx(i);
        }
        #endif
        #if !defined BOOT_LOADER                                         // the boot loader doesn't use storage lists
        UserStorageListPtr = (STORAGE_AREA_ENTRY *)&spi_flash_storage;   // insert spi flash as storage medium
        #endif
    }
#endif



#if defined _SPI_FLASH_INTERFACE
// This is the main interface code to the ATMEL SPI FLASH device
//

/* =================================================================== */
/*                           AT45DB041 driver                          */
/* =================================================================== */

#define MANUFACTURER_ID_ATMEL      0x1F                                  // D-version manufacturer's ID
#define DEVICE_ID_1_DATA_FLASH_1M  0x22                                  // D-version FLASH size - 1MBit / 128kByte
#define DEVICE_ID_1_DATA_FLASH_2M  0x23                                  // D-version FLASH size - 2MBit / 256kByte
#define DEVICE_ID_1_DATA_FLASH_4M  0x24                                  // D-version FLASH size - 4MBit / 0.5MegByte
#define DEVICE_ID_1_DATA_FLASH_8M  0x25                                  // D-version FLASH size - 8MBit / 1MegByte
#define DEVICE_ID_1_DATA_FLASH_16M 0x26                                  // D-version FLASH size - 16MBit / 2MegByte
#define DEVICE_ID_1_DATA_FLASH_32M 0x27                                  // C/D-version FLASH size - 32MBit / 4MegByte
#define DEVICE_ID_1_DATA_FLASH_64M 0x28                                  // D-version FLASH size - 64MBit / 8MegByte


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
#define POWER_OF_TWOS            0x3d
// Chip erase is not supported due to bugs in some chips - sector erase is used instead
//


// SPI FLASH hardware interface
//
    #if defined SPI_FLASH_MULTIPLE_CHIPS 
static void fnSPI_command(unsigned char ucCommand, unsigned long ulPageNumberOffset, int iChipSelect, volatile unsigned char *ucData, MAX_FILE_LENGTH DataLength)
    #else
static void fnSPI_command(unsigned char ucCommand, unsigned long ulPageNumberOffset, volatile unsigned char *ucData, MAX_FILE_LENGTH DataLength)
    #endif
{
#if defined KINETIS_K80
    unsigned char dummy = 0;
#endif
    #if defined SPI_FLASH_MULTIPLE_CHIPS
    unsigned long ulChipSelectLine = ulChipSelect[iChipSelect];
    #else
    #define ulChipSelectLine       CS0_LINE
    #define iChipSelect            0
    #endif
    MAX_FILE_LENGTH dummyWrites  = 0;
    MAX_FILE_LENGTH dataWrites   = 0;
    MAX_FILE_LENGTH discardCount = 0;

    FLUSH_SPI_FIFO_AND_FLAGS();                                          // ensure that the FIFOs are empty and the status flags are reset before starting

    if (SPI_FLASH_Danger[iChipSelect] != 0) {                            // check whether the chip is ready to work, if not wait
        volatile unsigned char ucStatus;
        SPI_FLASH_Danger[iChipSelect] = 0;                               // device will no longer be busy after continuing
        do {
            fnSPI_command(READ_STATUS_REGISTER, 0, _EXTENDED_CS &ucStatus, 1); // read busy status register 
        } while ((ucStatus & STATUS_READY) == 0);                        // until no longer busy
    }

    SET_SPI_FLASH_MODE();

    #if defined KINETIS_KL || defined MANUAL_FLASH_CS_CONTROL
    ASSERT_CS_LINE(ulChipSelectLine);                                    // assert the chip select line
    #endif

    WRITE_SPI_CMD0(ucCommand);                                           // write command byte
    #if defined _WINDOWS
    fnSimAT45DBXXX(AT45DBXXX_WRITE, (unsigned char)SPI_TX_BYTE);         // simulate the SPI FLASH device
    #endif

    switch (ucCommand) {
    #if SPI_FLASH_PAGE_LENGTH == 256 || SPI_FLASH_PAGE_LENGTH == 512 || SPI_FLASH_PAGE_LENGTH == 1024 // {1}
    case POWER_OF_TWOS:
        dataWrites = DataLength;
        discardCount = 4;
        break;
    #endif
    case WRITE_BUFFER_1:                                                 // write data to the buffer
  //case WRITE_BUFFER_2:
    #if defined KINETIS_KL
        WAIT_SPI_RECEPTION_END();                                        // wait until the command has been sent
        (void)READ_SPI_FLASH_DATA();                                     // discard the received byte
    #endif
        WRITE_SPI_CMD0(0);
    #if defined _WINDOWS
        fnSimAT45DBXXX(AT45DBXXX_WRITE, (unsigned char)SPI_TX_BYTE);     // simulate the SPI FLASH device
    #endif
    #if defined KINETIS_KL
        WAIT_SPI_RECEPTION_END();                                        // wait until the command has been sent
        (void)READ_SPI_FLASH_DATA();                                     // discard the received byte
    #endif
        WRITE_SPI_CMD0((unsigned char)(ulPageNumberOffset >> 8));        // write command byte
    #if defined _WINDOWS
        fnSimAT45DBXXX(AT45DBXXX_WRITE, (unsigned char)SPI_TX_BYTE);     // simulate the SPI FLASH device
    #endif
    #if defined KINETIS_KL
        WAIT_SPI_RECEPTION_END();                                        // wait until the command has been sent
        (void)READ_SPI_FLASH_DATA();                                     // discard the received byte
    #endif
        WRITE_SPI_CMD0((unsigned char)(ulPageNumberOffset));
    #if defined _WINDOWS
        fnSimAT45DBXXX(AT45DBXXX_WRITE, (unsigned char)SPI_TX_BYTE);     // simulate the SPI FLASH device
    #endif
        dataWrites = DataLength;
    #if defined KINETIS_KL
        discardCount = (DataLength + 1);
    #else
        discardCount = (DataLength + 4);
    #endif
        break;

    case MAIN_TO_BUFFER_1:                                               // command chip to copy a page from main memory to the buffer (400us transfer time)
  //case MAIN_TO_BUFFER_2:
    case PROG_FROM_BUFFER_1:                                             // command programming buffer to FLASH (no erase)
  //case PROG_FROM_BUFFER_2:
    case ERASE_PROG_FROM_BUFFER_1:                                       // erase before programming
  //case ERASE_PROG_FROM_BUFFER_2:
    case PAGE_ERASE:
    case BLOCK_ERASE:
    #if defined KINETIS_KL
        WAIT_SPI_RECEPTION_END();                                        // wait until the command has been sent
        (void)READ_SPI_FLASH_DATA();                                     // discard the received byte
    #endif
    #if SPI_FLASH_PAGE_LENGTH >= 1024
        #if SPI_FLASH_PAGE_LENGTH == 1024                                // power of 2s mode
        WRITE_SPI_CMD0((unsigned char)(ulPageNumberOffset >> 6));
        #else
        WRITE_SPI_CMD0((unsigned char)(ulPageNumberOffset >> 5));
        #endif
    #elif SPI_FLASH_PAGE_LENGTH >= 512
        #if SPI_FLASH_PAGE_LENGTH == 512                                // power of 2s mode
        WRITE_SPI_CMD0((unsigned char)(ulPageNumberOffset >> 7));
        #else
        WRITE_SPI_CMD0((unsigned char)(ulPageNumberOffset >> 6));
        #endif
    #else
        #if SPI_FLASH_PAGE_LENGTH == 256                                 // power of 2s mode
        WRITE_SPI_CMD0((unsigned char)(ulPageNumberOffset >> 8));
        #else
        WRITE_SPI_CMD0((unsigned char)(ulPageNumberOffset >> 7));
        #endif
    #endif
    #if defined _WINDOWS
        fnSimAT45DBXXX(AT45DBXXX_WRITE, (unsigned char)SPI_TX_BYTE);     // simulate the SPI FLASH device
    #endif
    #if defined KINETIS_KL
        WAIT_SPI_RECEPTION_END();                                        // wait until the command has been sent
        (void)READ_SPI_FLASH_DATA();                                     // discard the received byte
    #endif
    #if SPI_FLASH_PAGE_LENGTH >= 1024
        #if SPI_FLASH_PAGE_LENGTH == 1024                                // power of 2s mode
        WRITE_SPI_CMD0((unsigned char)(ulPageNumberOffset << 2));
        #else
        WRITE_SPI_CMD0((unsigned char)(ulPageNumberOffset << 3));
        #endif
    #elif SPI_FLASH_PAGE_LENGTH >= 512
        #if SPI_FLASH_PAGE_LENGTH == 512                                // power of 2s mode
        WRITE_SPI_CMD0((unsigned char)(ulPageNumberOffset << 1));
        #else
        WRITE_SPI_CMD0((unsigned char)(ulPageNumberOffset << 2));
        #endif
    #else
        #if SPI_FLASH_PAGE_LENGTH == 256                                 // power of 2s mode
        WRITE_SPI_CMD0((unsigned char)ulPageNumberOffset);
        #else
        WRITE_SPI_CMD0((unsigned char)(ulPageNumberOffset << 1));
        #endif
    #endif
    #if defined _WINDOWS
        fnSimAT45DBXXX(AT45DBXXX_WRITE, (unsigned char)SPI_TX_BYTE);     // simulate the SPI FLASH device
    #endif
    #if defined KINETIS_KL
        WAIT_SPI_RECEPTION_END();                                        // wait until the command has been sent
        (void)READ_SPI_FLASH_DATA();                                     // discard the received byte
    #endif
        if (DataLength == 0) {
            WRITE_SPI_CMD0_LAST(0);                                      // last dummy byte
        }
        else {
            WRITE_SPI_CMD0(0);                                           // dummy byte
        }
    #if defined _WINDOWS
        fnSimAT45DBXXX(AT45DBXXX_WRITE, (unsigned char)SPI_TX_BYTE);     // simulate the SPI FLASH device
    #endif
        SPI_FLASH_Danger[iChipSelect] = 1;                               // mark that the device will be busy for some time
    #if defined KINETIS_KL
        discardCount = 1;
    #else
        discardCount = 4;
    #endif
        break;

    case CONTINUOUS_ARRAY_READ:                                          // this is a legacy command for compatibility between B and D-devices
    #if defined KINETIS_KL
        WAIT_SPI_RECEPTION_END();                                        // wait until the command has been sent
        (void)READ_SPI_FLASH_DATA();                                     // discard the received byte
    #endif
        WRITE_SPI_CMD0((unsigned char)(ulPageNumberOffset >> 16));
    #if defined _WINDOWS
        fnSimAT45DBXXX(AT45DBXXX_WRITE, (unsigned char)SPI_TX_BYTE);     // simulate the SPI FLASH device
    #endif
    #if defined KINETIS_KL
        WAIT_SPI_RECEPTION_END();                                        // wait until the command has been sent
        (void)READ_SPI_FLASH_DATA();                                     // discard the received byte
    #endif
        WRITE_SPI_CMD0((unsigned char)(ulPageNumberOffset >> 8));
    #if defined _WINDOWS
        fnSimAT45DBXXX(AT45DBXXX_WRITE, (unsigned char)SPI_TX_BYTE);     // simulate the SPI FLASH device
    #endif
    #if defined KINETIS_KL
        WAIT_SPI_RECEPTION_END();                                        // wait until the command has been sent
        (void)READ_SPI_FLASH_DATA();                                     // discard the received byte
    #endif
        WRITE_SPI_CMD0((unsigned char)(ulPageNumberOffset));
    #if defined _WINDOWS
        fnSimAT45DBXXX(AT45DBXXX_WRITE, (unsigned char)SPI_TX_BYTE);     // simulate the SPI FLASH device
    #endif
        dummyWrites = (DataLength + 4);                                  // 4 dummy bytes needed before the device returns data
    #if defined KINETIS_KL
        discardCount = 5;
    #else
        discardCount = 8;
    #endif
        break;

    case READ_MANUFACTURER_ID:                                           // this only works on D-device     
    #if defined KINETIS_KL
        dummyWrites = 5;
    #else
        WRITE_SPI_CMD0(0xff);                                            // ensure transmit FIFO has more than one byte in it
        dummyWrites = 4;
    #endif
        discardCount = 1;
        break;

    case READ_STATUS_REGISTER:                                           // read single byte from status register
    #if defined KINETIS_KL
        dummyWrites = 1;
    #else
        WRITE_SPI_CMD0_LAST(0xff);                                       // dummy write
        #if defined _WINDOWS
        fnSimAT45DBXXX(AT45DBXXX_WRITE, (unsigned char)SPI_TX_BYTE);     // simulate the SPI FLASH device
        #endif
    #endif
        discardCount = 1;
        break;

        // Note - no default here. This must never be called with non-supported command since it will hang.
    }

    while (DataLength != 0) {
        WAIT_SPI_RECEPTION_END();                                        // wait until at least one byte is in the receive FIFO
        if (discardCount != 0) {
            (void)READ_SPI_FLASH_DATA();                                 // discard
            discardCount--;
        }
        else {
    #if defined _WINDOWS
            SPI_RX_BYTE = fnSimAT45DBXXX(AT45DBXXX_READ, (unsigned char)SPI_TX_BYTE); // simulate the SPI FLASH device
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
            fnSimAT45DBXXX(AT45DBXXX_WRITE, (unsigned char)SPI_TX_BYTE); // simulate the SPI FLASH device
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
    #if defined KINETIS_KL || defined MANUAL_FLASH_CS_CONTROL
    NEGATE_CS_LINE(ulChipSelectLine);                                    // negate the chip select line
    #endif
    #if defined _WINDOWS
        #if !defined KINETIS_KL
    if (SPI_TX_BYTE & SPI_PUSHR_EOQ) {                                   // check that the CS has been negated
        SPI_TX_BYTE &= ~(ulChipSelectLine);
    }
        #endif
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
    if (iChipSelect == 0) {                                              // only on first device check
        fnDelayLoop(25000);                                              // 25ms start up delay to ensure SPI FLASH ready
    }
    #else
    fnDelayLoop(25000);                                                  // 25ms start up delay to ensure SPI FLASH ready
    #endif

    fnSPI_command(READ_MANUFACTURER_ID, 0, _EXTENDED_CS ucID, sizeof(ucID));
    if (ucID[0] == MANUFACTURER_ID_ATMEL) {                              // ATMEL D-part recognised
        int iRtn;
        switch (ucID[1]) {
        case DEVICE_ID_1_DATA_FLASH_1M:
            iRtn = AT45DB011D;
            break;
        case DEVICE_ID_1_DATA_FLASH_2M:
            iRtn = AT45DB021D;
            break;
        case DEVICE_ID_1_DATA_FLASH_4M:
            iRtn = AT45DB041D;
            break;
        case DEVICE_ID_1_DATA_FLASH_8M:
            iRtn = AT45DB081D;
            break;
        case DEVICE_ID_1_DATA_FLASH_16M:
            iRtn = AT45DB161D;
            break;
        case DEVICE_ID_1_DATA_FLASH_32M:
            iRtn = AT45DB321D;
            break;
        case DEVICE_ID_1_DATA_FLASH_64M:
            iRtn = AT45DB641D;
            break;
        default:                                                         // possibly a larger part but we don't accept it
            iRtn = NO_SPI_FLASH_AVAILABLE;
            break;
        }
        if (iRtn != NO_SPI_FLASH_AVAILABLE) {
    #if SPI_FLASH_PAGE_LENGTH == 256 || SPI_FLASH_PAGE_LENGTH == 512 || SPI_FLASH_PAGE_LENGTH == 1024 // {1} the user wants to operate the device in power of 2s mode so check this and set if necessary
            fnSPI_command(READ_STATUS_REGISTER, 0, _EXTENDED_CS ucID, 1);// read the status register
            if ((ucID[0] & 0x01) == 0) {                                 // not configured for power of 2s page size
                static const unsigned char ucUnlock[3] = {0x2a, 0x80, 0xa6};
                fnSPI_command(POWER_OF_TWOS, 0, _EXTENDED_CS (unsigned char *)ucUnlock, 3); // set the mode - this requires a power cycle to before use!!
            }
    #else
            fnSPI_command(READ_STATUS_REGISTER, 0, _EXTENDED_CS ucID, 1);// read the status register
    #endif
            return iRtn;
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
    default:
        break;
    }
    return NO_SPI_FLASH_AVAILABLE;                                       // device not found
}
#endif
#endif
