/**********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher

    ---------------------------------------------------------------------
    File:      FileToDisk.c
    Project:   Single Chip Embedded Internet
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2016
    *********************************************************************
    18.01.2007 Correct file sharing flag (_SH_DENYNO) for more recent VisualStudio versions  {1}
    11.05.2007 Add SAVE_COMPLETE_FLASH support (mainly for devices with multiple FLASH banks){2}
    11.08.2007 Add SPI FLASH support                                     {3}
    15.09.2007 Add Luminary LM3SXXXX support                             {4}
    17.11.2007 Allow SPI and FLASH files system to work together         {5}
    15.02.2008 Support for SPI FLASH STM25PXXX                           {6}
    28.06.2008 Support for SPI FLASH SST25                               {7}
    27.01.2009 Add save and restore of NVRAM content                     {8}
    26.04.2009 Add AVR32                                                 {9}
    03.11.2009 Add save and restore of battery backed up content         {10}
    14.11.2009 Enable FLASH operations when USE_PARAMETER_AREA is used   {11}
    14.01.2010 Add LPC17XX                                               {12}
    30.01.2010 Add Flexis32                                              {13}
    19.06.2010 Add STM32                                                 {14}
    18.09.2010 Add optional call on exit to allow users to save data to disk {15}
    29.11.2010 Add RX6XX                                                 {16}
    22.12.2010 Add Kinetis                                               {17}
    27.08.2011 Add I2C_EEPROM_FILE_SYSTEM support                        {18}
    02.10.2011 Add EXT_FLASH_FILE_SYSTEM support                         {19}
    13.11.2011 Add SAM3                                                  {20}
    06.08.2013 Enable FLASH operations when USE_PARAMETER_BLOCK is used  {21}
    28.04.2014 Allow FLASH_ROUTINES to enable flash simulation           {22}

*/


#include "conio.h"
#include "Fcntl.h"
#include "io.h"

#include "config.h"

#define BUF1SIZE 100
#include <sys/stat.h>

#if _VC80_UPGRADE >= 0x0600
    #include <share.h>
    #define STRCPY(a, b) strcpy_s (a, BUF1SIZE, b)
    #define SPRINTF(a, b) sprintf_s(a, BUF1SIZE, b)
    #define SPRINTF1(a, b, c) sprintf_s(a, BUF1SIZE, b, c)
    #define STRCAT(a, b) strcat_s(a, BUF1SIZE, b)
#else
    #define STRCPY strcpy
    #define SPRINTF sprintf
    #define SPRINTF1 sprintf
    #define STRCAT strcat
#endif

#ifdef _HW_NE64
    #if defined SPI_FILE_SYSTEM && !defined FLASH_FILE_SYSTEM            // {5}
       #define FLASH_FILE  "M95XXX_NE64.ini"
    #else
       #define FLASH_FILE  "FLASH_NE64.ini"
    #endif
#endif
#ifdef _HW_SAM7X
    #if defined SPI_FILE_SYSTEM && !defined FLASH_FILE_SYSTEM            // {5}
        #define FLASH_FILE  "M95XXX_SAM7X.ini"
    #else
        #define FLASH_FILE  "FLASH_SAM7X.ini"
    #endif
#endif
#ifdef _HW_SAM3X                                                        // {20}
    #if defined SPI_FILE_SYSTEM && !defined FLASH_FILE_SYSTEM
        #define FLASH_FILE  "M95XXX_SAM3.ini"
    #else
        #define FLASH_FILE  "FLASH_SAM3.ini"
    #endif
#endif
#ifdef _HW_AVR32                                                         // {9}
    #if defined SPI_FILE_SYSTEM && !defined FLASH_FILE_SYSTEM
        #define FLASH_FILE  "M95XXX_AVR32.ini"
    #else
        #define FLASH_FILE  "FLASH_AVR32.ini"
    #endif
#endif
#if defined _M5223X || defined _FLEXIS32
    #if defined SPI_FILE_SYSTEM && !defined FLASH_FILE_SYSTEM            // {5}
        #define FLASH_FILE  "M95XXX_M5223X.ini"
    #else
        #define FLASH_FILE  "FLASH_M5223X.ini"
    #endif
#endif
#ifdef _STR91XF
    #if defined SPI_FILE_SYSTEM && !defined FLASH_FILE_SYSTEM            // {5}
        #define FLASH_FILE  "M95XXX_STR91XF.ini"
    #else
        #define FLASH_FILE  "FLASH_STR91XF.ini"
    #endif
#endif
#ifdef _LPC23XX
    #if defined SPI_FILE_SYSTEM && !defined FLASH_FILE_SYSTEM            // {5}
        #define FLASH_FILE  "M95XXX_LPC23XX.ini"
    #else
        #define FLASH_FILE  "FLASH_LPC23XX.ini"
    #endif
#endif
#ifdef _LPC17XX                                                          // {12}
    #define FLASH_FILE  "FLASH_LPC17XX.ini"
#endif
#ifdef _KINETIS                                                          // {17}
    #define FLASH_FILE  "FLASH_KINETIS.ini"
#endif
#ifdef _LM3SXXXX                                                         // {4}
    #if defined SPI_FILE_SYSTEM && !defined FLASH_FILE_SYSTEM            // {5}
        #define FLASH_FILE  "M95XXX_LM3SXXXX.ini"
    #else
        #define FLASH_FILE  "FLASH_LM3SXXXX.ini"
    #endif
#endif
#ifdef _STM32                                                            // {14}
    #if defined SPI_FILE_SYSTEM && !defined FLASH_FILE_SYSTEM
        #define FLASH_FILE  "M95XXX_STM32.ini"
    #else
        #define FLASH_FILE  "FLASH_STM32.ini"
    #endif
#endif
#ifdef _RX6XX                                                            // {16}
    #define FLASH_FILE  "FLASH_RX6XX.ini"
#endif

#if defined SPI_SW_UPLOAD || defined SPI_FLASH_FAT || (defined SPI_FILE_SYSTEM && defined FLASH_FILE_SYSTEM) // {5}
    #if defined SPI_FLASH_ST
        #define SPI_FLASH_FILE "STM25PXXX.ini"                           // {6}
    #elif defined SPI_FLASH_SST25
        #define SPI_FLASH_FILE "SST25XXXX.ini"                           // {7}
    #elif defined SPI_FLASH_S25FL1_K
        #define SPI_FLASH_FILE "S25FL1-K.ini"
    #elif defined SPI_FLASH_W25Q
        #define SPI_FLASH_FILE "W25QXXX.ini"
    #else
        #define SPI_FLASH_FILE "AT45DBXXX.ini"                           // {3}
    #endif
#endif
#if defined I2C_EEPROM_FILE_SYSTEM                                       // {18}
    #define I2C_EEPROM_FILE "M24M01.ini"
#endif
#if defined EXT_FLASH_FILE_SYSTEM                                        // {19}
    #define EXT_FLASH_FILE "ext_flash.ini"
#endif
#if defined NVRAM
    #define NVRAM_FILE "nvram.ini"                                       // {8}
#endif
#if defined BATTERY_BACKED_RAM
    #define BAT_BACK_FILE "batram.ini"                                   // {10}
#endif


static signed char *fnGetFileName(signed char *ptrPath)
{
    signed char *prtFileName = 0;

    while (*ptrPath) {
        if (*ptrPath++ == '\\') {
            prtFileName = ptrPath;
        }
    }
    return prtFileName;
}

// Read in a FLASH.ini file to get the present FLASH contents, including present parameters
//
extern void fnPrimeFileSystem(void)
{
#if defined FLASH_ROUTINES || defined ACTIVE_FILE_SYSTEM || defined USE_PARAMETER_BLOCK || defined USE_PARAMETER_AREA || defined SPI_SW_UPLOAD || defined SPI_FLASH_FAT || (defined SPI_FILE_SYSTEM && defined FLASH_FILE_SYSTEM)
    int iFileIni;
#endif
#if defined FLASH_ROUTINES || defined ACTIVE_FILE_SYSTEM || defined USE_PARAMETER_BLOCK || defined USE_PARAMETER_AREA // {11}{21}{22}
    unsigned char *ptrMem = fnGetFileSystemStart(0);
    #if _VC80_UPGRADE < 0x0600
	iFileIni = _open(FLASH_FILE, (_O_BINARY | _O_RDWR));
    #else
	_sopen_s(&iFileIni, FLASH_FILE, (_O_BINARY | _O_RDWR), _SH_DENYWR, _S_IREAD);
    #endif
    if (iFileIni >= 0) {
    #if defined SAVE_COMPLETE_FLASH                                      // {2}
		_read(iFileIni, ptrMem, fnGetFlashSize(0));
    #else
        #if (defined SPI_FILE_SYSTEM && defined FLASH_FILE_SYSTEM)       // {5}
        unsigned long ulSize = (FLASH_START_ADDRESS + SIZE_OF_FLASH);
        ulSize -= (unsigned long)uFILE_START;
        _read(iFileIni, ptrMem, ulSize);                                 // only the part of the file system in FLASH
        #else
		_read(iFileIni, ptrMem, FILE_SYSTEM_SIZE);
        #endif
    #endif
	    _close(iFileIni);        
    }
    #if defined SPI_FILE_SYSTEM && !defined FLASH_FILE_SYSTEM            // {5}
    else {
        fnInitSPI_Flash();                                               // set blank SPI file system
    }
    #endif
#endif
#if defined SPI_SW_UPLOAD || defined SPI_FLASH_FAT || (defined SPI_FILE_SYSTEM && defined FLASH_FILE_SYSTEM) // {3}{5}
    #if _VC80_UPGRADE < 0x0600
	iFileIni = _open(SPI_FLASH_FILE, (_O_BINARY | _O_RDWR));
    #else
	_sopen_s(&iFileIni, SPI_FLASH_FILE, (_O_BINARY | _O_RDWR), _SH_DENYWR, _S_IREAD);
    #endif

    if (iFileIni >= 0) {
		_read(iFileIni, fnGetDataFlashStart(), fnGetDataFlashSize());
	    _close(iFileIni);   
    }
    else {
        fnInitSPI_DataFlash();                                           // set blank SPI file system
    }
#endif
#if defined I2C_EEPROM_FILE_SYSTEM                                       // {18}
    #if _VC80_UPGRADE < 0x0600
	iFileIni = _open(I2C_EEPROM_FILE, (_O_BINARY | _O_RDWR));
    #else
	_sopen_s(&iFileIni, I2C_EEPROM_FILE, (_O_BINARY | _O_RDWR), _SH_DENYWR, _S_IREAD);
    #endif

    if (iFileIni >= 0) {
		_read(iFileIni, fnGetI2CEEPROMStart(), fnGetI2CEEPROMSize());
	    _close(iFileIni);   
    }
    else {
        fnInitI2C_EEPROM();                                              // set blank I2C EEPROM
    }
#endif
#if defined EXT_FLASH_FILE_SYSTEM                                        // {19}
    #if _VC80_UPGRADE < 0x0600
	iFileIni = _open(EXT_FLASH_FILE, (_O_BINARY | _O_RDWR));
    #else
	_sopen_s(&iFileIni, EXT_FLASH_FILE, (_O_BINARY | _O_RDWR), _SH_DENYWR, _S_IREAD);
    #endif

    if (iFileIni >= 0) {
		_read(iFileIni, fnGetExtFlashStart(), fnGetExtFlashSize());
	    _close(iFileIni);   
    }
    else {
        fnInitExtFlash();                                                // set blank external parallel flash
    }
#endif
#if defined NVRAM                                                        // {8}
    #if _VC80_UPGRADE < 0x0600
	iFileIni = _open(NVRAM_FILE, (_O_BINARY | _O_RDWR));
    #else
	_sopen_s(&iFileIni, NVRAM_FILE, (_O_BINARY | _O_RDWR), _SH_DENYWR, _S_IREAD);
    #endif

    if (iFileIni >= 0) {
		_read(iFileIni, fnGetFileSystemStart(1), fnGetFlashSize(1));
	    _close(iFileIni);   
    }
#endif
#ifdef BATTERY_BACKED_RAM                                                // {10}
    #if _VC80_UPGRADE < 0x0600
	iFileIni = _open(BAT_BACK_FILE, (_O_BINARY | _O_RDWR));
    #else
	_sopen_s(&iFileIni, BAT_BACK_FILE, (_O_BINARY | _O_RDWR), _SH_DENYWR, _S_IREAD);
    #endif

    if (iFileIni >= 0) {
        unsigned long ulDataLength = 0;
        unsigned char ucData;
        while (_read(iFileIni, &ucData, 1)) {
            if (fnPutBatteryRAMContent(ucData, ulDataLength++) == 0) {
                break;
            }
        }  
	    _close(iFileIni);   
    }
#endif
}



extern void fnSaveFlashToFile(void)
{
#if defined FLASH_ROUTINES || defined ACTIVE_FILE_SYSTEM || defined USE_PARAMETER_BLOCK || defined USE_PARAMETER_AREA || defined SPI_SW_UPLOAD || defined SPI_FLASH_FAT || (defined SPI_FILE_SYSTEM && defined FLASH_FILE_SYSTEM)
    int iFileIni;
#endif
#if defined FLASH_ROUTINES || defined ACTIVE_FILE_SYSTEM || defined USE_PARAMETER_BLOCK || defined USE_PARAMETER_AREA // {11}{21}{22}
    #if _VC80_UPGRADE < 0x0600
	iFileIni = _open(FLASH_FILE, (_O_BINARY | _O_TRUNC  | _O_CREAT | _O_RDWR ), _S_IREAD | _S_IWRITE );
    #else
	_sopen_s(&iFileIni, FLASH_FILE, (_O_BINARY |  _O_TRUNC  | _O_CREAT | _O_RDWR), _SH_DENYNO, _S_IREAD | _S_IWRITE); // {1}
    #endif
    if (iFileIni >= 0) {
    #if defined SAVE_COMPLETE_FLASH                                      // {2}
        _write(iFileIni, fnGetFileSystemStart(0), fnGetFlashSize(0));
    #else
        #if (defined SPI_FILE_SYSTEM && defined FLASH_FILE_SYSTEM)
        unsigned long ulSize = (FLASH_START_ADDRESS + SIZE_OF_FLASH);
        ulSize -= (unsigned long)uFILE_START;
        _write(iFileIni, fnGetFileSystemStart(0), ulSize);               // save only the part of the file system in FLASH
        #else
        _write(iFileIni, fnGetFileSystemStart(0), FILE_SYSTEM_SIZE);     // save the file system only
        #endif
    #endif
	    _close(iFileIni);        
    }
#endif
#if defined SPI_SW_UPLOAD || defined SPI_FLASH_FAT || (defined SPI_FILE_SYSTEM && defined FLASH_FILE_SYSTEM) // {3}
    #if _VC80_UPGRADE < 0x0600
	iFileIni = _open(SPI_FLASH_FILE, (_O_BINARY | _O_TRUNC  | _O_CREAT | _O_RDWR ), _S_IREAD | _S_IWRITE );
    #else
	_sopen_s(&iFileIni, SPI_FLASH_FILE, (_O_BINARY |  _O_TRUNC  | _O_CREAT | _O_RDWR), _SH_DENYNO, _S_IREAD | _S_IWRITE); // {1}
    #endif

    if (iFileIni >= 0) {
        _write(iFileIni, fnGetDataFlashStart(), fnGetDataFlashSize());   // save data flash
	    _close(iFileIni);        
    }
#endif
#if defined I2C_EEPROM_FILE_SYSTEM                                       // {18}
    #if _VC80_UPGRADE < 0x0600
	iFileIni = _open(I2C_EEPROM_FILE, (_O_BINARY | _O_TRUNC  | _O_CREAT | _O_RDWR ), _S_IREAD | _S_IWRITE );
    #else
	_sopen_s(&iFileIni, I2C_EEPROM_FILE, (_O_BINARY |  _O_TRUNC  | _O_CREAT | _O_RDWR), _SH_DENYNO, _S_IREAD | _S_IWRITE); // {1}
    #endif

    if (iFileIni >= 0) {
        _write(iFileIni, fnGetI2CEEPROMStart(), fnGetI2CEEPROMSize());   // save I2C EEPROM
	    _close(iFileIni);        
    }
#endif
#if defined EXT_FLASH_FILE_SYSTEM                                        // {19}
    #if _VC80_UPGRADE < 0x0600
	iFileIni = _open(EXT_FLASH_FILE, (_O_BINARY | _O_TRUNC  | _O_CREAT | _O_RDWR ), _S_IREAD | _S_IWRITE );
    #else
	_sopen_s(&iFileIni, EXT_FLASH_FILE, (_O_BINARY |  _O_TRUNC  | _O_CREAT | _O_RDWR), _SH_DENYNO, _S_IREAD | _S_IWRITE); // {1}
    #endif

    if (iFileIni >= 0) {
        _write(iFileIni, fnGetExtFlashStart(), fnGetExtFlashSize());     // save external parallel flash content
	    _close(iFileIni);        
    }
#endif
#if defined NVRAM                                                        // {8}
    #if _VC80_UPGRADE < 0x0600
	iFileIni = _open(NVRAM_FILE, (_O_BINARY | _O_TRUNC  | _O_CREAT | _O_RDWR ), _S_IREAD | _S_IWRITE );
    #else
	_sopen_s(&iFileIni, NVRAM_FILE, (_O_BINARY |  _O_TRUNC  | _O_CREAT | _O_RDWR), _SH_DENYNO, _S_IREAD | _S_IWRITE);
    #endif

    if (iFileIni >= 0) {
		_write(iFileIni, fnGetFileSystemStart(1), fnGetFlashSize(1));
	    _close(iFileIni);   
    }
#endif
#if defined BATTERY_BACKED_RAM                                           // {10}
    #if _VC80_UPGRADE < 0x0600
	iFileIni = _open(BAT_BACK_FILE, (_O_BINARY | _O_TRUNC  | _O_CREAT | _O_RDWR ), _S_IREAD | _S_IWRITE );
    #else
	_sopen_s(&iFileIni, BAT_BACK_FILE, (_O_BINARY |  _O_TRUNC  | _O_CREAT | _O_RDWR), _SH_DENYNO, _S_IREAD | _S_IWRITE);
    #endif

    if (iFileIni >= 0) {
        unsigned long ulDataLength = 0;
        unsigned char ucData;
        while (fnGetBatteryRAMContent(&ucData, ulDataLength++) != 0) {
		    _write(iFileIni, &ucData, 1);                                // write 1 byte at a time since the data may not be from a single location
        }       
	    _close(iFileIni);
    }
#endif
#ifdef _USER_MEMORY_SAVE                                                 // {15}
    fnSaveUserData();
#endif
}

extern void fnSetProjectDetails(signed char **ProjectDisplay)
{
    char buf[BUF1SIZE + 2];
    unsigned long ulFrequency = PLL_OUTPUT_FREQ;
    unsigned long ulMHz = PLL_OUTPUT_FREQ/1000000;
    unsigned long ulRemainder = ulFrequency - (ulMHz * 1000000);
    int iDecimalPlaces = 4;
    int iDiv = 100000;

    STRCPY(*ProjectDisplay, MY_PROJECT_NAME);
    buf[0] = ' ';
    buf[1] = '[';
    SPRINTF(&buf[2], TARGET_HW);
    STRCAT(*ProjectDisplay, buf);
    SPRINTF1(buf, " : %u.", ulMHz);
    STRCAT(*ProjectDisplay, buf);
    do {
        SPRINTF1(buf, "%u", ulRemainder/iDiv);
        STRCAT(*ProjectDisplay, buf);
        ulRemainder = ulRemainder - (iDiv * (ulRemainder/iDiv));
        iDiv/= 10;
    } while (ulRemainder && iDecimalPlaces--);
    SPRINTF(buf, "MHz]");
    STRCAT(*ProjectDisplay, buf);
}

