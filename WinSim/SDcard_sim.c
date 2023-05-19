/**********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher

    ---------------------------------------------------------------------
    File:      SDcard_sim.c
    Project:   uTasker project
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2016
    *********************************************************************
    19.06.2010 Rename fnWriteSector() to fnWriteSDSector() to avoid confilict with new external function with same name {1}
    20.07.2010 Add SD card controller interface                          {2}
    20.02.2012 Add SD card size configuration - 1G..16G                  {3}
    08.03.2012 Add dynamic SD card insertion / remove                    {4}
    07.02.2014 Add application CMD55 checking, pre-delete, read and write multiple sector commands {5}
    21.11.2015 Memory stick simulation added                             {6}
    21.11.2015 Enable FAST_SD_SIMULATION for faster disk simulation      {7}

*/        


#include "config.h"

#if defined SDCARD_SUPPORT || defined USB_MSD_HOST

#include "conio.h"
#include "Fcntl.h"
#include "io.h"

#include <sys/stat.h>

#if _VC80_UPGRADE>=0x0600
    #include <share.h>
#endif

#define FAST_SD_SIMULATION                                               // {7} use new method whih increases file access speed
//#define _SIMPLE_FIXED                                                  // simple simulation of a fixed card
    #define _PARTITIONED                                                 // simple fixed card is partitioned

#if SDCARD_SIM_SIZE == SDCARD_SIZE_1G                                    // {3}
    #define OFFSET_CONVERSION 512
    static unsigned char ucCardDetails_CSD[18] = {                       // 1G card
        0xff, 0xfe,
        0x00, 0x4f, 0x00, 0x32, 0x5f, 0x59, 0xa3, 0xcb, 0xff, 0xff, 0xff, 0x8f, 0x8a, 0x40, 0x00, 0x29
    };
#elif SDCARD_SIM_SIZE == SDCARD_SIZE_2G
    #define OFFSET_CONVERSION 512
    static unsigned char ucCardDetails_CSD[18] = {                       // 2G card
        0xff, 0xfe,
        0x00, 0x2e, 0x00, 0x32, 0x5b, 0x5a, 0xa3, 0xb4, 0xff, 0xff, 0xff, 0x80, 0x0a, 0x80, 0x00, 0x8d//, 0xd5
      //0x00, 0x26, 0x00, 0x32, 0x5f, 0x5a, 0x83, 0xae, 0xfe, 0xfb, 0xcf, 0xff, 0x92, 0x80, 0x40, 0xdf
    };
#elif SDCARD_SIM_SIZE == SDCARD_SIZE_4G
    #define _HC_SD_CARD
    #define OFFSET_CONVERSION 1
    static unsigned char ucCardDetails_CSD[18] = {                       // 4G card
        0xff, 0xfe,
        0x40, 0x0e, 0x00, 0x32, 0x5b, 0x59, 0x00, 0x00, 0x1e, 0xb9, 0x7f, 0x80, 0x0a, 0x40, 0x00, 0xc7
    };
#elif SDCARD_SIM_SIZE == SDCARD_SIZE_8G
    #define _HC_SD_CARD
    #define OFFSET_CONVERSION 1
    static unsigned char ucCardDetails_CSD[18] = {                       // 8G card
        0xff, 0xfe,
        0x40, 0x0e, 0x00, 0x32, 0x5b, 0x59, 0x00, 0x00, 0x3b, 0x37, 0x7f, 0x80, 0x0a, 0x40, 0x40, 0xaf
    };
#else
    #define _HC_SD_CARD
    #define OFFSET_CONVERSION 1
    static unsigned char ucCardDetails_CSD[18] = {                       // 16G card
        0xff, 0xfe,
        0x40, 0x0e, 0x00, 0x32, 0x5b, 0x59, 0x00, 0x00, 0x76, 0xb2, 0x7f, 0x80, 0x0a, 0x40, 0x40, 0x13
    };
#endif

static unsigned char ucCardInfo[18] = {
    0xff, 0xfe,
    0x40, 0x0e, 0x00, 0x32, 0x5b, 0x58, 0x00, 0x00, 0x1d, 0x7b, 0x7f, 0x80, 0x6a, 0x40, 0x00, 0x91
};

static unsigned char ucCardRCA[4] = {
    0x12, 0x34,
    0x05, 0x00
};

static unsigned char ucCardSelected[4] = {
    0x12, 0x34,
    0x05, 0x00
};

static const EXTENDED_BOOT_RECORD SD_Card_partition = {
    {0},
    {0},
    {0},
    {
        { 0, 2, 0xc, 0, 0xb, 0x38, 0xf8, 0xb8, {LITTLE_LONG_WORD_BYTES(0x89)}, {LITTLE_LONG_WORD_BYTES(0x003a9f77)}},
        {0}
    },
    {0},
    0x55,
    0xaa
};

static const BOOT_SECTOR_FAT32 SD_Card_boot_sector = {                   // 2G reference
    {
        {0xeb, 0x58, 0x90},
        {'M', 'S', 'D', 'O', 'S', '5', '.', '0'},
        {LITTLE_SHORT_WORD_BYTES(512)},                                  // 512 bytes per sector
        0x01,                                                            // sectors per cluster
        {LITTLE_SHORT_WORD_BYTES(6530)},                                 // number of reserved sectors in the Reserved region of the volume
        2,                                                               // number of FATs
        {LITTLE_SHORT_WORD_BYTES(0)},
        {LITTLE_SHORT_WORD_BYTES(0)},
        0xf8,                                                            // media
        {LITTLE_SHORT_WORD_BYTES(0)},
        {0xf3, 0x00},
        {0xff, 0x00},
        {0x89, 0x00, 0x00, 0x00},
        {LITTLE_LONG_WORD_BYTES(3841911)},
    },
    {LITTLE_LONG_WORD_BYTES(29503)},                                     // sectors per FAT
    {LITTLE_SHORT_WORD_BYTES(0)},
    {LITTLE_SHORT_WORD_BYTES(0)},
    {LITTLE_LONG_WORD_BYTES(2)},
    {LITTLE_SHORT_WORD_BYTES(0x01)},                                     // location of info section
    {LITTLE_SHORT_WORD_BYTES(6)},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    0x80,
    0, 
    0x29,
    {0xf0, 0x7e, 0xde, 0x0e},
    {'N', 'O', ' ', 'N', 'A', 'M', 'E', ' ', ' ', ' ', ' '},
    {'F', 'A', 'T', '3', '2', ' ', ' ', ' '},
    {0},                                                                 // this tends not be be filled with zeros
    0x55,
    0xaa
};

static const INFO_SECTOR_FAT32 SD_Card_info_sector = {
    {LITTLE_LONG_WORD_BYTES(0x41615252)},
    {0},
    {LITTLE_LONG_WORD_BYTES(0x61417272)},
    {LITTLE_LONG_WORD_BYTES(0x00399f76)},
    {LITTLE_LONG_WORD_BYTES(0x00000003)},
    {0},
    {0x00, 0x00, 0x55, 0xAA},
};

static const DIR_ENTRY_STRUCTURE_FAT32 SD_Card_volume[16] = {
    {
        {'V', 'O', 'L', 'U', 'M', 'E', ' ', 'I', 'D', 0, 0},
        0x08,                                                            // volume ID attribute
        0, 0, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}
    },
    {
        {'T', 'E', 'S', 'T', '1', ' ', ' ', ' ', 'T', 'X', 'T'},
        0x20,                                                            // archive attribute
        0, 0x24, {0xe7, 0xb3}, {0x77, 0x3b}, {0x77, 0x3b}, {0}, {0xee, 0xb3}, {0x77, 0x3b}, {0x03, 0x00}, {0x09, 0, 0, 0}
    },
    {
        {'D', 'I', 'R', '_', '1', ' ', ' ', ' ', ' ', ' ', ' '},
        0x10,                                                            // directory attribute
        0, 0x2f, {0xf0, 0xb3}, {0x77, 0x3b}, {0x77, 0x3b}, {0, 0}, {0xf1, 0xb3}, {0x77, 0x3b}, {0x04, 0x00}, {0x09, 0, 0, 0}
    },
    {
        {0x42, 0x68, 0, 0x69, 0, 0x63, 0, 0x5f, 0, 0x32, 0},             // long file name 2nd and last
        0x0f,                                                            // LFN
        0, 0xa6,                                                         // checksum
        {0x2e, 0}, {0x6a, 0}, {0x70, 0}, {0x65, 0}, {0x67, 0}, {0, 0}, {0, 0}, {0xff, 0xff, 0xff, 0xff}
    },
    {
        {0x01, 0x43, 0, 0x6f, 0, 0x6c, 0, 0x64, 0, 0x46, 0},             // long file name 1st
        0x0f,                                                            // LFN
        0, 0xa6,                                                         // checksum
        {0x69, 0}, {0x72, 0}, {0x65, 0}, {0x5f, 0}, {0x47, 0}, {0x72, 0}, {0, 0}, {0x61, 0x00, 0x70, 0x00}
    },
    {
        {'C', 'O', 'L', 'D', 'F', 'I', '~', '1', 'J', 'P', 'E'},         // ColdFire_Graphic_2.jpeg as short file name 
        0x20,                                                            // archive attribute
        0, 0xc7, {0x6d, 0x88}, {0x7d, 0x3b}, {0x7d, 0x3b}, {0}, {0x53, 0x8a}, {0x0c, 0x39}, {0x12, 0x00}, {0x87, 0xdf, 0x04, 0x00}
    },
    {
        {0x41, 0x75, 0, 0x54, 0, 0x61, 0, 0x73, 0, 0x6b, 0},             // long file name 1st and last
        0x0f,                                                            // LFN
        0, 0xb0,                                                         // checksum
        {0x65, 0}, {0x72, 0}, {0x57, 0}, {0x65, 0}, {0x62, 0}, {0x00, 0x00}, {0, 0}, {0xff, 0xff, 0xff, 0xff}
    },
    {
        {'U', 'T', 'A', 'S', 'K', 'E', '~', '1', ' ', ' ', ' '},         // uTaskerweb directory as short file name 
        0x10,                                                            // directory attribute
        0, 0xb0, {0x6d, 0x88}, {0x7d, 0x3b}, {0x7d, 0x3b}, {0}, {0xbd, 0x71}, {0x7c, 0x3b}, {0x06, 0x00}, {0, 0, 0, 0}
    },
    {0},
    {0},
    {0},
    {0},
    {0},
    {0},
    {0},
    {0}
};

static const DIR_ENTRY_STRUCTURE_FAT32 SD_Card_cluster_4[16] = {
    {0},
    {0},
    {
        {'D', 'E', 'M', 'O', 'S', ' ', ' ', ' ', ' ', ' ', ' '},         // uTaskerweb directory as short file name 
        0x10,                                                            // directory attribute
        0, 0x24, {0x6f, 0x88}, {0x7d, 0x3b}, {0x7d, 0x3b}, {0}, {0xb0, 0x79}, {0x42, 0x3b}, {0x6f, 0x00}, {0, 0, 0, 0}
    },
    {0},
    {0},
    {0},
    {0},
    {0},
    {0},
    {0},
    {0},
    {0},
    {0},
    {0},
    {0},
    {0}
};

static const DIR_ENTRY_STRUCTURE_FAT32 SD_Card_files[16] = {
    {0},
    {0},
    {                                                                    // always start at index 2 (why???)
        {0xe5, 'O', 'L', 'U', 'M', 'E', ' ', 'I', 'D', 0, 0},            // deleted entry
        0x08,
        0, 0, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}
    },
    {
        {'T', 'E', 'S', 'T', '2', ' ', ' ', ' ', 'T', 'X', 'T'},
        0x20,                                                            // archive attribute
        0x18, 
        0x49, {0xf6, 0xb3}, {0x77, 0x3b}, {0x77, 0x3b}, {0, 0}, {0x01, 0xb3}, {0x77, 0x3b}, {0x05, 0x00}, {LITTLE_LONG_WORD_BYTES(17)}
    },
    {0},
    {0},
    {0},
    {0},
    {0},
    {0},
    {0},
    {0},
    {0},
    {0},
    {0},
    {0}
};

unsigned char SD_Card_file_1[512] = {
    'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q',   0
};

unsigned char SD_Card_file_2[512] = {
    'z', 'y', 'x', 'w', 'v', 'u', 't', 's', 'r', 'q', 'p', 'o', 'n', 'm', 'l', 'k', 'j',   0
};

static int iSD_card_file = -1;
#if defined USB_MSD_HOST
    static int iMemStick_file[8] = {-1, -1, -1, -1, -1, -1, -1, -1};
#endif

#if defined FAST_SD_SIMULATION                                           // {7}
static int _fnSimSD_findSector(unsigned long sectorNum, unsigned char ucLun, unsigned char *sectorBuf)
{
    int iFile;
    unsigned char buffer[(512 + sizeof(unsigned long)) * 100];           // buffer to use when reading data
    if (sectorNum == 0) {                                                // reading the boot sector
        if (ucLun == 0xff) {                                             // SD card
            if (iSD_card_file < 0) {                                     // first time the boot sector is read
    #if _VC80_UPGRADE < 0x0600
	            iSD_card_file = _open("SD_CARD.bin", (_O_BINARY  | _O_CREAT | _O_RDWR ), _S_IREAD | _S_IWRITE ); 
    #else
	            _sopen_s(&iSD_card_file, "SD_CARD.bin", (_O_BINARY  | _O_CREAT | _O_RDWR), _SH_DENYNO, _S_IREAD | _S_IWRITE); 
    #endif
            }
            iFile = iSD_card_file;
        }
    #if defined USB_MSD_HOST
        else {
            if (iMemStick_file[ucLun] < 0) {                             // first time the boot sector is read
                char MemStickFile[] = {'M', 'E', 'M', 'S', 'T', 'I', 'C', 'K', '0', '.', 'b', 'i', 'n', 0};
                MemStickFile[8] += ucLun;
    #if _VC80_UPGRADE < 0x0600
	            iMemStick_file[ucLun] = _open((const char *)MemStickFile, (_O_BINARY  | _O_CREAT | _O_RDWR ), _S_IREAD | _S_IWRITE ); 
    #else
	            _sopen_s(&iMemStick_file[ucLun], (const char *)MemStickFile, (_O_BINARY  | _O_CREAT | _O_RDWR), _SH_DENYNO, _S_IREAD | _S_IWRITE); 
    #endif
            }
            iFile = iMemStick_file[ucLun];
        }
    #endif
        _lseek(iFile, 0, SEEK_SET);                                      // set to start of file
        if (sectorBuf != 0) {
            if (_read(iFile, sectorBuf, 512) < 512) {                    // read the partition sector which is always the first one
                memset(sectorBuf, 0, 512);
            }
        }
        _lseek(iFile, 0, SEEK_SET);
        return 1;                                                        // always found
    }
    else {
    #if defined SDCARD_SIM_DIRECT
        // Check to see how large the file is
        //
        long long seekPos = (sectorNum * 512LL);
        long long curPos;
        if (ucLun == 0xff) {
            iFile = iSD_card_file;
        }
        #if defined USB_MSD_HOST
        else {
            iFile = iMemStick_file[ucLun];
        }
        #endif
        curPos = _lseeki64(iFile, seekPos, SEEK_SET);
        if (curPos < seekPos) {
            memset(buffer, 0, sizeof(buffer));// write zeroes to get to the right position
            do {
                int nBytesToWrite = sizeof(buffer);
                if ((curPos + nBytesToWrite) > seekPos) {
                    nBytesToWrite = (int)(seekPos - curPos);
                }
                _write(iFile, buffer, nBytesToWrite);
                curPos += nBytesToWrite;
            } while(curPos < seekPos);
            if (sectorBuf != 0) {
                memset(sectorBuf, 0, 512);
            }
            return 1;                                                    // always found
        }
        if (sectorBuf != 0) {
            if (_read(iFile, sectorBuf, 512) < 512) {
                memset(sectorBuf, 0, 512);
            }
            _lseeki64(iFile, seekPos, SEEK_SET);
        }
        return 1;                                                        // always found
    #else
        unsigned long ulSectorFromFile;
        if (ucLun == 0xff) {
            iFile = iSD_card_file;
        }
        #if defined USB_MSD_HOST
        else {
            iFile = iMemStick_file[ucLun];
        }
        #endif
        _lseeki64(iFile, 512, SEEK_SET);                                 // set to first numbered sector
        while (1) {
            int pos = 0;
            int bytesRead = _read(iFile, buffer, sizeof(buffer));
            if (bytesRead == 0) {
                // Couldn't find the sector - set to all 0 bytes, and return with the file pointer at the end of the file
                // 
                if (sectorBuf != 0) {
                    memset(sectorBuf, 0, 512);
                }
                return 0;
            }
            while ((bytesRead - pos) >= (512 + sizeof(unsigned long))) {
                memcpy(&ulSectorFromFile, (buffer + pos), sizeof(unsigned long));
                if (ulSectorFromFile == sectorNum) {
                    _lseeki64(iFile, (pos - bytesRead), SEEK_CUR); // seek to point in file that contains sector number
                    if (sectorBuf != 0) {
                        memcpy(sectorBuf, buffer + pos + sizeof(unsigned long), 512);
                    }
                     return 1;
                }
                pos += 512 + sizeof(unsigned long);
            }
        }
    #endif
    }
}

static void fnExtractSector(unsigned long ulSectorNumber, unsigned char *ptrBuffer)
{
   _fnSimSD_findSector(ulSectorNumber, 0xff, ptrBuffer);
}

static void fnWriteSDSector(unsigned long ulSectorNumber, unsigned char ucLUN, unsigned char *ptrBuffer) // {1}
{
    int found = _fnSimSD_findSector(ulSectorNumber, ucLUN, NULL);
    int iFile;
    if (ucLUN == 0xff) {
        iFile = iSD_card_file;
    }
    #if defined USB_MSD_HOST
    else {
        iFile = iMemStick_file[ucLUN];
    }
    #endif
    #if defined SDCARD_SIM_DIRECT
    _write(iFile, ptrBuffer, 512);
    #else
    if (found == 0) {
        int i;
        for (i = 0; i < 512; ++i) {                                      // see if we need to write it
            if (ptrBuffer[i] != 0) {
                found = 1;
                break;
            }
        }
    }
    // Now, write the data if necessary
    //
    if (found != 0) {
        if (ulSectorNumber == 0) {
            _write(iFile, ptrBuffer, 512);                               // just write the data for sector 0
        }
        else {
            unsigned char buf[512 + sizeof(unsigned long)];
            memcpy(buf, &ulSectorNumber, sizeof(unsigned long));
            memcpy(buf + sizeof(unsigned long), ptrBuffer, 512);
            _write(iFile, buf, sizeof(buf));
        }
    }
    #endif
}
#else
static void fnExtractSector(unsigned long ulSectorNumber, unsigned char *ptrBuffer)
{
    if (ulSectorNumber == 0) {
        if (iSD_card_file < 0) {                                         // first time the boot sector is read
    #if _VC80_UPGRADE < 0x0600
	        iSD_card_file = _open("SD_CARD.bin", (_O_BINARY  | _O_CREAT | _O_RDWR ), _S_IREAD | _S_IWRITE ); 
    #else
	        _sopen_s(&iSD_card_file, "SD_CARD.bin", (_O_BINARY  | _O_CREAT | _O_RDWR), _SH_DENYNO, _S_IREAD | _S_IWRITE); 
    #endif
        }
        _lseek(iSD_card_file, 0, SEEK_SET);                              // set to start of file
        if (_read(iSD_card_file, ptrBuffer, 512) < 512) {                // read the partition sector which is always the first one
            memset(ptrBuffer, 0, 512);
/*
            _lseek(iSD_card_file, 0, SEEK_SET);

    #if defined _PARTITIONED
            _write(iSD_card_file, &SD_Card_partition, 512);
            memcpy(ptrBuffer, &SD_Card_partition, 512);                  // partition sector content
            _write(iSD_card_file, &SD_Card_partition.EBR_partition_table->start_sector[0], sizeof(unsigned long));
    #endif
            _write(iSD_card_file, &SD_Card_boot_sector, 512);            // boot sector content
            */
        }
    }
    else {
        unsigned long ulSectorFromFile;
        _lseek(iSD_card_file, 512, SEEK_SET);                            // set to first numbered sector
        while (1) {
            if (_read(iSD_card_file, &ulSectorFromFile, sizeof(unsigned long)) < sizeof(unsigned long)) { // read the sector number
                memset(ptrBuffer, 0, 512);                               // empty sector
                return;
            }
            if (ulSectorNumber == ulSectorFromFile) {
                _read(iSD_card_file, ptrBuffer, 512);                    // read the sector content
                return;
            }
            _lseek(iSD_card_file, 512, SEEK_CUR);                        // move to next numbered sector
        }
    }
}

static void fnWriteSDSector(unsigned long ulSectorNumber, unsigned char ucLUN, unsigned char *ptrBuffer) // {1}
{
    if (ulSectorNumber == 0) {                                           // writing to partition sector - this is a special case because it causes a new file to be created (old file's contents are destroyed)
    #if _VC80_UPGRADE < 0x0600
	    iSD_card_file = _open("SD_CARD.bin", (_O_BINARY | _O_TRUNC | _O_CREAT | _O_RDWR ), _S_IREAD | _S_IWRITE ); 
    #else
	    _sopen_s(&iSD_card_file, "SD_CARD.bin", (_O_BINARY | _O_TRUNC | _O_CREAT | _O_RDWR), _SH_DENYNO, _S_IREAD | _S_IWRITE); 
    #endif
        _lseek(iSD_card_file, 0, SEEK_SET);                              // set to start of file
    }
    else {
        unsigned long ulSectorFromFile;
        _lseek(iSD_card_file, 512, SEEK_SET);                            // set to first numbered sector
        while (1) {
            if (_read(iSD_card_file, &ulSectorFromFile, sizeof(unsigned long)) < sizeof(unsigned long)) { // read the sector number
                int iLength = 512;
                int iFound = 0;
                unsigned char *ptrTest = ptrBuffer;
                while (iLength--) {
                    if (*ptrTest++ != 0) {
                        iFound = 1;
                        break;
                    }
                }
                if (iFound == 0) {
                    return;                                              // don't write zeroed sectors
                }
                _write(iSD_card_file, &ulSectorNumber, sizeof(ulSectorNumber)); // sector not found so create 
                break;
            }
            if (ulSectorNumber == ulSectorFromFile) {                    // found existing sector content
                break;
            }
            _lseek(iSD_card_file, 512, SEEK_CUR);                        // move to next numbered sector
        }
    }
    _write(iSD_card_file, ptrBuffer, 512);                               // write
}
#endif


extern void fnSaveSDcard(void)
{
    #if defined USB_MSD_HOST
    int iFileRef = 0;
    while (iFileRef < 8) {
        if (iMemStick_file[iFileRef] >= 0) {
            _close(iMemStick_file[iFileRef]);
        }
        iFileRef++;
    }
    #endif
    if (iSD_card_file >= 0) {
        _close(iSD_card_file);
    }
}

    #if defined SDCARD_SUPPORT 
// Simulate SD card in SPI/SDHC modes
//
extern unsigned char _fnSimSD_write(unsigned char ucTxByte)
{
#define SD_CARD_START                  0
#define SD_CARD_INIT                   1
#define SD_CARD_ARGUMENTS              2
#define SD_CARD_CRC                    3
#define SD_CARD_ANSWER                 4
#define SD_CARD_ANSWER_EXTRA           5
#define SD_CARD_BLOCK_WRITE            6
#define SD_CARD_BLOCK_WRITING          7
#define SD_CARD_BLOCK_COLLECTING       8
#define SD_CARD_RX_CRC_1               9
#define SD_CARD_RX_CRC_2               10
#define SD_CARD_RX_CRC_3               11
#define SD_CARD_ANSWER_APP             12                                // {5}
#define SD_CARD_APP_CMD                13
#define SD_CARD_MULTI_BLOCK_WRITE      14
#define SD_CARD_MULTI_BLOCK_WRITING    15
#define SD_CARD_MULTI_BLOCK_COLLECTING 16

#if defined SD_CONTROLLER_AVAILABLE                                      // {2}
    static int iSD_Card_State = SD_CARD_INIT;
#else
    static int iSD_Card_State = SD_CARD_START;
#endif
    static int iMultiBlockWriteActive = 0;
    static int iMultiBlockReadActive = 0;
    static unsigned long ulPreDeleteLength = 1;                          // {5} default length of a delete when writing
    static unsigned long ulWriteOffset;
    static int iArguments = 0;
    static int iSendArg = 0;
    static unsigned char ucCommand = 0;
    static unsigned char ucAnswer = 0xff;
    static unsigned char ucArguments[515];
#if !defined NAND_FLASH_FAT
    if ((SD_card_state(0, 0) & 0x00000001) == 0) {                       // {4} if SD card is not inserted don't answer
        return 0xff;
    }
#endif
    switch (iSD_Card_State) {
    case SD_CARD_START:                                                  // with DI and CS held high it is expected that >= 74 clocks are received to enter native command mode
        {
            static int iStartUp = 0;
            if (++iStartUp >= 10) {                                      // count 10 bytes
                iSD_Card_State = SD_CARD_INIT;
            }
        }
        break;

    case SD_CARD_INIT:
        if ((ucTxByte == 0x69) || (ucTxByte == 0x46) || (ucTxByte == 0x57)) { // {5} commands requiring application command CMD55 to be sent as pre-sequence
            _EXCEPTION("Command requires pre-CMD55!!");
        }
    case SD_CARD_APP_CMD:
        if ((ucTxByte == 0x40) || (ucTxByte == 0x42) || (ucTxByte == 0x43) || (ucTxByte == 0x46) || (ucTxByte == 0x47) || (ucTxByte == 0x48) || (ucTxByte == 0x4c) || (ucTxByte == 0x50) || (ucTxByte == 0x57) || (ucTxByte == 0x77) || (ucTxByte == 0x69) || (ucTxByte == 0x7a) || (ucTxByte == 0x58) || (ucTxByte == 0x51) || (ucTxByte == 0x49) || (ucTxByte == 0x59)) { // all expected commands
            ucCommand = ucTxByte;
            iArguments = 0;
            iSD_Card_State = SD_CARD_ARGUMENTS;
            break;
        }
        else if ((ucTxByte == 0xfe) && (iMultiBlockWriteActive != 0)) {  // used to detect that a sector write is being started
            // Accept a sector write when performing multiple block writes
            //
            iSD_Card_State = SD_CARD_MULTI_BLOCK_COLLECTING;
        }
        else if (ucTxByte != 0xff) {                                     // accept idle line, which is used when poll for busy
            _EXCEPTION("Neither command not idle - program error!!");
        }
        break;
    case SD_CARD_ARGUMENTS:
        ucArguments[iArguments++] = ucTxByte;
        if (iArguments == 4) {
            iSD_Card_State = SD_CARD_CRC;
            break;
        }
        break;
    case SD_CARD_CRC:
        if (ucCommand == 0x51) {                                         // single block read
            unsigned long ulByteOffset = ((ucArguments[0] << 24) + (ucArguments[1] << 16) + (ucArguments[2] << 8) + ucArguments[3]);
            iSD_Card_State = SD_CARD_ANSWER;
            iArguments = 515;                                            // a block answer is expected
            ucArguments[0] = 0xfe;
            iSendArg = 0;
#if defined _SIMPLE_FIXED
    #if defined _PARTITIONED
            if (ulByteOffset == 0) {
                memcpy(&ucArguments[1], &SD_Card_partition, 512);        // boot sector content
            }
            else if (ulByteOffset == (0x8a * OFFSET_CONVERSION)) {       // info sector 
                memcpy(&ucArguments[1], &SD_Card_info_sector, 512);
            }
            else if (ulByteOffset == (0x10089 * OFFSET_CONVERSION)) {    // volume directory
                memcpy(&ucArguments[1], &SD_Card_volume[0], 512);
            }
            else if (ulByteOffset == (0x1008b * OFFSET_CONVERSION)) {    // file
                memcpy(&ucArguments[1], &SD_Card_files[0], 512);
            }
            else if (ulByteOffset == (0x1008a * OFFSET_CONVERSION)) {    // file content
                memcpy(&ucArguments[1], &SD_Card_file_1[0], 512);
            }
            else if (ulByteOffset == (0x1008d * OFFSET_CONVERSION)) {    // file content
                memcpy(&ucArguments[1], &SD_Card_cluster_4[0], 512);
            }
            else if (ulByteOffset == (0x1008c * OFFSET_CONVERSION)) {    // file content
                memcpy(&ucArguments[1], &SD_Card_file_2[0], 512);
            }
            else if (ulByteOffset == (0x89 * OFFSET_CONVERSION)) {
                memcpy(&ucArguments[1], &SD_Card_boot_sector, 512);      // boot sector content
            }
    #else
            if (ulByteOffset == 0) {
                memcpy(&ucArguments[1], &SD_Card_boot_sector, 512);      // boot sector content
            }
            else if (ulByteOffset == (1 * OFFSET_CONVERSION)) {          // info sector 
                memcpy(&ucArguments[1], &SD_Card_info_sector, 512);      // boot sector content
            }
    #endif
            else {
                memset(&ucArguments[1], 0x00, 512);
            }
#else
            fnExtractSector((ulByteOffset / OFFSET_CONVERSION), &ucArguments[1]);
#endif
            ucAnswer = 0x00;                                             // in idle state
        }
#if defined SD_CONTROLLER_AVAILABLE
        else if (ucCommand == 0x50) {                                    // set block length
            iSD_Card_State = SD_CARD_ANSWER;
            iArguments = 4;
            iSendArg = 0;
            memcpy(&ucArguments[0], ucCardSelected, 4);
            ucAnswer = 0x00;                                             // in idle state
        }
        else if (ucCommand == 0x47) {                                    // select card
            iSD_Card_State = SD_CARD_ANSWER;
            iArguments = 4;
            iSendArg = 0;
            memcpy(&ucArguments[0], ucCardSelected, 4);
            ucAnswer = 0x00;                                             // in idle state
        }
        else if (ucCommand == 0x46) {                                    // set interface width
            iSD_Card_State = SD_CARD_ANSWER;
            iArguments = 4;
            iSendArg = 0;
            memcpy(&ucArguments[0], ucCardSelected, 4);
            ucAnswer = 0x00;                                             // in idle state
        }
        else if (ucCommand == 0x43) {                                    // request RCA address
            iSD_Card_State = SD_CARD_ANSWER;
            iArguments = 4;
            iSendArg = 0;
            memcpy(&ucArguments[0], ucCardRCA, 4);
            ucAnswer = 0x00;                                             // in idle state
        }
        else if (ucCommand == 0x69) {                                    // read OCR
            iSD_Card_State = SD_CARD_ANSWER;            
            iArguments = 4;
            iSendArg = 0;
            ucArguments[0] |= 0x80;                                      // set the not-busy bit and echo back the arguments
    #if defined _HC_SD_CARD
            ucArguments[0] |= 0x40;                                      // high capacity
    #else
            ucArguments[0] &= ~0x40;                                     // not high capacity
    #endif
            ucAnswer = 0;
        }
#endif
        else if (ucCommand == 0x57) {                                    // {5} pre-delete command
            ulPreDeleteLength = ((ucArguments[0] << 24) + (ucArguments[1] << 16) + (ucArguments[2] << 8) + ucArguments[3]); // a multiple write is expected to follow
            if (ulPreDeleteLength == 0) {
                ulPreDeleteLength = 0;
            }
            iSD_Card_State = SD_CARD_ANSWER;
            iArguments = 0;
            ucArguments[0] = 0xfe;
            ucAnswer = 0;
        }
        else if (ucCommand == 0x58) {                                    // single block write
            ulWriteOffset = ((ucArguments[0] << 24) + (ucArguments[1] << 16) + (ucArguments[2] << 8) + ucArguments[3]);
            iSD_Card_State = SD_CARD_BLOCK_WRITE;                        // a block write is expected to follow
            iArguments = 0;
            ucArguments[0] = 0xfe;
            ucAnswer = 0;
        }
        else if (ucCommand == 0x59) {                                    // multiple block write
            ulWriteOffset = ((ucArguments[0] << 24) + (ucArguments[1] << 16) + (ucArguments[2] << 8) + ucArguments[3]);
            iSD_Card_State = SD_CARD_MULTI_BLOCK_WRITE;                  // multiple block writes are expected to follow
            iArguments = 0;
            ucArguments[0] = 0xfe;
            ucAnswer = 0;
        }
        else if (ucCommand == 0x4c) {                                    // {5} terminate multiple block operation
            iSD_Card_State = SD_CARD_ANSWER;
            iArguments = 0;
            ucArguments[0] = 0xfe;
            ucAnswer = 0;
            iMultiBlockWriteActive = iMultiBlockReadActive = 0;          // multiple block operation temrinated
        }
        else if (ucCommand == 0x42) {                                    // read card information
            iSD_Card_State = SD_CARD_ANSWER;
#if defined SD_CONTROLLER_AVAILABLE
            iArguments = 16;
#else
            iArguments = 21;
#endif
            iSendArg = 0;
            memcpy(&ucArguments[0], ucCardInfo, 18);
            ucArguments[17] = 0x55;                                      // dummy CRC
            ucArguments[18] = 0xaa;
            ucAnswer = 0x00;                                             // in idle state
        }
        else if (ucCommand == 0x49) {                                    // read card specific data
            iSD_Card_State = SD_CARD_ANSWER;
#if defined SD_CONTROLLER_AVAILABLE
            iArguments = 16;
#else
            iArguments = 21;
#endif
            iSendArg = 0;
            memcpy(&ucArguments[0], ucCardDetails_CSD, 18);
          //ucArguments[17] = 0x55;                                      // dummy CRC
            ucArguments[18] = 0xaa;
            ucAnswer = 0x00;                                             // in idle state
        }
        else if (ucCommand == 0x40) {
#if defined SD_CONTROLLER_AVAILABLE                                      // {2}
            ucTxByte = 0x95;
#endif
            if (ucTxByte == 0x95) {                                      // check that the CRC is as expected
                iSD_Card_State = SD_CARD_ANSWER;
                iArguments = 0;
                ucAnswer = 0x01;                                         // in idle state
            }
        }
        else if (ucCommand == 0x48) {
#if defined SD_CONTROLLER_AVAILABLE                                      // {2}
            ucTxByte = 0x87;
#endif
            if (ucTxByte == 0x87) {                                      // check that the CRC is as expected
                iSD_Card_State = SD_CARD_ANSWER;
              //ucAnswer = 0x00;                                         // SD version 1 or MMC
                ucAnswer = 0x01;                                         // SD version 2+
                iArguments = 4;                                          // 4 additional bytes of data
                iSendArg = 0;
                ucArguments[0] = 0;                                      // the card's characteristics
                ucArguments[1] = 0;
                ucArguments[2] = 0x01;
                ucArguments[3] = 0xaa;
#if defined SD_CONTROLLER_AVAILABLE                                      // {2}
                ucArguments[4] = 0x01;
                ucArguments[5] = 0xaa;
                iArguments = 6;
#endif
            }
        }
        else if (ucCommand == 0x77) {                                    // application CMD55 - no CRC
            iSD_Card_State = SD_CARD_ANSWER_APP;
            ucAnswer = 0x00;
            iArguments = 0;                                              // no additional bytes of data
        }
        else if (ucCommand == 0x69) {                                    // no CRC
            iSD_Card_State = SD_CARD_ANSWER;
            ucAnswer = 0x00;
            iArguments = 0;                                              // no additional bytes of data
        }
        else if (ucCommand == 0x7a) {
            iSD_Card_State = SD_CARD_ANSWER;
            ucAnswer = 0x00;
            iArguments = 4;                                              // 4 additional bytes of data
            iSendArg = 0;
    #if defined _HC_SD_CARD
            ucArguments[0] = 0x40;                                       // high capacity
    #else
            ucArguments[0] = 0x00;                                       // not high capacity
    #endif
            ucArguments[1] = 0;
            ucArguments[2] = 0;
            ucArguments[3] = 0;
        }
        break;
    case SD_CARD_RX_CRC_1:
        iSD_Card_State = SD_CARD_RX_CRC_2;
        break;
    case SD_CARD_RX_CRC_2:
        iSD_Card_State = SD_CARD_RX_CRC_3;
        break;
    case SD_CARD_RX_CRC_3:
        iSD_Card_State = SD_CARD_INIT;
        return 0x05;
    case SD_CARD_MULTI_BLOCK_WRITING:
        if (ucTxByte == 0xfe) {
            iMultiBlockWriteActive = 1;
            iSD_Card_State = SD_CARD_MULTI_BLOCK_COLLECTING;
        }
        break;
    case SD_CARD_BLOCK_WRITING:
        if (ucTxByte == 0xfe) {
            iSD_Card_State = SD_CARD_BLOCK_COLLECTING;
        }
        break;
    case SD_CARD_MULTI_BLOCK_COLLECTING:                                 // {5}
    case SD_CARD_BLOCK_COLLECTING:
        if (iArguments == 0) {                                           // write to a block starting
            if (ulPreDeleteLength > 1) {
                unsigned long ulDeleteOffset = ulWriteOffset;
                memset(ucArguments, 0x00, sizeof(ucArguments));          // deleted content
                while (ulPreDeleteLength > 1) {                          // pre-delete length is selected
                    // Normally the write to a sector causes the sector to be deleted as part of the process - if however a pre-delete has been commanded
                    // prior to sending the sector write a number of contiguous sectors are deleted, which results in a speed improvement.
                    // Here we delete these following contiguous sectors to ensure that a back pre-write value, which causes  data to be deleted can be detected
                    //
                    ulDeleteOffset += OFFSET_CONVERSION;
                    fnWriteSDSector((ulDeleteOffset/OFFSET_CONVERSION), 0xff, &ucArguments[0]);
                    ulPreDeleteLength--;
                }
            }
        }
        ucArguments[iArguments++] = ucTxByte;                            // collect the sector data
        if (iArguments >= 512) {
            fnWriteSDSector((ulWriteOffset/OFFSET_CONVERSION), 0xff, &ucArguments[0]); // write the complete sector data content
            iSD_Card_State = SD_CARD_RX_CRC_1;
            if (iMultiBlockWriteActive != 0) {
                iArguments = 0;
                ulWriteOffset += OFFSET_CONVERSION;                      // auto-increment to next sector
            }
        }
        break;
    case SD_CARD_MULTI_BLOCK_WRITE:
        iSD_Card_State = SD_CARD_MULTI_BLOCK_WRITING;
        return ucAnswer;
    case SD_CARD_BLOCK_WRITE:
        iSD_Card_State = SD_CARD_BLOCK_WRITING;
        return ucAnswer;
    case SD_CARD_ANSWER_APP:                                             // {5}
        iSD_Card_State = SD_CARD_APP_CMD;
        return ucAnswer;
    case SD_CARD_ANSWER:
        if (iArguments == 0) {
            iSD_Card_State = SD_CARD_INIT;
        }
        else {
            iSD_Card_State = SD_CARD_ANSWER_EXTRA;
        }
        return ucAnswer;
    case SD_CARD_ANSWER_EXTRA:
        {
            unsigned char ucReturnData = ucArguments[iSendArg++];
            if (iSendArg >= iArguments) {
                iSD_Card_State = SD_CARD_INIT;
            }
            return ucReturnData;
        }
    }
    return 0xff;
}
    #endif
    #if defined USB_MSD_HOST && defined USB_MSD_HOST
// Read a sector from a memory stick
//
extern int fnGetSimDiskData(unsigned char *ptrBuffer, unsigned char ucLUN, unsigned long ulLBA) // {6}
{
    _fnSimSD_findSector(ulLBA, ucLUN, ptrBuffer);
    return 0;
}

// Write a sector on a memory stick
//
extern int fnPutSimDiskData(unsigned char *ptrBuffer, unsigned char ucLUN, unsigned long ulLBA) // {6}
{
    fnWriteSDSector(ulLBA, ucLUN, ptrBuffer);
    return 0;
}
    #endif

#endif
