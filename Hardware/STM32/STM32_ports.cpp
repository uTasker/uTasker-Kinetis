/**********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET

    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland

    www.uTasker.com    Skype: M_J_Butcher

    ---------------------------------------------------------------------
    File:      STM32_ports.c
    Project:   Single Chip Embedded Internet
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2019
    *********************************************************************
    09.09.2012 Add additional functions                                  {1}
    18.10.2012 Increase buffer protection size (for VS2008+)             {2}
    22.08.2015 Add _STM32F7XX                                            {3}

*/

#include "config.h"

#ifdef _STM32

#define BUF1SIZE 200                                                     // {2}

#if _VC80_UPGRADE>=0x0600
    #define STRCPY(a, b) strcpy_s (a, BUF1SIZE, b)
    #define SPRINTF(a, b, c) sprintf_s(a, BUF1SIZE, b, c)
    #define STRCAT(a, b) strcat_s(a, BUF1SIZE, b)
#else
    #define STRCPY strcpy
    #define SPRINTF sprintf
    #define STRCAT strcat
#endif

#if defined _STM32L432 || defined _STM32L496
    #include "STM32L4XX_port.h"
#elif defined _STM32F031
    #include "STM32F031_port.h"
#elif defined _STM32L031 || defined _STM32L011
    #include "STM32L031_port.h"
#elif defined _STM32F7XX
    #include "STM32F7XX_port.h"
#elif defined _STM32F427 || defined _STM32F429
    #include "STM32F427_429_port.h"
#elif defined _STM32F2XX || defined _STM32F4XX
    #include "STM32F41X_port.h"
#elif defined _STM32F107X || defined _STM32F105X
    #include "STM32F105X_107X_port.h"
#elif defined _STM32F103X
    #include "STM32F103X_port.h"
#elif defined _STM32F100X 
    #include "STM32F100X_port.h"
#endif

#if defined _STM32F2XX || defined _STM32F4XX || defined _STM32F7XX
    #define _ALTERNATIVE_FUNCTIONS  (ALTERNATIVE_FUNCTIONS + 1)          // {1}
#else
    #define _ALTERNATIVE_FUNCTIONS  (ALTERNATIVE_FUNCTIONS)
#endif



static unsigned char *_ptrPerFunctions;

extern void fnSetPortDetails(char *cPortDetails, int iPort, int iBit, unsigned long *ulPortStates, unsigned long *ulPortFunction, unsigned long *ulPortPeripheral, int iMaxLength)
{
    char *ptrBuf = cPortDetails;
    int i;
    int iLength = 0;
    int iPortLength = PORT_WIDTH;
    char cBuf[BUF1SIZE + 1];

    switch (iPort) {
    case _PORTA:
        STRCPY(cPortDetails, "GPIO A");
        break;
    case _PORTB:
        STRCPY(cPortDetails, "GPIO B");
        break;
    case _PORTC:
        STRCPY(cPortDetails, "GPIO C");
        break;
    case _PORTD:
        STRCPY(cPortDetails, "GPIO D");
        break;
    case _PORTE:
        STRCPY(cPortDetails, "GPIO E");
        break;
    case _PORTF:
        STRCPY(cPortDetails, "GPIO F");
        break;
    case _PORTG:
        STRCPY(cPortDetails, "GPIO G");
        break;
    case _PORTH:
        STRCPY(cPortDetails, "GPIO H");
        break;
    case _PORTI:
        STRCPY(cPortDetails, "GPIO I");
        break;
    case _PORTJ:
        STRCPY(cPortDetails, "GPIO J");
        break;
    case _PORTK:
        STRCPY(cPortDetails, "GPIO K");
        break;
    default:
        STRCPY(cPortDetails, "?????");
        return;
    }

    iBit = (iPortLength - iBit - 1);                                     // bit position
    if (iBit < 0) {
        return;
    }
    SPRINTF(cBuf, " Bit %i Pin: ", iBit);
    STRCAT(cPortDetails, cBuf);
    if (*cPinNumber[iPort][iBit][_PIN_COUNT] == '-') {
        return;
    }
    STRCPY(cBuf, cPinNumber[iPort][iBit][_PIN_COUNT]);
    STRCAT(cPortDetails, cBuf);
    STRCAT(cPortDetails, " {");
    STRCAT(cPortDetails, cPer[iPort][iBit][0]);

    for (i = 1; i <= _ALTERNATIVE_FUNCTIONS; i++) {
        if (*cPer[iPort][iBit][i] != '-') {
            STRCAT(cPortDetails, "/");
            STRCAT(cPortDetails, cPer[iPort][iBit][i]);
        }
    }
    STRCAT(cPortDetails, "} ");

    if ((ulPortPeripheral[iPort] & (0x01 << iBit)) != 0) {               // if peripheral function selected
        unsigned char *ptrList = _ptrPerFunctions;
        int _iPort = iPort;
        int _iBit = iBit;
        while (_iPort-- != 0) {
            ptrList += (PORT_WIDTH);
        }
        while (_iBit-- != 0) {
            ptrList++;
        }
        if (*ptrList > _ALTERNATIVE_FUNCTIONS) {
            STRCPY(&cBuf[0], "??");
        }
        else {
            if (*cPer[iPort][iBit][*ptrList + 1] == '-') {
                STRCPY(&cBuf[0], "INVALID");
            }
            else {
                STRCPY(&cBuf[0], cPer[iPort][iBit][*ptrList + 1]);
            }
        }
        STRCAT(cPortDetails, cBuf);
    }
    else {
        if ((ulPortFunction[iPort] & (0x01 << iBit)) != 0) {
            STRCAT(cPortDetails, " Output");
        }
        else {
            STRCAT(cPortDetails, " Input");
        }
    }
}

extern unsigned long fnGetPortMask(int iPortNumber)
{
    unsigned long ulPortMask = 0x00000000;
    unsigned long ulMaskBit = 0x00000001;
    int i = 0;
    for (i = 0; i < 16; i++) {
        if (*cPinNumber[iPortNumber][i][_PIN_COUNT] == '-') {
            ulPortMask |= ulMaskBit;
        }
        ulMaskBit <<= 1;
    }
    return ulPortMask;
}

// Allow the hardware simulator to pass a pointer to hardware details which are useful for simulating this type
//
extern "C" void fnEnterHW_table(void *hw_table)
{
    _ptrPerFunctions = (unsigned char *)hw_table;
}
#endif