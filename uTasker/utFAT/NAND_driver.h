/**********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher

    ---------------------------------------------------------------------
    File:      NAND_driver.h
    Project:   uTasker project
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2016
    *********************************************************************
    Implementation originally for SAM7SE with K9F5608U0D-P in 8 bit mode
    24.02.2011 Correct management of last remap content location         {1}
    20.03.2013 Added MT29F1G08ABADAH4 / MT29F2G16ABAEAWP and Kinetis     {2}
    04.06.2013 Added NAND_MALLOC() default                               {3}

*/


#if defined NAND_FLASH_FAT

#ifndef NAND_MALLOC                                                      // {3}
    #define NAND_MALLOC(x) uMalloc((MAX_MALLOC)(x))
#endif

#if defined _KINETIS                                                     // {2} MT29F1G08ABADAH4 (8-bit) or MT29F2G16ABAEAWP (16-bit)
    #define NAND_CMD_READ_PARAMETER_PAGE    0xec
    #define NAND_CMD_READ_UNIQUE_ID         0xed
    #define NAND_CMD_GET_FEATURES           0xee
    #define NAND_CMD_SET_FEATURES           0xef
    #define NAND_CMD_READ_STATUS_ENCANCED   0x78
    #if defined MT29F1G08ABADAH4                                         // 1G 8-bit
        #define GOOD_BLOCK_PATTERN          0xff
        #define BAD_BLOCK_PATTERN           0x00
        #if defined _WINDOWS
            #define NAND_BLOCK_COUNT        64                           // when simulating we do so with reduced memeory
        #else
            #define NAND_BLOCK_COUNT        1024                         // single-plane
        #endif
    #else                                                                // 2G 16-bit
        #define NAND_16_BIT_WIDE
        #define GOOD_BLOCK_PATTERN          0xffff
        #define BAD_BLOCK_PATTERN           0x0000
        #if defined _WINDOWS
            #define NAND_BLOCK_COUNT        128                          // when simulating we do so with reduced memeory
        #else
            #define NAND_BLOCK_COUNT        2048                         // in two planes
        #endif
    #endif
    #define NAND_PAGE_SIZE                  2048                         // bytes
    #define NAND_SPARE_SIZE                 64                           // bytes
    #define NAND_PAGES_IN_BLOCK             64
    #define NAND_BLOCK_SIZE                 ((NAND_PAGE_SIZE + NAND_SPARE_SIZE) * NAND_PAGES_IN_BLOCK)
    #define BLOCK_VALIDITY_LOCATION         (NAND_PAGE_SIZE + 0)         // ONFI conform location for bad block mark (0x00 in 8 bit mode and 0x0000 in 16 bit mode)
#else                                                                    // K9F5608U0D-P (8-bit)
    #define GOOD_BLOCK_PATTERN              0xff
    #define BAD_BLOCK_PATTERN               0x53
    #define NAND_BLOCK_COUNT                2048
    #define NAND_PAGE_SIZE                  512
    #define NAND_SPARE_SIZE                 16
    #define NAND_PAGES_IN_BLOCK             32
    #define NAND_BLOCK_SIZE                 ((NAND_PAGE_SIZE + NAND_SPARE_SIZE) * NAND_PAGES_IN_BLOCK)
    #define BLOCK_VALIDITY_LOCATION         (NAND_PAGE_SIZE + 5)
#endif

#define NAND_FLASH_START_ADDRESS        0x40000000
#define NAND_READ_ADDRESS               (NAND_FLASH_START_ADDRESS)
#define NAND_WRITE_DATA_ADDRESS         (NAND_FLASH_START_ADDRESS)
#define NAND_WRITE_ADDRESS              (NAND_FLASH_START_ADDRESS | 0x00200000) // with active NANDALE on A21
#define NAND_COMMAND_ADDRESS            (NAND_FLASH_START_ADDRESS | 0x00400000) // with active NANDCLE on A22

#define NAND_CMD_READ1                  0x00                             // read from first half of array (0..255)
#define NAND_CMD_READ_SECOND_HALF       0x01                             // read from second half of array (256..511)
#define NAND_CMD_READ_SPARE             0x50                             // read from spare array (512..527)
#define NAND_CMD_READ2                  0x30
#define NAND_CMD_READID                 0x90
#define NAND_CMD_PAGEPROG1              0x80
#define NAND_CMD_PAGEPROG2              0x10
#define NAND_CMD_ERASE1                 0x60
#define NAND_CMD_ERASE2                 0xd0



#define NAND_CMD_RANDOM_DATA_READ_1     0x05
#define NAND_CMD_RANDOM_DATA_READ_2     0xe0
#define NAND_CMD_RANDOM_DATA_INPUT      0x85

#define NAND_CMD_READ_STATUS            0x70

#define NAND_CMD_RESET                  0xff

#define NAND_PROG_ERASE_ERROR           0x01
#define NAND_READY                      0x40
#define NAND_NOT_PROTECTED              0x80

#define REMAP_SWAP_BLOCKS               8                                // the number of blocks reserved after the block mapping area for secure address map swapping when the area needs cleaning (including redundancy)
#define REMAP_BLOCKS                    32                               // the number of blocks reserved at the start of the NAND flash for block mapping purposes - including redundancy
#define MAX_SINGLE_ERRORS_TOLERATED     5                                // should the remap area develop this quantity of single entry errors it will be refreshed
#define REMAP_AREA_BLOCKS               (REMAP_SWAP_BLOCKS + REMAP_BLOCKS)
#define END_OF_LIFE_PROCENT             90                               // use 90% of user area for FAT so that end of life is reached when 10% are bad
#define MAX_USER_BLOCKS                 (NAND_BLOCK_COUNT - REMAP_AREA_BLOCKS)
#define USER_AREA_BLOCKS                (((NAND_BLOCK_COUNT - REMAP_AREA_BLOCKS) * END_OF_LIFE_PROCENT)/100)

#define NAND_CHIP_SELECT                PB18
#define NAND_BUSY_PIN                   PB19

#define BAD_BLOCK_CAN_NOT_BE_MODIFIED   -1
#define DELETE_FAILED                   -2
#define WRITE_FAILED                    -3
#define ECC_READ_ERROR                  -4
#define NO_REMAP_TABLE_FOUND            -5
#define REMAP_TABLE_FAILED_DUE_TO_BAD_NAND -6
#define NO_GOOD_BLOCKS_IN_SWAP_AREA     -7
#define SWAP_AREA_NOT_VALID             -8

typedef struct stNAND_BLOCK_INFO {                                       // this struct much be packed (should be by construction)
    unsigned long  ulNumberOfDeletes[4];                                 // 4 copies for redundancy
    unsigned short usPhysicalBlockNumber[4];                             // 4 copies for redundancy
    unsigned short usUserBlockNumber[4];                                 // 4 copies for redundancy
} NAND_BLOCK_INFO;

typedef struct stNAND_SWAP_BLOCK_INFO {                                  // this struct much be packed (should be by construction)
    unsigned long  ulNumberOfDeletes;
    unsigned short ulPhysicalBlockNumber;
} NAND_SWAP_BLOCK_INFO;


// Map a user block to a physical block
//
#define fnMapBlock(usUserBlock) ptrRemap[(unsigned short)(usUserBlock)]

static unsigned char  *ptrBadBlocks = 0;                                 // list of bad blocks status of each physical block in the device [referenced by physical block 0..(NAND_BLOCK_COUNT - 1)]
static unsigned short *ptrRemap = 0;                                     // list of empty blocks referenced by the user's block number (0..((USER_AREA_BLOCKS) - 1))
static unsigned char  *ptrBlankBlocks = 0;                               // list of empty blocks referenced by the user's block number (0..((MAX_USER_BLOCKS) - 1))
static unsigned long  *ptrWearLevel = 0;                                 // list of empty blocks referenced by the user's block number (0..((MAX_USER_BLOCKS) - 1))
static unsigned char  *ptrRemapDeletes = 0;                              // remapping area blocks waiting to be deleted
static unsigned char  *ptrSpareDeletes = 0;                              // spare area blocks waiting to be deleted

static unsigned short usLastBlock = 0;
static unsigned char  ucLastPage = 0;
static unsigned short usLastOffset = 0;
static int            iSwapBufferValid = 0;



#ifdef _WINDOWS
// NAND Flash simulation
//
#if defined _KINETIS                                                     // {2}
    #if defined MT29F1G08ABADAH4
        static unsigned char  ucNAND_info[5] = {0x2c, 0xf1, 0x80, 0x95, 0x02}; // manufacturer ID and device ID and further information
    #else
        static unsigned char  ucNAND_info[5] = {0x2c, 0xca, 0x90, 0xd5, 0x06}; // manufacturer ID and device ID
    #endif
#else
    static unsigned char  ucNAND_info[2] = {0xec, 0x75};                 // manufacturer ID and device ID
#endif
static unsigned char  ucNAND_data[NAND_BLOCK_COUNT * NAND_PAGES_IN_BLOCK * (NAND_PAGE_SIZE + NAND_SPARE_SIZE)];

static unsigned char  ucNAND_status = 0;
static unsigned char  NAND_address = 0;
static unsigned char  ucColumnAddress = 0;
static unsigned int   uiArea = 0;
static unsigned short usSpare = 0;
static unsigned short usPageAddress = 0;
static unsigned char  ucLastMode = 0x01;
static unsigned char  ucPresentCommand = 0x00;
static int            iReadState = 0;

static void _NAND_command(unsigned char ucCommand, unsigned long ulOperation)
{
    switch (ucCommand) {
    case 0x00:                                                           // read mode (in first half)
    case 0x01:                                                           // read mode (in second half)
        iReadState = 0;
        usSpare = 0;
        break;
    case 0xff:                                                           // reset command
        ucLastMode = 0x00;
        ucPresentCommand = 0x00;
      //uMemset(ucNAND_data, 0xff, sizeof(ucNAND_data));                 // start with perfect blank FLASH
        break;
    case 0x80:                                                           // program
        ucLastMode = ucPresentCommand;
        iReadState = 0;
        break;
    case 0x90:                                                           // read ID
#if defined _KINETIS                                                     // {2} copy the result to the status register
        NFC_SR1 = ((ucNAND_info[NAND_address] << 24) | (ucNAND_info[NAND_address + 1] << 16) | (ucNAND_info[NAND_address + 2] << 8) | (ucNAND_info[NAND_address + 3]));
        NFC_SR2 = (ucNAND_info[NAND_address + 4] << 24);
#endif
        break;
    case 0x60:                                                           // block delete
        iReadState = 0;
        break;
    case 0x70:                                                           // read status
        ucNAND_status = 0;
        break;
    case 0xd0:                                                           // delete
        if ((ucPresentCommand == 0x60) && (iReadState == 2)) {
            uMemset(&ucNAND_data[(usPageAddress/NAND_PAGES_IN_BLOCK) * NAND_BLOCK_SIZE], 0xff, NAND_BLOCK_SIZE); // delete a block
        }
        break;
    case 0x50:                                                           // read spare area
        iReadState = 0;
        break;
    }
    ucPresentCommand = ucCommand;
}

static void _NAND_address(unsigned char ucAddress)
{
    switch (ucPresentCommand) {
    case 0x90:                                                           // info
        NAND_address = ucAddress;
        break;
    case 0x60:
        switch (iReadState) {
        case 0:
            usPageAddress = ucAddress;
            iReadState++;
            break;
        case 1:
            usPageAddress |= (ucAddress << 8);
            iReadState++;
            break;
        default:
            break;
        }
        break;
    case 0x80:                                                           // program
    case 0x01:
    case 0x50:
    case 0x00:                                                           // read
        switch (iReadState) {
        case 0:
            if (ucPresentCommand == 0x50) {
                ucColumnAddress = (ucAddress & 0x0f);
                usSpare = NAND_PAGE_SIZE;
            }
            else {
                if ((ucPresentCommand == 0x01) || ((ucPresentCommand == 0x80) && (ucLastMode == 0x01))) {
                    uiArea = 1;
                }
                else {
                    uiArea = 0;
                }
                ucColumnAddress = ucAddress;
            }
            iReadState++;
            break;
        case 1:
            usPageAddress = ucAddress;
            iReadState++;
            break;
        case 2:
            usPageAddress |= (ucAddress << 8);
            iReadState++;
            break;
        default:
            break;
        }
    }
}

static unsigned char _ReadNAND(void)
{
    unsigned char ucReturn = 0;
    switch (ucPresentCommand) {
    case 0x90:                                                           // reading info
        ucReturn = ucNAND_info[NAND_address++];
        if (NAND_address >= sizeof(ucNAND_info)) {
            NAND_address = 0;
        }
        break;
    case 0x70:                                                           // get status
        ucReturn = ucNAND_status;
        break;
    case 0x01:
    case 0x00:
        ucReturn = ucNAND_data[(usPageAddress * (NAND_PAGE_SIZE + NAND_SPARE_SIZE)) + (uiArea * (NAND_PAGE_SIZE/2)) + usSpare + ucColumnAddress++];
        if (ucColumnAddress == 0) {
            uiArea ^= 1;
            ucPresentCommand = 0x00;                                     // leave 0x01 mode on overflow
            if (uiArea == 0) {
                usSpare = NAND_PAGE_SIZE;                                // reading in spare area
            }
        }
        else if ((usSpare != 0) && (ucColumnAddress >= NAND_SPARE_SIZE)) { // overflow from end of spare area to start of next page
            ucColumnAddress = 0;
            uiArea = 0;
            usSpare = 0;
            usPageAddress++;
        }
        break;
    case 0x50:                                                           // reading spare area
        ucReturn = ucNAND_data[(usPageAddress * (NAND_PAGE_SIZE + NAND_SPARE_SIZE)) + NAND_PAGE_SIZE + ucColumnAddress++];
        if (ucColumnAddress >= NAND_SPARE_SIZE) {
            usPageAddress++;
            ucColumnAddress = 0;
        }
        break;
    }
    return ucReturn;
}

static void _WriteNAND(unsigned char ucData)
{
    ucNAND_data[(usPageAddress * (NAND_PAGE_SIZE + NAND_SPARE_SIZE)) + (uiArea * (NAND_PAGE_SIZE/2)) + usSpare + ucColumnAddress++] = ucData;
    switch (ucLastMode) {
    case 0x00:
    case 0x01:
        if (ucColumnAddress == 0) {
            uiArea ^= 1;
            ucLastMode = 0x00;                                           // leave 0x01 mode on overflow
            if (uiArea == 0) {
                usSpare = NAND_PAGE_SIZE;                                // reading in spare area
            }
        }
        else if ((usSpare != 0) && (ucColumnAddress >= NAND_SPARE_SIZE)) { // overflow from end of spare area to start of next page
            ucColumnAddress = 0;
            uiArea = 0;
            usSpare = 0;
            usPageAddress++;
        }
        break;
    case 0x50:
        if (ucColumnAddress >= NAND_SPARE_SIZE) {
            ucColumnAddress = 0;
            usPageAddress++;
        }
        break;
    }
}


#include "Fcntl.h"
#include "io.h"
#include <sys/stat.h>
#if _VC80_UPGRADE >= 0x0600
    #include <share.h>
#endif

#if defined _HW_SAM7X
    #define NAND_FILE    "Samung_NAND.bin"
#else
    #define NAND_FILE    "Micron_NAND.bin"
#endif

static int iFileNAND = -1;

// This is called on exit to allow the user code to save its own data to disk
//
extern void fnSaveUserData(void)
{
    #if _VC80_UPGRADE < 0x0600
	iFileNAND = _open(NAND_FILE, (_O_BINARY | _O_TRUNC  | _O_CREAT | _O_RDWR ), _S_IREAD | _S_IWRITE );
    #else
	_sopen_s(&iFileNAND, NAND_FILE, (_O_BINARY |  _O_TRUNC  | _O_CREAT | _O_RDWR), _SH_DENYNO, _S_IREAD | _S_IWRITE);
    #endif
	if (iFileNAND >= 0) {
        _write(iFileNAND, ucNAND_data, sizeof(ucNAND_data));             // save NAND content to disk
	    _close(iFileNAND);
	}
}

    #if defined _HW_SAM7X
        #define READ_NAND()           _ReadNAND()
        #define READ_NAND_ID_0()      _ReadNAND()
        #define READ_NAND_ID_1()      _ReadNAND()
        #define WRITE_NAND(d)         _WriteNAND(d)
        #define WRITE_NAND_ADDRESS(d) _NAND_address((unsigned char)(d))
        #define NAND_WAIT_READY()     while (!(PIO_PDSR_B & NAND_BUSY_PIN)) { PIO_PDSR_B |= NAND_BUSY_PIN; }
    #elif defined _KINETIS                                               // {2}
        #define READ_NAND()           1
        #define READ_NAND_ID_0()      (unsigned char)(NFC_SR1 >> 24)
        #define READ_NAND_ID_1()      (unsigned char)(NFC_SR1 >> 16)
        #define WRITE_NAND(d)         //
        #define WRITE_NAND_ADDRESS(d) //
        #define NAND_WAIT_READY()     //
    #endif
#else                                                                    // NAND Flash hardware interface
    #if defined _HW_SAM7X
        #define _NAND_command(d, dummy)      *(volatile unsigned char *)(NAND_COMMAND_ADDRESS) = (unsigned char)(d)
        #define NAND_WAIT_READY()            while (!(PIO_PDSR_B & NAND_BUSY_PIN)) {}
        #define WRITE_NAND_ADDRESS(d)        *(volatile unsigned char *)(NAND_WRITE_ADDRESS) = (unsigned char)(d)
        #define READ_NAND()                  *(volatile unsigned char *)(NAND_READ_ADDRESS)
        #define WRITE_NAND(d)                *(volatile unsigned char *)(NAND_WRITE_DATA_ADDRESS) = (unsigned char)d
    #elif defined _KINETIS                                               // {2}
        #define _NAND_command(d, operation)  NFC_ISR |= (NFC_ISR_WERRCLR | NFC_ISR_DONECLR | NFC_ISR_IDLECLR); \
                                             NFC_CMD2 = ((d << 24) | operation); \
                                             NFC_CMD2 |= NFC_CMD2_BUSY_START; \
                                             while (NFC_CMD2 & NFC_CMD2_BUSY_START) {};
        #define NAND_WAIT_READY()     //
        #define WRITE_NAND_ADDRESS(d) //
        #define READ_NAND()           1
        #define WRITE_NAND(d)         //
        #define READ_NAND_ID_0()      (unsigned char)(NFC_SR1 >> 24)
        #define READ_NAND_ID_1()      (unsigned char)(NFC_SR1 >> 16)
    #endif
#endif

#if defined _HW_SAM7X                                                    // chip select controlled via GPIO
    #define SELECT_NAND()         _CLEARBITS(B, NAND_CHIP_SELECT)
    #define DESELECT_NAND()       _SETBITS(B, NAND_CHIP_SELECT)
#elif defined _KINETIS                                                   // {2} the NAND controller in the Kinetis automatically controls these
    #define SELECT_NAND()
    #define DESELECT_NAND()
#endif

#define fnCommandNAND(command, operation)    SELECT_NAND(); \
                                             _NAND_command(command, operation); \
                                             NAND_WAIT_READY(); \
                                             DESELECT_NAND()


static int _fnWriteNANDdata(unsigned short usBlockNumber, unsigned char ucPageNumberInBlock, unsigned short usOffsetInPage, unsigned char *ptrBuffer, unsigned short usLength); // write to real physical block location


// Read status from NAND flash
// It is expected that this is called with the NAND device's chip select still asserted
//
#if defined _KINETIS                                                     // {2}
    #define fnReadNAND_status() (unsigned char)NFC_SR2
#else
static unsigned char fnReadNAND_status(void)
{
    _NAND_command(NAND_CMD_READ_STATUS, 0);                              // status command
    return READ_NAND();                                                  // return status byte value
}
#endif

// Mark a block as being bad, referenced by its physical block number in the device
//
static void fnMarkBadBlock(unsigned short usBadPhysicalBlock)
{
#if defined NAND_16_BIT_WIDE
    unsigned short BadBlockPattern = BAD_BLOCK_PATTERN;                  // indicate that we have marked the block as bad
#else
    unsigned char  BadBlockPattern= BAD_BLOCK_PATTERN;                   // indicate that we have marked the block as bad
#endif
    fnMemoryDebugMsg("New bad block -> ");
    fnMemoryDebugDec(usBadPhysicalBlock, WITH_CR_LF);
    _fnWriteNANDdata(usBadPhysicalBlock, 0, BLOCK_VALIDITY_LOCATION, (unsigned char *)&BadBlockPattern, sizeof(BadBlockPattern)); // mark the block as bad forever...
    BadBlockPattern = ~BadBlockPattern;
    _fnWriteNANDdata(usBadPhysicalBlock, 1, BLOCK_VALIDITY_LOCATION, (unsigned char *)&BadBlockPattern, sizeof(BadBlockPattern));
    ptrBadBlocks[usBadPhysicalBlock] = 1;                                // mark that this block is bad
}


// Erase a single NAND block
// An attempt to erase a bad block will be blocked to avoid the loss of the information but normally
// it is expected that there is never an attempt to delete a bad block
//
static int _fnEraseNAND_block(unsigned short usBlockToErase)
{
#if !defined _KINETIS
    unsigned short usPage;
#endif
    if (ptrBadBlocks[usBlockToErase] != 0) {
        fnMemoryDebugMsg("Attempted delete bad block\n\r");
        return BAD_BLOCK_CAN_NOT_BE_MODIFIED;
    }
#if defined _KINETIS                                                     // {2}
    NFC_ISR |= (NFC_ISR_WERRCLR | NFC_ISR_DONECLR | NFC_ISR_IDLECLR);    // clear flags before starting the read
    NFC_RAR = ((NFC_RAR_CS0 | NFC_RAR_RB0) | usBlockToErase);            // row address for block to be erased
    NFC_CAR = 0;                                                         // column address is always at first page
    NFC_CFG &= ~NFC_CFG_ECCMODE_60;                                      // ensure that ECC check is disabled
    NFC_CMD2 = ((NAND_CMD_ERASE1 << 24) | (NFC_CMD2_CODE_SEND_COMMAND_BYTE1_TO_FLASH | NFC_CMD2_CODE_SEND_ROW_ADD1_TO_FLASH | NFC_CMD2_CODE_SEND_ROW_ADD2_TO_FLASH | NFC_CMD2_CODE_SEND_ROW_ADD3_TO_FLASH | NFC_CMD2_CODE_SEND_COMMAND_BYTE2_TO_FLASH | NFC_CMD2_CODE_WAIT_FOR_FLASH_R_B_HANDSHAKE | NFC_CMD2_CODE_SEND_COMMAND_BYTE3_TO_FLASH | NFC_CMD2_CODE_READ_FLASH_STATUS));
    NFC_CMD1 = ((NAND_CMD_ERASE2 << 24) | (NAND_CMD_READ_STATUS << 16));
    NFC_CMD2 |= NFC_CMD2_BUSY_START;                                     // start erase
    while (NFC_CMD2 & NFC_CMD2_BUSY_START) {
    #if defined _WINDOWS
        NFC_CMD2 &= ~(NFC_CMD2_BUSY_START);                              // wait for the erase to complete
        uMemset((ucNAND_data + (usBlockToErase * (NAND_PAGE_SIZE + NAND_SPARE_SIZE))), 0, (NAND_PAGE_SIZE + NAND_SPARE_SIZE)); // delete a block
    #endif
    };     
#else
    usPage = (usBlockToErase * NAND_PAGES_IN_BLOCK);                     // the first page in the block to be deleted
    SELECT_NAND();
    _NAND_command(NAND_CMD_ERASE1, 0);                                   // prepare block erase
    WRITE_NAND_ADDRESS((unsigned char)usPage);                           // address the first page in the block to be deleted
    WRITE_NAND_ADDRESS((unsigned char)(usPage >> 8));
    _NAND_command(NAND_CMD_ERASE2, 0);                                   // start erase action
    NAND_WAIT_READY();                                                   // wait up to 200us for erase to complete
#endif
    if (fnReadNAND_status() & NAND_PROG_ERASE_ERROR) {                   // check whether there was an error when trying to erase the block
        fnMemoryDebugMsg("Block erase failed\n\r");
        DESELECT_NAND();
        fnMarkBadBlock(usBlockToErase);
        return DELETE_FAILED;
    }
    DESELECT_NAND();
    return UTFAT_SUCCESS;
}

// Read data from the specified block, page area to a buffer
// No check is made to see whether the read is from a bad block
// Optionally the ECC is checked as long as a complete page read is performed
//
static int _fnReadNANDdata(unsigned short usBlockNumber, unsigned char ucPageNumberInBlock, unsigned short usOffsetInPage, unsigned char *ptrBuffer, unsigned short usLength)
{
    int iECC = UTFAT_SUCCESS;
#if defined _KINETIS                                                     // {2}
    NFC_ISR |= (NFC_ISR_WERRCLR | NFC_ISR_DONECLR | NFC_ISR_IDLECLR);    // clear flags before starting the read
    NFC_RAR = ((NFC_RAR_CS0 | NFC_RAR_RB0) | usBlockNumber);             // row address for block
    NFC_CAR = ucPageNumberInBlock;                                       // column address to set page
    NFC_CFG |= NFC_CFG_ECCMODE_60;                                       // ensure that ECC check is enabled
    NFC_CMD2 = ((NAND_CMD_READ1 << 24) | (NFC_CMD2_CODE_SEND_COMMAND_BYTE1_TO_FLASH | NFC_CMD2_CODE_SEND_COL_ADD1_TO_FLASH | NFC_CMD2_CODE_SEND_COL_ADD2_TO_FLASH | NFC_CMD2_CODE_SEND_ROW_ADD1_TO_FLASH | NFC_CMD2_CODE_SEND_ROW_ADD2_TO_FLASH | NFC_CMD2_CODE_SEND_ROW_ADD3_TO_FLASH | NFC_CMD2_CODE_SEND_COMMAND_BYTE2_TO_FLASH | NFC_CMD2_CODE_WAIT_FOR_FLASH_R_B_HANDSHAKE | NFC_CMD2_CODE_READ_DATA_FROM_FLASH | NFC_CMD2_CODE_SEND_COMMAND_BYTE3_TO_FLASH | NFC_CMD2_CODE_READ_FLASH_STATUS));
    NFC_CMD1 = ((NAND_CMD_READ2 << 24) | (NAND_CMD_READ_STATUS << 16));
    NFC_CMD2 |= NFC_CMD2_BUSY_START;
    while (NFC_CMD2 & NFC_CMD2_BUSY_START) {
    #if defined _WINDOWS
        NFC_CMD2 &= ~(NFC_CMD2_BUSY_START);                              // wait for the read to complete
        uMemcpy((unsigned char *)NFC_RAM_0, (ucNAND_data + (usBlockNumber * (NAND_PAGES_IN_BLOCK * NFC_SECSZ)) + (ucPageNumberInBlock * NFC_SECSZ)), NFC_SECSZ); // read a page
    #endif
    };     
    uMemcpy(ptrBuffer, ((unsigned char *)NFC_RAM_0 + usOffsetInPage), usLength); // copy the read data from the first NAND flash buffer to user memory
#else
    SELECT_NAND();                                                       // select the chip
    if (usOffsetInPage < (NAND_PAGE_SIZE/2)) {                           // start in first half of page
        _NAND_command(NAND_CMD_READ1, 0);
        if (usLength == NAND_PAGE_SIZE) {
            iECC = 1;                                                    // ECC value to be calculated
        }
    }
    else if (usOffsetInPage < NAND_PAGE_SIZE) {                          // start in second half
        _NAND_command(NAND_CMD_READ_SECOND_HALF, 0);
        usOffsetInPage -= (NAND_PAGE_SIZE/2);
    }
    else {                                                               // start in spare area
        _NAND_command(NAND_CMD_READ_SPARE, 0);
        usOffsetInPage -= NAND_PAGE_SIZE;                                // column address (0..(NAND_SPARE_SIZE - 1))
    }
    usBlockNumber = ((usBlockNumber * NAND_PAGES_IN_BLOCK) + ucPageNumberInBlock); // calculate the page to be addresed
    WRITE_NAND_ADDRESS((unsigned char)usOffsetInPage);                   // column address
    WRITE_NAND_ADDRESS((unsigned char)usBlockNumber);                    // row address
    WRITE_NAND_ADDRESS((unsigned char)(usBlockNumber >> 8));
    NAND_WAIT_READY();                                                   // wait for read signal
    while (usLength--) {                                                 // read the requested amount of data
        *ptrBuffer++ = READ_NAND();
    }
    if (iECC != 0) {                                                     // a complete page read has been performed and the ECC is to be checked
        READ_NAND();                                                     // read stored ECC (4 bytes) so that the ECC can perform error checking and error correction
        READ_NAND();
        READ_NAND();
#ifdef _WINDOWS
    #if defined _HW_SAM7X
        ECC_SR = READ_NAND();                                            // when simulating 0 is written and so is expected as result
        if (ECC_SR != 0)  {
            ECC_SR = (MULERR | RECERR);
        }
    #endif
#else
        READ_NAND();
#endif
        if (ECC_SR & (MULERR | RECERR)) {                                // error detected
            if (ECC_SR & MULERR) {
                iECC = ECC_READ_ERROR;                                   // multiple bits in error and so unrecoverable
            }
            else {                                                       // recoverable single bit error in the page
                ptrBuffer -= NAND_PAGE_SIZE;
                ptrBuffer += (ECC_PR >> ECC_PR_WORDADDR_SHIFT);          // move to the location in the buffer where a bit error was detected
                *ptrBuffer ^= (0x01 << (ECC_PR & ECC_PR_BITADDR_MASK));  // invert the corrupted bit to correct the byte in the buffer
                iECC = UTFAT_SUCCESS;                                    // read successful - even though a bit correction was performed
            }
        }
        else {
            iECC = UTFAT_SUCCESS;
        }
    }
    DESELECT_NAND();                                                     // release the chip select line
#endif
    return iECC;                                                         // return the result of ECC check when a complete page was read (always successful when only partial read performed)
}

// User level call to read from NAND flash (the block number is the user's block number)
//
static int fnReadNANDsector(unsigned long ulUserSector, unsigned short usOffsetInSector, unsigned char *ptrBuffer, unsigned short usLength)
{
    return (_fnReadNANDdata(fnMapBlock(ulUserSector/NAND_PAGES_IN_BLOCK), (unsigned char)(ulUserSector%NAND_PAGES_IN_BLOCK), usOffsetInSector, ptrBuffer, usLength));
}



// Write data to NAND flash and add ECC if a page write (low level call using physical block number)
// Attempted writes to bad blocks are actively rejected
//
static int _fnWriteNANDdata(unsigned short usBlockToWrite, unsigned char ucPageNumberInBlock, unsigned short usOffsetInPage, unsigned char *ptrBuffer, unsigned short usLength)
{
#if !defined _KINETIS
    int iECC = 0;
    unsigned short usPageNumber;
#endif
    if (ptrBadBlocks[usBlockToWrite] != 0) {
        fnMemoryDebugMsg("Attempted write bad block\n\r");
        return BAD_BLOCK_CAN_NOT_BE_MODIFIED;
    }
#if defined _KINETIS                                                     // {2}
    uMemcpy(((unsigned char *)NFC_RAM_0 + usOffsetInPage), ptrBuffer, usLength); // copy the user data to the first NAND flash buffer
    NFC_ISR |= (NFC_ISR_WERRCLR | NFC_ISR_DONECLR | NFC_ISR_IDLECLR);    // clear flags before starting the read
    NFC_RAR = ((NFC_RAR_CS0 | NFC_RAR_RB0) | usBlockToWrite);            // row address for block
    NFC_CAR = ucPageNumberInBlock;                                       // column address to set page
    if (usLength == NAND_PAGE_SIZE) {
        NFC_CFG |= NFC_CFG_ECCMODE_60;                                   // ensure that ECC check is enabled before starting programming a complete page
    }
    else {
        NFC_CFG &= ~NFC_CFG_ECCMODE_60;
    }
    NFC_CMD2 = ((NAND_CMD_PAGEPROG1 << 24) | (NFC_CMD2_CODE_SEND_COMMAND_BYTE1_TO_FLASH | NFC_CMD2_CODE_SEND_COL_ADD1_TO_FLASH | NFC_CMD2_CODE_SEND_COL_ADD2_TO_FLASH | NFC_CMD2_CODE_SEND_ROW_ADD1_TO_FLASH | NFC_CMD2_CODE_SEND_ROW_ADD2_TO_FLASH | NFC_CMD2_CODE_SEND_ROW_ADD3_TO_FLASH | NFC_CMD2_CODE_WRITE_DATA_TO_FLASH | NFC_CMD2_CODE_SEND_COMMAND_BYTE2_TO_FLASH | NFC_CMD2_CODE_WAIT_FOR_FLASH_R_B_HANDSHAKE | NFC_CMD2_CODE_SEND_COMMAND_BYTE3_TO_FLASH | NFC_CMD2_CODE_READ_FLASH_STATUS));
    NFC_CMD1 = ((NAND_CMD_PAGEPROG2 << 24) | (NAND_CMD_READ_STATUS << 16));
    NFC_CMD2 |= NFC_CMD2_BUSY_START;
    while (NFC_CMD2 & NFC_CMD2_BUSY_START) {
    #if defined _WINDOWS
        NFC_CMD2 &= ~(NFC_CMD2_BUSY_START);                              // wait for the read to complete
        uMemcpy((ucNAND_data + (usBlockToWrite * (NAND_PAGES_IN_BLOCK * NFC_SECSZ)) + (ucPageNumberInBlock * NFC_SECSZ) + usOffsetInPage), (unsigned char *)(NFC_RAM_0 + usOffsetInPage), usLength); // write data content
    #endif
    };
#else
    SELECT_NAND();                                                       // select the chip
    if (usOffsetInPage < (NAND_PAGE_SIZE/2)) {                           // start in first half of page
        _NAND_command(NAND_CMD_READ1, 0);
        if (usLength == NAND_PAGE_SIZE) {
            iECC = 1;                                                    // ECC value to be calculated and set
        }
    }
    else if (usOffsetInPage < NAND_PAGE_SIZE) {                          // start in second half
        _NAND_command(NAND_CMD_READ_SECOND_HALF, 0);
        usOffsetInPage -= (NAND_PAGE_SIZE/2);
    }
    else {                                                               // start in spare area
        _NAND_command(NAND_CMD_READ_SPARE, 0);
        usOffsetInPage -= NAND_PAGE_SIZE;                                // column address (0..(NAND_SPARE_SIZE - 1))
    }
    _NAND_command(NAND_CMD_PAGEPROG1, 0);                                // start program command
    usPageNumber = ((usBlockToWrite * NAND_PAGES_IN_BLOCK) + ucPageNumberInBlock); // calculate the page to be addresed
    WRITE_NAND_ADDRESS((unsigned char)usOffsetInPage);                   // column address
    WRITE_NAND_ADDRESS((unsigned char)usPageNumber);                     // row address
    WRITE_NAND_ADDRESS((unsigned char)(usPageNumber >> 8));
    while (usLength--) {                                                 // write requested amount of data
        WRITE_NAND(*ptrBuffer++);                                        // write each byte
    }
    if (iECC != 0) {                                                     // a complete page read has been performed and the ECC is to be checked
        WRITE_NAND((unsigned char)(ECC_PR));                             // store ECC after writing a page
        WRITE_NAND((unsigned char)(ECC_PR >> 8));
        WRITE_NAND((unsigned char)(ECC_NPR));
        WRITE_NAND((unsigned char)(ECC_NPR >> 8));
    }
    _NAND_command(NAND_CMD_PAGEPROG2, 0);
    NAND_WAIT_READY();                                                   // wait for read signal
#endif
    if (fnReadNAND_status() & NAND_PROG_ERASE_ERROR) {
        fnMemoryDebugMsg("Page write failed\n\r");
        DESELECT_NAND();                                                 // negate the NAND flash chip select
        fnMarkBadBlock(usBlockToWrite);
        return WRITE_FAILED;
    }
    DESELECT_NAND();                                                     // negate the NAND flash chip select
    return UTFAT_SUCCESS;
}


// User level call to write to NAND flash (the block number is the user's block number)
//
static int fnWriteNANDsector(unsigned long ulUserSector, unsigned short usOffsetInPage, unsigned char *ptrBuffer, unsigned short usLength)
{
    int iReturn;
    unsigned short usUserBlockNumber = (unsigned short)(ulUserSector/NAND_PAGES_IN_BLOCK);
    iReturn = _fnWriteNANDdata(fnMapBlock(usUserBlockNumber), (unsigned char)(ulUserSector%NAND_PAGES_IN_BLOCK), usOffsetInPage, ptrBuffer, usLength);
    if (UTFAT_SUCCESS == iReturn) {
        ptrBlankBlocks[usUserBlockNumber] = 0;                           // the block is known to be not empty
    }
    return iReturn;
}

// Check all physical blocks at initialisation so see which are marked as bad blocks
//
static void fnCreateBadBlocktable(void)
{
    unsigned short usBlockToCheck = 0;
    unsigned short usBadBlockCount = 0;
    unsigned short ucBadInSwapArea = 0;
    unsigned short ucBadInRemapArea = 0;
#if defined NAND_16_BIT_WIDE
    unsigned short BadBlockPattern;
#else
    unsigned char  BadBlockPattern;
#endif

    ptrBadBlocks = NAND_MALLOC(NAND_BLOCK_COUNT);                        // get RAM for use as a bad block table

    for (usBlockToCheck = 0; usBlockToCheck < NAND_BLOCK_COUNT ; usBlockToCheck++) {
        _fnReadNANDdata(usBlockToCheck, 0, BLOCK_VALIDITY_LOCATION, (unsigned char *)&BadBlockPattern, sizeof(BadBlockPattern));// read the corresponding value from the spare area in first page of block
        if (BadBlockPattern == GOOD_BLOCK_PATTERN) {                     // first page doesn't inform of a bad block but also second page must be checked
            _fnReadNANDdata(usBlockToCheck, 1, BLOCK_VALIDITY_LOCATION, (unsigned char *)&BadBlockPattern, sizeof(BadBlockPattern)); // read the corresponding value from the spare area in second page of block
        }
        if (BadBlockPattern != GOOD_BLOCK_PATTERN) {                     // if non-good-block values are read from either of the first two page check locations the block has previously been marked as a bad block
            usBadBlockCount++;
            ptrBadBlocks[usBlockToCheck] = 1;                            // set bad block to our list
            if (usBlockToCheck < REMAP_SWAP_BLOCKS) {
                ucBadInSwapArea++;                                       // count bad blocks in the critical swap area
            }
            else if (usBlockToCheck < REMAP_BLOCKS) {
                ucBadInRemapArea++;                                      // count bad blocks in the remap area
            }
        }
    }
    fnMemoryDebugMsg("Number of bad blocks found in NAND = ");
    fnMemoryDebugDec(usBadBlockCount, WITH_CR_LF);
    fnMemoryDebugMsg("[Swap = ");
    fnMemoryDebugDec(ucBadInSwapArea, 0);
    fnMemoryDebugMsg(" from ");
    fnMemoryDebugDec(REMAP_SWAP_BLOCKS, 0);
    fnMemoryDebugMsg(", Remap = ");
    fnMemoryDebugDec(ucBadInRemapArea, 0);
    fnMemoryDebugMsg(" from ");
    fnMemoryDebugDec(REMAP_BLOCKS, 0);
    fnMemoryDebugMsg("]\r\n");
}


// This routine checks the 4 values that have been passed to it and returns the value that is most likely the correct one
// It expects that all 4 are the same, but will accept one or two erros and still return the one that occurs twice (redundancy)
// Two different values in the four input values is an error since the correct one can not be determined
//
static unsigned short fnGetBestBlock(unsigned short usBlock[4], int *iSingleErrors, int *DoubleErrors)
{
    unsigned short usMatch1 = usBlock[0];
    if ((usMatch1 == usBlock[1]) && (usMatch1 == usBlock[2]) && (usMatch1 == usBlock[3])) {
        return usMatch1;                                                 // all match - no bit errors
    }

    if (usMatch1 == usBlock[1]) {                                        // 2 match
        if ((usMatch1 == usBlock[2]) || (usMatch1 == usBlock[3])) {      // at least 3 matches
            ++(*iSingleErrors);                                          // count single errors
            return usMatch1;                                             // value accepted
        }
        else if (usBlock[2] != usBlock[3]) {                             // as long as the other two don't also match we accept a double error
            ++(*DoubleErrors);                                           // count double errors
            return usMatch1;                                             // value accepted
        }
    }

    if (usMatch1 == usBlock[2]) {                                        // 2 match
        if (usMatch1 == usBlock[3]) {                                    // at least 3 matches
            ++(*iSingleErrors);                                          // count single errors
            return usMatch1;                                             // value accepted
        }
        else if (usBlock[1] != usBlock[3]) {                             // as long as the other two don't also match we accept a double error
            ++(*DoubleErrors);                                           // count double errors
            return usMatch1;                                             // value accepted
        }
    }

    if (usMatch1 == usBlock[3]) {                                        // 2 match
        if (usBlock[1] != usBlock[2]) {                                  // as long as the other two don't also match we accept a double error
            ++(*DoubleErrors);                                           // count double errors
            return usMatch1;                                             // value accepted
        }
    }

    // The first entry doesn't match with any other so it is bad - but the other three can still be good
    //
    usMatch1 = usBlock[1];
    if ((usMatch1 == usBlock[2]) && (usMatch1 == usBlock[3])) {
        ++(*iSingleErrors);                                              // count single errors
        return usMatch1;                                                 // three match
    }
    if ((usMatch1 == usBlock[2]) || (usMatch1 == usBlock[3])) {
        ++(*DoubleErrors);                                               // count double errors
        return usMatch1;                                                 // two match
    }

    if (usBlock[2] == usBlock[3]) {                                      // final chance that last 2 match
        ++(*DoubleErrors);                                               // count double errors
        return usBlock[2];                                               // two match
    }
    return 0xffff;                                                       // value not accepted
}

// Same algorithm for long word entries
//
static unsigned long fnGetBestDeletes(unsigned long ulBlock[4], int *iSingleErrors, int *DoubleErrors)
{
    unsigned long ulMatch1 = ulBlock[0];
    if ((ulMatch1 == ulBlock[1]) && (ulMatch1 == ulBlock[2]) && (ulMatch1 == ulBlock[3])) {
        return ulMatch1;                                                 // all match - no bit errors
    }

    if (ulMatch1 == ulBlock[1]) {                                        // 2 match
        if ((ulMatch1 == ulBlock[2]) || (ulMatch1 == ulBlock[3])) {      // at least 3 matches
            ++(*iSingleErrors);                                          // count single errors
            return ulMatch1;                                             // value accepted
        }
        else if (ulBlock[2] != ulBlock[3]) {                             // as long as the other two don't also match we accept a double error
            ++(*DoubleErrors);                                           // count double errors
            return ulMatch1;                                             // value accepted
        }
    }

    if (ulMatch1 == ulBlock[2]) {                                        // 2 match
        if (ulMatch1 == ulBlock[3]) {                                    // at least 3 matches
            ++(*iSingleErrors);                                          // count single errors
            return ulMatch1;                                             // value accepted
        }
        else if (ulBlock[1] != ulBlock[3]) {                             // as long as the other two don't also match we accept a double error
            ++(*DoubleErrors);                                           // count double errors
            return ulMatch1;                                             // value accepted
        }
    }

    if (ulMatch1 == ulBlock[3]) {                                        // 2 match
        if (ulBlock[1] != ulBlock[2]) {                                  // as long as the other two don't also match we accept a double error
            ++(*DoubleErrors);                                           // count double errors
            return ulMatch1;                                             // value accepted
        }
    }

    // The first entry doesn't match with any other so it is bad - but the other three can still be good
    //
    ulMatch1 = ulBlock[1];
    if ((ulMatch1 == ulBlock[2]) && (ulMatch1 == ulBlock[3])) {
        ++(*iSingleErrors);                                              // count single errors
        return ulMatch1;                                                 // three match
    }
    if ((ulMatch1 == ulBlock[2]) || (ulMatch1 == ulBlock[3])) {
        ++(*DoubleErrors);                                               // count double errors
        return ulMatch1;                                                 // two match
    }

    if (ulBlock[2] == ulBlock[3]) {                                      // final chance that last 2 match
        ++(*DoubleErrors);                                               // count double errors
        return ulBlock[2];                                               // two match
    }
    return 0xffffffff;                                                   // value not accepted
}



// If the swap area is valid it takes presidence over the remapping area. The remapping area is deleted, if not already blank.
// The swap area will be used until the next block exchange is required, after which it will be deleted.
// Only the first non-bad block in the swap area is ever used.
//
static int fnCheckSwapArea(void)
{
    unsigned char ucSwapDate[NAND_PAGE_SIZE];
    unsigned short usSwapBlock = 0;
    unsigned char ucSwapPage = 0;
    int iBufferOffset;
    int iRemainingBytes = 0;
    int iUserBlockEntry = 0;
    NAND_SWAP_BLOCK_INFO SwapInfo = {0};
    while (ptrBadBlocks[usSwapBlock] != 0) {                             // skip bad blocks
        usSwapBlock++;
    }
    if (usSwapBlock >= REMAP_SWAP_BLOCKS) {                              // all blocks have failed
        return NO_GOOD_BLOCKS_IN_SWAP_AREA;
    }
    while (ucSwapPage < NAND_PAGES_IN_BLOCK) {                          // read all pages in the swap block (only one block content is required)
        _fnReadNANDdata(usSwapBlock, ucSwapPage++, 0, ucSwapDate, NAND_PAGE_SIZE); // read a page of mapping information with ECC support
        iBufferOffset = 0;
        switch (iRemainingBytes) {
        case 2:
            SwapInfo.ulNumberOfDeletes |= (ucSwapDate[iBufferOffset++] << 8);
            SwapInfo.ulNumberOfDeletes |= (ucSwapDate[iBufferOffset++]);
            if (SwapInfo.ulNumberOfDeletes == 0xffffffff) {              // blank entry signals end of list
                if (iUserBlockEntry < USER_AREA_BLOCKS) {
                    return SWAP_AREA_NOT_VALID;
                }
                return UTFAT_SUCCESS;
            }
            SwapInfo.ulPhysicalBlockNumber  = (ucSwapDate[iBufferOffset++] << 8);
            SwapInfo.ulPhysicalBlockNumber |= (ucSwapDate[iBufferOffset++]);
            ptrRemap[iUserBlockEntry] = SwapInfo.ulPhysicalBlockNumber;  // the physical block that it is mapped to
            ptrWearLevel[iUserBlockEntry] = SwapInfo.ulNumberOfDeletes;  // the delete count corresponding to the user block
            iUserBlockEntry++;
            break;
        case 4:
            SwapInfo.ulPhysicalBlockNumber  = (ucSwapDate[iBufferOffset++] << 8);
            SwapInfo.ulPhysicalBlockNumber |= (ucSwapDate[iBufferOffset++]);
            ptrRemap[iUserBlockEntry] = SwapInfo.ulPhysicalBlockNumber;  // the physical block that it is mapped to
            ptrWearLevel[iUserBlockEntry] = SwapInfo.ulNumberOfDeletes;  // the delete count corresponding to the user block
            iUserBlockEntry++;
            break;
        }
        iRemainingBytes = 0;
        while(iBufferOffset < NAND_PAGE_SIZE) {
            if ((NAND_PAGE_SIZE - iBufferOffset) < (sizeof(SwapInfo.ulNumberOfDeletes) + sizeof(SwapInfo.ulPhysicalBlockNumber))) { // not enough data for a complete entry
                iRemainingBytes = (NAND_PAGE_SIZE - iBufferOffset);
                break;
            }
            SwapInfo.ulNumberOfDeletes  = (ucSwapDate[iBufferOffset++] << 24);
            SwapInfo.ulNumberOfDeletes |= (ucSwapDate[iBufferOffset++] << 16);
            SwapInfo.ulNumberOfDeletes |= (ucSwapDate[iBufferOffset++] << 8);
            SwapInfo.ulNumberOfDeletes |= (ucSwapDate[iBufferOffset++]);
            if (SwapInfo.ulNumberOfDeletes == 0xffffffff) {              // blank entry signals end of list
                if (iUserBlockEntry < USER_AREA_BLOCKS) {
                    return SWAP_AREA_NOT_VALID;
                }
                return UTFAT_SUCCESS;
            }
            SwapInfo.ulPhysicalBlockNumber  = (ucSwapDate[iBufferOffset++] << 8);
            SwapInfo.ulPhysicalBlockNumber |= (ucSwapDate[iBufferOffset++]);
            ptrRemap[iUserBlockEntry] = SwapInfo.ulPhysicalBlockNumber;  // the physical block that it is mapped to
            ptrWearLevel[iUserBlockEntry] = SwapInfo.ulNumberOfDeletes;  // the delete count corresponding to the user block
            iUserBlockEntry++;
        }
        switch (iRemainingBytes) {
        case 2:
            SwapInfo.ulNumberOfDeletes  = (ucSwapDate[iBufferOffset++] << 24); // collect final 2 bytes
            SwapInfo.ulNumberOfDeletes |= (ucSwapDate[iBufferOffset++] << 16);
            break;
        case 4:
            SwapInfo.ulNumberOfDeletes  = (ucSwapDate[iBufferOffset++] << 24); // collect final 4 bytes
            SwapInfo.ulNumberOfDeletes |= (ucSwapDate[iBufferOffset++] << 16);
            SwapInfo.ulNumberOfDeletes |= (ucSwapDate[iBufferOffset++] << 8);
            SwapInfo.ulNumberOfDeletes |= (ucSwapDate[iBufferOffset++]);
            if (SwapInfo.ulNumberOfDeletes == 0xffffffff) {              // blank entry signals end of list
                if (iUserBlockEntry < USER_AREA_BLOCKS) {
                    return SWAP_AREA_NOT_VALID;
                }
                return UTFAT_SUCCESS;
            }
            break;
        }
    }
    return SWAP_AREA_NOT_VALID;
}

// Check whether there is non-blank data in the defined blocks of the remapping area and delete any that are
// not empty
//
static void fnCleanRemapArea(unsigned short usFirstBlock, unsigned short usEndBlock, unsigned short usMinimumBlocks)
{
    unsigned long ulCheckBuffer[NAND_PAGE_SIZE/sizeof(unsigned long)];
    int i;
    unsigned short usRemapBlock;
    unsigned short usBlocksDeleted = 0;
    unsigned char ucPage;
    for (usRemapBlock = usFirstBlock; usRemapBlock < usEndBlock; usRemapBlock++) { // ensure that the remap area is clean and delete anything found if not
        if (ptrBadBlocks[usRemapBlock] == 0) {                           // skip bad blocks in this area
            for (ucPage = 0; ucPage < NAND_PAGES_IN_BLOCK; ucPage++) {
                _fnReadNANDdata(usRemapBlock, ucPage, 0, (unsigned char *)ulCheckBuffer, NAND_PAGE_SIZE); // read a page of mapping information but ignore its ECC
                for (i = 0; i < (NAND_PAGE_SIZE/sizeof(unsigned long)); i++) {
                    if (ulCheckBuffer[i] != 0xffffffff) {
                        break;
                    }
                }
                if (i != (NAND_PAGE_SIZE/sizeof(unsigned long))) {
                    _fnEraseNAND_block(usRemapBlock);                    // if any non-deleted content is found delete the complete block
                    break;
                }
            }
            if (usMinimumBlocks != 0) {                                  // limit the number of deletes if required
                if (usBlocksDeleted >= usMinimumBlocks) {
                    while (usRemapBlock < usEndBlock) {
                        ptrRemapDeletes[usRemapBlock++] = 1;             // don't delete more blocks but mark that they need deleteing at the next opportunity
                    }
                    return;
                }
                else {
                    usBlocksDeleted++;
                }
            }
        }
    }
}


// This routine is used when there is no more space in the remap sector to add block change info.
// The block change info is already in the RAM tables but the present RAM table information needs to be consolidated
// and programmed to the swap buffer, which is used until the next change is made. The swap buffer ensures that no
// information is lost in case of power loss when deleting the remap area
//
static int fnConsolidateRemapInfo(unsigned short usMinimumDeleteBlocks)
{
    unsigned char ucPageBuffer[NAND_PAGE_SIZE];
    int iUserEntries = 0;
    int iBufferOffset = 0;
    unsigned short usSwapBlock = 0;
    unsigned char ucPageInSwapBlock = 0;

    while (ptrBadBlocks[usSwapBlock] != 0) {                             // skip bad blocks
        usSwapBlock++;
    }
    if (usSwapBlock >= REMAP_SWAP_BLOCKS) {                              // all blocks have failed
        return NO_GOOD_BLOCKS_IN_SWAP_AREA;
    }
    uMemset(ucPageBuffer, 0xff, sizeof(ucPageBuffer));

    while (iUserEntries < MAX_USER_BLOCKS) {
        ucPageBuffer[iBufferOffset++] = (unsigned char)(ptrWearLevel[iUserEntries] >> 24);
        ucPageBuffer[iBufferOffset++] = (unsigned char)(ptrWearLevel[iUserEntries] >> 16);
        if (iBufferOffset >= NAND_PAGE_SIZE) {
            _fnWriteNANDdata(usSwapBlock, ucPageInSwapBlock++, 0, ucPageBuffer, NAND_PAGE_SIZE); // write to page in the swap block
            iBufferOffset = 0;
            uMemset(ucPageBuffer, 0xff, sizeof(ucPageBuffer));
        }
        ucPageBuffer[iBufferOffset++] = (unsigned char)(ptrWearLevel[iUserEntries] >> 8);
        ucPageBuffer[iBufferOffset++] = (unsigned char)(ptrWearLevel[iUserEntries]);
        if (iBufferOffset >= NAND_PAGE_SIZE) {
            _fnWriteNANDdata(usSwapBlock, ucPageInSwapBlock++, 0, ucPageBuffer, NAND_PAGE_SIZE); // write to page in the swap block
            iBufferOffset = 0;
            uMemset(ucPageBuffer, 0xff, sizeof(ucPageBuffer));
        }
        ucPageBuffer[iBufferOffset++] = (unsigned char)(ptrRemap[iUserEntries] >> 8);
        ucPageBuffer[iBufferOffset++] = (unsigned char)(ptrRemap[iUserEntries]);
        if (iBufferOffset >= NAND_PAGE_SIZE) {
            _fnWriteNANDdata(usSwapBlock, ucPageInSwapBlock++, 0, ucPageBuffer, NAND_PAGE_SIZE); // write to page in the swap block
            iBufferOffset = 0;
            uMemset(ucPageBuffer, 0xff, sizeof(ucPageBuffer));
        }
        iUserEntries++;
    }
    if (iBufferOffset != 0) {
        _fnWriteNANDdata(usSwapBlock, ucPageInSwapBlock++, 0, ucPageBuffer, (unsigned short)iBufferOffset); // write final partial page in the swap block
    }
    fnCleanRemapArea(REMAP_SWAP_BLOCKS, REMAP_AREA_BLOCKS, usMinimumDeleteBlocks); // delete part the remap area ready for next use
    iSwapBufferValid = 1;                                                // flag that the swap buffer is the present location of data. On following update it wil be copied back to the remap area again
    return UTFAT_SUCCESS;
}

// This is used when the remap information is stored in the swap block.
// It builds a new initial map in the main remap area and deletes the swap block - the main remap area will be clean
//
static int fnActivateRemapArea(void)
{
    NAND_BLOCK_INFO remap_info[NAND_PAGE_SIZE/sizeof(NAND_BLOCK_INFO)];  // temporary buffer content
    int i = 0;
    unsigned short usUserBlock;

    usLastBlock = REMAP_SWAP_BLOCKS;
    ucLastPage = 0;
    usLastOffset = 0;

    for (usUserBlock = 0; usUserBlock < MAX_USER_BLOCKS; usUserBlock++) {// for each user block
        remap_info[i].ulNumberOfDeletes[0] = remap_info[i].ulNumberOfDeletes[1] = remap_info[i].ulNumberOfDeletes[2] = remap_info[i].ulNumberOfDeletes[3] = ptrWearLevel[usUserBlock]; // the numer delete cycles the block has alread been subjected to
        remap_info[i].usUserBlockNumber[0] = remap_info[i].usUserBlockNumber[1] = remap_info[i].usUserBlockNumber[2] = remap_info[i].usUserBlockNumber[3] = usUserBlock; // the user block number
        remap_info[i].usPhysicalBlockNumber[0] = remap_info[i].usPhysicalBlockNumber[1] = remap_info[i].usPhysicalBlockNumber[2] = remap_info[i].usPhysicalBlockNumber[3] = ptrRemap[usUserBlock]; // the physical block that it is mapped to
        usLastOffset++;
        i++;
        if (i >= (NAND_PAGE_SIZE/sizeof(NAND_BLOCK_INFO))) {             // a complete page in the remap area is ready
            while (BAD_BLOCK_CAN_NOT_BE_MODIFIED == _fnWriteNANDdata(usLastBlock, ucLastPage, 0, (unsigned char *)remap_info, NAND_PAGE_SIZE)) {
                if (++usLastBlock >= REMAP_AREA_BLOCKS) {                // jump bad blocks in the remap area
                    return REMAP_TABLE_FAILED_DUE_TO_BAD_NAND;
                }
            }
            ucLastPage++;
            if (ucLastPage >= NAND_PAGES_IN_BLOCK) {
                ucLastPage = 0;
                usLastBlock++;
            }
            i = 0;
        }
    }
    usLastOffset = (i * (sizeof(NAND_BLOCK_INFO)));                      // calculate the final offset in the page
    if (i != 0) {                                                        // if a full page has not been programmed
        while (BAD_BLOCK_CAN_NOT_BE_MODIFIED == _fnWriteNANDdata(usLastBlock, ucLastPage, 0, (unsigned char *)remap_info, usLastOffset)) { // program just partial buffer
            if (++usLastBlock >= REMAP_AREA_BLOCKS) {                    // jump bad blocks in the remap area
                return REMAP_TABLE_FAILED_DUE_TO_BAD_NAND;
            }
        }
    }
    fnCleanRemapArea(0, REMAP_SWAP_BLOCKS, 1);                           // clean out any data in the swap area (maximum first block needed)
    iSwapBufferValid = 0;                                                // swap block no longer valid
    return UTFAT_SUCCESS;
}

static void fnDeleteWaitingRemaps(void)
{
    unsigned short usRemapBlock;
    for (usRemapBlock = REMAP_SWAP_BLOCKS; usRemapBlock < REMAP_AREA_BLOCKS; usRemapBlock++) {
        if (ptrRemapDeletes[usRemapBlock] != 0) {
            _fnEraseNAND_block(usRemapBlock);                            // erase next in the list so that it is blank when next needed
            ptrRemapDeletes[usRemapBlock] = 0;
            return;
        }
    }
}

static void fnIncLastEntry(void)                                         // {1} general routine to advance remap management pointers
{
    usLastOffset += sizeof(NAND_BLOCK_INFO);                             // advance the remap area pointers
    if (usLastOffset >= NAND_PAGE_SIZE) {                                // moves to a new page in the block
        usLastOffset = 0;
        ucLastPage++;
        if (ucLastPage >= NAND_PAGES_IN_BLOCK) {                         // reached the end of a block
            ucLastPage = 0;
            do {
                usLastBlock++;
            } while (ptrBadBlocks[usLastBlock]);                         // skip bad blocks 
        }
        fnDeleteWaitingRemaps();                                         // if there are remap blocks waiting to be deleted one can be deleted now
    }
}


static int fnInitialiseBlockMap(int iCreate)
{
    int i;
    unsigned short usBlock = 0;
    unsigned short usPage;
    int iSingleErrors = 0;
    int iDoubleErrors = 0;
    int iCheckBlank = 0;

    if (iCreate != 0) {                                                  // create a fresh address map table in NAND Flash
        fnCleanRemapArea(REMAP_SWAP_BLOCKS, REMAP_AREA_BLOCKS, 0);       // clean out the remapping area ready for first use
        i = REMAP_AREA_BLOCKS;
        for (usBlock = 0; usBlock < MAX_USER_BLOCKS; usBlock++) {
            do {
                ptrRemap[usBlock] = i++;
            } while (ptrBadBlocks[ptrRemap[usBlock]] != 0);
        }
        return (fnActivateRemapArea());
    }

    if (ptrRemap == 0) {
        ptrRemap = NAND_MALLOC((MAX_USER_BLOCKS) * sizeof(unsigned short)); // get zeroed memory for RAM copy of remap table
        ptrWearLevel = NAND_MALLOC((MAX_USER_BLOCKS) * sizeof(unsigned long)); // get zeroed memory for RAM copy of wear level table
        ptrBlankBlocks = NAND_MALLOC((MAX_USER_BLOCKS) * sizeof(unsigned char)); // get zeroed memory for a table of presently blank spare blocks
        ptrRemapDeletes = NAND_MALLOC((REMAP_AREA_BLOCKS) * sizeof(unsigned char)); // remapping area blocks waiting to be deleted
        ptrSpareDeletes = NAND_MALLOC((MAX_USER_BLOCKS - USER_AREA_BLOCKS) * sizeof(unsigned char)); // spare area blocks waiting to be deleted
    }
    if (fnCheckSwapArea() == UTFAT_SUCCESS) {
        fnCleanRemapArea(REMAP_SWAP_BLOCKS, REMAP_AREA_BLOCKS, 0);       // completely clean the swap area
        iSwapBufferValid = 1;                                            // flag that the swap buffer is the present location of data. On following update it wil be copied back to the remap area again
    }
    else {
        NAND_BLOCK_INFO remap_info[NAND_PAGE_SIZE/sizeof(NAND_BLOCK_INFO)]; // temporary buffer content
        for (usBlock = REMAP_SWAP_BLOCKS; usBlock < REMAP_AREA_BLOCKS; usBlock++) { // for each block in the remap area
            if (ptrBadBlocks[usBlock] == 0) {                            // bad blocks are lost in this area and so a larger amount of reserved space is of advantage
                unsigned short usRealBlock;
                unsigned short usMappedBlock;
                unsigned long  ulDeleteCount;
                for (usPage = 0; usPage < NAND_PAGES_IN_BLOCK; usPage++) { // for each page in this remap block
                    _fnReadNANDdata(usBlock, (unsigned char)(usPage % NAND_PAGES_IN_BLOCK), 0, (unsigned char *)remap_info, NAND_PAGE_SIZE); // read a page of mapping information but ignore its ECC
                    for (i = 0; i < (NAND_PAGE_SIZE/sizeof(NAND_BLOCK_INFO)); i++) { // for each entry
                        if (iCheckBlank != 0) {                          // end has already been detected so check for non-blank locations
                            unsigned long *ptrCheck = (unsigned long *)&remap_info[i];
                            int j = (sizeof(remap_info[i])/ sizeof(unsigned long));
                            if (j--) {
                                if (*ptrCheck++ != 0xffffffff) {
                                    goto _check_error;                   // non-blank following content found. We need to clean up before continuing!
                                }
                            }
                            continue;
                        }
                        usRealBlock = fnGetBestBlock(remap_info[i].usPhysicalBlockNumber, &iSingleErrors, &iDoubleErrors);
                        if (usRealBlock < NAND_BLOCK_COUNT) {            // potentially valid entry
                            usMappedBlock = fnGetBestBlock(remap_info[i].usUserBlockNumber, &iSingleErrors, &iDoubleErrors);
                            if (usMappedBlock < NAND_BLOCK_COUNT) {      // valid mapping
                                ptrRemap[usMappedBlock] = usRealBlock;   // enter what seems to be a valid entry
                                usLastBlock = usBlock;
                                ucLastPage = (unsigned char)(usPage % NAND_PAGES_IN_BLOCK);
                                usLastOffset = (unsigned short)(i * sizeof(NAND_BLOCK_INFO)); // save the location containing the last found valid entry
                                ulDeleteCount = fnGetBestDeletes(remap_info[i].ulNumberOfDeletes, &iSingleErrors, &iDoubleErrors);
                                if (ulDeleteCount == 0xffffffff) {       // if the entry is not valid set an intermediate result
                                    ulDeleteCount = 50000;
                                }
                                ptrWearLevel[usMappedBlock] = ulDeleteCount;
                            }
                            else {
                                iCheckBlank = 1;                         // end of valid entries found so check that there are no non-blank entries after it
                            }
                        }
                        else {
                            iCheckBlank = 1;                             // end of valid entries found so check that there are no non-blank entries after it
                        }
                    }
                }
            }
        }
        fnIncLastEntry();                                                // {1} advance to position ready for next swap entry
    }
    iCheckBlank = 0;
_check_error:
    // We have now retrieved the most up to date block mapping information but we need to check its integrity
    //
    for (i = 0; i < MAX_USER_BLOCKS; i++) {
        if (ptrRemap[i] == 0) {                                          // no (more) entry found
            // We expect one entry of each user block (even when blocks are bad). If this is not the case there is an error or the remap area hasn't been initialised
            //
            break;
        }
    }
    fnMemoryDebugMsg("Block map table size = ");
    fnMemoryDebugDec(i, WITH_CR_LF);
    if (iSwapBufferValid != 0) {
        fnMemoryDebugMsg("Swap area active\r\n");
    }
    else {
        fnMemoryDebugMsg("Single errors in the block map table = ");
        fnMemoryDebugDec(iSingleErrors, WITH_CR_LF);
        fnMemoryDebugMsg("Double errors in the block map table = ");
        fnMemoryDebugDec(iDoubleErrors, WITH_CR_LF);
    }

    if (i < USER_AREA_BLOCKS) {                                          // at least the number of user blocks must be available
        return NO_REMAP_TABLE_FOUND;
    }

    if ((iCheckBlank != 0) || (iSingleErrors > MAX_SINGLE_ERRORS_TOLERATED) || (iDoubleErrors != 0)) { // non-blank following, or there are redundancy errors in the remapping NAND Flash - so clean up
        fnMemoryDebugMsg("Consolidate Remap\r\n");
        fnConsolidateRemapInfo(0);                                       // delete all remapping blocks
    }
    return UTFAT_SUCCESS;                                                // OK
}

// This routine adds a new remapping information block to the end of the remap data
//
static int fnUpdateRemapInfo(NAND_BLOCK_INFO info_block[2])
{
    int i;
    unsigned short usFirstBlock = 0;
    unsigned short usFirstOffset = 0;
    unsigned char  ucFirstPage = 0;
    
    if (iSwapBufferValid != 0) {                                         // presently using swap block as remap storage space
        fnMemoryDebugMsg("Activate Remap\r\n");
        return fnActivateRemapArea();                                    // activate the remap area and make changes there
    }

    for (i = 0; i < 2; i++) {
        if (i == 0) {
            usFirstBlock  = usLastBlock;                                 // save the location for the first information block
            usFirstOffset = usLastOffset;
            ucFirstPage   = ucLastPage;
        }
        else {
            if (usLastBlock >= REMAP_AREA_BLOCKS) {                      // end of the remap area reached
                fnMemoryDebugMsg("Consolidate Remap\r\n");
                return fnConsolidateRemapInfo(((MAX_USER_BLOCKS*sizeof(NAND_BLOCK_INFO) + (NAND_BLOCK_SIZE - 1))/NAND_BLOCK_SIZE)); // delete enough room to ensure that the new start remap table can fit
            }
            _fnWriteNANDdata(usLastBlock, ucLastPage, usLastOffset, (unsigned char *)&info_block[1], sizeof(NAND_BLOCK_INFO)); // add on the new information to the end of the data
            _fnWriteNANDdata(usFirstBlock, ucFirstPage, usFirstOffset, (unsigned char *)&info_block[0], sizeof(NAND_BLOCK_INFO)); // add on the new information to the end of the data
        }
        fnIncLastEntry();                                                // {1} use general routine
    }
    return UTFAT_SUCCESS;
}

// Exchange two user blocks in the remap table, update the replaced ones delete counter and delete it
//
static void fnExchangeBlock(unsigned short usSubstituteBlock, unsigned short usBlockToReplace)
{
    unsigned short usOriginalBlock = ptrRemap[usBlockToReplace];
    unsigned long  ulOriginalDeletes = ptrWearLevel[usBlockToReplace];
    NAND_BLOCK_INFO usNewRemap[2];                                       // new information to be added to the end of the remap area
    ptrRemap[usBlockToReplace] = ptrRemap[usSubstituteBlock];
    ptrWearLevel[usBlockToReplace] = ptrWearLevel[usSubstituteBlock];
    ptrRemap[usSubstituteBlock] = usOriginalBlock;
    ulOriginalDeletes++;                                                 // the original block will be deleted for increment its delete count
    ptrWearLevel[usSubstituteBlock] = ulOriginalDeletes;                 // swap has been made in the RAM tables - now create a new entry for the remap area
    usNewRemap[0].ulNumberOfDeletes[0] = usNewRemap[0].ulNumberOfDeletes[1] = usNewRemap[0].ulNumberOfDeletes[2] = usNewRemap[0].ulNumberOfDeletes[3] = ptrWearLevel[usBlockToReplace];
    usNewRemap[0].usPhysicalBlockNumber[0] = usNewRemap[0].usPhysicalBlockNumber[1] = usNewRemap[0].usPhysicalBlockNumber[2] = usNewRemap[0].usPhysicalBlockNumber[3] = ptrRemap[usBlockToReplace];
    usNewRemap[0].usUserBlockNumber[0] = usNewRemap[0].usUserBlockNumber[1] = usNewRemap[0].usUserBlockNumber[2] = usNewRemap[0].usUserBlockNumber[3] = usBlockToReplace;
    usNewRemap[1].ulNumberOfDeletes[0] = usNewRemap[1].ulNumberOfDeletes[1] = usNewRemap[1].ulNumberOfDeletes[2] = usNewRemap[1].ulNumberOfDeletes[3] = ulOriginalDeletes;
    usNewRemap[1].usPhysicalBlockNumber[0] = usNewRemap[1].usPhysicalBlockNumber[1] = usNewRemap[1].usPhysicalBlockNumber[2] = usNewRemap[1].usPhysicalBlockNumber[3] = usOriginalBlock;
    usNewRemap[1].usUserBlockNumber[0] = usNewRemap[1].usUserBlockNumber[1] = usNewRemap[1].usUserBlockNumber[2] = usNewRemap[1].usUserBlockNumber[3] = usSubstituteBlock;
    fnUpdateRemapInfo(usNewRemap);                                       // set the user block remapping to the new physical block
    ptrSpareDeletes[usSubstituteBlock - USER_AREA_BLOCKS] = 1;           // the original block is now in the spare use area and can be deleted when the opportunity arrises
    uTaskerMonoTimer(OWN_TASK, T_CLEAN_SPARE, E_CLEAN_SPARE);            // background task of cleaning the spare area
}

// Return the user block number of a completly deleted block in the spare user area, preferably the one with lowest delete cycles
//
static unsigned short fnFindEmptyBlock(void)
{
    unsigned long  ulLowestDeleteCount = 0xffffffff;
    unsigned long  ulLowestSecondChoice = 0xffffffff;
    unsigned short usUserBlock = USER_AREA_BLOCKS;                       // start after the file system's user blocks
    unsigned short usBestUserBlock = 0;
    unsigned short usBestSecondChoice = 0;    
    while (usUserBlock < MAX_USER_BLOCKS) {
        if (ptrBadBlocks[usUserBlock] == 0) {
            if (ptrBlankBlocks[usUserBlock] != 0) {                      // if empty
                if (ptrWearLevel[usUserBlock] < ulLowestDeleteCount) {
                    usBestUserBlock = usUserBlock;                       // empty user block with lowest amount of deletes
                    ulLowestDeleteCount = ptrWearLevel[usUserBlock];
                }
            }
            else if (ptrSpareDeletes[usUserBlock - USER_AREA_BLOCKS] != 0) { // block is still waiting to be deleted but if not bad enter it as second choice
                if (ptrWearLevel[usUserBlock] < ulLowestSecondChoice) {
                    usBestSecondChoice = usUserBlock;                    // non-empty user block with lowest amount of deletes
                    ulLowestSecondChoice = ptrWearLevel[usUserBlock];
                }
            }
        }
        usUserBlock++;
    }
    if (usBestUserBlock == 0) {                                          // if no best choice it means that all blocks are waiting to be deleted or also all are bad
        if (usBestSecondChoice != 0) {                                   // if there is a second choice we must delete it now
            if (_fnEraseNAND_block(fnMapBlock(usBestSecondChoice)) == UTFAT_SUCCESS) { // erase original block once new has become valid
                ptrBlankBlocks[usBestSecondChoice] = 1;                  // mark that the original block is now blank
                ptrSpareDeletes[usBestSecondChoice - USER_AREA_BLOCKS] = 0; // no longer needs to be deleted
            }
        }
        return usBestSecondChoice;
    }
    return usBestUserBlock;                                              // return user block best suited to being used
}

// Read the device information from the NAND device
//
static void fnGetNAND_info(unsigned char ucDeviceInfo[2])
{
    SELECT_NAND();
    _NAND_command(NAND_CMD_READID, (NFC_CMD2_CODE_SEND_COMMAND_BYTE1_TO_FLASH | NFC_CMD2_CODE_SEND_ROW_ADD1_TO_FLASH | NFC_CMD2_CODE_READ_ID));
#if !defined _KINETIS
    WRITE_NAND_ADDRESS(0x00);
#endif
    ucDeviceInfo[0] = READ_NAND_ID_0();                                  // manufacturer ID
    ucDeviceInfo[1] = READ_NAND_ID_1();                                  // device type ID
    DESELECT_NAND();
}


// Development code for NAND Flash on SAM7SE board or Kinetis K70
//
static void fnInitNAND(void)
{
    unsigned char ucDeviceInfo[2];

#ifdef _WINDOWS                                                          // read in the simulated NAND content
    #if _VC80_UPGRADE<0x0600
	iFileNAND = _open(NAND_FILE, (_O_BINARY | _O_RDWR));
    #else
	_sopen_s(&iFileNAND, NAND_FILE, (_O_BINARY | _O_RDWR), _SH_DENYWR, _S_IREAD);
    #endif
    if (iFileNAND >= 0) {
        _read(iFileNAND, ucNAND_data, sizeof(ucNAND_data));
        _close(iFileNAND);
    }
    else {
        uMemset(ucNAND_data, 0xff, sizeof(ucNAND_data));                 // simulate start with empty NAND flash
    }
#endif
#if defined _HW_SAM7X
    EBI_CSA |= EBI_CSA_CS3A_NAND;                                        /// activate the NAND flash logic connected to the static memory controller

    // 2 wait states, 2 data float cycle, connected in 8-bit mode, standard data read protocol, standard chip select assertion waveform, no extra RW setup cycles but 1 extra RW hold cycle
    //
    SMC_CSR3 = (SMC_SCR_NWS_2_WAIT_STATES | SMC_SCR_TDF_2 | SMC_SCR_DBW_8 | SMC_SCR_DRP_STANDARD | SMC_SCR_ACSS_STANDARD | SMC_SCR_RWSETUP_0 | SMC_SCR_RWHOLD_1);

    POWER_UP(PIOC);                                                      // configure peripheral and port interface pins
    PIO_ASR_C  = (D0 | D1 | D2 | D3 | D4 | D5 | D6 | D7 | A21_NANDALE | A22_REG_NANDCLE);
    PIO_BSR_C  = (NANDOE_B | NANDWE_B);
    PIO_PDR_C  = (D0 | D1 | D2 | D3 | D4 | D5 | D6 | D7 | A21_NANDALE | A22_REG_NANDCLE | NANDOE_B | NANDWE_B);
    _DISABLE_PULLUP(C, (NWR0_NWE_CFWE_B | NDR_CFOE_B));                  // dsable pull-ups
    _CONFIG_PORT_INPUT(B, (NAND_BUSY_PIN));                              // ready/busy input
    _CONFIG_PORT_OUTPUT(B, (NAND_CHIP_SELECT));                          // chip select line

    ECC_CR = EEC_CR_RST;                                                 // ECC software reset
    ECC_MR = ECC_MR_NAND_PAGE_SIZE_528;                                  // configure the ECC with NAND page size
#elif defined _KINETIS                                                   // {2} configure the pins for the 8/16 bit NAND
    SIM_CLKDIV4 |= (4 << 27);                                            // set the fractional clock divider (note that 
    MPU_CESR = 0;                                                        // allow concurrent access to MPU controller
    POWER_UP(7, SIM_SCGC7_FLEXBUS);                                      // ensure FlexBus is enabled
    POWER_UP(3, SIM_SCGC3_NFC);                                          // ensure NAND controller is enabled
    _CONFIG_PERIPHERAL(C, 16, (PC_16_NFC_RB | PORT_PS_UP_ENABLE));       // NFC_RB on PC16 (alt. function 6) with pull-up enabled
    _CONFIG_PERIPHERAL(C, 17, PC_17_NFC_CE0);                            // NFC_CE0 on PC17 (alt. function 6)
    _CONFIG_PERIPHERAL(C, 11, PC_11_NFC_WE);                             // NFC_WE on PC11 (alt. function 5)
    _CONFIG_PERIPHERAL(D, 10, PD_10_NFC_RE);                             // NFC_RE on PD10 (alt. function 6)
    _CONFIG_PERIPHERAL(D, 8,  PD_8_NFC_CLE);                             // NFC_CLE on PD8 (alt. function 6)
    _CONFIG_PERIPHERAL(D, 9,  PD_9_NFC_ALE);                             // NFC_ALE on PD9 (alt. function 6)
    _CONFIG_PERIPHERAL(D, 5,  PD_5_NFC_NFC_DATA0);                       // NFC_DATA0 on PD5 (alt. function 5)
    _CONFIG_PERIPHERAL(D, 4,  PD_4_NFC_NFC_DATA1);                       // NFC_DATA1 on PD4 (alt. function 5)
    _CONFIG_PERIPHERAL(C, 10, PC_10_NFC_NFC_DATA2);                      // NFC_DATA2 on PC10 (alt. function 5)
    _CONFIG_PERIPHERAL(C, 9,  PC_5_NFC_NFC_DATA3);                       // NFC_DATA3 on PC9 (alt. function 5)
    _CONFIG_PERIPHERAL(C, 8,  PC_5_NFC_NFC_DATA4);                       // NFC_DATA4 on PC8 (alt. function 5)
    _CONFIG_PERIPHERAL(C, 7,  PC_5_NFC_NFC_DATA5);                       // NFC_DATA5 on PC7 (alt. function 5)
    _CONFIG_PERIPHERAL(C, 6,  PC_5_NFC_NFC_DATA6);                       // NFC_DATA6 on PC6 (alt. function 5)
    _CONFIG_PERIPHERAL(C, 5,  PC_5_NFC_NFC_DATA7);                       // NFC_DATA7 on PC5 (alt. function 5)
    #if !defined MT29F1G08ABADAH4                                        // don't enable for 8-bit device
    _CONFIG_PERIPHERAL(C, 4,  PC_5_NFC_NFC_DATA8);                       // NFC_DATA8 on PC4 (alt. function 5)
    _CONFIG_PERIPHERAL(C, 2,  PC_5_NFC_NFC_DATA9);                       // NFC_DATA9 on PC2 (alt. function 5)
    _CONFIG_PERIPHERAL(C, 1,  PC_5_NFC_NFC_DATA10);                      // NFC_DATA10 on PC1 (alt. function 5)
    _CONFIG_PERIPHERAL(C, 0,  PC_5_NFC_NFC_DATA11);                      // NFC_DATA11 on PC0 (alt. function 5)
    _CONFIG_PERIPHERAL(B, 23, PB_5_NFC_NFC_DATA12);                      // NFC_DATA12 on PB23 (alt. function 5)
    _CONFIG_PERIPHERAL(B, 22, PB_5_NFC_NFC_DATA13);                      // NFC_DATA13 on PB22 (alt. function 5)
    _CONFIG_PERIPHERAL(B, 21, PB_5_NFC_NFC_DATA14);                      // NFC_DATA14 on PB21 (alt. function 5)
    _CONFIG_PERIPHERAL(B, 20, PB_5_NFC_NFC_DATA15);                      // NFC_DATA15 on PB20 (alt. function 5)
    NFC_CFG = (NFC_CFG_BITWIDTH_16 | NFC_CFG_FAST | NFC_CFG_STOPWERR | NFC_CFG_IDCNT_5 | (6 << 8) | 1);
    NFC_SECSZ = (NAND_PAGE_SIZE | (NAND_SPARE_SIZE + 1));
    #else
    NFC_CFG = (NFC_CFG_BITWIDTH_8 | NFC_CFG_FAST | NFC_CFG_STOPWERR | NFC_CFG_IDCNT_5 | (6 << 8) | 1);
    NFC_SECSZ = (NAND_PAGE_SIZE | NAND_SPARE_SIZE);
    #endif
    NFC_RAR = (NFC_RAR_CS0 | NFC_RAR_RB0);
    NFC_RAI = 0;
    NFC_SWAP = 0;
#endif

    fnMemoryDebugMsg("NAND Flash Device Initialized \n\r");

    fnCommandNAND(NAND_CMD_RESET, (NFC_CMD2_CODE_SEND_COMMAND_BYTE1_TO_FLASH | NFC_CMD2_CODE_WAIT_FOR_FLASH_R_B_HANDSHAKE)); // reset the NAND device (this takes maximum 5us when the device is not active)

    fnGetNAND_info(&ucDeviceInfo[0]);                                    // read information from the NAND flash
    fnMemoryDebugMsg("NAND Info\r\n");
    fnMemoryDebugMsg("Manufacturer = ");
    fnMemoryDebugHex(ucDeviceInfo[0], (WITH_LEADIN | WITH_CR_LF | sizeof(ucDeviceInfo[0])));
    fnMemoryDebugMsg("Device = ");
    fnMemoryDebugHex(ucDeviceInfo[1], (WITH_LEADIN | WITH_CR_LF | sizeof(ucDeviceInfo[1])));

    fnCreateBadBlocktable();                                             // check device integrity and create a bad block table

    if (fnInitialiseBlockMap(0) == NO_REMAP_TABLE_FOUND) {               // no table available
        if (fnInitialiseBlockMap(1) == REMAP_TABLE_FAILED_DUE_TO_BAD_NAND) { // so create a first one
            fnMemoryDebugMsg("NAND mapping table failure!!\r\n");
        }
        else {
            fnMemoryDebugMsg("Initial NAND address mapping table created\r\n"); // successfully created first remap table
        }
    }
}

// Read a user sector and check whether its content is deleted or not
//
static int fnCheckBlankPage(unsigned long ulSectorNumber)
{
    int iECC_result;
    unsigned long ulPageBuffer[NAND_PAGE_SIZE/sizeof(unsigned long)];
    int i = 0;
    if (ptrBlankBlocks[ulSectorNumber/NAND_PAGES_IN_BLOCK] != 0) {
        return 0;                                                        // if the block is known to be deleted all pages in it must be too
    }
    iECC_result = fnReadNANDsector(ulSectorNumber, 0, (unsigned char *)ulPageBuffer, NAND_PAGE_SIZE);
    while (i < NAND_PAGE_SIZE/sizeof(unsigned long)) {
        if (ulPageBuffer[i++] != 0xffffffff) {
            if (iECC_result == ECC_READ_ERROR) {                         // a blank page will not have a valid ECC so only report this when the page is not blank
                fnMemoryDebugMsg(" - ECC check failed\r\n");
            }
            return 1;                                                    // not blank page
        }
    }
    return 0;                                                            // blank
}

// Check all blocks in the user area and mark the ones that are blank
//
static int fnGetBlankBlocks(void)
{
    static unsigned short usBlankUserBlock = 0;
    int i, j;
    for (j = 0; j < 16; j++) {                                           // check up to 16 blocks at a time
        if (ptrBadBlocks[fnMapBlock(usBlankUserBlock)] == 0) {           // only check blocks that are not bad
            for (i = 0; i < NAND_PAGES_IN_BLOCK; i++) {                  // for each page in a block
                if (fnCheckBlankPage((usBlankUserBlock * NAND_PAGES_IN_BLOCK) + i) != 0) {
                    break;
                }
            }
            if (i == NAND_PAGES_IN_BLOCK) {                              // completely blank block found
                ptrBlankBlocks[usBlankUserBlock] = 1;                    // mark that this is a known fully deleted block
            }
        }
        if (++usBlankUserBlock >= MAX_USER_BLOCKS) {
            usBlankUserBlock = 0;                                        // reset in case the check is to be called again
            return 0;                                                    // completed check
        }
    }
    return 1;                                                            // not yet completed
}

// The content of an individual sector has to be modified.
// This is performed by moving all content to a new block and writing the new sector content
//
static int fnOverWriteSector(unsigned long ulSector, unsigned char *ptrData)
{
    int iECC_result;
    unsigned short usBlockToReplace = (unsigned short)(ulSector/NAND_PAGES_IN_BLOCK);
    unsigned char  ucPage = (unsigned char)(ulSector%NAND_PAGES_IN_BLOCK);
    unsigned long  ulPageBuffer[NAND_PAGE_SIZE/sizeof(unsigned long)];
    unsigned char  ucBlockPage = 0;
    unsigned short usSubstituteBlock = fnFindEmptyBlock();                // get an empty block to take over this block
    unsigned long  ulSubstituteSector;
    if (usSubstituteBlock == 0) {                                         // if we receive a value of zero (the partition) it means that there are no free blocks available
        return UTFAT_DISK_WRITE_ERROR;
    }
    ulSector = (usBlockToReplace * NAND_PAGES_IN_BLOCK);                  // first sector in the block
    ulSubstituteSector = (usSubstituteBlock * NAND_PAGES_IN_BLOCK);       // first sector in substitute block
    for (ucBlockPage = 0; ucBlockPage < NAND_PAGES_IN_BLOCK; ucBlockPage++) { // for each page in the block
        int i = 0;
        if (ucPage == ucBlockPage) {
            if (fnWriteNANDsector(ulSubstituteSector, 0, ptrData, NAND_PAGE_SIZE) != 0) {
                fnMemoryDebugMsg("Write error\r\n");
                return UTFAT_DISK_WRITE_ERROR;
            } 
        }
        else {
            iECC_result = fnReadNANDsector(ulSector, 0, (unsigned char *)ulPageBuffer, NAND_PAGE_SIZE);
            while (i < NAND_PAGE_SIZE/sizeof(unsigned long)) {
                if (ulPageBuffer[i++] != 0xffffffff) {
                    if (iECC_result == ECC_READ_ERROR) {                 // a blank page will not have a valid ECC so only report this when the page is not blank
                        fnMemoryDebugMsg(" - ECC check failed\r\n");
                    }
                    if (fnWriteNANDsector(ulSubstituteSector, 0, (unsigned char *)ulPageBuffer, NAND_PAGE_SIZE) != 0) {
                        fnMemoryDebugMsg("Write error\r\n");
                        return UTFAT_DISK_WRITE_ERROR;
                    } 
                    break;
                }
            }
        }
        ulSector++;
        ulSubstituteSector++;
    }
    fnExchangeBlock(usSubstituteBlock, usBlockToReplace);                // exchange the mapping information so that the user is mapped to the new block, deleting the old block
    return UTFAT_SUCCESS;
} 

// This routine prints details fo the user sector's physial remapping
//
extern void fnPrintSectorDetails(unsigned long ulUserSector)
{
    unsigned char ucPage = (unsigned char)(ulUserSector%NAND_PAGES_IN_BLOCK);
    fnDebugMsg("User sector ");
    fnDebugHex(ulUserSector, (sizeof(ulUserSector) | WITH_LEADIN));
    ulUserSector /= NAND_PAGES_IN_BLOCK;
    if (ulUserSector >= MAX_USER_BLOCKS) {                               // invalid to ignore
        fnDebugMsg(" doesn't exist\r\n");
        return;
    }
    fnDebugMsg(" (");
    fnDebugHex(ulUserSector, (sizeof(unsigned short) | WITH_LEADIN));
    fnDebugMsg("/");
    fnDebugHex(ucPage, (sizeof(ucPage) | WITH_LEADIN));
    fnDebugMsg(") is mapped to block ");
    fnDebugHex(ptrRemap[ulUserSector], (sizeof(ptrRemap[ulUserSector]) | WITH_LEADIN));
    fnDebugMsg(" [page=");
    fnDebugHex((unsigned long)((ptrRemap[ulUserSector]*NAND_PAGES_IN_BLOCK) + ucPage), (sizeof(unsigned long) | WITH_LEADIN));
    fnDebugMsg("]\r\n");
    fnDebugMsg("It is marked as ");
    if (ptrBlankBlocks[ulUserSector] != 0) {
        fnDebugMsg("BLANK");
    }
    else {
        fnDebugMsg("USED");
    }
    fnDebugMsg(" and has a delete count of ");
    fnDebugDec(ptrWearLevel[ulUserSector], (WITH_CR_LF));
}

// This routine is used only for development since it deletes the remapping information to start with fresh NAND chip contents
//
extern void fnDeleteRemapTable(void)
{
    unsigned short usBlockToErase;
    for (usBlockToErase = 0; usBlockToErase < REMAP_AREA_BLOCKS; usBlockToErase++) {
        _fnEraseNAND_block(usBlockToErase);
    }
    fnDebugMsg("Remapping info destroyed!!\r\n");
}

// This routine is used only for development since it deletes the FAT content (sectors 0..0x234)
//
extern void fnDeleteFAT(void)
{
    unsigned short usUserBlockToErase;
    for (usUserBlockToErase = 0; usUserBlockToErase < (0x234/32); usUserBlockToErase++) {
        _fnEraseNAND_block(fnMapBlock(usUserBlockToErase));
    }
    fnDebugMsg("FAT destroyed!!\r\n");

}

static void fnCleanNAND(void)
{
    int iDeleted = 0;
    unsigned short usSpareBlock = USER_AREA_BLOCKS;
    while (usSpareBlock < MAX_USER_BLOCKS) {
        if (ptrSpareDeletes[usSpareBlock - USER_AREA_BLOCKS] != 0) {      // a block in the spare area waiting to be deleted
            if (iDeleted != 0) {
                break;
            }
            if (_fnEraseNAND_block(ptrRemap[usSpareBlock]) == UTFAT_SUCCESS) { // erase the block
                ptrBlankBlocks[usSpareBlock] = 1;                        // mark that the spare block is now blank so that it can be used again
            }
            ptrSpareDeletes[usSpareBlock - USER_AREA_BLOCKS] = 0;
            iDeleted = 1;
        }
        usSpareBlock++;
    }
    if (usSpareBlock < MAX_USER_BLOCKS) {                                // a block was deleted but there were more found waiting
        uTaskerMonoTimer(OWN_TASK, T_CLEAN_SPARE, E_CLEAN_SPARE);        // start the next operation after a further delay
        return;
    }
    #ifdef VERIFY_NAND
    fnDebugMsg("NAND cleaned\r\n");
    #endif
}

#endif
